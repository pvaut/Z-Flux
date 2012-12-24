#include "stdafx.h"

#include "qstring.h"
#include "tools.h"
#include "qstringformat.h"


class TMformat_part_string : public TMformat_part
{
private:
	QString str;
public:
	TMformat_part_string(StrPtr istr) { str=istr; };
	StrPtr G_string() { return str; };
};

class TMformat_part_int : public TMformat_part
{
private:
	CString str;
public:
	TMformat_part_int(int vl) { str.Format(_qstr("%d"),vl); };
	StrPtr G_string() { return str; };
};

class TMformat_part_double : public TMformat_part
{
private:
	CString str;
public:
	TMformat_part_double(double vl) { str.Format(_qstr("%0.2lf"),vl); };
	StrPtr G_string() { return str; };
};




void TMformat::Astring(StrPtr str)
{
	components.add(new TMformat_part_string(str));
}

void TMformat::Aint(int vl)
{
	components.add(new TMformat_part_int(vl));
}

void TMformat::Adouble(double vl)
{
	components.add(new TMformat_part_double(vl));
}



void TMformat::format(StrPtr iformat, QString &dest)
{
	QString left,str2;
	QString format=iformat;
	dest.clear();
	while (format.G_length()>0)
	{
		format.splitstring(_qstr("^"),left);
		dest+=left;
		if (format.G_length()>0)
		{
			str2=format;str2.substring(0,0);
			int nr=qstr2int(str2);
			if ((nr>0)&&(nr<=components.G_count())) dest+=components[nr-1]->G_string();
			format.substring(1,format.G_length()-1);
		}
	}
}



void FormatString(QString &result, StrPtr iformat, StrPtr arg1)
{
	TMformat fmt;
	fmt.Astring(arg1);
	fmt.format(iformat,result);
}

void FormatString(QString &result, StrPtr iformat, StrPtr arg1, StrPtr arg2)
{
	TMformat fmt;
	fmt.Astring(arg1);
	fmt.Astring(arg2);
	fmt.format(iformat,result);
}

void FormatString(QString &result, StrPtr iformat, StrPtr arg1, StrPtr arg2, StrPtr arg3)
{
	TMformat fmt;
	fmt.Astring(arg1);
	fmt.Astring(arg2);
	fmt.Astring(arg3);
	fmt.format(iformat,result);
}

void FormatString(QString &result, StrPtr iformat, int arg1)
{
	TMformat fmt;
	fmt.Aint(arg1);
	fmt.format(iformat,result);
}

void FormatString(QString &result, StrPtr iformat, int arg1, int arg2)
{
	TMformat fmt;
	fmt.Aint(arg1);
	fmt.Aint(arg2);
	fmt.format(iformat,result);
}

////////////////////////////////////////////////////////////////////////
// TFormatString
////////////////////////////////////////////////////////////////////////

TFormatString::TFormatString(StrPtr format)
{
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, StrPtr s_arg1)
{
	fm.Astring(s_arg1);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, StrPtr s_arg1, StrPtr s_arg2)
{
	fm.Astring(s_arg1);
	fm.Astring(s_arg2);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, StrPtr s_arg1, StrPtr s_arg2, StrPtr s_arg3)
{
	fm.Astring(s_arg1);
	fm.Astring(s_arg2);
	fm.Astring(s_arg3);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, StrPtr s_arg1, StrPtr s_arg2, StrPtr s_arg3, StrPtr s_arg4)
{
	fm.Astring(s_arg1);
	fm.Astring(s_arg2);
	fm.Astring(s_arg3);
	fm.Astring(s_arg4);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, int i_arg1)
{
	fm.Aint(i_arg1);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, int i_arg1, int i_arg2)
{
	fm.Aint(i_arg1);
	fm.Aint(i_arg2);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, StrPtr s_arg1, StrPtr s_arg2, int i_arg3)
{
	fm.Astring(s_arg1);
	fm.Astring(s_arg2);
	fm.Aint(i_arg3);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, StrPtr s_arg1, int i_arg2, StrPtr s_arg3)
{
	fm.Astring(s_arg1);
	fm.Aint(i_arg2);
	fm.Astring(s_arg3);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, StrPtr s_arg1, int i_arg2)
{
	fm.Astring(s_arg1);
	fm.Aint(i_arg2);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, double d_arg1)
{
	fm.Adouble(d_arg1);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, double d_arg1, double d_arg2)
{
	fm.Adouble(d_arg1);
	fm.Adouble(d_arg2);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, double d_arg1, double d_arg2, double d_arg3)
{
	fm.Adouble(d_arg1);
	fm.Adouble(d_arg2);
	fm.Adouble(d_arg3);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, int i_arg1, double d_arg2, double d_arg3)
{
	fm.Aint(i_arg1);
	fm.Adouble(d_arg2);
	fm.Adouble(d_arg3);
	fm.format(format,str);
}



TFormatString::TFormatString(StrPtr format, StrPtr s_arg1, double d_arg2, StrPtr s_arg3)
{
	fm.Astring(s_arg1);
	fm.Adouble(d_arg2);
	fm.Astring(s_arg3);
	fm.format(format,str);
}



TFormatString::TFormatString(StrPtr format, double d_arg1, int i_arg2)
{
	fm.Adouble(d_arg1);
	fm.Aint(i_arg2);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, StrPtr s_arg1, double d_arg2)
{
	fm.Astring(s_arg1);
	fm.Adouble(d_arg2);
	fm.format(format,str);
}

TFormatString::TFormatString(StrPtr format, StrPtr s_arg1, double d_arg2, double d_arg3)
{
	fm.Astring(s_arg1);
	fm.Adouble(d_arg2);
	fm.Adouble(d_arg3);
	fm.format(format,str);
}


TFormatString::~TFormatString()
{
}



TFormatString::operator StrPtr ()
{
	return str;
}
