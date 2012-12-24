#include "stdafx.h"
#include "3DScene.h"
#include "displaydevices.h"

#include "video.h"


//*********************************************************************
// T3DVideo
//*********************************************************************

T3DVideo::T3DVideo()
{
	aviplayer=NULL;
	videotexture=NULL;

	playing=addparam(_qstr("Playing"),SC_valname_boolean)->content.G_content_boolean();playing->copyfrom(false);
	framereduction=addparam(_qstr("FrameReduction"),SC_valname_scalar)->content.G_content_scalar();framereduction->copyfrom(1);

	counter=0;
}


T3DVideo::~T3DVideo()
{
	if (videotexture!=NULL) delete videotexture;
	if (aviplayer!=NULL) delete aviplayer;
}

void T3DVideo::setfile(StrPtr ifilename)
{
	if (aviplayer!=NULL) delete aviplayer;aviplayer=NULL;
	if (videotexture!=NULL) delete videotexture;videotexture=NULL;
	aviplayer=new Taviplayer;
	aviplayer->load(ifilename);
	if (!aviplayer->opened)
	{
		delete aviplayer;
		aviplayer=NULL;
		throw QError(_text("Unable to open AVI file"));
	}
	counter=0;
}

void T3DVideo::createtexture()
{
	if (aviplayer==NULL) throw QError(_text("AVI player not initialised"));
	if (videotexture!=NULL) delete videotexture;videotexture=NULL;
	videotexture=new Tvideotexture(aviplayer);
}


Taviplayer* T3DVideo::G_aviplayer_required()
{
	if (aviplayer==NULL) throw QError(_text("AVI file is not initialised"));
	return aviplayer;
}

Tvideotexture* T3DVideo::G_videotexture_required()
{
	if (videotexture==NULL) throw QError(_text("Video texture is not initialised"));
	return videotexture;
}


void T3DVideo::loadcurframe()
{
	if ((playing->G_val())&&(framereduction->G_intval()>0))
	{
		counter++;
		if (counter%framereduction->G_intval()==0)
			G_aviplayer_required()->setcurframe(G_aviplayer_required()->curframe+1);
	}
	G_videotexture_required()->loadcurframe();
}

void T3DVideo::select(Trendercontext *rc)
{
	G_videotexture_required()->select();
}

void T3DVideo::unselect(Trendercontext *rc)
{
	G_videotexture_required()->unselect();
}



bool T3DVideo::G_param_optionlist(StrPtr paramname, Tarray<QString> &list)
{
	return false;
}

void T3DVideo::paramchanged(StrPtr iname)
{
}

TObjectTreeItem* T3DVideo::G_parent()
{
	return T3DCosmos::Get().G_videolist();
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_video_create,addvideo)
{
	setclasspath_fromtype(SC_valname_video);
	setreturntype(SC_valname_video);
	addvar(_qstr("FileName"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DVideo *vd=G_3DCosmos().G_videolist()->addvideo();
	vd->setfile(arglist->get(0)->G_content_string()->G_string());
	retval->encapsulate(vd);
}
ENDFUNCTION(func_video_create)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_video_getframecount,GetFrameCount)
{
	setmemberfunction(SC_valname_video);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DVideo *vd=G_valuecontent<T3DVideo>(owner);
	retval->G_content_scalar()->copyfrom(vd->G_aviplayer_required()->framecount);
}
ENDFUNCTION(func_video_getframecount)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_video_getframerate,GetFrameRate)
{
	setmemberfunction(SC_valname_video);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DVideo *vd=G_valuecontent<T3DVideo>(owner);
	retval->G_content_scalar()->copyfrom(1.0/vd->G_aviplayer_required()->framedelay);
}
ENDFUNCTION(func_video_getframerate)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_video_getxres,GetXRes)
{
	setmemberfunction(SC_valname_video);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DVideo *vd=G_valuecontent<T3DVideo>(owner);
	retval->G_content_scalar()->copyfrom(vd->G_aviplayer_required()->resx);
}
ENDFUNCTION(func_video_getxres)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_video_getyres,GetYRes)
{
	setmemberfunction(SC_valname_video);
	setreturntype(SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DVideo *vd=G_valuecontent<T3DVideo>(owner);
	retval->G_content_scalar()->copyfrom(vd->G_aviplayer_required()->resy);
}
ENDFUNCTION(func_video_getyres)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_video_curframe,CurrentFrame)
{
	setmemberfunction(SC_valname_video);
	setreturntype(SC_valname_scalar);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DVideo *vd=G_valuecontent<T3DVideo>(owner);
	if (assigntoval!=NULL)
		vd->G_aviplayer_required()->setcurframe(assigntoval->G_content_scalar()->G_intval());
	retval->G_content_scalar()->copyfrom(vd->G_aviplayer_required()->curframe);
}
ENDFUNCTION(func_video_curframe)
