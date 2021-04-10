// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "CWeChatOpenMore.h"
#include "resource.h"



int _stdcall wWinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPTSTR lpCmdLine,
  int nCmdShow
)
{
  InitWnd(hInstance);
  return 0;
}

