// Macro1.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "CombineDoc.h"
#include "Macro1.h"
#include "macparam.h"

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
	DDX_Control(pDX, IDC_SUB1, sub1);
	DDX_Control(pDX, IDC_SUB2, sub2);
	DDX_Control(pDX, IDC_SUB3, sub3);
	//	DDX_Text(pDX, IDC_P1, m_p1);
	//	DDX_Text(pDX, IDC_P2, m_p2);
	//	DDX_Text(pDX, IDC_P3, m_p3);
	//}}AFX_DATA_MAP

	int i,item;
	CString s,s1;

	int ts[]={170,0};
	SendDlgItemMessage(IDC_LIST,LB_SETTABSTOPS,1,(long)ts);

	int p1,p2,p3;
	CString st1,st2,st3;

	for(i=0; i<24; i++)
	{
		s=L"*"; s1=L"";
		if(this_macro.cmd[i].n!=-1)
		{
			s.LoadString(this_macro.cmd[i].n);
			if((this_macro.cmd[i].n>=IDS_MACRO1) && (this_macro.cmd[i].n<=IDS_MACRO10))
			{
				s=macro[this_macro.cmd[i].n-IDS_MACRO1].name;
			}
			if((this_macro.cmd[i].n>=IDS_EXCOM1) && (this_macro.cmd[i].n<=IDS_EXCOM10))
			{
				s=extcommand[this_macro.cmd[i].n-IDS_EXCOM1].name;
			}

			s1=L"";

			p1=this_macro.cmd[i].p1; if(p1<-1) p1++;
			if((int)MAC_P1V(this_macro.cmd[i].f)!=0 && (int)MAC_P1V(this_macro.cmd[i].f)!=11)
				st1=macparams[MAC_P1V(this_macro.cmd[i].f)].name;
			else st1.Format(L"%d",p1);

			p2=this_macro.cmd[i].p2; if(p2<-1) p2++;
			if((int)MAC_P2V(this_macro.cmd[i].f)!=0 && (int)MAC_P2V(this_macro.cmd[i].f)!=11)
				st2=macparams[MAC_P2V(this_macro.cmd[i].f)].name;
			else st2.Format(L"%d",p2);

			p3=this_macro.cmd[i].p3; if(p3<-1) p3++;
			if((int)MAC_P3V(this_macro.cmd[i].f)!=0 && (int)MAC_P3V(this_macro.cmd[i].f)!=11)
				st3=macparams[MAC_P3V(this_macro.cmd[i].f)].name;
			else st3.Format(L"%d",p3);

			if(this_macro.cmd[i].p1!=-1)
			{
				if(this_macro.cmd[i].p2!=-1)
				{
					if(this_macro.cmd[i].p3!=-1) s1.Format(L"(%s,%s,%s)",st1,st2,st3);
					else s1.Format(L"(%s,%s)",st1,st2);
				} else s1.Format(L"(%s)",st1);
			}
			m_list.SetItemData(m_list.GetCount()-1,this_macro.cmd[i].n);
		}

		s1+="\t";
		if(this_macro.cmd[i].f&MAC_SUBP1) s1+=L"S";
		else s1+=L"0";
		if(this_macro.cmd[i].f&MAC_SUBP2) s1+=L"S";
		else s1+=L"0";
		if(this_macro.cmd[i].f&MAC_SUBP3) s1+=L"S";
		else s1+=L"0";
		if(this_macro.cmd[i].f&MAC_TONAME) s1+=L"N";
		if(this_macro.cmd[i].f&MAC_PAUSE) s1+=L"P";
		m_list.AddString(s+s1);
	}
	m_list.SetCurSel(0);

	item=m_command.AddString(L"*");
	m_command.SetItemData(item,(DWORD_PTR)-1);
	for(i=0; i<=last_command-first_command; i++)
	{
		s.LoadString(default_command[i].n);
		if((i+first_command>=IDS_MACRO1) && (i+first_command<=IDS_MACRO10))
		{
			s=macro[i-(IDS_MACRO1-first_command)].name;
		}
		if((i+first_command>=IDS_EXCOM1) && (i+first_command<=IDS_EXCOM10))
		{
			s=extcommand[i-(IDS_EXCOM1-first_command)].name;
		}
		item=m_command.AddString(s);
		m_command.SetItemData(item,i);
	}
	GetDlgItem(IDC_P1)->EnableWindow(false);
	SetDlgItemInt(IDC_P1,0xffffffff,true);
	GetDlgItem(IDC_P2)->EnableWindow(false);
	SetDlgItemInt(IDC_P2,0xffffffff,true);
	GetDlgItem(IDC_P3)->EnableWindow(false);
	SetDlgItemInt(IDC_P3,0xffffffff,true);
	CheckDlgButton(IDC_PAUSE,false);
	GetDlgItem(IDC_PAUSE)->EnableWindow(false);
	CheckDlgButton(IDC_TONAME,false);
	GetDlgItem(IDC_TONAME)->EnableWindow(false);

	for(i=0; i<LAST_MACPARAM; i++)
