#pragma once
#include <Windows.h>

// 获取用户信息工作线程
DWORD WINAPI  WorkThread(LPVOID lpThreadParameter);

// 初始化注入
void InitInject();