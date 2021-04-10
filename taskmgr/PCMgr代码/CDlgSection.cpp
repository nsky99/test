// CDlgSection.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgSection.h"
#include "afxdialogex.h"


// CDlgSection 对话框

IMPLEMENT_DYNAMIC(CDlgSection, CDialogEx)

CDlgSection::CDlgSection(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SECTION, pParent)
{

}

CDlgSection::~CDlgSection()
{
}

void CDlgSection::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SECTION, m_lsSection);
}


BEGIN_MESSAGE_MAP(CDlgSection, CDialogEx)
END_MESSAGE_MAP()


// CDlgSection 消息处理程序


BOOL CDlgSection::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsSection.GetClientRect(rc);
	DWORD dwWidth = rc.right / 6;
	m_lsSection.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_lsSection.InsertColumn(0, L"区段名", 0, dwWidth);
	m_lsSection.InsertColumn(1, L"占内存实际大小(未对齐)", 0, dwWidth);
	m_lsSection.InsertColumn(2, L"RVA", 0, dwWidth);
	m_lsSection.InsertColumn(3, L"占磁盘大小(文件对齐)", 0, dwWidth);
	m_lsSection.InsertColumn(4, L"FOA", 0, dwWidth);
	m_lsSection.InsertColumn(5, L"区段属性", 0, dwWidth);


	ShowSectionInfo();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


// 设置信息
void CDlgSection::SetValue(CPE* pPe)
{
	m_pe = pPe;
}


// 显示区段头信息
void CDlgSection::ShowSectionInfo()
{
	m_lsSection.DeleteAllItems();
	DWORD dwLine = 0;
	CString strTmp;
	char buf[12] = { 0 };

	PIMAGE_SECTION_HEADER pSection = m_pe->m_pSectionHeader;
	DWORD dwSectionNumber = m_pe->m_pFileHeader->NumberOfSections;
	for (size_t i = 0; i < dwSectionNumber; i++)
	{
		memcpy(buf, pSection->Name, 8);
		m_lsSection.InsertItem(i, CString(buf));
		strTmp.Format(L"%08X", pSection->Misc.VirtualSize);
		m_lsSection.SetItemText(i, 1, strTmp);
		strTmp.Format(L"%08X", pSection->VirtualAddress);
		m_lsSection.SetItemText(i, 2, strTmp);
		strTmp.Format(L"%08X", pSection->SizeOfRawData);
		m_lsSection.SetItemText(i, 3, strTmp);
		strTmp.Format(L"%08X", pSection->PointerToRawData);
		m_lsSection.SetItemText(i, 4, strTmp);
		strTmp.Format(L"%08X", pSection->Characteristics);
		m_lsSection.SetItemText(i, 5, strTmp);
		pSection++;
	}

}
