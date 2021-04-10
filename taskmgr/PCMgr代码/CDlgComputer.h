#pragma once

//#include "CButtonPlus.h"
// CDlgComputer 对话框

class CDlgComputer : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgComputer)

public:
	CDlgComputer(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgComputer();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_COMPUTER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// CPU进度条
	CProgressCtrl m_progressCPU;
	CStatic m_staticCPU;
	HANDLE  m_hTGetCPUUsage;

	// 内存使用率
	CProgressCtrl m_progressMem;
	CStatic m_staticMem;
	HANDLE  m_hTGetMemUsage;

	// 内存优化
	HANDLE m_hTOptimizeMemory;

private:
	// 获取CPU使用率
	int GetNowCpuUse();
	int GetNowMemoryUse();
public:
	friend DWORD WINAPI  SetCpuUsage(LPVOID lpThreadParameter);
	friend DWORD WINAPI  SetMemUsage(LPVOID lpThreadParameter);
	friend DWORD WINAPI  OptimizeMemory(LPVOID lpThreadParameter);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonMem();
	afx_msg void OnIdcancel();
	afx_msg void OnIdok();
	afx_msg void OnBnClickedButtonShutdown();
	afx_msg void OnBnClickedButtonReboot();
	afx_msg void OnBnClickedButtonSleep();
	afx_msg void OnBnClickedButtonSleeped();
	afx_msg void OnBnClickedButtonLock();
	afx_msg void OnBnClickedButtonUnlogn();
};
