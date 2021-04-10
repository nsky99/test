#pragma once

#include "CSnapShot.h"
#include "CDlgThread.h"
#include "CDlgModel.h"
#include "CDlgHeapList.h"
#include "CDlgWND.h"
// CDlgProcess 对话框

class CDlgProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProcess)

public:
	CDlgProcess(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgProcess();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	// 私有成员函数
private:
	// 初始化进程列表框
	void InitProcessList();


	// 私有成员对象
private:
	// 进程列表框
	CListCtrl m_lsProcess;
	// 快照信息
	CSnapShot m_SnapShot;
	// 当前选择行
	DWORD     m_CurSel;
	// 线程对话框
	CDlgThread* m_pDlgThread;
	// 模块对话框
	CDlgModel*  m_pDlgModel;
	// 堆列表对话框
	CDlgHeapList* m_pDlgHeapList;
	// 要注入的dll路径
	CString m_strDllName;
	// 显示进程信息
	void ShowProcessInfo();
public:
	afx_msg void OnIdok();
	afx_msg void OnIdcancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRclickListProcess(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFlushProcess();
	afx_msg void OnProcessTerminate();
	afx_msg void OnProcessStop();
	afx_msg void OnProcessResume();
	afx_msg void OnRealtimePriorityClass();
	afx_msg void OnHighPriorityClass();
	afx_msg void OnAboveNormalPriorityClass();
	afx_msg void OnNormalPriorityClass();
	afx_msg void OnBelowNormalPriorityClass();
	afx_msg void OnIdlePriorityClass();
	afx_msg void OnOpenPath();
	afx_msg void OnAttritube();
	afx_msg void OnShowThread();
	afx_msg void OnShowModle();
	afx_msg void OnShowHeaplist();
	afx_msg void OnProtectprocess();

	DWORD GetInjectPid(CStringA strName);
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
