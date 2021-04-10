#pragma once

#include "CPE.h"
// CDlgSection 对话框

class CDlgSection : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSection)

public:
	CDlgSection(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgSection();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SECTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CPE*			m_pe;
	CListCtrl m_lsSection;


	// 设置信息
	void SetValue(CPE* pPe);
	// 显示区段头信息
	void ShowSectionInfo();
};
