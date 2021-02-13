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

void CCombineDoc::OnFilterCreatelowpass() 
{
	filter_param=dft.filter_param=1;
	dft.displacement=0.0001f;

	dft.filtering=false;
	tab_pressed=false;
	mode=dft.mode=LOWPASS;

	clearout();
	filter_reset=true;
	BeginWaitCursor();
	editlowpass();
	EndWaitCursor();
	display=OUT_BUFFER;
	RefreshScreen();
}

void CCombineDoc::editlowpass()
{
	if(dft.displacement<0.0f) {dft.displacement=0.0f; }
	if(dft.displacement>1.0f) {dft.displacement=1.0f; }
	if(dft.width<0.0f) {dft.width=0.0f;}

	if(!dft.filtering)
	{
		if(filter_param<1) filter_param=2;
		if(filter_param>2) filter_param=1;
		mode=LOWPASS;
	} else {
		if(filter_param>2 && filter_param<BRIGHTNESS)
		{
			switch(dft.out_mode)
			{
			case OUT_NORMAL:{filter_param=1; mode=LOWPASS; break;}
			case OUT_BRIGHT:
				if(filter_param==BRIGHTNESS-1){filter_param=2; mode=LOWPASS; break;}
				filter_param=BRIGHTNESS;
				mode=FBRIGHTNESS;
				break;
			case OUT_THRESHOLD:
				if(filter_param==THRESHOLD-1){filter_param=2; mode=LOWPASS; break;}
				filter_param=THRESHOLD;
				mode=FTHRESHOLD; break;
			}
		}
		if(filter_param>BRIGHTNESS)
		{
			filter_param=1;
			mode=LOWPASS;
		}
		if(filter_param<1)
		{
			switch(dft.out_mode)
			{
			case OUT_NORMAL: filter_param=2; mode=LOWPASS; break;
			case OUT_BRIGHT: filter_param=BRIGHTNESS; mode=FBRIGHTNESS; break;
			case OUT_THRESHOLD: filter_param=THRESHOLD; mode=FTHRESHOLD; break;
			}
		}
	}

	static float old_displacement,old_width;
	if(filter_reset) old_displacement=-1.0f;
	filter_reset=false;
	if((dft.displacement==old_displacement) && (dft.width==old_width))
	{
		filter_changed=true;
		return;
	}
	old_displacement=dft.displacement;
	old_width=dft.width;

	dftReset();

	if(tab_pressed)
	{
		tab_pressed=false;
		display=OUT_BUFFER;
	}

	if(dft.filtering)
	{
		switch(mode)
		{
		case FBRIGHTNESS:
			editbrightness();
			dftOutAgain();
			break;
		case FTHRESHOLD:
			editthreshold();
			dftOutAgain();
			break;
		default:
			if(filter_changed)
			{
				dft.Low(GetBuffer(DFT_FILTER_BUFFER));
				UpdateBuffer(DFT_FILTER_BUFFER);

				CopyBuffer(DFT_RSTORE_BUFFER,DFT_RIO_BUFFER);
				CopyBuffer(DFT_GSTORE_BUFFER,DFT_GIO_BUFFER);
				CopyBuffer(DFT_BSTORE_BUFFER,DFT_BIO_BUFFER);
				dft.valid_transform3=dft.valid_store3;
				dft.valid_transform1=dft.valid_store1;
				dft.valid_picture1=dft.valid_picture3=false;

				dft.Filter(
					GetBuffer(DFT_FILTER_BUFFER),
					GetBuffer(DFT_RIO_BUFFER),
					GetBuffer(DFT_GIO_BUFFER),
					GetBuffer(DFT_BIO_BUFFER)
					,bandw);
				FreeBuffer(DFT_FILTER_BUFFER);
				UpdateBuffer(DFT_RIO_BUFFER);
				UpdateBuffer(DFT_GIO_BUFFER);
				UpdateBuffer(DFT_BIO_BUFFER);

				dftOut();
			}
		}
	} else {
		if(filter_changed)
		{
			dft.Low(GetBuffer(DFT_FILTER_BUFFER));
			UpdateBuffer(DFT_FILTER_BUFFER);

			dft.VisualizeFilter(GetBuffer(OUT_BUFFER),GetBuffer(DFT_FILTER_BUFFER));
			UpdateBuffer(OUT_BUFFER);
			FreeBuffer(DFT_FILTER_BUFFER);
		}
	}
	filter_changed=true;
}

void CCombineDoc::OnFilterCreatehighpass() 
{
	filter_param=2;
	dft.width=1.0001f;

	dft.filtering=false;
	mode=dft.mode=HIGHPASS;
	clearout();
	filter_reset=true;
	BeginWaitCursor();
	edithighpass();
	EndWaitCursor();
	display=OUT_BUFFER;
	RefreshScreen();
}

void CCombineDoc::ImmediateHighpass()
{
	filter_param=2;
	dft.filtering=true;
	mode=dft.mode=HIGHPASS;
	filter_reset=true;
	void *f=GetBuffer(DFT_FILTER_BUFFER);
	dft.High(f);
	UpdateBuffer(DFT_FILTER_BUFFER);
}

void CCombineDoc::ImmediateLowpass()
{
	filter_param=1;
	dft.filtering=true;
	mode=dft.mode=LOWPASS;
	filter_reset=true;
	void *f=GetBuffer(DFT_FILTER_BUFFER);
	dft.Low(f);
	UpdateBuffer(DFT_FILTER_BUFFER);
}

void CCombineDoc::ImmediateBandpass()
{
	filter_param=2;
	dft.filtering=true;
	mode=dft.mode=BANDPASS;
	filter_reset=true;
	void *f=GetBuffer(DFT_FILTER_BUFFER);
	dft.Pass(f);
	UpdateBuffer(DFT_FILTER_BUFFER);
}

void CCombineDoc::ImmediateBandstop()
{
	filter_param=2;
	dft.filtering=true;
	mode=dft.mode=BANDSTOP;
	filter_reset=true;
	void *f=GetBuffer(DFT_FILTER_BUFFER);
	dft.Stop(f);
	UpdateBuffer(DFT_FILTER_BUFFER);
}

void CCombineDoc::ImmediateButterworth()
{
	filter_param=2;
	dft.filtering=true;
	mode=dft.mode=BUTTERWORTH;
	filter_reset=true;
	void *f=GetBuffer(DFT_FILTER_BUFFER);
	dft.Butterworth(f);
	UpdateBuffer(DFT_FILTER_BUFFER);
}

