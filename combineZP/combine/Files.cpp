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
#include "atlconv.h"
using namespace std;

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
#include "ImportDM.h"

#include "objbase.h"
#include "Dshow.h"
//#include "ChooseSource.h"
//#include "CameraSettings.h"
//#include "Qedit.h"
#include "dshowasf.h"
#include "wmsbuffer.h"
#include "wmsdk.h"
//#include "MovieControl.h"

extern int jcomp;

// FILES
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num=0;          // number of image encoders
	UINT size=0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo=NULL;

	GetImageEncodersSize(&num,&size);
	if(size==0) return -1;  // Failure

	pImageCodecInfo=(ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo==NULL) return -1;  // Failure

	GetImageEncoders(num,size,pImageCodecInfo);

	for(UINT j=0;j<num;++j)
	{
		if(wcscmp(pImageCodecInfo[j].MimeType,format)==0)
		{
			*pClsid=pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

void CCombineDoc::saveproperties(Bitmap *bm1,CString ext)
{
	int i;
	PropertyItem *bmpprops,property;
	char desc[2000],propbuf[2000];
	desc[0]=0;

	if(bmp_properties)
	{
		bmpprops=(PropertyItem*)bmp_properties;
		for(i=0; i<(int)bmpprop_count; i++)
		{
			switch(bmpprops[i].id)
			{
			case PropertyTagCopyright:
			case PropertyTagDateTime:
			case PropertyTagDocumentName:
			case PropertyTagEquipMake:
			case PropertyTagEquipModel:
			case PropertyTagExifAperture:
			case PropertyTagExifBrightness:
			case PropertyTagExifDTDigitized:
			case PropertyTagExifDTOrig:
			case PropertyTagExifExposureBias:
			case PropertyTagExifExposureIndex:
			case PropertyTagExifExposureProg:
			case PropertyTagExifExposureTime:
			case PropertyTagExifFlash:
			case PropertyTagExifFlashEnergy:
			case PropertyTagExifFNumber:
			case PropertyTagExifFocalLength:
			case PropertyTagExifISOSpeed:
			case PropertyTagExifLightSource:
			case PropertyTagExifMeteringMode:
			case PropertyTagExifShutterSpeed:
			case PropertyTagExifSubjectDist:
			case PropertyTagExifUserComment:
			case PropertyTagImageDescription:
			case PropertyTagOrientation:
			case PropertyTagPixelPerUnitX:
			case PropertyTagPixelPerUnitY:
			case PropertyTagPixelUnit:
			case PropertyTagResolutionUnit:
			case PropertyTagResolutionXLengthUnit:
			case PropertyTagResolutionXUnit:
			case PropertyTagResolutionYLengthUnit:
			case PropertyTagResolutionYUnit:
			case PropertyTagSoftwareUsed:
				bm1->SetPropertyItem(&bmpprops[i]); break;
			}
		}

		if(bm1->GetPropertyItem(PropertyTagImageDescription,i,(PropertyItem*)&propbuf)==Ok)
		{
			strcpy(desc,(char*)((PropertyItem*)propbuf)->value);
			strcat(desc," ");
		}
		property.id=PropertyTagImageDescription;
		property.type=PropertyTagTypeASCII;
		strcat(desc,"Modified by CombineZP");
		property.length=strlen(desc)+1;
		property.value=&desc;
		bm1->SetPropertyItem(&property);

		desc[0]=0;
		if(bm1->GetPropertyItem(PropertyTagSoftwareUsed,i,(PropertyItem*)&propbuf)==Ok)
		{
			strcpy(desc,(char*)((PropertyItem*)propbuf)->value);
			strcat(desc,", ");
		}
		property.id=PropertyTagSoftwareUsed;
		property.type=PropertyTagTypeASCII;
		strcat(desc,"CombineZP");
		property.length=strlen(desc)+1;
		property.value=&desc;
		bm1->SetPropertyItem(&property);
	} else {
		property.id=PropertyTagImageHeight;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&h;
		bm1->SetPropertyItem(&property);

		property.id=PropertyTagImageWidth;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&w;
		bm1->SetPropertyItem(&property);

		property.id=PropertyTagImageDescription;
		property.type=PropertyTagTypeASCII;
		strcat(desc,"Modified by CombineZP");
		property.length=strlen(desc)+1;
		property.value=&desc;
		bm1->SetPropertyItem(&property);
		
		desc[0]=0;
		property.id=PropertyTagSoftwareUsed;
		property.type=PropertyTagTypeASCII;
		strcat(desc,"CombineZP");
		property.length=strlen(desc)+1;
		property.value=&desc;
		bm1->SetPropertyItem(&property);
	}
}

void CCombineDoc::OnFileSave() 
{
	CString s;

	int result;

	str1.LoadString(IDS_STRING339);
	if(depth_flag && (display==PICTURE_BUFFER))
		s.Format(newnumber(str1));
	else
		s.Format(newnumber(buffer[display].name+"_"));

	save_ext=AfxGetApp()->GetProfileString(L"File",L"save_ext",L"jpg");
	jpeg_comp=AfxGetApp()->GetProfileInt(L"File",L"jpeg_comp",70);
	_wchdir(save_path);
	CFileDialog fd(false,save_ext,s,0,L"Default (*."+save_ext+L")|*."+save_ext+L"|*.jpg|*.jpg|*.tif|*.tif|*.bmp 32bpp|*.bmp|*.BMP 24bpp|*.BMP|*.gif|*.gif|*.png|*.png||",0);
	fd.m_ofn.lpstrDefExt=save_ext;
	fd.m_ofn.lpstrInitialDir=save_path;
	if(fd.DoModal()==IDCANCEL) return;
	s=fd.GetPathName();
	s=s.Left(s.ReverseFind('\\'));
	save_path=s;
	save_ext=fd.GetFileExt();
	unsigned long comp;

	unsigned short us[1024];
	int i;

	char *space=(char*)new char[sizeof(EncoderParameters)+1*sizeof(EncoderParameter)];
	EncoderParameters *ep=0;

	BeginWaitCursor();

	str1.LoadString(IDS_STRING340);
	progress.AddString(str1+buffer[display].name);
	str1.LoadString(IDS_STRING341);
	progress.AddString(str1+(CString)fd.m_ofn.lpstrFileTitle);

	Bitmap bm(w,h,4*w,PixelFormat32bppRGB,(unsigned char*)GetBuffer(display));
	CLSID Clsid;
	CString ext(&fd.m_ofn.lpstrFile[fd.m_ofn.nFileExtension]);
	if(ext=="BMP")
	{
		result=GetEncoderClsid(L"image/bmp",&Clsid);
		Bitmap *bm1=bm.Clone(0,0,w,h,PixelFormat24bppRGB);
		i=0; while(fd.m_ofn.lpstrFile[i]){us[i]=((unsigned short)fd.m_ofn.lpstrFile[i])&0xff; i++;} us[i]=0;

		saveproperties(bm1,ext);

		PropertyItem property;
		property.id=PropertyTagImageHeight;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&h;
		bm1->SetPropertyItem(&property);

		property.id=PropertyTagImageWidth;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&w;
		bm1->SetPropertyItem(&property);

		result=bm1->Save(us,&Clsid,ep);
		delete bm1;
	} else {
		ext.MakeLower();
		if(ext=="bmp") result=GetEncoderClsid(L"image/bmp",&Clsid);
		else if(ext=="tif") result=GetEncoderClsid(L"image/tiff",&Clsid);
		else if(ext=="gif") result=GetEncoderClsid(L"image/gif",&Clsid);
		else if(ext=="jpg")
		{
			ep=(EncoderParameters*)space;
			ep->Count=1;
			ep->Parameter[0].Guid=EncoderQuality;
			ep->Parameter[0].Type=EncoderParameterValueTypeLong;
			ep->Parameter[0].NumberOfValues=1;
			ep->Parameter[0].Value=&comp;

			mode=JPEG;
			if(!GetValue()) return;
			mode=0;
			comp=jpeg_comp;
			result=GetEncoderClsid(L"image/jpeg",&Clsid);
		}
		else if(ext==L"png") result=GetEncoderClsid(L"image/png",&Clsid);
		else
		{
			CString s1,s2;
			s1.LoadString(IDS_ERRORFILEEXT);
			MessageBox(progress.m_hWnd,s1,ext,MB_OK);
		}
		i=0; while(fd.m_ofn.lpstrFile[i]){us[i]=((unsigned short)fd.m_ofn.lpstrFile[i])&0xff; i++;} us[i]=0;

		PropertyItem property;
		property.id=PropertyTagImageHeight;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&h;
		bm.SetPropertyItem(&property);

		property.id=PropertyTagImageWidth;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&w;
		bm.SetPropertyItem(&property);

		saveproperties(&bm,ext);

		result=bm.Save(us,&Clsid,ep);
	}
	FreeBuffer(display);
	delete [] space;

	AfxGetApp()->WriteProfileString(L"File",L"save_path",save_path);
	AfxGetApp()->WriteProfileString(L"File",L"save_ext",save_ext);
	AfxGetApp()->WriteProfileInt(L"File",L"jpeg_comp",jpeg_comp);

	EndWaitCursor();
}

void CCombineDoc::OnFileSaverectangle() 
{
	if(!magon)
	{
		mag.bottom=h-1;
		mag.left=0;
		mag.right=w-1;
		mag.top=0;
	}

	CString s;
	s.Format(L"rectangle_%s",buffer[display].name);

//	save_path=AfxGetApp()->GetProfileString(L"File",L"save_path");
	save_ext=AfxGetApp()->GetProfileString(L"File",L"save_ext",L"jpg");
	jpeg_comp=AfxGetApp()->GetProfileInt(L"File",L"jpeg_comp",70);
	_wchdir(save_path);
	str1.LoadString(IDS_STRING342);
	CFileDialog fd(false,save_ext,s,0,str1+save_ext+L")|*."+save_ext+L"|*.jpg|*.jpg|*.tif|*.tif|*.bmp 32bpp|*.bmp|*.BMP 24bpp|*.BMP|*.gif|*.gif|*.png|*.png||",0);
	fd.m_ofn.lpstrDefExt=save_ext;
	fd.m_ofn.lpstrInitialDir=save_path;
	if(fd.DoModal()==IDCANCEL) return;
	s=fd.GetPathName();
	s=s.Left(s.ReverseFind('\\'));
	save_path=s;
	save_ext=fd.GetFileExt();

	str1.LoadString(IDS_STRING343);
	progress.AddString(str1+buffer[display].name);
	str1.LoadString(IDS_STRING344);
	progress.AddString(str1+(CString)fd.m_ofn.lpstrFileTitle);

	unsigned long comp;
	unsigned short us[1024];
	int i;
	char *space=(char*)new char[sizeof(EncoderParameters)+1*sizeof(EncoderParameter)];
	EncoderParameters *ep=0;

	BeginWaitCursor();

	str1.LoadString(IDS_STRING345);
	progress.AddString(str1+buffer[display].name);
	str1.LoadString(IDS_STRING346);
	progress.AddString(str1+(CString)fd.m_ofn.lpstrFileTitle);

	unsigned char *out=(unsigned char*)GetBuffer(display);

	Bitmap bm(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,out+4*(mag.left+w*mag.top));
	CLSID Clsid;
	CString ext(&fd.m_ofn.lpstrFile[fd.m_ofn.nFileExtension]);
	if(ext=="BMP")
	{
		GetEncoderClsid(L"image/bmp",&Clsid);
		Bitmap *bm1=bm.Clone(0,0,mag.right-mag.left,mag.bottom-mag.top,PixelFormat24bppRGB);

		saveproperties(bm1,ext);

		PropertyItem property;
		int w=mag.Width(),h=mag.Height();
		property.id=PropertyTagImageHeight;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&h;
		bm1->SetPropertyItem(&property);

		property.id=PropertyTagImageWidth;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&w;
		bm1->SetPropertyItem(&property);

		i=0; while(fd.m_ofn.lpstrFile[i]){us[i]=((unsigned short)fd.m_ofn.lpstrFile[i])&0xff; i++;} us[i]=0;
		bm1->Save(us,&Clsid,ep);
		delete bm1;
	} else {
		ext.MakeLower();
		if(ext=="bmp") GetEncoderClsid(L"image/bmp",&Clsid);
		else if(ext=="tif") GetEncoderClsid(L"image/tiff",&Clsid);
		else if(ext=="gif") GetEncoderClsid(L"image/gif",&Clsid);
		else if(ext=="jpg")
		{
			ep=(EncoderParameters*)space;
			ep->Count=1;
			ep->Parameter[0].Guid=EncoderQuality;
			ep->Parameter[0].Type=EncoderParameterValueTypeLong;
			ep->Parameter[0].NumberOfValues=1;
			ep->Parameter[0].Value=&comp;

			mode=JPEG;
			if(!GetValue()) return;
			mode=0;
			comp=jpeg_comp;
			GetEncoderClsid(L"image/jpeg",&Clsid);
		}
		else if(ext=="png") GetEncoderClsid(L"image/png",&Clsid);
		else
		{
			CString s1,s2;
			s1.LoadString(IDS_ERRORFILEEXT);
			MessageBox(progress.m_hWnd,s1,ext,MB_OK);
		}

		PropertyItem property;
		int w=mag.Width(),h=mag.Height();
		property.id=PropertyTagImageHeight;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&h;
		bm.SetPropertyItem(&property);

		property.id=PropertyTagImageWidth;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&w;
		bm.SetPropertyItem(&property);

		saveproperties(&bm,ext);

		i=0; while(fd.m_ofn.lpstrFile[i]){us[i]=((unsigned short)fd.m_ofn.lpstrFile[i])&0xff; i++;} us[i]=0;
		bm.Save(us,&Clsid,ep);
	}
	FreeBuffer(display);

	AfxGetApp()->WriteProfileString(L"File",L"save_path",save_path);
	AfxGetApp()->WriteProfileString(L"File",L"save_ext",save_ext);
	AfxGetApp()->WriteProfileInt(L"File",L"jpeg_comp",jpeg_comp);

	delete [] space;
	EndWaitCursor();
}

void CCombineDoc::OnFileExportrectangles() 
{
	int working;
	CFileFind file;
	CString p,fullp,ftitle;
	CFile f;

	BeginWaitCursor();

//	save_path=AfxGetApp()->GetProfileString(L"File",L"save_path");
	save_ext=AfxGetApp()->GetProfileString(L"File",L"save_ext",L"jpg");
	jpeg_comp=AfxGetApp()->GetProfileInt(L"File",L"jpeg_comp",70);

	_wchdir(save_path);
	p=path;
	str1.LoadString(IDS_STRING347);
	p+=str1;
	if(_wchdir(p))
	{
		_wmkdir(p);
		_wchdir(p);
	}
	str1.LoadString(IDS_STRING348);
	CString str2; str2.LoadString(IDS_STRING349);
	CFileDialog fd(false,save_ext,str1,0,str2+save_ext+L")|*."+save_ext+L"|*.jpg|*.jpg|*.tif|*.tif|*.bmp 32bpp|*.bmp|*.BMP 24bpp|*.BMP|*.gif|*.gif|*.png|*.png||",0);
	fd.m_ofn.lpstrDefExt=L"tif";
	fd.m_ofn.lpstrInitialDir=p.GetBuffer(1024);
	if(fd.DoModal()==IDCANCEL)
	{
		EndWaitCursor();
		return;
	}
	p.ReleaseBuffer();
	p=fd.GetPathName();
	p=p.Left(p.GetLength()-4);
	ftitle=fd.GetFileTitle();

	try
	{
		working=file.FindFile(p+"*.*");
		while(working)
		{
			working=file.FindNextFile();
			if(file.IsDirectory()) continue;
			CFile::Remove(file.GetFilePath());
		}
	}
	catch(...){;}
	file.Close();

	progress.Maximize();
	str1.LoadString(IDS_STRING350);
	progress.AddString(str1);
	progress.AddString("..."+p.Right(50));

	unsigned char *f0;

	if(!magon)
	{
		mag.bottom=h-1;
		mag.left=0;
		mag.right=w-1;
		mag.top=0;
	}

	unsigned long comp;
	unsigned short us[1024];
	int i;
	char *space=(char*)new char[sizeof(EncoderParameters)+1*sizeof(EncoderParameter)];
	EncoderParameters *ep=0;

	CLSID Clsid;
	CString ext(&fd.m_ofn.lpstrFile[fd.m_ofn.nFileExtension]);
	if(ext=="BMP")
	{
		GetEncoderClsid(L"image/bmp",&Clsid);

		for(int frame=0; frame<top_frame; frame++)
		{
			f0=(unsigned char*)GetBuffer(frame);
			Bitmap *bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
			Bitmap *bm1=bm->Clone(0,0,mag.right-mag.left,mag.bottom-mag.top,PixelFormat24bppRGB);

			fullp.Format(L"%s%05d-%s.%s",p,top_frame-frame,buffer[frame].name,ext);

			i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
			bm1->Save(us,&Clsid,ep);

			fullp.Format(L"%05d-%s.%s",top_frame-frame,buffer[frame].name,ext);
			progress.AddString("   "+ftitle+fullp);

			FreeBuffer(frame);
			delete bm1;
			delete bm;
		}
		f0=(unsigned char*)GetBuffer(PICTURE_BUFFER);
		Bitmap *bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
		Bitmap *bm1=bm->Clone(0,0,mag.right-mag.left,mag.bottom-mag.top,PixelFormat24bppRGB);
		str1.LoadString(IDS_STRING350);
		fullp.Format(str1,p,ext);
		i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
		bm1->Save(us,&Clsid,ep);
		str1.LoadString(IDS_STRING351);
		fullp.Format(str1,ext);
		progress.AddString("   "+ftitle+fullp);
		delete bm;
		delete bm1;
		FreeBuffer(PICTURE_BUFFER);

		f0=(unsigned char*)GetBuffer(OUT_BUFFER);
		bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
		bm1=bm->Clone(0,0,mag.right-mag.left,mag.bottom-mag.top,PixelFormat24bppRGB);
		str1.LoadString(IDS_STRING352);
		fullp.Format(str1,p,ext);
		i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
		bm1->Save(us,&Clsid,ep);
		str1.LoadString(IDS_STRING201);
		fullp.Format(str1,ext);
		progress.AddString("   "+ftitle+fullp);
		delete bm;
		delete bm1;
		FreeBuffer(OUT_BUFFER);
	} else {
		ext.MakeLower();
		if(ext=="bmp") GetEncoderClsid(L"image/bmp",&Clsid);
		else if(ext=="tif") GetEncoderClsid(L"image/tiff",&Clsid);
		else if(ext=="gif") GetEncoderClsid(L"image/gif",&Clsid);
		else if(ext=="jpg")
		{
			ep=(EncoderParameters*)space;
			ep->Count=1;
			ep->Parameter[0].Guid=EncoderQuality;
			ep->Parameter[0].Type=EncoderParameterValueTypeLong;
			ep->Parameter[0].NumberOfValues=1;
			ep->Parameter[0].Value=&comp;

			mode=JPEG;
			if(!GetValue()) return;
			mode=0;
			comp=jpeg_comp;
			GetEncoderClsid(L"image/jpeg",&Clsid);
		}
		else if(ext=="png") GetEncoderClsid(L"image/png",&Clsid);
		else
		{
			CString s1,s2;
			s1.LoadString(IDS_ERRORFILEEXT);
			MessageBox(progress.m_hWnd,s1,ext,MB_OK);
		}
		for(int frame=0; frame<top_frame; frame++)
		{
			f0=(unsigned char*)GetBuffer(frame);
			Bitmap *bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));

			fullp.Format(L"%s%05d-%s.%s",p,top_frame-frame,buffer[frame].name,ext);

			i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
			bm->Save(us,&Clsid,ep);

			fullp.Format(L"%05d-%s.%s",top_frame-frame,buffer[frame].name,ext);
			progress.AddString("   "+ftitle+fullp);

			FreeBuffer(frame);
		}
		f0=(unsigned char*)GetBuffer(PICTURE_BUFFER);
		Bitmap *bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
		str1.LoadString(IDS_STRING354);
		fullp.Format(str1,p,ext);
		i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
		bm->Save(us,&Clsid,ep);
		str1.LoadString(IDS_STRING355);
		fullp.Format(str1,ext);
		progress.AddString("   "+ftitle+fullp);
		delete bm;
		FreeBuffer(PICTURE_BUFFER);

		f0=(unsigned char*)GetBuffer(OUT_BUFFER);
		bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
		str1.LoadString(IDS_STRING356);
		fullp.Format(str1,p,ext);
		i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
		bm->Save(us,&Clsid,ep);
		str1.LoadString(IDS_STRING357);
		fullp.Format(str1,ext);
		progress.AddString("   "+ftitle+fullp);
		delete bm;
		FreeBuffer(OUT_BUFFER);
	}

	progress.Minimize();
	delete [] space;
	EndWaitCursor();
}

