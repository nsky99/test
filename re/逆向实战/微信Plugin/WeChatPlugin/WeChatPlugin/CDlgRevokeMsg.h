#pragma once

#include "afxdialogex.h"
// CDlgRevokeMsg 对话框

class CDlgRevokeMsg : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRevokeMsg)

public:
	CDlgRevokeMsg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgRevokeMsg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLGREVOKEMSG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 监控的撤回消息
	CEdit m_editRevokeMsg;
	virtual BOOL OnInitDialog();

};
