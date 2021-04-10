// CDlgTLS.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgTLS.h"
#include "afxdialogex.h"


// CDlgTLS 对话框

IMPLEMENT_DYNAMIC(CDlgTLS, CDialogEx)

CDlgTLS::CDlgTLS(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TLS, pParent)
{

}

CDlgTLS::~CDlgTLS()
{
}

void CDlgTLS::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TLS, m_lsTLS);
}


BEGIN_MESSAGE_MAP(CDlgTLS, CDialogEx)
END_MESSAGE_MAP()


// CDlgTLS 消息处理程序


void CDlgTLS::SetValue(CPE* pPe)
{
	m_pe = pPe;
}


BOOL CDlgTLS::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsTLS.GetClientRect(rc);
	DWORD dwWidth = rc.right / 2;
	m_lsTLS.InsertColumn(0, L"成员", 0, dwWidth);
	m_lsTLS.InsertColumn(1, L"值", 0, dwWidth);
	m_lsTLS.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);


	PIMAGE_TLS_DIRECTORY32 pTls = m_pe->m_pTLS32;
	DWORD dwLine = 0;
	CString strTmp;
	// DWORD   StartAddressOfRawData;
	m_lsTLS.InsertItem(dwLine, L"StartAddressOfRawData");
	strTmp.Format(L"%08X", pTls->StartAddressOfRawData);
	m_lsTLS.SetItemText(dwLine, 1, strTmp);
	dwLine++;

	// DWORD   EndAddressOfRawData;
	m_lsTLS.InsertItem(dwLine, L"EndAddressOfRawData");
	strTmp.Format(L"%08X", pTls->EndAddressOfRawData);
	m_lsTLS.SetItemText(dwLine, 1, strTmp);
	dwLine++;

	// DWORD   AddressOfIndex;             // PDWORD
	m_lsTLS.InsertItem(dwLine, L"AddressOfIndex");
	strTmp.Format(L"%08X", pTls->AddressOfIndex);
	m_lsTLS.SetItemText(dwLine, 1, strTmp);
	dwLine++;

	// DWORD   AddressOfCallBacks;         // PIMAGE_TLS_CALLBACK *
	m_lsTLS.InsertItem(dwLine, L"AddressOfCallBacks");
	strTmp.Format(L"%08X", pTls->AddressOfCallBacks);
	m_lsTLS.SetItemText(dwLine, 1, strTmp);
	dwLine++;

	// DWORD   SizeOfZeroFill;
	m_lsTLS.InsertItem(dwLine, L"SizeOfZeroFill");
	strTmp.Format(L"%08X", pTls->SizeOfZeroFill);
	m_lsTLS.SetItemText(dwLine, 1, strTmp);
	dwLine++;

	// union {
	// 	DWORD Characteristics;
	// 	struct {
	// 		DWORD Reserved0 : 20;
	// 		DWORD Alignment : 4;
	// 		DWORD Reserved1 : 8;
	// 	} DUMMYSTRUCTNAME;
	// } DUMMYUNIONNAME;
	m_lsTLS.InsertItem(dwLine, L"Characteristics");
	strTmp.Format(L"%08X", pTls->Characteristics);
	m_lsTLS.SetItemText(dwLine, 1, strTmp);
	dwLine++;


	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}
