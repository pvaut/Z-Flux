#include "stdafx.h"
#include "qstring.h"
#include "qxscrollbar.h"
#include "qxgrid.h"


TQXgriddata::~TQXgriddata()
{
}


//**************************************************************************
// TQXgridheader
//**************************************************************************

void TQXgridheader::draw(CDC *dc, RECT &updaterect)
{
	int px,lx,ly;
	RECT rc;
	TQXColor cl;

	GetClientRect(&rc);
	ly=rc.bottom;
	QString str;
	dc->SetBkMode(TRANSPARENT);
	dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_WINDOWHEADER));
	px=-G_offsetx();
	cl=G_QXSys().G_color(TQXSys::CL_WINDOWBACK2).IntensDecr(0.05);
	for (int colnr=0; colnr<parent->data->G_colcount(); colnr++)
	{
		lx=parent->data->G_colsize(colnr);
		if ((px<=updaterect.right+1)&&(px+lx>=updaterect.left-1))
		{
			dc->FillSolidRect(px,0,lx,ly,cl);
			parent->data->G_colname(colnr,str);
			cuttext(dc,str,lx-6);
			dc->TextOut(px+3,3,str);
		}
		dc->FillSolidRect(px+lx-2,0,1,ly,cl.IntensIncr(0.3));
		dc->FillSolidRect(px+lx-1,0,1,ly,cl.IntensDecr(0.1));
		px+=lx;
	}
}

//**************************************************************************
// TQXgridlist
//**************************************************************************

void TQXgridlist::draw(CDC *dc, RECT &updaterect)
{
	int px,py,lx,ly;
	QString str;
	TQXColor cl;

	bool isactive=parent->Isactiveview();

	dc->SetBkMode(TRANSPARENT);
	dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_WINDOW));
	py=-G_offsety();
	for (int rownr=0; rownr<parent->data->G_rowcount(); rownr++)
	{
		ly=parent->data->G_rowsize(rownr);
		if ((py<=updaterect.bottom+1)&&(py+ly>=updaterect.top-1))
		{
			px=-G_offsetx();
			cl=G_QXSys().G_color(TQXSys::CL_WINDOWBACK1);
			if (rownr%2==1) cl=G_QXSys().G_color(TQXSys::CL_WINDOWBACK2);
			if (rownr==parent->currownr)
			{
				if (isactive) cl=G_QXSys().G_color(TQXSys::CL_WINDOWHIGHLIGHT);
				else cl=G_QXSys().G_color(TQXSys::CL_WINDOWHIGHLIGHT).SatDecr(0.3)+cl;
			}
			for (int colnr=0; colnr<parent->data->G_colcount(); colnr++)
			{
				lx=parent->data->G_colsize(colnr);
				if ((px<=updaterect.right+1)&&(px+lx>=updaterect.left-1))
				{
					dc->FillSolidRect(px,py,lx,ly,cl);
					if (parent->data->G_celltext(rownr,colnr,str))
					{
						cuttext(dc,str,lx-6);
						dc->TextOut(px+3,py+ly/10,str);
					}
				}
				dc->FillSolidRect(px+lx-2,py,1,ly,cl.IntensIncr(0.3));
				dc->FillSolidRect(px+lx-1,py,1,ly,cl.IntensDecr(0.1));
				px+=lx;
			}
		}
		py+=ly;
	}
}

void TQXgridlist::SizeChanged()
{
}


void TQXgridlist::MouseLButtonDown(UINT nFlags, CPoint point)
{
	int py,ly;
	int newrownr=-1;
	py=-G_offsety();
	parent->makeactiveview();
	for (int rownr=0; (rownr<parent->data->G_rowcount())&&(newrownr<0); rownr++)
	{
		ly=parent->data->G_rowsize(rownr);
		if ((py<=point.y)&&(py+ly>=point.y)) newrownr=rownr;
		py+=ly;
	}
	if (newrownr>=0)
	{
		parent->cursor_changerow(newrownr);
	}
}


//*****************************************************************************************
// TQXgrid
//*****************************************************************************************



TQXgrid::TQXgrid(StrPtr iname, TQXgriddata *idata) : TQXview(iname) , header(this), list(this)
{
	currownr=0;
	curcolnr=0;
	data=idata;
	drawbuffered=false;
}

TQXgrid::~TQXgrid()
{
	delete data;
}

void TQXgrid::PostCreate()
{
	header.createwindow(this);
	list.createwindow(this);
}

void TQXgrid::attachscrollbars(TQXheadercomp_scrollbar *vert, TQXheadercomp_scrollbar *horz)
{
	vert->addview(&list,DIR_VERT);
	horz->addview(&list,DIR_HOR);
	horz->addview(&header,DIR_HOR);
	updatescrollbars();
}

void TQXgrid::updatescrollbars()
{
	if (list.G_scrollbar_vert()!=NULL)
	{
		int py=0;
		for (int rownr=0; rownr<data->G_rowcount(); rownr++) py+=data->G_rowsize(rownr);
		list.G_scrollbar_vert()->Set_totsize(py+20);
	}
	if (list.G_scrollbar_hor()!=NULL)
	{
		int px=0;
		for (int colnr=0; colnr<data->G_colcount(); colnr++) px+=data->G_colsize(colnr);
		list.G_scrollbar_hor()->Set_totsize(px+20);
	}
}

void TQXgrid::SizeChanged()
{
	RECT rc;
	GetClientRect(&rc);
	rc.bottom=rc.top+24;
	if (IsWindow(header.m_hWnd)) header.MoveWindow(&rc);
	GetClientRect(&rc);
	rc.top=rc.top+26;
	if (IsWindow(list.m_hWnd)) list.MoveWindow(&rc);
}


void TQXgrid::draw(CDC *dc, RECT &updaterect)
{
	dc->FillSolidRect(&updaterect,G_QXSys().G_color(TQXSys::CL_WINDOWBACK2).IntensDecr(0.3));
}

void TQXgrid::cursor_changerow(int newrownr)
{
	if ((newrownr<0)||(newrownr>=data->G_rowcount())) return;
	currownr=newrownr;
	list.Invalidate(0);
}

void TQXgrid::redraw()
{
	list.redraw();
	header.redraw();
}
