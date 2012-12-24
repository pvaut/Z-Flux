


#include "stdafx.h"
#include "SC_env.h"
#include "qxformattedstring.h"

#include "objecttree.h"
#include "SC_functree.h"
#include "SC_script.h"

StrPtr G_operatortypename(int opID);
StrPtr G_startupdir();

void G_highlightfuncid(QString &ID);


TFuncTreeItem::TFuncTreeItem(TFuncTreeItem *iparent)
{
	parent=iparent;
	showinreport=true;
}

void TFuncTreeItem::buildindex()
{
	if (!showinreport) return;

	QString fullID;
	G_fullID(fullID);
	GetTSCenv().addfunctreeitem(fullID,this);
	for (int i=0; i<childs.G_count(); i++)
		childs[i]->buildindex();
}


void TFuncTreeItem::fillist(TSC_list *lst)
{
	if (!showinreport) return;

	TSC_value *val=new TSC_value;
	val->createtype(GetTSCenv().G_datatype(SC_valname_map));
	TSC_map *map=G_valuecontent<TSC_map>(val);
	fillinfo(map);
	lst->add(val);
	for (int i=0; i<childs.G_count(); i++)
		childs[i]->fillist(lst);
}

void TFuncTreeItem::fillinfo(TSC_map *map)//fills info about this item
{
	QString st;
	G_fullID(st);
	map->SetString(_qstr("ID"),st);
	G_name(st);
	map->SetString(_qstr("Name"),st);

	map->SetString(_qstr("TypeName"),G_type());

	fillinfo_impl(map);
}


void TFuncTreeItem::exportdoc(QTextfile &fl, int level)//export the documentation as LaTex file
{
	QString name,st,str;

	QString fullID;
	G_fullID(fullID);

	QString secttype;

	G_name(name);

	name.Replace(_qstr("^"),_qstr("$\\wedge$"));
	name.Replace(_qstr("_"),_qstr("?"));
	name.Replace(_qstr("<"),_qstr("$<$"));
	name.Replace(_qstr(">"),_qstr("$>$"));

	bool dowrite=true;

	if ( (issame(G_type(),_qstr("Category"))) && (issame(name,_text("Operators"))) ) dowrite=false;
	if ( (issame(G_type(),_qstr("Category"))) && (level>=1) ) dowrite=false;


	if (dowrite)
	{
		if ( issame(G_type(),_qstr("Type"))) level=min(1,level);

		secttype=_text("section");
		if (level==1) secttype=_text("subsection");
		if (level==2) secttype=_text("subsubsection");
		QString tpstring;
		if (issame(G_type(),_qstr("Type"))) tpstring=_qstr("Type ");
		if (issame(G_type(),_qstr("Function"))) tpstring=_qstr("Function ");
		if (issame(G_type(),_qstr("Operator"))) tpstring=_qstr("Operator ");
		fl.write(TFormatString(_text("\\^1{^2^3 \\label{^4}}"),secttype,tpstring,name,fullID));fl.writeline();

		if (issame(G_type(),_qstr("Function"))||issame(G_type(),_qstr("Member function")))
		{
			TQXFormattedString fstr;
			G_displayname(fstr);
			fstr.ExportLaTex(st);
			fl.write(st);fl.write(_qstr(" \\\\"));fl.writeline();
		}


		GetTSCenv().G_ParsedDescription(fullID,st);
		if (qstrlen(st)<=0) st=_text("No description");
		fl.write(st);fl.writeline();fl.writeline();

		if ( issame(G_type(),_qstr("Category"))) level++;
		if ( issame(G_type(),_qstr("Type"))) level++;
		if (level>=2) level=2;

	}


	for (int i=0; i<childs.G_count(); i++)
		childs[i]->exportdoc(fl,level);
}


void TFuncTreeItem::G_description(QString &str)
{
	QString st2;

	G_name(st2);
	str=TFormatString(_text("\\large{^1 \\textbf{^2}}"),G_type(),st2);
	str+=_qstr("\n\n");

	if (issame(G_type(),_qstr("Function"))||issame(G_type(),_qstr("Member function")))
	{
		TQXFormattedString fstr;
		G_displayname(fstr);
		fstr.ExportLaTex(st2);
		str+=st2;
		str+=_qstr(" \n\n");
	}

	QString fullID;
	G_fullID(fullID);
	GetTSCenv().G_ParsedDescription(fullID,st2);
	str+=st2;
}



//////////////////////////////////////////////////////////////////////

void TFuncTreeItem_Category::G_fullID(QString &fullID)
{
	fullID=parentfullID;
	if (fullID.G_length()>0) fullID+=_qstr(":");
	fullID+=name;
}


TFuncTreeItem_Vartype::TFuncTreeItem_Vartype(TFuncTreeItem *iparent, TSC_datatype *idatatype)
 : TFuncTreeItem(iparent)
{
	datatype=idatatype;
}

void TFuncTreeItem_Vartype::G_name(QString &str)
{
	str=datatype->G_name();
}

