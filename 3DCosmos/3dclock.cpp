#include "stdafx.h"

#include "qfile.h"
#include "qtranslate.h"

#include "astrotools.h"

#include "3DScene.h"

#include "3DClock.h"

#include "SC_script.h"
#include "SC_functor.h"

#include "opengl.h"

StrPtr G_datadir();
StrPtr G_scriptsdir();
StrPtr G_texturesdir();


//******************************************************************
// T3DObjectClock
//******************************************************************

T3DObjectClock::T3DObjectClock(T3DScene *iscene) : T3DGeoObject(iscene)
{
	clocktype=&addparam(_qstr("Type"),SC_valname_ClockType)->content;
	size=addparam(_qstr("Size"),SC_valname_scalar)->content.G_content_scalar();
	timeshift=addparam(_qstr("TimeShift"),SC_valname_scalar)->content.G_content_scalar();

	hasowntime=addparam(_qstr("HasOwnTime"),SC_valname_boolean)->content.G_content_boolean();
	hasowntime->copyfrom(false);

	disptime=G_valuecontent<TSC_time>(&addparam(_qstr("Time"),SC_valname_time)->content);

	axis1=G_valuecontent<Tvector>(&addparam(_qstr("Axis1"),SC_valname_vector)->content);
	axis2=G_valuecontent<Tvector>(&addparam(_qstr("Axis2"),SC_valname_vector)->content);

	monthnames=G_valuecontent<TSC_list>(&addparam(_qstr("Monthnames"),SC_valname_list)->content);

	TSC_value val;
	val.settype(GetTSCenv().G_datatype(SC_valname_string));

	val.fromstring(_TRL(_text("January")));monthnames->add(&val);
	val.fromstring(_TRL(_text("February")));monthnames->add(&val);
	val.fromstring(_TRL(_text("March")));monthnames->add(&val);
	val.fromstring(_TRL(_text("April")));monthnames->add(&val);
	val.fromstring(_TRL(_text("May")));monthnames->add(&val);
	val.fromstring(_TRL(_text("June")));monthnames->add(&val);
	val.fromstring(_TRL(_text("July")));monthnames->add(&val);
	val.fromstring(_TRL(_text("August")));monthnames->add(&val);
	val.fromstring(_TRL(_text("September")));monthnames->add(&val);
	val.fromstring(_TRL(_text("October")));monthnames->add(&val);
	val.fromstring(_TRL(_text("November")));monthnames->add(&val);
	val.fromstring(_TRL(_text("December")));monthnames->add(&val);

	size->copyfrom(1.0);
	timeshift->copyfrom(0.0);
	axis1->copyfrom(1,0,0);
	axis2->copyfrom(0,1,0);
}


void T3DObjectClock::init()
{
}


void T3DObjectClock::precalcrender_exec(Tprecalcrendercontext *pre)
{
}


void T3DObjectClock::render_exec(Trendercontext *rc)
{
	Tvector pt;

	if (!hasowntime->G_val())
	{
		*disptime=*G_3DCosmos().G_time();
		disptime->addseconds(timeshift->G_val());
	}

	if (clocktype->toint()==ClockType_analog)
		render_exec_analogclock(rc,*disptime,size->G_val());

	if (clocktype->toint()==ClockType_digital)
		render_exec_digitalclock(rc,*disptime,size->G_val());

	if (clocktype->toint()==ClockType_calendar)
		render_exec_calendar(rc,*disptime,size->G_val());

	if (clocktype->toint()==ClockType_date)
		render_exec_date(rc,*disptime,size->G_val());

	if (clocktype->toint()==ClockType_datetime)
		render_exec_datetime(rc,*disptime,size->G_val());

}

