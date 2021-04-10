// CDlgPE.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgPE.h"
#include "afxdialogex.h"

// CDlgPE 对话框

IMPLEMENT_DYNAMIC(CDlgPE, CDialogEx)

CDlgPE::CDlgPE(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PE, pParent)
{

}

CDlgPE::~CDlgPE()
{
}

void CDlgPE::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DOS, m_lsDOS);
	DDX_Control(pDX, IDC_LIST_FILEHEADER, m_lsFileHeader);
	DDX_Control(pDX, IDC_LIST_DOS_OPHEADER, m_lsOpHeader);
	DDX_Control(pDX, IDC_EDIT_RVA, m_editRVA);
	DDX_Control(pDX, IDC_EDIT_VA, m_editVA);
	DDX_Control(pDX, IDC_EDIT_FOA, m_editFoa);
}


BEGIN_MESSAGE_MAP(CDlgPE, CDialogEx)
	ON_COMMAND(IDOK, &CDlgPE::OnIdok)
	ON_COMMAND(IDCANCEL, &CDlgPE::OnIdcancel)
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_DATADIR, &CDlgPE::OnBnClickedButtonDatadir)
	ON_BN_CLICKED(IDC_BUTTON_SECTION, &CDlgPE::OnBnClickedButtonSection)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CDlgPE::OnBnClickedButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CDlgPE::OnBnClickedButtonImport)
	ON_BN_CLICKED(IDC_BUTTON_RLOCATION, &CDlgPE::OnBnClickedButtonRlocation)
	ON_BN_CLICKED(IDC_BUTTON_RES, &CDlgPE::OnBnClickedButtonRes)
	ON_BN_CLICKED(IDC_BUTTON_TLS, &CDlgPE::OnBnClickedButtonTls)
	ON_BN_CLICKED(IDC_BUTTON_DELAYIMPORT, &CDlgPE::OnBnClickedButtonDelayimport)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &CDlgPE::OnBnClickedButtonChange)
	ON_EN_SETFOCUS(IDC_EDIT_FOA, &CDlgPE::OnSetfocusEditFoa)
	ON_EN_SETFOCUS(IDC_EDIT_RVA, &CDlgPE::OnSetfocusEditRva)
	ON_EN_SETFOCUS(IDC_EDIT_VA, &CDlgPE::OnSetfocusEditVa)
END_MESSAGE_MAP()


BOOL CDlgPE::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CRect rc;
	m_lsDOS.GetClientRect(rc);
	DWORD dwWidth = rc.right / 3;
	m_lsDOS.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsDOS.InsertColumn(0, L"成员", 0, dwWidth);
	m_lsDOS.InsertColumn(1, L"文件偏移", 0, dwWidth);
	m_lsDOS.InsertColumn(2, L"值", 0, dwWidth);

	m_lsFileHeader.GetClientRect(rc);
	dwWidth = rc.right / 3;
	m_lsFileHeader.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsFileHeader.InsertColumn(0, L"成员", 0, dwWidth);
	m_lsFileHeader.InsertColumn(1, L"文件偏移", 0, dwWidth);
	m_lsFileHeader.InsertColumn(2, L"值", 0, dwWidth);

	m_lsOpHeader.GetClientRect(rc);
	dwWidth = rc.right / 3;
	m_lsOpHeader.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsOpHeader.InsertColumn(0, L"成员", 0, dwWidth);
	m_lsOpHeader.InsertColumn(1, L"文件偏移", 0, dwWidth);
	m_lsOpHeader.InsertColumn(2, L"值", 0, dwWidth);
	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}






// CDlgPE 消息处理程序


void CDlgPE::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
}


void CDlgPE::OnIdcancel()
{
	// TODO: 在此添加命令处理程序代码
}


void CDlgPE::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);


}



