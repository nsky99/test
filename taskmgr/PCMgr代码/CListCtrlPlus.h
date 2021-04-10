#pragma once


// CListCtrlPlus

class CListCtrlPlus : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlPlus)

public:
	CListCtrlPlus();
	virtual ~CListCtrlPlus();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};


