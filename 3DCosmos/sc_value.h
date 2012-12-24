#pragma once


#define SC_simpletypepath _qstr("Simple types")

#define SC_mediapath _qstr("Media")


#define SC_containerpath _qstr("Containers")


#define SC_valname_any _qstr("Anytype")
#define SC_valname_list _qstr("List")
#define SC_valname_mappair _qstr("MapPair")
#define SC_valname_map _qstr("Map")
#define SC_valname_boolean _qstr("Bool")
#define SC_valname_scalar _qstr("Scalar")
#define SC_valname_string _qstr("String")
#define SC_valname_color _qstr("Color")
#define SC_valname_time _qstr("Time")
#define SC_valname_chrono _qstr("Chrono")
#define SC_valname_bitmap _qstr("Bitmap")



#include "tobject.h"

#include "qstring.h"
#include "qstringformat.h"
#include "qerror.h"
#include "vecmath.h"
#include "qxformattedstring.h"

class QBinTagWriter;
class QBinTagReader;

class TSC_value;
class TSC_funcarglist;
class TSC_boolean;
class TSC_list;
class TSC_enumval;
class TQXFormattedString;





class TSC_scalar : public TObject
{
private:
	bool isintegral;
	double value;
public:
	TSC_scalar()
	{
		value=0;
		isintegral=true;
	}
	TSC_scalar(double ival)
	{
		value=ival;
	}
	void tostring(QString &str, int tpe)
	{
		str.formatscalar(value);
	}
	void fromstring(StrPtr icontent)
	{
		copyfrom(qstr2double(icontent));
	}
	void copyfrom(double ival)
	{
		value=ival;
		isintegral=(fabs(value-G_intval())<1.0e-7*fabs(value));
	}
	void copyfrom(TSC_scalar *ivl)
	{
		value=ivl->value;
		isintegral=ivl->isintegral;
	}
	double G_val() { return value; }
	int G_intval() { return (int)floor(0.5+value); }
	static bool compare(TSC_scalar *val1, TSC_scalar *val2)
	{
		if (val1->isintegral&&val2->isintegral) return val1->G_intval()==val2->G_intval();
		return val1->value==val2->value;
	}
	static bool comparesize(TSC_scalar *val1, TSC_scalar *val2)
	{
		return val1->value<val2->value;
	}
	void operator=(const TSC_scalar &obj)
	{
		value=obj.value;
		isintegral=obj.isintegral;
	}
	static bool G_smaller(TSC_scalar *val1, TSC_scalar *val2)
	{
		if (val1->isintegral&&val2->isintegral) return val1->G_intval()<val2->G_intval();
		return val1->value<val2->value;
	}

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

};


class TSC_string : public TObject
{
private:
	QString content;
public:
	TSC_string()
	{
	}
	TSC_string(StrPtr icontent)
	{
		content=icontent;
	}
	void fromstring(StrPtr icontent)
	{
		content=icontent;
	}
	void tostring(QString &str, int tpe)
	{
		str=content;
	}
	void copyfrom(TSC_string *ivl)
	{
		content=ivl->content;
	}
	StrPtr G_string() { return content; }
	static bool G_smaller(TSC_string *val1, TSC_string *val2)
	{
		return qstricmp(val1->content,val2->content)<0;
	}
	static bool compare(TSC_string *val1, TSC_string *val2)
	{
		return qstricmp(val1->content,val2->content)==0;
	}
	static bool comparesize(TSC_string *val1, TSC_string *val2)
	{
		return qstricmp(val1->content,val2->content)<0;
	}
	void operator=(const TSC_string &m)
	{
		content=m.content;
	}
	QString& G_QString()
	{
		return content;
	}

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

};





class TSC_datatype
{
public:
	QString classpath;
	virtual StrPtr G_name()=0;
	StrPtr G_classpath() { return classpath; }
	void G_fullID(QString &fullID);
	void append_classpath(StrPtr iname) { classpath+=_qstr(";"); classpath+=iname; }
	virtual ~TSC_datatype() {}
	virtual TSC_datatype* duplicate()
	{
		ASSERT(false);
		return NULL;
	}
	virtual TObject* value_create()=0;
	virtual void value_delete(TObject *content)=0;
	virtual void value_copy(TObject *from, TObject *to)=0;
	virtual void tostring(TObject *content, QString &str, int tpe=0)=0;
	virtual void fromstring(TObject *content, StrPtr str) { ASSERT(false); };
	virtual int toint(TObject *content)
	{
		ASSERT(false);
		throw QError(_qstr("Unable to convert variable to integer value"));
	}
	virtual void fromint(int vl, TObject *content)
	{
		ASSERT(false);
		throw QError(_qstr("Unable to convert variable from integer value"));
	}
	virtual bool compare(TObject *content1, TObject *content2)=0;
	virtual bool cancopy()=0;
	virtual void dispose(TObject *content) {};

