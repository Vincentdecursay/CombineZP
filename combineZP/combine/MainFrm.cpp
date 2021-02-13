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
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "gdiplus.h"

#include "combine.h"

#include "MainFrm.h"
#include ".\mainfrm.h"
#include "combineDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_VIEW_SIMPLEMODE, OnViewSimplemode)
	ON_WM_PARENTNOTIFY()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	using namespace Gdiplus;
	GdiplusStartupInput gdipsi;
	GdiplusStartup(&GdipToken,&gdipsi,0);
	simple_mode=(AfxGetApp()->GetProfileInt(L"Simple",L"Startup",true)!=0);
}

CMainFrame::~CMainFrame()
{
	using namespace Gdiplus;
	GdiplusShutdown(GdipToken);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	cm_menu.Attach(GetMenu()->m_hMenu);
	m_menu=&cm_menu;
	SetMenu(m_menu);

	CRect rect(5,5,5,5);
	m_simple.CreateEx(this,TBSTYLE_FLAT,
		WS_CHILD|WS_VISIBLE|CBRS_TOOLTIPS,
		rect);
	m_simple.LoadToolBar(IDR_SIMPLE);

	m_simple.SetButtonInfo(1,IDC_SIMPLE_COMBO,TBBS_SEPARATOR,250);
	m_simple.GetItemRect(1,&rect);
	rect.top+=8;
	rect.bottom=rect.top+250;
	Simple_Combo.Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST,
		rect,&m_simple,IDC_SIMPLE_COMBO);

	CPoint pt(0,0);
	m_simple.EnableDocking(0);
	EnableDocking(0);
	FloatControlBar(&m_simple,pt);

    CWnd *pWnd=m_simple.GetParent();
	if(pWnd!=(CWnd *)NULL)
    {
		ASSERT_KINDOF(CWnd,pWnd);
		pWnd=pWnd->GetParent();
    }
    if(pWnd!=(CWnd *)NULL)
    {
		ASSERT_KINDOF(CWnd,pWnd);
		if(pWnd->GetSafeHwnd()!=AfxGetMainWnd()->GetSafeHwnd())
		{
			pWnd->ModifyStyle(WS_SYSMENU,0);
		}
	}

	CString s; s.LoadString(AFX_IDS_APP_TITLE);
	ToolBarText(s);

	if(!simple_mode)
		ShowControlBar(&m_simple,false,false);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style=WS_OVERLAPPEDWINDOW;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnClose() 
{
	CString s1,s2;
	s1.LoadString(IDS_QUITMESSAGE); s2.LoadString(AFX_IDS_APP_TITLE);
	int ret=MessageBox(s1,s2,MB_YESNO);
	if(ret==IDYES) return;
	CFrameWnd::OnClose();
}

void CMainFrame::OnViewSimplemode()
{
	if(!simple_mode)
	{
		SetMenu(0);
		simple_mode=true;
		ShowControlBar(&m_simple,true,false);
	} else {
		ShowControlBar(&m_simple,false,false);
		simple_mode=false;
		SetMenu(m_menu);
		DrawMenuBar();
	}
}

void  CMainFrame::ToolBarText(CString s)
{
	CWnd *pWnd=m_simple.GetParent();
	pWnd->SetWindowText(s);
	ShowControlBar(&m_simple,true,false);
}

void  CMainFrame::AddComboString(CString s,int MenuID,bool def)
{
	int i;

	if(MenuID==-1)
	{
		Simple_Combo.ResetContent();
		return;
	}

	if(s.Find('\t')!=-1) s=s.Left(s.Find('\t'));

	if(s[0]=='&') s=L"*** "+s.Right(s.GetLength()-1);

	i=Simple_Combo.AddString(s);

	Simple_Combo.SetItemData(i,MenuID);

	if(def) Simple_Combo.SetCurSel(i);
}

int  CMainFrame::GetComboCursel()
{
	return Simple_Combo.GetItemData(Simple_Combo.GetCurSel());
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if(simple_mode)
	{
		if(bShow)
			ShowControlBar(&m_simple,true,false);
		else
			ShowControlBar(&m_simple,false,false);
	}

	CFrameWnd::OnShowWindow(bShow, nStatus);
}
