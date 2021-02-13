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
#include "combinedoc.h"
#include ".\combinedoc.h"
#include "options.h"

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
	ON_COMMAND(ID_FRAME_REPLACE, OnFrameReplace)
	ON_COMMAND(ID_MOVIE_MAKE, OnMovieMake)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MOVIE_EXTRACTFRAMES, OnMovieExtractframes)
	ON_COMMAND(ID_AQUIREIMAGE_SELECTSOURCE, OnAcquireimageSelectsource)
	ON_COMMAND(ID_AQUIREIMAGE_GETIMAGE, OnAquireimageGetimage)
	ON_COMMAND(ID_COMMAND_EDITCOMMANDS, OnCommandEditcommands)
	ON_COMMAND(ID_ACQUIREIMAGE_TOGGLEVIDEO, OnAcquireimageTogglevideo)
	ON_COMMAND(ID_MOVIE_EXPORTMOVIEFRAMES, OnMovieExportmovieframes)
	ON_COMMAND(ID_EXCOM11, OnExcom11)
	ON_COMMAND(ID_EXCOM12, OnExcom12)
	ON_COMMAND(ID_SYSCOM13, OnSyscom13)
	ON_COMMAND(ID_EDIT_PARAMETERS, OnEditMacParams)
	ON_COMMAND(ID_INPUT_WEIGHTEDSUM, OnInputWeightedsumWC)
	ON_COMMAND(ID_BALANCECOLOURANDBRIGHTNESS_LOCALCOLOURONLY, OnBalancecolourandbrightnessLocalcolouronly)
	ON_COMMAND(ID_STACK_AVERAGETOOUT, OnStackAveragetoout)
	ON_COMMAND(ID_SIZEANDALIGNMENT_ASTRONOMICAL, OnSizeandalignmentAstronomical)
	ON_COMMAND(ID_BALANCECOLOURANDBRIGHTNESS_GLOBALBRIGHTNESS, OnBalancecolourandbrightnessGlobalbrightness)
	ON_COMMAND(ID_FRAME_SPLITRGB, OnFrameSplitrgb)
	ON_COMMAND(ID_FRAME_INVERT, OnFrameInvert)
	ON_COMMAND(ID_FRAME_REDUCENOISE, OnFrameReducenoise)
	ON_COMMAND(ID_FRAME_STRETCHCOLOURS, OnFrameStretchcolours)
	ON_COMMAND(ID_FRAME_SQUEEZESPECTRUM, OnFrameSqueezespectrum)
	ON_COMMAND(ID_FRAME_EXTRACTCOLOURRANGE, OnFrameExtractcolourrange)
	ON_COMMAND(ID_FRAME_SPLITHSG, OnFrameSplithsg)
	ON_COMMAND(ID_FRAME_COLOURPRODUCT, OnFrameColourproduct)
	ON_COMMAND(ID_FRAME_NEWFROMVISIBLE, OnFrameNewfromvisible)
	ON_COMMAND(ID_COLOUR_RAINBOW, OnColourRainbow)
	ON_COMMAND(ID_FRAME_NEWFROMDEPTHMAP, OnFrameNewfromdepthmap)
	ON_COMMAND(ID_HELP_COMBINEZPHELP, OnHelpCombinezphelp)
	ON_COMMAND(ID_PYRAMID_CLEARPYRAMID, OnPyramidClearpyramid)
	ON_COMMAND(ID_MERGEACTIVEWITHPYRAMID_MAXIMUMRULE, OnMergeactivewithpyramidMaximumrule)
	ON_COMMAND(ID_PYRAMID_PYRAMIDTOOUT, OnPyramidPyramidtoout)
	ON_COMMAND(ID_FILE_SETOPTIONS, OnFileSetoptions)
	ON_COMMAND(ID_PYRAMID_RECOVERDEPTH, OnPyramidRecoverdepth)
	ON_COMMAND(ID_RGBWEIGHTEDAVERAGERULE_ADDACTIVETYPYRAMID, OnRgbweightedaverageruleAddactivetypyramid)
	ON_COMMAND(ID_RGBWEIGHTEDAVERAGERULE_RESULTTOOUT, OnRgbweightedaverageruleResulttoout)
	ON_COMMAND(ID_RGBMAXIMUMCONTRASTRULE_ADDALLUSEDFRAMES, OnRgbmaximumcontrastruleAddallusedframes)
	ON_COMMAND(ID_RGBWEIGHTEDAVERAGERULE_ADDALLUSEDFRAMES, OnRgbweightedaverageruleAddallusedframes)
	ON_COMMAND(ID_FRAME_UP, OnFrameUp)
	ON_COMMAND(ID_FRAME_DOWN, OnFrameDown)
	ON_COMMAND(ID_GO, OnGo)
	ON_COMMAND(ID_ACQUIREIMAGE_CAMERASETTINGS, OnAcquireimageCamerasettings)
	ON_COMMAND(ID_FILE_EMPTYSTACK, OnFileEmptystack)
	ON_COMMAND(ID_2FRAMEFUNCTIONS_CORRECTBRIGHTNESS, On2framefunctionsCorrectbrightness)
	ON_COMMAND(ID_ACQUIREIMAGE_SAVESETTINGS, OnAcquireimageSavesettings)
	ON_COMMAND(ID_ACQUIREIMAGE_LOADSETTINGS, OnAcquireimageLoadsettings)
	ON_COMMAND(ID_ACQUIREIMAGE_SAVECAMERADEFAULTS, OnAcquireimageSavecameradefaults)
	ON_COMMAND(ID_ACQUIREIMAGE_GETMOVIE, OnAcquireimageGetmovie)
	ON_COMMAND(ID_FRAME_CREATESCALELINE, OnFrameCreatescaleline)
	ON_COMMAND(ID_SIZEANDALIGNMENT_AUTO, OnSizeandalignmentAuto)
	ON_COMMAND(ID_FRAME_SAVEANDEDIT, OnFrameSaveandedit)
	ON_COMMAND(ID_EDIT_COPYSETTOCLIPBOARD, OnEditCopysettoclipboard)
	ON_COMMAND(ID_ACQUIREIMAGE_CAPTUREMOVIETOFILE, OnAcquireimageCapturemovietofile)
	ON_COMMAND(ID_FILE_LOADSPECIAL, OnFileLoadspecial)
	ON_COMMAND(ID_FILE_SAVESPECIAL, OnFileSavespecial)
	ON_COMMAND(ID_FRAME_BEGINRETOUCHING, OnFrameBeginretouching)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCombineDoc construction/destruction

extern int ptree[];

CCombineDoc::CCombineDoc()
: bandw(false)
, hdr_bright(0)
, hdr_log(0)
, hdr_hia(0)
, buffer_count(0)
, detail_size(1)
, detail_filter(0)
, video_w(0)
, video_h(0)
, work_path(_T(""))
, index(99999)
, special_frame(0)
, paintonout(true)
, brush_feather(3)
{
#if !defined W2000
	CoInitializeEx(0,COINIT_MULTITHREADED);
#endif
	paintonout=true;
	brush_feather=3;
	bmp_properties=0;
	packed=0;
	special_frame=0;
	maxangle=100;
	bandw=false;
	movh=480;
	movw=640;
	movq=70;
	movt=100;
	silent=false;
	macro_mode=false;
	mode=0;
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
	buissy=false;

	magon=false;
	full_mode=true;
	plane_mode=saved_pm=false;
	fix_active=false;
	depth_flag=false;
	rainbow=0;
	save_bmp=false;
	show_fixed=false;
	filter_param=1;
	tab_pressed=false;
	redraw_filter=false;
	noise_level=25;
	detail_size=1;
	detail_filter=0;
	min_island_size=5;
	pause_enabled=true;
	skew_amount=skew_amountv=10000;
	patch_size=1;
	jpeg_comp=70;
	light_threshold=140;
	dark_threshold=140;
	filter_changed=true;
	patch_threshold=10;
	hdr_bright=1000;
	hdr_hia=0;
	hdr_log=0;

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

	Extract_redend=0;
	Extract_bluend=1529;
	Extract_th=0;
	Squeeze_redend=0;
	Squeese_bluend=1529;
	Squeeze_break=0;
	Noise_detect=100;
	Noise_sub=500;
	Noise_lim=15;
	buffer_count=0;
	frame_similarity=9999999;

	Tle=0;
	Tlt=0;
	Teq=0;
	Tge=0;
	Tgt=0;
	Tne=0;

	help_window=0;

	plast_level=0;
	pnoise=3;
	plevels=100;
	pnumber=7;
	pforeground=100;
	pbrightness=100;
	pbackground=100;

	progress.Create(IDD_PROGRESS);
	if(progress.minimized) progress.ShowWindow(SW_HIDE);

	endvideo();

	video_showing=false;

	Dump();

	index=AfxGetApp()->GetProfileInt(L"File",L"Index",index);
}

