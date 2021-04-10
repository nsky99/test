// CDlgThread.cpp: 实现文件
//

#include "pch.h"
#include "ArkApp.h"
#include "CDlgThread.h"
#include "afxdialogex.h"
#include "ArkCommon.h"

// CDlgThread 对话框

IMPLEMENT_DYNAMIC(CDlgThread, CDialogEx)

CDlgThread::CDlgThread(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_THREAD, pParent),
	m_ulEProcess(0)
{

}

CDlgThread::CDlgThread(ULONG ulEProcess, CWnd* pParent)
	: CDialogEx(IDD_DLG_THREAD, pParent),
	m_ulEProcess(ulEProcess)
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
	ON_COMMAND(ID_THREAD_SUSPEND, &CDlgThread::OnThreadSuspend)
	ON_COMMAND(ID_THREAD_RESUME, &CDlgThread::OnThreadResume)
	ON_COMMAND(ID_THREAD_KILL, &CDlgThread::OnThreadKill)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_THREAD, &CDlgThread::OnNMRClickListThread)
	ON_COMMAND(ID_THREAD_FLUSH, &CDlgThread::OnThreadFlush)
END_MESSAGE_MAP()


// CDlgThread 消息处理程序

// 响应初始化对话框事件
BOOL CDlgThread::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsThread.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CString strColumnName[] = {
	L"Tid",
	L"ETHREAD",
	};

	m_lsThread.GetClientRect(rc);
	DWORD dwWidth = rc.right / _countof(strColumnName) - 2;

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
	// 清空列表
	m_lsThread.DeleteAllItems();

	// 通过IO发送数据给驱动，让驱动查询指定EPROCESS的线程个数
	DWORD dwRet = 0;
	DWORD dwThreadNum = 0;
	::DeviceIoControl(theApp.m_hArk, ArkCountThread,
		&m_ulEProcess, sizeof(DWORD), &dwThreadNum, sizeof(DWORD), &dwRet, NULL);

	CStringW strTrdNum;
	strTrdNum.Format(L"线程个数：%u\n", dwThreadNum);
	OutputDebugStringW(strTrdNum);
	SetWindowTextW(strTrdNum);


	// 查询线程信息
	DWORD dwMemSize = sizeof(THREAD_INFO) * dwThreadNum;
	PTHREAD_INFO pTrdInfo = new THREAD_INFO[dwThreadNum]{ 0 };
	::DeviceIoControl(theApp.m_hArk, ArkQueryThread,
		&m_ulEProcess, sizeof(DWORD), pTrdInfo, dwMemSize, &dwRet, NULL);

	// 向列表框中输入信息
	for (size_t i = 0; i < dwThreadNum; i++)
	{
		m_lsThread.InsertItem(i, L"");
	}


	// 显示线程信息
	WCHAR szTmp[MAX_PATH] = { 0 };
	for (size_t i = 0; i < dwThreadNum; i++)
	{
		// tid
		wsprintfW(szTmp, L"%u", pTrdInfo[i].uTid);
		m_lsThread.SetItemText(i, 0, szTmp);
		// EPROCESS
		wsprintfW(szTmp, L"0x%08X", pTrdInfo[i].uEThread);
		m_lsThread.SetItemText(i, 1, szTmp);
	}

	// 释放内存
	if (pTrdInfo)
	{
		delete[] pTrdInfo;
		pTrdInfo = NULL;
	}
}


// 响应鼠标右键消息 - 右键单击线程列表框
void CDlgThread::OnNMRClickListThread(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	POINT point;
	GetCursorPos(&point);

	// 加载主菜单
	CMenu popMainMenu;
	popMainMenu.LoadMenuW(IDR_MENU1);

	CMenu* pSubMenu = popMainMenu.GetSubMenu(1);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

	*pResult = 0;
}


// 响应菜单消息 - 暂停线程
void CDlgThread::OnThreadSuspend()
{
		// 1 获取ETHREAD
	ULONG ulETrd = 0;
	// 获取当前选择行
	UINT uCurSelected = m_lsThread.GetSelectionMark();
	// 获取当前选中行的EPROCESS
	CString strEprocess = m_lsThread.GetItemText(uCurSelected, 1);

	swscanf_s(strEprocess.GetString(), L"0x%08X", &ulETrd);


	// 2 通过IO发送数据给驱动，让驱动暂停线程
	DWORD dwRet = 0;
	::DeviceIoControl(theApp.m_hArk, ArkSuspendThread, &ulETrd, sizeof(ULONG), NULL, 0, &dwRet, NULL);

	if (0 == GetLastError())
	{
		MessageBoxW(L"目标线程已经暂停");
	}
	else
	{
		MessageBoxW(L"目标线程未暂停");
	}

	OnThreadFlush();
}


// 响应菜单消息 - 恢复线程
void CDlgThread::OnThreadResume()
{
	// 1 获取ETHREAD
	ULONG ulETrd = 0;
	// 获取当前选择行
	UINT uCurSelected = m_lsThread.GetSelectionMark();
	// 获取当前选中行的EPROCESS
	CString strEprocess = m_lsThread.GetItemText(uCurSelected, 1);

	swscanf_s(strEprocess.GetString(), L"0x%08X", &ulETrd);


	// 2 通过IO发送数据给驱动，让驱动恢复线程
	DWORD dwRet = 0;
	::DeviceIoControl(theApp.m_hArk, ArkResumeThread, &ulETrd, sizeof(ULONG), NULL, 0, &dwRet, NULL);

	if (0 == GetLastError())
	{
		MessageBoxW(L"目标线程已经恢复");
	}
	else
	{
		MessageBoxW(L"目标线程未恢复");
	}

	OnThreadFlush();
}


// 响应菜单消息 - 结束线程
void CDlgThread::OnThreadKill()
{
	// 1 获取ETHREAD
	ULONG ulETrd = 0;
	// 获取当前选择行
	UINT uCurSelected = m_lsThread.GetSelectionMark();
	// 获取当前选中行的EPROCESS
	CString strEprocess = m_lsThread.GetItemText(uCurSelected, 1);

	swscanf_s(strEprocess.GetString(), L"0x%08X", &ulETrd);


	// 2 通过IO发送数据给驱动，让驱动结束线程
	DWORD dwRet = 0;
	::DeviceIoControl(theApp.m_hArk, ArkKillThread, &ulETrd, sizeof(ULONG), NULL, 0, &dwRet, NULL);

	if (0 == GetLastError())
	{
		MessageBoxW(L"目标线程已经结束");
	}
	else
	{
		MessageBoxW(L"目标线程未结束");
	}

}


// 响应菜单消息 - 刷新线程
void CDlgThread::OnThreadFlush()
{
	ShowThreadInfo();
}
