#pragma once


// CDlgThread 对话框

class CDlgThread : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgThread)

public:
	CDlgThread(CWnd* pParent = nullptr);   // 标准构造函数
	CDlgThread(ULONG ulEProcess, CWnd* pParent = nullptr);
	virtual ~CDlgThread();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_THREAD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 用于显示线程信息
	CListCtrl m_lsThread;
	// 线程所属进程的EPROCESS
	ULONG m_ulEProcess;

	// 初始化对话框
	virtual BOOL OnInitDialog();
	// 显示线程信息
	void ShowThreadInfo();
	// 响应鼠标右键消息 - 右键单击线程列表框
	afx_msg void OnNMRClickListThread(NMHDR* pNMHDR, LRESULT* pResult);
	// 响应菜单消息 - 暂停线程
	afx_msg void OnThreadSuspend();
	// 响应菜单消息 - 恢复线程
	afx_msg void OnThreadResume();
	// 响应菜单消息 - 结束线程
	afx_msg void OnThreadKill();
	// 响应菜单消息 - 刷新线程
	afx_msg void OnThreadFlush();
};
