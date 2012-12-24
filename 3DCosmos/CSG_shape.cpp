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

#include "vec_transformation.h"
#include "2DContour.h"
#include "tesselator.h"

#include "3dspacewarper.h"


#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <list>
#include <sys/types.h>





/////////////////////////////////////////////////////////////////////
// CSGShape
/////////////////////////////////////////////////////////////////////


CSGShape::CSGShape()
{
}

CSGShape::CSGShape(const CSGShape &ishape)
{
	copyfrom(ishape);
}

CSGShape::~CSGShape()
{
}



void CSGShape::copyfrom(const CSGShape &ishape)
{
	vertices.reset();
	faces.reset();
	bbox.copyfrom(ishape.bbox);

	for (int i=0; i<ishape.vertices.G_count(); i++)
	{
		CSGVertex *v=addvertex(*ishape.vertices[i]);
		v->status=CSGVertex::UNKNOWN;
		v->neighborvertices.reset();
	}

	for (int i=0; i<ishape.faces.G_count(); i++)
	{
		CSGFace &fc=ishape.faces[i];
		addFace(vertices[fc.v1->idx],vertices[fc.v2->idx],vertices[fc.v3->idx],fc.n1,fc.n2,fc.n3,fc.label);
	}
	finder.reset();
}

void CSGShape::operator=(const CSGShape &v)
{
	copyfrom(v);
}

void CSGShape::tostring(QString &str, int tpe)
{
	str=TFormatString(_text("^1 vertices + ^2 triangles"),vertices.G_count(),faces.G_count());
}

void CSGShape::fromstring(StrPtr str)
{
	ASSERT(false);
}

void CSGShape::reset()
{
	vertices.reset();
	faces.reset();
	finder.reset();
	bbox=CSGBBox();
	int ii=0;
}


void CSGShape::streamout(QBinTagWriter &writer)
{
	ASSERT(false);
}
void CSGShape::streamin(QBinTagReader &reader)
{
	ASSERT(false);
}


void CSGShape::addpolygon(Tcopyarray<Tvertex> &vertexlist, int ilabel)
{
	//--!!-- todo: use proper tesselation?
	if (vertexlist.G_count()<3) throw QError(_text("Unable to add shape polygon: at least 3 points are needed"));
	CSGVertex *v0=addvertexifnew(CSGVertex(vertexlist[0]->G3_x(),vertexlist[0]->G3_y(),vertexlist[0]->G3_z()));
	for (int i=1; i<vertexlist.G_count()-1; i++)
	{
		CSGVertex *v1=addvertexifnew(CSGVertex(vertexlist[i]->G3_x(),vertexlist[i]->G3_y(),vertexlist[i]->G3_z()));
		CSGVertex *v2=addvertexifnew(CSGVertex(vertexlist[i+1]->G3_x(),vertexlist[i+1]->G3_y(),vertexlist[i+1]->G3_z()));
		CSGFace *face=addFace(v0,v1,v2);
		if (face!=NULL)
		{
			face->label=ilabel;
			face->n1=face->G_Normal();
			face->n2=face->G_Normal();
			face->n3=face->G_Normal();
		}
	}
}


void CSGShape::createbar(const CSGVector &pos, const CSGVector &size)
{
	reset();
	CSGVertex *v1=addvertex(CSGVertex(pos.x,pos.y,pos.z));
	CSGVertex *v2=addvertex(CSGVertex(pos.x+size.x,pos.y,pos.z));
	CSGVertex *v3=addvertex(CSGVertex(pos.x+size.x,pos.y+size.y,pos.z));
	CSGVertex *v4=addvertex(CSGVertex(pos.x,pos.y+size.y,pos.z));
	CSGVertex *v5=addvertex(CSGVertex(pos.x,pos.y,pos.z+size.z));
	CSGVertex *v6=addvertex(CSGVertex(pos.x+size.x,pos.y,pos.z+size.z));
	CSGVertex *v7=addvertex(CSGVertex(pos.x+size.x,pos.y+size.y,pos.z+size.z));
	CSGVertex *v8=addvertex(CSGVertex(pos.x,pos.y+size.y,pos.z+size.z));

	addFace(v3,v2,v1,CSGVector(0,0,-1));addFace(v1,v4,v3,CSGVector(0,0,-1));

	addFace(v7,v8,v5,CSGVector(0,0,1));addFace(v5,v6,v7,CSGVector(0,0,1));

	addFace(v1,v2,v6,CSGVector(0,-1,0));addFace(v6,v5,v1,CSGVector(0,-1,0));

	addFace(v2,v3,v7,CSGVector(1,0,0));addFace(v7,v6,v2,CSGVector(1,0,0));

	addFace(v3,v4,v8,CSGVector(0,1,0));addFace(v8,v7,v3,CSGVector(0,1,0));

	addFace(v4,v1,v5,CSGVector(-1,0,0));addFace(v5,v8,v4,CSGVector(-1,0,0));

}

