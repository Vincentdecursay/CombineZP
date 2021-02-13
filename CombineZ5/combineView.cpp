
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
// combineView.cpp : implementation of the CCombineView class
//


/////////////////////////////////////////////////////////////////////////////
// CCombineView

#include "stdafx.h"
#include "combine.h"

#include "combineDoc.h"
#include "combineView.h"
#include "size1.h"
#include "progress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CCombineView, CView)

BEGIN_MESSAGE_MAP(CCombineView, CView)
	//{{AFX_MSG_MAP(CCombineView)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYUNDER, OnUpdateCopyunder)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_RELOAD, OnUpdateReload)
	ON_UPDATE_COMMAND_UI(ID_FILE_RELOADALL, OnUpdateReloadall)
	ON_UPDATE_COMMAND_UI(ID_FILE_REMOVE, OnUpdateRemove)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEFRAME, OnUpdateSaveframe)
	ON_UPDATE_COMMAND_UI(ID_FRAME_DONTUSE, OnUpdateDontuse)
	ON_UPDATE_COMMAND_UI(ID_FRAME_USE, OnUpdateUse)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_RESIZEALL, OnUpdateResizeall)
	ON_UPDATE_COMMAND_UI(ID_SAVEALLFRAMES, OnUpdateSaveallframes)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS_ALLFRAMES, OnUpdateAllframes)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS_MAKEFIRST, OnUpdateMakefirst)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS_MAKELAST, OnUpdateMakelast)
	ON_UPDATE_COMMAND_UI(ID_SIZE_CALCULATE, OnUpdateCalculate)
	ON_UPDATE_COMMAND_UI(M_FIX, OnUpdateFix)
	ON_UPDATE_COMMAND_UI(M_Transparent, OnUpdateTransparent)
	ON_UPDATE_COMMAND_UI(M_UNFIX, OnUpdateUnfix)
	ON_UPDATE_COMMAND_UI(ID_SIZE_CLEARPOINTS, OnUpdateClearpoints)
	ON_UPDATE_COMMAND_UI(ID_PROCESS_BALANCE, OnUpdateBalance)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCopy)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_ACTIVEBULU, OnUpdateActivebulu)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_ACTIVEGREEN, OnUpdateActivegreen)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_ACTIVERED, OnUpdatelActivered)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_MAKETRANSPARENT, OnUpdateMaketransparent)
	ON_UPDATE_COMMAND_UI(ID_FRAME_DECIMATE, OnUpdateFrameDecimate)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOGGLEPLANEMODE, OnUpdateViewToggleplanemode)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOGGLEFIX, OnUpdateViewTogglefix)
	ON_UPDATE_COMMAND_UI(ID_FRAME_RESIZEROTATEMOVE, OnUpdateFrameResizerotatemove)
	ON_UPDATE_COMMAND_UI(ID_FRAME_ALIGNACTIVE, OnUpdateFrameAlignactive)
	ON_UPDATE_COMMAND_UI(ID_RECTANGLE_UNDO, OnUpdateRectangleUndo)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_CONTRAST, OnUpdateSpecialContrast)
	ON_UPDATE_COMMAND_UI(ID_FRAME_NEWFROMPICTURE, OnUpdateFrameNewfrompicture)
	ON_UPDATE_COMMAND_UI(ID_FRAME_REPLACE, OnUpdateFrameReplace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESTOREPICTURE, OnUpdateViewRestorepicture)
	ON_UPDATE_COMMAND_UI(M_CLEAR, OnUpdateClear)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VIEWPICTURE, OnUpdateViewViewpicture)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_COMMAND(ID_VIEW_TOGGLEPLANEMODE, OnViewToggleplanemode)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVERECTANGLE, OnUpdateFileSaverectangle)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOGGLEDEPTHMAP, OnUpdateViewToggledepthmap)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULLMODE, OnUpdateViewFullmode)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOADDEPTHINFORMATION, OnUpdateFileLoaddepthinformation)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_LEAVESPECIALMODE, OnUpdateSpecialLeavespecialmode)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_CORRECTBRIGHTNESS, OnUpdateSpecialCorrectBrightness)
	ON_UPDATE_COMMAND_UI(ID_RECTANGLE_INVERTFIXED, OnUpdateRectangleInvertfixed)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_BRIGHTNESS, OnUpdateSpecialBrightness)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_RESET, OnUpdateSpecialReset)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SELECTALL, OnUpdateViewSelectall)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_BLEND, OnUpdateSpecialBlend)
	ON_UPDATE_COMMAND_UI(ID_FRAME_MAKEMONOCHROME, OnUpdateFrameMakemonochrome)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_ADD, OnUpdateSpecialAdd)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORTDEPTHMAP, OnUpdateFileImportdepthmap)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, OnUpdateFileImport)
	ON_UPDATE_COMMAND_UI(ID_FRAME_MAKEBG, OnUpdateFrameMakebg)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTRECTANGLES, OnUpdateFileExportrectangles)
	ON_UPDATE_COMMAND_UI(ID_FILTER_CREATELOWPASS, OnUpdateFilterCreatelowpass)
	ON_UPDATE_COMMAND_UI(ID_FILTER_CREATEHIGHPASS, OnUpdateFilterCreatehighpass)
	ON_UPDATE_COMMAND_UI(ID_FILTER_CREATEBANDPASS, OnUpdateFilterCreatebandpass)
	ON_UPDATE_COMMAND_UI(ID_FILTER_CREATEBUTTERWORTH, OnUpdateFilterCreatebutterworth)
	ON_UPDATE_COMMAND_UI(ID_FILTER_FILTERACTIVE, OnUpdateFilterFilteractive)
	ON_UPDATE_COMMAND_UI(ID_FILTER_CREATEBANDSTOP, OnUpdateFilterCreatebandstop)
	ON_UPDATE_COMMAND_UI(ID_FILTER_THRESHOLD, OnUpdateFilterThreshold)
	ON_UPDATE_COMMAND_UI(ID_FILTER_BRIGHTNESS, OnUpdateFilterBrightness)
	ON_UPDATE_COMMAND_UI(ID_FILTER_RESUMEFILTERING, OnUpdateFilterResumefiltering)
	ON_UPDATE_COMMAND_UI(ID_FILTER_TRANSFORMARITHMETIC_MAKEXFROMACTIVEFRAME, OnUpdateFilterTransformarithmeticMakexfromactiveframe)
	ON_UPDATE_COMMAND_UI(ID_FILTER_FILTERALLFRAMES, OnUpdateFilterFilterallframes)
	ON_UPDATE_COMMAND_UI(ID_FRAME_REVERSEORDER, OnUpdateFrameReverseorder)
	ON_UPDATE_COMMAND_UI(ID_FILTER_TRANSFORMARITHMETIC_MAKEXFROMSUMOFSTACK, OnUpdateFilterTransformarithmeticMakexfromsumofstack)
	ON_UPDATE_COMMAND_UI(ID_FILTER_MAKEXFROMDEPTHMAP, OnUpdateFilterMakexfromdepthmap)
	ON_UPDATE_COMMAND_UI(ID_FILTER_REDISPLAYLASTOUTPUT, OnUpdateFilterRedisplaylastoutput)
	ON_UPDATE_COMMAND_UI(ID_FRAME_TIDYEDGES, OnUpdateFrameTidyedges)
	ON_UPDATE_COMMAND_UI(ID_FILTER_FREEMEMORY, OnUpdateFilterFreememory)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LASTOUTPUT, OnUpdateViewOutput)
	ON_UPDATE_COMMAND_UI(ID_FILE_WORKFRAMES_SAVEWORK, OnUpdateFileWorkframesSavework)
	ON_UPDATE_COMMAND_UI(ID_FILE_RESUMEWORK, OnUpdateFileResumework)
	ON_UPDATE_COMMAND_UI(ID_STACK_DETAIL, OnUpdateStackDetail)
	ON_UPDATE_COMMAND_UI(ID_STACK_DEPTHFLOW, OnUpdateStackDepthflow)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_MAKEVISIBLEACTIVEFRAME2, OnUpdateSpecialMakevisibleactiveframe2)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEARUNFIXED, OnUpdateEditClearunfixed)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_ACTIVEFRAMEACTIVEFRAME2, OnUpdateSpecialActiveframeactiveframe2)
	ON_UPDATE_COMMAND_UI(ID_STACK_REMOVEISLANDS, OnUpdateStackRemoveislands)
	ON_UPDATE_COMMAND_UI(ID_STACK_REMOVESTEPS, OnUpdateStackRemovesteps)
	ON_UPDATE_COMMAND_UI(ID_FILE_WORKFRAMES_RELOADDEPTH, OnUpdateFileWorkframesReloaddepth)
	ON_UPDATE_COMMAND_UI(ID_FILE_WORKFRAMES_SAVEDEPTH, OnUpdateFileWorkframesSavedepth)
	ON_UPDATE_COMMAND_UI(ID_UNDO_SETUNDOPOINT, OnUpdateUndoSetundopoint)
	ON_UPDATE_COMMAND_UI(ID_STACK_INTERPOLATEDOUTPUT, OnUpdateStackInterpolate)
	ON_UPDATE_COMMAND_UI(ID_FILTER_IMMEDIATE_LASTOUTPUTTODEPTH, OnUpdateFiltertodepth)
	ON_WM_MOUSEMOVE()
	ON_UPDATE_COMMAND_UI(ID_STACK_SIZEANDALIGNMENT_AUTOMATICREGULARSTACK, OnUpdateStackSizeandalignmentAutomaticregularstack)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTDEPTHMAP, OnUpdateFileExportdepthmap)
	ON_UPDATE_COMMAND_UI(ID_MACRO1, OnUpdateMacro1)
	ON_UPDATE_COMMAND_UI(ID_MACRO10, OnUpdateMacro10)
	ON_UPDATE_COMMAND_UI(ID_MACRO2, OnUpdateMacro2)
	ON_UPDATE_COMMAND_UI(ID_MACRO3, OnUpdateMacro3)
	ON_UPDATE_COMMAND_UI(ID_MACRO4, OnUpdateMacro4)
	ON_UPDATE_COMMAND_UI(ID_MACRO5, OnUpdateMacro5)
	ON_UPDATE_COMMAND_UI(ID_MACRO6, OnUpdateMacro6)
	ON_UPDATE_COMMAND_UI(ID_MACRO7, OnUpdateMacro7)
	ON_UPDATE_COMMAND_UI(ID_MACRO8, OnUpdateMacro8)
	ON_UPDATE_COMMAND_UI(ID_MACRO9, OnUpdateMacro9)
	ON_UPDATE_COMMAND_UI(ID_MACRO_ENABLEPAUSE, OnUpdateMacroEnablepause)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOGGLEFRAMESA1A2, OnUpdateViewToggleframesa1a2)
	ON_UPDATE_COMMAND_UI(ID_FRAME_COPYVISIBLETOOUT, OnUpdateFrameCopyvisibletoout)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GOTOFRAME, OnUpdateViewGotoframe)
	ON_UPDATE_COMMAND_UI(ID_STACK_HORIZONTALSKEW, OnUpdateStackHorizontalskew)
	ON_UPDATE_COMMAND_UI(ID_STACK_PATCHWORK, OnUpdateStackPatchwork)
	ON_UPDATE_COMMAND_UI(ID_FILTER_INPUT_WEIGHTEDSUM, OnUpdateFilterInputWeightedsum)
	ON_UPDATE_COMMAND_UI(ID_STACK_DETAIL_LIGHTANDDARK, OnUpdateStackDetailLightanddark)
	ON_WM_SETCURSOR()
	ON_WM_CHAR()
	ON_UPDATE_COMMAND_UI(ID_FRAME_REPLACE, OnUpdateFrameReplace)
	ON_UPDATE_COMMAND_UI(ID_STACK_BALANCECOLOURANDBRIGHTNESS_LOCAL, OnUpdateStackBalancecolourandbrightnessLocal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CCombineView::CCombineView()
{
	first_time=true;
	shift=false;
	lbdown=false;
	orgx=orgy=0;
	hon=von=false;
}

CCombineView::~CCombineView()
{
}

BOOL CCombineView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

#ifdef _DEBUG
void CCombineView::AssertValid() const
{
	CView::AssertValid();
}

void CCombineView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCombineDoc* CCombineView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCombineDoc)));
	return (CCombineDoc*)m_pDocument;
}
#endif


