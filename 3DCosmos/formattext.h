#pragma once

#include "tools.h"

#include "qxcolor.h"

//#define BASELINEFRAC 0.75

class TxtEnv;
class TxtFont;

bool trykeyword(QString &content, StrPtr keyword);//determines if a string starts with a keyword, and splits if if true
bool getargument(QString &content, QString &subblock, bool alwaysreturnblock);//splits a {} subblock at the first position from a string
void splitblock_bykeyword(QString &content, QString &subblock, StrPtr openingkeyword, StrPtr closingkeyword);
void splitblock(QString &content, QString &subblock, StrPtr token);//splits at a token, treating \begin{} and \end{} as groups
void eatstartspaces(QString &content);
void eatendspaces(QString &content);

double ConvertSize(StrPtr content, const TxtFont &ifnt);//converts a string to a size measure


class TxtHyperlinklist
{
private:
	Tdoublearray posx,posy,sizex,sizey;
	Tarray<QString> ID;
public:
	void clear();
	void add(double iposx, double iposy, double isizex, double isizey, StrPtr iID);
	StrPtr G_linkat(double psx, double psy) const;
};

class TxtFont
{
public:
	typedef enum { FNTSTYLE_NORMAL, FNTSTYLE_BOLD, FNTSTYLE_ITALIC, FNTSTYLE_SYMBOL, FNTSTYLE_FIXED } Tfontstyle;
private:
	double size;
	Tfontstyle fontstyle;
	QString hyperlinkID;
	TQXColor colormixer;
public:
	TxtFont(double isize);
	TxtFont(const TxtFont &ifnt);
	void operator=(const TxtFont &ifnt);
	bool operator==(const TxtFont &f2) const;
	void copyfrom(const TxtFont &ifnt);
	double G_size() const { return size; }
	Tfontstyle G_fontstyle() const { return fontstyle; }
	void Set_size(double isize) { size=isize; }
	void makebold() { fontstyle=FNTSTYLE_BOLD; }
	void makeitalic() { fontstyle=FNTSTYLE_ITALIC; }
	void makesymbol() { fontstyle=FNTSTYLE_SYMBOL; }
	void makefixed() { fontstyle=FNTSTYLE_FIXED; }
	void changesize(double fc) { size=size*fc; }
	void Set_hyperlink(StrPtr iID);
	StrPtr G_hyperlink() const { return hyperlinkID; }
	void Set_color(const TQXColor &icl) { colormixer=icl; }
	const TQXColor& G_color() const { return colormixer; }
};

class TxtRenderer
{
private:
	double maxsizex;
public:
	TxtRenderer();
	virtual ~TxtRenderer();
	double G_maxsizex() const { return maxsizex; }
	virtual double G_textsizex(const TxtFont &fnt, StrPtr itxt) const=0;
	virtual double G_textsizey(const TxtFont &fnt, StrPtr itxt) const=0;
	virtual double G_fontsize(const TxtFont &fnt) const=0;
	virtual double G_textbaseline(const TxtFont &fnt, StrPtr itxt) const=0;
	virtual void showtext(const TxtFont &fnt, double x0, double y0, StrPtr itxt) const=0;

	virtual void render_horizontalline(double x0, double y0, double xlen, double width) const=0;
	virtual void render_verticalline(double x0, double y0, double ylen, double width) const=0;
	virtual void render_horizontalarrow(double x0, double y0, double xlen, double width) const=0;

	virtual void render_boundingbox(double x0, double y0, double sizex, double sizey, double baseline) const=0;

	virtual void render_polyline(Tdoublearray &px, Tdoublearray &py, double width) const=0;
};

