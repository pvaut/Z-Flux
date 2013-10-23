#include "stdafx.h"
#include <process.h>

#include "opengl.h"

#include "qerror.h"

#include "qxstoredbitmap.h"

#include "3DScene.h"
#include "3DFont.h"
#include "renderwindow.h"
#include "displaydevices.h"
#include "shader.h"




int GetAxisLevelKBModifier();

int G_rendertype();
StrPtr G_texturesdir();
StrPtr G_datadir();

#define fulltesting true








//**********************************************************************
// Trendertexture
//**********************************************************************

Trendertexture::Trendertexture()
{
	data=NULL;
	loaded=false;
	textureid=-1;
	hasalpha=false;
}

Trendertexture::~Trendertexture()
{
	clear();
}

void Trendertexture::clear()
{
	hasalpha=false;
	if (loaded)
	{
		glDeleteTextures(1,&textureid);
		if (data!=NULL) free(data);data=NULL;
		loaded=false;
	}
}



void Trendertexture::loadbitmapfile(StrPtr ifilename, StrPtr ibitmapalphafile)
{
	//load bitmap
	Tbmp bmp,bmpalpha;
	hasalpha=false;
	if (!bmp.loadgdiplus(ifilename))
	{
		QString err=_text("Unable to load texture ");err+=ifilename;
		throw QError(err);
	}
	if ((ibitmapalphafile!=NULL)&&(qstrlen(ibitmapalphafile)>0))
	{
		if (!bmpalpha.loadgdiplus(ibitmapalphafile))
		{
			QString err=_text("Unable to load texture alpha file ");err+=ibitmapalphafile;
			throw QError(err);
		}
		if ((bmpalpha.G_xres()!=bmp.G_xres())||(bmpalpha.G_yres()!=bmp.G_yres()))
			throw QError(_text("Incompatible resolution between texture and alpha map"));
		hasalpha=true;
	}

	loadbitmap(&bmp,hasalpha ? &bmpalpha : NULL);
}

void Trendertexture::loadbitmap(Tbmp *bmp, Tbmp *bmpalpha)
{
	clear();

	hasalpha=(bmpalpha!=NULL);

	//convert to OpenGL bitmap data
	unsigned char *data=(unsigned char*)malloc(bmp->G_xres()*bmp->G_yres()*4);
	unsigned char *valr=bmp->G_val8rptr();
	unsigned char *valg=bmp->G_val8gptr();
	unsigned char *valb=bmp->G_val8bptr();
	if (valr==NULL) valr=bmp->G_val8ptr();
	if (valg==NULL) valg=bmp->G_val8ptr();
	if (valb==NULL) valb=bmp->G_val8ptr();
	if ((valr==NULL)||(valg==NULL)||(valb==NULL)) throw QError(_text("Invalid bitmap file"));
	unsigned char *vala=NULL; if (bmpalpha!=NULL) vala=bmpalpha->G_val8rptr();
	int idx1,idx2;
	for (int iy=0; iy<bmp->G_yres(); iy++)
	{
		idx1=(iy*bmp->G_xres())*4;
		idx2=(bmp->G_yres()-1-iy)*bmp->G_xres();
		for (int ix=0; ix<bmp->G_xres(); ix++)
		{
			data[idx1+0]=valr[idx2];
			data[idx1+1]=valg[idx2];
			data[idx1+2]=valb[idx2];
			data[idx1+3]=255;
			if (bmpalpha!=NULL) data[idx1+3]=vala[idx2];
			idx1+=4;
			idx2++;
		}
	}

	xres=bmp->G_xres();
	yres=bmp->G_yres();
	loaded=true;

	glGenTextures(1,&textureid);
	glBindTexture(GL_TEXTURE_2D,textureid);

	//attach bitmap to texture
	if (!hasalpha)
		glTexImage2D(GL_TEXTURE_2D, 0, 3, bmp->G_xres(),bmp->G_yres(), 0,GL_RGBA,GL_UNSIGNED_BYTE,data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp->G_xres(),bmp->G_yres(), 0,GL_RGBA,GL_UNSIGNED_BYTE,data);

	free(data);data=NULL;


}

void Trendertexture::select(Trendercontext *rc)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,G_textureid());
    ::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    ::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
}

void Trendertexture::unselect(Trendercontext *rc)
{
	glDisable(GL_TEXTURE_2D);
}



///////////////////////////////////////////////////////////////////////
// Trendercontext
///////////////////////////////////////////////////////////////////////

Trendercontext::Trendercontext(TRenderWindow* irw)
{
	rw=irw;
	clipplaneusedcount=0;
	lightenabled=true;
	isrightpart=false;
//	colortransform=NULL;
}

Trendercontext::~Trendercontext()
{
//	delete colortransform;
	displaylists.reset();
	lightpoint1.clear();

}

void Trendercontext::setup()
{
	addlog(_text("Start setup of RenderContect"),+1);
	QString filename;
//	filename=G_texturesdir();filename+=_qstr("\\lightpoint1.bmp");
	filename=G_texturesdir();filename+=_qstr("\\star1.bmp");
	lightpoint1.loadbitmapfile(filename);


/*	...moet waarschijnlijk weer weg...
	addlog(_text("Creating colortransform shader"),+1);
	checkerror(_text("start shader"));
	colortransform=new TShader;
	checkerror(_text("shader created"));
	colortransform->loadsource_vertex(TFormatString(_qstr("^1\\shaders\\vertex0.txt"),G_datadir()));
	checkerror(_text("vertexer loaded"));
	colortransform->loadsource_fragment(TFormatString(_qstr("^1\\shaders\\frag0.txt"),G_datadir()));
	checkerror(_text("fragmenter loaded"));
	colortransform->compile();
	checkerror(_text("shader compiled"));
	colortransform->use();
	rendercontext.checkerror(_text("shader in use"));
	addlog(_text("Colortransform shader created"),+1);*/

	addlog(_text("Completed setup of RenderContect"),-1);

}

void Trendercontext::checkerror(StrPtr comment)
{
	GLenum err=glGetError();
	if (err==GL_NO_ERROR) return;

	QString errorstr=_text("OPenGL error ");
	errorstr+=comment;
	errorstr+=_text(" ");
	if (err==GL_INVALID_ENUM) errorstr+=_text("GL_INVALID_ENUM");
	if (err==GL_INVALID_VALUE) errorstr+=_text("GL_INVALID_VALUE");
	if (err==GL_INVALID_OPERATION) errorstr+=_text("GL_INVALID_OPERATION");
	if (err==GL_STACK_OVERFLOW) errorstr+=_text("GL_STACK_OVERFLOW");
	if (err==GL_STACK_UNDERFLOW) errorstr+=_text("GL_STACK_UNDERFLOW");
	if (err==GL_OUT_OF_MEMORY) errorstr+=_text("GL_OUT_OF_MEMORY");
	ASSERT(false);
	throw QError(errorstr);
}


