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
// combineDoc.cpp : implementation of the CCombineDoc class
//

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
#include "GotoFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#undef new

IMPLEMENT_DYNCREATE(CCombineDoc, CDocument)

BEGIN_MESSAGE_MAP(CCombineDoc, CDocument)
	//{{AFX_MSG_MAP(CCombineDoc)
	ON_COMMAND(M_FIX, OnFix)
	ON_COMMAND(M_UNFIX, OnUnfix)
	ON_COMMAND(ID_SETTINGS_MAKEFIRST, OnMakefirst)
	ON_COMMAND(ID_SETTINGS_MAKELAST, OnMakelast)
	ON_COMMAND(ID_SETTINGS_ALLFRAMES, OnAllframes)
	ON_COMMAND(M_CLEAR, OnClear)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_REMOVE, OnFileRemove)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_COPYUNDER, OnCopyunder)
	ON_COMMAND(ID_EDIT_CLEARUNFIXED, OnClearunfixed)
	ON_COMMAND(ID_SAVEALLFRAMES, OnSaveallframes)
	ON_COMMAND(ID_FILE_SAVEFRAME, OnFileSaveframe)
	ON_COMMAND(ID_FRAME_RESIZEROTATEMOVE, OnSizecompensation)
	ON_COMMAND(ID_FILE_RELOAD, OnFileReload)
	ON_COMMAND(ID_FILE_RELOADALL, OnFileReloadall)
	ON_COMMAND(ID_PROCESS_RESIZEALL, OnResizeall)
	ON_COMMAND(ID_PROCESS_BALANCE, OnBalance)
	ON_COMMAND(ID_SIZE_CALCULATE, OnSizeCalculate)
	ON_COMMAND(ID_SIZE_CLEARPOINTS, OnSizeClearpoints)
	ON_COMMAND(ID_FRAME_REPLACE, OnFrameReplace)
	ON_COMMAND(ID_FRAME_NEWFROMPICTURE, OnFrameNewfrompicture)
	ON_COMMAND(ID_SPECIAL_ACTIVERED, OnSpecialActivered)
	ON_COMMAND(ID_SPECIAL_ACTIVEGREEN, OnSpecialActivegreen)
	ON_COMMAND(ID_SPECIAL_ACTIVEBULU, OnSpecialActivebule)
	ON_COMMAND(ID_SPECIAL_MAKETRANSPARENT, OnSpecialMaketransparent)
	ON_COMMAND(ID_FRAME_USE, OnFrameUse)
	ON_COMMAND(ID_FRAME_DONTUSE, OnFrameDontuse)
	ON_COMMAND(ID_VIEW_VIEWPICTURE, OnViewpicture)
	ON_COMMAND(ID_FRAME_DECIMATE, OnDecimate)
	ON_COMMAND(ID_VIEW_TOGGLEFIX, OnViewTogglefix)
	ON_COMMAND(ID_SPECIAL_CONTRAST, OnSpecialContrast)
	ON_COMMAND(ID_FRAME_ALIGNACTIVE, OnFrameAutoalignandsize)
	ON_COMMAND(ID_RECTANGLE_UNDO, OnRectangleUndo)
	ON_COMMAND(ID_VIEW_INFORMATION, OnViewInformation)
	ON_COMMAND(ID_FILE_SAVERECTANGLE, OnFileSaverectangle)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_VIEW_TOGGLEDEPTHMAP, OnViewToggledepthmap)
	ON_COMMAND(ID_FILE_LOADDEPTHINFORMATION, OnFileResumeWork)
	ON_COMMAND(ID_VIEW_FULLMODE, OnViewFullmode)
	ON_COMMAND(ID_SPECIAL_MAKEVISIBLEACTIVEFRAME2, OnSpecialMakevisibleactiveframe2)
	ON_COMMAND(ID_SPECIAL_CORRECTBRIGHTNESS, OnSpecialCorrectBrightness)
	ON_COMMAND(ID_SPECIAL_LEAVESPECIALMODE, OnSpecialLeavespecialmode)
	ON_COMMAND(ID_RECTANGLE_INVERTFIXED, OnRectangleInvertfixed)
	ON_COMMAND(ID_SPECIAL_BRIGHTNESS, OnSpecialBrightness)
	ON_COMMAND(ID_SPECIAL_RESET, OnSpecialReset)
	ON_COMMAND(ID_VIEW_SELECTALL, OnViewSelectall)
	ON_COMMAND(ID_SPECIAL_BLEND, OnSpecialBlend)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_COMMAND(ID_FRAME_MAKEMONOCHROME, OnFrameMakemonochrome)
	ON_COMMAND(ID_SPECIAL_ADD, OnSpecialAdd)
	ON_COMMAND(ID_FILE_IMPORTDEPTHMAP, OnFileImportdepthmap)
	ON_COMMAND(ID_FRAME_MAKEBG, OnFrameMakebg)
	ON_COMMAND(ID_FILE_EXPORTRECTANGLES, OnFileExportrectangles)
	ON_COMMAND(ID_FILTER_CREATELOWPASS, OnFilterCreatelowpass)
	ON_COMMAND(ID_FILTER_CREATEHIGHPASS, OnFilterCreatehighpass)
	ON_COMMAND(ID_FILTER_CREATEBANDPASS, OnFilterCreatebandpass)
	ON_COMMAND(ID_FILTER_CREATEBUTTERWORTH, OnFilterCreatebutterworth)
	ON_COMMAND(ID_FILTER_FILTERACTIVE, OnFilterNormalOutput)
	ON_COMMAND(ID_FILTER_CREATEBANDSTOP, OnFilterCreatebandstop)
	ON_COMMAND(ID_FILTER_THRESHOLD, OnFilterThreshold)
	ON_COMMAND(ID_FILTER_BRIGHTNESS, OnFilterBrightness)
	ON_COMMAND(ID_FILTER_RESUMEFILTERING, OnFilterResumefiltering)
	ON_COMMAND(ID_FILTER_TRANSFORMARITHMETIC_MAKEXFROMACTIVEFRAME, OnFilterTransformarithmeticMakexfromactiveframe)
	ON_COMMAND(ID_FILTER_FILTERALLFRAMES, OnFilterFilterallframes)
	ON_COMMAND(ID_FRAME_REVERSEORDER, OnFrameReverseorder)
	ON_COMMAND(ID_FILTER_SUMOFSTACK, OnFilterSumofstack)
	ON_COMMAND(ID_STACK_DETAIL, OnStackDetail)
	ON_COMMAND(ID_STACK_DEPTHFLOW, OnStackDepthflow)
	ON_COMMAND(ID_FILTER_MAKEXFROMDEPTHMAP, OnFilterMakexfromdepthmap)
	ON_COMMAND(ID_FILTER_REDISPLAYLASTOUTPUT, OnFilterRedisplaylastoutput)
	ON_COMMAND(ID_FRAME_TIDYEDGES, OnFrameTidyedges)
	ON_COMMAND(ID_FILTER_FREEMEMORY, OnFilterFreememory)
	ON_COMMAND(ID_FILE_WORKFRAMES_SAVEWORK, OnFileSaveworkstate)
	ON_COMMAND(ID_VIEW_LASTOUTPUT, OnViewOutput)
	ON_COMMAND(ID_STACK_REMOVEISLANDS, OnStackRemoveislands)
	ON_COMMAND(ID_STACK_REMOVESTEPS, OnStackRemovesteps)
	ON_COMMAND(ID_FILE_WORKFRAMES_RELOADDEPTH, OnFileWorkframesReloaddepth)
	ON_COMMAND(ID_FILE_WORKFRAMES_SAVEDEPTH, OnFileWorkframesSavedepth)
	ON_COMMAND(ID_UNDO_SETUNDOPOINT, OnUndoSetundopoint)
	ON_COMMAND(ID_STACK_INTERPOLATEDOUTPUT, OnStackInterpolate)
	ON_COMMAND(ID_FILTER_IMMEDIATE_LASTOUTPUTTODEPTH, OnFiltertodepth)
	ON_COMMAND(ID_MACRO_EDITMACRO, OnMacroEditmacro)
	ON_COMMAND(ID_STACK_SIZEANDALIGNMENT_AUTOMATICREGULARSTACK, OnStackSizeandalignmentAutomaticregularstack)
	ON_COMMAND(ID_VIEW_PROGRESS, OnViewProgress)
	ON_COMMAND(ID_FILE_EXPORTDEPTHMAP, OnFileExportdepthmap)
	ON_COMMAND(ID_MACRO1, OnMacro1)
	ON_COMMAND(ID_MACRO10, OnMacro10)
	ON_COMMAND(ID_MACRO2, OnMacro2)
	ON_COMMAND(ID_MACRO3, OnMacro3)
	ON_COMMAND(ID_MACRO4, OnMacro4)
	ON_COMMAND(ID_MACRO5, OnMacro5)
	ON_COMMAND(ID_MACRO6, OnMacro6)
	ON_COMMAND(ID_MACRO7, OnMacro7)
	ON_COMMAND(ID_MACRO8, OnMacro8)
	ON_COMMAND(ID_MACRO9, OnMacro9)
	ON_COMMAND(ID_MACRO_SAVEASDEFAULTSET, OnMacroSaveasdefaultset)
	ON_COMMAND(ID_MACRO_RESTOREDEFAULTS, OnMacroRestoredefaults)
	ON_COMMAND(ID_MACRO_ENABLEPAUSE, OnMacroDisablepause)
	ON_COMMAND(ID_MACRO_LOADMACROSET, OnMacroLoadmacroset)
	ON_COMMAND(ID_MACRO_CLEARALLMACROS, OnMacroClearallmacros)
	ON_COMMAND(ID_VIEW_TOGGLEFRAMESA1A2, OnViewToggleframesa1a2)
	ON_COMMAND(ID_VIEW_RESTOREPICTURE, OnViewRestorepicture)
	ON_COMMAND(ID_FRAME_COPYVISIBLETOOUT, OnFrameCopyvisibletoout)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_VIEW_GOTOFRAME, OnViewGotoframe)
	ON_COMMAND(ID_STACK_HORIZONTALSKEW, OnStackHorizontalskew)
	ON_COMMAND(ID_STACK_PATCHWORK, OnStackPatchwork)
	ON_COMMAND(ID_FILTER_INPUT_WEIGHTEDSUM, OnFilterInputWeightedsum)
	ON_COMMAND(ID_STACK_DETAIL_LIGHTANDDARK, OnStackDetailLightanddark)
	ON_COMMAND(ID_STACK_BALANCECOLOURANDBRIGHTNESS_LOCAL, OnStackBalancecolourandbrightnessLocal)
	ON_COMMAND(ID_STACK_BALANCECOLOURANDBRIGHTNESS_4X4, OnStackBalance4x4)
	ON_COMMAND(ID_FRAME_REPLACE, OnFrameReplace)
	ON_COMMAND(ID_MOVIE_MAKE, OnMovieMake)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCombineDoc construction/destruction

