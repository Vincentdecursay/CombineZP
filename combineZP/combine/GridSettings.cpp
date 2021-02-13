// GridSettings.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "GridSettings.h"


// CGridSettings dialog

IMPLEMENT_DYNAMIC(CGridSettings, CDialog)
CGridSettings::CGridSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CGridSettings::IDD, pParent)
	, m_gridx(8)
	, m_gridy(6)
{
}

CGridSettings::~CGridSettings()
{
}

void CGridSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_GRIDX,m_gridx);
	DDX_Text(pDX,IDC_GRIDY,m_gridy);
}


BEGIN_MESSAGE_MAP(CGridSettings, CDialog)
END_MESSAGE_MAP()


// CGridSettings message handlers