CCombineDoc::~CCombineDoc()
{
	if(bmp_properties) delete [] bmp_properties;
	bmp_properties=0;

	if(Graph) endvideo();
	CoUninitialize();
	progress.DestroyWindow();
	delete_work();
}

void CCombineDoc::OnCloseDocument() 
{
	delete_work();
	endvideo();
	CDocument::OnCloseDocument();
}

BOOL CCombineDoc::OnNewDocument()
{
	if(!CDocument::OnNewDocument())	return false;

	if(top_frame)
		FreeMemory();

	paintonout=true;
	brush_feather=3;

	if(bmp_properties) delete [] bmp_properties;
	bmp_properties=0;
	special_frame=0;
	plevels=3;
	pnumber=1;
	maxangle=100;
	bandw=false;
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
	full_mode=true;
	brush_size=5;
	brush_delta=25;
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
	work_path="";
	CString p=path.Right(path.GetLength()-path.ReverseFind('\\')-1);
	progress.SetWindowText(L"CZP Progress ...");
	save_path="";
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
	mag.bottom=mag.right=100;
	tab_pressed=false;
	dft.initialized=false;
	redraw_filter=false;
	skip_fsel=false;
	noise_level=25;
	detail_size=1;
	detail_filter=0;
	min_island_size=5;
	skew_amount=skew_amountv=10000;
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
	hdr_bright=1000;
	hdr_hia=0;
	hdr_log=0;

	Extract_redend=0;
	Extract_bluend=1529;
	Extract_th=0;
	Squeeze_redend=0;
	Squeese_bluend=1529;
	Squeeze_break=0;
	Noise_detect=100;
	Noise_sub=500;
	Noise_lim=15;
	buffer_count=0;
	frame_similarity=9999999;

	Tle=0;
	Tlt=0;
	Teq=0;
	Tge=0;
	Tgt=0;
	Tne=0;

	plast_level=0;
	plevels=100;
	pnumber=7;
	pforeground=100;
	pbrightness=100;
	pnoise=3;
	pbackground=100;

	Dump();

	if(Graph==0) video_showing=false;

	packed=0;

	UpdateAllViews(0);

	index=AfxGetApp()->GetProfileInt(L"File",L"Index",index);

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
	static int number=100000;
	int i;
	bool match=true;

	while(match)
	{
		number--;
		if(number==0) number=99999;
		match=false;
		temps.Format(L"%05d",number);
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
	temps.Format(L"%s%d",s,number);
	return temps;
}

void CCombineDoc::RefreshScreen()
{
	if(macro_mode) return;
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
	if(top_used_frame==last_used_frame) top_used_frame=last_used_frame-1;
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
	case LOADSPECIAL:
		dd.m_string.LoadString(IDS_LOADSPECIAL);
		dd.m_value=special_frame;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		special_frame=dd.m_value;
		return true;
	case SAVESPECIAL:
		dd.m_string.LoadString(IDS_SAVESPECIAL);
		dd.m_value=special_frame;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		special_frame=dd.m_value;
		return true;
	case PYRAMID_WOUT:
		dd.m_string.LoadString(IDS_STRING231);
		dd.m_value=pforeground;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		pforeground=dd.m_value;

		dd.m_string.LoadString(IDS_STRING232);
		dd.m_value=pbrightness;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		pbrightness=dd.m_value;

		dd.m_string.LoadString(IDS_STRING233);
		dd.m_value=pbackground;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		pbackground=dd.m_value;

		plevels=plast_level+1;

		return true;
	case PYRAMID_DEPTH:
		while(true)
		{
			CString s;
			str1.LoadString(IDS_STRING234);
			s.Format(str1,plast_level+1);
			dd.m_string=s;
			dd.m_value=plevels;
			button=(int)dd.DoModal();
			if(button==IDCANCEL) return false;
			plevels=dd.m_value;
			if(plevels<0) continue;
			break;
		}

		while(true)
		{
			dd.m_string.LoadString(IDS_STRING235);
			dd.m_value=pnoise;
			button=(int)dd.DoModal();
			if(button==IDCANCEL) return false;
			pnoise=dd.m_value;
			if(pnoise<0) continue;
			break;
		}
		return true;
	case PYRAMID_OUT:
		dd.m_string.LoadString(IDS_STRING231);
		dd.m_value=pforeground;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		pforeground=dd.m_value;

		dd.m_string.LoadString(IDS_STRING232);
		dd.m_value=pbrightness;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		pbrightness=dd.m_value;

		dd.m_string.LoadString(IDS_STRING233);
		dd.m_value=pbackground;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		pbackground=dd.m_value;

		plevels=plast_level+1;

		return true;
	case HDR_BRIGHT:
		dd.m_string.LoadString(IDS_STRING239);
		dd.m_value=hdr_bright;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		hdr_bright=dd.m_value;
		return true;
	case HDR_LOG:
		dd.m_string.LoadString(IDS_STRING240);
		dd.m_value=hdr_log;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		hdr_log=dd.m_value;
		return true;
	case HDR_HIA:
		dd.m_string.LoadString(IDS_STRING241);
		dd.m_value=hdr_hia;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		hdr_hia=dd.m_value;
		return true;
	case SKEW:
		dd.m_string.LoadString(IDS_STRING242);
		dd.m_value=skew_amount;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		skew_amount=dd.m_value;

		dd.m_string.LoadString(IDS_STRING660);
		dd.m_value=skew_amountv;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		skew_amountv=dd.m_value;
		return true;
	case DETAIL:
		dd.m_string.LoadString(IDS_STRING243);
		dd.m_value=noise_level;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		noise_level=dd.m_value;

		dd.m_string.LoadString(IDS_STRING244);
		dd.m_value=detail_size;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		detail_size=dd.m_value;

		dd.m_string.LoadString(IDS_STRING245);
		dd.m_value=detail_filter;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		detail_filter=dd.m_value;

		OnStackDetail();
		return true;
	case PATCH:
		dd.m_string.LoadString(IDS_STRING246);
		dd.m_value=patch_size;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		patch_size=dd.m_value;

		dd.m_string.LoadString(IDS_STRING247);
		dd.m_value=patch_threshold;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		patch_threshold=dd.m_value;
		OnStackPatchwork();
		return true;
	case LIGHT:
		dd.m_string.LoadString(IDS_STRING248);
		dd.m_value=light_threshold;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		light_threshold=dd.m_value;
		return true;
	case DARK:
		dd.m_string.LoadString(IDS_STRING249);
		dd.m_value=dark_threshold;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		dark_threshold=dd.m_value;
		return true;
	case ISLAND:
		dd.m_string.LoadString(IDS_STRING250);
		dd.m_value=min_island_size;
		button=(int)dd.DoModal();
		if(button==IDCANCEL) return false;
		min_island_size=dd.m_value;
		OnStackRemoveislands();
		return true;
	case JPEG:
		dd.m_string.LoadString(IDS_STRING251);
		dd.m_value=jpeg_comp;
		button=(int)dd.DoModal();
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
	CString s1,s2;
	s1.LoadString(IDS_NOPAINT); s2.LoadString(IDS_MESSAGE);
	MessageBox(progress.m_hWnd,s1,s2,MB_OK);
}

void CCombineDoc::OnViewProgress() 
{
	progress.minimized=false;
//	AfxGetApp()->WriteProfileInt(L"Progress",L"Minimized",false);
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
		CString s1,s2;
		s1.LoadString(IDS_EXITMESSAGE); s2.LoadString(IDS_RESUMEWORK);
		int ret=MessageBox(progress.m_hWnd,s1,s2,MB_YESNO);
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
	if(top_frame==0) return;

	CGotoFrame gtf;
	gtf.top_frame=top_frame;
	gtf.new_frame=active_frame;
	int ret=(int)gtf.DoModal();
	if(ret==IDCANCEL) return;

	display=gtf.new_frame;
	if(display==DEPTH_BUFFER)
	{
		if(depth_flag) display=PICTURE_BUFFER;
		else OnViewToggledepthmap();
	} else {
		if(display==PICTURE_BUFFER)
		{
			if(depth_flag) OnViewToggledepthmap();
		}
	}
	if(gtf.new_frame<top_frame) active_frame=display;

	if(plane_mode)
	{
		draw_plane();
		display=PICTURE_BUFFER;
	}
	RefreshScreen();
}

#include "Noise.h"

void CCombineDoc::OnFrameReducenoise()
{
	int lim=Noise_lim,detect=Noise_detect,sub=Noise_sub;

	if(!macro_mode)
	{
		CNoise dlg;
		dlg.m_detection=detect;
		dlg.m_substitute=sub;
		dlg.m_limit=lim;
		int ret;
		ret=(int)dlg.DoModal();
		if(ret!=IDOK) return;
		Noise_lim=lim=dlg.m_limit;
		Noise_sub=sub=dlg.m_substitute;
		Noise_detect=detect=dlg.m_detection;
		BeginWaitCursor();
	}

	int x,y,Y;
	int i,count=0;
	PIXEL *f0,*t0,*f1,*t1,p0,p1;

	progress.AddString(IDS_REDUCENOISE,detect,lim,sub);

	if(!dft.initialized) dftReset();

	mode=0;
	dft.displacement=0.0f;
	dft.width=(float)detect/1000.0f;
	ImmediateLowpass();
	dft.out_mode=OUT_NORMAL;

	t0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);
	f0=(PIXEL*)GetBuffer(active_frame);
	filterframe(f0,t0);
	UpdateBuffer(TEMP_BUFFER+0);

	dft.displacement=0.0f;
	dft.width=(float)sub/1000.0f;
	ImmediateLowpass();
	dft.out_mode=OUT_NORMAL;

	t1=(PIXEL*)GetBuffer(TEMP_BUFFER+1);
	filterframe(f0,t1);

	t0=(PIXEL*)GetBuffer(TEMP_BUFFER+0);

	f1=(PIXEL*)GetBuffer(OUT_BUFFER);
	for(i=0; i<w*h; i++) f1[i].abgr=0;

	for(y=1; y<h-1; y++)
	{
		Y=(int)w*y;
		for(x=1; x<w-1; x++)
		{
			i=x+Y;
			p0=f0[i];
			p1=t0[i];

			if(
				(abs((int)p0.r-(int)p1.r)<lim) &&
				(abs((int)p0.g-(int)p1.g)<lim) &&
				(abs((int)p0.b-(int)p1.b)<lim)
			)
			{
				f1[i-1+w].abgr++;
				f1[i+w].abgr++;
				f1[i+1+w].abgr++;
				f1[i-1].abgr++;
				f1[i].abgr++;
				f1[i+1-w].abgr++;
				f1[i-1-w].abgr++;
				f1[i-w].abgr++;
				f1[i+1-w].abgr++;
			}
		}
	}

	int c0,c1;
	int r,g,b;
	for(i=0; i<w*h; i++)
	{
		c0=f1[i].abgr; c1=9-c0;
		r=(c0*(int)t1[i].r+c1*(int)f0[i].r)/9;
		g=(c0*(int)t1[i].g+c1*(int)f0[i].g)/9;
		b=(c0*(int)t1[i].b+c1*(int)f0[i].b)/9;
		f1[i].r=(unsigned char)r;
		f1[i].g=(unsigned char)g;
		f1[i].b=(unsigned char)b;
		f1[i].a=(unsigned char)f0[i].a;
	}

	FreeBuffer(TEMP_BUFFER+0);
	FreeBuffer(TEMP_BUFFER+1);
	UpdateBuffer(OUT_BUFFER);
	FreeBuffer(active_frame);

	display=OUT_BUFFER;

	if(!macro_mode)
	{
		EndWaitCursor();
		RefreshScreen();
	}
}

