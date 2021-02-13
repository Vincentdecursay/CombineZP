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
// Dft.h: interface for the CDft class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DFT_H__34D64603_89D9_4CAF_A26C_C940C80D10F2__INCLUDED_)
#define AFX_DFT_H__34D64603_89D9_4CAF_A26C_C940C80D10F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "fftw3.h"

class CDft  
{
public:
	float ming;
	float minb;
	float minr;
	void OutToDM(void *d,int max,void *in);
	float zero_value;
	void TransformIn1(void *io);
	void TransformOut1(void *io);
	CString wise_path;
	CDft();
	void Reset(int w,int h,CString path, void *io);
	void Destroy();
	virtual ~CDft();

	int mode;
	int out_mode;
	int filter_param;

	bool initialized;
	bool filtering;

	bool valid_transform1;
	bool valid_transform3;
	bool valid_picture1;
	bool valid_picture3;
	bool valid_store1;
	bool valid_store3;
	bool valid_filter;

	float order;
	float cutoff;
	float displacement;
	float width;
	float brightness;
	float threshold;
	float centre;

	float big_delta;
	float small_delta;

	static UINT dft1r(LPVOID param);
	static UINT dft1rgb(LPVOID param);
	void In3(void *f,void *inr,void *ing,void *inb,bool bandw);
	void In1(void *f,void *in);
	void InStack(void *f,int n, void *inr,void *ing,void *inb,bool bandw);
	static UINT dft2r(LPVOID param);
	static UINT dft2g(LPVOID param);
	static UINT dft2b(LPVOID param);
	static UINT dft3(LPVOID param);
	void InWeighted(void *f,int n, void *inr,void *ing,void *inb,void *inw,void *temp,bool bandw);

	void TransformIn(void *r,void *g,void *b,bool bandw);

	void Filter(void *f,void *r,void *g,void *b,bool bandw);

	void TransformOut(void *r,void *g,void *b,bool bandw);

	void Out3(void *f,void *inr,void *ing,void *inb,bool bandw);
	void Brightness(void *frame,void *inr,void *ing,void *inb,bool bandw);
	void Threshold(void *frame,void *inr,void *ing,void *inb,bool bandw);
	void Out1(void *f,int max,void *in);
	
	void Low(void *f);
	void High(void *f);
	void Pass(void *f);
	void Stop(void *f);
	void Butterworth(void *f);

	void VisualizeFilter(void* frame,void *f);

private:
	int h;
	int w;

	fftwf_plan pin;
	fftwf_plan pout;
};

#endif // !defined(AFX_DFT_H__34D64603_89D9_4CAF_A26C_C940C80D10F2__INCLUDED_)
