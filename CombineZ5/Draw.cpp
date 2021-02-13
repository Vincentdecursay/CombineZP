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

struct BUFFER buffer[];

void CCombineView::title()
{
	CString ttl,work;
	static CString old="";
	int f;

	CCombineDoc* doc;
	doc=GetDocument();

	f=doc->display;

	if((doc->mode&GROUP_MASK)==FILTER)
	{
		CString zero_value; zero_value.Format("%1.3f",doc->dft.zero_value); 
		if(doc->mode==FBRIGHTNESS)
		{
			ttl.Format("Brighten Factor %d V=",(int)(doc->dft.brightness*100.0f+0.5f));
			ttl+=buffer[doc->display].name;
			goto finish;
		}
		if(doc->mode==FTHRESHOLD)
		{
			ttl.Format("Threshold Factor %d V=",(int)(doc->dft.threshold*100.0f+0.5f));
			ttl+=buffer[doc->display].name;
			goto finish;
		}
		switch(doc->dft.mode)
		{
		case LOWPASS:
			ttl+="LowpassFilter";
			work.Format("(%d,%d) ",(int)(doc->dft.width*1000.0f+0.5f),(int)(doc->dft.displacement*1000.0f+0.5f));
			ttl+=work;
			if(doc->filter_param==WIDTH) ttl+="1 Decay Width";
			if(doc->filter_param==DISPLACEMENT) ttl+="2 Decay Start";
			ttl+=" - ZV="+zero_value;
			break;
		case HIGHPASS:
			ttl+="HighpassFilter";
			work.Format("(%d,%d) ",(int)(doc->dft.width*1000.0f+0.5f),(int)(doc->dft.displacement*1000.0f+0.5f));
			ttl+=work;
			if(doc->filter_param==WIDTH) ttl+="1 Decay Width";
			if(doc->filter_param==DISPLACEMENT) ttl+="2 Decay Start";
			ttl+=" - ZV="+zero_value;
			break;
		case BANDPASS:
			ttl+="BandpassFilter";
			work.Format("(%d,%d) ",(int)(doc->dft.width*1000.0f+0.5f),(int)((1.0f-doc->dft.displacement)*1000.0f+0.5f));
			ttl+=work;
			if(doc->filter_param==WIDTH) ttl+="1 Decay Width";
			if(doc->filter_param==DISPLACEMENT) ttl+="2 Center Position";
			ttl+=" - ZV="+zero_value;
			break;
		case BANDSTOP:
			ttl+="BandstopFilter";
			work.Format("(%d,%d,%d) ",(int)(doc->dft.width*1000.0f+0.5f),(int)(2.0*(0.5f-doc->dft.displacement)*1000.0f+0.5f),(int)(doc->dft.centre*1000.0f+0.5f));
			ttl+=work;
			if(doc->filter_param==WIDTH) ttl+="1 Decay Width";
			if(doc->filter_param==DISPLACEMENT) ttl+="2 Stopband Width";
			if(doc->filter_param==CENTRE) ttl+="3 Center Position";
			ttl+=" - ZV="+zero_value;
			break;
		case BUTTERWORTH:
			ttl+="Butterworth Filter";
			work.Format("(%d,%d,%d) ",(int)((1.0f-doc->dft.cutoff)*1000.0f+0.5f),(int)(doc->dft.displacement*1000.0f+0.5f),(int)(doc->dft.order));
			ttl+=work;
			if(doc->filter_param==CUTOFF) ttl+="1 Width";
			if(doc->filter_param==DISPLACEMENT) ttl+="2 Centre Position";
			if(doc->filter_param==ORDER) ttl+="3 Shape (Order)";
			ttl+=" - ZV="+zero_value;
			break;
		default:
			ttl="Escape to Finish Session";
		}
		if(doc->display!=OUT_BUFFER) ttl+=" V=";
		else ttl+=" A1=";
		ttl+=buffer[doc->active_frame].name;
		if(doc->display==OUT_BUFFER) ttl+=" V=out";
		if(buffer[doc->active_frame].hidden) ttl+="*";
		goto finish;
	}

	if(doc->top_frame==0) ttl="CombineZ5 Development Version";
	else
	{
		if(doc->mode&SPECIAL)
		{
			ttl="T=";
			switch(doc->mode)
			{
			case ADD:
				ttl+="Add";
				break;
			case CORRECT:
				ttl+="Correct";
				break;
			case BLEND:
				ttl+="Blend";
				break;
			case RED:
				ttl+="Red";
				break;
			case GREEN:
				ttl+="Green";
				break;
			case BLUE:
				ttl+="Blue";
				break;
			case CONTRASTX:
				ttl+="Contrast";
				break;
			case BRIGHTNESSX:
				ttl+="Bright";
				break;
			default:
				ttl+="None";
			}

			ttl+=",Fix V=";
			if(doc->magon)
			{
				if(doc->full_mode) ttl+="Full";
				else ttl+="Mag.";
			} else ttl+="Norm.";
			ttl+=buffer[doc->display].name;
			if((doc->active_frame==doc->display) && (buffer[doc->active_frame].hidden)) ttl+="*";

			ttl+=" A1=";
			ttl+=buffer[doc->active_frame].name;
			if(buffer[doc->active_frame].hidden) ttl+="*";

			ttl+=" A2=";
			if(doc->active_frame2!=UNSET)
			{
				ttl+=buffer[doc->active_frame2].name;
				if(buffer[doc->active_frame2].hidden) ttl+="*";
			} else {
				ttl+="None";
			}

			goto finish;
		}

		ttl="T=";
		switch(doc->mode)
		{
		case SIZE:
			ttl+="Size";
			break;
		default:
			ttl+="None";
		}

		if(doc->magon && !doc->full_mode)
		{
			if(doc->fix_active) ttl+=" M=Fix";
			else ttl+=" M=Paint";
		}
		else ttl+=" M=Select";

		ttl+=" V";
		if(doc->fix_active) ttl+="Fixed";
		if(doc->magon)
		{
			if(doc->full_mode) ttl+="Full";
			else ttl+="Mag.";
		} else ttl+="Normal";
		if(doc->plane_mode) ttl+="Plane";
		if(doc->depth_flag) ttl+="Depth";
		ttl+="=";
		ttl+=buffer[f].name;
		if(buffer[f].hidden) ttl+="*";

		ttl+=" A1=";
		ttl+=buffer[doc->active_frame].name;
		if(buffer[doc->active_frame].hidden) ttl+="*";

		ttl+=" A2=";
		if(doc->active_frame2!=UNSET)
		{
			ttl+=buffer[doc->active_frame2].name;
			if(buffer[doc->active_frame2].hidden) ttl+="*";
		} else {
			ttl+="None";
		}
	}
finish:
	if(ttl.Compare(old)) GetParent()->SetWindowText(ttl);
	old=ttl;
}

