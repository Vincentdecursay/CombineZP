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

/* FIXING ETC. RECTANGLES */

void CCombineDoc::OnFix() 
{
	if(!top_frame) return;

	BeginWaitCursor();
	fix();
	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::fix()
{
	unsigned int x,startx,endx;
	unsigned int y,starty,endy,Y;
	unsigned int i;

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

	int* depth=(int*)GetBuffer(DEPTH_BUFFER);

	if(!plane_mode)
	{
		for(y=starty; y<endy; y++)
		{
			Y=y*w;
			for(x=startx; x<endx; x++)
			{
				i=x+Y;
				if(depth[i]&fFILLED) depth[i]|=fFIXED;
			}
		}
	} else {
		for(y=starty; y<endy; y++)
		{
			Y=y*w;
			for(x=startx; x<endx; x++)
			{
				i=x+Y;
				if(mode&GROUP_MASK==SPECIAL)
				{
					depth[i]|=fSPECIAL_FIXED;
					continue;
				}
				if((((depth[i]&dMASK)+(1<<7))>>8)!=active_frame) continue;
				if(depth[i]&fFILLED) depth[i]|=fFIXED;
			}
		}
	}

	UpdateBuffer(DEPTH_BUFFER);

	if(fix_active)
	{
		backup_picture_valid=false;
		redraw_picture();
	}
}

void CCombineDoc::OnRectangleInvertfixed() 
{
	if(!top_frame) return;

	BeginWaitCursor();

	unsigned int x,startx,endx;
	unsigned int y,starty,endy,Y;
	unsigned int i;

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

	int* depth=(int*)GetBuffer(DEPTH_BUFFER);

	for(y=starty; y<endy; y++)
	{
		Y=y*w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(!plane_mode)
			{
				if(depth[i]&fFILLED)
				{
					if(depth[i]&fFIXED) depth[i]-=fFIXED;
					else depth[i]|=fFIXED;
				}
			} else {
				if(depth[i]&fFILLED)
				{
					if((((depth[i]&dMASK)+(1<<7))>>8)!=active_frame) continue;
					if(depth[i]&fFIXED) depth[i]-=fFIXED;
					else depth[i]+=fFIXED;
				}
			}
		}
	}

	UpdateBuffer(DEPTH_BUFFER);

	if(fix_active)
	{
		backup_picture_valid=false;
		redraw_picture();
	}
	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::OnUnfix() 
{
	if(!top_frame) return;

	BeginWaitCursor();
	unfix();
	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::unfix()
{

	unsigned int i;
	unsigned int x,startx,endx;
	unsigned int y,starty,endy,Y;

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

	int* depth=(int*)GetBuffer(DEPTH_BUFFER);

	for(y=starty; y<endy; y++)
	{
		Y=y*w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(plane_mode && ((((depth[i]&dMASK)+(1<<7))>>8)!=active_frame)) continue;
			if(depth[i]&fFILLED) depth[i]&=~fFIXED;
		}
	}

	UpdateBuffer(DEPTH_BUFFER);

	if(fix_active)
	{
		backup_picture_valid=false;
		redraw_picture();
	}
}