void CCombineDoc::silent_export_rectangles()
{
	int working;
	CFileFind file;
	CString p,fullp,ftitle;
	CFile f;

	p=path;
	str1.LoadString(IDS_STRING358);
	p+=str1;
	if(_wchdir(p))
	{
		_wmkdir(p);
		_wchdir(p);
	}

	p+="*.*";
	try
	{
		working=file.FindFile(p);
		while(working)
		{
			working=file.FindNextFile();
			if(file.IsDirectory()) continue;
			CFile::Remove(file.GetFilePath());
		}
	}
	catch(...){;}
	file.Close();

	str1.LoadString(IDS_STRING359);
	progress.AddString(str1);
	progress.AddString("..."+p.Right(50));

	unsigned char *f0;

	unsigned short us[1024];
	int i;
	char *space=(char*)new char[sizeof(EncoderParameters)+1*sizeof(EncoderParameter)];
	EncoderParameters *ep=0;

	CLSID Clsid;
	CString ext("BMP");
	GetEncoderClsid(L"image/bmp",&Clsid);

	for(int frame=0; frame<top_frame; frame++)
	{
		f0=(unsigned char*)GetBuffer(frame);
		Bitmap *bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
		Bitmap *bm1=bm->Clone(0,0,mag.right-mag.left,mag.bottom-mag.top,PixelFormat24bppRGB);

		str1.LoadString(IDS_STRING360);
		fullp.Format(str1,path,top_frame-frame,buffer[frame].name,ext);

		i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
		bm1->Save(us,&Clsid,ep);

		fullp.Format(L"%05d-%s.%s",top_frame-frame,buffer[frame].name,ext);
		progress.AddString("   "+ftitle+fullp);

		delete bm;
		delete bm1;
		FreeBuffer(frame);
	}

	f0=(unsigned char*)GetBuffer(PICTURE_BUFFER);
	Bitmap *bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
	Bitmap *bm1=bm->Clone(0,0,mag.right-mag.left,mag.bottom-mag.top,PixelFormat24bppRGB);
	str1.LoadString(IDS_STRING361);
	fullp.Format(str1,path,ext);
	i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
	bm1->Save(us,&Clsid,ep);
	str1.LoadString(IDS_STRING362);
	fullp.Format(str1,ext);
	progress.AddString("   "+ftitle+fullp);
	delete bm;
	delete bm1;
	FreeBuffer(PICTURE_BUFFER);

	f0=(unsigned char*)GetBuffer(OUT_BUFFER);
	bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
	bm1=bm->Clone(0,0,mag.right-mag.left,mag.bottom-mag.top,PixelFormat24bppRGB);
	str1.LoadString(IDS_STRING363);
	fullp.Format(str1,path,ext);
	i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
	bm1->Save(us,&Clsid,ep);
	str1.LoadString(IDS_STRING364);
	fullp.Format(str1,ext);
	progress.AddString("   "+ftitle+fullp);
	delete bm;
	delete bm1;
	FreeBuffer(OUT_BUFFER);
	delete [] space;
}