void Trendercontext::initrender(RenderPass irenderpass)
{
	checkerror(_text("Start initrender"));


/*	if (colortransform!=NULL)
	{
		colortransform->use();
		checkerror(_text("Colortransform activated"));
	}*/

	currentrenderpass=irenderpass;
	lightpoints_worldposit.reset();
	lightpoints_color.reset();
	lightpoints_size.reset();

	camerapos=*rw->viewport->G_camerapos();
	eyedir_horiz.vecprod(rw->viewport->G_cameradir(),rw->viewport->G_cameraupdir());eyedir_horiz.normfrom(&eyedir_horiz);
	eyedir_vert.vecprod(&eyedir_horiz,rw->viewport->G_cameradir());eyedir_vert.normfrom(&eyedir_vert);
	eyedir_depth=*rw->viewport->G_cameradir();eyedir_depth.normfrom(&eyedir_depth);

	clipplaneusedcount=0;
	clipplaneidentifiers.reset();
	clipplaneidentifiers.add(GL_CLIP_PLANE0);clipplaneidentifiers.add(GL_CLIP_PLANE1);
	clipplaneidentifiers.add(GL_CLIP_PLANE2);clipplaneidentifiers.add(GL_CLIP_PLANE3);
	clipplaneidentifiers.add(GL_CLIP_PLANE4);clipplaneidentifiers.add(GL_CLIP_PLANE5);
	for (int clnr=0; clnr<maxclipplanecount; clnr++)
		glDisable(clipplaneidentifiers[clnr]);

/*	if (!G_isrightwindow())
	{
		glMatrixMode(GL_COLOR);
		GLfloat rgbconversion[16] = 
		{
				1.2f, 0.0f, 0.0f, 0.0f,
				0.2f, 1.0f, 0.0f, 0.0f,
				0.2f, 0.0f, 0.8f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
		};
		glLoadMatrixf(rgbconversion);
		glMatrixMode(GL_MODELVIEW);
	}
	else
	{
		glMatrixMode(GL_COLOR);
		GLfloat rgbconversion[16] = 
		{
				0.8f, 0.0f, 0.2f, 0.0f,
				0.0f, 1.0f, 0.2f, 0.0f,
				0.0f, 0.0f, 1.2f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
		};
		glLoadMatrixf(rgbconversion);
		glMatrixMode(GL_MODELVIEW);
	}*/


	checkerror(_text("End initrender"));

}

void Trendercontext::activate()
{
	rw->GetRC();
	checkerror(_text("End activate"));
}
void Trendercontext::deactivate()
{
	rw->ReleaseRC();
	checkerror(_text("End deactivate"));
}

bool Trendercontext::G_isrightwindow()
{ 
	if (rw==NULL)
	{
		ASSERT(false);
		return false;
	}
	return rw->isrightpart;
}


void Trendercontext::SetBlendType(int blendtype)
{
	if (blendtype==BlendNormal)
	{
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
		glDepthMask(GL_TRUE);
		return;
	}
	if (blendtype==BlendTransparent)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
		glDepthMask(GL_FALSE);
		return;
	}
	if (blendtype==BlendTranslucent)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_TRUE);
		return;
	}
	ASSERT(false);
}


void Trendercontext::EnableDepthTest(bool enabled)
{
//	if (enabled==cachedsettings.enabedepthtest) return;
//	if (enabled) glEnable(GL_DEPTH_TEST);
//	else glDisable(GL_DEPTH_TEST);
	//--!!--modified 2009-04-11 into: (because previous version did not update the depth buffer)
	if (enabled) glDepthFunc(GL_LESS);
	else glDepthFunc(GL_ALWAYS);
}


void Trendercontext::EnableDepthMask(bool enabled)
{
//	if (enabled==cachedsettings.enabedepthmask) return;
	glDepthMask(enabled);
}

void Trendercontext::EnableRescaleNormal(bool enabled)
{
	if (enabled) glEnable(GL_NORMALIZE);
	else glDisable(GL_NORMALIZE);
}

T3DViewport* Trendercontext::G_viewport()
{ 
	return rw->viewport; 
}



void Trendercontext::setnearfarplane(double nearplane, double farplane)
{
	rw->createfrustum(nearplane,farplane);
}

void Trendercontext::setdefaultnearfarplane()
{
	setnearfarplane(rw->viewport->G_nearclipplane(),rw->viewport->G_farclipplane());
}


void Trendercontext::enablelight(bool enabled)
{
	if (enabled) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);
	lightenabled=enabled;
}


void Trendercontext::set_light0(Tvertex *posit, TSC_color *ambientcolor, TSC_color *diffusecolor, TSC_color *specularcolor)
{
	double fr=1,fg=1,fb=1;

/*	if (!isrightpart)
	{
		fr=1.5;
		fb=0.5;
	}
	else
	{
		fr=0.5;
		fb=1.5;
	}*/
	GLfloat lipos0[]= {(GLfloat)posit->G3_x(),(GLfloat)posit->G3_y(),(GLfloat)posit->G3_z(),(GLfloat)1.0};
	glLightfv(GL_LIGHT0,GL_POSITION,lipos0);

	GLfloat licol0[]= {(GLfloat)(fr*diffusecolor->G_R()),(fg*(GLfloat)diffusecolor->G_G()),(GLfloat)(fb*diffusecolor->G_B()),(GLfloat)diffusecolor->G_A()};
	glLightfv(GL_LIGHT0,GL_DIFFUSE,licol0);

	GLfloat alicol0[]= {(GLfloat)(fr*ambientcolor->G_R()),(fg*(GLfloat)ambientcolor->G_G()),(GLfloat)(fb*ambientcolor->G_B()),(GLfloat)ambientcolor->G_A()};
	glLightfv(GL_LIGHT0,GL_AMBIENT,alicol0);

	GLfloat slicol0[]= {(GLfloat)(fr*specularcolor->G_R()),(fg*(GLfloat)specularcolor->G_G()),(GLfloat)(fb*specularcolor->G_B()),(GLfloat)specularcolor->G_A()};
	glLightfv(GL_LIGHT0,GL_SPECULAR,slicol0);

}

void MapColor(int tpe, GLfloat iR, GLfloat iG, GLfloat iB, GLfloat &oR, GLfloat &oG, GLfloat &oB)
{
//todo: use T3DCosmos::colormatrixleft & right
	TMatrix *cmat=T3DCosmos::Get().colormatrixleft;
	if (tpe==1) cmat=T3DCosmos::Get().colormatrixright;

	oR=cmat->Get(0,0)*iR+cmat->Get(0,1)*iG+cmat->Get(0,2)*iB;
	oG=cmat->Get(1,0)*iR+cmat->Get(1,1)*iG+cmat->Get(1,2)*iB;
	oB=cmat->Get(2,0)*iR+cmat->Get(2,1)*iG+cmat->Get(2,2)*iB;
}


void T4color::ColMapL()
{
	double r2=r;
	double g2=g;
	double b2=b;
	MapColor(0,r2,g2,b2,r,g,b);
}

