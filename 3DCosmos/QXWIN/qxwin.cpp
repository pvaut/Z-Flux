#include "stdafx.h"
#include "qxwin.h"
#include "qxpanelheader.h"
#include "qxbuttonbar.h"
#include "qxscrollbar.h"
#include "qxtabbar.h"

#include "qerror.h"
#include "qstring.h"
#include "qfile.h"
#include "qbintagfile.h"
#include "qtranslate.h"

bool IsKeyDown(int virtkey)
{
	return ((GetKeyState(virtkey)>>1)!=0);
}


double Sshape(double x, double p, double w)
{
	double f=(x-p)/w;
	if (f<-1) return 0;
	if (f>1) return 1;
	return (1.0+(3.0*f-f*f*f)/2.0)/2.0;
}

PanelSideDef enumsides(int nr)
{
	if (nr==0) return SIDE_TOP;
	if (nr==1) return SIDE_BOTTOM;
	if (nr==2) return SIDE_LEFT;
	if (nr==3) return SIDE_RIGHT;
	ASSERT(false);return SIDE_TOP;
}


TQXdrawer::~TQXdrawer()
{
}


void cuttext(CDC *dc, QString &str, int len)
{
	int max;
	CSize sz;
	if (len<=0)
	{
		str.clear();
		return;
	}
	if (dc->GetTextExtent(str).cx<=len) return;
	int dotlen=dc->GetTextExtent(QString("..."),3).cx;
	::GetTextExtentExPoint(dc->m_hDC,str,qstrlen(str),len-dotlen,&max,NULL,&sz);
	str.substring(0,max-1);
	str+=_qstr("...");
}



//*******************************************************************************
// TQXSys
//*******************************************************************************


TQXSys SQsys;

TQXSys& G_QXSys()
{
	return SQsys;
}

TQXSys::TQXSys()
{
	double dx,dy,rd,pw,v1,v2;

	colscheme_dark=true;

	TQXbumpmap butbump;
	butbump.init(buttonsizex,buttonsizey);
	pw=2;
	for (int ix=0; ix<buttonsizex; ix++)
		for (int iy=0; iy<buttonsizey; iy++)
		{
			dx=fabs((ix-buttonsizex/2.0)/(buttonsizex/2.0));
			dy=fabs((iy-buttonsizey/2.0)/(buttonsizey/2.0));
			rd=pow(pow(dx,pw)+pow(dy,pw),1.0/pw);
			v1=20*fabs(rd-0.8);
			v1=0.75/(1+v1*v1*v1);
			v2=5*(0.795-rd);if (v2<0) v2=0;
			v2=4.0*(1-1/(1+v2*v2));
			butbump.Set(ix,iy,1*(v1+v2));
		}
	butbump.render(buttoncorrmap);

	buttoncolmap.init(buttonsizex,buttonsizey);
	for (int ix=0; ix<buttonsizex; ix++)
		for (int iy=0; iy<buttonsizey; iy++)
		{
			dx=fabs((ix-buttonsizex/2.0)/(buttonsizex/2.0));
			dy=fabs((iy-buttonsizey/2.0)/(buttonsizey/2.0));
			rd=pow(pow(dx,pw)+pow(dy,pw),1.0/pw);
			buttoncolmap.Set(ix,iy,(signed char)(127*(1-Sshape(rd,0.65,0.2))));
		}

	//create scroll button corrmap
	TQXbumpmap scrollbutbump;
	scrollbutbump.init(scrollbarsize,scrollbarsize);
	scrollcolmap.init(scrollbarsize,scrollbarsize);
	pw=2;
	for (int ix=0; ix<scrollbarsize; ix++)
		for (int iy=0; iy<scrollbarsize; iy++)
		{
			dx=fabs((ix-scrollbarsize/2.0)/(scrollbarsize/2.0));
			dy=fabs((iy-scrollbarsize/2.0)/(scrollbarsize/2.0));
			rd=pow(pow(dx,pw)+pow(dy,pw),1.0/pw);
			v2=5*(0.84-rd);if (v2<0) v2=0;
			v2=(1-1/(1+v2*v2));
			scrollbutbump.Set(ix,iy,3*(v2));
			scrollcolmap.Set(ix,iy,(signed char)(0.4+127*v2));
		}
	scrollbutbump.render(scrollbutmap);

	scrollslidermap1.init(scrollbarsize,scrollbarsize/2);
	scrollslidermap2.init(scrollbarsize,scrollbarsize/2);
	scrollslidermap3.init(scrollbarsize/2,scrollbarsize);
	scrollslidermap4.init(scrollbarsize/2,scrollbarsize);
	for (int ix=0; ix<scrollbarsize; ix++)
		for (int iy=0; iy<scrollbarsize; iy++)
		{
			dx=fabs((ix-scrollbarsize/2.0)/(scrollbarsize/2.0));
			dy=fabs((iy-scrollbarsize/2.0)/(scrollbarsize/2.0));
			rd=pow(pow(dx,pw)+pow(dy,pw),1.0/pw);
			signed char svl=(signed char)(127*(0.4-Sshape(rd,0.65,0.05)+0.6*Sshape(rd,0.5,0.05)));
			scrollslidermap1.Set(ix,iy,svl);
			scrollslidermap2.Set(ix,iy-scrollbarsize/2,svl);
			scrollslidermap3.Set(ix,iy,svl);
			scrollslidermap4.Set(ix-scrollbarsize/2,iy,svl);
		}
}

