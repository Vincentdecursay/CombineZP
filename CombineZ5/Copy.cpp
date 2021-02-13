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

void CCombineDoc::copyunder()
{
	OnCopyunder();
}

// copies or pastes rectangle to vacant pixels only
void CCombineDoc::OnCopyunder() 
{
	save_state();

	if(plane_mode)
	{
		MessageBox(0,
			"Paste is meaningless in Plane mode",
			"Error",
			MB_OK);
		return;
	}

	int x,startx,endx;
	int y,starty,endy,Y;
	int i;
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
	PIXEL* picture=(PIXEL*)GetBuffer(PICTURE_BUFFER);
	PIXEL* backup_picture=(PIXEL*)GetBuffer(BACKUP_PICTURE_BUFFER);
	PIXEL* f0=(PIXEL*)GetBuffer(active_frame);
	backup_picture_valid=false;

	for(y=starty; y<endy; y++)
	{
		Y=y*w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(depth[i]&fFILLED) continue;
			if(depth[i]&fFIXED) continue;
//			if(!(f0[i].a&aGOOD)) continue;
			if(f0[i].a&aCLEAR) continue;

			depth[i]=(active_frame<<8)|fFILLED;

			if(depth_flag) picture[i]=rainbow[active_frame<<8];
			else picture[i]=f0[i];
			picture[i].a=aGOOD;

			backup_picture[i]=f0[i];
			backup_picture[i].a=aGOOD;
		}
	}
	UpdateBuffer(DEPTH_BUFFER);
	UpdateBuffer(PICTURE_BUFFER);
	UpdateBuffer(BACKUP_PICTURE_BUFFER);
	FreeBuffer(active_frame);

	display=PICTURE_BUFFER;
	RefreshScreen();
}

// overwrites everything bar fixed
void CCombineDoc::OnEditCopy() 
{
	if(plane_mode)
	{
		MessageBox(0,
			"Paste is meaningless in Plane mode",
			"Error",
			MB_OK);
		return;
	}

	save_state();

	int x,startx,endx;
	int y,starty,endy,Y;
	int i;

	if(magon)
	{
		startx=mag.left;
		starty=mag.top;
		endx=mag.right+1;
		endy=mag.bottom+1;
	} else {
		startx=0;
		starty=0;
		endx=w;
		endy=h;
	}

	int* depth=(int*)GetBuffer(DEPTH_BUFFER);
	PIXEL* picture=(PIXEL*)GetBuffer(PICTURE_BUFFER);
	PIXEL* backup_picture=(PIXEL*)GetBuffer(BACKUP_PICTURE_BUFFER);
	PIXEL* f0=(PIXEL*)GetBuffer(active_frame);
	backup_picture_valid=false;

	for(y=starty; y<endy; y++)
	{
		Y=w*y;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
//			if(f0[i].a==aGOOD)
//			{
				if(depth[i]&fFIXED) continue;
//				if(!(f0[i].a&aGOOD)) continue;
				if(f0[i].a&aCLEAR) continue;

				depth[i]=(active_frame<<8)|fFILLED;

				if(depth_flag) picture[i]=rainbow[active_frame<<8];
				else picture[i]=f0[i];
				picture[i].a=aGOOD;

				backup_picture[i]=f0[i];
				backup_picture[i].a=aGOOD;
//			}
		}
	}
	UpdateBuffer(DEPTH_BUFFER);
	UpdateBuffer(PICTURE_BUFFER);
	UpdateBuffer(BACKUP_PICTURE_BUFFER);
	FreeBuffer(active_frame);

	display=PICTURE_BUFFER;
	RefreshScreen();
}

void CCombineDoc::OnFrameCopyvisibletoout() 
{
	int i;

	if(display==OUT_BUFFER) return;

	progress.AddString("Replace out");
	progress.AddString("   With "+buffer[display].name);

	PIXEL* src=(PIXEL*)GetBuffer(display);
	PIXEL* dest=(PIXEL*)GetBuffer(OUT_BUFFER);
	for(i=0; i<w*h; i++)
	{
		if(src[i].abgr!=wipeout)
		{
			dest[i]=src[i];
			dest[i].a=aGOOD;
		}
		else
		{
			dest[i].abgr=0;
			dest[i].a=aCLEAR;
		}
	}
	FreeBuffer(display);
	UpdateBuffer(OUT_BUFFER);

	display=OUT_BUFFER;
	RefreshScreen();
}
