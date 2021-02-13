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
#include "math.h"
#include "combine.h"
#include "combineDoc.h"


int ptree[25],pyramidw[25],pyramidh[25];

//********************** Initialize

void CCombineDoc::OnPyramidClearpyramid()
{
	if(top_frame<1) return;
	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;
	progress.AddString(IDS_PCLEAR,pnumber);
	AfxBeginThread(PyramidClearT,this);
}

UINT CCombineDoc::PyramidClearT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->PyramidClear();
	return 0;
}

struct p7_Params
{
	unsigned int index,step;
	int *p1,*p2,*p3,*p4,*p5,*p6,*p7;
	int end;
} p7_params[32];

UINT CCombineDoc::pc7(LPVOID param)
{
	p7_Params *pp=(p7_Params *)param;
	unsigned int index=pp->index;
	unsigned int step=pp->step;
	int *p1=pp->p1;
	int *p2=pp->p2;
	int *p3=pp->p3;
	int *p4=pp->p4;
	int *p5=pp->p5;
	int *p6=pp->p6;
	int *p7=pp->p7;
	int end=pp->end;

	int i;
	for(i=index; i<end; i+=step)
	{
		p1[i]=0;
		p2[i]=0;
		p3[i]=0;
		p4[i]=0;
		p5[i]=0;
		p6[i]=0;
		p7[i]=0;
	}

	return 0;
}

void CCombineDoc::PyramidClear()
{
	if(top_frame<1) return;

	BeginThreads
	SetThreads(w,h)

	int i;
	int *p1,*p2,*p3,*p4,*p5,*p6,*p7;

	int end=slot_size/4;

	p1=(int*)GetBuffer(PYRAMID_BUFFER+0);
	p2=(int*)GetBuffer(PYRAMID_BUFFER+1);
	p3=(int*)GetBuffer(PYRAMID_BUFFER+2);
	p4=(int*)GetBuffer(PYRAMID_BUFFER+3);
	p5=(int*)GetBuffer(PYRAMID_BUFFER+4);
	p6=(int*)GetBuffer(PYRAMID_BUFFER+5);
	p7=(int*)GetBuffer(PYRAMID_BUFFER+6);
	for(obj=0; obj<objects; obj++)
	{
		p7_params[obj].index=obj;
		p7_params[obj].step=objects;
		p7_params[obj].end=end;
		p7_params[obj].p1=p1;
		p7_params[obj].p2=p2;
		p7_params[obj].p3=p3;
		p7_params[obj].p4=p4;
		p7_params[obj].p5=p5;
		p7_params[obj].p6=p6;
		p7_params[obj].p7=p7;
		StartThreads(pc7,p7_params)
	}
	EndThreads
	UpdateBuffer(PYRAMID_BUFFER+0);
	UpdateBuffer(PYRAMID_BUFFER+1);
	UpdateBuffer(PYRAMID_BUFFER+2);
	UpdateBuffer(PYRAMID_BUFFER+3);
	UpdateBuffer(PYRAMID_BUFFER+4);
	UpdateBuffer(PYRAMID_BUFFER+5);
	UpdateBuffer(PYRAMID_BUFFER+6);

	p1=(int*)GetBuffer(PYRAMID_BUFFER+7);
	p2=(int*)GetBuffer(PYRAMID_BUFFER+8);
	p3=(int*)GetBuffer(PYRAMID_BUFFER+9);
	p4=(int*)GetBuffer(PYRAMID_BUFFER+10);
	p5=(int*)GetBuffer(PYRAMID_BUFFER+11);
	p6=(int*)GetBuffer(PYRAMID_BUFFER+12);
	p7=(int*)GetBuffer(PYRAMID_BUFFER+13);
	for(obj=0; obj<objects; obj++)
	{
		p7_params[obj].index=obj;
		p7_params[obj].step=objects;
		p7_params[obj].end=end;
		p7_params[obj].p1=p1;
		p7_params[obj].p2=p2;
		p7_params[obj].p3=p3;
		p7_params[obj].p4=p4;
		p7_params[obj].p5=p5;
		p7_params[obj].p6=p6;
		p7_params[obj].p7=p7;
		StartThreads(pc7,p7_params)
	}
	EndThreads

	UpdateBuffer(PYRAMID_BUFFER+7);
	UpdateBuffer(PYRAMID_BUFFER+8);
	UpdateBuffer(PYRAMID_BUFFER+9);
	UpdateBuffer(PYRAMID_BUFFER+10);
	UpdateBuffer(PYRAMID_BUFFER+11);
	UpdateBuffer(PYRAMID_BUFFER+12);
	UpdateBuffer(PYRAMID_BUFFER+13);

	p7=(int*)GetBuffer(PYRAMID_BUFFER+14);
	for(i=0; i<end; i++) p7[i]=0;
	UpdateBuffer(PYRAMID_BUFFER+14);

	initialize_pyramid();

	if(refreshscreen)
	{
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

void CCombineDoc::initialize_pyramid()
{
	int pw,ph;
	ptree[0]=0;
	pyramidw[0]=w+4;
	pyramidh[0]=h+4;
	
	ptree[1]=0;
	pw=pyramidw[1]=w/2+4;
	ph=pyramidh[1]=h/2+4;

	int level=1;
	while(level<25)
	{
		level++;
		ptree[level]=ptree[level-1]+pyramidw[level-1]*pyramidh[level-1];
		pw=(pw-4)/2+4; if(2*(pw/2)!=pw) pw++;
		ph=(ph-4)/2+4; if(2*(ph/2)!=ph) ph++;
		pyramidw[level]=pw;
		pyramidh[level]=ph;
		if((pw==6) && (ph==6)) break;
	};
	plast_level=level;

	level++;
	ptree[level]=ptree[level-1]+pyramidw[level-1]*pyramidh[level-1];
	pyramidw[level]=pw;
	pyramidh[level]=ph;

	plast=ptree[level]+pw*ph;
	pdepth=2*(ptree[level]+pw*ph);

	CString str;
	str1.LoadString(IDS_STRING620);
	str.Format(str1,plast_level+1);
	progress.AddString(str);
}

//**************************** RGB Max in


void CCombineDoc::OnRgbmaximumcontrastruleAddallusedframes()
{
	if(plast_level==0)	PyramidClear();

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	progress.AddString(IDS_PADDMAXUF);

	AfxBeginThread(PyramidMaxUFT,this);
}

UINT CCombineDoc::PyramidMaxUFT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;

	int f;
	_this->progress.SetRange(0,_this->last_used_frame-1);
	for(f=0; f<_this->last_used_frame; f++)
	{
		_this->progress.SetPos(f);
		if(f==_this->last_used_frame-1) _this->refreshscreen=true;
		_this->PyramidMax(_this->used_frame[f]);
	}
	_this->progress.SetPos(0);

	return 0;
}

void CCombineDoc::OnMergeactivewithpyramidMaximumrule()
{
	if(plast_level==0)	PyramidClear();

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;

	pframe=active_frame;
	progress.AddString(IDS_PADDMAX,active_frame);
	AfxBeginThread(PyramidMaxT,this);
}

UINT CCombineDoc::PyramidMaxT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->PyramidMax(_this->pframe);
	return 0;
}

