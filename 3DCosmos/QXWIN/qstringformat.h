#pragma once

#include "qstring.h"
#include "tools.h"

class TMformat_part
{
public:
	virtual StrPtr G_string()=0;
	virtual ~TMformat_part() {};
};

class TMformat
{
private:
	Tarray<TMformat_part> components;
public:
//	TMformat();
	void Astring(StrPtr str);
	void Aint(int vl);
	void Adouble(double vl);
	void format(StrPtr iformat, QString &dest);
};


void FormatString(QString &result, StrPtr iformat, StrPtr arg1);
void FormatString(QString &result, StrPtr iformat, StrPtr arg1, StrPtr arg2);
void FormatString(QString &result, StrPtr iformat, StrPtr arg1, StrPtr arg2, StrPtr arg3);

void FormatString(QString &result, StrPtr iformat, int arg1);
void FormatString(QString &result, StrPtr iformat, int arg1, int arg2);

class TFormatString
{
private:
	TMformat fm;
	QString str;
public:
	TFormatString(StrPtr format);

	TFormatString(StrPtr format, StrPtr s_arg1);
	TFormatString(StrPtr format, StrPtr s_arg1, StrPtr s_arg2);
	TFormatString(StrPtr format, StrPtr s_arg1, StrPtr s_arg2, StrPtr s_arg3);
	TFormatString(StrPtr format, StrPtr s_arg1, StrPtr s_arg2, StrPtr s_arg3, StrPtr s_arg4);

	TFormatString(StrPtr format, int i_arg1);
	TFormatString(StrPtr format, int i_arg1, int i_arg2);
	TFormatString(StrPtr format, StrPtr s_arg1, StrPtr s_arg2, int i_arg3);
	TFormatString(StrPtr format, StrPtr s_arg1, int i_arg2, StrPtr s_arg3);
	TFormatString(StrPtr format, StrPtr s_arg1, int i_arg2);

	TFormatString(StrPtr format, StrPtr s_arg1, double d_arg2);
	TFormatString(StrPtr format, StrPtr s_arg1, double d_arg2, double d_arg3);
	TFormatString(StrPtr format, StrPtr s_arg1, double d_arg2, StrPtr s_arg3);
	TFormatString(StrPtr format, int i_arg1, double d_arg2, double d_arg3);

	TFormatString(StrPtr format, double d_arg1);
	TFormatString(StrPtr format, double d_arg1, double d_arg2);
	TFormatString(StrPtr format, double d_arg1, double d_arg2, double d_arg3);
	TFormatString(StrPtr format, double d_arg1, int i_arg2);

	~TFormatString();

   operator StrPtr();

};
