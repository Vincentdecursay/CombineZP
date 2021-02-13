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

// adjust frames to a similar average tint
void CCombineDoc::OnBalance() 
{
	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;
	AfxBeginThread(balance,this);
}

UINT CCombineDoc::balance(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->balance();
	return 0;
}

void CCombineDoc::balance()
{
	buissy=true;
	progress.AddString(IDS_BALANCE);
	int f,x,y,Y;
	int i;
	int *rsum,*gsum,*bsum;
	rsum=new int[top_used_frame];
	gsum=new int[top_used_frame];
	bsum=new int[top_used_frame];
	int	maxrsum=0x7fffffff,maxgsum=0x7fffffff,maxbsum=0x7fffffff;
	PIXEL *f1,p;

	if((active_frame>used_frame[last_used_frame-1]) ||
		(active_frame<used_frame[0]))
		active_frame=used_frame[last_used_frame/2];

	progress.SetRange(0,2*top_used_frame-1);

// find biggist and frame rgb sums 
	for(f=0; f<top_used_frame; f++)
	{
		progress.SetPos(f);
		if(progress.m_cancel) break;

		f1=(PIXEL*)GetBuffer(used_frame[f]);

		rsum[f]=gsum[f]=bsum[f]=0;
		for(y=0; y<h; y+=(1+h/100))
		{
			Y=y*(int)w;
			for(x=0; x<w; x+=(1+w/100))
			{
				i=x+Y;
				p=f1[i];
				rsum[f]+=(int)p.r;
				gsum[f]+=(int)p.g;
				bsum[f]+=(int)p.b;
			}
		}
		if(rsum[f]==0) rsum[f]=1;
		if(gsum[f]==0) gsum[f]=1;
		if(bsum[f]==0) bsum[f]=1;
		if(used_frame[f]==active_frame)
		{
			maxrsum=rsum[f];
			maxgsum=gsum[f];
			maxbsum=bsum[f];
		}
		FreeBuffer(used_frame[f]);
	}

	int pr,pg,pb;
	int rfactor,gfactor,bfactor,c,rgbfactor;
	int lookupr[256],lookupg[256],lookupb[256];
	for(f=0; f<top_used_frame; f++)
	{
		progress.SetPos(f+top_used_frame-1);
		if(progress.m_cancel) break;

		rfactor=(maxrsum*64)/rsum[f];
		gfactor=(maxgsum*64)/gsum[f];
		bfactor=(maxbsum*64)/bsum[f];

		f1=(PIXEL*)GetBuffer(used_frame[f]);

		for(i=0; i<256; i++)
		{
			c=(rfactor*i)/64; if(c>255) c=255; lookupr[i]=c;
			c=(gfactor*i)/64; if(c>255) c=255; lookupg[i]=c;
			c=(bfactor*i)/64; if(c>255) c=255; lookupb[i]=c;
		}

		for(y=0; y<h; y++)
		{
			Y=(int)w*y;
			for(x=0; x<w; x++)
			{
				i=x+Y;
				p=f1[i];
				pr=(int)p.r; pg=(int)p.g; pb=(int)p.b;
				pr=lookupr[pr];
				pg=lookupg[pg];
				pb=lookupb[pb];
				if((pr==255) || (pg==255) || (pb==255))
				{
					pr=(rfactor*(int)p.r)/64;
					pg=(gfactor*(int)p.g)/64;
					pb=(bfactor*(int)p.b)/64;
					rgbfactor=pr;
					if(pg>rgbfactor) rgbfactor=pg;
					if(pb>rgbfactor) rgbfactor=pb;
					if(rgbfactor<1) rgbfactor=1;
					pr=(pr*255)/rgbfactor; if(pr>255) pr=255;
					pg=(pg*255)/rgbfactor; if(pg>255) pg=255;
					pb=(pb*255)/rgbfactor; if(pb>255) pb=255;
				}
				p.r=(unsigned char)pr;
				p.g=(unsigned char)pg;
				p.b=(unsigned char)pb;
				f1[i]=p;
			}
		}
		UpdateBuffer(used_frame[f]);
	}
	display=active_frame;
	delete [] rsum;
	delete [] gsum;
	delete [] bsum;
	progress.SetPos(0);
	backup_picture_valid=false;
	if(refreshscreen)
	{
		redraw_picture();
		progress.m_cancel=false;
		refreshscreen=false;
		buissy=false;;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}
}

