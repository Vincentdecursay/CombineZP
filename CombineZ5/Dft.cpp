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
// Dft.cpp: implementation of the CDft class.
//
//////////////////////////////////////////////////////////////////////


//******* WORK IN PROGRESS ***//


#include "stdafx.h"
#include "combine.h"
#include "Dft.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDft::CDft()
{
	initialized=false;
	filtering=false;
	valid_filter=false;
	valid_store1=false;
	valid_store3=false;
	valid_transform3=false;
	valid_transform1=false;
	valid_picture3=false;
	valid_picture1=false;
	minr=ming=minb=0;

	cutoff=0.5f;
	order=2.0f;
	width=1.0f;
	displacement=0.0f;
	centre=0.5f;
	brightness=2.0f;
	threshold=230.0f;
	out_mode=OUT_NORMAL;
	mode=0;
}

CDft::~CDft()
{
	Destroy();
}

#include "stdio.h"

void CDft::Destroy()
{
	if(!initialized) return;

	FILE *wisdom;
	char* s;
	if(wisdom=fopen(wise_path,"w"))
	{
		s=fftwf_export_wisdom_to_string();
		fputs(s,wisdom);
		fclose(wisdom);
		fftwf_free(s);
	}

	if(pin) fftwf_destroy_plan(pin);
	if(pout) fftwf_destroy_plan(pout);

	initialized=false;
	filtering=false;

	valid_store3=false;
	valid_store1=false;
	valid_filter=false;
	valid_transform3=false;
	valid_transform1=false;
	valid_picture3=false;
	valid_picture1=false;
}

void CDft::Reset(int Wdth, int Height, CString path, void *io)
{
	if(initialized && (w==Wdth) && (h==Height)) return;

	Destroy();

	wise_path=path+"Wisdom.czw";
	FILE *wisdom;
	char *s;
	int len;
	if(wisdom=fopen(wise_path,"r"))
	{
		fseek(wisdom,0,SEEK_END);
		len=ftell(wisdom);
		fseek(wisdom,0,SEEK_SET);
		s=new char[len];
		fread(s,1,len,wisdom);
		fftwf_import_wisdom_from_string(s);
		fclose(wisdom);
		delete s;
	}

	w=Wdth; h=Height;

	small_delta=0.5f/(float)sqrt(w*w+h*h);
	big_delta=10.0f*small_delta;

	initialized=true;

	int n[2]; n[0]=w; n[1]=h;

	pin=fftwf_plan_dft_r2c(2,n,(float*)io,(fftwf_complex*)io,FFTW_MEASURE);
	if(pin==0)
	{
		initialized=false;
		return;
	}
	pout=fftwf_plan_dft_c2r(2,n,(fftwf_complex*)io,(float*)io,FFTW_MEASURE);
	if(pout==0)
	{
		Destroy();
		initialized=false;
		return;
	}
}

void CDft::In3(void *vf,void *inr,void *ing,void *inb)
{
	PIXEL *f=(PIXEL*)vf;
	int j,x,y,Y;
	PIXEL p;
	float *infr=(float*)inr,*infg=(float*)ing,*infb=(float*)inb;

	int k3=2*(h/2+1);

	minr=ming=minb=0;

	for(y=0; y<k3; y++)
	{
		Y=w*y;
		for(x=0; x<w; x++)
		{
			j=y+k3*x;
			p=f[x+Y];
			infr[j]=(float)p.r;
		}
	}

	for(y=0; y<k3; y++)
	{
		Y=w*y;
		for(x=0; x<w; x++)
		{
			j=y+k3*x;
			p=f[x+Y];
			infg[j]=(float)p.g;
		}
	}

	for(y=0; y<k3; y++)
	{
		Y=w*y;
		for(x=0; x<w; x++)
		{
			j=y+k3*x;
			p=f[x+Y];
			infb[j]=(float)p.b;
		}
	}

	valid_picture3=true;
	valid_picture1=false;
}

void CDft::In1(void* vf,void *in)
{
	int j,x,y,Y;
	float p;

	int *f=(int*)vf;
	float *inf=(float*)in;

	int k3=2*(h/2+1);

	minr=ming=minb=0;

	for(y=0; y<k3; y++)
	{
		Y=w*y;
		for(x=0; x<w; x++)
		{
			j=y+k3*x;
			p=(float)((f[x+Y]&dMASK));
			inf[j]=p;
		}
	}

	valid_picture1=true;
	valid_picture3=false;
}

