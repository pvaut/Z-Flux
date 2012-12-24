#include "stdafx.h"
#include <process.h>

#include "opengl.h"

#include "3DScene.h"
#include "renderwindow.h"
#include "3DFont.h"



T3DFont::T3DFont(StrPtr iname, int ifontid)
{
	name=iname;
	fontid=ifontid;
}

void T3DFont::init(CDC *dc)
{
	float depth=1.0f;
	if (::wglUseFontOutlines(dc->GetSafeHdc(),0,256,fontid,0.0f,depth,WGL_FONT_POLYGONS,gmf)==FALSE)
		throw_GetLastError(_text("Unable to generate font"));
//	permdc->SelectObject(GetStockObject(ANSI_VAR_FONT));
//	glCullFace(GL_BACK);
}


double T3DFont::G_textsizex(StrPtr txt)
{
	if (fontid<0) throw QError(_text("Font is not initialised"));
	int len=qstrlen(txt);
	double lenx=0;
	for (int i=0; i<len; i++)
	{
		unsigned char bt=(unsigned char)txt[i];
		lenx+=gmf[bt].gmfCellIncX;
	}
	return lenx;
}