#include "stdafx.h"

#include "tools.h"

#include "3Dmesh.h"

#include "CSG_line.h"

#include "CSG_face.h"

#include "CSG_segment.h"

#include "CSG_shape.h"


#include "SC_func.h"
#include "SC_env.h"
#include "SC_script.h"



#include "blob.h"

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <list>
#include <sys/types.h>


#include "ImplicitPolygonizer.h"


//////////////////////////////////////////////////////
// TBlobComp
//////////////////////////////////////////////////////

TBlobComp::TBlobComp()
{
	tpe=0;
	R=1;
	Rinfl=2;
	w=1;
	x0=0;y0=0;z0=0;
	A=0;
}

void TBlobComp::copyfrom(const TBlobComp &icmp)
{
	tpe=icmp.tpe;
	R=icmp.R;
	Rinfl=icmp.Rinfl;
	w=icmp.w;
	A=icmp.A;
	x0=icmp.x0;
	y0=icmp.y0;
	z0=icmp.z0;
}

void TBlobComp::makesphere(double i_x0, double i_y0, double i_z0, double i_R, double i_Rinfl, double i_w)
{
	if (R<1.0e-9) throw QError(_text("Invalid blob component"));
	if (Rinfl<=R) throw QError(_text("Invalid blob component"));
	tpe=0;
	x0=i_x0;y0=i_y0;z0=i_z0;
	R=i_R;Rinfl=i_Rinfl;
	w=i_w;
	A=R/Rinfl;A=1-A*A;A=1/(A*A*A*A)*w;
}

void TBlobComp::makesegment(double i_x0, double i_y0, double i_z0, double i_x1, double i_y1, double i_z1, double i_R, double i_Rinfl, double i_w)
{
	if (R<1.0e-9) throw QError(_text("Invalid blob component"));
	if (Rinfl<=R) throw QError(_text("Invalid blob component"));
	tpe=1;
	x0=i_x0;y0=i_y0;z0=i_z0;
	xd=i_x1-i_x0;yd=i_y1-i_y0;zd=i_z1-i_z0;
	R=i_R;Rinfl=i_Rinfl;
	w=i_w;
	A=R/Rinfl;A=1-A*A;A=1/(A*A*A*A)*w;

	M=sqrt(xd*xd+yd*yd+zd*zd);
	xd/=M;yd/=M;zd/=M;
}


void TBlobComp::maketorus(double i_x0, double i_y0, double i_z0, double i_nx, double i_ny, double i_nz, double i_torusradius, double i_R, double i_Rinfl, double i_w)
{
	if (R<1.0e-9) throw QError(_text("Invalid blob component"));
	if (Rinfl<=R) throw QError(_text("Invalid blob component"));
	tpe=2;
	x0=i_x0;y0=i_y0;z0=i_z0;
	xn=i_nx;yn=i_ny;zn=i_nz;
	torusradius=i_torusradius;
	R=i_R;Rinfl=i_Rinfl;
	w=i_w;
	A=R/Rinfl;A=1-A*A;A=1/(A*A*A*A)*w;
}

void TBlobComp::makedisc(double i_x0, double i_y0, double i_z0, double i_nx, double i_ny, double i_nz, double i_dradius, double i_R, double i_Rinfl, double i_w)
{
	if (R<1.0e-9) throw QError(_text("Invalid blob component"));
	if (Rinfl<=R) throw QError(_text("Invalid blob component"));
	tpe=3;
	x0=i_x0;y0=i_y0;z0=i_z0;
	xn=i_nx;yn=i_ny;zn=i_nz;
	torusradius=i_dradius;
	R=i_R;Rinfl=i_Rinfl;
	w=i_w;
	A=R/Rinfl;A=1-A*A;A=1/(A*A*A*A)*w;
}


void TBlobComp::G_boundingbox(Tvertex &minpt, Tvertex &maxpt) const
{
	if (tpe==0)
	{
		minpt.Set3(x0-Rinfl,y0-Rinfl,z0-Rinfl);
		maxpt.Set3(x0+Rinfl,y0+Rinfl,z0+Rinfl);
		return;
	}
	if (tpe==1)
	{
		minpt.Set3(min(x0,x0+M*xd)-Rinfl,min(y0,y0+M*yd)-Rinfl,min(z0,z0+M*zd)-Rinfl);
		maxpt.Set3(max(x0,x0+M*xd)+Rinfl,max(y0,y0+M*yd)+Rinfl,max(z0,z0+M*zd)+Rinfl);
		return;
	}
	if (tpe==2)
	{
		minpt.Set3(x0-torusradius-Rinfl,y0-torusradius-Rinfl,z0-torusradius-Rinfl);
		maxpt.Set3(x0+torusradius+Rinfl,y0+torusradius+Rinfl,z0+torusradius+Rinfl);
		return;
	}
	if (tpe==3)
	{
		minpt.Set3(x0-torusradius-Rinfl,y0-torusradius-Rinfl,z0-torusradius-Rinfl);
		maxpt.Set3(x0+torusradius+Rinfl,y0+torusradius+Rinfl,z0+torusradius+Rinfl);
		return;
	}
	ASSERT(false);throw QError(_text("Undefined blob component"));
}

