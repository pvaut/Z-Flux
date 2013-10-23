#include "stdafx.h"

#include "Qthread.h"

#include "vecmath.h"
#include "vec_transformation.h"

#include "opengl.h"

#include "3DScene.h"
#include "3Dsurface.h"

#include "renderwindow.h"
#include "displaydevices.h"

#include "FBO.h"



int G_rendertype();


TRenderWindow* G_globalrenderwindow()
{
	TDisplayWindow *dwin=G_uniquedisplaywindow();
	if (dwin==NULL) throw QError(_text("Unable to find global render window"));
	return dwin->renderwindow;
}


//*********************************************************************
// T3DRenderViewport
//*********************************************************************

T3DRenderViewport::T3DRenderViewport()
{
	viewport=NULL;
	leftwin=NULL;
	rightwin=NULL;
}

bool T3DRenderViewport::G_isstereo()
{
	if (viewport==NULL)
	{
		ASSERT(false);
		return true;
	}
	return viewport->G_isstereo();
}




//*********************************************************************
// T3DScene
//*********************************************************************

T3DScene::T3DScene()
{

	cosmos=&T3DCosmos::Get();
	backcolor=G_valuecontent<TSC_color>(&addparam(_qstr("BackColor"),SC_valname_color)->content);backcolor->copyfrom(0,0,0,1);

	light0pos=G_valuecontent<Tvertex>(&addparam(_qstr("Light0Pos"),SC_valname_vertex)->content);
	light0color=G_valuecontent<TSC_color>(&addparam(_qstr("Light0Color"),SC_valname_color)->content);

	ambientlightcolor=G_valuecontent<TSC_color>(&addparam(_qstr("AmbientLightColor"),SC_valname_color)->content);ambientlightcolor->copyfrom(0,0,0,1);
	specularlightcolor=G_valuecontent<TSC_color>(&addparam(_qstr("SpecularLightColor"),SC_valname_color)->content);

	framenr=addparam(_qstr("FrameNr"),SC_valname_scalar)->content.G_content_scalar();

	QString nm,nm0;
	int nr=0;
	while (true)
	{
		nr++;
		nm="Scene_";nm+=QString(nr);
		bool found=false;
		for (int i=0; i<cosmos->G_scenecount(); i++)
		{
			cosmos->G_scene(i)->G_name(nm0);
			if (qstricmp(nm,nm0)==0) found=true;
		}
		if (!found) break;
	}
	param_setvalue(ObjNameStr,nm);

	cursoractive=false;
}

T3DScene::~T3DScene()
{
	objects.reset();
	attachedviewports.reset();
}


void T3DScene::G_sourcecodename(QString &str)//name as it should be copied to the source code
{
	str.clear();
	if (G_parent()!=NULL)
	{
		G_parent()->G_sourcecodename(str);
		str+=_qstr(".");
	}
	QString str2;
	G_name(str2);
	str+=str2;
}


TObjectTreeItem* T3DScene::G_parent()
{
	return cosmos;
}

int T3DScene::G_childcount()
{
	return objects.G_count();
}
TObjectTreeItem* T3DScene::G_child(int nr)
{
	if (nr<objects.G_count()) return objects[nr];
	nr-=objects.G_count();
	ASSERT(false);
	return NULL;
}



T3DObject* T3DScene::findobject(TObjectTreeItem *treeitem)
{
	for (int i=0; i<objects.G_count(); i++)
	{
		T3DObject *obj=objects[i]->findobject(treeitem);
		if (obj!=NULL) return obj;
	}
	return NULL;
}

void T3DScene::delchild(TObjectTreeItem *ichild)
{
	for (int i=0; i<objects.G_count(); i++)
		if (ichild==(TObjectTreeItem*) objects[i])
		{
			objects.del(i);
			return;
		}
}

void T3DScene::start()
{
	volshadow_reset();
	cursoractive=false;
}

void T3DScene::switch_cursoractive(Tvertex &icursorpos, double icursorsize)
{
	cursoractive=!cursoractive;
	if (cursoractive)
	{
		cursorsize=icursorsize;
		cursorpos=icursorpos;
	}
}

void T3DScene::cursor_move(const Tvector &vc)
{
	cursorpos.lincombfrom(1,&cursorpos,1,&vc);
}

void T3DScene::MouseArrowChangePos(const Tvector &vc)
{
	for (int i=0; i<objects.G_count(); i++)
		objects[i]->MouseArrowChangePos(vc);
}

