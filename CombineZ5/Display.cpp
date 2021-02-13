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

// switch colours of some special pixels
void CCombineDoc::flash()
{
	if(!top_frame) return;

	int x;
	int y,Y;
	int i;

	if(wipeout==aCLEAR<<24) wipeout|=0xffffff;
	else wipeout=aCLEAR<<24;

	if((mode&GROUP_MASK)==SPECIAL)
	{
		display=OUT_BUFFER;
		RefreshScreen();
		return;
	}

	PIXEL* picture=(PIXEL*)GetBuffer(PICTURE_BUFFER);
	PIXEL* backup=(PIXEL*)GetBuffer(BACKUP_PICTURE_BUFFER);
	int* depth=(int*)GetBuffer(DEPTH_BUFFER);

	for(y=0; y<h; y++)
	{
		Y=y*w;
		for(x=0; x<w; x++)
		{
			i=x+Y;
			if(picture[i].a==aCLEAR)
				picture[i].abgr=wipeout;
			if(fix_active && (depth[i]&fFIXED) && show_fixed)
			{
				if(wipeout==aCLEAR<<24) picture[i].abgr=wipeout|0xffffff;
				else picture[i].abgr=wipeout&0xff000000;
			}
			if(!depth_flag) backup[i]=picture[i];
		}
	}

	UpdateBuffer(PICTURE_BUFFER);
	UpdateBuffer(BACKUP_PICTURE_BUFFER);
	FreeBuffer(DEPTH_BUFFER);

	display=PICTURE_BUFFER;
	RefreshScreen();
}

void CCombineDoc::OnViewTogglefix() 
{
	flash_fixed();
}

void CCombineDoc::flash_fixed()
{
	if(!top_frame) return;

	saved_pm=plane_mode;

	fix_active=!fix_active;
	display=active_frame;
	backup_picture_valid=false;
	redraw_picture();
	display=active_frame;
	if(!saved_pm) display=PICTURE_BUFFER;
	RefreshScreen();
}

void CCombineDoc::OnStackInterpolate() 
{
	refreshscreen=true;
	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
 	AfxBeginThread(StackInterpolate,this);
}

UINT CCombineDoc::StackInterpolate(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->StackInterpolate();
	return 0;
}

void CCombineDoc::StackInterpolate()
{
	buissy=true;
	progress.AddString(IDS_INTERPOLATE);

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	PIXEL *out=(PIXEL*)GetBuffer(OUT_BUFFER);
	PIXEL *lower,*upper,p,pl,ph;
	int i,d,fraction,lowerf,upperf,r,g,b,f;

	progress.SetRange(0,top_frame-1);
	for(lowerf=0; lowerf<top_frame; lowerf++)
	{
		upperf=lowerf+1;
		upper=(PIXEL*)GetBuffer(upperf);
		lower=(PIXEL*)GetBuffer(lowerf);

		for(i=0; i<w*h; i++)
		{
			d=depth[i];

			if(d&fEMPTY)
			{
				out[i].abgr=0;
				out[i].a=aCLEAR;
				continue;
			}

			f=((d&0xff00)>>8);
			if(f!=lowerf) continue;

			if(f==top_frame-1)
			{
				out[i]=lower[i];
				continue;
			}

			if(buffer[upperf].hidden)
			{
				out[i]=lower[i];
				continue;
			}

			pl=lower[i];
			ph=upper[i];

			fraction=(d&0xff);
			r=(255-fraction)*(int)pl.r+fraction*(int)ph.r;
			g=(255-fraction)*(int)pl.g+fraction*(int)ph.g;
			b=(255-fraction)*(int)pl.b+fraction*(int)ph.b;
			p.r=(unsigned char)(r/255);
			p.g=(unsigned char)(g/255);
			p.b=(unsigned char)(b/255);
			p.a=aGOOD;
			out[i]=p;
		}
		FreeBuffer(lowerf);
		FreeBuffer(upperf);
		progress.SetPos(lowerf);
	}
	FreeBuffer(DEPTH_BUFFER);
	UpdateBuffer(OUT_BUFFER);
	progress.SetPos(0);
	display=OUT_BUFFER;
	if(refreshscreen)
	{
		buissy=false;
		refreshscreen=false;
		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetFocus();
		AfxGetMainWnd()->SetForegroundWindow();
		RefreshScreen();
	}
}

