#pragma once
#include "tools.h"
#include "qstring.h"
#include "objecttree.h"

#include "SC_value.h"

#include "vec_transformation.h"

#include "valnames.h"


class TSC_functor;
class TSC_time;
class T3DMotionlist;
class T3DObjectCurve;
class Tcyclorbit2;
class Tlunaorbit;
class TPolyBezier3D;

class TSolidObjectData
{
public:
	double mass,charge;
	Taffinetransformation *position;
	Taffinetransformation *velocity;
public:
	TSolidObjectData()
	{
		mass=1.0;
		charge=0.0;
	}
};


class T3DMotion : public TObjectTreeItem
{
protected:
	T3DCosmos *cosmos;
	T3DMotionlist *parentlist;
	bool isinmotioncatalog;
public:
	T3DMotion();
	virtual ~T3DMotion();
	void Set_parentlist(T3DMotionlist *iparentlist) { parentlist=iparentlist; }
public:
	void addtomotioncatalog();
	virtual TObjectTreeItem *G_parent();
	virtual void adjusttime(double timestep) {};
	virtual void apply(TSolidObjectData *obj, double timestep) {};
	virtual void G_curposition(Tvertex &iposit) {};
	virtual void G_curspeed(Tvector &ispeed) {};
};


class T3DMotionFree : public T3DMotion
{
private:
public:
	static StrPtr GetClassName() { return SC_valname_motionfree; }
	virtual StrPtr G_classname() { return SC_valname_motionfree; }
	T3DMotionFree();
	virtual void adjusttime(double timestep);
	virtual void apply(TSolidObjectData *obj, double timestep);
	virtual void G_curposition(Tvertex &iposit);
	virtual void G_curspeed(Tvector &ispeed);
};


class T3DMotionRotate : public T3DMotion
{
private:
	Tvector *normdir;
	TSC_scalar *rotspeed;//in rad/s
	TSC_scalar *offsetangle;//in rad
	TSC_time *reftime;
	Taffinetransformation *transformation;
	TSC_value *timetype;
public:
	static StrPtr GetClassName() { return SC_valname_motionrotate; }
	virtual StrPtr G_classname() { return SC_valname_motionrotate; }
	T3DMotionRotate();
	virtual void adjusttime(double timestep);
	virtual void apply(TSolidObjectData *obj, double timestep);
	virtual void G_curposition(Tvertex &iposit);
	virtual void G_curspeed(Tvector &ispeed);
};


class T3DMotionSwing : public T3DMotion
{
private:
	Tvector *normdir;
	TSC_scalar *period;//in s
	TSC_scalar *swingangle;//in rad
	TSC_time *reftime;
	Taffinetransformation *transformation;
public:
	static StrPtr GetClassName() { return SC_valname_motionrotate; }
	virtual StrPtr G_classname() { return SC_valname_motionrotate; }
	T3DMotionSwing();
	virtual void adjusttime(double timestep);
	virtual void apply(TSolidObjectData *obj, double timestep);
	virtual void G_curposition(Tvertex &iposit);
	virtual void G_curspeed(Tvector &ispeed);
};


class T3DMotionKepler : public T3DMotion
{
private:
	TSC_time *peritime;
	TSC_scalar *a,*e,*i,*O,*w,*T;
private:
	struct {
		double M,E,v;
		double l,b;
		double r;
		double X,Y,Z;
	} posit;
	Tvertex position;
	Tvector speed;
private:
	static void solvekepler(double M, double& E, double ecc);
	void calcposit(const TSC_time& time);
public:
	void CreateCurve(T3DObjectCurve *crv, int resol);
public:
	static StrPtr GetClassName() { return SC_valname_motionkepler; }
	virtual StrPtr G_classname() { return SC_valname_motionkepler; }
	T3DMotionKepler();
	virtual void adjusttime(double timestep);
	virtual void apply(TSolidObjectData *obj, double timestep);
	virtual void G_curposition(Tvertex &iposit);
	virtual void G_curspeed(Tvector &ispeed);
};


class T3DMotionTable : public T3DMotion
{
private:
	Tdoublearray table_JD,table_X,table_Y,table_Z;
	Tvertex position;
	Tvector speed;
	TSC_value *interpoltype;
public:
	T3DMotionTable();
	static StrPtr GetClassName() { return SC_valname_motiontable; }
	virtual StrPtr G_classname() { return SC_valname_motiontable; }
	void reset();
	void add(double iJD, double iX, double iY, double iZ);
	void eval(double JD, Tvertex &posit, Tvector &spd);
	virtual void adjusttime(double timestep);
	virtual void apply(TSolidObjectData *obj, double timestep);
	virtual void G_curposition(Tvertex &iposit);
	virtual void G_curspeed(Tvector &ispeed);
	void CreateCurve(T3DObjectCurve *crv, int resol, const TSC_time &tm1, const TSC_time &tm2);
};


