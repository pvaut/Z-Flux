#include "qstring.h"
#include "tools.h"

#include <vfw.h>

class Taviplayer : public CObject
{
public:
	bool opened;
	int resx,resy,framecount;
	double framedelay;
	int curframe;
private:
	AVISTREAMINFO		psi;										// Pointer To A Structure Containing Stream Info
	PAVISTREAM			pavi;										// Handle To An Open Stream
	PGETFRAME			pgf;										// Pointer To A GetFrame Object
public:
	Taviplayer();
	~Taviplayer();
	void load(StrPtr filename);
	void close();
	void setcurframe(int framenr);
	LPBITMAPINFOHEADER getframe(int nr);
	LPBITMAPINFOHEADER getcurframe();
};

Taviplayer* getaviplayer(const char *alias);


class Tvideotexture : public CObject
{
private:
	Taviplayer *avi;
	int resx,resy;
	unsigned char *data;
	GLuint texid;

	BITMAPINFOHEADER bmih;
	HDRAWDIB hdd;
	HBITMAP hBitmap;
	HDC hdc;

public:
	Tvideotexture(Taviplayer *iavi);
	~Tvideotexture();
	void loadcurframe();
	void select();
	void unselect();
};

class Tvideocapture : public CObject
{
private:
	CWnd *win;
	int resx,resy;
	QString filename;
	IAVIFile *pfile;
	PAVISTREAM pavi,cpavi;
private:
	HDRAWDIB hdd;
	HDC hdc;
	HBITMAP hBitmap;
	unsigned char *data;
public:
	int framenr;
	bool recording;
public:
	Tvideocapture();
	void start(CWnd *iwin, StrPtr ifilename);
	void stop();
	void addframe();
};
