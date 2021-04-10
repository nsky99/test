#include "CHookTaskMgr.h"
#include <Windows.h>																																											


unsigned char g_NewCode[5] = { 0xE9 };
unsigned char g_OldCode[5] = {  };
DWORD dwProtectPid;

HANDLE
WINAPI
MyOpenProcess(
  _In_ DWORD dwDesiredAccess,
  _In_ BOOL bInheritHandle,
  _In_ DWORD dwProcessId
)
{
  if (dwProtectPid == dwProcessId)
    return 0;
  HANDLE hReturnCode = 0;
  UnInstallHook();
  hReturnCode = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
  InstallHook();
  return hReturnCode;
}

void InitHook()
{
  // 创建内核对象：物理页								创建物理页，如果这个名字的内核对象存在了就直接返回句柄																
  HANDLE hMapFile = CreateFileMapping(0, 0, PAGE_READWRITE, 0, 0x1000, L"要保护的进程ID");

  // 将物理页和线性地址(虚拟内存)进行映射从哪里开始映射																								
  LPSTR lpBuf = (LPSTR)MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0x1000);
  // 读取共享物理页内容				
  dwProtectPid = *PDWORD(lpBuf);

  // 关闭映射 断开物理页和虚拟内存的映射												
  UnmapViewOfFile(lpBuf);

  // 关闭内核句柄												
  CloseHandle(hMapFile);
  //1 得到Hook的指令																								自己函数地址 - 目标函数地址 - 5																		
  DWORD dwOffset = (DWORD)MyOpenProcess - (DWORD)OpenProcess - 5;
  *(PDWORD)(g_NewCode + 1) = dwOffset;
  //memcpy(g_NewCode + 1, &dwOffset, 4);																																										
  //2 得到原始的指令																																										
  memcpy(g_OldCode, OpenProcess, 5);
}

void InstallHook()
{

  //1 修改Hook点的属性																													
  DWORD dwOldProtect = 0;
  VirtualProtect(OpenProcess, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
  //2 修改指令																																										
  memcpy(OpenProcess, g_NewCode, 5);
  //3 属性还原																																										
  VirtualProtect(OpenProcess, 5, dwOldProtect, &dwOldProtect);
}

void UnInstallHook()
{
  //1 改属性																																										
  DWORD dwOldProtect = 0;
  VirtualProtect(OpenProcess, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);																								
  //2 改指令																																										
  memcpy(OpenProcess, g_OldCode, 5);
  //3 属性还原																																										
  VirtualProtect(OpenProcess, 5, dwOldProtect, &dwOldProtect);
}

