// CDlgSSDT.cpp: 实现文件
//

#include "pch.h"
#include "ArkApp.h"
#include "CDlgSSDT.h"
#include "afxdialogex.h"
#include "ArkCommon.h"

// CDlgSSDT 对话框

IMPLEMENT_DYNAMIC(CDlgSSDT, CDialogEx)

CDlgSSDT::CDlgSSDT(CWnd* pParent /*=nullptr*/)
  : CDialogEx(IDD_DLG_SSDT, pParent)
{

}

CDlgSSDT::~CDlgSSDT()
{
}

void CDlgSSDT::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_SSDT, m_lsSSDT);
}


BEGIN_MESSAGE_MAP(CDlgSSDT, CDialogEx)
END_MESSAGE_MAP()


// CDlgSSDT 消息处理程序


BOOL CDlgSSDT::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  CRect rc;
  m_lsSSDT.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  CString strColumnName[] = {
  L"服务号",
  L"函数地址",
  L"参数个数",
  };

  m_lsSSDT.GetClientRect(rc);
  DWORD dwWidth = rc.right / _countof(strColumnName) - 2;

  for (size_t i = 0; i < _countof(strColumnName); i++)
  {
    m_lsSSDT.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
  }


  ShowSSDTInfo();
  return TRUE;  // return TRUE unless you set the focus to a control
                // 异常: OCX 属性页应返回 FALSE
}


// 显示SSDT信息
void CDlgSSDT::ShowSSDTInfo()
{
  m_lsSSDT.DeleteAllItems();

  // 查询数量
  DWORD dwRet = 0;
  DWORD dwNum = 0;
  ::DeviceIoControl(theApp.m_hArk, ArkCountSSDT,
    NULL, 0, &dwNum, sizeof(DWORD), &dwRet, NULL);


  // 申请内存
  DWORD dwMemSize = sizeof(SSDT_INFO) * dwNum;
  PSSDT_INFO pSSDTInfo = new SSDT_INFO[dwNum]{ 0 };
  if (!pSSDTInfo)
  {
    return;
  }

  // 查询信息
  ::DeviceIoControl(theApp.m_hArk, ArkQuerySSDT,
    NULL, 0, pSSDTInfo, dwMemSize, &dwRet, NULL);

  // 显示信息
  for (size_t i = 0; i < dwNum; i++)
  {
    m_lsSSDT.InsertItem(i, L"");
  }
  WCHAR szTmp[MAX_PATH] = { 0 };
  for (size_t i = 0; i < dwNum; i++)
  {
    // 服务号
    wsprintfW(szTmp, L"0x%08X", pSSDTInfo[i].ulServerNum);
    m_lsSSDT.SetItemText(i, 0, szTmp);

    // 函数地址
    wsprintfW(szTmp, L"0x%08X", pSSDTInfo[i].ulFunAddr);
    m_lsSSDT.SetItemText(i, 1, szTmp);

    // 参数个数
    wsprintfW(szTmp, L"%u", pSSDTInfo[i].ulCountArgs);
    m_lsSSDT.SetItemText(i, 2, szTmp);
  }

  // 释放内存
  if (pSSDTInfo)
  {
    delete[] pSSDTInfo;
    pSSDTInfo = NULL;
  }
}
