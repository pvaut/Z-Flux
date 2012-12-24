#include "stdafx.h"


#include "3DScene.h"

#include "3DSurface.h"

#include "matrix.h"

#include "SC_script.h"
#include "SC_functor.h"
#include "bezier.h"

#include "qxstoredbitmap.h"

#include "opengl.h"


//*********************************************************************
// T3DObjectSurface
//*********************************************************************

T3DObjectSurface::T3DObjectSurface(T3DScene *iscene) : T3DGeoObjectPreCalcMesh(iscene)
{
	//addparam(_qstr("Position"),SC_valname_vertex);
	//addparam(_qstr("Color"),SC_valname_color);

//	addparam(_qstr("Extra"),SC_valname_string);

	hasowncolors=false;
}


bool T3DObjectSurface::paramchanged_needrecalculate(StrPtr paramname)
{
	return false;
}

void T3DObjectSurface::calculate()
{
	int i,j;
	int ct=0;

	mesh.reset();
	for (i=0; i<dim1; i++)
	{
		for (j=0; j<dim2; j++)
		{
			if (!hasowncolors)
				mesh.addvertex(&vertices[i]->get(j),ct,&normals[i]->get(j),texors[i]->get(j).tx1,texors[i]->get(j).tx2);
			else
				mesh.addcolorvertex(&vertices[i]->get(j),ct,&normals[i]->get(j),colors[i]->get(j).G_R(),colors[i]->get(j).G_G(),colors[i]->get(j).G_B(),colors[i]->get(j).G_A());
			ct++;
		}
	}

	mesh.Make_stripbased(dim2,false);
}


void T3DObjectSurface::setdim(int idim1, int idim2)
{
	Set_notcalculated();
	dim1=idim1;
	dim2=idim2;
	vertices.reset();normals.reset();colors.reset();texors.reset();
	Ttexor txor;
	for (int i1=0; i1<dim1; i1++)
	{
		Tcheapitemarray<Tvertex> *vrow=new Tcheapitemarray<Tvertex>;vrow->dim(idim2);vertices.add(vrow);
		Tcheapitemarray<Tvector> *nrow=new Tcheapitemarray<Tvector>;nrow->dim(idim2);normals.add(nrow);
		Tcheapitemarray<TSC_color> *crow=new Tcheapitemarray<TSC_color>;crow->dim(idim2);colors.add(crow);
		Tcheapitemarray<Ttexor> *trow=new Tcheapitemarray<Ttexor>;trow->dim(idim2);texors.add(trow);
		for (int i2=0; i2<dim2; i2++)
		{
			txor.tx1=i1*1.0/(dim1-1);
			txor.tx2=i2*1.0/(dim2-1);
			trow->set(i2,txor);
		}
	}
}

