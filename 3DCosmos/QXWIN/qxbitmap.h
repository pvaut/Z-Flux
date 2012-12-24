#pragma once

#include "qxdrawer.h"

class TQXDIBitmap;


//implements a drawer for bitmaps that are part of the resources of the application
class TQXbitmapdrawer : public TQXdrawer
{
	int ID,resx,resy;
	CBitmap bitmap;
public:
	TQXbitmapdrawer();
	TQXbitmapdrawer(int iID);
	void init(int iID);
	virtual int G_resx() const;
	virtual int G_resy() const;
	virtual void draw(CDC *dc, int x0, int y0);
	HBITMAP G_handle()
	{
		return bitmap;
	}
};


//implements a bitmap used for in-memory drawing
class TQXmembitmap
{
private:
	int resx,resy;
	CBitmap *bitmap;
	CDC *memdc;
public:
	TQXmembitmap();
	~TQXmembitmap();
	CDC* start(CDC *dc, int iresx, int iresy);//prepares for drawing, returns memory DC for drawing
	void stop();
	void transfer(CDC *dc, int x0, int y0);
	HBITMAP G_bitmaphandle()
	{
		ASSERT(bitmap!=NULL);
		if (bitmap==NULL) return NULL;
		return  (HBITMAP)(*bitmap);
	}
};

//implements an internal gray map that can be both positive and negative
class TQXgraycorrmap
{
	friend class TQXDIBitmap;
private:
	int xres,yres;
	signed char *dat;
public:
	TQXgraycorrmap();
	~TQXgraycorrmap();
	void init(int ixres, int iyres);
	void vertgrad(double val1, double val2, int tpe);
	void add(TQXgraycorrmap &map, int x0, int y0);
	signed char Get(int ix, int iy)
	{
		if ((ix<0)||(iy<0)||(ix>=xres)||(iy>=yres)) return 0;
		return dat[iy*xres+ix];
	}
	void Set(int ix, int iy, signed char val)
	{
		if ((ix<0)||(iy<0)||(ix>=xres)||(iy>=yres)) return;
		dat[iy*xres+ix]=val;
	}
	void copyfrom(TQXDIBitmap &map);
	void clonehorline(int px, int lx, int py1, int py2);
	void clonevertline(int py, int ly, int px1, int px2);
};

//implements a bump map that can be converted to a gray correction map
class TQXbumpmap
{
private:
	double ldx,ldy,ldz;//light dir
	double sdx,sdy,sdz;//specular dir
private:
	int xres,yres;
	double *dat;
private:
	double calc(double dx, double dy, double dz);
public:
	TQXbumpmap();
	~TQXbumpmap();
	void init(int ixres, int iyres);
	void render(TQXgraycorrmap &map);
	double Get(int ix, int iy)
	{
		if ((ix<0)||(iy<0)||(ix>=xres)||(iy>=yres)) return 0.0;
		return dat[iy*xres+ix];
	}
	void Set(int ix, int iy, double val)
	{
		if ((ix<0)||(iy<0)||(ix>=xres)||(iy>=yres)) return;
		dat[iy*xres+ix]=val;
	}
};

//implements an internal DI bitmap
class TQXDIBitmap : public CObject
{
private:
	char present;
	int xs,xres,yres;
	BITMAPINFO bminfo;
	unsigned char *dat;
public:
	TQXDIBitmap();
	~TQXDIBitmap();
	void init(int ixres, int iyres, TQXColor &col);
	void vertgrad(TQXColor &col1, TQXColor &col2, int type);
	void horgrad(TQXColor &col1, TQXColor &col2, int type);
	void copyfrom(HDC hdc, HBITMAP hbm);
	void render(CDC *dc, int x0, int y0);
	void G_color(int px, int py, unsigned char &rr, unsigned char &gg, unsigned char &bb);
	void G_color(int px, int py, int &rr, int &gg, int &bb);
	void Set_color(int px, int py, unsigned char rr, unsigned char gg, unsigned char bb);
	void fliphor();
	void flipvert();

	void apply(TQXgraycorrmap &corrmap, int x0, int y0);//corrmap: scaled between -127 and 127
	void applycolormix(TQXgraycorrmap &transpmap, TQXColor &col, int x0, int y0);//transpmap: scaled between 0 and 127
	void replacecolor(TQXDIBitmap &bmp2, int bcolr, int bcolg, int bcolb);
};
