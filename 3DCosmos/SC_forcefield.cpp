#include "stdafx.h"
#include "vecmath.h"

#include "SC_forcefield.h"
#include "SC_env.h"
#include "SC_script.h"
#include "SC_expr.h"
#include "SC_customfunc.h"

#include "SC_functor.h"

#define CLASSPATH_FORCE_GRAVITY _text("Gravity")
#define CLASSPATH_FORCE_ELECTROMAGNETIC _text("Electromagnetic")
#define CLASSPATH_FORCE_HARMOSCILLATOR _text("Harmonic oscillator")
#define CLASSPATH_FORCE_GEO _text("Geometric")
#define CLASSPATH_FORCE_MISC _text("Miscellaneous")


///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp
///////////////////////////////////////////////////////////////////////////

Tintegratorcomp::Tintegratorcomp()
{
}

Tintegratorcomp::~Tintegratorcomp()
{
}


///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_centralgravity
///////////////////////////////////////////////////////////////////////////

class Tintegratorcomp_centralgravity : public Tintegratorcomp
{
private:
	Tvertex pt;
	double strength;
	double tmp_sz;
public:
	Tintegratorcomp_centralgravity(Tvertex &ipt, double istrength)
	{
		pt=ipt;
		strength=istrength;
	}
	virtual StrPtr G_typename() { return _qstr("CentralGravity"); }
	virtual Tintegratorcomp* duplicate() { return new Tintegratorcomp_centralgravity(pt,strength); }
	virtual void tostring(QString &str, int tpe)
	{
		QString st1;
		str=G_typename();str+=_qstr("{");
		pt.tostring(st1,0);str+=st1;
		str+=_qstr(",");
		st1.formatscalar(strength);str+=st1;
		str+=_qstr("}");
	}
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
	{
		force.lincombfrom(+1,&pt,-1,&posit);
		tmp_sz=force.G_size();
		if (tmp_sz>0) force.mul(mass*strength/(tmp_sz*tmp_sz*tmp_sz));
	}
};


///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_parallelgravity
///////////////////////////////////////////////////////////////////////////

class Tintegratorcomp_parallelgravity : public Tintegratorcomp
{
private:
	Tvector dir;
public:
	Tintegratorcomp_parallelgravity(Tvector idir)
	{
		dir=idir;
	}
	virtual StrPtr G_typename() { return _qstr("ParallelGravity"); }
	virtual Tintegratorcomp* duplicate() { return new Tintegratorcomp_parallelgravity(dir); }
	virtual void tostring(QString &str, int tpe)
	{
		QString st1;
		str=G_typename();str+=_qstr("{");
		dir.tostring(st1,0);str+=st1;
		str+=_qstr("}");
	}
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
	{
		force.mulfrom(&dir,mass);
	}
};


///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_sphericalharmonicoscillator
///////////////////////////////////////////////////////////////////////////

class Tintegratorcomp_sphericalharmonicoscillator : public Tintegratorcomp
{
private:
	Tvertex pt;
	double strength;
	double tmp_sz;
public:
	Tintegratorcomp_sphericalharmonicoscillator(Tvertex &ipt, double istrength)
	{
		pt=ipt;
		strength=istrength;
	}
	virtual StrPtr G_typename() { return _qstr("SphericalHarmonicOscillator"); }
	virtual Tintegratorcomp* duplicate() { return new Tintegratorcomp_sphericalharmonicoscillator(pt,strength); }
	virtual void tostring(QString &str, int tpe)
	{
		QString st1;
		str=G_typename();str+=_qstr("{");
		pt.tostring(st1,0);str+=st1;
		str+=_qstr(",");
		st1.formatscalar(strength);str+=st1;
		str+=_qstr("}");
	}
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
	{
		force.lincombfrom(+1,&pt,-1,&posit);
		tmp_sz=force.G_size();
		force.mul(strength);
	}
};


///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_cylindricalharmonicoscillator
///////////////////////////////////////////////////////////////////////////

