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
#include "macparam.h"

#include "objbase.h"
#include "Dshow.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//extern DWORD still_cookie;
//extern DWORD video_cookie;

//extern int frame_similarity;

#include "gdiplus.h"
using namespace Gdiplus;

extern IGraphBuilder *Graph;

#define PD_FILLED 1
#define PD_SMALLER 2
struct PDEPTH {
	unsigned short flags;
	unsigned short frame;
};

struct BUFFER
{
	CString name;
	void* pointer;
	int slot;
	int inuse;
	unsigned int age;
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
	int n,p1,p2,p3;
	unsigned int f;
};

struct CZCommand default_command[];

struct CZMacro
{
	CString name;
	struct CZCommand cmd[25];
};

struct CZMacro macro[];

struct CZExtCommand
{
	wchar_t name[128];
	wchar_t program[128];
	wchar_t line[128];
};

struct CZExtCommand_old
{
	char name[128];
	char program[128];
	char line[128];
};

struct CZExtCommand extcommand[];

struct CZMacParams
{
	wchar_t name[128];
	int value;
};

struct CZMacParams_old
{
	char name[128];
	int value;
};

#define LAST_MACPARAM 109
extern struct CZMacParams macparams[];

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
	void loadspecial(int number);
	void savespecial(int number);

	UINT bmpprop_size;
	UINT bmpprop_count;
	char* bmp_properties;

	CString str1;
	HWND help_window;
	int buffer_count;
	void Dump();
	void update_macparams();
	void compare_frames();
	int frame_similarity;

	int maxangle;
	int movw;
	int movh;
	int movt;
	int movq;
	int last_command;
	wchar_t * folder_path;
	void save_work();
	void silent_export_rectangles();
	bool silent;
	bool silent_save_out(CString file_path);
	bool silent_save_active(int quality);
	bool silent_save_depth(wchar_t *file_path);
	bool silent_macro(wchar_t *mac_name);
	bool silent_open();
	static UINT silent_openT(LPVOID param);
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
	int skew_amountv;
	void skew(int amount,int amountv);
	void monochrome();
	void unfix();
	void fix();
	bool macro_mode;
	unsigned char *packed;
	void gotoframe();
	void savedefaultmacros();
	void macronewframe(CString commands);
	void macronewfromdepthmap(CString commands);
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
	void Refilter();
	void FilterDM();
	static UINT FilterStack(LPVOID param);
	void FilterStack();
	static UINT ws1(LPVOID param);
	static UINT ws2(LPVOID param);
	static UINT ws3(LPVOID param);
	static UINT ws4(LPVOID param);
	static UINT FilterWeighted(LPVOID param);
	static UINT ws2a(LPVOID param);
	static UINT ws4a(LPVOID param);
	static UINT FilterWeightedWC(LPVOID param);
	void FilterWeighted();
	void FilterWeightedWC();
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
	static UINT ra(LPVOID param);
	void resizeall();
	CRect goodrect;
	void Remove();
	static UINT StackInterpolate(LPVOID param);
	void StackInterpolate();
	bool refreshscreen;
	int group;
	static UINT Autoalignandsize2(LPVOID param);
	void Autoalignandsize2t();
	static UINT Astroalign(LPVOID param);
	static UINT shrotalign(LPVOID param);
	void Astroalignt();
	static UINT rot(LPVOID param);
	void shrotalignt();
	bool buissy;
	CProgress progress;
	__int64 testshiftnrotate(PIXEL *sample,PIXEL *master,int width,int height,int xoffset,int yoffset,double angle);
	static UINT tss(LPVOID param);
	__int64 testshiftnscale(PIXEL *sample,PIXEL *master,int width,int height,int xoffset,int yoffset,float scale);
	static UINT tss2(LPVOID param);
	__int64 testshiftnscale2(PIXEL *sample,PIXEL *master,int width,int height,int xoffset,int yoffset,float scale);
	void outwarn();
	void dftouttodepth();
	void namebuffers();
	bool reload();
	void loaddepth();
	void savedepth();
//	void writefile(CFile *f,void* buffer);
	static UINT wpf(LPVOID param);
	void writepfile(CFile *f,void* buffer);
