#pragma once

#include "tools.h"
#include "qstring.h"

#include "SC_func.h"

class TSC_script_customfunc : public TSC_func
{
private:
	TSC_script_block *block;
public:
	TSC_script_customfunc(StrPtr ifuncname, Tarray<QString> &argnames,  TSC_script_block *iblock);
	~TSC_script_customfunc();
	void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner);
};