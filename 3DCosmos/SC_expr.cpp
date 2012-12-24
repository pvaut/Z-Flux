

#include "stdafx.h"

#include "qtranslate.h"

#include "SC_expr.h"

#include "SC_script.h"


class TSC_operatordef
{
private:

	Tarray<QString > operatornames,invalids;
	Tintarray operatorids;
	void add(StrPtr iname, int iID)
	{
		operatornames.add(new QString(iname));
		operatorids.add(iID);
	}
public:
	TSC_operatordef()
	{
		add(_qstr("="),     2);
		add(_qstr(" and "), 0);
		add(_qstr(" or "),  1);
		add(_qstr("=="),    3);
		add(_qstr("!="),   14);
		add(_qstr("<="),    6);
		add(_qstr(">="),    7);
		add(_qstr("<"),     4);
		add(_qstr(">"),     5);
		add(_qstr("+"),     8);
		add(_qstr("-"),     9);
		add(_qstr("*"),    10);
		add(_qstr("/"),    11);
		add(_qstr("^"),    12);
		add(_qstr(":"),    15);
		add(_qstr("."),    OPID_DOT);

		invalids.add(new QString(" "));
		invalids.add(new QString("("));invalids.add(new QString(")"));
		invalids.add(new QString("["));invalids.add(new QString("]"));
		invalids.add(new QString("{"));invalids.add(new QString("}"));
	}
	int G_operatorcount() { return operatornames.G_count(); }
	StrPtr G_operatorname_byID(int opID)
	{
		for (int i=0; i<operatorids.G_count(); i++)
			if (operatorids[i]==opID)
				return *operatornames[i];
		return _qstr("");
	}
	StrPtr G_operatorname(int nr)
	{
		ASSERT((nr>=0)&&(nr<operatornames.G_count()));
		return *operatornames[nr];
	}
	int G_operatorID(int nr)
	{
		ASSERT((nr>=0)&&(nr<operatornames.G_count()));
		return operatorids[nr];
	}

	int G_invalididentiferpart_count() { return invalids.G_count(); }
	StrPtr G_invalididentiferpart(int nr)
	{
		ASSERT((nr>=0)&&(nr<invalids.G_count()));
		return *invalids[nr];
	}

public:
	static TSC_operatordef& Get()
	{
		 static TSC_operatordef env;
		 return env;
	}
};

StrPtr G_operatortypename(int opID)
{
	return TSC_operatordef::Get().G_operatorname_byID(opID);
}

