#pragma once
#include "afxwin.h"


// COptions dialog

class COptions : public CDialog
{
	DECLARE_DYNAMIC(COptions)

public:
	COptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptions();

// Dialog Data
	enum { IDD = IDD_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_reserve_ram;
	int m_startup_help;
	int m_startup_simple;
	int m_save_header;
	int m_cores;
	int m_miniprogress;
};
