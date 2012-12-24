#include "stdafx.h"

#include "vecmath.h"
#include "vec_transformation.h"

#include "sc_script.h"

#include "3DScene.h"
#include "3Dsubframe.h"

#include "opengl.h"


//*********************************************************************
// T3DSubFrameObject
//*********************************************************************


T3DSubFrameObject::T3DSubFrameObject(T3DScene *iscene) : T3DObject(iscene)
{
//	isviewdirframe=addparam(_qstr("ViewDirFrame"),SC_valname_boolean)->content.G_content_boolean();isviewdirframe->copyfrom(false);

	subframetype=&addparam(SC_valname_subframetype,SC_valname_subframetype)->content;
	subframetype->fromint(SubFrameNormal);



	color=G_valuecontent<TSC_color>(&addparam(_qstr("Color"),SC_valname_color)->content);

	hasvolshadow=addparam(_qstr("CastVolumeShadow"),SC_valname_boolean)->content.G_content_boolean();hasvolshadow->copyfrom(true);


	transformation=G_valuecontent<Taffinetransformation>(&addparam(_qstr("Transf"),SC_valname_affinetransformation)->content);
	motionname=addparam(_qstr("MotionName"),SC_valname_string)->content.G_content_string();motionname->fromstring(MOTIONNAME_DEFAULT);
	mass=addparam(_qstr("Mass"),SC_valname_scalar)->content.G_content_scalar();mass->copyfrom(1.0);
	charge=addparam(_qstr("Charge"),SC_valname_scalar)->content.G_content_scalar();charge->copyfrom(0.0);

	blinkperiod=addparam(_qstr("BlinkPeriod"),SC_valname_scalar)->content.G_content_scalar();blinkperiod->copyfrom(0.0);
	maxblinkcount=addparam(_qstr("MaxBlinkCount"),SC_valname_scalar)->content.G_content_scalar();maxblinkcount->copyfrom(-1);
	blinkstarting=false;blinkstopping=false;blinking=false;blinkcount=0;


	blendtype=&addparam(SC_valname_blendtype,SC_valname_blendtype)->content;
	depthmask=&addparam(SC_valname_depthmask,SC_valname_depthmask)->content;
	depthtest=&addparam(SC_valname_depthtest,SC_valname_depthtest)->content;
	
	nearclipplane=addparam(_qstr("NearClipPlane"),SC_valname_scalar)->content.G_content_scalar();
	farclipplane=addparam(_qstr("FarClipPlane"),SC_valname_scalar)->content.G_content_scalar();
	nearclipplane->copyfrom(-1);
	farclipplane->copyfrom(-1);

	autosortbydistance=addparam(_qstr("AutoSort"),SC_valname_boolean)->content.G_content_boolean();autosortbydistance->copyfrom(false);


	enablelight=addparam(_qstr("EnableLight"),SC_valname_boolean)->content.G_content_boolean();

	cancache=addparam(_qstr("CanCache"),SC_valname_boolean,false,_text("Wraps the rendering of this subframe in a display list"))->content.G_content_boolean();
	cancache->copyfrom(false);
	displaylist=NULL;

	hasmousearrow=false;

}

T3DSubFrameObject::~T3DSubFrameObject()
{
	motionlist.reset();
	deletetextures();
}

int T3DSubFrameObject::G_childcount()
{
	int ct=members.G_count();
	if (texturelist.G_childcount()>0) ct++;
	if (motionlist.G_childcount()>0) ct++;
	return ct;
}
TObjectTreeItem* T3DSubFrameObject::G_child(int nr)
{
	if (nr<members.G_count()) return members[nr];
	nr-=members.G_count();
	if (texturelist.G_childcount()>0)
	{
		if (nr==0) return &texturelist;
		nr--;
	}
	if (motionlist.G_childcount()>0)
	{
		if (nr==0) return &motionlist;
		nr--;
	}
	ASSERT(false);
	return NULL;
}


T3DObject* T3DSubFrameObject::findobject(TObjectTreeItem *treeitem)
{
	if ((T3DObject*)this==treeitem) return this;
	for (int i=0; i<members.G_count(); i++)
	{
		T3DObject *obj=members[i]->findobject(treeitem);
		if (obj!=NULL) return obj;
	}
	return NULL;
}

void T3DSubFrameObject::delchild(TObjectTreeItem *ichild)
{
	for (int i=0; i<members.G_count(); i++)
		if (ichild==(TObjectTreeItem*) members[i])
		{
			members.del(i);
			return;
		}
}

