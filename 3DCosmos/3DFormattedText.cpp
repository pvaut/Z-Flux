#include "stdafx.h"

#include "vecmath.h"
#include "vec_transformation.h"

#include "opengl.h"

#include "3DScene.h"
#include "3Dsurface.h"
#include "3DGeoObjects.h"
#include "3DFont.h"

#include "formattext.h"

#include "3DFormattedText.h"

#include "renderwindow.h"

#include "opengl.h"

///////////////////////////////////////////////////////////////////////////
// TxtRendererOpenGL
///////////////////////////////////////////////////////////////////////////

TxtRendererOpenGL::TxtRendererOpenGL()
{
	sizefactor=1;
	rc=NULL;
}

void TxtRendererOpenGL::check_rcpresent() const
{
	if (rc==NULL) throw QError(_text("OpenGL formatted text renderer: no render context is present"));
}

StrPtr TxtRendererOpenGL::G_fontname(const TxtFont &fnt) const
{
	if (fnt.G_fontstyle()==TxtFont::FNTSTYLE_SYMBOL) return SYMBOL_FONT;
	if (fnt.G_fontstyle()==TxtFont::FNTSTYLE_BOLD) return BOLD_FONT;
	if (fnt.G_fontstyle()==TxtFont::FNTSTYLE_ITALIC) return ITALIC_FONT;
	return DEFAULT_FONT;
}


double TxtRendererOpenGL::G_textsizex(const TxtFont &fnt, StrPtr itxt) const
{
	check_rcpresent();
	return rc->G_renderwindow()->G_font(G_fontname(fnt))->G_textsizex(itxt)*sizefactor*fnt.G_size();
}

double TxtRendererOpenGL::G_textsizey(const TxtFont &fnt, StrPtr itxt) const
{
	check_rcpresent();
	return 1*sizefactor*fnt.G_size();
}

double TxtRendererOpenGL::G_fontsize(const TxtFont &fnt) const
{
	check_rcpresent();
	return 1*sizefactor*fnt.G_size();
}


double TxtRendererOpenGL::G_textbaseline(const TxtFont &fnt, StrPtr itxt) const
{
	check_rcpresent();
	return 0.75*sizefactor*fnt.G_size();
}

void TxtRendererOpenGL::showtext(const TxtFont &fnt, double x0, double y0, StrPtr itxt) const
{
	check_rcpresent();
	Tvertex origin(x0,-1*(y0+0.75*sizefactor*fnt.G_size()),0);
	Tvector unitdirx(sizefactor*fnt.G_size(),0,0);
	Tvector unitdiry(0,sizefactor*fnt.G_size(),0);
	rc->rendertext(G_fontname(fnt),&origin,&unitdirx,&unitdiry,itxt,0);
}

void TxtRendererOpenGL::render_horizontalline(double x0, double y0, double xlen, double width) const
{
	Tdoublearray px,py;
	px.add(x0);py.add(y0);
	px.add(x0+xlen);py.add(y0);
	render_polyline(px,py,width);
}

void TxtRendererOpenGL::render_verticalline(double x0, double y0, double ylen, double width) const
{
	Tdoublearray px,py;
	px.add(x0);py.add(y0);
	px.add(x0);py.add(y0+ylen);
	render_polyline(px,py,width);
}

void TxtRendererOpenGL::render_horizontalarrow(double x0, double y0, double xlen, double width) const
{
	check_rcpresent();
	double xlen2=xlen*0.75;
	render_horizontalline(x0,y0,xlen2,width);

	glBegin(GL_TRIANGLES);
	glVertex3d(x0+xlen2,-1*(y0+1.3*width),0.0);
	glVertex3d(x0+xlen,-1*(y0),0.0);
	glVertex3d(x0+xlen2,-1*(y0-1.3*width),0.0);
	glEnd();

}

