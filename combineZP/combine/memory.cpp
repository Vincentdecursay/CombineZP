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

struct cb_Params
{
	int index,step;
	int size;
	int *in,*out;
} cb_params[32];

UINT CCombineDoc::cb(LPVOID param)
{
	cb_Params *pp=(cb_Params *)param;
	int index=pp->index;
	int step=pp->step;
	int size=pp->size;
	int *in=pp->in;
	int *out=pp->out;

	int i;

	for(i=index; i<size; i+=step) out[i]=in[i];

	return 0;
}

void CCombineDoc::CopyBuffer(int in,int out)
{
//	unsigned long i;
	int *bs=(int*)GetBuffer(in);
	int *bd=(int*)GetBuffer(out);

	BeginThreads
	SetThreads(w,h)

	for(obj=0; obj<objects; obj++)
	{
		cb_params[obj].index=obj;
		cb_params[obj].step=objects;
		cb_params[obj].size=slot_size/4;
		cb_params[obj].in=bs;
		cb_params[obj].out=bd;
		StartThreads(cb,cb_params);
	}
	EndThreads

	FreeBuffer(in);
	UpdateBuffer(out);
}

void CCombineDoc::RemoveBuffer(int b)
{
	if(buffer[b].pointer)
	{
		int s=buffer[b].slot;
		memory_slot[s].buffer=-1;
	}

	buffer[b].ondisk=false;
	buffer[b].inuse=0;
	buffer[b].name="*** Not Initialized ***";
	buffer[b].pointer=0;
	buffer[b].slot=-1;
	buffer[b].updated=false;
	if(b<MAX_FRAMES) buffer[b].age=0x0;
	else buffer[b].age=0xffffffff;
}

