#pragma once
#include <md5.h>

#pragma comment(lib, "libcrypto.lib")
#include <vector>
using std::vector;


// CDlgVirus 对话框

class CDlgVirus : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVirus)

public:
	CDlgVirus(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgVirus();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VIRUS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	typedef struct _VirusLibInfo
	{
		WCHAR strVirusName[MAX_PATH]; // 病毒名称
		WCHAR strMD5[MAX_PATH];       // 病毒MD5
	} VirusLibInfo, * PVirusLibInfo;

public:
	// 是否正在扫描
	BOOL m_bIsScaning;
	// 扫描病毒的工作线程
	HANDLE m_hScanf;
	// 病毒库中的病毒信息
	vector<VirusLibInfo> m_vecVirusLib;
	// 全盘扫描
	CButton m_radioOverAll;
	// 指定路径
	CButton m_radioSpecifyPath;
	// 联网杀毒
	CButton m_checkInternate;
	// 扫描结果
	CListCtrl m_lsScanResult;
	// 病毒库信息
	CListCtrl m_lsVirusLib;
	// 开始和结束扫描
	CButton m_buttonScan;
	// 扫描路径
	CStatic m_staticPath;
	// 设置的扫描路径
	CString m_strChoosePath;
	// 扫描过的文件个数
	DWORD dwScanedNumber;
	// 扫描信息
	CStatic m_groupScanInfo;
	// 黑名单列表
	CListCtrl m_lsBlackList;
	// 病毒库中的病毒信息
	vector<VirusLibInfo> m_vecBlackList;


	afx_msg void OnIdok();
	afx_msg void OnIdcancel();
	// 加载默认本地病毒库
	void LoadLocalVirusLib();
	afx_msg void OnDeletevirus();
	afx_msg void OnInsertvirus();
	afx_msg void OnNMRClickListViruslib(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedRadioOverall();
	afx_msg void OnBnClickedRadioSpecifypath();
	afx_msg void OnBnClickedButtonScan();
	afx_msg void OnBnClickedButtonDeletevirus();

	// 获取文件路径（对话框获取）
	CString GetFilePath();
	// 获取文件的
	CString GetFileMD5(CString strFilePath);
	// 根据路径获取文件名称
	CString GetFileName(CString strFilePath);
	// 获取全部磁盘
	vector<CString> GetDriverList();
	// 扫描病毒
	void ScanFile(CString strPath);

	// 扫描病毒的工作线程
	friend DWORD WINAPI ScanFileProc(LPVOID lpThreadParameter);
	// 检测当前线程是否结束
	friend DWORD WINAPI IsScanEndProc(LPVOID lpThreadParameter);

	// 链接服务器进行扫描杀毒
	void ScanFileForServer(CString strPath, SOCKET* client);
	afx_msg void OnUpdataserver();
	// 加密数据
	void EnCode(char* buf);

	CListCtrl m_lsBlackSelectInfo;
	afx_msg void OnBnClickedButtonBlacksearch();
	afx_msg void OnNMRClickListBlack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddblacklist();
	afx_msg void OnDeleteblacklist();
	afx_msg void OnBnClickedButtonDeleteblacklistfile();
};
