#pragma once


// CSPECTRUM dialog

class CSPECTRUM : public CDialog
{
	DECLARE_DYNAMIC(CSPECTRUM)

public:
	CSPECTRUM(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSPECTRUM();

// Dialog Data
	enum { IDD = IDD_SPECTRUM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_redend;
	int m_blueend;
	int m_break;
};
