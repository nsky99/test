#ifndef _CBP_H_
#define _CBP_H_
#include <Windows.h>

// 软件断点
typedef struct _SOFTWAREBP
{
  BYTE bOldByte;      // 老的字节
  BOOL bEnale;        // 是否启用
  LPVOID lpBpAddr;    // 断点地址
}SOFTWAREBP, * PSOFTWAREBP;

// 设置CC断点
void SetCCBP(HANDLE hProcess, LPVOID lpAddr);

// 禁用CC断点
void DisableCCBP(HANDLE hProcess, LPVOID lpAddr);

// 启用CC断点
void EnableCCBP(HANDLE hProcess, LPVOID lpAddr);

// 显示所有的软件断点
void ShowAllCCBP();

// 删除CC断点
void CleanCCBP(HANDLE hProcess, LPVOID lpAddr);

// 硬件断点
typedef struct _HARDWAREBP
{
  LPVOID dwAddr;
  DWORD dwType;          // 硬件断点的类型 rwe
}HARDWAREBP, * PHARDWAREBP;

// 设置硬件执行断点
BOOL SetBPHW(HANDLE hThread, LPVOID dwBPAddr, BYTE type, BYTE len, bool isSave);

// 重新设置硬件断点
BOOL ReSetBPHW(HANDLE hThread, LPVOID dwBPAddr, BYTE type, BYTE len, bool isSave);

// 显示执行断点
void ShowHWBP();

// 修复硬件断点
void FixHWBP(HANDLE hThread, LPVOID lpAddr, bool& isHDPer);


// 删除硬件断点
void CleanHWBP(HANDLE hThread, LPVOID lpAddr);

typedef struct _MEMBP
{
  LPVOID lpAddr;      // 内存断点地址
  LPVOID lpBPAddr;    // 内存断点断下的地址
  DWORD  dwType;      // 内存断点类型
  BYTE   isBreak;     // 内存断点是否命中
  DWORD  dwOldProtect;// 这个内存的原来属性
}MEMBP, * PMEMBP;

// 设置内存断点
void SetMemBP(HANDLE hThread, HANDLE hProcess, LPVOID lpAddr, BYTE type);

// 禁用内存断点
void DisableMemBP(HANDLE hProcess, LPVOID lpAddr);

// 启用内存断点
void EnableMemBP(HANDLE hProcess, LPVOID lpAddr);

// 删除内存断点
void CleanMemBP(HANDLE hProcess, LPVOID lpAddr);

// 删除所有的CC断点
void DeleteAllCCBP(HANDLE hProcess);
#endif // !_CBP_H_

