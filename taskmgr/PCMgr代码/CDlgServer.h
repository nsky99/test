#pragma once


// CDlgServer 对话框
#include <vector>
using std::vector;
class CDlgServer : public CDialogEx
{
private:
	//###服务 信息
	typedef struct _SERVICEINFO
	{
		CString szServiceName;		// 服务名
		CString szBinaryPathName; // 全路径
		CString ServiceStartName; // 服务启动用户名
		CString szDisplayName;		// 显示名称
		CString ServiceType;			// 版本类型
		CString StartType;				// 开始类型
	}SERVICEINFO, * PSERVICEINFO;


	DECLARE_DYNAMIC(CDlgServer)

public:
	CDlgServer(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgServer();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SERVER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:

	CListCtrl m_lsServer;
	vector<SERVICEINFO> m_vecServiceInfo;

	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	// 显示服务信息
	void ShowServerInfo();
public:
	// 获取服务信息
	void GetSericesInfo();
	afx_msg void OnRclickListServer(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFlushserver();
	afx_msg void OnStartserver();
	afx_msg void OnStopserver();
	// 关闭服务
	BOOL CloseThisVerices(CString serverName);
	BOOL StartThisVerices(CString serverName);
};