T3DTexture* T3DSubFrameObject::G_texture(StrPtr name)
{
	T3DTexture *texture=texturelist.G_texture(name);
	if (texture==NULL)
	{
		if (parentframe!=NULL)
			texture=parentframe->G_texture(name);
	}
	return texture;
	
}

void T3DSubFrameObject::deletetextures()
{
	if (texturelist.G_count()<=0) return;
	ASSERT(G_scene()!=NULL);
	if (G_scene()==NULL) return;
	if (G_scene()->GetMyFirstRenderWindow()==NULL) return;
	G_scene()->GetMyFirstRenderWindow()->GetRC();
	texturelist.clear();
	//G_scene()->GetMyFirstRenderWindow()->ReleaseRC();//???
}

T3DTexture* T3DSubFrameObject::addtexture(StrPtr name, StrPtr bitmapfile, StrPtr bitmapalphafile)
{
	if (G_scene()->GetMyFirstRenderWindow()==NULL) throw QError(_text("Unable to create texture: there are render windows attached to the scene"));
	T3DTexture *tx=new T3DTexture;
	tx->Set_name(name);
	tx->Set_subframe(this);
	texturelist.add(tx);

	G_scene()->GetMyFirstRenderWindow()->GetRC();
	tx->loadbitmapfile(bitmapfile,bitmapalphafile);
	return tx;
}

T3DTexture* T3DSubFrameObject::addbitmaptexture(StrPtr name, Tbmp *bmp, Tbmp *bmpalpha)
{
	if (G_scene()->GetMyFirstRenderWindow()==NULL) throw QError(_text("Unable to create texture: there are render windows attached to the scene"));
	T3DTexture *tx=new T3DTexture;
	tx->Set_name(name);
	tx->Set_subframe(this);
	texturelist.add(tx);

	G_scene()->GetMyFirstRenderWindow()->GetRC();
	tx->loadbitmap(bmp,bmpalpha);
	return tx;
}


T3DTexture* T3DSubFrameObject::addvideotexture(StrPtr name, T3DVideo *video)
{
	if (G_scene()->GetMyFirstRenderWindow()==NULL) throw QError(_text("Unable to create texture: there are render windows attached to the scene"));
	T3DTexture *tx=new T3DTexture;
	tx->Set_name(name);
	tx->Set_subframe(this);
	texturelist.add(tx);
	G_scene()->GetMyFirstRenderWindow()->GetRC();
	video->createtexture();
	tx->attachvideo(video);
	return tx;
}

void T3DSubFrameObject::dropmember(StrPtr iname)
{
	for (int i=0; i<members.G_count(); i++)
	{
		QString nm;
		members[i]->G_name(nm);
		if (issame(nm,iname)) 
		{
			members.del(i);
			return;
		}
	}
}


void T3DSubFrameObject::moveobject(StrPtr objname, int step)
{
	if (step==0) return;
	int objnr=-1;
	for (int i=0; i<members.G_count(); i++)
	{
		QString nm;
		members[i]->G_name(nm);
		if (issame(nm,objname)) objnr=i;
	}
	if (objnr<0)
	{
		QString err;
		FormatString(err,_text("Could not find member object ^1"),objname);
		throw QError(err);
	}
	int dir=abs(step)/step;
	for (int i=0; i<abs(step); i++)
	{
		if ((objnr+dir<0)||(objnr+dir>=members.G_count())) return;
		T3DObject *tmp=members[objnr+dir];
		members.set(objnr+dir,members[objnr]);
		members.set(objnr,tmp);
		objnr+=dir;
	}
}


void T3DSubFrameObject::objectchangeparent(StrPtr objname, T3DSubFrameObject *subframe_dest)
{
	int objnr=-1;
	for (int i=0; i<members.G_count(); i++)
	{
		QString nm;
		members[i]->G_name(nm);
		if (issame(nm,objname)) objnr=i;
	}
	if (objnr<0)
	{
		QString err;
		FormatString(err,_text("Could not find member object ^1"),objname);
		throw QError(err);
	}
	subframe_dest->members.add(members[objnr]);
	members[objnr]->Set_parentframe(subframe_dest);
	members.set(objnr,NULL);
	members.del(objnr);
}


void T3DSubFrameObject::makeviewdirframe(Tvertex *origin)
{
	//isviewdirframe->copyfrom(true);
	subframetype->fromint(SubFrameViewDir);
	Tvector origv;origv.copyfrom(*origin);
	transformation->maketranslation(&origv);
}

void T3DSubFrameObject::makescreenframe()
{
	subframetype->fromint(SubFrameScreen);
	transformation->reset();
}

void T3DSubFrameObject::makeviewportframe()
{
	subframetype->fromint(SubFrameViewPort);
	transformation->reset();
}


