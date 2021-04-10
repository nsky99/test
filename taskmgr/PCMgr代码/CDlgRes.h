#pragma once
#include "CPE.h"
#include <vector>
using std::vector;

// CDlgRes 对话框

class CDlgRes : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRes)

public:
	CDlgRes(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgRes();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RES };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CPE* m_pe;
	PIMAGE_RESOURCE_DIRECTORY m_pResDir1;
	PIMAGE_RESOURCE_DIRECTORY m_pResDir2;
	PIMAGE_RESOURCE_DIRECTORY m_pResDir3;

	vector<PIMAGE_RESOURCE_DIRECTORY_ENTRY> m_vecResDirEntry1;
	vector<PIMAGE_RESOURCE_DIRECTORY_ENTRY> m_vecResDirEntry2;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY					m_ResDirEntry3;

	CListCtrl m_lsResDirEntry1;
	CListCtrl m_lsResDirEntry2;
	CListCtrl m_lsResDirEntry3;
	CEdit     m_editResData;

	// 设置值
	void SetValue(CPE* pPe);
	// 显示第一层资源信息
	int ShowResInfo1();


	afx_msg void OnNMClickListResdirentry1(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnNMClickListResdirentry2(NMHDR* pNMHDR, LRESULT* pResult);

};
