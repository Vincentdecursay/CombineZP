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
#if !defined(AFX_SIZE11_H__9099B4F1_B92E_4E09_AFCD_ECBB18EC6D88__INCLUDED_)
#define AFX_SIZE11_H__9099B4F1_B92E_4E09_AFCD_ECBB18EC6D88__INCLUDED_

#include "Point1.h"	// Added by ClassView
#include "combineDoc.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Size11.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Size1 dialog

class Size1 : public CDialog
{
// Construction
public:
	double length34;
	double length12;
	int angle;
	double ratio;
	Point1 p[4];
	void Calculate();
	Size1(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Size1)
	enum { IDD = 134 };
	CString	m_f1p1;
	CString	m_f1p2;
	CString	m_f1p3;
	CString	m_f1p4;
	CString	m_ratio;
	CString	m_length12;
	CString	m_length34;
	CString	m_angle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Size1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Size1)
	afx_msg void OnSetparams();
	afx_msg void OnSizereg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIZE11_H__9099B4F1_B92E_4E09_AFCD_ECBB18EC6D88__INCLUDED_)
