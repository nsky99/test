
#include "RemoteThreadInject.h"

#include <string.h>

BOOL Inject(DWORD dwPid, string strDllPath)
{
  //打开目标进程
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (0 == hProcess)
	{
		MessageBox(0, L"打开目标进程失败!", L"Error", MB_ICONERROR);
		return FALSE;
	}

	//申请内存----在目标进程中
	LPVOID lpAddress =
		VirtualAllocEx(hProcess, NULL, strDllPath.length() + 1, MEM_COMMIT, PAGE_READWRITE);
	if (0 == lpAddress)
	{
		MessageBox(0, L"在目标进程中申请内存失败", L"Error", MB_ICONERROR);
		CloseHandle(hProcess);
		return FALSE;
	}

	//将DLL路径写到申请的内存中
	BOOL bRet 
		= WriteProcessMemory(hProcess, lpAddress, strDllPath.c_str(), strDllPath.length() + 1, 0);
	if (FALSE == bRet)
	{
		MessageBox(0, L"路径写入失败", L"Error", MB_ICONERROR);
		VirtualFreeEx(hProcess, lpAddress, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return FALSE;
	}

	//远程线程注入DLL
	HANDLE hThread =
		CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, lpAddress, 0, 0);
	if (0 == hThread)
	{
		MessageBox(0, L"创建远程线程失败", L"Error", MB_ICONERROR);
		VirtualFreeEx(hProcess, lpAddress, 0, MEM_RELEASE);
		CloseHandle(hProcess);
	}

	//等待线程结束
	WaitForSingleObject(hThread, -1);
	return 1;
}