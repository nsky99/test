#ifndef _CPE_H_
#define _CPE_H_
#include <Windows.h>

PIMAGE_DOS_HEADER GetDosHeader();

PIMAGE_NT_HEADERS GetNTHeaders();

bool IsPE(unsigned char* pbuff);

bool InitPE(TCHAR* path);

DWORD RvaToFoa(DWORD dwRva);

// ��ȡ�����Ŀ¼
PIMAGE_EXPORT_DIRECTORY GetExportDirectory();

// ��ʾ������
void ShowExportInfo();

// ��ȡ������Ŀ¼
PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor();

// ��ʾ������
void ShowImportInfo();

#endif // !_CPE_H_
