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

#include "combine.h"
#include "MainFrm.h"
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
#include "GotoFrame.h"
#include "combinedoc.h"

#include "ChooseExtCommand.h"
#include "ExtCommand.h"

#include ".\combinedoc.h"
#include ".\chooseextcommand.h"
#include ".\extcommand.h"

struct CZExtCommand extcommand[13];
HANDLE command_handle[20];

void CCombineDoc::DoExtCom(int com, int p1, int p2, int p3)
{
	DWORD exitcode=0;
	int c;

	DWORD timeout=250;

	c=com;
	if(command_handle[c]!=0)
	{
		if(GetExitCodeProcess(command_handle[c],&exitcode)!=0)
		{
			if(exitcode==STILL_ACTIVE) return;
			CloseHandle(command_handle[c]);
		} else CloseHandle(command_handle[c]);
	}

	command_handle[c]=0;

	if((extcommand[com].program[0]==0) && com==12)
		OnAcquireimageCamerasettings();

	if(com==12)
	{
		CString s;
		s.Format(L"%s %s",extcommand[com].program,extcommand[com].line);
		_wspawnlp(_P_NOWAIT,extcommand[com].program,s,0);
		return;
	}

	CString comline,compath;
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString comdir=helppath.Left(helppath.ReverseFind('\\')+1);

	compath=comdir+extcommand[com].program;
	if(com==9)
	{
		compath=extcommand[com].program;

		comline.Format(
			L"\"%s\" \"%s\\output\\%s.jpg\" %s",
			extcommand[com].program,
			path,
			buffer[active_frame].name,
			extcommand[com].line);
		timeout=0;
	} else {
		compath+=L".exe";
		if(com<9)
		{
			comline.Format(
				L"\"%s.exe\" %s %d %d %d",
				extcommand[com].program,
				extcommand[com].line,
				p1,p2,p3);
		} else {
			if(com==11) com=10;
			else if(com==10) com=11;
			comline.Format(
				L"\"%s.exe\" %s",
				extcommand[com].program,
				extcommand[com].line);
		}
	}
	progress.AddString(comline);
	progress.AddString(compath);

	STARTUPINFO si;
	memset(&si,0,sizeof(si));
	si.cb=sizeof(si);
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=SW_HIDE;

	PROCESS_INFORMATION pi;

	if(!CreateProcess(compath.GetBuffer(),comline.GetBuffer(),0,0,false,CREATE_NEW_CONSOLE,0,comdir.GetBuffer(),&si,&pi))
	{
		CString s1,s2;
		s1.LoadString(IDS_ERROREXTCOM); s2.LoadString(IDS_ERROR);
		MessageBox(progress.m_hWnd,s1,s2,MB_OK);
		return;
	}

	command_handle[c]=pi.hProcess;

	WaitForSingleObject(pi.hProcess,timeout);

	CloseHandle(pi.hThread);

	compath.ReleaseBuffer();
	comline.ReleaseBuffer();
	comdir.ReleaseBuffer();
}

void CCombineDoc::InitializeExtCommands(void)
{
	int i;
	for(i=0; i<20; i++) command_handle[i]=0;
	InitializeMacros();
}

void CCombineDoc::initializecommands(void)
{
	str1.LoadString(IDS_STRING252);
	wcscpy(extcommand[0].name,str1); extcommand[0].line[0]=0; extcommand[0].program[0]=0;
	str1.LoadString(IDS_STRING253);
	wcscpy(extcommand[1].name,str1); extcommand[1].line[0]=0; extcommand[1].program[0]=0;
	str1.LoadString(IDS_STRING254);
	wcscpy(extcommand[2].name,str1); extcommand[2].line[0]=0; extcommand[2].program[0]=0;
	str1.LoadString(IDS_STRING255);
	wcscpy(extcommand[3].name,str1); extcommand[3].line[0]=0; extcommand[3].program[0]=0;
	str1.LoadString(IDS_STRING256);
	wcscpy(extcommand[4].name,str1); extcommand[4].line[0]=0; extcommand[4].program[0]=0;
	str1.LoadString(IDS_STRING257);
	wcscpy(extcommand[5].name,str1); extcommand[5].line[0]=0; extcommand[5].program[0]=0;
	str1.LoadString(IDS_STRING258);
	wcscpy(extcommand[6].name,str1); extcommand[6].line[0]=0; extcommand[6].program[0]=0;
	str1.LoadString(IDS_STRING259);
	wcscpy(extcommand[7].name,str1); extcommand[7].line[0]=0; extcommand[7].program[0]=0;
	str1.LoadString(IDS_STRING260);
	wcscpy(extcommand[8].name,str1); extcommand[8].line[0]=0; extcommand[8].program[0]=0;
	str1.LoadString(IDS_STRING261);
	wcscpy(extcommand[9].name,str1);
	extcommand[9].line[0]=0;
	wcscpy(extcommand[9].program,L"C:\\Program Files\\IrfanView\\i_view32.exe");

	str1.LoadString(IDS_STRING262);
	wcscpy(extcommand[10].name,str1); extcommand[10].line[0]=0; extcommand[10].program[0]=0;
	str1.LoadString(IDS_STRING263);
	wcscpy(extcommand[11].name,str1); extcommand[11].line[0]=0; extcommand[11].program[0]=0;
	str1.LoadString(IDS_STRING264);
	wcscpy(extcommand[12].name,str1); extcommand[12].line[0]=0; extcommand[12].program[0]=0;

	CString s;
	s.Format(L"%s\tF12",extcommand[10].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_EXCOM11,MF_BYCOMMAND,ID_EXCOM11,s);
	s.Format(L"%s\tF11",extcommand[11].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_EXCOM12,MF_BYCOMMAND,ID_EXCOM12,s);
	s.Format(L"%s\tF6",extcommand[12].name);
	((CMainFrame*)AfxGetMainWnd())->m_menu->ModifyMenu(ID_SYSCOM13,MF_BYCOMMAND,ID_SYSCOM13,s);
	if(!((CMainFrame*)AfxGetMainWnd())->simple_mode)
		((CMainFrame*)AfxGetMainWnd())->SetMenu((((CMainFrame*)AfxGetMainWnd())->m_menu));
}

