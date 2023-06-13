#include "stdafx.h"
#include <stdio.h>
#include <math.h>

#include "vecmath.h"
#include "SC_time.h"
#include "qfile.h"

#include "astrotools.h"
#include "astrotools_cyclorbit.h"

StrPtr G_datadir();
StrPtr G_scriptsdir();
StrPtr G_texturesdir();

/*
/////////////////////////////////////////////////////////////////////////////
// Tcyclorbit
/////////////////////////////////////////////////////////////////////////////


Tcyclorbit::Tcyclorbit(StrPtr ifilename, double iscalefactor)
{
	char css[1000];
	char css1[100],css2[100];
	int  i,j,order,cnt,tpe,ok;
	FILE *fp;


	scalefactor=iscalefactor;
	if (_wfopen_s(&fp,ifilename,_qstr("r"))!=0) throw QError(_text("Unable to find cycl file"));
  
	ok=1;
	for(;ok==1;)
	{
		ok=0;
		if (fscanf(fp,"%s %s %d %d\n",css1,css2,&order,&cnt)==4)
		{
			ok=1;
			tpe= -1;
			if (strcmp(css2,"L")==0) tpe=1;
			if (strcmp(css2,"B")==0) tpe=2;
			if (strcmp(css2,"R")==0) tpe=3;
			if (tpe<0) { printf("Unknown expa tpe %s\n",css2);exit(0); }

			expa[tpe].terms[order].count=cnt;
			if (expa[tpe].maxorder<order)
				expa[tpe].maxorder=order;
			for (i=1; i<=cnt; i++)
			{
				if (fscanf(fp,"%d %lf %lf %lf",&j,
					&expa[tpe].terms[order].A[i],
					&expa[tpe].terms[order].B[i],
					&expa[tpe].terms[order].C[i])
				!=4)
			{ printf("Error while reading expansion\n");exit(0); }
				if (j!=i) { printf("Incorrect index\n");exit(0); }
			}
		}
	}
	fclose(fp);
}

void Tcyclorbit::calcposit(const TSC_time& time, Tvertex &posit)
{
	int    tpe,order,i;
	double rs,rs1;

	double coord_l,coord_b,coord_r;

	for (tpe=1; tpe<=3; tpe++)
	{
		rs=0.0;
		for (order=0; order<=expa[tpe].maxorder; order++)
		{
			rs1=0.0;
			for (i=1; i<=expa[tpe].terms[order].count; i++)
			{
				rs1 += expa[tpe].terms[order].A[i] *
  					cos( expa[tpe].terms[order].B[i] +
					expa[tpe].terms[order].C[i]*
					time.G_tau());
			}
			rs += rs1*pow(time.G_tau(),order*1.0);
		}
		if (tpe==1) coord_l=rs/1.0e8;
		if (tpe==2) coord_b=rs/1.0e8;
		if (tpe==3) coord_r=rs/1.0e8;
	}

	posit.S3_x(scalefactor*coord_r*cos(coord_l)*cos(coord_b));
	posit.S3_y(scalefactor*coord_r*sin(coord_l)*cos(coord_b));
	posit.S3_z(scalefactor*coord_r*sin(coord_b));
}

void Tcyclorbit::getspeed(const TSC_time& time, Tvector &spd)
{
	Tvertex posit1,posit2;
	TSC_time tm1;tm1=time;tm1.addseconds(-60);
	TSC_time tm2;tm2=time;tm2.addseconds(+60);
	calcposit(tm1,posit1);
	calcposit(tm2,posit2);

	double fc=24.0*60.0/2;
	spd.lincombfrom(-fc,&posit1,+fc,&posit2);
}

double Tcyclorbit::G_period() const//in days
{
	double rs=(2*Pi)/(expa[1].terms[1].A[1]/1.0e9);
	rs=rs*36525.0;
	return rs;
}
*/

Tarray<Tcyclorbit2> stockcyclorbits;
Tarray<QString> stockcyclorbitnames;

