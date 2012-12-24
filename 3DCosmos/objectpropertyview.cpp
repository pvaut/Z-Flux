#include "stdafx.h"
#include "qstring.h"
#include "qxscrollbar.h"
#include "objectpropertyview.h"

#include "3Dscene.h"
#include "3DCosmos.h"
#include "objecttreeview.h"
#include "main.h"

class Tpropertyedit : public CEdit
{
private:
	Tobjectpropertyview *parent;
	DECLARE_MESSAGE_MAP();
	TQXColor backcolor;
	CBrush backbrush;
public:
	Tpropertyedit(Tobjectpropertyview *iparent)
	{
		parent=iparent;
		backcolor=RGB(192,192,192);
		backbrush.CreateSolidBrush(backcolor);
	}
	~Tpropertyedit()
	{
	}
	void OnKillFocus(CWnd* pNewWnd)
	{
		CWnd::OnKillFocus(pNewWnd);
	}
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		CEdit::OnKeyDown(nChar,nRepCnt,nFlags);
	}
	HBRUSH CtlColor(CDC* pDC, UINT nCtlColor)
	{
		pDC->SetBkColor(backcolor);
		pDC->SetTextColor(RGB(0,0,0));
		return backbrush;
	}
};

BEGIN_MESSAGE_MAP(Tpropertyedit,CEdit)
  ON_WM_KILLFOCUS()
  ON_WM_KEYDOWN()
  ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



class Tpropertycombo : public CComboBox
{
private:
	Tobjectpropertyview *parent;
	DECLARE_MESSAGE_MAP();
	TQXColor backcolor;
	CBrush backbrush;
public:
	Tpropertycombo(Tobjectpropertyview *iparent)
	{
		parent=iparent;
		backcolor=G_QXSys().G_color(TQXSys::CL_WINDOWBACK2);
		backbrush.CreateSolidBrush(backcolor);
	}
	~Tpropertycombo()
	{
	}
	void OnKillFocus(CWnd* pNewWnd)
	{
		CWnd::OnKillFocus(pNewWnd);
	}
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		CComboBox::OnKeyDown(nChar,nRepCnt,nFlags);
	}
	HBRUSH CtlColor(CDC* pDC, UINT nCtlColor)
	{
		pDC->SetBkColor(backcolor);
		return backbrush;
	}
};


BEGIN_MESSAGE_MAP(Tpropertycombo,CComboBox)
  ON_WM_KILLFOCUS()
  ON_WM_KEYDOWN()
  ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



Tpropertyviewer::Tpropertyviewer(Tobjectpropertyview *iparent, TSC_value *ivalue)
{
	ypos=0;
	parent=iparent;
	value.copyfrom(ivalue);
	edt=NULL;
	combo=NULL;
	has_optionlist=false;
	readonly=false;
}



void Tpropertyviewer::set_optionlist(Tarray<QString> &ioptionlist)
{
	has_optionlist=true;
	optionlist.reset();
	for (int i=0; i<ioptionlist.G_count(); i++)
		optionlist.add(new QString(*ioptionlist[i]));
}


int Tpropertyviewer::init(int ix0, int iy0)
{
	ypos=iy0;
	int ylen=30;
	QString str;
	value.tostring(str,999);
	RECT rc;
	rc.left=ix0;rc.top=iy0;rc.right=ix0+200;rc.bottom=iy0+22;
	bool created=false;

	if (has_optionlist)
	{
		rc.bottom=rc.top+200;
		combo=new Tpropertycombo(parent);
		combo->Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|WS_TABSTOP|WS_GROUP,rc,parent,1000);
		combo->SetFont(&G_QXSys().G_font(TQXSys::FNT_WINDOW));
		for (int i=0; i<optionlist.G_count(); i++) combo->AddString(*optionlist[i]);
		int nr=combo->FindStringExact(-1,str);
		if (nr>=0) combo->SetCurSel(nr);
		created=true;
	}
	if (value.istype(SC_valname_affinetransformation))
	{
		rc.right=rc.left+200;
		rc.bottom=rc.top+80;ylen=90;
		edt=new Tpropertyedit(parent);
		edt->CreateEx(WS_EX_CLIENTEDGE,_qstr("EDIT"), _qstr(""),WS_CHILD | WS_TABSTOP | WS_VISIBLE | ES_AUTOHSCROLL | ES_MULTILINE,rc,parent,1000);
		edt->SetFont(&G_QXSys().G_font(TQXSys::FNT_WINDOW));
		edt->SetWindowText(str);
		created=true;
	}
	if (!created)
	{
		edt=new Tpropertyedit(parent);
		edt->CreateEx(WS_EX_CLIENTEDGE,_qstr("EDIT"), _qstr(""),WS_CHILD | WS_TABSTOP | WS_VISIBLE | ES_AUTOHSCROLL,rc,parent,1000);
		edt->SetFont(&G_QXSys().G_font(TQXSys::FNT_WINDOW));
		edt->SetWindowText(str);
	}
	return ylen;
}

