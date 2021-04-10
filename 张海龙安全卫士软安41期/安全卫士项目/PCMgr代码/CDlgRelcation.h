#pragma once
#include "CPE.h"
#include <vector>
using std::vector;
// CDlgRelcation 对话框

class CDlgRelcation : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRelcation)

public:
	CDlgRelcation(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgRelcation();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RELATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CPE* m_pe;

	// 设置值
	void SetValue(CPE* pPe);
	CListCtrl m_lsRelcationInfo;
	CListCtrl m_lsRelcation;
	vector<PIMAGE_BASE_RELOCATION> m_vecRel;
	virtual BOOL OnInitDialog();
	// 显示重定位表头
	void ShowRelcationInfo();
	afx_msg void OnItemchangedListRelocation(NMHDR* pNMHDR, LRESULT* pResult);
};
