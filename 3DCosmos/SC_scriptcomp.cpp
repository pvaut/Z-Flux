

#include "stdafx.h"
#include "SC_script.h"
#include "SC_scriptcomp.h"

int scansource(Tscriptsourcepart &source, int startps, StrPtr keyword)
{
	bool instring=false;
	int keywordlen=qstrlen(keyword);
	int len=source.count-keywordlen-2;
	for (int ps=startps; ps<len; ps++)
	{
		if ((isquote(source[ps]))&&((ps==0)||(source[ps-1]!=ESCCHAR))) instring=!instring;
		if ((source[ps]==' ')&&(qstrnicmp(&source.txt[ps+1],keyword,keywordlen)==0)&&(source[ps+1+keywordlen]==' ')) return ps+1;
	}
	return -1;
}


//***************************************************************
// TSC_script_cmd_try
//***************************************************************


TSC_script_cmd_try::TSC_script_cmd_try(TSC_script_block *iblock, Tscriptsourcerange &irange) 
: TSC_script_cmd(iblock,irange)
{
	subblock=NULL;
}

TSC_script_cmd_try::~TSC_script_cmd_try()
{
	reset();
}

void TSC_script_cmd_try::reset()
{
	if (subblock!=NULL) delete subblock;subblock=NULL;
}


bool TSC_script_cmd_try::parse(Tscriptsourcepart &source)
{
	reset();
	subblock=new TSC_script_block(parentblock->G_script(),parentblock);
	if (!subblock->compile(source)) return false;
	return true;
}

void TSC_script_cmd_try::exec_start()
{
	ASSERT(G_parentblock()!=NULL);
	ASSERT(G_parentblock()->G_script()!=NULL);
	G_parentblock()->G_script()->Set_catchederror(_qstr(""));
	failed=false;
	subblock->exec_start();
}

void TSC_script_cmd_try::exec_stepone()
{
	QString error;
	try{
		subblock->exec_stepone();
		return;
	}
	catch (TSC_runerror &err)
	{
		error=err.G_content();
	}
	catch(QError &err)
	{
		error=err.G_content();
	}
	G_parentblock()->G_script()->Set_catchederror(error);
	failed=true;
}

bool TSC_script_cmd_try::exec_isfinished()
{
	if (failed) return true;
	return subblock->exec_isfinished();
}


//***************************************************************
// TSC_script_cmd_while
//***************************************************************


TSC_script_cmd_while::TSC_script_cmd_while(TSC_script_block *iblock, Tscriptsourcerange &irange) 
: TSC_script_cmd(iblock,irange)
{
	expr=NULL;
	subblock=NULL;
}

TSC_script_cmd_while::~TSC_script_cmd_while()
{
	reset();
}

void TSC_script_cmd_while::reset()
{
	if (expr!=NULL) delete expr;expr=NULL;
	if (subblock!=NULL) delete subblock;subblock=NULL;
}


bool TSC_script_cmd_while::parse(Tscriptsourcepart &source)
{
	reset();
	int ps=scansource(source,0,_qstr("do"));
	if (ps<0)
	{
		parentblock->G_script()->Set_error(source,0,source.count-1,_text("Missing keyword 'do'"));
		return false;
	}

	Tscriptsourcepart sourcepart_subblock(source,ps+3,source.count-1);
	subblock=new TSC_script_block(parentblock->G_script(),parentblock);
	if (!subblock->compile(sourcepart_subblock)) return false;

	Tscriptsourcepart sourcepart_expr(source,0,ps-1);
	Tscriptsourcerange range;
	sourcepart_expr.getrange(range,0,sourcepart_expr.count-1);
	expr=new TSC_script_cmd_expression(subblock,range);
	if (!expr->parse(sourcepart_expr)) return false;

	return true;
}

void TSC_script_cmd_while::exec_start()
{
	expr_evaluated=false;
	expr_istrue=false;
	expr->exec_start();
}