void CCombineDoc::OnFrameNewfromdepthmap()
{
	if(!top_frame) return;

	if(!macro_mode)
	{
		BeginWaitCursor();
	}

	int x,y,Y,i,c,max,min;

	CString s;

	progress.AddString(IDS_NEWDPTH);
	progress.AddString(s);

	PIXEL *f1=(PIXEL *)GetBuffer(top_frame);
	buffer[top_frame].name=newnumber("depthmap");
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

	if(!macro_mode)
	{
		display=active_frame=top_frame-1;
		EndWaitCursor();
		RefreshScreen();
	}
}

#include "htmlhelp.h"

void CCombineDoc::OnHelpCombinezphelp()
{
	POSITION pos=GetFirstViewPosition();
	CView *view=GetNextView(pos);

	if(help_window)
	{
		WINDOWPLACEMENT wp;
		GetWindowPlacement(help_window,&wp);
		if(wp.showCmd==SW_SHOWMINIMIZED) ShowWindow(help_window,SW_RESTORE);
		else ShowWindow(help_window,SW_MINIMIZE);
		view->SetFocus();
		return;
	}

	help_window=HtmlHelp(view->m_hWnd,L"CZPHelp.chm::/HTML/StartPage.htm>MyHelpChild",HH_DISPLAY_TOPIC,0);
//	help_window=HtmlHelp(AfxGetApp()->GetMainWnd()->m_hWnd,L"CZPHelp.chm::/HTML/StartPage.htm>MyHelpChild",HH_DISPLAY_TOPIC,0);

	view->SetFocus();
}

