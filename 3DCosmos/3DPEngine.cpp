#include "stdafx.h"

#include "utils.h"

#include "vecmath.h"
#include "vec_transformation.h"

#include "opengl.h"

#include "3DScene.h"
#include "3DPEngine.h"


PEengineFrame::PEengineFrame(int icount, const TSC_time &itm)
{
	time=itm;
	positx.dim(icount);posity.dim(icount);positz.dim(icount);
	colr.dim(icount);colg.dim(icount);colb.dim(icount);
	for (int i=0; i<icount; i++)
	{
		positx[i]=0.0f;posity[i]=0.0f;positz[i]=0.0f;
		colr[i]=255;colg[i]=255;colb[i]=255;
	}
}

void PEengineFrame::check_validnr(int nr)
{
	if ((nr<0)||(nr>=positx.G_count())) throw QError(_text("Invalid frame particle number"));
}


void PEengineFrame::setposit(int nr, Tvertex &posit)
{
	check_validnr(nr);
	positx[nr]=posit.G3_x();
	posity[nr]=posit.G3_y();
	positz[nr]=posit.G3_z();
}

void PEengineFrame::setcolor(int nr, TSC_color &color)
{
	check_validnr(nr);
	colr[nr]=(unsigned char)(255*color.G_R());
	colg[nr]=(unsigned char)(255*color.G_G());
	colb[nr]=(unsigned char)(255*color.G_B());
	cola[nr]=(unsigned char)(255*color.G_A());
}


Tvertex PEengineFrame::G_posit(int nr)
{
	check_validnr(nr);
	return Tvertex(positx[nr],posity[nr],positz[nr]);
}


TSC_color PEengineFrame::G_color(int nr)
{
	check_validnr(nr);
	return TSC_color(colr[nr]/255.0,colg[nr]/255.0,colb[nr]/255.0,cola[nr]/255.0);
}


//******************************************************************
// T3DObjectPEngine
//******************************************************************

T3DObjectPEngine::T3DObjectPEngine(T3DScene *iscene) : T3DObject(iscene)
{
	pointsize=addparam(_qstr("PointSize"),SC_valname_scalar)->content.G_content_scalar();
	pointsize->copyfrom(1.0);

	color=G_valuecontent<TSC_color>(&addparam(_qstr("Color"),SC_valname_color)->content);
	addparam(_qstr("Texture"),SC_valname_string);

	rendertype=&addparam(SC_valname_penginerendertype,SC_valname_penginerendertype)->content;
	rendertype->fromint(PengineRenderQuads);

	blendtype=&addparam(SC_valname_blendtype,SC_valname_blendtype)->content;
	depthmask=&addparam(SC_valname_depthmask,SC_valname_depthmask)->content;

	blendtype->fromint(BlendTransparent);
	depthmask->fromint(DepthMaskDisable);

	sort=addparam(_qstr("Sort"),SC_valname_boolean)->content.G_content_boolean();sort->copyfrom(false);
	hasowncolors=addparam(_qstr("HasParticleColors"),SC_valname_boolean)->content.G_content_boolean();hasowncolors->copyfrom(false);

	motionname=addparam(_qstr("MotionName"),SC_valname_string)->content.G_content_string();motionname->fromstring(MOTIONNAME_DEFAULT);

}

void T3DObjectPEngine::Set_motionname(StrPtr iname)
{
	motionname->fromstring(iname);
}

T3DMotion* T3DObjectPEngine::G_motion()
{
	if (parentframe==NULL) return NULL;
	return parentframe->findmotion(motionname->G_string());
}

void T3DObjectPEngine::setsize(int icount)
{
	if (frames.G_count()>0) throw QError(_text("Unable to set particle engine size: frames are present"));
	particle_posit.reset();
	particle_speed.reset();
	particle_color.reset();
	particle_sizefactor.reset();
	for (int i=0; i<icount; i++) add();
}


