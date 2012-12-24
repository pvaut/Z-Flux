#include "stdafx.h"
#include "qfile.h"
#include "3DScene.h"
#include "3Dmotion.h"
#include "3DCurve.h"
#include "bezier.h"

#include "sc_functor.h"
#include "sc_script.h"

#include "astrotools_cyclorbit.h"


//*********************************************************************
// T3DMotion
//*********************************************************************

T3DMotion::T3DMotion()
{
	parentlist=NULL;
	cosmos=&T3DCosmos::Get();

	param_readonly(ObjNameStr,true);
	param_setvalue(ObjNameStr,_text("Motion"));

	isinmotioncatalog=false;

/*	//generate unique, default name
	QString nm,nm0;
	int nr=0;
	while (true)
	{
		nr++;
		nm="Motion_";nm+=QString(nr);
		bool found=false;
		for (int i=0; i<cosmos->G_motioncount(); i++)
		{
			cosmos->G_motion(i)->G_name(nm0);
			if (qstricmp(nm,nm0)==0) found=true;
		}
		if (!found) break;
	}
	param_setvalue(ObjNameStr,nm);*/
}

T3DMotion::~T3DMotion()
{
	if (isinmotioncatalog) cosmos->motioncatalog_remove(this);
}

void T3DMotion::addtomotioncatalog()
{
	isinmotioncatalog=true;
	cosmos->motioncatalog_add(this);
}

TObjectTreeItem* T3DMotion::G_parent()
{
	return parentlist;
}



//*******************************************************************
// T3DMotionFree
//*******************************************************************

T3DMotionFree::T3DMotionFree()
{
}

void T3DMotionFree::adjusttime(double timestep)
{
}

void T3DMotionFree::apply(TSolidObjectData *obj, double timestep)
{
	Tvertex pt;
	Tvector spd;
	obj->position->G_origin(&pt);
	obj->velocity->G_origin_vector(&spd);
	pt.lincombfrom(1,&pt,timestep,&spd);
	obj->position->Set_origin(&pt);
}

void T3DMotionFree::G_curposition(Tvertex &iposit)
{
	iposit=Tvertex(0,0,0);
}

void T3DMotionFree::G_curspeed(Tvector &ispeed)
{
	ispeed=Tvector(0,0,0);
}





//*******************************************************************
// T3DMotionRotate
//*******************************************************************

T3DMotionRotate::T3DMotionRotate()
{
	normdir=G_valuecontent<Tvector>(&addparam(_qstr("NormDir"),SC_valname_vector)->content);
	*normdir=Tvector(1,0,0);
	rotspeed=addparam(_qstr("RotSpeed"),SC_valname_scalar)->content.G_content_scalar();rotspeed->copyfrom(1.0);
	offsetangle=addparam(_qstr("OffsetAngle"),SC_valname_scalar)->content.G_content_scalar();

	timetype=&addparam(_qstr("TimeType"),SC_valname_timetype)->content;

	reftime=G_valuecontent<TSC_time>(&addparam(_qstr("RefTime"),SC_valname_time)->content);
	transformation=G_valuecontent<Taffinetransformation>(&addparam(_qstr("Transf"),SC_valname_affinetransformation)->content);
}

void T3DMotionRotate::adjusttime(double timestep)
{
	double speed=rotspeed->G_val();
	if (speed==0) return;
	if (timetype->toint()==TimeUT0)
	{
		double period=2*Pi/speed;
		double frac=cosmos->G_time()->diffanglefrac(*reftime,period);
		transformation->makerotation(normdir,frac*2*Pi+offsetangle->G_val());
	}
	if (timetype->toint()==TimeST0)
	{
		double ang=cosmos->G_time()->G_ST0_uniform()*speed/(2*Pi/(24*60*60));
		transformation->makerotation(normdir,ang+offsetangle->G_val());
	}
}

void T3DMotionRotate::apply(TSolidObjectData *obj, double timestep)
{
	*obj->position=*transformation;
}

void T3DMotionRotate::G_curposition(Tvertex &iposit)
{
	iposit=Tvertex(0,0,0);
}

void T3DMotionRotate::G_curspeed(Tvector &ispeed)
{
	ispeed=Tvector(0,0,0);
}



//*******************************************************************
// T3DMotionSwing
//*******************************************************************

T3DMotionSwing::T3DMotionSwing()
{
	normdir=G_valuecontent<Tvector>(&addparam(_qstr("NormDir"),SC_valname_vector)->content);
	*normdir=Tvector(1,0,0);

	period=addparam(_qstr("Period"),SC_valname_scalar)->content.G_content_scalar();period->copyfrom(1.0);
	swingangle=addparam(_qstr("SwingAngle"),SC_valname_scalar)->content.G_content_scalar();
	reftime=G_valuecontent<TSC_time>(&addparam(_qstr("RefTime"),SC_valname_time)->content);

	transformation=G_valuecontent<Taffinetransformation>(&addparam(_qstr("Transf"),SC_valname_affinetransformation)->content);
}

void T3DMotionSwing::adjusttime(double timestep)
{
	double periodval=period->G_val();

	
	double rotangle=2*Pi*cosmos->G_time()->diffanglefrac(*reftime,periodval);

	double angle=sin(rotangle)*swingangle->G_val();

	transformation->makerotation(normdir,angle);
}

void T3DMotionSwing::apply(TSolidObjectData *obj, double timestep)
{
	*obj->position=*transformation;
}

void T3DMotionSwing::G_curposition(Tvertex &iposit)
{
	iposit=Tvertex(0,0,0);
}

