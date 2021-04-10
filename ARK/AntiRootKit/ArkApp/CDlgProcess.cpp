// CDlgProcess.cpp: 实现文件
//

#include "pch.h"
#include "ArkApp.h"
#include "afxdialogex.h"
#include "CDlgProcess.h"
#include "CDlgModule.h"
#include "CDlgThread.h"
#include "ArkCommon.h"

// CDlgProcess 对话框

IMPLEMENT_DYNAMIC(CDlgProcess, CDialogEx)

CDlgProcess::CDlgProcess(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_PROCESS, pParent)
{

}

CDlgProcess::~CDlgProcess()
{
	//// UnHook
	//DWORD dwRet = 0;
	//::DeviceIoControl(theApp.m_hArk, ArkUnHookSysEnter, NULL, 0, NULL, 0, &dwRet, NULL);
	MessageBox(0);
}

void CDlgProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_lsProcess);
}


BEGIN_MESSAGE_MAP(CDlgProcess, CDialogEx)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS, &CDlgProcess::OnNMRClickListProcess)
	ON_COMMAND(ID_PROCESS_FLUSH, &CDlgProcess::OnProcessFlush)
	ON_COMMAND(ID_PROCESS_SUSPEND, &CDlgProcess::OnProcessSuspend)
	ON_COMMAND(ID_PROCESS_RESUME, &CDlgProcess::OnProcessResume)
	ON_COMMAND(ID_PROCESS_KILL, &CDlgProcess::OnProcessKill)
	ON_COMMAND(ID_PROCESS_HIDE, &CDlgProcess::OnProcessHide)
	ON_COMMAND(ID_PROCESS_SEARSH_THREAD, &CDlgProcess::OnProcessSearshThread)
	ON_COMMAND(ID_PROCESS_SEARSH_MODULE, &CDlgProcess::OnProcessSearshModule)
	ON_COMMAND(ID_PROCESS_SAFE, &CDlgProcess::OnProcessSafe)
END_MESSAGE_MAP()


// CDlgProcess 消息处理程序


BOOL CDlgProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsProcess.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CString strColumnName[] = {
	L"进程名称",
	L"进程ID",
	L"父进程ID",
	L"进程路径",
	L"EPROCESS"
	};

	m_lsProcess.GetClientRect(rc);
	DWORD dwWidth = rc.right / _countof(strColumnName) - 2;

	for (size_t i = 0; i < _countof(strColumnName); i++)
	{
		m_lsProcess.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
	}

	// 显示进程信息
	ShowProcessInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}


// 显示进程信息
void CDlgProcess::ShowProcessInfo()
{
	// 清空列表框
	m_lsProcess.DeleteAllItems();

	// 1. 查询进程个数
	DWORD dwProcessNum = 0;
	DWORD dwRet = 0;
	::DeviceIoControl(theApp.m_hArk, ArkCountProcess, NULL, 0, &dwProcessNum, sizeof(DWORD), &dwRet, NULL);

	CStringW strProcessNum;
	strProcessNum.Format(L"进程个数：%u\n", dwProcessNum);
	OutputDebugStringW(strProcessNum);

	// 接收进程信息所需内存
	ULONG ulAllcateMemSize = sizeof(PROCESS_INFO) * dwProcessNum;
	PPROCESS_INFO pProcInfo = new PROCESS_INFO[dwProcessNum]{ 0 };
	if (!pProcInfo)
	{
		return;
	}
	
	// 通过R0获取进程信息
	DWORD dwRetMemSize = 0;
	::DeviceIoControl(theApp.m_hArk, ArkQueryProcess, NULL, 0, pProcInfo, ulAllcateMemSize, &dwRetMemSize, NULL);

	// 显示进程信息
	//for (size_t i = 0; i < dwProcessNum; i++)
	//{
	//	OutputDebugStringW(pProcInfo[i].szName);
	//}

	// 向列表框中输入信息
	for (size_t i = 0; i < dwProcessNum; i++)
	{
		m_lsProcess.InsertItem(i, L"");
	}
	WCHAR szTmp[MAX_PATH] = { 0 };
	for (size_t i = 0; i < dwProcessNum; i++)
	{
		// name
		m_lsProcess.SetItemText(i, 0, pProcInfo[i].szName);
		// pid
		wsprintfW(szTmp, L"%u", pProcInfo[i].uPid);
		m_lsProcess.SetItemText(i, 1, szTmp);
		// ppid
		wsprintfW(szTmp, L"%u", pProcInfo[i].uParentPid);
		m_lsProcess.SetItemText(i, 2, szTmp);
		// path
		m_lsProcess.SetItemText(i, 3, pProcInfo[i].szPath);
		// EPROCESS
		wsprintfW(szTmp, L"0x%08X", pProcInfo[i].uEprocess);
		m_lsProcess.SetItemText(i, 4, szTmp);
	}

	// 释放内存
	if (pProcInfo)
	{
		delete[] pProcInfo;
		pProcInfo = NULL;
	}
}


// 右键点击事件 - 弹出进程菜单
void CDlgProcess::OnNMRClickListProcess(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	POINT point;
	GetCursorPos(&point);

	// 加载主菜单
	CMenu popMainMenu;
	popMainMenu.LoadMenuW(IDR_MENU1);

	CMenu* pSubMenu = popMainMenu.GetSubMenu(0);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	
	*pResult = 0;
}


// 响应菜单 - 进程刷新
void CDlgProcess::OnProcessFlush()
{
	ShowProcessInfo();
}


