#include "stdafx.h"
#include "bezier.h"

#include "SC_func.h"
#include "SC_env.h"
#include "SC_script.h"
#include "SC_expr.h"

#include "interp.h"


///////////////////////////////////////////////////////////////
// TCubicSpline
///////////////////////////////////////////////////////////////


double mc2(double x) { return x*x; }
double mc3(double x) { return x*x*x; }


void calccubicpar(double x1, double y1, double m1,
						double x2, double y2, double m2,
						double &d, double &c, double &b, double &a)
{
	double p11,p12,p21,p22,r1,r2;
	p11=3*(mc2(x1)-mc2(x2));
	p12=2*(x1-x2);
	r1=m1-m2;
	p21=3*x2*mc2(x1)-2*mc3(x1)-mc3(x2);
	p22=-mc2(x1)-mc2(x2)+2*x1*x2;
	r2=y1-y2+m1*(x2-x1);
	a=(r1*p22-r2*p12)/(p11*p22-p21*p12);
	b=(r2*p11-r1*p21)/(p11*p22-p21*p12);
	c=m1-3*(a)*mc2(x1)-2*(b)*x1;
	d=y1-(a)*mc3(x1)-(b)*mc2(x1)-(c)*x1;
}


//TCubicSpline spl;

TCubicSpline::TCubicSpline()
{
	calcmethod=1;
	calculated=false;
}

void TCubicSpline::Set_calcmethod(int icalcmethod)
{
	calcmethod=icalcmethod;
	calculated=false;
}


void TCubicSpline::reset()
{
	X.reset();
	Y.reset();
	b.reset();
	c.reset();
	d.reset();
	calculated=false;
}

void TCubicSpline::add(double iX, double iY)
{
	if (X.G_count()>0)
		if (iX<X[X.G_count()-1]) throw QError(_text("Spline points should be created in increasing order"));
	X.add(iX);
	Y.add(iY);
	b.add(0.0);
	c.add(0.0);
	d.add(0.0);
	calculated=false;
}

void TCubicSpline::calc()
{
	if (calculated) return;
	if (X.G_count()<2) throw QError(_text("Unable to calculate spline: not enough points"));
	bool ok=false;

	if (calcmethod==0) { calc_notaknot(); ok=true; }
	if (calcmethod==1) { calc_CatmullRom(false); ok=true; }
	if (calcmethod==2) { calc_CatmullRom(true); ok=true; }

	if (!ok) throw QError(_text("Invalid spline calculation method"));
	calculated=true;
}

void TCubicSpline::calc_notaknot()
{
	if (X.G_count()<2) throw QError(_text("Unable to calculate spline: not enough points"));
	cubic_nak(X.G_count(),X.G_ptr(),Y.G_ptr(),b.G_ptr(),c.G_ptr(),d.G_ptr());
	calculated=true;
}

void TCubicSpline::calc_CatmullRom(bool isclosed)
{
	if (X.G_count()<2) throw QError(_text("Unable to calculate spline: not enough points"));

	//calculate first derivative in each point
	Tdoublearray Yder;
	if (!isclosed)
	{
		//first point
		Yder.add((Y[1]-Y[0])/(X[1]-X[0]));
		//inner points
		for (int i=1; i<X.G_count()-1; i++) Yder.add((Y[i+1]-Y[i-1])/(X[i+1]-X[i-1]));
		//end point
		Yder.add((Y[X.G_count()-1]-Y[X.G_count()-2])/(X[X.G_count()-1]-X[X.G_count()-2]));
	}
	else
	{
		//first point
		Yder.add((Y[1]-Y[X.G_count()-2])/(X[1]-(X[X.G_count()-2]-X[X.G_count()-1])));
		//inner points
		for (int i=1; i<X.G_count()-1; i++) Yder.add((Y[i+1]-Y[i-1])/(X[i+1]-X[i-1]));
		//end point
		Yder.add(Yder[0]);
	}

	double c_a,c_b,c_c,c_d;
	for (int i=0; i<X.G_count()-1; i++)
	{
		calccubicpar(0,Y[i],Yder[i],X[i+1]-X[i],Y[i+1],Yder[i+1],c_a,c_b,c_c,c_d);
		b[i]=c_b;
		c[i]=c_c;
		d[i]=c_d;
	}
}

