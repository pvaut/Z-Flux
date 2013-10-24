#include "stdafx.h"
#include "3DScene.h"
#include "displaydevices.h"
#include "video.h"

#include "qxstoredbitmap.h"

int G_rendertype();

bool IsKeyDown(int virtkey);

//*********************************************************************
// T3DViewport
//*********************************************************************

T3DViewport::T3DViewport()
{
	addlog(_text("Constructing viewport"),+1);
	videocapture=NULL;
	winxres=1;winyres=1;
	started=false;
	cosmos=&T3DCosmos::Get();
	addparam(_qstr("Scene"),SC_valname_string,true);

	active=addparam(_qstr("Active"),SC_valname_boolean)->content.G_content_boolean();
	active->copyfrom(true);

	camerapos=G_valuecontent<Tvertex>(&addparam(_qstr("CameraPos"),SC_valname_vertex)->content);
	cameradir=G_valuecontent<Tvector>(&addparam(_qstr("CameraDir"),SC_valname_vector)->content);
	cameraupdir=G_valuecontent<Tvector>(&addparam(_qstr("CameraUpDir"),SC_valname_vector)->content);
	enableusernavigation=addparam(_qstr("EnableUserNavigation"),SC_valname_boolean)->content.G_content_boolean();

	enableusertimecontrol=addparam(_qstr("EnableUserTimeControl"),SC_valname_boolean)->content.G_content_boolean();
	enableuserstop=addparam(_qstr("EnableUserStop"),SC_valname_boolean)->content.G_content_boolean();

	usernavigation_axislevel_move=addparam(_qstr("UserNavigationAxisLevelMove"),SC_valname_scalar)->content.G_content_scalar();
	usernavigation_axislevel_rotate=addparam(_qstr("UserNavigationAxisLevelRotate"),SC_valname_scalar)->content.G_content_scalar();
	usernavigation_axislevel_time=addparam(_qstr("UserNavigationAxisLevelTime"),SC_valname_scalar)->content.G_content_scalar();

	fadecolor=G_valuecontent<TSC_color>(&addparam(_qstr("FadeColor"),SC_valname_color)->content);
	fadecolor->copyfrom(0,0,0,0);

	usestereo=addparam(_qstr("UseStereo"),SC_valname_boolean)->content.G_content_boolean();
	usestereo->copyfrom(true);

	erasebackground=addparam(_qstr("EraseBackground"),SC_valname_boolean)->content.G_content_boolean();
	erasebackground->copyfrom(false);

	showcontrols=addparam(_qstr("ShowControls"),SC_valname_boolean)->content.G_content_boolean();

	framesize=addparam(_qstr("FrameSize"),SC_valname_scalar)->content.G_content_scalar();
	framesize->copyfrom(0.005);
	framecolor=G_valuecontent<TSC_color>(&addparam(_qstr("FrameColor"),SC_valname_color)->content);
	framecolor->copyfrom(0.4,0.4,0.4,1);
	frameleft=addparam(_qstr("FrameLeft"),SC_valname_boolean)->content.G_content_boolean();frameleft->copyfrom(true);
	frametop=addparam(_qstr("FrameTop"),SC_valname_boolean)->content.G_content_boolean();frametop->copyfrom(true);
	frameright=addparam(_qstr("FrameRight"),SC_valname_boolean)->content.G_content_boolean();frameright->copyfrom(true);
	framebottom=addparam(_qstr("FrameBottom"),SC_valname_boolean)->content.G_content_boolean();framebottom->copyfrom(true);


	transformation=G_valuecontent<Taffinetransformation>(&addparam(_qstr("Transf"),SC_valname_affinetransformation)->content);


//	addparam(_qstr("CameraPosFrame"),SC_valname_string);
//	addparam(_qstr("CameraLookAtFrame"),SC_valname_string);

	focaldistance=addparam(_qstr("FocalDistance"),SC_valname_scalar)->content.G_content_scalar();
	eyeseparation=addparam(_qstr("EyeSeparation"),SC_valname_scalar)->content.G_content_scalar();
	eyesepfactor=addparam(_qstr("EyeSepFactor"),SC_valname_scalar)->content.G_content_scalar();

	light0colorfactor=addparam(_qstr("Light0ColorFactor"),SC_valname_scalar)->content.G_content_scalar();
	

	aperture=addparam(_qstr("Aperture"),SC_valname_scalar)->content.G_content_scalar();

	nearclipplane=addparam(_qstr("NearClipPlane"),SC_valname_scalar)->content.G_content_scalar();
	farclipplane=addparam(_qstr("FarClipPlane"),SC_valname_scalar)->content.G_content_scalar();

	hstretchfactorX=addparam(_qstr("StretchFactor"),SC_valname_scalar)->content.G_content_scalar();
	hstretchfactorY=addparam(_qstr("StretchFactorY"),SC_valname_scalar)->content.G_content_scalar();

	swapstereo=addparam(_qstr("SwapStereo"),SC_valname_boolean)->content.G_content_boolean();
	hmirrorleft=addparam(_qstr("HMirrorLeft"),SC_valname_boolean)->content.G_content_boolean();
	hmirrorright=addparam(_qstr("HMirrorRight"),SC_valname_boolean)->content.G_content_boolean();
	vmirrorleft=addparam(_qstr("VMirrorLeft"),SC_valname_boolean)->content.G_content_boolean();
	vmirrorright=addparam(_qstr("VMirrorRight"),SC_valname_boolean)->content.G_content_boolean();

	xoffsetfrac=addparam(_qstr("XOffsetFrac"),SC_valname_scalar)->content.G_content_scalar();

	zoomx1=addparam(_qstr("ZoomX1"),SC_valname_scalar)->content.G_content_scalar();zoomx1->copyfrom(0.0);
	zoomy1=addparam(_qstr("ZoomY1"),SC_valname_scalar)->content.G_content_scalar();zoomy1->copyfrom(0.0);
	zoomx2=addparam(_qstr("ZoomX2"),SC_valname_scalar)->content.G_content_scalar();zoomx2->copyfrom(1.0);
	zoomy2=addparam(_qstr("ZoomY2"),SC_valname_scalar)->content.G_content_scalar();zoomy2->copyfrom(1.0);

	addparam(_qstr("LeftDisplay"),SC_valname_string);
	addparam(_qstr("RightDisplay"),SC_valname_string);

	xminfrac=addparam(_qstr("XMinFrac"),SC_valname_scalar)->content.G_content_scalar();
	yminfrac=addparam(_qstr("YMinFrac"),SC_valname_scalar)->content.G_content_scalar();
	xmaxfrac=addparam(_qstr("XMaxFrac"),SC_valname_scalar)->content.G_content_scalar();
	ymaxfrac=addparam(_qstr("YMaxFrac"),SC_valname_scalar)->content.G_content_scalar();

	rightviewshiftX=addparam(_qstr("RightViewShiftX"),SC_valname_scalar)->content.G_content_scalar();
	rightviewshiftY=addparam(_qstr("RightViewShiftY"),SC_valname_scalar)->content.G_content_scalar();

	xmaxfrac->copyfrom(1.0);
	ymaxfrac->copyfrom(1.0);
	rightviewshiftX->copyfrom(0.5);
	rightviewshiftY->copyfrom(0.0);
	
	


	//generate unique, default name
	QString nm,nm0;
	int nr=0;
	while (true)
	{
		nr++;
		nm="Viewport_";nm+=QString(nr);
		bool found=false;
		for (int i=0; i<cosmos->G_viewportcount(); i++)
		{
			cosmos->G_viewport(i)->G_name(nm0);
			if (qstricmp(nm,nm0)==0) found=true;
		}
		if (!found) break;
	}
	param_setvalue(ObjNameStr,nm);
	addlog(TFormatString(_text("Name: ^1"),nm));

	leftwin=NULL;
	rightwin=NULL;

	resetparams();

	addlog(_text("Completed constructing viewport"),-1);

	RenderToTexture=false;
	RTT_imasizex=500;
	RTT_imasizey=500;


}

