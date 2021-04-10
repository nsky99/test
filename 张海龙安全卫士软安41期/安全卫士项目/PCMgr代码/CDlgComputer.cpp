// CDlgComputer.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgComputer.h"
#include "afxdialogex.h"
#include <Psapi.h>
#include <powrprof.h>
#pragma comment(lib,"PowrProf.lib")

// CDlgComputer 对话框

IMPLEMENT_DYNAMIC(CDlgComputer, CDialogEx)

CDlgComputer::CDlgComputer(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_COMPUTER, pParent)
{

}

CDlgComputer::~CDlgComputer()
{
  
}

void CDlgComputer::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_PROGRESS_CPU, m_progressCPU);
  DDX_Control(pDX, IDC_PROGRESS_MEM, m_progressMem);
  DDX_Control(pDX, IDC_STATIC_CPU, m_staticCPU);
  DDX_Control(pDX, IDC_STATIC_MEM, m_staticMem);
}


BEGIN_MESSAGE_MAP(CDlgComputer, CDialogEx)
  ON_BN_CLICKED(IDC_BUTTON_MEM, &CDlgComputer::OnBnClickedButtonMem)
  ON_COMMAND(IDCANCEL, &CDlgComputer::OnIdcancel)
  ON_COMMAND(IDOK, &CDlgComputer::OnIdok)
  ON_BN_CLICKED(IDC_BUTTON_SHUTDOWN, &CDlgComputer::OnBnClickedButtonShutdown)
  ON_BN_CLICKED(IDC_BUTTON_REBOOT, &CDlgComputer::OnBnClickedButtonReboot)
  ON_BN_CLICKED(IDC_BUTTON_SLEEP, &CDlgComputer::OnBnClickedButtonSleep)
  ON_BN_CLICKED(IDC_BUTTON_SLEEPED, &CDlgComputer::OnBnClickedButtonSleeped)
  ON_BN_CLICKED(IDC_BUTTON_LOCK, &CDlgComputer::OnBnClickedButtonLock)
  ON_BN_CLICKED(IDC_BUTTON_UNLOGN, &CDlgComputer::OnBnClickedButtonUnlogn)
END_MESSAGE_MAP()


// CDlgComputer 消息处理程序
// 获取当前Cpu的使用率 
int CDlgComputer::GetNowCpuUse()
{
  //文件时间 
  FILETIME idleTime, kernelTime, userTime;
  //1 处理时间 
  GetSystemTimes(&idleTime, &kernelTime, &userTime);
  //2 创建内核对象 等待事件 
  HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (hEvent == 0)
    return 0;
  WaitForSingleObject(hEvent, 1000);
  //3获取第二次处理器时间 
  FILETIME preideTime, prekernelTime, preuserTime;
  GetSystemTimes(&preideTime, &prekernelTime, &preuserTime);

  //空闲时间 转换 
  double idTime1 = (double)(idleTime.dwHighDateTime * 4.294967296E9)
    + (double)idleTime.dwLowDateTime;
  double preidTime2 = (double)(preideTime.dwHighDateTime * 4.294967296E9)
    + (double)preideTime.dwLowDateTime;

  //内核层使用时间 
  double kerTime1 = (double)(kernelTime.dwHighDateTime * 4.294967296E9)
    + (double)kernelTime.dwLowDateTime;
  double prekerTime2 = (double)(prekernelTime.dwHighDateTime * 4.294967296E9)
    + (double)prekernelTime.dwLowDateTime;
  //用户层使用时间 
  double useTime1 = (double)(userTime.dwHighDateTime * 4.294967296E9)
    + (double)userTime.dwLowDateTime;
  double preuseTime2 = (double)(preuserTime.dwHighDateTime * 4.294967296E9)
    + (double)preuserTime.dwLowDateTime;
  //转换成使用率 
  int use = (int)(100.0 - (preidTime2 - idTime1)
    / (prekerTime2 - kerTime1 + preuseTime2 - useTime1) * 100.0);
  CloseHandle(hEvent);
  return use;
}