// n=0 to initialize, n=-1 after last frame
void CDft::InWeighted(void *f,int n,void *inr,void *ing,void *inb,void *inw,void *temp)
{
	int i,j,x,y,Y;
	PIXEL p,q;
	int d,min,max;
	float factor;

	PIXEL *frame=(PIXEL*)f;
	PIXEL *blurred=(PIXEL*)inw;

	int k1=2*(h/2+1);
	int k6=w*k1;

	int *red=(int*)ing,*green=(int*)inb,*blue=(int*)temp;
	float *infr=(float*)inr,*infg=(float*)ing,*infb=(float*)inb,*tempf=(float*)temp;

	switch(n)
	{
	case 0: // clear
		for(i=0; i<(w+2)*(h+2); i++)
		{
			red[i]=0;
			green[i]=0;
			blue[i]=0;
			infr[i]=0.0f;
			((int*)inw)[i]=0;
		}
		break;

	case -1: // finish
		min=0x7ffffff0; max=0-0x7ffffff0;
		for(y=0; y<h; y++)
		{
			Y=y*w;
			for(x=0; x<w; x++)
			{
				i=x+Y;
				red[i]/=((int*)inw)[i];
				d=red[i];
				if(d<min) min=d;
				if(d>max) max=d; 
				green[i]/=((int*)inw)[i];
				d=green[i];
				if(d<min) min=d;
				if(d>max) max=d; 
				blue[i]/=((int*)inw)[i];
				d=blue[i];
				if(d<min) min=d;
				if(d>max) max=d;
			}
		}
		if(max==min) max++;
		factor=250.0f/(float)(max-min);

		for(y=0; y<h; y++)
		{
			Y=y*w;
			for(x=0; x<w; x++)
			{
				i=x+Y;
				j=y+k1*x;
				infr[j]=factor*(float)(red[i]-min);
			}
		}

		for(y=0; y<h; y++)
		{
			Y=y*w;
			for(x=0; x<w; x++)
			{
				i=x+Y;
				j=y+k1*x;
				infg[j]=factor*(float)(green[i]-min);
			}
		}

		for(y=0; y<h; y++)
		{
			Y=y*w;
			for(x=0; x<w; x++)
			{
				i=x+Y;
				j=y+k1*x;
				infb[j]=factor*(float)(blue[i]-min);
			}
		}
		valid_picture3=true;
		valid_picture1=false;
		break;
		minr=ming=minb=0;//min*factor;
	default: // sum and weight
		for(y=0; y<h; y++)
		{
			Y=y*w;
			for(x=0; x<w; x++)
			{
				i=x+Y;

				p=frame[i];
				q=blurred[i];

				int weight=(int)q.r+(int)q.g+(int)q.b;
				if(weight<1) weight=1;

				d=(int)p.r;
				d=d*weight;
				red[i]+=d;

				d=(int)p.g;
				d=d*weight;
				green[i]+=d;

				d=(int)p.b;
				d=d*weight;
				blue[i]+=d;
			}
		}
	}
}

void CDft::InStack(void *f, int n, void *inr,void *ing,void *inb)
{
	PIXEL *frame=(PIXEL*)f;
	int i,j,x,y,Y;
	PIXEL p;
	float c,d;
	minr=ming=minb=10^99;
	int *r=(int*)inr,*g=(int*)ing,*b=(int*)inb;
	float *infr=(float*)inr,*infg=(float*)ing,*infb=(float*)inb;

	static frames;

	int k1=2*(h/2+1);
	int k6=w*k1;

	switch(n)
	{
	case 0:
		frames=0;
		for(i=0; i<w*k1; i++) r[i]=0;
		for(i=0; i<w*k1; i++) g[i]=0;
		for(i=0; i<w*k1; i++) b[i]=0;
		break;
	case -1:
		c=1.0f/(float)frames;
		for(i=0; i<k6; i++){d=c*(float)r[i]; infr[i]=d; if(d<minr) minr=d;}
		for(i=0; i<k6; i++){d=c*(float)g[i]; infg[i]=d; if(d<ming) ming=d;}
		for(i=0; i<k6; i++){d=c*(float)b[i]; infb[i]=d; if(d<minb) minb=d;}
		for(i=0; i<k6; i++){infr[i]-=minr;}
		for(i=0; i<k6; i++){infg[i]-=ming;}
		for(i=0; i<k6; i++){infb[i]-=minb;}
		valid_picture3=true;
		valid_picture1=false;
		break;
	default:
		frames++;

		for(y=0; y<k1; y++)
		{
			Y=w*y;
			for(x=0; x<w; x++)
			{
				j=y+k1*x;
				p=frame[x+Y];
				r[j]+=(int)p.r;
			}
		}

		for(y=0; y<k1; y++)
		{
			Y=w*y;
			for(x=0; x<w; x++)
			{
				j=y+k1*x;
				p=frame[x+Y];
				g[j]+=(int)p.g;
			}
		}

		for(y=0; y<k1; y++)
		{
			Y=w*y;
			for(x=0; x<w; x++)
			{
				j=y+k1*x;
				p=frame[x+Y];
				b[j]+=(int)p.b;
			}
		}
	}
}

