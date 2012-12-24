#include"stdafx.h"
#include<math.h>

#include "opengl.h"

#include"tools.h"
#include "3Dmesh.h"
#include "3DSpaceWarper.h"
#include "3Dscene.h"

#include "CSG_shape.h"


void debugmessage(const char* content);

//***************************************************************************
// Tvec3d
//***************************************************************************

void Tvec3d::lincomb(Tvec3d &pt1, double f1)
{
	x=pt1.x*(V3DFLOAT)f1;
	y=pt1.y*(V3DFLOAT)f1;
	z=pt1.z*(V3DFLOAT)f1;
}


void Tvec3d::lincomb(Tvec3d &pt1, Tvec3d &pt2, double f1, double f2)
{
	x=pt1.x*(V3DFLOAT)f1+pt2.x*(V3DFLOAT)f2;
	y=pt1.y*(V3DFLOAT)f1+pt2.y*(V3DFLOAT)f2;
	z=pt1.z*(V3DFLOAT)f1+pt2.z*(V3DFLOAT)f2;
}

void Tvec3d::lincomb(Tvec3d &pt1, Tvec3d &pt2, Tvec3d &pt3, double f1, double f2, double f3)
{
	x=pt1.x*(V3DFLOAT)f1+pt2.x*(V3DFLOAT)f2+pt3.x*(V3DFLOAT)f3;
	y=pt1.y*(V3DFLOAT)f1+pt2.y*(V3DFLOAT)f2+pt3.y*(V3DFLOAT)f3;
	z=pt1.z*(V3DFLOAT)f1+pt2.z*(V3DFLOAT)f2+pt3.z*(V3DFLOAT)f3;
}

void Tvec3d::lincomb(Tvec3d &pt1, Tvec3d &pt2, Tvec3d &pt3, Tvec3d &pt4, double f1, double f2, double f3, double f4)
{
	x=pt1.x*(V3DFLOAT)f1+pt2.x*(V3DFLOAT)f2+pt3.x*(V3DFLOAT)f3+pt4.x*(V3DFLOAT)f4;
	y=pt1.y*(V3DFLOAT)f1+pt2.y*(V3DFLOAT)f2+pt3.y*(V3DFLOAT)f3+pt4.y*(V3DFLOAT)f4;
	z=pt1.z*(V3DFLOAT)f1+pt2.z*(V3DFLOAT)f2+pt3.z*(V3DFLOAT)f3+pt4.z*(V3DFLOAT)f4;
}



void Tvec3d::vecprod(Tvec3d &pt1, Tvec3d &pt2)
{
	x= ( + pt1.y * pt2.z   - pt2.y * pt1.z );
	y= ( + pt1.z * pt2.x   - pt2.z * pt1.x );
	z= ( + pt1.x * pt2.y   - pt2.x * pt1.y );
}

void Tvec3d::norm()
{
	V3DFLOAT r=x*x+y*y+z*z;
	if (r>0)
	{
		r=sqrt(r);
		x/=r;y/=r;z/=r;
	}
}

void Tvec3d::multiply(double fc)
{
	x*=(V3DFLOAT)fc;
	y*=(V3DFLOAT)fc;
	z*=(V3DFLOAT)fc;
}

void Tvec3d::rotate(Tvec3d &vec, Tvec3d &norm, double ang)
{
	norm.norm();
	Tvec3d dirx,diry;
	diry.vecprod(norm,vec);
	if ((diry.x==0)&&(diry.y==0)&&(diry.z==0)) return;
	diry.norm();
	dirx.vecprod(diry,norm);
	dirx.norm();
	
	Tvec3d part1;
	part1.lincomb(dirx,diry,dotprod(dirx,vec)*cos(ang),dotprod(dirx,vec)*sin(ang));
	lincomb(part1,norm,1,dotprod(norm,vec));
}

double Tvec3d::getsize()
{
	return sqrt(x*x+y*y+z*z);
}



double Tvec3d::dotprod(Tvec3d &pt1, Tvec3d &pt2)
{
	return pt1.x*pt2.x+pt1.y*pt2.y+pt1.z*pt2.z;
}


double Tvec3d::distance(Tvec3d &pt1, Tvec3d &pt2)
{
	return sqrt((pt1.x-pt2.x)*(pt1.x-pt2.x)+(pt1.y-pt2.y)*(pt1.y-pt2.y)+(pt1.z-pt2.z)*(pt1.z-pt2.z));
}

Tvec3d& Tvec3d::operator=(const Tvec3d &inp)
{
	x=inp.x;y=inp.y;z=inp.z;
	return *this;
}

Tvec3d& Tvec3d::operator=(const Tvector &inp)
{
	x=inp.G_x();y=inp.G_y();z=inp.G_z();
	return *this;
}

Tvec3d& Tvec3d::operator=(const Tvertex &inp)
{
	x=inp.G3_x();y=inp.G3_y();z=inp.G3_z();
	return *this;
}

void Tvec3d::copyfrom(const Tvertex &inp)
{
	x=inp.G3_x();y=inp.G3_y();z=inp.G3_z();
}

/*
void streamout_vec3darray(QBinTagWriter &writer, Titemarray<Tvec3d> &arr)
{
	writer.write_int32(arr.G_count());
	writer.write_block(arr.G_ptr(),3*sizeof(V3DFLOAT)*arr.G_count());
}

void streamout_4colorarray(QBinTagWriter &writer, Titemarray<T4color> &arr)
{
	writer.write_int32(arr.G_count());
	writer.write_block(arr.G_ptr(),4*sizeof(V3DFLOAT)*arr.G_count());
}



void streamout_2texturearray(QBinTagWriter &writer, Titemarray<T2textureidx> &arr)
{
	writer.write_int32(arr.G_count());
	writer.write_block(arr.G_ptr(),2*sizeof(V3DFLOAT)*arr.G_count());
}


void streamin_vec3darray(QBinTagReader &reader, Titemarray<Tvec3d> &arr)
{
	arr.reset();
	int len=reader.read_int32();
	arr.resize(len);
	reader.read_block(arr.G_ptr(),3*sizeof(V3DFLOAT)*arr.G_count());
}

void streamin_4colorarray(QBinTagReader &reader, Titemarray<T4color> &arr)
{
	arr.reset();
	int len=reader.read_int32();
	arr.resize(len);
	reader.read_block(arr.G_ptr(),4*sizeof(V3DFLOAT)*arr.G_count());
}


void streamin_2texturearray(QBinTagReader &reader, Titemarray<T2textureidx> &arr)
{
	arr.reset();
	int len=reader.read_int32();
	arr.resize(len);
	reader.read_block(arr.G_ptr(),2*sizeof(V3DFLOAT)*arr.G_count());
}
*/


