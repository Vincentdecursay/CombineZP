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

void CCombineDoc::OnSpecialMakevisibleactiveframe2() 
{
	active_frame2=display;
	correction_factor=1.0;
	add_factor=1.0;
	blend_factor=0.5;
	Redraw(0,false);
}

void CCombineDoc::OnSpecialAdd() 
{
	if(!start_special()) return;

	mode=ADD;
	special_active=active_frame;
	old_mag=mag;
	add_factor=-1.0;
	add();

	if(magon && !full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

void CCombineDoc::OnSpecialBlend() 
{
	if(!start_special()) return;

	mode=BLEND;
	special_active=active_frame;
	old_mag=mag;
	blend();
	if(magon && !full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

void CCombineDoc::blend()
{
	unsigned int i;
	unsigned int x,startx,endx;
	unsigned int y,starty,endy,Y;

	BeginWaitCursor();

	special_changed=true;

	mode=BLEND;

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

	PIXEL *out,*af,*af2;
	out=(PIXEL*)GetBuffer(OUT_BUFFER);
	af=(PIXEL*)GetBuffer(active_frame);
	af2=(PIXEL*)GetBuffer(active_frame2);
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	double r,g,b;

	for(y=starty; y<endy; y++)
	{
		Y=y*w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(depth[i]&fSPECIAL_FIXED) continue;

			if(af2[i].a&aCLEAR)
			{
				out[i]=af[i];
				continue;
			}

			r=(1.0-blend_factor)*(double)(af[i].r)+(blend_factor)*(double)af2[i].r;
			g=(1.0-blend_factor)*(double)(af[i].g)+(blend_factor)*(double)af2[i].g;
			b=(1.0-blend_factor)*(double)(af[i].b)+(blend_factor)*(double)af2[i].b;
			if(r<0.0) r=0.0; if(r>255.0) r=255.0;
			if(g<0.0) g=0.0; if(g>255.0) g=255.0;
			if(b<0.0) b=0.0; if(b>255.0) b=255.0;
			out[i].r=(unsigned char)r;
			out[i].g=(unsigned char)g;
			out[i].b=(unsigned char)b;
			out[i].a=0;
			continue;
		}
	}
	display=OUT_BUFFER;
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(DEPTH_BUFFER);
	FreeBuffer(active_frame);
	FreeBuffer(active_frame2);
	EndWaitCursor();
}


void CCombineDoc::On2framefunctionsCorrectbrightness() // Neutral Background
{
	if(!start_special()) return;

	mode=CORRECT;
	special_active=active_frame;
	old_mag=mag;
	correct(1);

	RefreshScreen();
}

void CCombineDoc::OnSpecialCorrectBrightness() 
{
	if(!start_special()) return;

	mode=CORRECT;
	special_active=active_frame;
	old_mag=mag;
	correct(0);

	RefreshScreen();
}

struct Correct_Params
{
	int index,step;
	unsigned int startx,endx,starty,endy;
	int w;
	int *depth;
	PIXEL *out,*af,*af2;
	int m;
	double averager,averageg,averageb;
	double cf;
} correct_params[32];

UINT CCombineDoc::c1(LPVOID param)
{
	Correct_Params *pp=(Correct_Params *)param;
	int index=pp->index;
	int step=pp->step;
	unsigned int startx=pp->startx;
	unsigned int endx=pp->endx;
	unsigned int starty=pp->starty;
	unsigned int endy=pp->endy;
	int w=pp->w;
	int *depth=pp->depth;
	PIXEL *out=pp->out;
	PIXEL *af=pp->af;
	PIXEL *af2=pp->af2;
	int m=pp->m;
	double averager=pp->averager;
	double averageg=pp->averageg;
	double averageb=pp->averageb;
	double cf=pp->cf;

	double r,g,b,f,d,r1,r2,g1,g2,b1,b2;

	unsigned int i,x,y,Y;

	double averagergb=(averager+averageg+averageb)/3;

	for(y=starty+index; y<endy; y+=step)
	{
		Y=y*w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(depth[i]&fFIXED)
			{
				out[i]=af[i];
				continue;
			}

			r=r1=(double)af[i].r;
			r2=(double)af2[i].r;
			g=g1=(double)af[i].g;
			g2=(double)af2[i].g;
			b=b1=(double)af[i].b;
			b2=(double)af2[i].b;

			if(m==2 || m==3)
			{
				if(r2>averager)
				{
					f=cf*(r2-averager)+averager;
					r=r1*averager/f;
				} else {
					f=cf*(-r2+averager)+(255.0-averager);
					r=255.0-(255.0-r1)*(255.0-averager)/f;
				}

				if(g2>averageg)
				{
					f=cf*(g2-averageg)+averageg;
					g=g1*averageg/f;
				} else {
					f=cf*(-g2+averageg)+(255.0-averageg);
					g=255.0-(255.0-g1)*(255.0-averageg)/f;
				}

				if(b2>averageb)
				{
					f=cf*(b2-averageb)+averageb;
					b=b1*averageb/f;
				} else {
					f=cf*(-b2+averageb)+(255.0-averageb);
					b=255.0-(255.0-b1)*(255.0-averageb)/f;
				}
			}

			if(m==0 || m==1)
			{
				if(r2>averager)
				{
					if((r1>3) && (r2!=0))
					{
						f=abs(r2-r1)/r2;
						d=cf*(r2-averager)+averager;
						r=(1.0-f)*(r1*averager/d)+f*r;
					}
				} else {
					if((r1>3) && (r2!=0))
					{
						f=abs(r2-r1)/r2;
						d=cf*(-r2+averager)+(255.0-averager);
						r=(1.0-f)*(255.0-(255.0-r1)*(255.0-averager)/d)+f*r;;
					}
				}

				if(g2>averageg)
				{
					if((g1>3) && (g2!=0))
					{
						f=abs(g2-g1)/g2;
						d=cf*(g2-averageg)+averageg;
						g=(1.0-f)*(g1*averageg/d)+f*g;
					}
				} else {
					if((g1>3) && (g2!=0))
					{
						f=abs(g2-g1)/g2;
						d=cf*(-g2+averageg)+(255.0-averageg);
						g=(1.0-f)*(255.0-(255.0-g1)*(255.0-averageg)/d)+f*g;;
					}
				}

				if(b2>averageb)
				{
					if((b1>3) && (b2!=0))
					{
						f=abs(b2-b1)/b2;
						d=cf*(b2-averageb)+averageb;
						b=(1.0-f)*(b1*averageb/d)+f*b;
					}
				} else {
					if((b1>3) && (b2!=0))
					{
						f=abs(b2-b1)/b2;
						d=cf*(-b2+averageb)+(255.0-averageb);
						b=(1.0-f)*(255.0-(255.0-b1)*(255.0-averageb)/d)+f*b;
					}
				}
			}

			if(m!=0 && m!=2)
			{
				f=abs(r-averager)/255.0;
				r2=r*averagergb/averager;
				r=r2*(1.0-f)+r*f;

				f=abs(g-averageg)/255.0;
				g2=g*averagergb/averageg;
				g=g2*(1.0-f)+g*f;

				f=abs(b-averageb)/255.0;
				b2=b*averagergb/averageb;
				b=b2*(1.0-f)+b*f;
			}

			if(r<0.0) r=0.0;
			if(r>254.0) r=254.0;
			out[i].r=(unsigned char)(int)(r+0.5);
			if(g<0.0) g=0.0;
			if(g>254.0) g=254.0;
			out[i].g=(unsigned char)(int)(g+0.5);
			if(b<0.0) b=0.0;
			if(b>254.0) b=254.0;
			out[i].b=(unsigned char)(int)(b+0.5);
		}
	}

	return 0;
}

void CCombineDoc::correct(int m)
{
	BeginThreads
	SetThreads(w,h)

	unsigned int i;
	unsigned int x,startx,endx;
	unsigned int y,starty,endy,Y;

	BeginWaitCursor();

	special_changed=true;

	mode=CORRECT;

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

	PIXEL *out,*af,*af2;
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	out=(PIXEL*)GetBuffer(OUT_BUFFER);
	af=(PIXEL*)GetBuffer(active_frame);
	af2=(PIXEL*)GetBuffer(active_frame2);

	double cf=correction_factor;
	unsigned int n=0,rsum=0,gsum=0,bsum=0;
	double averager,averageg,averageb;
	for(y=starty; y<endy; y++)
	{
		Y=y*w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(depth[i]&fFIXED) continue;
			n++;
			rsum+=(int)(af2[i].r);
			gsum+=(int)(af2[i].g);
			bsum+=(int)(af2[i].b);
		}
	}

	averager=1.0+(double)rsum/(double)n;
	averageg=1.0+(double)gsum/(double)n;
	averageb=1.0+(double)bsum/(double)n;

	for(obj=0; obj<objects; obj++)
	{
		correct_params[obj].index=obj;
		correct_params[obj].step=objects;
		correct_params[obj].startx=startx;
		correct_params[obj].endx=endx;
		correct_params[obj].starty=starty;
		correct_params[obj].endy=endy;
		correct_params[obj].w=w;
		correct_params[obj].depth=depth;
		correct_params[obj].out=out;
		correct_params[obj].af=af;
		correct_params[obj].af2=af2;
		correct_params[obj].m=m;
		correct_params[obj].averager=averager;
		correct_params[obj].averageg=averageg;
		correct_params[obj].averageb=averageb;
		correct_params[obj].cf=cf;
		StartThreads(c1,correct_params)
	}
	EndThreads

	display=OUT_BUFFER;
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(DEPTH_BUFFER);
	FreeBuffer(active_frame);
	FreeBuffer(active_frame2);
	EndWaitCursor();
}

void CCombineDoc::add()
{
	unsigned int i;
	unsigned int x,startx,endx;
	unsigned int y,starty,endy,Y;

	BeginWaitCursor();

	special_changed=true;

	mode=ADD;

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

	PIXEL *out,*af,*af2;
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	out=(PIXEL*)GetBuffer(OUT_BUFFER);
	af=(PIXEL*)GetBuffer(active_frame);
	af2=(PIXEL*)GetBuffer(active_frame2);

	double r,g,b;

	for(y=starty; y<endy; y++)
	{
		Y=y*w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(depth[i]&fSPECIAL_FIXED) continue;
			if((af[i].a!=aCLEAR) && (af2[i].a!=aCLEAR))
			{
				r=(double)(af[i].r)-(add_factor*(double)(af2[i].r));
				g=(double)(af[i].g)-(add_factor*(double)(af2[i].g));
				b=(double)(af[i].b)-(add_factor*(double)(af2[i].b));
				if(r<0.0) r=0.0; if(r>255.0) r=255.0;
				if(g<0.0) g=0.0; if(g>255.0) g=255.0;
				if(b<0.0) b=0.0; if(b>255.0) b=255.0;
				out[i].r=(unsigned char)r;
				out[i].g=(unsigned char)g;
				out[i].b=(unsigned char)b;
				out[i].a=0;
				continue;
			}
			else
			{
				if(af[i].a!=aCLEAR) out[i]=af[i];
				else out[i]=af2[i];
			}
		}
	}
	display=OUT_BUFFER;
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(DEPTH_BUFFER);
	FreeBuffer(active_frame);
	FreeBuffer(active_frame2);
	EndWaitCursor();
}
