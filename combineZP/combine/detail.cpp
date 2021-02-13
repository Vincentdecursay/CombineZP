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

struct detail1_Params
{
	int index,stepp;
	int w,h;
	bool bandw,monof;
	PIXEL *in;
	int *depth,*score;
	int detail_size;
	short *maxdev,*mindev;
	int Mv,Mf;
	int *used_frame,f;
//	int step;
	int noise_level;
	bool rf,gf,bf;
} detail1_params[32];

UINT CCombineDoc::detail1(LPVOID param)
{
	detail1_Params *pp=(detail1_Params *)param;
	int index=pp->index;
	int stepp=pp->stepp;
	int w=pp->w;
	int h=pp->h;
	PIXEL *in=pp->in;
	int *depth=pp->depth;
	int *score=pp->score;
	short *maxdev=pp->maxdev;
	short *mindev=pp->mindev;
	int Mv=pp->Mv;
	int Mf=pp->Mf;
	int *used_frame=pp->used_frame;
	int f=pp->f;
	int noise_level=pp->noise_level;
	bool bandw=pp->bandw;
	bool monof=pp->monof;
	bool rf=pp->rf;
	bool gf=pp->gf;
	bool bf=pp->bf;
	int detail_size=pp->detail_size;
	int d1=detail_size; if(d1<1) d1=1;
	int d0=d1;
	int dc2=d1/2;
	int dr2=w*dc2;
	int dc20=d0/2;
	int dr20=w*dc20;

	PIXEL *j,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24;
	int r0,r1,r2,r3,r4,r5,r6,r7,r8,r09,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24;
	int g0,g1,g2,g3,g4,g5,g6,g7,g8,g09,g10,g11,g12,g13,g14,g15,g16,g17,g18,g19,g20,g21,g22,g23,g24;
	int b0,b1,b2,b3,b4,b5,b6,b7,b8,b09,b10,b11,b12,b13,b14,b15,b16,b17,b18,b19,b20,b21,b22,b23,b24;
	int h0,h1,h2,h3,h4,v0,v1,v2,v3,v4;
	int i,x,y,Y;
	int s,t,s1,s2;
	int r,g,b;
	int sum,step;

	for(y=2*d1+index; y<h-2*d1; y+=stepp)
	{
		Y=y*w;
		for(x=2*d1; x<w-2*d1; x++)
		{
			i=x+Y;
			if(depth[i]&fFIXED) continue;

			j=&in[i-(d0+d1)-(d0+d1)*w];		p9=*(j+dr2+dc2); j+=d1;  p14=*j; j+=d0; p16=*j; j+=d0; p18=*j; j+=d1; p20=*(j+dr2-dc2);
			j=&in[i-(d0+d1)-d0*w];			p10=*j; j+=d1; p0=*(j+dr20+dc20); j+=d0;  p3=*j; j+=d0;  p6=*(j+dr20-dc20); j+=d1;  p21=*j;
			j=&in[i-(d0+d1)];				p11=*j; j+=d1; p1=*j; j+=d0;  p4=*j; j+=d0;  p7=*j; j+=d1;  p22=*j;
			j=&in[i-(d0+d1)+d0*w];			p12=*j; j+=d1; p2=*(j-dr20+dc20); j+=d0;  p5=*j; j+=d0;  p8=*(j-dr20-dc20); j+=d1;  p23=*j;
			j=&in[i-(d0+d1)+(d0+d1)*w];		p13=*(j-dr2+dc2); j+=d1; p15=*j; j+=d0; p17=*j; j+=d0; p19=*j; j+=d1; p24=*(j-dr2-dc2);

			if(
				(p0.a|p1.a|p2.a|p3.a|p4.a|p5.a|p6.a|p7.a|p8.a|p9.a|
				p10.a|p11.a|p12.a|p13.a|p14.a|p15.a|p16.a|p17.a|p18.a|p19.a|
				p20.a|p21.a|p22.a|p23.a|p24.a)
				!=aGOOD)
				continue;

			if(bandw || monof)
			{
				if(bandw)
				{
					r0=3*(int)p0.r;
					r1=3*(int)p1.r;
					r2=3*(int)p2.r;
					r3=3*(int)p3.r;
					r4=3*(int)p4.r;
					r5=3*(int)p5.r;
					r6=3*(int)p6.r;
					r7=3*(int)p7.r;
					r8=3*(int)p8.r;
					r09=3*(int)p9.r;
					r10=3*(int)p10.r;
					r11=3*(int)p11.r;
					r12=3*(int)p12.r;
					r13=3*(int)p13.r;
					r14=3*(int)p14.r;
					r15=3*(int)p15.r;
					r16=3*(int)p16.r;
					r17=3*(int)p17.r;
					r18=3*(int)p18.r;
					r19=3*(int)p19.r;
					r20=3*(int)p20.r;
					r21=3*(int)p21.r;
					r22=3*(int)p22.r;
					r23=3*(int)p23.r;
					r24=3*(int)p24.r;
				} else {
					r0=(int)p0.r+(int)p0.g+(int)p0.b;
					r1=(int)p1.r+(int)p1.g+(int)p1.b;
					r2=(int)p2.r+(int)p2.g+(int)p2.b;
					r3=(int)p3.r+(int)p3.g+(int)p3.b;
					r4=(int)p4.r+(int)p4.g+(int)p4.b;
					r5=(int)p5.r+(int)p5.g+(int)p5.b;
					r6=(int)p6.r+(int)p6.g+(int)p6.b;
					r7=(int)p7.r+(int)p7.g+(int)p7.b;
					r8=(int)p8.r+(int)p8.g+(int)p8.b;
					r09=(int)p9.r+(int)p9.g+(int)p9.b;
					r10=(int)p10.r+(int)p10.g+(int)p10.b;
					r11=(int)p11.r+(int)p11.g+(int)p11.b;
					r12=(int)p12.r+(int)p12.g+(int)p12.b;
					r13=(int)p13.r+(int)p13.g+(int)p13.b;
					r14=(int)p14.r+(int)p11.g+(int)p14.b;
					r15=(int)p15.r+(int)p15.g+(int)p15.b;
					r16=(int)p16.r+(int)p16.g+(int)p16.b;
					r17=(int)p17.r+(int)p17.g+(int)p17.b;
					r18=(int)p18.r+(int)p18.g+(int)p18.b;
					r19=(int)p19.r+(int)p19.g+(int)p19.b;
					r20=(int)p20.r+(int)p20.g+(int)p20.b;
					r21=(int)p21.r+(int)p21.g+(int)p21.b;
					r22=(int)p22.r+(int)p22.g+(int)p22.b;
					r23=(int)p23.r+(int)p23.g+(int)p23.b;
					r24=(int)p24.r+(int)p24.g+(int)p24.b;
				}

				sum =3*abs(16*(r0+r1+r2+r3+r4+r5+r6+r7+r8)-9*(r09+r10+r11+r12+r13+r14+r15+r16+r17+r18+r19+r20+r21+r22+r23+r24));
				if(sum<500) continue;

				sum=r4;
				if(sum>maxdev[2*i+Mv])
				{
					maxdev[2*i+Mv]=(short)sum;
					maxdev[2*i+Mf]=(short)used_frame[f];
				}
				if(sum<mindev[2*i+Mv])
				{
					mindev[2*i+Mv]=(short)sum;
					mindev[2*i+Mf]=(short)used_frame[f];
				}

				sum=r0+r1+r2+r3+r4+r5+r6+r7+r8+r09+r10+r11+r12+r13+r14+r15+r16+r17+r18+r19+r20+r21+r22+r23+r24;
				h0=r09+r14+r16+r18+r20;
				h1=r10 +r0 +r3 +r6+r21;
				h2=r11 +r1 +r4 +r7+r22;
				h3=r12 +r2 +r5 +r8+r23;
				h4=sum-h0-h1-h2-h3;
				v0=r09+r10+r11+r12+r13;
				v1=r14 +r0 +r1 +r2+r15;
				v2=r16 +r3 +r4 +r5+r17;
				v3=r18 +r6 +r7 +r8+r19;
				v4=sum-v0-v1-v2-v3;

				if(((h1-h2)*(h2-h3))>0)
				{
					step=(h1-h3)/10;
					r10-=step; r0-=step; r3-=step; r6-=step; r21-=step;
					r12+=step; r2+=step; r5+=step; r8+=step; r23+=step; 
				}

				if(((h0-h1)*(h1-h2))>0)
				{
					step=(h0-h1)/5;
					r09-=step; r14-=step; r16-=step; r18-=step; r20-=step;
				}

				if(((h2-h3)*(h3-h4))>0)
				{
					step=(h3-h4)/5;
					r13+=step; r15+=step; r17+=step; r19+=step; r24+=step;
				}

				if(((v1-v2)*(v2-v3))>0)
				{
					step=(v1-v3)/10;
					r14-=step; r0-=step; r1-=step; r2-=step; r15-=step;
					r18+=step; r6+=step; r7+=step; r8+=step; r19+=step; 
				}

				if(((v0-v1)*(v1-v2))>0)
				{
					step=(v0-v1)/5;
					r09-=step; r10-=step; r11-=step; r12-=step; r13-=step;
				}

				if(((v2-v3)*(v3-v4))>0)
				{
					step=(v3-v4)/5;
					r20+=step; r21+=step; r22+=step; r23+=step; r24+=step;
				}

				t=sum/25;
				s=abs(t-r0)+abs(t-r1)+abs(t-r2)+abs(t-r3)+abs(t-r4)+abs(t-r5)+abs(t-r6)+abs(t-r7)+abs(t-r8);
				s/=2;
				if(s<noise_level*3)
				{
					s1=r0+2*r1+r2+2*r3+4*r4+2*r5+r6+2*r7+r8;
					s2=sum-r0-r1-r2-r3-r4-r5-r6-r7-r8;
					s=(5*abs(s1-s2))/8;
				}
				if(s>noise_level) r=s-noise_level;
				else r=0;

				g=b=r;
			} else {
				r0=(int)p0.r; g0=(int)p0.g; b0=(int)p0.b;
				r1=(int)p1.r; g1=(int)p1.g; b1=(int)p1.b;
				r2=(int)p2.r; g2=(int)p2.g; b2=(int)p2.b;
				r3=(int)p3.r; g3=(int)p3.g; b3=(int)p3.b;
				r4=(int)p4.r; g4=(int)p4.g; b4=(int)p4.b;
				r5=(int)p5.r; g5=(int)p5.g; b5=(int)p5.b;
				r6=(int)p6.r; g6=(int)p6.g; b6=(int)p6.b;
				r7=(int)p7.r; g7=(int)p7.g; b7=(int)p7.b;
				r8=(int)p8.r; g8=(int)p8.g; b8=(int)p8.b;

				r09=(int)p9.r;  g09=(int)p9.g;  b09=(int)p9.b;
				r10=(int)p10.r; g10=(int)p10.g; b10=(int)p10.b;
				r11=(int)p11.r; g11=(int)p11.g; b11=(int)p11.b;
				r12=(int)p12.r; g12=(int)p12.g; b12=(int)p12.b;
				r13=(int)p13.r; g13=(int)p13.g; b13=(int)p13.b;
				r14=(int)p14.r; g14=(int)p11.g; b14=(int)p14.b;
				r15=(int)p15.r; g15=(int)p15.g; b15=(int)p15.b;
				r16=(int)p16.r; g16=(int)p16.g; b16=(int)p16.b;
				r17=(int)p17.r; g17=(int)p17.g; b17=(int)p17.b;
				r18=(int)p18.r; g18=(int)p18.g; b18=(int)p18.b;
				r19=(int)p19.r; g19=(int)p19.g; b19=(int)p19.b;

				r20=(int)p20.r; g20=(int)p20.g; b20=(int)p20.b;
				r21=(int)p21.r; g21=(int)p21.g; b21=(int)p21.b;
				r22=(int)p22.r; g22=(int)p22.g; b22=(int)p22.b;
				r23=(int)p23.r; g23=(int)p23.g; b23=(int)p23.b;
				r24=(int)p24.r; g24=(int)p24.g; b24=(int)p24.b;

				sum =abs(16*(r0+r1+r2+r3+r4+r5+r6+r7+r8)-9*(r09+r10+r11+r12+r13+r14+r15+r16+r17+r18+r19+r20+r21+r22+r23+r24));
				sum+=abs(16*(g0+g1+g2+g3+g4+g5+g6+g7+g8)-9*(g09+g10+g11+g12+g13+g14+g15+g16+g17+g18+g19+g20+g21+g22+g23+g24));
				sum+=abs(16*(b0+b1+b2+b3+b4+b5+b6+b7+b8)-9*(b09+b10+b11+b12+b13+b14+b15+b16+b17+b18+b19+b20+b21+b22+b23+b24));
				if(sum<500) continue;

				sum=r4+g4+b4;
				if(sum>maxdev[2*i+Mv])
				{
					maxdev[2*i+Mv]=(short)sum;
					maxdev[2*i+Mf]=(short)used_frame[f];
				}
				if(sum<mindev[2*i+Mv])
				{
					mindev[2*i+Mv]=(short)sum;
					mindev[2*i+Mf]=(short)used_frame[f];
				}

				if(rf)
				{
					sum=r0+r1+r2+r3+r4+r5+r6+r7+r8+r09+r10+r11+r12+r13+r14+r15+r16+r17+r18+r19+r20+r21+r22+r23+r24;
					h0=r09+r14+r16+r18+r20;
					h1=r10 +r0 +r3 +r6+r21;
					h2=r11 +r1 +r4 +r7+r22;
					h3=r12 +r2 +r5 +r8+r23;
					h4=sum-h0-h1-h2-h3;
					v0=r09+r10+r11+r12+r13;
					v1=r14 +r0 +r1 +r2+r15;
					v2=r16 +r3 +r4 +r5+r17;
					v3=r18 +r6 +r7 +r8+r19;
					v4=sum-v0-v1-v2-v3;

					if(((h1-h2)*(h2-h3))>0)
					{
						step=(h1-h3)/20;
						r10-=step; r0-=step; r3-=step; r6-=step; r21-=step;
						r12+=step; r2+=step; r5+=step; r8+=step; r23+=step; 
					}

					if(((h0-h1)*(h1-h2))>0)
					{
						step=(h0-h1)/10;
						r09-=step; r14-=step; r16-=step; r18-=step; r20-=step;
					}

					if(((h2-h3)*(h3-h4))>0)
					{
						step=(h3-h4)/10;
						r13+=step; r15+=step; r17+=step; r19+=step; r24+=step;
					}

					if(((v1-v2)*(v2-v3))>0)
					{
						step=(v1-v3)/20;
						r14-=step; r0-=step; r1-=step; r2-=step; r15-=step;
						r18+=step; r6+=step; r7+=step; r8+=step; r19+=step; 
					}

					if(((v0-v1)*(v1-v2))>0)
					{
						step=(v0-v1)/10;
						r09-=step; r10-=step; r11-=step; r12-=step; r13-=step;
					}

					if(((v2-v3)*(v3-v4))>0)
					{
						step=(v3-v4)/10;
						r20+=step; r21+=step; r22+=step; r23+=step; r24+=step;
					}

					t=sum/25;
					s=abs(t-r0)+abs(t-r1)+abs(t-r2)+abs(t-r3)+abs(t-r4)+abs(t-r5)+abs(t-r6)+abs(t-r7)+abs(t-r8);
					s/=2;
					if(s<noise_level*3)
					{
						s1=r0+2*r1+r2+2*r3+4*r4+2*r5+r6+2*r7+r8;
						s2=sum-r0-r1-r2-r3-r4-r5-r6-r7-r8;
						s=(5*abs(s1-s2))/8;
					}
					if(s>noise_level) r=s-noise_level;
					else r=0;
				} else r=0;

				if(gf)
				{
					sum=g0+g1+g2+g3+g4+g5+g6+g7+g8+g09+g10+g11+g12+g13+g14+g15+g16+g17+g18+g19+g20+g21+g22+g23+g24;
					h0=g09+g14+g16+g18+g20;
					h1=g10 +g0 +g3 +g6+g21;
					h2=g11 +g1 +g4 +g7+g22;
					h3=g12 +g2 +g5 +g8+g23;
					h4=sum-h0-h1-h2-h3;
					v0=g09+g10+g11+g12+g13;
					v1=g14 +g0 +g1 +g2+g15;
					v2=g16 +g3 +g4 +g5+g17;
					v3=g18 +g6 +g7 +g8+g19;
					v4=sum-v0-v1-v2-v3;

					if(((h1-h2)*(h2-h3))>0)
					{
						step=(h1-h3)/20;
						g10-=step; g0-=step; g3-=step; g6-=step; g21-=step;
						g12+=step; g2+=step; g5+=step; g8+=step; g23+=step;
					}

					if(((h0-h1)*(h1-h2))>0)
					{
						step=(h0-h1)/10;
						g09-=step; g14-=step; g16-=step; g18-=step; g20-=step;
					}

					if(((h2-h3)*(h3-h4))>0)
					{
						step=(h3-h4)/10;
						g13+=step; g15+=step; g17+=step; g19+=step; g24+=step;
					}

					if(((v1-v2)*(v2-v3))>0)
					{
						step=(v1-v3)/20;
						g14-=step; g0-=step; g1-=step; g2-=step; g15-=step;
						g18+=step; g6+=step; g7+=step; g8+=step; g19+=step; 
					}

					if(((v0-v1)*(v1-v2))>0)
					{
						step=(v0-v1)/10;
						g09-=step; g10-=step; g11-=step; g12-=step; g13-=step;
					}

					if(((v2-v3)*(v3-v4))>0)
					{
						step=(v3-v4)/10;
						g20+=step; g21+=step; g22+=step; g23+=step; g24+=step;
					}

					t=sum/25;
					s=abs(t-g0)+abs(t-g1)+abs(t-g2)+abs(t-g3)+abs(t-g4)+abs(t-g5)+abs(t-g6)+abs(t-g7)+abs(t-g8);
					s/=2;
					if(s<noise_level*3)
					{
						s1=g0+2*g1+g2+2*g3+4*g4+2*g5+g6+2*g7+g8;
						s2=sum-g0-g1-g2-g3-g4-g5-g6-g7-g8;
						s=(5*abs(s1-s2))/8;
					}
					if(s>noise_level) g=s-noise_level;
					else g=0;
				} else g=0;

				if(bf)
				{
					sum=b0+b1+b2+b3+b4+b5+b6+b7+b8+b09+b10+b11+b12+b13+b14+b15+b16+b17+b18+b19+b20+b21+b22+b23+b24;
					h0=b09+b14+b16+b18+b20;
					h1=b10 +b0 +b3 +b6+b21;
					h2=b11 +b1 +b4 +b7+b22;
					h3=b12 +b2 +b5 +b8+b23;
					h4=sum-h0-h1-h2-h3;
					v0=b09+b10+b11+b12+b13;
					v1=b14 +b0 +b1 +b2+b15;
					v2=b16 +b3 +b4 +b5+b17;
					v3=b18 +b6 +b7 +b8+b19;
					v4=sum-v0-v1-v2-v3;

					if(((h1-h2)*(h2-h3))>0)
					{
						step=(h1-h3)/20;
						b10-=step; b0-=step; b3-=step; b6-=step; b21-=step;
						b12+=step; b2+=step; b5+=step; b8+=step; b23+=step; 
					}

					if(((h0-h1)*(h1-h2))>0)
					{
						step=(h0-h1)/10;
						b09-=step; b14-=step; b16-=step; b18-=step; b20-=step;
					}

					if(((h2-h3)*(h3-h4))>0)
					{
						step=(h3-h4)/10;
						b13+=step; b15+=step; b17+=step; b19+=step; b24+=step;
					}

					if(((v1-v2)*(v2-v3))>0)
					{
						step=(v1-v3)/20;
						b14-=step; b0-=step; b1-=step; b2-=step; b15-=step;
						b18+=step; b6+=step; b7+=step; b8+=step; b19+=step; 
					}

					if(((v0-v1)*(v1-v2))>0)
					{
						step=(v0-v1)/10;
						b09-=step; b10-=step; b11-=step; b12-=step; b13-=step;
					}

					if(((v2-v3)*(v3-v4))>0)
					{
						step=(v3-v4)/10;
						b20+=step; b21+=step; b22+=step; b23+=step; b24+=step;
					}

					t=sum/25;
					s=abs(t-b0)+abs(t-b1)+abs(t-b2)+abs(t-b3)+abs(t-r4)+abs(t-b5)+abs(t-b6)+abs(t-b7)+abs(t-b8);
					s/=2;
					if(s<noise_level)
					{
						s1=b0+2*b1+b2+2*b3+4*b4+2*b5+b6+2*b7+b8;
						s2=sum-b0-b1-b2-b3-b4-b5-b6-b7-b8;
						s=(5*abs(s1-s2))/8;
					}
					if(s>noise_level) b=s-noise_level;
					else b=0;
				} else b=0;
			}

			if((int)rf+(int)gf+(int)bf>1)
			{
				if(((int)(r==0)+(int)(g==0)+(int)(b==0))>1) continue;
			}

			r=r+g+b;

			if(r>score[i])
			{
				score[i]=r;
				depth[i]=fFILLED|(used_frame[f]<<8);
			}
		}
	}

	return 0;
}

