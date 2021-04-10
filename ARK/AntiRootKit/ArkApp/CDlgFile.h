#pragma once


// CDlgFile 对话框

class CDlgFile : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFile)

public:
	CDlgFile(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFile();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 当前路径
	CString m_strPath;
	// 初始化对话框
	virtual BOOL OnInitDialog();
	// 响应事件 - 鼠标左键点击查询按钮
	afx_msg void OnBnClickedBtnSearch();
	// 显示和管理文件信息
	CListCtrl m_lsFile;
	// 显示指定路径文件信息
	void ShowFileInfo();
	afx_msg void OnFileDelete();
	afx_msg void OnNMRClickListFile(NMHDR* pNMHDR, LRESULT* pResult);
};
