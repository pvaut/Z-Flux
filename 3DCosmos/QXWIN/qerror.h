#pragma once

#include<string>
#include<exception>

#include "qstring.h"


class QError: public std::exception{
private:
	QString content;
public:
	QError(StrPtr str);
	virtual StrPtr G_content()
	{
		return content;
	}
};


void reporterror(CWnd *win, QError &err);
void reporterror(QError &err);

void reporterror(CWnd *win, StrPtr err);
void reporterror(StrPtr err);

void posterror(StrPtr err);

void throw_GetLastError(StrPtr comment);

