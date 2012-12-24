#include "stdafx.h"

#include "vecmath.h"
#include "vec_transformation.h"

#include "sc_script.h"

#include "3DScene.h"
#include "3DControls.h"

#include "opengl.h"

/////////////////////////////////////////////////////////////
// T3DControl
/////////////////////////////////////////////////////////////

T3DControl::T3DControl(T3DScene *iscene, bool addtocontrols) : T3DObject(iscene)
{
	position=G_valuecontent<Tvertex>(&addparam(_qstr("Position"),SC_valname_vertex)->content);

	sizeunit=addparam(_qstr("Size"),SC_valname_scalar)->content.G_content_scalar();
	sizeunit->copyfrom(0.05);

	color=G_valuecontent<TSC_color>(&addparam(_qstr("Color"),SC_valname_color)->content);
	color->copyfrom(1,1,1,1);
	if (addtocontrols) G_3DCosmos().controls_add(this);

	wasmodified=addparam(_qstr("WasModified"),SC_valname_boolean)->content.G_content_boolean();
	wasmodified->copyfrom(false);

	isdefault=addparam(_qstr("IsDefault"),SC_valname_boolean)->content.G_content_boolean();
	isdefault->copyfrom(false);

}


T3DControl::~T3DControl()
{
	G_3DCosmos().controls_del(this);
}


void T3DControl::precalcrender(Tprecalcrendercontext *pre)
{
	if (!G_isvisible()) return;
	ASSERT(parentframe!=NULL);
	rendercolor=*color;
	if (pre->parentsubframe!=NULL)
		rendercolor.mulwith(pre->parentsubframe->G_rendercolor());
}

bool T3DControl::G_isactive()
{
	return G_3DCosmos().G_activecontrol()==this;
}

void T3DControl::render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording)
{
	if (!G_isvisible()) return;
	if (!rc->G_viewport()->G_showcontrols()) return;
	if (!G_3DCosmos().G_showcontrols()) return;

	double x0=position->G3_x();
	double y0=position->G3_y();
	double sx=G_controlsizex();
	double sy=G_controlsizey();

	rc->enablelight(status->enablelight);
	rc->SetBlendType(status->currentobjectblendtype);
	rc->EnableDepthMask(status->currentobjectdepthmask!=DepthMaskDisable);
	rc->EnableDepthTest(status->currentobjectdepthtest!=DepthTestDisable);

	rc->set_orientation(+1);


	if ((G_canselect())&&(G_isactive()))
	{
		rc->set_color(0.0,0.4,0.0,1.0);
		render_frame(rc,x0-G_textsize()/5,y0-G_textsize()/5,sx+2*G_textsize()/5,sy+2*G_textsize()/5,G_offset());
	}
	rc->set_color(color->G_R(),color->G_G(),color->G_B(),color->G_A());
	render_impl(rc,status);
}

double T3DControl::G_textsize()
{
	return 0.8*G_sizeunit();
}

double T3DControl::G_offset()
{
	return 0.01*G_sizeunit();
}

void T3DControl::render_rect(Trendercontext *rc, double x0, double y0, double lx, double ly, double z0)
{
	glDisable(GL_CULL_FACE);
	glBegin(GL_QUADS);
	glVertex3d(x0,y0,z0);
	glVertex3d(x0+lx,y0,z0);
	glVertex3d(x0+lx,y0+ly,z0);
	glVertex3d(x0,y0+ly,z0);
	glEnd();
}

void T3DControl::render_rect2(Trendercontext *rc, double x0, double y0, double lx, double ly, double z0, TQXColor &cl1, TQXColor &cl2)
{
	glDisable(GL_CULL_FACE);
	glBegin(GL_QUADS);
	rc->set_color(cl1.G_r(),cl1.G_g(),cl1.G_b(),cl1.G_a());
	glTexCoord2d(0,0);
	glVertex3d(x0,y0,z0);
	rc->set_color(cl2.G_r(),cl2.G_g(),cl2.G_b(),cl2.G_a());
	glTexCoord2d(1,0);
	glVertex3d(x0+lx,y0,z0);
	glTexCoord2d(1,1);
	glVertex3d(x0+lx,y0+ly,z0);
	rc->set_color(cl1.G_r(),cl1.G_g(),cl1.G_b(),cl1.G_a());
	glTexCoord2d(0,1);
	glVertex3d(x0,y0+ly,z0);
	glEnd();
}

