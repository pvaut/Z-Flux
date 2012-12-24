#pragma once
#include "qxwin.h"

class TQXgrid;


class TQXgriddata
{
public:
	virtual ~TQXgriddata();
	virtual int G_rowcount()=0;
	virtual int G_colcount()=0;
	virtual void G_colname(int colnr, QString &content)=0;
	virtual bool G_celltext(int rownr, int colnr, QString &content)=0;
	int G_colsize(int colnr) { return 80; }
	int G_rowsize(int rownr) { return 20; }
};

class TQXgridheader : public TQXview
{
private:
	TQXgrid *parent;
public:
	TQXgridheader(TQXgrid *iparent)
	{
		parent=iparent;
	}
	virtual void draw(CDC *dc, RECT &updaterect);
};


class TQXgridlist : public TQXview
{
private:
	TQXgrid *parent;
public:
	TQXgridlist(TQXgrid *iparent)
	{
		parent=iparent;
	}
	virtual void draw(CDC *dc, RECT &updaterect);
	virtual void SizeChanged();
	virtual void MouseLButtonDown(UINT nFlags, CPoint point);
};


class TQXgrid : public TQXview
{
	friend class TQXgridlist;
	friend class TQXgridheader;
private:
	int currownr,curcolnr;
public:
	TQXgriddata *data;
	TQXgridheader header;
	TQXgridlist list;
public:
	TQXgrid(StrPtr iname, TQXgriddata *idata);
	~TQXgrid();
	virtual void PostCreate();
	virtual void SizeChanged();
	void attachscrollbars(TQXheadercomp_scrollbar *vert, TQXheadercomp_scrollbar *horz);
	virtual void draw(CDC *dc, RECT &updaterect);
	void cursor_changerow(int newrownr);
	void redraw();
	void updatescrollbars();
};