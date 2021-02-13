#pragma once


// CTakePicture dialog

class CTakePicture : public CDialog
{
	DECLARE_DYNAMIC(CTakePicture)

public:
	CTakePicture(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTakePicture();

	RECT pos;

// Dialog Data
	enum { IDD = IDD_TAKE_PICTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTakePicture();
	afx_msg void OnBnClickedEndVideo();
	afx_msg void OnPaint();
};
