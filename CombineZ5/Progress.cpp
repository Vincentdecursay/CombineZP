// Progress.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "Progress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgress dialog


CProgress::CProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CProgress::IDD, pParent)
{
	m_cancel=false;
	//{{AFX_DATA_INIT(CProgress)
	//}}AFX_DATA_INIT
}


void CProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgress)
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_TEXT, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgress, CDialog)
	//{{AFX_MSG_MAP(CProgress)
	ON_BN_CLICKED(IDC_MINIMIZE, OnMinimize)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgress message handlers

void CProgress::OnCancel() 
{
	AddString("Operation Cancelled");
	AddString("");
	m_cancel=true;
}

void CProgress::OnMinimize() 
{
	ShowWindow(SW_MINIMIZE);
}

void CProgress::Minimize()
{
	ShowWindow(SW_MINIMIZE);
}

void CProgress::Maximize()
{
	ShowWindow(SW_SHOWNORMAL);
	SetForegroundWindow();
}

void CProgress::AddString(CString s)
{
	while(m_list.GetCount()>=1000) m_list.DeleteString(0);
	m_list.AddString(s);
	m_list.SetTopIndex(m_list.GetCount()-1);
//	UpdateWindow();
}

void CProgress::AddString(int id)
{
	CString s;
	s.LoadString(id);
	while(m_list.GetCount()>=1000) m_list.DeleteString(0);
	m_list.AddString(s);
	m_list.SetTopIndex(m_list.GetCount()-1);
//	UpdateWindow();
}

void CProgress::AddString(int id, int i)
{
	CString s0,s;
	s0.LoadString(id);
	s.Format("%s(%d)",s0,i);
	while(m_list.GetCount()>=1000) m_list.DeleteString(0);
	m_list.AddString(s);
	m_list.SetTopIndex(m_list.GetCount()-1);
//	UpdateWindow();
}
/*
void CProgress::AddString(int id, float f)
{
	CString s0,s;
	s0.LoadString(id);
	s.Format("%s(%0.4f)",s0,f);
	while(m_list.GetCount()>=1000) m_list.DeleteString(0);
	m_list.AddString(s);
	m_list.SetTopIndex(m_list.GetCount()-1);
	UpdateWindow();
}

void CProgress::AddString(CString s0, float f)
{
	CString s;
	s.Format("%s(%0.4f)",s0,f);
	while(m_list.GetCount()>=1000) m_list.DeleteString(0);
	m_list.AddString(s);
	m_list.SetTopIndex(m_list.GetCount()-1);
	UpdateWindow();
}
*/
void CProgress::AddString(int id, int f1, int f2)
{
	CString s,s1;
	s1.LoadString(id);
	s.Format("%s(%d,%d)",s1,f1,f2);
	while(m_list.GetCount()>=1000) m_list.DeleteString(0);
	m_list.AddString(s);
	m_list.SetTopIndex(m_list.GetCount()-1);
//	UpdateWindow();
}

void CProgress::AddString(int id, int f1, int f2, int f3)
{
	CString s,s1;
	s1.LoadString(id);
	s.Format("%s(%d,%d,%d)",s1,f1,f2,f3);
	while(m_list.GetCount()>=1000) m_list.DeleteString(0);
	m_list.AddString(s);
	m_list.SetTopIndex(m_list.GetCount()-1);
//	UpdateWindow();
}

void CProgress::SetPos(int pos)
{
	m_progress.SetPos(pos);
	UpdateWindow();
}

void CProgress::SetRange(int low, int high)
{
	m_progress.SetRange(low,high);
}

int CProgress::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_hicon=AfxGetApp()->LoadIcon(IDI_PROGRESS);
	SetIcon(m_hicon,true);
	return 0;
}

void CProgress::OnCopy() 
{
	if(!OpenClipboard()) return;
	EmptyClipboard();
	HGLOBAL copied_text=GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE,64000);
	char *buffer=(char*)GlobalLock(copied_text);
	buffer[0]=0;
	CString s="";
	
	int i,item[1000],n=m_list.GetSelItems(1000,item);
	if(n)
	{
		for(i=0; i<n; i++)
		{
			m_list.GetText(item[i],s);
			strcat(buffer,(s+"\n\r"));
		}
	}

	GlobalUnlock(copied_text);
	SetClipboardData(CF_TEXT,(void*)copied_text);
	CloseClipboard();
}