void T4color::ColMapR()
{
	double r2=r;
	double g2=g;
	double b2=b;
	MapColor(1,r2,g2,b2,r,g,b);
}


void Trendercontext::set_color(double R, double G, double B, double A)
{
	GLfloat R2=R;
	GLfloat G2=G;
	GLfloat B2=B;

	if (!isrightpart) MapColor(0,R,G,B,R2,G2,B2);
	else MapColor(1,R,G,B,R2,G2,B2);
	glColor4f(R2,G2,B2,A);
}

void Trendercontext::set_speccolor(TSC_color *color, double specval)
{
	GLfloat mat_speccol[]={(float)color->G_R(),(float)color->G_G(),(float)color->G_B(),(float)color->G_A()};
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_speccol);
	float shinival=specval;
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&shinival);
}


void Trendercontext::set_orientation(int orient)
{
	ASSERT((orient==+1)||(orient==-1));
//	if ((orient>0)&&(!Hmirrorred)) glFrontFace(GL_CCW);
//	else glFrontFace(GL_CW);

	bool ori=true;
	if (Hmirrorred) ori=!ori;
	if (Vmirrorred) ori=!ori;
	if (orient<0) ori=!ori;
	if (ori) glFrontFace(GL_CCW);
	else glFrontFace(GL_CW);
}

void Trendercontext::create_subframe_fulltransformation(Taffinetransformation *transf)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixd(rw->viewmatrix);
	transf->streamout(temp_mat44);
	glMultMatrixd(temp_mat44);
	EnableRescaleNormal(transf->G_isrescaling());
	if (fulltesting) checkerror(_text("Create subframe fulltransformation"));
}


void Trendercontext::create_subframe(Taffinetransformation *transf)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	transf->streamout(temp_mat44);
	glMultMatrixd(temp_mat44);
	if (fulltesting) checkerror(_text("Create subframe"));
}

void Trendercontext::create_subframe_translated(Tvector &shift)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslated(shift.G_x(),shift.G_y(),shift.G_z());
	if (fulltesting) checkerror(_text("Create subframe translated"));
}


void Trendercontext::create_subframe_viewdir(Tvertex *posit)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslated(posit->G3_x(),posit->G3_y(),posit->G3_z());

	double modelview[16];
	int i,j;
	glGetDoublev(GL_MODELVIEW_MATRIX,modelview);

	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
			if (i==j) modelview[i*4+j]=1.0;
			else modelview[i*4+j]=0.0;
	glLoadMatrixd(modelview);
	if (fulltesting) checkerror(_text("Create subframe viewdir"));
}

bool Trendercontext::G_isHmirrored()
{
	return Hmirrorred;
}

bool Trendercontext::G_isVmirrored()
{
	return Vmirrorred;
}


void Trendercontext::create_subframe_screen()
{
	double nearclip=rw->viewport->G_nearclipplane();
	double farclip=rw->viewport->G_farclipplane();
	double focaldistance=rw->viewport->G_focaldistance();

	double eyesep=rw->viewport->G_eyeseparation();
	int fact=-1;
	if (rw->isrightpart) fact=+1;


	double wd2=focaldistance*tan(rw->viewport->G_aperture()/2.0);
	double left   = abs(rw->screenratio)*wd2 + fact*0.5*eyesep;
	double bottom =  wd2;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTranslated(-left,-bottom,-focaldistance);
	glScaled(2*wd2,2*wd2,2*wd2);
	if (fulltesting) checkerror(_text("Create subframe screen"));
}

void Trendercontext::create_subframe_viewport()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double xl=fabs(rw->screenratio);
	if (!G_isHmirrored()) glOrtho(0,xl,0,1,-1,2);
	else glOrtho(xl,0,0,1,-1,2);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(0,0,1,
			0,0,0,
			0,1,0);

	//disable point attenuation --!!--todo: re-enable after this subframe was stopped!
	GLfloat atten[] =  { 1.0f, 0.0f, 0.0f };
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION,atten);

}

void Trendercontext::create_subframe_orig()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixd(rw->viewmatrix);
	if (fulltesting) checkerror(_text("Create subframe orig"));
}

void Trendercontext::start_viewportframe(bool preserveaspectratio)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double xl=1.0;
	if (preserveaspectratio) xl=fabs(rw->screenratio);
	if (!G_isHmirrored()) glOrtho(0,xl,0,1,-1,2);
	else glOrtho(xl,0,0,1,-1,2);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(0,0,1,
			0,0,0,
			0,1,0);
	if (fulltesting) checkerror(_text("Create viewportframe"));
}


void Trendercontext::del_subframe()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	if (fulltesting) checkerror(_text("Delete subframe"));
}


bool Trendercontext::G_istextureactive()
{
	GLboolean textureenabled;
	glGetBooleanv(GL_TEXTURE_2D,&textureenabled);
	return textureenabled;
}


void Trendercontext::addclipplane(const Tplane &plane)
{
	if (clipplaneusedcount>=maxclipplanecount) throw(QError(_text("Maximum number of clip planes reached")));
	glEnable(clipplaneidentifiers[clipplaneusedcount]);
	GLdouble equat[4];
	equat[0]=plane.G_x();
	equat[1]=plane.G_y();
	equat[2]=plane.G_z();
	equat[3]=plane.G_u();
	glClipPlane(clipplaneidentifiers[clipplaneusedcount],equat);
	clipplaneusedcount++;

}

void Trendercontext::delclipplane()
{
	if (clipplaneusedcount<=0) throw(QError(_text("No clip planes are currently defined")));
	clipplaneusedcount--;
	glDisable(clipplaneidentifiers[clipplaneusedcount]);
}


void Trendercontext::addlightpoint(Tvertex *worldposit, TSC_color *color, double size)
{
	lightpoints_worldposit.add(*worldposit);
	lightpoints_color.add(*color);
	lightpoints_size.add(size);
}

void Trendercontext::renderlightpoints()
{
	enablelight(false);
	EnableDepthMask(false);
	SetBlendType(BlendTransparent);

	create_subframe_orig();
	lightpoint1.select(this);

	Tvector dr1,dr2;
	rw->viewport->G_transformation()->mulinv(&eyedir_horiz,&dr1);
	rw->viewport->G_transformation()->mulinv(&eyedir_vert,&dr2);
	double d1x=dr1.G_x()/2;
	double d1y=dr1.G_y()/2;
	double d1z=dr1.G_z()/2;
	double d2x=dr2.G_x()/2;
	double d2y=dr2.G_y()/2;
	double d2z=dr2.G_z()/2;

	double px,py,pz,fc;
	for (int i=0; i<lightpoints_worldposit.G_count(); i++)
	{
		px=lightpoints_worldposit[i].G3_x();
		py=lightpoints_worldposit[i].G3_y();
		pz=lightpoints_worldposit[i].G3_z();
		fc=lightpoints_size[i];
		glBegin(GL_QUADS);
		set_color(lightpoints_color[i].G_R(),lightpoints_color[i].G_G(),lightpoints_color[i].G_B(),lightpoints_color[i].G_A());
		glTexCoord2d(0,0);
		glVertex3d( px-d1x*fc-d2x*fc, py-d1y*fc-d2y*fc, pz-d1z*fc-d2z*fc );
		glTexCoord2d(1,0);
		glVertex3d( px+d1x*fc-d2x*fc, py+d1y*fc-d2y*fc, pz+d1z*fc-d2z*fc );
		glTexCoord2d(1,1);
		glVertex3d( px+d1x*fc+d2x*fc, py+d1y*fc+d2y*fc, pz+d1z*fc+d2z*fc );
		glTexCoord2d(0,1);
		glVertex3d( px-d1x*fc+d2x*fc, py-d1y*fc+d2y*fc, pz-d1z*fc+d2z*fc );
		glEnd();
	}
	lightpoint1.unselect(this);

	del_subframe();
}


