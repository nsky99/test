// CDlgPlugin.cpp: 实现文件
//

#include "pch.h"
#include "llkPlugin.h"
#include "CDlgPlugin.h"
#include "afxdialogex.h"


// CDlgPlugin 对话框

IMPLEMENT_DYNAMIC(CDlgPlugin, CDialogEx)

CDlgPlugin::CDlgPlugin(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PLUGIN, pParent)
{

}

CDlgPlugin::~CDlgPlugin()
{
}

void CDlgPlugin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPlugin, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_PLUGIN1, &CDlgPlugin::OnBnClickedButtonPluginBomb)
	ON_BN_CLICKED(IDC_BUTTON_PLUGIN2, &CDlgPlugin::OnBnClickedButtonPluginBombKill)
	ON_BN_CLICKED(IDC_BUTTON_PLUGIN3, &CDlgPlugin::OnBnClickedButtonPluginCompass)
	ON_BN_CLICKED(IDC_BUTTON_PLUGIN4, &CDlgPlugin::OnBnClickedButtonPluginSimulation)
	ON_BN_CLICKED(IDC_BUTTON_PLUGIN5, &CDlgPlugin::OnBnClickedButtonPlugin5)
	ON_BN_CLICKED(IDC_BUTTON_PLUGIN6, &CDlgPlugin::OnBnClickedButtonPlugin6)
	ON_BN_CLICKED(IDC_BUTTON_PLUGIN7, &CDlgPlugin::OnBnClickedButtonPlugin7)
END_MESSAGE_MAP()


// CDlgPlugin 消息处理程序

// 单个炸弹
void CDlgPlugin::OnBnClickedButtonPluginBomb()
{
	CllkPluginApp* ptheApp = (CllkPluginApp*)AfxGetApp();

	HWND hwnd = ptheApp->m_llkhWnd;
	::SendMessage(hwnd, WM_USEPROP, 0xF4, 0);
}

// 炸弹秒杀线程
DWORD WINAPI MiaoSha(
	LPVOID lpThreadParameter
	)
{
	CllkPluginApp* ptheApp = (CllkPluginApp*)AfxGetApp();

	HWND hwnd = ptheApp->m_llkhWnd;
	for (int i = 0; i < 100; i++)
	{
		::SendMessage(hwnd, WM_USEPROP, 0xF4, 0);
	}
	return 0;
}

// 炸弹一键秒杀
void CDlgPlugin::OnBnClickedButtonPluginBombKill()
{
	HANDLE hThread = CreateThread(0, 0, MiaoSha, 0, 0, 0);
	if (hThread == 0)
	{
		MessageBox(_T("炸弹一键秒杀失败"));
		return;
	}
	CloseHandle(hThread);
}

// 指南针
void CDlgPlugin::OnBnClickedButtonPluginCompass()
{
	CllkPluginApp* ptheApp = (CllkPluginApp*)AfxGetApp();

	HWND hwnd = ptheApp->m_llkhWnd;
	::SendMessage(hwnd, WM_USEPROP, 0xF0, 0);
}


void CDlgPlugin::OnBnClickedButtonPluginSimulation()
{
	CllkPluginApp* ptheApp = (CllkPluginApp*)AfxGetApp();

	HWND hwnd = ptheApp->m_llkhWnd;
	::SendMessage(hwnd, WM_PLUGIN2, 0, 0);
}


void CDlgPlugin::OnBnClickedButtonPlugin5()
{
	CllkPluginApp* ptheApp = (CllkPluginApp*)AfxGetApp();

	HWND hwnd = ptheApp->m_llkhWnd;
	::SendMessage(hwnd, WM_USEPROP, 0xF1, 0);
}

// 炸弹秒杀线程
DWORD WINAPI MoNiMiaoSha(
	LPVOID lpThreadParameter
)
{
	CllkPluginApp* ptheApp = (CllkPluginApp*)AfxGetApp();

	HWND hwnd = ptheApp->m_llkhWnd;
	for (int i = 0; i < 100; i++)
	{
		::SendMessage(hwnd, WM_PLUGIN2, 0, 0);
	}
	return 0;
}


// 模拟点击秒杀
void CDlgPlugin::OnBnClickedButtonPlugin6()
{
	HANDLE hThread = CreateThread(0, 0, MoNiMiaoSha, 0, 0, 0);
	if (hThread == 0)
	{
		MessageBox(_T("模拟点击秒杀失败"));
		return;
	}
	CloseHandle(hThread);
}


void CDlgPlugin::OnBnClickedButtonPlugin7()
{
	CllkPluginApp* ptheApp = (CllkPluginApp*)AfxGetApp();
	(WNDPROC)SetWindowLong(ptheApp->m_llkhWnd, GWLP_WNDPROC, (LONG)ptheApp->m_OldProc);

	HMODULE hModule = GetModuleHandle(0);
	FreeLibraryAndExitThread(hModule, 0);
	CloseHandle(hModule);
}
