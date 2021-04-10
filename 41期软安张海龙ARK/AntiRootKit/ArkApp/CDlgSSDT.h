#pragma once


// CDlgSSDT 对话框

class CDlgSSDT : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSSDT)

public:
	CDlgSSDT(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgSSDT();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SSDT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	// 显示和管理SSDT
	CListCtrl m_lsSSDT;
	// 显示SSDT信息
	void ShowSSDTInfo();
};
