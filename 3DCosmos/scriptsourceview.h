
#pragma once

#include "main.h"
#include "SC_script.h"

class Tscriptsourceview : public TQXview , public TQXtabnotifyreceiver
{
private:
	Tmainwin *mainwin;
	TSC_script *script;
	Tscriptvarview *varview;
	bool pressingdown;
public:
	const static int xsep=8;
	const static int ysep=20;
	const static int marginx=15;
public:
	Tscriptsourceview(StrPtr iname, Tmainwin *imainwin);
	~Tscriptsourceview();
	void Set_varview(Tscriptvarview *ivarview) { varview=ivarview; };
	void Set_script(TSC_script *iscript);
	TSC_script* G_script() { return script; }
	void G_cursorpos(int &px, int &py);
	virtual void PostCreate();
	virtual void SizeChanged();
	void update_postchange();
	void scrollinside_debugline();
	void scrollinside_sel();
	void insertstring(StrPtr content);
	virtual void draw(CDC *dc, RECT &updaterect);
	virtual void KeyPressed(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void CharPressed(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void MouseLButtonDown(UINT nFlags, CPoint point);
	virtual void MouseLButtonUp(UINT nFlags, CPoint point);
	virtual void MouseLButtonDblClk(UINT nFlags, CPoint point);
	virtual void MouseMove(UINT nFlags, CPoint point);
	void cmd_find(StrPtr str);
	void updatescrollbars();
public:
	void pixel2textpos(int px, int py, int &linenr, int &colnr);
	virtual void tabchanged(TQXheadercomp_tab *tabcontrol, int prenr, int postnr);
};



class Tscriptvarview : public TQXview
{
private:
	Tscriptsourceview *scriptwin;
public:
	Tscriptvarview(StrPtr iname, Tscriptsourceview *iscriptwin);
//	~Tscriptvarview();
//	virtual void PostCreate();
	virtual void SizeChanged();
	virtual void draw(CDC *dc, RECT &updaterect);
//	virtual void MouseLButtonDown(UINT nFlags, CPoint point);
	void updatescrollbars();
public:
};


class Toutputview : public TQXview
{
private:
	Tscriptsourceview *scriptwin;
public:
	Toutputview(StrPtr iname, Tscriptsourceview *iscriptwin);
	virtual void SizeChanged();
	virtual void draw(CDC *dc, RECT &updaterect);
	void updatescrollbars();
public:
};