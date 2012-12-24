

#include "stdafx.h"

#include "qxwin.h"

#include "SC_script.h"
#include "SC_scriptcomp.h"
#include "SC_customfunc.h"

#include "3Dscene.h"

//***************************************************************
// TSC_script_var
//***************************************************************

TSC_script_var::TSC_script_var(StrPtr iname, TSC_value *ivalue)
{
	name=iname;
	value=ivalue;
}

TSC_script_var::~TSC_script_var()
{
	if (value!=NULL) delete value;
}

void TSC_script_var::copy_value(TSC_value *ivalue)
{
	if (value==NULL) value=ivalue->duplicate();
	value->copyfrom(ivalue);
}

void TSC_script_var::encapsulate(TSC_value *ivalue)
{
	if (value==NULL) value=new TSC_value;
	value->encapsulate(ivalue);
}




//***************************************************************
// TSC_script_varset
//***************************************************************

TSC_script_var* TSC_script_varset::addvar(StrPtr iname, TSC_value *icontent)
{
	TSC_script_var *var=findvar(NULL,iname);
	if (var!=NULL) return var;
	var=new TSC_script_var(iname,icontent);
	vars.add(var);
	index.add(iname,vars.G_count()-1);
	return var;
}

void TSC_script_varset::reset()
{
	vars.reset();
	index.reset();
}

TSC_script_var* TSC_script_varset::findvar(const TSC_ExpressionCacher *cache, StrPtr varname)
{
	//if ((cache!=NULL)&&(cache->cached_var!=NULL)&&(issame(varname,cache->cached_var->G_name())))
	//	return cache->cached_var;
	int nr=index.get(varname);
	if ((nr<0)||(nr>=vars.G_count())) return NULL;
	//if (cache!=NULL)
	//	cache->cached_var=vars[nr];
	return vars[nr];
}

TSC_script_var* TSC_script_varset::findvar_required(const TSC_ExpressionCacher *cache, StrPtr varname)
{
	TSC_script_var *vr=findvar(cache,varname);
	if (vr==NULL) throw QError(TFormatString(_text("Unable to find variable ^1"),varname));
	return vr;
}


//***************************************************************
// TSC_script_cmd
//***************************************************************


TSC_script_var* TSC_script_cmd::findvar(const TSC_ExpressionCacher *cache, StrPtr varname)
{
	if (parentblock==NULL) return NULL;
	return parentblock->findvar(cache,varname);
}

void TSC_script_cmd::GetDescription(QString &str)
{
	str+=_qstr("Type=");
	str=G_tpe();
	str+=_qstr(" Content=");
	str+=descriptioncontent;
	//parentblock->G_script()->G_source().
}


//***************************************************************
// TSC_script_cmd_expression
//***************************************************************


TSC_script_cmd_expression::TSC_script_cmd_expression(TSC_script_block *iblock, Tscriptsourcerange &irange) 
: TSC_script_cmd(iblock,irange) , root(this)
{
	retval=NULL;
}


bool TSC_script_cmd_expression::parse(Tscriptsourcepart &source)
{
	source.filltext(descriptioncontent);
	return root.parse(source);
}

TSC_value* TSC_script_cmd_expression::G_value()
{
	return retval;
}


void TSC_script_cmd_expression::exec_start()
{
	executed=false;
	retval=NULL;
	parentblock->G_script()->G_source().Set_debugline(range.line1);
}

void TSC_script_cmd_expression::exec_stepone()
{
//	throw TSC_runerror(range,_text("Unknown"));
	retval=root.execute(NULL);
	executed=true;
}

bool TSC_script_cmd_expression::exec_isfinished()
{ 
	return executed;
}



//***************************************************************
// TSC_script_block
//***************************************************************

TSC_script_block::TSC_script_block(TSC_script *iscript, TSC_script_block *iparentblock)
{
	curcommand=-1;
	script=iscript;
	parentblock=iparentblock;
	isruntimecode=false;
	isfunctionblock=false;
	returnvalue=NULL;
}

TSC_script_block::~TSC_script_block()
{
	reset();
}


void TSC_script_block::reset()
{
	curcommand=-1;
	varset.reset();
	customfunctions.reset();
	commands.reset();
}