void TFuncTreeItem_Vartype::G_sourcecodename(QString &str)
{
	str=datatype->G_name();
}


void TFuncTreeItem_Vartype::G_fullID(QString &fullID)
{
	datatype->G_fullID(fullID);
}


TFuncTreeItem_Memberfunction::TFuncTreeItem_Memberfunction(TFuncTreeItem *iparent, TSC_datatype *idatatype, TSC_func *ifunc)
 : TFuncTreeItem(iparent)
{
	datatype=idatatype;
	func=ifunc;
}

void TFuncTreeItem_Memberfunction::G_name(QString &str)
{
	str=func->G_vartype(0)->G_name();
	str+=_qstr(".");
	str+=func->G_name();
}

void TFuncTreeItem_Memberfunction::G_sourcecodename(QString &str)
{
	str=func->G_name();
}

void TFuncTreeItem_Memberfunction::G_displayname(TQXFormattedString &fstr)
{
	fstr.clear();
	if (func->G_hasreturn()) { fstr+=func->G_returntype()->G_name(); fstr+=_qstr(" "); }
//	fstr+=func->G_vartype(0)->G_name();
	fstr+=_qstr("X");
	fstr+=_qstr(" . ");
	fstr.makebold(true);
	fstr+=func->G_name();
	fstr.makebold(false);
	if ((func->G_varcount()>1)||(func->G_flexiblearglist()))
	{
		fstr+=_qstr(" ( ");
		bool required=true;
		for (int i=1; i<func->G_varcount(); i++)
		{
			if (i>1) fstr+=_qstr(", ");
			if (required&&!func->G_varrequired(i))
			{
				required=false;
				fstr+=_qstr(" [ ");
			}
			fstr+=func->G_vartype(i)->G_name();
			fstr+=_qstr(" ");
			fstr.makeitalic(true);
			fstr+=func->G_varname(i);
			fstr.makeitalic(false);
		}
		if (func->G_flexiblearglist()) fstr+=_qstr(" ... ");
		if (!required) fstr+=_qstr(" ]");
		fstr+=_qstr(" )");
	}
	if (func->G_canassign()) fstr+=_qstr(" =");
}


void TFuncTreeItem_Memberfunction::G_fullID(QString &fullID)
{
	func->G_fullID(fullID);
}






TFuncTreeItem_CustomMemberfunction::TFuncTreeItem_CustomMemberfunction(TFuncTreeItem *iparent, TSC_datatype *idatatype, StrPtr isubclass, StrPtr iname, TQXFormattedString &iprototype)
 : TFuncTreeItem(iparent)
{
	parentclassname=idatatype->G_name();
	name=iname;
	prototype=iprototype;
	myfullID=_qstr("F:");
	myfullID+=idatatype->G_name();
	myfullID+=_qstr(":");
	myfullID+=name;
}

void TFuncTreeItem_CustomMemberfunction::G_name(QString &str)
{
	str=TFormatString(_qstr("^1.^2"),parentclassname,name);
}

void TFuncTreeItem_CustomMemberfunction::G_sourcecodename(QString &str)
{
	str=name;
}


 void TFuncTreeItem_CustomMemberfunction::G_displayname(TQXFormattedString &fstr)
{
	fstr=prototype;
}

void TFuncTreeItem_CustomMemberfunction::G_fullID(QString &fullID)
{
	fullID=myfullID;
}




TFuncTreeItem_Function::TFuncTreeItem_Function(TFuncTreeItem *iparent, TSC_func *ifunc)
 : TFuncTreeItem(iparent)
{
	func=ifunc;
}

void TFuncTreeItem_Function::G_name(QString &str)
{
	str=func->G_name();
}

void TFuncTreeItem_Function::G_sourcecodename(QString &str)
{
	str=func->G_name();
}


void TFuncTreeItem_Function::G_displayname(TQXFormattedString &fstr)
{
	if (func->G_hasreturn()) { fstr+=func->G_returntype()->G_name(); fstr+=_qstr(" "); }
	fstr.makebold(true);
	fstr+=func->G_name();
	fstr.makebold(false);
	if ((func->G_varcount()>0)||(func->G_flexiblearglist()))
	{
		fstr+=_qstr(" ( ");
		bool required=true;
		for (int i=0; i<func->G_varcount(); i++)
		{
			if (i>0) fstr+=_qstr(", ");
			if (required&&!func->G_varrequired(i))
			{
				required=false;
				fstr+=_qstr(" [ ");
			}
			fstr+=func->G_vartype(i)->G_name();
			fstr+=_qstr(" ");
			fstr.makeitalic(true);
			fstr+=func->G_varname(i);
			fstr.makeitalic(false);
		}
		if (func->G_flexiblearglist()) fstr+=_qstr(" ... ");
		if (!required) fstr+=_qstr(" ]");
		fstr+=_qstr(" )");
	}
	if (func->G_canassign()) fstr+=_qstr("=");
}


