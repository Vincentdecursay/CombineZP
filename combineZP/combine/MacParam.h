#pragma once
#include "afxwin.h"
#include "atltypes.h"


// CMacParam dialog

class CMacParam : public CDialog
{
	DECLARE_DYNAMIC(CMacParam)

public:
	CMacParam(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMacParam();

// Dialog Data
	enum { IDD = IDD_MACPARAMS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int value1;
	int value2;
	int value3;
	int value4;
	int value5;
	int value6;
	int value7;
	int value8;
	int value9;
	int value10;
	CString name1;
	CString name2;
	CString name3;
	CString name4;
	CString name5;
	CString name6;
	CString name7;
	CString name8;
	CString name9;
	CString name10;
	bool enable;
	RECT r;
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
};
