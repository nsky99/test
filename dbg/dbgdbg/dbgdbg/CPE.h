#ifndef _CPE_H_
#define _CPE_H_
#include <Windows.h>

PIMAGE_DOS_HEADER GetDosHeader();

PIMAGE_NT_HEADERS GetNTHeaders();

bool IsPE(unsigned char* pbuff);

bool InitPE(TCHAR* path);

DWORD RvaToFoa(DWORD dwRva);

// 获取导入表目录
PIMAGE_EXPORT_DIRECTORY GetExportDirectory();

// 显示导出表
void ShowExportInfo();

// 获取导出表目录
PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor();

// 显示导出表
void ShowImportInfo();

#endif // !_CPE_H_
