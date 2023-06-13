#include "stdafx.h"

#include "displaydevices.h"


#include "3DScene.h"
#include "3DCosmos.h"

TDisplayWindow* uniquedisplaywindow = nullptr;


int G_rendertype();

Tcopyarray<TDisplayWindow> displaywindows;
Tcopyarray<TDisplayWindow>* G_displaywindows() { return &displaywindows; }

static TDisplayAdapterlist* sset = nullptr;

TDisplayAdapterlist& TDisplayAdapterlist::Get()
{
	if(sset == nullptr)
		sset = new TDisplayAdapterlist();
	return *sset;
}

TDisplayAdapterlist::TDisplayAdapterlist()
{
	if (sset == nullptr)
		sset = this;
	param_readonly(_qstr("Name"), true);
}

TObjectTreeItem* G_treeobject_adapterlist()
{
	return &TDisplayAdapterlist::Get();
}

TDisplayWindow* G_uniquedisplaywindow()
{
	return uniquedisplaywindow;
	/*
	ASSERT(displaywindows.G_count()==1);
	if (displaywindows.G_count()<1) throw (_text("Query for unique display window failed because there are none"));
	if (displaywindows.G_count()>1) throw (_text("Query for unique display window failed because there are more than one"));
	return displaywindows[0];
	*/
}


////////////////////////////////////////////////////////////////////////
// TDisplayWindow
////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(TDisplayWindow,CFrameWnd)
  ON_WM_ACTIVATE()
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
  ON_WM_LBUTTONDOWN()
  ON_WM_CREATE()
  ON_WM_NCDESTROY()
  ON_WM_CLOSE()

//  ON_COMMAND_RANGE(1,9001,OnCommand)//for general purposes
/*  
  ON_WM_SIZE()
  ON_WM_PAINT()
  ON_WM_MOUSEWHEEL()*/
/*  ON_WM_CLOSE()
  ON_WM_DESTROY()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
  ON_WM_MOUSEWHEEL()
  ON_WM_NCPAINT()
  ON_WM_NCLBUTTONDOWN()
  ON_WM_INITMENU()*/
//  ON_WM_NCHITTEST()
END_MESSAGE_MAP()

TDisplayWindow::TDisplayWindow()
{
	m_bAutoMenuEnable=FALSE;
	device=NULL;
	renderwindow=NULL;
}

BOOL TDisplayWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	CFrameWnd::PreCreateWindow(cs);
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return TRUE;
}


TDisplayWindow::~TDisplayWindow()
{
	addlog(_text("Destroying display window"),+1);
	if (renderwindow!=NULL)
	{
		delete renderwindow;
		renderwindow=NULL;
	}
	for (int i=0; i<displaywindows.G_count(); i++)
		if (displaywindows[i]==this)
		{
			displaywindows.del(i);
			i=9999;
		}
	if (device!=NULL) device->win=NULL;
	addlog(_text("Display window destroyed"),-1);
}


void TDisplayWindow::createwin(TDisplayDevice *idevice, RECT &boundingbox)
{
	addlog(_text("Creating display window"),+1);
	if (idevice==NULL) throw QError(_text("Unable to create display window: no device provided"));
	device=idevice;

	QString ddescr;
	device->G_description(ddescr);
	addlog(TFormatString(_text("On device: ^1"),ddescr));
	addlog(TFormatString(_text("TopLeft: ^1,^2"),boundingbox.left,boundingbox.top));
	addlog(TFormatString(_text("BottomRight: ^1,^2"),boundingbox.right,boundingbox.bottom));

	Create(
		    AfxRegisterWndClass(0,NULL,(HBRUSH)GetStockObject(BLACK_BRUSH),NULL),
		        QString("Display window"),
				WS_POPUP|WS_VISIBLE|WS_CLIPCHILDREN,
				boundingbox,NULL,
				NULL,0
		    );
	if(uniquedisplaywindow == nullptr)
		uniquedisplaywindow = this;
	displaywindows.add(this);

/*	m_hWnd=CreateWindowEx(
			WS_EX_APPWINDOW,
		    AfxRegisterWndClass(0,NULL,(HBRUSH)GetStockObject(GRAY_BRUSH),NULL),
		    QString("Display window"),
			WS_POPUP|WS_VISIBLE|WS_CLIPCHILDREN,
			boundingbox.left,boundingbox.top,boundingbox.right-boundingbox.left,boundingbox.bottom-boundingbox.top,
			NULL,
			NULL,
			NULL,
			NULL
		    );*/

	SetFocus();
	addlog(_text("Display window created"),-1);

}


int TDisplayWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	return 0;
}

void TDisplayWindow::OnNcDestroy()
{
   __super::OnNcDestroy();
}

bool IsAnimationRunning();