void loadstockcyclorbits()
{

	Tcyclorbit2 *orb;
	QString filename;

	addlog(_text("Loading stock cyclic orbits"),+1);

	filename=G_datadir();filename+=_text("\\AstroData\\MERCURIUS_cycl.tab");
	orb=new Tcyclorbit2(filename,0.00001,au2km);
	stockcyclorbits.add(orb);stockcyclorbitnames.add(new QString("MERCURIUS"));

	filename=G_datadir();filename+=_text("\\AstroData\\VENUS_cycl.tab");
	orb=new Tcyclorbit2(filename,0.00001,au2km);
	stockcyclorbits.add(orb);stockcyclorbitnames.add(new QString("VENUS"));

	filename=G_datadir();filename+=_text("\\AstroData\\EARTH_cycl.tab");
	orb=new Tcyclorbit2(filename,0.00001,au2km);stockcyclorbitnames.add(new QString("EARTH"));
	stockcyclorbits.add(orb);

	filename=G_datadir();filename+=_text("\\AstroData\\MARS_cycl.tab");
	orb=new Tcyclorbit2(filename,0.00001,au2km);
	stockcyclorbits.add(orb);stockcyclorbitnames.add(new QString("MARS"));

	filename=G_datadir();filename+=_text("\\AstroData\\JUPITER_cycl.tab");
	orb=new Tcyclorbit2(filename,0.00001,au2km);
	stockcyclorbits.add(orb);stockcyclorbitnames.add(new QString("JUPITER"));

	filename=G_datadir();filename+=_text("\\AstroData\\SATURNUS_cycl.tab");
	orb=new Tcyclorbit2(filename,0.00001,au2km);
	stockcyclorbits.add(orb);stockcyclorbitnames.add(new QString("SATURNUS"));

	filename=G_datadir();filename+=_text("\\AstroData\\URANUS_cycl.tab");
	orb=new Tcyclorbit2(filename,0.00001,au2km);
	stockcyclorbits.add(orb);stockcyclorbitnames.add(new QString("URANUS"));

	filename=G_datadir();filename+=_text("\\AstroData\\NEPTUNUS_cycl.tab");
	orb=new Tcyclorbit2(filename,0.00001,au2km);
	stockcyclorbits.add(orb);stockcyclorbitnames.add(new QString("NEPTUNUS"));


	addlog(_text("Stock cyclic orbits loaded"),-1);
}

Tcyclorbit2* G_stockcyclorbit(StrPtr name)
{
	for (int i=0; i<stockcyclorbitnames.G_count(); i++)
		if (issame(*stockcyclorbitnames[i],name)) return stockcyclorbits[i];
	return NULL;
}


///////////////////////////////////////////////////////////////////////////
// Tcyclorbit2comp
///////////////////////////////////////////////////////////////////////////


Tcyclorbit2comp::Tcyclorbit2comp()
{
	reset(0.0);
}

void Tcyclorbit2comp::reset(double iaccuracy)
{
	accuracy=iaccuracy;
	usedtermcount=0;
	rejectedtermcount=0;
	fixedaccuracy=false;
	maxA=0.0;
}

void Tcyclorbit2comp::try_addterm(int order, double iA, double iB, double iC)
{
	if (fabs(iA)<accuracy*maxA)
	{
		rejectedtermcount++;
		return;
	}
	if ((!fixedaccuracy)&&(maxA<fabs(iA))) maxA=fabs(iA);
	usedtermcount++;
	while (A.G_count()<=order)
	{
		A.addexact(new Tdoublearray);
		B.addexact(new Tdoublearray);
		C.addexact(new Tdoublearray);
	}
	A[order]->add(iA);
	B[order]->add(iB);
	C[order]->add(iC);
}


double Tcyclorbit2comp::calc(double tau)
{
	double rs=0.0;
	double taupow=1.0;
	for (int order=0; order<A.G_count(); order++)
	{
		double rs1=0.0;
		for (int i=0; i<A[order]->G_count(); i++)
			rs1 += A[order]->get(i) * cos( B[order]->get(i) + C[order]->get(i)*tau );
		rs += rs1*taupow;
		taupow*=tau;
	}
	return rs;
}


///////////////////////////////////////////////////////////////////////////
// Tcyclorbit2
///////////////////////////////////////////////////////////////////////////

