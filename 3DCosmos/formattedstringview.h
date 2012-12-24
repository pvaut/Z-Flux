
#pragma once

#include "objecttree.h"

#include "FormatText.h"

class Tmainwin;


class TformattedstringviewEnv : public TxtEnv
{
private:
public:
	TformattedstringviewEnv();
	virtual void processcustomkeyword(StrPtr keyword, StrPtr in, QString &out);
};

class Tformattedstringview : public TQXview
{
private:
	Tmainwin *mainwin;
	TformattedstringviewEnv doc;
	TxtHyperlinklist hyperlinklist;
	int offsety;
public:
	const static int ysep=30;
public:
	Tformattedstringview(StrPtr iname, Tmainwin *imainwin);
	~Tformattedstringview();
	void Set_formattedstring(StrPtr ifstr);
	virtual void PostCreate();
	virtual void SizeChanged();
	virtual void draw(CDC *dc, RECT &updaterect);
	virtual void MouseLButtonDown(UINT nFlags, CPoint point);
	virtual void MouseMove(UINT nFlags, CPoint point);
	void redraw();
	void updatescrollbars();
};