void T3DSubFrameObject::Set_velocity(const Tvector *v)
{
	solidvelocity.Set_origin_vector(v);
}

void T3DSubFrameObject::G_velocity(Tvector *v) const
{
	solidvelocity.G_origin_vector(v);
}

void T3DSubFrameObject::G_totaltransformation(Taffinetransformation *transf)
{
	*transf=*transformation;
	
	T3DSubFrameObject *fr=this;
	while (fr->G_parentframe()!=NULL)
	{
		T3DSubFrameObject *parentframe=fr->G_parentframe();
		Taffinetransformation temptf;temptf=*transf;
		transf->multiply(parentframe->transformation,&temptf);
		fr=parentframe;
	}
}



void T3DSubFrameObject::Set_motionname(StrPtr iname)
{
	motionname->fromstring(iname);
}


T3DMotion* T3DSubFrameObject::G_mymotion()
{
	return findmotion(motionname->G_string());
}

T3DMotion* T3DSubFrameObject::findmotion(StrPtr name)
{
	T3DMotion *mt=motionlist.G_motion(name);
	if (mt!=NULL) return mt;
	if (parentframe!=NULL) return parentframe->findmotion(name);

	if ((qstricmp(name,MOTIONNAME_DEFAULT)==0)||(qstrlen(name)<=0))
	{
		return T3DCosmos::Get().G_defaultmotion();
	}

	QString error;
	FormatString(error,_text("Unable to find motion '^1'"),name);
	throw QError(error);
}

bool T3DSubFrameObject::G_cancache()
{
	return cancache->G_val();
}

bool T3DSubFrameObject::G_needrecache()
{
	if (isdirty) return true;
	for (int i=0; i<members.G_count(); i++)
		if (members[i]->G_needrecache()) return true;
	return false;
}

bool T3DSubFrameObject::G_iscached()
{
	return (G_cancache()&&(displaylist!=NULL)&&(!G_needrecache()));
}


void T3DSubFrameObject::addclipplane(const Tplane &plane, StrPtr viewportname)
{
	Tclipplane *pl=new Tclipplane;
	*((Tplane*)pl)=plane;
	pl->viewportname=viewportname;
	clipplanes.add(pl);
}

void T3DSubFrameObject::resetclipplanes()
{
	clipplanes.reset();
}

void T3DSubFrameObject::addignoreviewport(StrPtr viewportname)
{
	ignoreviewportlist.add(new QString(viewportname));
}

void T3DSubFrameObject::delignoreviewport(StrPtr viewportname)
{
	for (int i=0; i<ignoreviewportlist.G_count();)
		if (issame(*ignoreviewportlist[i],viewportname))
			ignoreviewportlist.del(i);
		else 
			i++;
}


void T3DSubFrameObject::adjusttime(double timestep)
{
	TSolidObjectData obj;
	obj.mass=G_mass();
	obj.charge=G_charge();
	obj.position=transformation;
	obj.velocity=&solidvelocity;
	T3DMotion *mt=G_mymotion();
	if (mt!=NULL) mt->apply(&obj,timestep);
	for (int i=0; i<members.G_count(); i++)
		members[i]->adjusttime(timestep);
}


void T3DSubFrameObject::execute_functor_prerenderaction()
{
	TSC_functor *functor=functor_prerenderaction.G_ref();
	if (functor!=NULL)
	{
		try{
			TSC_funcarglist arglist(NULL);
			TSC_value retval0,arg;
			arg.createtype(GetTSCenv().G_datatype(SC_valname_subframe));
			arglist.add(&arg);
			arg.encapsulate(this);
			functor->eval(&arglist,&retval0);
		}
		catch(TSC_runerror err)
		{
			QString errstring;
			FormatString(errstring,_text("Custom point motion: ^1"),err.G_content());
			throw QError(errstring);
		}
		catch(QError err)
		{
			QString errstring;
			FormatString(errstring,_text("Custom point motion: ^1"),err.G_content());
			throw QError(errstring);
		}
	}
	for (int i=0; i<members.G_count(); i++)
		members[i]->execute_functor_prerenderaction();
}


void T3DSubFrameObject::paramchanged(StrPtr iname)
{
	__super::paramchanged(iname);
	if (issame(iname,_qstr("BlinkPeriod")))
	{
		if (blinkperiod->G_val()>=1.0e-9)
		{
			blinkstarting=true;blinkcount=0;
			blinkperiodval=blinkperiod->G_val();
		}
		else blinkstopping=true;
	}
}


