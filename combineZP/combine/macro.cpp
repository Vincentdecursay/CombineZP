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
#include "MainFrm.h"
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
#include "process.h"
#include "macparam.h"
#include ".\combinedoc.h"

CCombineApp theApp;

struct CZMacro macro[10];

struct CZMacParams macparams[LAST_MACPARAM];

void StartTimer(__int64 t);
void WaitForTimer();

wchar_t *short_name[]=
{
	L"SALL",L"LALL",L"REV",L"BAL",L"AL1",L"AL2",
	L"SZALL",L"GRP",L"D",L"G",L"I",
	L"S",L"INTERP",L"NEWP",L"NEWO",L"RPLF",L"RGB",
	L"CONT",L"FACT",L"FSTK",L"FDPTH",L"LPF",
	L"HPF",L"FIN",L"FALL",L"BPF",
	L"BUTF",L"BSF",L"FRDPTH",L"FNOUT",
	L"FBOUT",L"SD",L"LD",L"GTF",
	0,0,0,0,0,0,0,0,0,0,L"BAW",L"SKEW",L"FIX",
	L"UFIX",L"A",L"PCH",L"FWS",L"LTDK",L"LBAL",
	L"REM",L"ACTIVE2",L"CORBR",L"ADD",L"BLEND",
	L"GREL",L"GTOP",L"REM1",L"REM2",L"GO1",L"GO2",
	L"USE",L"DNTUSE",L"USEALL",0,0,L"INSRTA",L"INSRTB",
	L"RESTART",L"CPYTOO",L"PSTO",L"PSTU",L"CLEAR",L"CLEARU",
	L"TRANSP",L"INVFIX",L"CPYPTO",0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,L"FWSWC",L"THR",0,0,0,0,L"ENAV",
	L"BALCOL",L"ROTSH",L"BALBR",0,L"RGB",L"INV",
	L"HSG",L"CPROD",L"STRCHC",L"EXTRNG",L"ADJSPC",L"REDNSE",L"RAINB",0,0,0,0,0,
	L"PCLR",L"PADD",L"POUTMC",L"PDEPTH",L"PADDW",L"POUTW",L"PMAXUF",L"PWAUF",0,0,0,0,0,0,0,0,0,0,0,0,L"FLTR"
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
	{IDS_DETAIL,25,1,0,0},
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
	{IDS_SKEW,10050,10000,-1,0},
	{IDS_FIX,-1,-1,-1,0},
	{IDS_UNFIX,-1,-1,-1,0},
	{IDS_SELECTALL,-1,-1,-1,0},
	{IDS_PATCHWORK,4,10,-1,0},
	{IDS_DFTWEIGHTED,-1,-1,-1,0},
	{IDS_LIGHTDARK,140,140,-1,0},
	{IDS_LOCALBALANCE,-1,-1,-1,0},
	{IDS_REMOVE,-1,-1,-1,0},
	{IDS_ACTIVE2,-1,-1,-1,0},
	{IDS_CORRECT_BRIGHTNESS,1000,0,-1,0},
	{IDS_ADD_RECTANGLES,1000,-1,-1,0},
	{IDS_BLEND_RECTANGLES,1000,-1,-1,0},
	{IDS_GOTO_FRAME_REL,-2,-1,-1,0},
	{IDS_GOTO_TOP_FRAME,-1,-1,-1,0},
	{IDS_REMEMBER_FRAME1,-1,-1,-1,0},
	{IDS_REMEMBER_FRAME2,-1,-1,-1,0},
	{IDS_GOTO_FRAME1,-1,-1,-1,0},
	{IDS_GOTO_FRAME2,-1,-1,-1,0},
	{IDS_USE_FRAME,-1,-1,-1,0},
	{IDS_DONT_USE_FRAME,-1,-1,-1,0},
	{IDS_USE_ALL_FRAMES,-1,-1,-1,0},
	{IDS_DONT_USE_ABOVE,-1,-1,-1,0},
	{IDS_DONT_USE_BELOW,-1,-1,-1,0},
	{IDS_INSERT_ABOVE,-1,-1,-1,0},
	{IDS_INSERT_BELOW,-1,-1,-1,0},
	{IDS_RESTART_MACRO,-1,-1,-1,0},
	{IDS_COPY_TO_OUT,-1,-1,-1,0},
	{IDS_PASTE_OVER,-1,-1,-1,0},
	{IDS_PASTE_UNDER,-1,-1,-1,0},
	{IDS_CLEAR_ALL,-1,-1,-1,0},
	{IDS_CLEAR_UNFIXED,-1,-1,-1,0},
	{IDS_MAKE_TRANSPARENT,-1,-1,-1,0},
	{IDS_INVERT_FIXED,-1,-1,-1,0},
	{IDS_PICTURE_TO_FRAME,-1,-1,-1,0},
	{IDS_EXCOM1,0,0,0,0},
	{IDS_EXCOM2,0,0,0,0},
	{IDS_EXCOM3,0,0,0,0},
	{IDS_EXCOM4,0,0,0,0},
	{IDS_EXCOM5,0,0,0,0},
	{IDS_EXCOM6,0,0,0,0},
	{IDS_EXCOM7,0,0,0,0},
	{IDS_EXCOM8,0,0,0,0},
	{IDS_EXCOM9,0,0,0,0},
	{IDS_EXCOM10,0,0,0,0},
	{IDS_TAKEPIC,-1,-1,-1,0},
	{IDS_STARTSECOND,30,-1,-1,0},
	{IDS_STARTMINUTE,1,-1,-1,0},
	{IDS_STARTHOUR,1,-1,-1,0},
	{IDS_WAIT,-1,-1,-1,0},
	{IDS_BANDWSW,-1,-1,-1,0},
	{IDS_CHECK_DEPTH,-1,-1,-1,0},
	{IDS_CLEAN_START,-1,-1,-1,0},
	{IDS_PAUSEMPI,-1,-1,-1,0},
	{IDS_WEIGHTEDWC,-1,-1,-1},
	{IDS_DFTOUTTHRESHOLD,100,-1,-1,0},
	{IDS_PADD,0,0,0,0},
	{IDS_PSUB,0,0,0,0},
	{IDS_PMULT,0,0,0,0},
	{IDS_PDIV,0,0,0,0},
	{IDS_HDRAVERAGE,0,0,1000,0},
	{IDS_BALCOLOUR,-1,-1,-1,0},
	{IDS_ASTROALIGN,50,-1,-1,0},
	{IDS_BALBRIGHT,-1,-1,-1,0},
	{IDS_ACTIVEOUT,70,-1,-1,0},
	{IDS_SPLITRGB,-1,-1,-1,0},
	{IDS_INVERT,-1,-1,-1,0},
	{IDS_SPLITHSG,-1,-1,-1,0},
	{IDS_COLOURPROD,-1,-1,-1,0},
	{IDS_STRETCHCOLOURS,-1,-1,-1,0},
	{IDS_EXTRACTRANGE,0,1529,5,0},
	{IDS_ADJUSTSPECTRUM,0,1529,0,0},
	{IDS_REDUCENOISE,100,15,500,0},
	{IDS_RAINBOW,-1,-1,-1,0},
	{IDS_NEWUSED,-1,-1,-1,0},
	{IDS_NEWDPTH,-1,-1,-1,0},
	{IDS_TEST,0,0,-1,0},
	{IDS_COMPF,-1,-1,-1,0},
	{IDS_ENDMACRO,1,-1,-1,0},
	{IDS_PCLEAR,-1,-1,-1,0},
	{IDS_PADDMAX,0,-1,-1,0},
	{IDS_POUT,100,100,100,0},
	{IDS_PDEPTH,100,3,-1,0},
	{IDS_PADDWEIGHT,0,-1,-1,0},
	{IDS_PWOUT,100,100,100,0},
	{IDS_PADDMAXUF,-1,-1,-1,0},
	{IDS_PADDWAUF,-1,-1,-1,0},
	{IDS_RESETNAME,-1,-1,-1,0},
	{IDS_STARTVIDCAP,-1,-1,-1,0},
	{IDS_ENDVIDCAP,-1,-1,-1,0},
	{IDS_ALIGNSRM,100,-1,-1,0},
	{IDS_SHROTSC,-1,-1,-1,0},
	{IDS_LOADSPECIAL,0,-1,-1,0},
	{IDS_SAVESPECIAL,0,-1,-1,0},
	{IDS_CAPTOFILE,-1,-1,-1,0},
	{IDS_SELECTNEXTRECT,-1,-1,-1,0},
	{IDS_MAKEMOVIE,-1,-1,-1,0},
	{IDS_SAVECLIPPEDFRAMES,-1,-1,-1,0},
	{IDS_EMPTYSTACK,-1,-1,-1,0},
	{IDS_FILTER,-1,-1,-1,0}
// Update last_command in InitializeMacros() & OnMacroEditmacro() when adding new commands
};

