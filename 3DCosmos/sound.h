#pragma once
#include "tools.h"
#include "qstring.h"
#include "objecttree.h"

#include "SC_value.h"

#include "renderwindow.h"


class T3DCosmos;
class T3DScene;


class TSound : public TObjectTreeItem
{
private:
	T3DCosmos *cosmos;
	QString filename;
	QString ID;
	bool opened;
	int currentvolume;
private:
	void exec(StrPtr cmd);
	void setvol(int ivol);

public:
	TSound();
	void SetID(StrPtr iID);
	~TSound();
	static StrPtr GetClassName() { return SC_valname_sound; }
	virtual StrPtr G_classname() { return SC_valname_sound; }
public:
	virtual bool G_param_optionlist(StrPtr paramname, Tarray<QString> &list);
	virtual void paramchanged(StrPtr iname);
	virtual TObjectTreeItem *G_parent();

	void setfile(StrPtr ifilename);
	void open();
	void start();
	void stop();
	void close();
	void pauze();
	void resume();
	void changevolume(int ivol);

private:
	bool fader_active;
	int fader_startvol,fader_endvol;
	double fader_starttime,fader_durat;
public:
	void fadevolume(int inewvol, double idurat);
	void updaterender();

};
