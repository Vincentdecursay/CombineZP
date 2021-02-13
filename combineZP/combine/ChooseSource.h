#pragma once
#include "afxwin.h"


// CChooseSource dialog

class CChooseSource : public CDialog
{
	DECLARE_DYNAMIC(CChooseSource)

public:
	CChooseSource(CWnd* pParent = NULL);   // standard constructor
	virtual ~CChooseSource();

// Dialog Data
	enum {IDD=IDD_CHOOSESOURCE};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	CComboBox SourceList;
	int m_chosen;
	afx_msg void OnCbnSelchangeSourcelist();
	CComboBox Resolution;
	int m_resolution;
	int m_resolution2;
	afx_msg void OnCbnSelchangeResolution();
	int m_rate;
	int minrate;
	int maxrate;
	CEdit Rate;
	CComboBox Resolution2;
	afx_msg void OnCbnSelchangeResolution2();
private:
	CString m_camname[20];
	void saveprefs();
	void loadprefs();
};
