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
// combineView.h : interface of the CCombineView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMBINEVIEW_H__D5FA2D6E_0B0D_491B_9815_C372059ED640__INCLUDED_)
#define AFX_COMBINEVIEW_H__D5FA2D6E_0B0D_491B_9815_C372059ED640__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCombineView : public CView
{
protected: // create from serialization only
	CCombineView();
	DECLARE_DYNCREATE(CCombineView)

// Attributes
public:
	CCombineDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCombineView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	void pagebr();
	void pagetl();
	void pagescroll(int horiz,int vert);
	void draw_points(CDC* pDC,int,int,int,int);
	void new_doc();
	bool first_time;
	void getpoint4();
	void getpoint3();
	void getpoint2();
	void getpoint(int i);
	void MagscrollOn();
	bool von;
	bool hon;
	int orgy;
	int orgx;
	void scroll();
	void title();
	bool lbdown;
	bool shift;
	virtual ~CCombineView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCombineView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpdateCopyunder(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReload(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReloadall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRemove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveframe(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDontuse(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUse(CCmdUI* pCmdUI);
	afx_msg void OnUpdateResizeall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveallframes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAllframes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMakefirst(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMakelast(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCalculate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFix(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransparent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUnfix(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClearpoints(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBalance(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateActivebulu(CCmdUI* pCmdUI);
	afx_msg void OnUpdateActivegreen(CCmdUI* pCmdUI);
	afx_msg void OnUpdatelActivered(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMaketransparent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFrameDecimate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewToggleplanemode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTogglefix(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFrameResizerotatemove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFrameAlignactive(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRectangleUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecialContrast(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFrameNewfrompicture(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFrameReplace(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewRestorepicture(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClear(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewViewpicture(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnViewToggleplanemode();
	afx_msg void OnUpdateFileSaverectangle(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewToggledepthmap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFullmode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileLoaddepthinformation(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecialLeavespecialmode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecialCorrectBrightness(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRectangleInvertfixed(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecialBrightness(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecialReset(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSelectall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecialBlend(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFrameMakemonochrome(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecialAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileImportdepthmap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileImport(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFrameMakebg(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileExportrectangles(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterCreatelowpass(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterCreatehighpass(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterCreatebandpass(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterCreatebutterworth(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterFilteractive(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterCreatebandstop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterThreshold(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterBrightness(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterResumefiltering(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterTransformarithmeticMakexfromactiveframe(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterFilterallframes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFrameReverseorder(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterTransformarithmeticMakexfromsumofstack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterMakexfromdepthmap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterRedisplaylastoutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFrameTidyedges(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterFreememory(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewOutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileWorkframesSavework(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileResumework(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStackDetail(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStackDepthflow(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecialMakevisibleactiveframe2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditClearunfixed(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecialActiveframeactiveframe2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStackRemoveislands(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStackRemovesteps(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileWorkframesReloaddepth(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileWorkframesSavedepth(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUndoSetundopoint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStackInterpolate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFiltertodepth(CCmdUI* pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnUpdateStackSizeandalignmentAutomaticregularstack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileExportdepthmap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacro1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacro10(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacro2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacro3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacro4(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacro5(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacro6(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacro7(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacro8(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacro9(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroEnablepause(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewToggleframesa1a2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFrameCopyvisibletoout(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewGotoframe(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStackHorizontalskew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStackPatchwork(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilterInputWeightedsum(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStackDetailLightanddark(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStackBalancecolourandbrightnessLocal(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in combineView.cpp
inline CCombineDoc* CCombineView::GetDocument()
   { return (CCombineDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBINEVIEW_H__D5FA2D6E_0B0D_491B_9815_C372059ED640__INCLUDED_)
