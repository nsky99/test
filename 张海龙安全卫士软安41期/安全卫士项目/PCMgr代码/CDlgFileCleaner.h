#pragma once


// CDlgFileCleaner 对话框
#include "CListCtrlPlus.h"
#include <vector>
using std::vector;

class CDlgFileCleaner : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFileCleaner)

public:
	CDlgFileCleaner(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFileCleaner();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILE_CLEANER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	typedef struct _EnumFileInfo
	{
		std::vector<CString> vecDelFile;
		DOUBLE dwAllFileSize;//MB
		DWORD  dwAllFileCount;
		DOUBLE dwDelFileSize;//MB
		DWORD  dwDelFileCount;
	}EnumFileInfo, * PEnumFileInfo;

private:
	afx_msg void OnBnClickedButtonEnumSysFile();
	afx_msg void OnBnClickedButtonEnumSysdelfilelog();
	afx_msg void OnBnClickedButtonEnumDelFile();
	afx_msg void OnBnClickedButtonDelFile();
public:
	// 要删除的文件后缀
	CEdit m_editDeleteHZ;
	// 待清理文件列表
	CListCtrlPlus m_lsPath;
	EnumFileInfo  m_enumFileInfo;

	// 要删除的信息
	CEdit m_editDelInfo;
	// 删除的日志
	CListCtrl m_lsDeledFile;
	// 查询到的系统垃圾文件
	CListCtrl m_lsSysFileQuery;
	// 删除的系统垃圾日志
	CListCtrl m_lsSysFileLog;



	virtual BOOL OnInitDialog();
	// 枚举文件夹下所有文件
	void EnumFile(CString strPath, CString strType);
	void InsertToSysFileQuery();
	friend DWORD WINAPI QuerySysFile(LPVOID lpThreadParameter);
	friend DWORD WINAPI DelSysFile(LPVOID lpThreadParameter);

	afx_msg void OnBnClickedButtonClearRubbish();
	// 清理回收站
	BOOL ClearRubbish();
};