class Tintegratorcomp_cylindricalharmonicoscillator : public Tintegratorcomp
{
private:
	Tvertex pt;
	Tvector dir;
	double strength;
	Tvector tmp_v1;
	double tmp_sz;
public:
	Tintegratorcomp_cylindricalharmonicoscillator(Tvertex &ipt, Tvector &idir, double istrength)
	{
		pt=ipt;
		dir.normfrom(&idir);
		strength=istrength;
	}
	virtual StrPtr G_typename() { return _qstr("CylindricalHarmonicOscillator"); }
	virtual Tintegratorcomp* duplicate() { return new Tintegratorcomp_cylindricalharmonicoscillator(pt,dir,strength); }
	virtual void tostring(QString &str, int tpe)
	{
		QString st1;
		str=G_typename();str+=_qstr("{");
		pt.tostring(st1,0);str+=st1;
		str+=_qstr(",");
		dir.tostring(st1,0);str+=st1;
		str+=_qstr("}");
	}
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
	{
		tmp_v1.subtrvertices(&posit,&pt);
		force.projectfrom(&tmp_v1,&dir);
		tmp_sz=force.G_size();
		force.mul(-strength);
	}
};


///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_linearharmonicoscillator
///////////////////////////////////////////////////////////////////////////

class Tintegratorcomp_linearharmonicoscillator : public Tintegratorcomp
{
private:
	Tvertex pt;
	Tvector dir;
	double strength;
	Tvector tmp_v1;
	double tmp_sz;
public:
	Tintegratorcomp_linearharmonicoscillator(Tvertex &ipt, Tvector &idir, double istrength)
	{
		pt=ipt;
		dir.normfrom(&idir);
		strength=istrength;
	}
	virtual StrPtr G_typename() { return _qstr("LinearHarmonicOscillator"); }
	virtual Tintegratorcomp* duplicate() { return new Tintegratorcomp_linearharmonicoscillator(pt,dir,strength); }
	virtual void tostring(QString &str, int tpe)
	{
		QString st1;
		str=G_typename();str+=_qstr("{");
		pt.tostring(st1,0);str+=st1;
		str+=_qstr(",");
		dir.tostring(st1,0);str+=st1;
		str+=_qstr("}");
	}
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
	{
		tmp_v1.subtrvertices(&posit,&pt);
		double tmp_sz=Tvector::dotprod(&tmp_v1,&dir);
		force.mulfrom(&dir,-strength*tmp_sz);
	}
};


///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_centrifugal
///////////////////////////////////////////////////////////////////////////

class Tintegratorcomp_centrifugal : public Tintegratorcomp
{
private:
	Tvertex pt;
	Tvector rotationvector;
	Tvector tmpv1,tmpv2;
public:
	Tintegratorcomp_centrifugal(Tvertex &ipt, Tvector &irotationvector)
	{
		pt=ipt;
		rotationvector=irotationvector;
	}
	virtual StrPtr G_typename() { return _qstr("Centrifugal"); }
	virtual Tintegratorcomp* duplicate() { return new Tintegratorcomp_centrifugal(pt,rotationvector); }
	virtual void tostring(QString &str, int tpe)
	{
		QString st1;
		str=G_typename();str+=_qstr("{");
		pt.tostring(st1,0);str+=st1;
		str+=_qstr(",");
		rotationvector.tostring(st1,0);str+=st1;
		str+=_qstr("}");
	}
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
	{
		tmpv1.subtrvertices(&posit,&pt);
		tmpv2.vecprod(&rotationvector,&tmpv1);
		force.vecprod(&rotationvector,&tmpv2);
		force.mul(-mass);
	}
};


///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_coriolis
///////////////////////////////////////////////////////////////////////////

class Tintegratorcomp_coriolis : public Tintegratorcomp
{
private:
	Tvertex pt;
	Tvector rotationvector;
public:
	Tintegratorcomp_coriolis(Tvertex &ipt, Tvector &irotationvector)
	{
		pt=ipt;
		rotationvector=irotationvector;
	}
	virtual StrPtr G_typename() { return _qstr("Coriolis"); }
	virtual Tintegratorcomp* duplicate() { return new Tintegratorcomp_coriolis(pt,rotationvector); }
	virtual void tostring(QString &str, int tpe)
	{
		QString st1;
		str=G_typename();str+=_qstr("{");
		pt.tostring(st1,0);str+=st1;
		str+=_qstr(",");
		rotationvector.tostring(st1,0);str+=st1;
		str+=_qstr("}");
	}
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
	{
		force.vecprod(&rotationvector,&speed);
		force.mul(-2*mass);
	}
};



///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_electricpointcharge
///////////////////////////////////////////////////////////////////////////

