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
// RegisterDia.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "RegisterDia.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegisterDia dialog


CRegisterDia::CRegisterDia(CWnd* pParent /*=NULL*/)
	: CDialog(CRegisterDia::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegisterDia)
	m_a = 0;
	m_f = 0;
	m_x = 0;
	m_y = 0;
	m_oy = 0;
	m_ox = 0;
	//}}AFX_DATA_INIT
}


void CRegisterDia::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterDia)
	DDX_Text(pDX, IDC_SA, m_a);
	DDX_Text(pDX, IDC_SF, m_f);
	DDX_Text(pDX, IDC_SX, m_x);
	DDX_Text(pDX, IDC_SY, m_y);
	DDX_Text(pDX, IDC_SOY, m_oy);
	DDV_MinMaxInt(pDX, m_oy, -10000, 10000);
	DDX_Text(pDX, IDC_SOX, m_ox);
	DDV_MinMaxInt(pDX, m_ox, -10000, 10000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegisterDia, CDialog)
	//{{AFX_MSG_MAP(CRegisterDia)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_EN_UPDATE(IDC_SOX, OnUpdateSox)
	ON_EN_CHANGE(IDC_SOX, OnChangeSox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegisterDia message handlers

void CRegisterDia::OnReset() 
{
	SetDlgItemInt(IDC_SF,10000,0);
	SetDlgItemInt(IDC_SOX,0,0);
	SetDlgItemInt(IDC_SOY,0,0);
	SetDlgItemInt(IDC_SA,0,0);
}

void CRegisterDia::OnUpdateSox() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	
}

void CRegisterDia::OnChangeSox() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
