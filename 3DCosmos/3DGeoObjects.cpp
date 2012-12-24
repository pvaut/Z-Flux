#include "stdafx.h"

#include "vecmath.h"
#include "vec_transformation.h"

#include "opengl.h"

#include "3DScene.h"
#include "3Dsurface.h"
#include "3DGeoObjects.h"
#include "3DFont.h"

#include "opengl.h"


//*********************************************************************
// T3DObject
//*********************************************************************



T3DObject::T3DObject(T3DScene *iscene)
{
	parentframe=NULL;
	scene=iscene;
	cosmos=&T3DCosmos::Get();
	isobjvisible=addparam(_qstr("Visible"),SC_valname_boolean)->content.G_content_boolean();

	showlefteye=addparam(_qstr("ShowLeftEye"),SC_valname_boolean)->content.G_content_boolean();
	showrighteye=addparam(_qstr("ShowRightEye"),SC_valname_boolean)->content.G_content_boolean();

	centerpos=G_valuecontent<Tvertex>(&addparam(_qstr("Center"),SC_valname_vertex)->content);

}

T3DObject::~T3DObject()
{
}

void T3DObject::G_sourcecodename(QString &str)//name as it should be copied to the source code
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

bool T3DObject::G_isvisible()
{
	return isobjvisible->G_val();
}

bool T3DObject::G_isvisible(Trendercontext *rc)
{
	if (!isobjvisible->G_val())
		return false;
	if (!rc->isrightpart)
	{
		if (!showlefteye->G_val())
			return false;
	}
	else
	{
		if (!showrighteye->G_val())
			return false;
	}
	return true;
}


T3DObject* T3DObject::findobject(TObjectTreeItem *treeitem)
{
	if ((T3DObject*)this==treeitem) return this;
	return NULL;
}

TObjectTreeItem* T3DObject::G_parent()
{ 
	if (parentframe!=NULL) return parentframe;
	return scene;
}

T3DObject* T3DObject::MakeInstance()
{
	T3DObject *newobj=CreateInstance();
	newobj->init();
	return newobj;
}




//*********************************************************************
// T3DGeoObject
//*********************************************************************



T3DGeoObject::T3DGeoObject(T3DScene *iscene) : T3DObject(iscene)
{
//	append_classpath("Geometric objects");


	position=G_valuecontent<Tvertex>(&addparam(_qstr("Position"),SC_valname_vertex)->content);

	color=G_valuecontent<TSC_color>(&addparam(_qstr("Color"),SC_valname_color,false,_text(""),PARAMSUBCLASS_APPEARANCE)->content);
	speccolor=G_valuecontent<TSC_color>(&addparam(_qstr("SpecularColor"),SC_valname_color,false,_text(""),PARAMSUBCLASS_APPEARANCE)->content);speccolor->copyfrom(0,0,0,1);
	specvalue=addparam(_qstr("SpecularValue"),SC_valname_scalar,false,_text(""),PARAMSUBCLASS_APPEARANCE)->content.G_content_scalar();

	enablelight=addparam(_qstr("EnableLight"),SC_valname_boolean,false,_text(""),PARAMSUBCLASS_APPEARANCE)->content.G_content_boolean();

	addparam(_qstr("Texture"),SC_valname_string,false,_text(""),PARAMSUBCLASS_APPEARANCE);
	addparam(_qstr("TextureRight"),SC_valname_string,false,_text(""),PARAMSUBCLASS_APPEARANCE);

	renderfront=addparam(_qstr("RenderFront"),SC_valname_boolean,false,_text(""),_text("Optimization"))->content.G_content_boolean();renderfront->copyfrom(true);
	renderback=addparam(_qstr("RenderBack"),SC_valname_boolean,false,_text(""),_text("Optimization"))->content.G_content_boolean();renderback->copyfrom(false);
	renderoutline=addparam(_qstr("RenderOutLine"),SC_valname_boolean,false,_text(""),_text("Optimization"))->content.G_content_boolean();renderoutline->copyfrom(false);

	blendtype=&addparam(SC_valname_blendtype,SC_valname_blendtype,false,_text("Determines how the object blends with the background"),PARAMSUBCLASS_APPEARANCE)->content;
	depthmask=&addparam(SC_valname_depthmask,SC_valname_depthmask,false,_text(""),PARAMSUBCLASS_APPEARANCE)->content;
	depthtest=&addparam(SC_valname_depthtest,SC_valname_depthtest,false,_text(""),PARAMSUBCLASS_APPEARANCE)->content;

	isbackground=addparam(_qstr("IsBackGround"),SC_valname_boolean,false,_text(""),PARAMSUBCLASS_APPEARANCE)->content.G_content_boolean();isbackground->copyfrom(false);

	cancache=addparam(_qstr("CanCache"),SC_valname_boolean,false,_text("Wraps the rendering in a display list"),_text("Optimization"))->content.G_content_boolean();
	cancache->copyfrom(false);

	displaylist=NULL;
}

