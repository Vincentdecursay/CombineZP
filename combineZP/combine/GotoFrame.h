#if !defined(AFX_GOTOFRAME_H__CE46AE2B_FBF3_4E5E_B893_E65EAA1E692B__INCLUDED_)
#define AFX_GOTOFRAME_H__CE46AE2B_FBF3_4E5E_B893_E65EAA1E692B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GotoFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGotoFrame dialog

class CGotoFrame : public CDialog
{
// Construction
public:
	int DefID;
	int new_frame;
	int top_frame;
	CGotoFrame(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGotoFrame)
	enum { IDD = IDD_GOTOFRAME };
	CListBox	m_used;
	CListBox	m_unused;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGotoFrame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGotoFrame)
	afx_msg void OnSelchangeUnused();
	afx_msg void OnSelchangeUsed();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOTOFRAME_H__CE46AE2B_FBF3_4E5E_B893_E65EAA1E692B__INCLUDED_)
