#include "stdafx.h"

#include "tools.h"

#include "CSG_components.h"



double CSGVector::G_size() const
{
	return sqrt(x*x+y*y+z*z);
}

bool CSGVector::IsZero() const
{
	if (fabs(x)+fabs(y)+fabs(z)<TOL) return true;
	else return false;
}

bool CSGVector::IsSame(const CSGVector &iv) const
{
	if (fabs(x-iv.x)+fabs(y-iv.y)+fabs(z-iv.z)<TOL) return true;
	else return false;
}


void CSGVector::anynormalfrom(const CSGVector &v1)
{
	CSGVector tst1;tst1.vecprod(v1,CSGVector(1,0,0));
	CSGVector tst2;tst2.vecprod(v1,CSGVector(0,1,0));
	if (tst1.G_size()>tst2.G_size()) normfrom(tst1);
	else normfrom(tst2);
}


void CSGVector::normfrom(const CSGVector &v1)
{
	double sz=v1.G_size();
	if (sz==0)
	{
		x=0;y=0;z=0;
		return;
	}
	x=v1.x/sz;
	y=v1.y/sz;
	z=v1.z/sz;
}

void CSGVector::Normalise()
{
	double sz=G_size();
	if (sz==0)
	{
		x=0;y=0;z=0;
		return;
	}
	x/=sz;
	y/=sz;
	z/=sz;
}

void CSGVector::Invert()
{
	x=-x;
	y=-y;
	z=-z;
}


void CSGVector::mul(double fc)
{
	x*=fc;
	y*=fc;
	z*=fc;
}


void CSGVector::lincomb(const CSGVector &v1, double f1, const CSGVector &v2, double f2)
{
	x=f1*v1.x + f2*v2.x;
	y=f1*v1.y + f2*v2.y;
	z=f1*v1.z + f2*v2.z;
}

void CSGVector::lincomb(const CSGVector &v1, double f1, const CSGVector &v2, double f2, const CSGVector &v3, double f3)
{
	x=f1*v1.x + f2*v2.x + f3*v3.x;
	y=f1*v1.y + f2*v2.y + f3*v3.y;
	z=f1*v1.z + f2*v2.z + f3*v3.z;
}




void CSGVector::vecprod(const CSGVector &v1, const CSGVector &v2)
{
	x=v1.y*v2.z - v1.z*v2.y;
	y=v1.z*v2.x - v1.x*v2.z;
	z=v1.x*v2.y - v1.y*v2.x;
}

/*CSGVector operator*(const CSGVector &v1, const CSGVector &v2)
{
	CSGVector rs;
	rs.vecprod(v1,v2);
	return rs;
}*/

CSGVector operator-(const CSGVector &v1, const CSGVector &v2)
{
	CSGVector rs;
	rs.lincomb(v1,+1,v2,-1);
	return rs;
}


double CSGVector::dotprod(const CSGVector &v1, const CSGVector &v2)
{
	return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}


double CSGVector::G_angle(const CSGVector &v1, const CSGVector &v2)
{
	CSGVector va,vb,vv;

	va.normfrom(v1);
	vb.normfrom(v2);
	vv.vecprod(va,vb);
	double sina=vv.G_size();
	double cosa=dotprod(va,vb);
	double ang=atan2(sina,cosa);
	return ang;
}


