#pragma once


// CDlgReg 对话框

class CDlgReg : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgReg)

public:
	CDlgReg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgReg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_REG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strRegPath;
	// 注册表管理
	CListCtrl m_lsReg;
	afx_msg void OnBnClickedBtnSearchreg();
	// 显示和管理Reg
	void ShowRegInfo();
};
