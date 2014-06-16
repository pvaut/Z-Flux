#include "stdafx.h"

#include "qbintagfile.h"

#include "vecmath.h"
#include "vec_transformation.h"

#include "FormatText.h"

#include "opengl.h"

#include "3Denums.h"
#include "3DScene.h"
#include "3Dcosmos.h"
#include "3DSpecialObjects.h"
#include "3DGeoObjects.h"
#include "3DMeshObject.h"
#include "3Dsurface.h"
#include "3DCurve.h"
#include "3DPengine.h"
#include "3DStarGlobe.h"
#include "3DStarMap.h"
#include "3DClock.h"
#include "3DFormattedText.h"

#include "3DControls.h"

#include "sound.h"

#include "CSG_shape.h"
#include "Blob.h"

#include "bezier.h"

#include "displaydevices.h"
#include "qxformattedstring.h"

#include "qxstoredbitmap.h"

#include "astrotools.h"

void copyclipboard(StrPtr str);


#define TREECLASS_MOTION "Motions"

StrPtr G_datadir();

TObjectTreeItem* G_treeobject_adapterlist();
bool IsAnimationRunning();

TRenderWindow* G_globalrenderwindow();

bool IsKeyDown(int virtkey);

int G_rendertype()
{
	return T3DCosmos::Get().G_rendertype();
}


template <class valuetype>
class TSC_datatype_derived_objecttreeitem : public TSC_datatype_generic_withmemberfunctions<valuetype>
{
public:
	TSC_datatype_derived_objecttreeitem(StrPtr iname)
		: TSC_datatype_generic_withmemberfunctions<valuetype>(iname,false)
	{
		classpath=CLASSNAME_OBJTREE;
		valuetype tp;
		for (int i=0; i<tp.G_paramcount(); i++)
		{
			ASSERT(tp.G_paramcontent(i)!=NULL);
			ASSERT(tp.G_paramcontent(i)->G_datatype()!=NULL);
			TQXFormattedString prototype;
			prototype+=tp.G_paramcontent(i)->G_datatype()->G_name();
			prototype+=_qstr(" ");
//			prototype+=G_name();
			prototype+=_qstr("X . ");
			prototype.makebold(true);
			prototype+=tp.G_paramname(i);
			prototype.makebold(false);
			prototype+=_qstr(" =");
			addmemberfunction(tp.G_paramname(i),prototype,tp.G_paramcontent(i)->G_datatype()->G_name(),tp.G_paramsubclass(i));
		}
	}
};

template <class valuetype>
class TSC_datatype_geometricobject : public TSC_datatype_derived_objecttreeitem<valuetype>
{
public:
	TSC_datatype_geometricobject(StrPtr iname)
		: TSC_datatype_derived_objecttreeitem<valuetype>(iname)
	{
		append_classpath(CLASSNAME_GEOMETRICOBJECTS);
	}
};

template <class valuetype>
class TSC_datatype_specialobject : public TSC_datatype_derived_objecttreeitem<valuetype>
{
public:
	TSC_datatype_specialobject(StrPtr iname)
		: TSC_datatype_derived_objecttreeitem<valuetype>(iname)
	{
		append_classpath(CLASSNAME_SPECIALOBJECTS);
	}
};


