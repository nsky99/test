#pragma once


// CDlgProcess 对话框

class CDlgProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProcess)

public:
	CDlgProcess(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgProcess();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 进程列表框
	CListCtrl m_lsProcess;
	// 初始化进程对话框
	virtual BOOL OnInitDialog();
	// 响应进程列表右键
	afx_msg void OnNMRClickListProcess(NMHDR* pNMHDR, LRESULT* pResult);
	// 显示进程信息到进程列表
	void ShowProcessInfo();
	// 响应菜单消息 - 刷新进程列表
	afx_msg void OnProcessFlush();
	// 响应菜单消息 - 暂停进程
	afx_msg void OnProcessSuspend();
	// 响应菜单消息 - 恢复进程
	afx_msg void OnProcessResume();
	// 响应菜单消息 - 杀死进程
	afx_msg void OnProcessKill();
	// 响应菜单消息 - 隐藏进程
	afx_msg void OnProcessHide();
	// 响应菜单消息 - 查看进程线程
	afx_msg void OnProcessSearshThread();
	afx_msg void OnProcessSearshModule();
	afx_msg void OnProcessSafe();
};