class Tintegratorcomp_electricpointcharge : public Tintegratorcomp
{
private:
	Tvertex pt;
	double strength;
	double tmp_sz;
public:
	Tintegratorcomp_electricpointcharge(Tvertex &ipt, double istrength)
	{
		pt=ipt;
		strength=istrength;
	}
	virtual StrPtr G_typename() { return _qstr("ElectricPointCharge"); }
	virtual Tintegratorcomp* duplicate() { return new Tintegratorcomp_electricpointcharge(pt,strength); }
	virtual void tostring(QString &str, int tpe)
	{
		QString st1;
		str=G_typename();str+=_qstr("{");
		pt.tostring(st1,0);str+=st1;
		str+=_qstr(",");
		st1.formatscalar(strength);str+=st1;
		str+=_qstr("}");
	}
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
	{
		force.lincombfrom(+1,&pt,-1,&posit);
		tmp_sz=force.G_size();
		if (tmp_sz>0) force.mul(charge*strength/(tmp_sz*tmp_sz*tmp_sz));
	}
};



///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_electricdipole
///////////////////////////////////////////////////////////////////////////

class Tintegratorcomp_electricdipole : public Tintegratorcomp
{
private:
	Tvertex ps;
	Tvector dir;
	Tvector tmp_vc1;
	double tmp_sz,tmp_fc;
public:
	Tintegratorcomp_electricdipole(Tvertex ips, Tvector &idir)
	{
		ps=ips;
		dir=idir;
	}
	virtual StrPtr G_typename() { return _qstr("ElectricDipole"); }
	virtual Tintegratorcomp* duplicate() { return new Tintegratorcomp_electricdipole(ps,dir); }
	virtual void tostring(QString &str, int tpe)
	{
		QString st1;
		str=G_typename();str+=_qstr("{");
		ps.tostring(st1,0);str+=st1;
		str+=_qstr(",");
		dir.tostring(st1,0);str+=st1;
		str+=_qstr("}");
	}
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
	{
		tmp_vc1.lincombfrom(+1,&posit,-1,&ps);
		tmp_sz=tmp_vc1.G_size();
		if (tmp_sz<=0) { force.copyfrom(0,0,0); return; }
		tmp_fc=3*Tvector::dotprod(&dir,&tmp_vc1)/(tmp_sz*tmp_sz);
		force.lincombfrom(tmp_fc,&tmp_vc1,-1,&dir);
		force.mul(charge/(tmp_sz*tmp_sz*tmp_sz));
	}
};



///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_magneticdipole
///////////////////////////////////////////////////////////////////////////

class Tintegratorcomp_magneticdipole : public Tintegratorcomp
{
private:
	Tvertex ps;
	Tvector dir;
	Tvector tmp_vc1,tmp_vc2;
	double tmp_sz,tmp_fc;
public:
	Tintegratorcomp_magneticdipole(Tvertex ips, Tvector &idir)
	{
		ps=ips;
		dir=idir;
	}
	virtual StrPtr G_typename() { return _qstr("MagneticDipole"); }
	virtual Tintegratorcomp* duplicate() { return new Tintegratorcomp_magneticdipole(ps,dir); }
	virtual void tostring(QString &str, int tpe)
	{
		QString st1;
		str=G_typename();str+=_qstr("{");
		ps.tostring(st1,0);str+=st1;
		str+=_qstr(",");
		dir.tostring(st1,0);str+=st1;
		str+=_qstr("}");
	}
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
	{
		tmp_vc1.lincombfrom(+1,&posit,-1,&ps);
		tmp_sz=tmp_vc1.G_size();
		if (tmp_sz<=0) { force.copyfrom(0,0,0); return; }
		tmp_fc=3*Tvector::dotprod(&dir,&tmp_vc1)/(tmp_sz*tmp_sz);
		tmp_vc2.lincombfrom(tmp_fc,&tmp_vc1,-1,&dir);
		tmp_vc2.mul(charge/(tmp_sz*tmp_sz*tmp_sz));
		force.vecprod(&speed,&tmp_vc2);
	}
};



///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_lineardrag
///////////////////////////////////////////////////////////////////////////

class Tintegratorcomp_lineardrag : public Tintegratorcomp
{
private:
	double strength;
public:
	Tintegratorcomp_lineardrag(double istrength)
	{
		strength=istrength;
	}
	virtual StrPtr G_typename() { return _qstr("LinearDrag"); }
	virtual Tintegratorcomp* duplicate() { return new Tintegratorcomp_lineardrag(strength); }
	virtual void tostring(QString &str, int tpe)
	{
		QString st1;
		str=G_typename();str+=_qstr("{");
		st1.formatscalar(strength);str+=st1;
		str+=_qstr("}");
	}
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
	{
		force.mulfrom(&speed,-strength);
	}
};


