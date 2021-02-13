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
#include "Maxangle.h"

// MOVE FRAME (or sometimes picture) UP DOWN LEFT & RIGHT

struct move_Params
{
	int index,step;
	int w,h;
	PIXEL *inout;
} move_params[32];

void CCombineDoc::up()
{
	// move for 3d etc.
	if((mode&GROUP_MASK)==SPECIAL)
	{
		if(mode==ADD || mode==BLEND || mode==CORRECT);
		else return;
	}

	if(plane_mode) return;

	// only move picture for 3d
	if((display==PICTURE_BUFFER) || ((display==OUT_BUFFER) && !((mode&GROUP_MASK)==SPECIAL)))
	{
		CString s1,s2;
		s1.LoadString(IDS_MOVEFRAME1); s2.LoadString(IDS_MOVEFRAME2);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		return;
	}

	offsety--;

	int i,j;
	unsigned int a;

	PIXEL* p=(PIXEL*)GetBuffer(active_frame);
	PIXEL* temp=(PIXEL*)GetBuffer(TEMP_BUFFER);

	for(i=0,j=0; j<w; i++,j++)
		temp[j]=p[i]; // save top row

	int x,y,Yi,Yj;
	// move main body
	for(y=0; y<h-1; y++)
	{
		Yi=y*w;
		Yj=(y+1)*w;
		for(x=0; x<w; x++)
		{
			i=x+Yi;
			j=x+Yj;
			p[i]=p[j];
		}
	}

	// old top to new bottom
	for(i=w*(h-1)+w-1, j=w-1; j>=0; i--, j--)
	{
		a=(unsigned int)temp[j].a; // change dirty/clean state
		temp[j].a=(unsigned char)(a&~aVERTICAL|~a&aVERTICAL);

		p[i]=temp[j]; // bottom row=old top row
	}

	FreeBuffer(TEMP_BUFFER);

	if((mode&GROUP_MASK)==SPECIAL) special_rework(); // for 3d etc.

	UpdateBuffer(active_frame);
}


void CCombineDoc::down()
{
	// comments similar to up()
	if((mode&GROUP_MASK)==SPECIAL)
	{
		if(mode==ADD || mode==BLEND || mode==CORRECT);
		else return;
	}

	if(plane_mode) return;

	if((display==PICTURE_BUFFER) || ((display==OUT_BUFFER) && !((mode&GROUP_MASK)==SPECIAL)))
	{
		CString s1,s2;
		s1.LoadString(IDS_MOVEFRAME1); s2.LoadString(IDS_MOVEFRAME2);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		return;
	}

	offsety++;

	int i,j;
	unsigned int a;

	PIXEL* p=(PIXEL*)GetBuffer(active_frame);
	PIXEL* temp=(PIXEL*)GetBuffer(TEMP_BUFFER);

	for(i=w*(h-1)+w-1, j=w-1; j>=0; i--, j--) temp[j]=p[i];

	int x,y,Yi,Yj;
	for(y=h-1; y>0; y--)
	{
		Yi=y*w;
		Yj=(y-1)*w;
		for(x=0; x<w; x++)
		{
			i=x+Yi;
			j=x+Yj;
			p[i]=p[j];
		}
	}
	for(i=0,j=0; j<w; i++,j++)
	{
		a=(unsigned int)temp[i].a; // change dirty/clean state
		temp[j].a=(unsigned char)((a&~aVERTICAL|~a&aVERTICAL)|aGOOD);

		p[i]=temp[j];
	}

	FreeBuffer(TEMP_BUFFER);

	if((mode&GROUP_MASK)==SPECIAL) special_rework();

	UpdateBuffer(active_frame);
}

