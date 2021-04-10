// CDlgDriver.cpp: 实现文件
//

#include "pch.h"
#include "ArkApp.h"
#include "CDlgDriver.h"
#include "afxdialogex.h"
#include "ArkCommon.h"

// CDlgDriver 对话框

IMPLEMENT_DYNAMIC(CDlgDriver, CDialogEx)

CDlgDriver::CDlgDriver(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DRIVER, pParent)
{

}

CDlgDriver::~CDlgDriver()
{
}

void CDlgDriver::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DRIVER, m_lsDriver);
}


BEGIN_MESSAGE_MAP(CDlgDriver, CDialogEx)
	ON_COMMAND(ID_DRIVER_FLUSH, &CDlgDriver::OnDriverFlush)
	ON_COMMAND(ID_DRIVER_HIDE, &CDlgDriver::OnDriverHide)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DRIVER, &CDlgDriver::OnNMRClickListDriver)
END_MESSAGE_MAP()


// CDlgDriver 消息处理程序

// 响应驱动对话框初始化
BOOL CDlgDriver::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsDriver.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CString strColumnName[] = {
	L"驱动名称",
	L"基址",
	L"大小",
	L"驱动对象",
	L"驱动路径",
	L"加载顺序"
	};

	m_lsDriver.GetClientRect(rc);
	DWORD dwWidth = rc.right / _countof(strColumnName) - 2;

	for (size_t i = 0; i < _countof(strColumnName); i++)
	{
		m_lsDriver.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
	}

	ShowDriverInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}

// 显示驱动信息
void CDlgDriver::ShowDriverInfo()
{
	// 清空列表框
	m_lsDriver.DeleteAllItems();


	// 1. 查询驱动个数
	DWORD dwDrvNum = 0;
	DWORD dwRet = 0;
	::DeviceIoControl(theApp.m_hArk, ArkCountDriver, NULL, 0, &dwDrvNum, sizeof(DWORD), &dwRet, NULL);


	CStringW strDrvNum;
	strDrvNum.Format(L"驱动个数：%u\n", dwDrvNum);
	OutputDebugStringW(strDrvNum);


	// 2. 申请内存保存驱动信息
	PDRIVER_INFO pDrvInfo = new DRIVER_INFO[dwDrvNum]{ 0 };
	if (!pDrvInfo)
	{
		return;
	}
	ULONG ulMemSize = sizeof(DRIVER_INFO) * dwDrvNum;
	::DeviceIoControl(theApp.m_hArk, ArkQueryDriver, NULL, 0, pDrvInfo, ulMemSize, &dwRet, NULL);

	// 向列表框中输入信息
	for (size_t i = 0; i < dwDrvNum; i++)
	{
		m_lsDriver.InsertItem(i, L"");
	}


	// 显示驱动信息
	WCHAR szTmp[MAX_PATH] = { 0 };
	for (size_t i = 0; i < dwDrvNum; i++)
	{
		// name
		m_lsDriver.SetItemText(i, 0, pDrvInfo[i].szName);
		// base
		wsprintfW(szTmp, L"0x%08X", pDrvInfo[i].uBase);
		m_lsDriver.SetItemText(i, 1, szTmp);
		// size
		wsprintfW(szTmp, L"0x%08X", pDrvInfo[i].uSize);
		m_lsDriver.SetItemText(i, 2, szTmp);
		// drvobj
		wsprintfW(szTmp, L"0x%08X", pDrvInfo[i].uDrvObj);
		m_lsDriver.SetItemText(i, 3, szTmp);
		// path
		m_lsDriver.SetItemText(i, 4, pDrvInfo[i].szPath);
		// order
		wsprintfW(szTmp, L"%u", pDrvInfo[i].uOrder);
		m_lsDriver.SetItemText(i, 5, szTmp);
	}

	// 释放内存
	if (pDrvInfo)
	{
		delete[] pDrvInfo;
		pDrvInfo = NULL;
	}

}


// 右键点击事件 - 弹出驱动菜单
void CDlgDriver::OnNMRClickListDriver(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	POINT point;
	GetCursorPos(&point);

	// 加载主菜单
	CMenu popMainMenu;
	popMainMenu.LoadMenuW(IDR_MENU1);

	CMenu* pSubMenu = popMainMenu.GetSubMenu(2);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	*pResult = 0;
}

// 响应菜单事件 - 刷新驱动
void CDlgDriver::OnDriverFlush()
{
	ShowDriverInfo();
}

// 响应菜单事件 - 隐藏驱动
void CDlgDriver::OnDriverHide()
{
	
	// 获取当前选择行
	UINT uCurSelected = m_lsDriver.GetSelectionMark();
	// 获取当前选中行
	CString strDrvName = m_lsDriver.GetItemText(uCurSelected, 0);


	// 2 通过IO发送数据给驱动，让驱动隐藏进程
	WCHAR szDriverName[MAX_PATH] = { 0 };
	StrCpyW(szDriverName, strDrvName.GetBuffer());
	DWORD dwRet = 0;
	::DeviceIoControl(theApp.m_hArk, ArkHideDriver, 
		szDriverName, MAX_PATH, NULL, 0, &dwRet, NULL);
	OutputDebugStringW(szDriverName);

	if (0 == GetLastError())
	{
		MessageBoxW(L"目标驱动已经隐藏");
	}
	else
	{
		MessageBoxW(L"目标驱动未隐藏");
	}
	OnDriverFlush();
}