void CCombineDoc::OnBalancecolourandbrightnessGlobalbrightness()
{
	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;
	AfxBeginThread(balancebrightness,this);
}

UINT CCombineDoc::balancebrightness(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->balancebrightness();
	return 0;
}

void CCombineDoc::balancebrightness()
{
	buissy=true;
	progress.AddString(IDS_BALBRIGHT);
	int f,x,y,Y;
	int i;
	int *rsum,*gsum,*bsum;
	rsum=new int[top_used_frame];
	gsum=new int[top_used_frame];
	bsum=new int[top_used_frame];
	int	maxrsum=0x7fffffff,maxgsum=0x7fffffff,maxbsum=0x7fffffff;
	PIXEL *f1,p;

	if((active_frame>used_frame[last_used_frame-1]) ||
		(active_frame<used_frame[0]))
		active_frame=used_frame[last_used_frame/2];

	progress.SetRange(0,2*top_used_frame-1);

// find biggist and frame rgb sums 
	for(f=0; f<top_used_frame; f++)
	{
		progress.SetPos(f);
		if(progress.m_cancel) break;

		f1=(PIXEL*)GetBuffer(used_frame[f]);

		rsum[f]=gsum[f]=bsum[f]=0;
		for(y=0; y<h; y+=(1+h/100))
		{
			Y=y*(int)w;
			for(x=0; x<w; x+=(1+w/100))
			{
				i=x+Y;
				p=f1[i];
				rsum[f]+=(int)p.r;
				gsum[f]+=(int)p.g;
				bsum[f]+=(int)p.b;
			}
		}
		if(rsum[f]==0) rsum[f]=1;
		if(gsum[f]==0) gsum[f]=1;
		if(bsum[f]==0) bsum[f]=1;
		if(used_frame[f]==active_frame)
		{
			maxrsum=rsum[f];
			maxgsum=gsum[f];
			maxbsum=bsum[f];
		}
		FreeBuffer(used_frame[f]);
	}

	int pr,pg,pb;
	int factor,c,rgbfactor;
	int lookup[256];
	for(f=0; f<top_used_frame; f++)
	{
		progress.SetPos(f+top_used_frame-1);
		if(progress.m_cancel) break;

		factor=((maxrsum+maxgsum+maxbsum)*64)/((rsum[f]+gsum[f]+bsum[f]));

		f1=(PIXEL*)GetBuffer(used_frame[f]);

		for(i=0; i<256; i++)
		{
			c=(factor*i)/64; if(c>255) c=255; lookup[i]=c;
		}

		for(y=0; y<h; y++)
		{
			Y=(int)w*y;
			for(x=0; x<w; x++)
			{
				i=x+Y;
				p=f1[i];
				pr=(int)p.r; pg=(int)p.g; pb=(int)p.b;
				pr=lookup[pr];
				pg=lookup[pg];
				pb=lookup[pb];
				if((pr==255) || (pg==255) || (pb==255))
				{
					pr=(factor*p.r)/64;
					pg=(factor*p.g)/64;
					pb=(factor*p.b)/64;
					rgbfactor=pr;
					if(pg>rgbfactor) rgbfactor=pg;
					if(pb>rgbfactor) rgbfactor=pb;
					if(rgbfactor<1) rgbfactor=1;
					pr=(pr*255)/rgbfactor; if(pr>255) pr=255;
					pg=(pg*255)/rgbfactor; if(pg>255) pg=255;
					pb=(pb*255)/rgbfactor; if(pb>255) pb=255;
				}
				p.r=(unsigned char)pr;
				p.g=(unsigned char)pg;
				p.b=(unsigned char)pb;
				f1[i]=p;
			}
		}
		UpdateBuffer(used_frame[f]);
	}
	display=active_frame;
	delete [] rsum;
	delete [] gsum;
	delete [] bsum;
	progress.SetPos(0);
	backup_picture_valid=false;
	if(refreshscreen)
	{
		redraw_picture();
		progress.m_cancel=false;
		refreshscreen=false;
		buissy=false;;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}
}

