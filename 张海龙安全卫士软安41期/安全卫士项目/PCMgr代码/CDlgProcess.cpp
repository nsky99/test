// CDlgProcess.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgProcess.h"
#include "afxdialogex.h"
#include <TlHelp32.h>
// CDlgProcess 对话框

IMPLEMENT_DYNAMIC(CDlgProcess, CDialogEx)

CDlgProcess::CDlgProcess(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PROCESS, pParent)
{

}

CDlgProcess::~CDlgProcess()
{
}

void CDlgProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_lsProcess);
}


BEGIN_MESSAGE_MAP(CDlgProcess, CDialogEx)
	ON_COMMAND(IDOK, &CDlgProcess::OnIdok)
	ON_COMMAND(IDCANCEL, &CDlgProcess::OnIdcancel)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS, &CDlgProcess::OnRclickListProcess)
	ON_COMMAND(ID_FLUSH_PROCESS, &CDlgProcess::OnFlushProcess)
	ON_COMMAND(ID_PROCESS_TERMINATE, &CDlgProcess::OnProcessTerminate)
	ON_COMMAND(ID_PROCESS_STOP, &CDlgProcess::OnProcessStop)
	ON_COMMAND(ID_PROCESS_RESUME, &CDlgProcess::OnProcessResume)
	ON_COMMAND(ID_REALTIME_PRIORITY_CLASS, &CDlgProcess::OnRealtimePriorityClass)
	ON_COMMAND(ID_HIGH_PRIORITY_CLASS, &CDlgProcess::OnHighPriorityClass)
	ON_COMMAND(ID_ABOVE_NORMAL_PRIORITY_CLASS, &CDlgProcess::OnAboveNormalPriorityClass)
	ON_COMMAND(ID_NORMAL_PRIORITY_CLASS, &CDlgProcess::OnNormalPriorityClass)
	ON_COMMAND(ID_BELOW_NORMAL_PRIORITY_CLASS, &CDlgProcess::OnBelowNormalPriorityClass)
	ON_COMMAND(ID_IDLE_PRIORITY_CLASS, &CDlgProcess::OnIdlePriorityClass)
	ON_COMMAND(ID_OPEN_PATH, &CDlgProcess::OnOpenPath)
	ON_COMMAND(ID_ATTRITUBE, &CDlgProcess::OnAttritube)
	ON_COMMAND(ID_SHOW_THREAD, &CDlgProcess::OnShowThread)
	ON_COMMAND(ID_SHOW_MODLE, &CDlgProcess::OnShowModle)
	ON_COMMAND(ID_SHOW_HEAPLIST, &CDlgProcess::OnShowHeaplist)
	ON_COMMAND(ID_PROTECTPROCESS, &CDlgProcess::OnProtectprocess)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CDlgProcess 消息处理程序


BOOL CDlgProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 初始化进程列表框
	InitProcessList();
	m_SnapShot.QueryAllProcess();

	// 显示进程信息
	ShowProcessInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}


void CDlgProcess::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}


void CDlgProcess::OnIdcancel()
{
	// TODO: 在此添加命令处理程序代码
}


void CDlgProcess::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_lsProcess.GetSafeHwnd())
	{
		CRect rc;
		GetClientRect(rc);
		m_lsProcess.MoveWindow(rc);
	}
}


// 初始化进程列表框
void CDlgProcess::InitProcessList()
{
	CRect rc;
	m_lsProcess.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CString strColumnName[] = {
		L"进程名称",
		L"PID",
		L"工作集(内存)",
		L"优先级",
		L"句柄数",
		L"线程数",
		L"父进程ID",
		L"路径名称"
	};
	m_lsProcess.GetClientRect(rc);
	DWORD dwWidth = rc.right / _countof(strColumnName);

	for (size_t i = 0; i < _countof(strColumnName) - 1; i++)
	{
		m_lsProcess.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
	}
	m_lsProcess.InsertColumn(7, strColumnName[7], LVCFMT_LEFT, 270);

}


