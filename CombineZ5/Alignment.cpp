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

// MOVE FRAME (or sometimes picture) UP DOWN LEFT & RIGHT

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
		MessageBox(0,"Shift is on.  You can only move a Frame!","Move Frame",MB_OK);
		return;
	}

	offsety--;

	int i,j,x,y,Yi,Yj;
	unsigned int a;

	PIXEL* p=(PIXEL*)GetBuffer(active_frame);
	PIXEL* temp=(PIXEL*)GetBuffer(TEMP_BUFFER);

	for(i=0,j=0; j<w; i++,j++)
		temp[j]=p[i]; // save top row

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
		MessageBox(0,"Shift is on.  You can only move a Frame!","Move Frame",MB_OK);
		return;
	}

	offsety++;

	int i,j,x,y,Yi,Yj;
	unsigned int a;

	PIXEL* p=(PIXEL*)GetBuffer(active_frame);
	PIXEL* temp=(PIXEL*)GetBuffer(TEMP_BUFFER);

	for(i=w*(h-1)+w-1, j=w-1; j>=0; i--, j--) temp[j]=p[i];

	for(y=h-1; y>0; y--)
	{
		Yi=y*w;
		Yj=(y-1)*w;
		for(x=frame_border; x<smallw+frame_border; x++)
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
		MessageBox(0,"Shift is on.  You can only move a Frame!","Move Frame",MB_OK);
		return;
	}

	offsetx++;

	int i,j,x,y,Yi;
	unsigned int a;

	PIXEL* p=(PIXEL*)GetBuffer(active_frame);
	PIXEL* temp=(PIXEL*)GetBuffer(TEMP_BUFFER);

	for(i=w-1, j=0; j<h; i+=w, j++) temp[j]=p[i];

	for(y=0; y<h; y++)
	{
		Yi=y*w;
		for(x=w-1; x>0; x--)
		{
			i=x+Yi;
			j=x-1+Yi;
			p[i]=p[j];
		}
	}

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
		MessageBox(0,"Shift is on.  You can only move a Frame!","Move Frame",MB_OK);
		return;
	}

	offsetx--;

	int i,j,x,y,Yi;
	unsigned int a;

	PIXEL* p=(PIXEL*)GetBuffer(active_frame);
	PIXEL* temp=(PIXEL*)GetBuffer(TEMP_BUFFER);

	for(i=0, j=0; j<h; i+=w, j++) temp[j]=p[i];

	for(y=0; y<h; y++)
	{
		Yi=y*w;
		for(x=0; x<w-1; x++)
		{
			i=x+Yi;
			j=x+1+Yi;
			p[i]=p[j];
		}
	}

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

unsigned long CCombineDoc::testshiftnscale(PIXEL *sample,PIXEL *master,int width,int height,int xoffset,int yoffset,float scale)
{
	unsigned long result=0;
	int t;
	PIXEL s,m;
	int x,xx,y,yy,Y,YY;
	int startx,endx,starty,endy;
	int *xxtable=new int[width];
	int xstep,ystep;

	xstep=width/1000+1;
	ystep=height/1000+1;

	startx=width/32+w-smallw;
	endx=width-startx;
	starty=height/32+h-smallh;
	endy=height-starty;

	scale=1.0f/scale;

	for(x=0; x<width; x++)
	{
		xx=(int)(((float)(x-xoffset)-(float)(width/2))*scale+(float)(width/2)+0.5);
		if(xx<width/16) xx=width/16;
		if(xx>(15*width)/16) xx=(15*width)/16;
		xxtable[x]=xx;
	}

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

			t=(int)s.r-(int)m.r; result+=(unsigned long)(t*t);
			t=(int)s.g-(int)m.g; result+=(unsigned long)(t*t);
			t=(int)s.b-(int)m.b; result+=(unsigned long)(t*t);
		}
		if(result>0xf0000000) result-=0xf0000000;
	}
	delete xxtable;
	return result;
}

UINT CCombineDoc::Autoalignandsize(LPVOID param) // single pass
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->Autoalignandsizet();
	return 0;
}

