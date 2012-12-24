
class TxtEnv;


class TxtRendererOpenGL : public TxtRenderer
{
private:
	Trendercontext *rc;
	double sizefactor;
private:
	mutable Tarray<TxtFont> fontinfos;
	mutable Tarray<CFont> fonts;
protected:
	CFont& G_font(const TxtFont &fntinfo) const;
	void selectfont(const TxtFont &fntinfo) const;
	TQXColor G_textcolor() const;
	TQXColor G_linkcolor() const;

public:
	void Set_sizefactor(double ival) { sizefactor=ival; }
	void Set_rendercontext(Trendercontext *irc) { rc=irc; }
	void check_rcpresent() const;
	StrPtr G_fontname(const TxtFont &fnt) const;

public:
	TxtRendererOpenGL();
	virtual double G_textsizex(const TxtFont &fnt, StrPtr itxt) const;
	virtual double G_textsizey(const TxtFont &fnt, StrPtr itxt) const;
	virtual double G_fontsize(const TxtFont &fnt) const;
	virtual double G_textbaseline(const TxtFont &fnt, StrPtr itxt) const;
	virtual void showtext(const TxtFont &fnt, double x0, double y0, StrPtr itxt) const;

	virtual void render_horizontalline(double x0, double y0, double xlen, double width) const;
	virtual void render_verticalline(double x0, double y0, double ylen, double width) const;
	virtual void render_horizontalarrow(double x0, double y0, double xlen, double width) const;
	virtual void render_polyline(Tdoublearray &px, Tdoublearray &py, double width) const;

	virtual void render_boundingbox(double x0, double y0, double sizex, double sizey, double baseline) const;

};


class T3DObject3DFormattedText :  public T3DGeoObject
{
private:
	TxtRendererOpenGL renderer;
	TxtEnv *env;
	bool formatted;
private:
	TSC_string *content;
	TSC_scalar *size,*maxlenx;
public:
	double rendered_textsizey;//only present after rendering!
protected:
	virtual void paramchanged(StrPtr iname);
	void formattext(Trendercontext *rc);
public:
	static StrPtr GetClassName() { return SC_valname_formattedtext3d; }
	T3DObject3DFormattedText(T3DScene *iscene=NULL);
	~T3DObject3DFormattedText();
	virtual StrPtr G_classname() { return SC_valname_formattedtext3d; }
	virtual T3DObject* CreateInstance() { return new T3DObject3DFormattedText(NULL); }
	double G_textsizex();
public:
	virtual void precalcrender_exec(Tprecalcrendercontext *pre);
	void render_exec(Trendercontext *rc);
};


