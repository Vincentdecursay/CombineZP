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
#include "MainFrm.h"

#include "combineDoc.h"
#include "combineView.h"
#include "size1.h"
#include "GridSettings.h"

struct BUFFER buffer[];

void CCombineView::title()
{
	CString ttl,work;
	static CString old="";
	int f;

	CCombineDoc* doc;
	doc=GetDocument();

	if(((CMainFrame*)AfxGetMainWnd())->simple_mode)
	{
		CString s;
		s.LoadString(AFX_IDS_APP_TITLE);
		((CMainFrame*)AfxGetMainWnd())->SetWindowText(s);
		if(doc->top_frame!=0) s=buffer[doc->display].name;
		((CMainFrame*)AfxGetMainWnd())->ToolBarText(s);
		return;
	}

	f=doc->display;

	if((doc->mode&GROUP_MASK)==FILTER)
	{
		CString zero_value; zero_value.Format(L"%1.3f",doc->dft.zero_value); 
		if(doc->mode==FBRIGHTNESS)
		{
			str1.LoadString(IDS_STRING288);
			ttl.Format(str1,(int)(doc->dft.brightness*100.0f+0.5f));
			ttl+=buffer[doc->display].name;
			goto finish;
		}
		if(doc->mode==FTHRESHOLD)
		{
			str1.LoadString(IDS_STRING289);
			ttl.Format(str1,(int)(doc->dft.threshold+0.5f));
			ttl+=buffer[doc->display].name;
			goto finish;
		}
		switch(doc->dft.mode)
		{
		case LOWPASS:
			str1.LoadString(IDS_STRING290);
			ttl+=str1;
			work.Format(L"(%d,%d) ",(int)(doc->dft.width*1000.0f+0.5f),(int)(doc->dft.displacement*1000.0f+0.5f));
			ttl+=work;
			str1.LoadString(IDS_STRING291);
			if(doc->filter_param==WIDTH) ttl+=str1;
			str1.LoadString(IDS_STRING292);
			if(doc->filter_param==DISPLACEMENT) ttl+=str1;
			ttl+=" - ZV="+zero_value;
			break;
		case HIGHPASS:
			str1.LoadString(IDS_STRING295);
			ttl+=str1;
			work.Format(L"(%d,%d) ",(int)(doc->dft.width*1000.0f+0.5f),(int)(doc->dft.displacement*1000.0f+0.5f));
			ttl+=work;
			str1.LoadString(IDS_STRING293);
			if(doc->filter_param==WIDTH) ttl+=str1;
			str1.LoadString(IDS_STRING294);
			if(doc->filter_param==DISPLACEMENT) ttl+=str1;
			ttl+=" - ZV="+zero_value;
			break;
		case BANDPASS:
			str1.LoadString(IDS_STRING296);
			ttl+=str1;
			work.Format(L"(%d,%d) ",(int)(doc->dft.width*1000.0f+0.5f),(int)((1.0f-doc->dft.displacement)*1000.0f+0.5f));
			ttl+=work;
			str1.LoadString(IDS_STRING297);
			if(doc->filter_param==WIDTH) ttl+=str1;
			str1.LoadString(IDS_STRING298);
			if(doc->filter_param==DISPLACEMENT) ttl+=str1;
			ttl+=" - ZV="+zero_value;
			break;
		case BANDSTOP:
			str1.LoadString(IDS_STRING299);
			ttl+=str1;
			work.Format(L"(%d,%d,%d) ",(int)(doc->dft.width*1000.0f+0.5f),(int)(2.0*(0.5f-doc->dft.displacement)*1000.0f+0.5f),(int)(doc->dft.centre*1000.0f+0.5f));
			ttl+=work;
			str1.LoadString(IDS_STRING300);
			if(doc->filter_param==WIDTH) ttl+=str1;
			str1.LoadString(IDS_STRING301);
			if(doc->filter_param==DISPLACEMENT) ttl+=str1;
			str1.LoadString(IDS_STRING302);
			if(doc->filter_param==CENTRE) ttl+=str1;
			ttl+=" - ZV="+zero_value;
			break;
		case BUTTERWORTH:
			str1.LoadString(IDS_STRING303);
			ttl+=str1;
			work.Format(L"(%d,%d,%d) ",(int)((1.0f-doc->dft.cutoff)*1000.0f+0.5f),(int)(doc->dft.displacement*1000.0f+0.5f),(int)(doc->dft.order));
			ttl+=work;
			str1.LoadString(IDS_STRING304);
			if(doc->filter_param==CUTOFF) ttl+=str1;
			str1.LoadString(IDS_STRING305);
			if(doc->filter_param==DISPLACEMENT) ttl+=str1;
			str1.LoadString(IDS_STRING306);
			if(doc->filter_param==ORDER) ttl+=str1;
			ttl+=" - ZV="+zero_value;
			break;
		default:
			str1.LoadString(IDS_STRING307);
			ttl=str1;
		}
		if(doc->display!=OUT_BUFFER) ttl+=" V=";
		else ttl+=" A1=";
		ttl+=buffer[doc->active_frame].name;
		str1.LoadString(IDS_STRING308);
		if(doc->display==OUT_BUFFER) ttl+=str1;
		if(buffer[doc->active_frame].hidden) ttl+="*";
		goto finish;
	}

	if(doc->top_frame==0) ttl=L"CombineZP";
	else
	{
		if(doc->mode&SPECIAL)
		{
			ttl="T=";
			switch(doc->mode)
			{
			case ADD:
				str1.LoadString(IDS_STRING309);
				ttl+=str1;
				break;
			case CORRECT:
				str1.LoadString(IDS_STRING310);
				ttl+=str1;
				break;
			case BLEND:
				str1.LoadString(IDS_STRING311);
				ttl+=str1;
				break;
			case RED:
				str1.LoadString(IDS_STRING312);
				ttl+=str1;
				break;
			case GREEN:
				str1.LoadString(IDS_STRING313);
				ttl+=str1;
				break;
			case BLUE:
				str1.LoadString(IDS_STRING314);
				ttl+=str1;
				break;
			case CONTRASTX:
				str1.LoadString(IDS_STRING315);
				ttl+=str1;
				break;
			case BRIGHTNESSX:
				str1.LoadString(IDS_STRING316);
				ttl+=str1;
				break;
			default:
				str1.LoadString(IDS_STRING317);
				ttl+=str1;
			}

			str1.LoadString(IDS_STRING318);
			ttl+=str1;
			if(doc->magon)
			{
				if(doc->full_mode)
				{
					str1.LoadString(IDS_STRING319);
					ttl+=str1;
				} else { 
					str1.LoadString(IDS_STRING320);
					ttl+=str1;
				}
			} else {
				str1.LoadString(IDS_STRING321);
				ttl+=str1;
			}
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
				str1.LoadString(IDS_STRING322);
				ttl+=str1;
			}

			goto finish;
		}

		ttl="T=";
		switch(doc->mode)
		{
		case CZ_SIZE:
			str1.LoadString(IDS_STRING323);
			ttl+=str1;
			break;
		default:
			str1.LoadString(IDS_STRING322);
			ttl+=str1;
		}

		if(doc->magon && !doc->full_mode)
		{
			if(doc->fix_active)
			{
				str1.LoadString(IDS_STRING324);
				ttl+=str1;
			} else {
				str1.LoadString(IDS_STRING325);
				ttl+=str1;
			}
		} else {
			str1.LoadString(IDS_STRING326);
			ttl+=str1;
		}

		ttl+=" V";
		str1.LoadString(IDS_STRING327);
		if(doc->fix_active) ttl+=str1;
		if(doc->magon)
		{
			if(doc->full_mode)
			{
			str1.LoadString(IDS_STRING328);
				ttl+=str1;
			} else {
			str1.LoadString(IDS_STRING329);
				ttl+=str1;
			}
		} else {
			str1.LoadString(IDS_STRING330);
			ttl+=str1;
		}
		str1.LoadString(IDS_STRING331);
		if(doc->plane_mode) ttl+=str1;
		str1.LoadString(IDS_STRING332);
		if(doc->depth_flag) ttl+=str1;
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
			str1.LoadString(IDS_STRING322);
			ttl+=str1;
		}
	}