//	for(i=0; i<12; i++)
	{
		item=sub1.AddString(m_mp[i].name);
		sub1.SetItemData(item,i);
		item=sub2.AddString(m_mp[i].name);
		sub2.SetItemData(item,i);
		item=sub3.AddString(m_mp[i].name);
		sub3.SetItemData(item,i);
	}
	sub1.SelectString(-1,m_mp[0].name);
	sub2.SelectString(-1,m_mp[0].name);
	sub3.SelectString(-1,m_mp[0].name);

	GetDlgItem(IDC_SUB1)->EnableWindow(false);
	GetDlgItem(IDC_SUB2)->EnableWindow(false);
	GetDlgItem(IDC_SUB3)->EnableWindow(false);
	GetDlgItem(IDC_LIST)->SetActiveWindow();

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
	int p1,p2,p3;
	bool f1,f2,f3;

	i=m_command.GetItemData(m_command.GetCurSel());

	GetDlgItem(IDC_P1)->EnableWindow(false);
	SetDlgItemInt(IDC_P1,0xffffffff,true);
	GetDlgItem(IDC_P2)->EnableWindow(false);
	SetDlgItemInt(IDC_P2,0xffffffff-1,true);
	GetDlgItem(IDC_P3)->EnableWindow(false);
	SetDlgItemInt(IDC_P3,0xffffffff-1,true);
	CheckDlgButton(IDC_PAUSE,false);
	GetDlgItem(IDC_PAUSE)->EnableWindow(false);
	CheckDlgButton(IDC_TONAME,false);
	GetDlgItem(IDC_TONAME)->EnableWindow(false);
	sub1.SelectString(-1,m_mp[0].name);
	sub2.SelectString(-1,m_mp[0].name);
	sub3.SelectString(-1,m_mp[0].name);
	GetDlgItem(IDC_SUB1)->EnableWindow(false);
	GetDlgItem(IDC_SUB2)->EnableWindow(false);
	GetDlgItem(IDC_SUB3)->EnableWindow(false);

	if(i==-1) return;

	if((i<IDS_MACRO1) || (i>IDS_MACRO10)) GetDlgItem(IDC_TONAME)->EnableWindow(true);

	f1=(default_command[i].p1!=-1);
	f2=(default_command[i].p2!=-1);
	f3=(default_command[i].p3!=-1);
	if(default_command[i].p1>=-1) p1=default_command[i].p1;
	else p1=default_command[i].p1+1;
	if(default_command[i].p2>=-1) p2=default_command[i].p2;
	else p2=default_command[i].p2+1;
	if(default_command[i].p3>=-1) p3=default_command[i].p3;
	else p3=default_command[i].p3+1;

	if(f1)
	{
		if(f2)
		{
			if(f3)
			{
				GetDlgItem(IDC_P1)->EnableWindow(true);
				SetDlgItemInt(IDC_P1,p1,true);
				GetDlgItem(IDC_P2)->EnableWindow(true);
				SetDlgItemInt(IDC_P2,p2,true);
				GetDlgItem(IDC_P3)->EnableWindow(true);
				SetDlgItemInt(IDC_P3,p3,true);
				GetDlgItem(IDC_PAUSE)->EnableWindow(true);
				GetDlgItem(IDC_SUB1)->EnableWindow(true);
				GetDlgItem(IDC_SUB2)->EnableWindow(true);
				GetDlgItem(IDC_SUB3)->EnableWindow(true);
			} else {
				GetDlgItem(IDC_P1)->EnableWindow(true);
				SetDlgItemInt(IDC_P1,p1,true);
				GetDlgItem(IDC_P2)->EnableWindow(true);
				SetDlgItemInt(IDC_P2,p2,true);
				GetDlgItem(IDC_P3)->EnableWindow(false);
				SetDlgItemInt(IDC_P3,0xffffffff-1,true);
				GetDlgItem(IDC_PAUSE)->EnableWindow(true);
				GetDlgItem(IDC_SUB1)->EnableWindow(true);
				GetDlgItem(IDC_SUB2)->EnableWindow(true);
				GetDlgItem(IDC_SUB3)->EnableWindow(false);
			}
		} else {
			GetDlgItem(IDC_P1)->EnableWindow(true);
			SetDlgItemInt(IDC_P1,p1,true);
			GetDlgItem(IDC_P2)->EnableWindow(false);
			SetDlgItemInt(IDC_P2,0xffffffff,true);
			GetDlgItem(IDC_P3)->EnableWindow(false);
			SetDlgItemInt(IDC_P3,0xffffffff,true);
			GetDlgItem(IDC_PAUSE)->EnableWindow(true);
			GetDlgItem(IDC_SUB1)->EnableWindow(true);
			GetDlgItem(IDC_SUB2)->EnableWindow(false);
			GetDlgItem(IDC_SUB3)->EnableWindow(false);
		}
	} else {
		GetDlgItem(IDC_P1)->EnableWindow(false);
		SetDlgItemInt(IDC_P1,0xffffffff,true);
		GetDlgItem(IDC_P2)->EnableWindow(false);
		SetDlgItemInt(IDC_P2,0xffffffff,true);
		GetDlgItem(IDC_P3)->EnableWindow(false);
		SetDlgItemInt(IDC_P3,0xffffffff,true);
		GetDlgItem(IDC_PAUSE)->EnableWindow(true);
		GetDlgItem(IDC_SUB1)->EnableWindow(false);
		GetDlgItem(IDC_SUB2)->EnableWindow(false);
		GetDlgItem(IDC_SUB3)->EnableWindow(false);
	}

	command_modified=true;
}

