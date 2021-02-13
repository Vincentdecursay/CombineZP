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

#include "SPECTRUM.h"

void CCombineDoc::OnFrameSqueezespectrum()
{
	if(!top_frame) return;

	int a0=Squeeze_redend,a1=Squeese_bluend,p=Squeeze_break;

	if(!macro_mode)
	{
		CSPECTRUM dlg;
		dlg.m_redend=a0;
		dlg.m_blueend=a1;
		dlg.m_break=p;
		int ret;
		ret=(int)dlg.DoModal();
		if(ret!=IDOK) return;
		Squeeze_break=p=dlg.m_break;
		Squeeze_redend=a0=dlg.m_redend;
		Squeese_bluend=a1=dlg.m_blueend;
		BeginWaitCursor();
	}

	int x,y,Y,i;
	int r,g,b;
	int r0,g0,b0;
	int j,min,max;

	int R[3*510],G[3*510],B[3*510];

	progress.AddString(IDS_ADJUSTSPECTRUM);

	for(i=0; i<510; i++)
	{
		if((i>=0) && (i<255))
		{
			R[i]=255;
			G[i]=i;
			B[i]=0;

			R[i+510]=0;
			G[i+510]=255;
			B[i+510]=i;

			R[i+1020]=i;
			G[i+1020]=0;
			B[i+1020]=255;
		} else {
			j=i-255;

			R[i]=255-j;
			G[i]=255;
			B[i]=0;

			R[i+510]=0;
			G[i+510]=255-j;
			B[i+510]=255;

			R[i+1020]=255;
			G[i+1020]=0;
			B[i+1020]=255-j;
		}
	}

	PIXEL *f0=(PIXEL*)GetBuffer(active_frame);

	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER);

	for(y=0; y<(int)h; y++)
	{
		Y=y*(int)w;
		for(x=0; x<(int)w; x++)
		{
			i=x+Y;
			out[i].a=f0[i].a;

			r0=(int)f0[i].r;
			g0=(int)f0[i].g;
			b0=(int)f0[i].b;

			min=r0; if(g0<min) min=g0; if(b0<min) min=b0;
			r0-=min; g0-=min; b0-=min;
			max=r0; if(g0>max) max=g0; if(b0>max) max=b0;

			if(max) {r0=(255*r0)/max; g0=(255*g0)/max; b0=(255*b0)/max;}

			if(r0==0)
			{
				if(g0>=b0) j=510+b0;
				else j=510+255+(255-g0);
			}

			if(g0==0)
			{
				if(b0>=r0) j=1020+r0;
				else j=1020+255+(255-b0);
			}

			if(b0==0)
			{
				if(r0>=g0) j=0+g0;
				else j=0+255+(255-r0);
			}

			j+=p; if(j>=1530) j-=1530;

			if(a1>a0)
			{
				j=(j*(a1-a0))/1529+a0;
				if(j<a0) j+=(a1-a0);
				if(j>a1) j-=(a1-a0);
			} else {
				j=1529-j;
				j=(j*(a0-a1))/1529+a1;
				if(j<a1) j+=(a0-a1);
				if(j>a0) j-=(a0-a1);
			}

			r=(max*R[j])/255+min;
			g=(max*G[j])/255+min;
			b=(max*B[j])/255+min;

			out[i].r=(unsigned char)r;
			out[i].g=(unsigned char)g;
			out[i].b=(unsigned char)b;
		}
	}
	UpdateBuffer(OUT_BUFFER);
	UpdateBuffer(active_frame);

	display=OUT_BUFFER;

	if(!macro_mode)
	{
		EndWaitCursor();
		RefreshScreen();
	}
}

#include "ExtractRange.h"

