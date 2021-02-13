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

#include "stdafx.h"
#include "gdiplus.h"
#include "direct.h"
#include "math.h"

#include "combine.h"
#include "combineDoc.h"
#include "Size1.h"
#include "DecimateDia.h"
#include "SizeDia.h"
#include "RegisterDia.h"
#include "PaintDia.h"
#include "Info.h"
#include "rr.h"
#include "Dft.h"
#include "macro1.h"
#include "choosemacro.h"
#include "values.h"
#include ".\values.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CValues dialog


CValues::CValues(CWnd* pParent /*=NULL*/)
	: CDialog(CValues::IDD, pParent)
{
	//{{AFX_DATA_INIT(CValues)
	m_command = _T("");
	m_p1 = 0;
	m_p2 = 0;
	m_p3 = 0;
	//}}AFX_DATA_INIT
}


void CValues::DoDataExchange(CDataExchange* pDX)
{
	if(pDX->m_bSaveAndValidate==0)
	{
		RECT r;
		GetWindowRect(&r);

		pos.left=AfxGetApp()->GetProfileInt(L"DialogData",L"GetValuesLeft",r.left);
		pos.top=AfxGetApp()->GetProfileInt(L"DialogData",L"GetValuesTop",r.top);
		pos.right=AfxGetApp()->GetProfileInt(L"DialogData",L"GetValuesRight",r.right);
		pos.bottom=AfxGetApp()->GetProfileInt(L"DialogData",L"GetValuesBottom",r.bottom);

		m_cancel=false;
		m_end=false;

		if(macro[this_macro].cmd[command].p1!=-1)
		{
			if(macro[this_macro].cmd[command].p3!=-1)
			{
				GetDlgItem(IDC_CP1)->EnableWindow(true);
				GetDlgItem(IDC_CP2)->EnableWindow(true);
				GetDlgItem(IDC_CP3)->EnableWindow(true);
			} else {
				if(macro[this_macro].cmd[command].p2!=-1)
				{
					GetDlgItem(IDC_CP1)->EnableWindow(true);
					GetDlgItem(IDC_CP2)->EnableWindow(true);
					GetDlgItem(IDC_CP3)->EnableWindow(false);
				} else {
					GetDlgItem(IDC_CP1)->EnableWindow(true);
					GetDlgItem(IDC_CP2)->EnableWindow(false);
					GetDlgItem(IDC_CP3)->EnableWindow(false);
				}
			}
		} else {
				GetDlgItem(IDC_CP1)->EnableWindow(false);
				GetDlgItem(IDC_CP2)->EnableWindow(false);
				GetDlgItem(IDC_CP3)->EnableWindow(false);
		}
		if(m_p1<-1) m_p1++;
		if(m_p2<-1) m_p2++;
		if(m_p3<-1) m_p3++;
	} 

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CValues)
	DDX_Text(pDX, IDC_COMMAND, m_command);
	DDX_Text(pDX, IDC_CP1, m_p1);
	DDX_Text(pDX, IDC_CP2, m_p2);
	DDX_Text(pDX, IDC_CP3, m_p3);
	//}}AFX_DATA_MAP

	if(pDX->m_bSaveAndValidate)
	{
		if(m_p1<0) m_p1--;
		if(m_p2<0) m_p2--;
		if(m_p3<0) m_p3--;

		GetWindowRect(&pos);
		AfxGetApp()->WriteProfileInt(L"DialogData",L"GetValuesLeft",pos.left);
		AfxGetApp()->WriteProfileInt(L"DialogData",L"GetValuesTop",pos.top);
		AfxGetApp()->WriteProfileInt(L"DialogData",L"GetValuesRight",pos.right);
		AfxGetApp()->WriteProfileInt(L"DialogData",L"GetValuesBottom",pos.bottom);
	}
}


BEGIN_MESSAGE_MAP(CValues, CDialog)
	//{{AFX_MSG_MAP(CValues)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_END, OnEnd)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CValues message handlers

void CValues::OnCancel() 
{
	GetWindowRect(&pos);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"GetValuesLeft",pos.left);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"GetValuesTop",pos.top);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"GetValuesRight",pos.right);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"GetValuesBottom",pos.bottom);

	m_cancel=true;
	EndDialog(IDC_CANCEL);
}

void CValues::OnEnd() 
{
	GetWindowRect(&pos);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"GetValuesLeft",pos.left);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"GetValuesTop",pos.top);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"GetValuesRight",pos.right);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"GetValuesBottom",pos.bottom);

	m_end=true;
	EndDialog(IDC_END);
}

void CValues::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	MoveWindow(&pos,true);
}
