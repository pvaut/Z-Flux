#pragma once

#include "objecttree.h"
#include "qxformattedstring.h"

class TFuncTreeItem : public TObjectTreeItem
{
private:
	TFuncTreeItem *parent;
	Tarray<TFuncTreeItem> childs;
	bool showinreport;
public:
	TFuncTreeItem(TFuncTreeItem *iparent);
	virtual TObjectTreeItem *G_parent() { return parent; }
	virtual int G_childcount() { return childs.G_count(); }
	virtual TObjectTreeItem* G_child(int nr) { return childs[nr]; }
	void addchild(TFuncTreeItem *item) { childs.add(item); }
	TFuncTreeItem* addcategory(StrPtr iname);
	void reset() { childs.reset(); }
	void HideFromReport() { showinreport=false; }

	void buildindex();//adds to the index in the env

	void fillinfo(TSC_map *map);//fills info about this item
	void fillist(TSC_list *lst);//fills a list with function info & all descendant function infos
	void exportdoc(QTextfile &fl, int level);//export the documentation as LaTex file


public:
	virtual StrPtr G_type() { return _qstr(""); }
	virtual void G_fullID(QString &fullID) {};
	virtual void G_description(QString &str);
	virtual void fillinfo_impl(TSC_map *map) {};
};


class TFuncTreeItem_Category : public TFuncTreeItem
{
private:
	QString name,parentfullID;
public:
	TFuncTreeItem_Category(TFuncTreeItem *iparent, StrPtr iname, StrPtr iparentfullID) : TFuncTreeItem(iparent)
	{
		name=iname;
		parentfullID=iparentfullID;
	}
	virtual void G_name(QString &str)
	{
		str=name;
	}
	virtual void G_displayname(TQXFormattedString &fstr)
	{
		fstr.clear();
		fstr.makeitalic(true);
		fstr+=name;
//		fstr+=_qstr(" >");
	}
	virtual StrPtr G_type() { return _qstr("Category"); }
	virtual void G_fullID(QString &fullID);
};


class TFuncTreeItem_Vartype : public TFuncTreeItem
{
private:
	TSC_datatype *datatype;
public:
	TFuncTreeItem_Vartype(TFuncTreeItem *iparent, TSC_datatype *idatatype);
	virtual void G_name(QString &str);
	virtual StrPtr G_type() { return _qstr("Type"); }
	virtual void G_fullID(QString &fullID);
	virtual void G_sourcecodename(QString &str);//name as it should be copied to the source code
};


class TFuncTreeItem_Memberfunction : public TFuncTreeItem
{
private:
	TSC_datatype *datatype;
	TSC_func *func;
public:
	TFuncTreeItem_Memberfunction(TFuncTreeItem *iparent, TSC_datatype *idatatype, TSC_func *ifunc);
	virtual void G_name(QString &str);
	virtual void G_displayname(TQXFormattedString &fstr);
	virtual StrPtr G_type() { return _qstr("Member function"); }
	virtual void G_fullID(QString &fullID);
	virtual void G_sourcecodename(QString &str);//name as it should be copied to the source code
};


class TFuncTreeItem_EnumVariant : public TFuncTreeItem
{
private:
	QString name;
	QString parentfullID;
public:
	TFuncTreeItem_EnumVariant(TFuncTreeItem *iparent, TSC_datatype *idatatype, StrPtr iname) : TFuncTreeItem(iparent)
	{
		idatatype->G_fullID(parentfullID);
		name=iname;
	}
	virtual void G_name(QString &str)
	{
		str=name;
	}
	virtual void G_displayname(TQXFormattedString &fstr)
	{
		fstr.clear();
		fstr+=name;
	}
	virtual StrPtr G_type() { return _qstr("Variant"); }
	virtual void G_fullID(QString &fullID)
	{
		fullID=parentfullID;
		fullID+=_qstr("|");
		fullID+=name;
	}
	virtual void G_sourcecodename(QString &str)//name as it should be copied to the source code
	{
		str=name;
	}
};


class TFuncTreeItem_CustomMemberfunction : public TFuncTreeItem
{
private:
	QString parentclassname,name;
	TQXFormattedString prototype;
	QString myfullID;
public:
	TFuncTreeItem_CustomMemberfunction(TFuncTreeItem *iparent, TSC_datatype *idatatype, StrPtr isubclass, StrPtr iname, TQXFormattedString &iprototype);
	virtual void G_name(QString &str);
	virtual void G_displayname(TQXFormattedString &fstr);
	virtual StrPtr G_type() { return _qstr("Member function"); }
	virtual void G_fullID(QString &fullID);
	virtual void G_sourcecodename(QString &str);//name as it should be copied to the source code
};

class TFuncTreeItem_Function : public TFuncTreeItem
{
private:
	TSC_func *func;
public:
	TFuncTreeItem_Function(TFuncTreeItem *iparent, TSC_func *ifunc);
	virtual void G_name(QString &str);
	virtual void G_displayname(TQXFormattedString &fstr);
	virtual StrPtr G_type() { return _qstr("Function"); }
	virtual void fillinfo_impl(TSC_map *map);
	virtual void G_fullID(QString &fullID);
	virtual void G_sourcecodename(QString &str);//name as it should be copied to the source code
};

class TFuncTreeItem_Operator : public TFuncTreeItem
{
private:
	TSC_operator *op;
public:
	TFuncTreeItem_Operator(TFuncTreeItem *iparent, TSC_operator *iop);
	virtual void G_name(QString &str);
	virtual void G_displayname(TQXFormattedString &fstr);
	virtual StrPtr G_type() { return _qstr("Operator"); }
	virtual void G_fullID(QString &fullID);
};

class TFuncTreeItem_Typecast : public TFuncTreeItem
{
private:
	TSC_typecast *tc;
public:
	TFuncTreeItem_Typecast(TFuncTreeItem *iparent, TSC_typecast *itc);
	virtual void G_name(QString &str);
	virtual StrPtr G_type() { return _qstr("Type Cast"); }
	virtual void G_fullID(QString &fullID);
};