int T3DObjectPEngine::add()
{
	particle_posit.add(Tvec3d(0,0,0));
	particle_speed.add(Tvector(0,0,0));
	particle_color.add(T4color(0,0,0,0));
	particle_sizefactor.add(1.0);
	return particle_posit.G_count()-1;
}

void T3DObjectPEngine::addframe(const TSC_time &itm)
{
	PEengineFrame *fr=new PEengineFrame(particle_posit.G_count(),itm);
	frames.add(fr);
}

PEengineFrame* T3DObjectPEngine::G_frame(int framenr)
{
	if ((framenr<0)||(framenr>=frames.G_count())) throw QError(_text("Invalid particle engine frame number"));
	return frames[framenr];
}

void T3DObjectPEngine::interpolframes(const TSC_time &tm)
{
	if (frames.G_count()<2) throw QError(_text("A particle engine with frames should contain at least 2 frames"));

	int fra1=0;
	int fra2=1;
	for (int i=0; i<frames.G_count()-1; i++)
		if (tm.G_JD()>=frames[i]->time.G_JD()) { fra1=i; fra2=i+1; }
	double fc2=(tm.G_JD()-frames[fra1]->time.G_JD())/(frames[fra2]->time.G_JD()-frames[fra1]->time.G_JD());
	double fc1=1.0-fc2;
	if (fc1<0) { fc1=0; fc2=1; }
	if (fc2<0) { fc1=1; fc2=0; }
	if ( (particle_posit.G_count()!=frames[fra1]->positx.G_count()) || (particle_posit.G_count()!=frames[fra2]->positx.G_count()) )
		throw QError(_text("Inconsistent point count between frames"));

	for (int i=0; i<particle_posit.G_count(); i++)
	{
		particle_posit[i].x=fc1*frames[fra1]->positx[i] + fc2*frames[fra2]->positx[i] ;
		particle_posit[i].y=fc1*frames[fra1]->posity[i] + fc2*frames[fra2]->posity[i] ;
		particle_posit[i].z=fc1*frames[fra1]->positz[i] + fc2*frames[fra2]->positz[i] ;
		particle_color[i].r=( fc1*frames[fra1]->colr[i] + fc2*frames[fra2]->colr[i] )/255.0;
		particle_color[i].g=( fc1*frames[fra1]->colg[i] + fc2*frames[fra2]->colg[i] )/255.0;
		particle_color[i].b=( fc1*frames[fra1]->colb[i] + fc2*frames[fra2]->colb[i] )/255.0;
		particle_color[i].a=( fc1*frames[fra1]->cola[i] + fc2*frames[fra2]->cola[i] )/255.0;
	}
}



void T3DObjectPEngine::particle_setposit(int nr, Tvertex &posit)
{
	if ((nr<0)||(nr>=particle_posit.G_count())) throw QError(_text("Invalid particle index"));
	particle_posit[nr].copyfrom(posit);
}

void T3DObjectPEngine::particle_setspeed(int nr, Tvector &speed)
{
	if ((nr<0)||(nr>=particle_posit.G_count())) throw QError(_text("Invalid particle index"));
	particle_speed[nr]=speed;
}


void T3DObjectPEngine::particle_setcolor(int nr, TSC_color &color)
{
	if ((nr<0)||(nr>=particle_posit.G_count())) throw QError(_text("Invalid particle index"));
	particle_color[nr].copyfrom(color);
}

void T3DObjectPEngine::particle_setsizefactor(int nr, double ival)
{
	if ((nr<0)||(nr>=particle_posit.G_count())) throw QError(_text("Invalid particle index"));
	particle_sizefactor[nr]=ival;
}

Tvertex T3DObjectPEngine::particle_G_posit(int nr)
{
	if ((nr<0)||(nr>=particle_posit.G_count())) throw QError(_text("Invalid particle index"));
	return Tvertex(particle_posit[nr].x,particle_posit[nr].y,particle_posit[nr].z,1);
}

