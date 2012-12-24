#include "stdafx.h"
#include "qstring.h"
#include "qxbuttonbar.h"

#include "qxwin.h"


//***********************************************************************
// TQXheadercomp_title
//***********************************************************************

TQXheadercomp_title::TQXheadercomp_title(TQXpanel *iparentpanel)
{
	parentpanel=iparentpanel;
}

int TQXheadercomp_title::G_height()
{
	return 23;
}

void TQXheadercomp_title::draw(CDC *dc, TQXDIBitmap *backdrawer)
{
	int x0=px0+12;
	int y0=py0+3;

	TQXColor backcl=RGB(191,192,192);
	TQXColor textcl=RGB(80,80,80);

	StrPtr name=parentpanel->G_name();
	dc->SetBkMode(TRANSPARENT);
	dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_TITLEBAR));
	dc->SetTextColor(backcl.IntensIncr(0.2));
	dc->TextOut(x0+2,y0+1,name,qstrlen(name));
	dc->TextOut(x0+1,y0+2,name,qstrlen(name));
	dc->TextOut(x0+2,y0+2,name,qstrlen(name));

	dc->SetTextColor(backcl.IntensIncr(0.4));
	dc->TextOut(x0+1,y0+1,name,qstrlen(name));
	dc->SetTextColor(textcl);
	dc->TextOut(x0,y0,name,qstrlen(name));
}





//***********************************************************************
// TQXheadercomp_buttonbar
//***********************************************************************

TQXheadercomp_button_subitem* TQXheadercomp_button::addsubitem(int commandID, int bitmapID, StrPtr hint)
{
	TQXbitmapdrawer *drawer=NULL;
	if (bitmapID>0) drawer=new TQXbitmapdrawer(bitmapID);
	TQXheadercomp_button_subitem *sbt=new TQXheadercomp_button_subitem(commandID,hint,drawer);

	subitems.add(sbt);
	return sbt;
}


TQXheadercomp_buttonbar::TQXheadercomp_buttonbar(StrPtr iname, CWnd *icommandreceiver)
{
	name=iname;
	curbutnr=-1;
	butpressing=false;
	commandreceiver=icommandreceiver;
}


void TQXheadercomp_buttonbar::create(TQXpanelheader *iparent)
{
	parent=iparent;
}


int TQXheadercomp_buttonbar::G_height()
{
	return TQXSys::buttonsizey+6;
}


TQXheadercomp_button* TQXheadercomp_buttonbar::addbutton(int commandID, int bitmapID, StrPtr hint)
{
	TQXbitmapdrawer *drawer=new TQXbitmapdrawer(bitmapID);
	TQXheadercomp_button *bt=new TQXheadercomp_button(commandID,hint,drawer);

	buttons.add(bt);
	return bt;
}


void TQXheadercomp_buttonbar::G_buttonoffset(int nr, int &px, int &py)
{
	ASSERT(nr>=0);ASSERT(nr<buttons.G_count());
	px=3+nr*TQXSys::buttonsizex;
	py=3;
}

int TQXheadercomp_buttonbar::G_buttonatpos(int px, int py)
{
	int x0,y0;
	for (int i=0; i<buttons.G_count(); i++)
	{
		G_buttonoffset(i,x0,y0);
		x0+=TQXSys::buttonsizex/2;
		y0+=TQXSys::buttonsizex/2;
		if (sqrt((px-x0)*1.0*(px-x0)+(py-y0)*1.0*(py-y0))<=TQXSys::buttonsizex/2.0) return i;
	}
	return -1;
}



void TQXheadercomp_buttonbar::draw(CDC *dc, TQXDIBitmap *backdrawer)
{
	int px,py;
	corrmap.init(plx,ply);

	for (int i=0; i<buttons.G_count(); i++)
	{
		G_buttonoffset(i,px,py);
		TQXdrawer *drawer=buttons[i]->G_drawer();
		drawer->draw(dc,px0+px+TQXSys::buttonsizex/2-drawer->G_resx()/2,py0+py+TQXSys::buttonsizey/2-drawer->G_resy()/2);
		corrmap.add(G_QXSys().buttoncorrmap,px,py);
	}


	backdrawer->apply(corrmap,px0,py0);
	if (curbutnr>=0)
	{
		G_buttonoffset(curbutnr,px,py);
//		TQXColor cl(1.0f,0.7f,0.2f,0.6f);
		TQXColor cl=G_QXSys().G_color(TQXSys::CL_BUTTONHIGHLIGHT);
		if (!butpressing) cl=cl.transp(0.3);
		else cl=cl.SatIncr(0.5);
		backdrawer->applycolormix(G_QXSys().buttoncolmap,cl,px0+px,py0+py);
	}

//	dc->FillSolidRect(x0,y0,lx,ly,RGB(192,128,128));
}


void TQXheadercomp_buttonbar::tracksubitems()
{
	TQXheadercomp_button *bt=buttons[curbutnr];

	CMenu menu;
	UINT flags;
	POINT pt;
	int px,py;
	menu.CreatePopupMenu();
	for (int i=0; i<bt->subitems.G_count(); i++)
	{
		if (qstricmp(bt->subitems[i]->hint,_qstr("-"))==0)
			menu.AppendMenu(MF_SEPARATOR,0,_qstr(""));
		else
			menu.AppendMenu(MF_STRING,bt->subitems[i]->commandID,bt->subitems[i]->hint);
	}
	G_buttonoffset(curbutnr,px,py);
	pt.x=px0+px;pt.y=py0+ply-4;
	parent->ClientToScreen(&pt);
	flags=TPM_LEFTALIGN|TPM_LEFTBUTTON;
	menu.TrackPopupMenu(flags,pt.x,pt.y,commandreceiver);
}

void TQXheadercomp_buttonbar::OnLButtonDown(UINT nFlags, int px, int py)
{
	if (curbutnr>=0)
	{
		butpressing=true;
		parent->Invalidate(0);parent->UpdateWindow();
		if (buttons[curbutnr]->subitems.G_count()>0)
		{
			tracksubitems();
			butpressing=false;curbutnr=-1;
			parent->Invalidate(0);parent->UpdateWindow();
		}
	}
}

void TQXheadercomp_buttonbar::OnLButtonUp(UINT nFlags, int px, int py)
{
	if (butpressing)
	{
		int cmd=buttons[curbutnr]->commandID;
		butpressing=false;
		int oldcurbutnr=curbutnr;curbutnr=-1;
		parent->Invalidate(0);parent->UpdateWindow();
		commandreceiver->SendMessage(WM_COMMAND,cmd);
		curbutnr=oldcurbutnr;
//		parent->Invalidate(0);parent->UpdateWindow();
	}
}

void TQXheadercomp_buttonbar::OnMouseMove(UINT nFlags, int px, int py)
{
	int btnr=G_buttonatpos(px,py);
	if (btnr!=curbutnr)
	{
		curbutnr=btnr;
		parent->Invalidate(0);parent->UpdateWindow();
		if (btnr>=0)
		{
			if (parent->GetCapture()!=parent) parent->SetCapture();
		}
		else
		{
			if (parent->GetCapture()==parent) ReleaseCapture();
		}
	}
}
