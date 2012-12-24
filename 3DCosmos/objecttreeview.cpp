#include "stdafx.h"
#include "qstring.h"
#include "qxscrollbar.h"
#include "qxformattedstring.h"
#include "main.h"
#include "objecttreeview.h"
#include "objectpropertyview.h"
#include "scriptsourceview.h"
#include "formattedstringview.h"




//*****************************************************************************************
// Tobjecttreeview
//*****************************************************************************************



Tobjecttreeview::Tobjecttreeview(StrPtr iname, Tmainwin *imainwin, TObjectTreeItem *iroot) : TQXview(iname)
{
	mainwin=imainwin;
	root=iroot;
	cursor=NULL;
	totsizey=0;
	canmakeactive=false;
	historypos=-1;
}

Tobjecttreeview::~Tobjecttreeview()
{
}

void Tobjecttreeview::PostCreate()
{
}


void Tobjecttreeview::updatescrollbars()
{
	if (G_scrollbar_vert()!=NULL)
	{
		G_scrollbar_vert()->Set_totsize(totsizey);
	}
	if (G_scrollbar_hor()!=NULL)
	{
		G_scrollbar_hor()->Set_totsize(1000);
	}
}

void Tobjecttreeview::SizeChanged()
{
/*	RECT rc;
	GetClientRect(&rc);
	rc.bottom=rc.top+24;
	if (IsWindow(header.m_hWnd)) header.MoveWindow(&rc);
	GetClientRect(&rc);
	rc.top=rc.top+26;
	if (IsWindow(list.m_hWnd)) list.MoveWindow(&rc);*/
}


TObjectTreeItem* Tobjecttreeview::findsub(TObjectTreeItem *item, int px, int py)
{
	if (item->expanded)
	{
		for (int i=0; i<item->G_childcount(); i++)
		{
			TObjectTreeItem *fnd=findsub(item->G_child(i),px,py);
			if (fnd!=NULL) return fnd;
		}
	}
	if ((px>=item->posx-expandbuttonsize)&&(py>=item->posy-7)&&(py<=item->posy-7+item->leny)) return item;
	return NULL;
}


void Tobjecttreeview::MouseLButtonDown(UINT nFlags, CPoint point)
{
	bool controlpressed=((GetKeyState(VK_CONTROL)>>1)!=0);

	TObjectTreeItem *newsel=findsub(root,point.x+G_offsetx(),point.y+G_offsety());
	if (newsel!=cursor)
	{
		Set_cursor(newsel);
	}
	if (newsel!=NULL)
	{
		if ((abs(point.x+G_offsetx()-newsel->posx)<=expandbuttonsize)&&
			(abs(point.y+G_offsety()-newsel->posy)<=expandbuttonsize))
		{
			newsel->expanded=!newsel->expanded;
			redraw();
		}
		if ((controlpressed)&&(mainwin->G_curscript()!=NULL))
		{
			QString str;
			newsel->G_sourcecodename(str);
			if (qstrlen(str)>0)
				mainwin->scriptsourceview->insertstring(str);
		}
	}


}

void Tobjecttreeview::MouseLButtonDblClk(UINT nFlags, CPoint point)
{
	TObjectTreeItem *newsel=findsub(root,point.x+G_offsetx(),point.y+G_offsety());
	if (newsel!=NULL)
	{
		if (newsel->G_childcount()>0)
		{
			newsel->expanded=!newsel->expanded;
			redraw();
		}
	}
}



