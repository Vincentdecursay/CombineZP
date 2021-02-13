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

void CCombineDoc::replace()
{
	OnFrameReplace();
}

void CCombineDoc::Replace()
{
	int i;

	if(active_frame==display) return;

	str1.LoadString(IDS_STRING433);
	progress.AddString(str1+buffer[active_frame].name);
	str1.LoadString(IDS_STRING434);
	progress.AddString(str1+buffer[display].name);

	PIXEL* f1=(PIXEL*)GetBuffer(active_frame);
	PIXEL* f0=(PIXEL*)GetBuffer(display);
	for(i=0; i<w*h; i++)
	{
		if(f0[i].abgr!=wipeout)
		{
			f1[i]=f0[i];
			f1[i].a=aGOOD;
		}
		else
		{
			f1[i].abgr=0;
			f1[i].a=aCLEAR;
		}
	}
	FreeBuffer(display);
	UpdateBuffer(active_frame);

	display=active_frame;
}

void CCombineDoc::OnFrameReplace()
{
	if(display==active_frame) return;

	Replace();
	
	RefreshScreen();
}

void CCombineDoc::OnFrameNewfrompicture() 
{
	if(top_frame==MAX_FRAMES)
	{
		CString s1,s2;
		s1.LoadString(IDS_FILELIMITREACHED); s2.LoadString(IDS_FRAMELIMIT);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		return;
	}

	newfrompicture();

	RefreshScreen();
}

void CCombineDoc::newfrompicture()
{
	GetBuffer(top_frame);
	str1.LoadString(IDS_STRING435);
	buffer[top_frame].name=newnumber(str1)+"-"+buffer[display].name;
	buffer[top_frame].hidden=true;
	top_frame++;
	active_frame=top_frame-1;
	Replace();
	display=top_frame-1;
	UpdateBuffer(top_frame-1);
	refresh_used_list();
}

void CCombineDoc::OnFrameNewfromvisible()
{
	if(top_frame==MAX_FRAMES)
	{
		CString s1,s2;
		s1.LoadString(IDS_FILELIMITREACHED); s2.LoadString(IDS_FRAMELIMIT);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		return;
	}

	newfromvisible();

	RefreshScreen();
}

void CCombineDoc::newfromvisible()
{
	GetBuffer(top_frame);
	str1.LoadString(IDS_STRING435);
	buffer[top_frame].name=newnumber(str1)+"-"+buffer[display].name;
	buffer[top_frame].hidden=false;
	top_frame++;
	active_frame=top_frame-1;
	Replace();
	display=top_frame-1;
	UpdateBuffer(top_frame-1);
	refresh_used_list();
}

void CCombineDoc::clearout()
{
	int i;
	int *out=(int*)GetBuffer(OUT_BUFFER);
	for(i=0; i<(int)(w*h); i++) out[i]=0x000055;
	UpdateBuffer(OUT_BUFFER);
}

void CCombineDoc::OnFrameInvert()
{
	if(!top_frame) return;

	int x,startx,endx;
	int y,starty,endy,Y;

	int i;

	progress.AddString(IDS_INVERT);

	BeginWaitCursor();

	startx=0;
	starty=0;
	endx=w;
	endy=h;

	PIXEL *f0=(PIXEL*)GetBuffer(display);
	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER);

	PIXEL p;
	unsigned int r,g,b;

	for(y=0; y<(int)h; y++)
	{
		Y=y*(int)w;
		for(x=0; x<(int)w; x++)
		{
			i=x+Y;
			p=f0[i];
			r=p.r; g=p.g; b=p.b;

			out[i].r=(unsigned char)(255-r); 
			out[i].g=(unsigned char)(255-g); 
			out[i].b=(unsigned char)(255-b); 
		}
	}
	FreeBuffer(display);
	UpdateBuffer(OUT_BUFFER);

	display=OUT_BUFFER;
	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::OnFrameMakebg() 
{
	if(!top_frame) return;

	int x,startx,endx;
	int y,starty,endy,Y;
	int count,r,g,b,rsum,gsum,bsum,buf;

	int i;

	progress.AddString(IDS_STRING436);

	if(top_frame-1==MAX_FRAMES)
	{
		CString s1,s2;
		s1.LoadString(IDS_FILELIMITREACHED); s2.LoadString(IDS_FRAMELIMIT);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		return;
	}

	BeginWaitCursor();

	if(magon)
	{
		startx=mag.left;
		starty=mag.top;
		endx=mag.left+mag.Width()+1;
		endy=mag.top+mag.Height()+1;
	} else {
		startx=0;
		starty=0;
		endx=w;
		endy=h;
	}

	PIXEL *f0=(PIXEL*)GetBuffer(display);
	count=rsum=gsum=bsum=0;
	for(y=starty; y<endy; y++)
	{
		Y=y*(int)w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			rsum+=(int)f0[i].r;
			gsum+=(int)f0[i].g;
			bsum+=(int)f0[i].b;
			count++;
		}
	}
	FreeBuffer(display);

	if(count==0)
	{
		EndWaitCursor();
		return;
	}

	r=rsum/count;
	g=gsum/count;
	b=bsum/count;

	for(buf=top_frame-1; buf>=0; buf--)
	{
		if(buffer[buf].pointer)
			memory_slot[buffer[buf].slot].buffer++;
	}

	for(buf=top_frame-1; buf>=0; buf--) buffer[buf+1]=buffer[buf];

	buffer[0].inuse=false;
	str1.LoadString(IDS_STRING437);
	buffer[0].name.Format(L"%s",newnumber(str1));
	buffer[0].ondisk=false;
	buffer[0].pointer=0;
	buffer[0].slot=-1;
	buffer[0].updated=false;

	PIXEL *bg=(PIXEL*)GetBuffer(0);
	for(y=0; y<(int)h; y++)
	{
		Y=y*(int)w;
		for(x=0; x<(int)w; x++)
		{
			i=x+Y;
			bg[i].a=(unsigned char)aGOOD;
			bg[i].r=(unsigned char)r;
			bg[i].g=(unsigned char)g;
			bg[i].b=(unsigned char)b;
		}
	}
	UpdateBuffer(0);

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	for(i=0; i<w*h; i++)
	{
		if(depth[i]&fEMPTY) continue;
		depth[i]=((depth[i]&fMASK)|((depth[i]&dMASK)+0x100));
	}
	FreeBuffer(DEPTH_BUFFER);

	top_frame++;

	buffer[0].hidden=true;
	refresh_used_list();
	if(depth_flag) toggledepthmap();

	backup_picture_valid=false;
	redraw_picture();
	EndWaitCursor();
	display=active_frame=0;
	RefreshScreen();
}