void CCombineDoc::OnMacroEditmacro() 
{
	int button;

	CMacro mac;

	for(;;)
	{
		CChooseMacro choose_mac;
		button=choose_mac.DoModal()-100;
		if(button<0) return;

		mac.this_macro=macro[button];
		mac.m_name=macro[button].name;

		mac.first_command=IDS_SAVEALL;
		mac.last_command=IDS_FILTER;

		mac.m_mp=macparams;

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

	last_command=IDS_FILTER;

	if(!started)
	{
		initializecommands();
		initializemacparams();
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
			savedefaultmacros();
		}
		pause_enabled=true;
		started=true;
	}

	CString s;

	str1.LoadString(IDS_STRING439);
	s.Format(str1,macro[0].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_MACRO1,MF_BYCOMMAND|MF_STRING,ID_MACRO1,s);

	str1.LoadString(IDS_STRING440);
	s.Format(str1,macro[1].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_MACRO2,MF_BYCOMMAND|MF_STRING,ID_MACRO2,s);

	str1.LoadString(IDS_STRING441);
	s.Format(str1,macro[2].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_MACRO3,MF_BYCOMMAND|MF_STRING,ID_MACRO3,s);

	str1.LoadString(IDS_STRING442);
	s.Format(str1,macro[3].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_MACRO4,MF_BYCOMMAND|MF_STRING,ID_MACRO4,s);

	str1.LoadString(IDS_STRING443);
	s.Format(str1,macro[4].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_MACRO5,MF_BYCOMMAND|MF_STRING,ID_MACRO5,s);

	str1.LoadString(IDS_STRING444);
	s.Format(str1,macro[5].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_MACRO6,MF_BYCOMMAND|MF_STRING,ID_MACRO6,s);

	str1.LoadString(IDS_STRING445);
	s.Format(str1,macro[6].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_MACRO7,MF_BYCOMMAND|MF_STRING,ID_MACRO7,s);

	str1.LoadString(IDS_STRING446);
	s.Format(str1,macro[7].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_MACRO8,MF_BYCOMMAND|MF_STRING,ID_MACRO8,s);

	str1.LoadString(IDS_STRING447);
	s.Format(str1,macro[8].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_MACRO9,MF_BYCOMMAND|MF_STRING,ID_MACRO9,s);

	str1.LoadString(IDS_STRING448);
	s.Format(str1,macro[9].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_MACRO10,MF_BYCOMMAND|MF_STRING,ID_MACRO10,s);

	str1.LoadString(IDS_STRING449);
	s.Format(str1,extcommand[10].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_EXCOM11,MF_BYCOMMAND|MF_STRING,ID_EXCOM11,s);

	str1.LoadString(IDS_STRING450);
	s.Format(str1,extcommand[11].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_EXCOM12,MF_BYCOMMAND|MF_STRING,ID_EXCOM12,s);

	str1.LoadString(IDS_STRING451);
	s.Format(str1,extcommand[12].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_SYSCOM13,MF_BYCOMMAND|MF_STRING,ID_SYSCOM13,s);

	if(!((CMainFrame*)AfxGetMainWnd())->simple_mode)
		((CMainFrame*)AfxGetMainWnd())->SetMenu((((CMainFrame*)AfxGetMainWnd())->m_menu));

	InitializeComboBox();
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

// Align and Balance
void CCombineDoc::defaultmac01()
{
	Clear_macro_commands(0);

	macro[0].name.LoadString(IDS_STRING661);

	macro[0].cmd[0].n=IDS_SELECTALL;

	macro[0].cmd[1].n=IDS_DFTOUTNORMAL;

	macro[0].cmd[2].n=IDS_BALANCE;

	macro[0].cmd[3].n=IDS_AUTOALIGN1;
}

// Align and Balance
void CCombineDoc::defaultmac02()
{
	Clear_macro_commands(1);

	macro[1].name.LoadString(IDS_STRING644);

	macro[1].cmd[0].n=IDS_SELECTALL;

	macro[1].cmd[1].n=IDS_DFTOUTNORMAL;

	macro[1].cmd[2].n=IDS_BALANCE;

	macro[1].cmd[3].n=IDS_ALIGNSRM;
	macro[1].cmd[3].p1=100;
}

// Do Stack
void CCombineDoc::defaultmac03()
{
	Clear_macro_commands(2);

	macro[2].name.LoadString(IDS_STRING452);

	macro[2].cmd[0].n=IDS_DETAIL;
	macro[2].cmd[0].p1=5;
	macro[2].cmd[0].p2=1;
	macro[2].cmd[0].p3=710;

	macro[2].cmd[1].n=IDS_ISLANDS;
	macro[2].cmd[1].p1=5;

	macro[2].cmd[2].n=IDS_GAPS;

	macro[2].cmd[3].n=IDS_LOWPASS;
	macro[2].cmd[3].p1=250;
	macro[2].cmd[3].p2=0;

	macro[2].cmd[4].n=IDS_FANDRDEPTHMAP;

	macro[2].cmd[5].n=IDS_INTERPOLATE;

	macro[2].cmd[6].n=IDS_NEWFROMOUT;

	macro[2].cmd[7].n=IDS_GOTO_TOP_FRAME;

	macro[2].cmd[8].n=IDS_HIGHPASS;
	macro[2].cmd[8].p1=1000;
	macro[2].cmd[8].p2=750;

	macro[2].cmd[9].n=IDS_DFTACTIVE;

	macro[2].cmd[10].n=IDS_REPLACEFRAME;

	macro[2].cmd[11].n=IDS_CONTRAST;
	macro[2].cmd[11].p1=5;

	macro[2].cmd[12].n=IDS_REPLACEFRAME;

	macro[2].cmd[13].n=IDS_ACTIVEOUT;
	macro[2].cmd[13].p1=100;
}

// Do Soft Stack
void CCombineDoc::defaultmac04()
{
	Clear_macro_commands(3);

	macro[3].name.LoadString(IDS_STRING453);
	macro[3].name=L"Do Soft Stack";

	macro[3].cmd[0].n=IDS_DETAIL;
	macro[3].cmd[0].p1=15;
	macro[3].cmd[0].p2=3;
	macro[3].cmd[0].p3=710;

	macro[3].cmd[1].n=IDS_ISLANDS;
	macro[3].cmd[1].p1=12;

	macro[3].cmd[2].n=IDS_GAPS;

	macro[3].cmd[3].n=IDS_LOWPASS;
	macro[3].cmd[3].p1=250;
	macro[3].cmd[3].p2=0;

	macro[3].cmd[4].n=IDS_FANDRDEPTHMAP;

	macro[3].cmd[5].n=IDS_INTERPOLATE;

	macro[3].cmd[6].n=IDS_NEWFROMOUT;

	macro[3].cmd[7].n=IDS_GOTO_TOP_FRAME;

	macro[3].cmd[8].n=IDS_HIGHPASS;
	macro[3].cmd[8].p1=1000;
	macro[3].cmd[8].p2=750;

	macro[3].cmd[9].n=IDS_DFTACTIVE;

	macro[3].cmd[10].n=IDS_REPLACEFRAME;

	macro[3].cmd[11].n=IDS_CONTRAST;
	macro[3].cmd[11].p1=5;

	macro[3].cmd[12].n=IDS_REPLACEFRAME;

	macro[3].cmd[13].n=IDS_ACTIVEOUT;
	macro[3].cmd[13].p1=100;
}

// Weighted Average
void CCombineDoc::defaultmac05()
{
	Clear_macro_commands(4);

	macro[4].name.LoadString(IDS_STRING454);

	macro[4].cmd[0].n=IDS_HIGHPASS;
	macro[4].cmd[0].p1=1000;
	macro[4].cmd[0].p2=650;

	macro[4].cmd[1].n=IDS_DFTWEIGHTED;

	macro[4].cmd[2].n=IDS_NEWFROMOUT;

	macro[4].cmd[3].n=IDS_GOTO_TOP_FRAME;

	macro[4].cmd[4].n=IDS_CONTRAST;
	macro[4].cmd[4].p1=15;

	macro[4].cmd[5].n=IDS_REPLACEFRAME;

	macro[4].cmd[5].n=IDS_FINISH;

	macro[4].cmd[6].n=IDS_ACTIVEOUT;
	macro[4].cmd[6].p1=100;
}

// Pyramid Weighted Average
void CCombineDoc::defaultmac06()
{
	Clear_macro_commands(5);

	macro[5].name.LoadString(IDS_STRING455);

	macro[5].cmd[0].n=IDS_PCLEAR;

	macro[5].cmd[1].n=IDS_PADDWAUF;

	macro[5].cmd[2].n=IDS_PWOUT;
	macro[5].cmd[2].p1=150;
	macro[5].cmd[2].p2=110;
	macro[5].cmd[2].p3=90;

	macro[5].cmd[3].n=IDS_NEWFROMOUT;

	macro[5].cmd[4].n=IDS_GOTO_TOP_FRAME;

	macro[5].cmd[5].n=IDS_ACTIVEOUT;
	macro[5].cmd[5].p1=100;
}

// Pyramid Do Stack
void CCombineDoc::defaultmac07()
{
	Clear_macro_commands(6);

	macro[6].name.LoadString(IDS_STRING645);

	macro[6].cmd[0].n=IDS_PCLEAR;

	macro[6].cmd[1].n=IDS_PADDMAXUF;

	macro[6].cmd[2].n=IDS_PDEPTH;
	macro[6].cmd[2].p1=3;
	macro[6].cmd[2].p2=3;

	macro[6].cmd[3].n=IDS_ISLANDS;
	macro[6].cmd[3].p1=5;

	macro[6].cmd[4].n=IDS_GAPS;

	macro[6].cmd[5].n=IDS_LOWPASS;
	macro[6].cmd[5].p1=250;
	macro[6].cmd[5].p2=0;

	macro[6].cmd[6].n=IDS_FANDRDEPTHMAP;

	macro[6].cmd[7].n=IDS_INTERPOLATE;

	macro[6].cmd[8].n=IDS_NEWFROMOUT;

	macro[6].cmd[9].n=IDS_GOTO_TOP_FRAME;

	macro[6].cmd[10].n=IDS_HIGHPASS;
	macro[6].cmd[10].p1=1000;
	macro[6].cmd[10].p2=750;

	macro[6].cmd[11].n=IDS_DFTACTIVE;

	macro[6].cmd[12].n=IDS_REPLACEFRAME;

	macro[6].cmd[13].n=IDS_CONTRAST;
	macro[6].cmd[13].p1=5;

	macro[6].cmd[14].n=IDS_REPLACEFRAME;

	macro[6].cmd[15].n=IDS_ACTIVEOUT;
	macro[6].cmd[15].p1=100;
}

// Pyramoid Maximum Contrast
void CCombineDoc::defaultmac08()
{
	Clear_macro_commands(7);

	macro[7].name.LoadString(IDS_STRING646);

	macro[7].cmd[0].n=IDS_PCLEAR;

	macro[7].cmd[1].n=IDS_PADDMAXUF;

	macro[7].cmd[2].n=IDS_POUT;
	macro[7].cmd[2].p1=125;
	macro[7].cmd[2].p2=107;
	macro[7].cmd[2].p3=93;

	macro[7].cmd[3].n=IDS_NEWFROMOUT;

	macro[7].cmd[4].n=IDS_GOTO_TOP_FRAME;

	macro[7].cmd[5].n=IDS_ACTIVEOUT;
	macro[7].cmd[5].p1=100;
}

void CCombineDoc::defaultmac09()
{
	Clear_macro_commands(8);
	macro[8].name="All Methods";

	macro[8].cmd[0].n=IDS_MACRO1;
	macro[8].cmd[0].p1=1;
	macro[8].cmd[0].p2=0;
	macro[8].cmd[0].p3=1;

	macro[8].cmd[1].n=IDS_RESETNAME;

	macro[8].cmd[2].n=IDS_MACRO3;
	macro[8].cmd[2].p1=1;
	macro[8].cmd[2].p2=0;
	macro[8].cmd[2].p3=1;
	macro[8].cmd[2].f|=MAC_TONAME;

	macro[8].cmd[3].n=IDS_RESETNAME;

	macro[8].cmd[4].n=IDS_MACRO4;
	macro[8].cmd[4].p1=1;
	macro[8].cmd[4].p2=0;
	macro[8].cmd[4].p3=1;
	macro[8].cmd[4].f|=MAC_TONAME;

	macro[8].cmd[5].n=IDS_RESETNAME;

	macro[8].cmd[6].n=IDS_MACRO7;
	macro[8].cmd[6].p1=1;
	macro[8].cmd[6].p2=0;
	macro[8].cmd[6].p3=1;
	macro[8].cmd[6].f|=MAC_TONAME;

	macro[8].cmd[7].n=IDS_RESETNAME;

	macro[8].cmd[8].n=IDS_MACRO5;
	macro[8].cmd[8].p1=1;
	macro[8].cmd[8].p2=0;
	macro[8].cmd[8].p3=1;
	macro[8].cmd[8].f|=MAC_TONAME;

	macro[8].cmd[9].n=IDS_RESETNAME;

	macro[8].cmd[10].n=IDS_MACRO10;
	macro[8].cmd[10].p1=1;
	macro[8].cmd[10].p2=0;
	macro[8].cmd[10].p3=1;
	macro[8].cmd[10].f|=MAC_TONAME;

	macro[8].cmd[11].n=IDS_RESETNAME;

	macro[8].cmd[12].n=IDS_MACRO6;
	macro[8].cmd[12].p1=1;
	macro[8].cmd[12].p2=0;
	macro[8].cmd[12].p3=1;
	macro[8].cmd[12].f|=MAC_TONAME;

	macro[8].cmd[13].n=IDS_GOTO_TOP_FRAME;
	macro[8].cmd[13].n=IDS_GOTO_FRAME_REL;
	macro[8].cmd[13].p1=-6;
}

void CCombineDoc::defaultmac10()
{
	Clear_macro_commands(9);
	macro[9].name=L"_Pyramid Maximum Contrast";

	macro[9].cmd[0].n=IDS_POUT;
	macro[9].cmd[0].p1=125;
	macro[9].cmd[0].p2=107;
	macro[9].cmd[0].p3=93;

	macro[9].cmd[1].n=IDS_NEWFROMOUT;

	macro[9].cmd[2].n=IDS_GOTO_TOP_FRAME;

	macro[9].cmd[3].n=IDS_ACTIVEOUT;
	macro[9].cmd[3].p1=100;
}

int nest;

UINT CCombineDoc::DoMacroT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	nest=0;
	_this->progress.m_cancel=false;
	_this->DoMacro();
	return 0;
}

