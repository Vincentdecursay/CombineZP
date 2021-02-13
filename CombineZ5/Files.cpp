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
#include "ImportDM.h"

using namespace Gdiplus;

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

void CCombineDoc::OnFileSave() 
{
	CString s;

	int result;

	if(depth_flag && (display==PICTURE_BUFFER))
		s.Format(newnumber("depth"));
	else
		s.Format(newnumber(buffer[display].name+"_"));

	save_path=AfxGetApp()->GetProfileString("File","save_path");
	save_ext=AfxGetApp()->GetProfileString("File","save_ext");
	_chdir(save_path);
	CFileDialog fd(false,save_ext,s,0,"Default (*."+save_ext+")|*."+save_ext+"|*.jpg|*.jpg|*.tif|*.tif|*.bmp|*.bmp|*.gif|*.gif|*.png|*.png||",0);
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

	progress.AddString("Save  "+buffer[display].name);
	progress.AddString((CString)"  as  "+(CString)fd.m_ofn.lpstrFileTitle);

	Bitmap bm(w,h,4*w,PixelFormat32bppRGB,(unsigned char*)GetBuffer(display));
	CLSID Clsid;
	CString ext(&fd.m_ofn.lpstrFile[fd.m_ofn.nFileExtension]);
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
	else if(ext=="png") result=GetEncoderClsid(L"image/png",&Clsid);
	else
	{
		MessageBox(0,"Invalid file extention",ext,MB_OK);
	}


	i=0; while(fd.m_ofn.lpstrFile[i]){us[i]=((unsigned short)fd.m_ofn.lpstrFile[i])&0xff; i++;} us[i]=0;
	result=bm.Save(us,&Clsid,ep);
	FreeBuffer(display);
	delete space;

	AfxGetApp()->WriteProfileString("File","save_path",save_path);
	AfxGetApp()->WriteProfileString("File","save_ext",save_ext);

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
	s.Format("rectangle_%s",buffer[display].name);

	save_path=AfxGetApp()->GetProfileString("File","save_path");
	save_ext=AfxGetApp()->GetProfileString("File","save_ext");
	_chdir(save_path);
	CFileDialog fd(false,save_ext,s,0,"Default (*."+save_ext+")|*."+save_ext+"|*.jpg|*.jpg|*.tif|*.tif|*.bmp|*.bmp|*.gif|*.gif|*.png|*.png||",0);
	fd.m_ofn.lpstrDefExt=save_ext;
	fd.m_ofn.lpstrInitialDir=save_path;
	if(fd.DoModal()==IDCANCEL) return;
	s=fd.GetPathName();
	s=s.Left(s.ReverseFind('\\'));
	save_path=s;
	save_ext=fd.GetFileExt();

	progress.AddString("Save  Rectangle "+buffer[display].name);
	progress.AddString((CString)"  as  "+(CString)fd.m_ofn.lpstrFileTitle);

	unsigned long comp;
	unsigned short us[1024];
	int i;
	char *space=(char*)new char[sizeof(EncoderParameters)+1*sizeof(EncoderParameter)];
	EncoderParameters *ep=0;

	BeginWaitCursor();

	progress.AddString("Save  "+buffer[display].name);
	progress.AddString((CString)"  as  "+(CString)fd.m_ofn.lpstrFileTitle);

	unsigned char *out=(unsigned char*)GetBuffer(display);

	Bitmap bm(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,out+4*(mag.left+w*mag.top));
	CLSID Clsid;
	CString ext(&fd.m_ofn.lpstrFile[fd.m_ofn.nFileExtension]);
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
		MessageBox(0,"Invalid file extention",ext,MB_OK);
	}

	i=0; while(fd.m_ofn.lpstrFile[i]){us[i]=((unsigned short)fd.m_ofn.lpstrFile[i])&0xff; i++;} us[i]=0;
	bm.Save(us,&Clsid,ep);
	FreeBuffer(display);

	AfxGetApp()->WriteProfileString("File","save_path",save_path);
	AfxGetApp()->WriteProfileString("File","save_ext",save_ext);

	delete space;
	EndWaitCursor();
}