//	void readfile(CFile *f,void *buffer);
	static UINT rpf(LPVOID param);
	void readpfile(CFile *f,void *buffer);
	void ImmediateHighpass();
	static UINT DoDepthflowT(LPVOID param);
	static UINT ddf1(LPVOID param);
	static UINT ddf2(LPVOID param);
	static UINT ddf3(LPVOID param);
	static UINT ddf4(LPVOID param);
	void DoDepthflow();
	void DoColourflow();
	static UINT balance(LPVOID param);
	void balance();
	static UINT balancebrightness(LPVOID param);
	void balancebrightness();
	static UINT localbalance(LPVOID param);
	void localbalance();
	static UINT localbalancecolour(LPVOID param);
	void localbalancecolour();
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
	static UINT detail1(LPVOID param);
	static UINT detail2(LPVOID param);
	static UINT detail(LPVOID param);
	void detail();
	static UINT patch(LPVOID param);
	void patch();
	static UINT lightdark(LPVOID param);
	void lightdark();
	static UINT stackaverage(LPVOID param);
	void stackaverage();
	int noise_level;
	bool GetValue();
	void gotopoint(int pt);
	void refresh_used_list();
	bool ReInitializeMemory();
	wchar_t file_list_buffer[MAX_FRAMES*300];
	bool skip_fsel;
	bool *depth_fixed[64];
	int *depth_big[64];
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
	static UINT cb(LPVOID param);
	void CopyBuffer(int in,int out);
	bool ReleaseASlot();

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
	static UINT df1(LPVOID param);
	static UINT df2(LPVOID param);
	static UINT df3(LPVOID param);
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
	void newfromvisible();
	void average();
	static UINT split(LPVOID param);
	static UINT sax(LPVOID param);
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
	static UINT c1(LPVOID param);
	void correct(int mode);
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
	void seedfillout(int x,int y,int frame);
	int changed;
	void copyunder();
	void save_state();
	static UINT rrm(LPVOID param);
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
	void plotoutdots(int x,int y,int frame);
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
	void plotoutdot(int x,int y,int frame);
	void plotdot(int x,int y);
	bool magon;
	CRect mag;
	int mode;
	static UINT move_right(LPVOID param);
	void right();
	static UINT move_left(LPVOID param);
	void left();
//	static UINT move_down(LPVOID param);
	void down();
