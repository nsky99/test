#include "pch.h"
#include "CWeChatOpenMore.h"
#include "resource.h"
#include <tchar.h>


DWORD g_WeChatPid = 0;
DWORD g_WeChatTid = 0;
DWORD g_WeChatWinBase = 0;
DWORD g_dwHookOffsetAddress = 0x96A445;
DWORD g_dwRetOffsetAddress = 0x96A44B;
DWORD g_dwRet = 0;
DWORD g_dwCallAddress = (DWORD)CreateMutexW;
BYTE  g_bNewCode[5] = { 0xE9,0 };
BYTE  g_bOldCode[5] = { };
void __declspec(naked) function()
{
  __asm
  {
    push ebp
    mov ebp, esp
    pushad

    mov[ebp + 0xC], 0

    popad
    mov esp,ebp
    pop ebp
  }
  
  __asm
  {
    call [g_dwCallAddress]
    jmp [g_dwRet]
  }
}

INT_PTR CALLBACK OpenMoreProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_COMMAND:
  {
    // 微信多开
    if (wParam == IDC_BTN_STARTWECHAT)
    {
      if (FALSE == StartWeChat())
      {
        OutputDebugString(_T("启动微信失败\r\n"));
      }
    }
  }
  break;
  default:
    break;
  }


  return 0;
}

void InitWnd(HMODULE hModule)
{
  DialogBox(hModule, MAKEINTRESOURCE(IDD_DLG_MAIN), 0, OpenMoreProc);
}


BOOL StartWeChat()
{
  TCHAR szAppPath[] = _T("C:\\Program Files (x86)\\Tencent\\WeChat\\WeChat.exe");
  if (FALSE == StartProcess(szAppPath))
  {
    return FALSE;
  }

  StartHook();
  return TRUE;
}

BOOL StartProcess(TCHAR* startApp)
{
  STARTUPINFO si = { sizeof(si) };
  PROCESS_INFORMATION pi = { 0 };

  // 以挂起的方式创建一个进程
  BOOL bRet = CreateProcess(0, startApp, 0, 0, 0, CREATE_SUSPENDED, 0, 0, &si, &pi);
  g_WeChatPid = pi.dwProcessId;
  g_WeChatTid = pi.dwThreadId;
  if (pi.hProcess != 0)
  {
    CloseHandle(pi.hProcess);
  }
  if (pi.hThread != 0)
  {
    CloseHandle(pi.hThread);
  }
  return bRet;
}

BOOL StartHook()
{

  return TRUE;
}