/*
	int i1=mesh.addvertex(&Tvertex(0,0,0),				0,NULL,0,0);
	int i2=mesh.addvertex(&Tvertex(sizex,0,0),			1,NULL,1,0);
	int i3=mesh.addvertex(&Tvertex(sizex,sizey,0),		2,NULL,1,1);
	int i4=mesh.addvertex(&Tvertex(0,sizey,0),			3,NULL,0,1);
	int i5=mesh.addvertex(&Tvertex(0,0,sizez),			4,NULL,0,0);
	int i6=mesh.addvertex(&Tvertex(sizex,0,sizez),		5,NULL,1,0);
	int i7=mesh.addvertex(&Tvertex(sizex,sizey,sizez),	6,NULL,1,1);
	int i8=mesh.addvertex(&Tvertex(0,sizey,sizez),		7,NULL,0,1);

	mesh.addtriangle(i3,i2,i1);mesh.addtriangle(i1,i4,i3);

	mesh.addtriangle(i7,i8,i5);mesh.addtriangle(i5,i6,i7);

	mesh.addtriangle(i1,i2,i6);mesh.addtriangle(i6,i5,i1);

	mesh.addtriangle(i2,i3,i7);mesh.addtriangle(i7,i6,i2);

	mesh.addtriangle(i3,i4,i8);mesh.addtriangle(i8,i7,i3);

	mesh.addtriangle(i4,i1,i5);mesh.addtriangle(i5,i8,i4);
*/

void CSGShape::createsphere(const CSGVector &cent, double radius, int resol)
{
	reset();

	int i,j,i1,i2,j1,j2;
	double al1,al2;

//	reporttimer(_text("sphere start"));

	CSGVertex vertex;
	CSGVector norm;

	int ct=0;
	for (i=0; i<=2*resol-1; i++)
	{
		al1=(i*1.0)/resol*Pi;
		for (j=0; j<=resol; j++)
		{
			al2=(j*1.0)/resol*Pi;
			norm.x=sin(al2)*cos(al1);
			norm.y=sin(al2)*sin(al1);
			norm.z=cos(al2);
			vertex.x=cent.x+radius*norm.x;
			vertex.y=cent.y+radius*norm.y;
			vertex.z=cent.z+radius*norm.z;
			vertex.normal=norm;
			addvertex(vertex);
			ct++;
		}
	}

	for (i=0; i<=2*resol-1; i++)
	{
		i1=i;i2=(i+1)%(2*resol);
		for (j=0; j<resol; j++)
		{
			j1=j; j2=j+1;
			if (j<resol-1)
			{
				addFace(vertices[i1*(resol+1)+j2],vertices[i2*(resol+1)+j2],vertices[i2*(resol+1)+j1],
					vertices[i1*(resol+1)+j2]->normal,vertices[i2*(resol+1)+j2]->normal,vertices[i2*(resol+1)+j1]->normal,0);
			}
			if (j>0)
			{
				addFace(vertices[i1*(resol+1)+j2],vertices[i2*(resol+1)+j1],vertices[i1*(resol+1)+j1],
					vertices[i1*(resol+1)+j2]->normal,vertices[i2*(resol+1)+j1]->normal,vertices[i1*(resol+1)+j1]->normal,0);
			}
		}
	}

	for (int i=0; i<vertices.G_count(); i++)
		vertices[i]->normal=CSGVector(0,0,0);

//	reporttimer(_text("sphere stop"));

}


void CSGShape::createcylinder(const CSGVector &start, const CSGVector &axis, double radius , int resol)
{
	reset();

	double al;
	int i1,i2;
	CSGVertex vertex;

	CSGVector da,db;

	da.anynormalfrom(axis);
	db.vecprod(axis,da);db.Normalise();

	for (int i=0; i<resol; i++)
	{
		al=(i*1.0)/resol*2*Pi;
		vertex.normal.x=cos(al)*da.x + sin(al)*db.x;
		vertex.normal.y=cos(al)*da.y + sin(al)*db.y;
		vertex.normal.z=cos(al)*da.z + sin(al)*db.z;

		vertex.x=start.x+radius*vertex.normal.x;
		vertex.y=start.y+radius*vertex.normal.y;
		vertex.z=start.z+radius*vertex.normal.z;
		addvertex(vertex);

		vertex.x=start.x+axis.x+radius*vertex.normal.x;
		vertex.y=start.y+axis.y+radius*vertex.normal.y;
		vertex.z=start.z+axis.z+radius*vertex.normal.z;
		addvertex(vertex);
	}

	vertex.normal.normfrom(axis);vertex.normal.Invert();
	vertex.x=start.x;vertex.y=start.y;vertex.z=start.z;
	CSGVertex *center1=addvertex(vertex);

	vertex.normal.normfrom(axis);
	vertex.x=start.x+axis.x;vertex.y=start.y+axis.y;vertex.z=start.z+axis.z;
	CSGVertex *center2=addvertex(vertex);

	//top cap
	for (int i=0; i<resol; i++)
	{
		i1=i;i2=(i+1)%(resol);
		addFace(vertices[2*i1+1],vertices[2*i2+1],center2,
			center2->normal,center2->normal,center2->normal,0);
	}

	//bottom cap
	for (int i=0; i<resol; i++)
	{
		i1=i;i2=(i+1)%(resol);
		addFace(vertices[2*i2+0],vertices[2*i1+0],center1,
			center1->normal,center1->normal,center1->normal,0);
	}

	//vertical cylinder
	for (int i=0; i<resol; i++)
	{
		i1=i;i2=(i+1)%(resol);
		addFace(vertices[2*i1+0],vertices[2*i2+0],vertices[2*i1+1],
			vertices[2*i1+0]->normal,vertices[2*i2+0]->normal,vertices[2*i1+1]->normal,0);
		addFace(vertices[2*i2+0],vertices[2*i2+1],vertices[2*i1+1],
			vertices[2*i2+0]->normal,vertices[2*i2+1]->normal,vertices[2*i1+1]->normal,0);
	}
}


