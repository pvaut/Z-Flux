#include "stdafx.h"
#include "qstring.h"
#include "qxscrollbar.h"

#include "qxwin.h"


//***********************************************************************
// TQXheadercomp_scrollbar
//***********************************************************************

TQXheadercomp_scrollbar::TQXheadercomp_scrollbar(StrPtr iname)
{
	name=iname;
	totsize=2000;
	pagesize=100;
	jump=20;
	curpos=0;

	curpart=-1;
	pressing=false;
	parent=NULL;
}

void TQXheadercomp_scrollbar::addview(TQXview *iview, DirDef idir)
{
	TSXscrollaccociation *ass=new TSXscrollaccociation;
	ass->view=iview;
	ass->dir=idir;
	views.add(ass);
	if (idir==DIR_VERT) iview->Set_scrollbar_vert(this);
	if (idir==DIR_HOR) iview->Set_scrollbar_hor(this);
}


void TQXheadercomp_scrollbar::create(TQXpanelheader *iparent)
{
	parent=iparent;
}


int TQXheadercomp_scrollbar::G_height()
{
	return TQXSys::scrollbarsize;
}

bool insideradius(double px, double py, double px0, double py0, double radius)
{
	return (px-px0)*(px-px0)+(py-py0)*(py-py0)<=radius*radius;
}


int TQXheadercomp_scrollbar::pos2scroll(int ps)
{
	int p0a,p0b;
	double fr;

	if (dir==DIR_VERT)
	{
		p0a=py0+TQXSys::scrollbarsize*4/3;
		p0b=py0+ply-TQXSys::scrollbarsize*4/3;
		if (p0b<=p0a) return 0;
		fr=(ps-p0a)*1.0/(p0b-p0a);
		return (int)(0.5+fr*totsize);
	}
	if (dir==DIR_HOR)
	{
		p0a=px0+TQXSys::scrollbarsize*4/3;
		p0b=px0+plx-TQXSys::scrollbarsize*4/3;
		if (p0b<=p0a) return 0;
		fr=(ps-p0a)*1.0/(p0b-p0a);
		return (int)(0.5+fr*totsize);
	}

	return 0;
}


void TQXheadercomp_scrollbar::scroll2pos(int &p1, int &p2)
{
	int p0a,p0b;
	double fr1,fr2;

	fr1=0;fr2=1;
	if (totsize>0)
	{
		fr1=curpos*1.0/totsize;
		fr2=(curpos+pagesize)*1.0/totsize;
	}
	if (fr1<0) fr1=0;
	if (fr1>1) fr1=1;
	if (fr2<0) fr2=0;
	if (fr2>1) fr2=1;
	if (dir==DIR_VERT)
	{
		p0a=py0+TQXSys::scrollbarsize*4/3;
		p0b=py0+ply-TQXSys::scrollbarsize*4/3;
		p1=(int)(p0a+fr1*(p0b-p0a));
		p2=(int)(p0a+fr2*(p0b-p0a));
	}
	if (dir==DIR_HOR)
	{
		p0a=px0+TQXSys::scrollbarsize*4/3;
		p0b=px0+plx-TQXSys::scrollbarsize*4/3;
		p1=(int)(p0a+fr1*(p0b-p0a));
		p2=(int)(p0a+fr2*(p0b-p0a));
	}
}

