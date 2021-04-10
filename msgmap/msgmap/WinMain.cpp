// link "kernel32.lib" "user32.lib" "gdi32.lib"
#include "msgproc.h"
#include "resource.h"
#include <Windows.h>
// function declare
BOOL InitInstance(HINSTANCE hInst);
BOOL InitApplication(HINSTANCE hInst, int nShowCmd);

// globle variable
wchar_t g_szClsName[] = L"JackLoong";
wchar_t g_szWndName[] = L"MyApp";
HINSTANCE g_hInst = NULL;

// ��ں���
int WINAPI wWinMain(
  _In_ HINSTANCE hInst, 
  _In_opt_ HINSTANCE hPrevInst,
  _In_ LPWSTR lpCmdLine,
  _In_ int nShowCmd)
{
  g_hInst = hInst;
  // ��ʼ��ʵ��---ע�ᴰ����
  if (!InitInstance(hInst))
    return FALSE;

  // ��ʼ��Ӧ�ó���---������ʾ���´���
  if (!InitApplication(hInst, nShowCmd))
    return FALSE;

  // ��Ϣѭ�����ڷַ���Ϣ
  MSG msg = { 0 };
  while (GetMessage(&msg,NULL,0,0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  //while (true)
  //{
  //  if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
  //  {
  //    if (msg.message == WM_QUIT)
  //      break;
  //  }
  //  else
  //  {
  //    // ����ϵͳ����ʱ����
  //  }
  //}
  return msg.lParam;
}

// ��ʼ��Ӧ�ó���ʵ��
BOOL InitInstance(HINSTANCE hInst)
{
  WNDCLASS wc = { 0 };
  wc.style = CS_VREDRAW | CS_HREDRAW;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.lpfnWndProc = DispathMsg;
  wc.lpszClassName = g_szClsName;
  wc.lpszMenuName = NULL; //  rc menu_name set
  wc.hInstance = hInst;
  wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_JACKLOONGICON));
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
  return RegisterClass(&wc);
}

// ��ʼ��Ӧ�ó���
BOOL InitApplication(HINSTANCE hInst, int nShowCmd)
{
  HMENU hWinMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_WINMENU));// res ID set

  HWND hWnd = CreateWindow(g_szClsName, g_szWndName, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    NULL, hWinMenu, hInst, NULL);
  if (!hWnd)
    return FALSE; 

  ShowWindow(hWnd, nShowCmd);
  UpdateWindow(hWnd);
  return TRUE;
}