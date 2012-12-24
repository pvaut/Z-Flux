#pragma once

#include "qstring.h"

#define FILEPOS long

StrPtr G_softwarename();

void GetDocumentsDir(QString &name);

bool IsFilePresent(StrPtr name);


class QTextfile
{
private:
	QString filename;
	FILE *fp;
	StrChar *buff;
	const static int bufflen=20000;
public:
	QTextfile();
	~QTextfile();
	void openread(StrPtr ifilename);
	void openwrite(StrPtr ifilename);
	bool isend();
	void close();
	void readline(QString &line);
	void write(StrPtr line);
	void writeline();
};


class QBinfile
{
private:
	QString filename;
public:
	FILE *fp;
public:
	QBinfile();
	~QBinfile();
	void openread(StrPtr ifilename);
	void openwrite(StrPtr ifilename);
	void close();
	void readblock(int size, void *ptr);
	void writeblock(int size, const void *ptr);
	FILEPOS G_filepos();
	void Set_filepos(FILEPOS ipos);
};



class QFileFind
{
public:
	QFileFind();
	~QFileFind();
public:
	bool ispresent;
	QString name;
	bool isdir,ishidden;
	bool start(StrPtr path);
	bool getnext(QString &i_name, bool &i_isdir, bool &i_ishidden);
	void stop();
private:
	HANDLE hnd;
};


enum QParamLocation { PARAMLOCATION_FILE, PARAMLOCATION_REGISTRY };

void QParamStore(QParamLocation location, StrPtr name, StrPtr content);
bool QParamRead(QParamLocation location, StrPtr name, QString &content);

void QParamStore(QParamLocation location, StrPtr name, bool content);
bool QParamRead(QParamLocation location, StrPtr name, bool &content);

void QParamStore(QParamLocation location, StrPtr name, int content);
bool QParamRead(QParamLocation location, StrPtr name, int &content);