int TQXheadercomp_scrollbar::G_partatpos(int px, int py)
{
	int p1,p2;
	scroll2pos(p1,p2);
	p1-=TQXSys::scrollbarsize/2;
	p2+=TQXSys::scrollbarsize/2;
	if (dir==DIR_VERT)
	{
		if ((px<0)||(px>=plx)) return -1;
		if (insideradius(px,py,TQXSys::scrollbarsize/2,TQXSys::scrollbarsize/2,TQXSys::scrollbarsize/2)) return 1;
		if (insideradius(px,py,TQXSys::scrollbarsize/2,ply-TQXSys::scrollbarsize/2,TQXSys::scrollbarsize/2)) return 2;
		if ((py>=p1)&&(py<=p2)) return 0;
		if ((py>=0)&&(py<p1)) return 3;
		if ((py<=ply)&&(py>p2)) return 4;
		return -1;
	}
	if (dir==DIR_HOR)
	{
		if ((py<0)||(py>=ply)) return -1;
		if (insideradius(px,py,TQXSys::scrollbarsize/2,TQXSys::scrollbarsize/2,TQXSys::scrollbarsize/2)) return 1;
		if (insideradius(px,py,plx-TQXSys::scrollbarsize/2,TQXSys::scrollbarsize/2,TQXSys::scrollbarsize/2)) return 2;
		if ((px>=p1)&&(px<=p2)) return 0;
		if ((px>=0)&&(px<p1)) return 3;
		if ((px<=plx)&&(px>p2)) return 4;
		return -1;
	}

	return -1;
}




void TQXheadercomp_scrollbar::draw(CDC *dc, TQXDIBitmap *backdrawer)
{
	int p1,p2;

	TQXColor hcl=G_QXSys().G_color(TQXSys::CL_BUTTONHIGHLIGHT);
	if (!pressing) hcl=hcl.transp(0.3);
	else hcl=hcl.SatIncr(0.3);
	scroll2pos(p1,p2);

	corrmap.init(plx,ply);
	if (dir==DIR_VERT)
	{
		backdrawer->apply(G_QXSys().scrollbutmap,px0,py0);
		if (curpart==1) backdrawer->applycolormix(G_QXSys().scrollcolmap,hcl,px0,py0);
		backdrawer->apply(G_QXSys().scrollbutmap,px0,py0+ply-TQXSys::scrollbarsize);
		if (curpart==2) backdrawer->applycolormix(G_QXSys().scrollcolmap,hcl,px0,py0+ply-TQXSys::scrollbarsize);
		TQXColor cl=G_QXSys().G_color(TQXSys::CL_FRAME2).IntensChange(-0.9).transp(0.65);
		if (curpart==0)
		{
			cl=hcl.IntensDecr(0.2);
			if (pressing) cl=hcl;
		}
		if ((curpart==3)||(curpart==4)) cl=cl+hcl.IntensDecr(0.3);
		corrmap.add(G_QXSys().scrollslidermap1,0,p1-TQXSys::scrollbarsize/2);
		corrmap.clonehorline(0,TQXSys::scrollbarsize,p1-1,p2-1);
		corrmap.add(G_QXSys().scrollslidermap2,0,p2);
		backdrawer->applycolormix(corrmap,cl,px0,py0);
		G_QXSys().bmp_scrollup.drawcent(dc,px0+TQXSys::scrollbarsize/2,py0+TQXSys::scrollbarsize/2);
		G_QXSys().bmp_scrolldown.drawcent(dc,px0+TQXSys::scrollbarsize/2,py0+ply-TQXSys::scrollbarsize/2);
	}
	if (dir==DIR_HOR)
	{
		backdrawer->apply(G_QXSys().scrollbutmap,px0,py0);
		if (curpart==1) backdrawer->applycolormix(G_QXSys().scrollcolmap,hcl,px0,py0);
		backdrawer->apply(G_QXSys().scrollbutmap,px0+plx-TQXSys::scrollbarsize,py0);
		if (curpart==2) backdrawer->applycolormix(G_QXSys().scrollcolmap,hcl,px0+plx-TQXSys::scrollbarsize,py0);
		TQXColor cl=G_QXSys().G_color(TQXSys::CL_FRAME2).IntensChange(-0.9).transp(0.65);
		if (curpart==0)
		{
			cl=hcl.IntensDecr(0.2);
			if (pressing) cl=hcl;
		}
		if ((curpart==3)||(curpart==4)) cl=cl+hcl.IntensDecr(0.3);
		corrmap.add(G_QXSys().scrollslidermap3,p1-TQXSys::scrollbarsize/2,0);
		corrmap.clonevertline(0,TQXSys::scrollbarsize,p1-1,p2-1);
		corrmap.add(G_QXSys().scrollslidermap4,p2,0);
		backdrawer->applycolormix(corrmap,cl,px0,py0);
		G_QXSys().bmp_scrolleft.drawcent(dc,px0+TQXSys::scrollbarsize/2,py0+TQXSys::scrollbarsize/2);
		G_QXSys().bmp_scrollright.drawcent(dc,px0+plx-TQXSys::scrollbarsize/2,py0+TQXSys::scrollbarsize/2);
	}
}


