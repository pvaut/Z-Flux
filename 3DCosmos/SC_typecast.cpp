#include "stdafx.h"

#include "qerror.h"

#include "SC_typecast.h"
#include "SC_env.h"


///////////////////////////////////////////////////////////////////////////////////

TSC_typecast::TSC_typecast(StrPtr iargtpe, StrPtr irettpe)
{
	argtype=GetTSCenv().G_datatype(iargtpe);
	returntype=GetTSCenv().G_datatype(irettpe);
	ASSERT(argtype!=NULL);
	ASSERT(returntype!=NULL);

	QString path;
	path=returntype->G_classpath();
	if (path.G_length()>0) path+=_qstr(";");
	path+=irettpe;
	classpath=path;

}




void TSC_typecast::execute(TSC_value *argval, TSC_value *retval)
{
	retval->createtype(returntype);
	execute_implement(argval,retval);
}

void TSC_typecast::G_fullID(QString &fullID)
{
	fullID+=_qstr("C:");
	fullID+=returntype->G_name();
	fullID+=_qstr("=");
	fullID+=argtype->G_name();
}

///////////////////////////////////////////////////////////////////////////////////

TSC_typecastadder::TSC_typecastadder(TSC_typecast *itypecast)
{
	GetTSCenv().addtypecast(itypecast);
}

