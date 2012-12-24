#pragma once

#include "tobject.h"
#include "qstring.h"
#include "qbintagfile.h"
#include "tools.h"
#include "QError.h"
#include "valnames.h"

class Tvector;
class Tline;
class TMatrix;
class Tvec3d;

class CSGVector;

class Thomogcoord : public TObject
{
protected:
	double coord[4];
public:
	Thomogcoord()
	{
		coord[0]=0;coord[1]=0;coord[2]=0;coord[3]=0;
	}
public:
	double G4_x() const { return coord[0]; }
	double G4_y() const { return coord[1]; }
	double G4_z() const { return coord[2]; }
	double G4_u() const { return coord[3]; }
	double G_coord(int i) const { ASSERT((i>=0)&&(i<4)); return coord[i]; }
	void Set_coord(int i, double vl){ ASSERT((i>=0)&&(i<4)); coord[i]=vl; }
	void S4_x(double ivl) { coord[0]=ivl; }
	void S4_y(double ivl) { coord[1]=ivl; }
	void S4_z(double ivl) { coord[2]=ivl; }
	void S4_u(double ivl) { coord[3]=ivl; }
public:
	void add(const Thomogcoord *v1, const Thomogcoord *v2);
	void subtr(const Thomogcoord *v1, const Thomogcoord *v2);
	void mul(double vl);
	void mulfrom(const Thomogcoord *v1, double vl);
	void mulfrom(const TMatrix *mt, const Thomogcoord *v);
	void lincombfrom(double f1, const Thomogcoord *v1, double f2, const Thomogcoord *v2);

	static double dot4(const Thomogcoord *v1, const Thomogcoord *v2);

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);


};

