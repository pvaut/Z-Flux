#include "stdafx.h"

#include "qstringformat.h"

#include "SC_value.h"
#include "SC_env.h"


/////////////////////////////////////////////////////////////////////////////////
// TSC_scalar
/////////////////////////////////////////////////////////////////////////////////


void TSC_scalar::streamout(QBinTagWriter &writer)
{
	writer.write_double(value);
}

void TSC_scalar::streamin(QBinTagReader &reader)
{
	copyfrom(reader.read_double());
}

/////////////////////////////////////////////////////////////////////////////////
// TSC_string
/////////////////////////////////////////////////////////////////////////////////


void TSC_string::streamout(QBinTagWriter &writer)
{
	writer.write_string(content);
}

void TSC_string::streamin(QBinTagReader &reader)
{
	reader.read_string(content);
}


/////////////////////////////////////////////////////////////////////////////////
// TSC_datatype
/////////////////////////////////////////////////////////////////////////////////


void TSC_datatype::G_fullID(QString &fullID)
{
	fullID=_qstr("T:");
	fullID+=G_name();
}


void TSC_datatype::streamout(QBinTagWriter &writer, TObject *content)
{
	throw QError(TFormatString(_text("Unable to stream objects of type '^1'"),G_name()));
}
void TSC_datatype::streamin(QBinTagReader &reader, TObject *content)
{
	throw QError(TFormatString(_text("Unable to stream objects of type '^1'"),G_name()));
}




bool TSC_value::operator==(TSC_value &ivalue)
{
	if ((datatype==NULL)||(ivalue.datatype==NULL)) return false;
	if (!datatype->issame(ivalue.datatype)) return false;
	return datatype->compare(content,ivalue.content);
}

bool TSC_value::operator<(const TSC_value &ivalue) const
{
	if ((datatype==NULL)||(ivalue.datatype==NULL)) throw QError(_text("Unable to compare: empty value"));
	if (!datatype->issame(ivalue.datatype)) throw QError(_text("Unable to compare: incompatible data types"));
	return datatype->comparesize(content,ivalue.content);
}



void TSC_value::checkcontent(TSC_datatype *idatatype)
{
	if (content==NULL) throw QError(_qstr("Empty value"));
	if (datatype==NULL) throw QError(_qstr("Undefined value"));
	if (idatatype==NULL) throw QError(_qstr("Undefined requested datatype"));
	if (!datatype->issame(idatatype))
	{
		QString err;
		FormatString(err,_qstr("Invalid value type: expected '^2' and found '^1'"),datatype->G_name(),idatatype->G_name());
		throw QError(err);
	}
}

void TSC_value::copyfrom(bool vl)
{
	if (datatype==NULL) return;
	if (qstricmp(datatype->G_name(),SC_valname_boolean)!=0) return;
	if (content==NULL) content=datatype->value_create();
	isowner=true;
	((TSC_boolean*)content)->copyfrom(vl);
}




TSC_string* TSC_value::G_content_string()
{
	checkcontent(GETDATATYPE_STRING);
	return (TSC_string*)content;
}

TSC_scalar* TSC_value::G_content_scalar()
{
	checkcontent(GETDATATYPE_SCALAR);
	return (TSC_scalar*)content;
}

void TSC_value::fromint(int vl)
{
	if (content==NULL) throw QError(_qstr("Empty value"));
	if (datatype==NULL) throw QError(_qstr("Undefined value"));
	datatype->fromint(vl,content);
}

int TSC_value::toint()
{
	if (content==NULL) throw QError(_qstr("Empty value"));
	if (datatype==NULL) throw QError(_qstr("Undefined value"));
	return datatype->toint(content);
}

void TSC_value::streamout(QBinTagWriter &writer)
{
	if ((datatype==NULL)||(content==NULL)) throw QError(_text("Undefined value"));
	datatype->streamout(writer,content);
}

void TSC_value::streamin(QBinTagReader &reader)
{
	if ((datatype==NULL)||(content==NULL)) return;
	datatype->streamin(reader,content);
}

void TSC_value::streamout_withtype(QBinTagWriter &writer)
{
	if ((datatype==NULL)||(content==NULL)) throw QError(_text("Undefined value"));
	writer.write_shortstring(datatype->G_name());
	streamout(writer);
}

void TSC_value::streamin_withtype(QBinTagReader &reader)
{
	QString datatypename;
	reader.read_shortstring(datatypename);
	TSC_datatype *dtpe=GetTSCenv().G_datatype(datatypename);
	if (dtpe==NULL) throw QError(TFormatString(_text("Unrecognised data type ^1"),datatypename));
	createtype(dtpe);
	streamin(reader);
}


void GetObjectName(TSC_value *val, QString &name)
{
	QString error;
	name.clear();
	if ((val==NULL)||(val->G_datatype()==NULL)) return;
	TSC_value retval;
	TSC_funcarglist arglist(NULL);
	if (val->G_datatype()==NULL)
		throw QError(_text("Unable to get object name: no data type specified"));
	if (!val->G_datatype()->evalmemberfunction(val->G_content(),_qstr("name"),&retval,NULL,arglist,error))
	{
		QString err;FormatString(err,_text("Unable to get object name for type ^1"),val->G_datatype()->G_name());
		throw QError(err);
	}
	name=retval.G_content_string()->G_string();
}
