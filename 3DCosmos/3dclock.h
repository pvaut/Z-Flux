
#pragma once

#include "3dscene.h"
#include "3dmesh.h"
#include "3dtexture.h"

class Tvertex;
class TSC_functor;


class T3DObjectClock : public T3DGeoObject
{
protected:
	TSC_scalar *size,*timeshift;
	Tvector *axis1,*axis2;
	TSC_value *clocktype;
	TSC_list *monthnames;
	TSC_boolean *hasowntime;
	TSC_time *disptime;
public:
	T3DObjectClock(T3DScene *iscene=NULL);
	static StrPtr GetClassName() { return SC_valname_clock; }
	virtual StrPtr G_classname() { return SC_valname_clock; }
	virtual T3DObject* CreateInstance() { return new T3DObjectClock(NULL); }
	virtual void paramchanged(StrPtr iname);
public:
public:
	virtual void init();
protected:
	void precalcrender_exec(Tprecalcrendercontext *pre);
	void render_exec(Trendercontext *rc);

	void render_exec_analogclock(Trendercontext *rc, TSC_time &disptime, double size);
	void render_exec_digitalclock(Trendercontext *rc, TSC_time &disptime, double size);
	void render_exec_date(Trendercontext *rc, TSC_time &disptime, double size);
	void render_exec_calendar(Trendercontext *rc, TSC_time &disptime, double size);
	void render_exec_datetime(Trendercontext *rc, TSC_time &disptime, double size);

	void render_exec_string(Trendercontext *rc, StrPtr content);
};