void CCombineDoc::OnFileImportdepthmap()
{
	int pi=0,po,l,d;
	int startpi;

	str1.LoadString(IDS_STRING365);
	CFileDialog fd(true,L"bmp",0,0,str1,0);
	fd.m_ofn.lpstrInitialDir=path;
	if(fd.DoModal()==IDCANCEL) return;

	backup_picture_valid=false;

	BeginWaitCursor();

	progress.AddString(IDS_STRING366);

	CFile f(fd.m_ofn.lpstrFile,CFile::modeRead);
	progress.AddString("   "+(CString)fd.m_ofn.lpstrFileTitle);

	BITMAPFILEHEADER dmbmfh;
	BITMAPINFOHEADER dmbmih;
	f.Read((void*)&dmbmfh,sizeof(dmbmfh));
	f.Read((void*)&dmbmih,sizeof(dmbmih));
	if((int)h!=abs(dmbmih.biHeight) || (int)w!=abs(dmbmih.biWidth))
	{
		CString s1,s2;
		s1.LoadString(IDS_ERRORDMWH); s2.LoadString(IDS_IMPORTDM);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		f.Close();
		EndWaitCursor();
		progress.AddString(IDS_STRING367);
		return;
	}

	int bc=dmbmih.biBitCount/8;
	if(bc!=3)
	{
		CString s1,s2;
		s1.LoadString(IDS_ERROR24BIT); s2.LoadString(IDS_IMPORTDM);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		f.Close();
		EndWaitCursor();
		progress.AddString(IDS_STRING367);
		return;
	}

	bw=(w*bc+3)/4; bw*=4;
	if(dmbmih.biSizeImage!=0) bw=dmbmih.biSizeImage/h;

	unsigned char *t=(unsigned char*)GetBuffer(TEMP_BUFFER);
	int *depth=(int*)GetBuffer(DEPTH_BUFFER);

	f.Seek(dmbmfh.bfOffBits,CFile::begin);
	f.Read(t,bw*h);
	f.Close();

	int old_top_frame=(int)t[bw*(h-1)];
	if(old_top_frame>top_frame)
	{
		CString s1,s2;
		s1.LoadString(IDS_ERRORFRAMESDM); s2.LoadString(IDS_IMPORTDM);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		EndWaitCursor();
		progress.AddString(IDS_STRING369);
		return;
	}

	int button;
	CImportDM dlg;
	button=(int)dlg.DoModal();
	if(button==IDCANCEL) goto finish;

	if(button==IDC_REDDATA)
	{
		progress.AddString(IDS_STRING201);
		for(l=h-1; l>=0; l--)
		{
			startpi=pi;
			for(po=0; po<(int)w; po++)
			{
				if(t[pi+0]==0)
				{
					depth[po+l*w]=fEMPTY;
					pi+=bc;
					continue;
				}
				d=(int)t[pi+2]; pi+=bc;
				if(d<old_top_frame) depth[po+l*w]=(d<<8)|fFILLED;
				else depth[po+l*w]=fEMPTY;
			}
			pi=startpi+bw;
		}
	} else {
		progress.AddString(IDS_STRING201);
		for(l=h-1; l>=0; l--)
		{
			startpi=pi;
			for(po=0; po<(int)w; po++)
			{
				d=(int)t[pi+1]; pi+=bc;
				d=d*old_top_frame-1;
				if(d>=0) depth[po+l*w]=d|fFILLED;
				else depth[po+l*w]=fEMPTY;
			}
			pi=startpi+bw;
		}
	}

finish:
	FreeBuffer(TEMP_BUFFER);
	UpdateBuffer(DEPTH_BUFFER);

	redraw_picture();
	display=PICTURE_BUFFER;
	RefreshScreen();
	EndWaitCursor();
}

void CCombineDoc::OnFileExportdepthmap() 
{
	bool saveflag=depth_flag;
	if(!depth_flag)	OnViewToggledepthmap();
	display=PICTURE_BUFFER;
	char *c=(char*)GetBuffer(PICTURE_BUFFER);
	c[0]=(unsigned char)top_frame;
	UpdateBuffer(PICTURE_BUFFER);

	CString s;

	str1.LoadString(IDS_STRING371);
	s.Format(newnumber(str1));

	_wchdir(path);
	str1.LoadString(IDS_STRING372);
	CFileDialog fd(false,L"bmp",s,0,str1,0);
	fd.m_ofn.lpstrDefExt=L"bmp";
	fd.m_ofn.lpstrInitialDir=path;
	if(fd.DoModal()==IDCANCEL) return;

	BeginWaitCursor();

	str1.LoadString(IDS_STRING373);
	progress.AddString(str1+buffer[display].name);
	str1.LoadString(IDS_STRING374);
	progress.AddString(str1+(CString)fd.m_ofn.lpstrFileTitle);

	CFile f(fd.m_ofn.lpstrFile,CFile::modeCreate | CFile::modeWrite);

	int bwid=(3*w+3)/4; bwid*=4;
	bmih.biHeight=h*(bmih.biHeight/abs(bmih.biHeight));
	bmih.biWidth=w;
	bmih.biSizeImage=abs(bmih.biHeight)*bwid;
	bmfh.bfSize=sizeof(BITMAPINFOHEADER)+sizeof(BITMAPFILEHEADER)+bmih.biSizeImage;

	f.Write(&bmfh,sizeof(bmfh));
	f.Write(&bmih,sizeof(bmih));

	PIXEL *f0=(PIXEL*)GetBuffer(display);
	unsigned char *t=(unsigned char*)GetBuffer(TEMP_BUFFER);

	int pi,po=0,l,startpo;
	int L;
	PIXEL p;
	for(l=h-1; l>=0; l--)
	{
		startpo=po;
		L=l*(int)w;
		for(pi=0; pi<w; pi++)
		{
			if(f0[pi+l*w].abgr==wipeout)
			{
				t[po]=0; po++;
				t[po]=0; po++;
				t[po]=0; po++;
			} else {
				p=f0[pi+L];
				t[po]=p.b; po++;
				t[po]=p.g; po++;
				t[po]=p.r; po++;
			}
		}
		po=startpo+bwid;
	}

	const int chunk_size=1000000;
	int write_size;
	int write_start=0;
	int left=bmih.biSizeImage;
	while(left>0)
	{
		if(left<chunk_size) write_size=left;
		else write_size=chunk_size;
		f.Write((void*)(&((char*)t)[write_start]),write_size);
		left-=write_size;
		write_start+=write_size;
	}
//	f.Write(t,bmih.biSizeImage);

	f.Close();

	FreeBuffer(display);
	FreeBuffer(TEMP_BUFFER);

	EndWaitCursor();

	if(!saveflag) OnViewToggledepthmap();
}

void CCombineDoc::OnFileImport() 
{
	index--;
	AfxGetApp()->WriteProfileInt(L"File",L"Index",index);

	int height,width;

	str1.LoadString(IDS_STRING375);
	CFileDialog fd(true,L"bmp",0,0,str1,0);
	fd.m_ofn.lpstrInitialDir=path;
	fd.m_ofn.Flags|=OFN_ENABLESIZING;
	if(fd.DoModal()==IDCANCEL) return;

	progress.AddString(IDS_STRING376);

	BeginWaitCursor();

	const wchar_t *p=(const wchar_t *)fd.m_ofn.lpstrFile;
	using namespace Gdiplus;

	Bitmap *i;
	i=new Bitmap(p);

	height=i->GetHeight();
	width=i->GetWidth();

	unsigned char* b;
	BitmapData bd;
	Rect rect(0,0,width,height);
	i->LockBits(&rect,1,PixelFormat32bppRGB,&bd);
	b=(unsigned char*)bd.Scan0;
	bw=bd.Stride;

	int pi=0,po,l;

	PIXEL *f1=(PIXEL*)GetBuffer(top_frame);
	for(l=0; l<w*h; l++){f1[l].abgr=0; f1[l].a=aCLEAR;}

	int startpi;
	for(l=0; (l<(int)height) && (l<(int)h); l++)
	{
		startpi=pi;
		for(po=0; (po<width) && (po<(int)w); po++)
		{
			f1[po+l*w].b=(unsigned char)b[pi]; pi++;
			f1[po+l*w].g=(unsigned char)b[pi]; pi++;
			f1[po+l*w].r=(unsigned char)b[pi]; pi++;
			f1[po+l*w].a=aGOOD;  pi++;
		}
		pi=startpi+bw;
	}
	i->UnlockBits(&bd);

	buffer[top_frame].name.Format(L"%03d_%s",index,fd.m_ofn.lpstrFileTitle);
	buffer[top_frame].hidden=true;

	progress.AddString("   "+buffer[top_frame].name);

	display=active_frame=top_frame;
	top_frame;

	delete i;

	if(top_frame==MAX_FRAMES)
	{
		CString s1,s2;
		s1.LoadString(IDS_FILELIMITREACHED); s2.LoadString(IDS_FILELIMIT);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
	}

	UpdateBuffer(top_frame);
	top_frame++;
	EndWaitCursor();
	Redraw(0,true);
}

void CCombineDoc::Remove()
{
	OnFileRemove();
}

void CCombineDoc::OnFileRemove() 
{
	if(display==OUT_BUFFER) return;
	if(display==PICTURE_BUFFER) return;

	if(!macro_mode)
	{
		CString s1,s2;
		s1.LoadString(IDS_REMOVEFR); s2.LoadString(IDS_REMOVEFR);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		if(MessageBox(progress.m_hWnd,s1+buffer[display].name,s2,MB_OKCANCEL)==IDCANCEL)
			return;
	}

	remove();

	save_state();
	RefreshScreen();
}

