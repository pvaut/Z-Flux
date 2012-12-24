#pragma once

#include "3dscene.h"
#include "3dmesh.h"
#include "3dtexture.h"

class Tvertex;
class TSC_functor;


class T3DObjectStarGlobe : public T3DGeoObject
{
public:
	TSC_scalar *radius,*starsize,*conlinesize;
	TSC_color *conlinecolor;
private:
	bool calculated;
	Titemarray<Tvec3d> starvertexbuffer;
	Titemarray<T4color> starcolorbufferL,starcolorbufferR;
	Titemarray<T2textureidx> startexturebuffer;
	Titemarray<Tvec3d> conlinesvertexbuffer;
	Titemarray<T2textureidx> conlinestexturebuffer;
public:
	T3DObjectStarGlobe(T3DScene *iscene=NULL);
	static StrPtr GetClassName() { return SC_valname_starglobe; }
	virtual StrPtr G_classname() { return SC_valname_starglobe; }
	virtual T3DObject* CreateInstance() { return new T3DObjectStarGlobe(NULL); }
	virtual void paramchanged(StrPtr iname);
public:
	void load();
	void calculate();
public:
	virtual void init();
protected:
	void precalcrender_exec(Tprecalcrendercontext *pre);
	void render_exec(Trendercontext *rc);
};
