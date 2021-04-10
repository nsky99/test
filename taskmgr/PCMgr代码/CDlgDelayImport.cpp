// CDlgDelayImport.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgDelayImport.h"
#include "afxdialogex.h"


// CDlgDelayImport 对话框

IMPLEMENT_DYNAMIC(CDlgDelayImport, CDialogEx)

CDlgDelayImport::CDlgDelayImport(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DELAYIMPORT, pParent)
{

}

CDlgDelayImport::~CDlgDelayImport()
{
}

void CDlgDelayImport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DELAYIMPORT, m_lsDelayImport);
}


BEGIN_MESSAGE_MAP(CDlgDelayImport, CDialogEx)
END_MESSAGE_MAP()


// CDlgDelayImport 消息处理程序


void CDlgDelayImport::SetValue(CPE* pPe)
{
	m_pe = pPe;
}


BOOL CDlgDelayImport::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rc;
	m_lsDelayImport.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsDelayImport.GetClientRect(rc);
	DWORD dwWidth = rc.right / 2;

	m_lsDelayImport.InsertColumn(0, L"成员", 0, dwWidth);
	m_lsDelayImport.InsertColumn(1, L"值", 0, dwWidth);

	// 获取延迟加载表地址
	PIMAGE_DELAYLOAD_DESCRIPTOR pDelayLoad = m_pe->m_pDelayLoad;
	CString strTmp;
	DWORD dwLine = 0;
	//union {
	//	DWORD AllAttributes;
	//	struct {
	//		DWORD RvaBased : 1;             // Delay load version 2
	//		DWORD ReservedAttributes : 31;
	//	} DUMMYSTRUCTNAME;
	//} Attributes;
	m_lsDelayImport.InsertItem(dwLine, L"Attributes");
	strTmp.Format(L"%08X", pDelayLoad->Attributes);
	m_lsDelayImport.SetItemText(dwLine, 1, strTmp);
	dwLine++;

	//DWORD DllNameRVA;                       // RVA to the name of the target library (NULL-terminate ASCII string)
	m_lsDelayImport.InsertItem(dwLine, L"DllNameRVA");
	strTmp.Format(L"%08X", pDelayLoad->DllNameRVA);
	m_lsDelayImport.SetItemText(dwLine, 1, strTmp);
	dwLine++;

	//DWORD ModuleHandleRVA;                  // RVA to the HMODULE caching location (PHMODULE)
	m_lsDelayImport.InsertItem(dwLine, L"ModuleHandleRVA");
	strTmp.Format(L"%08X", pDelayLoad->ModuleHandleRVA);
	m_lsDelayImport.SetItemText(dwLine, 1, strTmp);
	dwLine++;

	//DWORD ImportAddressTableRVA;            // RVA to the start of the IAT (PIMAGE_THUNK_DATA)
	m_lsDelayImport.InsertItem(dwLine, L"ImportAddressTableRVA");
	strTmp.Format(L"%08X", pDelayLoad->ImportAddressTableRVA);
	m_lsDelayImport.SetItemText(dwLine, 1, strTmp);
	dwLine++;


	//DWORD ImportNameTableRVA;               // RVA to the start of the name table (PIMAGE_THUNK_DATA::AddressOfData)
	m_lsDelayImport.InsertItem(dwLine, L"ImportNameTableRVA");
	strTmp.Format(L"%08X", pDelayLoad->ImportNameTableRVA);
	m_lsDelayImport.SetItemText(dwLine, 1, strTmp);
	dwLine++;

	//DWORD BoundImportAddressTableRVA;       // RVA to an optional bound IAT
	m_lsDelayImport.InsertItem(dwLine, L"BoundImportAddressTableRVA");
	strTmp.Format(L"%08X", pDelayLoad->BoundImportAddressTableRVA);
	m_lsDelayImport.SetItemText(dwLine, 1, strTmp);
	dwLine++;


	//DWORD UnloadInformationTableRVA;        // RVA to an optional unload info table
	m_lsDelayImport.InsertItem(dwLine, L"UnloadInformationTableRVA");
	strTmp.Format(L"%08X", pDelayLoad->UnloadInformationTableRVA);
	m_lsDelayImport.SetItemText(dwLine, 1, strTmp);
	dwLine++;


	//DWORD TimeDateStamp;                    // 0 if not bound,
	//																				// Otherwise, date/time of the target DLL
	m_lsDelayImport.InsertItem(dwLine, L"TimeDateStamp");
	strTmp.Format(L"%08X", pDelayLoad->TimeDateStamp);
	m_lsDelayImport.SetItemText(dwLine, 1, strTmp);
	dwLine++;




	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}
