#include "stdafx.h"
#include "3DScene.h"

#include "3DCurve.h"

#include "SC_script.h"
#include "SC_functor.h"

#include "bezier.h"

#include "opengl.h"


//******************************************************************
// T3DObjectCurve
//******************************************************************

T3DObjectCurve::T3DObjectCurve(T3DScene *iscene) : T3DGeoObject(iscene)
{
	size=addparam(_qstr("Size"),SC_valname_scalar)->content.G_content_scalar();
	arrow1size=addparam(_qstr("Arrow1Size"),SC_valname_scalar)->content.G_content_scalar();
	arrow2size=addparam(_qstr("Arrow2Size"),SC_valname_scalar)->content.G_content_scalar();
	isclosed=addparam(_qstr("IsClosed"),SC_valname_boolean)->content.G_content_boolean();
	renderback->copyfrom(true);
	enablelight->copyfrom(false);
	isclosed->copyfrom(false);

	curverendertype=&addparam(SC_valname_curverendertype,SC_valname_curverendertype)->content;

	calculated=false;

	hasowncolors=false;
	mintrackdist=1.0;
}

void T3DObjectCurve::calculate()
{
	int ii;

	if (calculated) return;

	//calculate normals
	int i1,i2,i3;
	Tvec3d v1,v2,dr1,normal,dr2;
	if (points.G_count()<3)
	{
		return;
		//throw QError(_text("Ribbon-style curves need at least 3 non-colinear points"));
	}

	dir1.reset();norm1.reset();norm2.reset();
	for (int i=0; i<points.G_count(); i++)
	{
		i1=i-1;i2=i;i3=i+1;
		if (!G_isclosed())
		{
			if (i3>=points.G_count()) { i1--; i2--; i3--; }
			if (i1<0) { i1++; i2++; i3++; }
		}
		else
		{
			if (i3>=points.G_count()) i3-=points.G_count();
			if (i1<0) i1+=points.G_count();
		}
		v1.lincomb(points[i1],points[i2],+1,-1);v1.norm();
		v2.lincomb(points[i3],points[i2],+1,-1);v2.norm();
		dr1.lincomb(v2,v1,+1,-1);dr1.norm();
		normal.lincomb(v1,v2,+1,+1);normal.norm();
		dr2.vecprod(dr1,normal);dr2.norm();
		dir1.add(dr1);norm1.add(normal);norm2.add(dr2);
	}

	if (G_curverendertype()==CurveRenderRibbon)
	{
		vertices.reset();
		vertices_normals.reset();
		vertices_texcoord.reset();

		int ctt=points.G_count();
		if (G_isclosed()) ctt++;
		if (points.G_count()<2) return;

		double size2=G_size()/2;
		for (int i=0; i<ctt; i++)
		{
			ii=i%points.G_count();
			vertices_normals.add(Tvec3d(norm1[ii].x,norm1[ii].y,norm1[ii].z));
			vertices_normals.add(Tvec3d(norm1[ii].x,norm1[ii].y,norm1[ii].z));
			vertices.add(Tvec3d(points[ii].x-size2*norm2[ii].x,points[ii].y-size2*norm2[ii].y,points[ii].z-size2*norm2[ii].z));
			vertices.add(Tvec3d(points[ii].x+size2*norm2[ii].x,points[ii].y+size2*norm2[ii].y,points[ii].z+size2*norm2[ii].z));
			vertices_texcoord.add(T2textureidx(0,0));
			vertices_texcoord.add(T2textureidx(1,1));
		}
	}
	calculated=true;
}