CCombineDoc::CCombineDoc()
{
	silent=false;
	macro_mode=false;
	backup_picture_valid=false;
	refreshscreen=false;
	buissy=false;
	memory=0;
	max_slots=0;
	active_frame=0;
	active_frame2=UNSET;
	special_active=0;
	display=PICTURE_BUFFER;
	top_frame=0;
	top_used_frame=0;
	last_used_frame=0;
	bottom_used_frame=0;

	magon=false;
	full_mode=false;
	plane_mode=false;
	fix_active=false;
	depth_flag=false;
	rainbow=0;
	save_bmp=false;
	show_fixed=false;
	filter_param=1;
	tab_pressed=false;
	redraw_filter=false;
	noise_level=50;
	patch_size=4;
	min_island_size=5;
	pause_enabled=true;
	skew_amount=10000;
	patch_size=1;
	jpeg_comp=70;
	light_threshold=140;
	dark_threshold=140;
	filter_changed=true;
	patch_threshold=10;

	dft.filter_param=0;
	dft.filtering=false;
	dft.minr=dft.ming=dft.minb=0;
	dft.mode=0;
	dft.out_mode=OUT_NORMAL;
	dft.valid_filter=false;
	dft.valid_picture1=false;
	dft.valid_picture3=false;
	dft.valid_store1=false;
	dft.valid_store3=false;
	dft.valid_transform1=false;
	dft.valid_transform3=false;

	progress.Create(IDD_PROGRESS);
}

