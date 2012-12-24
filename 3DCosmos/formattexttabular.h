#pragma once

#include "FormatText.h"


class TabularCell : public TxtBlock
{
	friend class TxtTabular;
private:
	int colnr_start,colnr_end;
	int align;//-2:take default from table -1:left 0:center 1:right
public:
	TabularCell(TxtEnv *ienv, const TxtFont &ifnt);
};

class TabularLine
{
	friend class TxtTabular;
private:
	Tarray<TabularCell> cells;
public:
};

class TxtTabular : public TxtComponent
{
protected:
	int colcount;
	Tintarray colaligns;//-1:left 0:center 1:right
	Tdoublearray fixedcolsizes;
	Tarray<TabularLine> lines;
	Tdoublearray colposx,lineposy;
	Tintarray horlinetopof,vertlineleftfrom;
public:
	TxtTabular(TxtEnv *ienv, const TxtFont &ifnt);
	virtual void parsefrom(StrPtr icols, StrPtr icontent);
	virtual void calc_implement(const TxtRenderer &renderer, double maxsizex);
	virtual void render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const;
};
