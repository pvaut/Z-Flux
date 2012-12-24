#pragma once

#include "CSG_components.h"

class CSGShape;

class CSGFace
{
private:
	bool facenormcalculated;
public:
	int label;
	CSGVertex *v1,*v2,*v3;//pointer to vertices in shape object
	CSGVector facenormal;
	CSGVector n1,n2,n3;//normals at each vertex of the triangle
	double area;
	CSGBBox bbox;
	CSGFace();
	CSGFace(CSGVertex *iv1, CSGVertex *iv2, CSGVertex *iv3);
	CSGFace(const CSGFace &inp);
	void operator=(const CSGFace &inp);

	void copyfrom(const CSGFace &inp);

	bool equals_pointersmatch(CSGFace * pFace);
	bool equals(const CSGFace &pOtherObject) const;

	void calcfacenormal();
	
	CSGBBox G_BBox();
	CSGVector G_Normal() const;
	double G_Area();

	CSGVector InterpolNormal(const CSGVector &pos) const;
	
	void invert();
		
	bool simpleClassify();
	void rayTraceClassify(CSGShape &object);

//private:

	bool hasPoint(const CSGVector &point);


	static int UNKNOWN;
	static int INSIDE;
	static int OUTSIDE;
	static int SAME;
	static int OPPOSITE;

	static int UP;
	static int DOWN;
	static int ON;
	static int NONE;

	int status;

	int G_Status();

	int linePositionInX(const CSGVector &point, const CSGVector &pointLine1, const CSGVector &pointLine2);
	int linePositionInY(const CSGVector &point, const CSGVector &pointLine1, const CSGVector &pointLine2);
	int linePositionInZ(const CSGVector &point, const CSGVector &pointLine1, const CSGVector &pointLine2);

};