void TFuncTreeItem_Function::fillinfo_impl(TSC_map *map)
{
//	map->SetString(_qstr("Type"),_qstr("Function"));
}

void TFuncTreeItem_Function::G_fullID(QString &fullID)
{
	func->G_fullID(fullID);
}



TFuncTreeItem_Operator::TFuncTreeItem_Operator(TFuncTreeItem *iparent, TSC_operator *iop)
 : TFuncTreeItem(iparent)
{
	op=iop;
}

void TFuncTreeItem_Operator::G_name(QString &str)
{
	str=TFormatString(_qstr("^1 ^2 ^3"),
		op->G_vartype1()->G_name(),G_operatortypename(op->G_opID()),op->G_vartype2()->G_name());
}

void TFuncTreeItem_Operator::G_displayname(TQXFormattedString &fstr)
{
	fstr.clear();
	fstr+=op->G_returntype()->G_name();
	fstr+=_qstr(" = ");
	fstr+=op->G_vartype1()->G_name();
	fstr+=_qstr(" ");
	fstr.makebold(true);
	fstr+=G_operatortypename(op->G_opID());
	fstr.makebold(false);
	fstr+=_qstr(" ");
	fstr+=op->G_vartype2()->G_name();
}


void TFuncTreeItem_Operator::G_fullID(QString &fullID)
{
	op->G_fullID(fullID);
}


TFuncTreeItem_Typecast::TFuncTreeItem_Typecast(TFuncTreeItem *iparent, TSC_typecast *itc)
 : TFuncTreeItem(iparent)
{
	tc=itc;
}


void TFuncTreeItem_Typecast::G_name(QString &str)
{
	str.clear();
	str+=tc->G_returntype()->G_name();
	str+=_qstr("=");
	str+=tc->G_argtype()->G_name();
}

void TFuncTreeItem_Typecast::G_fullID(QString &fullID)
{
	tc->G_fullID(fullID);
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_functree_getlist,GetFunctionList)
{
	setclasspath(SC_introspection);
	setreturntype(SC_valname_list);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_list *lst=G_valuecontent<TSC_list>(retval);
	lst->reset();
	GetTSCenv().G_functreeroot()->fillist(lst);
}
ENDFUNCTION(func_functree_getlist)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_functree_getrawdescription,GetRawFunctionDescription)
{
	setclasspath(SC_introspection);
	setreturntype(SC_valname_string);
	addvar(_qstr("ID"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr ID=arglist->get(0)->G_content_string()->G_string();
	retval->fromstring(GetTSCenv().G_description(ID));
}
ENDFUNCTION(func_functree_getrawdescription)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_functree_setrawdescription,SetRawFunctionDescription)
{
	setclasspath(SC_introspection);
	addvar(_qstr("ID"),SC_valname_string);
	addvar(_qstr("description"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr ID=arglist->get(0)->G_content_string()->G_string();
	StrPtr descr=arglist->get(1)->G_content_string()->G_string();
	GetTSCenv().AddDescription(ID,descr);
	GetTSCenv().SaveDescriptions();
}
ENDFUNCTION(func_functree_setrawdescription)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_functree_gethighlightfunc,GetHighlightFuncID)
{
	setclasspath(SC_introspection);
	setreturntype(SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QString ID;
	G_highlightfuncid(ID);
	if (qstrlen(ID)<=0) throw QError(_text("No function is currently highlighted"));
	retval->fromstring(ID);
}
ENDFUNCTION(func_functree_gethighlightfunc)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_functree_exportfunctiondocumentation,ExportFunctionDocumentation)
{
	setclasspath(SC_introspection);
//	setreturntype(SC_valname_string);
//	addvar(_qstr("ID"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QTextfile fl;
	fl.openwrite(TFormatString(_qstr("^1\\scriptdoc.tex"),G_startupdir()));

	fl.write(_text("\\documentclass[10pt]{book}"));fl.writeline();

	fl.write(_text("\\usepackage{hyperref}"));fl.writeline();
	fl.write(_text("\\hypersetup{colorlinks=true,linkcolor=blue}"));fl.writeline();

	fl.write(_text("\\newcommand{\\linkitem}[1]{\\hyperref[#1]{\\nameref{#1} (\\ref{#1})}}"));fl.writeline();
	fl.write(_text("\\newcommand{\\sourcecode}[1]{\\texttt{#1}}"));fl.writeline();
	fl.write(_text("\\newcommand{\\var}[1]{\\textit{#1}}"));fl.writeline();
	fl.write(_text("\\newcommand{\\softwarename}{Z--Flux}"));fl.writeline();

	fl.write(_text("\\begin{document}"));fl.writeline();

	fl.write(_text("\\chapter{Script language definition}"));fl.writeline();

	GetTSCenv().G_functreeroot()->exportdoc(fl,0);

	fl.write(_text("\\end{document}"));fl.writeline();
	fl.close();
}
ENDFUNCTION(func_functree_exportfunctiondocumentation)