UINT CCombineDoc::move_right(LPVOID param)
{
	move_Params *pp=(move_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	PIXEL *inout=pp->inout;

	int i,j,x,y,Yi;

	for(y=index; y<h; y+=step)
	{
		Yi=y*w;
		for(x=w-1; x>0; x--)
		{
			i=x+Yi;
			j=x-1+Yi;
			inout[i]=inout[j];
		}
	}

	return 0;
}

void CCombineDoc::right()
{
	// comments similar to up()
	if((mode&GROUP_MASK)==SPECIAL)
	{
		if(mode==ADD || mode==BLEND || mode==CORRECT);
		else return;
	}

	if(plane_mode) return;

	if((display==PICTURE_BUFFER) || ((display==OUT_BUFFER) && !((mode&GROUP_MASK)==SPECIAL)))
	{
		CString s1,s2;
		s1.LoadString(IDS_MOVEFRAME1); s2.LoadString(IDS_MOVEFRAME2);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		return;
	}

	offsetx++;

	int i,j;
	unsigned int a;

	PIXEL* p=(PIXEL*)GetBuffer(active_frame);
	PIXEL* temp=(PIXEL*)GetBuffer(TEMP_BUFFER);

	for(i=w-1, j=0; j<h; i+=w, j++) temp[j]=p[i];

	BeginThreads
	SetThreads(w,h)
	for(obj=0; obj<objects; obj++)
	{
		move_params[obj].index=obj;
		move_params[obj].step=objects;
		move_params[obj].w=w;
		move_params[obj].h=h;
		move_params[obj].inout=p;
		StartThreads(move_right,move_params);
	}
	EndThreads

	for(i=0, j=0; j<h; i+=w, j++)
	{
		a=(unsigned int)temp[j].a; // change dirty/clean state
		temp[j].a=(unsigned char)(a&~aHORIZONTAL|~a&aHORIZONTAL);
		p[i]=temp[j];
	}

	FreeBuffer(TEMP_BUFFER);

	if((mode&GROUP_MASK)==SPECIAL) special_rework();

	UpdateBuffer(active_frame);
}

UINT CCombineDoc::move_left(LPVOID param)
{
	move_Params *pp=(move_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	PIXEL *inout=pp->inout;

	int i,j,x,y,Yi;

	for(y=index; y<h; y+=step)
	{
		Yi=y*w;
		for(x=0; x<w-1; x++)
		{
			i=x+Yi;
			j=x+1+Yi;
			inout[i]=inout[j];
		}
	}

	return 0;
}
void CCombineDoc::left()
{
	// comments similar to up()
	if((mode&GROUP_MASK)==SPECIAL)
	{
		if(mode==ADD || mode==BLEND || mode==CORRECT);
		else return;
	}

	if(plane_mode) return;

	if((display==PICTURE_BUFFER) || ((display==OUT_BUFFER) && !((mode&GROUP_MASK)==SPECIAL)))
	{
		CString s1,s2;
		s1.LoadString(IDS_MOVEFRAME1); s2.LoadString(IDS_MOVEFRAME2);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		return;
	}

	offsetx--;

	int i,j;
	unsigned int a;

	PIXEL* p=(PIXEL*)GetBuffer(active_frame);
	PIXEL* temp=(PIXEL*)GetBuffer(TEMP_BUFFER);

	for(i=0, j=0; j<h; i+=w, j++) temp[j]=p[i];

	BeginThreads
	SetThreads(w,h)
	for(obj=0; obj<objects; obj++)
	{
		move_params[obj].index=obj;
		move_params[obj].step=objects;
		move_params[obj].w=w;
		move_params[obj].h=h;
		move_params[obj].inout=p;
		StartThreads(move_left,move_params)
	}
	EndThreads

	for(i=w-1, j=0; j<h; i+=w, j++)
	{
		a=(unsigned int)temp[j].a; // change dirty/clean state
		temp[j].a=(unsigned char)(a&~aHORIZONTAL|~a&aHORIZONTAL);
		p[i]=temp[j];
	}

	FreeBuffer(TEMP_BUFFER);

	if((mode&GROUP_MASK)==SPECIAL) special_rework();

	UpdateBuffer(active_frame);
}

// ALIGN

void CCombineDoc::OnStackSizeandalignmentAutomaticregularstack() // 2 pass
{
	if(fix_active) flash_fixed();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	progress.Maximize();
	refreshscreen=true;
	AfxBeginThread(Autoalignandsize2,this);
}

void CCombineDoc::OnFrameAutoalignandsize() 
{
	if(fix_active) flash_fixed();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	progress.Maximize();
	refreshscreen=true;

	AfxBeginThread(Autoalignandsize,this);
}

struct tss_Params
{
	__int64 result;
	bool bandw;
	int starty,endy,ystep;
	int startx,endx,xstep;
	int width,height;
	int xoffset,yoffset;
	int cx,cy,r2;
	float scale;
	PIXEL *sample,*master;
	int *xxtable;
} tss_params[32];

UINT CCombineDoc::tss(LPVOID param)
{
	tss_Params *p=(tss_Params *)param;
	bool bandw=p->bandw;
	int endx=p->endx;
	int endy=p->endy;
	int height=p->height;
	PIXEL *master=p->master;
	__int64 result=p->result;
	PIXEL *sample=p->sample;
	float scale=p->scale;
	int startx=p->startx;
	int starty=p->starty;
	int width=p->width;
	int xoffset=p->xoffset;
	int xstep=p->xstep;
	int yoffset=p->yoffset;
	int ystep=p->ystep;
	int *xxtable=p->xxtable;

	if(xoffset>width/8){p->result=0x1000000000000000; return 1;}
	if(xoffset<-width/8){p->result=0x1000000000000000; return 1;}
	if(yoffset>height/8){p->result=0x1000000000000000; return 1;}
	if(yoffset<-height/8){p->result=0x1000000000000000; return 1;}
	if(scale>1.05){p->result=0x1000000000000000; return 1;}
	if(scale<0.95){p->result=0x1000000000000000; return 1;}

	int x,xx,y,yy,Y,YY;
	unsigned int t;
	PIXEL s,m;
	result=0;

	if(bandw)
	{
		for(y=starty; y<endy; y+=ystep)
		{
			Y=width*y;
			yy=(int)(((float)(y-yoffset)-(float)(height/2))*scale+(float)(height/2)+0.5);
			if(yy<height/16) yy=height/16;
			if(yy>(15*height)/16) yy=(15*height)/16;
			YY=yy*width;
			for(x=startx; x<endx; x+=xstep)
			{
				xx=xxtable[x];

				s=sample[xx+YY];
				m=master[x+Y];

				t=(int)((unsigned int)s.r)-(int)((unsigned int)m.r); result+=(__int64)((t*t)/16);
			}
		}
	} else {
		for(y=starty; y<endy; y+=ystep)
		{
			Y=width*y;
			yy=(int)(((float)(y-yoffset)-(float)(height/2))*scale+(float)(height/2)+0.5);
			if(yy<height/16) yy=height/16;
			if(yy>(15*height)/16) yy=(15*height)/16;
			YY=yy*width;
			for(x=startx; x<endx; x+=xstep)
			{
				xx=xxtable[x];
				s=sample[xx+YY];
				m=master[x+Y];

				t=(int)((unsigned int)s.r)-(int)((unsigned int)m.r); result+=(__int64)((t*t)/16);
				t=(int)((unsigned int)s.g)-(int)((unsigned int)m.g); result+=(__int64)((t*t)/16);
				t=(int)((unsigned int)s.b)-(int)((unsigned int)m.b); result+=(__int64)((t*t)/16);
			}
		}
	}
	p->result=result;
	return 0;
}

__int64 CCombineDoc::testshiftnscale(PIXEL *sample,PIXEL *master,int width,int height,int xoffset,int yoffset,float scale)
{
	__int64 result=0;
	int startx,endx,starty,endy;
	int xstep,ystep;

	xstep=width/1000; if(xstep<2) xstep=2;
	ystep=height/1000; if(ystep<2) ystep=2;

	startx=width/32;
	endx=width-startx;
	starty=height/32;
	endy=height-starty;

	scale=1.0f/scale;

	int *xxtable=new int[width],i,xx;
	for(i=0; i<width; i++)
	{
		xx=(int)(((float)(i-xoffset)-(float)(width/2))*scale+(float)(width/2)+0.5);
		if(xx<endx/16) xx=endx/16;
		if(xx>(15*endx)/16) xx=(15*endx)/16;
		xxtable[i]=xx;
	}

	BeginThreads
	SetThreads(width,height)
	for(obj=0; obj<objects; obj++)
	{
		tss_params[obj].bandw=bandw;
		tss_params[obj].starty=starty+ystep*obj;
		tss_params[obj].endy=endy;
		tss_params[obj].ystep=ystep*objects;
		tss_params[obj].startx=startx;
		tss_params[obj].endx=endx;
		tss_params[obj].xstep=xstep;
		tss_params[obj].width=width;
		tss_params[obj].height=height;
		tss_params[obj].xoffset=xoffset;
		tss_params[obj].yoffset=yoffset;
		tss_params[obj].scale=scale;
		tss_params[obj].sample=sample;
		tss_params[obj].master=master;
		tss_params[obj].result=0;
		tss_params[obj].xxtable=xxtable;
		StartThreads(tss,tss_params)
	}
	EndThreads
	for(obj=0; obj<objects; obj++)
		result+=tss_params[obj].result;

	delete [] xxtable;

	return result;
}

struct split_Params
{
	int step,index;
	bool bandw;
	int w,h;
	PIXEL *in,*out;
} split_params[32];

UINT CCombineDoc::split(LPVOID param)
{
	split_Params *pp=(split_Params *)param;
	int index=pp->index;
	int step=pp->step;
	bool bandw=pp->bandw;
	PIXEL *in=pp->in;
	int h=pp->h;
	PIXEL *out=pp->out;
	int w=pp->w;

	int min_half_size=MIN_HALF_SIZE;
	int i,j,x,y;
	PIXEL p,p0,p1,p2,p3;

	if(bandw)
	{
		for(y=index; y<h; y+=step)
		{
			for(x=0; x<w; x++)
			{
				i=x+y*w;
				j=2*i;
				p.abgr=0;
				p0=in[j];
				p1=in[j+1];
				p2=in[j+w];
				p3=in[j+1+w];

				p.r=(unsigned char)(((int)(p0.r)+(int)(p1.r)+(int)(p2.r)+(int)(p3.r)+2)/4);
				out[i]=p;
			}
		}
	} else {
		for(y=index; y<h; y+=step)
		{
			for(x=0; x<w; x++)
			{
				i=x+y*w;
				j=2*i;
				p.abgr=0;
				p0=in[j];
				p1=in[j+1];
				p2=in[j+w];
				p3=in[j+1+w];

				p.r=(unsigned char)(((int)((unsigned int)p0.r)+(int)((unsigned int)p1.r)+(int)((unsigned int)p2.r)+(int)((unsigned int)p3.r)+2)/4);
				p.g=(unsigned char)(((int)((unsigned int)p0.g)+(int)((unsigned int)p1.g)+(int)((unsigned int)p2.g)+(int)((unsigned int)p3.g)+2)/4);
				p.b=(unsigned char)(((int)((unsigned int)p0.b)+(int)((unsigned int)p1.b)+(int)((unsigned int)p2.b)+(int)((unsigned int)p3.b)+2)/4);
				out[i]=p;
			}
		}
	}
	return 0;
}

struct sax_Params
{
	unsigned int index,step;
	int w,h;
	int yoff;
	bool bandw;
	int frame_border;
	int smallw,smallh;
	int *xxtable,*dxxtable;
	bool *xxouttable;
	PIXEL *in,*out;
	RECT goodrect;
	float scale;
} sax_params[32];

UINT CCombineDoc::sax(LPVOID param)
{
	sax_Params *pp=(sax_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int yy;
	int dyy;
	int yoff=pp->yoff;
	int y,Y;
	bool bandw=pp->bandw;
	int frame_border=pp->frame_border;
	int smallw=pp->smallw;
	int smallh=pp->smallh;
	int w=pp->w;
	int h=pp->h;
	int *xxtable=pp->xxtable;
	int *dxxtable=pp->dxxtable;
	bool *xxouttable=pp->xxouttable;
	bool yyout;
	PIXEL *in=pp->in;
	PIXEL *out=pp->out;
	RECT goodrect=pp->goodrect;
	float scale=pp->scale;

	int x,xx,dxx;
	PIXEL p;
	int c00,c01,c10,c11;
	int r00,r01,r10,r11;
	int g00,g01,g10,g11;
	int b00,b01,b10,b11;
	int a,r,g,b;
	float yf;

	for(y=frame_border+index; y<frame_border+smallh; y+=step)
	{
		Y=w*y;
		yf=((float)(y-yoff)-(float)(h/2))*scale+(float)(h/2);
		yy=(int)yf;
		yyout=false;
		if(yy<frame_border){
			if(y>goodrect.top) goodrect.top=y;
			yy=frame_border;
			yyout=true;
			yf=(float)yy;
			if(yy>goodrect.top) goodrect.top=yy;
		}
		if(yy>frame_border+smallh-2)
		{
			if(y<goodrect.bottom) goodrect.bottom=y;
			yy=frame_border+smallh-2;
			yyout=true;
			if(yy<goodrect.bottom) goodrect.bottom=yy;
			yf=(float)yy;
		}
		dyy=(int)((yf-(float)yy)*(float)0x800);
		if(bandw)
		{
			for(x=frame_border; x<frame_border; x++)
			{
				xx=xxtable[x];

				dxx=dxxtable[x];

				a=0;
				if((yyout) || (xxouttable[x])) a=aAMB;

				p=in[(xx+0)+w*(yy+0)];
				r00=(int)p.r; a|=p.a;

				p=in[(xx+1)+w*(yy+0)];
				r10=(int)p.r; a|=p.a;

				p=in[(xx+1)+w*(yy+1)];
				r11=(int)p.r; a|=p.a;

				p=in[(xx+0)+w*(yy+1)];
				r01=(int)p.r; a|=p.a;

				c00=(0x800-dxx)*(0x800-dyy);
				c10=(dxx*(0x800-dyy));
				c11=(dxx*dyy);
				c01=(0x800*0x800-(c00+c10+c11));

				r=(c00*r00+c10*r10+c11*r11+c01*r01)/(0x800*0x800);
				if(r>255) r=255;
				if(r<0) r=0;

				p.r=p.g=p.b=(unsigned char)r;
				p.a=(unsigned char)a;

				out[x+Y]=p;
			}
		} else {
			for(x=frame_border; x<frame_border+smallw; x++)
			{
				xx=xxtable[x];

				dxx=dxxtable[x];

				a=0;
				if((yyout) || (xxouttable[x])) a=aAMB;

				p=in[(xx+0)+w*(yy+0)];
				r00=(int)p.r; g00=(int)p.g; b00=(int)p.b; a|=p.a;

				p=in[(xx+1)+w*(yy+0)];
				r10=(int)p.r; g10=(int)p.g; b10=(int)p.b; a|=p.a;

				p=in[(xx+1)+w*(yy+1)];
				r11=(int)p.r; g11=(int)p.g; b11=(int)p.b; a|=p.a;

				p=in[(xx+0)+w*(yy+1)];
				r01=(int)p.r; g01=(int)p.g; b01=(int)p.b; a|=p.a;

				c00=(0x800-dxx)*(0x800-dyy);
				c10=(dxx*(0x800-dyy));
				c11=(dxx*dyy);
				c01=(0x800*0x800-(c00+c10+c11));

				r=(c00*r00+c10*r10+c11*r11+c01*r01)/(0x800*0x800);
				if(r>255) r=255;
				if(r<0) r=0;

				g=(c00*g00+c10*g10+c11*g11+c01*g01)/(0x800*0x800);
				if(g>255) g=255;
				if(g<0) g=0;

				b=(int)(c00*b00+c10*b10+c11*b11+c01*b01)/(0x800*0x800);
				if(b>255) b=255;
				if(b<0) b=0;

				p.r=(unsigned char)r;
				p.g=(unsigned char)g;
				p.b=(unsigned char)b;
				p.a=(unsigned char)a;

				out[x+Y]=p;
			}
		}
	}
	return 0;
}

UINT CCombineDoc::Autoalignandsize(LPVOID param) // single pass
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->Autoalignandsizet();
	return 0;
}

void CCombineDoc::Autoalignandsizet()
{
	BeginThreads

	int min_half_size=MIN_HALF_SIZE;
	buissy=true;
	progress.AddString(IDS_AUTOALIGN1);
	progress.m_cancel=false;

	int f,master,Master,direction,count;
	int neww,newh,maxwh;
	PIXEL *f0,*f1,*sf0,*sf1,*tempsf;
	int xoff,yoff,newxoff,newyoff;
	float scale,newscale,scalestep,testscale;
	int i,x;
	int	xx;
	float xf;

	sf0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);
	sf1=(PIXEL*)GetBuffer(TEMP_BUFFER+1);

	int *xxtable=new int[w];
	int *dxxtable=new int[w];
	bool *xxouttable=new bool[w];

	int *xofftable=new int[last_used_frame];
	int *yofftable=new int[last_used_frame];
	float *scaletable=new float[last_used_frame];

	PIXEL *msubframe[32],*ssubframe[32];
	int level,levels;
	int width[32],height[32];

	__int64 test,besttest,besttest2;

	bool changed;

	CString s;

	// split master into subframes
	width[0]=w;
	height[0]=h;
	neww=w; newh=h;
	f0=(PIXEL*)GetBuffer(used_frame[0]);
	msubframe[0]=f0;
	msubframe[1]=sf0;
	level=1;
	while((neww>min_half_size) && (newh>min_half_size))
	{
		neww/=2;
		newh/=2;

		width[level]=neww;
		height[level]=newh;

		SetThreads(neww,newh)
		for(obj=0; obj<objects; obj++)
		{
			split_params[obj].bandw=bandw;
			split_params[obj].step=objects;
			split_params[obj].index=obj;
			split_params[obj].w=neww;
			split_params[obj].h=newh;
			split_params[obj].in=msubframe[level-1];
			split_params[obj].out=msubframe[level];
			StartThreads(split,split_params)
		}
		EndThreads

		level++;
		msubframe[level]=msubframe[level-1]+neww*newh;
	}
	levels=level;

	progress.SetRange(1,top_used_frame-1);
	for(f=1; f<last_used_frame; f++)
	{
		str1.LoadString(IDS_STRING201);
		s.Format(str1,buffer[used_frame[f]].name,buffer[used_frame[f-1]].name);
		progress.AddString(s);
		progress.SetPos(f);
		if(progress.m_cancel)
		{
			UpdateBuffer(used_frame[f-1]);
			goto finish;
		}

		// split subject frame into subframes
		neww=w; newh=h;
		f1=(PIXEL*)GetBuffer(used_frame[f]);
		ssubframe[0]=f1;
		ssubframe[1]=sf1;
		level=1;
		while((neww>min_half_size) && (newh>min_half_size))
		{
			neww/=2;
			newh/=2;

			SetThreads(neww,newh)
			for(obj=0; obj<objects; obj++)
			{
				split_params[obj].bandw=bandw;
				split_params[obj].step=objects;
				split_params[obj].index=obj;
				split_params[obj].w=neww;
				split_params[obj].h=newh;
				split_params[obj].in=ssubframe[level-1];
				split_params[obj].out=ssubframe[level];
				StartThreads(split,split_params)
			}
			EndThreads

			level++;
			ssubframe[level]=ssubframe[level-1]+neww*newh;
		}
		levels=level;

		xoff=yoff=0;
		scale=1.0;

		for(level=levels-1; level>=0; level--)
		{
retry:		changed=true;
			while(changed)
			{
				changed=false;

				besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
				newxoff=xoff;
				for(i=-1; i<2; i++)
				{
					if(i==0) continue;
					test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],i+xoff,yoff,scale);
					if(test<besttest)
					{
						besttest=test;
						newxoff=i+xoff;
						changed=true;
					}
				}
				xoff=newxoff;

				if(changed) besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
				newyoff=yoff;
				for(i=-1; i<2; i++)
				{
					if(i==0) continue;
					test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,i+yoff,scale);
					if(test<besttest)
					{
						besttest=test;
						newyoff=i+yoff;
						changed=true;
					}
				}
				yoff=newyoff;
			}

			changed=false;

			maxwh=width[level]; if(height[level]>maxwh) maxwh=height[level];
			scalestep=(float)(maxwh+2)/(float)maxwh-1.0f;

			test=besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
			newscale=testscale=scale;
			while(test<=besttest)
			{
				testscale+=scalestep;
				test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,testscale);
				if(test<besttest)
				{
					besttest=test;
					newscale=testscale;
					changed=true;
				}
			}

			testscale=scale;
			test=besttest2=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
			while(test<=besttest2)
			{
				testscale-=scalestep;
				test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,testscale);
				if(test<besttest2)
				{
					besttest2=test;
					if(besttest2<besttest)
					{
						newscale=testscale;
						besttest=besttest2;
						changed=true;
					}
				}
			}
			scale=newscale;

			if(changed) goto retry;

			if(level!=0)
			{
				xoff*=width[level-1]/width[level];
				yoff*=height[level-1]/height[level];
			}
		}

		// final alignment
		changed=true;
		level=0;
		while(changed)
		{
			changed=false;

			besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
			newxoff=xoff;
			for(i=-1; i<2; i++)
			{
				if(i==0) continue;
				test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],i+xoff,yoff,scale);
				if(test<besttest)
				{
					besttest=test;
					newxoff=i+xoff;
					changed=true;
				}
			}
			xoff=newxoff;

			besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
			newyoff=yoff;
			for(i=-1; i<2; i++)
			{
				if(i==0) continue;
				test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,i+yoff,scale);
				if(test<besttest)
				{
					besttest=test;
					newyoff=i+yoff;
					changed=true;
				}
			}
			yoff=newyoff;
		}
		UpdateBuffer(used_frame[f-1]);

		f0=f1;
		tempsf=sf0; sf0=sf1; sf1=tempsf;
		for(x=0; x<levels; x++) msubframe[x]=ssubframe[x];
		msubframe[0]=f0;

		xofftable[f]=xoff;
		yofftable[f]=yoff;
		scaletable[f]=scale;

		str1.LoadString(IDS_STRING202);
		s.Format(str1,-xoff,-yoff,1.0f/scale);
		progress.AddString(s);
	}
	UpdateBuffer(used_frame[f-1]);