void T3DMotionSwing::G_curspeed(Tvector &ispeed)
{
	ispeed=Tvector(0,0,0);
}



//*******************************************************************
// T3DMotionKepler
//*******************************************************************

T3DMotionKepler::T3DMotionKepler()
{
	a=addparam(_qstr("SemiMajorAxis"),SC_valname_scalar)->content.G_content_scalar();a->copyfrom(1);
	e=addparam(_qstr("Eccentricity"),SC_valname_scalar)->content.G_content_scalar();
	i=addparam(_qstr("Inclination"),SC_valname_scalar)->content.G_content_scalar();
	O=addparam(_qstr("AscendingNode"),SC_valname_scalar)->content.G_content_scalar();
	w=addparam(_qstr("ArgPerigee"),SC_valname_scalar)->content.G_content_scalar();
	T=addparam(_qstr("Period"),SC_valname_scalar)->content.G_content_scalar();T->copyfrom(1);
	peritime=G_valuecontent<TSC_time>(&addparam(_qstr("PeriTime"),SC_valname_time)->content);
//	transformation=G_valuecontent<Taffinetransformation>(&addparam(_qstr("Transf"),SC_valname_affinetransformation)->content);
}

void T3DMotionKepler::solvekepler(double M, double& E, double ecc)
{
	if (ecc<0.6)
	{
		double Evoor;
		E=M;
		Evoor=E+1.0;
		while (fabs(E-Evoor)>0.0000001)
		{
			Evoor=E;
			E=M+ecc*sin(E);
		}
	}
	else
	{
		double E1,E2,vl;
		M=M-floor(M/(2*Pi))*2*Pi;
		E1=0;E2=2*Pi;
		while (E2-E1>1.0e-6)
		{
			E=(E1+E2)/2;
			vl=M+ecc*sin(E)-E;
			if (vl<0) E2=E; else E1=E;
		}
	}
}

void T3DMotionKepler::calcposit(const TSC_time& time)
{
	double r;
	double period=T->G_val();
	posit.M=(time.G_JD()-peritime->G_JD())/period*360;

	double eV=e->G_val();
	double aV=a->G_val();
	double OV=O->G_val();
	double iV=i->G_val();
	double wV=w->G_val();

	while (posit.M<0)   posit.M=posit.M+360;
	while (posit.M>360) posit.M=posit.M-360;
	posit.M=posit.M/180*Pi;
	solvekepler(posit.M,posit.E,eV);
	posit.v=2*atan2(sqrt(1+eV)*sin(posit.E/2),
		 		    sqrt(1-eV)*cos(posit.E/2));
	r=aV*(1-eV*cos(posit.E));

	posit.X=r*( cos(OV)*cos(wV+posit.v)-sin(OV)*cos(iV)*sin(wV+posit.v) );
	posit.Y=r*( sin(OV)*cos(wV+posit.v)+cos(OV)*cos(iV)*sin(wV+posit.v) );
	posit.Z=r*( sin(iV)*sin(wV+posit.v) );
	posit.r=r;
	position.Set3(posit.X,posit.Y,posit.Z);

	//calculate speed
	double E1=posit.E-Pi/1000;
	double E2=posit.E+Pi/1000;
	double vv;
	Tvertex pst1,pst2;

	vv=2*atan2(sqrt(1+eV)*sin(E1/2),sqrt(1-eV)*cos(E1/2));
	r=aV*(1-eV*cos(E1));
	pst1.S4_x(r*( cos(OV)*cos(wV+vv)-sin(OV)*cos(iV)*sin(wV+vv) ) );
	pst1.S4_y(r*( sin(OV)*cos(wV+vv)+cos(OV)*cos(iV)*sin(wV+vv) ) );
	pst1.S4_z(r*( sin(iV)*sin(wV+vv) ) );

	vv=2*atan2(sqrt(1+eV)*sin(E2/2),sqrt(1-eV)*cos(E2/2));
	r=aV*(1-eV*cos(E2));
	pst2.S4_x(r*( cos(OV)*cos(wV+vv)-sin(OV)*cos(iV)*sin(wV+vv) ) );
	pst2.S4_y(r*( sin(OV)*cos(wV+vv)+cos(OV)*cos(iV)*sin(wV+vv) ) );
	pst2.S4_z(r*( sin(iV)*sin(wV+vv) ) );

	speed.subtrvertices(&pst2,&pst1);
	speed.normfrom(&speed);

	vv=2*atan2(sqrt(1+eV)*sin(posit.E/2),sqrt(1-eV)*cos(posit.E/2));
	r=aV*(1-eV*cos(posit.E));
	double veloc=2*Pi*aV/period*sqrt(2*aV/r-1);

	speed.mulfrom(&speed,veloc/(24*60*60));
}


void T3DMotionKepler::adjusttime(double timestep)
{
	calcposit(*G_3DCosmos().G_time());
/*	double speed=rotspeed->G_val();
	if (speed==0) return;
	double period=2*Pi/speed;
	double frac=cosmos->G_time()->diffanglefrac(*reftime,period);
	transformation->makerotation(normdir,frac*2*Pi);*/
}

void T3DMotionKepler::apply(TSolidObjectData *obj, double timestep)
{
	obj->position->Set_origin(&position);
	obj->velocity->Set_origin_vector(&speed);
}

void T3DMotionKepler::G_curposition(Tvertex &iposit)
{
	iposit=position;
}

void T3DMotionKepler::G_curspeed(Tvector &ispeed)
{
	ispeed=speed;
}


