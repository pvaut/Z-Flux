#pragma once

#include "vecmath.h"
#include "RBO.h"
#include "renderwindow.h"

class Tprecalcrendershadowvolumecontext;
class Tshadowvolume;
class Tspacewarper;
class CSGShape;


#define V3DFLOAT GLfloat
#define VELTPE GLuint

class Tvec3d {
public:
	V3DFLOAT x,y,z;
	Tvec3d() { x=0.0; y=0.0; z=0.0; };
	Tvec3d(double ix, double iy, double iz) { x=(V3DFLOAT)ix; y=(V3DFLOAT)iy; z=(V3DFLOAT)iz; }

	void set(double ix, double iy, double iz) { x=(V3DFLOAT)ix; y=(V3DFLOAT)iy; z=(V3DFLOAT)iz; }

	void lincomb(Tvec3d &pt1, double f1);
	void lincomb(Tvec3d &pt1, Tvec3d &pt2, double f1, double f2);
	void lincomb(Tvec3d &pt1, Tvec3d &pt2, Tvec3d &pt3, double f1, double f2, double f3);
	void lincomb(Tvec3d &pt1, Tvec3d &pt2, Tvec3d &pt3, Tvec3d &pt4, double f1, double f2, double f3, double f4);
	void vecprod(Tvec3d &pt1, Tvec3d &pt2);
	void norm();
	void multiply(double fc);
	void rotate(Tvec3d &vec, Tvec3d &norm, double ang);

	double getsize();

	static double distance(Tvec3d &pt1, Tvec3d &pt2);
	static double dotprod(Tvec3d &pt1, Tvec3d &pt2);

	Tvec3d& operator=(const Tvec3d &inp);
	Tvec3d& operator=(const Tvector &inp);
	Tvec3d& operator=(const Tvertex &inp);

	void copyfrom(const Tvertex &inp);
};

void streamout_vec3darray(QBinTagWriter &writer, Titemarray<Tvec3d> &arr);
void streamin_vec3darray(QBinTagReader &reader, Titemarray<Tvec3d> &arr);

class T4color
{
public:
	V3DFLOAT r,g,b,a;
	T4color()
	{
		r=1;g=1;b=1;a=1;
	}
	T4color(double ir, double ig, double ib, double ia)
	{
		r=(V3DFLOAT)ir;
		g=(V3DFLOAT)ig;
		b=(V3DFLOAT)ib;
		a=(V3DFLOAT)ia;
	}
	void copyfrom(const TSC_color &icol)
	{
		r=(V3DFLOAT)icol.G_R();
		g=(V3DFLOAT)icol.G_G();
		b=(V3DFLOAT)icol.G_B();
		a=(V3DFLOAT)icol.G_A();
	}
	void lincomb(const T4color &col1, const T4color &col2, V3DFLOAT vl1, V3DFLOAT vl2)
	{
		r=vl1*col1.r+vl2*col2.r;
		g=vl1*col1.g+vl2*col2.g;
		b=vl1*col1.b+vl2*col2.b;
		a=vl1*col1.a+vl2*col2.a;
	}
	void ColMapL();
	void ColMapR();
};

class T2textureidx
{
public:
	V3DFLOAT tx1,tx2;
	T2textureidx(double itx1, double itx2)
	{
		tx1=(V3DFLOAT)itx1;
		tx2=(V3DFLOAT)itx2;
	}
	void lincomb(const T2textureidx &t1, const T2textureidx &t2, V3DFLOAT vl1, V3DFLOAT vl2)
	{
		tx1=vl1*t1.tx1+vl2*t2.tx1;
		tx2=vl1*t1.tx2+vl2*t2.tx2;
	}
};


class Ttriangle
{
public:
	int label;
	int vertices[3];
	Tvec3d facenormal;
	char lightfacing;
	Ttriangle(int v1, int v2, int v3);
	Ttriangle& operator=(const Ttriangle &inp);
};

class Tedge
{
public:
	int vertices[2];
	int triangles[2];//0=forward 1=reverse
	Tedge& operator=(const Tedge &inp);
};

class T3Dmesh_Frame
{
public:
	double phase;
	Titemarray<Tvec3d> vertices,vertices_normals;
	Titemarray<T4color> vertices_colL,vertices_colR;
	Titemarray<T2textureidx> vertices_texcoord;
};

class T3Dmesh
{
private:
	int strips_rowsize;//used for faster drawing if vertices actually consist in a set of triangle strips
	bool strips_closed;
	bool buffered,hasowncolors;
	Trenderbufferobject buffer_vertices,buffer_normals,buffer_texture,buffer_elements,buffer_colorsL,buffer_colorsR;

private:
	void render_novertexnormals(Trendercontext *rc, bool hastexture, bool canbuffer);
	void render_vertexnormals(Trendercontext *rc, bool hastexture, bool canbuffer);

public:
	Titemarray<Tvec3d> vertices,vertices_normals;
	Tintarray vertices_label;
	Tintarray vertices_idx;//each unique vertex has unique index, "same" vertices have same indices used for edge determination and smooth shadowing
	Titemarray<T4color> vertices_colL,vertices_colR;
	Titemarray<T2textureidx> vertices_texcoord;

	Tvec3d zeronormal;

	Tarray<T3Dmesh_Frame> frames;

public:
	bool needcalcedges;
	bool hasvertexnormals;
	Tintarray boundingedgesv1,boundingedgesv2;
public:
	Tvec3d centerpos;
	Titemarray<VELTPE> trianglesindices;
	Titemarray<Ttriangle> triangles;
	Titemarray<Tedge> edges;
	void calccenter();
	void calcnormals();
	void calcsmoothnormals();
	void calcedges();
	void generateallvertexnormals();
	void subtriangle(double maxdist);
	void warp(Tspacewarper &w);

	void copyfrom(const CSGShape &shape);

public:
	T3Dmesh();
	~T3Dmesh();
	void reset();
	void loadfile(const char *filename);
	void Make_stripbased(int irowsize, bool iclosed);
	int addvertex(Tvec3d *v, int vertexidx, Tvec3d *normal=NULL, double texcoord1=0, double texcoord2=0);
	int addvertex(Tvertex *v, int vertexidx, Tvector *normal=NULL, double texcoord1=0, double texcoord2=0);
	int addcolorvertex(Tvec3d *v, int vertexidx, Tvec3d *normal, double r, double g, double b, double a);
	int addcolorvertex(Tvertex *v, int vertexidx, Tvector *normal, double r, double g, double b, double a);
	void Set_vertexcolor(int vertexnr, double r, double g, double b, double a);
	void Set_hasowncolors() { hasowncolors=true; }
	void addtriangle(int i1, int i2, int i3);
	void addquad(int i1, int i2, int i3, int i4);
	void createsphere(Tvec3d &cent, double radius, int resol);
	void copytoframe(double iphase);
	void interpolframe(double phase);

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

public:
	void render(Trendercontext *rc, bool hastexture, bool canbuffer);

private:
	Titemarray<Tvec3d> caps1,caps2,slices;
public:

	void calcshadowvolume(Tshadowvolume *shadowvolume, Tvertex &ilightpos, double depth1, double depth2);
};