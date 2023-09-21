#pragma once

#define DEFAULT_FONT _qstr("Default")
#define BOLD_FONT _qstr("Bold")
#define ITALIC_FONT _qstr("Italic")
#define SYMBOL_FONT _qstr("Symbol")


//FOR 3DConnexion
#define _ATL_ATTRIBUTES
#include <atlbase.h>
#include <atlcom.h>


//#include "gl/glaux.h"

#include "SC_color.h"

#include "RBO.h"
#include "FBO.h"

#include "WarpMesh.h"


class TShader;


class Trendercontext;
class TRenderWindow;
class T3DScene;
class T3DViewport;
class T3DTexture;
class TDisplayDevice;
class Taffinetransformation;
class TDisplayList;
class T3DTexture;
class T3DFont;
class Tbmp;

// outcommented for now, because the include file is alas... not available!
//#define _3DConnexion

#ifdef _3DConnexion
#include "tdxinput.tlh"
//	#import "progid:TDxInput.Device" embedded_idl no_namespace
#endif





class Trendertexture
{
private:
	unsigned char* data;
	bool loaded,hasalpha;
	GLuint textureid;
	int xres,yres;
public:
	Trendertexture();
	~Trendertexture();
	void clear();
	void loadbitmapfile(StrPtr ifilename, StrPtr ibitmapalphafile=NULL);
	void loadbitmap(Tbmp *bmp, Tbmp *bmpalpha=NULL);
	GLuint G_textureid() { return textureid; };
	void select(Trendercontext *rc);
	void unselect(Trendercontext *rc);
	bool G_ispresent() { return loaded; }
	int G_xres() { if (!loaded) return 1; return xres; }
	int G_yres() { if (!loaded) return 1; return yres; }
};

class Trendercontext : public TObject
{
	friend class TRenderWindow;
private:
	TRenderWindow *rw;
	QString viewportname;
	bool Hmirrorred,Vmirrorred;
	double temp_mat44[16];
	Tvector tempvecx,tempvecy,tempvecz;
	Tarray<TDisplayList> displaylists;
	Tvector eyedir_horiz,eyedir_vert,eyedir_depth;
	Tvertex camerapos;
	bool lightenabled;
	//TShader *colortransform;
public:
	Trendertexture lightpoint1;
	bool isrightpart;
public:
	enum RenderPass { RP_SOLID,RP_TRANSPARENT };
	RenderPass currentrenderpass;
	void SetBlendType(int blendtype);//of type SC_valname_blendtype
	void EnableDepthTest(bool enabled);
	void EnableDepthMask(bool enabled);
	void EnableRescaleNormal(bool enabled);
	StrPtr G_viewportname() { return viewportname; }
	TRenderWindow* G_renderwindow() { return rw; }
	T3DViewport* G_viewport();
	Tvector* G_eyedir_horiz() { return &eyedir_horiz; }
	Tvector* G_eyedir_vert() { return &eyedir_vert; }
	Tvector* G_eyedir_depth() { return &eyedir_depth; }
	Tvertex* G_camerapos() { return &camerapos; }

	bool G_isHmirrored();
	bool G_isVmirrored();

public:
	Trendercontext(TRenderWindow* irw);
	~Trendercontext();
	void setup();
	void checkerror(StrPtr comment);
	void initrender(RenderPass irenderpass);
	void activate();
	void deactivate();
	bool G_isrightwindow();
	bool G_istextureactive();
	bool G_lightenabled() { return lightenabled; }
	void setnearfarplane(double nearplane, double farplane);
	void setdefaultnearfarplane();
	void enablelight(bool enabled);
	void set_light0(Tvertex *posit, TSC_color *ambientcolor, TSC_color *diffusecolor, TSC_color *specularcolor);
	void set_color(double R, double G, double B, double A);
	void set_speccolor(TSC_color *color, double specval);
	void set_orientation(int orient);
	void create_subframe_orig();
	void create_subframe_fulltransformation(Taffinetransformation *transf);//applies a full transformation from start
	void create_subframe(Taffinetransformation *transf);//applies an incremental transformation from the previous state
	void create_subframe_translated(Tvector &shift);
	void create_subframe_viewdir(Tvertex *posit);
	void create_subframe_screen();
	void create_subframe_viewport();
	void start_viewportframe(bool preserveaspectratio=true);
	void del_subframe();
	void rendertext(StrPtr fontname, Tvertex *posit, Tvector *dir1, Tvector *dir2, StrPtr txt, double depth);
	void rendertext2d(StrPtr fontname, double x, double y, double size, StrPtr txt);
	TDisplayList* CreateDisplayList();
	void ForgetDisplayList(TDisplayList *list);

private:
	Titemarray<Tvertex> lightpoints_worldposit;
	Titemarray<TSC_color> lightpoints_color;
	Tdoublearray lightpoints_size;
public:
	void addlightpoint(Tvertex *worldposit, TSC_color *color, double size);
	void renderlightpoints();

private:
	int clipplaneusedcount;
	Tintarray clipplaneidentifiers;
	const static int maxclipplanecount=6;
public:
	void addclipplane(const Tplane &plane);
	void delclipplane();

public:
	void unproject(double screen_x, double screen_y, double dist, Tvertex &ps);

};

