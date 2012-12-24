#include "stdafx.h"
#include "SC_value.h"
#include "SC_boolean.h"
#include "SC_env.h"

TSC_boolean* TSC_value::G_content_boolean()
{
	checkcontent(GETDATATYPE_BOOLEAN);
	return (TSC_boolean*)content;
}

void TSC_boolean::streamout(QBinTagWriter &writer)
{
	writer.write_bool(value);
}

void TSC_boolean::streamin(QBinTagReader &reader)
{
	copyfrom(reader.read_bool());
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_bool_not,not)
{
	setclasspath_fromtype(SC_valname_boolean);
	setcanassign();
	setreturntype(SC_valname_boolean);
	addvar(_qstr("arg"),SC_valname_boolean);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_boolean()->copyfrom(!arglist->get(0)->G_content_boolean()->G_val());
}
ENDFUNCTION(func_bool_not)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_bool_and,0,SC_valname_boolean,SC_valname_boolean,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_boolean);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(val1->G_content_boolean()->G_val()&&val2->G_content_boolean()->G_val());
}
ENDOPERATOR(op_bool_and)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_bool_or,1,SC_valname_boolean,SC_valname_boolean,SC_valname_boolean)
{
	setclasspath_fromtype(SC_valname_boolean);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	retval->G_content_boolean()->copyfrom(val1->G_content_boolean()->G_val()||val2->G_content_boolean()->G_val());
}
ENDOPERATOR(op_bool_or)
