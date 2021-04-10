// CDlgVirus.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgVirus.h"
#include "afxdialogex.h"
#include "ConnectVirusLib.h"
#include "CSnapShot.h"
// CDlgVirus 对话框


IMPLEMENT_DYNAMIC(CDlgVirus, CDialogEx)

CDlgVirus::CDlgVirus(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_VIRUS, pParent)
{

}

CDlgVirus::~CDlgVirus()
{
}

void CDlgVirus::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_OVERALL, m_radioOverAll);
	DDX_Control(pDX, IDC_RADIO_SPECIFYPATH, m_radioSpecifyPath);
	DDX_Control(pDX, IDC_CHECK_INTERNATE, m_checkInternate);
	DDX_Control(pDX, IDC_LIST_SCANRESULT, m_lsScanResult);
	DDX_Control(pDX, IDC_LIST_VIRUSLIB, m_lsVirusLib);
	DDX_Control(pDX, IDC_BUTTON_SCAN, m_buttonScan);
	DDX_Control(pDX, IDC_STATIC_PATHINFO, m_staticPath);
	DDX_Control(pDX, IDC_STATIC_SCANINFO, m_groupScanInfo);
	DDX_Control(pDX, IDC_LIST_BLACK, m_lsBlackList);
	DDX_Control(pDX, IDC_LIST_BLACKSELECTINFO, m_lsBlackSelectInfo);
}


BEGIN_MESSAGE_MAP(CDlgVirus, CDialogEx)
	ON_COMMAND(IDOK, &CDlgVirus::OnIdok)
	ON_COMMAND(IDCANCEL, &CDlgVirus::OnIdcancel)
	ON_COMMAND(ID_DELETEVIRUS, &CDlgVirus::OnDeletevirus)
	ON_COMMAND(ID_INSERTVIRUS, &CDlgVirus::OnInsertvirus)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_VIRUSLIB, &CDlgVirus::OnNMRClickListViruslib)
	ON_BN_CLICKED(IDC_RADIO_OVERALL, &CDlgVirus::OnBnClickedRadioOverall)
	ON_BN_CLICKED(IDC_RADIO_SPECIFYPATH, &CDlgVirus::OnBnClickedRadioSpecifypath)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, &CDlgVirus::OnBnClickedButtonScan)
	ON_BN_CLICKED(IDC_BUTTON_DELETEVIRUS, &CDlgVirus::OnBnClickedButtonDeletevirus)
	ON_COMMAND(ID_UPDATASERVER, &CDlgVirus::OnUpdataserver)
	ON_BN_CLICKED(IDC_BUTTON_BLACKSEARCH, &CDlgVirus::OnBnClickedButtonBlacksearch)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_BLACK, &CDlgVirus::OnNMRClickListBlack)
	ON_COMMAND(ID_ADDBLACKLIST, &CDlgVirus::OnAddblacklist)
	ON_COMMAND(ID_DELETEBLACKLIST, &CDlgVirus::OnDeleteblacklist)
	ON_BN_CLICKED(IDC_BUTTON_DELETEBLACKLISTFILE, &CDlgVirus::OnBnClickedButtonDeleteblacklistfile)
END_MESSAGE_MAP()


// CDlgVirus 消息处理程序

void CDlgVirus::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}


void CDlgVirus::OnIdcancel()
{
	// TODO: 在此添加命令处理程序代码
}


