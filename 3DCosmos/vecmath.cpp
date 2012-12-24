#include "stdafx.h"
#include "vecmath.h"
#include "matrix.h"

#include "SC_func.h"
#include "SC_env.h"

#include "3dscene.h"

#include "CSG_components.h"

/////////////////////////////////////////////////////////////////
// Thomogcoord
/////////////////////////////////////////////////////////////////



void Thomogcoord::add(const Thomogcoord *v1, const Thomogcoord *v2)
{
	coord[0]=v1->coord[0]+v2->coord[0];
	coord[1]=v1->coord[1]+v2->coord[1];
	coord[2]=v1->coord[2]+v2->coord[2];
	coord[3]=v1->coord[3]+v2->coord[3];
}

void Thomogcoord::subtr(const Thomogcoord *v1, const Thomogcoord *v2)
{
	coord[0]=v1->coord[0]-v2->coord[0];
	coord[1]=v1->coord[1]-v2->coord[1];
	coord[2]=v1->coord[2]-v2->coord[2];
	coord[3]=v1->coord[3]-v2->coord[3];
}

void Thomogcoord::mulfrom(const Thomogcoord *v1, double vl)
{
	coord[0]=vl*v1->coord[0];
	coord[1]=vl*v1->coord[1];
	coord[2]=vl*v1->coord[2];
	coord[3]=vl*v1->coord[3];
}


void Thomogcoord::mul(double vl)
{
	coord[0]*=vl;
	coord[1]*=vl;
	coord[2]*=vl;
	coord[3]*=vl;
}


void Thomogcoord::mulfrom(const TMatrix *mt, const Thomogcoord *v)
{
	mt->mul(v,this);
}

void Thomogcoord::lincombfrom(double f1, const Thomogcoord *v1, double f2, const Thomogcoord *v2)
{
	coord[0]=f1*v1->coord[0]+f2*v2->coord[0];
	coord[1]=f1*v1->coord[1]+f2*v2->coord[1];
	coord[2]=f1*v1->coord[2]+f2*v2->coord[2];
	coord[3]=f1*v1->coord[3]+f2*v2->coord[3];
}

double Thomogcoord::dot4(const Thomogcoord *v1, const Thomogcoord *v2)
{
	return v1->coord[0]*v2->coord[0]+v1->coord[1]*v2->coord[1]+v1->coord[2]*v2->coord[2]+v1->coord[3]*v2->coord[3];
}


void Thomogcoord::streamout(QBinTagWriter &writer)
{
	for (int i=0; i<4; i++) writer.write_double(coord[i]);
}

void Thomogcoord::streamin(QBinTagReader &reader)
{
	for (int i=0; i<4; i++) coord[i]=reader.read_double();
}


/////////////////////////////////////////////////////////////////
// Tvertex
/////////////////////////////////////////////////////////////////

Tvertex::Tvertex()
{
	coord[0]=0;coord[1]=0;coord[2]=0;coord[3]=1;
}

Tvertex::Tvertex(Tvertex &v)
{
	coord[0]=v.coord[0];coord[1]=v.coord[1];coord[2]=v.coord[2];coord[3]=v.coord[3];
}

Tvertex::Tvertex(double x, double y, double z, double u)
{
	coord[0]=x;coord[1]=y;coord[2]=z;coord[3]=u;
}


void Tvertex::tostring(QString &str, int tpe)
{
	if (fabs(coord[3])<1e-19)
	{
		str=_qstr("(");
		QString st1;
		st1.formatscalar(G4_x());str+=st1;str+=_qstr(", ");
		st1.formatscalar(G4_y());str+=st1;str+=_qstr(", ");
		st1.formatscalar(G4_z());str+=st1;
		str+=_qstr(")");
	}
	else
	{
		str=_qstr("(");
		QString st1;
		st1.formatscalar(G3_x());str+=st1;str+=_qstr(", ");
		st1.formatscalar(G3_y());str+=st1;str+=_qstr(", ");
		st1.formatscalar(G3_z());str+=st1;
		str+=_qstr(")");
	}
}

void Tvertex::fromstring(StrPtr str)
{
	QString st=str,st1;
	st.splitstring(_qstr("("),st1);
	st.splitstring(_qstr(","),st1);coord[0]=qstr2double(st1);
	st.splitstring(_qstr(","),st1);coord[1]=qstr2double(st1);
	st.splitstring(_qstr(","),st1);coord[2]=qstr2double(st1);
	coord[3]=1;
}

void Tvertex::operator=(const Tvertex &v)
{
	coord[0]=v.coord[0];coord[1]=v.coord[1];coord[2]=v.coord[2];coord[3]=v.coord[3];
}

void Tvertex::copyfrom(const Tvector &v)
{
	coord[0]=v.G_x();coord[1]=v.G_y();coord[2]=v.G_z();coord[3]=1;
}

void Tvertex::copyfrom(const Tvec3d &v)
{
	coord[0]=v.x;coord[1]=v.y;coord[2]=v.z;coord[3]=1;
}


void Tvertex::copyfrom(double x, double y, double z)
{
	coord[0]=x;coord[1]=y;coord[2]=z;coord[3]=1;
}


double Tvertex::distance(const Tvertex *v1, const Tvertex *v2)
{
	double dx=v1->G3_x()-v2->G3_x();
	double dy=v1->G3_y()-v2->G3_y();
	double dz=v1->G3_z()-v2->G3_z();
	return sqrt(dx*dx+dy*dy+dz*dz);
}


