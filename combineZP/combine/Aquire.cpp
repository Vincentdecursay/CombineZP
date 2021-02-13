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
#include "math.h"
#include "time.h"

#include "combine.h"
#include "combineDoc.h"
#include "TakePicture.h"
#include ".\takepicture.h"

#include "objbase.h"
#include "Dshow.h"
#include "ChooseSource.h"
#include "CameraSettings.h"
#include "Qedit.h"
#include "dshowasf.h"
#include "wmsbuffer.h"
#include "wmsdk.h"
#include "MovieControl.h"

HANDLE timer=0;

void StartTimer(__int64 mseconds)
{
#if !defined W2000
	if(timer)CloseHandle(timer); timer=0;
	timer=CreateWaitableTimer(0,false,L"CZMtimer");
	LARGE_INTEGER due;
	due.QuadPart=-10000L*mseconds;
	SetWaitableTimer(timer,&due,0,0,0,false);
#endif
}

void WaitForTimer()
{
#if !defined W2000
	WaitForSingleObject(timer,INFINITE);
	if(timer) CloseHandle(timer);
	timer=0;
#endif
}

//----------------------------------------------------------------------------

IGraphBuilder *Graph=0;
ICaptureGraphBuilder2 *Builder=0;
int nDevMoniker=0;
IMoniker* DevMoniker[20];
CString CamName;
IBaseFilter *cap=0;
IBaseFilter *grab=0;
IBaseFilter *null=0;
IBaseFilter *writer=0;
int resolution=0;
IFileSinkFilter2 *vfsink=0;

void showerror(CString function,int number)
{
	CString s;
	s.Format(L"Some camera's require the Video and Still resolutions to be the same.   If this is not your problem try closeing CombineZP, unplug and then reconnect the camera, then try again. \n\n (Error Number %d in %s)",number,function);
	MessageBox(0,s,L"CombineZP ERROR",MB_OK);
}

void CCombineDoc::startvideo()
{
#if !defined W2000
	CoInitializeEx(0,COINIT_MULTITHREADED);

	HRESULT hr;
	int error=0;
	int i;
	if(Graph==0)
	{
		hr=CoCreateInstance(CLSID_FilterGraph,0,CLSCTX_INPROC_SERVER,IID_IGraphBuilder, (void**)&Graph);
		if(hr!=S_OK){
			error=1;
			goto end;
		}
		hr=CoCreateInstance(CLSID_CaptureGraphBuilder2,0,CLSCTX_INPROC_SERVER,IID_ICaptureGraphBuilder2,(void**)&Builder);
		if(hr!=S_OK){error=2; goto end;}
		hr=Builder->SetFiltergraph(Graph);
		if(hr!=S_OK){error=3; goto end;}
		cap=0;
		hr=CoCreateInstance(CLSID_NullRenderer,0,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void**)&null);
		if(hr!=S_OK){error=4; goto end;}
		hr=CoCreateInstance(CLSID_SampleGrabber,0,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void**)&grab);
		if(hr!=S_OK){error=5; goto end;}
		writer=0;
		for(i=0; i<20; i++)	DevMoniker[i]=0;
		video_showing=false;
		return;
	}
end:
	if(error!=0){showerror(L"startvideo()",error); endvideo();}
	if(writer!=0){writer->Release(); writer=0;}
	if(null!=0){null->Release(); null=0;}
	if(grab!=0){grab->Release(); grab=0;}
	if(Builder!=0){Builder->Release(); Builder=0;}
	if(Graph!=0){Graph->Release(); Graph=0;}
	video_showing=false;

	CoUninitialize();
#endif
}

void CCombineDoc::endvideo()
{
#if !defined W2000
	int i;

	HRESULT hr;

	if(Graph==0)
	{
		for(i=0; i<20; i++)	DevMoniker[i]=0;
		return;
	}

	IMediaControl* mc=0;

	hr=Graph->QueryInterface(IID_IMediaControl,(void **)&mc);
	OAFilterState state;
	hr=mc->GetState(INFINITE,&state);
	hr=mc->Stop();
	while((state!=State_Stopped) || (hr==VFW_S_STATE_INTERMEDIATE))
	{
		hr=mc->Stop();
		hr=mc->GetState(INFINITE,&state);
	}
	mc->Release();

	int n;
	if(vfsink!=0) vfsink->Release(); vfsink=0;
	if(writer!=0){
		n=writer->Release(); writer=0;}
	if(cap!=0){
		n=cap->Release(); cap=0;}
	if(null!=0){
		n=null->Release(); null=0;}
	if(grab!=0){
		n=grab->Release(); grab=0;}
	if(Builder!=0){
		n=Builder->Release(); Builder=0;}

	hr=Graph->QueryInterface(IID_IMediaControl,(void **)&mc);
	hr=mc->GetState(INFINITE,&state);
	hr=mc->Stop();
	while((state!=State_Stopped) || (hr==VFW_S_STATE_INTERMEDIATE))
	{
		hr=mc->Stop();
		hr=mc->GetState(INFINITE,&state);
	}
	mc->Release();

	if(Graph!=0){
		n=Graph->Release(); Graph=0;}
	for(i=0; i<20; i++)
	{
		if(DevMoniker[i]!=0) 
			n=DevMoniker[i]->Release();
		DevMoniker[i]=0;
	}
#endif
}

void CCombineDoc::videowindowon(void)
{
#if !defined W2000
	if(Graph==0) return;
	HRESULT hr;
	int error=0;
	IVideoWindow* vw=0;
	hr=Graph->QueryInterface(IID_IVideoWindow,(void**)&vw);
	if(hr!=S_OK){error=1; goto end;}
	hr=vw->put_Visible(OAFALSE);
	if(hr!=S_OK){error=2; goto end;}
	hr=vw->put_Owner(0);
	if(hr!=S_OK){error=6; goto end;}
	hr=vw->put_WindowStyle(WS_CAPTION);
	if(hr!=S_OK){error=3; goto end;}
	hr=vw->put_Caption(L"Preview");
	if(hr!=S_OK){error=4; goto end;}
	hr=vw->put_Visible(OATRUE);
	if(hr!=S_OK){error=7; goto end;}
	hr=vw->SetWindowForeground(OATRUE);
	if(hr!=S_OK){error=8; goto end;}
	int vww,vwh;
	vww=video_w;
	if(vww>640)
	{
		vwh=(video_h*640)/vww;
		vww=640;
	} else vwh=video_h;
	hr=vw->SetWindowPosition(0,0,vww,vwh);
	if(hr!=S_OK){error=9; goto end;}
end:
	if(error!=0){showerror(L"videowindowon()",error);}
	if(vw!=0) vw->Release();
#endif
}

void CCombineDoc::videowindowoff(void)
{
#if !defined W2000
	if(Graph==0) return;
	HRESULT hr;
	int error=0;
	POSITION pos=GetFirstViewPosition();
	CView *view=GetNextView(pos);
	IVideoWindow* vw=0;
	hr=Graph->QueryInterface(IID_IVideoWindow,(void**)&vw);
	if(hr!=S_OK){error=1; goto end;}
	hr=vw->put_Visible(OAFALSE);
	if(hr!=S_OK){error=2; goto end;}
	hr=vw->put_WindowStyle(WS_CHILD|WS_DLGFRAME);
	if(hr!=S_OK){error=3; goto end;}
	hr=vw->put_Owner((OAHWND)view->m_hWnd);
	if(hr!=S_OK){error=4; goto end;}
end:
	if(vw) vw->Release();
	if(error!=0){showerror(L"videowindowoff()",error);}
#endif
}

//NEW
void CCombineDoc::OnAcquireimageSelectsource()
{
#if !defined W2000
	if(Graph)
	{
		if(video_showing) togglevideo();
		endvideo();
	}
	startvideo();
	if(Graph==0) return;

	HRESULT hr;
	int error=0;

	CChooseSource cs;
	hr=cs.DoModal();
	if((hr==IDCANCEL) || (cap==0))
	{
		endvideo();
		return;
	}
	progress.AddString(CamName);

	BeginWaitCursor();

	IAMStreamConfig *config=0;
	AM_MEDIA_TYPE *fmt=0;
	VIDEOINFOHEADER *vinfo=0;
	VIDEO_STREAM_CONFIG_CAPS scc; ZeroMemory(&scc,sizeof(scc));

	hr=Builder->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,cap,IID_IAMStreamConfig,(void**)&config);
	if(hr!=S_OK){error=1; goto end;}
	resolution=cs.m_resolution;
	hr=config->GetStreamCaps(resolution,&fmt,(BYTE*)&scc);
	if(hr!=S_OK){error=2; goto end;}
	video_w=scc.InputSize.cx;
	video_h=scc.InputSize.cy;
	vinfo=(VIDEOINFOHEADER*)fmt->pbFormat;
	vinfo->AvgTimePerFrame=cs.m_rate;
	hr=config->SetFormat(fmt);
	if(hr!=S_OK){error=3; goto end;}
	config->Release(); config=0;
	CoTaskMemFree((PVOID)fmt->pbFormat);
	CoTaskMemFree(fmt); fmt=0;
	ZeroMemory(&scc,sizeof(scc));

	hr=Builder->RenderStream(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video,cap,0,0);
	if((hr!=S_OK) && (hr!=VFW_S_NOPREVIEWPIN)){error=4; goto end;}

	videowindowoff();
	IVideoWindow* vw=0;
	hr=Graph->QueryInterface(IID_IVideoWindow,(void**)&vw);
	if(hr!=S_OK){error=5; goto end;}
	hr=vw->put_Visible(OATRUE);
	vw->Release(); vw=0;

	hr=Graph->AddFilter(null,L"Null");
	hr=Graph->AddFilter(grab,L"Grab");

	IPin *still=0,*gin=0,*gout=0,*nin=0;

	hr=Builder->FindPin(cap,PINDIR_OUTPUT,&PIN_CATEGORY_STILL,0,true,0,&still);
	if(hr!=S_OK){error=6; goto end;}
	hr=Builder->FindInterface(&PIN_CATEGORY_STILL,&MEDIATYPE_Video,cap,IID_IAMStreamConfig,(void**)&config);
	if(hr!=S_OK){error=7; goto end;}
	hr=config->GetStreamCaps(cs.m_resolution2,&fmt,(BYTE*)&scc);
	if(hr!=S_OK){error=8; goto end;}
	vinfo=(VIDEOINFOHEADER*)fmt->pbFormat;
	hr=config->SetFormat(fmt);
	if(hr!=S_OK){error=9; goto end;}

	ISampleGrabber *g=0;
	hr=grab->QueryInterface(IID_ISampleGrabber,(void**)&g);
	if(hr!=S_OK){error=10; goto end;}
	hr=g->SetMediaType(fmt);
	if(hr!=S_OK){error=11; goto end;}

	hr=Builder->FindPin(grab,PINDIR_INPUT,0,0,true,0,&gin);
	if(hr!=S_OK){error=12; goto end;}
	hr=Graph->Connect(still,gin);
	if(hr!=S_OK){error=13; goto end;}
	hr=Builder->FindPin(grab,PINDIR_OUTPUT,0,0,true,0,&gout);
	if(hr!=S_OK){error=14; goto end;}
	hr=Builder->FindPin(null,PINDIR_INPUT,0,0,true,0,&nin);
	if(hr!=S_OK){error=15; goto end;}
	hr=Graph->Connect(gout,nin);
	if(hr!=S_OK){error=16; goto end;}

	AM_MEDIA_TYPE mt; ZeroMemory(&mt,sizeof(mt));
	still->ConnectionMediaType(&mt);
	if(hr!=S_OK){error=17; goto end;}
	vinfo=(VIDEOINFOHEADER*)mt.pbFormat;
	still_w=vinfo->bmiHeader.biWidth;
	still_h=vinfo->bmiHeader.biHeight;
	CoTaskMemFree(mt.pbFormat);

	if(!video_showing) OnAcquireimageTogglevideo();

	loaddefcamsettings();

