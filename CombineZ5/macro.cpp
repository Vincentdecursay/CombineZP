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
#include "macro1.h"
#include "choosemacro.h"
#include "values.h"

struct CZMacro macro[10];

/*
IDS_SAVEALL
IDS_RELOADALL
IDS_REVERSE
IDS_BALANCE
IDS_AUTOALIGN1
IDS_AUTOALIGN2
IDS_RESIZEALL
IDS_GROUP
IDS_DETAIL
IDS_GAPS
IDS_ISLANDS
IDS_STEPS
IDS_INTERPOLATE
IDS_NEWFROMPICTURE
IDS_NEWFROMOUT
IDS_REPLACEFRAME
IDS_RGB
IDS_CONTRAST
IDS_DFTACTIVE
IDS_DFTSTACK
IDS_DFTDEPTH
IDS_LOWPASS
IDS_HIGHPASS
IDS_FINISH
IDS_DFTALLFRAMES
IDS_BANDPASS
IDS_BUTTERWORTH
IDS_BANDSTOP
IDS_FANDRDEPTHMAP
IDS_DFTOUTNORMAL
IDS_DFTOUTBRIGHTEN
IDS_SAVEDEPTH
IDS_LOADDEPTH
IDS_GOTOFRAME
IDS_MACRO1
IDS_MACRO2
IDS_MACRO3
IDS_MACRO4
IDS_MACRO5
IDS_MACRO6
IDS_MACRO7
IDS_MACRO8
IDS_MACRO9
IDS_MACRO10
IDS_BANDW
IDS_SKEW
IDS_FIX
IDS_UNFIX
IDS_SELECTALL
IDS_PATCHWORK
IDS_WEIGHTED
IDS_LIGHTDARK
IDS_LOCALBALANCE
IDS_REMOVE
IDS_ACTIVE2
IDS_CORRECT_BRIGHTNESS
IDS_ADD_RECTANGLES
IDS_BLEND_RECTANGLES
*/

char *short_name[]=
{
	"SALL","LALL","REV","BAL","AL1","AL2",
	"SZALL","GRP","D","G","I",
	"S","INTERP","NEWP","NEWO","RPLF","RGB",
	"CONT","FACT","FSTK","FDPTH","LPF",
	"HPF","FIN","FALL","BPF",
	"BUTF","BSF","FRDPTH","FNOUT",
	"FBOUT","SD","LD","GTF",
	0,0,0,0,0,0,0,0,0,0,"BAW","SKEW","FIX",
	"UFIX","A","PCH","FWS","LTDK","LBAL",
	"REM","ACTIVE2","CORBR","ADD","BLEND"
};

struct CZCommand default_command[]=
{
	{IDS_SAVEALL,-1,-1,-1,0},
	{IDS_RELOADALL,-1,-1,-1,0},
	{IDS_REVERSE,-1,-1,-1,0},
	{IDS_BALANCE,-1,-1,-1,0},
	{IDS_AUTOALIGN1,-1,-1,-1,0},
	{IDS_AUTOALIGN2,-1,-1,-1,0},
	{IDS_RESIZEALL,10000,-1,-1,0},
	{IDS_GROUP,2,-1,-1,0},
	{IDS_DETAIL,75,-1,-1,0},
	{IDS_GAPS,-1,-1,-1,0},
	{IDS_ISLANDS,5,-1,-1,0},
	{IDS_STEPS,-1,-1,-1,0},
	{IDS_INTERPOLATE,-1,-1,-1,0},
	{IDS_NEWFROMPICTURE,-1,-1,-1,0},
	{IDS_NEWFROMOUT,-1,-1,-1,0},
	{IDS_REPLACEFRAME,-1,-1,-1,0},
	{IDS_RGB,1000,1000,1000,0},
	{IDS_CONTRAST,10,-1,-1,0},
	{IDS_DFTACTIVE,-1,-1,-1,0},
	{IDS_DFTSTACK,-1,-1,-1,0},
	{IDS_DFTDEPTH,-1,-1,-1,0},
	{IDS_LOWPASS,150,0,-1,0},
	{IDS_HIGHPASS,1000,200,-1,0},
	{IDS_FINISH,-1,-1,-1,0},
	{IDS_DFTALLFRAMES,-1,-1,-1,0},
	{IDS_BANDPASS,1000,1000,-1,0},
	{IDS_BUTTERWORTH,500,0,2,0},
	{IDS_BANDSTOP,1000,1000,500,0},
	{IDS_FANDRDEPTHMAP,-1,-1,-1,0},
	{IDS_DFTOUTNORMAL,-1,-1,-1,0},
	{IDS_DFTOUTBRIGHTEN,100,-1,-1,0},
	{IDS_SAVEDEPTH,-1,-1,-1,0},
	{IDS_LOADDEPTH,-1,-1,-1,0},
	{IDS_GOTOFRAME,0,-1,-1,0},
	{IDS_MACRO1,1,0,1,0},
	{IDS_MACRO2,1,0,1,0},
	{IDS_MACRO3,1,0,1,0},
	{IDS_MACRO4,1,0,1,0},
	{IDS_MACRO5,1,0,1,0},
	{IDS_MACRO6,1,0,1,0},
	{IDS_MACRO7,1,0,1,0},
	{IDS_MACRO8,1,0,1,0},
	{IDS_MACRO9,1,0,1,0},
	{IDS_MACRO10,1,0,1,0},
	{IDS_BANDW,-1,-1,-1,0},
	{IDS_SKEW,10050,-1,-1,0},
	{IDS_FIX,-1,-1,-1,0},
	{IDS_UNFIX,-1,-1,-1,0},
	{IDS_SELECTALL,-1,-1,-1,0},
	{IDS_PATCHWORK,4,10,-1,0},
	{IDS_DFTWEIGHTED,-1,-1,-1,0},
	{IDS_LIGHTDARK,140,140,-1,0},
	{IDS_LOCALBALANCE,-1,-1,-1,0},
	{IDS_REMOVE,-1,-1,-1,0},
	{IDS_ACTIVE2,-1,-1,-1,0},
	{IDS_CORRECT_BRIGHTNESS,100,0,0,0},
	{IDS_ADD_RECTANGLES,1000,-1,-1,0},
	{IDS_BLEND_RECTANGLES,1000,-1,-1,0}
// Update last_command in InitializeMacros() when adding new commands
};