void TSC_script_cmd_while::exec_stepone()
{
	if (!expr_evaluated)
	{
		expr_evaluated=true;
		while (!expr->exec_isfinished()) expr->exec_stepone();
		TSC_value *value=expr->G_value();
		if (value==NULL) throw TSC_runerror(expr->G_range(),_text("Statement does not evaluate to an expression"));
		expr_istrue=value->G_content_boolean()->G_val();
		if (expr_istrue) subblock->exec_start();
	}
	else
	{
		if (expr_istrue)
		{
			subblock->exec_stepone();
			if (subblock->exec_isfinished())
			{
				expr_evaluated=false;
				expr->exec_start();
			}
		}
	}
}

bool TSC_script_cmd_while::exec_isfinished()
{
	if (!expr_evaluated) return false;
	if (!expr_istrue) return true;
	return subblock->exec_isfinished();
}



//***************************************************************
// TSC_script_cmd_for
//***************************************************************


TSC_script_cmd_for::TSC_script_cmd_for(TSC_script_block *iblock, Tscriptsourcerange &irange) 
: TSC_script_cmd(iblock,irange)
{
	expr_assign=NULL;
	expr_test=NULL;
	subblock=NULL;
}

TSC_script_cmd_for::~TSC_script_cmd_for()
{
	reset();
}

void TSC_script_cmd_for::reset()
{
	if (expr_assign!=NULL) delete expr_assign;expr_assign=NULL;
	if (expr_test!=NULL) delete expr_test;expr_test=NULL;
	if (subblock!=NULL) delete subblock;subblock=NULL;
}


bool TSC_script_cmd_for::parse(Tscriptsourcepart &source)
{
	reset();
	int ps1=scansource(source,0,_qstr("to"));
	if (ps1<0)
	{
		parentblock->G_script()->Set_error(source,0,source.count-1,_text("Missing keyword 'to'"));
		return false;
	}
	int ps2=scansource(source,ps1+2,_qstr("do"));
	if (ps2<0)
	{
		parentblock->G_script()->Set_error(source,0,source.count-1,_text("Missing keyword 'do'"));
		return false;
	}

	Tscriptsourcepart sourcepart_subblock(source,ps2+3,source.count-1);
	subblock=new TSC_script_block(parentblock->G_script(),parentblock);
	if (!subblock->compile(sourcepart_subblock)) return false;

	Tscriptsourcerange range;

	Tscriptsourcepart sourcepart_expr_assign(source,0,ps1-1);
	sourcepart_expr_assign.getrange(range,0,sourcepart_expr_assign.count-1);
	expr_assign=new TSC_script_cmd_expression(G_parentblock(),range);
	if (!expr_assign->parse(sourcepart_expr_assign)) return false;

	Tscriptsourcepart sourcepart_expr_test(source,ps1+2,ps2-1);
	sourcepart_expr_test.getrange(range,0,sourcepart_expr_test.count-1);
	expr_test=new TSC_script_cmd_expression(G_parentblock(),range);
	if (!expr_test->parse(sourcepart_expr_test)) return false;

	return true;
}

void TSC_script_cmd_for::exec_start()
{
	varname.clear();
	expr_evaluated=0;startval=0;endval=1;curval=0;
	expr_assign->exec_start();
}

void TSC_script_cmd_for::exec_stepone()
{
	if (!expr_evaluated)
	{
		expr_evaluated=true;
		while (!expr_assign->exec_isfinished()) expr_assign->exec_stepone();
		varname=expr_assign->G_root()->G_assign_varname();
		if (qstrlen(varname)<=0) throw TSC_runerror(range,_qstr("Missing or invalid FOR index variable"));
		indexvar=subblock->findvar(this,varname);
		if (indexvar==NULL) throw TSC_runerror(range,_qstr("Could not find FOR index variable"));
		startval=indexvar->G_value()->G_content_scalar()->G_intval();
		expr_test->exec_start();while (!expr_test->exec_isfinished()) expr_test->exec_stepone();
		endval=expr_test->G_value()->G_content_scalar()->G_intval();
		curval=startval;
		if (curval<=endval) subblock->exec_start();
	}
	else
	{
		if (curval>endval) return;
		if (!subblock->exec_isfinished())
		{
			subblock->exec_stepone();
		}
		else
		{
			curval++;
			if (curval<=endval) subblock->exec_start();
			indexvar->G_value()->G_content_scalar()->copyfrom(curval);
		}
	}
}

