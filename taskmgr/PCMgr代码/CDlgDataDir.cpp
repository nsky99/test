// CDlgDataDir.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgDataDir.h"
#include "afxdialogex.h"

// CDlgDataDir 对话框

IMPLEMENT_DYNAMIC(CDlgDataDir, CDialogEx)

CDlgDataDir::CDlgDataDir(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DATADIR, pParent)
{

}

CDlgDataDir::~CDlgDataDir()
{
}

void CDlgDataDir::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATADIR, m_lsDataDir);
}


BEGIN_MESSAGE_MAP(CDlgDataDir, CDialogEx)
END_MESSAGE_MAP()


// CDlgDataDir 消息处理程序


BOOL CDlgDataDir::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rc;
	m_lsDataDir.GetClientRect(rc);
	DWORD dwWidth = rc.right / 3;

	m_lsDataDir.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsDataDir.InsertColumn(0, L"目录类型", 0, dwWidth);
	m_lsDataDir.InsertColumn(1, L"大小", 0, dwWidth);
	m_lsDataDir.InsertColumn(2, L"偏移", 0, dwWidth);
	ShowDataDirInfo();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


// 接收信息
void CDlgDataDir::SetValue(CPE* pPe)
{
	m_pe = pPe;
}


// 显示数据目录信息
void CDlgDataDir::ShowDataDirInfo()
{
	m_lsDataDir.DeleteAllItems();
	DWORD dwLine = 0;
	CString strTmp;

	CString strDataDir[] =
	{
		L"导入表",
		L"导出表",
		L"资源表",
		L"异常",
		L"安全证书",
		L"重定位表",
		L"调试信息",
		L"版权信息",
		L"全局指针",
		L"TLS表",
		L"加载配置",
		L"绑定导入",
		L"导入地址表",
		L"延迟加载表",
		L"COM",
		L"保留",
	};

	for (size_t i = 0; i < 16; i++)
	{
		m_lsDataDir.InsertItem(i, strDataDir[i]);
		strTmp.Format(L"%08X", m_pe->m_pDataDir[i].Size);
		m_lsDataDir.SetItemText(i, 1, strTmp);
		strTmp.Format(L"%08X", m_pe->m_pDataDir[i].VirtualAddress);
		m_lsDataDir.SetItemText(i, 2, strTmp);
	}
}
