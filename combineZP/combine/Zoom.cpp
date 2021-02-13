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

	CRect oldr;

	oldr=mag;

	int amount;

	amount=(mag.bottom-mag.top)/10; if(amount==0) amount=1;
	if(mag.bottom>mag.top+amount+1) mag.bottom-=amount;
	if(mag.top<mag.bottom-amount-1) mag.top+=amount;

	amount=(mag.right-mag.left)/10; if(amount==0) amount=1;
	if(mag.left<mag.right-amount-1) mag.left+=amount;
	if(mag.right>mag.left+amount+1) mag.right-=amount;

	if(full_mode) Redraw(&oldr,false);
	else Redraw(0,false);
}

void CCombineDoc::zoomout()
{
	if(!top_frame) return;
	if(!magon) return;

	int amount;

	amount=(mag.bottom-mag.top)/10; if(amount==0) amount=1;
	if(mag.bottom<(int)h-amount) mag.bottom+=amount; else mag.bottom=h-1;
	if(mag.top>amount-1) mag.top-=amount; else mag.top=0;

	amount=(mag.right-mag.left)/10; if(amount==0) amount=1;
	if(mag.left>amount-1) mag.left-=amount; else mag.left=0;
	if(mag.right<(int)w-amount) mag.right+=amount; else mag.right=w-1;

	if(full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

void CCombineDoc::xin()
{
	if(!top_frame) return;
	if(!magon) return;

	int amount;

	CRect oldr;

	oldr=mag;

	amount=(mag.right-mag.left)/10; if(amount==0) amount=1;
	if(mag.left<mag.right-amount-1) mag.left+=amount;
	if(mag.right>mag.left+amount+1) mag.right-=amount;

	if(full_mode) Redraw(&oldr,false);
	else Redraw(0,false);
}

void CCombineDoc::xout()
{
	if(!top_frame) return;
	if(!magon) return;

	int amount;

	amount=(mag.right-mag.left)/10; if(amount==0) amount=1;
	if(mag.left>amount-1) mag.left-=amount; else mag.left=0;
	if(mag.right<(int)w-amount) mag.right+=amount; else mag.right=w-1;

	if(full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

void CCombineDoc::yin()
{
	if(!top_frame) return;
	if(!magon) return;

	int amount;

	CRect oldr;

	oldr=mag;

	amount=(mag.bottom-mag.top)/10; if(amount==0) amount=1;
	if(mag.bottom>mag.top+amount+1) mag.bottom-=amount;
	if(mag.top<mag.bottom-amount-1) mag.top+=amount;

	if(full_mode) Redraw(&oldr,false);
	else Redraw(0,false);
}

void CCombineDoc::yout()
{
	if(!top_frame) return;
	if(!magon) return;

	int amount;

	amount=(mag.bottom-mag.top)/10; if(amount==0) amount=1;
	if(mag.bottom<(int)h-amount) mag.bottom+=amount; else mag.bottom=h-1;
	if(mag.top>amount-1) mag.top-=amount; else mag.top=0;

	if(full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}