void CCombineDoc::Dump()
{
	if(!theApp.debug_switch) return;

	CString s;

	static int dspecial_frame=999999;
	static int dmaxangle=-999999;
	static int dbandw=-999999;
	static int dmovh=-999999;
	static int dmovw=-999999;
	static int dmovq=-999999;
	static int dmovt=-999999;
	static int dsilent=-999999;
	static int dmacro_mode=-999999;
	static int dmode=-999999;
	static int dbackup_picture_valid=-999999;
	static int drefreshscreen=-999999;
	static int dbuissy=-999999;
	static int dmemory=-999999;
	static int dmax_slots=-999999;
	static int dactive_frame=-999999;
	static int dactive_frame2=-999999;
	static int dspecial_active=-999999;
	static int ddisplay=-999999;
	static int dtop_frame=-999999;
	static int dtop_used_frame=-999999;
	static int dlast_used_frame=-999999;
	static int dbottom_used_frame=-999999;
	static int dmagon=-999999;
	static int dfull_mode=-999999;
	static int dplane_mode=-999999;
	static int dsaved_pm=-999999;
	static int dfix_active=-999999;
	static int ddepth_flag=-999999;
	static int drainbow=-999999;
	static int dsave_bmp=-999999;
	static int dshow_fixed=-999999;
	static int dfilter_param=-999999;
	static int dtab_pressed=-999999;
	static int dredraw_filter=-999999;
	static int dnoise_level=-999999;
	static int ddetail_size=-999999;
	static int ddetail_filter=-999999;
	static int dmin_island_size=-999999;
	static int dpause_enabled=-999999;
	static int dskew_amount=-999999;
	static int dskew_amountv=-999999;
	static int dpatch_size=-999999;
	static int djpeg_comp=-999999;
	static int dlight_threshold=-999999;
	static int ddark_threshold=-999999;
	static int dfilter_changed=-999999;
	static int dpatch_threshold=-999999;
	static int dhdr_bright=-999999;
	static int dhdr_hia=-999999;
	static int dhdr_log=-999999;
	static int ddft_filter_param=-999999;
	static int ddft_filtering=-999999;
	static int ddft_minr=-999999;
	static int ddft_ming=-999999;
	static int ddft_minb=-999999;
	static int ddft_mode=-999999;
	static int ddft_out_mode=-999999;
	static int ddft_valid_filter=-999999;
	static int ddft_valid_picture1=-999999;
	static int ddft_valid_picture3=-999999;
	static int ddft_valid_store1=-999999;
	static int ddft_valid_store3=-999999;
	static int ddft_valid_transform1=-999999;
	static int ddft_valid_transform3=-999999;
	static int dExtract_redend=-999999;
	static int dExtract_bluend=-999999;
	static int dExtract_th=-999999;
	static int dSqueeze_redend=-999999;
	static int dSqueese_bluend=-999999;
	static int dSqueeze_break=-999999;
	static int dNoise_detect=-999999;
	static int dNoise_sub=-999999;
	static int dNoise_lim=-999999;
	static int dbuffer_count=-999999;
	static int dhelp_window=-999999;
	static int dmag_bottom=-999999;
	static int dmag_left=-999999;
	static int dmag_right=-999999;
	static int dmag_top=-999999;
	static int dSizer_length34=-999999;
	static int dSizer_length12=-999999;
	static int dSizer_angle=-999999;
	static int dSizer_ratio=-999999;
	static int dSizer_p0_f=-999999;
	static int dSizer_p0_valid=-999999;
	static int dSizer_p0_x=-999999;
	static int dSizer_p0_y=-999999;
	static int dSizer_p1_f=-999999;
	static int dSizer_p1_valid=-999999;
	static int dSizer_p1_x=-999999;
	static int dSizer_p1_y=-999999;
	static int dSizer_p2_f=-999999;
	static int dSizer_p2_valid=-999999;
	static int dSizer_p2_x=-999999;
	static int dSizer_p2_y=-999999;
	static int dSizer_p3_f=-999999;
	static int dSizer_p3_valid=-999999;
	static int dSizer_p3_x=-999999;
	static int dSizer_p3_y=-999999;
	static int dangle=-999999;
	static int dxorg=-999999;
	static int dyorg=-999999;
	static int dscale=-999999;
	static int dsmallw=-999999;
	static int dsmallh=-999999;
	static int dsize_factor=-999999;
	static int dsizexorg=-999999;
	static int dsizeyorg=-999999;
	static int doffsetx=-999999;
	static int doffsety=-999999;
	static int dframe_border=-999999;
	static int dgoodrect_bottom=-999999;
	static int dgoodrect_top=-999999;
	static int dgoodrect_left=-999999;
	static int dgoodrect_right=-999999;
	static int dbrush_size=-999999;
	static int dbrush_delta=-999999;
	static int dbufferactive_frame_hidden=-999999;
	static int dTle=-999999;
	static int dTlt=-999999;
	static int dTeq=-999999;
	static int dTge=-999999;
	static int dTgt=-999999;
	static int dTne=-999999;
	static int dframe_similarity=-999999;

	static UINT dbmpprop_size=0xffffffff;
	static UINT dbmpprop_count=0xffffffff;
	static int dplevels=-999999;
	static int dpnumber=-999999;
	static int dpnoise=-999999;
	static int dpframe=-999999;
	static int dplast=-999999;
	static int dpdepth=-999999;
	static int dpforeground=-999999;
	static int dpbrightness=-999999;
	static int dpbackground=-999999;
	static int dvideo_w=-999999;
	static int dvideo_h=-999999;
	static int dstill_w=-999999;
	static int dstill_h=-999999;
	static int dvideo_showing=-999999;

	if(dspecial_frame!=special_frame)
	{s.Format(L"#special_frame %d",(int)special_frame); progress.AddString(s);}
	if(dbmpprop_size!=bmpprop_size)
	{s.Format(L"#bmpprop_size %d",(int)bmpprop_size); progress.AddString(s);}
	if(dbmpprop_count!=bmpprop_count)
	{s.Format(L"#bmpprop_count %d",(int)bmpprop_count); progress.AddString(s);}
	if(dplevels!=plevels)
	{s.Format(L"#plevels %d",(int)plevels); progress.AddString(s);}
	if(dpnumber!=pnumber)
	{s.Format(L"#pnumber %d",(int)pnumber); progress.AddString(s);}
	if(dpnoise!=pnoise)
	{s.Format(L"#pnoise %d",(int)pnoise); progress.AddString(s);}
	if(dpframe!=pframe)
	{s.Format(L"#pframe %d",(int)pframe); progress.AddString(s);}
	if(dplast!=plast)
	{s.Format(L"#plast %d",(int)plast); progress.AddString(s);}
	if(dpdepth!=pdepth)
	{s.Format(L"#pdepth %d",(int)pdepth); progress.AddString(s);}
	if(dpforeground!=pforeground)
	{s.Format(L"#pforeground %d",(int)pforeground); progress.AddString(s);}
	if(dpbrightness!=pbrightness)
	{s.Format(L"#pbrightness %d",(int)pbrightness); progress.AddString(s);}
	if(dpbackground!=pbackground)
	{s.Format(L"#pbackground %d",(int)pbackground); progress.AddString(s);}
	if(dvideo_w!=video_w)
	{s.Format(L"#video_w %d",(int)video_w); progress.AddString(s);}
	if(dvideo_h!=video_h)
	{s.Format(L"#video_h %d",(int)video_h); progress.AddString(s);}
	if(dstill_w!=still_w)
	{s.Format(L"#still_w %d",(int)still_w); progress.AddString(s);}
	if(dstill_h!=still_h)
	{s.Format(L"#still_h %d",(int)still_h); progress.AddString(s);}
	if(dvideo_showing!=(int)video_showing)
	{s.Format(L"#video_showing %d",(int)video_showing); progress.AddString(s);}

	if(dmaxangle!=maxangle)
	{s.Format(L"#maxangle %d",(int)maxangle); progress.AddString(s);}
	if(dbandw!=(int)bandw)
	{s.Format(L"#bandw %d",(int)bandw); progress.AddString(s);}
	if(dmovh!=movh)
	{s.Format(L"#movh %d",(int)movh); progress.AddString(s);}
	if(dmovw!=movw)
	{s.Format(L"#movw %d",(int)movw); progress.AddString(s);}
	if(dmovq!=movq)
	{s.Format(L"#movq %d",(int)movq); progress.AddString(s);}
	if(dmovt!=movt)
	{s.Format(L"#movt %d",(int)movt); progress.AddString(s);}
	if(dsilent!=(int)silent)
	{s.Format(L"#silent %d",(int)silent); progress.AddString(s);}
	if(dmacro_mode!=(int)macro_mode)
	{s.Format(L"#macro_mode %d",(int)macro_mode); progress.AddString(s);}
	if(dmode!=mode)
	{s.Format(L"#mode %d",(int)mode); progress.AddString(s);}
	if(dbackup_picture_valid!=(int)backup_picture_valid)
	{s.Format(L"#backup_picture_valid %d",(int)backup_picture_valid); progress.AddString(s);}
	if(drefreshscreen!=(int)refreshscreen)
	{s.Format(L"#refreshscreen %d",(int)refreshscreen); progress.AddString(s);}
	if(dbuissy!=(int)buissy)
	{s.Format(L"#buissy %d",(int)buissy); progress.AddString(s);}
	if(dmemory!=(int)memory)
	{s.Format(L"#memory %d",(int)memory); progress.AddString(s);}
	if(dmax_slots!=(int)max_slots)
	{s.Format(L"#max_slots %d",(int)max_slots); progress.AddString(s);}
	if(dactive_frame!=active_frame)
	{s.Format(L"#active_frame %d",(int)active_frame); progress.AddString(s);}
	if(dactive_frame2!=active_frame2)
	{s.Format(L"#active_frame2 %d",(int)active_frame2); progress.AddString(s);}
	if(dspecial_active!=special_active)
	{s.Format(L"#special_active %d",(int)special_active); progress.AddString(s);}
	if(ddisplay!=display)
	{s.Format(L"#display %d",(int)display); progress.AddString(s);}
	if(dtop_frame!=top_frame)
	{s.Format(L"#top_frame %d",(int)top_frame); progress.AddString(s);}
	if(dtop_used_frame!=top_used_frame)
	{s.Format(L"#top_used_frame %d",(int)top_used_frame); progress.AddString(s);}
	if(dlast_used_frame!=last_used_frame)
	{s.Format(L"#last_used_frame %d",(int)last_used_frame); progress.AddString(s);}
	if(dbottom_used_frame!=bottom_used_frame)
	{s.Format(L"#bottom_used_frame %d",(int)bottom_used_frame); progress.AddString(s);}
	if(dmagon!=(int)magon)
	{s.Format(L"#magon %d",(int)magon); progress.AddString(s);}
	if(dfull_mode!=(int)full_mode)
	{s.Format(L"#full_mode %d",(int)full_mode); progress.AddString(s);}
	if(dplane_mode!=(int)plane_mode)
	{s.Format(L"#plane_mode %d",(int)plane_mode); progress.AddString(s);}
	if(dsaved_pm!=(int)saved_pm)
	{s.Format(L"#saved_pm %d",(int)saved_pm); progress.AddString(s);}
	if(dfix_active!=(int)fix_active)
	{s.Format(L"#fix_active %d",(int)fix_active); progress.AddString(s);}
	if(ddepth_flag!=(int)depth_flag)
	{s.Format(L"#depth_flag %d",(int)depth_flag); progress.AddString(s);}
	if(drainbow!=(int)rainbow)
	{s.Format(L"#rainbow %d",(int)rainbow); progress.AddString(s);}
	if(dsave_bmp!=(int)save_bmp)
	{s.Format(L"#save_bmp %d",(int)save_bmp); progress.AddString(s);}
	if(dshow_fixed!=(int)show_fixed)
	{s.Format(L"#show_fixed %d",(int)show_fixed); progress.AddString(s);}
	if(dfilter_param!=filter_param)
	{s.Format(L"#filter_param %d",(int)filter_param); progress.AddString(s);}
	if(dtab_pressed!=(int)tab_pressed)
	{s.Format(L"#tab_pressed %d",(int)tab_pressed); progress.AddString(s);}
	if(dredraw_filter!=(int)redraw_filter)
	{s.Format(L"#redraw_filter %d",(int)redraw_filter); progress.AddString(s);}
	if(dnoise_level!=noise_level)
	{s.Format(L"#noise_level %d",(int)noise_level); progress.AddString(s);}
	if(ddetail_size!=detail_size)
	{s.Format(L"#detail_size %d",(int)detail_size); progress.AddString(s);}
	if(ddetail_filter!=detail_filter)
	{s.Format(L"#detail_filter %d",(int)detail_filter); progress.AddString(s);}
	if(dmin_island_size!=min_island_size)
	{s.Format(L"#min_island_size %d",(int)min_island_size); progress.AddString(s);}
	if(dpause_enabled!=(int)pause_enabled)
	{s.Format(L"#pause_enabled %d",(int)pause_enabled); progress.AddString(s);}
	if(dskew_amount!=skew_amount)
	{s.Format(L"#skew_amount %d",(int)skew_amount); progress.AddString(s);}
	if(dskew_amountv!=skew_amountv)
	{s.Format(L"#skew_amountv %d",(int)skew_amountv); progress.AddString(s);}
	if(dpatch_size!=patch_size)
	{s.Format(L"#patch_size %d",(int)patch_size); progress.AddString(s);}
	if(djpeg_comp!=jpeg_comp)
	{s.Format(L"#jpeg_comp %d",(int)jpeg_comp); progress.AddString(s);}
	if(dlight_threshold!=light_threshold)
	{s.Format(L"#light_threshold %d",(int)light_threshold); progress.AddString(s);}
	if(ddark_threshold!=dark_threshold)
	{s.Format(L"#dark_threshold %d",(int)dark_threshold); progress.AddString(s);}
	if(dfilter_changed!=(int)filter_changed)
	{s.Format(L"#filter_changed %d",(int)filter_changed); progress.AddString(s);}
	if(dpatch_threshold!=patch_threshold)
	{s.Format(L"#patch_threshold %d",(int)patch_threshold); progress.AddString(s);}
	if(dhdr_bright!=hdr_bright)
	{s.Format(L"#hdr_bright %d",(int)hdr_bright); progress.AddString(s);}
	if(dhdr_hia!=hdr_hia)
	{s.Format(L"#hdr_hia %d",(int)hdr_hia); progress.AddString(s);}
	if(dhdr_log!=hdr_log)
	{s.Format(L"#hdr_log %d",(int)hdr_log); progress.AddString(s);}
	if(ddft_filter_param!=dft.filter_param)
	{s.Format(L"#dft.filter_param %d",(int)dft.filter_param); progress.AddString(s);}
	if(ddft_filtering!=(int)dft.filtering)
	{s.Format(L"#dft.filtering %d",(int)dft.filtering); progress.AddString(s);}
	if(ddft_minr!=dft.minr)
	{s.Format(L"#dft.minr %d",(int)dft.minr); progress.AddString(s);}
	if(ddft_ming!=dft.ming)
	{s.Format(L"#dft.ming %d",(int)dft.ming); progress.AddString(s);}
	if(ddft_minb!=dft.minb)
	{s.Format(L"#dft.minb %d",(int)dft.minb); progress.AddString(s);}
	if(ddft_mode!=dft.mode)
	{s.Format(L"#dft.mode %d",(int)dft.mode); progress.AddString(s);}
	if(ddft_out_mode!=dft.out_mode)
	{s.Format(L"#dft.out_mode %d",(int)dft.out_mode); progress.AddString(s);}
	if(ddft_valid_filter!=(int)dft.valid_filter)
	{s.Format(L"#dft.valid_filter %d",(int)dft.valid_filter); progress.AddString(s);}
	if(ddft_valid_picture1!=(int)dft.valid_picture1)
	{s.Format(L"#dft.valid_picture1 %d",(int)dft.valid_picture1); progress.AddString(s);}
	if(ddft_valid_picture3!=(int)dft.valid_picture3)
	{s.Format(L"#dft.valid_picture3 %d",(int)dft.valid_picture3); progress.AddString(s);}
	if(ddft_valid_store1!=(int)dft.valid_store1)
	{s.Format(L"#dft.valid_store1 %d",(int)dft.valid_store1); progress.AddString(s);}
	if(ddft_valid_store3!=(int)dft.valid_store3)
	{s.Format(L"#dft.valid_store3 %d",(int)dft.valid_store3); progress.AddString(s);}
	if(ddft_valid_transform1!=(int)dft.valid_transform1)
	{s.Format(L"#dft.valid_transform1 %d",(int)dft.valid_transform1); progress.AddString(s);}
	if(ddft_valid_transform3!=(int)dft.valid_transform3)
	{s.Format(L"#dft.valid_transform3 %d",(int)dft.valid_transform3); progress.AddString(s);}
	if(dExtract_redend!=Extract_redend)
	{s.Format(L"#Extract_redend %d",(int)Extract_redend); progress.AddString(s);}
	if(dExtract_bluend!=Extract_bluend)
	{s.Format(L"#Extract_bluend %d",(int)Extract_bluend); progress.AddString(s);}
	if(dExtract_th!=Extract_th)
	{s.Format(L"#Extract_th %d",(int)Extract_th); progress.AddString(s);}
	if(dSqueeze_redend!=Squeeze_redend)
	{s.Format(L"#Squeeze_redend %d",(int)Squeeze_redend); progress.AddString(s);}
	if(dSqueese_bluend!=Squeese_bluend)
	{s.Format(L"#Squeese_bluend %d",(int)Squeese_bluend); progress.AddString(s);}
	if(dSqueeze_break!=Squeeze_break)
	{s.Format(L"#Squeeze_break %d",(int)Squeeze_break); progress.AddString(s);}
	if(dNoise_detect!=Noise_detect)
	{s.Format(L"#Noise_detect %d",(int)Noise_detect); progress.AddString(s);}
	if(dNoise_sub!=Noise_sub)
	{s.Format(L"#Noise_sub %d",(int)Noise_sub); progress.AddString(s);}
	if(dNoise_lim!=Noise_lim)
	{s.Format(L"#Noise_lim %d",(int)Noise_lim); progress.AddString(s);}
	if(dbuffer_count!=buffer_count)
	{s.Format(L"#buffer_count %d",(int)buffer_count); progress.AddString(s);}
	if(dhelp_window!=(int)help_window)
	{s.Format(L"#help_window %d",(int)help_window); progress.AddString(s);}

	if(dmag_bottom!=(int)mag.bottom)
	{s.Format(L"#mag.bottom %d",(int)mag.bottom); progress.AddString(s);}
	if(dmag_left!=(int)mag.left)
	{s.Format(L"#mag.left %d",(int)mag.left); progress.AddString(s);}
	if(dmag_right!=(int)mag.right)
	{s.Format(L"#mag.right %d",(int)mag.right); progress.AddString(s);}
	if(dmag_top!=(int)mag.top)
	{s.Format(L"#mag.top %d",(int)mag.top); progress.AddString(s);}
	if(dSizer_length34!=(int)Sizer.length34)
	{s.Format(L"#Sizer.length34 %d",(int)Sizer.length34); progress.AddString(s);}
	if(dSizer_length12!=(int)Sizer.length12)
	{s.Format(L"#Sizer.length12 %d",(int)Sizer.length12); progress.AddString(s);}
	if(dSizer_angle!=(int)Sizer.angle)
	{s.Format(L"#Sizer.angle %d",(int)Sizer.angle); progress.AddString(s);}
	if(dSizer_ratio!=(int)Sizer.ratio)
	{s.Format(L"#Sizer.ratio %d",(int)Sizer.ratio); progress.AddString(s);}
	if(dSizer_p0_f!=(int)Sizer.p[0].f)
	{s.Format(L"#Sizer.p[0].f %d",(int)Sizer.p[0].f); progress.AddString(s);}
	if(dSizer_p0_valid!=(int)Sizer.p[0].valid)
	{s.Format(L"#Sizer.p[0].valid %d",(int)Sizer.p[0].valid); progress.AddString(s);}
	if(dSizer_p0_x!=(int)Sizer.p[0].x)
	{s.Format(L"#Sizer.p[0].x %d",(int)Sizer.p[0].x); progress.AddString(s);}
	if(dSizer_p0_y!=(int)Sizer.p[0].y)
	{s.Format(L"#Sizer.p[0].y %d",(int)Sizer.p[0].y); progress.AddString(s);}
	if(dSizer_p1_f!=(int)Sizer.p[1].f)
	{s.Format(L"#Sizer.p[1].f %d",(int)Sizer.p[1].f); progress.AddString(s);}
	if(dSizer_p1_valid!=(int)Sizer.p[1].valid)
	{s.Format(L"#Sizer.p[1].valid %d",(int)Sizer.p[1].valid); progress.AddString(s);}
	if(dSizer_p1_x!=(int)Sizer.p[1].x)
	{s.Format(L"#Sizer.p[1].x %d",(int)Sizer.p[1].x); progress.AddString(s);}
	if(dSizer_p1_y!=(int)Sizer.p[1].y)
	{s.Format(L"#Sizer.p[1].y %d",(int)Sizer.p[1].y); progress.AddString(s);}
	if(dSizer_p2_f!=(int)Sizer.p[2].f)
	{s.Format(L"#Sizer.p[2].f %d",(int)Sizer.p[2].f); progress.AddString(s);}
	if(dSizer_p2_valid!=(int)Sizer.p[2].valid)
	{s.Format(L"#Sizer.p[2].valid %d",(int)Sizer.p[2].valid); progress.AddString(s);}
	if(dSizer_p2_x!=(int)Sizer.p[2].x)
	{s.Format(L"#Sizer.p[2].x %d",(int)Sizer.p[2].x); progress.AddString(s);}
	if(dSizer_p2_y!=(int)Sizer.p[2].y)
	{s.Format(L"#Sizer.p[2].y %d",(int)Sizer.p[2].y); progress.AddString(s);}
	if(dSizer_p3_f!=(int)Sizer.p[3].f)
	{s.Format(L"#Sizer.p[3].f %d",(int)Sizer.p[3].f); progress.AddString(s);}
	if(dSizer_p3_valid!=(int)Sizer.p[3].valid)
	{s.Format(L"#Sizer.p[3].valid %d",(int)Sizer.p[3].valid); progress.AddString(s);}
	if(dSizer_p3_x!=(int)Sizer.p[3].x)
	{s.Format(L"#Sizer.p[3].x %d",(int)Sizer.p[3].x); progress.AddString(s);}
	if(dSizer_p3_y!=(int)Sizer.p[3].y)
	{s.Format(L"#Sizer.p[3].y %d",(int)Sizer.p[3].y); progress.AddString(s);}
	if(dangle!=(int)angle)
	{s.Format(L"#angle %d",(int)angle); progress.AddString(s);}
	if(dxorg!=(int)xorg)
	{s.Format(L"#xorg %d",(int)xorg); progress.AddString(s);}
	if(dyorg!=(int)yorg)
	{s.Format(L"#yorg %d",(int)yorg); progress.AddString(s);}
	if(dscale!=(int)scale)
	{s.Format(L"#scale %d",(int)scale); progress.AddString(s);}
	if(dsmallw!=(int)smallw)
	{s.Format(L"#smallw %d",(int)smallw); progress.AddString(s);}
	if(dsmallh!=(int)smallh)
	{s.Format(L"#smallh %d",(int)smallh); progress.AddString(s);}
	if(dsize_factor!=(int)size_factor)
	{s.Format(L"#size_factor %d",(int)size_factor); progress.AddString(s);}
	if(dsizexorg!=(int)sizexorg)
	{s.Format(L"#sizexorg %d",(int)sizexorg); progress.AddString(s);}
	if(dsizeyorg!=(int)sizeyorg)
	{s.Format(L"#sizeyorg %d",(int)sizeyorg); progress.AddString(s);}
	if(doffsetx!=(int)offsetx)
	{s.Format(L"#offsetx %d",(int)offsetx); progress.AddString(s);}
	if(doffsety!=(int)offsety)
	{s.Format(L"#offsety %d",(int)offsety); progress.AddString(s);}
	if(dframe_border!=(int)frame_border)
	{s.Format(L"#frame_border %d",(int)frame_border); progress.AddString(s);}
	if(dgoodrect_bottom!=(int)goodrect.bottom)
	{s.Format(L"#goodrect.bottom %d",(int)goodrect.bottom); progress.AddString(s);}
	if(dgoodrect_top!=(int)goodrect.top)
	{s.Format(L"#goodrect.top %d",(int)goodrect.top); progress.AddString(s);}
	if(dgoodrect_left!=(int)goodrect.left)
	{s.Format(L"#goodrect.left %d",(int)goodrect.left); progress.AddString(s);}
	if(dgoodrect_right!=(int)goodrect.right)
	{s.Format(L"#goodrect.right %d",(int)goodrect.right); progress.AddString(s);}
	if(dbrush_size!=(int)brush_size)
	{s.Format(L"#brush_size %d",(int)brush_size); progress.AddString(s);}
	if(dbrush_delta!=(int)brush_delta)
	{s.Format(L"#brush_delta %d",(int)brush_delta); progress.AddString(s);}
	if(dbufferactive_frame_hidden!=(int)buffer[active_frame].hidden)
	{s.Format(L"#buffer[active_frame].hidden %d",(int)buffer[active_frame].hidden); progress.AddString(s);}
	if(dTle!=(int)Tle)
	{s.Format(L"#Tle %d",(int)Tle); progress.AddString(s);}
	if(dTlt!=(int)Tlt)
	{s.Format(L"#Tlt %d",(int)Tlt); progress.AddString(s);}
	if(dTeq!=(int)Teq)
	{s.Format(L"#Teq %d",(int)Teq); progress.AddString(s);}
	if(dTge!=(int)Tge)
	{s.Format(L"#Tge %d",(int)Tge); progress.AddString(s);}
	if(dTgt!=(int)Tgt)
	{s.Format(L"#Tgt %d",(int)Tgt); progress.AddString(s);}
	if(dTne!=(int)Tne)
	{s.Format(L"#Tne %d",(int)Tne); progress.AddString(s);}
	if(dframe_similarity!=(int)frame_similarity)
	{s.Format(L"#frame_similarity %d",(int)frame_similarity); progress.AddString(s);}

	dspecial_frame=special_frame;
	dbmpprop_size=bmpprop_size;
	dbmpprop_count=bmpprop_count;
	dplevels=plevels;
	dpnumber=pnumber;
	dpnoise=pnoise;
	dpframe=pframe;
	dplast=plast;
	dpdepth=pdepth;
	dpforeground=pforeground;
	dpbrightness=pbrightness;
	dpbackground=pbackground;
	dvideo_w=video_w;
	dvideo_h=video_h;
	dstill_w=still_w;
	dstill_h=still_h;
	dvideo_showing=video_showing;
	dmaxangle=maxangle;
	dbandw=bandw;
	dmovh=movh;
	dmovw=movw;
	dmovq=movq;
	dmovt=movt;
	dsilent=silent;
	dmacro_mode=macro_mode;
	dmode=mode;
	dbackup_picture_valid=backup_picture_valid;
	drefreshscreen=refreshscreen;
	dbuissy=buissy;
	dmemory=(int)memory;
	dmax_slots=max_slots;
	dactive_frame=active_frame;
	dactive_frame2=active_frame2;
	dspecial_active=special_active;
	ddisplay=display;
	dtop_frame=top_frame;
	dtop_used_frame=top_used_frame;
	dlast_used_frame=last_used_frame;
	dbottom_used_frame=bottom_used_frame;
	dmagon=magon;
	dfull_mode=full_mode;
	dplane_mode=plane_mode;
	dsaved_pm=saved_pm;
	dfix_active=fix_active;
	ddepth_flag=depth_flag;
	drainbow=(int)rainbow;
	dsave_bmp=save_bmp;
	dshow_fixed=show_fixed;
	dfilter_param=filter_param;
	dtab_pressed=tab_pressed;
	dredraw_filter=redraw_filter;
	dnoise_level=noise_level;
	ddetail_size=detail_size;
	ddetail_filter=detail_filter;
	dmin_island_size=min_island_size;
	dpause_enabled=pause_enabled;
	dskew_amount=skew_amount;
	dskew_amountv=skew_amountv;
	dpatch_size=patch_size;
	djpeg_comp=jpeg_comp;
	dlight_threshold=light_threshold;
	ddark_threshold=dark_threshold;
	dfilter_changed=filter_changed;
	dpatch_threshold=patch_threshold;
	dhdr_bright=hdr_bright;
	dhdr_hia=hdr_hia;
	dhdr_log=hdr_log;
	ddft_filter_param=dft.filter_param;
	ddft_filtering=dft.filtering;
	ddft_minr=(int)dft.minr;
	ddft_ming=(int)dft.ming;
	ddft_minb=(int)dft.minb;
	ddft_mode=dft.mode;
	ddft_out_mode=dft.out_mode;
	ddft_valid_filter=dft.valid_filter;
	ddft_valid_picture1=dft.valid_picture1;
	ddft_valid_picture3=dft.valid_picture3;
	ddft_valid_store1=dft.valid_store1;
	ddft_valid_store3=dft.valid_store3;
	ddft_valid_transform1=dft.valid_transform1;
	ddft_valid_transform3=dft.valid_transform3;
	dExtract_redend=Extract_redend;
	dExtract_bluend=Extract_bluend;
	dExtract_th=Extract_th;
	dSqueeze_redend=Squeeze_redend;
	dSqueese_bluend=Squeese_bluend;
	dSqueeze_break=Squeeze_break;
	dNoise_detect=Noise_detect;
	dNoise_sub=Noise_sub;
	dNoise_lim=Noise_lim;
	dbuffer_count=buffer_count;
	dhelp_window=(int)help_window;
	dmag_bottom=(int)mag.bottom;
	dmag_left=(int)mag.left;
	dmag_right=(int)mag.right;
	dmag_top=(int)mag.top;
	dSizer_length34=(int)Sizer.length34;
	dSizer_length12=(int)Sizer.length12;
	dSizer_angle=(int)Sizer.angle;
	dSizer_ratio=(int)Sizer.ratio;
	dSizer_p0_f=(int)Sizer.p[0].f;
	dSizer_p0_valid=(int)Sizer.p[0].valid;
	dSizer_p0_x=(int)Sizer.p[0].x;
	dSizer_p0_y=(int)Sizer.p[0].y;
	dSizer_p1_f=(int)Sizer.p[1].f;
	dSizer_p1_valid=(int)Sizer.p[1].valid;
	dSizer_p1_x=(int)Sizer.p[1].x;
	dSizer_p1_y=(int)Sizer.p[1].y;
	dSizer_p2_f=(int)Sizer.p[2].f;
	dSizer_p2_valid=(int)Sizer.p[2].valid;
	dSizer_p2_x=(int)Sizer.p[2].x;
	dSizer_p2_y=(int)Sizer.p[2].y;
	dSizer_p3_f=(int)Sizer.p[3].f;
	dSizer_p3_valid=(int)Sizer.p[3].valid;
	dSizer_p3_x=(int)Sizer.p[3].x;
	dSizer_p3_y=(int)Sizer.p[3].y;
	dangle=(int)angle;
	dxorg=(int)xorg;
	dyorg=(int)yorg;
	dscale=(int)scale;
	dsmallw=(int)smallw;
	dsmallh=(int)smallh;
	dsize_factor=(int)size_factor;
	dsizexorg=(int)sizexorg;
	dsizeyorg=(int)sizeyorg;
	doffsetx=(int)offsetx;
	doffsety=(int)offsety;
	dframe_border=(int)frame_border;
	dgoodrect_bottom=(int)goodrect.bottom;
	dgoodrect_top=(int)goodrect.top;
	dgoodrect_left=(int)goodrect.left;
	dgoodrect_right=(int)goodrect.right;
	dbrush_size=(int)brush_size;
	dbrush_delta=(int)brush_delta;
	dbufferactive_frame_hidden=(int)buffer[active_frame].hidden;
	dTlt=Tlt;
	dTle=Tle;
	dTeq=Teq;
	dTge=Tge;
	dTgt=Tgt;
	dTne=Tne;
	dframe_similarity=frame_similarity;
}