void CCombineDoc::OnFrameExtractcolourrange()
{
	if(!top_frame) return;

	int a0=Extract_redend,a1=Extract_bluend,th=Extract_th;

	if(!macro_mode)
	{
		CExtractRange dlg;
		dlg.m_redend=a0;
		dlg.m_blueend=a1;
		dlg.m_th=th;
		int ret;
		ret=(int)dlg.DoModal();
		if(ret!=IDOK) return;
		Extract_th=th=dlg.m_th;
		Extract_redend=a0=dlg.m_redend;
		Extract_bluend=a1=dlg.m_blueend;
		BeginWaitCursor();
	}

	while(a0<0) a0+=1530; while(a0>1530) a0-=1530;
	while(a1<0) a1+=1530; while(a1>1530) a1-=1530;

	int x,y,Y,i;
	int r,g,b;
	int r0,g0,b0;
	int j,min,max;

	BeginWaitCursor();

	progress.AddString(IDS_EXTRACTRANGE,a0,a1,th);

	int *test;
	test=new int[w*h];
	for(y=1; y<(int)h-1; y++)
	{
		Y=y*(int)w;
		for(x=1; x<(int)w-1; x++)
		{
			i=x+Y;
			test[i]=0;
		}
	}

	PIXEL *f0=(PIXEL*)GetBuffer(display);

	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER);

	for(y=0; y<(int)h; y++)
	{
		Y=y*(int)w;
		for(x=0; x<(int)w; x++)
		{
			i=x+Y;
			out[i].a=f0[i].a|aCLEAR;

			r0=(int)f0[i].r;
			g0=(int)f0[i].g;
			b0=(int)f0[i].b;

			r=g=b=0;

			min=r0; if(g0<min) min=g0; if(b0<min) min=b0;
			r0-=min; g0-=min; b0-=min;
			max=r0; if(g0>max) max=g0; if(b0>max) max=b0;

			if((max<th) && (max>=0)) goto skip;
			if((max<th) && (max<0))

			if(max) {r0=(255*r0)/max; g0=(255*g0)/max; b0=(255*b0)/max;}

			if(r0==0)
			{
				if(g0>=b0) j=510+b0;
				else j=510+255+(255-g0);
			}

			if(g0==0)
			{
				if(b0>=r0) j=1020+r0;
				else j=1020+255+(255-b0);
			}

			if(b0==0)
			{
				if(r0>=g0) j=0+g0;
				else j=0+255+(255-r0);
			}

			if((j>=a0) && (j<=a1))
			{
				out[i].a-=aCLEAR;
				r=(int)f0[i].r;
				g=(int)f0[i].g;
				b=(int)f0[i].b;
				if((x>0) && (x<(int)w-1))
				{
					if((y>0) && (y<(int)h-1))
					{
						test[i]+=16;
						test[i-1-w]+=1;
						test[i-0-w]+=1;
						test[i+1-w]+=1;
						test[i-1-0]+=1;
						test[i+1-0]+=1;
						test[i-1+w]+=1;
						test[i-0+w]+=1;
						test[i+1+w]+=1;
					}
				}
			}
skip:
			out[i].r=(unsigned char)r;
			out[i].g=(unsigned char)g;
			out[i].b=(unsigned char)b;
		}
	}
	for(y=1; y<(int)h-1; y++)
	{
		Y=y*(int)w;
		for(x=1; x<(int)w-1; x++)
		{
			i=x+Y;

			j=test[i];
			if(j&16)
			{
				j-=16;
				if(j<=2)
				{
					out[i].abgr=0;
					out[i].a=f0[i].a|aCLEAR;
				}
			} else {
				if(j>2)
				{
					out[i].abgr=f0[i].abgr;
				}
			}
		}
	}

	delete [] test;
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(display);

	display=OUT_BUFFER;

	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::OnColourRainbow()
{
	if(!top_frame) return;

	if(!macro_mode)
	{
		BeginWaitCursor();
	}

	int x,y,Y,i;
	int r,g,b;
	int r0,g0,b0;
	int sum,j;

	int R[3*510],G[3*510],B[3*510];

	progress.AddString(IDS_RAINBOW);

	for(i=0; i<510; i++)
	{
		if((i>=0) && (i<255))
		{
			R[i]=255;
			G[i]=i;
			B[i]=0;

			R[i+510]=0;
			G[i+510]=255;
			B[i+510]=i;

			R[i+1020]=i;
			G[i+1020]=0;
			B[i+1020]=255;
		} else {
			j=i-255;

			R[i]=255-j;
			G[i]=255;
			B[i]=0;

			R[i+510]=0;
			G[i+510]=255-j;
			B[i+510]=255;

			R[i+1020]=255;
			G[i+1020]=0;
			B[i+1020]=255-j;
		}
	}

	PIXEL *f0=(PIXEL*)GetBuffer(active_frame);

	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER);

	for(y=0; y<(int)h; y++)
	{
		Y=y*(int)w;
		for(x=0; x<(int)w; x++)
		{
			i=x+Y;
			out[i].a=f0[i].a;

			r0=(int)f0[i].r;
			g0=(int)f0[i].g;
			b0=(int)f0[i].b;

			sum=r0+g0+b0; if(sum==0) sum=1;

			r=R[2*sum-1];
			g=G[2*sum-1];
			b=B[2*sum-1];

			out[i].r=(unsigned char)r;
			out[i].g=(unsigned char)g;
			out[i].b=(unsigned char)b;
		}
	}
	UpdateBuffer(OUT_BUFFER);
	UpdateBuffer(active_frame);

	if(!macro_mode)
	{
		display=OUT_BUFFER;
		EndWaitCursor();
		RefreshScreen();
	}
}

