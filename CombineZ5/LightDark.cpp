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

void CCombineDoc::OnStackDetailLightanddark() 
{
//	static int called_already=false;
//	if(called_already){called_already=false; return;} else called_already=true;

	mode=LIGHT;
	if(!GetValue())
	{
		mode=0;
//		called_already=false;
		return;
	}

	mode=DARK;
	if(!GetValue())
	{
		mode=0;
//		called_already=false;
		return;
	}

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	save_state();
	refreshscreen=true;
 	AfxBeginThread(lightdark,this);
}

UINT CCombineDoc::lightdark(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->lightdark();
	return 0;
}

void CCombineDoc::lightdark()
{
	backup_picture_valid=false;
	buissy=true;
	progress.AddString(IDS_LIGHTDARK,light_threshold,dark_threshold);
	progress.m_cancel=false;

	int i,x,y,Y,f;

	PIXEL p4;

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
	}

	progress.SetRange(0,top_used_frame-1);

	for(f=0; f<top_used_frame; f++)
	{
		progress.SetPos(f);
		if(progress.m_cancel)
			break;

		in=(PIXEL*)GetBuffer(used_frame[f]);

		for(y=0; y<h; y++)
		{
			Y=y*w;
			for(x=0; x<w; x++)
			{
				i=x+Y;

				if(depth[i]&fFIXED) continue;

				p4=in[i];

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
			}
		}
		FreeBuffer(used_frame[f]);
	}

	for(y=0; y<h; y++)
	{
		Y=y*w;
		for(x=0; x<w; x++)
		{
			i=x+Y;

			if(depth[i]&fFIXED) continue;

			max=(top_used_frame*(int)maxdev[2*i+Mv]-sigma[i])/top_used_frame;
			min=(sigma[i]-top_used_frame*(int)mindev[2*i+Mv])/top_used_frame;
//			if(max>min)
//			{
				if(max>light_threshold)
				{
					depth[i]=fFILLED|(maxdev[2*i+Mf]<<8);
					continue;
				}
//			} else {
				if(min>dark_threshold)
				{
					depth[i]=fFILLED|(mindev[2*i+Mf]<<8);
				}
//			}
		}
	}

	UpdateBuffer(DEPTH_BUFFER);
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
		progress.Minimize();
		buissy=false;
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
		mode=0;
		RefreshScreen();
	}
}