void CCombineDoc::remove()
{
	if(display==OUT_BUFFER) return;
	if(display==PICTURE_BUFFER) return;
	if(top_frame==1) return;

	progress.AddString(IDS_STRING377);
	progress.AddString("   "+buffer[display].name);

	int i,x,y,Y;

	if(display==active_frame2) active_frame2=UNSET;

	RemoveBuffer(display);

	for(i=0; i<(int)max_slots; i++)
	{
		if((memory_slot[i].buffer>display) && (memory_slot[i].buffer<MAX_FRAMES))
			memory_slot[i].buffer--;
	}

	if(display!=top_frame-1)
	{
		for(i=display; i<top_frame-1; i++)
		{
			buffer[i]=buffer[i+1];
		}

		int b=top_frame-1;
		buffer[b].ondisk=false;
		buffer[b].inuse=0;
		buffer[b].name="*** Not Initialized ***";
		buffer[b].pointer=0;
		buffer[b].slot=-1;
		buffer[b].updated=false;
	}

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	for(y=0; y<h; y++)
	{
		Y=y*w;
		for(x=0; x<w; x++)
		{
			i=x+Y;
			if(((depth[i]&dMASK)>>8)==display){depth[i]=fEMPTY; continue;}
			if(((depth[i]&dMASK)>>8)>display) depth[i]-=0x100;
		}
	}
	UpdateBuffer(DEPTH_BUFFER);

	for(i=0; i<4; i++)
	{
		if(Sizer.p[i].f==display) Sizer.p[i].valid=false;
		if(Sizer.p[i].f>display) Sizer.p[i].f--;
	}

	top_frame--;

	refresh_used_list();

	if(display<0) display=0;
	if(display>top_frame-1) display=active_frame=top_frame-1;

	backup_picture_valid=false;
	redraw_picture();
}

bool CCombineDoc::reload()
{
	int f=active_frame;

	void* f1;

	CFile file;

	CString s;

	str1.LoadString(IDS_STRING612);
	if(file.Open(work_path+L"\\"+str1+buffer[f].name+L".czp",CFile::modeRead))
	{
		s.Format(L"   %s",buffer[f].name);
		progress.AddString(s);
		f1=GetBuffer(f);
		readpfile(&file,f1);
		file.Close();
		UpdateBuffer(f);
		return true;
	} else {
		str1.LoadString(IDS_STRING378);
		s.Format(str1,buffer[f].name);
		progress.AddString(s);
		CString s1,s2;
		s1.LoadString(IDS_ERRORRELOAD); s2.LoadString(IDS_RELOAD);
		s.Format(s1,buffer[f].name);
		MessageBox(progress.m_hWnd,s,s2,MB_OK);
		file.Close();
		return false;
	}
}

void CCombineDoc::OnFileReload() 
{
	BeginWaitCursor();
	progress.AddString(IDS_STRING379);
	bool ret=reload();
	RefreshScreen();
	EndWaitCursor();
}

bool CCombineDoc::Reload()
{
	BeginWaitCursor();
	progress.AddString(IDS_STRING379);
	bool ret=reload();
	RefreshScreen();
	EndWaitCursor();
	return ret;
}

