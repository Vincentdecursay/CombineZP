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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CCombineView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//CString s;
//s.Format("%d",nChar);
//MessageBox(s,"Key Code",MB_OK);
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0)
	{
		if(nChar==VK_F2) doc->progress.Maximize();
		return;
	}

	if(doc->buissy) return;

	if(nFlags&1<<14) return;

	shift=false;
	if(GetKeyState(VK_SHIFT)&0x8000) shift=true;
	if(GetKeyState(VK_CAPITAL)&0x1) shift=true;

	bool control=false;
	if(GetKeyState(VK_CONTROL)&0x8000) control=true;

	if(((doc->mode&GROUP_MASK)==FILTER) && (nChar!=VK_ESCAPE))
	{
		doc->alter_filter(nChar);
		return;
	}

	if(control)
	{
		switch(nChar)
		{
		case VK_NUMPAD1: case '1':
			doc->gotopoint(0);
			return;
		case VK_NUMPAD2: case '2':
			doc->gotopoint(1);
			return;
		case VK_NUMPAD3: case '3':
			doc->gotopoint(2);
			return;
		case VK_NUMPAD4: case '4':
			doc->gotopoint(3);
			return;
		case VK_NEXT:
			pagescroll(1,0);
			return;
		case VK_PRIOR:
			pagescroll(-1,0);
			return;
		case VK_UP:
			doc->display=doc->active_frame=doc->top_frame-1;
			if(doc->plane_mode) doc->draw_plane();
			break;
		case VK_DOWN:
			doc->display=doc->active_frame=0;
			if(doc->plane_mode) doc->draw_plane();
			break;
		}
		goto finish;
	}    

	if(shift)
	{
		switch(nChar)
		{
		case VK_ESCAPE:
			BeginWaitCursor();
			if(doc->plane_mode)	getpoint3();
			doc->mode=0;
			doc->depth_flag=false;
			doc->dft.mode=0;
			doc->dft.filtering=false;
			doc->backup_picture_valid=false;
			doc->redraw_picture();
			doc->mag.left=doc->mag.top=0;
			doc->mag.right=doc->w-1;
			doc->mag.bottom=doc->h-1;
			doc->full_mode=true;
			InvalidateRect(0,false);
			EndWaitCursor();
			return;
		case VK_UP:
			doc->up();
			goto finish;
		case VK_DOWN:
			doc->down();
			goto finish;
		case VK_LEFT:
			doc->left();
			goto finish;
		case VK_RIGHT:
			doc->right();
			goto finish;
		case 'A':
			doc->selectall();
			goto finish;
		case 'C':
			getpoint2();
			goto finish;
		case 'X':
			doc->xout();
			return;
		case 'Y':
			doc->yout();
			return;
		case 'Z':
			doc->zoomout();
			return;
		}
	}

	switch(nChar)
	{
	case VK_DECIMAL: case 190:
		doc->active_frame2=doc->active_frame;
		break;
	case VK_ADD: case 187:
		if(doc->active_frame2==-1) return;
		if(doc->display==doc->active_frame) doc->display=doc->active_frame2;
		else doc->display=doc->active_frame;
		break;
	case VK_NEXT:
		if(doc->magon){doc->page_up(); scroll();}
		else pagescroll(0,1);
		return;
	case VK_PRIOR:
		if(doc->magon){doc->page_down(); scroll();}
		else pagescroll(0,-1);
		return;
	case VK_ESCAPE:
		BeginWaitCursor();
		if(doc->plane_mode)	getpoint3();
		doc->mode=0;
		doc->depth_flag=false;
		doc->dft.mode=0;
		doc->dft.filtering=false;
		doc->backup_picture_valid=false;
		doc->redraw_picture();
		doc->mag.left=doc->mag.top=0;
		doc->mag.right=doc->w-1;
		doc->mag.bottom=doc->h-1;
		doc->full_mode=true;
		InvalidateRect(0,false);
		EndWaitCursor();
		return;
	case VK_NUMPAD1: case '1':
		getpoint(0); 
		break;
	case VK_NUMPAD2: case '2':
		getpoint(1);
		break;
	case VK_NUMPAD3: case '3':
		getpoint(2);
		break;
	case VK_NUMPAD4: case '4':
		getpoint(3);
		break;
	case VK_NUMPAD0: case '0':
		doc->calc();
		break;
	case VK_SPACE:
		doc->flash();
		break;
	case 'V':
		doc->GetValue();
		return;
	case VK_RIGHT:
		if(doc->mode==SIZE)
		{
			BeginWaitCursor();
			doc->size_factor+=10;
			if(doc->Reload())
				doc->Reload();
			doc->sizecompensation();
			EndWaitCursor();
			break;
		}
		if(doc->mode==RED)
		{
			BeginWaitCursor();
			if(doc->rfactor<1.0)
			{
				doc->rfactor+=1.0/256.0;
			} else {
				if(doc->gfactor>0.0) doc->gfactor-=1.0/256.0;
				if(doc->bfactor>0.0) doc->bfactor-=1.0/256.0;
			}
			if(doc->rfactor>1.0) doc->rfactor=1.0;
			if(doc->rfactor<0.0) doc->rfactor=0.0;
			if(doc->gfactor>1.0) doc->gfactor=1.0;
			if(doc->gfactor<0.0) doc->gfactor=0.0;
			if(doc->bfactor>1.0) doc->bfactor=1.0;
			if(doc->bfactor<0.0) doc->bfactor=0.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==GREEN)
		{
			BeginWaitCursor();
			if(doc->gfactor<1.0)
			{
				doc->gfactor+=1.0/256.0;
			} else {
				if(doc->rfactor>0.0) doc->rfactor-=1.0/256.0;
				if(doc->bfactor>0.0) doc->bfactor-=1.0/256.0;
			}
			if(doc->rfactor>1.0) doc->rfactor=1.0;
			if(doc->rfactor<0.0) doc->rfactor=0.0;
			if(doc->gfactor>1.0) doc->gfactor=1.0;
			if(doc->gfactor<0.0) doc->gfactor=0.0;
			if(doc->bfactor>1.0) doc->bfactor=1.0;
			if(doc->bfactor<0.0) doc->bfactor=0.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==BLUE)
		{
			BeginWaitCursor();
			if(doc->bfactor<1.0)
			{
				doc->bfactor+=1.0/256.0;
			} else {
				if(doc->gfactor>0.0) doc->gfactor-=1.0/256.0;
				if(doc->rfactor>0.0) doc->rfactor-=1.0/256.0;
			}
			if(doc->rfactor>1.0) doc->rfactor=1.0;
			if(doc->rfactor<0.0) doc->rfactor=0.0;
			if(doc->gfactor>1.0) doc->gfactor=1.0;
			if(doc->gfactor<0.0) doc->gfactor=0.0;
			if(doc->bfactor>1.0) doc->bfactor=1.0;
			if(doc->bfactor<0.0) doc->bfactor=0.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==BRIGHTNESSX)
		{
			BeginWaitCursor();
			doc->rfactor+=1.0/256.0;
			doc->gfactor+=1.0/256.0;
			doc->bfactor+=1.0/256.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==CONTRASTX)
		{
			BeginWaitCursor();
			doc->contrast_delta++;
			doc->Contrast();
			EndWaitCursor();
			break;
		}
		if(doc->mode==CORRECT)
		{
			BeginWaitCursor();
			doc->correction_factor+=0.01;
			doc->correct(0.0,0.0);
			EndWaitCursor();
			break;
		}
		if(doc->mode==BLEND)
		{
			BeginWaitCursor();
			doc->blend_factor-=0.01;
			doc->blend();
			EndWaitCursor();
			break;
		}
		if(doc->mode==ADD)
		{
			BeginWaitCursor();
			doc->add_factor-=0.01;
			doc->add();
			EndWaitCursor();
			break;
		}
//		MessageBeep(MB_ICONERROR);
		break;
	case VK_LEFT:
		if(doc->mode==SIZE)
		{
			BeginWaitCursor();
			doc->size_factor-=10;
			if(doc->Reload())
				doc->sizecompensation();
			EndWaitCursor();
			break;
		}
		if(doc->mode==RED)
		{
			BeginWaitCursor();
			if(doc->gfactor<1.0 && doc->bfactor<1.0)
			{
				doc->gfactor+=1.0/256.0;
				doc->bfactor+=1.0/256.0;
			} else {
				if(doc->rfactor>0.0) doc->rfactor-=1.0/256.0;
			}
			if(doc->rfactor>1.0) doc->rfactor=1.0;
			if(doc->rfactor<0.0) doc->rfactor=0.0;
			if(doc->gfactor>1.0) doc->gfactor=1.0;
			if(doc->gfactor<0.0) doc->gfactor=0.0;
			if(doc->bfactor>1.0) doc->bfactor=1.0;
			if(doc->bfactor<0.0) doc->bfactor=0.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==GREEN)
		{
			BeginWaitCursor();
			if(doc->rfactor<1.0 && doc->bfactor<1.0)
			{
				doc->rfactor+=1.0/256.0;
				doc->bfactor+=1.0/256.0;
			} else {
				if(doc->gfactor>0.0) doc->gfactor-=1.0/256.0;
			}
			if(doc->rfactor>1.0) doc->rfactor=1.0;
			if(doc->rfactor<0.0) doc->rfactor=0.0;
			if(doc->gfactor>1.0) doc->gfactor=1.0;
			if(doc->gfactor<0.0) doc->gfactor=0.0;
			if(doc->bfactor>1.0) doc->bfactor=1.0;
			if(doc->bfactor<0.0) doc->bfactor=0.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==BLUE)
		{
			BeginWaitCursor();
			if(doc->rfactor<1.0 && doc->gfactor<1.0)
			{
				doc->rfactor+=1.0/256.0;
				doc->gfactor+=1.0/256.0;
			} else {
				if(doc->bfactor>0.0) doc->bfactor-=1.0/256.0;
			}
			if(doc->rfactor>1.0) doc->rfactor=1.0;
			if(doc->rfactor<0.0) doc->rfactor=0.0;
			if(doc->gfactor>1.0) doc->gfactor=1.0;
			if(doc->gfactor<0.0) doc->gfactor=0.0;
			if(doc->bfactor>1.0) doc->bfactor=1.0;
			if(doc->bfactor<0.0) doc->bfactor=0.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==BRIGHTNESSX)
		{
			BeginWaitCursor();
			doc->rfactor-=1.0/256.0;
			doc->gfactor-=1.0/256.0;
			doc->bfactor-=1.0/256.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==CONTRASTX)
		{
			BeginWaitCursor();
			doc->contrast_delta--;
			doc->Contrast();
			EndWaitCursor();
			break;
		}
		if(doc->mode==CORRECT)
		{
			BeginWaitCursor();
			doc->correction_factor-=0.01;
			doc->correct(0.0,0.0);
			EndWaitCursor();
			break;
		}
		if(doc->mode==BLEND)
		{
			BeginWaitCursor();
			doc->blend_factor+=0.01;
			doc->blend();
			EndWaitCursor();
			break;
		}
		if(doc->mode==ADD)
		{
			BeginWaitCursor();
			doc->add_factor+=0.01;
			doc->add();
			EndWaitCursor();
			break;
		}
//		MessageBeep(MB_ICONERROR);
		break;
	case 'M':
		if(doc->mode==SIZE)
		{
			BeginWaitCursor();
			doc->size_factor+=100;
			if(doc->Reload())
				doc->sizecompensation();
			EndWaitCursor();
			break;
		}
		if(doc->mode==RED)
		{
			BeginWaitCursor();
			if(doc->rfactor<1.0-1.0/16.0)
			{
				doc->rfactor+=1.0/16.0;
			} else {
				if(doc->gfactor>=1.0/16.0) doc->gfactor-=1.0/16.0;
				if(doc->bfactor>=1.0/16.0) doc->bfactor-=1.0/16.0;
			}
			doc->adjustcolour();
			if(doc->rfactor>1.0) doc->rfactor=1.0;
			if(doc->rfactor<0.0) doc->rfactor=0.0;
			if(doc->gfactor>1.0) doc->gfactor=1.0;
			if(doc->gfactor<0.0) doc->gfactor=0.0;
			if(doc->bfactor>1.0) doc->bfactor=1.0;
			if(doc->bfactor<0.0) doc->bfactor=0.0;
			EndWaitCursor();
			break;
		}
		if(doc->mode==GREEN)
		{
			BeginWaitCursor();
			if(doc->gfactor<1.0-1.0/16.0)
			{
				doc->gfactor+=1.0/16.0;
			} else {
				if(doc->rfactor>=1.0/16.0) doc->rfactor-=1.0/16.0;
				if(doc->bfactor>=1.0/16.0) doc->bfactor-=1.0/16.0;
			}
			if(doc->rfactor>1.0) doc->rfactor=1.0;
			if(doc->rfactor<0.0) doc->rfactor=0.0;
			if(doc->gfactor>1.0) doc->gfactor=1.0;
			if(doc->gfactor<0.0) doc->gfactor=0.0;
			if(doc->bfactor>1.0) doc->bfactor=1.0;
			if(doc->bfactor<0.0) doc->bfactor=0.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==BLUE)
		{
			BeginWaitCursor();
			if(doc->bfactor<1.0-1.0/16.0)
			{
				doc->bfactor+=1.0/16.0;
			} else {
				if(doc->rfactor>=1.0/16.0) doc->rfactor-=1.0/16.0;
				if(doc->gfactor>=1.0/16.0) doc->gfactor-=1.0/16.0;
			}
			if(doc->rfactor>1.0) doc->rfactor=1.0;
			if(doc->rfactor<0.0) doc->rfactor=0.0;
			if(doc->gfactor>1.0) doc->gfactor=1.0;
			if(doc->gfactor<0.0) doc->gfactor=0.0;
			if(doc->bfactor>1.0) doc->bfactor=1.0;
			if(doc->bfactor<0.0) doc->bfactor=0.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==BRIGHTNESSX)
		{
			BeginWaitCursor();
			doc->rfactor+=1.0/16.0;
			doc->gfactor+=1.0/16.0;
			doc->bfactor+=1.0/16.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==CONTRASTX)
		{
			BeginWaitCursor();
			doc->contrast_delta+=5;
			doc->Contrast();
			EndWaitCursor();
			break;
		}
		if(doc->mode==CORRECT)
		{
			BeginWaitCursor();
			doc->correction_factor+=0.1;
			doc->correct(0.0,0.0);
			EndWaitCursor();
			break;
		}
		if(doc->mode==BLEND)
		{
			BeginWaitCursor();
			doc->blend_factor-=0.1;
			doc->blend();
			EndWaitCursor();
			break;
		}
		if(doc->mode==ADD)
		{
			BeginWaitCursor();
			doc->add_factor-=0.1;
			doc->add();
			EndWaitCursor();
			break;
		}
		MessageBeep(MB_ICONERROR);
		break;
	case 'N':
		if(doc->mode==SIZE)
		{
			BeginWaitCursor();
			if(doc->size_factor<100) doc->size_factor=0;
			else doc->size_factor-=100;
			if(doc->Reload())
				doc->sizecompensation();
			EndWaitCursor();
			break;
		}
		if(doc->mode==RED)
		{
			BeginWaitCursor();
			if(doc->gfactor<=1.0-1.0/16.0 && doc->bfactor<=1.0-1.0/16.0)
			{
				doc->gfactor+=1.0/16.0;
				doc->bfactor+=1.0/16.0;
			} else {
				if(doc->rfactor>0.0+1.0/16.0) doc->rfactor-=1.0/16.0;
			}
			if(doc->rfactor>1.0) doc->rfactor=1.0;
			if(doc->rfactor<0.0) doc->rfactor=0.0;
			if(doc->gfactor>1.0) doc->gfactor=1.0;
			if(doc->gfactor<0.0) doc->gfactor=0.0;
			if(doc->bfactor>1.0) doc->bfactor=1.0;
			if(doc->bfactor<0.0) doc->bfactor=0.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==GREEN)
		{
			BeginWaitCursor();
			if(doc->rfactor<=1.0-1.0/16.0 && doc->bfactor<=1.0-1.0/16.0)
			{
				doc->rfactor+=1.0/16.0;
				doc->bfactor+=1.0/16.0;
			} else {
				if(doc->gfactor>0.0+1.0/16.0) doc->gfactor=doc->gfactor-=1.0/16.0;
			}
			if(doc->rfactor>1.0) doc->rfactor=1.0;
			if(doc->rfactor<0.0) doc->rfactor=0.0;
			if(doc->gfactor>1.0) doc->gfactor=1.0;
			if(doc->gfactor<0.0) doc->gfactor=0.0;
			if(doc->bfactor>1.0) doc->bfactor=1.0;
			if(doc->bfactor<0.0) doc->bfactor=0.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==BLUE)
		{
			BeginWaitCursor();
			if(doc->rfactor<=1.0-1.0/16.0 && doc->gfactor<=1.0-1.0/16.0)
			{
				doc->rfactor+=1.0/16.0;
				doc->gfactor+=1.0/16.0;
			} else {
				if(doc->bfactor>0.0+1.0/16.0) doc->bfactor-=1.0/16.0;
			}
			if(doc->rfactor>1.0) doc->rfactor=1.0;
			if(doc->rfactor<0.0) doc->rfactor=0.0;
			if(doc->gfactor>1.0) doc->gfactor=1.0;
			if(doc->gfactor<0.0) doc->gfactor=0.0;
			if(doc->bfactor>1.0) doc->bfactor=1.0;
			if(doc->bfactor<0.0) doc->bfactor=0.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==BRIGHTNESSX)
		{
			BeginWaitCursor();
			if(doc->rfactor>=1.0/16.0) doc->rfactor-=1.0/16.0;
			if(doc->gfactor>=1.0/16.0) doc->gfactor-=1.0/16.0;
			if(doc->bfactor>=1.0/16.0) doc->bfactor-=1.0/16.0;
			doc->adjustcolour();
			EndWaitCursor();
			break;
		}
		if(doc->mode==CONTRASTX)
		{
			BeginWaitCursor();
			doc->contrast_delta-=5;
			doc->Contrast();
			EndWaitCursor();
			break;
		}
		if(doc->mode==CORRECT)
		{
			BeginWaitCursor();
			doc->correction_factor-=0.1;
			doc->correct(0.0,0.0);
			EndWaitCursor();
			break;
		}
		if(doc->mode==BLEND)
		{
			BeginWaitCursor();
			doc->blend_factor+=0.1;
			doc->blend();
			EndWaitCursor();
			break;
		}
		if(doc->mode==ADD)
		{
			BeginWaitCursor();
			doc->add_factor+=0.1;
			doc->add();
			EndWaitCursor();
			break;
		}
		MessageBeep(MB_ICONERROR);
		break;
	case VK_UP:
/*		if(doc->top_frame==1)
		{
			doc->next_file();
			break;
		}
*/		if(doc->plane_mode) doc->plane_up();
		else doc->active_up();
		break;
	case VK_DOWN:
/*		if(doc->top_frame==1)
		{
			doc->prev_file();
			break;
		}
*/		if(doc->plane_mode) doc->plane_down();
		else doc->active_down();
		break;
	case VK_TAB:
		BeginWaitCursor();
		doc->home();
		EndWaitCursor();
		break;
	case 'O':
		if(doc->display!=OUT_BUFFER) doc->display=OUT_BUFFER;
		else doc->display=doc->active_frame;
		break;
	case VK_RETURN:
		BeginWaitCursor();
		if(doc->mode==SIZE)
		{
			doc->Size();
			EndWaitCursor();
			break;
		}
		if(doc->mode&SPECIAL)
		{
			doc->specialreturn();
			EndWaitCursor();
			break;
		}
		doc->mode=0;
		EndWaitCursor();
		break;
	case 'X':
		doc->xin();
		return;
	case 'Y':
		doc->yin();
		return;
	case 'Z':
		doc->zoomin();
		return;
	case 'A':
		doc->selectall();
		return;;
	case 'P':
		if(doc->mode!=SPECIAL) getpoint3();
		break;
	case 'G':
		if(doc->mode!=SPECIAL) getpoint4();
		break;
	case 'F':
		if(doc->mode!=SPECIAL) doc->flash_fixed();
		break;
	case 'C':
		getpoint2();
		break;
	case 'D':
		doc->depthmap();
		break;
	case 'B':
		doc->brushparams();
		break;
	case VK_DELETE:
		doc->Remove();
		doc->clearunfixed();
		break;
	case VK_BACK:
		if(doc->display==PICTURE_BUFFER) doc->undo();
		else doc->Reload();
		break;
	case VK_HOME:
		pagetl();
		return;
	case VK_END:
		pagebr();
		return;
	case VK_F2:
		doc->progress.Maximize();
		return;
	case VK_F3:
		doc->gotoframe();
		return;
	default:
		return;
	}
finish:
	InvalidateRect(0,false);
	return;
}

void CCombineView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	return;
	switch(nChar)
	{
	case VK_SHIFT:
		shift=false;
		return;
	}
}

void CCombineDoc::gotopoint(int pt)
{
	if(plane_mode) return;
	if(!Sizer.p[pt].valid) return;
	active_frame=display=Sizer.p[pt].f;
	full_mode=true;
	RefreshScreen();
}

