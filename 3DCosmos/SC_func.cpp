#include "stdafx.h"

#include "qerror.h"
#include "qtranslate.h"

#include "valnames.h"

#include "SC_func.h"
#include "SC_script.h"
#include "SC_env.h"

#include "3Dscene.h"

StrPtr G_startupdir();
StrPtr G_datadir();
StrPtr G_scriptsdir();
StrPtr G_texturesdir();

void ExecuteApplication(StrPtr commandline, StrPtr startupdir, bool waitforcompletion);
void ExitProgram();
void ShutDownWindows();
void SetMonitorStatus(bool status);


TSC_funcarglist::TSC_funcarglist(TSC_script_block *iparentblock)
{
	parentblock=iparentblock;
}


TSC_funcadder::TSC_funcadder(TSC_func *ifunc)
{
	GetTSCenv().addfunction(ifunc);
}


TSC_func::TSC_func(StrPtr iname)
{
	returntype=GETDATATYPE_SCALAR;
	hasreturn=false;
	ismemberfunction=false;
	canassign=false;
	flexiblearglist=false;
	name=iname;
}

void TSC_func::setreturntype(StrPtr itypename)
{
	hasreturn=true;
	returntype=GetTSCenv().G_datatype(itypename);
	ASSERT(returntype!=NULL);
}

void TSC_func::setmemberfunction(StrPtr itypename)
{
	TSC_datatype *datatype=GetTSCenv().G_datatype(itypename);
	ASSERT(datatype!=NULL);
	if (datatype==NULL) return;
	ASSERT(!ismemberfunction);
	ASSERT(vars.G_count()==0);
	if (ismemberfunction) return;
	ismemberfunction=true;
	TSC_func_var *var=new TSC_func_var(_qstr("_"),datatype);
	vars.add(var);
	setclasspath_fromtype(itypename);
}


void TSC_func::addvar(StrPtr iname, StrPtr itypename, bool irequired)
{
	TSC_datatype *datatype=GetTSCenv().G_datatype(itypename);
	ASSERT(datatype!=NULL);
	if (datatype==NULL) return;
	TSC_func_var *var=new TSC_func_var(iname,datatype);
	if (irequired&&(vars.G_count()>0)&&(!vars[vars.G_count()-1]->required)) ASSERT(false);
	var->required=irequired;
	vars.add(var);
}

StrPtr TSC_func::G_varname(int varnr)
{
	ASSERT((varnr>=0)&&(varnr<vars.G_count()));
	return vars[varnr]->name;
}

TSC_datatype* TSC_func::G_vartype(int varnr)
{
	ASSERT((varnr>=0)&&(varnr<vars.G_count()));
	return vars[varnr]->vartype;
}

bool TSC_func::G_varrequired(int varnr)
{
	ASSERT((varnr>=0)&&(varnr<vars.G_count()));
	return vars[varnr]->required;
}


void TSC_func::setclasspath_fromtype(StrPtr itypename)
{
	TSC_datatype *datatype=GetTSCenv().G_datatype(itypename);
	ASSERT(datatype!=NULL);
	QString path;
	path=datatype->G_classpath();
	if (path.G_length()>0) path+=_qstr(";");
	path+=itypename;
	setclasspath(path);
}

void TSC_func::extendclasspath(StrPtr ipart)
{
	if (classpath.G_length()>0) classpath+=_qstr(";");
	classpath+=ipart;
}

void TSC_func::G_fullID(QString &fullID)
{
	fullID=_qstr("F:");
	if (ismemberfunction)
	{
		fullID+=G_vartype(0)->G_name();
		fullID+=_qstr(":");
	}
	fullID+=name;
}



