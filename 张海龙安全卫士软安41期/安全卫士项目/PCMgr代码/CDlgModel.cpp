// CDlgModel.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgModel.h"
#include "afxdialogex.h"


// CDlgModel 对话框

IMPLEMENT_DYNAMIC(CDlgModel, CDialogEx)

CDlgModel::CDlgModel(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MODEL, pParent)
{

}

CDlgModel::~CDlgModel()
{
}

void CDlgModel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODEL, m_lsModel);
}


BEGIN_MESSAGE_MAP(CDlgModel, CDialogEx)
	ON_WM_CREATE()
	ON_COMMAND(IDCANCEL, &CDlgModel::OnIdcancel)
END_MESSAGE_MAP()


// CDlgModel 消息处理程序


int CDlgModel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	AnimateWindow(250, AW_ACTIVATE | AW_VER_NEGATIVE);

	return 0;
}


void CDlgModel::OnIdcancel()
{
	AnimateWindow(250, AW_HIDE | AW_VER_POSITIVE);
	CDialogEx::OnCancel();
}


// 设置初始化信息
void CDlgModel::SetInfo(CSnapShot* pSnapShot, DWORD dwPid)
{
	m_pSnapShot = pSnapShot;
	m_Pid = dwPid;
}


BOOL CDlgModel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_lsModel.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CString strColumnName[] = {
		L"模块名",
		L"进程ID",
		L"起始地址",
		L"模块大小",
		L"模块路径",

	};
	CRect rc;
	m_lsModel.GetClientRect(rc);
	DWORD dwWidth = rc.right / _countof(strColumnName);
	for (size_t i = 0; i < _countof(strColumnName); i++)
	{
		m_lsModel.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
	}
	ShowModelInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}


// 显示模块列表信息
void CDlgModel::ShowModelInfo()
{
	m_lsModel.DeleteAllItems();
	m_pSnapShot->QueryProcessModel(m_Pid);
	for (size_t i = 0; i < m_pSnapShot->m_dwModelCount; i++)
	{
		m_lsModel.InsertItem(i, L"");
	}
	for (size_t i = 0; i < m_pSnapShot->m_dwModelCount; i++)
	{
		m_lsModel.SetItemText(i, 0, m_pSnapShot->m_vecMInfo[i].strModelName);
		m_lsModel.SetItemText(i, 1, m_pSnapShot->m_vecMInfo[i].strPid);
		m_lsModel.SetItemText(i, 2, m_pSnapShot->m_vecMInfo[i].strStartAddr);
		m_lsModel.SetItemText(i, 3, m_pSnapShot->m_vecMInfo[i].strModelSize);
		m_lsModel.SetItemText(i, 4, m_pSnapShot->m_vecMInfo[i].strModelPath);
	}
}