BOOL CDlgVirus::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rc;
	DWORD dwWidth;

	// 默认没有在扫描
	m_bIsScaning = 0;
	dwScanedNumber = 0;
	// 初始化操作 - 初始化扫描结果存放的列表框
	m_lsScanResult.GetClientRect(rc);
	dwWidth = rc.right / 2;
	m_lsScanResult.InsertColumn(0, L"病毒路径", 0, dwWidth);
	m_lsScanResult.InsertColumn(1, L"MD5", 0, dwWidth);
	m_lsScanResult.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);


	// 初始化操作 - 初始化病毒库列表框
	m_lsVirusLib.GetClientRect(rc);
	dwWidth = rc.right / 2;
	m_lsVirusLib.InsertColumn(0, L"病毒名称", 0, dwWidth);
	m_lsVirusLib.InsertColumn(1, L"MD5", 0, dwWidth);
	m_lsVirusLib.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 加载默认病毒库
	LoadLocalVirusLib();

	// 默认选中全盘扫描
	m_radioOverAll.SetCheck(1);


	m_lsBlackList.GetClientRect(rc);
	dwWidth = rc.right / 2;
	m_lsBlackList.InsertColumn(0, L"名称", 0, dwWidth);
	m_lsBlackList.InsertColumn(1, L"MD5", 0, dwWidth);
	m_lsBlackList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);


	m_lsBlackSelectInfo.GetClientRect(rc);
	dwWidth = rc.right / 3;
	m_lsBlackSelectInfo.InsertColumn(0, L"路径", 0, dwWidth);
	m_lsBlackSelectInfo.InsertColumn(1, L"MD5", 0, dwWidth);
	m_lsBlackSelectInfo.InsertColumn(2, L"进程ID", 0, dwWidth);
	m_lsBlackSelectInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}


// 加载默认本地病毒库
void CDlgVirus::LoadLocalVirusLib()
{
	// 1 打开文件
	HANDLE hFile = CreateFileW(L".\\LoaclVirusLib.dat", GENERIC_READ | GENERIC_WRITE, 0,
		0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxW(L"读取本地病毒库失败\r\n本地病毒库LoaclVirusLib.dat不存在", L"Error", MB_ICONERROR);
		return;
	}
	// 2 获取文件大小和数量
	DWORD dwFileSize = GetFileSize(hFile, 0);
	DWORD dwVirusLibCount = dwFileSize / (sizeof(VirusLibInfo) / 2);

	// 3 读文件保存到病毒信息的vector
  // 4 输出到病毒库列表框中
	VirusLibInfo tmp;
	for (size_t i = 0; i < dwVirusLibCount; i++)
	{
		ReadFile(hFile, &tmp.strVirusName, MAX_PATH, 0, 0);
		ReadFile(hFile, &tmp.strMD5, MAX_PATH, 0, 0);
		m_vecVirusLib.push_back(tmp);

		m_lsVirusLib.InsertItem(i, tmp.strVirusName);
		m_lsVirusLib.SetItemText(i, 1, tmp.strMD5);
	}

	CloseHandle(hFile);
	// 5 关闭文件句柄
}


// 从病毒库中删除病毒标识
void CDlgVirus::OnDeletevirus()
{
	// 获取当前选中的病毒信息
	// 获取当前选中行病毒
	DWORD dwCurSel = m_lsVirusLib.GetSelectionMark();

	// 从列表框和病毒结构体中删除
	m_lsVirusLib.DeleteItem(dwCurSel);
	m_vecVirusLib.erase(m_vecVirusLib.cbegin() + dwCurSel);

	// 从文件中删除
		// 存入本地病毒库 - 如果病毒库存在就打开如果不存在就创建一个新的病毒库
	HANDLE hFile = CreateFileW(L".\\LoaclVirusLib.dat", GENERIC_READ | GENERIC_WRITE, 0,
		0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxW(L"存入本地病毒库失败", L"Error", MB_ICONERROR);
		return;
	}
	for (size_t i = 0; i < m_vecVirusLib.size(); i++)
	{
		WriteFile(hFile, &m_vecVirusLib[i].strVirusName, MAX_PATH, 0, 0);
		WriteFile(hFile, &m_vecVirusLib[i].strMD5, MAX_PATH, 0, 0);
	}

	CloseHandle(hFile);

}