bool TSC_script_cmd_for::exec_isfinished()
{
	if (!expr_evaluated) return false;
	if (curval<=endval) return false;
	return true;
}




//***************************************************************
// TSC_script_cmd_forall
//***************************************************************


TSC_script_cmd_forall::TSC_script_cmd_forall(TSC_script_block *iblock, Tscriptsourcerange &irange) 
: TSC_script_cmd(iblock,irange)
{
	expr_list=NULL;
	subblock=NULL;
	lst=NULL;
}

TSC_script_cmd_forall::~TSC_script_cmd_forall()
{
	reset();
}

void TSC_script_cmd_forall::reset()
{
	if (expr_list!=NULL) delete expr_list;expr_list=NULL;
	if (subblock!=NULL) delete subblock;subblock=NULL;
	lst=NULL;
}


bool TSC_script_cmd_forall::parse(Tscriptsourcepart &source)
{
	reset();
	int ps1=scansource(source,0,_qstr("in"));
	if (ps1<0)
	{
		parentblock->G_script()->Set_error(source,0,source.count-1,_text("Missing keyword 'in'"));
		return false;
	}
	int ps2=scansource(source,ps1+2,_qstr("do"));
	if (ps2<0)
	{
		parentblock->G_script()->Set_error(source,0,source.count-1,_text("Missing keyword 'do'"));
		return false;
	}

	Tscriptsourcepart sourcepart_subblock(source,ps2+3,source.count-1);
	subblock=new TSC_script_block(parentblock->G_script(),parentblock);
	if (!subblock->compile(sourcepart_subblock)) return false;

	Tscriptsourcerange range;

	Tscriptsourcepart sourcepart_expr_varname(source,0,ps1-1);
	sourcepart_expr_varname.getrange(range,0,sourcepart_expr_varname.count-1);
/*	expr_assign=new TSC_script_cmd_expression(G_parentblock(),range);
	if (!expr_assign->parse(sourcepart_expr_assign)) return false;*/
	sourcepart_expr_varname.filltext(varname);
	while (varname[varname.G_length()-1]==' ') varname.substring(0,varname.G_length()-2);

	Tscriptsourcepart sourcepart_expr_list(source,ps1+2,ps2-1);
	sourcepart_expr_list.getrange(range,0,sourcepart_expr_list.count-1);
	expr_list=new TSC_script_cmd_expression(G_parentblock(),range);
	if (!expr_list->parse(sourcepart_expr_list)) return false;

	return true;
}

void TSC_script_cmd_forall::exec_start()
{
	expr_evaluated=false;curidx=0;
	expr_list->exec_start();
	lst=NULL;
}

void TSC_script_cmd_forall::exec_stepone()
{
	if (!expr_evaluated)
	{
		expr_evaluated=true;
		expr_list->exec_start();while (!expr_list->exec_isfinished()) expr_list->exec_stepone();
		lst=G_valuecontent<TSC_list>(expr_list->G_value());
		curidx=0;
		if (lst->G_size()>0)
		{
			subblock->exec_start();
			subblock->addvar(varname,NULL)->encapsulate(lst->get(curidx));
		}
	}
	else
	{
		if (lst==NULL) throw QError(_text("Forall not initialised"));
		if (curidx>=lst->G_size()) return;
		if (!subblock->exec_isfinished())
		{
			subblock->exec_stepone();
		}
		else
		{
			curidx++;
			if (curidx<lst->G_size())
			{
				subblock->exec_start();
				subblock->addvar(varname,NULL)->encapsulate(lst->get(curidx));
				//subblock->G_varset()->findvar_required(varname)->G_value()->copyfrom(lst->get(curidx));
			}
		}
	}
}

