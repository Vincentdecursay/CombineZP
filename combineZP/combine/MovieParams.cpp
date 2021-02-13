// MovieParams.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "MovieParams.h"


// CMovieParams dialog

IMPLEMENT_DYNAMIC(CMovieParams, CDialog)
CMovieParams::CMovieParams(CWnd* pParent /*=NULL*/)
	: CDialog(CMovieParams::IDD, pParent)
	, m_width(0)
	, m_height(0)
	, m_time(0)
	, m_quality(0)
{
}

CMovieParams::~CMovieParams()
{
}

void CMovieParams::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_WIDTH, m_width);
	DDV_MinMaxInt(pDX, m_width, 4, 100000);
	DDX_Text(pDX, IDC_HEIGHT, m_height);
	DDV_MinMaxInt(pDX, m_height, 4, 100000);
	DDX_Text(pDX, IDC_TIME, m_time);
	DDV_MinMaxInt(pDX, m_time, 10, 100000);
	DDX_Text(pDX, IDC_QUALITY, m_quality);
	DDV_MinMaxInt(pDX, m_quality, 0, 100);
}


BEGIN_MESSAGE_MAP(CMovieParams, CDialog)
END_MESSAGE_MAP()


// CMovieParams message handlers