//	static UINT move_up(LPVOID param);
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
	afx_msg void OnFileReload();
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
	afx_msg void OnFrameUp();
	afx_msg void OnFrameDown();
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
	afx_msg void OnMovieMake();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void Copyvisibletoout();
	void saveproperties(Bitmap *bm1,CString ext);
	afx_msg void OnMovieExtractframes();
	void CopyToOut(void);
	void InsertAbove(CString name);
	void InsertBelow(CString name);
	static UINT MovieMakeT(LPVOID param);
	static UINT MovieExtractT(LPVOID param);
	void MovieMake();
	void MovieExtract();

	afx_msg void OnAcquireimageSelectsource();
	afx_msg void OnAquireimageGetimage();
	afx_msg void OnAcquireimageTogglevideo();
	void togglevideo(void);
	void getimage(void);
	void endvideo(void);
	void startvideo(void);
	bool extract;

	afx_msg void OnCommandEditcommands();
	afx_msg void OnCommandClearallcommands();
	afx_msg void OnCommandSavecommandset();
	afx_msg void OnCommandLoadcommandset();
	void InitializeExtCommands(void);
	void MacrobyNumber(int n);
	void initializecommands(void);
	void DoExtCom(int com, int p1, int p2, int p3);
	afx_msg void OnMovieExportmovieframes();
	bool bandw;
	void checkdepth(void);
	afx_msg void OnExcom11();
	afx_msg void OnExcom12();
	afx_msg void OnSyscom13();
	void macro_preamble(void);
	bool video_test(int m);
	afx_msg void OnEditMacParams();
	void initializemacparams();
	void initmacparamdlg(CMacParam* p,bool enable);
	void getmacparamsfromdlg(CMacParam* p);
	void GetMacParamValues();
	afx_msg void OnInputWeightedsumWC();
	afx_msg void OnBalancecolourandbrightnessLocalcolouronly();
	afx_msg void OnStackAveragetoout();
	int hdr_bright;
	int hdr_log;
	int hdr_hia;
	int Extract_redend;
	int Extract_bluend;
	int Extract_th;
	int Squeeze_redend;
	int Squeese_bluend;
	int Squeeze_break;
	int Noise_detect;
	int Noise_sub;
	int Noise_lim;
	int Tle;
	int Tlt;
	int Teq;
	int Tge;
	int Tgt;
	int Tne;

	afx_msg void OnSizeandalignmentAstronomical();
	afx_msg void OnBalancecolourandbrightnessGlobalbrightness();
	afx_msg void OnFrameSplitrgb();
	afx_msg void OnFrameInvert();
	afx_msg void OnFrameReducenoise();
	afx_msg void OnFrameStretchcolours();
	afx_msg void OnFrameSqueezespectrum();
	afx_msg void OnFrameExtractcolourrange();
	afx_msg void OnFrameSplithsg();
	afx_msg void OnFrameColourproduct();
	afx_msg void OnFrameNewfromvisible();
	afx_msg void OnColourRainbow();
	afx_msg void OnFrameNewfromdepthmap();
	afx_msg void OnHelpCombinezphelp();
	int detail_size;
	int detail_filter;
	afx_msg void OnPyramidClearpyramid();
	static UINT PyramidClearT(LPVOID param);
	static UINT pc7(LPVOID param);
	void PyramidClear();
	void initialize_pyramid();
	afx_msg void OnMergeactivewithpyramidMaximumrule();
	static UINT PyramidMaxT(LPVOID param);
	static UINT PyramidMaxUFT(LPVOID param);
	void PyramidMax(int frame);
	static UINT pmc5(LPVOID param);
	static UINT pmc6(LPVOID param);
	void PyramidMax1(int number, int frame, int channel);
	void ptempbuild();
	static UINT pmc2(LPVOID param);
	static UINT pmc3(LPVOID param);
	static UINT pmc4(LPVOID param);
	void pdecend(int* in,int level,int* out);
	static UINT pwa4(LPVOID param);
	static UINT pwa5(LPVOID param);
	static UINT pwa6(LPVOID param);
	void pdecendw(int* in,int level,int* out);
	afx_msg void OnPyramidPyramidtoout();
	static UINT pmc1(LPVOID param);
	void psmoothe(int *in,int level,int *out);
	void psetEdges(int *buffer,int level);
	void psetEdges_small(int *buffer,int level);
	static UINT PyramidToOutT(LPVOID param);
	void PyramidToOut();
	void PyramidToOut1(int number,int *t);
	void pascend(int* in,int level,int* out);
	static UINT PyramidDepthT(LPVOID param);
	void PyramidDepth(int levels,int noise);
	void PyramidDepth1(int number,int levels,int noise);
	void pdascend(int* inlarge,int* insmall,PDEPTH* ind,int level,PDEPTH* out,int noise);
	void pdout3();
	int plevels;
	int pnumber;
	int pnoise;
	int pframe;
	int plast;
	int pdepth;
	int pforeground;
	int pbrightness;
	int pbackground;
	static UINT pmcr(LPVOID param);
	static UINT pmcg(LPVOID param);
	static UINT pmcb(LPVOID param);
	void pinr(int frme);
	void ping(int frme);
	void pinb(int frme);
	static UINT pwa3(LPVOID param);
	void pinw(int frme);
	void pout3(int *t1,int *t2,int *t3);
	int plast_level;
	afx_msg void OnFileSetoptions();
	afx_msg void OnPyramidRecoverdepth();
	afx_msg void OnRgbweightedaverageruleAddactivetypyramid();
	afx_msg void OnRgbweightedaverageruleResulttoout();
	static UINT PyramidWeightT(LPVOID param);
	static UINT PyramidWeightUFT(LPVOID param);
	void PyramidWeight(int frame);
	static UINT pwa1(LPVOID param);
	static UINT pwa2(LPVOID param);
	void PyramidWeight1(int number, int frame,int channel);
	static UINT PyramidWToOutT(LPVOID param);
	void PyramidWToOut();
	void PyramidWToOut1(int number,int *t);
	void pWascend(int* in,int level,int* out,int *wt);
	void pWout3(int *p1,int *p2,int *p3);
	afx_msg void OnRgbmaximumcontrastruleAddallusedframes();
	afx_msg void OnRgbweightedaverageruleAddallusedframes();
	afx_msg void OnGo();
	void InitializeComboBox();
	afx_msg void OnAcquireimageCamerasettings();
	int video_w;
	int video_h;
	int still_w;
	int still_h;
	bool video_showing;
	void grabtoframe(void);
	void takepicture(void);
	void videowindowon(void);
	void videowindowoff(void);
	afx_msg void OnFileEmptystack();
	afx_msg void On2framefunctionsCorrectbrightness();
	afx_msg void OnAcquireimageSavesettings();
	afx_msg void OnAcquireimageLoadsettings();
	void savedefcamsettings();
	void loaddefcamsettings();
	afx_msg void OnAcquireimageSavecameradefaults();
	afx_msg void OnAcquireimageGetmovie();
	void startvidcap(CString p);
	void endvidcap(void);
	void extractgrabbed();
	bool silentextract();
	static UINT extractgrabbedT(LPVOID param);
	CString work_path;
	afx_msg void OnFrameCreatescaleline();
	afx_msg void OnSizeandalignmentAuto();
	afx_msg void OnFrameSaveandedit();
	int index;
	afx_msg void OnEditCopysettoclipboard();
	afx_msg void OnAcquireimageCapturemovietofile();
	afx_msg void OnFileLoadspecial();
	afx_msg void OnFileSavespecial();
	int special_frame;
	bool paintonout;
	int brush_feather;
	afx_msg void OnFrameBeginretouching();
	void beginretouching();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBINEDOC_H__A28F7CE7_BEAD_4EC2_988F_0EF4B734750A__INCLUDED_)