void  CDft::TransformIn(void *r,void *g,void *b)
{
	fftwf_complex *cr=(fftwf_complex*)r,*cg=(fftwf_complex*)g,*cb=(fftwf_complex*)b;
	if(valid_picture3)
	{
		fftwf_execute_dft(pin,cr,cr);
		fftwf_execute_dft(pin,cg,cg);
		fftwf_execute_dft(pin,cb,cb);

		valid_transform3=true;
		valid_transform1=false;
		valid_picture3=false;
		valid_picture1=false;
	}
}

void CDft::TransformIn1(void *io)
{
	fftwf_complex *cio=(fftwf_complex*)io;
	if(valid_picture1)
	{
		fftwf_execute_dft(pin,cio,cio);

		valid_transform1=true;
		valid_transform3=false;
		valid_picture3=false;
		valid_picture1=false;
	}
}

void CDft::Out3(void *vf,void *inr,void *ing,void *inb)
{
	PIXEL *f=(PIXEL*)vf;
	float *infr=(float*)inr,*infg=(float*)ing,*infb=(float*)inb;

	int j,x,y,Y;
	int c;
	float scale;

	scale=1.0f/(float)(w*h);

	if(out_mode==OUT_THRESHOLD) {Threshold(f,inr,ing,inb); return;}
	if(out_mode==OUT_BRIGHT) {Brightness(f,inr,ing,inb); return;}

	int k3=2*(h/2+1);

	for(y=0; y<h; y++)
	{
		Y=y*w;
		for(x=0; x<w; x++)
		{
			j=y+k3*x;

			c=(int)(infr[j]*scale+0.5f+minr);
			if(c<0) c=0;
			if(c>255) c=255;
			f[x+Y].r=(unsigned char)c;
		}
	}

	for(y=0; y<h; y++)
	{
		Y=y*w;
		for(x=0; x<w; x++)
		{
			j=y+k3*x;

			c=(int)(infg[j]*scale+0.5f+ming);
			if(c<0) c=0;
			if(c>255) c=255;
			f[x+Y].g=(unsigned char)c;
		}
	}

	for(y=0; y<h; y++)
	{
		Y=y*w;
		for(x=0; x<w; x++)
		{
			j=y+k3*x;

			c=(int)(infb[j]*scale+0.5f+minb);
			if(c<0) c=0;
			if(c>255) c=255;
			f[x+Y].b=(unsigned char)c;
		}
	}
}

void CDft::Threshold(void *f,void *inr,void *ing,void *inb)
{
	PIXEL *frame=(PIXEL*)f;
	float *infr=(float*)inr,*infg=(float*)ing,*infb=(float*)inb;

	int j,x,y;
	float c,d;
	float scale;
	PIXEL p;

	scale=1.0f/(float)(w*h);

	int k3=2*(h/2+1);

	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			d=0.0;

			j=y+k3*x;

			c=(abs(infr[j])+abs(infg[j])+abs(infb[j]))*scale+0.5f;
			if(c>threshold) d=255.0f;

			p.r=p.g=p.b=(unsigned char)d;
			frame[x+y*w].abgr=p.abgr;
		}
	}

	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			d=0.0;

			j=y+k3*x;

			c=infg[j]*scale+0.5f;
			if(c>threshold) d=255.0f;

			p.r=p.g=p.b=(unsigned char)d;
			frame[x+y*w].abgr=p.abgr;
		}
	}

	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			d=0.0;

			j=y+k3*x;

			c=infb[j]*scale+0.5f;
			if(c>threshold) d=255.0f;

			p.r=p.g=p.b=(unsigned char)d;
			frame[x+y*w].abgr=p.abgr;
		}
	}
}