void CCombineView::OnSize(UINT nType, int cx, int cy) 
{
	CCombineDoc* doc;
	doc=GetDocument();
	int h,w;

	h=doc->h-(cy-2*BORDER);
	w=doc->w-(cx-2*BORDER);

	if(doc->magon) return;
	else
	{
//		orgx=orgy=0;
		if(hon)
		{
			SetScrollRange(SB_HORZ,0,w,false);
			SetScrollPos(SB_HORZ,orgx);
		}
		if(von)
		{
			SetScrollRange(SB_VERT,0,h,false);
			SetScrollPos(SB_VERT,orgy);
		}
	}
}

void CCombineView::scroll()
{
	CCombineDoc* doc;
	doc=GetDocument();
	bool ud=false;

	if(doc->magon && !doc->full_mode) return;

	if(doc->full_mode)
	{
		if(hon)
		{
			ShowScrollBar(SB_HORZ,false);
			hon=false;
			ud=true;
		}
		if(von)
		{
			ShowScrollBar(SB_VERT,false);
			von=false;
			ud=true;
		}
		if(ud) UpdateWindow();
		return;
	}

	CRect r;
	int h,w;

	GetClientRect(&r);
	r.left+=BORDER;
	r.right-=BORDER;
	r.top+=BORDER;
	r.bottom-=BORDER;
	int wscroll=GetSystemMetrics(SM_CXVSCROLL);
	int hscroll=GetSystemMetrics(SM_CYHSCROLL);
	r.right+=wscroll;
	r.bottom+=hscroll;

	if((int)doc->w>r.Width()+1)
	{
		w=doc->w-r.Width()-1;
		SetScrollRange(SB_HORZ,0,w,false);
		SetScrollPos(SB_HORZ,orgx,false);
		if(!hon) ShowScrollBar(SB_HORZ,true);
		hon=true;
	} else {
		if(hon) ShowScrollBar(SB_HORZ,false);
		hon=false;
	}

	if((int)doc->h>r.Height())
	{
		h=doc->h-r.Height();
		SetScrollRange(SB_VERT,0,h,false);
		SetScrollPos(SB_VERT,orgy,false);
		if(!von) ShowScrollBar(SB_VERT,true);
		von=true;
	} else {
		if(von) ShowScrollBar(SB_VERT,false);
		von=false;
	}
}