CCombineDoc::~CCombineDoc()
{
	progress.DestroyWindow();
	delete_work();
	if(!top_frame) return;
}

void CCombineDoc::OnCloseDocument() 
{
	delete_work();
	CDocument::OnCloseDocument();
}

BOOL CCombineDoc::OnNewDocument()
{
	if(!CDocument::OnNewDocument())	return false;

	if(top_frame) FreeMemory();
	silent=false;
	macro_mode=false;
	backup_picture_valid=false;
	buissy=false;
	top_used_frame=0;
	last_used_frame=0;
	bottom_used_frame=0;
	active_frame=0;
	active_frame2=UNSET;
	special_active=0;
	display=PICTURE_BUFFER;
	top_frame=0;
	mode=0;
	plane_mode=saved_pm=false;
	depth_flag=false;
	full_mode=false;
	brush_size=1;
	brush_delta=10;
	size_factor=10000;
	wipeout=aCLEAR<<24;
	sizexorg=0;
	sizeyorg=0;
	magon=false;
	scale=1.0;
	angle=0;
	offsetx=0;
	offsety=0;
	sharp_factor=0.25;
	path="";
	fix_active=false;
	contrast_delta=5;
	show_fixed=true;
	xorg=yorg=0;
	active_frame=special_active=0;
	active_frame2=UNSET;
	bottom_used_frame=last_used_frame=0;
	rfactor=gfactor=bfactor=1.0;
	correction_factor=1.0;
	add_factor=1.0;
	blend_factor=0.5;
	new_flag=true;
	Sizer.p[0].valid=false;
	Sizer.p[1].valid=false;
	Sizer.p[2].valid=false;
	Sizer.p[3].valid=false;
	mag.top=mag.left=0;
	mag.bottom=mag.right=10;
	tab_pressed=false;
	dft.initialized=false;
	redraw_filter=false;
	skip_fsel=false;
	noise_level=50;
	min_island_size=5;
	UpdateAllViews(0);
	skew_amount=10000;
	patch_size=1;
	jpeg_comp=70;
	light_threshold=140;
	dark_threshold=140;
	filter_changed=true;
	patch_threshold=10;
	dft.filter_param=0;
	dft.filtering=false;
	dft.minr=dft.ming=dft.minb=0;
	dft.mode=0;
	dft.out_mode=OUT_NORMAL;
	dft.valid_filter=false;
	dft.valid_picture1=false;
	dft.valid_picture3=false;
	dft.valid_store1=false;
	dft.valid_store3=false;
	dft.valid_transform1=false;
	dft.valid_transform3=false;
	return true;
}

