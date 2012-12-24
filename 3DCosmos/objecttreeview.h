#pragma once

#include "objecttree.h"

class Tmainwin;

class Tobjecttreeview : public TQXview
{
	static const int expandbuttonsize=4;
private:
	Tmainwin *mainwin;
	TObjectTreeItem *root,*cursor;
	static const int sepx=20;
	static const int sepy=18;
	int totsizey;
	Tcopyarray<TObjectTreeItem> history;
	int historypos;
private:
	TObjectTreeItem* findsub(TObjectTreeItem *item, int px, int py);
	void drawsub(CDC *dc, RECT &updaterect, int offsetx, int &offsety, TObjectTreeItem *item, bool selsub);
public:
	Tobjecttreeview(StrPtr iname, Tmainwin *imainwin, TObjectTreeItem *iroot);
	~Tobjecttreeview();
	virtual void PostCreate();
	virtual void SizeChanged();
	virtual void draw(CDC *dc, RECT &updaterect);
	virtual void MouseLButtonDown(UINT nFlags, CPoint point);
	virtual void MouseLButtonDblClk(UINT nFlags, CPoint point);
	void redraw();
	void updatescrollbars();
	TObjectTreeItem* G_root() { return root; }
	TObjectTreeItem* G_cursor() { return cursor; }
	void Set_cursor(TObjectTreeItem *it, bool addtohistory=true);
	void cmd_cursorback();
	void cmd_cursorforward();
public:
	void cursorchanged();
};