void T3DObjectSurface::generate(TSC_functor *functor, double min1, double max1, int count1, double min2, double max2,int count2)
{
	Set_notcalculated();
	if (count1<2) throw QError(_qstr("Generate surface: invalid first resolution"));
	if (count2<2) throw QError(_qstr("Generate surface: invalid second resolution"));

	setdim(count1,count2);

	hasowncolors=false;

	TSC_color defcolor;
	
	try{
		double deriv_fudge1=(max1-min1)*0.025/(count1-1.0);
		double deriv_fudge2=(max2-min2)*0.025/(count2-1.0);

		double vl1,vl2;
		TSC_funcarglist arglist(NULL);
		TSC_value retval0,retval1,retval2,arg1,arg2;
		Tvector dir1,dir2,dirnorm,dirnormnorm;
		arg1.createtype(GetTSCenv().G_datatype(SC_valname_scalar));
		arg2.createtype(GetTSCenv().G_datatype(SC_valname_scalar));
		arglist.add(&arg1);arglist.add(&arg2);

		for (int i1=0; i1<count1; i1++)
			for (int i2=0; i2<count2; i2++)
			{
				vl1=min1+(max1-min1)*i1/(count1-1.0);
				vl2=min2+(max2-min2)*i2/(count2-1.0);
				arg1.copyfrom(vl1);arg2.copyfrom(vl2);
				functor->eval(&arglist,&retval0);
				Set_vertex(i1,i2,G_valuecontent<Tvertex>(&retval0));
				Set_texor(i1,i2,i1*1.0/(count1-1),i2*1.0/(count2-1));
				Set_color(i1,i2,&defcolor);

				//deriv1
				arg1.copyfrom(vl1+deriv_fudge1);functor->eval(&arglist,&retval2);
//				arg1.copyfrom(vl1-deriv_fudge1);functor->eval(&arglist,&retval1);
				dir1.subtr(G_valuecontent<Tvertex>(&retval2),G_valuecontent<Tvertex>(&retval0));
				//deriv2
				arg2.copyfrom(vl2+deriv_fudge2);functor->eval(&arglist,&retval2);
//				arg2.copyfrom(vl2-deriv_fudge2);functor->eval(&arglist,&retval1);
				dir2.subtr(G_valuecontent<Tvertex>(&retval2),G_valuecontent<Tvertex>(&retval0));
				//norm
				dirnorm.vecprod(&dir2,&dir1);dirnormnorm.normfrom(&dirnorm);
				Set_normal(i1,i2,&dirnormnorm);
			}

		hasowncolors=false;

	}
	catch(TSC_runerror err)
	{
		QString errstring;
		FormatString(errstring,_text("Generate surface: ^1"),err.G_content());
		throw QError(errstring);
	}
	catch(QError err)
	{
		QString errstring;
		FormatString(errstring,_text("Generate surface: ^1"),err.G_content());
		throw QError(errstring);
	}
}

void T3DObjectSurface::generate(TPolyBezierSurface *src, int count1, int count2)
{
	Set_notcalculated();
	if (count1<2) throw QError(_qstr("Generate surface: invalid first resolution"));
	if (count2<2) throw QError(_qstr("Generate surface: invalid second resolution"));

	setdim(count1,count2);

	hasowncolors=false;

	TSC_color defcolor;
	Tvertex pt;
	Tvector dir1,dir2,dirnorm,dirnormnorm;
	
	double fr1,fr2;
	for (int i1=0; i1<count1; i1++)
		for (int i2=0; i2<count2; i2++)
		{
			fr1=i1/(count1-1.0);
			fr2=i2/(count2-1.0);
			src->eval(fr1,fr2,pt);
			src->evalder1(fr1,fr2,dir1);
			src->evalder2(fr1,fr2,dir2);
			dirnorm.vecprod(&dir2,&dir1);dirnormnorm.normfrom(&dirnorm);
			Set_vertex(i1,i2,&pt);
			Set_texor(i1,i2,i1*1.0/(count1-1),i2*1.0/(count2-1));
			Set_color(i1,i2,&defcolor);
			Set_normal(i1,i2,&dirnormnorm);
		}
	hasowncolors=false;
}