//***************************************************************************
// Ttriangle
//***************************************************************************


Ttriangle::Ttriangle(int v1, int v2, int v3)
{
	label=0;
	vertices[0]=v1;vertices[1]=v2;vertices[2]=v3;
}

Ttriangle& Ttriangle::operator=(const Ttriangle &inp)
{
	vertices[0]=inp.vertices[0];vertices[1]=inp.vertices[1];vertices[2]=inp.vertices[2];
	lightfacing=inp.lightfacing;
	facenormal=inp.facenormal;
	return *this;
}

//***************************************************************************
// Tedge
//***************************************************************************

Tedge& Tedge::operator=(const Tedge &inp)
{
	vertices[0]=inp.vertices[0];vertices[1]=inp.vertices[1];
	triangles[0]=inp.triangles[0];triangles[1]=inp.triangles[1];
	return *this;
}


//***************************************************************************
// T3Dmesh
//***************************************************************************


T3Dmesh::T3Dmesh()
{
	hasvertexnormals=true;
	strips_rowsize=0;
	buffered=false;
	hasowncolors=false;
	needcalcedges=true;
}

void T3Dmesh::reset()
{
	hasvertexnormals=true;
	vertices.reset();
	vertices_normals.reset();
	vertices_label.reset();
	vertices_colL.reset();
	vertices_colR.reset();
	triangles.reset();
	trianglesindices.reset();
	edges.reset();
	strips_rowsize=0;
	buffered=false;
	hasowncolors=false;
	buffer_vertices.dispose();
	buffer_normals.dispose();
	buffer_texture.dispose();
	buffer_elements.dispose();
	buffer_colorsL.dispose();
	buffer_colorsR.dispose();
}

T3Dmesh::~T3Dmesh()
{
	buffer_vertices.dispose();
	buffer_normals.dispose();
	buffer_texture.dispose();
	buffer_elements.dispose();
	buffer_colorsL.dispose();
	buffer_colorsR.dispose();
}



void T3Dmesh::loadfile(const char *filename)
{
/*	FILE *fp;
	char css[10000],css2[1000];
	int i,j,i1,i2,i3,i4,ct;
	double rd;
	Tvec3d pt;

	Tarray<CString> tagdata;
	Tbaltree tagidx;

	fp=fopen(filename,"r");
	if (fp==NULL)
	{
		adderror("Unable to find mesh file");
		return;
	}

	char headerfinished=0;
	while (!headerfinished)
	{
		fgets(css,9999,fp);
		for (i=(int)strlen(css)-3; i<(int)strlen(css); i++) if ((css[i]==10)||(css[i]==13)) css[i]=0;
		Ttoken tok1(css,'=');
		if (stricmp(css,"DATA")==0) headerfinished=1;
		tagidx.add(tok1.getnext(),tagdata.G_count());
		tagdata.add(new CString(tok1.getnext()));
	}

	if (tagidx.get("FORMAT")<0)
	{
		adderror("Invalid mesh file format");
		fclose(fp);return;
	}

	char formatfound=0;

	if (stricmp(*tagdata[tagidx.get("FORMAT")],"DEFORMEDSPHERE")==0)
	{
		formatfound=1;
		int countlong,countlatt;
		if ((tagidx.get("COUNTLONG")<0)||(tagidx.get("COUNTLATT")<0))
		{
			adderror("Invalid DEFORMEDSPHERE mesh file format");
			fclose(fp);return;
		}
		countlong=atoi(*tagdata[tagidx.get("COUNTLONG")]);
		countlatt=atoi(*tagdata[tagidx.get("COUNTLATT")]);
		for (i=0; i<=countlong; i++)
			for (j=0; j<=countlatt; j++)
			{
				fgets(css,999,fp);rd=atof(css);
				pt.x=rd*cos(i*1.0/countlong*2*Pi)*cos((j-countlatt/2.0)/countlatt*Pi);
				pt.y=rd*sin(i*1.0/countlong*2*Pi)*cos((j-countlatt/2.0)/countlatt*Pi);
				pt.z=rd*sin((j-countlatt/2.0)/countlatt*Pi);
				addvertex(&pt,(i%countlong)*(countlatt+1)+j,NULL,i*1.0/countlong,j*1.0/countlatt);
			}
		for (i=0; i<countlong; i++)
			for (j=0; j<countlatt; j++)
			{
				i1=(i+0)*(countlatt+1)+j+0;
				i2=((i+1))*(countlatt+1)+j+0;
				i3=((i+1))*(countlatt+1)+(j+1);
				i4=(i+0)*(countlatt+1)+(j+1);
				addtriangle(i1,i2,i3);
				addtriangle(i3,i4,i1);
			}
		calcnormals();
		calcsmoothnormals();
		calcedges();
	}


	if (stricmp(*tagdata[tagidx.get("FORMAT")],"VERTEXFACES")==0)
	{
		int oo=0;
		if (tagidx.get("INDEX_START")>=0)
			oo=atoi(*tagdata[tagidx.get("INDEX_START")]);

		formatfound=1;
		ct=0;
		while (!feof(fp))
		{
			fgets(css,9999,fp);
			if (css[0]=='v')
			{
				sscanf(css,"%s %lf %lf %lf",css2,&pt.x,&pt.y,&pt.z);
				addvertex(&pt,ct);ct++;
			}
			if (css[0]=='f')
			{
				sscanf(css,"%s %d %d %d",css2,&i1,&i2,&i3);
				addtriangle(i1-oo,i2-oo,i3-oo);
			}
		}
		calcnormals();
		calcsmoothnormals();
		calcedges();
	}


	if (!formatfound) adderror("unknown mesh file format");

	fclose(fp);*/
}


