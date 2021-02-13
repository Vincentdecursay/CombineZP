// MovieControl.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "MovieControl.h"
#include ".\moviecontrol.h"


// CMovieControl dialog

IMPLEMENT_DYNAMIC(CMovieControl, CDialog)
CMovieControl::CMovieControl(CWnd* pParent /*=NULL*/)
	: CDialog(CMovieControl::IDD, pParent)
{
}

CMovieControl::~CMovieControl()
{
}

void CMovieControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if(m_state==IDC_MSTART)
	{
		GetDlgItem(IDC_MSTART)->EnableWindow(true);
		GetDlgItem(IDC_MEND)->EnableWindow(false);
		GetDlgItem(IDC_MCANCEL)->EnableWindow(true);
	} else {
		GetDlgItem(IDC_MSTART)->EnableWindow(false);
		GetDlgItem(IDC_MEND)->EnableWindow(true);
		GetDlgItem(IDC_MCANCEL)->EnableWindow(true);
	}
}

BEGIN_MESSAGE_MAP(CMovieControl, CDialog)
	ON_BN_CLICKED(IDC_MSTART, OnBnClickedMstart)
	ON_BN_CLICKED(IDC_MEND, OnBnClickedMend)
	ON_BN_CLICKED(IDC_MCANCEL, OnBnClickedMcancel)
END_MESSAGE_MAP()


// CMovieControl message handlers

void CMovieControl::OnBnClickedMstart()
{
	EndDialog(IDC_MSTART);
}

void CMovieControl::OnBnClickedMend()
{
	EndDialog(IDC_MEND);
}

void CMovieControl::OnBnClickedMcancel()
{
	EndDialog(IDC_MCANCEL);
}