void TQXSys::writesettings()
{
	QParamStore(PARAMLOCATION_REGISTRY,_qstr("DarkBackGround"),colscheme_dark);
	QParamStore(PARAMLOCATION_REGISTRY,_qstr("Language"),QTranslate::Get().GetLanguage());
}

void TQXSys::readsettings()
{
	addlog(_text("Reading settings"),1);
	QParamRead(PARAMLOCATION_REGISTRY,_qstr("DarkBackGround"),colscheme_dark);
	QString lang;
	QParamRead(PARAMLOCATION_REGISTRY,_qstr("Language"),lang);
	QTranslate::Get().SetLanguage(lang);
	addlog(_text("Reading settings completed"),-1);
}

void TQXSys::startup()
{
	readsettings();

	addlog(_text("Initialising UI decorations"),+1);

	bmp_roundedcorner.init(IDB_ROUNDCORNER);

	bmp_scrollup.init(IDB_SCROLLUP);
	bmp_scrolldown.init(IDB_SCROLLDOWN);
	bmp_scrolleft.init(IDB_SCROLLEFT);
	bmp_scrollright.init(IDB_SCROLLRIGHT);

	QString fontname("Lucida");
	QString fontnamefixed("Lucida console");

	font_title.CreateFont(-13,0,0,0,FW_BOLD,0,0,0,
	   DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,
	   FF_DONTCARE|DEFAULT_PITCH,fontname);

	int nfontsize=-12;
	font_window.CreateFont(nfontsize,0,0,0,FW_NORMAL,0,0,0,
	   DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,
	   FF_DONTCARE|DEFAULT_PITCH,fontname);
	font_window_bold.CreateFont(nfontsize,0,0,0,FW_BOLD,0,0,0,
	   DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,
	   FF_DONTCARE|DEFAULT_PITCH,fontname);
	font_window_italic.CreateFont(nfontsize,0,0,0,FW_NORMAL,1,0,0,
	   DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,
	   FF_DONTCARE|DEFAULT_PITCH,fontname);

	font_windowheader.CreateFont(nfontsize,0,0,0,FW_BOLD,0,0,0,
	   DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,
	   FF_DONTCARE|DEFAULT_PITCH,fontname);

	font_fixed.CreateFont(-13,0,0,0,FW_NORMAL,0,0,0,
	   DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,
	   FF_DONTCARE|DEFAULT_PITCH,fontnamefixed);

	addlog(_text("UI decorations initialised"),-1);

}


TQXColor TQXSys::G_color(ColorDef cl)
{

	if (colscheme_dark)
	{
		if (cl==CL_FRAME1) return TQXColor(0.6f,0.6f,0.6f);
		if (cl==CL_FRAME2) return TQXColor(0.4f,0.4f,0.4f);
		if (cl==CL_BUTTONHIGHLIGHT) return TQXColor(0.7f,0.9f,0.4f);
		if (cl==CL_WINDOWBACK1) return TQXColor(0.2f,0.2f,0.2f);
		if (cl==CL_WINDOWBACK2) return TQXColor(0.23f,0.23f,0.23f);
		if (cl==CL_WINDOWHIGHLIGHT) return TQXColor(0.2f,0.5f,0.2f);
		if (cl==CL_TEXT) return TQXColor(0.7f,0.7f,0.7f);
	}



	if (!colscheme_dark)
	{
		if (cl==CL_FRAME1) return TQXColor(0.82f,0.82f,0.82f);
		if (cl==CL_FRAME2) return TQXColor(0.63f,0.63f,0.63f);
		if (cl==CL_BUTTONHIGHLIGHT) return TQXColor(1.0f,0.6f,0.3f);
		if (cl==CL_WINDOWBACK1) return TQXColor(0.9685f,0.9685f,0.965f);
		if (cl==CL_WINDOWBACK2) return TQXColor(0.935f,0.935f,0.90f);
		if (cl==CL_WINDOWHIGHLIGHT) return TQXColor(1.0f,0.7f,0.3f);
		if (cl==CL_TEXT) return TQXColor(0.0f,0.0f,0.0f);
	}

	return TQXColor(1,0,0);
}

CFont& TQXSys::G_font(FontDef fnt)
{
	if (fnt==FNT_TITLEBAR) return font_title;
	if (fnt==FNT_WINDOW) return font_window;
	if (fnt==FNT_WINDOWBOLD) return font_window_bold;
	if (fnt==FNT_WINDOWITALIC) return font_window_italic;
	if (fnt==FNT_WINDOWHEADER) return font_windowheader;
	if (fnt==FNT_FIXED) return font_fixed;

	return font_window;
}




//*******************************************************************************
// TQXpanel
//*******************************************************************************

