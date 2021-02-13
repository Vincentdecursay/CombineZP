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
//	int	maxrsum=0,maxgsum=0,maxbsum=0;
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

		rsum[f]=gsum[f]=bsum[f]=1;
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
		if(used_frame[f]==active_frame)
		{
			maxrsum=rsum[f];
			maxgsum=gsum[f];
			maxbsum=bsum[f];
		}
		FreeBuffer(used_frame[f]);
	}

	int rfactor,gfactor,bfactor,c;
	unsigned char lookupr[256],lookupg[256],lookupb[256];
	for(f=0; f<top_used_frame; f++)
	{
		progress.SetPos(f+top_used_frame-1);
		if(progress.m_cancel) break;

		rfactor=maxrsum/(rsum[f]/4096);
		gfactor=maxgsum/(gsum[f]/4096);
		bfactor=maxbsum/(bsum[f]/4096);

//		if((rfactor>4000) && (gfactor>4000) && (bfactor)>4000) continue;

		f1=(PIXEL*)GetBuffer(used_frame[f]);

		for(i=0; i<256; i++)
		{
			c=(rfactor*i)/4096; if(c>255) c=255; lookupr[i]=(unsigned char)c;
			c=(gfactor*i)/4096; if(c>255) c=255; lookupg[i]=(unsigned char)c;
			c=(bfactor*i)/4096; if(c>255) c=255; lookupb[i]=(unsigned char)c;
		}

		for(y=0; y<h; y++)
		{
			Y=(int)w*y;
			for(x=0; x<w; x++)
			{
				i=x+Y;
				p=f1[i];
				p.r=lookupr[(int)p.r];
				p.g=lookupg[(int)p.g];
				p.b=lookupb[(int)p.b];
				f1[i]=p;
			}
		}
		UpdateBuffer(used_frame[f]);
	}
	display=active_frame;
	delete rsum;
	delete gsum;
	delete bsum;
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
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
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
	PIXEL *f0,*f1,*t0,*t1,p,q,u,v;

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
			for(y=0; y<h; y++)
			{
				Y=w*y;
				for(x=0; x<w; x++)
				{
					i=x+Y;
					p=f0[i]; q=t0[i];
					if(abs((int)p.r-(int)q.r)<5) f0[i].r=q.r;
					if(abs((int)p.g-(int)q.g)<5) f0[i].g=q.g;
					if(abs((int)p.b-(int)q.b)<5) f0[i].b=q.b;
				}
			}
			UpdateBuffer(used_frame[f-1]);
			UpdateBuffer(TEMP_BUFFER+0);

			t1=(PIXEL*)GetBuffer(TEMP_BUFFER+1);
			f1=(PIXEL*)GetBuffer(used_frame[f]);
			filterframe(f1,t1);
			for(y=0; y<h; y++)
			{
				Y=w*y;
				for(x=0; x<w; x++)
				{
					i=x+Y;
					p=f1[i]; q=t1[i];
					if(abs((int)p.r-(int)q.r)<5) f1[i].r=q.r;
					if(abs((int)p.g-(int)q.g)<5) f1[i].g=q.g;
					if(abs((int)p.b-(int)q.b)<5) f1[i].b=q.b;
				}
			}

			t0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);
//			f0=(PIXEL*)GetBuffer(used_frame[f-1]);

			for(y=0; y<h; y++)
			{
				Y=(int)w*y;
				for(x=0; x<w; x++)
				{
					i=x+Y;
					p=f1[i];
					u=t0[i];
					v=t1[i];

					if((int)u.r+(int)u.g+(int)u.b>=(int)v.r+(int)v.g+(int)v.b) continue;//{temp=p; p=q; q=temp;}

					if(v.r!=0)
					{
						t=(((int)u.r+1)*(int)p.r)/(int)v.r;
						if(t>255) t=255;
						p.r=(unsigned char)t;
					} else p.r=0;

					if(v.g!=0)
					{
						t=(((int)u.g+1)*(int)p.g)/(int)v.g;
						if(t>255) t=255;
						p.g=(unsigned char)t;
					} else p.g=0;

					if(v.b!=0)
					{
						t=(((int)u.b+1)*(int)p.b)/(int)v.b;
						if(t>255) t=255;
						p.b=(unsigned char)t;
					} else p.b=0;

					f1[i]=p;
				}
			}
