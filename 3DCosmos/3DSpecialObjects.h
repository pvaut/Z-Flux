#pragma once

#include "3Dcosmos.h"


class T3DObjectLightPoint : public T3DObject
{
protected:
	TSC_color *color;
	Tvertex *position;
	TSC_scalar *size;
protected:
public:
	T3DObjectLightPoint(T3DScene *iscene=NULL);
	static StrPtr GetClassName() { return SC_valname_lightpoint; }
	virtual StrPtr G_classname() { return SC_valname_lightpoint; }
	virtual T3DObject* CreateInstance() { return new T3DObjectLightPoint(NULL); }
public:
	virtual void precalcrender(Tprecalcrendercontext *pre) {};
	virtual void render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording);
	double G_size() { return size->G_val(); }
	Tvertex* G_position() { return position; }
	bool G_cancache() { return false; };
};



class T3DRepeatedObject : public T3DObject
{
protected:
	TSpecificObjectRef<T3DSubFrameObject> subframeref;
	Tvertex *position;
protected:
public:
	T3DRepeatedObject(T3DScene *iscene=NULL);
	static StrPtr GetClassName() { return SC_valname_repeatedobject; }
	virtual StrPtr G_classname() { return SC_valname_lightpoint; }
	virtual T3DObject* CreateInstance() { return new T3DRepeatedObject(NULL); }
	void Setref(T3DSubFrameObject *object);
public:
	virtual void precalcrender(Tprecalcrendercontext *pre) {};
	virtual void render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording);
	Tvertex* G_position() { return position; }
	bool G_cancache() { return false; };
};


class T3DObjectFog : public T3DObject
{
protected:
	TSC_value *fogtype;
	TSC_scalar *density,*start,*end;
	TSC_color *color;
protected:
public:
	T3DObjectFog(T3DScene *iscene=NULL);
	static StrPtr GetClassName() { return SC_valname_fog; }
	virtual StrPtr G_classname() { return SC_valname_fog; }
	virtual T3DObject* CreateInstance() { return new T3DObjectFog(NULL); }
public:
	virtual void precalcrender(Tprecalcrendercontext *pre) {};
	virtual void render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording);
	bool G_cancache() { return false; };
};
