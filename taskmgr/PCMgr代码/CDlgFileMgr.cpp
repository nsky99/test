// CDlgFileMgr.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgFileMgr.h"
#include "afxdialogex.h"
#include <md5.h>
#pragma comment(lib, "libcrypto.lib")

// CDlgFileMgr 对话框

IMPLEMENT_DYNAMIC(CDlgFileMgr, CDialogEx)

CDlgFileMgr::CDlgFileMgr(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FILEMGR, pParent)
{

}

CDlgFileMgr::~CDlgFileMgr()
{
}

void CDlgFileMgr::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILEMGR, m_lsFileMgr);
}


BEGIN_MESSAGE_MAP(CDlgFileMgr, CDialogEx)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILEMGR, &CDlgFileMgr::OnRclickListFilemgr)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILEMGR, &CDlgFileMgr::OnDblclkListFilemgr)
END_MESSAGE_MAP()


// CDlgFileMgr 消息处理程序
// 枚举文件的主工作函数
DWORD WINAPI EnumThreadProc(LPVOID lpThreadParameter)
{
	CDlgFileMgr* dlg = (CDlgFileMgr*)lpThreadParameter;
	
	dlg->EnumFile(dlg->m_strCurPath);
	for (size_t i = 0; i < dlg->m_vecFileInfo.size(); i++)
	{
		dlg->m_lsFileMgr.InsertItem(i, dlg->m_vecFileInfo[i].strFileName);
		dlg->m_lsFileMgr.SetItemText(i, 1, dlg->m_vecFileInfo[i].strFileSize);
		dlg->m_lsFileMgr.SetItemText(i, 2, dlg->m_vecFileInfo[i].strCreateTime);
		dlg->m_lsFileMgr.SetItemText(i, 3, dlg->m_vecFileInfo[i].strFileMD5);
		dlg->m_lsFileMgr.SetItemText(i, 4, dlg->m_vecFileInfo[i].strFilePath);
		dlg->m_lsFileMgr.SetItemText(i, 5, dlg->m_vecFileInfo[i].strFileType);

	}
	return 0;

}

BOOL CDlgFileMgr::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsFileMgr.GetClientRect(rc);
	m_lsFileMgr.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 初始化路径
	m_strCurPath = L"C:";

	CString strColumnName[] = {
	L"文件名称",
	L"文件大小",
	L"创建时间",
	L"文件MD5",
	L"文件路径",
	L"类型"
	};

	DWORD dwWidth = rc.right / _countof(strColumnName);
	for (size_t i = 0; i < _countof(strColumnName); i++)
	{
		m_lsFileMgr.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
	}

	// 创建一个线程去枚举文件
	HANDLE hThread = CreateThread(0, 0, EnumThreadProc, this, 0, 0);
	if (hThread == 0)
		return TRUE;
	CloseHandle(hThread);
	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}


void CDlgFileMgr::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_lsFileMgr.GetSafeHwnd())
	{
		CRect rc;
		GetClientRect(rc);
		m_lsFileMgr.MoveWindow(rc);
	}
}


// 枚举文件
void CDlgFileMgr::EnumFile(CString strPath)
{

	// 路径下加上//*   枚举路径下所有
	CString strEnumPath = strPath + _T("\\*");

	WIN32_FIND_DATA wfd{};
	HANDLE hFind = FindFirstFileW(strEnumPath, &wfd);
	MYFILEINFO fileinfo{};
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// 是文件夹
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(wfd.cFileName, _T(".")) == 0)
				{
					continue;
				}
				else
				{
					// CString strFilePath = strPath + _T("\\") + wfd.cFileName;
					// 获取文件的名字
					fileinfo.strFileName = wfd.cFileName;
					// 获取文件的大小
					fileinfo.strFileSize = L"";
					// 获取创建时间
					fileinfo.strCreateTime = GetFileCreateTime(wfd.ftCreationTime);
					// 文件MD5
					fileinfo.strFileMD5 = L"";
					// 获取文件路径
					fileinfo.strFilePath = strPath;
					fileinfo.strFileType = L"文件夹";
					m_vecFileInfo.push_back(fileinfo);
				}

			}
			else //if(wfd.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) // 默认文件
			{
				CString strFilePath = strPath + _T("\\") + wfd.cFileName;
				// 获取文件的名字
				fileinfo.strFileName = wfd.cFileName;
				// 获取文件的大小
				fileinfo.strFileSize = GetFileSize(wfd.nFileSizeHigh, wfd.nFileSizeLow);
				// 获取创建时间
				fileinfo.strCreateTime = GetFileCreateTime(wfd.ftCreationTime);
				// 文件MD5
				fileinfo.strFileMD5 = GetFileMD5(strFilePath);
				// 获取文件路径
				fileinfo.strFilePath = strPath;
				fileinfo.strFileType = L"文件";
				m_vecFileInfo.push_back(fileinfo);
			}
		} while (FindNextFileW(hFind, &wfd));
	}
}


