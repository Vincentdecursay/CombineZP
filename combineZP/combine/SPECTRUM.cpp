// SPECTRUM.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "SPECTRUM.h"


// CSPECTRUM dialog

IMPLEMENT_DYNAMIC(CSPECTRUM, CDialog)
CSPECTRUM::CSPECTRUM(CWnd* pParent /*=NULL*/)
	: CDialog(CSPECTRUM::IDD, pParent)
	, m_redend(0)
	, m_blueend(0)
	, m_break(0)
{
}

CSPECTRUM::~CSPECTRUM()
{
}

void CSPECTRUM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_REDEND, m_redend);
	DDV_MinMaxInt(pDX, m_redend, 0, 1529);
	DDX_Text(pDX, IDC_BLUEEND, m_blueend);
	DDV_MinMaxInt(pDX, m_blueend, 0, 1529);
	DDX_Text(pDX, IDC_BREAK, m_break);
	DDV_MinMaxInt(pDX, m_break, 0, 1529);
}


BEGIN_MESSAGE_MAP(CSPECTRUM, CDialog)
END_MESSAGE_MAP()


// CSPECTRUM message handlers