void CCombineView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CCombineDoc* doc;
	doc=GetDocument();
	CRect r;
	int xl,xr,w;

	if(doc->magon)
	{
		xl=doc->mag.left;
		xr=doc->mag.right;
		w=doc->mag.Width()+1;
		switch(nSBCode)
		{
		case SB_LEFT:
		case SB_LINELEFT:
			if(xl>0)
				doc->mag.left-=1;
			else
				doc->mag.left=0;
			break;
		case SB_PAGELEFT:
			if(xl>24)
				doc->mag.left-=25;
			else
				doc->mag.left=0;
			break;
		case SB_PAGERIGHT:
			if(xr<doc->w-25)
				doc->mag.left+=25;
			else
				doc->mag.left=doc->w-w;
			break;
		case SB_RIGHT:
		case SB_LINERIGHT:
			if(xr<doc->w-1)
				doc->mag.left+=1;
			else
				doc->mag.left=doc->w-w;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			doc->mag.left=nPos;
			break;
		}
		doc->mag.right=doc->mag.left+w-1;
		SetScrollPos(SB_HORZ,doc->mag.left,true);
		InvalidateRect(0,false);
		return;
	}

	GetClientRect(&r);
	r.left+=BORDER;
	r.right-=BORDER;
	r.top+=BORDER;
	r.bottom-=BORDER;

	w=doc->w-r.Width();

	switch(nSBCode)
	{
	case SB_LEFT:
		if(orgx>4) orgx-=5;
		else orgx=0;
		break;
	case SB_LINELEFT:
		if(orgx>24) orgx-=25;
		else orgx=0;
		break;
	case SB_LINERIGHT:
		if(orgx<(int)w-25) orgx+=25;
		else orgx=w;
		break;
	case SB_PAGELEFT:
		if(orgx>99) orgx-=100;
		else orgx=0;
		break;
	case SB_PAGERIGHT:
		if(orgx<(int)w-100) orgx+=100;
		else orgx=w;
		break;
	case SB_RIGHT:
		if(orgx<(int)w-5) orgx+=5;
		else orgx=w;
		break;
	case SB_THUMBPOSITION:
		orgx=nPos;
		break;
	case SB_THUMBTRACK:
		orgx=nPos;
		break;
	}
	doc->xorg=orgx;
	SetScrollPos(SB_HORZ,orgx,true);
	InvalidateRect(0,false);
}

