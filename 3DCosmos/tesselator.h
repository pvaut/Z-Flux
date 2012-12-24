#pragma once

#include "tools.h"

class GLUtesselator;
class T2DContourset;

class Ttesstriangle
{
public:
	int nr1,nr2,nr3;
};

class Ttesselator
{
private:
	GLUtesselator *tess;
	GLenum curtpe;
	int curptnr,centralpoint,prevpoint,prevprevpoint;
	Titemarray<Ttesstriangle> triangles;
private:
	static void __stdcall callback_begin_data(GLenum tpe, void *user_data);
	static void __stdcall callback_end_data(void *user_data);
	static void __stdcall callback_vertex_data(void *vertex_data, void *user_data);
	static void __stdcall callback_combine_data(GLdouble coords[3], void *vertex_data[4], GLfloat weigths[4], void **outData, void *user_data);
private:
	bool tessok();
	void start_triangle_fan();
	void addpoint_triangle_fan(int nr);
	void addpoint_triangles(int nr);
	void addpoint_trianglestrip(int nr);
	void addtriangle(int nr1, int nr2, int nr3);
public:
	static Ttesselator& Get()
	{
		 static Ttesselator env;
		 return env;
	}
	Ttesselator();
	~Ttesselator();
public:
	void tesselate(T2DContourset *contourset);
	int G_trianglecount() { return triangles.G_count(); }
	Ttesstriangle& G_triangle(int nr) { return triangles[nr]; }

};

Ttesselator& G_tesselator();