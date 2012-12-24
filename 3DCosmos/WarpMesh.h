#pragma once

#include "matrix.h"

class T3Dmesh;
class Trendercontext;

class TWarpMesh
{
private:
	T3Dmesh *themesh;
	double xsize,ysize;
public:
	TWarpMesh();
	~TWarpMesh();
	void init(const TMatrix &warpx, const TMatrix &warpy);

	void apply(Trendercontext *rc);
};