bool T3DGeoObject::G_param_optionlist(StrPtr paramname, Tarray<QString> &list)
{
/*	if (qstricmp(paramname,_qstr("Texture"))==0)
	{
		list.reset();
		list.add(new QString(_qstr("")));
		for (int i=0; i<cosmos->G_texturecount(); i++)
		{
			QString str;
			cosmos->G_texture(i)->G_name(str);
			list.add(new QString(str));
		}
		return true;
	}*/
	return T3DObject::G_param_optionlist(paramname,list);
}

void T3DGeoObject::precalcrender(Tprecalcrendercontext *pre)
{
	if (!G_isvisible()) return;
	ASSERT(parentframe!=NULL);
	rendercolor=*color;
	if (pre->parentsubframe!=NULL)
		rendercolor.mulwith(pre->parentsubframe->G_rendercolor());

	precalcrender_exec(pre);
}


void T3DGeoObject::render_drawobject(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording)
{
	if (G_cancache()&&(!isrecording)&&(displaylist!=NULL)&&(!G_needrecache()))
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
		render_exec(rc);
		if (displaylist!=NULL) displaylist->endrecord();
		isdirty=false;
	}
	rc->checkerror(_text("End render_drawobject"));
}


void T3DGeoObject::render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording)
{
	QString nm;
	G_name(nm);

	if (!G_isvisible(rc)) return;

	//transparency, blend type & depth mask logic
	int currentobjectblendtype=status->currentobjectblendtype;
	if (blendtype->toint()!=BlendNormal) currentobjectblendtype=blendtype->toint();
	int currentdepthmask=status->currentobjectdepthmask;
	if (depthmask->toint()!=DepthMaskNormal) currentdepthmask=depthmask->toint();

	bool rendernow=false;
	if (!isbackground->G_val())
	{
		if ((rc->currentrenderpass==Trendercontext::RP_SOLID)&&(currentobjectblendtype==BlendNormal)) rendernow=true;
		if ((rc->currentrenderpass==Trendercontext::RP_TRANSPARENT)&&(currentobjectblendtype!=BlendNormal)) rendernow=true;
	}
	else
	{
		if (rc->currentrenderpass==Trendercontext::RP_SOLID) rendernow=true;
	}
	if (!rendernow) return;

	bool currentenablelight=status->enablelight;
	if (!enablelight->G_val()) currentenablelight=false;
	rc->SetBlendType(currentobjectblendtype);
	
	rc->EnableDepthMask(currentdepthmask!=DepthMaskDisable);

	int currentdepthtest=status->currentobjectdepthtest;
	if (depthtest->toint()!=DepthTestNormal) currentdepthtest=depthtest->toint();
	rc->EnableDepthTest(currentdepthtest!=DepthTestDisable);



	ASSERT(parentframe!=NULL);
	ASSERT(scene!=NULL);

	if (scene==NULL) throw QError(_text("No scene attached to object"));
	StrPtr texturename=G_texturename();
	if (rc->G_isrightwindow())
		if (qstrlen(G_texturenameright())>0)
			texturename=G_texturenameright();
	T3DTexture *texture=NULL;
	if (qstrlen(texturename)>0)
	{
		texture=parentframe->G_texture(texturename);
		if (texture==NULL)
		{
			QString errstr;FormatString(errstr,_text("Could not find texture '^1'"),texturename);
			throw QError(errstr);
		}
		texture->select(rc);
	}

	rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	rc->set_speccolor(speccolor,specvalue->G_val());
	rc->set_orientation(+1);
	rc->enablelight(currentenablelight);

	Tvector transl;transl.copyfrom(*position);
	rc->create_subframe_translated(transl);

	if (renderoutline->G_val()) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	if ((currentobjectblendtype==BlendTranslucent)&&(renderfront->G_val())&&(renderback->G_val()))
	{//two-sided objects with true transparency need to be drawn in a special way
		glEnable(GL_CULL_FACE);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
		glCullFace(GL_FRONT);
		render_drawobject(rc,status,isrecording);
		glCullFace(GL_BACK);
		render_drawobject(rc,status,isrecording);
	}
	else
	{
		if ((renderfront->G_val())&&(renderback->G_val()))
		{
			glDisable(GL_CULL_FACE);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
		}
		if ((renderfront->G_val())&&(!renderback->G_val()))
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
		}
		if ((!renderfront->G_val())&&(renderback->G_val()))
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
		}
		if ((!renderfront->G_val())&&(!renderback->G_val())) return;
		render_drawobject(rc,status,isrecording);
	}

	if (renderoutline->G_val()) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	if (texture!=NULL) texture->unselect(rc);

	rc->del_subframe();

	isdirty=false;
}


