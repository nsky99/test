#include <ntifs.h>
#include "ArkReg.h"
#include "ArkCommon.h"

//*********************************************************************************
// ����ע���
// InputBuff: regPath
// OutputBuff:regInfo
//*********************************************************************************
SIZE_T GetRegList(PVOID InputBuff, PVOID OutputBuff)
{
	//0.�������
	HANDLE hKey;
	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING strPath  = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE");
	//RtlInitUnicodeString(&strPath, (PCWSTR)InputBuff);
	NTSTATUS nRet;

	//1.��key
	InitializeObjectAttributes(&oa, &strPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	nRet = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &oa);
	if (!NT_SUCCESS(nRet))
	{
		DbgPrint("Key��ʧ�ܣ�\n");
		return 0;
	}

	//2.��ѯ��key������
	PKEY_FULL_INFORMATION p_kfi = (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, sizeof(KEY_FULL_INFORMATION));
	ULONG uRealSize;
	//����ߴ���Ҫ������һ�㣬��������
	nRet = ZwQueryKey(hKey, KeyFullInformation, p_kfi, 1024, &uRealSize);
	if (!NT_SUCCESS(nRet))
	{
		DbgPrint("Key��ѯʧ�ܣ�\n");
		ExFreePool(p_kfi);
		ZwClose(hKey);
		return 0;
	}

	//2.��ʼ���ڴ�ռ�
	UINT32 index = 0;
	PREGINF pRinfo = (PREGINF)OutputBuff;

	//3.������ȡ��Key��Ϣ
	PKEY_BASIC_INFORMATION p_kbi = (PKEY_BASIC_INFORMATION)ExAllocatePool(PagedPool, 1024);
	for (ULONG i = 0; i < p_kfi->SubKeys; i++, index++)
	{
		//ÿ�μǵ�����ڴ棬��Ȼ�����
		RtlZeroMemory(p_kbi, 1024);
		nRet = ZwEnumerateKey(hKey,
			i,							   //����������ţ�����Ҫ�У�����ֻ�ܲ�ѯ
			KeyBasicInformation,		   //��ȡʲô���Ľṹ��Ϣ
			p_kbi,						   //��Ϣ�������
			1024, //��Ϣ��ȡ�ĳߴ�
			&uRealSize);				   //��ʵ��ȡ���ĳߴ�
		if (NT_SUCCESS(nRet))
		{
			pRinfo[index].RegType = 1;
			RtlCopyMemory(pRinfo[index].KeyName, p_kbi->Name, p_kbi->NameLength);
			KdPrint(("%ws\n", p_kbi->Name));
		}
	}

	//4.��ѯ��ǰ���ֵ
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
// ɾ��ָ��ע�������
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
		DbgPrint("Key��ʧ�ܣ�\n");
		return nRet;
	}

	nRet = ZwDeleteKey(hKey);
	if (!NT_SUCCESS(nRet))
	{
		DbgPrint("Keyɾ��ʧ�ܣ�\n");
	}
	ZwClose(hKey);
	return nRet;
}

//*********************************************************************************
// ����ָ��ע�������
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
		REG_OPTION_NON_VOLATILE,//�������ڴ��У����Ǵ����ڴ�����
		&uDisp);	//���ص�ǰ�ǣ��������Ǵ�
	if (!NT_SUCCESS(nRet))
	{
		DbgPrint("Key����ʧ�ܣ�\n");
		return nRet;
	}
	ZwClose(hKey);
	return nRet;
}


// ��Ӧio ��ѯע�����Ϣ
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

// ��Ӧio ��ѯע�������
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
