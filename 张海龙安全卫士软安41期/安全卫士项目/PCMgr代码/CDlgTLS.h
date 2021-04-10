#pragma once

#include "CPE.h"

// CDlgTLS 对话框

class CDlgTLS : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTLS)

public:
	CDlgTLS(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgTLS();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TLS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CPE* m_pe;
	void SetValue(CPE* pPe);
	CListCtrl m_lsTLS;
	virtual BOOL OnInitDialog();
};