void CDlgPE::OnDropFiles(HDROP hDropInfo)
{
	DragQueryFile(hDropInfo, 0, m_strFile.GetBuffer(MAX_PATH), MAX_PATH);

	// 1 把数据读到内存
	if (!m_pe.ReadFileToMem(m_strFile))
	{
		MessageBoxW(L"文件打开失败", L"Error", MB_ICONERROR);
		return;
	}

	// 2 检查PE
	if (!m_pe.CheckPE())
	{
		MessageBoxW(L"这可能不是一个PE文件", L"Error", MB_ICONERROR);
		return;
	}

	// 3 检查是64还是32
	if (0 == m_pe.Check64or32())//32
	{
		m_pe.AnalysisPE32();
		m_pe.AnalysisDataDir();
		m_pe.AnalysisSection();
		ShowDosHeaderInfo();
		ShowFileHeaderInfo();
		ShowOpHeaderInfo32();
	}
	else if(1 == m_pe.Check64or32())//64
	{
		m_pe.AnalysisPE64();
		m_pe.AnalysisDataDir();
		m_pe.AnalysisSection();
		ShowDosHeaderInfo();
		ShowFileHeaderInfo();
		ShowOpHeaderInfo64();
	}
	else//ROM
	{





	}





	CDialogEx::OnDropFiles(hDropInfo);
}


// 显示DOS信息
void CDlgPE::ShowDosHeaderInfo()
{
	CString strTmp;
	DWORD dwLine = 0;
	m_lsDOS.DeleteAllItems();
	//  e_magic
	m_lsDOS.InsertItem(dwLine, L"e_magic");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_magic - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_magic);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;


	// e_cblp;
	m_lsDOS.InsertItem(dwLine, L"e_cblp");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_cblp - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_cblp);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_cp;
	m_lsDOS.InsertItem(dwLine, L"e_cp");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_cp - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_cp);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_crlc;
	m_lsDOS.InsertItem(dwLine, L"e_crlc");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_crlc - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_crlc);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_cparhdr;
	m_lsDOS.InsertItem(dwLine, L"e_cparhdr");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_cparhdr - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_cparhdr);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_minalloc;
	m_lsDOS.InsertItem(dwLine, L"e_minalloc");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_minalloc - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_minalloc);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_maxalloc;
	m_lsDOS.InsertItem(dwLine, L"e_maxalloc");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_maxalloc - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_maxalloc);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_ss;
	m_lsDOS.InsertItem(dwLine, L"e_ss");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_ss - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_ss);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_sp;
	m_lsDOS.InsertItem(dwLine, L"e_sp");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_sp - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_sp);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_csum;
	m_lsDOS.InsertItem(dwLine, L"e_csum");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_csum - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_csum);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_ip;
	m_lsDOS.InsertItem(dwLine, L"e_ip");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_ip - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_ip);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_cs;
	m_lsDOS.InsertItem(dwLine, L"e_cs");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_cs - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_cs);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_lfarlc;
	m_lsDOS.InsertItem(dwLine, L"e_lfarlc");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_lfarlc - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_lfarlc);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_ovno;
	m_lsDOS.InsertItem(dwLine, L"e_ovno");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_ovno - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_ovno);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_res[4];

	for (DWORD i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			m_lsDOS.InsertItem(dwLine, L"e_res[4]");
		}
		else
		{
			m_lsDOS.InsertItem(dwLine, L"");
		}
		strTmp.Format(L"%08X", (DWORD)(&m_pe.m_pDOS->e_res + i) - (DWORD)m_pe.m_pImage);
		m_lsDOS.SetItemText(dwLine, 1, strTmp);
		strTmp.Format(L"%04X", m_pe.m_pDOS->e_res[i]);
		m_lsDOS.SetItemText(dwLine, 2, strTmp);
		dwLine++;
	}


	// e_oemid;
	m_lsDOS.InsertItem(dwLine, L"e_oemid");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_oemid - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_oemid);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_oeminfo;
	m_lsDOS.InsertItem(dwLine, L"e_oeminfo");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_oeminfo - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pDOS->e_oeminfo);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// e_res2[10];
	for (DWORD i = 0; i < 10; i++)
	{
		if (i == 0)
		{
			m_lsDOS.InsertItem(dwLine, L"e_res2[10]");
		}
		else
		{
			m_lsDOS.InsertItem(dwLine, L"");
		}
		strTmp.Format(L"%08X", (DWORD)(&m_pe.m_pDOS->e_res2 + i) - (DWORD)m_pe.m_pImage);
		m_lsDOS.SetItemText(dwLine, 1, strTmp);
		strTmp.Format(L"%04X", m_pe.m_pDOS->e_res2[i]);
		m_lsDOS.SetItemText(dwLine, 2, strTmp);
		dwLine++;
	}


	// e_lfanew;
	m_lsDOS.InsertItem(dwLine, L"e_lfanew");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pDOS->e_lfanew - (DWORD)m_pe.m_pImage);
	m_lsDOS.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pDOS->e_lfanew);
	m_lsDOS.SetItemText(dwLine, 2, strTmp);
	dwLine++;

}