BEGIN_MESSAGE_MAP(TQXpanel,CWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_PAINT()
  ON_WM_NCHITTEST()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
/*  ON_WM_CLOSE()
  ON_WM_DESTROY()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
  ON_WM_MOUSEWHEEL()
  ON_WM_ACTIVATE()
  ON_WM_NCPAINT()
  ON_WM_NCLBUTTONDOWN()
  ON_WM_INITMENU()*/
END_MESSAGE_MAP()


TQXpanel::TQXpanel(TQXwin *iparentwindow, TQXpanel *iparentpanel, TQXwinpaneldef &paneldef)
{
	parentwindow=iparentwindow;
	parentpanel=iparentpanel;
	subtype=paneldef.subtype;
	sizefrac=paneldef.sizefrac;
	for (int i=0; i<paneldef.subpanels.G_count(); i++)
	{
		TQXpanel *subpanel=new TQXpanel(parentwindow,this,*paneldef.subpanels[i]);
		childpanels.add(subpanel);
	}
	name=paneldef.name;
	tabbar=NULL;
	hascustomtabs=paneldef.hascustomtabs;



	for (int i=0; i<4; i++) sidebars[i]=new TQXpanelheader(this,enumsides(i));

	//create title
	if (paneldef.hastitlebar)
	{
		TQXheadercomp_title *title=new TQXheadercomp_title(this);
		parentwindow->headercomponents.add(title);
		sidebars[0]->addcomponent(title);
	}

	//create tabs
	if ((paneldef.subtype==TQXwinpaneldef::SUB_TAB)||(hascustomtabs))
	{
		TQXheadercomp_tab *tab=new TQXheadercomp_tab(this);
		parentwindow->headercomponents.add(tab);
		sidebars[0]->addcomponent(tab);
		for (int i=0; i<childpanels.G_count(); i++)
			tab->addtab(childpanels[i]->G_name());
		tab->Set_receiver(this);
		tabbar=tab;
	}

	for (int i=0; i<4; i++)
	{
		for (int j=0; j<paneldef.headercomponents.G_count(); j++)
			if (sidebars[i]->G_side()==paneldef.headercomponents[j]->side)
			{
				TQXheadercomp *cmp=parentwindow->G_headercomponent(paneldef.headercomponents[j]->name);
				ASSERT(cmp!=NULL);
				if (cmp!=NULL) sidebars[i]->addcomponent(cmp);
			}
	}

	myview=parentwindow->G_view(G_name());


	moving_separator=-1;
	minsizex=60;
	minsizey=60;
}

TQXpanel::~TQXpanel()
{
	for (int i=0; i<4; i++) if (sidebars[i]!=NULL) delete sidebars[i];
}

void TQXpanel::createwindow(CWnd *pwnd)
{
	RECT rc;

	rc.left=60;rc.top=80;rc.right=160;rc.bottom=160;
	Create(
		    AfxRegisterWndClass
		    (CS_DBLCLKS,NULL,
			(HBRUSH)GetStockObject(NULL_BRUSH)
			),
		        QString(""),
		        WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,
				rc,pwnd,0xA);

	for (int i=0; i<4; i++)
		sidebars[i]->createwindow(this);

	for (int i=0; i<childpanels.G_count(); i++)
		childpanels[i]->createwindow(this);

	if (myview!=NULL)
	{
		myview->createwindow(this);
		myview->Set_framewindow(parentwindow);
	}
}

int TQXpanel::G_clientoffset(PanelSideDef side)
{
	if (side==SIDE_TOP)
	{
		if (sidebars[0]!=NULL) return sidebars[0]->G_size();
		return 0;
	}
	if (side==SIDE_BOTTOM)
	{
		if (sidebars[1]!=NULL) return sidebars[1]->G_size();
		return 0;
	}
	if (side==SIDE_LEFT)
	{
		if (sidebars[2]!=NULL) return sidebars[2]->G_size();
//		if (parentpanel==NULL) return TQXSys::sepsize;
		return 0;
	}
	if (side==SIDE_RIGHT)
	{
		if (sidebars[3]!=NULL) return sidebars[3]->G_size();
//		if (parentpanel==NULL) return TQXSys::sepsize;
		return 0;
	}
	return 0;
}

void TQXpanel::G_minsize(int &x, int &y)
{
	int xs,ys;
	x=minsizex;
	y=minsizey;
	int xtot=0;
	int ytot=0;
	for (int i=0; i<childpanels.G_count(); i++)
	{
		childpanels[i]->G_minsize(xs,ys);
		if (xs>x) x=xs;
		if (ys>y) y=ys;
		if (subtype==TQXwinpaneldef::SUB_HOR) xtot+=xs;
		if (subtype==TQXwinpaneldef::SUB_VERT) ytot+=ys;
	}
	if (xtot>x) x=xtot;
	if (ytot>y) y=ytot;
}

int TQXpanel::G_actchildnr()
{
	if (tabbar==NULL) return 0;
	return tabbar->G_curtab();
}




void TQXpanel::OnPaint() 
{	
	RECT rc;
	GetClientRect(&rc);
	CPaintDC ddc(this);
	if (sidebars[0]!=NULL)
	{
//		rc.top+=sidebars[0]->G_size();
//		headers[0]->draw(&ddc,0,0,rc.right-rc.left,headers[0]->G_size(),false);
	}
/*	rc.top+=G_clientoffset(SIDE_TOP);
	rc.bottom-=G_clientoffset(SIDE_BOTTOM);
	rc.left+=G_clientoffset(SIDE_LEFT);
	rc.right-=G_clientoffset(SIDE_RIGHT);*/
	COLORREF cl=G_QXSys().G_color(TQXSys::CL_FRAME2);
	ddc.FillSolidRect(rc.left,rc.top,rc.right-rc.left+1,rc.bottom-rc.top+1,cl);

	//paint unused area
//	COLORREF cl=G_QXSys().G_color(TQXSys::CL_FRAME2);

	if (childpanels.G_count()==0)
	{
		cl=RGB(230,230,230);
		rc.top+=G_clientoffset(SIDE_TOP);
		rc.bottom-=G_clientoffset(SIDE_BOTTOM);
		rc.left+=G_clientoffset(SIDE_LEFT);
		rc.right-=G_clientoffset(SIDE_RIGHT);
		ddc.FillSolidRect(rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,cl);
	}

}


void TQXpanel::sizefrac2sepposits(int ps1, int ps2)
{
	int i;
	double fr=0,totfrac=0;
	for (i=0; i<childpanels.G_count(); i++) totfrac+=childpanels[i]->sizefrac;
	if (totfrac<=0) totfrac=1;
	sepposits.reset();
	for (i=0; i<=childpanels.G_count(); i++)
	{
		sepposits.add((int)(0.5+ps1+(ps2-ps1)*fr));
		if (i<childpanels.G_count()) fr+=childpanels[i]->sizefrac/totfrac;
	}
}

void TQXpanel::sepposits2sizefrac(int ps1, int ps2)
{
	int i;
	if (ps2<=ps1) ps2=ps1+1;
	for (i=0; i<childpanels.G_count(); i++)
	{
		childpanels[i]->sizefrac=(sepposits[i+1]-sepposits[i])*1.0/(ps2-ps1);
	}
}


void TQXpanel::tabchanged(TQXheadercomp_tab *tabcontrol, int prenr, int postnr)
{
	if (subtype==TQXwinpaneldef::SUB_TAB) OnSize(1,0,0);
}



void TQXpanel::OnSize(UINT tpe, int x, int y)
{ 
	int i;
	RECT rc,rc2;
	if (IsWindow(m_hWnd))
	{
		GetClientRect(&rc);
		if ((sidebars[0]!=NULL)&&(IsWindow(sidebars[0]->m_hWnd)))
		{
			rc2=rc;rc2.bottom=sidebars[0]->G_size();
			rc2.left+=G_clientoffset(SIDE_LEFT);rc2.right-=G_clientoffset(SIDE_RIGHT);
			sidebars[0]->SetPosition(rc2);
		}
		if ((sidebars[1]!=NULL)&&(IsWindow(sidebars[1]->m_hWnd)))
		{
			rc2=rc;rc2.top=rc.bottom-sidebars[1]->G_size();
			rc2.left+=G_clientoffset(SIDE_LEFT);rc2.right-=G_clientoffset(SIDE_RIGHT);
			sidebars[1]->SetPosition(rc2);
		}
		if ((sidebars[2]!=NULL)&&(IsWindow(sidebars[2]->m_hWnd)))
		{
			rc2=rc;rc2.right=sidebars[2]->G_size();
			rc2.top+=G_clientoffset(SIDE_TOP);rc2.bottom-=G_clientoffset(SIDE_BOTTOM);
			sidebars[2]->SetPosition(rc2);
		}
		if ((sidebars[3]!=NULL)&&(IsWindow(sidebars[3]->m_hWnd)))
		{
			rc2=rc;rc2.left=rc.right-sidebars[3]->G_size();
			rc2.top+=G_clientoffset(SIDE_TOP);rc2.bottom-=G_clientoffset(SIDE_BOTTOM);
			sidebars[3]->SetPosition(rc2);
		}

		rc.left+=G_clientoffset(SIDE_LEFT);
		rc.right-=G_clientoffset(SIDE_RIGHT);
		rc.top+=G_clientoffset(SIDE_TOP);
		rc.bottom-=G_clientoffset(SIDE_BOTTOM);
		if ((myview!=NULL)&&(IsWindow(myview->m_hWnd)))
			myview->MoveWindow(&rc);
		rc2=rc;
		if ((subtype==TQXwinpaneldef::SUB_HOR)&&(childpanels.G_count()>0))
		{
			sizefrac2sepposits(rc.left,rc.right);
			for (i=0; i<childpanels.G_count(); i++)
			{
				rc2.left=sepposits[i];
				if (i>0) rc2.left+=TQXSys::sepsize/2;
				rc2.right=sepposits[i+1];
				if (i<childpanels.G_count()-1) rc2.right-=TQXSys::sepsize/2;
				if (IsWindow(childpanels[i]->m_hWnd)) childpanels[i]->SetPosition(rc2);
			}
		}
		if ((subtype==TQXwinpaneldef::SUB_VERT)&&(childpanels.G_count()>0))
		{
			sizefrac2sepposits(rc.top,rc.bottom);
			for (i=0; i<childpanels.G_count(); i++)
			{
				rc2.top=sepposits[i];
				if (i>0) rc2.top+=TQXSys::sepsize/2;
				rc2.bottom=sepposits[i+1];
				if (i<childpanels.G_count()-1) rc2.bottom-=TQXSys::sepsize/2;
				if (IsWindow(childpanels[i]->m_hWnd)) childpanels[i]->SetPosition(rc2);
			}
		}
		if ((subtype==TQXwinpaneldef::SUB_TAB)&&(childpanels.G_count()>0))
		{
			int actchild=G_actchildnr();
			for (int i=0; i<childpanels.G_count(); i++)
				if ((i!=actchild)&&(IsWindow(childpanels[i]->m_hWnd))) childpanels[i]->ShowWindow(SW_HIDE);
			if ((childpanels[actchild]!=NULL)&&(IsWindow(childpanels[actchild]->m_hWnd)))
			{
				childpanels[actchild]->ShowWindow(SW_SHOW);
				childpanels[actchild]->SetPosition(rc);
			}
		}
	}
}

int TQXpanel::G_onseparator(int px, int py)//returns the separator# underneath a position (-1 if none)
{
	if ((subtype==TQXwinpaneldef::SUB_HOR)&&(childpanels.G_count()>0))
	{
		for (int i=1; i<sepposits.G_count()-1; i++)
			if (abs(sepposits[i]-px)<=TQXSys::sepsize/2) return i;
		return -1;
	}
	if ((subtype==TQXwinpaneldef::SUB_VERT)&&(childpanels.G_count()>0))
	{
		for (int i=1; i<sepposits.G_count()-1; i++)
			if (abs(sepposits[i]-py)<=TQXSys::sepsize/2) return i;
		return -1;
	}
	return -1;
}

void TQXpanel::invalidateall()
{
	int i;
	if (IsWindow(m_hWnd))
	{
		for (i=0; i<childpanels.G_count(); i++) childpanels[i]->invalidateall();
		for (i=0; i<4; i++) if (sidebars[i]!=NULL) sidebars[i]->Invalidate(0);
		Invalidate(0);
	}
}

void TQXpanel::updatewindowall()
{
	int i;
	if (IsWindow(m_hWnd))
	{
		for (i=0; i<childpanels.G_count(); i++) childpanels[i]->updatewindowall();
		for (i=0; i<4; i++) if (sidebars[i]!=NULL) sidebars[i]->UpdateWindow();
		UpdateWindow();
	}
}

TQXpanel* TQXpanel::G_panel_byname(StrPtr name)
{
	if (qstricmp(name,G_name())==0) return this;
	TQXpanel *rs=NULL;
	for (int i=0; i<childpanels.G_count(); i++)
	{
		rs=childpanels[i]->G_panel_byname(name);
		if (rs!=NULL) return rs;
	}
	return NULL;
}



LRESULT TQXpanel::OnNcHitTest(CPoint point)
{
	RECT rc;
	GetWindowRect(&rc);
	if (G_onseparator(point.x-rc.left,point.y-rc.top)>=0) return CWnd::OnNcHitTest(point);
	return HTTRANSPARENT;
//	if ((parentpanel==NULL)&&(point.y>=rc.top)&&(point.y<=rc.top+TQXSys::titlebarh)) return HTTRANSPARENT;
//	if ((parentpanel==NULL)&&(point.y<=rc.bottom)&&(point.y>=rc.bottom-TQXSys::statusbar)) return HTTRANSPARENT;
	return CWnd::OnNcHitTest(point);
}


void TQXpanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	moving_separator=G_onseparator(point.x,point.y);
	if (moving_separator>=0)
	{
		SetCapture();
	}
	moving_lastx=point.x;
	moving_lasty=point.y;
}