void CCombineDoc::OnFileExportrectangles() 
{
	int working;
	CFileFind file;
	CString p,fullp,ftitle;
	CFile f;

	BeginWaitCursor();

	save_path=AfxGetApp()->GetProfileString("File","save_path");
	save_ext=AfxGetApp()->GetProfileString("File","save_ext");

	_chdir(save_path);
	p=path;
	p+="\\rectangles\\";
	if(_chdir(p))
	{
		_mkdir(p);
		_chdir(p);
	}
	CFileDialog fd(false,save_ext,"rect_",0,"Default (*."+save_ext+")|*."+save_ext+"|*.jpg|*.jpg|*.tif|*.tif|*.bmp|*.bmp|*.gif|*.gif|*.png|*.png||",0);
//	CFileDialog fd(false,save_ext,"rect_",0,"Default (*."+save_ext+")|*."+save_ext+"|*.jpg|*.jpg|*.tif|*.tif|*.bmp|*.bmp|*.gif|*.gif|*.png|*.png||",0);
	fd.m_ofn.lpstrDefExt="tif";
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

	progress.Maximize();
	progress.AddString("Export Rectangles to");
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
		MessageBox(0,"Invalid file extention",ext,MB_OK);
	}

	for(int frame=0; frame<top_frame; frame++)
	{
		f0=(unsigned char*)GetBuffer(frame);
		Bitmap *bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));

		fullp.Format("%s%03d-%s.%s",p,top_frame-frame,buffer[frame].name,ext);

		i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
		bm->Save(us,&Clsid,ep);

		fullp.Format("%03d-%s.%s",top_frame-frame,buffer[frame].name,ext);
		progress.AddString("   "+ftitle+fullp);

		delete bm;
		FreeBuffer(frame);
	}

	f0=(unsigned char*)GetBuffer(PICTURE_BUFFER);
	Bitmap *bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
	fullp.Format("%s_picture.%s",p,ext);
	i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
	bm->Save(us,&Clsid,ep);
	fullp.Format("_picture.%s",ext);
	progress.AddString("   "+ftitle+fullp);
	delete bm;
	FreeBuffer(PICTURE_BUFFER);

	f0=(unsigned char*)GetBuffer(OUT_BUFFER);
	bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
	fullp.Format("%s_out.%s",p,ext);
	i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
	bm->Save(us,&Clsid,ep);
	fullp.Format("_out.%s",ext);
	progress.AddString("   "+ftitle+fullp);
	delete bm;
	FreeBuffer(OUT_BUFFER);

	progress.Minimize();
	delete space;
	EndWaitCursor();
}

void CCombineDoc::silent_export_rectangles()
{
	int working;
	CFileFind file;
	CString p,fullp,ftitle;
	CFile f;

	p=path;
	p+="\\rectangles\\";
	if(_chdir(p))
	{
		_mkdir(p);
		_chdir(p);
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

	progress.AddString("Export Rectangles to");
	progress.AddString("..."+p.Right(50));

	unsigned char *f0;

	unsigned short us[1024];
	int i;
	char *space=(char*)new char[sizeof(EncoderParameters)+1*sizeof(EncoderParameter)];
	EncoderParameters *ep=0;

	CLSID Clsid;
	CString ext("tif");
	GetEncoderClsid(L"image/tiff",&Clsid);

	for(int frame=0; frame<top_frame; frame++)
	{
		f0=(unsigned char*)GetBuffer(frame);
		Bitmap *bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));

		fullp.Format("%s/rectangles/%03d-%s.%s",path,top_frame-frame,buffer[frame].name,ext);

		i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
		bm->Save(us,&Clsid,ep);

		fullp.Format("%03d-%s.%s",top_frame-frame,buffer[frame].name,ext);
		progress.AddString("   "+ftitle+fullp);

		delete bm;
		FreeBuffer(frame);
	}

	f0=(unsigned char*)GetBuffer(PICTURE_BUFFER);
	Bitmap *bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
	fullp.Format("%s/rectangles/picture.%s",path,ext);
	i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
	bm->Save(us,&Clsid,ep);
	fullp.Format("picture.%s",ext);
	progress.AddString("   "+ftitle+fullp);
	delete bm;
	FreeBuffer(PICTURE_BUFFER);

	f0=(unsigned char*)GetBuffer(OUT_BUFFER);
	bm=(Bitmap *)new Bitmap(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,f0+4*(mag.left+w*mag.top));
	fullp.Format("%s/rectangles/out.%s",path,ext);
	i=0; while(fullp[i]){us[i]=((unsigned short)fullp[i])&0xff; i++;} us[i]=0;
	bm->Save(us,&Clsid,ep);
	fullp.Format("out.%s",ext);
	progress.AddString("   "+ftitle+fullp);
	delete bm;
	FreeBuffer(OUT_BUFFER);
	delete space;
}

