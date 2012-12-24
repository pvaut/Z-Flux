#pragma once

#include "tools.h"


class TSC_source;

class TSC_source_undoaction
{
public:
	bool isstart,isstop;
	int cursor_linenr,cursor_colnr;
public:
	TSC_source_undoaction()
	{
		isstart=false;isstop=false;
		cursor_linenr=-1;cursor_colnr=-1;
	};
	virtual ~TSC_source_undoaction() {}
	virtual void undo() {};
	virtual void redo() {};
};


class TSC_source_undoaction_start : public TSC_source_undoaction
{
public:
	TSC_source_undoaction_start(int i_linenr, int i_colnr)
	{ 
		isstart=true;
		cursor_linenr=i_linenr;
		cursor_colnr=i_colnr;
	};
};

class TSC_source_undoaction_stop : public TSC_source_undoaction
{
public:
	TSC_source_undoaction_stop(int i_linenr, int i_colnr)
	{ 
		isstop=true;
		cursor_linenr=i_linenr;
		cursor_colnr=i_colnr;
	};
};

class TSC_source_undoaction_insertstring : public TSC_source_undoaction
{
private:
	TSC_source *src;
	QString str;
public:
	TSC_source_undoaction_insertstring(TSC_source *isrc, StrPtr istr);
	virtual void undo();
	virtual void redo();
};

class TSC_source_undoaction_del : public TSC_source_undoaction
{
private:
	TSC_source *src;
	QString str;
public:
	TSC_source_undoaction_del(TSC_source *isrc, int icnt);
	virtual void undo();
	virtual void redo();
};

class TSC_source_undoaction_mergeline : public TSC_source_undoaction
{
private:
	TSC_source *src;
public:
	TSC_source_undoaction_mergeline(TSC_source *isrc);
	virtual void undo();
	virtual void redo();
};

class TSC_source_undoaction_insertline : public TSC_source_undoaction
{
private:
	TSC_source *src;
public:
	TSC_source_undoaction_insertline(TSC_source *isrc);
	virtual void undo();
	virtual void redo();
};



class TSC_source
{
private:
	QString filename;
	Tarray<QString> lines;
	int debugline;
	int cursor_linenr,cursor_colnr;
	int sel1_linenr,sel1_colnr,sel2_linenr,sel2_colnr;
	bool modified;
public:
	int scrollposy;
public:
	bool readonly;
	void Set_cursor(int linenr, int colnr, bool resetsel=true);
	void Set_sel(int linenr1, int colnr1, int linenr2, int colnr2);
	void cmd_selectblock();
	bool G_sel(int &linenrstart, int &colnrstart, int &linenrstop, int &colnrstop);
	bool G_issel();
	int G_cursor_linenr() { return cursor_linenr; }
	int G_cursor_colnr() { return cursor_colnr; }
	bool G_canmodify() { return !readonly; }
	bool G_ismodified() { return modified; }
public:
	TSC_source();
	~TSC_source();
	void clear();
	void load(StrPtr ifilename);
	void save();
	void saveas(StrPtr ifilename);
	StrPtr G_filename() { return filename; }
	int G_linecount() { return lines.G_count(); }
	const QString* G_line(int linenr);
	void Set_debugline(int nr) { debugline=nr; }
	int G_debugline() { return debugline; }

public://editing
	void cmd_mergeline(bool backup=true);
	void cmd_backspace(bool backup=true);
	void cmd_insertline(bool autoindent, bool backup=true);
	void cmd_cutselblock(bool backup=true);
	void cmd_del(int cnt, bool backup=true);
	void cmd_tab(bool invert, bool backup=true);
	void cmd_copy();
	void cmd_paste();
	void changecursorpos(int incrline, int incrcol, bool resetsel=true);
	void jumpcursorpos(int tpe, bool resetsel);
	void insertstring(StrPtr str, bool backup=true);
	void find(StrPtr istr);


private://undo functionality
	bool undo_busy;
	Tarray<TSC_source_undoaction> undos;
	int undos_current;
	void undo_begin();
	void undo_end();
public:
	void addundo(TSC_source_undoaction *undo);
	void undos_reset();
	bool G_canundo();
	bool G_canredo();
	void cmd_undo();
	void cmd_redo();

};