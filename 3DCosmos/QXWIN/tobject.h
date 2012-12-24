#pragma once

#include "tools.h"


class TObjectRef;

class TObject
{
private:
	Tcheapcopyarray<TObjectRef> refs;
public:
	void addref(TObjectRef *iref);
	void delref(TObjectRef *iref);
	TObject();
	virtual ~TObject();
	int G_objID()
	{
		return 0;
	}
private:
	void operator=(TObject &obj)
	{
	}
};


class TObjectRef
{
	friend class TObject;
protected:
	TObject *refto;
	void objectdeleted();
public:
	TObjectRef();
	TObjectRef(const TObjectRef &iobj);
	void setreffor(TObject *iref);
	virtual ~TObjectRef();
	virtual void notify_objectdeleted();
	void operator=(const TObjectRef& iobj);
	void copyfrom(const TObjectRef &iobj);
};

template<class A>
class TSpecificObjectRef : public TObjectRef
{
public:
	void setreffor(A *iref)
	{
		TObjectRef::setreffor(iref);
	}
	A* G_ref()
	{
		return (A*)refto;
	}
	A& G_obj()
	{
		if (refto==NULL) throw QError(_text("Empty object reference"));
		return *((A*)refto);
	}
};