void T3DScene::MouseArrowClick(TMouseClickInfo info)
{
	for (int i=0; i<objects.G_count(); i++)
		objects[i]->MouseArrowClick(info);
}


TRenderWindow* T3DScene::GetMyFirstRenderWindow()
{
	if (G_rendertype()==RenderSingle) return G_globalrenderwindow();
	if ((G_rendertype()==RenderDual)||(G_rendertype()==RenderMultithreaded))
	{
		ASSERT(attachedviewports.G_count()>0);
		if (attachedviewports.G_count()<=0) throw(_text("No viewport attached"));
		return attachedviewports[0]->leftwin;
	}
	ASSERT(false);
	return NULL;
}






void T3DScene::attachviewport(T3DViewport *vp)
{
	if (!vp->G_started())
		throw QError(_text("Viewport is not yet started"));
	T3DRenderViewport *rvp=new T3DRenderViewport;
	rvp->viewport=vp;
	rvp->leftwin=vp->G_leftwin();
	rvp->rightwin=vp->G_rightwin();
	attachedviewports.add(rvp);

//	rvp->leftwin->setup();
//	if (rvp->isstereo) rvp->rightwin->setup();

//	if (attachedviewports.G_count()>1) wglShareLists(rvp->leftwin->rc,attachedviewports[0]->leftwin->rc);
//	if (rvp->isstereo) wglShareLists(rvp->leftwin->rc,rvp->rightwin->rc);

}

void T3DScene::delattachedviewport(T3DViewport *vp)
{
	for (int i=0; i<attachedviewports.G_count(); i++)
		if (attachedviewports[i]->viewport==vp) attachedviewports.del(i);
}


void T3DScene::execute_functor_prerenderaction()
{
	for (int i=0; i<objects.G_count(); i++)
		objects[i]->execute_functor_prerenderaction();
}

void T3DScene::adjusttime(double timestep)
{
	for (int i=0; i<objects.G_count(); i++)
		objects[i]->adjusttime(timestep);
}


void T3DScene::precalcrender()
{
	Tprecalcrendercontext pre;
	pre.elapsed=elapsed.G_elapsed();
	for (int i=0; i<objects.G_count(); i++)
	{
		pre.parentsubframe=NULL;
		objects[i]->precalcrender(&pre);
	}
}

void T3DScene::precalcrender2(T3DRenderViewport *rvp)
{
	Tprecalcrendercontext pre;
	pre.elapsed=elapsed.G_elapsed();
	for (int i=0; i<objects.G_count(); i++)
	{
		pre.parentsubframe=NULL;
		objects[i]->precalcrender2(rvp,&pre);
	}
}

void T3DScene::calcshadowvolumes()
{
	volshadows.init(this);
	volshadows.lightpos=*light0pos;
	for (int i=0; i<objects.G_count(); i++)
	{
		objects[i]->calcshadowvolumes(&volshadows,NULL);
	}
}



void T3DScene::createvolumeshadows(Trendercontext *rc)
{
	if (false)//make shadows visible
	{
		rc->enablelight(false);
		rc->EnableDepthMask(false);
		rc->SetBlendType(BlendTransparent);
		glDisable(GL_CULL_FACE);
		rc->set_color(1,0.5,0.5,0.15);
		for (int shadownr=0; shadownr<volshadowinfo.G_count(); shadownr++)
			volshadows.rendershadowvolumes(shadownr,rc);
		return;
	}

	for (int shadownr=0; shadownr<volshadowinfo.G_count(); shadownr++)
	{
		rc->EnableDepthTest(true);

		//glStencilMask(0x2);
		glClear(GL_STENCIL_BUFFER_BIT);
		//glStencilMask(0xff);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);


		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glEnable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.0f, 100.0f);

		//back pass in stencil buffer
		glCullFace(GL_FRONT);
		glStencilFunc(GL_ALWAYS, 0x0, 0xff);
		glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
		volshadows.rendershadowvolumes(shadownr,rc);

		//front pass in stencil buffer
		glCullFace(GL_BACK);
		glStencilFunc(GL_ALWAYS, 0x0, 0xff);
		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
		volshadows.rendershadowvolumes(shadownr,rc);


		glDisable(GL_POLYGON_OFFSET_FILL);
		glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

		//draw shadow mask
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		rc->set_color(volshadowinfo[shadownr]->color.G_R(),volshadowinfo[shadownr]->color.G_G(),volshadowinfo[shadownr]->color.G_B(),volshadowinfo[shadownr]->color.G_A());
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-1,1,-1,1,0.1,2);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		gluLookAt(0,0,0,0,1,0,0,0,1);
		glBegin(GL_QUADS);
		glVertex3d(-1,0.5,-1);
		glVertex3d(+1,0.5,-1);
		glVertex3d(+1,0.5,+1);
		glVertex3d(-1,0.5,+1);
		glEnd();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		glDisable(GL_STENCIL_TEST);

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}
}