double TCubicSpline::eval(double iX)
{
	calc();
	return spline_eval(X.G_count(),X.G_ptr(),Y.G_ptr(),b.G_ptr(),c.G_ptr(),d.G_ptr(),iX);
}

double TCubicSpline::evalder(double iX)
{
	calc();
	return spline_eval_der(X.G_count(),X.G_ptr(),Y.G_ptr(),b.G_ptr(),c.G_ptr(),d.G_ptr(),iX);
}

double TCubicSpline::evaldersecond(double iX)
{
	calc();
	return spline_eval_der2(X.G_count(),X.G_ptr(),Y.G_ptr(),b.G_ptr(),c.G_ptr(),d.G_ptr(),iX);
}

void TCubicSpline::copyfrom(const TCubicSpline &inp)
{
	reset();
	for (int i=0; i<inp.X.G_count(); i++)
		add(inp.X[i],inp.Y[i]);
	calcmethod=inp.calcmethod;
}

int TCubicSpline::G_count() const
{
	return X.G_count();
}

double TCubicSpline::G_pointx(int nr) const
{
	if ((nr<0)||(nr>=X.G_count())) throw QError(_text("Unable to return spline point: invalid index"));
	return X[nr];
}

double TCubicSpline::G_pointy(int nr) const
{
	if ((nr<0)||(nr>=Y.G_count())) throw QError(_text("Unable to return spline point: invalid index"));
	return Y[nr];
}

double TCubicSpline::G_coef(int nr, int deg)
{
	calc();
	if ((nr<0)||(nr>=X.G_count())) throw QError(_text("Unable to return spline coefficient: invalid index"));
	if (deg==0) return Y[nr];
	if (deg==1) return b[nr];
	if (deg==2) return c[nr];
	if (deg==3) return d[nr];
	throw QError(_text("Unable to return spline coefficient: invalid degree"));
}




/////////////////////////////////////////////////////////////
// TCubicSplineSurface
/////////////////////////////////////////////////////////////

double TCubicSplineSurfacePatch::eval(double u1, double u2)
{
	double pw1[4],pw2[4];

	pw1[0]=1;pw2[0]=1;
	for (int i=1; i<4; i++)
	{
		pw1[i]=u1*pw1[i-1];
		pw2[i]=u2*pw2[i-1];
	}

	double rs=0;
	for (int i1=0; i1<4; i1++)
		for (int i2=0; i2<4; i2++)
		rs+=coefs[i1][i2]*pw1[i1]*pw2[i2];
	return rs;
}

double TCubicSplineSurfacePatch::evalder1(double u1, double u2)//partial derivative with respect to u1
{
	double pw1[4],pw2[4];

	pw1[0]=1;pw2[0]=1;
	for (int i=1; i<4; i++)
	{
		pw1[i]=u1*pw1[i-1];
		pw2[i]=u2*pw2[i-1];
	}

	double rs=0;
	for (int i1=1; i1<4; i1++)
		for (int i2=0; i2<4; i2++)
		rs+=i1*coefs[i1][i2]*pw1[i1-1]*pw2[i2];
	return rs;
}


double TCubicSplineSurfacePatch::evalder2(double u1, double u2)//partial derivative with respect to u1
{
	double pw1[4],pw2[4];

	pw1[0]=1;pw2[0]=1;
	for (int i=1; i<4; i++)
	{
		pw1[i]=u1*pw1[i-1];
		pw2[i]=u2*pw2[i-1];
	}

	double rs=0;
	for (int i1=0; i1<4; i1++)
		for (int i2=1; i2<4; i2++)
		rs+=i2*coefs[i1][i2]*pw1[i1]*pw2[i2-1];
	return rs;
}


//TCubicSplineSurface srf;

TCubicSplineSurface::TCubicSplineSurface()
{
	calculated=false;

/*	for (int i1=0; i1<=4; i1++)
		for (int i2=0; i2<=4; i2++)
			addpoint(i1,i2,i1);

	double vl;
	vl=eval(0.1,0);
	vl=eval(0.2,0);
	vl=eval(0.3,0);
	vl=eval(0.4,0);
	vl=eval(0.5,0);
	vl=eval(0.6,0);
	vl=eval(0.7,0);
	vl=eval(0.8,0);*/
}

void TCubicSplineSurface::reset()
{
	Y.reset();
	calculated=false;
}