void CDft::Brightness(void *f,void *inr,void *ing,void *inb)
{
	PIXEL *frame=(PIXEL*)f;
	float *infr=(float*)inr,*infg=(float*)ing,*infb=(float*)inb;

	int j,x,y;
	int c;
	float scale;

	scale=brightness/(float)(w*h);

	int k3=2*(h/2+1);

	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			j=y+k3*x;

			c=(int)(infr[j]*scale+0.5f);
			if(c<0) c=0;
			if(c>255) c=255;
			frame[x+y*w].r=(unsigned char)c;
		}
	}

	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			j=y+k3*x;

			c=(int)(infg[j]*scale+0.5f);
			if(c<0) c=0;
			if(c>255) c=255;
			frame[x+y*w].g=(unsigned char)c;
		}
	}

	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			j=y+k3*x;

			c=(int)(infb[j]*scale+0.5f);
			if(c<0) c=0;
			if(c>255) c=255;
			frame[x+y*w].b=(unsigned char)c;
		}
	}
}

void CDft::Out1(void *vf,int max,void *in)
{
	PIXEL *f=(PIXEL*)vf;
	float *inf=(float*)in;
	int j,x,y;
	int p;
	float scale,c,d;
	PIXEL q;

	scale=1.0f/(float)(w*h);
	if(out_mode==OUT_BRIGHT) scale*=brightness;

	int k3=2*(h/2+1);

	if(out_mode==OUT_THRESHOLD) goto thold;

	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			j=y+k3*x;

			p=(int)(inf[j]*scale+0.5f);
			if(p<0) p=0;
			if(p>max) p=max;
			q.abgr=0;
			q.b=(unsigned char)(int)(p*255/max);
			q.r=(unsigned char)(int)(p/255);
			f[x+y*w]=q;
		}
	}
	return;

thold:
	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			d=0.0;

			j=y+k3*x;

			c=inf[j]*scale+0.5f;
			if(c>threshold*max/255) d=255.0f;

			q.r=q.g=q.b=(unsigned char)d;
			f[x+y*w].abgr=q.abgr;
		}
	}
}

void CDft::OutToDM(void *d, int max, void *in)
{
	int *depth=(int*)d;
	float *inf=(float*)in;
	int j,x,y;
	int p;
	float scale;

	scale=1.0f/(float)(w*h);

	int k3=2*(h/2+1);

	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			j=y+k3*x;

			if(depth[x+y*w]&fFIXED) continue;

			p=(int)(inf[j]*scale);
			if(p<0) p=0;
			if(p>max) p=max;
			depth[x+y*w]=(p|fFILLED);
		}
	}
}

void CDft::TransformOut(void *r,void *g,void *b)
{
	fftwf_complex *cr=(fftwf_complex*)r,*cg=(fftwf_complex*)g,*cb=(fftwf_complex*)b;
	if(valid_transform3)
	{
		fftwf_execute_dft(pout,cr,cr);
		fftwf_execute_dft(pout,cg,cg);
		fftwf_execute_dft(pout,cb,cb);

		valid_picture3=true;
		valid_picture1=false;
		valid_transform3=false;
		valid_transform1=false;
	}
}

void CDft::TransformOut1(void *io)
{
	fftwf_complex *cio=(fftwf_complex*)io;
	if(valid_transform1)
	{
		fftwf_execute_dft(pout,cio,cio);

		valid_picture1=true;
		valid_picture3=false;
		valid_transform3=false;
		valid_transform1=false;
	}
}

void CDft::Filter(void *f,void *r,void *g,void *b)
{
	float *ff=(float*)f;
	fftwf_complex *fr=(fftwf_complex*)r,*fg=(fftwf_complex*)g,*fb=(fftwf_complex*)b;
	int j,test;
	float u,v,fu,scale;

	zero_value=ff[0];

	test=w*(h/2+1);

	if(ff[0]>1.0f/1024.0f) scale=1.0f/ff[0];
	else scale=1.0f;

	if(valid_transform3)
	{
		for(j=0; j<test; j++)
		{
			fu=ff[j];
			u=fr[j][0]; v=fr[j][1];
			fr[j][0]=scale*u*fu; fr[j][1]=scale*v*fu;
		}

		for(j=0; j<test; j++)
		{
			fu=ff[j];
			u=fg[j][0]; v=fg[j][1];
			fg[j][0]=scale*u*fu; fg[j][1]=scale*v*fu;
		}

		for(j=0; j<test; j++)
		{
			fu=ff[j];
			u=fb[j][0]; v=fb[j][1];
			fb[j][0]=scale*u*fu; fb[j][1]=scale*v*fu;
		}
	}

	if(valid_transform1)
	{
		for(j=0; j<test; j++)
		{
			fu=ff[j];
			u=fr[j][0]; v=fr[j][1];
			fr[j][0]=scale*u*fu; fr[j][1]=scale*v*fu;
		}
	}
}

