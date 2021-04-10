// CDlgReg.cpp: 实现文件
//

#include "pch.h"
#include "ArkApp.h"
#include "CDlgReg.h"
#include "afxdialogex.h"
#include "ArkCommon.h"

// CDlgReg 对话框

IMPLEMENT_DYNAMIC(CDlgReg, CDialogEx)

CDlgReg::CDlgReg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_REG, pParent)
	, m_strRegPath(_T(""))
{

}

CDlgReg::~CDlgReg()
{
}

void CDlgReg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SEARCHREG, m_strRegPath);
	DDX_Control(pDX, IDC_LIST_REG, m_lsReg);
}


BEGIN_MESSAGE_MAP(CDlgReg, CDialogEx)
  ON_BN_CLICKED(IDC_BTN_SEARCHREG, &CDlgReg::OnBnClickedBtnSearchreg)
END_MESSAGE_MAP()


// CDlgReg 消息处理程序


BOOL CDlgReg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

  CRect rc;
  m_lsReg.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  CString strColumnName[] = {
  L"名称",
  L"类型",
  L"数值",
  };

  m_lsReg.GetClientRect(rc);
  DWORD dwWidth = rc.right / _countof(strColumnName) - 2;

  for (size_t i = 0; i < _countof(strColumnName); i++)
  {
    m_lsReg.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
  }
	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}


void CDlgReg::OnBnClickedBtnSearchreg()
{
  // 获取路径
  UpdateData(TRUE);
  ShowRegInfo();
}


// 显示和管理Reg
void CDlgReg::ShowRegInfo()
{
  //m_lsReg.DeleteAllItems();


  //// 获取IDT数量
  //DWORD dwRet = 0;
  //DWORD dwNum = 0;
  //// 发送IO请求 - 获取数量 向驱动发送文件路径
  //// \\Registry\\Machine\\Software\\Itao
  //WCHAR szSearchPath[0xFF] = { 0 };
  //wsprintf(szSearchPath, L"%ws", m_strRegPath.GetString());
  //::DeviceIoControl(theApp.m_hArk, ArkCounteReg,
  //  szSearchPath, 0xFF, &dwNum, sizeof(DWORD), &dwRet, NULL);


  //PREGINF pRegInfo = new REGINF[dwNum]{ 0 };
  //::DeviceIoControl(theApp.m_hArk, ArkQueryReg,
  //  szSearchPath, 0xFF, pRegInfo, dwNum * sizeof(REGINF), &dwRet, NULL);

  //
  //// 显示信息
  //for (size_t i = 0; i < dwNum; i++)
  //{
  //  m_lsReg.InsertItem(i, L"");
  //}
  //WCHAR szTmp[MAX_PATH] = { 0 };
  //for (size_t i = 0; i < dwNum; i++)
  //{
  //  // 目录
  //  if (pRegInfo[i].RegType == 1)
  //  {
  //    // key name
  //    wsprintfW(szTmp, L"%ws", pRegInfo[i].KeyName);
  //    m_lsReg.SetItemText(i, 0, szTmp);

  //    // type
  //    wsprintfW(szTmp, L"-");
  //    m_lsReg.SetItemText(i, 1, szTmp);

  //    // value
  //    wsprintfW(szTmp, L"-");
  //    m_lsReg.SetItemText(i, 2, szTmp);
  //  }
  //  else
  //  {
  //    // name
  //    wsprintfW(szTmp, L"%ws", pRegInfo[i].ValueName);
  //    m_lsReg.SetItemText(i, 0, szTmp);

  //    // type
  //    wsprintfW(szTmp, L"项");
  //    m_lsReg.SetItemText(i, 1, szTmp);

  //    // value
  //    wsprintfW(szTmp, L"%u", pRegInfo[i].Value);
  //    m_lsReg.SetItemText(i, 2, szTmp);
  //  }

  //}

  //// 释放内存
  //if (pRegInfo)
  //{
  //  delete[] pRegInfo;
  //  pRegInfo = NULL;
  //}

}