void TQXpanel::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (moving_separator>=0)
	{
		moving_separator=-1;
		ReleaseCapture();
	}
}

void TQXpanel::OnMouseMove(UINT nFlags, CPoint point)
{
	RECT rc;
	int minp,mnx,mny;
	if (moving_separator>=0)
	{
		GetClientRect(&rc);
		rc.left+=G_clientoffset(SIDE_LEFT);
		rc.right-=G_clientoffset(SIDE_RIGHT);
		rc.top+=G_clientoffset(SIDE_TOP);
		rc.bottom-=G_clientoffset(SIDE_BOTTOM);

		if (subtype==TQXwinpaneldef::SUB_HOR)
		{
			if (point.x==sepposits[moving_separator]) return;
			if (point.x<sepposits[moving_separator])
			{
				childpanels[moving_separator-1]->G_minsize(mnx,mny);
				minp=sepposits[moving_separator-1]+mnx;
				if (point.x<minp) point.x=minp;
			}
			if (point.x>sepposits[moving_separator])
			{
				childpanels[moving_separator]->G_minsize(mnx,mny);
				minp=sepposits[moving_separator+1]-mnx;
				if (point.x>minp) point.x=minp;
			}
			sepposits[moving_separator]=point.x;
			sepposits2sizefrac(rc.left,rc.right);
			OnSize(1,0,0);
			updatewindowall();
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		}
		if (subtype==TQXwinpaneldef::SUB_VERT)
		{
			if (point.y==sepposits[moving_separator]) return;
			if (point.y<sepposits[moving_separator])
			{
				childpanels[moving_separator-1]->G_minsize(mnx,mny);
				minp=sepposits[moving_separator-1]+mny;
				if (point.y<minp) point.y=minp;
			}
			if (point.y>sepposits[moving_separator])
			{
				childpanels[moving_separator]->G_minsize(mnx,mny);
				minp=sepposits[moving_separator+1]-mny;
				if (point.y>minp) point.y=minp;
			}
			sepposits[moving_separator]=point.y;
			sepposits2sizefrac(rc.top,rc.bottom);
			updatewindowall();
			OnSize(1,0,0);
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
		}
		return;
	}

	bool othercursor=false;
	if (G_onseparator(point.x,point.y)>=0)
	{
		if (subtype==TQXwinpaneldef::SUB_HOR) SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		else SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
		othercursor=true;
	}
	if (!othercursor)
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void TQXpanel::customtabs_init(TQXtabnotifyreceiver *receiver)
{
	ASSERT(tabbar!=NULL);
	if (tabbar==NULL) return;
	hascustomtabs=true;
	tabbar->Set_receiver(receiver);
	tabbar->reset();
}

void TQXpanel::customtabs_add(StrPtr iname)
{
	ASSERT(tabbar!=NULL);
	if (tabbar==NULL) return;
	tabbar->addtab(iname);
}




//*******************************************************************************
// TQXwin
//*******************************************************************************



void TQXwin::createwindow(HWND pwnd, StrPtr iname)
{
	RECT rc;

	rc.left=60;rc.top=60;
	rc.right=1200;
	rc.bottom=900;
	Create(
		    AfxRegisterWndClass
		    (CS_DBLCLKS/*|CS_DROPSHADOW*/,AfxGetApp()->LoadStandardCursor(IDC_ARROW),
			(HBRUSH)GetStockObject(BLACK_BRUSH),
			AfxGetApp()->LoadIcon(1)
			),
		        iname,
//				WS_POPUP|WS_VISIBLE|WS_TABSTOP|WS_CLIPCHILDREN,
		        WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_CLIPCHILDREN,
				rc,NULL,
				NULL
		    );
	PostCreate();
}

BEGIN_MESSAGE_MAP(TQXwin,CFrameWnd)
  ON_COMMAND_RANGE(1,9001,OnCommand)//for general purposes
/*  ON_COMMAND_RANGE(29700,29999,oncmdrange)//for script menu items & buttons
  ON_COMMAND_RANGE(30000,30500,oncmdrange)//for dockpanel mechanisms
  ON_COMMAND_RANGE(30500,30800,oncmdrange)//for plugin buttons*/
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_PAINT()
  ON_WM_MOUSEWHEEL()

  ON_WM_ACTIVATE()
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
  ON_WM_DESTROY()
  ON_WM_CLOSE()

/*  ON_WM_CLOSE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
  ON_WM_MOUSEWHEEL()
  ON_WM_ACTIVATE()
  ON_WM_NCPAINT()
  ON_WM_NCLBUTTONDOWN()
  ON_WM_INITMENU()*/
  ON_WM_NCHITTEST()
END_MESSAGE_MAP()


TQXwin::TQXwin()
{
	m_bAutoMenuEnable=FALSE;
	rootpanel=NULL;
	activeview=NULL;
}

TQXwin::~TQXwin()
{
	if (rootpanel!=NULL) delete rootpanel;
}

BOOL TQXwin::PreCreateWindow( CREATESTRUCT& cs )
{
	if (!CFrameWnd::PreCreateWindow(cs)) return FALSE;
	cs.dwExStyle &= !WS_EX_CLIENTEDGE;
	return TRUE;
}


int TQXwin::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{ 
	RECT rc;
	GetWindowRect(&rc);
	rc.right-=rc.left;rc.bottom-=rc.top;rc.left=0;rc.top=0;

/*	//!!!
	CRgn rgn;
	createcliparea(rc,rgn);
	SetWindowRgn(static_cast<HRGN>(rgn.GetSafeHandle()),false);
	rgn.Detach();
	//end!!!*/

	createviews();

	TQXwindef windef;
	buildwindef(windef);

	rootpanel=new TQXpanel(this,NULL,windef.rootpanel);
	rootpanel->createwindow(this);

	//read settings
	try{
		QString filename;
		GetDocumentsDir(filename);filename+=_qstr("\\");filename+=G_softwarename();filename+=_qstr("\\WinConfig.stt");
		if (IsFilePresent(filename))
		{
			QBinTagFileReader reader;
			QBinTagReaderRef root=reader.open(filename);

			while (root.G_obj().hassubtags())
			{
				QBinTagReaderRef curtag=root.G_obj().getnextsubtag();
				if (issame(curtag.G_obj().G_name(),_qstr("Dim")))
				{
					RECT rc;
					rc.left=reader.read_int32();rc.top=reader.read_int32();
					rc.right=reader.read_int32();rc.bottom=reader.read_int32();
					SetWindowPos(NULL,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,SWP_NOZORDER);
				}
				curtag.G_obj().close();
			}
		}
	}
	catch(QError &err)
	{
		reporterror(err);
	}


	return 0;
}


void TQXwin::createcliparea(RECT &rc, CRgn &region)
{
	region.CreateRectRgn(rc.left,rc.top,rc.right,rc.bottom);

	int sz=11;


	{//top-left
		CRgn reg1,reg2;
		reg1.CreateRectRgn(0,0,sz,sz);
		region.CombineRgn(&region,&reg1,RGN_DIFF);
		reg2.CreateEllipticRgn(0,0,2*sz+1,2*sz+1);
		region.CombineRgn(&region,&reg2,RGN_OR);
	}

	{//top-right
		CRgn reg1,reg2;
		reg1.CreateRectRgn(rc.right-sz,0,rc.right,sz);
		region.CombineRgn(&region,&reg1,RGN_DIFF);
		reg2.CreateEllipticRgn(rc.right-2*sz,0,rc.right+1,2*sz+1);
		region.CombineRgn(&region,&reg2,RGN_OR);
	}

	{//bottom-left
		CRgn reg1,reg2;
		reg1.CreateRectRgn(0,rc.bottom-sz,sz,rc.bottom);
		region.CombineRgn(&region,&reg1,RGN_DIFF);
		reg2.CreateEllipticRgn(0,rc.bottom-2*sz,2*sz+1,rc.bottom+1);
		region.CombineRgn(&region,&reg2,RGN_OR);
	}


	{//bottom-right
		CRgn reg1,reg2;
		reg1.CreateRectRgn(rc.right-sz,rc.bottom-sz,rc.right,rc.bottom);
		region.CombineRgn(&region,&reg1,RGN_DIFF);
		reg2.CreateEllipticRgn(rc.right-2*sz,rc.bottom-2*sz,rc.right+1,rc.bottom+1);
		region.CombineRgn(&region,&reg2,RGN_OR);
	}

//	region.CreateEllipticRgn(0,0,400,400);
/*	int sizex=rc->right-rc->left;
	int sizey=rc->bottom-rc->top;
	region.CreateRectRgn(0,50,sizex,sizey-bottomh-1);
	CRgn reg1,reg2;
	reg1.Attach(BitmapToRegion(logobitmap1,RGB(255,0,255),RGB(0,0,0)));
	region.CombineRgn(&region,&reg1,RGN_OR);
	reg1.Detach();
	reg2.Attach(BitmapToRegion(logobitmap2,RGB(255,0,255),RGB(0,0,0)));
	reg2.OffsetRgn(0,rc->bottom-rc->top-bottomh-1);
	region.CombineRgn(&region,&reg2,RGN_OR);
	reg2.Detach();*/
}

void TQXwin::OnSize(UINT tpe, int x, int y)
{ 
	RECT rc;
	GetWindowRect(&rc);
	rc.right-=rc.left;rc.bottom-=rc.top;rc.left=0;rc.top=0;
	CRgn rgn;
	createcliparea(rc,rgn);
//	SetWindowRgn(static_cast<HRGN>(rgn.GetSafeHandle()),true);
	rgn.Detach();
	Invalidate(0);

	GetClientRect(&rc);
//	rc.top+=80;rc.bottom-=80;
	rootpanel->SetPosition(rc);

}



void TQXwin::OnPaint() 
{	
	RECT rc;
	GetClientRect(&rc);
	CPaintDC ddc(this);

	ddc.FillSolidRect(rc.left,0,rc.right-rc.left+1,rc.bottom+1,RGB(255,0,0));
}

LRESULT TQXwin::OnNcHitTest(CPoint point)
{
	RECT rc;
	GetWindowRect(&rc);
//	if (point.y<rc.top+50) return HTCAPTION;
	if ((point.x>rc.right-15)&&(point.y>rc.bottom-15)) return HTBOTTOMRIGHT;
	return CWnd::OnNcHitTest(point);
}

BOOL TQXwin::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	POINT pt2;
	int lx,ly;
	for (int i=0; i<views.G_count(); i++)
	{
		pt2=pt;views[i]->ScreenToClient(&pt2);
		lx=views[i]->G_clientsizex();ly=views[i]->G_clientsizey();
		if ((views[i]->IsWindowVisible())&&(pt2.x>=0)&&(pt2.y>=0)&&(pt2.x<=lx)&&(pt2.y<=ly))
			if (views[i]->MouseWheel(nFlags,zDelta,pt)) return TRUE;
	}
	return FALSE;
}

