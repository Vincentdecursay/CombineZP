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

void CCombineDoc::save_state()
{
	int i;
	
	progress.AddString(IDS_STRING624);

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	int *svdepth=(int*)GetBuffer(SAVED_DEPTH_BUFFER);

	for(i=0; i<(int)(w*h); i++)
	{
		svdepth[i]=depth[i];
	}
	UpdateBuffer(SAVED_DEPTH_BUFFER);
	FreeBuffer(DEPTH_BUFFER);
}

void CCombineDoc::OnUndoSetundopoint() 
{
	save_state();
}

void CCombineDoc::home()
{
	if(!top_frame) return;
	if(plane_mode || saved_pm)
	{
		if(display==PICTURE_BUFFER)
		{
			end_plane();
			display=active_frame;
			saved_pm=true;
			plane_mode=false;
		} else {
			display=PICTURE_BUFFER;
			saved_pm=false;
			plane_mode=true;
			draw_plane();
		}
	} else {
		if(display!=PICTURE_BUFFER)
		{
			display=PICTURE_BUFFER;
		}
		else
		{
			if(!((mode&GROUP_MASK)==SPECIAL)) display=active_frame;
			else display=special_active;
		}
	}
}

void CCombineDoc::undo()
{
	if(!top_frame) return;
	if((mode&GROUP_MASK)==SPECIAL) return;
	int i,x,y,Y;
	int startx,endx;
	int starty,endy;

	if((mode&GROUP_MASK)==SPECIAL) return;

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

	int *svdepth=(int*)GetBuffer(SAVED_DEPTH_BUFFER);
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	for(y=starty; y<endy; y++)
	{
		Y=w*y;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			depth[i]=svdepth[i];
		}
	}
	UpdateBuffer(DEPTH_BUFFER);
	FreeBuffer(SAVED_DEPTH_BUFFER);
	display=PICTURE_BUFFER;
	backup_picture_valid=false;
	redraw_picture();
	if(plane_mode) draw_plane();
	RefreshScreen();
}

void CCombineDoc::OnRectangleUndo() 
{
	undo();
}
