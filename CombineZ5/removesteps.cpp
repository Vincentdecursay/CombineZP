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

void CCombineDoc::OnStackRemovesteps() 
{
	save_state();
	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;
	AfxBeginThread(removesteps,this);
}

UINT CCombineDoc::removesteps(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->removesteps();
	return 0;
}

void CCombineDoc::removesteps()
{
	buissy=true;
	progress.AddString(IDS_STEPS);
	progress.SetRange(0,4);

	int step_delta=50;
	int linarity_limit=200;

	int x,y,i,Y,d1,d2;
	int r11,r21,g11,g21,b11,b21;
	int r12,r22,g12,g22,b12,b22;
	int r13,r23,g13,g23,b13,b23;
	int linarity1,linarity2;

	if((plane_mode) || (depth_flag))
	{
		if(depth_flag) toggledepthmap();
		if(plane_mode) toggle_plane(0,0);
		redraw_picture();
	}

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	PIXEL *f1,*f2;

// forward x
	for(y=0; y<h; y++)
	{
		Y=w*y;
		for(x=1; x<w-1; x++)
		{
			i=x+Y;
			if(depth[i]&fFIXED) continue;
			if(depth[i]&fEMPTY) continue;
			if(depth[i+1]&fFIXED) continue;
			if(depth[i+1]&fEMPTY) continue;
			d1=((depth[i]&dMASK)>>8);
			d2=((depth[i]&dMASK)>>8);
			if(d1!=d2)
			{
				f1=(PIXEL*)GetBuffer(d1);
				f2=(PIXEL*)GetBuffer(d2);

				r12=(int)f1[i].r; r22=(int)f2[i].r;
				g12=(int)f1[i].g; g22=(int)f2[i].g;
				b12=(int)f1[i].b; b22=(int)f2[i].b;

				if(abs(r12-r22)+abs(g12-g22)+abs(b12-b22)<step_delta)
				{
					FreeBuffer(d1);
					FreeBuffer(d2);
					continue;
				}

				r13=(int)f1[i+1].r; r23=(int)f2[i-2].r;
				g13=(int)f1[i+1].g; g23=(int)f2[i-2].g;
				b13=(int)f1[i+1].b; b23=(int)f2[i-2].b;

				r11=(int)f1[i+1].r; r21=(int)f2[i-1].r;
				g11=(int)f1[i+1].g; g21=(int)f2[i-1].g;
				b11=(int)f1[i+1].b; b21=(int)f2[i-1].b;

				// how close is midpoint to line through ends?
				linarity1=abs(r11+r13-2*r12)+abs(g11+g13-2*g12)+abs(b11+b13-2*b12);
				linarity2=abs(r21+r23-2*r22)+abs(g21+g23-2*g22)+abs(b21+b23-2*b22);

				if((linarity1>linarity_limit) || (linarity2>linarity_limit))
				{
					if(d1>d2) depth[i+1]=(depth[i]&0xffffff00);
					FreeBuffer(d1);
					FreeBuffer(d2);
					continue;
				}

				if(linarity1<linarity2)	depth[i+1]=(depth[i]&0xffffff00);

				FreeBuffer(d1);
				FreeBuffer(d2);
			}
		}
	}
	progress.SetPos(1);

// forward y
	for(y=1; y<h-1; y++)
	{
		Y=w*y;
		for(x=0; x<w; x++)
		{
			i=x+Y;
			if(depth[i]&fFIXED) continue;
			if(depth[i]&fEMPTY) continue;
			if(depth[i+w]&fFIXED) continue;
			if(depth[i+w]&fEMPTY) continue;
			d1=((depth[i]&dMASK)>>8);
			d2=((depth[i+w]&dMASK)>>8);
			if(d1!=d2)
			{
				f1=(PIXEL*)GetBuffer(d1);
				f2=(PIXEL*)GetBuffer(d2);

				r12=(int)f1[i].r; r22=(int)f2[i].r;
				g12=(int)f1[i].g; g22=(int)f2[i].g;
				b12=(int)f1[i].b; b22=(int)f2[i].b;

				if(abs(r12-r22)+abs(g12-g22)+abs(b12-b22)<step_delta)
				{
					FreeBuffer(d1);
					FreeBuffer(d2);
					continue;
				}

				r13=(int)f1[i+w].r; r23=(int)f2[i-w].r;
				g13=(int)f1[i+w].g; g23=(int)f2[i-w].g;
				b13=(int)f1[i+w].b; b23=(int)f2[i-w].b;

				r11=(int)f1[i+w].r; r21=(int)f2[i-w].r;
				g11=(int)f1[i+w].g; g21=(int)f2[i-w].g;
				b11=(int)f1[i+w].b; b21=(int)f2[i-w].b;

				// how close is midpoint to line through ends?
				linarity1=abs(r11+r13-2*r12)+abs(g11+g13-2*g12)+abs(b11+b13-2*b12);
				linarity2=abs(r21+r23-2*r22)+abs(g21+g23-2*g22)+abs(b21+b23-2*b22);

				if((linarity1>linarity_limit) || (linarity2>linarity_limit))
				{
					if(d1>d2) depth[i+w]=(depth[i]&0xffffff00);
					FreeBuffer(d1);
					FreeBuffer(d2);
					continue;
				}

				if(linarity1<linarity2)	depth[i+w]=(depth[i]&0xffffff00);

				FreeBuffer(d1);
				FreeBuffer(d2);
			}
		}
	}
	progress.SetPos(2);

// backward x
	for(y=0; y<h; y++)
	{
		Y=w*y;
		for(x=w-2; x>1; x--)
		{
			i=x+Y;
			if(depth[i]&fFIXED) continue;
			if(depth[i]&fEMPTY) continue;
			if(depth[i-1]&fFIXED) continue;
			if(depth[i-1]&fEMPTY) continue;
			d1=((depth[i]&dMASK)>>8);
			d2=((depth[i-1]&dMASK)>>8);
			if(d1!=d2)
			{
				f1=(PIXEL*)GetBuffer(d1);
				f2=(PIXEL*)GetBuffer(d2);

				r12=(int)f1[i].r; r22=(int)f2[i].r;
				g12=(int)f1[i].g; g22=(int)f2[i].g;
				b12=(int)f1[i].b; b22=(int)f2[i].b;

				if(abs(r12-r22)+abs(g12-g22)+abs(b12-b22)<step_delta)
				{
					FreeBuffer(d1);
					FreeBuffer(d2);
					continue;
				}

				r13=(int)f1[i-1].r; r23=(int)f2[i+2].r;
				g13=(int)f1[i-1].g; g23=(int)f2[i+2].g;
				b13=(int)f1[i-1].b; b23=(int)f2[i+2].b;

				r11=(int)f1[i-1].r; r21=(int)f2[i+1].r;
				g11=(int)f1[i-1].g; g21=(int)f2[i+1].g;
				b11=(int)f1[i-1].b; b21=(int)f2[i+1].b;

				// how close is midpoint to line through ends?
				linarity1=abs(r11+r13-2*r12)+abs(g11+g13-2*g12)+abs(b11+b13-2*b12);
				linarity2=abs(r21+r23-2*r22)+abs(g21+g23-2*g22)+abs(b21+b23-2*b22);

				if((linarity1>linarity_limit) || (linarity2>linarity_limit))
				{
					if(d1>d2) depth[i-1]=(depth[i]&0xffffff00);
					FreeBuffer(d1);
					FreeBuffer(d2);
					continue;
				}

				if(linarity1<linarity2)	depth[i-1]=(depth[i]&0xffffff00);

				FreeBuffer(d1);
				FreeBuffer(d2);
			}
		}
	}
	progress.SetPos(3);

// backward y
	for(y=h-2; y>1; y--)
	{
		Y=w*y;
		for(x=0; x<w; x++)
		{
			i=x+Y;
			if(depth[i]&fFIXED) continue;
			if(depth[i]&fEMPTY) continue;
			if(depth[i-w]&fFIXED) continue;
			if(depth[i-w]&fEMPTY) continue;
			d1=((depth[i]&dMASK)>>8);
			d2=((depth[i-w]&dMASK)>>8);
			if(d1!=d2)
			{
				f1=(PIXEL*)GetBuffer(d1);
				f2=(PIXEL*)GetBuffer(d2);

				r12=(int)f1[i].r; r22=(int)f2[i].r;
				g12=(int)f1[i].g; g22=(int)f2[i].g;
				b12=(int)f1[i].b; b22=(int)f2[i].b;

				if(abs(r12-r22)+abs(g12-g22)+abs(b12-b22)<step_delta)
				{
					FreeBuffer(d1);
					FreeBuffer(d2);
					continue;
				}

				r13=(int)f1[i-w].r; r23=(int)f2[i+w].r;
				g13=(int)f1[i-w].g; g23=(int)f2[i+w].g;
				b13=(int)f1[i-w].b; b23=(int)f2[i+w].b;

				r11=(int)f1[i-w].r; r21=(int)f2[i+w].r;
				g11=(int)f1[i-w].g; g21=(int)f2[i+w].g;
				b11=(int)f1[i-w].b; b21=(int)f2[i+w].b;

				// how close is midpoint to line through ends?
				linarity1=abs(r11+r13-2*r12)+abs(g11+g13-2*g12)+abs(b11+b13-2*b12);
				linarity2=abs(r21+r23-2*r22)+abs(g21+g23-2*g22)+abs(b21+b23-2*b22);

				if((linarity1>linarity_limit) || (linarity2>linarity_limit))
				{
					if(d1>d2) depth[i-w]=(depth[i]&0xffffff00);
					FreeBuffer(d1);
					FreeBuffer(d2);
					continue;
				}

				if(linarity1<linarity2)	(depth[i]&0xffffff00);

				FreeBuffer(d1);
				FreeBuffer(d2);
			}
		}
	}
	progress.SetPos(4);

	UpdateBuffer(DEPTH_BUFFER);

	progress.SetPos(0);
	backup_picture_valid=false;
	if(refreshscreen)
	{
		redraw_picture();
		buissy=false;
		display=PICTURE_BUFFER;
		refreshscreen=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
		RefreshScreen();
	}
}
