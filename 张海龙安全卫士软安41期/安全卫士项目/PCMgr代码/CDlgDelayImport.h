#pragma once

#include "CPE.h"
// CDlgDelayImport 对话框

class CDlgDelayImport : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDelayImport)

public:
	CDlgDelayImport(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgDelayImport();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DELAYIMPORT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lsDelayImport;
	CPE* m_pe;

	void SetValue(CPE* pPe);
	virtual BOOL OnInitDialog();
};
