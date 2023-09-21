#include "stdafx.h"
#include "qerror.h"
#include "SC_env.h"
#include "SC_time.h"
#include "SC_script.h"

#include "2DContour.h"

#include "objecttree.h"
#include "SC_functree.h"
#include "3dviewport.h"

void create_externaltypes();

StrPtr G_startupdir();

StrPtr G_operatortypename(int opID);

static TSC_env* env = nullptr;

TSC_env& GetTSCenv()
{
	if (env == nullptr)
		env = new TSC_env();
	return *env;
	//return TSC_env::Get();
}

class TSC_datatype_any : public TSC_datatype
	
{
public:
	TSC_datatype_any()
	{
		classpath="General";
	}
	StrPtr G_name() { return SC_valname_any; }
	TObject* value_create()
	{
		ASSERT(false);
		return NULL;
	}
	void value_delete(TObject *content)
	{
		ASSERT(false);
	}
	void value_copy(TObject *from, TObject *to)
	{
		ASSERT(false);
	}
	void tostring(TObject *content, QString &str, int tpe=0)
	{
		str.clear();
	};
	void fromstring(TObject *content, StrPtr str)
	{
		ASSERT(false);
	}
	bool compare(TObject *content1, TObject *content2)
	{
		ASSERT(false);
		return false;
	}
	bool cancopy() { return true; };
	bool G_isanytype() { return true; }
};


class TSC_datatype_list : public TSC_datatype_generic<TSC_list>
{
public:
	TSC_datatype_list()
		: TSC_datatype_generic<TSC_list>(SC_valname_list,true)
	{
	}
};

class TSC_datatype_map: public TSC_datatype_generic<TSC_map>

{
public:
	TSC_datatype_map()
		: TSC_datatype_generic<TSC_map>(SC_valname_map,true)
	{
	}
	virtual bool memberfunction_accept(TObject *content, StrPtr funcname)
	{
		if (::issame(funcname,_qstr("clear"))) return false;
		if (::issame(funcname,_qstr("get"))) return false;
		if (::issame(funcname,_qstr("IsPresent"))) return false;
		if (::issame(funcname,_qstr("GetNames"))) return false;
		if (::issame(funcname,_qstr("AddItem"))) return false;
		return true;
//		TSC_map *map=(TSC_map*)content;
//		return map->get(funcname)!=NULL;
	}
	virtual bool evalmemberfunction(TObject *content, StrPtr funcname, TSC_value *returnval, TSC_value *assigntoval, TSC_funcarglist &arglist, QString &error)
	{
		TSC_map *map=(TSC_map*)content;
		if (assigntoval!=NULL) map->Set(funcname,assigntoval);
		returnval->copyfrom_flexibletype(map->get(funcname));
		return true;
	}
};


class TSC_datatype_mappair : public TSC_datatype_generic<TSC_mappair>

{
public:
	TSC_datatype_mappair()
		: TSC_datatype_generic<TSC_mappair>(SC_valname_mappair,true)
	{
	}
};


TSC_env::TSC_env()
{
	if (env == nullptr)
		env = this;
	addlog(_text("Constructing ENV"),+1);

	adddatatype(new TSC_datatype_any)->classpath=SC_simpletypepath;

	datatype_string=adddatatype(new TSC_datatype_generic<TSC_string>(SC_valname_string,true));
	datatype_string->classpath=SC_simpletypepath;
	datatype_scalar=adddatatype(new TSC_datatype_generic<TSC_scalar>(SC_valname_scalar,true));
	datatype_scalar->classpath=SC_simpletypepath;
	datatype_boolean=adddatatype(new TSC_datatype_generic<TSC_boolean>(SC_valname_boolean,true));
	datatype_boolean->classpath=SC_simpletypepath;
	adddatatype(new TSC_datatype_generic<TSC_time>(SC_valname_time,true))->classpath=SC_simpletypepath;
	adddatatype(new TSC_datatype_generic<TSC_chrono>(SC_valname_chrono,true))->classpath=SC_simpletypepath;

	adddatatype(new TSC_datatype_list)->classpath=SC_containerpath;
	adddatatype(new TSC_datatype_mappair)->classpath=SC_containerpath;
	adddatatype(new TSC_datatype_map)->classpath=SC_containerpath;


	create_externaltypes();

	functreeroot=new TFuncTreeItem(NULL);
	functreeroot->Set_name(_qstr("Function root"));
	functreeroot->expanded=true;

	InitDescriptMappings();

	addlog(_text("ENV constructed"),-1);

}


