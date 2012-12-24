#pragma once

#include "3DMesh.h"


class T3DGeoObject : public T3DObject
{
protected:
	TSC_color *color,*speccolor;
	TSC_scalar *specvalue;
	Tvertex *position;
	TSC_boolean *renderfront,*renderback,*cancache,*enablelight,*isbackground,*renderoutline;
	TSC_value *blendtype,*depthmask,*depthtest;
protected:
	TSC_color rendercolor;
	TDisplayList *displaylist;
protected:
	void render_drawobject(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording);
public:
	T3DGeoObject(T3DScene *iscene);
public:
	virtual bool G_param_optionlist(StrPtr paramname, Tarray<QString> &list);
	virtual void precalcrender(Tprecalcrendercontext *pre);
	virtual void render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording);
	Tvertex* G_position() { return position; }
	virtual bool G_cancache();
	StrPtr G_texturename() { return G_paramstring(_qstr("Texture")); }
	StrPtr G_texturenameright() { return G_paramstring(_qstr("TextureRight")); }

public:
	virtual void precalcrender_exec(Tprecalcrendercontext *pre) {};
	virtual void render_exec(Trendercontext *rc) {};

private:
	Tarray<Tshadowvolume> shadowvolumes;
public:
	virtual void calcshadowvolumes(Tshadowvolumecontext *svc, T3DSubFrameObject *parentsubframe);

	virtual bool calcshadowvolume_exec(Tshadowvolumecontext *svc, Tshadowvolume *shadowvolume, Tvertex &locallightpos, double lightradius, double depth1, double depth2) { return false; }
};


class Ttexor
{
public:
	double tx1,tx2;
};


class T3DGeoObjectPreCalcMesh : public T3DGeoObject
{
protected:
	bool iscalculated;
	T3Dmesh mesh;
	TSC_boolean *canbuffer,*calcedges;
protected:
	virtual void paramchanged(StrPtr iname);
	virtual void precalcrender_exec(Tprecalcrendercontext *pre);
	void render_exec(Trendercontext *rc);
	virtual bool calcshadowvolume_exec(Tshadowvolumecontext *svc, Tshadowvolume *shadowvolume, Tvertex &locallightpos, double lightradius, double depth1, double depth2);
public:
	T3DGeoObjectPreCalcMesh(T3DScene *iscene);
public:
	void Set_notcalculated() { iscalculated=false; }
	virtual bool paramchanged_needrecalculate(StrPtr paramname)=0;
	virtual void calculate()=0;
	bool G_canbuffer() { return canbuffer->G_val(); }
	T3Dmesh& G_mesh() { return mesh; };

	virtual void streamout_content(QBinTagWriter &writer);
	virtual void streamin_content(QBinTagReader &reader);

};


class T3DObjectSphere :  public T3DGeoObjectPreCalcMesh
{
private:
public:
	static StrPtr GetClassName() { return SC_valname_sphere; }
	virtual StrPtr G_classname() { return SC_valname_sphere; }
	T3DObjectSphere(T3DScene *iscene=NULL);
	virtual T3DObject* CreateInstance() { return new T3DObjectSphere(NULL); }
	virtual bool paramchanged_needrecalculate(StrPtr paramname);
	virtual void calculate();
	virtual bool calcshadowvolume_exec(Tshadowvolumecontext *svc, Tshadowvolume *shadowvolume, Tvertex &locallightpos, double lightradius, double depth1, double depth2);
};

class T3DObjectBar :  public T3DGeoObjectPreCalcMesh
{
private:
public:
	static StrPtr GetClassName() { return SC_valname_bar; }
	T3DObjectBar(T3DScene *iscene=NULL);
	virtual StrPtr G_classname() { return SC_valname_bar; }
	virtual T3DObject* CreateInstance() { return new T3DObjectBar(NULL); }
	virtual bool paramchanged_needrecalculate(StrPtr paramname);
	virtual void calculate();
};

