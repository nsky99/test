// CDlgModule.cpp: 实现文件
//

#include "pch.h"
#include "ArkApp.h"
#include "CDlgModule.h"
#include "afxdialogex.h"
#include "ArkCommon.h"

// CDlgModule 对话框

IMPLEMENT_DYNAMIC(CDlgModule, CDialogEx)

CDlgModule::CDlgModule(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_MODULE, pParent),
	m_ulEProcess(0)
{

}

CDlgModule::CDlgModule(ULONG ulEProcess, CWnd* pParent)
	: CDialogEx(IDD_DLG_MODULE, pParent),
	m_ulEProcess(ulEProcess)
{
}

CDlgModule::~CDlgModule()
{
}

void CDlgModule::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODULE, m_lsModule);
}


BEGIN_MESSAGE_MAP(CDlgModule, CDialogEx)
END_MESSAGE_MAP()


// CDlgModule 消息处理程序


BOOL CDlgModule::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsModule.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CString strColumnName[] = {
	L"模块名称",
	L"基址",
	L"大小",
	L"路径"
	};

	m_lsModule.GetClientRect(rc);
	DWORD dwWidth = rc.right / _countof(strColumnName) - 2;

	for (size_t i = 0; i < _countof(strColumnName); i++)
	{
		m_lsModule.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
	}


	ShowModuleInfo();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


// 显示模块信息到模块list
void CDlgModule::ShowModuleInfo()
{
	m_lsModule.DeleteAllItems();

	// 通过IO发送数据给驱动，让驱动查询指定EPROCESS的模块个数
	DWORD dwRet = 0;
	DWORD dwModuleNum = 0;
	::DeviceIoControl(theApp.m_hArk, ArkCountModule,
		&m_ulEProcess, sizeof(DWORD), &dwModuleNum, sizeof(DWORD), &dwRet, NULL);

	CStringW strModuleNum;
	strModuleNum.Format(L"模块个数：%u\n", dwModuleNum);
	OutputDebugStringW(strModuleNum);
	SetWindowTextW(strModuleNum);


	// 申请内存
	// 内存大小
	ULONG ulMemSize = sizeof(MODULE_INFO) * dwModuleNum;
	PMODULE_INFO pModuleInfo = new MODULE_INFO[dwModuleNum]{ 0 };

	// 发送IO请求 - 查询模块信息
	::DeviceIoControl(theApp.m_hArk, ArkQueryModule,
		&m_ulEProcess, sizeof(DWORD), pModuleInfo, ulMemSize, &dwRet, NULL);

	// 向列表框中输入信息
	for (size_t i = 0; i < dwModuleNum; i++)
	{
		m_lsModule.InsertItem(i, L"");
	}

	WCHAR szTmp[MAX_PATH] = { 0 };
	for (size_t i = 0; i < dwModuleNum; i++)
	{
		// name
		m_lsModule.SetItemText(i, 0, pModuleInfo[i].szName);
		// base
		wsprintfW(szTmp, L"%u", pModuleInfo[i].ulBase);
		m_lsModule.SetItemText(i, 1, szTmp);
		// size
		wsprintfW(szTmp, L"%u", pModuleInfo[i].ulSize);
		m_lsModule.SetItemText(i, 2, szTmp);
		// path
		m_lsModule.SetItemText(i, 3, pModuleInfo[i].szPath);
	}

	// 释放内存
	if (pModuleInfo)
	{
		delete[] pModuleInfo;
		pModuleInfo = NULL;
	}
}