void CDft::Butterworth(void *f)
{
	float *filter=(float*)f;
	int i,j,z;
	float x,y,rsq,value;
	float sqt,ccutoff;
	sqt=(float)pow(0.5,0.5);
	ccutoff=100.0f*cutoff*cutoff;

	float k1=(float)(w/2+1)*(2.0f*sqt/(float)w);
	int k2=w/2+1;
	float k3=2.0f*sqt/(float)w;
	float k4=(float)(h/2+1)*(2.0f*sqt/(float)h);
	int k5=h/2+1;
	float k6=2.0f*sqt/(float)h;
	float k7=1.0f-displacement;
	int k8=w-1;

	for(j=0,x=k1; j<k2; j++,x-=k3)
	{
		for(i=0,y=k4; i<k5; i++,y-=k6)
		{
			rsq=(x*x+y*y)-k7;

			value=1.0f/(1.0f+(float)pow(rsq*rsq*ccutoff,order));
			if(value>1.0) value=1.0;
			if(value<0.0) value=0.0;

			z=i+j*k5; filter[z]=value;

			z=i+(k8-j)*(k5); filter[z]=value;
		}
	}
	valid_filter=true;
}

void CDft::Pass(void *f)
{
	float *filter=(float*)f;
	int i,j,z;
	float x,y,r,t,value,pi=3.14159f;
	float sqt,rwidth,wwidth,ddisplacement;
	sqt=(float)pow(0.5f,0.5f);
	wwidth=width*2;
	rwidth=1.0f/wwidth;
	ddisplacement=displacement-0.5f*wwidth;

	float k1=(w/2+1)*(2.0f*sqt/(float)w);
	int k2=w/2+1;
	float k3=2.0f*sqt/(float)w;
	float k4=((float)h/2.0f)*(2.0f*sqt/(float)h);
	int k5=h/2+1;
	float k6=2.0f*sqt/(float)h;
	int k7=w-1;

	for(j=0,x=k1; j<k2; j++,x-=k3)
	{
		for(i=0,y=k4; i<k5; i++,y-=k6)
		{
			r=(float)sqrt(x*x+y*y);
			if(r<0.0f) r=0.0;
			if(r>1.0f) r=1.0;
			t=(rwidth*(r-ddisplacement))*pi;
			value=(float)sin(t);

			if(value<0.0) value=0.0;
			if(r<ddisplacement) value=0.0;
			if(r>ddisplacement+wwidth) value=0.0;

			z=i+j*k5; filter[z]=value;
			z=i+(k7-j)*k5; filter[z]=value;
		}
	}
	valid_filter=true;
}

void CDft::Low(void *f)
{
	float *filter=(float*)f;
	int i,j;
	float x,y,r,t,value,pi=3.14159f;
	float sqt,rwidth,wwidth,ddisplacement;
	sqt=(float)pow(0.5f,0.5f);

	wwidth=2.0f*width;
	rwidth=1.0f/wwidth;
	ddisplacement=displacement;
	float diag=(float)sqrt(2.0f);

	int k1=w/2+1;
	float k2=1.0f/((float)w/2.0f);
	int k3=h/2+1;
	float k4=1.0f/((float)h/2.0f);
	float k5=1.0f-displacement;
	float k6=0.5f*pi;
	float k7=0.5f*wwidth;
	int k8=w-1;

	for(j=0,x=1.0f; j<k1; j++,x-=k2)
	{
		for(i=0,y=1.0f; i<k3; i++,y-=k4)
		{
			r=k5-(float)sqrt(x*x+y*y)/diag;
			if(r<0.0f) r=0.0;
			if(r>1.0f) r=1.0;
			t=r/width;
			value=1.0f-(float)sin(k6*t);
			if(value<0.0) value=0.0;
			if(r>k7) value=0.0;

			filter[i+j*k3]=value;
			filter[i+(k8-j)*k3]=value;
		}
	}
	valid_filter=true;
}

