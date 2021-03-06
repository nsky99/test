#include "CPE.h"
#include <stdio.h>
UCHAR* g_ImageBase = 0;

bool InitPE(TCHAR* path)
{
	HANDLE hFile = CreateFile(
		path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	int size = GetFileSize(hFile, 0);

	g_ImageBase = new unsigned char[size];
	DWORD dwReadSize;

	ReadFile(hFile, g_ImageBase, size, &dwReadSize, 0);
	return IsPE(g_ImageBase);
}

bool IsPE(unsigned char* pbuff)
{
	PIMAGE_DOS_HEADER pDos = GetDosHeader();
	if (pDos->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return false;
	}
	PIMAGE_NT_HEADERS pNt = GetNTHeaders();
	if (pNt->Signature != IMAGE_NT_SIGNATURE)
	{
		return false;
	}
	return true;
}

PIMAGE_DOS_HEADER GetDosHeader()
{
	return (PIMAGE_DOS_HEADER)g_ImageBase;
}

PIMAGE_NT_HEADERS GetNTHeaders()
{
	PIMAGE_DOS_HEADER pDos = GetDosHeader();
	PIMAGE_NT_HEADERS pNt =
		(PIMAGE_NT_HEADERS)(pDos->e_lfanew + (DWORD)g_ImageBase);
	return pNt;
}

DWORD RvaToFoa(DWORD dwRva)
{
	DWORD offset = 0;
	//1.判断这个RVA落在哪个区段
	PIMAGE_NT_HEADERS pNt = GetNTHeaders();
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);

	DWORD dwSectionCount = GetNTHeaders()->FileHeader.NumberOfSections;

	for (size_t i = 0; i < dwSectionCount; ++i)
	{
		if (dwRva >= pSection[i].VirtualAddress
			&& dwRva < (pSection[i].SizeOfRawData + pSection[i].VirtualAddress))
		{
			//2.遍历找到后根据公式计算
			offset = dwRva - pSection[i].VirtualAddress + pSection[i].PointerToRawData;
		}
	}
	return offset;
}

PIMAGE_EXPORT_DIRECTORY GetExportDirectory()
{
	DWORD ExportRva = GetNTHeaders()->OptionalHeader.DataDirectory[0].VirtualAddress;
	DWORD dwOffset = RvaToFoa(ExportRva);
	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)
		(dwOffset + (DWORD)g_ImageBase);
	return pExport;
}

void ShowExportInfo()
{
	if (GetNTHeaders()->OptionalHeader.DataDirectory[0].Size == 0)
	{
		printf("没有导出表信息\n");
		return;
	}
	PIMAGE_EXPORT_DIRECTORY pExport = GetExportDirectory();

	//dll名字
	char* pDllName = (char*)(RvaToFoa(pExport->Name) + (DWORD)g_ImageBase);
	printf("%s\n", pDllName);
	//导出地址表
	DWORD* pEAT = (DWORD*)(RvaToFoa(pExport->AddressOfFunctions) + (DWORD)g_ImageBase);
	//导出名称表
	DWORD* pENT = (DWORD*)(RvaToFoa(pExport->AddressOfNames) + (DWORD)g_ImageBase);
	//导出序号表,元素是WORD
	WORD* pEOT = (WORD*)(RvaToFoa(pExport->AddressOfNameOrdinals) + (DWORD)g_ImageBase);
	//导出地址表的个数
	DWORD dwAddrCount = pExport->NumberOfFunctions;
	//导出名称表的个数
	DWORD dwNameCount = pExport->NumberOfNames;

	//遍历地址表
	for (size_t i = 0; i < dwAddrCount; ++i)
	{
		printf("序号:%d\t", i + pExport->Base);
		//遍历名称表
		for (size_t j = 0; j < dwNameCount; ++j)
		{
			//地址表的序号==序号表的内容，说明这个函数有名字
			if (i == pEOT[j])
			{
				char* pFunName = (char*)
					(RvaToFoa(pENT[j]) + (DWORD)g_ImageBase);
				printf("%s", pFunName);
				break;
			}
		}
		printf("\n");
	}
}

PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor()
{
	PIMAGE_NT_HEADERS pNt = GetNTHeaders();
	DWORD ImportRva = pNt->OptionalHeader.DataDirectory[1].VirtualAddress;
	DWORD dwOffset = RvaToFoa(ImportRva);
	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)
		(dwOffset + (DWORD)g_ImageBase);
	return pImport;
}

void ShowImportInfo()
{
	PIMAGE_IMPORT_DESCRIPTOR pImport = GetImportDescriptor();
	//导入表结构体数组以0结尾
	while (pImport->Name)
	{
		char* pImportName = (char*)
			(RvaToFoa(pImport->Name) + (DWORD)g_ImageBase);
		printf("%s\n", pImportName);

		//获取IAT的地址
		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)
			(RvaToFoa(pImport->FirstThunk) + (DWORD)g_ImageBase);
		//遍历IAT中的函数
		while (pIAT->u1.Ordinal)
		{
			//判断是名称导入还是仅序号导入
			if (pIAT->u1.Ordinal & 0x80000000)//最高位是1，仅序号导入
			{
				printf("序号:%d\n", (int)(pIAT->u1.Function & 0x7FFFFFFFF));
			}
			else//最高位0，名称导入，前三个值都无效
			{
				//找到PIMAGE_IMPORT_BY_NAME结构体的地址
				PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)
					(RvaToFoa(pIAT->u1.AddressOfData) + (DWORD)g_ImageBase);
				printf("序号:%08d\t函数名:%s\n", pName->Hint, pName->Name);
			}
			pIAT++;
		}
		pImport++;
	}
}
