// CDlgHeap.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgHeap.h"
#include "afxdialogex.h"


// CDlgHeap 对话框

IMPLEMENT_DYNAMIC(CDlgHeap, CDialogEx)

CDlgHeap::CDlgHeap(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HEAP, pParent)
{

}

CDlgHeap::~CDlgHeap()
{
}

void CDlgHeap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgHeap, CDialogEx)
	ON_WM_CREATE()
	ON_COMMAND(IDCANCEL, &CDlgHeap::OnIdcancel)
END_MESSAGE_MAP()


// CDlgHeap 消息处理程序


int CDlgHeap::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	AnimateWindow(250, AW_ACTIVATE | AW_VER_NEGATIVE);

	return 0;
}


void CDlgHeap::OnIdcancel()
{
	AnimateWindow(250, AW_HIDE | AW_VER_POSITIVE);
	CDialogEx::OnCancel();
}
