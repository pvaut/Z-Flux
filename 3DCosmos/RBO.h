#pragma once

#include "opengl.h"

class Trendercontext;

class Trenderbufferobject
{
public:
	static enum DataType { BFD_VERTEXDATA,BDF_ELEMENTS };
private:
	bool ispresent;
	GLuint buffID;
	DataType datatype;
	TSpecificObjectRef<Trendercontext> rcref;
public:
	Trenderbufferobject();
	~Trenderbufferobject();
	void create(Trendercontext *irc, DataType idatatype, void *data, int datasize);
	void dispose();
	void select();
	void unselect();
};
