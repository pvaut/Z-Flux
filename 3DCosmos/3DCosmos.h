#pragma once
#include "tools.h"
#include "qstring.h"
#include "objecttree.h"

#include "SC_value.h"
#include "SC_boolean.h"
#include "SC_color.h"
#include "SC_time.h"

#include "3Dscene.h"
#include "3Dviewport.h"
#include "3Dtexture.h"
#include "3Dmotion.h"
#include "3Dvideo.h"
#include "3Dvolshadow.h"
#include "sound.h"

#include "joystick.h"

#define axiscount 6
#define axislevelcount 3

#define MAX_JOYSTICKCOUNT 2



class T3DScene;
class T3DSubFrameObject;
class T3DRenderViewport;
class Trendercontex;
class Tprecalcrendercontext;
class Tprecalcrendershadowvolumecontext;
class TObjectRenderStatus;
class T3DMotionFree;
class T3DControl;

class TMouseClickInfo
{
public:
	enum ClickType { LeftDown, LeftDouble, ScrollUp, ScrollDown };
	ClickType clicktype;
public:
	TMouseClickInfo()
	{
		clicktype=LeftDown;
	}
public:
	Tvertex posit;
};

class T3DObject : public TObjectTreeItem
{
protected:
	Tvertex *centerpos;
	TSC_boolean *isobjvisible;
	TSC_boolean *showlefteye,*showrighteye;
protected:
	T3DScene *scene;
	T3DCosmos *cosmos;
	T3DSubFrameObject *parentframe;
public:
	T3DObject(T3DScene *iscene);
	virtual ~T3DObject();
	virtual StrPtr G_classname()=0;
	virtual void G_sourcecodename(QString &str);//name as it should be copied to the source code
	T3DScene* G_scene() { return scene; }
	void Set_parentframe(T3DSubFrameObject *iparentframe) { parentframe=iparentframe; }
	void Set_scene(T3DScene *iscene) { scene=iscene; }
	virtual T3DObject* findobject(TObjectTreeItem *treeitem);
	bool G_isvisible();
	bool G_isvisible(Trendercontext *rc);
//	bool G_showlefteye() { return showlefteye->G_val(); }
//	bool G_showrighteye() { return showrighteye->G_val(); }
	const Tvertex* G_centerpos() { return centerpos; }

public:
	T3DObject* MakeInstance();
	virtual T3DObject* CreateInstance()=0;
	virtual bool G_acceptsubobjects() { return false; }
	virtual void addsubobject(T3DObject *iobject) {};
	virtual T3DSubFrameObject *G_parentframe() { return parentframe; }
	virtual TObjectTreeItem *G_parent();

	virtual void execute_functor_prerenderaction() {}

	virtual void MouseArrowChangePos(const Tvector &vc) {}

	virtual void MouseArrowClick(TMouseClickInfo info) {}


	virtual void adjusttime(double timestep) {}
	virtual bool G_cancache() { return true; }
	virtual bool G_needrecache() { return isdirty; }//returns true of the cache of this object is outdated e.g. because of a parameter change

	//called once when object is created:
	virtual void init() {}

	//NOTE: precalcrender(2) can alter the object's state (never called multithreaded)
	virtual void precalcrender(Tprecalcrendercontext *pre) {}// common for all viewports
	virtual void precalcrender2(T3DRenderViewport *rvp, Tprecalcrendercontext *pre) {}// per viewport, common for left-right

	//NOTE: render should not alter the object's state (use for multithreaded rendering)
	virtual void render(Trendercontext *rc, const TObjectRenderStatus *renderstatus, bool isrecording) {}

	virtual void calcshadowvolumes(Tshadowvolumecontext *svc, T3DSubFrameObject *parentsubframe) {}
};




class T3DViewportlist : public TObjectTreeItem
{
	friend class T3DCosmos;
private:
	Tarray<T3DViewport> viewports;
public:
	T3DViewportlist()
	{
		param_readonly(_qstr("Name"),true);
	}
public:
	virtual StrPtr G_classname() { return _qstr("Viewports"); }
	void G_name(QString &str) { str=G_classname(); }
	int G_childcount();
	TObjectTreeItem* G_child(int nr);
	void delchild(TObjectTreeItem *ichild);
	T3DViewport* addviewport();
	void reset();
};