double TBlobComp::eval(double x, double y, double z) const
{
	if (tpe==0)
	{
		double dx=x-x0;
		double dy=y-y0;
		double dz=z-z0;
		double q=(dx*dx+dy*dy+dz*dz)/(Rinfl*Rinfl);
		q=1-q;
		if (q<0) q=0;
		return A*q*q*q*q;
	}

	if (tpe==1)
	{
		double t=(x-x0)*xd+(y-y0)*yd+(z-z0)*zd;
		if (t<0) t=0;
		if (t>M) t=M;

		double dx=x-(x0+t*xd);
		double dy=y-(y0+t*yd);
		double dz=z-(z0+t*zd);
		double q=(dx*dx+dy*dy+dz*dz)/(Rinfl*Rinfl);
		q=1-q;
		if (q<0) q=0;
		return A*q*q*q*q;
	}

	if (tpe==2)
	{
		double dx=x-x0;
		double dy=y-y0;
		double dz=z-z0;

		double t=(x-x0)*xn+(y-y0)*yn+(z-z0)*zn;
		dx-=t*xn;
		dy-=t*yn;
		dz-=t*zn;

		double sz=sqrt(dx*dx+dy*dy+dz*dz);
		dx=dx/sz*torusradius;
		dy=dy/sz*torusradius;
		dz=dz/sz*torusradius;

		dx=x-(x0+dx);
		dy=y-(y0+dy);
		dz=z-(z0+dz);

		double q=(dx*dx+dy*dy+dz*dz)/(Rinfl*Rinfl);
		q=1-q;
		if (q<0) q=0;
		return A*q*q*q*q;
	}


	if (tpe==3)
	{
		double dx=x-x0;
		double dy=y-y0;
		double dz=z-z0;

		double t=(x-x0)*xn+(y-y0)*yn+(z-z0)*zn;
		dx-=t*xn;
		dy-=t*yn;
		dz-=t*zn;

		double sz=sqrt(dx*dx+dy*dy+dz*dz);
		double sz2=sz;
		if (sz2>torusradius) sz2=torusradius;
		if (sz>0)
		{
			dx=dx/sz*sz2;
			dy=dy/sz*sz2;
			dz=dz/sz*sz2;
		}

		dx=x-(x0+dx);
		dy=y-(y0+dy);
		dz=z-(z0+dz);

		double q=(dx*dx+dy*dy+dz*dz)/(Rinfl*Rinfl);
		q=1-q;
		if (q<0) q=0;
		return A*q*q*q*q;
	}


	ASSERT(false);throw QError(_text("Undefined blob component"));
}



//////////////////////////////////////////////////////
// TBlob
//////////////////////////////////////////////////////


TBlob::TBlob()
{
}

TBlob::TBlob(const TBlob &iblob)
{
	copyfrom(iblob);
}

TBlob::~TBlob()
{
}

void TBlob::operator=(const TBlob &v)
{
	copyfrom(v);
}

void TBlob::tostring(QString &str, int tpe)
{
}

void TBlob::fromstring(StrPtr str)
{
	ASSERT(false);
}

void TBlob::reset()
{
	comps.reset();
}

TBlobComp* TBlob::addcomp()
{
	TBlobComp *cmp=new TBlobComp;
	comps.add(cmp);
	return cmp;
}


void TBlob::streamout(QBinTagWriter &writer)
{
	throw QError(_text("Not implemented"));
}

void TBlob::streamin(QBinTagReader &reader)
{
	throw QError(_text("Not implemented"));
}


void TBlob::copyfrom(const TBlob &iblob)
{
	reset();
	for (int i=0; i<iblob.comps.G_count(); i++)
	{
		TBlobComp *cmp=new TBlobComp;
		comps.add(cmp);
		cmp->copyfrom(*iblob.comps[i]);
	}
}