Tvector T3DObjectPEngine::particle_G_speed(int nr)
{
	if ((nr<0)||(nr>=particle_posit.G_count())) throw QError(_text("Invalid particle index"));
	return particle_speed[nr];
}

TSC_color T3DObjectPEngine::particle_G_color(int nr)
{
	if ((nr<0)||(nr>=particle_posit.G_count())) throw QError(_text("Invalid particle index"));
	return TSC_color(particle_color[nr].r,particle_color[nr].g,particle_color[nr].b,particle_color[nr].a);
}

double T3DObjectPEngine::particle_G_sizefactor(int nr)
{
	if ((nr<0)||(nr>=particle_posit.G_count())) throw QError(_text("Invalid particle index"));
	return particle_sizefactor[nr];
}



void T3DObjectPEngine::adjusttime(double timestep)
{
	if (frames.G_count()>0)
	{
		interpolframes(*G_3DCosmos().G_time());
	}
	else
	{
		if (issame(motionname->G_string(),MOTIONNAME_DEFAULT))
		{
			for (int i=0; i<particle_posit.G_count(); i++)
			{
				particle_posit[i].x+=(GLfloat)(timestep*particle_speed[i].G_x());
				particle_posit[i].y+=(GLfloat)(timestep*particle_speed[i].G_y());
				particle_posit[i].z+=(GLfloat)(timestep*particle_speed[i].G_z());
			}
		}
		else
		{
			T3DMotion *mt=G_motion();
			TSolidObjectData obj;
			Taffinetransformation position;
			Taffinetransformation velocity;
			obj.position=&position;
			obj.velocity=&velocity;
			Tvertex orig;
			if (mt!=NULL)
			{
				for (int i=0; i<particle_posit.G_count(); i++)
				{
					orig.copyfrom(particle_posit[i]);position.Set_origin(&orig);
					velocity.Set_origin_vector(&particle_speed[i]);
					mt->apply(&obj,timestep);
					position.G_origin(&orig);particle_posit[i].copyfrom(orig);
					velocity.G_origin_vector(&particle_speed[i]);
				}
			}
		}
	}
}


void T3DObjectPEngine::precalcrender(Tprecalcrendercontext *pre)
{
	rendercolor=*color;
	if (pre->parentsubframe!=NULL)
		rendercolor.mulwith(pre->parentsubframe->G_rendercolor());
}