// 显示文件头信息
void CDlgPE::ShowFileHeaderInfo()
{
	CString strTmp;
	DWORD dwLine = 0;
	m_lsFileHeader.DeleteAllItems();

	//WORD    Machine;
	m_lsFileHeader.InsertItem(dwLine, L"Machine");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pFileHeader->Machine - (DWORD)m_pe.m_pImage);
	m_lsFileHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pFileHeader->Machine);
	m_lsFileHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	//WORD    NumberOfSections;
	m_lsFileHeader.InsertItem(dwLine, L"NumberOfSections");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pFileHeader->NumberOfSections - (DWORD)m_pe.m_pImage);
	m_lsFileHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pFileHeader->NumberOfSections);
	m_lsFileHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	//DWORD   TimeDateStamp;
	m_lsFileHeader.InsertItem(dwLine, L"TimeDateStamp");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pFileHeader->TimeDateStamp - (DWORD)m_pe.m_pImage);
	m_lsFileHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pFileHeader->TimeDateStamp);
	m_lsFileHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	//DWORD   PointerToSymbolTable;
	m_lsFileHeader.InsertItem(dwLine, L"PointerToSymbolTable");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pFileHeader->PointerToSymbolTable - (DWORD)m_pe.m_pImage);
	m_lsFileHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pFileHeader->PointerToSymbolTable);
	m_lsFileHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	//DWORD   NumberOfSymbols;
	m_lsFileHeader.InsertItem(dwLine, L"NumberOfSymbols");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pFileHeader->NumberOfSymbols - (DWORD)m_pe.m_pImage);
	m_lsFileHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pFileHeader->NumberOfSymbols);
	m_lsFileHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	//WORD    SizeOfOptionalHeader;
	m_lsFileHeader.InsertItem(dwLine, L"SizeOfOptionalHeader");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pFileHeader->SizeOfOptionalHeader - (DWORD)m_pe.m_pImage);
	m_lsFileHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pFileHeader->SizeOfOptionalHeader);
	m_lsFileHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	//WORD    Characteristics;
	m_lsFileHeader.InsertItem(dwLine, L"Characteristics");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pFileHeader->Characteristics - (DWORD)m_pe.m_pImage);
	m_lsFileHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pFileHeader->Characteristics);
	m_lsFileHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

}


