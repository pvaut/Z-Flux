#include "stdafx.h"

#include "qstring.h"
#include "qstringformat.h"
#include "qerror.h"


#include "opengl.h"

bool openglstarted=false;

void InitOpenGLExtensions()
{
	if (openglstarted) return;

	//addlog(_text("starting Glew"));
	//GLenum err = glewInit();
	//if (GLEW_OK != err)
	//{
	//  QString errstr=TFormatString(_text("Glew error: ^1"),0/*glewGetErrorString(err)*/);
	//  addlog(errstr);
	//  throw QError(errstr);
	//}
	//addlog(TFormatString(_text("Status: Using GLEW ^1"),0/*glewGetString(GLEW_VERSION)*/));


	openglstarted=true;
}