void T3DObjectCurve::streamout_content(QBinTagWriter &writer)
{
	calculate();
	{
		QBinTagWriter &ptag=writer.subtag(_qstr("Params")).G_obj();
		ptag.write_bool(hasowncolors);
		ptag.write_double(mintrackdist);
		for (int i=0; i<10; i++) ptag.write_bool(false);//buffer for future use
		for (int i=0; i<10; i++) ptag.write_int32(0);//buffer for future use
		ptag.close();
	}
	{
		QBinTagWriter &dtag=writer.subtag(_qstr("Data")).G_obj();
		points.streamout(writer);
		colors.streamout(writer);
		dir1.streamout(writer);
		norm1.streamout(writer);
		norm2.streamout(writer);
		vertices.streamout(writer);
		vertices_normals.streamout(writer);
		vertices_texcoord.streamout(writer);
		dtag.close();
	}
}

void T3DObjectCurve::streamin_content(QBinTagReader &reader)
{
	while (reader.hassubtags())
	{
		QBinTagReader &tg1=reader.getnextsubtag().G_obj();
		if (tg1.Istag(_qstr("Params")))
		{
			hasowncolors=tg1.read_bool();
			mintrackdist=tg1.read_double();
		}
		if (tg1.Istag(_qstr("Data")))
		{
			points.streamin(reader);
			colors.streamin(reader);
			dir1.streamin(reader);
			norm1.streamin(reader);
			norm2.streamin(reader);
			vertices.streamin(reader);
			vertices_normals.streamin(reader);
			vertices_texcoord.streamin(reader);
		}
		tg1.close();
	}
	calculated=true;
}


void T3DObjectCurve::precalcrender_exec(Tprecalcrendercontext *pre)
{
	track_update();

	if ((G_curverendertype()==CurveRenderRibbon)||(G_curverendertype()==CurveRenderCube)||(G_curverendertype()==CurveRenderTube))
		calculate();
}

void T3DObjectCurve::renderarrow(Trendercontext *rc, Tvec3d &ptend, Tvec3d &pt2, double size)
{
	Tvec3d dir;
	dir.lincomb(pt2,ptend,+1,-1);
	dir.norm();
	dir.multiply(size);


	Tvec3d n1;n1.vecprod(dir,Tvec3d(1,0,0));
	Tvec3d n2;n2.vecprod(dir,Tvec3d(0,1,0));
	if (n1.getsize()<n2.getsize()) n1=n2;
	n1.norm();n1.multiply(size/4);

	n2.vecprod(n1,dir);n2.norm();n2.multiply(size/4);

	Tvec3d pta,ptb;

	pta.lincomb(ptend,dir,1,1);

	glBegin(GL_TRIANGLES);
	glVertex3d(ptend.x,ptend.y,ptend.z);
	ptb.lincomb(pta,n1,1,+1); glVertex3d(ptb.x,ptb.y,ptb.z);
	ptb.lincomb(pta,n1,1,-1); glVertex3d(ptb.x,ptb.y,ptb.z);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3d(ptend.x,ptend.y,ptend.z);
	ptb.lincomb(pta,n2,1,+1); glVertex3d(ptb.x,ptb.y,ptb.z);
	ptb.lincomb(pta,n2,1,-1); glVertex3d(ptb.x,ptb.y,ptb.z);
	glEnd();
}