/*
void Tvertex::add(const Tvertex *v1, const Tvertex *v2)
{
	coord[0]=v1->coord[0]+v2->coord[0];
	coord[0]=v1->coord[0]+v2->coord[0];
	coord[0]=v1->coord[0]+v2->coord[0];
	coord[0]=v1->coord[0]+v2->coord[0];
}
*/
/*
void Tvertex::add(const Tvertex *v1, const Tvector *v2)
{
	x=v1->x+v2->G_x();
	y=v1->y+v2->G_y();
	z=v1->z+v2->G_z();
	u=v1->u;
}

void Tvertex::subtr(const Tvertex *v1, const Tvector *v2)
{
	x=v1->x-v2->G_x();
	y=v1->y-v2->G_y();
	z=v1->z-v2->G_z();
	u=v1->u;
}


void Tvertex::mulfrom(const Tvertex *v1, double vl)
{
	x=vl*v1->x;
	y=vl*v1->y;
	z=vl*v1->z;
	u=vl*v1->u;
}
*/


/////////////////////////////////////////////////////////////////
// Tvector
/////////////////////////////////////////////////////////////////

Tvector::Tvector()
{
	coord[0]=0;coord[1]=0;coord[2]=0;coord[3]=0;
}

Tvector::Tvector(double x, double y, double z, double u)
{
	coord[0]=x;coord[1]=y;coord[2]=z;coord[3]=u;
}

void Tvector::copyfrom(const Tvec3d &v)
{
	coord[0]=v.x;coord[1]=v.y;coord[2]=v.z;coord[3]=0;
}


void Tvector::copyfrom(double x, double y, double z)
{
	coord[0]=x;coord[1]=y;coord[2]=z;coord[3]=0;
}



Tvector::Tvector(Tvector &v)
{
	coord[0]=v.coord[0];coord[1]=v.coord[1];coord[2]=v.coord[2];
}

void Tvector::tostring(QString &str, int tpe)
{
	str=_qstr("(");
	QString st1;
	st1.formatscalar(G_x());str+=st1;str+=_qstr(", ");
	st1.formatscalar(G_y());str+=st1;str+=_qstr(", ");
	st1.formatscalar(G_z());str+=st1;
	str+=_qstr(")");
}

void Tvector::fromstring(StrPtr str)
{
	QString st=str,st1;
	st.splitstring(_qstr("("),st1);
	st.splitstring(_qstr(","),st1);coord[0]=qstr2double(st1);
	st.splitstring(_qstr(","),st1);coord[1]=qstr2double(st1);
	st.splitstring(_qstr(","),st1);coord[2]=qstr2double(st1);
	coord[3]=0;
}

void Tvector::operator=(const Tvector &v)
{
	coord[0]=v.coord[0];coord[1]=v.coord[1];coord[2]=v.coord[2];
}

void Tvector::copyfrom(const Tvertex &v)
{
	coord[0]=v.G3_x();coord[1]=v.G3_y();coord[2]=v.G3_z();coord[3]=0;
}

void Tvector::copyfrom(const CSGVector &v)
{
	coord[0]=v.x;coord[1]=v.y;coord[2]=v.z;coord[3]=0;
}


double Tvector::G_size() const
{
	return sqrt(G_x()*G_x()+G_y()*G_y()+G_z()*G_z());
}

void Tvector::normfrom(const Tvector *v1)
{
	double sz=v1->G_size();
	if (sz==0)
	{
		coord[0]=0;
		coord[1]=0;
		coord[2]=0;
		return;
	}
	coord[0]=v1->coord[0]/sz;
	coord[1]=v1->coord[1]/sz;
	coord[2]=v1->coord[2]/sz;
}


void Tvector::anynormalfrom(const Tvector *v1)
{
	Tvector tst1;tst1.vecprod(v1,&Tvector(1,0,0));
	Tvector tst2;tst2.vecprod(v1,&Tvector(0,1,0));
	if (tst1.G_size()>tst2.G_size()) normfrom(&tst1);
	else normfrom(&tst2);
}


void Tvector::rotatefrom(const Tvector *vc, const Tvector *norm, double angle)
{
	Tvector n,p,r1,r2;
	n.normfrom(norm);

	p.mulfrom(&n,dotprod(vc,&n));
	r1.subtr(vc,&p);
	r2.vecprod(&n,&r1);

	r1.mulfrom(&r1,cos(angle));
	r2.mulfrom(&r2,sin(angle));

	S_x(r1.G_x() + r2.G_x() + p.G_x());
	S_y(r1.G_y() + r2.G_y() + p.G_y());
	S_z(r1.G_z() + r2.G_z() + p.G_z());
}

void Tvector::projectfrom(const Tvector *vc, const Tvector *normdir)
{
	lincombfrom(1.0,vc,-1.0*Tvector::dotprod(vc,normdir)/normdir->G_size(),normdir);
}


void Tvector::subtrvertices(const Tvertex *v1, const Tvertex *v2)
{
	coord[0]=v1->G3_x()-v2->G3_x();
	coord[1]=v1->G3_y()-v2->G3_y();
	coord[2]=v1->G3_z()-v2->G3_z();
	coord[3]=0;
}

void Tvector::vecprod(const Tvector *v1, const Tvector *v2)
{
	coord[0]=v1->G_y()*v2->G_z() - v1->G_z()*v2->G_y();
	coord[1]=v1->G_z()*v2->G_x() - v1->G_x()*v2->G_z();
	coord[2]=v1->G_x()*v2->G_y() - v1->G_y()*v2->G_x();
	coord[3]=0;
}

