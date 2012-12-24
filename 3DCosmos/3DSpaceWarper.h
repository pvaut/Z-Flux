#pragma once

#include "vecmath.h"
#include "vec_transformation.h"

class TSC_functor;

class Tspacewarper
{
public:
	virtual void warpvertex(const Tvertex *vin, Tvertex *vout)=0;
	virtual void warpnormal(const Tvertex *vin, const Tvector *nin, Tvector *nout)=0;
};



class Tlineartransformer : public Tspacewarper
{
private:
	Taffinetransformation transf;
public:
	Tlineartransformer(Taffinetransformation *itransf);
	void warpvertex(const Tvertex *vin, Tvertex *vout);
	void warpnormal(const Tvertex *vin, const Tvector *nin, Tvector *nout);
};


class Tconalpincher : public Tspacewarper
{
private:
	double zh,a;
public:
	Tconalpincher(double i_zh);
	void warpvertex(const Tvertex *vin, Tvertex *vout);
	void warpnormal(const Tvertex *vin, const Tvector *nin, Tvector *nout);
};


class Tspiraler : public Tspacewarper
{
private:
	double a;
public:
	Tspiraler(double i_a);
	void warpvertex(const Tvertex *vin, Tvertex *vout);
	void warpnormal(const Tvertex *vin, const Tvector *nin, Tvector *nout);
};

class Tcustomwarper : public Tspacewarper
{
private:
	TSpecificObjectRef<TSC_functor> functorref;
public:
	Tcustomwarper(TSC_functor *ifunctor);
	void eval(const Tvertex &inp, Tvertex &outp);
	void warpvertex(const Tvertex *vin, Tvertex *vout);
	void warpnormal(const Tvertex *vin, const Tvector *nin, Tvector *nout);
};