void create_externaltypes()
{
	TSC_datatype *dtt;

	addscriptenumtypes();

	dtt=GetTSCenv().adddatatype(new TSC_datatype_generic<TSC_color>(SC_valname_color,true));
	dtt->classpath=SC_simpletypepath;

	dtt=GetTSCenv().adddatatype(new TSC_datatype_generic<Tbmp>(SC_valname_bitmap,true));
	dtt->classpath=SC_mediapath;

	GetTSCenv().adddatatype(new TSC_datatype_generic<Tvertex>(SC_valname_vertex,true));
	GetTSCenv().G_datatype(SC_valname_vertex)->classpath=CLASSNAME_MATHEMATICS;
	GetTSCenv().adddatatype(new TSC_datatype_generic<Tvector>(SC_valname_vector,true));
	GetTSCenv().G_datatype(SC_valname_vector)->classpath=CLASSNAME_MATHEMATICS;
	GetTSCenv().adddatatype(new TSC_datatype_generic<Tplane>(SC_valname_plane,true));
	GetTSCenv().G_datatype(SC_valname_plane)->classpath=CLASSNAME_MATHEMATICS;
	GetTSCenv().adddatatype(new TSC_datatype_generic<Tline>(SC_valname_line,true));
	GetTSCenv().G_datatype(SC_valname_line)->classpath=CLASSNAME_MATHEMATICS;
	GetTSCenv().adddatatype(new TSC_datatype_generic<TLineSegment>(SC_valname_linesegment,true));
	GetTSCenv().G_datatype(SC_valname_linesegment)->classpath=CLASSNAME_MATHEMATICS;

	GetTSCenv().adddatatype(new TSC_datatype_generic<TMatrix>(SC_valname_matrix,true));
	GetTSCenv().G_datatype(SC_valname_matrix)->classpath=CLASSNAME_MATHEMATICS;
	GetTSCenv().adddatatype(new TSC_datatype_generic<Taffinetransformation>(SC_valname_affinetransformation,true));
	GetTSCenv().G_datatype(SC_valname_affinetransformation)->classpath=CLASSNAME_MATHEMATICS;

	GetTSCenv().adddatatype(new TSC_datatype_generic<TSC_functor>(SC_valname_functor,false));
	GetTSCenv().G_datatype(SC_valname_functor)->classpath=CLASSNAME_MATHEMATICS;

	GetTSCenv().adddatatype(new TSC_datatype_generic<TSC_forcefield>(SC_valname_forcefield,true));
	GetTSCenv().G_datatype(SC_valname_forcefield)->classpath=CLASSNAME_MATHEMATICS;


	GetTSCenv().adddatatype(new TSC_datatype_generic<T2DContourset>(SC_valname_2dcontourset,true));
	GetTSCenv().G_datatype(SC_valname_2dcontourset)->classpath=CLASSNAME_GEOMETRY;

	GetTSCenv().adddatatype(new TSC_datatype_generic<CSGShape>(SC_valname_shape,true));
	GetTSCenv().G_datatype(SC_valname_shape)->classpath=CLASSNAME_GEOMETRY;

	GetTSCenv().adddatatype(new TSC_datatype_generic<TBlob>(SC_valname_blob,true));
	GetTSCenv().G_datatype(SC_valname_blob)->classpath=CLASSNAME_GEOMETRY;

	GetTSCenv().adddatatype(new TSC_datatype_generic<TPolyBezier3D>(SC_valname_polybeziercurve,true));
	GetTSCenv().G_datatype(SC_valname_polybeziercurve)->classpath=CLASSNAME_INTERPOLATION;
	GetTSCenv().adddatatype(new TSC_datatype_generic<TPolyBezierSurface>(SC_valname_polybeziersurface,true));
	GetTSCenv().G_datatype(SC_valname_polybeziersurface)->classpath=CLASSNAME_INTERPOLATION;

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<TObjectTreeItem>(SC_valname_object));

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DCosmos>(SC_valname_objectroot));

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DScene>(SC_valname_scene));

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DSubFrameObject>(SC_valname_subframe));

	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectMesh>(SC_valname_meshobject));
	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectBar>(SC_valname_bar));
	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectSphere>(SC_valname_sphere));
	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectCylinder>(SC_valname_cylinder));
	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectPie>(SC_valname_pie));
	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectRectangle>(SC_valname_rectangle));
	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectSolidArrow>(SC_valname_solidarrow));
	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectPoint>(SC_valname_solidpoint));
	

	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObject3DText>(SC_valname_text3d));
	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectFlatText>(SC_valname_textflat));
	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObject3DFormattedText>(SC_valname_formattedtext3d));


	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectSurface>(SC_valname_surface));
	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectCurve>(SC_valname_curve));

	GetTSCenv().adddatatype(new TSC_datatype_specialobject<T3DObjectLightPoint>(SC_valname_lightpoint));
	GetTSCenv().adddatatype(new TSC_datatype_specialobject<T3DRepeatedObject>(SC_valname_repeatedobject));
	GetTSCenv().adddatatype(new TSC_datatype_specialobject<T3DObjectPEngine>(SC_valname_pengine));
	GetTSCenv().adddatatype(new TSC_datatype_specialobject<T3DObjectFog>(SC_valname_fog));

	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectStarGlobe>(SC_valname_starglobe));
	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectStarMap>(SC_valname_starmap));
	GetTSCenv().adddatatype(new TSC_datatype_geometricobject<T3DObjectClock>(SC_valname_clock));

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DControl_Frame>(SC_valname_control_frame));
	GetTSCenv().G_datatype(SC_valname_control_frame)->classpath=SC_treeclass_controls;

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DControl_Text>(SC_valname_control_text));
	GetTSCenv().G_datatype(SC_valname_control_text)->classpath=SC_treeclass_controls;

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DControl_Scalar>(SC_valname_control_scalar));
	GetTSCenv().G_datatype(SC_valname_control_scalar)->classpath=SC_treeclass_controls;

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DControl_Edit>(SC_valname_control_edit));
	GetTSCenv().G_datatype(SC_valname_control_edit)->classpath=SC_treeclass_controls;

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DControl_List>(SC_valname_control_list));
	GetTSCenv().G_datatype(SC_valname_control_list)->classpath=SC_treeclass_controls;

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DControl_Check>(SC_valname_control_check));
	GetTSCenv().G_datatype(SC_valname_control_check)->classpath=SC_treeclass_controls;

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DControl_Button>(SC_valname_control_button));
	GetTSCenv().G_datatype(SC_valname_control_button)->classpath=SC_treeclass_controls;

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DControl_Menu>(SC_valname_control_menu));
	GetTSCenv().G_datatype(SC_valname_control_menu)->classpath=SC_treeclass_controls;


	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DTexture>(SC_valname_texture));

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DViewport>(SC_valname_viewport));
	GetTSCenv().G_datatype(SC_valname_viewport)->classpath=CLASSNAME_OBJTREE;

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DVideo>(SC_valname_video));
	GetTSCenv().G_datatype(SC_valname_video)->classpath=SC_mediapath;

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<TSound>(SC_valname_sound));
	GetTSCenv().G_datatype(SC_valname_sound)->classpath=SC_mediapath;

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DMotionTable>(SC_valname_motiontable));
	GetTSCenv().G_datatype(SC_valname_motiontable)->classpath=_qstr(TREECLASS_MOTION);
	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DMotionSpline>(SC_valname_motionspline));
	GetTSCenv().G_datatype(SC_valname_motionspline)->classpath=_qstr(TREECLASS_MOTION);
	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DMotionRotate>(SC_valname_motionrotate));
	GetTSCenv().G_datatype(SC_valname_motionrotate)->classpath=_qstr(TREECLASS_MOTION);

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DMotionSwing>(SC_valname_motionswing));
	GetTSCenv().G_datatype(SC_valname_motionswing)->classpath=_qstr(TREECLASS_MOTION);

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DMotionKepler>(SC_valname_motionkepler));
	GetTSCenv().G_datatype(SC_valname_motionkepler)->classpath=_qstr(TREECLASS_MOTION);
	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DMotionCyclOrbit>(SC_valname_motioncyclorbit));
	GetTSCenv().G_datatype(SC_valname_motioncyclorbit)->classpath=_qstr(TREECLASS_MOTION);
	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DMotionLuna>(SC_valname_motionluna));
	GetTSCenv().G_datatype(SC_valname_motionluna)->classpath=_qstr(TREECLASS_MOTION);

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DMotionForceField>(SC_valname_motionforcefield));
	GetTSCenv().G_datatype(SC_valname_motionforcefield)->classpath=_qstr(TREECLASS_MOTION);

	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DMotionSolidFunctor>(SC_valname_motionsolidfunctor));
	GetTSCenv().G_datatype(SC_valname_motionsolidfunctor)->classpath=_qstr(TREECLASS_MOTION);
	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<T3DMotionPointFunctor>(SC_valname_motionpointfunctor));
	GetTSCenv().G_datatype(SC_valname_motionpointfunctor)->classpath=_qstr(TREECLASS_MOTION);


	GetTSCenv().adddatatype(new TSC_datatype_derived_objecttreeitem<TDisplayDevice>(SC_valname_display));


}



//*********************************************************************
// T3DViewportlist
//*********************************************************************


int T3DViewportlist::G_childcount()
{
	return viewports.G_count();
}
TObjectTreeItem* T3DViewportlist::G_child(int nr)
{
	return viewports[nr];
}


void T3DViewportlist::delchild(TObjectTreeItem *ichild)
{
	for (int i=0; i<viewports.G_count(); i++)
		if (ichild==(TObjectTreeItem*) viewports[i])
		{
			viewports.del(i);
			return;
		}
}

T3DViewport* T3DViewportlist::addviewport()
{
	T3DViewport *vp=new T3DViewport;
	viewports.add(vp);
	return vp;
}

void T3DViewportlist::reset()
{
	viewports.reset();
}


//*********************************************************************
// T3DVideolist
//*********************************************************************


int T3DVideolist::G_childcount()
{
	return videos.G_count();
}
TObjectTreeItem* T3DVideolist::G_child(int nr)
{
	return videos[nr];
}


void T3DVideolist::delchild(TObjectTreeItem *ichild)
{
	for (int i=0; i<videos.G_count(); i++)
		if (ichild==(TObjectTreeItem*) videos[i])
		{
			videos.del(i);
			return;
		}
}

T3DVideo* T3DVideolist::addvideo()
{
	T3DVideo *vd=new T3DVideo;
	videos.add(vd);
	return vd;
}


void T3DVideolist::reset()
{
	videos.reset();
}

void T3DVideolist::loadcurframes()
{
	for (int i=0; i<videos.G_count(); i++)
		videos[i]->loadcurframe();
}



//*********************************************************************
// TSoundlist
//*********************************************************************


int TSoundlist::G_childcount()
{
	return sounds.G_count();
}
TObjectTreeItem* TSoundlist::G_child(int nr)
{
	return sounds[nr];
}


void TSoundlist::delchild(TObjectTreeItem *ichild)
{
	for (int i=0; i<sounds.G_count(); i++)
		if (ichild==(TObjectTreeItem*) sounds[i])
		{
			sounds.del(i);
			return;
		}
}

TSound* TSoundlist::addsound()
{
	IDnr++;
	TSound *snd=new TSound;
	snd->SetID(TFormatString(_qstr("ID^1"),IDnr));
	sounds.add(snd);
	return snd;
}


void TSoundlist::reset()
{
	sounds.reset();
}

void TSoundlist::updaterender()
{
	for (int i=0; i<sounds.G_count(); i++)
		sounds[i]->updaterender();
}




//*********************************************************************
// T3DTexturelist
//*********************************************************************

void T3DTexturelist::delchild(TObjectTreeItem *ichild)
{
	for (int i=0; i<textures.G_count(); i++)
		if (ichild==(TObjectTreeItem*) textures[i])
		{
			textures.del(i);
			return;
		}
}

