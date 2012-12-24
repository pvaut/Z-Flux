#include "stdafx.h"
#include "SC_value.h"
#include "SC_map.h"
#include "SC_env.h"


TSC_mappair::TSC_mappair()
{
	item=new TSC_value;
}

TSC_mappair::~TSC_mappair()
{
	delete item;
}

void TSC_mappair::fromstring(StrPtr icontent)
{
	ASSERT(false);
}

void TSC_mappair::tostring(QString &str, int tpe)
{
	QString st2;
	str=name;
	str+=":";
	item->tostring(st2);
	str+=st2;
}

void TSC_mappair::import(StrPtr i_name, const TSC_value *i_item)
{
	name=i_name;
	item->copyfrom_flexibletype(i_item);
}


void TSC_mappair::operator=(const TSC_mappair &m)
{
	name=m.name;
	item->copyfrom_flexibletype(m.item);
}

void TSC_mappair::streamout(QBinTagWriter &writer)
{
	writer.write_string(name);
	item->streamout_withtype(writer);
}
void TSC_mappair::streamin(QBinTagReader &reader)
{
	reader.read_string(name);
	item->streamin_withtype(reader);
}



/////////////////////////////////////////////////////////
// TSC_map
/////////////////////////////////////////////////////////

void TSC_map::tostring(QString &str, int tpe)
{
	QString str1;
	str=_qstr("{");
	for (int i=0; i<items.G_count(); i++)
	{
		items[i]->tostring(str1,0);
		if (str.G_length()+str1.G_length()<3000)
		{
			if (i>0) str+=_qstr(", ");
			str+=str1;
		}
	}
	str+=_qstr("}");
}

void TSC_map::operator=(const TSC_map &m)
{
	reset();
	for (int i=0; i<m.items.G_count(); i++)
		Set(m.items[i]->G_name(),m.items[i]->G_item());
}


void TSC_map::reset()
{
	items.reset();
	index.reset();
}


void TSC_map::Set(StrPtr name, const TSC_value *ival)
{
	TSC_mappair *pair=NULL;
	int idx=index.get(name);
	if (idx<0)
	{
		pair=new TSC_mappair;
		pair->name=name;
		items.add(pair);
		index.add(name,items.G_count()-1);
	}
	else pair=items[idx];
	pair->item->copyfrom_flexibletype(ival);
}

void TSC_map::SetString(StrPtr iname, StrPtr icontent)
{
	TSC_value val;
	val.createtype(GetTSCenv().G_datatype(SC_valname_string));
	val.fromstring(icontent);
	Set(iname,&val);
}


void TSC_map::streamout(QBinTagWriter &writer)
{
	writer.write_int32(items.G_count());
	for (int i=0; i<items.G_count(); i++)
		items[i]->streamout(writer);
}
void TSC_map::streamin(QBinTagReader &reader)
{
	reset();
	int ct=reader.read_int32();
	for (int i=0; i<ct; i++)
	{
		TSC_mappair *pair=new TSC_mappair;
		pair->streamin(reader);
		items.add(pair);
		index.add(pair->G_name(),items.G_count()-1);
	}
}


/*
void TSC_map::del(StrPtr name)
{
	items.del(nr);
}
*/

TSC_value* TSC_map::get(StrPtr name)
{
	int idx=index.get(name);
	if (idx<0)
	{
		QString err;
		FormatString(err,_text("Invalid map index '^1'"),name);
		throw QError(err);
	}
	return items[idx]->item;
}

bool TSC_map::IsItemPresent(StrPtr iname) const
{
	return (index.get(iname)>=0);
}


StrPtr TSC_map::G_itemname(int nr) const
{
	if ((nr<0)||(nr>=items.G_count())) throw QError(_text("Invalid map index number"));
	return items[nr]->G_name();
}




///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_map,map)
{
	setclasspath_fromtype(SC_valname_map);
	setreturntype(SC_valname_map);
	addvar(_qstr("comp"),SC_valname_mappair);
	setflexiblearglist();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_map *map=G_valuecontent<TSC_map>(retval);
	map->reset();
	for (int i=0; i<arglist->G_count(); i++)
	{
		TSC_mappair *mappair=G_valuecontent<TSC_mappair>(arglist->get(i));
		map->Set(mappair->G_name(),mappair->G_item());
	}
}
ENDFUNCTION(func_map)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_mapadd,AddItem)
{
	setmemberfunction(SC_valname_map);
	addvar(_qstr("item"),SC_valname_mappair);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_mappair *mappair=G_valuecontent<TSC_mappair>(arglist->get(0));
	G_valuecontent<TSC_map>(owner)->Set(mappair->G_name(),mappair->G_item());
}
ENDFUNCTION(func_mapadd)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_mapclear,clear)
{
	setmemberfunction(SC_valname_map);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<TSC_map>(owner)->reset();
}
ENDFUNCTION(func_mapclear)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_map_get,Get)
{
	setmemberfunction(SC_valname_map);
	setreturntype(SC_valname_any);
	setcanassign();
	addvar(_qstr("name"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_map *map=G_valuecontent<TSC_map>(owner);
	StrPtr name=arglist->get(0)->G_content_string()->G_string();
	if (assigntoval!=NULL) map->Set(name,assigntoval);
	retval->encapsulate(map->get(name));
}
ENDFUNCTION(func_map_get)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_map_ispresent,IsPresent)
{
	setmemberfunction(SC_valname_map);
	setreturntype(SC_valname_boolean);
	addvar(_qstr("name"),SC_valname_string);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_map *map=G_valuecontent<TSC_map>(owner);
	StrPtr name=arglist->get(0)->G_content_string()->G_string();
	retval->G_content_boolean()->copyfrom(map->IsItemPresent(name));
}
ENDFUNCTION(func_map_ispresent)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_map_getnamelist,GetNames)
{
	setmemberfunction(SC_valname_map);
	setreturntype(SC_valname_list);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_map *map=G_valuecontent<TSC_map>(owner);
	TSC_list *list=G_valuecontent<TSC_list>(retval);

	list->reset();
	for (int i=0; i<map->G_itemcount(); i++)
	{
		TSC_value vl;
		vl.settype(GetTSCenv().G_datatype(SC_valname_string));
		vl.fromstring(map->G_itemname(i));
		list->add(&vl);
	}
}
ENDFUNCTION(func_map_getnamelist)
