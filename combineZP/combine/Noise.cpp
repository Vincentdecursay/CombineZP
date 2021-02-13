// Noise.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "Noise.h"


// CNoise dialog

IMPLEMENT_DYNAMIC(CNoise, CDialog)
CNoise::CNoise(CWnd* pParent /*=NULL*/)
	: CDialog(CNoise::IDD, pParent)
	, m_detection(0)
	, m_limit(0)
	, m_substitute(0)
{
}

CNoise::~CNoise()
{
}

void CNoise::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_DETECTION,m_detection);
	DDV_MinMaxInt(pDX, m_detection, 0, 1000);
	DDX_Text(pDX, IDC_DETECT_LIMIT, m_limit);
	DDV_MinMaxInt(pDX, m_limit, 0, 255);
	DDX_Text(pDX, IDC_SUBSTITUTE, m_substitute);
	DDV_MinMaxInt(pDX, m_substitute, 0, 1000);
}


BEGIN_MESSAGE_MAP(CNoise, CDialog)
END_MESSAGE_MAP()


// CNoise message handlers
