#pragma once

#include "FormatText.h"

class TxtEquation;

class TxtEquationIndex : public TxtComponent
{
protected:
	TxtComponent *base,*sub,*super;
public:
	TxtEquationIndex(TxtEnv *ienv, const TxtFont &ifnt);
	~TxtEquationIndex();
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
	void Set_base(TxtComponent *icomp);
	void Set_sub(TxtComponent *icomp);
	void Set_super(TxtComponent *icomp);
};

class TxtEquationFrac : public TxtComponent
{
protected:
	TxtComponent *comp1,*comp2;
	double sep;
public:
	TxtEquationFrac(TxtEnv *ienv, const TxtFont &ifnt, TxtComponent *icomp1, TxtComponent *icomp2);
	~TxtEquationFrac();
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};

class TxtEquationMatrix : public TxtComponent
{
protected:
	Tarray< Tarray<TxtComponent> > lines;
	Tintarray colaligns;//-1=left 0=center 1=right
protected:
	TxtComponent* G_cell(int linenr, int colnr);
	const TxtComponent* G_cell(int linenr, int colnr) const;
public:
	TxtEquationMatrix(TxtEnv *ienv, const TxtFont &ifnt, StrPtr icontent, StrPtr alignstring);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};


class TxtEquationBracket : public TxtComponent
{
protected:
	TxtComponent *subcomp;
	int tpe;//0=( 1=[ 2=|
	double bsizex;
public:
	TxtEquationBracket(TxtEnv *ienv, const TxtFont &ifnt, TxtComponent *isubcomp, int itpe);
	~TxtEquationBracket();
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};

class TxtEquationSqrt : public TxtComponent
{
protected:
	TxtComponent *subcomp;
	double xsep,ysep;
public:
	TxtEquationSqrt(TxtEnv *ienv, const TxtFont &ifnt, TxtComponent *isubcomp);
	~TxtEquationSqrt();
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};

class TxtEquationVector : public TxtComponent
{
protected:
	TxtComponent *subcomp;
	double ysep;
public:
	TxtEquationVector(TxtEnv *ienv, const TxtFont &ifnt, TxtComponent *isubcomp);
	~TxtEquationVector();
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};

class TxtEquationOverline : public TxtComponent
{
protected:
	TxtComponent *subcomp;
	double ysep;
public:
	TxtEquationOverline(TxtEnv *ienv, const TxtFont &ifnt, TxtComponent *isubcomp);
	~TxtEquationOverline();
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};


//implementation of a variable in an equation
class TxtEquationVar : public TxtComponent
{
protected:
	QString content;
protected:
public:
	TxtEquationVar(TxtEnv *ienv, const TxtFont &ifnt, StrPtr icontent);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};

//implementation of a character in an equation
class TxtEquationChar : public TxtComponent
{
protected:
	QString content;
protected:
public:
	TxtEquationChar(TxtEnv *ienv, const TxtFont &ifnt, StrPtr icontent, bool issymbol);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};


//implementation of a summation sign in an equation
class TxtEquationSum : public TxtComponent
{
protected:
	QString sign;
public:
	TxtEquationSum(TxtEnv *ienv, const TxtFont &ifnt, StrPtr isign);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};

//implementation of a integral sign in an equation
class TxtEquationIntegral : public TxtComponent
{
protected:
	QString sign;
public:
	TxtEquationIntegral(TxtEnv *ienv, const TxtFont &ifnt);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};


class TxtEqToken
{
public:
	QString token,word;
	bool issymbol;
public:
	TxtEqToken(StrPtr itoken, StrPtr iword, bool iissymbol)
	{
		token=itoken;
		word=iword;
		issymbol=iissymbol;
	}
};

class TxtEquation : public TxtComponent
{
protected:
	Tarray<TxtComponent> components;
	Tarray<TxtEqToken> tokens;
protected:
	void addtoken(StrPtr token, StrPtr word, bool issymbol);
	void parsesub(const TxtFont &currentfnt, StrPtr icontent);
	TxtEquationIndex* create_indexcomponent();
public:
	TxtEquation(TxtEnv *ienv, const TxtFont &ifnt);
	virtual void parsefrom(StrPtr icontent);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};
