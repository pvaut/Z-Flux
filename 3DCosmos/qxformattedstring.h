#pragma once

#include "tobject.h"
#include "tools.h"
#include "qstring.h"

class TQXFormattedStringState
{
public:
	bool bold,italic,gray;
	int startps;
	TQXFormattedStringState()
	{
		reset();
	}
	void reset()
	{
		bold=false;
		italic=false;
		gray=false;
		startps=0;
	}
	void operator=(const TQXFormattedStringState &arg)
	{
		bold=arg.bold;
		italic=arg.italic;
		gray=arg.gray;
		startps=arg.startps;
	}
};

class TQXFormattedString
{
private:
	TQXFormattedStringState currentstate;
	QString content;
	Tcheapitemarray<TQXFormattedStringState> components;
	void newcomponent();
	TQXFormattedStringState& G_curcomponent();
public:
	TQXFormattedString();
	TQXFormattedString(const TQXFormattedString &arg);
	void clear();
	void copyfrom(const TQXFormattedString &arg);
	void operator=(const TQXFormattedString &arg);
	void append(StrPtr arg);
	void appendnewline();
	void makebold(bool newstatus);
	void makeitalic(bool newstatus);
	void operator+=(StrPtr arg) { append(arg); }
	TQXFormattedString operator+(const TQXFormattedString &arg) const;
public:
	void draw(CDC *dc, int x0, int y0);
	void drawmultiline(CDC *dc, int x0, int y0, int xlen);
	void ExportLaTex(QString &expstr);//export LaTex style
};