	virtual void streamout(QBinTagWriter &writer, TObject *content);
	virtual void streamin(QBinTagReader &reader, TObject *content);

public:
	bool issame(TSC_datatype *itype)
	{
		return this==itype;
	}
	virtual bool memberfunction_accept(TObject *content, StrPtr funcname)//offers a datatype the opportunity to implement member functions by itself
	{ return false; }
	virtual bool evalmemberfunction(TObject *content, StrPtr funcname, TSC_value *returnval, TSC_value *assigntoval, TSC_funcarglist &arglist, QString &error)
	{ error="Invalid member function"; return false; }

	virtual int G_memberfunctioncount() { return 0; }
	virtual void G_memberfunctiondescription(int nr, QString &name, TQXFormattedString &prototype, QString &returndatatype, QString &cfsubclass) {};

	virtual int G_optioncount() { return 0; }
	virtual StrPtr G_option(int nr) { ASSERT(false); return NULL; }

	virtual bool G_isanytype() { return false; }

	virtual bool comparesize(const TObject *content1, const TObject *content2)
	{
		throw QError(TFormatString(_text("Unable to compare sizes of type ^1"),G_name()));
		return false;
	}


};



//WARNING: assignment operator needs to be properly working for this template
template <class valuetype>
class TSC_datatype_generic : public TSC_datatype
{
protected:
	QString name;
	bool _cancopy;
public:
	StrPtr G_name()
	{ 
		return name;
	}
	TSC_datatype_generic(StrPtr iname, bool icancopy)
	{
		name=iname;
		_cancopy=icancopy;
	}
	TObject* value_create()
	{
		return new valuetype();
	}
	void value_delete(TObject *content)
	{
		((valuetype*)content)->G_objID();
		if (content!=NULL) delete content;
	}
	void value_copy(TObject *from, TObject *to)
	{
		ASSERT(_cancopy);
		if ((from==NULL)||(to==NULL)) return;
		*((valuetype*)to)=*((valuetype*)from);
	}
	TSC_datatype* duplicate()
	{
		ASSERT(false);
		return NULL;
	}
	void tostring(TObject *content, QString &str, int tpe=-1)
	{
		if (content==NULL) { str=_qstr("-?-"); return; }
		((valuetype*)content)->tostring(str,tpe);
	};
	void fromstring(TObject *content, StrPtr str)
	{
		if (content==NULL) return;
		((valuetype*)content)->fromstring(str);
	}
	bool compare(TObject *content1, TObject *content2)
	{
		if ((content1==NULL)||(content2==NULL)) return false;
		return valuetype::compare((valuetype*)content1,(valuetype*)content2);
	}
	bool comparesize(const TObject *content1, const TObject *content2)
	{
		if ((content1==NULL)||(content2==NULL)) return false;
		return valuetype::comparesize((valuetype*)content1,(valuetype*)content2);
	}
	bool cancopy() { return _cancopy; };

	virtual void streamout(QBinTagWriter &writer, TObject *content)
	{
		((valuetype*)content)->streamout(writer);
	}

	virtual void streamin(QBinTagReader &reader, TObject *content)
	{
		((valuetype*)content)->streamin(reader);
	}

};


