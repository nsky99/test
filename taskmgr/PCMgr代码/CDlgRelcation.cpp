// CDlgRelcation.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgRelcation.h"
#include "afxdialogex.h"


// CDlgRelcation 对话框

IMPLEMENT_DYNAMIC(CDlgRelcation, CDialogEx)

CDlgRelcation::CDlgRelcation(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RELATION, pParent)
{

}

CDlgRelcation::~CDlgRelcation()
{
}

void CDlgRelcation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RELOCATION, m_lsRelcationInfo);
	DDX_Control(pDX, IDC_LIST_RELOCATIONTABLE, m_lsRelcation);
}


BEGIN_MESSAGE_MAP(CDlgRelcation, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RELOCATION, &CDlgRelcation::OnItemchangedListRelocation)
END_MESSAGE_MAP()


// CDlgRelcation 消息处理程序


void CDlgRelcation::SetValue(CPE* pPe)
{
	m_pe = pPe;
}


BOOL CDlgRelcation::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsRelcationInfo.GetClientRect(rc);
	DWORD dwWidth = rc.right / 3;

	m_lsRelcationInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsRelcationInfo.InsertColumn(0, L"RVA", 0, dwWidth);
	m_lsRelcationInfo.InsertColumn(1, L"块", 0, dwWidth);
	m_lsRelcationInfo.InsertColumn(2, L"数量", 0, dwWidth);

	ShowRelcationInfo();

	m_lsRelcation.GetClientRect(rc);
	dwWidth = rc.right / 5;

	m_lsRelcation.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsRelcation.InsertColumn(0, L"重定位项", 0, dwWidth);
	m_lsRelcation.InsertColumn(1, L"重定位Rva", 0, dwWidth);
	m_lsRelcation.InsertColumn(2, L"重定位Type", 0, dwWidth);
	m_lsRelcation.InsertColumn(3, L"重定位文件偏移", 0, dwWidth);
	m_lsRelcation.InsertColumn(4, L"重定位值", 0, dwWidth);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


// 显示重定位表头
void CDlgRelcation::ShowRelcationInfo()
{
	m_lsRelcationInfo.DeleteAllItems();
	CString strTmp;
	m_vecRel.clear();

	// 获取重定位表头信息
	PIMAGE_BASE_RELOCATION pBaseRelocation = m_pe->m_pBaseRelocation;
	/*
			DWORD   VirtualAddress;
			DWORD   SizeOfBlock;
	//  WORD    TypeOffset[1];
	*/
	// 结束标识的最后一个表头信息为空
	DWORD dwLine = 0;
	while (*(unsigned __int64*)pBaseRelocation)
	{
		m_vecRel.push_back(pBaseRelocation);

		strTmp.Format(L"%08X", pBaseRelocation->VirtualAddress);
		m_lsRelcationInfo.InsertItem(dwLine, strTmp);

		strTmp.Format(L"%08X", pBaseRelocation->SizeOfBlock);
		m_lsRelcationInfo.SetItemText(dwLine, 1, strTmp);

		DWORD dwNumber = (pBaseRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2;
		strTmp.Format(L"%08X", dwNumber);
		m_lsRelcationInfo.SetItemText(dwLine, 2, strTmp);
		// 指向下一个表头信息
		pBaseRelocation = 
			(PIMAGE_BASE_RELOCATION)((DWORD)pBaseRelocation + pBaseRelocation->SizeOfBlock);
		dwLine++;
	}

}


void CDlgRelcation::OnItemchangedListRelocation(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_lsRelcation.DeleteAllItems();
	CString strTmp;
	// 当前选中行
	DWORD dwCurSel = pNMLV->iItem;
	PIMAGE_BASE_RELOCATION pRelcation = m_vecRel[dwCurSel];

	DWORD dwNumber = (pRelcation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2;
	PWORD pRelcationItem = PWORD((DWORD)pRelcation + 0x8);
	for (size_t i = 0; i < dwNumber; i++)
	{
		// 高4位是3就输出
		BYTE RelType = (*pRelcationItem >> 12);
		WORD RelOffset = (*pRelcationItem & 0x0FFF);

		if (RelType == 3)
		{
			// ITEM
			strTmp.Format(L"%08X", *pRelcationItem);
			m_lsRelcation.InsertItem(i, strTmp);
			// RVA
			DWORD dwRelRva = pRelcation->VirtualAddress + RelOffset;
			strTmp.Format(L"%08X", dwRelRva);
			m_lsRelcation.SetItemText(i, 1, strTmp);
			// Type
			m_lsRelcation.SetItemText(i, 2, L"HIGHLOW");

			// RelFoa
			DWORD dwRelFoa = m_pe->RvaToFoa(dwRelRva);
			strTmp.Format(L"%08X", dwRelFoa);
			m_lsRelcation.SetItemText(i, 3, strTmp);

			// RelValue
			PBYTE pData = PBYTE(dwRelFoa + m_pe->m_pImage);
			strTmp = L"";
			for (size_t i = 0; i < 4; i++)
			{
				CString strT;
				strT.Format(L"%02X ", *pData);
				
				strTmp += strT;
				pData++;
			}
			m_lsRelcation.SetItemText(i, 4, strTmp);
		}
		else
		{
			// 垃圾数据不输出
		}
		pRelcationItem++;
	}



	*pResult = 0;
}