int T3Dmesh::addvertex(Tvec3d *v, int vertexidx, Tvec3d *normal, double texcoord1, double texcoord2)
{
	vertices.add(*v);
	vertices_idx.add(vertexidx);
	if (normal==NULL) { vertices_normals.add(zeronormal); hasvertexnormals=false; }
	else vertices_normals.add(*normal);
	vertices_texcoord.add(T2textureidx(texcoord1,texcoord2));
	vertices_colL.add(T4color(1,1,1,1));vertices_colL[vertices_colL.G_count()-1].ColMapL();
	vertices_colR.add(T4color(1,1,1,1));vertices_colL[vertices_colL.G_count()-1].ColMapR();
	vertices_label.add(0);
	return vertices.G_count()-1;
}

int T3Dmesh::addvertex(Tvertex *v, int vertexidx, Tvector *normal, double texcoord1, double texcoord2)
{
	Tvec3d v1,v2;
	v1.set(v->G3_x(),v->G3_y(),v->G3_z());
	if (normal!=NULL)
	{
		v2.set(normal->G_x(),normal->G_y(),normal->G_z());
		return addvertex(&v1,vertexidx,&v2,texcoord1,texcoord2);
	}
	else
		return addvertex(&v1,vertexidx,NULL,texcoord1,texcoord2);
}


int T3Dmesh::addcolorvertex(Tvec3d *v, int vertexidx, Tvec3d *normal, double r, double g, double b, double a)
{
	vertices.add(*v);
	vertices_idx.add(vertexidx);
	if (normal==NULL) { vertices_normals.add(zeronormal); hasvertexnormals=false; }
	else vertices_normals.add(*normal);
	vertices_texcoord.add(T2textureidx(0,0));
	vertices_colL.add(T4color(r,g,b,a));vertices_colL[vertices_colL.G_count()-1].ColMapL();
	vertices_colR.add(T4color(r,g,b,a));vertices_colR[vertices_colL.G_count()-1].ColMapR();
	vertices_label.add(0);
	hasowncolors=true;
	return vertices.G_count()-1;
}

int T3Dmesh::addcolorvertex(Tvertex *v, int vertexidx, Tvector *normal, double r, double g, double b, double a)
{
	Tvec3d v1,v2;
	v1.set(v->G3_x(),v->G3_y(),v->G3_z());
	hasowncolors=true;
	if (normal!=NULL)
	{
		v2.set(normal->G_x(),normal->G_y(),normal->G_z());
		return addcolorvertex(&v1,vertexidx,&v2,r,g,b,a);
	}
	else
		return addcolorvertex(&v1,vertexidx,NULL,r,g,b,a);
}

void T3Dmesh::Set_vertexcolor(int vertexnr, double r, double g, double b, double a)
{
	if ((vertexnr<0)||(vertexnr>=vertices.G_count())) return;
	vertices_colL[vertexnr]=T4color(r,g,b,a);vertices_colL[vertexnr].ColMapL();
	vertices_colR[vertexnr]=T4color(r,g,b,a);vertices_colR[vertexnr].ColMapR();
	hasowncolors=true;
}



void T3Dmesh::addtriangle(int i1, int i2, int i3)
{
	ASSERT(i1>=0);ASSERT(i1<vertices.G_count());
	ASSERT(i2>=0);ASSERT(i2<vertices.G_count());
	ASSERT(i3>=0);ASSERT(i3<vertices.G_count());
	triangles.add(Ttriangle(i1,i2,i3));
	trianglesindices.add(i1);trianglesindices.add(i2);trianglesindices.add(i3);
//	trianglesindices.add((unsigned __int16)i1);trianglesindices.add((unsigned __int16)i2);trianglesindices.add((unsigned __int16)i3);
}

void T3Dmesh::addquad(int i1, int i2, int i3, int i4)
{
	addtriangle(i1,i2,i3);
	addtriangle(i3,i4,i1);
}


void T3Dmesh::Make_stripbased(int irowsize, bool iclosed)
{
	int i,j,i1,i2,j1,j2;

	calccenter();
	ASSERT(hasvertexnormals);
	if (!hasvertexnormals)
		throw QError(_text("Strip based mesh should have vertex normals defined"));

	if (irowsize==0) return;

	strips_rowsize=irowsize;
	strips_closed=iclosed;

	int colsize=vertices.G_count()/irowsize;

	int colsize2=colsize-1;
	if (strips_closed)
	{
		for (j=0; j<strips_rowsize; j++)
			vertices_idx[(colsize-1)*strips_rowsize+j]=vertices_idx[j];
	}

	for (i=0; i<colsize2; i++)
	{
		i1=i;i2=(i+1)%colsize;
		for (j=0; j<strips_rowsize-1; j++)
		{
			j1=j; j2=j+1;
			addtriangle(i1*strips_rowsize+j2,i2*strips_rowsize+j2,i2*strips_rowsize+j1);
			addtriangle(i1*strips_rowsize+j2,i2*strips_rowsize+j1,i1*strips_rowsize+j1);
		}
	}

	calcnormals();

	calcedges();
}


void T3Dmesh::createsphere(Tvec3d &cent, double radius, int resol)
{
	int i,j,i1,i2,j1,j2;
	double al1,al2;

	Tvec3d norm,vertex;

	int ct=0;
	for (i=0; i<=2*resol-1; i++)
	{
		al1=(i*1.0)/resol*Pi;
		for (j=0; j<=resol; j++)
		{
			al2=(j*1.0)/resol*Pi;
			norm.x=(V3DFLOAT)(sin(al2)*cos(al1));
			norm.y=(V3DFLOAT)(sin(al2)*sin(al1));
			norm.z=(V3DFLOAT)(cos(al2));
			vertex.lincomb(cent,norm,1,radius);
			addvertex(&vertex,ct,&norm,al1/(2*Pi)+0.5,1.0-al2/Pi);
			ct++;
		}
	}

	for (i=0; i<=2*resol-1; i++)
	{
		i1=i;i2=(i+1)%(2*resol);
		for (j=0; j<resol; j++)
		{
			j1=j; j2=j+1;
			addtriangle(i1*(resol+1)+j2,i2*(resol+1)+j2,i2*(resol+1)+j1);
			addtriangle(i1*(resol+1)+j2,i2*(resol+1)+j1,i1*(resol+1)+j1);
		}
	}

	calcnormals();
	calcedges();
	hasowncolors=false;
}