///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_quadraticdrag
///////////////////////////////////////////////////////////////////////////

class Tintegratorcomp_quadraticdrag : public Tintegratorcomp
{
private:
	double strength;
public:
	Tintegratorcomp_quadraticdrag(double istrength)
	{
		strength=istrength;
	}
	virtual StrPtr G_typename() { return _qstr("QuadraticDrag"); }
	virtual Tintegratorcomp* duplicate() { return new Tintegratorcomp_quadraticdrag(strength); }
	virtual void tostring(QString &str, int tpe)
	{
		QString st1;
		str=G_typename();str+=_qstr("{");
		st1.formatscalar(strength);str+=st1;
		str+=_qstr("}");
	}
	virtual void evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
	{
		force.mulfrom(&speed,-strength*speed.G_size());
	}
};



///////////////////////////////////////////////////////////////////////////
// Tintegratorcomp_functor
///////////////////////////////////////////////////////////////////////////

void Tintegratorcomp_functor::tostring(QString &str, int tpe)
{
	str=G_typename();
}

void Tintegratorcomp_functor::evalforce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
{
	TSC_functor &functor=functorref.G_obj();

	try{
		TSC_funcarglist arglist(NULL);
		TSC_value retval0,arg1,arg2,arg3,arg4,arg5;

		arg1.createtype(GetTSCenv().G_datatype(SC_valname_vertex));arglist.add(&arg1);
		*(G_valuecontent<Tvertex>(&arg1))=posit;

		arg2.createtype(GetTSCenv().G_datatype(SC_valname_vector));arglist.add(&arg2);
		*(G_valuecontent<Tvector>(&arg2))=speed;

		arg3.createtype(GetTSCenv().G_datatype(SC_valname_scalar));arglist.add(&arg3);
		arg3.G_content_scalar()->copyfrom(mass);

		arg4.createtype(GetTSCenv().G_datatype(SC_valname_scalar));arglist.add(&arg4);
		arg4.G_content_scalar()->copyfrom(charge);

		arg5.createtype(GetTSCenv().G_datatype(SC_valname_scalar));arglist.add(&arg5);
		arg5.G_content_scalar()->copyfrom(timeJD);


		functor.eval(&arglist,&retval0);
		force=*(G_valuecontent<Tvector>(&retval0));
	}
	catch(TSC_runerror err)
	{
		QString errstring;
		FormatString(errstring,_text("Functor integrator component: ^1"),err.G_content());
		throw QError(errstring);
	}
	catch(QError err)
	{
		QString errstring;
		FormatString(errstring,_text("Functor integrator component: ^1"),err.G_content());
		throw QError(errstring);
	}

}




///////////////////////////////////////////////////////////////////////////
// TSC_forcefield
///////////////////////////////////////////////////////////////////////////


TSC_forcefield::TSC_forcefield()
{
	maxtimestep=0.01;
	maxspacestep=0.01;
	constrictsurf_type=0;
}

TSC_forcefield::~TSC_forcefield()
{
}


void TSC_forcefield::SetAccuracy(double imaxtimestep, double imaxspacestep)
{
	maxtimestep=imaxtimestep;
	maxspacestep=imaxspacestep;
}

void TSC_forcefield::Set_constrict_sphere(Tvertex &posit, double radius)
{
	constrictsurf_type=1;
	constrictsphere_center=posit;
	constrictsphere_radius=radius;
}


void TSC_forcefield::tostring(QString &str, int tpe)
{
	str.clear();
	for (int i=0; i<components.G_count(); i++)
	{
		QString st;
		components[i]->tostring(st,tpe);
		if (i>0) str+=_qstr("; ");
		str+=st;
	}
}

void TSC_forcefield::fromstring(StrPtr str)
{
	ASSERT(false);
}


void TSC_forcefield::operator=(const TSC_forcefield &v)
{
	copyfrom(v);
}

void TSC_forcefield::copyfrom(const TSC_forcefield &v)
{
	components.reset();
	for (int i=0; i<v.components.G_count(); i++)
		components.add(v.components[i]->duplicate());

	maxtimestep=v.maxtimestep;
	maxspacestep=v.maxspacestep;

	constrictsurf_type=v.constrictsurf_type;
	constrictsphere_center=v.constrictsphere_center;
	constrictsphere_radius=v.constrictsphere_radius;


}