class T3DVideolist : public TObjectTreeItem
{
	friend class T3DCosmos;
private:
	Tarray<T3DVideo> videos;
public:
	T3DVideolist()
	{
		param_readonly(_qstr("Name"),true);
	}
public:
	virtual StrPtr G_classname() { return _qstr("Videos"); }
	void G_name(QString &str) { str=G_classname(); }
	int G_childcount();
	TObjectTreeItem* G_child(int nr);
	void delchild(TObjectTreeItem *ichild);
	T3DVideo* addvideo();
	void reset();
	void loadcurframes();
};

class TSoundlist : public TObjectTreeItem
{
	friend class T3DCosmos;
private:
	int IDnr;
	Tarray<TSound> sounds;
public:
	TSoundlist()
	{
		IDnr=0;
		param_readonly(_qstr("Name"),true);
	}
public:
	virtual StrPtr G_classname() { return _qstr("Sounds"); }
	void G_name(QString &str) { str=G_classname(); }
	int G_childcount();
	TObjectTreeItem* G_child(int nr);
	void delchild(TObjectTreeItem *ichild);
	TSound* addsound();
	void reset();
	void updaterender();
};





class T3DCosmos : public TObjectTreeItem
{
private:
	Tarray<T3DObject> objectcatalog;
	Tarray<T3DScene> scenes;
	T3DViewportlist viewportlist;
	T3DVideolist videolist;
	TSoundlist soundlist;
	TSC_time *curtime;
	TSC_scalar *timespeedfactor;//time speed multiplication factor
	TSC_scalar *mousedampingfactor;//damping factor for mouse movements
	TSC_boolean *pauzed,*showcontrols;
	Tjoystickinfo joysticks[MAX_JOYSTICKCOUNT];
	double prev_mouseshift_x,prev_mouseshift_y,prev_mouseshift_z;//for smoothening of mouse response
	__int64 lastusernavigationcounter;
private:
	TSC_value *rendertype;//type: SC_valname_rendertype
	TSC_scalar *vsynccount,*requestedframerate;
	__int64 lastrendercounter,lasttimespeedcounter;
	double rendertimerfreq;
	DWORD timingtest_lasttick;
	int timingtest_count;
	double timingtest_framerate;
public:
	TMatrix *colormatrixleft,*colormatrixright;
	bool UseJoystickInput;

public://star information
	Tdoublearray stars_RA,stars_DEC,stars_E1,stars_E2,stars_MAG,stars_X,stars_Y,stars_Z,dir1_X,dir1_Y,dir1_Z,dir2_X,dir2_Y,dir2_Z;
	Tarray<QString> const_code;
	Tdoublearray const_RA_1,const_RA_2,const_DEC_1,const_DEC_2,const_E1_1,const_E1_2,const_E2_1,const_E2_2;
	Tdoublearray const_X_1,const_X_2,const_Y_1,const_Y_2,const_Z_1,const_Z_2;
public:
	void readstarinfo();

public:
	static StrPtr GetClassName() { return SC_valname_objectroot; }
	virtual StrPtr G_classname() { return SC_valname_objectroot; }
	virtual void G_sourcecodename(QString &str) {str=_qstr("root"); };//name as it should be copied to the source code
	T3DCosmos();
	~T3DCosmos();
	void init();
	void initvalues();
	void cleanup();
	void resetall();
	void resetallscenes();
	void resetallviewports();
	void resetallvideos();
	void resetallsounds();
	int G_objectcatalog_count() { return objectcatalog.G_count(); }
	T3DObject* G_objectcatalog_object(int nr) { return objectcatalog[nr]; }
	T3DObject* G_objectcatalog_object(StrPtr tpename);
	int G_rendertype() { return rendertype->toint(); }
	int G_vsynccount() { return vsynccount->G_intval(); }
	double G_requestedframerate() { return requestedframerate->G_val(); }
	double G_elapsedrendertime();
public:
	int G_childcount();
	TObjectTreeItem* G_child(int nr);
	virtual void delchild(TObjectTreeItem *ichild);
	T3DObject* findobject(TObjectTreeItem *treeitem);
	T3DScene* findscene(TObjectTreeItem *treeitem);

public:
	T3DScene* addscene();
	void delscene(int scenenr);
	int G_scenecount() { return scenes.G_count(); }
	T3DScene* G_scene(int nr) { return scenes[nr]; }
	T3DScene* G_scene(StrPtr name);

