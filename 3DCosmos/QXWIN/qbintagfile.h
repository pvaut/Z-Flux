#pragma once

#include "tobject.h"
#include "tools.h"

#include "binstream.h"
#include "qfile.h"

#define TAGSIZE unsigned __int32

class QBinTagFileWriter;
class QBinTagWriter;
class QBinTagFileReader;
class QBinTagReader;

typedef TSpecificObjectRef<QBinTagWriter> QBinTagWriterRef;

class QBinTagWriter : public TObject, public QBinStreamOut
{
private:
	QString name;
	QBinTagFileWriter &file;
	QBinTagWriterRef child;
	QBinTagWriter *parent;
	FILEPOS startpos;
public:
	QBinTagWriter(QBinTagFileWriter &ifile, QBinTagWriter *iparent, StrPtr iname);
	~QBinTagWriter();
	void close() { delete this; }
	QBinTagWriterRef subtag(StrPtr itagname);

	void write_block(const void *ptr, int len);
	void write_shortstring(StrPtr istr);
	void write_string(StrPtr istr);
	void write_int32(__int32 ivl);
	void write_double(double ivl);
	void write_bool(bool ivl);
};


class QBinTagFileWriter
{
	friend class QBinTagWriter;
private:
	QBinfile file;
	QBinTagWriter *writer;
public:
	QBinTagFileWriter();
	~QBinTagFileWriter();
	QBinTagWriterRef open(StrPtr ifilename);
public:
	void write_block(const void *ptr, int len);
	void write_shortstring(StrPtr istr);
	void write_string(StrPtr istr);
	void write_int32(__int32 ivl);
	void write_double(double ivl);
	void write_bool(bool ivl);
};

typedef TSpecificObjectRef<QBinTagReader> QBinTagReaderRef;

class QBinTagReader : public TObject, public QBinStreamIn
{
private:
	QString name;
	QBinTagFileReader &file;
	QBinTagReaderRef child;
	QBinTagReader *parent;
	TAGSIZE tagsize;
	FILEPOS startpos;
public:
	QBinTagReader(QBinTagFileReader &ifile, QBinTagReader *iparent);
	~QBinTagReader();
	StrPtr G_name();
	bool Istag(StrPtr iname);
	void close();
	bool hassubtags();
	QBinTagReaderRef getnextsubtag();

	void read_block(void *ptr, int len);
	void read_shortstring(QString &istr);
	void read_string(QString &istr);
	__int32 read_int32();
	double read_double();
	bool read_bool();

};


class QBinTagFileReader
{
	friend class QBinTagReader;
private:
	QBinfile file;
	QBinTagReader *reader;
public:
	QBinTagFileReader();
	~QBinTagFileReader();
	QBinTagReaderRef open(StrPtr ifilename);
public:
	void read_block(void *ptr, int len);
	void read_shortstring(QString &istr);
	void read_string(QString &istr);
	__int32 read_int32();
	double read_double();
	bool read_bool();
};