// 显示扩展头信息32
void CDlgPE::ShowOpHeaderInfo32()
{
	CString strTmp;
	DWORD dwLine = 0;
	m_lsOpHeader.DeleteAllItems();

	// WORD    Magic;
	m_lsOpHeader.InsertItem(dwLine, L"Magic");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->Magic - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp32->Magic);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// BYTE    MajorLinkerVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MajorLinkerVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->MajorLinkerVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%02X", m_pe.m_pOp32->MajorLinkerVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// BYTE    MinorLinkerVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MinorLinkerVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->MinorLinkerVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%02X", m_pe.m_pOp32->MinorLinkerVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   SizeOfCode;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfCode");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->SizeOfCode - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->SizeOfCode);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   SizeOfInitializedData;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfInitializedData");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->SizeOfInitializedData - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->SizeOfInitializedData);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   SizeOfUninitializedData;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfUninitializedData");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->SizeOfUninitializedData - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->SizeOfUninitializedData);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   AddressOfEntryPoint;
	m_lsOpHeader.InsertItem(dwLine, L"AddressOfEntryPoint");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->AddressOfEntryPoint - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->AddressOfEntryPoint);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   BaseOfCode;
	m_lsOpHeader.InsertItem(dwLine, L"BaseOfCode");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->BaseOfCode - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->BaseOfCode);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   BaseOfData;
	m_lsOpHeader.InsertItem(dwLine, L"BaseOfData");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->BaseOfData - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->BaseOfData);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	//
	// NT additional fields.
	//

	// DWORD   ImageBase;
	m_lsOpHeader.InsertItem(dwLine, L"ImageBase");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->ImageBase - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->ImageBase);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   SectionAlignment;
	m_lsOpHeader.InsertItem(dwLine, L"SectionAlignment");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->SectionAlignment - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->SectionAlignment);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   FileAlignment;
	m_lsOpHeader.InsertItem(dwLine, L"FileAlignment");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->FileAlignment - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->FileAlignment);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD    MajorOperatingSystemVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MajorOperatingSystemVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->MajorOperatingSystemVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp32->MajorOperatingSystemVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD    MinorOperatingSystemVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MinorOperatingSystemVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->MinorOperatingSystemVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp32->MinorOperatingSystemVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD    MajorImageVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MajorImageVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->MajorImageVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp32->MajorImageVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD    MinorImageVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MinorImageVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->MinorImageVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp32->MinorImageVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD    MajorSubsystemVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MajorSubsystemVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->MajorSubsystemVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp32->MajorSubsystemVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD    MinorSubsystemVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MinorSubsystemVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->MinorSubsystemVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp32->MinorSubsystemVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   Win32VersionValue;
	m_lsOpHeader.InsertItem(dwLine, L"Win32VersionValue");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->Win32VersionValue - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->Win32VersionValue);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   SizeOfImage;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfImage");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->SizeOfImage - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->SizeOfImage);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   SizeOfHeaders;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfHeaders");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->SizeOfHeaders - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->SizeOfHeaders);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   CheckSum;
	m_lsOpHeader.InsertItem(dwLine, L"CheckSum");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->CheckSum - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->CheckSum);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD    Subsystem;
	m_lsOpHeader.InsertItem(dwLine, L"Subsystem");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->Subsystem - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp32->Subsystem);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD    DllCharacteristics;
	m_lsOpHeader.InsertItem(dwLine, L"DllCharacteristics");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->DllCharacteristics - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp32->DllCharacteristics);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   SizeOfStackReserve;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfStackReserve");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->SizeOfStackReserve - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->SizeOfStackReserve);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;


	// DWORD   SizeOfStackCommit;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfStackCommit");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->SizeOfStackCommit - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->SizeOfStackCommit);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   SizeOfHeapReserve;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfHeapReserve");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->SizeOfHeapReserve - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->SizeOfHeapReserve);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   SizeOfHeapCommit;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfHeapCommit");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->SizeOfHeapCommit - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->SizeOfHeapCommit);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   LoaderFlags;
	m_lsOpHeader.InsertItem(dwLine, L"LoaderFlags");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->LoaderFlags - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->LoaderFlags);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD   NumberOfRvaAndSizes;
	m_lsOpHeader.InsertItem(dwLine, L"NumberOfRvaAndSizes");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp32->NumberOfRvaAndSizes - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp32->NumberOfRvaAndSizes);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;
}