void T3DTexturelist::clear()
{
	textures.reset();
}


void T3DTexturelist::add(T3DTexture *tx)
{
	QString str;
	tx->G_name(str);
	if (str.G_length()==0)
	{
		throw QError(_text("A texture should have a non-trivial name"));
	}
	if (idx.get(str)>=0)
	{
		QString errstr;FormatString(errstr,_text("Texture '^1' is already present"),str);
		throw QError(errstr);
	}
	textures.add(tx);
	idx.add(str,textures.G_count()-1);
}

T3DTexture* T3DTexturelist::G_texture(StrPtr name)
{
	int nr=idx.get(name);
	if (nr<0) return NULL;
	return textures[nr];
}




//*********************************************************************
// T3DMotionlist
//*********************************************************************

T3DMotionlist::T3DMotionlist()
{
	param_readonly(_qstr("Name"),true);

}


void T3DMotionlist::delchild(TObjectTreeItem *ichild)
{
	for (int i=0; i<motions.G_count(); i++)
		if (ichild==(TObjectTreeItem*) motions[i])
		{
			QString str;
			motions[i]->G_name(str);
			index.add(str,-1);
			motions.del(i);
			return;
		}
}

void T3DMotionlist::reset()
{
	index.reset();
	motions.reset();
}

void T3DMotionlist::createdefaults()
{
/*	T3DMotionFree *mt=new T3DMotionFree;
	mt->Set_name(MOTIONNAME_DEFAULT);
	add(mt);*/
}


T3DMotionlist::~T3DMotionlist()
{
	motions.reset();
}


int T3DMotionlist::G_childcount()
{ 
	return motions.G_count();
}

TObjectTreeItem* T3DMotionlist::G_child(int nr)
{
	return motions[nr];
}

int T3DMotionlist::G_count()
{
	return motions.G_count();
}

T3DMotion* T3DMotionlist::G_motion(int nr)
{
	return motions[nr];
}

T3DMotion* T3DMotionlist::G_motion(StrPtr name)
{
	int nr=index.get(name);
	if ((nr<0)||(nr>=motions.G_count())) return NULL;
	return motions[nr];
}



void T3DMotionlist::add(T3DMotion *imotion)
{
	QString str;
	imotion->G_name(str);
	if (index.get(str)>=0) throw QError(_text("A motion objects with the same name already exists"));
	motions.add(imotion);
	index.add(str,motions.G_count()-1);
}



//*********************************************************************
// T3DCosmos
//*********************************************************************


T3DCosmos::T3DCosmos()
{
	addlog(_text("Constructing 3DC"),+1);

	TObjectTreeItem::expanded=true;
	param_readonly(ObjNameStr,true);

	curtime=G_valuecontent<TSC_time>(&addparam(_qstr("Time"),SC_valname_time)->content);//param_readonly(_qstr("Time"),true);
	timespeedfactor=addparam(_qstr("TimeSpeedFactor"),SC_valname_scalar)->content.G_content_scalar();
	pauzed=addparam(_qstr("Pauzed"),SC_valname_boolean)->content.G_content_boolean();

	showcontrols=addparam(_qstr("ShowControls"),SC_valname_boolean)->content.G_content_boolean();showcontrols->copyfrom(true);

	mousedampingfactor=addparam(_qstr("MouseDampingFactor"),SC_valname_scalar)->content.G_content_scalar();//should be 0<=...<1
	mousedampingfactor->copyfrom(0.85);

//	time->Set_year(1990);

	rendertype=&addparam(_qstr("Rendertype"),SC_valname_rendertype)->content;
//	G_param(_qstr("Rendertype"))->comment=_text("//0=mono // 1=stereo with 1 RC and multiple viewport // 2=stereo with 2 RCs and 2 windows, single threaded  // 3=two RC's and multithreaded");


	vsynccount=addparam(_qstr("VSync"),SC_valname_scalar)->content.G_content_scalar();
	requestedframerate=addparam(_qstr("FrameRate"),SC_valname_scalar)->content.G_content_scalar();

	colormatrixleft=G_valuecontent<TMatrix>(&addparam(_qstr("ColorMatrixLeft"),SC_valname_matrix)->content);
	colormatrixleft->makeunit(3);
	colormatrixright=G_valuecontent<TMatrix>(&addparam(_qstr("ColorMatrixRight"),SC_valname_matrix)->content);
	colormatrixright->makeunit(3);


	objectcatalog.add(new T3DSubFrameObject(NULL));
	objectcatalog.add(new T3DObjectPoint(NULL));
	objectcatalog.add(new T3DObjectBar(NULL));
	objectcatalog.add(new T3DObjectSphere(NULL));
	objectcatalog.add(new T3DObjectCylinder(NULL));
	objectcatalog.add(new T3DObjectPie(NULL));
	objectcatalog.add(new T3DObjectRectangle(NULL));
	objectcatalog.add(new T3DObjectSolidArrow(NULL));

	objectcatalog.add(new T3DObjectMesh(NULL));

	objectcatalog.add(new T3DObjectSurface(NULL));
	objectcatalog.add(new T3DObjectCurve(NULL));
	objectcatalog.add(new T3DObjectPEngine(NULL));
	objectcatalog.add(new T3DObject3DText(NULL));
	objectcatalog.add(new T3DObjectFlatText(NULL));
	objectcatalog.add(new T3DObject3DFormattedText(NULL));
	objectcatalog.add(new T3DObjectLightPoint(NULL));
	objectcatalog.add(new T3DRepeatedObject(NULL));
	objectcatalog.add(new T3DObjectFog(NULL));

	objectcatalog.add(new T3DObjectStarGlobe(NULL));
	objectcatalog.add(new T3DObjectStarMap(NULL));
	objectcatalog.add(new T3DObjectClock(NULL));


	objectcatalog.add(new T3DControl_Frame(NULL,false));
	objectcatalog.add(new T3DControl_Text(NULL,false));
	objectcatalog.add(new T3DControl_Scalar(NULL,false));
	objectcatalog.add(new T3DControl_Edit(NULL,false));
	objectcatalog.add(new T3DControl_List(NULL,false));
	objectcatalog.add(new T3DControl_Check(NULL,false));
	objectcatalog.add(new T3DControl_Button(NULL,false));
	objectcatalog.add(new T3DControl_Menu(NULL,false));

	timingtest_lasttick=0;timingtest_count=0;timingtest_framerate=0;
	lasttimespeedcounter=0;
	lastrendercounter=-1;

	LARGE_INTEGER ifreq;
	QueryPerformanceFrequency(&ifreq);
	rendertimerfreq=(double)ifreq.QuadPart;
	lastusernavigationcounter=0;

	defaultmotion=new T3DMotionFree;

	initvalues();

	initkeyboardcodes();

	disptype=0;
	showinfoline=false;
	showdepthlayers=false;
	showstereogrid=false;
	UseJoystickInput = true;

	prev_mouseshift_x=0;prev_mouseshift_y=0;prev_mouseshift_z=0;


	addlog(_text("3D Constructed"),-1);

}


void Set_JoystickUseForNavigation(bool vl) {
	T3DCosmos::Get().UseJoystickInput = vl;
}


void T3DCosmos::init()
{
	readstarinfo();
}

