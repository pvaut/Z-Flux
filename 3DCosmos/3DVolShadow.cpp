#include"stdafx.h"
#include<math.h>

#include "opengl.h"

#include"tools.h"
#include "3DVolShadow.h"
#include "3Dscene.h"


//*********************************************************************************
// Tshadowvolume
//*********************************************************************************

void Tshadowvolume::reset()
{
	hascaps1=true;hascaps2=true;
	caps1.reset();caps2.reset();slices.reset();
	capstype=GL_TRIANGLES;
	slicestype=GL_QUADS;
}


void Tshadowvolume::render(Trendercontext *rc)
{

	rc->create_subframe(&transformation);

	glEnableClientState(GL_VERTEX_ARRAY);

	if (hascaps1)
	{
		glVertexPointer(3,GL_FLOAT,0,caps1.G_ptr());
		glDrawArrays(capstype,0,caps1.G_count());
	}

	if (hascaps2)
	{
		glVertexPointer(3,GL_FLOAT,0,caps2.G_ptr());
		glDrawArrays(capstype,0,caps2.G_count());
	}

	glVertexPointer(3,GL_FLOAT,0,slices.G_ptr());
	glDrawArrays(slicestype,0,slices.G_count());

	glDisableClientState(GL_VERTEX_ARRAY);

	rc->del_subframe();

}


void Tshadowvolume::calcsphere(double radius, int resol, Tvertex &locallightpos, double lightradius, double depth1, double depth2)
{
	reset();
	hascaps1=true;hascaps2=true;

	Tvec3d cent(0,0,0);

	double neardist,nearradius,fardist,farradius,lightdist,pointdist;
	int i;
	double csa,sna;
	Tvec3d lightpos;lightpos=locallightpos;


	lightdist=Tvec3d::distance(lightpos,cent);

	//pointdist=distance apparent start point of cone
	//positive=towards light point , negative=other side
	if (radius+lightradius==0) pointdist=1.0e9;
	else pointdist=radius/(radius+lightradius)*lightdist;

	neardist=depth1;
	nearradius=(neardist+pointdist)/pointdist*radius;

	//back cap parameters
	if (pointdist>0)//diverging cone
	{
		fardist=depth2;
		farradius=(fardist+pointdist)/pointdist*radius;
	}
	else//converging cone
	{
		fardist=depth2;
		if (fardist>-pointdist*0.99) fardist=-pointdist*0.99;
		farradius=(-pointdist-fardist)/(-pointdist)*radius;
	}

	//create directions along & perpendicular to cone
	Tvec3d tdir,v1,v2,lightdir,pt,pt1;
	lightdir.lincomb(lightpos,cent,1,-1);lightdir.norm();
	tdir.set(1,0,0);
	v1.vecprod(tdir,lightdir);
	if (v1.getsize()<0.001)
	{
		tdir.set(0,1,0);
		v1.vecprod(tdir,lightdir);
	}
	v1.norm();
	v2.vecprod(lightdir,v1);

	capstype=GL_TRIANGLE_FAN;

	//front cap
	pt.lincomb(cent,lightdir,1,-neardist);
	caps1.add(Tvec3d(pt.x,pt.y,pt.z));  
	for (i=0; i<=resol; i++)
	{
		csa=cos(i*1.0/resol*2*Pi);sna=sin(i*1.0/resol*2*Pi);
		pt.lincomb(v1,v2,csa,sna);
		pt.lincomb(cent,pt,1,nearradius);
		pt.lincomb(pt,lightdir,1,-neardist);
		caps1.add(Tvec3d(pt.x,pt.y,pt.z));
	}

	//back cap
	pt.lincomb(cent,lightdir,1,-fardist);
	caps2.add(Tvec3d(pt.x,pt.y,pt.z));  
	for (i=resol; i>=0; i--)
	{
		csa=cos(i*1.0/resol*2*Pi);sna=sin(i*1.0/resol*2*Pi);
		pt.lincomb(v1,v2,csa,sna);
		pt.lincomb(cent,pt,1,farradius);
		pt.lincomb(pt,lightdir,1,-fardist);
		caps2.add(Tvec3d(pt.x,pt.y,pt.z));
	}

	//tube
	slicestype=GL_QUAD_STRIP;
	glBegin(GL_QUAD_STRIP);
	for (i=0; i<=resol; i++)
	{
		csa=cos(i*1.0/resol*2*Pi);sna=sin(i*1.0/resol*2*Pi);
		pt.lincomb(v1,v2,csa,sna);
		pt1.lincomb(cent,pt,1,nearradius);
		pt1.lincomb(pt1,lightdir,1,-neardist);
		slices.add(Tvec3d(pt1.x,pt1.y,pt1.z));
		pt1.lincomb(cent,pt,1,farradius);
		pt1.lincomb(pt1,lightdir,1,-fardist);
		slices.add(Tvec3d(pt1.x,pt1.y,pt1.z));
	}
	glEnd();

}


//*********************************************************************************
// Tshadowvolumecontext
//*********************************************************************************

Tshadowvolumecontext::Tshadowvolumecontext()
{
}

void Tshadowvolumecontext::init(T3DScene *iscene)
{
	scene=iscene;
	for (int i=0; i<G_shadowcount(); i++)
	{
		shadowvolumes.add(new Tcopyarray<Tshadowvolume>);
	}

	while (shadowvolumes.G_count()<G_shadowcount())
		shadowvolumes.add(new Tcopyarray<Tshadowvolume>);
	for (int i=0; i<shadowvolumes.G_count(); i++) shadowvolumes.reset();
}

void Tshadowvolumecontext::adjustshadowvolumelist()
{
	while (shadowvolumes.G_count()<G_shadowcount())
		shadowvolumes.add(new Tcopyarray<Tshadowvolume>);
}


int Tshadowvolumecontext::G_shadowcount()
{
	return scene->volshadowinfo.G_count();
}

double Tshadowvolumecontext::G_lightradius(int shadownr)
{
	if ((shadownr<0)||(shadownr>=G_shadowcount())) { ASSERT(false); return 0; }
	return scene->volshadowinfo[shadownr]->lightradius;
}

double Tshadowvolumecontext::G_depth1(int shadownr)
{
	if ((shadownr<0)||(shadownr>=G_shadowcount())) { ASSERT(false); return 0; }
	return scene->volshadowinfo[shadownr]->depth1;
}

double Tshadowvolumecontext::G_depth2(int shadownr)
{
	if ((shadownr<0)||(shadownr>=G_shadowcount())) { ASSERT(false); return 0; }
	return scene->volshadowinfo[shadownr]->depth2;
}


void Tshadowvolumecontext::addshadowvolume(int shadownr, Tshadowvolume *ivol)
{
	if ((shadownr<0)||(shadownr>=G_shadowcount()))
	{
		ASSERT(false);
		return;
	}
	adjustshadowvolumelist();
	shadowvolumes[shadownr]->add(ivol);
}

void Tshadowvolumecontext::rendershadowvolumes(int shadownr, Trendercontext *rc)
{
	if ((shadownr<0)||(shadownr>=G_shadowcount()))
	{
		ASSERT(false);
		return;
	}
	adjustshadowvolumelist();
	for (int i=0; i<shadowvolumes[shadownr]->G_count(); i++)
		shadowvolumes[shadownr]->get(i)->render(rc);
}