// scale and shift frames
	i=active_frame;
	do{
		if(!buffer[i].hidden)
		{
			Master=UseFrame(i);
			break;
		}
		i++;
		if(i==top_frame) i=0;
	}while(i!=active_frame);
	if(Master==0) direction=1; else direction=-1;

	str1.LoadString(IDS_STRING203);
	progress.AddString(str1);
	progress.SetRange(0,top_used_frame-1);
	count=0;
	changed=false;
	while(true) // break out at end of loop
	{
		xoff=0;
		yoff=0;
		scale=1.0f;
		master=Master;
		for(f=master+direction; f<last_used_frame && f>=0; f+=direction)
		{
			progress.SetPos(count);
			count++;

			if(direction==1)
			{
				xoff+=xofftable[f];
				yoff+=yofftable[f];	
				scale/=scaletable[f];
			}
			else
			{
				xoff-=xofftable[f+1];
				yoff-=yofftable[f+1];	
				scale*=scaletable[f+1];
			}

			if((xoff==0) && (yoff==0) && (scale>0.9999f) && (scale<1.0001f)) continue;

			changed=true;

			f1=(PIXEL*)GetBuffer(used_frame[f]);

			for(i=0; i<w*h; i++)
			{
				sf1[i]=f1[i];
				f1[i].abgr=0;
				f1[i].a=aAMB;
			}

			for(x=0; x<w; x++)
			{
				xf=((float)(x-xoff)-(float)(w/2))*scale+(float)(w/2);
				xx=(int)xf;
				xxouttable[x]=false;
				if(xx<frame_border)
				{
					if(x>goodrect.left) goodrect.left=x;
					xx=frame_border;
					xxouttable[x]=true;
					xf=(float)xx;
					if(xx>goodrect.left) goodrect.left=xx;
				}
				if(xx>frame_border+smallw-2)
				{
					if(x<goodrect.right) goodrect.right=x;
					xx=frame_border+smallw-2;
					xxouttable[x]=true;
					xf=(float)xx;
					if(xx<goodrect.right) goodrect.right=xx;
				}
				xxtable[x]=xx;
				dxxtable[x]=(int)((xf-(float)xx)*(float)0x800);
			}

			SetThreads(w,h)
			for(obj=0; obj<objects; obj++)
			{
				sax_params[obj].index=obj;
				sax_params[obj].step=objects;
				sax_params[obj].bandw=bandw;
				sax_params[obj].dxxtable=dxxtable;
				sax_params[obj].frame_border=frame_border;
				sax_params[obj].in=sf1;
				sax_params[obj].out=f1;
				sax_params[obj].smallw=smallw;
				sax_params[obj].smallh=smallh;
				sax_params[obj].w=w;
				sax_params[obj].h=h;
				sax_params[obj].xxouttable=xxouttable;
				sax_params[obj].xxtable=xxtable;
				sax_params[obj].goodrect=goodrect;
				sax_params[obj].yoff=yoff;
				sax_params[obj].scale=scale;
				StartThreads(sax,sax_params)
			}
			EndThreads

			TidyOuterEdges(used_frame[f]);

			UpdateBuffer(used_frame[f]);
		}

		if(direction==1) break; 
		direction=1;
	}
finish:
	for(f=0; f<last_used_frame;f++) TidyOuterEdges(used_frame[f]);
	delete [] xxouttable;
	progress.SetPos(count);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+0);

	delete [] xxtable;
	delete [] dxxtable;
	delete [] xofftable;
	delete [] yofftable;
	delete [] scaletable;
	progress.SetPos(0);
	str1.LoadString(IDS_STRING204);
	if(!changed) progress.AddString(str1);
	if(refreshscreen)
	{
		display=active_frame;
		refreshscreen=false;
		progress.m_cancel=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
		RefreshScreen();
	}
}

UINT CCombineDoc::Autoalignandsize2(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->Autoalignandsize2t();
	return 0;
}

void CCombineDoc::Autoalignandsize2t()
{
	BeginThreads

	buissy=true;

	progress.AddString(IDS_AUTOALIGN2);

	int pass;
	bool no_first_pass=false;
	int f,master,Master,direction,count;
	int neww,newh,maxwh;
	PIXEL *f0,*f1,*sf0,*sf1,*tempsf;
	int xoff,yoff,newxoff,newyoff;
	float scale,newscale,scalestep,testscale,scale_factor;
	int i,j,x;
	int	xx;
	float xf;

	sf0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);
	sf1=(PIXEL*)GetBuffer(TEMP_BUFFER+1);

	int *xxtable=new int[w];
	int *dxxtable=new int[w];

	int *xofftable=new int[last_used_frame];
	int *yofftable=new int[last_used_frame];
	bool *xxouttable=new bool[w];

	float *scaletable=new float[last_used_frame];
	float *scalevalues=new float[last_used_frame];
	int *scalehist=new int[last_used_frame];
	int bins,scale_count;

	PIXEL *msubframe[10],*ssubframe[10];
	int level,levels;
	int width[10],height[10];

	__int64 test,besttest,besttest2;

	bool changed;

	CString s;

	refresh_used_list();

	for(pass=0; pass<2; pass++)
	{
		str1.LoadString(IDS_STRING205);
		if(pass==0) progress.AddString(str1);
		else {
			str1.LoadString(IDS_STRING206);
			progress.AddString(str1);
			if(no_first_pass)
			{
				str1.LoadString(IDS_STRING207);
				progress.AddString(str1);
				goto skip;
			}
		}
		// split master into subframes
		width[0]=w;
		height[0]=h;
		neww=w; newh=h;
		f0=(PIXEL*)GetBuffer(used_frame[0]);
		msubframe[0]=f0;
		msubframe[1]=sf0;
		level=1;
		while((neww>MIN_HALF_SIZE) && (newh>MIN_HALF_SIZE))
		{
			neww/=2;
			newh/=2;

			width[level]=neww;
			height[level]=newh;

			SetThreads(neww,newh)
			for(obj=0; obj<objects; obj++)
			{
				split_params[obj].bandw=bandw;
				split_params[obj].step=objects;
				split_params[obj].index=obj;
				split_params[obj].w=neww;
				split_params[obj].h=newh;
				split_params[obj].in=msubframe[level-1];
				split_params[obj].out=msubframe[level];
				StartThreads(split,split_params)
			}
			EndThreads

			level++;
			msubframe[level]=msubframe[level-1]+neww*newh;
		}
		levels=level;

		scale=1.0f;
		
		progress.SetRange(1,top_used_frame-1);
		for(f=1; f<last_used_frame; f++)
		{
			str1.LoadString(IDS_STRING208);
			s.Format(str1,buffer[used_frame[f]].name,buffer[used_frame[f-1]].name);
			progress.AddString(s);
			progress.SetPos(f);
			if(progress.m_cancel)
			{
				UpdateBuffer(used_frame[f-1]);
				goto finish;
			}

			// split subject frame into subframes
			neww=w; newh=h;
			f1=(PIXEL*)GetBuffer(used_frame[f]);
			ssubframe[0]=f1;
			ssubframe[1]=sf1;
			level=1;
			while((neww>MIN_HALF_SIZE) && (newh>MIN_HALF_SIZE))
			{
				neww/=2;
				newh/=2;

				width[level]=neww;
				height[level]=newh;

				SetThreads(neww,newh)
				for(obj=0; obj<objects; obj++)
				{
					split_params[obj].bandw=bandw;
					split_params[obj].step=objects;
					split_params[obj].index=obj;
					split_params[obj].w=neww;
					split_params[obj].h=newh;
					split_params[obj].in=ssubframe[level-1];
					split_params[obj].out=ssubframe[level];
					StartThreads(split,split_params)
				}
				EndThreads

				level++;
				ssubframe[level]=ssubframe[level-1]+neww*newh;
			}

			xoff=yoff=0;
			scale=1.0;
			for(level=levels-1; level>=0; level--)
			{
retry:			changed=true;
				while(changed)
				{
					changed=false;

					besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
					newxoff=xoff;
					for(i=-1; i<2; i++)
					{
						if(i==0) continue;
						test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],i+xoff,yoff,scale);
						if(test<besttest)
						{
							besttest=test;
							newxoff=i+xoff;
							changed=true;
						}
					}
					xoff=newxoff;

					if(changed) besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
					newyoff=yoff;
					for(i=-1; i<2; i++)
					{
						if(i==0) continue;
						test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,i+yoff,scale);
						if(test<besttest)
						{
							besttest=test;
							newyoff=i+yoff;
							changed=true;
						}
					}
					yoff=newyoff;
				}
				maxwh=width[level]; if(height[level]>maxwh) maxwh=height[level];
				scalestep=(float)(maxwh+2)/(float)maxwh-1.0f;
				test=besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
				newscale=testscale=scale;
				changed=false;
				test--;
				while(test<besttest)
				{
					testscale+=scalestep;
					test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,testscale);
					if(test<besttest)
					{
						besttest=test;
						newscale=testscale;
						changed=true;
					}
				}

				testscale=scale;
				test=besttest2=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
				test--;
				while(test<besttest2)
				{
					testscale-=scalestep;
					test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,testscale);
					if(test<besttest2)
					{
						besttest2=test;
						if(besttest2<besttest)
						{
							newscale=testscale;
							changed=true;
						}
					}
				}
				scale=newscale;
				if(changed) goto retry;

				if(level!=0)
				{
					xoff*=width[level-1]/width[level];
					yoff*=height[level-1]/height[level];
				}
			}

			// final alignment
			changed=true;
			level=0;
			while(changed)
			{
				changed=false;

				besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
				newxoff=xoff;
				for(i=-1; i<2; i++)
				{
					if(i==0) continue;
					test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],i+xoff,yoff,scale);
					if(test<besttest)
					{
						besttest=test;
						newxoff=i+xoff;
						changed=true;
					}
				}
				xoff=newxoff;

				if(changed) besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
				newyoff=yoff;
				for(i=-1; i<2; i++)
				{
					if(i==0) continue;
					test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,i+yoff,scale);
					if(test<besttest)
					{
						besttest=test;
						newyoff=i+yoff;
						changed=true;
					}
				}
				yoff=newyoff;
			}
			UpdateBuffer(used_frame[f-1]);

			f0=f1;
			tempsf=sf0; sf0=sf1; sf1=tempsf;
			for(x=1; x<levels; x++) msubframe[x]=ssubframe[x];
			msubframe[0]=f0;

			xofftable[f]=xoff;
			yofftable[f]=yoff;
			scaletable[f]=scale;

			str1.LoadString(IDS_STRING209);
			s.Format(str1,-xoff,-yoff,1.0f/scale);
			progress.AddString(s);
		}
		UpdateBuffer(used_frame[f-1]);