void CCombineDoc::OnFileImportdepthmap()
{
	int pi=0,po,l,d;
	int startpi;

	CFileDialog fd(true,"bmp",0,0,"Bitmap (*.bmp)|*.bmp||",0);
	fd.m_ofn.lpstrInitialDir=(const char*)path;
	if(fd.DoModal()==IDCANCEL) return;

	backup_picture_valid=false;

	BeginWaitCursor();

	progress.AddString("Import Depthmap");

	CFile f(fd.m_ofn.lpstrFile,CFile::modeRead);
	progress.AddString("   "+(CString)fd.m_ofn.lpstrFileTitle);

	BITMAPFILEHEADER dmbmfh;
	BITMAPINFOHEADER dmbmih;
	f.Read((void*)&dmbmfh,sizeof(dmbmfh));
	f.Read((void*)&dmbmih,sizeof(dmbmih));
	if((int)h!=abs(dmbmih.biHeight) || (int)w!=abs(dmbmih.biWidth))
	{
		MessageBox(0,"The Width or Hight does not match the loaded frames, Aborting!","Import Depth Map",MB_OK);
		f.Close();
		EndWaitCursor();
		progress.AddString("*** Failed ***");
		return;
	}

	int bc=dmbmih.biBitCount/8;
	if(bc!=3)
	{
		MessageBox(0,"This is not a 24-bit RGB file, Aborting!","Import Depth Map",MB_OK);
		f.Close();
		EndWaitCursor();
		progress.AddString("*** Failed ***");
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
		MessageBox(0,"There are not enough Frames loaded","Import Depth Map",MB_OK);
		EndWaitCursor();
		progress.AddString("*** Failed ***");
		return;
	}

	int button;
	CImportDM dlg;
	button=dlg.DoModal();
	if(button==IDCANCEL) goto finish;

	if(button==IDC_REDDATA)
	{
		progress.AddString("   Import 'Red' data to Depthmap");
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
		progress.AddString("   Import 'Green' data to Depthmap");
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

	s.Format(newnumber("depth"));

	_chdir(path);
	CFileDialog fd(false,"bmp",s,0,"Bitmap (*.bmp)|*.bmp||",0);
	fd.m_ofn.lpstrDefExt="bmp";
	fd.m_ofn.lpstrInitialDir=path;
	if(fd.DoModal()==IDCANCEL) return;

	BeginWaitCursor();

	progress.AddString("Export  "+buffer[display].name);
	progress.AddString((CString)"  as  "+(CString)fd.m_ofn.lpstrFileTitle);

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

	f.Write(t,bmih.biSizeImage);

	f.Close();

	FreeBuffer(display);
	FreeBuffer(TEMP_BUFFER);

	EndWaitCursor();

	if(!saveflag) OnViewToggledepthmap();
}

void CCombineDoc::OnFileImport() 
{
	int height,width;

	CFileDialog fd(true,"bmp",0,0,"All formats|*.bmp;*.jpg;*.jpeg;*.gif;*.tif;*.tiff;*.png|*.bmp|*.bmp|*.jpg|*.jpg|*.jpeg|*.jpeg|*.gif|*.gif|*.tif|*.tif|*.tiff|*.tiff|*.png|*.png||",0);
//	CFileDialog fd(true,"bmp",0,0,"Bitmap (*.bmp)|*.bmp||",0);
	fd.m_ofn.lpstrInitialDir=(const char*)path;
	fd.m_ofn.Flags|=OFN_ENABLESIZING;
	if(fd.DoModal()==IDCANCEL) return;

	progress.AddString("Import Frame");

	BeginWaitCursor();

	const char *p=(const char*)fd.m_ofn.lpstrFile;
	unsigned short s[1024];
	for(int c=0; s[c]=p[c]; c++);
	using namespace Gdiplus;

	Bitmap *i;
	i=new Bitmap(s);

	height=i->GetHeight();
	width=i->GetWidth();

	unsigned char* b;
	BitmapData bd;
	Rect rect(0,0,width,height);
	i->LockBits(rect,1,PixelFormat32bppRGB,&bd);
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

	buffer[top_frame].name=fd.m_ofn.lpstrFileTitle;
	buffer[top_frame].hidden=true;

	progress.AddString("   "+buffer[top_frame].name);

	display=active_frame=top_frame;
	top_frame;

	delete i;

	if(top_frame==MAX_FRAMES)
	{
		MessageBox(0,
			"Input File Limit Reached",
			"File Limit",
			MB_OK);
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
	if(top_frame==1) return;

	if(!macro_mode)
		if(MessageBox(0,"Remove Frame\n"+buffer[display].name,"Remove Frame",MB_OKCANCEL)==IDCANCEL)
			return;

	remove();
	progress.AddString("Remove Visible Frame");
	progress.AddString("   "+buffer[display].name);
	save_state();
	RefreshScreen();
}

void CCombineDoc::remove()
{
	if(display==OUT_BUFFER) return;
	if(display==PICTURE_BUFFER) return;

	int i,x,y,Y;

	if(display==active_frame2) active_frame2=UNSET;

	CString s;

	UseFrame(display);

	RemoveBuffer(display);

	if(display!=top_frame-1)
	{
		for(i=display; i<top_frame-1; i++)
		{
			buffer[i]=buffer[i+1];
		}
	}

	ResetBuffer(top_frame-1);

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
	FreeBuffer(DEPTH_BUFFER);

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

	if(file.Open(path+"\\work\\"+buffer[f].name+".czp",CFile::modeRead))
	{
		s.Format("   %s",buffer[f].name);
		progress.AddString(s);
		f1=GetBuffer(f);
		readfile(&file,f1);
		file.Close();
		UpdateBuffer(f);
		return true;
	} else {
		s.Format("*** Failed to Reload %s",buffer[f].name);
		progress.AddString(s);
		s.Format("Can't Reload %s\nDid You Save it?",buffer[f].name);
		MessageBox(0,s,"Reload",MB_OK);
		file.Close();
		return false;
	}
}

bool CCombineDoc::OnFileReload() 
{
	BeginWaitCursor();
	progress.AddString("Reload Active Frame");
	bool ret=reload();
	RefreshScreen();
	EndWaitCursor();
	return ret;
}

bool CCombineDoc::Reload()
{
	return OnFileReload();
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
//	display=PICTURE_BUFFER;
	progress.Minimize();
	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::savedepth()
{
	BeginWaitCursor();
	unsigned char *depth=(unsigned char*)GetBuffer(DEPTH_BUFFER);
//	unsigned char *packed=(unsigned char*)GetBuffer(TEMP_BUFFER+4);
//	unsigned char *packed=new unsigned char[w*h*4];

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
	s.Format("%s\\work\\depth.czp",path);
	CFile f(s,CFile::modeCreate | CFile::modeWrite);
	f.Write(packed,out);
	f.Close();

//	FreeBuffer(TEMP_BUFFER+4);
//	delete packed;
	FreeBuffer(DEPTH_BUFFER);
	EndWaitCursor();
}

void CCombineDoc::OnFileWorkframesReloaddepth() 
{
	loaddepth();
	progress.AddString("Reload Depthmap");
	backup_picture_valid=false;
	redraw_picture();
	RefreshScreen();
}

void CCombineDoc::OnFileWorkframesSavedepth() 
{
	progress.AddString("Save Depthmap");
	savedepth();
}

void CCombineDoc::loaddepth()
{
	BeginWaitCursor();
	unsigned char *depth=(unsigned char*)GetBuffer(DEPTH_BUFFER);
//	unsigned char *packed=(unsigned char*)GetBuffer(TEMP_BUFFER+4);
//	unsigned char *packed=new unsigned char[w*h*4];

	CString s;
	s.Format("%s\\work\\depth.czp",path);
	CFile f(s,CFile::modeRead);
	f.Read(packed,f.GetLength());
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

//	FreeBuffer(TEMP_BUFFER+4);
//	delete packed;
	UpdateBuffer(DEPTH_BUFFER);
	EndWaitCursor();
}

void CCombineDoc::readfile(CFile *f, void *buffer)
{
//	unsigned char *packed=new unsigned char[w*h*4];
//	unsigned char *packed=(unsigned char*)GetBuffer(TEMP_BUFFER+4);

	int wh=(w+2)*(h+2);

	f->SeekToBegin();
	f->Read(packed,f->GetLength());

	unsigned char *bytes=(unsigned char*)buffer;
	unsigned int *ints=(unsigned int*)buffer;
	unsigned int a;
	int i,in=0;
	unsigned int count;
	unsigned int p1,p2;

	for(i=0; i<wh; i++) ints[i]=0;

	i=3;
	while(i<4*wh)
	{
		a=packed[in++];
		count=packed[in++];
		while(count)
		{
			bytes[i]=a;
			i+=4;
			count--;
		}
	};

	i=0;
	for(i=0; i<wh; i+=2)
	{
		p1=0;p2=0;
		a=packed[in++]; p1=((a&0xf0)>>4)|((a&0xf)<<8);
		a=packed[in++]; p1|=((a&0xf0)<<12); p2=(a&0xf);
		a=packed[in++]; p2|=((a&0xf0)<<4)|((a&0xf)<<16);

		ints[i]|=p1;
		ints[i+1]|=p2;
	}

	int c1,c2,c3;
	for(i=0; i<4*wh;)
	{
		p1=(int)packed[in++];
		p2=(int)packed[in++];

		count=(p2&0xf);
		c1=(p2&0xf0);
		c2=((p1<<4)&0xf0);
		c3=(p1&0xf0);
		while(count)
		{
			bytes[i++]|=(unsigned char)c1;
			bytes[i++]|=(unsigned char)c2;
			bytes[i++]|=(unsigned char)c3;
			i++;
			count--;
		}
	}

//	FreeBuffer(TEMP_BUFFER+4);
//	delete packed;
}

void CCombineDoc::writefile(CFile *f, void *buffer)
{
//	unsigned char *packed=new unsigned char[w*h*4];
//	unsigned char *packed=(unsigned char*)GetBuffer(TEMP_BUFFER+4);

	int wh=(w+2)*(h+2);

	int in,out=0;

	unsigned char *bytes=(unsigned char*)buffer;
	unsigned char this_byte,next_byte;
	unsigned int count;
	unsigned int p1,p2;

	count=1;
	next_byte=bytes[3];
	packed[out++]=next_byte;
	for(in=7; in<4*wh; in+=4)
	{
		this_byte=next_byte;
		next_byte=bytes[in];

		if((next_byte==this_byte) && (count<255))
		{
			count++;
		} else {
			packed[out++]=(unsigned char)count;
			count=1;
			packed[out++]=next_byte;
		}
	}
	packed[out++]=(unsigned char)count;

	for(in=0; in<4*wh; in+=8)
	{
		packed[out++]=((bytes[in+0]&0x0f)<<4)|(bytes[in+1]&0x0f);
		packed[out++]=((bytes[in+2]&0x0f)<<4)|(bytes[in+4]&0x0f);
		packed[out++]=((bytes[in+5]&0x0f)<<4)|(bytes[in+6]&0x0f);
	}

	unsigned int *ints=(unsigned int*)buffer;
	unsigned int this_int,next_int;

	count=1;
	next_int=(ints[0]&0x00f0f0f0);
	for(in=1; in<wh; in++)
	{
		this_int=next_int;
		next_int=(ints[in]&0x00f0f0f0);

		if((next_int==this_int) && (count<15))
		{
			count++;
		} else {
			p1=(((this_int>>16)&0xf0)|((this_int>>12)&0xf));
			p2=((this_int&0xf0)|(count&0xf));
			packed[out++]=(unsigned char)p1;
			packed[out++]=(unsigned char)p2;
			count=1;
		}
	}
	p1=(((this_int>>16)&0xf0)|((this_int>>12)&0xf));
	p2=((this_int&0xf0)|(count&0xf));
	packed[out++]=(unsigned char)p1;
	packed[out++]=(unsigned char)p2;

	f->Write(packed,out);
	f->Close();

//	FreeBuffer(TEMP_BUFFER+4);
//	delete packed;
}

void CCombineDoc::OnFileSaveframe() // work frame
{
	BeginWaitCursor();

	CString s;
	s.Format("%s\\work\\%s.czp",path,buffer[active_frame].name);
	CFile f(s,CFile::modeCreate | CFile::modeWrite);

	void *f0=GetBuffer(active_frame);

	progress.AddString("Save Active Frame");
	progress.AddString("   "+buffer[active_frame].name);

	writefile(&f,f0);

	FreeBuffer(active_frame);

	EndWaitCursor();
}

void CCombineDoc::saveframe()
{
	CString s;
	s.Format("%s\\work\\%s.czp",path,buffer[active_frame].name);
	CFile f(s,CFile::modeCreate | CFile::modeWrite);

	void *f0=GetBuffer(active_frame);

	progress.AddString("   "+buffer[active_frame].name);

	writefile(&f,f0);

	FreeBuffer(active_frame);
}

void CCombineDoc::OnSaveallframes() // work frames
{
	BeginWaitCursor();

	progress.AddString("Save Working copy of All Frames");
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
		progress.AddString("   "+buffer[f].name+".czp");

		s.Format("%s\\work\\%s.czp",path,buffer[f].name);
		CFile of(s,CFile::modeCreate | CFile::modeWrite);

		void *f0=GetBuffer(f);

		writefile(&of,f0);

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

	p=path;
	p+="\\work\\*.czp";
	working=file.FindFile(p);
	while(working)
	{
		working=file.FindNextFile();
		if(file.IsDirectory()) continue;
		if(working) CFile::Remove(file.GetFilePath());
	}
	file.Close();

	progress.AddString("Save Work");

	save_workstate();

	saveallframes();

	savedepth();
}

void CCombineDoc::save_workstate()
{
	int i,j,count;
	CString s;

	if(top_frame==0) return;

	progress.AddString("   work.czd");

	CString full_path=path+"\\work.czd";

	BeginWaitCursor();
	CFile f((const char*)full_path,CFile::modeCreate | CFile::modeWrite);

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
		f.Write((const char*)buffer[i].name,buffer[i].name.GetLength());
		f.Write("\0",1);
	}
	f.Write("\0",1);

	count=progress.m_list.GetCount();
	f.Write(&count,sizeof(int));
	for(i=0; i<count; i++)
	{
		j=progress.m_list.GetTextLen(i);
		progress.m_list.GetText(i,s);
		f.Write(s,j);
		j=0;
		f.Write(&j,1);
	}

	f.Close();

	EndWaitCursor();
}

void CCombineDoc::OnFileResumeWork() 
{
	CString s;
	int i,count;
	char ch[1];

	if(top_frame!=0)
	{
		int ret=MessageBox(0,"Do you wish to save anything before Reloading a New Stack?","Resume Work",MB_YESNO);
		if(ret==IDYES) return;
	}

	OnNewDocument();
//	InitializeMacros();

	path=AfxGetApp()->GetProfileString("File","Path");
	save_path=AfxGetApp()->GetProfileString("File","save_Path");
	save_ext=AfxGetApp()->GetProfileString("File","save_ext");

	CFileDialog fd(true,"czd",0,0,"work.czd|work.czd",0);
	fd.m_ofn.lpstrInitialDir=path;
	fd.m_ofn.lpstrTitle="Resume";
	if(fd.DoModal()==IDCANCEL) return;
	path=fd.GetPathName();
	path=path.Left(path.ReverseFind('\\'));

	progress.AddString("Resume Work");
	progress.AddString("   work");

	CFile f(fd.m_ofn.lpstrFile,CFile::modeRead);

	int version;
	f.Read(&version,sizeof(int));
	if(version!=VERSION)
	{
		MessageBox(0,"This file was created by an earlier version of CombineZ, It's fortmat is not compatible with this version, srrry.   You must start processing this stack from scratch again.","Resume Work",MB_OK);
		f.Close();
		progress.AddString("*** Failed ***");
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
		return;
	}

	namebuffers();

	for(i=0; i<top_frame; i++)
	{
		buffer[i].name="";
		while(f.Read(ch,1),ch[0]) buffer[i].name+=ch[0];
	}
	f.Read(ch,1);

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

//	OnViewSelectall();
	RefreshScreen();

	progress.AddString("****************** From Previous Session(s) ***************");

	f.Read(&count,sizeof(int));
	for(i=0; i<count; i++)
	{
		s="";
		f.Read(&ch[0],1);
		while(ch[0]){
			s+=ch[0];
			f.Read(&ch[0],1);
		}
		if((s[0]!=0) && (s[0]!='*')) progress.AddString(s);
	}

	f.Close();

	progress.AddString("*****************************************************************");

	save_state();

	AfxGetApp()->WriteProfileString("File","Path",path);
}

// slide show
/*
void CCombineDoc::next_file()
{
	if(top_frame!=1) return;

	CString this_file=buffer[0].name;

	bool finish=false;

	CFileFind file;
	CString s;
	s.Format("%s\\*.*",path);
	file.FindFile((const char*)s);
	do
	{
		if(file.FindNextFile()==0)
		{
			file.Close();
			return;
		}
		if(file.GetFileTitle()==this_file) break;
	} while(true);

	do{
		if(file.FindNextFile()==0)
		{
			finish=true;
		}

		s=file.GetFilePath().Right(3);
		s.MakeLower();
		if(s=="jpg") break;
		if(s=="bmp") break;
		if(s=="gif") break;
		if(s=="tif") break;
		if(s=="png") break;
		if(finish) return;
	}while(true);

	OnNewDocument();

	strcpy(file_list_buffer,file.GetFilePath());
	file_list_buffer[strlen(file_list_buffer)]=0;
	file_list_buffer[strlen(file_list_buffer)+1]=0;
	file_list_buffer[strlen(file_list_buffer)+2]=0;

	skip_fsel=true;
	AfxBeginThread(open,this);
	skip_fsel=false;

	file.Close();
}

void CCombineDoc::prev_file()
{
	if(top_frame!=1) return;

	CFileFind file;
	CString s;
	CString prev="";
	CString this_file=buffer[0].name;
	s.Format("%s\\*.*",path);
	file.FindFile((const char*)s);
	do
	{
		file.FindNextFile();
		if(file.GetFileTitle()==this_file) break;
		s=file.GetFilePath().Right(3);
		s.MakeLower();
		if(s=="jpg") prev=file.GetFilePath();
		if(s=="bmp") prev=file.GetFilePath();
		if(s=="gif") prev=file.GetFilePath();
		if(s=="tif") prev=file.GetFilePath();
		if(s=="png") prev=file.GetFilePath();
	} while(true);

	if(prev=="") return;

	OnNewDocument();

	strcpy(file_list_buffer,prev);
	file_list_buffer[strlen(file_list_buffer)]=0;
	file_list_buffer[strlen(file_list_buffer)+1]=0;
	file_list_buffer[strlen(file_list_buffer)+2]=0;

	skip_fsel=true;
	AfxBeginThread(open,this);
	skip_fsel=false;

	file.Close();
}
*/
void CCombineDoc::OnFileOpen() 
{
	progress.Maximize();
//	open();
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
		while(s[j].GetLength()<maxlen) s[j]="@"+s[j];
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
		int ret=MessageBox(0,"Do you wish to save anything before Loading a New Stack?","New Frame",MB_YESNO);
		if(ret==IDYES) return;
	}
	OnNewDocument();
	AfxBeginThread(openT,this);
}

UINT CCombineDoc::openT(LPVOID param)
{
	return((CCombineDoc*)param)->open();
}

bool CCombineDoc::open()
{
	CString str;
	CString error_message;
	int error;

	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	progress.Maximize();
	progress.AddString("Load Frame(s)");

	if(!skip_fsel)
	{
		file_list_buffer[0]=file_list_buffer[1]=0;

		path=AfxGetApp()->GetProfileString("File","Path");

		CFileDialog fd(true,"bmp",0,0,"All formats|*.bmp;*.jpg;*.jpeg;*.gif;*.tif;*.tiff;*.png|*.bmp|*.bmp|*.jpg|*.jpg|*.jpeg|*.jpeg|*.gif|*.gif|*.tif|*.tif|*.tiff|*.tiff|*.png|*.png||",0);
		fd.m_ofn.Flags|=OFN_ALLOWMULTISELECT|OFN_ENABLESIZING;
		fd.m_ofn.lpstrFile=file_list_buffer;
		fd.m_ofn.nMaxFile=MAX_FRAMES*128;
		fd.m_ofn.lpstrInitialDir=path;
		fd.SetWindowPos(0,0,0,640,480,SWP_NOOWNERZORDER|SWP_DRAWFRAME|SWP_FRAMECHANGED);
		fd.UpdateWindow();
		if(fd.DoModal()==IDCANCEL)
		{
			buissy=false;
			AfxGetMainWnd()->ShowWindow(SW_SHOW);
			return false;
		}
	}

//	BeginWaitCursor();

// get first part of path, whole path if single file
	bool single;
	int pos=strlen(file_list_buffer);

	if(file_list_buffer[pos+1]) single=false;
	else single=true;

	path=file_list_buffer;
	if(single)
	{
		path=path.Left(path.ReverseFind('\\'));
		while(file_list_buffer[pos]!='\\') pos--;
		pos++;
	} else pos++;

	_mkdir(path+"\\work");

	int files=0;
	CString name[MAX_FRAMES];
	while(file_list_buffer[pos])
	{
		while(file_list_buffer[pos]){name[files]+=file_list_buffer[pos]; pos++;}
		pos++;
		files++;
	};

// n=0 if starting from scratch, save path to registry
	if(!top_frame) AfxGetApp()->WriteProfileString("File","Path",path);
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
		strcpy(file_list_buffer,path);
		strcat(file_list_buffer,"\\");
		strcat(file_list_buffer,name[nf]);

		pos=0;
		while(file_list_buffer[pos])
		{
			s[pos]=((unsigned short)file_list_buffer[pos])&0xff;
			pos++;
		}
		s[pos]=0;

		bmp=(Bitmap *)new Bitmap(s,false);
		if(error=bmp->GetLastStatus()) // can't open
			continue;

		int pixel_format=bmp->GetPixelFormat();

		BitmapData bd;
		Rect r(0,0,bmp->GetWidth(),bmp->GetHeight());
		if(bmp->LockBits(r,1,PixelFormat24bppRGB,&bd)!=Ok)
		{
			MessageBox(0,"Unsupported File Format","Load Frame",MB_OK);
			delete bmp;
			continue;
		}

		if(!top_frame) // first file/frame build names of other buffers, initialize variables
		{
			CString s;
			s.Format("Pixel Format %x",pixel_format);
			progress.AddString(s);

			smallh=bd.Height;//bmp->GetHeight();
			smallw=bd.Width;//bmp->GetWidth();

			if(!InitializeMemory())
			{
				AfxGetMainWnd()->ShowWindow(SW_SHOW);
				AfxGetMainWnd()->SetFocus();
				AfxGetMainWnd()->SetForegroundWindow();
				bmp->UnlockBits(&bd);
				buissy=false;
				return false;
			}

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

//			InitializeMacros();
		} else { // check each file is correct size
			if(((int)bmp->GetHeight()!=smallh) || ((int)bmp->GetWidth()!=smallw))
			{
				MessageBox(progress.m_hWnd,"Frames must all be the same size","Error",MB_OK);
				bmp->UnlockBits(&bd);
				delete bmp;
				continue; // won't fit in a frame
			}
		}

// get pointer to buffer, and name it
		buffer[top_frame].name=name[nf].Left(name[nf].ReverseFind('.'));

		str.Format("(%d) %s",top_frame,buffer[top_frame].name);
		progress.AddString(str);
		progress.UpdateWindow();

		PIXEL *f1=(PIXEL*)GetBuffer(top_frame);

		used_frame[top_used_frame]=top_frame;
		top_used_frame++;
		last_used_frame=top_used_frame;
		top_frame++;

// collect info from bitmap info header, build same for when frames are saved
		unsigned char* b;
//		Rect r(0,0,smallw,smallh);
//		bmp->LockBits(r,1,PixelFormat32bppRGB,&bd);
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
				p.a=aGOOD; //pi++;
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
			MessageBox(progress.m_hWnd,
				"Input File Limit Reached",
				"File Limit",
				MB_OK);
			break;
		}

		UpdateBuffer(top_frame-1);

//		RefreshScreen();
//		progress.Maximize();
		if(progress.m_cancel)
		{
			progress.m_cancel=false;
			break;
		}
	}
	if(depth_flag)
	{
		delete rainbow; rainbow=0;
		depth_flag=false;
		backup_picture_valid=false;
		redraw_picture();
	}

