#pragma once
#include <Windows.h>

// ���ڻص�
INT_PTR CALLBACK OpenMoreProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ��ʼ��
void InitWnd(HMODULE hModule);

// ΢�Ŷ࿪
BOOL StartWeChat();

// ��������
BOOL StartProcess(TCHAR* startApp);

// ��ʼHOOK
BOOL StartHook();