end:
	if(error!=0){showerror(L"OnAcquireimageSelectsource()",error); endvideo();}
	EndWaitCursor();
	if(config!=0) config->Release();
	if(fmt!=0)
	{
		CoTaskMemFree((PVOID)fmt->pbFormat);
		CoTaskMemFree(fmt);
	}
	if(still!=0) still->Release();
	if(gin!=0) gin->Release();
	if(gout!=0) gout->Release();
	if(nin!=0) nin->Release();
	if(g!=0) g->Release();
#endif
}

void CCombineDoc::OnAcquireimageTogglevideo()
{
#if !defined W2000
	if(Graph==0)
	{
		OnAcquireimageSelectsource();
		return;
	}
	togglevideo();
#endif
}

void CCombineDoc::togglevideo(void)
{
#if !defined W2000
	if(Graph==0) return;

	HRESULT hr;
	int error=0;

	BeginWaitCursor();

	IVideoWindow* vw=0;
	POSITION pos=GetFirstViewPosition();
	CView *view=GetNextView(pos);
	hr=Graph->QueryInterface(IID_IVideoWindow,(void**)&vw);
	if(hr!=S_OK){error=1; goto end;}

	IMediaControl* mc=0;
	OAFilterState state;
	hr=Graph->QueryInterface(IID_IMediaControl,(void **)&mc);
	if(hr!=S_OK){error=2; goto end;}
	hr=mc->GetState(INFINITE,&state);
//	if(!((hr=S_OK) || (hr==VFW_S_STATE_INTERMEDIATE))){error=3; goto end;}
	if((video_showing) && (state==State_Running))
	{
		hr=mc->Stop();
//		if(hr!=S_OK){error=40; goto end;}
		hr=mc->GetState(INFINITE,&state);
		while((state!=State_Stopped) || (hr==VFW_S_STATE_INTERMEDIATE))
		{
			hr=mc->Stop();
			hr=mc->GetState(INFINITE,&state);
//			if(!((hr=S_OK) || (hr==VFW_S_STATE_INTERMEDIATE))){error=41; goto end;}
		}
		mc->Release(); mc=0;
		video_showing=false;
		hr=vw->put_Visible(OAFALSE);
		if(hr!=S_OK){error=5; goto end;}
		vw->Release();
		if(!macro_mode) Redraw(0,true);
		EndWaitCursor();
		return;
	} else {
		if(video_showing)
		{
			mc->Release(); mc=0;
			video_showing=false;
			hr=vw->put_Visible(OAFALSE);
			if(hr!=S_OK){error=5; goto end;}
			vw->Release();
			if(!macro_mode) Redraw(0,true);
			EndWaitCursor();
			return;
		}
	}
	RECT rc;
	view->GetClientRect(&rc);
	rc.bottom-=4;
	rc.left+=4;
	rc.right-=4;
	rc.top+=4;
	int recw=rc.right-rc.left+1,rech=rc.bottom-rc.top+1;
	if((video_w>=recw) || (video_h>=rech))
	{
		int ow,oh;
		ow=(video_w*rech)/video_h;
		if(ow<=recw) 
			oh=rech;
		else
		{
			oh=(video_h*recw)/video_w;
			ow=recw;
		}
		int borderw=(recw-ow)/2,borderh=(rech-oh)/2;
		hr=vw->put_Left(borderw+4);
		if(hr!=S_OK){error=8; goto end;}
		hr=vw->put_Top(borderh+4);
		if(hr!=S_OK){error=9; goto end;}
		hr=vw->put_Height(oh);
		if(hr!=S_OK){error=10; goto end;}
		hr=vw->put_Width(ow);
		if(hr!=S_OK){error=11; goto end;}
	}	else {
		int borderw=(recw-video_w)/2,borderh=(rech-video_h)/2;
		hr=vw->put_Left(borderw);
		if(hr!=S_OK){error=12; goto end;}
		hr=vw->put_Top(borderh);
		if(hr!=S_OK){error=13; goto end;}
		hr=vw->put_Height(video_h);
		if(hr!=S_OK){error=14; goto end;}
		hr=vw->put_Width(video_w);
		if(hr!=S_OK){error=15; goto end;}
	}

	vw->put_Visible(OATRUE);

	hr=mc->Run();
	hr=mc->GetState(INFINITE,&state);
//	if(!((hr==S_OK) || (hr==VFW_S_STATE_INTERMEDIATE))){error=17; goto end;}
	while((state!=State_Running) || (hr==VFW_S_STATE_INTERMEDIATE))
	{
		hr=mc->Run();
		hr=mc->GetState(INFINITE,&state);
//		if(!((hr==S_OK) || (hr==VFW_S_STATE_INTERMEDIATE))){error=18; goto end;}
	}

	video_showing=true;
	if(!macro_mode) Redraw(0,true);
end:
	EndWaitCursor();
	if(error!=0){showerror(L"togglevideo()",error); endvideo();}
	if(vw!=0) vw->Release();
	if(mc!=0) mc->Release();
#endif
}

void CCombineDoc::OnAcquireimageCamerasettings()
{
#if !defined W2000
	if(!video_showing) OnAcquireimageTogglevideo();
	if(Graph==0) return;

	HRESULT hr;

	IAMVfwCaptureDialogs *Vfw=0;
	hr=Builder->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,cap,IID_IAMVfwCaptureDialogs,(void **)&Vfw);
	if(SUCCEEDED(hr))
	{
		if(S_OK==Vfw->HasDialog(VfwCaptureDialog_Display))
		{
			hr=Vfw->ShowDialog(VfwCaptureDialog_Display,AfxGetMainWnd()->m_hWnd);
			Vfw->Release(); Vfw=0;
			return;
		}
	}

	ISpecifyPropertyPages *spp=0;
	CAUUID cauuid;
	hr=cap->QueryInterface(IID_ISpecifyPropertyPages,(void **)&spp);
	if(hr==S_OK)
    {
        hr=spp->GetPages(&cauuid);
        if(hr==S_OK)
        {
			spp->GetPages(&cauuid);
			hr=OleCreatePropertyFrame(AfxGetMainWnd()->m_hWnd,30,30,0,1,
				(IUnknown **)&cap,cauuid.cElems,
				(GUID *)cauuid.pElems,0,0,0);
			CoTaskMemFree(cauuid.pElems);
	        spp->Release();
			if(Vfw) Vfw->Release();	
			return;
        }
    }

	CCameraSettings cs;
	cs.DoModal();

	if(Vfw!=0) Vfw->Release();
	if(spp!=0) spp->Release();
#endif
}

void CCombineDoc::OnAquireimageGetimage()
{
	getimage();
}

void CCombineDoc::getimage(void)
{
#if !defined W2000
	CString s1,s2;

	if(Graph==0) OnAcquireimageSelectsource();
	if(Graph==0) return;

	if(!video_showing) OnAcquireimageTogglevideo();

	if(top_frame!=0)
	{
		if((smallw!=still_w) || (smallh!=still_h))
		{
			if(video_showing)
			{
				togglevideo();
				s1.LoadString(IDS_STRING652);
				s2.LoadString(IDS_STRING653);
				MessageBox(progress.m_hWnd,s1,s2,MB_OK);
				togglevideo();
			} else {
				s1.LoadString(IDS_STRING652);
				s2.LoadString(IDS_STRING653);
				MessageBox(progress.m_hWnd,s1,s2,MB_OK);
			}
			return;
		}
	}

	BeginWaitCursor();

	int button=0;
	CTakePicture takepic;
	if(!macro_mode) button=takepic.DoModal();
	else button=IDC_TAKE_PICTURE;
	while(button==IDC_TAKE_PICTURE)
	{
		takepicture();

		grabtoframe();

		if(!macro_mode) button=takepic.DoModal();
		else button=0;
	}

	EndWaitCursor();

	if(!macro_mode) togglevideo();
#endif
}

