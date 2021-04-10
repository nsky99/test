#pragma once


// CDlgGDT 对话框

class CDlgGDT : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgGDT)

public:
	CDlgGDT(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgGDT();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_GDT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 显示和管理gdt
	CListCtrl m_lsGDT;
	virtual BOOL OnInitDialog();
	// 显示GDT信息
	void ShowGdtInfo();
};
