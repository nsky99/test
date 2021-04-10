
// PCMgrDlg.h: 头文件
//

#pragma once

#include "CTabCtrlPlus.h"


// CPCMgrDlg 对话框
class CPCMgrDlg : public CDialogEx
{
// 构造
public:
	CPCMgrDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PCMGR };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	// 窗口管理对象
	CTabCtrlPlus m_tabMgr;
public:
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnIdok();
	afx_msg void OnIdcancel();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);

	// 注册热键
	void RegisterHotKey();
	// 卸载热键
	void UnRegisterHotKey();
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	void LoadSkin();
};
