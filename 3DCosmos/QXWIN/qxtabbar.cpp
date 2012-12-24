#include "stdafx.h"
#include "qstring.h"
//#include "qxtabbar.h"

#include "qxwin.h"


//***********************************************************************
// TQXheadercomp_tab
//***********************************************************************

TQXheadercomp_tab::TQXheadercomp_tab(TQXpanel *iparentpanel)
{
	parentpanel=iparentpanel;
	curtab=0;
	mouseinside=false;
	parent=NULL;
	receiver=NULL;
}

void TQXheadercomp_tab::reset()
{
	tabs.reset();
	curtab=NULL;
}


void TQXheadercomp_tab::addtab(StrPtr name)
{
	tabs.add(new QString(name));
	if (curtab<0)
	{
		curtab=0;
		if (receiver!=NULL)
			receiver->tabchanged(this,-1,curtab);
	}
}

void TQXheadercomp_tab::deltab(int tabnr)
{
	if ((tabnr<0)||(tabnr>=tabs.G_count())) return;
	tabs.del(tabnr);
	if (curtab>=tabs.G_count()) curtab=tabs.G_count()-1;
	parent->Invalidate(0);parent->UpdateWindow();
	if (receiver!=NULL)
		receiver->tabchanged(this,-1,curtab);
}

void TQXheadercomp_tab::Set_tabname(int tabnr, StrPtr iname)
{
	if ((tabnr<0)||(tabnr>=tabs.G_count())) return;
	*tabs[tabnr]=iname;
	parent->Invalidate(0);parent->UpdateWindow();
}



StrPtr TQXheadercomp_tab::G_curtabname()
{
	if ((curtab<0)||(curtab>=tabs.G_count())) return _qstr("");
	return *tabs[curtab];
}

void TQXheadercomp_tab::create(TQXpanelheader *iparent)
{
	parent=iparent;
}



int TQXheadercomp_tab::G_height()
{
	return 23;
}

void TQXheadercomp_tab::draw(CDC *dc, TQXDIBitmap *backdrawer)
{
	int x0=px0+24;
	int y0=py0+3;

	TQXColor backcl=RGB(191,192,192);
	TQXColor textcl=RGB(80,80,80);

	if (mouseinside)
	{
		backcl=G_QXSys().G_color(TQXSys::CL_BUTTONHIGHLIGHT).SatDecr(0.3);
		dc->FillSolidRect(px0,py0,plx,ply,backcl);
	}

	StrPtr name=G_curtabname();
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

	int bpx[2];
	bpx[0]=px0+7;
	bpx[1]=px0+plx-20;
	for (int btnr=0; btnr<=1; btnr++)
	{
		x0=bpx[btnr];
		y0=py0+7;
		for (int i=0; i<6; i++) dc->FillSolidRect(x0+i,y0+i,12-2*i,1,textcl);
	}
}


void TQXheadercomp_tab::change_tab(int newtabnr)
{
	int oldnr=curtab;
	if ((newtabnr<0)||(newtabnr>=tabs.G_count())) return;
	curtab=newtabnr;
	parent->Invalidate(0);parent->UpdateWindow();
	if (receiver!=NULL)
		receiver->tabchanged(this,oldnr,curtab);
}


void TQXheadercomp_tab::OnCommand(UINT id)
{
	int tabnr=(int)id-1000;
	if ((tabnr>=0)&&(tabnr<tabs.G_count()))
		change_tab(tabnr);
}


void TQXheadercomp_tab::OnLButtonDown(UINT nFlags, int px, int py)
{
	if (mouseinside)
	{
		CMenu menu;
		UINT flags;
		POINT pt;
		menu.CreatePopupMenu();
		for (int i=0; i<tabs.G_count(); i++)
			menu.AppendMenu(MF_STRING,1000+i,*tabs[i]);
		pt.x=px0;if (px-20>pt.x) pt.x=px-20;
		pt.y=py0+ply;
		parent->ClientToScreen(&pt);
		flags=TPM_LEFTALIGN|TPM_LEFTBUTTON;
		parent->currentheadercomp=this;
		menu.TrackPopupMenu(flags,pt.x,pt.y,parent);
		mouseinside=false;ReleaseCapture();
		parent->Invalidate(0);parent->UpdateWindow();
	}
}

void TQXheadercomp_tab::OnLButtonUp(UINT nFlags, int px, int py)
{
/*	if (butpressing)
	{
		butpressing=false;
		parent->Invalidate(0);parent->UpdateWindow();
	}*/
}

void TQXheadercomp_tab::OnMouseMove(UINT nFlags, int px, int py)
{
	bool newmouseinside=((px>=0)&&(px<=plx)&&(py>=0)&&(py<=ply));
	if (newmouseinside==mouseinside) return;
	mouseinside=newmouseinside;
	parent->Invalidate(0);parent->UpdateWindow();
	if (mouseinside)
	{
		if (parent->GetCapture()!=parent) parent->SetCapture();
	}
	else
	{
		if (parent->GetCapture()==parent) ReleaseCapture();
	}
}