void T3DMotionKepler::CreateCurve(T3DObjectCurve *crv, int resol)
{
	TSC_time tm;
	crv->reset();
	double period=T->G_val();
	for (int i=0; i<resol; i++)
	{
		tm=*peritime;
		tm.addseconds(i*1.0/resol*period*24*60*60);
		calcposit(tm);
		crv->addpoint(&position);
	}
	crv->close();
}




//*******************************************************************
// T3DMotionSpline
//*******************************************************************


T3DMotionSpline::T3DMotionSpline()
{
	spline=G_valuecontent<TPolyBezier3D>(&addparam(_qstr("Spline"),SC_valname_polybeziercurve)->content);
	reftime=G_valuecontent<TSC_time>(&addparam(_qstr("StartTime"),SC_valname_time)->content);
	timefactor=addparam(_qstr("TimeFactor"),SC_valname_scalar)->content.G_content_scalar();timefactor->copyfrom(1);
}




void T3DMotionSpline::eval(double JD, Tvertex &posit, Tvector &spd)
{
	double timediff=timefactor->G_val()*((JD-reftime->G_JD())*(24*60*60));
	if (!spline->G_isclosed())
	{
		if (timediff<spline->G_minxval()) timediff=spline->G_minxval();
		if (timediff>spline->G_maxxval()) timediff=spline->G_maxxval();
	}
	else
	{
		double T=spline->G_closed_T();
		if (timediff<0) timediff+=T*floor(-timediff/T);
		while (timediff<0) timediff+=T;
		timediff=timediff-T*floor(timediff/T);
	}
	spline->eval(timediff,posit);
	spline->evalder(timediff,spd);
}


void T3DMotionSpline::adjusttime(double timestep)
{
	eval(G_3DCosmos().G_time()->G_JD(),position,speed);
}

void T3DMotionSpline::apply(TSolidObjectData *obj, double timestep)
{
	obj->position->Set_origin(&position);
	obj->velocity->Set_origin_vector(&speed);
}

void T3DMotionSpline::G_curposition(Tvertex &iposit)
{
	iposit=position;
}

void T3DMotionSpline::G_curspeed(Tvector &ispeed)
{
	ispeed=speed;
}



//*******************************************************************
// T3DMotionTable
//*******************************************************************


T3DMotionTable::T3DMotionTable()
{
	interpoltype=&addparam(SC_valname_interpoltype,SC_valname_interpoltype,false,_text("Determines the table interpolation method"))->content;
}


void T3DMotionTable::reset()
{
	table_JD.reset();
	table_X.reset();
	table_Y.reset();
	table_Z.reset();
}

void T3DMotionTable::add(double iJD, double iX, double iY, double iZ)
{
	table_JD.add(iJD);
	table_X.add(iX);
	table_Y.add(iY);
	table_Z.add(iZ);
}


void T3DMotionTable::eval(double JD, Tvertex &posit, Tvector &spd)
{
	int k0,k1,k2,k;
	double h,a,b;

	int ipol=interpoltype->toint();

	k1=0;k2=table_JD.G_count()-1;
	while(k2-k1>1)
	{
		k=(k2+k1)>>1;
		if (table_JD[k]>JD) k2=k; else k1=k;
	}


	if (ipol==InterpolLinear)
	{
		h=table_JD[k2]-table_JD[k1];
		if (h==0.0) h=1;
		a=(table_JD[k2]-JD)/h;
		b=(JD-table_JD[k1])/h;

		posit.Set3(
			a*table_X[k1]+b*table_X[k2],
			a*table_Y[k1]+b*table_Y[k2],
			a*table_Z[k1]+b*table_Z[k2]
			);

		spd.Set(
			(table_X[k2]-table_X[k1])/(h*24*60*60),
			(table_Y[k2]-table_Y[k1])/(h*24*60*60),
			(table_Z[k2]-table_Z[k1])/(h*24*60*60)
			);

		return;
	}

	if (ipol==InterpolQuad)
	{
		if (k1%2!=1) { k1=k1+1; k2=k1+1; }
		k0=k1-1;

		if ((k0<0)||(k2>=table_JD.G_count()))
		{
			posit.Set3(0,0,0);
			spd.Set(0,0,0);
			return;
		}

		h=table_JD[k2]-table_JD[k1];
		if (h==0.0) h=1;
		double fr=(JD-table_JD[k1])/h;
		posit.Set3(
			table_X[k1]+fr*(table_X[k2]-table_X[k0])/2+fr*fr*(table_X[k2]+table_X[k0]-2*table_X[k1])/2,
			table_Y[k1]+fr*(table_Y[k2]-table_Y[k0])/2+fr*fr*(table_Y[k2]+table_Y[k0]-2*table_Y[k1])/2,
			table_Z[k1]+fr*(table_Z[k2]-table_Z[k0])/2+fr*fr*(table_Z[k2]+table_Z[k0]-2*table_Z[k1])/2
			);

		spd.Set(
			((table_X[k2]-table_X[k0])/2+2*fr*(table_X[k2]+table_X[k0]-2*table_X[k1])/2)/(h*24*60*60),
			((table_Y[k2]-table_Y[k0])/2+2*fr*(table_Y[k2]+table_Y[k0]-2*table_Y[k1])/2)/(h*24*60*60),
			((table_Z[k2]-table_Z[k0])/2+2*fr*(table_Z[k2]+table_Z[k0]-2*table_Z[k1])/2)/(h*24*60*60)
			);

		return;
	}


	throw QError(_qstr("Invalid interpolation method"));
}


void T3DMotionTable::adjusttime(double timestep)
{
	eval(G_3DCosmos().G_time()->G_JD(),position,speed);

}

