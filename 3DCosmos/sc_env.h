#pragma once

#include "tools.h"

#include "SC_value.h"

#include "SC_boolean.h"
#include "SC_list.h"
#include "SC_map.h"
#include "SC_color.h"
#include "SC_datatype_enum.h"


#include "SC_func.h"
#include "SC_operator.h"
#include "SC_typecast.h"
#include "SC_ExpressionCacher.h"

#include "SC_functor.h"
#include "SC_forcefield.h"

class TFuncTreeItem;
class TSC_datatype_genericenum;
class TSC_script_var;


class TSC_env
{
private:
	Tbaltree datatypesindex;
	Tarray<TSC_datatype> datatypes;
	Tcopyarray<TSC_datatype_genericenum> enumtypes;
private:
	Tbaltree functionindex;
	Tarray<TSC_func> functions;
	Tarray<TSC_operator> operators;
	Tarray<TSC_typecast> typecasts;
private:
	TSC_datatype *datatype_scalar,*datatype_string,*datatype_boolean;
public:
	TSC_env();
	~TSC_env();
	TSC_datatype* G_datatype(StrPtr iname);
	TSC_datatype* G_datatype_scalar() { return datatype_scalar; };
	TSC_datatype* G_datatype_string() { return datatype_string; };
	TSC_datatype* G_datatype_boolean() { return datatype_boolean; };

	int G_enumtypecount() { return enumtypes.G_count(); }
	TSC_datatype_genericenum* G_enumtype(int nr)
	{
		ASSERT((nr>=0)&&(nr<G_enumtypecount()));
		if ((nr<0)||(nr>=G_enumtypecount())) return NULL;
		return enumtypes[nr];
	}

	static TSC_env& Get()
	{
		 static TSC_env env;
		 return env;
	}
public:
	int G_functioncount() { return functions.G_count(); }
	TSC_func* G_function(int nr) { return functions[nr]; }
	int G_typecastcount() { return typecasts.G_count(); }
	TSC_typecast* G_typecast(int nr) { return typecasts[nr]; }

private:
	TFuncTreeItem *functreeroot;
	Tcopyarray<TFuncTreeItem> functreeitemlist;
	Tbaltree functreeindex;
public:
	TFuncTreeItem* G_functreeroot() { return functreeroot; }
	void addfunctreeitem(StrPtr fullID, TFuncTreeItem *item);
	TFuncTreeItem *G_functreeitem(StrPtr fullID);

public:
	TSC_datatype* adddatatype(TSC_datatype *idatatype);
	TSC_datatype_genericenum* addenumtype(TSC_datatype_genericenum *ienumtype);
	void addfunction(TSC_func *ifunc);
	void addoperator(TSC_operator *ioperator);
	void addtypecast(TSC_typecast *itypecast);
	void buildfunctiontree();

public:
	bool evalfunction(StrPtr funcname,TSC_value *returnval, TSC_value *assigntoval, TSC_funcarglist &arglist, QString &error);
	bool evalmemberfunction(StrPtr funcname,TSC_value *returnval, TSC_value *leftval, TSC_value *assigntoval, TSC_funcarglist &arglist, QString &error);
	bool evaloperator(const TSC_ExpressionCacher *cache, int operatortype, TSC_value *val1, TSC_value *val2, TSC_value *retval, QString &error);
	bool evaltypecast(StrPtr name, TSC_value *argval, TSC_value *retval, QString &error);
private:
	Tarray<TSC_script_var> storedvars;
public:
	void storevar(StrPtr iname, const TSC_value *ival);
	bool isvarstored(StrPtr iname);
	TSC_value* recallvar(StrPtr iname);

private:
	mutable Tbaltree descriptindex;
	Tarray<QString> descriptID,descriptcontent;
public:
	void AddDescription(StrPtr iID, StrPtr icontent);
	void LoadDescriptions();
	void SaveDescriptions();
	StrPtr G_description(StrPtr iID) const;
	void G_ParsedDescription(StrPtr iID, QString &descr) const;
private://determines what objects get empty descriptions from other objects
	mutable Tbaltree descriptmapindex;
	Tarray<QString> descriptmapobject;
public:
	void AddDescriptMapObject(StrPtr ichild, StrPtr iparent);
	StrPtr G_DescriptMapObject(StrPtr ichild) const;
	void InitDescriptMappings();

};

TSC_env& GetTSCenv();

#define GETDATATYPE_ANY (GetTSCenv().G_datatype(SC_valname_any))
#define GETDATATYPE_LIST (GetTSCenv().G_datatype(SC_valname_list))
#define GETDATATYPE_STRING (GetTSCenv().G_datatype_string())
#define GETDATATYPE_SCALAR (GetTSCenv().G_datatype_scalar())
#define GETDATATYPE_BOOLEAN (GetTSCenv().G_datatype_boolean())
/*#define GETDATATYPE_VERTEX (GetTSCenv().G_datatype(SC_valname_vertex))
#define GETDATATYPE_MATRIX (GetTSCenv().G_datatype(SC_valname_matrix))
#define GETDATATYPE_OBJECT (GetTSCenv().G_datatype(SC_valname_object))*/