bool TSC_exprcomp::parse(Tscriptsourcepart &source)
{
	int level,level2,foundpos,i,j;
	bool ok;
	int startps=0;
	int endps=source.count-1;
	bool processed=false;

	subcomp.reset();

	//remove trailing ;
	if (source[endps]==';') endps--;

	//remove surrounding spaces
	while ((startps<endps)&&(source[startps]==' ')) startps++;
	while ((startps<endps)&&(source[endps]==' ')) endps--;


	Tchararray instring;

//marks the characters that occur inside a string
	instring.reset();
	ok=false;
	for (i=0; i<source.count; i++)
	{
		if (isquote(source[i]))
		{
			if (!ok) ok=true;
			else { if ((i==0)||(source[i-1]!=ESCCHAR)) ok=false; }
		}
		instring.add(ok);
	}


//remove possible extra surrounding brackets
	for (bool reduced=true; reduced;)
	{
		reduced=false;
		if ((endps>startps)&&(source[startps]==' ')&&(source[endps]==' '))
		{
			reduced=true;
			startps++;endps--;
		}
		if ((endps>startps)&&(source[startps]=='(')&&(source[endps]==')'))
		{
			level=0;ok=true;
			for (i=startps+1; i<endps; i++)
			{
				if ((!instring[i])&&(source[i]=='(')) level++;
				if ((!instring[i])&&(source[i]==')')) level--;
				if (level<0) ok=false;
			}
			if (ok)
			{
				reduced=true;
				startps++;endps--;
			}
		}
	}


	//check if content equals a number
	if (!processed)
	{
		StrChar *endptr;
		double r=wcstod(&source.txt[startps],&endptr);
		if (endptr==&source.txt[endps]+1)
		{
			tpe=1;
			value.settype(GetTSCenv().G_datatype(SC_valname_scalar));
			value.copyfrom(r);
			processed=true;
		}
	}

	//check if content equals a string
	if (!processed)
	{
		if ((endps>startps)&&(isquote(source[startps]))&&(isquote(source[endps])))
		{
			ok=true;
			for (i=startps; i<endps; i++)	if (!instring[i]) ok=0;
			if (ok)
			{
				if (isquotesingle(source[startps])!=isquotesingle(source[endps]))
				{
					expr->G_parentblock()->G_script()->Set_error(source,startps,endps,_text("Inconsistent string quotes"));
					return false;
				}
				tpe=1;
				QString value_string;
				for (i=startps+1; i<endps;)
				{
					if ((source[i]==ESCCHAR)&&(i<endps-1))
					{
						i++;
						if (source[i]=='t') value_string+='\t';
						if (source[i]=='n') value_string+='\n';
						if (source[i]=='r') value_string+='\r';
						if (source[i]=='"') value_string+='"';
						if (source[i]=='\'') value_string+='\'';
						if (source[i]=='\\') value_string+='\\';
					}
					else value_string+=source[i];
					i++;
				}
				value.settype(GetTSCenv().G_datatype(SC_valname_string));
				value.fromstring(value_string);
				if (isquotesingle(source[startps])) value.fromstring(QTranslate::Get().Translate(value_string));
				processed=true;
			}
		}
	}

	//check if content equals a boolean value
	if (!processed)
	{
		if ((endps-startps+1>=4)&&(qstrnicmp(&source.txt[startps],_qstr("true"),endps-startps+1)==0))
		{
			tpe=1;
			value.settype(GetTSCenv().G_datatype(SC_valname_boolean));
			value.fromstring(_qstr("True"));
			processed=true;
		}
		if ((endps-startps+1>=5)&&(qstrnicmp(&source.txt[startps],_qstr("false"),endps-startps+1)==0))
		{
			tpe=1;
			value.settype(GetTSCenv().G_datatype(SC_valname_boolean));
			value.fromstring(_qstr("False"));
			processed=true;
		}
	}


	//check for operator
	for (int operatornr=0; operatornr<TSC_operatordef::Get().G_operatorcount(); operatornr++)
	{
		if (!processed)
		{
			StrPtr opname=TSC_operatordef::Get().G_operatorname(operatornr);

			level=0;level2=0;foundpos=-1;
			if ((!instring[startps])&&(source[startps]=='(')) level++;
			if ((!instring[startps])&&(source[startps]=='[')) level2++;
			for (i=startps+1; i<=endps-qstrlen(opname); i++)
			{
				ASSERT(level>=0);ASSERT(level2>=0);
				if ((!instring[i])&&(source[i]=='(')) level++;
				if ((!instring[i])&&(source[i]==')')) level--;
				if ((!instring[i])&&(source[i]=='[')) level2++;
				if ((!instring[i])&&(source[i]==']')) level2--;
				if ((!instring[i])&&(level==0)&&(level2==0))
					if (qstrnicmp(&source.txt[i],opname,qstrlen(opname))==0)
						foundpos=i;
			}
			if ((foundpos>0)&&(TSC_operatordef::Get().G_operatorID(operatornr)==2))//special check for <= , >= , != and ==
			{
				if ((source[foundpos-1]=='<')||(source[foundpos-1]=='>')||(source[foundpos-1]=='=')||(source[foundpos-1]=='!')) foundpos=-1;
			}
			if (foundpos>=0)
			{
				tpe=3;
				operatortype=TSC_operatordef::Get().G_operatorID(operatornr);
				if (isassignable&&(operatortype!=OPID_DOT))
				{
					expr->G_parentblock()->G_script()->Set_error(source,startps,endps,_text("Invalid assignment"));
					return false;
				}
				Tscriptsourcepart sub1(source,startps,foundpos-1);
				TSC_exprcomp *comp1=new TSC_exprcomp(expr);subcomp.add(comp1);
				Tscriptsourcepart sub2(source,foundpos+qstrlen(opname),endps);
				TSC_exprcomp *comp2=new TSC_exprcomp(expr);subcomp.add(comp2);
				if (operatortype==2) comp1->isassignable=true;
				if (!comp1->parse(sub1)) return false;
				if (!comp2->parse(sub2)) return false;
				processed=true;
			}
		}
	}

	//check if function argument list is present
	if ((!processed)&&(source[endps]==')'))
	{
		subcomptype=1;
		int sep=startps;

		//search for last '('
		sep=endps-1;level=0;level2=0;
		while ((sep>startps) && ((source[sep]!='(')||(instring[sep])||(level>0)||(level2>0)) )
		{
			if ((!instring[sep])&&(source[sep]==')')) level++;
			if ((!instring[sep])&&(source[sep]=='(')) level--;
			if ((!instring[sep])&&(source[sep]==']')) level2++;
			if ((!instring[sep])&&(source[sep]=='(')) level2--;
			sep--;
		}

//		while ((sep<endps-1)&&(source[sep]!='(')) sep++;

		if (source[sep]=='(')
		{
			int ps2=sep+1;
			while (ps2<endps)
			{
				int ps3=ps2;level=0;level2=0;
				while ( (ps3<endps)&& ((source[ps3]!=',')||(instring[ps3])||(level>0)||(level2>0)) )
				{
					if ((!instring[ps3])&&(source[ps3]=='(')) level++;
					if ((!instring[ps3])&&(source[ps3]==')')) level--;
					if ((!instring[ps3])&&(source[ps3]=='[')) level2++;
					if ((!instring[ps3])&&(source[ps3]==']')) level2--;
					ps3++;
				}
				Tscriptsourcepart sub(source,ps2,ps3-1);
				TSC_exprcomp *comp=new TSC_exprcomp(expr);subcomp.add(comp);
				if (!comp->parse(sub)) return false;
				ps2=ps3+1;
			}
			endps=sep-1;
		}
		else
		{
			expr->G_parentblock()->G_script()->Set_error(source,startps,endps,_text("Unbalanced brackets in function call"));
			return false;
		}
	}

	//check if valid identifier
	if (!processed)
	{
		ok=true;
		if (endps<startps) ok=false;
		for (j=0; j<TSC_operatordef::Get().G_invalididentiferpart_count(); j++)
		{
			StrPtr inv=TSC_operatordef::Get().G_invalididentiferpart(j);
			for (i=startps; i<=endps-qstrlen(inv); i++)
				if (qstrnicmp(&source.txt[i],inv,qstrlen(inv))==0) ok=false;
		}
		if (ok)
		{
			identifiername.clear();
			for (i=startps; i<=endps; i++) identifiername+=source[i];
			tpe=4;
			processed=true;
		}
	}


	if (processed) return true;
	else
	{
		expr->G_parentblock()->G_script()->Set_error(source,startps,endps,_text("Syntax error"));
		return false;
	}
}