void T3DCosmos::readstarinfo()
{
	//read stars

	stars_MAG.reset();
	stars_X.reset();
	stars_Y.reset();
	stars_Z.reset();
	dir1_X.reset();dir1_Y.reset();dir1_Z.reset();
	dir2_X.reset();dir2_Y.reset();dir2_Z.reset();

	QString line,part;
	double ra,dec,mag,e1,e2;
	QString filename=G_datadir();
	filename+=_qstr("\\AstroData\\stars1.txt");
	QTextfile fp;
	fp.openread(filename);
	fp.readline(line);
	while (!fp.isend())
	{
		fp.readline(line);
		line.splitstring(_qstr("\t"),part);ra=qstr2double(part);
		line.splitstring(_qstr("\t"),part);dec=qstr2double(part);
		line.splitstring(_qstr("\t"),part);mag=qstr2double(part);
		ra=ra*deg2rad;
		dec=dec*deg2rad;
		stars_MAG.add(mag);
		equat2eclipt(ra,dec,e1,e2);
		stars_RA.add(ra);stars_DEC.add(dec);
		stars_E1.add(e1);stars_E2.add(e2);
		stars_X.add(cos(e1)*cos(e2));
		stars_Y.add(sin(e1)*cos(e2));
		stars_Z.add(sin(e2));
	}
	fp.close();


	double d1x,d1y,d1z,r,d2x,d2y,d2z;
	for (int i=0; i<stars_X.G_count(); i++)
	{
		d1x=-stars_Y[i];d1y=stars_X[i];r=sqrt(d1x*d1x+d1y*d1y);
		d1x/=r;d1y/=r;d1z=0;
		d2x=d1y*stars_Z[i];d2y=-d1x*stars_Z[i];d2z=stars_Y[i]*d1x-stars_X[i]*d1y;
		r=sqrt(d2x*d2x+d2y*d2y+d2z*d2z);
		d2x/=r;d2y/=r;d2z/=r;
		dir1_X.add(d1x);dir1_Y.add(d1y);dir1_Z.add(d1z);
		dir2_X.add(d2x);dir2_Y.add(d2y);dir2_Z.add(d2z);
	}


	//read constellation lines
	int a1,a2,d1,d2;
	QString st1,cd;
	filename=G_datadir();
	filename+=_qstr("\\AstroData\\conlines.txt");
	fp.openread(filename);
	while (!fp.isend())
	{
		fp.readline(line);

		if (qstrlen(line)>4)
		{
			line.splitstring(_qstr(","),cd);

			line.splitstring(_qstr(","),st1);a1=qstr2int(st1);
			line.splitstring(_qstr(","),st1);d1=qstr2int(st1);
			line.splitstring(_qstr(","),st1);a2=qstr2int(st1);
			line.splitstring(_qstr(","),st1);d2=qstr2int(st1);

			ra=a1/1000.0*15*deg2rad;
			dec=d1/100.0*deg2rad;
			equat2eclipt(ra,dec,e1,e2);
			const_code.add(new QString(cd));
			const_X_1.add(cos(e1)*cos(e2));
			const_Y_1.add(sin(e1)*cos(e2));
			const_Z_1.add(sin(e2));
			const_RA_1.add(ra);const_DEC_1.add(dec);
			const_E1_1.add(e1);const_E2_1.add(e2);

			ra=a2/1000.0*15*deg2rad;
			dec=d2/100.0*deg2rad;
			equat2eclipt(ra,dec,e1,e2);
			const_X_2.add(cos(e1)*cos(e2));
			const_Y_2.add(sin(e1)*cos(e2));
			const_Z_2.add(sin(e2));
			const_RA_2.add(ra);const_DEC_2.add(dec);
			const_E1_2.add(e1);const_E2_2.add(e2);

		}
	}
	fp.close();
}

void T3DCosmos::addkeyboardcode(StrPtr iname, int ival)
{
	keyboardcodes_index.add(iname,keyboardcodes_names.G_count());
	keyboardcodes_names.add(new CString(iname));
	keyboardcodes_vals.add(ival);
}


void T3DCosmos::initkeyboardcodes()
{
	addkeyboardcode(_qstr("LEFT"),VK_LEFT);
	addkeyboardcode(_qstr("RIGHT"),VK_RIGHT);
	addkeyboardcode(_qstr("UP"),VK_UP);
	addkeyboardcode(_qstr("DOWN"),VK_DOWN);
	addkeyboardcode(_qstr("PAGE UP"),VK_PRIOR);
	addkeyboardcode(_qstr("PAGE DOWN"),VK_NEXT);
	addkeyboardcode(_qstr("INSERT"),VK_INSERT);
	addkeyboardcode(_qstr("DELETE"),VK_DELETE);
	addkeyboardcode(_qstr("HOME"),VK_HOME);
	addkeyboardcode(_qstr("END"),VK_END);
	addkeyboardcode(_qstr("CONTROL"),VK_CONTROL);
	addkeyboardcode(_qstr("SHIFT"),VK_SHIFT);
	addkeyboardcode(_qstr("SPACE"),VK_SPACE);
	addkeyboardcode(_qstr("+"),VK_ADD);
	addkeyboardcode(_qstr("-"),VK_SUBTRACT);
	addkeyboardcode(_qstr("RETURN"),VK_RETURN);
	addkeyboardcode(_qstr("ESCAPE"),VK_ESCAPE);

	addkeyboardcode(_qstr("F5"),VK_F5);
	addkeyboardcode(_qstr("F6"),VK_F6);
	addkeyboardcode(_qstr("F7"),VK_F7);
	addkeyboardcode(_qstr("F8"),VK_F8);
	addkeyboardcode(_qstr("F9"),VK_F9);
	addkeyboardcode(_qstr("F10"),VK_F10);
}


int T3DCosmos::G_keyboardcode(StrPtr iname)
{
	int idx=keyboardcodes_index.get(iname);
	if (idx<0) return -1;
	return keyboardcodes_vals[idx];
}

StrPtr T3DCosmos::G_keyboardname(int icode)
{
	for (int i=0; i<keyboardcodes_vals.G_count(); i++)
		if (keyboardcodes_vals[i]==icode) return *keyboardcodes_names[i];
	return _qstr("");
}


void T3DCosmos::initvalues()
{

	timespeedfactor->copyfrom(1.0);
	pauzed->copyfrom(false);
	requestedframerate->copyfrom(30);
	vsynccount->copyfrom(1);
	lasttimespeedcounter=0;
	lastrendercounter=-1;
	lastusernavigationcounter=0;

	for (int i=0; i<axiscount; i++)
		for (int j=0; j<axislevelcount; j++)
		{
			axisposit[i][j]=0;
			axispositraw[i][j]=0;
		}
}


void T3DCosmos::resetall()
{
	scenes.reset();
	viewportlist.reset();
	videolist.reset();
	soundlist.reset();
	controls.reset();
	lasttimespeedcounter=0;
	lastrendercounter=-1;
	lastusernavigationcounter=0;
}

void T3DCosmos::resetallscenes()
{
	scenes.reset();
	lasttimespeedcounter=0;
	lastrendercounter=-1;
	lastusernavigationcounter=0;
}

void T3DCosmos::resetallviewports()
{
	viewportlist.viewports.reset();
	lasttimespeedcounter=0;
	lastrendercounter=-1;
	lastusernavigationcounter=0;
}

void T3DCosmos::resetallvideos()
{
	videolist.reset();
}

void T3DCosmos::resetallsounds()
{
	soundlist.reset();
}

double T3DCosmos::G_elapsedrendertime()
{
	return lastrendercounter/G_rendertimerfreq();
}



void T3DCosmos::cleanup()
{
	addlog(_text("Cleaning up 3DC"),+1);
	objectcatalog.reset();
	resetall();
	viewportlist.delallparams();
	viewportlist.reset();
	videolist.delallparams();
	videolist.reset();
	soundlist.delallparams();
	soundlist.reset();
	delallparams();
	controls.reset();
	if (defaultmotion!=NULL) delete defaultmotion;defaultmotion=NULL;
	addlog(_text("3DC cleaned"),-1);
}

