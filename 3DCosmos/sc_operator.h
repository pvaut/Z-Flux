#pragma once

#include "tools.h"
#include "SC_value.h"




class TSC_env;


#define OPID_DOT 13



class TSC_operator
{
protected:
	QString classpath;
	int opID;
	TSC_datatype *vartype1,*vartype2;
	TSC_datatype *returntype;
	QString description;
public:
	TSC_operator(int iopID, StrPtr ivartpe1, StrPtr ivartpe2, StrPtr irettpe);
	void setclasspath_fromtype(StrPtr itypename);
	void setclasspath(StrPtr iclasspath) { classpath=iclasspath; }
public:
	StrPtr G_classpath() { return classpath; }
	void G_fullID(QString &fullID);
	int G_opID() { return opID; }
	TSC_datatype* G_vartype1() { return vartype1; }
	TSC_datatype* G_vartype2() { return vartype2; }
	TSC_datatype* G_returntype() { return returntype; }
	StrPtr G_description() { return description; }
	void Set_description(StrPtr istr) { description=istr; }

	void execute(TSC_value *val1, TSC_value *val2, TSC_value *retval);

	virtual void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)=0;
};




class TSC_operatoradder{
public:
	TSC_operatoradder(TSC_operator *ioperator);
};


#define OPERATOR(CLASSNAME,OPID,TPE1,TPE2,RETTPE)		class CLASSNAME : public TSC_operator{\
public:\
	static TSC_operator* Create() { return new CLASSNAME;  }\
	static TSC_operatoradder  adder;\
	CLASSNAME() : TSC_operator(OPID,TPE1,TPE2,RETTPE)

#define ENDOPERATOR(X)		}; TSC_operatoradder  X::adder(X::Create());
