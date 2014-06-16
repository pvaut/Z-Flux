#include "stdafx.h"

#include "vecmath.h"
#include "vec_transformation.h"

#include "sc_script.h"

#include "3DScene.h"
//#include "3DUI.h"
#include "3DScene.h"

#include "opengl.h"

#define PATH_UI (_text("Input / Output;User Interface"))
#define PATH_NAVIGATION (_text("Input / Output;Navigation"))
#define PATH_JOYSTICK _text("Input / Output;User Interface;Joystick")


bool IsKeyDown(int virtkey);


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_UI_getaxisposition,UIGetAxisPos)
{
	addvar(_qstr("Axis"),SC_valname_UIAxisType);
	addvar(_qstr("Level"),SC_valname_UIAxisLevel);
	setreturntype(SC_valname_scalar);
	setclasspath(PATH_UI);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	int axisnr=arglist->get(0)->toint();
	int axislevel=arglist->get(1)->toint();
	retval->G_content_scalar()->copyfrom(G_3DCosmos().G_axisposit(axisnr,axislevel));
}
ENDFUNCTION(func_UI_getaxisposition)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_UI_getaxisactive,UIGetAxisActive)
{
	addvar(_qstr("Axis"),SC_valname_UIAxisType);
	addvar(_qstr("Level"),SC_valname_UIAxisLevel);
	setreturntype(SC_valname_boolean);
	setclasspath(PATH_UI);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	int axisnr=arglist->get(0)->toint();
	int axislevel=arglist->get(1)->toint();
	retval->G_content_boolean()->copyfrom(G_3DCosmos().G_axisactive(axisnr,axislevel));
}
ENDFUNCTION(func_UI_getaxisactive)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_UI_isleftmousebuttondown,UIIsLeftMouseButtonDown)
{
	setreturntype(SC_valname_boolean);
	setclasspath(PATH_UI);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_boolean()->copyfrom(T3DCosmos::Get().G_scene(0)->GetMyFirstRenderWindow()->G_mouse_leftbuttondown());
}
ENDFUNCTION(func_UI_isleftmousebuttondown)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_UI_isrightmousebuttondown,UIIsRightMouseButtonDown)
{
	setreturntype(SC_valname_boolean);
	setclasspath(PATH_UI);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_boolean()->copyfrom(T3DCosmos::Get().G_scene(0)->GetMyFirstRenderWindow()->G_mouse_rightbuttondown());
}
ENDFUNCTION(func_UI_isrightmousebuttondown)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_UI_ismiddlemousebuttondown,UIIsMiddleMouseButtonDown)
{
	setreturntype(SC_valname_boolean);
	setclasspath(PATH_UI);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_boolean()->copyfrom(T3DCosmos::Get().G_scene(0)->GetMyFirstRenderWindow()->G_mouse_middlebuttondown());
}
ENDFUNCTION(func_UI_ismiddlemousebuttondown)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_UI_iskeydown,UIIsKeyDown)
{
	addvar(_qstr("key"),SC_valname_string);
	setreturntype(SC_valname_boolean);
	setclasspath(PATH_UI);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr keyname=arglist->get(0)->G_content_string()->G_string();
	if (issame(keyname,_qstr("CONTROL")))
	{
		retval->G_content_boolean()->copyfrom(((GetKeyState(VK_CONTROL)>>1)!=0));
		return;
	}
	if (issame(keyname,_qstr("SHIFT")))
	{
		retval->G_content_boolean()->copyfrom(((GetKeyState(VK_SHIFT)>>1)!=0));
		return;
	}
	int keycode=G_3DCosmos().G_keyboardcode(keyname);
	if (keycode<0) throw QError(_text("Invalid key code"));
	retval->G_content_boolean()->copyfrom(IsKeyDown(keycode));
}
ENDFUNCTION(func_UI_iskeydown)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_UI_getkeypressed,UIGetKeyPressed)
{
	setreturntype(SC_valname_string);
	setclasspath(PATH_UI);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_string()->fromstring(G_3DCosmos().G_lastkeypressed());
}
ENDFUNCTION(func_UI_getkeypressed)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_navigation_currentmove,NavigationMove)
{
	setclasspath(PATH_NAVIGATION);
	setreturntype(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	*G_valuecontent<Tvector>(retval)=G_3DCosmos().G_selviewport()->currentmove.cammove;
}
ENDFUNCTION(func_navigation_currentmove)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_navigation_currentrotate,NavigationRotate)
{
	setclasspath(PATH_NAVIGATION);
	setreturntype(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	*G_valuecontent<Tvector>(retval)=G_3DCosmos().G_selviewport()->currentmove.scenerot;
}
ENDFUNCTION(func_navigation_currentrotate)



void Set_JoystickUseForNavigation(bool vl);

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_JoystickUseForNavigation,JoystickUseForNavigation)
{
	setclasspath(PATH_JOYSTICK);
	addvar(_qstr("status"),SC_valname_boolean);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Set_JoystickUseForNavigation(false/*arglist->get(0)->G_content_boolean()->G_val()*/);
//	Tjoystickinfo &joystickinfo=G_3DCosmos().G_joystickinfo(arglist->get(0)->G_content_scalar()->G_intval());
//	retval->G_content_scalar()->copyfrom(joystickinfo.G_axis(arglist->get(1)->G_content_scalar()->G_intval(),false));
}
ENDFUNCTION(func_JoystickUseForNavigation)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_joystick_getpos,JoystickGetAxis)
{
	setclasspath(PATH_JOYSTICK);
	addvar(_qstr("ID"),SC_valname_scalar);
	addvar(_qstr("AxisID"),SC_valname_scalar);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tjoystickinfo &joystickinfo=G_3DCosmos().G_joystickinfo(arglist->get(0)->G_content_scalar()->G_intval());
	retval->G_content_scalar()->copyfrom(joystickinfo.G_axis(arglist->get(1)->G_content_scalar()->G_intval(),false));
}
ENDFUNCTION(func_joystick_getpos)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_joystick_getposcorr,JoystickGetAxisCorrected)
{
	setclasspath(PATH_JOYSTICK);
	addvar(_qstr("ID"),SC_valname_scalar);
	addvar(_qstr("AxisID"),SC_valname_scalar);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tjoystickinfo &joystickinfo=G_3DCosmos().G_joystickinfo(arglist->get(0)->G_content_scalar()->G_intval());
	retval->G_content_scalar()->copyfrom(joystickinfo.G_axis(arglist->get(1)->G_content_scalar()->G_intval(),true));
}
ENDFUNCTION(func_joystick_getposcorr)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_joystick_rockerpos,JoystickRockerPos)
{
	setclasspath(PATH_JOYSTICK);
	addvar(_qstr("ID"),SC_valname_scalar);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tjoystickinfo &joystickinfo=G_3DCosmos().G_joystickinfo(arglist->get(0)->G_content_scalar()->G_intval());
	retval->G_content_scalar()->copyfrom(joystickinfo.G_rockerpos());
}
ENDFUNCTION(func_joystick_rockerpos)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_joystick_getbuttondown,JoystickButtonDown)
{
	setclasspath(PATH_JOYSTICK);
	addvar(_qstr("ID"),SC_valname_scalar);
	addvar(_qstr("ButtonID"),SC_valname_scalar);
	setreturntype(SC_valname_boolean);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tjoystickinfo &joystickinfo=G_3DCosmos().G_joystickinfo(arglist->get(0)->G_content_scalar()->G_intval());
	int butnr=arglist->get(1)->G_content_scalar()->G_intval();
	retval->G_content_boolean()->copyfrom(joystickinfo.G_buttondown(butnr));
}
ENDFUNCTION(func_joystick_getbuttondown)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_joystick_getbuttonclicked,JoystickButtonClicked)
{
	setclasspath(PATH_JOYSTICK);
	addvar(_qstr("ID"),SC_valname_scalar);
	addvar(_qstr("ButtonID"),SC_valname_scalar);
	setreturntype(SC_valname_boolean);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tjoystickinfo &joystickinfo=G_3DCosmos().G_joystickinfo(arglist->get(0)->G_content_scalar()->G_intval());
	int butnr=arglist->get(1)->G_content_scalar()->G_intval();
	retval->G_content_boolean()->copyfrom(joystickinfo.G_buttonclicked(butnr));
}
ENDFUNCTION(func_joystick_getbuttonclicked)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_joystick_getbuttonlongclicked,JoystickButtonLongClicked)
{
	setclasspath(PATH_JOYSTICK);
	addvar(_qstr("ID"),SC_valname_scalar);
	addvar(_qstr("ButtonID"),SC_valname_scalar);
	setreturntype(SC_valname_boolean);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tjoystickinfo &joystickinfo=G_3DCosmos().G_joystickinfo(arglist->get(0)->G_content_scalar()->G_intval());
	int butnr=arglist->get(1)->G_content_scalar()->G_intval();
	retval->G_content_boolean()->copyfrom(joystickinfo.G_buttonlongclicked(butnr));
}
ENDFUNCTION(func_joystick_getbuttonlongclicked)

