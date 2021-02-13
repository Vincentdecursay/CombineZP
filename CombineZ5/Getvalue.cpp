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
// Getvalue.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "Getvalue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetvalue dialog


CGetvalue::CGetvalue(CWnd* pParent /*=NULL*/)
	: CDialog(CGetvalue::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetvalue)
	m_value = 0;
	m_string = _T("");
	//}}AFX_DATA_INIT
}


void CGetvalue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetvalue)
	DDX_Text(pDX, IDC_VALUE, m_value);
	DDV_MinMaxInt(pDX, m_value, 0, 999999);
	DDX_Text(pDX, IDC_STRING, m_string);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetvalue, CDialog)
	//{{AFX_MSG_MAP(CGetvalue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetvalue message handlers

BOOL CGetvalue::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
//	CString s;
//	GetDlgItem(IDC_VALUE)->SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
//	return false;
}
