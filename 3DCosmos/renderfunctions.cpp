#include "stdafx.h"

#include "vecmath.h"
#include "vec_transformation.h"

#include "displaydevices.h"

#include "3DScene.h"
#include "3Dsurface.h"

#define rendergroup _qstr("Rendering")



/*
void threadtest()
{
	::MessageBox(NULL,_qstr("1"),_qstr(""),MB_OK);
	for (int i=0; i<5000; i++)
	{
		Tthread thr1,thr2;
		thr1.start();thr2.start();
		thr1.waitforfinished();
		thr2.waitforfinished();
	}
	::MessageBox(NULL,_qstr("2"),_qstr(""),MB_OK);
	int i=0;
}
*/


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_render_resetall,resetall)
{
	setclasspath(rendergroup);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_3DCosmos().resetall();
	G_3DCosmos().initvalues();
	TDisplayAdapterlist::Get().load();
}
ENDFUNCTION(func_render_resetall)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_render_resetallscenes,resetallscenes)
{
	setclasspath(rendergroup);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_3DCosmos().resetallscenes();
}
ENDFUNCTION(func_render_resetallscenes)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_render_resetallviewports,resetallviewports)
{
	setclasspath(rendergroup);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_3DCosmos().resetallviewports();
}
ENDFUNCTION(func_render_resetallviewports)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_render_resetallvideos,resetallvideos)
{
	setclasspath(rendergroup);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_3DCosmos().resetallvideos();
}
ENDFUNCTION(func_render_resetallvideos)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_render_resetallsounds,resetallsounds)
{
	setclasspath(rendergroup);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_3DCosmos().resetallsounds();
}
ENDFUNCTION(func_render_resetallsounds)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_render_incrtime,incrtime)
{
	setreturntype(SC_valname_scalar);
	setclasspath(rendergroup);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_scalar()->copyfrom(G_3DCosmos().incrtime());
}
ENDFUNCTION(func_render_incrtime)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_render_render,render)
{
	setclasspath(rendergroup);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
//	threadtest();
	G_3DCosmos().render(false);
}
ENDFUNCTION(func_render_render)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_render_renderskip,renderskip)
{
	setclasspath(rendergroup);
	addvar(_qstr("doskip"),SC_valname_boolean);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	bool doskip=arglist->get(0)->G_content_boolean()->G_val();
	G_3DCosmos().render(doskip);
}
ENDFUNCTION(func_render_renderskip)