void TCubicSplineSurface::addpoint(int i1, int i2, double iy)
{
	while (Y.G_count()<=i1) Y.add(new Tdoublearray);
	while (Y[i1]->G_count()<=i2) Y[i1]->add(0);
	Y[i1]->set(i2,iy);
	calculated=false;
}

void TCubicSplineSurface::calculate()
{
	if (calculated) return;

	Tarray<TCubicSpline> splines1;

	dim1=Y.G_count();
	if (dim1<2) throw QError(_text("Unable to calculate spline surface: not enough points"));
	dim2=Y[0]->G_count();
	if (dim2<2) throw QError(_text("Unable to calculate spline surface: not enough points"));

	patches.reset();
	for (int i1=0; i1<dim1-1; i1++)
	{
		patches.add(new Tarray<TCubicSplineSurfacePatch>);
		for (int i2=0; i2<dim2-1; i2++)
			patches[i1]->add(new TCubicSplineSurfacePatch);
	}

	for (int i1=0; i1<dim1; i1++)
	{
		TCubicSpline *spl=new TCubicSpline;splines1.add(spl);
		for (int i2=0; i2<dim2; i2++)
			spl->add(i2*1.0/(dim2-1),Y[i1]->get(i2));
	}

	for (int i2=0; i2<dim2-1; i2++)
	{
		for (int deg1=0; deg1<4; deg1++)
		{
			TCubicSpline spl;
			for (int i1=0; i1<dim1; i1++) spl.add(i1*1.0/(dim1-1),splines1[i1]->G_coef(i2,deg1));
			for (int i1=0; i1<dim1-1; i1++)
				for (int deg2=0; deg2<4; deg2++)
					patches[i1]->get(i2)->coefs[deg2][deg1]=spl.G_coef(i1,deg2);
		}
	}

	calculated=true;
}

void TCubicSplineSurface::copyfrom(const TCubicSplineSurface &inp)
{
	reset();

	for (int i1=0; i1<inp.Y.G_count(); i1++)
		for (int i2=0; i2<inp.Y[i1]->G_count(); i2++)
			addpoint(i1,i2,inp.Y[i1]->get(i2));
}


void TCubicSplineSurface::calcpatch(double fr1, double fr2, int &i1, int &i2, double &u1, double &u2)
{
	if (fr1<0) fr1=0;
	if (fr1>1) fr1=1;
	if (fr2<0) fr2=0;
	if (fr2>1) fr2=1;

	//determine patch
	i1=(int)(0.000001+floor(fr1*(dim1-1)));
	i2=(int)(0.000001+floor(fr2*(dim2-1)));
	if (i1>=dim1-1) i1=dim1-2;
	if (i1<0) i1=0;
	if (i2>=dim2-1) i2=dim2-2;
	if (i2<0) i2=0;

	u1=fr1-i1*1.0/(dim1-1);
	u2=fr2-i2*1.0/(dim2-1);
}


double TCubicSplineSurface::eval(double fr1, double fr2)
{
	calculate();
	int i1,i2;
	double u1,u2;
	calcpatch(fr1,fr2,i1,i2,u1,u2);
	return patches[i1]->get(i2)->eval(u1,u2);
}

double TCubicSplineSurface::evalder1(double fr1, double fr2)//partial derivative with respect to dim1
{
	calculate();
	int i1,i2;
	double u1,u2;
	calcpatch(fr1,fr2,i1,i2,u1,u2);
	return patches[i1]->get(i2)->evalder1(u1,u2);
}

double TCubicSplineSurface::evalder2(double fr1, double fr2)//partial derivative with respect to dim2
{
	calculate();
	int i1,i2;
	double u1,u2;
	calcpatch(fr1,fr2,i1,i2,u1,u2);
	return patches[i1]->get(i2)->evalder2(u1,u2);
}