void T3DSubFrameObject::precalcrender(Tprecalcrendercontext *pre)
{
	if (blinkstarting)
	{
		blinkoffset=pre->elapsed;
		blinkstarting=false;blinking=true;
	}
	if (blinking)
	{
		bool prevblinkvisible=blinkvisible;
		blinkvisible=(int)((pre->elapsed-blinkoffset)/blinkperiodval)%2==0;
		if ((blinkvisible)&&(blinkvisible!=prevblinkvisible))
		{
			blinkcount++;
			if ((blinkcount>maxblinkcount->G_val())&&(maxblinkcount->G_val()>0))
			{
				blinking=false;
			}
		}
		if ((blinkstopping)&&(blinkvisible!=prevblinkvisible))
		{
			blinkstopping=false;
			blinking=false;
		}
	}
	else
	{
		if (!G_isvisible()) return;
	}

	execute_functor_prerenderaction();

	rendercolor=*color;
	rendertransformation=*transformation;
	if (pre->parentsubframe!=NULL)
	{
		rendercolor.mulwith(pre->parentsubframe->G_rendercolor());
//		Taffinetransformation tmptrans;
//		tmptrans=(rendertransformation);
//		rendertransformation.multiply(pre->parentsubframe->G_rendertransformation(),&tmptrans);
		rendertransformation.multiply(pre->parentsubframe->G_rendertransformation(),transformation);
	}
	for (int i=0; i<members.G_count(); i++)
	{
		pre->parentsubframe=this;
		members[i]->precalcrender(pre);
	}
}

void T3DSubFrameObject::precalcrender2(T3DRenderViewport *rvp, Tprecalcrendercontext *pre)
{
	if (blinking)
	{
		if (!blinkvisible) return;
	}
	else
	{
		if (!G_isvisible()) return;
	}
	rendercolor=*color;
	rendertransformation=*transformation;
	if (pre->parentsubframe!=NULL)
	{
		rendercolor.mulwith(pre->parentsubframe->G_rendercolor());
//		Taffinetransformation tmptrans;
//		tmptrans=(rendertransformation);
//		rendertransformation.multiply(pre->parentsubframe->G_rendertransformation(),&tmptrans);
		rendertransformation.multiply(pre->parentsubframe->G_rendertransformation(),transformation);
	}
	for (int i=0; i<members.G_count(); i++)
	{
		pre->parentsubframe=this;
		members[i]->precalcrender2(rvp,pre);
	}

	sortidx.reset();
	for (int i=0; i<members.G_count(); i++) sortidx.add(i);

	if (autosortbydistance->G_val())//auto-sort by distance
	{
		Tvertex *campos=rvp->viewport->G_camerapos();
		//Tvector *camdir=rvp->viewport->G_cameradir();
		Thomogcoord tcenter,centdir;
		sortvalidx.reset();
		for (int i=0; i<members.G_count(); i++)
		{
			rendertransformation.mul(members[i]->G_centerpos(),&tcenter);
			centdir.lincombfrom(+1,&tcenter,-1,campos);
			//double dst=Thomogcoord::dot4(&centdir,camdir);
			double dst=Thomogcoord::dot4(&centdir,&centdir);
			sortvalidx.add(-dst);
		}
		doublearray_sort(sortvalidx,sortidx);
	}
}



