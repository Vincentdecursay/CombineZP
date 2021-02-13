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
#include "combine.h"

#include "combineDoc.h"
#include "combineView.h"
#include "size1.h"


/*void CCombineView::OnUpdateSizecompensation(CCmdUI* pCmdUI)
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL)  || doc->plane_mode) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}
*/
void CCombineView::OnUpdateEditClearunfixed(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateCopyunder(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL)  || doc->plane_mode) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateOpen(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame<1) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateReload(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateReloadall(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateRemove(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if(doc->display==OUT_BUFFER) pCmdUI->Enable(false);
	if(doc->display==PICTURE_BUFFER) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateSaveframe(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateDontuse(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateUse(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateResizeall(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL)  || doc->plane_mode) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if(doc->top_frame==1) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateSaveallframes(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateAllframes(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateMakefirst(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateMakelast(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateCalculate(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFix(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateTransparent(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL) || doc->plane_mode) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateUnfix(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateClearpoints(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateBalance(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL) || doc->plane_mode) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if(doc->top_frame==1) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}


void CCombineView::OnUpdateStackBalancecolourandbrightnessLocal(CCmdUI* pCmdUI) 
{
	OnUpdateBalance(pCmdUI);
}

void CCombineView::OnUpdateCopy(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL) || doc->plane_mode) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateMaketransparent(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL) || doc->plane_mode) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFrameDecimate(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL) || doc->plane_mode) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if(doc->top_frame==1) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateViewToggleplanemode(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL)) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateViewTogglefix(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL)) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFrameResizerotatemove(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL)) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFrameAlignactive(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL) || doc->plane_mode) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if(doc->top_frame==1) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateStackSizeandalignmentAutomaticregularstack(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL) || doc->plane_mode) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if(doc->top_frame==1) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateRectangleUndo(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->mode&SPECIAL)) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFrameNewfrompicture(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CCombineView::OnUpdateFrameReplace(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CCombineView::OnUpdateViewRestorepicture(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CCombineView::OnUpdateClear(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateViewViewpicture(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CCombineView::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CCombineView::OnUpdateFileNew(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();
}

void CCombineView::OnUpdateFileSaverectangle(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CCombineView::OnUpdateViewToggledepthmap(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&SPECIAL)) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateViewFullmode(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CCombineView::OnUpdateFileLoaddepthinformation(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

//	orgx=orgy=0;
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateRectangleInvertfixed(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(!doc->top_frame) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateActivebulu(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	OnUpdateSpecialBrightness(pCmdUI);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateActivegreen(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	OnUpdateSpecialBrightness(pCmdUI);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdatelActivered(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	OnUpdateSpecialBrightness(pCmdUI);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateViewSelectall(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame) pCmdUI->Enable(true);
	else pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFileImport(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	OnUpdateOpen(pCmdUI);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFrameMakemonochrome(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	OnUpdateTransparent(pCmdUI);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
	if(doc->plane_mode || doc->depth_flag) pCmdUI->Enable(false);
}

/*void CCombineView::OnUpdateFrameCopyactivetodepthmap(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	OnUpdateTransparent(pCmdUI);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}
*/
void CCombineView::OnUpdateFileImportdepthmap(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	OnUpdateFileImport(pCmdUI);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
	if(doc->plane_mode) pCmdUI->Enable(false);
}


void CCombineView::OnUpdateFileExportdepthmap(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	OnUpdateFileImport(pCmdUI);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
	if(doc->plane_mode) pCmdUI->Enable(false);
}

/*void CCombineView::OnUpdateFrameDuplicate(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if(doc->plane_mode || doc->depth_flag) pCmdUI->Enable(false);
}
*/
void CCombineView::OnUpdateFrameMakebg(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
	if(doc->plane_mode || doc->depth_flag) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFrameTidyedges(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame<1) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
	if(doc->plane_mode || doc->depth_flag) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFileExportrectangles(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFrameReverseorder(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame<2) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode!=0) pCmdUI->Enable(false);
}

// Filter

void CCombineView::OnUpdateFilterCreatelowpass(CCmdUI* pCmdUI) 
{
	OnUpdateFilterCreatebandstop(pCmdUI);
}

void CCombineView::OnUpdateFilterCreatehighpass(CCmdUI* pCmdUI) 
{
	OnUpdateFilterCreatebandstop(pCmdUI);
}

void CCombineView::OnUpdateFilterCreatebandpass(CCmdUI* pCmdUI) 
{
	OnUpdateFilterCreatebandstop(pCmdUI);
}

void CCombineView::OnUpdateFilterCreatebutterworth(CCmdUI* pCmdUI) 
{
	OnUpdateFilterCreatebandstop(pCmdUI);
}

void CCombineView::OnUpdateFilterCreatebandstop(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if(doc->plane_mode) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFilterThreshold(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if(doc->dft.out_mode==OUT_THRESHOLD) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
	if(doc->plane_mode) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFilterBrightness(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if(doc->dft.out_mode==OUT_BRIGHT) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
	if(doc->plane_mode) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFilterFilteractive(CCmdUI* pCmdUI) // Normal Output
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if(doc->dft.out_mode==OUT_NORMAL) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
	if(doc->plane_mode) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFilterMakexfromdepthmap(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if(doc->plane_mode) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER && !doc->dft.filtering) pCmdUI->Enable(false);
}


void CCombineView::OnUpdateFilterTransformarithmeticMakexfromactiveframe(CCmdUI* pCmdUI) 
{
	OnUpdateFilterMakexfromdepthmap(pCmdUI);
}

void CCombineView::OnUpdateFilterTransformarithmeticMakexfromsumofstack(CCmdUI* pCmdUI) 
{
	OnUpdateFilterMakexfromdepthmap(pCmdUI);
}


void CCombineView::OnUpdateFilterInputWeightedsum(CCmdUI* pCmdUI) 
{
	OnUpdateFilterMakexfromdepthmap(pCmdUI);
}

void CCombineView::OnUpdateFilterFilterallframes(CCmdUI* pCmdUI) 
{
	OnUpdateFilterResumefiltering(pCmdUI);
}

void CCombineView::OnUpdateFilterResumefiltering(CCmdUI* pCmdUI) // filter active frame
{
	CCombineDoc* doc;
	doc=GetDocument();

	OnUpdateFilterMakexfromdepthmap(pCmdUI);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
	if(!doc->dft.valid_filter) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFilterRedisplaylastoutput(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if(doc->plane_mode) pCmdUI->Enable(false);
	if(	!doc->dft.valid_picture1 &&
		!doc->dft.valid_picture3 &&
		!(doc->dft.valid_store1 && doc->dft.valid_filter) &&
		!(doc->dft.valid_store3 && doc->dft.valid_filter))
		pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFilterFreememory(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->dft.initialized) pCmdUI->Enable(true);
	else pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateViewOutput(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CCombineView::OnUpdateFileWorkframesSavework(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CCombineView::OnUpdateFileResumework(CCmdUI* pCmdUI) 
{
/*	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
*/}

void CCombineView::OnUpdateStackDetail(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}


void CCombineView::OnUpdateStackPatchwork(CCmdUI* pCmdUI) 
{
	OnUpdateStackDetail(pCmdUI);
}

void CCombineView::OnUpdateStackDetailLightanddark(CCmdUI* pCmdUI) 
{
	OnUpdateStackDetail(pCmdUI);
}

void CCombineView::OnUpdateStackDepthflow(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateSpecialMakevisibleactiveframe2(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CCombineView::OnUpdateSpecialAdd(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->active_frame2==UNSET) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
	if(doc->plane_mode || doc->depth_flag) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateSpecialActiveframeactiveframe2(CCmdUI* pCmdUI) 
{
	OnUpdateSpecialAdd(pCmdUI);
}

void CCombineView::OnUpdateSpecialBrightness(CCmdUI* pCmdUI) // BRIGHTNESS ON MENU
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(!doc->top_frame) pCmdUI->Enable(false);
	else
	{
		if(doc->mode&SPECIAL)
		{
		switch(doc->mode)
			{
			case RED:
			case GREEN:
			case BLUE:
			case BRIGHTNESSX:
				pCmdUI->Enable(true);
				break;
			default:
				if(doc->mode==SPECIAL) pCmdUI->Enable(true);
				else pCmdUI->Enable(false);
			}
		} else {
			if(!(doc->mode&GROUP_MASK==SPECIAL)) pCmdUI->Enable(true);
		}
	}
	if(doc->plane_mode || doc->depth_flag) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateSpecialLeavespecialmode(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->mode&SPECIAL) pCmdUI->Enable(true);
	else pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateSpecialCorrectBrightness(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->active_frame2!=UNSET) && (!(doc->mode&SPECIAL) || doc->mode==SPECIAL))
		pCmdUI->Enable(true);
	else pCmdUI->Enable(false);
	if(doc->plane_mode || doc->depth_flag) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateSpecialBlend(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if((doc->active_frame2!=UNSET) && (!(doc->mode&SPECIAL) || doc->mode==SPECIAL))
		pCmdUI->Enable(true);
	else pCmdUI->Enable(false);
	if(doc->plane_mode || doc->depth_flag) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateSpecialContrast(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(!doc->top_frame)
	{
		pCmdUI->Enable(false);
		return;
	}
	pCmdUI->Enable(true);
	if(!(doc->mode&SPECIAL) || doc->mode==SPECIAL)
		pCmdUI->Enable(true);
	else pCmdUI->Enable(false);
	if(doc->plane_mode || doc->depth_flag) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateSpecialReset(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->mode&SPECIAL) pCmdUI->Enable(true);
	else pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateStackRemoveislands(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(!doc->top_frame) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateStackRemovesteps(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(!doc->top_frame) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if(doc->mode&SPECIAL) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

/*void CCombineView::OnUpdateSpecialDostack(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame<2) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}
*/
void CCombineView::OnUpdateFileWorkframesReloaddepth(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame<1) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFileWorkframesSavedepth(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame<1) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateUndoSetundopoint(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame<1) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateStackInterpolate(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame<2) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==SPECIAL) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFiltertodepth(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame<1) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==SPECIAL) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
	if(!doc->dft.valid_picture1) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateMacro1(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame<1) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==SPECIAL) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateMacro2(CCmdUI* pCmdUI) 
{
	OnUpdateMacro1(pCmdUI);
}

void CCombineView::OnUpdateMacro3(CCmdUI* pCmdUI) 
{
	OnUpdateMacro1(pCmdUI);
}

void CCombineView::OnUpdateMacro4(CCmdUI* pCmdUI) 
{
	OnUpdateMacro1(pCmdUI);
}

void CCombineView::OnUpdateMacro5(CCmdUI* pCmdUI) 
{
	OnUpdateMacro1(pCmdUI);
}

void CCombineView::OnUpdateMacro6(CCmdUI* pCmdUI) 
{
	OnUpdateMacro1(pCmdUI);
}

void CCombineView::OnUpdateMacro7(CCmdUI* pCmdUI) 
{
	OnUpdateMacro1(pCmdUI);
}

void CCombineView::OnUpdateMacro8(CCmdUI* pCmdUI) 
{
	OnUpdateMacro1(pCmdUI);
}

void CCombineView::OnUpdateMacro9(CCmdUI* pCmdUI) 
{
	OnUpdateMacro1(pCmdUI);
}

void CCombineView::OnUpdateMacro10(CCmdUI* pCmdUI) 
{
	OnUpdateMacro1(pCmdUI);
}

void CCombineView::OnUpdateMacroEnablepause(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->pause_enabled) pCmdUI->SetCheck(true);
	else pCmdUI->SetCheck(false);
}

void CCombineView::OnUpdateViewToggleframesa1a2(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	pCmdUI->Enable(true);
	if(doc->top_frame==0) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}

void CCombineView::OnUpdateFrameCopyvisibletoout(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CCombineView::OnUpdateViewGotoframe(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

void CCombineView::OnUpdateStackHorizontalskew(CCmdUI* pCmdUI) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame<2) pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
	if((doc->mode&GROUP_MASK)==SPECIAL) pCmdUI->Enable(false);
	if((doc->mode&GROUP_MASK)==FILTER) pCmdUI->Enable(false);
}
