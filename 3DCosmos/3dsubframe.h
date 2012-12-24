#pragma once
#include "3dgeoobjects.h"
#include "vec_transformation.h"

#include "sc_functor.h"

class Taffinetransformation;


class T3DTexturelist : public TObjectTreeItem
{
private:
	Tbaltree idx;
	Tarray<T3DTexture> textures;
public:
	T3DTexturelist()
	{
		param_readonly(_qstr("Name"),true);
	}
public:
	virtual StrPtr G_classname() { return _qstr("Textures"); }
	void G_name(QString &str) { str=G_classname(); }
	int G_childcount() { return textures.G_count(); }
	TObjectTreeItem* G_child(int nr) { return textures[nr]; }
	void delchild(TObjectTreeItem *ichild);
	void clear();
	void add(T3DTexture *tx);
	int G_count() { return textures.G_count(); }
	T3DTexture* G_texture(int nr) { return textures[nr]; }
	T3DTexture* G_texture(StrPtr name);
};


class T3DMotionlist : public TObjectTreeItem
{
private:
	Tbaltree index;
	Tarray<T3DMotion> motions;
public:
	T3DMotionlist();
	~T3DMotionlist();
public:
	virtual StrPtr G_classname() { return _qstr("Motions"); }
	void G_name(QString &str) { str=G_classname(); }
	int G_childcount();
	TObjectTreeItem* G_child(int nr);
	void delchild(TObjectTreeItem *ichild);
	int G_count();
	T3DMotion* G_motion(int nr);
	T3DMotion* G_motion(StrPtr name);
	void reset();
	void createdefaults();
	void add(T3DMotion *imotion);
};

class Tclipplane : public Tplane
{
public:
	QString viewportname;//if <>"", only apply to this viewport
};

class T3DSubFrameObject :  public T3DObject
{
private:
	TSC_boolean *enablelight,*cancache,*hasvolshadow,*autosortbydistance;
	TSC_color *color;
	TSC_string *motionname;
	Tarray<T3DObject> members;
	Taffinetransformation *transformation;
	Taffinetransformation solidvelocity;
	TSC_scalar *mass,*charge;
	TSC_value *subframetype,*blendtype,*depthmask,*depthtest;
	TSC_scalar *nearclipplane,*farclipplane;//only used if >0
	TSC_scalar *blinkperiod,*maxblinkcount;
	bool blinkstarting,blinkstopping,blinking,blinkvisible;
	int blinkcount;
	double blinkoffset,blinkperiodval;

	Tarray<QString> ignoreviewportlist;//frame will not be rendered to viewports in this list

private:
	Tintarray sortidx;
	Tdoublearray sortvalidx;

public:
	TSpecificObjectRef<TSC_functor> functor_prerenderaction;

private:
	TSC_color rendercolor;
	Taffinetransformation rendertransformation;
	TDisplayList *displaylist;
	T3DTexturelist texturelist;
	T3DMotionlist motionlist;
public:
	static StrPtr GetClassName() { return SC_valname_subframe; }
	T3DSubFrameObject(T3DScene *iscene=NULL);
	~T3DSubFrameObject();
	virtual StrPtr G_classname() { return SC_valname_subframe; }
	bool G_acceptsubobjects() { return true; }
	void addsubobject(T3DObject *iobject) { iobject->Set_parentframe(this); members.add(iobject); }
	void dropmember(StrPtr iname);
	void clearobjects() { members.reset(); }
	void moveobject(StrPtr objname, int step);
	void objectchangeparent(StrPtr objname, T3DSubFrameObject *subframe_dest);
	void makeviewdirframe(Tvertex *origin);
	void makescreenframe();
	void makeviewportframe();
	void Set_motionname(StrPtr iname);
	T3DMotion* G_mymotion();
	void Set_velocity(const Tvector *v);
	void G_velocity(Tvector *v) const;
	void G_totaltransformation(Taffinetransformation *transf);
	double G_mass() { return mass->G_val(); }
	double G_charge() { return charge->G_val(); }
public:
	int G_childcount();
	TObjectTreeItem* G_child(int nr);
	T3DObject* findobject(TObjectTreeItem *treeitem);
	void delchild(TObjectTreeItem *ichild);
public:
	virtual T3DObject* CreateInstance() { return new T3DSubFrameObject(NULL); }
	bool G_hasvolshadow() { return hasvolshadow->G_val(); }
	bool G_cancache();
	bool G_needrecache();
	bool G_iscached();
	virtual void execute_functor_prerenderaction();
	virtual void adjusttime(double timestep);
	virtual void precalcrender(Tprecalcrendercontext *pre);
	virtual void precalcrender2(T3DRenderViewport *rvp, Tprecalcrendercontext *pre);
	virtual void render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording);
	virtual void calcshadowvolumes(Tshadowvolumecontext *svc, T3DSubFrameObject *parentsubframe);
	const TSC_color* G_rendercolor() { return &rendercolor; }
	const Taffinetransformation* G_rendertransformation() { return &rendertransformation; }

	virtual void paramchanged(StrPtr iname);

	void loadobject_3ds(StrPtr filepath, StrPtr filename, double factor);

	virtual void streamout_content(QBinTagWriter &writer);
	virtual void streamin_content(QBinTagReader &reader);

public:
	T3DTexturelist* G_texturelist() { return & texturelist; }
	int G_texturecount() { return texturelist.G_count(); }
	T3DTexture* G_texture(int nr) { return texturelist.G_texture(nr); }
	T3DTexture* G_texture(StrPtr name);
	T3DTexture* addtexture(StrPtr name, StrPtr bitmapfile, StrPtr bitmapalphafile=NULL);
	T3DTexture* addbitmaptexture(StrPtr name, Tbmp *bmp, Tbmp *bmpalpha=NULL);
	T3DTexture* addvideotexture(StrPtr name, T3DVideo *video);
	void deletetextures();

public:
	T3DMotionlist* G_motionlist() { return & motionlist; }
	void addmotion(T3DMotion *motion);
	T3DMotion* findmotion(StrPtr name);

private:
	Tarray<Tclipplane> clipplanes;
public:
	void addclipplane(const Tplane &plane, StrPtr viewportname);
	void resetclipplanes();

public:
	void addignoreviewport(StrPtr viewportname);
	void delignoreviewport(StrPtr viewportname);

	void G_BoundingBox(Tvertex &vmin, Tvertex &vmax) const;

private:
	bool hasmousearrow;
	double mousearrow_size;
	Tvertex mousearrow_pos,mousearrow_bboxmin,mousearrow_bboxmax;
public:
	void MouseArrowEnable(const Tvertex &posit0, const Tvertex &bboxmin, const Tvertex &bboxmax, double size);
	virtual void MouseArrowChangePos(const Tvector &vc);
	virtual void MouseArrowClick(TMouseClickInfo info);

};