void Trendercontext::rendertext(StrPtr fontname, Tvertex *posit, Tvector *dir1, Tvector *dir2, StrPtr txt, double depth)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	tempvecz.vecprod(dir1,dir2);tempvecz.normfrom(&tempvecz);
	temp_mat44[0]=dir1->G_x(); temp_mat44[4]=dir2->G_x(); temp_mat44[ 8]=tempvecz.G_x(); temp_mat44[12]=posit->G3_x();
	temp_mat44[1]=dir1->G_y(); temp_mat44[5]=dir2->G_y(); temp_mat44[ 9]=tempvecz.G_y(); temp_mat44[13]=posit->G3_y();
	temp_mat44[2]=dir1->G_z(); temp_mat44[6]=dir2->G_z(); temp_mat44[10]=tempvecz.G_z(); temp_mat44[14]=posit->G3_z();
	temp_mat44[3]=0;           temp_mat44[7]=0;           temp_mat44[11]=0;              temp_mat44[15]=1;
	glMultMatrixd(temp_mat44);
	glScaled(1.0,1.0,depth);
	glEnable(GL_CULL_FACE);

//	glListBase(rw->fontid1);
	int offset=rw->G_font(fontname)->G_ID();
	int len=qstrlen(txt);
	for (int i=0; i<len; i++)
	{
		unsigned char bt=(unsigned char)txt[i];
		glCallList(offset+bt);
	}
	glPopMatrix();
	glFrontFace(GL_CCW);
}

void Trendercontext::rendertext2d(StrPtr fontname, double x, double y, double size, StrPtr txt)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
/*	temp_mat44[0]=1; temp_mat44[4]=0; temp_mat44[ 8]=0; temp_mat44[12]=x;
	temp_mat44[1]=0; temp_mat44[5]=1; temp_mat44[ 9]=0; temp_mat44[13]=y;
	temp_mat44[2]=0; temp_mat44[6]=0; temp_mat44[10]=1; temp_mat44[14]=0;
	temp_mat44[3]=0; temp_mat44[7]=0; temp_mat44[11]=0; temp_mat44[15]=1;
	glMultMatrixd(temp_mat44);*/
	glTranslated(x,y,0);
	glScaled(size,size,0);
	glEnable(GL_CULL_FACE);

//	glListBase(rw->fontid1);
	int offset=rw->G_font(fontname)->G_ID();
	int len=qstrlen(txt);
	for (int i=0; i<len; i++)
	{
		unsigned char bt=(unsigned char)txt[i];
		glCallList(offset+bt);
	}
	glPopMatrix();
	glFrontFace(GL_CCW);
}


TDisplayList* Trendercontext::CreateDisplayList()
{
	TDisplayList *dlist=new TDisplayList(this);
	displaylists.add(dlist);
	return dlist;
}

void Trendercontext::ForgetDisplayList(TDisplayList *list)
{
	for (int i=0; i<displaylists.G_count(); i++)
		if (displaylists[i]==list) displaylists.del(i);
}

void Trendercontext::unproject(double screen_x, double screen_y, double dist, Tvertex &ps)
{
	int vp[18];
	double mmat[18],pmat[18];
	double xp,yp,zp;

	T3DViewport *viewport=G_viewport();
	if (viewport==NULL) return;

	glGetIntegerv(GL_VIEWPORT,vp);
	glGetDoublev(GL_MODELVIEW_MATRIX,mmat);
	glGetDoublev(GL_PROJECTION_MATRIX,pmat);

	//determine projected z-value at distance (for center of view direction)
	Tvertex pst;
	pst.lincombfrom(1.0,viewport->G_camerapos(),dist,viewport->G_cameradir());
	gluProject(pst.G3_x(),pst.G3_y(),pst.G3_z(),mmat,pmat,vp,&xp,&yp,&zp);

	gluUnProject(screen_x,screen_y,zp,mmat,pmat,vp,&xp,&yp,&zp);
	ps.copyfrom(xp,yp,zp);
}



///////////////////////////////////////////////////////////////////////
// TDisplayList
///////////////////////////////////////////////////////////////////////


TDisplayList::TDisplayList(Trendercontext *irc)
{
	rc=irc;
	listidx=glGenLists(1);
}

TDisplayList::~TDisplayList()
{
	glDeleteLists(listidx,1);
}

void TDisplayList::startrecord()
{
	glGetError();
	glNewList(listidx,GL_COMPILE_AND_EXECUTE);
	if (glGetError()!=GL_NO_ERROR) throw QError(_text("Unable to create display list"));
}

void TDisplayList::endrecord()
{
	glEndList();
}

void TDisplayList::draw()
{
	glCallList(listidx);
}




///////////////////////////////////////////////////////////////////////
// Trenderthread
///////////////////////////////////////////////////////////////////////

void Trenderthread::runthread(void *obj)
{
	((Trenderthread*)obj)->run();
}

void Trenderthread::run()
{
	while(true)
	{
		WaitForSingleObject(renderevent,INFINITE);
		if (threadstopping)
		{
			return;
		}
		error.clear();
		ResetEvent(renderevent);
		try{
			dorender(isrightpart);
		}
		catch(QError err)
		{
			error=err.G_content();
		}
		SetEvent(renderfinishevent);
		WaitForSingleObject(swapevent,INFINITE);
		ResetEvent(swapevent);
		win->swapbuffers();
		Sleep(0);
		SetEvent(swapfinishevent);
//		...nog betere synchro met swapbuffers?
	}
}

void Trenderthread::dorender(bool i_isrightpart)
{
	win->render_clear(curscene->G_backcolor());
	win->render_begin(curviewport,0,0,1,1,i_isrightpart);
	curscene->renderobjects(&win->rendercontext);
	win->render_end();
}

