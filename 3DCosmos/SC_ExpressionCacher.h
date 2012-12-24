#pragma once

class TSC_operator;
class TSC_script_var;

class TSC_ExpressionCacher
{
public:
	mutable TSC_operator *cached_operator;
	mutable TSC_script_var *cached_var;
public:
	TSC_ExpressionCacher()
	{
		cached_operator=NULL;
		cached_var=NULL;
	}
	virtual ~TSC_ExpressionCacher() {}
};
