#pragma once
#include "tools.h"
#include "qstring.h"
#include "objecttree.h"

#include "SC_value.h"
#include "SC_boolean.h"
#include "SC_color.h"

#include "opengl.h"

#include "3Denums.h"
#include "3Dviewport.h"
#include "3Dtexture.h"
#include "3Dmotion.h"
#include "3Dcosmos.h"
#include "3DGeoObjects.h"
#include "3DSubframe.h"
#include "3DVolShadow.h"

class TSC_env;
class T3DCosmos;
class T3DScene;
class T3DSubFrameObject;
class T3DObjectSurface;
class Taffinetransformation;
class TRenderWindow;

#include "valnames.h"


class Tprecalcrendercontext
{
public:
	T3DSubFrameObject *parentsubframe;
	double elapsed;
public:
	Tprecalcrendercontext()
	{
		parentsubframe=NULL;
	}
};


class TObjectRenderStatus
{
public:
	int currentobjectblendtype;//of type SC_valname_blendtype
	int currentobjectdepthmask;//SC_valname_depthmask
	int currentobjectdepthtest;//SC_valname_depthtest
	bool enablelight;
public:
	TObjectRenderStatus()
	{
		currentobjectblendtype=BlendNormal;
		currentobjectdepthmask=DepthMaskNormal;
		currentobjectdepthtest=DepthTestNormal;
		enablelight=true;
	}
	TObjectRenderStatus(const TObjectRenderStatus &istatus)
	{
		copyfrom(istatus);
	}
	void operator=(const TObjectRenderStatus &istatus)
	{
		copyfrom(istatus);
	}
	void copyfrom(const TObjectRenderStatus &istatus)
	{
		currentobjectblendtype=istatus.currentobjectblendtype;
		currentobjectdepthmask=istatus.currentobjectdepthmask;
		currentobjectdepthtest=istatus.currentobjectdepthtest;
		enablelight=istatus.enablelight;
	}
};



class T3DRenderViewport
{
public:
	T3DViewport *viewport;
	TRenderWindow *leftwin,*rightwin;
public:
	T3DRenderViewport();
	bool G_isstereo();
};




class Tvolshadowinfo
{
public:
	TSC_color color;
	double depth1,depth2;
	double lightradius;
};

class T3DScene : public TObjectTreeItem
{
private:
	Tarray<T3DObject> objects;
	T3DCosmos *cosmos;
	Tarray<T3DRenderViewport> attachedviewports;
	TSC_chrono elapsed;
private:
	Tvertex *light0pos;
	TSC_color *ambientlightcolor,*backcolor,*light0color,*specularlightcolor;
	TSC_scalar *framenr;
public:
	static StrPtr GetClassName() { return SC_valname_scene; }
	T3DScene();
	~T3DScene();
	void addobject(T3DObject *iobject) { objects.add(iobject); }
public:
	virtual StrPtr G_classname() { return SC_valname_scene; }
	virtual TObjectTreeItem *G_parent();
	virtual void G_sourcecodename(QString &str);//name as it should be copied to the source code
	int G_childcount();
	TObjectTreeItem* G_child(int nr);
	T3DObject* findobject(TObjectTreeItem *treeitem);
	void delchild(TObjectTreeItem *ichild);
public:
	void execute_functor_prerenderaction();
	void adjusttime(double timestep);
	void precalcrender();//first precalc = common for all viewports
	void precalcrender2(T3DRenderViewport *rvp);//second precalc = per viewport; common for left & right
	void calcshadowvolumes();
	void createvolumeshadows(Trendercontext *rc);
	void drawdepthlayers(Trendercontext *rc);
	void rendercursor(Trendercontext *rc);
	void renderobjects(Trendercontext *rc);
	void start();
	void renderall();
	void attachviewport(T3DViewport *vp);
	void delattachedviewport(T3DViewport *vp);
	TSC_color* G_backcolor() { return backcolor; }
	TRenderWindow* GetMyFirstRenderWindow();

private:
	bool cursoractive;
	double cursorsize;
	Tvertex cursorpos;
public:
	void switch_cursoractive(Tvertex &icursorpos, double icursorsize);
	bool G_cursoractive() { return cursoractive; }
	void cursor_move(const Tvector &vc);
	void MouseArrowChangePos(const Tvector &vc);
	void MouseArrowClick(TMouseClickInfo info);
	Tvertex& G_cursorpos() { return cursorpos; }

public:
	Tshadowvolumecontext volshadows;
	Tarray<Tvolshadowinfo> volshadowinfo;
public:
	void volshadow_reset();
	void volshadow_add(double lightradius, TSC_color *color, double depth1, double depth2);
};