void CCombineDoc::compare_frames()
{
	if(active_frame2<0) return;

	PIXEL* af1=(PIXEL *)GetBuffer(active_frame);
	PIXEL* af2=(PIXEL *)GetBuffer(active_frame2);

	int startx=(int)w-goodrect.right;
	int endx=(int)w-2*startx;
	int starty=h-goodrect.bottom;
	int endy=(int)h-2*starty;

	int x,y,i,Y;

	PIXEL p1,p2;

	__int64 absresult=0;
	__int64 result=0;

	for(y=starty; y<endy; y++)
	{
		Y=y*w;
		for(x=startx; x<endx; x++)
		{
			i=x+Y;
			p1=af1[i];
			p2=af2[i];

			absresult+=abs((int)p1.r-(int)p2.r);
			absresult+=abs((int)p1.g-(int)p2.g);
			absresult+=abs((int)p1.b-(int)p2.b);

			result+=((int)p1.r-(int)p2.r);
			result+=((int)p1.g-(int)p2.g);
			result+=((int)p1.b-(int)p2.b);
		}
	}

	FreeBuffer(active_frame);
	FreeBuffer(active_frame2);

	frame_similarity=(int)(absresult/((endx-startx+1)*(endy-starty+1)));

	if(result==0){Tlt=2; Tle=1; Teq=1; Tge=1; Tgt=2; Tne=2; return;} 
	if(result< 0){Tlt=1; Tle=1; Teq=2; Tge=2; Tgt=2; Tne=1; return;} 
	Tlt=2; Tle=2; Teq=2; Tge=1; Tgt=1; Tne=1;
}

