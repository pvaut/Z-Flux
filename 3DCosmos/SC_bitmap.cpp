#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stdafx.h"

#include "tools.h"

#include <gdiplus.h>
using namespace Gdiplus;

//#include "b_math.h"

//#include "sxstring.h"
//#include "sxfile.h"
//#include "sxwin.h"

#include "qerror.h"
#include "qstringformat.h"

#include "qxstoredbitmap.h"

#include "SC_func.h"
#include "SC_env.h"


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_load,Load)
{
	setmemberfunction(SC_valname_bitmap);
	addvar(_qstr("filename"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *bmp=G_valuecontent<Tbmp>(owner);
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();
	bmp->loadgdiplus(filename);
}
ENDFUNCTION(func_bitmap_load)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_savejpg,SaveJpg)
{
	setmemberfunction(SC_valname_bitmap);
	addvar(_qstr("filename"),SC_valname_string);
	addvar(_qstr("quality"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *bmp=G_valuecontent<Tbmp>(owner);
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();
	int quality=arglist->get(1)->G_content_scalar()->G_intval();
	bmp->savejpg(filename,quality);
}
ENDFUNCTION(func_bitmap_savejpg)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_loadbitmap,LoadBitmap)
{
	setreturntype(SC_valname_bitmap);
	setclasspath_fromtype(SC_valname_bitmap);
	addvar(_qstr("filename"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *bmp=G_valuecontent<Tbmp>(retval);
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();
	bmp->loadgdiplus(filename);
}
ENDFUNCTION(func_bitmap_loadbitmap)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_createbitmap,CreateBitmap)
{
	setreturntype(SC_valname_bitmap);
	setclasspath_fromtype(SC_valname_bitmap);
	addvar(_qstr("xres"),SC_valname_scalar);
	addvar(_qstr("yres"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *bmp=G_valuecontent<Tbmp>(retval);
	int xres=arglist->get(0)->G_content_scalar()->G_intval();
	int yres=arglist->get(1)->G_content_scalar()->G_intval();
	bmp->init(xres,yres,3,0,255);
}
ENDFUNCTION(func_bitmap_createbitmap)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_xres,XRes)
{
	setmemberfunction(SC_valname_bitmap);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *bmp=G_valuecontent<Tbmp>(owner);
	retval->G_content_scalar()->copyfrom(bmp->G_xres());
}
ENDFUNCTION(func_bitmap_xres)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_yres,YRes)
{
	setmemberfunction(SC_valname_bitmap);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *bmp=G_valuecontent<Tbmp>(owner);
	retval->G_content_scalar()->copyfrom(bmp->G_yres());
}
ENDFUNCTION(func_bitmap_yres)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_mirror_horizontal,MirrorHorizontal)
{
	setmemberfunction(SC_valname_bitmap);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *bmp=G_valuecontent<Tbmp>(owner);
	bmp->mirrorhoriz();
}
ENDFUNCTION(func_bitmap_mirror_horizontal)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_mirror_vertical,MirrorVertical)
{
	setmemberfunction(SC_valname_bitmap);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *bmp=G_valuecontent<Tbmp>(owner);
	bmp->mirrorvert();
}
ENDFUNCTION(func_bitmap_mirror_vertical)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_mirror_diagonal,MirrorDiagonal)
{
	setmemberfunction(SC_valname_bitmap);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *bmp=G_valuecontent<Tbmp>(owner);
	bmp->mirrorvert();
}
ENDFUNCTION(func_bitmap_mirror_diagonal)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_crop,Crop)
{
	setmemberfunction(SC_valname_bitmap);
	setreturntype(SC_valname_bitmap);
	addvar(_qstr("x0"),SC_valname_scalar);
	addvar(_qstr("y0"),SC_valname_scalar);
	addvar(_qstr("lx"),SC_valname_scalar);
	addvar(_qstr("ly"),SC_valname_scalar);
	addvar(_qstr("angle"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *sourcebmp=G_valuecontent<Tbmp>(owner);
	Tbmp *targetbmp=G_valuecontent<Tbmp>(retval);
	int x0=arglist->get(0)->G_content_scalar()->G_intval();
	int y0=arglist->get(1)->G_content_scalar()->G_intval();
	int lx=arglist->get(2)->G_content_scalar()->G_intval();
	int ly=arglist->get(3)->G_content_scalar()->G_intval();
	double angle=0;
	if (arglist->G_ispresent(4))
		angle=arglist->get(4)->G_content_scalar()->G_val()/Pi*180;
	targetbmp->cropfrom(sourcebmp,x0,y0,lx,ly,angle);
//	StrPtr filename=arglist->get(0)->G_content_string()->G_string();
//	bmp->loadgdiplus(filename);
}
ENDFUNCTION(func_bitmap_crop)




///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_insert,Insert)
{
	setmemberfunction(SC_valname_bitmap);
	addvar(_qstr("bmp"),SC_valname_bitmap);
	addvar(_qstr("x0"),SC_valname_scalar);
	addvar(_qstr("y0"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *bmp=G_valuecontent<Tbmp>(owner);
	Tbmp *insertbmp=G_valuecontent<Tbmp>(arglist->get(0));
	int x0=arglist->get(1)->G_content_scalar()->G_intval();
	int y0=arglist->get(2)->G_content_scalar()->G_intval();
	bmp->insert(insertbmp,x0,y0);
}
ENDFUNCTION(func_bitmap_insert)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_reduce,Reduce)
{
	setmemberfunction(SC_valname_bitmap);
	setreturntype(SC_valname_bitmap);
	addvar(_qstr("factor"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *sourcebmp=G_valuecontent<Tbmp>(owner);
	Tbmp *targetbmp=G_valuecontent<Tbmp>(retval);
	int redfac=arglist->get(0)->G_content_scalar()->G_intval();
	targetbmp->reducefrom(sourcebmp,redfac);
}
ENDFUNCTION(func_bitmap_reduce)




///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bitmap_convertgray,ConvertGray)
{
	setmemberfunction(SC_valname_bitmap);
	setreturntype(SC_valname_bitmap);
	addvar(_qstr("channel"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tbmp *sourcebmp=G_valuecontent<Tbmp>(owner);
	Tbmp *targetbmp=G_valuecontent<Tbmp>(retval);
	targetbmp->copyfrom(sourcebmp);
	int channel=arglist->get(0)->G_content_scalar()->G_intval();
	targetbmp->convertgray(channel);
}
ENDFUNCTION(func_bitmap_convertgray)
