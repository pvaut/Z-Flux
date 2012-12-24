
#pragma once

#include "objecttree.h"

class Tmainwin;
class Tobjectpropertyview;
class Tpropertyedit;
class Tpropertycombo;

class Tpropertyviewer
{
private:
	bool readonly;
	Tobjectpropertyview *parent;
	TSC_value value;
	Tpropertyedit *edt;
	Tpropertycombo *combo;
	bool has_optionlist;
	Tarray<QString> optionlist;
public:
	int ypos;
public:
	Tpropertyviewer(Tobjectpropertyview *iparent, TSC_value *ivalue);
	~Tpropertyviewer();
	void Set_readonly();
	void set_optionlist(Tarray<QString> &ioptionlist);
	virtual int init(int ix0, int iy0);//returns: Y length
	TSC_value* GetValue();
};


class Tobjectpropertyview : public TQXview
{
private:
	Tmainwin *mainwin;
	Tparamset *paramset;
	Tarray<Tpropertyviewer> viewers;
public:
	const static int ysep=30;
public:
	Tobjectpropertyview(StrPtr iname, Tmainwin *imainwin);
	~Tobjectpropertyview();
	virtual void PostCreate();
	virtual void SizeChanged();
	virtual void draw(CDC *dc, RECT &updaterect);
	virtual void MouseLButtonDown(UINT nFlags, CPoint point);
	void redraw();
	void updatescrollbars();
public:
	void tranfercontent();
	void setcurrent(Tparamset *iparamset);
};