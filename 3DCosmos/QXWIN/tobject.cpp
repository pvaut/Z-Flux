#include "stdafx.h"
#include "tobject.h"


void TObject::addref(TObjectRef *iref)
{
	refs.add(iref);
}

void TObject::delref(TObjectRef *iref)
{
	for (int i=0; i<refs.G_count(); i++)
		if (refs[i]==iref) refs.del(i);
}


TObject::TObject()
{
}


TObject::~TObject()
{
	while (refs.G_count()>0)
	{
		refs[0]->objectdeleted();
		refs.del(0);
	}
}


void TObjectRef::objectdeleted()
{
	refto=NULL;
	notify_objectdeleted();
}

TObjectRef::TObjectRef()
{
	refto=NULL;
}

TObjectRef::TObjectRef(const TObjectRef &iobj)
{
	refto=NULL;
	copyfrom(iobj);
}


void TObjectRef::setreffor(TObject *iref)
{
	if (refto!=NULL) refto->delref(this);
	refto=iref;
	if (refto!=NULL) refto->addref(this);
}

TObjectRef::~TObjectRef()
{
	if (refto!=NULL) refto->delref(this);
}

void TObjectRef::notify_objectdeleted()
{
}

void TObjectRef::operator=(const TObjectRef& iobj)
{
	copyfrom(iobj);
}

void TObjectRef::copyfrom(const TObjectRef &iobj)
{
	setreffor(iobj.refto);
}