void CCombineDoc::OnMacroEditmacro() 
{
	int button;

	CMacro mac;

//	InitializeMacros();

	for(;;)
	{
		CChooseMacro choose_mac;
		button=choose_mac.DoModal()-100;
		if(button<0) return;

		mac.this_macro=macro[button];
		mac.m_name=macro[button].name;

		mac.first_command=IDS_SAVEALL;
		mac.last_command=IDS_BLEND_RECTANGLES;

		if(mac.DoModal()==IDCANCEL) continue;
		macro[button]=mac.this_macro;
		macro[button].name=mac.m_name;

		InitializeMacros();
		savedefaultmacros();
	}
}

void CCombineDoc::OnMacroRestoredefaults() 
{
	OnMacroClearallmacros();
	defaultmac01();
	defaultmac02();
	defaultmac03();
	defaultmac04();
	defaultmac05();
	defaultmac06();
	defaultmac07();
	defaultmac08();
	defaultmac09();
	defaultmac10();
	InitializeMacros();
	savedefaultmacros();
}

void CCombineDoc::InitializeMacros()
{
	static bool started=false;
	int i;

	last_command=IDS_BLEND_RECTANGLES;

	if(!started)
	{
		for(i=0; i<10; i++)
			Clear_macro_commands(i);
		if(!loaddefaultmacros())
		{
			defaultmac01();
			defaultmac02();
			defaultmac03();
			defaultmac04();
			defaultmac05();
			defaultmac06();
			defaultmac07();
			defaultmac08();
			defaultmac09();
			defaultmac10();
//			loaddefaultmacros();
			savedefaultmacros();
		}
		pause_enabled=true;
		started=true;
	}

	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO1,MF_BYCOMMAND,ID_MACRO1,macro[0].name);
	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO2,MF_BYCOMMAND,ID_MACRO2,macro[1].name);
	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO3,MF_BYCOMMAND,ID_MACRO3,macro[2].name);
	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO4,MF_BYCOMMAND,ID_MACRO4,macro[3].name);
	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO5,MF_BYCOMMAND,ID_MACRO5,macro[4].name);
	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO6,MF_BYCOMMAND,ID_MACRO6,macro[5].name);
	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO7,MF_BYCOMMAND,ID_MACRO7,macro[6].name);
	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO8,MF_BYCOMMAND,ID_MACRO8,macro[7].name);
	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO9,MF_BYCOMMAND,ID_MACRO9,macro[8].name);
	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO10,MF_BYCOMMAND,ID_MACRO10,macro[9].name);
}

void CCombineDoc::Clear_macro_commands(int m)
{
	int i;
	for(i=0; i<24; i++)
	{
		macro[m].cmd[i].n=-1;
		macro[m].cmd[i].p1=-1;
		macro[m].cmd[i].p2=-1;
		macro[m].cmd[i].p3=-1;
		macro[m].cmd[i].f=0;
	}
}

void CCombineDoc::defaultmac01()
{
	Clear_macro_commands(0);
	macro[0].name="Do Stack";

	macro[0].cmd[0].n=IDS_SELECTALL;

	macro[0].cmd[1].n=IDS_BALANCE;

	macro[0].cmd[2].n=IDS_AUTOALIGN2;

	macro[0].cmd[3].n=IDS_DETAIL;
	macro[0].cmd[3].p1=25;

	macro[0].cmd[4].n=IDS_ISLANDS;
	macro[0].cmd[4].p1=5;

	macro[0].cmd[5].n=IDS_GAPS;

	macro[0].cmd[6].n=IDS_DFTOUTNORMAL;

	macro[0].cmd[7].n=IDS_LOWPASS;
	macro[0].cmd[7].p1=250;
	macro[0].cmd[7].p2=0;

	macro[0].cmd[8].n=IDS_FANDRDEPTHMAP;

	macro[0].cmd[9].n=IDS_INTERPOLATE;

	macro[0].cmd[10].n=IDS_NEWFROMOUT;

	macro[0].cmd[11].n=IDS_GOTOFRAME;
	macro[0].cmd[11].p1=999;

	macro[0].cmd[12].n=IDS_HIGHPASS;
	macro[0].cmd[12].p1=1000;
	macro[0].cmd[12].p2=750;

	macro[0].cmd[13].n=IDS_DFTACTIVE;

	macro[0].cmd[14].n=IDS_REPLACEFRAME;

	macro[0].cmd[15].n=IDS_CONTRAST;
	macro[0].cmd[15].p1=5;

	macro[0].cmd[16].n=IDS_REPLACEFRAME;
}