void T3DObjectCurve::render_exec(Trendercontext *rc)
{
	int ctt=points.G_count();
	int ii;
	if (G_isclosed()) ctt++;
	if (points.G_count()<2) return;

	bool hastexture=rc->G_istextureactive();
	bool lightenabled=rc->G_lightenabled();


	if ( (G_curverendertype()==CurveRenderNormal) || (G_curverendertype()==CurveRenderDot)  || (G_curverendertype()==CurveRenderDash)  || (G_curverendertype()==CurveRenderDashDot) )
	{
		double lw=G_size();
		if (lw<=0.0001) lw=1;
		glLineWidth((GLfloat)lw);

		if (G_curverendertype()!=CurveRenderNormal) glEnable(GL_LINE_STIPPLE);
		if (G_curverendertype()==CurveRenderDot)     glLineStipple(2+(int)(0.5*G_size()), (short) 0xAAAA);
		if (G_curverendertype()==CurveRenderDash)    glLineStipple(1+(int)(0.5*G_size()), (short) 0x1F1F);
		if (G_curverendertype()==CurveRenderDashDot) glLineStipple(2+(int)(0.5*G_size()), (short) 0xD6D6);

		glEnableClientState(GL_VERTEX_ARRAY);glVertexPointer(3,GL_FLOAT,0,points.G_ptr());
		if (hasowncolors) { glEnableClientState(GL_COLOR_ARRAY); glColorPointer(4,GL_FLOAT,0,colors.G_ptr()); }

		if (!G_isclosed()) glDrawArrays(GL_LINE_STRIP,0,points.G_count());
		else glDrawArrays(GL_LINE_LOOP,0,points.G_count());

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		if ((G_curverendertype()==CurveRenderDot)  || (G_curverendertype()==CurveRenderDash)  || (G_curverendertype()==CurveRenderDashDot)) glDisable(GL_LINE_STIPPLE);

		if (points.G_count()>1)
		{
			if (arrow1size->G_val()>1.0e-9) renderarrow(rc,points[0],points[1],arrow1size->G_val());
			if (arrow2size->G_val()>1.0e-9) renderarrow(rc,points[points.G_count()-1],points[points.G_count()-2],arrow2size->G_val());
		}

		return;
	}

	if (G_curverendertype()==CurveRenderSmooth)
	{
		Tvec3d dir1,dir2,eyedir;
		Tvector veyedir;
		int i1,i2,i3;
		double size2=G_size()/2;
		if (qstrlen(G_texturename())==0) rc->lightpoint1.select(rc);
		glBegin(GL_TRIANGLE_STRIP);
		parentframe->G_rendertransformation()->mulinv(rc->G_eyedir_depth(),&veyedir);
		eyedir=veyedir;
		for (int i=0; i<ctt; i++)
		{
			ii=i%points.G_count();
			if (!G_isclosed())
			{
				if (i==0) dir1.lincomb(points[1],points[0],+1,-1);
				if (i==points.G_count()-1) dir1.lincomb(points[i],points[i-1],+1,-1);
				if ((i>0)&&(i<points.G_count()))
				{
					i1=i-1;i2=i;i3=i+1;
					dir1.lincomb(points[i3],points[i1],+1,-1);
				}
			}
			else
			{
				i1=i-1;i2=i;i3=i+1;
				if (i1<0) i1+=points.G_count();
				if (i3>=points.G_count()) i3-=points.G_count();
				dir1.lincomb(points[i3],points[i1],+1,-1);
			}
			dir1.norm();
			dir2.vecprod(dir1,eyedir);dir2.y+=(GLfloat)0.1;dir2.norm();
			glTexCoord2d(0,0);
			glVertex3d(points[ii].x-size2*dir2.x,points[ii].y-size2*dir2.y,points[ii].z-size2*dir2.z);
			glTexCoord2d(1,1);
			glVertex3d(points[ii].x+size2*dir2.x,points[ii].y+size2*dir2.y,points[ii].z+size2*dir2.z);
		}
		glEnd();
		if (qstrlen(G_texturename())==0) rc->lightpoint1.unselect(rc);
	}


	if (G_curverendertype()==CurveRenderRibbon)
	{
		glEnableClientState(GL_VERTEX_ARRAY);glVertexPointer(3,GL_FLOAT,0,vertices.G_ptr());
		if (hastexture) { glEnableClientState(GL_TEXTURE_COORD_ARRAY);glTexCoordPointer(2,GL_FLOAT,0,vertices_texcoord.G_ptr()); }
		if (lightenabled) { glEnableClientState(GL_NORMAL_ARRAY);glNormalPointer(GL_FLOAT,0,vertices_normals.G_ptr()); }
		glDrawArrays(GL_TRIANGLE_STRIP,0,vertices.G_count());
		glDisableClientState(GL_VERTEX_ARRAY);
		if (hastexture) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		if (lightenabled) glDisableClientState(GL_NORMAL_ARRAY);
		return;
	}

	if (G_curverendertype()==CurveRenderCube)
	{
		double size2=G_size()/2;
		glBegin(GL_TRIANGLE_STRIP);
		for (int i=0; i<ctt; i++)
		{
			ii=i%points.G_count();
			glNormal3d(norm1[ii].x,norm1[ii].y,norm1[ii].z);
			glVertex3d(points[ii].x-size2*norm2[ii].x-size2*norm1[ii].x,points[ii].y-size2*norm2[ii].y-size2*norm1[ii].y,points[i].z-size2*norm2[ii].z-size2*norm1[ii].z);
			glVertex3d(points[ii].x+size2*norm2[ii].x-size2*norm1[ii].x,points[ii].y+size2*norm2[ii].y-size2*norm1[ii].y,points[i].z+size2*norm2[ii].z-size2*norm1[ii].z);
		}
		glEnd();
		glBegin(GL_TRIANGLE_STRIP);
		for (int i=0; i<ctt; i++)
		{
			ii=i%points.G_count();
			glNormal3d(norm1[ii].x,norm1[ii].y,norm1[ii].z);
			glVertex3d(points[ii].x-size2*norm2[ii].x+size2*norm1[ii].x,points[ii].y-size2*norm2[ii].y+size2*norm1[ii].y,points[ii].z-size2*norm2[ii].z+size2*norm1[ii].z);
			glVertex3d(points[ii].x+size2*norm2[ii].x+size2*norm1[ii].x,points[ii].y+size2*norm2[ii].y+size2*norm1[ii].y,points[ii].z+size2*norm2[ii].z+size2*norm1[ii].z);
		}
		glEnd();
		glBegin(GL_TRIANGLE_STRIP);
		for (int i=0; i<ctt; i++)
		{
			ii=i%points.G_count();
			glNormal3d(norm2[ii].x,norm2[ii].y,norm2[ii].z);
			glVertex3d(points[ii].x-size2*norm1[ii].x-size2*norm2[ii].x,points[ii].y-size2*norm1[ii].y-size2*norm2[ii].y,points[ii].z-size2*norm1[ii].z-size2*norm2[ii].z);
			glVertex3d(points[ii].x+size2*norm1[ii].x-size2*norm2[ii].x,points[ii].y+size2*norm1[ii].y-size2*norm2[ii].y,points[ii].z+size2*norm1[ii].z-size2*norm2[ii].z);
		}
		glEnd();
		glBegin(GL_TRIANGLE_STRIP);
		for (int i=0; i<ctt; i++)
		{
			ii=i%points.G_count();
			glNormal3d(norm2[ii].x,norm2[ii].y,norm2[ii].z);
			glVertex3d(points[ii].x-size2*norm1[ii].x+size2*norm2[ii].x,points[ii].y-size2*norm1[ii].y+size2*norm2[ii].y,points[ii].z-size2*norm1[ii].z+size2*norm2[ii].z);
			glVertex3d(points[ii].x+size2*norm1[ii].x+size2*norm2[ii].x,points[ii].y+size2*norm1[ii].y+size2*norm2[ii].y,points[ii].z+size2*norm1[ii].z+size2*norm2[ii].z);
		}
		glEnd();
		return;
	}

	if (G_curverendertype()==CurveRenderTube)
	{
		double size2=G_size()/2;
		double ang1,ang2,cs1,sn1,cs2,sn2;
		Tvec3d dr1,dr2;

		int resol=6;
		for (int rs=0; rs<resol; rs++)
		{
			ang1=(rs+0)*2*Pi/resol;cs1=cos(ang1);sn1=sin(ang1);
			ang2=(rs+1)*2*Pi/resol;cs2=cos(ang2);sn2=sin(ang2);
			glBegin(GL_TRIANGLE_STRIP);
			for (int i=0; i<ctt; i++)
			{
				ii=i%points.G_count();
				dr1.lincomb(norm1[ii],norm2[ii],cs1,sn1);
				dr2.lincomb(norm1[ii],norm2[ii],cs2,sn2);
				glNormal3d(dr1.x,dr1.y,dr1.z);
				glVertex3d(points[ii].x+size2*dr1.x,points[ii].y+size2*dr1.y,points[ii].z+size2*dr1.z);
				glNormal3d(dr2.x,dr2.y,dr2.z);
				glVertex3d(points[ii].x+size2*dr2.x,points[ii].y+size2*dr2.y,points[ii].z+size2*dr2.z);
			}
			glEnd();
		}
		return;
	}

}


