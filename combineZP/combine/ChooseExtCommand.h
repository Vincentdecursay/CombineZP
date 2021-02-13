#pragma once


// CChooseExtCommand dialog

class CChooseExtCommand : public CDialog
{
	DECLARE_DYNAMIC(CChooseExtCommand)

public:
	CChooseExtCommand(CWnd* pParent = NULL);   // standard constructor
	virtual ~CChooseExtCommand();

// Dialog Data
	enum { IDD = IDD_CHOOSEEXTCOMMAND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedEcom1();
	afx_msg void OnBnClickedEcom2();
	afx_msg void OnBnClickedEcom3();
	afx_msg void OnBnClickedEcom4();
	afx_msg void OnBnClickedEcom5();
	afx_msg void OnBnClickedEcom6();
	afx_msg void OnBnClickedEcom7();
	afx_msg void OnBnClickedEcom8();
	afx_msg void OnBnClickedEcom9();
	afx_msg void OnBnClickedEcom10();
	afx_msg void OnBnClickedEcom11();
	afx_msg void OnBnClickedEcom12();
	afx_msg void OnBnClickedEcom13();
};