// 显示进程信息
void CDlgProcess::ShowProcessInfo()
{
	m_lsProcess.DeleteAllItems();
	m_SnapShot.QueryAllProcess();
	for (size_t i = 0; i < m_SnapShot.m_dwProcessCount; i++)
	{
		m_lsProcess.InsertItem(i, L"");
	}
	for (size_t i = 0; i < m_SnapShot.m_dwProcessCount; i++)
	{

		m_lsProcess.SetItemText(i, 0, m_SnapShot.m_vecPInfo[i].strProcessName);
		m_lsProcess.SetItemText(i, 1, m_SnapShot.m_vecPInfo[i].strPid);
		m_lsProcess.SetItemText(i, 2, m_SnapShot.m_vecPInfo[i].strWorkingSet);
		m_lsProcess.SetItemText(i, 3, m_SnapShot.m_vecPInfo[i].strPriority);
		m_lsProcess.SetItemText(i, 4, m_SnapShot.m_vecPInfo[i].strHandleNumber);
		m_lsProcess.SetItemText(i, 5, m_SnapShot.m_vecPInfo[i].strThreadNumber);
		m_lsProcess.SetItemText(i, 6, m_SnapShot.m_vecPInfo[i].strFatherPid);
		m_lsProcess.SetItemText(i, 7, m_SnapShot.m_vecPInfo[i].strProcessPath);
	}
}


// 右键点击列表框
void CDlgProcess::OnRclickListProcess(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	m_CurSel = pNMItemActivate->iItem;

	POINT point;
	GetCursorPos(&point);
	
	// 加载主菜单
	CMenu popMainMenu;
	popMainMenu.LoadMenuW(IDR_MENU_POP);

	CMenu* pSubMenu = popMainMenu.GetSubMenu(0);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	*pResult = 0;
}


/////////////进程弹出菜单//////////////////

// 刷新进程列表信息
void CDlgProcess::OnFlushProcess()
{
	ShowProcessInfo();
}

// 销毁进程
void CDlgProcess::OnProcessTerminate()
{
	CString strPid;
	DWORD dwPid;
	// 获取进程ID
	strPid = m_lsProcess.GetItemText(m_CurSel, 1);
	// 字符串转整数
	dwPid = _wtoi(strPid);
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, dwPid);
	if (0 == hProcess)
	{
		MessageBoxW(L"进程终结失败", L"Error", MB_ICONERROR);
		return;
	}
	DWORD dwChoose = MessageBoxW(L"确定要关闭进程?", L"Tip", MB_OKCANCEL);
	if (dwChoose == IDOK)
	{
		if (TerminateProcess(hProcess, 0))
		{
			m_lsProcess.DeleteItem(m_CurSel);
			m_SnapShot.m_dwProcessCount--;
			m_SnapShot.m_vecPInfo.erase(m_SnapShot.m_vecPInfo.begin() + m_CurSel);
		}
		else
		{
			MessageBoxW(L"拒绝访问", L"Error", MB_ICONERROR);
		}
		CloseHandle(hProcess);
	}
	else if (dwChoose == IDCANCEL)
	{
		CloseHandle(hProcess);
	}
}

// 暂停进程
void CDlgProcess::OnProcessStop()
{
	CString strPid;
	DWORD dwPid;
	// 获取进程ID
	strPid = m_lsProcess.GetItemText(m_CurSel, 1);
	// 字符串转整数
	dwPid = _wtoi(strPid);
	HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, 0, dwPid);
	if (0 == hProcess)
	{
		MessageBoxW(L"进程暂停失败", L"Error", MB_ICONERROR);
		return;
	}

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	THREADENTRY32 te{};
	te.dwSize = sizeof(te);

	if (Thread32First(hSnapshot, &te))
	{
		do
		{
			if (te.th32OwnerProcessID == dwPid)
			{
				HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, false, te.th32ThreadID);
				if (hThread)
				{
					SuspendThread(hThread);
					CloseHandle(hThread);
				}
			}
		} while (Thread32Next(hSnapshot, &te));
	}
	CloseHandle(hProcess);
	CloseHandle(hSnapshot);
}

// 恢复进程
void CDlgProcess::OnProcessResume()
{
	CString strPid;
	DWORD dwPid;
	// 获取进程ID
	strPid = m_lsProcess.GetItemText(m_CurSel, 1);
	// 字符串转整数
	dwPid = _wtoi(strPid);
	HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, 0, dwPid);
	if (0 == hProcess)
	{
		MessageBoxW(L"进程恢复失败", L"Error", MB_ICONERROR);
		return;
	}

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	THREADENTRY32 te{};
	te.dwSize = sizeof(te);

	if (Thread32First(hSnapshot, &te))
	{
		do
		{
			if (te.th32OwnerProcessID == dwPid)
			{
				HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, false, te.th32ThreadID);
				if (hThread)
				{
					ResumeThread(hThread);
					CloseHandle(hThread);
				}
			}
		} while (Thread32Next(hSnapshot, &te));
	}
	CloseHandle(hProcess);
	CloseHandle(hSnapshot);
}