void CCombineDoc::defaultmac02()
{
	Clear_macro_commands(1);
	macro[1].name="Do Average and Filter";

	macro[1].cmd[0].n=IDS_SELECTALL;

	macro[1].cmd[1].n=IDS_BALANCE;

	macro[1].cmd[2].n=IDS_AUTOALIGN2;

	macro[1].cmd[3].n=IDS_BANDPASS;
	macro[1].cmd[3].p1=250;
	macro[1].cmd[3].p2=200;

	macro[1].cmd[4].n=IDS_DFTWEIGHTED;
}

void CCombineDoc::defaultmac03()
{
	Clear_macro_commands(2);
	macro[2].name="Balance and Align";

	macro[2].cmd[0].n=IDS_SELECTALL;

	macro[2].cmd[1].n=IDS_BALANCE;

	macro[2].cmd[2].n=IDS_AUTOALIGN2;
}

void CCombineDoc::defaultmac04()
{
	Clear_macro_commands(3);
	macro[3].name="Stack Only";

	macro[3].cmd[0].n=IDS_DETAIL;
	macro[3].cmd[0].p1=25;
	macro[3].cmd[0].f=MAC_SUBP1|MAC_TONAME;

	macro[3].cmd[1].n=IDS_ISLANDS;
	macro[3].cmd[1].p1=5;

	macro[3].cmd[2].n=IDS_GAPS;

	macro[3].cmd[3].n=IDS_DFTOUTNORMAL;

	macro[3].cmd[4].n=IDS_LOWPASS;
	macro[3].cmd[4].p1=250;
	macro[3].cmd[4].p2=0;

	macro[3].cmd[5].n=IDS_FANDRDEPTHMAP;

	macro[3].cmd[6].n=IDS_INTERPOLATE;

	macro[3].cmd[7].n=IDS_NEWFROMOUT;

	macro[3].cmd[8].n=IDS_GOTOFRAME;
	macro[3].cmd[8].p1=999;
}

void CCombineDoc::defaultmac05()
{
	Clear_macro_commands(4);
	macro[4].name="Filter Only";

	macro[4].cmd[0].n=IDS_HIGHPASS;
	macro[4].cmd[0].p1=1000;
	macro[4].cmd[0].p2=40;
	macro[4].cmd[0].f=MAC_SUBP2|MAC_TONAME;

	macro[4].cmd[1].n=IDS_DFTSTACK;

	macro[4].cmd[2].n=IDS_FINISH;

	macro[4].cmd[3].n=IDS_NEWFROMOUT;

	macro[4].cmd[4].n=IDS_GOTOFRAME;
	macro[4].cmd[4].p1=999;
}

void CCombineDoc::defaultmac06()
{
	Clear_macro_commands(5);
	macro[5].name="Stack Loop";

	macro[5].cmd[0].n=IDS_MACRO4;
	macro[5].cmd[0].p1=20;
	macro[5].cmd[0].p2=5;
	macro[5].cmd[0].p3=50;
}

void CCombineDoc::defaultmac07()
{
	Clear_macro_commands(6);
	macro[6].name="Filter Loop";

	macro[6].cmd[0].n=IDS_MACRO5;
	macro[6].cmd[0].p1=40;
	macro[6].cmd[0].p2=30;
	macro[6].cmd[0].p3=250;
}

void CCombineDoc::defaultmac08()
{
	Clear_macro_commands(7);
	macro[7].name="Noise Filter (All Frames)";

	macro[7].cmd[0].n=IDS_DFTOUTNORMAL;

	macro[7].cmd[1].n=IDS_LOWPASS;
	macro[7].cmd[1].p1=50;
	macro[7].cmd[1].p2=400;

	macro[7].cmd[2].n=IDS_DFTALLFRAMES;

	macro[7].cmd[3].n=IDS_FINISH;
}

void CCombineDoc::defaultmac09()
{
	Clear_macro_commands(8);
	macro[8].name="Skew (Nested)";

	macro[8].cmd[0].n=IDS_RELOADALL;

	macro[8].cmd[1].n=IDS_SKEW;
	macro[8].cmd[1].p1=10100;
	macro[8].cmd[1].f=MAC_SUBP1|MAC_TONAME;

	macro[8].cmd[2].n=IDS_MACRO5;
	macro[8].cmd[2].p1=100;
	macro[8].cmd[2].p2=99999;
	macro[8].cmd[2].p3=99999;
}