void TDisplayWindow::OnClose()
{
	addlog(_text(""));
	addlog(_text(""));
	addlog(_text("************** REQUEST CLOSE DISPLAY WINDOW *********************"),+1);
	//>>>
//	_exit(-1);

	if (IsAnimationRunning())
	{
		addlog(_text("Stopping animation"),+1);
		int ctr=0;
		for (bool stopped=false; (!stopped)&&(ctr<50);)
		{
			G_3DCosmos().Set_requeststop();
			Sleep(100);ctr++;
			if (!IsAnimationRunning()) stopped=true;
		}
		if (!IsAnimationRunning()) addlog(_text("Animation successfully stopped"));
		else addlog(_text("ERRO: unable to stop Animation"));
		addlog(_text("Completed stopping animation"));
	}
	addlog(_text("Request processed"),-1);

	addlog(_text("Closing main window"));
	AfxGetMainWnd()->DestroyWindow();
}



void TDisplayWindow::initrenderwindow()
{
	RECT boundingbox;
	if (renderwindow==NULL)
	{
		QString ddescr;
		if (device==NULL) throw QError(_text("Unable to init render window: no device defined"));
		device->G_description(ddescr);
		addlog(TFormatString(_text("Init Render window on device ^1"),ddescr),+1);
		GetClientRect(&boundingbox);
		renderwindow=new TRenderWindow(false,device);//--!!--warning: this can never be called from a thread!!! (cannot create windows in a thread)
		boundingbox.left+=1;
		boundingbox.right-=1;
		boundingbox.top+=1;
		boundingbox.bottom-=1;
		renderwindow->createwin(boundingbox);
		renderwindow->setup();
		addlog(TFormatString(_text("Render window initialised")),-1);
	}
}

void TDisplayWindow::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (renderwindow!=NULL)
	{
		renderwindow->OnActivate();
		renderwindow->SetFocus();
	}
}


void TDisplayWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	int i=0;
}


void TDisplayWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	G_3DCosmos().UI_OnKeyDown(nChar,nRepCnt,nFlags);
}

void TDisplayWindow::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	G_3DCosmos().UI_OnChar(nChar,nRepCnt,nFlags);
}



////////////////////////////////////////////////////////////////////////////
// TDisplayDevice
////////////////////////////////////////////////////////////////////////////

TDisplayDevice::TDisplayDevice()
{
	win=NULL;
	param_readonly(_qstr("Name"),true);
	addparam(_qstr("Description"),SC_valname_string,true);
	addparam(_qstr("ID"),SC_valname_string,true);
	addparam(_qstr("Xoffset"),SC_valname_scalar,true);
	addparam(_qstr("Yoffset"),SC_valname_scalar,true);
	addparam(_qstr("Xres"),SC_valname_scalar,true);
	addparam(_qstr("Yres"),SC_valname_scalar,true);
	addparam(_qstr("BitDepth"),SC_valname_scalar,true);
	addparam(_qstr("Fullscreen"),SC_valname_boolean);
	addparam(_qstr("InterleavePattern"),SC_valname_scalar,true);

	G_param(_qstr("InterleavePattern"))->content.copyfrom(0);

}

TDisplayDevice::~TDisplayDevice()
{
	if (win!=NULL) delete win;
}

void TDisplayDevice::G_displayrect(RECT &rc)
{
	rc.left=G_paramint(_qstr("Xoffset"));
	rc.top=G_paramint(_qstr("Yoffset"));
	rc.right=rc.left+G_paramint(_qstr("XRes"));
	rc.bottom=rc.top+G_paramint(_qstr("YRes"));
}

void TDisplayDevice::G_description(QString &str)
{
	str=_qstr("{");
	str+=TFormatString(_text("^1,^2;"),G_paramstring(_qstr("Description")),G_paramstring(_qstr("ID")));
	str+=TFormatString(_text("^1,^2,"),G_paramint(_qstr("Xoffset")),G_paramint(_qstr("Yoffset")));
	str+=TFormatString(_text("^1,^2"),G_paramint(_qstr("XRes")),G_paramint(_qstr("YRes")));
	str+=_qstr("}");
}



void TDisplayDevice::load()
{
	QString name;
	int nr=0;
	G_name(name);
	addlog(TFormatString(_text("Loading display device ^1"),name),1);
	DISPLAY_DEVICE devinfo;
	devinfo.cb=sizeof(devinfo);
	while (EnumDisplayDevices(name,nr,&devinfo,0))
	{
		TDisplayMonitor *mon=new TDisplayMonitor;
		monitors.add(mon);
		mon->G_param(_qstr("Name"))->content.fromstring(devinfo.DeviceName);
		mon->G_param(_qstr("Description"))->content.fromstring(devinfo.DeviceString);
		mon->G_param(_qstr("ID"))->content.fromstring(devinfo.DeviceID);
		addlog(TFormatString(_text("   Found monitor ^1,^2,^3"),devinfo.DeviceName,devinfo.DeviceString,devinfo.DeviceID));
		nr++;
	}

	DEVMODE devmode;
	devmode.dmSize=sizeof(devmode);
	if (EnumDisplaySettings(name,ENUM_CURRENT_SETTINGS,&devmode))
	{
		G_param(_qstr("Xoffset"))->content.copyfrom((int)devmode.dmPosition.x);
		G_param(_qstr("Yoffset"))->content.copyfrom((int)devmode.dmPosition.y);
		G_param(_qstr("Xres"))->content.copyfrom((int)devmode.dmPelsWidth);
		G_param(_qstr("Yres"))->content.copyfrom((int)devmode.dmPelsHeight);
		G_param(_qstr("BitDepth"))->content.copyfrom((int)devmode.dmBitsPerPel);
	}
	addlog(TFormatString(_text("Display device loaded")),-1);
}

