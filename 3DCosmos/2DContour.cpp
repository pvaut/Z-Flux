#include "stdafx.h"
#include "vecmath.h"

#include "SC_func.h"
#include "SC_env.h"
#include "SC_script.h"
#include "SC_expr.h"
#include "SC_functor.h"

#include "2DContour.h"


//****************************************************************
// T2DContour
//****************************************************************

T2DContour::T2DContour()
{
	closed=false;
}


void T2DContour::addpoint(int PTID, Tvertex *pt, Tvector *nm)
{
	if (closed) throw QError(_text("Unable to add points to an already closed contour"));
	T2DContourPoint cpt;
	cpt.pt=*pt;
	cpt.PTID=PTID;
	if (nm!=NULL)
	{
		cpt.normalpresent=true;
		cpt.nm=*nm;
	}
	else cpt.normalpresent=false;
	points.add(cpt);
}

void T2DContour::calcdims()
{
	double cx=0,cy=0;
	int n=0;
	maxx=-1.0e99;minx=-maxx;
	miny=minx;maxy=maxx;
	for (int i=0; i<points.G_count(); i++)
	{
		maxx=max(maxx,points[i].pt.G3_x());
		minx=min(minx,points[i].pt.G3_x());
		maxy=max(maxy,points[i].pt.G3_y());
		miny=min(miny,points[i].pt.G3_y());
		cx+=points[i].pt.G3_x();
		cy+=points[i].pt.G3_y();
		n++;
	}
	if (n<=0) return;
	center.Set3(cx/n,cy/n,0);
}


void T2DContour::close()
{
	if (closed) return;
	T2DContourPoint cpt;
	cpt.pt=points[0].pt;
	cpt.PTID=points[0].PTID;
	if (points[0].normalpresent)
	{
		cpt.normalpresent=true;
		cpt.nm=points[0].nm;
	}
	else cpt.normalpresent=false;
	points.add(cpt);
	closed=true;
}


void T2DContour::calcflatautonormals()
{
	double dx,dy;
	Tvector nm;
	calcdims();
	for (int i=1; i<points.G_count(); i++)
	{
		dx=points[i].pt.G3_x()-points[i-1].pt.G3_x();
		dy=points[i].pt.G3_y()-points[i-1].pt.G3_y();
		if ((fabs(dx)>=(maxx-minx)/10000)||(fabs(dy)>=(maxy-miny)/10000))
		{
			nm.Set(dy,-dx,0);nm.normfrom(&nm);
			if (!points[i-1].normalpresent)
			{
				points[i-1].normalpresent=true;
				points[i-1].nm=nm;
			}
			if (!points[i].normalpresent)
			{
				points[i].normalpresent=true;
				points[i].nm=nm;
				if (i<points.G_count()-1)
				{
					T2DContourPoint cpt;
					cpt.pt=points[i].pt;cpt.PTID=points[i].PTID;
					cpt.normalpresent=false;
					points.insertbefore(i+1,cpt);
					i++;
				}
			}
		}
	}
}


void T2DContour::calccirctexcoord()
{
	if (points.G_count()==0) return;

	calcdims();
	double size=max(maxx-minx,maxy-miny);

	//calculate circular texture coordinates
	double incrlen,totlen=0;
	for (int i=1; i<=points.G_count(); i++) totlen+=Tvertex::distance(&points[i%points.G_count()].pt,&points[i-1].pt);
	if (totlen>0)
	{
		points[0].texcoord=0;
		incrlen=0;
		for (int i=1; i<points.G_count(); i++)
		{
			incrlen+=Tvertex::distance(&points[i].pt,&points[i-1].pt);
			points[i].texcoord=incrlen/totlen;
		}
	}

}

void T2DContour::operator=(const T2DContour &v)
{
	points.reset();
	T2DContourPoint pt;
	for (int i=0; i<v.points.G_count(); i++)
	{
		pt=v.points[i];
		points.add(pt);
	}
}



//****************************************************************
// T2DContourset
//****************************************************************

T2DContourset::T2DContourset()
{
	reset();
}

T2DContourset::~T2DContourset()
{
}

void T2DContourset::operator=(const T2DContourset &v)
{
	contours.reset();
	for (int i=0; i<v.contours.G_count(); i++)
	{
		T2DContour *cnt=new T2DContour;
		contours.add(cnt);
		*cnt=*v.contours[i];
	}
	curcontour=v.curcontour;
	curPTID=v.curPTID;
}


void T2DContourset::reset()
{
	contours.reset();
	contours.add(new T2DContour);
	curcontour=contours.G_count()-1;
	curPTID=0;
}


void T2DContourset::newcontour()
{
	contours.add(new T2DContour);
	curcontour=contours.G_count()-1;
}

void T2DContourset::addpoint(Tvertex *pt, Tvector *nm)
{
	contours[curcontour]->addpoint(curPTID,pt,nm);
	curPTID++;
}


