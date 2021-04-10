// CDlgFileCleaner.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgFileCleaner.h"
#include "afxdialogex.h"


// CDlgFileCleaner 对话框

IMPLEMENT_DYNAMIC(CDlgFileCleaner, CDialogEx)

CDlgFileCleaner::CDlgFileCleaner(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FILE_CLEANER, pParent)
{

}

CDlgFileCleaner::~CDlgFileCleaner()
{
}

void CDlgFileCleaner::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CLEARHZ, m_editDeleteHZ);
	DDX_Control(pDX, IDC_LIST_CLEARPATH, m_lsPath);
	DDX_Control(pDX, IDC_EDIT_DELETE_INFO, m_editDelInfo);
	DDX_Control(pDX, IDC_LIST_DELED_INFO, m_lsDeledFile);
	DDX_Control(pDX, IDC_LIST_SYS_FILE, m_lsSysFileQuery);
	DDX_Control(pDX, IDC_LIST_SYS_FILE2, m_lsSysFileLog);
}


BEGIN_MESSAGE_MAP(CDlgFileCleaner, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ENUM_DEL_FILE, &CDlgFileCleaner::OnBnClickedButtonEnumDelFile)
	ON_BN_CLICKED(IDC_BUTTON_DEL_FILE, &CDlgFileCleaner::OnBnClickedButtonDelFile)
	ON_BN_CLICKED(IDC_BUTTON_ENUM_SYS_FILE, &CDlgFileCleaner::OnBnClickedButtonEnumSysFile)
	ON_BN_CLICKED(IDC_BUTTON_ENUM_SYSDELFILELOG, &CDlgFileCleaner::OnBnClickedButtonEnumSysdelfilelog)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_RUBBISH, &CDlgFileCleaner::OnBnClickedButtonClearRubbish)
END_MESSAGE_MAP()


// CDlgFileCleaner 消息处理程序


BOOL CDlgFileCleaner::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CString strDel = L".obj\\.tlog\\.lastbuildstate\\.idb\\.pdb\\.pch\\.res\\.ilk\\.exe\\.sdf\\.ipch\\.log\\.db\\";
	m_editDeleteHZ.SetWindowTextW(strDel);

	m_lsDeledFile.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsPath.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);	
	m_lsSysFileQuery.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsSysFileLog.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CRect rc;
	GetClientRect(rc);
	m_lsPath.InsertColumn(0, L"文件路径列表:", 0, rc.right);

	m_lsDeledFile.GetClientRect(rc);
	m_lsDeledFile.InsertColumn(0, L"已删除文件:", 0, rc.right);

	m_lsSysFileQuery.GetClientRect(rc);
	m_lsSysFileQuery.InsertColumn(0, L"要删除的系统垃圾:", 0, rc.right);

	m_lsSysFileLog.GetClientRect(rc);
	m_lsSysFileLog.InsertColumn(0, L"已经删除的系统垃圾:", 0, rc.right);
	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}


void CDlgFileCleaner::OnBnClickedButtonEnumDelFile()
{
	m_enumFileInfo.dwAllFileCount = 0;
	m_enumFileInfo.dwDelFileCount = 0;
	m_enumFileInfo.dwAllFileSize = 0.0;
	m_enumFileInfo.dwDelFileSize = 0.0;
	m_enumFileInfo.vecDelFile.clear();

	// 保存临时路径
	CString strTmpPath;
	// 获取要遍历路径的个数
	DWORD   dwFileCount;
	dwFileCount = m_lsPath.GetItemCount();
	CString strDelType;
	m_editDeleteHZ.GetWindowTextW(strDelType);
	for (size_t i = 0; i < dwFileCount; i++)
	{
		strTmpPath = m_lsPath.GetItemText(i, 0);
		// 枚举这个路径下的文件信息
		EnumFile(strTmpPath, strDelType);
	}

	CString strInfo;
	for (size_t i = 0; i < m_enumFileInfo.dwDelFileCount; i++)
	{
		strInfo.Format(L"%s%s\r\n", strInfo.GetBuffer(), m_enumFileInfo.vecDelFile[i].GetBuffer());
	}

	strInfo.Format(
		L"%s\r\n文件总大小:%.2f MB\r\n文件总个数: %d\r\n要删除文件总大小:%.2f MB\r\n要删除文件总个数: %d\r\n"
		, strInfo.GetBuffer(),
		m_enumFileInfo.dwAllFileSize / 1024.0 / 1024.0, m_enumFileInfo.dwAllFileCount,
		m_enumFileInfo.dwDelFileSize / 1024.0 / 1024.0, m_enumFileInfo.dwDelFileCount);
	m_editDelInfo.SetWindowTextW(strInfo);
}


void CDlgFileCleaner::OnBnClickedButtonDelFile()
{
	for (size_t i = 0; i < m_enumFileInfo.dwDelFileCount; i++)
	{
		m_lsDeledFile.InsertItem(0, m_enumFileInfo.vecDelFile[i]);
		DeleteFileW(m_enumFileInfo.vecDelFile[i]);
	}
	m_editDelInfo.SetWindowTextW(L"");
}