void CCombineDoc::OnFileReloadall()
{
	backup_picture_valid=false;
	int d;

	BeginWaitCursor();
	progress.Maximize();
	progress.AddString(IDS_RELOADALL);
	for(d=0; d<top_frame; d++)
	{
		if(buffer[d].hidden) continue;
		display=active_frame=d;
		reload();
	}
	backup_picture_valid=false;
	redraw_picture();
	progress.Minimize();
	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::savedepth()
{
	BeginWaitCursor();
	unsigned char *depth=(unsigned char*)GetBuffer(DEPTH_BUFFER);

	unsigned char next_byte,this_byte;
	int out=0,i,count;

	count=1;
	next_byte=depth[0];
	for(i=4; i<4*w*h; i+=4)
	{
		this_byte=next_byte;
		next_byte=depth[i];
		if((this_byte==next_byte) && (count<255))
		{
			count++;
		} else {
			packed[out++]=this_byte;
			packed[out++]=(unsigned char)count;
			count=1;
		}
	}
	packed[out++]=this_byte;
	packed[out++]=(unsigned char)count;

	count=1;
	next_byte=depth[1];
	for(i=5; i<4*w*h; i+=4)
	{
		this_byte=next_byte;
		next_byte=depth[i];
		if((this_byte==next_byte) && (count<255))
		{
			count++;
		} else {
			packed[out++]=this_byte;
			packed[out++]=(unsigned char)count;
			count=1;
		}
	}
	packed[out++]=this_byte;
	packed[out++]=(unsigned char)count;

	count=1;
	next_byte=depth[2];
	for(i=6; i<4*w*h; i+=4)
	{
		this_byte=next_byte;
		next_byte=depth[i];
		if((this_byte==next_byte) && (count<255))
		{
			count++;
		} else {
			packed[out++]=this_byte;
			packed[out++]=(unsigned char)count;
			count=1;
		}
	}
	packed[out++]=this_byte;
	packed[out++]=(unsigned char)count;

	count=1;
	next_byte=depth[3];
	for(i=7; i<4*w*h; i+=4)
	{
		this_byte=next_byte;
		next_byte=depth[i];
		if((this_byte==next_byte) && count<255)
		{
			count++;
		} else {
			packed[out++]=this_byte;
			packed[out++]=(unsigned char)count;
			count=1;
		}
	}
	packed[out++]=this_byte;
	packed[out++]=(unsigned char)count;

	CString s;
	str1.LoadString(IDS_STRING380);
	s.Format(str1,path);
	CFile f(s,CFile::modeCreate | CFile::modeWrite);

	const int chunk_size=1000000;
	int write_size;
	int write_start=0;
	int left=out;
	while(left>0)
	{
		if(left<chunk_size) write_size=left;
		else write_size=chunk_size;
		f.Write((void*)(&((char*)packed)[write_start]),write_size);
		left-=write_size;
		write_start+=write_size;
	}
//	f.Write(packed,out);
	f.Close();

	FreeBuffer(TEMP_BUFFER+4);
	FreeBuffer(DEPTH_BUFFER);
	EndWaitCursor();
}

void CCombineDoc::OnFileWorkframesReloaddepth() 
{
	loaddepth();
	progress.AddString(IDS_STRING381);
	backup_picture_valid=false;
	redraw_picture();
	RefreshScreen();
}

void CCombineDoc::OnFileWorkframesSavedepth() 
{
	progress.AddString(IDS_STRING383);
	savedepth();
}

void CCombineDoc::loaddepth()
{
	BeginWaitCursor();
	unsigned char *depth=(unsigned char*)GetBuffer(DEPTH_BUFFER);

	CString s;
	str1.LoadString(IDS_STRING383);
	s.Format(str1,work_path);
	CFile f(s,CFile::modeRead);
	f.Read(packed,(unsigned int)f.GetLength());
	f.Close();

	int in=0,i,count;
	unsigned char this_byte;

	for(i=0; i<4*w*h;)
	{
		this_byte=packed[in++];
		count=packed[in++];
		while(count!=0)
		{
			depth[i]=this_byte;
			count--;
			i+=4;
		}
	}

	for(i=1; i<4*w*h;)
	{
		this_byte=packed[in++];
		count=packed[in++];
		while(count!=0)
		{
			depth[i]=this_byte;
			count--;
			i+=4;
		}
	}

	for(i=2; i<4*w*h;)
	{
		this_byte=packed[in++];
		count=packed[in++];
		while(count!=0)
		{
			depth[i]=this_byte;
			count--;
			i+=4;
		}
	}

	for(i=3; i<4*w*h;)
	{
		this_byte=packed[in++];
		count=packed[in++];
		while(count!=0)
		{
			depth[i]=this_byte;
			count--;
			i+=4;
		}
	}

	UpdateBuffer(DEPTH_BUFFER);
	EndWaitCursor();
}

struct rpf_Params
{
	unsigned int in,out,wh;
	unsigned char *opacked;
	unsigned char *bytes;
} rpf_params[4];

UINT CCombineDoc::rpf(LPVOID param)
{
	rpf_Params *pp=(rpf_Params *)param;
	unsigned int in=pp->in;
	unsigned int out=pp->out;
	unsigned int wh=pp->wh;
	unsigned char *opacked=pp->opacked;
	unsigned char *bytes=pp->bytes;

	unsigned int i,count,a;

	while(out<wh)
	{
		count=opacked[in++];
		if(count==0)
		{
			count=opacked[in++];
			for(i=0; i<=count; i++)
			{
				bytes[out]=opacked[in++];
				out+=4;
			}
		} else {
			a=opacked[in++];
			for(i=0; i<=count; i++)
			{
				bytes[out]=(unsigned char)a;
				out+=4;
			}
		}
	}

	return 0;
}

void CCombineDoc::readpfile(CFile *f, void *buffer)
{
//	unsigned int wh=4*(w+4)*(h+4);
	unsigned int wh=slot_size;

	f->SeekToBegin();
	f->Read(packed,(unsigned int)f->GetLength());

	unsigned char *bytes=(unsigned char*)buffer;
	unsigned char *opacked=(unsigned char*)(packed+16);
	unsigned int *intso=(unsigned int*)packed;
	unsigned int *ints=(unsigned int*)buffer;
	unsigned int in,out;
	unsigned int i;

	unsigned int obj,objects;
	CWinThread *th[4];
	HANDLE hand[4];
	objects=pam;
	if(objects==0) objects=1;
	if(wh<4000) objects=1;

	if(objects>=4)
	{
		for(obj=0; obj<4; obj++)
		{
			in=intso[obj];
			out=obj;
			rpf_params[obj].in=in;
			rpf_params[obj].out=out;
			rpf_params[obj].wh=wh;
			rpf_params[obj].opacked=opacked;
			rpf_params[obj].bytes=bytes;
			StartThreads(rpf,rpf_params)
		}
		EndThreads
	}
	else if(objects>=2)
	{
		for(obj=0; obj<2; obj++)
		{
			in=intso[obj];
			out=obj;
			rpf_params[obj].in=in;
			rpf_params[obj].out=out;
			rpf_params[obj].wh=wh;
			rpf_params[obj].opacked=opacked;
			rpf_params[obj].bytes=bytes;
			StartThreads(rpf,rpf_params)
		}
		EndThreads

		for(obj=0; obj<2; obj++)
		{
			in=intso[obj+2];
			out=obj+2;
			rpf_params[obj].in=in;
			rpf_params[obj].out=out;
			rpf_params[obj].wh=wh;
			rpf_params[obj].opacked=opacked;
			rpf_params[obj].bytes=bytes;
			StartThreads(rpf,rpf_params)
		}
		EndThreads
	}
	else
	{
		for(obj=0; obj<4; obj++)
		{
			in=intso[obj];
			out=obj;
			rpf_params[obj].in=in;
			rpf_params[obj].out=out;
			rpf_params[obj].wh=wh;
			rpf_params[obj].opacked=opacked;
			rpf_params[obj].bytes=bytes;
			rpf(&rpf_params[obj]);
		}
	}

	for(i=0; i<wh/4; i++) ints[i]-=0x80000000;
}

struct wpf_Params
{
	unsigned int index;
	unsigned int out,wh;
	unsigned char *p;
	unsigned char *bytes;
} wpf_params[4];

UINT CCombineDoc::wpf(LPVOID param)
{
	wpf_Params *pp=(wpf_Params *)param;
	unsigned int index=pp->index;
	unsigned int out=pp->out;
	unsigned int wh=pp->wh;
	unsigned char *p=pp->p;
	unsigned char *bytes=pp->bytes;

	unsigned int in;
	const unsigned int minsingles=8;
	unsigned char this_byte,next_byte;
	unsigned int i,count,singlecount;
	bool singletons; // start with 0 then count

	count=0;
	next_byte=bytes[index];
	singletons=false;
	for(in=4+index; in<wh; in+=4)
	{
		this_byte=next_byte;
		next_byte=bytes[in];
		if((next_byte==this_byte) && (count<255))
		{
			count++;
		} else {
			if(count==0)
			{
				if(singletons)
				{
					if(p[singlecount]<255)
					{
						p[singlecount]++;
						p[out++]=this_byte;
					} else {
						p[out++]=0;
						singlecount=out;
						p[out++]=0;
						p[out++]=this_byte;
					}
				} else {
					singletons=true;
					p[out++]=0;
					singlecount=out;
					p[out++]=0;
					p[out++]=this_byte;
					if(in+minsingles*4<wh-4)
					{
						for(i=in; i<in+minsingles*4; i+=4)
							p[out++]=bytes[i];
						p[singlecount]=minsingles;
						next_byte=bytes[i];
						in=i;
					}
				}
			} else {
				singletons=false;
				p[out++]=(unsigned char)count;
				p[out++]=this_byte;
			}
			count=0;
		}
	}
	p[out++]=(unsigned char)count;
	if(count==0) p[out++]=(unsigned char)count;
	p[out++]=this_byte;

	pp->out=out;

	return 0;
}

void CCombineDoc::writepfile(CFile *f, void *buffer)
{
	unsigned int wh=slot_size;

	unsigned char *p[4];
	p[0]=packed;
	p[1]=packed+(slot_size+slot_size/4)/4;
	p[2]=packed+(slot_size+slot_size/4)/2;
	p[3]=packed+3*(slot_size+slot_size/4)/4;
	unsigned int out[4];

	const unsigned int minsingles=8;
	unsigned char *bytes=(unsigned char*)buffer;
	unsigned int *ints=(unsigned int*)buffer;
	unsigned int i;

	for(i=0; i<wh/4; i++) ints[i]+=0x80000000;

	unsigned int obj,objects;
	CWinThread *th[4];
	HANDLE hand[4];
	objects=pam;
	if(objects==0) objects=1;
	if(wh<4000) objects=1;

	if(objects>=4)
	{
		for(obj=0; obj<4; obj++)
		{
			wpf_params[obj].index=obj;
			wpf_params[obj].out=0;
			wpf_params[obj].wh=wh;
			wpf_params[obj].p=p[obj];
			wpf_params[obj].bytes=bytes;
			StartThreads(wpf,wpf_params)
		}
		EndThreads
		for(obj=0; obj<4; obj++)
			out[obj]=wpf_params[obj].out;
	}
	else if(objects>=2)
	{
		for(obj=0; obj<2; obj++)
		{
			wpf_params[obj].index=obj;
			wpf_params[obj].out=0;
			wpf_params[obj].wh=wh;
			wpf_params[obj].p=p[obj];
			wpf_params[obj].bytes=bytes;
			StartThreads(wpf,wpf_params)
		}
		EndThreads
		for(obj=0; obj<2; obj++)
			out[obj]=wpf_params[obj].out;

		for(obj=0; obj<2; obj++)
		{
			wpf_params[obj].index=obj+2;
			wpf_params[obj].out=0;
			wpf_params[obj].wh=wh;
			wpf_params[obj].p=p[obj+2];
			wpf_params[obj].bytes=bytes;
			StartThreads(wpf,wpf_params)
		}
		EndThreads
		for(obj=0; obj<2; obj++)
			out[obj+2]=wpf_params[obj].out;
	}
	else
	{
		for(obj=0; obj<4; obj++)
		{
			wpf_params[obj].index=obj;
			wpf_params[obj].out=0;
			wpf_params[obj].wh=wh;
			wpf_params[obj].p=p[obj];
			wpf_params[obj].bytes=bytes;
			wpf(&wpf_params[obj]);
		}
		for(obj=0; obj<4; obj++)
			out[obj]=wpf_params[obj].out;
	}

	i=0; f->Write(&i,4);
	i+=out[0]; f->Write(&i,4);
	i+=out[1]; f->Write(&i,4);
	i+=out[2]; f->Write(&i,4);

	const int chunk_size=(1024*1024*16);
	int write_size;
	int write_start;
	int left;

	left=out[0];
	write_start=0;
	while(left>0)
	{
		if(left<chunk_size) write_size=left;
		else write_size=chunk_size;
		f->Write((void*)(&p[0][write_start]),write_size);
		left-=write_size;
		write_start+=write_size;
	}

	left=out[1];
	write_start=0;
	while(left>0)
	{
		if(left<chunk_size) write_size=left;
		else write_size=chunk_size;
		f->Write((void*)(&p[1][write_start]),write_size);
		left-=write_size;
		write_start+=write_size;
	}

	left=out[2];
	write_start=0;
	while(left>0)
	{
		if(left<chunk_size) write_size=left;
		else write_size=chunk_size;
		f->Write((void*)(&p[2][write_start]),write_size);
		left-=write_size;
		write_start+=write_size;
	}

	left=out[3];
	write_start=0;
	while(left>0)
	{
		if(left<chunk_size) write_size=left;
		else write_size=chunk_size;
		f->Write((void*)(&p[3][write_start]),write_size);
		left-=write_size;
		write_start+=write_size;
	}

	f->Close();
}

void CCombineDoc::OnFileSaveframe() // work frame
{
	BeginWaitCursor();

	CString s;
	str1.LoadString(IDS_STRING384);
	s.Format(str1,path,buffer[active_frame].name);
	CFile f(s,CFile::modeCreate | CFile::modeWrite);

	void *f0=GetBuffer(active_frame);

	progress.AddString(IDS_STRING385);
	progress.AddString("   "+buffer[active_frame].name);

	writepfile(&f,f0);

	FreeBuffer(active_frame);

	EndWaitCursor();
}

void CCombineDoc::saveframe()
{
	CString s;
	str1.LoadString(IDS_STRING386);
	s.Format(str1,path,buffer[active_frame].name);
	CFile f(s,CFile::modeCreate | CFile::modeWrite);

	void *f0=GetBuffer(active_frame);

	progress.AddString("   "+buffer[active_frame].name);

	writepfile(&f,f0);

	FreeBuffer(active_frame);
}

void CCombineDoc::OnSaveallframes() // work frames
{
	BeginWaitCursor();

	progress.AddString(IDS_STRING387);
	saveallframes();

	EndWaitCursor();
}

void CCombineDoc::saveallframes()
{
	int f;

	CString s;

	BeginWaitCursor();

	for(f=top_frame-1; f>=0; f--)
	{
		progress.AddString(L"   "+buffer[f].name+L".czp");

		str1.LoadString(IDS_STRING388);
		s.Format(str1,path,buffer[f].name);
		CFile of(s,CFile::modeCreate | CFile::modeWrite);

		void *f0=GetBuffer(f);

		writepfile(&of,f0);

		FreeBuffer(f);
	}

	EndWaitCursor();
}

void CCombineDoc::save_work()
{
	OnFileSaveworkstate();
}

void CCombineDoc::OnFileSaveworkstate() 
{
	int working;
	CFileFind file;
	CString p;

	p=work_path;
	str1.LoadString(IDS_STRING389);
	p+=str1;
	working=file.FindFile(p);
	while(working)
	{
		working=file.FindNextFile();
		if(file.IsDirectory()) continue;
		if(working) CFile::Remove(file.GetFilePath());
	}
	file.Close();
	progress.AddString(IDS_STRING390);

	save_workstate();

	saveallframes();

	savedepth();
}

void CCombineDoc::save_workstate()
{
	int i,j,count;
	CString s;

	if(top_frame==0) return;

	progress.AddString(IDS_STRING391);

	str1.LoadString(IDS_STRING392);
	CString full_path=work_path+str1;

	BeginWaitCursor();
	CFile f(full_path,CFile::modeCreate | CFile::modeWrite);

	int version=VERSION;
	f.Write(&version,sizeof(int));

	f.Write(&w,sizeof(int));
	f.Write(&h,sizeof(int));
	f.Write(&top_frame,sizeof(int));
	f.Write(&bmfh,sizeof(bmfh));
	f.Write(&bmh,sizeof(bmh));
	f.Write(&bmih,sizeof(bmih));
	f.Write(&smallw,sizeof(int));
	f.Write(&smallh,sizeof(int));
	f.Write(&slot_size,sizeof(int));
	f.Write(&frame_border,sizeof(int));
	f.Write(&goodrect,sizeof(goodrect));

	f.Write(&top_used_frame,sizeof(int));
	for(i=0; i<top_used_frame; i++)
		f.Write(&used_frame[i],sizeof(int));

	char ch0=0;
	for(i=0; i<top_frame; i++)
	{
		f.Write(buffer[i].name,2*buffer[i].name.GetLength());
		f.Write("\0",2);
	}
	f.Write("\0",2);

	count=progress.m_list.GetCount();
	f.Write(&count,sizeof(int));
	for(i=0; i<count; i++)
	{
		j=progress.m_list.GetTextLen(i);
		progress.m_list.GetText(i,s);
		f.Write(s,j*2);
		j=0;
		f.Write(&j,2);
	}

	if(bmp_properties)
	{
		f.Write(&bmpprop_count,4);
		f.Write(&bmpprop_size,4);
		f.Write(bmp_properties,bmpprop_size);
	}

	f.Close();

	EndWaitCursor();
}

void CCombineDoc::OnFileResumeWork() 
{
	CString s;
	int i,count;
	wchar_t wc;

	if(top_frame!=0)
	{
		CString s1,s2;
		s1.LoadString(IDS_RELOADSAVE); s2.LoadString(IDS_RESUMEWORK);
		int ret=MessageBox(progress.m_hWnd,s1,s2,MB_YESNO);
		if(ret==IDYES) return;
	}


	if(help_window)
	{
		ShowWindow(help_window,SW_MINIMIZE);
	}

	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	OnNewDocument();

	path=AfxGetApp()->GetProfileString(L"File",L"Path");
	save_path=AfxGetApp()->GetProfileString(L"File",L"save_Path");
	work_path=AfxGetApp()->GetProfileString(L"File",L"work_path",path);
	CString p=work_path.Right(work_path.GetLength()-work_path.ReverseFind('\\')-1);
	progress.SetWindowText(p);
	save_ext=AfxGetApp()->GetProfileString(L"File",L"save_ext");

	str1.LoadString(IDS_STRING393);
	CFileDialog fd(true,L"czd",0,0,str1,0);
	fd.m_ofn.lpstrInitialDir=work_path;
	str1.LoadString(IDS_STRING394);
	fd.m_ofn.lpstrTitle=str1;
	if(fd.DoModal()==IDCANCEL)
	{
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		return;
	}
	work_path=fd.GetPathName();
	work_path=work_path.Left(work_path.ReverseFind('\\'));
	CString pp=work_path.Right(work_path.GetLength()-work_path.ReverseFind('\\')-1);
	progress.SetWindowText(pp);

	progress.AddString(IDS_STRING395);
	progress.AddString(IDS_STRING396);

	CFile f(fd.m_ofn.lpstrFile,CFile::modeRead);

	int version;
	f.Read(&version,sizeof(int));
	if(version!=VERSION)
	{
		CString s1,s2;
		s1.LoadString(IDS_RESUMEVERSION); s2.LoadString(IDS_RESUMEWORK);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		f.Close();
		progress.AddString(IDS_STRING397);
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		return;
	}

	BeginWaitCursor();

	f.Read(&w,sizeof(int));
	f.Read(&h,sizeof(int));
	f.Read(&top_frame,sizeof(int));
	f.Read(&bmfh,sizeof(bmfh));
	f.Read(&bmh,sizeof(bmh));
	f.Read(&bmih,sizeof(bmih));
	f.Read(&smallw,sizeof(int));
	f.Read(&smallh,sizeof(int));
	f.Read(&slot_size,sizeof(int));
	f.Read(&frame_border,sizeof(int));
	f.Read(&goodrect,sizeof(goodrect));

	f.Read(&top_used_frame,sizeof(int));
	for(i=0; i<top_used_frame; i++)
		f.Read(&used_frame[i],sizeof(int));
	last_used_frame=top_used_frame;

	if(!ReInitializeMemory())
	{
		f.Close();
		EndWaitCursor();
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		return;
	}

	namebuffers();

	for(i=0; i<top_frame; i++)
	{
		buffer[i].name=L"";
		while(f.Read(&wc,2),wc) buffer[i].name+=wc;
	}
	f.Read(&wc,2);

	mag.left=mag.top=0;
	mag.right=w-1;
	mag.bottom=h-1;

	saved_mag=mag;

	loaddepth();

	for(i=0; i<top_frame; i++)
	{
		display=active_frame=i;
		reload();
		buffer[i].hidden=true;
	}

	for(i=0; i<top_used_frame; i++) buffer[used_frame[i]].hidden=false;

	redraw_picture();

	sizexorg=w/2;
	sizeyorg=h/2;

	EndWaitCursor();

	RefreshScreen();

	progress.AddString(IDS_STRING398);

	f.Read(&count,sizeof(int));
	for(i=0; i<count; i++)
	{
		s="";
		f.Read(&wc,2);
		while(wc){
			s+=wc;
			f.Read(&wc,2);
		}
		if((s[0]!=0) && (s[0]!='*')) progress.AddString(s);
	}

	if(f.Read(&bmpprop_count,4)==4)
	{
		f.Read(&bmpprop_size,4);
		bmp_properties=new char[bmpprop_size];
		f.Read(bmp_properties,bmpprop_size);
	}
	if(!AfxGetApp()->GetProfileInt(L"Header",L"Save",true))
	{
		delete bmp_properties;
		bmp_properties=0;
	}

	f.Close();

	progress.AddString(IDS_STRING399);

	save_state();

	path=save_path=work_path;
	AfxGetApp()->WriteProfileString(L"File",L"Path",path);
	AfxGetApp()->WriteProfileString(L"File",L"save_path",save_path);
	AfxGetApp()->WriteProfileString(L"File",L"work_path",work_path);
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	AfxGetMainWnd()->SetFocus();
}

void CCombineDoc::OnFileOpen() 
{
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	if(help_window)
	{
		ShowWindow(help_window,SW_MINIMIZE);
	}
	progress.Maximize();
	AfxBeginThread(openT,this);
}

void sort(CString s[],int n)
{
	CString temp,t1,t2;
	int i,j;
	int maxlen;
	maxlen=0;
	for(j=0; j<n; j++)
	{
		if(s[j].GetLength()>maxlen) maxlen=s[j].GetLength();
	}
	for(j=0; j<n; j++)
	{
		while(s[j].GetLength()<maxlen) s[j]='@'+s[j];
	}

	for(j=0; j<n; j++)
	{
		for(i=0; i<n-1; i++)
		{
			t1=s[i]; t2=s[i+1];
			if(t1.CompareNoCase(t2)>=0) continue;
			temp=s[i];
			s[i]=s[i+1];
			s[i+1]=temp;
		}
	}
	for(j=0; j<n; j++)
	{
		while(s[j][0]=='@') s[j]=s[j].Right(s[j].GetLength()-1);
	}
}

void CCombineDoc::OnFileNew() 
{
	if(top_frame!=0)
	{
		CString s1,s2;
		s1.LoadString(IDS_LOADSAVE); s2.LoadString(IDS_NEWSTACK);
		int ret=MessageBox(progress.m_hWnd,s1,s2,MB_YESNO);
		if(ret==IDYES) return;
	}
	OnNewDocument();
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	if(help_window)
	{
		ShowWindow(help_window,SW_MINIMIZE);
	}
	progress.Maximize();
	AfxBeginThread(openT,this);
}

UINT CCombineDoc::openT(LPVOID param)
{
	return((CCombineDoc*)param)->open();
}

bool CCombineDoc::open()
{
	index--;
	AfxGetApp()->WriteProfileInt(L"File",L"Index",index);

	buissy=true;

	CString loading_path;
	bool loading=false;
	if(top_frame!=0) loading=true;

	CString name[MAX_FRAMES];
	CString str;
	CString error_message;
	int error;

	progress.AddString(IDS_STRING400);
	if(!skip_fsel)
	{
		file_list_buffer[0]=file_list_buffer[1]=0;

		if(loading)
			loading_path=AfxGetApp()->GetProfileString(L"File",L"Path");
		else
			path=AfxGetApp()->GetProfileString(L"File",L"Path");

		str1.LoadString(IDS_STRING401);
		CFileDialog fd(true,L"bmp",0,0,str1,0);
		fd.m_ofn.Flags|=OFN_ALLOWMULTISELECT|OFN_ENABLESIZING;
		fd.m_ofn.lpstrFile=file_list_buffer;
		fd.m_ofn.nMaxFile=MAX_FRAMES*128;
		if(loading)
			fd.m_ofn.lpstrInitialDir=loading_path;
		else
			fd.m_ofn.lpstrInitialDir=path;
		if(fd.DoModal()==IDCANCEL) goto end;
	}

// get first part of path, whole path if single file
	bool single;
	int pos=(int)wcslen(file_list_buffer);

	if(file_list_buffer[pos+1]) single=false;
	else single=true;

	if(loading)
	{
		loading_path=file_list_buffer;
		if(single)
		{
			loading_path=loading_path.Left(loading_path.ReverseFind('\\'));
			while(file_list_buffer[pos]!='\\') pos--;
			pos++;
		} else pos++;
		progress.AddString(IDS_STRING402);
		progress.AddString((CString)"  "+loading_path);
	} else {
		path=file_list_buffer;
		if(single)
		{
			path=path.Left(path.ReverseFind('\\'));
			while(file_list_buffer[pos]!='\\') pos--;
			pos++;
		} else pos++;
		progress.AddString(IDS_STRING402);
		progress.AddString((CString)"  "+path);

		if(top_frame==0)
		{
			str1.LoadString(IDS_STRING403);
			_wmkdir(path+str1);
			work_path=path;
			CString p=path.Right(path.GetLength()-path.ReverseFind('\\')-1);
			progress.SetWindowText(p);
		}
	}

	int files=0;
	while(file_list_buffer[pos])
	{
		while(file_list_buffer[pos]){name[files]+=file_list_buffer[pos]; pos++;}
		pos++;
		files++;
	};

// n=0 if starting from scratch, save path to registry
	if((top_frame<1) && !skip_fsel)
	{
		AfxGetApp()->WriteProfileString(L"File",L"Path",path);
		AfxGetApp()->WriteProfileString(L"File",L"work_path",work_path);
	}
// sort into ascending order
	if(files>1) sort(name,files);

	using namespace Gdiplus;
	Bitmap *bmp;

// Main open/store/close loop
	int nf;
	for(nf=0; nf<files; nf++)
	{
// need 2 byte character string to open bitmap files using GDI+
		if(loading)
			wcscpy(file_list_buffer,loading_path);
		else
			wcscpy(file_list_buffer,path);
		wcscat(file_list_buffer,L"\\");
		wcscat(file_list_buffer,name[nf]);

		bmp=(Bitmap *)new Bitmap(file_list_buffer,false);
		error=bmp->GetLastStatus();
		if(error)
		{// can't open
			CString s1,s2;
			s1.LoadString(IDS_ERRORUNSUP);
			MessageBox(progress.m_hWnd,s1,file_list_buffer,MB_OK);
			delete bmp;
			continue;
		}

		int pixel_format=bmp->GetPixelFormat();

		BitmapData bd;
		Rect r(0,0,bmp->GetWidth(),bmp->GetHeight());
		if(bmp->LockBits(&r,1,PixelFormat24bppRGB,&bd)!=Ok)
		{
			CString s1,s2;
			s1.LoadString(IDS_ERRORUNSUP); s2.LoadString(IDS_LOADFRAME);
			MessageBox(progress.m_hWnd,s1,s2,MB_OK);
			delete bmp;
			continue;
		}

		if(!top_frame) // first file/frame build names of other buffers, initialize variables
		{
			if(AfxGetApp()->GetProfileInt(L"Header",L"Save",true))
			{
				bmp->GetPropertySize(&bmpprop_size,&bmpprop_count);
				bmp_properties=new char[bmpprop_size];
				bmp->GetAllPropertyItems(bmpprop_size,bmpprop_count,(PropertyItem*)bmp_properties);
			} else bmp_properties=0;

			CString s;
			str1.LoadString(IDS_STRING404);
			s.Format(str1,pixel_format);
			progress.AddString(s);

			smallh=bd.Height;
			smallw=bd.Width;

			if(!InitializeMemory())
			{
				bmp->UnlockBits(&bd);
				buissy=false;
				AfxGetMainWnd()->SetForegroundWindow();
				AfxGetMainWnd()->ShowWindow(SW_SHOW);
				AfxGetMainWnd()->SetFocus();
				return false;
			}

			str1.LoadString(IDS_STRING405);
			s.Format(str1,smallw,smallh);
			progress.AddString(s);
			str1.LoadString(IDS_STRING406);
			s.Format(str1,w,h);
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
		} else { // check each file is correct size
			if(((int)bmp->GetHeight()!=smallh) || ((int)bmp->GetWidth()!=smallw))
			{
				CString s1,s2;
				s1.LoadString(IDS_ERRORFRSIZE); s2.LoadString(IDS_ERROR);
//				MessageBox(progress.m_hWnd,s1,s2,MB_OK);
				progress.AddString(s1);
				bmp->UnlockBits(&bd);
				delete bmp;
				continue; // won't fit in a frame
			}
		}

// get pointer to buffer, and name it
		top_frame++;
		PIXEL *f1=(PIXEL*)GetBuffer(top_frame-1);

		buffer[top_frame-1].name.Format(L"%05d_%s",index,name[nf].Left(name[nf].ReverseFind('.')));

		str.Format(L"(%d) %s",top_frame-1,buffer[top_frame-1].name);
		progress.AddString(str);
		progress.UpdateWindow();

		used_frame[top_used_frame]=top_frame-1;
		top_used_frame++;
		last_used_frame=top_used_frame;

// collect info from bitmap info header, build same for when frames are saved
		unsigned char* b;
		b=(unsigned char*)bd.Scan0;

		bw=bd.Stride;

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

// unpack bitmap into buffer
		int x,y,Y,i,startpi,pi=0;
		PIXEL p;
		for(y=0; y<smallh; y++)
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
				f1[i]=p;
			}
			pi=startpi+bd.Stride;
		}
		bmp->UnlockBits(&bd);

		display=active_frame=top_frame-1;
		TidyOuterEdges(display);

		delete bmp;

		UpdateBuffer(top_frame-1);

		if(top_frame==MAX_FRAMES)
		{
			CString s1,s2;
			s1.LoadString(IDS_FILELIMITREACHED); s2.LoadString(IDS_FILELIMIT);
			MessageBox(progress.m_hWnd,s1,s2,MB_OK);
			break;
		}

		if(progress.m_cancel)
		{
			progress.m_cancel=false;
			break;
		}
	}
	if(depth_flag)
	{
		delete [] rainbow; rainbow=0;
		depth_flag=false;
		backup_picture_valid=false;
		if(top_frame) redraw_picture();
	}

	if(top_frame)
	{
		active_frame2=UNSET;
		special_active=active_frame=display=top_frame-1;

		save_state();

		if(top_frame!=0) this_file="";
	}