	virtual void paramchanged(StrPtr iname);

public:
	T3DViewportlist* G_viewportlist() { return &viewportlist; }
	int G_viewportcount() { return viewportlist.viewports.G_count(); }
	T3DViewport* G_viewport(int nr) { return viewportlist.viewports[nr]; }
	T3DViewport* G_selviewport()
	{
		if (G_viewportcount()<=0) return NULL;
		return viewportlist.viewports[0];
	}
public:
	T3DVideolist* G_videolist() { return &videolist; }
	TSoundlist* G_soundlist() { return &soundlist; }

private://functions for UI axis: 0=X 1=Y 2=Z
	double axisposit[axiscount][axislevelcount];//idx1=axis idx2=level
	double axispositraw[axiscount][axislevelcount];//undamped, and from mouse only
public:
	void dispatchjoysticks();
	void updateaxisposit();
	bool G_axisactive(int axisnr, int axislevel);
	double G_axisposit(int axisnr, int axislevel);
	bool G_axisrawactive(int axisnr, int axislevel);
	double G_axisrawposit(int axisnr, int axislevel);
	void UIaction_axis(int axisnr, int axislevel, double val);//this function is called if a user performs an action
	Tjoystickinfo& G_joystickinfo(int ID);

public:
	static T3DCosmos& Get()
	{
		static T3DCosmos sset;
		return sset;
	}

	void UI_OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void UI_OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	void UI_processkey(UINT nChar, bool controlpressed, bool shiftpressed);
	void UI_OnMouseArrowClick(TMouseClickInfo info);

public:
	int disptype;//0=both Left+Right image 1=Left only 2=Right only
	bool showdepthlayers,showstereogrid,showinfoline;
private:
	bool requeststop;
public:
	void Reset_requeststop();
	void Set_requeststop();
	bool G_requeststop();

	void releaseallcontexts();//called at the end of a script, to release all contexts from that thread

	void modify_timespeed(double ifactor);
	void pauze(bool ipauzed);
	bool G_pauzed();
	void adjusttimes(double timestep);
	double incrtime();
	void waitforframerate();
	void render(bool dryrun=false);
	double G_framerate() { return timingtest_framerate; }
	const TSC_time* G_time() { return curtime; }
	double G_rendertimerfreq() { return rendertimerfreq; }

	void cmd_switchstereo();
	void cmd_copycamerapos();
	void cmd_copycursorpos();
	void cmd_exportscreenimage();

private:
	Tcopyarray<T3DMotion> motioncatalog;
	T3DMotionFree *defaultmotion;
public:
	void motioncatalog_add(T3DMotion *mt);
	void motioncatalog_remove(T3DMotion *mt);
	T3DMotion* G_defaultmotion() { return defaultmotion; }

private:
	Tcopyarray<T3DControl> controls;
	int activecontrol;
public:
	bool G_showcontrols() { return showcontrols->G_val(); }
	void controls_add(T3DControl *icontrol);
	void controls_del(T3DControl *icontrol);
	void controls_selectnext(int dir);
	void controls_activate(T3DControl *ctrl);
	T3DControl* G_activecontrol();
	T3DControl* G_defaultcontrol();


private:
	Tbaltree keyboardcodes_index;
	Tarray<CString> keyboardcodes_names;
	Tintarray keyboardcodes_vals;
	QString lastkeypressed;
public:
	void addkeyboardcode(StrPtr iname, int ival);
	void initkeyboardcodes();
	int G_keyboardcode(StrPtr iname);
	StrPtr G_keyboardname(int icode);
	StrPtr G_lastkeypressed() { return lastkeypressed; }

private:
	Tarray<QString> settings_id,settings_content;
	Tbaltree settings_index;
public:
	void LoadSettings();
	bool ReadSetting(StrPtr ID, QString &content);
	void WriteSetting(StrPtr ID, StrPtr content);


};

T3DCosmos& G_3DCosmos();