void T3DGeoObject::calcshadowvolumes(Tshadowvolumecontext *svc, T3DSubFrameObject *parentsubframe)
{
	if (!G_isvisible()) return;
	ASSERT(parentsubframe!=NULL);
	while (shadowvolumes.G_count()<svc->G_shadowcount()) shadowvolumes.add(new Tshadowvolume);

	Taffinetransformation tf;
	Tvector transl;transl.copyfrom(*position);
	tf.translatefrom(parentsubframe->G_rendertransformation(),&transl);
	Tvertex locallightpos;
	tf.mulinv(&svc->lightpos,&locallightpos);
	double scalefactor=tf.G_scalefactor();

	for (int shadownr=0; shadownr<svc->G_shadowcount(); shadownr++)
		if (calcshadowvolume_exec(svc,shadowvolumes[shadownr],locallightpos,svc->G_lightradius(shadownr)/scalefactor,svc->G_depth1(shadownr),svc->G_depth2(shadownr)))
		{
			shadowvolumes[shadownr]->transformation=tf;
			svc->addshadowvolume(shadownr,shadowvolumes[shadownr]);
		}
}



bool T3DGeoObject::G_cancache()
{
	return cancache->G_val();
}



//*********************************************************************
// T3DGeoObjectPreCalcMesh
//*********************************************************************

T3DGeoObjectPreCalcMesh::T3DGeoObjectPreCalcMesh(T3DScene *iscene) : T3DGeoObject(iscene)
{
	iscalculated=false;
	canbuffer=addparam(_qstr("CanBuffer"),SC_valname_boolean,false,_text("Uses Vertex Buffer Objects (accelerates for complex objects)"),_text("Optimization"))->content.G_content_boolean();
	canbuffer->copyfrom(false);

	calcedges=addparam(_qstr("CalculateEdges"),SC_valname_boolean,false,_text("Determines whether or not the edges are calculated (needed for volume shadows)"),_text("Optimization"))->content.G_content_boolean();

}

void T3DGeoObjectPreCalcMesh::paramchanged(StrPtr iname)
{
	mesh.needcalcedges=calcedges->G_val();

	if (paramchanged_needrecalculate(iname)) iscalculated=false;
}


void T3DGeoObjectPreCalcMesh::streamout_content(QBinTagWriter &writer)
{
	if (!iscalculated) calculate();

	{
		QBinTagWriter &meshtag=writer.subtag(_qstr("Mesh")).G_obj();
		mesh.streamout(meshtag);
		meshtag.close();
	}
}

void T3DGeoObjectPreCalcMesh::streamin_content(QBinTagReader &reader)
{
	while (reader.hassubtags())
	{
		QBinTagReader &tg1=reader.getnextsubtag().G_obj();
		if (tg1.Istag(_qstr("Mesh")))
		{
			mesh.streamin(tg1);
		}
		tg1.close();
	}

	iscalculated=true;
}



void T3DGeoObjectPreCalcMesh::precalcrender_exec(Tprecalcrendercontext *pre)
{
	if (!iscalculated)
	{
		calculate();
		iscalculated=true;
	}
}


void T3DGeoObjectPreCalcMesh::render_exec(Trendercontext *rc)
{
	mesh.render(rc,rc->G_istextureactive(),G_canbuffer());
}

bool T3DGeoObjectPreCalcMesh::calcshadowvolume_exec(Tshadowvolumecontext *svc, Tshadowvolume *shadowvolume, Tvertex &locallightpos, double lightradius, double depth1, double depth2)
{
	shadowvolume->reset();
	mesh.calcshadowvolume(shadowvolume,locallightpos,depth1,depth2);
	return true;
}



//*********************************************************************
// T3DObjectSphere
//*********************************************************************

T3DObjectSphere::T3DObjectSphere(T3DScene *iscene) : T3DGeoObjectPreCalcMesh(iscene)
{
	addparam(_qstr("Radius"),SC_valname_scalar);param_setvalue(_qstr("Radius"),1.0);
	addparam(_qstr("Resolution"),SC_valname_scalar);param_setvalue(_qstr("Resolution"),20);

	addparam(_qstr("Angle1Min"),SC_valname_scalar);param_setvalue(_qstr("Angle1Min"),0.0);
	addparam(_qstr("Angle1Max"),SC_valname_scalar);param_setvalue(_qstr("Angle1Max"),2*Pi);

	addparam(_qstr("Angle2Min"),SC_valname_scalar);param_setvalue(_qstr("Angle2Min"),0.0);
	addparam(_qstr("Angle2Max"),SC_valname_scalar);param_setvalue(_qstr("Angle2Max"),Pi);

	addparam(_qstr("Flattening"),SC_valname_scalar);param_setvalue(_qstr("Flattening"),1.0);

}


bool T3DObjectSphere::paramchanged_needrecalculate(StrPtr paramname)
{
	if (qstricmp(paramname,_qstr("Radius"))==0) return true;
	if (qstricmp(paramname,_qstr("Resolution"))==0) return true;
	return false;
}

