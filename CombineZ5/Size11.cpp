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
#include "combine.h"
#include "combineDoc.h"
#include "Size11.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Size1::Size1(CWnd* pParent /*=NULL*/)
	: CDialog(Size1::IDD, pParent)
{
	//{{AFX_DATA_INIT(Size1)
	m_f1p1 = _T("");
	m_f1p2 = _T("");
	m_f1p3 = _T("");
	m_f1p4 = _T("");
	m_ratio = _T("");
	m_length12 = _T("");
	m_length34 = _T("");
	m_angle = _T("");
	//}}AFX_DATA_INIT
	p[0].valid=false;
	p[1].valid=false;
	p[2].valid=false;
	p[3].valid=false;
}


void Size1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Size1)
	DDX_Text(pDX, IDC_F1P1, m_f1p1);
	DDX_Text(pDX, IDC_F1P2, m_f1p2);
	DDX_Text(pDX, IDC_F1P3, m_f1p3);
	DDX_Text(pDX, IDC_F1P4, m_f1p4);
	DDX_Text(pDX, IDC_RATIO, m_ratio);
	DDX_Text(pDX, IDC_LENGTH12, m_length12);
	DDX_Text(pDX, IDC_LENGTH34, m_length34);
	DDX_Text(pDX, IDC_ANG, m_angle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Size1, CDialog)
	//{{AFX_MSG_MAP(Size1)
	ON_BN_CLICKED(IDC_SETPARAMS, OnSetparams)
	ON_BN_CLICKED(IDC_SIZEREG, OnSizereg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void Size1::Calculate()
{
	if(p[0].valid)
		m_f1p1.Format("Point 1 (%d,%d) Frame %s",p[0].x,p[0].y,p[0].title);
	else m_f1p1="Point 1 Not Initialized";

	if(p[1].valid)
		m_f1p2.Format("Point 2 (%d,%d) Frame %s",p[1].x,p[1].y,p[1].title);
	else m_f1p2="Point 2 Not Initialized";

	if(p[0].valid && p[1].valid)
	{
		length12=sqrt(
				(double)((p[0].x-p[1].x)*(p[0].x-p[1].x)+
						(p[0].y-p[1].y)*(p[0].y-p[1].y)));
		m_length12.Format("Length of p1p2=%d",(int)length12);
	}
	else m_length12="Length unknown";

	if(p[2].valid)
		m_f1p3.Format("Point 3 (%d,%d) Frame %s",p[2].x,p[2].y,p[2].title);
	else m_f1p3="Point 3 Not Initialized";

	if(p[3].valid)
		m_f1p4.Format("Point 4 (%d,%d) Frame %s",p[3].x,p[3].y,p[3].title);
	else m_f1p4="Point 4 Not Initialized";

	if(p[3].valid && p[4].valid)
	{
		length34=sqrt(
				(double)((p[2].x-p[3].x)*(p[2].x-p[3].x)+
						(p[2].y-p[3].y)*(p[2].y-p[3].y)));
		m_length34.Format("Length of p3p4=%d",(int)length34);
	}
	else m_length34="Length unknown";

	if(p[0].valid && p[1].valid && p[2].valid && p[3].valid)
	{
		ratio=length34/length12;
		m_ratio.Format("p1p2 : p3p4  ::  1.0000 : %0.4g",ratio);

		double m12=(double)(p[1].y-p[0].y)/(double)(p[1].x-p[0].x);
		double m34=(double)(p[3].y-p[2].y)/(double)(p[3].x-p[2].x);
		double tana=(m12-m34)/(1.0f+m12*m34);
		angle=(int)(36000.0f*atan(tana)/(3.142f*2.0f));
		if(angle<0) angle+=36000;
		if(angle==0 && (tana>1 || tana<-1)) angle=9000;
		angle=36000-angle;
		m_angle.Format("Angle=%g degrees",((double)angle)/100.0f);
	}
	else
	{
		m_ratio="No Ratio available";
		m_angle="No Angle available";
	}
}

void Size1::OnSetparams() 
{
	UpdateData();
	EndDialog(SAVE);
}

void Size1::OnSizereg() 
{
	UpdateData();
	EndDialog(REG);
}
