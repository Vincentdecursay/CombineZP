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

/* PLOTTING AND FIXING PIXELS */

bool CCombineDoc::brushparams()
{
	if(!top_frame) return false;
	CPaintDia dd;
	dd.m_delta=brush_delta;
	dd.m_size=brush_size;
	int button;
	do{
		button=dd.DoModal();
		if(button==IDCANCEL) return false;
	} while((dd.m_delta<0) || (dd.m_delta>255) || (dd.m_size<0) || (dd.m_size>25));
	brush_delta=dd.m_delta;
	brush_size=dd.m_size;
	return true;
}

void CCombineDoc::plotdots(int x,int y)
{
	if((display==OUT_BUFFER) && ((mode&GROUP_MASK)!=SPECIAL))
	{
		outwarn();
		return;
	}

	if((x<0) || (y<0) || (x>=w) || (y>=h)) return;

	if(!top_frame) return;
	if(full_mode) return;

	x+=mag.left; y+=mag.top;

	int i=x+y*w,j;
	int xx,yy;

	if(i>w*h-1) return;

	PIXEL *f0=(PIXEL*)GetBuffer(active_frame);
	if(f0[i].a==aCLEAR && !((mode&GROUP_MASK)==SPECIAL))
	{
		FreeBuffer(active_frame);
		return;
	}

	int pBUFFER;
	PIXEL *picture,*f1;
	if((mode&GROUP_MASK)==SPECIAL) pBUFFER=OUT_BUFFER;
	else pBUFFER=PICTURE_BUFFER;
	picture=(PIXEL*)GetBuffer(pBUFFER);
	PIXEL *backup_picture=(PIXEL*)GetBuffer(BACKUP_PICTURE_BUFFER);
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	if(fix_active || ((mode&GROUP_MASK)==SPECIAL))
	{
//		if(plane_mode && (((depth[i]&dMASK)>>8)!=active_frame) && ((mode&GROUP_MASK)!=SPECIAL))
		if(plane_mode && ((((depth[i]&dMASK)+(1<<7))>>8)!=active_frame) && ((mode&GROUP_MASK)!=SPECIAL))
		{
			FreeBuffer(active_frame);
			FreeBuffer(pBUFFER);
			FreeBuffer(DEPTH_BUFFER);
			return;
		}

		for(xx=x-brush_size; xx<x+brush_size+1; xx++)
		{
			for(yy=y-brush_size; yy<y+brush_size+1; yy++)
			{
				if(xx<mag.left) continue;
				if(yy<mag.top) continue;
				if(xx>mag.right) continue;
				if(yy>mag.bottom) continue;

				if(xx<0) continue;
				if(yy<0) continue;
				if(xx>w-1) continue;
				if(yy>h-1) continue;

				j=xx+yy*w;

				f1=(PIXEL*)GetBuffer(active_frame);
				if(f1[j].r+brush_delta<f1[i].r) continue;
				if(f1[j].r>f1[i].r+brush_delta) continue;
				if(f1[j].g+brush_delta<f1[i].g) continue;
				if(f1[j].g>f1[i].g+brush_delta) continue;
				if(f1[j].b+brush_delta<f1[i].b) continue;
				if(f1[j].b>f1[i].b+brush_delta) continue;
				FreeBuffer(active_frame);
			
				if(depth[j]&fFILLED)
				{
					depth[j]|=(fFIXED);
					if(show_fixed)
						backup_picture[j].abgr=picture[j].abgr=((~wipeout)&0xffffff);
				}
			}
		}
		UpdateBuffer(DEPTH_BUFFER);
		UpdateBuffer(pBUFFER);
		UpdateBuffer(BACKUP_PICTURE_BUFFER);
		FreeBuffer(active_frame);
		RefreshScreen();
		return;
	}

	int rtemp,gtemp,btemp;

	int brushs=brush_size;
	PIXEL p,q;
	p=f0[x+y*w];
	if((display==PICTURE_BUFFER) && (depth[i]&fFILLED) && !depth_flag) p=picture[i];

	for(xx=x-brush_size; xx<x+(int)brush_size+1; xx++)
		for(yy=y-brush_size; yy<y+(int)brush_size+1; yy++)
	{
		if(xx<mag.left) continue;
		if(yy<mag.top) continue;
		if(xx>mag.right) continue;
		if(yy>mag.bottom) continue;

		i=xx+yy*w;
		if(depth[i]&fFIXED) continue;

		if(xx!=x || yy!=y)
		{
			q=f0[i];

			if(p.r>q.r) rtemp=p.r-q.r; else rtemp=q.r-p.r;
			if(p.g>q.g) gtemp=p.g-q.g; else gtemp=q.g-p.g;
			if(p.b>q.b) btemp=p.b-q.b; else btemp=q.b-p.b;

			if(rtemp>brush_delta) continue;
			if(gtemp>brush_delta) continue;
			if(btemp>brush_delta) continue;
		}

		backup_picture[i]=f0[i];
		backup_picture[i].a=aGOOD;
		if(depth_flag)
		{
			picture[i]=rainbow[active_frame<<8];
			picture[i].a=aGOOD;
		} else {
			picture[i]=f0[i];
			picture[i].a=aGOOD;
		}
		depth[i]=(fFILLED|(active_frame<<8));
	}
	FreeBuffer(active_frame);
	UpdateBuffer(DEPTH_BUFFER);
	UpdateBuffer(PICTURE_BUFFER);
	UpdateBuffer(BACKUP_PICTURE_BUFFER);

	CRect r;
/*	r.left=x-brush_size;
	r.right=x+brush_size;
	r.top=y-brush_size;
	r.bottom=y+brush_size;
*/	r.left=mag.left;
	r.right=mag.right;
	r.top=y-brush_size;
	r.bottom=y+brush_size;
	r=mag;
	Redraw(&r,false);
//	RefreshScreen();
}