void CCombineDoc::ResetBuffer(int b)
{
	buffer[b].ondisk=false;
	buffer[b].inuse=0;
	buffer[b].pointer=0;
	buffer[b].slot=-1;
	buffer[b].updated=false;
	if(b<MAX_FRAMES) buffer[b].age=0x0;
	else buffer[b].age=0xffffffff;
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
	CString s1,s2;

	namebuffers();

	int maxwh=w; if(h>maxwh) maxwh=h;
	slot_size=4*((w+16)*(h+16));
	packed=new unsigned char[slot_size+slot_size/4];
	MEMORYSTATUS ms;
	CString s;
	int slot;

	unsigned long i;

	progress.AddString(IDS_STRING611);

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString wisepath=helppath.Left(helppath.ReverseFind('\\')+1);
	float *buf=new float[slot_size];
	dft.Reset(w,h,wisepath,buf);
	delete [] buf;
	if(!dft.initialized)
	{
		CString s1,s2;
		s1.LoadString(IDS_ERRORDFTPLAN); s2.LoadString(IDS_MEMORY);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		return false;
	}

	frame_border=(w-smallw)/2;
	i=(h-smallh)/2;
	if(i<(unsigned long)frame_border) frame_border=(unsigned long)i;

	slot=0;
	GlobalMemoryStatus(&ms);
	if(theApp.debug_switch)
	{
		CString s;
		s.Format(L"Avail Page File %d Mb",ms.dwAvailPageFile/1000000); progress.AddString(s);
		s.Format(L"Total Page File %d Mb",ms.dwTotalPageFile/1000000); progress.AddString(s);
		s.Format(L"Avail Virtual %d Mb",ms.dwAvailVirtual/1000000); progress.AddString(s);
		s.Format(L"Total Virtual %d Mb",ms.dwTotalVirtual/1000000); progress.AddString(s);
		s.Format(L"Avail Phys %d Mb",ms.dwAvailPhys/1000000); progress.AddString(s);
		s.Format(L"Total Phys %d Mb",ms.dwTotalPhys/1000000); progress.AddString(s);
		s.Format(L"Memory Load %d%%",ms.dwMemoryLoad); progress.AddString(s);
		s.Format(L"Slot Size %d",slot_size); progress.AddString(s);
	}

	int m;
	SIZE_T reserve;
	reserve=1000000L*AfxGetApp()->GetProfileInt(L"Memory",L"Reserve",DEFAULT_RESERVE);
	if((ms.dwAvailVirtual<reserve+7*slot_size) && (reserve!=0))
	{
		s1.LoadString(IDS_STRING647); s2.LoadString(IDS_STRING648);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		reserve=0;
	}

	void *rmem=0;
	if(reserve!=0)
	{
		rmem=VirtualAlloc(0,reserve,MEM_COMMIT,PAGE_READWRITE);
		if(rmem==0)
			progress.MessageBox(L"Could not Reserve Memory in a single block.   It may not be possible to make large movies, if so try reducing the amount Reserved.",L"Memory Allocation",MB_OK);
	}

	m=ms.dwAvailVirtual-reserve;
	if(m>=(int)slot_size)
	{
		for(m=ms.dwAvailVirtual-reserve; m>0; m-=slot_size)
		{
			memory=VirtualAlloc(0,slot_size,MEM_RESERVE,PAGE_READWRITE);
			if(memory!=0)
			{
				memory_slot[slot].buffer=-1;
				memory_slot[slot].pointer=(char*)memory;
				memory_slot[slot].vacant=true;
				slot++;
				if(slot>LAST_BUFFER) break;
			} else break;
		}
	}
	memory=(void*)1;
	max_slots=slot;
	if(max_slots<7)
	{
		s1.LoadString(IDS_ERRORINSUFMEM); s2.LoadString(IDS_MEMORY);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		FreeMemory();
		return false;
	}

	if(rmem!=0)
		VirtualFree(rmem,0,MEM_RELEASE);

	for(i=0; i<=LAST_BUFFER; i++)
		ResetBuffer(i);

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
	CString s1,s2;
	namebuffers();

	MEMORYSTATUS ms;
	CString s;
	int slot;

	unsigned long i;

	w=upsize(smallw);
	h=upsize(smallh);
	int maxwh=w; if(h>maxwh) maxwh=h;
	slot_size=4*((w+16)*(h+16));
	packed=new unsigned char[slot_size+slot_size/4];

	progress.AddString(IDS_STRING611);
	dft.Destroy();

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString wisepath=helppath.Left(helppath.ReverseFind('\\')+1);
	float *buf=new float[slot_size];
	dft.Reset(w,h,wisepath,buf);
	delete [] buf;
	if(!dft.initialized)
	{
		CString s1,s2;
		s1.LoadString(IDS_ERRORDFTPLAN); s2.LoadString(IDS_MEMORY);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		return false;
	}

	frame_border=(w-smallw)/2;
	i=(h-smallh)/2;
	if(i<(unsigned long)frame_border) frame_border=(unsigned long)i;

	unsigned int m;
	SIZE_T reserve;
	GlobalMemoryStatus(&ms);
	if(theApp.debug_switch)
	{
		CString s;
		s.Format(L"Avail Page File %d Mb",ms.dwAvailPageFile/1000000); progress.AddString(s);
		s.Format(L"Total Page File %d Mb",ms.dwTotalPageFile/1000000); progress.AddString(s);
		s.Format(L"Avail Virtual %d Mb",ms.dwAvailVirtual/1000000); progress.AddString(s);
		s.Format(L"Total Virtual %d Mb",ms.dwTotalVirtual/1000000); progress.AddString(s);
		s.Format(L"Avail Phys %d Mb",ms.dwAvailPhys/1000000); progress.AddString(s);
		s.Format(L"Total Phys %d Mb",ms.dwTotalPhys/1000000); progress.AddString(s);
		s.Format(L"Memory Load %d%%",ms.dwMemoryLoad); progress.AddString(s);
		s.Format(L"Slot Size %d",slot_size); progress.AddString(s);
	}

	slot=0;
	void *rmem=0;
	reserve=1000000L*AfxGetApp()->GetProfileInt(L"Memory",L"Reserve",DEFAULT_RESERVE);
	if((ms.dwAvailVirtual<reserve+7*slot_size) && (reserve!=0))
	{
		s1.LoadString(IDS_STRING647); s2.LoadString(IDS_STRING648);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		reserve=0;
	}
	if(reserve!=0)
	{
		rmem=VirtualAlloc(0,reserve,MEM_COMMIT,PAGE_READWRITE);
		if(rmem==0)
			progress.MessageBox(L"Could not Reserve Memory in a single block.   It may not be possible to make large movies, if so try reducing the amount Reserved.",L"Memory Allocation",MB_OK);
	}
	m=ms.dwAvailVirtual-reserve;
	if(m>=slot_size)
	{
		for(m=ms.dwAvailVirtual-reserve; m>slot_size; m-=slot_size)
		{
			memory=VirtualAlloc(0,slot_size,MEM_RESERVE,PAGE_READWRITE);
			if(memory!=0)
			{
				memory_slot[slot].buffer=-1;
				memory_slot[slot].pointer=(char*)memory;
				memory_slot[slot].vacant=true;
				slot++;
				if(slot>LAST_BUFFER) break;
			} else break;
		}
	}
	memory=(void*)1;
	max_slots=slot;
	if(max_slots<7)
	{
		s1.LoadString(IDS_ERRORINSUFMEM); s2.LoadString(IDS_MEMORY);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		FreeMemory();
		return false;
	}

	if(rmem!=0)
		VirtualFree(rmem,0,MEM_RELEASE);

	for(i=0; i<=LAST_BUFFER; i++)
		ResetBuffer(i);

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
	int slot;

	unsigned long i;

	delete_work();

	if(packed) delete [] packed; packed=0;

	for(slot=0; slot<(int)max_slots; slot++)
	{
		if(memory_slot[slot].pointer!=0)
		{
			if(buffer[memory_slot[slot].buffer].inuse!=0)
				progress.AddString("*** INTERNAL ERROR *** FreeMemory() - Buffer in use");
		}
		if(memory_slot[slot].pointer!=0)
			VirtualFree(memory_slot[slot].pointer,0,MEM_RELEASE);
		memory_slot[slot].buffer=-1;
		memory_slot[slot].pointer=0;
		memory_slot[slot].vacant=false;
	}

	for(i=0; i<=LAST_BUFFER; i++)
	{
		buffer[i].hidden=false;
		buffer[i].inuse=0;
		buffer[i].name="*** Not Initialized ***";
		buffer[i].ondisk=false;
		buffer[i].pointer=0;
		buffer[i].slot=-1;
		buffer[i].updated=false;
	}

	if(dft.initialized) dft.Destroy();

	slot_size=0;
	buffer_count=0;
	memory=0;
	max_slots=0;
	smallw=smallh=w=h=0;
	plast_level=0;
}