skip:
	// scale and shift frames
		i=active_frame;
		do{
			if(!buffer[i].hidden)
			{
				Master=UseFrame(i);
				break;
			}
			i++;
			if(i==top_frame) i=0;
		} while(i!=active_frame);
		if(Master==0) direction=1; else direction=-1;

		if(last_used_frame>2)
		{
			bins=1;
			scalevalues[0]=scaletable[1];
			scalehist[0]=1;
			for(f=2; f<last_used_frame; f++)
			{
				for(i=0; i<bins; i++)
				{
					if(scaletable[f]==scalevalues[i])
					{
						scalehist[i]++;
						break;
					}
				}
				if(i==bins)
				{
					scalevalues[i]=scaletable[f];
					scalehist[i]=1;
					bins++;
				}
			}
			scale_count=0;
			scale_factor=0.0f;
			for(i=0; i<bins; i++)
			{
				j=scalehist[i]*scalehist[i];
				scale_count+=j;
				scale_factor+=(float)j*scalevalues[i];
			}
			scale_factor=scale_factor/(float)(scale_count);
		} else scale_factor=scaletable[1];

		if(pass==0)
		{
			if((1.0f/scale_factor>0.9999) && (1.0f/scale_factor<1.0001))
			{
				str1.LoadString(IDS_STRING210);
				s.Format(str1);
				progress.AddString(s);
				progress.SetRange(0,top_used_frame-1);
				no_first_pass=true;
				continue;
			}
			str1.LoadString(IDS_STRING211);
			s.Format(str1,1.0f/scale_factor);
			progress.AddString(s);
			progress.SetRange(0,top_used_frame-1);
			no_first_pass=false;
		} else {
			str1.LoadString(IDS_STRING212);
			s.Format(str1);
			progress.AddString(s);
			progress.SetRange(0,top_used_frame-1);
		}
		count=0;
		changed=false;
		progress.SetPos(count);
		while(true) // break out at end of loop
		{
			xoff=0;
			yoff=0;
			scale=1.0f;
			master=Master;
			for(f=master+direction; f<last_used_frame && f>=0; f+=direction)
			{
				progress.SetPos(count);
				count++;

				if(direction==1)
				{
					if(pass==1)
					{
						xoff+=xofftable[f];
						yoff+=yofftable[f];	
					}
					scale/=scaletable[f];
				} else {
					xoff-=xofftable[f+1];
					yoff-=yofftable[f+1];
					scale*=scaletable[f+1];
				}

				if((xoff==0) && (yoff==0) && (scale>0.9999f) && (scale<1.0001f)) continue;

				changed=true;

				f1=(PIXEL*)GetBuffer(used_frame[f]);

				for(i=0; i<w*h; i++)
				{
					sf1[i]=f1[i];
					f1[i].abgr=0;
					f1[i].a=aAMB;
				}

				for(x=0; x<w; x++)
				{
					xf=((float)(x-xoff)-(float)(w/2))*scale+(float)(w/2);
					xx=(int)xf;
					xxouttable[x]=false;
					if(xx<frame_border)
					{
						if(x>goodrect.left) goodrect.left=x;
						xx=frame_border;
						xf=(float)xx;
						xxouttable[x]=true;
					}
					if(xx>frame_border+smallw-2)
					{
						if(x<goodrect.right) goodrect.right=x;
						xx=frame_border+smallw-2;
						xf=(float)xx;
						xxouttable[x]=true;
					}
					xxtable[x]=xx;
					dxxtable[x]=(int)((xf-(float)xx)*(float)0x800);
				}

				SetThreads(w,h)
				for(obj=0; obj<objects; obj++)
				{
					sax_params[obj].index=obj;
					sax_params[obj].step=objects;
					sax_params[obj].bandw=bandw;
					sax_params[obj].dxxtable=dxxtable;
					sax_params[obj].frame_border=frame_border;
					sax_params[obj].in=sf1;
					sax_params[obj].out=f1;
					sax_params[obj].smallw=smallw;
					sax_params[obj].smallh=smallh;
					sax_params[obj].w=w;
					sax_params[obj].h=h;
					sax_params[obj].xxouttable=xxouttable;
					sax_params[obj].xxtable=xxtable;
					sax_params[obj].goodrect=goodrect;
					sax_params[obj].yoff=yoff;
					sax_params[obj].scale=scale;
					StartThreads(sax,sax_params)
				}
				EndThreads

				TidyOuterEdges(used_frame[f]);

				UpdateBuffer(used_frame[f]);
			}

			if(direction==1) break; 
			direction=1;
		}
		progress.SetPos(count);
	}
finish:
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+0);

	delete [] xxtable;
	delete [] dxxtable;
	delete [] xofftable;
	delete [] yofftable;
	delete [] scaletable;
	delete [] scalevalues;
	delete [] scalehist;
	delete [] xxouttable;
	progress.SetPos(0);
	if(!changed)
	{
		str1.LoadString(IDS_STRING213);
		progress.AddString(str1);
	}
	if(refreshscreen)
	{
		display=active_frame;
		refreshscreen=false;
		progress.m_cancel=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
		RefreshScreen();
	}
}

// CZ_SIZE

// resize/rotate/move
void CCombineDoc::OnSizecompensation() 
{
	if(!last_used_frame) return;
	if((mode&GROUP_MASK)==SPECIAL) return;

	if(!positionparams()) return;

	mode=CZ_SIZE;

	OnClear();

	sizecompensation();

	Redraw(0,false);
}

// public function
void CCombineDoc::Size()
{
	OnSizecompensation();
}

struct rrm_Params
{
	int index,step;
	bool bandw;
	int w,h;
	int sizexorg,sizeyorg;
	int offsetx,offsety;
	double f;
	int angle;
	PIXEL *in,*out;
	RECT temp_goodect;
} rrm_params[32];

UINT CCombineDoc::rrm(LPVOID param)
{
	rrm_Params *pp=(rrm_Params *)param;
	int angle=pp->angle;
	bool bandw=pp->bandw;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	int sizexorg=pp->sizexorg;
	int sizeyorg=pp->sizeyorg;
	int offsetx=pp->offsetx;
	int offsety=pp->offsety;
	double f=pp->f;
	PIXEL *in=pp->in;
	PIXEL *out=pp->out;
	RECT goodrect=pp->temp_goodect;

	int aa;
	double r,g,b;
	double r00,g00,b00; // the colour values at the 4 surrounding pixels
	double r10,g10,b10;
	double r11,g11,b11;
	double r01,g01,b01;
	double xf,yf,xsf,ysf;
	double c00,c10,c11,c01;
	double dxx,dyy;
	double sina=(double)sin(2.0*3.14159*(double)(36000-angle)/36000.0);
	double cosa=(double)cos(2.0*3.14159*(double)(36000-angle)/36000.0);
	int xs,ys; // source coords
	int xd,yd; // dest
	int tempx,tempy;
	PIXEL p;
	int notgood;

	for(xd=index; xd<w; xd+=step)
	{
		for(yd=0; yd<h; yd++)
		{
			// convert dest coords to double and adjust for centre
			xf=f*((double)xd-(double)sizexorg);
			yf=f*((double)yd-(double)sizeyorg);

			// calc floating point source,adjusted for centre and displacement
			xsf=cosa*xf+sina*yf+(double)sizexorg-(double)offsetx;
			ysf=cosa*yf-sina*xf+(double)sizeyorg-(double)offsety;

			// roll pixels over from oposite edge when needed
			notgood=0;
			while(xsf<0.0){notgood|=1; xsf+=(double)w;}
			while(xsf>=(double)w){notgood|=2; xsf-=(double)w;}
			xs=(unsigned int)(int)xsf;
			while(ysf<0.0){notgood|=4; ysf+=(double)h;}
			while(ysf>=(double)h){notgood|=8; ysf-=(double)h;}
			ys=(unsigned int)(int)ysf;

			aa=0;

			// get pixel from surrounding corners, allowing for rollovers
			p=in[xs+w*ys];
			r00=(double)(int)p.r; g00=(double)(int)p.g; b00=(double)(int)p.b; aa|=p.a;

			tempx=xs+1;
			if(tempx>=w) tempx-=w;

			p=in[tempx+w*ys];
			r10=(double)(int)p.r; g10=(double)(int)p.g; b10=(double)(int)p.b; aa|=p.a;

			tempy=ys+1;
			if(tempy>=h) tempy-=h;

			p=in[tempx+w*tempy];
			r11=(double)(int)p.r; g11=(double)(int)p.g; b11=(double)(int)p.b; aa|=p.a;

			p=in[xs+w*tempy];
			r01=(double)(int)p.r; g01=(double)(int)p.g; b01=(double)(int)p.b; aa|=p.a;

			// find fraction part of coords
			dxx=xsf-(double)(int)xsf; dyy=ysf-(double)(int)ysf;

			// calculate interpolation constants, just a guess but I have had no complaints
			c00=(1.0-dxx)*(1.0-dyy);
			c10=dxx*(1.0-dyy);
			c11=dxx*dyy;
			c01=(1.0-dxx)*dyy;

			// interpolate the true rgb values
			r=c00*r00+c10*r10+c11*r11+c01*r01;
			if(r>255.0) r=255.0;
			if(r<0.0) r=0.0;
			g=c00*g00+c10*g10+c11*g11+c01*g01;
			if(g>255.0) g=255.0;
			if(g<0.0) g=0.0;
			b=c00*b00+c10*b10+c11*b11+c01*b01;
			if(b>255.0) b=255.0;
			if(b<0.0) b=0.0;

			p.r=(unsigned char)r;
			p.g=(unsigned char)g;
			p.b=(unsigned char)b;

			// adjust clean rectangle
			if(notgood!=0)
			{
				aa=aAMB;
				if((xd>goodrect.left) && (xd<goodrect.right) && (yd>goodrect.top) && (yd<goodrect.bottom))
				{
					if((xd>goodrect.left) && (xd<w/2) && (notgood&1)) goodrect.left=xd;
					if((xd<goodrect.right) && (xd>w/2) && (notgood&2)) goodrect.right=xd;
					if((yd>goodrect.top) && (yd<h/2) && (notgood&4)) goodrect.top=yd;
					if((yd<goodrect.bottom) && (yd>h/2) && (notgood&8)) goodrect.bottom=yd;
				}
			}
			p.a=(unsigned char)aa;

			out[xd+yd*w]=p;
		}
	}

	return 0;
}