void CCombineDoc::defaultmac10()
{
	Clear_macro_commands(9);
	macro[9].name="Skew Loop";

	macro[9].cmd[0].n=IDS_SAVEALL;

	macro[9].cmd[1].n=IDS_MACRO9;
	macro[9].cmd[1].p1=9900;
	macro[9].cmd[1].p2=20;
	macro[9].cmd[1].p3=10100;

	macro[9].cmd[2].n=IDS_RELOADALL;

	macro[9].cmd[3].n=IDS_GOTOFRAME;
	macro[9].cmd[3].p1=999;
}

UINT CCombineDoc::DoMacroT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->DoMacro();
	return 0;
}

void CCombineDoc::DoMacro()
{
	static int nest=0;

	static int value=0;
	int saved_value;

	CRect full(0,0,w-1,h-1);

	static CString newname;
	CString saved_newname;
	if(nest==0) newname="";

	if(nest>4)
	{
		MessageBox(0,"Macros Nested too deeply, the limit is 5","MACRO NESTING LIMIT",MB_OK);
		progress.AddString("*** Nesting Limit Surpassed ********************");
		progress.m_cancel=true;
		return;
	}

	if(nest==0)
	{
		progress.m_cancel=false;
	}

	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	int i,j,start,inc,end,p1,p2,p3;
	int saved_af=active_frame;
	int saved_mac;
	int button;
	CString s;
	CValues params;
	if((nest==0) && !(silent)) progress.Maximize();
	s.Format("***  Executing %s Macro  *** (Main Window Hidden)",macro[mac].name);
	progress.AddString(s);
	if(nest)
	{
		s.Format("*** Value Passed(%d) ***",value);
		progress.AddString(s);
	}

	CZMacro this_macro;
	this_macro=macro[mac];

	macro_mode=true;
	static bool redraw_pic=true;
	if(nest==0) redraw_pic=false;

	for(i=0; i<24; i++)
	{
		buissy=true;

		if(progress.m_cancel)
		{
			if(nest==0) progress.m_cancel=false;
			break;
		}
	
		if(this_macro.cmd[i].n==-1) continue;

		if((this_macro.cmd[i].f&MAC_SUBP1) && nest) p1=value;
		else p1=this_macro.cmd[i].p1;
		if((this_macro.cmd[i].f&MAC_SUBP2) && nest) p2=value;
		else p2=this_macro.cmd[i].p2;
		if((this_macro.cmd[i].f&MAC_SUBP3) && nest) p3=value;
		else p3=this_macro.cmd[i].p3;

		if((this_macro.cmd[i].n==IDS_GOTOFRAME) && nest && ((p1>=top_frame) || (buffer[p1].hidden)) && (this_macro.cmd[i].f&MAC_SUBP1) && (i==0))
			break;

		if((this_macro.cmd[i].f&MAC_PAUSE) && (pause_enabled))
		{
			progress.AddString("*** Paused ***");
			params.this_macro=mac;
			params.command=i;
			params.m_command.LoadString(this_macro.cmd[i].n);
			params.m_p1=p1;
			params.m_p2=p2;
			params.m_p3=p3;
			button=params.DoModal();
			if(button==IDC_CANCEL)
			{
				progress.AddString("*** Step Canceled ***");
				continue;
			}
			if(button==IDC_END)
			{
				progress.AddString("*** Macro Execution Canceled ***");
				break;
			}
			this_macro.cmd[i].p1=p1=params.m_p1;
			this_macro.cmd[i].p2=p2=params.m_p2;
			this_macro.cmd[i].p3=p3=params.m_p3;
			progress.SetForegroundWindow();
		}

		if(this_macro.cmd[i].f&MAC_TONAME)
		{
			if(this_macro.cmd[i].p3!=-1)
			{
				s.Format("-%s[%d,%d,%d]",
					short_name[this_macro.cmd[i].n-IDS_SAVEALL],p1,p2,p3);
			} else {
				if(this_macro.cmd[i].p2!=-1)
				{
					s.Format("-%s(%d,%d)",
						short_name[this_macro.cmd[i].n-IDS_SAVEALL],p1,p2);
				} else {
					if(this_macro.cmd[i].p1!=-1)
					{
						s.Format("-%s(%d)",
							short_name[this_macro.cmd[i].n-IDS_SAVEALL],p1);
					} else {
						s.Format("-%s",short_name[this_macro.cmd[i].n-IDS_SAVEALL]);
					}
				}
			}
			newname+=s;
		}		

		active_frame=display=saved_af;
		buissy=true;
		switch(this_macro.cmd[i].n)
		{
		case IDS_REMOVE:
			mode=0;
			OnFileRemove();
			if(saved_af>top_frame-1) saved_af=top_frame-1;
			display=active_frame=saved_af;
			backup_picture_valid=false;
			redraw_pic=true;
			break;
		case IDS_ACTIVE2:
			mode=0;
			progress.AddString(IDS_ACTIVE2);
			OnSpecialMakevisibleactiveframe2();
			break;
		case IDS_CORRECT_BRIGHTNESS:
			mode=0;
			correction_factor=((double)p1)/100.0;
			progress.AddString(IDS_CORRECT_BRIGHTNESS,(int)(correction_factor*100.0));
			correct(((double)p2)/1000.0,(double)p3);
			backup_picture_valid=false;
			redraw_pic=true;
			break;
		case IDS_ADD_RECTANGLES:
			mode=0;
			add_factor=-((double)p1)/1000.0;
			progress.AddString(IDS_ADD_RECTANGLES,(int)(add_factor*1000.0));
			add();
			backup_picture_valid=false;
			redraw_pic=true;
			break;
		case IDS_BLEND_RECTANGLES:
			mode=0;
			blend_factor=((double)p1)/1000.0;
			progress.AddString(IDS_BLEND_RECTANGLES,(int)(blend_factor*1000.0));
			blend();
			backup_picture_valid=false;
			redraw_pic=true;
			break;
		case IDS_SKEW:
			mode=0;
			skew(p1);
			backup_picture_valid=false;
			redraw_pic=true;
			break;
		case IDS_BANDW:
			mode=0;
			progress.AddString(IDS_BANDW);
			monochrome();
			backup_picture_valid=false;
			redraw_pic=true;
			break;
		case IDS_SELECTALL:
			mode=0;
			progress.AddString(IDS_SELECTALL);
			full_mode=true;
			mag=full;
			break;
		case IDS_FIX:
			mode=0;
			progress.AddString(IDS_FIX);
			fix();
			redraw_pic=true;
			display=PICTURE_BUFFER;
			break;
		case IDS_UNFIX:
			mode=0;
			progress.AddString(IDS_UNFIX);
			unfix();
			redraw_pic=true;
			display=PICTURE_BUFFER;
			break;
		case IDS_SAVEALL:
			mode=0;
			progress.AddString(IDS_SAVEALL);
			saveallframes();
			break;
		case IDS_RELOADALL:
			mode=0;
			progress.AddString(IDS_RELOADALL);
			for(j=0; j<top_frame; j++)
			{
				if(buffer[j].hidden) continue;
				display=active_frame=j;
				reload();
			}
			break;
		case IDS_REVERSE:
			mode=0;
			reverseorder();
			break;
		case IDS_BALANCE:
			mode=0;
			balance(this);
			break;
		case IDS_LOCALBALANCE:
			mode=0;
			localbalance(this);
			break;
		case IDS_AUTOALIGN1:
			mode=0;
			Autoalignandsize(this);
			break;
		case IDS_AUTOALIGN2:
			mode=0;
			Autoalignandsize2(this);
			break;
		case IDS_RESIZEALL:
			mode=0;
			size_factor=p1;
			fbase=(float)(10000/size_factor);
			resizeall(this);
			break;
		case IDS_GROUP:
			mode=0;
			group=p1;
			decimate(this);
			saved_af=active_frame;
			break;
		case IDS_DETAIL:
			mode=0;
			noise_level=p1;
			detail(this);
			display=PICTURE_BUFFER;
			redraw_pic=true;
			break;
		case IDS_PATCHWORK:
			mode=0;
			patch_size=p1;
			patch_threshold=p2;
			patch(this);
			display=PICTURE_BUFFER;
			redraw_pic=true;
			break;
		case IDS_LIGHTDARK:
			mode=0;
			light_threshold=p1;
			dark_threshold=p2;
			lightdark(this);
			display=PICTURE_BUFFER;
			redraw_pic=true;
			break;
		case IDS_GAPS:
			mode=0;
			DoDepthflow();
			display=PICTURE_BUFFER;
			redraw_pic=true;
			break;
		case IDS_ISLANDS:
			mode=0;
			min_island_size=p1;
			removeislands();
			display=PICTURE_BUFFER;
			redraw_pic=true;
			break;
		case IDS_STEPS:
			if(redraw_pic)
			{
				redraw_picture();
				redraw_pic=false;
			}
			mode=0;
			removesteps(this);
			display=PICTURE_BUFFER;
			redraw_pic=true;
			break;
		case IDS_INTERPOLATE:
			mode=0;
			StackInterpolate(this);
			display=OUT_BUFFER;
			break;
		case IDS_NEWFROMPICTURE:
			if(redraw_pic)
			{
				redraw_picture();
				redraw_pic=false;
			}
			mode=0;
			display=PICTURE_BUFFER;
			macronewframe(newname);
			break;
		case IDS_NEWFROMOUT:
			mode=0;
			display=OUT_BUFFER;
			macronewframe(newname);
			break;
		case IDS_REPLACEFRAME:
			mode=0;
			display=OUT_BUFFER;
			Replace();
			break;
		case IDS_RGB:
			mode=0;
			rfactor=(float)p1/1000.0f;
			gfactor=(float)p2/1000.0f;
			bfactor=(float)p3/1000.0f;
			adjustcolour();
			progress.AddString(IDS_RGB,(int)(rfactor*1000.0f),(int)(gfactor*1000.0f),(int)(bfactor*1000.0f));
			mode=BRIGHTNESSX;
			break;
		case IDS_CONTRAST:
			mode=0;
			contrast_delta=p1;
			contrast();
			progress.AddString(IDS_CONTRAST,contrast_delta);
			mode=CONTRASTX;
			break;
		case IDS_DFTACTIVE:
			FilterActive();
			display=OUT_BUFFER;
			progress.AddString(IDS_DFTACTIVE);
			break;
		case IDS_DFTSTACK:
			FilterStack(this);
			display=OUT_BUFFER;
			break;
		case IDS_DFTWEIGHTED:
			FilterWeighted(this);
			display=OUT_BUFFER;
			break;
		case IDS_DFTDEPTH:
			FilterDM();
			display=OUT_BUFFER;
			progress.AddString(IDS_DFTDEPTH);
			break;
		case IDS_LOWPASS:
			mode=0;
			dft.displacement=(float)p2/1000.0f;
			dft.width=(float)p1/1000.0f;
			progress.AddString(IDS_LOWPASS,
				(int)(dft.width*1000.0f),
				(int)(dft.displacement*1000.0f));
			display=OUT_BUFFER;
			ImmediateLowpass();
			break;
		case IDS_HIGHPASS:
			mode=0;
			dft.displacement=(float)p2/1000.0f;
			dft.width=(float)p1/1000.0f;
			progress.AddString(IDS_HIGHPASS,
				(int)(dft.width*1000.0f),
				(int)(dft.displacement*1000.0f));
			display=OUT_BUFFER;
			ImmediateHighpass();
			break;
		case IDS_FINISH:
			progress.AddString("Finish all Filtering etc.");
			mode=0;
			break;
		case IDS_DFTALLFRAMES:
			filterallframes(this);
			display=active_frame;
			break;
		case IDS_BANDPASS:
			mode=0;
			dft.displacement=1.0f-(float)p2/1000.0f;
			dft.width=(float)p1/1000.0f;
			progress.AddString(IDS_BANDPASS,
				(int)(dft.width*1000.0f),
				(int)(1000.0f-dft.displacement*1000.0f));
			display=OUT_BUFFER;
			ImmediateBandpass();
			break;
		case IDS_BUTTERWORTH:
			mode=0;
			dft.displacement=(float)p2/1000.0f;
			dft.cutoff=1.0f-(float)p1/1000.0f;
			dft.order=(float)p3;
			progress.AddString(IDS_BUTTERWORTH,
				(int)((1.0f-dft.cutoff)*1000.0f),
				(int)(dft.displacement*1000.0f),
				(int)(dft.order));
			display=OUT_BUFFER;
			ImmediateButterworth();
			break;
		case IDS_BANDSTOP:
			mode=0;
			dft.displacement=0.5f-(float)p2/2000.0f;
			dft.width=(float)p1/1000.0f;
			dft.centre=(float)p3/1000.0f;
			progress.AddString(IDS_BANDSTOP,
				(int)(dft.width*1000.0f),
				(int)(2.0f*(0.5f-dft.displacement)*1000.0f),
				(int)(dft.centre*1000.0f));
			display=OUT_BUFFER;
			ImmediateBandstop();
			break;
		case IDS_FANDRDEPTHMAP:
			FilterDM();
			progress.AddString(IDS_DFTDEPTH);
			dftouttodepth();
			backup_picture_valid=false;
			redraw_picture();
			display=PICTURE_BUFFER;
			mode=0;
			redraw_pic=true;
			break;
		case IDS_DFTOUTNORMAL:
			dft.out_mode=OUT_NORMAL;
			progress.AddString(IDS_DFTOUTNORMAL);
			break;
		case IDS_DFTOUTBRIGHTEN:
			dft.out_mode=OUT_BRIGHT;
			dft.brightness=(float)p1/100.0f;
			progress.AddString(IDS_DFTOUTBRIGHTEN,this_macro.cmd[i].p1);
			break;
		case IDS_SAVEDEPTH:
			progress.AddString(IDS_SAVEDEPTH);
			savedepth();
			break;
		case IDS_LOADDEPTH:
			progress.AddString(IDS_LOADDEPTH);
			loaddepth();
			redraw_pic=true;
			break;
		case IDS_GOTOFRAME:
			saved_af=p1;
			if(saved_af<0) saved_af=0;
			if(saved_af>top_frame-1) saved_af=top_frame-1;
			active_frame=display=saved_af;
			s.Format("Goto Frame(%d)",saved_af);
			progress.AddString(s);
			break;
		case IDS_MACRO1:
		case IDS_MACRO2:
		case IDS_MACRO3:
		case IDS_MACRO4:
		case IDS_MACRO5:
		case IDS_MACRO6:
		case IDS_MACRO7:
		case IDS_MACRO8:
		case IDS_MACRO9:
		case IDS_MACRO10:
			start=p1;
			inc=p2;
			end=p3;

			saved_mac=mac;
			saved_value=value;

			mac=this_macro.cmd[i].n-IDS_MACRO1;

			progress.AddString("*** Start of Nested Macro ***");

			if(inc==0) inc=1;
			for(j=start; j<=end; j+=inc)
			{
				if(progress.m_cancel)
				{
					progress.AddString("*** Macro Terminated ***");
					break;
				}
				saved_newname=newname;
				value=j;
				nest++;
				active_frame=saved_af;
				DoMacro();
				saved_af=active_frame;
				nest--;
				newname=saved_newname;
			}

			progress.AddString("*** End of Nested Macro ***");
			value=saved_value;
			mac=saved_mac;
			break;
		}
	}
	macro_mode=false;
	active_frame=saved_af;
	s.Format("***  Finished Executing %s Macro  ***",macro[mac].name);
	progress.AddString(s);
	if(nest) return;
	if(redraw_pic)
	{
		progress.AddString("* Please wait while Picture is redrawn");
		redraw_picture();
	}
	buissy=false;
	if((display!=PICTURE_BUFFER) && (display!=OUT_BUFFER)) display=active_frame;
	if(silent) return;
	progress.Minimize();
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	AfxGetMainWnd()->SetFocus();
	AfxGetMainWnd()->SetForegroundWindow();
	RefreshScreen();
	return;
}

