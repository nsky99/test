#include "CDbg.h"
#include <locale>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

int main()
{
  if (SymInitialize(GetCurrentProcess(), NULL, TRUE))
  {
    DWORD64  dwDisplacement = 0;
    char* buffer = new char[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(CHAR)]{ 0 };
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;
    //根据地址获取符号信息

    SymFromAddr(GetCurrentProcess(), (DWORD64)main, &dwDisplacement, pSymbol);

    SymCleanup(GetCurrentProcess());

  }

  setlocale(LC_ALL, "chs");
  MainLoop();
  return 0;
}


