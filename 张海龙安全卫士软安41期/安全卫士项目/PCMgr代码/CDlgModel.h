#pragma once

#include "CSnapShot.h"
// CDlgModel 对话框

class CDlgModel : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgModel)

public:
	CDlgModel(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgModel();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MODEL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnIdcancel();
	// 设置初始化信息
	void SetInfo(CSnapShot* pSnapShot, DWORD dwPid);
	// 指向快照信息的指针
	CSnapShot* m_pSnapShot;
	// 模块所属进程ID
	DWORD m_Pid;
	// 显示模块的列表控件
	CListCtrl m_lsModel;
	virtual BOOL OnInitDialog();
	// 显示模块列表信息
	void ShowModelInfo();
};
