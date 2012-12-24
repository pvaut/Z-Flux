#include "stdafx.h"

#include "vecmath.h"
#include "vec_transformation.h"

#include "3DScene.h"
#include "3DSpecialObjects.h"




//******************************************************************
// T3DObjectLightPoint
//******************************************************************

T3DObjectLightPoint::T3DObjectLightPoint(T3DScene *iscene) : T3DObject(iscene)
{
	color=G_valuecontent<TSC_color>(&addparam(_qstr("Color"),SC_valname_color)->content);
	position=G_valuecontent<Tvertex>(&addparam(_qstr("Position"),SC_valname_vertex)->content);
	size=addparam(_qstr("Size"),SC_valname_scalar)->content.G_content_scalar();
}

void T3DObjectLightPoint::render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording)
{
	if (!G_isvisible(rc)) return;
//	if (rc->currentrenderpass==Trendercontext::RP_SOLID)
	{
		Tvertex worldposition;
		parentframe->G_rendertransformation()->mul(position,&worldposition);
//		rc->G_viewport()->G_transformation()->mul(&worldposition,&worldposition2);
		rc->addlightpoint(&worldposition,color,G_size());
	}
}


//******************************************************************
// T3DRepeatedObject
//******************************************************************

T3DRepeatedObject::T3DRepeatedObject(T3DScene *iscene) : T3DObject(iscene)
{
	position=G_valuecontent<Tvertex>(&addparam(_qstr("Position"),SC_valname_vertex)->content);
}

void T3DRepeatedObject::Setref(T3DSubFrameObject *object)
{
	subframeref.setreffor(object);
}


void T3DRepeatedObject::render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording)
{
	if (!G_isvisible(rc)) return;
	ASSERT(!isrecording);
	if (isrecording) throw QError(_text("Cached objects cannot be rendered in recording modus"));
	Tvector shift;shift.copyfrom(*position);
	rc->create_subframe_translated(shift);
	ASSERT(G_scene()!=NULL);
	if (G_scene()==NULL) throw QError(_text("Cached objects should be member of a scene"));
	T3DSubFrameObject *repeatedsubframe=subframeref.G_ref();
	if (repeatedsubframe==NULL)
		throw QError(_text("Could not find subframe for caching object"));
	repeatedsubframe->render(rc,status,false);
	rc->del_subframe();
}

//******************************************************************
// T3DObjectFog
//******************************************************************

T3DObjectFog::T3DObjectFog(T3DScene *iscene) : T3DObject(iscene)
{
	fogtype=&addparam(_qstr("Type"),SC_valname_FogType)->content;
	density=addparam(_qstr("Density"),SC_valname_scalar,false,_text("Used for exponential fog"))->content.G_content_scalar();
	start=addparam(_qstr("Start"),SC_valname_scalar,false,_text("Used for linear fog"))->content.G_content_scalar();
	end=addparam(_qstr("End"),SC_valname_scalar,false,_text("Used for linear fog"))->content.G_content_scalar();
	color=G_valuecontent<TSC_color>(&addparam(_qstr("Color"),SC_valname_color)->content);
}

void T3DObjectFog::render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording)
{
	if (!G_isvisible(rc)) return;
	int tpe=fogtype->toint();
	if (tpe==FOG_NONE)
	{
		glDisable(GL_FOG);
		return;
	}
	glEnable(GL_FOG);
	if (tpe==FOG_EXP)
	{
		glFogi(GL_FOG_MODE,GL_EXP);
		glFogf(GL_FOG_DENSITY,density->G_val());
	}
	if (tpe==FOG_EXP2)
	{
		glFogi(GL_FOG_MODE,GL_EXP2);
		glFogf(GL_FOG_DENSITY,density->G_val());
	}
	if (tpe==FOG_LINEAR)
	{
		glFogi(GL_FOG_MODE,GL_LINEAR);
		glFogf(GL_FOG_START,start->G_val());
		glFogf(GL_FOG_END,end->G_val());
	}

	GLfloat fogcol[]= { (GLfloat)color->G_R(), (GLfloat)color->G_G(), (GLfloat)color->G_B(), (GLfloat)color->G_A() };
	glFogfv(GL_FOG_COLOR,fogcol);

}






///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_addrepeatedobject,addrepeatedobject)
{
	setreturntype(SC_valname_repeatedobject);
	setmemberfunction(SC_valname_subframe);
	extendclasspath(SC_subpart_createshapes);
	addvar(_qstr("cachedsubframe"),SC_valname_subframe);
	addvar(_qstr("position"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe0=G_valuecontent<T3DSubFrameObject>(owner);
	T3DRepeatedObject *newobject=new T3DRepeatedObject(subframe0->G_scene());

	T3DSubFrameObject *refsubframe=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	newobject->Setref(refsubframe);

	*newobject->G_position()=*G_valuecontent<Tvertex>(arglist->get(1));

	subframe0->addsubobject(newobject);
	retval->settype(newobject->G_datatype());
	retval->encapsulate(newobject);
}
ENDFUNCTION(func_object_addrepeatedobject)




