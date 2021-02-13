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

void CCombineView::OnLButtonUp(UINT nFlags, CPoint point) 
{
//	lbdown=false;
}

void CCombineView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->top_frame==0) return;

	CRect r,rr,savedr;
	GetClientRect(&r);

	shift=false;
	if(GetKeyState(VK_SHIFT)&0x8000) shift=true;
	if(GetKeyState(VK_CAPITAL)&0x1) shift=true;

	unsigned int x,y;

	static unsigned int x0=0,y0=0;

	double factor;
	if(doc->full_mode && doc->scale!=0) factor=1.0/(doc->scale);
	else factor=1.0;

	if(doc->magon && !doc->full_mode)
	{
//		lbdown=true;

		x=(point.x)/doc->xfactor;
		y=(point.y)/doc->yfactor;

		if(shift)
			doc->unplotdot(x,y);
		else
			doc->plotdot(x,y);

		InvalidateRect(0,false);
		return;
	}

	CPoint p;
	CRectTracker rt(r,CRectTracker::hatchInside);

	rt.TrackRubberBand(this,point);
//	lbdown=false;
	GetCursorPos(&p);
	ScreenToClient(&p);

	int temp;
	CRect temp_rect;

	if(point.x>p.x){temp=p.x; p.x=point.x; point.x=temp;}
	if(point.y>p.y){temp=p.y; p.y=point.y; point.y=temp;}
	point.x-=BORDER;
	point.y-=BORDER;
	if(point.x<0) point.x=0;
	if(point.y<0) point.y=0;
	p.x-=BORDER;
	p.y-=BORDER;

	if(doc->full_mode)
	{
		savedr=doc->mag;
		if(p.x>(int)(doc->scale*((double)(doc->w)-1)+0.5))
			p.x=(int)(doc->scale*((double)(doc->w)-1)+0.5);
		if(p.y>(int)(doc->scale*((double)(doc->h)-1)+0.5))
			p.y=(int)(doc->scale*((double)(doc->h)-1)+0.5);
	
		if(
			point.x>(int)(doc->scale*(double)((int)doc->w-1)) ||
			point.y>(int)(doc->scale*(double)((int)doc->h-1))
		) return;
		if(p.x<0 || p.y<0) return;

		temp_rect.bottom=(long)(factor*(double)p.y+0.5);
		temp_rect.left=(long)(factor*(double)point.x+0.5);
		temp_rect.right=(long)(factor*(double)p.x+0.5);
		temp_rect.top=(long)(factor*(double)point.y+0.5);
		if(temp_rect.Width()<2 || temp_rect.Height()<2) return;
		doc->mag=temp_rect;
		doc->magon=true;

		doc->Redraw(&(doc->mag),false);
		doc->Redraw(&(savedr),false);

		doc->xfactor=(r.Width()-1)/(doc->mag.Width());
		doc->yfactor=(r.Height()-1)/(doc->mag.Height());
	} else {
		if(p.x>(int)doc->w-1) p.x=(int)doc->w-1;
		if(p.y>(int)doc->h-1) p.y=(int)doc->h-1;
		
		if(point.x==p.x) return;
		if(point.y==p.y) return;

		doc->mag.bottom=p.y+orgy;
		doc->mag.left=point.x+orgx;
		doc->mag.right=p.x+orgx;
		doc->mag.top=point.y+orgy;
		doc->magon=true;

		MagscrollOn();

		doc->xfactor=(r.Width()-1)/(doc->mag.Width());
		doc->yfactor=(r.Height()-1)/(doc->mag.Height());

		if(doc->show_fixed)
		{
			doc->show_fixed=false;
			doc->backup_picture_valid=false;
			doc->redraw_picture();
		}
		InvalidateRect(0,true);
	}
}

void CCombineView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(!nFlags) return;

	CPoint p;
	CRect r;
	unsigned int x,y;

	shift=false;
	if(GetKeyState(VK_SHIFT)&0x8000) shift=true;
	if(GetKeyState(VK_CAPITAL)&0x1) shift=true;

	if((MK_LBUTTON&nFlags) && doc->magon && !doc->full_mode)
	{
		GetCursorPos(&p);
		ScreenToClient(&p);

		x=p.x/doc->xfactor;
		y=p.y/doc->yfactor;

		r.left=(x-doc->brush_size-2)*doc->xfactor;
		r.top=(y-doc->brush_size-2)*doc->yfactor;
		r.right=(x+doc->brush_size+2)*doc->xfactor;
		r.bottom=(y+doc->brush_size+2)*doc->yfactor;

		if(shift)
			doc->unplotdots(x,y);
		else
			doc->plotdots(x,y);

		if(doc->display==0 || doc->plane_mode)
			InvalidateRect(&r,false);
		return;
	}

	CView::OnMouseMove(nFlags, point);
}