bool TSC_script_cmd_forall::exec_isfinished()
{
	if (!expr_evaluated) return false;
	if (lst==NULL) throw QError(_text("Forall not initialised"));
	if (curidx<lst->G_size()) return false;
	return true;
}



//***************************************************************
// TSC_script_insertcodeblock
//***************************************************************


TSC_script_insertcodeblock::TSC_script_insertcodeblock(TSC_script_block *iblock, Tscriptsourcerange &irange) 
: TSC_script_cmd(iblock,irange)
{
	expr=NULL;
	subblock=NULL;
}

TSC_script_insertcodeblock::~TSC_script_insertcodeblock()
{
	reset();
}

void TSC_script_insertcodeblock::reset()
{
	if (expr!=NULL) delete expr;expr=NULL;
	if (subblock!=NULL) delete subblock;subblock=NULL;
}


bool TSC_script_insertcodeblock::parse(Tscriptsourcepart &source)
{
	reset();

	Tscriptsourcerange range;
	source.getrange(range,0,source.count-1);

	expr=new TSC_script_cmd_expression(G_parentblock(),range);
	if (!expr->parse(source)) return false;
	return true;
}

void TSC_script_insertcodeblock::exec_start()
{
	expr_evaluated=0;
	expr->exec_start();
}

void TSC_script_insertcodeblock::exec_stepone()
{
	if (!expr_evaluated)
	{
		expr_evaluated=1;
		while (!expr->exec_isfinished()) expr->exec_stepone();
		TSC_value *val=expr->G_value();
		if (val==NULL) throw TSC_runerror(expr->G_range(),_text("expression does not return a value"));
		if (!val->istype(SC_valname_string)) throw TSC_runerror(expr->G_range(),_text("expression does not return a string"));
		QString str0=val->G_content_string()->G_string();

//		str0.replace(_qstr("\r"),_qstr("\r"));
//		str0.replace(_qstr("\n"),_qstr(" "));

		//process block to remove comments
		QString str;
		WCHAR ch;
		while (str0.G_length()>0)
		{
			QString line;
			str0.splitstring(_qstr("\n"),line);

			bool instr=false;
			bool incomment=false;
			for (int colnr=0; colnr<line.G_length(); colnr++)
			{
				ch=line.G_char(colnr);
				if ((!incomment)&&(!instr)&&(ch=='#')/*&&(line->G_char(colnr+1)=='/')*/) incomment=true;
				if (!incomment)
				{
					if (isquote(ch)) instr=!instr;
					str+=(StrPtr)CString(ch);
				}
			}
			str+=_qstr(" ");
		}


		Tscriptsourcepart blocksource;
		Tintarray lines,cols;
		blocksource.txt=str;
		for (int i=0; i<str.G_length(); i++)
		{
			lines.add(range.line1);cols.add(i);
		}
		blocksource.linenr=lines.G_ptr();
		blocksource.colnr=cols.G_ptr();
		blocksource.count=lines.G_count();

		if (subblock!=NULL) delete subblock;
		subblock=new TSC_script_block(parentblock->G_script(),parentblock);
		subblock->Set_isruntimecode();

		if (!subblock->compile(blocksource))
		{
			QString err;
			FormatString(err,_text("Error while compiling code block:\n^1"),parentblock->G_script()->G_error());
			throw TSC_runerror(expr->G_range(),err);
		}
		try{
			subblock->exec_start();
		}
		catch (TSC_runerror err)
		{
			QString newerr;
			FormatString(newerr,_text("Runtime error in code block:\n^1"),err.G_content());
			throw TSC_runerror(expr->G_range(),newerr);
		}
		catch (QError &err)
		{
			QString newerr;
			FormatString(newerr,_text("Runtime error in code block:\n^1"),err.G_content());
			throw TSC_runerror(expr->G_range(),newerr);
		}
	}
	else
	{
		try{
			subblock->exec_stepone();
		}
		catch (TSC_runerror err)
		{
			QString newerr;
			FormatString(newerr,_text("Runtime error in code block:\n^1"),err.G_content());
			throw TSC_runerror(expr->G_range(),newerr);
		}
		catch (QError &err)
		{
			QString newerr;
			FormatString(newerr,_text("Runtime error in code block:\n^1"),err.G_content());
			throw TSC_runerror(expr->G_range(),newerr);
		}
	}
}