void CCombineDoc::OnMacro1() 
{
	if(fix_active) fix_active=false;
	if(depth_flag) depth_flag=false;
	if(plane_mode) plane_mode=false;
	mac=0;
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro2() 
{
	if(fix_active) fix_active=false;
	if(depth_flag) depth_flag=false;
	if(plane_mode) plane_mode=false;
	mac=1;
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro3() 
{
	if(fix_active) fix_active=false;
	if(depth_flag) depth_flag=false;
	if(plane_mode) plane_mode=false;
	mac=2;
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro4() 
{
	if(fix_active) fix_active=false;
	if(depth_flag) depth_flag=false;
	if(plane_mode) plane_mode=false;
	mac=3;
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro5() 
{
	if(fix_active) fix_active=false;
	if(depth_flag) depth_flag=false;
	if(plane_mode) plane_mode=false;
	mac=4;
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro6() 
{
	if(fix_active) fix_active=false;
	if(depth_flag) depth_flag=false;
	if(plane_mode) plane_mode=false;
	mac=5;
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro7() 
{
	if(fix_active) fix_active=false;
	if(depth_flag) depth_flag=false;
	if(plane_mode) plane_mode=false;
	mac=6;
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro8() 
{
	if(fix_active) fix_active=false;
	if(depth_flag) depth_flag=false;
	if(plane_mode) plane_mode=false;
	mac=7;
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro9()
{
	if(fix_active) fix_active=false;
	if(depth_flag) depth_flag=false;
	if(plane_mode) plane_mode=false;
	mac=8;
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro10() 
{
	if(fix_active) fix_active=false;
	if(depth_flag) depth_flag=false;
	if(plane_mode) plane_mode=false;
	mac=9;
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacroSaveasdefaultset() 
{
	int i,j;
	char c[1];
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString macpath=helppath.Left(helppath.ReverseFind('\\')+1);
	CFile f;

	CFileDialog fd(false,"czm",0,0,"Macro Set|*.czm;|*.czm||",0);
	fd.m_ofn.lpstrInitialDir=macpath.GetBuffer(1024);
	CString temp="defaultmac.czm";
	fd.m_ofn.lpstrFile=temp.GetBuffer(1024);
	if(fd.DoModal()==IDCANCEL) goto finish;

	f.Open(fd.GetPathName(),CFile::modeCreate|CFile::modeWrite);
	for(i=0; i<10; i++)
	{
		f.Write(macro[i].cmd,24*sizeof(CZCommand));
		for(j=0; j<macro[i].name.GetLength(); j++)
		{
			c[0]=macro[i].name[j];
			f.Write(&c[0],1);
		}
		f.Write("\0",1);
	}
	f.Close();
finish:
	temp.ReleaseBuffer();
	macpath.ReleaseBuffer();
}

void CCombineDoc::OnMacroLoadmacroset() 
{
	int i,j,k;
	char c[1];

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString macpath=helppath.Left(helppath.ReverseFind('\\')+1);
	CFile f;
	CFileDialog fd(true,"czm",0,0,"Macro Set|*.czm;|*.czm||",0);
	fd.m_ofn.lpstrInitialDir=macpath.GetBuffer(1024);
	CString temp="defaultmac.czm";
	fd.m_ofn.lpstrFile=temp.GetBuffer(1024);
	if(fd.DoModal()==IDCANCEL) goto finish;

	f.Open(fd.GetPathName(),CFile::modeRead);
	for(i=0; i<10; i++)
	{
		f.Read(macro[i].cmd,24*sizeof(CZCommand));
		for(j=0; j<24; j++)
		{
			if((macro[i].cmd[j].n<0) || (macro[i].cmd[j].n>last_command))
			{
				macro[i].cmd[j].p1=-1;
				macro[i].cmd[j].p2=-1;
				macro[i].cmd[j].p3=-1;
				continue;
			}
			for(k=0; default_command[k].n<=last_command; k++)
			{
				if(macro[i].cmd[j].n==default_command[k].n)
					break;
			}
			if(default_command[k].p1==-1)
				macro[i].cmd[j].p1=-1;
			if(default_command[k].p2==-1)
				macro[i].cmd[j].p2=-1;
			if(default_command[k].p3==-1)
				macro[i].cmd[j].p3=-1;
		}
		macro[i].name="";
		f.Read(&c[0],1);
		while(c[0])
		{
			macro[i].name+=c[0];
			f.Read(&c[0],1);
		}
	}
	f.Close();
finish:
	temp.ReleaseBuffer();
	macpath.ReleaseBuffer();
	InitializeMacros();
	savedefaultmacros();
}

void CCombineDoc::OnMacroDisablepause() 
{
	if(!pause_enabled)
	{
		AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO_ENABLEPAUSE,MF_BYCOMMAND,ID_MACRO_ENABLEPAUSE,"&Pause (Untick to Disable)");
		pause_enabled=true;
	} else {
		AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO_ENABLEPAUSE,MF_BYCOMMAND,ID_MACRO_ENABLEPAUSE,"&Pause (Tick to Enable)");
		pause_enabled=false;
	}
}

void CCombineDoc::macronewframe(CString commands)
{
	CString s;
	GetBuffer(top_frame);
	if(display==OUT_BUFFER) buffer[top_frame].name=(newnumber("New-Out")+commands);
	else buffer[top_frame].name=(newnumber("New")+"-Pic"+commands);
	buffer[top_frame].hidden=true;
	top_frame++;
	active_frame=top_frame-1;
	int i;

//	Replace();
	PIXEL* f1=(PIXEL*)GetBuffer(active_frame);
	PIXEL* f0=(PIXEL*)GetBuffer(display);
	for(i=0; i<w*h; i++)
	{
		if(f0[i].abgr!=wipeout)
		{
			f1[i]=f0[i];
			f1[i].a=aGOOD;
		}
		else
		{
			f1[i].abgr=0;
			f1[i].a=aCLEAR;
		}
	}
	FreeBuffer(display);
	UpdateBuffer(active_frame);

	display=top_frame-1;
	UpdateBuffer(top_frame-1);
	progress.AddString("Create New Frame");
	s.Format("(%d) %s",top_frame-1,buffer[top_frame-1].name);
	progress.AddString(s);
}

bool CCombineDoc::loaddefaultmacros()
{
	int i,j,k;
	char c[2];
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString macpath=helppath.Left(helppath.ReverseFind('\\')+1)+"workingmac.czx";
//	MessageBox(0,macpath,"*** 1 *** workingmac.czx",MB_OK);
	CFile f;
	if(f.Open(macpath,CFile::modeRead)==0) return false;
	for(i=0; i<10; i++)
	{
		f.Read(macro[i].cmd,24*sizeof(CZCommand));
		for(j=0; j<24; j++)
		{
			if((macro[i].cmd[j].n<0) || (macro[i].cmd[j].n>last_command))
			{
				macro[i].cmd[j].p1=-1;
				macro[i].cmd[j].p2=-1;
				macro[i].cmd[j].p3=-1;
				continue;
			}
			for(k=0; default_command[k].n<=last_command; k++)
			{
				if(macro[i].cmd[j].n==default_command[k].n)
					break;
			}
			if(default_command[k].p1==-1)
				macro[i].cmd[j].p1=-1;
			if(default_command[k].p2==-1)
				macro[i].cmd[j].p2=-1;
			if(default_command[k].p3==-1)
				macro[i].cmd[j].p3=-1;
		}
		macro[i].name="";
		f.Read(&c[0],1);
		while(c[0])
		{
			macro[i].name+=c[0];
			f.Read(&c[0],1);
		}
//		MessageBox(0,macro[i].name,"*** 2 *** Loaded Macro",MB_OK);
	}
	f.Close();
//	MessageBox(0,"Finished Loading Macros","*** 3 ***",MB_OK);
	return true;
//	return false;
}

void CCombineDoc::savedefaultmacros()
{
	int i,j;
	char c[2];
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString macpath=helppath.Left(helppath.ReverseFind('\\')+1)+"workingmac.czx";
	CFile f;

	f.Open(macpath,CFile::modeCreate|CFile::modeWrite);
	for(i=0; i<10; i++)
	{
		f.Write(macro[i].cmd,24*sizeof(CZCommand));
		for(j=0; j<macro[i].name.GetLength(); j++)
		{
			c[0]=macro[i].name[j];
			f.Write(&c[0],1);
		}
		f.Write("\0",1);
	}
	f.Close();
}

void CCombineDoc::OnMacroClearallmacros() 
{
	int i;
	for(i=0; i<10; i++)
	{
		Clear_macro_commands(i);
	}
	macro[0].name="Macro 1";
	macro[1].name="Macro 2";
	macro[2].name="Macro 3";
	macro[3].name="Macro 4";
	macro[4].name="Macro 5";
	macro[5].name="Macro 6";
	macro[6].name="Macro 7";
	macro[7].name="Macro 8";
	macro[8].name="Macro 9";
	macro[9].name="Macro 10";
	InitializeMacros();
	savedefaultmacros();
}

bool CCombineDoc::silent_macro(char *mac_name)
{
	silent=true;
	int i;
	for(i=0; i<10; i++)
	{
		if(macro[i].name==mac_name) break;
	}
	if(i==10) return false;
	mac=i;
	DoMacro();
	return true;
}
