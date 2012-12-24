#pragma once
#include "3DGeoObjects.h"
#include "2DContour.h"

class CSGShape;

class T3DObjectMesh :  public T3DGeoObjectPreCalcMesh
{
private:
public:
	static StrPtr GetClassName() { return SC_valname_meshobject; }
	T3DObjectMesh(T3DScene *iscene=NULL);
	virtual StrPtr G_classname() { return GetClassName(); }
	virtual T3DObject* CreateInstance() { return new T3DObjectMesh(NULL); }
	virtual bool paramchanged_needrecalculate(StrPtr paramname);
	virtual void calculate();

	void copyfrom(const CSGShape &shape);

	void SetColor(int label,TSC_color &color);
	void generatevertexproperty(TSC_functor *functor, int vertexproperty);
	void CreateFlatPolygon(T2DContourset *contourset);
	void CreateExtrudedPolygon(T2DContourset *contourset, double height, bool closed, int layercount);
	void CreateRevolvedPolygon(T2DContourset *contourset, double iangle, int resolution);


	void transform(Taffinetransformation *tf);
	void warpspiral(double winding);
	void warpconalpinch(double toph);
};