void TSC_func::execute(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	if (!canassign&&(assigntoval!=NULL)) 
		throw QError(_text("Invalid assignment"));
	if (!ismemberfunction)
	{
//		if (values->G_count()!=varnames.G_count()) throw QError(_text("Invalid number of function arguments"));
		if (owner!=NULL) throw QError(_text("Invalid function call"));
	}
	else
	{
//		if (values->G_count()!=varnames.G_count()-1) throw QError(_text("Invalid number of function arguments"));
		if (owner==NULL) throw QError(_text("No owner of member function"));
	}
	if (retval==NULL) throw QError(_text("No return variable for function"));

	if (returntype!=GETDATATYPE_ANY)
		retval->maketype(returntype);

	execute_implement(arglist,retval,assigntoval,owner);
}

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_store,Store)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("name"),SC_valname_string);
	addvar(_qstr("content"),SC_valname_any);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr name=arglist->get(0)->G_content_string()->G_string();

	TSC_value *arg=NULL;
	if (arglist->G_ispresent(1)) arg=arglist->get(1);

	GetTSCenv().storevar(name,arg);
}
ENDFUNCTION(func_store)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_recall,Recall)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("name"),SC_valname_string);
	setreturntype(SC_valname_any);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr name=arglist->get(0)->G_content_string()->G_string();

	retval->copyfrom_flexibletype(GetTSCenv().recallvar(name));
}
ENDFUNCTION(func_recall)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_executescript,ExecuteScript)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("script"),SC_valname_string);
	addvar(_qstr("argument"),SC_valname_any);
	setreturntype(SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QString error;
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();

	TSC_value *arg=NULL;
	if (arglist->G_ispresent(1)) arg=arglist->get(1);

	TSC_script script(&GetTSCenv());
	script.G_source().load(filename);
	if (!script.exec_run(arg,error)) throw QError(error);
	retval->fromstring(script.stopstring);
}
ENDFUNCTION(func_executescript)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scriptargument,ScriptArgument)
{
	setclasspath(_qstr("General"));
	setreturntype(SC_valname_any);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->copyfrom_flexibletype(arglist->G_parentblock()->G_script()->G_scriptargument());
}
ENDFUNCTION(func_scriptargument)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scriptfilename,ScriptFilename)
{
	setclasspath(_qstr("General"));
	setreturntype(SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->fromstring(arglist->G_parentblock()->G_script()->G_source().G_filename());
}
ENDFUNCTION(func_scriptfilename)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scriptfilepath,ScriptFilePath)
{
	setclasspath(_qstr("General"));
	setreturntype(SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QString fp;
	QString st=arglist->G_parentblock()->G_script()->G_source().G_filename();
	st.invert();
	st.splitstring(_qstr("\\"),fp);
	st.invert();
	retval->fromstring(st);
}
ENDFUNCTION(func_scriptfilepath)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_getlanguagelist,GetLanguageList)
{
	setclasspath(_qstr("General"));
	setreturntype(SC_valname_list);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_list *list=G_valuecontent<TSC_list>(retval);
	list->reset();
	for (int i=0; i<QTranslate::Get().G_languagecount(); i++)
	{
		TSC_value vl;
		vl.settype(GetTSCenv().G_datatype(SC_valname_string));
		vl.fromstring(QTranslate::Get().G_language(i));
		list->add(&vl);
	}
}
ENDFUNCTION(func_getlanguagelist)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_getactivelanguage,GetActiveLanguage)
{
	setclasspath(_qstr("General"));
	setreturntype(SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->fromstring(QTranslate::Get().GetLanguage());
}
ENDFUNCTION(func_getactivelanguage)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_setactivelanguage,SetActiveLanguage)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("error"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QTranslate::Get().SetLanguage(arglist->get(0)->G_content_string()->G_string());
}
ENDFUNCTION(func_setactivelanguage)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_throw,Throw)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("error"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	throw QError(arglist->get(0)->G_content_string()->G_string());
}
ENDFUNCTION(func_throw)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scriptcatchederror,CatchedError)
{
	setclasspath(_qstr("General"));
	setreturntype(SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_string()->fromstring(arglist->G_parentblock()->G_script()->G_catchederror());
}
ENDFUNCTION(func_scriptcatchederror)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_execdir,ExecDir)
{
	setclasspath(_qstr("General"));
	setreturntype(SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_string()->fromstring(G_startupdir());
}
ENDFUNCTION(func_execdir)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_datadir,DataDir)
{
	setclasspath(_qstr("General"));
	setreturntype(SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_string()->fromstring(G_datadir());
}
ENDFUNCTION(func_datadir)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scriptdir,ScriptDir)
{
	setclasspath(_qstr("General"));
	setreturntype(SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->G_content_string()->fromstring(G_scriptsdir());
}
ENDFUNCTION(func_scriptdir)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_readsetting,ReadSetting)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("SettingID"),SC_valname_string);
	addvar(_qstr("DefaultValue"),SC_valname_any);
	setreturntype(SC_valname_any);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->copyfrom_flexibletype(arglist->get(1));
	QString settingcontent;
	StrPtr settingid=arglist->get(0)->G_content_string()->G_string();
	if (T3DCosmos::Get().ReadSetting(settingid,settingcontent))
		retval->fromstring(settingcontent);
}
ENDFUNCTION(func_readsetting)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_writesetting,WriteSetting)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("SettingID"),SC_valname_string);
	addvar(_qstr("Value"),SC_valname_any);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QString str;
	StrPtr settingid=arglist->get(0)->G_content_string()->G_string();
	arglist->get(1)->tostring(str,0);
	T3DCosmos::Get().WriteSetting(settingid,str);
}
ENDFUNCTION(func_writesetting)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_createvar,createvar)
{
	setreturntype(SC_valname_any);
	setclasspath(_qstr("General"));
	addvar(_qstr("variable"),SC_valname_any);
}