T3DCosmos::~T3DCosmos()
{
	addlog(_text("Destroying 3DC"),+1);
	cleanup();
	addlog(_text("3DC destroyed"),-1);
}



T3DObject* T3DCosmos::G_objectcatalog_object(StrPtr tpename)
{
	for (int i=0; i<objectcatalog.G_count(); i++)
		if (qstricmp(tpename,objectcatalog[i]->G_classname())==0)
			return objectcatalog[i];
	return NULL;
}


int T3DCosmos::G_childcount()
{
	return scenes.G_count()+1+1+1+1;
}
TObjectTreeItem* T3DCosmos::G_child(int nr)
{
	if (nr<scenes.G_count()) return scenes[nr];
	nr-=scenes.G_count();
	if (nr<1) return &viewportlist;
	nr-=1;
	if (nr<1) return G_treeobject_adapterlist();
	nr-=1;
	if (nr<1) return &videolist;
	nr-=1;
	if (nr<1) return &soundlist;
	nr-=1;

	ASSERT(false);
	return NULL;
}

void T3DCosmos::delchild(TObjectTreeItem *ichild)
{
	for (int i=0; i<scenes.G_count(); i++)
	{
		if (scenes[i]==(T3DScene*)ichild)
		{
			delscene(i);
			return;
		}
	}
}


T3DObject* T3DCosmos::findobject(TObjectTreeItem *treeitem)
{
	for (int i=0; i<scenes.G_count(); i++)
	{
		T3DObject *obj=scenes[i]->findobject(treeitem);
		if (obj!=NULL) return obj;
	}
	return NULL;
}

T3DScene* T3DCosmos::findscene(TObjectTreeItem *treeitem)
{
	for (int i=0; i<scenes.G_count(); i++)
		if ((TObjectTreeItem*)scenes[i]==treeitem)
			return scenes[i];
	return NULL;
}


T3DScene* T3DCosmos::addscene()
{
	T3DScene *scene=new T3DScene;
	scenes.add(scene);
	return scene;
}

void T3DCosmos::delscene(int scenenr)
{
	scenes.del(scenenr);
}


T3DScene* T3DCosmos::G_scene(StrPtr name)
{
	QString str;
	for (int i=0; i<scenes.G_count(); i++)
	{
		scenes[i]->G_name(str);
		if (qstricmp(name,str)==0) return scenes[i];
	}
	return NULL;
}


void T3DCosmos::paramchanged(StrPtr iname)
{
	if (issame(iname,_qstr("time")))
		adjusttimes(-1);
}


void T3DCosmos::UIaction_axis(int axisnr, int axislevel, double val)//this function is called if a user performs an action
{
	try{

		if ((!IsAnimationRunning())&&(G_selviewport()!=NULL))//keyboard control dispatching if no animation is running
		{
			double rotunit=0.02;

			if (axisnr==UIA_Y)
				int jj=0;

			if (axislevel==0)
			{
				if (axisnr==UIA_X)  { G_selviewport()->camera_rotatehor(-rotunit*val); render(); }
				if (axisnr==UIA_Y)  { G_selviewport()->camera_rotatevert(+rotunit*val); render(); }
				if (axisnr==UIA_Z)  { G_selviewport()->camera_move(val); render(); }
			}

			if (axislevel==1)
			{
				if (axisnr==UIA_X)  { G_selviewport()->scene_rotatehor(+rotunit*val); render(); }
				if (axisnr==UIA_Y)  { G_selviewport()->scene_rotatevert(-rotunit*val); render(); }
				if (axisnr==UIA_Z)  { G_selviewport()->scene_move(val); render(); }
			}
		}
	}
	catch(QError &err)
	{
		reporterror(err.G_content());
	}
}


Tjoystickinfo& T3DCosmos::G_joystickinfo(int ID)
{
	if ((ID<0)||(ID>=MAX_JOYSTICKCOUNT)) throw QError(_text("Invalid joystick ID"));
	return joysticks[ID];
}


int GetAxisLevelKBModifier()
{
	int kblevel=2;
	if (IsKeyDown(VK_CONTROL)) kblevel=1;
	if (IsKeyDown(VK_SHIFT)) kblevel=0;
	return kblevel;
}



void T3DCosmos::updateaxisposit()
{
	//++++

	double stepfactor=1;
	LARGE_INTEGER li;
	__int64 currentusernavigationcounter;
	QueryPerformanceCounter(&li);currentusernavigationcounter=li.QuadPart;
	if (lastusernavigationcounter!=0)
		stepfactor=30*(currentusernavigationcounter-lastusernavigationcounter)/T3DCosmos::Get().G_rendertimerfreq();
	lastusernavigationcounter=currentusernavigationcounter;

	for (int i=0; i<axiscount; i++)
		for (int j=0; j<axislevelcount; j++)
		{
			axisposit[i][j]=0;
			axispositraw[i][j]=0;
		}

	int kblevel=GetAxisLevelKBModifier();

	//from mouse
	Tcopyarray<TDisplayWindow> *dispwins=G_displaywindows();
	for (int i=0; i<dispwins->G_count(); i++)
	{
		TRenderWindow *rw=dispwins->get(i)->renderwindow;
		if (rw!=NULL)
		{
			double df1=mousedampingfactor->G_val();
			double df2=1-df1;
			prev_mouseshift_x=df1*prev_mouseshift_x+df2*rw->G_mouseshiftx();if (fabs(prev_mouseshift_x)<0.02) prev_mouseshift_x=0;
			prev_mouseshift_y=df1*prev_mouseshift_y+df2*rw->G_mouseshifty();if (fabs(prev_mouseshift_y)<0.02) prev_mouseshift_y=0;
			prev_mouseshift_z=df1*prev_mouseshift_z+df2*rw->G_mouseshiftz();if (fabs(prev_mouseshift_z)<0.02) prev_mouseshift_z=0;
			axisposit[UIA_X][kblevel]=+prev_mouseshift_x/7.0;
			axisposit[UIA_Y][kblevel]=-prev_mouseshift_y/7.0;
			axisposit[UIA_Z][kblevel]=+prev_mouseshift_z/15.0;
			axispositraw[UIA_X][kblevel]=+rw->G_mouseshiftx()/7.0;
			axispositraw[UIA_Y][kblevel]=-rw->G_mouseshifty()/7.0;
			axispositraw[UIA_Z][kblevel]=+rw->G_mouseshiftz()/15.0;
			rw->resetmouseshift();
		}
	}

	//from keyboard
	if (IsKeyDown(VK_LEFT )) axisposit[UIA_X][kblevel]=-1*stepfactor;
	if (IsKeyDown(VK_RIGHT)) axisposit[UIA_X][kblevel]=+1*stepfactor;
	if (IsKeyDown(VK_DOWN )) axisposit[UIA_Y][kblevel]=-1*stepfactor;
	if (IsKeyDown(VK_UP   )) axisposit[UIA_Y][kblevel]=+1*stepfactor;
	if (IsKeyDown(VK_PRIOR)) axisposit[UIA_Z][kblevel]=+1*stepfactor;
	if (IsKeyDown(VK_NEXT )) axisposit[UIA_Z][kblevel]=-1*stepfactor;

	//from joystick
	if (UseJoystickInput) {//J!!!
		double joyspeedfactor=1.5;
		int jlevel=0;
		if (joysticks[0].G_buttondown(joysticks[0].G_button_control())) jlevel=1;
		if (joysticks[0].G_buttondown(joysticks[0].G_button_shift())) jlevel=2;
		axisposit[UIA_X][jlevel]+=joyspeedfactor*joysticks[0].G_xpcorr()*stepfactor;
		axisposit[UIA_Y][jlevel]+=joyspeedfactor*joysticks[0].G_ypcorr()*stepfactor;
		axisposit[UIA_Z][jlevel]+=joyspeedfactor*joysticks[0].G_zpcorr()*stepfactor;
		axisposit[UIA_R][jlevel]+=joyspeedfactor*joysticks[0].G_rpcorr()*stepfactor;
		axisposit[UIA_U][jlevel]+=joyspeedfactor*joysticks[0].G_upcorr()*stepfactor;
		axisposit[UIA_V][jlevel]+=joyspeedfactor*joysticks[0].G_vpcorr()*stepfactor;
	}
}

