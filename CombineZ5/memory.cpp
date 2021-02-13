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

struct BUFFER buffer[LAST_BUFFER+1];

struct MEMORY_SLOT memory_slot[LAST_BUFFER+1];

// Memory Masnagement

void CCombineDoc::reverseorder()
{
	if(top_frame<2) return;

	int i,j;
	int framei,framej;

	progress.AddString(IDS_REVERSE);
	progress.AddString("");

	BUFFER temp_buffer;
	for(i=0,j=top_used_frame-1; i<j; i++,j--)
	{
		framei=used_frame[i];
		framej=used_frame[j];

		temp_buffer=buffer[framei];
		buffer[framei]=buffer[framej];
		buffer[framej]=temp_buffer;

		memory_slot[buffer[framei].slot].buffer=framei;
		memory_slot[buffer[framej].slot].buffer=framej;
	}

	refresh_used_list();

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	for(i=0; i<w*h; i++)
	{
		if(depth[i]==fEMPTY) continue;
		depth[i]=((((top_frame-1)<<8)-(depth[i]&dMASK))|(depth[i]&fMASK));
	}
	UpdateBuffer(DEPTH_BUFFER);
}

void CCombineDoc::OnFrameReverseorder() 
{
	if(top_frame<2) return;

	BeginWaitCursor();

	reverseorder();

	redraw_picture();
	EndWaitCursor();
	RefreshScreen();
}

void CCombineDoc::SwapBuffers(int *a, int *b)
{
	int *temp;
	temp=a;
	a=b;
	b=temp;
}

void CCombineDoc::CopyBuffer(int in,int out)
{
	unsigned long i;
	int *bs=(int*)GetBuffer(in);
	int *bd=(int*)GetBuffer(out);
	for(i=0; i<slot_size/4; i++) bd[i]=bs[i];
	FreeBuffer(in);
	UpdateBuffer(out);
}

void CCombineDoc::RemoveBuffer(int b)
{
	if(buffer[b].pointer)
	{
		int s=buffer[b].slot;
		memory_slot[s].buffer=-1;
		memory_slot[s].vacant=true;
	}

	buffer[b].ondisk=false;
	buffer[b].inuse=0;
	buffer[b].name="*** Not Initialized ***";
	buffer[b].pointer=0;
	buffer[b].slot=-1;
	buffer[b].updated=false;
}

void CCombineDoc::ResetBuffer(int b)
{
	buffer[b].ondisk=false;
	buffer[b].inuse=0;
	buffer[b].name="*** Not Initialized ***";
	buffer[b].pointer=0;
	buffer[b].slot=-1;
	buffer[b].updated=false;
}

int CCombineDoc::upsize(int x)
{
	int out,test,extra,border,product;
	int n2,n3,n5,n7;

	border=FRAME_BORDER;
	for(extra=0; extra<2000; extra++)
	{
		out=1;

		n7=n5=n3=n2=0;

		test=x+extra+2*border;

		while((test/2)*2==test){test/=2; out*=2; n2++;};
		if(n2<2) continue;

		while((test/3)*3==test){test/=3; out*=3; n3++;};

		while((test/5)*5==test){test/=5; out*=5; n5++;};

		while((test/7)*7==test){test/=7; out*=7; n7++;};

		product=1;
		while(n7){product*=7; n7--;}
		while(n5){product*=5; n5--;}
		while(n3){product*=3; n3--;}
		if(product>MIN_HALF_SIZE) continue;

		if(test==1) break;
	}
	return out;
}

