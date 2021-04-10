#pragma once

#include "CPE.h"
// CDlgImport 对话框

class CDlgImport : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImport)

public:
	CDlgImport(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgImport();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_IMPORT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lsImportInfo;
	CListCtrl m_lsImport;
	CPE* m_pe;

	// 显示导入表信息
	void ShowImportInfo();
	void SetValue(CPE* pPe);
	afx_msg void OnItemchangedListImportinfo(NMHDR* pNMHDR, LRESULT* pResult);
};
