#pragma once

#include "qxwin.h"
#include "qstring.h"
#include "qxcolor.h"
#include "qxdrawer.h"
#include "qxbitmap.h"
#include "qxpanelheader.h"

class TQXheadercomp_tab;

class TQXtabnotifyreceiver
{
public:
	virtual void tabchanged(TQXheadercomp_tab *tabcontrol, int prenr, int postnr)=0;
};

class TQXheadercomp_tab : public TQXheadercomp
{
protected:
	TQXpanelheader *parent;
	bool mouseinside;
	Tarray<QString> tabs;
	TQXpanel *parentpanel;
	int curtab;
	TQXtabnotifyreceiver *receiver;
public:
	TQXheadercomp_tab(TQXpanel *iparentpanel);
	void Set_receiver(TQXtabnotifyreceiver *ireceiver) { receiver=ireceiver; }
	void create(TQXpanelheader *iparent);
	void reset();
	void addtab(StrPtr name);
	void deltab(int tabnr);
	void Set_tabname(int tabnr, StrPtr iname);
	virtual int G_height();
	virtual StrPtr G_name() { return QString(""); }
	virtual void draw(CDC *dc, TQXDIBitmap *backdrawer);
	int G_curtab() { return curtab; }
	void change_tab(int newtabnr);
	StrPtr G_curtabname();

	virtual void OnCommand(UINT id);
	virtual void OnLButtonDown(UINT nFlags, int px, int py);
	virtual void OnLButtonUp(UINT nFlags, int px, int py);
	virtual void OnMouseMove(UINT nFlags, int px, int py);

};
