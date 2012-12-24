#include "stdafx.h"

#include "vecmath.h"
#include "vec_transformation.h"

#include "sc_script.h"

#include "3DScene.h"
#include "3Dsubframe.h"

#include "3DMeshObject.h"

#include "l3ds.h"

#include "opengl.h"

class TMaterial
{
public:
	QString texturename;
	TSC_color color,speccolor;
	double shininess;
};



void T3DSubFrameObject::loadobject_3ds(StrPtr filepath, StrPtr filename, double factor)
{
	L3DS loader;
	Tvec3d tmpvertex,tmpnormal;


	QString totfilename;
	totfilename=filepath;
	totfilename+="\\";
	totfilename+=filename;
	loader.LoadFile(totfilename);

	QString texfilename,objname;


	Tarray<TMaterial> materials;
	Tbaltree usedtextures;

	for (int i=0; i<loader.GetMaterialCount(); i++)
	{
		TMaterial *material=new TMaterial;materials.add(material);
		LMaterial &mat=loader.GetMaterial(i);

		material->color.Set_R(mat.GetDiffuseColor().r);
		material->color.Set_G(mat.GetDiffuseColor().g);
		material->color.Set_B(mat.GetDiffuseColor().b);
		material->color.Set_A(mat.GetTransparency());

		material->speccolor.Set_R(mat.GetSpecularColor().r);
		material->speccolor.Set_G(mat.GetSpecularColor().g);
		material->speccolor.Set_B(mat.GetSpecularColor().b);
		material->speccolor.Set_A(mat.GetTransparency());

		double shinival=mat.GetShininess()*128;
		double specfc=min(1,shinival/50);
		if (shinival<10) specfc=0.0;
		material->shininess=shinival;

		texfilename=filepath;
		texfilename+=_qstr("\\");
		texfilename+=mat.GetTextureMap1().mapName;
		if (texfilename.G_length()>0)
		{
			texfilename.Replace(_qstr(".jpg"),_qstr(".bmp"));
			texfilename.Replace(_qstr(".JPG"),_qstr(".bmp"));
			texfilename.Replace(_qstr(".JPEG"),_qstr(".bmp"));
			texfilename.Replace(_qstr(".jpeg"),_qstr(".bmp"));
			texfilename.Replace(_qstr(".gif"),_qstr(".bmp"));
			texfilename.Replace(_qstr(".GIF"),_qstr(".bmp"));
			texfilename.Replace(_qstr(".tga"),_qstr(".bmp"));
			texfilename.Replace(_qstr(".TGA"),_qstr(".bmp"));
			texfilename.Replace(_qstr(".tif"),_qstr(".bmp"));
			texfilename.Replace(_qstr(".TIF"),_qstr(".bmp"));
			FILE *fp=_wfopen(texfilename,_qstr("r"));
			if (fp!=NULL)
			{
				fclose(fp);
				material->texturename=mat.GetTextureMap1().mapName;
				if (usedtextures.get(material->texturename)<=0)
				{
					addtexture(material->texturename,texfilename);
					usedtextures.add(material->texturename,1);
				}
			}
		}
	}


	for (int i=0; i<loader.GetMeshCount(); i++)
	{
		LMesh &imesh=loader.GetMesh(i);
		bool transp=false;
		int matidx=-1;
		if (imesh.GetTriangleCount()>0)
		{
			LTriangle2 tr=imesh.GetTriangle2(0);
			matidx=tr.materialId;
			if ((matidx>=0)&&(matidx<loader.GetMaterialCount()))
			{
				LMaterial &mat=loader.GetMaterial(matidx);
				if (mat.GetTransparency()>0.505) transp=true;
			}
		}
		if (!transp)
		{
			TMaterial *material=materials[matidx];

			T3DObjectMesh *meshobject=new T3DObjectMesh;
			addsubobject(meshobject);

			meshobject->Set_scene(G_scene());
			objname=imesh.GetName().c_str();
			meshobject->G_param(_qstr("Name"))->content.fromstring(objname);

			if (material!=NULL)
			{
				G_valuecontent<TSC_color>(&meshobject->G_param(_qstr("Color"))->content)->copyfrom(&material->color);

				if (material->texturename.G_length()>0)
					meshobject->G_param(_qstr("Texture"))->content.fromstring(material->texturename);

			}

			T3Dmesh &mesh=meshobject->G_mesh();

			int vct=0;
			for (int j=0; j<imesh.GetVertexCount(); j++)
			{
				const LVector4 &ivertex=imesh.GetVertex(j);
				const LVector3 &inormal=imesh.GetNormal(j);
				const LVector2 &UV=imesh.GetUV(j);
				tmpvertex.set(ivertex.x*factor,ivertex.y*factor,ivertex.z*factor);
				tmpnormal.set(inormal.x,inormal.y,inormal.z);
				mesh.addvertex(&tmpvertex,vct,&tmpnormal,UV.x,UV.y);
				vct++;
			}
			for (int j=0; j<imesh.GetTriangleCount(); j++)
			{
				const LTriangle &itr=imesh.GetTriangle(j);
				mesh.addtriangle(itr.a,itr.b,itr.c);
			}
			mesh.generateallvertexnormals();
			mesh.calcedges();


		}
	}

}


void T3DSubFrameObject::streamout_content(QBinTagWriter &writer)
{
	for (int i=0; i<members.G_count(); i++)
	{
		QBinTagWriter &otag=writer.subtag(_qstr("Object")).G_obj();
		otag.write_shortstring(members[i]->G_classname());
		members[i]->streamout(otag);
		otag.close();
	}
	for (int i=0; i<texturelist.G_count(); i++)
	{
		QBinTagWriter &otag=writer.subtag(_qstr("Texture")).G_obj();
		texturelist.G_texture(i)->streamout(otag);
		otag.close();
	}
}

void T3DSubFrameObject::streamin_content(QBinTagReader &reader)
{
	members.reset();
	texturelist.clear();

	while (reader.hassubtags())
	{
		QBinTagReader &tg1=reader.getnextsubtag().G_obj();
		if (tg1.Istag(_qstr("Object")))
		{
			QString objtypename;
			tg1.read_shortstring(objtypename);
			T3DObject *catobj=T3DCosmos::Get().G_objectcatalog_object(objtypename);
			if (catobj==NULL) throw QError(TFormatString(_text("Invalid object type ^1"),objtypename));
			T3DObject *obj=catobj->MakeInstance();
			obj->Set_scene(G_scene());
			obj->Set_parentframe(this);
			members.add(obj);
			obj->streamin(tg1);
		}
		if (tg1.Istag(_qstr("Texture")))
		{
			T3DTexture *tx=new T3DTexture;
			tx->Set_subframe(this);
			tx->streamin(tg1);
			texturelist.add(tx);
		}
		tg1.close();
	}

}

