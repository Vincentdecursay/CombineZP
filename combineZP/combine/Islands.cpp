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

#define MIN_ISLAND_SIZE 1000

void CCombineDoc::OnStackRemoveislands() 
{
	static int called_already=false;
	if(called_already){called_already=false; return;} else called_already=true;
	mode=ISLAND;
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
	AfxBeginThread(removeislandsT,this);
}

UINT CCombineDoc::removeislandsT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->removeislands();
	return 0;
}

void CCombineDoc::removeislands()
{
	progress.AddString(IDS_ISLANDS,min_island_size);

	int i,cli,m,x,y,xx,yy,Yw;
	short *X,*Y;
	int x0,y0,pn,ps,*toclear,*PS,*PN,f,j,k,t,ub,lb;
	bool *visited;

	visited=new bool[w*h];
	toclear=(int*)GetBuffer(TEMP_BUFFER+0);
	PS=(int*)GetBuffer(TEMP_BUFFER+1); // patch size
	PN=(int*)GetBuffer(TEMP_BUFFER+2);// patch number
	X=(short*)GetBuffer(TEMP_BUFFER+3);
	Y=(short*)GetBuffer(TEMP_BUFFER+4);
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	for(i=0; i<(int)(w*h); i++)
	{
		visited[i]=false;
		PN[i]=-1;
	}
	pn=0;

	progress.SetRange(0,h);
	for(y0=0; y0<h; y0++)
	{
		progress.SetPos(y0);
		for(x0=0; x0<w; x0++)
		{
			k=x0+y0*w;
			if(PN[k]!=-1) continue;
			if(depth[k]&fFIXED) continue;
			if(depth[k]&fEMPTY) continue;
			m=1;
			X[0]=(short)x0; Y[0]=(short)y0;

			f=(((depth[k]&dMASK)+(1<<7))>>8);
			ub=f+2; if(ub>top_frame) ub=top_frame;
			lb=f-2; if(lb<-1) lb=-1;

			ps=0;
			toclear[0]=true;
			cli=1;
			for(;;)
			{
				if(m==0) break;
				m--;

				x=X[m]; y=Y[m];

				ps++;

				PN[x+y*w]=pn;

				xx=x-1;
				if(xx>=0)
				{
					j=xx+y*w;
					t=(((depth[j]&dMASK)+(1<<7))>>8);
					if((!visited[j]) && (t>lb) && (t<ub) && (depth[j]&fFILLED))
						{X[m]=(short)xx; Y[m]=(short)y; m++; visited[j]=true; toclear[cli]=j; cli++;}
				}
	
				yy=y-1;
				if(yy>=0)
				{
					j=x+yy*w;
					t=(((depth[j]&dMASK)+(1<<7))>>8);
					if((!visited[j]) && (t>lb) && (t<ub) && (depth[j]&fFILLED))
						{X[m]=(short)x; Y[m]=(short)yy; m++; visited[j]=true; toclear[cli]=j; cli++;}
				}

				yy=y+1;
				if(yy<h)
				{
					j=x+yy*w;
					t=(((depth[j]&dMASK)+(1<<7))>>8);
					if((!visited[j]) && (t>lb) && (t<ub) && (depth[j]&fFILLED))
						{X[m]=(short)x; Y[m]=(short)yy; m++; visited[j]=true; toclear[cli]=j; cli++;}
				}

				xx=x+1;
				if(xx<w)
				{
					j=xx+y*w;
					t=(((depth[j]&dMASK)+(1<<7))>>8);
					if((!visited[j]) && (t>lb) && (t<ub) && (depth[j]&fFILLED))
						{X[m]=(short)xx; Y[m]=(short)y; m++; visited[j]=true; toclear[cli]=j; cli++;}
				}
			}
			PS[pn]=ps; pn++;
			for(i=0; i<cli; i++) visited[toclear[i]]=false;
		}
	}

	for(y=0; y<h; y++)
	{
		Yw=w*y;
		for(x=0; x<w; x++)
		{
			i=x+Yw;
			if(depth[i]&fFIXED) continue;
			if(PN[i]==-1) continue;
			if(PS[PN[i]]<min_island_size*3) depth[i]=fEMPTY;
		}
	}
	for(i=0; i<(int)(w*h); i++)
	{
		visited[i]=false;
		PN[i]=-1;
	}
	pn=0;

	progress.SetRange(0,h);
	for(y0=0; y0<h; y0++)
	{
		progress.SetPos(y0);
		for(x0=0; x0<w; x0++)
		{
			k=x0+y0*w;
			if(PN[k]!=-1) continue;
			if(depth[k]&fFIXED) continue;
			if(depth[k]&fEMPTY) continue;
			m=1;
			X[0]=(short)x0; Y[0]=(short)y0;
			f=(((depth[k]&dMASK)+(1<<7))>>8);
			ps=0;
			toclear[0]=true;
			cli=1;
			for(;;)
			{
				if(m==0) break;
				m--;

				x=X[m]; y=Y[m];

				ps++;

				PN[x+y*w]=pn;

				xx=x-1;
				if(xx>=0)
				{
					j=xx+y*w;
					t=(((depth[j]&dMASK)+(1<<7))>>8);
					if((!visited[j]) && (t==f) && (depth[j]&fFILLED))
						{X[m]=(short)xx; Y[m]=(short)y; m++; visited[j]=true; toclear[cli]=j; cli++;}
				}
	
				yy=y-1;
				if(yy>=0)
				{
					j=x+yy*w;
					t=(((depth[j]&dMASK)+(1<<7))>>8);
					if((!visited[j]) && (t==f) && (depth[j]&fFILLED))
						{X[m]=(short)x; Y[m]=(short)yy; m++; visited[j]=true; toclear[cli]=j; cli++;}
				}

				yy=y+1;
				if(yy<h)
				{
					j=x+yy*w;
					t=(((depth[j]&dMASK)+(1<<7))>>8);
					if((!visited[j]) && (t==f) && (depth[j]&fFILLED))
						{X[m]=(short)x; Y[m]=(short)yy; m++; visited[j]=true; toclear[cli]=j; cli++;}
				}

				xx=x+1;
				if(xx<w)
				{
					j=xx+y*w;
					t=(((depth[j]&dMASK)+(1<<7))>>8);
					if((!visited[j]) && (t==f) && (depth[j]&fFILLED))
						{X[m]=(short)xx; Y[m]=(short)y; m++; visited[j]=true; toclear[cli]=j; cli++;}
				}
			}
			PS[pn]=ps; pn++;
			for(i=0; i<cli; i++) visited[toclear[i]]=false;
		}
	}

	for(y=0; y<h; y++)
	{
		Yw=w*y;
		for(x=0; x<w; x++)
		{
			i=x+Yw;
			if(depth[i]&fFIXED) continue;
			if(PN[i]==-1) continue;
			if(PS[PN[i]]<min_island_size) depth[i]=fEMPTY;
		}
	}

	delete [] visited;
	FreeBuffer(TEMP_BUFFER+0);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+2);
	FreeBuffer(TEMP_BUFFER+3);
	FreeBuffer(TEMP_BUFFER+4);
	UpdateBuffer(DEPTH_BUFFER);

	progress.SetPos(0);
	backup_picture_valid=false;
	if(refreshscreen)
	{
		redraw_picture();
		display=PICTURE_BUFFER;
		refreshscreen=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}
	mode=0;
}
