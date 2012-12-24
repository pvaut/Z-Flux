#include "stdafx.h"

#include "vecmath.h"
#include "vec_transformation.h"

#include "SC_script.h"
#include "3DScene.h"
#include "3Dsurface.h"
#include "3DGeoObjects.h"
#include "3DMeshObject.h"
#include "3DSpaceWarper.h"
#include "SC_functor.h"

#include "tesselator.h"

#include "CSG_shape.h"

//*********************************************************************
// T3DObjectMesh
//*********************************************************************

T3DObjectMesh::T3DObjectMesh(T3DScene *iscene) : T3DGeoObjectPreCalcMesh(iscene)
{
}

bool T3DObjectMesh::paramchanged_needrecalculate(StrPtr paramname)
{
	return false;
}

void T3DObjectMesh::calculate()
{
	int i=0;
}

void T3DObjectMesh::copyfrom(const CSGShape &shape)
{
		mesh.copyfrom(shape);
}

void T3DObjectMesh::SetColor(int label,TSC_color &color)
{
	for (int i=0; i<mesh.vertices.G_count(); i++)
		if (mesh.vertices_label[i]==label)
			mesh.Set_vertexcolor(i,color.G_R(),color.G_G(),color.G_B(),color.G_A());
}


void T3DObjectMesh::generatevertexproperty(TSC_functor *functor, int vertexproperty)
{

	try{
		Tvertex pt;
		TSC_funcarglist arglist(NULL);
		TSC_value retval0,arg;
		TSC_color *rcl;
		double value;
		arg.createtype(GetTSCenv().G_datatype(SC_valname_vertex));
		arglist.add(&arg);
		if ((vertexproperty==VertexPropertyRed)||(vertexproperty==VertexPropertyGreen)||(vertexproperty==VertexPropertyBlue)||(vertexproperty==VertexPropertyTransp)) mesh.Set_hasowncolors();
		for (int i=0; i<mesh.vertices.G_count(); i++)
		{
			(G_valuecontent<Tvertex>(&arg))->Set3(mesh.vertices[i].x,mesh.vertices[i].y,mesh.vertices[i].z);
			functor->eval(&arglist,&retval0);
			if (vertexproperty==VertexPropertyRed) { mesh.vertices_colL[i].r=retval0.G_content_scalar()->G_val();mesh.vertices_colR[i].r=mesh.vertices_colL[i].r; }
			if (vertexproperty==VertexPropertyGreen) { mesh.vertices_colL[i].g=retval0.G_content_scalar()->G_val();mesh.vertices_colR[i].g=mesh.vertices_colL[i].g; }
			if (vertexproperty==VertexPropertyBlue) { mesh.vertices_colL[i].b=retval0.G_content_scalar()->G_val();mesh.vertices_colR[i].b=mesh.vertices_colL[i].b; }
			if (vertexproperty==VertexPropertyTransp) { mesh.vertices_colL[i].a=retval0.G_content_scalar()->G_val();mesh.vertices_colR[i].a=mesh.vertices_colL[i].a; }
			if (vertexproperty==VertexPropertyColor)
			{
				rcl=G_valuecontent<TSC_color>(&retval0);
				mesh.Set_vertexcolor(i,rcl->G_R(),rcl->G_G(),rcl->G_B(),rcl->G_A());
			}
			if (vertexproperty==VertexPropertyTC1) mesh.vertices_texcoord[i].tx1=retval0.G_content_scalar()->G_val();
			if (vertexproperty==VertexPropertyTC2) mesh.vertices_texcoord[i].tx2=retval0.G_content_scalar()->G_val();
		}
	}
	catch(TSC_runerror err)
	{
		QString errstring;
		FormatString(errstring,_text("Generate vertex property: ^1"),err.G_content());
		throw QError(errstring);
	}
	catch(QError err)
	{
		QString errstring;
		FormatString(errstring,_text("Generate vertex property: ^1"),err.G_content());
		throw QError(errstring);
	}
}