void TQXwin::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	KeyPressed(nChar,nRepCnt,nFlags);
}

void TQXwin::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags )
{
	CharPressed(nChar,nRepCnt,nFlags);
}

void TQXwin::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (activeview!=NULL) activeview->SetFocus();
}

void TQXwin::invalidateall()
{
	Invalidate();
	rootpanel->invalidateall();

	for (int i=0; i<views.G_count(); i++)
		views[i]->Invalidate(0);
}

void TQXwin::writesettings()
{
	RECT rc;
	GetWindowRect(&rc);

	QString filename;
	GetDocumentsDir(filename);filename+=_qstr("\\");filename+=G_softwarename();filename+=_qstr("\\WinConfig.stt");

	QBinTagFileWriter writer;
	QBinTagWriterRef root=writer.open(filename);


	{
		QBinTagWriterRef dimref=root.G_obj().subtag(_qstr("Dim"));
		writer.write_int32(rc.left);writer.write_int32(rc.top);
		writer.write_int32(rc.right);writer.write_int32(rc.bottom);
		dimref.G_obj().close();
	}

	{
		QBinTagWriterRef ref2=root.G_obj().subtag(_qstr("Test"));
		writer.write_int32(1234);
		ref2.G_obj().close();
	}

}



void TQXwin::addheadercomponent(TQXheadercomp *icomp)
{
	headercomponents.add(icomp);
}