void T2DContourset::generate(TSC_functor *functor, double minv, double maxv, int count)
{
	if (count<2) throw QError(_qstr("Generate contour: invalid number of points"));

	try{
		double vl,vl1,vl2,dff;
		Tvertex pt,pt1,pt2;
		Tvector tangent,norm;
		TSC_funcarglist arglist(NULL);
		TSC_value retval0,arg;
		arg.createtype(GetTSCenv().G_datatype(SC_valname_scalar));
		arglist.add(&arg);
		dff=(maxv-minv)/count/50;
		for (int i=0; i<count; i++)
		{
			vl=minv+(maxv-minv)*i/(count-1.0);
			vl1=vl-dff/2;
			vl2=vl+dff/2;
			arg.copyfrom(vl);functor->eval(&arglist,&retval0);pt=*G_valuecontent<Tvertex>(&retval0);
			arg.copyfrom(vl1);functor->eval(&arglist,&retval0);pt1=*G_valuecontent<Tvertex>(&retval0);
			arg.copyfrom(vl2);functor->eval(&arglist,&retval0);pt2=*G_valuecontent<Tvertex>(&retval0);
			tangent.subtr(&pt2,&pt1);
			norm.vecprod(&tangent,&Tvector(0,0,1));norm.normfrom(&norm);
			addpoint(&pt,&norm);
		}
	}
	catch(TSC_runerror err)
	{
		QString errstring;
		FormatString(errstring,_text("Generate contour: ^1"),err.G_content());
		throw QError(errstring);
	}
	catch(QError err)
	{
		QString errstring;
		FormatString(errstring,_text("Generate contour: ^1"),err.G_content());
		throw QError(errstring);
	}
}




void T2DContourset::tostring(QString &str, int tpe)
{
	str=description;
}

void T2DContourset::fromstring(StrPtr str)
{
	ASSERT(false);
}



void T2DContourset::createstring(QString &str)
{
}

void T2DContourset::streamout(QBinTagWriter &writer)
{
	ASSERT(false);
}
void T2DContourset::streamin(QBinTagReader &reader)
{
	ASSERT(false);
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_2dcontourset_create,Contour)
{
	setclasspath_fromtype(SC_valname_2dcontourset);
	addvar(_qstr("pointlist"),SC_valname_list);
	setreturntype(SC_valname_2dcontourset);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T2DContourset *contour=G_valuecontent<T2DContourset>(retval);
	contour->reset();

	TSC_list *pointlist=G_valuecontent<TSC_list>(arglist->get(0));
	for (int i=0; i<pointlist->G_size(); i++)
	{
		Tvertex *point=G_valuecontent<Tvertex>(pointlist->get(i));
		contour->addpoint(point);
	}
	contour->G_curcontour()->close();
	contour->G_curcontour()->calcflatautonormals();

/*	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *nm=NULL;
	if (arglist->G_ispresent(1)) nm=G_valuecontent<Tvector>(arglist->get(1));
	G_valuecontent<T2DContourset>(owner)->addpoint(pt,nm);*/
}
ENDFUNCTION(func_2dcontourset_create)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_2dcontourset_addpoint,addpoint)
{
	setmemberfunction(SC_valname_2dcontourset);
	addvar(_qstr("point"),SC_valname_vertex);
	addvar(_qstr("normal"),SC_valname_vector,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *nm=NULL;
	if (arglist->G_ispresent(1)) nm=G_valuecontent<Tvector>(arglist->get(1));
	G_valuecontent<T2DContourset>(owner)->addpoint(pt,nm);
}
ENDFUNCTION(func_2dcontourset_addpoint)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_2dcontourset_close,close)
{
	setmemberfunction(SC_valname_2dcontourset);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<T2DContourset>(owner)->G_curcontour()->close();
}
ENDFUNCTION(func_2dcontourset_close)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_2dcontourset_calcflatnormals,calcflatnormals)
{
	setmemberfunction(SC_valname_2dcontourset);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<T2DContourset>(owner)->G_curcontour()->calcflatautonormals();
}
ENDFUNCTION(func_2dcontourset_calcflatnormals)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_2dcontourset_generate,generate)
{
	setmemberfunction(SC_valname_2dcontourset);
	addvar(_qstr("function"),SC_valname_functor);
	addvar(_qstr("min"),SC_valname_scalar);
	addvar(_qstr("max"),SC_valname_scalar);
	addvar(_qstr("count"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T2DContourset *contourset=G_valuecontent<T2DContourset>(owner);
	TSC_functor *functor=G_valuecontent<TSC_functor>(arglist->get(0));
	double min1=arglist->get(1)->G_content_scalar()->G_val();
	double max1=arglist->get(2)->G_content_scalar()->G_val();
	int count1=arglist->get(3)->G_content_scalar()->G_intval();
	contourset->generate(functor,min1,max1,count1);
}
ENDFUNCTION(func_2dcontourset_generate)





///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_2dcontourset_newcontour,newcontour)
{
	setmemberfunction(SC_valname_2dcontourset);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<T2DContourset>(owner)->newcontour();
}
ENDFUNCTION(func_2dcontourset_newcontour)