void CCombineDoc::takepicture(void)
{
#if !defined W2000
	long* buffer;

	HRESULT hr;
	int error=0;

	if(top_frame==0)
	{
		smallw=still_w; smallh=still_h;
		InitializeMemory();
	}

	buffer=(long*)GetBuffer(GRAB_BUFFER);

	ISampleGrabber *g=0;
	hr=grab->QueryInterface(IID_ISampleGrabber,(void**)&g);
	if(hr!=S_OK){error=1; goto end;}
	hr=g->SetBufferSamples(true);
	if(hr!=S_OK){error=2; goto end;}
	
	IPin *still;
	IAMVideoControl *vc;
	hr=cap->QueryInterface(IID_IAMVideoControl,(void**)&vc);
	if(hr!=S_OK){error=4; goto end;}
	hr=Builder->FindPin(cap,PINDIR_OUTPUT,&PIN_CATEGORY_STILL,0,false,0,&still);
	if(hr!=S_OK){error=5; goto end;}
	hr=vc->SetMode(still,VideoControlFlag_Trigger);
	if(hr!=S_OK){error=6; goto end;}

	clock_t start,now;
	start=clock();
	while(clock()<start+CLOCKS_PER_SEC/2);

	start=clock()/CLOCKS_PER_SEC;
	long size=w*h*4;
	buffer[0]=0x12345678;
	static long check[]={0,0,0,0,0,0,0,0,0};
	while((buffer[0]==0x12345678) || (
		(check[0]==buffer[(9*w*h)/10-1]) &&
		(check[1]==buffer[(8*w*h)/10-1]) &&
		(check[2]==buffer[(7*w*h)/10-1]) &&
		(check[3]==buffer[(6*w*h)/10-1]) &&
		(check[4]==buffer[(5*w*h)/10-1]) &&
		(check[5]==buffer[(4*w*h)/10-1]) &&
		(check[6]==buffer[(3*w*h)/10-1]) &&
		(check[7]==buffer[(2*w*h)/10-1]) &&
		(check[8]==buffer[(1*w*h)/10-1])))
	{
		buffer[0]=0x12345678;
		hr=vc->SetMode(still,VideoControlFlag_Trigger);
		if(hr!=S_OK){error=7; goto end;}
		hr=g->GetCurrentBuffer(&size,0);
		if(hr!=S_OK) continue;
		hr=g->GetCurrentBuffer(&size,buffer);
		if(hr!=S_OK){error=9; goto end;}
		now=clock()/CLOCKS_PER_SEC;
		if(now-start>10) break;
	}
	check[0]=buffer[(9*w*h)/10-1];
	check[1]=buffer[(8*w*h)/10-1];
	check[2]=buffer[(7*w*h)/10-1];
	check[3]=buffer[(6*w*h)/10-1];
	check[4]=buffer[(5*w*h)/10-1];
	check[5]=buffer[(4*w*h)/10-1];
	check[6]=buffer[(3*w*h)/10-1];
	check[7]=buffer[(2*w*h)/10-1];
	check[8]=buffer[(1*w*h)/10-1];

	hr=g->SetBufferSamples(false);
	if(hr!=S_OK){error=10; goto end;}

end:
	UpdateBuffer(GRAB_BUFFER);
	if(error!=0) showerror(L"takepicture()",error);
	if(g!=0) g->Release();
	if(still!=0) still->Release();
	if(vc!=0) vc->Release();
#endif
}

void CCombineDoc::grabtoframe(void)
{
#if !defined W2000
	char *b;
	PIXEL *f;
	CString s,s1;

	if(top_frame==MAX_FRAMES)
	{
		CString s,s1;
		s.LoadString(IDS_FILELIMITREACHED); s1.LoadString(IDS_FILELIMIT);
		MessageBox(progress.m_hWnd,s,s1,MB_OK);
		return;
	}

	b=(char*)GetBuffer(GRAB_BUFFER);

	if(top_frame==0)
	{
		s1.LoadString(IDS_STRING415);
		s.Format(s1,smallw,smallh);
		progress.AddString(s);
		s1.LoadString(IDS_STRING416);
		s.Format(s1,w,h);
		progress.AddString(s);

		sizexorg=w/2;
		sizeyorg=h/2;
		mag.left=mag.top=0;
		mag.bottom=h-1;
		mag.right=w-1;
		saved_mag=mag;
		goodrect.left=goodrect.top=frame_border;
		goodrect.right=frame_border+smallw-1;
		goodrect.bottom=frame_border+smallh-1;
		xfactor=yfactor=0;
		scale=0.0;

		bmfh.bfOffBits=sizeof(bmfh)+sizeof(bmih);
		bmfh.bfSize=sizeof(bmfh)+sizeof(bmih)+w*h*3;
		bmfh.bfType='MB';
		bmfh.bfReserved1=bmfh.bfReserved2=0;

		bmih.biSize=sizeof(bmih);
		bmih.biWidth=w;
		bmih.biHeight=h;
		bmih.biSizeImage=w*h*3;
		bmih.biBitCount=24;
		bmih.biClrImportant=0;
		bmih.biClrUsed=0;
		bmih.biCompression=BI_RGB;
		bmih.biPlanes=1;
		bmih.biXPelsPerMeter=0;
		bmih.biYPelsPerMeter=0;

		CString helppath=AfxGetApp()->m_pszHelpFilePath;
		work_path=helppath.Left(helppath.ReverseFind('\\')+1);
		work_path+="CZTemp\\";
		progress.SetWindowText(L"CZTemp");
		path=work_path;
		_wmkdir(work_path);
		_wmkdir(work_path+L"work\\");
		_wchdir(work_path);
	}

	f=(PIXEL*)GetBuffer(top_frame);
	str1.LoadString(IDS_STRING649);
	buffer[top_frame].name=newnumber(str1);
	buffer[top_frame].hidden=false;
	active_frame=top_frame;
	display=top_frame;

	int x,y,Y,i,startpi,pi=0;
	PIXEL p;
	for(y=smallh-1; y>=0; y--)
	{
		startpi=pi;
		Y=(y+frame_border)*w;
		for(x=frame_border; x<smallw+frame_border; x++)
		{
			i=x+Y;
			p.b=b[pi]; pi++;
			p.g=b[pi]; pi++;
			p.r=b[pi]; pi++;
			p.a=aGOOD;
			f[i]=p;
		}
		pi=startpi+3*smallw;
	}
	TidyOuterEdges(top_frame);

	s1.LoadString(IDS_STRING654);
	s.Format(s1,buffer[top_frame].name);
	progress.AddString(s);

	UpdateBuffer(top_frame);
	FreeBuffer(GRAB_BUFFER);

	top_frame++;
	refresh_used_list();
#endif
}

//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CTakePicture, CDialog)
CTakePicture::CTakePicture(CWnd* pParent)
	: CDialog(CTakePicture::IDD, pParent)
{
}

CTakePicture::~CTakePicture()
{
}

void CTakePicture::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if(!pDX->m_bSaveAndValidate)
	{
		RECT r;
		GetWindowRect(&r);
		r.right-=r.left;
		r.bottom-=r.top;
		r.left=r.top=0;

		pos.left=AfxGetApp()->GetProfileInt(L"DialogData",L"TakePictLeft",0);
		pos.top=AfxGetApp()->GetProfileInt(L"DialogData",L"TakePictTop",0);
		pos.right=AfxGetApp()->GetProfileInt(L"DialogData",L"TakePictRight",r.right);
		pos.bottom=AfxGetApp()->GetProfileInt(L"DialogData",L"TakePictBottom",r.bottom);
	} else {
		GetWindowRect(&pos);
		AfxGetApp()->WriteProfileInt(L"DialogData",L"TakePictLeft",pos.left);
		AfxGetApp()->WriteProfileInt(L"DialogData",L"TakePictTop",pos.top);
		AfxGetApp()->WriteProfileInt(L"DialogData",L"TakePictRight",pos.right);
		AfxGetApp()->WriteProfileInt(L"DialogData",L"TakePictBottom",pos.bottom);
	}
}

BEGIN_MESSAGE_MAP(CTakePicture, CDialog)
	ON_BN_CLICKED(IDC_TAKE_PICTURE, OnBnClickedTakePicture)
	ON_BN_CLICKED(IDC_END_VIDEO, OnBnClickedEndVideo)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CTakePicture message handlers

void CTakePicture::OnBnClickedTakePicture()
{
	GetWindowRect(&pos);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"TakePictLeft",pos.left);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"TakePictTop",pos.top);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"TakePictRight",pos.right);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"TakePictBottom",pos.bottom);
	EndDialog(IDC_TAKE_PICTURE);
}

void CTakePicture::OnBnClickedEndVideo()
{
	GetWindowRect(&pos);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"TakePictLeft",pos.left);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"TakePictTop",pos.top);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"TakePictRight",pos.right);
	AfxGetApp()->WriteProfileInt(L"DialogData",L"TakePictBottom",pos.bottom);
	EndDialog(IDC_END_VIDEO);
}


void CTakePicture::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	MoveWindow(&pos,true);
}

// C:\Documents and Settings\Administrator\My Documents\Visual Studio Projects\combine\Aquire.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "ChooseSource.h"
#include ".\choosesource.h"


// CChooseSource dialog

IMPLEMENT_DYNAMIC(CChooseSource, CDialog)
CChooseSource::CChooseSource(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseSource::IDD, pParent)
	, m_chosen(0)
	, m_resolution(0)
	, m_resolution2(0)
	, m_rate(0)
	, minrate(0)
	, maxrate(0)
{
	CoInitializeEx(0,COINIT_MULTITHREADED);
}