void TxtRendererOpenGL::render_polyline(Tdoublearray &px, Tdoublearray &py, double width) const
{
	check_rcpresent();

	if (px.G_count()<2) return;
	glBegin(GL_QUADS);
	for (int i=0; i<px.G_count(); i++)
	{
		double nx=0;
		double ny=0;
		if (i<px.G_count()-1)
		{
			double dx=px[i+1]-px[i];
			double dy=py[i+1]-py[i];
			double dd=sqrt(dx*dx+dy*dy);
			if (dd<=0) throw QError(_text("Invalid polyline"));
			nx+=dy/dd;ny+=-dx/dd;
		}
		if (i>0)
		{
			double dx=px[i]-px[i-1];
			double dy=py[i]-py[i-1];
			double dd=sqrt(dx*dx+dy*dy);
			if (dd<=0) throw QError(_text("Invalid polyline"));
			nx+=dy/dd;ny+=-dx/dd;
		}
		double ns=sqrt(nx*nx+ny*ny);
		if (ns<=0) throw QError(_text("Invalid polyline"));
		nx=nx/ns*width/2;
		ny=ny/ns*width/2;

		if ((i>0)&&(i<px.G_count()-1))//correct join thickness
		{
			double d1x=px[i-1]-px[i];
			double d1y=py[i-1]-py[i];
			double d2x=px[i+1]-px[i];
			double d2y=py[i+1]-py[i];
			double cosang=(d1x*d2x+d1y*d2y)/(sqrt(d1x*d1x+d1y*d1y)*sqrt(d2x*d2x+d2y*d2y));
			if (cosang<1)
			{
				double sinang=sqrt(1-cosang*cosang);
				double sf=sqrt(2.0)*(1+cosang/sinang);
				nx*=sf;
				ny*=sf;
			}
		}

		if (i>0)//close previous rectangle
		{
			glVertex3d(px[i]-nx,-1*(py[i]-ny),0.0);
			glVertex3d(px[i]+nx,-1*(py[i]+ny),0.0);
		}
		if (i<px.G_count()-1)//begin next rectangle
		{
			glVertex3d(px[i]+nx,-1*(py[i]+ny),0.0);
			glVertex3d(px[i]-nx,-1*(py[i]-ny),0.0);
		}
	}
	glEnd();
}

void TxtRendererOpenGL::render_boundingbox(double x0, double y0, double sizex, double sizey, double baseline) const
{
	return;

	if ((sizex<=0)||(sizey<=0))
	{
		return;
	}
	Tdoublearray px,py;
	px.add(x0);py.add(y0);
	px.add(x0+sizex);py.add(y0);
	px.add(x0+sizex);py.add(y0+sizey);
	px.add(x0);py.add(y0+sizey);
	px.add(x0);py.add(y0);
	render_polyline(px,py,0.01);

	render_horizontalline(x0,y0+baseline,sizex,0.002);
}


//*********************************************************************
// T3DObject3DFormattedText
//*********************************************************************

T3DObject3DFormattedText::T3DObject3DFormattedText(T3DScene *iscene) : T3DGeoObject(iscene)
{
	env=new TxtEnv(1);
	content=addparam(_qstr("Content"),SC_valname_string)->content.G_content_string();
	size=addparam(_qstr("Size"),SC_valname_scalar)->content.G_content_scalar();
	size->copyfrom(1.0);
	maxlenx=addparam(_qstr("MaxLenX"),SC_valname_scalar)->content.G_content_scalar();
	maxlenx->copyfrom(99999.0);
	cancache->copyfrom(false);
	formatted=false;
	rendered_textsizey=-1;
}

T3DObject3DFormattedText::~T3DObject3DFormattedText()
{
	delete env;
}

void T3DObject3DFormattedText::paramchanged(StrPtr iname)
{
	formatted=false;
}


void T3DObject3DFormattedText::formattext(Trendercontext *rc)
{
	renderer.Set_sizefactor(size->G_val());
	env->parsefrom(content->G_string());
	env->calc(renderer,maxlenx->G_val());

	formatted=true;
	rendered_textsizey=env->G_sizey();
}


void T3DObject3DFormattedText::precalcrender_exec(Tprecalcrendercontext *pre)
{
}

void T3DObject3DFormattedText::render_exec(Trendercontext *rc)
{
	renderer.Set_rendercontext(rc);
	if (!formatted) formattext(rc);
	env->render(0,0,renderer);
	renderer.Set_rendercontext(NULL);
}


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_formattexgetsizey,GetSizeY)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_formattedtext3d);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObject3DFormattedText *obj=G_valuecontent<T3DObject3DFormattedText>(owner);
	if (obj->rendered_textsizey<0)
		throw QError(_text("Rendered text size is not yet determined"));
	retval->copyfrom(obj->rendered_textsizey);
}
ENDFUNCTION(func_object_formattexgetsizey)
