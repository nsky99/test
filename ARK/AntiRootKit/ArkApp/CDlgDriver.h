#pragma once


// CDlgDriver 对话框

class CDlgDriver : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDriver)

public:
	CDlgDriver(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgDriver();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DRIVER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 响应对话框事件 - 初始化对话框
	virtual BOOL OnInitDialog();
	// 驱动列表框
	CListCtrl m_lsDriver;
	// 显示驱动信息
	void ShowDriverInfo();

	// 右键点击事件 - 弹出驱动菜单
	afx_msg void OnNMRClickListDriver(NMHDR* pNMHDR, LRESULT* pResult);
	// 响应菜单事件 - 刷新驱动
	afx_msg void OnDriverFlush();
	// 响应菜单事件 - 隐藏驱动
	afx_msg void OnDriverHide();

};