void CCombineView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	GetDocument()->viewfull();
}

void CCombineView::getpoint(int i) // to set points
{
	CCombineDoc* doc;
	doc=GetDocument();

	if(doc->plane_mode) return;

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(&p);

	if(doc->magon)
	{
		if(doc->full_mode)
		{
			p.x-=BORDER;
			if(p.x>=(int)(doc->w*doc->scale)) return;
			if(p.x<0) return;
			p.x=(int)((double)(p.x)/doc->scale);
			doc->Sizer.p[i].x=p.x;

			p.y-=BORDER;
			if(p.y>=(int)(doc->h*doc->scale)) return;
			if(p.y<0) return;
			p.y=(int)((double)(p.y)/doc->scale);
			doc->Sizer.p[i].y=p.y;
		} else {
			if(p.x>doc->mag.right*(int)doc->xfactor)
				p.x=doc->mag.right*(int)doc->xfactor;
			p.x=p.x/(int)doc->xfactor+doc->mag.left;
			doc->Sizer.p[i].x=p.x;

			if(p.y>doc->mag.bottom*(int)doc->yfactor)
				p.y=doc->mag.bottom*(int)doc->yfactor;
			p.y=p.y/(int)doc->yfactor+doc->mag.top;
			doc->Sizer.p[i].y=p.y;
		}
	} else {
		CRect r;
		GetClientRect(&r);
		r.left+=BORDER;
		r.top+=BORDER;

		r.right= (hon)? r.right-BORDER : doc->w+BORDER-1;
		r.bottom= (von)? r.bottom-BORDER : doc->h+BORDER-1;

		if(p.x<r.left) p.x=r.left;
		if(p.x>r.right) p.x=r.right;
		p.x=p.x-BORDER+orgx;
		doc->Sizer.p[i].x=p.x;

		if(p.y<r.top) p.y=r.top;
		if(p.y>r.bottom) p.y=r.bottom;
		p.y=p.y-BORDER+orgy;
		doc->Sizer.p[i].y=p.y;
	}

	int* depth=(int*)doc->GetBuffer(DEPTH_BUFFER);

	CString work;
	int f;
	f=doc->active_frame;
	if(doc->display==PICTURE_BUFFER) f=depth[doc->Sizer.p[i].x+(int)(doc->w)*doc->Sizer.p[i].y];
	if(f&fEMPTY) f=doc->active_frame;
	doc->Sizer.p[i].title=buffer[f].name;
	doc->Sizer.p[i].f=f;
	doc->Sizer.p[i].valid=true;

	doc->calc();
	doc->FreeBuffer(DEPTH_BUFFER);
}

void CCombineView::getpoint2() // for seed fill and clear
{
	CCombineDoc* doc;
	doc=GetDocument();

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(&p);

	if(doc->magon && !doc->full_mode)
	{
		if(p.x>doc->mag.right*(int)doc->xfactor)
			p.x=doc->mag.right*(int)doc->xfactor;
		p.x=p.x/(int)doc->xfactor+doc->mag.left;

		if(p.y>doc->mag.bottom*(int)doc->yfactor)
			p.y=doc->mag.bottom*(int)doc->yfactor;
		p.y=p.y/(int)doc->yfactor+doc->mag.top;
	} else {
		CRect r;
		GetClientRect(&r);
		r.left+=BORDER;
		r.top+=BORDER;

		if(!doc->full_mode)
		{
			r.right= (hon)? r.right-BORDER : doc->w+BORDER-1;
			r.bottom= (von)? r.bottom-BORDER : doc->h+BORDER-1;

			if(p.x-BORDER+orgx>=(int)doc->w) return;
			if(p.x<r.left) p.x=r.left;
			if(p.x>r.right) p.x=r.right;
			p.x=p.x-BORDER+orgx;

			if(p.y-BORDER+orgy>=(int)doc->h) return;
			if(p.y<r.top) p.y=r.top;
			if(p.y>r.bottom) p.y=r.bottom;
			p.y=p.y-BORDER+orgy;
		} else {
			r.right-=BORDER+1;
			r.bottom-=BORDER+1;

			p.x-=BORDER;
			if(p.x>=r.Width()) return;
			if(p.x<0) return;
			p.x=(int)((double)(p.x)/doc->scale);

			p.y-=BORDER;
			if(p.y>=r.Height()) return;
			if(p.y<0) return;
			p.y=(int)((double)(p.y)/doc->scale);
		}
	}
	if(shift) doc->clearbg(p.x,p.y);
	else doc->seedfill(p.x,p.y);
}

