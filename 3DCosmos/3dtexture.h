#pragma once
#include "tools.h"
#include "qstring.h"
#include "objecttree.h"

#include "SC_value.h"


class T3DScene;
class T3DSubFrameObject;
class Trendercontext;
class T3DVideo;

class T3DTexture : public TObjectTreeItem
{
private:
	T3DSubFrameObject *subframe;
	TSC_string *filename_bitmap,*filename_bitmapalpha;
	TSpecificObjectRef<T3DVideo> attachedvideo;
private:
	Trendertexture texture;
public:
	static StrPtr GetClassName() { return SC_valname_texture; }
	T3DTexture();
	~T3DTexture();
	void Set_name(StrPtr iname);
	void Set_subframe(T3DSubFrameObject *isubframe);
	virtual StrPtr G_classname() { return _qstr("Texture"); }
public:
	void paramchanged(StrPtr iname);
	virtual TObjectTreeItem *G_parent();
	void clear();
	void loadbitmap(Tbmp *bmp, Tbmp *bmpalpha=NULL);
	void loadbitmapfile(StrPtr ifilename, StrPtr bitmapalphafile=NULL);
	void loadbitmapfile();
	void attachvideo(T3DVideo *ivideo);
	void select(Trendercontext *rc);
	void unselect(Trendercontext *rc);
	double G_aspectratio();

	virtual void streamout_content(QBinTagWriter &writer);
	virtual void streamin_content(QBinTagReader &reader);

};