void T3DObjectPEngine::precalcrender2(T3DRenderViewport *rvp, Tprecalcrendercontext *pre)
{
	double px,py,pz,sz;
	int ii;
	Tvertex pst0,pst;
	TSC_color col1;
	T4color col2;

	double pointsizeval=pointsize->G_val();

	bool dosort=sort->G_val();
	bool owncolors=hasowncolors->G_val();

	if ((dosort)||(rendertype->toint()==PengineRenderQuads))
	{
		Tvertex camerapos=*rvp->viewport->G_camerapos();

		positions.reset();sortidx.reset();dists.reset();
		for (int i=0; i<particle_posit.G_count(); i++)
		{
			pst0.copyfrom(particle_posit[i]);
			pre->parentsubframe->G_rendertransformation()->mul(&pst0,&pst);
			positions.add(pst);
			sortidx.add(i);
			if (dosort) dists.add(-1*Tvertex::distance(&pst,&camerapos));
		}
		if (dosort) doublearray_sort(dists,sortidx);

		if (rendertype->toint()==PengineRenderPoints)
		{
			particle_posit_sorted.reset();
			for (int i=0; i<particle_posit.G_count(); i++)
				particle_posit_sorted.add(particle_posit[sortidx[i]]);
			if (owncolors) 
			{
				particle_color_sorted.reset();
				for (int i=0; i<particle_posit.G_count(); i++)
					particle_color_sorted.add(particle_color[sortidx[i]]);
			}
		}

	}

	if (rendertype->toint()==PengineRenderQuads)
	{
		Tvector eyedir_horiz,eyedir_vert;
		eyedir_horiz.vecprod(rvp->viewport->G_cameradir(),rvp->viewport->G_cameraupdir());eyedir_horiz.normfrom(&eyedir_horiz);
		eyedir_vert.vecprod(&eyedir_horiz,rvp->viewport->G_cameradir());eyedir_vert.normfrom(&eyedir_vert);


		Tvector dr1,dr2;
		rvp->viewport->G_transformation()->mulinv(&eyedir_horiz,&dr1);
		rvp->viewport->G_transformation()->mulinv(&eyedir_vert,&dr2);
		double d1x=dr1.G_x()/2;
		double d1y=dr1.G_y()/2;
		double d1z=dr1.G_z()/2;
		double d2x=dr2.G_x()/2;
		double d2y=dr2.G_y()/2;
		double d2z=dr2.G_z()/2;

		vertexbuffer.reset();colorbuffer.reset();texturebuffer.reset();
		for (int i=0; i<particle_posit.G_count(); i++)
		{
			ii=sortidx[i];
			px=positions[ii].G3_x();
			py=positions[ii].G3_y();
			pz=positions[ii].G3_z();
			sz=pointsizeval*particle_sizefactor[ii];
			vertexbuffer.add(Tvec3d( px-sz*d1x-sz*d2x , py-sz*d1y-sz*d2y , pz-sz*d1z-sz*d2z ));
			vertexbuffer.add(Tvec3d( px+sz*d1x-sz*d2x , py+sz*d1y-sz*d2y , pz+sz*d1z-sz*d2z ));
			vertexbuffer.add(Tvec3d( px+sz*d1x+sz*d2x , py+sz*d1y+sz*d2y , pz+sz*d1z+sz*d2z ));
			vertexbuffer.add(Tvec3d( px-sz*d1x+sz*d2x , py-sz*d1y+sz*d2y , pz-sz*d1z+sz*d2z ));
			texturebuffer.add(T2textureidx(0,0));
			texturebuffer.add(T2textureidx(0,1));
			texturebuffer.add(T2textureidx(1,1));
			texturebuffer.add(T2textureidx(1,0));
			if (owncolors)
			{
				col1.copyfrom(particle_color[i].r,particle_color[i].g,particle_color[i].b,particle_color[i].a);
				col1.mulwith(&rendercolor);
				col2.r=(V3DFLOAT)col1.G_R();
				col2.g=(V3DFLOAT)col1.G_G();
				col2.b=(V3DFLOAT)col1.G_B();
				col2.a=(V3DFLOAT)col1.G_A();
				colorbuffer.add(col2);colorbuffer.add(col2);colorbuffer.add(col2);colorbuffer.add(col2);
			}
		}
	}
}


void T3DObjectPEngine::render(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording)
{
	if ((rc->currentrenderpass==Trendercontext::RP_SOLID)&&(blendtype->toint()!=BlendNormal)) return;
	if ((rc->currentrenderpass==Trendercontext::RP_TRANSPARENT)&&(blendtype->toint()==BlendNormal)) return;
	ASSERT(parentframe!=NULL);

	rc->enablelight(false);
	rc->EnableDepthMask(false);
	rc->SetBlendType(blendtype->toint());
	rc->EnableDepthMask(depthmask->toint()!=DepthMaskDisable);

	T3DTexture *texture=NULL;
	StrPtr texturename=G_texturename();
	if (qstrlen(texturename)>0)
	{
		texture=parentframe->G_texture(texturename);
		if (texture==NULL)
		{
			QString errstr;FormatString(errstr,_text("Could not find texture '^1'"),texturename);
			throw QError(errstr);
		}
		texture->select(rc);
	}

	rc->set_color(rendercolor.G_R(),rendercolor.G_G(),rendercolor.G_B(),rendercolor.G_A());
	rc->set_orientation(+1);


	if (rendertype->toint()==PengineRenderQuads) render_quads(rc,status,isrecording);
	if (rendertype->toint()==PengineRenderPoints) render_points(rc,status,isrecording);

	if (texture!=NULL) texture->unselect(rc);
}