class TDisplayList
{
private:
	Trendercontext *rc;
	GLuint listidx;
public:
	TDisplayList(Trendercontext *irc);
	~TDisplayList();
	void startrecord();
	void endrecord();
	void draw();
};

class Trenderthread
{
private:
	TRenderWindow *win;
	T3DScene *curscene;
	T3DViewport *curviewport;
	HANDLE threadhandle;
	HANDLE renderevent,swapfinishevent,renderfinishevent,swapevent;
	static void runthread(void *obj);
	bool threadstopping;
	QString error;
	bool isrightpart;
private:
	void run();
protected:
	void dorender(bool i_isrightpart);

public:
	Trenderthread(bool i_isrightpart);
	~Trenderthread();
	void start(TRenderWindow *iwin);
	void render(T3DScene *iscene, T3DViewport *iviewport);
	void waitforrenderfinished();
	void waitforswapfinished();
	void swap();
	StrPtr G_error() { return error; }
};


//for 3DConnexion
//[ event_receiver(com)]
class TRenderWindow : public CWnd
{
	friend class Trendercontext;
private:
	bool isrightpart;
	T3DViewport *viewport;
	struct {
		double x0,y0,lx,ly;
	} renderviewport;
	TDisplayDevice *disp;
	DECLARE_MESSAGE_MAP();
	double viewmatrix[16];
	double screenratioX,screenratioY;
	DWORD s_dwLastDraw;
	HCURSOR emptycursor;
	int centerposx,centerposy,mouseshiftx,mouseshifty,mouseshiftz;
	Tarray<T3DFont> fonts;
	bool mouse_leftbuttondown,mouse_rightbuttondown,mouse_middlebuttondown;


public:
	CDC *permdc;
	HGLRC rc;
	Trenderthread renderthread;
	Trendercontext rendercontext;
public:
	TRenderWindow(bool iisrightpart, TDisplayDevice *idisp);
	~TRenderWindow();
	void createwin(RECT &boundingbox);
	void OnActivate();
//	void OnPaint();
	void setup();
	void setup_fonts();
	void setdefaults();
	void Set_isrightwindow(bool status) { isrightpart=status; }
	void GetRC();
	void ReleaseRC();
	void initviewmatrix();
	void render_clear(TSC_color *backcolor);
	void render_begin(T3DViewport *iviewport, double frxmin, double frymin, double frxmax, double frymax, bool i_isrightpart);
	void createfrustum(double nearplane, double farplane);
	void render_end();
	void swapbuffers();
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnRButtonUp(UINT nFlags, CPoint point);
	void OnMButtonDown(UINT nFlags, CPoint point);
	void OnMButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void CenterCursor();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual void OnNcDestroy();

	TDisplayDevice* G_DisplayDevice() { return disp; }

	T3DFont* G_font(StrPtr iname);

	double G_screenratioX() { return screenratioX; }
	double G_screenratioY() { return screenratioY; }

	double G_mouseshiftx() { return mouseshiftx; }
	double G_mouseshifty() { return mouseshifty; }
	double G_mouseshiftz() { return mouseshiftz; }
	bool G_mouse_leftbuttondown() { return mouse_leftbuttondown; }
	bool G_mouse_rightbuttondown() { return mouse_rightbuttondown; }
	bool G_mouse_middlebuttondown() { return mouse_middlebuttondown; }
	void resetmouseshift() { mouseshiftx=0; mouseshifty=0; mouseshiftz=0; }

	void UpdateStencil(int InterleavePattern, bool isrightpart);


//////////////////////////////////////////////////////////////////////////////////////////
// FOR 3DConnexion device
//////////////////////////////////////////////////////////////////////////////////////////
#ifdef _3DConnexion
public:
   HRESULT InitializeCOM();
   HRESULT OnDeviceChange(long reserved );
   HRESULT OnKeyDown(int keyCode );
   HRESULT OnKeyUp(int keyCode );
   HRESULT OnSensorInput(void);
   HRESULT UninitializeCOM();

protected:  // com members
   CComPtr<ISensor> m_pISensor;
   CComPtr<IKeyboard> m_pIKeyboard;
#endif

};