CChooseSource::~CChooseSource()
{
	CoUninitialize();
}

void CChooseSource::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_SOURCELIST,SourceList);
	DDX_Control(pDX,IDC_RESOLUTION,Resolution);
	DDX_Control(pDX,IDC_RESOLUTION2,Resolution2);
	DDX_Control(pDX,IDC_RATE,Rate);
	HRESULT hr;
	int error=0;
#if !defined W2000
	ICreateDevEnum *DevEnum=0;
	IEnumMoniker *EnumVideo=0;
	IPropertyBag *data=0;

	loadprefs();

	if(pDX->m_bSaveAndValidate==false)
	{
		VARIANT name;
		wchar_t *dispname;
		unsigned int hash;
		int i;
		CString s;

		hr=CoCreateInstance(CLSID_SystemDeviceEnum,0,CLSCTX_INPROC_SERVER,IID_ICreateDevEnum,(void**)(&DevEnum));
		if(hr!=S_OK){error=1; goto end;}
		hr=DevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&EnumVideo,0);
		if(hr!=S_OK){error=2; goto end;}
		nDevMoniker=0;
		while(EnumVideo->Next(1,&DevMoniker[nDevMoniker],0)==S_OK)
		{
			hr=DevMoniker[nDevMoniker]->BindToStorage(0,0,IID_IPropertyBag,(void**)(&data));
			if(hr!=S_OK){error=3; goto end;}

			VariantInit(&name);
			hr=data->Read(L"Description",&name,0);
			if(FAILED(hr)) hr=data->Read(L"FriendlyName",&name,0);
			if(hr!=S_OK){error=4; goto end;}

			DevMoniker[nDevMoniker]->GetDisplayName(0,0,&dispname);
			hash=0;
			for(i=0; i<1024; i++)
			{
				if(dispname[i]==0) break;
				hash+=i*(0xf&dispname[i]);
			}

			s.Format(L"%s (%d)",name.bstrVal,hash);
			SourceList.InsertString(nDevMoniker,s);
			m_camname[nDevMoniker]=s;
			VariantClear(&name);
			data->Release(); data=0;
			nDevMoniker++;
		}
		SourceList.SetCurSel(m_chosen);

		OnCbnSelchangeSourcelist();
		VariantClear(&name);
	}

	if(pDX->m_bSaveAndValidate)
	{
		Rate.UpdateData();
		m_rate=GetDlgItemInt(IDC_RATE);
		if(m_rate<minrate) m_rate=minrate;
		if(m_rate>maxrate) m_rate=maxrate;
		saveprefs();
	}
end:
	if(error!=0) showerror(L"CChooseSource::DoDataExchange()",error);
	if(DevEnum!=0) DevEnum->Release();
	if(EnumVideo!=0) EnumVideo->Release();
	if(data!=0) data->Release();
#endif
}

BEGIN_MESSAGE_MAP(CChooseSource, CDialog)
	ON_CBN_SELCHANGE(IDC_SOURCELIST, OnCbnSelchangeSourcelist)
	ON_CBN_SELCHANGE(IDC_RESOLUTION, OnCbnSelchangeResolution)
	ON_CBN_SELCHANGE(IDC_RESOLUTION2, OnCbnSelchangeResolution2)
END_MESSAGE_MAP()

void CChooseSource::OnCbnSelchangeSourcelist()
{
#if !defined W2000
	HRESULT hr;
	int error=0;
	CString s,s1;

	if(cap!=0)
	{
		hr=Graph->RemoveFilter(cap);
		if(hr!=S_OK){error=1; goto end;}
		cap->Release();
		cap=0;
	}

	m_chosen=SourceList.GetCurSel();
	if((m_chosen<0) || (m_chosen>10)) m_chosen=0;

	CamName=m_camname[m_chosen];

	hr=DevMoniker[m_chosen]->BindToObject(0,0,IID_IBaseFilter,(void**)&cap);
	if(hr!=S_OK){error=2; goto end;}

	hr=Graph->AddFilter(cap,L"Capture Filter");
	if(hr!=S_OK){error=3; goto end;}

	int i,j,count,size;
	VIDEO_STREAM_CONFIG_CAPS scc;
	IAMStreamConfig *config;
	AM_MEDIA_TYPE *fmt;
	VIDEOINFOHEADER *vinfo;

	int defrez=0,defi=0;
	hr=Builder->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,cap,IID_IAMStreamConfig,(void**)&config);
	if(hr!=S_OK){error=4; goto end;}

	config->GetNumberOfCapabilities(&count,&size);
	if(hr!=S_OK){error=5; goto end;}

	Resolution.ResetContent();
	for(i=j=0; i<count; i++)
	{
		config->GetStreamCaps(i,&fmt,(BYTE*)&scc);
		if(fmt->subtype!=MEDIASUBTYPE_RGB24)
		{
			CoTaskMemFree((PVOID)fmt->pbFormat);
			CoTaskMemFree(fmt);
			continue;
		}
		if(scc.InputSize.cx>=defrez){defrez=scc.InputSize.cx; defi=j;}
		vinfo=(VIDEOINFOHEADER*)fmt->pbFormat;
		s1.LoadString(IDS_STRING650);
		s.Format(s1,scc.InputSize.cx,scc.InputSize.cy);
		Resolution.InsertString(j,s); j++;
		CoTaskMemFree((PVOID)fmt->pbFormat);
		CoTaskMemFree(fmt);
	}
	if(m_resolution>=j) m_resolution=defi;
	Resolution.SetCurSel(m_resolution);
	config->Release(); config=0;

	hr=Builder->FindInterface(&PIN_CATEGORY_STILL,&MEDIATYPE_Video,cap,IID_IAMStreamConfig,(void**)&config);
	if(hr!=S_OK){error=6; goto end;}
	config->GetNumberOfCapabilities(&count,&size);
	if(hr!=S_OK){error=7; goto end;}

	Resolution2.ResetContent();
	for(i=j=0; i<count; i++)
	{
		config->GetStreamCaps(i,&fmt,(BYTE*)&scc);
		if(hr!=S_OK){error=8; goto end;}
		if(fmt->subtype!=MEDIASUBTYPE_RGB24)
		{
			CoTaskMemFree((PVOID)fmt->pbFormat);
			CoTaskMemFree(fmt);
			continue;
		}
		vinfo=(VIDEOINFOHEADER*)fmt->pbFormat;
		s1.LoadString(IDS_STRING651);
		s.Format(s1,scc.InputSize.cx,scc.InputSize.cy);
		Resolution2.InsertString(j,s); j++;
		CoTaskMemFree((PVOID)fmt->pbFormat);
		CoTaskMemFree(fmt);
	}
	if(m_resolution2>=j) m_resolution2=0;
	Resolution2.SetCurSel(m_resolution2);

	OnCbnSelchangeResolution();
	OnCbnSelchangeResolution2();

	saveprefs();
end:
	if(error!=0) showerror(L"OnCbnSelchangeSourcelist()",error);
	if(config!=0) config->Release();
#endif
}

void CChooseSource::OnCbnSelchangeResolution()
{
#if !defined W2000
	HRESULT hr;
	int error=0;

	m_resolution=Resolution.GetCurSel();

	IAMStreamConfig *config=0;
	AM_MEDIA_TYPE *fmt=0;
	VIDEO_STREAM_CONFIG_CAPS scc;
	VIDEOINFOHEADER *vih;
	hr=Builder->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,cap,IID_IAMStreamConfig,(void**)&config);
	if(hr!=S_OK){error=1; goto end;}
	hr=config->GetStreamCaps(m_resolution,&fmt,(BYTE*)&scc);
	if(hr!=S_OK){error=2; goto end;}
	minrate=(int)scc.MinFrameInterval;
	SetDlgItemInt(IDC_MINRATE,(UINT)minrate);
	vih=(VIDEOINFOHEADER*)fmt->pbFormat;
	if(m_rate==0)
		SetDlgItemInt(IDC_RATE,(UINT)vih->AvgTimePerFrame);
	else
		SetDlgItemInt(IDC_RATE,m_rate);
	maxrate=(int)scc.MaxFrameInterval;
	SetDlgItemInt(IDC_MAXRATE,(UINT)maxrate);

	saveprefs();
end:
	if(error!=0) showerror(L"OnCbnSelchangeSourcelist()",error);
	if(config!=0) config->Release();
	if(fmt!=0)
	{
		CoTaskMemFree((PVOID)fmt->pbFormat);
		CoTaskMemFree(fmt);
	}
#endif
}

void CChooseSource::OnCbnSelchangeResolution2()
{
	m_resolution2=Resolution2.GetCurSel();
	saveprefs();
}

void CChooseSource::saveprefs()
{
#if !defined W2000
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString settingspath=helppath.Left(helppath.ReverseFind('\\')+1);
	CString str1;
	str1.LoadString(IDS_STRING656);
	settingspath+=str1+L".cpc";
	CFile f;
	if(f.Open(settingspath,CFile::modeCreate|CFile::modeReadWrite,0)==0) return;

	f.Write(&m_chosen,sizeof(m_chosen));
	f.Write(&m_resolution,sizeof(m_resolution));
	f.Write(&m_resolution2,sizeof(m_resolution2));
	f.Write(&m_rate,sizeof(m_rate));

	f.Close();
#endif
}

void CChooseSource::loadprefs()
{
#if !defined W2000
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString settingspath=helppath.Left(helppath.ReverseFind('\\')+1);
	CString str1;
	str1.LoadString(IDS_STRING656);
	settingspath+=str1+L".cpc";
	CFile f;
	if(f.Open(settingspath,CFile::modeReadWrite,0)==0) return;

	f.Read(&m_chosen,sizeof(m_chosen));
	f.Read(&m_resolution,sizeof(m_resolution));
	f.Read(&m_resolution2,sizeof(m_resolution2));
	f.Read(&m_rate,sizeof(m_rate));

	f.Close();
#endif
}

