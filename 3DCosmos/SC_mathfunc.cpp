#include "stdafx.h"

#include "qerror.h"

#include "valnames.h"

#include "utils.h"

#include "SC_func.h"
#include "SC_script.h"
#include "SC_env.h"

#include "3Dscene.h"


#define mathfunctions CLASSNAME_MATHEMATICS
#define goniofunctions _qstr("Mathematics;Trigonometry")





///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_pi,Pi)
{
	setclasspath(goniofunctions);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_scalar()->copyfrom(Pi);
}
ENDFUNCTION(func_pi)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_rad2deg,rad2deg)
{
	setclasspath(goniofunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("anglerad"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(val/Pi*180);
}
ENDFUNCTION(func_rad2deg)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_deg2rad,deg2rad)
{
	setclasspath(goniofunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("angledeg"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(val/180*Pi);
}
ENDFUNCTION(func_deg2rad)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_sin,sin)
{
	setclasspath(goniofunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("angle"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(sin(val));
}
ENDFUNCTION(func_sin)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_cos,cos)
{
	setclasspath(goniofunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("angle"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(cos(val));
}
ENDFUNCTION(func_cos)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_tan,tan)
{
	setclasspath(goniofunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("angle"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(tan(val));
}
ENDFUNCTION(func_tan)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_asin,asin)
{
	setclasspath(goniofunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("arg"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(asin(val));
}
ENDFUNCTION(func_asin)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_acos,acos)
{
	setclasspath(goniofunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("arg"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(acos(val));
}
ENDFUNCTION(func_acos)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_atan,atan)
{
	setclasspath(goniofunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("arg"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(atan(val));
}
ENDFUNCTION(func_atan)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_angle,angle)
{
	setclasspath(goniofunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("x"),SC_valname_scalar);
	addvar(_qstr("y"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double x=arglist->get(0)->G_content_scalar()->G_val();
	double y=arglist->get(1)->G_content_scalar()->G_val();
	double ang=atan2(y,x);
	if (ang<0) ang+=2*Pi;
	retval->G_content_scalar()->copyfrom(ang);
}
ENDFUNCTION(func_angle)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_random,random)
{
	setclasspath(mathfunctions);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_scalar()->copyfrom(random());
}
ENDFUNCTION(func_random)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_floor,floor)
{
	setclasspath(mathfunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("arg"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(floor(val));
}
ENDFUNCTION(func_floor)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_round,round)
{
	setclasspath(mathfunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("arg"),SC_valname_scalar);
	addvar(_qstr("digits"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	int digits=0;
	if (arglist->G_ispresent(1)) digits=arglist->get(1)->G_content_scalar()->G_intval();
	double pw=1;for (int i=0; i<digits; i++) pw*=10;
	val=floor(val*pw+0.5)/pw;
	retval->G_content_scalar()->copyfrom(val);
}
ENDFUNCTION(func_round)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_abs,abs)
{
	setclasspath(mathfunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("arg"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(fabs(val));
}
ENDFUNCTION(func_abs)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_sign,sign)
{
	setclasspath(mathfunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("arg"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	if (val>0) val=+1;
	if (val<0) val=-1;
	retval->G_content_scalar()->copyfrom(val);
}
ENDFUNCTION(func_sign)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_sqr,sqr)
{
	setclasspath(mathfunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("arg"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(val*val);
}
ENDFUNCTION(func_sqr)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_sqrt,sqrt)
{
	setclasspath(mathfunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("arg"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	if (val<0) throw QError(_text("Invalid value for sqrt function"));
	retval->G_content_scalar()->copyfrom(sqrt(val));
}
ENDFUNCTION(func_sqrt)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_distfunc,DistFunc)
{
	setclasspath(mathfunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("x"),SC_valname_scalar);
	addvar(_qstr("y"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double x=arglist->get(0)->G_content_scalar()->G_val();
	double y=arglist->get(1)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(sqrt(x*x+y*y));
}
ENDFUNCTION(func_distfunc)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_exp,exp)
{
	setclasspath(mathfunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("arg"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(exp(val));
}
ENDFUNCTION(func_exp)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_log,log)
{
	setclasspath(mathfunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("arg"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double val=arglist->get(0)->G_content_scalar()->G_val();
	if (val<=0) throw QError(_text("Invalid value for log function"));
	retval->G_content_scalar()->copyfrom(log(val));
}
ENDFUNCTION(func_log)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_smoothramp1,SmoothRamp1)
{
	setclasspath(mathfunctions);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("arg"),SC_valname_scalar);
	addvar(_qstr("SmoothFraction"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double x=arglist->get(0)->G_content_scalar()->G_val();

	double d=arglist->get(1)->G_content_scalar()->G_val();//fraction where smooth growth becomes linear
	if (d>0.5) d=0.5;
	double A=2*d/(Pi-2*Pi*d+4*d);
	if (x<d)
	{
		retval->G_content_scalar()->copyfrom(A-A*cos(x/d*Pi/2));
		return;
	}
	if (x>1-d)
	{
		retval->G_content_scalar()->copyfrom(1-(A-A*cos((1-x)/d*Pi/2)));
		return;
	}
	retval->G_content_scalar()->copyfrom(0.5+(x-0.5)*A*Pi/(2*d));
	return;
}
ENDFUNCTION(func_smoothramp1)
