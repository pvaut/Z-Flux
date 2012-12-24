#pragma once

class TSC_boolean : public TObject
{
private:
	bool value;
public:
	TSC_boolean()
	{
		value=true;
	}
	TSC_boolean(bool ival)
	{
		value=ival;
	}
	void tostring(QString &str, int tpe)
	{
		if (value) str=_qstr("True"); else str=_qstr("False");
	}
	void fromstring(StrPtr icontent)
	{
		value=(qstricmp(icontent,_qstr("True"))==0);
	}
	void copyfrom(bool ival)
	{
		value=ival;
	}
	void copyfrom(TSC_boolean *ivl)
	{
		value=ivl->value;
	}
	bool G_val() { return value; }
	static bool compare(TSC_boolean *val1, TSC_boolean *val2)
	{
		return val1->value==val2->value;
	}
	static bool comparesize(TSC_boolean *val1, TSC_boolean *val2)
	{
		return val1->value<val2->value;
	}
	void operator=(TSC_boolean &obj)
	{
		value=obj.value;
	}

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

};