end:
	if(!macro_mode)
	{
		buissy=false;
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		AfxGetMainWnd()->SetForegroundWindow();
		AfxGetMainWnd()->SetFocus();

		progress.Minimize();
		RefreshScreen();
	}
	return true;
}

void CCombineDoc::delete_work()
{
	int working;
	CFileFind file;
	CString p;

	p=work_path;
//	str1.LoadString(IDS_STRING407);
	p+=L"\\work\\*.czb";
	working=file.FindFile(p);
	while(working)
	{
		working=file.FindNextFile();
		if(file.IsDirectory()) continue;
		CFile::Remove(file.GetFilePath());
	}
	file.Close();

	p=work_path;
	str1.LoadString(IDS_STRING408);
	p+=str1;
	_wrmdir(p);
}

void CCombineDoc::namebuffers()
{
	int i;

	str1.LoadString(IDS_STRING427);
	buffer[OUT_BUFFER].name				=str1;
	str1.LoadString(IDS_STRING428);
	buffer[PICTURE_BUFFER].name			=str1;
	buffer[BACKUP_PICTURE_BUFFER].name	="backup";
	str1.LoadString(IDS_STRING429);
	buffer[DEPTH_BUFFER].name			=str1;
	buffer[SAVED_DEPTH_BUFFER].name		="svdepth";
	buffer[DFT_RIO_BUFFER].name			="dftrio";
	buffer[DFT_GIO_BUFFER].name			="dftgio";
	buffer[DFT_BIO_BUFFER].name			="dftbio";
	buffer[DFT_RSTORE_BUFFER].name		="dftrstore";
	buffer[DFT_GSTORE_BUFFER].name		="dftgstore";
	buffer[DFT_BSTORE_BUFFER].name		="dftbstore";
	buffer[DFT_FILTER_BUFFER].name		="dftfilter";
	buffer[GRAB_BUFFER].name			="grab";

	CString s;
	str1.LoadString(IDS_STRING409);
	for(i=PYRAMID_BUFFER; i<PYRAMID_BUFFER+15; i++)
	{
		s.Format(str1,i);
		buffer[i].name=s;
	}
	str1.LoadString(IDS_STRING410);
	for(i=TEMP_BUFFER; i<=LAST_BUFFER; i++)
	{
		s.Format(str1,i);
		buffer[i].name=s;
	}
}

