#include "stdafx.h"

#include "qxwin.h"




TQXheadercomp::~TQXheadercomp()
{
}


//*******************************************************************************
// TQXpanelheader
//*******************************************************************************


BEGIN_MESSAGE_MAP(TQXpanelheader,CWnd)
//  ON_WM_CREATE()
  ON_COMMAND_RANGE(1000,1100,oncmdrange)
  ON_WM_SIZE()
  ON_WM_PAINT()
  ON_WM_NCHITTEST()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_TIMER()
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


TQXpanelheader::TQXpanelheader(TQXpanel *iowner, PanelSideDef iside)
{
	owner=iowner;
	side=iside;
	timer_win=NULL;
	currentheadercomp=NULL;
}


void TQXpanelheader::createwindow(CWnd *pwnd)
{
	RECT rc;

	rc.left=60;rc.top=80;rc.right=160;rc.bottom=160;
	Create(
		    AfxRegisterWndClass
		    (0/*CS_DBLCLKS*/,AfxGetApp()->LoadStandardCursor(IDC_ARROW),
			(HBRUSH)GetStockObject(NULL_BRUSH)
			),
		        QString(""),
		        WS_CHILD|WS_VISIBLE,
				rc,pwnd,0xA);
	for (int i=0; i<comps.G_count(); i++)
		comps[i]->create(this);
}

void TQXpanelheader::oncmdrange(UINT id)
{
	if ((currentheadercomp!=NULL)&&(IsWindow(m_hWnd)))
		currentheadercomp->OnCommand(id);
}




void TQXpanelheader::starttimer(TQXheadercomp *comp, int itv)
{
	timer_win=comp;
	timer_ct=0;
	SetTimer(5321,itv,NULL);
	timer_win->timerevent(0);
}

void TQXpanelheader::stoptimer()
{
	KillTimer(5321);
	timer_win=NULL;
}

void TQXpanelheader::OnTimer(UINT ev)
{
	if ((ev==5321)&&(timer_win!=NULL))
	{
		timer_ct++;
		timer_win->timerevent(timer_ct);
	}
}


LRESULT TQXpanelheader::OnNcHitTest(CPoint point)
{
	RECT rc;
	GetWindowRect(&rc);
//	return HTTRANSPARENT;
//	if ((parentpanel==NULL)&&(point.y>=rc.top)&&(point.y<=rc.top+TQXSys::titlebarh)) return HTTRANSPARENT;
//	if ((parentpanel==NULL)&&(point.y<=rc.bottom)&&(point.y>=rc.bottom-TQXSys::statusbar)) return HTTRANSPARENT;
	return CWnd::OnNcHitTest(point);
}


void TQXpanelheader::OnSize(UINT tpe, int x, int y)
{
	Invalidate(0);
	calcposits();
}


void TQXpanelheader::addcomponent(TQXheadercomp *icomp)
{
	comps.add(icomp);
}


int TQXpanelheader::G_size()
{
	int sz=0;
	for (int i=0; i<comps.G_count(); i++)
		sz+=comps[i]->G_height();
	return sz;
}


void TQXpanelheader::calcposits()
{
	RECT rc;
	GetClientRect(&rc);
	int i;
	int lx=rc.right;
	int ly=rc.bottom;
	if (ly<=0) return;

	if ((side==SIDE_TOP)||(side==SIDE_BOTTOM))
	{
		int y0=0;
		for (i=0; i<comps.G_count(); i++)
		{
			comps[i]->px0=0;comps[i]->py0=y0;
			comps[i]->plx=lx;comps[i]->ply=comps[i]->G_height();
			y0+=comps[i]->G_height();
			comps[i]->dir=DIR_HOR;
		}
	}
	else
	{
		int x0=0;
		for (i=0; i<comps.G_count(); i++)
		{
			comps[i]->px0=x0;comps[i]->py0=0;
			comps[i]->plx=comps[i]->G_height();comps[i]->ply=ly;
			x0+=comps[i]->G_height();
			comps[i]->dir=DIR_VERT;
		}
	}
}



