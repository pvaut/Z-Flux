#pragma once

class TQXbasewin;
class TQXwin;
class TQXpanel;
class TQXpanelheader;
class TQXheadercomp;
class TQXview;
class TQXheadercomp_scrollbar;
class TQXheadercomp_tab;

class TQXbasewin : public CWnd
{
public:
	void SetPosition(RECT &rc)
	{
//		SetWindowPos(&wndTop,rc.left,rc.top,rc.right-rc.left+1,rc.bottom-rc.top+1,SWP_NOOWNERZORDER);
		MoveWindow(&rc,true);
	}

};

enum DirDef { DIR_HOR,DIR_VERT };
enum PanelSideDef { SIDE_TOP,SIDE_BOTTOM,SIDE_LEFT,SIDE_RIGHT };
PanelSideDef enumsides(int nr);

#include "qstring.h"
#include "qxcolor.h"
#include "qxdrawer.h"
#include "qxbitmap.h"
#include "qxwindef.h"
#include "qxpanelheader.h"
#include "qxtabbar.h"


bool IsKeyDown(int virtkey);

void cuttext(CDC *dc, QString &str, int len);

double Sshape(double x, double p, double w);


class TQXSys
{
private:
	CFont font_title,font_window,font_window_bold,font_window_italic,font_windowheader,font_fixed;
public:
	enum ColorDef { CL_FRAME1,CL_FRAME2,CL_BUTTONHIGHLIGHT,CL_WINDOWBACK1,CL_WINDOWBACK2,CL_WINDOWHIGHLIGHT,CL_TEXT };
	enum FontDef { FNT_TITLEBAR, FNT_WINDOW,FNT_WINDOWBOLD,FNT_WINDOWITALIC,FNT_WINDOWHEADER,FNT_FIXED };
public:
	const static int buttonsizex=40;
	const static int buttonsizey=40;
	const static int sepsize=4+2;
	const static int titlebarh=23;
	const static int statusbar=20;
	const static int scrollbarsize=22+2;
	bool colscheme_dark;
public:
	TQXgraycorrmap buttoncorrmap,buttoncolmap;
	TQXbitmapdrawer bmp_roundedcorner;
	TQXgraycorrmap scrollbutmap,scrollcolmap,scrollslidermap1,scrollslidermap2,scrollslidermap3,scrollslidermap4;
	TQXbitmapdrawer bmp_scrollup,bmp_scrolldown,bmp_scrolleft,bmp_scrollright;
public:
	TQXSys();
	void startup();
	TQXColor G_color(ColorDef cl);
	CFont& G_font(FontDef fnt);

	void writesettings();
	void readsettings();
};

TQXSys& G_QXSys();




