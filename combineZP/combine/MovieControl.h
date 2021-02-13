#pragma once


// CMovieControl dialog

class CMovieControl : public CDialog
{
	DECLARE_DYNAMIC(CMovieControl)

public:
	CMovieControl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMovieControl();

// Dialog Data
	enum { IDD = IDD_GETMOVIE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMstart();
	afx_msg void OnBnClickedMend();
	afx_msg void OnBnClickedMcancel();
	int m_state;
};
