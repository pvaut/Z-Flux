#include "stdafx.h"

#include "qfile.h"
#include "qerror.h"
#include "shlobj.h"
#include "qstringformat.h"


void GetDocumentsDir(QString &name)
{
	HRESULT hr;
	StrChar path[2000];	path[0]=0;
	hr=SHGetFolderPath(NULL,CSIDL_PERSONAL|CSIDL_FLAG_CREATE,NULL,SHGFP_TYPE_CURRENT,path);
	if (hr!=NOERROR) throw QError(_text("Unable to obtain documents folder"));
	name=path;
}

bool IsFilePresent(StrPtr name)
{
	FILE *fp;
	if (_wfopen_s(&fp,name,_qstr("r"))!=0) return false;
	else
	{
		fclose(fp);
		return true;
	}
}



QTextfile::QTextfile()
{
	fp=NULL;
	buff=(StrChar*)malloc(sizeof(StrChar)*(bufflen+1));
}

QTextfile::~QTextfile()
{
	close();
	free(buff);
}

void QTextfile::openread(StrPtr ifilename)
{
	QString str;
	close();
	filename=ifilename;

	if (_wfopen_s(&fp,filename,_qstr("r"))!=0)
		throw QError(TFormatString(_text("Unable to open text file ^1 for reading"),filename));
}

void QTextfile::openwrite(StrPtr ifilename)
{
	QString str;
	close();
	filename=ifilename;

	if (_wfopen_s(&fp,filename,_qstr("w"))!=0)
		throw QError(TFormatString(_text("Unable to open text file ^1 for writing"),filename));
}


bool QTextfile::isend()
{
	if (fp==NULL) return true;
	return feof(fp)!=0;
}


void QTextfile::close()
{
	if (fp!=NULL) fclose(fp);
	fp=NULL;
}

void QTextfile::readline(QString &line)
{
	if (fp==NULL) throw(_text("Could not read from file: it is not opened"));
	buff[0]=0;
	fgetws(buff,bufflen,fp);
	for (int i=qstrlen(buff)-3; i<qstrlen(buff); i++)
		if ((buff[i]==10)||(buff[i]==13)) buff[i]=0;
	line=buff;

}

void QTextfile::write(StrPtr line)
{
	if (fp==NULL) throw(_text("Could not read from file: it is not opened"));
	fputws(line,fp);
}

void QTextfile::writeline()
{
	fputws(_qstr("\n"),fp);
}



//********************** TSXfilefind ********************

QFileFind::QFileFind()
{
	hnd=NULL;
	ispresent=false;
}

QFileFind::~QFileFind()
{
	stop();
}

bool QFileFind::start(StrPtr path)
{
	stop();

	WIN32_FIND_DATA data;
	hnd=FindFirstFile(path,&data);
	name=data.cFileName;
	isdir=false;
	ishidden=false;
	if (data.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY) isdir=true;
	if ((data.dwFileAttributes==FILE_ATTRIBUTE_HIDDEN)||(data.dwFileAttributes==FILE_ATTRIBUTE_SYSTEM)||(data.dwFileAttributes==FILE_ATTRIBUTE_VIRTUAL))
		ishidden=true;
	if (hnd!=INVALID_HANDLE_VALUE)
	{
		ispresent=true;
		return true;
	}
	return false;
}

bool QFileFind::getnext(QString &i_name, bool &i_isdir, bool &i_ishidden)
{
	if (!ispresent) return false;
	if (hnd==NULL) return false;
	if (hnd==INVALID_HANDLE_VALUE) return false;

	i_name=name;
	i_isdir=isdir;
	i_ishidden=ishidden;

	WIN32_FIND_DATA data;
	if (!FindNextFile(hnd,&data)) ispresent=false;
	else
	{
		name=data.cFileName;
		isdir=false;
		ishidden=false;
		if ((data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)>0) isdir=true;
		if ((data.dwFileAttributes==FILE_ATTRIBUTE_HIDDEN)||(data.dwFileAttributes==FILE_ATTRIBUTE_SYSTEM)||(data.dwFileAttributes==FILE_ATTRIBUTE_VIRTUAL))
			ishidden=true;
	}

	return true;
}

void QFileFind::stop()
{
	if (hnd!=NULL) FindClose(hnd);
	hnd=NULL;
	ispresent=false;
}



////////////////////////////////////////////////////////////////////////
// QBinfile
////////////////////////////////////////////////////////////////////////

QBinfile::QBinfile()
{
	fp=NULL;
}

QBinfile::~QBinfile()
{
	close();
}

void QBinfile::openread(StrPtr ifilename)
{
	QString str;
	close();
	filename=ifilename;

	if (_wfopen_s(&fp,filename,_qstr("rb"))!=0)
	{
		QString str;
		FormatString(str,_text("Unable to open binary file ^1 for reading"),filename);
		throw QError(str);
	}
}

void QBinfile::openwrite(StrPtr ifilename)
{
	QString str;
	close();
	filename=ifilename;

	if (_wfopen_s(&fp,filename,_qstr("wb"))!=0)
	{
		QString str;
		FormatString(str,_text("Unable to binary file ^1 for writing"),filename);
		throw QError(str);
	}
}



void QBinfile::close()
{
	if (fp!=NULL) fclose(fp);
	fp=NULL;
}

void QBinfile::readblock(int size, void *ptr)
{
	if (fp==NULL) throw QError(_text("File not opened"));
	fread(ptr,size,1,fp);
}

void QBinfile::writeblock(int size, const void *ptr)
{
	if (fp==NULL) throw QError(_text("File not opened"));
	fwrite(ptr,size,1,fp);
}


long QBinfile::G_filepos()
{
	if (fp==NULL) throw QError(_text("File not opened"));
	return ftell(fp);
}

void QBinfile::Set_filepos(long ipos)
{
	if (fp==NULL) throw QError(_text("File not opened"));
	fseek(fp,ipos,SEEK_SET);
}



void QParamStore(QParamLocation location, StrPtr name, StrPtr content)
{
	if (location==PARAMLOCATION_REGISTRY)
	{
		::AfxGetApp()->WriteProfileString(_qstr("Settings"),name,content);
		return;
	}

	ASSERT(false);
}

bool QParamRead(QParamLocation location, StrPtr name, QString &content)
{
	if (location==PARAMLOCATION_REGISTRY)
	{
		QString st;
		StrPtr noval=_qstr("__%NoValue%__");
		st=(StrPtr)::AfxGetApp()->GetProfileString(_qstr("Settings"),name,noval);
		if (!issame(st,noval))
		{
			content=st;
			return true;
		}
		return false;
	}

	content.clear();
	ASSERT(false);
	return false;
}


void QParamStore(QParamLocation location, StrPtr name, bool content)
{
	QString stcontent=_qstr("No");if (content) stcontent=_qstr("Yes");
	QParamStore(location,name,stcontent);
}

bool QParamRead(QParamLocation location, StrPtr name, bool &content)
{
	QString stcontent;
	if (!QParamRead(location,name,stcontent)) return false;
	content=issame(stcontent,_qstr("Yes"));
	return true;
}


void QParamStore(QParamLocation location, StrPtr name, int content)
{
	QString stcontent;
	FormatString(stcontent,_qstr("^1"),content);
	QParamStore(location,name,stcontent);
}

bool QParamRead(QParamLocation location, StrPtr name, int &content)
{
	QString stcontent;
	if (!QParamRead(location,name,stcontent)) return false;
	content=qstr2int(stcontent);
	return true;
}
