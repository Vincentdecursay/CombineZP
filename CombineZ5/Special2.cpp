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
//	if(!start_special()) return;

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
			if((af[i].a|aCLEAR) && (af2[i].a|aCLEAR))
			{
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
			else
			{
				if(af[i].a|aCLEAR) out[i]=af[i];
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

void CCombineDoc::OnSpecialCorrectBrightness() 
{
	if(!start_special()) return;

	mode=CORRECT;
	special_active=active_frame;
	old_mag=mag;
	correct(0.0,0.0);

	RefreshScreen();
}

void CCombineDoc::correct(double bwcontrast,double bwnorm)
{
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

	bool test;
	double r,g,b,c1,c2;
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

	averager=(double)rsum/(double)n;
	averageg=(double)gsum/(double)n;
	averageb=(double)bsum/(double)n;

	for(y=starty; y<endy; y++)
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

			if(bwcontrast>0.0)
			{
				c2=(double)af2[i].r;
				if(c2>127.0)
				{
					c2=correction_factor*(c2-averager)+averager;
					c1=(double)(af[i].r);
					r=c1*averager/c2;
				} else {
					c2=correction_factor*(-c2+averager)+(255.0-averager);
					c1=(double)(255.0-af[i].r);
					r=255.0-c1*(255.0-averager)/c2;
				}
				r=r-averager;
				if(abs(r)<4) r=0;
				r=r*bwcontrast+bwnorm;
				if(r<0.0) r=0.0;
				if(r>255.0) r=255.0;
				out[i].r=(unsigned char)(int)(r+0.5);
				out[i].b=out[i].g=out[i].r;
				continue;
			}

			c2=(double)af2[i].r;
			if(c2>127.0)
			{
				c2=correction_factor*(c2-averager)+averager;
				c1=(double)(af[i].r);
				r=c1*averager/c2;
				if(r<0.0) r=0.0;
				if(r>255.0) r=255.0;
				out[i].r=(unsigned char)(int)(r+0.5);
			} else {
				c2=correction_factor*(-c2+averager)+(255.0-averager);
				c1=(double)(255.0-af[i].r);
				r=c1*(255.0-averager)/c2;
				if(r<0.0) r=0.0;
				if(r>255.0) r=255.0;
				out[i].r=(unsigned char)(int)(255.0-r+0.5);
			}

			c2=(double)af2[i].g;
			if(c2>127.0)
			{
				c2=correction_factor*(c2-averageg)+averageg;
				c1=(double)(af[i].g);
				r=(c1*averageg/c2);
				if(r<0.0) r=0.0;
				if(r>255.0) r=255.0;
				out[i].g=(unsigned char)(int)(r+0.5);
			} else {
				c2=correction_factor*(-c2+averageg)+(255.0-averageg);
				c1=(double)(255.0-af[i].g);
				r=c1*(255.0-averageg)/c2;
				if(r<0.0) r=0.0;
				if(r>255.0) r=255.0;
				out[i].g=(unsigned char)(int)(255.0-r+0.5);
			}

			c2=(double)af2[i].b;
			if(c2>127.0)
			{
				c2=correction_factor*(c2-averageb)+averageb;
				c1=(double)(af[i].b);
				r=(c1*averageb/c2);
				if(r<0.0) r=0.0;
				if(r>255.0) r=255.0;
				out[i].b=(unsigned char)(int)(r+0.5);
			} else {
				c2=correction_factor*(-c2+averageb)+(255.0-averageb);
				c1=(double)(255.0-af[i].b);
				r=(c1*(255.0-averageb)/c2);
				if(r<0.0) r=0.0;
				if(r>255.0) r=255.0;
				out[i].b=(unsigned char)(int)(255.0-r+0.5);
			}
		}
	}
finish:
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
			if((af[i].a|aCLEAR) && (af2[i].a|aCLEAR))
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
				if(af[i].a|aCLEAR) out[i]=af[i];
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