void CCombineDoc::redraw_picture()
{
	int i,f,d;
	if((mode&GROUP_MASK)==SPECIAL) return;

	if(plane_mode)
	{
		draw_plane();
		return;
	}

	int* picture=(int*)GetBuffer(PICTURE_BUFFER),*f0,*f1,*f2,*f3;
	int* backup_picture;

	if(backup_picture_valid && !depth_flag)
	{
		backup_picture=(int*)GetBuffer(BACKUP_PICTURE_BUFFER);

		for(i=0; i<w*h; i++) picture[i]=backup_picture[i];

		FreeBuffer(BACKUP_PICTURE_BUFFER);
		UpdateBuffer(PICTURE_BUFFER);
		return;
	}

	if(!macro_mode) BeginWaitCursor();

	int* depth=(int*)GetBuffer(DEPTH_BUFFER);

	for(i=0; i<w*h; i++)
	{
		if(!(depth[i]&fEMPTY)) break;
	}
	if(i==w*h) goto finish;

	if(depth_flag)
	{
		for(i=0; i<w*h; i++)
		{
			if(depth[i]&fEMPTY) picture[i]=wipeout;
			else picture[i]=((int*)rainbow)[depth[i]&dMASK];
			if(fix_active && (depth[i]&fFIXED) && show_fixed)
			{
				if(wipeout==aCLEAR<<24) picture[i]=wipeout|0xffffff;
				else picture[i]=wipeout&0xff000000;
			}
		}
	} else {
		for(f=0; f<top_frame; f+=4)
		{
			f0=(int*)GetBuffer(f);
			if(f<top_frame-1)
			{
				f1=(int*)GetBuffer(f+1);
				if(f<top_frame-2)
				{
					f2=(int*)GetBuffer(f+2);
					if(f<top_frame-3)
					{
						f3=(int*)GetBuffer(f+3);
						for(i=0; i<w*h; i++)
						{
//							d=((depth[i]&dMASK)>>8);
							d=(((depth[i]&dMASK)+(1<<7))>>8);
							if(depth[i]&fEMPTY) continue;
							if(d==f){picture[i]=f0[i]; continue;}
							if(d==f+1){picture[i]=f1[i]; continue;}
							if(d==f+2){picture[i]=f2[i]; continue;}
							if(d==f+3) picture[i]=f3[i];
						}
						FreeBuffer(f);
						FreeBuffer(f+1);
						FreeBuffer(f+2);
						FreeBuffer(f+3);
						continue;
					} else {
						for(i=0; i<w*h; i++)
						{
//							d=((depth[i]&dMASK)>>8);
							d=(((depth[i]&dMASK)+(1<<7))>>8);
							if(depth[i]&fEMPTY) continue;
							if(d==f){picture[i]=f0[i]; continue;}
							if(d==f+1){picture[i]=f1[i]; continue;}
							if(d==f+2) picture[i]=f2[i];
						}
						FreeBuffer(f);
						FreeBuffer(f+1);
						FreeBuffer(f+2);
						continue;
					}
				} else {
					for(i=0; i<w*h; i++)
					{
//						d=((depth[i]&dMASK)>>8);
						d=(((depth[i]&dMASK)+(1<<7))>>8);
						if(depth[i]&fEMPTY) continue;
						if(d==f){picture[i]=f0[i]; continue;}
						if(d==f+1) picture[i]=f1[i];
					}
					FreeBuffer(f);
					FreeBuffer(f+1);
					continue;
				}
			}
			for(i=0; i<w*h; i++)
			{
//				d=((depth[i]&dMASK)>>8);
				d=(((depth[i]&dMASK)+(1<<7))>>8);
				if(depth[i]&fEMPTY) continue;
				if(d==f) picture[i]=f0[i];
			}
			FreeBuffer(f);
		}
	}

finish:
	for(i=0; i<w*h; i++)
	{
		if(depth[i]&fEMPTY) picture[i]=wipeout;
		if(fix_active && (depth[i]&fFIXED) && show_fixed)
		{
			if(wipeout==aCLEAR<<24) picture[i]=wipeout|0xffffff;
			else picture[i]=wipeout&0xff000000;
		}
	}

	FreeBuffer(DEPTH_BUFFER);

	if((!backup_picture_valid) && (!depth_flag))
	{
		backup_picture=(int*)GetBuffer(BACKUP_PICTURE_BUFFER);

		for(i=0; i<w*h; i++) backup_picture[i]=picture[i];

		UpdateBuffer(BACKUP_PICTURE_BUFFER);

		backup_picture_valid=true;
	}

	UpdateBuffer(PICTURE_BUFFER);
	if(!macro_mode) EndWaitCursor();
}

void CCombineDoc::Redraw(CRect *r, bool flag)
{
	CRect rout;
	POSITION pos=GetFirstViewPosition();
	CView* View=GetNextView(pos);

	if(magon && full_mode)
	{
		if(r)
		{
			rout.left=(int)(scale*(double)r->left)+BORDER-1;
			rout.top=(int)(scale*(double)r->top)+BORDER-1;
			rout.right=(int)(scale*(double)(r->right+1))+BORDER+1;
			rout.bottom=(int)(scale*(double)(r->bottom+1))+BORDER+1;
		} else {
			View->GetClientRect(&rout);
		}
		View->InvalidateRect(&rout,flag);
	} else {
		if(magon)
		{
			if(r)
			{
//				rout.left=r->left-mag.left;
//				rout.top=r->top-mag.top;
				rout.left=0;
				rout.top=0;
				rout.right=xfactor*(r->Width()+1);
				rout.bottom=yfactor*(r->Height()+1);
//				rout.right=r->Width()+1;
//				rout.bottom=r->Height()+1;
			} else {
				View->GetClientRect(&rout);
			}
			View->InvalidateRect(&rout,flag);
		} else {
			if(r)
			{
				rout.left=r->left+BORDER-xorg;
				rout.top=r->top+BORDER-yorg;
				rout.right=r->Width()+BORDER-xorg;
				rout.bottom=r->bottom+BORDER-yorg;
			} else {
				View->GetClientRect(&rout);
			}
			View->InvalidateRect(&rout,flag);
		}
	}
	View->UpdateWindow();
}