TQXheadercomp* TQXwin::G_headercomponent(StrPtr name)
{
	for (int i=0; i<headercomponents.G_count(); i++)
		if (qstricmp(name,headercomponents[i]->G_name())==0) return headercomponents[i];
	return NULL;
}

void TQXwin::addview(TQXview *iview)
{
	views.add(iview);
}

TQXview* TQXwin::G_view(StrPtr name)
{
	for (int i=0; i<views.G_count(); i++)
		if (qstricmp(name,views[i]->G_name())==0) return views[i];
	return NULL;
}

void TQXwin::Set_activeview(TQXview *newview)
{
	newview->SetFocus();
	if (newview==activeview) return;
	TQXview *oldactive=activeview;
	activeview=newview;
	if (oldactive!=NULL) oldactive->redraw();
	if (activeview!=NULL) activeview->redraw();
}

TQXpanel* TQXwin::G_panel_byname(StrPtr name)
{
	ASSERT(rootpanel!=NULL);
	return rootpanel->G_panel_byname(name);
}

void TQXwin::OnClose()
{
	if (!DoClose()) return;
	writesettings();
	CFrameWnd::OnClose();
}


//********************************************************************
// TQXview
//********************************************************************

TQXview::TQXview()
{
	init();
}

TQXview::TQXview(StrPtr iname)
{
	name=iname;
	init();
}

