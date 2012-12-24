#pragma once

#include "tools.h"

#include "vecmath.h"


class CSGVector
{
public:
	double x,y,z;
	CSGVector()
	{
		x=0;y=0;z=0;
	}
	CSGVector(double ix, double iy, double iz)
	{
		x=ix;y=iy;z=iz;
	}
	CSGVector(const CSGVector &iv)
	{
		x=iv.x;y=iv.y;z=iv.z;
	}
	CSGVector(const Tvertex &iv)
	{
		x=iv.G3_x();
		y=iv.G3_y();
		z=iv.G3_z();
	}
	CSGVector(const Tvector &iv)
	{
		x=iv.G_x();
		y=iv.G_y();
		z=iv.G_z();
	}

	double G_size() const;
	bool IsZero() const;
	bool IsSame(const CSGVector &iv) const;

	void mul(double fc);
	void lincomb(const CSGVector &v1, double f1, const CSGVector &v2, double f2);
	void lincomb(const CSGVector &v1, double f1, const CSGVector &v2, double f2, const CSGVector &v3, double f3);
	void vecprod(const CSGVector &v1, const CSGVector &v2);
	void anynormalfrom(const CSGVector &v1);
	void normfrom(const CSGVector &v1);

	void Normalise();
	void Invert();

	static double dotprod(const CSGVector &v1, const CSGVector &v2);
	static double G_angle(const CSGVector &v1, const CSGVector &v2);

//	friend CSGVector operator*(const CSGVector &v1, const CSGVector &v2);
	friend CSGVector operator-(const CSGVector &v1, const CSGVector &v2);

};


double PointToLineDistance(const CSGVector &pt, const CSGVector &linept1, const CSGVector &linept2);



class CSGVertex;
class CSGFace;

typedef Tcheapcopyarray<CSGVertex> VertexPointerSet;

class CSGVertex
{
public:
	double x,y,z;
	CSGVector normal;
	int idx;//index in the parent shape structure
	VertexPointerSet neighborvertices;
	Tcheapcopyarray<CSGFace> memberfaces;

	CSGVertex();
	CSGVertex(double ix, double iy, double iz);
	CSGVertex(const CSGVertex &iv);
	CSGVertex(double ix, double iy, double iz, int istatus);

	void copyfrom(const CSGVertex &iv);
	CSGVector G_Position() const;
	void Set_Status(int iStatus);
	VertexPointerSet * getAdjacentVertices();
	int G_Status() const;
	void add_neighbor(CSGVertex *nb);
	void mark(int iStatus);
	CSGVertex & operator=(const CSGVertex &v);
	bool issamepos(const CSGVertex &iv) const;

	bool equals(const CSGVertex *iv) const { return issamepos(*iv); }

	bool equals(const CSGVector &ipos) const;

	static double G_distance(const CSGVertex &v1, const CSGVertex &v2);

public:
	static int UNKNOWN;
	static int INSIDE;
	static int OUTSIDE;
	static int BOUNDARY;
	int status;

};




typedef Tarray<CSGVertex> CSGVertexList;





class CSGBBox
{
public:

	double xmin;
	double xmax;
	double ymin;
	double ymax;
	double zmin;
	double zmax;
	
	CSGBBox();
	CSGBBox(const CSGVertex &v1, const CSGVertex &v2, const CSGVertex &v3);
	CSGBBox(const CSGVertexList &vertexlist);
	bool overlap(const CSGBBox &ibox);

	void copyfrom(const CSGBBox &ibbox);
	CSGBBox & operator=(const CSGBBox &v);


public:

	void AddVertex(const CSGVertex &vertex);
	void add(const CSGBBox &ibbox);
};