// 把文件添加到病毒库
void CDlgVirus::OnInsertvirus()
{
	// 用于存放临时病毒信息
	VirusLibInfo VirusInfotmp = { 0 };
	// 获取要添加病毒的路径
	CString strVirusPath = GetFilePath();

	CString strTmp;

	strTmp = GetFileName(strVirusPath);
	// 提取病毒文件名称
	memcpy(VirusInfotmp.strVirusName, strTmp.GetString(), strTmp.GetLength() * 2 + 2);


	// 获取MD5
	strTmp = GetFileMD5(strVirusPath);
	memcpy(VirusInfotmp.strMD5, strTmp.GetString(), strTmp.GetLength() * 2 + 2);

	// 存入病毒列表中 - 在vector尾部
	m_vecVirusLib.push_back(VirusInfotmp);

	// 列表框数据也存入尾部
	DWORD dwInsertPos = m_lsVirusLib.GetItemCount();
	m_lsVirusLib.InsertItem(dwInsertPos, VirusInfotmp.strVirusName);
	m_lsVirusLib.SetItemText(dwInsertPos, 1, VirusInfotmp.strMD5);

	// 存入本地病毒库 - 如果病毒库存在就打开如果不存在就创建一个新的病毒库
	HANDLE hFile = CreateFileW(L".\\LoaclVirusLib.dat", GENERIC_READ | GENERIC_WRITE, 0,
		0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxW(L"存入本地病毒库失败", L"Error", MB_ICONERROR);
		return;
	}
	for (size_t i = 0; i < m_vecVirusLib.size(); i++)
	{
		WriteFile(hFile, &m_vecVirusLib[i].strVirusName, MAX_PATH , 0, 0);
		WriteFile(hFile, &m_vecVirusLib[i].strMD5, MAX_PATH, 0, 0);
	}
	
	CloseHandle(hFile);
	//// 在文件尾部写入新的病毒
	//FILE* fp = 0;
	//fopen_s(&fp, ".\\LoaclVirusLib.dat", "rb+");
	//if (fp == 0)
	//{
 //   MessageBoxW(L"存入本地病毒库失败", L"Error", MB_ICONERROR);
 //   return;
	//}
	//fseek(fp, 0, SEEK_END);
	//fwrite(&VirusInfotmp, sizeof(VirusInfotmp), 1, fp);
	//fclose(fp);
	


}


void CDlgVirus::OnNMRClickListViruslib(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// 获取当前点击的位置
	POINT point;
	GetCursorPos(&point);

	// 加载主菜单
	CMenu popMainMenu;
	popMainMenu.LoadMenuW(IDR_MENU_POP);

	// 弹出子菜单
	CMenu* pSubMenu = popMainMenu.GetSubMenu(6);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);


	*pResult = 0;
}


// 获取文件路径（对话框获取）
CString CDlgVirus::GetFilePath()
{
	TCHAR szBuffer[MAX_PATH] = { 0 };
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = NULL;
	bi.pszDisplayName = szBuffer;
	bi.lpszTitle = _T("选择病毒");
	bi.ulFlags = BIF_BROWSEINCLUDEFILES;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (NULL == idl)
	{
		return CString("");
	}
	SHGetPathFromIDList(idl, szBuffer);
	return szBuffer;
}


// 获取文件的MD5
CString CDlgVirus::GetFileMD5(CString strFilePath)
{
	// 初始化保存 md5 信息的结构体
	MD5_CTX ctx = { 0 };

	MD5_Init(&ctx);

	// 将需要计算的数据传入到对应的结构中
	HANDLE hFile = CreateFileW(strFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == (HANDLE)-1)
	{
		return 0;
	}
	DWORD dwFileSize = GetFileSize(hFile, 0);
	BYTE* strBuf = new BYTE[dwFileSize];
	ReadFile(hFile, strBuf, dwFileSize, 0, 0);
	CloseHandle(hFile);

	MD5_Update(&ctx, strBuf, dwFileSize);

	// 从结构中获取计算后的结果
	unsigned char ustrMD5[MAX_PATH] = { 0 };
	MD5_Final(ustrMD5, &ctx);
	CString strMD5;
	CString strTmp;
	for (size_t i = 0; i < 16; i++)
	{
		strTmp.Format(L"%02X", ustrMD5[i]);
		strMD5 += strTmp;
	}
	delete[] strBuf;
	return strMD5;
}