void TQXview::init()
{
	framewindow=NULL;
	scrollbar_vert=NULL;
	scrollbar_hor=NULL;
	drawbuffered=true;
	canmakeactive=true;
}

TQXview::~TQXview()
{
}


BEGIN_MESSAGE_MAP(TQXview,TQXbasewin)
  ON_COMMAND_RANGE(1,9001,OnCommand)//for general purposes
/*  ON_COMMAND_RANGE(29700,29999,oncmdrange)//for script menu items & buttons
  ON_COMMAND_RANGE(30000,30500,oncmdrange)//for dockpanel mechanisms
  ON_COMMAND_RANGE(30500,30800,oncmdrange)//for plugin buttons*/
//  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_MOUSEMOVE()
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
/*  ON_WM_CLOSE()
  ON_WM_DESTROY()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEWHEEL()
  ON_WM_ACTIVATE()
  ON_WM_NCPAINT()
  ON_WM_NCLBUTTONDOWN()
  ON_WM_INITMENU()*/
//  ON_WM_NCHITTEST()
END_MESSAGE_MAP()


void TQXview::createwindow(CWnd *pwnd)
{
	RECT rc;
	rc.left=20;rc.top=20;rc.right=160;rc.bottom=160;
	Create(
		    AfxRegisterWndClass
		    (CS_DBLCLKS,NULL,
			(HBRUSH)GetStockObject(NULL_BRUSH)
			),
		        QString(""),
		        WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,
				rc,pwnd,0xA);
	PostCreate();
}

