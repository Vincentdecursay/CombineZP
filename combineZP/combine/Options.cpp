// Options.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "Options.h"
#include ".\options.h"


// COptions dialog

IMPLEMENT_DYNAMIC(COptions, CDialog)
COptions::COptions(CWnd* pParent /*=NULL*/)
	: CDialog(COptions::IDD, pParent)
	, m_reserve_ram(0)
	, m_startup_help(true)
	, m_startup_simple(true)
	, m_save_header(true)
{
}

COptions::~COptions()
{
}

void COptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_RESERVE_RAM,m_reserve_ram);
	DDX_Check(pDX,IDC_STARTUP_HELP,m_startup_help);
	DDX_Check(pDX,IDC_STARTUP_SIMPLE,m_startup_simple);
	DDX_Check(pDX,IDC_HEADER,m_save_header);
	DDX_Text(pDX,IDC_CORES,m_cores);
	DDX_Check(pDX,IDC_MINIPROGRESS,m_miniprogress);
}


BEGIN_MESSAGE_MAP(COptions, CDialog)
END_MESSAGE_MAP()


// COptions message handlers