// 根据路径获取文件名称
CString CDlgVirus::GetFileName(CString strFilePath)
{
	int nPos = strFilePath.ReverseFind('\\');

	strFilePath = strFilePath.Mid(nPos + 1);
	return strFilePath;
}

// 全盘扫描显示
void CDlgVirus::OnBnClickedRadioOverall()
{
	m_staticPath.SetWindowTextW(L"全盘扫描");
	m_strChoosePath = L"";
}

// 指定路径扫描显示
void CDlgVirus::OnBnClickedRadioSpecifypath()
{
	m_strChoosePath = GetFilePath();
	if (m_strChoosePath == TEXT(""))
	{
		m_radioOverAll.SetCheck(TRUE);
		m_radioSpecifyPath.SetCheck(FALSE);
		return;
	}
	m_staticPath.SetWindowTextW(m_strChoosePath.GetString());
}

// 扫描病毒的工作线程
DWORD WINAPI ScanFileProc(LPVOID lpThreadParameter)
{
	CDlgVirus* dlg = (CDlgVirus*)lpThreadParameter;
	// 1. 判断是否是联网杀毒
	if (dlg->m_checkInternate.GetCheck() == 0)
	{
		// 全盘扫描
		if (dlg->m_radioOverAll.GetCheck())
		{
			vector<CString> vecDiverList;
			vecDiverList = dlg->GetDriverList();
			for (auto& val : vecDiverList) dlg->ScanFile(val);
		}
		else
		{
			//dlg->ScanFileProcPro(dlg->m_strChoosePath);
			dlg->ScanFile(dlg->m_strChoosePath);
		}
	}
	else // 联网查杀
	{
		if (!InitStartUp())
		{
			MessageBox(0, L"初始化套接字模块失败", L"Error", MB_ICONERROR);
			return 0;
		}

		SOCKET client;
		if (!CreateSocket(&client))
		{
			MessageBox(0, L"创建套接字失败", L"Error", MB_ICONERROR);
			return 0;
		}

		SOCKADDR_IN client_addr = { 0 };
		if (!ConnectServer(&client, &client_addr))
		{
			MessageBox(0, L"连接服务器失败", L"Error", MB_ICONERROR);
			return 0;
		}


		// 全盘扫描
		if (dlg->m_strChoosePath.GetLength() == 0)
		{
			vector<CString> vecDiverList;
			vecDiverList = dlg->GetDriverList();
			for (auto& val : vecDiverList) dlg->ScanFileForServer(val, &client);
		}
		else
		{
			dlg->ScanFileForServer(dlg->m_strChoosePath, &client);
		}

		ClearUp(&client);
	}
	return 0;
}

// 判断工作线程是否结束
DWORD WINAPI IsScanEndProc(LPVOID lpThreadParameter)
{
	CDlgVirus* dlg = (CDlgVirus*)lpThreadParameter;
	while (dlg->m_bIsScaning)
	{
		WaitForSingleObject(dlg->m_hScanf, -1);
		CloseHandle(dlg->m_hScanf);
		CString strTmp;
		strTmp.Format(L"扫描完成 - 一共扫描了%d个文件", dlg->dwScanedNumber);
		dlg->m_groupScanInfo.SetWindowTextW(strTmp);
		dlg->m_buttonScan.SetWindowTextW(L"开始扫描");
		dlg->m_bIsScaning = 0;
		dlg->dwScanedNumber = 0;
	}
	return 0;
}