void CCombineDoc::OnCommandEditcommands()
{
	int button;

	CExtCommand ec;

	for(;;)
	{
		CChooseExtCommand choose_extcom;
		button=(int)choose_extcom.DoModal()-101;
		if(button<0) break;

		ec.m_name=extcommand[button].name;
		ec.m_program=extcommand[button].program;
		ec.m_line=extcommand[button].line;

		if(ec.DoModal()==IDCANCEL) continue;

		wcscpy(extcommand[button].name,ec.m_name);
		wcscpy(extcommand[button].program,ec.m_program);
		wcscpy(extcommand[button].line,ec.m_line);
	}

	savedefaultmacros();

	CString s;
	s.Format(L"%s\tF12",extcommand[10].name);
	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_EXCOM11,MF_BYCOMMAND,ID_EXCOM11,s);
	s.Format(L"%s\tF11",extcommand[11].name);
	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_EXCOM12,MF_BYCOMMAND,ID_EXCOM12,s);
	s.Format(L"%s\tF6",extcommand[12].name);
	AfxGetMainWnd()->GetMenu()->ModifyMenu(ID_SYSCOM13,MF_BYCOMMAND,ID_SYSCOM13,s);
}

// D:\Programming\Projects\combine\Command.cpp : implementation file
//


// CChooseExtCommand dialog

IMPLEMENT_DYNAMIC(CChooseExtCommand, CDialog)
CChooseExtCommand::CChooseExtCommand(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseExtCommand::IDD, pParent)
{
}

CChooseExtCommand::~CChooseExtCommand()
{
}

void CChooseExtCommand::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	SetDlgItemText(IDC_ECOM1,extcommand[0].name);
	SetDlgItemText(IDC_ECOM2,extcommand[1].name);
	SetDlgItemText(IDC_ECOM3,extcommand[2].name);
	SetDlgItemText(IDC_ECOM4,extcommand[3].name);
	SetDlgItemText(IDC_ECOM5,extcommand[4].name);
	SetDlgItemText(IDC_ECOM6,extcommand[5].name);
	SetDlgItemText(IDC_ECOM7,extcommand[6].name);
	SetDlgItemText(IDC_ECOM8,extcommand[7].name);
	SetDlgItemText(IDC_ECOM9,extcommand[8].name);
	SetDlgItemText(IDC_ECOM10,extcommand[9].name);
	SetDlgItemText(IDC_ECOM11,extcommand[10].name);
	SetDlgItemText(IDC_ECOM12,extcommand[11].name);
	SetDlgItemText(IDC_ECOM13,extcommand[12].name);
}


BEGIN_MESSAGE_MAP(CChooseExtCommand, CDialog)
	ON_BN_CLICKED(IDC_ECOM1, OnBnClickedEcom1)
	ON_BN_CLICKED(IDC_ECOM2, OnBnClickedEcom2)
	ON_BN_CLICKED(IDC_ECOM3, OnBnClickedEcom3)
	ON_BN_CLICKED(IDC_ECOM4, OnBnClickedEcom4)
	ON_BN_CLICKED(IDC_ECOM5, OnBnClickedEcom5)
	ON_BN_CLICKED(IDC_ECOM6, OnBnClickedEcom6)
	ON_BN_CLICKED(IDC_ECOM7, OnBnClickedEcom7)
	ON_BN_CLICKED(IDC_ECOM8, OnBnClickedEcom8)
	ON_BN_CLICKED(IDC_ECOM9, OnBnClickedEcom9)
	ON_BN_CLICKED(IDC_ECOM10, OnBnClickedEcom10)
	ON_BN_CLICKED(IDC_ECOM11, OnBnClickedEcom11)
	ON_BN_CLICKED(IDC_ECOM12, OnBnClickedEcom12)
	ON_BN_CLICKED(IDC_ECOM13, OnBnClickedEcom13)
END_MESSAGE_MAP()


