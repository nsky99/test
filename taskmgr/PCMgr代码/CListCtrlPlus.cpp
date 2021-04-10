// CListCtrlPlus.cpp: 实现文件
//

#include "pch.h"

#include "CListCtrlPlus.h"


// CListCtrlPlus

IMPLEMENT_DYNAMIC(CListCtrlPlus, CListCtrl)

CListCtrlPlus::CListCtrlPlus()
{

}

CListCtrlPlus::~CListCtrlPlus()
{
}


BEGIN_MESSAGE_MAP(CListCtrlPlus, CListCtrl)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()



// CListCtrlPlus 消息处理程序




void CListCtrlPlus::OnDropFiles(HDROP hDropInfo)
{
	CString strPath;

	DragQueryFile(hDropInfo, 0, strPath.GetBuffer(MAX_PATH), MAX_PATH);
	
	InsertItem(0, _T(""));
	SetItemText(0, 0, strPath);
	CListCtrl::OnDropFiles(hDropInfo);
}