// 开始扫描
void CDlgVirus::OnBnClickedButtonScan()
{

	if (m_bIsScaning == 1)
	{
		m_bIsScaning = 0;
		TerminateThread(m_hScanf, 0);
		m_buttonScan.SetWindowTextW(L"开始扫描");
	}
	else
	{
		dwScanedNumber = 0;
		m_bIsScaning = 1;
		m_buttonScan.SetWindowTextW(L"停止扫描");
		m_hScanf = CreateThread(0, 0, ScanFileProc, this, 0, 0);
		CreateThread(0, 0, IsScanEndProc, this, 0, 0);
	}
}

// 获取全部磁盘
vector<CString> CDlgVirus::GetDriverList()
{
	vector<CString> vecDiverList;
	TCHAR nDrive[MAX_PATH];
	GetLogicalDriveStrings(100, (LPWSTR)nDrive);
	TCHAR* pName = nDrive;
	int i = 0;
	while (*pName != 0) 
	{

		vecDiverList.push_back(pName);
		int pos = vecDiverList[i].ReverseFind('\\');
		vecDiverList[i] = vecDiverList[i].Left(pos);
		pName += _tcslen(pName) + 1;
		i++;
	}
	return vecDiverList;
}

// 扫描病毒
void CDlgVirus::ScanFile(CString strPath)
{
	
	CString strTmp;
	WIN32_FIND_DATA wfd = { 0 };
	HANDLE hFind = FindFirstFileW(strPath + L"\\*", &wfd);
	if (hFind == INVALID_HANDLE_VALUE || m_bIsScaning == 0)// 停止继续扫描
	{
		return;
	}

	do
	{
		// 停止扫描
		if (m_bIsScaning == 0)
		{
			return;
		}

		// 当前文件和上一层文件
		if (CString(wfd.cFileName) == L"." || CString(wfd.cFileName) == L"..")continue;

		// 扫描文件夹
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{

			ScanFile(strPath + L"\\" + wfd.cFileName);
		}
		// 扫描到的是文件就对比md5
		else 
		{
			InterlockedIncrement(&dwScanedNumber);
			strTmp.Format(L"已经扫描%d - 正在扫描: %s", dwScanedNumber, wfd.cFileName);
			m_groupScanInfo.SetWindowTextW(strTmp);
			dwScanedNumber++;
			for (size_t i = 0; i < m_vecVirusLib.size(); i++)
			{
				CString strScanedFilePath = strPath + L"\\" + wfd.cFileName;
				CString strScanedFileMD5 = GetFileMD5(strScanedFilePath);
				// 对比MD5
				if (strScanedFileMD5 == CString(m_vecVirusLib[i].strMD5))
				{
					DWORD dwIndex = m_lsScanResult.GetItemCount();
					m_lsScanResult.InsertItem(dwIndex, strPath + L"\\" + wfd.cFileName);
					m_lsScanResult.SetItemText(dwIndex, 1, CString(strScanedFileMD5));
					break;
				}
			}
		}

	} while (FindNextFileW(hFind, &wfd));


}

// 删除全部病毒
void CDlgVirus::OnBnClickedButtonDeletevirus()
{
	// 病毒数量
	DWORD dwCount = m_lsScanResult.GetItemCount();
	CString strVirusFilePath;
	for (size_t i = 0; i < dwCount; i++)
	{
		strVirusFilePath = m_lsScanResult.GetItemText(0, 0);
		DeleteFileW(strVirusFilePath);
		m_lsScanResult.DeleteItem(0);
	}
}

