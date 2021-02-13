#if !defined(AFX_CHOOSEMACRO1_H__2ED754D5_C4FB_4F93_91DB_61DE706A9845__INCLUDED_)
#define AFX_CHOOSEMACRO1_H__2ED754D5_C4FB_4F93_91DB_61DE706A9845__INCLUDED_

#include "combineDoc.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseMacro1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChooseMacro dialog

class CChooseMacro : public CDialog
{
// Construction
public:
	CChooseMacro(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChooseMacro)
	enum { IDD = IDD_CHOOSEMACRO };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseMacro)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChooseMacro)
	afx_msg void OnMacro1();
	afx_msg void OnMacro2();
	afx_msg void OnMacro3();
	afx_msg void OnMacro4();
	afx_msg void OnMacro5();
	afx_msg void OnMacro6();
	afx_msg void OnMacro7();
	afx_msg void OnMacro8();
	afx_msg void OnMacro9();
	afx_msg void OnMacro10();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSEMACRO1_H__2ED754D5_C4FB_4F93_91DB_61DE706A9845__INCLUDED_)