void T3DMotionTable::apply(TSolidObjectData *obj, double timestep)
{
	obj->position->Set_origin(&position);
	obj->velocity->Set_origin_vector(&speed);
}

void T3DMotionTable::G_curposition(Tvertex &iposit)
{
	iposit=position;
}

void T3DMotionTable::G_curspeed(Tvector &ispeed)
{
	ispeed=speed;
}

void T3DMotionTable::CreateCurve(T3DObjectCurve *crv, int resol, const TSC_time &tm1, const TSC_time &tm2)
{
	double JD1=tm1.G_JD();
	double JD2=tm2.G_JD();
	crv->reset();
	for (int i=0; i<resol; i++)
	{
		Tvertex pst;Tvector spd;
		eval(JD1+(JD2-JD1)*i/resol,pst,spd);
		crv->addpoint(&pst);
	}
}

//////////////////////////////////////////////////////////////////////////////
// T3DMotionSolidFunctor
//////////////////////////////////////////////////////////////////////////////

T3DMotionSolidFunctor::T3DMotionSolidFunctor()
{
}

void T3DMotionSolidFunctor::set(TSC_functor *ifunctor)
{
	functorref.setreffor(ifunctor);
}

void T3DMotionSolidFunctor::adjusttime(double timestep)
{
	TSC_functor *functor=functorref.G_ref();
	if (functor==NULL) throw QError(_text("Functor not set"));

	try{
		TSC_funcarglist arglist(NULL);
		TSC_value retval0,arg;
		arg.createtype(GetTSCenv().G_datatype(SC_valname_time));
		arglist.add(&arg);

		*(G_valuecontent<TSC_time>(&arg))=*G_3DCosmos().G_time();
		functor->eval(&arglist,&retval0);
		position=*(G_valuecontent<Taffinetransformation>(&retval0));
	}
	catch(TSC_runerror err)
	{
		QString errstring;
		FormatString(errstring,_text("Custom solid motion: ^1"),err.G_content());
		throw QError(errstring);
	}
	catch(QError err)
	{
		QString errstring;
		FormatString(errstring,_text("Custom solid motion: ^1"),err.G_content());
		throw QError(errstring);
	}

}

void T3DMotionSolidFunctor::apply(TSolidObjectData *obj, double timestep)
{
	*(obj->position)=position;
}

void T3DMotionSolidFunctor::G_curposition(Tvertex &iposit)
{
	position.G_origin(&iposit);
}

void T3DMotionSolidFunctor::G_curspeed(Tvector &ispeed)
{
	ispeed.Set(0,0,0);
}



//////////////////////////////////////////////////////////////////////////////
// T3DMotionPointFunctor
//////////////////////////////////////////////////////////////////////////////

T3DMotionPointFunctor::T3DMotionPointFunctor()
{
}

void T3DMotionPointFunctor::set(TSC_functor *ifunctor)
{
	functorref.setreffor(ifunctor);
}

void T3DMotionPointFunctor::adjusttime(double timestep)
{
	TSC_functor *functor=functorref.G_ref();
	if (functor==NULL) throw QError(_text("Functor not set"));

	try{
		TSC_funcarglist arglist(NULL);
		TSC_value retval0,arg;
		arg.createtype(GetTSCenv().G_datatype(SC_valname_time));
		arglist.add(&arg);

		*(G_valuecontent<TSC_time>(&arg))=*G_3DCosmos().G_time();
		functor->eval(&arglist,&retval0);
		position=*(G_valuecontent<Tvertex>(&retval0));
	}
	catch(TSC_runerror err)
	{
		QString errstring;
		FormatString(errstring,_text("Custom point motion: ^1"),err.G_content());
		throw QError(errstring);
	}
	catch(QError err)
	{
		QString errstring;
		FormatString(errstring,_text("Custom point motion: ^1"),err.G_content());
		throw QError(errstring);
	}

}

void T3DMotionPointFunctor::apply(TSolidObjectData *obj, double timestep)
{
	obj->position->Set_origin(&position);
}

void T3DMotionPointFunctor::G_curposition(Tvertex &iposit)
{
	iposit=position;
}

void T3DMotionPointFunctor::G_curspeed(Tvector &ispeed)
{
	ispeed.Set(0,0,0);
}




//////////////////////////////////////////////////////////////////////////////
// T3DMotionForceField
//////////////////////////////////////////////////////////////////////////////

T3DMotionForceField::T3DMotionForceField()
{
	forcefield=G_valuecontent<TSC_forcefield>(&addparam(_qstr("ForceField"),SC_valname_forcefield)->content);
}


void T3DMotionForceField::adjusttime(double timestep)
{
	timeJDcurrent=G_3DCosmos().G_time()->G_JD();
}


void T3DMotionForceField::apply(TSolidObjectData *obj, double timestep)
{
	if (timestep>0)
	{
		Tvertex posit;
		Tvector speed;
		obj->position->G_origin(&posit);
		obj->velocity->G_origin_vector(&speed);
		forcefield->integrate(posit,speed,obj->mass,obj->charge,timeJDcurrent,timestep);
		obj->position->Set_origin(&posit);
		obj->velocity->Set_origin_vector(&speed);
	}
}






//*******************************************************************
// T3DMotionCyclOrbit
//*******************************************************************


T3DMotionCyclOrbit::T3DMotionCyclOrbit()
{
	cyclorbit2=NULL;
	isowner=false;
}

T3DMotionCyclOrbit::~T3DMotionCyclOrbit()
{
	if ((isowner)&&(cyclorbit2!=NULL)) delete cyclorbit2;
}