void T3DObjectPEngine::render_points(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording)
{
	bool dosort=sort->G_val();

	if (qstrlen(G_texturename())>0)
	{
		glDisable(GL_POINT_SMOOTH);
		glEnable(GL_POINT_SPRITE);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	}
	else
	{
		glEnable(GL_POINT_SMOOTH);
		glDisable(GL_POINT_SPRITE);
	}

	glEnableClientState(GL_VERTEX_ARRAY);

	if (!dosort)
	{
		glVertexPointer(3,GL_FLOAT,0,particle_posit.G_ptr());
		if (G_hasowncolors())
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4,GL_FLOAT,0,particle_color.G_ptr());
		}
	}
	else
	{
		glVertexPointer(3,GL_FLOAT,0,particle_posit_sorted.G_ptr());
		if (G_hasowncolors())
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4,GL_FLOAT,0,particle_color_sorted.G_ptr());
		}
	}



	glPointSize((GLfloat)pointsize->G_val());

	glDrawArrays(GL_POINTS,0,particle_posit.G_count());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

}


void T3DObjectPEngine::render_quads(Trendercontext *rc, const TObjectRenderStatus *status, bool isrecording)
{
	rc->create_subframe_orig();

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,vertexbuffer.G_ptr());

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,texturebuffer.G_ptr());

	if (G_hasowncolors())
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_FLOAT,0,colorbuffer.G_ptr());
	}

	glDrawArrays(GL_QUADS,0,vertexbuffer.G_count());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	rc->del_subframe();
}



void T3DObjectPEngine::streamout_content(QBinTagWriter &writer)
{
	{
		QBinTagWriter &dtag=writer.subtag(_qstr("Data")).G_obj();
		particle_posit.streamout(writer);
		particle_speed.streamout(writer);
		particle_color.streamout(writer);
		particle_sizefactor.streamout(writer);
		positions.streamout(writer);
		vertexbuffer.streamout(writer);
		colorbuffer.streamout(writer);
		texturebuffer.streamout(writer);
		dtag.close();
	}

	{
		QBinTagWriter &ftag=writer.subtag(_qstr("Frames")).G_obj();
		ftag.write_int32(frames.G_count());
		for (int framenr=0; framenr<frames.G_count(); framenr++)
		{
			frames[framenr]->time.streamout(writer);
			frames[framenr]->positx.streamout(writer);
			frames[framenr]->posity.streamout(writer);
			frames[framenr]->positz.streamout(writer);
			frames[framenr]->colr.streamout(writer);
			frames[framenr]->colg.streamout(writer);
			frames[framenr]->colb.streamout(writer);
			frames[framenr]->cola.streamout(writer);
		}
		ftag.close();
	}

}

