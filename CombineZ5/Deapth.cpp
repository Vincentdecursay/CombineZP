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

void CCombineDoc::OnViewToggledepthmap() 
{
	if((mode&GROUP_MASK)==SPECIAL) return;
	
	BeginWaitCursor();

	toggledepthmap();

//	backup_picture_valid=false;
	redraw_picture();
	display=PICTURE_BUFFER;

	if(plane_mode) draw_plane();
	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::toggledepthmap()
{
	int i;

	if(!depth_flag)
	{
		if(rainbow) delete rainbow;
		rainbow=new PIXEL[top_frame*256];

		for(i=0; i<top_frame*256; i++)
		{
			rainbow[i].abgr=aGOOD<<24;
			rainbow[i].r=i/256;
			rainbow[i].g=(unsigned char)(i/top_frame+1);
			rainbow[i].b=128;
		}
		depth_flag=true;
	} else depth_flag=false;
}

void CCombineDoc::depthmap()
{
	OnViewToggledepthmap();	
}
