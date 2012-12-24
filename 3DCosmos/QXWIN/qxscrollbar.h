#pragma once

#include "qxwin.h"
#include "qstring.h"
#include "qxcolor.h"
#include "qxdrawer.h"
#include "qxbitmap.h"
#include "qxpanelheader.h"


class TSXscrollaccociation
{
public:
	TQXview *view;
	DirDef dir;
};

class TQXheadercomp_scrollbar : public TQXheadercomp
{
private:
	QString name;
	TQXpanelheader *parent;
	TQXgraycorrmap corrmap;
	Tarray<TSXscrollaccociation> views;
private:
	int totsize,pagesize,jump,curpos;
	int curpart;
	bool pressing;
	int ps0dff;
private:
	void scroll2pos(int &p1, int &p2);
	int pos2scroll(int ps);
	int G_partatpos(int px, int py);//-1=nothing 0=scrollblock 1=arrow1 2=arrow2 3=empty1 4=empty2
public:
	int G_curpos() { return curpos; }
	void doscroll(int dff);
	void setscrollpos(int ps);
	void Set_totsize(int itotsize);
	void Set_pagesize(int ipagesize);
	int G_jump() { return jump; }
public:
	TQXheadercomp_scrollbar(StrPtr iname);
	void addview(TQXview *iview, DirDef idir);
	virtual void create(TQXpanelheader *iparent);
	virtual int G_height();
	virtual StrPtr G_name() { return name; }
	virtual void draw(CDC *dc, TQXDIBitmap *backdrawer);
	virtual void OnLButtonDown(UINT nFlags, int px, int py);
	virtual void OnLButtonUp(UINT nFlags, int px, int py);
	virtual void OnMouseMove(UINT nFlags, int px, int py);
	virtual void timerevent(int ct);
};
