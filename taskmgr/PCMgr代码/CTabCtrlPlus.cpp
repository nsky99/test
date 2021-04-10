// CTabCtrlPlus.cpp: 实现文件
//

#include "pch.h"
#include "CTabCtrlPlus.h"
#include <stdarg.h>


// CTabCtrlPlus

IMPLEMENT_DYNAMIC(CTabCtrlPlus, CTabCtrl)

CTabCtrlPlus::CTabCtrlPlus()
{

}

CTabCtrlPlus::~CTabCtrlPlus()
{
}

// tab的Item名字
// ID
// CDialog
void CTabCtrlPlus::InitTab(UINT uCount, ...)
{
  va_list ls;
  va_start(ls, uCount);
  for (UINT uCur = 0; uCur < uCount; uCur++)
  {
    // TabItemName
    TCHAR* pszItemName = va_arg(ls, TCHAR*);
    // CDialogEx的ID
    DWORD dwDlgId = va_arg(ls, DWORD);
    // CDialogEx控件
    CDialogEx* pDlg = va_arg(ls, CDialogEx*);

    // 添加tab项并移动dlg到指定位置
    InsertItem(uCur, pszItemName);
    pDlg->Create(dwDlgId, this);

    // 获取Tab的客户端区域
    CRect rc;
    GetClientRect(rc);
    rc.DeflateRect(0, 23, 0, 0);
    pDlg->MoveWindow(rc);

    // 放入dlg的vector
    m_vecDlg.push_back(pDlg);
  }
  // 显示第一个窗口
  m_vecDlg[0]->ShowWindow(SW_SHOW);
  va_end(ls);
}


BEGIN_MESSAGE_MAP(CTabCtrlPlus, CTabCtrl)
  ON_NOTIFY_REFLECT(TCN_SELCHANGE, &CTabCtrlPlus::OnTcnSelchange)
  ON_NOTIFY_REFLECT(TCN_SELCHANGING, &CTabCtrlPlus::OnTcnSelchanging)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()



// CTabCtrlPlus 消息处理程序




void CTabCtrlPlus::OnTcnSelchange(NMHDR* pNMHDR, LRESULT* pResult)
{
  UINT uTag = GetCurSel();
  m_vecDlg[uTag]->ShowWindow(SW_SHOW);
  *pResult = 0;
}


void CTabCtrlPlus::OnTcnSelchanging(NMHDR* pNMHDR, LRESULT* pResult)
{
  UINT uSrc = GetCurSel();
  m_vecDlg[uSrc]->ShowWindow(SW_HIDE);
  *pResult = 0;
}


