#pragma once


// CNoise dialog

class CNoise : public CDialog
{
	DECLARE_DYNAMIC(CNoise)

public:
	CNoise(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNoise();

// Dialog Data
	enum { IDD = IDD_NOISE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_detection;
	int m_limit;
	int m_substitute;
};