bool CCombineDoc::ReInitializeMemory()
{
	int maxwh=w; if(h>maxwh) maxwh=h;
	slot_size=4*((w+2)*(h+2));//+4*maxwh); // 4 bytes per pixel, or 1 int, DFT and Depthflow need extra padding
	MEMORYSTATUS ms;
	CString s;
	int slot;

	unsigned long memory_size,i;

	progress.AddString("Filter Initializeation (Please be patient)");

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString wisepath=helppath.Left(helppath.ReverseFind('\\')+1);
	float *buf=new float[slot_size];
	dft.Reset(w,h,wisepath,buf);
	delete buf;
	if(!dft.initialized)
	{
		MessageBox(progress.m_hWnd,"Failed to create DFT Plan","InitializeMemory()",MB_OK);
		return false;
	}

	packed=new unsigned char[slot_size];

	frame_border=(w-smallw)/2;
	i=(h-smallh)/2;
	if(i<(unsigned long)frame_border) frame_border=(unsigned long)i;

	slot=0;
	GlobalMemoryStatus(&ms);
	memory_size=ms.dwAvailPhys;
	while(memory_size>slot_size)
	{
		memory=VirtualAlloc(0,slot_size,MEM_RESERVE,PAGE_READWRITE);
		if(memory!=0)
		{
			memory_slot[slot].buffer=-1;
			memory_slot[slot].pointer=(char*)memory;
			memory_slot[slot].vacant=true;
			slot++;
			if(slot==LAST_BUFFER) break;
		} else break;
		memory_size-=slot_size;
		if(slot==LAST_BUFFER) break;
	}
	while(slot<6)
	{
		memory=VirtualAlloc(0,slot_size,MEM_RESERVE,PAGE_READWRITE);
		if(memory!=0)
		{
			memory_slot[slot].buffer=-1;
			memory_slot[slot].pointer=(char*)memory;
			memory_slot[slot].vacant=true;
			slot++;
		} else break;
	}
	memory=(void*)1;
	max_slots=slot;
	if(max_slots<6)
	{
		MessageBox(progress.m_hWnd,"Insufficient memory","InitializeMemory()",MB_OK);
		FreeMemory();
		return false;
	}

	for(i=0; i<=LAST_BUFFER; i++)
	{
		buffer[i].hidden=false;
		buffer[i].ondisk=false;
		buffer[i].inuse=0;
		buffer[i].name="*** Not Initialized ***";
		buffer[i].pointer=0;
		buffer[i].slot=-1;
		buffer[i].updated=false;
	}

	namebuffers();

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	int *picture=(int*)GetBuffer(PICTURE_BUFFER);
	int *out=(int*)GetBuffer(OUT_BUFFER);
	for(i=0; i<(unsigned long)(w*h); i++)
	{
		depth[i]=fEMPTY;
		picture[i]=wipeout;
		out[i]=aCLEAR<<24;
	}
	UpdateBuffer(DEPTH_BUFFER);
	UpdateBuffer(PICTURE_BUFFER);
	UpdateBuffer(OUT_BUFFER);
	return true;
}

