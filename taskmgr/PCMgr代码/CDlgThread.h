#pragma once

#include "CSnapShot.h"
// CDlgThread 对话框

class CDlgThread : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgThread)

public:
	CDlgThread(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgThread();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_THREAD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CSnapShot* m_pSnapShot;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnIdcancel();

public:
	// 设置快照指针
	void SetInfo(CSnapShot* snapShot, DWORD dwPid);
public:
	virtual BOOL OnInitDialog();
	// 线程列表框
	CListCtrl m_lsThread;
	DWORD m_Pid;
	DWORD m_CurSel;
	// 显示线程信息
	void ShowThreadInfo();
	afx_msg void OnRclickListThread(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnThreadTerminate();
	afx_msg void OnThreadSuspend();
	afx_msg void OnThreadResume();
	afx_msg void OnThreadFlush();
};