class Tvertex : public Thomogcoord
{
public:
	static StrPtr GetClassName() { return SC_valname_vertex; }
	Tvertex(double x, double y, double z, double u=1);
	Tvertex();
	Tvertex(Tvertex &v);
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
	void operator=(const Tvertex &v);
	void copyfrom(const Tvector &v);
	void copyfrom(const Tvec3d &v);
	void copyfrom(double x, double y, double z);
public:
	static bool compare(Tvertex *v1, Tvertex *v2) { return false; }
	static bool comparesize(Tvertex *v1, Tvertex *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
public:
	double G3_x() const { if (coord[3]==0) return 0; else return coord[0]/coord[3]; } 
	double G3_y() const { if (coord[3]==0) return 0; else return coord[1]/coord[3]; } 
	double G3_z() const { if (coord[3]==0) return 0; else return coord[2]/coord[3]; } 
	void S3_x(double ivl) { coord[0]=ivl*coord[3]; }
	void S3_y(double ivl) { coord[1]=ivl*coord[3]; }
	void S3_z(double ivl) { coord[2]=ivl*coord[3]; }
	void Set3(double ix, double iy, double iz) { coord[0]=ix; coord[1]=iy; coord[2]=iz; coord[3]=1; }
public:
	static double distance(const Tvertex *v1, const Tvertex *v2);
};


class Tvector : public Thomogcoord
{
public:
	static StrPtr GetClassName() { return SC_valname_vector; }
	Tvector();
	Tvector(double x, double y, double z, double u=0);
	Tvector(Tvector &v);
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
	void operator=(const Tvector &v);
	void copyfrom(double x, double y, double z);
	void copyfrom(const Tvec3d &v);
	void copyfrom(const Tvertex &v);
	void copyfrom(const CSGVector &v);
public:
	static bool compare(Tvector *v1, Tvector *v2) { return false; }
	static bool comparesize(Tvector *v1, Tvector *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
public:
	double G_x() const { return coord[0]; }
	double G_y() const { return coord[1]; }
	double G_z() const { return coord[2]; }
	void S_x(double ivl) { coord[0]=ivl; }
	void S_y(double ivl) { coord[1]=ivl; }
	void S_z(double ivl) { coord[2]=ivl; }
	void Set(double ix, double iy, double iz) { coord[0]=ix; coord[1]=iy; coord[2]=iz; coord[3]=0; }
	double G_size() const;
public:
	void subtrvertices(const Tvertex *v1, const Tvertex *v2);
	void vecprod(const Tvector *v1, const Tvector *v2);
	void normfrom(const Tvector *v1);
	void anynormalfrom(const Tvector *v1);
	void rotatefrom(const Tvector *vc, const Tvector *norm, double angle);
	void projectfrom(const Tvector *vc, const Tvector *normdir);
	static double dotprod(const Tvector *v1, const Tvector *v2);
	static double angle(const Tvector *v1, const Tvector *v2);
};


class Tplane : public Thomogcoord
{
protected:
	void normalise();
public:
	static StrPtr GetClassName() { return SC_valname_plane; }
	Tplane();
	Tplane(double x, double y, double z, double u);
	~Tplane();
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
	void operator=(const Tplane &v);
	void copyfrom(double x, double y, double z, double u);
public:
	static bool compare(Tplane *v1, Tplane *v2) { return false; }
	static bool comparesize(Tplane *v1, Tplane *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
public:
	double G_x() const { return coord[0]; }
	double G_y() const { return coord[1]; }
	double G_z() const { return coord[2]; }
	double G_u() const { return coord[3]; }
	void G_normvector(Tvector &normvector) const;
	void G_anypoint(Tvertex &pt) const;
	void Set_normvector(const Tvector &ivc);
	void Set_anypoint(const Tvertex &ipt);
	void S_x(double ivl) { coord[0]=ivl; }
	void S_y(double ivl) { coord[1]=ivl; }
	void S_z(double ivl) { coord[2]=ivl; }
	void S_u(double ivl) { coord[3]=ivl; }
	void Set(double ix, double iy, double iz, double iu) { coord[0]=ix; coord[1]=iy; coord[2]=iz; coord[3]=iu; }
	void Create(const Tvertex &pt, const Tvector &normal);
	void Create(const Tvertex &pt1, const Tvertex &pt2, const Tvertex &pt3);
	double evalpoint(const Tvertex &ipt) const;
	void findclosestpoint(const Tvertex &ipt, Tvertex &result) const;
public:
	void intersect(const Tline &line, Tvertex &point) const;
	bool isparallel(const Tplane &pl) const;
	bool isparallel(const Tline &line) const;
};




class Tline : public TObject
{
protected:
	Tvertex point;
	Tvector direction;
public:
	static StrPtr GetClassName() { return SC_valname_line; }
	Tline();
	~Tline();
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
	void operator=(const Tline &v);
public:
	static bool compare(Tline *v1, Tline *v2) { return false; }
	static bool comparesize(Tline *v1, Tline *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
public:
	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);
	void Create(const Tvertex &p1, const Tvertex &p2);
public:
	const Tvector& G_direction() const { return direction; }
	void Set_direction(const Tvector &idir) { direction.normfrom(&idir); }
	const Tvertex& G_point() const { return point; }
	void Set_point(const Tvertex &ipt) { point=ipt; }

public:
	void findclosestpoint(const Tvertex &ipt, Tvertex &result) const;
	void intersect(const Tplane &pl1, const Tplane &pl2);
};


class TLineSegment : public TObject
{
protected:
	Tvertex pt1,pt2;
public:
	static StrPtr GetClassName() { return SC_valname_linesegment; }
	TLineSegment();
	~TLineSegment();
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
	void operator=(const TLineSegment &v);
public:
	static bool compare(TLineSegment *v1, TLineSegment *v2) { return false; }
	static bool comparesize(TLineSegment *v1, TLineSegment *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
public:
	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);
	void Create(const Tvertex &p1, const Tvertex &p2);
public:
	const Tvertex& G_point1() const { return pt1; }
	const Tvertex& G_point2() const { return pt2; }
	void Set_point1(const Tvertex &ipt) { pt1=ipt; }
	void Set_point2(const Tvertex &ipt) { pt2=ipt; }

public:
	void intersect(const Tline &l1, const Tline &l2);
};
