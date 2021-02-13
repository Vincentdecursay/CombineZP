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

void CCombineDoc::OnStackDepthflow() 
{
	save_state();
	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;
	AfxBeginThread(DoDepthflowT,this);
}

UINT CCombineDoc::DoDepthflowT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->DoDepthflow();
	return 0;
}

void CCombineDoc::DoColourflow()
{
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	int f,*fr,p1,p2,x,y,Y,i;
	for(f=0; f<top_frame; f++)
	{
		fr=(int*)GetBuffer(f);

		for(y=0; y<h; y++)
		{
			Y=y*w;
			for(x=1; x<w; x++)
			{
				i=x+Y;
				if((((depth[i-1]&dMASK)+128)&0xff00) != 256*f) continue;
				if((depth[i]&fEMPTY) &! (depth[i-1]&fEMPTY))
				{
					p1=fr[i-1]&0x00fcfcfc;
					p2=fr[i]&0x00fcfcfc;
					if(p1!=p2) continue;
					depth[i]=depth[i-1];
				}
			}
		}

		for(y=0; y<h; y++)
		{
			Y=y*w;
			for(x=w-2; x>-1; x--)
			{
				i=x+Y;
				if((((depth[i+1]&dMASK)+128)&0xff00) != 256*f) continue;
				if((depth[i]&fEMPTY) &! (depth[i+1]&fEMPTY))
				{
					p1=fr[i+1]&0x00fcfcfc;
					p2=fr[i]&0x00fcfcfc;
					if(p1!=p2) continue;
					depth[i]=depth[i+1];
				}
			}
		}

		for(x=0; x<w; x++)
		{
			for(y=1; y<h; y++)
			{
				i=x+y*w;
				if((((depth[i-w]&dMASK)+128)&0xff00) != 256*f) continue;
				if((depth[i]&fEMPTY) &! (depth[i-w]&fEMPTY))
				{
					p1=fr[i-w]&0x00fcfcfc;
					p2=fr[i]&0x00fcfcfc;
					if(p1!=p2) continue;
					depth[i]=depth[i-w];
				}
			}
		}

		for(x=0; x<w; x++)
		{
			for(y=h-2; y>-1; y--)
			{
				i=x+y*w;
				if((((depth[i+w]&dMASK)+128)&0xff00) != 256*f) continue;
				if((depth[i]&fEMPTY) &! (depth[i+w]&fEMPTY))
				{
					p1=fr[i+w]&0x00fcfcfc;
					p2=fr[i]&0x00fcfcfc;
					if(p1!=p2) continue;
					depth[i]=depth[i+w];
				}
			}
		}
		FreeBuffer(f);
	}
	UpdateBuffer(DEPTH_BUFFER);
}