void T3DObjectMesh::CreateFlatPolygon(T2DContourset *contourset)
{
	G_tesselator().tesselate(contourset);
	
	Tvec3d v,normal;

	normal.set(0,0,1);
	mesh.reset();
	for (int contournr=0; contournr<contourset->G_contourcount(); contournr++)
	{
		T2DContour *contour=contourset->G_contour(contournr);
		for (int vnr=0; vnr<contour->G_pointcount(); vnr++)
		{
			ASSERT(mesh.vertices.G_count()==contour->G_point(vnr).ptnr);
			v.set(contour->G_point(vnr).pt.G3_x(),contour->G_point(vnr).pt.G3_y(),contour->G_point(vnr).pt.G3_z());
			mesh.addvertex(&v,contour->G_point(vnr).ptnr,&normal);
		}
	}
	for (int tnr=0; tnr<G_tesselator().G_trianglecount(); tnr++)
		mesh.addtriangle(G_tesselator().G_triangle(tnr).nr1,G_tesselator().G_triangle(tnr).nr2,G_tesselator().G_triangle(tnr).nr3);

	mesh.calcnormals();
	mesh.calcedges();
}

void T3DObjectMesh::CreateExtrudedPolygon(T2DContourset *contourset0, double height, bool closed, int layercount)
{
	Tvertex v;
	Tvector *pnormal,normal;
	T2DContourPoint *cpt;
	int totlayerpointcount,cvnr,offs;
	double texcoord1;

	for (int contournr=0; contournr<contourset0->G_contourcount(); contournr++)
		contourset0->G_contour(contournr)->calccirctexcoord();

	G_tesselator().tesselate(contourset0);

	if (layercount<2) layercount=2;

	mesh.reset();

	//vertical slices


	//calculate total number of points in a single layer
	totlayerpointcount=0;
	for (int contournr=0; contournr<contourset0->G_contourcount(); contournr++)
	{
		T2DContour *contour=contourset0->G_contour(contournr);
		totlayerpointcount+=contour->G_pointcount()+1;
	}

	for (int layernr=0; layernr<layercount; layernr++)
	{
		for (int contournr=0; contournr<contourset0->G_contourcount(); contournr++)
		{
			T2DContour *contour=contourset0->G_contour(contournr);
			for (int vnr=0; vnr<=contour->G_pointcount(); vnr++)
			{
				cpt=&contour->G_point(vnr%contour->G_pointcount());
				texcoord1=cpt->texcoord;
				if (vnr==contour->G_pointcount()) texcoord1=1;
				v.Set3(cpt->pt.G3_x(),cpt->pt.G3_y(),cpt->pt.G3_z()+layernr*1.0/(layercount-1)*height);
				pnormal=NULL;
				if (cpt->normalpresent)
					pnormal=&cpt->nm;
				mesh.addvertex(&v,cpt->PTID+layernr*totlayerpointcount,pnormal,texcoord1,layernr*1.0/(layercount-1));
				if ((vnr>0)&&(layernr>0))
				{
					cvnr=mesh.vertices.G_count()-1;
					mesh.addquad(	cvnr -0 -0*totlayerpointcount,
									cvnr -1 -0*totlayerpointcount,
									cvnr -1 -1*totlayerpointcount,
									cvnr -0 -1*totlayerpointcount);
				}
			}
		}
	}

	if (closed)
	{
		//top cap
		normal=Tvector(0,0,1);
		offs=mesh.vertices.G_count();
		for (int contournr=0; contournr<contourset0->G_contourcount(); contournr++)
		{
			T2DContour *contour=contourset0->G_contour(contournr);
			for (int vnr=0; vnr<contour->G_pointcount(); vnr++)
			{
				cpt=&contour->G_point(vnr);
				v.Set3(cpt->pt.G3_x(),cpt->pt.G3_y(),cpt->pt.G3_z()+height);
				mesh.addvertex(&v,cpt->PTID+(layercount-1)*totlayerpointcount,&normal);
			}
		}
		for (int tnr=0; tnr<G_tesselator().G_trianglecount(); tnr++)
			mesh.addtriangle(	G_tesselator().G_triangle(tnr).nr1+offs,
								G_tesselator().G_triangle(tnr).nr2+offs,
								G_tesselator().G_triangle(tnr).nr3+offs);
		//bottom cap
		normal=Tvector(0,0,-1);
		offs=mesh.vertices.G_count();
		for (int contournr=0; contournr<contourset0->G_contourcount(); contournr++)
		{
			T2DContour *contour=contourset0->G_contour(contournr);
			for (int vnr=0; vnr<contour->G_pointcount(); vnr++)
			{
				cpt=&contour->G_point(vnr);
				v.Set3(cpt->pt.G3_x(),cpt->pt.G3_y(),cpt->pt.G3_z());
				mesh.addvertex(&v,cpt->PTID,&normal);
			}
		}
		for (int tnr=0; tnr<G_tesselator().G_trianglecount(); tnr++)
			mesh.addtriangle(	G_tesselator().G_triangle(tnr).nr1+offs,
								G_tesselator().G_triangle(tnr).nr3+offs,
								G_tesselator().G_triangle(tnr).nr2+offs);
	}

	mesh.calcnormals();
	mesh.calcedges();
}


