#include "stdafx.h"
#include "qstring.h"
#include "objecttree.h"
#include "qxformattedstring.h"


Tparamset::Tparamset()
{
	addparam(ObjNameStr,SC_valname_string);
	addparam(_qstr("Custom"),SC_valname_map,false,_text("This map can be used to attach custom data to an object"));
}
Tparam* Tparamset::addparam(StrPtr iname, StrPtr tpe, bool readonly, StrPtr idummy, StrPtr isubclass)
{
	Tparam *param=new Tparam;
	param->content.settype(GetTSCenv().G_datatype(tpe));
	param->content.create();
	param->name=iname;
	param->readonly=readonly;
	param->subclass=isubclass;
	params.add(param);
	isdirty=true;
	return param;
}

void Tparamset::G_name(QString &str)
{
	Tparam *prm=G_param(ObjNameStr);
	if (prm!=NULL) prm->content.tostring(str);
}

void Tparamset::Set_name(StrPtr iname)
{
	Tparam *prm=G_param(ObjNameStr);
	if (prm!=NULL) prm->content.fromstring(iname);
}


void Tparamset::G_displayname(TQXFormattedString &fstr)
{
	fstr.clear();
	QString name;
	G_name(name);
	if (qstrlen(name)>0)
	{
		fstr.makebold(true);
		fstr.append(name);
		fstr.makebold(false);
		if (qstrlen(G_classname())>0)
		{
			fstr.append(_qstr(" ("));
			fstr.append(G_classname());
			fstr.append(_qstr(")"));
		}
		return;
	}
	fstr.append(G_classname());
}

void Tparamset::G_description(QString &str)
{
	str.clear();
}


int Tparamset::G_paramcount()
{
	return params.G_count();
}

StrPtr Tparamset::G_paramname(int nr)
{
	if ((nr<0)||(nr>=params.G_count())) return _qstr("");
	return params[nr]->name;
}

StrPtr Tparamset::G_paramsubclass(int nr)
{
	if ((nr<0)||(nr>=params.G_count())) return _qstr("");
	return params[nr]->subclass;
}


TSC_value* Tparamset::G_paramcontent(int nr)
{
	if ((nr<0)||(nr>=params.G_count())) return NULL;
	return &params[nr]->content;
}



void Tparamset::Set_paramcontent(int nr, TSC_value *newvalue)
{
	if ((nr<0)||(nr>=params.G_count())) return;
	if (params[nr]->content==*newvalue) return;

	if ((newvalue->G_datatype()!=NULL)&&(newvalue->G_datatype()!=NULL))
		if (!params[nr]->content.istype(newvalue->G_datatype()->G_name()))
		{
			QString err;
			FormatString(err,_text("Invalid parameter assignment data type: found ^1 and expected ^2"),
				newvalue->G_datatype()->G_name(),
				params[nr]->content.G_datatype()->G_name());
			throw QError(err);
		}
	params[nr]->content.copyfrom(newvalue);
	isdirty=true;
	paramchanged(params[nr]->name);
}

bool Tparamset::G_paramreadonly(int nr)
{
	if ((nr<0)||(nr>=params.G_count())) return true;
	return params[nr]->readonly;
}

Tparam* Tparamset::G_param(StrPtr iname)
{
	for (int i=0; i<params.G_count(); i++)
		if (qstricmp(params[i]->name,iname)==0) return params[i];
	return NULL;
}

Tparam& Tparamset::G_param_required(StrPtr iname)
{
	Tparam *pr=G_param(iname);
	if (pr==NULL) throw QError(TFormatString(_text("Unable to find parameter ^1"),iname));
	return *pr;
}


StrPtr Tparamset::G_paramstring(StrPtr iname)
{
	return G_param(iname)->content.G_content_string()->G_string();
}

int Tparamset::G_paramint(StrPtr iname)
{
	return G_param(iname)->content.G_content_scalar()->G_intval();
}

double Tparamset::G_paramdouble(StrPtr iname)
{
	return G_param(iname)->content.G_content_scalar()->G_val();
}




void Tparamset::param_setvalue(StrPtr iname, StrPtr ival)
{
	Tparam *param=G_param(iname);
	if (param==NULL) return;
	param->content.fromstring(ival);
}
void Tparamset::param_setvalue(StrPtr iname, double ival)
{
	Tparam *param=G_param(iname);
	if (param==NULL) return;
	param->content.G_content_scalar()->copyfrom(ival);
}

