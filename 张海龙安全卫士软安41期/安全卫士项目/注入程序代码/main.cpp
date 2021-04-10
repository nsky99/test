#include <stdio.h>
#include <string>
#include "RemoteThreadInject.h"
using std::string;
int main(int argc,char* argv[])
{
	// 如果参数 == 1没有传入参数
	if (argc == 1)
	{
		MessageBoxW(0, L"参数不正确!", L"Error", MB_ICONERROR);
		return 0;
	}

	DWORD dwTaskmgrPid = 0;
	DWORD dwProtectPid = 0;
	char  strDllPath[MAX_PATH];
	memset(strDllPath, 0, MAX_PATH);
	//argv[1] 为一个参数：任务管理器进程ID
	sscanf_s(argv[1], "%d", &dwTaskmgrPid);
	//argv[2] 为二个参数：被保护的进程ID
	sscanf_s(argv[2], "%d", &dwProtectPid); 
	//argv[3] 为三个参数：用于Hook的Dll
	memcpy(strDllPath, argv[3], strlen(argv[3]) + 1);
	// 创建内核对象：物理页								创建物理页，如果这个名字的内核对象存在了就直接返回句柄																
	HANDLE hMapFile = CreateFileMapping(0, 0, PAGE_READWRITE, 0, 0x1000, L"要保护的进程ID");

	// 将物理页和线性地址(虚拟内存)进行映射从哪里开始映射																								
	LPSTR lpBuf = (LPSTR)MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0x1000);
	// 向物理页中写内容					
	*PDWORD(lpBuf) = dwProtectPid;

	Inject(dwTaskmgrPid, strDllPath);

	// 关闭映射 断开物理页和虚拟内存的映射												
	UnmapViewOfFile(lpBuf);

	// 关闭内核句柄												
	CloseHandle(hMapFile);

	return 0;
}