void T3Dmesh::copyfrom(const CSGShape &shape)
{
	reset();

	Titemarray<CSGVector> tmp_normals;
	for (int i=0; i<shape.G_vertexcount(); i++)
	{
		addvertex(&Tvec3d(shape.G_vertex(i).x,shape.G_vertex(i).y,shape.G_vertex(i).z),i);
		tmp_normals.add(CSGVector(0,0,0));
		vertices_label[vertices.G_count()-1]=-1;
	}

	for (int i=0; i<shape.G_facecount(); i++)
	{
		const CSGFace &face=shape.G_face(i);
		CSGVertex* vs[3];
		const CSGVector *ns[3];
		int idxs[3];
		vs[0]=face.v1;vs[1]=face.v2;vs[2]=face.v3;
		ns[0]=&face.n1;ns[1]=&face.n2;ns[2]=&face.n3;
		for (int j=0; j<3; j++)
		{
			int oidx=vs[j]->idx;idxs[j]=oidx;
			bool makenew=false;
			if ((vertices_label[oidx]>=0)&&(face.label!=vertices_label[oidx])) makenew=true;
			if (!ns[j]->IsZero())
			{
				if (!tmp_normals[oidx].IsZero())
				{
					if (!ns[j]->IsSame(tmp_normals[oidx])) makenew=true;
				}
			}

			if (makenew)
			{
				Tvec3d v0=vertices[oidx];
				idxs[j]=addvertex(&v0,oidx);
				vertices_label[idxs[j]]=face.label;
				vertices_normals[idxs[j]].set(ns[j]->x,ns[j]->y,ns[j]->z);
			}
			else
			{
				tmp_normals[oidx]=*ns[j];
				vertices_label[oidx]=face.label;
				vertices_normals[oidx].set(ns[j]->x,ns[j]->y,ns[j]->z);
			}
		}
		addtriangle(idxs[0],idxs[1],idxs[2]);
	}


	hasvertexnormals=true;

	calcnormals();
	calcedges();
	hasowncolors=false;

}


void T3Dmesh::copytoframe(double iphase)
{
	T3Dmesh_Frame *frame=new T3Dmesh_Frame;
	frame->phase=iphase;
	frames.add(frame);

	for (int i=0; i<vertices.G_count(); i++)
	{
		frame->vertices.add(vertices[i]);
		frame->vertices_normals.add(vertices_normals[i]);
		frame->vertices_texcoord.add(vertices_texcoord[i]);
		if (hasowncolors)
		{
			frame->vertices_colL.add(vertices_colL[i]);
			frame->vertices_colR.add(vertices_colR[i]);
		}
	}
}

void T3Dmesh::interpolframe(double phase)
{
	phase=phase-floor(phase);

	double fr1,fr2;
	if (frames.G_count()<2) throw QError(_text("No frames present for this object"));
	int i1=0;
	while ((i1<frames.G_count()-1)&&(phase>frames[i1+1]->phase)) i1++;
	int i2=i1+1;
	if (i2<frames.G_count())
	{
		if (frames[i2]->phase<=frames[i1]->phase) throw QError(_text("Invalid frame phases"));
		fr2=(phase-frames[i1]->phase)/(frames[i2]->phase-frames[i1]->phase);
		fr1=1-fr2;
	}
	else
	{
		i2=0;
		fr2=(phase-frames[i1]->phase)/(1+frames[i2]->phase-frames[i1]->phase);
		fr1=1-fr2;
	}
	T3Dmesh_Frame *frame1=frames[i1];
	T3Dmesh_Frame *frame2=frames[i2];
	if (vertices.G_count()!=frame1->vertices.G_count()) throw QError(_text("Inconsistent frame dimensions"));
	if (vertices.G_count()!=frame2->vertices.G_count()) throw QError(_text("Inconsistent frame dimensions"));
	if (hasowncolors)
	{
		if (vertices.G_count()!=frame1->vertices_colL.G_count()) throw QError(_text("Inconsistent frame color dimensions"));
		if (vertices.G_count()!=frame2->vertices_colL.G_count()) throw QError(_text("Inconsistent frame color dimensions"));
	}
	for (int i=0; i<vertices.G_count(); i++)
	{
		vertices[i].lincomb(frame1->vertices[i],frame2->vertices[i],fr1,fr2);
		vertices_normals[i].lincomb(frame1->vertices_normals[i],frame2->vertices_normals[i],fr1,fr2);
		vertices_texcoord[i].lincomb(frame1->vertices_texcoord[i],frame2->vertices_texcoord[i],fr1,fr2);
		if (hasowncolors)
		{
			vertices_colL[i].lincomb(frame1->vertices_colL[i],frame2->vertices_colL[i],fr1,fr2);
			vertices_colR[i].lincomb(frame1->vertices_colR[i],frame2->vertices_colR[i],fr1,fr2);
		}
	}
}