// C:\Documents and Settings\Administrator\My Documents\Visual Studio Projects\combine\Aquire.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "CameraSettings.h"
#include ".\camerasettings.h"


// CCameraSettings dialog

IMPLEMENT_DYNAMIC(CCameraSettings, CDialog)
CCameraSettings::CCameraSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CCameraSettings::IDD, pParent)
	, brightnessmin(0)
	, brightness(0)
	, brightnessmax(0)
	, brightnessauto(0)
	, contrastmin(0)
	, contrast(0)
	, contrastmax(0)
	, contrastauto(0)
	, huemin(0)
	, hue(0)
	, huemax(0)
	, hueauto(0)
	, saturationmin(0)
	, saturation(0)
	, saturationmax(0)
	, saturationauto(0)
	, sharpnessmin(0)
	, sharpness(0)
	, sharpnessmax(0)
	, sharpnessauto(0)
	, gammamin(0)
	, gamma(0)
	, gammamax(0)
	, gammaauto(0)
	, colourmin(0)
	, colour(0)
	, colourmax(0)
	, colourauto(0)
	, whitebalmin(0)
	, whitebal(0)
	, whitebalmax(0)
	, whitebalauto(0)
	, blcompmin(0)
	, blcomp(0)
	, blcompmax(0)
	, blcompauto(0)
	, gainmin(0)
	, gain(0)
	, gainmax(0)
	, gainauto(0)
	, m_CamName(_T(""))
{
}

CCameraSettings::~CCameraSettings()
{
}

void CCameraSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if(!pDX->m_bSaveAndValidate) getsettings();
	else SetWindowText(m_CamName+L" defaults");
}

void CCameraSettings::getsettings(void)
{
#if !defined W2000
	HRESULT hr;
	IAMVideoProcAmp *setting;
	hr=cap->QueryInterface(IID_IAMVideoProcAmp,(void**)&setting);
	long min,max,step,def,flags,val,valf;

	hr=setting->GetRange(VideoProcAmp_Brightness,&min,&max,&step,&def,&flags);
	hr=setting->Get(VideoProcAmp_Brightness,&val,&valf);
	if(hr==E_PROP_ID_UNSUPPORTED)
	{
		GetDlgItem(IDC_BRIGHTNESSMIN)->EnableWindow(false);
		GetDlgItem(IDC_BRIGHTNESS)->EnableWindow(false);
		GetDlgItem(IDC_BRIGHTNESSMAX)->EnableWindow(false);
		GetDlgItem(IDC_BRIGHTNESSAUTO)->EnableWindow(false);
	}
	if(hr==S_OK)
	{
		SetDlgItemInt(IDC_BRIGHTNESSMIN,(int)(min/step));
		SetDlgItemInt(IDC_BRIGHTNESS,(int)(val/step));
		SetDlgItemInt(IDC_BRIGHTNESSMAX,(int)(max/step));
		if(valf&VideoProcAmp_Flags_Auto)
			CheckDlgButton(IDC_BRIGHTNESSAUTO,true);
		else
			CheckDlgButton(IDC_BRIGHTNESSAUTO,false);
	}

	hr=setting->GetRange(VideoProcAmp_Contrast,&min,&max,&step,&def,&flags);
	hr=setting->Get(VideoProcAmp_Contrast,&val,&valf);
	if(hr==E_PROP_ID_UNSUPPORTED)
	{
		GetDlgItem(IDC_CONTRASTMIN)->EnableWindow(false);
		GetDlgItem(IDC_CONTRAST)->EnableWindow(false);
		GetDlgItem(IDC_CONTRASTMAX)->EnableWindow(false);
		GetDlgItem(IDC_CONTRASTAUTO)->EnableWindow(false);
	}
	if(hr==S_OK)
	{
		SetDlgItemInt(IDC_CONTRASTMIN,(int)(min/step));
		SetDlgItemInt(IDC_CONTRAST,(int)(val/step));
		SetDlgItemInt(IDC_CONTRASTMAX,(int)(max/step));
		if(valf&VideoProcAmp_Flags_Auto)
			CheckDlgButton(IDC_CONTRASTAUTO,true);
		else
			CheckDlgButton(IDC_CONTRASTAUTO,false);
	}

	hr=setting->GetRange(VideoProcAmp_Hue,&min,&max,&step,&def,&flags);
	hr=setting->Get(VideoProcAmp_Hue,&val,&valf);
	if(hr==E_PROP_ID_UNSUPPORTED)
	{
		GetDlgItem(IDC_HUEMIN)->EnableWindow(false);
		GetDlgItem(IDC_HUE)->EnableWindow(false);
		GetDlgItem(IDC_HUEMAX)->EnableWindow(false);
		GetDlgItem(IDC_HUEAUTO)->EnableWindow(false);
	}
	if(hr==S_OK)
	{
		SetDlgItemInt(IDC_HUEMIN,(int)(min/step));
		SetDlgItemInt(IDC_HUE,(int)(val/step));
		SetDlgItemInt(IDC_HUEMAX,(int)(max/step));
		if(valf&VideoProcAmp_Flags_Auto)
			CheckDlgButton(IDC_HUEAUTO,true);
		else
			CheckDlgButton(IDC_HUEAUTO,false);
	}

	hr=setting->GetRange(VideoProcAmp_Saturation,&min,&max,&step,&def,&flags);
	hr=setting->Get(VideoProcAmp_Saturation,&val,&valf);
	if(hr==E_PROP_ID_UNSUPPORTED)
	{
		GetDlgItem(IDC_SATURATIONMIN)->EnableWindow(false);
		GetDlgItem(IDC_SATURATION)->EnableWindow(false);
		GetDlgItem(IDC_SATURATIONMAX)->EnableWindow(false);
		GetDlgItem(IDC_SATURATIONAUTO)->EnableWindow(false);
	}
	if(hr==S_OK)
	{
		SetDlgItemInt(IDC_SATURATIONMIN,(int)(min/step));
		SetDlgItemInt(IDC_SATURATION,(int)(val/step));
		SetDlgItemInt(IDC_SATURATIONMAX,(int)(max/step));
		if(valf&VideoProcAmp_Flags_Auto)
			CheckDlgButton(IDC_SATURATIONAUTO,true);
		else
			CheckDlgButton(IDC_SATURATIONAUTO,false);
	}

	hr=setting->GetRange(VideoProcAmp_Sharpness,&min,&max,&step,&def,&flags);
	hr=setting->Get(VideoProcAmp_Sharpness,&val,&valf);
	if(hr==E_PROP_ID_UNSUPPORTED)
	{
		GetDlgItem(IDC_SHARPNESSMIN)->EnableWindow(false);
		GetDlgItem(IDC_SHARPNESS)->EnableWindow(false);
		GetDlgItem(IDC_SHARPNESSMAX)->EnableWindow(false);
		GetDlgItem(IDC_SHARPNESSAUTO)->EnableWindow(false);
	}
	if(hr==S_OK)
	{
		SetDlgItemInt(IDC_SHARPNESSMIN,(int)(min/step));
		SetDlgItemInt(IDC_SHARPNESS,(int)(val/step));
		SetDlgItemInt(IDC_SHARPNESSMAX,(int)(max/step));
		if(valf&VideoProcAmp_Flags_Auto)
			CheckDlgButton(IDC_SHARPNESSAUTO,true);
		else
			CheckDlgButton(IDC_SHARPNESSAUTO,false);
	}

	hr=setting->GetRange(VideoProcAmp_Gamma,&min,&max,&step,&def,&flags);
	hr=setting->Get(VideoProcAmp_Gamma,&val,&valf);
	if(hr==E_PROP_ID_UNSUPPORTED)
	{
		GetDlgItem(IDC_GAMMAMIN)->EnableWindow(false);
		GetDlgItem(IDC_GAMMA)->EnableWindow(false);
		GetDlgItem(IDC_GAMMAMAX)->EnableWindow(false);
		GetDlgItem(IDC_GAMMAAUTO)->EnableWindow(false);
	}
	if(hr==S_OK)
	{
		SetDlgItemInt(IDC_GAMMAMIN,(int)(min/step));
		SetDlgItemInt(IDC_GAMMA,(int)(val/step));
		SetDlgItemInt(IDC_GAMMAMAX,(int)(max/step));
		if(valf&VideoProcAmp_Flags_Auto)
			CheckDlgButton(IDC_GAMMAAUTO,true);
		else
			CheckDlgButton(IDC_GAMMAAUTO,false);
	}

	hr=setting->GetRange(VideoProcAmp_ColorEnable,&min,&max,&step,&def,&flags);
	hr=setting->Get(VideoProcAmp_ColorEnable,&val,&valf);
	if(hr==E_PROP_ID_UNSUPPORTED)
	{
		GetDlgItem(IDC_COLOURMIN)->EnableWindow(false);
		GetDlgItem(IDC_COLOUR)->EnableWindow(false);
		GetDlgItem(IDC_COLOURMAX)->EnableWindow(false);
		GetDlgItem(IDC_COLOURAUTO)->EnableWindow(false);
	}
	if(hr==S_OK)
	{
		SetDlgItemInt(IDC_COLOURMIN,(int)(min/step));
		SetDlgItemInt(IDC_COLOUR,(int)(val/step));
		SetDlgItemInt(IDC_COLOURMAX,(int)(max/step));
		if(valf&VideoProcAmp_Flags_Auto)
			CheckDlgButton(IDC_COLOURAUTO,true);
		else
			CheckDlgButton(IDC_COLOURAUTO,false);
	}

	hr=setting->GetRange(VideoProcAmp_WhiteBalance,&min,&max,&step,&def,&flags);
	hr=setting->Get(VideoProcAmp_WhiteBalance,&val,&valf);
	if(hr==E_PROP_ID_UNSUPPORTED)
	{
		GetDlgItem(IDC_WBALANCEMIN)->EnableWindow(false);
		GetDlgItem(IDC_WBALANCE)->EnableWindow(false);
		GetDlgItem(IDC_WBALANCEMAX)->EnableWindow(false);
		GetDlgItem(IDC_WBALANCEAUTO)->EnableWindow(false);
	}
	if(hr==S_OK)
	{
		SetDlgItemInt(IDC_WBALANCEMIN,(int)(min/step));
		SetDlgItemInt(IDC_WBALANCE,(int)(val/step));
		SetDlgItemInt(IDC_WBALANCEMAX,(int)(max/step));
		if(valf&VideoProcAmp_Flags_Auto)
			CheckDlgButton(IDC_WBALANCEAUTO,true);
		else
			CheckDlgButton(IDC_WBALANCEAUTO,false);
	}

	hr=setting->GetRange(VideoProcAmp_BacklightCompensation,&min,&max,&step,&def,&flags);
	hr=setting->Get(VideoProcAmp_BacklightCompensation,&val,&valf);
	if(hr==E_PROP_ID_UNSUPPORTED)
	{
		GetDlgItem(IDC_BLCOMPMIN)->EnableWindow(false);
		GetDlgItem(IDC_BLCOMP)->EnableWindow(false);
		GetDlgItem(IDC_BLCOMPMAX)->EnableWindow(false);
		GetDlgItem(IDC_BLCOMPAUTO)->EnableWindow(false);
	}
	if(hr==S_OK)
	{
		SetDlgItemInt(IDC_BLCOMPMIN,(int)(min/step));
		SetDlgItemInt(IDC_BLCOMP,(int)(val/step));
		SetDlgItemInt(IDC_BLCOMPMAX,(int)(max/step));
		if(valf&VideoProcAmp_Flags_Auto)
			CheckDlgButton(IDC_BLCOMPAUTO,true);
		else
			CheckDlgButton(IDC_BLCOMPAUTO,false);
	}

	hr=setting->GetRange(VideoProcAmp_Gain,&min,&max,&step,&def,&flags);
	hr=setting->Get(VideoProcAmp_Gain,&val,&valf);
	if(hr==E_PROP_ID_UNSUPPORTED)
	{
		GetDlgItem(IDC_GAINMIN)->EnableWindow(false);
		GetDlgItem(IDC_GAIN)->EnableWindow(false);
		GetDlgItem(IDC_GAINMAX)->EnableWindow(false);
		GetDlgItem(IDC_GAINAUTO)->EnableWindow(false);
	}
	if(hr==S_OK)
	{
		SetDlgItemInt(IDC_GAINMIN,(int)(min/step));
		SetDlgItemInt(IDC_GAIN,(int)(val/step));
		SetDlgItemInt(IDC_GAINMAX,(int)(max/step));
		if(valf&VideoProcAmp_Flags_Auto)
			CheckDlgButton(IDC_GAINAUTO,true);
		else
			CheckDlgButton(IDC_GAINAUTO,false);
	}

	setting->Release();
#endif
}


