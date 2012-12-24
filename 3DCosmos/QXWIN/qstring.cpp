
#include "stdafx.h"

#include "qstring.h"


void QString::formatscalar(double val)
{
	int numdecims=0;
	int ok=0;
	TCHAR formatstr[100];
	while(!ok)
	{
		_stprintf_s(formatstr,_qstr("%%0.%dlf"),numdecims);
		str.Format(formatstr,val);
		if (fabs(val-_tstof(str))<0.000000001) ok=1;
		if (numdecims>11) ok=1;
		numdecims++;
	}
}

void QString::Replace(StrPtr from, StrPtr to)
{
	str.Replace(from,to);
}


void QString::formatscalar(double val, int numdecims)
{
	TCHAR formatstr[100];
	_stprintf_s(formatstr,_qstr("%%0.%dlf"),numdecims);
	str.Format(formatstr,val);
}


void QString::splitstring(StrPtr sep, QString &firstpart)
{
	int ps=str.Find(sep);
	if (ps<0)
	{
		firstpart=(StrPtr)str;
		clear();
		return;
	}
	firstpart=(StrPtr)str.Left(ps);
	str=str.Mid(ps+qstrlen(sep));
}




int qstricmp(StrPtr str1, StrPtr str2)
{
	return _wcsicmp(str1,str2);
}

int qstrnicmp(StrPtr str1, StrPtr str2, int count)
{
	return _wcsnicmp(str1,str2,count);
}

int qstrncmp(StrPtr str1, StrPtr str2, int count)
{
	return wcsncmp(str1,str2,count);
}


int qstrlen(StrPtr str1)
{
	return (int)wcslen(str1);
}

bool qisempty(StrPtr txt)
{
	return txt[0]==0;
}

bool issame(StrPtr str1, StrPtr str2)
{
	return qstricmp(str1,str2)==0;
}

bool isquote(StrChar ch)
{
	if (ch=='"') return true;
	if (ch=='\'') return true;
	return false;
}

bool isquotesingle(StrChar ch)
{
	if (ch=='\'') return true;
	return false;
}

bool isquotedouble(StrChar ch)
{
	if (ch=='"') return true;
	return false;
}


int qfind(StrPtr txt, StrPtr fnd)
{
	StrPtr posit=wcsstr(txt,fnd);
	if (posit==NULL) return -1;
	return posit-txt;
}


QString qstr(int vl)
{
	CString str;
	str.Format(_T("%d"),vl);
	return QString(str);
}

QString operator+(QString st1, StrPtr st2)
{
	CString str;
	str.Format(_T("%s%s"),st1,st2);
	return QString(str);
}


int qstr2int(StrPtr str)
{
	return _tstoi(str);
}


double qstr2double(StrPtr str)
{
	return _tstof(str);
}


bool Qisalphanumerical(StrChar ch)
{
	if ((ch>='a')&&(ch<='z')) return true;
	if ((ch>='A')&&(ch<='Z')) return true;
	if ((ch>='0')&&(ch<='9')) return true;
	if (ch=='_') return true;
	if (ch=='@') return true;
	return false;
}