StrPtr TSC_exprcomp::G_assign_varname()//if expression is a variable assignment, returns the name of the variable in the left part
{
	if ((tpe!=3)||(operatortype!=2)) return _qstr("");
	if (subcomp[0]->tpe!=4) return _qstr("");
	return subcomp[0]->identifiername;
}



TSC_value* TSC_exprcomp::execute(TSC_value *assigntoval)
{
//	for (int subcompnr=0; subcompnr<subcomp.G_count(); subcompnr++)
//		subcomp[subcompnr]->execute();

	if (tpe==1)
	{
		if (assigntoval!=NULL) 
			throw TSC_runerror(expr->G_range(),_text("Invalid assignment"));
		return &value;
	}

	if ((tpe==3)&&(operatortype==2)) return execute_operator_assign(assigntoval);
	if ((tpe==3)&&(operatortype==OPID_DOT)) return execute_operator_dot(assigntoval);

	if (tpe==3)
	{
		if (assigntoval!=NULL) 
			throw TSC_runerror(expr->G_range(),_text("Invalid assignment"));
		return execute_operator();
	}


	if (tpe==4) return execute_identifier(assigntoval);

	throw TSC_runerror(expr->G_range(),_text("Non-executable expression"));
	return NULL;
}

TSC_value* TSC_exprcomp::execute_identifier(TSC_value *assigntoval)
{
	TSC_script_var *var=expr->findvar(this,identifiername);

	//try as type cast
	if (identifiername[0]==TYPECASTCHAR)
	{
		if (subcomp.G_count()!=1) throw TSC_runerror(expr->G_range(),_qstr("Type conversion should have exactly 1 argument"));
		TSC_value *val=subcomp[0]->execute(NULL);
		if (val==NULL) throw TSC_runerror(expr->G_range(),_qstr("Type conversion argument does not return a value"));
		QString error;
		if (!expr->G_parentblock()->G_script()->G_env()->evaltypecast(identifiername,val,&value,error))
			throw TSC_runerror(expr->G_range(),error);
		
		return &value;
	}

	if (var!=NULL)//return existing variable
	{
		if (subcomp.G_count()>0)//return indexed variable = shortcut for general 'get' function
		{
			TSC_funcarglist arglist(expr->G_parentblock());
			for (int subnr=0; subnr<subcomp.G_count(); subnr++) arglist.add(subcomp[subnr]->execute(NULL));
			QString error;
			if (!expr->G_parentblock()->G_script()->G_env()->evalmemberfunction(_qstr("get"),&value,var->G_value(),assigntoval,arglist,error))
				throw TSC_runerror(expr->G_range(),error);
			return &value;
		}
		if (assigntoval!=NULL)
		{
			if (!var->G_value()->copyfrom(assigntoval))
				throw TSC_runerror(expr->G_range(),_text("Could not copy value"));
		}
		return var->G_value();
	}

	if ((isassignable)&&(subcomp.G_count()==0))
	{//create new variable
		TSC_value *val=new TSC_value;
		if (assigntoval!=NULL) val->copyfrom(assigntoval);
		return expr->G_parentblock()->G_varset()->addvar(identifiername,val)->G_value();
	}

	//try as createvar function
	if (qstricmp(identifiername,_qstr("createvar"))==0)
	{
		if (subcomp.G_count()!=1) throw TSC_runerror(expr->G_range(),_text("Invalid argument list for 'createvar'"));
		StrPtr name=subcomp[0]->identifiername;
		if (qstrlen(name)==0) throw TSC_runerror(expr->G_range(),_text("Invalid argument for 'createvar'"));
		expr->G_parentblock()->addvar(name,new TSC_value);
		return NULL;
	}

	//try as enum value
	for (int enumnr=0; enumnr<GetTSCenv().G_enumtypecount(); enumnr++)
	{
		TSC_datatype_genericenum *enumtype=GetTSCenv().G_enumtype(enumnr);
		int vl=enumtype->string2int(identifiername);
		if (vl>=0)
		{
			value.settype(enumtype);
			value.fromstring(identifiername);
			return &value;
		}
	}


	//try as function
	TSC_funcarglist arglist(expr->G_parentblock());
	for (int subnr=0; subnr<subcomp.G_count(); subnr++) arglist.add(subcomp[subnr]->execute(NULL));
	QString error;
	if (!expr->G_parentblock()->evalfunction(identifiername,&value,assigntoval,arglist,error))
		throw TSC_runerror(expr->G_range(),error);
	return &value;

	error=_text("Unknown identifier '");error+=identifiername;error+=_text("'");
	throw TSC_runerror(expr->G_range(),error);
	return NULL;
}