bool CCombineDoc::InitializeMemory()
{
	MEMORYSTATUS ms;
	CString s;
	int slot;

	unsigned long i,memory_size;

	w=upsize(smallw);
	h=upsize(smallh);
	int maxwh=w; if(h>maxwh) maxwh=h;
	slot_size=4*((w+2)*(h+2));//+4*maxwh); // 4 bytes per pixel, or 1 int, DFT and Depthflow need extra padding

	progress.AddString("Filter Initializeation (Please be patient)");
	dft.Destroy();

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString wisepath=helppath.Left(helppath.ReverseFind('\\')+1);
	float *buf=new float[slot_size];
	dft.Reset(w,h,wisepath,buf);
	delete buf;
	if(!dft.initialized)
	{
		MessageBox(progress.m_hWnd,"Failed to create DFT Plan","InitializeMemory()",MB_OK);
		return false;
	}

	packed=new unsigned char[slot_size];

	frame_border=(w-smallw)/2;
	i=(h-smallh)/2;
	if(i<(unsigned long)frame_border) frame_border=(unsigned long)i;

	slot=0;
	GlobalMemoryStatus(&ms);
	memory_size=ms.dwAvailPhys;
	while(memory_size>slot_size)
	{
		memory=VirtualAlloc(0,slot_size,MEM_RESERVE,PAGE_READWRITE);
		if(memory!=0)
		{
			memory_slot[slot].buffer=-1;
			memory_slot[slot].pointer=(char*)memory;
			memory_slot[slot].vacant=true;
			slot++;
		} else break;
		memory_size-=slot_size;
		if(slot==LAST_BUFFER) break;
	}
	memory=(void*)1;
	max_slots=slot;
	if(max_slots<6)
	{
		MessageBox(progress.m_hWnd,"Insufficient memory","InitializeMemory()",MB_OK);
		FreeMemory();
		return false;
	}

	for(i=0; i<=LAST_BUFFER; i++)
	{
		buffer[i].hidden=false;
		buffer[i].ondisk=false;
		buffer[i].inuse=0;
		buffer[i].name="*** Not Initialized ***";
		buffer[i].pointer=0;
		buffer[i].slot=-1;
		buffer[i].updated=false;
	}

	namebuffers();

	int *depth=(int*)GetBuffer(DEPTH_BUFFER);
	int *picture=(int*)GetBuffer(PICTURE_BUFFER);
	int *out=(int*)GetBuffer(OUT_BUFFER);
	for(i=0; i<(unsigned long)(w*h); i++)
	{
		depth[i]=fEMPTY;
		picture[i]=wipeout;
		out[i]=aCLEAR<<24;
	}
	UpdateBuffer(DEPTH_BUFFER);
	UpdateBuffer(PICTURE_BUFFER);
	UpdateBuffer(OUT_BUFFER);
	return true;
}

void CCombineDoc::FreeMemory()
{
	unsigned int slot;
	BeginWaitCursor();

	delete_work();

	for(slot=0; slot<max_slots; slot++)
	{
		if(memory_slot[slot].pointer!=0)
			VirtualFree(memory_slot[slot].pointer,0,MEM_RELEASE);
		memory_slot[slot].pointer=0;
	}

	delete packed;

	if(dft.initialized) dft.Destroy();

	EndWaitCursor();
}

void CCombineDoc::SaveBuffer(int number,CString ext)
{
	if(buffer[number].updated)
	{
		CFile f(
			path+"\\work\\CZ@"+buffer[number].name+"."+ext,
			CFile::modeCreate | CFile::modeWrite);

		if((number>=FIRST_DFT_BUFFER) && (number<=LAST_DFT_BUFFER))
		{
			f.Write(buffer[number].pointer, slot_size);
			f.Close();
		}
		else writefile(&f,(void*)(buffer[number].pointer));

		buffer[number].ondisk=true;
		return;
	}
}

void* CCombineDoc::ReuseBufferAs(int b, CString name)
{
	int s=buffer[b].slot;
	CString p;
	try
	{
		p=path;
		p+="\\work\\";
		p+=buffer[b].name;
		p+=".czb";
		CFile::Remove(p);
	}
	catch(...){;}

	buffer[b].hidden=false;
	buffer[b].inuse=0;
	buffer[b].name=name;
	buffer[b].ondisk=false;
	buffer[b].updated=false;

	return GetBuffer(b);
}

void* CCombineDoc::GetBuffer(int number)
{
	void *address;

	address=buffer[number].pointer;
	if(address)
	{
		buffer[number].inuse++;
		goto ret;
	}

	if(buffer[number].ondisk)
	{
		address=GetBufferFile(number);
		if(address)
		{
			buffer[number].inuse++;
			goto ret;
		}
	}

	address=NewBuffer(number);
	if(address)
	{
		buffer[number].inuse++;
		goto ret;
	}
	MessageBox(progress.m_hWnd,"Failed","GetBuffer() 4",MB_OK);
	AfxAbort();
	return 0;
ret:
	return address;
}