bool T3DCosmos::G_axisactive(int axisnr, int axislevel)
{
	if ((axisnr<0)||(axisnr>=axiscount)||(axislevel<0)||(axislevel>=axislevelcount))
	{
		ASSERT(false);
		return false;
	}
	return fabs(axisposit[axisnr][axislevel])>1.0e-6;
}

double T3DCosmos::G_axisposit(int axisnr, int axislevel)
{
	if ((axisnr<0)||(axisnr>=axiscount)||(axislevel<0)||(axislevel>=axislevelcount))
	{
		ASSERT(false);
		return 0;
	}
	return axisposit[axisnr][axislevel];
}

bool T3DCosmos::G_axisrawactive(int axisnr, int axislevel)
{
	if ((axisnr<0)||(axisnr>=axiscount)||(axislevel<0)||(axislevel>=axislevelcount))
	{
		ASSERT(false);
		return false;
	}
	return fabs(axispositraw[axisnr][axislevel])>1.0e-6;
}

double T3DCosmos::G_axisrawposit(int axisnr, int axislevel)
{
	if ((axisnr<0)||(axisnr>=axiscount)||(axislevel<0)||(axislevel>=axislevelcount))
	{
		ASSERT(false);
		return 0;
	}
	return axispositraw[axisnr][axislevel];
}




void T3DCosmos::UI_OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool shiftpressed=((GetKeyState(VK_SHIFT)>>1)!=0);
	bool controlpressed=((GetKeyState(VK_CONTROL)>>1)!=0);

	UI_processkey(nChar,controlpressed,shiftpressed);
}

void T3DCosmos::UI_processkey(UINT nChar, bool controlpressed, bool shiftpressed)
{
	double rotunit=0.02;

	if (nChar==13) nChar=VK_RETURN;


	int kblevel=GetAxisLevelKBModifier();

	if (nChar==VK_LEFT) UIaction_axis(UIA_X,kblevel,-1);
	if (nChar==VK_RIGHT) UIaction_axis(UIA_X,kblevel,+1);
	if (nChar==VK_UP) UIaction_axis(UIA_Y,kblevel,+1);
	if (nChar==VK_DOWN) UIaction_axis(UIA_Y,kblevel,-1);
	if (nChar==VK_PRIOR) UIaction_axis(UIA_Y,kblevel,-1);
	if (nChar==VK_NEXT) UIaction_axis(UIA_Y,kblevel,+1);

	if ((nChar==VK_SPACE)&&(!controlpressed)&&(!shiftpressed)) { pauze(!pauzed->G_val()); }

	T3DViewport *viewport=G_selviewport();
	if (viewport!=NULL)
	{
		if (viewport->G_enableuserstop())
			if (nChar==VK_ESCAPE) Set_requeststop();

		if ((nChar==VK_F1)&&(!shiftpressed)&&(!controlpressed)) viewport->switchcursoractive();
		if ((nChar==VK_F2)&&(!shiftpressed)&&(!controlpressed)) showcontrols->copyfrom(!showcontrols->G_val());
		if (viewport->G_enableusertimecontrol())
		{
			if ((nChar==VK_F3)&&(!shiftpressed)&&(!controlpressed)) pauze(!G_pauzed());
			if ((nChar==VK_F3)&&(shiftpressed)&&(!controlpressed)) timespeedfactor->copyfrom(-timespeedfactor->G_val());
		}
	}


	if (G_showcontrols())
	{
		T3DControl *actcontrol=G_activecontrol();
		T3DControl *defcontrol=G_defaultcontrol();
		bool processed=false;
		if (actcontrol!=NULL)
		{
			if (!processed) processed=actcontrol->UI_OnKeyDown(nChar,shiftpressed,controlpressed);
			if ((!processed)&&(nChar==VK_UP  )&&(!shiftpressed)&&(!controlpressed)) processed=actcontrol->UI_OnPressed_Up();
			if ((!processed)&&(nChar==VK_DOWN)&&(!shiftpressed)&&(!controlpressed)) processed=actcontrol->UI_OnPressed_Down();
			if ((!processed)&&(nChar==VK_RIGHT)&&(!shiftpressed)&&(!controlpressed)) controls_selectnext(+1);
			if ((!processed)&&(nChar==VK_LEFT)&&(!shiftpressed)&&(!controlpressed)) controls_selectnext(-1);
			if ((!processed)&&(nChar==VK_TAB)&&(!shiftpressed)&&(!controlpressed)) controls_selectnext(+1);
			if ((!processed)&&(nChar==VK_TAB)&&(shiftpressed)&&(!controlpressed)) controls_selectnext(-1);
		}
		if ((defcontrol!=NULL)&&(defcontrol!=actcontrol))
		{
			if (!processed) processed=defcontrol->UI_OnKeyDown(nChar,shiftpressed,controlpressed);
		}
	}

	StrPtr keyname=G_keyboardname(nChar);
	if (qstrlen(keyname)>0) lastkeypressed=keyname;
}

void T3DCosmos::UI_OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool shiftpressed=((GetKeyState(VK_SHIFT)>>1)!=0);
	bool controlpressed=((GetKeyState(VK_CONTROL)>>1)!=0);

	if ((nChar=='Z'-'A'+1)&&(!shiftpressed)) Set_requeststop();//ctrl+Z

	if ((nChar=='S'-'A'+1)&&(!shiftpressed)) cmd_switchstereo();//ctrl+S

	if ((nChar=='S'-'A'+1)&&(shiftpressed)) disptype=(disptype+1)%3;//ctrl+shift+S

	if ((nChar=='L'-'A'+1)&&(!shiftpressed))//ctrl+L
	{
		showdepthlayers=!showdepthlayers;
		if (!IsAnimationRunning()) render();
	}
	if ((nChar=='G'-'A'+1)&&(!shiftpressed))//ctrl+G
	{
		showstereogrid=!showstereogrid;
		if (!IsAnimationRunning()) render();
	}
	if ((nChar=='I'-'A'+1)&&(!shiftpressed)&&controlpressed)//ctrl+I
	{
		showinfoline=!showinfoline;
		if (!IsAnimationRunning()) render();
	}
	if ((nChar=='P'-'A'+1)&&(!shiftpressed)) cmd_copycamerapos();//ctrl+P
	if ((nChar=='P'-'A'+1)&&(shiftpressed)) cmd_copycursorpos();//ctrl+shift+P

	if ((nChar=='E'-'A'+1)&&(!shiftpressed)) cmd_exportscreenimage();//ctrl+E

	if (G_showcontrols())
	{
		T3DControl *control=G_activecontrol();
		if (control!=NULL)
		{
			bool processed=false;
			if (!processed) processed=control->UI_OnChar(nChar,shiftpressed,controlpressed);
		}
	}

	if (nChar!=13)
	{
		CString charstring;
		charstring+=(char)nChar;
		lastkeypressed=(StrPtr)charstring;
	}
}