void TSC_forcefield::constrictsurface_point(Tvertex &posit)
{
	if (constrictsurf_type==1)//constrict to sphere
	{
		Tvector normdir;normdir.lincombfrom(+1,&posit,-1,&constrictsphere_center);normdir.normfrom(&normdir);
		posit.lincombfrom(+1.0,&constrictsphere_center,constrictsphere_radius,&normdir);
	}
}

void TSC_forcefield::constrictsurface_vector(const Tvertex &posit, Tvector &vc)
{
	if (constrictsurf_type==1)//constrict to sphere
	{
		Tvector normdir;normdir.lincombfrom(+1,&posit,-1,&constrictsphere_center);normdir.normfrom(&normdir);
		Tvector vc2;
		vc2.projectfrom(&vc,&normdir);
		vc=vc2;
	}
}

void TSC_forcefield::EvalForce(const Tvertex &posit, const Tvector &speed, double mass, double charge, double timeJD, Tvector &force)
{
	force.copyfrom(0,0,0);

	Tvector force0;

	for (int i=0; i<components.G_count(); i++)
	{
		components[i]->evalforce(posit,speed,mass,charge,timeJD,force0);
		force.lincombfrom(+1,&force,+1,&force0);
	}

	constrictsurface_vector(posit,force);

	if (constrictsurf_type==1)//add reaction force to constrict to sphere
	{
		//add centripetal force to correct speed
		Tvector normdir;normdir.lincombfrom(+1,&posit,-1,&constrictsphere_center);normdir.normfrom(&normdir);
		double distcent=Tvertex::distance(&posit,&constrictsphere_center);
		double speedsize=speed.G_size();
		force.lincombfrom(1.0,&force,-speedsize*speedsize/distcent,&normdir);
	}
}


void TSC_forcefield::integrate(Tvertex &posit, Tvector &speed, double mass, double charge, double timeJDstart, double timejump)
{
	Tvector force;
	
/*	//simplest, naive implementation
	EvalForce(posit,speed,mass,charge,timeJD,force);
	speed.lincombfrom(+1,&speed,0.5*timestep/mass,&force);
	posit.lincombfrom(+1,&posit,timestep,&speed);
	EvalForce(posit,speed,mass,charge,timeJD,force);
	speed.lincombfrom(+1,&speed,0.5*timestep/mass,&force);
	return;*/

	double timeJD=timeJDstart;
	double totincr=0;

	constrictsurface_point(posit);
	constrictsurface_vector(posit,speed);

	
	bool completed=false;
	while (!completed)
	{
		double timestep=maxtimestep;

		double speedsize=speed.G_size();
		if (speedsize>0)
		{
			double maxspeedtimestep=maxspacestep/speedsize;
			if (maxspeedtimestep<timestep) timestep=maxspeedtimestep;
		}

		if (totincr+timestep>timejump) {timestep=timejump-totincr; completed=true; }

		double timestepJD=timestep/(24*60*60);

		//Runge-Kutta degree 4
		Tvector posit_1,posit_2,posit_3,posit_4;
		Tvector speed_1,speed_2,speed_3,speed_4;
		Tvertex posit_t;
		Tvector speed_t;

		EvalForce(posit,speed,mass,charge,timeJD,force);
		posit_1=speed;
		speed_1=force;

		posit_t.lincombfrom(+1,&posit,0.5*timestep,&posit_1);
		speed_t.lincombfrom(+1,&speed,0.5*timestep,&speed_1);
		EvalForce(posit_t,speed_t,mass,charge,timeJD+0.5*timestepJD,force);
		posit_2.lincombfrom(+1,&speed,0.5*timestep,&speed_1);
		speed_2=force;

		posit_t.lincombfrom(+1,&posit,0.5*timestep,&posit_2);
		speed_t.lincombfrom(+1,&speed,0.5*timestep,&speed_2);
		EvalForce(posit_t,speed_t,mass,charge,timeJD+0.5*timestepJD,force);
		posit_3.lincombfrom(+1,&speed,0.5*timestep,&speed_2);
		speed_3=force;

		posit_t.lincombfrom(+1,&posit,timestep,&posit_3);
		speed_t.lincombfrom(+1,&speed,timestep,&speed_3);
		EvalForce(posit_t,speed_t,mass,charge,timeJD+timestepJD,force);
		posit_4.lincombfrom(+1,&speed,timestep,&speed_3);
		speed_4=force;

		posit.lincombfrom(+1,&posit,timestep*1/6,&posit_1);
		posit.lincombfrom(+1,&posit,timestep*2/6,&posit_2);
		posit.lincombfrom(+1,&posit,timestep*2/6,&posit_3);
		posit.lincombfrom(+1,&posit,timestep*1/6,&posit_4);

		speed.lincombfrom(+1,&speed,timestep*1/6,&speed_1);
		speed.lincombfrom(+1,&speed,timestep*2/6,&speed_2);
		speed.lincombfrom(+1,&speed,timestep*2/6,&speed_3);
		speed.lincombfrom(+1,&speed,timestep*1/6,&speed_4);

		constrictsurface_point(posit);
		constrictsurface_vector(posit,speed);

		totincr+=timestep;
		timeJD+=timestepJD;
	}
}


