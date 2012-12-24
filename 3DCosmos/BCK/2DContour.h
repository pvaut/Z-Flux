#pragma once

#include "vecmath.h"

class TSC_functor;

class T2DContourPoint
{
public:
	Tvertex pt;
	Tvector nm;
	double texcoord;
	bool normalpresent;
	int ptnr,PTID;
public:
	T2DContourPoint()
	{
		texcoord=0;
		normalpresent=false;
		ptnr=-1;
		PTID=-1;
	}
	void operator=(const T2DContourPoint &v)
	{
		pt=v.pt;
		nm=v.nm;
		normalpresent=v.normalpresent;
		ptnr=v.ptnr;
		PTID=v.PTID;
		texcoord=v.texcoord;
	}
};

class T2DContour
{
private:
	Tvertex center;
	double minx,maxx,miny,maxy;
	bool closed;
public:
	Titemarray<T2DContourPoint> points;
	T2DContour();
	void addpoint(int PTID, Tvertex *pt, Tvector *nm=NULL);
	void calcdims();
	void calccirctexcoord();
	void operator=(const T2DContour &v);
	bool G_closed() { return closed; }
	int G_pointcount() { return points.G_count(); }
	T2DContourPoint& G_point(int nr) { return points[nr]; }
};

class T2DContourset : public TObject
{
private:
	QString description;
	Tarray<T2DContour> contours;
	int curcontour;
	int curPTID;
public:
	static StrPtr GetClassName() { return SC_valname_2dcontourset; }
	T2DContourset();
	~T2DContourset();
	void operator=(const T2DContourset &v);
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
public:
	static bool compare(T2DContourset *v1, T2DContourset *v2) { return false; }
	void createstring(QString &str);
	void reset();
	void newcontour();
	void addpoint(Tvertex *pt, Tvector *nm=NULL);
	void generate(TSC_functor *functor, double minv, double maxv, int count);
	int G_contourcount() { return contours.G_count(); }
	T2DContour* G_contour(int nr) { return contours[nr]; }
};
