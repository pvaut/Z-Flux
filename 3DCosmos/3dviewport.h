#pragma once
#include "tools.h"
#include "qstring.h"
#include "objecttree.h"

#include "SC_value.h"

#include "renderwindow.h"

class T3DCosmos;
class TDisplayDevice;
class T3DScene;
class Tvideocapture;



class T3DViewport : public TObjectTreeItem
{
private:
	T3DCosmos *cosmos;
	TRenderWindow *leftwin,*rightwin;
	bool started,isstereo;
	Tvertex *camerapos;
	Tvector *cameradir,*cameraupdir;
	TSC_boolean *active,*enableusernavigation,*enableusertimecontrol,*enableuserstop,*usestereo,*erasebackground,*showcontrols;
	TSC_scalar *usernavigation_axislevel_move,*usernavigation_axislevel_rotate,*usernavigation_axislevel_time;
	TSC_scalar *xminfrac,*yminfrac,*xmaxfrac,*ymaxfrac;
	TSC_scalar *rightviewshiftX,*rightviewshiftY;
	TSC_color *fadecolor;
	Taffinetransformation *transformation;
	int winxres,winyres;//only stored for informative reason (e.g. in function G_aspectratio)

public://for warped image rendering
	bool RenderToTexture;
	int RTT_imasizex,RTT_imasizey;
	Tframebufferobject RTT_fbo_left,RTT_fbo_right;
	TWarpMesh RTT_warp;

public:
	struct{
		Tvector scenerot,cammove;
	} currentmove;

public:
	T3DViewport();
	~T3DViewport();
	static StrPtr GetClassName() { return SC_valname_viewport; }
	virtual StrPtr G_classname() { return SC_valname_viewport; }
	virtual void G_sourcecodename(QString &str);//name as it should be copied to the source code
public:
	virtual bool G_param_optionlist(StrPtr paramname, Tarray<QString> &list);
	virtual void paramchanged(StrPtr iname);
	virtual TObjectTreeItem *G_parent();
private:
	TSC_scalar *hstretchfactorX,*hstretchfactorY,*focaldistance,*eyeseparation,*eyesepfactor,*aperture,*nearclipplane,*farclipplane,*xoffsetfrac,*light0colorfactor;//scalars
	TSC_boolean *swapstereo,*hmirrorleft,*hmirrorright,*vmirrorleft,*vmirrorright;
public:
	TSC_scalar *framesize;
	TSC_color *framecolor;
	TSC_boolean *frameleft,*frametop,*frameright,*framebottom;
public:
	TSC_scalar *zoomx1,*zoomy1,*zoomx2,*zoomy2;
public:
	bool G_active() { return active->G_val(); }
	bool G_started() { return started; }
	bool G_isstereo() { return isstereo; }
	bool G_erasebackground() { return erasebackground->G_val(); }
	bool G_showcontrols() { return showcontrols->G_val(); }
	TRenderWindow* G_leftwin();
	TRenderWindow* G_rightwin();
	StrPtr G_attachedscenename();
	void resetparams();
	void resetcurrentmove();
	void applycurrentmove();
	void delfromscene();
	void start();
	void setscene(T3DScene *iscene);
public:
	bool G_isswappedstereo() { return swapstereo->G_val(); }
	bool G_isHmirrorred(bool isrightpart);
	bool G_isVmirrorred(bool isrightpart);
	double G_hstretchfactorX() { return hstretchfactorX->G_val(); }
	double G_hstretchfactorY() { return hstretchfactorY->G_val(); }
	double G_eyeseparation();
	double G_focaldistance() { return focaldistance->G_val(); }
	double G_aperture() { return aperture->G_val(); }
	double G_nearclipplane() { return nearclipplane->G_val(); }
	double G_farclipplane() { return farclipplane->G_val(); }
	double G_cameramovestep() { return G_focaldistance()/50.0; }
	double G_xoffsetfrac() { return xoffsetfrac->G_val(); }

	double G_light0colorfactor() { return light0colorfactor->G_val(); }

	double G_framesize() { return framesize->G_val(); }

	double G_xminfrac() { return xminfrac->G_val(); }
	double G_yminfrac() { return yminfrac->G_val(); }
	double G_xmaxfrac() { return xmaxfrac->G_val(); }
	double G_ymaxfrac() { return ymaxfrac->G_val(); }

	double G_rightviewshiftX() { return rightviewshiftX->G_val(); }
	double G_rightviewshiftY() { return rightviewshiftY->G_val(); }

	TSC_color* G_fadecolor() { return fadecolor; }

	double G_aspectratio();


	Tvertex* G_camerapos() { return camerapos; }
	Tvector* G_cameradir() { return cameradir; }
	Tvector* G_cameraupdir() { return cameraupdir; }
	Taffinetransformation* G_transformation() { return transformation; }
	bool G_enableusernavigation() { return enableusernavigation->G_val(); }
	bool G_enableusertimecontrol() { return enableusertimecontrol->G_val(); }
	bool G_enableuserstop() { return enableuserstop->G_val(); }

	void camera_rotatehor(double angle);
	void camera_rotatevert(double angle);
	void camera_move(double fc);
	void scene_rotatehor(double angle);
	void scene_rotatevert(double angle);
	void scene_move(double fc);
	void dispatchusernavigation();

	void switchcursoractive();
	void switchstereo();
	bool G_cursoractive();

private:
	Tvideocapture *videocapture;
public:
	void videocapture_start(StrPtr ifilename);
	void videocapture_stop();
	void videocapture_addframe();
	void capture(Tbmp *bmp);


};