void TBlob::G_boundingbox(Tvertex &minpt, Tvertex &maxpt) const
{
	minpt.Set3(1.0e9,1.0e9,1.0e9);
	maxpt.Set3(-1.0e9,-1.0e9,-1.0e9);
	for (int i=0; i<comps.G_count(); i++)
	{
		Tvertex min2,max2;
		comps[i]->G_boundingbox(min2,max2);
		if (minpt.G3_x()>min2.G3_x()) minpt.S3_x(min2.G3_x());
		if (minpt.G3_y()>min2.G3_y()) minpt.S3_y(min2.G3_y());
		if (minpt.G3_z()>min2.G3_z()) minpt.S3_z(min2.G3_z());
		if (maxpt.G3_x()<max2.G3_x()) maxpt.S3_x(max2.G3_x());
		if (maxpt.G3_y()<max2.G3_y()) maxpt.S3_y(max2.G3_y());
		if (maxpt.G3_z()<max2.G3_z()) maxpt.S3_z(max2.G3_z());
	}
}

double TBlob::eval(double x, double y, double z) const
{
	double rs=0;
	for (int i=0; i<comps.G_count(); i++)
		rs+=comps[i]->eval(x,y,z);
	return rs-1.0;
}




class ImplicitFunctionFunctor: public ImplicitPolygonizer::ImplicitFunction
{
private:
	int tpe;//0=vertex as argument  1=x,y,z as arguments
	TSC_functor *functor;
	TSC_funcarglist arglist;
	TSC_value retval0,arg1,arg2,arg3;
public:

	ImplicitFunctionFunctor(TSC_functor *ifunctor) : arglist(NULL)
	{
		functor=ifunctor;
		if (functor->G_argcount()==3)
		{
			tpe=1;
			arg1.createtype(GetTSCenv().G_datatype(SC_valname_scalar));arglist.add(&arg1);
			arg2.createtype(GetTSCenv().G_datatype(SC_valname_scalar));arglist.add(&arg2);
			arg3.createtype(GetTSCenv().G_datatype(SC_valname_scalar));arglist.add(&arg3);
		}
		else
		{
			tpe=0;
			arg1.createtype(GetTSCenv().G_datatype(SC_valname_vertex));arglist.add(&arg1);
		}
	}

	float eval (float x, float y, float z)
	{
		try{
			if (tpe==1)
			{
				arg1.G_content_scalar()->copyfrom(x);
				arg2.G_content_scalar()->copyfrom(y);
				arg3.G_content_scalar()->copyfrom(z);
				functor->eval(&arglist,&retval0);
				return retval0.G_content_scalar()->G_val();
			}
			if (tpe==0)
			{
				*(G_valuecontent<Tvertex>(&arg1))=Tvertex(x,y,z);
				functor->eval(&arglist,&retval0);
				return retval0.G_content_scalar()->G_val();
			}
			throw QError(_text("Unknown equation type"));
		}
		catch(TSC_runerror &err)
		{
			QString errstring;
			FormatString(errstring,_text("Surface equation functor: ^1"),err.G_content());
			throw QError(errstring);
		}
		catch(QError &err)
		{
			QString errstring;
			FormatString(errstring,_text("Surface equation functor: ^1"),err.G_content());
			throw QError(errstring);
		}
	}
};



class ImplicitFunctionBlob: public ImplicitPolygonizer::ImplicitFunction
{
private:
	const TBlob &blob;
public:

	ImplicitFunctionBlob(const TBlob &iblob) : blob(iblob)
	{
	}

	float eval (float x, float y, float z)
	{
		return blob.eval(x,y,z);
	}
};




void CSGShape::create_surface(TSC_functor *functor, double gridsize, double totalsize, Tvertex *ipt, bool dotet)
{
	ImplicitFunctionFunctor fnc(functor);

	double sx=0.005;
	double sy=0.006;
	double sz=0.007;
	if (ipt!=NULL)
	{
		sx=ipt->G3_x();
		sy=ipt->G3_y();
		sz=ipt->G3_z();
	}

	try{
		ImplicitPolygonizer::Polygonizer pol(&fnc, gridsize,(int)(1+totalsize/gridsize));
		pol.march(dotet, sx,sy,sz);

		Tintarray vidx;
		for (int i=0; i<pol.no_vertices(); i++)
		{
			CSGVertex *v=addvertex(CSGVertex(pol.get_vertex(i).x,pol.get_vertex(i).y,pol.get_vertex(i).z));
			v->normal=CSGVector(-pol.get_normal(i).x,-pol.get_normal(i).y,-pol.get_normal(i).z);
			vidx.add(v->idx);
		}

		for (int i=0; i<pol.no_triangles(); i++)
		{
			int i1=vidx[pol.get_triangle(i).v0];
			int i2=vidx[pol.get_triangle(i).v1];
			int i3=vidx[pol.get_triangle(i).v2];
			const CSGVertex &v1=G_vertex(i1);
			const CSGVertex &v2=G_vertex(i2);
			const CSGVertex &v3=G_vertex(i3);
			CSGVector d1(v2.x-v1.x,v2.y-v1.y,v2.z-v1.z);
			CSGVector d2(v3.x-v1.x,v3.y-v1.y,v3.z-v1.z);
			CSGVector n;n.vecprod(d1,d2);
			if (CSGVector::dotprod(n,G_vertex(i1).normal)>=0) addFace(i1,i2,i3);
			else
				addFace(i1,i3,i2);
		}

	}
	catch(string &st)
	{
		throw QError(_text("Surface equation polygonizer error"));
	}

}