void CSGShape::create_extrudedpolygon(T2DContourset *contourset0, double height, int layercount)
{
	reset();

	G_tesselator().tesselate(contourset0);

	if (layercount<2) layercount=2;


	//calculate total number of points in a single layer
	int totlayerpointcount=0;
	for (int contournr=0; contournr<contourset0->G_contourcount(); contournr++)
	{
		T2DContour *contour=contourset0->G_contour(contournr);
		totlayerpointcount+=contour->G_pointcount();
	}

	Tintarray topcapidx,bottomcapidx;

	//vertical slices
	for (int layernr=0; layernr<layercount; layernr++)
	{
		for (int contournr=0; contournr<contourset0->G_contourcount(); contournr++)
		{
			T2DContour *contour=contourset0->G_contour(contournr);
			for (int vnr=0; vnr<contour->G_pointcount(); vnr++)
			{
				T2DContourPoint &cpt=contour->G_point(vnr%contour->G_pointcount());
				CSGVertex v(cpt.pt.G3_x(),cpt.pt.G3_y(),cpt.pt.G3_z()+layernr*1.0/(layercount-1)*height);
				v.normal=cpt.nm;
				CSGVertex *nv=addvertex(v);
				if (layernr==0) bottomcapidx.add(nv->idx);
				if (layernr==layercount-1) topcapidx.add(nv->idx);
				if ((vnr>0)&&(layernr>0))
				{
					int cvnr=vertices.G_count()-1;
					addquad(	cvnr -0 -0*totlayerpointcount,
								cvnr -1 -0*totlayerpointcount,
								cvnr -1 -1*totlayerpointcount,
								cvnr -0 -1*totlayerpointcount);
				}
			}
		}
	}

	//top cap
	{
		CSGVector normal(0,0,1);
		for (int tnr=0; tnr<G_tesselator().G_trianglecount(); tnr++)
		{
			Ttesstriangle &tri=G_tesselator().G_triangle(tnr);
			addFace(vertices[topcapidx[tri.nr1]],vertices[topcapidx[tri.nr2]],vertices[topcapidx[tri.nr3]],normal);
		}
	}

	//bottom cap
	{
		CSGVector normal(0,0,-1);
		for (int tnr=0; tnr<G_tesselator().G_trianglecount(); tnr++)
		{
			Ttesstriangle &tri=G_tesselator().G_triangle(tnr);
			addFace(vertices[bottomcapidx[tri.nr1]],vertices[bottomcapidx[tri.nr3]],vertices[bottomcapidx[tri.nr2]],normal);
		}
	}
}


void CSGShape::create_conepolygon(T2DContourset *contourset,const Tvertex &top)
{
	reset();

	G_tesselator().tesselate(contourset);

	//introduce points
	Tintarray capidx;
	for (int contournr=0; contournr<contourset->G_contourcount(); contournr++)
	{
		T2DContour *contour=contourset->G_contour(contournr);
		for (int vnr=0; vnr<contour->G_pointcount(); vnr++)
		{
			T2DContourPoint &cpt=contour->G_point(vnr%contour->G_pointcount());
			CSGVertex v(cpt.pt.G3_x(),cpt.pt.G3_y(),cpt.pt.G3_z());
			CSGVertex *nv=addvertex(v);
			capidx.add(nv->idx);
		}
	}
	int topidx=addvertex(CSGVertex(top.G3_x(),top.G3_y(),top.G3_z()))->idx;

	//bottom cap
	{
		CSGVector normal(0,0,-1);
		for (int tnr=0; tnr<G_tesselator().G_trianglecount(); tnr++)
		{
			Ttesstriangle &tri=G_tesselator().G_triangle(tnr);
			addFace(vertices[capidx[tri.nr1]],vertices[capidx[tri.nr3]],vertices[capidx[tri.nr2]],normal);
		}
	}

	//cone
	int i1,i2;
	for (int i=0; i<capidx.G_count(); i++)
	{
		i1=i;i2=i+1;
		addFace(vertices[capidx[i1]],vertices[capidx[i2]],vertices[topidx]);
	}

	createflatnormals();

}