void CCombineDoc::update_macparams()
{
	plane_mode=(macparams[21].value==1);
	mag.bottom=(long)macparams[25].value;
	mag.left=(long)macparams[26].value;
	mag.right=(long)macparams[27].value;
	mag.top=(long)macparams[28].value;
	Sizer.p[0].f=macparams[33].value;
	Sizer.p[0].valid=(macparams[34].value!=0);
	Sizer.p[0].x=macparams[35].value;
	Sizer.p[0].y=macparams[36].value;
	Sizer.p[1].f=macparams[37].value;
	Sizer.p[1].valid=(macparams[38].value!=0);
	Sizer.p[1].x=macparams[39].value;
	Sizer.p[1].y=macparams[40].value;
	Sizer.p[2].f=macparams[41].value;
	Sizer.p[2].valid=(macparams[42].value!=0);
	Sizer.p[2].x=macparams[43].value;
	Sizer.p[2].y=macparams[44].value;
	Sizer.p[3].f=macparams[45].value;
	Sizer.p[3].valid=(macparams[46].value!=0);
	Sizer.p[3].x=macparams[47].value;
	Sizer.p[3].y=macparams[48].value;
	angle=macparams[49].value;
	xorg=macparams[50].value;
	yorg=macparams[51].value;
	scale=macparams[52].value;
	smallw=macparams[53].value;
	smallh=macparams[54].value;
	sizexorg=macparams[55].value;
	sizeyorg=macparams[56].value;
	offsetx=macparams[57].value;
	offsety=macparams[58].value;
	goodrect.bottom=(long)macparams[60].value;
	goodrect.top=(long)macparams[61].value;
	goodrect.left=(long)macparams[62].value;
	goodrect.right=(long)macparams[63].value;
	brush_size=macparams[64].value;
	brush_delta=macparams[65].value;
	buffer[active_frame].hidden=(macparams[66].value!=0);
	size_factor=macparams[73].value;
	bandw=(macparams[76].value!=0);
	jpeg_comp=macparams[88].value;

	backup_picture_valid=false;
}

#define CHECK_TOP if(macro_mode && (top_frame<=0)){progress.AddString(IDS_STRING456); progress.m_cancel=true; break;}

