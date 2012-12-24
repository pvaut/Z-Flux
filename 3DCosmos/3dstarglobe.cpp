#include "stdafx.h"

#include "qfile.h"

#include "astrotools.h"

#include "3DScene.h"

#include "3DStarGlobe.h"

#include "SC_script.h"
#include "SC_functor.h"

#include "opengl.h"

StrPtr G_datadir();
StrPtr G_scriptsdir();
StrPtr G_texturesdir();


//******************************************************************
// T3DObjectStarGlobe
//******************************************************************

T3DObjectStarGlobe::T3DObjectStarGlobe(T3DScene *iscene) : T3DGeoObject(iscene)
{
	renderback->copyfrom(true);
	enablelight->copyfrom(false);

	blendtype->fromint(BlendTransparent);
	depthmask->fromint(DepthMaskDisable);


	radius=addparam(_qstr("Radius"),SC_valname_scalar)->content.G_content_scalar();
	starsize=addparam(_qstr("StarSize"),SC_valname_scalar)->content.G_content_scalar();
	starsize->copyfrom(0.01);

	conlinesize=addparam(_qstr("LineSize"),SC_valname_scalar)->content.G_content_scalar();conlinesize->copyfrom(0.0);
	conlinecolor=G_valuecontent<TSC_color>(&addparam(_qstr("LineColor"),SC_valname_color)->content);conlinecolor->copyfrom(0,0,1,0.5);



	calculated=false;

/*	isclosed=addparam(_qstr("IsClosed"),SC_valname_boolean)->content.G_content_boolean();
	isclosed->copyfrom(false);*/

//	curverendertype=&addparam(SC_valname_curverendertype,SC_valname_curverendertype)->content;

}


void T3DObjectStarGlobe::load()
{
}

void T3DObjectStarGlobe::init()
{
	load();
}

void T3DObjectStarGlobe::calculate()
{
	double x0,y0,z0,gr,fc;
	double hx,hy,hz,vx,vy,vz;
	T4color col2,col2L,col2R;

	double rd=radius->G_val();
	double ssize=starsize->G_val();

	if (calculated) return;

	Tdoublearray &stars_MAG=G_3DCosmos().stars_MAG;
	Tdoublearray &stars_X=G_3DCosmos().stars_X;
	Tdoublearray &stars_Y=G_3DCosmos().stars_Y;
	Tdoublearray &stars_Z=G_3DCosmos().stars_Z;
	Tdoublearray &dir1_X=G_3DCosmos().dir1_X;
	Tdoublearray &dir1_Y=G_3DCosmos().dir1_Y;
	Tdoublearray &dir1_Z=G_3DCosmos().dir1_Z;
	Tdoublearray &dir2_X=G_3DCosmos().dir2_X;
	Tdoublearray &dir2_Y=G_3DCosmos().dir2_Y;
	Tdoublearray &dir2_Z=G_3DCosmos().dir2_Z;

	Tarray<QString> &const_code=G_3DCosmos().const_code;
	Tdoublearray &const_X_1=G_3DCosmos().const_X_1;
	Tdoublearray &const_X_2=G_3DCosmos().const_X_2;
	Tdoublearray &const_Y_1=G_3DCosmos().const_Y_1;
	Tdoublearray &const_Y_2=G_3DCosmos().const_Y_2;
	Tdoublearray &const_Z_1=G_3DCosmos().const_Z_1;
	Tdoublearray &const_Z_2=G_3DCosmos().const_Z_2;



	//for stars

	starvertexbuffer.reset();starcolorbufferL.reset();starcolorbufferR.reset();startexturebuffer.reset();
	for (int i=0; i<stars_X.G_count(); i++)
	{
		x0=stars_X[i]*rd;
		y0=stars_Y[i]*rd;
		z0=stars_Z[i]*rd;
		gr=(7-stars_MAG[i])/7;
		gr=sqrt(gr);
		if (gr<0.15) gr=0.15;
		if (gr>1.0) gr=1.0;
		fc=ssize*gr;
		hx=dir1_X[i]*fc;hy=dir1_Y[i]*fc;hz=dir1_Z[i]*fc;
		vx=dir2_X[i]*fc;vy=dir2_Y[i]*fc;vz=dir2_Z[i]*fc;

		col2.r=gr*color->G_R();
		col2.g=gr*color->G_G();
		col2.b=gr*color->G_B();
		col2.a=1*color->G_A();
		col2L=col2;col2L.ColMapL();
		col2R=col2;col2R.ColMapR();

		startexturebuffer.add(T2textureidx(0,0));
		starvertexbuffer.add(Tvec3d(x0-hx-vx,y0-hy-vy,z0-hz-vz));

		startexturebuffer.add(T2textureidx(0,1));
		starvertexbuffer.add(Tvec3d(x0-hx+vx,y0-hy+vy,z0-hz+vz));

		startexturebuffer.add(T2textureidx(1,1));
		starvertexbuffer.add(Tvec3d(x0+hx+vx,y0+hy+vy,z0+hz+vz));

		startexturebuffer.add(T2textureidx(1,0));
		starvertexbuffer.add(Tvec3d(x0+hx-vx,y0+hy-vy,z0+hz-vz));


		starcolorbufferL.add(col2L);starcolorbufferL.add(col2L);starcolorbufferL.add(col2L);starcolorbufferL.add(col2L);
		starcolorbufferR.add(col2R);starcolorbufferR.add(col2R);starcolorbufferR.add(col2R);starcolorbufferR.add(col2R);
	}

	//for constellation lines
	conlinesvertexbuffer.reset();
	conlinestexturebuffer.reset();

	double linesize=conlinesize->G_val();
	Tvec3d ps1,ps2,dr1,dr2,ps1a,ps1b,ps2a,ps2b;
	if (linesize>1.0e-9)
	{
		for (int i=0; i<const_X_1.G_count(); i++)
		{
			ps1=Tvec3d(rd*const_X_1[i],rd*const_Y_1[i],rd*const_Z_1[i]);
			ps2=Tvec3d(rd*const_X_2[i],rd*const_Y_2[i],rd*const_Z_2[i]);
			dr1.lincomb(ps2,ps1,+1,-1);dr1.norm();
			dr2.vecprod(ps1,dr1);dr2.norm();
			ps1a.lincomb(ps1,dr2,1,-linesize/2);
			ps1b.lincomb(ps1,dr2,1,+linesize/2);
			ps2a.lincomb(ps2,dr2,1,-linesize/2);
			ps2b.lincomb(ps2,dr2,1,+linesize/2);
			conlinesvertexbuffer.add(ps1a);conlinestexturebuffer.add(T2textureidx(0,0));
			conlinesvertexbuffer.add(ps2a);conlinestexturebuffer.add(T2textureidx(0,0));
			conlinesvertexbuffer.add(ps2b);conlinestexturebuffer.add(T2textureidx(1,1));
			conlinesvertexbuffer.add(ps1b);conlinestexturebuffer.add(T2textureidx(1,1));
		}
	}
	calculated=true;
}

