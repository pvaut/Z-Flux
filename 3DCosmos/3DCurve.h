#pragma once

#include "3dscene.h"
#include "3dmesh.h"

class Tvertex;
class TSC_functor;
class TPolyBezier3D;


class T3DObjectCurve : public T3DGeoObject
{
protected:
	bool hasowncolors;
	TSC_scalar *size,*arrow1size,*arrow2size;
	TSC_boolean *isclosed;
	Titemarray<Tvec3d> points;
	Titemarray<T4color> colors;
	Titemarray<Tvec3d> dir1,norm1,norm2;

	Titemarray<Tvec3d> vertices,vertices_normals;
	Titemarray<T2textureidx> vertices_texcoord;

	TSpecificObjectRef<T3DSubFrameObject> trackframe;
	Tvertex track_lastpoint;
	double mintrackdist;

private:
	TSC_value *curverendertype;
	void calculate();
	bool calculated;
public:
	T3DObjectCurve(T3DScene *iscene=NULL);
	static StrPtr GetClassName() { return SC_valname_curve; }
	virtual StrPtr G_classname() { return SC_valname_curve; }
	virtual T3DObject* CreateInstance() { return new T3DObjectCurve(NULL); }
	virtual void paramchanged(StrPtr iname);
public:
	int G_count() { return points.G_count(); }
	double G_size() { return size->G_val(); }
	bool G_isclosed() { return isclosed->G_val(); }
	int G_curverendertype() { return curverendertype->toint(); }
	void reset();
	void addpoint(Tvertex *pt);
	void Set_color(int idx, const TSC_color &icol);
	void close();
	void generate(TSC_functor *functor, double minv, double maxv, int count);
	void generate(TPolyBezier3D *bezier, int count);
	void transform(const Taffinetransformation *transf);
	void makecircle(const Tvertex *center, const Tvector *normal, double radius, int resolution);
	void makearc(const Tvertex *center, const Tvector *dir1, const Tvector *dir2, double radius, int resolution);
	virtual bool G_cancache();
	void set_track(T3DSubFrameObject *itrackframe, double imintrackdist);
	void stop_track();
	void track_update();

protected:
	void precalcrender_exec(Tprecalcrendercontext *pre);
	void renderarrow(Trendercontext *rc, Tvec3d &ptend, Tvec3d &pt2, double size);
	void render_exec(Trendercontext *rc);

	virtual void streamout_content(QBinTagWriter &writer);
	virtual void streamin_content(QBinTagReader &reader);

};
