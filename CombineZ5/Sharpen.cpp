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
/*
void CCombineDoc::OnMacroSharpenall() 
{
	if((mode&GROUP_MASK)==SPECIAL) return;
	CSharpenDia dd;
	dd.m_n=(int)(sharp_factor*100.0);
	int button;
	button=dd.DoModal();
	if(button==IDCANCEL) return;
	sharp_factor=(float)dd.m_n/100.0;
	sharpensome();
}

void CCombineDoc::sharpensome()
{
	if(fix_active) flash_fixed();
	int i;

	for(i=0; i<top_frame; i++)
	{
		active_frame=i;
		sharpen();
		OnFrameReplace();
	}

	redraw_picture();

	display=1;
	RefreshScreen();
}
*/
void CCombineDoc::sharpen()
{
	if(!top_frame) return;
	
	int i,j,k,K,l,times;

	special_changed=true;

	int SHWH; SHWH=2*(w*h)/(640*480);
	if(SHWH<2) SHWH=2;

	int x,startx,endx;
	int y,starty,endy,Y;

	int r,g,b;
	int r0,g0,b0;
	int m;

	BeginWaitCursor();

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

	PIXEL* temp=(PIXEL*)GetBuffer(TEMP_BUFFER);
	int* depth=(int*)GetBuffer(DEPTH_BUFFER);
	PIXEL* f1=(PIXEL*)GetBuffer(PICTURE_BUFFER);
	PIXEL* f0=(PIXEL*)GetBuffer(active_frame);

	for(times=0; times<2; times++)
	{
		if(times!=0)
		{
			f0=temp;
			for(i=0; i<w*h; i++) f0[i]=f1[i];
		}

		for(y=starty; y<endy; y++)
		{
			Y=y*w;
			for(x=startx; x<endx; x++)
			{
				i=x+Y;
				if(depth[i]&fSPECIAL_FIXED)
				{
					f1[i]=f0[i];
					continue;
				}
				m=0;
				r=g=b=0;
				for(k=-SHWH; k<SHWH+1; k+=1)
				{
					if(y+k>=h) continue;
					if(y+k<0) continue;
					K=k*w;
					for(j=-SHWH; j<SHWH+1; j+=1)
					{
						if(x+j>=w) continue;
						if(x+j<0) continue;
						l=i+j+K;
						r+=(int)f0[l].r;
						g+=(int)f0[l].g;
						b+=(int)f0[l].b;
						m++;
					}
				}
				r0=(int)f0[i].r;
				g0=(int)f0[i].g;
				b0=(int)f0[i].b;
				double sf=sharp_factor/3.0;
				if(m)
				{
					double rm=1.0/(double)m;

					r0+=(int)(((double)r0-(double)r*rm)*sf);
					if(r0<0) r0=0;
					if(r0>255) r0=255;

					g0+=(int)(((double)g0-(double)g*rm)*sf);
					if(g0<0) g0=0;
					if(g0>255) g0=255;	

					b0+=(int)(((double)b0-(double)b*rm)*sf);
					if(b0<0) b0=0;
						if(b0>255) b0=255;
				}
				f0[i].r=(unsigned char)r0;
				f0[i].g=(unsigned char)g0;
				f0[i].b=(unsigned char)b0;
			}
		}
	}
	UpdateBuffer(PICTURE_BUFFER);
	FreeBuffer(active_frame);
	FreeBuffer(DEPTH_BUFFER);
	display=0;
	EndWaitCursor();
	RefreshScreen();
	FreeBuffer(TEMP_BUFFER);
}

void CCombineDoc::OnFrameSharpen() 
{
	if(!start_special()) return;
	sharp_factor=0.25;
	mode=SHARP;
	special_active=active_frame;
	old_mag=mag;
	sharpen();
	RefreshScreen();
}

/*void CCombineDoc::OnSpecialSharpenz() 
{
	sharpz_factor=0.25;

	mode=SHARPZ;
	special_active=active_frame;
	old_mag=mag;
	sharpenz();
	if(magon && !full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

void CCombineDoc::sharpenz()
{
	if(!top_frame) return;
	
	int i,j,k;

	unsigned int x,startx,endx;
	unsigned int y,starty,endy,Y;

	special_changed=true;

	int r,g,b;
	int r0,g0,b0;
	int m;

	BeginWaitCursor();

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

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	PIXEL *f1=(PIXEL*)GetBuffer(PICTURE_BUFFER),*f0,*f2;

	for(y=starty; y<endy; y++)
	{
		Y=w*y;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(depth[i]&fSPECIAL_FIXED) continue;
			m=0;
			f0=(PIXEL*)GetBuffer(depth[i]&dMASK);
			r0=f0[i].r;
			g0=f0[i].g;
			b0=f0[i].b;
			FreeBuffer(depth[i]&dMASK);
			r=g=b=0;
			j=depth[i];
			for(k=j-2; k<j+3; k++)
			{
				if(k<0 || k>top_frame-1) continue;
				if(k==j) continue;
//				if(hide[k]) continue;

				f2=(PIXEL*)GetBuffer(k);
				r+=(int)f2[i].r;
				g+=(int)f2[i].g;
				b+=(int)f2[i].b;
				m++;
				FreeBuffer(k);
			}
			if(m)
			{
				r0+=(int)((((double)r0)-(((double)r)/((double)m)))*sharpz_factor);
				if(r0<0) r0=0;
				if(r0>255) r0=255;

				g0+=(int)((((double)g0)-(((double)g)/((double)m)))*sharpz_factor);
				if(g0<0) g0=0;
				if(g0>255) g0=255;

				b0+=(int)((((double)b0)-(((double)b)/((double)m)))*sharpz_factor);
				if(b0<0) b0=0;
				if(b0>255) b0=255;

				f1[i].r=(unsigned char)r0;
				f1[i].g=(unsigned char)g0;
				f1[i].b=(unsigned char)b0;
			}
		}
	}
	UpdateBuffer(PICTURE_BUFFER);
	FreeBuffer(DEPTH_BUFFER);
	display=0;
	EndWaitCursor();
}
*/