// CChooseExtCommand message handlers

// D:\Programming\Projects\combine\Command.cpp : implementation file
//


// CExtCommand dialog

IMPLEMENT_DYNAMIC(CExtCommand, CDialog)
CExtCommand::CExtCommand(CWnd* pParent /*=NULL*/)
	: CDialog(CExtCommand::IDD, pParent)
	, m_name(_T(""))
	, m_program(_T(""))
	, m_line(_T(""))
{
}

CExtCommand::~CExtCommand()
{
}

void CExtCommand::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME, m_name);
	DDV_MaxChars(pDX, m_name, 127);
	DDX_Text(pDX, IDC_PROGRAM, m_program);
	DDV_MaxChars(pDX, m_program, 127);
	DDX_Text(pDX, IDC_COMLINE, m_line);
	DDV_MaxChars(pDX, m_line, 127);
}


BEGIN_MESSAGE_MAP(CExtCommand, CDialog)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDC_LOAD, OnBnClickedLoad)
END_MESSAGE_MAP()


// CExtCommand message handlers

void CChooseExtCommand::OnBnClickedEcom1()
{
	EndDialog(101);
}

void CChooseExtCommand::OnBnClickedEcom2()
{
	EndDialog(102);
}

void CChooseExtCommand::OnBnClickedEcom3()
{
	EndDialog(103);
}

void CChooseExtCommand::OnBnClickedEcom4()
{
	EndDialog(104);
}

void CChooseExtCommand::OnBnClickedEcom5()
{
	EndDialog(105);
}

void CChooseExtCommand::OnBnClickedEcom6()
{
	EndDialog(106);
}

void CChooseExtCommand::OnBnClickedEcom7()
{
	EndDialog(107);
}

void CChooseExtCommand::OnBnClickedEcom8()
{
	EndDialog(108);
}

void CChooseExtCommand::OnBnClickedEcom9()
{
	EndDialog(109);
}

void CChooseExtCommand::OnBnClickedEcom10()
{
	EndDialog(110);
}

void CChooseExtCommand::OnBnClickedEcom11()
{
	EndDialog(111);
}

void CChooseExtCommand::OnBnClickedEcom12()
{
	EndDialog(112);
}

void CChooseExtCommand::OnBnClickedEcom13()
{
	EndDialog(113);
}

void CExtCommand::OnBnClickedSave()
{
	CString name; GetDlgItemText(IDC_NAME,name);
	CString program; GetDlgItemText(IDC_PROGRAM,program);
	CString comline; GetDlgItemText(IDC_COMLINE,comline);

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString compath=helppath.Left(helppath.ReverseFind('\\')+1)+name+".ecd";
	CFile f;
	f.Open(compath,CFile::modeCreate|CFile::modeWrite);

	f.Write(name,name.GetLength()); f.Write("\0",1);
	f.Write(program,program.GetLength()); f.Write("\0",1);
	f.Write(comline,comline.GetLength()); f.Write("\0",1);

	f.Close();

	CString s;
	CString s1,s2;
	s1.LoadString(IDS_MACROSAVED); s2.LoadString(IDS_MACROEDITOR);
	s.Format(s1,name);
	MessageBox(s,s2,MB_OK);
}

void CExtCommand::OnBnClickedLoad()
{
	char c[1];
	CString name; GetDlgItemText(IDC_NAME,name);
	CString program; GetDlgItemText(IDC_PROGRAM,program);
	CString comline; GetDlgItemText(IDC_COMLINE,comline);

	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString compath=helppath.Left(helppath.ReverseFind('\\')+1)+name+".ecd";
	CFile f;

	CString str1;
	str1.LoadString(IDS_STRING265);
	CFileDialog fd(true,L"ecd",0,0,str1,0);
	fd.m_ofn.lpstrInitialDir=compath.GetBuffer(1024);
	if(fd.DoModal()==IDCANCEL) return;
	if(f.Open(fd.GetPathName(),CFile::modeRead)==0) return;

	name=""; f.Read(c,1); while(c[0]){name+=c[0]; f.Read(c,1);};
	program=""; f.Read(c,1); while(c[0]){program+=c[0]; f.Read(c,1);};
	comline=""; f.Read(c,1); while(c[0]){comline+=c[0]; f.Read(c,1);};

	f.Close();
	
	SetDlgItemText(IDC_NAME,name);
	SetDlgItemText(IDC_PROGRAM,program);
	SetDlgItemText(IDC_COMLINE,comline);
}

void CCombineDoc::OnExcom11()
{
	DoExtCom(11,0,0,0);
}

void CCombineDoc::OnExcom12()
{
	DoExtCom(10,0,0,0);
}

void CCombineDoc::OnSyscom13()
{
	DoExtCom(12,0,0,0);
}

void CCombineDoc::OnFrameSaveandedit()
{
	if(top_frame<1) return;

	if((display==OUT_BUFFER) || (display==PICTURE_BUFFER))
	{
		newfromvisible();
		display=active_frame=top_frame-1;
	}

	silent_save_active(100);
	DoExtCom(9,0,0,0);
}
