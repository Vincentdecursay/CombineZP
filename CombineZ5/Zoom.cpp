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


void CCombineDoc::zoomin()
{
	if(!top_frame) return;
	if(!magon) return;

	CRect r;

	if(mag.bottom>mag.top+2) mag.bottom-=1;
	if(mag.top<mag.bottom-2) mag.top+=1;

	if(mag.left<mag.right-2) mag.left+=1;
	if(mag.right>mag.left+2) mag.right-=1;

	r=mag;
	r.left--;
	r.top--;
	if(full_mode) Redraw(&r,false);
	else Redraw(0,false);
}

void CCombineDoc::zoomout()
{
	if(!top_frame) return;
	if(!magon) return;

	if(mag.bottom<(int)h-1) mag.bottom+=1;
	if(mag.top>0) mag.top-=1;

	if(mag.left>0) mag.left-=1;
	if(mag.right<(int)w-1) mag.right+=1;

	if(full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

void CCombineDoc::xin()
{
	if(!top_frame) return;
	if(!magon) return;

	if(mag.right>mag.left+2) mag.right-=1;

	if(full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

void CCombineDoc::xout()
{
	if(!top_frame) return;
	if(!magon) return;

	if(mag.right<(int)w-1) mag.right+=1;

	if(full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

void CCombineDoc::yin()
{
	if(!top_frame) return;
	if(!magon) return;

	if(mag.bottom>mag.top+2) mag.bottom-=1;

	if(full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

void CCombineDoc::yout()
{
	if(!top_frame) return;
	if(!magon) return;

	if(mag.bottom<(int)h-1) mag.bottom+=1;

	if(full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