BEGIN_MESSAGE_MAP(CCameraSettings, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK2, OnBnClickedOk2)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


void CCameraSettings::OnBnClickedOk2()
{
#if !defined W2000
	HRESULT hr;

	IAMVideoProcAmp *setting;
	hr=cap->QueryInterface(IID_IAMVideoProcAmp,(void**)&setting);
	long min,max,step,def,flags,val,valf;

	hr=setting->GetRange(VideoProcAmp_Brightness,&min,&max,&step,&def,&flags);
	if(hr==S_OK)
	{
		val=GetDlgItemInt(IDC_BRIGHTNESS);
		if(IsDlgButtonChecked(IDC_BRIGHTNESSAUTO))
			valf=VideoProcAmp_Flags_Auto;
		else
			valf=VideoProcAmp_Flags_Manual;
		setting->Set(VideoProcAmp_Brightness,val*step,valf);
	}

	hr=setting->GetRange(VideoProcAmp_Contrast,&min,&max,&step,&def,&flags);
	if(hr==S_OK)
	{
		val=GetDlgItemInt(IDC_CONTRAST);
		if(IsDlgButtonChecked(IDC_CONTRASTAUTO))
			valf=VideoProcAmp_Flags_Auto;
		else
			valf=VideoProcAmp_Flags_Manual;
		setting->Set(VideoProcAmp_Contrast,val*step,valf);
	}

	hr=setting->GetRange(VideoProcAmp_Hue,&min,&max,&step,&def,&flags);
	if(hr==S_OK)
	{
		val=GetDlgItemInt(IDC_HUE);
		if(IsDlgButtonChecked(IDC_HUEAUTO))
			valf=VideoProcAmp_Flags_Auto;
		else
			valf=VideoProcAmp_Flags_Manual;
		setting->Set(VideoProcAmp_Hue,val*step,valf);
	}

	hr=setting->GetRange(VideoProcAmp_Saturation,&min,&max,&step,&def,&flags);
	if(hr==S_OK)
	{
		val=GetDlgItemInt(IDC_SATURATION);
		if(IsDlgButtonChecked(IDC_SATURATIONAUTO))
			valf=VideoProcAmp_Flags_Auto;
		else
			valf=VideoProcAmp_Flags_Manual;
		setting->Set(VideoProcAmp_Saturation,val*step,valf);
	}

	hr=setting->GetRange(VideoProcAmp_Sharpness,&min,&max,&step,&def,&flags);
	if(hr==S_OK)
	{
		val=GetDlgItemInt(IDC_SHARPNESS);
		if(IsDlgButtonChecked(IDC_SHARPNESSAUTO))
			valf=VideoProcAmp_Flags_Auto;
		else
			valf=VideoProcAmp_Flags_Manual;
		setting->Set(VideoProcAmp_Sharpness,val*step,valf);
	}

	hr=setting->GetRange(VideoProcAmp_Gamma,&min,&max,&step,&def,&flags);
	if(hr==S_OK)
	{
		val=GetDlgItemInt(IDC_GAMMA);
		if(IsDlgButtonChecked(IDC_GAMMAAUTO))
			valf=VideoProcAmp_Flags_Auto;
		else
			valf=VideoProcAmp_Flags_Manual;
		setting->Set(VideoProcAmp_Gamma,val*step,valf);
	}

	hr=setting->GetRange(VideoProcAmp_ColorEnable,&min,&max,&step,&def,&flags);
	if(hr==S_OK)
	{
		val=GetDlgItemInt(IDC_COLOUR);
		if(IsDlgButtonChecked(IDC_COLOURAUTO))
			valf=VideoProcAmp_Flags_Auto;
		else
			valf=VideoProcAmp_Flags_Manual;
		setting->Set(VideoProcAmp_ColorEnable,val*step,valf);
	}

	hr=setting->GetRange(VideoProcAmp_WhiteBalance,&min,&max,&step,&def,&flags);
	if(hr==S_OK)
	{
		val=GetDlgItemInt(IDC_WBALANCE);
		if(IsDlgButtonChecked(IDC_WBALANCEAUTO))
			valf=VideoProcAmp_Flags_Auto;
		else
			valf=VideoProcAmp_Flags_Manual;
		setting->Set(VideoProcAmp_WhiteBalance,val*step,valf);
	}

	hr=setting->GetRange(VideoProcAmp_BacklightCompensation,&min,&max,&step,&def,&flags);
	if(hr==S_OK)
	{
		val=GetDlgItemInt(IDC_BLCOMP);
		if(IsDlgButtonChecked(IDC_BLCOMPAUTO))
			valf=VideoProcAmp_Flags_Auto;
		else
			valf=VideoProcAmp_Flags_Manual;
		setting->Set(VideoProcAmp_BacklightCompensation,val*step,valf);
	}

	hr=setting->GetRange(VideoProcAmp_Gain,&min,&max,&step,&def,&flags);
	if(hr==S_OK)
	{
		val=GetDlgItemInt(IDC_GAIN);
		if(IsDlgButtonChecked(IDC_GAINAUTO))
			valf=VideoProcAmp_Flags_Auto;
		else
			valf=VideoProcAmp_Flags_Manual;
		setting->Set(VideoProcAmp_Gain,val*step,valf);
	}

	setting->Release();

	getsettings();
#endif
}

void CCameraSettings::OnBnClickedOk()
{
	OnBnClickedOk2();
	OnOK();
}