double PointToLineDistance(const CSGVector &pt, const CSGVector &linept1, const CSGVector &linept2)
{
	CSGVector v21=linept2-linept1;
	CSGVector v10=linept1-pt;

	double size21=v21.G_size();
	if (size21==0) return 0;

	CSGVector cross;cross.vecprod(v21,v10);

	return cross.G_size()/size21;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// CSGVertex
/////////////////////////////////////////////////////////////////////////////////////////////////////


int CSGVertex::UNKNOWN = 1;
int CSGVertex::INSIDE = 2;
int CSGVertex::OUTSIDE = 3;
int CSGVertex::BOUNDARY = 4;


CSGVertex::CSGVertex()
{
	x=0;y=0;z=0;
	status=UNKNOWN;
}

CSGVertex::CSGVertex(double ix, double iy, double iz)
{
	x=ix;y=iy;z=iz;
	status=UNKNOWN;
}

CSGVertex::CSGVertex(const CSGVertex &iv)
{
	copyfrom(iv);
}


CSGVertex::CSGVertex(double ix, double iy, double iz, int istatus)
{
	x=ix;y=iy;z=iz;
	status=istatus;
}

void CSGVertex::copyfrom(const CSGVertex &iv)
{
	x=iv.x;y=iv.y;z=iv.z;
	status=iv.status;
	normal=iv.normal;
	neighborvertices.reset();
	for (int i=0; i<iv.neighborvertices.G_count(); i++)
		neighborvertices.add(iv.neighborvertices[i]);
}

CSGVertex& CSGVertex::operator=(const CSGVertex &v)
{
	copyfrom(v);
	return *this;
}


CSGVector CSGVertex::G_Position() const
{
	return CSGVector(x,y,z);
}

void CSGVertex::Set_Status(int istatus)
{
	if(istatus>=UNKNOWN && istatus<=BOUNDARY)
	{
		status = istatus;
	}
}

int CSGVertex::G_Status() const
{
	return status;
}



void CSGVertex::add_neighbor(CSGVertex *nb)
{
	for(int i=0; i<neighborvertices.G_count(); i++)
		if(neighborvertices[i]==nb) return;
	neighborvertices.add(nb);
}


void CSGVertex::mark(int istatus)
{
	status = istatus;
	if (istatus!=CSGVertex::UNKNOWN)
	{
		for(int i=0; i < neighborvertices.G_count(); i++)
			if(neighborvertices[i]->G_Status()==CSGVertex::UNKNOWN)
				neighborvertices[i]->mark(istatus);
	}
}



bool CSGVertex::issamepos(const CSGVertex &iv) const
{
	if (fabs(x-iv.x)>TOL) return false;
	if (fabs(y-iv.y)>TOL) return false;
	if (fabs(z-iv.z)>TOL) return false;
	return true;
}

double CSGVertex::G_distance(const CSGVertex &v1, const CSGVertex &v2)
{
	return sqrt( (v1.x-v2.x)*(v1.x-v2.x) + (v1.y-v2.y)*(v1.y-v2.y) + (v1.z-v2.z)*(v1.z-v2.z) );
}

bool CSGVertex::equals(const CSGVector &ipos) const
{
	if (x<ipos.x-TOL) return false;
	if (x>ipos.x+TOL) return false;
	if (y<ipos.y-TOL) return false;
	if (y>ipos.y+TOL) return false;
	if (z<ipos.z-TOL) return false;
	if (z>ipos.z+TOL) return false;
	return true;
}





/////////////////////////////////////////////////////////////////////////////////////////////////////
//CSGBBox
/////////////////////////////////////////////////////////////////////////////////////////////////////

CSGBBox::CSGBBox()
{
	xmax=-1.0e99;xmin=+1.0e99;
	ymax=-1.0e99;ymin=+1.0e99;
	zmax=-1.0e99;zmin=+1.0e99;
}

CSGBBox::CSGBBox(const CSGVertex &v1, const CSGVertex &v2, const CSGVertex &v3)
{
	xmax=-1.0e99;xmin=+1.0e99;
	ymax=-1.0e99;ymin=+1.0e99;
	zmax=-1.0e99;zmin=+1.0e99;
	AddVertex(v1);AddVertex(v2);AddVertex(v3);
}

CSGBBox::CSGBBox(const CSGVertexList &vertexlist)
{
	xmax=-1.0e99;xmin=+1.0e99;
	ymax=-1.0e99;ymin=+1.0e99;
	zmax=-1.0e99;zmin=+1.0e99;
	for (int i=0; i<vertexlist.G_count(); i++) AddVertex(*vertexlist[i]);
}

void CSGBBox::copyfrom(const CSGBBox &ibbox)
{
	xmax=ibbox.xmax;xmin=ibbox.xmin;
	ymax=ibbox.ymax;ymin=ibbox.ymin;
	zmax=ibbox.zmax;zmin=ibbox.zmin;
}


CSGBBox& CSGBBox::operator=(const CSGBBox &v)
{
	copyfrom(v);
	return *this;
}


bool CSGBBox::overlap(const CSGBBox &ibox)
{
	if ((xmin>ibox.xmax+TOL)||(xmax<ibox.xmin-TOL)||(ymin>ibox.ymax+TOL)||(ymax<ibox.ymin-TOL)||(zmin>ibox.zmax+TOL)||(zmax<ibox.zmin-TOL))
		return false;
	else
		return true;
}


void CSGBBox::AddVertex(const CSGVertex &vertex)
{
	if(vertex.x>xmax) xmax = vertex.x;
	if(vertex.x<xmin) xmin = vertex.x;
	if(vertex.y>ymax) ymax = vertex.y;
	if(vertex.y<ymin) ymin = vertex.y;
	if(vertex.z>zmax) zmax = vertex.z;
	if(vertex.z<zmin) zmin = vertex.z;
}

void CSGBBox::add(const CSGBBox &ibbox)
{
	if(ibbox.xmax>xmax) xmax = ibbox.xmax;
	if(ibbox.xmin<xmin) xmin = ibbox.xmin;
	if(ibbox.ymax>ymax) ymax = ibbox.ymax;
	if(ibbox.ymin<ymin) ymin = ibbox.ymin;
	if(ibbox.zmax>zmax) zmax = ibbox.zmax;
	if(ibbox.zmin<zmin) zmin = ibbox.zmin;
}




