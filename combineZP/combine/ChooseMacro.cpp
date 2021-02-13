// ChooseMacro1.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "combinedoc.h"
#include "ChooseMacro.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseMacro dialog


CChooseMacro::CChooseMacro(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseMacro::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseMacro)
	//}}AFX_DATA_INIT
}


void CChooseMacro::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	SetDlgItemText(IDC_MACRO1,macro[0].name);
	SetDlgItemText(IDC_MACRO2,macro[1].name);
	SetDlgItemText(IDC_MACRO3,macro[2].name);
	SetDlgItemText(IDC_MACRO4,macro[3].name);
	SetDlgItemText(IDC_MACRO5,macro[4].name);
	SetDlgItemText(IDC_MACRO6,macro[5].name);
	SetDlgItemText(IDC_MACRO7,macro[6].name);
	SetDlgItemText(IDC_MACRO8,macro[7].name);
	SetDlgItemText(IDC_MACRO9,macro[8].name);
	SetDlgItemText(IDC_MACRO10,macro[9].name);
	//{{AFX_DATA_MAP(CChooseMacro)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseMacro, CDialog)
	//{{AFX_MSG_MAP(CChooseMacro)
	ON_BN_CLICKED(IDC_MACRO1, OnMacro1)
	ON_BN_CLICKED(IDC_MACRO2, OnMacro2)
	ON_BN_CLICKED(IDC_MACRO3, OnMacro3)
	ON_BN_CLICKED(IDC_MACRO4, OnMacro4)
	ON_BN_CLICKED(IDC_MACRO5, OnMacro5)
	ON_BN_CLICKED(IDC_MACRO6, OnMacro6)
	ON_BN_CLICKED(IDC_MACRO7, OnMacro7)
	ON_BN_CLICKED(IDC_MACRO8, OnMacro8)
	ON_BN_CLICKED(IDC_MACRO9, OnMacro9)
	ON_BN_CLICKED(IDC_MACRO10, OnMacro10)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseMacro message handlers

void CChooseMacro::OnMacro1() 
{
	EndDialog(100);	
}

void CChooseMacro::OnMacro2() 
{
	EndDialog(101);	
}

void CChooseMacro::OnMacro3() 
{
	EndDialog(102);	
}

void CChooseMacro::OnMacro4() 
{
	EndDialog(103);	
}

void CChooseMacro::OnMacro5() 
{
	EndDialog(104);	
}

void CChooseMacro::OnMacro6() 
{
	EndDialog(105);	
}

void CChooseMacro::OnMacro7() 
{
	EndDialog(106);	
}

void CChooseMacro::OnMacro8() 
{
	EndDialog(107);	
}

void CChooseMacro::OnMacro9() 
{
	EndDialog(108);	
}

void CChooseMacro::OnMacro10() 
{
	EndDialog(109);	
}