void Tobjecttreeview::drawsub(CDC *dc, RECT &updaterect, int offsetx, int &offsety, TObjectTreeItem *item, bool selsub)
{
	TQXFormattedString fstr;
	TQXColor backcl=G_QXSys().G_color(TQXSys::CL_WINDOWBACK2);
	TQXColor textcl=G_QXSys().G_color(TQXSys::CL_TEXT);
	TQXColor linecl=2*backcl+textcl;
	QString str;
	item->posx=offsetx+G_offsetx();
	item->posy=offsety+G_offsety();
	item->G_displayname(fstr);
	if (item==cursor)
		selsub=true;
	if (selsub)
	{
		TQXColor cl=G_QXSys().G_color(TQXSys::CL_WINDOWHIGHLIGHT);
		if (item!=cursor) cl=cl+2*G_QXSys().G_color(TQXSys::CL_WINDOWBACK1);
//		if (!Isactiveview()) cl=cl.SatDecr(0.3)+G_QXSys().G_color(TQXSys::CL_WINDOWBACK1);
		dc->FillSolidRect(offsetx+expandbuttonsize+2,offsety-8,2000,sepy,cl);
	}

	dc->SetBkMode(TRANSPARENT);
	dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_WINDOW));
	dc->SetTextColor(textcl);
	fstr.draw(dc,offsetx+expandbuttonsize+5,offsety-7);

	if (item->G_childcount()>0)
	{
		dc->FillSolidRect(offsetx-expandbuttonsize,offsety-expandbuttonsize,2*expandbuttonsize+1,2*expandbuttonsize+1,linecl);
		dc->FillSolidRect(offsetx-expandbuttonsize+1,offsety-expandbuttonsize+1,2*expandbuttonsize-1,2*expandbuttonsize-1,backcl);
		dc->FillSolidRect(offsetx-expandbuttonsize+2,offsety,2*expandbuttonsize-3,1,textcl);
		if (!item->expanded)
			dc->FillSolidRect(offsetx,offsety-expandbuttonsize+2,1,2*expandbuttonsize-3,textcl);
	}

	int offsety0=offsety;
	offsetx+=sepx;
	offsety+=sepy;
	if (item->expanded)
	{
		for (int i=0; i<item->G_childcount(); i++)
		{
			dc->FillSolidRect(offsetx-sepx,offsety,sepx,1,linecl);
			dc->FillSolidRect(offsetx-sepx,offsety0+expandbuttonsize+1,1,offsety-offsety0-expandbuttonsize-1,linecl);
			drawsub(dc,updaterect,offsetx,offsety,item->G_child(i),selsub);
		}
	}
	item->leny=offsety+G_offsety()-item->posy;


}



void Tobjecttreeview::draw(CDC *dc, RECT &updaterect)
{
	int psx,psy;
	psx=20-G_offsetx();
	psy=20-G_offsety();
	drawsub(dc,updaterect,psx,psy,root,false);
	psy+=G_offsety();
	if (psy!=totsizey)
	{
		totsizey=psy;
		updatescrollbars();
	}
}




void Tobjecttreeview::redraw()
{
	Invalidate(0);
}

void Tobjecttreeview::Set_cursor(TObjectTreeItem *it, bool addtohistory)
{
	TObjectTreeItem *oldcursor=cursor;
	cursor=it;
	try{
		cursorchanged();
		if (cursor!=NULL)
		{
			TObjectTreeItem *pr=cursor->G_parent();
			while (pr!=NULL) { pr->expanded=true;pr=pr->G_parent(); }
		}
	}
	catch (QError &err)
	{
		cursor=oldcursor;
		reporterror(mainwin,err);
	}
	redraw();
	UpdateWindow();
	if (cursor!=NULL) scrollinview_vert(cursor->posy-5,cursor->posy+15);

	if (addtohistory)
	{
		while ((history.G_count()>0)&&(history.G_count()-1>historypos)) history.del(history.G_count()-1);
		history.add(cursor);historypos=history.G_count()-1;
	}
}

void Tobjecttreeview::cursorchanged()
{
	mainwin->propertyview->setcurrent(cursor);
	QString latexstr;
	if (cursor!=NULL) cursor->G_description(latexstr);
	mainwin->funcdescrview->G_scrollbar_vert()->setscrollpos(0);
	try{
		mainwin->funcdescrview->Set_formattedstring(latexstr);
	}
	catch(QError &err)
	{
		reporterror(mainwin,err);
	}
	mainwin->funcdescrview->redraw();
}


void Tobjecttreeview::cmd_cursorback()
{
	if ((historypos-1<0)||(historypos-1>=history.G_count())) return;
	historypos--;
	Set_cursor(history[historypos],false);
}

void Tobjecttreeview::cmd_cursorforward()
{
	if (historypos+1>=history.G_count()) return;
	historypos++;
	Set_cursor(history[historypos],false);
}
