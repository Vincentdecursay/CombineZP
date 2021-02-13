// Progress.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "Progress.h"
#include ".\progress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgress dialog


CProgress::CProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CProgress::IDD, pParent)
	, minimized(false)
{
	m_cancel=false;
//	minimized=(AfxGetApp()->GetProfileInt(L"Minimized",L"Progress",false)==0);
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
	ON_BN_CLICKED(IDC_SELECTALL, OnBnClickedSelectall)
ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgress message handlers

void CProgress::OnCancel() 
{
	m_cancel=true;
	AddString(IDS_STRING619);
	AddString("");
}

void CProgress::OnMinimize() 
{
	ShowWindow(SW_MINIMIZE);
	minimized=true;
	AfxGetApp()->WriteProfileInt(L"Progress",L"Minimized",minimized);
}

void CProgress::Minimize()
{
	ShowWindow(SW_MINIMIZE);
	UpdateWindow();
}

void CProgress::Maximize()
{
	if(!minimized)
	{
		ShowWindow(SW_SHOW);
		ShowWindow(SW_SHOWNORMAL);
		SetForegroundWindow();
	} else ShowWindow(SW_MINIMIZE);
}

void CProgress::AddString(CString s)
{
	if(theApp.debug_switch) debug(s);
	while(m_list.GetCount()>=1000) m_list.DeleteString(0);
	m_list.AddString(s);
	m_list.SetTopIndex(m_list.GetCount()-1);
}

void CProgress::AddString(int id)
{
	CString s;
	s.LoadString(id);
	if(theApp.debug_switch) debug(s);
	while(m_list.GetCount()>=1000) m_list.DeleteString(0);
	m_list.AddString(s);
	m_list.SetTopIndex(m_list.GetCount()-1);
}

void CProgress::AddString(int id, int i)
{
	CString s0,s;
	s0.LoadString(id);
	s.Format(L"%s(%d)",s0,i);
	if(theApp.debug_switch) debug(s);
	while(m_list.GetCount()>=1000) m_list.DeleteString(0);
	m_list.AddString(s);
	m_list.SetTopIndex(m_list.GetCount()-1);
}

void CProgress::AddString(int id, int f1, int f2)
{
	CString s,s1;
	s1.LoadString(id);
	s.Format(L"%s(%d,%d)",s1,f1,f2);
	if(theApp.debug_switch) debug(s);
	while(m_list.GetCount()>=1000) m_list.DeleteString(0);
	m_list.AddString(s);
	m_list.SetTopIndex(m_list.GetCount()-1);
}

void CProgress::AddString(int id, int f1, int f2, int f3)
{
	CString s,s1;
	s1.LoadString(id);
	s.Format(L"%s(%d,%d,%d)",s1,f1,f2,f3);
	if(theApp.debug_switch) debug(s);
	while(m_list.GetCount()>=1000) m_list.DeleteString(0);
	m_list.AddString(s);
	m_list.SetTopIndex(m_list.GetCount()-1);
}

void CProgress::SetPos(int pos)
{
	m_progress.SetPos(pos);
	UpdateWindow();
}

void CProgress::SetRange(int low, int high)
{
	m_progress.SetRange((short)low,(short)high);
}

int CProgress::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_hicon=AfxGetApp()->LoadIcon(IDI_PROGRESS);
	SetIcon(m_hicon,true);
	minimized=(AfxGetApp()->GetProfileInt(L"Minimized",L"Progress",false)!=0);
	return 0;
}

void CProgress::OnCopy() 
{
	if(!OpenClipboard()) return;
	EmptyClipboard();
	HGLOBAL copied_text=GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE,64000);
	wchar_t *buffer=(wchar_t*)GlobalLock(copied_text);
	buffer[0]=0;
	CString s="";
	
	int i,item[1000],n=m_list.GetSelItems(1000,item);
	if(n)
	{
		for(i=0; i<n; i++)
		{
			m_list.GetText(item[i],s);
			wcscat(buffer,(s+L"\r\n"));
		}
	}

	GlobalUnlock(copied_text);
	SetClipboardData(CF_UNICODETEXT,(void*)copied_text);
	CloseClipboard();
}


void CProgress::debug(CString s)
{
	CString helppath=AfxGetApp()->m_pszHelpFilePath;
	CString debug_path=helppath.Left(helppath.ReverseFind('\\')+1);
	debug_path+="CZP_debug.txt";

	CFile f;
	int i;
	unsigned char ch;
	f.Open(debug_path,CFile::modeWrite|CFile::modeNoTruncate|CFile::modeCreate);
	f.Seek(0,CFile::end);

	for(i=0; i<s.GetLength(); i++)
	{
		ch=(char)s[i];
		f.Write(&ch,1);
	}
	f.Write("\r\n",2);
	f.Close();
}

void CProgress::debug_catchup(void)
{
	CString s;
	int i,n=m_list.GetCount();

	for(i=0; i<n; i++)
	{
		m_list.GetText(i,s);
		debug(s);
	}
}

void CProgress::OnBnClickedSelectall()
{
	m_list.SelItemRange(true,0,m_list.GetCount());
}

BOOL CProgress::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

void CProgress::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	static bool first=true;

	if(IsZoomed()) minimized=false;
 	CDialog::OnWindowPosChanged(lpwndpos);

	if(first)
	{
		minimized=(AfxGetApp()->GetProfileInt(L"Minimized",L"Progress",false)!=0);
		if(minimized) Minimize();
		else Maximize();
		first=false;
	} 
}
