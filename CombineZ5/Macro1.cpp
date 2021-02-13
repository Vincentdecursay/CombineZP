// Macro1.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "CombineDoc.h"
#include "Macro1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMacro dialog


CMacro::CMacro(CWnd* pParent /*=NULL*/)
	: CDialog(CMacro::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMacro)
	m_name = _T("");
	m_p1 = 0;
	m_p2 = 0;
	m_p3 = 0;
	//}}AFX_DATA_INIT
}


void CMacro::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMacro)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_COMMAND, m_command);
	DDX_Text(pDX, IDC_NAME, m_name);
//	DDX_Text(pDX, IDC_P1, m_p1);
//	DDX_Text(pDX, IDC_P2, m_p2);
//	DDX_Text(pDX, IDC_P3, m_p3);
	//}}AFX_DATA_MAP

	int i,item;
	CString s,s1;

	int ts[]={170,0};
	SendDlgItemMessage(IDC_LIST,LB_SETTABSTOPS,1,(long)ts);

	for(i=0; i<24; i++)
	{
		s="*"; s1="";
		if(this_macro.cmd[i].n!=-1)
		{
			if((this_macro.cmd[i].n>=IDS_MACRO1) && (this_macro.cmd[i].n<=IDS_MACRO10))
			{
				s=macro[this_macro.cmd[i].n-IDS_MACRO1].name;
			} else s.LoadString(this_macro.cmd[i].n);
			s1="";
			if(this_macro.cmd[i].p1!=-1)
			{
				if(this_macro.cmd[i].p2!=-1)
				{
					if(this_macro.cmd[i].p3!=-1)
						s1.Format("(%d,%d,%d)",
						this_macro.cmd[i].p1,
						this_macro.cmd[i].p2,
						this_macro.cmd[i].p3);
					else s1.Format("(%d,%d)",
						this_macro.cmd[i].p1,
						this_macro.cmd[i].p2);
				} else s1.Format("(%d)",
						this_macro.cmd[i].p1);
			}
			m_list.SetItemData(m_list.GetCount()-1,this_macro.cmd[i].n);
		}

		if((this_macro.cmd[i].f&MAC_TONAME) || this_macro.cmd[i].f&MAC_PAUSE ||(this_macro.cmd[i].f&MAC_SUBP1) || (this_macro.cmd[i].f&MAC_SUBP2) || (this_macro.cmd[i].f&MAC_SUBP3))
			s1+="\t";
		if(this_macro.cmd[i].f&MAC_TONAME)
		{
			s1+="N";
		}
		if(this_macro.cmd[i].f&MAC_PAUSE)
		{
			s1+="P";
		}
		if(this_macro.cmd[i].f&MAC_SUBP1)
		{
			s1+="S";
		}
		if(this_macro.cmd[i].f&MAC_SUBP2)
		{
			s1+="S";
		}
		if(this_macro.cmd[i].f&MAC_SUBP3)
		{
			s1+="S";
		}
		m_list.AddString(s+s1);
	}
	m_list.SetCurSel(0);

	item=m_command.AddString("*");
	m_command.SetItemData(item,-1);
	for(i=0; i<=last_command-first_command; i++)
	{
		if((i+first_command>=IDS_MACRO1) && (i+first_command<=IDS_MACRO10))
		{
			s=macro[i-(IDS_MACRO1-first_command)].name;
		} else s.LoadString(default_command[i].n);
		item=m_command.AddString(s);
		m_command.SetItemData(item,i);
	}
	GetDlgItem(IDC_P1)->EnableWindow(false);
	SetDlgItemInt(IDC_P1,-1,true);
	GetDlgItem(IDC_P2)->EnableWindow(false);
	SetDlgItemInt(IDC_P2,-1,true);
	GetDlgItem(IDC_P3)->EnableWindow(false);
	SetDlgItemInt(IDC_P3,-1,true);
	CheckDlgButton(IDC_PAUSE,false);
	GetDlgItem(IDC_PAUSE)->EnableWindow(false);
	CheckDlgButton(IDC_TONAME,false);
	GetDlgItem(IDC_TONAME)->EnableWindow(false);
	CheckDlgButton(IDC_SUBP1,false);
	GetDlgItem(IDC_SUBP1)->EnableWindow(false);
	CheckDlgButton(IDC_SUBP2,false);
	GetDlgItem(IDC_SUBP2)->EnableWindow(false);
	CheckDlgButton(IDC_SUBP3,false);
	GetDlgItem(IDC_SUBP3)->EnableWindow(false);
	GetDlgItem(IDC_LIST)->SetActiveWindow();
	GetDlgItem(IDC_EPS)->EnableWindow(false);

	command_modified=false;
}