void CCombineDoc::DoDepthflow()
{
	backup_picture_valid=false;
	progress.AddString(IDS_GAPS);
	progress.m_cancel=false;

	DoColourflow();

	int i,j,x,y,Yi,Yj,z;

// Use oversized arrays to simplify edge processing
	int bigw=w+2,bigh=h+2;

	depth_big[0]=(int*)GetBuffer(TEMP_BUFFER+1);
	char* pointer=(char*)GetBuffer(TEMP_BUFFER+2);

	// save pointers to reduced size work areas
	depth_fixed[0]=(bool*)pointer; pointer+=bigw*bigh;
	for(i=1,x=bigw/2+2,y=bigh/2+2; (x>MIN_DEPTHFLOW/4) && (y>MIN_DEPTHFLOW/4); i++,x=x/2+2,y=y/2+2)
	{
		depth_big[i]=(int*)pointer; pointer+=4*x*y;
		depth_fixed[i]=(bool*)pointer; pointer+=x*y;
	}

	depth_depth=0;
	int *big=depth_big[depth_depth];
	bool *fixed=depth_fixed[depth_depth];
	depth_depth++;

// populate big arrays
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	for(y=1; y<h-1; y++)
	{
		Yi=(y+1)*bigw;
		Yj=y*w;
		for(x=1; x<w-1; x++)
		{
			i=(x+1)+Yi;
			j=x+Yj;
			if(depth[j]&fFILLED)
			{
				big[i]=(depth[j]&dMASK)<<8;
				fixed[i]=true;
				continue;
			}
			int count,average;
			big[i]=0;
			fixed[i]=false;
			count=
				((depth[j-1]&fFILLED)!=0)+
				((depth[j+1]&fFILLED)!=0)+
				((depth[j-w]&fFILLED)!=0)+
				((depth[j+w]&fFILLED)!=0)+
				((depth[j-1-w]&fFILLED)!=0)+
				((depth[j+1-w]&fFILLED)!=0)+
				((depth[j-1+w]&fFILLED)!=0)+
				((depth[j+1+w]&fFILLED)!=0);
			if(count)
			{
				average=(
					((depth[j-1]&fFILLED)!=0)*(depth[j-1]&dMASK)+
					((depth[j+1]&fFILLED)!=0)*(depth[j+1]&dMASK)+
					((depth[j-w]&fFILLED)!=0)*(depth[j-w]&dMASK)+
					((depth[j+w]&fFILLED)!=0)*(depth[j+w]&dMASK)+
					((depth[j-1-w]&fFILLED)!=0)*(depth[j-1-w]&dMASK)+
					((depth[j+1-w]&fFILLED)!=0)*(depth[j+1-w]&dMASK)+
					((depth[j-1+w]&fFILLED)!=0)*(depth[j-1+w]&dMASK)+
					((depth[j+1+w]&fFILLED)!=0)*(depth[j+1+w]&dMASK))/count;
				big[i]=average<<8;
			}
		}
	}
	FreeBuffer(DEPTH_BUFFER);

// clear big arrays edges
	for(i=0; i<bigw; i++)
	{
		big[i]=big[i+bigw*(bigh-1)]=0;
		fixed[i]=fixed[i+bigw*(bigh-1)]=false;
		big[i+bigw]=big[i+bigw*(bigh-2)]=0;
		fixed[i+bigw]=fixed[i+bigw*(bigh-2)]=false;
	}
	for(i=0; i<bigw*bigh; i+=bigw)
	{
		big[i]=big[i+bigw-1]=0;
		fixed[i]=fixed[i+bigw-1]=false;
		big[i+1]=big[i+bigw-2]=0;
		fixed[i+1]=fixed[i+bigw-2]=false;
	}

	progress.SetRange(0,4);
	for(z=0; z<4; z++)
	{
		progress.SetPos(z);
		if(progress.m_cancel) break;
// This is an iterative function
		DepthFlow(big,bigw,bigh,fixed);
	}

	FreeBuffer(TEMP_BUFFER+2);

	// smooth to get rid of isolated errors
	int* t=(int*)GetBuffer(TEMP_BUFFER);
	for(z=0; z<2; z++)
	{
		for(i=0; i<bigw*bigh; i++) t[i]=big[i];

		for(y=1; y<bigh-1; y++)
		{
			Yi=y*bigw;
			for(x=1; x<bigw-1; x++)
			{
				i=x+Yi;
				if(!fixed[i])
				{
					big[i]=(
							t[i-1-bigw]+
							t[i-1+bigw]+
							t[i+1-bigw]+
							t[i+1+bigw]+
							t[i-1]+
							t[i-bigw]+
							t[i]+
							t[i+bigw]+
							t[i+1]+5)/9;
				}
			}
		}
	}
	FreeBuffer(TEMP_BUFFER);

// save result
	depth=(int*)GetBuffer(DEPTH_BUFFER);
	for(y=0; y<h; y++)
	{
		Yi=(y+1)*bigw;
		Yj=y*w;
		for(x=0; x<w; x++)
		{
			i=(x+1)+Yi;
			j=x+Yj;
			if(depth[j]&fEMPTY)
			{
				z=big[i];
				z+=1<<7;
				z=z>>8;
				if(!(depth[j]&fFIXED)) depth[j]=fFILLED|z;
			}
		}
	}
	UpdateBuffer(DEPTH_BUFFER);
	FreeBuffer(TEMP_BUFFER+1);
	progress.SetPos(0);
	progress.GetDlgItem(IDC_CANCEL)->EnableWindow(true);
	if(refreshscreen)
	{
		redraw_picture();
		display=PICTURE_BUFFER;
		refreshscreen=false;
		progress.m_cancel=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
		RefreshScreen();
	}
	return;
}