// the resize/rotate/move calculation, floating point with interpolation which I hope is the right way round
void CCombineDoc::sizecompensation()
{
	BeginThreads
	SetThreads(w,h)

	str1.LoadString(IDS_STRING214);
	progress.AddString(str1);
	progress.AddString("   "+buffer[active_frame].name);

	PIXEL* buf=(PIXEL*)GetBuffer(TEMP_BUFFER);

	double f=((double)size_factor)/10000.0;
	f=1.0/f;
	int x,xs,ys;
	PIXEL *f1;

	f1=(PIXEL*)GetBuffer(active_frame);

	BeginWaitCursor();

	for(xs=0; xs<w; xs++)
		for(ys=0; ys<h; ys++)
			buf[xs+ys*w]=f1[xs+ys*w];

	for(obj=0; obj<objects; obj++)
	{
		rrm_params[obj].angle=angle;
		rrm_params[obj].bandw=bandw;
		rrm_params[obj].f=f;
		rrm_params[obj].h=h;
		rrm_params[obj].in=buf;
		rrm_params[obj].index=obj;
		rrm_params[obj].offsetx=offsetx;
		rrm_params[obj].offsety=offsety;
		rrm_params[obj].out=f1;
		rrm_params[obj].sizexorg=sizexorg;
		rrm_params[obj].sizeyorg=sizeyorg;
		rrm_params[obj].step=objects;
		rrm_params[obj].temp_goodect=goodrect;
		rrm_params[obj].w=w;
		StartThreads(rrm,rrm_params)
	}
	EndThreads
	for(x=0; x<(int)objects; x++)
	{
		if(rrm_params[x].temp_goodect.bottom>goodrect.bottom) goodrect.bottom=rrm_params[x].temp_goodect.bottom;
		if(rrm_params[x].temp_goodect.left>goodrect.left) goodrect.left=rrm_params[x].temp_goodect.left;
		if(rrm_params[x].temp_goodect.right<goodrect.right) goodrect.right=rrm_params[x].temp_goodect.right;
		if(rrm_params[x].temp_goodect.top<goodrect.top) goodrect.top=rrm_params[x].temp_goodect.top;
	}

	EndWaitCursor();
	display=active_frame;
	Redraw(0,false);
	UpdateBuffer(active_frame);
	FreeBuffer(TEMP_BUFFER);
}

// use points to set size parameters
void CCombineDoc::OnSizeCalculate() 
{
	if(!top_frame) return;
	int button;

	Sizer.Calculate();

	button=Sizer.DoModal();

	// t represents valid number fields in dialog
	int t=0;
	if((Sizer.p[0].valid)) t+=1;
	if((Sizer.p[1].valid)) t+=2;
	if((Sizer.p[2].valid)) t+=4;
	if((Sizer.p[3].valid)) t+=8;

	switch(button)
	{
	case REG:
	case SAVE:
		if(t==15)
		{
			if(
				(Sizer.p[0].f!=Sizer.p[1].f) ||
				(Sizer.p[2].f!=Sizer.p[3].f))
			{
				CString s1,s2;
				s1.LoadString(IDS_SIZEERROR1); s2.LoadString(IDS_ERROR);
				MessageBox(progress.m_hWnd,s1,s2,MB_OK);
				return;
			}

			if(Sizer.length12==0 || Sizer.length34==0)
			{
				CString s1,s2;
				s1.LoadString(IDS_SIZEERROR2); s2.LoadString(IDS_ERROR);
				MessageBox(progress.m_hWnd,s1,s2,MB_OK);
				return;
			}
		}

		switch (t)
		{
		case 1:
			sizexorg=Sizer.p[0].x;
			sizeyorg=Sizer.p[0].y;
			if(button!=SAVE)
			{
				active_frame=display=(int)Sizer.p[2].f;
				OnSizecompensation();
			}
			return;
		case 5: case 15:
			sizexorg=Sizer.p[0].x;
			sizeyorg=Sizer.p[0].y;
			offsetx=Sizer.p[0].x-Sizer.p[2].x;
			offsety=Sizer.p[0].y-Sizer.p[2].y;
			if(t==15)
			{
				angle=Sizer.angle;
				size_factor=(int)(10000.0f/Sizer.ratio);
			} else {
				angle=0;
				size_factor=10000;
			}
			if(button!=SAVE)
			{
				active_frame=display=Sizer.p[2].f;
				OnSizecompensation();
			}
			return;
		default:
			CString s1,s2;
			s1.LoadString(IDS_SIZEERROR3); s2.LoadString(IDS_ERROR);
			MessageBox(progress.m_hWnd,s1,s2,MB_OK);
			return;
		}
	}
}

void CCombineDoc::OnSizeClearpoints() 
{
	Sizer.p[0].valid=false;
	Sizer.p[1].valid=false;
	Sizer.p[2].valid=false;
	Sizer.p[3].valid=false;
	Redraw(0,false);
}

void CCombineDoc::calc()
{
	if(!top_frame) return;
	OnSizeCalculate();
}

bool CCombineDoc::sizeparams()
{
	if(!top_frame) return false;
	CSizeDia dd;
	dd.m_factor=size_factor;
	dd.m_x=sizexorg;
	dd.m_y=sizeyorg;
	int button;
	do{
		button=dd.DoModal();
		if(button==IDCANCEL) return false;
	} while((dd.m_factor<1) || (dd.m_x<0) || (dd.m_y<0) || (dd.m_x>=w) || (dd.m_y>=h));
	size_factor=dd.m_factor;
	sizexorg=dd.m_x;
	sizeyorg=dd.m_y;

	fbase=((float)(int)dd.m_factor)/10000.0f;
	fbase=1.0f/fbase;
	return true;
}

// same method as earlier function but size only, and a frame loop
// only increases in size so no dirty edges but old ones forgotten.
void CCombineDoc::OnResizeall() 
{
	if(top_frame<2) return;
	if(!sizeparams()) return;
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	progress.Maximize();
	refreshscreen=true;
	AfxBeginThread(resizeall,this);
}

UINT CCombineDoc::resizeall(LPVOID pointer)
{
	CCombineDoc *_this=(CCombineDoc*)pointer;
	_this->resizeall();
	return 0;
}

struct RA_Params
{
	int index,step;
	int w,h;
	float f;
	PIXEL *buf,*f1;
	int sizexorg,sizeyorg;
} ra_params[32];

UINT CCombineDoc::ra(LPVOID param)
{
	RA_Params *pp=(RA_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	float f=pp->f;
	PIXEL *buf=pp->buf;
	PIXEL *f1=pp->f1;
	int sizexorg=pp->sizexorg;
	int sizeyorg=pp->sizeyorg;

	double r,g,b;
	double r00,g00,b00;
	double r10,g10,b10;
	double r11,g11,b11;
	double r01,g01,b01;

	double xf,yf;
	double c00,c10,c11,c01;
	double dxx,dyy;
	int xx,yy;
	int x,y,Y;
	int a;
	PIXEL p;

	for(y=index; y<h; y+=step)
	{
		Y=w*y;
		yf=f*((float)y-(float)sizeyorg)+(float)sizeyorg;
		yy=(int)yf;
		if(h<yy+2) continue;
		dyy=yf-(double)yy;
		for(x=0; x<w; x++)
		{
			xf=f*((float)x-(float)sizexorg)+(float)sizexorg;

			xx=(unsigned int)xf;
			if(w<xx+2) continue;

			a=0;

			p=buf[(xx+0)+w*(yy+0)];
			r00=(float)(int)p.r; g00=(float)(int)p.g; b00=(float)(int)p.b; a|=p.a;

			p=buf[(xx+1)+w*(yy+0)];
			r10=(float)(int)p.r; g10=(float)(int)p.g; b10=(float)(int)p.b; a|=p.a;

			p=buf[(xx+1)+w*(yy+1)];
			r11=(float)(int)p.r; g11=(float)(int)p.g; b11=(float)(int)p.b; a|=p.a;

			p=buf[(xx+0)+w*(yy+1)];
			r01=(float)(int)p.r; g01=(float)(int)p.g; b01=(float)(int)p.b; a|=p.a;

			dxx=xf-(double)(int)xx;
			c00=(1.0-dxx)*(1.0-dyy);
			c10=dxx*(1.0-dyy);
			c11=dxx*dyy;
			c01=1.0-(c00+c10+c11);

			r=c00*r00+c10*r10+c11*r11+c01*r01;
			if(r>255.0f) r=255.0f;
			if(r<0.0f) r=0.0f;
			g=c00*g00+c10*g10+c11*g11+c01*g01;
			if(g>255.0f) g=255.0f;
			if(g<0.0f) g=0.0f;
			b=c00*b00+c10*b10+c11*b11+c01*b01;
			if(b>255.0f) b=255.0f;
			if(b<0.0f) b=0.0f;

			p.r=(unsigned char)r;
			p.g=(unsigned char)g;
			p.b=(unsigned char)b;
			p.a=(unsigned char)a;
			f1[x+Y]=p;
		}
	}

	return 0;
}

void CCombineDoc::resizeall()
{
	BeginThreads
	SetThreads(w,h)

	buissy=true;

	progress.AddString(IDS_RESIZEALL);
	progress.m_cancel=false;

	int i,j;
	int x,y,Y;
	PIXEL *f1;

	float f;

	PIXEL* buf=(PIXEL*)GetBuffer(TEMP_BUFFER);

	int pr=0;
	progress.SetRange(1,top_used_frame-1);

	// start at top|bottom of stack depending on mag factor
	int start=last_used_frame-2,end=-1,inc=-1,count=0;
	if(size_factor<10000)
	{
		start=1;
		end=last_used_frame;
		inc=+1;
		fbase=1/fbase;
	}
	f=fbase; // f is current mag factor, multiplied by fbase for each new frame
	for(i=start; i!=end; i+=inc)
	{
		progress.SetPos(pr);
		if(progress.m_cancel)
		{
			progress.m_cancel=false;
			break;
		}
		pr++;
		f1=(PIXEL*)GetBuffer(used_frame[i]);
		for(y=0; y<h; y++)
		{
			Y=w*y;
			for(x=0; x<w; x++)
			{
				j=x+Y;
				buf[j]=f1[j];
			}
		}

		for(obj=0; obj<objects; obj++)
		{
			ra_params[obj].index=obj;
			ra_params[obj].step=objects;
			ra_params[obj].w=w;
			ra_params[obj].h=h;
			ra_params[obj].f=f;
			ra_params[obj].buf=buf;
			ra_params[obj].f1=f1;
			ra_params[obj].sizexorg=sizexorg;
			ra_params[obj].sizeyorg=sizeyorg;
			StartThreads(ra,ra_params)
		}
		EndThreads

		f*=fbase;
		UpdateBuffer(used_frame[i]);
	}

	FreeBuffer(TEMP_BUFFER);

	progress.SetPos(0);

	if(refreshscreen)
	{
		display=active_frame;
		progress.m_cancel=false;
		buissy=false;
		refreshscreen=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
		RefreshScreen();
	}
}

bool CCombineDoc::positionparams()
{
	if(!last_used_frame) return false;
	CRegisterDia dd;
	dd.m_f=size_factor;
	dd.m_x=sizexorg;
	dd.m_y=sizeyorg;
	dd.m_ox=offsetx;
	dd.m_oy=offsety;
	dd.m_a=angle;
	int button;
	do{
		button=dd.DoModal();
		if(button==IDCANCEL) return false;
	} while(
		(dd.m_f<1) ||
		(dd.m_x<0) || (dd.m_y<0) ||
		(dd.m_x>=w) || (dd.m_y>=h));
	size_factor=dd.m_f;
	sizexorg=dd.m_x;
	sizeyorg=dd.m_y;
	offsetx=dd.m_ox;
	offsety=dd.m_oy;
	angle=dd.m_a;
	return true;
}

// Seach for good pixel in same place above and below
void CCombineDoc::TidyEdges(int f)
{
	int i,ff,x,y,Y;

	f=UseFrame(f);

	PIXEL *f1,*f2;
	f1=(PIXEL*)GetBuffer(f);

	int startx=frame_border;
	int endx=frame_border+smallw;
	int starty=frame_border;
	int endy=frame_border+smallh;

	if(top_frame==1) goto finish;

	for(y=starty; y<endy; y++)
	{
		Y=y*w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(((f1[i].a!=aGOOD) || (f1[i].a&(aHORIZONTAL|aVERTICAL|aAMB))) && (f1[i].a!=aCLEAR))
			{
				for(ff=1; ff<last_used_frame; ff++)
				{
					if(f+ff<last_used_frame)
					{
						f2=(PIXEL*)GetBuffer(f+ff);
						if(f2[i].a==aGOOD)
						{
							f1[i]=f2[i];
							f1[i].a=aAMB;
							FreeBuffer(f+ff);
							continue;
						}
						FreeBuffer(f+ff);
					}

					if(f-ff>=0)
					{
						f2=(PIXEL*)GetBuffer(f-ff);
						if(f2[i].a==aGOOD)
						{
							f1[i]=f2[i];
							f1[i].a=aAMB;
							FreeBuffer(f-ff);
							continue;
						}
						FreeBuffer(f-ff);
					}
				}
			}
		}
	}
	UpdateBuffer(f);
finish:
	TidyOuterEdges(f);
}