Tcyclorbit2::Tcyclorbit2(StrPtr ifilename, double accuracy, double iscalefactor)
{
	try{
		FILE *fp;
		char termtype[1000];
		int order;
		double A,B,C;

		addlog(TFormatString(_text("Loading stock cyclic orbit ^1"),ifilename));

		scalefactor=iscalefactor;
		if (_wfopen_s(&fp,ifilename,_qstr("r"))!=0) throw QError(TFormatString(_text("Unable to find cycl file ^1"),ifilename));

		expa_L.reset(accuracy);
		expa_B.reset(accuracy);
		expa_R.reset(accuracy/100.0);

		//preset for L & B
		expa_L.maxA=2*Pi*accuracy;expa_L.fixedaccuracy=true;
		expa_B.maxA=2*Pi*accuracy;expa_B.fixedaccuracy=true;

		expa_R.maxA=1.0;

		bool ok=true;
		for(;(ok)&&(!feof(fp));)
		{
			if (fscanf(fp,"%s %d %lf %lf %lf\n",termtype,&order,&A,&B,&C)==5)
			{
				if (stricmp(termtype,"L")==0) expa_L.try_addterm(order,A,B,C);
				if (stricmp(termtype,"B")==0) expa_B.try_addterm(order,A,B,C);
				if (stricmp(termtype,"R")==0) expa_R.try_addterm(order,A,B,C);
			}
		}
		fclose(fp);
	}
	catch(QError &err)
	{
		reporterror(err.G_content());
	}
}

void Tcyclorbit2::calcposit(const TSC_time& time, Tvertex &posit)
{
	double L=expa_L.calc(time.G_tau());
	double B=expa_B.calc(time.G_tau());
	double R=expa_R.calc(time.G_tau());

	posit.S3_x(scalefactor*R*cos(L)*cos(B));
	posit.S3_y(scalefactor*R*sin(L)*cos(B));
	posit.S3_z(scalefactor*R*sin(B));
}


void Tcyclorbit2::getspeed(const TSC_time& time, Tvector &spd)
{
	Tvertex posit1,posit2;
	TSC_time tm1;tm1=time;tm1.addseconds(-60);
	TSC_time tm2;tm2=time;tm2.addseconds(+60);
	calcposit(tm1,posit1);
	calcposit(tm2,posit2);

	double fc=24.0*60.0/2;
	spd.lincombfrom(-fc,&posit1,+fc,&posit2);
}

double Tcyclorbit2::G_period() const//in days
{
	double rs=(2*Pi)/(expa_L.A[1]->get(0));
	rs=rs*36525.0*10.0;
	return rs;
}




/////////////////////////////////////////////////////////////////////////////
// Tlunaorbit
/////////////////////////////////////////////////////////////////////////////

Tlunaorbit::Tlunaorbit()
{
	FILE *fp;
	int  i;
	char css[1000];

	QString filename=G_datadir();
	filename+=_qstr("\\AstroData\\moon.tab");

	if (_wfopen_s(&fp,filename,_qstr("r"))!=0) throw QError(_text("Unable to find moon.tab file"));
	fscanf(fp,"%s %d",css,&term1count);
	if (strcmp(css,"TAB1")!=0)
		{ _exit(0); }
	for (i=1; i<=term1count; i++)
	{
		fscanf(fp,"%d %d %d %d %d %d",
		&term1[i].i1,
		&term1[i].i2,
		&term1[i].i3,
		&term1[i].i4,
		&term1[i].j1,
		&term1[i].j2);
	}
	fscanf(fp,"%s %d",css,&term2count);
	if (strcmp(css,"TAB2")!=0)
		{ _exit(0); }
	for (i=1; i<=term2count; i++)
	{
		fscanf(fp,"%d %d %d %d %d",
		&term2[i].i1,
		&term2[i].i2,
		&term2[i].i3,
		&term2[i].i4,
		&term2[i].j1);
	}

	fclose(fp);
}

