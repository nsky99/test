#pragma once


// CDlgHeap 对话框

class CDlgHeap : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHeap)

public:
	CDlgHeap(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgHeap();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HEAP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnIdcancel();
};
