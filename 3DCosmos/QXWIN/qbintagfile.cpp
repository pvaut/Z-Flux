#include "stdafx.h"

#include "qbintagfile.h"
#include "qerror.h"


///////////////////////////////////////////////////////////////////////
// QBinTagWriter
///////////////////////////////////////////////////////////////////////


QBinTagWriter::QBinTagWriter(QBinTagFileWriter &ifile, QBinTagWriter *iparent, StrPtr iname)
:  file(ifile)
{
	name=iname;
	parent=iparent;
	file.file.writeblock(2,"TG");
	startpos=file.file.G_filepos();

	TAGSIZE tagsize0=0;
	file.file.writeblock(sizeof(TAGSIZE),&tagsize0);

	file.write_shortstring(name);
}

QBinTagWriter::~QBinTagWriter()
{
	if (child.G_ref()!=NULL) delete child.G_ref();
	FILEPOS endpos=file.file.G_filepos();
	file.file.Set_filepos(startpos);
	TAGSIZE tagsize=endpos-startpos;
	file.file.writeblock(sizeof(TAGSIZE),&tagsize);
	file.file.Set_filepos(endpos);
}

QBinTagWriterRef QBinTagWriter::subtag(StrPtr itagname)
{
	if (child.G_ref()!=NULL)
	{
		ASSERT(false);
		throw QError(_text("Unable to create new sub tag: current sub tag is still active"));
	}
	QBinTagWriter *subtag=new QBinTagWriter(file,this,itagname);
	child.setreffor(subtag);
	return child;
}

void QBinTagWriter::write_block(const void *ptr, int len)
{
	file.write_block(ptr,len);
}


void QBinTagWriter::write_shortstring(StrPtr istr)
{
	file.write_shortstring(istr);
}

void QBinTagWriter::write_string(StrPtr istr)
{
	file.write_string(istr);
}

void QBinTagWriter::write_int32(__int32 ivl)
{
	file.write_int32(ivl);
}

void QBinTagWriter::write_double(double ivl)
{
	file.write_double(ivl);
}

void QBinTagWriter::write_bool(bool ivl)
{
	file.write_bool(ivl);
}



///////////////////////////////////////////////////////////////////////
// QBinTagFileWriter
///////////////////////////////////////////////////////////////////////


QBinTagFileWriter::QBinTagFileWriter()
{
	writer=NULL;
}

QBinTagFileWriter::~QBinTagFileWriter()
{
	if (writer!=NULL) { delete writer; writer=NULL; }
	file.close();
}

QBinTagWriterRef QBinTagFileWriter::open(StrPtr ifilename)
{
	if (writer!=NULL) throw QError(_text("Binary tag file writer is already open"));
	file.openwrite(ifilename);
	writer=new QBinTagWriter(*this,NULL,_qstr("Root"));
	QBinTagWriterRef ref;
	ref.setreffor(writer);
	return ref;
}

void QBinTagFileWriter::write_block(const void *ptr, int len)
{
	file.writeblock(len,ptr);
}


void QBinTagFileWriter::write_shortstring(StrPtr istr)
{
	int len=qstrlen(istr);
	if (len>=255) throw QError(_text("QBinTagFileWriter: short string is too long"));
	unsigned __int8 slen=len;
	file.writeblock(1,&slen);
	file.writeblock(slen*sizeof(StrChar),istr);
}

void QBinTagFileWriter::write_string(StrPtr istr)
{
	__int32 len=qstrlen(istr);
	file.writeblock(4,&len);
	file.writeblock(len*sizeof(StrChar),istr);
}

void QBinTagFileWriter::write_int32(__int32 ivl)
{
	file.writeblock(4,&ivl);
}

void QBinTagFileWriter::write_double(double ivl)
{
	file.writeblock(sizeof(double),&ivl);
}

void QBinTagFileWriter::write_bool(bool ivl)
{
	file.writeblock(sizeof(bool),&ivl);
}