/*
TPolyBezier1D::TPolyBezier1D()
{
}

void TPolyBezier1D::reset()
{
	pts.reset();
	pts_ctrl1.reset();
	pts_ctrl2.reset();
	calculated=false;
}


void TPolyBezier1D::add(double ival)
{
	pts.add(ival);
	calculated=false;
}

double TPolyBezier1D::eval(double ifrc)
{
	calccontrols();
	int n=pts.G_count()-1;
	if (ifrc<0) ifrc=0;
	if (ifrc>1) ifrc=1;
	int idx=(int)(0.00001+floor(ifrc*n));
	if (idx<0) idx=0;
	if (idx>=n) idx=n-1;
	double t=(ifrc-idx*1.0/n)*n;
	double ti=1-t;

	return ti*ti*ti*pts[idx]+3*ti*ti*t*pts_ctrl1[idx]+3*ti*t*t*pts_ctrl2[idx]+t*t*t*pts[idx+1];
}

double TPolyBezier1D::evalder(double ifrc)
{
	calccontrols();
	int n=pts.G_count()-1;
	if (ifrc<0) ifrc=0;
	if (ifrc>1) ifrc=1;
	int idx=(int)(0.00001+floor(ifrc*n));
	if (idx<0) idx=0;
	if (idx>=n) idx=n-1;
	double t=(ifrc-idx*1.0/n)*n;

	double C1=-3*(1-t)*(1-t);
	double C2=3*(3*t*t-4*t+1);
	double C3=3*(2*t-3*t*t);
	double C4=3*t*t;
	return C1*pts[idx]+C2*pts_ctrl1[idx]+C3*pts_ctrl2[idx]+C4*pts[idx+1];
}

double TPolyBezier1D::evalder2(double ifrc)
{
	calccontrols();
	int n=pts.G_count()-1;
	if (ifrc<0) ifrc=0;
	if (ifrc>1) ifrc=1;
	int idx=(int)(0.00001+floor(ifrc*n));
	if (idx<0) idx=0;
	if (idx>=n) idx=n-1;
	double t=(ifrc-idx*1.0/n)*n;

	double C1=6*(1-t);
	double C2=3*(6*t-4);
	double C3=3*(2-6*t);
	double C4=6*t;
	return C1*pts[idx]+C2*pts_ctrl1[idx]+C3*pts_ctrl2[idx]+C4*pts[idx+1];
}


void TPolyBezier1D::calccontrols()
{
	if (calculated) return;

	pts_ctrl1.reset();
	pts_ctrl2.reset();

     int n=pts.G_count()-1;
	 if (n<1) throw QError(_text("Unable to calculate Bezier curve: not enough points"));

     // Right hand side vector
	 Tdoublearray rhs;

     // Set right hand side values
     rhs.add(pts[0] + 2*pts[1]);
     for (int i=1; i<n-1; ++i)
         rhs.add(4*pts[i]+2*pts[i+1]);
     rhs.add(3*pts[n-1]);

     // Get first control points values
     GetFirstControlPoints(rhs,pts_ctrl1);

     // Second control points
     for (int i=0; i<n-1; ++i)
         pts_ctrl2.add(2*pts[i+1]-pts_ctrl1[i + 1]);
     pts_ctrl2.add((pts[n]+pts_ctrl1[n-1])/2);

	 calculated=true;
}

void TPolyBezier1D::copyfrom(const TPolyBezier1D &inp)
{
	pts.redim(inp.pts.G_count());
	for (int i=0; i<pts.G_count(); i++) pts[i]=inp.pts[i];
	calculated=false;
}


void TPolyBezier1D::GetFirstControlPoints(const Tdoublearray &rhs, Tdoublearray &x)
{
     int n=rhs.G_count();

	 x.redim(n);

	 Tdoublearray tmp;tmp.redim(n);

     double b=2.0;
     x[0]=rhs[0]/b;
     for (int i=1; i<n; i++) // Decomposition and forward substitution.
     {
         tmp[i]=1/b;
         b=(i<n-1 ? 4.0 : 2.0) - tmp[i];
         x[i]=(rhs[i]-x[i-1])/b;
     }
     for (int i=1; i<n; i++)
         x[n-i-1]-=tmp[n-i]*x[n-i]; // Backsubstitution.
}
*/

//****************************************************************
// TPolyBezier3D
//****************************************************************

TPolyBezier3D::TPolyBezier3D()
{
	reset();
}

TPolyBezier3D::~TPolyBezier3D()
{
}

void TPolyBezier3D::operator=(const TPolyBezier3D &v)
{
	CX.copyfrom(v.CX);
	CY.copyfrom(v.CY);
	CZ.copyfrom(v.CZ);
	isclosed=v.isclosed;
	closed_T=v.closed_T;
}


void TPolyBezier3D::reset()
{
	CX.reset();CY.reset();CZ.reset();
	isclosed=false;
	closed_T=-1;
}

