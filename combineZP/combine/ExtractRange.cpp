// ExtractRange.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "ExtractRange.h"


// CExtractRange dialog

IMPLEMENT_DYNAMIC(CExtractRange, CDialog)
CExtractRange::CExtractRange(CWnd* pParent /*=NULL*/)
	: CDialog(CExtractRange::IDD, pParent)
	, m_redend(0)
	, m_blueend(0)
	, m_th(0)
{
}

CExtractRange::~CExtractRange()
{
}

void CExtractRange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_REDEND1, m_redend);
	DDV_MinMaxInt(pDX, m_redend, 0, 1529);
	DDX_Text(pDX, IDC_BLUEEND1, m_blueend);
	DDV_MinMaxInt(pDX, m_blueend, 0, 1529);
	DDX_Text(pDX, IDC_BREAKPOINT1, m_th);
	DDV_MinMaxInt(pDX, m_th, -1529, 1529);
}


BEGIN_MESSAGE_MAP(CExtractRange, CDialog)
END_MESSAGE_MAP()


// CExtractRange message handlers