// 响应菜单 - 进程暂停
void CDlgProcess::OnProcessSuspend()
{
	// 1 获取EPROCESS
	ULONG uEprocess = 0;
	// 获取当前选择行
	UINT uCurSelected = m_lsProcess.GetSelectionMark();
	// 获取当前选中行的EPROCESS
	CString strEprocess = m_lsProcess.GetItemText(uCurSelected, 4);

	swscanf_s(strEprocess.GetString(), L"0x%08X", &uEprocess);
	

	// 2 通过IO发送数据给驱动，让驱动暂停进程
	DWORD dwRet = 0;
	::DeviceIoControl(theApp.m_hArk, ArkSuspendProcess, &uEprocess, sizeof(ULONG), NULL, 0, &dwRet, NULL);

	if (0 == GetLastError())
	{
		MessageBoxW(L"目标进程已经暂停");
	}
	else
	{
		MessageBoxW(L"目标进程未暂停");
	}

	OnProcessFlush();
}


// 响应菜单 - 进程恢复
void CDlgProcess::OnProcessResume()
{
	// 1 获取EPROCESS
	ULONG uEprocess = 0;
	// 获取当前选择行
	UINT uCurSelected = m_lsProcess.GetSelectionMark();
	// 获取当前选中行的EPROCESS
	CString strEprocess = m_lsProcess.GetItemText(uCurSelected, 4);

	swscanf_s(strEprocess.GetString(), L"0x%08X", &uEprocess);


	// 2 通过IO发送数据给驱动，让驱动恢复进程
	DWORD dwRet = 0;
	::DeviceIoControl(theApp.m_hArk, ArkResumeProcess, &uEprocess, sizeof(ULONG), NULL, 0, &dwRet, NULL);

	if (0 == GetLastError())
	{
		MessageBoxW(L"目标进程已经恢复");
	}
	else
	{
		MessageBoxW(L"目标进程未恢复");
	}
	OnProcessFlush();
}


// 响应菜单 - 进程结束
void CDlgProcess::OnProcessKill()
{
	// 1 获取pid
	ULONG ulPid = 0;
	// 获取当前选择行
	UINT uCurSelected = m_lsProcess.GetSelectionMark();
	// 获取当前选中行的EPROCESS
	CString strEprocess = m_lsProcess.GetItemText(uCurSelected, 1);

	swscanf_s(strEprocess.GetString(), L"%u", &ulPid);


	// 2 通过IO发送数据给驱动，让驱动结束进程
	DWORD dwRet = 0;
	::DeviceIoControl(theApp.m_hArk, ArkKillProcess, &ulPid, sizeof(ULONG), NULL, 0, &dwRet, NULL);

	if (0 == GetLastError())
	{
		MessageBoxW(L"目标进程已经结束");
	}
	else
	{
		MessageBoxW(L"目标进程未结束");
	}
	OnProcessFlush();
}


// 响应菜单 - 进程隐藏
void CDlgProcess::OnProcessHide()
{
	// 1 获取EPROCESS
	ULONG uEprocess = 0;
	// 获取当前选择行
	UINT uCurSelected = m_lsProcess.GetSelectionMark();
	// 获取当前选中行的EPROCESS
	CString strEprocess = m_lsProcess.GetItemText(uCurSelected, 4);
	swscanf_s(strEprocess.GetString(), L"0x%08X", &uEprocess);

	// 2 通过IO发送数据给驱动，让驱动隐藏进程
	DWORD dwRet = 0;
	::DeviceIoControl(theApp.m_hArk, ArkHideProcess, &uEprocess, sizeof(ULONG), NULL, 0, &dwRet, NULL);

	if (0 == GetLastError())
	{
		MessageBoxW(L"目标进程已经隐藏");
	}
	else
	{
		MessageBoxW(L"目标进程未隐藏");
	}
}


// 响应菜单 - 查看进程线程
void CDlgProcess::OnProcessSearshThread()
{
	// 1 获取EPROCESS
	ULONG uEprocess = 0;
	// 获取当前选择行
	UINT uCurSelected = m_lsProcess.GetSelectionMark();
	// 获取当前选中行的EPROCESS
	CString strEprocess = m_lsProcess.GetItemText(uCurSelected, 4);
	swscanf_s(strEprocess.GetString(), L"0x%08X", &uEprocess);

	// 创建线程窗口 - 用于显示线程信息
	CDlgThread* pTrdDlg = new CDlgThread(uEprocess);
	pTrdDlg->DoModal();
	delete pTrdDlg;
}


// 响应菜单 - 查看进程模块
void CDlgProcess::OnProcessSearshModule()
{
	// 1 获取EPROCESS
	ULONG uEprocess = 0;
	// 获取当前选择行
	UINT uCurSelected = m_lsProcess.GetSelectionMark();
	// 获取当前选中行的EPROCESS
	CString strEprocess = m_lsProcess.GetItemText(uCurSelected, 4);
	swscanf_s(strEprocess.GetString(), L"0x%08X", &uEprocess);

	// 创建模块窗口 - 用于显示模块信息
	CDlgModule* pModuleDlg = new CDlgModule(uEprocess);
	pModuleDlg->DoModal();
	delete pModuleDlg;
}


// 响应菜单 - 保护进程
void CDlgProcess::OnProcessSafe()
{
	// 1 获取pid
	ULONG ulPid = 0;
	UINT uCurSelected = m_lsProcess.GetSelectionMark();
	CString strEprocess = m_lsProcess.GetItemText(uCurSelected, 1);
	swscanf_s(strEprocess.GetString(), L"%u", &ulPid);

	// io请求 - 保护进程
	DWORD dwRet = 0;
	::DeviceIoControl(theApp.m_hArk, ArkHookSysEnter, &ulPid, sizeof(ULONG), NULL, 0, &dwRet, NULL);
}