void T3DMotionCyclOrbit::loadfile(StrPtr filename, double allowederror, double scalefactor)
{
	if ((isowner)&&(cyclorbit2!=NULL)) delete cyclorbit2;
	isowner=false;
	cyclorbit2=G_stockcyclorbit(filename);
	if (cyclorbit2==NULL)
	{
		cyclorbit2= new Tcyclorbit2(filename,allowederror,scalefactor);
		isowner=true;
	}

}



void T3DMotionCyclOrbit::eval(double JD, Tvertex &posit, Tvector &spd)
{
	if (cyclorbit2==NULL) throw QError(_text("Cyclic orbit definition is not present"));

	TSC_time tme;
	tme.Set_JD(JD);
	cyclorbit2->calcposit(tme,posit);
	cyclorbit2->getspeed(tme,spd);
}


void T3DMotionCyclOrbit::adjusttime(double timestep)
{
	eval(G_3DCosmos().G_time()->G_JD(),position,speed);

}

void T3DMotionCyclOrbit::apply(TSolidObjectData *obj, double timestep)
{
	obj->position->Set_origin(&position);
	obj->velocity->Set_origin_vector(&speed);
}

void T3DMotionCyclOrbit::G_curposition(Tvertex &iposit)
{
	iposit=position;
}

void T3DMotionCyclOrbit::G_curspeed(Tvector &ispeed)
{
	ispeed=speed;
}


void T3DMotionCyclOrbit::CreateCurve(T3DObjectCurve *crv, int resol, const TSC_time &tm1)
{
	if (cyclorbit2==NULL) throw QError(_text("Cyclic orbit definition is not present"));

	double period=cyclorbit2->G_period();
	double JD1=tm1.G_JD();
	double JD2=JD1+period;
	crv->reset();
	Tvertex pst1,pst2,pst;
	TSC_time tme;
	for (int i=0; i<resol; i++)
	{
		tme.Set_JD(JD1+(JD2-JD1)*i/resol);
		cyclorbit2->calcposit(tme,pst1);

		tme.Set_JD(JD1+(JD2-JD1)*i/resol-period);
		cyclorbit2->calcposit(tme,pst2);

		pst.lincombfrom(1-i*1.0/resol,&pst1,i*1.0/resol,&pst2);
		crv->addpoint(&pst);
	}
	crv->close();
}





//*******************************************************************
// T3DMotionLuna
//*******************************************************************


T3DMotionLuna::T3DMotionLuna()
{
	lunaorbit=NULL;
	factor=addparam(_text("factor"),SC_valname_scalar,false,_text("Orbit magnification factor"))->content.G_content_scalar();
	factor->copyfrom(1.0);
}

T3DMotionLuna::~T3DMotionLuna()
{
	if (lunaorbit!=NULL) delete lunaorbit;
}



void T3DMotionLuna::load()
{
	if (lunaorbit!=NULL) delete lunaorbit;
	lunaorbit= new Tlunaorbit();
}



void T3DMotionLuna::eval(double JD, Tvertex &posit, Tvector &spd, double &rotatangle)
{
	if (lunaorbit==NULL) throw QError(_text("Luna orbit definition is not present"));

	TSC_time tme;
	tme.Set_JD(JD);
	lunaorbit->calcposit(tme,posit,rotatangle);
	lunaorbit->getspeed(tme,spd);
	double fc=factor->G_val();
	posit.Set3(fc*posit.G3_x(),fc*posit.G3_y(),fc*posit.G3_z());
}


void T3DMotionLuna::adjusttime(double timestep)
{
	eval(G_3DCosmos().G_time()->G_JD(),position,speed,rotatangle);

}

void T3DMotionLuna::apply(TSolidObjectData *obj, double timestep)
{
	obj->position->reset();
	obj->position->makerotation(&Tvector(0,0,1),rotatangle);
	obj->position->Set_origin(&position);
	obj->velocity->Set_origin_vector(&speed);
}

void T3DMotionLuna::G_curposition(Tvertex &iposit)
{
	iposit=position;
}

void T3DMotionLuna::G_curspeed(Tvector &ispeed)
{
	ispeed=speed;
}

void T3DMotionLuna::CreateCurve(T3DObjectCurve *crv, int resol, const TSC_time &tm1)
{
	double rotatangle;
	double period=27.321582;
	double JD1=tm1.G_JD();
	double JD2=JD1+period;
	crv->reset();
	Tvertex pst1,pst2,pst;Tvector spd;
	for (int i=0; i<resol; i++)
	{
		eval(JD1+(JD2-JD1)*i/resol,pst1,spd,rotatangle);
		eval(JD1+(JD2-JD1)*i/resol-period,pst2,spd,rotatangle);
		pst.lincombfrom(1-i*1.0/resol,&pst1,i*1.0/resol,&pst2);
		crv->addpoint(&pst);
	}
	crv->close();
}