void CCombineDoc::DepthFlow(int *big, int bigw, int bigh, bool* fixed)
{
	int i,j,x,y,Yi,Yj,z;
	bool allfixed,changed;

// Use oversized arrays to simplify edge processing
	int littlew=bigw/2+2,littleh=bigh/2+2;
	int* little=depth_big[depth_depth];
	bool* fixeds=depth_fixed[depth_depth];
	depth_depth++;

	int maxwh=bigw;
	if(bigh>bigw) maxwh=bigh;
	if(maxwh<MIN_DEPTHFLOW)
	{
		depth_depth--;
		return;
	}

	for(i=0; i<littlew*littleh; i++){little[i]=0; fixeds[i]=false;}

// populate small array
	allfixed=true;
	for(y=1; y<littleh-1; y++)
	{
		Yi=y*littlew;
		if(2*y>bigh-2) Yj=(bigh-2)*bigw;
		else Yj=(2*y-1)*bigw;
		for(x=1; x<littlew-1; x++)
		{
			i=x+Yi;
			if(2*x>bigw-2) j=bigw-2+Yj;
			else j=(2*x-1)+Yj;
			little[i]=(
							big[j-1-bigw]+
							big[j-1+bigw]+
							big[j+1-bigw]+
							big[j+1+bigw]+
							big[j-1]+
							big[j-bigw]+
							big[j]+
							big[j+bigw]+
							big[j+1]+5)/9;
			fixeds[i]=
							fixed[j-1-bigw]|
							fixed[j-1+bigw]|
							fixed[j+1-bigw]|
							fixed[j+1+bigw]|
							fixed[j-1]|
							fixed[j-bigw]|
							fixed[j]|
							fixed[j+bigw]|
							fixed[j+1];
			allfixed&=fixeds[i];
		}
	}
	if(allfixed)
	{
		depth_depth--;
		return; // nothing can be changed
	}
	for(i=0; i<littlew; i++)
	{
		little[i]=little[i+w];
		little[i+littlew*(littleh-1)]=little[i+littlew*(littleh-2)];
		fixeds[i]=fixeds[i+littlew*(littleh-1)]=false;
	}
	for(i=0; i<littlew*littleh; i+=littlew)
	{
		little[i]=little[i+1];
		little[i+littlew-1]=little[i+littlew-2];
		fixeds[i]=fixeds[i+littlew-1]=false;
	}

// This is an iterative function
	for(z=0; z<3; z++)
	{
		DepthFlow(little,littlew,littleh,fixeds);
	}

// Fill holes in big array
	for(y=1; y<bigh-1; y+=2)
	{
		Yj=y*bigw;
		Yi=((y+1)/2)*littlew;
		for(x=1; x<bigw-1; x+=2)
		{
			i=((x+1)/2)+Yi;
			j=x+Yj;
			if(!fixed[j]) big[j]=little[i]; j+=1;
			if(!fixed[j]) big[j]=(little[i]+little[i+1]+1)/2; j=j-1+bigw;
			if(!fixed[j]) big[j]=(little[i]+little[i+littlew]+1)/2; j=j+1;
			if(!fixed[j]) big[j]=(little[i]+little[i+1]+little[i+littlew]+little[i+littlew+1]+2)/4;
		}
	}

// Smooth big array
	int* t=(int*)GetBuffer(TEMP_BUFFER);
	for(z=0; z<7; z++)
	{
		for(i=0; i<bigw*bigh; i++)
		{
			t[i]=big[i];
			if(big[i]<0) t[i]=big[i]=0;
			if(big[i]>(top_frame-1)<<16) t[i]=big[i]=(top_frame-1)<<16;
		}

		for(y=1; y<bigh-1; y++)
		{
			Yi=y*bigw;
			changed=false;
			for(x=1; x<bigw-1; x++)
			{
				i=x+Yi;
				if(!fixed[i])
				{
					big[i]=(
							t[i-1]+
							t[i-bigw]+
							t[i]+
							t[i+bigw]+
							t[i+1]+3)/5;
					if((t[i]|0xff)==(big[i]|0xff)) changed=true;
				}
			}
		}
		if(!changed) break;
	}
	FreeBuffer(TEMP_BUFFER);
	depth_depth--;
	return;
}