void T3Dmesh::streamout(QBinTagWriter &writer)
{
	{
		QBinTagWriter &ptag=writer.subtag(_qstr("Params")).G_obj();
		ptag.write_bool(buffered);
		ptag.write_bool(hasowncolors);
		ptag.write_bool(hasvertexnormals);
		ptag.write_bool(strips_closed);
		ptag.write_int32(strips_rowsize);
		for (int i=0; i<10; i++) ptag.write_bool(false);//buffer for future use
		for (int i=0; i<10; i++) ptag.write_int32(0);//buffer for future use
		ptag.close();
	}
	{
		QBinTagWriter &dtag=writer.subtag(_qstr("Data")).G_obj();
		vertices.streamout(dtag);
		vertices_normals.streamout(dtag);
		vertices_colL.streamout(dtag);
		vertices_colR.streamout(dtag);
		vertices_idx.streamout(dtag);
		vertices_texcoord.streamout(dtag);
		trianglesindices.streamout(dtag);
		triangles.streamout(dtag);
		edges.streamout(dtag);
		dtag.close();
	}

	{
		QBinTagWriter &ftag=writer.subtag(_qstr("Frames")).G_obj();
		ftag.write_int32(frames.G_count());
		for (int framenr=0; framenr<frames.G_count(); framenr++)
		{
			ftag.write_double(frames[framenr]->phase);
			frames[framenr]->vertices.streamout(ftag);
			frames[framenr]->vertices_normals.streamout(ftag);
			frames[framenr]->vertices_colL.streamout(ftag);
			frames[framenr]->vertices_colR.streamout(ftag);
			frames[framenr]->vertices_texcoord.streamout(ftag);
		}
		ftag.close();
	}

}

void T3Dmesh::streamin(QBinTagReader &reader)
{
	frames.reset();

	while (reader.hassubtags())
	{
		QBinTagReader &tg1=reader.getnextsubtag().G_obj();
		if (tg1.Istag(_qstr("Params")))
		{
			buffered=tg1.read_bool();
			hasowncolors=tg1.read_bool();
			hasvertexnormals=tg1.read_bool();
			strips_closed=tg1.read_bool();
			strips_rowsize=tg1.read_int32();
		}
		if (tg1.Istag(_qstr("Data")))
		{
			vertices.streamin(tg1);
			vertices_normals.streamin(tg1);
			vertices_colL.streamin(tg1);
			vertices_colR.streamin(tg1);
			vertices_idx.streamin(tg1);
			vertices_texcoord.streamin(tg1);
			trianglesindices.streamin(tg1);
			triangles.streamin(tg1);
			edges.streamin(tg1);
		}
		if (tg1.Istag(_qstr("Frames")))
		{
			int fcount=tg1.read_int32();
			for (int framenr=0; framenr<fcount; framenr++)
			{
				T3Dmesh_Frame *frame=new T3Dmesh_Frame;frames.add(frame);
				frame->phase=tg1.read_double();
				frame->vertices.streamin(tg1);
				frame->vertices_normals.streamin(tg1);
				frame->vertices_colL.streamin(tg1);
				frame->vertices_colR.streamin(tg1);
				frame->vertices_texcoord.streamin(tg1);
			}
		}
		tg1.close();
	}
}


void T3Dmesh::calccenter()
{
	centerpos.set(0,0,0);
	for (int i=0; i<vertices.G_count(); i++)
	{
		centerpos.x+=vertices[i].x;
		centerpos.y+=vertices[i].y;
		centerpos.z+=vertices[i].z;
	}
	centerpos.x/=vertices.G_count();
	centerpos.y/=vertices.G_count();
	centerpos.z/=vertices.G_count();
}


void T3Dmesh::warp(Tspacewarper &w)
{

	generateallvertexnormals();

	Tvertex vin,vout;
	Tvector nin,nout;
	for (int i=0; i<vertices.G_count(); i++)
	{
		vin.Set3(vertices[i].x,vertices[i].y,vertices[i].z);
		w.warpvertex(&vin,&vout);
		vertices[i]=vout;
		nin.Set(vertices_normals[i].x,vertices_normals[i].y,vertices_normals[i].z);
		w.warpnormal(&vin,&nin,&nout);
		vertices_normals[i]=nout;
	}

	calcnormals();
}


void T3Dmesh::calcnormals()
{
	int i,vi1,vi2,vi3;
	Tvec3d d1,d2;

	calccenter();

	for (i=0; i<triangles.G_count(); i++)
	{
		vi1=triangles[i].vertices[0];
		vi2=triangles[i].vertices[1];
		vi3=triangles[i].vertices[2];
		d1.lincomb(vertices[vi2],vertices[vi1],1,-1);d1.norm();
		d2.lincomb(vertices[vi3],vertices[vi1],1,-1);d2.norm();
		triangles[i].facenormal.vecprod(d1,d2);
		if (triangles[i].facenormal.getsize()<0.000001)
			triangles[i].facenormal.set(1,0,0);
		triangles[i].facenormal.norm();
	}
}

void T3Dmesh::calcsmoothnormals()
{
	int i,j,k;

	Titemarray<Tvec3d> vrt_normals;
	Tintarray vrt_count;

	calccenter();

	int maxidx=0;
	ASSERT(vertices_normals.G_count()==vertices.G_count());
	for (i=0; i<vertices.G_count(); i++)
	{
		if (vertices_normals[i].getsize()<0.0001) vertices_normals.set(i,Tvec3d(0,0,0));
		vertices_normals[i].set(0,0,0);
		maxidx=max(maxidx,vertices_idx[i]);
	}
	for (i=0; i<=maxidx; i++)
	{
		vrt_normals.add(Tvec3d(0,0,0));
		vrt_count.add(0);
	}

	for (i=0; i<triangles.G_count(); i++)
		for (j=0; j<3; j++)
		{
			k=triangles[i].vertices[j];
			ASSERT(k>=0);
			ASSERT(k<vertices.G_count());
			k=vertices_idx[k];
			vrt_count[k]++;
			vrt_normals[k].x+=triangles[i].facenormal.x;
			vrt_normals[k].y+=triangles[i].facenormal.y;
			vrt_normals[k].z+=triangles[i].facenormal.z;
		}

	for (i=0; i<=maxidx; i++)
	{
		k=vrt_count[i];
		if (k<=0)
			k=1;
		vrt_normals[k].x/=k;
		vrt_normals[k].y/=k;
		vrt_normals[k].z/=k;
		vrt_normals[k].norm();
	}
	for (i=0; i<vertices.G_count(); i++)
		vertices_normals[i]=vrt_normals[vertices_idx[i]];
	hasvertexnormals=true;
}