class TxtComponent
{
public:
	typedef enum { TXTCOMP_UNKNOWN, TXTCOMP_SPACE, TXTCOMP_INDEX } TxtCompType;
protected:
	TxtEnv *env;
	TxtCompType comptpe;
	bool startnewlinebefore,startnewlineafter;
	bool writesubscriptunder;//false: sub/super scripts on the right of the block. false: under or over the block
	double posx,posy;//top left corner, relative to parent object
	double sizex,sizey;
	double baseline;//offset from top
	TxtFont fnt;
public:
	TxtComponent(TxtEnv *ienv, const TxtFont &ifnt);
	virtual ~TxtComponent();
	TxtCompType G_type() { return comptpe; }
	bool G_startnewlinebefore() { return startnewlinebefore; }
	bool G_startnewlineafter() { return startnewlineafter; }
	virtual void parsefrom(StrPtr icontent) { ASSERT(false); }
	void calc(const TxtRenderer &renderer, double maxsizex);
	void render(double xoffs, double yoffs, const TxtRenderer &renderer) const;
	void buildhyperlinklist(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const;
	void Set_posx(double ival) { posx=ival; }
	void Set_posy(double ival) { posy=ival; }
	void Set_sizex(double ival) { sizex=ival; }
	void Set_sizey(double ival) { sizey=ival; }
	double G_posx() const { return posx; }
	double G_posy() const { return posy; }
	double G_sizex() const { return sizex; }
	double G_sizey() const { return sizey; }
	double G_baseline() const { return baseline; }
	bool G_writesubscriptunder() const { return writesubscriptunder; }
	void makestandaloneblock() { startnewlinebefore=true;startnewlineafter=true; }
	const TxtFont& G_fnt() const { return fnt; }
public:
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex)=0;
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const { ASSERT(false); }
	virtual void buildhyperlinklist_impl(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const {};
};


//implementation of a word
class TxtWord : public TxtComponent
{
protected:
	QString content;
protected:
public:
	TxtWord(TxtEnv *ienv, const TxtFont &ifnt, StrPtr icontent);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
	virtual void buildhyperlinklist_impl(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const;
};


//implementation of a whitespace
class TxtSpace : public TxtComponent
{
protected:
public:
	TxtSpace(TxtEnv *ienv, const TxtFont &ifnt);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};

//implementation of a line break
class TxtLineBreak : public TxtComponent
{
protected:
public:
	TxtLineBreak(TxtEnv *ienv, const TxtFont &ifnt);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};


//implementation of a list
class TxtList : public TxtComponent
{
protected:
	QString bulletstring;
	TxtFont bulletfnt;
	Tarray<TxtComponent> items;
	Tarray<QString> itembullets;
protected:
public:
	TxtList(TxtEnv *ienv, const TxtFont &ifnt, StrPtr icontent, StrPtr ibulletstring);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
	virtual void buildhyperlinklist_impl(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const;
};


//implementation of a text block, consisting in a list of components (e.g. a paragraph)
class TxtBlock : public TxtComponent
{
private:
	bool calcfirstline;
protected:
	Tarray<TxtComponent> components;
protected:
	void format_addline(const TxtRenderer &renderer, int i1, int i2, double yp, double &maxh);
	void parsesub(const TxtFont &currentfnt, StrPtr icontent);
public:
	TxtBlock(TxtEnv *ienv, const TxtFont &ifnt);
	virtual void parsefrom(StrPtr icontent);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
	virtual void buildhyperlinklist_impl(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const;
};

//implementation of a text document, consisting in a list of vertically listed components
class TxtDoc : public TxtComponent
{
protected:
	Tarray<TxtComponent> components;
protected:
	void addparagraph(StrPtr icontent);
public:
	TxtDoc(TxtEnv *ienv, const TxtFont &ifnt);
	virtual void parsefrom(StrPtr icontent);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
	virtual void buildhyperlinklist_impl(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const;
};

class TxtEnv
{
private:
	TxtDoc *doc;
public:
	TxtEnv(double ifontsize);
	virtual ~TxtEnv();
	void parsefrom(StrPtr icontent);
	void calc(const TxtRenderer &renderer, double maxsizex);
	void render(double xoffs, double yoffs, const TxtRenderer &renderer) const;
	void buildhyperlinklist(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const;
	double G_sizey() const;
private:
	Tarray<QString> customkeywords;
public:
	void addcustomkeyword(StrPtr ikeyword);
	int G_customkeywordcount() const;
	StrPtr G_customkeyword(int nr) const;

	virtual void processcustomkeyword(StrPtr keyword, StrPtr in, QString &out) {}
};


class TxtRendererGDI : public TxtRenderer
{
private:
	CDC *dc;
	mutable Tarray<TxtFont> fontinfos;
	mutable Tarray<CFont> fonts;
protected:
	CFont& G_font(const TxtFont &fntinfo) const;
	void selectfont(const TxtFont &fntinfo) const;
	TQXColor G_textcolor() const;
	TQXColor G_linkcolor() const;
public:
	TxtRendererGDI(CDC *idc);
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

/*
class TxtParser
{
private:
public:
	void parse(StrPtr icontent);
};
*/