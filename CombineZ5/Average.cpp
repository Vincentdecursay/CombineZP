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
#include "Getvalue.h"
#include "progress.h"

// AVERAGE

void CCombineDoc::average()
{
	int c,i,z,m=last_used_frame-bottom_used_frame;
	if(m==1) return;

	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER),*f0;
	int* rt=(int*)GetBuffer(TEMP_BUFFER+0);
	int* gt=(int*)GetBuffer(TEMP_BUFFER+1);
	int* bt=(int*)GetBuffer(TEMP_BUFFER+2);

	BeginWaitCursor();

	for(i=0; i<w*h; i++)
	{
		out[i].abgr=wipeout;
		rt[i]=gt[i]=bt[i]=0;
	}

	for(z=bottom_used_frame; z<last_used_frame; z++)
	{
		f0=(PIXEL*)GetBuffer(used_frame[z]);
		for(i=0; i<w*h; i++)
		{
			rt[i]+=(int)f0[i].r;
			gt[i]+=(int)f0[i].g;
			bt[i]+=(int)f0[i].b;
		}
		FreeBuffer(used_frame[z]);
	}

	PIXEL p;
	for(i=0; i<w*h; i++)
	{
		p.a=aGOOD;
		c=rt[i]/m; if(c>255) c=255;	p.r=(unsigned char)c;
		c=gt[i]/m; if(c>255) c=255;	p.g=(unsigned char)c;
		c=bt[i]/m; if(c>255) c=255;	p.b=(unsigned char)c;
		out[i]=p;
	}

	EndWaitCursor();
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(TEMP_BUFFER+0);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+2);
}

// replace groups of frames with their average
void CCombineDoc::OnDecimate() 
{
	CDecimateDia dd;
	dd.m_n=top_used_frame;
	int	ff=0,lf=top_used_frame;
	int button;
	progress.Maximize();
	do{
		button=dd.DoModal();
		if(button==IDCANCEL) return;
	} while((dd.m_n<2) || (dd.m_n>100));
	group=dd.m_n; // ng number of frames in a group

	refreshscreen=true;
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	AfxBeginThread(decimate,this);
}

UINT CCombineDoc::decimate(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->decimate();
	return 0;
}

void CCombineDoc::decimate()
{
	buissy=true;

	CWnd* button=progress.GetDlgItem(IDC_CANCEL);
	button->EnableWindow(false);
	progress.AddString(IDS_GROUP,group);

	int i;

	CString name,s;

	for(i=0; i<4; i++) Sizer.p[i].valid=false;

	if(fix_active) flash_fixed();

	int	ng,ff=0,lf=top_used_frame,count,c;
	ng=group; // ng number of frames in a group

	count=(lf-ff)/ng; // ff/lf first/last frame

	int rem=(lf-ff)-count*ng;
	if(rem) count++; // if a remainder do one more time

	magon=false;

	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER),*f1;

	int replace_frame=0;

	last_used_frame=0;

	progress.SetRange(0,count-1);

	for(c=0; c<count; c++)
	{
		progress.SetPos(c);

		bottom_used_frame=last_used_frame;
		last_used_frame=bottom_used_frame+ng;

		name=" group_"+buffer[used_frame[bottom_used_frame]].name;
		s.Format("   Creating %s",name);
		progress.AddString(s);

		// adjust last group if there was a remainder
		if(last_used_frame>=lf)
		{
			last_used_frame=top_used_frame;
			ng=rem;
		}

		average();

		f1=(PIXEL*)ReuseBufferAs(used_frame[replace_frame],name);
		for(i=0; i<w*h; i++) f1[i]=out[i];
		UpdateBuffer(used_frame[replace_frame]);

		replace_frame++;
	}

	for(i=lf-1; i>=replace_frame; i--)
	{
		display=used_frame[i];
		remove();
	}
	save_state();

	refresh_used_list();

	FreeBuffer(OUT_BUFFER);

	progress.SetPos(0);
	button->EnableWindow(true);
	if(refreshscreen)
	{
		buissy=false;
		progress.Minimize();
		refreshscreen=false;
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
		RefreshScreen();
	}
}