void T3DViewport::resetparams()
{
	camerapos->copyfrom(Tvector(-1,0,0));
	cameradir->copyfrom(Tvertex(1,0,0));
	cameraupdir->copyfrom(Tvertex(0,1,0));
	enableusernavigation->copyfrom(false);
	usernavigation_axislevel_move->copyfrom(0.0);
	usernavigation_axislevel_rotate->copyfrom(1.0);
	usernavigation_axislevel_time->copyfrom(2.0);
	enableusertimecontrol->copyfrom(false);
	enableuserstop->copyfrom(false);

	focaldistance->copyfrom(7.0);
	eyeseparation->copyfrom(0.25);
	eyesepfactor->copyfrom(0.025);
	aperture->copyfrom(3.1415927/4);
	nearclipplane->copyfrom(0.1);
	farclipplane->copyfrom(100);
	hstretchfactorX->copyfrom(1.0);
	hstretchfactorY->copyfrom(1.0);
	light0colorfactor->copyfrom(1.0);

	swapstereo->copyfrom(false);
	hmirrorleft->copyfrom(false);
	hmirrorright->copyfrom(false);
	vmirrorleft->copyfrom(false);
	vmirrorright->copyfrom(false);

	resetcurrentmove();
}


void T3DViewport::resetcurrentmove()
{
	currentmove.scenerot=Tvector(0,0,0);
	currentmove.cammove=Tvector(0,0,0);
}


