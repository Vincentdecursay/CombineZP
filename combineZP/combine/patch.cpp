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
#include "Progress.h"

void CCombineDoc::OnStackPatchwork() 
{
	static int called_already=false;
	if(called_already){called_already=false; return;} else called_already=true;

	mode=PATCH;
	if(!GetValue())
	{
		mode=0;
		called_already=false;
		return;
	}

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	save_state();
	refreshscreen=true;
 	AfxBeginThread(patch,this);
}

UINT CCombineDoc::patch(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->patch();
	return 0;
}

void CCombineDoc::patch()
{
	backup_picture_valid=false;
	buissy=true;
	progress.AddString(IDS_PATCHWORK,patch_size,patch_threshold);
	progress.m_cancel=false;

	int i,x,y,Y,f;
	int r,g,b,csz=patch_size+2;

	int u;
	int cell_cs=w/csz;
	int cell_rs=h/csz;
	CELL *cell=(CELL*)GetBuffer(TEMP_BUFFER);

	PIXEL p;

	PIXEL *in;
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	if((patch_size==0) || (patch_size>w/8))
		goto end;
	
	for(y=0; y<cell_rs; y++)
	{
		Y=y*cell_cs;
		for(x=0; x<cell_cs; x++)
		{
			u=x+Y;
			cell[u].frame=cell[u].colour_range=0;
		}
	}

	for(y=0; y<h; y++)
	{
		Y=y*w;
		for(x=0; x<w; x++)
		{
			i=x+Y;
			if(depth[i]&fFIXED) continue;
			depth[i]=fEMPTY;
		}
	}

	progress.SetRange(0,top_used_frame-1);

	for(f=0; f<top_used_frame; f++)
	{
		progress.SetPos(f);
		if(progress.m_cancel)break;

		in=(PIXEL*)GetBuffer(used_frame[f]);

		for(y=0; y<h; y++)
		{
			Y=y*w;
			for(x=0; x<w; x++)
			{
				i=x+Y;

				u=x/csz+cell_cs*(y/csz);
				if(((x%csz)==0) && ((y%csz)==0))
				{
					cell[u].rsum=cell[u].gsum=cell[u].bsum=0;
					cell[u].good=true;
				}

				p=in[i];
				if(p.a!=aGOOD)
				{
					cell[u].good=false;
				}

				cell[u].rsum+=(int)p.r;
				cell[u].gsum+=(int)p.g;
				cell[u].bsum+=(int)p.b;
			}
		}
		FreeBuffer(used_frame[f]);

		for(y=1; y<cell_rs-1; y++)
		{
			Y=y*cell_cs;
			for(x=1; x<cell_cs-1; x++)
			{
				u=x+Y;
				if(!(cell[u].good&cell[u-1].good&cell[u+1].good&cell[u-cell_cs].good&cell[u+cell_cs].good))
					continue;
				r=	abs(2*cell[u].rsum-cell[u-1].rsum-cell[u+1].rsum)+
					abs(2*cell[u].rsum-cell[u-cell_cs].rsum-cell[u+cell_cs].rsum);
				g=	abs(2*cell[u].gsum-cell[u-1].gsum-cell[u+1].gsum)+
					abs(2*cell[u].gsum-cell[u-cell_cs].gsum-cell[u+cell_cs].gsum);
				b=	abs(2*cell[u].bsum-cell[u-1].bsum-cell[u+1].bsum)+
					abs(2*cell[u].bsum-cell[u-cell_cs].bsum-cell[u+cell_cs].bsum);
				if(r>cell[u].colour_range)
				{
					cell[u].colour_range=r;
					cell[u].frame=used_frame[f];
				}
				if(g>cell[u].colour_range)
				{
					cell[u].colour_range=g;
					cell[u].frame=used_frame[f];
				}
				if(b>cell[u].colour_range)
				{
					cell[u].colour_range=b;
					cell[u].frame=used_frame[f];
				}
			}
		}
	}

	for(y=1; y<cell_rs-1; y++)
	{
		Y=y*cell_cs;
		for(x=1; x<cell_cs-1; x++)
		{
			u=x+Y;
			if(cell[u].colour_range<csz*csz*patch_threshold) cell[u].frame=-1;

			r=	(cell[u].frame==cell[u-1].frame)+
				(cell[u].frame==cell[u+1].frame)+
				(cell[u].frame==cell[u-cell_cs].frame)+
				(cell[u].frame==cell[u+cell_cs].frame)+
				(cell[u].frame==cell[u-1-cell_cs].frame)+
				(cell[u].frame==cell[u+1+cell_cs].frame)+
				(cell[u].frame==cell[u-cell_cs+1].frame)+
				(cell[u].frame==cell[u+cell_cs-1].frame);
			if(r<3) continue;

			if(cell[u].colour_range<csz*csz*patch_threshold) continue;

			for(g=1; g<csz-1; g++)
			{
				for(b=1; b<csz-1; b++)
				{
					i=(x*csz+b)+(y*csz+g)*w;
					if(!(depth[i]&fFIXED)) depth[i]=(cell[u].frame<<8)|fFILLED;
				}
			}
		}
	}

end:
	FreeBuffer(TEMP_BUFFER);
	UpdateBuffer(DEPTH_BUFFER);
	progress.SetPos(0);
	if(refreshscreen)
	{
		redraw_picture();
		progress.m_cancel=false;
		display=PICTURE_BUFFER;
		refreshscreen=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
		mode=0;
	}
}