void TQXview::OnPaint()
{
	if (!IsWindow(m_hWnd)) return;
	RECT rc;
	GetClientRect(&rc);
	CPaintDC ddc(this);
	if (drawbuffered)
	{
		CDC *drawdc=drawbuffer.start(&ddc,rc.right,rc.bottom);
		drawdc->FillSolidRect(0,0,rc.right,rc.bottom,G_QXSys().G_color(TQXSys::CL_WINDOWBACK1));
		draw(drawdc,ddc.m_ps.rcPaint);
		drawbuffer.transfer(&ddc,0,0);
		drawbuffer.stop();
	}
	else
	{
		draw(&ddc,ddc.m_ps.rcPaint);
	}
}

void TQXview::OnSize(UINT tpe, int x, int y)
{
	if (IsWindow(m_hWnd))
	{
		RECT rc;
		GetClientRect(&rc);
		if (G_scrollbar_vert()!=NULL) G_scrollbar_vert()->Set_pagesize(rc.bottom);
		if (G_scrollbar_hor()!=NULL) G_scrollbar_hor()->Set_pagesize(rc.right);
		SizeChanged();
	}
}

void TQXview::OnLButtonDown(UINT nFlags, CPoint point)
{
	makeactiveview();
	MouseLButtonDown(nFlags,point);
}

void TQXview::OnRButtonDown(UINT nFlags, CPoint point)
{
	makeactiveview();
	MouseRButtonDown(nFlags,point);
}


void TQXview::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	makeactiveview();
	MouseLButtonDblClk(nFlags,point);
}


void TQXview::OnLButtonUp(UINT nFlags, CPoint point)
{
	MouseLButtonUp(nFlags,point);
}

void TQXview::OnMouseMove(UINT nFlags, CPoint point)
{
	MouseMove(nFlags,point);
}

void TQXview::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	KeyPressed(nChar,nRepCnt,nFlags);
	if (framewindow!=NULL) framewindow->OnKeyDown(nChar,nRepCnt,nFlags);
}

void TQXview::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags )
{
	CharPressed(nChar,nRepCnt,nFlags);
	if (framewindow!=NULL) framewindow->OnChar(nChar,nRepCnt,nFlags);
}


void TQXview::MouseMove(UINT nFlags, CPoint point)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

bool TQXview::MouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (scrollbar_vert!=NULL)
	{
		if (zDelta>0) scrollbar_vert->doscroll(-3*scrollbar_vert->G_jump());
		else scrollbar_vert->doscroll(+3*scrollbar_vert->G_jump());
		return true;
	}
	return false;
}

void TQXview::Set_scrollbar_vert(TQXheadercomp_scrollbar *scroll)
{
	scrollbar_vert=scroll;
}

void TQXview::Set_scrollbar_hor(TQXheadercomp_scrollbar *scroll)
{
	scrollbar_hor=scroll;
}

void TQXview::scrollinview_vert(int y1, int y2)
{
	if (scrollbar_vert==NULL) return;
	int ly=G_clientsizey();
	if (y2>G_offsety()+ly) scrollbar_vert->setscrollpos(y2-ly);
	if (y1<G_offsety())	scrollbar_vert->setscrollpos(y1);
}

void TQXview::scrollinview_horz(int x1, int x2)
{
	if (scrollbar_hor==NULL) return;
	int lx=G_clientsizex();
	if (x2>G_offsetx()+lx) scrollbar_hor->setscrollpos(x2-lx);
	if (x1<G_offsetx())	scrollbar_hor->setscrollpos(x1);
}


void TQXview::redraw(bool immediateupdate)
{
	if (IsWindow(m_hWnd))
	{
		Invalidate(0);
		if (immediateupdate) UpdateWindow();
	}
}

int TQXview::G_clientsizex()
{
	RECT rc;
	GetClientRect(&rc);
	return rc.right;
}

int TQXview::G_clientsizey()
{
	RECT rc;
	GetClientRect(&rc);
	return rc.bottom;
}

int TQXview::G_offsetx()
{
	if (scrollbar_hor==NULL) return 0;
	return scrollbar_hor->G_curpos();
}

int TQXview::G_offsety()
{
	if (scrollbar_vert==NULL) return 0;
	return scrollbar_vert->G_curpos();
}

void TQXview::makeactiveview()
{
	if ((canmakeactive)&&(framewindow!=NULL)) framewindow->Set_activeview(this);
}


bool TQXview::Isactiveview()
{
	if (framewindow==NULL) return false;
	return (framewindow->G_activeview()==this);
}
