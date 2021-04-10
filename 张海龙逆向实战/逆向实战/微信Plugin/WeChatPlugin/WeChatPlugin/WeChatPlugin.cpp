// WeChatPlugin.cpp: 定义 DLL 的初始化例程。
//

#include "pch.h"
#include "framework.h"
#include "WeChatPlugin.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为以下项中的第一个语句:
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CWeChatPluginApp

BEGIN_MESSAGE_MAP(CWeChatPluginApp, CWinApp)
END_MESSAGE_MAP()


// CWeChatPluginApp 构造

CWeChatPluginApp::CWeChatPluginApp()
	:m_RevokeDlg(0)
{
	// TODO:  在此处添加构造代码，
	
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CWeChatPluginApp 对象

CWeChatPluginApp theApp;


// CWeChatPluginApp 初始化

DWORD WINAPI WorkThread(
	LPVOID lpThreadParameter
)
{
	// HOOK地址0x54AAA8FF
	// 获取WeChatWi模块的加载基址
	CDlgRevokeMsg* dlg = (CDlgRevokeMsg*)lpThreadParameter;
	dlg = new CDlgRevokeMsg;
	dlg->DoModal();
	HMODULE hWeChatPlugin = GetModuleHandle(_T("WeChatPlugin.dll"));
	if (0 == hWeChatPlugin)
	{
		return 0;
	}
	FreeLibraryAndExitThread(hWeChatPlugin, 0);
	return 0;
}

BOOL CWeChatPluginApp::InitInstance()
{
	CWinApp::InitInstance();
	 //弹出消息撤回的窗口----用于显示撤回的消息

	HANDLE ht = ::CreateThread(0, 0, WorkThread, m_RevokeDlg, 0, 0);
	
	if (ht == 0)
	{
		::MessageBox(0, _T("创建线程失败"), _T("Error"), MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}


int CWeChatPluginApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类

	return CWinApp::ExitInstance();
}
