#include "CBp.h"
#include "Reg.h"
#include <vector>
#include "CDbg.h"

// 反汇编引擎
#include "./capstone/include/capstone.h"
#ifdef _WIN64 // 64位平台编译器会自动定义这个宏
#pragma comment(lib, "capstone/lib/capstone_x64.lib")
#else
#pragma comment(lib,"capstone/lib/capstone_x86.lib")
#endif // _64
using std::vector;

vector<SOFTWAREBP> g_vecSoftWareBP;
vector<HARDWAREBP> g_vecHardWareBP;
vector<MEMBP>      g_vecMemBP;

// 设置CC断点
void SetCCBP(HANDLE hProcess, LPVOID lpAddr)
{
  SOFTWAREBP bp;
  // 保存原来的字节
  ReadProcessMemory(hProcess, lpAddr, &bp.bOldByte, 1, 0);

  // 写入CC断点（新字节）
  WriteProcessMemory(hProcess, lpAddr, "\xCC", 1, 0);

  // 设置断点信息
  bp.bEnale = TRUE;
  bp.lpBpAddr = lpAddr;
  // 保存断点
  g_vecSoftWareBP.push_back(bp);
}

// 禁用CC断点
void DisableCCBP(HANDLE hProcess, LPVOID lpAddr)
{
  // 把老的字节写进去,禁用断点,但是不删除
  for (size_t i = 0; i < g_vecSoftWareBP.size(); i++)
  {
    // 如果是当前断点，并且启用，就禁用断点
    if (g_vecSoftWareBP[i].lpBpAddr == lpAddr && g_vecSoftWareBP[i].bEnale == TRUE)
    {
      // 恢复老的字节
      WriteProcessMemory(hProcess, lpAddr, &g_vecSoftWareBP[i].bOldByte, 1, 0);
      // 禁用断点
      g_vecSoftWareBP[i].bEnale = FALSE;

      break;
    }
  }
}

// 启用CC断点
void EnableCCBP(HANDLE hProcess, LPVOID lpAddr)
{
  // 把老的字节写进去,禁用断点,但是不删除
  for (size_t i = 0; i < g_vecSoftWareBP.size(); i++)
  {
    if (g_vecSoftWareBP[i].lpBpAddr == lpAddr && g_vecSoftWareBP[i].bEnale == FALSE)
    {
      // 保存老的断点
      ReadProcessMemory(hProcess, lpAddr, &g_vecSoftWareBP[i].bOldByte, 1, 0);
      // 写入CC
      WriteProcessMemory(hProcess, lpAddr, "\xCC", 1, 0);
      // 启用断点
      g_vecSoftWareBP[i].bEnale = TRUE;
      break;
    }
  }
}

// 显示所有CC断点
void ShowAllCCBP()
{
  printf("========软件断点=========\n");
  for (size_t i = 0; i < g_vecSoftWareBP.size(); i++)
  {
    printf("%d 0x%08X %s\n", i, (DWORD)g_vecSoftWareBP[i].lpBpAddr,
      g_vecSoftWareBP[i].bEnale ? "启用" : "禁用");
  }
  printf("========================\n\n");
}

// 删除指定CC断点
void CleanCCBP(HANDLE hProcess, LPVOID lpAddr)
{
  for (size_t i = 0; i < g_vecSoftWareBP.size(); i++)
  {
    if (g_vecSoftWareBP[i].lpBpAddr == lpAddr)
    {
      WriteProcessMemory(hProcess, lpAddr, &g_vecSoftWareBP[i].bOldByte, 1, 0);
      g_vecSoftWareBP.erase(g_vecSoftWareBP.begin() + i);
    }
  }
}

// 删除所有的CC断点
void DeleteAllCCBP(HANDLE hProcess)
{
  for (size_t i = 0; i < g_vecSoftWareBP.size(); i++)
  {
    WriteProcessMemory(hProcess, g_vecSoftWareBP[i].lpBpAddr, &g_vecSoftWareBP[i].bOldByte, 1, 0);
    g_vecSoftWareBP.erase(g_vecSoftWareBP.begin() + i);
  }
}