BEGIN_MESSAGE_MAP(CMacro, CDialog)
	//{{AFX_MSG_MAP(CMacro)
	ON_CBN_SELCHANGE(IDC_COMMAND, OnSelchangeCommand)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMacro message handlers

void CMacro::OnSelchangeCommand() 
{
	int i;
	i=m_command.GetItemData(m_command.GetCurSel());

	GetDlgItem(IDC_P1)->EnableWindow(false);
	SetDlgItemInt(IDC_P1,-1,true);
	GetDlgItem(IDC_P2)->EnableWindow(false);
	SetDlgItemInt(IDC_P2,-1,true);
	GetDlgItem(IDC_P3)->EnableWindow(false);
	SetDlgItemInt(IDC_P3,-1,true);
	CheckDlgButton(IDC_PAUSE,false);
	GetDlgItem(IDC_PAUSE)->EnableWindow(false);
	CheckDlgButton(IDC_TONAME,false);
	GetDlgItem(IDC_TONAME)->EnableWindow(false);
	CheckDlgButton(IDC_SUBP1,false);
	GetDlgItem(IDC_SUBP1)->EnableWindow(false);
	CheckDlgButton(IDC_SUBP2,false);
	GetDlgItem(IDC_SUBP2)->EnableWindow(false);
	CheckDlgButton(IDC_SUBP3,false);
	GetDlgItem(IDC_SUBP3)->EnableWindow(false);
	GetDlgItem(IDC_EPS)->EnableWindow(false);

	if(i==-1) return;

	if((i<IDS_MACRO1) || (i>IDS_MACRO10)) GetDlgItem(IDC_TONAME)->EnableWindow(true);

	if(default_command[i].p1!=-1)
	{
		if(default_command[i].p2!=-1)
		{
			if(default_command[i].p3!=-1)
			{
				GetDlgItem(IDC_P1)->EnableWindow(true);
				SetDlgItemInt(IDC_P1,default_command[i].p1,true);
				GetDlgItem(IDC_P2)->EnableWindow(true);
				SetDlgItemInt(IDC_P2,default_command[i].p2,true);
				GetDlgItem(IDC_P3)->EnableWindow(true);
				SetDlgItemInt(IDC_P3,default_command[i].p3,true);
				GetDlgItem(IDC_PAUSE)->EnableWindow(true);
				GetDlgItem(IDC_SUBP1)->EnableWindow(true);
				GetDlgItem(IDC_SUBP2)->EnableWindow(true);
				GetDlgItem(IDC_SUBP3)->EnableWindow(true);
				GetDlgItem(IDC_EPS)->EnableWindow(true);
			} else {
				GetDlgItem(IDC_P1)->EnableWindow(true);
				SetDlgItemInt(IDC_P1,default_command[i].p1,true);
				GetDlgItem(IDC_P2)->EnableWindow(true);
				SetDlgItemInt(IDC_P2,default_command[i].p2,true);
				GetDlgItem(IDC_P3)->EnableWindow(false);
				SetDlgItemInt(IDC_P3,-1,true);
				GetDlgItem(IDC_PAUSE)->EnableWindow(true);
				GetDlgItem(IDC_SUBP1)->EnableWindow(true);
				GetDlgItem(IDC_SUBP2)->EnableWindow(true);
				GetDlgItem(IDC_SUBP3)->EnableWindow(false);
				GetDlgItem(IDC_EPS)->EnableWindow(true);
			}
		} else {
			GetDlgItem(IDC_P1)->EnableWindow(true);
			SetDlgItemInt(IDC_P1,default_command[i].p1,true);
			GetDlgItem(IDC_P2)->EnableWindow(false);
			SetDlgItemInt(IDC_P2,-1,true);
			GetDlgItem(IDC_P3)->EnableWindow(false);
			SetDlgItemInt(IDC_P3,-1,true);
			GetDlgItem(IDC_PAUSE)->EnableWindow(true);
			GetDlgItem(IDC_SUBP1)->EnableWindow(true);
			GetDlgItem(IDC_SUBP2)->EnableWindow(false);
			GetDlgItem(IDC_SUBP3)->EnableWindow(false);
			GetDlgItem(IDC_EPS)->EnableWindow(true);
		}
	} else {
		GetDlgItem(IDC_P1)->EnableWindow(false);
		SetDlgItemInt(IDC_P1,-1,true);
		GetDlgItem(IDC_P2)->EnableWindow(false);
		SetDlgItemInt(IDC_P2,-1,true);
		GetDlgItem(IDC_P3)->EnableWindow(false);
		SetDlgItemInt(IDC_P3,-1,true);
		GetDlgItem(IDC_PAUSE)->EnableWindow(true);
		GetDlgItem(IDC_SUBP1)->EnableWindow(false);
		GetDlgItem(IDC_SUBP2)->EnableWindow(false);
		GetDlgItem(IDC_SUBP3)->EnableWindow(false);
		GetDlgItem(IDC_EPS)->EnableWindow(false);
	}

	command_modified=true;
}

