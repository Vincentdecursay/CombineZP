#pragma once
#include "afxwin.h"


// CScaleLine dialog

class CScaleLine : public CDialog
{
	DECLARE_DYNAMIC(CScaleLine)

public:
	CScaleLine(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScaleLine();

// Dialog Data
	enum { IDD = IDD_SCALELINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_slcalsrc;
	CComboBox m_slcalunits;
	CComboBox m_slcolours;
	int m_colours;
	CComboBox m_slorient;
	int m_orient;
	CComboBox m_sloutunits;
	int m_outunits;
	int m_slaspx;
	int m_slaspy;
	int m_slcallen;
	int m_inlen;
	int m_slcalpixels;
	int m_slpips;
	int m_sloutlen;
	int m_pixelsx;
	int m_pixelsy;
	bool SetPixels(int mode);
	afx_msg void OnCbnSelchangeSlcalibsrc();
	afx_msg void OnEnChangeSlcalpixw();
	afx_msg void OnEnChangeSlcalpixh();
	afx_msg void OnBnClickedOk();
	CComboBox m_slposition;
	int m_position;
};