void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
/*	if (!arglist->G_ispresent(0)) throw QError(_text("Missing argument for return function"));
	if (arglist->G_parentblock()==NULL) throw QError(_text("Return: missing parent block"));
	arglist->G_parentblock()->addvar(???);*/
}

ENDFUNCTION(func_createvar)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_createref,Ref)
{
	setreturntype(SC_valname_any);
	setclasspath(_qstr("General"));
	addvar(_qstr("variable"),SC_valname_any);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->encapsulate(arglist->get(0));
	retval->Make_explicitreference();
}
ENDFUNCTION(func_createref)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_unref,Deref)
{
	setreturntype(SC_valname_any);
	setclasspath(_qstr("General"));
	addvar(_qstr("variable"),SC_valname_any);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	retval->unref(arglist->get(0));
}
ENDFUNCTION(func_unref)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_typeof,typeof)
{
	setreturntype(SC_valname_string);
	setclasspath(_qstr("General"));
	addvar(_qstr("variable"),SC_valname_any);
}

void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	if (arglist->G_ispresent(0))
	{
		TSC_datatype *tpe=arglist->get(0)->G_datatype();
		if (tpe!=NULL)
		{
			retval->G_content_string()->fromstring(tpe->G_name());
			return;
		}
	}
	retval->G_content_string()->fromstring(_text("Unassigned"));
}

ENDFUNCTION(func_typeof)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_isvardefined,isvardefined)
{
	setreturntype(SC_valname_boolean);
	setclasspath(_qstr("General"));
	addvar(_qstr("variablename"),SC_valname_string);
}

void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr varname=arglist->get(0)->G_content_string()->G_string();
	retval->G_content_boolean()->copyfrom((arglist->G_parentblock()->findvar(NULL,varname))!=NULL);
}

ENDFUNCTION(func_isvardefined)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_isfunctiondefined,isfunctiondefined)
{
	setreturntype(SC_valname_boolean);
	setclasspath(_qstr("General"));
	addvar(_qstr("functionname"),SC_valname_string);
}

void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr funcname=arglist->get(0)->G_content_string()->G_string();
	retval->G_content_boolean()->copyfrom((arglist->G_parentblock()->G_customfunction(funcname))!=NULL);
}

ENDFUNCTION(func_isfunctiondefined)


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_evalfunction,EvalFunction)
{
	setreturntype(SC_valname_any);
	setclasspath(_qstr("General"));
	addvar(_qstr("FunctionName"),SC_valname_string);
	addvar(_qstr("Arguments"),SC_valname_list);
}

void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QString error;
	StrPtr funcname=arglist->get(0)->G_content_string()->G_string();
	TSC_list *args=G_valuecontent<TSC_list>(arglist->get(1));
	TSC_funcarglist locarglist(arglist->G_parentblock());
	for (int i=0; i<args->G_size(); i++)
		locarglist.add(args->get(i));
	if (!arglist->G_parentblock()->evalfunction(funcname,retval,NULL, locarglist, error)) throw QError(error);
}

ENDFUNCTION(func_evalfunction)

//...todo: gelijkaardige script functie evalvar?--!!--


///////////////////////////////////////////////////////////////////////////////////

FUNCTION(func_isdefined,isdefined)
{
	setreturntype(SC_valname_boolean);
	setclasspath(_qstr("General"));
	addvar(_qstr("variable"),SC_valname_any);
}

void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	if (!arglist->G_ispresent(0)) throw QError(_text("Missing argument"));
	bool rs=(arglist->get(0)->G_content()!=NULL);
	retval->G_content_boolean()->copyfrom(rs);
}