void CMacro::OnEdit() 
{
	int src,dest;
	src=m_list.GetCurSel();

	GetDlgItem(IDC_P1)->EnableWindow(false);
	SetDlgItemInt(IDC_P1,-1,true);
	GetDlgItem(IDC_P2)->EnableWindow(false);
	SetDlgItemInt(IDC_P2,-1,true);
	GetDlgItem(IDC_P3)->EnableWindow(false);
	SetDlgItemInt(IDC_P3,-1,true);
	CheckDlgButton(IDC_PAUSE,false);
	GetDlgItem(IDC_PAUSE)->EnableWindow(false);
	CheckDlgButton(IDC_TONAME,false);
	GetDlgItem(IDC_TONAME)->EnableWindow(false);
	CheckDlgButton(IDC_SUBP1,false);
	GetDlgItem(IDC_SUBP1)->EnableWindow(false);
	CheckDlgButton(IDC_SUBP2,false);
	GetDlgItem(IDC_SUBP2)->EnableWindow(false);
	CheckDlgButton(IDC_SUBP3,false);
	GetDlgItem(IDC_SUBP3)->EnableWindow(false);
	GetDlgItem(IDC_EPS)->EnableWindow(false);
	m_command.SetCurSel(0);

	if(this_macro.cmd[src].n==-1)
		m_command.SetCurSel(0);
	else {
		for(dest=0; dest<last_command-first_command+1; dest++)
			if(
				default_command[m_command.GetItemData(dest)].n==
				this_macro.cmd[src].n) break;
		if(dest>last_command-first_command+1) return;
		if(dest==0) return;
		m_command.SetCurSel(dest);
	}

	if(this_macro.cmd[src].p1!=-1)
	{
		if(this_macro.cmd[src].p2!=-1)
		{
			if(this_macro.cmd[src].p3!=-1)
			{
				GetDlgItem(IDC_P1)->EnableWindow(true);
				SetDlgItemInt(IDC_P1,this_macro.cmd[src].p1,true);
				GetDlgItem(IDC_P2)->EnableWindow(true);
				SetDlgItemInt(IDC_P2,this_macro.cmd[src].p2,true);
				GetDlgItem(IDC_P3)->EnableWindow(true);
				SetDlgItemInt(IDC_P3,this_macro.cmd[src].p3,true);
				GetDlgItem(IDC_PAUSE)->EnableWindow(true);
				GetDlgItem(IDC_SUBP1)->EnableWindow(true);
				GetDlgItem(IDC_SUBP2)->EnableWindow(true);
				GetDlgItem(IDC_SUBP3)->EnableWindow(true);
				GetDlgItem(IDC_EPS)->EnableWindow(true);
			}
			else
			{
				GetDlgItem(IDC_P1)->EnableWindow(true);
				SetDlgItemInt(IDC_P1,this_macro.cmd[src].p1,true);
				GetDlgItem(IDC_P2)->EnableWindow(true);
				SetDlgItemInt(IDC_P2,this_macro.cmd[src].p2,true);
				GetDlgItem(IDC_P3)->EnableWindow(false);
				SetDlgItemInt(IDC_P3,this_macro.cmd[src].p3,true);
				GetDlgItem(IDC_PAUSE)->EnableWindow(true);
				GetDlgItem(IDC_SUBP1)->EnableWindow(true);
				GetDlgItem(IDC_SUBP2)->EnableWindow(true);
				GetDlgItem(IDC_SUBP3)->EnableWindow(false);
				GetDlgItem(IDC_EPS)->EnableWindow(true);
			}
		}
		else
		{
			GetDlgItem(IDC_P1)->EnableWindow(true);
			SetDlgItemInt(IDC_P1,this_macro.cmd[src].p1,true);
			GetDlgItem(IDC_P2)->EnableWindow(false);
			SetDlgItemInt(IDC_P2,this_macro.cmd[src].p2,true);
			GetDlgItem(IDC_P3)->EnableWindow(false);
			SetDlgItemInt(IDC_P3,this_macro.cmd[src].p3,true);
			GetDlgItem(IDC_PAUSE)->EnableWindow(true);
			GetDlgItem(IDC_SUBP1)->EnableWindow(true);
			GetDlgItem(IDC_SUBP2)->EnableWindow(false);
			GetDlgItem(IDC_SUBP3)->EnableWindow(false);
			GetDlgItem(IDC_EPS)->EnableWindow(true);
		}
	} else {
		GetDlgItem(IDC_P1)->EnableWindow(false);
		SetDlgItemInt(IDC_P1,this_macro.cmd[src].p1,true);
		GetDlgItem(IDC_P2)->EnableWindow(false);
		SetDlgItemInt(IDC_P2,this_macro.cmd[src].p2,true);
		GetDlgItem(IDC_P3)->EnableWindow(false);
		SetDlgItemInt(IDC_P3,this_macro.cmd[src].p3,true);
		GetDlgItem(IDC_PAUSE)->EnableWindow(false);
		GetDlgItem(IDC_SUBP1)->EnableWindow(false);
		GetDlgItem(IDC_SUBP2)->EnableWindow(false);
		GetDlgItem(IDC_SUBP3)->EnableWindow(false);
		GetDlgItem(IDC_EPS)->EnableWindow(false);
	}

	CheckDlgButton(IDC_PAUSE,false);
	CheckDlgButton(IDC_SUBP1,false);
	CheckDlgButton(IDC_SUBP2,false);
	CheckDlgButton(IDC_SUBP3,false);

	GetDlgItem(IDC_TONAME)->EnableWindow(true);
	CheckDlgButton(IDC_TONAME,false);
	GetDlgItem(IDC_PAUSE)->EnableWindow(true);
	CheckDlgButton(IDC_PAUSE,false);

	if(this_macro.cmd[src].f&MAC_PAUSE) CheckDlgButton(IDC_PAUSE,true);
	if(this_macro.cmd[src].f&MAC_TONAME) CheckDlgButton(IDC_TONAME,true);
	if(this_macro.cmd[src].f&MAC_SUBP1) CheckDlgButton(IDC_SUBP1,true);
	if(this_macro.cmd[src].f&MAC_SUBP2) CheckDlgButton(IDC_SUBP2,true);
	if(this_macro.cmd[src].f&MAC_SUBP3) CheckDlgButton(IDC_SUBP3,true);

	SetDefID(IDC_UPDATE);

	command_modified=true;
}