finish:
	if(ttl.Compare(old)) GetParent()->SetWindowText(ttl);
	old=ttl;
}

void CCombineView::OnDraw(CDC* pDC)
{
	static bool once=true;

	static bool maglast=false;

	CCombineDoc* pDoc = GetDocument();
	if(pDoc->buissy) return;
	if(pDoc->macro_mode) return;

	static bool videoflag=false;
	static bool skip=false;
	if(skip) return;
	if(pDoc->video_showing)
	{
		if(!videoflag)
		{
			videoflag=true;
			skip=true;
			RECT r;
			GetClientRect(&r);
			pDC->FillSolidRect(&r,GetSysColor(COLOR_WINDOW));
			skip=false;
		}
		return;
	} else videoflag=false;

	if(!pDoc->top_frame) return;

	ASSERT_VALID(pDoc);

	static int oldw=999999,oldh=999999;

	if(pDoc->new_flag) new_doc();

	title();

	int x,y,xm,ym;

	int right,bottom,left,top;

	unsigned char* in;

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
	} else BeginWaitCursor();

	int rastercaps=pDC->GetDeviceCaps(RASTERCAPS);
	bool canblt=((rastercaps&RC_STRETCHBLT)!=0);
	bool candib=((rastercaps&RC_STRETCHDIB)!=0);
	if(!canblt & !candib)
	{
		if(once)
		{
			CString s1,s2;
			s1.LoadString(IDS_LIMITEDCAPS); s2.LoadString(IDS_WARNING);
			MessageBox(s1,s2,MB_OK);
			once=false;
		}
		pDoc->magon=0;
		pDoc->full_mode=0;
	}

	// picture is used to display contents of a plane
	if(!pDoc->plane_mode) in=(unsigned char*)pDoc->GetBuffer(pDoc->display);
	else in=(unsigned char*)pDoc->GetBuffer(PICTURE_BUFFER);

	int *depth=(int*)pDoc->GetBuffer(DEPTH_BUFFER);

	BITMAPINFO bmi;	memset((void *)&bmi,0,sizeof(bmi));
	bmi.bmiHeader.biBitCount=32;
	bmi.bmiHeader.biHeight=-(int)pDoc->h;
	bmi.bmiHeader.biPlanes=1;
	bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth=pDoc->w;
	bmi.bmiHeader.biCompression=BI_RGB;
	bmi.bmiHeader.biClrUsed=0;

	void* ptr=0;
	HBITMAP hDib=0;

	CDC src;
	src.CreateCompatibleDC(0);

	CBitmap picout,*tempbmp=(CBitmap*)0;

	if(canblt)
	{
		if(0==picout.CreateCompatibleBitmap(pDC,pDoc->w,pDoc->h))
		{
			pDoc->progress.AddString("CreateCompatibleBitmap failed");
		} else {
			if(0==SetDIBits(pDC->m_hDC,(HBITMAP)picout,0,pDoc->h,in,&bmi,DIB_RGB_COLORS))
			{
				pDoc->progress.AddString("SetDIBits failed");
				canblt=false;
			} else {
				tempbmp=(CBitmap*)src.SelectObject(picout);
				candib=false;
			}
		}
	}

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

	// do we need to magnify but don't know the scale factors?
	if(pDoc->magon && !pDoc->full_mode && (pDoc->xfactor==0 || pDoc->yfactor==0))
	{
		pDoc->magon=false;
		pDoc->full_mode=false;
		GetClientRect(&r);
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

		if(canblt)
		{
			pDC->SetStretchBltMode(HALFTONE);
			if(0==pDC->StretchBlt(BORDER,BORDER,(int)((double)(pDoc->w)*pDoc->scale),(int)((double)(pDoc->h)*pDoc->scale),
				&src,0,0,pDoc->w,pDoc->h,
				SRCCOPY))
					pDoc->progress.AddString("ERROR BLT 1");
		}

		if(candib)
		{
			pDC->SetStretchBltMode(HALFTONE);
			if(0==StretchDIBits(pDC->m_hDC,BORDER,BORDER,(int)((double)(pDoc->w)*pDoc->scale),(int)((double)(pDoc->h)*pDoc->scale),
				0,0,pDoc->w,pDoc->h,
				in,&bmi,0,SRCCOPY))
					pDoc->progress.AddString("ERROR DIB 1");
		}

		draw_points(pDC,BORDER,BORDER,(int)((double)(pDoc->w)*pDoc->scale),(int)((double)(pDoc->h)*pDoc->scale));

		// draw selection box
		int swx=0,shy=0;
		GetClientRect(&r);
		if(r.Width()<pDoc->w) swx=GetSystemMetrics(SM_CXVSCROLL);
		if(r.Height()<pDoc->h) shy=GetSystemMetrics(SM_CYHSCROLL);
		if((pDoc->mag.Width()+1<r.Width()-swx) &&  (pDoc->mag.Height()+1<r.Height()-shy))
			pDC->SetBkColor(0x0000ff);
		else
			pDC->SetBkColor(0x000000);
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

		maglast=false;
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

			pDC->SetBkColor(0);
			CPen linepen1(PS_SOLID,0,0x00ffffff);
			CPen linepen2(PS_SOLID,0,0x000000ff);

			int lx=(pDoc->mag.Width()+1)*pDoc->xfactor;
			int ly=(pDoc->mag.Height()+1)*pDoc->yfactor;

			CPen *old_pen=(CPen*)pDC->SelectObject(linepen1);
			pDC->MoveTo(lx,0);
			pDC->LineTo(lx,ly);
			pDC->MoveTo(0,ly);
			pDC->LineTo(lx,ly);

			(CPen*)pDC->SelectObject(linepen2);
			pDC->MoveTo(lx+2,0);
			pDC->LineTo(lx+2,ly+2);
			pDC->MoveTo(0,ly+2);
			pDC->LineTo(lx+2,ly+2);

			pDC->SelectObject(&old_pen);

			if(canblt)
			{
				pDC->SetStretchBltMode(HALFTONE);
				if(0==pDC->StretchBlt(0,0,pDoc->xfactor*(pDoc->mag.Width()+1),pDoc->yfactor*(pDoc->mag.Height()+1),
					&src,pDoc->mag.left,pDoc->mag.top,pDoc->mag.Width()+1,pDoc->mag.Height()+1,
					SRCCOPY))
						pDoc->progress.AddString("ERROR BLT 2");
			}

			if(candib)
			{
				pDC->SetStretchBltMode(HALFTONE);
				if(0==StretchDIBits(pDC->m_hDC,0,0,pDoc->xfactor*(pDoc->mag.Width()+1),pDoc->yfactor*(pDoc->mag.Height()+1),
					pDoc->mag.left,pDoc->mag.top,pDoc->mag.Width()+1,pDoc->mag.Height()+1,
					in,&bmi,0,SRCCOPY))
					pDoc->progress.AddString("ERROR DIB 2");
			}


			draw_points(pDC,0,0,pDoc->xfactor*(pDoc->mag.Width()+1),pDoc->yfactor*(pDoc->mag.Height()+1));

			// is there room to show fixed pixels as a dot in the coner of each pixel?
			if(pDoc->xfactor>1 && pDoc->yfactor>1)
			{
				// switch off show fixed so this effort is not wasted
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
			maglast=true;
		} else { // normal view i.e. 1 pixel on screen to 1 in picture
			scroll();

			GetClientRect(&r);

			r.left=BORDER;
			r.top=BORDER;
			r.right-=BORDER;
			r.bottom-=BORDER;

			if(r.Width()>pDoc->w)
			{
				r.right=r.left+pDoc->w-1;
				orgx=0;
			} else {
				if(maglast)
				{
					orgx=pDoc->mag.left+pDoc->mag.Width()/2-(r.right-r.left)/2;
					if(orgx<0) orgx=0;
					if(orgx+r.right>=pDoc->w) orgx=pDoc->w-r.right;
				}
			}

			if(r.Height()>pDoc->h)
			{
				r.bottom=r.top+pDoc->h-1;
				orgy=0;
			} else {
				if(maglast)
				{
					orgy=pDoc->mag.top+pDoc->mag.Height()/2-(r.bottom-r.top)/2;
					if(orgy<0) orgy=0;
					if(orgy+r.bottom>=pDoc->h) orgy=pDoc->h-r.bottom;
				}
			}

			if(!canblt)
			{
				picout.CreateCompatibleBitmap(pDC,pDoc->w,pDoc->h);
				SetDIBits(pDC->m_hDC,(HBITMAP)picout,0,pDoc->h,in,&bmi,DIB_RGB_COLORS);
				tempbmp=(CBitmap*)src.SelectObject(picout);
			}

			if(0==pDC->BitBlt(r.left,r.top,r.Width()+1,r.Height()+1,&src,orgx,orgy,SRCCOPY))
				pDoc->progress.AddString("ERROR BITBLT");

			draw_points(pDC,r.left,r.top,r.Width()+1,r.Height()+1);

			int lx1=r.Width()+BORDER+BORDER/2;
			int ly1=r.Height()+BORDER+BORDER/2;
			int lx2=r.left-BORDER/2;
			int ly2=r.top-BORDER/2;
			CPen linepen1(PS_SOLID,0,0x00ffffff);
			CPen linepen2(PS_SOLID,0,0x00ff0000);
			CPen *old_pen=(CPen*)pDC->SelectObject(linepen1);
			pDC->MoveTo(lx2,ly2);
			pDC->LineTo(lx1,ly2);
			pDC->LineTo(lx1,ly1);
			pDC->LineTo(lx2,ly1);
			pDC->LineTo(lx2,ly2);
			pDC->SelectObject(linepen2);
			lx1+=1;
			ly1+=1;
			lx2-=1;
			ly2-=1;
			pDC->MoveTo(lx2,ly2);
			pDC->LineTo(lx1,ly2);
			pDC->LineTo(lx1,ly1);
			pDC->LineTo(lx2,ly1);
			pDC->LineTo(lx2,ly2);

			scroll();
			maglast=false;
		}
	}
	if(tempbmp) src.SelectObject(&tempbmp);
	if(hDib) DeleteObject(hDib);

	// draw grid
	if(grid_showing)
	{
		pDC->SetBkColor(0);
		CPen gridpen1(PS_SOLID,0,0x00ffffff);
		CPen gridpen2(PS_SOLID,0,0x00888888);
		CPen *old_pen=(CPen*)pDC->SelectObject(gridpen1);
		GetClientRect(&r);
		int dx=r.Width()/gridx;
		int dy=r.Height()/gridy;
		for(x=dx; x<=r.Width()-dx; x+=dx)
		{
			pDC->SelectObject(gridpen1);
			pDC->MoveTo(x,r.top);
			pDC->LineTo(x,r.bottom);
			pDC->SelectObject(gridpen2);
			pDC->MoveTo(x+1,r.top);
			pDC->LineTo(x+1,r.bottom);
		}
		for(y=dy; y<=r.Height()-dy; y+=dy)
		{
			pDC->SelectObject(gridpen1);
			pDC->MoveTo(r.left,y);
			pDC->LineTo(r.right,y);
			pDC->SelectObject(gridpen2);
			pDC->MoveTo(r.left,y+1);
			pDC->LineTo(r.right,y+1);
		}
		pDC->SelectObject(&old_pen);
	}

	if(!pDoc->plane_mode) pDoc->FreeBuffer(pDoc->display);
	else pDoc->FreeBuffer(PICTURE_BUFFER);
	pDoc->FreeBuffer(DEPTH_BUFFER);
	if(!(pDoc->magon && !pDoc->full_mode)) EndWaitCursor();
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
		if(!((doc->Sizer.p[i].valid) && (doc->Sizer.p[i+1].valid))) continue;

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

void CCombineView::OnViewGrid()
{
	if(grid_showing)
	{
		grid_showing=false;
	} else {
		CGridSettings gs;
		gs.m_gridx=gridx;
		gs.m_gridy=gridy;
		int ret=gs.DoModal();
		if(ret==IDCANCEL) return;
		gridx=gs.m_gridx;
		gridy=gs.m_gridy;
		grid_showing=true;
	}
	InvalidateRect(0);
}
