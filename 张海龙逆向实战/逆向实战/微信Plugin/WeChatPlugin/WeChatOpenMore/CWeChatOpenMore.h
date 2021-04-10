#pragma once
#include <Windows.h>

// 窗口回调
INT_PTR CALLBACK OpenMoreProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 初始化
void InitWnd(HMODULE hModule);

// 微信多开
BOOL StartWeChat();

// 启动进程
BOOL StartProcess(TCHAR* startApp);

// 开始HOOK
BOOL StartHook();