void CCombineDoc::OnViewpicture() 
{
	home();
	if(magon && !full_mode) Redraw(&mag,false);
	else Redraw(0,false);
}

void CCombineDoc::OnViewFullmode() 
{
	if(full_mode && magon)
	{
		if(mag.Width()+1<clientw && mag.Height()+1<clienth)
		{
			magon=true;
			full_mode=false;
			redraw_picture();
			Redraw(0,true);
		} else {
			full_mode=false;
			magon=false;
			if(!show_fixed)
			{
				show_fixed=true;
				redraw_picture();
			}
			Redraw(0,true);
		}
		return;
	}
	if(!full_mode && magon)
	{
		full_mode=false;
		magon=false;
		if(!show_fixed)
		{
			show_fixed=true;
			redraw_picture();
		}
		Redraw(0,true);
		return;
	}
	magon=true;
	full_mode=true;
	redraw_picture();
	Redraw(0,true);
}

void CCombineDoc::viewfull()
{
	OnViewFullmode();
}

void CCombineDoc::active_up()
{
	if(top_frame<2) return;

	BeginWaitCursor();
	if((mode&GROUP_MASK)==SPECIAL)
	{
		if(display>top_frame-1) display=special_active;
		if(display<top_frame-1)
		{
			display++;
			special_active=display;
		}
//		else MessageBeep(MB_ICONWARNING);
		EndWaitCursor();
		return;
	}

	if(display>top_frame-1) display=active_frame;

	if(active_frame<top_frame-1)
	{
		active_frame++;
		display=active_frame;
	}
//	else MessageBeep(MB_ICONWARNING);
	EndWaitCursor();
}

void CCombineDoc::active_down()
{
	if(top_frame<2) return;

	BeginWaitCursor();
	if((mode&GROUP_MASK)==SPECIAL)
	{
		if(display>last_used_frame) display=special_active;
		if(display>0)
		{
			display--;
			special_active=display;
		}
//		else MessageBeep(MB_ICONWARNING);
		EndWaitCursor();
		return;
	}

	if(display>top_frame-1) display=active_frame;

	if(active_frame>0)
	{
		active_frame--;
		display=active_frame;
	}
//	else MessageBeep(MB_ICONWARNING);
	EndWaitCursor();
}

void CCombineDoc::OnViewSelectall() 
{
	CRect full(0,0,w-1,h-1);
	CRect original(frame_border,frame_border,frame_border+smallw,frame_border+smallh);

	bool redraw_bg=false;
	if(magon && !full_mode) redraw_bg=true;

	if(!full_mode)
	{
		if((mag!=full) && (mag!=original) && (mag!=goodrect)) saved_mag=mag;
		magon=true;
		full_mode=true;
		mag=full;
		Redraw(0,redraw_bg);
		return;
	}

	magon=true;
	full_mode=true;

	if(mag==full)
	{
		mag=original;
		Redraw(0,redraw_bg);
		return;
	}

	if((mag==original) && (goodrect!=original))
	{
		mag=goodrect;
		Redraw(0,redraw_bg);
		return;
	}

	if((mag==goodrect) && (saved_mag!=goodrect))
	{
		mag=saved_mag;
		Redraw(0,redraw_bg);
		return;
	}

	if(mag!=full)
	{
		saved_mag=mag;
		mag=full;
		Redraw(0,redraw_bg);
		return;
	}
}

void CCombineDoc::selectall()
{
	OnViewSelectall();
}

/*void CCombineDoc::OnViewSelectoriginal() 
{
	mag=goodrect;
	magon=true;
	full_mode=true;
	Redraw(0,true);
}

void CCombineDoc::selectoriginal()
{
	OnViewSelectoriginal();
}
*/
void CCombineDoc::page_down()
{
/*	if(full_mode) return;

	int magh=mag.Height();
	mag.top-=mag.Height();
	if(mag.top<0)
	{
		MessageBeep(MB_ICONASTERISK);
		mag.top=0;
	}
	mag.bottom=mag.top+magh-1;

	RefreshScreen();
*/}

void CCombineDoc::page_up()
{
/*	if(full_mode) return;

	int magh=mag.Height();
	mag.bottom+=mag.Height();
	if(mag.bottom>=h)
	{
		MessageBeep(MB_ICONASTERISK);
		mag.bottom=h-1;
	}
	mag.top=mag.bottom-magh+1;

	RefreshScreen();
*/}

void CCombineDoc::page_left()
{
}

void CCombineDoc::page_right()
{
}