void T3DViewport::applycurrentmove()
{
	if (G_enableusernavigation())
	{
		double distunit=G_focaldistance();

		Tvector dirx,diry,dirz;

		dirx.vecprod(cameradir,cameraupdir);dirx.normfrom(&dirx);
		diry.vecprod(&dirx,cameradir);diry.normfrom(&diry);
		dirz.normfrom(cameradir);

		camerapos->lincombfrom(1,camerapos,distunit*currentmove.cammove.G_x(),&dirx);
		camerapos->lincombfrom(1,camerapos,distunit*currentmove.cammove.G_y(),&diry);
		camerapos->lincombfrom(1,camerapos,distunit*currentmove.cammove.G_z(),&dirz);

		Tvector rotdir;

		rotdir.lincombfrom(1,&rotdir,currentmove.scenerot.G_x(),&dirx);
		rotdir.lincombfrom(1,&rotdir,currentmove.scenerot.G_y(),&diry);
		rotdir.lincombfrom(1,&rotdir,currentmove.scenerot.G_z(),&dirz);

		double angle=currentmove.scenerot.G_size();
		cameradir->rotatefrom(cameradir,&rotdir,angle);
		cameraupdir->rotatefrom(cameraupdir,&rotdir,angle);

		Tvector cpos;cpos.subtrvertices(camerapos,&Tvertex(0,0,0));
		cpos.rotatefrom(&cpos,&rotdir,angle);
		camerapos->copyfrom(cpos);
	}
}


T3DViewport::~T3DViewport()
{
	if (videocapture!=NULL) delete videocapture;
	delfromscene();
	if (leftwin!=NULL) delete leftwin;
	if (rightwin!=NULL) delete rightwin;
}


void T3DViewport::G_sourcecodename(QString &str)
{ 
	QString str2;
	str=_qstr("root.Viewports.");
	G_name(str2);str+=str2;
}


bool T3DViewport::G_param_optionlist(StrPtr paramname, Tarray<QString> &list)
{
	return false;
}

void T3DViewport::paramchanged(StrPtr iname)
{
	if (issame(iname,_qstr("FocalDistance")))
	{
		eyeseparation->copyfrom(focaldistance->G_val()*eyesepfactor->G_val());
	}
}

TObjectTreeItem* T3DViewport::G_parent()
{
	return cosmos->G_viewportlist();
}

