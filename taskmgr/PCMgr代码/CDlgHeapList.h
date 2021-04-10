#pragma once


// CDlgHeapList 对话框
#include "CSnapShot.h"

class CDlgHeapList : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHeapList)

public:
	CDlgHeapList(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgHeapList();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HEAPLIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	// 堆列表的list控件
	CListCtrl  m_lsHeapList;
	// 指向快照
	CSnapShot* m_pSnapShot;
	// 进程ID
	DWORD      m_Pid;
public:
	// 设置遍历堆所需要的信息
	void SetInfo(CSnapShot* pSnapShot, DWORD dwPid);
	afx_msg void OnIdcancel();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
private:
	// 显示堆列表
	void ShowHeapListInfo();
public:
	// 堆信息列表框
	CListCtrl m_lsHeap;
	afx_msg void OnNMClickListHeaplist(NMHDR* pNMHDR, LRESULT* pResult);
};