void T3DObjectCurve::paramchanged(StrPtr iname)
{
	if (qstricmp(iname,SC_valname_curverendertype)==0)
	{
		if (G_curverendertype()==CurveRenderNormal)
		{
			cancache->copyfrom(true);
			enablelight->copyfrom(false);
			blendtype->fromint(BlendNormal);
			depthmask->fromint(DepthMaskEnable);
		}
		if ((G_curverendertype()==CurveRenderRibbon)||(G_curverendertype()==CurveRenderTube)||(G_curverendertype()==CurveRenderCube))
		{
			cancache->copyfrom(true);
			//enablelight->copyfrom(true);
			//blendtype->fromint(BlendNormal);
			//depthmask->fromint(DepthMaskEnable);
		}

		if (G_curverendertype()==CurveRenderSmooth)
		{
			cancache->copyfrom(false);
			enablelight->copyfrom(false);
			blendtype->fromint(BlendTransparent);
			depthmask->fromint(DepthMaskDisable);
		}
	}
	calculated=false;
}



void T3DObjectCurve::reset()
{
	points.reset();dir1.reset();norm1.reset();norm2.reset();
	isclosed->copyfrom(false);
	isdirty=true;
	calculated=false;
	hasowncolors=false;
}

void T3DObjectCurve::addpoint(Tvertex *pt)
{
	Tvec3d vc;vc=*pt;
	points.add(vc);
	colors.add(T4color(1,1,1,1));
	isdirty=true;
	calculated=false;
}

