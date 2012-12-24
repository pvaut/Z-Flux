#pragma once
#include "tools.h"
#include "qstring.h"
#include "objecttree.h"

#include "SC_value.h"

#include "renderwindow.h"


class T3DCosmos;
class T3DScene;

class Taviplayer;
class Tvideotexture;


class T3DVideo : public TObjectTreeItem
{
private:
	T3DCosmos *cosmos;
	Taviplayer *aviplayer;
	Tvideotexture *videotexture;

	TSC_boolean *playing;
	TSC_scalar *framereduction;

	int counter;

public:
	T3DVideo();
	~T3DVideo();
	static StrPtr GetClassName() { return SC_valname_video; }
	virtual StrPtr G_classname() { return SC_valname_video; }
public:
	virtual bool G_param_optionlist(StrPtr paramname, Tarray<QString> &list);
	virtual void paramchanged(StrPtr iname);
	virtual TObjectTreeItem *G_parent();

	void setfile(StrPtr ifilename);
	void createtexture();

	Taviplayer* G_aviplayer_required();
	Tvideotexture* G_videotexture_required();

	void loadcurframe();
	void select(Trendercontext *rc);
	void unselect(Trendercontext *rc);

};