void CMacro::OnUpdate() 
{
	int src,dest,item;
	item=m_command.GetCurSel();
	dest=m_list.GetCurSel();
	if(item==0)
	{
		this_macro.cmd[dest].n=-1;
		this_macro.cmd[dest].p1=-1;
		this_macro.cmd[dest].p2=-1;
		this_macro.cmd[dest].p3=-1;
		m_list.DeleteString(dest);
		m_list.InsertString(dest,"*");
		m_list.SetCurSel(dest);
		GetDlgItem(IDC_LIST)->SetActiveWindow();
		return;
	}
	if(item==CB_ERR) return;
	src=m_command.GetItemData(item);
	if(dest==LB_ERR) return;
	this_macro.cmd[dest].n=default_command[src].n;
	this_macro.cmd[dest].p1=GetDlgItemInt(IDC_P1);
	this_macro.cmd[dest].p2=GetDlgItemInt(IDC_P2);
	this_macro.cmd[dest].p3=GetDlgItemInt(IDC_P3);

	CString s,s1;
	if((this_macro.cmd[dest].n>=IDS_MACRO1) && (this_macro.cmd[dest].n<=IDS_MACRO10))
	{
		s=macro[this_macro.cmd[dest].n-IDS_MACRO1].name;
	} else s.LoadString(this_macro.cmd[dest].n);
	s1="";
	if(this_macro.cmd[dest].p1!=-1)
	{
		if(this_macro.cmd[dest].p2!=-1)
		{
			if(this_macro.cmd[dest].p3!=-1)
				s1.Format("(%d,%d,%d)",
				this_macro.cmd[dest].p1,
				this_macro.cmd[dest].p2,
				this_macro.cmd[dest].p3);
			else s1.Format("(%d,%d)",
				this_macro.cmd[dest].p1,
				this_macro.cmd[dest].p2);
		} else s1.Format("(%d)",
				this_macro.cmd[dest].p1);
	}
	m_list.DeleteString(dest);
	if
	(
		(IsDlgButtonChecked(IDC_TONAME)) ||
		(IsDlgButtonChecked(IDC_PAUSE))  ||
		(IsDlgButtonChecked(IDC_SUBP1))  ||
		(IsDlgButtonChecked(IDC_SUBP2))  ||
		(IsDlgButtonChecked(IDC_SUBP3))
	)
		s1+="\t";
	if(IsDlgButtonChecked(IDC_TONAME))
	{
		this_macro.cmd[dest].f|=MAC_TONAME;
		s1+="N";
	} else this_macro.cmd[dest].f&=~MAC_TONAME;
	if(IsDlgButtonChecked(IDC_PAUSE))
	{
		this_macro.cmd[dest].f|=MAC_PAUSE;
		s1+="P";
	} else this_macro.cmd[dest].f&=~MAC_PAUSE;
	if(IsDlgButtonChecked(IDC_SUBP1))
	{
		this_macro.cmd[dest].f|=MAC_SUBP1;
		s1+="S";
	} else this_macro.cmd[dest].f&=~MAC_SUBP1;
	if(IsDlgButtonChecked(IDC_SUBP2))
	{
		this_macro.cmd[dest].f|=MAC_SUBP2;
		s1+="S";
	} else this_macro.cmd[dest].f&=~MAC_SUBP2;
	if(IsDlgButtonChecked(IDC_SUBP3))
	{
		this_macro.cmd[dest].f|=MAC_SUBP3;
		while((s.GetLength()+s1.GetLength())<46) s1+=" ";
		s1+="S";
	} else this_macro.cmd[dest].f&=~MAC_SUBP3;
	m_list.InsertString(dest,s+s1);
	if(dest<24) m_list.SetCurSel(dest+1);
	if(IsDlgButtonChecked(IDC_PAUSE)) this_macro.cmd[dest].f|=1;
	else this_macro.cmd[dest].f&=(~1);
	GetDlgItem(IDC_P1)->EnableWindow(false);
	SetDlgItemInt(IDC_P1,-1,true);
	GetDlgItem(IDC_P2)->EnableWindow(false);
	SetDlgItemInt(IDC_P2,-1,true);
	GetDlgItem(IDC_P3)->EnableWindow(false);
	SetDlgItemInt(IDC_P3,-1,true);
	CheckDlgButton(IDC_PAUSE,false);
	GetDlgItem(IDC_PAUSE)->EnableWindow(false);
	CheckDlgButton(IDC_TONAME,false);
	GetDlgItem(IDC_TONAME)->EnableWindow(false);
	CheckDlgButton(IDC_SUBP1,false);
	GetDlgItem(IDC_SUBP1)->EnableWindow(false);
	CheckDlgButton(IDC_SUBP2,false);
	GetDlgItem(IDC_SUBP2)->EnableWindow(false);
	CheckDlgButton(IDC_SUBP3,false);
	GetDlgItem(IDC_SUBP3)->EnableWindow(false);
	GetDlgItem(IDC_EPS)->EnableWindow(false);
	m_command.SetCurSel(0);
	SetDefID(IDC_EDIT);
	GetDlgItem(IDC_LIST)->SetActiveWindow();
	GetDlgItem(IDC_LIST)->SetFocus();

	command_modified=false;
}