//			UpdateBuffer(used_frame[f-1]);
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
			for(y=0; y<h; y++)
			{
				Y=w*y;
				for(x=0; x<w; x++)
				{
					i=x+Y;
					p=f0[i]; q=t0[i];
					if(abs((int)p.r-(int)q.r)<5) f0[i].r=q.r;
					if(abs((int)p.g-(int)q.g)<5) f0[i].g=q.g;
					if(abs((int)p.b-(int)q.b)<5) f0[i].b=q.b;
				}
			}
			UpdateBuffer(used_frame[f+1]);
			UpdateBuffer(TEMP_BUFFER+0);

			t1=(PIXEL*)GetBuffer(TEMP_BUFFER+1);
			f1=(PIXEL*)GetBuffer(used_frame[f]);
			filterframe(f1,t1);
			for(y=0; y<h; y++)
			{
				Y=w*y;
				for(x=0; x<w; x++)
				{
					i=x+Y;
					p=f1[i]; q=t1[i];
					if(abs((int)p.r-(int)q.r)<5) f1[i].r=q.r;
					if(abs((int)p.g-(int)q.g)<5) f1[i].g=q.g;
					if(abs((int)p.b-(int)q.b)<5) f1[i].b=q.b;
				}
			}

			t0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);
//			f0=(PIXEL*)GetBuffer(used_frame[f+1]);

			for(y=0; y<h; y++)
			{
				Y=(int)w*y;
				for(x=0; x<w; x++)
				{
					i=x+Y;
					p=f1[i];
					u=t0[i];
					v=t1[i];

					if((int)u.r+(int)u.g+(int)u.b>=(int)v.r+(int)v.g+(int)v.b) continue;//{temp=p; p=q; q=temp;}

					if(v.r!=0)
					{
						t=((int)u.r*(int)p.r)/(int)v.r;
						if(t>255) t=255;
						p.r=(unsigned char)t;
					} else p.r=0;

					if(v.g!=0)
					{
						t=((int)u.g*(int)p.g)/(int)v.g;
						if(t>255) t=255;
						p.g=(unsigned char)t;
					} else p.g=0;

					if(v.b!=0)
					{
						t=((int)u.b*(int)p.b)/(int)v.b;
						if(t>255) t=255;
						p.b=(unsigned char)t;
					} else p.b=0;

					f1[i]=p;
				}
			}
//			UpdateBuffer(used_frame[f+1]);
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
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
		RefreshScreen();
	}
}