// 计算的文件大小
CString CDlgFileMgr::GetFileSize(DWORD dwHight, DWORD dwLow)
{
	CString strFileSize;
	double dfilesize = ((ULONGLONG(dwHight) << 32) + dwLow) / 1024.0;
	strFileSize.Format(L"%.2lf KB", dfilesize);
	return strFileSize;
}


// 获取创建文件的时间
CString CDlgFileMgr::GetFileCreateTime(FILETIME createfiletime)
{
	CString strCreateTime;
	FILETIME localFileTime = { 0 };
	FileTimeToLocalFileTime(&createfiletime, &localFileTime);
	SYSTEMTIME systemTime = { 0 };
	FileTimeToSystemTime(&localFileTime, &systemTime);

	strCreateTime.Format(L"%04d年/%02d月/%02d日 %02d时:%02d分:%02d秒"
		, systemTime.wYear, systemTime.wMonth, systemTime.wDay,
		systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
	return strCreateTime;
}


// 获取文件的MD5
CString CDlgFileMgr::GetFileMD5(CString strPath)
{
	// 初始化保存 md5 信息的结构体
	MD5_CTX ctx = { 0 };

	MD5_Init(&ctx);

	// 将需要计算的数据传入到对应的结构中
	HANDLE hFile = CreateFileW(strPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == (HANDLE)-1)
	{
		return 0;
	}
	DWORD dwFileSize = ::GetFileSize(hFile, 0);
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


// 右键查看属性
void CDlgFileMgr::OnRclickListFilemgr(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	*pResult = 0;
}

// 双击进入文件夹或者打开文件
void CDlgFileMgr::OnDblclkListFilemgr(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// 获取当前选中
	DWORD dwCurSel = m_lsFileMgr.GetSelectionMark();

	// 获取文件名
	CString strFileName;
	strFileName = m_lsFileMgr.GetItemText(dwCurSel, 0);
	CString strFileType;
	strFileType = m_lsFileMgr.GetItemText(dwCurSel, 5);
	CString strFilePath;
	strFilePath = m_lsFileMgr.GetItemText(dwCurSel, 4);
	if (strFileType == L"文件夹")
	{
		m_vecFileInfo.clear();
		m_lsFileMgr.DeleteAllItems();
		if (strFileName == L"..") // 返回上一层目录
		{
			DWORD dwPos = strFilePath.ReverseFind('\\');
			strFilePath = strFilePath.Left(dwPos);
			m_strCurPath = strFilePath;
		}
		else // 进入目录
		{
			m_strCurPath = strFilePath + L"\\" + strFileName;
		}
		// 创建一个线程去枚举文件
		HANDLE hThread = CreateThread(0, 0, EnumThreadProc, this, 0, 0);
		if (hThread == 0)
			return;
		CloseHandle(hThread);
	}
	else
	{
		STARTUPINFOW si = { sizeof(si) };
		PROCESS_INFORMATION pi = { 0 };
		CString strCMD = strFilePath + L"\\" + strFileName;
		BOOL bRet = CreateProcessW(strCMD.GetBuffer(), 0, 0, 0, 0, 0, 0, 0, &si, &pi);
		if (bRet)
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		else
		{
			CString strsysCMD;
			strsysCMD.Format(L"start %s", strCMD.GetString());
			system(CStringA(strsysCMD));
		}
	}
	*pResult = 0;
}