Trenderthread::Trenderthread(bool i_isrightpart)
{
	isrightpart=i_isrightpart;
	renderevent=CreateEvent(NULL,false,false,NULL);
	if (renderevent==NULL) throw_GetLastError(_qstr("CreateEvent"));
	renderfinishevent=CreateEvent(NULL,false,false,NULL);
	if (renderfinishevent==NULL) throw_GetLastError(_qstr("CreateEvent"));
	swapevent=CreateEvent(NULL,false,false,NULL);
	if (swapevent==NULL) throw_GetLastError(_qstr("CreateEvent"));
	swapfinishevent=CreateEvent(NULL,false,false,NULL);
	if (swapfinishevent==NULL) throw_GetLastError(_qstr("CreateEvent"));
	threadstopping=false;
}

Trenderthread::~Trenderthread()
{
	CloseHandle(renderevent);
	CloseHandle(renderfinishevent);
	CloseHandle(swapevent);
}

void Trenderthread::start(TRenderWindow *iwin)
{
	win=iwin;
	threadhandle=(HANDLE)_beginthread(runthread,0,(void*)this);
}

void Trenderthread::render(T3DScene *iscene, T3DViewport *iviewport)
{
	curscene=iscene;
	curviewport=iviewport;
	ResetEvent(renderfinishevent);
	SetEvent(renderevent);
}

void Trenderthread::swap()
{
	ResetEvent(swapfinishevent);
	SetEvent(swapevent);
}


void Trenderthread::waitforrenderfinished()
{
	WaitForSingleObject(renderfinishevent,INFINITE);
}

void Trenderthread::waitforswapfinished()
{
	WaitForSingleObject(swapfinishevent,INFINITE);
}


////////////////////////////////////////////////////////////////////////
// TRenderWindow
////////////////////////////////////////////////////////////////////////

//For 3D Connexion
[module(name="Zflux")];


BEGIN_MESSAGE_MAP(TRenderWindow,CWnd)
//  ON_WM_PAINT()
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
  ON_WM_CREATE()
  ON_WM_NCDESTROY()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

TRenderWindow::TRenderWindow(bool iisrightpart, TDisplayDevice *idisp) : rendercontext(this) , renderthread(iisrightpart)
{
	addlog(_text("Constructing render window"),+1);
	viewport=NULL;
	isrightpart=iisrightpart;
	disp=idisp;
	permdc=NULL;
	s_dwLastDraw=0;
	mouse_leftbuttondown=false;
	mouse_rightbuttondown=false;

	mouseshiftx=0;
	mouseshifty=0;
	mouseshiftz=0;
	centerposx=-10000;
	centerposy=-10000;
	addlog(_text("Render window constructed"),-1);


}

TRenderWindow::~TRenderWindow()
{

	addlog(_text("Destroying render window"),+1);
	if (permdc!=NULL) delete permdc;
	addlog(_text("Render window destroyed"),-1);
}



void TRenderWindow::createwin(RECT &boundingbox)
{
	addlog(_text("Creating render window"),+1);

	emptycursor=AfxGetApp()->LoadCursor(101);
	Create(
		    AfxRegisterWndClass
				(CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW|CS_OWNDC,emptycursor,
				(HBRUSH)GetStockObject(BLACK_BRUSH)
				),
		        _qstr("s-d"),
		        WS_CHILD|WS_VISIBLE,
				boundingbox,
				disp->G_win(),
		        0x12);
	permdc=new CClientDC(this);
	addlog(_text("Render window created"),-1);
}



int TRenderWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (__super::OnCreate(lpCreateStruct) == -1)
      return -1;

	#ifdef _3DConnexion
		HRESULT hr = InitializeCOM();
	#endif

	return 0;
}

void TRenderWindow::OnNcDestroy()
{
   __super::OnNcDestroy();
}




void TRenderWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	G_3DCosmos().UI_OnKeyDown(nChar,nRepCnt,nFlags);
}

void TRenderWindow::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	G_3DCosmos().UI_OnChar(nChar,nRepCnt,nFlags);
}


void TRenderWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	TMouseClickInfo info;
	G_3DCosmos().UI_OnMouseArrowClick(info);
	mouse_leftbuttondown=true;
	SetCapture();
}

void TRenderWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	TMouseClickInfo info;
	info.clicktype=TMouseClickInfo::LeftDouble;
	G_3DCosmos().UI_OnMouseArrowClick(info);
}


void TRenderWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	mouse_leftbuttondown=false;
	ReleaseCapture();
}

void TRenderWindow::OnRButtonDown(UINT nFlags, CPoint point)
{
	mouse_rightbuttondown=true;
	SetCapture();
}

void TRenderWindow::OnRButtonUp(UINT nFlags, CPoint point)
{
	mouse_rightbuttondown=false;
	ReleaseCapture();
}


void TRenderWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetFocus()!=this) return;
	int deltax=0;
	if (centerposx!=-10000)
		deltax=point.x-centerposx;
	int deltay=0;
	if (centerposy!=-10000)
		deltay=point.y-centerposy;

	if (abs(deltax)>0) G_3DCosmos().UIaction_axis(UIA_X,GetAxisLevelKBModifier(),deltax/13.0);
	if (abs(deltay)>0) G_3DCosmos().UIaction_axis(UIA_Y,GetAxisLevelKBModifier(),-deltay/13.0);

	mouseshiftx+=deltax;
	mouseshifty+=deltay;
	centerposx=point.x;
	centerposy=point.y;
	CenterCursor();
}

BOOL TRenderWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	mouseshiftz+=zDelta/2;
	if (zDelta!=0) G_3DCosmos().UIaction_axis(UIA_Z,GetAxisLevelKBModifier(),zDelta/2.0/20.0);

	TMouseClickInfo info;
	info.clicktype=zDelta<0 ? TMouseClickInfo::ScrollDown : TMouseClickInfo::ScrollUp;
	G_3DCosmos().UI_OnMouseArrowClick(info);

	return TRUE;
}


void TRenderWindow::OnActivate()
{
	mouseshiftx=0;
	mouseshifty=0;
	mouseshiftz=0;
	CenterCursor();
}

void TRenderWindow::CenterCursor()
{
	RECT rc;
	POINT pt,wincenter;

	GetClientRect(&rc);
	wincenter.x=(rc.left+rc.right)/2;
	wincenter.y=(rc.top+rc.bottom)/2;

	GetCursorPos(&pt);
	ScreenToClient(&pt);
	if ((abs(pt.x-wincenter.x)>100)||(abs(pt.y-wincenter.y)>100))
	{
		pt=wincenter;
		centerposx=pt.x;
		centerposy=pt.y;
		ClientToScreen(&pt);
		SetCursorPos(pt.x,pt.y);
	}
}


