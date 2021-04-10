// CDlgHeapList.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgHeapList.h"
#include "afxdialogex.h"


// CDlgHeapList 对话框

IMPLEMENT_DYNAMIC(CDlgHeapList, CDialogEx)

CDlgHeapList::CDlgHeapList(CWnd* pParent /*=nullptr*/)
  : CDialogEx(IDD_DIALOG_HEAPLIST, pParent)
{

}

CDlgHeapList::~CDlgHeapList()
{
}

void CDlgHeapList::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_HEAPLIST, m_lsHeapList);
  DDX_Control(pDX, IDC_LIST_HEAPLIST2, m_lsHeap);
}


BEGIN_MESSAGE_MAP(CDlgHeapList, CDialogEx)
  ON_COMMAND(IDCANCEL, &CDlgHeapList::OnIdcancel)
  ON_WM_CREATE()
  ON_NOTIFY(NM_CLICK, IDC_LIST_HEAPLIST, &CDlgHeapList::OnNMClickListHeaplist)
END_MESSAGE_MAP()


// CDlgHeapList 消息处理程序


// 设置遍历堆所需要的信息
void CDlgHeapList::SetInfo(CSnapShot* pSnapShot, DWORD dwPid)
{
  m_pSnapShot = pSnapShot;
  m_Pid = dwPid;
}


void CDlgHeapList::OnIdcancel()
{
  AnimateWindow(250, AW_HIDE | AW_VER_POSITIVE);
  CDialogEx::OnCancel();
}


int CDlgHeapList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CDialogEx::OnCreate(lpCreateStruct) == -1)
    return -1;

  AnimateWindow(250, AW_ACTIVATE | AW_VER_NEGATIVE);

  return 0;
}


BOOL CDlgHeapList::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  m_lsHeapList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  CString strColumnName[] =
  {
      L"进程ID",
      L"堆ID",
      L"堆标志",
  };
  CRect rc;
  m_lsHeapList.GetClientRect(rc);
  DWORD dwWidth = rc.right / _countof(strColumnName);

  for (size_t i = 0; i < _countof(strColumnName); i++)
  {
    m_lsHeapList.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
  }

  m_lsHeap.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  CString strColumnName1[] =
  {
    L"堆块句柄",
    L"堆起始地址",
    L"堆块的大小",
    L"堆块的标志",
    L"所属进程ID"
  };
  m_lsHeap.GetClientRect(rc);
  dwWidth = rc.right / _countof(strColumnName1);
  for (size_t i = 0; i < _countof(strColumnName1); i++)
  {
    m_lsHeap.InsertColumn(i, strColumnName1[i], LVCFMT_LEFT, dwWidth);
  }

  ShowHeapListInfo();
  return TRUE;  // return TRUE unless you set the focus to a control
                // 异常: OCX 属性页应返回 FALSE
}


// 显示堆列表
void CDlgHeapList::ShowHeapListInfo()
{
  m_lsHeapList.DeleteAllItems();
  m_pSnapShot->QueryProcessHeapList(m_Pid);

  for (size_t i = 0; i < m_pSnapShot->m_dwHeapListCount; i++)
  {
    m_lsHeapList.InsertItem(i, L"");
  }

  for (size_t i = 0; i < m_pSnapShot->m_dwHeapListCount; i++)
  {
    m_lsHeapList.SetItemText(i, 0, m_pSnapShot->m_vecHLInfo[i].strPid);
    m_lsHeapList.SetItemText(i, 1, m_pSnapShot->m_vecHLInfo[i].strHeapID);
    m_lsHeapList.SetItemText(i, 2, m_pSnapShot->m_vecHLInfo[i].strHeapFlag);
  }
}


void CDlgHeapList::OnNMClickListHeaplist(NMHDR* pNMHDR, LRESULT* pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
  // 当前选中行

  DWORD dwCurSel = pNMItemActivate->iItem;

  if (dwCurSel > m_pSnapShot->m_dwHeapCount)
    return;
  DWORD dwPid = _wtoi(m_pSnapShot->m_vecHLInfo[dwCurSel].strPid);
  DWORD dwHeapID = 0;
  swscanf_s(m_pSnapShot->m_vecHLInfo[dwCurSel].strHeapID, L"%x", &dwHeapID);

  m_lsHeap.DeleteAllItems();
  m_pSnapShot->QueryProcessHeap(dwPid, dwHeapID);
  

  for (size_t i = 0; i < m_pSnapShot->m_dwHeapCount; i++)
  {
    m_lsHeap.InsertItem(i, L"");
  }
  for (size_t i = 0; i < m_pSnapShot->m_dwHeapCount; i++)
  {
    m_lsHeap.SetItemText(i, 0, m_pSnapShot->m_vecHInfo[i].strHeaphandle);
    m_lsHeap.SetItemText(i, 1, m_pSnapShot->m_vecHInfo[i].strStartAddress);
    m_lsHeap.SetItemText(i, 2, m_pSnapShot->m_vecHInfo[i].strBlockSize);
    m_lsHeap.SetItemText(i, 3, m_pSnapShot->m_vecHInfo[i].strFlag);
    m_lsHeap.SetItemText(i, 4, m_pSnapShot->m_vecHInfo[i].strPid);
  }

  *pResult = 0;
}