void CMacro::OnEdit() 
{
	int src,dest;
	src=m_list.GetCurSel();
	int p1,p2,p3;

	GetDlgItem(IDC_P1)->EnableWindow(false);
	SetDlgItemInt(IDC_P1,0xffffffff,true);
	GetDlgItem(IDC_P2)->EnableWindow(false);
	SetDlgItemInt(IDC_P2,0xffffffff,true);
	GetDlgItem(IDC_P3)->EnableWindow(false);
	SetDlgItemInt(IDC_P3,0xffffffff,true);
	CheckDlgButton(IDC_PAUSE,false);
	GetDlgItem(IDC_PAUSE)->EnableWindow(false);
	CheckDlgButton(IDC_TONAME,false);
	GetDlgItem(IDC_TONAME)->EnableWindow(false);
	sub1.SelectString(-1,m_mp[0].name);
	sub2.SelectString(-1,m_mp[0].name);
	sub3.SelectString(-1,m_mp[0].name);
	GetDlgItem(IDC_SUB1)->EnableWindow(false);
	GetDlgItem(IDC_SUB2)->EnableWindow(false);
	GetDlgItem(IDC_SUB3)->EnableWindow(false);
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
		m_command.SetFocus();
	}

	p1=this_macro.cmd[src].p1; if(p1<-1) p1++;
	p2=this_macro.cmd[src].p2; if(p2<-1) p2++;
	p3=this_macro.cmd[src].p3; if(p3<-1) p3++;
	if(this_macro.cmd[src].p1!=-1)
	{
		if(this_macro.cmd[src].p2!=-1)
		{
			if(this_macro.cmd[src].p3!=-1)
			{
				GetDlgItem(IDC_P1)->EnableWindow(true);
				SetDlgItemInt(IDC_P1,p1,true);
				GetDlgItem(IDC_P2)->EnableWindow(true);
				SetDlgItemInt(IDC_P2,p2,true);
				GetDlgItem(IDC_P3)->EnableWindow(true);
				SetDlgItemInt(IDC_P3,p3,true);
				GetDlgItem(IDC_PAUSE)->EnableWindow(true);
				sub1.SelectString(-1,macparams[MAC_P1V(this_macro.cmd[src].f)].name);
				sub2.SelectString(-1,macparams[MAC_P2V(this_macro.cmd[src].f)].name);
				sub3.SelectString(-1,macparams[MAC_P3V(this_macro.cmd[src].f)].name);
				GetDlgItem(IDC_SUB1)->EnableWindow(true);
				GetDlgItem(IDC_SUB2)->EnableWindow(true);
				GetDlgItem(IDC_SUB3)->EnableWindow(true);
			}
			else
			{
				GetDlgItem(IDC_P1)->EnableWindow(true);
				SetDlgItemInt(IDC_P1,p1,true);
				GetDlgItem(IDC_P2)->EnableWindow(true);
				SetDlgItemInt(IDC_P2,p2,true);
				GetDlgItem(IDC_P3)->EnableWindow(false);
				SetDlgItemInt(IDC_P3,p3,true);
				GetDlgItem(IDC_PAUSE)->EnableWindow(true);
				sub3.SelectString(-1,m_mp[0].name);
				sub1.SelectString(-1,macparams[MAC_P1V(this_macro.cmd[src].f)].name);
				sub2.SelectString(-1,macparams[MAC_P2V(this_macro.cmd[src].f)].name);
				GetDlgItem(IDC_SUB1)->EnableWindow(true);
				GetDlgItem(IDC_SUB2)->EnableWindow(true);
				GetDlgItem(IDC_SUB3)->EnableWindow(false);
			}
		}
		else
		{
			GetDlgItem(IDC_P1)->EnableWindow(true);
			SetDlgItemInt(IDC_P1,p1,true);
			GetDlgItem(IDC_P2)->EnableWindow(false);
			SetDlgItemInt(IDC_P2,p2,true);
			GetDlgItem(IDC_P3)->EnableWindow(false);
			SetDlgItemInt(IDC_P3,p3,true);
			GetDlgItem(IDC_PAUSE)->EnableWindow(true);
			sub2.SelectString(-1,m_mp[0].name);
			sub3.SelectString(-1,m_mp[0].name);
			GetDlgItem(IDC_SUB1)->EnableWindow(true);
			sub1.SelectString(-1,macparams[MAC_P1V(this_macro.cmd[src].f)].name);
			GetDlgItem(IDC_SUB2)->EnableWindow(false);
			GetDlgItem(IDC_SUB3)->EnableWindow(false);
		}
	} else {
		GetDlgItem(IDC_P1)->EnableWindow(false);
		SetDlgItemInt(IDC_P1,p1,true);
		GetDlgItem(IDC_P2)->EnableWindow(false);
		SetDlgItemInt(IDC_P2,p2,true);
		GetDlgItem(IDC_P3)->EnableWindow(false);
		SetDlgItemInt(IDC_P3,p3,true);
		GetDlgItem(IDC_PAUSE)->EnableWindow(false);
		sub1.SelectString(-1,m_mp[0].name);
		sub2.SelectString(-1,m_mp[0].name);
		sub3.SelectString(-1,m_mp[0].name);
		GetDlgItem(IDC_SUB1)->EnableWindow(false);
		GetDlgItem(IDC_SUB2)->EnableWindow(false);
		GetDlgItem(IDC_SUB3)->EnableWindow(false);
	}

	sub1.SelectString(-1,macparams[MAC_P1V(this_macro.cmd[src].f)].name);
	sub2.SelectString(-1,macparams[MAC_P2V(this_macro.cmd[src].f)].name);
	sub3.SelectString(-1,macparams[MAC_P3V(this_macro.cmd[src].f)].name);

	CheckDlgButton(IDC_PAUSE,false);

	GetDlgItem(IDC_TONAME)->EnableWindow(true);
	CheckDlgButton(IDC_TONAME,false);
	GetDlgItem(IDC_PAUSE)->EnableWindow(true);
	CheckDlgButton(IDC_PAUSE,false);

	if(this_macro.cmd[src].f&MAC_PAUSE) CheckDlgButton(IDC_PAUSE,true);
	if(this_macro.cmd[src].f&MAC_TONAME) CheckDlgButton(IDC_TONAME,true);

	SetDefID(IDC_UPDATE);

	command_modified=true;
}