void CSGShape::create_polyhedron(Tcopyarray<Tplane> &planelist, Tintarray &indexlist)
{
	reset();

	//iterate over all planes
	for (int facenr=0; facenr<planelist.G_count(); facenr++)
	{

		Tarray<Tline> lines;
		Tcopyarray<Tplane> lines_fromplane;

		//iterate over all other planes to find the lines
		for (int linenr=0; linenr<planelist.G_count(); linenr++)
			if (linenr!=facenr)
			{
				if (!planelist[facenr]->isparallel(*planelist[linenr]))
				{
					Tline *line=new Tline;
					line->intersect(*planelist[facenr],*planelist[linenr]);
					lines.add(line);lines_fromplane.add(planelist[linenr]);
				}
			}

		Tarray<Tvertex> points;
		Tcopyarray<Tplane> points_fromplane1,points_fromplane2;

		//find all line crossing points
		for (int l1=0; l1<lines.G_count(); l1++)
			for (int l2=0; l2<l1; l2++)
				if (!lines_fromplane[l1]->isparallel(*lines[l2]))
				{
					Tvertex *pt=new Tvertex;
					lines_fromplane[l1]->intersect(*lines[l2],*pt);
					points.add(pt);
					points_fromplane1.add(lines_fromplane[l1]);
					points_fromplane2.add(lines_fromplane[l2]);
				}

		//remove crossing points that lie on the wrong side of a plane
		for (int ptnr=0; ptnr<points.G_count();)
		{
			bool ok=true;
			for (int planenr=0; planenr<planelist.G_count(); planenr++)
				if ((planenr!=facenr)&&(planelist[planenr]!=points_fromplane1[ptnr])&&(planelist[planenr]!=points_fromplane2[ptnr]))
					if (planelist[planenr]->evalpoint(*points[ptnr])>0) ok=false;
			if (!ok)
			{
				points.del(ptnr);
				points_fromplane1.del(ptnr);
				points_fromplane2.del(ptnr);
			}
			else ptnr++;
		}

		Tintarray ptused;
		for (int ptnr=0; ptnr<points.G_count(); ptnr++) ptused.add(0);

		if (points.G_count()>=3)
		{

			Tcopyarray<Tvertex> vlist;

			ptused[0]=1;
			vlist.add(points[0]);
			Tplane *lastplane=points_fromplane1[0];

			bool finished=false;
			while (!finished)
			{
				finished=true;
				for (int ptnr=0; (ptnr<points.G_count())&&(finished); ptnr++)
					if (!ptused[ptnr])
					{
						if (points_fromplane1[ptnr]==lastplane)
						{
							finished=false;
							lastplane=points_fromplane2[ptnr];
							vlist.add(points[ptnr]);
							ptused[ptnr]=1;
						} else if (points_fromplane2[ptnr]==lastplane)
						{
							finished=false;
							lastplane=points_fromplane1[ptnr];
							vlist.add(points[ptnr]);
							ptused[ptnr]=1;
						}
					}
			}

			//determine winding
			Tvector normvector;
			planelist[facenr]->G_normvector(normvector);
			double vl=0;
			for (int ptnr=0; ptnr<vlist.G_count(); ptnr++)
			{
				Tvector v1;v1.subtrvertices(vlist[(ptnr+1)%vlist.G_count()],vlist[ptnr]);
				Tvector v2;v2.subtrvertices(vlist[ptnr],vlist[(ptnr-1+vlist.G_count())%vlist.G_count()]);
				Tvector v3;v3.vecprod(&v1,&v2);
				vl+=Tvector::dotprod(&v3,&normvector);
			}

			if (vl<0)
			{
				addpolygon(vlist,indexlist[facenr]);
			}
			else
			{
				Tcopyarray<Tvertex> vlist2;
				for (int ptnr=0; ptnr<vlist.G_count(); ptnr++) vlist2.add(vlist[vlist.G_count()-1-ptnr]);
				addpolygon(vlist2,indexlist[facenr]);
			}
		}

	}
}






void CSGShape::create_revolvedpolygon(T2DContourset *contourset, int resolution)
{
	reset();

	if (resolution<2) throw QError(_text("Resolution should be at least 2"));
	if (contourset->G_contourcount()!=1) throw QError(_text("Contour set should contain exactly one contour"));
	T2DContour *contour=contourset->G_contour(0);

	contour->calcdims();


	int contourptcount=contour->G_pointcount();
	double ang;
	Tvec3d vr,nm;
	int cvnr,i1,i2;
	CSGVertex vertex;

	Tintarray bottomcapidx,topcapidx;

	for (int i=0; i<=resolution; i++)
	{
		ang=i*1.0/resolution*2*Pi;
		for (int j=0; j<contour->G_pointcount(); j++)
		{
			T2DContourPoint &pt=contour->G_point(j);
			vr.x=(GLfloat)(pt.pt.G3_x());
			vr.y=(GLfloat)(pt.pt.G3_y()*cos(ang));
			vr.z=(GLfloat)(pt.pt.G3_y()*sin(ang));
			nm.x=-(GLfloat)(pt.nm.G_x());
			nm.y=-(GLfloat)(pt.nm.G_y()*cos(ang));
			nm.z=-(GLfloat)(pt.nm.G_y()*sin(ang));
			CSGVertex v(vr.x,vr.y,vr.z);
			v.normal=CSGVector(nm.x,nm.y,nm.z);
			CSGVertex *nv=addvertex(v);
			cvnr=nv->idx;
			if (j==0) bottomcapidx.add(cvnr);
			if (j==contour->G_pointcount()-1) topcapidx.add(cvnr);
			if ((i>0)&&(j>0))
				addquad(
							cvnr-0 -0*contourptcount,
							cvnr-0 -1*contourptcount,
							cvnr-1 -1*contourptcount,
							cvnr-1 -0*contourptcount
							);
		}
	}

	if (!contour->G_closed())
	{
		//bottom cap
		if (fabs(contour->G_point(0).pt.G3_y())>TOL)
		{
			vertex.normal=CSGVector(-1,0,0);
			vertex.x=contour->G_point(0).pt.G3_x();
			vertex.y=0;
			vertex.z=0;
			CSGVertex *center1=addvertex(vertex);
			for (int i=0; i<resolution; i++)
			{
				i1=i;i2=(i+1)%(resolution);
				addFace(vertices[bottomcapidx[i2]],vertices[bottomcapidx[i1]],center1,
					center1->normal,center1->normal,center1->normal,0);
			}
		}
		//top cap
		if (fabs(contour->G_point(contour->G_pointcount()-1).pt.G3_y())>TOL)
		{
			vertex.normal=CSGVector(+1,0,0);
			vertex.x=contour->G_point(contour->G_pointcount()-1).pt.G3_x();
			vertex.y=0;
			vertex.z=0;
			CSGVertex *center2=addvertex(vertex);
			for (int i=0; i<resolution; i++)
			{
				i1=i;i2=(i+1)%(resolution);
				addFace(vertices[topcapidx[i1]],vertices[topcapidx[i2]],center2,
					center2->normal,center2->normal,center2->normal,0);
			}
		}
	}
}


