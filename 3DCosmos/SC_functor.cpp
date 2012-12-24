#include "stdafx.h"
#include "vecmath.h"

#include "SC_env.h"
#include "SC_script.h"
#include "SC_expr.h"
#include "SC_customfunc.h"

#include "SC_functor.h"


TSC_functor::TSC_functor()
{
	tpe=0;
	block=NULL;
}

TSC_functor::~TSC_functor()
{
	if (block!=NULL) delete block;
}


void TSC_functor::tostring(QString &str, int tpe)
{
	str=description;
}

void TSC_functor::fromstring(StrPtr str)
{
	ASSERT(false);
}


void TSC_functor::operator=(const TSC_functor &v)
{
	ASSERT(false);
	throw QError(_text("Unable to copy functors"));
}

void TSC_functor::set_function(TSC_func *ifunc)
{
	tpe=1;
	functionref.setreffor(ifunc);
}


void TSC_functor::parse(TSC_script *iscript, StrPtr content)
{
	tpe=2;
	argnames.reset();
	if (block!=NULL) delete block;block=NULL;

	block=new TSC_script_block(iscript,iscript->G_root());

	description=content;


	Tscriptsourcepart blocksource;
	Tintarray lines,cols;
	blocksource.txt=content;
	for (int i=0; i<qstrlen(content); i++)
	{
		lines.add(0);cols.add(i);
	}
	blocksource.linenr=lines.G_ptr();
	blocksource.colnr=cols.G_ptr();
	blocksource.count=lines.G_count();

	Tscriptsourcerange range;
	blocksource.getrange(range,0,blocksource.count-1);
	TSC_script_cmd_expression *exprcmd=new TSC_script_cmd_expression(block,range);
	block->addcommand(exprcmd);
	if (!exprcmd->parse(blocksource))
	{
		QString err;
		FormatString(err,_text("Error while compiling functor:\n^1"),iscript->G_error());
		throw QError(err);
	}
}

void TSC_functor::addargumentname(StrPtr argname)
{
	argnames.add(new QString(argname));
	if (polycomps.G_count()>0) throw QError(_text("Invalid functor operation"));
	if ((tpe==3)&&(argnames.G_count()>3)) throw QError(_text("Polynomial functors are restricted to 3 arguments"));
}

void TSC_functor::init_polynomial()
{
	tpe=3;
	polycomps.reset();
	argnames.reset();
	argpowers[0].reset();argpowers[0].add(1.0);
	argpowers[1].reset();argpowers[1].add(1.0);
	argpowers[2].reset();argpowers[2].add(1.0);
}

void TSC_functor::add_polynomial(double coef, int deg1, int deg2, int deg3)
{
	if (tpe!=3) throw QError(_text("Invalid polynomial type"));
	TSC_functor_polycomp *comp=new TSC_functor_polycomp;
	comp->coef=coef;
	comp->degs[0]=deg1;
	comp->degs[1]=deg2;
	comp->degs[2]=deg3;
	polycomps.add(comp);
	while (argpowers[0].G_count()<=deg1) argpowers[0].add(0);
	while (argpowers[1].G_count()<=deg2) argpowers[1].add(0);
	while (argpowers[2].G_count()<=deg3) argpowers[2].add(0);
}


TSC_script_cmd_expression* TSC_functor::G_expression()
{
	if (tpe!=2) throw QError(_text("Invalid functor type"));
	if (block==NULL) throw QError(_text("Functor is not initialized"));
	TSC_script_cmd *cmd=block->G_command(0);
	if (cmd==NULL) throw QError(_text("Functor command is not initialized"));
	if (qstricmp(cmd->G_tpe(),_qstr("expression"))!=0) throw QError(_text("Invalid functor command"));
	TSC_script_cmd_expression *expr=(TSC_script_cmd_expression*)cmd;
	return expr;
}

void TSC_functor::eval(TSC_funcarglist *arglist, TSC_value *retval)
{
	if (tpe==1)
	{
		TSC_func *func=functionref.G_ref();
		if (func==NULL) throw QError(_text("Functor does not point to a valid function"));
		func->execute(arglist,retval,NULL,NULL);
		return;
	}

	if (tpe==2)
	{
		TSC_script_cmd_expression *expr=G_expression();
		QString argname;
		for (int i=0; i<arglist->G_count(); i++)
		{
			if (i<argnames.G_count()) argname=*argnames[i];
			else argname=_qstr("a")+QString(i+1);
			TSC_script_var *var=block->addvar(argname,NULL);
			if (var==NULL) throw QError(_text("Could not create functor variable"));
			var->copy_value(arglist->get(i));
		}

		expr->exec_start();
		while (!expr->exec_isfinished()) expr->exec_stepone();
		retval->copyfrom(expr->G_value());
		return;
	}

	if (tpe==3)
	{
		double a;
		if (arglist->G_count()<=0) return;

		a=arglist->get(0)->G_content_scalar()->G_val();
		for (int i=1; i<argpowers[0].G_count(); i++)
			argpowers[0][i]=a*argpowers[0][i-1];

		if (arglist->G_count()>=1)
		{
			a=arglist->get(1)->G_content_scalar()->G_val();
			for (int i=1; i<argpowers[0].G_count(); i++)
				argpowers[1][i]=a*argpowers[1][i-1];
		}

		if (arglist->G_count()>=2)
		{
			a=arglist->get(2)->G_content_scalar()->G_val();
			for (int i=1; i<argpowers[0].G_count(); i++)
				argpowers[2][i]=a*argpowers[2][i-1];
		}

		double rs=0;
		for (int i=0; i<polycomps.G_count(); i++)
		{
			rs+=polycomps[i]->coef
				*argpowers[0][polycomps[i]->degs[0]]
				*argpowers[1][polycomps[i]->degs[1]]
				*argpowers[2][polycomps[i]->degs[2]] ;
		}

		retval->createtype(GetTSCenv().G_datatype(SC_valname_scalar));
		retval->copyfrom(rs);
		return;
	}

	throw QError(_text("Unassigned functor evaluation"));
}