void T3DObjectSphere::calculate()
{
	int i,j;
	double al1,al2,xa,ya,za;
	double cs1,sn1,cs2,sn2;

	int rr=G_paramint(_qstr("Resolution"));
	double rad=G_paramdouble(_qstr("Radius"));

	double angle1min=G_paramdouble(_qstr("Angle1Min"));
	double angle1max=G_paramdouble(_qstr("Angle1Max"));
	double angle2min=G_paramdouble(_qstr("Angle2Min"));
	double angle2max=G_paramdouble(_qstr("Angle2Max"));

	double flattening=G_paramdouble(_qstr("Flattening"));

	mesh.reset();


	Tvertex pt;
	Tvector nm;
	int ct=0;
	for (i=0; i<2*rr; i++)
	{
		al1=angle1min+(i*1.0)/(2*rr-1)*(angle1max-angle1min);
		cs1=cos(al1);sn1=sin(al1);
		for (j=0; j<=rr; j++)
		{
			al2=angle2min+(j*1.0)/rr*(angle2max-angle2min);
			cs2=cos(al2);sn2=sin(al2);
			xa=sn2*cs1;
			ya=sn2*sn1;
			za=cs2;
			pt.S3_x(xa*rad);pt.S3_y(ya*rad);pt.S3_z(flattening*za*rad);
			nm.S_x(xa);nm.S_y(ya);nm.S_z(za/flattening);
			nm.normfrom(&nm);
			mesh.addvertex(&pt,ct,&nm,i*1.0/(2*rr-1),1-j*1.0/rr);
			ct++;
		}
	}
	mesh.Make_stripbased(rr+1,true);
}

bool T3DObjectSphere::calcshadowvolume_exec(Tshadowvolumecontext *svc, Tshadowvolume *shadowvolume, Tvertex &locallightpos, double lightradius, double depth1, double depth2)
{
	int rr=G_paramint(_qstr("Resolution"));
	double rad=G_paramdouble(_qstr("Radius"));

	shadowvolume->calcsphere(rad*1.004,2*rr,locallightpos,lightradius,depth1,depth2);
	return true;
}


//*********************************************************************
// T3DObjectPoint
//*********************************************************************

T3DObjectPoint::T3DObjectPoint(T3DScene *iscene) : T3DGeoObject(iscene)
{
	enablelight->copyfrom(false);
	size=addparam(_qstr("Size"),SC_valname_scalar)->content.G_content_scalar();
	size->copyfrom(1);
}

void T3DObjectPoint::precalcrender_exec(Tprecalcrendercontext *pre)
{
}

void T3DObjectPoint::render_exec(Trendercontext *rc)
{
	if (qstrlen(G_texturename())>0)
	{
		glDisable(GL_POINT_SMOOTH);
		glEnable(GL_POINT_SPRITE);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	}
	else
	{
		glEnable(GL_POINT_SMOOTH);
		glDisable(GL_POINT_SPRITE);
	}
	glPointSize((GLfloat)size->G_val());
	glBegin(GL_POINTS);
//	glVertex3d(position->G3_x(),position->G3_y(),position->G3_z());
	glVertex3d(0,0,0);
	glEnd();
}


//*********************************************************************
// T3DObject3DText
//*********************************************************************

T3DObject3DText::T3DObject3DText(T3DScene *iscene) : T3DGeoObject(iscene)
{
	content=addparam(_qstr("Content"),SC_valname_string)->content.G_content_string();
	depth=addparam(_qstr("Depth"),SC_valname_scalar)->content.G_content_scalar();
	unitdirx=G_valuecontent<Tvector>(&addparam(_qstr("UnitX"),SC_valname_vector)->content);
	unitdiry=G_valuecontent<Tvector>(&addparam(_qstr("UnitY"),SC_valname_vector)->content);
	*unitdirx=Tvector(1,0,0);
	*unitdiry=Tvector(0,1,0);
	cancache->copyfrom(false);
}

void T3DObject3DText::precalcrender_exec(Tprecalcrendercontext *pre)
{
}

void T3DObject3DText::render_exec(Trendercontext *rc)
{
	Tvertex zero(0,0,0);
	rc->rendertext(DEFAULT_FONT,&zero,unitdirx,unitdiry,content->G_string(),depth->G_val());
}

double T3DObject3DText::G_textsizex()
{
	return scene->GetMyFirstRenderWindow()->G_font(DEFAULT_FONT)->G_textsizex(content->G_string());
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_3dtext_sizex,GetSizeX)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_text3d);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObject3DText *txt=G_valuecontent<T3DObject3DText>(owner);
	retval->G_content_scalar()->copyfrom(txt->G_textsizex());
}
ENDFUNCTION(func_3dtext_sizex)




//*********************************************************************
// T3DObjectFlatText
//*********************************************************************

T3DObjectFlatText::T3DObjectFlatText(T3DScene *iscene) : T3DGeoObject(iscene)
{
	content=addparam(_qstr("Content"),SC_valname_string)->content.G_content_string();
	size=addparam(_qstr("Size"),SC_valname_scalar)->content.G_content_scalar();
	cancache->copyfrom(false);
}