void CSGShape::createflatnormals()
{
	calcfacenormals();
	for (int i=0; i<faces.G_count(); i++)
	{
		faces[i].n1=faces[i].G_Normal();
		faces[i].n2=faces[i].G_Normal();
		faces[i].n3=faces[i].G_Normal();
	}
}


void CSGShape::SetFaceLabels(int ilabel)
{
	for (int i=0; i<faces.G_count(); i++)
		faces[i].label=ilabel;
}


void CSGShape::calcfacenormals()
{
	for (int i=0; i<faces.G_count(); i++)
		faces[i].calcfacenormal();
}


void CSGShape::subsample(double maxdist)
{
	double d[3];
	CSGVector newpos;
	bool broken;
	double dmax;
	int pmax;
	CSGVertex *va,*vb;

//	reporttimer(_text("subsampling started"));

	startfinder(bbox);

	for (int facenr=0; facenr<faces.G_count(); )
	{
		CSGFace face=faces[facenr];
		broken=false;
		d[0]=CSGVertex::G_distance(*face.v1,*face.v2);
		d[1]=CSGVertex::G_distance(*face.v2,*face.v3);
		d[2]=CSGVertex::G_distance(*face.v3,*face.v1);
		dmax=maxdist;pmax=-1;for (int i=0; i<3; i++) if (d[i]>dmax) { dmax=d[i];pmax=i; }
		if (pmax>=0)
		{
			if (pmax==0) { va=face.v1; vb=face.v2; }
			if (pmax==1) { va=face.v2; vb=face.v3; }
			if (pmax==2) { va=face.v3; vb=face.v1; }
			newpos.x=(va->x+vb->x)/2;
			newpos.y=(va->y+vb->y)/2;
			newpos.z=(va->z+vb->z)/2;
			breakFaceInTwo(facenr,newpos,pmax+1);
			broken=true;
		}
		if (!broken) facenr++;
	}

	stopfinder();

//	reporttimer(_text("subsampling completed"));

}


void CSGShape::smoothen(double maxdist, double alpha)
{


	Tarray< Tcheapcopyarray<CSGVertex> > smoothneighbors;
	Tarray< Tdoublearray > smoothneighborsdistances;
	Titemarray<CSGVector> corrs;
	double d;

	for (int vnr1=0; vnr1<vertices.G_count(); vnr1++)
	{
		smoothneighbors.add(new Tcheapcopyarray<CSGVertex>);
		smoothneighborsdistances.add(new Tdoublearray);
	}

	for (int vnr1=0; vnr1<vertices.G_count(); vnr1++)
		for (int vnr2=0; vnr2<vnr1; vnr2++)
		{
			d=CSGVertex::G_distance(*vertices[vnr1],*vertices[vnr2]);
			if (d<maxdist)
			{
				smoothneighbors[vnr1]->add(vertices[vnr2]);smoothneighborsdistances[vnr1]->add(d);
				smoothneighbors[vnr2]->add(vertices[vnr1]);smoothneighborsdistances[vnr2]->add(d);
			}
		}


	for (int ct=0; ct<=5; ct++)
	{

		for (int vnr1=0; vnr1<vertices.G_count(); vnr1++)
		{
			CSGVertex *vertex=vertices[vnr1];
			Tcheapcopyarray<CSGVertex> &neighblist=*smoothneighbors[vnr1];
			Tdoublearray &distlist=*smoothneighborsdistances[vnr1];
			double cx=0,cy=0,cz=0,fs=0,f;
			for (int i=0; i<neighblist.G_count(); i++)
			{
				f=(maxdist-distlist[i])/maxdist;
				fs+=f;
				cx+=f*neighblist[i]->x;
				cy+=f*neighblist[i]->y;
				cz+=f*neighblist[i]->z;
			}
			fs+=0.01;
			cx+=0.01*vertex->x;
			cy+=0.01*vertex->y;
			cz+=0.01*vertex->z;
			corrs.add(CSGVector(cx/fs,cy/fs,cz/fs));
		}

		for (int vnr1=0; vnr1<vertices.G_count(); vnr1++)
		{
			CSGVertex *vertex=vertices[vnr1];
			vertex->x=(1-alpha)*vertex->x+alpha*corrs[vnr1].x;
			vertex->y=(1-alpha)*vertex->y+alpha*corrs[vnr1].y;
			vertex->z=(1-alpha)*vertex->z+alpha*corrs[vnr1].z;
		}

	}



	//average normals
	calcfacenormals();
	determine_memberfaces();
	for (int vnr1=0; vnr1<vertices.G_count(); vnr1++)
	{
		CSGVertex *vertex=vertices[vnr1];
		CSGVector norm;
		for (int fnr=0; fnr<vertex->memberfaces.G_count(); fnr++)
			norm.lincomb(norm,1,vertex->memberfaces[fnr]->G_Normal(),vertex->memberfaces[fnr]->area);
		norm.Normalise();
		for (int fnr=0; fnr<vertex->memberfaces.G_count(); fnr++)
		{
			CSGFace *face=vertex->memberfaces[fnr];
			if (face->v1==vertex) face->n1=norm;
			if (face->v2==vertex) face->n2=norm;
			if (face->v3==vertex) face->n3=norm;
		}
	}
	clear_memberfaces();

}


