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
// combine.h : main header file for the COMBINE application
//

#if !defined(AFX_COMBINE_H__F727C85C_0FC7_4AE2_A011_8E70117C8A67__INCLUDED_)
#define AFX_COMBINE_H__F727C85C_0FC7_4AE2_A011_8E70117C8A67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

//#define W2000

#include "resource.h"       // main symbols

struct BUFFER buffer[];

#define VERSION					0x05000005 // fore save/resume work

#define FRAME_BORDER			2
#define MIN_HALF_SIZE			32
#define	MIN_DEPTHFLOW			64

#define BeginThreads			unsigned int obj,objects; \
								CWinThread *th[32]; \
								HANDLE hand[32]; 

#define SetThreads(w,h)			objects=pam; \
								if(objects==0) objects=1; \
								if(w*h/objects<32*1024) objects=1; \
								if(w<1024) objects=1; \
								if(h<(int)objects) objects=1;

#define StartThreads(func,params) \
								if(objects==1) func(&params[obj]);	\
								else \
								{ \
									th[obj]=AfxBeginThread(func,&params[obj],0,10000,CREATE_SUSPENDED); \
									hand[obj]=th[obj]->m_hThread; \
									th[obj]->m_bAutoDelete=false; \
									th[obj]->ResumeThread(); \
								}

#define EndThreads				if(objects!=1) \
								{ \
									obj=WaitForMultipleObjects(objects,hand,true,INFINITE); \
									for(obj=0; obj<objects; obj++) \
									{ \
										th[obj]->Delete(); \
										CloseHandle(hand[obj]); \
									} \
								}

#define MAX_FRAMES				25000
#define OUT_BUFFER				MAX_FRAMES+0
#define PICTURE_BUFFER			MAX_FRAMES+1
#define BACKUP_PICTURE_BUFFER	MAX_FRAMES+2
#define DEPTH_BUFFER			MAX_FRAMES+3
#define SAVED_DEPTH_BUFFER		MAX_FRAMES+4
#define DFT_RIO_BUFFER			MAX_FRAMES+5
#define DFT_GIO_BUFFER			MAX_FRAMES+6
#define DFT_BIO_BUFFER			MAX_FRAMES+7
#define DFT_RSTORE_BUFFER		MAX_FRAMES+8
#define DFT_GSTORE_BUFFER		MAX_FRAMES+9
#define DFT_BSTORE_BUFFER		MAX_FRAMES+10
#define DFT_FILTER_BUFFER		MAX_FRAMES+11
#define FIRST_DFT_BUFFER		DFT_RIO_BUFFER
#define LAST_DFT_BUFFER			DFT_FILTER_BUFFER
#define PYRAMID_BUFFER			MAX_FRAMES+12 // first Pyramid buffer
#define GRAB_BUFFER				PYRAMID_BUFFER+15
#define TEMP_BUFFER				GRAB_BUFFER+1 // first temporary buffer
#define LAST_BUFFER				TEMP_BUFFER+7 // last possible buffer

#define UNSET			-1 // for active frame 2

#define BORDER			10 // selectin border in some viewing modes

// modes
#define IMPORT			1
#define CZ_SIZE			2
#define PLOT			3
#define COPY			4
#define FILL			5
#define TEMP			6
#define DETAIL			7
#define ISLAND			8
#define STEPS			9
#define PATCH			10
#define JPEG			11
#define LIGHT			12
#define DARK			13
#define HDR_LOG			14
#define HDR_BRIGHT		15
#define HDR_HIA			16
#define PYRAMID_CLEAR	17
#define PYRAMID_MAX		18
#define PYRAMID_OUT		19
#define PYRAMID_DEPTH	20
#define PYRAMID_WEIGHT  21
#define PYRAMID_WOUT	22
#define LOADSPECIAL		23
#define SAVESPECIAL		24

#define RED				SPECIAL+1
#define GREEN			SPECIAL+2
#define BLUE			SPECIAL+3
#define ADD				SPECIAL+4
#define BLEND			SPECIAL+5
#define SHARP			SPECIAL+6
#define BRIGHTNESSX		SPECIAL+8
#define CORRECT			SPECIAL+9
#define CONTRASTX		SPECIAL+11
#define REPLACEDIRTY	SPECIAL+12

#define LOWPASS			FILTER+1
#define HIGHPASS		FILTER+2
#define BANDPASS		FILTER+3
#define BANDSTOP		FILTER+4
#define BUTTERWORTH		FILTER+5
#define	FTHRESHOLD		FILTER+10
#define	FBRIGHTNESS		FILTER+11
#define OUT_NORMAL		1
#define OUT_BRIGHT		2
#define OUT_THRESHOLD	3
#define BRIGHTNESS		10
#define THRESHOLD		11
#define CENTRE			5
#define CUTOFF			4
#define ORDER			3
#define DISPLACEMENT	2
#define WIDTH			1

// for frame
#define aGOOD			1
#define aCLEAR			2
#define aHORIZONTAL		4
#define aVERTICAL		8
#define aAMB			16 // AMBIGUOUS

// for flags and depthmap
#define dMASK			0x0000ffff
#define fMASK			0xffff0000
#define fEMPTY			0x00010000 // Empty pixel
#define fFILLED			0x00020000 // Filled pixel
#define fFIXED			0x00040000 // Fixed pixel
#define fSPECIAL_FIXED	0x00080000
#define fTEMP			0x00100000 // Special Mode Fixed pixel

#define MAC_PAUSE		0x00000001
#define MAC_TONAME		0x00000002
#define MAC_SUBP1		0x00000010
#define MAC_SUBP2		0x00000020
#define MAC_SUBP3		0x00000040
#define MAC_P1V(flags)	(((flags&0x000f0000)>>12)+((flags&0xf0000000)>>28))
#define MAC_P2V(flags)	(((flags&0x0000f000)>> 8)+((flags&0x0f000000)>>24))
#define MAC_P3V(flags)	(((flags&0x00000f00)>> 4)+((flags&0x00f00000)>>20))

// Default reserved memory for movies etc.
#define DEFAULT_RESERVE 0

union PIXEL
{
	unsigned long abgr;
	struct {unsigned char b,g,r,a;};
};

struct CELL
{
	int frame,colour_range;
	int rsum;
	int gsum;
	int bsum;
	bool good;
};

/////////////////////////////////////////////////////////////////////////////
// CCombineApp:
// See combine.cpp for the implementation of this class
//

class CCombineApp : public CWinApp
{
public:
	static UINT DoCommandsT(LPVOID param);
	bool DoCommands(LPVOID doc);
	wchar_t path_param[1024];
	wchar_t macro_param[256];
	wchar_t save_param[1024];
	bool crop_switch;
	bool rectangle_switch;
	bool work_switch;
	bool quit_switch;
	bool depth_switch;
	bool debug_switch;
	bool clean_switch;
	bool suppress_switch;
	CCombineApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCombineApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CCombineApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	int jcomp;
	afx_msg void OnFileSavedebuginformation();
	afx_msg void OnUpdateFileSavedebuginformation(CCmdUI *pCmdUI);
	afx_msg void OnHelpCombinezphelp();
};

extern unsigned int pam;

extern CCombineApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBINE_H__F727C85C_0FC7_4AE2_A011_8E70117C8A67__INCLUDED_)
