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

// combineDoc.h : interface of the CCombineDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMBINEDOC_H__A28F7CE7_BEAD_4EC2_988F_0EF4B734750A__INCLUDED_)
#define AFX_COMBINEDOC_H__A28F7CE7_BEAD_4EC2_988F_0EF4B734750A__INCLUDED_

#include "Size11.h"	// Added by ClassView
#include "combine.h"	// Added by ClassView
#include "Dft.h"	// Added by ClassView
#include "Progress.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct BUFFER
{
	CString name;
	void* pointer;
	int slot;
	int inuse;
	bool ondisk;
	bool updated;
	bool hidden;
};

struct BUFFER buffer[];

struct MEMORY_SLOT
{
	void* pointer;
	int buffer;
	bool vacant;
};

struct MEMORY_SLOT memory_slot[];

struct CZCommand
{
	int n,p1,p2,p3,f;
};

struct CZCommand default_command[];

struct CZMacro
{
	CString name;
	struct CZCommand cmd[25];
};

struct CZMacro macro[];

class CCombineDoc : public CDocument
{
protected: // create from serialization only
	CCombineDoc();
	DECLARE_DYNCREATE(CCombineDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCombineDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	int last_command;
	char * folder_path;
	void save_work();
	void silent_export_rectangles();
	bool silent;
	bool silent_save_out(char *file_path);
	bool silent_macro(char *mac_name);
	bool silent_open();
	static UINT silent_openT(LPVOID param);
	void Balance4x4(int *io,int *e);
	CString save_ext;
	CString save_path;
	void filterframe(PIXEL *in,PIXEL *out);
	void saveframe();
	int patch_threshold;
	bool filter_changed;
	int light_threshold;
	int dark_threshold;
	int jpeg_comp;
	int patch_size;
	static UINT skew_threaded(LPVOID param);
	void skew_threaded();
	int skew_amount;
	void skew(int amount);
	void monochrome();
	void unfix();
	void fix();
	bool macro_mode;
	unsigned char *packed;
	void gotoframe();
	void savedefaultmacros();
	void macronewframe(CString commands);
	CRect saved_mag;
	bool pause_enabled;
	void getvalue(int key);
	void page_right();
	void page_left();
	void page_down();
	void page_up();
	void reverseorder();
	void ImmediateButterworth();
	void ImmediateBandstop();
	void ImmediateBandpass();
	void ImmediateLowpass();
	void FilterDM();
	static UINT FilterStack(LPVOID param);
	void FilterStack();
	static UINT FilterWeighted(LPVOID param);
	void FilterWeighted();
	void FilterActive();
	void contrast();
	void reportspecialvalues();
	void Replace();
	bool loaddefaultmacros();
	int mac;
	static UINT DoMacroT(LPVOID param);
	void DoMacro();
	void Clear_macro_commands(int m);
	void InitializeMacros();
	void defaultmac01();
	void defaultmac02();
	void defaultmac03();
	void defaultmac04();
	void defaultmac05();
	void defaultmac06();
	void defaultmac07();
	void defaultmac08();
	void defaultmac09();
	void defaultmac10();
	bool backup_picture_valid;
	void saveallframes();
	static UINT filterallframes(LPVOID param);
	void filterallframes();
	void filteractiveframe();
	void filterreport();
	static UINT resizeall(LPVOID pointer);
	void resizeall();
	CRect goodrect;
	void Remove();
	static UINT StackInterpolate(LPVOID param);
	void StackInterpolate();
	bool refreshscreen;
	int group;
	static UINT Autoalignandsize2(LPVOID param);
	void Autoalignandsize2t();
	bool buissy;
	CProgress progress;
	unsigned long testshiftnscale(PIXEL *sample,PIXEL *master,int width,int height,int xoffset,int yoffset,float scale);
	void outwarn();
	void dftouttodepth();
	void namebuffers();
	bool reload();
	void loaddepth();
	void savedepth();
	void writefile(CFile *f,void* buffer);
	void readfile(CFile *f,void *buffer);
	void ImmediateHighpass();
	static UINT DoDepthflowT(LPVOID param);
	void DoDepthflow();
	void DoColourflow();
	static UINT balance(LPVOID param);
	void balance();
	static UINT localbalance(LPVOID param);
	void localbalance();
	void toggledepthmap();
	static UINT removesteps(LPVOID param);
	void removesteps();
	static UINT removeislandsT(LPVOID param);
	void removeislands();
	int min_island_size;
	void* ReuseBufferAs(int buffer,CString name);
	static UINT decimate(LPVOID param);
	void decimate();
	void specialreturn();
	static UINT detail(LPVOID param);
	void detail();
	static UINT patch(LPVOID param);
	void patch();
	static UINT lightdark(LPVOID param);
	void lightdark();
	int noise_level;
	bool GetValue();
	void gotopoint(int pt);
	void refresh_used_list();
	bool ReInitializeMemory();
	char file_list_buffer[MAX_FRAMES*300];
	bool skip_fsel;
	bool *depth_fixed[32];
	int *depth_big[32];
	int depth_depth;
	void RemoveBuffer(int b);
	void ResetBuffer(int b);
	void UnuseFrame(int used);
	int UseFrame(int frame);
	void TidyOuterEdges(int f);
	int frame_border;
	void* memory;
	void FreeMemory();
	bool InitializeMemory();
	unsigned long slot_size;
	unsigned long max_slots;
	void *NewBuffer(int number);
	int LocateFreeSlot();
	void *GetBuffer(int number);
	void *GetBufferFile(int f);
	void UpdateBuffer(int number);
	void FreeBuffer(int number);
	void SaveBuffer(int number,CString ext);
	void CopyBuffer(int in,int out);
	void SwapBuffers(int *a,int *b);

	int smallh;
	int smallw;
	int h;
	int w;
	int upsize(int x);

	int used_frame[LAST_BUFFER+1];
	int top_used_frame;
	int last_used_frame;
	int bottom_used_frame;
	int active_frame;
	int active_frame2;
	int special_active;
	int display;
	int top_frame;
	void active_down();
	void active_up();

	bool redraw_filter;
	void TidyEdges(int frame);
	void DepthFlow(int* big,int bigw,int bigh,bool* fixed);
	bool filter_reset;
	void dftOutAgain();
	void RefreshScreen();
	void dftOut();
	void dftReOut();
	void dftIn();
	void dftReset();
	void getvalue();
	void clearout();
	void filteractive();
	void editbrightness();
	void editthreshold();
	void editbandpass();
	void editbutterworth();
	void editbandstop();
	void edithighpass();
	void editlowpass();
	void rework_filter();
	bool tab_pressed;
	void alter_filter(int key);
	int filter_param;
	CDft dft;
	void fittocurve(double* y,int points);
	void clearunfixedfr();
	CString newnumber(CString s);
	void clearbg(int x,int y);
	bool open();
	static UINT openT(LPVOID param);
	void remove();
	void newfrompicture();
	void average();
//	void Autoalignandsize();
	static UINT Autoalignandsize(LPVOID param);
	void Autoalignandsizet();
	int buttons;
	bool saved_pm;
	void copy_forward();
	void prev_file();
	void next_file();
	CString this_file;
	bool new_flag;
	CRect old_mag;
	void special_rework();
	void copyback();
	void blend();
	double blend_factor;
	void leave_special();
	void special_reset();
	bool special_changed;
	void selectall();
	bool start_special();
	void replace();
	void delete_work();
	double add_factor;
	double correction_factor;
	void add();
	void correct(double bwcontrast,double bwnorm);
	void adjustcolour();
	float bfactor;
	float gfactor;
	float rfactor;
	int yorg;
	int xorg;
	void Redraw(CRect *r, bool flag);
	void redraw_picture();
	bool show_fixed;
	CString MakeAName();
	bool save_bmp;
	void viewfull();
	double scale;
	bool full_mode;
	void save_workstate();
	void clearunfixed();
	PIXEL* rainbow;
	bool depth_flag;
	void depthmap();
	void undo();
	void seedfill(int x,int y);
	int changed;
	void copyunder();
	void save_state();
	void sizecompensation();
	int contrast_delta; // FOR CONTRAST
	void Contrast();
	bool fix_active;
	void yout();
	void yin();
	void xout();
	void xin();
	int clienth;
	int clientw;
	int yfactor;
	int xfactor;
	void zoomout();
	void zoomin();
	int bw;
	void flash_fixed();
	float fbase;
	bool brushparams();
	bool positionparams();
	bool sizeparams();
	double colour_factor;
	double sharp_factor;
	CString path;
	void calc();
	int angle;
	int offsety;
	int offsetx;
	Size1 Sizer;
	void unplotdots(int x,int y);
	void plotdots(int x,int y);
	int sizeyorg;
	int sizexorg;
	unsigned int wipeout;
	void toggle_plane(int x,int y);
	int brush_delta;
	void parametrers();
	void home();
	void Size();
	bool Reload();
	int size_factor;
	int brush_size;
	void flash();
	void plane_down();
	void plane_up();
	bool plane_mode;
	void draw_plane();
	void end_plane();
	void unplotdot(int x,int y);
	void plotdot(int x,int y);
	bool magon;
	CRect mag;
	int mode;
	void right();
	void left();
	void down();
	void up();
	void* bmh;
	BITMAPINFOHEADER bmih;
	BITMAPFILEHEADER bmfh;
	virtual ~CCombineDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCombineDoc)
	afx_msg void OnFix();
	afx_msg void OnUnfix();
	afx_msg void OnParameters();
	afx_msg void OnMakefirst();
	afx_msg void OnMakelast();
	afx_msg void OnAllframes();
	afx_msg void OnClear();
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnFileRemove();
	afx_msg void OnSaveparameters();
	afx_msg void OnRestoreparameters();
	afx_msg void OnEditCopy();
	afx_msg void OnCopyunder();
	afx_msg void OnClearunfixed();
	afx_msg void OnSaveallframes();
	afx_msg void OnFileSaveframe();
	afx_msg void OnSizecompensation();
	afx_msg bool OnFileReload();
	afx_msg void OnFileReloadall();
	afx_msg void OnResizeall();
	afx_msg void OnBalance();
	afx_msg void OnSizeCalculate();
	afx_msg void OnSizeClearpoints();
	afx_msg void OnFrameReplace();
	afx_msg void OnFrameNewfrompicture();
	afx_msg void OnSpecialActivered();
	afx_msg void OnSpecialActivegreen();
	afx_msg void OnSpecialActivebule();
	afx_msg void OnSpecialMaketransparent();
	afx_msg void OnFrameUse();
	afx_msg void OnFrameDontuse();
	afx_msg void OnViewpicture();
	afx_msg void OnDecimate();
	afx_msg void OnViewTogglefix();
	afx_msg void OnSpecialContrast();
	afx_msg void OnFrameAutoalignandsize();
	afx_msg void OnRectangleUndo();
	afx_msg void OnViewInformation();
	afx_msg void OnFileSaverectangle();
	afx_msg void OnFileNew();
	afx_msg void OnViewToggledepthmap();
	afx_msg void OnFileBatchprocessor();
	afx_msg void OnFileResumeWork();
	afx_msg void OnViewFullmode();
	afx_msg void OnSpecialMakevisibleactiveframe2();
	afx_msg void OnSpecialCorrectBrightness();
	afx_msg void OnSpecialLeavespecialmode();
	afx_msg void OnRectangleInvertfixed();
	afx_msg void OnSpecialBrightness();
	afx_msg void OnSpecialReset();
	afx_msg void OnViewSelectall();
	afx_msg void OnSpecialBlend();
	afx_msg void OnFileImport();
	afx_msg void OnFrameMakemonochrome();
	afx_msg void OnSpecialAdd();
	afx_msg void OnFileImportdepthmap();
	afx_msg void OnFrameMakebg();
	afx_msg void OnFileExportrectangles();
	afx_msg void OnFilterCreatelowpass();
	afx_msg void OnFilterCreatehighpass();
	afx_msg void OnFilterCreatebandpass();
	afx_msg void OnFilterCreatebutterworth();
	afx_msg void OnFilterNormalOutput();
	afx_msg void OnFilterCreatebandstop();
	afx_msg void OnFilterThreshold();
	afx_msg void OnFilterBrightness();
	afx_msg void OnFilterResumefiltering();
	afx_msg void OnFilterTransformarithmeticMakexfromactiveframe();
	afx_msg void OnFilterFilterallframes();
	afx_msg void OnFrameReverseorder();
	afx_msg void OnFilterSumofstack();
	afx_msg void OnStackDetail();
	afx_msg void OnStackDepthflow();
	afx_msg void OnFilterMakexfromdepthmap();
	afx_msg void OnFilterRedisplaylastoutput();
	afx_msg void OnFrameTidyedges();
	afx_msg void OnFilterFreememory();
	afx_msg void OnFileSaveworkstate();
	afx_msg void OnViewOutput();
	afx_msg void OnSpecialDostack();
	afx_msg void OnStackRemoveislands();
	afx_msg void OnStackRemovesteps();
	afx_msg void OnFileWorkframesReloaddepth();
	afx_msg void OnFileWorkframesSavedepth();
	afx_msg void OnUndoSetundopoint();
	afx_msg void OnStackInterpolate();
	afx_msg void OnFiltertodepth();
	afx_msg void OnMacroEditmacro();
	afx_msg void OnStackSizeandalignmentAutomaticregularstack();
	afx_msg void OnViewProgress();
	afx_msg void OnFileExportdepthmap();
	afx_msg void OnMacro1();
	afx_msg void OnMacro10();
	afx_msg void OnMacro2();
	afx_msg void OnMacro3();
	afx_msg void OnMacro4();
	afx_msg void OnMacro5();
	afx_msg void OnMacro6();
	afx_msg void OnMacro7();
	afx_msg void OnMacro8();
	afx_msg void OnMacro9();
	afx_msg void OnMacroSaveasdefaultset();
	afx_msg void OnMacroRestoredefaults();
	afx_msg void OnMacroDisablepause();
	afx_msg void OnMacroLoadmacroset();
	afx_msg void OnMacroClearallmacros();
	afx_msg void OnViewToggleframesa1a2();
	afx_msg void OnViewRestorepicture();
	afx_msg void OnFrameCopyvisibletoout();
	afx_msg void OnAppExit();
	afx_msg void OnViewGotoframe();
	afx_msg void OnStackHorizontalskew();
	afx_msg void OnStackPatchwork();
	afx_msg void OnFilterInputWeightedsum();
	afx_msg void OnStackDetailLightanddark();
	afx_msg void OnStackBalancecolourandbrightnessLocal();
	afx_msg void OnStackBalance4x4();
	afx_msg void OnMovieMake();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBINEDOC_H__A28F7CE7_BEAD_4EC2_988F_0EF4B734750A__INCLUDED_)