double Tvector::dotprod(const Tvector *v1, const Tvector *v2)
{
	return v1->G_x()*v2->G_x()+v1->G_y()*v2->G_y()+v1->G_z()*v2->G_z();
}

double Tvector::angle(const Tvector *v1, const Tvector *v2)
{
	Tvector va,vb,vv;

	va.normfrom(v1);
	vb.normfrom(v2);
	vv.vecprod(&va,&vb);
	double sina=vv.G_size();
	double cosa=dotprod(&va,&vb);
	double ang=atan2(sina,cosa);
	return ang;
}





/////////////////////////////////////////////////////////////////
// Tplane
/////////////////////////////////////////////////////////////////

Tplane::Tplane()
{
	coord[0]=1;coord[1]=0;coord[2]=0;coord[3]=0;
}

Tplane::Tplane(double x, double y, double z, double u)
{
	coord[0]=x;coord[1]=y;coord[2]=z;coord[3]=u;
}

void Tplane::copyfrom(double x, double y, double z, double u)
{
	coord[0]=x;coord[1]=y;coord[2]=z;coord[3]=u;
}

Tplane::~Tplane()
{
	int i=0;
}

void Tplane::Create(const Tvertex &pt, const Tvector &normal)
{
	Tvector nm;
	nm=normal;
	//nm.normfrom(&normal);
	S_x(nm.G_x());S_y(nm.G_y());S_z(nm.G_z());
	double offset=nm.G_x()*pt.G3_x()+nm.G_y()*pt.G3_y()+nm.G_z()*pt.G3_z();
	S_u(-offset);
	normalise();
}

void Tplane::normalise()
{
	double sz=G_x()*G_x()+G_y()*G_y()+G_z()*G_z();
	if (sz<=1.0e-19) throw QError(_text("Invalid plane"));
	mul(1.0/sqrt(sz));
}


void Tplane::Create(const Tvertex &pt1, const Tvertex &pt2, const Tvertex &pt3)
{
	Tvector dr1,dr2,norm;
	dr1.subtrvertices(&pt2,&pt1);
	dr2.subtrvertices(&pt3,&pt1);
	norm.vecprod(&dr1,&dr2);
	if (norm.G_size()<1.0e-19) throw QError(_text("Invalid set of points for creation of a plane"));
	norm.normfrom(&norm);
	Create(pt1,norm);
}


void Tplane::G_normvector(Tvector &normvector) const
{
	normvector.Set(G_x(),G_y(),G_z());
	normvector.normfrom(&normvector);
}

void Tplane::G_anypoint(Tvertex &pt) const
{
	findclosestpoint(Tvertex(0,0,0),pt);
}

void Tplane::Set_normvector(const Tvector &ivc)
{
	Tvertex pt;
	G_anypoint(pt);
	Create(pt,ivc);
}

void Tplane::Set_anypoint(const Tvertex &ipt)
{
	Tvector vc;
	G_normvector(vc);
	Create(ipt,vc);
}



double Tplane::evalpoint(const Tvertex &ipt) const
{
	return Thomogcoord::dot4(this,&ipt);
}


void Tplane::findclosestpoint(const Tvertex &ipt, Tvertex &result) const
{
	double vl=Thomogcoord::dot4(this,&ipt);
	Tvector normvector;G_normvector(normvector);
	result.lincombfrom(1.0,&ipt,-vl,&normvector);
}


void Tplane::tostring(QString &str, int tpe)
{
	str.clear();
	QString st1;
	st1.formatscalar(G_x());str+=st1;str+=_qstr(".X+");
	st1.formatscalar(G_y());str+=st1;str+=_qstr(".Y+");
	st1.formatscalar(G_z());str+=st1;str+=_qstr(".Z+");
	st1.formatscalar(G_u());str+=st1;str+=_qstr("=0");
}

void Tplane::fromstring(StrPtr str)
{
	QString st=str,st1;
	st.splitstring(_qstr(".X+"),st1);coord[0]=qstr2double(st1);
	st.splitstring(_qstr(".Y+"),st1);coord[1]=qstr2double(st1);
	st.splitstring(_qstr(".Z+"),st1);coord[2]=qstr2double(st1);
	st.splitstring(_qstr("=0"),st1);coord[3]=qstr2double(st1);
}

void Tplane::operator=(const Tplane &v)
{
	coord[0]=v.coord[0];coord[1]=v.coord[1];coord[2]=v.coord[2];coord[3]=v.coord[3];
}


void Tplane::intersect(const Tline &line, Tvertex &point) const
{
	Tvector planenormal;
	G_normvector(planenormal);

	double dp=Tvector::dotprod(&planenormal,&line.G_direction());
	if (fabs(dp)<1.0e-19)
		throw QError(_text("Unable to intersect line and plane"));

	double u=-evalpoint(line.G_point())/dp;

	point.lincombfrom(1,&line.G_point(),u,&line.G_direction());
}

bool Tplane::isparallel(const Tline &line) const
{
	Tvector planenormal;
	G_normvector(planenormal);
	double dp=Tvector::dotprod(&planenormal,&line.G_direction());
	return (fabs(dp)<1.0e-19);
}


