// CDlgRevokeMsg.cpp: 实现文件
//

#include "pch.h"
#include "WeChatPlugin.h"
#include "CDlgRevokeMsg.h"
#include "afxdialogex.h"
#include "CDlgRevokeMsg.h"
	// 要HOOK的消息撤回call地址
DWORD g_dwRevokeAddress;
// 消息撤回call，call的地址
DWORD g_dwRevokeTagAddress;//  = (DWORD)hWeChatWi + 0x54AAB210;
DWORD g_dwRevokeRet;
BYTE g_NewCode[0x5] = { 0xE9,0 };
BYTE g_OldCode[0x5] = { 0 };
CDlgRevokeMsg* g_pthis;
// CDlgRevokeMsg 对话框


IMPLEMENT_DYNAMIC(CDlgRevokeMsg, CDialogEx)

CDlgRevokeMsg::CDlgRevokeMsg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLGREVOKEMSG, pParent)
{

}

CDlgRevokeMsg::~CDlgRevokeMsg()
{
}

void CDlgRevokeMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_REVOKEMSG, m_editRevokeMsg);
}


BEGIN_MESSAGE_MAP(CDlgRevokeMsg, CDialogEx)
END_MESSAGE_MAP()

void _stdcall MyMessage(const wchar_t* who, const wchar_t* wxid, const wchar_t* msg, CDlgRevokeMsg* classthis)
{
	// 编辑框中原来的字符串
	CString src;
	// 微信截获的字符串
	CString str;
	str.Format(_T("%s\r\n微信ID：%s\r\n撤回消息内容：%s\r\n\r\n"), who, wxid, msg);
	classthis->m_editRevokeMsg.GetWindowText(src);
	src += str;
	classthis->m_editRevokeMsg.SetWindowText(src);
}

void __declspec(naked) LogRevokeMsg()
{
	__asm
	{
		push ebp;
		mov ebp, esp;
		pushad;
		push g_pthis;
		push[ebp + 0x7C];
		push[ebp + 0x54];
		push[ebp + 0x8];
		call MyMessage;
		popad;
		mov esp, ebp;
		pop ebp;
	}


	__asm
	{
		call[g_dwRevokeTagAddress];
		jmp[g_dwRevokeRet];
	}
}

// CDlgRevokeMsg 消息处理程序



BOOL CDlgRevokeMsg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	g_pthis = this;
	// TODO:  在此添加额外的初始化
	HMODULE hWeChatWi = GetModuleHandle(_T("WeChatWin.dll"));
	if (NULL == hWeChatWi)
	{
		::MessageBox(0, _T("获取WeChatWi模块失败"), _T("Error"), MB_ICONERROR);
	}
	g_dwRevokeAddress = (DWORD)hWeChatWi + 0x2D98FF;
	// 计算一下跳回的地址
	g_dwRevokeRet = (DWORD)hWeChatWi + 0x2D9904;
	g_dwRevokeTagAddress = (DWORD)hWeChatWi + 0x2DA210;
	// 修改hWeChatWi 模块撤回call的硬编码，让他跳转到我们的函数，
	// 函数不在一个模块，跨模块跳转，一般是5个字节的jmp
	// 0xE9
	// 保存目标原来的信息
	ReadProcessMemory(GetCurrentProcess(), (LPVOID)g_dwRevokeAddress, g_OldCode, 0x5, 0);

	// 向目标地址写入jmp LogRevokeMsg
	*(PDWORD)(g_NewCode + 1) = (DWORD)LogRevokeMsg - g_dwRevokeAddress - 0x5;

	WriteProcessMemory(GetCurrentProcess(), (LPVOID)g_dwRevokeAddress, g_NewCode, 0x5, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}
