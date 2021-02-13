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
#include "combineDoc.h"
#include "combine.h"

#include "MainFrm.h"
#include "combineView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CCombineApp, CWinApp)
	//{{AFX_MSG_MAP(CCombineApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

CCombineApp::CCombineApp()
{
}

CCombineApp theApp;

class Commands:public CCommandLineInfo
{
	void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);
};

void Commands::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
	if(!bFlag)
	{
		if(!theApp.path_param[0])
		{
			strcpy(theApp.path_param,lpszParam);
			return;
		}

		if(!theApp.macro_param[0])
		{
			strcpy(theApp.macro_param,lpszParam);
			return;
		}

		if(!theApp.save_param[0])
		{
			strcpy(theApp.save_param,lpszParam);
			return;
		}
	} else {
		switch(lpszParam[0])
		{
		case 'c':
			theApp.crop_switch=true;
			return;
		case 'r':
			theApp.rectangle_switch=true;
			return;
		case 'w':
			theApp.work_switch=true;
			return;
		case 'q':
			theApp.quit_switch=true;
			return;
		}
	}
	MessageBox(0,lpszParam,"CombineZ - Unrecognised Parameter",MB_OK);
}

BOOL CCombineApp::InitInstance()
{
#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

	SetRegistryKey(_T("CombineZ by AH"));

	LoadStdProfileSettings();

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CCombineDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CCombineView));
	AddDocTemplate(pDocTemplate);

	path_param[0]=0;
	macro_param[0]=0;
	save_param[0]=0;
	crop_switch=false;
	rectangle_switch=false;
	work_switch=false;
	quit_switch=false;
	Commands cmdInfo;
	ParseCommandLine(cmdInfo);

	if(!ProcessShellCommand(cmdInfo))
		return false;

	m_pMainWnd->ShowWindow(SW_MAXIMIZE);
	m_pMainWnd->UpdateWindow();
	m_pMainWnd->ShowWindow(SW_HIDE);

	POSITION docPos=pDocTemplate->GetFirstDocPosition();
	CCombineDoc *doc=(CCombineDoc*)(pDocTemplate->GetNextDoc(docPos));

	doc->InitializeMacros();

	AfxBeginThread(DoCommandsT,doc);

	return true;
}

UINT CCombineApp::DoCommandsT(LPVOID param)
{
	return theApp.DoCommands(param);
}

bool CCombineApp::DoCommands(LPVOID param)
{
	CCombineDoc *doc=(CCombineDoc *)param;
	CString message;
	if(path_param[0])
	{
		doc->folder_path=path_param;
		if(!doc->silent_open())
		{
			message.Format("Failed to Open any Picture Files in the folder at %s",path_param);
			MessageBox(0,message,"CombineZ",MB_OK);
			theApp.PostThreadMessage(WM_QUIT,0,0);
			return false;
		}
		if(macro_param[0])
		{
			AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
			if(!doc->silent_macro(macro_param))
			{
				message.Format("Error Executing %s",macro_param);
				MessageBox(0,message,"CombineZ",MB_OK);
				theApp.PostThreadMessage(WM_QUIT,0,0);
			}
		}
		if(crop_switch)
		{
			CRect original(
				doc->frame_border,
				doc->frame_border,
				doc->frame_border+doc->smallw,
				doc->frame_border+doc->smallh);
			doc->mag=original;
		}
		if(save_param[0])
		{
			if(!doc->silent_save_out(save_param))
			{
				message.Format("Could not Save %s",save_param);
				MessageBox(0,message,"CombineZ",MB_OK);
				theApp.PostThreadMessage(WM_QUIT,0,0);
			}
		}
		if(rectangle_switch)
			doc->silent_export_rectangles();
		if(work_switch)
			doc->save_work();
	}
	if(quit_switch)
		theApp.PostThreadMessage(WM_QUIT,0,0);

	m_pMainWnd->ShowWindow(SW_MAXIMIZE);
	m_pMainWnd->SetFocus();
	m_pMainWnd->SetForegroundWindow();
	m_pMainWnd->UpdateWindow();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CCombineApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CCombineApp message handlers


BOOL CCombineApp::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CWinApp::PreTranslateMessage(pMsg);
}