bool Tplane::isparallel(const Tplane &pl) const
{
	Tvector nm1,nm2,pr;
	G_normvector(nm1);
	pl.G_normvector(nm2);
	pr.vecprod(&nm1,&nm2);
	return (pr.G_size()<1.0e-9);
}



////////////////////////////////////////////////////////////////////////////
// Tline
////////////////////////////////////////////////////////////////////////////

Tline::Tline()
{
	direction=Tvector(1,0,0);
}

Tline::~Tline()
{
}

void Tline::tostring(QString &str, int tpe)
{
	QString st2;
	point.tostring(str,tpe);
	direction.tostring(st2,tpe);
	str+=_qstr(" ");
	str+=st2;
}

void Tline::fromstring(StrPtr str)
{
	ASSERT(false);
	throw QError(_text("Unable to convert a string to a line object"));
}

void Tline::operator=(const Tline &v)
{
	point=v.point;
	direction=v.direction;
}

void Tline::streamout(QBinTagWriter &writer)
{
	point.streamout(writer);
	direction.streamout(writer);
}

void Tline::streamin(QBinTagReader &reader)
{
	point.streamin(reader);
	direction.streamin(reader);
}

void Tline::Create(const Tvertex &p1, const Tvertex &p2)
{
	point=p1;
	direction.subtrvertices(&p2,&p1);
	if (direction.G_size()<1e-19) throw QError(_text("Invalid line construction"));
	direction.normfrom(&direction);
}


void Tline::findclosestpoint(const Tvertex &ipt, Tvertex &result) const
{
	Tvector dr;dr.subtrvertices(&ipt,&point);
	double u=Tvector::dotprod(&dr,&direction);

	Tvector ptt;ptt.lincombfrom(1,&point,u,&direction);
	result.copyfrom(ptt);
}


void Tline::intersect(const Tplane &pl1, const Tplane &pl2)
{
	Tvector n1,n2;
	pl1.G_normvector(n1);
	pl2.G_normvector(n2);
	direction.vecprod(&n1,&n2);
	if (direction.G_size()<1e-19)
	{
		throw QError(_text("Invalid plane intersection"));
	}
	direction.normfrom(&direction);

	double n11=Tvector::dotprod(&n1,&n1);
	double n12=Tvector::dotprod(&n1,&n2);
	double n22=Tvector::dotprod(&n2,&n2);

	double d1=-pl1.G4_u();
	double d2=-pl2.G4_u();

	double det=n11*n22-n12*n12;
	if (fabs(det)<1e-19)
		throw QError(_text("Invalid plane intersection"));

	double c1=(d1*n22-d2*n12)/det;
	double c2=(d2*n11-d1*n12)/det;

	Tvector ptt;ptt.lincombfrom(c1,&n1,c2,&n2);
	point.copyfrom(ptt);

}




////////////////////////////////////////////////////////////////////////////
// TLineSegment
////////////////////////////////////////////////////////////////////////////

TLineSegment::TLineSegment()
{
	pt1=Tvertex(0,0,0);
	pt2=Tvertex(1,0,0);
}

TLineSegment::~TLineSegment()
{
}

void TLineSegment::tostring(QString &str, int tpe)
{
	QString st2;
	pt1.tostring(str,tpe);
	pt2.tostring(st2,tpe);
	str+=_qstr(" ");
	str+=st2;
}

void TLineSegment::fromstring(StrPtr str)
{
	ASSERT(false);
	throw QError(_text("Unable to convert a string to a line segment object"));
}

void TLineSegment::operator=(const TLineSegment &v)
{
	pt1=v.pt1;
	pt2=v.pt2;
}

void TLineSegment::streamout(QBinTagWriter &writer)
{
	pt1.streamout(writer);
	pt2.streamout(writer);
}

void TLineSegment::streamin(QBinTagReader &reader)
{
	pt1.streamin(reader);
	pt2.streamin(reader);
}

void TLineSegment::Create(const Tvertex &p1, const Tvertex &p2)
{
	pt1=p1;
	pt2=p2;
}




void TLineSegment::intersect(const Tline &l1, const Tline &l2)
{
	Tvector dr1;dr1=l1.G_direction();
	Tvector dr2;dr2=l2.G_direction();
	double a=Tvector::dotprod(&dr1,&dr2);
	if (1-a*a<1.0e-19) throw QError(_text("Unable to determine line intersection"));
	Tvector DP;DP.subtrvertices(&l2.G_point(),&l1.G_point());
	double P=Tvector::dotprod(&DP,&dr1);
	double Q=Tvector::dotprod(&DP,&dr2);

	double r=(P-a*Q)/(1-a*a);
	double s=(a*P-Q)/(1-a*a);

	pt1.lincombfrom(1,&l1.G_point(),r,&dr1);
	pt2.lincombfrom(1,&l2.G_point(),s,&dr2);
}




///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_vertex,point)
{
	setclasspath_fromtype(SC_valname_vertex);
	setreturntype(SC_valname_vertex);
	addvar(_qstr("x"),SC_valname_scalar);
	addvar(_qstr("y"),SC_valname_scalar);
	addvar(_qstr("z"),SC_valname_scalar,false);
	addvar(_qstr("u"),SC_valname_scalar,false);
}

void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *v=G_valuecontent<Tvertex>(retval);
	v->S4_x(arglist->get(0)->G_content_scalar()->G_val());
	v->S4_y(arglist->get(1)->G_content_scalar()->G_val());
	if (arglist->G_ispresent(2)) v->S4_z(arglist->get(2)->G_content_scalar()->G_val());
	else v->S4_z(0);
	if (arglist->G_ispresent(3)) v->S4_u(arglist->get(3)->G_content_scalar()->G_val());
	else v->S4_u(1);
}

