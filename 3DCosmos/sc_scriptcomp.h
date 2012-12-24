#pragma once

#include "SC_script.h"

class TSC_script_block;
class TSC_script_var;



class TSC_script_cmd_try : public TSC_script_cmd
{
private:
	TSC_script_block *subblock;
	bool failed;
public:
	TSC_script_cmd_try(TSC_script_block *iblock, Tscriptsourcerange &irange);
	~TSC_script_cmd_try();
	void reset();
	bool parse(Tscriptsourcepart &source);
public:
	virtual void exec_start();
	virtual void exec_stepone();
	virtual bool exec_isfinished();
};


class TSC_script_cmd_while : public TSC_script_cmd
{
private:
	TSC_script_cmd_expression *expr;
	TSC_script_block *subblock;
	bool expr_evaluated,expr_istrue;
public:
	TSC_script_cmd_while(TSC_script_block *iblock, Tscriptsourcerange &irange);
	~TSC_script_cmd_while();
	void reset();
	bool parse(Tscriptsourcepart &source);
public:
	virtual void exec_start();
	virtual void exec_stepone();
	virtual bool exec_isfinished();
};


class TSC_script_cmd_for : public TSC_script_cmd
{
private:
	TSC_script_var *indexvar;
	QString varname;
	TSC_script_cmd_expression *expr_assign,*expr_test;
	TSC_script_block *subblock;
	bool expr_evaluated;
	int startval,endval,curval;
public:
	TSC_script_cmd_for(TSC_script_block *iblock, Tscriptsourcerange &irange);
	~TSC_script_cmd_for();
	void reset();
	bool parse(Tscriptsourcepart &source);
public:
	virtual void exec_start();
	virtual void exec_stepone();
	virtual bool exec_isfinished();
};

class TSC_script_cmd_forall : public TSC_script_cmd
{
private:
	QString varname;
	TSC_script_cmd_expression *expr_list;
	TSC_script_block *subblock;
	bool expr_evaluated;
	TSC_list *lst;
	int curidx;
public:
	TSC_script_cmd_forall(TSC_script_block *iblock, Tscriptsourcerange &irange);
	~TSC_script_cmd_forall();
	void reset();
	bool parse(Tscriptsourcepart &source);
public:
	virtual void exec_start();
	virtual void exec_stepone();
	virtual bool exec_isfinished();
};




class TSC_script_insertcodeblock : public TSC_script_cmd
{
private:
	bool expr_evaluated;
	TSC_script_cmd_expression *expr;
	TSC_script_block *subblock;
public:
	TSC_script_insertcodeblock(TSC_script_block *iblock, Tscriptsourcerange &irange);
	~TSC_script_insertcodeblock();
	void reset();
	bool parse(Tscriptsourcepart &source);
public:
	virtual void exec_start();
	virtual void exec_stepone();
	virtual bool exec_isfinished();
};


class TSC_script_cmd_if : public TSC_script_cmd
{
private:
	bool expr_evaluated,expr_istrue;
	TSC_script_cmd_expression *expr_test;
	TSC_script_block *subblock;
public:
	TSC_script_cmd_if(TSC_script_block *iblock, Tscriptsourcerange &irange);
	~TSC_script_cmd_if();
	virtual StrPtr G_tpe() { return _qstr("then"); }
	void reset();
	bool parse(Tscriptsourcepart &source);
public:
	bool G_expr_istrue() { return expr_istrue; }
	virtual void exec_start();
	virtual void exec_stepone();
	virtual bool exec_isfinished();
};

class TSC_script_cmd_else : public TSC_script_cmd
{
private:
	TSC_script_cmd_if *iftest;
	TSC_script_block *subblock;
public:
	TSC_script_cmd_else(TSC_script_block *iblock, Tscriptsourcerange &irange, TSC_script_cmd_if *i_iftest);
	~TSC_script_cmd_else();
	void reset();
	bool parse(Tscriptsourcepart &source);
public:
	virtual void exec_start();
	virtual void exec_stepone();
	virtual bool exec_isfinished();
};
