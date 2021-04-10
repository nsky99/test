// CDlgWND.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgWND.h"
#include "afxdialogex.h"


// CDlgWND 对话框

IMPLEMENT_DYNAMIC(CDlgWND, CDialogEx)

CDlgWND::CDlgWND(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_WND, pParent)
{

}

CDlgWND::~CDlgWND()
{
}

void CDlgWND::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WND, m_lsWnd);
}


BEGIN_MESSAGE_MAP(CDlgWND, CDialogEx)
	ON_WM_SIZE()
	ON_COMMAND(IDCANCEL, &CDlgWND::OnIdcancel)
	ON_COMMAND(IDOK, &CDlgWND::OnIdok)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_WND, &CDlgWND::OnRclickListWnd)
	ON_COMMAND(ID_FLUSHWND, &CDlgWND::OnFlushwnd)
END_MESSAGE_MAP()


// CDlgWND 消息处理程序


void CDlgWND::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_lsWnd.GetSafeHwnd())
	{
		CRect rc;
		GetClientRect(rc);
		m_lsWnd.MoveWindow(rc);
	}
}


BOOL CALLBACK EnumWindowsProc(
	_In_ HWND   hwnd,
	_In_ LPARAM lParam
)
{
	if (0 == IsWindowVisible(hwnd))
		return true;
	CString strWindowClass('\0', MAXBYTE);
	CString strWindowName('\0', MAXBYTE);
	CString strWindowHandle('\0', MAXBYTE);

	CDlgWND* dlg = (CDlgWND*)lParam;
	GetClassName(hwnd, strWindowClass.GetBuffer(), MAXBYTE);
	GetWindowText(hwnd, strWindowName.GetBuffer(), MAXBYTE);
	strWindowHandle.Format(_T("%08X"), (DWORD)hwnd);
	dlg->m_lsWnd.InsertItem(0, strWindowName);
	dlg->m_lsWnd.SetItemText(0, 1, strWindowClass);
	dlg->m_lsWnd.SetItemText(0, 2, strWindowHandle);

	return true;
}

BOOL CDlgWND::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_lsWnd.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CRect rc;
	m_lsWnd.GetClientRect(rc);
	DWORD dwWidth = rc.right / 3;
	m_lsWnd.InsertColumn(0, L"窗口标题", 0, dwWidth);
	m_lsWnd.InsertColumn(1, L"窗口类名", 0, dwWidth);
	m_lsWnd.InsertColumn(2, L"窗口句柄", 0, dwWidth);

	EnumWindows(EnumWindowsProc, (LPARAM)this);
	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}





void CDlgWND::OnIdcancel()
{
	// TODO: 在此添加命令处理程序代码
}


void CDlgWND::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}


void CDlgWND::OnRclickListWnd(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	POINT point;
	GetCursorPos(&point);

	// 加载主菜单
	CMenu popMainMenu;
	popMainMenu.LoadMenuW(IDR_MENU_POP);

	CMenu* pSubMenu = popMainMenu.GetSubMenu(4);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

	*pResult = 0;
}


void CDlgWND::OnFlushwnd()
{
	m_lsWnd.DeleteAllItems();
	EnumWindows(EnumWindowsProc, (LPARAM)this);
}
