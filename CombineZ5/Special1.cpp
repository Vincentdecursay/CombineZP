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

void CCombineDoc::copyback()
{
	int i;

	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER);
	PIXEL *af=(PIXEL*)GetBuffer(active_frame);

	for(i=0; i<w*h; i++) af[i]=out[i];

	FreeBuffer(OUT_BUFFER);
	UpdateBuffer(active_frame);
}

void CCombineDoc::copy_forward()
{
	int i;

	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER);
	PIXEL *af=(PIXEL*)GetBuffer(active_frame);

	for(i=0; i<w*h; i++) out[i]=af[i];

	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(active_frame);
}

void CCombineDoc::specialreturn()
{
	copyback();
	if(special_active!=active_frame || old_mag!=mag)
	{
		if(MessageBox(0,"Continue Using Tool?","Displayed Frame Changed",MB_YESNO)==IDNO)
		{
			special_changed=false;
			reportspecialvalues();
			leave_special();
			return;
		}
		copy_forward();
		active_frame=special_active;
		special_rework();
		old_mag=mag;
	} else {
		special_changed=false;
		reportspecialvalues();
		leave_special();
		return;
	}
	RefreshScreen();
}

void CCombineDoc::reportspecialvalues()
{
	CString s;
	switch(mode)
	{
	case RED:
	case GREEN:
	case BLUE:
	case BRIGHTNESSX:
		progress.AddString(IDS_RGB,(int)(rfactor*1000.0f),(int)(gfactor*1000.0f),(int)(bfactor*1000.0f));
		break;
	case CONTRASTX:
		progress.AddString(IDS_CONTRAST,contrast_delta);
		break;
	}
}

void CCombineDoc::OnSpecialLeavespecialmode() 
{
	CString s,nm;
	s="Save the changes you made to ";
	s+=buffer[active_frame].name;
	s+="?";

	if(MessageBox(0,(const char *)s,"Save?",MB_YESNO)==IDYES)
	{
		display=OUT_BUFFER;
		OnFrameReplace();
	}

	mode=0;
	display=active_frame;
	RefreshScreen();
}

void CCombineDoc::OnSpecialActivered() 
{
	if(!(mode==BRIGHTNESSX) && !(mode==RED) && !(mode==GREEN) && !(mode==BLUE))
		if(!start_special()) return;

	BeginWaitCursor();
	mode=RED;
	special_active=active_frame;
	old_mag=mag;
	adjustcolour();
	RefreshScreen();

	EndWaitCursor();
}

void CCombineDoc::OnSpecialActivegreen() 
{
	if(!(mode==BRIGHTNESSX) && !(mode==RED) && !(mode==GREEN) && !(mode==BLUE))
		if(!start_special()) return;

	BeginWaitCursor();

	mode=GREEN;
	special_active=active_frame;
	old_mag=mag;
	adjustcolour();
	RefreshScreen();

	EndWaitCursor();
}

void CCombineDoc::OnSpecialActivebule() 
{
	if(!(mode==BRIGHTNESSX) && !(mode==RED) && !(mode==GREEN) && !(mode==BLUE))

	if(!start_special()) return;

	BeginWaitCursor();

	mode=BLUE;
	special_active=active_frame;
	old_mag=mag;
	adjustcolour();
	RefreshScreen();

	EndWaitCursor();
}

void CCombineDoc::OnSpecialBrightness() // BRIGHTNESS ON MENU
{
	if(!(mode==BRIGHTNESSX) && !(mode==RED) && !(mode==GREEN) && !(mode==BLUE))
		if(!start_special()) return;

	BeginWaitCursor();

	mode=BRIGHTNESSX;
	special_active=active_frame;
	old_mag=mag;
	adjustcolour();
	RefreshScreen();

	EndWaitCursor();
}

void CCombineDoc::adjustcolour()
{
	if(!top_frame) return;
	
	unsigned int i;
	unsigned int x,startx,endx;
	unsigned int y,starty,endy,Y;

	double r,g,b;

	special_changed=true;

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

	PIXEL *af=(PIXEL*)GetBuffer(active_frame);
	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER);
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	for(y=starty; y<endy; y++)
	{
		Y=y*w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(depth[i]&fFIXED)
			{
				out[i]=af[i];
				continue;
			}
			if(af[i].a!=aCLEAR)
			{
				r=((double)af[i].r)*rfactor;
				if(r>255.0) r=255.0;

				g=((double)af[i].g)*gfactor;
				if(g>255.0) g=255.0;

				b=((double)af[i].b)*bfactor;
				if(b>255.0) b=255.0;

				out[i].r=(unsigned char)r;
				out[i].g=(unsigned char)g;
				out[i].b=(unsigned char)b;
				out[i].a=aGOOD;
				continue;
			}
			out[i].abgr=0;
			out[i].a=aCLEAR;
		}
	}
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(active_frame);
	FreeBuffer(DEPTH_BUFFER);
	display=OUT_BUFFER;
}