///////////////////////////////////////////////////////////////////////
// QBinTagReader
///////////////////////////////////////////////////////////////////////

QBinTagReader::QBinTagReader(QBinTagFileReader &ifile, QBinTagReader *iparent)
: file(ifile)
{
	parent=iparent;
	char stg[2];
	file.file.readblock(2,stg);
	if ((stg[0]!='T')||(stg[1]!='G')) throw QError(_text("QBinTagReader: invalid tag header"));

	startpos=file.file.G_filepos();

	file.file.readblock(sizeof(TAGSIZE),&tagsize);

	file.read_shortstring(name);

}

QBinTagReader::~QBinTagReader()
{
	if (child.G_ref()!=NULL) delete child.G_ref();
	file.file.Set_filepos(startpos+tagsize);
}

StrPtr QBinTagReader::G_name()
{
	return name;
}

bool QBinTagReader::Istag(StrPtr iname)
{
	return issame(iname,name);
}


void QBinTagReader::close()
{
	delete this;
}

bool QBinTagReader::hassubtags()
{
	if (child.G_ref()!=NULL) throw QError(_text("QBinTagReader: unable to get information about next tag because a tag is currently open"));
	FILEPOS curpos=file.file.G_filepos();
	if (curpos<startpos+tagsize) return true;
	else return false;
}

QBinTagReaderRef QBinTagReader::getnextsubtag()
{
	if (child.G_ref()!=NULL) throw QError(_text("QBinTagReader: unable to get next tag because a tag is currently open"));
	QBinTagReader *subtag=new QBinTagReader(file,this);
	child.setreffor(subtag);
	return child;
}

void QBinTagReader::read_block(void *ptr, int len)
{
	file.read_block(ptr,len);
}

void QBinTagReader::read_shortstring(QString &istr)
{
	file.read_shortstring(istr);
}
void QBinTagReader::read_string(QString &istr)
{
	file.read_string(istr);
}
__int32 QBinTagReader::read_int32()
{
	return(file.read_int32());
}
double QBinTagReader::read_double()
{
	return(file.read_double());
}
bool QBinTagReader::read_bool()
{
	return(file.read_bool());
}



///////////////////////////////////////////////////////////////////////
// QBinTagFileReader
///////////////////////////////////////////////////////////////////////

QBinTagFileReader::QBinTagFileReader()
{
	reader=NULL;
}

QBinTagFileReader::~QBinTagFileReader()
{
	if (reader!=NULL) { delete reader; reader=NULL; }
	file.close();
}

QBinTagReaderRef QBinTagFileReader::open(StrPtr ifilename)
{
	if (reader!=NULL) throw QError(_text("Binary tag file reader is already open"));
	file.openread(ifilename);
	reader=new QBinTagReader(*this,NULL);
	QBinTagReaderRef ref;
	ref.setreffor(reader);
	return ref;
}

void QBinTagFileReader::read_block(void *ptr, int len)
{
	file.readblock(len,ptr);
}

void QBinTagFileReader::read_shortstring(QString &istr)
{
	StrChar css[260];
	unsigned __int8 slen;
	file.readblock(1,&slen);
	file.readblock(slen*sizeof(StrChar),css);css[slen]=0;
	istr=css;
}


void QBinTagFileReader::read_string(QString &istr)
{
	unsigned __int32 len;
	file.readblock(4,&len);
	if (len>500000) throw QError(_text("Unable to read string: too large"));
	StrChar *buff=(StrChar*)malloc(sizeof(StrChar)*(len+1));
	file.readblock(len*sizeof(StrChar),buff);buff[len]=0;
	istr=buff;
	free(buff);
}


__int32 QBinTagFileReader::read_int32()
{
	__int32 vl;
	file.readblock(4,&vl);
	return vl;
}

double QBinTagFileReader::read_double()
{
	double vl;
	file.readblock(sizeof(double),&vl);
	return vl;
}

bool QBinTagFileReader::read_bool()
{
	bool vl;
	file.readblock(sizeof(bool),&vl);
	return vl;
}


