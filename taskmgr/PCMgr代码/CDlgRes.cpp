// CDlgRes.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgRes.h"
#include "afxdialogex.h"


// CDlgRes 对话框

IMPLEMENT_DYNAMIC(CDlgRes, CDialogEx)

CDlgRes::CDlgRes(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_RES, pParent)
{

}

CDlgRes::~CDlgRes()
{
}

void CDlgRes::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESDIRENTRY1, m_lsResDirEntry1);
	DDX_Control(pDX, IDC_LIST_RESDIRENTRY2, m_lsResDirEntry2);
	DDX_Control(pDX, IDC_LIST_RESDIRENTRY3, m_lsResDirEntry3);
	DDX_Control(pDX, IDC_EDIT_RESDATA, m_editResData);
}


BEGIN_MESSAGE_MAP(CDlgRes, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RESDIRENTRY1, &CDlgRes::OnNMClickListResdirentry1)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RESDIRENTRY2, &CDlgRes::OnNMClickListResdirentry2)
END_MESSAGE_MAP()


// CDlgRes 消息处理程序


// 设置值
void CDlgRes::SetValue(CPE* pPe)
{
	m_pe = pPe;
}


BOOL CDlgRes::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	m_lsResDirEntry1.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsResDirEntry1.GetClientRect(rc);
	DWORD dwWidth = 0;
	dwWidth = rc.right;

	m_lsResDirEntry1.InsertColumn(0, L"第一层资源目录", 0, dwWidth);
	ShowResInfo1();

	m_lsResDirEntry2.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsResDirEntry2.GetClientRect(rc);
	dwWidth = rc.right;
	m_lsResDirEntry2.InsertColumn(0, L"第二层资源目录", 0, dwWidth);

	m_lsResDirEntry3.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lsResDirEntry3.GetClientRect(rc);
	dwWidth = rc.right / 2;
	m_lsResDirEntry3.InsertColumn(0, L"成员", 0, dwWidth);
	m_lsResDirEntry3.InsertColumn(1, L"信息", 0, dwWidth);
	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}


// 显示第一层资源信息
int CDlgRes::ShowResInfo1()
{
	CString strTmp;
	DWORD dwLine = 0;
	m_lsResDirEntry1.DeleteAllItems();
	m_editResData.SetWindowTextW(L"");
	m_vecResDirEntry1.clear();
	m_vecResDirEntry2.clear();


	// 资源目录基地址
	m_pResDir1 = m_pe->m_ResDir;

	// 第一层资源表的资源种类 
	DWORD dwCount =
		m_pResDir1->NumberOfIdEntries + m_pResDir1->NumberOfNamedEntries;

	// 第一层资源目录入口
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pResEntry1 = 
		PIMAGE_RESOURCE_DIRECTORY_ENTRY(m_pResDir1 + 1);
	for (size_t i = 0; i < dwCount; i++)
	{
		m_vecResDirEntry1.push_back(pResEntry1);

		// 如果最高位是1，则不是系统资源
		if (pResEntry1->NameIsString)
		{
			PIMAGE_RESOURCE_DIR_STRING_U pResString 
				= PIMAGE_RESOURCE_DIR_STRING_U((DWORD)m_pResDir1 + pResEntry1->NameOffset);
			wchar_t* strbuf = new wchar_t[pResString->Length + 1]{ 0 };
			memcpy(strbuf, pResString->NameString, pResString->Length * 2);
			strTmp.Format(L"%s", strbuf);
		}
		// 系统资源
		else
		{
			switch (pResEntry1->Name)
			{
			case RT_CURSOR:
				strTmp.Format(L"%s", L"光标");
				break;
			case RT_BITMAP:
				strTmp.Format(L"%s", L"位图");
				break;
			case RT_ICON:
				strTmp.Format(L"%s", L"图标");
				break;
			case RT_MENU:
				strTmp.Format(L"%s", L"菜单");
				break;
			case RT_DIALOG:
				strTmp.Format(L"%s", L"对话框");
				break;
			case RT_STRING:
				strTmp.Format(L"%s", L"字符串");
				break;
			case RT_FONTDIR:
				strTmp.Format(L"%s", L"字体目录");
				break;
			case RT_FONT:
				strTmp.Format(L"%s", L"字体");
				break;
			case RT_ACCELERATOR:
				strTmp.Format(L"%s", L"快捷键");
				break;
			case RT_RCDATA:
				strTmp.Format(L"%s", L"非格式化资源");
				break;
			case RT_MESSAGETABLE:
				strTmp.Format(L"%s", L"消息列表");
				break;
			case RT_GROUP_CURSOR:
				strTmp.Format(L"%s", L"光标组");
				break;
			case RT_GROUP_ICON:
				strTmp.Format(L"%s", L"图标组");
				break;
			case RT_VERSION:
				strTmp.Format(L"%s", L"版本信息");
				break;
			default:
				strTmp.Format(L"%08X", pResEntry1->Name);
				break;
			}
		}
		m_lsResDirEntry1.InsertItem(i, strTmp);

		pResEntry1++;
	}
	return 0;
}