void CMacro::OnUpdate() 
{
	int p1,p2,p3;
	CString st1,st2,st3;
	bool f1,f2,f3;
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
		m_list.InsertString(dest,L"*");
		m_list.SetCurSel(dest);
		GetDlgItem(IDC_LIST)->SetActiveWindow();
		return;
	}
	if(item==CB_ERR) return;
	src=m_command.GetItemData(item);
	if(dest==LB_ERR) return;
	this_macro.cmd[dest].n=default_command[src].n;

	f1=(default_command[src].p1!=-1);
	f2=(default_command[src].p2!=-1);
	f3=(default_command[src].p3!=-1);

	p1=GetDlgItemInt(IDC_P1); if(p1<0 && f1) p1--;
	p2=GetDlgItemInt(IDC_P2); if(p2<0 && f2) p2--;
	p3=GetDlgItemInt(IDC_P3); if(p3<0 && f3) p3--;
	this_macro.cmd[dest].p1=p1;
	this_macro.cmd[dest].p2=p2;
	this_macro.cmd[dest].p3=p3;

	if(p1<-1 && f1) p1++;
	st1.Format(L"%d",p1);
	if(sub1.GetItemData(sub1.GetCurSel())!=0 && sub1.GetItemData(sub1.GetCurSel())!=11)
		st1=macparams[sub1.GetItemData(sub1.GetCurSel())].name;

	if(p2<-1 && f2) p2++;
	st2.Format(L"%d",p2);
	if(sub2.GetItemData(sub2.GetCurSel())!=0 && sub2.GetItemData(sub2.GetCurSel())!=11)
		st2=macparams[sub2.GetItemData(sub2.GetCurSel())].name;

	if(p3<-1 && f3) p3++;
	st3.Format(L"%d",p3);
	if(sub3.GetItemData(sub3.GetCurSel())!=0 && sub3.GetItemData(sub3.GetCurSel())!=11)
		st3=macparams[sub3.GetItemData(sub3.GetCurSel())].name;

	CString s,s1;
	s.LoadString(this_macro.cmd[dest].n);
	if((this_macro.cmd[dest].n>=IDS_MACRO1) && (this_macro.cmd[dest].n<=IDS_MACRO10))
		s=macro[this_macro.cmd[dest].n-IDS_MACRO1].name;
	if((this_macro.cmd[dest].n>=IDS_EXCOM1) && (this_macro.cmd[dest].n<=IDS_EXCOM10))
		s=extcommand[this_macro.cmd[dest].n-IDS_EXCOM1].name;
	if((this_macro.cmd[dest].n>=IDS_EXCOM11) && (this_macro.cmd[dest].n<=IDS_EXCOM12))
		s=extcommand[this_macro.cmd[dest].n-IDS_EXCOM11].name;
	if(f1)
	{
		if(f2)
		{
			if(f3) s1.Format(L"(%s,%s,%s)",st1,st2,st3);
			else s1.Format(L"(%s,%s)",st1,st2);
		} else s1.Format(L"(%s)",st1);
	}
	m_list.DeleteString(dest);
	s1+="\t";
	
	this_macro.cmd[dest].f=0;

	unsigned int temp;
	temp=sub1.GetItemData(sub1.GetCurSel());
	this_macro.cmd[dest].f|=(((temp&0xf)<<28)+((temp&0xf0)<<12));
	if(sub1.GetItemData(sub1.GetCurSel())==11)
	{this_macro.cmd[dest].f|=MAC_SUBP1; s1+="S";}
	else{this_macro.cmd[dest].f&=(~MAC_SUBP1); s1+="0";}

	temp=sub2.GetItemData(sub2.GetCurSel());
	this_macro.cmd[dest].f|=(((temp&0xf)<<24)+((temp&0xf0)<<8));
	if(sub2.GetItemData(sub2.GetCurSel())==11)
	{this_macro.cmd[dest].f|=MAC_SUBP2; s1+="S";}
	else{this_macro.cmd[dest].f&=(~MAC_SUBP2); s1+="0";}

	temp=sub3.GetItemData(sub3.GetCurSel());
	this_macro.cmd[dest].f|=(((temp&0xf)<<20)+((temp&0xf0)<<4));
	if(sub3.GetItemData(sub3.GetCurSel())==11)
	{this_macro.cmd[dest].f|=MAC_SUBP3; s1+="S";}
	else{this_macro.cmd[dest].f&=(~MAC_SUBP3); s1+="0";}

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

	m_list.InsertString(dest,s+s1);
	if(dest<24) m_list.SetCurSel(dest+1);
	if(IsDlgButtonChecked(IDC_PAUSE)) this_macro.cmd[dest].f|=1;
	else this_macro.cmd[dest].f&=(~1);
	GetDlgItem(IDC_P1)->EnableWindow(false);
	SetDlgItemInt(IDC_P1,0xffffffff,true);
	GetDlgItem(IDC_P2)->EnableWindow(false);
	SetDlgItemInt(IDC_P2,0xffffffff,true);
	GetDlgItem(IDC_P3)->EnableWindow(false);
	SetDlgItemInt(IDC_P3,0xffffffff,true);
	CheckDlgButton(IDC_PAUSE,false);
	GetDlgItem(IDC_PAUSE)->EnableWindow(false);
	CheckDlgButton(IDC_TONAME,false);
	GetDlgItem(IDC_TONAME)->EnableWindow(false);
	sub1.SelectString(-1,m_mp[0].name);
	sub2.SelectString(-1,m_mp[0].name);
	sub3.SelectString(-1,m_mp[0].name);
	GetDlgItem(IDC_SUB1)->EnableWindow(false);
	GetDlgItem(IDC_SUB2)->EnableWindow(false);
	GetDlgItem(IDC_SUB3)->EnableWindow(false);
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

	m_list.InsertString(dest,L"*");
	m_list.DeleteString(24);

	m_list.SetCurSel(dest);
	OnUpdate();
}