void CCombineDoc::DoMacro()
{
	static int nest=0;

	if(theApp.debug_switch)
	{
		CString s;
		s.Format(L"Macro Level %d",nest);
		progress.AddString(s);
	}

	static int frame1;
	static int frame2;
	static bool abort;
	if(nest==0)	abort=false;
	bool end_macro;

	CString p;
	CString help_path=AfxGetApp()->m_pszHelpFilePath;
	p=help_path.Left(help_path.ReverseFind('\\')+1);
	p=p+L"CZTemp";

	int this_mac=mac;
	CString mac_name;

	RECT saved_r;
	saved_r.left=saved_r.right=0;

	if((abort==true) && nest>0) return;

	static int value=0;
	int saved_value;

	CMacParam paramsdlg;

	CRect full(0,0,w-1,h-1);

	static CString newname;
	CString saved_newname,reset_newname;

	if(nest>5)
	{
		CString s1,s2;
		s1.LoadString(IDS_ERRORMACDEEP); s2.LoadString(IDS_MACRO);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		progress.AddString(IDS_STRING457);
		abort=true;
		return;
	}

	if(nest==0)
	{
		AfxGetMainWnd()->ShowWindow(SW_HIDE);
		newname="";
		progress.Maximize();
	}

	WINDOWPLACEMENT ppl;
	int i,j,start,inc,end,p1,p2,p3;
	int saved_af=active_frame;
	static int saved_disp=display;
	int saved_mac;
	int button;
	CString s;
	CValues params;
	str1.LoadString(IDS_STRING458);
	s.Format(str1,macro[mac].name);
	progress.AddString(s);

	if(nest)
	{
		str1.LoadString(IDS_STRING459);
		s.Format(str1,value);
		progress.AddString(s);
	}

	CZMacro this_macro;
	this_macro=macro[mac];

	int macparam,saved_macparams[15];
	for(i=0; i<12; i++) saved_macparams[i]=macparams[i].value;

	macro_mode=true;

	static bool redraw_pic=true;
	if(nest==0) redraw_pic=false;

	macparams[12].value=w;
	macparams[13].value=h;
	macparams[16].value=max_slots;

	bool usemp1,usemp2,usemp3;

	reset_newname=newname;

	for(i=0; i<24; i++)
	{
		buissy=true;
		progress.BringWindowToTop();
		progress.SetForegroundWindow();
		progress.SetFocus();

		if(frame1>=top_frame) frame1=top_frame;
		if(frame2>=top_frame) frame2=top_frame;
		if(frame1<0) frame1=0;
		if(frame2<0) frame2=0;

		if((progress.m_cancel) || abort)
		{
			progress.m_cancel=false;
			if((nest==0) && (i!=23))
			{
				i=22;
				abort=false;
				continue;
			}
			if(nest)
			{
				abort=true;
				return;
			}
			break;
		}
	
		if(this_macro.cmd[i].n==-1) continue;

		usemp1=usemp2=usemp3=false;
		if(((this_macro.cmd[i].f&MAC_SUBP1)==0) && (MAC_P1V(this_macro.cmd[i].f)!=0)) usemp1=true;
		if(((this_macro.cmd[i].f&MAC_SUBP2)==0) && (MAC_P2V(this_macro.cmd[i].f)!=0)) usemp2=true;
		if(((this_macro.cmd[i].f&MAC_SUBP3)==0) && (MAC_P3V(this_macro.cmd[i].f)!=0)) usemp3=true;

		if(usemp1 || usemp2 || usemp3)
		{
			macparams[14].value=top_used_frame;
			macparams[15].value=top_frame;
			macparams[17].value=active_frame;
			macparams[18].value=active_frame2;
			macparams[19].value=used_frame[last_used_frame-1];
			macparams[20].value=used_frame[bottom_used_frame];
			macparams[21].value=(int)plane_mode;
			macparams[22].value=(int)fix_active;
			macparams[23].value=(int)depth_flag;
			macparams[24].value=(int)show_fixed;
			macparams[25].value=(int)mag.bottom;
			macparams[26].value=(int)mag.left;
			macparams[27].value=(int)mag.right;
			macparams[28].value=(int)mag.top;
			macparams[29].value=(int)Sizer.length34;
			macparams[30].value=(int)Sizer.length12;
			macparams[31].value=Sizer.angle;
			macparams[32].value=(int)(Sizer.ratio*1000.0);
			macparams[33].value=Sizer.p[0].f;
			macparams[34].value=(int)Sizer.p[0].valid;
			macparams[35].value=Sizer.p[0].x;
			macparams[36].value=Sizer.p[0].y;
			macparams[37].value=Sizer.p[1].f;
			macparams[38].value=(int)Sizer.p[1].valid;
			macparams[39].value=Sizer.p[1].x;
			macparams[40].value=Sizer.p[1].y;
			macparams[41].value=Sizer.p[2].f;
			macparams[42].value=(int)Sizer.p[2].valid;
			macparams[43].value=Sizer.p[2].x;
			macparams[44].value=Sizer.p[2].y;
			macparams[45].value=Sizer.p[3].f;
			macparams[46].value=(int)Sizer.p[3].valid;
			macparams[47].value=Sizer.p[3].x;
			macparams[48].value=Sizer.p[3].y;
			macparams[49].value=angle;
			macparams[50].value=xorg;
			macparams[51].value=yorg;
			macparams[52].value=(int)(scale*1000.0);
			macparams[53].value=smallw;
			macparams[54].value=smallh;
			macparams[55].value=sizexorg;
			macparams[56].value=sizeyorg;
			macparams[57].value=offsetx;
			macparams[58].value=offsety;
			macparams[59].value=frame_border;
			macparams[60].value=(int)goodrect.bottom;
			macparams[61].value=(int)goodrect.top;
			macparams[62].value=(int)goodrect.left;
			macparams[63].value=(int)goodrect.right;
			macparams[64].value=brush_size;
			macparams[65].value=brush_delta;
			macparams[66].value=(int)buffer[active_frame].hidden;
			macparams[67].value=Tlt;
			macparams[68].value=Tle;
			macparams[69].value=Teq;
			macparams[70].value=Tge;
			macparams[71].value=Tgt;
			macparams[72].value=Tne;
			macparams[73].value=size_factor;
			macparams[74].value=frame_similarity;
			macparams[75].value=maxangle;
			macparams[76].value=(int)bandw;
			macparams[77].value=contrast_delta;
			macparams[78].value=(int)(rfactor*1000.0);
			macparams[79].value=(int)(gfactor*1000.0);
			macparams[80].value=(int)(bfactor*1000.0);
			macparams[81].value=(int)(correction_factor*1000.0);
			macparams[82].value=(int)(add_factor*1000.0);
			macparams[83].value=(int)(blend_factor*1000.0);
			macparams[84].value=noise_level;
			macparams[85].value=min_island_size;
			macparams[86].value=skew_amount;
			macparams[87].value=patch_size;
			macparams[88].value=jpeg_comp;
			macparams[89].value=light_threshold;
			macparams[90].value=dark_threshold;
			macparams[91].value=patch_threshold;
			macparams[92].value=hdr_bright;
			macparams[93].value=hdr_hia;
			macparams[94].value=hdr_log;
			macparams[95].value=Extract_redend;
			macparams[96].value=Extract_bluend;
			macparams[97].value=Extract_th;
			macparams[98].value=Squeeze_redend;
			macparams[99].value=Squeese_bluend;
			macparams[100].value=Squeeze_break;
			macparams[101].value=Noise_detect;
			macparams[102].value=Noise_sub;
			macparams[103].value=Noise_lim;
			macparams[104].value=detail_size;
			macparams[105].value=detail_filter;
			macparams[106].value=pnumber;
			macparams[107].value=plevels;
			macparams[108].value=pframe;
			macparams[109].value=pnoise;
			macparams[110].value=skew_amountv;
		}

		if((this_macro.cmd[i].f&MAC_SUBP1) && nest) p1=value;
		else p1=this_macro.cmd[i].p1;
		if(p1<-1) p1++;
		if(usemp1)
			p1=macparams[MAC_P1V(this_macro.cmd[i].f)].value;

		if((this_macro.cmd[i].f&MAC_SUBP2) && nest) p2=value;
		else p2=this_macro.cmd[i].p2;
		if(p2<-1) p2++;
		if(usemp2)
			p2=macparams[MAC_P2V(this_macro.cmd[i].f)].value;

		if((this_macro.cmd[i].f&MAC_SUBP3) && nest) p3=value;
		else p3=this_macro.cmd[i].p3;
		if(p3<-1) p3++;
		if(usemp3)
		{
			macparam=MAC_P3V(this_macro.cmd[i].f);
			p3=macparams[macparam].value;
		}

		if((this_macro.cmd[i].f&MAC_PAUSE) && (pause_enabled))
		{
			progress.AddString(IDS_STRING460);
			params.this_macro=mac;
			params.command=i;
			int n=this_macro.cmd[i].n;
			s.LoadString(n);
			if((n>=IDS_MACRO1) && (n<=IDS_MACRO10)) s=macro[n-IDS_MACRO1].name;
			params.m_command=this_macro.name+" "+s;
			params.m_p1=p1;
			params.m_p2=p2;
			params.m_p3=p3;
			button=params.DoModal();
			if(button==IDC_CANCEL)
			{
				progress.AddString(IDS_STRING461);
				continue;
			}
			if(button==IDC_END)
			{
				progress.AddString(IDS_STRING462);
				progress.m_cancel=true;
				continue;
			}
			this_macro.cmd[i].p1=p1=params.m_p1;
			this_macro.cmd[i].p2=p2=params.m_p2;
			this_macro.cmd[i].p3=p3=params.m_p3;
		}

		if(this_macro.cmd[i].f&MAC_TONAME)
		{
			if(this_macro.cmd[i].p3!=-1)
			{
				if((this_macro.cmd[i].n>=IDS_MACRO1) && (this_macro.cmd[i].n<=IDS_MACRO10))
					s.Format(L"-%s[%d,%d,%d]",
						macro[this_macro.cmd[i].n-IDS_MACRO1].name,p1,p2,p3);
				else
					s.Format(L"-%s[%d,%d,%d]",
						short_name[this_macro.cmd[i].n-IDS_SAVEALL],p1,p2,p3);
			} else {
				if(this_macro.cmd[i].p2!=-1)
				{
					s.Format(L"-%s(%d,%d)",
						short_name[this_macro.cmd[i].n-IDS_SAVEALL],p1,p2);
				} else {
					if(this_macro.cmd[i].p1!=-1)
					{
						s.Format(L"-%s(%d)",
							short_name[this_macro.cmd[i].n-IDS_SAVEALL],p1);
					} else {
						s.Format(L"-%s",short_name[this_macro.cmd[i].n-IDS_SAVEALL]);
					}
				}
			}
			newname+=s;
		}		
		active_frame=saved_af;
		saved_disp=display;
		buissy=true;
		switch(this_macro.cmd[i].n)
		{
		case IDS_EMPTYSTACK:
			OnFileEmptystack();
			break;
		case IDS_MAKEMOVIE:
			CHECK_TOP
			movw=w;
			movh=h;
			movq=100;
			movt=200;
			MovieMake();
			break;
		case IDS_SAVECLIPPEDFRAMES:
			CHECK_TOP
			OnMovieExportmovieframes();
			break;
		case IDS_LOADSPECIAL:
			loadspecial(p1);
			break;
		case IDS_SAVESPECIAL:
			CHECK_TOP
			savespecial(p1);
			break;
		case IDS_CAPTOFILE:
			OnAcquireimageCapturemovietofile();
			extract=false;
			break;
		case IDS_SHROTSC:
			sizecompensation();
			break;
		case IDS_ALIGNSRM:
			CHECK_TOP
			mode=0;
			maxangle=p1;
			shrotalignt();
			break;
		case IDS_STARTVIDCAP:
			path=p;
			startvidcap(L"\\CapturedMovie");
			extract=true;
			break;
		case IDS_ENDVIDCAP:
			macro_mode=true;
			endvidcap();
			if(extract) extractgrabbed();
			display=saved_disp=saved_af=active_frame=top_frame-1;
			break;
		case IDS_RESETNAME:
			newname="";
			break;
		case IDS_PADDMAXUF:
			progress.SetRange(0,last_used_frame-1);
			progress.AddString(IDS_STRING642);
			for(j=0; j<last_used_frame; j++)
			{
				progress.SetPos(j);
				PyramidMax(used_frame[j]);
			}
			progress.SetPos(0);
			break;
		case IDS_PADDWAUF:
			progress.AddString(IDS_STRING643);
			progress.SetRange(0,last_used_frame-1);
			for(j=0; j<last_used_frame; j++)
			{
				progress.SetPos(j);
				PyramidWeight(used_frame[j]);
			}
			progress.SetPos(0);
			break;
		case IDS_PADDWEIGHT:
			pframe=p1;
			progress.AddString(IDS_PADDWEIGHT,p1);
			PyramidWeight(p1);
			break;
		case IDS_PWOUT:
			progress.AddString(IDS_PWOUT,p1,p2,p3);
			pforeground=p1;
			pbrightness=p2;
			pbackground=p3;
			plevels=plast_level+1;
			PyramidWToOut();
			break;
		case IDS_PDEPTH:
			progress.AddString(IDS_PDEPTH,p1,p2);
			if(p1<=plast_level) plevels=p1;
			else plevels=plast_level;
			pnoise=p2;
			PyramidDepth(plevels,pnoise<<4);
			break;
		case IDS_PCLEAR:
			progress.AddString(IDS_PCLEAR);
			PyramidClear();
			break;
		case IDS_PADDMAX:
			pframe=p1;
			progress.AddString(IDS_PADDMAX,p1);
			PyramidMax(p1);
			break;
		case IDS_POUT:
			progress.AddString(IDS_POUT,p1,p2,p3);
			pforeground=p1;
			pbrightness=p2;
			pbackground=p3;
			plevels=plast_level+1;
			PyramidToOut();
			break;
		case IDS_ENDMACRO:
			progress.AddString(IDS_ENDMACRO,p1);
			if(p1==1) i=25;
			break;
		case IDS_COMPF:
			progress.AddString(IDS_COMPF);
			compare_frames();
			str1.LoadString(IDS_STRING463);
			s.Format(str1,frame_similarity);
			progress.AddString(s);
			break;
		case IDS_SPLITHSG:
			OnFrameSplithsg();
			break;
		case IDS_COLOURPROD:
			OnFrameColourproduct();
			saved_disp=display=OUT_BUFFER;
			break;
		case IDS_STRETCHCOLOURS:
			OnFrameStretchcolours();
			saved_disp=display=OUT_BUFFER;
			break;
		case IDS_EXTRACTRANGE:
			Extract_redend=p1;
			Extract_bluend=p2;
			Extract_th=p3;
			OnFrameExtractcolourrange();
			saved_disp=display=OUT_BUFFER;
			break;
		case IDS_ADJUSTSPECTRUM:
			Squeeze_redend=p1;
			Squeese_bluend=p2;
			Squeeze_break=p3;
			OnFrameSqueezespectrum();
			saved_disp=display=OUT_BUFFER;
			break;
		case IDS_REDUCENOISE:
			Noise_detect=p1;
			Noise_lim=p2;
			Noise_sub=p3;
			OnFrameReducenoise();
			saved_disp=display=OUT_BUFFER;
			break;
		case IDS_RAINBOW:
			OnColourRainbow();
			saved_disp=display=OUT_BUFFER;
			break;
		case IDS_NEWUSED:
			display=OUT_BUFFER;
			macronewframe(newname);
			buffer[top_frame-1].hidden=false;
			break;
		case IDS_NEWDPTH:
			OnFrameNewfromdepthmap();
			macronewfromdepthmap(newname);
			break;
		case IDS_INVERT:
			OnFrameInvert();
			saved_disp=display=OUT_BUFFER;
			break;
		case IDS_SPLITRGB:
			OnFrameSplitrgb();
			break;
		case IDS_ACTIVEOUT:
			silent_save_active(p1);
			break;
		case IDS_ASTROALIGN:
			maxangle=p1;
			Astroalignt();
			break;
		case IDS_BALBRIGHT:
			balancebrightness();
			break;
		case IDS_BALCOLOUR:
			localbalancecolour();
			break;
		case IDS_HDRAVERAGE:
			hdr_log=p1;
			hdr_hia=p2;
			hdr_bright=p3;
			stackaverage();
			saved_disp=display=OUT_BUFFER;
			break;
		case IDS_TEST:
			progress.AddString(IDS_TEST,p1,p2);
			if(p1==p2){Tlt=2; Tle=1; Teq=1; Tge=1; Tgt=2; Tne=2; break;} 
			if(p1< p2){Tlt=1; Tle=1; Teq=2; Tge=2; Tgt=2; Tne=1; break;} 
			Tlt=2; Tle=2; Teq=2; Tge=1; Tgt=1; Tne=1; break;
		case IDS_PADD:
			if((macparam<1) || (macparam>=LAST_MACPARAM))
			{
				progress.AddString(IDS_STRING464);
				break;
			}
			macparams[macparam].value=p1+p2;
			progress.AddString(IDS_PADD);
			s.Format(L"    %s=%d+%d=%d",macparams[macparam].name,p1,p2,p1+p2);
			progress.AddString(s);
			update_macparams();
			redraw_pic=true;
			break;
		case IDS_PSUB:
			if((macparam<1) || (macparam>=LAST_MACPARAM))
			{
				progress.AddString(IDS_STRING465);
				break;
			}
			macparams[macparam].value=p1-p2;
			progress.AddString(IDS_PSUB);
			s.Format(L"    %s=%d-%d=%d",macparams[macparam].name,p1,p2,p1-p2);
			progress.AddString(s);
			update_macparams();
			redraw_pic=true;
			break;
		case IDS_PMULT:
			if((macparam<1) || (macparam>=LAST_MACPARAM))
			{
				progress.AddString(IDS_STRING466);
				break;
			}
			macparams[macparam].value=p1*p2;
			progress.AddString(IDS_PMULT);
			s.Format(L"    %s=%d*%d=%d",macparams[macparam].name,p1,p2,p1*p2);
			progress.AddString(s);
			update_macparams();
			redraw_pic=true;
			break;
		case IDS_PDIV:
			if((macparam<1) || (macparam>=LAST_MACPARAM))
			{
				progress.AddString(IDS_STRING467);
				break;
			}
			progress.AddString(IDS_PDIV);
			if(p2!=0)
			{
				macparams[macparam].value=p1/p2;
				s.Format(L"    %s=%d/%d=%d",macparams[macparam].name,p1,p2,p1/p2);
				progress.AddString(s);
			}
			else
			{
				macparams[macparam].value=999999999;
				str1.LoadString(IDS_STRING468);
				s.Format(L"    %s=%d/%d=%s",macparams[macparam].name,p1,p2,str1);
				progress.AddString(s);
			}
			update_macparams();
			redraw_pic=true;
			break;
		case IDS_PAUSEMPI:
			if(theApp.suppress_switch) break;

			progress.AddString(IDS_PAUSEMPI);
			progress.GetWindowPlacement(&ppl);
			progress.ShowWindow(SW_HIDE);

			if(!video_showing)
			{
				buissy=false;
				macro_mode=false;
				AfxGetMainWnd()->ShowWindow(SW_SHOW);
				AfxGetMainWnd()->SetForegroundWindow();
				AfxGetMainWnd()->BringWindowToTop();
				AfxGetMainWnd()->SetFocus();
				RefreshScreen();
			}

			initmacparamdlg(&paramsdlg,false);
			paramsdlg.r=saved_r;
			if(paramsdlg.DoModal()==IDCANCEL) i=25;
			getmacparamsfromdlg(&paramsdlg);
			if(ppl.showCmd!=0) progress.ShowWindow(SW_SHOW);
			saved_r=paramsdlg.r;

			if(!video_showing)
			{
				macro_mode=true;
				buissy=true;
				AfxGetMainWnd()->ShowWindow(SW_HIDE);
			}
			break;
		case IDS_CLEAN_START:
			progress.AddString(IDS_CLEAN_START);
			if(top_frame<=0) break;
			bandw=false;
			mode=0;
			full_mode=true;
			mag=full;
			dft.out_mode=OUT_NORMAL;
			dft.valid_filter=false;
			dft.filtering=false;
			OnClear();
			refresh_used_list();
			break;
		case IDS_CHECK_DEPTH:
			CHECK_TOP
			checkdepth();
			progress.AddString(IDS_CHECK_DEPTH);
			redraw_pic=true;
			saved_disp=display=PICTURE_BUFFER;
			break;
		case IDS_BANDWSW:
			bandw=true;
			progress.AddString(IDS_BANDWSW);
			break;
		case IDS_EXCOM1:
		case IDS_EXCOM2:
		case IDS_EXCOM3:
		case IDS_EXCOM4:
		case IDS_EXCOM5:
		case IDS_EXCOM6:
		case IDS_EXCOM7:
		case IDS_EXCOM8:
		case IDS_EXCOM9:
		case IDS_EXCOM10:
			DoExtCom(this_macro.cmd[i].n-IDS_EXCOM1,p1,p2,p3);
			break;
		case IDS_TAKEPIC:
			OnAquireimageGetimage();
			display=saved_disp=saved_af=active_frame=top_frame-1;
			break;
		case IDS_STARTSECOND:
			progress.AddString(IDS_STARTSECOND,p1);
			StartTimer((__int64)p1);
			break;
		case IDS_STARTMINUTE:
			progress.AddString(IDS_STARTMINUTE,p1);
			StartTimer((__int64)(60*1000*p1));
			break;
		case IDS_STARTHOUR:
			progress.AddString(IDS_STARTHOUR,p1);
			StartTimer((__int64)(60*60*1000*p1));
			break;
		case IDS_WAIT:
			progress.AddString(IDS_WAIT);
			WaitForTimer();
			break;
		case IDS_PICTURE_TO_FRAME:
			CHECK_TOP
			if(redraw_pic)
			{
				redraw_picture();
				redraw_pic=false;
			}
			display=PICTURE_BUFFER;
			Replace();
			display=active_frame;
			saved_disp=active_frame;
			break;
		case IDS_PASTE_OVER:
			CHECK_TOP
			OnEditCopy();
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_PASTE_UNDER:
			CHECK_TOP
			OnCopyunder();
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_CLEAR_ALL:
			CHECK_TOP
			OnClear();
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_CLEAR_UNFIXED:
			CHECK_TOP
			OnClearunfixed();
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_MAKE_TRANSPARENT:
			CHECK_TOP
			OnSpecialMaketransparent();
			display=active_frame;
			saved_af=saved_disp=display;
			break;
		case IDS_INVERT_FIXED:
			CHECK_TOP
			OnRectangleInvertfixed();
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_GOTO_FRAME_REL:
			CHECK_TOP
			active_frame+=p1;
			if((active_frame>=top_frame) || (active_frame<0)) end_macro=true;
			saved_af=saved_disp=display=active_frame;
			str1.LoadString(IDS_STRING469);
			s.Format(str1,saved_af);
			progress.AddString(s);
			break;
		case IDS_GOTO_TOP_FRAME:
			saved_af=saved_disp=display=active_frame=top_frame-1;
			str1.LoadString(IDS_STRING470);
			s.Format(str1,saved_af);
			progress.AddString(s);
			break;
		case IDS_REMEMBER_FRAME1:
			CHECK_TOP
			frame1=active_frame;
			str1.LoadString(IDS_STRING471);
			s.Format(str1,active_frame);
			progress.AddString(s);
			break;
		case IDS_REMEMBER_FRAME2:
			CHECK_TOP
			frame2=active_frame;
			str1.LoadString(IDS_STRING472);
			s.Format(str1,active_frame);
			progress.AddString(s);
			break;
		case IDS_GOTO_FRAME1:
			CHECK_TOP
			saved_af=saved_disp=display=active_frame=frame1;
			str1.LoadString(IDS_STRING473);
			s.Format(str1,frame1);
			progress.AddString(s);
			if((active_frame>=top_frame) || (active_frame<0)) end_macro=true;
			break;
		case IDS_GOTO_FRAME2:
			CHECK_TOP
			saved_af=saved_disp=display=active_frame=frame2;
			str1.LoadString(IDS_STRING474);
			s.Format(str1,frame2);
			progress.AddString(s);
			if((active_frame>=top_frame) || (active_frame<0)) end_macro=true;
			break;
		case IDS_USE_FRAME:
			if(top_frame==0) break;
			buffer[active_frame].hidden=false;
			refresh_used_list();
			str1.LoadString(IDS_STRING475);
			s.Format(str1,active_frame);
			progress.AddString(s);
			break;
		case IDS_DONT_USE_FRAME:
			if(top_frame==0) break;
			buffer[active_frame].hidden=true;
			refresh_used_list();
			str1.LoadString(IDS_STRING476);
			s.Format(str1,active_frame);
			progress.AddString(s);
			break;
		case IDS_USE_ALL_FRAMES:
			if(top_frame==0) break;
			for(j=0; j<top_frame; j++)
			{
				buffer[j].hidden=false;
				used_frame[j]=j;
			}
			top_used_frame=top_frame;
			refresh_used_list();
			progress.AddString(IDS_STRING477);
			break;
		case IDS_DONT_USE_ABOVE:
			if(top_frame==0) break;
			if(active_frame==top_frame-1) break;
			for(j=active_frame+1; j<top_frame; j++) buffer[j].hidden=true;
			refresh_used_list();
			str1.LoadString(IDS_STRING478);
			s.Format(str1,active_frame);
			progress.AddString(s);
			break;
		case IDS_DONT_USE_BELOW:
			if(top_frame==0) break;
			if(active_frame==0) break;
			for(j=active_frame-1; j>=0; j--) buffer[j].hidden=true;
			refresh_used_list();
			str1.LoadString(IDS_STRING479);
			s.Format(str1,active_frame);
			progress.AddString(s);
			break;
		case IDS_INSERT_ABOVE:
			CHECK_TOP
			if(active_frame==MAX_FRAMES-1)
			{
				progress.AddString(IDS_STRING480);
				break;
			}
			InsertAbove(newname);
			backup_picture_valid=false;
			redraw_pic=true;
			str1.LoadString(IDS_STRING481);
			s.Format(str1,active_frame+1);
			progress.AddString(s);
			if(active_frame2>active_frame) active_frame2++;
			if(frame1>active_frame) frame1++;
			if(frame2>active_frame) frame2++;
			saved_af=saved_disp=display=active_frame;
			break;
		case IDS_INSERT_BELOW:
			CHECK_TOP
			if(active_frame==MAX_FRAMES-1)
			{
				progress.AddString(IDS_STRING480);
				break;
			}
			str1.LoadString(IDS_STRING481);
			s.Format(str1,active_frame);
			progress.AddString(s);
			InsertBelow(newname);
			backup_picture_valid=false;
			redraw_pic=true;
			if(active_frame2>active_frame-1) active_frame2++;
			if(frame1>active_frame-1) frame1++;
			if(frame2>active_frame-1) frame2++;
			saved_af=saved_disp=display=active_frame;
			break;
		case IDS_RESTART_MACRO:
			i=-1;
			str1.LoadString(IDS_STRING482);
			s.Format(str1);
			progress.AddString(s);
			newname=reset_newname;
			break;
		case IDS_COPY_TO_OUT:
			CHECK_TOP
			CopyToOut();
			display=active_frame;
			break;
		case IDS_REMOVE:
			CHECK_TOP
			mode=0;
			display=active_frame;
			remove();
			if(saved_af>top_frame-1) saved_af=saved_disp=top_frame-1;
			display=active_frame=saved_af;
			backup_picture_valid=false;
			redraw_pic=true;
			break;
		case IDS_ACTIVE2:
			CHECK_TOP
			mode=0;
			str1.LoadString(IDS_STRING483);
			s.Format(str1,active_frame);
			progress.AddString(s);
			OnSpecialMakevisibleactiveframe2();
			break;
		case IDS_CORRECT_BRIGHTNESS:
			CHECK_TOP
			mode=0;
			correction_factor=((double)p1)/1000.0;
			progress.AddString(IDS_CORRECT_BRIGHTNESS,(int)(correction_factor*1000.0),p2);
			correct(p2);
			backup_picture_valid=false;
			redraw_pic=true;
			saved_disp=display=OUT_BUFFER;
			break;
		case IDS_ADD_RECTANGLES:
			CHECK_TOP
			mode=0;
			add_factor=-((double)p1)/1000.0;
			progress.AddString(IDS_ADD_RECTANGLES,(int)(-add_factor*1000.0));
			add();
			backup_picture_valid=false;
			redraw_pic=true;
			saved_disp=display=OUT_BUFFER;
			break;
		case IDS_BLEND_RECTANGLES:
			CHECK_TOP
			mode=0;
			blend_factor=((double)p1)/1000.0;
			progress.AddString(IDS_BLEND_RECTANGLES,(int)(blend_factor*1000.0));
			blend();
			backup_picture_valid=false;
			redraw_pic=true;
			saved_disp=OUT_BUFFER;
			break;
		case IDS_SKEW:
			CHECK_TOP
			mode=0;
			skew(p1,p2);
			backup_picture_valid=false;
			redraw_pic=true;
			break;
		case IDS_BANDW:
			CHECK_TOP
			mode=0;
			progress.AddString(IDS_BANDW);
			monochrome();
			backup_picture_valid=false;
			redraw_pic=true;
			break;
		case IDS_SELECTALL:
			CHECK_TOP
			mode=0;
			progress.AddString(IDS_SELECTALL);
			full_mode=true;
			mag=full;
			break;
		case IDS_SELECTNEXTRECT:
			CHECK_TOP
			mode=0;
			progress.AddString(IDS_SELECTNEXTRECT);
			selectall();
			redraw_pic=true;
			break;
		case IDS_FIX:
			CHECK_TOP
			mode=0;
			progress.AddString(IDS_FIX);
			fix();
			redraw_pic=true;
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_UNFIX:
			CHECK_TOP
			mode=0;
			progress.AddString(IDS_UNFIX);
			unfix();
			redraw_pic=true;
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_SAVEALL:
			CHECK_TOP
			mode=0;
			progress.AddString(IDS_SAVEALL);
			saveallframes();
			break;
		case IDS_RELOADALL:
			CHECK_TOP
			mode=0;
			progress.AddString(IDS_RELOADALL);
			for(j=0; j<top_frame; j++)
			{
				if(buffer[j].hidden) continue;
				display=active_frame=j;
				reload();
			}
			bandw=false;
			backup_picture_valid=false;
			redraw_picture();
			break;
		case IDS_REVERSE:
			CHECK_TOP
			mode=0;
			reverseorder();
			break;
		case IDS_BALANCE:
			CHECK_TOP
			mode=0;
			balance();
			break;
		case IDS_LOCALBALANCE:
			CHECK_TOP
			mode=0;
			localbalance();
			break;
		case IDS_AUTOALIGN1:
			CHECK_TOP
			mode=0;
			Autoalignandsizet();
			break;
		case IDS_AUTOALIGN2:
			CHECK_TOP
			mode=0;
			Autoalignandsize2t();
			break;
		case IDS_RESIZEALL:
			CHECK_TOP
			mode=0;
			size_factor=p1;
			fbase=(float)(10000/size_factor);
			resizeall();
			break;
		case IDS_GROUP:
			CHECK_TOP
			mode=0;
			group=p1;
			decimate();
			saved_af=saved_disp=active_frame;
			break;
		case IDS_DETAIL:
			CHECK_TOP
			mode=0;
			noise_level=p1;
			detail_size=p2;
			detail_filter=p3;
			detail();
			display=PICTURE_BUFFER;
			redraw_pic=true;
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_PATCHWORK:
			CHECK_TOP
			mode=0;
			patch_size=p1;
			patch_threshold=p2;
			patch();
			display=PICTURE_BUFFER;
			redraw_pic=true;
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_LIGHTDARK:
			CHECK_TOP
			mode=0;
			light_threshold=p1;
			dark_threshold=p2;
			lightdark();
			display=PICTURE_BUFFER;
			redraw_pic=true;
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_GAPS:
			CHECK_TOP
			mode=0;
			DoDepthflow();
			display=PICTURE_BUFFER;
			redraw_pic=true;
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_ISLANDS:
			CHECK_TOP
			mode=0;
			min_island_size=p1;
			removeislands();
			display=PICTURE_BUFFER;
			redraw_pic=true;
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_STEPS:
			CHECK_TOP
			if(redraw_pic)
			{
				redraw_picture();
				redraw_pic=false;
			}
			mode=0;
			removesteps();
			display=PICTURE_BUFFER;
			redraw_pic=true;
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_INTERPOLATE:
			CHECK_TOP
			mode=0;
			StackInterpolate();
			saved_disp=OUT_BUFFER;
			break;
		case IDS_NEWFROMPICTURE:
			CHECK_TOP
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
			saved_af=active_frame;
			display=OUT_BUFFER;
			macronewframe(newname);
			break;
		case IDS_REPLACEFRAME:
			CHECK_TOP
			mode=0;
			display=OUT_BUFFER;
			Replace();
			saved_af=saved_disp=active_frame;
			break;
		case IDS_RGB:
			CHECK_TOP
			mode=0;
			rfactor=(float)p1/1000.0f;
			gfactor=(float)p2/1000.0f;
			bfactor=(float)p3/1000.0f;
			adjustcolour();
			progress.AddString(IDS_RGB,(int)(rfactor*1000.0f),(int)(gfactor*1000.0f),(int)(bfactor*1000.0f));
			mode=BRIGHTNESSX;
			saved_disp=OUT_BUFFER;
			break;
		case IDS_CONTRAST:
			CHECK_TOP
			mode=0;
			contrast_delta=p1;
			contrast();
			progress.AddString(IDS_CONTRAST,contrast_delta);
			mode=CONTRASTX;
			saved_disp=OUT_BUFFER;
			break;
		case IDS_DFTACTIVE:
			CHECK_TOP
			FilterActive();
			display=OUT_BUFFER;
			progress.AddString(IDS_DFTACTIVE);
			saved_disp=OUT_BUFFER;
			break;
		case IDS_FILTER:
			CHECK_TOP
			Refilter();
			display=OUT_BUFFER;
			saved_disp=OUT_BUFFER;
			break;
		case IDS_DFTSTACK:
			CHECK_TOP
			FilterStack();
			display=OUT_BUFFER;
			saved_disp=OUT_BUFFER;
			break;
		case IDS_DFTWEIGHTED:
			CHECK_TOP
			FilterWeighted();
			display=OUT_BUFFER;
			saved_disp=OUT_BUFFER;
			break;
		case IDS_WEIGHTEDWC:
			CHECK_TOP
			FilterWeightedWC();
			display=OUT_BUFFER;
			saved_disp=OUT_BUFFER;
			break;
		case IDS_DFTDEPTH:
			CHECK_TOP
			FilterDM();
			display=OUT_BUFFER;
			progress.AddString(IDS_DFTDEPTH);
			saved_disp=OUT_BUFFER;
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
			saved_disp=OUT_BUFFER;
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
			saved_disp=OUT_BUFFER;
			break;
		case IDS_FINISH:
			progress.AddString(IDS_STRING484);
			mode=0;
			break;
		case IDS_DFTALLFRAMES:
			CHECK_TOP
			filterallframes();
			display=active_frame;
			saved_af=saved_disp=active_frame;
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
			saved_disp=OUT_BUFFER;
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
			saved_disp=OUT_BUFFER;
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
			saved_disp=OUT_BUFFER;
			break;
		case IDS_FANDRDEPTHMAP:
			CHECK_TOP
			FilterDM();
			progress.AddString(IDS_DFTDEPTH);
			dftouttodepth();
			backup_picture_valid=false;
			redraw_picture();
			display=PICTURE_BUFFER;
			mode=0;
			redraw_pic=true;
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_DFTOUTNORMAL:
			dft.out_mode=OUT_NORMAL;
			progress.AddString(IDS_DFTOUTNORMAL);
			saved_disp=OUT_BUFFER;
			break;
		case IDS_DFTOUTBRIGHTEN:
			dft.out_mode=OUT_BRIGHT;
			dft.brightness=(float)p1/100.0f;
			progress.AddString(IDS_DFTOUTBRIGHTEN,this_macro.cmd[i].p1);
			saved_disp=OUT_BUFFER;
			break;
		case IDS_DFTOUTTHRESHOLD:
			dft.out_mode=OUT_THRESHOLD;
			dft.threshold=(float)p1;
			progress.AddString(IDS_DFTOUTTHRESHOLD,this_macro.cmd[i].p1);
			saved_disp=OUT_BUFFER;
			break;
		case IDS_SAVEDEPTH:
			CHECK_TOP
			progress.AddString(IDS_SAVEDEPTH);
			savedepth();
			break;
		case IDS_LOADDEPTH:
			CHECK_TOP
			progress.AddString(IDS_LOADDEPTH);
			loaddepth();
			redraw_pic=true;
			saved_disp=PICTURE_BUFFER;
			break;
		case IDS_GOTOFRAME:
			CHECK_TOP
			saved_af=saved_disp=p1;
			if((saved_af>=top_frame) || (saved_af<0)) end_macro=true;
			if(saved_af<0) saved_af=0;
			if(saved_af>top_frame-1) saved_af=top_frame-1;
			active_frame=display=saved_disp=saved_af;
			s.Format(L"Go to Frame(%d)",saved_af);
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

			mac=this_macro.cmd[i].n-IDS_MACRO1;
			if(this_mac==mac)
			{
				CString s1,s2;
				s1.LoadString(IDS_ERRORMACCALL); s2.LoadString(IDS_MACRO);
				MessageBox(progress.m_hWnd,s1,s2,MB_OK);
				mac=this_mac;
				progress.AddString(IDS_STRING485);
				break;
			}

			saved_mac=this_mac;
			saved_value=value;

			progress.AddString(IDS_STRING486);

			if((progress.m_cancel) || abort)
			{
//				nest--;
				abort=true;
				break;
			}

			if(inc==0)
			{
				saved_newname=newname;
				value=p1;
				nest++;
				display=saved_disp;
				active_frame=saved_af;
				DoMacro();
				saved_af=active_frame;
				nest--;
				newname=saved_newname;
			} else {
				j=start;
				while(j<=end)
				{
					if((progress.m_cancel) || abort)
					{
//						nest--;
						progress.AddString(IDS_STRING487);
						abort=true;
						break;
					}
					saved_newname=newname;
					value=j;
					nest++;
					display=saved_disp;
					active_frame=saved_af;
					DoMacro();
					saved_af=active_frame;
					nest--;
					newname=saved_newname;
					j+=inc;
				};
			}

			progress.AddString(IDS_STRING488);
			value=saved_value;
			mac=saved_mac;
			break;
		}
		if(top_frame>0)
		{
			if(active_frame<0){saved_af=0; saved_disp=saved_af; break;}
			if(active_frame>=top_frame){saved_af=top_frame-1; saved_disp=saved_af; break;}
		}
	}
	newname=reset_newname;
#if !defined W2000
	if(nest==0)
	{
		if(video_showing)
		{
			togglevideo();
		}
		videowindowoff();
	}
#endif

	active_frame=saved_af;
	display=saved_disp;
	if(active_frame<0) active_frame=0;
	if(active_frame>top_frame-1) active_frame=top_frame-1;
	refresh_used_list();
	str1.LoadString(IDS_STRING489);
	s.Format(str1,macro[mac].name);
	progress.AddString(s);
	if(nest) return;
	if(redraw_pic)
	{
		progress.AddString(IDS_STRING490);
		redraw_picture();
	}
	bandw=false;
	buissy=false;
	macro_mode=false;
	display=saved_disp;
	if((display!=PICTURE_BUFFER) && (display!=OUT_BUFFER)) display=active_frame;
	progress.Minimize();
	progress.m_cancel=false;
	abort=false;
	for(i=0; i<12; i++) macparams[i].value=saved_macparams[i];
	AfxGetMainWnd()->RedrawWindow();
	RefreshScreen();
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	AfxGetMainWnd()->SetForegroundWindow();
	AfxGetMainWnd()->BringWindowToTop();
	AfxGetMainWnd()->SetFocus();
	return;
}