void T3DObjectCurve::Set_color(int idx, const TSC_color &icol)
{
	if ((idx<0)||(idx>=colors.G_count())) throw QError(_text("Invalid curve point index"));
	colors[idx]=T4color(icol.G_R(),icol.G_G(),icol.G_B(),icol.G_A());
	hasowncolors=true;
}


void T3DObjectCurve::close()
{
	isclosed->copyfrom(true);
	isdirty=true;
	calculated=false;
}


void T3DObjectCurve::generate(TSC_functor *functor, double minv, double maxv, int count)
{
	if (count<2) throw QError(_qstr("Generate curve: invalid number of points"));

	reset();
	try{
		double vl;
		TSC_funcarglist arglist(NULL);
		TSC_value retval0,arg;
		arg.createtype(GetTSCenv().G_datatype(SC_valname_scalar));
		arglist.add(&arg);

		for (int i=0; i<count; i++)
		{
			vl=minv+(maxv-minv)*i/(count-1.0);
			arg.copyfrom(vl);
			functor->eval(&arglist,&retval0);
			addpoint(G_valuecontent<Tvertex>(&retval0));
		}
	}
	catch(TSC_runerror err)
	{
		QString errstring;
		FormatString(errstring,_text("Generate curve: ^1"),err.G_content());
		throw QError(errstring);
	}
	catch(QError err)
	{
		QString errstring;
		FormatString(errstring,_text("Generate curve: ^1"),err.G_content());
		throw QError(errstring);
	}
}

void T3DObjectCurve::generate(TPolyBezier3D *bezier, int count)
{
	if (count<2) throw QError(_qstr("Generate bezier curve: invalid number of points"));

	double t;
	Tvertex rs;
	reset();
	for (int i=0; i<count; i++)
	{
		t=bezier->G_minxval()+(bezier->G_maxxval()-bezier->G_minxval())*i/(count-1.0);
		bezier->eval(t,rs);
		addpoint(&rs);
	}
	if (bezier->G_isclosed()) close();
}


