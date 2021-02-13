// MaxAngle.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "MaxAngle.h"


// CMaxAngle dialog

IMPLEMENT_DYNAMIC(CMaxAngle, CDialog)
CMaxAngle::CMaxAngle(CWnd* pParent /*=NULL*/)
	: CDialog(CMaxAngle::IDD, pParent)
	, m_maxangle(0)
{
}

CMaxAngle::~CMaxAngle()
{
}

void CMaxAngle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MAXANGLE, m_maxangle);
	DDV_MinMaxInt(pDX, m_maxangle, 0, 4500);
}


BEGIN_MESSAGE_MAP(CMaxAngle, CDialog)
END_MESSAGE_MAP()


// CMaxAngle message handlers