ENDFUNCTION(func_vertex)



///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_radial2point,radial2point)
{
	setclasspath_fromtype(SC_valname_vertex);
	setreturntype(SC_valname_vertex);
	addvar(_qstr("R"),SC_valname_scalar);
	addvar(_qstr("ang1"),SC_valname_scalar);
	addvar(_qstr("ang2"),SC_valname_scalar);
}

void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *v=G_valuecontent<Tvertex>(retval);
	double R=arglist->get(0)->G_content_scalar()->G_val();
	double ang1=arglist->get(1)->G_content_scalar()->G_val();
	double ang2=arglist->get(2)->G_content_scalar()->G_val();
	v->S3_x(R*cos(ang1)*cos(ang2));
	v->S3_y(R*sin(ang1)*cos(ang2));
	v->S3_z(R*sin(ang2));
}

ENDFUNCTION(func_radial2point)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_vertex3x,x)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *v=G_valuecontent<Tvertex>(owner);
	if (assigntoval!=NULL) v->S3_x(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(v->G3_x());
}
ENDFUNCTION(func_vertex3x)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_vertex3y,y)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *v=G_valuecontent<Tvertex>(owner);
	if (assigntoval!=NULL) v->S3_y(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(v->G3_y());
}
ENDFUNCTION(func_vertex3y)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_vertex3z,z)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvertex *v=G_valuecontent<Tvertex>(owner);
	if (assigntoval!=NULL) v->S3_z(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(v->G3_z());
}
ENDFUNCTION(func_vertex3z)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_vertex_distance,distance)
{
	setclasspath_fromtype(SC_valname_vertex);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("point1"),SC_valname_vertex);
	addvar(_qstr("point2"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double dst=Tvertex::distance(G_valuecontent<Tvertex>(arglist->get(0)),G_valuecontent<Tvertex>(arglist->get(1)));
	retval->G_content_scalar()->copyfrom(dst);
}
ENDFUNCTION(func_vertex_distance)



///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_vector,vector)
{
	setclasspath_fromtype(SC_valname_vector);
	setreturntype(SC_valname_vector);
	addvar(_qstr("x"),SC_valname_scalar);
	addvar(_qstr("y"),SC_valname_scalar);
	addvar(_qstr("z"),SC_valname_scalar,false);
}

void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvector *v=G_valuecontent<Tvector>(retval);
	v->S_x(arglist->get(0)->G_content_scalar()->G_val());
	v->S_y(arglist->get(1)->G_content_scalar()->G_val());
	if (arglist->G_ispresent(2)) v->S_z(arglist->get(2)->G_content_scalar()->G_val());
	else v->S_z(0);
}