void Tpropertyviewer::Set_readonly()
{
	readonly=true;
	if (edt!=NULL) edt->EnableWindow(false);
}


TSC_value* Tpropertyviewer::GetValue()
{
	QString str;
	if (edt!=NULL) edt->GetWindowText(str);
	if (combo!=NULL)
	{
		int nr=combo->GetCurSel();
		if (nr>=0) combo->GetLBText(nr,str);
	}
	value.fromstring(str);
	return &value;
}


Tpropertyviewer::~Tpropertyviewer()
{
	if (edt!=NULL) delete edt;
	if (combo!=NULL) delete combo;
}

//*****************************************************************************************
// Tobjectpropertyview
//*****************************************************************************************



Tobjectpropertyview::Tobjectpropertyview(StrPtr iname, Tmainwin *imainwin) : TQXview(iname)
{
	paramset=NULL;
	mainwin=imainwin;
}

Tobjectpropertyview::~Tobjectpropertyview()
{
}

void Tobjectpropertyview::PostCreate()
{
/*	RECT rc;
	rc.left=20;rc.top=40;rc.right=200;rc.bottom=65;
	CEdit *ed=new CEdit();
	ed->CreateEx(WS_EX_CLIENTEDGE,_qstr("EDIT"), _qstr(""),WS_CHILD | WS_TABSTOP | WS_VISIBLE,rc,this,1000);*/
}


void Tobjectpropertyview::updatescrollbars()
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


void Tobjectpropertyview::tranfercontent()
{
	if (paramset==NULL) return;
	for (int i=0; i<paramset->G_paramcount(); i++)
		if (!paramset->G_paramreadonly(i))
		{
			TSC_value vl;
			paramset->Set_paramcontent(i,viewers[i]->GetValue());
		}
	mainwin->objtreeview->redraw();
}

void Tobjectpropertyview::setcurrent(Tparamset *iparamset)
{
	int ylen;
	tranfercontent();
	if (G_scrollbar_vert()!=NULL) G_scrollbar_vert()->setscrollpos(0);
	paramset=iparamset;
	viewers.reset();
	int ypos=10;
	if (paramset!=NULL)
	{
		for (int i=0; i<paramset->G_paramcount(); i++)
		{
			ASSERT(paramset->G_paramcontent(i)!=NULL);
			Tarray<QString> optionlist;
			Tpropertyviewer *view=new Tpropertyviewer(this,paramset->G_paramcontent(i));
			if (paramset->G_param_optionlist(paramset->G_paramname(i),optionlist)) view->set_optionlist(optionlist);

			TSC_datatype *datatype=paramset->G_paramcontent(i)->G_datatype();
			if ((datatype!=NULL)&&(datatype->G_optioncount()>0))
			{
				Tarray<QString> optionlist;
				for (int j=0; j<datatype->G_optioncount(); j++)
					optionlist.add(new QString(datatype->G_option(j)));
				view->set_optionlist(optionlist);
			}

			if (paramset->G_paramcontent(i)->istype(SC_valname_boolean))
			{
				Tarray<QString> optionlist;
				optionlist.add(new QString("True"));optionlist.add(new QString("False"));
				view->set_optionlist(optionlist);
			}

			ylen=view->init(150,ypos);
			if (paramset->G_paramreadonly(i)) view->Set_readonly();
			viewers.add(view);
			ypos+=ylen;
		}
	}
	redraw();
	updatescrollbars();
}


void Tobjectpropertyview::SizeChanged()
{
}

void Tobjectpropertyview::MouseLButtonDown(UINT nFlags, CPoint point)
{
}




void Tobjectpropertyview::draw(CDC *dc, RECT &updaterect)
{
	TQXColor linecl=G_QXSys().G_color(TQXSys::CL_WINDOWBACK1)+G_QXSys().G_color(TQXSys::CL_TEXT);

	dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_WINDOWHEADER));
	dc->SetBkMode(TRANSPARENT);
	dc->SetTextColor(G_QXSys().G_color(TQXSys::CL_TEXT));
	if (paramset!=NULL)
	{
		for (int i=0; i<paramset->G_paramcount(); i++)
		{
			dc->FillSolidRect(10-G_offsetx(),viewers[i]->ypos-G_offsety()+15,200,2,linecl);
			dc->TextOutW(10-G_offsetx(),viewers[i]->ypos-G_offsety()+2,paramset->G_paramname(i),qstrlen(paramset->G_paramname(i)));
		}
	}

}




void Tobjectpropertyview::redraw()
{
	Invalidate(0);
}