void CCombineDoc::OnFileSetoptions()
{
	COptions dlg;

	dlg.m_reserve_ram=AfxGetApp()->GetProfileInt(L"Memory",L"Reserve",128);
	dlg.m_startup_help=AfxGetApp()->GetProfileInt(L"Help",L"Startup",true);
	dlg.m_startup_simple=AfxGetApp()->GetProfileInt(L"Simple",L"Startup",true);
	dlg.m_save_header=AfxGetApp()->GetProfileInt(L"Header",L"Save",true);
	dlg.m_cores=AfxGetApp()->GetProfileInt(L"Cores",L"CPU",pam);
	dlg.m_miniprogress=AfxGetApp()->GetProfileInt(L"Minimized",L"Progress",false);

	dlg.DoModal();

	AfxGetApp()->WriteProfileInt(L"Memory",L"Reserve",dlg.m_reserve_ram);
	AfxGetApp()->WriteProfileInt(L"Help",L"Startup",dlg.m_startup_help);
	AfxGetApp()->WriteProfileInt(L"Simple",L"Startup",dlg.m_startup_simple);
	AfxGetApp()->WriteProfileInt(L"Header",L"Save",dlg.m_save_header);
	AfxGetApp()->WriteProfileInt(L"Minimized",L"Progress",dlg.m_miniprogress);

	DWORD sam;
	if(GetProcessAffinityMask(GetCurrentProcess(),(DWORD*)&pam,&sam))
	{
		int bit,bits=0;
		for(bit=0; bit<31; bit++)
			bits=bits+(((long)pam>>bit)&1);
		pam=bits;
	}
	else
		pam=1;
	if(dlg.m_cores==0) dlg.m_cores=pam;
	if(dlg.m_cores<(int)pam) pam=(unsigned int)dlg.m_cores;
	AfxGetApp()->WriteProfileInt(L"Cores",L"CPU",pam);
}

