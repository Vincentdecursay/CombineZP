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

void CCombineDoc::OnClear() // rectangle
{
	if(display==OUT_BUFFER) return;

	if((mode&GROUP_MASK)==SPECIAL) return;

	save_state();

/*	if(!plane_mode)
	{
//		depth_flag=false;
		fix_active=false;
		show_fixed=false;
	}
*/
	int i;
	int x,startx,endx;
	int y,starty,endy,Y;
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

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	PIXEL *picture=(PIXEL*)GetBuffer(PICTURE_BUFFER);
	backup_picture_valid=false;

	// just clear pixels on this plane
	if(plane_mode)
	{
		for(y=starty; y<endy; y++)
		{
			Y=y*w;
			for(x=startx; x<endx; x++)
			{
				i=x+Y;
				picture[i].abgr=wipeout;
//				if(((depth[i]&dMASK)>>8)==active_frame) depth[i]=fEMPTY;
				if((((depth[i]&dMASK)+(1<<7))>>8)==active_frame) depth[i]=fEMPTY;
			}
		}
		display=PICTURE_BUFFER;
		RefreshScreen();

		UpdateBuffer(DEPTH_BUFFER);
		UpdateBuffer(PICTURE_BUFFER);
		return;
	}

	// clear a rectangle
	for(y=starty; y<endy; y++)
	{
		Y=w*y;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			picture[i].abgr=wipeout;
			depth[i]=fEMPTY;
		}
	}

	UpdateBuffer(DEPTH_BUFFER);
	UpdateBuffer(PICTURE_BUFFER);

	display=PICTURE_BUFFER;
	RefreshScreen();
}

void CCombineDoc::clearunfixed()
{
	if((mode&GROUP_MASK)==SPECIAL) return;
	OnClearunfixed();
}

void CCombineDoc::OnClearunfixed() 
{
	if(!top_frame) return;

	if(display!=PICTURE_BUFFER) return;

	if((mode&GROUP_MASK)==SPECIAL) return;

	save_state();

	backup_picture_valid=false;

	int i;
	int x,startx,endx;
	int y,starty,endy,Y;

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

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	if(plane_mode)
	{
		PIXEL *picture=(PIXEL*)GetBuffer(PICTURE_BUFFER);
		for(y=starty; y<endy; y++)
		{
			Y=y*w;
			for(x=startx; x<endx; x++)
			{
				i=x+Y;
				if(!(depth[i]&fFIXED))
				{
					picture[i].abgr=wipeout;
//					if(((depth[i]&dMASK)>>8)==active_frame) depth[i]=fEMPTY;
					if((((depth[i]&dMASK)+(1<<7))>>8)==active_frame) depth[i]=fEMPTY;
				}
			}
		}
		UpdateBuffer(DEPTH_BUFFER);
		UpdateBuffer(PICTURE_BUFFER);

		RefreshScreen();
		return;
	}

	PIXEL *picture=(PIXEL*)GetBuffer(PICTURE_BUFFER);
	for(y=starty; y<endy; y++)
	{
		Y=y*w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(!(depth[i]&fFIXED))
			{
				picture[i].abgr=wipeout;
				depth[i]=fEMPTY;
			}
		}
	}
	UpdateBuffer(DEPTH_BUFFER);
	UpdateBuffer(PICTURE_BUFFER);

	display=PICTURE_BUFFER;
	RefreshScreen();
}
