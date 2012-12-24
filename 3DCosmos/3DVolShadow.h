#pragma once

#include "vecmath.h"
#include "renderwindow.h"
#include "3dmesh.h"
#include "vec_transformation.h"

class Tprecalcrendershadowvolumecontext;

class Tshadowvolume
{
public:
	Taffinetransformation transformation;
	bool hascaps1,hascaps2;
	Titemarray<Tvec3d> caps1,caps2,slices;
	GLenum capstype,slicestype;
public:
	void reset();
	void render(Trendercontext *rc);

public:
	void calcsphere(double radius, int resol, Tvertex &locallightpos, double lightradius, double depth1, double depth2);
};


class Tshadowvolumecontext
{
private:
	T3DScene *scene;
	Tarray< Tcopyarray<Tshadowvolume> > shadowvolumes;//idx1=shadownr
public:
	Tvertex lightpos;
public:
	Tshadowvolumecontext();
	void init(T3DScene *iscene);
	void adjustshadowvolumelist();
	int G_shadowcount();
	double G_lightradius(int shadownr);
	double G_depth1(int shadownr);
	double G_depth2(int shadownr);
	void addshadowvolume(int shadownr, Tshadowvolume *ivol);
	void rendershadowvolumes(int shadownr, Trendercontext *rc);
};