void T3DObjectFlatText::precalcrender_exec(Tprecalcrendercontext *pre)
{
}

void T3DObjectFlatText::render_exec(Trendercontext *rc)
{
	ASSERT(parentframe!=NULL);

	rc->enablelight(false);
	//Tvertex origin;
	//parentframe->G_rendertransformation()->mul(position,&origin);
	//rc->create_subframe_viewdir(position);
	rc->create_subframe_viewdir(&Tvertex(0,0,0));//--!!-- dit begrijp ik niet helemaal. Wat gebeurt met position?

	Tvertex zero(0,0,0);
	Tvector unitdirx(size->G_val(),0,0),unitdiry(0,size->G_val(),0);
	rc->rendertext(DEFAULT_FONT,&zero,&unitdirx,&unitdiry,content->G_string(),0);

	rc->del_subframe();

}



//*********************************************************************
// T3DObjectBar
//*********************************************************************

T3DObjectBar::T3DObjectBar(T3DScene *iscene) : T3DGeoObjectPreCalcMesh(iscene)
{
	addparam(_qstr("SizeX"),SC_valname_scalar);
	addparam(_qstr("SizeY"),SC_valname_scalar);
	addparam(_qstr("SizeZ"),SC_valname_scalar);
	param_setvalue(_qstr("SizeX"),1.0);
	param_setvalue(_qstr("SizeY"),1.1);
	param_setvalue(_qstr("SizeZ"),1.23);
}

bool T3DObjectBar::paramchanged_needrecalculate(StrPtr paramname)
{
	if (qstricmp(paramname,_qstr("SizeX"))==0) return true;
	if (qstricmp(paramname,_qstr("SizeY"))==0) return true;
	if (qstricmp(paramname,_qstr("SizeZ"))==0) return true;
	return false;
}

void T3DObjectBar::calculate()
{
	int i,j;
	double al1,al2,xa,ya,za;
	double cs1,sn1,cs2,sn2;

	double sizex=G_paramdouble(_qstr("SizeX"));
	double sizey=G_paramdouble(_qstr("SizeY"));
	double sizez=G_paramdouble(_qstr("SizeZ"));

	mesh.reset();
	int i1=mesh.addvertex(&Tvertex(0,0,0),				0,NULL,0,0);
	int i2=mesh.addvertex(&Tvertex(sizex,0,0),			1,NULL,1,0);
	int i3=mesh.addvertex(&Tvertex(sizex,sizey,0),		2,NULL,1,1);
	int i4=mesh.addvertex(&Tvertex(0,sizey,0),			3,NULL,0,1);
	int i5=mesh.addvertex(&Tvertex(0,0,sizez),			4,NULL,0,0);
	int i6=mesh.addvertex(&Tvertex(sizex,0,sizez),		5,NULL,1,0);
	int i7=mesh.addvertex(&Tvertex(sizex,sizey,sizez),	6,NULL,1,1);
	int i8=mesh.addvertex(&Tvertex(0,sizey,sizez),		7,NULL,0,1);

	mesh.addtriangle(i3,i2,i1);mesh.addtriangle(i1,i4,i3);

	mesh.addtriangle(i7,i8,i5);mesh.addtriangle(i5,i6,i7);

	mesh.addtriangle(i1,i2,i6);mesh.addtriangle(i6,i5,i1);

	mesh.addtriangle(i2,i3,i7);mesh.addtriangle(i7,i6,i2);

	mesh.addtriangle(i3,i4,i8);mesh.addtriangle(i8,i7,i3);

	mesh.addtriangle(i4,i1,i5);mesh.addtriangle(i5,i8,i4);

	mesh.calcnormals();
	mesh.calcedges();
}



//*********************************************************************
// T3DObjectCylinder
//*********************************************************************

T3DObjectCylinder::T3DObjectCylinder(T3DScene *iscene) : T3DGeoObjectPreCalcMesh(iscene)
{
	radius=addparam(_qstr("Radius"),SC_valname_scalar)->content.G_content_scalar();
	radiuschange=addparam(_qstr("RadiusChange"),SC_valname_scalar)->content.G_content_scalar();
	axis=G_valuecontent<Tvector>(&addparam(_qstr("Axis"),SC_valname_vector)->content);
	resolution=addparam(_qstr("Resolution"),SC_valname_scalar)->content.G_content_scalar();
	isclosed=addparam(_qstr("IsClosed"),SC_valname_boolean)->content.G_content_boolean();
	radius->copyfrom(1);radiuschange->copyfrom(1);
	resolution->copyfrom(20);
	axis->copyfrom(1,0,0);
}