void CCombineDoc::unplotdots(int x, int y)
{
	if((display==OUT_BUFFER) && ((mode&GROUP_MASK)!=SPECIAL))
	{
		outwarn();
		return;
	}

	if(!top_frame) return;
	if(full_mode) return;
	int i=x+mag.left+(y+mag.top)*w,j;
	int xx,yy;

	int pBUFFER;
	PIXEL *f0=(PIXEL*)GetBuffer(active_frame),*f1,*f2;
	if((mode&GROUP_MASK)==SPECIAL) pBUFFER=OUT_BUFFER;
	else pBUFFER=PICTURE_BUFFER;
	f1=(PIXEL*)GetBuffer(pBUFFER);
	PIXEL *backup_picture=(PIXEL*)GetBuffer(BACKUP_PICTURE_BUFFER);
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	if(fix_active || ((mode&GROUP_MASK)==SPECIAL))
	{
//		if(plane_mode && (((depth[i]&dMASK)>>8)!=active_frame)) return;
		if(plane_mode && ((((depth[i]&dMASK)+(1<<7))>>8)!=active_frame)) return;

		for(xx=x+mag.left-brush_size; xx<x+mag.left+(int)brush_size+1; xx++)
		{
			for(yy=y+mag.top-brush_size; yy<y+mag.top+(int)brush_size+1; yy++)
			{
				if(xx<mag.left) continue;
				if(yy<mag.top) continue;
				if(xx>mag.right) continue;
				if(yy>mag.bottom) continue;

				if(xx<0) continue;
				if(yy<0) continue;
				if(xx>(int)w-1) continue;
				if(yy>(int)h-1) continue;

				j=xx+yy*w;

				if(f0[j].r+brush_delta<f0[i].r) continue;
				if(f0[j].r>f0[i].r+brush_delta) continue;
				if(f0[j].g+brush_delta<f0[i].g) continue;
				if(f0[j].g>f0[i].g+brush_delta) continue;
				if(f0[j].b+brush_delta<f0[i].b) continue;
				if(f0[j].b>f0[i].b+brush_delta) continue;

				if((depth[j]&fFIXED))
				{
					depth[j]-=fFIXED;

					f2=(PIXEL*)GetBuffer(((depth[j]&dMASK)+(1<<7))>>8);
					backup_picture[j].abgr=f2[j].abgr;
					if(!depth_flag) f1[j].abgr=f2[j].abgr;
					else f1[j]=rainbow[depth[j]&dMASK];
					FreeBuffer(((depth[j]&dMASK)+(1<<7))>>8);
				}
			}
		}
		if(magon && !full_mode) Redraw(&mag,false);
		UpdateBuffer(pBUFFER);
		UpdateBuffer(BACKUP_PICTURE_BUFFER);
		UpdateBuffer(DEPTH_BUFFER);
		FreeBuffer(active_frame);
		return;
	}

	int r,g,b;
	PIXEL p,q;

	if(x<0 || y<0) goto finish;
	if(x+mag.left>=(int)w) goto finish;
	if(y+mag.top>=(int)h) goto finish;

	if(depth[x+mag.left+(y+mag.top)*w]&fFIXED) goto finish;

	if(depth_flag) j=PICTURE_BUFFER; // so you can clear patches from a plain easily

	p=f0[x+mag.left+(y+mag.top)*w];

	for(xx=x+mag.left-(int)brush_size; xx<x+mag.left+(int)brush_size+1; xx++)
	{
		for(yy=y+mag.top-(int)brush_size; yy<y+mag.top+(int)brush_size+1; yy++)
		{
			if(xx<mag.left) continue;
			if(yy<mag.top) continue;
			if(xx>mag.right) continue;
			if(yy>mag.bottom) continue;

			if(xx<0) continue;
			if(yy<0) continue;
			if(xx>(int)w-1) continue;
			if(yy>(int)h-1) continue;

			i=xx+yy*w;

			if(depth[i]&fFIXED) continue;

			q=f0[i];

			if(p.r>q.r) r=p.r-q.r; else r=q.r-p.r;
			if(r>brush_delta) continue;
			if(p.g>q.g) g=p.g-q.g; else g=q.g-p.g;
			if(g>brush_delta) continue;
			if(p.b>q.b) b=p.b-q.b; else b=q.b-p.b;
			if(b>brush_delta) continue;
			f1[i].abgr=wipeout;
			depth[i]=fEMPTY;
			backup_picture[i].abgr=wipeout;
		}
	}
finish:
	UpdateBuffer(DEPTH_BUFFER);
	UpdateBuffer(BACKUP_PICTURE_BUFFER);
	UpdateBuffer(pBUFFER);
	FreeBuffer(active_frame);
	RefreshScreen();
}

