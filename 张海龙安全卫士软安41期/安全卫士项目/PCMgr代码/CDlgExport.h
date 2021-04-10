#pragma once

#include "CPE.h"
// CDlgExport 对话框

class CDlgExport : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgExport)

public:
	CDlgExport(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgExport();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EXPORT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lsExport;
	CListCtrl m_lsExportInfo;
	CPE*			m_pe;

	// 显示导出表信息
	void ShowExportInfo();
	// 显示导出表
	void ShowExport();
	void SetValue(CPE* pPe);
};