void TSC_env::addfunctreeitem(StrPtr fullID, TFuncTreeItem *item)
{
	if (functreeindex.get(fullID)>=0)
	{
		throw QError(TFormatString(_text("Function signature ^1 is already present"),fullID));
	}
	functreeitemlist.add(item);
	functreeindex.add(fullID,functreeitemlist.G_count()-1);
}

TFuncTreeItem* TSC_env::G_functreeitem(StrPtr fullID)
{
	int nr=functreeindex.get(fullID);
	if (nr<0) return NULL;
	return functreeitemlist[nr];
}


void TSC_env::AddDescription(StrPtr iID, StrPtr icontent)
{
	int nr=descriptindex.get(iID);
	if (nr<0)
	{
		descriptID.add(new QString(iID));
		descriptcontent.add(new QString(icontent));
		descriptindex.add(iID,descriptcontent.G_count()-1);
	}
	else
		(*descriptcontent[nr])=icontent;
}

	
void TSC_env::LoadDescriptions()
{
	try{
		addlog(_text("Loading ENV descriptions"),+1);

		QTextfile fl;
		QString filename,ID,line;
		FormatString(filename,_qstr("^1\\scriptlang.flf"),G_startupdir());
		fl.openread(filename);
		while(!fl.isend())
		{
			ID.clear();while((!fl.isend())&&(ID.G_length()<=0)) fl.readline(ID);
			fl.readline(line);
			bool finished=false;
			bool firstline=true;
			QString dblock;
			while (!finished)
			{
				fl.readline(line);
				if ((issame(line,_qstr("----")))||fl.isend()) finished=true;
				else
				{
					if (!firstline) dblock+="\n";
					dblock+=line;
					firstline=false;
				}
			}
			AddDescription(ID,dblock);
		}
		fl.close();
		addlog(_text("ENV Function descriptions loaded"),-1);
	} catch(QError &err)
	{
		reporterror(err.G_content());
	}
}

void TSC_env::SaveDescriptions()
{
	try{
		QTextfile fl;
		QString filename,ID,line;
		FormatString(filename,_qstr("^1\\scriptlang.flf"),G_startupdir());
		fl.openwrite(filename);
		for (int i=0; i<descriptID.G_count(); i++)
			if (descriptID[i]->G_length()>0)
			{
				fl.writeline();
				fl.write(*descriptID[i]);
				fl.writeline();
				fl.write(_qstr(">>>>\n"));
				if (descriptcontent[i]->G_length()>0)
				{
					fl.write(*descriptcontent[i]);
					fl.writeline();
				}
				fl.write(_qstr("----\n"));
			}
/*		while(!fl.isend())
		{
			ID.clear();while((!fl.isend())&&(ID.G_length()<=0)) fl.readline(ID);
			fl.readline(line);
			bool finished=false;
			bool firstline=true;
			QString dblock;
			while (!finished)
			{
				fl.readline(line);
				if ((issame(line,_qstr("----")))||fl.isend()) finished=true;
				else
				{
					if (!firstline) dblock+="\n";
					dblock+=line;
					firstline=false;
				}
			}
			AddDescription(ID,dblock);
		}*/
		fl.close();
	} catch(QError &err)
	{
		reporterror(err.G_content());
	}
}