void TRenderWindow::setup()
{
	unsigned int fl;
	int colordepth;
	QString error;

	addlog(_text("Setup of render window context"),+1);



	fl=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	//|PFD_STEREO;
	colordepth=32;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
			1,
			fl,
			PFD_TYPE_RGBA|PFD_SWAP_EXCHANGE,
			colordepth,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			colordepth,
			8,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};


	int pixelformat;
	pixelformat=ChoosePixelFormat(permdc->GetSafeHdc(),&pfd);
	if (pixelformat==0) throw QError(_qstr("Unsupported pixel format"));

	BOOL rt=SetPixelFormat(permdc->GetSafeHdc(),pixelformat,&pfd);
	if (rt==FALSE) throw_GetLastError(_qstr("SetPixelFormat"));

	rc=wglCreateContext(permdc->GetSafeHdc());
	if (rc==0) throw_GetLastError(_qstr("wglCreateContext"));
	if (wglMakeCurrent(permdc->GetSafeHdc(),rc)==FALSE) throw_GetLastError(_qstr("wglMakeCurrent"));

	if (GLEE_WGL_EXT_swap_control)
	{
		int cnt=G_3DCosmos().G_vsynccount();
		if (!wglSwapIntervalEXT(cnt)) reporterror(_text("Could not execute wglSwapIntervalEXT"));
		if (wglGetSwapIntervalEXT()!=cnt) reporterror(_text("wglSwapIntervalEXT failed to set correct value"));
	}
	else throw QError(_text("WGL_EXT_swap is not supported"));


	setdefaults();

	setup_fonts();

	rendercontext.setup();

	rendercontext.checkerror(_text("End setup"));


	//check OpenGL version
	int ver_major,ver_minor;
	const char * verstring=(const char *)glGetString(GL_VERSION);
	if (verstring==NULL) throw QError(_text("Unable to determine version"));
	if (sscanf(verstring,"%d.%d",&ver_major,&ver_minor)!=2) throw QError(_text("Failed to determine version"));
	addlog(TFormatString(_text("OpenGL version ^1.^2"),ver_major,ver_minor));
	if (ver_major<2) throw QError(_text("Invalid OpenGL version"));

	InitOpenGLExtensions();


	//wglMakeCurrent(permdc->GetSafeHdc(),NULL);//???
	wglMakeCurrent(NULL,NULL);//???




	if (G_rendertype()==RenderMultithreaded) renderthread.start(this);


	addlog(_text("Completed setup of render window context"),-1);

}


T3DFont* TRenderWindow::G_font(StrPtr iname)
{
	for (int i=0; i<fonts.G_count(); i++)
		if (issame(iname,fonts[i]->G_name())) return fonts[i];
	throw QError(TFormatString(_text("Unable to find font ^1"),iname));
}


void TRenderWindow::setup_fonts()
{
	addlog(_text("Setting render fonts"),+1);

	{
		T3DFont *f1=new T3DFont(DEFAULT_FONT,300);fonts.add(f1);
		if (!isrightpart)
		{
			CFont fnt3;
			fnt3.CreateFont(-18,0, 0,0, FW_NORMAL,0,0,0, 
				   ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,
				   FF_SWISS|VARIABLE_PITCH,_qstr("Verdana"));
			permdc->SelectObject(fnt3);
			f1->init(permdc);
			permdc->SelectObject(GetStockObject(ANSI_VAR_FONT));
		}
	}
	{
		T3DFont *f1=new T3DFont(BOLD_FONT,600);fonts.add(f1);
		if (!isrightpart)
		{
			CFont fnt3;
			fnt3.CreateFont(-18,0, 0,0, FW_BOLD,0,0,0, 
				   ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,
				   FF_SWISS|VARIABLE_PITCH,_qstr("Verdana"));
			permdc->SelectObject(fnt3);
			f1->init(permdc);
			permdc->SelectObject(GetStockObject(ANSI_VAR_FONT));
		}
	}
	{
		T3DFont *f1=new T3DFont(ITALIC_FONT,900);fonts.add(f1);
		if (!isrightpart)
		{
			CFont fnt3;
			fnt3.CreateFont(-18,0, 0,0, FW_NORMAL,1,0,0, 
				   ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,
				   FF_SWISS|VARIABLE_PITCH,_qstr("Verdana"));
			permdc->SelectObject(fnt3);
			f1->init(permdc);
			permdc->SelectObject(GetStockObject(ANSI_VAR_FONT));
		}
	}
	{
		T3DFont *f1=new T3DFont(SYMBOL_FONT,1200);fonts.add(f1);
		if (!isrightpart)
		{
			CFont fnt3;
			fnt3.CreateFont(-18,0, 0,0, FW_NORMAL,0,0,0, 
				   ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,
				   FF_SWISS|VARIABLE_PITCH,_qstr("Symbol"));
			permdc->SelectObject(fnt3);
			f1->init(permdc);
			permdc->SelectObject(GetStockObject(ANSI_VAR_FONT));
		}
	}

	addlog(_text("Completed setting render fonts"),-1);

}


void TRenderWindow::setdefaults()
{
	addlog(_text("Setting render defaults"),+1);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_FALSE);
	glEnable(GL_LIGHT0);

	GLfloat ambli0[]= {0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambli0);

	GLfloat difli0[]= {1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0,GL_DIFFUSE,difli0);

	GLfloat ambient[]= { 0.03f, 0.03f, 0.03f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);


	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	addlog(_text("Completed setting render defaults"),-1);
}

void TRenderWindow::GetRC()
{
	HGLRC currentrc=wglGetCurrentContext();
	if (currentrc!=rc)
	{
		if (currentrc!=NULL)
		{
			if (!wglMakeCurrent(NULL,NULL))
			{
				ASSERT(false);
				throw_GetLastError(_qstr("Could not release rendering context"));
			}
		}
		if (!wglMakeCurrent(permdc->GetSafeHdc(),rc))
		{
			ASSERT(false);
			throw_GetLastError(_qstr("Could not activate rendering context"));
		}
	}
}

void TRenderWindow::ReleaseRC()
{
//	wglMakeCurrent(permdc->GetSafeHdc(),NULL);
	wglMakeCurrent(NULL,NULL);
}


void TRenderWindow::initviewmatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	double eyesep=viewport->G_eyeseparation();
	int fact=-1;
	if (isrightpart) fact=+1;

	Tvector horizdir;
	horizdir.vecprod(viewport->G_cameradir(),viewport->G_cameraupdir());
	horizdir.normfrom(&horizdir);

	Tvector eyesepdir;eyesepdir.mulfrom(&horizdir,eyesep/2*fact);
	Tvertex eyepos;eyepos.add(viewport->G_camerapos(),&eyesepdir);

	Tvector viewdir;viewdir.normfrom(viewport->G_cameradir());viewdir.mulfrom(&viewdir,100.0);
	Tvertex lookatpos;lookatpos.add(&eyepos,&viewdir);

	Tvector updir;
	updir.vecprod(&horizdir,viewport->G_cameradir());
	updir.normfrom(&updir);

	gluLookAt(eyepos.G3_x(),eyepos.G3_y(),eyepos.G3_z(),
		      lookatpos.G3_x(),lookatpos.G3_y(),lookatpos.G3_z(),
			  updir.G_x(),updir.G_y(),updir.G_z());

	//apply viewport transformation
	double temp_mat44[16];
	viewport->G_transformation()->streamout(temp_mat44);
	glMultMatrixd(temp_mat44);


	glGetDoublev(GL_MODELVIEW_MATRIX,viewmatrix);
}

