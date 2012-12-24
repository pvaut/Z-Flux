#include "stdafx.h"

#include "qfile.h"

#include "astrotools.h"

#include "3DScene.h"

#include "3DStarMap.h"

#include "SC_script.h"
#include "SC_functor.h"

#include "opengl.h"

StrPtr G_datadir();
StrPtr G_scriptsdir();
StrPtr G_texturesdir();


//******************************************************************
// T3DObjectStarMap
//******************************************************************

T3DObjectStarMap::T3DObjectStarMap(T3DScene *iscene) : T3DGeoObject(iscene)
{
	renderback->copyfrom(true);
	enablelight->copyfrom(false);

	blendtype->fromint(BlendTransparent);
	depthmask->fromint(DepthMaskDisable);


	sizex=addparam(_qstr("SizeX"),SC_valname_scalar)->content.G_content_scalar();
	sizey=addparam(_qstr("SizeY"),SC_valname_scalar)->content.G_content_scalar();

	offsetanglex=addparam(_qstr("OffsetAngleX"),SC_valname_scalar)->content.G_content_scalar();

	starsize=addparam(_qstr("StarSize"),SC_valname_scalar)->content.G_content_scalar();
	starsize->copyfrom(0.01);

	conlinesize=addparam(_qstr("LineSize"),SC_valname_scalar)->content.G_content_scalar();conlinesize->copyfrom(0.0);
	conlinecolor=G_valuecontent<TSC_color>(&addparam(_qstr("LineColor"),SC_valname_color)->content);conlinecolor->copyfrom(0,0,1,0.5);

	singleconstellationname=addparam(_qstr("SingleConstellationName"),SC_valname_string)->content.G_content_string();singleconstellationname->fromstring(_qstr(""));

	calculated=false;
}


void T3DObjectStarMap::load()
{
}

void T3DObjectStarMap::init()
{
	load();
}

double T3DObjectStarMap::mapx(double vl)
{
	double x0=(-vl-offsetanglex->G_val())/(2*Pi);
	while (x0>1) x0-=1;
	while (x0<0) x0+=1;
	return x0*sizex->G_val();
}

double T3DObjectStarMap::mapy(double vl)
{
	return (vl+Pi/2)/Pi*sizey->G_val();
}

void T3DObjectStarMap::addline(double xx1, double yy1, double xx2, double yy2)
{
	Tvec3d ps1,ps2,dr1,dr2,ps1a,ps1b,ps2a,ps2b;
	double linesize=conlinesize->G_val();

	ps1=Tvec3d(xx1,yy1,0);
	ps2=Tvec3d(xx2,yy2,0);
	dr1.lincomb(ps2,ps1,+1,-1);dr1.norm();
	dr2.vecprod(Tvec3d(0,0,1.0),dr1);dr2.norm();
	ps1a.lincomb(ps1,dr2,1,-linesize/2);
	ps1b.lincomb(ps1,dr2,1,+linesize/2);
	ps2a.lincomb(ps2,dr2,1,-linesize/2);
	ps2b.lincomb(ps2,dr2,1,+linesize/2);
	conlinesvertexbuffer.add(ps1a);conlinestexturebuffer.add(T2textureidx(0,0));
	conlinesvertexbuffer.add(ps2a);conlinestexturebuffer.add(T2textureidx(0,0));
	conlinesvertexbuffer.add(ps2b);conlinestexturebuffer.add(T2textureidx(1,1));
	conlinesvertexbuffer.add(ps1b);conlinestexturebuffer.add(T2textureidx(1,1));
}