void CCombineDoc::SaveBuffer(int number,CString ext)
{
	if(buffer[number].updated)
	{
		str1.LoadString(IDS_STRING612);
		CFile f;
		CString s;
		s=work_path+L"\\"+str1+buffer[number].name+L"."+ext;
		f.Open(
			s,
			CFile::modeCreate | CFile::modeWrite | CFile::osSequentialScan);

		writepfile(&f,(void*)(buffer[number].pointer));
		return;

	}
}

void* CCombineDoc::ReuseBufferAs(int b, CString name)
{
	int s=buffer[b].slot;
	memory_slot[s].buffer=-1;

	CString p;
	try
	{
		p=work_path;
		str1.LoadString(IDS_STRING613);
		p+=str1;
		p+=buffer[b].name;
		p+=".czb";
		CFile::Remove(p);
	}
	catch(...){;}

	ResetBuffer(b);

	buffer[b].name=name;

	return GetBuffer(b);
}

void* CCombineDoc::GetBuffer(int number)
{
	void *address;

	int i;
	for(i=0; i<LAST_BUFFER; i++)
	{
		if(buffer[i].pointer)
		{
			if((i<MAX_FRAMES) && (buffer[i].age>0))  buffer[i].age--;
			if((i>=MAX_FRAMES) && (buffer[i].age<0xffffffff)) buffer[i].age++;
		}
	}

	address=buffer[number].pointer;
	if(address)
	{
		buffer[number].inuse++;
		if(number<MAX_FRAMES) buffer[number].age=0xffffffff;
		else buffer[number].age=0;
		buffer_count++;
		goto ret;
	}

	if(buffer[number].ondisk)
	{
		address=GetBufferFile(number);
		if(address)
		{
			buffer[number].inuse++;
			if(number<MAX_FRAMES) buffer[number].age=0xffffffff;
			else buffer[number].age=0;
			buffer_count++;
			goto ret;
		}
	}

	address=NewBuffer(number);
	if(address)
	{
		buffer[number].inuse++;
		if(number<MAX_FRAMES) buffer[number].age=0xffffffff;
		else buffer[number].age=0;
		buffer_count++;
		goto ret;
	}

	if(theApp.debug_switch)
	{
		CString s;
		s.Format(L"%d Buffers in use",buffer_count);
		progress.AddString(s);
	}
	return 0;
ret:
	if(theApp.debug_switch)
	{
		CString s;
		if(buffer_count>25)
		{
			s.Format(L"%d Buffers in use",buffer_count);
			progress.AddString(s);
		}
	}
	return address;
}