void TRenderWindow::render_clear(TSC_color *backcolor)
{
	GetRC();
	glClearColor(backcolor->G_R(),backcolor->G_G(),backcolor->G_B(),backcolor->G_A());
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}


void TRenderWindow::render_begin(T3DViewport *iviewport, double frxmin, double frymin, double frxmax, double frymax, bool i_isrightpart)
{
	RECT rec;

	isrightpart=i_isrightpart;
	viewport=iviewport;
	GetClientRect(&rec);
	int imasizex=rec.right;
	int imasizey=rec.bottom;
	renderviewport.x0=frxmin*imasizex;
	renderviewport.y0=frymin*imasizey;
	renderviewport.lx=(frxmax-frxmin)*imasizex;
	renderviewport.ly=(frymax-frymin)*imasizey;
	screenratio=(renderviewport.lx/renderviewport.ly)/abs(viewport->G_hstretchfactor());

	//if (i_isrightpart)
	//	renderviewport.y0-=160;

	if (viewport->RenderToTexture)
	{
		Tframebufferobject *renbuff=&viewport->RTT_fbo_left;
		if (isrightpart) renbuff=&viewport->RTT_fbo_right;
		if (!renbuff->G_ispresent())
		{
			//suggestion: to correct for inaccuracies because of warp inflation, render to a higher resolution first
			int resx=viewport->RTT_imasizex;
			int resy=viewport->RTT_imasizey;
			renbuff->create(&rendercontext,resx,resy);

		}
		renbuff->bind();
		rendercontext.checkerror(_text("FBO bound"));
		render_clear(&TSC_color(0.0,0.0,0.0,1.0));
	}


	if (!viewport->RenderToTexture)
	{
		glDrawBuffer(GL_BACK);
		rendercontext.checkerror(_text("Set glBackBuffer"));
	}

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_FALSE);
	glColor4d(1,1,1,1);
	GLfloat mat_emi0[]={0.0,0.0,0.0,1.0};
	::glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,mat_emi0);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glLineWidth(1);

	GLfloat mat_speccol[]={0.0,0.0,0.0,1.0};
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_speccol);
	float shinival=30.0;
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&shinival);

	if (!viewport->RenderToTexture)
	{
		glViewport(renderviewport.x0,renderviewport.y0,renderviewport.lx,renderviewport.ly);
	}
	else
	{
	}

	//create frustum


//	if (viewport->G_isHmirrorred(isrightpart)) screenratio=-screenratio;
	double nearplane=viewport->G_nearclipplane();
	double farplane=viewport->G_farclipplane();
	createfrustum(nearplane,farplane);

//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluPerspective(45.0f,(GLfloat)imasizex/(GLfloat)imasizey,0.01f,10.0f);

	initviewmatrix();


	GLfloat lipos0[]= {(GLfloat)10.0,(GLfloat)20.0,(GLfloat)10.0,(GLfloat).0};
	glLightfv(GL_LIGHT0,GL_POSITION,lipos0);


	rendercontext.set_orientation(+1);
	rendercontext.isrightpart=i_isrightpart;
	viewport->G_name(rendercontext.viewportname);
}