void T3Dmesh::subtriangle(double maxdist)
{
	int v1,v2,tr1,tr2,vnew,maxidx,vtr1,vtr2;
	Tvec3d newnorm;

	generateallvertexnormals();
	calcedges();

	maxidx=0;
	for (int i=0; i<vertices_idx.G_count(); i++)
		if (vertices_idx[i]>maxidx) maxidx=vertices_idx[i];

	for (int edgenr=0; edgenr<edges.G_count();)
	{
		v1=edges[edgenr].vertices[0];
		v2=edges[edgenr].vertices[1];
		tr1=edges[edgenr].triangles[0];
		tr2=edges[edgenr].triangles[1];
		if (Tvec3d::distance(vertices[v1],vertices[v2])>maxdist)
		{
			vnew=vertices.G_count();
			vertices.add(Tvec3d((vertices[v1].x+vertices[v2].x)/2,(vertices[v1].y+vertices[v2].y)/2,(vertices[v1].z+vertices[v2].z)/2));
			vertices_colL.add(T4color((vertices_colL[v1].r+vertices_colL[v2].r)/2,(vertices_colL[v1].g+vertices_colL[v2].g)/2,(vertices_colL[v1].b+vertices_colL[v2].b)/2,(vertices_colL[v1].a+vertices_colL[v2].a)/2));
			vertices_colR.add(T4color((vertices_colR[v1].r+vertices_colR[v2].r)/2,(vertices_colR[v1].g+vertices_colR[v2].g)/2,(vertices_colR[v1].b+vertices_colR[v2].b)/2,(vertices_colR[v1].a+vertices_colR[v2].a)/2));
			vertices_texcoord.add(T2textureidx((vertices_texcoord[v1].tx1+vertices_texcoord[v2].tx1)/2,(vertices_texcoord[v1].tx2+vertices_texcoord[v2].tx2)/2));
			maxidx++;vertices_idx.add(maxidx);
			newnorm.lincomb(vertices_normals[v1],vertices_normals[v2],1,1);newnorm.norm();
			vertices_normals.add(Tvec3d(newnorm.x,newnorm.y,newnorm.z));
			ASSERT(tr1>=0);
			vtr1=-1;
			if (triangles[tr1].vertices[0]==v1) vtr1=0;
			if (triangles[tr1].vertices[1]==v1) vtr1=1;
			if (triangles[tr1].vertices[2]==v1) vtr1=2;
			ASSERT(vtr1>=0);
			vtr2=(vtr1+1)%3;
			ASSERT(triangles[tr1].vertices[vtr2]==v2);
//			triangles[tr1].vertices[vtr2]=vnew;trianglesindices[3*tr1+vtr2]=vnew;
//			addtriangle(vnew,v2,v1);
			if (tr2>=0)
			{
				vtr1=-1;
				if (triangles[tr2].vertices[0]==v1) vtr1=0;
				if (triangles[tr2].vertices[1]==v1) vtr1=1;
				if (triangles[tr2].vertices[2]==v1) vtr1=2;
				ASSERT(vtr1>=0);
				vtr2=(vtr1+2)%3;
				ASSERT(triangles[tr2].vertices[vtr2]==v2);
//				triangles[tr2].vertices[vtr2]=vnew;trianglesindices[3*tr2+vtr2]=vnew;
	//			addtriangle(vnew,v2,v1);
			}
			edges[edgenr].vertices[1]=vnew;
			edgenr++;

			//... ondoenbaar ingewikkeld omwille van verschillende vertices met shared index!
		}
		else edgenr++;
	}

	calcnormals();
//	calcedges();
}


void T3Dmesh::generateallvertexnormals()
{
	int i,j,k;
	Tvec3d v;
	T4color cl;
	T2textureidx tidx(0,0);
	if (hasvertexnormals) return;

	calcnormals();
	for (i=0; i<triangles.G_count(); i++)
		for (j=0; j<3; j++)
		{
			k=triangles[i].vertices[j];
			ASSERT(k>=0);
			ASSERT(k<vertices.G_count());
			if (vertices_normals[k].getsize()<0.000001)
			{
				v=vertices[k];vertices.add(Tvec3d(v.x,v.y,v.z));
				v=triangles[i].facenormal;vertices_normals.add(Tvec3d(v.x,v.y,v.z));
				vertices_idx.add(vertices_idx[k]);
				cl=vertices_colL[k];vertices_colL.add(T4color(cl.r,cl.g,cl.b,cl.a));
				cl=vertices_colR[k];vertices_colR.add(T4color(cl.r,cl.g,cl.b,cl.a));
				tidx=vertices_texcoord[k];vertices_texcoord.add(T2textureidx(tidx.tx1,tidx.tx2));
				triangles[i].vertices[j]=vertices.G_count()-1;
				trianglesindices[3*i+j]=vertices.G_count()-1;
			}
		}

	calcedges();
	hasvertexnormals=true;
}




void T3Dmesh::calcedges()
{
	int trinr,ei,v1,v2,edgenr;
	//Tbaltree idx;
	//CString str;
	TBtree<unsigned __int64> idx;
	unsigned __int64 idxval;

	if (!needcalcedges) return;

	unsigned __int64 mf=1;
	mf=mf<<30;

	edges.reset();

	for (trinr=0; trinr<triangles.G_count(); trinr++)
	{
		for (ei=0; ei<3; ei++)
		{
			v1=triangles[trinr].vertices[(ei+0)%3];
			v2=triangles[trinr].vertices[(ei+1)%3];
			//str.Format(_qstr("%d_%d"),vertices_idx[v1],vertices_idx[v2]);
			//ASSERT(idx.get(str)<0);
			//str.Format(_qstr("%d_%d"),vertices_idx[v2],vertices_idx[v1]);
			//edgenr=idx.get(str);
			idxval=mf*(unsigned __int64)vertices_idx[v1]+(unsigned __int64)vertices_idx[v2];
			//ASSERT(idx.get(idxval)<0);
			idxval=mf*(unsigned __int64)vertices_idx[v2]+(unsigned __int64)vertices_idx[v1];
			edgenr=idx.get(idxval);
			if (edgenr<0)
			{
				Tedge edge;
				edge.triangles[0]=trinr;
				edge.triangles[1]=-1;
				edge.vertices[0]=v1;
				edge.vertices[1]=v2;
				edges.add(edge);
//				str.Format(_qstr("%d_%d"),vertices_idx[v1],vertices_idx[v2]);
//				idx.add(str,edges.G_count()-1);
				idxval=mf*(unsigned __int64)vertices_idx[v1]+(unsigned __int64)vertices_idx[v2];
				idx.add(idxval,edges.G_count()-1);
			}
			else
			{
				ASSERT(edges[edgenr].triangles[1]<0);
				edges[edgenr].triangles[1]=trinr;
			}
		}
	}

}


