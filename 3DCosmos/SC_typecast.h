#pragma once

#include "tools.h"
#include "SC_value.h"




class TSC_env;




class TSC_typecast
{
protected:
	QString classpath;
	TSC_datatype *argtype;
	TSC_datatype *returntype;
public:
	TSC_typecast(StrPtr iargtpe, StrPtr irettpe);
public:
	StrPtr G_classpath() { return classpath; }
	TSC_datatype* G_argtype() { return argtype; }
	TSC_datatype* G_returntype() { return returntype; }
	void G_fullID(QString &fullID);

	void execute(TSC_value *argval, TSC_value *retval);
	virtual void execute_implement(TSC_value *argval, TSC_value *retval)=0;
};


class TSC_typecastadder{
public:
	TSC_typecastadder(TSC_typecast *itypecast);
};


#define TYPECAST(CLASSNAME,ARGTPE,RETTPE)		class CLASSNAME : public TSC_typecast{\
public:\
	static TSC_typecast* Create() { return new CLASSNAME;  }\
	static TSC_typecastadder  adder;\
	CLASSNAME() : TSC_typecast(ARGTPE,RETTPE)

#define ENDTYPECAST(X)		}; TSC_typecastadder  X::adder(X::Create());