void CMacro::OnInsert() 
{
	int i,dest;
	dest=m_list.GetCurSel();

	for(i=23; i>dest; i--)
		this_macro.cmd[i]=this_macro.cmd[i-1];

	m_list.InsertString(dest,"*");
	m_list.DeleteString(24);

	m_list.SetCurSel(dest);
	OnUpdate();
}

void CMacro::OnDelete() 
{
	int i,dest;
	dest=m_list.GetCurSel();

	m_list.DeleteString(dest);
	m_list.AddString("*");
	m_list.SetCurSel(dest);

	for(i=dest; i<23; i++)
		this_macro.cmd[i]=this_macro.cmd[i+1];
	this_macro.cmd[23].n=-1;
	this_macro.cmd[23].p1=-1;
	this_macro.cmd[23].p2=-1;
	this_macro.cmd[23].p3=-1;
}

void CMacro::OnLoad() 
{
	char c[1];
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString macpath=helppath.Left(helppath.ReverseFind('\\')+1);
	CFile f;

	CFileDialog fd(true,"mac",0,0,"Single Macro|*.mac;|*.mac||",0);
	fd.m_ofn.lpstrInitialDir=macpath.GetBuffer(1024);
	if(fd.DoModal()==IDCANCEL) return;
	if(f.Open(fd.GetPathName(),CFile::modeRead)==0) return;

	f.Read(this_macro.cmd,24*sizeof(CZCommand));
	this_macro.name="";
	f.Read(&c[0],1);
	while(c[0])
	{
		this_macro.name+=c[0];
		f.Read(&c[0],1);
	}
	f.Close();
	
	m_list.ResetContent();

	int i;
	CString s,s1;
	for(i=0; i<24; i++)
	{
//		if(this_macro.cmd[i].n==-1) this_macro.cmd[i].n=0x80000001;
//		if(this_macro.cmd[i].p1==-1) this_macro.cmd[i].p1=0x80000001;
//		if(this_macro.cmd[i].p2==-1) this_macro.cmd[i].p2=0x80000001;
//		if(this_macro.cmd[i].p3==-1) this_macro.cmd[i].p3=0x80000001;
		s="*"; s1="";
		if(this_macro.cmd[i].n!=-1)
		{
			s.LoadString(this_macro.cmd[i].n);
			s1="";
			if(this_macro.cmd[i].p1!=-1)
			{
				if(this_macro.cmd[i].p2!=-1)
				{
					if(this_macro.cmd[i].p3!=-1)
						s1.Format("(%d,%d,%d)",
						this_macro.cmd[i].p1,
						this_macro.cmd[i].p2,
						this_macro.cmd[i].p3);
					else s1.Format("(%d,%d)",
						this_macro.cmd[i].p1,
						this_macro.cmd[i].p2);
				} else s1.Format("(%d)",
						this_macro.cmd[i].p1);
			}
			m_list.SetItemData(m_list.GetCount()-1,this_macro.cmd[i].n);
		}
		m_list.AddString(s+s1);
	}
	m_list.SetCurSel(0);
	SetDlgItemText(IDC_NAME,this_macro.name);
}

void CMacro::OnSave() 
{
	int i;
	char c[1];
	CString name;
	GetDlgItemText(IDC_NAME,name);
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString macpath=helppath.Left(helppath.ReverseFind('\\')+1)+name+".mac";
	CFile f;
	f.Open(macpath,CFile::modeCreate|CFile::modeWrite);
	f.Write(this_macro.cmd,24*sizeof(CZCommand));
	for(i=0; i<name.GetLength(); i++)
	{
		c[0]=name[i];
		f.Write(&c[0],1);
	}
	f.Write("\0",1);
	f.Close();
	CString s;
	s.Format("Macro %s Saved",name);
	MessageBox(s,"Macro Editor",MB_OK);
}

void CMacro::OnOK() 
{
	if(command_modified)
	{
		command_modified=false;
		int ret=MessageBox("Leave Editor without saving your last entry?","Macro Editor",MB_YESNO);
		if(ret==IDNO) return;
	}
	CDialog::OnOK();
}
