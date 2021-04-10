#pragma once


// CDlgInject 对话框

class CDlgInject : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgInject)

public:
	CDlgInject(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgInject();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INJECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

};
