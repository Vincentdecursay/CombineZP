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
#include "gdiplus.h"
#include "direct.h"
#include "math.h"
#include "stdlib.h"
using namespace std;

#include "combine.h"
#include "combineDoc.h"
#include "MovieParams.h"

#include "wmsbuffer.h"
#include "wmsdk.h"
#include "combinedoc.h"

using namespace Gdiplus;

void CCombineDoc::OnMovieMake() 
{
#if !defined W2000
	progress.Maximize();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	AfxBeginThread(MovieMakeT,this);
#endif
}

UINT CCombineDoc::MovieMakeT(LPVOID param)
{
#if !defined W2000
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->MovieMake();
#endif
	return 0;
}

void CCombineDoc::MovieMake()
{
#if !defined W2000

	int e=0;
	CString es;

	CString path="";
	CString save_path="";
	CFileDialog fd(false,L"wmv",L"cz.wmv",0,L"*.wmv|*.wmv||");

	str1.LoadString(IDS_STRING615);
	progress.AddString(str1);
	progress.m_cancel=false;
	progress.SetRange(0,top_used_frame);

	CMovieParams p;
	p.m_height=movh;
	p.m_width=movw;
	p.m_time=movt;
	p.m_quality=movq;
	if(p.DoModal()==IDCANCEL) goto end;
	movh=p.m_height;
	movw=p.m_width;
	movt=p.m_time;
	movq=p.m_quality;

	path=AfxGetApp()->GetProfileString(L"File",L"save_path");
	fd.m_ofn.lpstrInitialDir=path;
	if(fd.DoModal()==IDCANCEL) goto end;
	wchar_t s[1000]; wcscpy(s,fd.GetPathName());
	wchar_t wcPath[1000];
	int i=0; while(s[i]!=0){wcPath[i]=s[i]; i++;}; wcPath[i]=0;
	save_path=fd.GetPathName();
	save_path=save_path.Left(save_path.ReverseFind('\\'));
	AfxGetApp()->WriteProfileString(L"File",L"save_path",save_path);

	int outw=movw,outh=movh,quality=movq,mspf=movt;
	wchar_t wcQuality[10]; _itow(quality,wcQuality,10);
	wchar_t wcDelay[10]; _itow(mspf*10000,wcDelay,10);

	int startx,starty;
	int srcw=smallw,srch=smallh;
	if(!magon)
	{
		srcw=smallw; startx=frame_border;
		srch=smallh; starty=frame_border;
	} else {
		srcw=mag.Width(); startx=mag.left;
		srch=mag.Height(); starty=mag.top;
	}
	srcw=((srcw)/4)*4;
	srch=((srch)/4)*4;

	int destw,desth;
	if(srcw<outw && srch<outh)
	{
		destw=srcw;
		desth=srch;
	} else {
		destw=(srcw*outh)/srch; desth=outh;
		if(destw>outw){desth=(srch*outw)/srcw; destw=outw;}
	}
	destw=((destw)/4)*4;
	desth=((desth)/4)*4;
	wchar_t wcDestw[10]; _itow(destw,wcDestw,10);
	wchar_t wcDesth[10]; _itow(desth,wcDesth,10);

	wchar_t profile[10000]=L"<profile ";
		wcscat(profile,L"version=\"589824\"\n");
		wcscat(profile,L"storageformat=\"1\"\n");
		wcscat(profile,L"name=\"CZMovie\"\n");
		wcscat(profile,L"description=\"Streams: 1 video\">\n");
		wcscat(profile,L"<streamconfig\n");
		wcscat(profile,L"majortype=\"{73646976-0000-0010-8000-00AA00389B71}\"\n");
		wcscat(profile,L"streamnumber=\"1\"\n");
		wcscat(profile,L"streamname=\"Video Stream\"\n");
		wcscat(profile,L"inputname=\"Video809\"\n");
		wcscat(profile,L"bitrate=\"1000000\"\n");
		wcscat(profile,L"bufferwindow=\"3000\"\n");
		wcscat(profile,L"reliabletransport=\"0\"\n");
		wcscat(profile,L"decodercomplexity=\"\"\n");
		wcscat(profile,L"rfc1766langid=\"en-gb\">\n");
		wcscat(profile,L"<videomediaprops\n");
		wcscat(profile,L"maxkeyframespacing=\"80000000\"\n");
		wcscat(profile,L"quality=\""); wcscat(profile,wcQuality); wcscat(profile,L"\"/>\n");
		wcscat(profile,L"<wmmediatype\n");
		wcscat(profile,L"subtype=\"{33564D57-0000-0010-8000-00AA00389B71}\"\n");
		wcscat(profile,L"bfixedsizesamples=\"0\"\n");
		wcscat(profile,L"btemporalcompression=\"1\"\n");
		wcscat(profile,L"lsamplesize=\"0\">\n");
		wcscat(profile,L"<videoinfoheader\n");
		wcscat(profile,L"dwbitrate=\"1000000\"\n");
		wcscat(profile,L"dwbiterrorrate=\"0\"\n");
		wcscat(profile,L"avgtimeperframe=\""); wcscat(profile,wcDelay); wcscat(profile,L"\">\n");
		wcscat(profile,L"<rcsource\n");
		wcscat(profile,L"left=\"0\"\n");
		wcscat(profile,L"top=\"0\"\n");
		wcscat(profile,L"right=\""); wcscat(profile,wcDestw); wcscat(profile,L"\"\n");
		wcscat(profile,L"bottom=\""); wcscat(profile,wcDesth); wcscat(profile,L"\"/>\n");
		wcscat(profile,L"<rctarget\n");
		wcscat(profile,L"left=\"0\"\n");
		wcscat(profile,L"top=\"0\"\n");
		wcscat(profile,L"right=\""); wcscat(profile,wcDestw); wcscat(profile,L"\"\n");
		wcscat(profile,L"bottom=\""); wcscat(profile,wcDesth); wcscat(profile,L"\"/>\n");
		wcscat(profile,L"<bitmapinfoheader\n");
		wcscat(profile,L"biwidth=\""); wcscat(profile,wcDestw); wcscat(profile,L"\"\n");
		wcscat(profile,L"biheight=\""); wcscat(profile,wcDesth); wcscat(profile,L"\"\n");
		wcscat(profile,L"biplanes=\"1\"\n");
		wcscat(profile,L"bibitcount=\"24\"\n");
		wcscat(profile,L"bicompression=\"WMV3\"\n");
		wcscat(profile,L"bisizeimage=\"0\"\n");
		wcscat(profile,L"bixpelspermeter=\"0\"\n");
		wcscat(profile,L"biypelspermeter=\"0\"\n");
		wcscat(profile,L"biclrused=\"0\"\n");
		wcscat(profile,L"biclrimportant=\"0\"/>\n");
		wcscat(profile,L"</videoinfoheader>\n");
		wcscat(profile,L"</wmmediatype>\n");
		wcscat(profile,L"</streamconfig>\n");
		wcscat(profile,L"</profile>");

	HRESULT hr=S_OK;
	int x,y,Y,f;
	PIXEL *f1;
	int pos;
	unsigned char *out=0;
	DWORD fi,nf,n=sizeof(WM_MEDIA_TYPE);

	WMVIDEOINFOHEADER *pVidinfo;
	WM_MEDIA_TYPE *pType=0;

	IWMProfileManager *pProfileMgr=0;
	IWMProfile *pProfile=0;
	IWMWriter *pWriter=0;
	IWMInputMediaProps *pInProps=0;
	INSSBuffer *pSample=0;

	CoInitialize(0);

	hr=WMCreateProfileManager(&pProfileMgr); if(hr!=S_OK){e=1; goto clearup;}
	hr=pProfileMgr->LoadProfileByData(profile,&pProfile); if(hr!=S_OK){e=2; goto clearup;}
	hr=WMCreateWriter(0,&pWriter); if(hr!=S_OK){e=3; goto clearup;}
	hr=pWriter->SetProfile(pProfile); if(hr!=S_OK){e=4; goto clearup;}
	hr=pWriter->GetInputProps(0,&pInProps); if(hr!=S_OK){e=5; goto clearup;}
	hr=pWriter->GetInputFormatCount(0,&nf); if(hr!=S_OK){e=6; goto clearup;}
	for(fi=0; fi<nf; fi++)
	{
		if(pType) free(pType); pType=0; 
		if(pInProps!=0) pInProps->Release(); pInProps=0;
		hr=pWriter->GetInputFormat(0,fi,&pInProps); if(hr!=S_OK){e=7; goto clearup;}
		hr=pInProps->GetMediaType(0,&n); if(hr!=S_OK){e=8; goto clearup;}
		pType=(WM_MEDIA_TYPE *)new char[n];
		hr=pInProps->GetMediaType(pType,&n); if(hr!=S_OK){e=9; goto clearup;}
		if(pType->subtype==WMMEDIASUBTYPE_RGB24) break;
	}
	if(pType->subtype!=WMMEDIASUBTYPE_RGB24){e=10; goto clearup;}

	pVidinfo=(WMVIDEOINFOHEADER *)pType->pbFormat;
	pVidinfo->rcSource.bottom=srch;
	pVidinfo->rcSource.left=0;
	pVidinfo->rcSource.right=srcw;
	pVidinfo->rcSource.top=0;
	pVidinfo->rcTarget=pVidinfo->rcSource;
	pVidinfo->bmiHeader.biBitCount=24;
	pVidinfo->bmiHeader.biCompression=BI_RGB;
	pVidinfo->bmiHeader.biHeight=srch;
	pVidinfo->bmiHeader.biSizeImage=srcw*srch*3;
	pVidinfo->bmiHeader.biWidth=srcw;
	pVidinfo->AvgTimePerFrame=0;

	hr=pInProps->SetMediaType(pType); if(hr!=S_OK){e=11; goto clearup;}
	hr=pWriter->SetInputProps(0,pInProps); if(hr!=S_OK){e=12; goto clearup;}

	hr=pWriter->SetOutputFilename(wcPath); if(hr!=S_OK){e=13; goto clearup;}
	hr=pWriter->BeginWriting();
	if(hr!=S_OK)
	{
		while(hr!=S_OK)
		{
			if(!ReleaseASlot()) break;
			hr=pWriter->BeginWriting();
		}
		e=14;
		if(hr!=S_OK) goto clearup;
	}

	for(f=0; f<last_used_frame; f++)
	{
		progress.SetPos(f);
		if(progress.m_cancel) break;

		pos=0;
		f1=(PIXEL*)GetBuffer(used_frame[f]);
		hr=pWriter->AllocateSample(srcw*srch*3,&pSample);
		while(hr==E_OUTOFMEMORY)
		{
			if(!ReleaseASlot()) break;
			hr=pWriter->AllocateSample(srcw*srch*3,&pSample);
		}
		if(hr!=S_OK)
		{
			e=15;
			FreeBuffer(used_frame[f]);
			pWriter->EndWriting();
			goto clearup;
		}
		hr=pSample->GetBuffer(&out);
		if(hr!=S_OK)
		{
			e=16;
			FreeBuffer(used_frame[f]);
			pWriter->EndWriting();
			goto clearup;
		}
		for(y=starty+srch-1; y>=(int)starty; y--)
		{
			Y=w*y;
			for(x=startx; x<(int)startx+srcw; x++)
			{
				i=x+Y;
				out[pos]=f1[i].b; pos++;
				out[pos]=f1[i].g; pos++;
				out[pos]=f1[i].r; pos++;
			}
		}
		FreeBuffer(used_frame[f]);

		hr=pWriter->WriteSample(0,mspf*10000*f,WM_SF_CLEANPOINT,pSample);
		pSample->Release();
		if(hr!=S_OK)
		{
			e=17;
			pWriter->EndWriting();
			goto clearup;
		}
	}
	hr=pWriter->EndWriting(); if(hr!=S_OK){e=18; goto clearup;}

clearup:
	if(hr!=S_OK)
	{
		CString s1,s2;
		s1.LoadString(IDS_ERRORMOVIE); s2.LoadString(IDS_MOVIE);
		es.Format(s1,e,hr);
		MessageBox(progress.m_hWnd,es,s2,MB_OK);
	}

	if(pType) free(pType); pType=0;
	if((pWriter!=0) && (e!=3) && (e!=6))
	{
		pWriter->EndWriting();
		pWriter->Release(); pWriter=0;
	}
	if((pInProps!=0) && (e!=5)) pInProps->Release(); pInProps=0;
	if((pProfile!=0) && (e!=2)) pProfile->Release(); pProfile=0;
	if((pProfileMgr!=0) && (e!=1)) pProfileMgr->Release(); pProfileMgr=0;
	CoUninitialize();

end:
	progress.SetPos(0);
	progress.GetDlgItem(IDC_CANCEL)->EnableWindow(true);

	progress.m_cancel=false;
	progress.Minimize();
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	AfxGetMainWnd()->SetForegroundWindow();
	AfxGetMainWnd()->SetFocus();
	return;
#endif
}