// 链接服务器进行扫描杀毒
void CDlgVirus::ScanFileForServer(CString strPath, SOCKET* client)
{
	CString strTmp;
	WIN32_FIND_DATA wfd = { 0 };
	HANDLE hFind = FindFirstFileW(strPath + L"\\*", &wfd);
	if (hFind == INVALID_HANDLE_VALUE || m_bIsScaning == 0)// 停止继续扫描
	{
		return;
	}

	do
	{
		// 停止扫描
		if (m_bIsScaning == 0)
		{
			return;
		}

		// 当前文件和上一层文件
		if (CString(wfd.cFileName) == L"." || CString(wfd.cFileName) == L"..")continue;

		// 扫描文件夹
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			ScanFileForServer(strPath + L"\\" + wfd.cFileName, client);
		}
		// 扫描到的是文件就对比md5
		else
		{
			InterlockedIncrement(&dwScanedNumber);
			strTmp.Format(L"已经扫描%d - 正在扫描: %s", dwScanedNumber, wfd.cFileName);
			m_groupScanInfo.SetWindowTextW(strTmp);
			dwScanedNumber++;
			for (size_t i = 0; i < m_vecVirusLib.size(); i++)
			{
				CString strScanedFilePath = strPath + L"\\" + wfd.cFileName;
				CString strScanedFileMD5 = GetFileMD5(strScanedFilePath);
				CStringA strMD5 = CStringA(strScanedFileMD5);
				char buf[0x100] = { 0 };
				sprintf_s(buf, 0x100, "select virus_md5 from virus_table where virus_md5 = '%s';",
					strMD5.GetString());
				EnCode(buf);
				if (SOCKET_ERROR == send(*client, buf, 0x100, 0))
				{
					MessageBox(L"向服务器发送MD5失败", L"Error", MB_ICONERROR);
					exit(0);
				}
				if (SOCKET_ERROR == recv(*client, buf, 0x100, 0))
				{
					MessageBox(L"接收服务器消息失败", L"Error", MB_ICONERROR);
					exit(0);
				}
				// 对比MD5
				if (buf[0] == 0) // 返回0 代表在病毒库中找到了
				{
					DWORD dwIndex = m_lsScanResult.GetItemCount();
					m_lsScanResult.InsertItem(dwIndex, strPath + L"\\" + wfd.cFileName);
					m_lsScanResult.SetItemText(dwIndex, 1, CString(strScanedFileMD5));
					break;
				}
			}
		}
	} while (FindNextFileW(hFind, &wfd));
}

// 更新数据到mysql病毒库
void CDlgVirus::OnUpdataserver()
{
	if (!InitStartUp())
	{
		::MessageBox(0, L"初始化套接字模块失败", L"Error", MB_ICONERROR);
		return ;
	}

	SOCKET client;
	if (!CreateSocket(&client))
	{
		::MessageBox(0, L"创建套接字失败", L"Error", MB_ICONERROR);
		return ;
	}

	SOCKADDR_IN client_addr = { 0 };
	if (!ConnectServer(&client, &client_addr))
	{
		::MessageBox(0, L"连接服务器失败", L"Error", MB_ICONERROR);
		return ;
	}

	// 获取当前选中
	DWORD dwCurSel = m_lsVirusLib.GetSelectionMark();

	VirusLibInfo tmp;
	tmp = m_vecVirusLib[dwCurSel];
	char buf[0x100];
	memset(buf, 0, 0x100);
	CStringA strName;
	strName = CStringA(tmp.strVirusName);
	CStringA strMd5;
	strMd5 = CStringA(tmp.strMD5);

	sprintf_s(buf, 0x100, "insert into virus_table value('%s','%s');",
		strName.GetString(), strMd5.GetString());

	//memcpy(buf,Base64Encode(buf, strlen(buf) + 1, 0),0x100);
	EnCode(buf);
	send(client, buf, 0x100, 0);
	MessageBox(L"上传成功", L"Success", MB_OK);


	ClearUp(&client);
}

// 加密数据
void CDlgVirus::EnCode(char* buf)
{
	for (size_t i = 0; i < strlen(buf); i++)
	{
		buf[i] ^= 0x99;
	}
}

