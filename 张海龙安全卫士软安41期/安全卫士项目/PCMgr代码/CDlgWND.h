#pragma once


// CDlgWND 对话框

class CDlgWND : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWND)

public:
	CDlgWND(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgWND();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	// 窗口信息列表
	CListCtrl m_lsWnd;
private:	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnIdcancel();
	afx_msg void OnIdok();

private:
	friend BOOL CALLBACK EnumWindowsProc(_In_ HWND   hwnd, _In_ LPARAM lParam);
public:
	afx_msg void OnRclickListWnd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFlushwnd();
};