ENDFUNCTION(func_vector)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_vectorx,x)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvector *v=G_valuecontent<Tvector>(owner);
	if (assigntoval!=NULL) v->S_x(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(v->G_x());
}
ENDFUNCTION(func_vectorx)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_vectory,y)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvector *v=G_valuecontent<Tvector>(owner);
	if (assigntoval!=NULL) v->S_y(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(v->G_y());
}
ENDFUNCTION(func_vectory)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_vectorz,z)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tvector *v=G_valuecontent<Tvector>(owner);
	if (assigntoval!=NULL) v->S_z(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(v->G_z());
}
ENDFUNCTION(func_vectorz)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_vector_size,size)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_scalar()->copyfrom(G_valuecontent<Tvector>(owner)->G_size());
}
ENDFUNCTION(func_vector_size)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_vector_vecnorm,vecnorm)
{
	setreturntype(SC_valname_vector);
	setclasspath_fromtype(SC_valname_vector);
	addvar(_qstr("v"),SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<Tvector>(retval)->normfrom(G_valuecontent<Tvector>(arglist->get(0)));
}
ENDFUNCTION(func_vector_vecnorm)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_vector_angle,vecangle)
{
	setreturntype(SC_valname_scalar);
	setclasspath_fromtype(SC_valname_vector);
	addvar(_qstr("v1"),SC_valname_vector);
	addvar(_qstr("v2"),SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double vl=Tvector::angle(G_valuecontent<Tvector>(arglist->get(0)),G_valuecontent<Tvector>(arglist->get(1)));
	retval->G_content_scalar()->copyfrom(vl);
}
ENDFUNCTION(func_vector_angle)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_vector_rotate,vecrotate)
{
	setreturntype(SC_valname_vector);
	setclasspath_fromtype(SC_valname_vector);
	addvar(_qstr("vc"),SC_valname_vector);
	addvar(_qstr("rotdir"),SC_valname_vector);
	addvar(_qstr("angle"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<Tvector>(retval)->rotatefrom(
		G_valuecontent<Tvector>(arglist->get(0)),
		G_valuecontent<Tvector>(arglist->get(1)),
		arglist->get(2)->G_content_scalar()->G_val());
}
ENDFUNCTION(func_vector_rotate)



///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_plane,Plane)
{
	setclasspath_fromtype(SC_valname_plane);
	setreturntype(SC_valname_plane);
	addvar(_qstr("fx"),SC_valname_scalar);
	addvar(_qstr("fy"),SC_valname_scalar);
	addvar(_qstr("fz"),SC_valname_scalar);
	addvar(_qstr("fu"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tplane *v=G_valuecontent<Tplane>(retval);
	v->S_x(arglist->get(0)->G_content_scalar()->G_val());
	v->S_y(arglist->get(1)->G_content_scalar()->G_val());
	v->S_z(arglist->get(2)->G_content_scalar()->G_val());
	v->S_u(arglist->get(3)->G_content_scalar()->G_val());
}
ENDFUNCTION(func_plane)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_createplane1,CreatePlane1)
{
	setclasspath_fromtype(SC_valname_plane);
	setreturntype(SC_valname_plane);
	addvar(_qstr("pt"),SC_valname_vertex);
	addvar(_qstr("normal"),SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tplane *v=G_valuecontent<Tplane>(retval);
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *nm=G_valuecontent<Tvector>(arglist->get(1));
	v->Create(*pt,*nm);
}
ENDFUNCTION(func_createplane1)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_createplane2,CreatePlane2)
{
	setclasspath_fromtype(SC_valname_plane);
	setreturntype(SC_valname_plane);
	addvar(_qstr("pt1"),SC_valname_vertex);
	addvar(_qstr("pt2"),SC_valname_vertex);
	addvar(_qstr("pt3"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tplane *v=G_valuecontent<Tplane>(retval);
	Tvertex *pt1=G_valuecontent<Tvertex>(arglist->get(0));
	Tvertex *pt2=G_valuecontent<Tvertex>(arglist->get(1));
	Tvertex *pt3=G_valuecontent<Tvertex>(arglist->get(2));
	v->Create(*pt1,*pt2,*pt3);
}
ENDFUNCTION(func_createplane2)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_planex,fx)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_plane);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tplane *v=G_valuecontent<Tplane>(owner);
	if (assigntoval!=NULL) v->S_x(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(v->G_x());
}
ENDFUNCTION(func_planex)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_planey,fy)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_plane);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tplane *v=G_valuecontent<Tplane>(owner);
	if (assigntoval!=NULL) v->S_y(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(v->G_y());
}
ENDFUNCTION(func_planey)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_planez,fz)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_plane);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tplane *v=G_valuecontent<Tplane>(owner);
	if (assigntoval!=NULL) v->S_z(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(v->G_z());
}
ENDFUNCTION(func_planez)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_planeu,fu)
{
	setcanassign();
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_plane);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tplane *v=G_valuecontent<Tplane>(owner);
	if (assigntoval!=NULL) v->S_u(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(v->G_u());
}
ENDFUNCTION(func_planeu)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_getplanenormal,Normal)
{
	setmemberfunction(SC_valname_plane);
	setreturntype(SC_valname_vector);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tplane *v=G_valuecontent<Tplane>(owner);
	if (assigntoval!=NULL) v->Set_normvector(*G_valuecontent<Tvector>(assigntoval));
	Tvector *nm=G_valuecontent<Tvector>(retval);
	v->G_normvector(*nm);
}
ENDFUNCTION(func_getplanenormal)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_getplaneanypoint,AnyPoint)
{
	setmemberfunction(SC_valname_plane);
	setreturntype(SC_valname_vertex);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tplane *v=G_valuecontent<Tplane>(owner);
	if (assigntoval!=NULL) v->Set_anypoint(*G_valuecontent<Tvertex>(assigntoval));
	Tvertex *pt=G_valuecontent<Tvertex>(retval);
	v->G_anypoint(*pt);
}
ENDFUNCTION(func_getplaneanypoint)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_evalplanepoint,EvalPoint)
{
	setmemberfunction(SC_valname_plane);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("pt"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tplane *v=G_valuecontent<Tplane>(owner);
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	retval->G_content_scalar()->copyfrom(v->evalpoint(*pt));
}
ENDFUNCTION(func_evalplanepoint)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_getplaneclosestpoint,ClosestPoint)
{
	setmemberfunction(SC_valname_plane);
	setreturntype(SC_valname_vertex);
	addvar(_qstr("pt"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tplane *v=G_valuecontent<Tplane>(owner);
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvertex *rtpt=G_valuecontent<Tvertex>(retval);
	v->findclosestpoint(*pt,*rtpt);
}
ENDFUNCTION(func_getplaneclosestpoint)




///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_createline1,CreateLine1)
{
	setclasspath_fromtype(SC_valname_line);
	setreturntype(SC_valname_line);
	addvar(_qstr("pt1"),SC_valname_vertex);
	addvar(_qstr("pt2"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tline *v=G_valuecontent<Tline>(retval);
	Tvertex *pt1=G_valuecontent<Tvertex>(arglist->get(0));
	Tvertex *pt2=G_valuecontent<Tvertex>(arglist->get(1));
	v->Create(*pt1,*pt2);
}
ENDFUNCTION(func_createline1)

///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_getlinedirection,Direction)
{
	setmemberfunction(SC_valname_line);
	setreturntype(SC_valname_vector);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tline *v=G_valuecontent<Tline>(owner);
	if (assigntoval!=NULL) v->Set_direction(*G_valuecontent<Tvector>(assigntoval));
	*(G_valuecontent<Tvector>(retval))=v->G_direction();
}
ENDFUNCTION(func_getlinedirection)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_getlineanypoint,AnyPoint)
{
	setmemberfunction(SC_valname_line);
	setreturntype(SC_valname_vertex);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tline *v=G_valuecontent<Tline>(owner);
	if (assigntoval!=NULL) v->Set_point(*G_valuecontent<Tvertex>(assigntoval));
	*(G_valuecontent<Tvertex>(retval))=v->G_point();
}
ENDFUNCTION(func_getlineanypoint)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_getlineclosestpoint,ClosestPoint)
{
	setmemberfunction(SC_valname_line);
	setreturntype(SC_valname_vertex);
	addvar(_qstr("pt"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Tline *l=G_valuecontent<Tline>(owner);
	Tvertex *pt=G_valuecontent<Tvertex>(arglist->get(0));
	Tvertex *rtpt=G_valuecontent<Tvertex>(retval);
	l->findclosestpoint(*pt,*rtpt);
}
ENDFUNCTION(func_getlineclosestpoint)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_linesegmentstart,StartPoint)
{
	setmemberfunction(SC_valname_linesegment);
	setreturntype(SC_valname_vertex);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TLineSegment *v=G_valuecontent<TLineSegment>(owner);
	if (assigntoval!=NULL) v->Set_point1(*G_valuecontent<Tvertex>(assigntoval));
	*(G_valuecontent<Tvertex>(retval))=v->G_point1();
}
ENDFUNCTION(func_linesegmentstart)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_linesegmentstop,EndPoint)
{
	setmemberfunction(SC_valname_linesegment);
	setreturntype(SC_valname_vertex);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TLineSegment *v=G_valuecontent<TLineSegment>(owner);
	if (assigntoval!=NULL) v->Set_point2(*G_valuecontent<Tvertex>(assigntoval));
	*(G_valuecontent<Tvertex>(retval))=v->G_point2();
}
ENDFUNCTION(func_linesegmentstop)



///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_createlinesegment,CreateLineSegment)
{
	setclasspath_fromtype(SC_valname_linesegment);
	setreturntype(SC_valname_linesegment);
	addvar(_qstr("pt1"),SC_valname_vertex);
	addvar(_qstr("pt2"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TLineSegment *v=G_valuecontent<TLineSegment>(retval);
	Tvertex *pt1=G_valuecontent<Tvertex>(arglist->get(0));
	Tvertex *pt2=G_valuecontent<Tvertex>(arglist->get(1));
	v->Create(*pt1,*pt2);
}
ENDFUNCTION(func_createlinesegment)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_plus_vertex,8,SC_valname_vertex,SC_valname_vertex,SC_valname_vertex)
{
	setclasspath_fromtype(SC_valname_vertex);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	Tvertex *v1=G_valuecontent<Tvertex>(val1);
	Tvertex *v2=G_valuecontent<Tvertex>(val2);
	G_valuecontent<Tvertex>(retval)->add(v1,v2);
}
ENDOPERATOR(op_plus_vertex)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_min_vertex,9,SC_valname_vertex,SC_valname_vertex,SC_valname_vector)
{
	setclasspath_fromtype(SC_valname_vertex);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	Tvertex *v1=G_valuecontent<Tvertex>(val1);
	Tvertex *v2=G_valuecontent<Tvertex>(val2);
	G_valuecontent<Tvector>(retval)->subtrvertices(v1,v2);
}
ENDOPERATOR(op_min_vertex)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_plus_pointvector,8,SC_valname_vertex,SC_valname_vector,SC_valname_vertex)
{
	setclasspath_fromtype(SC_valname_vertex);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	Tvertex *v1=G_valuecontent<Tvertex>(val1);
	Tvector *v2=G_valuecontent<Tvector>(val2);
	G_valuecontent<Tvertex>(retval)->add(v1,v2);
}
ENDOPERATOR(op_plus_pointvector)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_min_pointvector,9,SC_valname_vertex,SC_valname_vector,SC_valname_vertex)
{
	setclasspath_fromtype(SC_valname_vertex);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	Tvertex *v1=G_valuecontent<Tvertex>(val1);
	Tvector *v2=G_valuecontent<Tvector>(val2);
	G_valuecontent<Tvertex>(retval)->subtr(v1,v2);
}
ENDOPERATOR(op_min_pointvector)



