#pragma once
#include "tools.h"
#include "qstring.h"
#include "objecttree.h"
#include "valnames.h"


class TDisplayDevice;
class TRenderWindow;

class TDisplayWindow : public CFrameWnd
{
private:
	TDisplayDevice *device;
	DECLARE_MESSAGE_MAP();
public:
	TRenderWindow *renderwindow;//used when rendertype=1
public:
	TDisplayWindow();
	~TDisplayWindow();
	void createwin(TDisplayDevice *idevice, RECT &boundingbox);
	void initrenderwindow();

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags );
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    virtual void OnNcDestroy();
	virtual void OnClose();
};

class TDisplayMonitor : public TObjectTreeItem
{
private:
public:
	TDisplayMonitor()
	{
		param_readonly(_qstr("Name"),true);
		addparam(_qstr("Description"),SC_valname_string,true);
		addparam(_qstr("ID"),SC_valname_string,true);
	}
public:
	virtual StrPtr G_classname() { return _qstr("Monitor"); }
};


class TDisplayDevice : public TObjectTreeItem
{
	friend class TDisplayWindow;
private:
	Tarray<TDisplayMonitor> monitors;
	TDisplayWindow *win;
	bool fullscreen;
public:
	static StrPtr GetClassName() { return SC_valname_display; }
	TDisplayDevice();
	~TDisplayDevice();
public:
	virtual StrPtr G_classname() { return SC_valname_display; }
	int G_childcount() { return monitors.G_count(); }
	TObjectTreeItem* G_child(int nr) { return monitors[nr]; }
	void load();
	void start();
	void G_displayrect(RECT &rc);
	TDisplayWindow* G_win() { return win; }
	virtual void G_description(QString &str);

};


class TDisplayAdapterlist : public TObjectTreeItem
{
private:
	Tarray<TDisplayDevice> devices;
public:
	TDisplayAdapterlist();
public:
	virtual StrPtr G_classname() { return _qstr("Displays"); }
	void G_name(QString &str) { str=G_classname(); }
	int G_childcount() { return devices.G_count(); }
	TObjectTreeItem* G_child(int nr) { return devices[nr]; }
	int G_devicecount() { return devices.G_count(); }
	TDisplayDevice* G_device(int nr);
	TDisplayDevice* G_device(StrPtr name);
public:
	void load();
public:
	static TDisplayAdapterlist& Get();
};

Tcopyarray<TDisplayWindow>* G_displaywindows();

//TDisplayDevice* G_displaydevice(StrPtr name);

TObjectTreeItem* G_treeobject_adapterlist();

TDisplayWindow* G_uniquedisplaywindow();