void CSGShape::determine_memberfaces()
{
	for (int i=0; i<vertices.G_count(); i++)
		vertices[i]->memberfaces.reset();

	for (int i=0; i<faces.G_count(); i++)
	{
		faces[i].v1->memberfaces.add(&faces[i]);
		faces[i].v2->memberfaces.add(&faces[i]);
		faces[i].v3->memberfaces.add(&faces[i]);
	}
}

void CSGShape::clear_memberfaces()
{
	for (int i=0; i<vertices.G_count(); i++)
		vertices[i]->memberfaces.reset();
}


void CSGShape::determine_vertexneighbors()
{
	for (int i=0; i<G_vertexcount(); i++)
		vertices[i]->neighborvertices.reset();
	for(int i=0;i<G_facecount();i++)
	{
		CSGFace &face=G_face(i);
		face.v1->add_neighbor(face.v2);
		face.v1->add_neighbor(face.v3);
		face.v2->add_neighbor(face.v1);
		face.v2->add_neighbor(face.v3);
		face.v3->add_neighbor(face.v1);
		face.v3->add_neighbor(face.v2);
	}
}

void CSGShape::forget_vertexneighbors()
{
	for (int i=0; i<G_vertexcount(); i++)
		vertices[i]->neighborvertices.reset();
}


void CSGShape::transform(const Taffinetransformation &tf)
{
	Tvertex ps,ps2;
	Tvector vc,vc2;
	for (int i=0; i<vertices.G_count(); i++)
	{
		ps.Set3(vertices[i]->x,vertices[i]->y,vertices[i]->z);
		tf.mul(&ps,&ps2);
		vertices[i]->x=ps2.G3_x();
		vertices[i]->y=ps2.G3_y();
		vertices[i]->z=ps2.G3_z();
	}
	for (int i=0; i<faces.G_count(); i++)
	{
		vc.copyfrom(faces[i].n1);  tf.mulnormvec(&vc,&vc2);  faces[i].n1=CSGVector(vc2); faces[i].n1.Normalise();
		vc.copyfrom(faces[i].n2);  tf.mulnormvec(&vc,&vc2);  faces[i].n2=CSGVector(vc2); faces[i].n2.Normalise();
		vc.copyfrom(faces[i].n3);  tf.mulnormvec(&vc,&vc2);  faces[i].n3=CSGVector(vc2); faces[i].n3.Normalise();
	}
}


void CSGShape::warp(Tspacewarper &warper)
{
	Tvertex ps,ps2;
	Tvector vc,vc2;
	for (int i=0; i<vertices.G_count(); i++)
	{
		ps.Set3(vertices[i]->x,vertices[i]->y,vertices[i]->z);
		warper.warpvertex(&ps,&ps2);
		vertices[i]->x=ps2.G3_x();
		vertices[i]->y=ps2.G3_y();
		vertices[i]->z=ps2.G3_z();
	}
	for (int i=0; i<faces.G_count(); i++)
	{
		ps.Set3(faces[i].v1->x,faces[i].v1->y,faces[i].v1->z);
		vc.copyfrom(faces[i].n1);  warper.warpnormal(&ps,&vc,&vc2);  faces[i].n1=CSGVector(vc2);
		ps.Set3(faces[i].v2->x,faces[i].v2->y,faces[i].v2->z);
		vc.copyfrom(faces[i].n2);  warper.warpnormal(&ps,&vc,&vc2);  faces[i].n2=CSGVector(vc2);
		ps.Set3(faces[i].v3->x,faces[i].v3->y,faces[i].v3->z);
		vc.copyfrom(faces[i].n3);  warper.warpnormal(&ps,&vc,&vc2);  faces[i].n3=CSGVector(vc2);
	}
}

int intersect_triangle(double orig[3], double dir[3],
                   double vert0[3], double vert1[3], double vert2[3],
                   double *t, double *u, double *v);