//获取当前内存使用率 20%
int CDlgComputer::GetNowMemoryUse()
{
  MEMORYSTATUSEX memex = { sizeof(memex) };
  GlobalMemoryStatusEx(&memex);
  return memex.dwMemoryLoad;
}

DWORD WINAPI  SetCpuUsage(LPVOID lpThreadParameter)
{
  CDlgComputer* dlg = (CDlgComputer*)lpThreadParameter;
  CString strCpuUsage;
  while (true)
  {
    // CPU使用率
    int iCpuUsage = dlg->GetNowCpuUse();

    // 进度条进度
    dlg->m_progressCPU.SetPos(iCpuUsage);

    // 显示百分比
    strCpuUsage.Format(L"CPU使用率: %d%%", iCpuUsage);
    dlg->m_staticCPU.SetWindowTextW(strCpuUsage);
  }


  return 0;
}

DWORD WINAPI  SetMemUsage(LPVOID lpThreadParameter)
{
  CDlgComputer* dlg = (CDlgComputer*)lpThreadParameter;
  CString strMemUsage;
  while (true)
  {
    // CPU使用率
    int iMemUsage = dlg->GetNowMemoryUse();

    // 进度条进度
    dlg->m_progressMem.SetPos(iMemUsage);

    // 显示百分比
    strMemUsage.Format(L"内存使用率: %d%%", iMemUsage);
    dlg->m_staticMem.SetWindowTextW(strMemUsage);
    Sleep(1000);
  }
  return 0;
}


DWORD WINAPI  OptimizeMemory(LPVOID lpThreadParameter)
{

  CDlgComputer* dlg = (CDlgComputer*)lpThreadParameter;

  DWORD dwPIDList[1000] = {};
  DWORD bufSize = sizeof(dwPIDList);
  DWORD dwNeedSize = 0;
  // 获取所有进程
  K32EnumProcesses(dwPIDList, bufSize, &dwNeedSize);
  for (DWORD i = 0; i < dwNeedSize / sizeof(DWORD); i++)
  {
    HANDLE hProcess = OpenProcess(PROCESS_SET_QUOTA, false, dwPIDList[i]);
    // 清空工作内存
    SetProcessWorkingSetSize(hProcess, -1, -1);
    CloseHandle(hProcess);
  }

  return 0;
}

BOOL CDlgComputer::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  m_hTGetCPUUsage = CreateThread(0, 0, SetCpuUsage, this, 0, 0);

  m_hTGetMemUsage = CreateThread(0, 0, SetMemUsage, this, 0, 0);
  

  return TRUE;  // return TRUE unless you set the focus to a control
                // 异常: OCX 属性页应返回 FALSE
}


void CDlgComputer::OnBnClickedButtonMem()
{
  m_hTOptimizeMemory = CreateThread(0, 0, OptimizeMemory, this, 0, 0);
  if(m_hTOptimizeMemory)
    CloseHandle(m_hTOptimizeMemory);
}


void CDlgComputer::OnIdcancel()
{
  // TODO: 在此添加命令处理程序代码
}


void CDlgComputer::OnIdok()
{
  // TODO: 在此添加命令处理程序代码
}

// 关机
void CDlgComputer::OnBnClickedButtonShutdown()
{
  ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0);
}

// 重启
void CDlgComputer::OnBnClickedButtonReboot()
{
  ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
}

// 休眠
void CDlgComputer::OnBnClickedButtonSleep()
{
  SetSuspendState(TRUE, false, false);
}

// 睡眠
void CDlgComputer::OnBnClickedButtonSleeped()
{
  SetSuspendState(false, false, false);
}

// 锁屏
void CDlgComputer::OnBnClickedButtonLock()
{
  LockWorkStation();
}

// 注销
void CDlgComputer::OnBnClickedButtonUnlogn()
{
  ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 0);
}
