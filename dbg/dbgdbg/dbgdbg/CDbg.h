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
// 进程信息
typedef struct _DBGPROCESSINFO
{
  DWORD  dwPid;           // 进程id
  string strProcessName;  // 进程名称
}DBGPROCESSINFO, * PDBGPROCESSINFO;
// 调试器主菜单
typedef enum _MAINMENU
{
  OPEN = '1',
  ATTACH,
  EXIT
}MAINMENU;

// 加载调试符号结构体信息
typedef struct _SYMLOADINFO
{
  DWORD64 base;
  HANDLE  hFile;
}SYMLOADINFO, * PSYMLOADINFO;
// 调试器主循环
void MainLoop();

// 打开调试进程
bool OpenDbgProcess(char* pszFile);

// 附加调试进程
bool AttachDbgProcess(DWORD dwPid);

// 调试事件主循环
DWORD DbgEventLoop();

// 响应异常调试事件
void OnExceptionDbgEvent(EXCEPTION_DEBUG_INFO exceptDbgInfo);

// 响应进程创建调试事件
void OnCreateProcessDbgEvent(CREATE_PROCESS_DEBUG_INFO cpdi);

// 响应dll被加载事件
void OnLoadDllDbgEvent(LOAD_DLL_DEBUG_INFO lddi);

// 响应dll被卸载事件
void OnUnLoadDllDbgEvent(UNLOAD_DLL_DEBUG_INFO uddi);

// 响应线程创建调试事件
void OnCreateThreadDbgEvent(CREATE_THREAD_DEBUG_INFO ctdi);

// 响应线程退出调试事件
void OnExitThreadDbgEvent(EXIT_THREAD_DEBUG_INFO etdi);

// 响应异常断点 -- int3断点
void OnExceptionBreakPoint(LPVOID lpAddr);

// 反汇编
void DbgDisasm(LPVOID pAddr, size_t nLen);

// 接收用户命令
void Command(LPVOID lpAddr);

// 单步步入
void StepIn();

// 单步步过
void StepOver();

// 查询寄存器
void QueryReg();

// 修改寄存器
void ModifyReg(DWORD dwVar);

// 显示内存
void ShowMemory(DWORD dwVar);

// 修改内存
void ModifyMemory(DWORD dwAddr,  DWORD bytes);

// 查看模块信息
void ListModule(std::vector<MMODULEINFO>& mModule);

// 查看堆栈
void ShowStack();

// 修改汇编代码
void ModifyDisasm(LPVOID addr);

// dump
void DumpExe();

// 加载调试符号
void LoadSymbol(HANDLE hProcess, CREATE_PROCESS_DEBUG_INFO* pInfo);

// 获取调试符号函数名称
const char* GetFunSymbolName(SIZE_T nAddress);

// 隐藏PEB
void HidePEB();

// 条件断点
void SetConditionBp(HANDLE hProcess);

// 获取进程信息
bool GetProcessList(vector<DBGPROCESSINFO>& vecProcessInfo);

// 查找API地址
DWORD FindApiAddress(HANDLE hProcess, const char* pszName);

DWORD GetSymLine(DWORD dwAddr, DWORD& dwLine);

// 返回源代码
const char* ShowSrcCode(DWORD dwLine);
//// 注入dll进行hook IsDebugParent;
//bool Inject(DWORD dwPid, LPTSTR pszDllPath);
#endif // !_CDBG_H_


