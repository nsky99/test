// CDlgUnInstall.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgUnInstall.h"
#include "afxdialogex.h"


// CDlgUnInstall 对话框

IMPLEMENT_DYNAMIC(CDlgUnInstall, CDialogEx)

CDlgUnInstall::CDlgUnInstall(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SOFTUININSTALL, pParent)
{

}

CDlgUnInstall::~CDlgUnInstall()
{
}

void CDlgUnInstall::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_UNINSTALL, m_lsUnInstall);
}


BEGIN_MESSAGE_MAP(CDlgUnInstall, CDialogEx)
	ON_COMMAND(IDOK, &CDlgUnInstall::OnIdok)
	ON_COMMAND(IDCANCEL, &CDlgUnInstall::OnIdcancel)
	ON_WM_SIZE()
	ON_COMMAND(ID_UNINSTALL, &CDlgUnInstall::OnUninstall)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_UNINSTALL, &CDlgUnInstall::OnNMRClickListUninstall)
	ON_COMMAND(ID_FLUSHSOFT, &CDlgUnInstall::OnFlushsoft)
END_MESSAGE_MAP()


// CDlgUnInstall 消息处理程序


void CDlgUnInstall::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}


void CDlgUnInstall::OnIdcancel()
{
	// TODO: 在此添加命令处理程序代码
}


BOOL CDlgUnInstall::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsUnInstall.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CString strColumnName[] = {
	L"软件名称",
	L"版本",
	L"出品公司",
	L"卸载目录",
	};
	m_lsUnInstall.GetClientRect(rc);
	DWORD dwWidth = rc.right / 4;

	for (size_t i = 0; i < _countof(strColumnName); i++)
	{
		m_lsUnInstall.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
	}
	ShowSoftInfo();
	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}


void CDlgUnInstall::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);


	if (m_lsUnInstall.GetSafeHwnd())
	{
		CRect rc;
		GetClientRect(rc);
		m_lsUnInstall.MoveWindow(rc);
	}
}


// 获取已安装软件信息
BOOL CDlgUnInstall::GetSoftwareInfo()
{
	//1.主键位置
	HKEY RootKey = HKEY_LOCAL_MACHINE;
	//接收将要打开的键句柄
	HKEY hkResult = 0;
	//2.默认64位系统子键位置
	LPCTSTR lpSubKey = X64_SUBKEY;
	//3.打开注册表键
	LONG lReturn = RegOpenKeyEx(
		RootKey,
		lpSubKey,
		0,
		KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,
		&hkResult
	);
	//索引下标
	DWORD dwIndex = 0;
	//4.循环遍历Uninstall目录下的子键
	while (TRUE)
	{
		SOFTINFO SoftInfo = {};
		DWORD dwkeyLen = 255;
		//注册表名称
		WCHAR szNewKeyName[MAX_PATH] = {};
		//枚举注册表名称
		LONG lReturn = RegEnumKeyEx(
			hkResult, // 子键句柄
			dwIndex, //子建的索引
			szNewKeyName, //指向保存子建名称
			&dwkeyLen, //子建名称长度
			0, NULL, NULL, NULL);
		if (lReturn != ERROR_SUCCESS)
		{
			break;
		}
		//5.组合子建成完整名称
		WCHAR strMidReg[MAX_PATH] = {};
		swprintf_s(strMidReg, L"%s%s%s", lpSubKey, L"\\", szNewKeyName);
		//6.打开新子键 获得句柄
		HKEY hkValueKey = 0;
		RegOpenKeyEx(RootKey, strMidReg, 0, KEY_QUERY_VALUE, &hkValueKey);
		//7.获取键值
		DWORD dwNameLen = 255;
		DWORD dwType = 0;
		//8.获取软件名字

		RegQueryValueEx(hkValueKey, L"DisplayName", 0, &dwType, (LPBYTE)SoftInfo.szSoftName,
			&dwNameLen);
		if (wcscmp(SoftInfo.szSoftName, L"") == 0)
		{
			dwIndex++;
			continue;
		}
		if (m_vecSoftInfo.size() > 1)
		{
			if (wcscmp(SoftInfo.szSoftName, m_vecSoftInfo[m_vecSoftInfo.size() - 1].szSoftName) ==
				0)
			{
				dwIndex++;
				continue;
			}
		}
		dwNameLen = 255;
		dwType = 0;
		RegQueryValueEx(hkValueKey, L"Uninstallstring", 0, &dwType,
			(LPBYTE)SoftInfo.strSoftUniPath, &dwNameLen);//卸载路径
		dwNameLen = 255;
		dwType = 0;
		RegQueryValueEx(hkValueKey, L"DisplayIcon", 0, &dwType, (LPBYTE)SoftInfo.strSoftIco,
			&dwNameLen);//图标
		dwNameLen = 255;
		dwType = 0;
		RegQueryValueEx(hkValueKey, L"DisplayVersion", 0, &dwType, (LPBYTE)SoftInfo.szSoftVer,
			&dwNameLen);//版本号
		dwNameLen = 255;
		dwType = 0;
		RegQueryValueEx(hkValueKey, L"Publisher", 0, &dwType, (LPBYTE)SoftInfo.strSoftvenRel,
			&dwNameLen);//发布商
		dwNameLen = 255;
		dwType = 0;
		RegQueryValueEx(hkValueKey, L"InstallDate", 0, &dwType, (LPBYTE)SoftInfo.szSoftDate,
			&dwNameLen);//安装日期
		dwNameLen = 255;
		dwType = 0;
		RegQueryValueEx(hkValueKey, L"URLInfoAbout", 0, &dwType, (LPBYTE)SoftInfo.strSoftURL,
			&dwNameLen);//网址
		dwNameLen = 255;
		dwType = 0;
		RegQueryValueEx(hkValueKey, L"InstallLocation", 0, &dwType,
			(LPBYTE)SoftInfo.strSoftInsPath, &dwNameLen);//安装路径
		dwNameLen = 255;
		dwType = 0;
		//保存信息
		m_vecSoftInfo.push_back(SoftInfo);
		dwIndex++;
	}
	return TRUE;

}

