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
  // �����ں˶�������ҳ								��������ҳ�����������ֵ��ں˶�������˾�ֱ�ӷ��ؾ��																
  HANDLE hMapFile = CreateFileMapping(0, 0, PAGE_READWRITE, 0, 0x1000, L"Ҫ�����Ľ���ID");

  // ������ҳ�����Ե�ַ(�����ڴ�)����ӳ������￪ʼӳ��																								
  LPSTR lpBuf = (LPSTR)MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0x1000);
  // ��ȡ��������ҳ����				
  dwProtectPid = *PDWORD(lpBuf);

  // �ر�ӳ�� �Ͽ�����ҳ�������ڴ��ӳ��												
  UnmapViewOfFile(lpBuf);

  // �ر��ں˾��												
  CloseHandle(hMapFile);
  //1 �õ�Hook��ָ��																								�Լ�������ַ - Ŀ�꺯����ַ - 5																		
  DWORD dwOffset = (DWORD)MyOpenProcess - (DWORD)OpenProcess - 5;
  *(PDWORD)(g_NewCode + 1) = dwOffset;
  //memcpy(g_NewCode + 1, &dwOffset, 4);																																										
  //2 �õ�ԭʼ��ָ��																																										
  memcpy(g_OldCode, OpenProcess, 5);
}

void InstallHook()
{

  //1 �޸�Hook�������																													
  DWORD dwOldProtect = 0;
  VirtualProtect(OpenProcess, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
  //2 �޸�ָ��																																										
  memcpy(OpenProcess, g_NewCode, 5);
  //3 ���Ի�ԭ																																										
  VirtualProtect(OpenProcess, 5, dwOldProtect, &dwOldProtect);
}

void UnInstallHook()
{
  //1 ������																																										
  DWORD dwOldProtect = 0;
  VirtualProtect(OpenProcess, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);																								
  //2 ��ָ��																																										
  memcpy(OpenProcess, g_OldCode, 5);
  //3 ���Ի�ԭ																																										
  VirtualProtect(OpenProcess, 5, dwOldProtect, &dwOldProtect);
}

