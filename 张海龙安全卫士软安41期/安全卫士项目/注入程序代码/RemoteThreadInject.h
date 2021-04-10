#pragma once
#include <Windows.h>
#include <string>
using  std::string;
BOOL Inject(DWORD dwPid, string strDllPath);