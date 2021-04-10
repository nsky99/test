#include "CBp.h"
#include "Reg.h"
#include <vector>
#include "CDbg.h"

// ���������
#include "./capstone/include/capstone.h"
#ifdef _WIN64 // 64λƽ̨���������Զ����������
#pragma comment(lib, "capstone/lib/capstone_x64.lib")
#else
#pragma comment(lib,"capstone/lib/capstone_x86.lib")
#endif // _64
using std::vector;

vector<SOFTWAREBP> g_vecSoftWareBP;
vector<HARDWAREBP> g_vecHardWareBP;
vector<MEMBP>      g_vecMemBP;

// ����CC�ϵ�
void SetCCBP(HANDLE hProcess, LPVOID lpAddr)
{
  SOFTWAREBP bp;
  // ����ԭ�����ֽ�
  ReadProcessMemory(hProcess, lpAddr, &bp.bOldByte, 1, 0);

  // д��CC�ϵ㣨���ֽڣ�
  WriteProcessMemory(hProcess, lpAddr, "\xCC", 1, 0);

  // ���öϵ���Ϣ
  bp.bEnale = TRUE;
  bp.lpBpAddr = lpAddr;
  // ����ϵ�
  g_vecSoftWareBP.push_back(bp);
}

// ����CC�ϵ�
void DisableCCBP(HANDLE hProcess, LPVOID lpAddr)
{
  // ���ϵ��ֽ�д��ȥ,���öϵ�,���ǲ�ɾ��
  for (size_t i = 0; i < g_vecSoftWareBP.size(); i++)
  {
    // ����ǵ�ǰ�ϵ㣬�������ã��ͽ��öϵ�
    if (g_vecSoftWareBP[i].lpBpAddr == lpAddr && g_vecSoftWareBP[i].bEnale == TRUE)
    {
      // �ָ��ϵ��ֽ�
      WriteProcessMemory(hProcess, lpAddr, &g_vecSoftWareBP[i].bOldByte, 1, 0);
      // ���öϵ�
      g_vecSoftWareBP[i].bEnale = FALSE;

      break;
    }
  }
}

// ����CC�ϵ�
void EnableCCBP(HANDLE hProcess, LPVOID lpAddr)
{
  // ���ϵ��ֽ�д��ȥ,���öϵ�,���ǲ�ɾ��
  for (size_t i = 0; i < g_vecSoftWareBP.size(); i++)
  {
    if (g_vecSoftWareBP[i].lpBpAddr == lpAddr && g_vecSoftWareBP[i].bEnale == FALSE)
    {
      // �����ϵĶϵ�
      ReadProcessMemory(hProcess, lpAddr, &g_vecSoftWareBP[i].bOldByte, 1, 0);
      // д��CC
      WriteProcessMemory(hProcess, lpAddr, "\xCC", 1, 0);
      // ���öϵ�
      g_vecSoftWareBP[i].bEnale = TRUE;
      break;
    }
  }
}

// ��ʾ����CC�ϵ�
void ShowAllCCBP()
{
  printf("========����ϵ�=========\n");
  for (size_t i = 0; i < g_vecSoftWareBP.size(); i++)
  {
    printf("%d 0x%08X %s\n", i, (DWORD)g_vecSoftWareBP[i].lpBpAddr,
      g_vecSoftWareBP[i].bEnale ? "����" : "����");
  }
  printf("========================\n\n");
}

// ɾ��ָ��CC�ϵ�
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

// ɾ�����е�CC�ϵ�
void DeleteAllCCBP(HANDLE hProcess)
{
  for (size_t i = 0; i < g_vecSoftWareBP.size(); i++)
  {
    WriteProcessMemory(hProcess, g_vecSoftWareBP[i].lpBpAddr, &g_vecSoftWareBP[i].bOldByte, 1, 0);
    g_vecSoftWareBP.erase(g_vecSoftWareBP.begin() + i);
  }
}