void CCombineDoc::Autoalignandsizet()
{
	int min_half_size=MIN_HALF_SIZE;
	buissy=true;
	progress.AddString(IDS_AUTOALIGN1);
	progress.m_cancel=false;

	int f,master,Master,direction,count;
	int neww,newh,wmultiplier,hmultiplier,maxwh;
	PIXEL *f0,*f1,*sf0,*sf1,*src,*dest,*tempsf;
	int xoff,yoff,newxoff,newyoff;
	float scale,newscale,scalestep,testscale;
	int i,j,x,y,Y;
	int dxx,dyy;
	int	xx,yy;
	float xf,yf;
	PIXEL p,p0,p1,p2,p3;
	int c00,c01,c10,c11;
	int r00,r01,r10,r11;
	int g00,g01,g10,g11;
	int b00,b01,b10,b11;
	int a,r,g,b;

	sf0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);
	sf1=(PIXEL*)GetBuffer(TEMP_BUFFER+1);

	int *xxtable=new int[w];
	int *dxxtable=new int[w];
	bool *xxouttable=new bool[w];
	bool yyout;

	int *xofftable=new int[last_used_frame];
	int *yofftable=new int[last_used_frame];
	float *scaletable=new float[last_used_frame];

	PIXEL *msubframe[10],*ssubframe[10];
	int level,levels;
	int width[10],height[10];

	unsigned long test,besttest;

	bool changed;

	CString s;

	// split master into subframes
	f0=(PIXEL*)GetBuffer(used_frame[0]);
	level=levels=0;
	msubframe[level]=f0;
	width[level]=w;
	height[level]=h;
	level++;
	neww=w; newh=h; i=0;
	src=f0; dest=sf0;
	while((neww>min_half_size) && (newh>min_half_size))
	{
		if(neww>min_half_size){neww/=2; wmultiplier=1;} else wmultiplier=0;
		if(newh>min_half_size){newh/=2; hmultiplier=1;} else hmultiplier=0;

		msubframe[level]=dest;
		width[level]=neww;
		height[level]=newh;
		level++;

		for(y=0; y<newh; y++)
		{
			for(x=0; x<neww; x++)
			{
				j=(1+wmultiplier)*(x+(1+hmultiplier)*y*neww);
				p.abgr=0;
				p0=src[j];
				p1=src[j+1];
				p2=src[j+neww*(1+wmultiplier)];
				p3=src[j+1+(1+wmultiplier)*neww];
				p.r=(unsigned char)(((int)(p0.r)+(int)(p1.r)+(int)(p2.r)+(int)(p3.r))/4);
				p.g=(unsigned char)(((int)(p0.g)+(int)(p1.g)+(int)(p2.g)+(int)(p3.g))/4);
				p.b=(unsigned char)(((int)(p0.b)+(int)(p1.b)+(int)(p2.b)+(int)(p3.b))/4);
				sf0[i++]=p;
			}
		}
		src=dest;
		dest=&sf0[i];
	}
	levels=level;

	scale=1.0f;
	xoff=0; yoff=0;
	
	progress.SetRange(1,top_used_frame-1);
	for(f=1; f<last_used_frame; f++)
	{
		s.Format("   Compare Frame %s with %s",buffer[used_frame[f]].name,buffer[used_frame[f-1]].name);
		progress.AddString(s);
		progress.SetPos(f);
		if(progress.m_cancel)
		{
			UpdateBuffer(used_frame[f-1]);
			goto finish;
		}

		// split subject frame into subframes
		f1=(PIXEL*)GetBuffer(used_frame[f]);
		level=0; 
		ssubframe[level]=f1; level++;
		neww=w; newh=h; i=0;
		src=f1; dest=sf1;
		while((neww>min_half_size) && (newh>min_half_size))
		{
			if(neww>min_half_size){neww/=2; wmultiplier=1;} else wmultiplier=0;
			if(newh>min_half_size){newh/=2; hmultiplier=1;} else hmultiplier=0;

			ssubframe[level]=dest; level++;

			for(y=0; y<newh; y++)
			{
				for(x=0; x<neww; x++)
				{
					j=(1+wmultiplier)*(x+(1+hmultiplier)*y*neww);
					p.abgr=0;
					p0=src[j];
					p1=src[j+1];
					p2=src[j+neww*(1+wmultiplier)];
					p3=src[j+1+(1+wmultiplier)*neww];
					p.r=(unsigned char)(((int)(p0.r)+(int)(p1.r)+(int)(p2.r)+(int)(p3.r))/4);
					p.g=(unsigned char)(((int)(p0.g)+(int)(p1.g)+(int)(p2.g)+(int)(p3.g))/4);
					p.b=(unsigned char)(((int)(p0.b)+(int)(p1.b)+(int)(p2.b)+(int)(p3.b))/4);
					sf1[i++]=p;
				}
			}
			src=dest;
			dest=&sf1[i];
		}

		xoff=yoff=0;
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
			if(!changed)
			{
				testscale-=scalestep;
				test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
				test--;
				while(test<besttest)
				{
					testscale-=scalestep;
					test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,testscale);
					if(test<besttest)
					{
						besttest=test;
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
		msubframe[0]=f0;

		xofftable[f]=xoff;
		yofftable[f]=yoff;
		scaletable[f]=scale;

		s.Format("      xoffset=%d   yoffset=%d   scale=%0.4f",-xoff,-yoff,1.0f/scale);
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

	progress.AddString("   Actual Scale and Size");
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

			for(y=frame_border; y<frame_border+smallh; y++)
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
				for(x=frame_border; x<frame_border+smallw; x++)
				{
					xx=xxtable[x];

					dxx=dxxtable[x];

					a=0;
					if((yyout) || (xxouttable[x])) a=aAMB;

					p=sf1[(xx+0)+w*(yy+0)];
					r00=(int)p.r; g00=(int)p.g; b00=(int)p.b; a|=p.a;

					p=sf1[(xx+1)+w*(yy+0)];
					r10=(int)p.r; g10=(int)p.g; b10=(int)p.b; a|=p.a;

					p=sf1[(xx+1)+w*(yy+1)];
					r11=(int)p.r; g11=(int)p.g; b11=(int)p.b; a|=p.a;

					p=sf1[(xx+0)+w*(yy+1)];
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

					f1[x+Y]=p;
				}
			}
			UpdateBuffer(used_frame[f]);
		}

		if(direction==1) break; 
		direction=1;
	}
