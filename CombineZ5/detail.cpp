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

void CCombineDoc::OnStackDetail() 
{
	static int called_already=false;
	if(called_already){called_already=false; return;} else called_already=true;

	mode=DETAIL;
	if(!GetValue())
	{
		mode=0;
		called_already=false;
		return;
	}

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	save_state();
	refreshscreen=true;
 	AfxBeginThread(detail,this);
}

UINT CCombineDoc::detail(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->detail();
	return 0;
}

void CCombineDoc::detail()
{
	backup_picture_valid=false;
	buissy=true;
	progress.AddString(IDS_DETAIL,noise_level);
	progress.m_cancel=false;

	int i,x,y,Y,f;
	int s,s1,s2;
	int r,g,b;

	PIXEL *j,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24;

	int *score=(int*)GetBuffer(TEMP_BUFFER);

	const int Mf=0;
	const int Mv=1;
	short sum,max,min;
	short *maxdev=(short*)GetBuffer(TEMP_BUFFER+1);
	short *mindev=(short*)GetBuffer(TEMP_BUFFER+2);
	int *sigma=(int*)GetBuffer(TEMP_BUFFER+3);

	PIXEL *in;
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	for(i=0; i<w*h; i++)
	{
		maxdev[2*i+Mv]=0;
		maxdev[2*i+Mf]=0;
		
		mindev[2*i+Mv]=999;
		mindev[2*i+Mf]=0;
		sigma[i]=0;
		
		if(depth[i]&fFIXED) continue;
		depth[i]=fEMPTY;
		score[i]=0;
	}

	progress.SetRange(0,	top_used_frame-1);

	for(f=0; f<top_used_frame; f++)
	{
		progress.SetPos(f);
		if(progress.m_cancel)
		{
			break;
		}

		in=(PIXEL*)GetBuffer(used_frame[f]);

		for(y=2; y<h-2; y++)
		{
			Y=y*w;
			for(x=2; x<w-2; x++)
			{
				i=x+Y;

				if(depth[i]&fFIXED) goto endloop;

				j=&in[i-2-2*w];		p9=*j++;  p14=*j++; p16=*j++; p18=*j++; p20=*j;
				j=&in[i-2-w];		p10=*j++; p0=*j++;  p3=*j++;  p6=*j++;  p21=*j;
				j=&in[i-2];					p11=*j++; p1=*j++;  p4=*j++;  p7=*j++;  p22=*j;
				j=&in[i-2+w];		p12=*j++; p2=*j++;  p5=*j++;  p8=*j++;  p23=*j;
				j=&in[i-2+2*w];		p13=*j++; p15=*j++; p17=*j++; p19=*j++; p24=*j;

				if(
					(p0.a|p1.a|p2.a|p3.a|p4.a|p5.a|p6.a|p7.a|p8.a|p9.a|
					p10.a|p11.a|p12.a|p13.a|p14.a|p15.a|p16.a|p17.a|p18.a|p19.a|
					p20.a|p21.a|p22.a|p23.a|p24.a)
					!=aGOOD)
					continue;

				sum=((short)(p4.r)+(short)(p4.g)+(short)(p4.b));
				sigma[i]+=(int)sum;
				if(sum>maxdev[2*i+Mv])
				{
					maxdev[2*i+Mv]=sum;
					maxdev[2*i+Mf]=(short)used_frame[f];
				}
				if(sum<mindev[2*i+Mv])
				{
					mindev[2*i+Mv]=sum;
					mindev[2*i+Mf]=(short)used_frame[f];
				}
				
				if(p4.r>p1.r) s=(int)p4.r-(int)p1.r; else s=(int)p1.r-(int)p4.r;
				if(p4.r>p3.r) s+=(int)p4.r-(int)p3.r; else s+=(int)p3.r-(int)p4.r;
				if(p4.r>p5.r) s+=(int)p4.r-(int)p5.r; else s+=(int)p5.r-(int)p4.r;
				if(p4.r>p7.r) s+=(int)p4.r-(int)p7.r; else s+=(int)p7.r-(int)p4.r;
				s*=4;
				if(s<noise_level*4)
				{
					s1=
						(int)p0.r+
						2*(int)p1.r+
						(int)p2.r+
						2*(int)p3.r+
						4*(int)p4.r+
						2*(int)p5.r+
						(int)p6.r+
						2*(int)p7.r+
						(int)p8.r;
					s2=
						(int)p9.r+
						(int)p10.r+
						(int)p11.r+
						(int)p12.r+
						(int)p13.r+
						(int)p14.r+
						(int)p15.r+
						(int)p16.r+
						(int)p17.r+
						(int)p18.r+
						(int)p19.r+
						(int)p20.r+
						(int)p21.r+
						(int)p22.r+
						(int)p23.r+
						(int)p24.r;
					s=abs(s1-s2);
				}
				if(s>noise_level)
				{
					r=s-noise_level;
				}
				else r=0;

				if(p4.g>p1.g) s=(int)p4.g-(int)p1.g; else s=(int)p1.g-(int)p4.g;
				if(p4.g>p3.g) s+=(int)p4.g-(int)p3.g; else s+=(int)p3.g-(int)p4.g;
				if(p4.g>p5.g) s+=(int)p4.g-(int)p5.g; else s+=(int)p5.g-(int)p4.g;
				if(p4.g>p7.g) s+=(int)p4.g-(int)p7.g; else s+=(int)p7.g-(int)p4.g;
				s*=3;
				if(s<noise_level*4)
				{
					s1=
						(int)p0.g+
						2*(int)p1.g+
						(int)p2.g+
						2*(int)p3.g+
						4*(int)p4.g+
						2*(int)p5.g+
						(int)p6.g+
						2*(int)p7.g+
						(int)p8.g;
					s2=
						(int)p9.g+
						(int)p10.g+
						(int)p11.g+
						(int)p12.g+
						(int)p13.g+
						(int)p14.g+
						(int)p15.g+
						(int)p16.g+
						(int)p17.g+
						(int)p18.g+
						(int)p19.g+
						(int)p20.g+
						(int)p21.g+
						(int)p22.g+
						(int)p23.g+
						(int)p24.g;
					s=abs(s1-s2);
				}
				if(s>noise_level)
				{
					g=s-noise_level;
				}
				else g=0;

				if(p4.b>p1.b) s=(int)p4.b-(int)p1.b; else s=(int)p1.b-(int)p4.b;
				if(p4.b>p3.b) s+=(int)p4.b-(int)p3.b; else s+=(int)p3.b-(int)p4.b;
				if(p4.b>p5.b) s+=(int)p4.b-(int)p5.b; else s+=(int)p5.b-(int)p4.b;
				if(p4.b>p7.b) s+=(int)p4.b-(int)p7.b; else s+=(int)p7.b-(int)p4.b;
				s*=4;
				if(s<noise_level*4)
				{
					s1=
						(int)p0.b+
						2*(int)p1.b+
						(int)p2.b+
						2*(int)p3.b+
						4*(int)p4.b+
						2*(int)p5.b+
						(int)p6.b+
						2*(int)p7.b+
						(int)p8.b;
					s2=
						(int)p9.b+
						(int)p10.b+
						(int)p11.b+
						(int)p12.b+
						(int)p13.b+
						(int)p14.b+
						(int)p15.b+
						(int)p16.b+
						(int)p17.b+
						(int)p18.b+
						(int)p19.b+
						(int)p20.b+
						(int)p21.b+
						(int)p22.b+
						(int)p23.b+
						(int)p24.b;
					s=abs(s1-s2);
				}
				if(s>noise_level)
				{
					b=s-noise_level;
				}
				else b=0;

				if((r==0 && g==0) || (r==0 && b==0) || (g==0 && b==0)) goto endloop;

				r=r+g+b;

				if(s<noise_level) goto endloop;

				if(r>score[i])
				{
					score[i]=r;
					depth[i]=fFILLED|(used_frame[f]<<8);
				}
endloop:		;
			}
		}
		FreeBuffer(used_frame[f]);
	}

	for(y=2; y<h-2; y++)
	{
		Y=y*w;
		for(x=2; x<w-2; x++)
		{
			i=x+Y;

			if(depth[i]&fFIXED) continue;
			if(score[i]>noise_level*4) continue;

			max=(top_used_frame*(int)maxdev[2*i+Mv]-sigma[i])/top_used_frame;
			min=(sigma[i]-top_used_frame*(int)mindev[2*i+Mv])/top_used_frame;

			if((maxdev[2*i+Mf]!=((depth[i]&dMASK)>>8)) || (mindev[2*i+Mf]!=((depth[i]&dMASK)>>8)))
				depth[i]=fEMPTY;
		}
	}

	UpdateBuffer(DEPTH_BUFFER);
	FreeBuffer(TEMP_BUFFER);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+2);
	FreeBuffer(TEMP_BUFFER+3);
	progress.SetPos(0);
	if(refreshscreen)
	{
		redraw_picture();
		progress.m_cancel=false;
		display=PICTURE_BUFFER;
		refreshscreen=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
		RefreshScreen();
		mode=0;
	}
}