///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_mul_vertex,10,SC_valname_scalar,SC_valname_vertex,SC_valname_vertex)
{
	setclasspath_fromtype(SC_valname_vertex);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	double vl=val1->G_content_scalar()->G_val();
	Tvertex *v=G_valuecontent<Tvertex>(val2);
	G_valuecontent<Tvertex>(retval)->mulfrom(v,vl);
}
ENDOPERATOR(op_mul_vertex)



///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_plus_vector,8,SC_valname_vector,SC_valname_vector,SC_valname_vector)
{
	setclasspath_fromtype(SC_valname_vector);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	Tvector *v1=G_valuecontent<Tvector>(val1);
	Tvector *v2=G_valuecontent<Tvector>(val2);
	G_valuecontent<Tvector>(retval)->add(v1,v2);
}
ENDOPERATOR(op_plus_vector)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_min_vector,9,SC_valname_vector,SC_valname_vector,SC_valname_vector)
{
	setclasspath_fromtype(SC_valname_vector);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	Tvector *v1=G_valuecontent<Tvector>(val1);
	Tvector *v2=G_valuecontent<Tvector>(val2);
	G_valuecontent<Tvector>(retval)->subtr(v1,v2);
}
ENDOPERATOR(op_min_vector)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_mul_vector,10,SC_valname_scalar,SC_valname_vector,SC_valname_vector)
{
	setclasspath_fromtype(SC_valname_vector);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	double vl=val1->G_content_scalar()->G_val();
	Tvector *v=G_valuecontent<Tvector>(val2);
	G_valuecontent<Tvector>(retval)->mulfrom(v,vl);
}
ENDOPERATOR(op_mul_vector)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_vecprod,10,SC_valname_vector,SC_valname_vector,SC_valname_vector)
{
	setclasspath_fromtype(SC_valname_vector);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	G_valuecontent<Tvector>(retval)->vecprod(G_valuecontent<Tvector>(val1),G_valuecontent<Tvector>(val2));
}
ENDOPERATOR(op_vecprod)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_dotprod,12,SC_valname_vector,SC_valname_vector,SC_valname_scalar)
{
	setclasspath_fromtype(SC_valname_vector);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_scalar()->copyfrom(Tvector::dotprod(G_valuecontent<Tvector>(val1),G_valuecontent<Tvector>(val2)));
}
ENDOPERATOR(op_dotprod)