bool T3DObjectCylinder::paramchanged_needrecalculate(StrPtr paramname)
{
	if (qstricmp(paramname,_qstr("Radius"))==0) return true;
	if (qstricmp(paramname,_qstr("RadiusChange"))==0) return true;
	if (qstricmp(paramname,_qstr("Axis"))==0) return true;
	if (qstricmp(paramname,_qstr("Resolution"))==0) return true;
	if (qstricmp(paramname,_qstr("IsClosed"))==0) return true;
	return false;
}

void T3DObjectCylinder::calculate()
{
	int i;
	double ang;
	int rr=resolution->G_intval();
	double radius1=radius->G_val();
	double radius2=radius1*radiuschange->G_val();
	Tvector n1,n2,dd,axdirmin,axdirpos;
	Tvertex ps,end1,end2;
	end2.copyfrom(*axis);
	n1.anynormalfrom(axis);
	n2.vecprod(&n1,axis);n2.normfrom(&n2);
	mesh.reset();
	for (i=0; i<=rr; i++)
	{
		ang=i*2*Pi/rr;
		dd.lincombfrom(cos(ang),&n1,sin(ang),&n2);
		ps.lincombfrom(1,&end1,radius1,&dd);
		mesh.addvertex(&ps,2*(i%rr)+0,&dd,ang/(2*Pi),0.003);
		ps.lincombfrom(1,&end2,radius2,&dd);
		mesh.addvertex(&ps,2*(i%rr)+1,&dd,ang/(2*Pi),1-0.003);
	}
	for (i=0; i<rr; i++) mesh.addquad(2*i+0,2*i+1,2*i+3,2*i+2);

	if (isclosed->G_val())
	{
		axdirpos.normfrom(axis);
		axdirmin.mulfrom(&axdirpos,-1);
		for (i=0; i<=rr; i++)
		{
			ang=i*2*Pi/rr;
			dd.lincombfrom(cos(ang),&n1,sin(ang),&n2);
			ps.lincombfrom(1,&end1,radius1,&dd);
			mesh.addvertex(&ps,2*(i%rr)+0,&axdirmin,0,ang/(2*Pi));
			ps.lincombfrom(1,&end2,radius2,&dd);
			mesh.addvertex(&ps,2*(i%rr)+1,&axdirpos,1,ang/(2*Pi));
		}
		int centeridx1=mesh.addvertex(&end1,-1,&axdirmin,0,0);
		int centeridx2=mesh.addvertex(&end2,-2,&axdirpos,0,0);
		for (i=0; i<rr; i++)
		{
//			mesh.addtriangle(2*(rr+1)+2*i+0,centeridx1,2*(rr+1)+2*i+2);
//			mesh.addtriangle(centeridx2,2*(rr+1)+2*i+1,2*(rr+1)+2*i+3);
			mesh.addtriangle(2*(rr+1)+2*i+0,2*(rr+1)+2*i+2,centeridx1);
			mesh.addtriangle(centeridx2,2*(rr+1)+2*i+3,2*(rr+1)+2*i+1);
		}
	}

	mesh.calcnormals();
	mesh.calcedges();
}




//*********************************************************************
// T3DObjectPie
//*********************************************************************

T3DObjectPie::T3DObjectPie(T3DScene *iscene) : T3DGeoObjectPreCalcMesh(iscene)
{

	radius1=addparam(_qstr("Radius1"),SC_valname_scalar)->content.G_content_scalar();
	radius2=addparam(_qstr("Radius2"),SC_valname_scalar)->content.G_content_scalar();
	height=addparam(_qstr("Height"),SC_valname_scalar)->content.G_content_scalar();
	axis1=G_valuecontent<Tvector>(&addparam(_qstr("Axis1"),SC_valname_vector)->content);
	axis2=G_valuecontent<Tvector>(&addparam(_qstr("Axis2"),SC_valname_vector)->content);
	otherdir=addparam(_qstr("InvertDir"),SC_valname_boolean)->content.G_content_boolean();
	otherdir->copyfrom(false);
	resolution=addparam(_qstr("Resolution"),SC_valname_scalar)->content.G_content_scalar();
	radius1->copyfrom(0.5);radius2->copyfrom(1.0);
	resolution->copyfrom(20);
	axis1->copyfrom(1,0,0);
	axis2->copyfrom(0,1,0);
	height->copyfrom(1.0);
}

bool T3DObjectPie::paramchanged_needrecalculate(StrPtr paramname)
{
	if (qstricmp(paramname,_qstr("Radius1"))==0) return true;
	if (qstricmp(paramname,_qstr("Radius2"))==0) return true;
	if (qstricmp(paramname,_qstr("Height"))==0) return true;
	if (qstricmp(paramname,_qstr("Axis1"))==0) return true;
	if (qstricmp(paramname,_qstr("Axis2"))==0) return true;
	if (qstricmp(paramname,_qstr("Resolution"))==0) return true;
	if (qstricmp(paramname,_qstr("InvertDir"))==0) return true;
	return false;
}

