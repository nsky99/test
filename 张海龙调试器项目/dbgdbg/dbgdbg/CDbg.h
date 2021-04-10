#ifndef _CDBG_H_
#define _CDBG_H_
#include <Windows.h>
#include <vector>
#include <string>
using namespace std;
typedef struct MMODULEINFO
{
  char    mname[226];
  BYTE* uStart;
  DWORD	 uSize;
}MMODULEINFO, * PMMODULEINFO;
// ������Ϣ
typedef struct _DBGPROCESSINFO
{
  DWORD  dwPid;           // ����id
  string strProcessName;  // ��������
}DBGPROCESSINFO, * PDBGPROCESSINFO;
// ���������˵�
typedef enum _MAINMENU
{
  OPEN = '1',
  ATTACH,
  EXIT
}MAINMENU;

// ���ص��Է��Žṹ����Ϣ
typedef struct _SYMLOADINFO
{
  DWORD64 base;
  HANDLE  hFile;
}SYMLOADINFO, * PSYMLOADINFO;
// ��������ѭ��
void MainLoop();

// �򿪵��Խ���
bool OpenDbgProcess(char* pszFile);

// ���ӵ��Խ���
bool AttachDbgProcess(DWORD dwPid);

// �����¼���ѭ��
DWORD DbgEventLoop();

// ��Ӧ�쳣�����¼�
void OnExceptionDbgEvent(EXCEPTION_DEBUG_INFO exceptDbgInfo);

// ��Ӧ���̴��������¼�
void OnCreateProcessDbgEvent(CREATE_PROCESS_DEBUG_INFO cpdi);

// ��Ӧdll�������¼�
void OnLoadDllDbgEvent(LOAD_DLL_DEBUG_INFO lddi);

// ��Ӧdll��ж���¼�
void OnUnLoadDllDbgEvent(UNLOAD_DLL_DEBUG_INFO uddi);

// ��Ӧ�̴߳��������¼�
void OnCreateThreadDbgEvent(CREATE_THREAD_DEBUG_INFO ctdi);

// ��Ӧ�߳��˳������¼�
void OnExitThreadDbgEvent(EXIT_THREAD_DEBUG_INFO etdi);

// ��Ӧ�쳣�ϵ� -- int3�ϵ�
void OnExceptionBreakPoint(LPVOID lpAddr);

// �����
void DbgDisasm(LPVOID pAddr, size_t nLen);

// �����û�����
void Command(LPVOID lpAddr);

// ��������
void StepIn();

// ��������
void StepOver();

// ��ѯ�Ĵ���
void QueryReg();

// �޸ļĴ���
void ModifyReg(DWORD dwVar);

// ��ʾ�ڴ�
void ShowMemory(DWORD dwVar);

// �޸��ڴ�
void ModifyMemory(DWORD dwAddr,  DWORD bytes);

// �鿴ģ����Ϣ
void ListModule(std::vector<MMODULEINFO>& mModule);

// �鿴��ջ
void ShowStack();

// �޸Ļ�����
void ModifyDisasm(LPVOID addr);

// dump
void DumpExe();

// ���ص��Է���
void LoadSymbol(HANDLE hProcess, CREATE_PROCESS_DEBUG_INFO* pInfo);

// ��ȡ���Է��ź�������
const char* GetFunSymbolName(SIZE_T nAddress);

// ����PEB
void HidePEB();

// �����ϵ�
void SetConditionBp(HANDLE hProcess);

// ��ȡ������Ϣ
bool GetProcessList(vector<DBGPROCESSINFO>& vecProcessInfo);

// ����API��ַ
DWORD FindApiAddress(HANDLE hProcess, const char* pszName);

DWORD GetSymLine(DWORD dwAddr, DWORD& dwLine);

// ����Դ����
const char* ShowSrcCode(DWORD dwLine);
//// ע��dll����hook IsDebugParent;
//bool Inject(DWORD dwPid, LPTSTR pszDllPath);
#endif // !_CDBG_H_