// 设置进程优先级实时
void CDlgProcess::OnRealtimePriorityClass()
{
	CString strPid;
	DWORD dwPid;
	// 获取进程ID
	strPid = m_lsProcess.GetItemText(m_CurSel, 1);
	// 字符串转整数
	dwPid = _wtoi(strPid);
	HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, 0, dwPid);
	if (0 == hProcess)
	{
		MessageBoxW(L"打开进程失败", L"Error", MB_ICONERROR);
		return;
	}
	if(!SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS))
		MessageBoxW(L"设置优先级失败",L"Error", MB_ICONERROR);
	else
	{
		m_lsProcess.SetItemText(m_CurSel, 3, L"实时");
	}
	CloseHandle(hProcess);
}

// 设置进程优先级高
void CDlgProcess::OnHighPriorityClass()
{
	CString strPid;
	DWORD dwPid;
	// 获取进程ID
	strPid = m_lsProcess.GetItemText(m_CurSel, 1);
	// 字符串转整数
	dwPid = _wtoi(strPid);
	HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, 0, dwPid);
	if (0 == hProcess)
	{
		MessageBoxW(L"打开进程失败", L"Error", MB_ICONERROR);
		return;
	}
	if (!SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
		MessageBoxW(L"设置优先级失败", L"Error", MB_ICONERROR);
	else
	{
		m_lsProcess.SetItemText(m_CurSel, 3, L"高");
	}
	CloseHandle(hProcess);
}

// 设置进程优先级高于正常
void CDlgProcess::OnAboveNormalPriorityClass()
{
	CString strPid;
	DWORD dwPid;
	// 获取进程ID
	strPid = m_lsProcess.GetItemText(m_CurSel, 1);
	// 字符串转整数
	dwPid = _wtoi(strPid);
	HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, 0, dwPid);
	if (0 == hProcess)
	{
		MessageBoxW(L"打开进程失败", L"Error", MB_ICONERROR);
		return;
	}
	if (!SetPriorityClass(hProcess, ABOVE_NORMAL_PRIORITY_CLASS))
		MessageBoxW(L"设置优先级失败", L"Error", MB_ICONERROR);
	else
	{
		m_lsProcess.SetItemText(m_CurSel, 3, L"高于正常");
	}
	CloseHandle(hProcess);
}

// 设置进程优先级正常
void CDlgProcess::OnNormalPriorityClass()
{
	CString strPid;
	DWORD dwPid;
	// 获取进程ID
	strPid = m_lsProcess.GetItemText(m_CurSel, 1);
	// 字符串转整数
	dwPid = _wtoi(strPid);
	HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, 0, dwPid);
	if (0 == hProcess)
	{
		MessageBoxW(L"打开进程失败", L"Error", MB_ICONERROR);
		return;
	}
	if (!SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS))
		MessageBoxW(L"设置优先级失败", L"Error", MB_ICONERROR);
	else
	{
		m_lsProcess.SetItemText(m_CurSel, 3, L"正常");
	}
	CloseHandle(hProcess);
}

// 设置进程优先级低于正常
void CDlgProcess::OnBelowNormalPriorityClass()
{
	CString strPid;
	DWORD dwPid;
	// 获取进程ID
	strPid = m_lsProcess.GetItemText(m_CurSel, 1);
	// 字符串转整数
	dwPid = _wtoi(strPid);
	HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, 0, dwPid);
	if (0 == hProcess)
	{
		MessageBoxW(L"打开进程失败", L"Error", MB_ICONERROR);
		return;
	}
	if (!SetPriorityClass(hProcess, BELOW_NORMAL_PRIORITY_CLASS))
		MessageBoxW(L"设置优先级失败", L"Error", MB_ICONERROR);
	else
	{
		m_lsProcess.SetItemText(m_CurSel, 3, L"低于正常");
	}
	CloseHandle(hProcess);
}

// 设置进程优先级低
void CDlgProcess::OnIdlePriorityClass()
{
	CString strPid;
	DWORD dwPid;
	// 获取进程ID
	strPid = m_lsProcess.GetItemText(m_CurSel, 1);
	// 字符串转整数
	dwPid = _wtoi(strPid);
	HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, 0, dwPid);
	if (0 == hProcess)
	{
		MessageBoxW(L"打开进程失败", L"Error", MB_ICONERROR);
		return;
	}
	if (!SetPriorityClass(hProcess, IDLE_PRIORITY_CLASS))
		MessageBoxW(L"设置优先级失败", L"Error", MB_ICONERROR);
	else
	{
		m_lsProcess.SetItemText(m_CurSel, 3, L"低");
	}
	CloseHandle(hProcess);
}