void TQXheadercomp_scrollbar::doscroll(int dff)
{
	if ((curpos+pagesize>=totsize)&&(dff>0)) return;
	int newpos=curpos+dff;
	if (newpos<0) newpos=0;
	setscrollpos(newpos);
}

void TQXheadercomp_scrollbar::setscrollpos(int ps)
{
	if (curpos==ps) return;
	int oldpos=curpos;
	curpos=ps;
	if (curpos>=totsize-pagesize) curpos=totsize-pagesize;
	if (curpos<0) curpos=0;
	parent->Invalidate(0);parent->UpdateWindow();
	for (int i=0; i<views.G_count(); i++)
	{
		if (views[i]->dir==DIR_HOR)
		{
			views[i]->view->UpdateWindow();
			views[i]->view->ScrollWindow(oldpos-curpos,0,NULL,NULL);
			views[i]->view->UpdateWindow();
		}
		else
		{
			views[i]->view->UpdateWindow();
			views[i]->view->ScrollWindow(0,oldpos-curpos,NULL,NULL);
			views[i]->view->UpdateWindow();
		}
	}
}

void TQXheadercomp_scrollbar::Set_totsize(int itotsize)
{
	totsize=itotsize;
	if ((parent!=NULL)&&(IsWindow(parent->m_hWnd)))
	{
		parent->Invalidate(0);parent->UpdateWindow();
	}
}

void TQXheadercomp_scrollbar::Set_pagesize(int ipagesize)
{
	pagesize=ipagesize;
	if ((parent!=NULL)&&(IsWindow(parent->m_hWnd)))
	{
		parent->Invalidate(0);parent->UpdateWindow();
	}
}


void TQXheadercomp_scrollbar::OnLButtonDown(UINT nFlags, int px, int py)
{
	if (curpart>=0)
	{
		pressing=true;
		parent->Invalidate(0);parent->UpdateWindow();
		if (curpart==0)
		{
			int p1,p2;
			scroll2pos(p1,p2);
			if (dir==DIR_VERT) ps0dff=p1-py;
			if (dir==DIR_HOR) ps0dff=p1-px;
		}
		parent->starttimer(this,50);
	}
}

void TQXheadercomp_scrollbar::OnLButtonUp(UINT nFlags, int px, int py)
{
	if (pressing)
	{
		pressing=false;
		parent->Invalidate(0);parent->UpdateWindow();
		parent->stoptimer();
	}
}

void TQXheadercomp_scrollbar::OnMouseMove(UINT nFlags, int px, int py)
{
	int prt=G_partatpos(px,py);
	if ((prt!=curpart)&&((!pressing)||(curpart==1)||(curpart==2)))
	{
		curpart=prt;
		parent->Invalidate(0);parent->UpdateWindow();
		if (curpart>=0)
		{
			if (parent->GetCapture()!=parent) parent->SetCapture();
		}
		else
		{
			if (parent->GetCapture()==parent) ReleaseCapture();
		}
	}
	if ((curpart==0)&&(pressing))
	{
		if (dir==DIR_VERT) setscrollpos(pos2scroll(py+ps0dff));
		if (dir==DIR_HOR) setscrollpos(pos2scroll(px+ps0dff));
	}
}

void TQXheadercomp_scrollbar::timerevent(int ct)
{
	if ((pressing)&&((ct==0)||(ct>4)))
	{
		if (curpart==1) doscroll(-jump);
		if (curpart==2) doscroll(+jump);
		if (curpart==3) doscroll(-pagesize);
		if (curpart==4) doscroll(+pagesize);
	}
}