void CCombineView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CCombineDoc* doc;
	doc=GetDocument();
	CRect r;
	int yl,yr,h;

	if(doc->magon)
	{
		yl=doc->mag.top;
		yr=doc->mag.bottom;
		h=doc->mag.Height()+1;
		switch(nSBCode)
		{
		case SB_LEFT:
		case SB_LINELEFT:
			if(yl>0)
				doc->mag.top-=1;
			else
				doc->mag.top=0;
			break;
		case SB_PAGELEFT:
			if(yl>24)
				doc->mag.top-=25;
			else
				doc->mag.top=0;
			break;
		case SB_PAGERIGHT:
			if(yr<(int)doc->h-25)
				doc->mag.top+=25;
			else
				doc->mag.top=doc->h-h;
			break;
		case SB_RIGHT:
		case SB_LINERIGHT:
			if(yr<(int)doc->h-1)
				doc->mag.top+=1;
			else
				doc->mag.top=doc->h-h;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			doc->mag.top=nPos;
			break;
		}
		doc->mag.bottom=doc->mag.top+h-1;
		SetScrollPos(SB_VERT,doc->mag.top,true);
		InvalidateRect(0,false);
		return;
	}

	GetClientRect(&r);
	r.left+=BORDER;
	r.right-=BORDER;
	r.top+=BORDER;
	r.bottom-=BORDER;
	h=doc->h-r.Height();

	switch(nSBCode)
	{
	case SB_LEFT:
		if(orgy>4) orgy-=5;
		else orgy=0;
		break;
	case SB_LINELEFT:
		if(orgy>24) orgy-=25;
		else orgy=0;
		break;
	case SB_LINERIGHT:
		if(orgy<(int)h-25) orgy+=25;
		else orgy=h;
		break;
	case SB_PAGELEFT:
		if(orgy>99) orgy-=100;
		else orgy=0;
		break;
	case SB_PAGERIGHT:
		if(orgy<(int)h-100) orgy+=100;
		else orgy=h;
		break;
	case SB_RIGHT:
		if(orgy<(int)h-5) orgy+=5;
		else orgy=h;
		break;
	case SB_THUMBPOSITION:
		orgy=nPos;
		break;
	case SB_THUMBTRACK:
		orgy=nPos;
		break;
	}
	doc->yorg=orgy;
	SetScrollPos(SB_VERT,orgy,true);
	InvalidateRect(0,false);
}