void CMacro::OnDelete() 
{
	int i,dest;
	dest=m_list.GetCurSel();

	m_list.DeleteString(dest);

	if(dest==23)
	{
		m_list.AddString(L"*\t000");
		m_list.SetItemData(23,(DWORD)(-1));
		m_list.SetCurSel(dest);
		i=23;
	} else {
		for(i=dest; i<22; i++)
		{
			this_macro.cmd[i]=this_macro.cmd[i+1];
			if(this_macro.cmd[i+1].n==-1) break;
		}

		m_list.InsertString(22,L"*\t000");
		m_list.SetItemData(22,(DWORD)(-1));
		m_list.SetCurSel(dest);
	}

	this_macro.cmd[i].n=-1;
	this_macro.cmd[i].p1=-1;
	this_macro.cmd[i].p2=-1;
	this_macro.cmd[i].p3=-1;
}

void CMacro::OnLoad() 
{
	wchar_t wc;
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString macpath=helppath.Left(helppath.ReverseFind('\\')+1);
	CFile f;

	CString str1; str1.LoadString(IDS_STRING610);
	CFileDialog fd(true,L"mac",0,0,str1,0);
	fd.m_ofn.lpstrInitialDir=macpath.GetBuffer(1024);
	if(fd.DoModal()==IDCANCEL) return;

	if(f.Open(fd.GetPathName(),CFile::modeRead)==0) return;

	f.Read(this_macro.cmd,24*sizeof(CZCommand));

	this_macro.name=L"";
	f.Read(&wc,2);
	while(wc)
	{
		this_macro.name+=wc;
		f.Read(&wc,2);
	}

	f.Close();
	
	m_list.ResetContent();

	int i;
	int p1,p2,p3;
	CString st1,st2,st3;

	CString s,s1;
	for(i=0; i<24; i++)
	{
		s="*"; s1="";
		p1=this_macro.cmd[i].p1; if(p1<-1) p1++;
		if((int)MAC_P1V(this_macro.cmd[i].f)!=0 && (int)MAC_P1V(this_macro.cmd[i].f)!=11)
			st1=macparams[MAC_P1V(this_macro.cmd[i].f)].name;
		else st1.Format(L"%d",p1);

		p2=this_macro.cmd[i].p2; if(p2<-1) p2++;
		if((int)MAC_P2V(this_macro.cmd[i].f)!=0 && (int)MAC_P2V(this_macro.cmd[i].f)!=11)
			st2=macparams[MAC_P2V(this_macro.cmd[i].f)].name;
		else st2.Format(L"%d",p2);

		p3=this_macro.cmd[i].p3; if(p3<-1) p3++;
		if((int)MAC_P3V(this_macro.cmd[i].f)!=0 && (int)MAC_P3V(this_macro.cmd[i].f)!=11)
			st3=macparams[MAC_P3V(this_macro.cmd[i].f)].name;
		else st3.Format(L"%d",p3);

		if(this_macro.cmd[i].n!=-1)
		{
			s.LoadString(this_macro.cmd[i].n);
			s1="";
			if(this_macro.cmd[i].p1!=-1)
			{
				if(this_macro.cmd[i].p2!=-1)
				{
					if(this_macro.cmd[i].p3!=-1) s1.Format(L"(%s,%s,%s)",st1,st2,st3);
					else s1.Format(L"(%s,%s)",st1,st2);
				} else s1.Format(L"(%s)",st1);
			}
			m_list.SetItemData(m_list.GetCount()-1,this_macro.cmd[i].n);
		}

		s1+="\t";
		if(this_macro.cmd[i].f&MAC_SUBP1) s1+="S";
		else s1+="0";
		if(this_macro.cmd[i].f&MAC_SUBP2) s1+="S";
		else s1+="0";
		if(this_macro.cmd[i].f&MAC_SUBP3) s1+="S";
		else s1+="0";
		if(this_macro.cmd[i].f&MAC_TONAME) s1+="N";
		if(this_macro.cmd[i].f&MAC_PAUSE) s1+="P";
		m_list.AddString(s+s1);
	}
	m_list.SetCurSel(0);
	SetDlgItemText(IDC_NAME,this_macro.name);
}

void CMacro::OnSave() 
{
	int i;
	wchar_t wc;
	CString name;
	GetDlgItemText(IDC_NAME,name);
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString macpath=helppath.Left(helppath.ReverseFind('\\')+1)+name+L".mac";
	CFile f;
	f.Open(macpath,CFile::modeCreate|CFile::modeWrite);

	f.Write(this_macro.cmd,24*sizeof(CZCommand));
	for(i=0; i<name.GetLength(); i++)
	{
		wc=name[i];
		f.Write(&wc,2);
	}
	f.Write("\0",2);

	f.Close();

	CString s1,s2;
	s1.LoadString(IDS_MACROSAVED); s2.LoadString(IDS_MACROEDITOR);
	CString s;
	s.Format(s1,name);
	MessageBox(s,s2,MB_OK);
}

void CMacro::OnOK() 
{
	if(command_modified)
	{
		command_modified=false;
		OnUpdate();
	}
	CDialog::OnOK();
}
