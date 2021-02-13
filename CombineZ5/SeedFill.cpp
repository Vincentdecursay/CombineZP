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

void CCombineDoc::seedfill(int x, int y)
{
	if(!top_frame) return;
	if((mode&GROUP_MASK)==SPECIAL) return;
	int delta=brush_delta;

	int d,m,i,j,xx,yy;
	PIXEL p,p0;
	int r,g,b;

	i=x+y*w;

	int startx,endx;
	int starty,endy;
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

	if(x<startx+1 || x>endx-1) return;
	if(y<starty+1 || y>endy-1) return;

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	PIXEL *af=(PIXEL*)GetBuffer(active_frame);
	PIXEL *picture=(PIXEL*)GetBuffer(PICTURE_BUFFER);
	PIXEL *backup_picture=(PIXEL*)GetBuffer(BACKUP_PICTURE_BUFFER);

	BeginWaitCursor();

	bool *visited=new bool[w*h];
	for(int v=0; v<(int)(w*h); v++) visited[v]=false;

	int *X=new int[w*h];
	int *Y=new int[w*h];

	m=0;
	X[m]=x; Y[m]=y; m++;

	if(display==PICTURE_BUFFER) p0=picture[i];
	else p0=af[i];

	bool empty=false;
	if((depth[i]&fEMPTY)) empty=true;

	for(;;)
	{
		if(m==0) break;
		m--;
		x=X[m]; y=Y[m];
		i=x+y*w;

		if(empty)
		{
			d=0;
			empty=false;
		} else {
			if(display==PICTURE_BUFFER) p=picture[i];
			else p=af[i];
			r=(int)p.r-(int)p0.r; g=(int)p.g-(int)p0.g; b=(int)p.b-(int)p0.b;
			if(r<0) r=-r; if(g<0) g=-g; if(b<0) b=-b; d=r; if(g>d) d=g; if(b>d) d=b;
		}

		if(d<delta)
		{
			if(depth[i]&fFIXED && !fix_active) continue;

			if(fix_active)
			{
				depth[i]|=fFIXED;
			} else {
				if(af[i].a==aGOOD)
				{
					depth[i]=(active_frame<<8)|fFILLED;
					backup_picture[i]=af[i];
					if(depth_flag) picture[i]=rainbow[active_frame<<8];
					else picture[i]=af[i];
				}
			}

			visited[i]=true;

			xx=x-1;
			if(xx>=startx)
			{
				j=xx+y*w;
				if(!visited[j])
				{X[m]=xx; Y[m]=y; m++; visited[j]=true;}
			}
	
			yy=y-1;
			if(yy>=starty)
			{
				j=x+yy*w;
				if(!visited[j])
				{X[m]=x; Y[m]=yy; m++; visited[j]=true;}	
			}
	
			yy=y+1;
			if(yy<endy)
			{
				j=x+yy*w;
				if(!visited[j])
				{X[m]=x; Y[m]=yy; m++; visited[j]=true;}
			}
	
			xx=x+1;
			if(xx<endx)
			{
				j=xx+y*w;
				if(!visited[j])
				{X[m]=xx; Y[m]=y; m++; visited[j]=true;}
			}
		}
	}

	delete visited;
	delete X;
	delete Y;

	UpdateBuffer(DEPTH_BUFFER);
	UpdateBuffer(PICTURE_BUFFER);
	UpdateBuffer(BACKUP_PICTURE_BUFFER);
	FreeBuffer(active_frame);

	EndWaitCursor();
	backup_picture_valid=false;
	redraw_picture();
	RefreshScreen();
}

void CCombineDoc::clearbg(int x,int y)
{
	if(!top_frame) return;
	if((mode&GROUP_MASK)==SPECIAL) return;

	if(magon && full_mode)
	{
		if(x<mag.left || x>mag.right || y<mag.top || y>mag.bottom)
			MessageBox(0,"You are outside the Selected rectangle","Clearing Pixels",MB_OK);
	}

	if(display!=PICTURE_BUFFER) return;

	int delta=brush_delta;

	int d,m,i,j,xx,yy;
	PIXEL p,p0;
	int r,g,b;

	i=x+y*w;

	int startx,endx;
	int starty,endy;
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

	if(x<startx+1 || x>endx-1) return;
	if(y<starty+1 || y>endy-1) return;

	BeginWaitCursor();

	int v;
	bool *visited;
	visited=new bool[w*h];
	for(v=0; v<(int)(w*h); v++) visited[v]=false;

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	if(depth[i]&fEMPTY)
	{
		FreeBuffer(DEPTH_BUFFER);
		EndWaitCursor();
		return;
	}

	PIXEL *picture=(PIXEL*)GetBuffer(PICTURE_BUFFER);
	PIXEL *backup_picture=(PIXEL*)GetBuffer(BACKUP_PICTURE_BUFFER);
	p0=picture[i];

	m=0;
	short* X=new short[w*h];
	short* Y=new short[w*h];
	X[m]=x; Y[m]=y; m++;

	for(;;)
	{
		if(m==0) break;
		m--;
		x=X[m]; y=Y[m];
		i=x+y*(int)w;

		p=picture[i];
		r=p.r-p0.r; g=p.g-p0.g; b=p.b-p0.b;
		if(r<0) r=-r; if(g<0) g=-g; if(b<0) b=-b; d=r; if(g>d) d=g; if(b>d) d=b;

		if(d<delta)
		{
			if(depth[i]&fFIXED && !fix_active) continue;

			if(fix_active)
			{
				depth[i]&=~fFIXED;
			} else {
				depth[i]=fEMPTY;
				backup_picture[i].abgr=picture[i].abgr=wipeout;
			}

			xx=x-1;
			if(xx>=startx)
			{
				j=xx+y*w;
				if(!visited[j])
				{X[m]=xx; Y[m]=y; m++; visited[j]=true;}
			}
	
			yy=y-1;
			if(yy>=starty)
			{
				j=x+yy*w;
				if(!visited[j])
				{X[m]=x; Y[m]=yy; m++; visited[j]=true;}	
			}
	
			yy=y+1;
			if(yy<endy)
			{
				j=x+yy*w;
				if(!visited[j])
				{X[m]=x; Y[m]=yy; m++; visited[j]=true;}
			}
	
			xx=x+1;
			if(xx<endx)
			{
				j=xx+y*w;
				if(!visited[j])
				{X[m]=xx; Y[m]=y; m++; visited[j]=true;}
			}
		}
	}

	delete visited;
	delete X;
	delete Y;

	UpdateBuffer(DEPTH_BUFFER);
	UpdateBuffer(PICTURE_BUFFER);
	UpdateBuffer(BACKUP_PICTURE_BUFFER);

	EndWaitCursor();
	RefreshScreen();
}
