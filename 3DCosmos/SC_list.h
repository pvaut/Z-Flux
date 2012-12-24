#pragma once

#include "qbintagfile.h"

class TSC_list : public TObject
{
private:
	Tarray<TSC_value> items;
public:
	static StrPtr GetClassName() { return SC_valname_list; }
	TSC_list()
	{
	}
	void fromstring(StrPtr icontent)
	{
	}
	void tostring(QString &str, int tpe);
	static bool compare(TSC_list *val1, TSC_list *val2)
	{
		return false;
	}
	static bool comparesize(TSC_list *val1, TSC_list *val2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
	void operator=(const TSC_list &m);
	void add(const TSC_value *ival);
	void del(int nr);
	TSC_value* get(int nr);
	void grow(int cnt);
	void reset() { items.reset(); }
	int G_size() { return items.G_count(); }

	void mergefrom(TSC_list *l1, TSC_list *l2);

	void invert();

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);


	void sort(Tintarray &idx);

};
