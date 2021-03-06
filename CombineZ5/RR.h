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
#if !defined(AFX_RR_H__34F595EA_2943_4DE5_9FDF_F8570E73B2B3__INCLUDED_)
#define AFX_RR_H__34F595EA_2943_4DE5_9FDF_F8570E73B2B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RR.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRR dialog

class CRR : public CDialog
{
// Construction
public:
	CRR(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRR)
	enum { IDD = IDD_DIALOG12 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRR)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRR)
	afx_msg void OnRr90();
	afx_msg void OnRr180();
	afx_msg void OnRr270();
	afx_msg void OnRr0();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RR_H__34F595EA_2943_4DE5_9FDF_F8570E73B2B3__INCLUDED_)