void T3DObjectMesh::CreateRevolvedPolygon(T2DContourset *contourset, double iangle, int resolution)
{
	double ang;
	int vidx,cvnr;
	Tvec3d vr,nm;

	mesh.reset();
	if (resolution<2) throw QError(_text("Resolution should be at least 2"));
	if (contourset->G_contourcount()!=1) throw QError(_text("Contour set should contain exactly one contour"));
	T2DContour *contour=contourset->G_contour(0);

/*	for (int j=0; j<contour->G_pointcount(); j++)
		if (!contour->G_point(j).normalpresent)
			throw QError(_text("All points in the contour should have normals defined"));*/

	contour->calcdims();
	contour->calccirctexcoord();


	int contourptcount=contour->G_pointcount();

	for (int i=0; i<=resolution; i++)
	{
		ang=i*1.0/resolution*iangle;
		for (int j=0; j<contour->G_pointcount(); j++)
		{
			T2DContourPoint &pt=contour->G_point(j);
			vidx=i*contour->G_pointcount()+pt.PTID;
			vr.x=pt.pt.G3_x()*cos(ang);
			vr.z=pt.pt.G3_x()*sin(ang);
			vr.y=pt.pt.G3_y();
			nm.x=pt.nm.G_x()*cos(ang);
			nm.z=pt.nm.G_x()*sin(ang);
			nm.y=pt.nm.G_y();
			mesh.addvertex(&vr,vidx,&nm,ang/(2*Pi),pt.texcoord);
			cvnr=mesh.vertices.G_count()-1;
			if ((i>0)&&(j>0)&&(contour->G_point(j).PTID!=contour->G_point(j-1).PTID))
				mesh.addquad(	cvnr-0 -0*contourptcount,
								cvnr-1 -0*contourptcount,
								cvnr-1 -1*contourptcount,
								cvnr-0 -1*contourptcount);

		}
	}

	mesh.calcnormals();
	mesh.calcedges();
}



void T3DObjectMesh::transform(Taffinetransformation *tf)
{
	Tlineartransformer w(tf);
	mesh.warp(w);
}

void T3DObjectMesh::warpspiral(double winding)
{
	Tspiraler w(winding);
	mesh.warp(w);
}