void CSGShape::create_blob(const TBlob &blob, double gridsize)
{
	ImplicitFunctionBlob fnc(blob);

	Tvertex minpt,maxpt;
	blob.G_boundingbox(minpt,maxpt);
	double centx=(minpt.G3_x()+maxpt.G3_x())/2.0;
	double centy=(minpt.G3_y()+maxpt.G3_y())/2.0;
	double centz=(minpt.G3_z()+maxpt.G3_z())/2.0;

	double sizex=maxpt.G3_x()-minpt.G3_x();
	double sizey=maxpt.G3_y()-minpt.G3_y();
	double sizez=maxpt.G3_z()-minpt.G3_z();
	double totalsize=sizex;
	if (totalsize<sizey) totalsize=sizey;
	if (totalsize<sizez) totalsize=sizez;

	try{
		ImplicitPolygonizer::Polygonizer pol(&fnc, gridsize,(int)(1+totalsize/gridsize));
		pol.march(false, centx,centy,centz);//--!!--true??

		Tintarray vidx;
		for (int i=0; i<pol.no_vertices(); i++)
		{
			CSGVertex *v=addvertex(CSGVertex(pol.get_vertex(i).x,pol.get_vertex(i).y,pol.get_vertex(i).z));
			v->normal=CSGVector(-pol.get_normal(i).x,-pol.get_normal(i).y,-pol.get_normal(i).z);
			vidx.add(v->idx);
		}

		for (int i=0; i<pol.no_triangles(); i++)
		{
			int i1=vidx[pol.get_triangle(i).v0];
			int i2=vidx[pol.get_triangle(i).v1];
			int i3=vidx[pol.get_triangle(i).v2];
			const CSGVertex &v1=G_vertex(i1);
			const CSGVertex &v2=G_vertex(i2);
			const CSGVertex &v3=G_vertex(i3);
			CSGVector d1(v2.x-v1.x,v2.y-v1.y,v2.z-v1.z);
			CSGVector d2(v3.x-v1.x,v3.y-v1.y,v3.z-v1.z);
			CSGVector n;n.vecprod(d1,d2);
			if (CSGVector::dotprod(n,G_vertex(i1).normal)>=0) addFace(i1,i2,i3);
			else
				addFace(i1,i3,i2);
		}

	}
	catch(string &st)
	{
		throw QError(_text("Surface equation polygonizer error"));
	}
}