void T3DObjectStarMap::calculate()
{
	double x0,y0,z0,gr,fc;
	double hx,hy,hz,vx,vy,vz;
	T4color col2,col2L,col2R;

	double sx=sizex->G_val();
	double sy=sizey->G_val();
	double ssize=starsize->G_val();

	if (calculated) return;

	Tdoublearray &stars_MAG=G_3DCosmos().stars_MAG;
	Tdoublearray &stars_E1=G_3DCosmos().stars_E1;
	Tdoublearray &stars_E2=G_3DCosmos().stars_E2;

	Tarray<QString> &const_code=G_3DCosmos().const_code;
	Tdoublearray &const_E1_1=G_3DCosmos().const_E1_1;
	Tdoublearray &const_E1_2=G_3DCosmos().const_E1_2;
	Tdoublearray &const_E2_1=G_3DCosmos().const_E2_1;
	Tdoublearray &const_E2_2=G_3DCosmos().const_E2_2;

	//for stars

	starvertexbuffer.reset();starcolorbufferL.reset();starcolorbufferR.reset();startexturebuffer.reset();
	if (ssize>0)
	{
		for (int i=0; i<stars_E1.G_count(); i++)
		{
			x0=mapx(stars_E1[i]);
			y0=mapy(stars_E2[i]);
			z0=0;
			gr=(7-stars_MAG[i])/7;
			gr=sqrt(gr);
			if (gr<0.15) gr=0.15;
			if (gr>1.0) gr=1.0;
			fc=ssize*gr;
			hx=fc;hy=0;hz=0;
			vx=0;vy=fc;vz=0;

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
	}

	//for constellation lines
	conlinesvertexbuffer.reset();
	conlinestexturebuffer.reset();

	QString constname=singleconstellationname->G_string();

	double linesize=conlinesize->G_val();
	if (linesize>1.0e-9)
	{
		for (int i=0; i<const_E1_1.G_count(); i++)
			if ((qstrlen(constname)==0)||(issame(*const_code[i],constname)))
		{
			double xx1=mapx(const_E1_1[i]);
			double xx2=mapx(const_E1_2[i]);
			double yy1=mapy(const_E2_1[i]);
			double yy2=mapy(const_E2_2[i]);
			if (xx1>xx2)
			{
				double tmp=xx1; xx1=xx2; xx2=tmp;
				tmp=yy1; yy1=yy2; yy2=tmp;
			}
			if (fabs(xx1-xx2)<sx/2)
			{
				addline(xx1,yy1,xx2,yy2);
			}
			else
			{
				addline(xx2-sx,yy1,xx1,yy2);
				addline(xx2,yy1,xx1+sx,yy2);
			}
		}
	}
	calculated=true;
}

void T3DObjectStarMap::precalcrender_exec(Tprecalcrendercontext *pre)
{
	calculate();
}


void T3DObjectStarMap::render_exec(Trendercontext *rc)
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


void T3DObjectStarMap::paramchanged(StrPtr iname)
{
	calculated=false;
}


/////////////////////////////////////////////////////////////////////////////////////
//FUNCTION(func_starglobe_getconlines,GetConLines)
//{
//	setmemberfunction(SC_valname_starmap);
//	addvar(_qstr("ConstellationID"),SC_valname_string);
//	setreturntype(SC_valname_list);
//	//addvar(_qstr("Content"),SC_valname_string);
//	//addvar(_qstr("ID"),SC_valname_string,false);
//	//addvar(_qstr("CanCheck"),SC_valname_boolean,false);
//}
//void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
//{
//	TSC_list *list=G_valuecontent<TSC_list>(retval);
//	list->reset();
//	T3DObjectStarMap *starglobe=G_valuecontent<T3DObjectStarMap>(owner);
//	StrPtr constid=arglist->get(0)->G_content_string()->G_string();
//
//	Tarray<QString> &const_code=G_3DCosmos().const_code;
//	Tdoublearray &const_X_1=G_3DCosmos().const_X_1;
//	Tdoublearray &const_X_2=G_3DCosmos().const_X_2;
//	Tdoublearray &const_Y_1=G_3DCosmos().const_Y_1;
//	Tdoublearray &const_Y_2=G_3DCosmos().const_Y_2;
//	Tdoublearray &const_Z_1=G_3DCosmos().const_Z_1;
//	Tdoublearray &const_Z_2=G_3DCosmos().const_Z_2;
//
//	double rd=starglobe->radius->G_val();
//	for (int i=0; i<const_code.G_count(); i++)
//		if (issame(constid,*const_code[i]))
//		{
//			TSC_value val_map;
//			val_map.createtype(GetTSCenv().G_datatype(SC_valname_map));
//			TSC_map *map=G_valuecontent<TSC_map>(&val_map);
//			{
//				TSC_value val_pt;
//				val_pt.createtype(GetTSCenv().G_datatype(SC_valname_vertex));
//				Tvertex *pt=G_valuecontent<Tvertex>(&val_pt);
//				pt->copyfrom(rd*const_X_1[i],rd*const_Y_1[i],rd*const_Z_1[i]);
//				map->Set(_qstr("Point1"),&val_pt);
//			}
//			{
//				TSC_value val_pt;
//				val_pt.createtype(GetTSCenv().G_datatype(SC_valname_vertex));
//				Tvertex *pt=G_valuecontent<Tvertex>(&val_pt);
//				pt->copyfrom(rd*const_X_2[i],rd*const_Y_2[i],rd*const_Z_2[i]);
//				map->Set(_qstr("Point2"),&val_pt);
//			}
//			list->add(&val_map);
//		}
//
//
//}
//ENDFUNCTION(func_starglobe_getconlines)
