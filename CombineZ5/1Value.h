#if !defined(AFX_1VALUE_H__31A9F166_7E4C_4FAF_96D2_55B355D55339__INCLUDED_)
#define AFX_1VALUE_H__31A9F166_7E4C_4FAF_96D2_55B355D55339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 1Value.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// C1Value dialog

class C1Value : public CDialog
{
// Construction
public:
	C1Value(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(C1Value)
	enum { IDD = IDD_DIALOG2 };
	UINT	m_value;
	CString	m_value_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C1Value)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(C1Value)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_1VALUE_H__31A9F166_7E4C_4FAF_96D2_55B355D55339__INCLUDED_)
