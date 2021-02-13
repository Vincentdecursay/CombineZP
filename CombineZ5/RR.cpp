/*
	CombineZ Image Stacking Program
	Copyright (C) 2004 Alan Hadley (alan_hadley@blueyonder.co.uk)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
// RR.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "RR.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRR dialog


CRR::CRR(CWnd* pParent /*=NULL*/)
	: CDialog(CRR::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRR)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRR::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRR)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRR, CDialog)
	//{{AFX_MSG_MAP(CRR)
	ON_BN_CLICKED(IDC_RR90, OnRr90)
	ON_BN_CLICKED(IDC_RR180, OnRr180)
	ON_BN_CLICKED(IDC_RR270, OnRr270)
	ON_BN_CLICKED(IDC_RR0, OnRr0)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRR message handlers

void CRR::OnRr90() 
{
	EndDialog(IDC_RR90);
}

void CRR::OnRr180() 
{
	EndDialog(IDC_RR180);
}

void CRR::OnRr270() 
{
	EndDialog(IDC_RR270);
}

void CRR::OnRr0() 
{
	EndDialog(IDC_RR0);
}