void CCombineDoc::OnMovieExtractframes()
{
#if !defined W2000
	if(help_window)
	{
		ShowWindow(help_window,SW_MINIMIZE);
	}
	if(top_frame!=0)
	{
		CString s1,s2;
		s1.LoadString(IDS_LOADSAVE); s2.LoadString(IDS_MOVIE);
		int ret=MessageBox(progress.m_hWnd,s1,s2,MB_YESNO);
		if(ret==IDYES) return;
	}
	OnNewDocument();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	progress.Maximize();
	AfxBeginThread(MovieExtractT,this);
#endif
}

UINT CCombineDoc::MovieExtractT(LPVOID param)
{
#if !defined W2000
	CCombineDoc *_this=(CCombineDoc*)param;
	_this->MovieExtract();
#endif
	return 0;
}

void CCombineDoc::MovieExtract()
{
	index--;
	AfxGetApp()->WriteProfileInt(L"File",L"Index",index);

	CString str;
#if !defined W2000
	str1.LoadString(IDS_STRING616);
	progress.AddString(str1);
	progress.m_cancel=false;

	CString name;
	path=AfxGetApp()->GetProfileString(L"File",L"Path");
	work_path=AfxGetApp()->GetProfileString(L"File",L"work_path");
	CString p=work_path.Right(work_path.GetLength()-work_path.ReverseFind('\\')-1);
	progress.SetWindowText(p);
	CFileDialog fd(true,L"wmv",L"cz.wmv",0,L"*.wmv|*.wmv||");
	if(top_frame!=0)
	{
		CString s1,s2;
		s1.LoadString(IDS_LOADSAVE); s2.LoadString(IDS_MOVIE);
		int ret=MessageBox(progress.m_hWnd,s1,s2,MB_YESNO);
		if(ret==IDYES) goto finish;
	}
	fd.m_ofn.lpstrInitialDir=path;
	if(fd.DoModal()==IDCANCEL) goto finish;
	wchar_t st[1000]; wcscpy(st,fd.GetPathName());
	wchar_t wcPath[1000];
	int i=0; while(st[i]!=0){wcPath[i]=st[i]; i++;}; wcPath[i]=0;
	name=fd.GetFileTitle();
	HRESULT hr=S_OK;

	path=fd.GetPathName();
	path=path.Left(path.ReverseFind('\\'));
	AfxGetApp()->WriteProfileString(L"File",L"Path",path);
	_wchdir(path);
	if(top_frame==0)
	{
		str1.LoadString(IDS_STRING617);
		_wmkdir(work_path+str1);
		AfxGetApp()->WriteProfileString(L"File",L"work_path",work_path);
	}

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
	hr=pIReader->Open(wcPath);
	if(hr!=S_OK) goto clearup;

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

	if(pIReader->GetStreamNumberForOutput(o,(WORD*)&s)!=S_OK) goto clearup;
	if(pIReader->SetReadStreamSamples(s,false)!=S_OK) goto clearup;

	do{
		if(progress.m_cancel) break;
		hr=pIReader->GetNextSample(s,&pIBuffer,&cnsSampleTime,&cnsSampleDuration,&dwFlags,&dwOutputNumber,&s);
		if(hr==S_OK)
		{
			if(pIBuffer->GetBuffer(&pBuffer)!=S_OK) break;

			buffer[top_frame].name.Format(L"%d_%s%05d",index,name.GetString(),top_frame);
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
	if(pType!=0) delete [] pType; pType=0;
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

finish:
	if(top_frame!=0) this_file="";

	if(pType) delete [] pType; pType=0;
	
	progress.SetPos(0);
	progress.GetDlgItem(IDC_CANCEL)->EnableWindow(true);

	progress.m_cancel=false;
	progress.Minimize();
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	AfxGetMainWnd()->SetForegroundWindow();
	AfxGetMainWnd()->SetFocus();

	return;
#endif
}