void T3DObjectCurve::makecircle(const Tvertex *center, const Tvector *normal, double radius, int resolution)
{
	double ang;
	reset();
	isclosed->copyfrom(true);
	Tvector norm1,norm2,dr1;
	Tvertex ps;
	norm1.anynormalfrom(normal);
	norm2.vecprod(normal,&norm1);norm2.normfrom(&norm2);
	for (int i=0; i<resolution; i++)
	{
		ang=i*2.0*Pi/resolution;
		dr1.lincombfrom(radius*cos(ang),&norm1,radius*sin(ang),&norm2);
		ps.add(center,&dr1);
		addpoint(&ps);
	}
}

void T3DObjectCurve::makearc(const Tvertex *center, const Tvector *dir1, const Tvector *dir2, double radius, int resolution)
{
	double ang;
	reset();
	double ang0=Tvector::angle(dir1,dir2);
	isclosed->copyfrom(false);


	Tvector norm1,norm,dirn1,dr1;
	Tvertex ps;
	norm1.vecprod(dir1,dir2);
	norm.vecprod(&norm1,dir1);
	norm.normfrom(&norm);
	dirn1.normfrom(dir1);
	for (int i=0; i<resolution; i++)
	{
		ang=i*1.0/resolution*ang0;
		dr1.lincombfrom(radius*cos(ang),&dirn1,radius*sin(ang),&norm);
		ps.add(center,&dr1);
		addpoint(&ps);
	}
}


void T3DObjectCurve::transform(const Taffinetransformation *transf)
{
	Tvertex pt0,pt1;
	for (int i=0; i<points.G_count(); i++)
	{
		pt0.S3_x(points[i].x);pt0.S3_y(points[i].y);pt0.S3_z(points[i].z);
		transf->mul(&pt0,&pt1);
		points[i]=pt1;
	}
}


bool T3DObjectCurve::G_cancache()
{
	if (G_curverendertype()==CurveRenderSmooth) return false;
	return T3DGeoObject::G_cancache();
}

void T3DObjectCurve::set_track(T3DSubFrameObject *itrackframe, double imintrackdist)
{
	reset();
	mintrackdist=imintrackdist;
	trackframe.setreffor(itrackframe);
	track_lastpoint.copyfrom(-1.0e9,-1.0e9,-1.0e9);
}

void T3DObjectCurve::stop_track()
{
	trackframe.setreffor(NULL);
}


