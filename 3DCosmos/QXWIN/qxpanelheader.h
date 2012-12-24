#pragma once

#include "qxwin.h"
#include "qstring.h"
#include "qxcolor.h"
#include "qxdrawer.h"
#include "qxbitmap.h"



class TQXheadercomp
{
public:
	int px0,py0,plx,ply;//to be filled in by parent structure
	DirDef dir;//to be filled in by parent structure
public:
	virtual ~TQXheadercomp();
	virtual void create(TQXpanelheader *iparent) {};
	virtual int G_height()=0;//dimension perpendicular to the header
	virtual StrPtr G_name()=0;
	virtual void draw(CDC *dc, TQXDIBitmap *backdrawer)=0;
	virtual void OnCommand(UINT id) {};
	virtual void OnLButtonDown(UINT nFlags, int px, int py) {};
	virtual void OnLButtonUp(UINT nFlags, int px, int py) {};
	virtual void OnMouseMove(UINT nFlags, int px, int py) {};
	virtual void timerevent(int ct) {};
};


class TQXpanelheader : public TQXbasewin
{
	DECLARE_MESSAGE_MAP();

private:
	TQXpanel *owner;
	PanelSideDef side;
	Tcopyarray<TQXheadercomp> comps;
private:
	TQXmembitmap membitmap;
	TQXDIBitmap drawer1,drawer2;

	TQXDIBitmap corner1;
	TQXgraycorrmap gcorner1;

	void calcposits();

public:
	TQXheadercomp *currentheadercomp;

public:
	int G_size();
	void addcomponent(TQXheadercomp *icomp);
	PanelSideDef G_side() { return side; }

public:
	TQXpanelheader(TQXpanel *iowner, PanelSideDef iside);
	void createwindow(CWnd *pwnd);
	LRESULT OnNcHitTest(CPoint point);
	void OnPaint();
	void oncmdrange(UINT id);
	void OnSize(UINT tpe, int x, int y);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnTimer(UINT ev);

private:
	TQXheadercomp *timer_win;
	int timer_ct;
public:
	void starttimer(TQXheadercomp *comp, int itv);
	void stoptimer();
};