void T3Dmesh::render(Trendercontext *rc, bool hastexture, bool canbuffer)
{
	if (!hasvertexnormals) render_novertexnormals(rc,hastexture,canbuffer);
	else render_vertexnormals(rc,hastexture,canbuffer);
}


void T3Dmesh::render_novertexnormals(Trendercontext *rc, bool hastexture, bool canbuffer)
{
	int k;
	glBegin(GL_TRIANGLES);
	for (int i=0; i<triangles.G_count(); i++)
	{
		glNormal3d(triangles[i].facenormal.x,triangles[i].facenormal.y,triangles[i].facenormal.z);
		for (int j=0; j<3; j++)
		{
			k=triangles[i].vertices[j];
			if (hastexture) glTexCoord2d(vertices_texcoord[k].tx1,vertices_texcoord[k].tx2);
			glVertex3d(vertices[k].x,vertices[k].y,vertices[k].z);
		}
	}
	glEnd();
}



void T3Dmesh::render_vertexnormals(Trendercontext *rc, bool hastexture, bool canbuffer)
{
	int i,i2,j,k;
	Tvec3d v1;
	

	if (canbuffer)
	{
		rc->checkerror(_text("Start buffered render with normals"));
		if (!buffered)
		{
			buffer_vertices.create(rc,Trenderbufferobject::BFD_VERTEXDATA,vertices.G_ptr(),3*sizeof(V3DFLOAT)*vertices.G_count());
			buffer_normals.create(rc,Trenderbufferobject::BFD_VERTEXDATA,vertices_normals.G_ptr(),3*sizeof(V3DFLOAT)*vertices_normals.G_count());
			buffer_elements.create(rc,Trenderbufferobject::BDF_ELEMENTS,trianglesindices.G_ptr(),sizeof(VELTPE)*trianglesindices.G_count());
			if (hastexture) buffer_texture.create(rc,Trenderbufferobject::BFD_VERTEXDATA,vertices_texcoord.G_ptr(),2*sizeof(V3DFLOAT)*vertices_texcoord.G_count());
			if (hasowncolors)
			{
				buffer_colorsL.create(rc,Trenderbufferobject::BFD_VERTEXDATA,vertices_colL.G_ptr(),4*sizeof(V3DFLOAT)*vertices_colL.G_count());
				buffer_colorsR.create(rc,Trenderbufferobject::BFD_VERTEXDATA,vertices_colR.G_ptr(),4*sizeof(V3DFLOAT)*vertices_colR.G_count());
			}
			buffered=true;
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		buffer_vertices.select();
		glVertexPointer(3,GL_FLOAT,0,NULL);
		buffer_normals.select();
		glNormalPointer(GL_FLOAT,0,NULL);
		if (hastexture)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			buffer_texture.select();
			glTexCoordPointer(2,GL_FLOAT,0,NULL);
		}
		if (hasowncolors)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			if (!rc->isrightpart) buffer_colorsL.select();
			else buffer_colorsR.select();
			glColorPointer(4,GL_FLOAT,0,NULL);
		}
		buffer_elements.select();



		glDrawElements(GL_TRIANGLES,trianglesindices.G_count(),GL_UNSIGNED_INT,NULL);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		buffer_vertices.unselect();
		buffer_normals.unselect();
		if (hastexture) buffer_texture.unselect();
		if (hasowncolors)
		{
			if (!rc->isrightpart) buffer_colorsL.unselect();
			else buffer_colorsR.unselect();
		}
		buffer_elements.unselect();

		rc->checkerror(_text("End buffered render with normals"));
		return;
	}

	else
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,vertices.G_ptr());
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT,0,vertices_normals.G_ptr());
		if (hastexture)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2,GL_FLOAT,0,vertices_texcoord.G_ptr());
		}
		if (hasowncolors)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			if (!rc->isrightpart) glColorPointer(4,GL_FLOAT,0,vertices_colL.G_ptr());
			else glColorPointer(4,GL_FLOAT,0,vertices_colR.G_ptr());
		}

		glDrawElements(GL_TRIANGLES,trianglesindices.G_count(),GL_UNSIGNED_INT,trianglesindices.G_ptr());

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

	}
}