void TSC_forcefield::integrateforce(Tvertex &posit, double mass, double charge, double timeJDstart, double timejump)//updates posit
{
	Tvector force;
	
	double timeJD=timeJDstart;
	double totincr=0;

	constrictsurface_point(posit);
	
	bool completed=false;
	while (!completed)
	{
		double timestep=maxtimestep;

		if (totincr+timestep>timejump) {timestep=timejump-totincr; completed=true; }

		double timestepJD=timestep/(24*60*60);

		//Runge-Kutta degree 4
		Tvector posit_1,posit_2,posit_3,posit_4;
		Tvertex posit_t;
		Tvector spd0(0,0,0);

		EvalForce(posit,spd0,mass,charge,timeJD,force);force.normfrom(&force);
		posit_1=force;

		posit_t.lincombfrom(+1,&posit,0.5*timestep,&posit_1);
		EvalForce(posit_t,spd0,mass,charge,timeJD+0.5*timestepJD,force);force.normfrom(&force);
		posit_2=force;

		posit_t.lincombfrom(+1,&posit,0.5*timestep,&posit_2);
		EvalForce(posit_t,spd0,mass,charge,timeJD+0.5*timestepJD,force);force.normfrom(&force);
		posit_3=force;

		posit_t.lincombfrom(+1,&posit,timestep,&posit_3);
		EvalForce(posit_t,spd0,mass,charge,timeJD+timestepJD,force);force.normfrom(&force);
		posit_4=force;

		posit.lincombfrom(+1,&posit,timestep*1/6,&posit_1);
		posit.lincombfrom(+1,&posit,timestep*2/6,&posit_2);
		posit.lincombfrom(+1,&posit,timestep*2/6,&posit_3);
		posit.lincombfrom(+1,&posit,timestep*1/6,&posit_4);

		constrictsurface_point(posit);

		totincr+=timestep;
		timeJD+=timestepJD;
	}
}

void TSC_forcefield::streamout(QBinTagWriter &writer)
{
	ASSERT(false);
}
void TSC_forcefield::streamin(QBinTagReader &reader)
{
	ASSERT(false);
}