#include "ScaleLine.h"
#include "gdiplus.h"
#define LCM 55440  // LCM of 1 ... 16

void CCombineDoc::OnFrameCreatescaleline()
{
	int saved_active_frame=active_frame;

	CScaleLine sl;
	if((Sizer.p[1].valid) && (Sizer.p[0].valid))
	{
		sl.m_pixelsx=abs(Sizer.p[1].x-Sizer.p[0].x);
		sl.m_pixelsy=abs(Sizer.p[1].y-Sizer.p[0].y);
	} else sl.m_pixelsy=sl.m_pixelsx=0;

	int ret;
	ret=sl.DoModal();
	if(ret==IDCANCEL) return;

	int x,y,i,j,len,pips,pcount,border,pws,pwl,bgw,bgh,interpip;
	int lincolour,bgcolour;
	float outdelta;

	pws=h/40; if(pws%2!=2) pws++;
	pwl=pws*2;
	bgh=pws*4;
	border=2*pws;

	CString s,s1;
	__int64 outlen;

	
	progress.AddString(IDS_SCALELINE);
	s.LoadString(IDS_STRING659);
	s1.Format(s,buffer[top_frame-1].name);
	progress.AddString(s1);

	switch(sl.m_outunits)
	{
	case 0:
		s1=L"0 nm";
		outlen=sl.m_sloutlen;
		break;
	case 1:
		s1=L"0 µm";
		outlen=sl.m_sloutlen*1000;
		break;
	case 2:
		s1=L"0 mm";
		outlen=sl.m_sloutlen*1000000;
		break;
	case 3:
		s1=L"0 cm";
		outlen=sl.m_sloutlen*10000000;
		break;
	case 4:
		s1=L"0 m";
		outlen=sl.m_sloutlen*1000000000;
		break;
	}

	pips=sl.m_slpips;
	len=(int)(((__int64)LCM*sl.m_slcalpixels*outlen)/(__int64)sl.m_inlen);
	if(pips<2) pips=1;
	if(pips>len/3) pips=len/3;
	outdelta=sl.m_sloutlen/(float)pips;
	bgw=len+border*LCM*2;
	interpip=len/pips;

	len/=LCM;
	bgw/=LCM;

	int font_size;
	if(pws>=12) font_size=pws; else font_size=12; 

	int rw,rh;
	if(sl.m_orient==0)
	{
		rw=bgw;
		rh=bgh;
		if((bgw>=goodrect.Width()-8) || (len<pips*5))
		{
			MessageBox(progress.m_hWnd,L"The Line just requested is too big or too small to draw on a picture of this size.   OR you have requested too many subdivisions.",L"Scale Line",MB_OK);
			return;
		}
	} else {
		if((bgw-2*border+2*pws>=goodrect.Height()-8) || (len<pips*5))
		{
			MessageBox(progress.m_hWnd,L"The Line just requested is too big or too small to draw on a picture of this size.   OR you have requested too many subdivisions.",L"Scale Line",MB_OK);
			return;
		}
		pws/=2;
		pwl/=2;
		bgw=bgw-2*border+2*pws;
		bgh+=border/2;
		rw=bgh;
		rh=bgw;
	}

	int topleftx,toplefty;
	switch(sl.m_position)
	{
	case 0:
		topleftx=goodrect.left+4;
		toplefty=goodrect.top+4;
		break;
	case 1:
		topleftx=(goodrect.left+goodrect.right-rw)/2;
		toplefty=goodrect.top+4;
		break;
	case 2:
		topleftx=goodrect.right-16-rw;
		toplefty=goodrect.top+4;
		break;
	case 3:
		topleftx=goodrect.left+4;
		toplefty=(goodrect.bottom-4-rh)/2;
		break;
	case 4:
		topleftx=goodrect.right-16-rw;
		toplefty=(goodrect.bottom-4-rh)/2;
		break;
	case 5:
		topleftx=goodrect.left+4;
		toplefty=goodrect.bottom-4-rh;
		break;
	case 6:
		topleftx=(goodrect.left+goodrect.right-rw)/2;
		toplefty=goodrect.bottom-4-rh;
		break;
	case 7:
		topleftx=goodrect.right-16-rw;
		toplefty=goodrect.bottom-4-rh;
		break;
	}

	str1=L"ScaleLine";
	int *b=(int*)GetBuffer(top_frame);
	buffer[top_frame].name=newnumber(str1);
	buffer[top_frame].hidden=true;
	top_frame++;

	active_frame=top_frame-1;
	display=top_frame-1;
	UpdateBuffer(top_frame-1);
	refresh_used_list();

	for(i=0; i<w*h; i++) b[i]=0x00000100|aCLEAR<<24;

	using namespace Gdiplus;
	{
		Bitmap bm(w,h,4*w,PixelFormat32bppRGB,(BYTE*)b);
		Graphics g(&bm);
		FontFamily fontFamily(L"Arial");
		Font font(&fontFamily,(REAL)font_size,FontStyleBold,UnitPixel);
		PointF pointF;
		switch(sl.m_colours)
		{
		case 0:
			bgcolour=0x00ffffff;
			lincolour=0x00000000;
			break;
		case 1:
			bgcolour=0x00808080;
			lincolour=0x00ffffff;
			break;
		case 2:
			lincolour=0x00000000;
			break;
		case 3:
			lincolour=0x00ffffff;
			break;
		}

		SolidBrush brush(lincolour|0xff000000);

		if(sl.m_orient==0)
		{
			if(sl.m_colours<2)
			{
				for(y=0; y<bgh; y++)
				{
					for(x=0; x<bgw; x++)
					{
						i=(x+topleftx)+(y+toplefty)*w;
						b[i]=bgcolour|aGOOD<<24;
					}
				}
			}
			pcount=0;
			for(x=0; x<=len; x++)
			{
				i=(x+topleftx)+border+(pws/2+toplefty)*w;
				if((interpip>20*LCM) && (w>=2000))
				{
					b[i-2*w]=lincolour|aGOOD<<24;
					b[i+2*w]=0x00888888|aGOOD<<24;
				}
				if((interpip>10*LCM) && (w>=1000))
					b[i-w]=lincolour|aGOOD<<24;
				b[i]=lincolour|aGOOD<<24;
				b[i+w]=0x00888888|aGOOD<<24;
				if(abs(x*LCM-interpip*pcount)<LCM)
				{
					for(y=0; y<pwl; y++)
					{
						j=(x+topleftx)+border+(y+pws/2+toplefty)*w;
						if((interpip>20*LCM) && (w>=2000))
						{
							b[j-2]=lincolour|aGOOD<<24;
							b[j+2]=0x00888888|aGOOD<<24;
						}
						if((interpip>10*LCM) && (w>=1000))
							b[j-1]=lincolour|aGOOD<<24;
						b[j]=lincolour|aGOOD<<24;
						b[j+1]=0x00888888|aGOOD<<24;
						if((y>=pws) && (pcount%5!=0) && (pcount!=pips)) break;
					}
					if(y==pwl)
					{
						if(x==0) s=s1;
						else s.Format(L"%1.4g",pcount*outdelta);
						pointF.X=(REAL)(x+border-pws/2+topleftx);
						pointF.Y=(REAL)((pws*3-pws/2+toplefty));
						g.DrawString(s,-1,&font,pointF,&brush);
					}
					pcount++;
				}
			}
			for(y=0; (y<bgh+16) && (y<h); y++)
			{
				for(x=0; x<(bgw+64) && (x<w); x++)
				{
					i=(x+topleftx)+(y+toplefty)*w;
					if((b[i]&0x00ffffff)!=0x00000100) b[i]=(b[i]&0x00ffffff)|(aGOOD<<24);
					else b[i]=0x00007f00|aCLEAR<<24;
				}
			}
		}
		if(sl.m_orient==1)
		{
			if(sl.m_colours<2)
			{
				for(y=0; y<bgw; y++)
				{
					for(x=0; x<bgh; x++)
					{
						i=(x+topleftx)+(y+toplefty)*w;
						b[i]=bgcolour|aGOOD<<24;
					}
				}
			}
			pcount=0;
			for(y=0; y<=len; y++)
			{
				i=(y+pws+toplefty)*w+pws+topleftx;
				if((interpip>20*LCM) && (h>=2000))
				{
					b[i-2]=lincolour|aGOOD<<24;
					b[i+2]=0x00888888|aGOOD<<24;
				}
				if((interpip>10*LCM) && (h>=1000))
					b[i-1]=lincolour|aGOOD<<24;
				b[i]=lincolour|aGOOD<<24;
				b[i+1]=0x00888888|aGOOD<<24;
				if(abs(y*LCM-interpip*pcount)<LCM)
				{
					for(x=0; x<pwl; x++)
					{
						j=(y+pws+toplefty)*w+x+pws+topleftx;
						if((interpip>20*LCM) && (h>=2000))
						{
							b[j-2*w]=lincolour|aGOOD<<24;
							b[j+2*w]=0x00888888|aGOOD<<24;
						}
						if((interpip>10*LCM) && (h>=1000))
							b[j-w]=lincolour|aGOOD<<24;
						b[j]=lincolour|aGOOD<<24;
						b[j+w]=0x00888888|aGOOD<<24;
						if((x>=pws) && ((pips-pcount)%5!=0) && ((pips-pcount)!=pips)) break;
					}
					if(x==pwl)
					{
						if(y==len) s=s1;
						else s.Format(L"%1.4g",(pips-pcount)*outdelta);
						pointF.Y=(REAL)(y-2+toplefty);
						pointF.X=(REAL)(pws*4+topleftx);
						g.DrawString(s,-1,&font,pointF,&brush);
					}
					pcount++;
				}
			}
			for(y=0; (y<bgw+16) && (y<h); y++)
			{
				for(x=0; (x<bgh+64) && (x<w); x++)
				{
					i=(x+topleftx)+(y+toplefty)*w;
					if((b[i]&0x00ffffff)!=0x00000100) b[i]=(b[i]&0x00ffffff)|(aGOOD<<24);
					else b[i]=0x00007f00|aCLEAR<<24;
				}
			}
		}
	}

	active_frame2=top_frame-1;
	active_frame=display=saved_active_frame;

	OnSpecialBlend();
}
