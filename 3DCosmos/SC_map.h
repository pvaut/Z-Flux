#pragma once

#include "qbintagfile.h"

class TSC_value;

class TSC_mappair : public TObject
{
	friend class TSC_map;
private:
	QString name;
	TSC_value *item;
public:
	static StrPtr GetClassName() { return SC_valname_mappair; }
	TSC_mappair();
	~TSC_mappair();
	void fromstring(StrPtr icontent);
	void tostring(QString &str, int tpe);
	static bool compare(TSC_mappair *val1, TSC_mappair *val2)
	{
		return false;
	}
	static bool comparesize(TSC_mappair *val1, TSC_mappair *val2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
	void operator=(const TSC_mappair &m);
	void import(StrPtr i_name, const TSC_value *i_item);
	StrPtr G_name() { return name; }
	TSC_value* G_item() { return item; }

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

};


class TSC_map : public TObject
{
private:
	Tarray<TSC_mappair> items;
	mutable Tbaltree index;
public:
	static StrPtr GetClassName() { return SC_valname_map; }
	TSC_map()
	{
	}
	void fromstring(StrPtr icontent)
	{
	}
	void tostring(QString &str, int tpe);
	static bool compare(TSC_map *val1, TSC_map *val2)
	{
		return false;
	}
	static bool comparesize(TSC_map *val1, TSC_map *val2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
	void operator=(const TSC_map &m);
	void Set(StrPtr iname, const TSC_value *ival);
	void SetString(StrPtr iname, StrPtr icontent);
//	void del(StrPtr iname);
	TSC_value* get(StrPtr iname);
	int G_itemcount() const { return items.G_count(); }
	StrPtr G_itemname(int nr) const;
	bool IsItemPresent(StrPtr iname) const;
	void reset();

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

};