void CCombineDoc::OnAcquireimageSavesettings()
{
#if !defined W2000
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString settingspath=helppath.Left(helppath.ReverseFind('\\')+1);
	CFile f;
	str1.LoadString(IDS_STRING655);
	CFileDialog fd(false,L"czc",0,0,str1,0);
	fd.m_ofn.lpstrInitialDir=settingspath.GetBuffer(1024);
	str1.LoadString(IDS_STRING656);
	CString temp=str1;
	fd.m_ofn.lpstrFile=temp.GetBuffer(1024);

	IAMVideoProcAmp *setting=0;
	if(!cap) goto finish;
	if(cap->QueryInterface(IID_IAMVideoProcAmp,(void**)&setting)!=S_OK) goto finish;

	long val,valf;

	if(fd.DoModal()==IDCANCEL) goto finish;
	if(!f.Open(fd.GetPathName(),CFile::modeCreate|CFile::modeWrite)) goto finish;

	setting->Get(VideoProcAmp_Brightness,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_Contrast,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_Hue,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_Saturation,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_Sharpness,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_Gamma,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_ColorEnable,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_WhiteBalance,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_BacklightCompensation,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_Gain,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	f.Close();

finish:
	temp.ReleaseBuffer();
	settingspath.ReleaseBuffer();
	if(setting) setting->Release();
#endif
}

void CCombineDoc::OnAcquireimageLoadsettings()
{
#if !defined W2000
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString settingspath=helppath.Left(helppath.ReverseFind('\\')+1);
	CFile f;
	str1.LoadString(IDS_STRING655);
	CFileDialog fd(true,L"czc",0,0,str1,0);
	fd.m_ofn.lpstrInitialDir=settingspath.GetBuffer(1024);
	str1.LoadString(IDS_STRING656);
	CString temp=str1;
	fd.m_ofn.lpstrFile=temp.GetBuffer(1024);

	IAMVideoProcAmp *setting=0;
	if(!cap) goto finish;
	if(cap->QueryInterface(IID_IAMVideoProcAmp,(void**)&setting)!=S_OK) goto finish;

	long val,valf;

	if(fd.DoModal()==IDCANCEL) goto finish;
	str1=fd.GetPathName();
	if(!f.Open(str1,CFile::modeRead)) goto finish;

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Brightness,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Contrast,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Hue,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Saturation,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Sharpness,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Gamma,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_ColorEnable,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_WhiteBalance,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_BacklightCompensation,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Gain,val,valf);

	f.Close();

finish:
	temp.ReleaseBuffer();
	settingspath.ReleaseBuffer();
	if(setting) setting->Release();
#endif
}

void CCombineDoc::savedefcamsettings()
{
#if !defined W2000
	if(!cap) return;
	IAMVideoProcAmp *setting=0;
	if(cap->QueryInterface(IID_IAMVideoProcAmp,(void**)&setting)!=S_OK) return;

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString settingspath=helppath.Left(helppath.ReverseFind('\\')+1);
	str1.LoadString(IDS_STRING656);
	settingspath+=str1+L" "+CamName+L".czc";
	CFile f;
	if(f.Open(settingspath,CFile::modeCreate|CFile::modeReadWrite,0)==0)
	{
		setting->Release();
		return;
	}

	long val,valf;

	setting->Get(VideoProcAmp_Brightness,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_Contrast,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_Hue,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_Saturation,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_Sharpness,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_Gamma,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_ColorEnable,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_WhiteBalance,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_BacklightCompensation,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	setting->Get(VideoProcAmp_Gain,&val,&valf);
	f.Write(&val,sizeof(val));
	f.Write(&valf,sizeof(valf));

	f.Close();

	setting->Release();
#endif
}

void CCombineDoc::loaddefcamsettings()
{
#if !defined W2000
	if(!cap) return;
	IAMVideoProcAmp *setting=0;
	if(cap->QueryInterface(IID_IAMVideoProcAmp,(void**)&setting)!=S_OK) return;

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString settingspath=helppath.Left(helppath.ReverseFind('\\')+1);
	str1.LoadString(IDS_STRING656);
	settingspath+=str1+L" "+CamName+L".czc";
	CFile f;
	if(f.Open(settingspath,CFile::modeReadWrite,0)==0)
	{
		setting->Release();
		return;
	}

	long val,valf;

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Brightness,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Contrast,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Hue,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Saturation,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Sharpness,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Gamma,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_ColorEnable,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_WhiteBalance,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_BacklightCompensation,val,valf);

	f.Read(&val,sizeof(val));
	f.Read(&valf,sizeof(valf));
	setting->Set(VideoProcAmp_Gain,val,valf);

	f.Close();

	setting->Release();
#endif
}

void CCombineDoc::OnAcquireimageSavecameradefaults()
{
	CCameraSettings cs;
	cs.m_CamName=CamName;
	if(cs.DoModal()!=IDCANCEL) savedefcamsettings();
}

void CCombineDoc::OnAcquireimageGetmovie()
{
#if !defined W2000
	CString str1,str2;

	CString p;
	CString help_path=AfxGetApp()->m_pszHelpFilePath;
	path=help_path.Left(help_path.ReverseFind('\\')+1);
	path=path+L"CZTemp";

	if(!Graph) OnAcquireimageSelectsource();
	if(!Graph) return;

	if(top_frame)
	{
		if((smallw!=video_w) || (smallh!=video_h))
		{
			str1.LoadString(IDS_STRING652);
			str2.LoadString(192);
			MessageBox(progress.m_hWnd,str1,str2,MB_OK);
			return;
		}
	}

	if(!video_showing) togglevideo();

	CMovieControl mov;
	mov.m_state=IDC_MSTART;
	int button=0;
	while((button!=IDC_MEND) && (button!=IDC_MCANCEL))
	{
		button=mov.DoModal();
		switch(button)
		{
		case IDC_MSTART:
			mov.m_state=0;
			startvidcap(L"CapturedMovie");
			break;
		case IDC_MEND:
			togglevideo();
			endvidcap();
			if(help_window)
			{
				ShowWindow(help_window,SW_MINIMIZE);
			}
			AfxGetMainWnd()->ShowWindow(SW_HIDE);
			progress.Maximize();
			extract=true;
			AfxBeginThread(extractgrabbedT,this);
			break;
		case IDC_MCANCEL:
			togglevideo();
			endvidcap();
			break;
		}
	};
#endif
}

void CCombineDoc::startvidcap(CString p)
{
#if !defined W2000
	HRESULT hr;
	int error=0;

	if(Graph==0) return;

	static AM_MEDIA_TYPE *fmt=0;

	CString px;
	px=path;

	BeginWaitCursor();

	IMediaControl* mc=0;
	hr=Graph->QueryInterface(IID_IMediaControl,(void **)&mc);
	OAFilterState state;
	hr=mc->Stop();
	hr=mc->GetState(INFINITE,&state);
	while((state!=State_Stopped) || (hr==VFW_S_STATE_INTERMEDIATE))
	{
		hr=mc->Stop();
		hr=mc->GetState(INFINITE,&state);
	}

	if(writer==0)
	{
		IAMStreamConfig *config=0;
		VIDEOINFOHEADER *vinfo=0;
		VIDEO_STREAM_CONFIG_CAPS scc;

		hr=Builder->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,cap,IID_IAMStreamConfig,(void**)&config);
		if(hr!=S_OK){error=1; goto end;}
		hr=config->GetStreamCaps(resolution,&fmt,(BYTE*)&scc);
		if(hr!=S_OK){error=2; goto end;}
		vinfo=(VIDEOINFOHEADER*)fmt->pbFormat;
		if(hr!=S_OK){error=3; goto end;}
		config->Release(); config=0;

		IWMProfileManager* pmgr;
		hr=WMCreateProfileManager(&pmgr);

		IWMProfile *profile;
		hr=pmgr->CreateEmptyProfile(WMT_VER_9_0,&profile);
		
		IWMStreamConfig *wmconfig;
		hr=profile->CreateNewStream(WMMEDIATYPE_Video,&wmconfig);

		hr=wmconfig->SetBitrate(vinfo->dwBitRate);

		IWMVideoMediaProps *vmprops=0;
		DWORD wmtsize;
		WM_MEDIA_TYPE *wmtype;
		hr=wmconfig->QueryInterface(IID_IWMVideoMediaProps,(void**)&vmprops);
		hr=vmprops->GetMediaType(0,&wmtsize);
		wmtype=(WM_MEDIA_TYPE *)new char[wmtsize];
		hr=vmprops->GetMediaType(wmtype,&wmtsize);
		*((WMVIDEOINFOHEADER*)wmtype->pbFormat)=*((WMVIDEOINFOHEADER*)vinfo);
		hr=vmprops->SetMediaType(wmtype);
		vmprops->Release(); vmprops=0;
		delete [] wmtype;

		hr=wmconfig->SetStreamNumber(1);
		hr=profile->AddStream(wmconfig);
		wmconfig->Release(); wmconfig=0;

		hr=Builder->SetOutputFileName(&MEDIASUBTYPE_Asf,path+p+L".wmv",&writer,(IFileSinkFilter **)&vfsink);

		IConfigAsfWriter *asfconfig=0;
		hr=writer->QueryInterface(IID_IConfigAsfWriter,(void**)&asfconfig);
		hr=asfconfig->ConfigureFilterUsingProfile(profile);
		asfconfig->Release(); asfconfig=0;
		profile->Release(); profile=0;

		hr=Builder->RenderStream(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,cap,0,writer);

		CoTaskMemFree((PVOID)fmt->pbFormat);
		CoTaskMemFree(fmt);
		pmgr->Release();
	} else {
		Graph->AddFilter(writer,L"Writer");
		hr=Builder->RenderStream(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,cap,0,writer);
	}
end:
	path=px;
	if(mc)
	{
		hr=mc->Run();
		hr=mc->GetState(INFINITE,&state);
		while((state!=State_Running) || (hr==VFW_S_STATE_INTERMEDIATE))
		{
			hr=mc->Run();
			hr=mc->GetState(INFINITE,&state);
		}

		mc->Release();
	}
	if(error!=0){showerror(L"OnAcquireimageSelectsource()",error); endvideo();}
	EndWaitCursor();
	str1.LoadString(IDS_STARTVIDCAP);
	progress.AddString(str1);
#endif
}

void CCombineDoc::endvidcap(void)
{
#if !defined W2000
	HRESULT hr;
	int error=0;

	IMediaControl* mc=0;
	hr=Graph->QueryInterface(IID_IMediaControl,(void **)&mc);
	OAFilterState state;
	hr=mc->Stop();
	hr=mc->GetState(INFINITE,&state);
	while((state!=State_Stopped) || (hr==VFW_S_STATE_INTERMEDIATE))
	{
		hr=mc->Stop();
		hr=mc->GetState(INFINITE,&state);
	}

	hr=Graph->RemoveFilter(writer);

	hr=mc->Run();
	hr=mc->GetState(INFINITE,&state);
	while((state!=State_Running) || (hr==VFW_S_STATE_INTERMEDIATE))
	{
		hr=mc->Run();
		hr=mc->GetState(INFINITE,&state);
	}
	mc->Release();

	str1.LoadString(IDS_ENDVIDCAP);
	progress.AddString(str1);
#endif
}