template <class valuetype>
class TSC_datatype_generic_withmemberfunctions : public TSC_datatype_generic<valuetype>
{
protected:
	Tarray<QString> memberfunction_names,memberfunction_returndatatypes,memberfunction_subclass;
	Tarray<TQXFormattedString> memberfunction_prototypes;
public:
	TSC_datatype_generic_withmemberfunctions(StrPtr iname,bool canassign)
		: TSC_datatype_generic<valuetype>(iname,canassign)
	{
	}
	virtual void dispose(TObject *content)
	{
		if (content==NULL) return;
		valuetype *vl=(valuetype*)content;
		return vl->dispose();
	}
	virtual bool memberfunction_accept(TObject *content, StrPtr funcname)
	{
		if (content==NULL) return false;
		valuetype *vl=(valuetype*)content;
		return vl->memberfunction_accept(funcname);
	}
	virtual bool evalmemberfunction(TObject *content, StrPtr funcname, TSC_value *returnval, TSC_value *assigntoval, TSC_funcarglist &arglist, QString &error)
	{
		if (content==NULL) return false;
		valuetype *vl=(valuetype*)content;
		return vl->memberfunction_eval(funcname,returnval,assigntoval,arglist,error);
	}
	void addmemberfunction(StrPtr name, TQXFormattedString &prototype, StrPtr returndatatype, StrPtr subclass)
	{
		memberfunction_names.add(new QString(name));
		TQXFormattedString *fstr=new TQXFormattedString;*fstr=prototype;
		memberfunction_prototypes.add(fstr);
		memberfunction_returndatatypes.add(new QString(returndatatype));
		memberfunction_subclass.add(new QString(subclass));
	}
	virtual int G_memberfunctioncount()
	{ 
		return memberfunction_names.G_count();
	}
	virtual void G_memberfunctiondescription(int nr, QString &iname, TQXFormattedString &prototype, QString &returndatatype, QString &cfsubclass)
	{
		iname=*memberfunction_names[nr];
		prototype=*memberfunction_prototypes[nr];
		returndatatype=*memberfunction_returndatatypes[nr];
		cfsubclass=*memberfunction_subclass[nr];
	};
};



class TSC_value : public TObjectRef
{
private:
	TSC_datatype *datatype;
	TObject *content;
	bool isowner;
	bool isexplicitreference;
public:
	TSC_value()
	{
		datatype=NULL;
		content=NULL;
		isowner=true;
		isexplicitreference=false;
	}
	~TSC_value()
	{
		clear();
	}
	void clear()
	{
		if ((datatype!=NULL)&&(content!=NULL)&&isowner) datatype->value_delete(content);
		datatype=NULL;content=NULL;
		isexplicitreference=false;
	}
	void Make_explicitreference() { isexplicitreference=true; }
	TSC_value* duplicate()
	{
		TSC_value *val=new TSC_value;
		if (datatype!=NULL)
		{
			val->datatype=datatype;
			if (datatype->cancopy())
			{
				val->content=datatype->value_create();
				val->isowner=true;
				datatype->value_copy(content,val->content);
			}
			else
			{
				val->encapsulate(content);
			}
		}
		return val;
	}
	void settype(TSC_datatype *idatatype)
	{
		if ((datatype!=NULL)&&(idatatype!=NULL)&&(datatype->issame(idatatype))) return;
		clear();
		if (idatatype!=NULL) datatype=idatatype;
	}
	void encapsulate(TObject *icontent)
	{
		if ((datatype!=NULL)&&(content!=NULL)&&isowner) datatype->value_delete(content);
		content=icontent;
		setreffor(content);
		isowner=false;
	}
	void encapsulate(TSC_value *ivalue)
	{
		clear();
		if ((ivalue->datatype!=NULL)&&(ivalue->content!=NULL))
		{
			datatype=ivalue->datatype;
			encapsulate(ivalue->content);
		}
	}
	bool istype(StrPtr tpename)
	{
		if (datatype==NULL) return false;
		return qstricmp(datatype->G_name(),tpename)==0;
	}
	void create()
	{
		if (datatype!=NULL)
		{
			if ((content!=NULL)&&isowner) datatype->value_delete(content);
			ASSERT(datatype!=NULL);
			content=datatype->value_create();
			isowner=true;isexplicitreference=false;
		}
	}
	void createtype(TSC_datatype *idatatype)
	{
		settype(idatatype);
		create();
	}
	void maketype(TSC_datatype *idatatype)
	{
		ASSERT(idatatype!=NULL);
		if ((idatatype!=datatype)||(content==NULL)||(!isowner))
		{
			if (idatatype->cancopy()) createtype(idatatype);
			else
				if (datatype!=idatatype) settype(idatatype);
		}
	}
	void tostring(QString &str, int tpe=0)
	{
		if (datatype==NULL) { str=_qstr("?"); return; }
		datatype->tostring(content,str,tpe);
	}
	void fromstring(StrPtr str)
	{
		if (datatype==NULL) return;
		if (content==NULL) content=datatype->value_create();
		datatype->fromstring(content,str);
	}
	void copyfrom(double vl)
	{
		if (datatype==NULL) return;
		if (qstricmp(datatype->G_name(),SC_valname_scalar)!=0)
		{
			ASSERT(false);
			return;
		}
		if (content==NULL) content=datatype->value_create();
		isowner=true;isexplicitreference=false;
		((TSC_scalar*)content)->copyfrom(vl);
	}
	void copyfrom(int vl)
	{
		if (datatype==NULL) return;
		if (qstricmp(datatype->G_name(),SC_valname_scalar)!=0)
		{
			ASSERT(false);
			return;
		}
		if (content==NULL) content=datatype->value_create();
		isowner=true;isexplicitreference=false;
		((TSC_scalar*)content)->copyfrom(vl);
	}
	void copyfrom(bool vl);
	double todouble()
	{
		if (datatype==NULL) return 0;
		if (qstricmp(datatype->G_name(),SC_valname_scalar)!=0) return 0;
		return ((TSC_scalar*)content)->G_val();
	}
	int toint();
	void fromint(int vl);
	bool copyfrom_flexibletype(const TSC_value *ivalue)
	{
		if (ivalue==NULL) { clear(); return true; }
		if (ivalue->content==NULL) clear();
		settype(ivalue->datatype);
		if (datatype!=NULL)
		{
			if ((datatype->cancopy())&&(ivalue->isowner))
			{
				if ((content==NULL)&&(ivalue->content!=NULL))
				{
					content=datatype->value_create();
					isowner=true;
				}
				if (ivalue->content!=NULL) datatype->value_copy(ivalue->content,content);
			}
			else
			{
				encapsulate(ivalue->content);
			}
		}
		return true;
	}
	bool copyfrom(const TSC_value *ivalue)
	{
		if (ivalue==NULL) return false;
		if (datatype==NULL) settype(ivalue->datatype);
		if ((ivalue->content==NULL)||(ivalue->datatype==NULL)) return false;
		if ((datatype!=NULL)&&(!datatype->issame(ivalue->datatype))) return false;
		if ((datatype->cancopy())&&(!ivalue->isexplicitreference))
		{
			if (content==NULL)
			{
				content=datatype->value_create();
				isowner=true;
			}
			datatype->value_copy(ivalue->content,content);
		}
		else encapsulate(ivalue->content);
		return true;
	}
	void unref(const TSC_value *ivalue)
	{
		if (ivalue==NULL) return;
		if (datatype==NULL) settype(ivalue->datatype);
		if ((ivalue->content==NULL)||(ivalue->datatype==NULL)) return;
		if ((datatype!=NULL)&&(!datatype->issame(ivalue->datatype))) throw QError(_text("Unref: inconsistent data types"));
		if (ivalue->isowner) throw QError(_text("Unref: argument is not a reference"));
		if (!datatype->cancopy()) throw QError(_text("Unref: data type can not be copied"));
		if (content==NULL)
		{
			content=datatype->value_create();
			isowner=true;isexplicitreference=false;
		}
		datatype->value_copy(ivalue->content,content);
	}
	bool operator==(TSC_value &ivalue);
	bool operator<(const TSC_value &ivalue) const;
	TSC_datatype* G_datatype() { return datatype; }
	bool G_isreference() { return (content!=NULL)&&(!isowner); }

