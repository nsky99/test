#pragma once
#include "CTabCtrlPlus.h"
#include "CPE.h"
#include "CDlgDataDir.h"
#include "CDlgSection.h"
#include "CDlgExport.h"
#include "CDlgImport.h"
#include "CDlgRelcation.h"
#include "CDlgRes.h"
#include "CDlgTLS.h"
#include "CDlgDelayImport.h"
// CDlgPE 对话框

class CDlgPE : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPE)

public:
	CDlgPE(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgPE();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnIdok();
	afx_msg void OnIdcancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropFiles(HDROP hDropInfo);

private:
	// 文件名
	CString m_strFile;
	// PE
	CPE			m_pe;
	// DOS头
	CListCtrl m_lsDOS;
	// File头
	CListCtrl m_lsFileHeader;
	// 扩展头
	CListCtrl m_lsOpHeader;

	// 数据目录对话框
	CDlgDataDir* m_pDlgDataDir;
	// 数据目录对话框
	CDlgSection* m_pDlgSection;
	// 导出表对话框
	CDlgExport* m_pDlgExport;
	// 导入表信息
	CDlgImport* m_pDlgImport;
	// 重定位表对话框
	CDlgRelcation* m_pDlgRelcation;
	// 资源表对话框
	CDlgRes* m_pDlgRes;
	// TLS
	CDlgTLS* m_pDlgTLS;
	// 延迟加载表
	CDlgDelayImport* m_pDlgDelayImport;
private:
	// 显示DOS信息
	void ShowDosHeaderInfo();
	// 显示文件头信息
	void ShowFileHeaderInfo();
	

public:
	// 显示扩展头信息
	void ShowOpHeaderInfo32();
	// 显示扩展头信息64
	void ShowOpHeaderInfo64();
	afx_msg void OnBnClickedButtonDatadir();
	afx_msg void OnBnClickedButtonSection();
	afx_msg void OnBnClickedButtonExport();
	afx_msg void OnBnClickedButtonImport();
	afx_msg void OnBnClickedButtonRlocation();
	afx_msg void OnBnClickedButtonRes();
	afx_msg void OnBnClickedButtonTls();
	afx_msg void OnBnClickedButtonDelayimport();
	CEdit m_editRVA;
	CEdit m_editVA;
	CEdit m_editFoa;
	afx_msg void OnBnClickedButtonChange();
	afx_msg void OnSetfocusEditFoa();
	afx_msg void OnSetfocusEditRva();
	afx_msg void OnSetfocusEditVa();
};