UINT CCombineDoc::extractgrabbedT(LPVOID param)
{
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->extractgrabbed();
	return 0;
}

void CCombineDoc::extractgrabbed()
{
#if !defined W2000
	HRESULT hr=0;

	CString str;
	str1.LoadString(IDS_STRING616);
	progress.AddString(str1);
	if(!macro_mode) progress.m_cancel=false;

	CString name;
	name=L"MovieFrame";

	CString p;
	CString help_path=AfxGetApp()->m_pszHelpFilePath;
	path=help_path.Left(help_path.ReverseFind('\\')+1);
	path=path+L"CZTemp\\";
	work_path=path;
	_wmkdir(path);
	_wchdir(path);
	progress.SetWindowText(L"CZTemp");

	AfxGetApp()->WriteProfileString(L"File",L"work_path",work_path);

	p=work_path+L"work\\";
	_wmkdir(p);

	CoInitializeEx(0,COINIT_MULTITHREADED);

	INSSBuffer *pIBuffer=0;
	IWMSyncReader *pIReader=0;
	IWMOutputMediaProps *pIProps=0;

	WM_MEDIA_TYPE *pType=0;
	BYTE *pBuffer=0;
	WMVIDEOINFOHEADER *pVidinfo=0;

	QWORD cnsSampleTime=0,cnsSampleDuration=0;
	DWORD dwFlags=0,dwOutputNumber=0;

	DWORD n,o,no,f,nf;
	WORD s;

	WMCreateSyncReader(0,WMT_RIGHT_PLAYBACK,&pIReader); if(hr!=S_OK) goto clearup;
	if(hr=pIReader->Open(path+L"CapturedMovie.wmv")!=S_OK) goto clearup;

    if(pIReader->GetOutputCount(&no)!=S_OK) goto clearup;
	for(o=0; o<no; o++)
	{
		if(pType!=0) delete [] pType; pType=0;
		if(pIProps!=0) pIProps->Release(); pIProps=0;

		if(pIReader->GetOutputProps(o,&pIProps)!=S_OK) continue;
		if(pIProps->GetMediaType(0,&n)!=S_OK) continue;
		pType=(WM_MEDIA_TYPE *)new char[n];
		memset(pType,0,n);
		if(pIProps->GetMediaType(pType,&n)!=S_OK) continue;
		if(pType->majortype!=WMMEDIATYPE_Video) continue;

		if(pIReader->GetOutputFormatCount(o,&nf)!=S_OK) continue;
		for(f=0; f<nf; f++)
		{
			if(pType!=0) delete [] pType; pType=0;
			if(pType!=0) free(pType); pType=0;
			if(pIProps!=0) pIProps->Release(); pIProps=0;
			if(pIReader->GetOutputFormat(o,f,&pIProps)!=S_OK) continue;
			if(pIProps->GetMediaType(0,&n)!=S_OK) continue;
			pType=(WM_MEDIA_TYPE *)new char[n];
			memset(pType,0,n);
			if(pIProps->GetMediaType(pType,&n)!=S_OK) continue;
			if(pType->majortype!=WMMEDIATYPE_Video) continue;
			if(pType->subtype!=WMMEDIASUBTYPE_RGB24) continue;
			break;
		}
		if(f==nf) continue;

		break;
	}
	if(o==no) goto clearup;
	if(pIProps->SetMediaType(pType)!=S_OK) goto clearup;
	if(pIReader->SetOutputProps(o,pIProps)!=S_OK) goto clearup;

	pVidinfo=(WMVIDEOINFOHEADER *)pType->pbFormat;

	if(!top_frame)
	{
		smallw=pVidinfo->rcSource.right;
		smallh=pVidinfo->rcSource.bottom;

		InitializeMemory();
		sizexorg=w/2;
		sizeyorg=h/2;
		mag.left=mag.top=0;
		mag.bottom=h-1;
		mag.right=w-1;
		saved_mag=mag;
		goodrect.left=goodrect.top=frame_border;
		goodrect.right=frame_border+smallw-1;
		goodrect.bottom=frame_border+smallh-1;
		xfactor=yfactor=0;
		scale=0.0;

		bmfh.bfOffBits=sizeof(bmfh)+sizeof(bmih);

		bmfh.bfSize=sizeof(bmfh)+sizeof(bmih)+w*h*3;
		bmfh.bfType='MB';
		bmfh.bfReserved1=bmfh.bfReserved2=0;

		bmih.biSize=sizeof(bmih);
		bmih.biWidth=w;
		bmih.biHeight=h;
		bmih.biSizeImage=w*h*3;
		bmih.biBitCount=24;
		bmih.biClrImportant=0;
		bmih.biClrUsed=0;
		bmih.biCompression=BI_RGB;
		bmih.biPlanes=1;
		bmih.biXPelsPerMeter=0;
		bmih.biYPelsPerMeter=0;
	}

	if(pIReader->GetStreamNumberForOutput(o,(WORD*)&s)!=S_OK) goto clearup;
	if(pIReader->SetReadStreamSamples(s,false)!=S_OK) goto clearup;

	do{
		if(progress.m_cancel) break;
		hr=pIReader->GetNextSample(s,&pIBuffer,&cnsSampleTime,&cnsSampleDuration,&dwFlags,&dwOutputNumber,&s);
		if(hr==S_OK)
		{
			if(pIBuffer->GetBuffer(&pBuffer)!=S_OK) break;

			buffer[top_frame].name.Format(L"%s%05d",name.GetString(),top_frame);
			str1.LoadString(IDS_STRING618);
			progress.AddString(buffer[top_frame].name+str1);

			PIXEL *f1=(PIXEL*)GetBuffer(top_frame);

			used_frame[top_used_frame]=top_frame;
			top_used_frame++;
			last_used_frame=top_used_frame;
			top_frame++;

			int x,y,Y,i,pi=0;
			PIXEL p;
			for(y=smallh-1; y>=0; y--)
			{
				Y=(y+frame_border)*w;
				for(x=frame_border; x<smallw+frame_border; x++)
				{
					i=x+Y;
					p.b=pBuffer[pi]; pi++;
					p.g=pBuffer[pi]; pi++;
					p.r=pBuffer[pi]; pi++;
					p.a=aGOOD;
					f1[i]=p;
				}
			}
			display=active_frame=top_frame-1;
			TidyOuterEdges(display);
			UpdateBuffer(top_frame-1);

			if(top_frame==MAX_FRAMES)
			{
				CString s1,s2;
				s1.LoadString(IDS_FILELIMITREACHED); s2.LoadString(IDS_MOVIE);
				MessageBox(progress.m_hWnd,s1,s2,MB_OK);
				break;
			}

			if(pIBuffer) pIBuffer->Release(); pIBuffer=0;
			pBuffer=0;
			cnsSampleTime=0;
			cnsSampleDuration=0;
			dwFlags=0;
			dwOutputNumber=0;
		}
	} while(hr==S_OK);
clearup:
	pIReader->Close();
	if(pType!=0) free(pType); pType=0;
	if(pIReader!=0) pIReader->Release(); pIReader=0;
	if(pIBuffer!=0) pIBuffer->Release(); pIBuffer=0;
	if(pIProps!=0) pIProps->Release(); pIProps=0;

	if((hr!=S_OK) && (hr!=NS_E_NO_MORE_SAMPLES))
	{
		CString s1,s2;
		s1.LoadString(IDS_ERRORMOVIER); s2.LoadString(IDS_MOVIE);
		progress.MessageBox(s1,s2,MB_OK);
	}

	if(depth_flag)
	{
		delete [] rainbow; rainbow=0;
		depth_flag=false;
		backup_picture_valid=false;
	}

	active_frame2=UNSET;
	special_active=active_frame=display=top_frame-1;

	if(display<0) FreeMemory();
	else save_state();

	CoUninitialize();

	if(top_frame!=0) this_file="";

	if(pType) delete [] pType;
	
	progress.SetPos(0);
	progress.GetDlgItem(IDC_CANCEL)->EnableWindow(true);

	if(!macro_mode)
	{
		progress.m_cancel=false;

		progress.Minimize();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();
	}

	return;
#endif
}

void CCombineDoc::OnAcquireimageCapturemovietofile()
{
	if(!Graph) OnAcquireimageSelectsource();
	if(!Graph) return;

	if(writer)
	{
		writer->Release();
		writer=0;
	}

	CString str1,str2,p;

	path=AfxGetApp()->GetProfileString(L"File",L"Path");
	CFileDialog fd(false,L"wmv",L"cz.wmv",0,L"*.wmv|*.wmv||");
	fd.m_ofn.lpstrInitialDir=path;
	if(fd.DoModal()==IDCANCEL) return;
	path=fd.GetPathName();
	path=path.Left(path.ReverseFind('\\'));
	AfxGetApp()->WriteProfileString(L"File",L"Path",path);

	if(macro_mode)
	{
		startvidcap(fd.GetFileTitle());
		extract=false;
		return;
	}

	if(!video_showing) togglevideo();

	CMovieControl mov;
	mov.m_state=IDC_MSTART;
	int button=0;

	while((button!=IDC_MEND) && (button!=IDC_MCANCEL))
	{
		button=mov.DoModal();
		switch(button)
		{
		case IDC_MSTART:
			mov.m_state=0;
			startvidcap(fd.GetFileTitle());
			break;
		case IDC_MEND:
			togglevideo();
			extract=false;
			endvidcap();
			break;
		case IDC_MCANCEL:
			togglevideo();
			extract=false;
			endvidcap();
			break;
		}
	};
}