void CDlgUnInstall::ShowSoftInfo()
{
	m_lsUnInstall.DeleteAllItems();
	m_vecSoftInfo.clear();
	GetSoftwareInfo(); 
	DWORD dwnumber = m_vecSoftInfo.size();

	for (size_t i = 0; i < dwnumber; i++)
	{
		m_lsUnInstall.InsertItem(i, m_vecSoftInfo[i].szSoftName);
		m_lsUnInstall.SetItemText(i, 1, m_vecSoftInfo[i].szSoftVer);
		m_lsUnInstall.SetItemText(i, 2, m_vecSoftInfo[i].strSoftvenRel);
		m_lsUnInstall.SetItemText(i, 3, m_vecSoftInfo[i].strSoftUniPath);
	}
}


// 卸载软件
void CDlgUnInstall::OnUninstall()
{
	// 获取当前选中行
	DWORD dwCurSel = m_lsUnInstall.GetSelectionMark();

	CString strUninstallPath = m_lsUnInstall.GetItemText(dwCurSel, 3);
	if (DeletSoftware(strUninstallPath.GetBuffer()))
	{
		m_lsUnInstall.DeleteItem(dwCurSel);
		m_vecSoftInfo.erase(m_vecSoftInfo.cbegin() + dwCurSel);
	}
	else
	{
		MessageBox(L"卸载失败", L"Error", MB_ICONERROR);
	}
}


// 卸载软件
BOOL CDlgUnInstall::DeletSoftware(LPTSTR szPath)
{
	//启动软件的卸载路径
	HINSTANCE ret = ShellExecute(NULL, L"open", szPath, NULL, NULL, SW_SHOW);
	if (int(ret) < 32)
	{
		return FALSE;

	}
	return TRUE;
}


void CDlgUnInstall::OnNMRClickListUninstall(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	POINT point;
	GetCursorPos(&point);

	// 加载主菜单
	CMenu popMainMenu;
	popMainMenu.LoadMenuW(IDR_MENU_POP);

	CMenu* pSubMenu = popMainMenu.GetSubMenu(7);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	*pResult = 0;
}


void CDlgUnInstall::OnFlushsoft()
{
	ShowSoftInfo();
}