// 设置硬件断点
BOOL SetBPHW(HANDLE hThread, LPVOID dwBPAddr, BYTE type, BYTE len, bool isSave)
{
  HARDWAREBP hwbp = { dwBPAddr ,type };
  // 硬件断点只能下4个
  if (g_vecHardWareBP.size() < 4)
  {
    CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
    GetThreadContext(hThread, &ct);
    DBG_REG7* pDr7 = (DBG_REG7*)&ct.Dr7;
    if (pDr7->L0 == 0)// Dr0没有被使用
    {
      ct.Dr0 = (DWORD)dwBPAddr;
      pDr7->RW0 = type;
      pDr7->LEN0 = len;
      pDr7->L0 = 1;
    }
    else if (pDr7->L1 == 0)// Dr1没有被使用
    {
      ct.Dr1 = (DWORD)dwBPAddr;
      pDr7->RW1 = type;
      pDr7->LEN1 = len;
      pDr7->L1 = 1;
    }
    else if (pDr7->L2 == 0)// Dr2没有被使用
    {
      ct.Dr2 = (DWORD)dwBPAddr;
      pDr7->RW2 = type;
      pDr7->LEN2 = len;
      pDr7->L2 = 1;
    }
    else if (pDr7->L3 == 0)// Dr3没有被使用
    {
      ct.Dr3 = (DWORD)dwBPAddr;
      pDr7->RW3 = type;
      pDr7->LEN3 = len;
      pDr7->L3 = 1;
    }
    else
    {
      return FALSE;
    }
    SetThreadContext(hThread, &ct);
    if (isSave)
    {
      g_vecHardWareBP.push_back(hwbp);
    }
  }
  else
  {
    return FALSE;
  }
  return TRUE;
}

// 重新设置硬件断点
BOOL ReSetBPHW(HANDLE hThread, LPVOID dwBPAddr, BYTE type, BYTE len, bool isSave)
{
  HARDWAREBP hwbp = { dwBPAddr ,type };
  // 硬件断点只能下4个
  CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
  GetThreadContext(hThread, &ct);
  DBG_REG7* pDr7 = (DBG_REG7*)&ct.Dr7;
  if (pDr7->L0 == 0 && dwBPAddr == (LPVOID)ct.Dr0)// Dr0没有被使用
  {
    ct.Dr0 = (DWORD)dwBPAddr;
    pDr7->RW0 = type;
    pDr7->LEN0 = len;
    pDr7->L0 = 1;
  }
  else if (pDr7->L1 == 0 && dwBPAddr == (LPVOID)ct.Dr1)// Dr1没有被使用
  {
    ct.Dr1 = (DWORD)dwBPAddr;
    pDr7->RW1 = type;
    pDr7->LEN1 = len;
    pDr7->L1 = 1;
  }
  else if (pDr7->L2 == 0 && dwBPAddr == (LPVOID)ct.Dr2)// Dr2没有被使用
  {
    ct.Dr2 = (DWORD)dwBPAddr;
    pDr7->RW2 = type;
    pDr7->LEN2 = len;
    pDr7->L2 = 1;
  }
  else if (pDr7->L3 == 0 && dwBPAddr == (LPVOID)ct.Dr3)// Dr3没有被使用
  {
    ct.Dr3 = (DWORD)dwBPAddr;
    pDr7->RW3 = type;
    pDr7->LEN3 = len;
    pDr7->L3 = 1;
  }
  else
  {
    return FALSE;
  }
  SetThreadContext(hThread, &ct);
  if (isSave)
  {
    g_vecHardWareBP.push_back(hwbp);
  }
  return TRUE;
}

// 显示硬件断点
void ShowHWBP()
{
  printf("========硬件断点=========\n");
  for (size_t i = 0; i < g_vecHardWareBP.size(); i++)
  {
    char type[5] = { 0 };
    if (g_vecHardWareBP[i].dwType == 0)
    {
      memcpy(type, "e", sizeof("e"));
    }
    else if (g_vecHardWareBP[i].dwType == 1)
    {
      memcpy(type, "w", sizeof("w"));
    }
    else if (g_vecHardWareBP[i].dwType == 3)
    {
      memcpy(type, "rw", sizeof("rw"));
    }
    printf("%d 0x%08X %s\n", i, (DWORD)g_vecHardWareBP[i].dwAddr, type);
  }
  printf("========================\n\n");
}

// 修复硬件断点
void FixHWBP(HANDLE hThread, LPVOID lpAddr, bool& isHDPer)
{
  //1. 获取目标线程的寄存器
  CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
  GetThreadContext(hThread, &ct);
  DBG_REG7* Dr7 = (DBG_REG7*)&ct.Dr7;

  //2. 恢复被触发断点的LEN位
  switch (ct.Dr6 & 0xF)
  {
  case 1:Dr7->L0 = 0; break;
  case 2:Dr7->L1 = 0; break;
  case 4:Dr7->L2 = 0; break;
  case 8:Dr7->L3 = 0; break;
  default:break;
  }

  SetThreadContext(hThread, &ct);

  //设置TF断点
  StepIn();
  isHDPer = true;
}