void T3DScene::drawdepthlayers(Trendercontext *rc)
{
	if (!G_3DCosmos().showdepthlayers) return;

	T3DViewport *viewport=rc->G_viewport();
	if (viewport==NULL) return;


	//determine viewport dimensions
	int vp[18];
	glGetIntegerv(GL_VIEWPORT,vp);
	int x0=vp[0];
	int y0=vp[1];
	int sx=vp[2];
	int sy=vp[3];


	Tdoublearray alphavals,colr,colg,colb,colw;

//	alphavals.add(0.95);    colr.add(0.0);colg.add(0.0);colb.add(0.5);colw.add(0.8);
	alphavals.add(0.75);    colr.add(0.0);colg.add(0.0);colb.add(1.0);colw.add(0.8);
	alphavals.add(0.5);     colr.add(0.0);colg.add(0.5);colb.add(1.0);colw.add(0.6);
	alphavals.add(0.25);    colr.add(0.0);colg.add(1.0);colb.add(0.5);colw.add(0.4);
	alphavals.add(0.0);     colr.add(1.0);colg.add(0.7);colb.add(0.0);colw.add(0.4);
	alphavals.add(-0.5);    colr.add(1.0);colg.add(0.3);colb.add(0.0);colw.add(0.6);
	alphavals.add(-1);      colr.add(1.0);colg.add(0.3);colb.add(0.0);colw.add(0.8);
	alphavals.add(-2);      colr.add(1.0);colg.add(0.0);colb.add(0.0);colw.add(1.0);
	alphavals.add(-4);      colr.add(0.7);colg.add(0.0);colb.add(0.0);colw.add(1.0);

	alphavals.add(-200);    colr.add(0.5);colg.add(0.0);colb.add(0.0);colw.add(1.0);


	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);

	for (int ct=0; ct<alphavals.G_count(); ct++)
	{
		double dist1=viewport->G_focaldistance()/(1.0-alphavals[ct+0]);
		double dist2=viewport->G_focaldistance()/(1.0-alphavals[ct+1]);

		Tvertex posit1[4],posit2[4];

		rc->unproject(x0    ,y0    ,dist1,posit1[0]);
		rc->unproject(x0+sx ,y0    ,dist1,posit1[1]);
		rc->unproject(x0+sx ,y0+sy ,dist1,posit1[2]);
		rc->unproject(x0    ,y0+sy ,dist1,posit1[3]);
		rc->unproject(x0    ,y0    ,dist2,posit2[0]);
		rc->unproject(x0+sx ,y0    ,dist2,posit2[1]);
		rc->unproject(x0+sx ,y0+sy ,dist2,posit2[2]);
		rc->unproject(x0    ,y0+sy ,dist2,posit2[3]);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		//pass back in stencil buffer
		glStencilFunc(GL_ALWAYS, 0x0, 0xff);
		glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
		rc->set_color(1,0,0,1);
		glBegin(GL_QUADS);
		for (int pidx=0; pidx<4; pidx++) glVertex3d(posit1[pidx].G3_x(),posit1[pidx].G3_y(),posit1[pidx].G3_z());
		glEnd();
		//pass front in stencil buffer
		glStencilFunc(GL_ALWAYS, 0x0, 0xff);
		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
		glBegin(GL_QUADS);
		for (int pidx=0; pidx<4; pidx++) glVertex3d(posit2[pidx].G3_x(),posit2[pidx].G3_y(),posit2[pidx].G3_z());
		glEnd();

		//draw color polygon
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		rc->set_color(colr[ct],colg[ct],colb[ct],colw[ct]);
		glBegin(GL_QUADS);
		for (int pidx=0; pidx<4; pidx++) glVertex3d(posit1[pidx].G3_x(),posit1[pidx].G3_y(),posit1[pidx].G3_z());
		glEnd();

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);


	}


	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);

}