StrPtr TSC_env::G_description(StrPtr iID) const
{
	int nr=descriptindex.get(iID);
	if (nr<0)
	{
		//first try mapping to another object
		QString comps=iID;
		QString cmp1,cmp2,cmp3;
		comps.splitstring(_qstr(":"),cmp1);
		comps.splitstring(_qstr(":"),cmp2);
		comps.splitstring(_qstr(":"),cmp3);
		if (issame(cmp1,_qstr("F")))
		{
			StrPtr parentobject=G_DescriptMapObject(cmp2);
			if (qstrlen(parentobject)>0)
				return G_description(TFormatString(_qstr("^1:^2:^3"),cmp1,parentobject,cmp3));
		}
		return _qstr("");
	}
	else return (*descriptcontent[nr]);
}

void TSC_env::G_ParsedDescription(StrPtr iID, QString &descr) const
{
	descr=G_description(iID);
	if (descr[0]=='@')
	{
		QString newid;newid=descr;newid.substring(1,99999);
		G_ParsedDescription(newid,descr);
	}
}


void TSC_env::AddDescriptMapObject(StrPtr ichild, StrPtr iparent)
{
	if (issame(ichild,iparent)) return;
	int nr=descriptmapindex.get(ichild);
	if (nr>=0) return;
	descriptmapindex.add(ichild,descriptmapobject.G_count());
	descriptmapobject.add(new QString(iparent));
}

StrPtr TSC_env::G_DescriptMapObject(StrPtr ichild) const
{
	int nr=descriptmapindex.get(ichild);
	if (nr<0) return _qstr("");
	else return *descriptmapobject[nr];
}

