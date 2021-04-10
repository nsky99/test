#include <ntifs.h>
#include "ArkReg.h"
#include "ArkCommon.h"

//*********************************************************************************
// 遍历注册表
// InputBuff: regPath
// OutputBuff:regInfo
//*********************************************************************************
SIZE_T GetRegList(PVOID InputBuff, PVOID OutputBuff)
{
	//0.定义变量
	HANDLE hKey;
	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING strPath  = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE");
	//RtlInitUnicodeString(&strPath, (PCWSTR)InputBuff);
	NTSTATUS nRet;

	//1.打开key
	InitializeObjectAttributes(&oa, &strPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	nRet = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &oa);
	if (!NT_SUCCESS(nRet))
	{
		DbgPrint("Key打开失败！\n");
		return 0;
	}

	//2.查询子key的索引
	PKEY_FULL_INFORMATION p_kfi = (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, sizeof(KEY_FULL_INFORMATION));
	ULONG uRealSize;
	//分配尺寸需要尽量大一点，否则会出错
	nRet = ZwQueryKey(hKey, KeyFullInformation, p_kfi, 1024, &uRealSize);
	if (!NT_SUCCESS(nRet))
	{
		DbgPrint("Key查询失败！\n");
		ExFreePool(p_kfi);
		ZwClose(hKey);
		return 0;
	}

	//2.初始化内存空间
	UINT32 index = 0;
	PREGINF pRinfo = (PREGINF)OutputBuff;

	//3.遍历获取子Key信息
	PKEY_BASIC_INFORMATION p_kbi = (PKEY_BASIC_INFORMATION)ExAllocatePool(PagedPool, 1024);
	for (ULONG i = 0; i < p_kfi->SubKeys; i++, index++)
	{
		//每次记得情况内存，不然会出错
		RtlZeroMemory(p_kbi, 1024);
		nRet = ZwEnumerateKey(hKey,
			i,							   //子项的索引号，必须要有，所以只能查询
			KeyBasicInformation,		   //获取什么样的结构信息
			p_kbi,						   //信息放在哪里？
			1024, //信息获取的尺寸
			&uRealSize);				   //真实获取到的尺寸
		if (NT_SUCCESS(nRet))
		{
			pRinfo[index].RegType = 1;
			RtlCopyMemory(pRinfo[index].KeyName, p_kbi->Name, p_kbi->NameLength);
			KdPrint(("%ws\n", p_kbi->Name));
		}
	}

	//4.查询当前项的值
	PKEY_VALUE_FULL_INFORMATION pkvfi = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(PagedPool, 2000);
	for (ULONG i = 0; i < p_kfi->Values; i++, index++)
	{
		RtlZeroMemory(pkvfi, 1024);
		nRet = ZwEnumerateValueKey(hKey, i, KeyValueFullInformation, pkvfi, 2000, &uRealSize);
		if (NT_SUCCESS(nRet))
		{
			pRinfo[index].RegType = 2;
			RtlCopyMemory(pRinfo[index].ValueName, pkvfi->Name, pkvfi->NameLength);
			pRinfo[index].ValueType = pkvfi->Type;
			RtlCopyMemory(pRinfo[index].Value, (PVOID)((ULONG)pkvfi + pkvfi->DataOffset), pkvfi->DataLength);
			KdPrint(("%ws", pRinfo->ValueName));
		}
	}

	if (!p_kfi)ExFreePool(p_kfi);
	if (!p_kbi)ExFreePool(p_kbi);
	if (!pkvfi)ExFreePool(pkvfi);
	ZwClose(hKey);

	return (index + 1) * sizeof(REGINF);
}

//*********************************************************************************
// 删除指定注册表子项
//*********************************************************************************
NTSTATUS DeleteReg(PVOID InputBuff)
{
	HANDLE hKey;
	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING strPath;
	RtlInitUnicodeString(&strPath, (PCWSTR)InputBuff);
	NTSTATUS nRet;

	InitializeObjectAttributes(&oa, &strPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	nRet = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &oa);
	if (!NT_SUCCESS(nRet))
	{
		DbgPrint("Key打开失败！\n");
		return nRet;
	}

	nRet = ZwDeleteKey(hKey);
	if (!NT_SUCCESS(nRet))
	{
		DbgPrint("Key删除失败！\n");
	}
	ZwClose(hKey);
	return nRet;
}

//*********************************************************************************
// 创建指定注册表子项
//*********************************************************************************
NTSTATUS CreateReg(PVOID InputBuff)
{
	HANDLE hKey;
	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING strPath;
	RtlInitUnicodeString(&strPath, (PCWSTR)InputBuff);
	ULONG uDisp;
	NTSTATUS nRet;

	InitializeObjectAttributes(&oa, &strPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	nRet = ZwCreateKey(&hKey, KEY_ALL_ACCESS, &oa, 0, NULL,
		REG_OPTION_NON_VOLATILE,//创建在内存中，还是创建在磁盘上
		&uDisp);	//返回当前是：创建还是打开
	if (!NT_SUCCESS(nRet))
	{
		DbgPrint("Key创建失败！\n");
		return nRet;
	}
	ZwClose(hKey);
	return nRet;
}


// 响应io 查询注册表信息
NTSTATUS OnArkQueryReg(PIRP pIrp)
{
	WCHAR szRegPath[0xff] = { 0 };
	RtlCopyMemory(szRegPath, pIrp->AssociatedIrp.SystemBuffer, 0xff);
	ULONG ulMemSize = GetRegList(szRegPath, NULL);
	PVOID pRegInfo = ExAllocatePool(PagedPool, ulMemSize);
	ulMemSize = GetRegList(szRegPath, pRegInfo);
	RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pRegInfo, ulMemSize);

	ExFreePool(pRegInfo);
	pRegInfo = NULL;
	pIrp->IoStatus.Information = ulMemSize;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	return STATUS_SUCCESS;
}

// 响应io 查询注册表数量
NTSTATUS OnArkCountReg(PIRP pIrp)
{
	WCHAR szRegPath[0xff] = { 0 };
	RtlCopyMemory(szRegPath, pIrp->AssociatedIrp.SystemBuffer, 0xff);
	ULONG ulMemSize = GetRegList(szRegPath, NULL)/sizeof(REGINF);
	RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, &ulMemSize, sizeof(REGINF));


	pIrp->IoStatus.Information = sizeof(ULONG);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	return STATUS_SUCCESS;
}
