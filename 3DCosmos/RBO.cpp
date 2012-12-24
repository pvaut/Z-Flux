#include "stdafx.h"
#include <process.h>

#include "opengl.h"

#include "qerror.h"

#include "qxstoredbitmap.h"

#include "RBO.h"


#include "3DScene.h"
#include "3DFont.h"
#include "displaydevices.h"


////////////////////////////////////////////////////////////////////////////////////////
// Trenderbufferobject
////////////////////////////////////////////////////////////////////////////////////////


Trenderbufferobject::Trenderbufferobject()
{
	ispresent=false;
}

Trenderbufferobject::~Trenderbufferobject()
{
	dispose();
}

void Trenderbufferobject::create(Trendercontext *irc, DataType idatatype, void *data, int datasize)
{
	dispose();
	ispresent=true;
	datatype=idatatype;
	rcref.setreffor(irc);
	irc->activate();
	glGenBuffers(1,&buffID);
	select();
	GLenum tpe=GL_ARRAY_BUFFER;
	if (datatype==BDF_ELEMENTS) tpe=GL_ELEMENT_ARRAY_BUFFER;
	glBufferData(tpe,datasize,data,GL_STATIC_DRAW);
	irc->checkerror(_text("Create buffer object"));
}

void Trenderbufferobject::dispose()
{
	if (!ispresent) return;
	Trendercontext *rc=rcref.G_ref();
	if (rc==NULL)
	{
		ASSERT(false);
		return;
	}
	rc->activate();
	glDeleteBuffers(1,&buffID);
	ispresent=false;
}

void Trenderbufferobject::select()
{
	ASSERT(ispresent);if (!ispresent) return;
	Trendercontext *rc=rcref.G_ref();
	if (rc==NULL)
	{
		ASSERT(false);
		return;
	}
	rc->checkerror(_text("Select buffer object"));
	GLenum tpe=GL_ARRAY_BUFFER;
	if (datatype==BDF_ELEMENTS) tpe=GL_ELEMENT_ARRAY_BUFFER;
	glBindBuffer(tpe,buffID);
	rc->checkerror(_text("End select buffer object"));
}

void Trenderbufferobject::unselect()
{
	ASSERT(ispresent);if (!ispresent) return;
	GLenum tpe=GL_ARRAY_BUFFER;
	if (datatype==BDF_ELEMENTS) tpe=GL_ELEMENT_ARRAY_BUFFER;
	glBindBuffer(tpe,NULL);
}
