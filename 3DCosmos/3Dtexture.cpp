#include "stdafx.h"

#include "qerror.h"

#include "qxstoredbitmap.h"


#include "3DScene.h"
#include "3DTexture.h"



//*********************************************************************
// T3DTexture
//*********************************************************************

T3DTexture::T3DTexture()
{
	subframe=NULL;
	filename_bitmap=addparam(_qstr("Bitmap"),SC_valname_string,true)->content.G_content_string();
	filename_bitmapalpha=addparam(_qstr("BitmapAlpha"),SC_valname_string,true)->content.G_content_string();
}

T3DTexture::~T3DTexture()
{
	texture.clear();
}

void T3DTexture::clear()
{
	texture.clear();
}

void T3DTexture::Set_name(StrPtr iname)
{
	G_param(_qstr("Name"))->content.fromstring(iname);
}


void T3DTexture::Set_subframe(T3DSubFrameObject *isubframe)
{
	subframe=isubframe;
}



void T3DTexture::paramchanged(StrPtr iname)
{
	//loadbitmapfile();
}

TObjectTreeItem* T3DTexture::G_parent()
{
	return subframe->G_texturelist();
}

void T3DTexture::loadbitmap(Tbmp *bmp, Tbmp *bmpalpha)
{
	texture.loadbitmap(bmp,bmpalpha);
}

void T3DTexture::loadbitmapfile(StrPtr ifilename, StrPtr bitmapalphafile)
{
	filename_bitmap->fromstring(ifilename);
	if (bitmapalphafile!=NULL) filename_bitmapalpha->fromstring(bitmapalphafile);
	else filename_bitmapalpha->fromstring(_qstr(""));
	loadbitmapfile();
}

void T3DTexture::loadbitmapfile()
{
	texture.loadbitmapfile(filename_bitmap->G_string(),filename_bitmapalpha->G_string());
}

void T3DTexture::attachvideo(T3DVideo *ivideo)
{
	attachedvideo.setreffor(ivideo);
}


void T3DTexture::select(Trendercontext *rc)
{
	if (attachedvideo.G_ref()!=NULL)
	{//video texture
		attachedvideo.G_obj().select(rc);
		rc->checkerror(_text("End select video texture"));
	}
	else
	{//static texture
		texture.select(rc);
		rc->checkerror(_text("End select texture"));
	}
}
void T3DTexture::unselect(Trendercontext *rc)
{
	if (attachedvideo.G_ref()!=NULL)
	{//video texture
		attachedvideo.G_obj().unselect(rc);
		rc->checkerror(_text("End unselect video texture"));
	}
	else
	{//static texture
		texture.unselect(rc);
		rc->checkerror(_text("End unselect texture"));
	}
}

double T3DTexture::G_aspectratio()
{
	if (!texture.G_ispresent()) return 1;
	int resx=texture.G_xres();
	int resy=texture.G_xres();
	if (resx<=0) return 1;
	else return resy*1.0/resx;
}

void T3DTexture::streamout_content(QBinTagWriter &writer)
{
}

void T3DTexture::streamin_content(QBinTagReader &reader)
{
	loadbitmapfile();
}


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_texture_create,CreateTexture)
{
	setmemberfunction(SC_valname_subframe);
	setreturntype(SC_valname_texture);
	addvar(_qstr("name"),SC_valname_string);
	addvar(_qstr("bitmapfile"),SC_valname_string);
	addvar(_qstr("bitmapfilealpha"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);
	StrPtr alphafile=NULL;
	if (arglist->G_ispresent(2)) alphafile=arglist->get(2)->G_content_string()->G_string();
	T3DTexture *texture=subframe->addtexture(arglist->get(0)->G_content_string()->G_string(),arglist->get(1)->G_content_string()->G_string(),alphafile);
	if (arglist->G_ispresent(0))
		texture->G_param(_qstr("Name"))->content.copyfrom(arglist->get(0));
	retval->encapsulate(texture);
}
ENDFUNCTION(func_texture_create)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_texture_create_frombitmap,CreateBitmapTexture)
{
	setmemberfunction(SC_valname_subframe);
	setreturntype(SC_valname_texture);
	addvar(_qstr("name"),SC_valname_bitmap);
	addvar(_qstr("bitmap"),SC_valname_bitmap);
	addvar(_qstr("bitmapalpha"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);

	Tbmp *bmp=G_valuecontent<Tbmp>(arglist->get(1));
	Tbmp *bmpalpha=NULL;
	if (arglist->G_ispresent(2))
		bmpalpha=G_valuecontent<Tbmp>(arglist->get(2));

	T3DTexture *texture=subframe->addbitmaptexture(arglist->get(0)->G_content_string()->G_string(),bmp,bmpalpha);
	if (arglist->G_ispresent(0))
		texture->G_param(_qstr("Name"))->content.copyfrom(arglist->get(0));
	retval->encapsulate(texture);
}
ENDFUNCTION(func_texture_create_frombitmap)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_texture_createvideo,CreateVideoTexture)
{
	setmemberfunction(SC_valname_subframe);
	setreturntype(SC_valname_texture);
	addvar(_qstr("name"),SC_valname_string);
	addvar(_qstr("source"),SC_valname_video);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(owner);
	T3DVideo *video=G_valuecontent<T3DVideo>(arglist->get(1));
	T3DTexture *texture=subframe->addvideotexture(
		arglist->get(0)->G_content_string()->G_string(),video);
	if (arglist->G_ispresent(0))
		texture->G_param(_qstr("Name"))->content.copyfrom(arglist->get(0));
	retval->encapsulate(texture);
}
ENDFUNCTION(func_texture_createvideo)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_texture_aspectratio,aspectratio)
{
	setmemberfunction(SC_valname_texture);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DTexture *texture=G_valuecontent<T3DTexture>(owner);
	retval->copyfrom(texture->G_aspectratio());
}
ENDFUNCTION(func_texture_aspectratio)