ENDFUNCTION(func_isdefined)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_return,return)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("returnvalue"),SC_valname_any,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	if (!arglist->G_ispresent(0)) throw QError(_text("Missing argument for return function"));
	TSC_script_block *block=arglist->G_parentblock();
	while ((block!=NULL)&&(!block->G_isfunctionblock()))
	{
		block=block->G_parentblock();
	}
	if (block==NULL) throw QError(_text("Invalid call to 'return'"));
	block->Set_returnvalue(arglist->get(0));
}
ENDFUNCTION(func_return)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_message,Message)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("content"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	::MessageBox(NULL,arglist->get(0)->G_content_string()->G_string(),_text("Message"),MB_OK);
}
ENDFUNCTION(func_message)


bool EditText(QString &str);

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_edittext,EditText)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("content"),SC_valname_string);
	setreturntype(SC_valname_boolean);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QString &str=arglist->get(0)->G_content_string()->G_QString();
	retval->G_content_boolean()->copyfrom(EditText(str));
}
ENDFUNCTION(func_edittext)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_stop,stop)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("ReturnValue"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	if (arglist->G_ispresent(0))
		arglist->G_parentblock()->G_script()->stopstring=arglist->get(0)->G_content_string()->G_string();
	arglist->G_parentblock()->exec_stop();
	arglist->G_parentblock()->G_script()->exec_stop();
}
ENDFUNCTION(func_stop)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_delay,delay)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("delay"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	Sleep(1000.0*arglist->get(0)->G_content_scalar()->G_val());
}
ENDFUNCTION(func_delay)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_break,break)
{
	setclasspath(_qstr("General"));
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	arglist->G_parentblock()->G_script()->exec_break();
}
ENDFUNCTION(func_break)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_output,Output)
{
	addvar(_qstr("content"),SC_valname_string);
	setclasspath(_qstr("General"));
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QString st;
	arglist->get(0)->tostring(st);
	arglist->G_parentblock()->G_script()->add_output_line(st);
}
ENDFUNCTION(func_output)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_execute,Execute)
{
	addvar(_qstr("content"),SC_valname_string);
	addvar(_qstr("startupdir"),SC_valname_string);
	addvar(_qstr("wait"),SC_valname_boolean);
	setclasspath(_qstr("General"));
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr commandline=arglist->get(0)->G_content_string()->G_string();
	StrPtr startupdir=arglist->get(1)->G_content_string()->G_string();
	bool wait=arglist->get(2)->G_content_boolean()->G_val();
	ExecuteApplication(commandline,startupdir,wait);
}
ENDFUNCTION(func_execute)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_exitprogram,ExitProgram)
{
	setclasspath(_qstr("General"));
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	ExitProgram();
}
ENDFUNCTION(func_exitprogram)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_shutdown,ShutdownComputer)
{
	setclasspath(_qstr("General"));
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	ShutDownWindows();
}
ENDFUNCTION(func_shutdown)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_setmonitorstatus,SetMonitorStatus)
{
	setclasspath(_qstr("General"));
	addvar(_qstr("status"),SC_valname_boolean);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	SetMonitorStatus(arglist->get(0)->G_content_boolean()->G_val());
}
ENDFUNCTION(func_setmonitorstatus)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_length,Length)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	int len=qstrlen(owner->G_content_string()->G_string());

	retval->G_content_scalar()->copyfrom(len);
}
ENDFUNCTION(func_length)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_string_get,get)
{
	setmemberfunction(SC_valname_string);
	setreturntype(SC_valname_string);
	addvar(_qstr("position"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	int posit=arglist->get(0)->G_content_scalar()->G_intval();
	StrChar stt[2];
	stt[0]=owner->G_content_string()->G_QString().G_char(posit);
	stt[1]=0;
	retval->G_content_string()->fromstring(stt);
}
ENDFUNCTION(func_string_get)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_substring,substring)
{
	setreturntype(SC_valname_string);
	setmemberfunction(SC_valname_string);
	addvar(_qstr("start"),SC_valname_scalar);
	addvar(_qstr("stop"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	int ps1=arglist->get(0)->G_content_scalar()->G_intval();
	int ps2=arglist->get(1)->G_content_scalar()->G_intval();
	CString str=owner->G_content_string()->G_string();
	retval->G_content_string()->fromstring(str.Mid(ps1,ps2-ps1+1));
}
ENDFUNCTION(func_substring)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_findstring,find)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_string);
	addvar(_qstr("target"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr target=arglist->get(0)->G_content_string()->G_string();
	int posit=owner->G_content_string()->G_QString().findfirst(target);
	retval->G_content_scalar()->copyfrom(posit);
}
ENDFUNCTION(func_findstring)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_splitstring,split)
{
	setreturntype(SC_valname_string);
	setmemberfunction(SC_valname_string);
	addvar(_qstr("split"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr split=arglist->get(0)->G_content_string()->G_string();
	QString firstpart;
	owner->G_content_string()->G_QString().splitstring(split,firstpart);
	retval->G_content_string()->fromstring(firstpart);
}
ENDFUNCTION(func_splitstring)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_replacestring,Replace)
{
	//setreturntype(SC_valname_string);
	setmemberfunction(SC_valname_string);
	addvar(_qstr("source"),SC_valname_string);
	addvar(_qstr("dest"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr source=arglist->get(0)->G_content_string()->G_string();
	StrPtr dest=arglist->get(1)->G_content_string()->G_string();
	owner->G_content_string()->G_QString().replace(source,dest);
	//retval->G_content_string()->fromstring(firstpart);
}
ENDFUNCTION(func_replacestring)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_invertstring,Invert)
{
	setmemberfunction(SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	owner->G_content_string()->G_QString().invert();
}
ENDFUNCTION(func_invertstring)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_str,Str)
{
	setclasspath_fromtype(SC_valname_string);
	setreturntype(SC_valname_string);
	addvar(_qstr("value"),SC_valname_any);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QString str;
	arglist->get(0)->tostring(str);
	retval->G_content_string()->fromstring(str);
}
ENDFUNCTION(func_str)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_translate,Translate)
{
	setclasspath_fromtype(SC_valname_string);
	setreturntype(SC_valname_string);
	addvar(_qstr("source"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr source=arglist->get(0)->G_content_string()->G_string();
	retval->G_content_string()->fromstring(_TRL(source));
}
ENDFUNCTION(func_translate)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_toscalar,ToScalar)
{
	setclasspath_fromtype(SC_valname_scalar);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("value"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QString str;
	double vl=qstr2double(arglist->get(0)->G_content_string()->G_string());
	retval->G_content_scalar()->copyfrom(vl);
}
ENDFUNCTION(func_toscalar)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scalartostring,ScalarToString)
{
	setclasspath_fromtype(SC_valname_scalar);
	setreturntype(SC_valname_string);
	addvar(_qstr("value"),SC_valname_scalar);
	addvar(_qstr("length"),SC_valname_scalar);
	addvar(_qstr("digits"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	double vl=arglist->get(0)->G_content_scalar()->G_val();
	int len=arglist->get(1)->G_content_scalar()->G_intval();
	int digits=arglist->get(2)->G_content_scalar()->G_intval();

	TCHAR formatstr[100];
	CString cstr;
	_stprintf_s(formatstr,99,_qstr("%%%d.%dlf"),len,digits);
	cstr.Format(formatstr,vl);

	retval->G_content_string()->fromstring(cstr);
}
ENDFUNCTION(func_scalartostring)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scalar_min,Min)
{
	setclasspath_fromtype(SC_valname_scalar);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("val1"),SC_valname_scalar);
	addvar(_qstr("val2"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_scalar *sc1=arglist->get(0)->G_content_scalar();
	TSC_scalar *sc2=arglist->get(1)->G_content_scalar();
	if (TSC_scalar::G_smaller(sc1,sc2))
		retval->G_content_scalar()->copyfrom(sc1);
	else
		retval->G_content_scalar()->copyfrom(sc2);
}
ENDFUNCTION(func_scalar_min)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_scalar_max,Max)
{
	setclasspath_fromtype(SC_valname_scalar);
	setreturntype(SC_valname_scalar);
	addvar(_qstr("val1"),SC_valname_scalar);
	addvar(_qstr("val2"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_scalar *sc1=arglist->get(0)->G_content_scalar();
	TSC_scalar *sc2=arglist->get(1)->G_content_scalar();
	if (TSC_scalar::G_smaller(sc1,sc2))
		retval->G_content_scalar()->copyfrom(sc2);
	else
		retval->G_content_scalar()->copyfrom(sc1);
}
ENDFUNCTION(func_scalar_max)
