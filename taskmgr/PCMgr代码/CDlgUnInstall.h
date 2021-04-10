#pragma once
#include <vector>
using std::vector;

//64位安装路径
#define X86_SUBKEY _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
//32位安装路径
#define X64_SUBKEY _T("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall")


// CDlgUnInstall 对话框

class CDlgUnInstall : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgUnInstall)

public:
	CDlgUnInstall(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgUnInstall();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SOFTUININSTALL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	/*软件信息结构体*/
	typedef struct _SOFTINFO
	{
		WCHAR szSoftName[50]; //软件名称
		WCHAR szSoftVer[50]; //版本号
		WCHAR szSoftDate[20]; //安装日期
		WCHAR strSoftSize[MAX_PATH]; //软件大小
		WCHAR strSoftInsPath[MAX_PATH]; //安装路径
		WCHAR strSoftUniPath[MAX_PATH]; //卸载路径
		WCHAR strSoftvenRel[50]; //软件发布厂商
		WCHAR strSoftIco[MAX_PATH]; //图标路径
		WCHAR strSoftURL[MAX_PATH]; //网址链接
	}SOFTINFO, * PSOFTINFO;

public:
	vector<SOFTINFO> m_vecSoftInfo;
	// 软件列表
	CListCtrl m_lsUnInstall;
	afx_msg void OnIdok();
	afx_msg void OnIdcancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// 获取已安装软件信息
	BOOL GetSoftwareInfo();
	void ShowSoftInfo();
	afx_msg void OnUninstall();
	// 卸载软件
	BOOL DeletSoftware(LPTSTR szPath);
	afx_msg void OnNMRClickListUninstall(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFlushsoft();
};
