#pragma once


// CDlgModule 对话框

class CDlgModule : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgModule)

public:
	CDlgModule(CWnd* pParent = nullptr);   // 标准构造函数
	CDlgModule(ULONG ulEProcess, CWnd* pParent = nullptr);
	virtual ~CDlgModule();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_MODULE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 管理模块信息的LIST控件
	CListCtrl m_lsModule;
	// 模块EPROCESS
	ULONG m_ulEProcess;
	virtual BOOL OnInitDialog();
	// 显示模块信息
	void ShowModuleInfo();
};
