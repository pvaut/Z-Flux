#pragma once

#include "SC_value.h"
#include "SC_env.h"
#include "qbintagfile.h"

#include "valnames.h"

#define ObjNameStr _qstr("Name")

class TQXFormattedString;

class Tparam
{
public:
	QString name,subclass;
	TSC_value content;
	bool readonly;
public:
	Tparam()
	{
		readonly=false;
	}
};


class Tparamset :  public TObject
{
protected:
	Tarray<Tparam> params;
	bool isdirty;//true if a parameter has been modified since last reset of this flag
public:
	Tparamset();
	Tparam* addparam(StrPtr iname, StrPtr tpe, bool readonly=false, StrPtr idummy=NULL, StrPtr isubclass=NULL);
	virtual StrPtr G_classname()=0;
	virtual void G_name(QString &str);
	void Set_name(StrPtr iname);
	virtual void G_displayname(TQXFormattedString &fstr);
	virtual void G_description(QString &str);//Latex-formatted description
	void delallparams() { params.reset(); }
	int G_paramcount();
	StrPtr G_paramname(int nr);
	StrPtr G_paramsubclass(int nr);
	TSC_value* G_paramcontent(int nr);
	void Set_paramcontent(int nr, TSC_value *newvalue);
	bool G_paramreadonly(int nr);
	Tparam* G_param(StrPtr iname);
	Tparam& G_param_required(StrPtr iname);
	StrPtr G_paramstring(StrPtr iname);
	int G_paramint(StrPtr iname);
	double G_paramdouble(StrPtr iname);
	template<class A> A* G_paramval(StrPtr iname)
	{
		Tparam *param=G_param(iname);
		ASSERT(param!=NULL);if (param==NULL) return NULL;
		param->content.checkcontent(GetTSCenv().G_datatype(A::GetClassName()));
		return (A*)param->content.G_content();
	}
	void param_setvalue(StrPtr iname, StrPtr ival);
	void param_setvalue(StrPtr iname, double ival);
	void param_readonly(StrPtr iname, bool isreadonly);
	virtual bool G_param_optionlist(StrPtr paramname, Tarray<QString> &list) { return false; }
public:
	virtual void paramchanged(StrPtr iname) {}//overwrite this to be notified about parameter changes
	void makedirty() { isdirty=true; }

};


class TObjectTreeItem : public Tparamset
{
public:
	bool expanded;
	int posx,posy,leny;
	static StrPtr GetClassName() { return SC_valname_object; }
public:
	TObjectTreeItem()
	{
		expanded=false;
	}
	virtual StrPtr G_classname() { return _qstr(""); }
	virtual void G_fullID(QString &fullID) {};
	virtual TSC_datatype* G_datatype();
	virtual ~TObjectTreeItem();
	virtual TObjectTreeItem *G_parent() { return NULL; }
	virtual int G_childcount() { return 0; }
	virtual TObjectTreeItem* G_child(int nr) { return NULL; }
	virtual TObjectTreeItem* G_childbyname(StrPtr iname);
	virtual void delchild(TObjectTreeItem *ichild) {}

	virtual void G_sourcecodename(QString &str) {};//name as it should be copied to the source code

//implemented to give valid value in context of TSC_datatype_generic
	virtual void tostring(QString &str, int tpe);
	virtual void fromstring(StrPtr str) {}
	void operator=(TObjectTreeItem &v)
	{
		ASSERT(false);
	}

	static bool compare(TObjectTreeItem *content1, TObjectTreeItem *content2)
	{
		return content1==content2;
	}
	static bool comparesize(TObjectTreeItem *content1, TObjectTreeItem *content2)
	{
		throw QError(_text("Unable to compare this data type"));
	}


	void dispose();

	bool memberfunction_accept(StrPtr funcname);
	bool memberfunction_eval(StrPtr funcname, TSC_value *returnval, TSC_value *assigntoval, TSC_funcarglist &arglist, QString &error);

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

	virtual void streamout_content(QBinTagWriter &writer);
	virtual void streamin_content(QBinTagReader &reader);


};