int CCombineDoc::LocateFreeSlot()
{
	int slot=0,i;
	int b;
	bool vacant=false;
	void* ret;
	CString s;
	unsigned int oldest;

// search for vacant slot first, needs allocating
	for(slot=0; slot<(int)max_slots; slot++)
	{
		if((memory_slot[slot].vacant) && (memory_slot[slot].pointer!=0)) break;
	}
	if(slot<(int)max_slots)
	{
		ret=VirtualAlloc(memory_slot[slot].pointer,slot_size,MEM_COMMIT,PAGE_READWRITE);
		if(ret==0)
		{
			CString s1,s2;
			s1.LoadString(IDS_ERRORMALLOC); s2.LoadString(IDS_MEMORY);
			s.Format(s1,GetLastError());
			MessageBox(progress.m_hWnd,s,s2,MB_OK);
			AfxAbort();
		}
		memory_slot[slot].vacant=false;
		goto OK;
	}
// search for slot with pointer but no buffer
	for(slot=0; slot<(int)max_slots; slot++)
	{
		if((memory_slot[slot].buffer==-1) && (memory_slot[slot].pointer!=0)) break;
	}
	if(slot<(int)max_slots)
	{
		goto OK;
	}
// Try to make an empty slot
	for(slot=(int)max_slots-1; slot>=0; slot--)
	{
		if(memory_slot[slot].buffer!=-1) continue;
		if(memory_slot[slot].pointer!=0) continue;
		break;
	}
	if(slot>-1)
	{
		memory=0;
		memory=VirtualAlloc(0,slot_size,MEM_RESERVE,PAGE_READWRITE);
		if(memory!=0)
		{
			memory_slot[slot].buffer=-1;
			memory_slot[slot].pointer=(char*)memory;
			memory_slot[slot].vacant=true;

			ret=VirtualAlloc(memory,slot_size,MEM_COMMIT,PAGE_READWRITE);
			if(ret==0)
			{
				CString s1,s2;
				s1.LoadString(IDS_ERRORMALLOC); s2.LoadString(IDS_MEMORY);
				s.Format(s1,GetLastError());
				MessageBox(progress.m_hWnd,s,s2,MB_OK);
				AfxAbort();
			}
			goto OK;
		}
	}
// last resort reuse a buffer
	oldest=0;
	slot=-1;
	for(i=0; i<(int)max_slots; i++)
	{
		if(memory_slot[i].pointer==0) continue;
		b=memory_slot[i].buffer;
		if(b==-1) continue;
		if(buffer[b].updated) continue;
		if(buffer[b].inuse==0)
		{
			if(buffer[b].age>oldest)
			{
				slot=i;
				oldest=buffer[b].age;
			}
		}
	}
	if(slot==-1)
	{
		for(i=0; i<(int)max_slots; i++)
		{
			if(memory_slot[i].pointer==0) continue;
			b=memory_slot[i].buffer;
			if(b==-1) continue;
			if(buffer[b].inuse==0)
			{
				if(buffer[b].age>oldest)
				{
					slot=i;
					oldest=buffer[b].age;
				}
			}
		}
	}
	if(slot!=-1)
	{
		b=memory_slot[slot].buffer;
		if(buffer[b].updated)
		{
			SaveBuffer(b,"czb");
			buffer[b].updated=false;
			buffer[b].ondisk=true;
		}
		buffer[b].pointer=0;
		buffer[b].slot=-1;
		memory_slot[slot].buffer=-1;
		goto OK;
	}
OK:
	if((slot==(int)max_slots) || (slot==-1))
	{
		slot=-1;
		CString s1,s2;
		s1.LoadString(IDS_ERRORSLOTS); s2.LoadString(IDS_MEMORY);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
	}
	return slot;
}

void* CCombineDoc::GetBufferFile(int f)
{
	if(!buffer[f].ondisk) return 0;

	int slot;
	if(buffer[f].pointer==0)
	{
		buffer[f].inuse++;
		slot=LocateFreeSlot();
		buffer[f].inuse--;
	}
	else
		slot=buffer[f].slot;

	if(slot!=-1)
	{
		buffer[f].pointer=memory_slot[slot].pointer;
		buffer[f].slot=slot;
		buffer[f].updated=false;

		memory_slot[slot].buffer=f;

		str1.LoadString(IDS_STRING612);
		CFile file(
			work_path+"\\"+str1+buffer[f].name+".czb",
			CFile::modeRead | CFile::osSequentialScan);

		readpfile(&file,buffer[f].pointer);

		file.Close();

		if(theApp.debug_switch)
		{
			progress.AddString(buffer[f].name);
			progress.AddString(L"LOADED");
		}

		return buffer[f].pointer;
	}
	return 0;
}