void CCombineDoc::plotdot(int x, int y)
{
	if((display==OUT_BUFFER) && ((mode&GROUP_MASK)!=SPECIAL))
	{
		outwarn();
		return;
	}

	if(!top_frame) return;
	if(full_mode) return;
	unsigned int i;

	if(x<0 || y<0) return;
	if(x+mag.left>=(int)w) return;
	if(y+mag.top>=(int)h) return;

	i=x+mag.left+(y+mag.top)*w;

	int pBUFFER;
	PIXEL *f0=(PIXEL*)GetBuffer(active_frame),*f1;
	if((mode&GROUP_MASK)==SPECIAL) pBUFFER=OUT_BUFFER;
	else pBUFFER=PICTURE_BUFFER;
	f1=(PIXEL*)GetBuffer(pBUFFER);
	PIXEL *backup_picture=(PIXEL*)GetBuffer(BACKUP_PICTURE_BUFFER);
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	if((mode&GROUP_MASK)==SPECIAL)
	{
		depth[i]|=fFIXED;
		if(magon && !full_mode) Redraw(&mag,false);
		FreeBuffer(pBUFFER);
		FreeBuffer(active_frame);
		UpdateBuffer(DEPTH_BUFFER);
		return;
	}

	if(fix_active)
	{
//		if(depth[i]&fFILLED && !(plane_mode && (((depth[i]&dMASK)>>8)!=active_frame)))
		if(depth[i]&fFILLED && !(plane_mode && ((((depth[i]&dMASK)+(1<<7))>>8)!=active_frame)))
		{
			depth[i]|=fFIXED;
			if(show_fixed && !((mode&GROUP_MASK)==SPECIAL))
				backup_picture[i].abgr=f1[i].abgr=((~wipeout)&0xffffff);
		}
		UpdateBuffer(pBUFFER);
		UpdateBuffer(DEPTH_BUFFER);
		UpdateBuffer(BACKUP_PICTURE_BUFFER);
		FreeBuffer(active_frame);
		return;
	}

	if(f0[i].a==aCLEAR) goto finish;

	if(!(depth[i]&fFIXED))
	{
		backup_picture[i]=f0[i];
		if(depth_flag) f1[i]=rainbow[active_frame<<8];
		else f1[i]=f0[i];
		depth[i]=(fFILLED|(active_frame<<8));
	}
finish:
	UpdateBuffer(DEPTH_BUFFER);
	UpdateBuffer(BACKUP_PICTURE_BUFFER);
	UpdateBuffer(pBUFFER);
	FreeBuffer(active_frame);
}