void T3DSubFrameObject::render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording)
{
	TObjectRenderStatus substatus=*status;

	for (int i=0; i<ignoreviewportlist.G_count(); i++)
		if (issame(*ignoreviewportlist[i],rc->G_viewportname()))
			return;

	if (blinking)
	{
		if (!blinkvisible) return;
	}
	else
	{
		if (!G_isvisible(rc)) return;
	}

	bool customnearfarplane=false;
	if ((nearclipplane->G_val()>0)&&(farclipplane->G_val()>0))
	{
		customnearfarplane=true;
		rc->setnearfarplane(nearclipplane->G_val(),farclipplane->G_val());
	}

	//transparency, blend type & depth mask logic
	if (blendtype->toint()!=BlendNormal) substatus.currentobjectblendtype=blendtype->toint();
	if (depthmask->toint()!=DepthMaskNormal) substatus.currentobjectdepthmask=depthmask->toint();
	if (depthtest->toint()!=DepthTestNormal) substatus.currentobjectdepthtest=depthtest->toint();
	if (!enablelight->G_val()) substatus.enablelight=false;

	if ((rc->currentrenderpass==Trendercontext::RP_SOLID)&&(substatus.currentobjectblendtype!=BlendNormal)) return;

//	rc->EnableDepthMask(substatus.currentobjectdepthmask);
//	rc->EnableDepthTest(substatus.currentobjectdepthtest);

	if (subframetype->toint()==SubFrameNormal) 
	{
//		rc->create_subframe(transformation);
		rc->create_subframe_fulltransformation(&rendertransformation);//-> this allows more precision in case of stacks of large, compensating transformations
	}

	if (subframetype->toint()==SubFrameViewDir) 
	{
		ASSERT(displaylist==NULL);ASSERT(!isrecording);
		Tvertex origin;
		transformation->G_origin(&origin);
		rc->create_subframe_viewdir(&origin);
	}

	if (subframetype->toint()==SubFrameScreen) 
	{
		ASSERT(displaylist==NULL);ASSERT(!isrecording);
		rc->create_subframe_screen();
	}

	if (subframetype->toint()==SubFrameViewPort) 
	{
		ASSERT(displaylist==NULL);ASSERT(!isrecording);
		rc->create_subframe_viewport();
	}

	for (int plnr=0; plnr<clipplanes.G_count(); plnr++)
		if ((clipplanes[plnr]->viewportname.G_length()<=0)||
			(issame(clipplanes[plnr]->viewportname,rc->G_viewportname())))
			rc->addclipplane(*clipplanes[plnr]);

	if (G_cancache()&&(displaylist!=NULL)&&(!G_needrecache()))
	{//draw display list
		displaylist->draw();
	}
	else
	{//explicit render
		if (displaylist!=NULL) rc->ForgetDisplayList(displaylist);displaylist=NULL;
		if (G_cancache()&&(!isrecording))
		{
			displaylist=rc->CreateDisplayList();
			displaylist->startrecord();
		}
		for (int i=0; i<members.G_count(); i++)
			members[sortidx[i]]->render(rc,&substatus,displaylist!=NULL);
		if (displaylist!=NULL) displaylist->endrecord();
		isdirty=false;
	}

	for (int plnr=0; plnr<clipplanes.G_count(); plnr++)
		if ((clipplanes[plnr]->viewportname.G_length()<=0)||
			(issame(clipplanes[plnr]->viewportname,rc->G_viewportname())))
			rc->delclipplane();

	if ((hasmousearrow)&&(G_3DCosmos().G_showcontrols()))
	{//render mousearrow
		double xp=mousearrow_pos.G3_x();
		double yp=mousearrow_pos.G3_y();
		double sz=mousearrow_size;
		double d1=sz/5;
		double d2=3*d1;
		rc->enablelight(false);
		rc->set_color(0.5,1.0,0,1);
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(xp,        yp+0,     0);
		glVertex3d(xp+d2,        yp+0,  0);
		glVertex3d(xp+d2-d1,        yp+0+d1,  0);
		glVertex3d(xp+sz+d1/2,        yp+sz-d1/2,  0);
		glVertex3d(xp+sz-d1/2,        yp+sz+d1/2,  0);
		glVertex3d(xp+0+d1,        yp+d2-d1,  0);
		glVertex3d(xp+0,        yp+d2,  0);
		glEnd();
	}


	rc->del_subframe();

	if (customnearfarplane)
	{
		rc->setdefaultnearfarplane();
	}

}

void T3DSubFrameObject::calcshadowvolumes(Tshadowvolumecontext *svc, T3DSubFrameObject *parentsubframe)
{
	if (blinking)
	{
		if (!blinkvisible) return;
	}
	else
	{
		if (!G_isvisible()) return;
	}

	if (!G_hasvolshadow()) return;
	rendertransformation=*transformation;
	if (parentsubframe!=NULL)
	{
		Taffinetransformation tmptrans;
		tmptrans=(rendertransformation);
		rendertransformation.multiply(parentsubframe->G_rendertransformation(),&tmptrans);
	}
	for (int i=0; i<members.G_count(); i++)
		members[i]->calcshadowvolumes(svc,this);
}

void T3DSubFrameObject::addmotion(T3DMotion *motion)
{
	motionlist.add(motion);
	motion->Set_parentlist(&motionlist);
}


void T3DSubFrameObject::MouseArrowEnable(const Tvertex &posit0, const Tvertex &bboxmin, const Tvertex &bboxmax, double size)
{
	hasmousearrow=true;
	mousearrow_pos=posit0;
	mousearrow_bboxmin=bboxmin;mousearrow_bboxmax=bboxmax;
	mousearrow_size=size;
}

