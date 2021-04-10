
// ArkAppDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ArkApp.h"
#include "ArkAppDlg.h"
#include "afxdialogex.h"

#include "ArkCommon.h"		// R3 - R0 交互

#include "CDlgProcess.h"
#include "CDlgDriver.h"
#include "CDlgGDT.h"
#include "CDlgIDT.h"
#include "CDlgFile.h"
#include "CDlgReg.h"
#include "CDlgSSDT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CArkAppDlg 对话框



CArkAppDlg::CArkAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ARKAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CArkAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_WIN, m_ctrlTabFrame);
}

BEGIN_MESSAGE_MAP(CArkAppDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CArkAppDlg 消息处理程序

BOOL CArkAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_ctrlTabFrame.InitTab(7,// Tab项目个数 
		L"进程", IDD_DLG_PROCESS, new CDlgProcess(),
		L"驱动", IDD_DLG_DRIVER, new CDlgDriver(),
		L"GDT", IDD_DLG_GDT, new CDlgGDT(),
		L"IDT", IDD_DLG_IDT, new CDlgIDT,
		L"文件", IDD_DLG_FILE, new CDlgFile(),
		L"注册表", IDD_DLG_REG,new CDlgReg(),
		L"SSDT",IDD_DLG_SSDT,new CDlgSSDT()
	);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CArkAppDlg::OnPaint()
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
HCURSOR CArkAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// 初始化主窗口 - 加载驱动
int CArkAppDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	// 启动服务
	if (!theApp.m_ctrlArk.CreateARK())
	{
		::MessageBoxW(0, L"Ark驱动创建失败", L"Error", MB_ICONERROR);
		ExitProcess(-1);
	}
	if (!theApp.m_ctrlArk.StartARK())
	{
		::MessageBoxW(0, L"Ark驱动启动失败", L"Error", MB_ICONERROR);
		ExitProcess(-1);
	}

	// 创建驱动对象
	theApp.m_hArk = ::CreateFileW(
		ARK_SYMLINK_NAME,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
	);
	return 0;
}


// 结束应用程序 - 卸载驱动
void CArkAppDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值



	// 关闭驱动对象
	CloseHandle(theApp.m_hArk);

	// 卸载服务
	theApp.m_ctrlArk.StopARK();
	theApp.m_ctrlArk.DelARK();
	CDialogEx::OnClose();
}