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

	progress.AddString("Replace "+buffer[active_frame].name);
	progress.AddString("   With "+buffer[display].name);

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
		MessageBox(0,"Frame Limit Reached","Frame Limit",MB_OK);
		return;
	}

	newfrompicture();

	RefreshScreen();
}

void CCombineDoc::newfrompicture()
{
	GetBuffer(top_frame);
	buffer[top_frame].name=newnumber("copy")+"-"+buffer[display].name;
	buffer[top_frame].hidden=true;
	top_frame++;
	active_frame=top_frame-1;
	Replace();
	display=top_frame-1;
	UpdateBuffer(top_frame-1);
}

void CCombineDoc::clearout()
{
	int i;
	int *out=(int*)GetBuffer(OUT_BUFFER);
	for(i=0; i<(int)(w*h); i++) out[i]=0x000055;
	UpdateBuffer(OUT_BUFFER);
}

void CCombineDoc::OnFrameMakebg() 
{
	if(!top_frame) return;

	int x,startx,endx;
	int y,starty,endy,Y;
	int count,r,g,b,rsum,gsum,bsum,buf;

	int i;

	progress.AddString("Generate a Background frame");

	if(top_frame-1==MAX_FRAMES)
	{
		MessageBox(0,"Frame Limit Reached","Frame Limit",MB_OK);
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
	buffer[0].name.Format("%s",(const char *)newnumber("background"));
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

	backup_picture_valid=false;
	redraw_picture();
	EndWaitCursor();
	display=active_frame=0;
	RefreshScreen();
}
