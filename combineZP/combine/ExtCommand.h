#pragma once


// CExtCommand dialog

class CExtCommand : public CDialog
{
	DECLARE_DYNAMIC(CExtCommand)

public:
	CExtCommand(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExtCommand();

// Dialog Data
	enum { IDD = IDD_EXTCOMMAND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_name;
	CString m_program;
	CString m_line;
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
};
