#pragma once

#include "3dscene.h"

class Tvertex;
class TSC_functor;
class TMatrix;
class TPolyBezierSurface;


class T3DObjectSurface :  public T3DGeoObjectPreCalcMesh
{
public:
	static StrPtr GetClassName() { return SC_valname_surface; }
private:
	int dim1,dim2;
	bool hasowncolors;
	Tarray<Tcheapitemarray<Tvertex> > vertices;
	Tarray<Tcheapitemarray<Tvector> > normals;
	Tarray<Tcheapitemarray<TSC_color> > colors;
	Tarray<Tcheapitemarray<Ttexor> > texors;
public:
	T3DObjectSurface(T3DScene *iscene=NULL);
	virtual StrPtr G_classname() { return SC_valname_surface; }
	virtual T3DObject* CreateInstance() { return new T3DObjectSurface(NULL); }
public:
	virtual bool paramchanged_needrecalculate(StrPtr paramname);
	void calculate();
	void setdim(int idim1, int idim2);
	void generate(TSC_functor *functor, double min1, double max1, int count1, double min2, double max2,int count2);
	void generate(TPolyBezierSurface *src, int count1, int count2);
	void generatevertexproperty(TSC_functor *functor, int vertexproperty);
	void copyfrom(T3DObjectSurface *isf);
	void Set_vertex(int i1, int i2, Tvertex *vertex);
	Tvertex* G_vertex(int i1, int i2);
	void Set_normal(int i1, int i2, Tvector *normal);
	Tvector* G_normal(int i1, int i2);
	void Set_color(int i1, int i2, TSC_color *color);
	TSC_color* G_color(int i1, int i2);
	void Set_texor(int i1, int i2, double tx1, double tx2);

	void CalcNormals();//calculate approximative normals from the vertex data

	void LoadTopoSurface(TMatrix *mtr, double  ixmin, double ixmax, double iymin, double iymax, double ihfactor);

	void LoadTopoSphere(TMatrix *mtr, double  ilongmin, double ilongmax, double ilattmin, double ilattmax, double ihfactor, double iradius);

	void copytoframe(double phase);
	void interpolframe(double phase);
};