void TSC_script_block::resetvars()
{
	varset.reset();
}

TSC_script_var* TSC_script_block::addvar(StrPtr iname, TSC_value *icontent)
{
	return varset.addvar(iname,icontent);
}

void TSC_script_block::addcommand(TSC_script_cmd *icommand)
{
	commands.add(icommand);
}


bool TSC_script_block::parse_expression(Tscriptsourcepart &source)
{
	Tscriptsourcerange range;
	source.getrange(range,0,source.count-1);
	TSC_script_cmd_expression *expr=new TSC_script_cmd_expression(this,range);
	addcommand(expr);
	return expr->parse(source);
}


bool TSC_script_block::parse_try(Tscriptsourcepart &source)
{
	Tscriptsourcerange range;
	source.getrange(range,0,source.count-1);
	TSC_script_cmd_try *it=new TSC_script_cmd_try(this,range);
	addcommand(it);
	return it->parse(source);
}


bool TSC_script_block::parse_while(Tscriptsourcepart &source)
{
	Tscriptsourcerange range;
	source.getrange(range,0,source.count-1);
	TSC_script_cmd_while *it=new TSC_script_cmd_while(this,range);
	addcommand(it);
	return it->parse(source);
}

bool TSC_script_block::parse_for(Tscriptsourcepart &source)
{
	Tscriptsourcerange range;
	source.getrange(range,0,source.count-1);
	TSC_script_cmd_for *it=new TSC_script_cmd_for(this,range);
	addcommand(it);
	return it->parse(source);
}

bool TSC_script_block::parse_forall(Tscriptsourcepart &source)
{
	Tscriptsourcerange range;
	source.getrange(range,0,source.count-1);
	TSC_script_cmd_forall *it=new TSC_script_cmd_forall(this,range);
	addcommand(it);
	return it->parse(source);
}


bool TSC_script_block::parse_if(Tscriptsourcepart &source)
{
	Tscriptsourcerange range;
	source.getrange(range,0,source.count-1);
	TSC_script_cmd_if *it=new TSC_script_cmd_if(this,range);
	addcommand(it);
	return it->parse(source);
}

bool TSC_script_block::parse_else(Tscriptsourcepart &source)
{
	Tscriptsourcerange range;
	source.getrange(range,0,source.count-1);
	if ((commands.G_count()==0)||(qstricmp(commands[commands.G_count()-1]->G_tpe(),_qstr("then"))!=0))
	{
		G_script()->Set_error(source,0,source.count-1,_text("ELSE without IF"));
		return false;
	}
	TSC_script_cmd_else *it=new TSC_script_cmd_else(this,range,(TSC_script_cmd_if*)commands[commands.G_count()-1]);
	addcommand(it);
	return it->parse(source);
	return true;
}

bool TSC_script_block::parse_function(Tscriptsourcepart &source)
{
	Tscriptsourcerange range;
	source.getrange(range,0,source.count-1);

	int ps1=0;
	while ((ps1<source.count)&&(source[ps1]!='(')) ps1++;
	if (source[ps1]!='(')
	{
		G_script()->Set_error(source,0,source.count-1,_text("Missing parameter list in function definition"));
		return false;
	}
	if (ps1==0)
	{
		G_script()->Set_error(source,0,source.count-1,_text("Missing function name in function definition"));
		return false;
	}

	QString funcname;
	for (int i=0; i<ps1; i++) funcname+=source[i];

	Tarray<QString> argnames;
	int ps2=ps1+1;
	while ((ps2<source.count)&&(source[ps2]!=')'))
	{
		QString argname;
		while ((ps2<source.count)&&(source[ps2]!=')')&&(Qisalphanumerical(source[ps2])))
		{
			argname+=source[ps2];
			ps2++;
		}
		if ((source[ps2]!=',')&&(source[ps2]!=')'))
		{
			G_script()->Set_error(source,0,source.count-1,_text("Invalid function argument list"));
			return false;
		}
		if (source[ps2]==',') ps2=ps2+1;
		argnames.add(new QString(argname));
	}


	Tscriptsourcepart sourcepart_subblock(source,ps2+1,source.count-1);
	TSC_script_block *subblock=new TSC_script_block(G_script(),this);
	if (!subblock->compile(sourcepart_subblock))
	{
		delete subblock;
		return false;
	}

	TSC_script_customfunc *func=new TSC_script_customfunc(funcname,argnames,subblock);
	addcustomfunction(func);

	return true;
}