void CCombineDoc::OnFrameSplithsg()
{
	if(!top_frame) return;

	int x,startx,endx;
	int y,starty,endy,Y;

	int i;
	int r0,g0,b0,a,r,g,b;
	int min,max;

	progress.AddString(IDS_SPLITHSG);

	if(top_frame-3>=MAX_FRAMES-3)
	{
		CString s1,s2;
		s1.LoadString(IDS_3FRAMEERROR); s2.LoadString(IDS_FRAMELIMIT);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		return;
	}

	BeginWaitCursor();

	startx=0;
	starty=0;
	endx=w;
	endy=h;

	PIXEL *f0=(PIXEL*)GetBuffer(display);

	PIXEL *fh=(PIXEL *)GetBuffer(top_frame);
	str1.LoadString(IDS_STRING333);
	buffer[top_frame].name=buffer[display].name+str1;
	buffer[top_frame].hidden=false;
	top_frame++;

	PIXEL *fs=(PIXEL *)GetBuffer(top_frame);
	str1.LoadString(IDS_STRING334);
	buffer[top_frame].name=buffer[display].name+str1;
	buffer[top_frame].hidden=false;
	top_frame++;

	PIXEL *fg=(PIXEL *)GetBuffer(top_frame);
	str1.LoadString(IDS_STRING335);
	buffer[top_frame].name=buffer[display].name+str1;
	buffer[top_frame].hidden=false;
	top_frame++;

	PIXEL p;

	for(y=0; y<(int)h; y++)
	{
		Y=y*(int)w;
		for(x=0; x<(int)w; x++)
		{
			i=x+Y;
			p=f0[i];
			a=p.a; r0=p.r; g0=p.g; b0=p.b;

			min=r0; if(g0<min) min=g0; if(b0<min) min=b0;
			r0-=min; g0-=min; b0-=min;
			max=r0; if(g0>max) max=g0; if(b0>max) max=b0;

			r=g=b=0;

			if(max)
			{
				r=(r0*255)/max;
				g=(g0*255)/max;
				b=(b0*255)/max;
			}

			fh[i].a=(unsigned char)a; 
			fh[i].r=(unsigned char)r; 
			fh[i].g=(unsigned char)g; 
			fh[i].b=(unsigned char)b; 

			if(max<3) max=0;
			fs[i].a=(unsigned char)a; 
			fs[i].r=(unsigned char)max; 
			fs[i].g=(unsigned char)max; 
			fs[i].b=(unsigned char)max; 

			fg[i].a=(unsigned char)a; 
			fg[i].r=(unsigned char)(min); 
			fg[i].g=(unsigned char)(min); 
			fg[i].b=(unsigned char)(min); 
		}
	}
	FreeBuffer(display);
	UpdateBuffer(top_frame-1);
	UpdateBuffer(top_frame-2);
	UpdateBuffer(top_frame-3);

	active_frame=top_frame-3;
	display=top_frame-3;
	refresh_used_list();

	EndWaitCursor();
	RefreshScreen();
}


