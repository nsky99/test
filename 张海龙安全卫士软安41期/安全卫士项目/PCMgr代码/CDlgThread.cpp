// CDlgThread.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgThread.h"
#include "afxdialogex.h"


// CDlgThread 对话框

IMPLEMENT_DYNAMIC(CDlgThread, CDialogEx)

CDlgThread::CDlgThread(CWnd* pParent /*=nullptr*/)
  : CDialogEx(IDD_DIALOG_THREAD, pParent)
{

}

CDlgThread::~CDlgThread()
{
}

void CDlgThread::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_THREAD, m_lsThread);
}


BEGIN_MESSAGE_MAP(CDlgThread, CDialogEx)
  ON_WM_CREATE()
  ON_COMMAND(IDCANCEL, &CDlgThread::OnIdcancel)
  ON_NOTIFY(NM_RCLICK, IDC_LIST_THREAD, &CDlgThread::OnRclickListThread)
    ON_COMMAND(ID_THREAD_TERMINATE, &CDlgThread::OnThreadTerminate)
  ON_COMMAND(ID_THREAD_SUSPEND, &CDlgThread::OnThreadSuspend)
  ON_COMMAND(ID_THREAD_RESUME, &CDlgThread::OnThreadResume)
  ON_COMMAND(ID_THREAD_FLUSH, &CDlgThread::OnThreadFlush)
END_MESSAGE_MAP()


// CDlgThread 消息处理程序


int CDlgThread::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CDialogEx::OnCreate(lpCreateStruct) == -1)
    return -1;
  AnimateWindow(250, AW_ACTIVATE | AW_VER_NEGATIVE);
  return 0;
}


void CDlgThread::OnIdcancel()
{
  AnimateWindow(250, AW_HIDE | AW_VER_POSITIVE);

  CDialogEx::OnCancel();
}


// 设置快照指针
void CDlgThread::SetInfo(CSnapShot* snapShot,DWORD dwPid)
{
  m_pSnapShot = snapShot;
  m_Pid = dwPid;
}

BOOL CDlgThread::OnInitDialog()
{
  CDialogEx::OnInitDialog();
  
  m_lsThread.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  CString strColumnName[] =
  {
      L"线程ID",
      L"进程ID",
      L"进程名称",
      L"优先级",
      L"状态",
  };
  CRect rc;
  m_lsThread.GetClientRect(rc);
  DWORD dwWidth = rc.right / _countof(strColumnName);

  for (size_t i = 0; i < _countof(strColumnName); i++)
  {
    m_lsThread.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
  }

  ShowThreadInfo();

  return TRUE;  // return TRUE unless you set the focus to a control
                // 异常: OCX 属性页应返回 FALSE
}

// 显示线程信息
void CDlgThread::ShowThreadInfo()
{
  m_lsThread.DeleteAllItems();
  m_pSnapShot->QueryProcessThread(m_Pid);
  for (size_t i = 0; i < m_pSnapShot->m_dwThreadCount; i++)
  {
    m_lsThread.InsertItem(i, L"");
  }
  for (size_t i = 0; i < m_pSnapShot->m_dwThreadCount; i++)
  {
    m_lsThread.SetItemText(i, 0, m_pSnapShot->m_vecTInfo[i].strTid);
    m_lsThread.SetItemText(i, 1, m_pSnapShot->m_vecTInfo[i].strPid);
    m_lsThread.SetItemText(i, 2, m_pSnapShot->m_vecTInfo[i].strProcessName);
    m_lsThread.SetItemText(i, 3, m_pSnapShot->m_vecTInfo[i].strPriority);
    m_lsThread.SetItemText(i, 4, m_pSnapShot->m_vecTInfo[i].strStaus);
  }
}


void CDlgThread::OnRclickListThread(NMHDR* pNMHDR, LRESULT* pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
  m_CurSel = pNMItemActivate->iItem;

  POINT point;
  GetCursorPos(&point);

  // 加载主菜单
  CMenu popMainMenu;
  popMainMenu.LoadMenuW(IDR_MENU_POP);

  CMenu* pSubMenu = popMainMenu.GetSubMenu(1);
  pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
  *pResult = 0;
}

// 终结线程
void CDlgThread::OnThreadTerminate()
{
  CString strTid;
  DWORD dwTid = 0;
  strTid = m_lsThread.GetItemText(m_CurSel, 0);
  dwTid = _wtoi(strTid);

  HANDLE hThread = OpenThread(THREAD_TERMINATE, 0, dwTid);
  if (0 == hThread)
  {
    MessageBoxW(L"进程无法终结", L"Error");
    return;
  }
  BOOL bRet = TerminateThread(hThread, 0);
  if (bRet == 0)
  {
    MessageBoxW(L"进程终结失败", L"Error");
    return;
  }
  m_lsThread.DeleteItem(m_CurSel);
  m_pSnapShot->m_dwThreadCount--;
  m_pSnapShot->m_vecTInfo.erase(m_pSnapShot->m_vecTInfo.begin() + m_CurSel);
  CloseHandle(hThread);
}

// 暂停线程
void CDlgThread::OnThreadSuspend()
{
  CString strTid;
  DWORD dwTid = 0;
  strTid = m_lsThread.GetItemText(m_CurSel, 0);
  dwTid = _wtoi(strTid);

  HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, 0, dwTid);
  if (0 == hThread)
  {
    MessageBoxW(L"进程无法暂停", L"Error");
    return;
  }
  SuspendThread(hThread);

  m_lsThread.SetItemText(m_CurSel, 4, L"暂停");
  CloseHandle(hThread);
}

// 恢复线程
void CDlgThread::OnThreadResume()
{
  CString strTid;
  DWORD dwTid = 0;
  strTid = m_lsThread.GetItemText(m_CurSel, 0);
  dwTid = _wtoi(strTid);

  HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, 0, dwTid);
  if (0 == hThread)
  {
    MessageBoxW(L"进程无法恢复", L"Error");
    return;
  }
  while (1 == ResumeThread(hThread));
  m_lsThread.SetItemText(m_CurSel, 4, L"运行");
  CloseHandle(hThread);
}

// 刷新线程显示
void CDlgThread::OnThreadFlush()
{
  ShowThreadInfo();
}
