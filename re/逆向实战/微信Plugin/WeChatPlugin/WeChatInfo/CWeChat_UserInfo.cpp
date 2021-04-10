#include "CWeChat_UserInfo.h"
#include <tchar.h>
#include <Wininet.h>
#include "resource.h"
// WeChatWinģ���ַ
#define WeChatWinModule _T("WeChatWin.dll")
DWORD g_dwWeChatWinBase = 0;

// ��*��ʾָ�롣
DWORD g_dwWXIDOffset   = 0x1856A14;// *΢��IDƫ��
DWORD g_dwNameOffset   = 0x1856A8C;// ΢���ǳ�ƫ��
DWORD g_dwTelOffset    = 0x1856AC0;// ΢�Ű��ֻ���ƫ��
DWORD g_dwEmilOffset   = 0x1856AA8;// *΢�Ű�����ƫ��
DWORD g_dwIDOffset     = 0x1856BF0;// *΢���˺�ƫ��
DWORD g_dwHeaderOffset = 0x1856D54;// *ͷ����վƫ��
HINSTANCE g_hSelfModule;

INT_PTR CALLBACK  UserInfoDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_COMMAND:
  {
    if (wParam == IDC_BTM_GETINFO)
    {
      // ����΢��ID
      SetDlgItemTextA(hWnd, IDC_EDIT_ID, (LPCSTR) * (PDWORD)(g_dwWeChatWinBase + g_dwWXIDOffset));
      
      // ����΢���ǳ�
      SetDlgItemTextA(hWnd, IDC_EDIT_NAME, (LPCSTR)(g_dwWeChatWinBase + g_dwNameOffset));

      // ����΢���ֻ���
      SetDlgItemTextA(hWnd, IDC_EDIT_TEL, (LPCSTR)(g_dwWeChatWinBase + g_dwTelOffset));

      // ����΢������
      SetDlgItemTextA(hWnd, IDC_EDIT_EMIL, (LPCSTR) * (PDWORD)(g_dwWeChatWinBase + g_dwEmilOffset));

      // ����΢���˺�
      SetDlgItemTextA(hWnd, IDC_EDIT_ZH, (LPCSTR) * (PDWORD)(g_dwWeChatWinBase + g_dwIDOffset));

      // ����΢��ͷ����վ
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

// ��ȡ�û���Ϣ�����߳�
DWORD WINAPI  WorkThread(LPVOID lpThreadParameter)
{
  g_hSelfModule = GetModuleHandle(_T("WeChatInfo.dll"));
  DialogBox(g_hSelfModule, MAKEINTRESOURCE(IDD_DLGUSERINFO), 0, UserInfoDlgProc);
  return 0;
}

// ע��DLL��ʼ��
void InitInject()
{
  // 1����ȡWeChatWin.dll ģ���ַ
  g_dwWeChatWinBase = (DWORD)GetModuleHandle(WeChatWinModule);
  if (0 == g_dwWeChatWinBase)
  {
    OutputDebugString(_T("��ȡWeChatWin.dllģ���ַʧ��\r\n"));
    return;
  }

  // 2�������߳�
  HANDLE hThread = CreateThread(0, 0, WorkThread, 0, 0, 0);
  if (hThread == 0)
  {
    OutputDebugString(_T("�����߳̿���ʧ��\r\n"));
    return;
  }
  CloseHandle(hThread);
}
