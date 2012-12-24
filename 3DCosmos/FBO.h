#pragma once

class Trendercontext;

class Tframebufferobject
{
private:
	bool ispresent,bound;
	bool usemipmaps;
	GLuint fbo;
	GLuint depthbuffer;
	GLuint img;
	int resx,resy;
public:
	bool G_ispresent() { return ispresent; }
	int G_resx() { return resx; }
	int G_resy() { return resy; }
	Tframebufferobject();
	~Tframebufferobject();
	void create(Trendercontext *irc, int iresx, int iresy);
	void dispose();
	void bind();
	void unbind();
	void activate_texture();
};

