#pragma once
#include "SC_value.h"

class TSC_enumval : public TObject
{
	friend class TSC_datatype_genericenum;
private:
	int val;
public:
	int G_val() { return val; }
	void Set_val(int vl)
	{
		val=vl;
	}
	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);
};

class TSC_datatype_genericenum : public TSC_datatype
{
private:
	int defaultval;
	QString name;
	Tarray<QString> numnames,numdescriptions;
	Tintarray numvals;
	Tbaltree numidx;
public:
	virtual StrPtr G_name() { return name; }
	TSC_datatype_genericenum(StrPtr iname)
	{
		defaultval=0;
		name=iname;
		classpath=_qstr("Enumerations");
	}
	void add(StrPtr inumname, int inumval, StrPtr idescription=NULL)
	{
		if (numvals.G_count()==0) defaultval=inumval;
		numnames.add(new QString(inumname));
		numvals.add(inumval);
		numidx.add(inumname,inumval);
		if (idescription!=NULL) numdescriptions.add(new QString(idescription));
		else numdescriptions.add(new QString(_qstr("")));
	}
	virtual ~TSC_datatype_genericenum() {}
	virtual TSC_datatype_genericenum* duplicate();
	virtual TObject* value_create()
	{
		TSC_enumval *vl=new TSC_enumval;
		vl->val=defaultval;
		return vl;
	}
	virtual void value_delete(TObject *content)
	{
		delete (TSC_enumval*) content;
	}
	virtual void value_copy(TObject *from, TObject *to)
	{
		((TSC_enumval*)to)->val=((TSC_enumval*)from)->val;
	}
	virtual void tostring(TObject *content, QString &str, int tpe=0)
	{
		str.clear();
		if (content==NULL) return;
		int vl=((TSC_enumval*)content)->val;
		for (int i=0; i<numvals.G_count(); i++)
			if (numvals[i]==vl)
			{
				str=*numnames[i];
				return;
			}
	}
	virtual void fromint(int vl, TObject *content)
	{
		if (content==NULL) throw QError(_qstr("Empty enum value"));
		return ((TSC_enumval*)content)->Set_val(vl);
	}

	virtual int toint(TObject *content)
	{
		if (content==NULL) throw QError(_qstr("Empty enum value"));
		return ((TSC_enumval*)content)->G_val();
	}

	virtual void fromstring(TObject *content, StrPtr str)
	{
		if (content==NULL) return;
		((TSC_enumval*)content)->val=numidx.get(str);
	}
	int string2int(StrPtr str)
	{
		return numidx.get(str);
	}
	virtual bool compare(TObject *content1, TObject *content2)
	{
		return ((TSC_enumval*)content1)->val==((TSC_enumval*)content2)->val;
	}
	virtual bool cancopy() { return true; }
	virtual void dispose(TObject *content) {};

	virtual int G_optioncount() { return numnames.G_count(); }
	virtual StrPtr G_option(int nr)
	{
		if ((nr<0)||(nr>=G_optioncount())) return _qstr("");
		return *numnames[nr];
	}
	virtual StrPtr G_optiondescription(int nr)
	{
		if ((nr<0)||(nr>=G_optioncount())) return _qstr("");
		return *numdescriptions[nr];
	}

public:
	virtual void streamout(QBinTagWriter &writer, TObject *content)
	{
		((TSC_enumval*) content)->streamout(writer);
	}

	virtual void streamin(QBinTagReader &reader, TObject *content)
	{
		((TSC_enumval*) content)->streamin(reader);
	}
};