void T3DObjectPie::calculate()
{
	int i;
	int rr=resolution->G_intval();
	double r1=radius1->G_val();
	double r2=radius2->G_val();
	double h=height->G_val();


	double ang=Tvector::angle(axis1,axis2);
	Tvector n0,n0neg,n,dd;
	n0.vecprod(axis1,axis2);
	n.vecprod(&n0,axis1);n.normfrom(&n);

	n0neg.mulfrom(&n0,-1);

	Tvertex end1,end2,ps;
	end2.lincombfrom(1.0,&end1,h,&n0);

	if (otherdir->G_val())
	{
		ang=2*Pi-ang;
		n.mul(-1);
	}

	mesh.reset();
	for (int i=0; i<rr; i++)
	{
		double fr=i*1.0/(rr-1);
		double a=ang*fr;
		dd.lincombfrom(cos(a),axis1,sin(a),&n);
		ps.lincombfrom(1.0,&end1,r1,&dd);mesh.addvertex(&ps,i+0*rr,&n0neg,fr,0);
		ps.lincombfrom(1.0,&end1,r2,&dd);mesh.addvertex(&ps,i+1*rr,&n0neg,fr,1);
		ps.lincombfrom(1.0,&end2,r1,&dd);mesh.addvertex(&ps,i+2*rr,&n0,fr,0);
		ps.lincombfrom(1.0,&end2,r2,&dd);mesh.addvertex(&ps,i+3*rr,&n0,fr,1);
	}

	for (i=0; i<rr-1; i++) mesh.addquad((i+0)*4+0,(i+1)*4+0,(i+1)*4+1,(i+0)*4+1);
	for (i=0; i<rr-1; i++) mesh.addquad((i+0)*4+2,(i+0)*4+3,(i+1)*4+3,(i+1)*4+2);

	//--!!-- todo: this only calculates part of a fully closed pie: the side walls are missing!

	mesh.calcnormals();
	mesh.calcedges();
}


//*********************************************************************
// T3DObjectRectangle
//*********************************************************************

T3DObjectRectangle::T3DObjectRectangle(T3DScene *iscene) : T3DGeoObjectPreCalcMesh(iscene)
{
	axis1=G_valuecontent<Tvector>(&addparam(_qstr("Axis1"),SC_valname_vector)->content);
	axis2=G_valuecontent<Tvector>(&addparam(_qstr("Axis2"),SC_valname_vector)->content);
	axis1->copyfrom(1,0,0);
	axis2->copyfrom(0,1,0);
}

bool T3DObjectRectangle::paramchanged_needrecalculate(StrPtr paramname)
{
	if (qstricmp(paramname,_qstr("Axis1"))==0) return true;
	if (qstricmp(paramname,_qstr("Axis2"))==0) return true;
	return false;
}

void T3DObjectRectangle::calculate()
{
	Tvertex zero,pos00,pos10,pos11,pos01;
	Tvector normal;

	pos10.add(&zero,axis1);
	pos01.add(&zero,axis2);
	pos11.add(&pos10,axis2);

	normal.vecprod(axis1,axis2);normal.normfrom(&normal);

	double txlo=0.015;
	double txhi=0.985;
	mesh.reset();
	mesh.addvertex(&pos00,0,&normal,txlo,txlo);
	mesh.addvertex(&pos10,1,&normal,txhi,txlo);
	mesh.addvertex(&pos11,2,&normal,txhi,txhi);
	mesh.addvertex(&pos01,3,&normal,txlo,txhi);
	mesh.addquad(0,1,2,3);
	mesh.calcnormals();
	mesh.calcedges();
}




//*********************************************************************
// T3DObjectSolidArrow
//*********************************************************************

T3DObjectSolidArrow::T3DObjectSolidArrow(T3DScene *iscene) : T3DGeoObjectPreCalcMesh(iscene)
{
	axis1=G_valuecontent<Tvector>(&addparam(_qstr("Axis"),SC_valname_vector)->content);
	axis2=G_valuecontent<Tvector>(&addparam(_qstr("NormAxis"),SC_valname_vector)->content);
	axis1->copyfrom(1,0,0);
	axis2->copyfrom(0,1,0);

	width=addparam(_qstr("Width"),SC_valname_scalar)->content.G_content_scalar();
	thick=addparam(_qstr("Thickness"),SC_valname_scalar)->content.G_content_scalar();

}

bool T3DObjectSolidArrow::paramchanged_needrecalculate(StrPtr paramname)
{
	if (qstricmp(paramname,_qstr("Axis"))==0) return true;
	if (qstricmp(paramname,_qstr("NormAxis"))==0) return true;
	if (qstricmp(paramname,_qstr("Width"))==0) return true;
	if (qstricmp(paramname,_qstr("Thickness"))==0) return true;
	return false;
}