void T3DObjectClock::render_exec_analogclock(Trendercontext *rc, TSC_time &disptime, double size)
{
	Tvector pt;
	for (int hr=0; hr<12; hr++)
	{
		double ang=hr/12.0*2*Pi;
		double ang1=ang-1.2/180*Pi;
		double ang2=ang+1.2/180*Pi;
		double rd1=size*0.92;
		double rd2=size*0.98;
		if (hr%3==0) rd1=size*0.85;
		glBegin(GL_QUADS);
		pt.lincombfrom(rd1*sin(ang2),axis1,rd1*cos(ang2),axis2);		glVertex3d(pt.G_x(),pt.G_y(),pt.G_z());
		pt.lincombfrom(rd2*sin(ang2),axis1,rd2*cos(ang2),axis2);		glVertex3d(pt.G_x(),pt.G_y(),pt.G_z());
		pt.lincombfrom(rd2*sin(ang1),axis1,rd2*cos(ang1),axis2);		glVertex3d(pt.G_x(),pt.G_y(),pt.G_z());
		pt.lincombfrom(rd1*sin(ang1),axis1,rd1*cos(ang1),axis2);		glVertex3d(pt.G_x(),pt.G_y(),pt.G_z());
		glEnd();
	}

	//minutes hand
	{
		double ang=(disptime.G_min()+disptime.G_sec()/60.0)/60.0*2*Pi;
		double ang1=ang-165.0/180*Pi;
		double ang2=ang+165.0/180*Pi;
		double rda=size*0.98;
		double rdb=size*0.08;
		glBegin(GL_TRIANGLES);
		pt.lincombfrom(rda*sin(ang),axis1,rda*cos(ang),axis2);		glVertex3d(pt.G_x(),pt.G_y(),pt.G_z());
		pt.lincombfrom(rdb*sin(ang1),axis1,rdb*cos(ang1),axis2);		glVertex3d(pt.G_x(),pt.G_y(),pt.G_z());
		pt.lincombfrom(rdb*sin(ang2),axis1,rdb*cos(ang2),axis2);		glVertex3d(pt.G_x(),pt.G_y(),pt.G_z());
		glEnd();
	}

	//hours hand
	{
		double ang=(disptime.G_hour()+disptime.G_min()/60.0+disptime.G_sec()/3600.0)/12.0*2*Pi;
		double ang1=ang-165.0/180*Pi;
		double ang2=ang+165.0/180*Pi;
		double rda=size*0.5;
		double rdb=size*0.12;
		glBegin(GL_TRIANGLES);
		pt.lincombfrom(rda*sin(ang),axis1,rda*cos(ang),axis2);		glVertex3d(pt.G_x(),pt.G_y(),pt.G_z());
		pt.lincombfrom(rdb*sin(ang1),axis1,rdb*cos(ang1),axis2);		glVertex3d(pt.G_x(),pt.G_y(),pt.G_z());
		pt.lincombfrom(rdb*sin(ang2),axis1,rdb*cos(ang2),axis2);		glVertex3d(pt.G_x(),pt.G_y(),pt.G_z());
		glEnd();
	}

	//am/pm sign
	{
		Tvertex textposit;
		Tvector textdir1,textdir2;
		QString content=_qstr("AM");
		if (disptime.G_hour()+disptime.G_min()/60.0+disptime.G_sec()/3600.0>12) content=_qstr("PM");
		textposit.lincombfrom(1,&Tvertex(0,0,0),-0.5*size,axis2);
		textposit.lincombfrom(1,&textposit,-0.13*size,axis1);
		textdir1.mulfrom(axis1,0.25*size);
		textdir2.mulfrom(axis2,0.25*size);
		rc->rendertext(DEFAULT_FONT,&textposit,&textdir1,&textdir2,content,0);
	}
}


void T3DObjectClock::render_exec_digitalclock(Trendercontext *rc, TSC_time &disptime, double size)
{
	QString content;
	disptime.tostring(content,1);
	render_exec_string(rc,content);
}

void T3DObjectClock::render_exec_calendar(Trendercontext *rc, TSC_time &disptime, double size)
{
	try{
		QString content,monthstring;

		monthnames->get(disptime.G_month()-1)->tostring(monthstring,0);

		FormatString(content,_qstr("^1 "),disptime.G_day());
		content+=_TRL(monthstring);

		render_exec_string(rc,content);
	}
	catch(QError &)
	{
		throw QError(_text("Error while constructing calendar info"));
	}
}


void T3DObjectClock::render_exec_date(Trendercontext *rc, TSC_time &disptime, double size)
{
	QString content;
	disptime.tostring(content,2);
	render_exec_string(rc,content);
}

void T3DObjectClock::render_exec_datetime(Trendercontext *rc, TSC_time &disptime, double size)
{
	QString content;
	disptime.tostring(content,0);
	render_exec_string(rc,content);
}

void T3DObjectClock::render_exec_string(Trendercontext *rc, StrPtr content)
{
	Tvector textdir1,textdir2;
	textdir1.mulfrom(axis1,size->G_val());
	textdir2.mulfrom(axis2,size->G_val());
	rc->rendertext(DEFAULT_FONT,&Tvertex(0,0,0),&textdir1,&textdir2,content,0);
}


void T3DObjectClock::paramchanged(StrPtr iname)
{
}