void CCombineView::getpoint3() // to change planes
{
	CCombineDoc* doc;
	doc=GetDocument();

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(&p);

	if(doc->magon && !doc->full_mode)
	{
		p.x=p.x/(int)doc->xfactor+doc->mag.left;

		p.y=p.y/(int)doc->yfactor+doc->mag.top;
	} else {
		CRect r;
		GetClientRect(&r);
		r.left+=BORDER;
		r.top+=BORDER;

		if(!doc->full_mode)
		{
			r.right= (hon)? r.right-BORDER : doc->w+BORDER-1;
			r.bottom= (von)? r.bottom-BORDER : doc->h+BORDER-1;
		} else {
			r.right-=BORDER+1;
			r.bottom-=BORDER+1;
		}

		if(p.x<r.left) p.x=-1;
		if(p.x>r.right) p.x=doc->w+1+BORDER;
		p.x=p.x-BORDER;
		if(p.y<r.top) p.y=-1;
		if(p.y>r.bottom) p.y=doc->h+1+BORDER;
		p.y=p.y-BORDER;
		if(!doc->full_mode)
		{
			p.y+=orgy;
			p.x+=orgx;
		}
	}
	if(doc->full_mode)	doc->toggle_plane((int)((double)(p.x)/doc->scale),(int)((double)(p.y)/doc->scale));
	else doc->toggle_plane(p.x,p.y);
}

void CCombineView::getpoint4() // to goto frame under mouse pointer
{
	CCombineDoc* doc;
	doc=GetDocument();

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(&p);

	if(doc->magon && !doc->full_mode)
	{
		p.x=p.x/(int)doc->xfactor+doc->mag.left;

		p.y=p.y/(int)doc->yfactor+doc->mag.top;
	} else {
		CRect r;
		GetClientRect(&r);
		r.left+=BORDER;
		r.top+=BORDER;

		if(!doc->full_mode)
		{
			r.right= (hon)? r.right-BORDER : doc->w+BORDER-1;
			r.bottom= (von)? r.bottom-BORDER : doc->h+BORDER-1;
		} else {
			r.right-=BORDER+1;
			r.bottom-=BORDER+1;
		}

		if(p.x<r.left) p.x=-1;
		if(p.x>r.right) p.x=doc->w+1+BORDER;
		p.x=p.x-BORDER;
		if(p.y<r.top) p.y=-1;
		if(p.y>r.bottom) p.y=doc->h+1+BORDER;
		p.y=p.y-BORDER;
		if(!doc->full_mode)
		{
			p.y+=orgy;
			p.x+=orgx;
		}
	}
	int d;
	int *depth=(int*)doc->GetBuffer(DEPTH_BUFFER);
	if(doc->full_mode)
		d=(int)((double)(p.x)/doc->scale)+doc->w*(int)((double)(p.y)/doc->scale);
	else d=p.x+doc->w*p.y;
	doc->active_frame=doc->display=(((depth[d]&dMASK)+(1<<7))>>8);
	doc->FreeBuffer(DEPTH_BUFFER);
	if(doc->plane_mode)
	{
		doc->draw_plane();
		doc->display=PICTURE_BUFFER;
	}
}

BOOL CCombineView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CCombineDoc* pDoc = GetDocument();

	POINT pm,pw;
	GetCursorPos(&pm);
	RECT r;
	GetClientRect(&r);
	pw.x=r.right; pw.y=r.bottom;
	ClientToScreen(&pw);
	if((pm.x>pw.x-BORDER) || (pm.y>pw.y-BORDER))
		return CView::OnSetCursor(pWnd, nHitTest, message);

	if(pDoc->magon && !pDoc->full_mode)
	{
		HCURSOR cur;
		if(pDoc->display==OUT_BUFFER)
			cur=AfxGetApp()->LoadCursor(IDC_CURSOR1);
		if(pDoc->display==PICTURE_BUFFER)
			if(pDoc->depth_flag) cur=AfxGetApp()->LoadCursor(IDC_CURSOR5);
			else cur=AfxGetApp()->LoadCursor(IDC_CURSOR2);
		if(pDoc->display==pDoc->active_frame)
			cur=AfxGetApp()->LoadCursor(IDC_CURSOR3);
		if(pDoc->display==pDoc->active_frame2)
			cur=AfxGetApp()->LoadCursor(IDC_CURSOR4);
		SetCursor(cur);
		return true;
	}

	return CView::OnSetCursor(pWnd, nHitTest, message);
}


