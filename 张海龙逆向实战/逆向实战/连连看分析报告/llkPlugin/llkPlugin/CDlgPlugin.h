#pragma once
#include "afxdialogex.h"

// CDlgPlugin 对话框

class CDlgPlugin : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPlugin)

public:
	CDlgPlugin(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgPlugin();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PLUGIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPluginBomb();
	afx_msg void OnBnClickedButtonPluginBombKill();
	afx_msg void OnBnClickedButtonPluginCompass();
	afx_msg void OnBnClickedButtonPluginSimulation();
	afx_msg void OnBnClickedButtonPlugin5();
	afx_msg void OnBnClickedButtonPlugin6();
	afx_msg void OnBnClickedButtonPlugin7();
};