void TSC_functor::createstring(QString &str)
{
	str.clear();
	TSC_script_cmd_expression *expr=G_expression();
	expr->G_root()->createstring(NULL,str);
}



void TSC_functor::streamout(QBinTagWriter &writer)
{
	throw QError(_text("Unable to stream functors"));
}
void TSC_functor::streamin(QBinTagReader &reader)
{
	throw QError(_text("Unable to stream functors"));
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(functor_functor,Functor)
{
	setreturntype(SC_valname_functor);
	setflexiblearglist();
	setclasspath_fromtype(SC_valname_functor);
	addvar(_qstr("function"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr content=arglist->get(0)->G_content_string()->G_string();
	retval->create();
	TSC_functor *functor=G_valuecontent<TSC_functor>(retval);
	functor->parse(arglist->G_parentblock()->G_script(),content);

	for (int i=1; i<arglist->G_count(); i++)
		functor->addargumentname(arglist->get(i)->G_content_string()->G_string());

}
ENDFUNCTION(functor_functor)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(functor_functionfunctor,FunctionFunctor)
{
	setreturntype(SC_valname_functor);
	setflexiblearglist();
	setclasspath_fromtype(SC_valname_functor);
	addvar(_qstr("functionname"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr funcname=arglist->get(0)->G_content_string()->G_string();
	TSC_func *func=arglist->G_parentblock()->G_customfunction(funcname);
	if (func==NULL) throw QError(_text("Invalid function name for functor assignment"));

	retval->create();
	TSC_functor *functor=G_valuecontent<TSC_functor>(retval);
	functor->set_function(func);

	for (int i=1; i<arglist->G_count(); i++)
		functor->addargumentname(arglist->get(i)->G_content_string()->G_string());

}
ENDFUNCTION(functor_functionfunctor)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(functor_polyfunctor,PolynomialFunctor)
{
	setreturntype(SC_valname_functor);
	setflexiblearglist();
	setclasspath_fromtype(SC_valname_functor);
	addvar(_qstr("argument"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->create();
	TSC_functor *functor=G_valuecontent<TSC_functor>(retval);
	functor->init_polynomial();

	for (int i=0; i<arglist->G_count(); i++)
		functor->addargumentname(arglist->get(i)->G_content_string()->G_string());

}
ENDFUNCTION(functor_polyfunctor)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(functor_polyfunctoraddcomp,AddPolynomialComponent)
{
	setmemberfunction(SC_valname_functor);
	addvar(_qstr("coefficient"),SC_valname_scalar);
	addvar(_qstr("degree1"),SC_valname_scalar);
	addvar(_qstr("degree2"),SC_valname_scalar,false);
	addvar(_qstr("degree3"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_functor *functor=G_valuecontent<TSC_functor>(owner);
	double coef=arglist->get(0)->G_content_scalar()->G_val();
	int deg1=arglist->get(1)->G_content_scalar()->G_intval();
	int deg2=0;
	int deg3=0;
	if (arglist->G_ispresent(2)) deg2=arglist->get(2)->G_content_scalar()->G_intval();
	if (arglist->G_ispresent(3)) deg3=arglist->get(3)->G_content_scalar()->G_intval();
	functor->add_polynomial(coef,deg1,deg2,deg3);
}
ENDFUNCTION(functor_polyfunctoraddcomp)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(functor_eval,eval)
{
	setmemberfunction(SC_valname_functor);
	setreturntype(SC_valname_any);
	setflexiblearglist();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
//	arglist->G_parentblock()->fin
	TSC_functor *functor=G_valuecontent<TSC_functor>(owner);
	functor->eval(arglist,retval);

}
ENDFUNCTION(functor_eval)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(functor_derive,derive)
{
	setmemberfunction(SC_valname_functor);
	setreturntype(SC_valname_functor);
	addvar(_qstr("varname"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_functor *functor=G_valuecontent<TSC_functor>(owner);
	StrPtr varname=arglist->get(0)->G_content_string()->G_string();
	throw QError(_text("Derive is not yet implemented"));

}
ENDFUNCTION(functor_derive)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(functor_getstring,getstring)
{
	setmemberfunction(SC_valname_functor);
	setreturntype(SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QString str;
	G_valuecontent<TSC_functor>(owner)->createstring(str);
	retval->G_content_string()->fromstring(str);

}
ENDFUNCTION(functor_getstring)