void T3DControl::RenderGenericBackGround(Trendercontext *rc)
{
	TSC_color *backcol=G_scene()->G_backcolor();
	TQXColor col_foreground(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	TQXColor col_background(backcol->G_R(),backcol->G_G(),backcol->G_B(),backcol->G_A());
	TQXColor cl1=10*col_background+col_foreground;
	TQXColor cl2=cl1;
	cl2=cl1.IntensIncr(0.25)+0.1*TQXColor(0.0,1.0,0.0);
	cl1.set(cl1.G_r(),cl1.G_g(),cl1.G_b(),col_foreground.G_a());
	cl2.set(cl2.G_r(),cl2.G_g(),cl2.G_b(),col_foreground.G_a());
	render_rect2(rc,G_controlposx(),G_controlposy(),G_controlsizex(),G_controlsizey(),G_offset(),cl1,cl2);
}


void T3DControl::render_frame(Trendercontext *rc, double x0, double y0, double lx, double ly, double z0)
{
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3d(x0,y0,z0);
	glVertex3d(x0+lx,y0,z0);
	glVertex3d(x0+lx,y0+ly,z0);
	glVertex3d(x0,y0+ly,z0);
	glEnd();
}

void T3DControl::MouseArrowClick(TMouseClickInfo info)
{
	double cpx=info.posit.G3_x();
	double cpy=info.posit.G3_y();
	if ((cpx>=G_controlposx())&&(cpx<=G_controlposx()+G_controlsizex())&&
		(cpy>=G_controlposy())&&(cpy<=G_controlposy()+G_controlsizey()))
	{
		G_3DCosmos().controls_activate(this);
	}
	MouseArrowClickImpl(info);
}



/////////////////////////////////////////////////////////////
// T3DControl_Frame
/////////////////////////////////////////////////////////////


T3DControl_Frame::T3DControl_Frame(T3DScene *iscene, bool addtocontrols) : T3DControl(iscene,addtocontrols)
{
	sizex=addparam(_qstr("SizeX"),SC_valname_scalar)->content.G_content_scalar();
	sizey=addparam(_qstr("SizeY"),SC_valname_scalar)->content.G_content_scalar();

/*	color->Set_R(0.15);
	color->Set_G(0.15);
	color->Set_B(0.15);*/
}

T3DControl_Frame::~T3DControl_Frame()
{
}


double T3DControl_Frame::G_controlsizex()
{
	return sizex->G_val();
}

double T3DControl_Frame::G_controlsizey()
{
	return sizey->G_val();
}




void T3DControl_Frame::render_impl(Trendercontext *rc, const TObjectRenderStatus *status)
{
	double x0=G_controlposx();
	double y0=G_controlposy();
	double z0=0;
	double sx=G_controlsizex();
	double sy=G_controlsizey();

	rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	render_rect(rc,x0,y0,sx,sy,z0);
}



/////////////////////////////////////////////////////////////
// T3DControl_Text
/////////////////////////////////////////////////////////////


T3DControl_Text::T3DControl_Text(T3DScene *iscene, bool addtocontrols) : T3DControl(iscene,addtocontrols)
{
	content=addparam(_qstr("Content"),SC_valname_string)->content.G_content_string();
}

T3DControl_Text::~T3DControl_Text()
{
}


double T3DControl_Text::G_controlsizex()
{
	return 0.0;
}

double T3DControl_Text::G_controlsizey()
{
	return G_sizeunit();
}




void T3DControl_Text::render_impl(Trendercontext *rc, const TObjectRenderStatus *status)
{
	double x0=G_controlposx();
	double y0=G_controlposy();
	double z0=G_offset();
	double sy=G_sizeunit();


	QString str;
	content->tostring(str,0);
	rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	rc->rendertext(DEFAULT_FONT,&Tvertex(x0,y0+sy/4,z0),
		&Tvector(G_textsize(),0,0),
		&Tvector(0,G_textsize(),0),str,0);
}



/////////////////////////////////////////////////////////////
// T3DControl_Scalar
/////////////////////////////////////////////////////////////


T3DControl_Scalar::T3DControl_Scalar(T3DScene *iscene, bool addtocontrols) : T3DControl(iscene,addtocontrols)
{
	sizex=addparam(_qstr("SizeX"),SC_valname_scalar)->content.G_content_scalar();
	rangesizex=addparam(_qstr("RangeSizeX"),SC_valname_scalar)->content.G_content_scalar();

	value=addparam(_qstr("Value"),SC_valname_scalar)->content.G_content_scalar();
	vmin=addparam(_qstr("Min"),SC_valname_scalar)->content.G_content_scalar();vmin->copyfrom(0.0);
	vmax=addparam(_qstr("Max"),SC_valname_scalar)->content.G_content_scalar();vmax->copyfrom(100.0);
	vstep=addparam(_qstr("Step"),SC_valname_scalar)->content.G_content_scalar();vstep->copyfrom(2.0);
	decimalcount=addparam(_qstr("DecimalCount"),SC_valname_scalar)->content.G_content_scalar();
}

T3DControl_Scalar::~T3DControl_Scalar()
{
}


double T3DControl_Scalar::G_controlsizex()
{
	return sizex->G_val()+rangesizex->G_val();
}

double T3DControl_Scalar::G_controlsizey()
{
	return G_sizeunit();
}


void T3DControl_Scalar::checkrange()
{
	double vl=value->G_val();
	double minvl=vmin->G_val();
	double maxvl=vmax->G_val();
	if (vl>maxvl) vl=maxvl;
	if (vl<minvl) vl=minvl;
	value->copyfrom(vl);
}

void T3DControl_Scalar::addstep(double stepcount)
{
	value->copyfrom(value->G_val()+stepcount*vstep->G_val());
	checkrange();
	Set_wasmodified();
}


bool T3DControl_Scalar::UI_OnPressed_Up()
{
	addstep(+1.0);
	return true;
}

bool T3DControl_Scalar::UI_OnPressed_Down()
{
	addstep(-1.0);
	return true;
}


bool T3DControl_Scalar::UI_OnKeyDown(UINT ch, bool shiftpressed, bool controlpressed)
{
	return false;
}

bool T3DControl_Scalar::UI_OnChar(UINT ch, bool shiftpressed, bool controlpressed)
{
	if (ch=='+')
	{
		addstep(+1.0);
		return true;
	}
	if (ch=='-')
	{
		addstep(-1.0);
		return true;
	}
	return false;
}


void T3DControl_Scalar::render_impl(Trendercontext *rc, const TObjectRenderStatus *status)
{
	double x0=G_controlposx();
	double y0=G_controlposy();
	double z0=G_offset();
	double sx=sizex->G_val();
	double sy=G_sizeunit();

	double vl=value->G_val();
	int digitcount=decimalcount->G_intval();

	RenderGenericBackGround(rc);

	TCHAR formatstr[100];
	CString str;
	_stprintf_s(formatstr,99,_qstr("%%0.%dlf"),digitcount);
	str.Format(formatstr,vl);

	rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	rc->rendertext(DEFAULT_FONT,&Tvertex(x0+sy/10,y0+sy/4,z0),
		&Tvector(G_textsize(),0,0),
		&Tvector(0,G_textsize(),0),str,0);

	rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	render_frame(rc,x0,y0,sx,sy,z0+G_offset());

	//draw range
	x0=G_controlposx()+sizex->G_val()+G_sizeunit()/5;
	y0=y0+sy/2;
	sx=rangesizex->G_val()-G_sizeunit()/5;
	if (sx>0)
	{
		double x1=sx*(value->G_val()-vmin->G_val())/(vmax->G_val()-vmin->G_val());
		if (x1<0) x1=0;if (x1>sx) x1=sx;
		x1+=x0;
		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glVertex3d(x0,y0,z0);
		glVertex3d(x0+sx,y0,z0);
		glVertex3d(x1,y0-sy/4,z0);
		glVertex3d(x1,y0+sy/4,z0);
		glEnd();
	}

}


void T3DControl_Scalar::paramchanged(StrPtr iname)
{
	checkrange();
}


/////////////////////////////////////////////////////////////
// T3DControl_Edit
/////////////////////////////////////////////////////////////


T3DControl_Edit::T3DControl_Edit(T3DScene *iscene, bool addtocontrols) : T3DControl(iscene,addtocontrols)
{
	sizex=addparam(_qstr("SizeX"),SC_valname_scalar)->content.G_content_scalar();

	content=addparam(_qstr("Content"),SC_valname_string)->content.G_content_string();
}

T3DControl_Edit::~T3DControl_Edit()
{
}


double T3DControl_Edit::G_controlsizex()
{
	return sizex->G_val();
}

double T3DControl_Edit::G_controlsizey()
{
	return G_sizeunit();
}


bool T3DControl_Edit::UI_OnKeyDown(UINT ch, bool shiftpressed, bool controlpressed)
{
	if (ch==VK_BACK)
	{
		QString str;
		content->tostring(str,0);
		str.substring(0,str.G_length()-2);
		content->fromstring(str);
		Set_wasmodified();
		return true;
	}
	return false;
}

bool T3DControl_Edit::UI_OnChar(UINT ch, bool shiftpressed, bool controlpressed)
{
	if (ch>=31)
	{
		QString str;
		content->tostring(str,0);
		str+=ch;
		content->fromstring(str);
		Set_wasmodified();
		return true;
	}
	return false;
}


void T3DControl_Edit::render_impl(Trendercontext *rc, const TObjectRenderStatus *status)
{
	double x0=G_controlposx();
	double y0=G_controlposy();
	double z0=G_offset();
	double sx=sizex->G_val();
	double sy=G_sizeunit();

	RenderGenericBackGround(rc);

	QString str;
	str=content->G_string();
	if (G_isactive())
		str+=_qstr("|");
	rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	rc->rendertext(DEFAULT_FONT,&Tvertex(x0+sy/10,y0+sy/4,z0),
		&Tvector(G_textsize(),0,0),
		&Tvector(0,G_textsize(),0),str,0);

	rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	render_frame(rc,x0,y0,sx,sy,z0+G_offset());
}


void T3DControl_Edit::paramchanged(StrPtr iname)
{
}



/////////////////////////////////////////////////////////////
// T3DControl_Check
/////////////////////////////////////////////////////////////


T3DControl_Check::T3DControl_Check(T3DScene *iscene, bool addtocontrols) : T3DControl(iscene,addtocontrols)
{
	checked=addparam(_qstr("Checked"),SC_valname_boolean)->content.G_content_boolean();
}

T3DControl_Check::~T3DControl_Check()
{
}


double T3DControl_Check::G_controlsizex()
{
	return G_sizeunit();
}

double T3DControl_Check::G_controlsizey()
{
	return G_sizeunit();
}


bool T3DControl_Check::UI_OnKeyDown(UINT ch, bool shiftpressed, bool controlpressed)
{
	if (ch==VK_RETURN)
	{
		checked->copyfrom(!checked->G_val());
		Set_wasmodified();
		return true;
	}
	return false;
}

bool T3DControl_Check::UI_OnChar(UINT ch, bool shiftpressed, bool controlpressed)
{
	if (ch=='-')
	{
		checked->copyfrom(false);
		Set_wasmodified();
		return true;
	}
	if (ch=='+')
	{
		checked->copyfrom(true);
		Set_wasmodified();
		return true;
	}
	return false;
}

void T3DControl_Check::MouseArrowClickImpl(TMouseClickInfo info)
{
	double cpx=info.posit.G3_x();
	double cpy=info.posit.G3_y();
	if ((cpx>=G_controlposx())&&(cpx<=G_controlposx()+G_controlsizex())&&
		(cpy>=G_controlposy())&&(cpy<=G_controlposy()+G_controlsizey()))
	{
		checked->copyfrom(!checked->G_val());
		Set_wasmodified();
	}
}

void T3DControl_Check::render_impl(Trendercontext *rc, const TObjectRenderStatus *status)
{
	double x0=G_controlposx();
	double y0=G_controlposy();
	double z0=G_offset();
	double sx=G_sizeunit();
	double sy=G_sizeunit();

	RenderGenericBackGround(rc);

	if (checked->G_val())
	{
		rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glVertex3d(x0,y0,z0);
		glVertex3d(x0+sx,y0+sy,z0);
		glVertex3d(x0+sx,y0,z0);
		glVertex3d(x0,y0+sy,z0);
		glEnd();
	}


	rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	render_frame(rc,x0,y0,sx,sy,z0+G_offset());
}



/////////////////////////////////////////////////////////////
// T3DControl_Button
/////////////////////////////////////////////////////////////


T3DControl_Button::T3DControl_Button(T3DScene *iscene, bool addtocontrols) : T3DControl(iscene,addtocontrols)
{
	sizex=addparam(_qstr("SizeX"),SC_valname_scalar)->content.G_content_scalar();
	content=addparam(_qstr("Content"),SC_valname_string)->content.G_content_string();
}

T3DControl_Button::~T3DControl_Button()
{
}


double T3DControl_Button::G_controlsizex()
{
	return sizex->G_val();
}

double T3DControl_Button::G_controlsizey()
{
	return G_sizeunit();
}


bool T3DControl_Button::UI_OnKeyDown(UINT ch, bool shiftpressed, bool controlpressed)
{
	if (ch==VK_RETURN)
	{
		Set_wasmodified();
		return true;
	}
	return false;
}

void T3DControl_Button::MouseArrowClickImpl(TMouseClickInfo info)
{
	if (info.clicktype==TMouseClickInfo::LeftDown)
	{
		double cpx=info.posit.G3_x();
		double cpy=info.posit.G3_y();
		if ((cpx>=G_controlposx())&&(cpx<=G_controlposx()+G_controlsizex())&&
			(cpy>=G_controlposy())&&(cpy<=G_controlposy()+G_controlsizey()))
		{
			Set_wasmodified();
		}
	}

	if ((IsDefault())&&(info.clicktype==TMouseClickInfo::LeftDouble))
		Set_wasmodified();
}



void T3DControl_Button::render_impl(Trendercontext *rc, const TObjectRenderStatus *status)
{
	double x0=G_controlposx();
	double y0=G_controlposy();
	double z0=G_offset();
	double sx=sizex->G_val();
	double sy=G_sizeunit();



	QString str;
	content->tostring(str,0);

	//rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	rc->set_color(0.2,0.8,0.2,rendercolor.G_A());
	rc->rendertext(DEFAULT_FONT,&Tvertex(x0+sy/5,y0+sy/4,z0),
		&Tvector(G_textsize(),0,0),
		&Tvector(0,G_textsize(),0),str,0);

	//rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());

	double lx=sx;double ly=sy;
	double oo=sy/4;
	glLineWidth(3.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3d(x0+oo,y0,z0);
	glVertex3d(x0+lx-oo,y0,z0);
	glVertex3d(x0+lx,y0+oo,z0);
	glVertex3d(x0+lx,y0+ly-oo,z0);
	glVertex3d(x0+lx-oo,y0+ly,z0);
	glVertex3d(x0+oo,y0+ly,z0);
	glVertex3d(x0,y0+ly-oo,z0);
	glVertex3d(x0,y0+oo,z0);
	glEnd();

	//render_frame(x0,y0,sx,sy,z0+G_offset());
}




/////////////////////////////////////////////////////////////
// T3DControl_List
/////////////////////////////////////////////////////////////


T3DControl_List::T3DControl_List(T3DScene *iscene, bool addtocontrols) : T3DControl(iscene,addtocontrols)
{
	sizex=addparam(_qstr("SizeX"),SC_valname_scalar)->content.G_content_scalar();

	list=G_valuecontent<TSC_list>(&addparam(_qstr("List"),SC_valname_list)->content);

	selectidx=addparam(_qstr("SelectIdx"),SC_valname_scalar)->content.G_content_scalar();
	selectidx->copyfrom(0.0);

	county=addparam(_qstr("CountY"),SC_valname_scalar)->content.G_content_scalar();
	county->copyfrom(4);

	offset=0;

}

T3DControl_List::~T3DControl_List()
{
}

TSC_value* T3DControl_List::G_selecteditem()
{
	return list->get(selectidx->G_intval());
}



double T3DControl_List::G_controlsizex()
{
	return sizex->G_val();
}

double T3DControl_List::G_controlsizey()
{
	return G_sizeunit()*county->G_val();
}


void T3DControl_List::MouseArrowClickImpl(TMouseClickInfo info)
{
	double x0=G_controlposx();
	double y0=G_controlposy();
	double z0=G_offset();
	double sx=sizex->G_val();
	int linecount=county->G_intval();
	double unity=G_sizeunit();

	double cpx=info.posit.G3_x();
	double cpy=info.posit.G3_y();
	if ((cpx<x0)||(cpx>=x0+sx)) return;

	int linenr=linecount-(cpy-y0-unity/10)/unity;
	if ((linenr>=0)&&(linenr<linecount)&&(linenr+offset<list->G_size()))
	{
		if (info.clicktype==TMouseClickInfo::LeftDown)
		{
			selectidx->copyfrom(linenr+offset);
			paramchanged(_qstr(""));
			Set_wasmodified();
		}

		if (info.clicktype==TMouseClickInfo::ScrollDown)
		{
			offset=min(list->G_size()-1,offset+2);
		}
		if (info.clicktype==TMouseClickInfo::ScrollUp)
		{
			offset=max(0,offset-2);
		}
	}
}


void T3DControl_List::render_impl(Trendercontext *rc, const TObjectRenderStatus *status)
{
	QString str;

	double x0=G_controlposx();
	double y0=G_controlposy();
	double z0=G_offset();
	double sx=sizex->G_val();

	TSC_color *backcol=G_scene()->G_backcolor();

	TQXColor col_foreground(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	TQXColor col_background(backcol->G_R(),backcol->G_G(),backcol->G_B(),backcol->G_A());
	TQXColor selcl1=3*col_background+col_foreground;
	TQXColor selcl2=selcl1.IntensIncr(0.25)+0.2*TQXColor(0.0,1.0,0.0);

	int linecount=county->G_intval();
	double unity=G_sizeunit();

	RenderGenericBackGround(rc);

	for (int i=0; i<list->G_size(); i++)
		if ((i-offset>=0)&&(i-offset<linecount))
		{
			double yps=y0+unity/10+unity*(linecount-1-(i-offset));
			if (i==selectidx->G_intval())
			{
				rc->set_color((rendercolor.G_R()+backcol->G_R())/2,(rendercolor.G_G()+backcol->G_G())/2,(rendercolor.G_B()+backcol->G_B())/2,(rendercolor.G_A()+backcol->G_A())/2);
				render_rect2(rc,x0,yps,sx,unity,z0,selcl1,selcl2);
			}
			list->get(i)->tostring(str);
			rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
			rc->rendertext(DEFAULT_FONT,&Tvertex(x0+unity/10,yps+unity/4,z0+G_offset()),
				&Tvector(G_textsize(),0,0),
				&Tvector(0,G_textsize(),0),str,0);
		}

	rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	render_frame(rc,x0,y0,sx,unity*linecount,z0+G_offset());
}


bool T3DControl_List::UI_OnPressed_Up()
{
	if (selectidx->G_val()>0)
	{
		selectidx->copyfrom(selectidx->G_intval()-1);
		paramchanged(_qstr(""));
		Set_wasmodified();
		return true;
	}
	return false;
}

bool T3DControl_List::UI_OnPressed_Down()
{
	if (selectidx->G_val()<list->G_size()-1)
	{
		selectidx->copyfrom(selectidx->G_intval()+1);
		paramchanged(_qstr(""));
		Set_wasmodified();
		return true;
	}
	return false;
}

bool T3DControl_List::UI_OnChar(UINT ch, bool shiftpressed, bool controlpressed)
{
	if (ch=='-') return UI_OnPressed_Up();
	if (ch=='+') return UI_OnPressed_Down();
	return false;
}


void T3DControl_List::paramchanged(StrPtr iname)
{
	if (offset>selectidx->G_val()) offset=selectidx->G_val();
	if (offset<=selectidx->G_val()-county->G_val()) offset=selectidx->G_val()-county->G_val()+1;
	if (offset<0) offset=0;
}



/////////////////////////////////////////////////////////////
// T3DControl_Menu
/////////////////////////////////////////////////////////////


void T3DControl_Menu_Item::additem(StrPtr iID, StrPtr iname, bool cancheck)
{
	T3DControl_Menu_Item *it=new T3DControl_Menu_Item(this);subitems.add(it);
	it->ID=iID;
	it->name=iname;
	it->cancheck=cancheck;
}


T3DControl_Menu_Item* T3DControl_Menu_Item::finditem(StrPtr iID)
{
	if (issame(iID,ID)) return this;
	for (int i=0; i<subitems.G_count(); i++)
	{
		T3DControl_Menu_Item *rs=subitems[i]->finditem(iID);
		if (rs!=NULL) return rs;
	}
	return NULL;
}


T3DControl_Menu::T3DControl_Menu(T3DScene *iscene, bool addtocontrols) : T3DControl(iscene,addtocontrols) , root(NULL)
{
	sizex=addparam(_qstr("SizeX"),SC_valname_scalar)->content.G_content_scalar();
	selectid=addparam(_qstr("SelectID"),SC_valname_string)->content.G_content_string();

	addparam(_qstr("Texture"),SC_valname_string,false,_text(""),PARAMSUBCLASS_APPEARANCE);

	root.cursubsel=0;

}

T3DControl_Menu::~T3DControl_Menu()
{
}

void T3DControl_Menu::additem(StrPtr iID, StrPtr iname, StrPtr parentid, bool cancheck)
{
	T3DControl_Menu_Item *parent=&root;
	if (qstrlen(parentid)>0)
	{
		parent=root.finditem(parentid);
		if (parent==NULL)
		{
			QString err;FormatString(err,_text("Invalid parent menu ID '^1'"),parentid);
			throw QError(err);
		}
	}
	parent->additem(iID,iname,cancheck);
}


T3DControl_Menu_Item* T3DControl_Menu::G_selitem()
{
	T3DControl_Menu_Item *it=&root;
	bool finished=false;
	while (!finished)
	{
		if ((it->cursubsel>=0)&&(it->cursubsel<it->subitems.G_count()))
			it=it->subitems[it->cursubsel];
		else finished=true;
	}
	return it;
}


void T3DControl_Menu::TestClickMenu(T3DControl_Menu_Item *parentitem,double x0, double y0, double clickx, double clicky)
{
	double sx=sizex->G_val();
	double yp=y0;

	for (int i=0; i<parentitem->subitems.G_count(); i++)
	{
		T3DControl_Menu_Item *item=parentitem->subitems[i];
		double lineh=GetItemH(*item);
		if (i==parentitem->cursubsel)
			TestClickMenu(item,x0+sx+lineh/10,yp,clickx,clicky);
		if ((clickx>=x0)&&(clickx<x0+sx)&&(clicky<=yp)&&(clicky>=yp-lineh))
		{
			parentitem->cursubsel=i;
			item->cursubsel=-1;
			UI_OnKeyDown(VK_RETURN,false,false);
			//if ((selitem!=NULL)&&(selitem->subitems.G_count()==0))
			//{
			//	if (selitem->cancheck) selitem->checked=!selitem->checked;
			//	setselectid();
			//	Set_wasmodified();
			//}
		}
		yp-=lineh;
	}
}


double T3DControl_Menu::GetItemH(T3DControl_Menu_Item &item)
{
	if (!item.IsSeparator())
		return G_sizeunit();
	else
		return G_sizeunit()/3;
}


void T3DControl_Menu::rendermenu(T3DControl_Menu_Item *parentitem, Trendercontext *rc, double x0, double y0)
{
	StrPtr texturename=G_texturename();
	T3DTexture *texture=NULL;
	if (qstrlen(texturename)>0)
	{
		texture=parentframe->G_texture(texturename);
		if (texture==NULL)
		{
			QString errstr;FormatString(errstr,_text("Could not find texture '^1'"),texturename);
			throw QError(errstr);
		}
	}

	TQXColor col_foreground(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());

	TSC_color *backcol=G_scene()->G_backcolor();
	TQXColor col_background(backcol->G_R(),backcol->G_G(),backcol->G_B(),backcol->G_A());

	TQXColor col_menuback=6*col_background+col_foreground;


	TQXColor col_sel=TQXColor(0,0.7,0);

	TQXColor col_menubacksel=1.5*col_background+col_foreground;

	TQXColor col_menubackactivesel=col_menubacksel+1.5*col_sel;

	if (texture!=NULL)
	{
		col_menuback=col_foreground;
		col_menubacksel=col_foreground;
		col_menubackactivesel=col_sel;
	}


	double z0=G_offset();
	double sx=sizex->G_val();
	double yp=y0;
	for (int i=0; i<parentitem->subitems.G_count(); i++)
	{
		T3DControl_Menu_Item *item=parentitem->subitems[i];
		double lineh=GetItemH(*item);

		TQXColor cl1=col_menuback;
		if (i==parentitem->cursubsel)
		{
			cl1=col_menubacksel;
			if (G_isactive())
				cl1=col_menubackactivesel;
		}

		TQXColor cl2=cl1;
		if (texture==NULL)
			cl2=cl1.IntensIncr(0.10)+0.05*TQXColor(0.0,1.0,0.0);
		cl1.set(cl1.G_r(),cl1.G_g(),cl1.G_b(),col_foreground.G_a());
		cl2.set(cl2.G_r(),cl2.G_g(),cl2.G_b(),col_foreground.G_a());

		if (texture!=NULL) texture->select(rc);
		render_rect2(rc,x0,yp-lineh,sx,lineh,z0,cl1,cl2);
		if (texture!=NULL) texture->unselect(rc);

		if (!item->IsSeparator())
		{
			rc->set_color(0,0,0,col_foreground.G_a());
			rc->rendertext(DEFAULT_FONT,&Tvertex(x0+0.7*lineh+G_textsize()/15,yp-lineh+lineh/4-G_textsize()/15,z0+G_offset()/2),
				&Tvector(G_textsize(),0,0),
				&Tvector(0,G_textsize(),0),item->name,0);

			rc->set_color(col_foreground.G_r(),col_foreground.G_g(),col_foreground.G_b(),col_foreground.G_a());
			rc->rendertext(DEFAULT_FONT,&Tvertex(x0+0.7*lineh,yp-lineh+lineh/4,z0+G_offset()),
				&Tvector(G_textsize(),0,0),
				&Tvector(0,G_textsize(),0),item->name,0);
		}
		else
		{
			rc->set_color(col_foreground.G_r(),col_foreground.G_g(),col_foreground.G_b(),col_foreground.G_a());
			//render_rect(rc,x0,yp,sx,lineh/3,z0+G_offset());
			render_rect2(rc,x0,yp-lineh/2-lineh/6,sx,lineh/3,z0+G_offset(),3*col_foreground+col_menuback,6*col_foreground+col_menuback);
		}

		if (item->checked)
		{
			double oo=lineh/8;
			glLineWidth(2.0f);
			glBegin(GL_LINE_STRIP);
			glVertex3d(x0+lineh/4-oo,yp-lineh+0.4*lineh+oo,z0+G_offset());
			glVertex3d(x0+lineh/4,yp-lineh+0.4*lineh,z0+G_offset());
			glVertex3d(x0+lineh/4+2*oo,yp-lineh+0.4*lineh+2*oo,z0+G_offset());
			glEnd();
		}

		if (item->subitems.G_count()>0)
		{
			double oo=lineh/4;
//			glLineWidth(2.0f);
			glBegin(GL_TRIANGLES);
			glVertex3d(x0+sx-oo/2-oo,yp-lineh+lineh/2-oo,z0+G_offset());
			glVertex3d(x0+sx-oo/2,yp-lineh+lineh/2,z0+G_offset());
			glVertex3d(x0+sx-oo/2-oo,yp-lineh+lineh/2+oo,z0+G_offset());
			glEnd();
		}

		if (i==parentitem->cursubsel) rendermenu(item,rc,x0+sx+lineh/10,yp);

		yp-=lineh;
	}
}


void T3DControl_Menu::render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording)
{
	if (!G_isvisible()) return;
	if (!rc->G_viewport()->G_showcontrols()) return;
	if (!G_3DCosmos().G_showcontrols()) return;

	rc->enablelight(status->enablelight);
	rc->SetBlendType(status->currentobjectblendtype);
	rc->EnableDepthMask(status->currentobjectdepthmask!=DepthMaskDisable);
	rc->EnableDepthTest(status->currentobjectdepthtest!=DepthTestDisable);

	rc->set_orientation(+1);



	rendermenu(&root,rc,G_controlposx(),G_controlposy());


}


void T3DControl_Menu::setselectid()
{
	T3DControl_Menu_Item *selitem=G_selitem();
	selectid->fromstring(selitem->ID);
}

bool T3DControl_Menu::UI_OnPressed_Up()
{
	T3DControl_Menu_Item *selitem=G_selitem();
	if (selitem->parent!=NULL)
	{
		do
			selitem->parent->cursubsel=(selitem->parent->cursubsel-1+selitem->parent->subitems.G_count())%selitem->parent->subitems.G_count();
		while (selitem->parent->subitems[selitem->parent->cursubsel]->IsSeparator());
		setselectid();
	}
	return true;
}

bool T3DControl_Menu::UI_OnPressed_Down()
{
	T3DControl_Menu_Item *selitem=G_selitem();
	if (selitem->parent==NULL)
	{
		if (root.subitems.G_count()>0)
		{
			root.cursubsel=0;
			setselectid();
		}
	}
	else
	{
		do
			selitem->parent->cursubsel=(selitem->parent->cursubsel+1)%selitem->parent->subitems.G_count();
		while (selitem->parent->subitems[selitem->parent->cursubsel]->IsSeparator());
		setselectid();
	}
	return true;
}

bool T3DControl_Menu::UI_OnKeyDown(UINT ch, bool shiftpressed, bool controlpressed)
{
	T3DControl_Menu_Item *selitem=G_selitem();
	if ((ch==VK_LEFT)&&(!shiftpressed)&&(!controlpressed))
	{
		if ((selitem->parent!=NULL)&&(selitem->parent->parent!=NULL))
		{
			selitem->parent->cursubsel=-1;
			setselectid();
			return true;
		}
	}
	if ((ch==VK_RIGHT)&&(!shiftpressed)&&(!controlpressed))
	{
		if (selitem->subitems.G_count()>0)
		{
			selitem->cursubsel=0;
			setselectid();
			return true;
		}
	}

	if ((ch==VK_TAB)&&(!shiftpressed)&&(!controlpressed)) return UI_OnKeyDown(VK_RIGHT,false,false);
	if ((ch==VK_TAB)&&( shiftpressed)&&(!controlpressed)) return UI_OnKeyDown(VK_LEFT,false,false);

	if (ch==VK_RETURN)
	{
		if ((selitem!=NULL)&&(selitem->subitems.G_count()==0))
		{
			if (selitem->cancheck) selitem->checked=!selitem->checked;
			setselectid();
			Set_wasmodified();
			return true;
		}
	}

	return false;
}


bool T3DControl_Menu::UI_OnChar(UINT ch, bool shiftpressed, bool controlpressed)
{
	if (ch=='-') return UI_OnPressed_Up();
	if (ch=='+') return UI_OnPressed_Down();
	return false;
}


bool T3DControl_Menu::SelectItem(T3DControl_Menu_Item *current, StrPtr id)
{
	current->cursubsel=-1;
	if (issame(id,current->ID))
	{
		return true;
	}

	for (int i=0; i<current->subitems.G_count(); i++)
		if (SelectItem(current->subitems[i],id))
		{
			current->cursubsel=i;
			return true;
		}
	return false;
}


void T3DControl_Menu::paramchanged(StrPtr iname)
{
	if (issame(iname,_qstr("SelectID")))
	{
		SelectItem(&root,selectid->G_string());
	}
/*	if (offset>selectidx->G_val()) offset=selectidx->G_val();
	if (offset<=selectidx->G_val()-county->G_val()) offset=selectidx->G_val()-county->G_val()+1;
	if (offset<0) offset=0;*/
}

void T3DControl_Menu::MouseArrowClickImpl(TMouseClickInfo info)
{
	if (info.clicktype==TMouseClickInfo::LeftDown)
		TestClickMenu(&root,G_controlposx(),G_controlposy(),info.posit.G3_x(),info.posit.G3_y());
}


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_control_list_getselection,Selected)
{
	setreturntype(SC_valname_any);
	setmemberfunction(SC_valname_control_list);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DControl_List *list=G_valuecontent<T3DControl_List>(owner);
	retval->copyfrom(list->G_selecteditem());
}
ENDFUNCTION(func_control_list_getselection)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_control_select,ActivateControl)
{
	addvar(_qstr("control"),SC_valname_any);
	extendclasspath(SC_treeclass_controls);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DControl *ctrl=(T3DControl*)(arglist->get(0)->G_content());
	G_3DCosmos().controls_activate(ctrl);
//	T3DControl_List *list=G_valuecontent<T3DControl_List>(owner);
//	retval->copyfrom(list->G_selecteditem());
}
ENDFUNCTION(func_control_select)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_control_menu_additem,Add)
{
	setmemberfunction(SC_valname_control_menu);
	addvar(_qstr("ParentID"),SC_valname_string);
	addvar(_qstr("Content"),SC_valname_string);
	addvar(_qstr("ID"),SC_valname_string,false);
	addvar(_qstr("CanCheck"),SC_valname_boolean,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DControl_Menu *menu=G_valuecontent<T3DControl_Menu>(owner);
	QString parentid=arglist->get(0)->G_content_string()->G_string();
	QString name=arglist->get(1)->G_content_string()->G_string();
	QString id=name;
	if (arglist->G_ispresent(2))
		id=arglist->get(2)->G_content_string()->G_string();
	bool cancheck=false;
	if (arglist->G_ispresent(3)) cancheck=arglist->get(3)->G_content_boolean()->G_val();
	menu->additem(id,name,parentid,cancheck);
}
ENDFUNCTION(func_control_menu_additem)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_control_menu_ischecked,Checked)
{
	setmemberfunction(SC_valname_control_menu);
	addvar(_qstr("ID"),SC_valname_string);
	setreturntype(SC_valname_boolean);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DControl_Menu *menu=G_valuecontent<T3DControl_Menu>(owner);
	QString id=arglist->get(0)->G_content_string()->G_string();
	T3DControl_Menu_Item *item=menu->root.finditem(id);
	if (item==NULL) throw QError(_text("Invalid menu item ID"));
	if (assigntoval!=NULL)
		item->checked=assigntoval->G_content_boolean()->G_val();
	retval->G_content_boolean()->copyfrom(item->checked);
}
ENDFUNCTION(func_control_menu_ischecked)