void T3DSubFrameObject::MouseArrowChangePos(const Tvector &vc)
{
	for (int i=0; i<members.G_count(); i++)
		members[i]->MouseArrowChangePos(vc);
	if (hasmousearrow)
	{
		mousearrow_pos.lincombfrom(1,&mousearrow_pos,1,&vc);
		if (mousearrow_pos.G3_x()<mousearrow_bboxmin.G3_x()) mousearrow_pos.S3_x(mousearrow_bboxmin.G3_x());
		if (mousearrow_pos.G3_y()<mousearrow_bboxmin.G3_y()) mousearrow_pos.S3_y(mousearrow_bboxmin.G3_y());
		if (mousearrow_pos.G3_x()>mousearrow_bboxmax.G3_x()) mousearrow_pos.S3_x(mousearrow_bboxmax.G3_x());
		if (mousearrow_pos.G3_y()>mousearrow_bboxmax.G3_y()) mousearrow_pos.S3_y(mousearrow_bboxmax.G3_y());
	}
}

void T3DSubFrameObject::MouseArrowClick(TMouseClickInfo info)
{
	if (hasmousearrow)
		info.posit=mousearrow_pos;
	for (int i=0; i<members.G_count(); i++)
		members[i]->MouseArrowClick(info);
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_subframevelocity,velocity)
{
	setreturntype(SC_valname_vector);
	setmemberfunction(SC_valname_subframe);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *frame=G_valuecontent<T3DSubFrameObject>(owner);
	if (assigntoval) frame->Set_velocity(G_valuecontent<Tvector>(assigntoval));
	frame->G_velocity(G_valuecontent<Tvector>(retval));
}
ENDFUNCTION(func_subframevelocity)