///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_createtablemotion,create)
{
	setreturntype(SC_valname_motiontable);
	setmemberfunction(SC_valname_motiontable);
	addvar(_qstr("owner"),SC_valname_subframe);
	addvar(_qstr("Name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionTable *motion=new T3DMotionTable;
	motion->addtomotioncatalog();
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	if (arglist->G_ispresent(1)) motion->Set_name(arglist->get(1)->G_content_string()->G_string());
	subframe->addmotion(motion);
	retval->encapsulate(motion);
}
ENDFUNCTION(func_createtablemotion)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_tablemotion_reset,reset)
{
	setmemberfunction(SC_valname_motiontable);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionTable *motion=G_valuecontent<T3DMotionTable>(owner);
	motion->reset();
}
ENDFUNCTION(func_tablemotion_reset)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_tablemotion_add,add)
{
	setmemberfunction(SC_valname_motiontable);
	addvar(_qstr("tm"),SC_valname_time);
	addvar(_qstr("posit"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionTable *motion=G_valuecontent<T3DMotionTable>(owner);
	TSC_time *tm=G_valuecontent<TSC_time>(arglist->get(0));
	Tvertex *pst=G_valuecontent<Tvertex>(arglist->get(1));
	motion->add(tm->G_JD(),pst->G3_x(),pst->G3_y(),pst->G3_z());
}
ENDFUNCTION(func_tablemotion_add)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_tablemotion_loadfile,loadfile)
{
	setmemberfunction(SC_valname_motiontable);
	addvar(_qstr("FileName"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionTable *motion=G_valuecontent<T3DMotionTable>(owner);
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();
	QTextfile file;
	QString line,identifier,st;
	file.openread(filename);
	bool datastarted=false;
	double factor=1.0;
	double JD,X,Y,Z;
	motion->reset();
	while (!file.isend())
	{
		file.readline(line);
		if (issame(line,_qstr("START"))) datastarted=true;
		else
		{
			if (!datastarted)
			{
				line.splitstring(_qstr("="),identifier);
				if (issame(identifier,_qstr("FACT"))) factor=qstr2double(line);
			}
			else
			{
				line.splitstring(_qstr(" "),st);JD=qstr2double(st);
				line.splitstring(_qstr(" "),st);X=factor*qstr2double(st);
				line.splitstring(_qstr(" "),st);Y=factor*qstr2double(st);
				line.splitstring(_qstr(" "),st);Z=factor*qstr2double(st);
				motion->add(JD,X,Y,Z);
			}
		}
	}
	file.close();
}
ENDFUNCTION(func_tablemotion_loadfile)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_tablemotion_createcurve,CreateCurve)
{
	setmemberfunction(SC_valname_motiontable);
	addvar(_qstr("Curve"),SC_valname_curve);
	addvar(_qstr("Resolution"),SC_valname_scalar);
	addvar(_qstr("Start"),SC_valname_time);
	addvar(_qstr("Stop"),SC_valname_time);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionTable *motion=G_valuecontent<T3DMotionTable>(owner);
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(arglist->get(0));
	int resol=arglist->get(1)->G_content_scalar()->G_intval();
		TSC_time *tm1=G_valuecontent<TSC_time>(arglist->get(2));
	TSC_time *tm2=G_valuecontent<TSC_time>(arglist->get(3));
	motion->CreateCurve(curve,resol,*tm1,*tm2);
}
ENDFUNCTION(func_tablemotion_createcurve)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_tablemotion_posit,position)
{
	setreturntype(SC_valname_vertex);
	setmemberfunction(SC_valname_motiontable);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionTable *motion=G_valuecontent<T3DMotionTable>(owner);
	Tvertex posit;
	motion->G_curposition(posit);
	*G_valuecontent<Tvertex>(retval)=posit;
}
ENDFUNCTION(func_tablemotion_posit)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_tablemotion_speed,speed)
{
	setreturntype(SC_valname_vector);
	setmemberfunction(SC_valname_motiontable);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionTable *motion=G_valuecontent<T3DMotionTable>(owner);
	Tvector speed;
	motion->G_curspeed(speed);
	*G_valuecontent<Tvector>(retval)=speed;
}
ENDFUNCTION(func_tablemotion_speed)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_splinemotion_create,create)
{
	setreturntype(SC_valname_motionspline);
	setmemberfunction(SC_valname_motionspline);
	addvar(_qstr("owner"),SC_valname_subframe);
	addvar(_qstr("Name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionSpline *motion=new T3DMotionSpline;
	motion->addtomotioncatalog();
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	if (arglist->G_ispresent(1)) motion->Set_name(arglist->get(1)->G_content_string()->G_string());
	subframe->addmotion(motion);
	retval->encapsulate(motion);
}
ENDFUNCTION(func_splinemotion_create)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_createsolidfunctormotion,create)
{
	setreturntype(T3DMotionSolidFunctor::GetClassName());
	setmemberfunction(T3DMotionSolidFunctor::GetClassName());
	addvar(_qstr("owner"),SC_valname_subframe);
	addvar(_qstr("descript"),SC_valname_functor);
	addvar(_qstr("Name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionSolidFunctor *motion=new T3DMotionSolidFunctor;
	motion->addtomotioncatalog();
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	TSC_functor *functor=G_valuecontent<TSC_functor>(arglist->get(1));
	motion->set(functor);
	if (arglist->G_ispresent(2)) motion->Set_name(arglist->get(2)->G_content_string()->G_string());
	subframe->addmotion(motion);
	retval->encapsulate(motion);
}
ENDFUNCTION(func_createsolidfunctormotion)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_solidfunctormotion_posit,position)
{
	setreturntype(SC_valname_vertex);
	setmemberfunction(T3DMotionSolidFunctor::GetClassName());
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionSolidFunctor *motion=G_valuecontent<T3DMotionSolidFunctor>(owner);
	Tvertex posit;
	motion->G_curposition(posit);
	*G_valuecontent<Tvertex>(retval)=posit;
}
ENDFUNCTION(func_solidfunctormotion_posit)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_createpointfunctormotion,create)
{
	setreturntype(T3DMotionPointFunctor::GetClassName());
	setmemberfunction(T3DMotionPointFunctor::GetClassName());
	addvar(_qstr("owner"),SC_valname_subframe);
	addvar(_qstr("descript"),SC_valname_functor);
	addvar(_qstr("Name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionPointFunctor *motion=new T3DMotionPointFunctor;
	motion->addtomotioncatalog();
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	TSC_functor *functor=G_valuecontent<TSC_functor>(arglist->get(1));
	motion->set(functor);
	if (arglist->G_ispresent(2)) motion->Set_name(arglist->get(2)->G_content_string()->G_string());
	subframe->addmotion(motion);
	retval->encapsulate(motion);
}
ENDFUNCTION(func_createpointfunctormotion)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_pointfunctormotion_posit,position)
{
	setreturntype(SC_valname_vertex);
	setmemberfunction(T3DMotionPointFunctor::GetClassName());
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionPointFunctor *motion=G_valuecontent<T3DMotionPointFunctor>(owner);
	Tvertex posit;
	motion->G_curposition(posit);
	*G_valuecontent<Tvertex>(retval)=posit;
}
ENDFUNCTION(func_pointfunctormotion_posit)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_createmotionrotate,create)
{
	setreturntype(SC_valname_motionrotate);
	setmemberfunction(SC_valname_motionrotate);
	addvar(_qstr("owner"),SC_valname_subframe);
	addvar(_qstr("Name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionRotate *motion=new T3DMotionRotate;
	motion->addtomotioncatalog();
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	if (arglist->G_ispresent(1)) motion->Set_name(arglist->get(1)->G_content_string()->G_string());
	subframe->addmotion(motion);
	retval->encapsulate(motion);
}
ENDFUNCTION(func_createmotionrotate)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_createmotionswing,create)
{
	setreturntype(SC_valname_motionswing);
	setmemberfunction(SC_valname_motionswing);
	addvar(_qstr("owner"),SC_valname_subframe);
	addvar(_qstr("Name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionSwing *motion=new T3DMotionSwing;
	motion->addtomotioncatalog();
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	if (arglist->G_ispresent(1)) motion->Set_name(arglist->get(1)->G_content_string()->G_string());
	subframe->addmotion(motion);
	retval->encapsulate(motion);
}
ENDFUNCTION(func_createmotionswing)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_createmotionkepler,create)
{
	setreturntype(SC_valname_motionkepler);
	setmemberfunction(SC_valname_motionkepler);
	addvar(_qstr("owner"),SC_valname_subframe);
	addvar(_qstr("Name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionKepler *motion=new T3DMotionKepler;
	motion->addtomotioncatalog();
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	if (arglist->G_ispresent(1)) motion->Set_name(arglist->get(1)->G_content_string()->G_string());
	subframe->addmotion(motion);
	retval->encapsulate(motion);
}
ENDFUNCTION(func_createmotionkepler)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_createmotionkepler_createcurve,CreateCurve)
{
	setmemberfunction(SC_valname_motionkepler);
	addvar(_qstr("Curve"),SC_valname_curve);
	addvar(_qstr("Resolution"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionKepler *motion=G_valuecontent<T3DMotionKepler>(owner);
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(arglist->get(0));
	int resol=arglist->get(1)->G_content_scalar()->G_intval();
	motion->CreateCurve(curve,resol);
}
ENDFUNCTION(func_createmotionkepler_createcurve)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_motionkepler_posit,position)
{
	setreturntype(SC_valname_vertex);
	setmemberfunction(SC_valname_motionkepler);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionKepler *motion=G_valuecontent<T3DMotionKepler>(owner);
	Tvertex posit;
	motion->G_curposition(posit);
	*G_valuecontent<Tvertex>(retval)=posit;
}
ENDFUNCTION(func_motionkepler_posit)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_motionkepler_speed,speed)
{
	setreturntype(SC_valname_vector);
	setmemberfunction(SC_valname_motionkepler);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionKepler *motion=G_valuecontent<T3DMotionKepler>(owner);
	Tvector speed;
	motion->G_curspeed(speed);
	*G_valuecontent<Tvector>(retval)=speed;
}
ENDFUNCTION(func_motionkepler_speed)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_createmotioncyclorbit,create)
{
	setreturntype(SC_valname_motioncyclorbit);
	setmemberfunction(SC_valname_motioncyclorbit);
	addvar(_qstr("owner"),SC_valname_subframe);
	addvar(_qstr("Name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionCyclOrbit *motion=new T3DMotionCyclOrbit;
	motion->addtomotioncatalog();
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	if (arglist->G_ispresent(1)) motion->Set_name(arglist->get(1)->G_content_string()->G_string());
	subframe->addmotion(motion);
	retval->encapsulate(motion);
}
ENDFUNCTION(func_createmotioncyclorbit)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_motioncyclorbit_load,loadfile)
{
	setmemberfunction(SC_valname_motioncyclorbit);
	addvar(_qstr("Filename"),SC_valname_string);
	addvar(_qstr("ScaleFactor"),SC_valname_scalar);
	addvar(_qstr("AllowedError"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();
	double scalefactor=arglist->get(1)->G_content_scalar()->G_val();
	double allowederror=arglist->get(2)->G_content_scalar()->G_val();
	T3DMotionCyclOrbit *motion=G_valuecontent<T3DMotionCyclOrbit>(owner);
	motion->loadfile(filename,allowederror,scalefactor);
}
ENDFUNCTION(func_motioncyclorbit_load)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_motioncyclorbit_posit,position)
{
	setreturntype(SC_valname_vertex);
	setmemberfunction(SC_valname_motioncyclorbit);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionCyclOrbit *motion=G_valuecontent<T3DMotionCyclOrbit>(owner);
	Tvertex posit;
	motion->G_curposition(posit);
	*G_valuecontent<Tvertex>(retval)=posit;
}
ENDFUNCTION(func_motioncyclorbit_posit)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_motioncyclorbit_speed,speed)
{
	setreturntype(SC_valname_vector);
	setmemberfunction(SC_valname_motioncyclorbit);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionCyclOrbit *motion=G_valuecontent<T3DMotionCyclOrbit>(owner);
	Tvector speed;
	motion->G_curspeed(speed);
	*G_valuecontent<Tvector>(retval)=speed;
}
ENDFUNCTION(func_motioncyclorbit_speed)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_motioncyclorbit_createcurve,CreateCurve)
{
	setmemberfunction(SC_valname_motioncyclorbit);
	addvar(_qstr("Curve"),SC_valname_curve);
	addvar(_qstr("Time"),SC_valname_time);
	addvar(_qstr("Resolution"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionCyclOrbit *motion=G_valuecontent<T3DMotionCyclOrbit>(owner);
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(arglist->get(0));
	TSC_time *tm1=G_valuecontent<TSC_time>(arglist->get(1));
	int resol=200;
	if (arglist->G_ispresent(2)) resol=arglist->get(2)->G_content_scalar()->G_intval();
	motion->CreateCurve(curve,resol,*tm1);
}
ENDFUNCTION(func_motioncyclorbit_createcurve)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_createmotionluna,create)
{
	setreturntype(SC_valname_motionluna);
	setmemberfunction(SC_valname_motionluna);
	addvar(_qstr("owner"),SC_valname_subframe);
	addvar(_qstr("Name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionLuna *motion=new T3DMotionLuna;
	motion->addtomotioncatalog();
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	if (arglist->G_ispresent(1)) motion->Set_name(arglist->get(1)->G_content_string()->G_string());
	subframe->addmotion(motion);
	retval->encapsulate(motion);
}
ENDFUNCTION(func_createmotionluna)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_motionluna_load,load)
{
	setmemberfunction(SC_valname_motionluna);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionLuna *motion=G_valuecontent<T3DMotionLuna>(owner);
	motion->load();
}
ENDFUNCTION(func_motionluna_load)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_motionluna_posit,position)
{
	setreturntype(SC_valname_vertex);
	setmemberfunction(SC_valname_motionluna);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionLuna *motion=G_valuecontent<T3DMotionLuna>(owner);
	Tvertex posit;
	motion->G_curposition(posit);
	*G_valuecontent<Tvertex>(retval)=posit;
}
ENDFUNCTION(func_motionluna_posit)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_motionluna_speed,speed)
{
	setreturntype(SC_valname_vector);
	setmemberfunction(SC_valname_motionluna);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionLuna *motion=G_valuecontent<T3DMotionLuna>(owner);
	Tvector speed;
	motion->G_curspeed(speed);
	*G_valuecontent<Tvector>(retval)=speed;
}
ENDFUNCTION(func_motionluna_speed)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_motionluna_createcurve,CreateCurve)
{
	setmemberfunction(SC_valname_motionluna);
	addvar(_qstr("Curve"),SC_valname_curve);
	addvar(_qstr("Time"),SC_valname_time);
	addvar(_qstr("Resolution"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionLuna *motion=G_valuecontent<T3DMotionLuna>(owner);
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(arglist->get(0));
	TSC_time *tm1=G_valuecontent<TSC_time>(arglist->get(1));
	int resol=200;
	if (arglist->G_ispresent(2)) resol=arglist->get(2)->G_content_scalar()->G_intval();
	motion->CreateCurve(curve,resol,*tm1);
}
ENDFUNCTION(func_motionluna_createcurve)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_createpointintegratormotion,create)
{
	setreturntype(T3DMotionForceField::GetClassName());
	setmemberfunction(T3DMotionForceField::GetClassName());
	addvar(_qstr("owner"),SC_valname_subframe);
	addvar(_qstr("Name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DMotionForceField *motion=new T3DMotionForceField;
	motion->addtomotioncatalog();
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	if (arglist->G_ispresent(1)) motion->Set_name(arglist->get(1)->G_content_string()->G_string());
	subframe->addmotion(motion);
	retval->encapsulate(motion);
}
ENDFUNCTION(func_createpointintegratormotion)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_evalmotionlist,EvalMotionList)
{
	setclasspath(_qstr("Motions"));
	addvar(_qstr("MotionList"),SC_valname_list);
	addvar(_qstr("PositionList"),SC_valname_list);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_list *motions=G_valuecontent<TSC_list>(arglist->get(0));
	TSC_list *points=G_valuecontent<TSC_list>(arglist->get(1));
	points->reset();
	TSC_funcarglist arglist1(arglist->G_parentblock());
	QString error;
	TSC_value value;
	for (int i=0; i<motions->G_size(); i++)
	{
//		vl.settype(GetTSCenv().G_datatype(SC_valname_vertex));
		if (!arglist->G_parentblock()->G_script()->G_env()->evalmemberfunction(_qstr("Position"),&value,motions->get(i),NULL,arglist1,error))
			throw QError(error);
		points->add(&value);
	}
/*	T3DMotionLuna *motion=new T3DMotionLuna;
	motion->addtomotioncatalog();
	T3DSubFrameObject *subframe=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	if (arglist->G_ispresent(1)) motion->Set_name(arglist->get(1)->G_content_string()->G_string());
	subframe->addmotion(motion);
	retval->encapsulate(motion);*/
}
ENDFUNCTION(func_evalmotionlist)
