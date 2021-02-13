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

// HIDE - UNHIDE

// used frames run from 0 to top used frame
// last_used frame defines range for replacing groups with average

void CCombineDoc::refresh_used_list()
{
	int i,j;

	for(i=j=0; i<top_frame; i++)
	{
		if(buffer[i].hidden==true) continue;
		used_frame[j]=i;
		j++;
	}

	top_used_frame=last_used_frame=j;
}

void CCombineDoc::OnFrameUse() 
{
	buffer[active_frame].hidden=false;

	refresh_used_list();

	RefreshScreen();
}

void CCombineDoc::OnFrameDontuse() 
{
	buffer[active_frame].hidden=true;

	refresh_used_list();

	RefreshScreen();
}

void CCombineDoc::OnMakefirst() 
{
	if(active_frame==0) return;

	int i;

	for(i=0; i<active_frame; i++) buffer[i].hidden=true;

	refresh_used_list();

	RefreshScreen();
}

void CCombineDoc::OnMakelast() 
{
	if(active_frame==top_frame-1) return;

	int i;

	for(i=active_frame+1; i<top_frame; i++) buffer[i].hidden=true;

	refresh_used_list();

	RefreshScreen();
}

void CCombineDoc::OnAllframes() 
{
	int i;

	for(i=0; i<top_frame; i++)
	{
		buffer[i].hidden=false;
		used_frame[i]=i;
	}
	top_used_frame=last_used_frame=i;

	RefreshScreen();
}