// 点击资源表1 -> 显示资源表2
void CDlgRes::OnNMClickListResdirentry1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if ((DWORD)pNMItemActivate->iItem > m_vecResDirEntry1.size())
	{
		return;
	}
	m_editResData.SetWindowTextW(L"");
	m_lsResDirEntry2.DeleteAllItems();
	m_lsResDirEntry3.DeleteAllItems();

	m_vecResDirEntry2.clear();

	CString strTmp;
	// 当前选中
	DWORD dwCurSel = pNMItemActivate->iItem;
	PIMAGE_RESOURCE_DIRECTORY pResDir2 = 0;

	// 找到第二层数据目录头
	if (m_vecResDirEntry1[dwCurSel]->DataIsDirectory)
	{
		pResDir2 = 
			PIMAGE_RESOURCE_DIRECTORY((DWORD)m_pResDir1 + m_vecResDirEntry1[dwCurSel]->OffsetToDirectory);
		// 每种资源个数
		DWORD dwCount = pResDir2->NumberOfIdEntries + pResDir2->NumberOfNamedEntries;

		// 第二层资源目录入口
		PIMAGE_RESOURCE_DIRECTORY_ENTRY pResEntry2 =
			PIMAGE_RESOURCE_DIRECTORY_ENTRY(pResDir2 + 1);

		for (size_t i = 0; i < dwCount; i++)
		{
			m_vecResDirEntry2.push_back(pResEntry2);
			if (pResEntry2->NameIsString)
			{
				PIMAGE_RESOURCE_DIR_STRING_U pResString
					= PIMAGE_RESOURCE_DIR_STRING_U((DWORD)m_pResDir1 + pResEntry2->NameOffset);
				wchar_t* strbuf = new wchar_t[pResString->Length + 1]{ 0 };
				memcpy(strbuf, pResString->NameString, pResString->Length * 2);
				strTmp.Format(L"%s", strbuf);
			}
			else
			{
				strTmp.Format(L"%08X", pResEntry2->Name);
			}
			m_lsResDirEntry2.InsertItem(i, strTmp);
			pResEntry2++;
		}
	
	}
	*pResult = 0;
}

// 点击资源表2 -> 显示资源表3和数据
void CDlgRes::OnNMClickListResdirentry2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	// 当前选中
	DWORD dwCurSel = pNMItemActivate->iItem;
	if (dwCurSel > m_vecResDirEntry2.size())
	{
		return;// 检测越界
	}
	m_lsResDirEntry3.DeleteAllItems();
	m_editResData.SetWindowTextW(L"");
	CString strTmp;
	PIMAGE_RESOURCE_DIRECTORY pResDir3 = 0;

	// 找到第三层数据目录头
	if (m_vecResDirEntry2[dwCurSel]->DataIsDirectory)
	{
		pResDir3 =
			PIMAGE_RESOURCE_DIRECTORY((DWORD)m_pResDir1 + m_vecResDirEntry2[dwCurSel]->OffsetToDirectory);
		// 每种资源个数
		DWORD dwCount = pResDir3->NumberOfIdEntries + pResDir3->NumberOfNamedEntries;

		// 第三层资源目录入口
		PIMAGE_RESOURCE_DIRECTORY_ENTRY pResEntry3 =
			PIMAGE_RESOURCE_DIRECTORY_ENTRY(pResDir3 + 1);

    // 指向具体的数据信息
    PIMAGE_RESOURCE_DATA_ENTRY pResDataInfo =
      PIMAGE_RESOURCE_DATA_ENTRY(pResEntry3->OffsetToDirectory + (DWORD)m_pResDir1);

		DWORD i = 0;
    // DWORD   OffsetToData;
    m_lsResDirEntry3.InsertItem(i, L"OffsetToData");
    strTmp.Format(L"%08X", pResDataInfo->OffsetToData);
    m_lsResDirEntry3.SetItemText(i, 1, strTmp);
		i++;

    // DWORD   Size;
    m_lsResDirEntry3.InsertItem(i, L"Size");
    strTmp.Format(L"%08X", pResDataInfo->Size);
    m_lsResDirEntry3.SetItemText(i, 1, strTmp);
		i++;

    // DWORD   CodePage;
    m_lsResDirEntry3.InsertItem(i, L"CodePage");
    strTmp.Format(L"%08X", pResDataInfo->CodePage);
    m_lsResDirEntry3.SetItemText(i, 1, strTmp);
		i++;

    // DWORD   Reserved;
    m_lsResDirEntry3.InsertItem(i, L"Reserved");
    strTmp.Format(L"%08X", pResDataInfo->Reserved);
    m_lsResDirEntry3.SetItemText(i, 1, strTmp);
		i++;

		CString strData;
		DWORD dwResDataRva = pResDataInfo->OffsetToData;
		DWORD dwResDataFoa = m_pe->RvaToFoa(pResDataInfo->OffsetToData);
		DWORD dwResDataSize = pResDataInfo->Size;
		PBYTE pResData = PBYTE(dwResDataFoa + m_pe->m_pImage);

		for (size_t i = 0; i < dwResDataSize; i++)
		{
			if (i % 16 == 0)
			{
				strData += "\r\n";
			}

      CString strTmpData;
      strTmpData.Format(L"%02X  ", pResData[i]);
      strData += strTmpData;

		}
		m_editResData.SetWindowTextW(strData);
	}

	*pResult = 0;
}
