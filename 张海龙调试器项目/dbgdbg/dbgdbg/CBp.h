#ifndef _CBP_H_
#define _CBP_H_
#include <Windows.h>

// ����ϵ�
typedef struct _SOFTWAREBP
{
  BYTE bOldByte;      // �ϵ��ֽ�
  BOOL bEnale;        // �Ƿ�����
  LPVOID lpBpAddr;    // �ϵ��ַ
}SOFTWAREBP, * PSOFTWAREBP;

// ����CC�ϵ�
void SetCCBP(HANDLE hProcess, LPVOID lpAddr);

// ����CC�ϵ�
void DisableCCBP(HANDLE hProcess, LPVOID lpAddr);

// ����CC�ϵ�
void EnableCCBP(HANDLE hProcess, LPVOID lpAddr);

// ��ʾ���е�����ϵ�
void ShowAllCCBP();

// ɾ��CC�ϵ�
void CleanCCBP(HANDLE hProcess, LPVOID lpAddr);

// Ӳ���ϵ�
typedef struct _HARDWAREBP
{
  LPVOID dwAddr;
  DWORD dwType;          // Ӳ���ϵ������ rwe
}HARDWAREBP, * PHARDWAREBP;

// ����Ӳ��ִ�жϵ�
BOOL SetBPHW(HANDLE hThread, LPVOID dwBPAddr, BYTE type, BYTE len, bool isSave);

// ��������Ӳ���ϵ�
BOOL ReSetBPHW(HANDLE hThread, LPVOID dwBPAddr, BYTE type, BYTE len, bool isSave);

// ��ʾִ�жϵ�
void ShowHWBP();

// �޸�Ӳ���ϵ�
void FixHWBP(HANDLE hThread, LPVOID lpAddr, bool& isHDPer);


// ɾ��Ӳ���ϵ�
void CleanHWBP(HANDLE hThread, LPVOID lpAddr);

typedef struct _MEMBP
{
  LPVOID lpAddr;      // �ڴ�ϵ��ַ
  LPVOID lpBPAddr;    // �ڴ�ϵ���µĵ�ַ
  DWORD  dwType;      // �ڴ�ϵ�����
  BYTE   isBreak;     // �ڴ�ϵ��Ƿ�����
  DWORD  dwOldProtect;// ����ڴ��ԭ������
}MEMBP, * PMEMBP;

// �����ڴ�ϵ�
void SetMemBP(HANDLE hThread, HANDLE hProcess, LPVOID lpAddr, BYTE type);

// �����ڴ�ϵ�
void DisableMemBP(HANDLE hProcess, LPVOID lpAddr);

// �����ڴ�ϵ�
void EnableMemBP(HANDLE hProcess, LPVOID lpAddr);

// ɾ���ڴ�ϵ�
void CleanMemBP(HANDLE hProcess, LPVOID lpAddr);

// ɾ�����е�CC�ϵ�
void DeleteAllCCBP(HANDLE hProcess);
#endif // !_CBP_H_

