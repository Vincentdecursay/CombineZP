#pragma once


// CExtractRange dialog

class CExtractRange : public CDialog
{
	DECLARE_DYNAMIC(CExtractRange)

public:
	CExtractRange(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExtractRange();

// Dialog Data
	enum { IDD = IDD_EXTRACTRANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_redend;
	int m_blueend;
	int m_th;
};
