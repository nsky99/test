// CDlgInject.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgInject.h"
#include "afxdialogex.h"
#include <TlHelp32.h>

// CDlgInject 对话框

IMPLEMENT_DYNAMIC(CDlgInject, CDialogEx)

CDlgInject::CDlgInject(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_INJECT, pParent)
{

}

CDlgInject::~CDlgInject()
{
}

void CDlgInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}


BEGIN_MESSAGE_MAP(CDlgInject, CDialogEx)
END_MESSAGE_MAP()


// CDlgInject 消息处理程序