void T3DObjectPEngine::streamin_content(QBinTagReader &reader)
{
	while (reader.hassubtags())
	{
		QBinTagReader &tg1=reader.getnextsubtag().G_obj();
		if (tg1.Istag(_qstr("Data")))
		{
			particle_posit.streamin(reader);
			particle_speed.streamin(reader);
			particle_color.streamin(reader);
			particle_sizefactor.streamin(reader);
			positions.streamin(reader);
			vertexbuffer.streamin(reader);
			colorbuffer.streamin(reader);
			texturebuffer.streamin(reader);
		}
		if (tg1.Istag(_qstr("Frames")))
		{
			int fcount=tg1.read_int32();
			for (int framenr=0; framenr<fcount; framenr++)
			{
				TSC_time tm;tm.streamin(reader);
				PEengineFrame *frame=new PEengineFrame(particle_posit.G_count(),tm);frames.add(frame);
				frame->positx.streamin(reader);
				frame->posity.streamin(reader);
				frame->positz.streamin(reader);
				frame->colr.streamin(reader);
				frame->colg.streamin(reader);
				frame->colb.streamin(reader);
				frame->cola.streamin(reader);
			}
		}
		tg1.close();
	}
}


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_addpengine,addpengine)
{
	setreturntype(SC_valname_pengine);
	setmemberfunction(SC_valname_subframe);
	extendclasspath(SC_subpart_createshapes);
	addvar(_qstr("name"),SC_valname_string,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DSubFrameObject *subframe0=G_valuecontent<T3DSubFrameObject>(owner);
	T3DObjectPEngine *newobject=new T3DObjectPEngine(subframe0->G_scene());

	if (arglist->G_ispresent(0))
	{
		newobject->Set_name(arglist->get(0)->G_content_string()->G_string());
	}

	subframe0->addsubobject(newobject);
	retval->settype(newobject->G_datatype());
	retval->encapsulate(newobject);
}
ENDFUNCTION(func_object_addpengine)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_pengine_motion,motion)
{
	setreturntype(SC_valname_any);
	setmemberfunction(SC_valname_pengine);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectPEngine *pe=G_valuecontent<T3DObjectPEngine>(owner);
	if (assigntoval!=NULL)
	{
		QString motionname;
		GetObjectName(assigntoval,motionname);
		pe->Set_motionname(motionname);
	}
	T3DMotion *mt=pe->G_motion();
	if (mt==NULL) throw QError(_text("Invalid or absent motion"));
	retval->settype(mt->G_datatype());
	retval->encapsulate(mt);
}
ENDFUNCTION(func_object_pengine_motion)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_pengine_setsize,SetSize)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_pengine);
	addvar(_qstr("ParticleCount"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectPEngine *pe=G_valuecontent<T3DObjectPEngine>(owner);

	pe->setsize(arglist->get(0)->G_content_scalar()->G_val());
}
ENDFUNCTION(func_object_pengine_setsize)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_pengine_add,add)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_pengine);
	addvar(_qstr("position"),SC_valname_vertex,false);
	addvar(_qstr("sizefactor"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectPEngine *pe=G_valuecontent<T3DObjectPEngine>(owner);

	int pnr=pe->add();
	if (arglist->G_ispresent(0)) pe->particle_setposit(pnr,*G_valuecontent<Tvertex>(arglist->get(0)));
	if (arglist->G_ispresent(1)) pe->particle_setsizefactor(pnr,arglist->get(1)->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(pnr);
}
ENDFUNCTION(func_object_pengine_add)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_pengine_position,Pposition)
{
	setreturntype(SC_valname_vertex);
	setcanassign();
	setmemberfunction(SC_valname_pengine);
	addvar(_qstr("Index"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectPEngine *pe=G_valuecontent<T3DObjectPEngine>(owner);
	int nr=arglist->get(0)->G_content_scalar()->G_intval();

	if (assigntoval!=NULL)
		pe->particle_setposit(nr,*G_valuecontent<Tvertex>(assigntoval));

	*G_valuecontent<Tvertex>(retval)=pe->particle_G_posit(nr);
}
ENDFUNCTION(func_object_pengine_position)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_pengine_positionvec,SetPpositionVec)
{
	setmemberfunction(SC_valname_pengine);
	addvar(_qstr("PointList"),SC_valname_list);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectPEngine *pe=G_valuecontent<T3DObjectPEngine>(owner);
	TSC_list *points=G_valuecontent<TSC_list>(arglist->get(0));
	for (int i=0; i<points->G_size(); i++)
		pe->particle_setposit(i,*G_valuecontent<Tvertex>(points->get(i)));
}
ENDFUNCTION(func_object_pengine_positionvec)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_pengine_velocity,Pvelocity)
{
	setreturntype(SC_valname_vector);
	setcanassign();
	setmemberfunction(SC_valname_pengine);
	addvar(_qstr("Index"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectPEngine *pe=G_valuecontent<T3DObjectPEngine>(owner);
	int nr=arglist->get(0)->G_content_scalar()->G_intval();

	if (assigntoval!=NULL)
		pe->particle_setspeed(nr,*G_valuecontent<Tvector>(assigntoval));

	*G_valuecontent<Tvector>(retval)=pe->particle_G_speed(nr);
}
ENDFUNCTION(func_object_pengine_velocity)




///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_pengine_color,Pcolor)
{
	setreturntype(SC_valname_color);
	setcanassign();
	setmemberfunction(SC_valname_pengine);
	addvar(_qstr("Index"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectPEngine *pe=G_valuecontent<T3DObjectPEngine>(owner);
	if (!pe->G_hasowncolors()) throw(QError(_text("Particle engine does not have particle colors")));
	int nr=arglist->get(0)->G_content_scalar()->G_intval();

	if (assigntoval!=NULL)
	{
		pe->particle_setcolor(nr,*G_valuecontent<TSC_color>(assigntoval));
	}

	*G_valuecontent<TSC_color>(retval)=pe->particle_G_color(nr);
}
ENDFUNCTION(func_object_pengine_color)





///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_pengine_size,Psizefactor)
{
	setreturntype(SC_valname_scalar);
	setcanassign();
	setmemberfunction(SC_valname_pengine);
	addvar(_qstr("Index"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectPEngine *pe=G_valuecontent<T3DObjectPEngine>(owner);
	int nr=arglist->get(0)->G_content_scalar()->G_intval();

	if (assigntoval!=NULL)
		pe->particle_setsizefactor(nr,assigntoval->G_content_scalar()->G_val());

	retval->G_content_scalar()->copyfrom(pe->particle_G_sizefactor(nr));
}
ENDFUNCTION(func_object_pengine_size)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_pengine_addframe,AddFrame)
{
	setmemberfunction(SC_valname_pengine);
	addvar(_qstr("Time"),SC_valname_time);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectPEngine *pe=G_valuecontent<T3DObjectPEngine>(owner);
	TSC_time *tm=G_valuecontent<TSC_time>(arglist->get(0));
	pe->addframe(*tm);
}
ENDFUNCTION(func_object_pengine_addframe)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_pengine_framepoint_position,FramePtPosition)
{
	setreturntype(SC_valname_vertex);
	setcanassign();
	setmemberfunction(SC_valname_pengine);
	addvar(_qstr("Framenr"),SC_valname_scalar);
	addvar(_qstr("Index"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectPEngine *pe=G_valuecontent<T3DObjectPEngine>(owner);
	int framenr=arglist->get(0)->G_content_scalar()->G_intval();
	int ptnr=arglist->get(1)->G_content_scalar()->G_intval();

	if (assigntoval!=NULL)
		pe->G_frame(framenr)->setposit(ptnr,*G_valuecontent<Tvertex>(assigntoval));

	*G_valuecontent<Tvertex>(retval)=pe->G_frame(framenr)->G_posit(ptnr);
}
ENDFUNCTION(func_object_pengine_framepoint_position)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_object_pengine_framepoint_color,FramePtColor)
{
	setreturntype(SC_valname_color);
	setcanassign();
	setmemberfunction(SC_valname_pengine);
	addvar(_qstr("Framenr"),SC_valname_scalar);
	addvar(_qstr("Index"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	T3DObjectPEngine *pe=G_valuecontent<T3DObjectPEngine>(owner);
	int framenr=arglist->get(0)->G_content_scalar()->G_intval();
	int ptnr=arglist->get(1)->G_content_scalar()->G_intval();

	if (assigntoval!=NULL)
		pe->G_frame(framenr)->setcolor(ptnr,*G_valuecontent<TSC_color>(assigntoval));

	*G_valuecontent<TSC_color>(retval)=pe->G_frame(framenr)->G_color(ptnr);
}
ENDFUNCTION(func_object_pengine_framepoint_color)