void T3DViewport::start()
{
	RECT bbox,drect,rc;
	bool shareddevice;

	addlog(_text("Start viewport"),+1);

	if (started) throw QError(_text("Viewport is already started"));

	StrPtr leftdisplayname=G_param(_qstr("LeftDisplay"))->content.G_content_string()->G_string();
	StrPtr rightdisplayname=G_param(_qstr("RightDisplay"))->content.G_content_string()->G_string();
	isstereo=(qstrlen(rightdisplayname)>0);

	addlog(TFormatString(_text("Left display: ^1"),leftdisplayname));
	addlog(TFormatString(_text("Right display: ^1"),rightdisplayname));


	if (!usestereo->G_val()) isstereo=false;
	addlog(TFormatString(_text("Use stereo: ^1"),isstereo));

	if (G_rendertype()==RenderSingle)
	{//do not create render windows here: a single render window is owned by the display window
		addlog(_text("Using RenderSingle"));
		StrPtr leftdisplayname=G_param(_qstr("LeftDisplay"))->content.G_content_string()->G_string();
		TDisplayDevice *leftdisplay=TDisplayAdapterlist::Get().G_device(leftdisplayname);
		if (leftdisplay==NULL) throw QError(_text("Could not find left display device"));
		if (leftdisplay->G_win()==NULL) throw QError(_text("Could not find left display device window"));
		leftdisplay->G_win()->initrenderwindow();

		leftdisplay->G_win()->GetClientRect(&rc);
		winxres=rc.right-rc.left;
		winyres=rc.bottom-rc.top;
		if (isstereo) winxres/=2;

		started=true;
		addlog(_text("Viewport started"),-1);
		return;
	}

	double xminfr=xminfrac->G_val();
	double xmaxfr=xmaxfrac->G_val();
	double yminfr=yminfrac->G_val();
	double ymaxfr=ymaxfrac->G_val();
	double xminfr2,xmaxfr2,yminfr2,ymaxfr2;

	if (leftwin=NULL) delete leftwin;leftwin=NULL;
	if (rightwin!=NULL) delete rightwin;rightwin=NULL;

	shareddevice=(qstricmp(leftdisplayname,rightdisplayname)==0);

	TDisplayDevice *leftdisplay=TDisplayAdapterlist::Get().G_device(leftdisplayname);

	leftdisplay->G_win()->GetClientRect(&rc);
	winxres=rc.right-rc.left;
	winyres=rc.bottom-rc.top;


	if (leftdisplay==NULL) throw QError(_text("Could not find left display device"));
	if (leftdisplay->G_win()==NULL) throw QError(_text("Left display device is not yet started"));
	leftdisplay->G_displayrect(drect);
	xminfr2=xminfr;xmaxfr2=xmaxfr;yminfr2=yminfr;ymaxfr2=ymaxfr;
	if (shareddevice)
	{
		xminfr2=xminfr*0.5;xmaxfr2=xmaxfr*0.5;
	}
	bbox.left=  (int)(0.5+drect.left+xminfr2*(drect.right-drect.left));
	bbox.top=   (int)(0.5+drect.top+yminfr2*(drect.bottom-drect.top));
	bbox.right= (int)(0.5+drect.left+xmaxfr2*(drect.right-drect.left));
	bbox.bottom=(int)(0.5+drect.top+ymaxfr2*(drect.bottom-drect.top));
	if (leftwin!=NULL) throw QError(_text("Left window already created"));
	leftwin=new TRenderWindow(false,leftdisplay);//--!!--warning: this can never be called from a thread!!! (cannot create windows in a thread)
	leftwin->createwin(bbox);
	leftwin->setup();

	if (isstereo)
	{
		xminfr2=xminfr;xmaxfr2=xmaxfr;yminfr2=yminfr;ymaxfr2=ymaxfr;
		if (shareddevice)
		{
			xminfr2=0.5+xminfr*0.5;xmaxfr2=0.5+xmaxfr*0.5;
		}
		TDisplayDevice *rightdisplay=TDisplayAdapterlist::Get().G_device(rightdisplayname);
		if (rightdisplay==NULL) throw QError(_text("Could not find left display device"));
		if (rightdisplay->G_win()==NULL) throw QError(_text("Right display device is not yet started"));
		bbox.left=  (int)(0.5+drect.left+xminfr2*(drect.right-drect.left));
		bbox.top=   (int)(0.5+drect.top+yminfr2*(drect.bottom-drect.top));
		bbox.right= (int)(0.5+drect.left+xmaxfr2*(drect.right-drect.left));
		bbox.bottom=(int)(0.5+drect.top+ymaxfr2*(drect.bottom-drect.top));
		if (rightwin!=NULL) throw QError(_text("Right window already created"));
		rightwin=new TRenderWindow(true,rightdisplay);//--!!--warning: this can never be called from a thread!!! (cannot create windows in a thread)
		rightwin->createwin(bbox);
		rightwin->setup();
	}

	if (isstereo) wglShareLists(leftwin->rc,rightwin->rc);
	started=true;

	addlog(_text("Viewport started"),-1);

}


TRenderWindow* T3DViewport::G_leftwin()
{ 
//	if (leftwin==NULL) throw QError(_text("Left window is not defined"));
	return leftwin;
}
TRenderWindow* T3DViewport::G_rightwin()
{ 
//	if (rightwin==NULL) throw QError(_text("Right window is not defined"));
	return rightwin; 
}


StrPtr T3DViewport::G_attachedscenename()
{
	return  G_paramstring(_qstr("Scene"));
}

void T3DViewport::delfromscene()
{
	StrPtr scenename=G_attachedscenename();
	T3DScene *scene=G_3DCosmos().G_scene(scenename);
	if (scene!=NULL) scene->delattachedviewport(this);
	G_param(_qstr("Scene"))->content.fromstring(_qstr(""));
}


void T3DViewport::setscene(T3DScene *iscene)
{
	delfromscene();
	iscene->attachviewport(this);
	QString str;
	iscene->G_name(str);
	G_param(_qstr("Scene"))->content.fromstring(str);
}