void TSC_script_block::addcustomfunction(TSC_script_customfunc *func)
{
	if ((isruntimecode)&&(parentblock!=NULL)) parentblock->addcustomfunction(func);
	else
	{
		int fnd=-1;
		for (int i=0; i<customfunctions.G_count(); i++)
			if (qstricmp(func->G_name(),customfunctions[i]->G_name())==0) fnd=i;
		if (fnd<0) customfunctions.add(func);
		else
		{
			TSC_script_customfunc *oldfunc=customfunctions[fnd];
			customfunctions.set(fnd,func);
			delete oldfunc;
		}
		
	}
}


bool TSC_script_block::parse_insertcodeblock(Tscriptsourcepart &source)
{
	Tscriptsourcerange range;
	source.getrange(range,0,source.count-1);
	TSC_script_insertcodeblock *it=new TSC_script_insertcodeblock(this,range);
	commands.add(it);
	return it->parse(source);
}



int TSC_script_block::splitkeyword(Tscriptsourcepart &source)
{
	int i=0;
	while ((i<source.count)&&
		( ((source[i]>='a')&&(source[i]<='z')) || ((source[i]>='A')&&(source[i]<='Z')) )
		) i++;
	return i;
}


bool TSC_script_block::parsecomp(Tscriptsourcepart &source)
{
	int ps=splitkeyword(source);
	if ((ps>0)&&(ps<source.count-1))
	{
		Tscriptsourcepart subpart(source,ps+1,source.count-1);
		if (qstrnicmp(source.txt,_qstr("try "),4)==0)
			return parse_try(subpart);
		if (qstrnicmp(source.txt,_qstr("while "),6)==0)
			return parse_while(subpart);
		if (qstrnicmp(source.txt,_qstr("for "),4)==0)
			return parse_for(subpart);
		if (qstrnicmp(source.txt,_qstr("foreach "),8)==0)
			return parse_forall(subpart);
		if (qstrnicmp(source.txt,_qstr("if "),3)==0)
			return parse_if(subpart);
		if (qstrnicmp(source.txt,_qstr("else "),5)==0)
			return parse_else(subpart);
		if (qstrnicmp(source.txt,_qstr("function "),9)==0)
			return parse_function(subpart);
		if (qstrnicmp(source.txt,_qstr("codeblock "),10)==0)
			return parse_insertcodeblock(subpart);
	}

	return parse_expression(source);
	return true;
}


bool TSC_script_block::compile(Tscriptsourcepart &source)
{
	bool ok,instring;
	int level3,i,ps,ps2,newps;

	curcommand=-1;
	if (source.count<=0) return true;

	int startps=0;
	int endps=source.count-1;

	while ((startps<endps)&&(source[startps]==' ')) startps++;
	while ((startps<endps)&&(source[endps]==' ')) endps--;

	if ((source[startps]=='{')&&(source[endps]=='}'))// remove surrounding {}
	{
		ok=true;instring=false;level3=0;
		for (i=startps+1; i<endps; i++) 
		{
			if ((isquote(source[i]))&&(source[i-1]!=ESCCHAR)) instring=!instring;
			if ((!instring)&&(source[i]=='{')) level3++;
			if ((!instring)&&(source[i]=='}')) level3--;
			if (level3<0) ok=false;
		}
		if (ok) { startps++; endps--; }
	}

	while ((startps<endps)&&(source[startps]==' ')) startps++;
	while ((startps<endps)&&(source[endps]==' ')) endps--;

	for (ps=startps; ps<=endps; )
	{
		while ((ps<endps)&&(source[ps]==' ')) ps++;
		instring=false;level3=0;ok=false;
		for (ps2=ps; (ps2<=endps)&&(!ok); ps2++)
		{
			if (isquote(source[ps2])) if ((ps2==0)||(source[ps2-1]!=ESCCHAR)) instring=!instring;
			if ((!instring)&&(source[ps2]=='{')) level3++;
			if ((!instring)&&(source[ps2]=='}'))
			{
				level3--;
				if (level3==0) ok=true;
			}
			if (level3<0) { script->Set_error(source,ps2,ps2,_text("Unbalanced {} brackets")); return false; }
			if ((source[ps2]==';')&&(!instring)&&(level3==0)) ok=1;
		}
		if (level3>0) { script->Set_error(source,ps2,ps2,_text("Missing closing bracket }")); return false; }
		if (!ok)
		{
			script->Set_error(source,ps2,ps2,_text("; expected")); 
			return false;
		}
		ps2--;
		if (source[ps2]==';') { newps=ps2+1; }
		if (source[ps2]=='}') { newps=ps2+1; }
		if (ps2>endps) ps2=endps;
		while ((ps2>ps)&&(source[ps2]==' ')) ps2--;

		Tscriptsourcepart subsource(source,ps,ps2);
		if (!parsecomp(subsource)) return false;

		ps=newps;

	}
	return true;
}


