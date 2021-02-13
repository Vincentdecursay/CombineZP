// ImportDM.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "ImportDM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportDM dialog


CImportDM::CImportDM(CWnd* pParent /*=NULL*/)
	: CDialog(CImportDM::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportDM)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CImportDM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportDM)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportDM, CDialog)
	//{{AFX_MSG_MAP(CImportDM)
	ON_BN_CLICKED(IDC_REDDATA, OnReddata)
	ON_BN_CLICKED(IDC_GREENDATA, OnGreendata)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportDM message handlers

void CImportDM::OnReddata() 
{
	EndDialog(IDC_REDDATA);
}

void CImportDM::OnGreendata() 
{
	EndDialog(IDC_GREENDATA);
}