void CCombineDoc::Refilter()
{
	progress.AddString(IDS_FILTER);

	if(dft.valid_store3)
	{
		CopyBuffer(DFT_RSTORE_BUFFER,DFT_RIO_BUFFER);
		CopyBuffer(DFT_GSTORE_BUFFER,DFT_GIO_BUFFER);
		CopyBuffer(DFT_BSTORE_BUFFER,DFT_BIO_BUFFER);
		dft.valid_transform3=true;
	} else return;

	dft.Filter(
		GetBuffer(DFT_FILTER_BUFFER),
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	FreeBuffer(DFT_FILTER_BUFFER);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dft.TransformOut(
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dft.Out3(
		GetBuffer(OUT_BUFFER),
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(DFT_RIO_BUFFER);
	FreeBuffer(DFT_GIO_BUFFER);
	FreeBuffer(DFT_BIO_BUFFER);
}

void CCombineDoc::edithighpass()
{
	if(dft.displacement<0.0f) {dft.displacement=0.0f; }
	if(dft.displacement>1.0f) {dft.displacement=1.0f; }
	if(dft.width<0.0f) {dft.width=0.0f;}

	if(!dft.filtering)
	{
		if(filter_param<1) filter_param=2;
		if(filter_param>2) filter_param=1;
		mode=HIGHPASS;
	} else {
		if(filter_param>2 && filter_param<BRIGHTNESS)
		{
			switch(dft.out_mode)
			{
			case OUT_NORMAL:{filter_param=1; mode=HIGHPASS; break;}
			case OUT_BRIGHT:
				if(filter_param==BRIGHTNESS-1){filter_param=2; mode=HIGHPASS; break;}
				filter_param=BRIGHTNESS;
				mode=FBRIGHTNESS;
				break;
			case OUT_THRESHOLD:
				if(filter_param==THRESHOLD-1){filter_param=2; mode=HIGHPASS; break;}
				filter_param=THRESHOLD;
				mode=FTHRESHOLD; break;
			}
		}
		if(filter_param>BRIGHTNESS)
		{
			filter_param=1;
			mode=HIGHPASS;
		}
		if(filter_param<1)
		{
			switch(dft.out_mode)
			{
			case OUT_NORMAL: filter_param=2; mode=HIGHPASS; break;
			case OUT_BRIGHT: filter_param=BRIGHTNESS; mode=FBRIGHTNESS; break;
			case OUT_THRESHOLD: filter_param=THRESHOLD; mode=FTHRESHOLD; break;
			}
		}
	}

	static float old_displacement,old_width;
	if(filter_reset) old_displacement=-1.0f;
	filter_reset=false;
	if((dft.displacement==old_displacement) && (dft.width==old_width))
	{
		filter_changed=true;
		return;
	}
	old_displacement=dft.displacement;
	old_width=dft.width;

	dftReset();

	if(tab_pressed)
	{
		tab_pressed=false;
		display=OUT_BUFFER;
	}

	if(dft.filtering)
	{
		switch(mode)
		{
		case FBRIGHTNESS:
			editbrightness();
			dftOutAgain();
			break;
		case FTHRESHOLD:
			editthreshold();
			dftOutAgain();
			break;
		default:
			if(filter_changed)
			{
				dft.High(GetBuffer(DFT_FILTER_BUFFER));
				UpdateBuffer(DFT_FILTER_BUFFER);

				CopyBuffer(DFT_RSTORE_BUFFER,DFT_RIO_BUFFER);
				CopyBuffer(DFT_GSTORE_BUFFER,DFT_GIO_BUFFER);
				CopyBuffer(DFT_BSTORE_BUFFER,DFT_BIO_BUFFER);
				dft.valid_transform3=dft.valid_store3;
				dft.valid_transform1=dft.valid_store1;
				dft.valid_picture1=dft.valid_picture3=false;

				dft.Filter(
					GetBuffer(DFT_FILTER_BUFFER),
					GetBuffer(DFT_RIO_BUFFER),
					GetBuffer(DFT_GIO_BUFFER),
					GetBuffer(DFT_BIO_BUFFER)
					,bandw);
				FreeBuffer(DFT_FILTER_BUFFER);
				UpdateBuffer(DFT_RIO_BUFFER);
				UpdateBuffer(DFT_GIO_BUFFER);
				UpdateBuffer(DFT_BIO_BUFFER);

				dftOut();
			}
		}
	} else {
		if(filter_changed)
		{
			dft.High(GetBuffer(DFT_FILTER_BUFFER));
			UpdateBuffer(DFT_FILTER_BUFFER);

			dft.VisualizeFilter(GetBuffer(OUT_BUFFER),GetBuffer(DFT_FILTER_BUFFER));
			UpdateBuffer(OUT_BUFFER);
			FreeBuffer(DFT_FILTER_BUFFER);
		}
	}
	filter_changed=true;
}

void CCombineDoc::OnFilterCreatebandpass() 
{
	dft.filtering=false;
	mode=dft.mode=BANDPASS;
	clearout();
	filter_reset=true;
	BeginWaitCursor();
	editbandpass();
	EndWaitCursor();
	display=OUT_BUFFER;
	RefreshScreen();
}

void CCombineDoc::editbandpass()
{
	if(dft.displacement<0.0f) {dft.displacement=0.0f; }
	if(dft.displacement>1.0f) {dft.displacement=1.0f; }
	if(dft.width<0.0f) {dft.width=0.0f; }
	if(dft.width>1.0f) {dft.width=1.0f; }

	if(!dft.filtering)
	{
		if(filter_param<1) filter_param=2;
		if(filter_param>2) filter_param=1;
		mode=BANDPASS;
	} else {
		if(filter_param>2 && filter_param<BRIGHTNESS)
		{
			switch(dft.out_mode)
			{
			case OUT_NORMAL:{filter_param=1; mode=BANDPASS; break;}
			case OUT_BRIGHT:
				if(filter_param==BRIGHTNESS-1){filter_param=2; mode=BANDPASS; break;}
				else filter_param=BRIGHTNESS;
				mode=FBRIGHTNESS;
				break;
			case OUT_THRESHOLD:
				if(filter_param==THRESHOLD-1){filter_param=2; mode=FTHRESHOLD; break;}
				else filter_param=THRESHOLD;
				mode=FTHRESHOLD; break;
			}
		}
		if(filter_param>BRIGHTNESS)
		{
			filter_param=1;
			mode=BANDPASS;
		}
		if(filter_param<1)
		{
			switch(dft.out_mode)
			{
			case OUT_NORMAL: filter_param=2; mode=BANDPASS; break;
			case OUT_BRIGHT: filter_param=BRIGHTNESS; mode=FBRIGHTNESS; break;
			case OUT_THRESHOLD: filter_param=THRESHOLD; mode=FTHRESHOLD; break;
			}
		}
	}

	static float old_displacement,old_width;
	if(filter_reset) old_displacement=-1.0f;
	filter_reset=false;
	if((dft.displacement==old_displacement) && (dft.width==old_width))
	{
		filter_changed=true;
		return;
	}
	old_displacement=dft.displacement;
	old_width=dft.width;

	dftReset();

	if(tab_pressed)
	{
		tab_pressed=false;
		display=OUT_BUFFER;
	}

	if(dft.filtering)
	{
		switch(mode)
		{
		case FBRIGHTNESS:
			editbrightness();
			dftOutAgain();
			break;
		case FTHRESHOLD:
			editthreshold();
			dftOutAgain();
			break;
		default:
			if(filter_changed)
			{
				dft.Pass(GetBuffer(DFT_FILTER_BUFFER));
				UpdateBuffer(DFT_FILTER_BUFFER);

				CopyBuffer(DFT_RSTORE_BUFFER,DFT_RIO_BUFFER);
				CopyBuffer(DFT_GSTORE_BUFFER,DFT_GIO_BUFFER);
				CopyBuffer(DFT_BSTORE_BUFFER,DFT_BIO_BUFFER);
				dft.valid_transform3=dft.valid_store3;
				dft.valid_transform1=dft.valid_store1;
				dft.valid_picture1=dft.valid_picture3=false;

				dft.Filter(
					GetBuffer(DFT_FILTER_BUFFER),
					GetBuffer(DFT_RIO_BUFFER),
					GetBuffer(DFT_GIO_BUFFER),
					GetBuffer(DFT_BIO_BUFFER)
					,bandw);
				FreeBuffer(DFT_FILTER_BUFFER);
				UpdateBuffer(DFT_RIO_BUFFER);
				UpdateBuffer(DFT_GIO_BUFFER);
				UpdateBuffer(DFT_BIO_BUFFER);

				dftOut();
			}
		}
	} else {
		if(filter_changed)
		{
			dft.Pass(GetBuffer(DFT_FILTER_BUFFER));
			UpdateBuffer(DFT_FILTER_BUFFER);

			if(!macro_mode)
			{
				dft.VisualizeFilter(GetBuffer(OUT_BUFFER),GetBuffer(DFT_FILTER_BUFFER));
				UpdateBuffer(OUT_BUFFER);
				FreeBuffer(DFT_FILTER_BUFFER);
			}
		}
	}
	filter_changed=true;
}

void CCombineDoc::OnFilterCreatebandstop() 
{
	filter_param=dft.filter_param=1;

	dft.filtering=false;
	tab_pressed=false;
	mode=dft.mode=BANDSTOP;

	clearout();
	filter_reset=true;
	BeginWaitCursor();
	editbandstop();
	EndWaitCursor();
	display=OUT_BUFFER;
	RefreshScreen();
}

void CCombineDoc::editbandstop()
{
	if(dft.width<0.0f) {dft.width=0.0f; }
	if(dft.width>1.0f) {dft.width=1.0f; }
	if(dft.displacement<0.0f) {dft.displacement=0.0f; }
	if(dft.displacement>0.5f) {dft.displacement=0.5f; }
	if(dft.centre<0.0f) {dft.centre=0.0f; return;}
	if(dft.centre>1.0f) {dft.centre=1.0f; return;}

	if(!dft.filtering)
	{
		if(filter_param<1) filter_param=5;
		if(filter_param==3) filter_param=5;
		if(filter_param==4) filter_param=2;
		if(filter_param>5) filter_param=1;
		mode=BANDSTOP;
	} else {
		if(filter_param==3){filter_param=5; mode=BANDSTOP;}
		if(filter_param==4){filter_param=2; mode=BANDSTOP;}
		if(filter_param>5 && filter_param<=BRIGHTNESS)
		{
			switch(dft.out_mode)
			{
			case OUT_NORMAL:{filter_param=1; mode=BANDSTOP; break;}
			case OUT_BRIGHT:
				if(filter_param==BRIGHTNESS-1){filter_param=5; mode=BANDSTOP; break;}
				filter_param=BRIGHTNESS;
				mode=FBRIGHTNESS;
				break;
			case OUT_THRESHOLD:
				if(filter_param==THRESHOLD-1){filter_param=5; mode=FTHRESHOLD; break;}
				filter_param=THRESHOLD;
				mode=FTHRESHOLD;
			}
		}
		if(filter_param>BRIGHTNESS)
		{
			filter_param=1;
			mode=BANDSTOP;
		}
		if(filter_param<1)
		{
			switch(dft.out_mode)
			{
			case OUT_NORMAL: filter_param=5; mode=BANDSTOP; break;
			case OUT_BRIGHT: filter_param=BRIGHTNESS; mode=FBRIGHTNESS; break;
			case OUT_THRESHOLD: filter_param=THRESHOLD; mode=FTHRESHOLD; break;
			}
		}
	}

	static float old_displacement,old_width,old_centre;
	if(filter_reset) old_displacement=-1.0f;
	filter_reset=false;
	if((dft.displacement==old_displacement) && (dft.width==old_centre) && (dft.width==old_centre))
	{
		filter_changed=true;
		return;
	}
	old_displacement=dft.displacement;
	old_width=dft.width;
	old_centre=dft.centre;

	dftReset();

	if(tab_pressed)
	{
		tab_pressed=false;
		display=OUT_BUFFER;
	}

	if(dft.filtering)
	{
		switch(mode)
		{
		case FBRIGHTNESS:
			editbrightness();
			dftOutAgain();
			break;
		case FTHRESHOLD:
			editthreshold();
			dftOutAgain();
			break;
		default:
			if(filter_changed)
			{
				dft.Stop(GetBuffer(DFT_FILTER_BUFFER));
				UpdateBuffer(DFT_FILTER_BUFFER);

				CopyBuffer(DFT_RSTORE_BUFFER,DFT_RIO_BUFFER);
				CopyBuffer(DFT_GSTORE_BUFFER,DFT_GIO_BUFFER);
				CopyBuffer(DFT_BSTORE_BUFFER,DFT_BIO_BUFFER);
				dft.valid_transform3=dft.valid_store3;
				dft.valid_transform1=dft.valid_store1;
				dft.valid_picture1=dft.valid_picture3=false;

				dft.Filter(
					GetBuffer(DFT_FILTER_BUFFER),
					GetBuffer(DFT_RIO_BUFFER),
					GetBuffer(DFT_GIO_BUFFER),
					GetBuffer(DFT_BIO_BUFFER)
					,bandw);
				FreeBuffer(DFT_FILTER_BUFFER);
				UpdateBuffer(DFT_RIO_BUFFER);
				UpdateBuffer(DFT_GIO_BUFFER);
				UpdateBuffer(DFT_BIO_BUFFER);

				dftOut();
			}
		}
	} else {
		if(filter_changed)
		{
			dft.Stop(GetBuffer(DFT_FILTER_BUFFER));
			UpdateBuffer(DFT_FILTER_BUFFER);

			dft.VisualizeFilter(GetBuffer(OUT_BUFFER),GetBuffer(DFT_FILTER_BUFFER));
			FreeBuffer(DFT_FILTER_BUFFER);
			UpdateBuffer(OUT_BUFFER);
		}
	}
	filter_changed=true;
}

void CCombineDoc::OnFilterCreatebutterworth() 
{
	dft.filtering=false;
	mode=dft.mode=BUTTERWORTH;
	clearout();
	filter_reset=true;
	BeginWaitCursor();
	editbutterworth();
	EndWaitCursor();
	display=OUT_BUFFER;
	RefreshScreen();
}

void CCombineDoc::editbutterworth()
{
	if(dft.cutoff<0.0f) {dft.cutoff=0.0f; }
	if(dft.cutoff>1.0f) {dft.cutoff=1.0f; }
	if(dft.order<1.0f) {dft.order=1.0f; }
	if(dft.displacement<0.0f) {dft.displacement=0.0f; }
	if(dft.displacement>1.0f) {dft.displacement=1.0f; }

	if(!dft.filtering)
	{
		if(filter_param<2) filter_param=4;
		if(filter_param>4) filter_param=2;
		mode=BUTTERWORTH;
	} else {
		if(filter_param>4 && filter_param<BRIGHTNESS)
		{
			switch(dft.out_mode)
			{
			case OUT_NORMAL:{filter_param=2; mode=BUTTERWORTH; break;}
			case OUT_BRIGHT:
				if(filter_param==BRIGHTNESS-1){filter_param=4; mode=BUTTERWORTH; break;}
				else filter_param=BRIGHTNESS;
				mode=FBRIGHTNESS;
				break;
			case OUT_THRESHOLD:
				if(filter_param==THRESHOLD-1){filter_param=4; mode=FTHRESHOLD; break;}
				else filter_param=THRESHOLD;
				mode=FTHRESHOLD;
			}
		}
		if(filter_param>BRIGHTNESS)
		{
			filter_param=2;
			mode=BUTTERWORTH;
		}
		if(filter_param<2)
		{
			switch(dft.out_mode)
			{
			case OUT_NORMAL: filter_param=4; mode=BUTTERWORTH; break;
			case OUT_BRIGHT: filter_param=BRIGHTNESS; mode=FBRIGHTNESS; break;
			case OUT_THRESHOLD: filter_param=THRESHOLD; mode=FTHRESHOLD; break;
			}
		}
	}

	static float old_displacement,old_cutoff,old_order;
	if(filter_reset) old_displacement=-1.0f;
	filter_reset=false;
	if((dft.displacement==old_displacement) && (dft.cutoff==old_cutoff) && (dft.order==old_order))
	{
		filter_changed=true;
		return;
	}
	old_displacement=dft.displacement;
	old_cutoff=dft.cutoff;
	old_order=dft.order;

	dftReset();

	if(tab_pressed)
	{
		tab_pressed=false;
		display=OUT_BUFFER;
	}

	if(dft.filtering)
	{
		switch(mode)
		{
		case FBRIGHTNESS:
			editbrightness();
			dftOutAgain();
			break;
		case FTHRESHOLD:
			editthreshold();
			dftOutAgain();
			break;
		default:
			if(filter_changed)
			{
				dft.Butterworth(GetBuffer(DFT_FILTER_BUFFER));
				UpdateBuffer(DFT_FILTER_BUFFER);

				CopyBuffer(DFT_RSTORE_BUFFER,DFT_RIO_BUFFER);
				CopyBuffer(DFT_GSTORE_BUFFER,DFT_GIO_BUFFER);
				CopyBuffer(DFT_BSTORE_BUFFER,DFT_BIO_BUFFER);
				dft.valid_transform3=dft.valid_store3;
				dft.valid_transform1=dft.valid_store1;
				dft.valid_picture1=dft.valid_picture3=false;

				dft.Filter(
					GetBuffer(DFT_FILTER_BUFFER),
					GetBuffer(DFT_RIO_BUFFER),
					GetBuffer(DFT_GIO_BUFFER),
					GetBuffer(DFT_BIO_BUFFER)
					,bandw);
				FreeBuffer(DFT_FILTER_BUFFER);
				UpdateBuffer(DFT_RIO_BUFFER);
				UpdateBuffer(DFT_GIO_BUFFER);
				UpdateBuffer(DFT_BIO_BUFFER);

				dftOut();
			}
		}
	} else {
		if(filter_changed)
		{
			dft.Butterworth(GetBuffer(DFT_FILTER_BUFFER));
			UpdateBuffer(DFT_FILTER_BUFFER);

			if(!macro_mode)
			{
				dft.VisualizeFilter(GetBuffer(OUT_BUFFER),GetBuffer(DFT_FILTER_BUFFER));
				UpdateBuffer(OUT_BUFFER);
				FreeBuffer(DFT_FILTER_BUFFER);
			}
		}
	}
	filter_changed=true;
}

void CCombineDoc::OnFilterThreshold() 
{
	dft.out_mode=OUT_THRESHOLD;
	if(!dft.valid_picture3) return;

	dft.filtering=true;
	mode=FTHRESHOLD;
	if(!((dft.mode&GROUP_MASK)==FILTER)) dft.mode=FTHRESHOLD;
	if(dft.filtering || !((dft.mode&GROUP_MASK)==FILTER)) filter_param=THRESHOLD;
	editthreshold();

	display=OUT_BUFFER;
	RefreshScreen();
}

void CCombineDoc::editthreshold()
{
	if(dft.threshold<0.0f){dft.threshold=0.0f; return;}
	BeginWaitCursor();

	if(!(mode==FTHRESHOLD && mode==dft.mode))
	{
		if(filter_param!=THRESHOLD && !dft.filtering && (dft.mode&GROUP_MASK)==FILTER)
		{
			mode=dft.mode;
			filter_param=dft.filter_param;
			EndWaitCursor();
			return;
		} else filter_param=THRESHOLD;
	}

	CopyBuffer(DFT_RSTORE_BUFFER,DFT_RIO_BUFFER);
	CopyBuffer(DFT_GSTORE_BUFFER,DFT_GIO_BUFFER);
	CopyBuffer(DFT_BSTORE_BUFFER,DFT_BIO_BUFFER);
	if(dft.valid_picture3)
	{
		dft.valid_transform3=true;
		dft.valid_transform1=false;
	} else {
		dft.valid_transform1=true;
		dft.valid_transform3=false;
	}
	dft.valid_picture1=dft.valid_picture3=false;

	dft.Filter(
		GetBuffer(DFT_FILTER_BUFFER),
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	FreeBuffer(DFT_FILTER_BUFFER);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dftOut();

	display=OUT_BUFFER;
	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::OnFilterBrightness() 
{
	dft.out_mode=OUT_BRIGHT;
	if(!dft.valid_picture3) return;

	dft.filtering=true;
	mode=FBRIGHTNESS;
	if(!((dft.mode&GROUP_MASK)==FILTER)) dft.mode=FBRIGHTNESS;
	if(dft.filtering || !((dft.mode&GROUP_MASK)==FILTER)) filter_param=BRIGHTNESS;
	editbrightness();

	display=OUT_BUFFER;
	RefreshScreen();
}

void CCombineDoc::editbrightness()
{
	if(dft.brightness<0.0f){dft.brightness=0.0f; return;}
	BeginWaitCursor();

	if(!(mode==FBRIGHTNESS && mode==dft.mode))
	{
		if(filter_param!=BRIGHTNESS && !dft.filtering && (dft.mode&GROUP_MASK)==FILTER)
		{
			mode=dft.mode;
			filter_param=dft.filter_param;
			EndWaitCursor();
			return;
		} else filter_param=BRIGHTNESS;
	}

	CopyBuffer(DFT_RSTORE_BUFFER,DFT_RIO_BUFFER);
	CopyBuffer(DFT_GSTORE_BUFFER,DFT_GIO_BUFFER);
	CopyBuffer(DFT_BSTORE_BUFFER,DFT_BIO_BUFFER);
	if(dft.valid_picture3)
	{
		dft.valid_transform3=true;
		dft.valid_transform1=false;
	} else {
		dft.valid_transform1=true;
		dft.valid_transform3=false;
	}
	dft.valid_picture1=dft.valid_picture3=false;

	dft.Filter(
		GetBuffer(DFT_FILTER_BUFFER),
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	FreeBuffer(DFT_FILTER_BUFFER);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dftOut();

	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::OnFilterNormalOutput() 
{
	dft.out_mode=OUT_NORMAL;
	if(!dft.valid_picture3) return;
	
	mode=dft.mode;
	filter_param=dft.filter_param;
	if(mode==FBRIGHTNESS || mode==FTHRESHOLD) mode=dft.mode=0;
	if(!dft.filtering && (mode&GROUP_MASK)==FILTER) return;

	BeginWaitCursor();

	CopyBuffer(DFT_RSTORE_BUFFER,DFT_RIO_BUFFER);
	CopyBuffer(DFT_GSTORE_BUFFER,DFT_GIO_BUFFER);
	CopyBuffer(DFT_BSTORE_BUFFER,DFT_BIO_BUFFER);
	dft.valid_transform3=true;
	dft.valid_transform1=false;
	dft.valid_picture1=dft.valid_picture3=false;

	dft.Filter(
		GetBuffer(DFT_FILTER_BUFFER),
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	FreeBuffer(DFT_FILTER_BUFFER);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dftOut();

	EndWaitCursor();

	display=OUT_BUFFER;
	RefreshScreen();
}

void CCombineDoc::alter_filter(int key)
{
	CRect rect(0,0,1,1);

	BeginWaitCursor();
	switch(key)
	{
	case VK_UP:
		if(mode==FILTER || (tab_pressed && dft.filtering))
		{
			tab_pressed=true;
			active_frame++;
			if(active_frame>top_frame-1) active_frame=top_frame-1;
			display=active_frame;
			break;
		}
	case VK_PRIOR:
		switch(dft.mode)
		{
		case FTHRESHOLD:
		case FBRIGHTNESS:
			break;
		case LOWPASS:
			filter_param++;
			filter_changed=false;
			editlowpass();
			break;
		case HIGHPASS:
			filter_param++;
			filter_changed=false;
			edithighpass();
			break;
		case BANDPASS:
			filter_param++;
			filter_changed=false;
			editbandpass();
			break;
		case BANDSTOP:
			filter_param++;
			filter_changed=false;
			editbandstop();
			break;
		case BUTTERWORTH:
			filter_param++;
			filter_changed=false;
			editbutterworth();
			break;
		}
		EndWaitCursor();
		Redraw(&rect,false);
		return;
	case VK_DOWN:
		if(mode==FILTER || (tab_pressed && dft.filtering))
		{
			tab_pressed=true;
			active_frame--;
			if(active_frame<0) active_frame=0;
			display=active_frame;
			break;
		}
	case VK_NEXT:
		switch(dft.mode)
		{
		case FTHRESHOLD:
		case FBRIGHTNESS:
			break;
		case LOWPASS:
			filter_param--;
			filter_changed=false;
			editlowpass();
			break;
		case HIGHPASS:
			filter_param--;
			filter_changed=false;
			edithighpass();
			break;
		case BANDPASS:
			filter_param--;
			filter_changed=false;
			editbandpass();
			break;
		case BANDSTOP:
			filter_param--;
			filter_changed=false;
			editbandstop();
			break;
		case BUTTERWORTH:
			filter_param--;
			filter_changed=false;
			editbutterworth();
			break;
		}
		EndWaitCursor();
		Redraw(&rect,false);
		return;
	case VK_LEFT:
		switch(mode)
		{
		case FBRIGHTNESS:
			dft.brightness-=0.01f;
			editbrightness();
			break;
		case FTHRESHOLD:
			dft.threshold-=0.1f;
			editthreshold();
			break;
		case LOWPASS:
			if(filter_param==WIDTH) dft.width-=dft.small_delta;
			else dft.displacement-=dft.small_delta;
			editlowpass();
			break;
		case HIGHPASS:
			if(filter_param==WIDTH) dft.width-=dft.small_delta;
			else dft.displacement-=dft.small_delta;
			edithighpass();
			break;
		case BANDSTOP:
			switch(filter_param)
			{
			case WIDTH:
				dft.width-=dft.small_delta;
				break;
			case CENTRE:
				dft.centre-=dft.small_delta;
				break;
			case DISPLACEMENT:
				dft.displacement+=dft.small_delta;
				break;
			}
			editbandstop();
			break;
		case BANDPASS:
			switch(filter_param)
			{
			case WIDTH:
				dft.width-=dft.small_delta;
				break;
			case CENTRE:
				dft.centre-=dft.small_delta;
				break;
			case DISPLACEMENT:
				dft.displacement+=dft.small_delta;
				break;
			}
			editbandpass();
			break;
		case BUTTERWORTH:
			switch(filter_param)
			{
			case CUTOFF:
				dft.cutoff+=dft.small_delta;
				break;
			case ORDER:
				dft.order-=1.0f;
				break;
			case DISPLACEMENT:
				dft.displacement-=dft.small_delta;
				break;
			}
			editbutterworth();
			break;
		}
		break;
	case VK_RIGHT:
		switch(mode)
		{
		case FBRIGHTNESS:
			dft.brightness+=0.01f;
			editbrightness();
			break;
		case FTHRESHOLD:
			dft.threshold+=0.1f;
			editthreshold();
			break;
		case LOWPASS:
			if(filter_param==WIDTH) dft.width+=dft.small_delta;
			else dft.displacement+=dft.small_delta;
			editlowpass();
			break;
		case HIGHPASS:
			if(filter_param==WIDTH) dft.width+=dft.small_delta;
			else dft.displacement+=dft.small_delta;
			edithighpass();
			break;
		case BANDSTOP:
			switch(filter_param)
			{
			case WIDTH:
				dft.width+=dft.small_delta;
				break;
			case CENTRE:
				dft.centre+=dft.small_delta;
				break;
			case DISPLACEMENT:
				dft.displacement-=dft.small_delta;
				break;
			}
			editbandstop();
			break;
		case BANDPASS:
			switch(filter_param)
			{
			case WIDTH:
				dft.width+=dft.small_delta;
				break;
			case CENTRE:
				dft.centre+=dft.small_delta;
				break;
			case DISPLACEMENT:
				dft.displacement-=dft.small_delta;
				break;
			}
			editbandpass();
			break;
		case BUTTERWORTH:
			switch(filter_param)
			{
			case CUTOFF:
				dft.cutoff-=dft.small_delta;
				break;
			case ORDER:
				dft.order+=1.0f;
				break;
			case DISPLACEMENT:
				dft.displacement+=dft.small_delta;
				break;
			}
			editbutterworth();
			break;
		}
		break;
	case 'N':
		switch(mode)
		{
		case FBRIGHTNESS:
			dft.brightness-=0.1f;
			editbrightness();
			break;
		case FTHRESHOLD:
			dft.threshold-=1.0f;
			editthreshold();
			break;
		case LOWPASS:
			if(filter_param==WIDTH) dft.width-=dft.big_delta;
			else dft.displacement-=dft.big_delta;
			editlowpass();
			break;
		case HIGHPASS:
			if(filter_param==WIDTH) dft.width-=dft.big_delta;
			else dft.displacement-=dft.big_delta;
			edithighpass();
			break;
		case BANDSTOP:
			switch(filter_param)
			{
			case WIDTH:
				dft.width-=dft.big_delta;
				break;
			case CENTRE:
				dft.centre-=dft.big_delta;
				break;
			case DISPLACEMENT:
				dft.displacement+=dft.big_delta;
				break;
			}
			editbandstop();
			break;
		case BANDPASS:
			switch(filter_param)
			{
			case WIDTH:
				dft.width-=dft.big_delta;
				break;
			case CENTRE:
				dft.centre-=dft.big_delta;
				break;
			case DISPLACEMENT:
				dft.displacement+=dft.big_delta;
				break;
			}
			editbandpass();
			break;
		case BUTTERWORTH:
			switch(filter_param)
			{
			case CUTOFF:
				dft.cutoff+=dft.big_delta;
				break;
			case ORDER:
				dft.order-=10.0f;
				break;
			case DISPLACEMENT:
				dft.displacement-=dft.big_delta;
				break;
			}
			editbutterworth();
			break;
		}
		break;
	case 'M':
		switch(mode)
		{
		case FBRIGHTNESS:
			dft.brightness+=0.1f;
			editbrightness();
			break;
		case FTHRESHOLD:
			dft.threshold+=1.0f;
			editthreshold();
			break;
		case LOWPASS:
			if(filter_param==WIDTH) dft.width+=dft.big_delta;
			else dft.displacement+=dft.big_delta;
			editlowpass();
			break;
		case HIGHPASS:
			if(filter_param==WIDTH) dft.width+=dft.big_delta;
			else dft.displacement+=dft.big_delta;
			edithighpass();
			break;
		case BANDSTOP:
			switch(filter_param)
			{
			case WIDTH:
				dft.width+=dft.big_delta;
				break;
			case CENTRE:
				dft.centre+=dft.big_delta;
				break;
			case DISPLACEMENT:
				dft.displacement-=dft.big_delta;
				break;
			}
			editbandstop();
			break;
		case BANDPASS:
			switch(filter_param)
			{
			case WIDTH:
				dft.width+=dft.big_delta;
				break;
			case CENTRE:
				dft.centre+=dft.big_delta;
				break;
			case DISPLACEMENT:
				dft.displacement-=dft.big_delta;
				break;
			}
			editbandpass();
			break;
		case BUTTERWORTH:
			switch(filter_param)
			{
			case CUTOFF:
				dft.cutoff-=dft.big_delta;
				break;
			case ORDER:
				dft.order+=10.0f;
				break;
			case DISPLACEMENT:
				dft.displacement+=dft.big_delta;
				break;
			}
			editbutterworth();
			break;
		}
		break;
	case 'V':
		getvalue();
		break;
	case VK_NUMPAD0: case '0':
	case VK_NUMPAD1: case '1':
	case VK_NUMPAD2: case '2':
	case VK_NUMPAD3: case '3':
	case VK_NUMPAD4: case '4':
	case VK_NUMPAD5: case '5':
	case VK_NUMPAD6: case '6':
	case VK_NUMPAD7: case '7':
	case VK_NUMPAD8: case '8':
	case VK_NUMPAD9: case '9':
		getvalue(key);
		break;
	case VK_RETURN:
		if(dft.mode==FILTER) break;
		if(!dft.filtering && !(dft.mode==FBRIGHTNESS || dft.mode==FTHRESHOLD))
		{
			dft.filtering=true;

			if(!((dft.valid_store3) || (dft.valid_store1)))
			{
				dftIn();

				CopyBuffer(DFT_RIO_BUFFER,DFT_RSTORE_BUFFER);
				CopyBuffer(DFT_GIO_BUFFER,DFT_GSTORE_BUFFER);
				CopyBuffer(DFT_BIO_BUFFER,DFT_BSTORE_BUFFER);
				dft.valid_store1=false;
				dft.valid_store3=true;
			} else {
				CopyBuffer(DFT_RSTORE_BUFFER,DFT_RIO_BUFFER);
				CopyBuffer(DFT_GSTORE_BUFFER,DFT_GIO_BUFFER);
				CopyBuffer(DFT_BSTORE_BUFFER,DFT_BIO_BUFFER);
				dft.valid_picture1=dft.valid_picture3=false;
				dft.valid_transform1=dft.valid_store1;
				dft.valid_transform3=dft.valid_store3;
			}

			mode=dft.mode;

			dft.Filter(
				GetBuffer(DFT_FILTER_BUFFER),
				GetBuffer(DFT_RIO_BUFFER),
				GetBuffer(DFT_GIO_BUFFER),
				GetBuffer(DFT_BIO_BUFFER)
				,bandw);
			FreeBuffer(DFT_FILTER_BUFFER);
			UpdateBuffer(DFT_RIO_BUFFER);
			UpdateBuffer(DFT_GIO_BUFFER);
			UpdateBuffer(DFT_BIO_BUFFER);

			dftOut();

			switch(dft.out_mode)
			{
			case OUT_BRIGHT:
				filter_param=BRIGHTNESS;
				mode=FBRIGHTNESS;
				break;
			case OUT_NORMAL:
				break;
			case OUT_THRESHOLD:
				filter_param=THRESHOLD;
				mode=FTHRESHOLD;
			}
			rework_filter();
			dftReOut();
			break;
		}
		filterreport();
		dft.filtering=false;
		mode=dft.mode=0;
		filter_param=dft.filter_param=0;
		break;
	case VK_TAB:
		if(tab_pressed)
		{
			tab_pressed=false;
			if(depth_flag && dft.valid_store1)
			{
				if(!dft.filtering)
				{
					clearout();

					dft.VisualizeFilter(GetBuffer(OUT_BUFFER),GetBuffer(DFT_FILTER_BUFFER));
					FreeBuffer(DFT_FILTER_BUFFER);
					UpdateBuffer(OUT_BUFFER);
				}
				else
				{
					dft.Out1(GetBuffer(OUT_BUFFER),255*top_frame,GetBuffer(DFT_RIO_BUFFER));
					FreeBuffer(DFT_RIO_BUFFER);
					UpdateBuffer(OUT_BUFFER);
				}
			}
			display=OUT_BUFFER;
			break;
		}
		tab_pressed=true;
		if(dft.valid_store1)
		{
			if(!depth_flag) OnViewToggledepthmap();
			else
			{
				redraw_picture();
			}
			display=OUT_BUFFER;
		}
		else display=active_frame;
		break;
	case 'A':
		selectall();
		break;
	default:
		EndWaitCursor();
		return;
	}
	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::rework_filter()
{
	BeginWaitCursor();
	switch(dft.mode)
	{
	case LOWPASS:
		editlowpass();
		break;
	case HIGHPASS:
		edithighpass();
		break;
	case BANDPASS:
		editbandpass();
		break;
	case BANDSTOP:
		editbandstop();
		break;
	case BUTTERWORTH:
		editbutterworth();
		break;
	}
	EndWaitCursor();
}

VOID CCombineDoc::dftReset()
{
	dft.Reset(w,h,path,GetBuffer(DFT_RIO_BUFFER));
	FreeBuffer(DFT_RIO_BUFFER);
}

void CCombineDoc::dftIn()
{
	dft.In3(
		GetBuffer(active_frame),
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	FreeBuffer(active_frame);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dft.TransformIn(
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);
}

void CCombineDoc::dftOut()
{
	if(dft.valid_transform3)
	{
		dft.TransformOut(
			GetBuffer(DFT_RIO_BUFFER),
			GetBuffer(DFT_GIO_BUFFER),
			GetBuffer(DFT_BIO_BUFFER)
			,bandw);
		UpdateBuffer(DFT_RIO_BUFFER);
		UpdateBuffer(DFT_GIO_BUFFER);
		UpdateBuffer(DFT_BIO_BUFFER);

		dft.Out3(
			GetBuffer(OUT_BUFFER),
			GetBuffer(DFT_RIO_BUFFER),
			GetBuffer(DFT_GIO_BUFFER),
			GetBuffer(DFT_BIO_BUFFER)
			,bandw);
		UpdateBuffer(OUT_BUFFER);
		FreeBuffer(DFT_RIO_BUFFER);
		FreeBuffer(DFT_GIO_BUFFER);
		FreeBuffer(DFT_BIO_BUFFER);
		return;
	}

	if(dft.valid_transform1)
	{
		dft.TransformOut1(GetBuffer(DFT_RIO_BUFFER));
		UpdateBuffer(DFT_RIO_BUFFER);

		dft.Out1(GetBuffer(OUT_BUFFER),255*top_frame,GetBuffer(DFT_RIO_BUFFER));
		UpdateBuffer(OUT_BUFFER);
		FreeBuffer(DFT_RIO_BUFFER);
	}
}

void CCombineDoc::OnFiltertodepth() 
{
	save_state();
	if(dft.valid_picture1) dftouttodepth();
	backup_picture_valid=false;
	redraw_picture();
	display=PICTURE_BUFFER;
	RefreshScreen();
}

void CCombineDoc::dftouttodepth()
{
	CString str1; str1.LoadString(IDS_STRING201);
	progress.AddString("Copy Last Filter Output to Depthmap");
	dft.OutToDM(GetBuffer(DEPTH_BUFFER),255*top_frame,GetBuffer(DFT_RIO_BUFFER));
	UpdateBuffer(DEPTH_BUFFER);
	FreeBuffer(DFT_RIO_BUFFER);
}

void CCombineDoc::dftOutAgain()
{
	if(dft.valid_transform3)
	{
		dft.Out3(
			GetBuffer(OUT_BUFFER),
			GetBuffer(DFT_RIO_BUFFER),
			GetBuffer(DFT_GIO_BUFFER),
			GetBuffer(DFT_BIO_BUFFER)
			,bandw);
		UpdateBuffer(OUT_BUFFER);
		FreeBuffer(DFT_RIO_BUFFER);
		FreeBuffer(DFT_GIO_BUFFER);
		FreeBuffer(DFT_BIO_BUFFER);
	}

	if(dft.valid_transform1)
	{
		dft.Out1((PIXEL*)GetBuffer(OUT_BUFFER),255*top_frame,GetBuffer(DFT_RIO_BUFFER));
		UpdateBuffer(OUT_BUFFER);
		FreeBuffer(DFT_RIO_BUFFER);
	}
}

void CCombineDoc::dftReOut()
{
	dftOutAgain();
	display=OUT_BUFFER;
	RefreshScreen();
}

void CCombineDoc::OnFilterResumefiltering() // filtr active frame in place
{
	CString str1; str1.LoadString(IDS_STRING201);
	progress.AddString("Filter Active Frame");
	BeginWaitCursor();
	filteractiveframe();
	EndWaitCursor();
	RefreshScreen();
	display=active_frame;
}


void CCombineDoc::filterframe(PIXEL *in, PIXEL *out)
{
	void *r,*g,*b,*f;

	r=GetBuffer(DFT_RIO_BUFFER);
	g=GetBuffer(DFT_GIO_BUFFER);
	b=GetBuffer(DFT_BIO_BUFFER);
	f=GetBuffer(DFT_FILTER_BUFFER);

	dft.In3(in,r,g,b,bandw);

	dft.TransformIn(r,g,b,bandw);

	dft.Filter(f,r,g,b,bandw);

	dft.TransformOut(r,g,b,bandw);

	dft.Out3(out,r,g,b,bandw);

	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);
	FreeBuffer(DFT_FILTER_BUFFER);

	mode=0;
}

void CCombineDoc::filteractiveframe()
{
	void *r,*g,*b,*f,*af;

	r=GetBuffer(DFT_RIO_BUFFER);
	g=GetBuffer(DFT_GIO_BUFFER);
	b=GetBuffer(DFT_BIO_BUFFER);
	f=GetBuffer(DFT_FILTER_BUFFER);
	af=GetBuffer(active_frame);

	dft.In3(af,r,g,b,bandw);

	dft.TransformIn(r,g,b,bandw);

	dft.Filter(f,r,g,b,bandw);

	dft.TransformOut(r,g,b,bandw);

	dft.Out3(af,r,g,b,bandw);

	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);
	FreeBuffer(DFT_FILTER_BUFFER);
	UpdateBuffer(active_frame);
}

void CCombineDoc::OnFilterFilterallframes() 
{
	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;
	progress.Maximize();
	AfxBeginThread(filterallframes,this);
}

UINT CCombineDoc::filterallframes(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->filterallframes();
	return 0;
}

void CCombineDoc::filterallframes()
{
	buissy=true;
	progress.AddString(IDS_DFTALLFRAMES);
	int f;
	mode=FILTER;
	GetBuffer(DFT_FILTER_BUFFER); // force it to stay in memory

	progress.SetRange(0,top_used_frame-1);
	for(f=0; f<top_used_frame; f++)
	{
		if(progress.m_cancel)
		{
			progress.m_cancel=false;
			break;
		}
		progress.AddString("   "+buffer[used_frame[f]].name);
		progress.SetPos(used_frame[f]);
		active_frame=used_frame[f];
		filteractiveframe();
	}
	FreeBuffer(DFT_FILTER_BUFFER);
	progress.SetPos(0);
	backup_picture_valid=false;
	redraw_picture();
	dft.mode=0;
	mode=0;
	if(refreshscreen)
	{
		refreshscreen=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}
}

void CCombineDoc::FilterActive()
{
	dftReset();

	dft.In3(
		GetBuffer(active_frame),
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	FreeBuffer(active_frame);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dft.TransformIn(
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	CopyBuffer(DFT_RIO_BUFFER,DFT_RSTORE_BUFFER);
	CopyBuffer(DFT_GIO_BUFFER,DFT_GSTORE_BUFFER);
	CopyBuffer(DFT_BIO_BUFFER,DFT_BSTORE_BUFFER);
	dft.valid_store3=true;
	dft.valid_store1=false;

	dft.Filter(
		GetBuffer(DFT_FILTER_BUFFER),
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	FreeBuffer(DFT_FILTER_BUFFER);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dft.TransformOut(
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dft.Out3(
		GetBuffer(OUT_BUFFER),
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(DFT_RIO_BUFFER);
	FreeBuffer(DFT_GIO_BUFFER);
	FreeBuffer(DFT_BIO_BUFFER);
}

void CCombineDoc::OnFilterTransformarithmeticMakexfromactiveframe() 
{
	BeginWaitCursor();

	CString str1; str1.LoadString(IDS_STRING201);
	progress.AddString("Copy Active Frame to Filter Input Buffer");

	mode=FILTER;

	FilterActive();

	mode=0;

	EndWaitCursor();
	display=OUT_BUFFER;
	RefreshScreen();
}

UINT CCombineDoc::FilterStack(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->FilterStack();
	return 0;
}

void CCombineDoc::FilterStack()
{
	buissy=true;
	progress.AddString(IDS_DFTSTACK);
	progress.m_cancel=false;
	progress.SetRange(0,top_used_frame-1);

	int f,count=0;

	dftReset();

	dft.InStack(
		0,0,
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	for(f=0; f<top_used_frame; f++)
	{
		progress.SetPos(f);
		if(progress.m_cancel)
		{
			break;
		}

		count++;

		dft.InStack(
			GetBuffer(used_frame[f]),
			1,
			GetBuffer(DFT_RIO_BUFFER),
			GetBuffer(DFT_GIO_BUFFER),
			GetBuffer(DFT_BIO_BUFFER)
			,bandw);
		FreeBuffer(used_frame[f]);
		UpdateBuffer(DFT_RIO_BUFFER);
		UpdateBuffer(DFT_GIO_BUFFER);
		UpdateBuffer(DFT_BIO_BUFFER);
	}

	dft.InStack(
		0,
		-1,
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dft.TransformIn(
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	CopyBuffer(DFT_RIO_BUFFER,DFT_RSTORE_BUFFER);
	CopyBuffer(DFT_GIO_BUFFER,DFT_GSTORE_BUFFER);
	CopyBuffer(DFT_BIO_BUFFER,DFT_BSTORE_BUFFER);
	dft.valid_store3=true;
	dft.valid_store1=false;

	dft.Filter(
		GetBuffer(DFT_FILTER_BUFFER),
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	FreeBuffer(DFT_FILTER_BUFFER);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dft.TransformOut(
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dft.Out3(
		GetBuffer(OUT_BUFFER),
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(DFT_RIO_BUFFER);
	FreeBuffer(DFT_GIO_BUFFER);
	FreeBuffer(DFT_BIO_BUFFER);

	progress.SetPos(0);

	if(refreshscreen)
	{
		progress.m_cancel=false;
		display=OUT_BUFFER;
		refreshscreen=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}

	mode=0;
}

void CCombineDoc::OnFilterSumofstack() 
{
	CString str1; str1.LoadString(IDS_STRING201);
	progress.AddString("Copy Sum of Stack to Filter Input Buffer");

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;
 	AfxBeginThread(FilterStack,this);
}

UINT CCombineDoc::FilterWeighted(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->FilterWeighted();
	return 0;
}

struct ws1_Params
{
	int index,step;
	int size;
	int *temp,*temp1,*rio,*gio,*bio,*wsum;
} ws1_params[32];

UINT CCombineDoc::ws1(LPVOID param)
{
	ws1_Params *pp=(ws1_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int size=pp->size;
	int *temp=pp->temp;
	int *temp1=pp->temp1;
	int *rio=pp->rio;
	int *gio=pp->gio;
	int *bio=pp->bio;
	int *wsum=pp->wsum;

	int i;

	for(i=index; i<size; i+=step)
	{
		((int*)temp)[i]=0;
		((int*)temp1)[i]=0;
		((int*)rio)[i]=0;
		((int*)gio)[i]=0;
		((int*)bio)[i]=0;
		((int*)wsum)[i]=0;
	}

	return 0;
}

struct ws2_Params
{
	int index,step;
	int w,h;
	PIXEL *frame;
	int *temp,*wt;
} ws2_params[32];

UINT CCombineDoc::ws2(LPVOID param)
{
	ws2_Params *pp=(ws2_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	int *temp=pp->temp;
	PIXEL *frame=pp->frame;
	int *wt=pp->wt;

	int i;
	int pr,pg,pb,qr,qg,qb;
	int d,dr,dg,db;

	PIXEL p,q;

	for(i=index; i<w*h; i+=step)
	{
		p=((PIXEL*)frame)[i];
		q=((PIXEL*)temp)[i];

		pr=(int)p.r; pg=(int)p.g; pb=(int)p.b;
		qr=(int)q.r; qg=(int)q.g; qb=(int)q.b;

		dr=pr-qr;
		dg=pg-qg;
		db=pb-qb;
		
		const int div=64;
		dr=dr*(dr-((int)p.r)/div);
		dg=dg*(dg-((int)p.g)/div);
		db=db*(db-((int)p.b)/div);

		const int ns=4;
		if(dr<ns) dr=0;
		if(dg<ns) dg=0;
		if(db<ns) db=0;

		d=dr+dg+db;

		wt[i]=d;
	}

	return 0;
}

struct ws3_Params
{
	int index,step;
	int size;
	int w,h;
	int *wt,*wsum;
} ws3_params[32];

UINT CCombineDoc::ws3(LPVOID param)
{
	ws3_Params *pp=(ws3_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	int *wt=pp->wt;

	int i,x,y;

	for(y=1+index; y<h-1; y+=step)
	{
		i=(y+1)*w;
		for(x=1; x<w-1; x++)
		{
			i=x+y*w;

			wt[i]=(
				wt[i-1-w]+wt[i-0-w]+wt[i+1-w]+
				wt[i-1-0]+wt[i-0-0]+wt[i+1-0]+
				wt[i-1+w]+wt[i-0+w]+wt[i+1+w])/9;
		}
	}

	return 0;
}

UINT CCombineDoc::ws4(LPVOID param)
{
	ws3_Params *pp=(ws3_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	int *wt=pp->wt;
	int *wsum=pp->wsum;

	int i;

	for(i=index; i<w*h; i+=step)
	{
		if(wt[i]==0) wt[i]=1;
		wsum[i]+=wt[i];
	}

	return 0;
}

void CCombineDoc::FilterWeighted()
{
	BeginThreads
	SetThreads(w,h)

	buissy=true;
	progress.AddString(IDS_DFTWEIGHTED);
	progress.m_cancel=false;
	progress.SetRange(0,top_used_frame-1);

	dftReset();

	int f,count=0;

	void *temp;
	void *temp1;
	void *rio;
	void *gio;
	void *bio;
	void *frame;
	int *wsum;
	int *wt;
	int *r1,*r2,*r3;
	r1=new int[w];
	r2=new int[w];
	r3=new int[w];

	// initialize lowpaSS FILTER
	mode=0;
	int saved_mode=dft.mode;
	float saved_displacement=dft.displacement;
	float saved_width=dft.width;
	float saved_order=dft.order;
	float saved_centre=dft.centre;
	float saved_cutoff=dft.cutoff;
	int saved_out_mode=dft.out_mode;

	dft.displacement=0.0f;
	dft.width=0.3f;
	ImmediateLowpass();

	// clear arrays
	temp=GetBuffer(TEMP_BUFFER+0);
	temp1=GetBuffer(TEMP_BUFFER+1);
	rio=GetBuffer(TEMP_BUFFER+2);
	gio=GetBuffer(TEMP_BUFFER+3);
	bio=GetBuffer(TEMP_BUFFER+4);
	wsum=(int *)GetBuffer(TEMP_BUFFER+5);
	for(obj=0; obj<objects; obj++)
	{
		ws1_params[obj].index=obj;
		ws1_params[obj].step=objects;
		ws1_params[obj].size=(w+2)*(h+2);
		ws1_params[obj].temp=(int *)temp;
		ws1_params[obj].temp1=(int *)temp1;
		ws1_params[obj].rio=(int *)rio;
		ws1_params[obj].gio=(int *)gio;
		ws1_params[obj].bio=(int *)bio;
		ws1_params[obj].wsum=(int *)wsum;
		StartThreads(ws1,ws1_params)
	}
	EndThreads

	UpdateBuffer(TEMP_BUFFER+0);
	UpdateBuffer(TEMP_BUFFER+1);
	UpdateBuffer(TEMP_BUFFER+2);
	UpdateBuffer(TEMP_BUFFER+3);
	UpdateBuffer(TEMP_BUFFER+4);
	UpdateBuffer(TEMP_BUFFER+5);

	for(f=0; f<top_used_frame; f++)
	{
		progress.SetPos(f);
		if(progress.m_cancel) break;

		// filter frame
		temp=GetBuffer(TEMP_BUFFER+0);
		frame=GetBuffer(used_frame[f]);
		filterframe((PIXEL*)frame,(PIXEL*)temp);

		wt=(int*)temp;
		wsum=(int*)GetBuffer(TEMP_BUFFER+5);
		for(obj=0; obj<objects; obj++)
		{
			ws2_params[obj].index=obj;
			ws2_params[obj].step=objects;
			ws2_params[obj].w=w;
			ws2_params[obj].h=h;
			ws2_params[obj].temp=(int *)temp;
			ws2_params[obj].frame=(PIXEL *)frame;
			ws2_params[obj].wt=(int *)wt;
			StartThreads(ws2,ws2_params)
		}
		EndThreads

		for(int a=0; a<5; a++)
		{
			for(obj=0; obj<objects; obj++)
			{
				ws3_params[obj].index=obj;
				ws3_params[obj].step=objects;
				ws3_params[obj].w=w;
				ws3_params[obj].h=h;
				ws3_params[obj].wt=(int *)wt;
				StartThreads(ws3,ws3_params)
			}
			EndThreads
		}

		for(obj=0; obj<objects; obj++)
		{
			ws3_params[obj].index=obj;
			ws3_params[obj].step=objects;
			ws3_params[obj].w=w;
			ws3_params[obj].h=h;
			ws3_params[obj].wt=(int *)wt;
			ws3_params[obj].wsum=(int *)wsum;
			StartThreads(ws4,ws3_params)
		}
		EndThreads

		UpdateBuffer(TEMP_BUFFER+5);

		// input frame
		rio=GetBuffer(TEMP_BUFFER+2);
		gio=GetBuffer(TEMP_BUFFER+3);
		bio=GetBuffer(TEMP_BUFFER+4);
		temp1=GetBuffer(TEMP_BUFFER+1);
		count++;
		dft.InWeighted(frame,1,rio,gio,bio,temp,temp1,bandw);
		UpdateBuffer(TEMP_BUFFER+0);
		UpdateBuffer(TEMP_BUFFER+1);
		UpdateBuffer(TEMP_BUFFER+2);
		UpdateBuffer(TEMP_BUFFER+3);
		UpdateBuffer(TEMP_BUFFER+4);
		FreeBuffer(used_frame[f]);
	}

	if(count<2) return;

	// finalize inputs
	rio=GetBuffer(TEMP_BUFFER+2);
	gio=GetBuffer(TEMP_BUFFER+3);
	bio=GetBuffer(TEMP_BUFFER+4);
	wsum=(int*)GetBuffer(TEMP_BUFFER+5);
	temp1=GetBuffer(TEMP_BUFFER+1);
	count++;
	dft.InWeighted(0,-1,rio,gio,bio,wsum,temp1,bandw);
	FreeBuffer(TEMP_BUFFER+5);
	UpdateBuffer(TEMP_BUFFER+1);

	dft.TransformIn(rio,gio,bio,bandw);

	dft.displacement=saved_displacement;
	dft.width=saved_width;
	dft.order=saved_order;
	dft.centre=saved_centre;
	dft.cutoff=saved_cutoff;
	dft.out_mode=saved_out_mode;
	switch(saved_mode)
	{
	case LOWPASS: ImmediateLowpass(); break;
	case HIGHPASS: ImmediateHighpass(); break;
	case BANDPASS: ImmediateBandpass(); break;
	case BANDSTOP: ImmediateBandstop(); break;
	case BUTTERWORTH: ImmediateButterworth(); break;
	}

	CopyBuffer(TEMP_BUFFER+2,DFT_RSTORE_BUFFER);
	CopyBuffer(TEMP_BUFFER+3,DFT_GSTORE_BUFFER);
	CopyBuffer(TEMP_BUFFER+4,DFT_BSTORE_BUFFER);

	dft.valid_store3=true;
	dft.valid_store1=false;

	dft.Filter(GetBuffer(DFT_FILTER_BUFFER),rio,gio,bio,bandw);
	FreeBuffer(DFT_FILTER_BUFFER);

	dft.TransformOut(rio,gio,bio,bandw);

	dft.Out3(GetBuffer(OUT_BUFFER),rio,gio,bio,bandw);
	UpdateBuffer(OUT_BUFFER);

	UpdateBuffer(TEMP_BUFFER+2);
	UpdateBuffer(TEMP_BUFFER+3);
	UpdateBuffer(TEMP_BUFFER+4);

	progress.SetPos(0);

	if(refreshscreen)
	{
		mode=0;
		progress.m_cancel=false;
		display=OUT_BUFFER;
		refreshscreen=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}
	delete [] r1;
	delete [] r2;
	delete [] r3;
}

void CCombineDoc::OnInputWeightedsumWC()
{
	mode=FILTER;

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;
 	AfxBeginThread(FilterWeightedWC,this);
}

UINT CCombineDoc::FilterWeightedWC(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->FilterWeightedWC();
	return 0;
}

UINT CCombineDoc::ws2a(LPVOID param)
{
	ws2_Params *pp=(ws2_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	int *temp=pp->temp;
	PIXEL *frame=pp->frame;
	int *wt=pp->wt;

	int i;
	int pr,pg,pb,qr,qg,qb;
	int d,dr,dg,db;

	PIXEL p,q;

	for(i=0; i<w*h; i++)
	{
		p=((PIXEL*)frame)[i];
		q=((PIXEL*)temp)[i];

		pr=(int)p.r; pg=(int)p.g; pb=(int)p.b;
		qr=(int)q.r; qg=(int)q.g; qb=(int)q.b;

		dr=pr-qr;
		dg=pg-qg;
		db=pb-qb;
		
		int div=32;
		dr=dr*(dr-((int)p.r)/div);
		dg=dg*(dg-((int)p.g)/div);
		db=db*(db-((int)p.b)/div);

		int ns=5;
		if(dr<ns) dr=0;
		if(dg<ns) dg=0;
		if(db<ns) db=0;

		d=dr+dg+db;

		wt[i]=d;
	}

	return 0;
}

UINT CCombineDoc::ws4a(LPVOID param)
{
	ws3_Params *pp=(ws3_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int w=pp->w;
	int h=pp->h;
	int *wt=pp->wt;
	int *wsum=pp->wsum;

	int i,d;

	for(i=index; i<w*h; i+=step)
	{
		d=wt[i];
		if((wsum[i]==0) && (d==0)) wt[i]=1;
		if((wsum[i]==1) && (d>1))
		{
			wsum[i]=d;
			wt[i]=-d;
		}
		else wsum[i]+=wt[i];
	}

	return 0;
}

void CCombineDoc::FilterWeightedWC() // With Correction
{
	BeginThreads
	SetThreads(w,h)

	buissy=true;
	progress.AddString(IDS_WEIGHTEDWC);
	progress.m_cancel=false;
	progress.SetRange(0,top_used_frame-1);

	dftReset();

	int f,count=0;

	void *temp;
	void *temp1;
	void *rio;
	void *gio;
	void *bio;
	void *frame;
	int *wsum;
	int *wt;

	int *r1,*r2,*r3;
	r1=new int[w];
	r2=new int[w];
	r3=new int[w];

	// initialize lowpaSS FILTER
	mode=0;
	int saved_mode=dft.mode;
	float saved_displacement=dft.displacement;
	float saved_width=dft.width;
	float saved_order=dft.order;
	float saved_centre=dft.centre;
	float saved_cutoff=dft.cutoff;
	int saved_out_mode=dft.out_mode;

	dft.displacement=0.0f;
	dft.width=1.0f;
	ImmediateLowpass();

	// clear arrays
	temp=GetBuffer(TEMP_BUFFER+0);
	temp1=GetBuffer(TEMP_BUFFER+1);
	rio=GetBuffer(TEMP_BUFFER+2);
	gio=GetBuffer(TEMP_BUFFER+3);
	bio=GetBuffer(TEMP_BUFFER+4);
	wsum=(int *)GetBuffer(TEMP_BUFFER+5);
	for(obj=0; obj<objects; obj++)
	{
		ws1_params[obj].index=obj;
		ws1_params[obj].step=objects;
		ws1_params[obj].size=(w+2)*(h+2);
		ws1_params[obj].temp=(int *)temp;
		ws1_params[obj].temp1=(int *)temp1;
		ws1_params[obj].rio=(int *)rio;
		ws1_params[obj].gio=(int *)gio;
		ws1_params[obj].bio=(int *)bio;
		ws1_params[obj].wsum=(int *)wsum;
		StartThreads(ws1,ws1_params)
	}
	EndThreads

	UpdateBuffer(TEMP_BUFFER+0);
	UpdateBuffer(TEMP_BUFFER+1);
	UpdateBuffer(TEMP_BUFFER+2);
	UpdateBuffer(TEMP_BUFFER+3);
	UpdateBuffer(TEMP_BUFFER+4);
	UpdateBuffer(TEMP_BUFFER+5);

	for(f=top_used_frame-1; f>=0; f--)
	{
		progress.SetPos(top_used_frame-f-1);
		if(progress.m_cancel) break;

		// filter frame
		temp=GetBuffer(TEMP_BUFFER+0);
		frame=GetBuffer(used_frame[f]);
		filterframe((PIXEL*)frame,(PIXEL*)temp);

		wt=(int*)temp;
		wsum=(int*)GetBuffer(TEMP_BUFFER+5);
		for(obj=0; obj<objects; obj++)
		{
			ws2_params[obj].index=obj;
			ws2_params[obj].step=objects;
			ws2_params[obj].w=w;
			ws2_params[obj].h=h;
			ws2_params[obj].temp=(int *)temp;
			ws2_params[obj].frame=(PIXEL *)frame;
			ws2_params[obj].wt=(int *)wt;
			StartThreads(ws2a,ws2_params)
		}
		EndThreads

		for(int a=0; a<25; a++)
		{
			for(obj=0; obj<objects; obj++)
			{
				ws3_params[obj].index=obj;
				ws3_params[obj].step=objects;
				ws3_params[obj].w=w;
				ws3_params[obj].h=h;
				ws3_params[obj].wt=(int *)wt;
				StartThreads(ws3,ws3_params)
			}
			EndThreads
		}

		for(obj=0; obj<objects; obj++)
		{
			ws3_params[obj].index=obj;
			ws3_params[obj].step=objects;
			ws3_params[obj].w=w;
			ws3_params[obj].h=h;
			ws3_params[obj].wt=(int *)wt;
			ws3_params[obj].wsum=(int *)wsum;
			StartThreads(ws4a,ws3_params)
		}
		EndThreads

		UpdateBuffer(TEMP_BUFFER+5);

		// input frame
		rio=GetBuffer(TEMP_BUFFER+2);
		gio=GetBuffer(TEMP_BUFFER+3);
		bio=GetBuffer(TEMP_BUFFER+4);
		temp1=GetBuffer(TEMP_BUFFER+1);
		count++;
		dft.InWeighted(frame,1,rio,gio,bio,temp,temp1,bandw);
		UpdateBuffer(TEMP_BUFFER+0);
		UpdateBuffer(TEMP_BUFFER+1);
		UpdateBuffer(TEMP_BUFFER+2);
		UpdateBuffer(TEMP_BUFFER+3);
		UpdateBuffer(TEMP_BUFFER+4);
		FreeBuffer(used_frame[f]);
	}

	if(count<2) return;

	// finalize inputs
	rio=GetBuffer(TEMP_BUFFER+2);
	gio=GetBuffer(TEMP_BUFFER+3);
	bio=GetBuffer(TEMP_BUFFER+4);
	wsum=(int*)GetBuffer(TEMP_BUFFER+5);
	temp1=GetBuffer(TEMP_BUFFER+1);
	dft.InWeighted(0,-1,rio,gio,bio,wsum,temp1,bandw);
	FreeBuffer(TEMP_BUFFER+5);
	UpdateBuffer(TEMP_BUFFER+1);

	dft.TransformIn(rio,gio,bio,bandw);

	dft.displacement=saved_displacement;
	dft.width=saved_width;
	dft.order=saved_order;
	dft.centre=saved_centre;
	dft.cutoff=saved_cutoff;
	dft.out_mode=saved_out_mode;
	switch(saved_mode)
	{
	case LOWPASS: ImmediateLowpass(); break;
	case HIGHPASS: ImmediateHighpass(); break;
	case BANDPASS: ImmediateBandpass(); break;
	case BANDSTOP: ImmediateBandstop(); break;
	case BUTTERWORTH: ImmediateButterworth(); break;
	}

	CopyBuffer(TEMP_BUFFER+2,DFT_RSTORE_BUFFER);
	CopyBuffer(TEMP_BUFFER+3,DFT_GSTORE_BUFFER);
	CopyBuffer(TEMP_BUFFER+4,DFT_BSTORE_BUFFER);

	dft.valid_store3=true;
	dft.valid_store1=false;

	dft.Filter(GetBuffer(DFT_FILTER_BUFFER),rio,gio,bio,bandw);
	FreeBuffer(DFT_FILTER_BUFFER);

	dft.TransformOut(rio,gio,bio,bandw);

	dft.Out3(GetBuffer(OUT_BUFFER),rio,gio,bio,bandw);
	UpdateBuffer(OUT_BUFFER);

	UpdateBuffer(TEMP_BUFFER+2);
	UpdateBuffer(TEMP_BUFFER+3);
	UpdateBuffer(TEMP_BUFFER+4);

	progress.SetPos(0);

	if(refreshscreen)
	{
		mode=0;
		progress.m_cancel=false;
		display=OUT_BUFFER;
		refreshscreen=false;
		buissy=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
		RefreshScreen();
	}
	delete [] r1;
	delete [] r2;
	delete [] r3;
}

void CCombineDoc::OnFilterInputWeightedsum() 
{
	progress.AddString(270);

	mode=FILTER;

	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	refreshscreen=true;
 	AfxBeginThread(FilterWeighted,this);
}

void CCombineDoc::FilterDM()
{
	dftReset();

	dft.In1(GetBuffer(DEPTH_BUFFER),GetBuffer(DFT_RIO_BUFFER));
	FreeBuffer(DEPTH_BUFFER);
	UpdateBuffer(DFT_RIO_BUFFER);

	dft.TransformIn1(GetBuffer(DFT_RIO_BUFFER));
	UpdateBuffer(DFT_RIO_BUFFER);

	CopyBuffer(DFT_RIO_BUFFER,DFT_RSTORE_BUFFER);
	dft.valid_store1=true;
	dft.valid_store3=false;

	dft.Filter(
		GetBuffer(DFT_FILTER_BUFFER),
		GetBuffer(DFT_RIO_BUFFER),
		GetBuffer(DFT_GIO_BUFFER),
		GetBuffer(DFT_BIO_BUFFER)
		,bandw);
	FreeBuffer(DFT_FILTER_BUFFER);
	UpdateBuffer(DFT_RIO_BUFFER);
	UpdateBuffer(DFT_GIO_BUFFER);
	UpdateBuffer(DFT_BIO_BUFFER);

	dft.TransformOut1(GetBuffer(DFT_RIO_BUFFER));
	UpdateBuffer(DFT_RIO_BUFFER);

	dft.Out1(GetBuffer(OUT_BUFFER),255*top_frame,GetBuffer(DFT_RIO_BUFFER));
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(DFT_RIO_BUFFER);
}

void CCombineDoc::OnFilterMakexfromdepthmap() 
{
	BeginWaitCursor();

	progress.AddString(271);

	mode=FILTER;

	FilterDM();

	mode=0;
	EndWaitCursor();
	display=OUT_BUFFER;
	RefreshScreen();
}

void CCombineDoc::OnFilterRedisplaylastoutput() 
{
	BeginWaitCursor();

	progress.AddString(272);

	mode=FILTER;

	if(dft.valid_picture3)
	{
		dft.Out3(
			GetBuffer(OUT_BUFFER),
			GetBuffer(DFT_RIO_BUFFER),
			GetBuffer(DFT_GIO_BUFFER),
			GetBuffer(DFT_BIO_BUFFER)
			,bandw);
		UpdateBuffer(OUT_BUFFER);
		FreeBuffer(DFT_RIO_BUFFER);
		FreeBuffer(DFT_GIO_BUFFER);
		FreeBuffer(DFT_BIO_BUFFER);
		EndWaitCursor();
		display=OUT_BUFFER;
		mode=0;
		RefreshScreen();
		return;
	}

	if(dft.valid_picture1)
	{
		dft.Out1(GetBuffer(OUT_BUFFER),255*top_frame,GetBuffer(DFT_RIO_BUFFER));
		UpdateBuffer(OUT_BUFFER);
		FreeBuffer(DFT_RIO_BUFFER);
		EndWaitCursor();
		display=OUT_BUFFER;
		mode=0;
		RefreshScreen();
		return;
	}

	if(dft.valid_store3)
	{
		CopyBuffer(DFT_RSTORE_BUFFER,DFT_RIO_BUFFER);
		CopyBuffer(DFT_GSTORE_BUFFER,DFT_GIO_BUFFER);
		CopyBuffer(DFT_BSTORE_BUFFER,DFT_BIO_BUFFER);

		dft.Filter(
			GetBuffer(DFT_FILTER_BUFFER),
			GetBuffer(DFT_RIO_BUFFER),
			GetBuffer(DFT_GIO_BUFFER),
			GetBuffer(DFT_BIO_BUFFER)
			,bandw);
		FreeBuffer(DFT_FILTER_BUFFER);
		UpdateBuffer(DFT_RIO_BUFFER);
		UpdateBuffer(DFT_GIO_BUFFER);
		UpdateBuffer(DFT_BIO_BUFFER);

		dft.TransformOut(
			GetBuffer(DFT_RIO_BUFFER),
			GetBuffer(DFT_GIO_BUFFER),
			GetBuffer(DFT_BIO_BUFFER)
			,bandw);
		UpdateBuffer(DFT_RIO_BUFFER);
		UpdateBuffer(DFT_GIO_BUFFER);
		UpdateBuffer(DFT_BIO_BUFFER);

		dft.Out3(
			GetBuffer(OUT_BUFFER),
			GetBuffer(DFT_RIO_BUFFER),
			GetBuffer(DFT_GIO_BUFFER),
			GetBuffer(DFT_BIO_BUFFER)
			,bandw);
		UpdateBuffer(OUT_BUFFER);
		FreeBuffer(DFT_RIO_BUFFER);
		FreeBuffer(DFT_GIO_BUFFER);
		FreeBuffer(DFT_BIO_BUFFER);

		EndWaitCursor();
		display=OUT_BUFFER;
		mode=0;
		RefreshScreen();
		return;
	}

	if(dft.valid_store1)
	{
		CopyBuffer(DFT_RSTORE_BUFFER,DFT_RIO_BUFFER);

		dft.Filter(
			GetBuffer(DFT_FILTER_BUFFER),
			GetBuffer(DFT_RIO_BUFFER),
			GetBuffer(DFT_GIO_BUFFER),
			GetBuffer(DFT_BIO_BUFFER)
			,bandw);
		FreeBuffer(DFT_FILTER_BUFFER);
		UpdateBuffer(DFT_RIO_BUFFER);
		UpdateBuffer(DFT_GIO_BUFFER);
		UpdateBuffer(DFT_BIO_BUFFER);

		dft.TransformOut1(GetBuffer(DFT_RIO_BUFFER));
		UpdateBuffer(DFT_RIO_BUFFER);

		dft.Out1(GetBuffer(OUT_BUFFER),255*top_frame,GetBuffer(DFT_RIO_BUFFER));
		UpdateBuffer(OUT_BUFFER);
		FreeBuffer(DFT_RIO_BUFFER);

		EndWaitCursor();
		display=OUT_BUFFER;
		RefreshScreen();
		mode=0;
		return;
	}
}

void CCombineDoc::OnFilterFreememory() 
{
	progress.AddString(273);
	filter_param=0;
	dft.Destroy();
}

void CCombineDoc::getvalue()
{
	CGetvalue dd;
	switch(filter_param)
	{
	case CENTRE:
		dd.m_string.LoadString(IDS_STRING274);
		dd.m_value=(int)(dft.centre*1000.0f);
		break;
	case CUTOFF:
		dd.m_string.LoadString(IDS_STRING275);
		dd.m_value=(int)(dft.cutoff*1000.0f);
		if(mode==BUTTERWORTH) dd.m_value=1000-(int)(dft.cutoff*1000.0f);
		break;
	case ORDER:
		dd.m_string.LoadString(IDS_STRING276);
		dd.m_value=(int)dft.order;
		break;
	case DISPLACEMENT:
		dd.m_string.LoadString(IDS_STRING277);
		if(mode==BANDPASS) dd.m_value=(int)(1000.0f-dft.displacement*1000.0f);
		else dd.m_value=(int)(dft.displacement*1000.0f);
		if(mode==BANDSTOP) dd.m_value=(int)(1000.0f-dft.displacement*2000.0f);
		break;
	case WIDTH:
		dd.m_string.LoadString(IDS_STRING278);
		dd.m_value=(int)(dft.width*1000.0f);
		break;
	}
	if(mode==FBRIGHTNESS) dd.m_value=(int)(dft.brightness*100.0f);
	if(mode==FTHRESHOLD) dd.m_value=(int)dft.threshold;
	int button;
	button=(int)dd.DoModal();
	if(button==IDCANCEL) return;
	if(mode==FBRIGHTNESS) {dft.brightness=(float)dd.m_value/100.0f; editbrightness(); return;}
	if(mode==FTHRESHOLD)  {dft.threshold=(float)dd.m_value; editthreshold(); return;}
	switch(filter_param)
	{
	case CENTRE:
		dft.centre=(float)(dd.m_value/1000.0f);
		break;
	case CUTOFF:
		dft.cutoff=(float)(dd.m_value/1000.0f);
		if(mode==BUTTERWORTH) dft.cutoff=1.0f-(float)(dd.m_value/1000.0f);
		break;
	case ORDER:
		dft.order=(float)(dd.m_value);
		break;
	case DISPLACEMENT:
		if(mode==BANDPASS) dft.displacement=1.0f-(float)dd.m_value/1000.0f;
		else dft.displacement=(float)dd.m_value/1000.0f;
		if(mode==BANDSTOP) dft.displacement=0.5f-(float)dd.m_value/2000.0f;
		break;
	case WIDTH:
		dft.width=(float)(dd.m_value/1000.0f);
		break;
	}
	rework_filter();
}

void CCombineDoc::getvalue(int key)
{
	getvalue();
}

void CCombineDoc::filterreport()
{
	CString s;
	progress.AddString(279);
	switch(dft.out_mode)
	{
	case OUT_NORMAL:
		progress.AddString(IDS_STRING280);
		break;
	case OUT_BRIGHT:
		str1.LoadString(IDS_STRING281);
		s.Format(L"(%d)",(int)(dft.brightness*100.0f));
		progress.AddString(str1+s);
		break;
	case OUT_THRESHOLD:
		str1.LoadString(IDS_STRING282);
		s.Format(L"(%d)",(int)(dft.threshold*100.0f));
		progress.AddString(str1+s);
		break;
	}
	switch(dft.mode)
	{
	case LOWPASS:
		str1.LoadString(IDS_STRING283);
		s.Format(str1,
			(int)(dft.width*1000.0f),
			(int)(dft.displacement*1000.0f));
		progress.AddString(s);
		break;
	case HIGHPASS:
		str1.LoadString(IDS_STRING284);
		s.Format(str1,
			(int)(dft.width*1000.0f),
			(int)(dft.displacement*1000.0f));
		progress.AddString(s);
		break;
	case BANDPASS:
		str1.LoadString(IDS_STRING285);
		s.Format(str1,
			(int)(dft.width*1000.0f),
			(int)((1.0f-dft.displacement)*1000.0f));
		progress.AddString(s);
		break;
	case BANDSTOP:
		str1.LoadString(IDS_STRING286);
		s.Format(str1,
			(int)(dft.width*1000.0f),
			(int)(2.0f*(0.5f-dft.displacement)*1000.0f),
			(int)(dft.centre*1000.0f));
		progress.AddString(s);
		break;
	case BUTTERWORTH:
		str1.LoadString(IDS_STRING287);
		s.Format(287,
			(int)((1.0f-dft.cutoff)*1000.0f),
			(int)(dft.displacement*1000.0f),
			(int)dft.order);
		progress.AddString(s);
		break;
	}
}
