
// PCMgrDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "PCMgr.h"
#include "PCMgrDlg.h"
#include "afxdialogex.h"
#include "CDlgProcess.h"
#include "CDlgComputer.h"
#include "CDlgFileCleaner.h"
#include "CDlgPE.h"
#include "CDlgServer.h"
#include "CDlgVirus.h"
#include "CDlgInject.h"
#include "CDlgUnInstall.h"
#include "CDlgStartItem.h"
#include "CDlgFileMgr.h"
//#include "SkinH.h"
//#pragma comment(lib,"SkinH.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPCMgrDlg 对话框


CPCMgrDlg::CPCMgrDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PCMGR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPCMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_tabMgr);
}

BEGIN_MESSAGE_MAP(CPCMgrDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(IDOK, &CPCMgrDlg::OnIdok)
	ON_COMMAND(IDCANCEL, &CPCMgrDlg::OnIdcancel)
	ON_WM_CREATE()
	ON_WM_HOTKEY()
END_MESSAGE_MAP()


// CPCMgrDlg 消息处理程序

BOOL CPCMgrDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 加载皮肤
	LoadSkin();



	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_tabMgr.InitTab(11,
		L"进程管理", IDD_DIALOG_PROCESS, new CDlgProcess(),
		L"电脑管理", IDD_DIALOG_COMPUTER, new CDlgComputer(),
		L"文件清理", IDD_DIALOG_FILE_CLEANER, new CDlgFileCleaner(),
		L"窗口管理", IDD_DIALOG_WND, new CDlgWND(),
		L"PE查看器", IDD_DIALOG_PE,new CDlgPE(),
		L"服务管理", IDD_DIALOG_SERVER,new CDlgServer(),
		L"病毒查杀", IDD_DIALOG_VIRUS,new CDlgVirus(),
		L"汇编",IDD_DIALOG_INJECT,new CDlgInject(),
		L"卸载软件",IDD_DIALOG_SOFTUININSTALL,new CDlgUnInstall(),
		L"启动项", IDD_DIALOG_START,new CDlgStartItem(),
		L"文件管理",IDD_DIALOG_FILEMGR,new CDlgFileMgr()
);

	// 注册热键
	RegisterHotKey();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPCMgrDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPCMgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPCMgrDlg::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}


void CPCMgrDlg::OnIdcancel()
{
	AnimateWindow(250, AW_CENTER | AW_HIDE);
	HANDLE hProcess = GetCurrentProcess();
	TerminateProcess(hProcess, 0);
}


int CPCMgrDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CenterWindow();
	this->AnimateWindow(250, AW_CENTER);


	return 0;
}


// 注册热键
void CPCMgrDlg::RegisterHotKey()
{
	// TODO: 在此处添加实现代码.
	//注册热键 注意要使用大写字母
		// 注册全局快捷键
	::RegisterHotKey(m_hWnd, 0x1234, MOD_CONTROL, 'Z');
}


// 卸载热键
void CPCMgrDlg::UnRegisterHotKey()
{
	// TODO: 在此处添加实现代码.

}


void CPCMgrDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nHotKeyId)
	{
	case 0x1234:
	{
		if (IsWindowVisible())
		{
			ShowWindow(SW_HIDE);
		}
		else
		{
			ShowWindow(SW_SHOW);
		}
	}
	break;
	default:
		break;
	}
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

// 加载皮肤
void CPCMgrDlg::LoadSkin()
{
	////加载皮肤
	//HMODULE hModule = ::GetModuleHandle(NULL);;
	//if (NULL == hModule) return;

	//HRSRC hRsrc = FindResource(hModule, (LPCWSTR)IDR_SHE2, _T("SHE"));
	//if (NULL == hRsrc) return ;

	//HGLOBAL hGol = LoadResource(hModule, hRsrc);
	//BYTE* lpData = (BYTE*)LockResource(hGol);
	//DWORD dwSize = SizeofResource(hModule, hRsrc);
	//SkinH_AttachRes(lpData, dwSize, NULL, 0, 0, 0);

	HMODULE hModule = LoadLibrary(TEXT("Skin.dll"));
	if (hModule)
	{
		typedef  int  (WINAPI* pMySafeSkin)(void);
		pMySafeSkin MySafeSkin;
		MySafeSkin = (pMySafeSkin)GetProcAddress(hModule, "MySafeSkin");
		MySafeSkin();
	}
}