TSC_value* TSC_exprcomp::execute_operator_assign(TSC_value *assigntoval)
{
	if (assigntoval!=NULL) 
		throw TSC_runerror(expr->G_range(),_text("Invalid assignment"));
	ASSERT(subcomp.G_count()==2);
	TSC_value *valright=subcomp[1]->execute(NULL);
	if (valright==NULL)
		throw TSC_runerror(expr->G_range(),_text("Empty right part of assignment"));
	TSC_value *valleft=subcomp[0]->execute(valright);
/*	if (val1==NULL)	throw TSC_runerror(expr->G_range(),_text("Empty left part of assignment"));
	if (!val1->copyfrom(val2))
	{
		throw TSC_runerror(expr->G_range(),_text("Unable to convert data types"));
	}*/
	return valleft;
}


TSC_value* TSC_exprcomp::execute_operator_dot(TSC_value *assigntoval)
{
	ASSERT(subcomp.G_count()==2);
	TSC_value *val1=subcomp[0]->execute(NULL);

	if (subcomp[1]->tpe!=4)
		throw TSC_runerror(expr->G_range(),_text("Invalid right part of dot operator"));
	StrPtr funcname=subcomp[1]->identifiername;
	TSC_funcarglist arglist(expr->G_parentblock());
	for (int subnr=0; subnr<subcomp[1]->subcomp.G_count(); subnr++)
		arglist.add(subcomp[1]->subcomp[subnr]->execute(NULL));

	QString error;
	if (!expr->G_parentblock()->G_script()->G_env()->evalmemberfunction(funcname,&value,val1,assigntoval,arglist,error))
		throw TSC_runerror(expr->G_range(),error);

	return &value;
}