void TDisplayDevice::start()
{
	QString name;
	G_name(name);
	addlog(TFormatString(_text("Start display device ^1"),name),+1);
	if (win!=NULL)
	{
		addlog(_text("Destroying existing device"),+1);
		win->DestroyWindow();
		win=NULL;
		addlog(_text("Existing device destroyed"),-1);
	}

	fullscreen=G_param(_qstr("Fullscreen"))->content.G_content_boolean()->G_val();
	if (fullscreen)
	{
		addlog(_text("Initialising fullscreen"),+1);
		DEVMODE devmode;
		devmode.dmSize=sizeof(devmode);
		devmode.dmPelsWidth=G_param(_qstr("Xres"))->content.G_content_scalar()->G_intval();
		devmode.dmPelsHeight=G_param(_qstr("Yres"))->content.G_content_scalar()->G_intval();
		devmode.dmBitsPerPel=G_param(_qstr("BitDepth"))->content.G_content_scalar()->G_intval();
		devmode.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		LONG rs=ChangeDisplaySettings(&devmode,CDS_FULLSCREEN);
		if (rs!=DISP_CHANGE_SUCCESSFUL)
		{
			QString cmt=_qstr("Unknown reason");
			if (rs==DISP_CHANGE_BADDUALVIEW) cmt=_text("The settings change was unsuccessful because system is DualView capable.");
			if (rs==DISP_CHANGE_BADFLAGS) cmt=_text("An invalid set of flags was passed in.");
			if (rs==DISP_CHANGE_BADMODE) cmt=_text("The graphics mode is not supported.");
			if (rs==DISP_CHANGE_BADPARAM) cmt=_text("An invalid parameter was passed in. This can include an invalid flag or combination of flags.");
			if (rs==DISP_CHANGE_FAILED) cmt=_text("The display driver failed the specified graphics mode.");
			if (rs==DISP_CHANGE_NOTUPDATED) cmt=_text("Unable to write settings to the registry.");
			if (rs==DISP_CHANGE_RESTART) cmt=_text("The computer must be restarted in order for the graphics mode to work");
			QString err=_text("Unable to change display settings: ");err+=cmt;
			throw QError(err);
		}
		addlog(_text("Fullscreen initialised"),-1);
	}

	RECT boundingbox;
	G_displayrect(boundingbox);
	win=new TDisplayWindow;
	win->createwin(this,boundingbox);
	addlog(TFormatString(_text("Display device started")),-1);
}


////////////////////////////////////////////////////////////////////////////
// TDisplayAdapterlist
////////////////////////////////////////////////////////////////////////////


void TDisplayAdapterlist::load()
{
	addlog(_text("Loading display adaptor information"),+1);
	int nr=0;
	devices.reset();
	DISPLAY_DEVICE devinfo;
	devinfo.cb=sizeof(devinfo);
	while (EnumDisplayDevices(NULL,nr,&devinfo,0))
	{
		TDisplayDevice *dev=new TDisplayDevice;
		devices.add(dev);
		dev->G_param(_qstr("Name"))->content.fromstring(devinfo.DeviceName);
		dev->G_param(_qstr("Description"))->content.fromstring(devinfo.DeviceString);
		dev->G_param(_qstr("ID"))->content.fromstring(devinfo.DeviceID);
		dev->load();
		nr++;
	}
	addlog(_text("Display information loaded"),-1);
}

TDisplayDevice* TDisplayAdapterlist::G_device(int nr)
{
	return devices[nr];
}


TDisplayDevice* TDisplayAdapterlist::G_device(StrPtr name)
{
	QString str;
	for (int i=0; i<devices.G_count(); i++)
	{
		devices[i]->G_name(str);
		if (qstricmp(name,str)==0) return devices[i];
	}
	return NULL;
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_display_start,start)
{
	setmemberfunction(SC_valname_display);
//	addvar(_qstr("type"),SC_valname_string);
//	addvar(_qstr("name"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TDisplayDevice *display=G_valuecontent<TDisplayDevice>(owner);
	display->start();
}
ENDFUNCTION(func_display_start)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_display_getlist,GetDisplayList)
{
	setclasspath_fromtype(SC_valname_display);
	setreturntype(SC_valname_list);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_list *list=G_valuecontent<TSC_list>(retval);
	list->reset();
	for (int i=0; i<TDisplayAdapterlist::Get().G_devicecount(); i++)
	{
		QString name;
		TDisplayAdapterlist::Get().G_device(i)->G_name(name);
		TSC_value vl;
		vl.settype(GetTSCenv().G_datatype(SC_valname_string));
		vl.fromstring(name);
		list->add(&vl);
	}
}
ENDFUNCTION(func_display_getlist)