TSC_script_var* TSC_script_block::findvar(const TSC_ExpressionCacher *cache, StrPtr varname)
{
	TSC_script_var *var=varset.findvar(cache,varname);
	if (var!=NULL) return var;
	if (parentblock!=NULL) return parentblock->findvar(cache,varname);
	return NULL;
}

void TSC_script_block::GetDescription(QString &str)
{
	str=_qstr("");
	if (isfunctionblock)
	{
		str+=_qstr("Function: ");
		str+=functionname;
	}
	else
	{
		str+=_qstr("Code block");
	}
	str+=_qstr(" ");
	if ((curcommand>=0)&&(curcommand<commands.G_count()))
	{
		QString str2;
		commands[curcommand]->GetDescription(str2);
		str+=str2;
	}
	else
		str+=_qstr("[Not started]");
}


void TSC_script_block::exec_start()
{
	varset.reset();
	curcommand=0;
	if (commands.G_count()>0)
	{
		commands[curcommand]->exec_start();
		script->Set_debug_currentblock(this);
		script->G_source().Set_debugline(commands[curcommand]->G_range().line1);
	}
}

void TSC_script_block::exec_stop()
{
	curcommand=-1;
	script->G_source().Set_debugline(-1);
	if (parentblock!=NULL) parentblock->exec_stop();
}




void TSC_script_block::exec_stepone()
{
	script->Set_debug_currentblock(this);
	if (!script->exec_isstarted()) curcommand=-1;
	if ((curcommand>=commands.G_count())||(curcommand<0)) return;
	if ((!exec_isfinished())&&(!commands[curcommand]->exec_isfinished())) commands[curcommand]->exec_stepone();
	if ((curcommand>=0)&&(curcommand<commands.G_count())&&(commands[curcommand]->exec_isfinished()))
	{
		curcommand++;
		if (curcommand<commands.G_count())
			commands[curcommand]->exec_start();
	}
	//script->Set_debug_currentblock(this); weggenomen 2009-02-15 om locale variabelen binnen een blok te kunnen zien
}

bool TSC_script_block::exec_isstarted()
{
	return curcommand>=0;
}

bool TSC_script_block::exec_isfinished()
{
	if (G_3DCosmos().G_requeststop())
	{
//		G_3DCosmos().Reset_requeststop();???moet dit hier staan?
		return true;
	}
	return (curcommand<0)||(curcommand>=commands.G_count());
}

void TSC_script_block::exec_buildstack(Tarray<QString> &stacklist)
{
	QString str;
	GetDescription(str);
	stacklist.add(new QString(str));
	if (parentblock!=NULL)
		parentblock->exec_buildstack(stacklist);
}


void TSC_script_block::exec_buildvarlist(int curdepth, Tcopyarray<TSC_script_var> &varlist, Tintarray &vardepthlist)
{
	for (int i=0; i<varset.G_varcount(); i++)
	{
		varlist.add(varset.G_var(i));
		vardepthlist.add(curdepth);
	}
	if (parentblock!=NULL)
		parentblock->exec_buildvarlist(curdepth+1,varlist,vardepthlist);
}