void TPolyBezier3D::addpoint(double t, const Tvertex &pt)
{
	if (isclosed) throw QError(_text("Unable to add point: this curve is closed"));
	CX.add(t,pt.G3_x());
	CY.add(t,pt.G3_y());
	CZ.add(t,pt.G3_z());
}

void TPolyBezier3D::close(double iclosed_T)
{
	if (isclosed) throw QError(_text("Unable to close: this curve is already closed"));
	isclosed=true;
	closed_T=iclosed_T;
	CX.add(closed_T,CX.G_pointy(0));CX.Set_calcmethod(2);
	CY.add(closed_T,CY.G_pointy(0));CY.Set_calcmethod(2);
	CZ.add(closed_T,CZ.G_pointy(0));CZ.Set_calcmethod(2);
}


double TPolyBezier3D::G_minxval()
{
	if (CX.G_count()<=0) throw QError(_text("No spline points available"));
	if (isclosed) return 0;
	return CX.G_pointx(0);
}

double TPolyBezier3D::G_maxxval()
{
	if (CX.G_count()<=0) throw QError(_text("No spline points available"));
	if (isclosed) return closed_T;
	return CX.G_pointx(CX.G_count()-1);
}

void TPolyBezier3D::eval(double t, Tvertex &rs)
{
	rs.copyfrom(CX.eval(t),CY.eval(t),CZ.eval(t));
}

void TPolyBezier3D::evalder(double t, Tvector &rs)
{
	rs.copyfrom(CX.evalder(t),CY.evalder(t),CZ.evalder(t));
}

void TPolyBezier3D::evaldersecond(double t, Tvector &rs)
{
	rs.copyfrom(CX.evaldersecond(t),CY.evaldersecond(t),CZ.evaldersecond(t));
}



void TPolyBezier3D::tostring(QString &str, int tpe)
{
	str=description;
}

void TPolyBezier3D::fromstring(StrPtr str)
{
	ASSERT(false);
}



void TPolyBezier3D::createstring(QString &str)
{
}

void TPolyBezier3D::streamout(QBinTagWriter &writer)
{
	ASSERT(false);
}
void TPolyBezier3D::streamin(QBinTagReader &reader)
{
	ASSERT(false);
}




//****************************************************************
// TPolyBezierSurface
//****************************************************************

TPolyBezierSurface::TPolyBezierSurface()
{
	reset();
}

TPolyBezierSurface::~TPolyBezierSurface()
{
}

void TPolyBezierSurface::operator=(const TPolyBezierSurface &v)
{
	CX.copyfrom(v.CX);
	CY.copyfrom(v.CY);
	CZ.copyfrom(v.CZ);
}


void TPolyBezierSurface::reset()
{
	CX.reset();CY.reset();CZ.reset();
}

void TPolyBezierSurface::addpoint(int i1, int i2, const Tvertex &pt)
{
	CX.addpoint(i1,i2,pt.G3_x());
	CY.addpoint(i1,i2,pt.G3_y());
	CZ.addpoint(i1,i2,pt.G3_z());
}


void TPolyBezierSurface::eval(double fr1, double fr2, Tvertex &rs)
{
	rs.copyfrom(CX.eval(fr1,fr2),CY.eval(fr1,fr2),CZ.eval(fr1,fr2));
}

void TPolyBezierSurface::evalder1(double fr1, double fr2, Tvector &rs)//first order partial derivative in dir 1
{
	rs.copyfrom(CX.evalder1(fr1,fr2),CY.evalder1(fr1,fr2),CZ.evalder1(fr1,fr2));
}

void TPolyBezierSurface::evalder2(double fr1, double fr2, Tvector &rs)//first order partial derivative in dir 2
{
	rs.copyfrom(CX.evalder2(fr1,fr2),CY.evalder2(fr1,fr2),CZ.evalder2(fr1,fr2));
}



void TPolyBezierSurface::tostring(QString &str, int tpe)
{
	str=description;
}

void TPolyBezierSurface::fromstring(StrPtr str)
{
	ASSERT(false);
}



void TPolyBezierSurface::createstring(QString &str)
{
}

void TPolyBezierSurface::streamout(QBinTagWriter &writer)
{
	ASSERT(false);
}
void TPolyBezierSurface::streamin(QBinTagReader &reader)
{
	ASSERT(false);
}