bool CSGShape::Intersect(const Tvertex &raypos, const Tvector &raydir, double &t0, Tvector &normdir)
{
	double orig[3],dir[3];
	double vert0[3],vert1[3],vert2[3];
	double t,u,v;

	t0=1.0e99;

	orig[0]=raypos.G3_x();orig[1]=raypos.G3_y();orig[2]=raypos.G3_z();
	dir[0]=raydir.G_x();dir[1]=raydir.G_y();dir[2]=raydir.G_z();

	CSGVector nm;

	for (int i=0; i<faces.G_count(); i++)
	{
		vert0[0]=faces[i].v1->x;vert0[1]=faces[i].v1->y;vert0[2]=faces[i].v1->z;
		vert1[0]=faces[i].v2->x;vert1[1]=faces[i].v2->y;vert1[2]=faces[i].v2->z;
		vert2[0]=faces[i].v3->x;vert2[1]=faces[i].v3->y;vert2[2]=faces[i].v3->z;
		if (intersect_triangle(orig,dir,
                   vert0,vert1,vert2,
                   &t,&u,&v))
		{
			if ((t>=0)&&(t<t0))
			{
				t0=t;
				nm.lincomb(faces[i].n1,1-u-v,faces[i].n2,u,faces[i].n3,v);
			}
		}
	}

	nm.Normalise();
	normdir.copyfrom(nm);
	return t0<1.0e99;
}


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_create_bar,Bar)
{
	setclasspath_fromtype(SC_valname_shape);
	setreturntype(SC_valname_shape);
	addvar(_qstr("position"),SC_valname_vertex);
	addvar(_qstr("size"),SC_valname_vector);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(retval);
	Tvertex *pos=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *size=G_valuecontent<Tvector>(arglist->get(1));
	shape->createbar(CSGVector(*pos),CSGVector(*size));
}
ENDFUNCTION(func_CSG_create_bar)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_create_sphere,Sphere)
{
	setclasspath_fromtype(SC_valname_shape);
	setreturntype(SC_valname_shape);
	addvar(_qstr("center"),SC_valname_vertex);
	addvar(_qstr("radius"),SC_valname_scalar);
	addvar(_qstr("resolution"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(retval);
	Tvertex *center=G_valuecontent<Tvertex>(arglist->get(0));
	double radius=arglist->get(1)->G_content_scalar()->G_val();
	int resol=15;
	if (arglist->G_ispresent(2)) resol=arglist->get(2)->G_content_scalar()->G_intval();
	shape->createsphere(CSGVector(*center),radius,resol);
}
ENDFUNCTION(func_CSG_create_sphere)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_create_cylinder,Cylinder)
{
	setclasspath_fromtype(SC_valname_shape);
	setreturntype(SC_valname_shape);
	addvar(_qstr("position"),SC_valname_vertex);
	addvar(_qstr("axis"),SC_valname_vector);
	addvar(_qstr("radius"),SC_valname_scalar);
	addvar(_qstr("resolution"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(retval);
	Tvertex *pos=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *axis=G_valuecontent<Tvector>(arglist->get(1));
	double radius=arglist->get(2)->G_content_scalar()->G_val();
	int resol=25;
	if (arglist->G_ispresent(3)) resol=arglist->get(3)->G_content_scalar()->G_intval();
	shape->createcylinder(CSGVector(*pos),CSGVector(*axis),radius,resol);
}
ENDFUNCTION(func_CSG_create_cylinder)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_create_polyhedron,Polyhedron)
{
	setclasspath_fromtype(SC_valname_shape);
	setreturntype(SC_valname_shape);
	addvar(_qstr("planes"),SC_valname_list);
	addvar(_qstr("indices"),SC_valname_list,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(retval);
	TSC_list *planes=G_valuecontent<TSC_list>(arglist->get(0));
	TSC_list *indices=NULL;
	if (arglist->G_ispresent(1)) indices=G_valuecontent<TSC_list>(arglist->get(1));
	Tcopyarray<Tplane> planelist;
	Tintarray indexlist;
	for (int i=0; i<planes->G_size(); i++)
	{
		planelist.add(G_valuecontent<Tplane>(planes->get(i)));
		if (indices!=NULL) indexlist.add(indices->get(i)->G_content_scalar()->G_intval());
	}
	shape->create_polyhedron(planelist,indexlist);
}
ENDFUNCTION(func_CSG_create_polyhedron)




///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_create_extrudedshape,ExtrudedShape)
{
	setclasspath_fromtype(SC_valname_shape);
	setreturntype(SC_valname_shape);

	addvar(_qstr("contour"),SC_valname_2dcontourset);
	addvar(_qstr("height"),SC_valname_scalar);
	addvar(_qstr("layercount"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(retval);
	int layercount=2;
	if (arglist->G_ispresent(2)) layercount=arglist->get(2)->G_content_scalar()->G_intval();
	shape->create_extrudedpolygon(G_valuecontent<T2DContourset>(arglist->get(0)),
		arglist->get(1)->G_content_scalar()->G_val(),layercount);
}
ENDFUNCTION(func_CSG_create_extrudedshape)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_create_revolvedshape,RevolvedShape)
{
	setclasspath_fromtype(SC_valname_shape);
	setreturntype(SC_valname_shape);

	addvar(_qstr("contour"),SC_valname_2dcontourset);
	addvar(_qstr("resolution"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(retval);
	int resolution=30;
	if (arglist->G_ispresent(1)) resolution=arglist->get(1)->G_content_scalar()->G_intval();
	shape->create_revolvedpolygon(G_valuecontent<T2DContourset>(arglist->get(0)),resolution);
}
ENDFUNCTION(func_CSG_create_revolvedshape)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_create_conalshape,ConeShape)
{
	setclasspath_fromtype(SC_valname_shape);
	setreturntype(SC_valname_shape);

	addvar(_qstr("contour"),SC_valname_2dcontourset);
	addvar(_qstr("top"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(retval);
	shape->create_conepolygon(G_valuecontent<T2DContourset>(arglist->get(0)),
		*G_valuecontent<Tvertex>(arglist->get(1)));
}
ENDFUNCTION(func_CSG_create_conalshape)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_addface,AddFace)
{
	setmemberfunction(SC_valname_shape);

	addvar(_qstr("vertices"),SC_valname_list);
	addvar(_qstr("label"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(owner);
	TSC_list *list=G_valuecontent<TSC_list>(arglist->get(0));
	Tcopyarray<Tvertex> vertexlist;
	for (int i=0; i<list->G_size(); i++)
		vertexlist.add(G_valuecontent<Tvertex>(list->get(i)));
	int label=arglist->get(1)->G_content_scalar()->G_intval();
	shape->addpolygon(vertexlist,label);
}
ENDFUNCTION(func_CSG_addface)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_setlabel,SetLabel)
{
	setmemberfunction(SC_valname_shape);
	addvar(_qstr("label"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(owner);
	int label=arglist->get(0)->G_content_scalar()->G_intval();
	shape->SetFaceLabels(label);
}
ENDFUNCTION(func_CSG_setlabel)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_subsample,SubSample)
{
	setmemberfunction(SC_valname_shape);
	addvar(_qstr("maxdist"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(owner);
	double maxdist=arglist->get(0)->G_content_scalar()->G_val();
	shape->subsample(maxdist);
}
ENDFUNCTION(func_CSG_subsample)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_createflatnormals,CreateFlatNormals)
{
	setmemberfunction(SC_valname_shape);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(owner);
	shape->createflatnormals();
}
ENDFUNCTION(func_CSG_createflatnormals)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_smoothen,Smoothen)
{
	setmemberfunction(SC_valname_shape);
	addvar(_qstr("maxdist"),SC_valname_scalar);
	addvar(_qstr("factor"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(owner);
	double maxdist=arglist->get(0)->G_content_scalar()->G_val();
	double factor=arglist->get(1)->G_content_scalar()->G_val();
	shape->smoothen(maxdist,factor);
}
ENDFUNCTION(func_CSG_smoothen)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_transform,Transform)
{
	setmemberfunction(SC_valname_shape);
	addvar(_qstr("tf"),SC_valname_affinetransformation);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(owner);
	Taffinetransformation *tf=G_valuecontent<Taffinetransformation>(arglist->get(0));
	shape->transform(*tf);
}
ENDFUNCTION(func_CSG_transform)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_warpspiral,WarpSpiral)
{
	setmemberfunction(SC_valname_shape);
	addvar(_qstr("winding"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(owner);
	double winding=arglist->get(0)->G_content_scalar()->G_val();
	Tspiraler w(winding);
	shape->warp(w);
}
ENDFUNCTION(func_CSG_warpspiral)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_warpconal,WarpConalPinch)
{
	setmemberfunction(SC_valname_shape);
	addvar(_qstr("height"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(owner);
	double h=arglist->get(0)->G_content_scalar()->G_val();
	Tconalpincher w(h);
	shape->warp(w);
}
ENDFUNCTION(func_CSG_warpconal)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_warpcustom,WarpCustom)
{
	setmemberfunction(SC_valname_shape);
	addvar(_qstr("warpfunction"),SC_valname_functor);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(owner);
	TSC_functor *functor=G_valuecontent<TSC_functor>(arglist->get(0));
	Tcustomwarper w(functor);
	shape->warp(w);
}
ENDFUNCTION(func_CSG_warpcustom)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_CSG_intersect,Intersect)
{
	setmemberfunction(SC_valname_shape);
	addvar(_qstr("RayOrigin"),SC_valname_vertex);
	addvar(_qstr("RayDirection"),SC_valname_vector);

	addvar(_qstr("ReturnDistance"),SC_valname_scalar);
	addvar(_qstr("ReturnNormal"),SC_valname_vector);

	setreturntype(SC_valname_boolean);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(owner);
	Tvertex *raypos=G_valuecontent<Tvertex>(arglist->get(0));
	Tvector *raydir=G_valuecontent<Tvector>(arglist->get(1));
	Tvector normdir;
	double t;
	bool rs=shape->Intersect(*raypos,*raydir,t,normdir);
	if (rs)
	{
		arglist->get(2)->G_content_scalar()->copyfrom(t);
		(G_valuecontent<Tvector>(arglist->get(3)))->copyfrom(normdir);
	}
	retval->G_content_boolean()->copyfrom(rs);

}
ENDFUNCTION(func_CSG_intersect)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_plus_CSG,8,SC_valname_shape,SC_valname_shape,SC_valname_shape)
{
	setclasspath_fromtype(SC_valname_shape);
	description=_text("Determines the union of the first shape and the second shape");
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	CSGShape *v1=G_valuecontent<CSGShape>(val1);
	CSGShape *v2=G_valuecontent<CSGShape>(val2);
	CSGShapeCombiner comb(*v1,*v2);
	comb.getUnion(*G_valuecontent<CSGShape>(retval));
}
ENDOPERATOR(op_plus_CSG)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_min_CSG,9,SC_valname_shape,SC_valname_shape,SC_valname_shape)
{
	setclasspath_fromtype(SC_valname_shape);
	description=_text("Subtracts the volume occupied by the second shape from the first shape");
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	CSGShape *v1=G_valuecontent<CSGShape>(val1);
	CSGShape *v2=G_valuecontent<CSGShape>(val2);
	CSGShapeCombiner comb(*v1,*v2);
	comb.getDifference(*G_valuecontent<CSGShape>(retval));
}
ENDOPERATOR(op_min_CSG)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_mul_CSG,10,SC_valname_shape,SC_valname_shape,SC_valname_shape)
{
	setclasspath_fromtype(SC_valname_shape);
	description=_text("Determines the intersection between the first shape and the second shape");
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	CSGShape *v1=G_valuecontent<CSGShape>(val1);
	CSGShape *v2=G_valuecontent<CSGShape>(val2);
	CSGShapeCombiner comb(*v1,*v2);
	comb.getIntersection(*G_valuecontent<CSGShape>(retval));
}
ENDOPERATOR(op_mul_CSG)