// 开始查杀进程
void CDlgVirus::OnBnClickedButtonBlacksearch()
{
	CSnapShot snapshot;
	snapshot.QueryAllProcess();
	
	for (size_t i = 0; i < snapshot.m_vecPInfo.size(); i++)
	{
		CString strProcessMD5 = GetFileMD5(snapshot.m_vecPInfo[i].strProcessPath);
		for (size_t j = 0; j < m_vecBlackList.size(); j++)
		{
			if (strProcessMD5 == m_vecBlackList[j].strMD5)
			{
				DWORD dwIndex = m_lsScanResult.GetItemCount();
				m_lsBlackSelectInfo.InsertItem(dwIndex, snapshot.m_vecPInfo[i].strProcessPath);
				m_lsBlackSelectInfo.SetItemText(dwIndex,1, strProcessMD5);
				m_lsBlackSelectInfo.SetItemText(dwIndex, 2, snapshot.m_vecPInfo[i].strPid);
			}
		}
	}

}

void CDlgVirus::OnNMRClickListBlack(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// 获取当前点击的位置
	POINT point;
	GetCursorPos(&point);

	// 加载主菜单
	CMenu popMainMenu;
	popMainMenu.LoadMenuW(IDR_MENU_POP);

	// 弹出子菜单
	CMenu* pSubMenu = popMainMenu.GetSubMenu(8);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	*pResult = 0;
}

// 添加黑名单
void CDlgVirus::OnAddblacklist()
{
	// 用于存放临时病毒信息
	VirusLibInfo VirusInfotmp = { 0 };
	// 获取要添加病毒的路径
	CString strVirusPath = GetFilePath();

	CString strTmp;

	strTmp = GetFileName(strVirusPath);
	// 提取病毒文件名称
	memcpy(VirusInfotmp.strVirusName, strTmp.GetString(), strTmp.GetLength() * 2 + 2);


	// 获取MD5
	strTmp = GetFileMD5(strVirusPath);
	memcpy(VirusInfotmp.strMD5, strTmp.GetString(), strTmp.GetLength() * 2 + 2);

	// 存入病毒列表中 - 在vector尾部
	m_vecBlackList.push_back(VirusInfotmp);

	// 列表框数据也存入尾部
	m_lsBlackList.InsertItem(m_vecBlackList.size() - 1, VirusInfotmp.strVirusName);
	m_lsBlackList.SetItemText(m_vecBlackList.size() - 1, 1, VirusInfotmp.strMD5);
}

// 删除黑名单
void CDlgVirus::OnDeleteblacklist()
{
	DWORD dwCurSel = m_lsBlackList.GetSelectionMark();
	//if (dwCurSel + 1 > m_vecBlackList.size())
	//{
	//	MessageBox(L"删除内容不在指定范围内", L"Error", MB_ICONERROR);
	//	return;
	//}
	m_lsBlackList.DeleteItem(dwCurSel);
	m_vecBlackList.erase(m_vecBlackList.begin() + dwCurSel);
}

// 删除黑名单文件
void CDlgVirus::OnBnClickedButtonDeleteblacklistfile()
{
	for (int i = 0; i < m_lsBlackSelectInfo.GetItemCount(); i++)
	{
		CString strPath = m_lsBlackSelectInfo.GetItemText(i, 0);
		CString strPid = m_lsBlackSelectInfo.GetItemText(i, 2);
		DWORD dwPid = _wtoi(strPid);
		HANDLE hprocess = OpenProcess(PROCESS_TERMINATE, 0, dwPid);
		if (hprocess == 0)
		{
			CString strTip;
			strTip.Format(L"黑名单程序无法删除:pid %d", dwPid);
			MessageBox(strTip, L"Error", MB_ICONERROR);
			return;
		}
		m_lsBlackSelectInfo.DeleteItem(i);
		if (TerminateProcess(hprocess, 0))
		{
			if (!DeleteFileW(strPath))
			{
				CString strTip;
				strTip.Format(L"黑名单程序文件:%s未能成功删除\r\n可能程序正在被占用", strPath.GetString());
				MessageBox(strTip,L"Error",MB_ICONERROR);
			}
		}
	}
}