void T3DCosmos::UI_OnMouseArrowClick(TMouseClickInfo info)
{
	for (int i=0; i<scenes.G_count(); i++)
		scenes[i]->MouseArrowClick(info);
}

void T3DCosmos::cmd_switchstereo()
{
	for (int i=0; i<G_viewportcount(); i++)
		G_viewport(i)->switchstereo();
}

void T3DCosmos::cmd_copycamerapos()
{
	QString st,st1;
	if (G_viewportcount()<=0) return;
	G_viewport(0)->G_camerapos()->tostring(st,0);
	G_viewport(0)->G_cameradir()->tostring(st1,0);st+=_qstr(";");st+=st1;
	G_viewport(0)->G_cameraupdir()->tostring(st1,0);st+=_qstr(";");st+=st1;
	copyclipboard(st);
}

void T3DCosmos::cmd_exportscreenimage()
{
	if (G_viewportcount()<=0) return;
	Tbmp bmp;
	G_viewport(0)->capture(&bmp);
	bmp.savejpg(TFormatString(_qstr("^1\\screendump.jpg"),G_datadir()),95);
}

void T3DCosmos::cmd_copycursorpos()
{

	QString st;
	if (G_scenecount()<=0) return;
	if (!G_scene(0)->G_cursoractive()) return;
	G_scene(0)->G_cursorpos().tostring(st,0);
	copyclipboard(st);
}




void T3DCosmos::waitforframerate()
{
	LARGE_INTEGER li;
	__int64 currendercounter;

	if (lastrendercounter!=-1)
	{
		double frameinterval=1000.0/G_requestedframerate();
		do {
			QueryPerformanceCounter(&li);currendercounter=li.QuadPart;
		} while (currendercounter-lastrendercounter<frameinterval/1000.0*rendertimerfreq);
	}
	else
	{
		QueryPerformanceCounter(&li);currendercounter=li.QuadPart;
	}
	lastrendercounter=currendercounter;
}

void T3DCosmos::Reset_requeststop()
{ 
	requeststop=false; 
}
void T3DCosmos::Set_requeststop()
{
	requeststop=true;
}
bool T3DCosmos::G_requeststop()
{ 
	return requeststop; 
}


void T3DCosmos::releaseallcontexts()
{
	Tcopyarray<TDisplayWindow> *dispwins=G_displaywindows();
	for (int i=0; i<dispwins->G_count(); i++)
	{
		TRenderWindow *rw=dispwins->get(i)->renderwindow;
		if (rw!=NULL) rw->ReleaseRC();
	}

	for (int i=0; i<viewportlist.viewports.G_count(); i++)
	{
		if (viewportlist.viewports[i]->G_leftwin()!=NULL) viewportlist.viewports[i]->G_leftwin()->ReleaseRC();
		if (viewportlist.viewports[i]->G_rightwin()!=NULL) viewportlist.viewports[i]->G_rightwin()->ReleaseRC();
	}
}

void T3DCosmos::modify_timespeed(double ifactor)
{
	timespeedfactor->copyfrom(ifactor*timespeedfactor->G_val());
}


void T3DCosmos::pauze(bool ipauzed)
{
	pauzed->copyfrom(ipauzed);
}

bool T3DCosmos::G_pauzed()
{
	return pauzed->G_val();
}


void T3DCosmos::adjusttimes(double timestep)
{

	for (int motionnr=0; motionnr<motioncatalog.G_count(); motionnr++)
		motioncatalog[motionnr]->adjusttime(timestep);

	for (int scenenr=0; scenenr<scenes.G_count(); scenenr++)
		scenes[scenenr]->adjusttime(timestep);
}


double T3DCosmos::incrtime()
{

	double realtimestep=0;
	LARGE_INTEGER li;
	__int64 currenttimespeedcounter;
	QueryPerformanceCounter(&li);currenttimespeedcounter=li.QuadPart;
	if ((lasttimespeedcounter!=0)&&(currenttimespeedcounter>lasttimespeedcounter))
		realtimestep=(currenttimespeedcounter-lasttimespeedcounter)/G_rendertimerfreq();
	lasttimespeedcounter=currenttimespeedcounter;
	if (realtimestep<0) realtimestep=0;
	if (realtimestep>0.2) realtimestep=0.2;

	if (!pauzed->G_val())
	{
		double timefactor=timespeedfactor->G_val();
		double timestep=realtimestep*timefactor;
		curtime->addseconds(timestep);
		curtime->calcsiderial();
		adjusttimes(timestep);
		return timestep;
	}
	return 0;
}

void T3DCosmos::motioncatalog_add(T3DMotion *mt)
{
	motioncatalog.add(mt);
}

void T3DCosmos::motioncatalog_remove(T3DMotion *mt)
{
	for (int i=0; i<motioncatalog.G_count(); i++)
		if (motioncatalog[i]==mt)
		{
			motioncatalog.del(i);
			return;
		}
}

void T3DCosmos::controls_add(T3DControl *icontrol)
{
	controls.add(icontrol);
	//if (icontrol->G_canselect())
	activecontrol=controls.G_count()-1;
}

void T3DCosmos::controls_del(T3DControl *icontrol)
{
	for (int i=0; i<controls.G_count(); i++)
		if (controls[i]==icontrol)
			controls.del(i);
	if (controls.G_count()>=activecontrol) activecontrol=controls.G_count()-1;
	if (activecontrol<0) activecontrol=0;
}

void T3DCosmos::controls_selectnext(int dir)
{
	for (int ct=0; ct<=controls.G_count(); ct++)
	{
		activecontrol=(controls.G_count()+activecontrol+dir)%controls.G_count();
		if ((controls[activecontrol]->G_canselect())&&(controls[activecontrol]->G_isvisible())) return;
	}
}


void T3DCosmos::controls_activate(T3DControl *ctrl)
{
	bool found=false;
	for (int ct=0; ct<=controls.G_count(); ct++)
		if (controls[ct]==ctrl)
		{
			activecontrol=ct;
			found=true;
		}
	if (!found) throw QError(_text("Activate control: Unable to find control"));
}


T3DControl* T3DCosmos::G_activecontrol()
{
	return controls[activecontrol];
}

T3DControl* T3DCosmos::G_defaultcontrol()
{
	for (int i=0; i<controls.G_count(); i++)
		if (controls[i]->IsDefault())
		return controls[i];
	return false;
}


void T3DCosmos::dispatchjoysticks()
{
	//update joystick info
	for (int joysticknr=0; joysticknr<MAX_JOYSTICKCOUNT; joysticknr++)
		joysticks[joysticknr].read(joysticknr);

	if ((G_selviewport()!=NULL)&&(G_selviewport()->G_enableusernavigation()))
	{
		bool controlpressed=joysticks[0].G_buttondown(joysticks[0].G_button_control());
		bool shiftpressed=joysticks[0].G_buttondown(joysticks[0].G_button_shift());

		if (UseJoystickInput) {//J!!!
			if (joysticks[0].G_buttonclicked(joysticks[0].G_button_F1())) UI_processkey(VK_F1,controlpressed,shiftpressed);
			if (joysticks[0].G_buttonclicked(joysticks[0].G_button_F2())) UI_processkey(VK_F2,controlpressed,shiftpressed);
			if (joysticks[0].G_buttonclicked(joysticks[0].G_button_F3())) UI_processkey(VK_F3,controlpressed,shiftpressed);
			if (joysticks[0].G_buttonclicked(joysticks[0].G_button_F4())) UI_processkey(VK_F4,controlpressed,shiftpressed);

			if (joysticks[0].G_buttonclicked(joysticks[0].G_button_return())) UI_processkey(VK_RETURN,controlpressed,shiftpressed);
			if (joysticks[0].G_buttonclicked(joysticks[0].G_button_esc())) UI_processkey(VK_ESCAPE,controlpressed,shiftpressed);

			int rockerclicked=joysticks[0].G_rockerclicked();
			if (rockerclicked==1) UI_processkey(VK_UP,false,false);
			if (rockerclicked==2) UI_processkey(VK_RIGHT,false,false);
			if (rockerclicked==3) UI_processkey(VK_DOWN,false,false);
			if (rockerclicked==4) UI_processkey(VK_LEFT,false,false);
		}
	}

}




