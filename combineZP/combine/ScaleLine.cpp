// ScaleLine.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "ScaleLine.h"
#include ".\scaleline.h"
#include "math.h"


// CScaleLine dialog

IMPLEMENT_DYNAMIC(CScaleLine, CDialog)
CScaleLine::CScaleLine(CWnd* pParent /*=NULL*/)
	: CDialog(CScaleLine::IDD, pParent)
	, m_slaspx(1)
	, m_slaspy(1)
	, m_slcallen(1000)
	, m_slcalpixels(100)
	, m_slpips(10)
	, m_sloutlen(1000)
{
}

CScaleLine::~CScaleLine()
{
}

void CScaleLine::DoDataExchange(CDataExchange* pDX)
{
	int r;

	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLPOSITION, m_slposition);
	DDX_Control(pDX, IDC_SLCALIBSRC, m_slcalsrc);
	DDX_Control(pDX, IDC_SLCALIUNITS, m_slcalunits);
	DDX_Control(pDX, IDC_SLCOLOURS, m_slcolours);
	DDX_Control(pDX, IDC_SLORIENT, m_slorient);
	DDX_Control(pDX, IDC_SLOUTUNITS, m_sloutunits);
	m_slaspx=AfxGetApp()->GetProfileInt(L"ScaleLine",L"AspX",1);
	DDX_Text(pDX, IDC_SLCALPIXW, m_slaspx);
	m_slaspy=AfxGetApp()->GetProfileInt(L"ScaleLine",L"AspY",1);
	DDX_Text(pDX, IDC_SLCALPIXH, m_slaspy);
	m_slcallen=AfxGetApp()->GetProfileInt(L"ScaleLine",L"CalLen",1);
	DDX_Text(pDX, IDC_SLCALILENGTH, m_slcallen);
	m_slcalpixels=AfxGetApp()->GetProfileInt(L"ScaleLine",L"CalPixels",1);
	DDX_Text(pDX, IDC_SLCALPIXLEN, m_slcalpixels);
	if(!pDX->m_bSaveAndValidate) SetPixels(AfxGetApp()->GetProfileInt(L"ScaleLine",L"CalSrc",3));
	m_slpips=AfxGetApp()->GetProfileInt(L"ScaleLine",L"Pips",1);
	DDX_Text(pDX, IDC_SLPIPS, m_slpips);
	m_sloutlen=AfxGetApp()->GetProfileInt(L"ScaleLine",L"OutLen",1);
	DDX_Text(pDX, IDC_SLOUTLEN, m_sloutlen);

	if(!pDX->m_bSaveAndValidate)
	{
		m_slcalsrc.AddString(L"Points 1 and 2 Horiz.");
		m_slcalsrc.AddString(L"Points 1 and 2 Vert.");
		m_slcalsrc.AddString(L"Points 1 and 2");
		r=m_slcalsrc.AddString(L"User");
		r=AfxGetApp()->GetProfileInt(L"ScaleLine",L"CalSrc",r);
		m_slcalsrc.SetCurSel(r);

		m_slcalunits.AddString(L"nm");
		r=m_slcalunits.AddString(L"µm");
		m_slcalunits.AddString(L"mm");
		m_slcalunits.AddString(L"cm");
		m_slcalunits.AddString(L"m");
		r=AfxGetApp()->GetProfileInt(L"ScaleLine",L"CalUnit",r);
		m_slcalunits.SetCurSel(r);

		m_sloutunits.AddString(L"nm");
		r=m_sloutunits.AddString(L"µm");
		m_sloutunits.AddString(L"mm");
		m_sloutunits.AddString(L"cm");
		m_sloutunits.AddString(L"m");
		r=AfxGetApp()->GetProfileInt(L"ScaleLine",L"OutUnit",r);
		m_sloutunits.SetCurSel(r);

		r=m_slcolours.AddString(L"Black on White");
		m_slcolours.AddString(L"White on Grey");
		m_slcolours.AddString(L"Black+Shadow");
		m_slcolours.AddString(L"White+Shadow");
		r=AfxGetApp()->GetProfileInt(L"ScaleLine",L"Colours",r);
		m_slcolours.SetCurSel(r);

		r=m_slorient.AddString(L"Horizontal");
		m_slorient.AddString(L"Vertical");
		r=AfxGetApp()->GetProfileInt(L"ScaleLine",L"Orientation",r);
		m_slorient.SetCurSel(r);

		r=m_slposition.AddString(L"Top-Left");
		m_slposition.AddString(L"Top-Middle");
		m_slposition.AddString(L"Top-Right");
		m_slposition.AddString(L"Middle-Left");
		m_slposition.AddString(L"Middle-right");
		m_slposition.AddString(L"Bottom-Left");
		m_slposition.AddString(L"Bottom-Middle");
		m_slposition.AddString(L"Bottom-Right");
		r=AfxGetApp()->GetProfileInt(L"ScaleLine",L"Position",r);
		m_slposition.SetCurSel(r);
	} else {
		m_colours=m_slcolours.GetCurSel();
		AfxGetApp()->WriteProfileInt(L"ScaleLine",L"Colours",m_colours);
		m_outunits=m_sloutunits.GetCurSel();
		AfxGetApp()->WriteProfileInt(L"ScaleLine",L"OutUnit",m_outunits);
		r=m_slcalunits.GetCurSel();
		AfxGetApp()->WriteProfileInt(L"ScaleLine",L"CalUnit",r);
		m_orient=m_slorient.GetCurSel();
		AfxGetApp()->WriteProfileInt(L"ScaleLine",L"Orientation",m_orient);
		m_position=m_slposition.GetCurSel();
		AfxGetApp()->WriteProfileInt(L"ScaleLine",L"Position",m_position);
		switch(r)
		{
		case 0: m_inlen=m_slcallen; break;
		case 1: m_inlen=m_slcallen*1000; break;
		case 2: m_inlen=m_slcallen*1000000; break;
		case 3: m_inlen=m_slcallen*10000000; break;
		case 4: m_inlen=m_slcallen*1000000000; break;
		}
		AfxGetApp()->WriteProfileInt(L"ScaleLine",L"CalSrc",m_slcalsrc.GetCurSel());
		AfxGetApp()->WriteProfileInt(L"ScaleLine",L"AspX",m_slaspx);
		AfxGetApp()->WriteProfileInt(L"ScaleLine",L"AspY",m_slaspy);
		AfxGetApp()->WriteProfileInt(L"ScaleLine",L"CalLen",m_slcallen);
		AfxGetApp()->WriteProfileInt(L"ScaleLine",L"CalPixels",m_slcalpixels);
		AfxGetApp()->WriteProfileInt(L"ScaleLine",L"Pips",m_slpips);
		AfxGetApp()->WriteProfileInt(L"ScaleLine",L"OutLen",m_sloutlen);
	}
	DDX_Control(pDX, IDC_SLPOSITION, m_slposition);
}

