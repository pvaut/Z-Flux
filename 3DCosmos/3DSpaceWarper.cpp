#include "stdafx.h"
#include "3DSpaceWarper.h"

#include "SC_env.h"
#include "SC_script.h"
#include "SC_functor.h"


Tlineartransformer::Tlineartransformer(Taffinetransformation *itransf)
{
	transf=*itransf;
	transf.updateinverse();
}
void Tlineartransformer::warpvertex(const Tvertex *vin, Tvertex *vout)
{
	transf.mul(vin,vout);
}

void Tlineartransformer::warpnormal(const Tvertex *vin, const Tvector *nin, Tvector *nout)
{
	transf.mulnormvec(nin,nout);
};



//******************************************************************************************************

Tconalpincher::Tconalpincher(double i_zh)
{
	zh=i_zh;
	if (zh==0) throw QError(_text("Invalid zh value"));
	a=1.0/zh;
}

void Tconalpincher::warpvertex(const Tvertex *vin, Tvertex *vout)
{
	double x=vin->G3_x();
	double y=vin->G3_y();
	double z=vin->G3_z();

	double fc=1-a*z;
	double x2=fc*x;
	double y2=fc*y;
	double z2=z;
	vout->Set3(x2,y2,z2);
}

void Tconalpincher::warpnormal(const Tvertex *vin, const Tvector *nin, Tvector *nout)
{
	double x=vin->G3_x();
	double y=vin->G3_y();
	double z=vin->G3_z();
	double nx=nin->G_x();
	double ny=nin->G_y();
	double nz=nin->G_z();

	double nx2=nx;
	double ny2=ny;
	double nz2=(1-a*z)*nz+a*x*nx+a*y*ny;

	nout->Set(nx2,ny2,nz2);
	nout->normfrom(nout);
}



//******************************************************************************************************

Tspiraler::Tspiraler(double i_a)
{
	a=i_a;
}

void Tspiraler::warpvertex(const Tvertex *vin, Tvertex *vout)
{
	double x=vin->G3_x();
	double y=vin->G3_y();
	double z=vin->G3_z();

	double sn=sin(a*z);
	double cs=cos(a*z);
	double x2= x*cs+y*sn;
	double y2=-x*sn+y*cs;
	double z2=z;
	vout->Set3(x2,y2,z2);
}

void Tspiraler::warpnormal(const Tvertex *vin, const Tvector *nin, Tvector *nout)
{
	double x=vin->G3_x();
	double y=vin->G3_y();
	double z=vin->G3_z();
	double nx=nin->G_x();
	double ny=nin->G_y();
	double nz=nin->G_z();

	double sn=sin(a*z);
	double cs=cos(a*z);
	double nx2= nx*cs+ny*sn;
	double ny2=-nx*sn+ny*cs;
	double nz2=nz-a*y*nx+a*x*ny;

	nout->Set(nx2,ny2,nz2);
	nout->normfrom(nout);
}



Tcustomwarper::Tcustomwarper(TSC_functor *ifunctor)
{
	functorref.setreffor(ifunctor);
}


void Tcustomwarper::eval(const Tvertex &inp, Tvertex &outp)
{
	TSC_functor &functor=functorref.G_obj();

	try{
		TSC_funcarglist arglist(NULL);
		TSC_value retval0,arg;
		arg.createtype(GetTSCenv().G_datatype(SC_valname_vertex));
		arglist.add(&arg);
		*(G_valuecontent<Tvertex>(&arg))=inp;
		functor.eval(&arglist,&retval0);
		outp=*(G_valuecontent<Tvertex>(&retval0));
	}
	catch(TSC_runerror err)
	{
		QString errstring;
		FormatString(errstring,_text("Custom warp: ^1"),err.G_content());
		throw QError(errstring);
	}
	catch(QError err)
	{
		QString errstring;
		FormatString(errstring,_text("Custom warp: ^1"),err.G_content());
		throw QError(errstring);
	}
}

void Tcustomwarper::warpvertex(const Tvertex *vin, Tvertex *vout)
{
	eval(*vin,*vout);
}

void Tcustomwarper::warpnormal(const Tvertex *vin, const Tvector *nin, Tvector *nout)
{
	Tvector tangent1,tangent2;
	tangent1.anynormalfrom(nin);
	tangent2.vecprod(nin,&tangent1);tangent2.normfrom(&tangent2);
	Tvertex p1,p2,wp0,wp1,wp2;
	Tvector wt1,wt2;

	eval(*vin,wp0);
	p1.lincombfrom(1,vin,1.0e-4,&tangent1);eval(p1,wp1);wt1.lincombfrom(1,&wp1,-1,&wp0);
	p2.lincombfrom(1,vin,1.0e-4,&tangent2);eval(p2,wp2);wt2.lincombfrom(1,&wp2,-1,&wp0);
	nout->vecprod(&wt1,&wt2);
	nout->normfrom(nout);
}