void Tparamset::param_readonly(StrPtr iname, bool isreadonly)
{
	Tparam *param=G_param(iname);
	if (param==NULL) return;
	param->readonly=isreadonly;
}




TSC_datatype* TObjectTreeItem::G_datatype()
{
	TSC_datatype *tp=GetTSCenv().G_datatype(G_classname());
	if (tp==NULL) tp=GetTSCenv().G_datatype(SC_valname_object);
	ASSERT(tp!=NULL);
	return tp;
}




TObjectTreeItem::~TObjectTreeItem()
{
}

void TObjectTreeItem::tostring(QString &str, int tpe)
{
	G_name(str);
	str+=_qstr(" (");str+=G_classname();str+=_qstr(")");
}

TObjectTreeItem* TObjectTreeItem::G_childbyname(StrPtr iname)
{
	QString str;
	for (int i=0; i<G_childcount(); i++)
	{
		TObjectTreeItem *it=G_child(i);
		it->G_name(str);
		if (qstricmp(iname,str)==0)
			return it;
	}
	return NULL;
}


void TObjectTreeItem::dispose()
{
	TObjectTreeItem *parent=G_parent();
	if (parent==NULL) return;
	parent->delchild(this);
}


bool TObjectTreeItem::memberfunction_accept(StrPtr funcname)
{
	QString str;
	for (int i=0; i<G_childcount(); i++)
	{
		G_child(i)->G_name(str);
		if (qstricmp(str,funcname)==0) return true;
	}

	for (int i=0; i<G_paramcount(); i++)
		if (qstricmp(G_paramname(i),funcname)==0) return true;

	return false;
}

bool TObjectTreeItem::memberfunction_eval(StrPtr funcname, TSC_value *returnval, TSC_value *assigntoval, TSC_funcarglist &arglist, QString &error)
{
	QString str;
	for (int i=0; i<G_childcount(); i++)
	{
		G_child(i)->G_name(str);
		if (qstricmp(str,funcname)==0)
		{
			if (assigntoval!=NULL)
			{ 
				error=_text("Invalid assignment");
				return false;
			}
			if (arglist.G_count()>0) { error=_text("An object does not have an argument list");return false; }
			returnval->settype(G_child(i)->G_datatype());
			returnval->encapsulate(G_child(i));
			return true;
		}
	}

	for (int i=0; i<G_paramcount(); i++)
	{
		if (qstricmp(G_paramname(i),funcname)==0)
		{
			if (arglist.G_count()>0) { error=_text("An object property does not have an argument list");return false; }
			if (assigntoval!=NULL) Set_paramcontent(i,assigntoval);
			returnval->encapsulate(G_paramcontent(i));
			return true;
		}
	}

	return false;
}


void TObjectTreeItem::streamout(QBinTagWriter &writer)
{
	{
		QBinTagWriter &paramstag=writer.subtag(_qstr("Parameters")).G_obj();
		for (int paramnr=0; paramnr<params.G_count(); paramnr++)
		{
			QBinTagWriter &paramtag=paramstag.subtag(_qstr("Param")).G_obj();
			paramtag.write_shortstring(params[paramnr]->name);
			params[paramnr]->content.streamout(paramtag);
			paramtag.close();
		}
		paramstag.close();
	}
	{
		QBinTagWriter &contenttag=writer.subtag(_qstr("Content")).G_obj();
		streamout_content(contenttag);
		contenttag.close();
	}

}
void TObjectTreeItem::streamin(QBinTagReader &reader)
{
	while (reader.hassubtags())
	{
		QBinTagReader &tg1=reader.getnextsubtag().G_obj();

		if (tg1.Istag(_qstr("Parameters")))
		{
			while (tg1.hassubtags())
			{
				QBinTagReader &tg2=tg1.getnextsubtag().G_obj();
				if (tg2.Istag(_qstr("Param")))
				{
					QString paramid;tg2.read_shortstring(paramid);
					G_param_required(paramid).content.streamin(tg2);
				}
				tg2.close();
			}
		}

		if (tg1.Istag(_qstr("Content")))
		{
			streamin_content(tg1);
		}

		tg1.close();
	}
}

void TObjectTreeItem::streamout_content(QBinTagWriter &writer)
{
	throw QError(_text("This object type cannot be streamed"));
}

void TObjectTreeItem::streamin_content(QBinTagReader &reader)
{
	throw QError(_text("This object type cannot be streamed"));
}