void T3DScene::rendercursor(Trendercontext *rc)
{
	if (!cursoractive) return;

	double sz=0.5*cursorsize;
	double d1=sz/5;
	double d2=3*d1;

	rc->create_subframe_viewdir(&cursorpos);
	rc->enablelight(false);
	rc->set_color(1,0.5,0,1);
/*	glBegin(GL_LINES);
	glVertex3d(0-size,0,     0);
	glVertex3d(0+size,0,     0);
	glVertex3d(0,     0-size,0);
	glVertex3d(0,     0+size,0);
	glVertex3d(0,     0,     0-size);
	glVertex3d(0,     0,     0+size);
	glEnd();*/

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0,        0,     0);

	glVertex3d(d2,        0,  0);
	glVertex3d(d2-d1,        0+d1,  0);
	glVertex3d(+sz+d1/2,        sz-d1/2,  0);
	glVertex3d(+sz-d1/2,        sz+d1/2,  0);
	glVertex3d(0+d1,        d2-d1,  0);
	glVertex3d(0,        d2,  0);



	glEnd();
	rc->del_subframe();
}



void T3DScene::renderobjects(Trendercontext *rc)
{


	if ((G_3DCosmos().disptype==1)&&(rc->isrightpart)) return;
	if ((G_3DCosmos().disptype==2)&&(!rc->isrightpart)) return;

	T3DViewport *viewport=rc->G_viewport();
	if (viewport==NULL)
	{
		ASSERT(false);
		throw(_text("Unable to render objects: missing viewport"));
	}

	if (viewport->G_erasebackground())
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		rc->start_viewportframe();
		rc->enablelight(false);
		rc->EnableDepthMask(false);
		rc->set_color(backcolor->G_R(),backcolor->G_G(),backcolor->G_B(),backcolor->G_A());
		glBegin(GL_QUADS);
		glVertex2d(0.0,0.0);
		glVertex2d(rc->G_renderwindow()->G_screenratio(),0.0);
		glVertex2d(rc->G_renderwindow()->G_screenratio(),1.0);
		glVertex2d(0.0,1.0);
		glEnd();
		rc->EnableDepthMask(true);
		rc->enablelight(true);
		rc->del_subframe();
		rc->setdefaultnearfarplane();
	}

	//set point size calculation function
	double pscale=viewport->G_focaldistance();
	GLfloat atten[] =  { (GLfloat)(sqr((viewport->G_focaldistance()/10.0)/pscale)), 0.0f, (GLfloat)(1.0/(pscale*pscale)) };
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,atten);
	glPointParameterf(GL_POINT_SIZE_MAX,50);
	glPointParameterf(GL_POINT_SIZE_MIN,0.25f);
	glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE,1.5f);

	double light0colorfactor=viewport->G_light0colorfactor();
	TSC_color vp_light0color(light0colorfactor*light0color->G_R(),light0colorfactor*light0color->G_G(),light0colorfactor*light0color->G_B(),1);
	TSC_color vp_light0specularcolor(light0colorfactor*specularlightcolor->G_R(),light0colorfactor*specularlightcolor->G_G(),light0colorfactor*specularlightcolor->G_B(),1);

	//pass 1: solid ojects
	{
		rc->initrender(Trendercontext::RP_SOLID);
		for (int i=0; i<objects.G_count(); i++)
		{
			TObjectRenderStatus renderstatus;
			renderstatus.currentobjectblendtype=BlendNormal;
			rc->set_light0(light0pos,ambientlightcolor,&vp_light0color,&vp_light0specularcolor);
			objects[i]->render(rc,&renderstatus,false);
		}
		rc->checkerror(_text("End pass 1 render"));
	}


	rendercursor(rc);

	rc->renderlightpoints();

	//pass 2: transparent ojects
	{
		rc->initrender(Trendercontext::RP_TRANSPARENT);
		TObjectRenderStatus renderstatus;
		renderstatus.currentobjectblendtype=BlendNormal;
		for (int i=0; i<objects.G_count(); i++) objects[i]->render(rc,&renderstatus,false);
		rc->checkerror(_text("End pass render"));
	}

	//shadow volumes
	createvolumeshadows(rc);
	rc->checkerror(_text("End shadow volumes render"));

	//depth layers
	drawdepthlayers(rc);

	rc->SetBlendType(BlendNormal);


	//viewport bound overlays
	{
		double framesize=viewport->G_framesize();

		double xright=rc->G_renderwindow()->G_screenratio();

		rc->start_viewportframe();
		rc->enablelight(false);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glCullFace(GL_BACK);

		//status text
		if (G_3DCosmos().showinfoline)
		{
			rc->set_color(0.4,0.4,0.4,1);
			QString str,str1,str2;
			FormatString(str1,_text("Frame=^1"),framenr->G_intval());str+=str1;
			str+=_qstr("  ");
			str2.formatscalar(G_3DCosmos().G_framerate(),1);FormatString(str1,_text("Rate=^1/s"),str2);str+=str1;
			str+=_qstr("  ");
			G_3DCosmos().G_time()->tostring(str1,0);str+=str1;
			rc->rendertext2d(DEFAULT_FONT,framesize+0.005,framesize+0.005,0.015,str);
		}

		//show stereo grid
		if (G_3DCosmos().showstereogrid)
		{
			double cr=1,cg=0,cb=0;
			if (rc->isrightpart) { cr=0; cg=0.85; cb=0; }
			rc->SetBlendType(BlendTranslucent);
			for (int i=0; i<=12; i++)
			{
				glLineWidth(1.0f);rc->set_color(cr,cg,cb,0.5);
				if (i%3==0)
				{
					glLineWidth(2.0f);
					rc->set_color(cr,cg,cb,1);
				}
				glBegin(GL_LINES);glVertex2d(0.0,i/12.0);glVertex2d(xright,i/12.0);glEnd();
				glBegin(GL_LINES);glVertex2d(i/12.0*xright,0);glVertex2d(i/12.0*xright,1);glEnd();
			}
		}

		//viewport frame
		if (framesize>0.00001)
		{
			rc->SetBlendType(BlendNormal);
			rc->set_color(viewport->framecolor->G_R(),viewport->framecolor->G_G(),viewport->framecolor->G_B(),1.0);
			if (viewport->framebottom->G_val())
			{
				glBegin(GL_QUADS);	glVertex2d(0.0,0.0);	glVertex2d(xright,0.0);	glVertex2d(xright,framesize);	glVertex2d(0.0,framesize);	glEnd();
			}
			if (viewport->frameleft->G_val())
			{
				glBegin(GL_QUADS);	glVertex2d(0.0,0.0);	glVertex2d(framesize,0.0);	glVertex2d(framesize,1.0);	glVertex2d(0.0,1.0);	glEnd();
			}
			if (viewport->frametop->G_val())
			{
				glBegin(GL_QUADS);	glVertex2d(0.0,1.0-framesize);	glVertex2d(xright,1.0-framesize);	glVertex2d(xright,1.0);	glVertex2d(0.0,1.0);	glEnd();
			}
			if (viewport->frameright->G_val())
			{
				glBegin(GL_QUADS);	glVertex2d(xright-framesize,0.0);	glVertex2d(xright,0.0);	glVertex2d(xright,1.0);	glVertex2d(xright-framesize,1.0);	glEnd();
			}
		}

		//softwarename
		bool showshoftwarename=false;
		if (showshoftwarename)
		{
			double txtsize=0.03;
			QString txt=_qstr("www.z-flux.com © 2010");
			rc->SetBlendType(BlendTranslucent);
			rc->set_color(0.0,1.0,0.0,0.2);
			rc->rendertext(DEFAULT_FONT,&Tvertex(txtsize+framesize+0.005,framesize+0.25,0),
				&Tvector(0,txtsize,0),&Tvector(-txtsize,0,0),
				txt,0.0);
		}


		//fader rectangle
		if (viewport->G_fadecolor()->G_A()>0.001)
		{
			rc->EnableDepthTest(false);
			rc->SetBlendType(BlendTranslucent);
			rc->set_color(viewport->G_fadecolor()->G_R(),viewport->G_fadecolor()->G_G(),viewport->G_fadecolor()->G_B(),viewport->G_fadecolor()->G_A());
			glBegin(GL_QUADS);	glVertex2d(0.0,0.0);	glVertex2d(xright,0.0);	glVertex2d(xright,1);	glVertex2d(0.0,1);	glEnd();
			rc->EnableDepthTest(true);
		}


		rc->del_subframe();
	}

	//if (false)//--!!--
	//{
	//	_renderscene();
	//	return;
	//}


	rc->checkerror(_text("End renderobjects"));
}