//	EndWaitCursor();

	active_frame2=UNSET;
	special_active=active_frame=display=top_frame-1;

//	progress.GetDlgItem(IDC_CANCEL)->EnableWindow(true);

	save_state();

	if(top_frame!=0) this_file="";

	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	AfxGetMainWnd()->SetFocus();
	AfxGetMainWnd()->SetForegroundWindow();

//	InitializeMacros();
	progress.Minimize();

	RefreshScreen();
	return true;
}

void CCombineDoc::delete_work()
{
	int working;
	CFileFind file;
	CString p;

	p=path;
	p+="\\work\\*.czb";
	working=file.FindFile(p);
	while(working)
	{
		working=file.FindNextFile();
		if(file.IsDirectory()) continue;
		CFile::Remove(file.GetFilePath());
	}
	file.Close();
}

void CCombineDoc::namebuffers()
{
	buffer[OUT_BUFFER].name				="out";
	buffer[PICTURE_BUFFER].name			="picture";
	buffer[BACKUP_PICTURE_BUFFER].name	="backup";
	buffer[DEPTH_BUFFER].name			="depth";
	buffer[SAVED_DEPTH_BUFFER].name		="svdepth";
	buffer[DFT_RIO_BUFFER].name			="dftrio";
	buffer[DFT_GIO_BUFFER].name			="dftgio";
	buffer[DFT_BIO_BUFFER].name			="dftbio";
	buffer[DFT_RSTORE_BUFFER].name		="dftrstore";
	buffer[DFT_GSTORE_BUFFER].name		="dftgstore";
	buffer[DFT_BSTORE_BUFFER].name		="dftbstore";
	buffer[DFT_FILTER_BUFFER].name		="dftfilter";

	CString s;
	for(int i=TEMP_BUFFER; i<=LAST_BUFFER; i++)
	{
		s.Format("temp%2d",i);
		buffer[i].name=s;
	}
}