void CCombineDoc::OnFrameColourproduct()
{
	if(!top_frame) return;
	if(active_frame2<0) return;

	int x,y,Y,i;

	BeginWaitCursor();

	progress.AddString(IDS_COLOURPROD);

	PIXEL *f0=(PIXEL*)GetBuffer(active_frame);
	PIXEL *f1=(PIXEL*)GetBuffer(active_frame2);
	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER);

	for(y=0; y<(int)h; y++)
	{
		Y=y*(int)w;
		for(x=0; x<(int)w; x++)
		{
			i=x+Y;
			out[i].a=f0[i].a|f0[i].a;
			out[i].r=(unsigned char)((((int)f0[i].r)*((int)f1[i].r))/255);
			out[i].g=(unsigned char)((((int)f0[i].g)*((int)f1[i].g))/255);
			out[i].b=(unsigned char)((((int)f0[i].b)*((int)f1[i].b))/255);
		}
	}
	FreeBuffer(active_frame);
	FreeBuffer(active_frame2);
	UpdateBuffer(OUT_BUFFER);

	display=OUT_BUFFER;
	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::OnFrameSplitrgb()
{
	if(!top_frame) return;

	int x,startx,endx;
	int y,starty,endy,Y;

	int i;

	progress.AddString(IDS_SPLITRGB);

	if(top_frame-3>=MAX_FRAMES-3)
	{
		CString s1,s2;
		s1.LoadString(IDS_3FRAMEERROR); s2.LoadString(IDS_FRAMELIMIT);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		return;
	}

	BeginWaitCursor();

	startx=0;
	starty=0;
	endx=w;
	endy=h;

	PIXEL *f0=(PIXEL*)GetBuffer(display);

	PIXEL *fr=(PIXEL *)GetBuffer(top_frame);
	str1.LoadString(IDS_STRING336);
	buffer[top_frame].name=buffer[display].name+str1;
	buffer[top_frame].hidden=false;
	top_frame++;

	PIXEL *fg=(PIXEL *)GetBuffer(top_frame);
	str1.LoadString(IDS_STRING337);
	buffer[top_frame].name=buffer[display].name+str1;
	buffer[top_frame].hidden=false;
	top_frame++;

	PIXEL *fb=(PIXEL *)GetBuffer(top_frame);
	str1.LoadString(IDS_STRING338);
	buffer[top_frame].name=buffer[display].name+str1;
	buffer[top_frame].hidden=false;
	top_frame++;

	PIXEL p;
	unsigned int a,r,g,b;

	for(y=0; y<(int)h; y++)
	{
		Y=y*(int)w;
		for(x=0; x<(int)w; x++)
		{
			i=x+Y;
			p=f0[i];
			a=p.a; r=p.r; g=p.g; b=p.b;

			fr[i].a=(unsigned char)a; 
			fr[i].r=(unsigned char)r; 
			fr[i].g=(unsigned char)r; 
			fr[i].b=(unsigned char)r; 

			fg[i].a=(unsigned char)a; 
			fg[i].r=(unsigned char)g; 
			fg[i].g=(unsigned char)g; 
			fg[i].b=(unsigned char)g; 

			fb[i].a=(unsigned char)a; 
			fb[i].r=(unsigned char)b; 
			fb[i].g=(unsigned char)b; 
			fb[i].b=(unsigned char)b; 
		}
	}
	FreeBuffer(display);
	UpdateBuffer(top_frame-1);
	UpdateBuffer(top_frame-2);
	UpdateBuffer(top_frame-3);

	active_frame=top_frame-3;
	display=top_frame-3;
	refresh_used_list();

	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::OnFrameStretchcolours()
{
	if(!top_frame) return;

	int x,startx=16,endx=w-16;
	int y,starty=16,endy=h-16,Y;
	int r,g,b,rmax=0,rmin=255,gmax=0,gmin=255,bmax=0,bmin=255;

	int i;

	progress.AddString(IDS_STRETCHCOLOURS);

	BeginWaitCursor();

	PIXEL *f0=(PIXEL*)GetBuffer(active_frame);
	for(y=starty; y<endy; y++)
	{
		Y=y*(int)w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			r=(int)f0[i].r;
			g=(int)f0[i].g;
			b=(int)f0[i].b;
			if(r<rmin) rmin=r; if(r>rmax) rmax=r;
			if(g<gmin) gmin=g; if(g>gmax) gmax=g;
			if(b<bmin) bmin=b; if(b>bmax) bmax=b;
		}
	}

	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER);
	for(y=0; y<(int)h; y++)
	{
		Y=y*(int)w;
		for(x=0; x<(int)w; x++)
		{
			i=x+Y;
			out[i].a=(unsigned char)f0[i].a;
			(int)f0[i].r;

			if(rmax-rmin!=0) r=(((int)(f0[i].r)-rmin)*255)/(rmax-rmin); else r=(int)(f0[i].r);
			if(gmax-gmin!=0) g=(((int)(f0[i].g)-gmin)*255)/(gmax-gmin); else g=(int)(f0[i].g);
			if(bmax-bmin!=0) b=(((int)(f0[i].b)-bmin)*255)/(bmax-bmin); else b=(int)(f0[i].b);

			out[i].r=(unsigned char)r;
			out[i].g=(unsigned char)g;
			out[i].b=(unsigned char)b;
		}
	}
	UpdateBuffer(OUT_BUFFER);
	UpdateBuffer(active_frame);

	EndWaitCursor();
	display=OUT_BUFFER;
	RefreshScreen();
}