void T3DObjectSurface::generatevertexproperty(TSC_functor *functor, int vertexproperty)
{
	Set_notcalculated();

	try{
		Tvertex pt;
		TSC_funcarglist arglist(NULL);
		TSC_value retval0,arg;
		TSC_color *rcl;
		double value;
		arg.createtype(GetTSCenv().G_datatype(SC_valname_vertex));
		arglist.add(&arg);


		if ((vertexproperty==VertexPropertyRed)||(vertexproperty==VertexPropertyGreen)||(vertexproperty==VertexPropertyBlue)||(vertexproperty==VertexPropertyTransp)||(vertexproperty==VertexPropertyColor))
			hasowncolors=true;

		for (int i1=0; i1<dim1; i1++)
			for (int i2=0; i2<dim2; i2++)
			{
				*(G_valuecontent<Tvertex>(&arg))=(vertices[i1]->get(i2));
				functor->eval(&arglist,&retval0);

				if (vertexproperty==VertexPropertyRed)    colors[i1]->get(i2).Set_R(retval0.G_content_scalar()->G_val());
				if (vertexproperty==VertexPropertyGreen)  colors[i1]->get(i2).Set_G(retval0.G_content_scalar()->G_val());
				if (vertexproperty==VertexPropertyBlue)   colors[i1]->get(i2).Set_B(retval0.G_content_scalar()->G_val());
				if (vertexproperty==VertexPropertyTransp) colors[i1]->get(i2).Set_A(retval0.G_content_scalar()->G_val());
				if (vertexproperty==VertexPropertyColor)
				{
					rcl=G_valuecontent<TSC_color>(&retval0);
					colors[i1]->get(i2)=*rcl;
				}
				if (vertexproperty==VertexPropertyTC1) texors[i1]->get(i2).tx1=retval0.G_content_scalar()->G_val();
				if (vertexproperty==VertexPropertyTC2) texors[i1]->get(i2).tx2=retval0.G_content_scalar()->G_val();
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


void T3DObjectSurface::copyfrom(T3DObjectSurface *isf)
{
	Set_notcalculated();
	setdim(isf->dim1,isf->dim2);
	hasowncolors=isf->hasowncolors;
	for (int i1=0; i1<dim1; i1++)
		for (int i2=0; i2<dim2; i2++)
		{
			vertices[i1]->set(i2,isf->vertices[i1]->get(i2));
			normals[i1]->set(i2,isf->normals[i1]->get(i2));
			colors[i1]->set(i2,isf->colors[i1]->get(i2));
			texors[i1]->set(i2,isf->texors[i1]->get(i2));
		}
}




void T3DObjectSurface::Set_vertex(int i1, int i2, Tvertex *vertex)
{
	if ((i1<0)||(i2<0)||(i1>=dim1)||(i2>=dim2)) throw QError(_text("Invalid surface index"));
	vertices[i1]->set(i2,*vertex);
	Set_notcalculated();
}

Tvertex* T3DObjectSurface::G_vertex(int i1, int i2)
{
	if ((i1<0)||(i2<0)||(i1>=dim1)||(i2>=dim2)) throw QError(_text("Invalid surface index"));
	return &vertices[i1]->get(i2);
}

void T3DObjectSurface::Set_normal(int i1, int i2, Tvector *normal)
{
	if ((i1<0)||(i2<0)||(i1>=dim1)||(i2>=dim2)) throw QError(_text("Invalid surface index"));
	normals[i1]->set(i2,*normal);
	Set_notcalculated();
}

Tvector* T3DObjectSurface::G_normal(int i1, int i2)
{
	if ((i1<0)||(i2<0)||(i1>=dim1)||(i2>=dim2)) throw QError(_text("Invalid surface index"));
	return &normals[i1]->get(i2);
}

void T3DObjectSurface::Set_texor(int i1, int i2, double tx1, double tx2)
{
	if ((i1<0)||(i2<0)||(i1>=dim1)||(i2>=dim2)) throw QError(_text("Invalid surface index"));
	texors[i1]->get(i2).tx1=tx1;
	texors[i1]->get(i2).tx2=tx2;
}

void T3DObjectSurface::Set_color(int i1, int i2, TSC_color *color)
{
	if ((i1<0)||(i2<0)||(i1>=dim1)||(i2>=dim2)) throw QError(_text("Invalid surface index"));
	colors[i1]->set(i2,*color);
	hasowncolors=true;
	if (iscalculated)
		mesh.Set_vertexcolor(i1*dim2+i2,color->G_R(),color->G_G(),color->G_B(),color->G_A());
}

TSC_color* T3DObjectSurface::G_color(int i1, int i2)
{
	if ((i1<0)||(i2<0)||(i1>=dim1)||(i2>=dim2)) throw QError(_text("Invalid surface index"));
	return &colors[i1]->get(i2);
}


void T3DObjectSurface::CalcNormals()
{
	int i1a,i1b,i2a,i2b;
	Tvector dr1,dr2,nm;

	for (int i1=0; i1<dim1; i1++)
		for (int i2=0; i2<dim2; i2++)
		{
			i1a=i1-1; if (i1a<0) i1a=0;
			i1b=i1+1; if (i1b>=dim1) i1b=dim1-1;
			i2a=i2-1; if (i2a<0) i2a=0;
			i2b=i2+1; if (i2b>=dim2) i2b=dim2-1;
			dr1.subtr(G_vertex(i1b,i2),G_vertex(i1a,i2));
			dr2.subtr(G_vertex(i1,i2b),G_vertex(i1,i2a));
			nm.vecprod(&dr2,&dr1);nm.normfrom(&nm);
			Set_normal(i1,i2,&nm);
		}
}


void T3DObjectSurface::LoadTopoSurface(TMatrix *mtr, double  ixmin, double ixmax, double iymin, double iymax, double ihfactor)
{
	int resx=mtr->G_dim1();
	int resy=mtr->G_dim2();

	if (resx<2) throw QError(_text("Invalid topo data"));
	if (resy<2) throw QError(_text("Invalid topo data"));

	setdim(mtr->G_dim1(),mtr->G_dim2());

	Tvertex vertex;
	double x,y,z;
	for (int ix=0; ix<resx; ix++)
	{
		x=ixmin+ix*1.0/(resx-1)*(ixmax-ixmin);
		for (int iy=0; iy<resy; iy++)
		{
			z=ihfactor*mtr->Get(ix,iy);
			y=iymax-iy*1.0/(resy-1)*(iymax-iymin);
			vertex.copyfrom(x,y,z);
			Set_vertex(ix,iy,&vertex);
			Set_texor(ix,iy,ix*1.0/(resx-1),iy*1.0/(resy-1));
		}
	}

	CalcNormals();
}



void T3DObjectSurface::LoadTopoSphere(TMatrix *mtr, double  ilongmin, double ilongmax, double ilattmin, double ilattmax, double ihfactor, double iradius)
{
	int reslong=mtr->G_dim1();
	int reslatt=mtr->G_dim2();

	if (reslong<2) throw QError(_text("Invalid topo data"));
	if (reslatt<2) throw QError(_text("Invalid topo data"));

	setdim(mtr->G_dim1(),mtr->G_dim2());

	Tvertex vertex;
	double ang_long,ang_latt,rd;
	for (int ilatt=0; ilatt<reslatt; ilatt++)
	{
		ang_latt=ilattmin+ilatt*1.0/(reslatt-1)*(ilattmax-ilattmin);
		for (int ilong=0; ilong<reslong; ilong++)
		{
			rd=iradius+ihfactor*mtr->Get(ilong,ilatt);
			ang_long=ilongmax-ilong*1.0/(reslong-1)*(ilongmax-ilongmin);
			vertex.copyfrom(rd*cos(ang_latt)*cos(ang_long),rd*cos(ang_latt)*sin(ang_long),rd*sin(ang_latt));
			Set_vertex(ilong,ilatt,&vertex);
			Set_texor(ilong,ilatt,ilong*1.0/(reslong-1),ilatt*1.0/(reslatt-1));
		}
	}

	CalcNormals();
}


void T3DObjectSurface::copytoframe(double phase)
{
	calculate();
	mesh.copytoframe(phase);
}

void T3DObjectSurface::interpolframe(double phase)
{
	mesh.interpolframe(phase);
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_surface_setdim,dim)
{
//	setreturntype(SC_valname_surface);
	setmemberfunction(SC_valname_surface);
	addvar(_qstr("Dim1"),SC_valname_scalar);
	addvar(_qstr("Dim2"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectSurface *surf=G_valuecontent<T3DObjectSurface>(owner);
	int dim1=arglist->get(0)->G_content_scalar()->G_intval();
	int dim2=arglist->get(1)->G_content_scalar()->G_intval();
	surf->setdim(dim1,dim2);
//	retval->encapsulate(surf);
}
ENDFUNCTION(func_surface_setdim)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_surface_vertexs,points)
{
	setcanassign();
	setreturntype(SC_valname_vertex);
	setmemberfunction(SC_valname_surface);
	addvar(_qstr("Idx1"),SC_valname_scalar);
	addvar(_qstr("idx2"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectSurface *surf=G_valuecontent<T3DObjectSurface>(owner);
	int idx1=arglist->get(0)->G_content_scalar()->G_intval();
	int idx2=arglist->get(1)->G_content_scalar()->G_intval();
	if (assigntoval!=NULL)
		surf->Set_vertex(idx1,idx2,G_valuecontent<Tvertex>(assigntoval));
	*G_valuecontent<Tvertex>(retval)=*surf->G_vertex(idx1,idx2);
}
ENDFUNCTION(func_surface_vertexs)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_surface_normals,normals)
{
	setcanassign();
	setreturntype(SC_valname_vector);
	setmemberfunction(SC_valname_surface);
	addvar(_qstr("Idx1"),SC_valname_scalar);
	addvar(_qstr("idx2"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectSurface *surf=G_valuecontent<T3DObjectSurface>(owner);
	int idx1=arglist->get(0)->G_content_scalar()->G_intval();
	int idx2=arglist->get(1)->G_content_scalar()->G_intval();
	if (assigntoval!=NULL)
		surf->Set_normal(idx1,idx2,G_valuecontent<Tvector>(assigntoval));
	*G_valuecontent<Tvector>(retval)=*surf->G_normal(idx1,idx2);
}
ENDFUNCTION(func_surface_normals)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_surface_colors,colors)
{
	setcanassign();
	setreturntype(SC_valname_color);
	setmemberfunction(SC_valname_surface);
	addvar(_qstr("Idx1"),SC_valname_scalar);
	addvar(_qstr("idx2"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectSurface *surf=G_valuecontent<T3DObjectSurface>(owner);
	int idx1=arglist->get(0)->G_content_scalar()->G_intval();
	int idx2=arglist->get(1)->G_content_scalar()->G_intval();
	if (assigntoval!=NULL)
		surf->Set_color(idx1,idx2,G_valuecontent<TSC_color>(assigntoval));
	*G_valuecontent<TSC_color>(retval)=*surf->G_color(idx1,idx2);
}
ENDFUNCTION(func_surface_colors)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_surface_generate,Generate)
{
//	setcanassign();
//	setreturntype(SC_valname_vertex);
	setmemberfunction(SC_valname_surface);
	addvar(_qstr("function"),SC_valname_functor);
	addvar(_qstr("min1"),SC_valname_scalar);
	addvar(_qstr("max1"),SC_valname_scalar);
	addvar(_qstr("count1"),SC_valname_scalar);
	addvar(_qstr("min2"),SC_valname_scalar);
	addvar(_qstr("max2"),SC_valname_scalar);
	addvar(_qstr("count2"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectSurface *surf=G_valuecontent<T3DObjectSurface>(owner);

	TSC_functor *functor=G_valuecontent<TSC_functor>(arglist->get(0));

	double min1=arglist->get(1)->G_content_scalar()->G_val();
	double max1=arglist->get(2)->G_content_scalar()->G_val();
	double count1=arglist->get(3)->G_content_scalar()->G_intval();
	double min2=arglist->get(4)->G_content_scalar()->G_val();
	double max2=arglist->get(5)->G_content_scalar()->G_val();
	double count2=arglist->get(6)->G_content_scalar()->G_intval();

	surf->generate(functor,min1,max1,count1,min2,max2,count2);
}
ENDFUNCTION(func_surface_generate)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_surface_generate_spline,GenerateSplineSurface)
{
//	setcanassign();
//	setreturntype(SC_valname_vertex);
	setmemberfunction(SC_valname_surface);
	addvar(_qstr("source"),SC_valname_polybeziersurface);
	addvar(_qstr("count1"),SC_valname_scalar);
	addvar(_qstr("count2"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectSurface *surf=G_valuecontent<T3DObjectSurface>(owner);

	TPolyBezierSurface *inp=G_valuecontent<TPolyBezierSurface>(arglist->get(0));

	double count1=arglist->get(1)->G_content_scalar()->G_intval();
	double count2=arglist->get(2)->G_content_scalar()->G_intval();

	surf->generate(inp,count1,count2);
}
ENDFUNCTION(func_surface_generate_spline)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_surface_generatevertexproperty,GenerateVertexProperty)
{
	setmemberfunction(SC_valname_surface);
	addvar(_qstr("function"),SC_valname_functor);
	addvar(_qstr("property"),SC_valname_vertexproperty);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectSurface *surf=G_valuecontent<T3DObjectSurface>(owner);
	TSC_functor *functor=G_valuecontent<TSC_functor>(arglist->get(0));

	surf->generatevertexproperty(functor,arglist->get(1)->toint());
}
ENDFUNCTION(func_surface_generatevertexproperty)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_surface_copy,copyfrom)
{
	setmemberfunction(SC_valname_surface);
	addvar(_qstr("from"),SC_valname_surface);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectSurface *surf1=G_valuecontent<T3DObjectSurface>(arglist->get(0));
	T3DObjectSurface *surf2=G_valuecontent<T3DObjectSurface>(owner);
	surf2->copyfrom(surf1);
}
ENDFUNCTION(func_surface_copy)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_surface_create_topo_surface,CreateTopoSurface)
{
	setmemberfunction(SC_valname_surface);
	addvar(_qstr("DataMatrix"),SC_valname_matrix);
	addvar(_qstr("XMin"),SC_valname_scalar);
	addvar(_qstr("XMax"),SC_valname_scalar);
	addvar(_qstr("YMin"),SC_valname_scalar);
	addvar(_qstr("YMax"),SC_valname_scalar);
	addvar(_qstr("HeightFactor"),SC_valname_scalar);
}


void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectSurface *surf=G_valuecontent<T3DObjectSurface>(owner);
	TMatrix *mtr=G_valuecontent<TMatrix>(arglist->get(0));

	double x_min=arglist->get(1)->G_content_scalar()->G_val();
	double x_max=arglist->get(2)->G_content_scalar()->G_val();
	double y_min=arglist->get(3)->G_content_scalar()->G_val();
	double y_max=arglist->get(4)->G_content_scalar()->G_val();
	double hfactor=arglist->get(5)->G_content_scalar()->G_val();

	surf->LoadTopoSurface(mtr,x_min,x_max,y_min,y_max,hfactor);
}
ENDFUNCTION(func_surface_create_topo_surface)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_surface_create_topo_sphere,CreateTopoSphere)
{
	setmemberfunction(SC_valname_surface);
	addvar(_qstr("DataMatrix"),SC_valname_matrix);
	addvar(_qstr("LongitudeMin"),SC_valname_scalar);
	addvar(_qstr("LongitudeMax"),SC_valname_scalar);
	addvar(_qstr("LattitudeMin"),SC_valname_scalar);
	addvar(_qstr("LattitudeMax"),SC_valname_scalar);
	addvar(_qstr("HeightFactor"),SC_valname_scalar);
	addvar(_qstr("Radius"),SC_valname_scalar);
}


void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectSurface *surf=G_valuecontent<T3DObjectSurface>(owner);
	TMatrix *mtr=G_valuecontent<TMatrix>(arglist->get(0));

	double longit_min=arglist->get(1)->G_content_scalar()->G_val();
	double longit_max=arglist->get(2)->G_content_scalar()->G_val();
	double lattit_min=arglist->get(3)->G_content_scalar()->G_val();
	double lattit_max=arglist->get(4)->G_content_scalar()->G_val();
	double hfactor=arglist->get(5)->G_content_scalar()->G_val();
	double radius=arglist->get(6)->G_content_scalar()->G_val();

	surf->LoadTopoSphere(mtr,longit_min,longit_max,lattit_min,lattit_max,hfactor,radius);
}
ENDFUNCTION(func_surface_create_topo_sphere)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_surface_copytoframe,CopyToFrame)
{
	setmemberfunction(SC_valname_surface);
	addvar(_qstr("Phase"),SC_valname_scalar);
}


void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectSurface *surf=G_valuecontent<T3DObjectSurface>(owner);
	double phase=arglist->get(0)->G_content_scalar()->G_val();
	surf->copytoframe(phase);
}
ENDFUNCTION(func_surface_copytoframe)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_surface_interpolframe,InterpolFrame)
{
	setmemberfunction(SC_valname_surface);
	addvar(_qstr("Phase"),SC_valname_scalar);
}


void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectSurface *surf=G_valuecontent<T3DObjectSurface>(owner);
	double phase=arglist->get(0)->G_content_scalar()->G_val();
	surf->interpolframe(phase);
}
ENDFUNCTION(func_surface_interpolframe)
