#pragma once

#include "qxwin.h"
#include "qstring.h"
#include "qxcolor.h"
#include "qxdrawer.h"
#include "qxbitmap.h"
#include "qxpanelheader.h"


class TQXheadercomp_title : public TQXheadercomp
{
protected:
	TQXpanel *parentpanel;
public:
	TQXheadercomp_title(TQXpanel *iparentpanel);
	virtual int G_height();
	virtual StrPtr G_name() { return QString(""); }
	virtual void draw(CDC *dc, TQXDIBitmap *backdrawer);
};


class TQXheadercomp_button_subitem
{
public:
	int commandID;
	QString hint;
	TQXdrawer *drawer;
	TQXheadercomp_button_subitem(int i_commandID, StrPtr i_hint, TQXdrawer *i_drawer)
	{
		commandID=i_commandID;
		hint=i_hint;
		drawer=i_drawer;
	}
	~TQXheadercomp_button_subitem()
	{
		if (drawer!=NULL) delete drawer;
	}
};


class TQXheadercomp_button
{
	friend class TQXheadercomp_buttonbar;
private:
	int commandID;
	QString hint;
	TQXdrawer *drawer;
	Tarray<TQXheadercomp_button_subitem> subitems;
public:
	TQXheadercomp_button(int i_commandID, StrPtr i_hint, TQXdrawer *i_drawer)//takes over ownership of i_drawer
	{
		commandID=i_commandID;
		hint=i_hint;
		drawer=i_drawer;
	}
	~TQXheadercomp_button()
	{
		if (drawer!=NULL) delete drawer;
	}
	TQXdrawer* G_drawer()
	{
		return drawer;
	}
	TQXheadercomp_button_subitem* addsubitem(int commandID, int bitmapID, StrPtr hint);
};

class TQXheadercomp_buttonbar : public TQXheadercomp
{
private:
	QString name;
	CWnd *commandreceiver;
	TQXpanelheader *parent;
	Tarray<TQXheadercomp_button> buttons;
	TQXgraycorrmap corrmap;
	int curbutnr;
	bool butpressing;
private:
	void G_buttonoffset(int nr, int &px, int &py);
	int G_buttonatpos(int px, int py);
	void tracksubitems();
public:
	TQXheadercomp_buttonbar(StrPtr iname, CWnd *icommandreceiver);
	virtual void create(TQXpanelheader *iparent);
	virtual int G_height();
	virtual StrPtr G_name() { return name; }
	TQXheadercomp_button* addbutton(int commandID, int bitmapID, StrPtr hint);
	virtual void draw(CDC *dc, TQXDIBitmap *backdrawer);
	virtual void OnLButtonDown(UINT nFlags, int px, int py);
	virtual void OnLButtonUp(UINT nFlags, int px, int py);
	virtual void OnMouseMove(UINT nFlags, int px, int py);
};