// fill borders with something
void CCombineDoc::TidyOuterEdges(int f)
{
	int i,j,x,y,Y,fullx,fully,fullY;
	PIXEL *f1=(PIXEL*)GetBuffer(f);
	PIXEL *full=(PIXEL*)memory_slot[buffer[f].slot].pointer;

	// left border
	for(y=frame_border; y<smallh+frame_border; y++)
	{
		Y=y*w;
		for(fullx=0,x=2*frame_border; fullx<frame_border; fullx++,x--)
		{
			i=x+Y;
			j=fullx+Y;
			full[j]=f1[i];
			full[j].a=aAMB;
		}
	}

	// right border
	for(y=frame_border; y<smallh+frame_border; y++)
	{
		Y=y*w;
		for(fullx=w-1,x=2*frame_border+2*smallw-w-1; fullx>=frame_border+smallw; fullx--,x++)
		{
			i=x+Y;
			j=fullx+Y;
			full[j]=f1[i];
			full[j].a=aAMB;
		}
	}

	// top border
	for(fully=0,y=2*frame_border; fully<frame_border; fully++,y--)
	{
		fullY=fully*w;
		Y=y*w;
		for(fullx=0; fullx<w; fullx++)
		{
			i=fullx+Y;
			j=fullx+fullY;
			full[j]=full[i];
			full[j].a=aAMB;
		}
	}

	// bottom border
	for(fully=h-1,y=2*frame_border+2*smallh-h-1; fully>=smallh+frame_border; fully--,y++)
	{
		fullY=fully*w;
		Y=y*w;
		for(fullx=0; fullx<w; fullx++)
		{
			i=fullx+Y;
			j=fullx+fullY;
			full[j]=full[i];
			full[j].a=aAMB;
		}
	}
	UpdateBuffer(f);
}

void CCombineDoc::OnFrameTidyedges() 
{
	int f;
	str1.LoadString(IDS_STRING215);
	progress.AddString(str1);
	BeginWaitCursor();
	for(f=0; f<top_used_frame; f++)
	{
		TidyEdges(used_frame[f]);
	}
	EndWaitCursor();
	display=active_frame;
	RefreshScreen();
}


void CCombineDoc::OnSizeandalignmentAstronomical()
{
	CMaxAngle a;
	a.m_maxangle=maxangle;
	if(a.DoModal()==IDCANCEL) return;
	maxangle=a.m_maxangle;
	if(fix_active) flash_fixed();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	progress.Maximize();
	refreshscreen=true;
	AfxBeginThread(Astroalign,this);
}

UINT CCombineDoc::Astroalign(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->Astroalignt();
	return 0;
}

void CCombineDoc::Astroalignt()
{
	buissy=true;
	progress.AddString(IDS_ASTROALIGN,maxangle);
	progress.m_cancel=false;
	progress.SetRange(1,top_used_frame-1);

	int maxshift=w/4;
	if(h/4<maxshift) maxshift=h/4;

	int xx,yy,a,rsteps,best_score,score,w2=w/2,h2=h/2;
	double radius,angle,degrees,rstepsize;
	int ca,sa,doublex,doubley,doublexx,doubleyy;

	angle=2.0/100.0*(double)maxangle;
	if(angle>0.1)
	{
		radius=(double)h2; if(w2<h2) radius=(double)w2;
		degrees=angle * 2*3.142/360.0;
		rsteps=(int)(radius*degrees);
		rstepsize=degrees/(double)rsteps;
	} else {
		rsteps=2;
		rstepsize=0;
	}

	void *ptemp;
	PIXEL *frame;
	PIXEL *aframe; aframe=(PIXEL*)GetBuffer(active_frame);
	PIXEL *temp_frame; temp_frame=(PIXEL*)GetBuffer(TEMP_BUFFER);
	PIXEL *temp1_frame; temp1_frame=(PIXEL*)GetBuffer(TEMP_BUFFER+1);
	PIXEL *new_frame; new_frame=(PIXEL*)GetBuffer(TEMP_BUFFER+2);
	int r,f,test;
	int x,dx,newx,bestdx;
	int y,dy,newy,bestdy;
	int *dxsum; dxsum=new int[maxshift*2];
	int *dysum; dysum=new int[maxshift*2];
	int *rc0; rc0=new int[w];
	int *rc; rc=new int[w];
	int *rr0; rr0=new int[h];
	int *rr; rr=new int[h];
	int *lookupxx; lookupxx=new int[3*w];
	for(x=0; x<3*w; x++){xx=x; if(xx>w) xx-=w; if(xx>w) xx-=w; lookupxx[x]=xx;};
	int *lookupyy; lookupyy=new int[3*h];
	for(y=0; y<3*h; y++){yy=y; if(yy>h) yy-=h; if(yy>h) yy-=h; lookupyy[y]=yy;};

	int startx=maxshift+10;
	int endx=w-startx;
	int starty=maxshift+10;
	int endy=h-starty;

	for(x=startx; x<endx; x++)
	{
		rc0[x]=0;
		for(y=starty; y<endy; y++)
		{
			r=(int)aframe[x+y*w].r;
			if(r<10) continue;
			rc0[x]+=r;
		}
	}

	for(y=starty; y<endy; y++)
	{
		rr0[y]=0;
		for(x=startx; x<endx; x++)
		{
			r=(int)aframe[x+y*w].r;
			if(r<10) continue;
			rr0[y]+=r;
		}
	}

	for(f=0; f<last_used_frame; f++)
	{
		progress.SetPos(f);

		if(used_frame[f]==active_frame) continue;
		frame=(PIXEL*)GetBuffer(used_frame[f]);

		best_score=0x7fffffff;
		for(a=-rsteps/2; a<rsteps/2; a++)
		{
			angle=rstepsize*(double)a;
			ca=(int)(cos(angle)*2048.0); sa=(int)(sin(angle)*2048.0);
			for(x=0; x<w; x++)
			{
				doublex=2*(x-w2)+1;
				for(y=0; y<h; y++)
				{
					doubley=2*(y-h2)+1;
					doublexx=doublex*ca+doubley*sa;
					doubleyy=doubley*ca-doublex*sa;
					xx=lookupxx[((doublexx+2048)>>12)+w2+w];
					yy=lookupyy[((doubleyy+2048)>>12)+h2+h];
					temp_frame[x+w*y]=frame[xx+w*yy];
				}
			}

			for(x=0; x<w; x++) rc[x]=0;
			for(y=starty; y<endy; y++)
			{
				yy=y*w;
				for(x=0; x<w; x++)
				{
					r=(int)temp_frame[x+yy].r;
					if(r>10) rc[x]+=r;
				}
			}
			for(dx=-maxshift; dx<maxshift; dx++) // sum of difference bitween master and this col
			{
				xx=dx+maxshift;
				dxsum[xx]=0;
				for(x=startx; x<endx; x++)
				{
					newx=x+dx;
					dxsum[xx]+=abs(rc[newx]-rc0[x]);
				}
			}
			bestdx=0;
			for(x=-maxshift; x<maxshift; x++) // find lowest col difference
			{
				test=dxsum[x+maxshift];
				if(test<dxsum[bestdx+maxshift]) bestdx=x;
			}
			for(x=0; x<w; x++)
			{
				newx=lookupxx[x-bestdx+w];
				for(y=0; y<h; y++)
				{
					temp1_frame[newx+y*w]=temp_frame[x+y*w];
				}
			}

			for(y=0; y<h; y++)
			{
				rr[y]=0;
				yy=y*w;
				for(x=startx; x<endx; x++)
				{
					r=(int)temp1_frame[x+yy].r;
					if(r>10) rr[y]+=r;
				}
			}
			for(dy=-maxshift; dy<maxshift; dy++)
			{
				yy=dy+maxshift;
				dysum[yy]=0;
				for(y=starty; y<endy; y++)
				{
					newy=y+dy;
					dysum[yy]+=abs(rr[newy]-rr0[y]);
				}
			}
			bestdy=0;
			for(y=-maxshift; y<maxshift; y++)
			{
				test=dysum[y+maxshift];
				if(test<dysum[bestdy+maxshift]) bestdy=y;
			}
			for(y=0; y<h; y++)
			{
				newy=lookupyy[y-bestdy+h]*w;
				yy=y*w;
				for(x=0; x<w; x++)
				{
					temp_frame[x+newy]=temp1_frame[x+yy];
				}
			}

			score=0;
			for(y=starty; y<endy; y++)
			{
				yy=w*y;
				for(x=startx; x<endx; x++)
				{
					xx=x+yy;
					r=abs((int)temp_frame[xx].r-(int)aframe[xx].r);
					if(r>10) score+=r;
				}
			}

			if(score<best_score)
			{
				ptemp=buffer[TEMP_BUFFER].pointer;
				buffer[TEMP_BUFFER].pointer=buffer[TEMP_BUFFER+2].pointer;
				buffer[TEMP_BUFFER+2].pointer=ptemp;
				ptemp=temp_frame;
				temp_frame=new_frame;
				new_frame=(PIXEL *)ptemp;
				best_score=score;
				ptemp=memory_slot[buffer[TEMP_BUFFER].slot].pointer;
				memory_slot[buffer[TEMP_BUFFER].slot].pointer=memory_slot[buffer[TEMP_BUFFER+2].slot].pointer;
				memory_slot[buffer[TEMP_BUFFER+2].slot].pointer=ptemp;
			}
		}
		ptemp=buffer[used_frame[f]].pointer;
		buffer[used_frame[f]].pointer=buffer[TEMP_BUFFER+2].pointer;
		buffer[TEMP_BUFFER+2].pointer=ptemp;
		ptemp=frame;
		frame=new_frame;
		new_frame=(PIXEL *)ptemp;
		UpdateBuffer(used_frame[f]);
		ptemp=(PIXEL *)memory_slot[buffer[used_frame[f]].slot].pointer;
		memory_slot[buffer[used_frame[f]].slot].pointer=memory_slot[buffer[TEMP_BUFFER+2].slot].pointer;
		memory_slot[buffer[TEMP_BUFFER+2].slot].pointer=ptemp;
	}
	FreeBuffer(active_frame);
	FreeBuffer(TEMP_BUFFER);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+2);
	delete [] dxsum;
	delete [] dysum;
	delete [] rc0;
	delete [] rc;
	delete [] rr0;
	delete [] rr;
	delete [] lookupxx;
	delete [] lookupyy;
	progress.SetPos(0);
	if(refreshscreen)
	{
		display=active_frame;
		refreshscreen=false;
		progress.m_cancel=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
		RefreshScreen();
	}
}