void T3DScene::renderall()
{

	framenr->copyfrom(framenr->G_intval()+1);

	execute_functor_prerenderaction();

	precalcrender();


	calcshadowvolumes();

	if (G_rendertype()==RenderSingle)//render to single RC and window with multiple viewports
	{
		if (G_globalrenderwindow()==NULL) throw QError(_text("No render window present"));//>>>
		G_globalrenderwindow()->rendercontext.checkerror(_text("Start renderall"));
		for (int vpnr=0; vpnr<attachedviewports.G_count(); vpnr++)
		{
			T3DRenderViewport *rvp=attachedviewports[vpnr];
			if (rvp->viewport->G_active())
			{
				rvp->viewport->applycurrentmove();
				precalcrender2(rvp);
				double xminfrac=rvp->viewport->G_xminfrac();
				double xmaxfrac=rvp->viewport->G_xmaxfrac();
				double yminfrac=rvp->viewport->G_yminfrac();
				double ymaxfrac=rvp->viewport->G_ymaxfrac();
				double mulfrac=1;
				double ofsx=0;
				if (rvp->viewport->G_isswappedstereo()) ofsx=rvp->viewport->G_rightviewshiftX();
				if (rvp->G_isstereo()) mulfrac=0.5;
				G_globalrenderwindow()->render_begin(rvp->viewport,ofsx+xminfrac*mulfrac,yminfrac,ofsx+xmaxfrac*mulfrac,ymaxfrac,false);
				renderobjects(&G_globalrenderwindow()->rendercontext);
				G_globalrenderwindow()->render_end();
				if (rvp->G_isstereo())
				{
					G_globalrenderwindow()->Set_isrightwindow(true);
					ofsx=rvp->viewport->G_rightviewshiftX();
					if (rvp->viewport->G_isswappedstereo()) ofsx=0;
					double ofsy = -rvp->viewport->G_rightviewshiftY();
					G_globalrenderwindow()->render_begin(rvp->viewport,ofsx+xminfrac*0.5,ofsy+yminfrac,ofsx+xmaxfrac*0.5,ofsy+ymaxfrac,true);
					renderobjects(&G_globalrenderwindow()->rendercontext);
					G_globalrenderwindow()->render_end();
					G_globalrenderwindow()->Set_isrightwindow(false);
				}
			}
		}
	}



	if (G_rendertype()==RenderDual)//2 RC's and windows, singlethreaded
	{
		for (int vpnr=0; vpnr<attachedviewports.G_count(); vpnr++)
		{
			T3DRenderViewport *rvp=attachedviewports[vpnr];
			rvp->viewport->applycurrentmove();
			precalcrender2(rvp);
			rvp->leftwin->render_clear(backcolor);
			rvp->leftwin->render_begin(rvp->viewport,0,0,1,1,false);
			renderobjects(&rvp->leftwin->rendercontext);
			rvp->leftwin->render_end();
			if (rvp->G_isstereo())
			{
				rvp->rightwin->render_clear(backcolor);
				rvp->rightwin->render_begin(rvp->viewport,0,0,1,1,true);
				renderobjects(&rvp->rightwin->rendercontext);
				rvp->rightwin->render_end();
			}
		}
		for (int vpnr=0; vpnr<attachedviewports.G_count(); vpnr++)
		{
			T3DRenderViewport *rvp=attachedviewports[vpnr];
			rvp->leftwin->swapbuffers();
			if (rvp->G_isstereo()) rvp->rightwin->swapbuffers();
		}
		G_3DCosmos().waitforframerate();
	}


	if (G_rendertype()==RenderMultithreaded)////2 RC's and windows, multithreaded
	{
		for (int vpnr=0; vpnr<attachedviewports.G_count(); vpnr++)
		{
			T3DRenderViewport *rvp=attachedviewports[vpnr];
			rvp->viewport->applycurrentmove();
			precalcrender2(rvp);
			rvp->leftwin->renderthread.render(this,rvp->viewport);
			if (rvp->G_isstereo()) rvp->rightwin->renderthread.render(this,rvp->viewport);
		}
		G_3DCosmos().waitforframerate();
		for (int vpnr=0; vpnr<attachedviewports.G_count(); vpnr++)
		{
			T3DRenderViewport *rvp=attachedviewports[vpnr];
			rvp->leftwin->renderthread.waitforrenderfinished();
			if (rvp->G_isstereo()) rvp->rightwin->renderthread.waitforrenderfinished();
			if (!qisempty(rvp->leftwin->renderthread.G_error()))
				throw QError(rvp->leftwin->renderthread.G_error());
			if (rvp->G_isstereo())
			{
				if (!qisempty(rvp->rightwin->renderthread.G_error()))
					throw QError(rvp->rightwin->renderthread.G_error());
			}
		}
		Sleep(0);
		for (int vpnr=0; vpnr<attachedviewports.G_count(); vpnr++)
		{
			T3DRenderViewport *rvp=attachedviewports[vpnr];
			rvp->leftwin->renderthread.swap();
			if (rvp->G_isstereo()) rvp->rightwin->renderthread.swap();
		}
		for (int vpnr=0; vpnr<attachedviewports.G_count(); vpnr++)
		{
			T3DRenderViewport *rvp=attachedviewports[vpnr];
			rvp->leftwin->renderthread.waitforswapfinished();
			if (rvp->G_isstereo()) rvp->rightwin->renderthread.waitforswapfinished();
		}
		Sleep(0);
	}
}