void CCombineView::MagscrollOn()
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->mag.Width()+1<(int)doc->w)
	{
		SetScrollRange(SB_HORZ,0,doc->w-doc->mag.Width()-1,false);
		SetScrollPos(SB_HORZ,doc->mag.left,false);
		von=true;
		ShowScrollBar(SB_HORZ,true);
	} else {
		von=false;
		ShowScrollBar(SB_HORZ,false);
	}

	if(doc->mag.Height()+1<(int)doc->h)
	{
		SetScrollRange(SB_VERT,0,doc->h-doc->mag.Height()-1,false);
		SetScrollPos(SB_VERT,doc->mag.top,false);
		hon=true;
		ShowScrollBar(SB_VERT,true);
	} else {
		hon=false;
		ShowScrollBar(SB_VERT,false);
	}
}

void CCombineView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	hon=von=false;

/*	CCombineDoc* doc;
	doc=GetDocument();
	doc->InitializeMacros();
*/}

/*BOOL CCombineView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CView::OnSetCursor(pWnd, nHitTest, message);
}
*/
void CCombineView::new_doc()
{
	CCombineDoc* doc;
	doc=GetDocument();
	orgx=orgy=0;
	ShowScrollBar(SB_HORZ,false);
	hon=false;
	ShowScrollBar(SB_VERT,false);
	von=false;
	UpdateWindow();

	doc->full_mode=doc->magon=true;
	doc->new_flag=false;
	doc->old_mag=doc->mag;
	doc->special_active=doc->active_frame;
}

