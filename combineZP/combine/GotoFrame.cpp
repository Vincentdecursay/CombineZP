// GotoFrame.cpp : implementation file
//

#include "stdafx.h"
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
#include "GotoFrame.h"
#include ".\gotoframe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGotoFrame dialog


CGotoFrame::CGotoFrame(CWnd* pParent /*=NULL*/)
	: CDialog(CGotoFrame::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGotoFrame)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGotoFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGotoFrame)
	DDX_Control(pDX, IDC_USED, m_used);
	DDX_Control(pDX, IDC_UNUSED, m_unused);
	//}}AFX_DATA_MAP
	int f,index;
	for(f=top_frame-1; f>=0; f--)
	{
		if(buffer[f].hidden)
		{
			index=m_unused.AddString(buffer[f].name);
			m_unused.SetItemData(index,f);
			if(f==new_frame)
			{
				m_unused.SetCurSel(index);
				m_unused.SetTopIndex(index);
				DefID=IDC_UNUSED;
			}
		} else {
			index=m_used.AddString(buffer[f].name);
			m_used.SetItemData(index,f);
			if(f==new_frame)
			{
				m_used.SetCurSel(index);
				m_used.SetTopIndex(index);
				DefID=IDC_USED;
			}
		}
	}
	index=m_unused.AddString(buffer[PICTURE_BUFFER].name);
	m_unused.SetItemData(index,PICTURE_BUFFER);
	index=m_unused.AddString(buffer[DEPTH_BUFFER].name);
	m_unused.SetItemData(index,DEPTH_BUFFER);
	index=m_unused.AddString(buffer[OUT_BUFFER].name);
	m_unused.SetItemData(index,OUT_BUFFER);
}

BEGIN_MESSAGE_MAP(CGotoFrame, CDialog)
	//{{AFX_MSG_MAP(CGotoFrame)
	ON_LBN_SELCHANGE(IDC_UNUSED, OnSelchangeUnused)
	ON_LBN_SELCHANGE(IDC_USED, OnSelchangeUsed)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_OK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGotoFrame message handlers

void CGotoFrame::OnSelchangeUnused() 
{
	new_frame=(int)m_unused.GetItemData(m_unused.GetCurSel());
//	OnOK();
}

void CGotoFrame::OnSelchangeUsed() 
{
	new_frame=(int)m_used.GetItemData(m_used.GetCurSel());
//	OnOK();
}

BOOL CGotoFrame::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GotoDlgCtrl(GetDlgItem(DefID));
	
	return false;  // return TRUE unless you set the focus to a control
}

void CGotoFrame::OnBnClickedOk()
{
	OnOK();
}
