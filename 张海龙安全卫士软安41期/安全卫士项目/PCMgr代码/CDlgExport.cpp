// CDlgExport.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgExport.h"
#include "afxdialogex.h"


// CDlgExport 对话框

IMPLEMENT_DYNAMIC(CDlgExport, CDialogEx)

CDlgExport::CDlgExport(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_EXPORT, pParent)
{

}

CDlgExport::~CDlgExport()
{
}

void CDlgExport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EXPORT, m_lsExport);
	DDX_Control(pDX, IDC_LIST_EXPORTINFO, m_lsExportInfo);
}


BEGIN_MESSAGE_MAP(CDlgExport, CDialogEx)
END_MESSAGE_MAP()


// CDlgExport 消息处理程序


BOOL CDlgExport::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsExport.GetClientRect(rc);
	DWORD dwWidth = rc.right / 4;

	m_lsExportInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsExportInfo.InsertColumn(0, L"成员", 0, dwWidth);
	m_lsExportInfo.InsertColumn(1, L"文件偏移", 0, dwWidth);
	m_lsExportInfo.InsertColumn(2, L"值", 0, dwWidth);
	m_lsExportInfo.InsertColumn(3, L"解释", 0, dwWidth);

	ShowExportInfo();

	dwWidth = rc.right / 5;
	m_lsExport.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsExport.InsertColumn(0, L"序号", 0, dwWidth);
	m_lsExport.InsertColumn(1, L"函数RVA", 0, dwWidth);
	m_lsExport.InsertColumn(2, L"名称序号", 0, dwWidth);
	m_lsExport.InsertColumn(3, L"名称RVA", 0, dwWidth);
	m_lsExport.InsertColumn(4, L"名称", 0, dwWidth);
	ShowExport();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


// 显示导出表信息
void CDlgExport::ShowExportInfo()
{
	m_lsExportInfo.DeleteAllItems();
	CString strTmp;
	DWORD dwLine = 0;

	PIMAGE_EXPORT_DIRECTORY pExportDir = m_pe->m_pExportDir;

	// DWORD   Characteristics;
	m_lsExportInfo.InsertItem(dwLine, L"Characteristics");
	strTmp.Format(L"%08X", (DWORD)&pExportDir->Characteristics - (DWORD)m_pe->m_pImage);
	m_lsExportInfo.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", pExportDir->Characteristics);
	m_lsExportInfo.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   TimeDateStamp;
	m_lsExportInfo.InsertItem(dwLine, L"TimeDateStamp");
	strTmp.Format(L"%08X", (DWORD)&pExportDir->TimeDateStamp - (DWORD)m_pe->m_pImage);
	m_lsExportInfo.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", pExportDir->TimeDateStamp);
	m_lsExportInfo.SetItemText(dwLine, 2, strTmp);
	dwLine++;
	// WORD    MajorVersion;
	m_lsExportInfo.InsertItem(dwLine, L"MajorVersion");
	strTmp.Format(L"%08X", (DWORD)&pExportDir->MajorVersion - (DWORD)m_pe->m_pImage);
	m_lsExportInfo.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", pExportDir->MajorVersion);
	m_lsExportInfo.SetItemText(dwLine, 2, strTmp);
	dwLine++;
	// WORD    MinorVersion;
	m_lsExportInfo.InsertItem(dwLine, L"MinorVersion");
	strTmp.Format(L"%08X", (DWORD)&pExportDir->MinorVersion - (DWORD)m_pe->m_pImage);
	m_lsExportInfo.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", pExportDir->MinorVersion);
	m_lsExportInfo.SetItemText(dwLine, 2, strTmp);
	dwLine++;
	// DWORD   Name;
	m_lsExportInfo.InsertItem(dwLine, L"Name");
	strTmp.Format(L"%08X", (DWORD)&pExportDir->Name - (DWORD)m_pe->m_pImage);
	m_lsExportInfo.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", pExportDir->Name);
	m_lsExportInfo.SetItemText(dwLine, 2, strTmp);
	DWORD dwNameRva = pExportDir->Name;
	DWORD dwNameFoa = m_pe->RvaToFoa(dwNameRva);
	char* pNameImage = dwNameFoa + m_pe->m_pImage;
	strTmp.Format(L"%s", CString(pNameImage));
	m_lsExportInfo.SetItemText(dwLine, 3, strTmp);

	dwLine++;
	// DWORD   Base;
	m_lsExportInfo.InsertItem(dwLine, L"Base");
	strTmp.Format(L"%08X", (DWORD)&pExportDir->Base - (DWORD)m_pe->m_pImage);
	m_lsExportInfo.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", pExportDir->Base);
	m_lsExportInfo.SetItemText(dwLine, 2, strTmp);
	dwLine++;
	// DWORD   NumberOfFunctions;
	m_lsExportInfo.InsertItem(dwLine, L"NumberOfFunctions");
	strTmp.Format(L"%08X", (DWORD)&pExportDir->NumberOfFunctions - (DWORD)m_pe->m_pImage);
	m_lsExportInfo.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", pExportDir->NumberOfFunctions);
	m_lsExportInfo.SetItemText(dwLine, 2, strTmp);
	dwLine++;
	// DWORD   NumberOfNames;
	m_lsExportInfo.InsertItem(dwLine, L"NumberOfNames");
	strTmp.Format(L"%08X", (DWORD)&pExportDir->NumberOfNames - (DWORD)m_pe->m_pImage);
	m_lsExportInfo.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", pExportDir->NumberOfNames);
	m_lsExportInfo.SetItemText(dwLine, 2, strTmp);
	dwLine++;
	// DWORD   AddressOfFunctions;     // RVA from base of image
	m_lsExportInfo.InsertItem(dwLine, L"AddressOfFunctions(RVA)");
	strTmp.Format(L"%08X", (DWORD)&pExportDir->AddressOfFunctions - (DWORD)m_pe->m_pImage);
	m_lsExportInfo.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", pExportDir->AddressOfFunctions);
	m_lsExportInfo.SetItemText(dwLine, 2, strTmp);
	dwLine++;
	// DWORD   AddressOfNames;         // RVA from base of image
	m_lsExportInfo.InsertItem(dwLine, L"AddressOfNames(RVA)");
	strTmp.Format(L"%08X", (DWORD)&pExportDir->AddressOfNames - (DWORD)m_pe->m_pImage);
	m_lsExportInfo.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", pExportDir->AddressOfNames);
	m_lsExportInfo.SetItemText(dwLine, 2, strTmp);
	dwLine++;
	// DWORD   AddressOfNameOrdinals;  // RVA from base of image
	m_lsExportInfo.InsertItem(dwLine, L"AddressOfNameOrdinals(RVA)");
	strTmp.Format(L"%08X", (DWORD)&pExportDir->AddressOfNameOrdinals - (DWORD)m_pe->m_pImage);
	m_lsExportInfo.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", pExportDir->AddressOfNameOrdinals);
	m_lsExportInfo.SetItemText(dwLine, 2, strTmp);
	dwLine++;

}