// 删除硬件断点
void CleanHWBP(HANDLE hThread, LPVOID lpAddr)
{
  //1. 获取目标线程的寄存器
  CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
  GetThreadContext(hThread, &ct);
  DBG_REG7* Dr7 = (DBG_REG7*)&ct.Dr7;

  if (Dr7->RW0 == (DWORD)lpAddr)
  {
    Dr7->RW0 = 0;
    Dr7->L0 = 0;
    Dr7->LEN0 = 0;
  }
  if (Dr7->RW1 == (DWORD)lpAddr)
  {
    Dr7->RW1 = 0;
    Dr7->L1 = 0;
    Dr7->LEN1 = 0;
  }
  if (Dr7->RW2 == (DWORD)lpAddr)
  {
    Dr7->RW2 = 0;
    Dr7->L2 = 0;
    Dr7->LEN2 = 0;
  }
  if (Dr7->RW3 == (DWORD)lpAddr)
  {
    Dr7->RW3 = 0;
    Dr7->L3 = 0;
    Dr7->LEN3 = 0;
  }
  SetThreadContext(hThread, &ct);
  for (size_t i = 0; i < g_vecHardWareBP.size(); i++)
  {
    if (g_vecHardWareBP[i].dwAddr == lpAddr)
    {
      g_vecHardWareBP.erase(g_vecHardWareBP.begin() + i);
    }
  }
}

// 设置内存断点
void SetMemBP(HANDLE hThread, HANDLE hProcess, LPVOID lpAddr, BYTE type)
{
  MEMBP membp;
  membp.dwType = type;
  membp.lpAddr = lpAddr;
  membp.isBreak = false;

  switch (type)
  {
  case 0:// 读
    // 这里给内存地址设置不可读属性 ---- 设置个标志用于区分是软件的异常还是调试器的异常
    VirtualProtectEx(hProcess, lpAddr, 1, PAGE_NOACCESS, &membp.dwOldProtect);
    break;
  case 1:// 写
    // 这里给内存地址设置不可写属性 ---- 设置个标志用于区分是软件的异常还是调试器的异常
    VirtualProtectEx(hProcess, lpAddr, 1, PAGE_READONLY, &membp.dwOldProtect);
    break;
  case 2:// 执行
    // 这里给内存地址设置不可执行属性 ---- 设置个标志用于区分是软件的异常还是调试器的异常
    VirtualProtectEx(hProcess, lpAddr, 1, PAGE_NOACCESS, &membp.dwOldProtect);
    break;
  default:
    break;
  }
  g_vecMemBP.push_back(membp);
}

void DisableMemBP(HANDLE hProcess, LPVOID lpAddr)
{
  for (size_t i = 0; i < g_vecMemBP.size(); i++)
  {
    //if (g_vecMemBP[i].lpAddr == lpAddr)
    //{
      DWORD dwProtect;
      VirtualProtectEx(hProcess, lpAddr, 1, g_vecMemBP[i].dwOldProtect, &dwProtect);
      g_vecMemBP[i].isBreak = true;
      break;
    //}
  }
}

void EnableMemBP(HANDLE hProcess, LPVOID lpAddr)
{
  for (size_t i = 0; i < g_vecMemBP.size(); i++)
  {
    if (g_vecMemBP[i].isBreak == TRUE)
    {
      switch (g_vecMemBP[i].dwType)
      {
      case 0:// 读
        // 这里给内存地址设置不可读属性 ---- 设置个标志用于区分是软件的异常还是调试器的异常
        VirtualProtectEx(hProcess, g_vecMemBP[i].lpAddr, 1, PAGE_NOACCESS, &g_vecMemBP[i].dwOldProtect);
        break;
      case 1:// 写
        // 这里给内存地址设置不可写属性 ---- 设置个标志用于区分是软件的异常还是调试器的异常
        VirtualProtectEx(hProcess, g_vecMemBP[i].lpAddr, 1, PAGE_READONLY, &g_vecMemBP[i].dwOldProtect);
        break;
      case 2:// 执行
        // 这里给内存地址设置不可执行属性 ---- 设置个标志用于区分是软件的异常还是调试器的异常
        VirtualProtectEx(hProcess, g_vecMemBP[i].lpAddr, 1, PAGE_NOACCESS, &g_vecMemBP[i].dwOldProtect);
        break;
      }
      g_vecMemBP[i].isBreak = false;
      break;
    }
  }
}

void CleanMemBP(HANDLE hProcess, LPVOID lpAddr)
{
  for (size_t i = 0; i < g_vecMemBP.size(); i++)
  {
    if (g_vecMemBP[i].lpAddr == lpAddr)
    {
      DWORD dwProtect;
      VirtualProtectEx(hProcess, lpAddr, 1, g_vecMemBP[i].dwOldProtect, &dwProtect);
      g_vecMemBP.erase(g_vecMemBP.begin() + i);
    }
  }
}


