#pragma once


// CMaxAngle dialog

class CMaxAngle : public CDialog
{
	DECLARE_DYNAMIC(CMaxAngle)

public:
	CMaxAngle(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMaxAngle();

// Dialog Data
	enum { IDD = IDD_MAXANGLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_maxangle;
};
