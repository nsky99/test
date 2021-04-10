#include <Windows.h>
#include "CWeChat_UserInfo.h"


BOOL WINAPI DllMain(
  HMODULE hModule,
  DWORD dwResult,
  LPVOID lpReserved
)
{

  switch (dwResult)
  {
  case DLL_PROCESS_ATTACH:
    InitInject();
    break;

  case DLL_PROCESS_DETACH:
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  default:
    break;
  }



  return TRUE;
}