void CCombineDoc::OnSpecialContrast() 
{
	if(!start_special()) return;

	mode=CONTRASTX;
	special_active=active_frame;
	old_mag=mag;
	contrast_delta=10;
	Contrast();
	if(magon && !full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

void CCombineDoc::contrast()
{
	int i;
	int x,startx,endx;
	int y,starty,endy,Y;

	int hist[256];
	for(i=0; i<256; i++) hist[i]=0;

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

	PIXEL *af=(PIXEL*)GetBuffer(active_frame);
	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER);
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	double count=0.0;
	for(y=starty; y<endy; y++)
	{
		Y=y*(int)w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(depth[i]&fFIXED) continue;
			hist[(int)af[i].r]++;
			hist[(int)af[i].g]++;
			hist[(int)af[i].b]++;
			count+=1.0;
		}
	}

	for(i=0; i<255; i++)
	{
		if(hist[i]>(w*h)/1000+10) break;
	}
	double min=i;

	for(i=255; i>(w*h)/1000+10; i--)
	{
		if(hist[i]>0) break;
	}
	double max=i;

	min+=contrast_delta/2.0;
	max-=(contrast_delta+1)/2.0;
	if(min>contrast_delta) min=contrast_delta;

	double factor;
	if(max-min!=0) factor=255.0/(max-min);
	else factor=255.0;

	int r,g,b;
	for(y=starty; y<endy; y++)
	{
		Y=y*(int)w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			if(depth[i]&fSPECIAL_FIXED)
			{
				out[i]=af[i];
				continue;
			}

			r=(int)(((double)(int)af[i].r-min)*factor);
			g=(int)(((double)(int)af[i].g-min)*factor);
			b=(int)(((double)(int)af[i].b-min)*factor);

			if(r>255) r=255;
			if(r<0) r=0;

			if(g>255) g=255;
			if(g<0) g=0;

			if(b>255) b=255;
			if(b<0) b=0;

			out[i].r=(unsigned char)r;
			out[i].g=(unsigned char)g;
			out[i].b=(unsigned char)b;
		}
	}
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(DEPTH_BUFFER);
	FreeBuffer(active_frame);
	display=OUT_BUFFER;
}

void CCombineDoc::Contrast() // CONTRAST ON MENU
{
	special_changed=true;

	BeginWaitCursor();

	contrast();

	EndWaitCursor();
}

bool CCombineDoc::start_special()
{
	int i;
	if(!top_frame) return false;
	if(fix_active) flash_fixed();

	if((display==PICTURE_BUFFER) || (display==OUT_BUFFER))
	{
		if(MessageBox(0,"Create a new Frame from this Picture","NEW FRAME",MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL)
		return false;
		OnFrameNewfrompicture();
	}

	PIXEL *af=(PIXEL*)GetBuffer(active_frame);
	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER);
	for(i=0; i<(int)w*(int)h; i++) out[i]=af[i];

	special_changed=false;
	OnSpecialReset();
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(active_frame);
	return true;
}

void CCombineDoc::special_rework()
{
	switch(mode)
	{
	case RED:
	case GREEN:
	case BLUE:
	case BRIGHTNESSX:
		adjustcolour();
		break;
	case CORRECT:
		correct(0.0,0.0);
		break;
	case ADD:
		add();
		break;
	case BLEND:
		blend();
		break;
	case CONTRASTX:
		contrast();
		break;
	}
}

void CCombineDoc::OnSpecialReset() 
{
	add_factor=1.0;
	rfactor=1.0;
	gfactor=1.0;
	bfactor=1.0;
	correction_factor=1.0;
	sharp_factor=0.25;
	blend_factor=0.5;
	special_rework();
	RefreshScreen();
}

void CCombineDoc::special_reset()
{
	OnSpecialReset();
}

void CCombineDoc::leave_special()
{
	OnSpecialLeavespecialmode();
}