void CCombineDoc::OnSizeandalignmentAuto()
{
	CMaxAngle a;
	a.m_maxangle=maxangle;
	if(a.DoModal()==IDCANCEL) return;
	maxangle=a.m_maxangle;
	if(fix_active) flash_fixed();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	progress.Maximize();
	refreshscreen=true;
	AfxBeginThread(shrotalign,this);
}

UINT CCombineDoc::shrotalign(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->shrotalignt();
	return 0;
}

struct rot_Params
{
	int index,step;
	int w,h;
	double angle;
	int *lookupxx,*lookupyy;
	PIXEL *in,*out;
} rot_params[32];

UINT CCombineDoc::rot(LPVOID param)
{
	rot_Params *pp=(rot_Params *)param;
	int index=pp->index;
	int step=pp->step;
	double angle=pp->angle;
	int *lookupxx=pp->lookupxx;
	int *lookupyy=pp->lookupyy;
	int w=pp->w;
	int h=pp->h;
	PIXEL *in=pp->in;
	PIXEL *out=pp->out;

	int x,y,xx,yy;
	int ca,sa,doublex,doubley,doublexx,doubleyy,w2=w/2,h2=h/2;
	int dxca,dxsa;
	int h2h=h2+h;
	int w2w=w2+w;

	ca=(int)(cos(angle)*2048.0); sa=(int)(sin(angle)*2048.0);
	for(x=index; x<w; x+=step)
	{
		doublex=2*(x-w2)+1;
		dxca=doublex*ca;
		dxsa=doublex*sa;
		for(y=0; y<h; y++)
		{
			doubley=2*(y-h2)+1;
			doublexx=dxca+doubley*sa;
			doubleyy=doubley*ca-dxsa;
			xx=lookupxx[((doublexx+1024)>>12)+w2w];
			yy=lookupyy[((doubleyy+1024)>>12)+h2h];
			out[x+w*y]=in[xx+w*yy];
		}
	}

	return 0;
}

void CCombineDoc::shrotalignt()
{
	BeginThreads

	CString s;

	if(last_used_frame<1) goto end;

	int min_half_size=MIN_HALF_SIZE;
	buissy=true;
	progress.AddString(IDS_ALIGNSRM,maxangle);
	progress.m_cancel=false;

	int f;
	int neww,newh,maxwh;
	PIXEL *f0,*f1,*fr,*sf0,*sf1,*tempsf;
	int xoff,yoff,newxoff,newyoff,bestxoff,bestyoff,xoffm,xoffp,yoffm,yoffp;
	float scale,scalem,scalep,newscale,scalestep,testscale,bestscale;
	double angle,bestangle,dangle;
	__int64 old_score,bestangle_score,angleminus_score,angleplus_score;
	int i,k,x,y;
	int	xx,yy;

	sf0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);
	sf1=(PIXEL*)GetBuffer(TEMP_BUFFER+1);
	fr=(PIXEL*)GetBuffer(TEMP_BUFFER+2);

	int *xxtable=new int[w];
	int *dxxtable=new int[w];
	bool *xxouttable=new bool[w];

	int *xofftable=new int[last_used_frame];
	int *yofftable=new int[last_used_frame];
	double *scaletable=new double[last_used_frame];
	double *angletable=new double[last_used_frame];
	xofftable[0]=yofftable[0]=0;
	scaletable[0]=1.0;
	angletable[0]=0.0;

	int *lookupxx; lookupxx=new int[3*w];
	for(x=0; x<3*w; x++){xx=x; if(xx>=w) xx-=w; if(xx>=w) xx-=w; lookupxx[x]=xx;};
	int *lookupyy; lookupyy=new int[3*h];
	for(y=0; y<3*h; y++){yy=y; if(yy>=h) yy-=h; if(yy>=h) yy-=h; lookupyy[y]=yy;};

	PIXEL *msubframe[32],*ssubframe[32];
	int level,levels;
	int width[32],height[32];

	__int64 test,besttest,besttest2;

	bool changed;

	// split master into subframes
	width[0]=w;
	height[0]=h;
	neww=w; newh=h;
	f0=(PIXEL*)GetBuffer(used_frame[0]);
	msubframe[0]=f0;
	msubframe[1]=sf0;
	level=1;
	while((neww>min_half_size) && (newh>min_half_size))
	{
		neww/=2;
		newh/=2;

		width[level]=neww;
		height[level]=newh;

		SetThreads(neww,newh)
		for(obj=0; obj<objects; obj++)
		{
			split_params[obj].bandw=bandw;
			split_params[obj].step=objects;
			split_params[obj].index=obj;
			split_params[obj].w=neww;
			split_params[obj].h=newh;
			split_params[obj].in=msubframe[level-1];
			split_params[obj].out=msubframe[level];
			StartThreads(split,split_params)
		}
		EndThreads

		level++;
		msubframe[level]=msubframe[level-1]+neww*newh;
	}
	levels=level;

	progress.SetRange(1,top_used_frame-1);
	for(f=used_frame[1]; f<last_used_frame; f++)
	{
		f1=(PIXEL*)GetBuffer(used_frame[f]);

		str1.LoadString(IDS_STRING201);
		s.Format(str1,buffer[used_frame[f]].name,buffer[used_frame[f-1]].name);
		progress.AddString(s);
		progress.SetPos(f);
		if(progress.m_cancel)
		{
			UpdateBuffer(used_frame[f-1]);
			goto finish;
		}

		bestxoff=bestyoff=0;
		bestscale=1.0;
		dangle=(7.284f*(float)maxangle)/36000.0f;
		bestangle=0.0f;
		bestangle_score=-1;
		old_score=1;
		angleminus_score=angleplus_score=0x1000000000000000;

		while((abs((int)(old_score-angleminus_score))>old_score/1000) || (abs((int)(old_score-angleplus_score))>old_score/1000))
		{
			old_score=bestangle_score;

			if(progress.m_cancel) break;

			for(angle=bestangle-dangle, k=-1; k<2; angle+=dangle, k++)
			{
				if(progress.m_cancel) break;

				if((k==0) && (bestangle_score!=-1)) continue;

				SetThreads(w,h)
				for(obj=0; obj<objects; obj++)
				{
					rot_params[obj].index=obj;
					rot_params[obj].step=objects;
					rot_params[obj].w=w;
					rot_params[obj].h=h;
					rot_params[obj].in=f1;
					rot_params[obj].out=fr;
					rot_params[obj].angle=angle;
					rot_params[obj].lookupxx=lookupxx;
					rot_params[obj].lookupyy=lookupyy;
					StartThreads(rot,rot_params)
				}
				EndThreads

				// split subject frame into subframes
				neww=w; newh=h;
				ssubframe[0]=fr;
				ssubframe[1]=sf1;
				level=1;
				while((neww>min_half_size) && (newh>min_half_size))
				{
					neww/=2;
					newh/=2;

					SetThreads(neww,newh)
					for(obj=0; obj<objects; obj++)
					{
						split_params[obj].bandw=bandw;
						split_params[obj].step=objects;
						split_params[obj].index=obj;
						split_params[obj].w=neww;
						split_params[obj].h=newh;
						split_params[obj].in=ssubframe[level-1];
						split_params[obj].out=ssubframe[level];
						StartThreads(split,split_params)
					}
					EndThreads

					level++;
					ssubframe[level]=ssubframe[level-1]+neww*newh;
				}

				xoff=yoff=0;
				scale=1.0f;

				for(level=levels-1; level>=0; level--)
				{
retry:				changed=true;
					while(changed)
					{
						changed=false;

						besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
						newxoff=xoff;
						for(i=-1; i<2; i++)
						{
							if(i==0) continue;
							test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],i+xoff,yoff,scale);
							if(test<besttest)
							{
								besttest=test;
								newxoff=i+xoff;
								changed=true;
							}
						}
						xoff=newxoff;

						if(changed) besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
						newyoff=yoff;
						for(i=-1; i<2; i++)
						{
							if(i==0) continue;
							test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,i+yoff,scale);
							if(test<besttest)
							{
								besttest=test;
								newyoff=i+yoff;
								changed=true;
							}
						}
						yoff=newyoff;
					}

					changed=false;

					maxwh=width[level]; if(height[level]>maxwh) maxwh=height[level];
					scalestep=(float)(maxwh+2)/(float)maxwh-1.0f;

					test=besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
					newscale=testscale=scale;
					while(test<=besttest)
					{
						testscale+=scalestep;
						test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,testscale);
						if(test<besttest)
						{
							besttest=test;
							newscale=testscale;
							changed=true;
						}
					}

					testscale=scale;
					test=besttest2=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
					while(test<=besttest2)
					{
						testscale-=scalestep;
						test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,testscale);
						if(test<besttest2)
						{
							besttest2=test;
							if(besttest2<besttest)
							{
								besttest=besttest2;
								newscale=testscale;
								changed=true;
							}
						}
					}

					scale=newscale;

					if(changed) goto retry;

					if(level!=0)
					{
						xoff*=2;
						yoff*=2;
					}
				} // level loop

				// final alignment

				changed=true;
				level=0;
				while(changed)
				{
					changed=false;

					besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
					newxoff=xoff;
					for(i=-1; i<2; i++)
					{
						if(i==0) continue;
						test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],i+xoff,yoff,scale);
						if(test<besttest)
						{
							besttest=test;
							newxoff=i+xoff;
							changed=true;
						}
					}
					xoff=newxoff;

					besttest=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
					newyoff=yoff;
					for(i=-1; i<2; i++)
					{
						if(i==0) continue;
						test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,i+yoff,scale);
						if(test<besttest)
						{
							besttest=test;
							newyoff=i+yoff;
							changed=true;
						}
					}
					yoff=newyoff;
				} // final align

				if((bestangle_score==-1) && (k==0))
				{
					bestangle_score=testshiftnscale2(fr,f0,w,h,xoff,yoff,scale);
					bestangle=0;
					bestxoff=xoff;
					bestyoff=yoff;
					bestscale=scale;
				}
				if(k==-1)
				{
					angleminus_score=testshiftnscale2(fr,f0,w,h,xoff,yoff,scale);
					xoffm=xoff;
					yoffm=yoff;
					scalem=scale;
				}
				if(k==1)
				{
					angleplus_score=testshiftnscale2(fr,f0,w,h,xoff,yoff,scale);
					xoffp=xoff;
					yoffp=yoff;
					scalep=scale;
				}
			} // +/- dangle loop

			if(theApp.debug_switch)
			{
				str1.LoadString(IDS_STRING657);
				s.Format(str1,
					(int)((bestangle-dangle)*36000.0/(2.0*3.14159)),
					(int)((bestangle)*36000.0/(2.0*3.14159)),
					(int)((bestangle+dangle)*36000.0/(2.0*3.14159)),
					(int)(angleminus_score/1000),(int)(bestangle_score/1000),(int)(angleplus_score/1000));
				progress.AddString(s);
			}

			old_score=bestangle_score;
			if(angleminus_score<bestangle_score)
			{
				bestangle_score=angleminus_score;
				bestangle=bestangle-dangle;
				bestxoff=xoffm;
				bestyoff=yoffm;
				bestscale=scalem;
			}
			if(angleplus_score<bestangle_score)
			{
				bestangle_score=angleplus_score;
				bestangle=bestangle+dangle;
				bestxoff=xoffp;
				bestyoff=yoffp;
				bestscale=scalep;
			}

			if(angleminus_score+angleplus_score!=2*bestangle_score)
			{
				double t=bestangle+0.5f*dangle*(angleminus_score-angleplus_score)/
					(angleminus_score+angleplus_score-2*bestangle_score);
				if((t>bestangle-dangle/2) && (t<bestangle+dangle/2))
				{
					if(abs(bestangle-t)<dangle/2) dangle/=2;
					bestangle=t;
				}
			}

			if(old_score!=bestangle_score) dangle*=2.0f;
			dangle/=2.0f;

			if((dangle*360000.0)/(2.0*3.14159)<1.0) break;
			if(bestangle>4500)
			{
				bestangle=0;
				bestxoff=0;
				bestyoff=0;
				bestscale=1.0;
				break;
			}
		} // angle loop
		UpdateBuffer(used_frame[f-1]);

		f0=f1;
		tempsf=sf0; sf0=sf1; sf1=tempsf;
		for(x=0; x<levels; x++) msubframe[x]=ssubframe[x];
		msubframe[0]=f0;

		xofftable[f]=bestxoff;
		yofftable[f]=bestyoff;
		scaletable[f]=bestscale;
		angletable[f]=bestangle;

		str1=L"  Results Xoffset=%d Yoffset=%d Scale=%f Angle=%d";
		s.Format(str1,-bestxoff,-bestyoff,1.0f/bestscale,(int)(bestangle*36000.0/(2.0*3.14159)));
		progress.AddString(s);
	} // frame loop
	UpdateBuffer(used_frame[f-1]);
