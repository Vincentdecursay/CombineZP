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
#include "parameters.h"
#include "Size1.h"
#include "DecimateDia.h"
#include "SizeDia.h"
#include "RegisterDia.h"
#include "SharpenDia.h"
#include "EdgeDia.h"
#include "Despeckle.h"
#include "PaintDia.h"
#include "Info.h"
#include "rr.h"
#include "FindFolder.h"
#include "BatchControl.h"
#include "Dft.h"
#include "Getvalue.h"

void CCombineDoc::OnDespeckle() 
{
/*	save_state();
	if(despeckleparams()) despeckle();
*/}

void CCombineDoc::despeckle()
{
/*	if(!n) return;
	if((mode&GROUP_MASK)==SPECIAL) return;
	mode=DESPECKLE;
	BeginWaitCursor();

	dodespeckle();
	int temp=times;
	if(times>1)
	{
		times=1;
		dodespeckle();
	}
	times=temp;

 	display=0;
 	EndWaitCursor();
	RefreshScreen();
*/}

#define MAC1	frame=old_srcf[pixel]; \
				if(input[frame][pixel].a==aGOOD) \
					fok[frame]=fok[frame]+fixed[pixel];

void CCombineDoc::dodespeckle()
{
/*	int i,j,f,max_f,score,max_score;
	int x,startx,endx,sx,ex;
	int y,starty,endy,Y;
	int startf,endf;
	int fok[MAX_FRAMES];

	int *fixed,pixel,frame;
	fixed=new int[w*h];
	int savet,savef;

	int *first,*last,first_change,last_change;
	first=new int[h];
	last=new int[h];

	int *tmp,*new_srcf,*old_srcf;
	new_srcf=new int[w*h];
	old_srcf=srcf;

	if(magon)
	{
		startx=mag.left;
		starty=mag.top;
		endx=mag.left+mag.Width()+1;
		endy=mag.top+mag.Height()+1;
	} else {
		startx=0;
		starty=0;
		endx=w;
		endy=h;
	}

	if(startx==0) startx=1;
	if(endx==w) endx=w-1;
	if(starty==0) starty=1;
	if(endy==h) endy=h-1;

	if(plane_mode)
	{
		startf=endf=active_frame;
	} else {
		startf=1;
		endf=n;
	}
	while(hide[startf] && startf<n+1) startf++;
	if(startf==n+1) goto finish;
	while(hide[endf] && endf>0) endf--;
	if(endf==0) goto finish;

	for(y=0; y<h; y++)
	{
		Y=y*w;
		for(x=0; x<w; x++)
		{
			i=x+Y;
			new_srcf[i]=old_srcf[i];
			if(new_srcf[i]>n) new_srcf[i]=old_srcf[i]=0;
		}
	}

	for(y=starty-1; y<endy+1; y++)
	{
		Y=y*w;

		first[y]=startx;
		score=0;
		while(!score && first[y]<last[y])
		{
			for(f=startf; f<=endf; f++)
			{
				if(hide[f]) continue;
				if(input[f][first[y]].a==aGOOD)
				{
					score++;
					break;
				}
			}
			if(score==0) first[y]++;
		}

		last[y]=endx;
		while(!score && first[y]<last[y])
		{
			for(f=startf; f<=endf; f++)
			{
				if(hide[f]) continue;
				if(input[f][last[y]].a==aGOOD)
				{
					score++;
					break;
				}
			}
			if(score==0) last[y]--;
		}

		for(x=startx-1; x<endx+1; x++)
		{
			i=x+Y;
			fixed[i]=(int)(((srct[i]&FIX)==0)/* && (input[srcf[i]][i].a==aGOOD))*//*);
		}
	}

	for(f=0; f<=n; f++) fok[f]=0;

	for(j=0; j<times; j++)
	{
		changed=0;
		for(y=starty; y<endy; y++)
		{
			Y=y*w;

			sx=endx-1;
			if(sx>first[y-1] && first[y-1]) sx=first[y-1];
			if(sx>first[y] && first[y]) sx=first[y];
			if(sx>first[y+1] && first[y+1]) sx=first[y+1];
			if(sx<startx) sx=startx;

			ex=startx;
			if(ex<last[y-1] && last[y-1]) ex=last[y-1];
			if(ex<last[y] && last[y]) ex=last[y];
			if(ex<last[y+1] && last[y+1]) ex=last[y+1];
			if(ex>endx) ex=endx;

			first_change=0; last_change=0;
			for(x=sx; x<ex; x++)
			{
				i=x+Y;
				if(!fixed[i]) continue;
				if(hide[old_srcf[i]]) continue;

				pixel=i-w-1; MAC1
				pixel++; MAC1
				pixel++; MAC1
				pixel=i-1; MAC1
				pixel++; MAC1
				pixel++; MAC1
				pixel=i+w-1; MAC1
				pixel++; MAC1
				pixel++; MAC1

				fok[0]=0;

				max_score=0;
				max_f=0;
				for(f=endf; f>=startf; f--)
				{
					if(fok[f]>4)
					{
						max_score=fok[f];
						for(int g=f; g>=(int)startf; g--) fok[g]=0;
						max_f=f;
						break;
					}

					if(fok[f]==0) continue;

					score=fok[f];
					fok[f]=0;

					if(score>max_score)
					{
						max_score=score;
						max_f=f;
					}
				}

				savet=srct[i];
				savef=old_srcf[i];

				if(max_score==9 || max_score==0) continue;

				if(max_score<despeckle_delta || max_f==0 || input[max_f][i].a!=aGOOD)
				{
					if(old_srcf[i]==0)
					{
						srct[i]=0;
						new_srcf[i]=0;
						continue;
					}
					changed++;
					if(first_change==0) first_change=x;
					last_change=x;

					if(!plane_mode)
					{
						srct[i]=0;
						new_srcf[i]=0;
					} else {
						if(old_srcf[i]==active_frame)
						{
							srct[i]=0;
							new_srcf[i]=0;
						}
					}
					continue;
				}

				new_srcf[i]=max_f;
				if(savef==max_f
					&& savet!=0) srct[i]=savet;
				else
				{
					srct[i]=DESPECKLE;
					changed++;
					if(first_change==0) first_change=x;
					last_change=x;
				}
			}
			if(first_change)
			{
				first[y]=first_change-1;
				if(first[y]<startx) first[y]++;
				last[y]=last_change+1;
				if(last[y]>endx) last[y]=endx;
			} else {
				first[y]=0;
				last[y]=0;
			}
		}
		tmp=new_srcf; new_srcf=old_srcf; old_srcf=tmp;
		if(changed<2) break;
	}

	srcf=old_srcf;

	for(y=starty; y<endy; y++)
	{
		Y=y*w;
	 	for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(srct[i]&FIX!=0) continue;
			if(srcf[i]==0 || srct[i]==0)
			{
				input[0][i].abgr=wipeout;
				srcf[i]=srct[i]=0;
				continue;
			}
			if(!plane_mode)
			{
				if(deapth_flag) input[0][i]=rainbow[srcf[i]];
				else input[0][i]=input[srcf[i]][i];
			} else {
				if(active_frame==srcf[i])
				{
					if(deapth_flag) input[0][i]=rainbow[srcf[i]];
					else input[0][i]=input[srcf[i]][i];
				}
			}
		}
	}
finish:
	delete new_srcf;
	delete first;
	delete last;
	delete fixed;
*/}

bool CCombineDoc::despeckleparams()
{
/*	CDespeckle dd;
	dd.m_delta=despeckle_delta;
	dd.m_times=times;
	int button;
	do{
		button=dd.DoModal();
		if(button==IDCANCEL) return false;
	} while((dd.m_delta<0) || (dd.m_delta>8) || dd.m_times<1);
	despeckle_delta=dd.m_delta;
	times=dd.m_times;
*/	return true;
}
