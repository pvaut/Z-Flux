#include "stdafx.h"

#include "gl/gl.h"
#include "gl/glu.h"
//#include "gl/glaux.h"

#include "tesselator.h"
#include "2DContour.h"

#include "qerror.h"




#define tessclback void (__stdcall *) ()

Ttesselator& G_tesselator()
{
	return Ttesselator::Get();
}

Ttesselator::Ttesselator()
{
	tess=gluNewTess();
	if (tess==NULL) throw QError(_text("Unable to initialise OpenGL tesselator"));

	gluTessCallback(tess,GLU_TESS_BEGIN_DATA,(tessclback)callback_begin_data);
	gluTessCallback(tess,GLU_TESS_END_DATA,(tessclback)callback_end_data);
	gluTessCallback(tess,GLU_TESS_VERTEX_DATA,(tessclback)callback_vertex_data);

}

Ttesselator::~Ttesselator()
{
	gluDeleteTess(tess);
}

bool Ttesselator::tessok()
{
	ASSERT(tess!=NULL);
	return tess!=NULL;
}


void Ttesselator::addtriangle(int nr1, int nr2, int nr3)
{
	Ttesstriangle tr;tr.nr1=nr1;tr.nr2=nr2;tr.nr3=nr3;
	triangles.add(tr);
}


void Ttesselator::start_triangle_fan()
{
}

void Ttesselator::addpoint_triangle_fan(int ptnr)
{
	if (curptnr==0) centralpoint=ptnr;
	if (curptnr>=2)
	{
		addtriangle(centralpoint,prevpoint,ptnr);
	}
	prevpoint=ptnr;
}

void Ttesselator::addpoint_triangles(int ptnr)
{
	if (curptnr%3==2)
	{
		addtriangle(prevprevpoint,prevpoint,ptnr);
	}
	prevprevpoint=prevpoint;
	prevpoint=ptnr;
}

void Ttesselator::addpoint_trianglestrip(int ptnr)
{
	if (curptnr>=2)
	{
		if (curptnr%2==0)
			addtriangle(prevprevpoint,prevpoint,ptnr);
		else
			addtriangle(prevprevpoint,ptnr,prevpoint);
	}
	prevprevpoint=prevpoint;
	prevpoint=ptnr;
}


void Ttesselator::callback_begin_data(GLenum tpe, void *user_data)
{
	Ttesselator *ts=(Ttesselator*)user_data;
	ts->curtpe=tpe;
	ts->curptnr=0;
	if (tpe==GL_TRIANGLE_FAN)
	{
		ts->start_triangle_fan();
		return;
	}
	if (tpe==GL_TRIANGLE_STRIP)
	{
		return;
	}
	if (tpe==GL_TRIANGLES)
	{
		return;
	}
		
//	&&(tpe!=GL_TRIANGLE_STRIP)&&(tpe!=GL_TRIANGLES))
	ASSERT(false);
	throw QError(_text("Invalid tesselation primitive type"));
}

void Ttesselator::callback_end_data(void *user_data)
{
	Ttesselator *ts=(Ttesselator*)user_data;
	ts->curtpe=-1;
}

void Ttesselator::callback_vertex_data(void *vertex_data, void *user_data)
{
	Ttesselator *ts=(Ttesselator*)user_data;
	int ptnr=*(int*)vertex_data;
	if (ts->curtpe==GL_TRIANGLE_FAN)
	{
		ts->addpoint_triangle_fan(ptnr);
		ts->curptnr++;
		return;
	}
	if (ts->curtpe==GL_TRIANGLES)
	{
		ts->addpoint_triangles(ptnr);
		ts->curptnr++;
		return;
	}
	if (ts->curtpe==GL_TRIANGLE_STRIP)
	{
		ts->addpoint_trianglestrip(ptnr);
		ts->curptnr++;
		return;
	}
	ASSERT(false);
}

void Ttesselator::callback_combine_data(GLdouble coords[3], void *vertex_data[4], GLfloat weigths[4], void **outData, void *user_data)
{
	ASSERT(false);
	throw QError(_text("Invalid tesselation action"));
}





void Ttesselator::tesselate(T2DContourset *contourset)
{
	GLdouble coords[3];
	if (!tessok()) return;
	curtpe=-1;

/*	for (int contournr=0; contournr<contourset->G_contourcount(); contournr++)
		if (!contourset->G_contour(contournr)->G_closed())
			throw QError(_text("Unable to tesselate unclosed contours"));*/


	gluTessNormal(tess,0,0,1);
	gluTessBeginPolygon(tess,this);
	triangles.reset();

	int ptnr=0;
	for (int contournr=0; contournr<contourset->G_contourcount(); contournr++)
	{
		T2DContour *contour=contourset->G_contour(contournr);
		gluTessBeginContour(tess);
		for (int vnr=0; vnr<contour->G_pointcount(); vnr++)
		{
			coords[0]=contour->G_point(vnr).pt.G3_x();
			coords[1]=contour->G_point(vnr).pt.G3_y();
			coords[2]=contour->G_point(vnr).pt.G3_z();
			contour->G_point(vnr).ptnr=ptnr;ptnr++;
			gluTessVertex(tess,coords,(void*)&contour->G_point(vnr).ptnr);
		}
		gluTessEndContour(tess);
	}

	gluTessEndPolygon(tess);
}
