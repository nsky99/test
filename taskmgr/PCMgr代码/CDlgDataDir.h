#pragma once

#include "CPE.h"
// CDlgDataDir 对话框

class CDlgDataDir : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDataDir)

public:
	CDlgDataDir(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgDataDir();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DATADIR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CPE*			m_pe;
	CListCtrl m_lsDataDir;
	virtual BOOL OnInitDialog();
	// 接收信息
	void SetValue(CPE* pPe);
	// 显示数据目录信息
	void ShowDataDirInfo();
};