class T3DObjectCylinder :  public T3DGeoObjectPreCalcMesh
{
private:
	TSC_scalar *radius,*radiuschange,*resolution;
	TSC_boolean *isclosed;
	Tvector *axis;
public:
	static StrPtr GetClassName() { return SC_valname_cylinder; }
	T3DObjectCylinder(T3DScene *iscene=NULL);
	virtual StrPtr G_classname() { return SC_valname_cylinder; }
	virtual T3DObject* CreateInstance() { return new T3DObjectCylinder(NULL); }
	virtual bool paramchanged_needrecalculate(StrPtr paramname);
	virtual void calculate();
};

class T3DObjectPie :  public T3DGeoObjectPreCalcMesh
{
private:
	TSC_scalar *radius1,*radius2,*resolution,*height;
	Tvector *axis1,*axis2;
	TSC_boolean *otherdir;
public:
	static StrPtr GetClassName() { return SC_valname_pie; }
	T3DObjectPie(T3DScene *iscene=NULL);
	virtual StrPtr G_classname() { return SC_valname_pie; }
	virtual T3DObject* CreateInstance() { return new T3DObjectPie(NULL); }
	virtual bool paramchanged_needrecalculate(StrPtr paramname);
	virtual void calculate();
};

class T3DObjectRectangle :  public T3DGeoObjectPreCalcMesh
{
private:
	Tvector *axis1,*axis2;
public:
	static StrPtr GetClassName() { return SC_valname_rectangle; }
	T3DObjectRectangle(T3DScene *iscene=NULL);
	virtual StrPtr G_classname() { return GetClassName(); }
	virtual T3DObject* CreateInstance() { return new T3DObjectRectangle(NULL); }
	virtual bool paramchanged_needrecalculate(StrPtr paramname);
	virtual void calculate();
};

class T3DObjectSolidArrow :  public T3DGeoObjectPreCalcMesh
{
private:
	Tvector *axis1,*axis2;
	TSC_scalar *width,*thick;
public:
	static StrPtr GetClassName() { return SC_valname_solidarrow; }
	T3DObjectSolidArrow(T3DScene *iscene=NULL);
	virtual StrPtr G_classname() { return GetClassName(); }
	virtual T3DObject* CreateInstance() { return new T3DObjectSolidArrow(NULL); }
	virtual bool paramchanged_needrecalculate(StrPtr paramname);
	virtual void calculate();
};



class T3DObjectPoint :  public T3DGeoObject
{
private:
	TSC_scalar *size;
public:
	static StrPtr GetClassName() { return SC_valname_solidpoint; }
	T3DObjectPoint(T3DScene *iscene=NULL);
	virtual StrPtr G_classname() { return SC_valname_solidpoint; }
	virtual T3DObject* CreateInstance() { return new T3DObjectPoint(NULL); }
public:
	virtual void precalcrender_exec(Tprecalcrendercontext *pre);
	void render_exec(Trendercontext *rc);
};


class T3DObject3DText :  public T3DGeoObject
{
private:
	TSC_string *content;
	TSC_scalar *depth;
	Tvector *unitdirx,*unitdiry;
public:
	static StrPtr GetClassName() { return SC_valname_text3d; }
	T3DObject3DText(T3DScene *iscene=NULL);
	virtual StrPtr G_classname() { return SC_valname_text3d; }
	virtual T3DObject* CreateInstance() { return new T3DObject3DText(NULL); }
	double G_textsizex();
public:
	virtual void precalcrender_exec(Tprecalcrendercontext *pre);
	void render_exec(Trendercontext *rc);
};

class T3DObjectFlatText :  public T3DGeoObject
{
private:
	TSC_string *content;
	TSC_scalar *size;
public:
	static StrPtr GetClassName() { return SC_valname_textflat; }
	T3DObjectFlatText(T3DScene *iscene=NULL);
	virtual StrPtr G_classname() { return SC_valname_textflat; }
	virtual T3DObject* CreateInstance() { return new T3DObjectFlatText(NULL); }
public:
	virtual void precalcrender_exec(Tprecalcrendercontext *pre);
	void render_exec(Trendercontext *rc);
	TSC_string* G_content() { return content; }
	TSC_scalar* G_size() { return size; }
};