void Tlunaorbit::calcposit(const TSC_time& time, Tvertex &posit, double &rotatangle)
{
  double L2,D,M,M2,F,A1,A2,A3,E,suml,sumr,sumb,arg,cf1,cf2;
  int    i;

  double T=time.G_T();
  double T2=T*T;
  double T3=T2*T;
  double T4=T3*T;

  L2=     218.3164591
     + 481267.88134236   *T
     -      0.0013268    *T2
     + 1.0/538841        *T3
     - 1.0/65194000      *T4;
  reduc360(&L2);

  D=      297.8502042
     + 445267.1115168 *T
     -      0.0016300 *T2
     + 1.0/545868     *T3
     - 1.0/113065000  *T4;
  reduc360(&D);

  M=    357.5291092
    + 35999.0502909 *T
    -     0.0001536 *T2
    + 1.0/24490000  *T3;
  reduc360(&M);

  M2=     134.9634114
     + 477198.8676313 *T
     +      0.0089970 *T2
     + 1.0/69699      *T3
     - 1.0/14712000   *T4;
  reduc360(&M2);

  F=      93.2720993
    + 483202.0175273 *T
    -      0.0034029 *T2
    - 1.0/3526000    *T3
    + 1.0/863310000  *T4;
  reduc360(&F);

  A1= 119.75 +    131.849*T;
  A2=  53.09 + 479264.290*T;
  A3= 313.45 + 481266.484*T;
  reduc360(&A1);
  reduc360(&A2);
  reduc360(&A3);

  E=1.0-0.002516*T-0.0000074*T*T;


  suml=0.0;
  sumr=0.0;
  for (i=1; i<=term1count; i++)
    {
      arg=(term1[i].i1*D + 
	   term1[i].i2*M + 
	   term1[i].i3*M2 + 
	   term1[i].i4*F     )*deg2rad;
      cf1=term1[i].j1;
      cf2=term1[i].j2;
      if ((term1[i].i2==1)||(term1[i].i2==-1)) 
	{ cf1 *= E;   cf2 *= E;   }
      if ((term1[i].i2==2)||(term1[i].i2==-2)) 
	{ cf2 *= E*E; cf2 *= E*E; }
      suml += cf1*sin(arg);
      sumr += cf2*cos(arg);
    }

  suml +=  3958* sin( A1       *deg2rad)
         + 1962* sin( (L2-F)   *deg2rad)
	 +  318* sin( A2       *deg2rad);



  sumb=0.0;
  for (i=1; i<=term2count; i++)
    {
      arg=(term2[i].i1*D  + 
	   term2[i].i2*M  + 
	   term2[i].i3*M2 + 
	   term2[i].i4*F    )*deg2rad;
      cf1=term2[i].j1;
      if ((term2[i].i2==1)||(term2[i].i2==-1)) 
	{ cf1 *= E; }
      if ((term2[i].i2==2)||(term2[i].i2==-2)) 
	{ cf1 *= E*E; }
      sumb += cf1*sin(arg);
    }

  sumb +=  -2235 * sin( L2      *deg2rad)
         +   382 * sin( A3      *deg2rad)
	 +   175 * sin( (A1-F)  *deg2rad)
	 +   175 * sin( (A1+F)  *deg2rad)
	 +   127 * sin( (L2-M2) *deg2rad)
	 -   115 * sin( (L2+M2) *deg2rad);


  l=L2+suml/1000000.0;
  b=sumb/1000000;
  d=385000.56+sumr/1000;

  rotatangle=(0+l)/180*Pi;;

  l=l/180*Pi;
  b=b/180*Pi;

  Xrel=d*cos(l)*cos(b);
  Yrel=d*sin(l)*cos(b);
  Zrel=d*sin(b);

  posit.Set3(Xrel,Yrel,Zrel);

}

void Tlunaorbit::getspeed(const TSC_time& time, Tvector &spd)
{
	Tvertex posit1,posit2;
	double rotatangle=0;
	TSC_time tm1;tm1=time;tm1.addseconds(-60);
	TSC_time tm2;tm2=time;tm2.addseconds(+60);
	calcposit(tm1,posit1,rotatangle);
	calcposit(tm2,posit2,rotatangle);

	double fc=24.0*60.0/2;
	spd.lincombfrom(-fc,&posit1,+fc,&posit2);
}
