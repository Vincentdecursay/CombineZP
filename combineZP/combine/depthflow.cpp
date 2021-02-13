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
}

struct ddf_Params
{
	int index,step;
	int w,h;
	int bigw,bigh;
	int littlew,littleh;
	int *depth,*big,*t;
	bool *fixed;
	int *used_frame;
	int last_used_frame;
} ddf_params[32];

UINT CCombineDoc::ddf1(LPVOID param)
{
	ddf_Params *pp=(ddf_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	int bigw=pp->bigw;
	int bigh=pp->bigh;
	int *depth=pp->depth;
	int *big=pp->big;
	bool *fixed=pp->fixed;

	int i,j,x,y,Yi,Yj;

	for(y=1+index; y<h-1; y+=step)
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
					((depth[j+1+w]&fFILLED)!=0)*(depth[j+1+w]&dMASK));
				big[i]=((average<<8)+count/2)/count;
				fixed[i]=true;
			}
		}
	}
	return 0;
}

UINT CCombineDoc::ddf2(LPVOID param)
{
	ddf_Params *pp=(ddf_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	int bigw=pp->bigw;
	int bigh=pp->bigh;
	int *depth=pp->depth;
	int *big=pp->big;
	bool *fixed=pp->fixed;

	int i,j,x,y,Yi,Yj,z;

	for(y=1+index; y<h-1; y+=step)
	{
		Yi=(y+1)*bigw;
		Yj=y*w;
		for(x=1; x<w-1; x++)
		{
			i=(x+1)+Yi;
			j=x+Yj;

			if(fixed[i])
			{
				z=big[i];
				z+=1<<7;
				z=z>>8;
				depth[j]=fFILLED|z;
			}
		}
	}

	return 0;
}

UINT CCombineDoc::ddf3(LPVOID param)
{
	ddf_Params *pp=(ddf_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int bigw=pp->bigw;
	int bigh=pp->bigh;
	int *big=pp->big;
	bool *fixed=pp->fixed;
	int *t=pp->t;

	int i,x,y,Yi;

	for(y=1+index; y<bigh-1; y+=step)
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

	return 0;
}

UINT CCombineDoc::ddf4(LPVOID param)
{
	ddf_Params *pp=(ddf_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int bigw=pp->bigw;
	int bigh=pp->bigh;
	int *big=pp->big;
	int *depth=pp->depth;
	bool *fixed=pp->fixed;
	int w=pp->w;
	int h=pp->h;
	int *used_frame=pp->used_frame;
	int last_used_frame=pp->last_used_frame;

	int i,j,x,y,Yi,Yj,z;

	for(y=index; y<h; y+=step)
	{
		Yi=(y+1)*bigw;
		Yj=y*w;
		for(x=0; x<w; x++)
		{
			i=(x+1)+Yi;
			j=x+Yj;
			if(!fixed[i])
			{
				z=big[i];
				if(z>(used_frame[last_used_frame-1])<<16) z=(used_frame[last_used_frame-1])<<16;
				if(z<(used_frame[0])<<16) z=(used_frame[0])<<16;
				z+=1<<7;
				z=z>>8;
				depth[j]=fFILLED|z;
			}
		}
	}

	return 0;
}

void CCombineDoc::DoDepthflow()
{
	backup_picture_valid=false;
	progress.AddString(IDS_GAPS);

	int i,x,y,z;

// Use oversized arrays to simplify edge processing
	int bigw=w+2,bigh=h+2;

	depth_big[0]=(int*)GetBuffer(TEMP_BUFFER+1);
	depth_fixed[0]=(bool*)GetBuffer(TEMP_BUFFER+2);
	char* pointer=(char*)GetBuffer(TEMP_BUFFER+3);

// save pointers to reduced size work areas
	for(i=1,x=bigw/2+2,y=bigh/2+2; (x>=16) && (y>=16); i++,x=x/2+2,y=y/2+2)
	{
		depth_big[i]=(int*)pointer; pointer+=sizeof(int)*x*y;
		depth_fixed[i]=(bool*)pointer; pointer+=sizeof(bool)*x*y;
		if(i==64){
			CString s1,s2;
			s1.LoadString(IDS_DEPTHBIG); s2.LoadString(IDS_INTERNERROR);
			MessageBox(progress.m_hWnd,s1,s2,MB_OK);
			break;
		}
	}

	depth_depth=0;
	int *big=depth_big[0];
	bool *fixed=depth_fixed[0];
	depth_depth++;

	BeginThreads
	SetThreads(w,h)

// populate big arrays
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	for(obj=0; obj<objects; obj++)
	{
		ddf_params[obj].step=objects;
		ddf_params[obj].index=obj;
		ddf_params[obj].big=big;
		ddf_params[obj].bigh=bigh;
		ddf_params[obj].bigw=bigw;
		ddf_params[obj].fixed=fixed;
		ddf_params[obj].w=w;
		ddf_params[obj].h=h;
		ddf_params[obj].depth=depth;
		StartThreads(ddf1,ddf_params)
	}
	EndThreads

	for(obj=0; obj<objects; obj++)
	{
		ddf_params[obj].step=objects;
		ddf_params[obj].index=obj;
		ddf_params[obj].big=big;
		ddf_params[obj].bigh=bigh;
		ddf_params[obj].bigw=bigw;
		ddf_params[obj].fixed=fixed;
		ddf_params[obj].w=w;
		ddf_params[obj].h=h;
		ddf_params[obj].depth=depth;
		StartThreads(ddf2,ddf_params);
	}
	EndThreads

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

	// smooth to get rid of isolated errors
	int* t=(int*)GetBuffer(TEMP_BUFFER);
	for(z=0; z<2; z++)
	{
		for(i=0; i<bigw*bigh; i++) t[i]=big[i];

		for(obj=0; obj<objects; obj++)
		{
			ddf_params[obj].step=objects;
			ddf_params[obj].index=obj;
			ddf_params[obj].big=big;
			ddf_params[obj].bigh=bigh;
			ddf_params[obj].bigw=bigw;
			ddf_params[obj].fixed=fixed;
			ddf_params[obj].t=t;
			StartThreads(ddf3,ddf_params)
		}
		EndThreads
	}

// save result
//	depth=(int*)GetBuffer(DEPTH_BUFFER);
	for(obj=0; obj<objects; obj++)
	{
		ddf_params[obj].step=objects;
		ddf_params[obj].index=obj;
		ddf_params[obj].big=big;
		ddf_params[obj].bigh=bigh;
		ddf_params[obj].bigw=bigw;
		ddf_params[obj].fixed=fixed;
		ddf_params[obj].w=w;
		ddf_params[obj].h=h;
		ddf_params[obj].depth=depth;
		ddf_params[obj].used_frame=used_frame;
		ddf_params[obj].last_used_frame=last_used_frame;
		StartThreads(ddf4,ddf_params);
	}
	EndThreads

	UpdateBuffer(DEPTH_BUFFER);
	FreeBuffer(TEMP_BUFFER);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+2);
	FreeBuffer(TEMP_BUFFER+3);
	progress.SetPos(0);
//	progress.GetDlgItem(IDC_CANCEL)->EnableWindow(true);

	if(refreshscreen)
	{
		redraw_picture();
		display=PICTURE_BUFFER;
		refreshscreen=false;
		progress.m_cancel=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}
	return;
}

struct df1_Params
{
	int index,step;
	int bigw,bigh;
	int littlew,littleh;
	bool *fixed,*fixeds,allfixed;
	int *big,*little,*temp;
} df1_params[32];

UINT CCombineDoc::df1(LPVOID param)
{
	df1_Params *pp=(df1_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int bigw=pp->bigw;
	int bigh=pp->bigh;
	int littlew=pp->littlew;
	int littleh=pp->littleh;
	bool *fixed=pp->fixed;
	bool *fixeds=pp->fixeds;
	bool allfixed=pp->allfixed;
	int *big=pp->big;
	int *little=pp->little;

	int i,j,x,y,Yi,Yj;

	for(i=index; i<littlew*littleh; i+=step){little[i]=0; fixeds[i]=false;}

	allfixed=true;
	for(y=1+index; y<littleh-1; y+=step)
	{
		Yi=y*littlew;
		if(2*y>bigh-2) Yj=(bigh-2)*bigw;
		else Yj=(2*y-1)*bigw;
		for(x=1; x<littlew-1; x++)
		{
			i=x+Yi;
			if(2*x>bigw-2) j=bigw-2+Yj;
			else j=(2*x-1)+Yj;
			if(fixed[j])
			{
				little[i]=big[j];
				fixeds[i]=true;
				continue;
			} else {
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
			}
		}
		allfixed&=fixeds[i];
	}

	return 0;
}

UINT CCombineDoc::df2(LPVOID param)
{
	df1_Params *pp=(df1_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int bigw=pp->bigw;
	int bigh=pp->bigh;
	int littlew=pp->littlew;
	bool *fixed=pp->fixed;
	int *big=pp->big;
	int *little=pp->little;

	int i,j,x,y,Yi,Yj;

	for(y=1+index*2; y<bigh-1; y+=2*step)
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

	return 0;
}

UINT CCombineDoc::df3(LPVOID param)
{
	df1_Params *pp=(df1_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int bigw=pp->bigw;
	int bigh=pp->bigh;
	bool *fixed=pp->fixed;
	int *big=pp->big;
	int *t=pp->temp;

	int i,x,y,Yi;

	for(y=1+index; y<bigh-1; y+=step)
	{
		Yi=y*bigw;
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
			}
		}
	}

	return 0;
}

void CCombineDoc::DepthFlow(int *big, int bigw, int bigh, bool* fixed)
{
	BeginThreads

	int i,z;
	bool allfixed;

// Use oversized arrays to simplify edge processing
	int littlew=bigw/2+2,littleh=bigh/2+2;
	int* little=depth_big[depth_depth];
	bool* fixeds=depth_fixed[depth_depth];
	depth_depth++;

	int maxwh=bigw;
	if(bigh>bigw) maxwh=bigh;
	if((maxwh<MIN_DEPTHFLOW) || (littlew<16) || (littleh<16))
	{
		depth_depth--;
		return;
	}

// populate small array
	SetThreads(bigw,bigh)
	for(obj=0; obj<objects; obj++)
	{
		df1_params[obj].step=objects;
		df1_params[obj].index=obj;
		df1_params[obj].big=big;
		df1_params[obj].bigh=bigh;
		df1_params[obj].bigw=bigw;
		df1_params[obj].fixed=fixed;
		df1_params[obj].fixeds=fixeds;
		df1_params[obj].little=little;
		df1_params[obj].littleh=littleh;
		df1_params[obj].littlew=littlew;
		StartThreads(df1,df1_params)
	}
	EndThreads

	allfixed=true;
	for(obj=0; obj<objects; obj++)
		allfixed&=df1_params[obj].allfixed;

	if(allfixed)
	{
		depth_depth--;
		return; // nothing can be changed
	}

	for(i=0; i<littlew; i++)
	{
		little[i]=little[i+littlew];
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
	for(obj=0; obj<objects; obj++)
	{
		df1_params[obj].step=objects;
		df1_params[obj].index=obj;
		df1_params[obj].big=big;
		df1_params[obj].bigh=bigh;
		df1_params[obj].bigw=bigw;
		df1_params[obj].fixed=fixed;
		df1_params[obj].little=little;
		df1_params[obj].littlew=littlew;
		StartThreads(df2,df1_params)
	}
	EndThreads

// Smooth big array
	int* t=(int*)GetBuffer(TEMP_BUFFER);
	for(i=0; i<bigw*bigh; i++)
	{
		t[i]=big[i];
		if(big[i]<0) t[i]=big[i]=0;
	}

	for(obj=0; obj<objects; obj++)
	{
		df1_params[obj].index=obj;
		df1_params[obj].step=objects;
		df1_params[obj].big=big;
		df1_params[obj].bigh=bigh;
		df1_params[obj].bigw=bigw;
		df1_params[obj].fixed=fixed;
		df1_params[obj].temp=t;
		StartThreads(df3,df1_params);
	}
	EndThreads

	FreeBuffer(TEMP_BUFFER);
	depth_depth--;
	return;
}