///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_subframemotion,motion)
{
	setreturntype(SC_valname_any);
	setmemberfunction(SC_valname_subframe);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *frame=G_valuecontent<T3DSubFrameObject>(owner);
	if (assigntoval!=NULL)
	{
		QString motionname;
		GetObjectName(assigntoval,motionname);
		frame->Set_motionname(motionname);
	}
	T3DMotion *mt=frame->G_mymotion();
	if (mt==NULL) throw QError(_text("Invalid or absent motion"));
	retval->settype(mt->G_datatype());
	retval->encapsulate(mt);
}
ENDFUNCTION(func_subframemotion)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_subframesetprerenderaction,setprerenderaction)
{
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("action"),SC_valname_functor);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *obj=G_valuecontent<T3DSubFrameObject>(owner);
	TSC_functor *functor=G_valuecontent<TSC_functor>(arglist->get(0));
	obj->functor_prerenderaction.setreffor(functor);
}
ENDFUNCTION(func_subframesetprerenderaction)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_subframegetsubitem,get)
{
	setreturntype(SC_valname_any);
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("name"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TObjectTreeItem *obj=G_valuecontent<T3DSubFrameObject>(owner);
	TObjectTreeItem *subobj=obj->G_childbyname(arglist->get(0)->G_content_string()->G_string());
	if (subobj==NULL) throw QError(_text("Invalid subobject name"));
	retval->settype(subobj->G_datatype());
	retval->encapsulate(subobj);
}
ENDFUNCTION(func_subframegetsubitem)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_subframegetsubitems,getmembers)
{
	setreturntype(SC_valname_list);
	setmemberfunction(SC_valname_subframe);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TObjectTreeItem *obj=G_valuecontent<T3DSubFrameObject>(owner);
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
ENDFUNCTION(func_subframegetsubitems)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_subframegetparent,getparent)
{
	setreturntype(SC_valname_subframe);
	setmemberfunction(SC_valname_subframe);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *obj=G_valuecontent<T3DSubFrameObject>(owner);
	retval->encapsulate(obj->G_parentframe());
}
ENDFUNCTION(func_subframegetparent)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_addobject_subframe,add)
{
	setreturntype(SC_valname_any);
	setmemberfunction(SC_valname_subframe);
	setflexiblearglist();
	addvar(_qstr("type"),SC_valname_string);
	addvar(_qstr("attribute"),SC_valname_mappair,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);
	StrPtr objtypename=arglist->get(0)->G_content_string()->G_string();
	T3DObject *catobj=T3DCosmos::Get().G_objectcatalog_object(objtypename);
	if (catobj==NULL) throw QError(_text("Invalid object type"));
	T3DObject *obj=catobj->MakeInstance();
	obj->Set_scene(subframe->G_scene());
	obj->Set_parentframe(subframe);

	for (int i=1; i<arglist->G_count(); i++)
	{
		TSC_mappair *mappair=G_valuecontent<TSC_mappair>(arglist->get(i));
		Tparam *param=obj->G_param(mappair->G_name());
		if (param==NULL)
		{
			QString err;
			FormatString(err,_text("Invalid parameter ^1"),mappair->G_name());
			throw QError(err);
		}
		param->content.copyfrom(mappair->G_item());
	}

	subframe->addsubobject(obj);
	retval->settype(obj->G_datatype());
	retval->encapsulate(obj);
}
ENDFUNCTION(func_addobject_subframe)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_dropobject_subframe,drop)
{
	setmemberfunction(SC_valname_subframe);
	setflexiblearglist();
	addvar(_qstr("name"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);
	StrPtr name=arglist->get(0)->G_content_string()->G_string();

	subframe->dropmember(name);
/*
	T3DObject *catobj=T3DCosmos::Get().G_objectcatalog_object(objtypename);
	if (catobj==NULL) throw QError(_text("Invalid object type"));
	T3DObject *obj=catobj->MakeInstance();
	obj->Set_scene(subframe->G_scene());
	obj->Set_parentframe(subframe);

	for (int i=1; i<arglist->G_count(); i++)
	{
		TSC_mappair *mappair=G_valuecontent<TSC_mappair>(arglist->get(i));
		Tparam *param=obj->G_param(mappair->G_name());
		if (param==NULL)
		{
			QString err;
			FormatString(err,_text("Invalid parameter ^1"),mappair->G_name());
			throw QError(err);
		}
		param->content.copyfrom(mappair->G_item());
	}

	subframe->addsubobject(obj);
	retval->settype(obj->G_datatype());
	retval->encapsulate(obj);*/
}
ENDFUNCTION(func_dropobject_subframe)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_moveobject_subframe,moveobject)
{
	setreturntype(SC_valname_any);
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("objectname"),SC_valname_string);
	addvar(_qstr("step"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);
	StrPtr objname=arglist->get(0)->G_content_string()->G_string();
	int step=arglist->get(1)->G_content_scalar()->G_intval();
	subframe->moveobject(objname,step);
}
ENDFUNCTION(func_moveobject_subframe)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_moveobject_cangesubframe,objectchangeparent)
{
	setreturntype(SC_valname_any);
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("destframe"),SC_valname_subframe);
	addvar(_qstr("objectname"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe_from=G_valuecontent<T3DSubFrameObject>(owner);
	T3DSubFrameObject *subframe_dest=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	StrPtr objname=arglist->get(1)->G_content_string()->G_string();
	subframe_from->objectchangeparent(objname,subframe_dest);
}
ENDFUNCTION(func_moveobject_cangesubframe)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_clearobjects_subframe,clearobjects)
{
	setmemberfunction(SC_valname_subframe);
//	description="Removes all member objects";
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);
	subframe->clearobjects();
}
ENDFUNCTION(func_clearobjects_subframe)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_addsubframe,addsubframe)
{
	setreturntype(SC_valname_subframe);
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe0=G_valuecontent<T3DSubFrameObject>(owner);
	T3DSubFrameObject *newsubframe=new T3DSubFrameObject(subframe0->G_scene());
	if (arglist->G_ispresent(0)) newsubframe->Set_name(arglist->get(0)->G_content_string()->G_string());
	subframe0->addsubobject(newsubframe);
	retval->settype(newsubframe->G_datatype());
	retval->encapsulate(newsubframe);
}
ENDFUNCTION(func_object_addsubframe)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_addviewdirframe,addviewdirframe)
{
	setreturntype(SC_valname_subframe);
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("position"),SC_valname_vertex);
	addvar(_qstr("name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe0=G_valuecontent<T3DSubFrameObject>(owner);
	T3DSubFrameObject *newsubframe=new T3DSubFrameObject(subframe0->G_scene());
	newsubframe->makeviewdirframe(G_valuecontent<Tvertex>(arglist->get(0)));
	if (arglist->G_ispresent(1)) newsubframe->Set_name(arglist->get(1)->G_content_string()->G_string());
	subframe0->addsubobject(newsubframe);
	retval->settype(newsubframe->G_datatype());
	retval->encapsulate(newsubframe);
}
ENDFUNCTION(func_object_addviewdirframe)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_addscreenframe,addscreenframe)
{
	setreturntype(SC_valname_subframe);
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe0=G_valuecontent<T3DSubFrameObject>(owner);
	T3DSubFrameObject *newsubframe=new T3DSubFrameObject(subframe0->G_scene());
	newsubframe->makescreenframe();
	if (arglist->G_ispresent(0)) newsubframe->Set_name(arglist->get(0)->G_content_string()->G_string());
	subframe0->addsubobject(newsubframe);
	retval->settype(newsubframe->G_datatype());
	retval->encapsulate(newsubframe);
}
ENDFUNCTION(func_object_addscreenframe)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_addviewportframe,addviewportframe)
{
	setreturntype(SC_valname_subframe);
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe0=G_valuecontent<T3DSubFrameObject>(owner);
	T3DSubFrameObject *newsubframe=new T3DSubFrameObject(subframe0->G_scene());
	newsubframe->makeviewportframe();
	if (arglist->G_ispresent(0)) newsubframe->Set_name(arglist->get(0)->G_content_string()->G_string());
	subframe0->addsubobject(newsubframe);
	retval->settype(newsubframe->G_datatype());
	retval->encapsulate(newsubframe);
}
ENDFUNCTION(func_object_addviewportframe)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_totaltransformation,totaltransformation)
{
	setreturntype(SC_valname_affinetransformation);
	setmemberfunction(SC_valname_subframe);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe0=G_valuecontent<T3DSubFrameObject>(owner);

	Taffinetransformation *m=G_valuecontent<Taffinetransformation>(retval);

	subframe0->G_totaltransformation(m);

/*	Taffinetransformation *tf;
	subframe0->G_totaltransformation(
	retval->
	retval->settype(newsubframe->G_datatype());
	retval->encapsulate(newsubframe);*/
}
ENDFUNCTION(func_object_totaltransformation)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_addignoreviewport,addignoreviewport)
{
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("viewportname"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);
	StrPtr viewportname=arglist->get(0)->G_content_string()->G_string();
	subframe->addignoreviewport(viewportname);
}
ENDFUNCTION(func_object_addignoreviewport)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_delignoreviewport,delignoreviewport)
{
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("viewportname"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);
	StrPtr viewportname=arglist->get(0)->G_content_string()->G_string();
	subframe->delignoreviewport(viewportname);
}
ENDFUNCTION(func_object_delignoreviewport)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_addclipplane,addclipplane)
{
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("plane"),SC_valname_plane);
	addvar(_qstr("viewportname"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);
	Tplane *plane=G_valuecontent<Tplane>(arglist->get(0));
	QString viewportname;
	if (arglist->G_ispresent(1))
		viewportname=arglist->get(1)->G_content_string()->G_string();
	subframe->addclipplane(*plane,viewportname);
}
ENDFUNCTION(func_object_addclipplane)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_resetclipplanes,resetclipplanes)
{
	setmemberfunction(SC_valname_subframe);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);
	subframe->resetclipplanes();
}
ENDFUNCTION(func_object_resetclipplanes)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_getboundingbox,BoundingBox)
{
	setmemberfunction(SC_valname_subframe);
	setreturntype(SC_valname_map);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);
	Tvertex v1,v2;
