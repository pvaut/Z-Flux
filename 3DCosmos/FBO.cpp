#include "stdafx.h"
#include <process.h>

#include "opengl.h"

#include "qerror.h"

#include "qxstoredbitmap.h"

#include "3DScene.h"
#include "3DFont.h"
#include "renderwindow.h"
#include "displaydevices.h"
#include "shader.h"



////////////////////////////////////////////////////////////////////////////////////////
// Tframebufferobject
////////////////////////////////////////////////////////////////////////////////////////

Tframebufferobject::Tframebufferobject()
{
	ispresent=false;
	bound=false;

	usemipmaps=true;
}

Tframebufferobject::~Tframebufferobject()
{
	dispose();
}

void Tframebufferobject::create(Trendercontext *irc, int iresx, int iresy)
{
	resx=iresx;resy=iresy;


	dispose();
	glGenFramebuffers(1,&fbo);
	glBindFramebuffer(GL_FRAMEBUFFER,fbo);


	//create color renderbuffer
	//create texture
	glGenTextures(1, &img);
	glBindTexture(GL_TEXTURE_2D, img);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  resx, resy, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//alternative suggestion: antialiased version:
	//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,8,GL_RGBA, resx, resy, false);

	//generate mipmap
	if (usemipmaps)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	//attach it
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, img, 0);



	//create depth buffer
	glGenRenderbuffers(1, &depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, resx, resy);
	// Attach the depth render buffer to the FBO as it's depth attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);


	//check status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status==GL_FRAMEBUFFER_UNSUPPORTED)
		throw QError(_text("Unsupported frame buffer"));


	if (status!=GL_FRAMEBUFFER_COMPLETE)
		throw QError(_text("Incomplete frame buffer"));

	irc->checkerror(_text("FBO created"));

	ispresent=true;
}

void Tframebufferobject::dispose()
{
	unbind();
	if (!ispresent) return;
	ispresent=false;
}

void Tframebufferobject::bind()
{
	if (!ispresent) throw QError(_text("Frame buffer is not created"));
	if (bound)
		throw QError(_text("Frame buffer is already bound"));
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0,0,resx, resy);
	bound=true;
}

void Tframebufferobject::unbind()
{
	if (bound)
	{
		glPopAttrib();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	}
	bound=false;
}

void Tframebufferobject::activate_texture()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, img);
	if (usemipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);
    ::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    ::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
}


