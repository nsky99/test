// CDlgFile.cpp: 实现文件
//

#include "pch.h"
#include "ArkApp.h"
#include "CDlgFile.h"
#include "afxdialogex.h"
#include "ArkCommon.h"

// CDlgFile 对话框

IMPLEMENT_DYNAMIC(CDlgFile, CDialogEx)

CDlgFile::CDlgFile(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_FILE, pParent)
	, m_strPath(_T(""))
{

}

CDlgFile::~CDlgFile()
{
}

void CDlgFile::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_SEARCH, m_strPath);
  DDX_Control(pDX, IDC_LIST_FILE, m_lsFile);
}


BEGIN_MESSAGE_MAP(CDlgFile, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CDlgFile::OnBnClickedBtnSearch)
    ON_COMMAND(ID_FILE_DELETE, &CDlgFile::OnFileDelete)
  ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CDlgFile::OnNMRClickListFile)
END_MESSAGE_MAP()


// CDlgFile 消息处理程序

// 初始化对话框
BOOL CDlgFile::OnInitDialog()
{
	CDialogEx::OnInitDialog();

  CRect rc;
  m_lsFile.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  CString strColumnName[] = {
  L"文件名称",
  L"文件大小",
  L"类型",
  };

  m_lsFile.GetClientRect(rc);
  DWORD dwWidth = rc.right / _countof(strColumnName) - 2;

  for (size_t i = 0; i < _countof(strColumnName); i++)
  {
    m_lsFile.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
  }

	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}


// 响应消息 - 查询按钮
void CDlgFile::OnBnClickedBtnSearch()
{
	// 获取路径
	UpdateData(TRUE);
  ShowFileInfo();
}



void CDlgFile::ShowFileInfo()
{
  m_lsFile.DeleteAllItems();


  // 获取IDT数量
  DWORD dwRet = 0;
  DWORD dwNum = 0;
  // 发送IO请求 - 获取数量 向驱动发送文件路径
  WCHAR szSearchPath[0xFF] = { 0 };
  wsprintf(szSearchPath, L"\\??\\%ws", m_strPath.GetString());
  ::DeviceIoControl(theApp.m_hArk, ArkCountFile,
    szSearchPath, 0xFF, &dwNum, sizeof(DWORD), &dwRet, NULL);
  if (!dwNum)
  {
    MessageBox(L"指定目录下没有文件");
    return;
  }

  // 申请内存保存内核查询到的文件信息
  DWORD dwMemSize = sizeof(FILE_INFO) * dwNum;
  PFILE_INFO pFileInfo = new FILE_INFO[dwNum]{ 0 };
  if (!pFileInfo)
  {
    pFileInfo = NULL;
    return;
  }

  // Io请求 - 查询文件信息
  ::DeviceIoControl(theApp.m_hArk, ArkQueryFile,
    szSearchPath, 0xFF, pFileInfo, dwMemSize, &dwRet, NULL);


  // 显示信息
  for (size_t i = 0; i < dwNum; i++)
  {
    m_lsFile.InsertItem(i, L"");
  }
  WCHAR szTmp[0xFF] = { 0 };
  for (size_t i = 0; i < dwNum; i++)
  {
    // name
    m_lsFile.SetItemText(i, 0, pFileInfo[i].szName);
    // size 
    if (0 == wcscmp(pFileInfo[i].szType, L"文件"))
    {
      wsprintfW(szTmp, L"%I64u字节", pFileInfo[i].ulSize);
      m_lsFile.SetItemText(i, 1, szTmp);
    }
    else
    {
      wsprintfW(szTmp, L" - ");
      m_lsFile.SetItemText(i, 1, szTmp);
    }
    // type
    m_lsFile.SetItemText(i, 2, pFileInfo[i].szType);
  }



  // 释放内存
  if (pFileInfo)
  {
    delete[] pFileInfo;
    pFileInfo = NULL;
  }
}

void CDlgFile::OnFileDelete()
{
  DWORD dwRet = 0;
  // 发送IO请求 - 获取数量 向驱动发送文件路径
  WCHAR szDelPath[0xFF] = { 0 };

  // 获取当前选择行
  UINT uCurSelected = m_lsFile.GetSelectionMark();
  // 获取当前选中行的FileName
  CString strFileName = m_lsFile.GetItemText(uCurSelected, 0);
  wsprintf(szDelPath, L"\\??\\%ws%ws", m_strPath.GetString(), strFileName.GetString());
  ::DeviceIoControl(theApp.m_hArk, ArkDeleteFile,
    szDelPath, 0xFF, NULL, 0, &dwRet, NULL);

  m_lsFile.DeleteItem(uCurSelected);
}


void CDlgFile::OnNMRClickListFile(NMHDR* pNMHDR, LRESULT* pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
  
  POINT point;
  GetCursorPos(&point);

  // 加载主菜单
  CMenu popMainMenu;
  popMainMenu.LoadMenuW(IDR_MENU1);

  CMenu* pSubMenu = popMainMenu.GetSubMenu(3);
  pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);


  *pResult = 0;
}
