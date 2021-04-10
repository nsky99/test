#include "common.h"
#include "msgproc.h"
#include "resource.h"
#include "about.h"

// 绑定消息
MSGMAP_ENTRY g_messageEntries[] = {
  WM_CLOSE,OnClose,
  WM_DESTROY, OnDestroy,
  WM_QUIT,OnQuit,
  WM_PAINT, OnPaint,
  WM_CREATE, OnCreate,
  WM_COMMAND, OnCommand
};

MSGMAP_ENTRY g_CommandEntries[] = {
  IDM_QUIT,OnQuitApp,
  IDM_ABOUT,OnAbout,
};

///////////////////////////////////////////////////////////////////////////
// System Msg
///////////////////////////////////////////////////////////////////////////

// 消息分发函数---这里直接循环分发消息给我绑定的函数
LRESULT CALLBACK DispathMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  for (size_t i = 0; i < dim(g_messageEntries); i++)
    if (uMsg == g_messageEntries[i].uMsg)
      return (*g_messageEntries[i].pfn)(hWnd, uMsg, wParam, lParam);

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


LRESULT  OnCommand(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  for (size_t i = 0; i < dim(g_CommandEntries); i++)
  {
    // 给响应的控件ID分发函数
    if (LOWORD(wParam) == g_CommandEntries[i].uMsg)
      return (*g_CommandEntries[i].pfn)(hWnd, uMsg, wParam, lParam);
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


LRESULT  OnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


LRESULT  OnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  MessageBox(hWnd, L"创建正在创建", L"Tip", MB_OK);
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


LRESULT  OnClose(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  DestroyWindow(hWnd);
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


LRESULT  OnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  PostQuitMessage(0);
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


LRESULT  OnQuit(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////
// Command Msg
///////////////////////////////////////////////////////////////////////////

LRESULT  OnQuitApp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  PostQuitMessage(0);
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


LRESULT  OnAbout(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUT), NULL, (DLGPROC)AboutDispathMsg);
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}