void *CCombineDoc::NewBuffer(int number)
{
//	buffer[number].inuse++;
	int slot=LocateFreeSlot();
	if(slot!=-1)
	{
		buffer[number].pointer=memory_slot[slot].pointer;
		buffer[number].inuse=0;
		buffer[number].slot=slot;
		buffer[number].updated=false;
		buffer[number].hidden=false;
		buffer[number].ondisk=false;
		if(number<MAX_FRAMES) buffer[number].age=0x0;
		else buffer[number].age=0xffffffff;

		memory_slot[slot].buffer=number;

		return memory_slot[slot].pointer;
	} else {
		CString s1,s2;
		s1.LoadString(IDS_ERRORSLOTS); s2.LoadString(IDS_MEMORY);
		MessageBox(0,s1,s2,MB_OK);
	}
	return 0;
}

void CCombineDoc::UpdateBuffer(int number)
{
	buffer[number].inuse--;
	buffer[number].updated=true;
	buffer_count--;
	if(buffer[number].inuse<0)
	{
		buffer[number].inuse=0;
		progress.AddString("*** INTERNAL ERROR *** UpdateBuffer() - Buffer freed too many times");
		buffer_count--;
		if(theApp.debug_switch)
		{
			CString s;
			if(buffer_count>5)
			{
				s.Format(L"%d Buffers in use",buffer_count);
				progress.AddString(s);
			}
			if(buffer_count==0) Dump();
		}
		return;
	}
	if(theApp.debug_switch)
	{
		CString s;
		if(buffer_count>5)
		{
			s.Format(L"%d Buffers in use",buffer_count);
			progress.AddString(s);
		}
		if(buffer_count==0) Dump();
	}
}

void CCombineDoc::FreeBuffer(int number) // no need to save anything
{
	buffer_count--;
	if(theApp.debug_switch)
	{
		CString s;
		if(buffer_count>5)
		{
			s.Format(L"%d Buffers in use",buffer_count);
			progress.AddString(s);
		}
		if(buffer_count==0) Dump();
	}
	buffer[number].inuse--;
	if(buffer[number].inuse<0)
	{
		progress.AddString("*** INTERNAL ERROR *** FreeBuffer() - Buffer freed too many times");
		buffer[number].inuse=0;
		return;
	}
}

bool CCombineDoc::ReleaseASlot()
{
	int bnumber;
	int slot;

	if(max_slots<7) return false;

	for(slot=0; slot<(int)max_slots; slot++)
	{
		if(memory_slot[slot].buffer!=-1) continue;
		if(!memory_slot[slot].vacant) continue;
		if(memory_slot[slot].pointer==0) continue;
		break;
	}
	if(slot!=(int)max_slots)
	{
		VirtualFree(memory_slot[slot].pointer,0,MEM_RELEASE);
		memory_slot[slot].pointer=0;
		memory_slot[slot].vacant=false;
		return true;
	}

	for(bnumber=0; bnumber<LAST_BUFFER; bnumber++)
	{
		if(buffer[bnumber].inuse>0) continue;
		if(buffer[bnumber].pointer==0) continue;
		if(buffer[bnumber].slot==-1) continue;
		break;
	}
	if(bnumber==LAST_BUFFER) return false;

	if(buffer[bnumber].updated)
	{
		SaveBuffer(bnumber,"czb");
		buffer[bnumber].ondisk=true;
	}

	slot=buffer[bnumber].slot;

	VirtualFree(memory_slot[slot].pointer,0,MEM_RELEASE);

	memory_slot[slot].pointer=0;
	memory_slot[slot].buffer=-1;
	memory_slot[slot].vacant=false;

	buffer[bnumber].pointer=0;
	buffer[bnumber].slot=-1;
	buffer[bnumber].updated=false;
	if(bnumber<MAX_FRAMES) buffer[bnumber].age=0;
	else buffer[bnumber].age=0xffffffff;

	max_slots--;

	return true;
}