// 显示扩展头信息64
void CDlgPE::ShowOpHeaderInfo64()
{
	CString strTmp;
	DWORD dwLine = 0;
	m_lsOpHeader.DeleteAllItems();

	// WORD        Magic;
	m_lsOpHeader.InsertItem(dwLine, L"Magic");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->Magic - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp64->Magic);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// BYTE        MajorLinkerVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MajorLinkerVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->MajorLinkerVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%02X", m_pe.m_pOp64->MajorLinkerVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// BYTE        MinorLinkerVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MinorLinkerVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->MinorLinkerVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%02X", m_pe.m_pOp64->MinorLinkerVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       SizeOfCode;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfCode");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->SizeOfCode - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->SizeOfCode);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       SizeOfInitializedData;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfInitializedData");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->SizeOfInitializedData - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->SizeOfInitializedData);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       SizeOfUninitializedData;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfUninitializedData");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->SizeOfUninitializedData - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->SizeOfUninitializedData);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       AddressOfEntryPoint;
	m_lsOpHeader.InsertItem(dwLine, L"AddressOfEntryPoint");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->AddressOfEntryPoint - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->AddressOfEntryPoint);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       BaseOfCode;
	m_lsOpHeader.InsertItem(dwLine, L"BaseOfCode");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->BaseOfCode - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->BaseOfCode);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// ULONGLONG   ImageBase;
	m_lsOpHeader.InsertItem(dwLine, L"ImageBase");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->ImageBase - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%016X", m_pe.m_pOp64->ImageBase);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       SectionAlignment;
	m_lsOpHeader.InsertItem(dwLine, L"SectionAlignment");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->SectionAlignment - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->SectionAlignment);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       FileAlignment;
	m_lsOpHeader.InsertItem(dwLine, L"FileAlignment");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->FileAlignment - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->FileAlignment);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD        MajorOperatingSystemVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MajorOperatingSystemVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->MajorOperatingSystemVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp64->MajorOperatingSystemVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD        MinorOperatingSystemVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MinorOperatingSystemVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->MinorOperatingSystemVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp64->MinorOperatingSystemVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD        MajorImageVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MajorImageVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->MajorImageVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp64->MajorImageVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD        MinorImageVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MinorImageVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->MinorImageVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp64->MinorImageVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD        MajorSubsystemVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MajorSubsystemVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->MajorSubsystemVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp64->MajorSubsystemVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD        MinorSubsystemVersion;
	m_lsOpHeader.InsertItem(dwLine, L"MinorSubsystemVersion");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->MinorSubsystemVersion - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp64->MinorSubsystemVersion);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       Win32VersionValue;
	m_lsOpHeader.InsertItem(dwLine, L"Win32VersionValue");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->Win32VersionValue - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->Win32VersionValue);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       SizeOfImage;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfImage");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->SizeOfImage - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->SizeOfImage);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       SizeOfHeaders;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfHeaders");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->SizeOfHeaders - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->SizeOfHeaders);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       CheckSum;
	m_lsOpHeader.InsertItem(dwLine, L"CheckSum");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->CheckSum - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->CheckSum);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD        Subsystem;
	m_lsOpHeader.InsertItem(dwLine, L"Subsystem");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->Subsystem - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp64->Subsystem);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// WORD        DllCharacteristics;
	m_lsOpHeader.InsertItem(dwLine, L"DllCharacteristics");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->DllCharacteristics - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%04X", m_pe.m_pOp64->DllCharacteristics);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// ULONGLONG   SizeOfStackReserve;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfStackReserve");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->SizeOfStackReserve - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%016X", m_pe.m_pOp64->SizeOfStackReserve);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// ULONGLONG   SizeOfStackCommit;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfStackCommit");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->SizeOfStackCommit - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%016X", m_pe.m_pOp64->SizeOfStackCommit);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// ULONGLONG   SizeOfHeapReserve;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfHeapReserve");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->SizeOfHeapReserve - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%016X", m_pe.m_pOp64->SizeOfHeapReserve);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// ULONGLONG   SizeOfHeapCommit;
	m_lsOpHeader.InsertItem(dwLine, L"SizeOfHeapCommit");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->SizeOfHeapCommit - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%016X", m_pe.m_pOp64->SizeOfHeapCommit);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       LoaderFlags;
	m_lsOpHeader.InsertItem(dwLine, L"LoaderFlags");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->LoaderFlags - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->LoaderFlags);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;

	// DWORD       NumberOfRvaAndSizes;
	m_lsOpHeader.InsertItem(dwLine, L"NumberOfRvaAndSizes");
	strTmp.Format(L"%08X", (DWORD)&m_pe.m_pOp64->NumberOfRvaAndSizes - (DWORD)m_pe.m_pImage);
	m_lsOpHeader.SetItemText(dwLine, 1, strTmp);
	strTmp.Format(L"%08X", m_pe.m_pOp64->NumberOfRvaAndSizes);
	m_lsOpHeader.SetItemText(dwLine, 2, strTmp);
	dwLine++;
}


