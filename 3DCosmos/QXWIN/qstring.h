#pragma once


#define StrPtr const WCHAR*
#define StrChar WCHAR

#define _qstr _T

//reserved for user-visible text in the source code
#define _text _T


void addlog(StrPtr content, int incrpos=0);


class QString
{
private:
	CString str;
public:
	QString()
	{
	}
	QString(const char *istr)
	{
		str=istr;
	}
	QString(StrPtr istr)
	{
		str=istr;
	}
	QString(const CString &istr)
	{
		str=istr;
	}
	QString(int vl)
	{
		str.Format(_qstr("%d"),vl);
	}
	void clear()
	{
		str=_qstr("");
	}
	void ToUpper()
	{
		str.MakeUpper();
	}
	void Replace(StrPtr from, StrPtr to);
	void formatscalar(double val);
	void formatscalar(double val, int numdecims);
	void operator=(StrPtr ival)
	{
		str=ival;
	}
	void operator=(QString ival)
	{
		str=ival.str;
	}
	void operator+=(StrPtr ival)
	{
		str=str+ival;
	}
	void operator+=(const QString &ival)
	{
		if (ival.str.GetLength()>0)
			str=str+ival.str;
	}
	void operator+=(StrChar ival)
	{
		str=str+ival;
	}
	operator StrPtr() const
	{
		return str;
	};
	bool IsEmpty()
	{
		return str[0]!=0;
	}
	int G_length() const
	{
		return str.GetLength();
	}
	StrChar G_char(int pos) const
	{
		if (pos<0) return 0;
		return str[pos];
	}
	const StrChar* G_ptr()
	{
		return str;
	}
	operator CString&()
	{
		return str;
	}
	void substring(int start, int stop)
	{
		str=str.Mid(start,stop-start+1);
	}
	void splitstring(StrPtr sep, QString &firstpart);

	int findfirst(StrPtr target) const
	{
		return str.Find(target);
	}

	void replace(StrPtr source, StrPtr dest)
	{
		str.Replace(source,dest);
	}

	void invert()
	{
		str.MakeReverse();
	}


};

int qstrnicmp(StrPtr str1, StrPtr str2, int count);
int qstrncmp(StrPtr str1, StrPtr str2, int count);
int qstricmp(StrPtr str1, StrPtr str2);
int qstrlen(StrPtr str1);
int qfind(StrPtr txt, StrPtr fnd);
bool qisempty(StrPtr txt);

bool isquote(StrChar ch);
bool isquotesingle(StrChar ch);
bool isquotedouble(StrChar ch);

bool issame(StrPtr str1, StrPtr str2);

bool Qisalphanumerical(StrChar ch);

QString qstr(int vl);

int qstr2int(StrPtr str);
double qstr2double(StrPtr str);

QString operator+(QString st1, StrPtr st2);



StrPtr _TRL(StrPtr inp);