void TRenderWindow::createfrustum(double nearplane, double farplane)
{
	rendercontext.Hmirrorred=viewport->G_isHmirrorred(isrightpart);
	rendercontext.Vmirrorred=viewport->G_isVmirrorred(isrightpart);

	int fact=-1;
	if (isrightpart) fact=+1;
	double eyesep=viewport->G_eyeseparation();
	double focallength=viewport->G_focaldistance();
	double aperture=viewport->G_aperture();
	double wd2=nearplane*tan(aperture/2.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double offsetx=viewport->G_xoffsetfrac()*wd2*2;

	double left   = -screenratio*wd2 - fact*0.5*(eyesep/focallength)*nearplane+offsetx;
	double right  = +screenratio*wd2 - fact*0.5*(eyesep/focallength)*nearplane+offsetx;
	if (rendercontext.Hmirrorred)
	{
		left   = +screenratio*wd2 - fact*0.5*(eyesep/focallength)*nearplane+offsetx;
		right  = -screenratio*wd2 - fact*0.5*(eyesep/focallength)*nearplane+offsetx;
	}

	double top    = +wd2;
	double bottom = -wd2;
	if (rendercontext.Vmirrorred)
	{
		top    = -wd2;
		bottom = +wd2;
	}

	double frx1=viewport->zoomx1->G_val();
	double frx2=viewport->zoomx2->G_val();
	double fry1=viewport->zoomy1->G_val();
	double fry2=viewport->zoomy2->G_val();
	double left2=left+(right-left)*frx1;
	double right2=left+(right-left)*frx2;
	double top2=top+(bottom-top)*fry1;
	double bottom2=top+(bottom-top)*fry2;

	glFrustum(left2,right2,bottom2,top2,nearplane,farplane);

}


void TRenderWindow::render_end()
{
	if (viewport->RenderToTexture)
	{
		Tframebufferobject *renbuff=&viewport->RTT_fbo_left;
		if (isrightpart) renbuff=&viewport->RTT_fbo_right;
		renbuff->unbind();
		glViewport(renderviewport.x0,renderviewport.y0,renderviewport.lx,renderviewport.ly);
		renbuff->activate_texture();
		rendercontext.start_viewportframe(false);
		rendercontext.enablelight(false);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		rendercontext.EnableDepthTest(false);
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		viewport->RTT_warp.apply(&rendercontext);
		rendercontext.EnableDepthTest(true);
		rendercontext.del_subframe();
	}

	glFlush();
	glFinish();
	//ReleaseRC();//???
	viewport=NULL;

}

void TRenderWindow::swapbuffers()
{
	if (!SwapBuffers(permdc->GetSafeHdc()))
		throw_GetLastError(_qstr("SwapBuffers"));
}




/////////////////////////////////////////////////////////////////////////////////////////
// Tools for 3DConnexion IO


#ifdef _3DConnexion
HRESULT TRenderWindow::InitializeCOM()
{
   HRESULT hr;
   CComPtr<IUnknown> _3DxDevice;

   // Create the device object
   hr = _3DxDevice.CoCreateInstance(__uuidof(Device));
   if (SUCCEEDED(hr))
   {

      CComPtr<ISimpleDevice> _3DxSimpleDevice;

      hr = _3DxDevice.QueryInterface(&_3DxSimpleDevice);


      if (SUCCEEDED(hr))
      {

         // Set the preferences we want to use
         _3DxSimpleDevice->LoadPreferences(L"Z-Flux");

         hr = __hook(&_ISimpleDeviceEvents::DeviceChange, 
                        _3DxSimpleDevice, 
                        &TRenderWindow::OnDeviceChange,
                        this);

         // Get the interfaces to the sensor and the keyboard;
         hr = _3DxSimpleDevice->get_Sensor(&m_pISensor);

         hr = __hook(&_ISensorEvents::SensorInput, m_pISensor, 
                        &TRenderWindow::OnSensorInput);

         hr = _3DxSimpleDevice->get_Keyboard(&m_pIKeyboard);

         hr = __hook(&_IKeyboardEvents::KeyDown, m_pIKeyboard, 
                        &TRenderWindow::OnKeyDown);

         hr = __hook(&_IKeyboardEvents::KeyUp, m_pIKeyboard, 
                        &TRenderWindow::OnKeyUp);

         // Connect to the driver
         _3DxSimpleDevice->Connect();
      }
      _3DxDevice.Release();
   }
   else
      //::MessageBox (NULL, 
      //               _T("Could not create Device"),
      //               _T("CoCreateInstance failed"), 
      //               MB_ICONERROR|MB_OK);
   return hr;
}
#endif

#ifdef _3DConnexion
HRESULT TRenderWindow::OnDeviceChange(long reserved )
{
   HRESULT result = S_OK;
   AtlTrace (_T("CCubeWindow::OnDeviceChange(reserved =0x%x)\n"), reserved);

   HRESULT hr;
   ISimpleDevicePtr _p3DxDevice;
   hr = m_pIKeyboard->get_Device((IDispatch**)&_p3DxDevice);
   if (SUCCEEDED(hr))
   {
      long device;
      _p3DxDevice->get_Type(&device);
      AtlTrace (_T("Attached device=%d\n"), device);
      _p3DxDevice.Release();

      long keys, programmableKeys;
      m_pIKeyboard->get_Keys(&keys);
      m_pIKeyboard->get_ProgrammableKeys(&programmableKeys);
      AtlTrace (_T("Number of Keys=%d\tProgrammable keys=%d\n"), keys, programmableKeys);
      for (long i=1; i<=keys; i++)
      {
         BSTR bstrLabel;
         BSTR bstrName;
         m_pIKeyboard->GetKeyLabel(i, &bstrLabel);
         m_pIKeyboard->GetKeyName(i, &bstrName);

         CString strLabel(bstrLabel);
         CString strName(bstrName);

         AtlTrace (_T("Key Label=%s\tKey Name=%s\n"), (const TCHAR *)strLabel, (const TCHAR *)strName);
      }
   }
   return result;
}
#endif

#ifdef _3DConnexion
HRESULT TRenderWindow::OnKeyDown(int keyCode )
{
   HRESULT result = S_OK;
   AtlTrace (_T("CCubeWindow::OnKeyDown(keyCode =%d)\n"), keyCode);
   switch (keyCode)
   {
   case 31: // Fit
/*      MathFrameTranslation( &FrameCube, 0.0, 0.0, -10. );
      MathFrameRotation( &FrameCube, 0., 0., 0. );
      m_wndView.RedrawWindow(NULL, NULL, RDW_INVALIDATE);*/
      break;
   }
   return result;
}
#endif


#ifdef _3DConnexion
HRESULT TRenderWindow::OnKeyUp(int keyCode )
{
   HRESULT result = S_OK;
   AtlTrace (_T("CCubeWindow::OnKeyUp(keyCode =%d)\n"), keyCode);
   return result;
}
#endif



#ifdef _3DConnexion
HRESULT TRenderWindow::OnSensorInput(void)
{

   HRESULT result = S_OK;
   static DWORD s_dwLastDraw = 0;

	if (G_3DCosmos().G_selviewport()==NULL) return result;

   AtlTrace (_T("CCubeWindow::OnSensorInput()\n"));
//   try {
      CComPtr<IAngleAxis> pRotation;
      HRESULT hr = m_pISensor->get_Rotation(&pRotation);

      double angle;
      pRotation->get_Angle(&angle);


      CComPtr<IVector3D> pTranslation;
      hr = m_pISensor->get_Translation(&pTranslation);
      
 
      double length;
      pTranslation->get_Length(&length);

      if ((angle > 0. || length > 0.) )
      {


		  double timeFactor=1;
		 DWORD dwNow = ::GetTickCount();
		 if (s_dwLastDraw)
		 {
			double period;
			m_pISensor->get_Period(&period);
//			timeFactor = (double)(dwNow-s_dwLastDraw)/period;
		 }
		 s_dwLastDraw = dwNow;


		double x,y,z;


		pRotation->get_X(&x);
		pRotation->get_Y(&y);
		pRotation->get_Z(&z);

		angle *= timeFactor;
		angle /= 1024*6;
		x*=angle;
		y*=angle;
		z*=angle;
//		G_3DCosmos().G_selviewport()->currentmove.scenerot=Tvector(x,-z,-y);
		G_3DCosmos().G_selviewport()->currentmove.scenerot=Tvector(x,y,z);






		length *= timeFactor;
		length /= 1024*3;
		pTranslation->put_Length(length);

		pTranslation->get_X(&x);
		pTranslation->get_Y(&y);
		pTranslation->get_Z(&z);

//		G_3DCosmos().G_selviewport()->currentmove.cammove=Tvector(x,-z,-y);
		G_3DCosmos().G_selviewport()->currentmove.cammove=Tvector(x,y,z);
      }
      else
	  {
         s_dwLastDraw = 0;
		G_3DCosmos().G_selviewport()->resetcurrentmove();
	  }

      pRotation.Release();
      pTranslation.Release();

   return result;
}
#endif

#ifdef _3DConnexion
HRESULT TRenderWindow::UninitializeCOM()
{
   HRESULT hr = E_FAIL;

   CComPtr<IDispatch> _3DxDevice;
   if (m_pISensor)
      hr = m_pISensor->get_Device(&_3DxDevice);

   else if (m_pIKeyboard)
      hr = m_pIKeyboard->get_Device(&_3DxDevice);

   if (SUCCEEDED(hr))
   {
      CComPtr<ISimpleDevice> _3DxSimpleDevice;
      hr = _3DxDevice.QueryInterface(&_3DxSimpleDevice);
      if (SUCCEEDED(hr))
      {
         __unhook(&_ISimpleDeviceEvents::DeviceChange, 
                        _3DxSimpleDevice, 
                        &TRenderWindow::OnDeviceChange,
                        this);

         _3DxSimpleDevice->Disconnect();
      }
   }

   if (m_pISensor)
   {
      // unhook (unadvise) the sensor event sink
      __unhook(&_ISensorEvents::SensorInput, m_pISensor, 
                     &TRenderWindow::OnSensorInput);

      m_pISensor.Release();
   }

   if (m_pIKeyboard)
   {
      __unhook(&_IKeyboardEvents::KeyDown, m_pIKeyboard, 
                     &TRenderWindow::OnKeyDown);

      __unhook(&_IKeyboardEvents::KeyUp, m_pIKeyboard, 
                     &TRenderWindow::OnKeyUp);

      m_pIKeyboard.Release();
   }

   return hr;
}
#endif