BEGIN_MESSAGE_MAP(CScaleLine, CDialog)
	ON_CBN_SELCHANGE(IDC_SLCALIBSRC, OnCbnSelchangeSlcalibsrc)
	ON_EN_CHANGE(IDC_SLCALPIXW, OnEnChangeSlcalpixw)
	ON_EN_CHANGE(IDC_SLCALPIXH, OnEnChangeSlcalpixh)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CScaleLine message handlers


bool CScaleLine::SetPixels(int mode)
{
	if(mode==3)
	{
		GetDlgItem(IDC_SLCALPIXLEN)->EnableWindow(true);
		return true;
	}
	else GetDlgItem(IDC_SLCALPIXLEN)->EnableWindow(false);

	if((m_pixelsx==0) && (m_pixelsy==0))
	{
		MessageBox(L"To use Points for Calibration, Cancel the next dialog, Set Points 1 and 2, then try again.   Otherwise Set User as the Calibration Data Source",L"Scale Line",MB_OK);
		SetDlgItemInt(IDC_SLCALPIXLEN,0,true);
		return false;
	}

	switch(mode)
	{
	case 0:
		m_slcalpixels=m_pixelsx;
		break;
	case 1:
		if(m_slaspy) m_slcalpixels=(m_pixelsy*m_slaspx)/m_slaspy;
		break;
	case 2:
		if(m_slaspy) m_slcalpixels=(int)(sqrt((double)(m_pixelsy*m_pixelsy*m_slaspx*m_slaspx)/(double)(m_slaspy*m_slaspy)+(double)(m_pixelsx*m_pixelsx))+0.5);
		break;
	}
	SetDlgItemInt(IDC_SLCALPIXLEN,m_slcalpixels,true);
	return true;
}

void CScaleLine::OnCbnSelchangeSlcalibsrc()
{
	int r;
	r=m_slcalsrc.GetCurSel();
	SetPixels(r);
}

void CScaleLine::OnEnChangeSlcalpixw()
{
	m_slaspx=GetDlgItemInt(IDC_SLCALPIXW);

	int r;
	r=m_slcalsrc.GetCurSel();
	SetPixels(r);
}

void CScaleLine::OnEnChangeSlcalpixh()
{
	m_slaspy=GetDlgItemInt(IDC_SLCALPIXH);

	int r;
	r=m_slcalsrc.GetCurSel();
	SetPixels(r);
}

void CScaleLine::OnBnClickedOk()
{
	if(GetDlgItemInt(IDC_SLCALPIXLEN)==0){OnCancel(); return;}
	OnOK();
}