//	subframe->G_BoundingBox(v1,v2);
//	TSC_map *map=...  --!!--
}
ENDFUNCTION(func_object_getboundingbox)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_forgetcache,ForgetCache)
{
	setmemberfunction(SC_valname_subframe);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe0=G_valuecontent<T3DSubFrameObject>(owner);
	subframe0->makedirty();
}
ENDFUNCTION(func_object_forgetcache)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_load,LoadObject)
{
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("FilePath"),SC_valname_string);
	addvar(_qstr("FileName"),SC_valname_string);
	addvar(_qstr("FileType"),SC_valname_string);
	addvar(_qstr("ScaleFactor"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);
	StrPtr filepath=arglist->get(0)->G_content_string()->G_string();
	StrPtr filename=arglist->get(1)->G_content_string()->G_string();
	StrPtr filetype=arglist->get(2)->G_content_string()->G_string();

	double scalefactor=1.0;
	if (arglist->G_ispresent(3)) scalefactor=arglist->get(3)->G_content_scalar()->G_val();

	if (issame(filetype,_qstr("3DS")))
	{
		subframe->loadobject_3ds(filepath,filename,scalefactor);
		return;
	}
	throw QError(_text("Unrecognised object type"));
}
ENDFUNCTION(func_object_load)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_mousearrow_enable,EnabeMouseArrow)
{
	setmemberfunction(SC_valname_subframe);
	addvar(_qstr("Position"),SC_valname_vertex);
	addvar(_qstr("BoundingBoxMin"),SC_valname_vertex);
	addvar(_qstr("BoundingBoxMax"),SC_valname_vertex);
	addvar(_qstr("Size"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe0=G_valuecontent<T3DSubFrameObject>(owner);
	subframe0->MouseArrowEnable(
		*G_valuecontent<Tvertex>(arglist->get(0)),
		*G_valuecontent<Tvertex>(arglist->get(1)),
		*G_valuecontent<Tvertex>(arglist->get(2)),
		arglist->get(3)->G_content_scalar()->G_val());
}
ENDFUNCTION(func_mousearrow_enable)
