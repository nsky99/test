#pragma once

#include <vector>
using std::vector;
// CMyTab

class CTabCtrlPlus : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabCtrlPlus)

public:
	CTabCtrlPlus();
	virtual ~CTabCtrlPlus();
	void InitTab(UINT uCount, ...);
protected:
	DECLARE_MESSAGE_MAP()
private:
	// ����dlg�Ķ�̬����
	vector<CDialogEx*> m_vecDlg;
public:
	CString m_strFile;

public:
	afx_msg void OnTcnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTcnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
};