void CCombineView::OnDraw(CDC* pDC)
{
	CCombineDoc* pDoc = GetDocument();

	if(!pDoc->top_frame) return;
	if(pDoc->buissy) return;
	if(pDoc->macro_mode) return;

	ASSERT_VALID(pDoc);

	static int oldw=999999,oldh=999999;

	if(pDoc->new_flag) new_doc();

	title();

	int x,y,xm,ym;

	int right,bottom,left,top;

	if(!pDoc->top_frame) return; // nothing there to draw

	unsigned char* in;

	BeginWaitCursor();

	// picture is used to display contents of a plane
	if(!pDoc->plane_mode) in=(unsigned char*)pDoc->GetBuffer(pDoc->display);
	else in=(unsigned char*)pDoc->GetBuffer(PICTURE_BUFFER);

	int *depth=(int*)pDoc->GetBuffer(DEPTH_BUFFER);

	BITMAPINFO bmi={0};
	bmi.bmiHeader.biBitCount=32;
	bmi.bmiHeader.biHeight=-(int)pDoc->h;
	bmi.bmiHeader.biPlanes=1;
	bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth=pDoc->w;
	bmi.bmiHeader.biCompression=BI_RGB;
	bmi.bmiHeader.biClrUsed=0;

	CBitmap picout,*tempbmp;
	picout.CreateCompatibleBitmap(pDC,pDoc->w,pDoc->h);
	SetDIBits(pDC->m_hDC,(HBITMAP)picout,0,pDoc->h,in,&bmi,DIB_RGB_COLORS);

	CDC src;
	src.CreateCompatibleDC(pDC);
	tempbmp=(CBitmap*)src.SelectObject(picout);

	CRect r;
	GetClientRect(&r);
	r.left+=BORDER;
	r.top+=BORDER;

	int wscroll=GetSystemMetrics(SM_CXVSCROLL);
	int hscroll=GetSystemMetrics(SM_CYHSCROLL);

	CRect wr;
	GetClientRect(&wr);
	wr.right+=wscroll;
	wr.bottom+=hscroll;

	pDoc->clientw=wr.Width()+wscroll;
	pDoc->clienth=wr.Height()+hscroll;
	pDoc->xfactor=(wr.Width()-wscroll)/(pDoc->mag.Width()+1);
	pDoc->yfactor=(wr.Height()-hscroll)/(pDoc->mag.Height()+1);

	// do we need to magnify but don't cnow the scale factors?
	if(pDoc->magon && !pDoc->full_mode && (pDoc->xfactor==0 || pDoc->yfactor==0))
	{
		pDoc->magon=false;
		pDoc->full_mode=false;
		GetClientRect(&r);
		pDC->FillSolidRect(&r,0);
		if(!pDoc->show_fixed)
		{
			pDoc->show_fixed=true;
			pDoc->redraw_picture();
		}
	}

	if(pDoc->full_mode)
	{
		scroll(); // to make sure scroll bars are off

		GetClientRect(&r);

		r.left=BORDER;
		r.top=BORDER;
		r.right-=BORDER;
		r.bottom-=BORDER;

		// to show picture including bordewrs
		pDoc->scale=((double)r.Width()-1)/((double)pDoc->w-1);
		double hh=pDoc->scale*((double)pDoc->h);
		if(hh>(double)(r.Height()))
		{
			pDoc->scale=((double)r.Height()-1)/((double)pDoc->h-1);
		}

		if(!pDoc->show_fixed)
		{
			pDoc->show_fixed=true;
			pDoc->redraw_picture();
		}

		pDC->SetStretchBltMode(HALFTONE);
		pDC->StretchBlt(BORDER,BORDER,(int)((double)(pDoc->w)*pDoc->scale),(int)((double)(pDoc->h)*pDoc->scale),
		&src,0,0,pDoc->w,pDoc->h,
		SRCCOPY);

		draw_points(pDC,BORDER,BORDER,(int)((double)(pDoc->w)*pDoc->scale),(int)((double)(pDoc->h)*pDoc->scale));

		// draw selection box
		pDC->SetBkColor(0);
		CPen pen(PS_DOT,0,0x00ffffff);
		CPen *old_pen=(CPen*)pDC->SelectObject(pen);
		left=(int)((double)pDoc->mag.left*pDoc->scale)+BORDER;
		right=(int)((double)pDoc->mag.right*pDoc->scale)+BORDER;
		top=(int)((double)pDoc->mag.top*pDoc->scale)+BORDER;
		bottom=(int)((double)pDoc->mag.bottom*pDoc->scale)+BORDER;
		if(left<BORDER) left=BORDER;
		if(right>BORDER+(int)((double)(pDoc->w-1)*pDoc->scale)-1) right=BORDER+(int)((double)(pDoc->w-1)*pDoc->scale)-1;
		if(top<BORDER) top=BORDER;
		if(bottom>BORDER+(int)((double)(pDoc->h-1)*pDoc->scale)-1) bottom=BORDER+(int)((double)(pDoc->h-1)*pDoc->scale)-1;
		pDC->MoveTo(left,top);
		pDC->LineTo(right,top);
		pDC->LineTo(right,bottom);
		pDC->LineTo(left,bottom);
		pDC->LineTo(left,top);
		pDC->SelectObject(&old_pen);
	} else { // not full mode
		if(pDoc->magon)
		{
			MagscrollOn();

			// need to clear borders if different size, often
			if(oldw!=(int)pDoc->mag.Width() || oldh!=(int)pDoc->mag.Height())
			{
				GetClientRect(&r);
				r.left+=(pDoc->mag.Width())*pDoc->xfactor;
				pDC->FillSolidRect(&r,0);

				GetClientRect(&r);
				r.top+=(pDoc->mag.Height())*pDoc->yfactor;
				r.right+=(pDoc->mag.Width())*pDoc->xfactor;
				pDC->FillSolidRect(&r,0);
			}

			pDC->StretchBlt(0,0,pDoc->xfactor*(pDoc->mag.Width()+1),pDoc->yfactor*(pDoc->mag.Height()+1),
				&src,pDoc->mag.left,pDoc->mag.top,pDoc->mag.Width()+1,pDoc->mag.Height()+1,
				SRCCOPY);

			draw_points(pDC,0,0,pDoc->xfactor*(pDoc->mag.Width()+1),pDoc->yfactor*(pDoc->mag.Height()+1));

			// is there room to show fixed pixels as a dot in the coner of each pixel?
			if(pDoc->xfactor>1 && pDoc->yfactor>1)
			{
				// swith of show fixed so this effort is not wasted
				if(pDoc->show_fixed)
				{
					pDoc->show_fixed=false;
					pDoc->redraw_picture();
				}

				int YM;
				int left=pDoc->mag.left;
				int top=pDoc->mag.top;
				int xfactor=pDoc->xfactor;
				int yfactor=pDoc->yfactor;
				int xlimit=(int)((pDoc->mag.Width())*xfactor+1);
				int ylimit=(int)((pDoc->mag.Height())*yfactor+1);
				int colour=~pDoc->wipeout&0xffffff;
				for(y=0, ym=pDoc->mag.top; y<ylimit; y+=yfactor, ym++)
				{
					if(ym==pDoc->h) break;
					YM=ym*pDoc->w;
					for(x=0, xm=left; x<xlimit; x+=xfactor,xm++)
					{
						if(xm==pDoc->w) break;
						if(depth[xm+YM]&fFIXED)
						{
							pDC->SetPixel(x,y,colour);
							if(xfactor<8 || yfactor<8) continue;
							pDC->SetPixel(x+1,y,colour);
							pDC->SetPixel(x,y+1,colour);
							pDC->SetPixel(x+1,y+1,colour);
						}
					}
				}
			} else {
				if(!pDoc->show_fixed)
				{
					pDoc->show_fixed=true;
					pDoc->redraw_picture();
				}
			}
		} else { // normal view i.e. 1 pixel on screen to 1 in picture
			scroll();

			GetClientRect(&r);

			r.left=BORDER;
			r.top=BORDER;
			r.right-=BORDER;
			r.bottom-=BORDER;

			pDC->BitBlt(r.left,r.top,r.Width()+1,r.Height()+1,&src,orgx,orgy,SRCCOPY);
			draw_points(pDC,r.left,r.top,r.Width()+1,r.Height()+1);

			// ******************* need to add points here ***************************
			draw_points(pDC,r.left=BORDER,r.top=BORDER,r.right-=BORDER,r.bottom-=BORDER);
		}
	}
	src.SelectObject(&tempbmp);

	if(!pDoc->plane_mode) pDoc->FreeBuffer(pDoc->display);
	else pDoc->FreeBuffer(PICTURE_BUFFER);
	pDoc->FreeBuffer(DEPTH_BUFFER);
	EndWaitCursor();
}

