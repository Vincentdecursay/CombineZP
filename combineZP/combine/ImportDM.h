#if !defined(AFX_IMPORTDM_H__14E4F0F7_D89E_420F_86C3_4524912347A7__INCLUDED_)
#define AFX_IMPORTDM_H__14E4F0F7_D89E_420F_86C3_4524912347A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportDM.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImportDM dialog

class CImportDM : public CDialog
{
// Construction
public:
	CImportDM(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImportDM)
	enum { IDD = IDD_IMPORTDM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportDM)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportDM)
	afx_msg void OnReddata();
	afx_msg void OnGreendata();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTDM_H__14E4F0F7_D89E_420F_86C3_4524912347A7__INCLUDED_)
