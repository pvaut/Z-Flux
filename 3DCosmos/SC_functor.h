#pragma once

#include "tobject.h"
#include "qstring.h"
#include "qbintagfile.h"
#include "tools.h"
#include "QError.h"
#include "valnames.h"

#include "SC_func.h"

class TSC_script;
class TSC_script_cmd_expression;

class TSC_functor_polycomp
{
public:
	double coef;
	int degs[3];
};

class TSC_functor : public TObject
{
private:
	int tpe;//0=unassigned 1=custom function name 2=local recipe 3=polynomial in up tp 3 variables
	//fpr tpe=1
	TSpecificObjectRef<TSC_func> functionref;
	//for tpe=2
	TSC_script_block *block;
	//for tpe==3
	Tarray<TSC_functor_polycomp> polycomps;
	Tdoublearray argpowers[3];


	Tarray<QString> argnames;
	QString description;
public:
	static StrPtr GetClassName() { return SC_valname_functor; }
	TSC_functor();
	~TSC_functor();
	void operator=(const TSC_functor &v);
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
	void set_function(TSC_func *ifunc);
	void parse(TSC_script *iscript, StrPtr content);
	void addargumentname(StrPtr argname);
	TSC_script_cmd_expression* G_expression();
	void eval(TSC_funcarglist *arglist, TSC_value *retval);

public:
	void init_polynomial();
	void add_polynomial(double coef, int deg1, int deg2, int deg3);

public:
	static bool compare(TSC_functor *v1, TSC_functor *v2) { return false; }
	static bool comparesize(TSC_functor *v1, TSC_functor *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
	void createstring(QString &str);

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

	int G_argcount() const { return argnames.G_count(); }

};
