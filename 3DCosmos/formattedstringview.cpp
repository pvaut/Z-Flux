#include "stdafx.h"
#include "qstring.h"
#include "qxscrollbar.h"
#include "formattedstringview.h"
#include "main.h"

#include "SC_functree.h"

#include "objecttreeview.h"

////////////////////////////////////////////////////////////////
// TformattedstringviewEnv
////////////////////////////////////////////////////////////////

TformattedstringviewEnv::TformattedstringviewEnv() : TxtEnv(12)
{
	addcustomkeyword(_qstr("\\linkitem"));
	addcustomkeyword(_qstr("\\softwarename"));
}

void TformattedstringviewEnv::processcustomkeyword(StrPtr keyword, StrPtr in, QString &out)
{
	if (issame(keyword,_qstr("\\linkitem")))
	{
		TFuncTreeItem *item=GetTSCenv().G_functreeitem(in);
		if (item==NULL)
		{
			out=_qstr("!!!Unresolved link ");
			out+=in;
			return;
		}
		QString name;
		item->G_name(name);
		out=TFormatString(_qstr("\\hyperlink{^1}{^2}"),in,name);
	}
	if (issame(keyword,_qstr("\\softwarename")))
		out="\\textbf{Z--Flux}";
}


//*****************************************************************************************
// Tformattedstringview
//*****************************************************************************************



Tformattedstringview::Tformattedstringview(StrPtr iname, Tmainwin *imainwin) : TQXview(iname)
{
	mainwin=imainwin;
	offsety=0;
}

Tformattedstringview::~Tformattedstringview()
{
}

void Tformattedstringview::Set_formattedstring(StrPtr ifstr)
{
	doc.parsefrom(ifstr);
}


void Tformattedstringview::PostCreate()
{
/*	RECT rc;
	rc.left=20;rc.top=40;rc.right=200;rc.bottom=65;
	CEdit *ed=new CEdit();
	ed->CreateEx(WS_EX_CLIENTEDGE,_qstr("EDIT"), _qstr(""),WS_CHILD | WS_TABSTOP | WS_VISIBLE,rc,this,1000);*/
}


void Tformattedstringview::updatescrollbars()
{
	if (G_scrollbar_vert()!=NULL)
	{
		G_scrollbar_vert()->Set_totsize(1000);
	}
	if (G_scrollbar_hor()!=NULL)
	{
		G_scrollbar_hor()->Set_totsize(700);
	}
}




void Tformattedstringview::SizeChanged()
{
	redraw();
}




void Tformattedstringview::draw(CDC *dc, RECT &updaterect)
{
	int sizex=G_clientsizex();
	dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_WINDOWHEADER));
	dc->SetBkMode(TRANSPARENT);
	dc->SetTextColor(G_QXSys().G_color(TQXSys::CL_TEXT));

	offsety=G_scrollbar_vert()->G_curpos();

	TxtRendererGDI renderer(dc);
	doc.calc(renderer,sizex-10);
	doc.render(5,5-offsety,renderer);
//	fstring.drawmultiline(dc,10,10,sizex-20);

	hyperlinklist.clear();
	doc.buildhyperlinklist(hyperlinklist,5,5,renderer);

	int sizey=(int)(doc.G_sizey());
	G_scrollbar_vert()->Set_totsize(sizey);

}

void Tformattedstringview::MouseLButtonDown(UINT nFlags, CPoint point)
{
	StrPtr hyperID=hyperlinklist.G_linkat(point.x,point.y+offsety);
	if (qstrlen(hyperID)>0)
	{
		TFuncTreeItem *item=GetTSCenv().G_functreeitem(hyperID);
		if (item!=NULL)
		{
			mainwin->functreeview->Set_cursor(item);
		}
	}
}

void Tformattedstringview::MouseMove(UINT nFlags, CPoint point)
{
	StrPtr hyperID=hyperlinklist.G_linkat(point.x,point.y+offsety);
	if (qstrlen(hyperID)>0)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
	}
	else SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}



void Tformattedstringview::redraw()
{
	Invalidate(0);
}

