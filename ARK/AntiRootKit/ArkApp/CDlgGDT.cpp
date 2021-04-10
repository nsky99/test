// CDlgGDT.cpp: 实现文件
//

#include "pch.h"
#include "ArkApp.h"
#include "CDlgGDT.h"
#include "afxdialogex.h"
#include "ArkCommon.h"

// CDlgGDT 对话框

IMPLEMENT_DYNAMIC(CDlgGDT, CDialogEx)

CDlgGDT::CDlgGDT(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_GDT, pParent)
{

}

CDlgGDT::~CDlgGDT()
{
}

void CDlgGDT::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GDT, m_lsGDT);
}


BEGIN_MESSAGE_MAP(CDlgGDT, CDialogEx)
END_MESSAGE_MAP()


// CDlgGDT 消息处理程序


BOOL CDlgGDT::OnInitDialog()
{
	CDialogEx::OnInitDialog();

  CRect rc;
  m_lsGDT.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  CString strColumnName[] = {
  L"序号",
  L"Base",
  L"Limit",
  L"段粒度",
  L"DPL",
  L"类型"
  };

  m_lsGDT.GetClientRect(rc);
  DWORD dwWidth = rc.right / _countof(strColumnName) - 2;

  for (size_t i = 0; i < _countof(strColumnName); i++)
  {
    m_lsGDT.InsertColumn(i, strColumnName[i], LVCFMT_LEFT, dwWidth);
  }
  ShowGdtInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
								// 异常: OCX 属性页应返回 FALSE
}


// 显示GDT信息
void CDlgGDT::ShowGdtInfo()
{
  m_lsGDT.DeleteAllItems();


  // 获取IDT数量
  DWORD dwRet = 0;
  DWORD dwNum = 0;
  // 发送IO请求 - 获取数量
  ::DeviceIoControl(theApp.m_hArk, ArkCountGDT,
    NULL, 0, &dwNum, sizeof(DWORD), &dwRet, NULL);

  // 根据数量申请内存
  DWORD dwMemSize = sizeof(GDT_INFO) * dwNum;
  PGDT_INFO pGdtInfo = new GDT_INFO[dwNum]{ 0 };
  if (!pGdtInfo)
  {
    return;
  }
  WCHAR szTmp1[MAX_PATH] = { 0 };
  wsprintfW(szTmp1, L"0x%p\n", pGdtInfo);
  OutputDebugString(szTmp1);

  // 发送IO请求 - 查询信息
  ::DeviceIoControl(theApp.m_hArk, ArkQueryGDT,
    NULL, 0, pGdtInfo, dwMemSize, &dwRet, NULL);
  // 向列表框中输入信息
  for (size_t i = 0; i < dwNum; i++)
  {
    m_lsGDT.InsertItem(i, L"");
  }

  DWORD dwBase = 0;
  DWORD dwLimit = 0;
  WCHAR szTmp[MAX_PATH] = { 0 };
  for (size_t i = 0; i < dwNum; i++)
  {
    // 序号
    wsprintfW(szTmp, L"%u", i);
    m_lsGDT.SetItemText(i, 0, szTmp);
    // Base
    dwBase = pGdtInfo[i].uiLowBase1 + (pGdtInfo[i].uiLowBase2 << 16) + (pGdtInfo[i].uiHighBase << 24);
    wsprintfW(szTmp, L"0x%08X", dwBase);
    m_lsGDT.SetItemText(i, 1, szTmp);
    // limit
    // 字节
    if (pGdtInfo[i].uiG == 0)
    {
      dwLimit = pGdtInfo[i].uiLowLimit + (pGdtInfo[i].ulHighLimit << 16);
    }
    // page
    else
    {
      dwLimit = ((pGdtInfo[i].uiLowLimit + (pGdtInfo[i].ulHighLimit << 16)) + 1) * 0x1000 - 1;
    }
    wsprintfW(szTmp, L"0x%08X", dwLimit);
    m_lsGDT.SetItemText(i, 2, szTmp);

    // 段粒度
    if (pGdtInfo[i].uiG == 0)
    {
      wsprintfW(szTmp, L"Byte");
    }
    else
    {
      wsprintfW(szTmp, L"Page");
    }
    m_lsGDT.SetItemText(i, 3, szTmp);
    // DPL
    wsprintfW(szTmp, L"%u", pGdtInfo[i].uiDPL);
    m_lsGDT.SetItemText(i, 4, szTmp);
    // Attr
    if (pGdtInfo[i].uiS == 0)
    {
      switch (pGdtInfo[i].uiType)
      {
      case 5:
        wsprintfW(szTmp, L"任务门描述符");
        break;
      case 9:
        wsprintfW(szTmp, L"32bit tss (available)");
        break;
      case 11:
        wsprintfW(szTmp, L"32bit tss (busy)");
        break;
      case 12:
        wsprintfW(szTmp, L"32bit call gate");
        break;
      case 14:
        wsprintfW(szTmp, L"32bit 中断门");
        break;
      case 15:
        wsprintfW(szTmp, L"32bit 陷阱门");
        break;
      default:
        wsprintfW(szTmp, L"-");
        break;
      }
    }
    else
    {
      switch (pGdtInfo[i].uiType)
      {
      case 0:
        wsprintfW(szTmp, L"只读数据段");
        break;
      case 1:
        wsprintfW(szTmp, L"只读-已访问数据段");
        break;
      case 2:
        wsprintfW(szTmp, L"读-写数据段");
        break;
      case 3:
        wsprintfW(szTmp, L"读-写已访问数据段");
        break;
      case 4:
        wsprintfW(szTmp, L"只读-向下拓展数据段");
        break;
      case 5:
        wsprintfW(szTmp, L"只读-向下拓展-已访问数据段");
        break;
      case 6:
        wsprintfW(szTmp, L"读-写-向下拓展-数据段");
        break;
      case 7:
        wsprintfW(szTmp, L"读-写-向下拓展-已访问数据段");
        break;
      case 8:
        wsprintfW(szTmp, L"X-代码段");
        break;
      case 9:
        wsprintfW(szTmp, L"X-A-代码段");
        break;
      case 10:
        wsprintfW(szTmp, L"X-R-代码段");
        break;
      case 11:
        wsprintfW(szTmp, L"X-R-A代码段");
        break;
      case 12:
        wsprintfW(szTmp, L"X-一致代码段");
        break;
      case 13:
        wsprintfW(szTmp, L"X-A一致代码段");
        break;
      case 14:
        wsprintfW(szTmp, L"X-R一致代码段");
        break;
      case 15:
        wsprintfW(szTmp, L"X-R-A一致代码段");
        break;
      default:
        wsprintfW(szTmp, L"-");
        break;
      }
    }
    m_lsGDT.SetItemText(i, 5, szTmp);
  }

  // 释放内存
  if (pGdtInfo)
  {
    delete[] pGdtInfo;
    pGdtInfo = NULL;
  }
}