// ����Ӳ���ϵ�
BOOL SetBPHW(HANDLE hThread, LPVOID dwBPAddr, BYTE type, BYTE len, bool isSave)
{
  HARDWAREBP hwbp = { dwBPAddr ,type };
  // Ӳ���ϵ�ֻ����4��
  if (g_vecHardWareBP.size() < 4)
  {
    CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
    GetThreadContext(hThread, &ct);
    DBG_REG7* pDr7 = (DBG_REG7*)&ct.Dr7;
    if (pDr7->L0 == 0)// Dr0û�б�ʹ��
    {
      ct.Dr0 = (DWORD)dwBPAddr;
      pDr7->RW0 = type;
      pDr7->LEN0 = len;
      pDr7->L0 = 1;
    }
    else if (pDr7->L1 == 0)// Dr1û�б�ʹ��
    {
      ct.Dr1 = (DWORD)dwBPAddr;
      pDr7->RW1 = type;
      pDr7->LEN1 = len;
      pDr7->L1 = 1;
    }
    else if (pDr7->L2 == 0)// Dr2û�б�ʹ��
    {
      ct.Dr2 = (DWORD)dwBPAddr;
      pDr7->RW2 = type;
      pDr7->LEN2 = len;
      pDr7->L2 = 1;
    }
    else if (pDr7->L3 == 0)// Dr3û�б�ʹ��
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

// ��������Ӳ���ϵ�
BOOL ReSetBPHW(HANDLE hThread, LPVOID dwBPAddr, BYTE type, BYTE len, bool isSave)
{
  HARDWAREBP hwbp = { dwBPAddr ,type };
  // Ӳ���ϵ�ֻ����4��
  CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
  GetThreadContext(hThread, &ct);
  DBG_REG7* pDr7 = (DBG_REG7*)&ct.Dr7;
  if (pDr7->L0 == 0 && dwBPAddr == (LPVOID)ct.Dr0)// Dr0û�б�ʹ��
  {
    ct.Dr0 = (DWORD)dwBPAddr;
    pDr7->RW0 = type;
    pDr7->LEN0 = len;
    pDr7->L0 = 1;
  }
  else if (pDr7->L1 == 0 && dwBPAddr == (LPVOID)ct.Dr1)// Dr1û�б�ʹ��
  {
    ct.Dr1 = (DWORD)dwBPAddr;
    pDr7->RW1 = type;
    pDr7->LEN1 = len;
    pDr7->L1 = 1;
  }
  else if (pDr7->L2 == 0 && dwBPAddr == (LPVOID)ct.Dr2)// Dr2û�б�ʹ��
  {
    ct.Dr2 = (DWORD)dwBPAddr;
    pDr7->RW2 = type;
    pDr7->LEN2 = len;
    pDr7->L2 = 1;
  }
  else if (pDr7->L3 == 0 && dwBPAddr == (LPVOID)ct.Dr3)// Dr3û�б�ʹ��
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

// ��ʾӲ���ϵ�
void ShowHWBP()
{
  printf("========Ӳ���ϵ�=========\n");
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

// �޸�Ӳ���ϵ�
void FixHWBP(HANDLE hThread, LPVOID lpAddr, bool& isHDPer)
{
  //1. ��ȡĿ���̵߳ļĴ���
  CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
  GetThreadContext(hThread, &ct);
  DBG_REG7* Dr7 = (DBG_REG7*)&ct.Dr7;

  //2. �ָ��������ϵ��LENλ
  switch (ct.Dr6 & 0xF)
  {
  case 1:Dr7->L0 = 0; break;
  case 2:Dr7->L1 = 0; break;
  case 4:Dr7->L2 = 0; break;
  case 8:Dr7->L3 = 0; break;
  default:break;
  }

  SetThreadContext(hThread, &ct);

  //����TF�ϵ�
  StepIn();
  isHDPer = true;
}

// ɾ��Ӳ���ϵ�
void CleanHWBP(HANDLE hThread, LPVOID lpAddr)
{
  //1. ��ȡĿ���̵߳ļĴ���
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

// �����ڴ�ϵ�
void SetMemBP(HANDLE hThread, HANDLE hProcess, LPVOID lpAddr, BYTE type)
{
  MEMBP membp;
  membp.dwType = type;
  membp.lpAddr = lpAddr;
  membp.isBreak = false;

  switch (type)
  {
  case 0:// ��
    // ������ڴ��ַ���ò��ɶ����� ---- ���ø���־����������������쳣���ǵ��������쳣
    VirtualProtectEx(hProcess, lpAddr, 1, PAGE_NOACCESS, &membp.dwOldProtect);
    break;
  case 1:// д
    // ������ڴ��ַ���ò���д���� ---- ���ø���־����������������쳣���ǵ��������쳣
    VirtualProtectEx(hProcess, lpAddr, 1, PAGE_READONLY, &membp.dwOldProtect);
    break;
  case 2:// ִ��
    // ������ڴ��ַ���ò���ִ������ ---- ���ø���־����������������쳣���ǵ��������쳣
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
      case 0:// ��
        // ������ڴ��ַ���ò��ɶ����� ---- ���ø���־����������������쳣���ǵ��������쳣
        VirtualProtectEx(hProcess, g_vecMemBP[i].lpAddr, 1, PAGE_NOACCESS, &g_vecMemBP[i].dwOldProtect);
        break;
      case 1:// д
        // ������ڴ��ַ���ò���д���� ---- ���ø���־����������������쳣���ǵ��������쳣
        VirtualProtectEx(hProcess, g_vecMemBP[i].lpAddr, 1, PAGE_READONLY, &g_vecMemBP[i].dwOldProtect);
        break;
      case 2:// ִ��
        // ������ڴ��ַ���ò���ִ������ ---- ���ø���־����������������쳣���ǵ��������쳣
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


