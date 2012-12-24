
#include "3Dcosmos.h"

class PEengineFrame
{
public:
	TSC_time time;
	Tcheapitemarray<float> positx,posity,positz;
	Tcheapitemarray<unsigned char> colr,colg,colb,cola;
public:
	PEengineFrame(int icount, const TSC_time &itm);
	void check_validnr(int nr);
	void setposit(int nr, Tvertex &posit);
	void setcolor(int nr, TSC_color &color);
	Tvertex G_posit(int nr);
	TSC_color G_color(int nr);
};

class T3DObjectPEngine : public T3DObject
{
protected:
	Titemarray<Tvec3d> particle_posit,particle_posit_sorted;
	Titemarray<Tvector> particle_speed;
	Titemarray<T4color> particle_color,particle_color_sorted;
	Tdoublearray particle_sizefactor;
private:
	Titemarray<Tvertex> positions;
	Tintarray sortidx;
	Tdoublearray dists;
	Titemarray<Tvec3d> vertexbuffer;
	Titemarray<T4color> colorbuffer;
	Titemarray<T2textureidx> texturebuffer;

	Tarray<PEengineFrame> frames;

private:
	TSC_scalar *pointsize;
	TSC_color *color;
	TSC_boolean *sort,*hasowncolors;
	TSC_value *rendertype,*blendtype,*depthmask;
	TSC_color rendercolor;
	TSC_string *motionname;
public:
public:
	T3DObjectPEngine(T3DScene *iscene=NULL);
	static StrPtr GetClassName() { return SC_valname_pengine; }
	virtual StrPtr G_classname() { return SC_valname_pengine; }
	virtual T3DObject* CreateInstance() { return new T3DObjectPEngine(NULL); }
	void Set_motionname(StrPtr iname);
	T3DMotion* G_motion();
	void setsize(int icount);
	int add();
	void addframe(const TSC_time &itm);
	PEengineFrame* G_frame(int framenr);
	void interpolframes(const TSC_time &tm);
	void particle_setposit(int nr, Tvertex &posit);
	void particle_setspeed(int nr, Tvector &speed);
	void particle_setcolor(int nr, TSC_color &color);
	void particle_setsizefactor(int nr, double ival);
	Tvertex particle_G_posit(int nr);
	Tvector particle_G_speed(int nr);
	TSC_color particle_G_color(int nr);
	double particle_G_sizefactor(int nr);
	bool G_hasowncolors() { return hasowncolors->G_val(); }
public:
	virtual void adjusttime(double timestep);
	virtual void precalcrender(Tprecalcrendercontext *pre);
	virtual void precalcrender2(T3DRenderViewport *rvp, Tprecalcrendercontext *pre);
	virtual void render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording);
	void render_quads(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording);
	void render_points(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording);
	bool G_cancache() { return false; };
	StrPtr G_texturename() { return G_paramstring(_qstr("Texture")); }

	virtual void streamout_content(QBinTagWriter &writer);
	virtual void streamin_content(QBinTagReader &reader);

};