TSC_script_customfunc* TSC_script_block::G_customfunction(StrPtr funcname)
{
	for (int funcnr=0; funcnr<customfunctions.G_count(); funcnr++)
		if (qstricmp(funcname,customfunctions[funcnr]->G_name())==0)
			return customfunctions[funcnr];
	if (parentblock!=NULL) return parentblock->G_customfunction(funcname);
	return NULL;
}


bool TSC_script_block::evalfunction(StrPtr funcname,TSC_value *returnval, TSC_value *assigntoval, TSC_funcarglist &arglist, QString &error)
{
	for (int funcnr=0; funcnr<customfunctions.G_count(); funcnr++)
		if (qstricmp(funcname,customfunctions[funcnr]->G_name())==0)
		{
			try{
				customfunctions[funcnr]->execute(&arglist,returnval,assigntoval,NULL);
			}
			catch (QError &ferror)
			{
				error=ferror.G_content();
				return false;
			}
			return true;
		}
	if (parentblock!=NULL) return parentblock->evalfunction(funcname,returnval,assigntoval,arglist,error);
	else return G_script()->G_env()->evalfunction(funcname,returnval,assigntoval,arglist,error);
}



//***************************************************************
// TSC_script
//***************************************************************

TSC_script::TSC_script(TSC_env *ienv) : root(this,NULL)
{
	QString str;

	env=ienv;
	curdebugblock=NULL;

	TQXColor cl1=TQXColor(1,0,1)+2*G_QXSys().G_color(TQXSys::CL_TEXT);
	addkeyword(_qstr("function"),cl1);
	addkeyword(_qstr("codeblock"),cl1);
	addkeyword(_qstr("try"),cl1);
	addkeyword(_qstr("if"),cl1);
	addkeyword(_qstr("then"),cl1);
	addkeyword(_qstr("else"),cl1);
	addkeyword(_qstr("for"),cl1);
	addkeyword(_qstr("to"),cl1);
	addkeyword(_qstr("do"),cl1);
	addkeyword(_qstr("foreach"),cl1);
	addkeyword(_qstr("in"),cl1);
	addkeyword(_qstr("while"),cl1);
	addkeyword(_qstr("and"),cl1);
	addkeyword(_qstr("or"),cl1);

	TQXColor cl2=TQXColor(0.3f,0.3f,1)+2*G_QXSys().G_color(TQXSys::CL_TEXT);
	for (int i=0; i<env->G_functioncount(); i++)
		addkeyword(env->G_function(i)->G_name(),cl2);
	for (int i=0; i<env->G_typecastcount(); i++)
	{
		str.clear();str+=TYPECASTCHAR;str+=env->G_typecast(i)->G_returntype()->G_name();
		addkeyword(str,cl2);
	}
}

TSC_script::~TSC_script()
{
	scriptargument.clear();
	keywords.reset();
	root.reset();
	source.clear();
}


void TSC_script::addexternalvar(StrPtr iname, TSC_value *ival)
{
	root.G_varset()->addvar(iname,ival);
}


void TSC_script::Set_error(Tscriptsourcepart &source, int ps1, int ps2, StrPtr icontent)
{
	error.content.clear();

	for (int i=ps1; i<=ps2; i++) error.content+=source[i];
	error.content+=_qstr("--");
	error.content+=icontent;
	error.line1=source.linenr[ps1];
	error.col1=source.colnr[ps1];
	error.line2=source.linenr[ps2];
	error.col2=source.colnr[ps2];
}