void T3Dmesh::calcshadowvolume(Tshadowvolume *shadowvolume, Tvertex &ilightpos, double depth1, double depth2)
{
	int i,j;
	char triactive1,triactive2;
	Tvec3d ldir,normal;

	shadowvolume->reset();

	Tvec3d lightpos;lightpos=ilightpos;
	double neardist=depth1;
	double fardist=depth2;

	boundingedgesv1.reset();
	boundingedgesv2.reset();

	ldir.lincomb(lightpos,centerpos,1,-1);ldir.norm();
	for (i=0; i<triangles.G_count(); i++)
	{
		if (Tvec3d::dotprod(ldir,triangles[i].facenormal)>=0)
			triangles[i].lightfacing=1;
		else
			triangles[i].lightfacing=0;
	}

	ASSERT(edges.G_count()>0);
	for (i=0; i<edges.G_count(); i++)
	{
		triactive1=0;triactive2=0;
		{
			if ((edges[i].triangles[0]>=0)&&(triangles[edges[i].triangles[0]].lightfacing)) triactive1=1;
			if ((edges[i].triangles[1]>=0)&&(triangles[edges[i].triangles[1]].lightfacing)) triactive2=1;
			if ((triactive1)&&(!triactive2))
			{
				boundingedgesv1.add(edges[i].vertices[0]);
				boundingedgesv2.add(edges[i].vertices[1]);
			}
			if ((!triactive1)&&(triactive2))
			{
				boundingedgesv1.add(edges[i].vertices[1]);
				boundingedgesv2.add(edges[i].vertices[0]);
			}
		}
	}



	Tvec3d v1,v2,v1p1,v1p2,v2p1,v2p2,locldir;
	Tvec3d vp1[3],vp2[3];

	for (i=0; i<triangles.G_count(); i++)
		if (triangles[i].lightfacing)
		{
			for (j=0; j<3; j++)
			{
				v1=vertices[triangles[i].vertices[j]];
				locldir.lincomb(lightpos,v1,1,-1);locldir.norm();
				vp1[j].lincomb(v1,locldir,1,-neardist);
				vp2[j].lincomb(v1,locldir,1,-fardist);
			}
			for (j=0; j<3; j++)	shadowvolume->caps1.add(Tvec3d(vp1[j].x,vp1[j].y,vp1[j].z));
			for (j=2; j>=0; j--) shadowvolume->caps2.add(Tvec3d(vp2[j].x,vp2[j].y,vp2[j].z));
		}

	for (i=0; i<boundingedgesv1.G_count(); i++)
	{
		v1=vertices[boundingedgesv1[i]];
		locldir.lincomb(lightpos,v1,1,-1);locldir.norm();
		v1p1.lincomb(v1,locldir,1,-neardist);
		v1p2.lincomb(v1,locldir,1,-fardist);
		v2=vertices[boundingedgesv2[i]];
		locldir.lincomb(lightpos,v2,1,-1);locldir.norm();
		v2p1.lincomb(v2,locldir,1,-neardist);
		v2p2.lincomb(v2,locldir,1,-fardist);
		shadowvolume->slices.add(Tvec3d(v1p1.x,v1p1.y,v1p1.z));
		shadowvolume->slices.add(Tvec3d(v1p2.x,v1p2.y,v1p2.z));
		shadowvolume->slices.add(Tvec3d(v2p2.x,v2p2.y,v2p2.z));
		shadowvolume->slices.add(Tvec3d(v2p1.x,v2p1.y,v2p1.z));
	}


}


/*
void T3Dmesh::precalcrendershadowvolume(Tprecalcrendershadowvolumecontext *pre, Taffinetransformation *tf)
{
	int i,j;
	char triactive1,triactive2;
	Tvec3d ldir,normal;

	Tvec3d lightpos;
	double neardist,fardist;


	Tvertex lpos;
	tf->mulinv(&pre->lightpos,&lpos);
	//Tvec3d lightpos;
	lightpos=lpos;
	neardist=pre->depth1;
	fardist=pre->depth2;

	boundingedgesv1.reset();
	boundingedgesv2.reset();

	ldir.lincomb(lightpos,centerpos,1,-1);ldir.norm();
	for (i=0; i<triangles.G_count(); i++)
	{
		if (Tvec3d::dotprod(ldir,triangles[i].facenormal)>0)
			triangles[i].lightfacing=1;
		else
			triangles[i].lightfacing=0;
	}

	ASSERT(edges.G_count()>0);
	for (i=0; i<edges.G_count(); i++)
	{
		triactive1=0;triactive2=0;
		{
			if ((edges[i].triangles[0]>=0)&&(triangles[edges[i].triangles[0]].lightfacing)) triactive1=1;
			if ((edges[i].triangles[1]>=0)&&(triangles[edges[i].triangles[1]].lightfacing)) triactive2=1;
			if ((triactive1)&&(!triactive2))
			{
				boundingedgesv1.add(edges[i].vertices[0]);
				boundingedgesv2.add(edges[i].vertices[1]);
			}
			if ((!triactive1)&&(triactive2))
			{
				boundingedgesv1.add(edges[i].vertices[1]);
				boundingedgesv2.add(edges[i].vertices[0]);
			}
		}
	}


	//precalc shadowvolume


	Tvec3d v1,v2,v1p1,v1p2,v2p1,v2p2,locldir;
	Tvec3d vp1[3],vp2[3];

	caps1.reset();caps2.reset();slices.reset();
	for (i=0; i<triangles.G_count(); i++)
		if (triangles[i].lightfacing)
		{
			for (j=0; j<3; j++)
			{
				v1=vertices[triangles[i].vertices[j]];
				locldir.lincomb(lightpos,v1,1,-1);locldir.norm();
				vp1[j].lincomb(v1,locldir,1,-neardist);
				vp2[j].lincomb(v1,locldir,1,-fardist);
			}
//			for (j=2; j>=0; j--) caps1.add(Tvec3d(vp1[j].x,vp1[j].y,vp1[j].z));
//			for (j=0; j<3; j++) caps2.add(Tvec3d(vp2[j].x,vp2[j].y,vp2[j].z));
			for (j=0; j<3; j++)	caps1.add(Tvec3d(vp1[j].x,vp1[j].y,vp1[j].z));
			for (j=2; j>=0; j--) caps2.add(Tvec3d(vp2[j].x,vp2[j].y,vp2[j].z));
		}

	for (i=0; i<boundingedgesv1.G_count(); i++)
	{
		v1=vertices[boundingedgesv1[i]];
		locldir.lincomb(lightpos,v1,1,-1);locldir.norm();
		v1p1.lincomb(v1,locldir,1,-neardist);
		v1p2.lincomb(v1,locldir,1,-fardist);
		v2=vertices[boundingedgesv2[i]];
		locldir.lincomb(lightpos,v2,1,-1);locldir.norm();
		v2p1.lincomb(v2,locldir,1,-neardist);
		v2p2.lincomb(v2,locldir,1,-fardist);
		slices.add(Tvec3d(v1p1.x,v1p1.y,v1p1.z));
		slices.add(Tvec3d(v1p2.x,v1p2.y,v1p2.z));
		slices.add(Tvec3d(v2p2.x,v2p2.y,v2p2.z));
		slices.add(Tvec3d(v2p1.x,v2p1.y,v2p1.z));
	}

}
*/
