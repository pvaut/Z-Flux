#include "stdafx.h"

#include "SC_script.h"
#include "SC_scriptcomp.h"
#include "SC_customfunc.h"


TSC_script_customfunc::TSC_script_customfunc(StrPtr ifuncname, Tarray<QString> &argnames,  TSC_script_block *iblock)
  : TSC_func(ifuncname)
{
	setreturntype(SC_valname_any);
	for (int i=0; i<argnames.G_count(); i++)
		addvar(*argnames[i],SC_valname_any);
	block=iblock;
	block->Set_isfunctionblock(ifuncname);
}

TSC_script_customfunc::~TSC_script_customfunc()
{
	if (block!=NULL) delete block;
}

void TSC_script_customfunc::execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	block->resetvars();
	block->Set_returnvalue(NULL);
	block->exec_start();
	for (int i=0; i<G_varcount(); i++)
	{
		if (arglist->G_ispresent(i))
		{
			TSC_value *val=new TSC_value;
			val->copyfrom_flexibletype(arglist->get(i));
			//val->encapsulate(arglist->get(i));
			block->addvar(G_varname(i),val);
		}
	}
	while ((!block->exec_isfinished())&&(block->G_returnvalue()==NULL))
	{
		block->exec_stepone();
	}
	if (block->G_returnvalue()!=NULL)
		retval->copyfrom_flexibletype(block->G_returnvalue());
}