void TSC_env::InitDescriptMappings()
{
	AddDescriptMapObject(_qstr("Bar"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("Sphere"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("Cylinder"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("Pie"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("Rectangle"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("Arrow"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("SolidPoint"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("Text3D"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("TextFlat"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("FormattedText"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("Surface"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("Curve"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("StarGlobe"),_qstr("SolidObject"));
	AddDescriptMapObject(_qstr("Clock"),_qstr("SolidObject"));

	AddDescriptMapObject(_qstr("TextControl"),_qstr("FrameControl"));
	AddDescriptMapObject(_qstr("ScalarControl"),_qstr("FrameControl"));
	AddDescriptMapObject(_qstr("EditControl"),_qstr("FrameControl"));
	AddDescriptMapObject(_qstr("ListControl"),_qstr("FrameControl"));
	AddDescriptMapObject(_qstr("CheckControl"),_qstr("FrameControl"));
	AddDescriptMapObject(_qstr("ButtonControl"),_qstr("FrameControl"));
	AddDescriptMapObject(_qstr("MenuControl"),_qstr("FrameControl"));

}



TFuncTreeItem* TFuncTreeItem::addcategory(StrPtr iname)
{
	TFuncTreeItem *it=(TFuncTreeItem*)G_childbyname(iname);
	if (it!=NULL) return it;
	QString parentfullid;
	G_fullID(parentfullid);
	it=new TFuncTreeItem_Category(this,iname,parentfullid);
	addchild(it);
	return it;
}


TFuncTreeItem* findtreepath(TFuncTreeItem *iroot, StrPtr ipath)
{
	TFuncTreeItem *posit2,*posit=iroot;
	QString token,path=ipath;
	while (path.G_length()>0)
	{
		path.splitstring(_qstr(";"),token);
		posit2=(TFuncTreeItem*)posit->G_childbyname(token);
		if (posit2==NULL)
		{
			QString parentfullid;
			posit->G_fullID(parentfullid);
			TFuncTreeItem *it=new TFuncTreeItem_Category(posit,token,parentfullid);
			posit->addchild(it);
			posit=it;
		}
		else posit=posit2;
	}
	return posit;
}




void TSC_env::buildfunctiontree()
{

	addlog(_text("Building function tree"),+1);

	for (int i=0; i<datatypes.G_count(); i++)
	{
		TFuncTreeItem *posit=findtreepath(functreeroot,datatypes[i]->G_classpath());
		TFuncTreeItem_Vartype *vrtp=new TFuncTreeItem_Vartype(posit,datatypes[i]);
		posit->addchild(vrtp);
		for (int optnr=0; optnr<datatypes[i]->G_optioncount(); optnr++)
		{
			TFuncTreeItem_EnumVariant *vr=new TFuncTreeItem_EnumVariant(vrtp,datatypes[i],datatypes[i]->G_option(optnr));
			vrtp->addchild(vr);
		}
		if (datatypes[i]->G_memberfunctioncount()>0)
		{
			TFuncTreeItem *subcat=vrtp->addcategory(_text("Attributes"));
			for (int j=0; j<datatypes[i]->G_memberfunctioncount(); j++)
			{
				QString cfname,cfreturndatatype,cfsubclass;
				TQXFormattedString cfprototype;
				datatypes[i]->G_memberfunctiondescription(j,cfname,cfprototype,cfreturndatatype,cfsubclass);
				TFuncTreeItem *myparent=subcat;
				if (qstrlen(cfsubclass)>0) myparent=subcat->addcategory(cfsubclass);
				TFuncTreeItem_CustomMemberfunction *custfn=new TFuncTreeItem_CustomMemberfunction(myparent,datatypes[i],cfsubclass,cfname,cfprototype);
				myparent->addchild(custfn);
				TSC_datatype *returndatatype=G_datatype(cfreturndatatype);
				if (returndatatype!=NULL)
				{
					for (int optnr=0; optnr<returndatatype->G_optioncount(); optnr++)
					{
						TFuncTreeItem_EnumVariant *vr=new TFuncTreeItem_EnumVariant(custfn,returndatatype,returndatatype->G_option(optnr));
						vr->HideFromReport();
						custfn->addchild(vr);
					}
				}
			}
		}
		for (int j=0; j<functions.G_count(); j++)
			if ((functions[j]->G_ismemberfunction())&&(functions[j]->G_vartype(0)==datatypes[i]))
			{
				TFuncTreeItem *posit=findtreepath(functreeroot,functions[j]->G_classpath());
				TFuncTreeItem_Memberfunction *mbr=new TFuncTreeItem_Memberfunction(posit,datatypes[i],functions[j]);
				posit->addchild(mbr);
			}
	}

	for (int i=0; i<functions.G_count(); i++)
		if (!functions[i]->G_ismemberfunction())
		{
			TFuncTreeItem *posit=findtreepath(functreeroot,functions[i]->G_classpath());
			TFuncTreeItem_Function *fnc=new TFuncTreeItem_Function(posit,functions[i]);
			posit->addchild(fnc);
		}

	for (int i=0; i<operators.G_count(); i++)
	{
		TFuncTreeItem *posit=findtreepath(functreeroot,operators[i]->G_classpath());
		posit=posit->addcategory(_qstr("Operators"));
		TFuncTreeItem_Operator *op=new TFuncTreeItem_Operator(posit,operators[i]);
		posit->addchild(op);
	}

	for (int i=0; i<typecasts.G_count(); i++)
	{
		QString path;
		TFuncTreeItem *posit=findtreepath(functreeroot,typecasts[i]->G_classpath());
		posit=posit->addcategory(_qstr("Type conversions"));
		TFuncTreeItem_Typecast *op=new TFuncTreeItem_Typecast(posit,typecasts[i]);
		posit->addchild(op);
	}

	addlog(_text("Function tree built"),-1);

	try{
		addlog(_text("Building function index"));
		functreeroot->buildindex();
	}
	catch (QError &err)
	{
		reporterror(err.G_content());
	}

	LoadDescriptions();
}

TSC_env::~TSC_env()
{
	delete functreeroot;
}


TSC_datatype* TSC_env::G_datatype(StrPtr iname)
{
	if (qstricmp(iname,SC_valname_scalar)==0) return GETDATATYPE_SCALAR;
	if (qstricmp(iname,SC_valname_string)==0) return GETDATATYPE_STRING;
	if (qstricmp(iname,SC_valname_boolean)==0) return GETDATATYPE_BOOLEAN;
	int nr=datatypesindex.get(iname);
	if (nr<0) return NULL;
	return datatypes[nr];
}

TSC_datatype* TSC_env::adddatatype(TSC_datatype *idatatype)
{
	ASSERT(datatypesindex.get(idatatype->G_name())<0);
	datatypes.add(idatatype);
	datatypesindex.add(idatatype->G_name(),datatypes.G_count()-1);
	return idatatype;
}

TSC_datatype_genericenum* TSC_env::addenumtype(TSC_datatype_genericenum *ienumtype)
{
	adddatatype(ienumtype);
	enumtypes.add(ienumtype);
	return ienumtype;
}



void TSC_env::addfunction(TSC_func *ifunc)
{
	QString storename=ifunc->G_name();
	if (ifunc->G_ismemberfunction())
	{
		ASSERT(ifunc->G_varcount()>0);
		storename+=_qstr("|");
		storename+=ifunc->G_vartype(0)->G_name();
	}
	storename.ToUpper();
	ASSERT(functionindex.get(storename)<0);
	functions.add(ifunc);
	functionindex.add(storename,functions.G_count()-1);
}

void TSC_env::addoperator(TSC_operator *ioperator)
{
	operators.add(ioperator);
}

void TSC_env::addtypecast(TSC_typecast *itypecast)
{
	typecasts.add(itypecast);
}


bool TSC_env::evalfunction(StrPtr funcname,TSC_value *returnval, TSC_value *assigntoval, TSC_funcarglist &arglist, QString &error)
{

	int funcnr=functionindex.get(funcname);
	if (funcnr>=0)
	{
		try
		{
			functions[funcnr]->execute(&arglist,returnval,assigntoval,NULL);
		}
		catch (QError &ferror)
		{
			error=ferror.G_content();
			return false;
		}
		return true;
	}

	//try as initialiser of a variable type
	TSC_datatype *datatype=G_datatype(funcname);
	if (datatype!=NULL)
	{
		returnval->settype(datatype);
		returnval->create();
		return true;
	}


	FormatString(error,_text("Unknown identifier '^1'"),funcname);
	return false;

}

bool TSC_env::evalmemberfunction(StrPtr funcname,TSC_value *returnval, TSC_value *leftval, TSC_value *assigntoval, TSC_funcarglist &arglist, QString &error)
{
	if (leftval==NULL) { error=_text("Member function: empty left value"); return false; }

	TSC_datatype *datatype=leftval->G_datatype();
	if (datatype==NULL) { 
		error=_text("Member function: undefined left value"); return false; 
	}

	if (qstricmp(funcname,_qstr("dispose"))==0)
	{
		leftval->dispose();
		return true;
	}

	if (leftval->memberfunction_accept(funcname))
		return leftval->evalmemberfunction(funcname,returnval,assigntoval,arglist,error);

	QString storename=funcname;
	storename+=_qstr("|");
	storename+=datatype->G_name();
	storename.ToUpper();


	int funcnr=functionindex.get(storename);
	if (funcnr<0)
	{
		FormatString(error,_text("Unknown member function '^1' of type '^2'"),funcname,datatype->G_name());
		return false;
	}

	try
	{
		functions[funcnr]->execute(&arglist,returnval,assigntoval,leftval);
	}
	catch (QError &ferror)
	{
		error=ferror.G_content();
		return false;
	}

	return true;
}

bool TSC_env::evaloperator(const TSC_ExpressionCacher *cache, int operatortype, TSC_value *val1, TSC_value *val2, TSC_value *retval, QString &error)
{
	if ((val1==NULL)||(val1->G_datatype()==NULL))
	{
		error=_text("Invalid left operator argument"); return false; 
	}
	if ((val2==NULL)||(val2->G_datatype()==NULL)) { error=_text("Invalid right operator argument"); return false; }

	//!!! todo: further optimization with index
	//or alternatively, try to cache from previous evaluation at this point?
	TSC_operator *op=NULL;

	if ((cache!=NULL)&&(cache->cached_operator!=NULL))
	{
		if (cache->cached_operator->G_opID()==operatortype)
		{
			op=cache->cached_operator;
			if ((!cache->cached_operator->G_vartype1()->G_isanytype())&&(!cache->cached_operator->G_vartype1()->issame(val1->G_datatype()))) op=NULL;
			if ((!cache->cached_operator->G_vartype2()->G_isanytype())&&(!cache->cached_operator->G_vartype2()->issame(val2->G_datatype()))) op=NULL;
		}
	}

	if (op==NULL)
	{
		for (int opnr=0; (opnr<operators.G_count())&&(op==NULL); opnr++)
			if (operators[opnr]->G_opID()==operatortype)
			{
				bool ok=true;
				if ((!operators[opnr]->G_vartype1()->G_isanytype())&&(!operators[opnr]->G_vartype1()->issame(val1->G_datatype()))) ok=false;
				if ((!operators[opnr]->G_vartype2()->G_isanytype())&&(!operators[opnr]->G_vartype2()->issame(val2->G_datatype()))) ok=false;
				if (ok) op=operators[opnr];
			}
		if (cache!=NULL)
			cache->cached_operator=op;
	}

	if (op==NULL)
	{
		FormatString(error,_text("Invalid arguments '^1' and '^2' for operator ^3"),
			val1->G_datatype()->G_name(),val2->G_datatype()->G_name(),G_operatortypename(operatortype));
		return false;
	}

	try
	{
		op->execute(val1,val2,retval);
	}
	catch (QError &ferror)
	{
		error=_text("Error while executing operator: ");
		error+=ferror.G_content();
		return false;
	}

	return true;
}


bool TSC_env::evaltypecast(StrPtr name, TSC_value *argval, TSC_value *retval, QString &error)
{
	if (argval->G_datatype()==NULL)
	{
		error=_text("Empty argument for type conversion");
		return false;
	}
	QString returntype=&name[1];
	TSC_typecast *tc=NULL;
	for (int tcnr=0; tcnr<typecasts.G_count(); tcnr++)
		if ((qstricmp(returntype,typecasts[tcnr]->G_returntype()->G_name())==0)&&(typecasts[tcnr]->G_argtype()->issame(argval->G_datatype())))
			tc=typecasts[tcnr];

	if (tc==NULL)
	{
		FormatString(error,_text("Invalid type conversion from '^1' to '^2'"),argval->G_datatype()->G_name(),returntype);
		return false;
	}

	try
	{
		tc->execute(argval,retval);
	}
	catch (QError &ferror)
	{
		error=_text("Error while executing type conversion: ");
		error+=ferror.G_content();
		return false;
	}

	return true;
}


void TSC_env::storevar(StrPtr iname, const TSC_value *ival)
{
	for (int i=0; i<storedvars.G_count(); i++)
		if (issame(iname,storedvars[i]->G_name()))
		{
			storedvars[i]->G_value()->copyfrom_flexibletype(ival);
			return;
		}

	TSC_value *newval=new TSC_value;
	newval->copyfrom(ival);
	TSC_script_var *var=new TSC_script_var(iname,newval);
	storedvars.add(var);
	
}

bool TSC_env::isvarstored(StrPtr iname)
{
	for (int i=0; i<storedvars.G_count(); i++)
		if (issame(iname,storedvars[i]->G_name())) return true;
	return false;
}

TSC_value* TSC_env::recallvar(StrPtr iname)
{
	for (int i=0; i<storedvars.G_count(); i++)
		if (issame(iname,storedvars[i]->G_name())) return storedvars[i]->G_value();
	return NULL;
}