// 按下按钮解析数据目录
void CDlgPE::OnBnClickedButtonDatadir()
{
	// 定位数据目录在镜像中的地址
	m_pDlgDataDir = new CDlgDataDir;
	m_pDlgDataDir->SetValue(&m_pe);
	m_pDlgDataDir->DoModal();
	delete m_pDlgDataDir;
}

// 按下按钮解析区段
void CDlgPE::OnBnClickedButtonSection()
{
	m_pDlgSection = new CDlgSection;
	m_pDlgSection->SetValue(&m_pe);
	m_pDlgSection->DoModal();
	delete m_pDlgSection;
}

// 按下按钮解析导出表
void CDlgPE::OnBnClickedButtonExport()
{
	if (!m_pe.AnalysisExport())
	{
		MessageBoxW(L"不存在导出表\r\n");
		return;
	}
	m_pDlgExport = new CDlgExport;
	m_pDlgExport->SetValue(&m_pe);
	m_pDlgExport->DoModal();
	delete m_pDlgExport;
}

// 按下按钮解析导入表
void CDlgPE::OnBnClickedButtonImport()
{
	if (!m_pe.AnalysisImport())
	{
		MessageBoxW(L"不存在导入表\r\n");
		return;
	}
	m_pDlgImport = new CDlgImport;
	m_pDlgImport->SetValue(&m_pe);
	m_pDlgImport->DoModal();
	delete m_pDlgImport;
}

// 按下按钮解析重定位表
void CDlgPE::OnBnClickedButtonRlocation()
{
	if (!m_pe.AnalysisRelocation())
	{
		MessageBoxW(L"不存在重定位表\r\n");
		return;
	}
	m_pDlgRelcation = new CDlgRelcation;
	m_pDlgRelcation->SetValue(&m_pe);
	m_pDlgRelcation->DoModal();

	delete m_pDlgRelcation;
}

// 按下按钮解析资源表
void CDlgPE::OnBnClickedButtonRes()
{
	if (!m_pe.AnalysisRes())
	{
		MessageBoxW(L"不存在资源表\r\n");
		return;
	}
	m_pDlgRes = new CDlgRes;
	m_pDlgRes->SetValue(&m_pe);
	m_pDlgRes->DoModal();
	delete m_pDlgRes;
}


// 按下按钮解析TLS表
void CDlgPE::OnBnClickedButtonTls()
{
	if (!m_pe.AnalysisTLS())
	{
		MessageBoxW(L"不存在TLS表\r\n");
		return;
	}
	m_pDlgTLS = new CDlgTLS;
	m_pDlgTLS->SetValue(&m_pe);
	m_pDlgTLS->DoModal();
	delete m_pDlgTLS;
}


// 按下按钮解析延迟加载表
void CDlgPE::OnBnClickedButtonDelayimport()
{
	if (!m_pe.AnalysisDelayImport())
	{
		MessageBoxW(L"不存在延迟加载表\r\n");
		return;
	}
	m_pDlgDelayImport = new CDlgDelayImport;
	m_pDlgDelayImport->SetValue(&m_pe);
	m_pDlgDelayImport->DoModal();
	delete m_pDlgDelayImport;
}