///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_create_surfaceequation,CreateSurface)
{
	setclasspath_fromtype(SC_valname_shape);
	setreturntype(SC_valname_shape);
	addvar(_qstr("equation"),SC_valname_functor);
	addvar(_qstr("gridsize"),SC_valname_scalar);
	addvar(_qstr("boundingbox"),SC_valname_scalar);
	addvar(_qstr("startpoint"),SC_valname_vertex,false);
	addvar(_qstr("tetrahedral"),SC_valname_boolean,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(retval);
	TSC_functor *functor=G_valuecontent<TSC_functor>(arglist->get(0));
	double gridsize=arglist->get(1)->G_content_scalar()->G_val();
	double totsize=arglist->get(2)->G_content_scalar()->G_val();
	Tvertex *ipt=NULL;
	if (arglist->G_ispresent(3)) ipt=G_valuecontent<Tvertex>(arglist->get(3));
	bool dotet=true;
	if (arglist->G_ispresent(4)) dotet=arglist->get(4)->G_content_boolean()->G_val();
	shape->create_surface(functor,gridsize,totsize,ipt,dotet);
}
ENDFUNCTION(func_CSG_create_surfaceequation)




///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_blob_addsphere,AddSphere)
{
	setmemberfunction(SC_valname_blob);

	addvar(_qstr("Position"),SC_valname_vertex);
	addvar(_qstr("Radius"),SC_valname_scalar);
	addvar(_qstr("InfluenceRadius"),SC_valname_scalar);
	addvar(_qstr("Weight"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TBlob *blob=G_valuecontent<TBlob>(owner);
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	double radius=arglist->get(1)->G_content_scalar()->G_val();
	double inflradius=arglist->get(2)->G_content_scalar()->G_val();

	double w=1.0;
	if (arglist->G_ispresent(3)) w=arglist->get(3)->G_content_scalar()->G_val();

	blob->addcomp()->makesphere(pt->G3_x(),pt->G3_y(),pt->G3_z(),radius,inflradius,w);
}
ENDFUNCTION(func_blob_addsphere)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_blob_addsegment,AddSegment)
{
	setmemberfunction(SC_valname_blob);

	addvar(_qstr("Start"),SC_valname_vertex);
	addvar(_qstr("Stop"),SC_valname_vertex);
	addvar(_qstr("Radius"),SC_valname_scalar);
	addvar(_qstr("InfluenceRadius"),SC_valname_scalar);
	addvar(_qstr("Weight"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TBlob *blob=G_valuecontent<TBlob>(owner);
	Tvertex *pt1=G_valuecontent<Tvertex>(arglist->get(0));
	Tvertex *pt2=G_valuecontent<Tvertex>(arglist->get(1));
	double radius=arglist->get(2)->G_content_scalar()->G_val();
	double inflradius=arglist->get(3)->G_content_scalar()->G_val();

	double w=1.0;
	if (arglist->G_ispresent(3)) w=arglist->get(3)->G_content_scalar()->G_val();

	blob->addcomp()->makesegment(pt1->G3_x(),pt1->G3_y(),pt1->G3_z(),pt2->G3_x(),pt2->G3_y(),pt2->G3_z(),radius,inflradius,w);
}
ENDFUNCTION(func_blob_addsegment)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_blob_addtorus,AddTorus)
{
	setmemberfunction(SC_valname_blob);

	addvar(_qstr("Position"),SC_valname_vertex);
	addvar(_qstr("Normal"),SC_valname_vector);
	addvar(_qstr("TorusRadius"),SC_valname_scalar);
	addvar(_qstr("Radius"),SC_valname_scalar);
	addvar(_qstr("InfluenceRadius"),SC_valname_scalar);
	addvar(_qstr("Weight"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TBlob *blob=G_valuecontent<TBlob>(owner);
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *nm=G_valuecontent<Tvector>(arglist->get(1));
	double torusradius=arglist->get(2)->G_content_scalar()->G_val();
	double radius=arglist->get(3)->G_content_scalar()->G_val();
	double inflradius=arglist->get(4)->G_content_scalar()->G_val();

	double w=1.0;
	if (arglist->G_ispresent(5)) w=arglist->get(5)->G_content_scalar()->G_val();

	blob->addcomp()->maketorus(pt->G3_x(),pt->G3_y(),pt->G3_z(),nm->G_x(),nm->G_y(),nm->G_z(),torusradius,radius,inflradius,w);
}
ENDFUNCTION(func_blob_addtorus)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_blob_adddisc,AddDisc)
{
	setmemberfunction(SC_valname_blob);

	addvar(_qstr("Position"),SC_valname_vertex);
	addvar(_qstr("Normal"),SC_valname_vector);
	addvar(_qstr("DiscRadius"),SC_valname_scalar);
	addvar(_qstr("Radius"),SC_valname_scalar);
	addvar(_qstr("InfluenceRadius"),SC_valname_scalar);
	addvar(_qstr("Weight"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TBlob *blob=G_valuecontent<TBlob>(owner);
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *nm=G_valuecontent<Tvector>(arglist->get(1));
	double discradius=arglist->get(2)->G_content_scalar()->G_val();
	double radius=arglist->get(3)->G_content_scalar()->G_val();
	double inflradius=arglist->get(4)->G_content_scalar()->G_val();

	double w=1.0;
	if (arglist->G_ispresent(5)) w=arglist->get(5)->G_content_scalar()->G_val();

	blob->addcomp()->makedisc(pt->G3_x(),pt->G3_y(),pt->G3_z(),nm->G_x(),nm->G_y(),nm->G_z(),discradius,radius,inflradius,w);
}
ENDFUNCTION(func_blob_adddisc)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_blob_createshape,CreateSolidShape)
{
	setmemberfunction(SC_valname_blob);
	setreturntype(SC_valname_shape);

	addvar(_qstr("GridSize"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TBlob *blob=G_valuecontent<TBlob>(owner);
	CSGShape *shape=G_valuecontent<CSGShape>(retval);
	double gridsize=arglist->get(0)->G_content_scalar()->G_val();

	shape->create_blob(*blob,gridsize);
}
ENDFUNCTION(func_blob_createshape)
