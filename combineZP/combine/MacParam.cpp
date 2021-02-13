// MacParam.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "MacParam.h"
#include ".\macparam.h"


// CMacParam dialog

IMPLEMENT_DYNAMIC(CMacParam, CDialog)
CMacParam::CMacParam(CWnd* pParent /*=NULL*/)
	: CDialog(CMacParam::IDD, pParent)
	, value1(0)
	, value2(0)
	, value3(0)
	, value4(0)
	, value5(0)
	, value6(0)
	, value7(0)
	, value8(0)
	, value9(0)
	, value10(0)
	, name1(_T(""))
	, name2(_T(""))
	, name3(_T(""))
	, name4(_T(""))
	, name5(_T(""))
	, name6(_T(""))
	, name7(_T(""))
	, name8(_T(""))
	, name9(_T(""))
	, name10(_T(""))
	, enable(false)
{
}

CMacParam::~CMacParam()
{
}

void CMacParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MP1V, value1);
	DDX_Text(pDX, IDC_MP2V, value2);
	DDX_Text(pDX, IDC_MP3V, value3);
	DDX_Text(pDX, IDC_MP4V, value4);
	DDX_Text(pDX, IDC_MP5V, value5);
	DDX_Text(pDX, IDC_MP6V, value6);
	DDX_Text(pDX, IDC_MP7V, value7);
	DDX_Text(pDX, IDC_MP8V, value8);
	DDX_Text(pDX, IDC_MP9V, value9);
	DDX_Text(pDX, IDC_MP10V, value10);
	DDX_Text(pDX, IDC_MP1N, name1);
	DDX_Text(pDX, IDC_MP2N, name2);
	DDX_Text(pDX, IDC_MP3N, name3);
	DDX_Text(pDX, IDC_MP4N, name4);
	DDX_Text(pDX, IDC_MP5N, name5);
	DDX_Text(pDX, IDC_MP6N, name6);
	DDX_Text(pDX, IDC_MP7N, name7);
	DDX_Text(pDX, IDC_MP8N, name8);
	DDX_Text(pDX, IDC_MP9N, name9);
	DDX_Text(pDX, IDC_MP10N, name10);

	GetDlgItem(IDC_MP1N)->EnableWindow(enable);
	GetDlgItem(IDC_MP2N)->EnableWindow(enable);
	GetDlgItem(IDC_MP3N)->EnableWindow(enable);
	GetDlgItem(IDC_MP4N)->EnableWindow(enable);
	GetDlgItem(IDC_MP5N)->EnableWindow(enable);
	GetDlgItem(IDC_MP6N)->EnableWindow(enable);
	GetDlgItem(IDC_MP7N)->EnableWindow(enable);
	GetDlgItem(IDC_MP8N)->EnableWindow(enable);
	GetDlgItem(IDC_MP9N)->EnableWindow(enable);
	GetDlgItem(IDC_MP10N)->EnableWindow(enable);

	if(!enable)
	{
		if(name1[0]!='_')
		{
			GetDlgItem(IDC_MP1N)->EnableWindow(true);
			SendDlgItemMessage(IDC_MP1N,EM_SETREADONLY,true,0);
			GetDlgItem(IDC_MP1V)->EnableWindow(true);
		} else GetDlgItem(IDC_MP1V)->EnableWindow(false);

		if(name2[0]!='_')
		{
			GetDlgItem(IDC_MP2N)->EnableWindow(true);
			SendDlgItemMessage(IDC_MP2N,EM_SETREADONLY,true,0);
			GetDlgItem(IDC_MP2V)->EnableWindow(true);
		} else GetDlgItem(IDC_MP2V)->EnableWindow(false);

		if(name3[0]!='_')
		{
			GetDlgItem(IDC_MP3N)->EnableWindow(true);
			SendDlgItemMessage(IDC_MP3N,EM_SETREADONLY,true,0);
			GetDlgItem(IDC_MP3V)->EnableWindow(true);
		} else GetDlgItem(IDC_MP3V)->EnableWindow(false);

		if(name4[0]!='_')
		{
			GetDlgItem(IDC_MP4N)->EnableWindow(true);
			SendDlgItemMessage(IDC_MP4N,EM_SETREADONLY,true,0);
			GetDlgItem(IDC_MP4V)->EnableWindow(true);
		} else GetDlgItem(IDC_MP4V)->EnableWindow(false);

		if(name5[0]!='_')
		{
			GetDlgItem(IDC_MP5N)->EnableWindow(true);
			SendDlgItemMessage(IDC_MP5N,EM_SETREADONLY,true,0);
			GetDlgItem(IDC_MP5V)->EnableWindow(true);
		} else GetDlgItem(IDC_MP5V)->EnableWindow(false);

		if(name6[0]!='_')
		{
			GetDlgItem(IDC_MP6N)->EnableWindow(true);
			SendDlgItemMessage(IDC_MP6N,EM_SETREADONLY,true,0);
			GetDlgItem(IDC_MP6V)->EnableWindow(true);
		} else GetDlgItem(IDC_MP6V)->EnableWindow(false);

		if(name7[0]!='_')
		{
			GetDlgItem(IDC_MP7N)->EnableWindow(true);
			SendDlgItemMessage(IDC_MP7N,EM_SETREADONLY,true,0);
			GetDlgItem(IDC_MP7V)->EnableWindow(true);
		} else GetDlgItem(IDC_MP7V)->EnableWindow(false);

		if(name8[0]!='_')
		{
			GetDlgItem(IDC_MP8N)->EnableWindow(true);
			SendDlgItemMessage(IDC_MP8N,EM_SETREADONLY,true,0);
			GetDlgItem(IDC_MP8V)->EnableWindow(true);
		} else GetDlgItem(IDC_MP8V)->EnableWindow(false);

		if(name9[0]!='_')
		{
			GetDlgItem(IDC_MP9N)->EnableWindow(true);
			SendDlgItemMessage(IDC_MP9N,EM_SETREADONLY,true,0);
			GetDlgItem(IDC_MP9V)->EnableWindow(true);
		} else GetDlgItem(IDC_MP9V)->EnableWindow(false);

		if(name10[0]!='_')
		{
			GetDlgItem(IDC_MP10N)->EnableWindow(true);
			SendDlgItemMessage(IDC_MP10N,EM_SETREADONLY,true,0);
			GetDlgItem(IDC_MP10V)->EnableWindow(true);
		} else GetDlgItem(IDC_MP10V)->EnableWindow(false);
	}
}


BEGIN_MESSAGE_MAP(CMacParam, CDialog)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CMacParam message handlers
void CMacParam::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if(r.right!=0) MoveWindow(&r,true);
	r.right=0;
}

void CMacParam::OnClose()
{
	GetWindowRect(&r);
	CDialog::OnClose();
}

void CMacParam::OnBnClickedOk()
{
	GetWindowRect(&r);
	OnOK();
}
