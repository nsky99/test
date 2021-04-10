// CDlgIDT.cpp: 实现文件
//

#include "pch.h"
#include "ArkApp.h"
#include "CDlgIDT.h"
#include "afxdialogex.h"
#include "ArkCommon.h"

// CDlgIDT 对话框

IMPLEMENT_DYNAMIC(CDlgIDT, CDialogEx)

CDlgIDT::CDlgIDT(CWnd* pParent /*=nullptr*/)
  : CDialogEx(IDD_DLG_IDT, pParent)
{

}

CDlgIDT::~CDlgIDT()
{
}

void CDlgIDT::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_IDT, m_lsIDT);
}


BEGIN_MESSAGE_MAP(CDlgIDT, CDialogEx)

END_MESSAGE_MAP()


// CDlgIDT 消息处理程序


BOOL CDlgIDT::OnInitDialog()
{
  CDialogEx::OnInitDialog();


  CRect rc;
  m_lsIDT.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  CString strColumnName[] = {
  L"序号",
  L"选择子",
  L"函数地址"
  };

  m_lsIDT.GetClientRect(rc);
  DWORD dwWidth = rc.right / _countof(strColumnName) - 2;

  for (size_t i = 0; i < _countof(strColumnName); i++)
  {
    m_lsIDT.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
  }
  ShowIdtInfo();

  return TRUE;  // return TRUE unless you set the focus to a control
                // 异常: OCX 属性页应返回 FALSE
}



// 显示IDT信息
void CDlgIDT::ShowIdtInfo()
{
  m_lsIDT.DeleteAllItems();


  // 获取IDT数量
  DWORD dwRet = 0;
  DWORD dwNum = 0;
  // 发送IO请求 - 获取数量
  ::DeviceIoControl(theApp.m_hArk, ArkCountIDT,
    NULL, 0, &dwNum, sizeof(DWORD), &dwRet, NULL);

  // 根据数量申请内存
  DWORD dwMemSize = sizeof(IDT_INFO) * dwNum;
  PIDT_INFO pIdtInfo = new IDT_INFO[dwNum]{ 0 };
  if (!pIdtInfo)
  {
    return;
  }

  // 发送IO请求 - 查询信息
  ::DeviceIoControl(theApp.m_hArk, ArkQueryIDT,
    NULL, 0, pIdtInfo, dwMemSize, &dwRet, NULL);

  // 向列表框中输入信息
  for (size_t i = 0; i < dwNum; i++)
  {
    m_lsIDT.InsertItem(i, L"");
  }

  DWORD dwFunAddr = 0;
  WCHAR szTmp[MAX_PATH] = { 0 };
  for (size_t i = 0; i < dwNum; i++)
  {
    
    // 序号
    wsprintfW(szTmp, L"%u", i);
    m_lsIDT.SetItemText(i, 0, szTmp);
    // 选择子
    wsprintfW(szTmp, L"0x%02X", pIdtInfo[i].uSelector);
    m_lsIDT.SetItemText(i, 1, szTmp);
    // fun addr
    dwFunAddr = pIdtInfo[i].uOffsetLow + ((ULONG)pIdtInfo[i].uOffsetHigh << 16);
    wsprintfW(szTmp, L"0x%08X", dwFunAddr);
    m_lsIDT.SetItemText(i, 2, szTmp);

  }

  // 释放内存
  if (pIdtInfo)
  {
    delete[] pIdtInfo;
    pIdtInfo = NULL;
  }
}
