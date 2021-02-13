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
#include "combineView.h"

void CCombineView::OnViewToggleplanemode() 
{
	CCombineDoc* doc;
	doc=GetDocument();

	getpoint3();
	InvalidateRect(0,false);
}

void CCombineDoc::end_plane()
{
	backup_picture_valid=false;
	redraw_picture();
}

void CCombineDoc::draw_plane()
{
	int i,x,y,Y;

	PIXEL *picture=(PIXEL*)GetBuffer(PICTURE_BUFFER);
	PIXEL *af=(PIXEL*)GetBuffer(active_frame);
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	for(y=0; y<h; y++)
	{
		Y=w*y;
		for(x=0; x<w; x++)
		{
			i=x+Y;
			if(depth[i]&fEMPTY) continue;
			if((((depth[i]&dMASK)+(1<<7))>>8)==active_frame)
			{
				if(fix_active && (depth[i]&fFIXED) && show_fixed)
				{
					picture[i].abgr=((~wipeout)&0xffffff);
				} else {
					if(depth_flag) picture[i]=rainbow[depth[i]&dMASK];
					else picture[i].abgr=af[i].abgr;
				}
			} else {
				picture[i].abgr=wipeout;
			}
		}
	}
	UpdateBuffer(PICTURE_BUFFER);
	FreeBuffer(active_frame);
	FreeBuffer(DEPTH_BUFFER);
}

void CCombineDoc::plane_up()
{
	if(display==PICTURE_BUFFER)
	{
		if(active_frame!=top_frame-1) active_frame++;
	}
	display=PICTURE_BUFFER;
	draw_plane();
}

void CCombineDoc::plane_down()
{
	if(display==PICTURE_BUFFER)
	{
		if(active_frame!=0) active_frame--;
	}
	display=PICTURE_BUFFER;
	draw_plane();
}

void CCombineDoc::toggle_plane(int x,int y)
{
	if((mode&GROUP_MASK)==SPECIAL) return;
	if(top_frame<2) return;

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	if(plane_mode)
	{
		plane_mode=false;
		end_plane();
	} else {
		if(magon)
		{
			if(!(x<mag.left || y<mag.top || x>=mag.right || y>=mag.bottom)) //return;
			{
				if(depth[x+w*y]&fFILLED)
					active_frame=(((depth[x+w*y]&dMASK)+(1<<7))>>8);
			}
		} else {
			if(!(x<0 || y<0 || x>=w || y>=h)) //return;
			{
				if(depth[x+w*y]&fFILLED)
					active_frame=(((depth[x+w*y]&dMASK)+(1<<7))>>8);
			}
		}
		plane_mode=true;
		draw_plane();
		display=PICTURE_BUFFER;
	}

	FreeBuffer(DEPTH_BUFFER);
}
