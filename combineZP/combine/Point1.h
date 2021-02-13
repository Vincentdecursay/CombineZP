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
// Point1.h: interface for the Point1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINT1_H__BF7ECE61_C61D_4109_B836_3065A77C2D72__INCLUDED_)
#define AFX_POINT1_H__BF7ECE61_C61D_4109_B836_3065A77C2D72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Point1  
{
public:
	CString title;
	bool valid;
	int f;
	int y;
	int x;
	Point1();
	virtual ~Point1();

};

#endif // !defined(AFX_POINT1_H__BF7ECE61_C61D_4109_B836_3065A77C2D72__INCLUDED_)