void CDlgPE::OnBnClickedButtonChange()
{
	// foa -> rva/va
	if (m_editFoa.GetWindowTextLengthW())
	{
		CString strFoa;
		m_editFoa.GetWindowTextW(strFoa);
		DWORD dwFoa = 0;
		DWORD dwLen = strFoa.GetLength();
		char* buf = new char[dwLen + 1]{ 0 };
		WideCharToMultiByte(NULL, NULL, strFoa.GetBuffer(), dwLen
			, buf, dwLen, NULL, FALSE);
		dwFoa = strtol(buf, NULL, 16);
		delete[] buf;

		DWORD dwRva = m_pe.FoaToRva(dwFoa);
		CString strRva;
		strRva.Format(L"%08X", dwRva);
		m_editRVA.SetWindowTextW(strRva);

		DWORD dwVa = 0x400000 + dwRva;
		CString strVa;
		strVa.Format(L"%08X", dwVa);
		m_editVA.SetWindowTextW(strVa);
	}
	// rva -> foa/va
	else if (m_editRVA.GetWindowTextLengthW())
	{
		CString strRva;
		m_editRVA.GetWindowTextW(strRva);
		DWORD dwRva = 0;
		DWORD dwLen = strRva.GetLength();
		char* buf = new char[dwLen + 1]{ 0 };
		WideCharToMultiByte(NULL, NULL, strRva.GetBuffer(), dwLen
			, buf, dwLen, NULL, FALSE);
		dwRva = strtol(buf, NULL, 16);
		delete[] buf;
		DWORD dwFoa = m_pe.RvaToFoa(dwRva);
		CString strFoa;
		strFoa.Format(L"%08X", dwFoa);
		m_editFoa.SetWindowTextW(strFoa);

		DWORD dwVa = 0x400000 + dwRva;
		CString strVa;
		strVa.Format(L"%08X", dwVa);
		m_editVA.SetWindowTextW(strVa);

	}
	// va -> foa/rva
	else if (m_editVA.GetWindowTextLengthW())
	{
		CString strVa;
		m_editVA.GetWindowTextW(strVa);
		DWORD dwVa = 0;
		DWORD dwLen = strVa.GetLength();
		char* buf = new char[dwLen + 1]{ 0 };
		WideCharToMultiByte(NULL, NULL, strVa.GetBuffer(), dwLen
			, buf, dwLen, NULL, FALSE);
		dwVa = strtol(buf, NULL, 16);
		delete[] buf;

		DWORD dwRva = dwVa - 0x400000;
		CString strRva;
		strRva.Format(L"%08X", dwRva);
		m_editRVA.SetWindowTextW(strRva);

		DWORD dwFoa = m_pe.RvaToFoa(dwRva);
		CString strFoa;
		strFoa.Format(L"%08X", dwFoa);
		m_editFoa.SetWindowTextW(strFoa);
	}

}


void CDlgPE::OnSetfocusEditFoa()
{
	m_editRVA.SetWindowTextW(L"");
	m_editVA.SetWindowTextW(L"");
	m_editFoa.SetReadOnly(0);
	m_editRVA.SetReadOnly(1);
	m_editVA.SetReadOnly(1);
}


void CDlgPE::OnSetfocusEditRva()
{
	m_editFoa.SetWindowTextW(L"");
	m_editVA.SetWindowTextW(L"");
	m_editRVA.SetReadOnly(0);
	m_editFoa.SetReadOnly(1);
	m_editVA.SetReadOnly(1);
}


void CDlgPE::OnSetfocusEditVa()
{
	m_editFoa.SetWindowTextW(L"");
	m_editRVA.SetWindowTextW(L"");
	m_editVA.SetReadOnly(0);
	m_editFoa.SetReadOnly(1);
	m_editRVA.SetReadOnly(1);
}