void CCombineDoc::unplotdot(int x, int y)
{
	if((display==OUT_BUFFER) && ((mode&GROUP_MASK)!=SPECIAL))
	{
		outwarn();
		return;
	}

	if(!top_frame) return;
	if(full_mode) return;
	unsigned int i;

	if(x<0 || y<0) return;
	if(x+mag.left>=(int)w) return;
	if(y+mag.top>=(int)h) return;

	i=x+mag.left+(y+mag.top)*w;

	int pBUFFER;
	PIXEL *f0=(PIXEL*)GetBuffer(active_frame),*f1,*f2;
	if(mode&SPECIAL) pBUFFER=OUT_BUFFER;
	else pBUFFER=PICTURE_BUFFER;
	f1=(PIXEL*)GetBuffer(pBUFFER);
	PIXEL *backup_picture=(PIXEL*)GetBuffer(BACKUP_PICTURE_BUFFER);
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	if((mode&GROUP_MASK)==SPECIAL)
	{
		if(depth[i]&fFIXED) depth[i]-=fFIXED;
		if(magon && !full_mode) Redraw(&mag,false);
		FreeBuffer(active_frame);
		FreeBuffer(pBUFFER);
		UpdateBuffer(DEPTH_BUFFER);
		return;
	}

	if(fix_active)
	{
//		if(!(plane_mode && (((depth[i]&dMASK)>>8)!=active_frame)) && (depth[i]&fFIXED))
		if(!(plane_mode && ((((depth[i]&dMASK)+(1<<7))>>8)!=active_frame)) && (depth[i]&fFIXED))
		{
			depth[i]-=(fFIXED);
			f2=(PIXEL*)GetBuffer(((depth[i]&dMASK)+(1<<7))>>8);
			backup_picture[i].abgr=f2[i].abgr;
			if(!depth_flag) f1[i].abgr=f2[i].abgr;
			else f1[i]=rainbow[depth[i]&dMASK];
			FreeBuffer(((depth[i]&dMASK)+(1<<7))>>8);
		}
		UpdateBuffer(DEPTH_BUFFER);
		UpdateBuffer(BACKUP_PICTURE_BUFFER);
		UpdateBuffer(pBUFFER);
		FreeBuffer(active_frame);
		return;
	}

	backup_picture[i].abgr=wipeout;
	if(!(depth[i]&fFIXED))
	{
		f1[i].abgr=wipeout;
		depth[i]=fEMPTY;
	}

	UpdateBuffer(DEPTH_BUFFER);
	UpdateBuffer(BACKUP_PICTURE_BUFFER);
	UpdateBuffer(pBUFFER);
	FreeBuffer(active_frame);
}
