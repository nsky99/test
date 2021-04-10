#pragma once
#include <vector>
using std::vector;

// CDlgFileMgr 对话框

class CDlgFileMgr : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFileMgr)

public:
	CDlgFileMgr(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFileMgr();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILEMGR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	typedef struct _MYFILEINFO
	{
		CString strFileName;	// 文件名
		CString strFileSize;	// 文件大小
		CString strCreateTime;// 创建时间
		CString strFileMD5;   // 文件MD5
		CString strFilePath;  // 文件路径
		CString strFileType;  // 文件类型
	}MYFILEINFO, * PMYFILEINFO;

public:
	CString m_strCurPath;
	vector<MYFILEINFO> m_vecFileInfo;
	CListCtrl m_lsFileMgr;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// 枚举文件
	void EnumFile(CString strPath);
	// 计算的文件大小
	CString GetFileSize(DWORD dwHight, DWORD dwLow);
	// 获取创建文件的时间
	CString GetFileCreateTime(FILETIME createfiletime);
	// 获取文件的MD5
	CString GetFileMD5(CString strPath);
	afx_msg void OnRclickListFilemgr(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListFilemgr(NMHDR* pNMHDR, LRESULT* pResult);
};
