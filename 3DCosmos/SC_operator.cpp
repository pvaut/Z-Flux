#include "stdafx.h"

#include "qerror.h"

#include "SC_operator.h"
#include "SC_env.h"


StrPtr G_operatortypename(int opID);


///////////////////////////////////////////////////////////////////////////////////

TSC_operator::TSC_operator(int iopID, StrPtr ivartpe1, StrPtr ivartpe2, StrPtr irettpe)
{
	classpath=_qstr("Operators");
	opID=iopID;
	vartype1=GetTSCenv().G_datatype(ivartpe1);
	vartype2=GetTSCenv().G_datatype(ivartpe2);
	returntype=GetTSCenv().G_datatype(irettpe);
	ASSERT(vartype1!=NULL);
	ASSERT(vartype2!=NULL);
	ASSERT(returntype!=NULL);
}


void TSC_operator::setclasspath_fromtype(StrPtr itypename)
{
	TSC_datatype *datatype=GetTSCenv().G_datatype(itypename);
	ASSERT(datatype!=NULL);
	QString path;
	path=datatype->G_classpath();
	if (path.G_length()>0) path+=_qstr(";");
	path+=itypename;
	setclasspath(path);
}



void TSC_operator::G_fullID(QString &fullID)
{
	fullID+=_qstr("O:");
	fullID+=vartype1->G_name();
	fullID+=G_operatortypename(G_opID());
	fullID+=vartype2->G_name();
}


void TSC_operator::execute(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	if (returntype!=GETDATATYPE_ANY) retval->maketype(returntype);
	execute_implement(val1,val2,retval);
}


///////////////////////////////////////////////////////////////////////////////////

TSC_operatoradder::TSC_operatoradder(TSC_operator *ioperator)
{
	GetTSCenv().addoperator(ioperator);
}




///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_mapper,15,SC_valname_string,SC_valname_any,SC_valname_mappair)
{
	setclasspath_fromtype(SC_valname_mappair);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	QString str,st2;
	StrPtr st1=val1->G_content_string()->G_string();
	val2->tostring(st2);
	str=st1;str+="<Mapped>";str+=st2;
	G_valuecontent<TSC_mappair>(retval)->import(st1,val2);
}
ENDOPERATOR(op_mapper)



///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_plus_string,8,SC_valname_string,SC_valname_string,SC_valname_string)
{
	setclasspath_fromtype(SC_valname_string);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	QString str;
	StrPtr st1=val1->G_content_string()->G_string();
	StrPtr st2=val2->G_content_string()->G_string();
	str=st1;str+=st2;
	retval->G_content_string()->fromstring(str);
}
ENDOPERATOR(op_plus_string)



///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_smaller_string,4,SC_valname_string,SC_valname_string,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_string);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(TSC_string::G_smaller(val1->G_content_string(),val2->G_content_string()));
}
ENDOPERATOR(op_smaller_string)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_smallereq_string,6,SC_valname_string,SC_valname_string,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_string);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(!TSC_string::G_smaller(val2->G_content_string(),val1->G_content_string()));
}
ENDOPERATOR(op_smallereq_string)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_bigger_string,5,SC_valname_string,SC_valname_string,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_string);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(TSC_string::G_smaller(val2->G_content_string(),val1->G_content_string()));
}
ENDOPERATOR(op_bigger_string)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_biggereq_string,7,SC_valname_string,SC_valname_string,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_string);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(!TSC_string::G_smaller(val1->G_content_string(),val2->G_content_string()));
}
ENDOPERATOR(op_biggereq_string)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_eq_string,3,SC_valname_string,SC_valname_string,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_string);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(TSC_string::compare(val1->G_content_string(),val2->G_content_string()));
}
ENDOPERATOR(op_eq_string)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_diff_string,14,SC_valname_string,SC_valname_string,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_string);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(!TSC_string::compare(val1->G_content_string(),val2->G_content_string()));
}
ENDOPERATOR(op_diff_string)



///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_plus_scalar,8,SC_valname_scalar,SC_valname_scalar,SC_valname_scalar)
{
	setclasspath_fromtype(SC_valname_scalar);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	double rs=val1->G_content_scalar()->G_val()+val2->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(rs);
}
ENDOPERATOR(op_plus_scalar)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_min_scalar,9,SC_valname_scalar,SC_valname_scalar,SC_valname_scalar)
{
	setclasspath_fromtype(SC_valname_scalar);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	double rs=val1->G_content_scalar()->G_val()-val2->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(rs);
}
ENDOPERATOR(op_min_scalar)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_mul_scalar,10,SC_valname_scalar,SC_valname_scalar,SC_valname_scalar)
{
	setclasspath_fromtype(SC_valname_scalar);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	double rs=val1->G_content_scalar()->G_val()*val2->G_content_scalar()->G_val();
	retval->G_content_scalar()->copyfrom(rs);
}
ENDOPERATOR(op_mul_scalar)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_div_scalar,11,SC_valname_scalar,SC_valname_scalar,SC_valname_scalar)
{
	setclasspath_fromtype(SC_valname_scalar);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	double vl1=val1->G_content_scalar()->G_val();
	double vl2=val2->G_content_scalar()->G_val();
	if (vl2==0)
		throw QError(_text("Division by zero"));
	double rs=vl1/vl2;
	retval->G_content_scalar()->copyfrom(rs);
}
ENDOPERATOR(op_div_scalar)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_pow_scalar,12,SC_valname_scalar,SC_valname_scalar,SC_valname_scalar)
{
	setclasspath_fromtype(SC_valname_scalar);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	double vl1=val1->G_content_scalar()->G_val();
	double vl2=val2->G_content_scalar()->G_val();
	try{
		double rs=pow(vl1,vl2);
		retval->G_content_scalar()->copyfrom(rs);
	}catch(...)
	{
		throw QError(_text("Invalid arguments for power operator"));
	}
}
ENDOPERATOR(op_pow_scalar)



///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_smaller_scalar,4,SC_valname_scalar,SC_valname_scalar,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_scalar);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(TSC_scalar::G_smaller(val1->G_content_scalar(),val2->G_content_scalar()));
}
ENDOPERATOR(op_smaller_scalar)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_smallereq_scalar,6,SC_valname_scalar,SC_valname_scalar,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_scalar);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(!TSC_scalar::G_smaller(val2->G_content_scalar(),val1->G_content_scalar()));
}
ENDOPERATOR(op_smallereq_scalar)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_bigger_scalar,5,SC_valname_scalar,SC_valname_scalar,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_scalar);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(TSC_scalar::G_smaller(val2->G_content_scalar(),val1->G_content_scalar()));
}
ENDOPERATOR(op_bigger_scalar)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_biggereq_scalar,7,SC_valname_scalar,SC_valname_scalar,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_scalar);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(!TSC_scalar::G_smaller(val1->G_content_scalar(),val2->G_content_scalar()));
}
ENDOPERATOR(op_biggereq_scalar)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_eq_scalar,3,SC_valname_scalar,SC_valname_scalar,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_scalar);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(TSC_scalar::compare(val1->G_content_scalar(),val2->G_content_scalar()));
}
ENDOPERATOR(op_eq_scalar)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_diff_scalar,14,SC_valname_scalar,SC_valname_scalar,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_scalar);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(!TSC_scalar::compare(val1->G_content_scalar(),val2->G_content_scalar()));
}
ENDOPERATOR(op_diff_scalar)