// 显示导出表
void CDlgExport::ShowExport()
{
	m_lsExport.DeleteAllItems();
	CString strTmp;
	DWORD dwLine = 0;
	
	PIMAGE_EXPORT_DIRECTORY pExportDir = m_pe->m_pExportDir;


	// 导出表地址表
	for (size_t i = 0; i < pExportDir->NumberOfFunctions; i++)
	{
		// 在镜像中找到函数地址表
		DWORD dwEATRva = pExportDir->AddressOfFunctions;
		DWORD dwEATFoa = m_pe->RvaToFoa(dwEATRva);
		PDWORD pEAT = (PDWORD)(dwEATFoa + (DWORD)m_pe->m_pImage);


		// 在镜像中找到函数名表
		DWORD dwENTRva = pExportDir->AddressOfNames;
		DWORD dwENTFoa = m_pe->RvaToFoa(dwENTRva);
		PDWORD pENT = (PDWORD)(dwENTFoa + (DWORD)m_pe->m_pImage);

		// 在镜像中找到函数名序号表
		DWORD dwEOTRva = pExportDir->AddressOfNameOrdinals;
		DWORD dwEOTFoa = m_pe->RvaToFoa(dwEOTRva);
		PWORD pEOT = (PWORD)(dwEOTFoa + (DWORD)m_pe->m_pImage);

		// 函数地址表中值为0
		if (pEAT[i] == 0)
		{
			continue;
		}


		// 输出导出序号
		strTmp.Format(L"%04X", i + pExportDir->Base);
		m_lsExport.InsertItem(i, strTmp);
		
		// 输出导出地址RVA
		strTmp.Format(L"%08X", pEAT[i]);
		m_lsExport.SetItemText(i, 1, strTmp);


		// 导出表函数名表
		size_t j = 0;
		for (; j < pExportDir->NumberOfNames; j++)
		{
			// 地址表中函数有名称  函数地址表下标和名称序号表中的值对比
			if (i == pEOT[j])
			{
				// 输出名称序号
				strTmp.Format(L"%04X", pEOT[i]);
				m_lsExport.SetItemText(i, 2, strTmp);

				// 输出名称RVA
				strTmp.Format(L"%08X", pENT[i]);
				m_lsExport.SetItemText(i, 3, strTmp);

				// 输出函数名称
				DWORD dwNameRva = pENT[i];
				DWORD dwNameFoa = m_pe->RvaToFoa(dwNameRva);
				char* pNameImage = dwNameFoa + m_pe->m_pImage;
				strTmp.Format(L"%s", CString(pNameImage));
				m_lsExport.SetItemText(i, 4, strTmp);

				continue;
			}

		}
	}
}


void CDlgExport::SetValue(CPE* pPe)
{
	m_pe = pPe;
}