bool CCombineDoc::video_test(int m)
{
	int i;
	int n;
	for(i=0; i<24; i++)
	{
		n=macro[m].cmd[i].n;
		if(n==IDS_TAKEPIC) return true;
		if((n==IDS_STARTVIDCAP) || (n==IDS_ENDVIDCAP)) return true;
		if((n>=IDS_MACRO1) && (n<=IDS_MACRO10) && (n!=m+IDS_MACRO1))
		{
			if(video_test(n-IDS_MACRO1)) return true;
		}
	}
	return false;
}

void CCombineDoc::macro_preamble(void)
{
	progress.m_cancel=false;
	if(video_test(mac))
	{
		if(!video_showing) OnAcquireimageTogglevideo();

		if(video_showing)
		{
			macro_mode=true;
			videowindowon();
			video_showing=true;
			macro_mode=false;
		}
	}
}

void CCombineDoc::OnMacro1() 
{
	mac=0;
	macro_preamble();
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro2() 
{
	mac=1;
	macro_preamble();
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro3() 
{
	mac=2;
	macro_preamble();
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro4() 
{
	mac=3;
	macro_preamble();
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro5() 
{
	mac=4;
	macro_preamble();
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro6() 
{
	mac=5;
	macro_preamble();
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro7() 
{
	mac=6;
	macro_preamble();
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro8() 
{
	mac=7;
	macro_preamble();
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro9()
{
	mac=8;
	macro_preamble();
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacro10() 
{
	mac=9;
	macro_preamble();
	AfxBeginThread(DoMacroT,this);
}

void CCombineDoc::OnMacroSaveasdefaultset() 
{
	int i,j;
	wchar_t wc;
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString macpath=helppath.Left(helppath.ReverseFind('\\')+1);
	CFile f;

	str1.LoadString(IDS_STRING491);
	CFileDialog fd(false,L"czm",0,0,str1,0);
	fd.m_ofn.lpstrInitialDir=macpath.GetBuffer(1024);
	str1.LoadString(IDS_STRING492);
	CString temp=str1;
	fd.m_ofn.lpstrFile=temp.GetBuffer(1024);
	if(fd.DoModal()==IDCANCEL) goto finish;

	f.Open(fd.GetPathName(),CFile::modeCreate|CFile::modeWrite);
	for(i=0; i<10; i++)
	{
		f.Write(macro[i].cmd,24*sizeof(CZCommand));
		for(j=0; j<macro[i].name.GetLength(); j++)
		{
			wc=macro[i].name[j];
			f.Write(&wc,2);
		}
		f.Write(L"\0",2);
	}

	for(i=0; i<13; i++)
		f.Write((void*)&extcommand[i],sizeof(CZExtCommand));

	for(i=0; i<12; i++)
		f.Write((void*)&macparams[i],sizeof(CZMacParams));

	f.Close();
finish:
	temp.ReleaseBuffer();
	macpath.ReleaseBuffer();
}

void CCombineDoc::OnMacroLoadmacroset() 
{
	int i,j,k;
	wchar_t wc;

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString macpath=helppath.Left(helppath.ReverseFind('\\')+1);
	CFile f;
	str1.LoadString(IDS_STRING493);
	CFileDialog fd(true,L"czm",0,0,str1,0);
	fd.m_ofn.lpstrInitialDir=macpath.GetBuffer(1024);
	str1.LoadString(IDS_STRING494);
	CString temp=str1;
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
		macro[i].name=L"";
		f.Read(&wc,2);
		while(wc)
		{
			macro[i].name+=wc;
			f.Read(&wc,2);
		}
	}

	for(i=0; i<13; i++)
	{
		f.Read(&extcommand[i],sizeof(CZExtCommand));
	}
	if(wcscmp(extcommand[9].name,L"External Command 10")==0)
	{
		str1.LoadString(IDS_STRING261);
		wcscpy(extcommand[9].name,str1);
		extcommand[9].line[0]=0;
		wcscpy(extcommand[9].program,L"C:\\Program Files\\IrfanView\\i_view32.exe");
	}

	for(i=0; i<13; i++)
	{
		f.Read(&macparams[i],sizeof(CZMacParams));
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
		str1.LoadString(IDS_STRING495);
		AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO_ENABLEPAUSE,MF_BYCOMMAND,ID_MACRO_ENABLEPAUSE,str1);
		pause_enabled=true;
	} else {
		str1.LoadString(IDS_STRING496);
		AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_MACRO_ENABLEPAUSE,MF_BYCOMMAND,ID_MACRO_ENABLEPAUSE,str1);
		pause_enabled=false;
	}
}

void CCombineDoc::macronewframe(CString commands)
{
	if(!top_frame) return;

	top_frame++;
	active_frame=top_frame-1;
	PIXEL* f1=(PIXEL*)GetBuffer(active_frame);
	PIXEL* f0=(PIXEL*)GetBuffer(display);

	CString s;
	str1.LoadString(IDS_STRING497);
	if(display==OUT_BUFFER) buffer[active_frame].name=(newnumber(str1)+commands);
	else buffer[active_frame].name=(newnumber("New")+"-Pic"+commands);
	buffer[active_frame].hidden=true;
	refresh_used_list();

	int i;
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
	str1.LoadString(IDS_STRING498);
	progress.AddString(str1);
	s.Format(L"(%d) %s",top_frame-1,buffer[top_frame-1].name);
	progress.AddString(s);
}

void CCombineDoc::macronewfromdepthmap(CString commands)
{
	if(!top_frame) return;

	int x,y,Y,i,c,max,min;

	CString s;

	progress.AddString(IDS_NEWDPTH);
	progress.AddString(s);

	PIXEL *f1=(PIXEL *)GetBuffer(top_frame);
	buffer[top_frame].name=newnumber("depthmap+commands");
	buffer[top_frame].hidden=true;
	top_frame++;
	s="  "+buffer[top_frame].name;
	progress.AddString(s);

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	max=0; min=0x7fffffff;
	for(y=0; y<(int)h; y++)
	{
		Y=y*(int)w;
		for(x=0; x<(int)w; x++)
		{
			i=x+Y;
			c=(dMASK&depth[i]);
			if(c>max) max=c;
			if(c<min) min=c;
		}
	}

	if(max-min)
	{
		for(y=0; y<(int)h; y++)
		{
			Y=y*(int)w;
			for(x=0; x<(int)w; x++)
			{
				i=x+Y;
				c=(((dMASK&depth[i])-min)*255)/(max-min);
				if(c>255) c=255;
				f1[i].abgr=0;
				f1[i].r=f1[i].g=f1[i].b=(unsigned char)c;
			}
		}
	} else {
		for(y=0; y<(int)h; y++)
		{
			Y=y*(int)w;
			for(x=0; x<(int)w; x++)
			{
				i=x+Y;
				f1[i].abgr=0;
			}
		}
	}

	FreeBuffer(DEPTH_BUFFER);
	UpdateBuffer(top_frame-1);
}

bool CCombineDoc::loaddefaultmacros()
{
	int i,j,k,n;
	wchar_t wc;
	CString s;
	s=theApp.macro_param;
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	str1.LoadString(IDS_STRING499);
	CString macpath=helppath.Left(helppath.ReverseFind('\\')+1)+str1;
	if((s[0]!=0))
	{
		if((n=s.Find('/'))!=-1)
		{
			wcscpy(theApp.macro_param,s.Right(s.GetLength()-n-1));
			s=s.Left(n);
			macpath=helppath.Left(helppath.ReverseFind('\\')+1)+s+L".czm";
		}
	}
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
		macro[i].name=L"";
		f.Read(&wc,2);
		while(wc)
		{
			macro[i].name+=wc;
			f.Read(&wc,2);
		}
	}

	for(i=0; i<13; i++)
	{
		if(f.Read(&extcommand[i],sizeof(CZExtCommand))==0) break;
	}
	if(wcscmp(extcommand[9].name,L"External Command 10")==0)
	{
		str1.LoadString(IDS_STRING261);
		wcscpy(extcommand[9].name,str1);
		extcommand[9].line[0]=0;
		wcscpy(extcommand[9].program,L"C:\\Program Files\\IrfanView\\i_view32.exe");
	}

	for(i=0; i<12; i++)
	{
		if(f.Read(&macparams[i],sizeof(CZMacParams))==0)
		{
			for(j=0; j<10; j++)
			{
				for(k=0; k<24; k++)
				{
					if(macro[j].cmd[k].f&MAC_SUBP1) macro[j].cmd[k].f|=(11<<28);
					if(macro[j].cmd[k].f&MAC_SUBP2) macro[j].cmd[k].f|=(11<<24);
					if(macro[j].cmd[k].f&MAC_SUBP3) macro[j].cmd[k].f|=(11<<20);
				}
			}
			break;
		}
	}

	f.Close();
	return true;
}

void CCombineDoc::savedefaultmacros()
{
	if(theApp.quit_switch) return;

	int i,j;
	wchar_t wc;
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	str1.LoadString(IDS_STRING499);
	CString macpath=helppath.Left(helppath.ReverseFind('\\')+1)+str1;
	CFile f;

	f.Open(macpath,CFile::modeCreate|CFile::modeWrite);
	for(i=0; i<10; i++)
	{
		f.Write(macro[i].cmd,24*sizeof(CZCommand));
		for(j=0; j<macro[i].name.GetLength(); j++)
		{
			wc=macro[i].name[j];
			f.Write(&wc,2);
		}
		f.Write(L"\0",2);
	}

	for(i=0; i<13; i++)
		f.Write((void*)&extcommand[i],sizeof(CZExtCommand));

	for(i=0; i<12; i++)
		f.Write((void*)&macparams[i],sizeof(CZMacParams));

	f.Close();
}

void CCombineDoc::OnMacroClearallmacros() 
{
	int i;
	for(i=0; i<10; i++)
	{
		Clear_macro_commands(i);
	}
	macro[0].name.LoadString(IDS_STRING500);
	macro[1].name.LoadString(IDS_STRING501);
	macro[2].name.LoadString(IDS_STRING502);
	macro[3].name.LoadString(IDS_STRING503);
	macro[4].name.LoadString(IDS_STRING504);
	macro[5].name.LoadString(IDS_STRING505);
	macro[6].name.LoadString(IDS_STRING506);
	macro[7].name.LoadString(IDS_STRING507);
	macro[8].name.LoadString(IDS_STRING508);
	macro[9].name.LoadString(IDS_STRING509);
	InitializeMacros();
	initializecommands();
	initializemacparams();
	savedefaultmacros();
}

bool CCombineDoc::silent_macro(wchar_t *mac_name)
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

void CCombineDoc::MacrobyNumber(int n)
{
	switch(n)
	{
	case 0: OnMacro1(); break;
	case 1: OnMacro2(); break;
	case 2: OnMacro3(); break;
	case 3: OnMacro4(); break;
	case 4: OnMacro5(); break;
	case 5: OnMacro6(); break;
	case 6: OnMacro7(); break;
	case 7: OnMacro8(); break;
	case 8: OnMacro9(); break;
	case 9: OnMacro10(); break;
	}
}

void CCombineDoc::initializemacparams()
{
	int i;
	for(i=1; i<11; i++)
	{
		str1.LoadString(IDS_STRING511);
		wsprintf(macparams[i].name,str1,i);
		macparams[i].value=0;
	}

	str1.LoadString(IDS_STRING510);
	wsprintf(macparams[0].name,str1);
	macparams[0].value=0;

	str1.LoadString(IDS_STRING512);
	wsprintf(macparams[11].name,str1);
	macparams[11].value=0;

	str1.LoadString(IDS_STRING513); wsprintf(macparams[12].name,str1);
	str1.LoadString(IDS_STRING514); wsprintf(macparams[13].name,str1);
	str1.LoadString(IDS_STRING515); wsprintf(macparams[14].name,str1);
	str1.LoadString(IDS_STRING516); wsprintf(macparams[15].name,str1);
	str1.LoadString(IDS_STRING517); wsprintf(macparams[16].name,str1);
	str1.LoadString(IDS_STRING518); wsprintf(macparams[17].name,str1);
	str1.LoadString(IDS_STRING519); wsprintf(macparams[18].name,str1);
	str1.LoadString(IDS_STRING520); wsprintf(macparams[19].name,str1);
	str1.LoadString(IDS_STRING521); wsprintf(macparams[20].name,str1);
	str1.LoadString(IDS_STRING522); wsprintf(macparams[21].name,str1);
	str1.LoadString(IDS_STRING523); wsprintf(macparams[22].name,str1);
	str1.LoadString(IDS_STRING524); wsprintf(macparams[23].name,str1);
	str1.LoadString(IDS_STRING525); wsprintf(macparams[24].name,str1);
	str1.LoadString(IDS_STRING526); wsprintf(macparams[25].name,str1);
	str1.LoadString(IDS_STRING527); wsprintf(macparams[26].name,str1);
	str1.LoadString(IDS_STRING528); wsprintf(macparams[27].name,str1);
	str1.LoadString(IDS_STRING529); wsprintf(macparams[28].name,str1);
	str1.LoadString(IDS_STRING530); wsprintf(macparams[29].name,str1);
	str1.LoadString(IDS_STRING531); wsprintf(macparams[30].name,str1);
	str1.LoadString(IDS_STRING532); wsprintf(macparams[31].name,str1);
	str1.LoadString(IDS_STRING533); wsprintf(macparams[32].name,str1);
	str1.LoadString(IDS_STRING534); wsprintf(macparams[33].name,str1);
	str1.LoadString(IDS_STRING535); wsprintf(macparams[34].name,str1);
	str1.LoadString(IDS_STRING536); wsprintf(macparams[35].name,str1);
	str1.LoadString(IDS_STRING537); wsprintf(macparams[36].name,str1);
	str1.LoadString(IDS_STRING538); wsprintf(macparams[37].name,str1);
	str1.LoadString(IDS_STRING539); wsprintf(macparams[38].name,str1);
	str1.LoadString(IDS_STRING540); wsprintf(macparams[39].name,str1);
	str1.LoadString(IDS_STRING541); wsprintf(macparams[40].name,str1);
	str1.LoadString(IDS_STRING542); wsprintf(macparams[41].name,str1);
	str1.LoadString(IDS_STRING543); wsprintf(macparams[42].name,str1);
	str1.LoadString(IDS_STRING544); wsprintf(macparams[43].name,str1);
	str1.LoadString(IDS_STRING545); wsprintf(macparams[44].name,str1);
	str1.LoadString(IDS_STRING546); wsprintf(macparams[45].name,str1);
	str1.LoadString(IDS_STRING547); wsprintf(macparams[46].name,str1);
	str1.LoadString(IDS_STRING548); wsprintf(macparams[47].name,str1);
	str1.LoadString(IDS_STRING549); wsprintf(macparams[48].name,str1);
	str1.LoadString(IDS_STRING550); wsprintf(macparams[49].name,str1);
	str1.LoadString(IDS_STRING551); wsprintf(macparams[50].name,str1);
	str1.LoadString(IDS_STRING552); wsprintf(macparams[51].name,str1);
	str1.LoadString(IDS_STRING553); wsprintf(macparams[52].name,str1);
	str1.LoadString(IDS_STRING554); wsprintf(macparams[53].name,str1);
	str1.LoadString(IDS_STRING555); wsprintf(macparams[54].name,str1);
	str1.LoadString(IDS_STRING556); wsprintf(macparams[55].name,str1);
	str1.LoadString(IDS_STRING557); wsprintf(macparams[56].name,str1);
	str1.LoadString(IDS_STRING558); wsprintf(macparams[57].name,str1);
	str1.LoadString(IDS_STRING559); wsprintf(macparams[58].name,str1);
	str1.LoadString(IDS_STRING560); wsprintf(macparams[59].name,str1);
	str1.LoadString(IDS_STRING561); wsprintf(macparams[60].name,str1);
	str1.LoadString(IDS_STRING562); wsprintf(macparams[61].name,str1);
	str1.LoadString(IDS_STRING563); wsprintf(macparams[62].name,str1);
	str1.LoadString(IDS_STRING564); wsprintf(macparams[63].name,str1);
	str1.LoadString(IDS_STRING565); wsprintf(macparams[64].name,str1);
	str1.LoadString(IDS_STRING566); wsprintf(macparams[65].name,str1);
	str1.LoadString(IDS_STRING567); wsprintf(macparams[66].name,str1);
	str1.LoadString(IDS_STRING568); wsprintf(macparams[67].name,str1);
	str1.LoadString(IDS_STRING569); wsprintf(macparams[68].name,str1);
	str1.LoadString(IDS_STRING570); wsprintf(macparams[69].name,str1);
	str1.LoadString(IDS_STRING571); wsprintf(macparams[70].name,str1);
	str1.LoadString(IDS_STRING572); wsprintf(macparams[71].name,str1);
	str1.LoadString(IDS_STRING573); wsprintf(macparams[72].name,str1);
	str1.LoadString(IDS_STRING574); wsprintf(macparams[73].name,str1);
	str1.LoadString(IDS_STRING575); wsprintf(macparams[74].name,str1);
	str1.LoadString(IDS_STRING576); wsprintf(macparams[75].name,str1);
	str1.LoadString(IDS_STRING577); wsprintf(macparams[76].name,str1);
	str1.LoadString(IDS_STRING578); wsprintf(macparams[77].name,str1);
	str1.LoadString(IDS_STRING579); wsprintf(macparams[78].name,str1);
	str1.LoadString(IDS_STRING580); wsprintf(macparams[79].name,str1);
	str1.LoadString(IDS_STRING581); wsprintf(macparams[80].name,str1);
	str1.LoadString(IDS_STRING582); wsprintf(macparams[81].name,str1);
	str1.LoadString(IDS_STRING583); wsprintf(macparams[82].name,str1);
	str1.LoadString(IDS_STRING584); wsprintf(macparams[83].name,str1);
	str1.LoadString(IDS_STRING585); wsprintf(macparams[84].name,str1);
	str1.LoadString(IDS_STRING586); wsprintf(macparams[85].name,str1);
	str1.LoadString(IDS_STRING587); wsprintf(macparams[86].name,str1);
	str1.LoadString(IDS_STRING588); wsprintf(macparams[87].name,str1);
	str1.LoadString(IDS_STRING589); wsprintf(macparams[88].name,str1);
	str1.LoadString(IDS_STRING590); wsprintf(macparams[89].name,str1);
	str1.LoadString(IDS_STRING591); wsprintf(macparams[90].name,str1);
	str1.LoadString(IDS_STRING592); wsprintf(macparams[91].name,str1);
	str1.LoadString(IDS_STRING593); wsprintf(macparams[92].name,str1);
	str1.LoadString(IDS_STRING594); wsprintf(macparams[93].name,str1);
	str1.LoadString(IDS_STRING595); wsprintf(macparams[94].name,str1);
	str1.LoadString(IDS_STRING596); wsprintf(macparams[95].name,str1);
	str1.LoadString(IDS_STRING597); wsprintf(macparams[96].name,str1);
	str1.LoadString(IDS_STRING598); wsprintf(macparams[97].name,str1);
	str1.LoadString(IDS_STRING599); wsprintf(macparams[98].name,str1);
	str1.LoadString(IDS_STRING600); wsprintf(macparams[99].name,str1);
	str1.LoadString(IDS_STRING601); wsprintf(macparams[100].name,str1);
	str1.LoadString(IDS_STRING602); wsprintf(macparams[101].name,str1);
	str1.LoadString(IDS_STRING603); wsprintf(macparams[102].name,str1);
	str1.LoadString(IDS_STRING604); wsprintf(macparams[103].name,str1);
	str1.LoadString(IDS_STRING605); wsprintf(macparams[104].name,str1);
	str1.LoadString(IDS_STRING606); wsprintf(macparams[105].name,str1);
	str1.LoadString(IDS_STRING607); wsprintf(macparams[106].name,str1);
	str1.LoadString(IDS_STRING608); wsprintf(macparams[107].name,str1);
	str1.LoadString(IDS_STRING609); wsprintf(macparams[108].name,str1);
}

void CCombineDoc::initmacparamdlg(CMacParam* p,bool enable)
{
	p->name1=macparams[1].name;
	p->name2=macparams[2].name;
	p->name3=macparams[3].name;
	p->name4=macparams[4].name;
	p->name5=macparams[5].name;
	p->name6=macparams[6].name;
	p->name7=macparams[7].name;
	p->name8=macparams[8].name;
	p->name9=macparams[9].name;
	p->name10=macparams[10].name;

	p->value1=macparams[1].value;
	p->value2=macparams[2].value;
	p->value3=macparams[3].value;
	p->value4=macparams[4].value;
	p->value5=macparams[5].value;
	p->value6=macparams[6].value;
	p->value7=macparams[7].value;
	p->value8=macparams[8].value;
	p->value9=macparams[9].value;
	p->value10=macparams[10].value;

	p->enable=enable;
}

void CCombineDoc::getmacparamsfromdlg(CMacParam* p)
{
	wcscpy(macparams[1].name,p->name1);
	wcscpy(macparams[2].name,p->name2);
	wcscpy(macparams[3].name,p->name3);
	wcscpy(macparams[4].name,p->name4);
	wcscpy(macparams[5].name,p->name5);
	wcscpy(macparams[6].name,p->name6);
	wcscpy(macparams[7].name,p->name7);
	wcscpy(macparams[8].name,p->name8);
	wcscpy(macparams[9].name,p->name9);
	wcscpy(macparams[10].name,p->name10);

	macparams[1].value=p->value1;
	macparams[2].value=p->value2;
	macparams[3].value=p->value3;
	macparams[4].value=p->value4;
	macparams[5].value=p->value5;
	macparams[6].value=p->value6;
	macparams[7].value=p->value7;
	macparams[8].value=p->value8;
	macparams[9].value=p->value9;
	macparams[10].value=p->value10;
}

void CCombineDoc::OnEditMacParams()
{
	CMacParam params;
	initmacparamdlg(&params,true);
	params.r.right=0;
	params.DoModal();
	getmacparamsfromdlg(&params);
	savedefaultmacros();
}

void CCombineDoc::GetMacParamValues()
{
	CMacParam params;
	initmacparamdlg(&params,false);
	params.DoModal();
	getmacparamsfromdlg(&params);
}

void CCombineDoc::InitializeComboBox()
{
	CMainFrame* mw=(CMainFrame*)AfxGetMainWnd();
	CMenu *menu;
	menu=mw->m_menu;
	CString s;

	mw->AddComboString(L"",-1,false);

	menu->GetMenuString(ID_MACRO1,s,MF_BYCOMMAND);
	if(s[0]!='_') mw->AddComboString(s,ID_MACRO1,true);

	menu->GetMenuString(ID_MACRO2,s,MF_BYCOMMAND);
	if(s[0]!='_') mw->AddComboString(s,ID_MACRO2,false);

	menu->GetMenuString(ID_MACRO3,s,MF_BYCOMMAND);
	if(s[0]!='_') mw->AddComboString(s,ID_MACRO3,false);

	menu->GetMenuString(ID_MACRO4,s,MF_BYCOMMAND);
	if(s[0]!='_') mw->AddComboString(s,ID_MACRO4,false);

	menu->GetMenuString(ID_MACRO5,s,MF_BYCOMMAND);
	if(s[0]!='_') mw->AddComboString(s,ID_MACRO5,false);

	menu->GetMenuString(ID_MACRO6,s,MF_BYCOMMAND);
	if(s[0]!='_') mw->AddComboString(s,ID_MACRO6,false);

	menu->GetMenuString(ID_MACRO7,s,MF_BYCOMMAND);
	if(s[0]!='_') mw->AddComboString(s,ID_MACRO7,false);

	menu->GetMenuString(ID_MACRO8,s,MF_BYCOMMAND);
	if(s[0]!='_') mw->AddComboString(s,ID_MACRO8,false);

	menu->GetMenuString(ID_MACRO9,s,MF_BYCOMMAND);
	if(s[0]!='_') mw->AddComboString(s,ID_MACRO9,false);

	menu->GetMenuString(ID_MACRO10,s,MF_BYCOMMAND);
	if(s[0]!='_') mw->AddComboString(s,ID_MACRO10,false);

	menu->GetMenuString(ID_MACRO_LOADMACROSET,s,MF_BYCOMMAND);
	if(s[0]!='_') mw->AddComboString(s,ID_MACRO_LOADMACROSET,false);

	menu->GetMenuString(ID_MACRO_RESTOREDEFAULTS,s,MF_BYCOMMAND);
	if(s[0]!='_') mw->AddComboString(s,ID_MACRO_RESTOREDEFAULTS,false);
}

void CCombineDoc::OnGo()
{
	CMainFrame* mw=(CMainFrame*)AfxGetMainWnd();
	int i=mw->GetComboCursel();
	switch(i)
	{
	case ID_MACRO1: OnMacro1(); break;
	case ID_MACRO2: OnMacro2(); break;
	case ID_MACRO3: OnMacro3(); break;
	case ID_MACRO4: OnMacro4(); break;
	case ID_MACRO5: OnMacro5(); break;
	case ID_MACRO6: OnMacro6(); break;
	case ID_MACRO7: OnMacro7(); break;
	case ID_MACRO8: OnMacro8(); break;
	case ID_MACRO9: OnMacro9(); break;
	case ID_MACRO10: OnMacro10(); break;
	case ID_MACRO_LOADMACROSET: OnMacroLoadmacroset(); break;
	case ID_MACRO_RESTOREDEFAULTS: OnMacroRestoredefaults(); break;
	}
}


void CCombineDoc::OnEditCopysettoclipboard()
{
	if(!OpenClipboard(AfxGetApp()->GetMainWnd()->m_hWnd)) return;
	EmptyClipboard();
	HGLOBAL copied_text=GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE,64000);
	wchar_t *buffer=(wchar_t*)GlobalLock(copied_text);
	buffer[0]=0;

	CString s="",s1,s2,st1,st2,st3;
	int m,l,p1,p2,p3;

	for(m=0; m<10; m++)
	{
		s.Format(L"Macro %2d - %s",m+1,macro[m].name);
		wcscat(buffer,(s+L"\r\n"));
		for(l=0; l<24; l++)
		{
			if(macro[m].cmd[l].n==-1) continue;
			s.Format(L"Line %2d",l+1);

			s1.LoadString(macro[m].cmd[l].n);
			if((macro[m].cmd[l].n>=IDS_MACRO1) && (macro[m].cmd[l].n<=IDS_MACRO10))
			{
				s1=macro[macro[m].cmd[l].n-IDS_MACRO1].name;
			}
			if((macro[m].cmd[l].n>=IDS_EXCOM1) && (macro[m].cmd[l].n<=IDS_EXCOM10))
			{
				s1=extcommand[macro[m].cmd[l].n-IDS_EXCOM1].name;
			}

			p1=macro[m].cmd[l].p1; if(p1<-1) p1++;
			if((int)MAC_P1V(macro[m].cmd[l].f)!=0 && (int)MAC_P1V(macro[m].cmd[l].f)!=11)
				st1=macparams[MAC_P1V(macro[m].cmd[l].f)].name;
			else st1.Format(L"%d",p1);

			p2=macro[m].cmd[l].p2; if(p2<-1) p2++;
			if((int)MAC_P2V(macro[m].cmd[l].f)!=0 && (int)MAC_P2V(macro[m].cmd[l].f)!=11)
				st2=macparams[MAC_P2V(macro[m].cmd[l].f)].name;
			else st2.Format(L"%d",p2);

			p3=macro[m].cmd[l].p3; if(p3<-1) p3++;
			if((int)MAC_P3V(macro[m].cmd[l].f)!=0 && (int)MAC_P3V(macro[m].cmd[l].f)!=11)
				st3=macparams[MAC_P3V(macro[m].cmd[l].f)].name;
			else st3.Format(L"%d",p3);

			s2=L"";
			if(macro[m].cmd[l].p1!=-1)
			{
				if(macro[m].cmd[l].p2!=-1)
				{
					if(macro[m].cmd[l].p3!=-1) s2.Format(L"(%s,%s,%s)",st1,st2,st3);
					else s2.Format(L"(%s,%s)",st1,st2);
				} else s2.Format(L"(%s)",st1);
			}

			if(macro[m].cmd[l].f&MAC_SUBP1) s2+=L" - S";
			else s2+=L" - 0";
			if(macro[m].cmd[l].f&MAC_SUBP2) s2+=L"S";
			else s2+=L"0";
			if(macro[m].cmd[l].f&MAC_SUBP3) s2+=L"S";
			else s2+=L"0";
			if(macro[m].cmd[l].f&MAC_TONAME) s2+=L"N";
			if(macro[m].cmd[l].f&MAC_PAUSE) s2+=L"P";

			wcscat(buffer,(s+L" - "+s1+s2+L"\r\n"));
		}
		wcscat(buffer,(L"\r\n"));
	}

	int c;
	for(c=0; c<13; c++)
	{
		s.Format(L"Command %2d - %s",c+1,extcommand[c].name);
		wcscat(buffer,(s+L"\r\n"));
		s.Format(L"Program - %s",extcommand[c].program);
		wcscat(buffer,(s+L"\r\n"));
		s.Format(L"Command Line - %s",extcommand[c].line);
		wcscat(buffer,(s+L"\r\n"));
		wcscat(buffer,(L"\r\n"));
	}

	int p;
	for(p=1; p<11; p++)
	{
		s.Format(L"Paramater %2d - %s=%d",p,macparams[p].name,macparams[p].value);
		wcscat(buffer,(s+L"\r\n"));
		wcscat(buffer,(L"\r\n"));
	}

	GlobalUnlock(copied_text);
	SetClipboardData(CF_UNICODETEXT,(void*)copied_text);
	CloseClipboard();
}