class TQXpanel : public TQXbasewin , public TQXtabnotifyreceiver
{
private:
	DECLARE_MESSAGE_MAP();
	TQXwin *parentwindow;
	TQXpanel *parentpanel;
	TQXview *myview;
	Tarray<TQXpanel> childpanels;
	TQXwinpaneldef::SubTypeDef subtype;
	double sizefrac;
	Tintarray sepposits;
	TQXpanelheader *sidebars[4];//all headers: TOP,BOTTOM,LEFT,RIGHT
	QString name;
	int moving_separator;
	int moving_lastx,moving_lasty;
	int minsizex,minsizey;
	TQXheadercomp_tab *tabbar;//for tabbed childs
private:
	void sizefrac2sepposits(int ps1, int ps2);
	void sepposits2sizefrac(int ps1, int ps2);
	int G_onseparator(int px, int py);//returns the separator# underneath a position (-1 if none)
private://for custom tabs
	bool hascustomtabs;
public:
	void customtabs_init(TQXtabnotifyreceiver *receiver);
	void customtabs_add(StrPtr iname);
	TQXheadercomp_tab* G_tabbar() { return tabbar; }
public:
	TQXpanel(TQXwin *iparentwindow, TQXpanel *iparentpanel, TQXwinpaneldef &paneldef);
	~TQXpanel();
	StrPtr G_name() { return name; }
	TQXpanel* G_parentpanel() { return parentpanel; }
	void createwindow(CWnd *pwnd);
	void OnPaint();
	void OnSize(UINT tpe, int x, int y);
	int G_clientoffset(PanelSideDef side);
	void G_minsize(int &x, int &y);
	int G_actchildnr();//for tabbed childs
	LRESULT OnNcHitTest(CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	virtual void tabchanged(TQXheadercomp_tab *tabcontrol, int prenr, int postnr);
	void invalidateall();
	void updatewindowall();
	TQXpanel* G_panel_byname(StrPtr name);
};


class TQXwin : public CFrameWnd
{
	friend class TQXpanel;
private:
	DECLARE_MESSAGE_MAP();
	TQXpanel *rootpanel;
private:
	TQXmembitmap topmembitmap;
	TQXgraycorrmap topcorrmap;
	TQXDIBitmap topdrawer1,topdrawer2;
	Tarray<TQXheadercomp> headercomponents;
	Tarray<TQXview> views;
	TQXview* activeview;

private:
	void createcliparea(RECT &rc, CRgn &region);

public:
	TQXwin();
	~TQXwin();
	void createwindow(HWND pwnd, StrPtr iname);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnPaint();
	void OnSize(UINT tpe, int x, int y);
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
	LRESULT OnNcHitTest(CPoint point);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags );
	void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	void OnClose();
	void invalidateall();
	void writesettings();

public:
	virtual void createviews()=0;//to be implemented; creates all views & button bars
	virtual void buildwindef(TQXwindef &windef)=0;//to be implemented by concrete window
	void addview(TQXview *iview);
	void addheadercomponent(TQXheadercomp *icomp);
	TQXheadercomp* G_headercomponent(StrPtr name);
	TQXpanel* G_panel_byname(StrPtr name);
	TQXview* G_view(StrPtr name);
	TQXview* G_activeview() { return activeview; }
	void Set_activeview(TQXview *newview);
	virtual void OnCommand(UINT id) {}
	virtual void PostCreate() {}
	virtual void OnDestroy() {}
	virtual void KeyPressed(UINT nChar, UINT nRepCnt, UINT nFlags) {}
	virtual void CharPressed(UINT nChar, UINT nRepCnt, UINT nFlags) {}
	virtual bool DoClose() { return true; }
};


class TQXview : public TQXbasewin
{
	DECLARE_MESSAGE_MAP();
private:
	QString name;
	TQXmembitmap drawbuffer;
	TQXheadercomp_scrollbar *scrollbar_vert,*scrollbar_hor;
	TQXwin *framewindow;
private:
	void init();
public:
	bool drawbuffered,canmakeactive;
public:
	TQXview();
	TQXview(StrPtr iname);
	virtual ~TQXview();
	StrPtr G_name() { return name; }
	virtual void createwindow(CWnd *pwnd);
	void Set_framewindow(TQXwin *iframewindow)
	{
		framewindow=iframewindow;
	}
	TQXwin* G_framewindow() { return framewindow; }
	void OnPaint();
	void OnSize(UINT tpe, int x, int y);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags );
	int G_clientsizex();
	int G_clientsizey();
	int G_offsetx();
	int G_offsety();
	void makeactiveview();
	bool Isactiveview();
	void Set_scrollbar_vert(TQXheadercomp_scrollbar *scroll);
	void Set_scrollbar_hor(TQXheadercomp_scrollbar *scroll);
	TQXheadercomp_scrollbar* G_scrollbar_vert() { return scrollbar_vert; }
	TQXheadercomp_scrollbar* G_scrollbar_hor() { return scrollbar_hor; }
	void scrollinview_vert(int y1, int y2);
	void scrollinview_horz(int x1, int x2);
public:
	virtual void redraw(bool immediateupdate=false);
	virtual void PostCreate() {}
	virtual void SizeChanged() {}
	virtual void OnCommand(UINT id) {}
	virtual void KeyPressed(UINT nChar, UINT nRepCnt, UINT nFlags) {}
	virtual void CharPressed(UINT nChar, UINT nRepCnt, UINT nFlags) {}
	virtual void MouseLButtonDown(UINT nFlags, CPoint point) {}
	virtual void MouseLButtonUp(UINT nFlags, CPoint point) {}
	virtual void MouseLButtonDblClk(UINT nFlags, CPoint point) {}
	virtual void MouseMove(UINT nFlags, CPoint point);
	virtual bool MouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void MouseRButtonDown(UINT nFlags, CPoint point) {}
	virtual void draw(CDC *dc, RECT &updaterect) {}
};