void T3DCosmos::render(bool dryrun)
{
	DWORD curtick=GetTickCount();

	if (curtick>timingtest_lasttick+2000)
	{
		timingtest_framerate=timingtest_count*1000.0/(curtick-timingtest_lasttick);
		timingtest_lasttick=curtick;
		timingtest_count=0;
	}

	timingtest_count++;

	lastkeypressed=_qstr("");
	for (int i=0; i<controls.G_count(); i++) controls[i]->Unset_hasmodified();

	dispatchjoysticks();

	updateaxisposit();

	videolist.loadcurframes();

	soundlist.updaterender();


	if (IsAnimationRunning())
	{
		for (int vnr=0; vnr<G_viewportcount(); vnr++)
			G_viewport(vnr)->dispatchusernavigation();
	}

	if ((!dryrun)&&(scenes.G_count()>0))
	{
		if (G_rendertype()==RenderSingle)//render to single RC and window with multiple viewports
			G_globalrenderwindow()->render_clear(scenes[0]->G_backcolor());

		for (int scenenr=0; scenenr<scenes.G_count(); scenenr++)
			scenes[scenenr]->renderall();

		if (G_rendertype()==RenderSingle)//render to single RC and window with multiple viewports
		{
			waitforframerate();
			G_globalrenderwindow()->swapbuffers();
		}
	}



	for (int vnr=0; vnr<G_viewportcount(); vnr++)
		G_viewport(vnr)->videocapture_addframe();

}



T3DCosmos& G_3DCosmos()
{
	return T3DCosmos::Get();
}

void T3DCosmos::LoadSettings()
{
	try{
		addlog(_text("Loading settings"),1);
		settings_id.reset();
		settings_content.reset();
		QTextfile fl;
		QString line,ID;
		fl.openread(TFormatString(_qstr("^1\\settings.txt"),G_datadir()));
		while (!fl.isend())
		{
			fl.readline(line);
			if (line.G_length()>2)
			{
				line.splitstring(_qstr("="),ID);
				settings_id.add(new QString(ID));
				settings_content.add(new QString(line));
				settings_index.add(ID,settings_id.G_count()-1);
				addlog(TFormatString(_qstr("^1:^2"),ID,line));
			}
		}
		fl.close();
	}
	catch(QError &err)
	{
		reporterror(err);
	}
	addlog(_text("Settings loaded"),-1);
}


bool T3DCosmos::ReadSetting(StrPtr ID, QString &content)
{
	int nr=settings_index.get(ID);
	if (nr<0) return false;
	content=*settings_content[nr];
	return true;
}

void T3DCosmos::WriteSetting(StrPtr ID, StrPtr content)
{
	int nr=settings_index.get(ID);
	if (nr<0)
	{
		settings_id.add(new QString(ID));
		settings_content.add(new QString(content));
		settings_index.add(ID,settings_id.G_count()-1);
	}
	else
	{
		*settings_content[nr]=content;
	}
	try{
		QTextfile fl;
		fl.openwrite(TFormatString(_qstr("^1\\settings.txt"),G_datadir()));
		for (int i=0; i<settings_id.G_count(); i++)
		{
			fl.write(TFormatString(_qstr("^1=^2"),*settings_id[i],*settings_content[i]));
			fl.writeline();
		}
		fl.close();
	}
	catch(QError &err)
	{
		reporterror(err);
	}
}


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectroot,root)
{
	setreturntype(SC_valname_object);
	setclasspath(CLASSNAME_OBJTREE);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
//	TObjectTreeItem *obj=G_valuecontent<TObjectTreeItem>(retval);
	retval->encapsulate(&T3DCosmos::Get());
}
ENDFUNCTION(func_objectroot)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_delobject,DelObject)
{
	setclasspath(CLASSNAME_OBJTREE);
	addvar(_qstr("object"),SC_valname_any);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	arglist->get(0)->dispose();
}
ENDFUNCTION(func_delobject)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_saveobject,SaveObject)
{
	setclasspath(CLASSNAME_OBJTREE);
	addvar(_qstr("object"),SC_valname_any);
	addvar(_qstr("filename"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_value *obj=arglist->get(0);
	QString filename=arglist->get(1)->G_content_string()->G_string();
	
	TSC_datatype *datatype=obj->G_datatype();
	if (datatype==NULL) throw QError(_text("Empty value"));

	QBinTagFileWriter writer;
	QBinTagWriter &tagwriter=writer.open(filename).G_obj();
	tagwriter.write_shortstring(datatype->G_name());
	obj->streamout(tagwriter);
}
ENDFUNCTION(func_saveobject)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_readobject,ReadObject)
{
	setclasspath(CLASSNAME_OBJTREE);
	addvar(_qstr("object"),SC_valname_any);
	addvar(_qstr("filename"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_value *obj=arglist->get(0);
	QString filename=arglist->get(1)->G_content_string()->G_string();
	QString tpname;

	TSC_datatype *datatype=obj->G_datatype();
	if (datatype==NULL) throw QError(_text("Empty value"));

	QBinTagFileReader reader;
	QBinTagReader &tagreader=reader.open(filename).G_obj();
	tagreader.read_shortstring(tpname);
	if (!issame(tpname,datatype->G_name()))
		throw QError(TFormatString(_text("Incompatible data types for reading: ^1 vs. ^2"),tpname,datatype->G_name()));
	obj->streamin(tagreader);
}
ENDFUNCTION(func_readobject)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectget,get)
{
	setreturntype(SC_valname_any);
	setmemberfunction(SC_valname_object);
	addvar(_qstr("name"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TObjectTreeItem *obj=G_valuecontent<TObjectTreeItem>(owner);
	StrPtr classname=obj->G_classname();
	StrPtr name=arglist->get(0)->G_content_string()->G_string();
	TObjectTreeItem *obj2=obj->G_childbyname(name);
	if (obj2==NULL)
		throw QError(_text("Invalid object member"));
	retval->settype(obj2->G_datatype());
	retval->encapsulate(obj2);
}
ENDFUNCTION(func_objectget)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectgetsubitems,getmembers)
{
	setreturntype(SC_valname_list);
	setmemberfunction(SC_valname_object);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TObjectTreeItem *obj=G_valuecontent<TObjectTreeItem>(owner);
	TSC_list *list=G_valuecontent<TSC_list>(retval);
	list->reset();
	for (int i=0; i<obj->G_childcount(); i++)
	{
		TSC_value vl;
		vl.settype(obj->G_child(i)->G_datatype());
		vl.encapsulate(obj->G_child(i));
		list->add(&vl);
	}
}
ENDFUNCTION(func_objectgetsubitems)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_objectdelete,dispose)
{
	setmemberfunction(SC_valname_object);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TObjectTreeItem *obj=G_valuecontent<TObjectTreeItem>(owner);
	obj->dispose();
}
ENDFUNCTION(func_objectdelete)