UINT CCombineDoc::silent_openT(LPVOID param)
{
	return((CCombineDoc*)param)->silent_open();
}

bool CCombineDoc::silent_open()
{
	path=folder_path;
	if(path.Right(4)==L".wmv")
	{
		silentextract();
		return true;
	}

	CString str;
	CString error_message;
	int error,pos;

	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	progress.AddString(IDS_STRING411);
	AfxGetMainWnd()->ShowWindow(SW_HIDE);

// get first part of path, whole path if single file

	path=folder_path;

	progress.AddString(IDS_STRING412);
	progress.AddString((CString)"  "+path);

	wcscat(folder_path,L"/*.*");

	work_path=path;
	CString p=path.Right(path.GetLength()-path.ReverseFind('\\')-1);
	progress.SetWindowText(p);
	str1.LoadString(IDS_STRING413);
	_wmkdir(work_path+str1);

	CFileFind ff;
	int found=ff.FindFile(folder_path);
	int files=0;
	CString name[MAX_FRAMES];
	while(found)
	{
		found=ff.FindNextFile();
		name[files]=ff.GetFileName();
		if(name[files][0]=='.')	continue;
		files++;
	};
	if(files==0) return false;

	top_frame=0;
// sort into ascending order
	if(files>1) sort(name,files);

	using namespace Gdiplus;
	Bitmap *bmp;

// Main open/store/close loop
	int nf;
	unsigned short s[1024];
	for(nf=0; nf<files; nf++)
	{
// need 2 byte character string to open bitmap files using GDI+
		wcscpy(file_list_buffer,path);
		wcscat(file_list_buffer,L"\\");
		wcscat(file_list_buffer,name[nf]);

		pos=0;
		while(file_list_buffer[pos])
		{
			s[pos]=((unsigned short)file_list_buffer[pos])&0xff;
			pos++;
		}
		s[pos]=0;
		
		bmp=(Bitmap *)new Bitmap(s,false);

		error=bmp->GetLastStatus();
		if(error) // can't open
			continue;

		int pixel_format=bmp->GetPixelFormat();

		BitmapData bd;
		Rect r(0,0,bmp->GetWidth(),bmp->GetHeight());
		if(bmp->LockBits(&r,1,PixelFormat24bppRGB,&bd)!=Ok)
		{
			CString s1,s2;
			s1.LoadString(IDS_ERRORUNSUP); s2.LoadString(IDS_CZOPEN);
			MessageBox(progress.m_hWnd,s1,s2,MB_OK);
			delete bmp;
			continue;
		}

		if(!top_frame) // first file/frame build names of other buffers, initialize variables
		{
			if(AfxGetApp()->GetProfileInt(L"Header",L"Save",true))
			{
				bmp->GetPropertySize(&bmpprop_size,&bmpprop_count);
				bmp_properties=new char[bmpprop_size];
				bmp->GetAllPropertyItems(bmpprop_size,bmpprop_count,(PropertyItem*)bmp_properties);
			} else bmp_properties=0;

			CString s;
			str1.LoadString(IDS_STRING414);
			s.Format(str1,pixel_format);
			progress.AddString(s);

			smallh=bd.Height;
			smallw=bd.Width;

			if(!InitializeMemory())
			{
				bmp->UnlockBits(&bd);
				buissy=false;
				return false;
			}

			str1.LoadString(IDS_STRING415);
			s.Format(str1,smallw,smallh);
			progress.AddString(s);
			str1.LoadString(IDS_STRING416);
			s.Format(str1,w,h);
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

		} else { // check each file is correct size
			if(((int)bmp->GetHeight()!=smallh) || ((int)bmp->GetWidth()!=smallw))
			{
				CString s1,s2;
				s1.LoadString(IDS_ERRORFRSIZE);// s2.LoadString(IDS_CZOPEN);
				progress.AddString(s1);
//				MessageBox(progress.m_hWnd,s1,s2,MB_OK);
				bmp->UnlockBits(&bd);
				delete bmp;
				continue; // won't fit in a frame
			}
		}

// get pointer to buffer, and name it
		buffer[top_frame].name=name[nf].Left(name[nf].ReverseFind('.'));

		str.Format(L"(%d) %s",top_frame,buffer[top_frame].name);
		progress.AddString(str);

		PIXEL *f1=(PIXEL*)GetBuffer(top_frame);

		used_frame[top_used_frame]=top_frame;
		top_used_frame++;
		last_used_frame=top_used_frame;
		top_frame++;

// collect info from bitmap info header, build same for when frames are saved
		unsigned char* b;
		b=(unsigned char*)bd.Scan0;

		bw=bd.Stride;

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

// unpack bitmap into buffer
		int x,y,Y,i,startpi,pi=0;
		PIXEL p;
		for(y=0; y<smallh; y++)
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
				f1[i]=p;
			}
			pi=startpi+bd.Stride;
		}
		bmp->UnlockBits(&bd);

		display=active_frame=top_frame-1;
		TidyOuterEdges(display);

		delete bmp;

		if(top_frame==MAX_FRAMES)
		{
			CString s1,s2;
			s1.LoadString(IDS_FILELIMITREACHED); s2.LoadString(IDS_FILELIMIT);
			break;
		}

		UpdateBuffer(top_frame-1);

		if(progress.m_cancel)
		{
			progress.m_cancel=false;
			break;
		}

	}

	if(top_frame==0) return false;

	if(top_frame==0)
	{
		AfxGetApp()->WriteProfileString(L"File",L"Path",path);
		AfxGetApp()->WriteProfileString(L"File",L"work_path",work_path);
	}

	if(depth_flag)
	{
		delete [] rainbow; rainbow=0;
		depth_flag=false;
		backup_picture_valid=false;
		redraw_picture();
	}

	active_frame2=UNSET;
	special_active=active_frame=display=top_frame-1;

	progress.GetDlgItem(IDC_CANCEL)->EnableWindow(true);

	save_state();

	if(top_frame!=0) this_file="";

	return true;
}

bool CCombineDoc::silentextract()
{
//	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	HRESULT hr=0;

	CString str;
	str1.LoadString(IDS_STRING616);
	progress.AddString(str1);
	progress.m_cancel=false;

	CString name;
	name=L"MovieFrame";

	CString p,pname;
	p=path;
	path=p.Left(p.ReverseFind('.'));
	pname=path.Right(path.GetLength()-path.ReverseFind('\\')-1);
	path=p.Left(p.ReverseFind('\\'));
	path+=L"\\_"+pname;
	_wmkdir(path);
	CString pp=path.Right(path.GetLength()-path.ReverseFind('\\')-1);
	progress.SetWindowText(pp);

	work_path=path;
	_wmkdir(work_path+L"\\work\\");

	progress.AddString(L"Output and work Folder");
	CString of; of.Format(L"  %s",path);
	progress.AddString(of);

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
	if((hr=pIReader->Open(p))!=S_OK) goto clearup;

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

	progress.m_cancel=false;
	return true;
}

bool CCombineDoc::silent_save_depth(wchar_t *file_path)
{
	CString new_path,temp_path=file_path;
	int dot=temp_path.ReverseFind('.');
	new_path=temp_path.Left(dot)+'d'+temp_path.Right(temp_path.GetLength()-dot);
	display=PICTURE_BUFFER;
	toggledepthmap();
	redraw_picture();
	Replace();
	CopyToOut();
	bool ret=silent_save_out(new_path);
	return ret;
}

bool CCombineDoc::silent_save_out(CString file_path)
{
	display=OUT_BUFFER;

	CString ext=file_path.Right(file_path.GetLength()-file_path.Find('.')-1);

	str1.LoadString(IDS_STRING417);
	progress.AddString(str1+buffer[display].name);
	str1.LoadString(IDS_STRING418);
	progress.AddString(str1+file_path);

	unsigned long comp;
	char *space=(char*)new char[sizeof(EncoderParameters)+1*sizeof(EncoderParameter)];
	EncoderParameters *ep=0;

	unsigned char *out=(unsigned char*)GetBuffer(display);

	Bitmap bm(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,out+4*(mag.left+w*mag.top));
	CLSID Clsid;
	if(ext==L"BMP")
	{
		GetEncoderClsid(L"image/bmp",&Clsid);
		Bitmap *bm1=bm.Clone(0,0,mag.right-mag.left,mag.bottom-mag.top,PixelFormat24bppRGB);

		PropertyItem property;
		int w=mag.Width(),h=mag.Height();
		property.id=PropertyTagImageHeight;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&h;
		bm1->SetPropertyItem(&property);

		property.id=PropertyTagImageWidth;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&w;
		bm1->SetPropertyItem(&property);

		saveproperties(bm1,ext);

		bm1->Save(file_path,&Clsid,ep);
		delete bm1;
	} else {
		ext.MakeLower();
		if(ext==L"bmp") GetEncoderClsid(L"image/bmp",&Clsid);
		else if(ext==L"tif") GetEncoderClsid(L"image/tiff",&Clsid);
		else if(ext==L"gif") GetEncoderClsid(L"image/gif",&Clsid);
		else if(ext==L"jpg")
		{
			ep=(EncoderParameters*)space;
			ep->Count=1;
			ep->Parameter[0].Guid=EncoderQuality;
			ep->Parameter[0].Type=EncoderParameterValueTypeLong;
			ep->Parameter[0].NumberOfValues=1;
			ep->Parameter[0].Value=&comp;
			comp=jpeg_comp;
			GetEncoderClsid(L"image/jpeg",&Clsid);
		}
		else if(ext==L"png") GetEncoderClsid(L"image/png",&Clsid);
		else
		{
			FreeBuffer(display);
			return false;
		}

		PropertyItem property;
		int w=mag.Width(),h=mag.Height();
		property.id=PropertyTagImageHeight;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&h;
		bm.SetPropertyItem(&property);

		property.id=PropertyTagImageWidth;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&w;
		bm.SetPropertyItem(&property);

		saveproperties(&bm,ext);

		bm.Save(file_path,&Clsid,ep);
	}
	FreeBuffer(display);

	delete [] space;

	return true;
}