finish:
	for(f=0; f<last_used_frame;f++) TidyOuterEdges(used_frame[f]);
	delete xxouttable;
	progress.SetPos(count);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+0);

	delete xxtable;
	delete dxxtable;
	delete xofftable;
	delete yofftable;
	delete scaletable;
	progress.SetPos(0);
	if(!changed) progress.AddString("*** No changes were made to any Frame ***");
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
	buissy=true;

	progress.AddString(IDS_AUTOALIGN2);

	int pass;
	int f,master,Master,direction,count;
	int neww,newh,wmultiplier,hmultiplier,maxwh;
	PIXEL *f0,*f1,*sf0,*sf1,*src,*dest,*tempsf;
	int xoff,yoff,newxoff,newyoff;
	float scale,newscale,scalestep,testscale,scale_factor;
	int i,j,x,y,Y;
	int dxx,dyy;
	int	xx,yy;
	float xf,yf;
	PIXEL p,p0,p1,p2,p3;
	int c00,c01,c10,c11;
	int r00,r01,r10,r11;
	int g00,g01,g10,g11;
	int b00,b01,b10,b11;
	int a,r,g,b;

	sf0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);
	sf1=(PIXEL*)GetBuffer(TEMP_BUFFER+1);

	int *xxtable=new int[w];
	int *dxxtable=new int[w];

	int *xofftable=new int[last_used_frame];
	int *yofftable=new int[last_used_frame];
	bool *xxouttable=new bool[w];
	bool yyout;

	float *scaletable=new float[last_used_frame];
	float *scalevalues=new float[last_used_frame];
	int *scalehist=new int[last_used_frame];
	int bins,scale_count;

	PIXEL *msubframe[10],*ssubframe[10];
	int level,levels;
	int width[10],height[10];

	unsigned long test,besttest;

	bool changed;

	CString s;

	for(pass=0; pass<2; pass++)
	{
		if(pass==0) progress.AddString("   First Pass");
		else progress.AddString("   Second Pass");
		// split master into subframes
		f0=(PIXEL*)GetBuffer(used_frame[0]);
		level=levels=0;
		msubframe[level]=f0;
		width[level]=w;
		height[level]=h;
		level++;
		neww=w; newh=h; i=0;
		src=f0; dest=sf0;
		while((neww>MIN_HALF_SIZE) && (newh>MIN_HALF_SIZE))
		{
			if(neww>MIN_HALF_SIZE){neww/=2; wmultiplier=1;} else wmultiplier=0;
			if(newh>MIN_HALF_SIZE){newh/=2; hmultiplier=1;} else hmultiplier=0;

			msubframe[level]=dest;
			width[level]=neww;
			height[level]=newh;
			level++;

			for(y=0; y<newh; y++)
			{
				for(x=0; x<neww; x++)
				{
					j=(1+wmultiplier)*(x+(1+hmultiplier)*y*neww);
					p.abgr=0;
					p0=src[j];
					p1=src[j+1];
					p2=src[j+neww*(1+wmultiplier)];
					p3=src[j+1+(1+wmultiplier)*neww];
					p.r=(unsigned char)(((int)(p0.r)+(int)(p1.r)+(int)(p2.r)+(int)(p3.r))/4);
					p.g=(unsigned char)(((int)(p0.g)+(int)(p1.g)+(int)(p2.g)+(int)(p3.g))/4);
					p.b=(unsigned char)(((int)(p0.b)+(int)(p1.b)+(int)(p2.b)+(int)(p3.b))/4);
					sf0[i++]=p;
				}
			}
			src=dest;
			dest=&sf0[i];
		}
		levels=level;

		scale=1.0f;
//		xoff=0; yoff=0;
		
		progress.SetRange(1,top_used_frame-1);
		for(f=1; f<last_used_frame; f++)
		{
			s.Format("      Compare Frame %s with %s",buffer[used_frame[f]].name,buffer[used_frame[f-1]].name);
			progress.AddString(s);
			progress.SetPos(f);
			if(progress.m_cancel)
			{
				UpdateBuffer(used_frame[f-1]);
				goto finish;
			}

			// split subject frame into subframes
			f1=(PIXEL*)GetBuffer(used_frame[f]);
			level=0; 
			ssubframe[level]=f1; level++;
			neww=w; newh=h; i=0;
			src=f1; dest=sf1;
			while((neww>MIN_HALF_SIZE) && (newh>MIN_HALF_SIZE))
			{
				if(neww>MIN_HALF_SIZE){neww/=2; wmultiplier=1;} else wmultiplier=0;
				if(newh>MIN_HALF_SIZE){newh/=2; hmultiplier=1;} else hmultiplier=0;

				ssubframe[level]=dest; level++;

				for(y=0; y<newh; y++)
				{
					for(x=0; x<neww; x++)
					{
						j=(1+wmultiplier)*(x+(1+hmultiplier)*y*neww);
						p.abgr=0;
						p0=src[j];
						p1=src[j+1];
						p2=src[j+neww*(1+wmultiplier)];
						p3=src[j+1+(1+wmultiplier)*neww];
						p.r=(unsigned char)(((int)(p0.r)+(int)(p1.r)+(int)(p2.r)+(int)(p3.r))/4);
						p.g=(unsigned char)(((int)(p0.g)+(int)(p1.g)+(int)(p2.g)+(int)(p3.g))/4);
						p.b=(unsigned char)(((int)(p0.b)+(int)(p1.b)+(int)(p2.b)+(int)(p3.b))/4);
						sf1[i++]=p;
					}
				}
				src=dest;
				dest=&sf1[i];
			}

			xoff=yoff=0;
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
				if(!changed)
				{
					testscale-=scalestep;
					test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,scale);
					test--;
					while(test<besttest)
					{
						testscale-=scalestep;
						test=testshiftnscale(ssubframe[level],msubframe[level],width[level],height[level],xoff,yoff,testscale);
						if(test<besttest)
						{
							besttest=test;
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
			if(pass==1)
			{
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
			}
			UpdateBuffer(used_frame[f-1]);

			f0=f1;
			tempsf=sf0; sf0=sf1; sf1=tempsf;
			msubframe[0]=f0;

			xofftable[f]=xoff;
			yofftable[f]=yoff;
			scaletable[f]=scale;

			if(pass==1) s.Format("         xoffset=%d   yoffset=%d   scale=%0.4f",-xoff,-yoff,1.0f/scale);
			else s.Format("         scale=%0.4f",1.0f/scale);
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
			s.Format("      Scale Using Average factor=%0.4f",1.0f/scale_factor);
			progress.AddString(s);
			progress.SetRange(0,top_used_frame-1);
		} else {
			s.Format("      Final Shift and Scale");
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
				}
				else
				{
					if(pass==1)
					{
						xoff-=xofftable[f+1];
						yoff-=yofftable[f+1];
					}
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

				for(y=frame_border; y<frame_border+smallh; y++)
				{
					Y=w*y;
					yf=((float)(y-yoff)-(float)(h/2))*scale+(float)(h/2);
					yy=(int)yf;
					yyout=false;
					if(yy<frame_border)
					{
						if(y>goodrect.top) goodrect.top=y;
						yy=frame_border;
						yf=(float)yy;
						yyout=true;
					}
					if(yy>frame_border+smallh-2)
					{
						if(y<goodrect.bottom) goodrect.bottom=y;
						yy=frame_border+smallh-2;
						yf=(float)yy;
						yyout=true;
					}
					dyy=(int)((yf-(float)yy)*(float)0x800);	
					for(x=frame_border; x<frame_border+smallw; x++)
					{
						xx=xxtable[x];

						dxx=dxxtable[x];

						a=0;
						if((yyout) || (xxouttable[x])) a=aAMB;

						p=sf1[(xx+0)+w*(yy+0)];
						r00=(int)p.r; g00=(int)p.g; b00=(int)p.b; a|=(int)p.a;

						p=sf1[(xx+1)+w*(yy+0)];
						r10=(int)p.r; g10=(int)p.g; b10=(int)p.b; a|=(int)p.a;

						p=sf1[(xx+1)+w*(yy+1)];
						r11=(int)p.r; g11=(int)p.g; b11=(int)p.b; a|=(int)p.a;

						p=sf1[(xx+0)+w*(yy+1)];
						r01=(int)p.r; g01=(int)p.g; b01=(int)p.b; a|=(int)p.a;

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

						f1[x+Y]=p;
					}
				}
				TidyOuterEdges(used_frame[f]);
				UpdateBuffer(used_frame[f]);
			}

			if(direction==1) break; 
			direction=1;
		}
		progress.SetPos(count);
	}
finish:
//	for(f=0; f<last_used_frame;f++) TidyOuterEdges(used_frame[f]);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+0);

	delete xxtable;
	delete dxxtable;
	delete xofftable;
	delete yofftable;
	delete scaletable;
	delete scalevalues;
	delete scalehist;
	delete xxouttable;
	progress.SetPos(0);
	if(!changed) progress.AddString("*** No further changes were made to any Frame ***");
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

// SIZE

// resize/rotate/*move
void CCombineDoc::OnSizecompensation() 
{
	if(!last_used_frame) return;
	if(mode&GROUP_MASK==SPECIAL) return;

	if(!positionparams()) return;

	mode=SIZE;

	sizecompensation();

	Redraw(0,false);
}

// public function
void CCombineDoc::Size()
{
	OnSizecompensation();
}

// the resize/rotate/move calculation, floating point with interpolation which I hope is the right way round
void CCombineDoc::sizecompensation()
{
	progress.AddString("Resize Rotate Move");
	progress.AddString("   "+buffer[active_frame].name);

	PIXEL* buf=(PIXEL*)GetBuffer(TEMP_BUFFER);

	double f=((double)size_factor)/10000.0;
	f=1.0/f;

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
	PIXEL p,*f1;
	bool notgood;

	f1=(PIXEL*)GetBuffer(active_frame);

	BeginWaitCursor();

	for(xs=0; xs<w; xs++)
		for(ys=0; ys<h; ys++)
			buf[xs+ys*w]=f1[xs+ys*w];

	for(xd=0; xd<w; xd++)
	{
		for(yd=0; yd<h; yd++)
		{
			// convert dest coords to double and adjust for centre
			xf=f*((double)xd-(double)(int)sizexorg);
			yf=f*((double)yd-(double)(int)sizeyorg);

			// calc floating point source,adjusted for centre and displacement
			xsf=cosa*xf+sina*yf+(double)(int)sizexorg-(double)offsetx;
			ysf=cosa*yf-sina*xf+(double)(int)sizeyorg-(double)offsety;

			// roll pixels over from oposite edge when needed
			notgood=false;
			while(xsf<0.0){notgood=true; xsf+=(double)w;}
			while(xsf>=(double)w){notgood=true; xsf-=(double)w;}
			xs=(unsigned int)(int)xsf;
			while(ysf<0.0){notgood=true; ysf+=(double)h;}
			while(ysf>=(double)h){notgood=true; ysf-=(double)h;}
			ys=(unsigned int)(int)ysf;

			aa=0;

			// get pixel from surrounding corners corners, allowing for rollovers
			p=buf[xs+w*ys];
			r00=(double)(int)p.r; g00=(double)(int)p.g; b00=(double)(int)p.b; aa|=p.a;
			tempx=xs+1;
			if(tempx>=w) tempx-=w;

			p=buf[tempx+w*ys];
			r10=(double)(int)p.r; g10=(double)(int)p.g; b10=(double)(int)p.b; aa|=p.a;
			tempx=xs+1;
			if(tempx>=w) tempx-=w;

			tempy=ys+1;
			if(tempy>=h) tempy-=h;

			p=buf[tempx+w*tempy];
			r11=(double)(int)p.r; g11=(double)(int)p.g; b11=(double)(int)p.b; aa|=p.a;
			tempy=ys+1;
			if(tempy>=h) tempy-=h;

			p=buf[xs+w*tempy];
			r01=(double)(int)p.r; g01=(double)(int)p.g; b01=(double)(int)p.b; aa|=p.a;

			// find fraction part of coords
			dxx=xsf-(double)(int)xsf; dyy=ysf-(double)(int)ysf;

			// calculate interpolation constants, just a guess but I have had no complaints
			dxx*=dxx; dyy*=dyy;
			dxx*=dxx; dyy*=dyy;
			dxx*=dxx; dyy*=dyy;
			c00=(1.0-dxx)*(1.0-dyy);
			c10=dxx*(1.0-dyy);
			c11=dxx*dyy;
			c01=1.0-(c00+c10+c11);

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

			if(notgood) aa|=aAMB;
			p.a=(unsigned char)aa;

			f1[xd+yd*w]=p;
		}
	}

	EndWaitCursor();
	OnClear();
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
				MessageBox(0,"P1 and P2, or P3 and P4 Lie on different Frames","Error",MB_OK);
				return;
			}

			if(Sizer.length12==0 || Sizer.length34==0)
			{
				MessageBox(0,"P1 and P2, or P3 and P4 are the same Point","Error",MB_OK);
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
			MessageBox(0,"Invalid combination of Points: P1 Origin; P1,P3 Origin & Offset; P1,P2,P3,P4 Origin, Offset, Angle & Scale.","Error",MB_OK);
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

void CCombineDoc::resizeall()
{
	buissy=true;

	progress.AddString(IDS_RESIZEALL);
	progress.m_cancel=false;

	int a;

	double r,g,b;
	double r00,g00,b00;
	double r10,g10,b10;
	double r11,g11,b11;
	double r01,g01,b01;

	double xf,yf;
	double c00,c10,c11,c01;
	double dxx,dyy;
	int xx,yy;
	int i,j;
	int x,y,Y;
	PIXEL p,*f1;

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

		for(y=0; y<h; y++)
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
	progress.AddString("Tidy Edges");
	BeginWaitCursor();
	for(f=0; f<top_used_frame; f++)
	{
		TidyEdges(used_frame[f]);
//		TidyOuterEdges(used_frame[f]);
	}
	EndWaitCursor();
	display=active_frame;
	RefreshScreen();
}
