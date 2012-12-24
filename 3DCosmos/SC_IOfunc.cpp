#include "stdafx.h"

#include "qerror.h"

#include "valnames.h"

#include "utils.h"
#include "qfile.h"

#include "SC_func.h"
#include "SC_script.h"
#include "SC_env.h"

//#include "3Dscene.h"


#define filefunctions _qstr("Input / Output;Files")


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_readtextfile,readtextfile)
{
	setclasspath(filefunctions);
	setreturntype(SC_valname_string);
	addvar(_qstr("filename"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	QString content,line;
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();
	QTextfile fl;
	fl.openread(filename);
	while (!fl.isend())
	{
		fl.readline(line);
		content+=line;
		content+=_qstr("\n");
	}
	fl.close();
	retval->G_content_string()->fromstring(content);
}
ENDFUNCTION(func_readtextfile)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_writetextfile,writetextfile)
{
	setclasspath(filefunctions);
	setreturntype(SC_valname_string);
	addvar(_qstr("filename"),SC_valname_string);
	addvar(_qstr("content"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();
	StrPtr content=arglist->get(1)->G_content_string()->G_string();
	QTextfile fl;
	fl.openwrite(filename);
	fl.write(content);
	fl.close();
}
ENDFUNCTION(func_writetextfile)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_isfilepresent,FileIsPresent)
{
	setclasspath(filefunctions);
	setreturntype(SC_valname_boolean);
	addvar(_qstr("filename"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();

//	FILETIME tme,loctme;
	HANDLE hfile=CreateFile(filename,0,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL) ;
	if (hfile==INVALID_HANDLE_VALUE) retval->G_content_boolean()->copyfrom(false);
	else
	{
		retval->G_content_boolean()->copyfrom(true);
		CloseHandle(hfile);
	}
/*	if (timespec==1) GetFileTime(hfile,&tme,NULL,NULL);
	if (timespec==2) GetFileTime(hfile,NULL,&tme,NULL);
	if (timespec==3) GetFileTime(hfile,NULL,NULL,&tme);
	FileTimeToLocalFileTime(&tme,&loctme);*/
}
ENDFUNCTION(func_isfilepresent)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_getfilecreatedtime,FileCreatedTime)
{
	setclasspath(filefunctions);
	setreturntype(SC_valname_time);
	addvar(_qstr("filename"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();

	FILETIME tme,loctme;
	HANDLE hfile=CreateFile(filename,0,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL) ;
	if (hfile==INVALID_HANDLE_VALUE) throw QError(_text("File is not present"));
	GetFileTime(hfile,&tme,NULL,NULL);
	CloseHandle(hfile);
	FileTimeToLocalFileTime(&tme,&loctme);
	SYSTEMTIME systime;
	FileTimeToSystemTime(&loctme,&systime);
	TSC_time *tm=G_valuecontent<TSC_time>(retval);
	tm->Set_year(systime.wYear);
	tm->Set_month(systime.wMonth);
	tm->Set_day(systime.wDay);
	tm->Set_hour(systime.wHour);
	tm->Set_min(systime.wMinute);
	tm->Set_sec(systime.wSecond);
}
ENDFUNCTION(func_getfilecreatedtime)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_getfilemodifiedtime,FileModifiedTime)
{
	setclasspath(filefunctions);
	setreturntype(SC_valname_time);
	addvar(_qstr("filename"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();

	FILETIME tme,loctme;
	HANDLE hfile=CreateFile(filename,0,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL) ;
	if (hfile==INVALID_HANDLE_VALUE) throw QError(_text("File is not present"));
	GetFileTime(hfile,NULL,NULL,&tme);
	CloseHandle(hfile);
	FileTimeToLocalFileTime(&tme,&loctme);
	SYSTEMTIME systime;
	FileTimeToSystemTime(&loctme,&systime);
	TSC_time *tm=G_valuecontent<TSC_time>(retval);
	tm->Set_year(systime.wYear);
	tm->Set_month(systime.wMonth);
	tm->Set_day(systime.wDay);
	tm->Set_hour(systime.wHour);
	tm->Set_min(systime.wMinute);
	tm->Set_sec(systime.wSecond);
}
ENDFUNCTION(func_getfilemodifiedtime)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_copyfile,CopyFile)
{
	setclasspath(filefunctions);
	addvar(_qstr("source"),SC_valname_string);
	addvar(_qstr("destination"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr sourcename=arglist->get(0)->G_content_string()->G_string();
	StrPtr destname=arglist->get(1)->G_content_string()->G_string();
	if (!CopyFile(sourcename,destname,false))
		throw QError(TFormatString(_text("Unable to copy file from ^1 to ^2"),sourcename,destname));
}
ENDFUNCTION(func_copyfile)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_delfile,DeleteFile)
{
	setclasspath(filefunctions);
	addvar(_qstr("filename"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr filename=arglist->get(0)->G_content_string()->G_string();
	if (!DeleteFile(filename))
		throw QError(TFormatString(_text("Unable to delete file ^1"),filename));
}
ENDFUNCTION(func_delfile)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_createdirectory,CreateDirectory)
{
	setclasspath(filefunctions);
	addvar(_qstr("name"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr dirname=arglist->get(0)->G_content_string()->G_string();
	CreateDirectory(dirname,NULL);
}
ENDFUNCTION(func_createdirectory)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_getfilelist,GetFileList)
{
	setclasspath(filefunctions);
	setreturntype(SC_valname_list);
	addvar(_qstr("wildcard"),SC_valname_string);
	addvar(_qstr("directories"),SC_valname_boolean,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	StrPtr wildcard=arglist->get(0)->G_content_string()->G_string();

	bool returndirs=false;
	if (arglist->G_ispresent(1)) returndirs=arglist->get(1)->G_content_boolean()->G_val();

	TSC_list *list=G_valuecontent<TSC_list>(retval);
	list->reset();

	if ((returndirs)&&(qstrlen(wildcard)==0))
	{//return list of drives instead
		StrChar drivename[100]=L"A:";

		DWORD drivesmask=GetLogicalDrives();
		while(drivesmask)
		{
			if( drivesmask & 1 )
			{
				TSC_value vl;
				vl.settype(GetTSCenv().G_datatype(SC_valname_string));
				vl.fromstring(drivename);
				list->add(&vl);
			}
			drivename[0]++;
			drivesmask >>= 1;
		}
		return;
	}

	QFileFind filefind;
	filefind.start(wildcard);
	QString filename;
	bool isdir,ishidden;
	while (filefind.getnext(filename,isdir,ishidden))
	{
		bool ok=false;
		if ((!returndirs)&&(!isdir)) ok=true;
		if ((returndirs)&&(isdir)) ok=true;
		if (ishidden) ok=false;
		if (ok)
		{
			TSC_value vl;
			vl.settype(GetTSCenv().G_datatype(SC_valname_string));
			vl.fromstring(filename);
			list->add(&vl);
		}
	}
}
ENDFUNCTION(func_getfilelist)