BOOL CCombineView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CCombineDoc* doc=GetDocument();

	static int delta;
	delta+=zDelta;

	int d;
	d=delta/120;
	delta-=120*d;

	if(d==0) return true;

	RECT r;
	GetClientRect(&r);
	ClientToScreen(&r);
	if(pt.x<r.left) goto end;
	if(pt.x>r.right) goto end;
	if(pt.y<r.top) goto end;
	if(pt.y>r.bottom) goto end;

	int w,h;

	if(!doc->full_mode)
	{
		if(doc->magon)
		{
			w=doc->mag.Width()+1;
			h=doc->mag.Height()+1;

			GetClientRect(&r);
			r.right=r.left+BORDER*2+w*doc->xfactor;
			r.bottom=r.top+BORDER*2+h*doc->yfactor;

			ScreenToClient(&pt);

			if((pt.x>r.right-(r.right-r.left)/4) && (doc->mag.Width()-doc->w))
			{
				doc->mag.left-=d;
				if(doc->mag.left<0) doc->mag.left=0;
				if(doc->mag.left+w>doc->w-1) doc->mag.left=doc->w-w;
				doc->mag.right=doc->mag.left+w-1;
				SetScrollPos(SB_HORZ,doc->mag.left,true);
				InvalidateRect(0,false);
			}
			if((pt.x<r.left+(r.right-r.left)/4) && (doc->mag.Width()-doc->w))
			{
				doc->mag.left+=d;
				if(doc->mag.left<0) doc->mag.left=0;
				if(doc->mag.left+w>doc->w-1) doc->mag.left=doc->w-w;
				doc->mag.right=doc->mag.left+w-1;
				SetScrollPos(SB_HORZ,doc->mag.left,true);
				InvalidateRect(0,false);
			}
			if((pt.y>r.bottom-(r.bottom-r.top)/4) && (doc->mag.Height()-doc->h))
			{
				doc->mag.top-=d;
				if(doc->mag.top<0) doc->mag.top=0;
				if(doc->mag.top+h>doc->h-1) doc->mag.top=doc->h-h;
				doc->mag.bottom=doc->mag.top+h-1;
				SetScrollPos(SB_VERT,doc->mag.top,true);
				InvalidateRect(0,false);
			}
			if((pt.y<r.top+(r.bottom-r.top)/4) && (doc->mag.Height()-doc->h))
			{
				doc->mag.top+=d;
				if(doc->mag.top<0) doc->mag.top=0;
				if(doc->mag.top+h>doc->h-1) doc->mag.top=doc->h-h;
				doc->mag.bottom=doc->mag.top+h-1;
				SetScrollPos(SB_VERT,doc->mag.top,true);
				InvalidateRect(0,false);
			}
			return true;
		} else {
			GetClientRect(&r);
			r.left+=BORDER;
			r.right-=BORDER;
			r.top+=BORDER;
			r.bottom-=BORDER;

			w=r.right-r.left;
			h=r.bottom-r.top;

			ScreenToClient(&pt);

			doc->xorg=orgx;
			doc->yorg=orgy;

			if((pt.x>r.left+(3*w)/4) && (w<doc->w))
			{
				doc->xorg-=(2*d+(d*w)/25);
				if(doc->xorg<0) doc->xorg=0;
				if(doc->xorg+w>doc->w-1) doc->xorg=doc->w-w;
				SetScrollPos(SB_HORZ,doc->xorg,true);
				orgx=doc->xorg;
				InvalidateRect(0,false);
			}
			if((pt.x<r.left+w/4) && (w<doc->w))
			{
				doc->xorg+=(2*d+(d*w)/25);
				if(doc->xorg<0) doc->xorg=0;
				if(doc->xorg+w>doc->w-1) doc->xorg=doc->w-w;
				SetScrollPos(SB_HORZ,doc->xorg,true);
				orgx=doc->xorg;
				InvalidateRect(0,false);
			}
			if((pt.y>r.top+(3*h)/4) && (h<doc->h))
			{
				doc->yorg-=(2*d+(d*h)/25);
				if(doc->yorg<0) doc->yorg=0;
				if(doc->yorg+h>doc->h-1) doc->yorg=doc->h-h;
				SetScrollPos(SB_VERT,doc->yorg,true);
				orgy=doc->yorg;
				InvalidateRect(0,false);
			}
			if((pt.y<r.top+h/4) && (h<doc->h))
			{
				doc->yorg+=(2*d+(d*h)/25);
				if(doc->yorg<0) doc->yorg=0;
				if(doc->yorg+h>doc->h-1) doc->yorg=doc->h-h;
				SetScrollPos(SB_VERT,doc->yorg,true);
				orgy=doc->yorg;
				InvalidateRect(0,false);
			}
		}
		return true;
	}
end:
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}