void CDft::High(void *f)
{
	float *filter=(float*)f;
	int i,j;
	float x,y,r,t,value,pi=3.14159f;
	float sqt,rwidth,wwidth,ddisplacement;
	float diag=(float)sqrt(2.0f);
	sqt=(float)pow(0.5f,0.5f);
	wwidth=2*width;
	rwidth=1.0f/wwidth;
	ddisplacement=(1.0f-displacement)-0.5f*wwidth;

	int k1=w/2+1;
	float k2=1.0f/((float)w/2.0f);
	int k3=h/2+1;
	float k4=1.0f/((float)h/2.0f);
	float k5=0.5f*pi;
	int k6=w-1;

	for(j=0,x=1.0f; j<k1; j++,x-=k2)
	{
		for(i=0,y=1.0f; i<k3; i++,y-=k4)
		{
			r=(float)sqrt(x*x+y*y)/diag-displacement;
			if(r<0.0f) r=0.0;
			if(r>1.0f) r=1.0;
			t=r/width;
//			value=1.0f-(float)sin(k5*t);
			value=1.0f+(float)sin(k5*t-pi);
			if(value<0.0) value=0.0;
			if(r>width) value=0.0;

			filter[i+j*k3]=value;
			filter[i+(k6-j)*k3]=value;
		}
	}
	valid_filter=true;
}

void CDft::Stop(void *f)
{
	float *filter=(float*)f;
	int i,j,z;
	float x,y,r,t,value,pi=3.14159f;
	float sqt,rwidth,wwidth,ddisplacement;
	sqt=(float)pow(0.5f,0.5f);

	wwidth=2.0f*width;
	rwidth=1.0f/wwidth;
	ddisplacement=displacement;

	float k1=2.0f*sqt/(float)w;
	float k2=2.0f*sqt/(float)h;
	float k3=centre-0.5f;
	float k4=rwidth*pi;
	float k5=pi/2;
	float k6=ddisplacement+wwidth;
	int k7=h/2+1;
	int k8=w-1;

	for(j=w/2,x=0.0f; j>=0; j--,x+=k1)
	{
		for(i=h/2,y=0.0f; i>=0; i--,y+=k2)
		{
			r=((float)sqrt(x*x+y*y)-k3);
			t=k4*(r-ddisplacement);
			if(t>0.0 && t<k5) value=(float)cos(t);
			else value=0.0;
			if(r<=ddisplacement) value=1.0f;
			if(r>=k6) value=0.0f;

			z=i+j*k7; filter[z]=value;
			z=i+(k8-j)*k7; filter[z]=value;
		}
	}

	wwidth=2.0f*width;
	rwidth=1.0f/wwidth;
	ddisplacement=displacement;

	k3=centre+0.5f;

	for(j=w/2,x=0.0f; j>=0; j--,x+=k1)
	{
		for(i=h/2,y=0.0f; i>=0; i--,y+=k2)
		{
			r=k3-(float)sqrt(x*x+y*y);
			t=k4*(r-ddisplacement);
			if(t>0.0 && t<k5) value=(float)cos(t);
			else value=0.0;
			if(r>k6) continue;
			if(r<ddisplacement) value=1.0;

			z=i+j*k7;
			if(value>filter[z])
			{
				filter[z]=value;
				z=i+(k8-j)*k7; filter[z]=value;
			}
		}
	}
	valid_filter=true;
}

void CDft::VisualizeFilter(void* fm,void *f)
{
	float *filter=(float*)f;

	zero_value=filter[0];

	PIXEL *frame=(PIXEL*)fm;
	int i,ix,iy,j,IY,IY3,H3,half,offx,offy;
	float limit;
	float x;
	half=w*(h/2);
	PIXEL pr;
	H3=((h/2)+1);
	limit=0.001f;
	offx=(w-w/2)/2; offy=(h-h/2)/2;

	int k1=h/2;
	int k2=w/2+1;

	for(iy=0; iy<k1; iy++)
	{
		IY=w*(iy+offy);
		IY3=iy;
		for(ix=0; ix<k2; ix++)
		{
			i=ix+offx+IY;
			j=IY3+H3*ix;

			x=filter[j];
			if(x>=limit)
			{
				if(x>1.0) pr.r=(unsigned char)255;
				else pr.r=(unsigned char)(int)(255.0*x);
			} else pr.r=0;
			pr.g=pr.b=pr.r;

			frame[i]=pr;
		}
	}
}