TSC_value* TSC_exprcomp::execute_operator()
{
	if (subcomp.G_count()!=2) throw TSC_runerror(expr->G_range(),_text("Invalid number of arguments for operator"));
	TSC_value *val1=subcomp[0]->execute(NULL);
	TSC_value *val2=subcomp[1]->execute(NULL);

	if (!expr->G_parentblock()->G_script()->G_env()->evaloperator(this,operatortype,val1,val2,&value,tmp_error))
		throw TSC_runerror(expr->G_range(),tmp_error);

	return &value;
}


void TSC_exprcomp::createstring(TSC_exprcomp *parentcomp, QString &str)
{
	if (tpe==1)
	{
		value.tostring(str,0);
		return;
	}

	if (tpe==3)
	{
		bool needbrackets=false;
		if ((parentcomp!=NULL)&&(parentcomp->tpe==3))
		{
			if (parentcomp->operatortype>operatortype) needbrackets=true;
			if ((parentcomp->operatortype==9)&&(operatortype==9)) needbrackets=true;
		}
		QString st1,st2;
		subcomp[0]->createstring(this,st1);
		subcomp[1]->createstring(this,st2);
		str.clear();
		if (needbrackets) str+=_qstr("(");
		str+=st1;
		str+=TSC_operatordef::Get().G_operatorname_byID(operatortype);
		str+=st2;
		if (needbrackets) str+=_qstr(")");
		return;
	}


	if (tpe==4)
	{
		str=identifiername;
		if (subcomp.G_count()>0)
		{
			str+=_qstr("(");
			for (int i=0; i<subcomp.G_count(); i++)
			{
				QString stsub;
				if (i>0) str+=_qstr(",");
				subcomp[i]->createstring(this,stsub);
				str+=stsub;
			}
			str+=_qstr(")");
		}
	}
}