void CCombineDoc::PyramidMax(int frame)
{
	if(top_frame<1) return;

	if(plast_level==0) return;

	if(max_slots>10)
	{
		GetBuffer(TEMP_BUFFER+0);
		GetBuffer(TEMP_BUFFER+1);
		GetBuffer(TEMP_BUFFER+2);
		GetBuffer(TEMP_BUFFER+3);
		GetBuffer(TEMP_BUFFER+4);
		GetBuffer(TEMP_BUFFER+5);
	}
	if(max_slots>13)
	{
		GetBuffer(PYRAMID_BUFFER+0);
		GetBuffer(PYRAMID_BUFFER+1);
		GetBuffer(PYRAMID_BUFFER+2);
	}
	if(max_slots>21)
	{
		GetBuffer(PYRAMID_BUFFER+3);
		GetBuffer(PYRAMID_BUFFER+4);
		GetBuffer(PYRAMID_BUFFER+5);
		GetBuffer(PYRAMID_BUFFER+6);
		GetBuffer(PYRAMID_BUFFER+7);
		GetBuffer(PYRAMID_BUFFER+8);
		GetBuffer(PYRAMID_BUFFER+9);
		GetBuffer(PYRAMID_BUFFER+10);
		GetBuffer(PYRAMID_BUFFER+11);
		GetBuffer(PYRAMID_BUFFER+12);
		GetBuffer(PYRAMID_BUFFER+13);
		GetBuffer(PYRAMID_BUFFER+14);
	}

	if(max_slots>10) GetBuffer(frame);
	pinw(frame);
	PyramidMax1(1,frame,1);
	PyramidMax1(2,frame,2);
	PyramidMax1(3,frame,3);
	if(max_slots>10) FreeBuffer(frame);

	if(max_slots>10)
	{
		FreeBuffer(TEMP_BUFFER+0);
		FreeBuffer(TEMP_BUFFER+1);
		FreeBuffer(TEMP_BUFFER+2);
		FreeBuffer(TEMP_BUFFER+3);
		FreeBuffer(TEMP_BUFFER+4);
		FreeBuffer(TEMP_BUFFER+5);
	}
	if(max_slots>13)
	{
		FreeBuffer(PYRAMID_BUFFER+0);
		FreeBuffer(PYRAMID_BUFFER+1);
		FreeBuffer(PYRAMID_BUFFER+2);
	}
	if(max_slots>21)
	{
		FreeBuffer(PYRAMID_BUFFER+3);
		FreeBuffer(PYRAMID_BUFFER+4);
		FreeBuffer(PYRAMID_BUFFER+5);
		FreeBuffer(PYRAMID_BUFFER+6);
		FreeBuffer(PYRAMID_BUFFER+7);
		FreeBuffer(PYRAMID_BUFFER+8);
		FreeBuffer(PYRAMID_BUFFER+9);
		FreeBuffer(PYRAMID_BUFFER+10);
		FreeBuffer(PYRAMID_BUFFER+11);
		FreeBuffer(PYRAMID_BUFFER+12);
		FreeBuffer(PYRAMID_BUFFER+13);
		FreeBuffer(PYRAMID_BUFFER+14);
	}
}

struct pmc5_Params
{
	int index,step;
	int xmax,ymax;
	int *p2,*t2,*pwt2,*twt2;
	int frame,p,pdepth,plast;
} pmc5_params[32];