void T3DObjectSolidArrow::calculate()
{
	mesh.reset();
	Tvec3d pos1,pos2,normdir;

	Tvertex pst;

	Tvertex ps2;
	ps2.lincombfrom(1,&pst,1,axis1);

	pos1=pst;
	pos2=ps2;
	normdir=*axis2;
	double w=width->G_val();
	double h=thick->G_val();

	Tvec3d dir1,dir2,v;
	dir1.lincomb(pos2,pos1,1,-1);dir1.norm();
	dir2.vecprod(normdir,dir1);dir2.norm();
	normdir.vecprod(dir1,dir2);normdir.norm();

	v.lincomb(pos1,dir2,normdir,1,+w/2,+h/2);mesh.addvertex(&v,0);
	v.lincomb(pos2,dir1,dir2,normdir,1,-w,+w/2,+h/2);mesh.addvertex(&v,1);
	v.lincomb(pos2,dir1,dir2,normdir,1,-w,+w,+h/2);mesh.addvertex(&v,2);
	v.lincomb(pos2,normdir,1,+h/2);mesh.addvertex(&v,3);
	v.lincomb(pos2,dir1,dir2,normdir,1,-w,-w,+h/2);mesh.addvertex(&v,4);
	v.lincomb(pos2,dir1,dir2,normdir,1,-w,-w/2,+h/2);mesh.addvertex(&v,5);
	v.lincomb(pos1,dir2,normdir,1,-w/2,+h/2);mesh.addvertex(&v,6);

	v.lincomb(pos1,dir2,normdir,1,+w/2,-h/2);mesh.addvertex(&v,7);
	v.lincomb(pos2,dir1,dir2,normdir,1,-w,+w/2,-h/2);mesh.addvertex(&v,8);
	v.lincomb(pos2,dir1,dir2,normdir,1,-w,+w,-h/2);mesh.addvertex(&v,9);
	v.lincomb(pos2,normdir,1,-h/2);mesh.addvertex(&v,10);
	v.lincomb(pos2,dir1,dir2,normdir,1,-w,-w,-h/2);mesh.addvertex(&v,11);
	v.lincomb(pos2,dir1,dir2,normdir,1,-w,-w/2,-h/2);mesh.addvertex(&v,12);
	v.lincomb(pos1,dir2,normdir,1,-w/2,-h/2);mesh.addvertex(&v,13);

	mesh.addtriangle(0,6,1);
	mesh.addtriangle(1,6,5);
	mesh.addtriangle(2,4,3);

	mesh.addtriangle(7,8,13);
	mesh.addtriangle(8,12,13);
	mesh.addtriangle(9,10,11);

	mesh.addtriangle(0,1,8);
	mesh.addtriangle(8,7,0);

	mesh.addtriangle(0,7,13);
	mesh.addtriangle(13,6,0);


	mesh.addtriangle(1,2,9);
	mesh.addtriangle(9,8,1);

	mesh.addtriangle(2,3,10);
	mesh.addtriangle(10,9,2);

	mesh.addtriangle(6,13,12);
	mesh.addtriangle(12,5,6);

	mesh.addtriangle(5,12,11);
	mesh.addtriangle(11,4,5);

	mesh.addtriangle(4,10,3);
	mesh.addtriangle(10,4,11);

	mesh.calcnormals();
	mesh.calcedges();
}




///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_addbar,addbar)
{
	setreturntype(SC_valname_bar);
	setmemberfunction(SC_valname_subframe);
	extendclasspath(SC_subpart_createshapes);
	addvar(_qstr("position"),SC_valname_vertex);
	addvar(_qstr("SizeX"),SC_valname_scalar);
	addvar(_qstr("SizeY"),SC_valname_scalar);
	addvar(_qstr("SizeZ"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);

	T3DObjectBar *bar=new T3DObjectBar(subframe->G_scene());
	bar->init();
	subframe->addsubobject(bar);
	bar->G_param(_qstr("Position"))->content.copyfrom(arglist->get(0));
	bar->G_param(_qstr("SizeX"))->content.copyfrom(arglist->get(1));
	bar->G_param(_qstr("SizeY"))->content.copyfrom(arglist->get(2));
	bar->G_param(_qstr("SizeZ"))->content.copyfrom(arglist->get(3));

	retval->encapsulate(bar);
}
ENDFUNCTION(func_addbar)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_addflattext,addflattext)
{
	setreturntype(SC_valname_textflat);
	setmemberfunction(SC_valname_subframe);
	extendclasspath(SC_subpart_createshapes);
	addvar(_qstr("Content"),SC_valname_string);
	addvar(_qstr("Position"),SC_valname_vertex);
	addvar(_qstr("Size"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);

	T3DObjectFlatText *flattext=new T3DObjectFlatText(subframe->G_scene());
	subframe->addsubobject(flattext);
	flattext->G_content()->fromstring(arglist->get(0)->G_content_string()->G_string());
	*flattext->G_position()=*G_valuecontent<Tvertex>(arglist->get(1));
	flattext->G_size()->copyfrom(arglist->get(2)->G_content_scalar()->G_val());
	retval->encapsulate(flattext);
}
ENDFUNCTION(func_addflattext)
