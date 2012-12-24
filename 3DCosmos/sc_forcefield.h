#pragma once

#include "tobject.h"
#include "qstring.h"
#include "qbintagfile.h"
#include "tools.h"
#include "QError.h"
#include "valnames.h"

#include "SC_time.h"
#include "SC_functor.h"

class TSC_script;
class TSC_script_cmd_expression;

class Tintegratorcomp
{
public:
	Tintegratorcomp();
	virtual ~Tintegratorcomp();
	virtual StrPtr G_typename()=0;
	virtual Tintegratorcomp* duplicate()=0;
	virtual void tostring(QString &str, int tpe) { str=G_typename(); }
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)=0;
};



class Tintegratorcomp_functor : public Tintegratorcomp
{
private:
	TSpecificObjectRef<TSC_functor> functorref;
public:
	Tintegratorcomp_functor(TSC_functor &ifunctor)
	{
		functorref.setreffor(&ifunctor);
	}
	virtual StrPtr G_typename() { return _qstr("Functor"); }
	virtual Tintegratorcomp* duplicate()
	{
		return new Tintegratorcomp_functor(functorref.G_obj());
	}
	virtual void tostring(QString &str, int tpe);
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force);
};


class TSC_forcefield : public TObject
{
private:
	double maxtimestep;//in seconds
	double maxspacestep;
	Tarray<Tintegratorcomp> components;
private:
	int constrictsurf_type;//0=none 1=sphere
	Tvertex constrictsphere_center;
	double constrictsphere_radius;
public:
	static StrPtr GetClassName() { return SC_valname_forcefield; }
	TSC_forcefield();
	~TSC_forcefield();
	void copyfrom(const TSC_forcefield &v);
	void operator=(const TSC_forcefield &v);
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
public:
	void SetAccuracy(double imaxtimestep, double imaxspacestep);
	void Set_constrict_sphere(Tvertex &posit, double radius);
	void addcomp(Tintegratorcomp *icomp) { components.add(icomp); }
	static bool compare(TSC_forcefield *v1, TSC_forcefield *v2) { return false; }
	static bool comparesize(TSC_forcefield *v1, TSC_forcefield *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
	void createstring(QString &str);

	void EvalForce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force);

	void constrictsurface_point(Tvertex &posit);
	void constrictsurface_vector(const Tvertex &posit, Tvector &vc);
	void integrate(Tvertex &posit, Tvector &speed, double mass, double charge, double timeJDstart, double timejump);//updates posit & speed
	void integrateforce(Tvertex &posit, double mass, double charge, double timeJDstart, double timejump);//updates posit

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

};
