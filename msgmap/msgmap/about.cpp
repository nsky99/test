#include "common.h"
#include "about.h"
#include "resource.h"

// ����Ϣ
MSGMAP_ENTRYDLG g_messageEntries[] = {
  WM_INITDIALOG,OnInitDialog,
  WM_CLOSE,OnAboutClose
};

//MSGMAP_ENTRYDLG g_CommandEntries[] = {
//
//};

INT_PTR CALLBACK AboutDispathMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  for (size_t i = 0; i < dim(g_messageEntries); i++)
    if (uMsg == g_messageEntries[i].uMsg)
      return (*g_messageEntries[i].pfn)(hWnd, uMsg, wParam, lParam);

  return 0;
}


INT_PTR  OnInitDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  MessageBox(hWnd, L"���ڶԻ������ڱ�����", L"OK", MB_OK);
  return 1;
}


INT_PTR  OnAboutClose(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  EndDialog(hWnd, 0);
  return 1;
}