	virtual bool memberfunction_accept(StrPtr funcname)//offers a value the opportunity to implement member functions by itself
	{
		if ((datatype==NULL)||(content==NULL)) return false;
		return datatype->memberfunction_accept(content,funcname);
	}
	virtual bool evalmemberfunction(StrPtr funcname, TSC_value *returnval, TSC_value *assigntoval, TSC_funcarglist &arglist, QString &error)
	{
		if ((datatype==NULL)||(content==NULL))
		{
			error="Invalid obejct for member function";
			return false;
		}
		return datatype->evalmemberfunction(content,funcname,returnval,assigntoval,arglist,error);
	}
	void dispose()
	{
		if ((datatype==NULL)||(content==NULL)) return;
		datatype->dispose(content);
		clear();
	}

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

	void streamout_withtype(QBinTagWriter &writer);
	void streamin_withtype(QBinTagReader &reader);


	virtual void notify_objectdeleted()
	{
		content=NULL;
	}


	void checkcontent(TSC_datatype *idatatype);
	TObject* G_content() { return content; }
	TSC_string* G_content_string();
	TSC_scalar* G_content_scalar();
	TSC_boolean* G_content_boolean();

};

template<class A> A* G_valuecontent(TSC_value *value)
{
	ASSERT(value!=NULL);
	value->checkcontent(GetTSCenv().G_datatype(A::GetClassName()));
	return (A*)value->G_content();
}

void GetObjectName(TSC_value *val, QString &name);
