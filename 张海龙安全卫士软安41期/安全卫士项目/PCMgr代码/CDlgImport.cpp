// CDlgImport.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgImport.h"
#include "afxdialogex.h"


// CDlgImport 对话框

IMPLEMENT_DYNAMIC(CDlgImport, CDialogEx)

CDlgImport::CDlgImport(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_IMPORT, pParent)
{

}

CDlgImport::~CDlgImport()
{
}

void CDlgImport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IMPORT, m_lsImport);
	DDX_Control(pDX, IDC_LIST_IMPORTINFO, m_lsImportInfo);
}


BEGIN_MESSAGE_MAP(CDlgImport, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_IMPORTINFO, &CDlgImport::OnItemchangedListImportinfo)
END_MESSAGE_MAP()


// CDlgImport 消息处理程序


BOOL CDlgImport::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsImportInfo.GetClientRect(rc);
	DWORD dwWidth = rc.right / 6;

	m_lsImportInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsImportInfo.InsertColumn(0, L"模块名称", 0, dwWidth);
	/*m_lsImportInfo.InsertColumn(1, L"导入函数个数", 0, dwWidth);*/
	m_lsImportInfo.InsertColumn(1, L"OriginalFirstThunk(RVA)", 0, dwWidth);
	m_lsImportInfo.InsertColumn(2, L"时间戳", 0, dwWidth);
	m_lsImportInfo.InsertColumn(3, L"转发链", 0, dwWidth);
	m_lsImportInfo.InsertColumn(4, L"名称RVA", 0, dwWidth);
	m_lsImportInfo.InsertColumn(5, L"FirstThunk(RVA)", 0, dwWidth);

	m_lsImport.GetClientRect(rc);
	dwWidth = rc.right / 3;
	m_lsImport.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsImport.InsertColumn(0, L"IAT(RVA)", 0, dwWidth);
	m_lsImport.InsertColumn(1, L"Hint", 0, dwWidth);
	m_lsImport.InsertColumn(2, L"函数名称", 0, dwWidth);
	ShowImportInfo();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


// 显示导入表信息
void CDlgImport::ShowImportInfo()
{
	m_lsImportInfo.DeleteAllItems();
	CString strTmp;
	DWORD dwLine = 0;

	// 获取导出表头描述
	PIMAGE_IMPORT_DESCRIPTOR pImportDes = m_pe->m_pImportDes;

	DWORD i = 0;
	while (*(PDWORD)pImportDes)
	{
		// 模块名称
		DWORD dwNameRva = pImportDes->Name;
		DWORD dwNameFoa = m_pe->RvaToFoa(dwNameRva);
		char* pName = dwNameFoa + m_pe->m_pImage;
		strTmp.Format(L"%s", CString(pName).GetBuffer());
		m_lsImportInfo.InsertItem(i, strTmp);

		// OriginalFirstThunk(RVA)
		strTmp.Format(L"%08X", pImportDes->OriginalFirstThunk);
		m_lsImportInfo.SetItemText(i, 1, strTmp);


		// 时间戳
		strTmp.Format(L"%08X", pImportDes->TimeDateStamp);
		m_lsImportInfo.SetItemText(i, 2, strTmp);

		// 转发链
		strTmp.Format(L"%08X", pImportDes->ForwarderChain);
		m_lsImportInfo.SetItemText(i, 3, strTmp);

		// 名称RVA
		strTmp.Format(L"%08X", pImportDes->Name);
		m_lsImportInfo.SetItemText(i, 4, strTmp);

		// FirstThunk(RVA)
		strTmp.Format(L"%08X", pImportDes->FirstThunk);
		m_lsImportInfo.SetItemText(i, 5, strTmp);

		pImportDes++;
		i++;
	}
}


void CDlgImport::SetValue(CPE* pPe)
{
	m_pe = pPe;
}


void CDlgImport::OnItemchangedListImportinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	DWORD dwCurSel = pNMLV->iItem;

	m_lsImport.DeleteAllItems();
	CString strTmp;
	// 选中的导入表成员
	PIMAGE_IMPORT_DESCRIPTOR pImportDes = &m_pe->m_pImportDes[dwCurSel];

	DWORD dwIATRva = pImportDes->OriginalFirstThunk;
	DWORD dwIATFoa = m_pe->RvaToFoa(dwIATRva);

	// 32位pe
	if (m_pe->m_is32)
	{
		PIMAGE_THUNK_DATA32 pImage_ThunkData32 = PIMAGE_THUNK_DATA32(dwIATFoa + m_pe->m_pImage);


		DWORD i = 0;
		// 遍历函数地址表
		while (*(PDWORD)pImage_ThunkData32)
		{

			// IAT(RVA)
			strTmp.Format(L"%08X", pImage_ThunkData32->u1.AddressOfData);
			m_lsImport.InsertItem(i, strTmp);


			// 判断最高位为1  输出序号
			if (pImage_ThunkData32->u1.Function >> 31)
			{
				strTmp.Format(L"%04X", (WORD)pImage_ThunkData32->u1.Ordinal);
				m_lsImport.SetItemText(i, 1, strTmp);
				i++;
				pImage_ThunkData32++;
				continue;
			}

			// 最高位不为1
			DWORD dwImportByNameRva = pImage_ThunkData32->u1.AddressOfData;
			DWORD dwImportByNameFoa = m_pe->RvaToFoa(dwImportByNameRva);
			PIMAGE_IMPORT_BY_NAME pImportByNameImage = 
				PIMAGE_IMPORT_BY_NAME(dwImportByNameFoa + m_pe->m_pImage);

			
			// Hint
			strTmp.Format(L"%04X", pImportByNameImage->Hint);
			m_lsImport.SetItemText(i, 1, strTmp);

			// 函数名称
			strTmp.Format(L"%s", CString(pImportByNameImage->Name).GetBuffer());
			m_lsImport.SetItemText(i, 2, strTmp);

			i++;
			pImage_ThunkData32++;
		}






	}
	// 64位
	else
	{





	}

	*pResult = 0;
}