bool TSC_script_insertcodeblock::exec_isfinished()
{
	if (!expr_evaluated) return false;
	return subblock->exec_isfinished();
}



//***************************************************************
// TSC_script_cmd_if
//***************************************************************


TSC_script_cmd_if::TSC_script_cmd_if(TSC_script_block *iblock, Tscriptsourcerange &irange) 
: TSC_script_cmd(iblock,irange)
{
	expr_test=NULL;
	subblock=NULL;
}

TSC_script_cmd_if::~TSC_script_cmd_if()
{
	reset();
}

void TSC_script_cmd_if::reset()
{
	if (expr_test!=NULL) delete expr_test;expr_test=NULL;
	if (subblock!=NULL) delete subblock;subblock=NULL;
}


bool TSC_script_cmd_if::parse(Tscriptsourcepart &source)
{
	reset();
	int ps=scansource(source,0,_qstr("then"));
	if (ps<0)
	{
		parentblock->G_script()->Set_error(source,0,source.count-1,_text("Missing keyword 'then'"));
		return false;
	}

	Tscriptsourcepart sourcepart_subblock(source,ps+5,source.count-1);
	subblock=new TSC_script_block(parentblock->G_script(),parentblock);
	if (!subblock->compile(sourcepart_subblock)) return false;

	Tscriptsourcerange range;

	Tscriptsourcepart sourcepart_expr_test(source,0,ps-1);
	sourcepart_expr_test.getrange(range,0,sourcepart_expr_test.count-1);
	expr_test=new TSC_script_cmd_expression(G_parentblock(),range);
	if (!expr_test->parse(sourcepart_expr_test)) return false;

	return true;
}

void TSC_script_cmd_if::exec_start()
{
	expr_evaluated=0;expr_istrue=0;
	expr_test->exec_start();
}

void TSC_script_cmd_if::exec_stepone()
{
	if (!expr_evaluated)
	{
		expr_evaluated=1;
		while (!expr_test->exec_isfinished()) expr_test->exec_stepone();
		expr_istrue=expr_test->G_value()->G_content_boolean()->G_val();
		if (expr_istrue) subblock->exec_start();
	}
	else
	{
		if (expr_istrue) subblock->exec_stepone();
	}
}

bool TSC_script_cmd_if::exec_isfinished()
{
	if (!expr_evaluated) return false;
	if (!expr_istrue) return true;
	return subblock->exec_isfinished();
}



//***************************************************************
// TSC_script_cmd_else
//***************************************************************


TSC_script_cmd_else::TSC_script_cmd_else(TSC_script_block *iblock, Tscriptsourcerange &irange, TSC_script_cmd_if *i_iftest) 
: TSC_script_cmd(iblock,irange)
{
	subblock=NULL;
	iftest=i_iftest;
}

TSC_script_cmd_else::~TSC_script_cmd_else()
{
	reset();
}

void TSC_script_cmd_else::reset()
{
	if (subblock!=NULL) delete subblock;subblock=NULL;
}


bool TSC_script_cmd_else::parse(Tscriptsourcepart &source)
{
	reset();

	Tscriptsourcepart sourcepart_subblock(source,0,source.count-1);
	subblock=new TSC_script_block(parentblock->G_script(),parentblock);
	if (!subblock->compile(sourcepart_subblock)) return false;
	return true;
}

void TSC_script_cmd_else::exec_start()
{
	if (iftest->G_expr_istrue()) return;
	subblock->exec_start();
}

void TSC_script_cmd_else::exec_stepone()
{
	if (iftest->G_expr_istrue()) return;
	if (!subblock->exec_isfinished())
	{
		subblock->exec_stepone();
	}
}

bool TSC_script_cmd_else::exec_isfinished()
{
	if (iftest->G_expr_istrue()) return true;
	return subblock->exec_isfinished();
}
