#if !defined(AFX_PROGRESS_H__B259E59D_A94E_4871_8973_687ACD4CD6DC__INCLUDED_)
#define AFX_PROGRESS_H__B259E59D_A94E_4871_8973_687ACD4CD6DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Progress.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgress dialog

class CProgress : public CDialog
{
// Construction
public:
	void Minimize();
	void AddString(CString);
	void AddString(int id);
	void AddString(int id,int i);
//	void AddString(int id,float f);
//	void AddString(CString s0, float f);
	void AddString(int id, int f1, int f2);
	void AddString(int id, int f1, int f2, int f3);

	HICON m_hicon;
	void SetRange(int low,int high);
	void SetPos(int pos);
	void Maximize();
	bool m_cancel;
	CProgress(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgress)
	enum { IDD = IDD_PROGRESS };
	CProgressCtrl	m_progress;
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgress)
	virtual void OnCancel();
	afx_msg void OnMinimize();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESS_H__B259E59D_A94E_4871_8973_687ACD4CD6DC__INCLUDED_)
