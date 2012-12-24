#pragma once

#include "tobject.h"
#include "tools.h"
#include "QError.h"
#include "vecmath.h"


class TCubicSpline
{
private:
	Tdoublearray X,Y,b,c,d;
	bool calculated;
	int calcmethod;//0=not-a-knot splines 1=Catmull-Rom  2=Catmull-Rom
private:
	void calc_notaknot();
	void calc_CatmullRom(bool isclosed);
public:
	TCubicSpline();
	void Set_calcmethod(int icalcmethod);
	void reset();
	void add(double iX, double iY);
	void calc();
	double eval(double iX);
	double evalder(double iX);
	double evaldersecond(double iX);
	void copyfrom(const TCubicSpline &inp);
	int G_count() const;
	double G_pointx(int nr) const;
	double G_pointy(int nr) const;
	double G_coef(int nr, int deg);
};


class TCubicSplineSurfacePatch
{
public:
	double coefs[4][4];
public:
	double eval(double u1, double u2);
	double evalder1(double u1, double u2);//partial derivative with respect to u1
	double evalder2(double u1, double u2);//partial derivative with respect to u2
};

class TCubicSplineSurface
{
private:
	bool calculated;
	Tarray< Tdoublearray > Y;

	int dim1,dim2;
	Tarray< Tarray<TCubicSplineSurfacePatch> > patches;
private:
	void calcpatch(double fr1, double fr2, int &i1, int &i2, double &u1, double &u2);

public:
	TCubicSplineSurface();
	void reset();
	void addpoint(int i1, int i2, double iy);
	void calculate();
	void copyfrom(const TCubicSplineSurface &inp);
	double eval(double fr1, double fr2);
	double evalder1(double fr1, double fr2);//partial derivative with respect to dim1
	double evalder2(double fr1, double fr2);//partial derivative with respect to dim2
};

/*
class TPolyBezier1D
{
private:
	Tdoublearray pts,pts_ctrl1,pts_ctrl2;
	bool calculated;

	void GetFirstControlPoints(const Tdoublearray &rhs, Tdoublearray &x);

public:
	TPolyBezier1D();
	void reset();
	void add(double ival);
	void calccontrols();

	void copyfrom(const TPolyBezier1D &inp);

	double eval(double ifrc);
	double evalder(double ifrc);
	double evalder2(double ifrc);
};
*/


class TPolyBezier3D : public TObject
{
private:
	QString description;
	TCubicSpline CX,CY,CZ;
	bool isclosed;
	double closed_T;//'period' of the closed loop
public:
	static StrPtr GetClassName() { return SC_valname_polybeziercurve; }
	TPolyBezier3D();
	~TPolyBezier3D();
	void operator=(const TPolyBezier3D &v);
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
public:
	static bool compare(TPolyBezier3D *v1, TPolyBezier3D *v2) { return false; }
	static bool comparesize(TPolyBezier3D *v1, TPolyBezier3D *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
	void createstring(QString &str);
	void reset();
	void addpoint(double t, const Tvertex &pt);
	void close(double iclosed_T);
	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

	bool G_isclosed() { return isclosed; }
	double G_closed_T() { return closed_T; }
	double G_minxval();
	double G_maxxval();

	void eval(double t, Tvertex &rs);
	void evalder(double t, Tvector &rs);
	void evaldersecond(double t, Tvector &rs);

};


class TPolyBezierSurface : public TObject
{
private:
	QString description;
	TCubicSplineSurface CX,CY,CZ;
public:
	static StrPtr GetClassName() { return SC_valname_polybeziersurface; }
	TPolyBezierSurface();
	~TPolyBezierSurface();
	void operator=(const TPolyBezierSurface &v);
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
public:
	static bool compare(TPolyBezierSurface *v1, TPolyBezierSurface *v2) { return false; }
	static bool comparesize(TPolyBezierSurface *v1, TPolyBezierSurface *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
	void createstring(QString &str);
	void reset();
	void addpoint(int i1, int i2, const Tvertex &pt);
	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

	void eval(double fr1, double fr2, Tvertex &rs);
	void evalder1(double fr1, double fr2, Tvector &rs);//first order partial derivative in dir 1
	void evalder2(double fr1, double fr2, Tvector &rs);//first order partial derivative in dir 2

};


