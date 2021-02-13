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

// TRANSPARENT

void CCombineDoc::OnSpecialMaketransparent() 
{
	if(!top_frame) return;
	if(fix_active) flash_fixed();
	int i,f,x,y,Y;

	bool flag=false;

	PIXEL *f1;
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	progress.AddString(IDS_STRING623);

	if(macro_mode) flag=true;
	else {
		CString s1,s2;
		s1.LoadString(IDS_ACTIVEONLY); s2.LoadString(IDS_MAKETRANS);
		flag=(MessageBox(0,s1,s2,MB_YESNO)==IDYES);
	}

	if(flag)
	{
		f1=(PIXEL*)GetBuffer(active_frame);
		progress.AddString("   "+buffer[active_frame].name);
		for(y=0; y<h; y++)
		{
			Y=y*w;
			for(x=0; x<w; x++)
			{
				i=x+Y;
				if(depth[i]&fFIXED) continue;
				if((f1[i].abgr&0xffffff)!=0) continue;
				f1[i].a|=aCLEAR;
				if((depth[i]&fFILLED) && ((((depth[i]&dMASK)+(1<<7))>>8)==active_frame))
					depth[i]=fEMPTY;
			}
		}
		UpdateBuffer(active_frame);
	} else {
		for(f=0; f<top_used_frame; f++)
		{
			f1=(PIXEL*)GetBuffer(used_frame[f]);
			progress.AddString("   "+buffer[active_frame].name);
			for(y=0; y<h; y++)
			{
				Y=y*w;
				for(x=0; x<w; x++)
				{
					i=x+Y;
					if(depth[i]&fFIXED) continue;
					if((f1[i].abgr&0xffffff)!=0) continue;
					f1[i].a|=aCLEAR;
					if((depth[i]&fFILLED) && ((((depth[i]&dMASK)+(1<<7))>>8)==f))
						depth[i]=fEMPTY;
				}
			}
			UpdateBuffer(used_frame[f]);
		}
	}

	UpdateBuffer(DEPTH_BUFFER);
	display=active_frame;
	RefreshScreen();
}