UINT CCombineDoc::pmc5(LPVOID param)
{
	pmc5_Params *pp=(pmc5_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int xmax=pp->xmax;
	int ymax=pp->ymax;
	int *p2=pp->p2;
	int *t2=pp->t2;
	int *pwt2=pp->pwt2;
	int *twt2=pp->twt2;
	int frame=pp->frame;
	int p=pp->p;
	int pdepth=pp->pdepth;
	int plast=pp->plast;

	int i,j,x,y,Y;

	for(y=index; y<ymax; y+=step)
	{
		Y=y*xmax+p;
		for(x=0; x<xmax; x++)
		{
			i=x+Y;

			if(abs(t2[i])>abs(p2[i]))
			{
				j=i+pdepth;
				((PDEPTH*)p2)[j].flags=PD_FILLED;
				((PDEPTH*)p2)[j].frame=(unsigned short)frame;
			}

			if(abs(twt2[i])>abs(pwt2[i]))
			{
				p2[i]=t2[i];
                pwt2[i]=twt2[i];
			}

			j=i+plast;
			p2[j]=t2[j];
		}
	}

	return 0;
}

struct pmc6_Params
{
	int index,step;
	int xmax,ymax;
	int *p1,*t1,*pwt1,*twt1;
	PDEPTH *p3;
	int frame;
} pmc6_params[32];

UINT CCombineDoc::pmc6(LPVOID param)
{
	pmc6_Params *pp=(pmc6_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int xmax=pp->xmax;
	int ymax=pp->ymax;
	int *p1=pp->p1;
	PDEPTH *p3=pp->p3;
	int *t1=pp->t1;
	int *pwt1=pp->pwt1;
	int *twt1=pp->twt1;
	int frame=pp->frame;

	int i,x,y,Y;

	for(y=index; y<ymax; y+=step)
	{
		Y=y*xmax;
		for(x=0; x<xmax; x++)
		{
			i=x+Y;

			if(abs(t1[i])>abs(p1[i]))
			{
				p3[i].flags=PD_FILLED;
				p3[i].frame=(unsigned short)frame;
			}

			if(abs(twt1[i])>abs(pwt1[i]))
			{
				p1[i]=t1[i];
				pwt1[i]=twt1[i];
			}
		}
	}

	return 0;
}

void CCombineDoc::PyramidMax1(int number, int frame,int channel)
{
	BeginThreads

	if(top_frame<1) return;

	int level;
	int *p1,*p2,*t1,*t2,*pwt1,*twt1,*pwt2,*twt2;
	PDEPTH *p3;
	unsigned int xmax,ymax,p;

	t1=(int *)GetBuffer(TEMP_BUFFER); // input data is in here
	switch(channel)
	{
	case 1:
		pinr(frame);
		break;
	case 2:
		ping(frame);
		break;
	case 3:
		pinb(frame);
		break;
	}
	t2=(int *)GetBuffer(TEMP_BUFFER+1); // input data is in here
	ptempbuild();
	UpdateBuffer(TEMP_BUFFER);

	p2=(int *)GetBuffer(PYRAMID_BUFFER+number+2);
	pwt2=(int *)GetBuffer(PYRAMID_BUFFER+number+11);
	twt2=(int *)GetBuffer(TEMP_BUFFER+5); // input data is in here
	level=plast_level;
	while(level>0)
	{
		xmax=pyramidw[level];
		ymax=pyramidh[level];
		p=ptree[level];

		SetThreads(xmax,ymax)
		for(obj=0; obj<objects; obj++)
		{
			if(ymax>objects)
			{
				pmc5_params[obj].index=obj;
				pmc5_params[obj].step=objects;
			} else {
				pmc5_params[obj].index=0;
				pmc5_params[obj].step=objects=1;
			}
			pmc5_params[obj].xmax=xmax;
			pmc5_params[obj].ymax=ymax;
			pmc5_params[obj].p2=p2;
			pmc5_params[obj].t2=t2;
			pmc5_params[obj].pwt2=pwt2;
			pmc5_params[obj].twt2=twt2;
			pmc5_params[obj].frame=frame;
			pmc5_params[obj].p=p;
			pmc5_params[obj].pdepth=pdepth;
			pmc5_params[obj].plast=plast;
			StartThreads(pmc5,pmc5_params)
		}
		EndThreads

		level--;
	}
	UpdateBuffer(PYRAMID_BUFFER+number+11);
	UpdateBuffer(PYRAMID_BUFFER+number+2);
	FreeBuffer(TEMP_BUFFER+5);
	FreeBuffer(TEMP_BUFFER+1);

	p1=(int *)GetBuffer(PYRAMID_BUFFER+number-1);
	p3=(PDEPTH*)GetBuffer(PYRAMID_BUFFER+number+5);
	pwt1=(int *)GetBuffer(PYRAMID_BUFFER+number+8);
	twt1=(int *)GetBuffer(TEMP_BUFFER+4); // input data is in here
	t1=(int *)GetBuffer(TEMP_BUFFER); // input data is in here
	xmax=pyramidw[0];
	ymax=pyramidh[0];
	p=ptree[0];

	for(obj=0; obj<objects; obj++)
	{
		pmc6_params[obj].index=obj;
		pmc6_params[obj].step=objects;
		pmc6_params[obj].xmax=xmax;
		pmc6_params[obj].ymax=ymax;
		pmc6_params[obj].p1=p1;
		pmc6_params[obj].p3=p3;
		pmc6_params[obj].t1=t1;
		pmc6_params[obj].pwt1=pwt1;
		pmc6_params[obj].twt1=twt1;
		pmc6_params[obj].frame=frame;
		StartThreads(pmc6,pmc6_params)
	}
	EndThreads

	UpdateBuffer(PYRAMID_BUFFER+number+8);
	UpdateBuffer(PYRAMID_BUFFER+number+5);
	UpdateBuffer(PYRAMID_BUFFER+number-1);
	FreeBuffer(TEMP_BUFFER+4);
	FreeBuffer(TEMP_BUFFER);

	if(refreshscreen)
	{
		refreshscreen=false;
		progress.m_cancel=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}
}

struct pmc3_Params
{
	int index,step;
	int w,h;
	PIXEL *in;
	int *out;
} pmc3_params[32];

UINT CCombineDoc::pmcr(LPVOID param)
{
	pmc3_Params *pp=(pmc3_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	PIXEL *in=pp->in;
	int *out=pp->out;

	int x,y,i,j,Y,pY;

	for(y=index; y<h; y+=step)
	{
		Y=y*w;
		pY=(y+2)*(w+4);
		for(x=0; x<w; x++)
		{
			i=x+Y;
			j=x+2+pY;
			out[j]=((int)in[i].r)<<4;
		}
	}

	return 0;
}

UINT CCombineDoc::pmcg(LPVOID param)
{
	pmc3_Params *pp=(pmc3_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	PIXEL *in=pp->in;
	int *out=pp->out;

	int x,y,i,j,Y,pY;

	for(y=index; y<h; y+=step)
	{
		Y=y*w;
		pY=(y+2)*(w+4);
		for(x=0; x<w; x++)
		{
			i=x+Y;
			j=x+2+pY;
			out[j]=((int)in[i].g)<<4;
		}
	}

	return 0;
}

UINT CCombineDoc::pmcb(LPVOID param)
{
	pmc3_Params *pp=(pmc3_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	PIXEL *in=pp->in;
	int *out=pp->out;

	int x,y,i,j,Y,pY;

	for(y=index; y<h; y+=step)
	{
		Y=y*w;
		pY=(y+2)*(w+4);
		for(x=0; x<w; x++)
		{
			i=x+Y;
			j=x+2+pY;
			out[j]=((int)in[i].b)<<4;
		}
	}

	return 0;
}

void CCombineDoc::pinr(int frame)
{
	if(top_frame<1) return;
	if(plevels<3) return;
	if(plevels>plast_level+1) plevels=plast_level+1;
	if(ptree[2]==0) return;

	int *p1;

	PIXEL* f=(PIXEL *)GetBuffer(frame);
	p1=(int *)GetBuffer(TEMP_BUFFER);

	BeginThreads
	SetThreads(w,h)

	for(obj=0; obj<objects; obj++)
	{
		pmc3_params[obj].index=obj;
		pmc3_params[obj].step=objects;
		pmc3_params[obj].w=w;
		pmc3_params[obj].h=h;
		pmc3_params[obj].in=f;
		pmc3_params[obj].out=p1;
		StartThreads(pmcr,pmc3_params)
	}
	EndThreads

	psetEdges(p1,0);
	FreeBuffer(frame);
	FreeBuffer(TEMP_BUFFER); // OK because Max is using it
}

void CCombineDoc::ping(int frame)
{
	if(top_frame<1) return;
	if(plevels<3) return;
	if(plevels>plast_level+1) plevels=plast_level+1;
	if(ptree[2]==0) return;

	int *p1;

	PIXEL* f=(PIXEL *)GetBuffer(frame);
	p1=(int *)GetBuffer(TEMP_BUFFER);

	BeginThreads
	SetThreads(w,h)

	for(obj=0; obj<objects; obj++)
	{
		pmc3_params[obj].index=obj;
		pmc3_params[obj].step=objects;
		pmc3_params[obj].w=w;
		pmc3_params[obj].h=h;
		pmc3_params[obj].in=f;
		pmc3_params[obj].out=p1;
		StartThreads(pmcg,pmc3_params)
	}
	EndThreads

	psetEdges(p1,0);
	FreeBuffer(frame);
	FreeBuffer(TEMP_BUFFER); // OK because Max is using it
}

void CCombineDoc::pinb(int frame)
{
	if(top_frame<1) return;
	if(plevels<3) return;
	if(plevels>plast_level+1) plevels=plast_level+1;
	if(ptree[2]==0) return;

	int *p1;

	PIXEL* f=(PIXEL *)GetBuffer(frame);
	p1=(int *)GetBuffer(TEMP_BUFFER);

	BeginThreads
	SetThreads(w,h)

	for(obj=0; obj<objects; obj++)
	{
		pmc3_params[obj].index=obj;
		pmc3_params[obj].step=objects;
		pmc3_params[obj].w=w;
		pmc3_params[obj].h=h;
		pmc3_params[obj].in=f;
		pmc3_params[obj].out=p1;
		StartThreads(pmcb,pmc3_params)
	}
	EndThreads

	psetEdges(p1,0);
	FreeBuffer(frame);
	FreeBuffer(TEMP_BUFFER); // OK because Max is using it
}

void CCombineDoc::ptempbuild()
{
	int level;

	int *p1,*p2;

	p1=(int *)GetBuffer(TEMP_BUFFER);
	p2=(int *)GetBuffer(TEMP_BUFFER+1);

	pdecend(p1,0,p2);

	for(level=1; level<=plast_level; level++)
	{
		pdecend(&p2[ptree[level]],level,&p2[ptree[level+1]]);
	}

	FreeBuffer(TEMP_BUFFER);
	FreeBuffer(TEMP_BUFFER+1);
}

struct pmc2_Params
{
	int index,step;
	int bigw,bigh,smallw,smallh;
	int plast;
	int *in,*out,*temp;
} pmc2_params[32];

UINT CCombineDoc::pmc2(LPVOID param)
{
	pmc2_Params *pp=(pmc2_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int bigw=pp->bigw;
	int bigh=pp->bigh;
	int smallw=pp->smallw;
	int smallh=pp->smallh;
	int plast=pp->plast;
	int *in=pp->in;
	int *out=pp->out;
	int *temp=pp->temp;

	int x,y,i,j,Y;
	int YY;

	int big1=bigw+1;
	int bigm1=bigw-1;
	int bighm1=bigh-1;
	int smallhm1=smallh-1;
	int smallwm1=smallw-1;

	for(y=2+index; y<smallhm1; y+=step)
	{
		Y=y*smallw;
		YY=(2*y-2)*bigw;
		for(x=2; x<smallwm1; x++)
		{
			i=x+Y;
			j=2*x-2+YY;

			out[i+plast]=out[i]=(
				temp[j]+
				temp[j+1]+
				temp[j+bigw]+
				temp[j+big1]
			)/4;
		}
	}
	return 0;
}

UINT CCombineDoc::pmc3(LPVOID param)
{
	pmc2_Params *pp=(pmc2_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int bigw=pp->bigw;
	int bigh=pp->bigh;
	int smallw=pp->smallw;
	int smallh=pp->smallh;
	int plast=pp->plast;
	int *in=pp->in;
	int *out=pp->out;
	int *temp=pp->temp;

	int x,y,i,j,Y,s;
	int YY;

	int big1=bigw+1;
	int bigm1=bigw-1;
	int bighm1=bigh-1;
	int smallhm1=smallh-1;
	int smallwm1=smallw-1;

	for(y=2+index; y<smallhm1; y+=step)
	{
		Y=y*smallw;
		YY=(2*y-2)*bigw;
		for(x=2; x<smallwm1; x++)
		{
			i=x+Y;
			j=2*x-2+YY;
			s=out[i];
			temp[j]=s;
			temp[j+1]=s;
			temp[j+bigw]=s;
			temp[j+big1]=s;
		}
	}

	return 0;
}

UINT CCombineDoc::pmc4(LPVOID param)
{
	pmc2_Params *pp=(pmc2_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int bigw=pp->bigw;
	int bigh=pp->bigh;
	int *in=pp->in;
	int *out=pp->out;
	int *temp=pp->temp;

	int x,y,i,Y;

	int big1=bigw+1;
	int bigm1=bigw-1;
	int bighm1=bigh-1;

	for(y=1+index; y<bighm1; y+=step)
	{
		Y=y*bigw;
		for(x=1; x<bigm1; x++)
		{
			i=x+Y;
			in[i]-=((
				1*temp[i-bigm1]+
				2*temp[i-bigw]+
				1*temp[i-bigm1]+
				2*temp[i-1]+
				4*temp[i]+
				2*temp[i+1]+
				1*temp[i+bigm1]+
				2*temp[i+bigw]+
				1*temp[i+big1]
			)/16);
		}
	}

	return 0;
}

void CCombineDoc::pdecend(int* in,int level,int* out)
{
	BeginThreads

	int* temp=(int *)GetBuffer(TEMP_BUFFER+3);

	int smallw=pyramidw[level+1];
	int smallh=pyramidh[level+1];
	int bigw=pyramidw[level];
	int bigh=pyramidh[level];

	psmoothe(in,level,temp);

	int big1=bigw+1;
	int bigm1=bigw-1;
	int bighm1=bigh-1;
	int smallhm1=smallh-1;
	int smallwm1=smallw-1;
// deflate
	SetThreads(smallw,smallh)
	for(obj=0; obj<objects; obj++)
	{
		if(smallh+1>(int)objects)
		{
			pmc2_params[obj].index=obj;
			pmc2_params[obj].step=objects;
		} else {
			pmc2_params[obj].index=0;
			pmc2_params[obj].step=objects=1;
		}
		pmc2_params[obj].bigw=bigw;
		pmc2_params[obj].bigh=bigh;
		pmc2_params[obj].smallw=smallw;
		pmc2_params[obj].smallh=smallh;
		pmc2_params[obj].plast=plast;
		pmc2_params[obj].in=in;
		pmc2_params[obj].out=out;
		pmc2_params[obj].temp=temp;
		StartThreads(pmc2,pmc2_params)
	}
	EndThreads

	psetEdges(out+plast,level+1);
	psetEdges(out,level+1);

// reinflate
	SetThreads(bigw,bigh)
	for(obj=0; obj<objects; obj++)
	{
		if(smallh+1>(int)objects)
		{
			pmc2_params[obj].index=obj;
			pmc2_params[obj].step=objects;
		} else {
			pmc2_params[obj].index=0;
			pmc2_params[obj].step=objects=1;
		}
		pmc2_params[obj].bigw=bigw;
		pmc2_params[obj].bigh=bigh;
		pmc2_params[obj].smallw=smallw;
		pmc2_params[obj].smallh=smallh;
		pmc2_params[obj].plast=plast;
		pmc2_params[obj].in=in;
		pmc2_params[obj].out=out;
		pmc2_params[obj].temp=temp;
		StartThreads(pmc3,pmc2_params)
	}
	EndThreads

	psetEdges(temp,level);
// smoothe and subtract from original
	for(obj=0; obj<objects; obj++)
	{
		if(bigh+1>(int)objects)
		{
			pmc2_params[obj].index=obj;
			pmc2_params[obj].step=objects;
		} else {
			pmc2_params[obj].index=0;
			pmc2_params[obj].step=objects=1;
		}
		pmc2_params[obj].bigw=bigw;
		pmc2_params[obj].bigh=bigh;
		pmc2_params[obj].in=in;
		pmc2_params[obj].out=out;
		pmc2_params[obj].temp=temp;
		StartThreads(pmc4,pmc2_params)
	}
	EndThreads

	psetEdges_small(in,level);

	FreeBuffer(TEMP_BUFFER+3);
}

struct pmc1_Params
{
	int index,step;
	int pw,ph;
	int *in,*out;
} pmc1_params[32];

UINT CCombineDoc::pmc1(LPVOID param)
{
	pmc1_Params *pp=(pmc1_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int pw=pp->pw;
	int ph=pp->ph;
	int *in=pp->in;
	int *out=pp->out;

	int i,j,x,y,Y;
	int sum;

	int m2=-2*pw;
	int m1=-pw;
	int p1=pw;
	int p2=2*pw;

	for(y=2+index; y<ph-2; y+=step)
	{
		Y=y*pw;

		sum=0;
		for(x=0; x<4; x++)
		{
			i=x+Y;
			sum+=(
				in[i+m2]+
				in[i+m1]+
				in[i]+
				in[i+p1]+
				in[i+p2]);
		}

		for(x=4; x<pw; x++)
		{
			i=x+Y;
			sum+=(
				in[i+m2]+
				in[i+m1]+
				in[i]+
				in[i+p1]+
				in[i+p2]);

			out[i-2]=(sum+12)/25;

			j=i-4;
			sum-=(
				in[j+m2]+
				in[j+m1]+
				in[j]+
				in[j+p1]+
				in[j+p2]);
		}
	}

	return 0;
}

void CCombineDoc::psmoothe(int *in,int level,int *out)
{
	int pw=pyramidw[level];
	int ph=pyramidh[level];

	BeginThreads
	SetThreads(pw,ph)

	for(obj=0; obj<objects; obj++)
	{
		if(ph+2>512)
		{
			pmc1_params[obj].index=obj;
			pmc1_params[obj].step=objects;
		} else {
			pmc1_params[obj].index=0;
			pmc1_params[obj].step=objects=1;
		}
		pmc1_params[obj].pw=pw;
		pmc1_params[obj].ph=ph;
		pmc1_params[obj].in=in;
		pmc1_params[obj].out=out;
		StartThreads(pmc1,pmc1_params)
	}
	EndThreads
}

void CCombineDoc::psetEdges(int *buffer,int level)
{
	int i,j,x,y,pY;
	int pw=pyramidw[level];
	int ph=pyramidh[level];

	bool OK;

	OK=false;

	if(level<2) OK=true;
	else 
	if(2*(pw-4)+4==pyramidw[level-1]) OK=true;
	if(OK)
	{
		for(y=0; y<ph; y++)
		{
			pY=y*pw;
			buffer[pY]=buffer[pY+1]=buffer[pY+2];
			pY=pY-1+pw;
			buffer[pY]=buffer[pY-1]=buffer[pY-2];
		}
	} else {
		for(y=0; y<ph; y++)
		{
			pY=y*pw;
			buffer[pY]=buffer[pY+1]=buffer[pY+2];
			pY=pY-1+pw;
			buffer[pY]=buffer[pY-1]=buffer[pY-2]=buffer[pY-3];
		}
	}

	OK=false;
	if(level==0) OK=true;
	else if(2*(ph-4)+4==pyramidh[level-1]) OK=true;
	if(OK)
	{
		for(x=0; x<pw; x++)
		{
			i=pw;
			buffer[x]=buffer[x+i]=buffer[x+2*i];
			j=x+pw*(ph-1);
			buffer[j]=buffer[j-i]=buffer[j-2*i];
		}
	} else {
		for(x=0; x<pw; x++)
		{
			i=pw;
			buffer[x]=buffer[x+i]=buffer[x+2*i];
			j=x+pw*(ph-1);
			buffer[j]=buffer[j-i]=buffer[j-2*i]=buffer[j-3*i];
		}
	}
}

void CCombineDoc::psetEdges_small(int *buffer,int level)
{
	int i,j,x,y,pY;
	int pw=pyramidw[level];
	int ph=pyramidh[level];

	bool OK;

	OK=false;

	if(level<2) OK=true;
	else 
	if(2*(pw-4)+4==pyramidw[level-1]) OK=true;
	if(OK)
	{
		for(y=0; y<ph; y++)
		{
			pY=y*pw;
			buffer[pY]=buffer[pY+1];
			pY=pY-1+pw;
			buffer[pY]=buffer[pY-1];
		}
	} else {
		for(y=0; y<ph; y++)
		{
			pY=y*pw;
			buffer[pY]=buffer[pY+1];
			pY=pY-1+pw;
			buffer[pY]=buffer[pY-1]=buffer[pY-2];
		}
	}

	OK=false;
	if(level==0) OK=true;
	else if(2*(ph-4)+4==pyramidh[level-1]) OK=true;
	if(OK)
	{
		for(x=0; x<pw; x++)
		{
			i=pw;
			buffer[x]=buffer[x+i];
			j=x+pw*(ph-1);
			buffer[j]=buffer[j-i];
		}
	} else {
		for(x=0; x<pw; x++)
		{
			i=pw;
			buffer[x]=buffer[x+i];
			j=x+pw*(ph-1);
			buffer[j]=buffer[j-i];
		}
	}
}

//***********************  out RGB

void CCombineDoc::OnPyramidPyramidtoout()
{
	mode=PYRAMID_OUT;
	if(!GetValue()){mode=0; return;}
	mode=0;

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;

	progress.AddString(IDS_POUT,pforeground,pbrightness,pbackground);
	refreshscreen=true;
	AfxBeginThread(PyramidToOutT,this);
}

UINT CCombineDoc::PyramidToOutT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->PyramidToOut();
	return 0;
}

void CCombineDoc::PyramidToOut()
{
	if(top_frame<1) return;

	int *t1,*t2,*t3;
	if(plast_level==0) return;

	t1=(int*)GetBuffer(TEMP_BUFFER+1);
	t2=(int*)GetBuffer(TEMP_BUFFER+2);
	t3=(int*)GetBuffer(TEMP_BUFFER+3);
	PyramidToOut1(1,t1);
	PyramidToOut1(2,t2);
	PyramidToOut1(3,t3);
	pout3(t1,t2,t3);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+2);
	FreeBuffer(TEMP_BUFFER+3);

	if(refreshscreen)
	{
		refreshscreen=false;
		progress.m_cancel=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		display=OUT_BUFFER;
		RefreshScreen();
	}
}

void CCombineDoc::PyramidToOut1(int number,int *t)
{
	if(top_frame<1) return;

	int *p1,*p2,*t2;

	int i,x,y,Y;

	if(plast_level==0) return;

	p1=(int*)GetBuffer(PYRAMID_BUFFER+number-1);
	t2=(int*)GetBuffer(TEMP_BUFFER);
	p2=(int*)GetBuffer(PYRAMID_BUFFER+number+2);

	for(y=0; y<h+16; y++)
	{
		Y=y*(w+16);
		for(x=0; x<w+16; x++)
		{
			i=x+Y;
			t[i]=p1[i];
			t2[i]=p2[i];
		}
	}
	FreeBuffer(PYRAMID_BUFFER+number-1);
	FreeBuffer(PYRAMID_BUFFER+number+2);

	int level=plast_level;
	for(y=0; y<pyramidh[level]; y++)
	{
		Y=y*pyramidw[level]+ptree[level];
		for(x=0; x<pyramidw[level]; x++)
		{
			i=x+Y;
			t2[i]=(t2[i])+(t2[plast+i]*pbackground)/100;
		}
	}

	if(level>1)
	{
		for(; level>1; level--)
		{
			pascend(&t2[ptree[level]],level,&t2[ptree[level-1]]);
		}
	}

	pascend(t2,1,t);

	FreeBuffer(TEMP_BUFFER);
}

void CCombineDoc::pascend(int* in,int level,int* out)
{
	int x,y,i,Y;
	int j,YY;
	int s;

	int *temp1=(int *)GetBuffer(TEMP_BUFFER+4);
	int *temp2=(int *)GetBuffer(TEMP_BUFFER+5);

	int bigw=pyramidw[level-1];
	int bigh=pyramidh[level-1];
	int smallw=pyramidw[level];
	int smallh=pyramidh[level];

	for(y=2; y<smallh-1; y++)
	{
		Y=y*smallw;
		YY=(2*y-2)*bigw;
		for(x=2; x<smallw-1; x++)
		{
			i=x+Y;
			j=(2*x-2)+YY;
			s=in[i];
			temp1[j]=s;
			temp1[j+1]=s;
			temp1[j+bigw]=s;
			temp1[j+bigw+1]=s;
		}
	}
	psetEdges(temp1,level-1);

	for(y=1; y<bigh-1; y++)
	{
		Y=y*bigw;
		for(x=1; x<bigw-1; x++)
		{
			i=x+Y;
			temp2[i]=(
				1*temp1[i-bigw-1]+
				2*temp1[i-bigw]+
				1*temp1[i-bigw+1]+
				2*temp1[i-1]+
				4*temp1[i]+
				2*temp1[i+1]+
				1*temp1[i+bigw-1]+
				2*temp1[i+bigw]+
				1*temp1[i+bigw+1]
			)/16;
		}
	}
	psetEdges_small(temp2,level-1);

	int contrast=100+((pforeground-100)*(12-level))/12;
	if(level>3) contrast=100;
	int bg=100+((pbackground-100)*(level)*(level))/(plast_level*plast_level);
	if(abs(level-plast_level)<4) bg=100;
	if(level!=1)
	{
		for(y=0; y<bigh; y++)
		{
			Y=y*bigw;
			for(x=0; x<bigw; x++)
			{
				i=x+Y;
				out[i]=(contrast*out[i])/100+(temp2[i]*bg)/100;
			}
		}
	} else {
		for(y=0; y<bigh; y++)
		{
			Y=y*bigw;
			for(x=0; x<bigw; x++)
			{
				i=x+Y;
				out[i]=(contrast*out[i])/100+temp2[i];
			}
		}
	}

	FreeBuffer(TEMP_BUFFER+4);
	FreeBuffer(TEMP_BUFFER+5);
}

void CCombineDoc::pout3(int *p1,int *p2,int *p3)
{
	int x,y,Y,pY,i,j;
	__int64 t1,t2,t3,m;
	PIXEL* f=(PIXEL *)GetBuffer(OUT_BUFFER);
	for(y=0; y<h; y++)
	{
		Y=y*w;
		pY=(y+2)*(w+4);
		for(x=0; x<w; x++)
		{
			i=x+Y;
			j=(x+2)+pY;
			f[i].a=aGOOD;
			t1=p1[j];
			t2=p2[j];
			t3=p3[j];

			m=t1; if(t2>m) m=t2; if(t3>m) m=t3;
			t1=((pbrightness)*t1)/1600;
			t2=((pbrightness)*t2)/1600;
			t3=((pbrightness)*t3)/1600;

			if(t1<0) t1=0;
			if(t2<0) t2=0;
			if(t3<0) t3=0;

			if(t1<0) t1=0; if(t1>255) t1=255;
			if(t2<0) t2=0; if(t2>255) t2=255;
			if(t3<0) t3=0; if(t3>255) t3=255;

			f[i].r=(unsigned char)t1;
			f[i].g=(unsigned char)t2;
			f[i].b=(unsigned char)t3;
		}
	}
	UpdateBuffer(OUT_BUFFER);
}

//**************** depth

void CCombineDoc::OnPyramidRecoverdepth()
{
	mode=PYRAMID_DEPTH;
	if(!GetValue()){mode=0; return;}
	mode=0;

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	progress.AddString(IDS_PDEPTH,plevels,pnoise);
	refreshscreen=true;
	AfxBeginThread(PyramidDepthT,this);
}

UINT CCombineDoc::PyramidDepthT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->PyramidDepth(_this->plevels,(_this->pnoise)<<4);
	return 0;
}

void CCombineDoc::PyramidDepth(int levels,int noise)
{
	if(top_frame<1) return;

	PDEPTH *tr,*tg,*tb;

	if(levels<3) levels=3;

	tr=(PDEPTH*)GetBuffer(TEMP_BUFFER+0);
	PyramidDepth1(1,levels,noise);
	UpdateBuffer(TEMP_BUFFER+0);

	tg=(PDEPTH*)GetBuffer(TEMP_BUFFER+1);
	PyramidDepth1(2,levels,noise);
	UpdateBuffer(TEMP_BUFFER+1);

	tb=(PDEPTH*)GetBuffer(TEMP_BUFFER+2);
	PyramidDepth1(3,levels,noise);

	tr=(PDEPTH*)GetBuffer(TEMP_BUFFER+0);
	tg=(PDEPTH*)GetBuffer(TEMP_BUFFER+1);
	pdout3();

	FreeBuffer(TEMP_BUFFER+0);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+2);

	if(refreshscreen)
	{
		backup_picture_valid=false;
		redraw_picture();
		progress.m_cancel=false;
		display=PICTURE_BUFFER;
		refreshscreen=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}
}

void CCombineDoc::PyramidDepth1(int number,int levels,int noise)
{
	int i;
	int level;

	int *t=(int*)GetBuffer(TEMP_BUFFER+3);
	int *p=(int*)GetBuffer(PYRAMID_BUFFER+number+2);
	int *p1=(int*)GetBuffer(PYRAMID_BUFFER+number-1);
	PDEPTH *pd=(PDEPTH*)GetBuffer(PYRAMID_BUFFER+number+5);
	PDEPTH *td=(PDEPTH*)GetBuffer(TEMP_BUFFER+number-1);
	for(i=0; i<(int)(slot_size/4); i++)
	{
		t[i]=p[i];
		if(abs(p1[i])>noise*2)
			td[i]=pd[i];
		else
			td[i].flags=td[i].frame=0;
	}
	FreeBuffer(PYRAMID_BUFFER+number+2);
	FreeBuffer(PYRAMID_BUFFER+number-1);
	FreeBuffer(PYRAMID_BUFFER+number+5);

	level=levels-1;
	if(level>1)
	{
		for(; level>1; level--)
		{
			pdascend(&t[ptree[level-1]],&t[ptree[level]],(PDEPTH *)(&t[ptree[level]+pdepth]),level,(PDEPTH *)(&t[ptree[level-1]+pdepth]),noise*level);
		}
	}

	p=(int*)GetBuffer(PYRAMID_BUFFER+number-1);
	pdascend(p,t,(PDEPTH *)(t+pdepth),1,td,noise);
	FreeBuffer(PYRAMID_BUFFER+number-1);

	FreeBuffer(TEMP_BUFFER+3);
	FreeBuffer(TEMP_BUFFER+number-1);
}

void CCombineDoc::pdascend(int* inlarge,int* insmall,PDEPTH* ind,int level,PDEPTH* out,int noise)
{
	int x,y,i,Y,j,Yp;

	int bigw=pyramidw[level-1];
	int bigh=pyramidh[level-1];
	int smallw=pyramidw[level];
	int smallh=pyramidh[level];

	for(y=2; y<bigh-2; y++)
	{
		Y=y*bigw;
		Yp=((y-2)/2+2)*smallw;
		for(x=2; x<bigw-2; x++)
		{
			i=x+Y;
			j=(x-2)/2+2+Yp;
			if((abs(insmall[j])>noise) && (ind[j].flags&PD_FILLED))
			{
				if(abs(insmall[j])>abs(inlarge[i]))
				{
					out[i]=ind[j];
					if(level!=1)
						inlarge[i]=insmall[j];
				}
			}
		}
	}
}

void CCombineDoc::pdout3()
{
	int x,y,Y,i,pY,j;
	int rf,gf,bf,r,g,b;
	int count;

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	PDEPTH *tr=(PDEPTH*)GetBuffer(TEMP_BUFFER);
	PDEPTH *tg=(PDEPTH*)GetBuffer(TEMP_BUFFER+1);
	PDEPTH *tb=(PDEPTH*)GetBuffer(TEMP_BUFFER+2);
	for(y=0; y<h; y++)
	{
		Y=y*w;
		pY=(y+2)*(w+4);
		for(x=0; x<w; x++)
		{
			i=x+Y;
			j=x+2+pY;
			if((depth[i]&fFIXED)==0)
			{
				depth[i]=fEMPTY;
				rf=(tr[j].flags&PD_FILLED);
				gf=(tg[j].flags&PD_FILLED);
				bf=(tb[j].flags&PD_FILLED);
				count=rf+gf+bf;
				if(count<2) continue;
				r=(int)tr[j].frame;
				g=(int)tg[j].frame;
				b=(int)tb[j].frame;
				if((r==g) && (rf&gf)){depth[i]=fFILLED|(r<<8); continue;}
				if((g==b) && (gf&bf)){depth[i]=fFILLED|(g<<8); continue;}
				if((b==r) && (bf&rf)) depth[i]=fFILLED|(b<<8);
			}
		}
	}
	UpdateBuffer(DEPTH_BUFFER);
	FreeBuffer(TEMP_BUFFER);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+2);
}

//**************** Weighted Average

void CCombineDoc::OnRgbweightedaverageruleAddallusedframes()
{
	if(plast_level==0)	PyramidClear();

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	progress.AddString(IDS_PADDWAUF);

	AfxBeginThread(PyramidWeightUFT,this);
}

UINT CCombineDoc::PyramidWeightUFT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;

	int f;
	_this->progress.SetRange(0,_this->last_used_frame-1);
	for(f=0; f<_this->last_used_frame; f++)
	{
		_this->progress.SetPos(f);
		if(f==_this->last_used_frame-1) _this->refreshscreen=true;
		_this->PyramidWeight(_this->used_frame[f]);
	}
	_this->progress.SetPos(0);

	return 0;
}

void CCombineDoc::OnRgbweightedaverageruleAddactivetypyramid()
{
	if(plast_level==0) PyramidClear();

	pframe=active_frame;

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;

	progress.AddString(IDS_PADDWEIGHT,pframe);
	AfxBeginThread(PyramidWeightT,this);
}

UINT CCombineDoc::PyramidWeightT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->PyramidWeight(_this->pframe);
	return 0;
}

void CCombineDoc::PyramidWeight(int frame)
{
	if(top_frame<1) return;

	if(plast_level==0) return;

	if(max_slots>14)
	{
		GetBuffer(PYRAMID_BUFFER+0);
		GetBuffer(PYRAMID_BUFFER+1);
		GetBuffer(PYRAMID_BUFFER+2);
		GetBuffer(PYRAMID_BUFFER+3);
		GetBuffer(PYRAMID_BUFFER+4);
		GetBuffer(PYRAMID_BUFFER+5);
		GetBuffer(PYRAMID_BUFFER+6);
		GetBuffer(PYRAMID_BUFFER+7);
		GetBuffer(PYRAMID_BUFFER+8);
		GetBuffer(TEMP_BUFFER+0);
		GetBuffer(TEMP_BUFFER+1);
		GetBuffer(TEMP_BUFFER+3);
		GetBuffer(TEMP_BUFFER+4);
		GetBuffer(TEMP_BUFFER+5);
	}

	pinw(frame);

	PyramidWeight1(1,frame,1);
	PyramidWeight1(2,frame,2);
	PyramidWeight1(3,frame,3);

	if(max_slots>14)
	{
		FreeBuffer(PYRAMID_BUFFER+0);
		FreeBuffer(PYRAMID_BUFFER+1);
		FreeBuffer(PYRAMID_BUFFER+2);
		FreeBuffer(PYRAMID_BUFFER+3);
		FreeBuffer(PYRAMID_BUFFER+4);
		FreeBuffer(PYRAMID_BUFFER+5);
		FreeBuffer(PYRAMID_BUFFER+6);
		FreeBuffer(PYRAMID_BUFFER+7);
		FreeBuffer(PYRAMID_BUFFER+8);
		FreeBuffer(TEMP_BUFFER+0);
		FreeBuffer(TEMP_BUFFER+1);
		FreeBuffer(TEMP_BUFFER+3);
		FreeBuffer(TEMP_BUFFER+4);
		FreeBuffer(TEMP_BUFFER+5);
	}
}

struct pwa1_Params
{
	int index,step;
	int ymax,xmax;
	int plast,pdepth,p,wtscale;
	int *p2,*t2,*wt2;
} pwa1_params[32];

UINT CCombineDoc::pwa1(LPVOID param)
{
	pwa1_Params *pp=(pwa1_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int ymax=pp->ymax;
	int xmax=pp->xmax;
	int plast=pp->plast;
	int pdepth=pp->pdepth;
	int p=pp->p;
	int wtscale=pp->wtscale;
	int *p2=pp->p2;
	int *t2=pp->t2;
	int *wt2=pp->wt2;

	int i,j,k,x,y,Y,a,aa;
	__int64 big;

	for(y=index; y<ymax; y+=step)
	{
		Y=y*xmax+p;
		for(x=0; x<xmax; x++)
		{
			i=x+Y;
			j=i+plast;
			k=i+pdepth;

			a=t2[i]>>4;
			aa=wt2[i]/wtscale;

			big=p2[i];
			big+=(__int64)(a*aa);
			if(big>0xffffffffL)
			{
				big/=2;
				aa/=2;
				p2[k]/=2;
			}
			p2[i]=(int)(big&0xffffffffL);

			p2[k]+=aa; // sum of weights

			p2[j]=t2[j]>>4; // remaining colour
		}
	}

	return 0;
}

struct pwa2_Params
{
	int index,step;
	int ymax,xmax;
	int wtscale;
	int *p1,*t1,*wt1,*swt;
} pwa2_params[32];

UINT CCombineDoc::pwa2(LPVOID param)
{
	pwa2_Params *pp=(pwa2_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int ymax=pp->ymax;
	int xmax=pp->xmax;
	int wtscale=pp->wtscale;
	int *p1=pp->p1;
	int *t1=pp->t1;
	int *wt1=pp->wt1;
	int *swt=pp->swt;

	int i,x,y,Y,a,aa;
	__int64 big;

	for(y=0; y<ymax; y++)
	{
		Y=y*xmax;
		for(x=0; x<xmax; x++)
		{
			i=x+Y;
			a=t1[i]>>4;
			aa=wt1[i]/wtscale;

			big=p1[i];
			big+=(__int64)(a*aa);
			if(big>0xffffffffL)
			{
				big/=2;
				aa/=2;
				swt[i]/=2;
			}
			p1[i]=(int)(big&0xffffffff);

			swt[i]+=aa;
		}
	}

	return 0;
}

void CCombineDoc::PyramidWeight1(int number, int frame,int channel)
{
	BeginThreads

	if(top_frame<1) return;

	int level;
	int *p1,*p2,*t1,*t2;
	int *wt1,*wt2,*swt;//,a,aa;
	int xmax,ymax,p;
	int wtscale=1;

	p1=(int *)GetBuffer(PYRAMID_BUFFER+number-1);
	t1=(int *)GetBuffer(TEMP_BUFFER); // input data is in here
	p2=(int *)GetBuffer(PYRAMID_BUFFER+number+2);
	t2=(int *)GetBuffer(TEMP_BUFFER+1); // input data is in here

	switch(channel)
	{
	case 1:
		pinr(frame);
		break;
	case 2:
		ping(frame);
		break;
	case 3:
		pinb(frame);
		break;
	}

//	__int64 big;
	ptempbuild();
	wt2=(int *)GetBuffer(TEMP_BUFFER+5); // input data is in here
	level=plast_level;
	while(level>0)
	{
		xmax=pyramidw[level];
		ymax=pyramidh[level];
		p=ptree[level];

		SetThreads(xmax,ymax)
		for(obj=0; obj<objects; obj++)
		{
			if(ymax>(int)objects)
			{
				pwa1_params[obj].index=obj;
				pwa1_params[obj].step=objects;
			} else {
				pwa1_params[obj].index=0;
				pwa1_params[obj].step=objects=1;
			}
			pwa1_params[obj].ymax=ymax;
			pwa1_params[obj].xmax=xmax;
			pwa1_params[obj].plast=plast;
			pwa1_params[obj].pdepth=pdepth;
			pwa1_params[obj].p=p;
			pwa1_params[obj].wtscale=wtscale;
			pwa1_params[obj].p2=p2;
			pwa1_params[obj].t2=t2;
			pwa1_params[obj].wt2=wt2;
			StartThreads(pwa1,pwa1_params)
		}
		EndThreads

		level--;
	}
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+5);
	wt1=(int *)GetBuffer(TEMP_BUFFER+4);
	swt=(int*)GetBuffer(PYRAMID_BUFFER+number+5);
	xmax=pyramidw[0];
	ymax=pyramidh[0];
	p=ptree[0];

	for(obj=0; obj<objects; obj++)
	{
		pwa2_params[obj].index=obj;
		pwa2_params[obj].step=objects;
		pwa2_params[obj].ymax=ymax;
		pwa2_params[obj].xmax=xmax;
		pwa2_params[obj].wtscale=wtscale;
		pwa2_params[obj].p1=p1;
		pwa2_params[obj].t1=t1;
		pwa2_params[obj].wt1=wt1;
		pwa2_params[obj].swt=swt;
		StartThreads(pwa2,pwa2_params);
	}
	EndThreads

	UpdateBuffer(PYRAMID_BUFFER+number+5);
	UpdateBuffer(PYRAMID_BUFFER+number-1);
	UpdateBuffer(PYRAMID_BUFFER+number+2);
	FreeBuffer(TEMP_BUFFER);
	FreeBuffer(TEMP_BUFFER+4);

	if(refreshscreen)
	{
		refreshscreen=false;
		progress.m_cancel=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}
}

struct pwa3_Params
{
	int index,step;
	int w,h;
	int pwsat,pwbrt;
	PIXEL *f;
	int *wt1;
} pwa3_params[32];

UINT CCombineDoc::pwa3(LPVOID param)
{
	pwa3_Params *pp=(pwa3_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	PIXEL *f=pp->f;
	int *wt1=pp->wt1;

	int x,y,i,j,Y,pY;
	int a,r,g,b;

	for(y=index; y<h; y+=step)
	{
		Y=y*w;
		pY=(y+2)*(w+4);
		for(x=0; x<w; x++)
		{
			i=x+Y;
			j=x+2+pY;

			r=(int)f[i].r;
			g=(int)f[i].g;
			b=(int)f[i].b;
			a=(r+g+b)/3;

			wt1[j]=a;
		}
	}

	return 0;
}

void CCombineDoc::pinw(int frame)
{
	BeginThreads
	SetThreads(w,h)

	if(top_frame<1) return;
	if(plevels<3) return;
	if(plevels>plast_level+1) plevels=plast_level+1;
	if(ptree[2]==0) return;

	int pwsat=plast;
	int pwbrt=pdepth;

	int *wt1,*wt2,level;//,a;//,*wt1sat,*wt1brt;

	PIXEL* f=(PIXEL *)GetBuffer(frame);
	wt1=(int *)GetBuffer(TEMP_BUFFER+4);

	for(obj=0; obj<objects; obj++)
	{
		pwa3_params[obj].index=obj;
		pwa3_params[obj].step=objects;
		pwa3_params[obj].w=w;
		pwa3_params[obj].h=h;
		pwa3_params[obj].wt1=wt1;
		pwa3_params[obj].f=f;
		StartThreads(pwa3,pwa3_params)
	}
	EndThreads

	FreeBuffer(frame);
	psetEdges(wt1,0);

	wt2=(int *)GetBuffer(TEMP_BUFFER+5);

	pdecendw(wt1,0,wt2);
	UpdateBuffer(TEMP_BUFFER+4);

	for(level=1; level<=plast_level; level++)
	{
		pdecendw(&wt2[ptree[level]],level,&wt2[ptree[level+1]]);
	}
	UpdateBuffer(TEMP_BUFFER+5);
}

struct pwa4_Params
{
	int index,step;
	int smallw,bigw;
	int smallh,bigh;
	int *out,*temp,*in;
} pwa4_params[32];

UINT CCombineDoc::pwa4(LPVOID param)
{
	pwa4_Params *pp=(pwa4_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int smallw=pp->smallw;
	int bigw=pp->bigw;
	int smallh=pp->smallh;
	int bigh=pp->bigh;
	int *out=pp->out;
	int *temp=pp->temp;

	int big1=bigw+1;
	int bigm1=bigw-1;
	int bighm1=bigh-1;
	int smallhm1=smallh-1;
	int smallwm1=smallw-1;
	int x,y,i,j,Y;
	int YY;

	for(y=2+index; y<smallhm1; y+=step)
	{
		Y=y*smallw;
		YY=(2*y-2)*bigw;
		for(x=2; x<smallwm1; x++)
		{
			i=x+Y;
			j=2*x-2+YY;

			out[i]=(
				temp[j]+
				temp[j+1]+
				temp[j+bigw]+
				temp[j+big1]
			)/4;
		}
	}

	return 0;
}

UINT CCombineDoc::pwa5(LPVOID param)
{
	pwa4_Params *pp=(pwa4_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int smallw=pp->smallw;
	int bigw=pp->bigw;
	int smallh=pp->smallh;
	int bigh=pp->bigh;
	int *out=pp->out;
	int *temp=pp->temp;

	int big1=bigw+1;
	int bigm1=bigw-1;
	int bighm1=bigh-1;
	int smallhm1=smallh-1;
	int smallwm1=smallw-1;
	int x,y,i,j,Y,s;
	int YY;

	for(y=2+index; y<smallhm1; y+=step)
	{
		Y=y*smallw;
		YY=(2*y-2)*bigw;
		for(x=2; x<smallwm1; x++)
		{
			i=x+Y;
			j=2*x-2+YY;
			s=out[i];
			temp[j]=s;
			temp[j+1]=s;
			temp[j+bigw]=s;
			temp[j+big1]=s;
		}
	}

	return 0;
}

UINT CCombineDoc::pwa6(LPVOID param)
{
	pwa4_Params *pp=(pwa4_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int smallw=pp->smallw;
	int bigw=pp->bigw;
	int smallh=pp->smallh;
	int bigh=pp->bigh;
	int *out=pp->out;
	int *temp=pp->temp;
	int *in=pp->in;

	int big1=bigw+1;
	int bigm1=bigw-1;
	int bighm1=bigh-1;
	int smallhm1=smallh-1;
	int smallwm1=smallw-1;
	int x,y,i,Y,aa;
//	int YY;

	for(y=1+index; y<bighm1; y+=step)
	{
		Y=y*bigw;
		for(x=1; x<bigm1; x++)
		{
			i=x+Y;
			aa=in[i]-((
				1*temp[i-bigm1]+
				2*temp[i-bigw]+
				1*temp[i-bigm1]+
				2*temp[i-1]+
				4*temp[i]+
				2*temp[i+1]+
				1*temp[i+bigm1]+
				2*temp[i+bigw]+
				1*temp[i+big1]
			)/16);

			in[i]=aa*aa;
		}
	}

	return 0;
}

void CCombineDoc::pdecendw(int* in,int level,int* out)
{
	BeginThreads

	int* temp=(int *)GetBuffer(TEMP_BUFFER+3);

	int smallw=pyramidw[level+1];
	int smallh=pyramidh[level+1];
	int bigw=pyramidw[level];
	int bigh=pyramidh[level];

	memcpy(temp,in,bigw*bigh*4);

	int big1=bigw+1;
	int bigm1=bigw-1;
	int bighm1=bigh-1;
	int smallhm1=smallh-1;
	int smallwm1=smallw-1;

// deflate
	SetThreads(smallw,smallh)
	for(obj=0; obj<objects; obj++)
	{
		if(smallh+1>(int)objects)
		{
			pwa4_params[obj].index=obj;
			pwa4_params[obj].step=objects;
		} else {
			pwa4_params[obj].index=0;
			pwa4_params[obj].step=objects=1;
		}
		pwa4_params[obj].smallw=smallw;
		pwa4_params[obj].bigw=bigw;
		pwa4_params[obj].smallh=smallh;
		pwa4_params[obj].bigh=bigh;
		pwa4_params[obj].out=out;
		pwa4_params[obj].temp=temp;
		StartThreads(pwa4,pwa4_params)
	}
	EndThreads

	psetEdges(out,level+1);

// reinflate
	SetThreads(bigw,bigh)
	for(obj=0; obj<objects; obj++)
	{
		if(smallh+1>(int)objects)
		{
			pwa4_params[obj].index=obj;
			pwa4_params[obj].step=objects;
		} else {
			pwa4_params[obj].index=0;
			pwa4_params[obj].step=objects=1;
		}
		pwa4_params[obj].smallw=smallw;
		pwa4_params[obj].bigw=bigw;
		pwa4_params[obj].smallh=smallh;
		pwa4_params[obj].bigh=bigh;
		pwa4_params[obj].out=out;
		pwa4_params[obj].temp=temp;
		StartThreads(pwa5,pwa4_params)
	}
	EndThreads

	psetEdges(temp,level);

// smoothe and subtract from original, compute final weights
	for(obj=0; obj<objects; obj++)
	{
		if(bigh+1>(int)objects)
		{
			pwa4_params[obj].index=obj;
			pwa4_params[obj].step=objects;
		} else {
			pwa4_params[obj].index=0;
			pwa4_params[obj].step=objects=1;
		}
		pwa4_params[obj].smallw=smallw;
		pwa4_params[obj].bigw=bigw;
		pwa4_params[obj].smallh=smallh;
		pwa4_params[obj].bigh=bigh;
		pwa4_params[obj].out=out;
		pwa4_params[obj].temp=temp;
		pwa4_params[obj].in=in;
		StartThreads(pwa6,pwa4_params)
	}
	EndThreads

	psetEdges_small(in,level);

	FreeBuffer(TEMP_BUFFER+3);
}

/* Weighted output */

void CCombineDoc::OnRgbweightedaverageruleResulttoout()
{
	mode=PYRAMID_WOUT;
	if(!GetValue()){mode=0; return;}
	mode=0;

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	refreshscreen=true;

	progress.AddString(IDS_PWOUT,pforeground,pbrightness,pbackground);
	AfxBeginThread(PyramidWToOutT,this);
}

UINT CCombineDoc::PyramidWToOutT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->PyramidWToOut();
	return 0;
}

void CCombineDoc::PyramidWToOut()
{
	if(top_frame<1) return;

	int *t1,*t2,*t3;
	if(plast_level==0) return;

	t1=(int*)GetBuffer(TEMP_BUFFER+1);
	t2=(int*)GetBuffer(TEMP_BUFFER+2);
	t3=(int*)GetBuffer(TEMP_BUFFER+3);
	PyramidWToOut1(1,t1);
	PyramidWToOut1(2,t2);
	PyramidWToOut1(3,t3);
	pWout3(t1,t2,t3);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+2);
	FreeBuffer(TEMP_BUFFER+3);

	if(refreshscreen)
	{
		refreshscreen=false;
		progress.m_cancel=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		display=OUT_BUFFER;
		RefreshScreen();
	}
}

void CCombineDoc::PyramidWToOut1(int number,int *t)
{
	if(top_frame<1) return;

	int *p1,*p2,*t2,*wt;

	int i,x,y,Y,a;

	p1=(int*)GetBuffer(PYRAMID_BUFFER+number-1);
	t2=(int*)GetBuffer(TEMP_BUFFER);
	p2=(int*)GetBuffer(PYRAMID_BUFFER+number+2);

	for(y=0; y<h+16; y++)
	{
		Y=y*(w+16);
		for(x=0; x<w+16; x++)
		{
			i=x+Y;
			t[i]=p1[i];
			t2[i]=p2[i];
		}
	}
	FreeBuffer(PYRAMID_BUFFER+number-1);
	FreeBuffer(PYRAMID_BUFFER+number+2);

	int level=plast_level;
	for(y=0; y<pyramidh[level]; y++)
	{
		Y=y*pyramidw[level]+ptree[level];
		for(x=0; x<pyramidw[level]; x++)
		{
			i=x+Y;
			a=t2[i+pdepth]; // sum of weights
			if(a!=0)
				t2[i]=(t2[i]/a)+(t2[i+plast]*pbackground)/100;
			else
				t2[i]=(t2[i+plast]*pbackground)/100;
		}
	}

	if(level>1)
	{
		for(; level>1; level--)
		{
			pWascend(&t2[ptree[level]],level,&t2[ptree[level-1]],0);
		}
	}

	wt=(int*)GetBuffer(PYRAMID_BUFFER+number+5);
	pWascend(t2,1,t,wt);
	FreeBuffer(PYRAMID_BUFFER+number+5);
	FreeBuffer(TEMP_BUFFER);
}

void CCombineDoc::pWascend(int* in,int level,int* out,int *wt)
{
	int x,y,i,Y;
	int j,YY;
	int s,a;
	__int64 big;

	int *temp1=(int *)GetBuffer(TEMP_BUFFER+4);
	int *temp2=(int *)GetBuffer(TEMP_BUFFER+5);

	int bigw=pyramidw[level-1];
	int bigh=pyramidh[level-1];
	int smallw=pyramidw[level];
	int smallh=pyramidh[level];

	for(y=2; y<smallh-1; y++)
	{
		Y=y*smallw;
		YY=(2*y-2)*bigw;
		for(x=2; x<smallw-1; x++)
		{
			i=x+Y;
			j=(2*x-2)+YY;
			s=in[i];
			temp1[j]=s;
			temp1[j+1]=s;
			temp1[j+bigw]=s;
			temp1[j+bigw+1]=s;
		}
	}
	psetEdges(temp1,level-1);

	for(y=1; y<bigh-1; y++)
	{
		Y=y*bigw;
		for(x=1; x<bigw-1; x++)
		{
			i=x+Y;
			temp2[i]=(
				1*temp1[i-bigw-1]+
				2*temp1[i-bigw]+
				1*temp1[i-bigw+1]+
				2*temp1[i-1]+
				4*temp1[i]+
				2*temp1[i+1]+
				1*temp1[i+bigw-1]+
				2*temp1[i+bigw]+
				1*temp1[i+bigw+1]
			)/16;
		}
	}
	psetEdges_small(temp2,level-1);

	int contrast=100+((pforeground-100)*(12-level))/12;
	if(level>3) contrast=100;
//	int bg=100+((pbackground-100)*(level)*(level))/(plast_level*plast_level);
	int bg=100+((pbackground-100)*level)/(plast_level*8);
	if(abs(level-plast_level)<2) bg=100;
	if(level!=1)
	{
		for(y=0; y<bigh; y++)
		{
			Y=y*bigw;
			for(x=0; x<bigw; x++)
			{
				i=x+Y;

				a=(100*out[i+pdepth])/contrast;
				big=(__int64)out[i];
				if(a!=0)
					out[i]=(int)((big/(__int64)a)+((temp2[i]*bg)/100L));
				else
					out[i]=(int)(temp2[i]*bg)/100;
			}
		}
	} else {
		for(y=0; y<bigh; y++)
		{
			Y=y*bigw;
			for(x=0; x<bigw; x++)
			{
				i=x+Y;

				a=(100*wt[i])/contrast;
				big=(__int64)out[i];
				if(a!=0)
					out[i]=(int)((big/(__int64)a)+temp2[i]);
				else
					out[i]=temp2[i];
			}
		}
	}

	FreeBuffer(TEMP_BUFFER+4);
	FreeBuffer(TEMP_BUFFER+5);
}

void CCombineDoc::pWout3(int *p1,int *p2,int *p3)
{
	int x,y,Y,pY,i,j;
	__int64 t1,t2,t3;
	PIXEL* f=(PIXEL *)GetBuffer(OUT_BUFFER);
	for(y=0; y<h; y++)
	{
		Y=y*w;
		pY=(y+2)*(w+4);
		for(x=0; x<w; x++)
		{
			i=x+Y;
			j=(x+2)+pY;
			f[i].a=aGOOD;
			t1=16*p1[j];
			t2=16*p2[j];
			t3=16*p3[j];

			t1=(pbrightness*t1)/1600;
			t2=(pbrightness*t2)/1600;
			t3=(pbrightness*t3)/1600;

			if(t1<0) t1=0; if(t1>255) t1=255;
			if(t2<0) t2=0; if(t2>255) t2=255;
			if(t3<0) t3=0; if(t3>255) t3=255;

			f[i].r=(unsigned char)t1;
			f[i].g=(unsigned char)t2;
			f[i].b=(unsigned char)t3;
		}
	}
	UpdateBuffer(OUT_BUFFER);
}
