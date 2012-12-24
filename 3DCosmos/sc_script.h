#pragma once

#include "qxcolor.h"

#include "SC_datatype_enum.h"
#include "SC_source.h"
#include "SC_env.h"
#include "SC_expr.h"

class TSC_script_block;
class TSC_script;
class TSC_script_customfunc;

#define ESCCHAR '~'
#define TYPECASTCHAR '@'



class Tscriptsourcerange
{
public:
	int line1,col1;
	int line2,col2;
public:
	Tscriptsourcerange()
	{
		line1=0;col1=0;
		line2=0;col2=0;
	}
	void copyfrom(Tscriptsourcerange &irange)
	{
		line1=irange.line1;col1=irange.col1;
		line2=irange.line2;col2=irange.col2;
	}
};

class Tscriptsourcepart
{
public:
	const StrChar *txt;
	int *linenr;
	int *colnr;
	int count;
public:
	Tscriptsourcepart()
	{
		txt=NULL;
		linenr=NULL;colnr=NULL;count=0;
	}
	Tscriptsourcepart(Tscriptsourcepart &parent, int ps1, int ps2)
	{
		ASSERT(ps1>=0);ASSERT(ps2>=ps1);ASSERT(ps2<parent.count);
//		if (ps2<ps1) throw QError(_text("Empty statement"));//--!!--added as a test
		txt=&parent.txt[ps1];
		linenr=&parent.linenr[ps1];
		colnr=&parent.colnr[ps1];
		count=ps2-ps1+1;
	}
	StrChar operator[](int nr)
	{
		if (txt==NULL) return ' ';
		if ((nr<0)|(nr>=count)) return ' ';
		return txt[nr];
	}
	void filltext(QString &str)
	{
		CString st;
		st=txt;
		str=(StrPtr)st.Left(count);
	}
	void getrange(Tscriptsourcerange &range, int ps1, int ps2)
	{
		ASSERT(ps1>=0);ASSERT(ps2>=ps1);ASSERT(ps2<count);
		range.line1=linenr[ps1];range.col1=colnr[ps1];
		range.line2=linenr[ps2];range.col2=colnr[ps2];
	}
};


class TSC_runerror: public std::exception{
private:
	Tscriptsourcerange sourcelocation;
	QString content;
public:
	TSC_runerror(Tscriptsourcerange &isourcelocation, StrPtr icontent)
	{
		sourcelocation=isourcelocation;
		content=icontent;
	}
	virtual StrPtr G_content()
	{
		return content;
	}
};


class TSC_script_var
{
private:
	QString name;
	TSC_value *value;
public:
	TSC_script_var(StrPtr iname, TSC_value *ivalue);
	~TSC_script_var();
	TSC_value* G_value() { return value; }
	StrPtr G_name() { return name; }
	void copy_value(TSC_value *ivalue);
	void encapsulate(TSC_value *ivalue);
};


class TSC_script_varset
{
private:
	Tarray<TSC_script_var> vars;
	Tbaltree index;
public:
	TSC_script_var* addvar(StrPtr iname, TSC_value *icontent);
	void reset();
	TSC_script_var* findvar(const TSC_ExpressionCacher *cache, StrPtr varname);
	TSC_script_var* findvar_required(const TSC_ExpressionCacher *cache, StrPtr varname);
	int G_varcount() { return vars.G_count(); }
	TSC_script_var* G_var(int nr) { return vars[nr]; }
};


class TSC_script_cmd : public TSC_ExpressionCacher
{
protected:
	TSC_script_block *parentblock;
protected:
	Tscriptsourcerange range;
	QString descriptioncontent;
public:
	TSC_script_cmd(TSC_script_block *iblock, Tscriptsourcerange &irange)
	{
		parentblock=iblock;
		range.copyfrom(irange);
	}
	virtual ~TSC_script_cmd()
	{
	}
	TSC_script_var* findvar(const TSC_ExpressionCacher *cache, StrPtr varname);
	virtual void exec_start()=0;
	virtual bool exec_isfinished()=0;
	virtual void exec_stepone()=0;
	virtual StrPtr G_tpe() { return _qstr(""); }
	TSC_script_block* G_parentblock() { return parentblock; }
	Tscriptsourcerange& G_range() { return range; }

	void GetDescription(QString &str);
};

class TSC_script_cmd_expression : public TSC_script_cmd
{
private:
	TSC_exprcomp root;
	TSC_value *retval;
	bool executed;
public:
	virtual StrPtr G_tpe() { return _qstr("expression"); }
	TSC_script_cmd_expression(TSC_script_block *iblock, Tscriptsourcerange &irange);
	bool parse(Tscriptsourcepart &source);
	virtual TSC_value* G_value();
	TSC_exprcomp* G_root() { return &root; }
public:
	virtual void exec_start();
	virtual void exec_stepone();
	virtual bool exec_isfinished();
};