int CCombineDoc::LocateFreeSlot()
{
	unsigned long slot;
	int b;
	bool vacant=false;

// search for vacant slot first
	for(slot=0; slot<max_slots; slot++)
	{
		if(memory_slot[slot].vacant) break;
	}
	if(slot<max_slots)
	{
		void* ret=VirtualAlloc(memory_slot[slot].pointer,slot_size,MEM_COMMIT,PAGE_READWRITE);
		CString s;
		if(ret==0)
		{
			s.Format("ERROR Code %d",GetLastError());
			MessageBox(progress.m_hWnd,s,"VirtualAlloc(MEM_COMMIT)",MB_OK);
			AfxAbort();
		}
		memory_slot[slot].vacant=false;
		return(slot);
	}

	for(slot=0; slot<max_slots; slot++)
	{
		if(memory_slot[slot].buffer==-1) break;
	}
	if(slot<max_slots)
	{
		return(slot);
	}

// no empty slots
// next search for slot who's buffer has not been updated
	for(slot=0; slot<max_slots; slot++)
	{
		b=memory_slot[slot].buffer;
		if(buffer[b].updated) continue;
		if(buffer[b].inuse) continue;
		break;
	}
	if(slot<max_slots)
	{
		b=memory_slot[slot].buffer;
		buffer[b].pointer=0;
		buffer[b].updated=false;
		buffer[b].slot=-1;
		memory_slot[slot].buffer=-1;
		return(slot);
	}

// search for updated buffer's best slot
	for(slot=0; slot<max_slots; slot++)
	{
		b=memory_slot[slot].buffer;
		if(buffer[b].inuse) continue;
		break;
	}
	if(slot<max_slots)
	{
		b=memory_slot[slot].buffer;
		SaveBuffer(b,"czb");
		buffer[b].pointer=0;
		buffer[b].updated=false;
		buffer[b].slot=-1;
		memory_slot[slot].buffer=-1;
		return(slot);
	}
	MessageBox(progress.m_hWnd,"No Memory Slot Available","LocateFreeSlot()",MB_OK);
	AfxAbort();
	return -1;
}

void* CCombineDoc::GetBufferFile(int f)
{
	int slot=LocateFreeSlot();

	if(!buffer[f].ondisk) return 0;

	if(slot!=-1)
	{
		buffer[f].pointer=memory_slot[slot].pointer;
		buffer[f].slot=slot;
		buffer[f].inuse=0;
		buffer[f].updated=false;

		memory_slot[slot].buffer=f;

		CFile file(
			path+"\\work\\CZ@"+buffer[f].name+".czb",
			CFile::modeRead);

		if((f>=FIRST_DFT_BUFFER) && (f<=LAST_DFT_BUFFER))
		{
			file.Read(buffer[f].pointer, slot_size);
			file.Close();
		}
		else readfile(&file,buffer[f].pointer);

		return buffer[f].pointer;
	}
	return 0;
}

void *CCombineDoc::NewBuffer(int number)
{
	int slot=LocateFreeSlot();

	if(slot!=-1)
	{
		buffer[number].pointer=memory_slot[slot].pointer;
		buffer[number].inuse=0;
		buffer[number].slot=slot;
		buffer[number].updated=false;
		buffer[number].hidden=false;
		buffer[number].ondisk=false;

		memory_slot[slot].buffer=number;

		return memory_slot[slot].pointer;
	}
	return 0;
}

void CCombineDoc::UpdateBuffer(int number)
{
	buffer[number].updated=true;
	buffer[number].inuse--;
	if(buffer[number].inuse<0)
	{
		buffer[number].inuse=0;
		MessageBox(progress.m_hWnd,"Buffer freed too many times","UpdateBuffer()",MB_OK);
	}
	return;
}

void CCombineDoc::FreeBuffer(int number) // no need to save anything
{
	buffer[number].inuse--;
	if(!(buffer[number].updated) && !(buffer[number].inuse) && (number>=TEMP_BUFFER))
	{
		memory_slot[buffer[number].slot].buffer=-1;
		memory_slot[buffer[number].slot].vacant=true;
		buffer[number].pointer=0;
		buffer[number].slot=-1;
	}
	if(buffer[number].inuse<0)
	{
		MessageBox(progress.m_hWnd,buffer[number].name+"Freed too many times","FreeBuffer()",MB_OK);
		buffer[number].inuse=0;
	}
}
