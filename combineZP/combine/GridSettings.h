#pragma once


// CGridSettings dialog

class CGridSettings : public CDialog
{
	DECLARE_DYNAMIC(CGridSettings)

public:
	CGridSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGridSettings();

// Dialog Data
	enum { IDD = IDD_GRIDSETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_gridx;
	int m_gridy;
};