void CCombineDoc::OnStackBalance4x4() 
{
	int i,x,y,jx,jy,Y,f;
	PIXEL *f0;
	int j,sum,rsum,gsum,bsum,esum,resum,gesum,besum,sum1,sum2;
	int rmin,rmax,gmin,gmax,bmin,bmax;
	bool skip;

	BeginWaitCursor();

	PIXEL p;
	int c,r[16],g[16],b[16],re[16],ge[16],be[16];

	for(f=0; f<top_used_frame; f++)
	{
		f0=(PIXEL*)GetBuffer(used_frame[f]);

		for(y=4; y<h-4; y+=4)
		{
			Y=(int)w*y;
			for(x=4; x<w-4; x+=4)
			{
				i=x+Y;

				for(jy=0; jy<4; jy++)
				{
					for(jx=0; jx<4; jx++)
					{
						p=f0[i+jx+w*jy];
						r[jx+4*jy]=p.r;
						g[jx+4*jy]=p.g;
						b[jx+4*jy]=p.b;
					}
				}

				for(jx=0; jx<4; jx++)
				{
					p=f0[i+jx-w];
					re[jx]=p.r;
					ge[jx]=p.g;
					be[jx]=p.b;

					p=f0[i+jx+4*w];
					re[jx+4]=p.r;
					ge[jx+4]=p.g;
					be[jx+4]=p.b;
				}

				for(jy=0; jy<4; jy++)
				{
					p=f0[i+jy*w-1];
					re[jy+8]=p.r;
					ge[jy+8]=p.g;
					be[jy+8]=p.b;

					p=f0[i+w*jy+4];
					re[jy+12]=p.r;
					ge[jy+12]=p.g;
					be[jy+12]=p.b;
				}

				rmin=gmin=bmin=0x7fffffff;
				rmax=gmax=bmax=0;
				for(j=0; j<16; j++)
				{
					if(r[j]<rmin) rmin=r[j];
					if(r[j]>rmax) rmax=r[j];
					if(g[j]<gmin) gmin=g[j];
					if(g[j]>gmax) gmax=g[j];
					if(b[j]<bmin) bmin=b[j];
					if(b[j]>bmax) bmax=b[j];
				}
				skip=false;
				if(abs(rmin-rmax)>16) skip=true;
				if(abs(gmin-gmax)>16) skip=true;
				if(abs(bmin-bmax)>16) skip=true;

				if(!skip)
				{
					Balance4x4(r,re);
					Balance4x4(g,ge);
					Balance4x4(b,be);
				}

				rsum=gsum=bsum=0;
				resum=gesum=besum=0;
				for(j=0; j<16; j++)
				{
					rsum+=r[j];
					gsum+=g[j];
					bsum+=b[j];
					resum+=re[j];
					gesum+=ge[j];
					besum+=be[j];
				}

				esum=resum+gesum+besum;
				sum=rsum+gsum+bsum;

				for(j=0; j<16; j++)
				{
					sum1=r[j]+g[j]+b[j];
					if(rsum) r[j]=(r[j]*resum)/rsum;
					if(gsum) g[j]=(g[j]*gesum)/gsum;
					if(bsum) b[j]=(b[j]*besum)/bsum;
					sum2=r[j]+g[j]+b[j];
					if(!skip)
					{
						sum1*=esum/16;
						sum2*=sum/16;
					}
					if(sum2)
					{
						r[j]=(r[j]*sum1)/sum2;
						g[j]=(g[j]*sum1)/sum2;
						b[j]=(b[j]*sum1)/sum2;
					}
				}

				for(j=0; j<4; j++)
				{
					sum1=re[j]+ge[j]+be[j];
					sum2=r[j]+g[j]+b[j];
					if(sum2!=0)
					{
						sum1=(0x100*(sum1+sum2))/(2*sum2);
						if(abs(sum1-sum2)<32)
						{
							r[j]=(r[j]*sum1)/0x100;
							g[j]=(g[j]*sum1)/0x100;
							b[j]=(b[j]*sum1)/0x100;
						}
					}

					sum1=re[j+4]+ge[j+4]+be[j+4];
					sum2=r[j+12]+g[j+12]+b[j+12];
					if(sum2!=0)
					{
						sum1=(0x100*(sum1+sum2))/(2*sum2);
						if(abs(sum1-sum2)<32)
						{
							r[j+12]=(r[j+12]*sum1)/0x100;
							g[j+12]=(g[j+12]*sum1)/0x100;
							b[j+12]=(b[j+12]*sum1)/0x100;
						}
					}

					sum1=re[j+8]+ge[j+8]+be[j+8];
					sum2=r[j*4]+g[j*4]+b[j*4];
					if(sum2!=0)
					{
						sum1=(0x100*(sum1+sum2))/(2*sum2);
						if(abs(sum1-sum2)<32)
						{
							r[j*4]=(r[j*4]*sum1)/0x100;
							g[j*4]=(g[j*4]*sum1)/0x100;
							b[j*4]=(b[j*4]*sum1)/0x100;
						}
					}
					sum1=re[j+12]+ge[j+12]+be[j+12];
					sum2=r[j*4+3]+g[j*4+3]+b[j*4+3];
					if(sum2!=0)
					{
						sum1=(0x100*(sum1+sum2))/(2*sum2);
						if(abs(sum1-sum2)<32)
						{
							r[j*4+3]=(r[j*4+3]*sum1)/0x100;
							g[j*4+3]=(g[j*4+3]*sum1)/0x100;
							b[j*4+3]=(b[j*4+3]*sum1)/0x100;
						}
					}
				}

				for(jy=0; jy<4; jy++)
				{
					for(jx=0; jx<4; jx++)
					{
						c=r[jx+jy*4];
						if(c>255) c=255;
						if(c<0) c=0;
						p.r=(unsigned char)c;

						c=g[jx+jy*4];
						if(c>255) c=255;
						if(c<0) c=0;
						p.g=(unsigned char)c;

						c=b[jx+jy*4];
						if(c>255) c=255;
						if(c<0) c=0;
						p.b=(unsigned char)c;

						f0[i+jx+w*jy]=p;
					}
				}
			}
		}
	}
	RefreshScreen();
	EndWaitCursor();
}

