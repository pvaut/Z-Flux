#pragma once

#include "qstring.h"
#include "qbintagfile.h"
#include "tobject.h"

class Tbmp : public TObject
{
	friend class Tbmpdisp;
private:
	int present,xres,yres;
	int format;// 1=8 bit gray ;  2=16 bit gray  ;  3=24 bit RGB
	QString filename,error;
	unsigned char *val8;
	unsigned char *val8r,*val8g,*val8b;
	unsigned __int16 *val16;
	int minval,maxval;
public:
	static StrPtr GetClassName() { return _qstr("Bitmap"); }
	Tbmp();
	~Tbmp();
	void clear();
	int loadgdiplus(StrPtr ifilename);
	int loadthumbnail(StrPtr ifilename);
	void savejpg(StrPtr filename, int qua);
	void init(int iresx, int iresy, int iformat, int iminval, int imaxval);
	StrPtr G_error() { return error; }
	StrPtr G_filename() { return filename; }
	int G_present() { return present; }
	int G_format() { return format; }
	int G_xres() const { return xres; }
	int G_yres() const { return yres; }
	int G_minval() { return minval; }
	int G_maxval() { return maxval; }
	unsigned char* G_val8ptr() { return val8; }
	unsigned char* G_val8rptr() { return val8r; }
	unsigned char* G_val8gptr() { return val8g; }
	unsigned char* G_val8bptr() { return val8b; }

	void copyfrom(const Tbmp *bmp);
	void cropfrom(const Tbmp *bmp, int x0, int y0, int lx, int ly, double rotat);
	void reducefrom(const Tbmp *bmp, int redfac);
	void insert(Tbmp *bmp, int px, int py);
	void mirrordiag();
	void mirrorhoriz();
	void mirrorvert();
	void convertgray(int channel);
	void invert();
	void expandluminosityrange();
	void extractcolor(double rfac, double gfac, double bfac);

public:
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
	void operator=(const Tbmp &bmp);
	static bool compare(Tbmp *v1, Tbmp *v2) { return false; }
	static bool comparesize(Tbmp *v1, Tbmp *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);
};