/*
///////////////////////////////////////////////////////////////////////////////////
FUNCTION(functor_integrator,Integrator)
{
	setreturntype(SC_valname_forcefield);
	//setflexiblearglist();
	setclasspath_fromtype(SC_valname_forcefield);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->create();
}
ENDFUNCTION(functor_integrator)
*/


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_setaccuracy,SetAccuracy)
{
	setmemberfunction(SC_valname_forcefield);
	addvar(_qstr("MaxTimeStep"),SC_valname_scalar);
	addvar(_qstr("MaxSpaceStep"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double maxtimestep=arglist->get(0)->G_content_scalar()->G_val();
	double maxspacestep=arglist->get(1)->G_content_scalar()->G_val();
	G_valuecontent<TSC_forcefield>(owner)->SetAccuracy(maxtimestep,maxspacestep);
}
ENDFUNCTION(func_integrator_setaccuracy)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_restricttosphere,RestrictToSphere)
{
	setmemberfunction(SC_valname_forcefield);
	addvar(_qstr("Center"),SC_valname_vertex);
	addvar(_qstr("Radius"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *posit=G_valuecontent<Tvertex>(arglist->get(0));
	double radius=arglist->get(1)->G_content_scalar()->G_val();
	G_valuecontent<TSC_forcefield>(owner)->Set_constrict_sphere(*posit,radius);
}
ENDFUNCTION(func_integrator_restricttosphere)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_evalforce,EvalForce)
{
	setmemberfunction(SC_valname_forcefield);
	setreturntype(SC_valname_vector);

	addvar(_qstr("Position"),SC_valname_vertex);
	addvar(_qstr("Speed"),SC_valname_vector);
	addvar(_qstr("Mass"),SC_valname_scalar);
	addvar(_qstr("Charge"),SC_valname_scalar);
	addvar(_qstr("Time"),SC_valname_time);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *posit=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *speed=G_valuecontent<Tvector>(arglist->get(1));
	double mass=arglist->get(2)->G_content_scalar()->G_val();
	double charge=arglist->get(3)->G_content_scalar()->G_val();
	TSC_time *tm=G_valuecontent<TSC_time>(arglist->get(4));

	Tvector *force=G_valuecontent<Tvector>(retval);

	G_valuecontent<TSC_forcefield>(owner)->EvalForce(*posit,*speed,mass,charge,tm->G_JD(),*force);
}
ENDFUNCTION(func_integrator_evalforce)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_eval,Integrate)
{
	setmemberfunction(SC_valname_forcefield);
//	setreturntype(SC_valname_vector);

	addvar(_qstr("Position"),SC_valname_vertex);
	addvar(_qstr("Speed"),SC_valname_vector);
	addvar(_qstr("Mass"),SC_valname_scalar);
	addvar(_qstr("Charge"),SC_valname_scalar);
	addvar(_qstr("Time"),SC_valname_time);
	addvar(_qstr("TimeStep"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *posit=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *speed=G_valuecontent<Tvector>(arglist->get(1));
	double mass=arglist->get(2)->G_content_scalar()->G_val();
	double charge=arglist->get(3)->G_content_scalar()->G_val();
	TSC_time *tm=G_valuecontent<TSC_time>(arglist->get(4));
	double timestep=arglist->get(5)->G_content_scalar()->G_val();

	G_valuecontent<TSC_forcefield>(owner)->integrate(*posit,*speed,mass,charge,tm->G_JD(),timestep);

}
ENDFUNCTION(func_integrator_eval)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_integrateforce,IntegrateForce)
{
	setmemberfunction(SC_valname_forcefield);
	setreturntype(SC_valname_vector);

	addvar(_qstr("Position"),SC_valname_vertex);
	addvar(_qstr("Mass"),SC_valname_scalar);
	addvar(_qstr("Charge"),SC_valname_scalar);
	addvar(_qstr("Time"),SC_valname_time);
	addvar(_qstr("TimeStep"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *posit=G_valuecontent<Tvertex>(arglist->get(0));
	double mass=arglist->get(1)->G_content_scalar()->G_val();
	double charge=arglist->get(2)->G_content_scalar()->G_val();
	TSC_time *tm=G_valuecontent<TSC_time>(arglist->get(3));
	double timestep=arglist->get(4)->G_content_scalar()->G_val();

	G_valuecontent<TSC_forcefield>(owner)->integrateforce(*posit,mass,charge,tm->G_JD(),timestep);

}
ENDFUNCTION(func_integrator_integrateforce)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_addparallelgravity,AddParallelGravity)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_GRAVITY);
	addvar(_qstr("Direction"),SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvector *dir=G_valuecontent<Tvector>(arglist->get(0));
	Tintegratorcomp_parallelgravity *comp=new Tintegratorcomp_parallelgravity(*dir);
	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);

}
ENDFUNCTION(func_integrator_addparallelgravity)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_addcentralgravity,AddCentralGravity)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_GRAVITY);
	addvar(_qstr("Point"),SC_valname_vertex);
	addvar(_qstr("Strength"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	double strength=arglist->get(1)->G_content_scalar()->G_val();
	Tintegratorcomp_centralgravity *comp=new Tintegratorcomp_centralgravity(*pt,strength);
	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);

}
ENDFUNCTION(func_integrator_addcentralgravity)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_electricpointcharge,AddElectricPointCharge)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_ELECTROMAGNETIC);
	addvar(_qstr("Point"),SC_valname_vertex);
	addvar(_qstr("Strength"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	double strength=arglist->get(1)->G_content_scalar()->G_val();
	Tintegratorcomp_electricpointcharge *comp=new Tintegratorcomp_electricpointcharge(*pt,strength);
	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);

}
ENDFUNCTION(func_integrator_electricpointcharge)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_electricdipole,AddElectricDipole)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_ELECTROMAGNETIC);
	addvar(_qstr("Position"),SC_valname_vertex);
	addvar(_qstr("Moment"),SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *dir=G_valuecontent<Tvector>(arglist->get(1));
	Tintegratorcomp_electricdipole *comp=new Tintegratorcomp_electricdipole(*pt,*dir);
	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);

}
ENDFUNCTION(func_integrator_electricdipole)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_magneticdipole,AddMagneticDipole)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_ELECTROMAGNETIC);
	addvar(_qstr("Position"),SC_valname_vertex);
	addvar(_qstr("Moment"),SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *dir=G_valuecontent<Tvector>(arglist->get(1));
	Tintegratorcomp_magneticdipole *comp=new Tintegratorcomp_magneticdipole(*pt,*dir);
	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);

}
ENDFUNCTION(func_integrator_magneticdipole)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_sphericalharmonicoscillator,AddSphericalHarmonicOscillator)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_HARMOSCILLATOR);
	addvar(_qstr("Point"),SC_valname_vertex);
	addvar(_qstr("Strength"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	double strength=arglist->get(1)->G_content_scalar()->G_val();
	Tintegratorcomp_sphericalharmonicoscillator *comp=new Tintegratorcomp_sphericalharmonicoscillator(*pt,strength);
	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);

}
ENDFUNCTION(func_integrator_sphericalharmonicoscillator)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_cylharmonicoscillator,AddCylindricalHarmonicOscillator)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_HARMOSCILLATOR);
	addvar(_qstr("Point"),SC_valname_vertex);
	addvar(_qstr("Direction"),SC_valname_vector);
	addvar(_qstr("Strength"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *dir=G_valuecontent<Tvector>(arglist->get(1));
	double strength=arglist->get(2)->G_content_scalar()->G_val();
	Tintegratorcomp_cylindricalharmonicoscillator *comp=new Tintegratorcomp_cylindricalharmonicoscillator(*pt,*dir,strength);
	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);

}
ENDFUNCTION(func_integrator_cylharmonicoscillator)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_linearharmonicoscillator,AddLinearHarmonicOscillator)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_HARMOSCILLATOR);
	addvar(_qstr("Point"),SC_valname_vertex);
	addvar(_qstr("Direction"),SC_valname_vector);
	addvar(_qstr("Strength"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *dir=G_valuecontent<Tvector>(arglist->get(1));
	double strength=arglist->get(2)->G_content_scalar()->G_val();
	Tintegratorcomp_linearharmonicoscillator *comp=new Tintegratorcomp_linearharmonicoscillator(*pt,*dir,strength);
	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);

}
ENDFUNCTION(func_integrator_linearharmonicoscillator)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_centrifugal,AddCentrifugal)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_GEO);
	addvar(_qstr("Point"),SC_valname_vertex);
	addvar(_qstr("Direction"),SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *dir=G_valuecontent<Tvector>(arglist->get(1));
	Tintegratorcomp_centrifugal *comp=new Tintegratorcomp_centrifugal(*pt,*dir);
	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);

}
ENDFUNCTION(func_integrator_centrifugal)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_coriolis,AddCoriolis)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_GEO);
	addvar(_qstr("Point"),SC_valname_vertex);
	addvar(_qstr("Direction"),SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *dir=G_valuecontent<Tvector>(arglist->get(1));
	Tintegratorcomp_coriolis *comp=new Tintegratorcomp_coriolis(*pt,*dir);
	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);

}
ENDFUNCTION(func_integrator_coriolis)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_lineardrag,AddLinearDrag)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_MISC);
	addvar(_qstr("Strength"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double strength=arglist->get(0)->G_content_scalar()->G_val();
	Tintegratorcomp_lineardrag *comp=new Tintegratorcomp_lineardrag(strength);
	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);
}
ENDFUNCTION(func_integrator_lineardrag)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_quadraticdrag,AddQuadraticDrag)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_MISC);
	addvar(_qstr("Strength"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double strength=arglist->get(0)->G_content_scalar()->G_val();
	Tintegratorcomp_quadraticdrag *comp=new Tintegratorcomp_quadraticdrag(strength);
	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);

}
ENDFUNCTION(func_integrator_quadraticdrag)




///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_integrator_addfunctor,AddCustomForce)
{
	setmemberfunction(SC_valname_forcefield);
	extendclasspath(CLASSPATH_FORCE_MISC);
	addvar(_qstr("function"),SC_valname_functor);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_functor *fnc=G_valuecontent<TSC_functor>(arglist->get(0));

	Tintegratorcomp_functor *comp=new Tintegratorcomp_functor(*fnc);

	G_valuecontent<TSC_forcefield>(owner)->addcomp(comp);

}
ENDFUNCTION(func_integrator_addfunctor)