finish:

	delete [] xxouttable;
	progress.SetPos(0);
	FreeBuffer(TEMP_BUFFER+2);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+0);

	sizexorg=w/2;
	sizeyorg=h/2;

	int save_active_frame=active_frame;
	int save_offsetx=offsetx;
	int save_offsety=offsety;
	int save_size_factor=size_factor;
	int save_angle=this->angle;

	int xsum,ysum;
	double scaleprod,anglesum;

	UseFrame(active_frame);
	if(save_active_frame<used_frame[last_used_frame-1])
	{
		xsum=0,ysum=0;
		scaleprod=1.0,anglesum=0.0;
		for(f=0; f<last_used_frame; f++)
		{
			active_frame=used_frame[f];
			if(active_frame<=save_active_frame) continue;

			if(progress.m_cancel) break;

			xsum+=xofftable[f];
			ysum+=yofftable[f];
			scaleprod*=scaletable[f];
			anglesum+=angletable[f];

			offsetx=xsum;
			offsety=ysum;
			size_factor=(int)(scaleprod*10000);
			this->angle=36000-(int)(anglesum*36000.0/(2.0*3.14159));

			sizecompensation();
		}
	}

	if(save_active_frame>used_frame[0])
	{
		xsum=0,ysum=0;
		scaleprod=1.0,anglesum=0.0;
		for(f=last_used_frame-1; f>=0; f--)
		{
			active_frame=used_frame[f];
			if(active_frame>=save_active_frame) continue;

			if(progress.m_cancel) break;

			xsum-=xofftable[f+1];
			ysum-=yofftable[f+1];
			scaleprod/=scaletable[f+1];
			anglesum-=angletable[f+1];

			offsetx=xsum;
			offsety=ysum;
			size_factor=(int)(scaleprod*10000);
			this->angle=36000-(int)(anglesum*36000.0/(2.0*3.14159));

			sizecompensation();
		}
	}

	for(f=0; f<last_used_frame;f++) TidyOuterEdges(used_frame[f]);

	active_frame=save_active_frame;
	offsetx=save_offsetx;
	offsety=save_offsety;
	size_factor=save_size_factor;
	this->angle=save_angle;

	delete [] xxtable;
	delete [] dxxtable;
	delete [] xofftable;
	delete [] yofftable;
	delete [] scaletable;
	delete [] angletable;
	delete [] lookupxx;
	delete [] lookupyy;
	progress.SetPos(0);
	str1.LoadString(IDS_STRING204);
end:
	if(refreshscreen)
	{
		display=active_frame;
		refreshscreen=false;
		progress.m_cancel=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
		RefreshScreen();
	}
}

UINT CCombineDoc::tss2(LPVOID param)
{
	tss_Params *p=(tss_Params *)param;
	bool bandw=p->bandw;
	int endx=p->endx;
	int endy=p->endy;
	int height=p->height;
	PIXEL *master=p->master;
	__int64 result=p->result;
	PIXEL *sample=p->sample;
	float scale=p->scale;
	int startx=p->startx;
	int starty=p->starty;
	int width=p->width;
	int xoffset=p->xoffset;
	int xstep=p->xstep;
	int yoffset=p->yoffset;
	int ystep=p->ystep;
	int cx=p->cx;
	int cy=p->cy;
	int r2=p->r2;
	int *xxtable=p->xxtable;

	if(xoffset>width/8){p->result=0x1000000000000000; return 1;}
	if(xoffset<-width/8){p->result=0x1000000000000000; return 1;}
	if(yoffset>height/8){p->result=0x1000000000000000; return 1;}
	if(yoffset<-height/8){p->result=0x1000000000000000; return 1;}
	if(scale>1.05){p->result=0x1000000000000000; return 1;}
	if(scale<0.95){p->result=0x1000000000000000; return 1;}

	int x,xx,y,yy,Y,YY,y2;
	int t;
	PIXEL s,m;
	result=0;

	if(bandw)
	{
		for(y=starty; y<endy; y+=ystep)
		{
			Y=width*y;
			yy=(int)(((float)(y-yoffset)-(float)(height/2))*scale+(float)(height/2)+0.5);
			if(yy<2) yy=2;
			if(yy>(17*endy)/16) yy=(17*endy)/16;
			YY=yy*width;
			y2=(y-cy)*(y-cy);
			for(x=startx; x<endx; x+=xstep)
			{
				if((x-cx)*(x-cx)+y2>r2) continue;

				xx=xxtable[x];

				s=sample[xx+YY];
				m=master[x+Y];

				t=(int)s.r-(int)m.r; result+=(__int64)(t*t);
			}
		}
	} else {
		for(y=starty; y<endy; y+=ystep)
		{
			Y=width*y;
			yy=(int)(((float)(y-yoffset)-(float)(height/2))*scale+(float)(height/2)+0.5);
			if(yy<2) yy=2;
			if(yy>(17*endy)/16) yy=(17*endy)/16;
			YY=yy*width;
			y2=(y-cy)*(y-cy);
			for(x=startx; x<endx; x+=xstep)
			{
				if((x-cx)*(x-cx)+y2>r2) continue;

				xx=xxtable[x];

				s=sample[xx+YY];
				m=master[x+Y];

				t=(int)((unsigned int)s.r)-(int)((unsigned int)m.r); result+=(__int64)((t*t)/16);
				t=(int)((unsigned int)s.g)-(int)((unsigned int)m.g); result+=(__int64)((t*t)/16);
				t=(int)((unsigned int)s.b)-(int)((unsigned int)m.b); result+=(__int64)((t*t)/16);
			}
		}
	}
	p->result=result;
	return 0;
}

__int64 CCombineDoc::testshiftnscale2(PIXEL *sample,PIXEL *master,int width,int height,int xoffset,int yoffset,float scale)
{
	__int64 result=0;
	unsigned int x;
	int startx,endx,starty,endy;
	int xstep,ystep;

	xstep=width/1000; if(xstep<2) xstep=2;
	ystep=height/1000; if(ystep<2) ystep=2;

	int cx=width/2;
	int cy=height/2;

	cx-=width/32;
	cy-=height/32;

	int r2,i,xx;
	if(width>height) r2=cy-height/16;
	else r2=cx-width/16;

	startx=cx-r2;
	endx=cx+r2;
	starty=cy-r2;
	endy=cy+r2;
	r2*=r2;

	int *xxtable=new int[width];
	for(i=0; i<width; i++)
	{
		xx=(int)(((float)(i-xoffset)-(float)(width/2))*scale+(float)(width/2)+0.5);
		if(xx<2) xx=2;
		if(xx>(17*endx)/16) xx=(17*endx)/16;
		xxtable[i]=xx;
	}

	scale=1.0f/scale;

	BeginThreads
	SetThreads(width,height)
	for(obj=0; obj<objects; obj++)
	{
		tss_params[obj].bandw=bandw;
		tss_params[obj].starty=starty+ystep*obj;
		tss_params[obj].endy=endy;
		tss_params[obj].ystep=ystep*objects;
		tss_params[obj].startx=startx;
		tss_params[obj].endx=endx;
		tss_params[obj].xstep=xstep;
		tss_params[obj].width=width;
		tss_params[obj].height=height;
		tss_params[obj].xoffset=xoffset;
		tss_params[obj].yoffset=yoffset;
		tss_params[obj].scale=scale;
		tss_params[obj].sample=sample;
		tss_params[obj].master=master;
		tss_params[obj].result=-1;
		tss_params[obj].cx=cx;
		tss_params[obj].cy=cy;
		tss_params[obj].r2=r2;
		tss_params[obj].xxtable=xxtable;
		StartThreads(tss2,tss_params)
	}
	EndThreads
	result=0;
	for(x=0; x<objects; x++)
		result+=tss_params[x].result;

	delete [] xxtable;
	return result;
}
