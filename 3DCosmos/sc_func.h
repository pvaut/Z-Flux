#pragma once

#include "valnames.h"

#include "tools.h"
#include "SC_value.h"
#include "qxformattedstring.h"




class TSC_env;
class TSC_script_block;

class TSC_funcarglist
{
private:
	Tcheapcopyarray<TSC_value> values;
	TSC_script_block *parentblock;
public:
	TSC_funcarglist(TSC_script_block *iparentblock);
	TSC_script_block* G_parentblock() { return parentblock; }
	void add(TSC_value *ival)
	{
		values.add(ival);
	};
	bool G_ispresent(int nr)
	{
		return nr<values.G_count();
	}
	TSC_value* get(int nr)
	{
		if (!G_ispresent(nr))
			throw QError(_text("Missing function argument"));
		return values[nr];
	}
	int G_count() { return values.G_count(); }
};

class TSC_func_var
{
public:
	QString name;
	TSC_datatype *vartype;
	bool required;
	TSC_func_var()
	{
		vartype=NULL;
		required=true;
	}
	TSC_func_var(StrPtr iname, TSC_datatype *itype)
	{
		name=iname;
		vartype=itype;
		required=true;
	}
};

class TSC_func : public TObject
{
private:
	QString name,classpath;
	bool hasreturn;
	bool ismemberfunction;//if true, arg 0 contains parent data type
	bool canassign;
	bool flexiblearglist;//if true, the argument list contains a variable number of arguments of any type
	Tarray<TSC_func_var> vars;
	TSC_datatype *returntype;
public:
	TSC_func(StrPtr iname);
	virtual ~TSC_func() {}
	void setcanassign() { canassign=true; }
	void setname(StrPtr iname) { name=iname; }
	void setclasspath_fromtype(StrPtr itypename);
	void setclasspath(StrPtr iclasspath) { classpath=iclasspath; }
	void extendclasspath(StrPtr ipart);
	void setreturntype(StrPtr itypename);
	void setmemberfunction(StrPtr itypename);
	void setflexiblearglist() { flexiblearglist=true; }
	void addvar(StrPtr iname, StrPtr itypename, bool irequired=true);
public:
	StrPtr G_name() { return name; }
	StrPtr G_classpath() { return classpath; }
	void G_fullID(QString &fullID);
	bool G_hasreturn() { return hasreturn; }
	bool G_ismemberfunction() { return ismemberfunction; }
	bool G_flexiblearglist() { return flexiblearglist; }
	int G_varcount() { return vars.G_count(); }
	StrPtr G_varname(int varnr);
	TSC_datatype* G_vartype(int varnr);
	bool G_varrequired(int varnr);
	TSC_datatype* G_returntype() { return returntype; }
	bool G_canassign() { return canassign; }

	void execute(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner);

	virtual void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)=0;
};

class TSC_funcadder{
public:
	TSC_funcadder(TSC_func *ifunc);
};


#define FUNCTION(CLASSNAME,FUNCNAME)		class CLASSNAME : public TSC_func{\
public:\
	static TSC_func* Create() { return new CLASSNAME;  }\
	static TSC_funcadder  adder;\
	CLASSNAME() : TSC_func(_qstr(#FUNCNAME))

#define ENDFUNCTION(X)		}; TSC_funcadder  X::adder(X::Create());