void CCombineDoc::OnStackBalancecolourandbrightnessLocal() 
{
	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;
	AfxBeginThread(localbalance,this);
}

UINT CCombineDoc::localbalance(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->localbalance();
	return 0;
}

void CCombineDoc::localbalance()
{
	buissy=true;
	progress.AddString(IDS_LOCALBALANCE);

	bool save_refresh=refreshscreen;
	refreshscreen=false;

	if((active_frame>used_frame[last_used_frame-1]) ||
		(active_frame<used_frame[0]))
		active_frame=used_frame[last_used_frame/2];

	int saved_af=active_frame;
	int af=UseFrame(active_frame);

	int f,x,y,Y,t;
	int i,count=0;
	PIXEL *f0,*f1,*t0,*t1,p,u,v;
	int pr,pg,pb;

	if(!dft.initialized)
		dftReset();

	mode=0;
	dft.displacement=0.0f;
	dft.width=0.1f;
	ImmediateLowpass();
	dft.out_mode=OUT_NORMAL;

	progress.SetRange(0,top_used_frame-1);
	if(af<top_used_frame-1)
	{
		for(f=af+1; f<top_used_frame; f++)
		{
			progress.SetPos(count);
			count++;
			if(progress.m_cancel) break;

			t0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);
			f0=(PIXEL*)GetBuffer(used_frame[f-1]);
			filterframe(f0,t0);
			UpdateBuffer(used_frame[f-1]);
			UpdateBuffer(TEMP_BUFFER+0);

			t1=(PIXEL*)GetBuffer(TEMP_BUFFER+1);
			f1=(PIXEL*)GetBuffer(used_frame[f]);
			filterframe(f1,t1);

			t0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);

			for(y=0; y<h; y++)
			{
				Y=(int)w*y;
				for(x=0; x<w; x++)
				{
					i=x+Y;
					p=f1[i];
					u=t0[i];
					v=t1[i];

					if((p.r<5) && (p.g<5) && (p.b<5))
					{
						pr=p.r; pg=p.g; pb=p.b;
					} else {
						if(v.r!=0)
						{
							pr=(((int)u.r)*(int)p.r+(int)v.r/2)/(int)v.r;
						} else pr=0;

						if(v.g!=0)
						{
							pg=(((int)u.g)*(int)p.g+(int)v.g/2)/(int)v.g;
						} else pg=0;

						if(v.b!=0)
						{
							pb=(((int)u.b)*(int)p.b+(int)v.b/2)/(int)v.b;
						} else pb=0;
					}

					if((pr>255) || (pg>255) || (pb>255))
					{
						t=pr;
						if(pg>t) t=pg;
						if(pb>t) t=pb;
						pr=(pr*255)/t;
						pg=(pg*255)/t;
						pb=(pb*255)/t;
					}

					if(pr>255) pr=255; p.r=(unsigned char)pr;
					if(pg>255) pg=255; p.g=(unsigned char)pg;
					if(pb>255) pb=255; p.b=(unsigned char)pb;

					f1[i]=p;
				}
			}
			UpdateBuffer(TEMP_BUFFER+0);
			UpdateBuffer(used_frame[f]);
			UpdateBuffer(TEMP_BUFFER+1);
		}
	}

	if(af>0)
	{
		for(f=af-1; f>=0; f--)
		{
			progress.SetPos(count);
			count++;
			if(progress.m_cancel) break;

			t0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);
			f0=(PIXEL*)GetBuffer(used_frame[f+1]);
			filterframe(f0,t0);
			UpdateBuffer(used_frame[f+1]);
			UpdateBuffer(TEMP_BUFFER+0);

			t1=(PIXEL*)GetBuffer(TEMP_BUFFER+1);
			f1=(PIXEL*)GetBuffer(used_frame[f]);
			filterframe(f1,t1);

			t0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);

			for(y=0; y<h; y++)
			{
				Y=(int)w*y;
				for(x=0; x<w; x++)
				{
					i=x+Y;
					p=f1[i];
					u=t0[i];
					v=t1[i];

					if((p.r<5) && (p.g<5) && (p.b<5))
					{
						pr=pg=pb=0;
					} else {
						if(v.r!=0)
						{
							pr=(((int)u.r)*(int)p.r+(int)v.r/2)/(int)v.r;
						} else pr=0;

						if(v.g!=0)
						{
							pg=(((int)u.g)*(int)p.g+(int)v.g/2)/(int)v.g;
						} else pg=0;

						if(v.b!=0)
						{
							pb=(((int)u.b)*(int)p.b+(int)v.b/2)/(int)v.b;
						} else pb=0;
					}

					if((pr>255) || (pg>255) || (pb>255))
					{
						t=pr;
						if(pg>t) t=pg;
						if(pb>t) t=pb;
						pr=(pr*255)/t;
						pg=(pg*255)/t;
						pb=(pb*255)/t;
					}

					if(pr>255) pr=255; p.r=(unsigned char)pr;
					if(pg>255) pg=255; p.g=(unsigned char)pg;
					if(pb>255) pb=255; p.b=(unsigned char)pb;

					f1[i]=p;
				}
			}
			UpdateBuffer(TEMP_BUFFER+0);
			UpdateBuffer(used_frame[f]);
			UpdateBuffer(TEMP_BUFFER+1);
		}
	}

	progress.SetPos(0);
	backup_picture_valid=false;
	if(save_refresh)
	{
		redraw_picture();
		progress.m_cancel=false;
		refreshscreen=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}
}