bool TSC_script::compile(QString &cerror)
{
	QString source_str;
	Tintarray source_linenr,source_colnr;

	cerror.clear();
	root.reset();
	curdebugblock=NULL;


	StrChar ch;
	for (int linenr=0; linenr<source.G_linecount(); linenr++)
	{
		const QString *line=source.G_line(linenr);
		bool instr=false;
		bool incomment=false;
		for (int colnr=0; colnr<line->G_length(); colnr++)
		{
			ch=line->G_char(colnr);
			if ((!incomment)&&(!instr)&&(ch=='#')/*&&(line->G_char(colnr+1)=='/')*/) incomment=true;
			if (!incomment)
			{
				if (isquote(ch)) instr=!instr;
				source_str+=(StrPtr)CString(ch);
				source_linenr.add(linenr);source_colnr.add(colnr);
			}
		}
		source_str+=_qstr(" ");
		source_linenr.add(linenr);source_colnr.add(line->G_length());
	}

	const StrChar *textptr=source_str.G_ptr();

	Tscriptsourcepart flatsource;
	flatsource.txt=textptr;
	flatsource.linenr=source_linenr.G_ptr();
	flatsource.colnr=source_colnr.G_ptr();
	flatsource.count=source_linenr.G_count();
	if (!root.compile(flatsource))
	{
		G_source().Set_sel(error.line1,error.col1,error.line2,error.col2);
		cerror=error.content;
		return false;
	}
	return true;
}



bool TSC_script::exec_start(QString &cerror)
{
	root.exec_stop();
	if (!compile(cerror)) return false;

	root.exec_start();
	//create variable for "all" object ! variables only created at runtime!
	TSC_value *val_all=new TSC_value;
	val_all->settype(GetTSCenv().G_datatype(SC_valname_object));
	val_all->encapsulate(&T3DCosmos::Get());
	addexternalvar(_qstr("All"),val_all);
	curdebugblock=&root;
	G_source().readonly=true;
	outputlines.reset();
	return true;
}

void TSC_script::exec_stop()
{
	root.exec_stop();
	curdebugblock=NULL;
	G_source().readonly=false;
}


bool TSC_script::exec_isstarted()
{
	return root.exec_isstarted();
}

bool TSC_script::exec_isfinished()
{
	return root.exec_isfinished();
}


bool TSC_script::exec_stepone(QString &cerror)
{
	if (root.exec_isfinished()) return true;
	try{
		root.exec_stepone();
	}
	catch (TSC_runerror err)
	{
		cerror=err.G_content();
		return false;
	}
	catch(QError err)
	{
		cerror=err.G_content();
		return false;
	}
	return true;
}

bool TSC_script::exec_run(const TSC_value *iargument, QString &cerror)
{
	addlog(TFormatString(_text("Execute script ^1"),G_source().G_filename()),+1);
	if (!root.exec_isstarted())
	{
		if (!exec_start(cerror)) return false;
	}
	scriptargument.copyfrom_flexibletype(iargument);
	exec_isbreak=false;
	while ((!exec_isfinished())&&(!exec_isbreak))
	{

		if (!exec_stepone(cerror)) return false;
	}
	if (G_3DCosmos().G_requeststop())
	{
		G_3DCosmos().resetallsounds();
		G_3DCosmos().Reset_requeststop();
	}
	addlog(_text("Script executed"),-1);
	return true;
}


void TSC_script::exec_buildstack(Tarray<QString> &stacklist)
{
	stacklist.reset();
	if (curdebugblock!=NULL)
		curdebugblock->exec_buildstack(stacklist);
}


void TSC_script::exec_buildvarlist(Tcopyarray<TSC_script_var> &varlist, Tintarray &vardepthlist)
{
	varlist.reset();
	if (curdebugblock!=NULL)
		curdebugblock->exec_buildvarlist(0,varlist,vardepthlist);
}

void TSC_script::addkeyword(StrPtr name, TQXColor &col)
{
	TSC_keyword *kw=new TSC_keyword;
	kw->name=name;kw->name.ToUpper();
	kw->color=col;
	keywords.add(kw);
	keywordindex.add(name,keywords.G_count()-1);
}

bool TSC_script::G_keyword(StrPtr name, TQXColor &col)
{
	QString lname=name;lname.ToUpper();
	int idx=keywordindex.get(lname);
	if (idx<0) return false;
	col=keywords[idx]->color;
	return true;
}



void TSC_script::add_output_line(StrPtr iline)
{
	outputlines.add(new QString(iline));
}

StrPtr TSC_script::G_output_line(int nr) const
{
	if (nr<0) return _qstr("");
	if (nr>=outputlines.G_count()) return _qstr("");
	return *outputlines[nr];
}