void CCombineDoc::OnMovieExportmovieframes()
{
	int working;
	CFileFind file;
	CString p,fullp,ftitle;
	CFile f;

	BeginWaitCursor();

//	save_path=AfxGetApp()->GetProfileString(L"File",L"save_path");
	save_ext="BMP";

	_wchdir(save_path);
	p=path;
	str1.LoadString(IDS_STRING419);
	p+=str1;
	if(_wchdir(p))
	{
		_wmkdir(p);
		_wchdir(p);
	}

	str1.LoadString(IDS_STRING420);
	CString str2; str2.LoadString(IDS_STRING421);
	CFileDialog fd(false,save_ext,str1,0,str2+save_ext+L")|*."+save_ext+L"|*.jpg|*.jpg|*.tif|*.tif|*.bmp 32bpp|*.bmp|*.BMP 24bpp|*.BMP|*.gif|*.gif|*.png|*.png||",0);
	fd.m_ofn.lpstrDefExt=L"BMP";
	fd.m_ofn.lpstrInitialDir=p.GetBuffer(1024);
	if(fd.DoModal()==IDCANCEL)
	{
		EndWaitCursor();
		return;
	}
	p.ReleaseBuffer();
	p=fd.GetPathName();
	p=p.Left(p.GetLength()-4);
	ftitle=fd.GetFileTitle();
	try
	{
		working=file.FindFile(p+"*.*");
		while(working)
		{
			working=file.FindNextFile();
			if(file.IsDirectory()) continue;
			CFile::Remove(file.GetFilePath());
		}
	}
	catch(...){;}
	file.Close();

	progress.Maximize();
	progress.AddString(IDS_STRING422);
	progress.AddString("..."+p.Right(50));

	unsigned char *f0;

	if(!magon)
	{
		mag.bottom=h-1;
		mag.left=0;
		mag.right=w-1;
		mag.top=0;
	}

	unsigned long comp;
	unsigned short us[1024];
	int i;
	char *space=(char*)new char[sizeof(EncoderParameters)+1*sizeof(EncoderParameter)];
	EncoderParameters *ep=0;

	CLSID Clsid;
	CString ext(&fd.m_ofn.lpstrFile[fd.m_ofn.nFileExtension]);
	if(ext=="BMP")
	{
		GetEncoderClsid(L"image/bmp",&Clsid);

		int number=0;
		for(int frame=0; frame<top_frame; frame++)
		{
			if(buffer[frame].hidden) continue;

			f0=(unsigned char*)GetBuffer(frame);
			Bitmap *bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
			Bitmap *bm1=bm->Clone(0,0,mag.right-mag.left,mag.bottom-mag.top,PixelFormat24bppRGB);

			fullp.Format(L"%s%05d.%s",p,number,ext);

			i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
			bm1->Save(us,&Clsid,ep);

			fullp.Format(L"%05d.%s",number,ext);
			progress.AddString("   "+ftitle+fullp);

			delete bm;
			delete bm1;
			FreeBuffer(frame);

			number++;
		}
	} else {
		ext.MakeLower();
		if(ext=="bmp") GetEncoderClsid(L"image/bmp",&Clsid);
		else if(ext=="tif") GetEncoderClsid(L"image/tiff",&Clsid);
		else if(ext=="gif") GetEncoderClsid(L"image/gif",&Clsid);
		else if(ext=="jpg")
		{
			ep=(EncoderParameters*)space;
			ep->Count=1;
			ep->Parameter[0].Guid=EncoderQuality;
			ep->Parameter[0].Type=EncoderParameterValueTypeLong;
			ep->Parameter[0].NumberOfValues=1;
			ep->Parameter[0].Value=&comp;

			mode=JPEG;
			if(!GetValue()) return;
			mode=0;
			comp=jpeg_comp;
			GetEncoderClsid(L"image/jpeg",&Clsid);
		}
		else if(ext=="png") GetEncoderClsid(L"image/png",&Clsid);
		else
		{
			CString s1;
			s1.LoadString(IDS_ERRORFILEEXT);
			MessageBox(progress.m_hWnd,s1,ext,MB_OK);
		}

		int number=0;
		for(int frame=0; frame<top_frame; frame++)
		{
			if(buffer[frame].hidden) continue;

			f0=(unsigned char*)GetBuffer(frame);
			Bitmap *bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));

			fullp.Format(L"%s%05d.%s",p,number,ext);

			i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
			bm->Save(us,&Clsid,ep);

			fullp.Format(L"%05d.%s",number,ext);
			progress.AddString("   "+ftitle+fullp);

			delete bm;
			FreeBuffer(frame);

			number++;
		}
	}
	progress.Minimize();
	delete [] space;
	EndWaitCursor();
}

bool CCombineDoc::silent_save_active(int quality)
{
	CString ext;
	switch(quality)
	{
	case 0:
		ext="bmp";
		break;
	case 1:
		ext="tif";
		break;
	case 2:
		ext="gif";
		break;
	case 3:
		ext="png";
		break;
	default:
		ext="jpg";
	}

	CString s;
	str1.LoadString(IDS_STRING423);
	s.Format(str1,path,buffer[active_frame].name,ext);

	str1.LoadString(IDS_STRING424);
	CString num; num.Format(str1,quality);
	progress.AddString(num);
	str1.LoadString(IDS_STRING425);
	progress.AddString(str1+buffer[active_frame].name+L"."+ext);

	str1.LoadString(IDS_STRING426);
	_wmkdir(path+str1);

	unsigned long comp;
	unsigned short us[1024];
	int i;
	char *space=(char*)new char[sizeof(EncoderParameters)+1*sizeof(EncoderParameter)];
	EncoderParameters *ep=0;

	unsigned char *out=(unsigned char*)GetBuffer(active_frame);

	Bitmap bm(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,out+4*(mag.left+w*mag.top));
	CLSID Clsid;
	if(ext=="BMP")
	{
		GetEncoderClsid(L"image/bmp",&Clsid);
		Bitmap *bm1=bm.Clone(0,0,w,h,PixelFormat24bppRGB);
		i=0; while(s[i]){us[i]=((unsigned short)s[i])&0xff; i++;} us[i]=0;

		PropertyItem property;
		int w=mag.Width(),h=mag.Height();
		property.id=PropertyTagImageHeight;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&h;
		bm1->SetPropertyItem(&property);

		property.id=PropertyTagImageWidth;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&w;
		bm1->SetPropertyItem(&property);

		saveproperties(bm1,ext);

		bm1->Save(us,&Clsid,ep);
		delete bm1;
	} else {
		ext.MakeLower();
		if(ext=="bmp") GetEncoderClsid(L"image/bmp",&Clsid);
		else if(ext=="tif") GetEncoderClsid(L"image/tiff",&Clsid);
		else if(ext=="gif") GetEncoderClsid(L"image/gif",&Clsid);
		else if(ext=="jpg")
		{
			ep=(EncoderParameters*)space;
			ep->Count=1;
			ep->Parameter[0].Guid=EncoderQuality;
			ep->Parameter[0].Type=EncoderParameterValueTypeLong;
			ep->Parameter[0].NumberOfValues=1;
			ep->Parameter[0].Value=&comp;
			comp=jpeg_comp;
			GetEncoderClsid(L"image/jpeg",&Clsid);
		}
		else if(ext=="png") GetEncoderClsid(L"image/png",&Clsid);
		else
		{
			FreeBuffer(display);
			return false;
		}
		i=0; while(s[i]){us[i]=((unsigned short)s[i])&0xff; i++;} us[i]=0;
		
		PropertyItem property;
		int w=mag.Width(),h=mag.Height();
		property.id=PropertyTagImageHeight;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&h;
		bm.SetPropertyItem(&property);

		property.id=PropertyTagImageWidth;
		property.type=PropertyTagTypeLong;
		property.length=4;
		property.value=&w;
		bm.SetPropertyItem(&property);

		saveproperties(&bm,ext);

		bm.Save(us,&Clsid,ep);
	}
	FreeBuffer(active_frame);

	delete [] space;

	return true;
}

void CCombineDoc::OnFileEmptystack()
{
#if !defined W2000
	OnNewDocument();

	CString s;
	s="frame";
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	work_path=helppath.Left(helppath.ReverseFind('\\')+1);
	work_path+="CZTemp\\";
	_wmkdir(work_path);
	_wchdir(work_path);
	save_ext="";
	Redraw(0,true);
	s.LoadString(AFX_IDS_APP_TITLE);
	AfxGetMainWnd()->SetWindowText(s);
	RefreshScreen();
	progress.SetWindowText(L"CZP Progress ...");
#endif
}

void CCombineDoc::OnFileLoadspecial()
{
	mode=LOADSPECIAL;
	if(!GetValue()) return;
	loadspecial(special_frame);
}

void CCombineDoc::loadspecial(int number)
{
	progress.AddString(IDS_LOADSPECIAL,number);

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString special_path=helppath.Left(helppath.ReverseFind('\\')+1);
	special_path+="CZSpecial\\";
	special_path.Format(L"%s%05d.bmp",special_path,number);

	bool skipsave=skip_fsel;
	skip_fsel=true;

	wcscpy(file_list_buffer,special_path);
	file_list_buffer[wcslen(file_list_buffer)+1]=0;

	open();

	skip_fsel=skipsave;
}

void CCombineDoc::OnFileSavespecial()
{
	mode=SAVESPECIAL;
	if(!GetValue()) return;
	savespecial(special_frame);
}

void CCombineDoc::savespecial(int number)
{
	if(top_frame<1) return;

	progress.AddString(IDS_SAVESPECIAL,number);

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString special_path=helppath.Left(helppath.ReverseFind('\\')+1);
	special_path+="CZSpecial\\";
	_wmkdir(special_path);
	special_path.Format(L"%s%05d.bmp",special_path,number);

	unsigned char *out=(unsigned char*)GetBuffer(display);

	Bitmap bm(smallw,smallh,4*w,PixelFormat32bppRGB,out+4*(frame_border+w*frame_border));

	CLSID Clsid;
	GetEncoderClsid(L"image/bmp",&Clsid);

	bm.Save(special_path,&Clsid,0);

	FreeBuffer(display);
}
