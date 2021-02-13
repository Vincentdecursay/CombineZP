#pragma once


// CMovieParams dialog

class CMovieParams : public CDialog
{
	DECLARE_DYNAMIC(CMovieParams)

public:
	CMovieParams(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMovieParams();

// Dialog Data
	enum { IDD = IDD_MOVIE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_width;
	int m_height;
	int m_time;
	int m_quality;
};