// 打开进程所在位置
void CDlgProcess::OnOpenPath()
{
	CString szExeName = m_lsProcess.GetItemText(m_CurSel, 7);
	if(szExeName.IsEmpty())
	{
		MessageBoxW(L"打开文件失败", L"Error", MB_ICONERROR);
		return;
	}
	LPITEMIDLIST nItem;
	nItem = ILCreateFromPathW(szExeName) ;
	SHOpenFolderAndSelectItems(nItem, 0, 0, 0);
	ILFree(nItem);
}

// 打开文件属性
void CDlgProcess::OnAttritube()
{
	
	CString szExeName = m_lsProcess.GetItemText(m_CurSel, 7);
	SHELLEXECUTEINFO   sei = { 0 };
	sei.hwnd = NULL;
	sei.lpVerb = L"properties";
	sei.lpFile = szExeName;
	sei.lpDirectory = NULL;
	sei.lpParameters = NULL;
	sei.nShow = SW_SHOWNORMAL;
	sei.fMask = SEE_MASK_INVOKEIDLIST;
	sei.lpIDList = NULL;
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	ShellExecuteExW(&sei);
}

// 显示线程信息
void CDlgProcess::OnShowThread()
{
	CString strPid;
	DWORD dwPid;
	// 获取进程ID
	strPid = m_lsProcess.GetItemText(m_CurSel, 1);
	// 字符串转整数
	dwPid = _wtoi(strPid);
	m_pDlgThread = new CDlgThread;
	m_pDlgThread->SetInfo(&m_SnapShot, dwPid);
	m_pDlgThread->DoModal();
	if (m_pDlgThread)
	{
		delete m_pDlgThread;
		m_pDlgThread = 0;
	}
}


void CDlgProcess::OnShowModle()
{
	CString strPid;
	DWORD dwPid;
	// 获取进程ID
	strPid = m_lsProcess.GetItemText(m_CurSel, 1);
	// 字符串转整数
	dwPid = _wtoi(strPid);

	m_pDlgModel = new CDlgModel;
	m_pDlgModel->SetInfo(&m_SnapShot, dwPid);
	m_pDlgModel->DoModal();
	if (m_pDlgModel)
	{
		delete m_pDlgModel;
		m_pDlgModel = 0;
	}
}


void CDlgProcess::OnShowHeaplist()
{
	CString strPid;
	DWORD dwPid;
	// 获取进程ID
	strPid = m_lsProcess.GetItemText(m_CurSel, 1);
	// 字符串转整数
	dwPid = _wtoi(strPid);

	m_pDlgHeapList = new CDlgHeapList;
	m_pDlgHeapList->SetInfo(&m_SnapShot, dwPid);
	m_pDlgHeapList->DoModal();
	if (m_pDlgHeapList)
	{
		delete m_pDlgHeapList;
		m_pDlgHeapList = 0;
	}
}




void CDlgProcess::OnProtectprocess()
{
	// 获取当前选中行
	DWORD dwCurSel = m_lsProcess.GetSelectionMark();
	CString strPid = m_lsProcess.GetItemText(dwCurSel, 1);

	CStringA strProtectID = CStringA(strPid);
	CStringA strDllName;// = "C:\\Users\\nSky\\source\\repos\\PCMgr\\Protect.dll";
	strDllName = CStringA(m_strDllName);
	DWORD dwInjectPid = GetInjectPid("peTest.exe");
	CStringA strCmd;
	strCmd.Format("./RemoteThreadInject.exe %d %s %s",
		dwInjectPid, strProtectID.GetBuffer(), strDllName.GetBuffer());
	STARTUPINFOA start{sizeof(start)};
	PROCESS_INFORMATION pinfo{0};
	CreateProcessA(0, strCmd.GetBuffer(), 0, 0, 0, 0, 0, 0, &start, &pinfo);
	CloseHandle(pinfo.hProcess);
	CloseHandle(pinfo.hThread);
}


DWORD CDlgProcess::GetInjectPid(CStringA strName)
{
	DWORD dwPid = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return 0;
	PROCESSENTRY32W pew = { sizeof(pew) };

	if (FALSE == Process32FirstW(hSnapshot, &pew))
		return 0;  // 没有找到进程


	do
	{
		if (strName == CStringA(pew.szExeFile))
		{
			dwPid = pew.th32ProcessID;
			break;
		}
	} while (Process32NextW(hSnapshot, &pew));
	CloseHandle(hSnapshot);
	return dwPid;
}


void CDlgProcess::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	DragQueryFile(hDropInfo, 0, m_strDllName.GetBuffer(MAX_PATH), MAX_PATH);
	CDialogEx::OnDropFiles(hDropInfo);
}
