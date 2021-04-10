
// ArkAppDlg.h: 头文件
//

#pragma once
#include "CTabCtrlPlus.h"

// CArkAppDlg 对话框
class CArkAppDlg : public CDialogEx
{
// 构造
public:
	CArkAppDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ARKAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 窗口的tab管理控件
	CTabCtrlPlus m_ctrlTabFrame;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
};