class T3DMotionSpline : public T3DMotion
{
private:
	TPolyBezier3D *spline;
	TSC_scalar *timefactor;//time multiplication factor
	TSC_time *reftime;
private:
	Tvertex position;
	Tvector speed;
public:
	T3DMotionSpline();
	static StrPtr GetClassName() { return SC_valname_motionspline; }
	virtual StrPtr G_classname() { return SC_valname_motionspline; }
	void eval(double JD, Tvertex &posit, Tvector &spd);
	virtual void adjusttime(double timestep);
	virtual void apply(TSolidObjectData *obj, double timestep);
	virtual void G_curposition(Tvertex &iposit);
	virtual void G_curspeed(Tvector &ispeed);
};


class T3DMotionSolidFunctor : public T3DMotion
{
private:
	TSpecificObjectRef<TSC_functor> functorref;
	Taffinetransformation position;
public:
	T3DMotionSolidFunctor();
	void set(TSC_functor *ifunctor);
	static StrPtr GetClassName() { return SC_valname_motionsolidfunctor; }
	virtual StrPtr G_classname() { return SC_valname_motionsolidfunctor; }
	virtual void adjusttime(double timestep);
	virtual void apply(TSolidObjectData *obj, double timestep);
	virtual void G_curposition(Tvertex &iposit);
	virtual void G_curspeed(Tvector &ispeed);
};

class T3DMotionPointFunctor : public T3DMotion
{
private:
	TSpecificObjectRef<TSC_functor> functorref;
	Tvertex position;
public:
	T3DMotionPointFunctor();
	void set(TSC_functor *ifunctor);
	static StrPtr GetClassName() { return SC_valname_motionpointfunctor; }
	virtual StrPtr G_classname() { return SC_valname_motionpointfunctor; }
	virtual void adjusttime(double timestep);
	virtual void apply(TSolidObjectData *obj, double timestep);
	virtual void G_curposition(Tvertex &iposit);
	virtual void G_curspeed(Tvector &ispeed);
};

class T3DMotionForceField : public T3DMotion
{
private:
	TSC_forcefield *forcefield;
	double timeJDcurrent;
public:
	T3DMotionForceField();
	static StrPtr GetClassName() { return SC_valname_motionforcefield; }
	virtual StrPtr G_classname() { return SC_valname_motionforcefield; }
	virtual void adjusttime(double timestep);
	virtual void apply(TSolidObjectData *obj, double timestep);
};


class T3DMotionCyclOrbit : public T3DMotion
{
private:
	Tcyclorbit2 *cyclorbit2;
	bool isowner;
	Tvertex position;
	Tvector speed;
public:
	T3DMotionCyclOrbit();
	~T3DMotionCyclOrbit();
	static StrPtr GetClassName() { return SC_valname_motioncyclorbit; }
	virtual StrPtr G_classname() { return SC_valname_motioncyclorbit; }
	void loadfile(StrPtr filename, double allowederror, double factor);
	void eval(double JD, Tvertex &posit, Tvector &spd);
	virtual void adjusttime(double timestep);
	virtual void apply(TSolidObjectData *obj, double timestep);
	virtual void G_curposition(Tvertex &iposit);
	virtual void G_curspeed(Tvector &ispeed);
	void CreateCurve(T3DObjectCurve *crv, int resol, const TSC_time &tm1);
};


class T3DMotionLuna : public T3DMotion
{
private:
	Tlunaorbit *lunaorbit;
	Tvertex position;
	Tvector speed;
	double rotatangle;
	TSC_scalar *factor;
public:
	T3DMotionLuna();
	~T3DMotionLuna();
	static StrPtr GetClassName() { return SC_valname_motionluna; }
	virtual StrPtr G_classname() { return SC_valname_motionluna; }
	void load();
	void eval(double JD, Tvertex &posit, Tvector &spd, double &rotatangle);
	virtual void adjusttime(double timestep);
	virtual void apply(TSolidObjectData *obj, double timestep);
	virtual void G_curposition(Tvertex &iposit);
	virtual void G_curspeed(Tvector &ispeed);
	void CreateCurve(T3DObjectCurve *crv, int resol, const TSC_time &tm1);
};
