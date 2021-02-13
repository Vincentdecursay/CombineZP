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
#if !defined(AFX_REGISTERDIA_H__30831F45_316C_451D_A40D_D80EBAA89458__INCLUDED_)
#define AFX_REGISTERDIA_H__30831F45_316C_451D_A40D_D80EBAA89458__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegisterDia.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegisterDia dialog

class CRegisterDia : public CDialog
{
// Construction
public:
	CRegisterDia(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterDia)
	enum { IDD = IDD_DIALOG5 };
	int		m_a;
	int		m_f;
	int		m_x;
	int		m_y;
	int		m_oy;
	int		m_ox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterDia)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegisterDia)
	afx_msg void OnReset();
	afx_msg void OnUpdateSox();
	afx_msg void OnChangeSox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTERDIA_H__30831F45_316C_451D_A40D_D80EBAA89458__INCLUDED_)