bool T3DViewport::G_isHmirrorred(bool isrightpart)
{
	if (!isrightpart) return hmirrorleft->G_val();
	else return hmirrorright->G_val();
}

bool T3DViewport::G_isVmirrorred(bool isrightpart)
{
	if (!isrightpart) return vmirrorleft->G_val();
	else return vmirrorright->G_val();
}


double T3DViewport::G_eyeseparation()
{ 
	if (!G_isstereo()) return 0.0;
	return eyeseparation->G_val();
}


double T3DViewport::G_aspectratio()
{
	double screenratio=(winxres*(xmaxfrac->G_val()-xminfrac->G_val())*1.0/(winyres*(ymaxfrac->G_val()-yminfrac->G_val())))/abs(G_hstretchfactorX())*abs(G_hstretchfactorY());
	return screenratio;
}


void T3DViewport::camera_rotatehor(double angle)
{
	Tvector hordir,rotdir;
	hordir.vecprod(cameradir,cameraupdir);
	rotdir.vecprod(&hordir,cameradir);
	if (rotdir.G_size()<0.00001) return;
	rotdir.normfrom(&rotdir);
	cameradir->rotatefrom(cameradir,&rotdir,angle);
	cameraupdir->rotatefrom(cameraupdir,&rotdir,angle);
}

void T3DViewport::camera_rotatevert(double angle)
{
	Tvector rotdir;
	rotdir.vecprod(cameradir,cameraupdir);
	if (rotdir.G_size()<0.00001) return;
	rotdir.normfrom(&rotdir);
	cameradir->rotatefrom(cameradir,&rotdir,angle);
	cameraupdir->rotatefrom(cameraupdir,&rotdir,angle);
}

void T3DViewport::camera_move(double fc)
{
	camerapos->lincombfrom(1,camerapos,fc*G_cameramovestep(),cameradir);
}


void T3DViewport::scene_rotatehor(double angle)
{
	Tvector rotdir=*cameraupdir;
	rotdir.normfrom(&rotdir);
	cameradir->rotatefrom(cameradir,&rotdir,angle);
	cameraupdir->rotatefrom(cameraupdir,&rotdir,angle);
	Tvertex rotcenter(0,0,0);
	Tvector camerapsd;
	camerapsd.subtrvertices(camerapos,&rotcenter);
	camerapsd.rotatefrom(&camerapsd,&rotdir,angle);
	camerapos->add(&rotcenter,&camerapsd);
}

void T3DViewport::scene_rotatevert(double angle)
{
	double ang2=Tvector::angle(cameradir,cameraupdir);
	if ((angle>0)&&(ang2-angle<0.01)) return;
	if ((angle<0)&&(ang2-angle>Pi-0.01)) return;

	Tvector rotdir;
	rotdir.vecprod(cameradir,cameraupdir);
	if (rotdir.G_size()<0.00001) return;
	rotdir.normfrom(&rotdir);
	cameradir->rotatefrom(cameradir,&rotdir,angle);
//	cameraupdir->rotatefrom(cameraupdir,&rotdir,angle);
	Tvertex rotcenter(0,0,0);
	Tvector camerapsd;
	camerapsd.subtrvertices(camerapos,&rotcenter);
	camerapsd.rotatefrom(&camerapsd,&rotdir,angle);
	camerapos->add(&rotcenter,&camerapsd);
}

void T3DViewport::scene_move(double fc)
{
	Tvertex rotcenter(0,0,0);
	Tvector camerapsd;
	camerapsd.subtrvertices(camerapos,&rotcenter);
	camerapsd.mulfrom(&camerapsd,1-0.015*fc);
	camerapos->add(&rotcenter,&camerapsd);
}


void T3DViewport::switchcursoractive()
{
	T3DScene *scene=G_3DCosmos().G_scene(G_attachedscenename());
	if (scene!=NULL)
	{
		Tvertex cursorpos;
		cursorpos.lincombfrom(1.0,camerapos,G_focaldistance(),cameradir);
		double cursorsize=G_focaldistance()/10.0;
		scene->switch_cursoractive(cursorpos,cursorsize);
	}
}

bool T3DViewport::G_cursoractive()
{
	T3DScene *scene=G_3DCosmos().G_scene(G_attachedscenename());
	if (scene!=NULL) return scene->G_cursoractive();
	else return false;
}

bool ArrowKeyPressing()
{
	if (IsKeyDown(VK_LEFT)) return true;
	if (IsKeyDown(VK_RIGHT)) return true;
	if (IsKeyDown(VK_UP)) return true;
	if (IsKeyDown(VK_DOWN)) return true;
	return false;
}