void CCombineView::draw_points(CDC *dc,int xm,int ym,int xM,int yM)
{
	CCombineDoc* doc=GetDocument();
	int i,x,y,x1,y1;

	int c=~doc->wipeout&0xffffff;
	dc->SetBkColor(0);
	CPen pen(PS_SOLID,0,c);
	CPen *old_pen=(CPen*)dc->SelectObject(pen);

	for(i=0; i<4; i++)
	{
		if(!doc->Sizer.p[i].valid) continue;

		// only show points on relavent frames etc.
		if((doc->display!=PICTURE_BUFFER) && (doc->display!=OUT_BUFFER))
		{
			if(doc->display!=doc->Sizer.p[i].f)	continue;
		}

		x=doc->Sizer.p[i].x;
		y=doc->Sizer.p[i].y;

		if(!doc->magon)
		{
			x+=BORDER-orgx; y+=BORDER-orgy;
		} else {
			if(doc->full_mode)
			{
				x=(int)(x*doc->scale)+BORDER;
				y=(int)(y*doc->scale)+BORDER;
			} else {
				x=(x-doc->mag.left)*doc->xfactor+doc->xfactor/2;
				y=(y-doc->mag.top)*doc->yfactor+doc->yfactor/2;
			}
		}

		// don't show points off edge of screen
		if(x-5<xm) continue;
		if(y-5<ym) continue;
		if(x+5>xM) continue;
		if(y+5>yM) continue;

		if(i==0) dc->MoveTo(x-4,y-4); else dc->MoveTo(x-2,y-2);
		if(i==2) dc->LineTo(x+5,y+5); else dc->LineTo(x+3,y+3);
		if(i==1) dc->MoveTo(x+4,y-4); else dc->MoveTo(x+2,y-2);
		if(i==3) dc->LineTo(x-5,y+5); else dc->LineTo(x-3,y+3);
	}

	// draw lines
	for(i=0; i<4; i+=2)
	{
		if(!(doc->Sizer.p[i].valid && doc->Sizer.p[i+1].valid)) continue;

		if((doc->display!=PICTURE_BUFFER) && (doc->display!=OUT_BUFFER))
		{
			if(doc->display!=doc->Sizer.p[i].f) continue;
			if(doc->display!=doc->Sizer.p[i+1].f) continue;
		}

		x=doc->Sizer.p[i].x;
		y=doc->Sizer.p[i].y;
		x1=doc->Sizer.p[i+1].x;
		y1=doc->Sizer.p[i+1].y;
		if(!doc->magon)
		{
			x+=BORDER-orgx; y+=BORDER-orgy;
			x1+=BORDER-orgx; y1+=BORDER-orgy;
		} else {
			if(doc->full_mode)
			{
				x=(int)(x*doc->scale)+BORDER;
				y=(int)(y*doc->scale)+BORDER;
				x1=(int)(x1*doc->scale)+BORDER;
				y1=(int)(y1*doc->scale)+BORDER;
			} else {
				x=(x-doc->mag.left)*doc->xfactor+doc->xfactor/2;
				y=(y-doc->mag.top)*doc->yfactor+doc->yfactor/2;
				x1=(x1-doc->mag.left)*doc->xfactor+doc->xfactor/2;
				y1=(y1-doc->mag.top)*doc->yfactor+doc->yfactor/2;
			}
		}
		if(x<xm) continue;
		if(y<ym) continue;
		if(x>xM) continue;
		if(y>yM) continue;
		if(x1<xm) continue;
		if(y1<ym) continue;
		if(x1>xM) continue;
		if(y1>yM) continue;

		dc->MoveTo(x,y);
		dc->LineTo(x1,y1);
	}
	dc->SelectObject(&old_pen);
}
