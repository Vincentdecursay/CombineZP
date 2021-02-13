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
// Info.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "Info.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfo dialog


CInfo::CInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInfo)
	m_n = 0;
	m_w = 0;
	m_h = 0;
	m_slots = 0;
	m_slotsize = 0;
	//}}AFX_DATA_INIT
}


void CInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfo)
	DDX_Text(pDX, IDC_EDIT1, m_n);
	DDX_Text(pDX, IDC_EDIT2, m_w);
	DDX_Text(pDX, IDC_EDIT4, m_h);
	DDX_Text(pDX, IDC_EDIT5, m_slots);
	DDX_Text(pDX, IDC_EDIT6, m_slotsize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfo, CDialog)
	//{{AFX_MSG_MAP(CInfo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfo message handlers