void T3DObjectMesh::warpconalpinch(double toph)
{
	Tconalpincher w(toph);
	mesh.warp(w);
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectmesh_createshape,CreateShape)
{
	setmemberfunction(SC_valname_meshobject);
	addvar(_qstr("shape"),SC_valname_shape);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	CSGShape *shape=G_valuecontent<CSGShape>(arglist->get(0));
	G_valuecontent<T3DObjectMesh>(owner)->copyfrom(*shape);
/*
	CSGShape sh1,sh2,sh3,shtmp;

//	sh1.createsphere(CSGVector(0,0,0),1,15);
	sh1.createbar(CSGVector(-1,0,-1),CSGVector(2,1,2));

//	sh2.createcylinder(CSGVector(0.5,-1,0),CSGVector(0,2,0),0.3,15);
	sh2.createsphere(CSGVector(0,0.6,0),0.7,15);

	CSGShapeCombiner comb(&sh1,&sh2);
	comb.getDifference(sh3);


	shtmp.copyfrom(sh3);
	G_valuecontent<T3DObjectMesh>(owner)->copyfrom(shtmp);

//G_valuecontent<T3DObjectMesh>(owner)->copyfrom(sh1);*/


}
ENDFUNCTION(func_objectmesh_createshape)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectmesh_createflatshape,CreateFlatShape)
{
	setmemberfunction(SC_valname_meshobject);
	addvar(_qstr("contour"),SC_valname_2dcontourset);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<T3DObjectMesh>(owner)->CreateFlatPolygon(G_valuecontent<T2DContourset>(arglist->get(0)));
}
ENDFUNCTION(func_objectmesh_createflatshape)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectmesh_createextrudedshape,Extrude)
{
	setmemberfunction(SC_valname_meshobject);
	addvar(_qstr("contour"),SC_valname_2dcontourset);
	addvar(_qstr("height"),SC_valname_scalar);
	addvar(_qstr("closed"),SC_valname_boolean,false);
	addvar(_qstr("layercount"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	bool closed=true;
	int layercount=2;
	if (arglist->G_ispresent(2)) closed=arglist->get(2)->G_content_boolean()->G_val();
	if (arglist->G_ispresent(3)) layercount=arglist->get(3)->G_content_scalar()->G_intval();
	G_valuecontent<T3DObjectMesh>(owner)->CreateExtrudedPolygon(
		G_valuecontent<T2DContourset>(arglist->get(0)),
		arglist->get(1)->G_content_scalar()->G_val(),closed,layercount);
}
ENDFUNCTION(func_objectmesh_createextrudedshape)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectmesh_revolve,Revolve)
{
	setmemberfunction(SC_valname_meshobject);
	addvar(_qstr("contour"),SC_valname_2dcontourset);
	addvar(_qstr("resolution"),SC_valname_scalar);
	addvar(_qstr("angle"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double angle=2*Pi;
	int resolution=arglist->get(1)->G_content_scalar()->G_intval();
	if (arglist->G_ispresent(2)) angle=arglist->get(2)->G_content_scalar()->G_val();
	G_valuecontent<T3DObjectMesh>(owner)->CreateRevolvedPolygon(
		G_valuecontent<T2DContourset>(arglist->get(0)),angle,resolution);
}
ENDFUNCTION(func_objectmesh_revolve)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectmesh_setcolor,SetColor)
{
	setmemberfunction(SC_valname_meshobject);
	addvar(_qstr("Label"),SC_valname_scalar);
	addvar(_qstr("Color"),SC_valname_color);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	int label=arglist->get(0)->G_content_scalar()->G_intval();
	TSC_color *color=G_valuecontent<TSC_color>(arglist->get(1));
	G_valuecontent<T3DObjectMesh>(owner)->SetColor(label,*color);
}
ENDFUNCTION(func_objectmesh_setcolor)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectmesh_generatevertexproperty,GenerateVertexProperty)
{
	setmemberfunction(SC_valname_meshobject);
	addvar(_qstr("function"),SC_valname_functor);
	addvar(_qstr("property"),SC_valname_vertexproperty);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<T3DObjectMesh>(owner)->generatevertexproperty(
		G_valuecontent<TSC_functor>(arglist->get(0)),
		arglist->get(1)->toint());
}
ENDFUNCTION(func_objectmesh_generatevertexproperty)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectmesh_transform,Transform)
{
	setmemberfunction(SC_valname_meshobject);
	addvar(_qstr("transf"),SC_valname_affinetransformation);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<T3DObjectMesh>(owner)->transform(G_valuecontent<Taffinetransformation>(arglist->get(0)));
}
ENDFUNCTION(func_objectmesh_transform)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectmesh_warpspiral,WarpSpiral)
{
	setmemberfunction(SC_valname_meshobject);
	addvar(_qstr("winding"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<T3DObjectMesh>(owner)->warpspiral(arglist->get(0)->G_content_scalar()->G_val());
}
ENDFUNCTION(func_objectmesh_warpspiral)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectmesh_warpconalpinch,WarpConalPinch)
{
	setmemberfunction(SC_valname_meshobject);
	addvar(_qstr("topheight"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<T3DObjectMesh>(owner)->warpconalpinch(arglist->get(0)->G_content_scalar()->G_val());
}
ENDFUNCTION(func_objectmesh_warpconalpinch)
