// CDlgStartItem.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgStartItem.h"
#include "afxdialogex.h"


// CDlgStartItem 对话框

IMPLEMENT_DYNAMIC(CDlgStartItem, CDialogEx)

CDlgStartItem::CDlgStartItem(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_START, pParent)
{

}

CDlgStartItem::~CDlgStartItem()
{
}

void CDlgStartItem::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_START, m_lsStartItem);
}


BEGIN_MESSAGE_MAP(CDlgStartItem, CDialogEx)
	ON_COMMAND(IDOK, &CDlgStartItem::OnIdok)
	ON_COMMAND(IDCANCEL, &CDlgStartItem::OnIdcancel)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgStartItem 消息处理程序


void CDlgStartItem::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}


void CDlgStartItem::OnIdcancel()
{
	// TODO: 在此添加命令处理程序代码
}


BOOL CDlgStartItem::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsStartItem.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CString strColumnName[] = {
	L"启动项名",
	L"启动项序号",
	L"删除时的路径",
	L"主键信息",
	L"启动位置"
	};
	m_lsStartItem.GetClientRect(rc);
	DWORD dwWidth = rc.right / 5;

	for (size_t i = 0; i < _countof(strColumnName); i++)
	{
		m_lsStartItem.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
	}

	ShowStartUpInfo();
	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}


void CDlgStartItem::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (m_lsStartItem.GetSafeHwnd())
	{
		CRect rc;
		GetClientRect(rc);
		m_lsStartItem.MoveWindow(rc);
	}
}


// 显示启动信息
void CDlgStartItem::ShowStartUpInfo()
{
	m_lsStartItem.DeleteAllItems();
	m_vecStartUp.clear();
	GetStartup();
	DWORD dwCount = m_vecStartUp.size();
	for (size_t i = 0; i < dwCount; i++)
	{
		m_lsStartItem.InsertItem(i, m_vecStartUp[i].BootName);
		m_lsStartItem.SetItemText(i, 1, m_vecStartUp[i].BootOrder);
		m_lsStartItem.SetItemText(i, 2, m_vecStartUp[i].BootOffset);
		m_lsStartItem.SetItemText(i, 3, m_vecStartUp[i].BootHKEY);
		m_lsStartItem.SetItemText(i, 4, m_vecStartUp[i].BootAllOffset);
	}
}


// 获取启动信息
void CDlgStartItem::GetStartup()
{
	MYSTARTUPINFO StartUpInfo = {};
	HKEY RootKey = NULL; //主键
	HKEY hkResult = NULL; //打开的键句柄
	LONG lReturn = 0; //记录打开的句柄
	DWORD Index = 0; //索引
	DWORD dwKeyLen = 255; //键的长度
	DWORD dwNameLen = 255; //名字长度
	DWORD dwType = 0;
	CString buff;
	LPCTSTR lpSubKey[2]; //子键名称
	lpSubKey[0] = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	lpSubKey[1] = _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run");
	for (DWORD i = 0; i < 2; i++)
	{
		if (i == 0)
			RootKey = HKEY_CURRENT_USER;
		if (i == 1)
			RootKey = HKEY_LOCAL_MACHINE;
		//打开子键
		lReturn = RegOpenKeyEx(RootKey, lpSubKey[i], 0, KEY_ALL_ACCESS, &hkResult);
		if (lReturn == ERROR_SUCCESS)
		{
			//打开成功
			while (ERROR_NO_MORE_ITEMS != RegEnumValue(
				hkResult, Index, StartUpInfo.BootName,
				&dwKeyLen, 0, NULL, NULL, NULL))
			{
				//获取键值中的名称
				RegQueryValueEx(
					hkResult, StartUpInfo.BootName,
					0, &dwType, (LPBYTE)StartUpInfo.BootOrder, &dwNameLen);
				//保存位置
				if (i == 0)
					_tcscpy_s(StartUpInfo.BootHKEY, _T("HKEY_CURRENT_USER"));
				if (i == 1)
					_tcscpy_s(StartUpInfo.BootHKEY, _T("HKEY_LOCAL_MACHINE"));
				//保存子键路径
				_tcscpy_s(StartUpInfo.BootOffset, lpSubKey[i]);
				//保存启动项的全路径
				buff.Empty();
				buff = buff + StartUpInfo.BootHKEY + _T("\\") + StartUpInfo.BootOffset;
				_tcscpy_s(StartUpInfo.BootAllOffset, buff);
				m_vecStartUp.push_back(StartUpInfo);
				Index++;
				dwNameLen = 255;
				dwKeyLen = 255;
				memset(&StartUpInfo, 0, sizeof(MYSTARTUPINFO));
			}
			Index = 0;
			RegCloseKey(hkResult);
		}
	}

}