void TQXpanelheader::OnPaint()
{
	RECT rc;
	GetClientRect(&rc);
	CPaintDC ddc(this);
	int i;

	int lx=rc.right;
	int ly=rc.bottom;

	if (ly<=0) return;

	drawer2.init(lx,ly,G_QXSys().G_color(TQXSys::CL_FRAME2));

	if (side==SIDE_TOP)
		drawer2.vertgrad(G_QXSys().G_color(TQXSys::CL_FRAME1),G_QXSys().G_color(TQXSys::CL_FRAME2),1);
	if (side==SIDE_BOTTOM)
		drawer2.vertgrad(G_QXSys().G_color(TQXSys::CL_FRAME2),G_QXSys().G_color(TQXSys::CL_FRAME1),0);
	if (side==SIDE_LEFT)
		drawer2.horgrad(G_QXSys().G_color(TQXSys::CL_FRAME1),G_QXSys().G_color(TQXSys::CL_FRAME2),1);
	if (side==SIDE_RIGHT)
		drawer2.horgrad(G_QXSys().G_color(TQXSys::CL_FRAME2),G_QXSys().G_color(TQXSys::CL_FRAME1),0);

	CDC *memdc=membitmap.start(&ddc,lx,ly);
	memdc->FillSolidRect(0,0,lx,ly,RGB(192,192,192));



	for (i=0; i<comps.G_count(); i++)
		comps[i]->draw(memdc,&drawer2);

	drawer1.init(lx,ly,TQXColor(RGB(180,180,180)));
	drawer1.copyfrom(memdc->m_hDC,membitmap.G_bitmaphandle());
	membitmap.stop();

	drawer1.replacecolor(drawer2,192,192,192);

	if (owner->G_parentpanel()!=NULL)
	{
		corner1.init(G_QXSys().bmp_roundedcorner.G_resx(),G_QXSys().bmp_roundedcorner.G_resy(),TQXColor(0,0,0));
		corner1.copyfrom(ddc.m_hDC,G_QXSys().bmp_roundedcorner.G_handle());
		gcorner1.init(G_QXSys().bmp_roundedcorner.G_resx(),G_QXSys().bmp_roundedcorner.G_resy());


		//top left corner
		if ((side==SIDE_TOP)||(side==SIDE_LEFT))
		{
			gcorner1.copyfrom(corner1);
			drawer1.applycolormix(gcorner1,G_QXSys().G_color(TQXSys::CL_FRAME2),0,0);
		}
		//top right corner
		corner1.fliphor();
		if ((side==SIDE_TOP)||(side==SIDE_RIGHT))
		{
			gcorner1.copyfrom(corner1);
			drawer1.applycolormix(gcorner1,G_QXSys().G_color(TQXSys::CL_FRAME2),lx-G_QXSys().bmp_roundedcorner.G_resx(),0);
		}
		//bottom right corner
		corner1.flipvert();
		if ((side==SIDE_BOTTOM)||(side==SIDE_RIGHT))
		{
			gcorner1.copyfrom(corner1);
			drawer1.applycolormix(gcorner1,G_QXSys().G_color(TQXSys::CL_FRAME2),lx-G_QXSys().bmp_roundedcorner.G_resx(),ly-G_QXSys().bmp_roundedcorner.G_resy());
		}
		//bottom left corner
		corner1.fliphor();
		if ((side==SIDE_BOTTOM)||(side==SIDE_LEFT))
		{
			gcorner1.copyfrom(corner1);
			drawer1.applycolormix(gcorner1,G_QXSys().G_color(TQXSys::CL_FRAME2),0,ly-G_QXSys().bmp_roundedcorner.G_resy());
		}
	}

	drawer1.render(&ddc,0,0);
}






void TQXpanelheader::OnLButtonDown(UINT nFlags, CPoint point)
{
	for (int i=0; i<comps.G_count(); i++)
		comps[i]->OnLButtonDown(nFlags,point.x-comps[i]->px0,point.y-comps[i]->py0);
}

void TQXpanelheader::OnLButtonUp(UINT nFlags, CPoint point)
{
	for (int i=0; i<comps.G_count(); i++)
		comps[i]->OnLButtonUp(nFlags,point.x-comps[i]->px0,point.y-comps[i]->py0);
}

void TQXpanelheader::OnMouseMove(UINT nFlags, CPoint point)
{
	for (int i=0; i<comps.G_count(); i++)
		comps[i]->OnMouseMove(nFlags,point.x-comps[i]->px0,point.y-comps[i]->py0);
}