///////////////////////////////////////////////////////////////////////////////////
TYPECAST(tc_vector_point,SC_valname_vector,SC_valname_vertex)
{
}
void execute_implement(TSC_value *argval, TSC_value *retval)
{
	Tvector *arg=G_valuecontent<Tvector>(argval);
	G_valuecontent<Tvertex>(retval)->copyfrom(*arg);
}
ENDTYPECAST(tc_vector_point)

///////////////////////////////////////////////////////////////////////////////////
TYPECAST(tc_point_vector,SC_valname_vertex,SC_valname_vector)
{
}
void execute_implement(TSC_value *argval, TSC_value *retval)
{
	Tvertex *arg=G_valuecontent<Tvertex>(argval);
	G_valuecontent<Tvector>(retval)->copyfrom(*arg);
}
ENDTYPECAST(tc_point_vector)


///////////////////////////////////////////////////////////////////////////////////
TYPECAST(tc_vector_matrix,SC_valname_vector,SC_valname_matrix)
{
}
void execute_implement(TSC_value *argval, TSC_value *retval)
{
	G_valuecontent<TMatrix>(retval)->copyfrom(G_valuecontent<Tvector>(argval));
}
ENDTYPECAST(tc_vector_matrix)

///////////////////////////////////////////////////////////////////////////////////
TYPECAST(tc_vertex_matrix,SC_valname_vertex,SC_valname_matrix)
{
}
void execute_implement(TSC_value *argval, TSC_value *retval)
{
	G_valuecontent<TMatrix>(retval)->copyfrom(G_valuecontent<Tvertex>(argval));
}
ENDTYPECAST(tc_vertex_matrix)

///////////////////////////////////////////////////////////////////////////////////
TYPECAST(tc_matrix_vertex,SC_valname_matrix,SC_valname_vertex)
{
}
void execute_implement(TSC_value *argval, TSC_value *retval)
{
	G_valuecontent<TMatrix>(argval)->copyto(G_valuecontent<Tvertex>(retval));
}
ENDTYPECAST(tc_matrix_vertex)

///////////////////////////////////////////////////////////////////////////////////
TYPECAST(tc_matrix_vector,SC_valname_matrix,SC_valname_vector)
{
}
void execute_implement(TSC_value *argval, TSC_value *retval)
{
	Tvector *vc=G_valuecontent<Tvector>(retval);
	G_valuecontent<TMatrix>(argval)->copyto(vc);
	if (vc->G_coord(3)!=0)
	{
		vc->Set_coord(0,vc->G_coord(0)/vc->G_coord(3));
		vc->Set_coord(1,vc->G_coord(1)/vc->G_coord(3));
		vc->Set_coord(2,vc->G_coord(2)/vc->G_coord(3));
		vc->Set_coord(3,0);
	}
}
ENDTYPECAST(tc_matrix_vector)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_and_planes,0,SC_valname_plane,SC_valname_plane,SC_valname_line)
{
	setclasspath_fromtype(SC_valname_plane);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	Tplane *p1=G_valuecontent<Tplane>(val1);
	Tplane *p2=G_valuecontent<Tplane>(val2);
	G_valuecontent<Tline>(retval)->intersect(*p1,*p2);
}
ENDOPERATOR(op_and_planes)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_and_planeline,0,SC_valname_plane,SC_valname_line,SC_valname_vertex)
{
	setclasspath_fromtype(SC_valname_plane);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	Tplane *p=G_valuecontent<Tplane>(val1);
	Tline *l=G_valuecontent<Tline>(val2);
	p->intersect(*l,*G_valuecontent<Tvertex>(retval));
}
ENDOPERATOR(op_and_planeline)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_and_lineplane,0,SC_valname_line,SC_valname_plane,SC_valname_vertex)
{
	setclasspath_fromtype(SC_valname_plane);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	Tline *l=G_valuecontent<Tline>(val1);
	Tplane *p=G_valuecontent<Tplane>(val2);
	p->intersect(*l,*G_valuecontent<Tvertex>(retval));
}
ENDOPERATOR(op_and_lineplane)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_and_lineline,0,SC_valname_line,SC_valname_line,SC_valname_linesegment)
{
	setclasspath_fromtype(SC_valname_line);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	Tline *l1=G_valuecontent<Tline>(val1);
	Tline *l2=G_valuecontent<Tline>(val2);
	TLineSegment *s=G_valuecontent<TLineSegment>(retval);
	s->intersect(*l1,*l2);
}
ENDOPERATOR(op_and_lineline)