// 枚举文件夹下所有文件
void CDlgFileCleaner::EnumFile(CString strPath,CString strType)
{
	// 获取要删除的文件后缀
	CString strDelType = strType;


	// 路径下加上//*   枚举路径下所有
	CString strEnumPath = strPath + _T("\\*");

	WIN32_FIND_DATA wfd{};
	HANDLE hFind = FindFirstFileW(strEnumPath, &wfd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// 是文件夹
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(wfd.cFileName, _T(".")) == 0 || _tcscmp(wfd.cFileName, _T("..")) == 0)
				{
					continue;
				}
				else
				{
					CString strChildPath = strPath + _T("\\") + wfd.cFileName;
					EnumFile(strChildPath, strDelType);
				}
			}
			else
			{
				// 计算文件大小
				DOUBLE dFileSize = (DOUBLE)((ULONGLONG)wfd.nFileSizeHigh << 32)
					+ wfd.nFileSizeLow;

				// 文件总大小  文件总个数
				m_enumFileInfo.dwAllFileSize += dFileSize;
				m_enumFileInfo.dwAllFileCount++;
				if (strDelType.Find(PathFindExtensionW(wfd.cFileName)) != -1)
				{
					// 拼接要删除的文件信息并保存
					m_enumFileInfo.vecDelFile.push_back(strPath + _T("\\") + wfd.cFileName);
					// 要删除的文件大小 和个数
					m_enumFileInfo.dwDelFileSize += dFileSize;
					m_enumFileInfo.dwDelFileCount++;
				}
			}
		} while (FindNextFileW(hFind, &wfd));
	}
}


DWORD WINAPI QuerySysFile(LPVOID lpThreadParameter)
{
	CDlgFileCleaner* dlg = (CDlgFileCleaner*)lpThreadParameter;
	// 要遍历的系统目录
	CString strPath[] =
	{
		L"C:\\Windows\\Temp",
		L"C:\\Users\\nSky\\AppData\\Local\\Temp",
	};
	wchar_t userName[0x100] = { 0 };
	DWORD dwLen = 0x100;
	GetUserNameW(userName, &dwLen);
	CString strTmpPath;
	strTmpPath.Format(L"C:\\Users\\%s\\AppData\\Local\\Temp", userName);
	strPath[1] = strTmpPath;

	CString strType;
	// 获取要遍历路径的个数
	DWORD   dwFileCount;
	dwFileCount = dlg->m_lsPath.GetItemCount();
	for (size_t i = 0; i < 2; i++)
	{
		dlg->EnumFile(strPath[i], L".tmp\\.log");
		dlg->InsertToSysFileQuery();
	}
	return 0;
}

// 枚举系统文件垃圾
void CDlgFileCleaner::OnBnClickedButtonEnumSysFile()
{
	m_enumFileInfo.dwAllFileCount = 0;
	m_enumFileInfo.dwDelFileCount = 0;
	m_enumFileInfo.dwAllFileSize = 0.0;
	m_enumFileInfo.dwDelFileSize = 0.0;
	m_enumFileInfo.vecDelFile.clear();


	HANDLE hThread = CreateThread(0, 0, QuerySysFile, this, 0, 0);
	if (0 == hThread)
		return;
	CloseHandle(hThread);

}

DWORD WINAPI DelSysFile(LPVOID lpThreadParameter)
{
	CDlgFileCleaner* dlg = (CDlgFileCleaner*)lpThreadParameter;
	DWORD dwCount = dlg->m_lsSysFileQuery.GetItemCount();
	CString strPath;
	for (size_t i = 0; i < dwCount; i++)
	{
		dlg->m_lsSysFileQuery.DeleteItem(0);
		dlg->m_lsSysFileQuery.GetItemText(i, 0, strPath.GetBuffer(MAX_PATH), MAX_PATH);
		dlg->m_lsSysFileLog.InsertItem(0, strPath);
		DeleteFileW(strPath);
	}
	return 0;
}
// 删除的系统垃圾日志
void CDlgFileCleaner::OnBnClickedButtonEnumSysdelfilelog()
{

	HANDLE hThread = CreateThread(0, 0, DelSysFile, this, 0, 0);
	if (0 == hThread)
		return;
	CloseHandle(hThread);

}

void CDlgFileCleaner::InsertToSysFileQuery()
{
	for (DWORD dwi = 0; dwi < m_enumFileInfo.dwDelFileCount; dwi++)
	{
		m_lsSysFileQuery.InsertItem(0, m_enumFileInfo.vecDelFile[dwi]);
	}
}

// 点击清理回收站
void CDlgFileCleaner::OnBnClickedButtonClearRubbish()
{
	ClearRubbish();
}

// 清理回收站
BOOL CDlgFileCleaner::ClearRubbish()
{
	//1.初始化回收站结构
	SHQUERYRBINFO RecyclebinInfo = {}; //回收站结构
	RecyclebinInfo.cbSize = sizeof(SHQUERYRBINFO);
	//2.查询回收站
	SHQueryRecycleBin(NULL, &RecyclebinInfo);
	//3.清空回收站
	HRESULT ret = SHEmptyRecycleBin(NULL, NULL, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI
		| SHERB_NOSOUND);
	if (ret != S_OK)
	{
		return FALSE;
	}
	return TRUE;
}
