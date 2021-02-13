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
// combineDoc.cpp : implementation of the CCombineDoc class
//

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
#include "Getvalue.h"
#include "GotoFrame.h"

void CCombineDoc::OnStackHorizontalskew() 
{
	if(top_used_frame<2) return;
	BeginWaitCursor();
	mode=SKEW;
	if(GetValue())
		AfxBeginThread(skew_threaded,this);
}

UINT CCombineDoc::skew_threaded(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->skew_threaded();
	return 0;
}

void CCombineDoc::skew_threaded()
{
	buissy=true;

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	skew(skew_amount);
	progress.Minimize();
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	AfxGetMainWnd()->SetFocus();
	AfxGetMainWnd()->SetForegroundWindow();
	mode=0;
	EndWaitCursor();
	buissy=false;
	RefreshScreen();
}

void CCombineDoc::skew(int amount)
{
	if(amount==10000) return;

// displacemwnt of a frame relative to one below it
	float fd=(float)w*(float)(amount-10000)/(10000.0f*((float)top_used_frame-1.0f)),d;

	int f,x,y,Y,i,j,id;

	PIXEL *temp,*frame,p,p1,p2;
	float r,g,b;

	temp=(PIXEL*)GetBuffer(TEMP_BUFFER);

	progress.AddString(IDS_SKEW,amount);
	progress.SetRange(0,top_used_frame-1);

	for(f=1,d=fd; f<top_used_frame; f++,d=(float)f*fd)
	{
		if(progress.m_cancel) break;
		progress.SetPos(f);
		frame=(PIXEL*)GetBuffer(used_frame[f]);
		for(y=0; y<h; y++)
		{
			Y=y*w;
			for(x=0; x<w; x++)
			{
				i=x+Y;
				temp[i]=frame[i];
			}
		}

		if(d>=0.0f)
		{
			id=(int)d;
			d=d-(float)id;
			for(y=0; y<h; y++)
			{
				Y=y*w;
				for(x=id+1; x<w; x++)
				{
					i=x+Y;
					j=i-id;
					p1=temp[j];
					p2=temp[j-1];
					r=(1.0f-d)*(float)p1.r+d*(float)p2.r;
					g=(1.0f-d)*(float)p1.g+d*(float)p2.g;
					b=(1.0f-d)*(float)p1.b+d*(float)p2.b;
					p.r=(unsigned char)r;
					p.g=(unsigned char)g;
					p.b=(unsigned char)b;
					frame[i]=p;
				}
			}
		} else {
			d=-d;
			id=(int)d;
			d=d-(float)id;
			for(y=0; y<h; y++)
			{
				Y=y*w;
				for(x=0; x<w-id-1; x++)
				{
					i=x+Y;
					j=i+id;
					p1=temp[j];
					p2=temp[j+1];
					r=(1.0f-d)*(float)p1.r+d*(float)p2.r;
					g=(1.0f-d)*(float)p1.g+d*(float)p2.g;
					b=(1.0f-d)*(float)p1.b+d*(float)p2.b;
					p.r=(unsigned char)r;
					p.g=(unsigned char)g;
					p.b=(unsigned char)b;
					frame[i]=p;
				}
			}
		}
		UpdateBuffer(used_frame[f]);
	}
	progress.SetPos(0);
	FreeBuffer(TEMP_BUFFER);
}
