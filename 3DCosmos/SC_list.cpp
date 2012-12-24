#include "stdafx.h"
#include "SC_value.h"
#include "SC_list.h"
#include "SC_env.h"

#include "sort.h"


void TSC_list::tostring(QString &str, int tpe)
{
	QString str1;
	str=_qstr("{");
	for (int i=0; i<items.G_count(); i++)
	{
		items[i]->tostring(str1);
		if (str.G_length()+str1.G_length()<3000)
		{
			if (i>0) str+=_qstr(", ");
			str+=str1;
		}
	}
	str+=_qstr("}");
}

void TSC_list::operator=(const TSC_list &m)
{
	items.reset();
	for (int i=0; i<m.items.G_count(); i++)
	{
		TSC_value *val=new TSC_value;
		items.add(val);
		val->copyfrom(m.items[i]);
	}
}

void TSC_list::add(const TSC_value *ival)
{
	TSC_value *val=new TSC_value;
	val->copyfrom(ival);
	items.add(val);
}

void TSC_list::mergefrom(TSC_list *l1, TSC_list *l2)
{
	items.reset();
	for (int i=0; i<l1->G_size(); i++)
	{
		TSC_value *val=new TSC_value;
		val->copyfrom(l1->get(i));
		items.add(val);
	}
	for (int i=0; i<l2->G_size(); i++)
	{
		TSC_value *val=new TSC_value;
		val->copyfrom(l2->get(i));
		items.add(val);
	}
}


void TSC_list::del(int nr)
{
	items.del(nr);
}


TSC_value* TSC_list::get(int nr)
{
	if ((nr<0)||(nr>=items.G_count())) throw QError(_text("Invalid list index"));
	return items.get(nr);
}

void TSC_list::grow(int cnt)
{
	while (items.G_count()<cnt)
	{
		TSC_value *val=new TSC_value;
//		val->createtype(GETDATATYPE_SCALAR);
		items.add(val);
	}
}

void TSC_list::invert()
{
	for (int i1=0; i1<items.G_count()/2; i1++)
	{
		int i2=items.G_count()-1-i1;
		TSC_value *tmp=items[i1];
		items.set(i1,items[i2]);
		items.set(i2,tmp);
	}
}


void TSC_list::streamout(QBinTagWriter &writer)
{
	writer.write_int32(items.G_count());
	for (int i=0; i<items.G_count(); i++)
		items[i]->streamout_withtype(writer);
}
void TSC_list::streamin(QBinTagReader &reader)
{
	items.reset();
	int ct=reader.read_int32();
	for (int i=0; i<ct; i++)
	{
		TSC_value *val=new TSC_value;
		items.add(val);
		val->streamin_withtype(reader);
	}
}

void TSC_list::sort(Tintarray &idx)
{
	idx.reset();
	for (int i=0; i<items.G_count(); i++) idx.add(i);

	sort_index(items.G_count(),items,idx.G_ptr());
	int i=0;
}



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_list,list)
{
	setclasspath_fromtype(SC_valname_list);
	setreturntype(SC_valname_list);
	addvar(_qstr("comp"),SC_valname_any);
	setflexiblearglist();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_list *list=G_valuecontent<TSC_list>(retval);
	list->reset();
	for (int i=0; i<arglist->G_count(); i++)
		list->add(arglist->get(i));
}
ENDFUNCTION(func_list)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_listclear,clear)
{
	setmemberfunction(SC_valname_list);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<TSC_list>(owner)->reset();
}
ENDFUNCTION(func_listclear)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_listgetsize,size)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_list);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_list *list=G_valuecontent<TSC_list>(owner);
	retval->G_content_scalar()->copyfrom(list->G_size());
}
ENDFUNCTION(func_listgetsize)



///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_listadd,add)
{
	setmemberfunction(SC_valname_list);
	addvar(_qstr("comp"),SC_valname_any);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<TSC_list>(owner)->add(arglist->get(0));
}
ENDFUNCTION(func_listadd)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_listget,get)
{
	setcanassign();
	setreturntype(SC_valname_any);
	setmemberfunction(SC_valname_list);
	addvar(_qstr("index"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_list *list=G_valuecontent<TSC_list>(owner);
	int idx=arglist->get(0)->G_content_scalar()->G_intval();
	list->grow(idx+1);
	TSC_value *val=list->get(idx);
	if (assigntoval!=NULL) val->copyfrom_flexibletype(assigntoval);
	retval->createtype(val->G_datatype());
	retval->encapsulate(val);
}
ENDFUNCTION(func_listget)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_listset,set)
{
	setreturntype(SC_valname_any);
	setmemberfunction(SC_valname_list);
	addvar(_qstr("index"),SC_valname_scalar);
	addvar(_qstr("comp"),SC_valname_any);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_list *list=G_valuecontent<TSC_list>(owner);
	int idx=arglist->get(0)->G_content_scalar()->G_intval();
	list->grow(idx+1);
	list->get(idx)->copyfrom_flexibletype(arglist->get(1));
}
ENDFUNCTION(func_listset)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_listdel,del)
{
	setreturntype(SC_valname_any);
	setmemberfunction(SC_valname_list);
	addvar(_qstr("index"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_list *list=G_valuecontent<TSC_list>(owner);
	int idx=arglist->get(0)->G_content_scalar()->G_intval();
	list->del(idx);
}
ENDFUNCTION(func_listdel)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_list_invert,Invert)
{
	setmemberfunction(SC_valname_list);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	G_valuecontent<TSC_list>(owner)->invert();
}
ENDFUNCTION(func_list_invert)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_listsort,Sort)
{
	setmemberfunction(SC_valname_list);
	setreturntype(SC_valname_list);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_list *list=G_valuecontent<TSC_list>(owner);
	Tintarray idx;
	list->sort(idx);

	TSC_list *retlist=G_valuecontent<TSC_list>(retval);
	retlist->reset();
	TSC_value vl;
	vl.createtype(GetTSCenv().G_datatype(SC_valname_scalar));
	for (int i=0; i<idx.G_count(); i++)
	{
		vl.G_content_scalar()->copyfrom(idx[i]);
		retlist->add(&vl);
	}

}
ENDFUNCTION(func_listsort)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_list_add,8,SC_valname_list,SC_valname_list,SC_valname_list)
{
	setclasspath_fromtype(SC_valname_list);
	Set_description(_text("Joins two lists"));
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	TSC_list *l1=G_valuecontent<TSC_list>(val1);
	TSC_list *l2=G_valuecontent<TSC_list>(val2);
	G_valuecontent<TSC_list>(retval)->mergefrom(l1,l2);
}
ENDOPERATOR(op_list_add)
