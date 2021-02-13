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
#include "combineDoc.h"
#include ".\combine.h"

#include "Progress.h"

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
	ON_COMMAND(ID_FILE_SAVEDEBUGINFORMATION, OnFileSavedebuginformation)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEDEBUGINFORMATION, OnUpdateFileSavedebuginformation)
	ON_COMMAND(ID_HELP_COMBINEZPHELP, OnHelpCombinezphelp)
END_MESSAGE_MAP()

CCombineApp::CCombineApp()
: jcomp(0)
{
}

extern CCombineApp theApp;

int jcomp;

unsigned int pam;

class Commands:public CCommandLineInfo
{
	void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);
};

void Commands::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
	int i,n=0;
	jcomp=70;
	if(!bFlag)
	{
		if(lpszParam[0]=='+')
		{
			if((lpszParam[1]=='j') || (lpszParam[1]=='J'))
			{
				for(i=2; i<5; i++)
				{
					if(!isdigit(lpszParam[i])) break;
					n=10*n+(lpszParam[i]-'0');
				}
				jcomp=n;
				return;
			}
		}

		if(!theApp.path_param[0])
		{
//MessageBox(0,lpszParam,L"",MB_OK);
			wcscpy(theApp.path_param,lpszParam);
			return;
		}

		if(!theApp.macro_param[0])
		{
			wcscpy(theApp.macro_param,lpszParam);
			return;
		}

		if(!theApp.save_param[0])
		{
			wcscpy(theApp.save_param,lpszParam);
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
		case 'd':
			theApp.depth_switch=true;
			return;
		case 'k':
			theApp.clean_switch=true;
			return;
		case 'u':
			theApp.suppress_switch=true;
			return;
		}
	}
	CString s2;
	s2.LoadString(IDS_MOVEFRAME2);
	MessageBox(0,lpszParam,s2,MB_OK);
}

BOOL CCombineApp::InitInstance()
{
#ifdef _AFXDLL
//	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

	SetRegistryKey(_T("CombineZP by AH"));

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
	depth_switch=false;
	debug_switch=false;
	suppress_switch=false;
	clean_switch=false;
	Commands cmdInfo;
	ParseCommandLine(cmdInfo);

	if(!ProcessShellCommand(cmdInfo))
		return false;

	m_pMainWnd->ShowWindow(SW_MAXIMIZE);
	m_pMainWnd->SetForegroundWindow();
	m_pMainWnd->BringWindowToTop();
	m_pMainWnd->UpdateWindow();

	POSITION docPos=pDocTemplate->GetFirstDocPosition();
	CCombineDoc *doc=(CCombineDoc*)(pDocTemplate->GetNextDoc(docPos));

	doc->InitializeMacros();

	if(((CMainFrame*)m_pMainWnd)->simple_mode) m_pMainWnd->SetMenu(0);

	if((path_param[0]) || (macro_param[0]))
		AfxBeginThread(DoCommandsT,doc);
	else 
	{
		m_pMainWnd->SetFocus();
		m_pMainWnd->SetForegroundWindow();
		m_pMainWnd->BringWindowToTop();
		m_pMainWnd->UpdateWindow();
	}

	unsigned long sam;
	if(GetProcessAffinityMask(GetCurrentProcess(),(DWORD*)&pam,&sam))
	{
		int bit,bits=0;
		for(bit=0; bit<31; bit++)
			bits=bits+(((long)pam>>bit)&1);
		pam=bits;
	}
	else
		pam=1;
	AfxGetApp()->GetProfileInt(L"Cores",L"CPU",pam);

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
			if(macro_param[0])
				theApp.PostThreadMessage(WM_QUIT,0,0);
		}
	}
	if(macro_param[0])
	{
		if(!doc->silent_macro(macro_param))
		{
			CString s1,s2;
			s1.LoadString(IDS_ERROREXEC); s2.LoadString(AFX_IDS_APP_TITLE);
			message.Format(s1,macro_param);
			MessageBox(0,message,s2,MB_OK);
			theApp.PostThreadMessage(WM_QUIT,0,0);
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
		if(clean_switch)
		{
			doc->mag=doc->goodrect;
		}
		if(save_param[0])
		{
			if(!doc->silent_save_out(save_param))
			{
				CString s1,s2;
				s1.LoadString(IDS_SAVEERROR); s2.LoadString(AFX_IDS_APP_TITLE);
				message.Format(s1,save_param);
				MessageBox(0,s1,s2,MB_OK);
				theApp.PostThreadMessage(WM_QUIT,0,0);
			}
			if(theApp.depth_switch)
			{
				if(!doc->silent_save_depth(save_param))
				{
					CString s1,s2;
					s1.LoadString(IDS_SAVEERROR); s2.LoadString(AFX_IDS_APP_TITLE);
					message.Format(s1,save_param);
					MessageBox(0,s1,s2,MB_OK);
					theApp.PostThreadMessage(WM_QUIT,0,0);
				}
			}
		}
		if(rectangle_switch)
			doc->silent_export_rectangles();
		if(work_switch)
			doc->save_work();
		if(quit_switch)
		{
			theApp.PostThreadMessage(WM_QUIT,0,0);
			doc->delete_work();
		}
	}

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
public:
	afx_msg void OnMovieExportmovieframes();
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

void CCombineApp::OnFileSavedebuginformation()
{
	POSITION p=theApp.GetFirstDocTemplatePosition();
	CDocTemplate *t=theApp.GetNextDocTemplate(p);
	POSITION docPos=t->GetFirstDocPosition();
	CCombineDoc *doc=(CCombineDoc*)(t->GetNextDoc(docPos));

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString debug_path=helppath.Left(helppath.ReverseFind('\\')+1);
	debug_path+="CZP_debug.txt";

	CString s1,s2;
	s1.LoadString(IDS_DEBUGPATH); s2.LoadString(IDS_DEBUGINFO);
	CString s; s.Format(s1,debug_path);
	MessageBox(0,s,s2,MB_OK);

	CFile f;

	if(debug_switch) debug_switch=false;
	else
	{
		debug_switch=true;

		f.Open(debug_path,CFile::modeWrite|CFile::modeCreate);
		f.Close();

		doc->progress.debug_catchup();
	}
}

void CCombineApp::OnUpdateFileSavedebuginformation(CCmdUI *pCmdUI)
{
	if(debug_switch) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

void CCombineApp::OnHelpCombinezphelp()
{
	// TODO: Add your command handler code here
}
