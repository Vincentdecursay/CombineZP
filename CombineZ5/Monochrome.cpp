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

void CCombineDoc::OnFrameMakemonochrome() 
{
	int i,c;

	PIXEL *f1;

	progress.AddString("Make Frame(s) Monochrome");

	if(MessageBox(0,"Active Frame Only?","Make Monochrome",MB_YESNO)==IDYES)
	{
		BeginWaitCursor();
		f1=(PIXEL*)GetBuffer(active_frame);
		progress.AddString("   "+buffer[active_frame].name);
		for(i=0; i<(int)(w*h); i++)
		{
			c=300*(int)f1[i].r;
			c+=600*(int)f1[i].g;
			c+=124*(int)f1[i].b;
			c/=1024;
			if(c>255) c=255;
			if(c<0) c=0;
			f1[i].r=(unsigned char)c;
			f1[i].g=(unsigned char)c;
			f1[i].b=(unsigned char)c;
		}
		UpdateBuffer(active_frame);
		EndWaitCursor();
	} else {
		BeginWaitCursor();
		monochrome();
		EndWaitCursor();
	}

	display=active_frame;
	backup_picture_valid=false;
	redraw_picture();
	RefreshScreen();
}

void CCombineDoc::monochrome()
{
	int i,c,f;

	PIXEL *f1;

	for(f=0; f<last_used_frame; f++)
	{
		f1=(PIXEL*)GetBuffer(used_frame[f]);
		progress.AddString("   "+buffer[used_frame[f]].name);
		for(i=0; i<(int)(w*h); i++)
		{
			c=300*(int)f1[i].r;
			c+=600*(int)f1[i].g;
			c+=124*(int)f1[i].b;
			c/=1024;
			if(c>255) c=255;
			if(c<0) c=0;
			f1[i].r=(unsigned char)c;
			f1[i].g=(unsigned char)c;
			f1[i].b=(unsigned char)c;
		}
		UpdateBuffer(used_frame[f]);
	}
}