class TSC_script_block
{
private:
	TSC_script *script;
	TSC_script_block *parentblock;
	TSC_script_varset varset;
	Tarray<TSC_script_cmd> commands;
	Tarray<TSC_script_customfunc> customfunctions;
	TSC_value *returnvalue;
	bool executed;
	int curcommand;
	bool isruntimecode;//determines is this block was generated with the 'codeblock' statement
	bool isfunctionblock;//true if this block contains the definition of a custom function
	QString functionname;
private:
	bool parse_expression(Tscriptsourcepart &source);
	bool parse_try(Tscriptsourcepart &source);
	bool parse_while(Tscriptsourcepart &source);
	bool parse_for(Tscriptsourcepart &source);
	bool parse_forall(Tscriptsourcepart &source);
	bool parse_if(Tscriptsourcepart &source);
	bool parse_else(Tscriptsourcepart &source);
	bool parse_function(Tscriptsourcepart &source);
	bool parse_insertcodeblock(Tscriptsourcepart &source);
	int splitkeyword(Tscriptsourcepart &source);
	bool parsecomp(Tscriptsourcepart &source);
public:
	TSC_script_block(TSC_script *iscript, TSC_script_block *iparentblock);
	~TSC_script_block();
	TSC_script_varset* G_varset() { return &varset;	}
	TSC_script_block* G_parentblock() { return parentblock; }
	TSC_script* G_script() { return script; }
	void reset();
	void Set_isruntimecode() { isruntimecode=true; }
	void Set_isfunctionblock(StrPtr ifunctionname) { isfunctionblock=true; functionname=ifunctionname; }
	bool G_isfunctionblock() const { return isfunctionblock; }
	void resetvars();
	TSC_script_var* addvar(StrPtr iname, TSC_value *icontent);
	void addcommand(TSC_script_cmd *icommand);
	void addcustomfunction(TSC_script_customfunc *func);
	bool compile(Tscriptsourcepart &source);
	TSC_script_cmd* G_command(int nr) { return commands[nr]; }
public:
	TSC_script_var* findvar(const TSC_ExpressionCacher *cache, StrPtr varname);
	void GetDescription(QString &str);
	virtual void exec_start();
	virtual void exec_stop();
	virtual void exec_stepone();
	virtual bool exec_isstarted();
	virtual bool exec_isfinished();
	void Set_returnvalue(TSC_value *ival) { returnvalue=ival; }
	TSC_value* G_returnvalue() { return returnvalue; }
	void exec_buildstack(Tarray<QString> &stacklist);
	void exec_buildvarlist(int curdepth, Tcopyarray<TSC_script_var> &varlist, Tintarray &vardepthlist);
	TSC_script_customfunc* G_customfunction(StrPtr funcname);
	bool evalfunction(StrPtr funcname,TSC_value *returnval, TSC_value *assigntoval, TSC_funcarglist &arglist, QString &error);
};


class TSC_keyword
{
public:
	QString name;
	TQXColor color;
};

class TSC_script
{
private:
	TSC_env *env;
	TSC_source source;
	TSC_script_block root;
	TSC_script_block *curdebugblock;
	bool compile(QString &cerror);
	TSC_value scriptargument;
	QString catchederror;
private:
	bool exec_isbreak;
	struct
	{
		QString content;
		int line1,col1,line2,col2;
	} error;
public:
	QString stopstring;
public:
	TSC_script(TSC_env *ienv);
	~TSC_script();
	TSC_source& G_source() { return source; }
	TSC_env* G_env() { return env; }
	TSC_script_block* G_root() { return &root; }
	void addexternalvar(StrPtr iname, TSC_value *ival);
	void Set_error(Tscriptsourcepart &source, int ps1, int ps2, StrPtr icontent);
	void Set_debug_currentblock(TSC_script_block *iblock) { curdebugblock=iblock; }
	TSC_script_block* G_debug_currentblock() { return curdebugblock; }
	StrPtr G_error() { return error.content; }
	StrPtr G_catchederror() { return catchederror; }
	void Set_catchederror(StrPtr icontent) { catchederror=icontent; }


	bool exec_isstarted();
	bool exec_isfinished();
	bool G_exec_isbreak() { return exec_isbreak; }
	void exec_clearbreak() { exec_isbreak=false; }
	bool exec_start(QString &cerror);
	bool exec_stepone(QString &cerror);
	bool exec_run(const TSC_value *iargument, QString &cerror);
	void exec_stop();
	void exec_break() { exec_isbreak=true; }

	void exec_buildstack(Tarray<QString> &stacklist);
	void exec_buildvarlist(Tcopyarray<TSC_script_var> &varlist, Tintarray &vardepthlist);

	const TSC_value* G_scriptargument() { return &scriptargument; }

private:
	Tarray<TSC_keyword> keywords;
	Tbaltree keywordindex;
public:
	void addkeyword(StrPtr name, TQXColor &col);
	bool G_keyword(StrPtr name, TQXColor &col);

private:
	Tarray<QString> outputlines;
public:
	void add_output_line(StrPtr iline);
	int G_output_linecount() const { return outputlines.G_count(); }
	StrPtr G_output_line(int nr) const;

};