void T3DViewport::dispatchusernavigation()
{
	T3DScene *scene=G_3DCosmos().G_scene(G_attachedscenename());

	double stepfactor=0.75;

	if (scene!=NULL)
	{
		Tvector horzdir,vertdir,cdir,horzdir2,vertdir2;
		horzdir.vecprod(cameradir,cameraupdir);
		horzdir.normfrom(&horzdir);
		vertdir.vecprod(&horzdir,cameradir);
		vertdir.normfrom(&vertdir);
		cdir.normfrom(cameradir);
		horzdir2.copyfrom(1,0,0);
		vertdir2.copyfrom(0,1,0);
		if (G_3DCosmos().G_axisactive(UIA_X,2))
		{
			if (scene->G_cursoractive())
			{
				horzdir.mulfrom(&horzdir,G_3DCosmos().G_axisposit(UIA_X,2)*G_focaldistance()/100.0/stepfactor);
				scene->cursor_move(horzdir);
			}
		}
		if (G_3DCosmos().G_axisrawactive(UIA_X,2))
		{
			if (!ArrowKeyPressing())
			{
				horzdir2.mulfrom(&horzdir2,G_3DCosmos().G_axisrawposit(UIA_X,2)/100.0*stepfactor);
				scene->MouseArrowChangePos(horzdir2);
			}
		}
		if (G_3DCosmos().G_axisactive(UIA_Y,2))
		{
			if (scene->G_cursoractive())
			{
				vertdir.mulfrom(&vertdir,G_3DCosmos().G_axisposit(UIA_Y,2)*G_focaldistance()/100.0*stepfactor);
				scene->cursor_move(vertdir);
			}
		}
		if (G_3DCosmos().G_axisrawactive(UIA_Y,2))
		{
			if (!ArrowKeyPressing())
			{
				vertdir2.mulfrom(&vertdir2,G_3DCosmos().G_axisrawposit(UIA_Y,2)/100.0*stepfactor);
				scene->MouseArrowChangePos(vertdir2);
			}
		}
		if (G_3DCosmos().G_axisactive(UIA_Z,2))
		{
			cdir.mulfrom(&cdir,G_3DCosmos().G_axisposit(UIA_Z,2)*G_focaldistance()/100.0*stepfactor);
			if (scene->G_cursoractive())
				scene->cursor_move(cdir);
		}
	}

	if (G_enableusernavigation())
	{
		double rotunit=0.02*stepfactor;

		int axislevel_move=usernavigation_axislevel_move->G_intval();
		int axislevel_rotate=usernavigation_axislevel_rotate->G_intval();


		if (G_3DCosmos().G_axisactive(UIA_X,axislevel_move)) camera_rotatehor(-G_3DCosmos().G_axisposit(UIA_X,axislevel_move)*rotunit);
		if (G_3DCosmos().G_axisactive(UIA_Y,axislevel_move)) camera_rotatevert(G_3DCosmos().G_axisposit(UIA_Y,axislevel_move)*rotunit);
		if (G_3DCosmos().G_axisactive(UIA_Z,axislevel_move)) camera_move(G_3DCosmos().G_axisposit(UIA_Z,axislevel_move));

		if (G_3DCosmos().G_axisactive(UIA_X,axislevel_rotate)) scene_rotatehor(G_3DCosmos().G_axisposit(UIA_X,axislevel_rotate)*rotunit);
		if (G_3DCosmos().G_axisactive(UIA_Y,axislevel_rotate)) scene_rotatevert(-G_3DCosmos().G_axisposit(UIA_Y,axislevel_rotate)*rotunit);
		if (G_3DCosmos().G_axisactive(UIA_Z,axislevel_rotate)) scene_move(G_3DCosmos().G_axisposit(UIA_Z,axislevel_rotate));
	}

	if ((G_enableusertimecontrol())&&(!G_cursoractive()))
	{
		int axislevel_time=usernavigation_axislevel_time->G_intval();
		if (G_3DCosmos().G_axisactive(UIA_Z,axislevel_time)) G_3DCosmos().modify_timespeed(1+0.06*stepfactor*G_3DCosmos().G_axisposit(UIA_Z,axislevel_time));
	}

}

void T3DViewport::switchstereo()
{
	usestereo->copyfrom(!usestereo->G_val());
	isstereo=usestereo->G_val();
}