void CCombineDoc::Balance4x4(int *io,int *e)
{
	int i,jx,jy,dif,dif1o3,dif2o3,max,min;
	int rf[4];

	int te=e[0]+e[1]+e[2]+e[3];
	min=e[0]; if(e[1]<min) min=e[1]; if(e[2]<min) min=e[2]; if(e[3]<min) min=e[3]; te-=min;
	max=e[0]; if(e[1]>max) max=e[1]; if(e[2]>max) max=e[2]; if(e[3]>max) max=e[3]; te-=max; te*=2;
	int be=e[4]+e[5]+e[6]+e[7];
	min=e[4]; if(e[5]<min) min=e[5]; if(e[6]<min) min=e[6]; if(e[7]<min) min=e[7]; be-=min;
	max=e[4]; if(e[5]>max) max=e[5]; if(e[6]>max) max=e[6]; if(e[7]>max) max=e[7]; be-=max; be*=2;
	int le=e[8]+e[9]+e[10]+e[11];
	min=e[8]; if(e[9]<min) min=e[9]; if(e[10]<min) min=e[10]; if(e[11]<min) min=e[11]; le-=min;
	max=e[8]; if(e[9]>max) max=e[9]; if(e[10]>max) max=e[10]; if(e[11]>max) max=e[11]; le-=max; le*=2;
	int re=e[12]+e[13]+e[14]+e[15];
	min=e[12]; if(e[13]<min) min=e[13]; if(e[14]<min) min=e[14]; if(e[15]<min) min=e[15]; re-=min;
	max=e[12]; if(e[13]>max) max=e[13]; if(e[14]>max) max=e[14]; if(e[15]>max) max=e[15]; re-=max; re*=2;

	int sumt=io[0]+io[1]+io[2]+io[3];
	min=io[0]; if(io[1]<min) min=io[1]; if(io[2]<min) min=io[2]; if(io[3]<min) min=io[3]; sumt-=min;
	max=io[0]; if(io[1]>max) max=io[1]; if(io[2]>max) max=io[2]; if(io[3]>max) max=io[3]; sumt-=max; sumt*=2;
	int sumb=io[15]+io[14]+io[13]+io[12];
	min=io[15]; if(io[14]<min) min=io[14]; if(io[13]<min) min=io[13]; if(io[12]<min) min=io[12]; sumb-=min;
	max=io[15]; if(io[14]>max) max=io[14]; if(io[13]>max) max=io[13]; if(io[12]>max) max=io[12]; sumb-=max; sumb*=2;

	int summt=io[4]+io[5]+io[6]+io[7];
	min=io[4]; if(io[5]<min) min=io[5]; if(io[6]<min) min=io[6]; if(io[7]<min) min=io[7]; summt-=min;
	max=io[4]; if(io[5]>max) max=io[5]; if(io[6]>max) max=io[6]; if(io[7]>max) max=io[7]; summt-=max; summt*=2;
	int summb=io[11]+io[10]+io[9]+io[8];
	min=io[11]; if(io[10]<min) min=io[10]; if(io[9]<min) min=io[9]; if(io[8]<min) min=io[8]; summb-=min;
	max=io[11]; if(io[10]>max) max=io[10]; if(io[9]>max) max=io[9]; if(io[8]>max) max=io[8]; summb-=max; summb*=2;

	if(
		((sumt>summt) && (summt>summb) && (summb>sumb)) ||
		((sumt<summt) && (summt<summb) && (summb<sumb))
	){
		dif=sumt-sumb;
		if(abs(dif)<24)
		{
			dif1o3=dif/12;
			dif2o3=dif/6;
			dif=dif/4;
			if(abs(te-sumt)<abs(be-sumb))
			{
				rf[0]=0;
				rf[1]=dif1o3;
				rf[2]=dif2o3;
				rf[3]=dif;
			} else {
				rf[3]=0;
				rf[2]=-dif1o3;
				rf[1]=-dif2o3;
				rf[0]=-dif;
			}
		} else {
			rf[3]=0;
			rf[2]=0;
			rf[1]=0;
			rf[0]=0;
		}

		for(jy=0; jy<4; jy++)
		{
			for(jx=0; jx<4; jx++)
			{
				i=jx+jy*4;
				io[i]=io[i]+rf[jy];
				if(io[i]<1) io[i]=1;
			}
		}
	}

	int suml=io[0]+io[4]+io[8]+io[12];
	min=io[0]; if(io[4]<min) min=io[4]; if(io[8]<min) min=io[8]; if(io[12]<min) min=io[12]; suml-=min;
	max=io[0]; if(io[4]>max) max=io[4]; if(io[8]>max) max=io[8]; if(io[12]>max) max=io[12]; suml-=max; suml*=2;
	int sumr=io[15]+io[11]+io[7]+io[3];
	min=io[15]; if(io[11]<min) min=io[11]; if(io[7]<min) min=io[7]; if(io[3]<min) min=io[3]; sumr-=min;
	max=io[15]; if(io[11]>max) max=io[11]; if(io[7]>max) max=io[7]; if(io[3]>max) max=io[3]; sumr-=max; sumr*=2;

	int summl=io[1]+io[5]+io[9]+io[13];
	min=io[1]; if(io[5]<min) min=io[5]; if(io[9]<min) min=io[9]; if(io[12]<min) min=io[13]; summl-=min;
	max=io[1]; if(io[5]>max) max=io[5]; if(io[9]>max) max=io[9]; if(io[12]>max) max=io[13]; summl-=max; summl*=2;
	int summr=io[14]+io[10]+io[6]+io[2];
	min=io[14]; if(io[10]<min) min=io[10]; if(io[6]<min) min=io[6]; if(io[2]<min) min=io[2]; summr-=min;
	max=io[14]; if(io[10]>max) max=io[10]; if(io[6]>max) max=io[6]; if(io[2]>max) max=io[2]; summr-=max; summr*=2;

	if(
		((suml>summl) && (summl>summr) && (summr>sumr)) ||
		((suml<summl) && (summl<summr) && (summr<sumr))
	){
		dif=suml-sumr;
		if(abs(dif)<24)
		{
			dif1o3=dif/12;
			dif2o3=dif/6;
			dif=dif/4;
			if(abs(le-suml)<abs(be-sumr))
			{
				rf[0]=0;
				rf[1]=dif1o3;
				rf[2]=dif2o3;
				rf[3]=dif;
			} else {
				rf[3]=0;
				rf[2]=-dif1o3;
				rf[1]=-dif2o3;
				rf[0]=-dif;
			}
		} else {
			rf[3]=0;
			rf[2]=0;
			rf[1]=0;
			rf[0]=0;
		}


		for(jy=0; jy<4; jy++)
		{
			for(jx=0; jx<4; jx++)
			{
				i=jx+jy*4;
				io[i]=io[i]+rf[jx];
				if(io[i]<1) io[i]=1;
			}
		}
	}
}
