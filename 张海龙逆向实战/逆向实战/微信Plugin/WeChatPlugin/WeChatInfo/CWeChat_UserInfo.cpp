#include "CWeChat_UserInfo.h"
#include <tchar.h>
#include <Wininet.h>
#include "resource.h"
// WeChatWin模块基址
#define WeChatWinModule _T("WeChatWin.dll")
DWORD g_dwWeChatWinBase = 0;

// 带*表示指针。
DWORD g_dwWXIDOffset   = 0x1856A14;// *微信ID偏移
DWORD g_dwNameOffset   = 0x1856A8C;// 微信昵称偏移
DWORD g_dwTelOffset    = 0x1856AC0;// 微信绑定手机号偏移
DWORD g_dwEmilOffset   = 0x1856AA8;// *微信绑定邮箱偏移
DWORD g_dwIDOffset     = 0x1856BF0;// *微信账号偏移
DWORD g_dwHeaderOffset = 0x1856D54;// *头像网站偏移
HINSTANCE g_hSelfModule;

INT_PTR CALLBACK  UserInfoDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_COMMAND:
  {
    if (wParam == IDC_BTM_GETINFO)
    {
      // 设置微信ID
      SetDlgItemTextA(hWnd, IDC_EDIT_ID, (LPCSTR) * (PDWORD)(g_dwWeChatWinBase + g_dwWXIDOffset));
      
      // 设置微信昵称
      SetDlgItemTextA(hWnd, IDC_EDIT_NAME, (LPCSTR)(g_dwWeChatWinBase + g_dwNameOffset));

      // 设置微信手机号
      SetDlgItemTextA(hWnd, IDC_EDIT_TEL, (LPCSTR)(g_dwWeChatWinBase + g_dwTelOffset));

      // 设置微信邮箱
      SetDlgItemTextA(hWnd, IDC_EDIT_EMIL, (LPCSTR) * (PDWORD)(g_dwWeChatWinBase + g_dwEmilOffset));

      // 设置微信账号
      SetDlgItemTextA(hWnd, IDC_EDIT_ZH, (LPCSTR) * (PDWORD)(g_dwWeChatWinBase + g_dwIDOffset));

      // 设置微信头像网站
      SetDlgItemTextA(hWnd, IDC_EDIT_HEADER, (LPCSTR) * (PDWORD)(g_dwWeChatWinBase + g_dwHeaderOffset));

      break;
    }
  }
    break;
  case WM_CLOSE:
    FreeLibraryAndExitThread(g_hSelfModule, 0);
    EndDialog(hWnd, 0);
    break;
  default:
    break;
  }
  return 0;
}

// 获取用户信息工作线程
DWORD WINAPI  WorkThread(LPVOID lpThreadParameter)
{
  g_hSelfModule = GetModuleHandle(_T("WeChatInfo.dll"));
  DialogBox(g_hSelfModule, MAKEINTRESOURCE(IDD_DLGUSERINFO), 0, UserInfoDlgProc);
  return 0;
}

// 注入DLL初始化
void InitInject()
{
  // 1、获取WeChatWin.dll 模块基址
  g_dwWeChatWinBase = (DWORD)GetModuleHandle(WeChatWinModule);
  if (0 == g_dwWeChatWinBase)
  {
    OutputDebugString(_T("获取WeChatWin.dll模块基址失败\r\n"));
    return;
  }

  // 2、创建线程
  HANDLE hThread = CreateThread(0, 0, WorkThread, 0, 0, 0);
  if (hThread == 0)
  {
    OutputDebugString(_T("工作线程开启失败\r\n"));
    return;
  }
  CloseHandle(hThread);
}