void T3DViewport::videocapture_start(StrPtr ifilename)
{
	if (videocapture!=NULL) throw QError(_text("Video capture is already busy"));

	StrPtr leftdisplayname=G_param(_qstr("LeftDisplay"))->content.G_content_string()->G_string();
	TDisplayDevice *displaydevice=TDisplayAdapterlist::Get().G_device(leftdisplayname);
	if (displaydevice==NULL) throw QError(_text("No display device present"));
	TDisplayWindow *dispwin=displaydevice->G_win();
	if (dispwin==NULL) throw QError(_text("No display window present"));
	if (dispwin->renderwindow==NULL) throw QError(_text("No render window associated to display window"));
	videocapture=new Tvideocapture;
	videocapture->start(dispwin->renderwindow,ifilename);
}

void T3DViewport::videocapture_stop()
{
	if (videocapture==NULL) throw QError(_text("No video capture is busy"));
	videocapture->stop();
	delete videocapture;
	videocapture=NULL;
}

void T3DViewport::videocapture_addframe()
{
	if (videocapture!=NULL)
		videocapture->addframe();
}


void T3DViewport::capture(Tbmp *bmp)
{
	//obtain display info
	RECT rc;
	StrPtr leftdisplayname=G_param(_qstr("LeftDisplay"))->content.G_content_string()->G_string();
	TDisplayDevice *displaydevice=TDisplayAdapterlist::Get().G_device(leftdisplayname);
	if (displaydevice==NULL) throw QError(_text("No display device present"));
	TDisplayWindow *dispwin=displaydevice->G_win();
	if (dispwin==NULL) throw QError(_text("No display window present"));
	if (dispwin->renderwindow==NULL) throw QError(_text("No render window associated to display window"));
	dispwin->renderwindow->GetClientRect(&rc);


	dispwin->renderwindow->ClientToScreen(&rc);

	int x0=rc.left;
	int y0=rc.left;
	int lx=rc.right-rc.left;
	int ly=rc.bottom-rc.top;

	HDRAWDIB hdd;
	HDC hdc;
	HBITMAP hBitmap;
	unsigned char *data;

	hdd = DrawDibOpen();	
	hdc=CreateCompatibleDC(0);

	BITMAPINFO	bmpinfo;
	ZeroMemory(&bmpinfo,sizeof(BITMAPINFO));
	bmpinfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);					// Size Of The BitmapInfoHeader
	bmpinfo.bmiHeader.biPlanes = 1;											// Bitplanes	
	bmpinfo.bmiHeader.biBitCount = 24;										// Bits Format We Want (24 Bit, 3 Bytes)
	bmpinfo.bmiHeader.biWidth = lx;										
	bmpinfo.bmiHeader.biHeight = ly;										
	bmpinfo.bmiHeader.biSizeImage=lx*ly*3;
	bmpinfo.bmiHeader.biCompression = BI_RGB;								// Requested Mode = RGB

	hBitmap = CreateDIBSection (hdc,&bmpinfo, DIB_RGB_COLORS, (void**)(&data), NULL, NULL);
	SelectObject(hdc,hBitmap);								// Select hBitmap Into Our Device Context (hdc)


	//HDC windc=GetDC(dispwin->renderwindow->m_hWnd);//doesn't seem to work...
	HDC windc=GetDC(0);

	SelectObject(hdc,hBitmap);
	BitBlt(hdc,0,0,lx,ly,windc,x0,y0,SRCCOPY);

	int xs=((3*lx+3)/4)*4;

	bmp->init(lx,ly,3,0,255);
	int idx1,idx2;
	for (int ix=0; ix<lx; ix++)
		for (int iy=0; iy<ly; iy++)
		{
			idx2=ix+iy*lx;
			idx1=3*ix+(ly-1-iy)*xs;
			bmp->G_val8rptr()[idx2]=data[idx1+2];
			bmp->G_val8gptr()[idx2]=data[idx1+1];
			bmp->G_val8bptr()[idx2]=data[idx1+0];
		}

	DrawDibClose(hdd);

}


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_viewport_create,addviewport)
{
	setclasspath_fromtype(SC_valname_viewport);
	setreturntype(SC_valname_viewport);
	addvar(_qstr("XMinFrac"),SC_valname_scalar);
	addvar(_qstr("YMinFrac"),SC_valname_scalar);
	addvar(_qstr("XMaxFrac"),SC_valname_scalar);
	addvar(_qstr("YMaxFrac"),SC_valname_scalar);
	addvar(_qstr("LeftDisplay"),SC_valname_string);
	addvar(_qstr("RightDisplay"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DViewport *vp=G_3DCosmos().G_viewportlist()->addviewport();

	vp->G_param(_qstr("XMinFrac"))->content.copyfrom(arglist->get(0)->G_content_scalar()->G_val());
	vp->G_param(_qstr("YMinFrac"))->content.copyfrom(arglist->get(1)->G_content_scalar()->G_val());
	vp->G_param(_qstr("XMaxFrac"))->content.copyfrom(arglist->get(2)->G_content_scalar()->G_val());
	vp->G_param(_qstr("YMaxFrac"))->content.copyfrom(arglist->get(3)->G_content_scalar()->G_val());

	vp->G_param(_qstr("LeftDisplay"))->content.fromstring(arglist->get(4)->G_content_string()->G_string());
	if (arglist->G_ispresent(5)) vp->G_param(_qstr("RightDisplay"))->content.fromstring(arglist->get(5)->G_content_string()->G_string());

	retval->encapsulate(vp);
}
ENDFUNCTION(func_viewport_create)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_viewport_start,start)
{
	setmemberfunction(SC_valname_viewport);
//	addvar(_qstr("type"),SC_valname_string);
//	addvar(_qstr("name"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DViewport *vp=G_valuecontent<T3DViewport>(owner);
	vp->start();
}
ENDFUNCTION(func_viewport_start)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_viewport_aspectratio,aspectratio)
{
	setmemberfunction(SC_valname_viewport);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DViewport *vp=G_valuecontent<T3DViewport>(owner);
	retval->G_content_scalar()->copyfrom(vp->G_aspectratio());
}
ENDFUNCTION(func_viewport_aspectratio)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_viewport_setscene,setscene)
{
	setmemberfunction(SC_valname_viewport);
	addvar(_qstr("scene"),SC_valname_scene);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DViewport *vp=G_valuecontent<T3DViewport>(owner);
	vp->setscene(G_valuecontent<T3DScene>(arglist->get(0)));
}
ENDFUNCTION(func_viewport_setscene)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_viewport_enablewarped,EnableWarped)
{
	setmemberfunction(SC_valname_viewport);
	addvar(_qstr("imaresx"),SC_valname_scalar);
	addvar(_qstr("imaresy"),SC_valname_scalar);
	addvar(_qstr("warpx"),SC_valname_matrix);
	addvar(_qstr("warpy"),SC_valname_matrix);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DViewport *vp=G_valuecontent<T3DViewport>(owner);
	vp->RenderToTexture=true;
	vp->RTT_imasizex=arglist->get(0)->G_content_scalar()->G_intval();
	vp->RTT_imasizey=arglist->get(1)->G_content_scalar()->G_intval();
	TMatrix *warpx=G_valuecontent<TMatrix>(arglist->get(2));
	TMatrix *warpy=G_valuecontent<TMatrix>(arglist->get(3));
	vp->RTT_warp.init(*warpx,*warpy);
	//int warpresx=arglist->get(2)->G_content_scalar()->G_intval();
	//int warpresy=arglist->get(3)->G_content_scalar()->G_intval();
	//vp->RenderToTexture=true;
	//vp->RTT_imasizex,RTT_imasizey
}
ENDFUNCTION(func_viewport_enablewarped)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_viewport_disablewarped,DisableWarped)
{
	setmemberfunction(SC_valname_viewport);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DViewport *vp=G_valuecontent<T3DViewport>(owner);
	vp->RenderToTexture=false;
}
ENDFUNCTION(func_viewport_disablewarped)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_viewport_startrecord,StartRecording)
{
	setmemberfunction(SC_valname_viewport);
	addvar(_qstr("filename"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DViewport *vp=G_valuecontent<T3DViewport>(owner);
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();
	vp->videocapture_start(filename);
}
ENDFUNCTION(func_viewport_startrecord)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_viewport_stoprecord,StopRecording)
{
	setmemberfunction(SC_valname_viewport);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DViewport *vp=G_valuecontent<T3DViewport>(owner);
	vp->videocapture_stop();
}
ENDFUNCTION(func_viewport_stoprecord)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_viewport_capture,CaptureFrame)
{
	setmemberfunction(SC_valname_viewport);
	setreturntype(SC_valname_bitmap);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DViewport *vp=G_valuecontent<T3DViewport>(owner);

	Tbmp *bmp=G_valuecontent<Tbmp>(retval);

	vp->capture(bmp);


}
ENDFUNCTION(func_viewport_capture)