UINT CCombineDoc::silent_openT(LPVOID param)
{
	return((CCombineDoc*)param)->silent_open();
}

bool CCombineDoc::silent_open()
{
	CString str;
	CString error_message;
	int error,pos;

//	progress.Minimize();
	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	progress.AddString("Load Frame(s)");
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	progress.SetForegroundWindow();

// get first part of path, whole path if single file

	path=folder_path;
	strcat(folder_path,"/*.*");

	_mkdir(path+"\\work");

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
		strcpy(file_list_buffer,path);
		strcat(file_list_buffer,"\\");
		strcat(file_list_buffer,name[nf]);

		pos=0;
		while(file_list_buffer[pos])
		{
			s[pos]=((unsigned short)file_list_buffer[pos])&0xff;
			pos++;
		}
		s[pos]=0;
		
		bmp=(Bitmap *)new Bitmap(s,false);

		if(error=bmp->GetLastStatus()) // can't open
			continue;

		int pixel_format=bmp->GetPixelFormat();

		BitmapData bd;
		Rect r(0,0,bmp->GetWidth(),bmp->GetHeight());
		if(bmp->LockBits(r,1,PixelFormat24bppRGB,&bd)!=Ok)
		{
			MessageBox(0,"Unsupported File Format","CombineZ - Load Frame",MB_OK);
			delete bmp;
			continue;
		}

		if(!top_frame) // first file/frame build names of other buffers, initialize variables
		{
			CString s;
			s.Format("Pixel Format %x",pixel_format);
			progress.AddString(s);

			smallh=bd.Height;//bmp->GetHeight();
			smallw=bd.Width;//bmp->GetWidth();

			if(!InitializeMemory())
			{
				bmp->UnlockBits(&bd);
				buissy=false;
				return false;
			}

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

//			InitializeMacros();
		} else { // check each file is correct size
			if(((int)bmp->GetHeight()!=smallh) || ((int)bmp->GetWidth()!=smallw))
			{
				MessageBox(progress.m_hWnd,"Frames must all be the same size","Error",MB_OK);
				bmp->UnlockBits(&bd);
				delete bmp;
				continue; // won't fit in a frame
			}
		}

// get pointer to buffer, and name it
		buffer[top_frame].name=name[nf].Left(name[nf].ReverseFind('.'));

		str.Format("(%d) %s",top_frame,buffer[top_frame].name);
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
				p.a=aGOOD; //pi++;
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
			MessageBox(progress.m_hWnd,
				"Input File Limit Reached",
				"File Limit",
				MB_OK);
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

	AfxGetApp()->WriteProfileString("File","Path",path);

	if(depth_flag)
	{
		delete rainbow; rainbow=0;
		depth_flag=false;
		backup_picture_valid=false;
		redraw_picture();
	}

	active_frame2=UNSET;
	special_active=active_frame=display=top_frame-1;

	progress.GetDlgItem(IDC_CANCEL)->EnableWindow(true);

	save_state();

	if(top_frame!=0) this_file="";

//	InitializeMacros();

	return true;
}

bool CCombineDoc::silent_save_out(char *file_path)
{
	display=OUT_BUFFER;

	CString ext=&file_path[strlen(file_path)-3];

	progress.AddString("Save "+buffer[display].name);
	progress.AddString((CString)"  as  "+file_path);

	unsigned long comp;
	unsigned short us[1024];
	int i;
	char *space=(char*)new char[sizeof(EncoderParameters)+1*sizeof(EncoderParameter)];
	EncoderParameters *ep=0;

	unsigned char *out=(unsigned char*)GetBuffer(display);

	Bitmap bm(mag.Width(),mag.Height(),4*w,PixelFormat32bppRGB,out+4*(mag.left+w*mag.top));
	CLSID Clsid;
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

		comp=70;
		GetEncoderClsid(L"image/jpeg",&Clsid);
	}
	else if(ext=="png") GetEncoderClsid(L"image/png",&Clsid);
	else
	{
		FreeBuffer(display);
		return false;
	}

	i=0; while(file_path[i]){us[i]=((unsigned short)file_path[i])&0xff; i++;} us[i]=0;
	bm.Save(us,&Clsid,ep);
	FreeBuffer(display);

	delete space;

	return true;
}