///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bezier3d_addpoint,AddPoint)
{
	setmemberfunction(SC_valname_polybeziercurve);
	addvar(_qstr("t"),SC_valname_scalar);
	addvar(_qstr("point"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double t=arglist->get(0)->G_content_scalar()->G_val();
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(1));
	G_valuecontent<TPolyBezier3D>(owner)->addpoint(t,*pt);
}
ENDFUNCTION(func_bezier3d_addpoint)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bezier3d_close,Close)
{
	setmemberfunction(SC_valname_polybeziercurve);
	addvar(_qstr("T"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double T=arglist->get(0)->G_content_scalar()->G_val();
	G_valuecontent<TPolyBezier3D>(owner)->close(T);
}
ENDFUNCTION(func_bezier3d_close)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bezier3d_eval,Eval)
{
	setmemberfunction(SC_valname_polybeziercurve);
	addvar(_qstr("frac"),SC_valname_scalar);
	setreturntype(SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double frc=arglist->get(0)->G_content_scalar()->G_val();
	G_valuecontent<TPolyBezier3D>(owner)->eval(frc,*G_valuecontent<Tvertex>(retval));
}
ENDFUNCTION(func_bezier3d_eval)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bezier3d_evalder1,EvalFirstDer)
{
	setmemberfunction(SC_valname_polybeziercurve);
	addvar(_qstr("frac"),SC_valname_scalar);
	setreturntype(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double frc=arglist->get(0)->G_content_scalar()->G_val();
	G_valuecontent<TPolyBezier3D>(owner)->evalder(frc,*G_valuecontent<Tvector>(retval));
}
ENDFUNCTION(func_bezier3d_evalder1)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bezier3d_evalder2,EvalSecondDer)
{
	setmemberfunction(SC_valname_polybeziercurve);
	addvar(_qstr("frac"),SC_valname_scalar);
	setreturntype(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double frc=arglist->get(0)->G_content_scalar()->G_val();
	G_valuecontent<TPolyBezier3D>(owner)->evaldersecond(frc,*G_valuecontent<Tvector>(retval));
}
ENDFUNCTION(func_bezier3d_evalder2)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_beziersurface_addpoint,AddPoint)
{
	setmemberfunction(SC_valname_polybeziersurface);
	addvar(_qstr("i1"),SC_valname_scalar);
	addvar(_qstr("i2"),SC_valname_scalar);
	addvar(_qstr("point"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	int i1=arglist->get(0)->G_content_scalar()->G_intval();
	int i2=arglist->get(1)->G_content_scalar()->G_intval();
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(2));
	G_valuecontent<TPolyBezierSurface>(owner)->addpoint(i1,i2,*pt);
}
ENDFUNCTION(func_beziersurface_addpoint)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_beziersurface_eval,Eval)
{
	setmemberfunction(SC_valname_polybeziersurface);
	addvar(_qstr("i1"),SC_valname_scalar);
	addvar(_qstr("i2"),SC_valname_scalar);
	setreturntype(SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double fr1=arglist->get(0)->G_content_scalar()->G_val();
	double fr2=arglist->get(1)->G_content_scalar()->G_val();
	G_valuecontent<TPolyBezierSurface>(owner)->eval(fr1,fr2,*G_valuecontent<Tvertex>(retval));
}
ENDFUNCTION(func_beziersurface_eval)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_beziersurface_eval_der1,EvalDer1)
{
	setmemberfunction(SC_valname_polybeziersurface);
	addvar(_qstr("i1"),SC_valname_scalar);
	addvar(_qstr("i2"),SC_valname_scalar);
	setreturntype(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double fr1=arglist->get(0)->G_content_scalar()->G_val();
	double fr2=arglist->get(1)->G_content_scalar()->G_val();
	G_valuecontent<TPolyBezierSurface>(owner)->evalder1(fr1,fr2,*G_valuecontent<Tvector>(retval));
}
ENDFUNCTION(func_beziersurface_eval_der1)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_beziersurface_eval_der2,EvalDer2)
{
	setmemberfunction(SC_valname_polybeziersurface);
	addvar(_qstr("i1"),SC_valname_scalar);
	addvar(_qstr("i2"),SC_valname_scalar);
	setreturntype(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double fr1=arglist->get(0)->G_content_scalar()->G_val();
	double fr2=arglist->get(1)->G_content_scalar()->G_val();
	G_valuecontent<TPolyBezierSurface>(owner)->evalder2(fr1,fr2,*G_valuecontent<Tvector>(retval));
}
ENDFUNCTION(func_beziersurface_eval_der2)