void T3DScene::volshadow_reset()
{
	volshadowinfo.reset();
}

void T3DScene::volshadow_add(double lightradius, TSC_color *color, double depth1, double depth2)
{
	Tvolshadowinfo *info=new Tvolshadowinfo;
	info->lightradius=lightradius;
	info->depth1=depth1;
	info->depth2=depth2;
	info->color=*color;
	volshadowinfo.add(info);
}






///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scene_create,addscene)
{
	setclasspath_fromtype(SC_valname_scene);
	setreturntype(SC_valname_scene);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DScene *sc=G_3DCosmos().addscene();
	retval->encapsulate(sc);
}
ENDFUNCTION(func_scene_create)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scene_start,start)
{
	setmemberfunction(SC_valname_scene);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DScene *sc=G_valuecontent<T3DScene>(owner);
	sc->start();
}
ENDFUNCTION(func_scene_start)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scenegetsubitems,getmembers)
{
	setreturntype(SC_valname_list);
	setmemberfunction(SC_valname_scene);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TObjectTreeItem *obj=G_valuecontent<T3DScene>(owner);
	TSC_list *list=G_valuecontent<TSC_list>(retval);
	list->reset();
	for (int i=0; i<obj->G_childcount(); i++)
	{
		TSC_value vl;
		vl.settype(obj->G_child(i)->G_datatype());
		vl.encapsulate(obj->G_child(i));
		list->add(&vl);
	}
}
ENDFUNCTION(func_scenegetsubitems)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scene_addsubframe,addsubframe)
{
	setreturntype(SC_valname_subframe);
	setmemberfunction(SC_valname_scene);
	addvar(_qstr("name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DScene *scene=G_valuecontent<T3DScene>(owner);
	T3DSubFrameObject *subframe=new T3DSubFrameObject(scene);
	subframe->init();
	if (arglist->G_ispresent(0)) subframe->Set_name(arglist->get(0)->G_content_string()->G_string());
	scene->addobject(subframe);
	retval->settype(subframe->G_datatype());
	retval->encapsulate(subframe);
}
ENDFUNCTION(func_scene_addsubframe)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scene_resetshadow,VolumeShadowReset)
{
	setmemberfunction(SC_valname_scene);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DScene *scene=G_valuecontent<T3DScene>(owner);
	scene->volshadow_reset();
}
ENDFUNCTION(func_scene_resetshadow)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scene_addshadow,VolumeShadowAdd)
{
	setmemberfunction(SC_valname_scene);
	addvar(_qstr("lightradius"),SC_valname_scalar);
	addvar(_qstr("color"),SC_valname_color);
	addvar(_qstr("depth1"),SC_valname_scalar);
	addvar(_qstr("depth2"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DScene *scene=G_valuecontent<T3DScene>(owner);
	scene->volshadow_add(
		arglist->get(0)->G_content_scalar()->G_val(),
		G_valuecontent<TSC_color>(arglist->get(1)),
		arglist->get(2)->G_content_scalar()->G_val(),
		arglist->get(3)->G_content_scalar()->G_val()
		);
}
ENDFUNCTION(func_scene_addshadow)