#ifdef _DEBUG
void CCombineDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCombineDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif

CString CCombineDoc::newnumber(CString s)
{
	CString temps,tests;
	static int number=1000;
	int i;
	bool match=true;

	while(match)
	{
		number--;
		if(number==0) number=999;
		match=false;
		temps.Format("%d",number);
		for(i=0; i<top_frame; i++)
		{
			tests=buffer[i].name;
			if(tests.Find(temps,0)!=0xffffffff)
			{
				match=true;
				break;
			}
		}
	};
	temps.Format("%s%d",s,number);
	return temps;
}

void CCombineDoc::RefreshScreen()
{
	if(magon && !full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

int CCombineDoc::UseFrame(int frame)
{
	int used,i;

	used=0; while(used_frame[used]<frame) used++;

	if(used_frame[used]==frame) return used;
	
	for(i=top_used_frame; i>used; i--) used_frame[i]=used_frame[i-1];
	top_used_frame++;
	used_frame[used]=frame;
	return used;
}

void CCombineDoc::UnuseFrame(int used)
{
	int i;
	for(i=used; i<last_used_frame; i++) used_frame[i]=used_frame[i+1];
	last_used_frame--;
	if(top_used_frame=last_used_frame) top_used_frame=last_used_frame-1;
}

void CCombineDoc::OnViewOutput() 
{
	display=OUT_BUFFER;
	RefreshScreen();
}

bool CCombineDoc::GetValue()
{
	int button;
	CGetvalue dd;
	switch(mode)
	{
	case SKEW:
		dd.m_string="10000 +(Right) -(Left) Pixels";
		dd.m_value=skew_amount;
		button=dd.DoModal();
		if(button==IDCANCEL) return false;
		skew_amount=dd.m_value;
		return true;
	case DETAIL:
		dd.m_string="0+ Ignore Detail below this level";
		dd.m_value=noise_level;
		button=dd.DoModal();
		if(button==IDCANCEL) return false;
		noise_level=dd.m_value;
		OnStackDetail();
		return true;
	case PATCH:
		dd.m_string="1+ Patch Width and Hight";
		dd.m_value=patch_size;
		button=dd.DoModal();
		if(button==IDCANCEL) return false;
		patch_size=dd.m_value;

		dd.m_string="1+ Patch Threshold";
		dd.m_value=patch_threshold;
		button=dd.DoModal();
		if(button==IDCANCEL) return false;
		patch_threshold=dd.m_value;
		OnStackPatchwork();
		return true;
	case LIGHT:
		dd.m_string="0+ Light Threshold Value";
		dd.m_value=light_threshold;
		button=dd.DoModal();
		if(button==IDCANCEL) return false;
		light_threshold=dd.m_value;
//		OnStackDetailLightanddark();
		return true;
	case DARK:
		dd.m_string="0+ Dark Threshold Value";
		dd.m_value=dark_threshold;
		button=dd.DoModal();
		if(button==IDCANCEL) return false;
		dark_threshold=dd.m_value;
//		OnStackDetailLightanddark();
		return true;
	case ISLAND:
		dd.m_string="1+ Remove islands below this size";
		dd.m_value=min_island_size;
		button=dd.DoModal();
		if(button==IDCANCEL) return false;
		min_island_size=dd.m_value;
		OnStackRemoveislands();
		return true;
	case JPEG:
		dd.m_string="0 to 100 Jpeg Compretion Factor";
		dd.m_value=jpeg_comp;
		button=dd.DoModal();
		if(button==IDCANCEL) return false;
		jpeg_comp=dd.m_value;
		return true;
	}
	return false;
}

void CCombineDoc::outwarn()
{
	static bool once=false;
	if(once==true) return;
	once=true;
	MessageBox(0,"You can not Paint on or Fix the output you must use a frame or the picture","Message",MB_OK);
}

void CCombineDoc::OnViewProgress() 
{
	progress.Maximize();
}

void CCombineDoc::OnViewToggleframesa1a2() 
{
	if(active_frame2==-1) return;
	if(display==active_frame) display=active_frame2;
	else display=active_frame;
	RefreshScreen();
}

void CCombineDoc::OnViewRestorepicture() 
{
	BeginWaitCursor();
	if(plane_mode) plane_mode=false;
	mode=0;
	dft.mode=0;
	dft.filtering=false;
	backup_picture_valid=false;
	redraw_picture();
	mag.left=mag.top=0;
	mag.right=w-1;
	mag.bottom=h-1;
	RefreshScreen();
	EndWaitCursor();
}

void CCombineDoc::OnAppExit() 
{
	delete_work();
	if(top_frame!=0)
	{
		int ret=MessageBox(0,"Do you wish to save anything before Exiting CombineZ?","Resume Work",MB_YESNO);
		if(ret==IDYES) return;
	}
	AfxPostQuitMessage(0);
}

void CCombineDoc::OnViewGotoframe() 
{
	gotoframe();
}

void CCombineDoc::gotoframe()
{
	CGotoFrame gtf;
	gtf.top_frame=top_frame;
	gtf.new_frame=active_frame;
	int ret=gtf.DoModal();
	if(ret==IDCANCEL) return;
	active_frame=display=gtf.new_frame;
	if(plane_mode)
	{
		draw_plane();
		display=PICTURE_BUFFER;
	}
	RefreshScreen();
}

void CCombineDoc::OnMovieMake() 
{
	int i,f;

	PIXEL *f1,*f2;
	int r,g,b;
BeginWaitCursor();
	for(f=0; f<last_used_frame-1; f++)
	{
		f1=(PIXEL*)GetBuffer(used_frame[f]);
		f2=(PIXEL*)GetBuffer(used_frame[f+1]);
		for(i=0; i<(int)(w*h); i++)
		{
			r=(int)f2[i].r-(int)f1[i].r;
			if(r<4) r=0;
			if(r>15) r=255; else r*=5;
			f1[i].r=(unsigned char)abs(r)&0xff;

			g=(int)f2[i].g-(int)f1[i].g;
			if(g<4) g=0;
			if(g>15) g=255; else g*=5;
			f1[i].g=(unsigned char)abs(g)&0xff;

			b=(int)f2[i].b-(int)f1[i].b;
			if(b<4) b=0;
			if(b>15) b=255; else b*=5;
			f1[i].b=(unsigned char)abs(b)&0xff;
		}
		UpdateBuffer(used_frame[f]);
		UpdateBuffer(used_frame[f+1]);
	}
EndWaitCursor();
}
