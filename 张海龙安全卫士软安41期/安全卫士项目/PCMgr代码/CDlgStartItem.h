#pragma once

#include <vector>
using std::vector;

// CDlgStartItem 对话框

class CDlgStartItem : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgStartItem)

public:
	CDlgStartItem(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgStartItem();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_START };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	/*启动项信息*/
	typedef struct _MYSTARTUPINFO
	{
		WCHAR BootName[MAX_PATH];      //启动项名
		WCHAR BootOrder[MAX_PATH];     //启动项序号
		WCHAR BootOffset[MAX_PATH];    //删除时的路径
		WCHAR BootHKEY[50];            //主键信息
		WCHAR BootAllOffset[MAX_PATH]; //启动位置
	}MYSTARTUPINFO, * PMYSTARTUPINFO;

public:
	vector<MYSTARTUPINFO> m_vecStartUp;

	CListCtrl m_lsStartItem;
	afx_msg void OnIdok();
	afx_msg void OnIdcancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// 显示启动信息
	void ShowStartUpInfo();
	// 获取启动信息
	void GetStartup();
};
