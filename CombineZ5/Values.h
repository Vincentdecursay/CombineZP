#if !defined(AFX_VALUES_H__76C4E4BF_E55B_4BA4_8153_39C9BBAF62D2__INCLUDED_)
#define AFX_VALUES_H__76C4E4BF_E55B_4BA4_8153_39C9BBAF62D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Values.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CValues dialog

class CValues : public CDialog
{
// Construction
public:
	bool m_end;
	bool m_cancel;
	int command;
	int this_macro;
	CValues(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CValues)
	enum { IDD = IDD_PARAMS };
	CString	m_command;
	int		m_p1;
	int		m_p2;
	int		m_p3;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CValues)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CValues)
	afx_msg void OnCancel();
	afx_msg void OnEnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VALUES_H__76C4E4BF_E55B_4BA4_8153_39C9BBAF62D2__INCLUDED_)
