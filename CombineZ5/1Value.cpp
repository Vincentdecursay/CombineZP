// 1Value.cpp : implementation file
//

#include "stdafx.h"
#include "combine.h"
#include "1Value.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C1Value dialog


C1Value::C1Value(CWnd* pParent /*=NULL*/)
	: CDialog(C1Value::IDD, pParent)
{
	//{{AFX_DATA_INIT(C1Value)
	m_value = 0;
	m_value_text = _T("");
	//}}AFX_DATA_INIT
}


void C1Value::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C1Value)
	DDX_Text(pDX, IDC_VALUE, m_value);
	DDX_Text(pDX, IDC_VALUE_TEXT, m_value_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(C1Value, CDialog)
	//{{AFX_MSG_MAP(C1Value)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C1Value message handlers
