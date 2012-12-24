#pragma once

#include "tools.h"
#include "SC_value.h"
#include "SC_ExpressionCacher.h"


class TSC_script_cmd_expression;
class Tscriptsourcepart;

class TSC_exprcomp : public TSC_ExpressionCacher
{
private:
	Tarray<TSC_exprcomp> subcomp;
	TSC_script_cmd_expression *expr;
	int tpe;//1=fixed value 3=binary operator 4=identifier
	int operatortype;//only used if tpe=3
	int subcomptype;//0=components of binary operator 1=function arguments 2=array indices
	bool isassignable;//true if appears as the left part of an assignation
	TSC_value value;
	QString identifiername;
	QString tmp_error;
public:
	TSC_exprcomp(TSC_script_cmd_expression *iexpr)
	{
		expr=iexpr;
		tpe=-1;
		operatortype=-1;
		subcomptype=0;
		isassignable=false;
	}
	bool parse(Tscriptsourcepart &source);
	StrPtr G_assign_varname();//if expression is a variable assignment, returns the name of the variable in the left part
	TSC_value* execute_identifier(TSC_value *assigntoval);
	TSC_value* execute_operator_assign(TSC_value *assigntoval);
	TSC_value* execute_operator_dot(TSC_value *assigntoval);
	TSC_value* execute_operator();
	TSC_value* execute(TSC_value *assigntoval);

	void createstring(TSC_exprcomp *parentcomp, QString &str);
};