void T3DObjectCurve::track_update()
{
	if (trackframe.G_ref()==NULL) return;
	T3DSubFrameObject *subframe=trackframe.G_ref();

	Taffinetransformation transf1,transf2;
	subframe->G_totaltransformation(&transf1);
	G_parentframe()->G_totaltransformation(&transf2);

	Tvertex rs1,rs2;
	transf1.mul(&Tvertex(0,0,0),&rs1);
	transf2.mulinv(&rs1,&rs2);

	if (Tvertex::distance(&rs2,&track_lastpoint)>=mintrackdist)
	{
		addpoint(&rs2);
		track_lastpoint=rs2;
	}
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_curve_addpoint,addpoint)
{
	setmemberfunction(SC_valname_curve);
	addvar(_qstr("position"),SC_valname_vertex);
	addvar(_qstr("color"),SC_valname_color,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(owner);
	curve->addpoint(G_valuecontent<Tvertex>(arglist->get(0)));
	if (arglist->G_ispresent(1))
	{
		curve->Set_color(curve->G_count()-1,*G_valuecontent<TSC_color>(arglist->get(1)));
	}
}
ENDFUNCTION(func_curve_addpoint)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_curve_makeline,makeline)
{
	setmemberfunction(SC_valname_curve);
	addvar(_qstr("from"),SC_valname_vertex);
	addvar(_qstr("to"),SC_valname_vertex);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(owner);
	curve->reset();
	curve->addpoint(G_valuecontent<Tvertex>(arglist->get(0)));
	curve->addpoint(G_valuecontent<Tvertex>(arglist->get(1)));
}
ENDFUNCTION(func_curve_makeline)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_curve_reset,reset)
{
	setmemberfunction(SC_valname_curve);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(owner);
	curve->reset();
}
ENDFUNCTION(func_curve_reset)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_curve_makecircle,makecircle)
{
//	setreturntype(SC_valname_textflat);
	setmemberfunction(SC_valname_curve);
	addvar(_qstr("center"),SC_valname_vertex);
	addvar(_qstr("normal"),SC_valname_vector);
	addvar(_qstr("radius"),SC_valname_scalar);
	addvar(_qstr("resolution"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(owner);
	curve->makecircle(
		G_valuecontent<Tvertex>(arglist->get(0)),
		G_valuecontent<Tvector>(arglist->get(1)),
		arglist->get(2)->G_content_scalar()->G_val(),
		arglist->get(3)->G_content_scalar()->G_intval());
}
ENDFUNCTION(func_curve_makecircle)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_curve_makearc,makearc)
{
	setmemberfunction(SC_valname_curve);
	addvar(_qstr("center"),SC_valname_vertex);
	addvar(_qstr("stardir"),SC_valname_vector);
	addvar(_qstr("enddir"),SC_valname_vector);
	addvar(_qstr("radius"),SC_valname_scalar);
	addvar(_qstr("resolution"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(owner);
	curve->makearc(
		G_valuecontent<Tvertex>(arglist->get(0)),
		G_valuecontent<Tvector>(arglist->get(1)),
		G_valuecontent<Tvector>(arglist->get(2)),
		arglist->get(3)->G_content_scalar()->G_val(),
		arglist->get(4)->G_content_scalar()->G_intval());
}
ENDFUNCTION(func_curve_makearc)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_curve_generate,Generate)
{
	setmemberfunction(SC_valname_curve);
	addvar(_qstr("function"),SC_valname_functor);
	addvar(_qstr("min"),SC_valname_scalar);
	addvar(_qstr("max"),SC_valname_scalar);
	addvar(_qstr("count"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(owner);
	TSC_functor *functor=G_valuecontent<TSC_functor>(arglist->get(0));
	double min1=arglist->get(1)->G_content_scalar()->G_val();
	double max1=arglist->get(2)->G_content_scalar()->G_val();
	int count1=arglist->get(3)->G_content_scalar()->G_intval();
	curve->generate(functor,min1,max1,count1);
}
ENDFUNCTION(func_curve_generate)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_curve_generatebezier,GenerateSplineCurve)
{
	setmemberfunction(SC_valname_curve);
	addvar(_qstr("curve"),SC_valname_polybeziercurve);
	addvar(_qstr("count"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(owner);
	TPolyBezier3D *src=G_valuecontent<TPolyBezier3D>(arglist->get(0));
	int count1=arglist->get(1)->G_content_scalar()->G_intval();
	curve->generate(src,count1);
}
ENDFUNCTION(func_curve_generatebezier)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_curve_transform,transform)
{
	setmemberfunction(SC_valname_curve);
	addvar(_qstr("transf"),SC_valname_affinetransformation);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(owner);
	Taffinetransformation *transf=G_valuecontent<Taffinetransformation>(arglist->get(0));
	curve->transform(transf);
}
ENDFUNCTION(func_curve_transform)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_curve_track,Track)
{
	setmemberfunction(SC_valname_curve);
	addvar(_qstr("source"),SC_valname_subframe);
	addvar(_qstr("MinDist"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(owner);
	T3DSubFrameObject *subfr=G_valuecontent<T3DSubFrameObject>(arglist->get(0));
	double mindist=arglist->get(1)->G_content_scalar()->G_val();
	curve->set_track(subfr,mindist);
}
ENDFUNCTION(func_curve_track)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_curve_stoptrack,StopTrack)
{
	setmemberfunction(SC_valname_curve);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectCurve *curve=G_valuecontent<T3DObjectCurve>(owner);
	curve->stop_track();
}
ENDFUNCTION(func_curve_stoptrack)
