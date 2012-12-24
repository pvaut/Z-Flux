#include"stdafx.h"
#include"Qerror.h"


QError::QError(StrPtr str)
{
	content=str;
	addlog(_text("*************** ERROR ****************"));
	addlog(str);
}


void reporterror(CWnd *win, QError &err)
{
	win->MessageBox(err.G_content(),_text("Error"),MB_ICONEXCLAMATION|MB_OK);
}

void reporterror(CWnd *win, StrPtr err)
{
	win->MessageBox(err,_text("Error"),MB_ICONEXCLAMATION|MB_OK);
}

void reporterror(QError &err)
{
	::MessageBox(NULL,err.G_content(),_text("Error"),MB_ICONEXCLAMATION|MB_OK);
}

void reporterror(StrPtr err)
{
	::MessageBox(NULL,err,_text("Error"),MB_ICONEXCLAMATION|MB_OK);
}

void throw_GetLastError(StrPtr comment)
{
	QString error;
	unsigned long lasterror=GetLastError();
	WCHAR *mess=NULL;
	int rs;
	rs=FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ARGUMENT_ARRAY,
					NULL,lasterror,LANG_NEUTRAL,(LPTSTR)&mess,0,NULL );
	if (mess!=NULL)
	{
		error=comment;error+=_qstr("\n");
		error+=mess;
		LocalFree(mess);
		throw QError(error);
	}
	else
	{
		error=comment;error+=_qstr("\n");
		error+=_text("Unspecified error");
		throw QError(error);
	}
}
