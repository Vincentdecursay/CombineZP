#if !defined(AFX_MACRO1_H__FDE9710B_0A98_4D04_A8C1_79BAE43118A7__INCLUDED_)
#define AFX_MACRO1_H__FDE9710B_0A98_4D04_A8C1_79BAE43118A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Macro1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMacro dialog

class CMacro : public CDialog
{
// Construction
public:
	bool command_modified;
	CZMacro this_macro;
	int last_command;
	int first_command;
	CMacro(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMacro)
	enum { IDD = IDD_MACRO };
	CListBox	m_list;
	CComboBox	m_command;
	CString	m_name;
	int		m_p1;
	int		m_p2;
	int		m_p3;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMacro)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMacro)
	afx_msg void OnSelchangeCommand();
	afx_msg void OnUpdate();
	afx_msg void OnEdit();
	afx_msg void OnInsert();
	afx_msg void OnDelete();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MACRO1_H__FDE9710B_0A98_4D04_A8C1_79BAE43118A7__INCLUDED_)
