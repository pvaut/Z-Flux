#include "stdafx.h"
#include <process.h>

#include "opengl.h"

#include "qerror.h"
#include "qfile.h"

#include "qxstoredbitmap.h"

#include "3DScene.h"
#include "3DFont.h"
#include "renderwindow.h"
#include "displaydevices.h"
#include "shader.h"



TShader::TShader()
{
	shadid_vertex=glCreateShader(GL_VERTEX_SHADER);
	shadid_fragment=glCreateShader(GL_FRAGMENT_SHADER);

	progid=glCreateProgram();

}

TShader::~TShader()
{
	glDeleteProgram(progid);
	glDeleteShader(shadid_vertex);
	glDeleteShader(shadid_fragment);
}


void TShader::setsource(GLuint id, StrPtr ifilename)
{

	QString content,line;
	QTextfile fl;
	fl.openread(ifilename);
	while (!fl.isend())
	{
		if (content.G_length()>0) content+=_qstr("\n");
		fl.readline(line);
		content+=line;
	}
	fl.close();



	char src[10000];
	wcstombs(src,content,9999);

//	strcpy(src,"void main()\n{\ngl_Position = ftransform();\n}\n");

	//GLint len=content.G_length();
	const char * srcp=src;
	GLint len=strlen(src);
	glShaderSource(id,1,&srcp,&len);
	return;
}


void TShader::loadsource_vertex(StrPtr ifilename)
{
	setsource(shadid_vertex,ifilename);
}

void TShader::loadsource_fragment(StrPtr ifilename)
{
	setsource(shadid_fragment,ifilename);
}


void TShader::compile()
{
	glCompileShader(shadid_vertex);
	printLog(shadid_vertex);
	glCompileShader(shadid_fragment);
	printLog(shadid_fragment);

	glAttachShader(progid,shadid_vertex);
//	glAttachShader(progid,shadid_fragment);
	glLinkProgram(progid);
	printLog(progid);
}

void TShader::printLog(GLuint obj)
{
	int infologLength = 0;
	int maxLength;
	
/*		if(glIsShader(obj))
			glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
		else
			glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&maxLength);*/
				
		char infoLog[10000];
	 
		if (glIsShader(obj))
			glGetShaderInfoLog(obj, 9999, &infologLength, infoLog);
		else
			glGetProgramInfoLog(obj, 9999, &infologLength, infoLog);
	 
		if (infologLength > 0)
			printf("%s\n",infoLog);
}



void TShader::use()
{
	glUseProgram(progid);
}