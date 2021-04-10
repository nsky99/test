#pragma once
#include <Windows.h>

// About对话框消息分发
INT_PTR CALLBACK AboutDispathMsg(HWND, UINT, WPARAM, LPARAM);


INT_PTR  OnInitDialog(HWND, UINT, WPARAM, LPARAM);
INT_PTR  OnAboutClose(HWND, UINT, WPARAM, LPARAM);