void CCombineDoc::OnBalancecolourandbrightnessLocalcolouronly()
{
	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;
	AfxBeginThread(localbalancecolour,this);
}

UINT CCombineDoc::localbalancecolour(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->localbalancecolour();
	return 0;
}

void CCombineDoc::localbalancecolour()
{
	buissy=true;
	progress.AddString(IDS_BALCOLOUR);

	bool save_refresh=refreshscreen;
	refreshscreen=false;

	if((active_frame>used_frame[last_used_frame-1]) ||
		(active_frame<used_frame[0]))
		active_frame=used_frame[last_used_frame/2];

	int saved_af=active_frame;
	int af=UseFrame(active_frame);

	int f,x,y,Y;
	int i,count=0;
	PIXEL *f0,*f1,*t0,*t1,p,u,v;
	int pr,pg,pb;

	if(!dft.initialized) dftReset();

	mode=0;
	dft.displacement=0.0f;
	dft.width=0.1f;
	ImmediateLowpass();
	dft.out_mode=OUT_NORMAL;

	progress.SetRange(0,top_used_frame-1);

	if(af<top_used_frame-1)
	{
		for(f=af+1; f<top_used_frame; f++)
		{
			progress.SetPos(count);
			count++;
			if(progress.m_cancel) break;

			t0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);
			f0=(PIXEL*)GetBuffer(used_frame[f-1]);
			filterframe(f0,t0);
			UpdateBuffer(used_frame[f-1]);
			UpdateBuffer(TEMP_BUFFER+0);

			t1=(PIXEL*)GetBuffer(TEMP_BUFFER+1);
			f1=(PIXEL*)GetBuffer(used_frame[f]);
			filterframe(f1,t1);
			t0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);

			for(y=0; y<h; y++)
			{
				Y=(int)w*y;
				for(x=0; x<w; x++)
				{
					i=x+Y;
					p=f1[i];
					u=t0[i];
					v=t1[i];

					if(v.r!=0)
					{
						pr=(((int)u.r)*(int)p.r)/(int)v.r;
					} else pr=0;

					if(v.g!=0)
					{
						pg=(((int)u.g)*(int)p.g)/(int)v.g;
					} else pg=0;

					if(v.b!=0)
					{
						pb=(((int)u.b)*(int)p.b)/(int)v.b;
					} else pb=0;

					int s1=pr+pg+pb;
					int s2=f1[i].r+f1[i].g+f1[i].b;
					int r=0,g=0,b=0;

					if(s1!=0)
					{
						r=((int)pr*s2)/s1;
						g=((int)pg*s2)/s1;
						b=((int)pb*s2)/s1;
					}
					int max=r; if(g>max) max=g; if(b>max) max=b;
					if(max!=0){if(max>255){r=(255*r)/max; g=(255*g)/max; b=(255*b)/max;}}
					p.r=(unsigned char)r;
					p.g=(unsigned char)g;
					p.b=(unsigned char)b;

					f1[i]=p;
				}
			}
			UpdateBuffer(TEMP_BUFFER+0);
			UpdateBuffer(used_frame[f]);
			UpdateBuffer(TEMP_BUFFER+1);
		}
	}

	if(af>0)
	{
		for(f=af-1; f>=0; f--)
		{
			progress.SetPos(count);
			count++;
			if(progress.m_cancel) break;

			t0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);
			f0=(PIXEL*)GetBuffer(used_frame[f+1]);
			filterframe(f0,t0);
			UpdateBuffer(used_frame[f+1]);
			UpdateBuffer(TEMP_BUFFER+0);

			t1=(PIXEL*)GetBuffer(TEMP_BUFFER+1);
			f1=(PIXEL*)GetBuffer(used_frame[f]);
			filterframe(f1,t1);

			t0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);

			for(y=0; y<h; y++)
			{
				Y=(int)w*y;
				for(x=0; x<w; x++)
				{
					i=x+Y;
					p=f1[i];
					u=t0[i];
					v=t1[i];

					if(v.r!=0)
					{
						pr=(((int)u.r+1)*(int)p.r)/(int)v.r;
					} else pr=0;

					if(v.g!=0)
					{
						pg=(((int)u.g+1)*(int)p.g)/(int)v.g;
					} else pg=0;

					if(v.b!=0)
					{
						pb=(((int)u.b+1)*(int)p.b)/(int)v.b;
					} else pb=0;

					int s1=pr+pg+pb;
					int s2=f1[i].r+f1[i].g+f1[i].b;
					int r=0,g=0,b=0;

					if(s1!=0)
					{
						r=((int)pr*s2)/s1;
						g=((int)pg*s2)/s1;
						b=((int)pb*s2)/s1;
					}
					int max=r; if(g>max) max=g; if(b>max) max=b;
					if(max!=0){if(max>255){r=(255*r)/max; g=(255*g)/max; b=(255*b)/max;}}
					p.r=(unsigned char)r;
					p.g=(unsigned char)g;
					p.b=(unsigned char)b;

					f1[i]=p;
				}
			}
			UpdateBuffer(TEMP_BUFFER+0);
			UpdateBuffer(used_frame[f]);
			UpdateBuffer(TEMP_BUFFER+1);
		}
	}

	progress.SetPos(0);
	backup_picture_valid=false;
	if(save_refresh)
	{
		redraw_picture();
		progress.m_cancel=false;
		refreshscreen=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}
}