struct detail2_Params
{
	int index,step;
	int w,h;
	PIXEL *in,*temp;
} detail2_params[32];

UINT CCombineDoc::detail2(LPVOID param)
{
	detail2_Params *pp=(detail2_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	PIXEL *in=pp->in;
	PIXEL *temp=pp->temp;

	int r0,g0,b0;
	int i,x,y,Y;

	for(y=1+index; y<h-1; y+=step)
	{
		Y=y*w;
		for(x=1; x<w-1; x++)
		{
			i=x+Y;
			r0=  1*(int)in[i-1-w].r
				+2*(int)in[i-w].r
				+1*(int)in[i+1-w].r
				+2*(int)in[i-1].r
				+4*(int)in[i].r
				+2*(int)in[i+1].r
				+1*(int)in[i-1+w].r
				+2*(int)in[i+w].r
				+1*(int)in[i+1+w].r;
			g0=  1*(int)in[i-1-w].g
				+2*(int)in[i-w].g
				+1*(int)in[i+1-w].g
				+2*(int)in[i-1].g
				+4*(int)in[i].g
				+2*(int)in[i+1].g
				+1*(int)in[i-1+w].g
				+2*(int)in[i+w].g
				+1*(int)in[i+1+w].g;
			b0=  1*(int)in[i-1-w].b
				+2*(int)in[i-w].b
				+1*(int)in[i+1-w].b
				+2*(int)in[i-1].b
				+4*(int)in[i].b
				+2*(int)in[i+1].b
				+1*(int)in[i-1+w].b
				+2*(int)in[i+w].b
				+1*(int)in[i+1+w].b;
			temp[i].r=(unsigned char)(r0/16);
			temp[i].g=(unsigned char)(g0/16);
			temp[i].b=(unsigned char)(b0/16);
		}
	}
	for(y=1; y<h-1; y++)
	{
		Y=y*w;
		for(x=1; x<w-1; x++)
		{
			i=x+Y;
			r0=  1*(int)temp[i-1-w].r
				+2*(int)temp[i-w].r
				+1*(int)temp[i+1-w].r
				+2*(int)temp[i-1].r
				+4*(int)temp[i].r
				+2*(int)temp[i+1].r
				+1*(int)temp[i-1+w].r
				+2*(int)temp[i+w].r
				+1*(int)temp[i+1+w].r;
			g0=  1*(int)temp[i-1-w].g
				+2*(int)temp[i-w].g
				+1*(int)temp[i+1-w].g
				+2*(int)temp[i-1].g
				+4*(int)temp[i].g
				+2*(int)temp[i+1].g
				+1*(int)temp[i-1+w].g
				+2*(int)temp[i+w].g
				+1*(int)temp[i+1+w].g;
			b0=  1*(int)temp[i-1-w].b
				+2*(int)temp[i-w].b
				+1*(int)temp[i+1-w].b
				+2*(int)temp[i-1].b
				+4*(int)temp[i].b
				+2*(int)temp[i+1].b
				+1*(int)temp[i-1+w].b
				+2*(int)temp[i+w].b
				+1*(int)temp[i+1+w].b;
			in[i].r=(unsigned char)(r0/16);
			in[i].g=(unsigned char)(g0/16);
			in[i].b=(unsigned char)(b0/16);
		}
	}

	return 0;
}

void CCombineDoc::detail()
{
	BeginThreads
	SetThreads(w,h)

	backup_picture_valid=false;
	buissy=true;
	progress.AddString(IDS_DETAIL,noise_level,detail_size,detail_filter);

	bool filterf,monof,rf,gf,bf;
	int dml,n100,n10;
	filterf=false;
	monof=true;
	rf=false;
	gf=false;
	bf=false;
	dml=0;
	if(detail_size!=-1)
	{
		dml=detail_filter&7; // dark/middle/light
		n10=(detail_filter/10)&0x7;
		n100=detail_filter/100;
		if(n100&1) rf=true;
		if(n100&2) gf=true;
		if(n100&4) bf=true;
		if(rf|gf|bf) monof=false;
		if(n10&1) filterf=true; 
	}

	int i,x,y,Y,f;
	int maxscore=0;
	int d0=1,d1,dr2,dc2,dr20,dc20;
	d1=detail_size; if(d1<1) d1=1;
	d0=d1;
	dc2=d1/2; dr2=w*dc2;
	dc20=d0/2; dr20=w*dc20;

	int *score=(int*)GetBuffer(TEMP_BUFFER);

	const int Mf=0;
	const int Mv=1;
//	int sum;
	short *maxdev=(short*)GetBuffer(TEMP_BUFFER+1);
	short *mindev=(short*)GetBuffer(TEMP_BUFFER+2);
	PIXEL *in;
	PIXEL *temp;
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	for(i=0; i<w*h; i++)
	{
		maxdev[2*i+Mv]=0;
		maxdev[2*i+Mf]=0;
		
		mindev[2*i+Mv]=999;
		mindev[2*i+Mf]=0;
		
		if(depth[i]&fFIXED) continue;
		depth[i]=fEMPTY;
		score[i]=0;
	}

	progress.SetRange(0,top_used_frame-1);

	for(f=0; f<top_used_frame; f++)
	{
		progress.SetPos(f);
		if(progress.m_cancel)
		{
			break;
		}
		if(filterf)
		{
			in=(PIXEL*)GetBuffer(TEMP_BUFFER+3);
			temp=(PIXEL*)GetBuffer(used_frame[f]);

			for(i=0; i<w*h; i++) in[i]=temp[i];
			FreeBuffer(used_frame[f]);
			temp=(PIXEL*)GetBuffer(TEMP_BUFFER+4);

			for(obj=0; obj<objects; obj++)
			{
				detail2_params[obj].index=obj;
				detail2_params[obj].step=objects;
				detail2_params[obj].w=w;
				detail2_params[obj].h=h;
				detail2_params[obj].in=in;
				detail2_params[obj].temp=temp;
				StartThreads(detail2,detail2_params)
			}
			EndThreads

			FreeBuffer(TEMP_BUFFER+4);
		} else {
			in=(PIXEL*)GetBuffer(used_frame[f]);
		}

		for(obj=0; obj<objects; obj++)
		{
			detail1_params[obj].index=obj;
			detail1_params[obj].stepp=objects;
			detail1_params[obj].w=w;
			detail1_params[obj].h=h;
			detail1_params[obj].bandw=bandw;
			detail1_params[obj].monof=monof;
			detail1_params[obj].in=in;
			detail1_params[obj].score=score;
			detail1_params[obj].depth=depth;
			detail1_params[obj].depth=depth;
			detail1_params[obj].score=score;
			detail1_params[obj].maxdev=maxdev;
			detail1_params[obj].mindev=mindev;
			detail1_params[obj].Mv=Mv;
			detail1_params[obj].Mf=Mf;
			detail1_params[obj].used_frame=used_frame;
			detail1_params[obj].f=f;
			detail1_params[obj].noise_level=noise_level;
			detail1_params[obj].rf=rf;
			detail1_params[obj].gf=gf;
			detail1_params[obj].bf=bf;
			detail1_params[obj].detail_size=detail_size;
			StartThreads(detail1,detail1_params)
		}
		EndThreads

		if(filterf) FreeBuffer(TEMP_BUFFER+3);
		else FreeBuffer(used_frame[f]);
	}

	int HI=maxscore/4;
	int LOW=80;
	switch(dml)
	{
	case 2:
		for(y=2; y<h-2; y++)
		{
			Y=y*w;
			for(x=2; x<w-2; x++)
			{
				i=x+Y;
				if(depth[i]&fEMPTY) continue;
				if(depth[i]&fFIXED) continue;

				f=((depth[i]&dMASK)>>8);
				if(score[i]>HI) continue;
				if(maxdev[2*i+Mv]<LOW) continue;
				if((mindev[2*i+Mf]!=f) && (maxdev[2*i+Mf]!=f)) continue;
				depth[i]=fEMPTY;
			}
		}
		break;
	case 3:
		for(y=2; y<h-2; y++)
		{
			Y=y*w;
			for(x=2; x<w-2; x++)
			{
				i=x+Y;
				if(depth[i]&fEMPTY) continue;
				if(depth[i]&fFIXED) continue;

				f=((depth[i]&dMASK)>>8);
				if(score[i]>HI) continue;
				if(maxdev[2*i+Mv]<LOW) continue;
				if(maxdev[2*i+Mf]!=f) continue;
				depth[i]=fEMPTY;
			}
		}
		break;
	case 6:
		for(y=2; y<h-2; y++)
		{
			Y=y*w;
			for(x=2; x<w-2; x++)
			{
				i=x+Y;
				if(depth[i]&fEMPTY) continue;
				if(depth[i]&fFIXED) continue;

				f=((depth[i]&dMASK)>>8);
				if(score[i]>HI) continue;
				if(maxdev[2*i+Mv]<LOW) continue;
				if(mindev[2*i+Mf]!=f) continue;
				depth[i]=fEMPTY;
			}
		}
		break;
	case 0: case 7:
		break;
	case 1:
		for(y=2; y<h-2; y++)
		{
			Y=y*w;
			for(x=2; x<w-2; x++)
			{
				i=x+Y;
				if(depth[i]&fEMPTY) continue;
				if(depth[i]&fFIXED) continue;

				f=((depth[i]&dMASK)>>8);
				if(score[i]>HI) continue;
				if(maxdev[2*i+Mv]<LOW) continue;
				if(mindev[2*i+Mf]==f) continue;
				depth[i]=fEMPTY;
			}
		}
		break;
	case 4:
		for(y=2; y<h-2; y++)
		{
			Y=y*w;
			for(x=2; x<w-2; x++)
			{
				i=x+Y;
				if(depth[i]&fEMPTY) continue;
				if(depth[i]&fFIXED) continue;

				f=((depth[i]&dMASK)>>8);
				if(score[i]>HI) continue;
				if(maxdev[2*i+Mv]<LOW) continue;
				if(maxdev[2*i+Mf]==f) continue;
				depth[i]=fEMPTY;
			}
		}
		break;
	case 5:
		for(y=2; y<h-2; y++)
		{
			Y=y*w;
			for(x=2; x<w-2; x++)
			{
				i=x+Y;
				if(depth[i]&fEMPTY) continue;
				if(depth[i]&fFIXED) continue;

				f=((depth[i]&dMASK)>>8);
				if(score[i]>HI) continue;
				if(maxdev[2*i+Mv]<LOW) continue;
				if((mindev[2*i+Mf]==f) || (maxdev[2*i+Mf]==f)) continue;
				depth[i]=fEMPTY;
			}
		}
		break;
	}

	FreeBuffer(TEMP_BUFFER);
	FreeBuffer(TEMP_BUFFER+1);
	FreeBuffer(TEMP_BUFFER+2);

	UpdateBuffer(DEPTH_BUFFER);
	progress.SetPos(0);
	if(refreshscreen)
	{
		redraw_picture();
		display=PICTURE_BUFFER;
		refreshscreen=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
		mode=0;
	}
}

void CCombineDoc::checkdepth()
{
	int d,f;
	int t1,t2,t3,t4,tl,th;
	int fr1,fr2,fr3,fr4;
	PIXEL *f1,*f2,*f3,*f4;
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	for(f=top_used_frame; f>2; f--)
	{
		fr1=used_frame[f]; f1=(PIXEL*)GetBuffer(fr1);
		if(f-1>=0){fr2=used_frame[f-1]; f2=(PIXEL*)GetBuffer(fr2);} else{fr2=fr1; f2=f1;}
		if(f-2>=0){fr3=used_frame[f-2]; f3=(PIXEL*)GetBuffer(fr3);} else{fr3=fr2; f3=f2;}
		if(f-3>=0){fr4=used_frame[f-3]; f4=(PIXEL*)GetBuffer(fr4);} else{fr4=fr3; f4=f3;}
		
		for(int i=0; i<(int)(w*h)-1; i++)
		{
			if(depth[i]==fEMPTY) continue;
			d=(depth[i]&dMASK)>>8;

			t1=(int)f1[i].r+(int)f1[i].g+(int)f1[i].b;
			t2=(int)f2[i].r+(int)f2[i].g+(int)f2[i].b;
			t3=(int)f3[i].r+(int)f3[i].g+(int)f3[i].b;
			t4=(int)f4[i].r+(int)f4[i].g+(int)f4[i].b;
			tl=(t1-t2)*(t3-t2); th=(t2-t3)*(t4-t3);

			if(d==fr2)
			{
				if(th>0)
				{
					if(abs(t4+t2-2*t3)-10>abs(t3+t1-2*t2))
						depth[i]=fFILLED|(fr3<<8);
				}
				continue;
			}

			if(d==fr3)
			{
				if(tl>0)
				{
					if(abs(t4+t2-2*t3)<abs(t3+t1-2*t2)-10)
						depth[i]=fFILLED|(fr2<<8);
				}
			}
		}

		if(fr4!=fr3) FreeBuffer(fr4);
		if(fr3!=fr2) FreeBuffer(fr3);
		if(fr2!=fr1) FreeBuffer(fr2);
		FreeBuffer(fr1);
	}
	UpdateBuffer(DEPTH_BUFFER);
	backup_picture_valid=false;
	display=PICTURE_BUFFER;
}