void CCombineView::pagescroll(int horiz, int vert)
{
	CCombineDoc* doc;
	doc=GetDocument();
	if(doc->full_mode) return;
	
	CRect r;
	int w,h;

	GetClientRect(&r);
	r.left+=BORDER;
	r.right-=BORDER;
	r.top+=BORDER;
	r.bottom-=BORDER;

	if(hon)
	{
		w=doc->w-r.Width();
		orgx+=horiz*r.Width();
		if(orgx<0){orgx=0; /*MessageBeep(MB_ICONASTERISK);*/}
		if(orgx>w-1){orgx=w-1; /*MessageBeep(MB_ICONASTERISK);*/}
		doc->xorg=orgx;
		SetScrollPos(SB_HORZ,orgx,true);
	}

	if(von)
	{
		h=doc->h-r.Height();
		orgy+=vert*r.Height();
		if(orgy<0){orgy=0; /*MessageBeep(MB_ICONASTERISK);*/}
		if(orgy>h-1){orgy=h-1; /*MessageBeep(MB_ICONASTERISK);*/}
		doc->yorg=orgy;
		SetScrollPos(SB_VERT,orgy,true);
	}

	InvalidateRect(0,false);
}

void CCombineView::pagetl()
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->full_mode) return;
	if(doc->magon) return;
	
	if(hon)
	{
		doc->xorg=orgx=0;
		SetScrollPos(SB_HORZ,orgx,true);
	}

	if(von)
	{
		doc->yorg=orgy=0;
		SetScrollPos(SB_VERT,orgy,true);
	}

	InvalidateRect(0,false);
}

void CCombineView::pagebr()
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->full_mode) return;
	if(doc->magon) return;
	
	CRect r;
	int w,h;

	GetClientRect(&r);
	r.left+=BORDER;
	r.right-=BORDER;
	r.top+=BORDER;
	r.bottom-=BORDER;

	if(hon)
	{
		w=doc->w-r.Width();
		orgx=w-1;
		doc->xorg=orgx;
		SetScrollPos(SB_HORZ,orgx,true);
	}

	if(von)
	{
		h=doc->h-r.Height();
		orgy=h-1;
		doc->yorg=orgy;
		SetScrollPos(SB_VERT,orgy,true);
	}

	InvalidateRect(0,false);
}