void T3DObjectStarGlobe::precalcrender_exec(Tprecalcrendercontext *pre)
{
	calculate();
}


void T3DObjectStarGlobe::render_exec(Trendercontext *rc)
{
	//stars
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,starvertexbuffer.G_ptr());

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,startexturebuffer.G_ptr());

	glEnableClientState(GL_COLOR_ARRAY);
	if (!rc->isrightpart) glColorPointer(4,GL_FLOAT,0,starcolorbufferL.G_ptr());
	else glColorPointer(4,GL_FLOAT,0,starcolorbufferR.G_ptr());

	glDrawArrays(GL_QUADS,0,starvertexbuffer.G_count());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	//constellation lines
	if (conlinesvertexbuffer.G_count()>0)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,conlinesvertexbuffer.G_ptr());

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,conlinestexturebuffer.G_ptr());

		rc->set_color(conlinecolor->G_R()*color->G_R(),conlinecolor->G_G()*color->G_G(),conlinecolor->G_B()*color->G_B(),conlinecolor->G_A()*color->G_A());
		glDrawArrays(GL_QUADS,0,conlinesvertexbuffer.G_count());

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}



}


void T3DObjectStarGlobe::paramchanged(StrPtr iname)
{
	calculated=false;
}


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_starglobe_getconlines,GetConLines)
{
	setmemberfunction(SC_valname_starglobe);
	addvar(_qstr("ConstellationID"),SC_valname_string);
	setreturntype(SC_valname_list);
	//addvar(_qstr("Content"),SC_valname_string);
	//addvar(_qstr("ID"),SC_valname_string,false);
	//addvar(_qstr("CanCheck"),SC_valname_boolean,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_list *list=G_valuecontent<TSC_list>(retval);
	list->reset();
	T3DObjectStarGlobe *starglobe=G_valuecontent<T3DObjectStarGlobe>(owner);
	StrPtr constid=arglist->get(0)->G_content_string()->G_string();

	Tarray<QString> &const_code=G_3DCosmos().const_code;
	Tdoublearray &const_X_1=G_3DCosmos().const_X_1;
	Tdoublearray &const_X_2=G_3DCosmos().const_X_2;
	Tdoublearray &const_Y_1=G_3DCosmos().const_Y_1;
	Tdoublearray &const_Y_2=G_3DCosmos().const_Y_2;
	Tdoublearray &const_Z_1=G_3DCosmos().const_Z_1;
	Tdoublearray &const_Z_2=G_3DCosmos().const_Z_2;

	double rd=starglobe->radius->G_val();
	for (int i=0; i<const_code.G_count(); i++)
		if (issame(constid,*const_code[i]))
		{
			TSC_value val_map;
			val_map.createtype(GetTSCenv().G_datatype(SC_valname_map));
			TSC_map *map=G_valuecontent<TSC_map>(&val_map);
			{
				TSC_value val_pt;
				val_pt.createtype(GetTSCenv().G_datatype(SC_valname_vertex));
				Tvertex *pt=G_valuecontent<Tvertex>(&val_pt);
				pt->copyfrom(rd*const_X_1[i],rd*const_Y_1[i],rd*const_Z_1[i]);
				map->Set(_qstr("Point1"),&val_pt);
			}
			{
				TSC_value val_pt;
				val_pt.createtype(GetTSCenv().G_datatype(SC_valname_vertex));
				Tvertex *pt=G_valuecontent<Tvertex>(&val_pt);
				pt->copyfrom(rd*const_X_2[i],rd*const_Y_2[i],rd*const_Z_2[i]);
				map->Set(_qstr("Point2"),&val_pt);
			}
			list->add(&val_map);
		}


}
ENDFUNCTION(func_starglobe_getconlines)
