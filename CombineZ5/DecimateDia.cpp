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
// DecimateDia.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "DecimateDia.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDecimateDia dialog


CDecimateDia::CDecimateDia(CWnd* pParent /*=NULL*/)
	: CDialog(CDecimateDia::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDecimateDia)
	m_n = 0;
	//}}AFX_DATA_INIT
}


void CDecimateDia::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDecimateDia)
	DDX_Text(pDX, IDC_DECIMATEN, m_n);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDecimateDia, CDialog)
	//{{AFX_MSG_MAP(CDecimateDia)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDecimateDia message handlers

