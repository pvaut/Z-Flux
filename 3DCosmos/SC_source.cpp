#include "stdafx.h"
#include "qfile.h"
#include "qerror.h"
#include "SC_source.h"


void copyclipboard(StrPtr str);
bool pasteclipboard(QString &str);


////////////////////////////////////////////////////////////////////////////////
// TSC_source_undoaction_insertstring
////////////////////////////////////////////////////////////////////////////////

TSC_source_undoaction_insertstring::TSC_source_undoaction_insertstring(TSC_source *isrc, StrPtr istr)
{
	src=isrc;
	cursor_linenr=src->G_cursor_linenr();
	cursor_colnr=src->G_cursor_colnr();
	str=istr;
}

void TSC_source_undoaction_insertstring::undo()
{
	src->Set_cursor(cursor_linenr,cursor_colnr);
	src->cmd_del(qstrlen(str),false);
}

void TSC_source_undoaction_insertstring::redo()
{
	src->Set_cursor(cursor_linenr,cursor_colnr);
	src->insertstring(str,false);
}


////////////////////////////////////////////////////////////////////////////////
// TSC_source_undoaction_del
////////////////////////////////////////////////////////////////////////////////

TSC_source_undoaction_del::TSC_source_undoaction_del(TSC_source *isrc, int icnt)
{
	src=isrc;
	cursor_linenr=src->G_cursor_linenr();
	cursor_colnr=src->G_cursor_colnr();
	for (int i=0; i<icnt; i++)
		str+=src->G_line(cursor_linenr)->G_char(cursor_colnr+i);
}

void TSC_source_undoaction_del::undo()
{
	src->Set_cursor(cursor_linenr,cursor_colnr);
	src->insertstring(str,false);
}

void TSC_source_undoaction_del::redo()
{
	src->Set_cursor(cursor_linenr,cursor_colnr);
	src->cmd_del(str.G_length(),false);
}


////////////////////////////////////////////////////////////////////////////////
// TSC_source_undoaction_mergeline
////////////////////////////////////////////////////////////////////////////////

TSC_source_undoaction_mergeline::TSC_source_undoaction_mergeline(TSC_source *isrc)
{
	src=isrc;
	cursor_linenr=src->G_cursor_linenr();
	cursor_colnr=src->G_cursor_colnr();
}

void TSC_source_undoaction_mergeline::undo()
{
	src->Set_cursor(cursor_linenr,cursor_colnr);
	src->cmd_insertline(false,false);
}

void TSC_source_undoaction_mergeline::redo()
{
	src->Set_cursor(cursor_linenr,cursor_colnr);
	src->cmd_mergeline(false);
}

////////////////////////////////////////////////////////////////////////////////
// TSC_source_undoaction_insertline
////////////////////////////////////////////////////////////////////////////////

TSC_source_undoaction_insertline::TSC_source_undoaction_insertline(TSC_source *isrc)
{
	src=isrc;
	cursor_linenr=src->G_cursor_linenr();
	cursor_colnr=src->G_cursor_colnr();
}

void TSC_source_undoaction_insertline::undo()
{
	src->Set_cursor(cursor_linenr,cursor_colnr);
	src->cmd_mergeline(false);
}

void TSC_source_undoaction_insertline::redo()
{
	src->Set_cursor(cursor_linenr,cursor_colnr);
	src->cmd_insertline(false,false);
}


////////////////////////////////////////////////////////////////////////////////
// TSC_source::TSC_source
////////////////////////////////////////////////////////////////////////////////


TSC_source::TSC_source()
{
	readonly=false;
	debugline=-1;
	cursor_linenr=0;cursor_colnr=0;
	modified=false;
	lines.add(new QString);
	undos_current=-1;
	undo_busy=false;
	scrollposy=0;
}

TSC_source::~TSC_source()
{
	clear();
}


void TSC_source::clear()
{
	undos.reset();
	lines.reset();
}


void TSC_source::load(StrPtr ifilename)
{
	QTextfile file;
	QString line;

	try{

		lines.reset();
		readonly=false;
		debugline=-1;
		cursor_linenr=0;cursor_colnr=0;
		modified=false;
		scrollposy=0;

		filename=ifilename;
		file.openread(ifilename);
		while (!file.isend())
		{
			file.readline(line);
			lines.add(new QString(line));
		}
		file.close();
	}
	catch(QError error)
	{
		reporterror(error);
	}

	if (lines.G_count()<=0) lines.add(new QString);
	undos_reset();
}

void TSC_source::saveas(StrPtr ifilename)
{
	filename=ifilename;
	save();
}


void TSC_source::save()
{
	QTextfile file;

	file.openwrite(filename);
	for (int i=0; i<lines.G_count(); i++)
	{
		if (i>0) file.writeline();
		file.write(*lines[i]);
	}
	file.close();
	modified=false;
}




const QString* TSC_source::G_line(int linenr)
{
	if ((linenr<0)||(linenr>=lines.G_count()))
	{
		ASSERT(false);
		return NULL;
	}
	return lines[linenr];
}


void TSC_source::Set_cursor(int linenr, int colnr, bool resetsel)
{
	if (linenr<0) linenr=0;
	if (colnr<0) colnr=0;
	if (G_line(linenr)!=NULL)
		if (colnr>G_line(linenr)->G_length()) colnr=G_line(linenr)->G_length();
	cursor_linenr=linenr;
	cursor_colnr=colnr;
	if (resetsel) Set_sel(linenr,colnr,linenr,colnr);
	else Set_sel(sel1_linenr,sel1_colnr,linenr,colnr);
}

void TSC_source::Set_sel(int linenr1, int colnr1, int linenr2, int colnr2)
{
	sel1_linenr=linenr1;sel1_colnr=colnr1;sel2_linenr=linenr2;sel2_colnr=colnr2;
}

void TSC_source::cmd_selectblock()
{
	sel1_linenr=cursor_linenr;sel1_colnr=cursor_colnr;sel2_linenr=cursor_linenr;sel2_colnr=cursor_colnr;
	while ((sel1_colnr>0)&&(Qisalphanumerical(G_line(cursor_linenr)->G_char(sel1_colnr-1)))) sel1_colnr--;
	while ((sel2_colnr<=G_line(cursor_linenr)->G_length()-1)&&(Qisalphanumerical(G_line(cursor_linenr)->G_char(sel2_colnr)))) sel2_colnr++;
	cursor_colnr=sel2_colnr;
}


bool TSC_source::G_sel(int &linenrstart, int &colnrstart, int &linenrstop, int &colnrstop)
{
	if ( (sel1_linenr<sel2_linenr) || ((sel1_linenr==sel2_linenr)&&(sel1_colnr<=sel2_colnr)) )
	{
		linenrstart=sel1_linenr;colnrstart=sel1_colnr;
		linenrstop=sel2_linenr;colnrstop=sel2_colnr;
	}
	else
	{
		linenrstart=sel2_linenr;colnrstart=sel2_colnr;
		linenrstop=sel1_linenr;colnrstop=sel1_colnr;
	}
	colnrstop--;
	return (linenrstop>linenrstart)||(colnrstop>=colnrstart);
}

bool TSC_source::G_issel()
{
	int d1,d2,d3,d4;
	return G_sel(d1,d2,d3,d4);
}


void TSC_source::cmd_mergeline(bool backup)
{
	if (!G_canmodify()) return;
	if ((cursor_linenr<0)||(cursor_linenr>=G_linecount()-1)) return;
	if (backup) addundo(new TSC_source_undoaction_mergeline(this));

	*lines[cursor_linenr]+=*lines[cursor_linenr+1];
	lines.del(cursor_linenr+1);

	modified=true;
}

void TSC_source::cmd_backspace(bool backup)
{
	if (!G_canmodify()) return;
	if ((cursor_linenr<0)||(cursor_linenr>=G_linecount())) return;

	if (G_issel())
	{
		cmd_cutselblock(backup);
		return;
	}

	if (cursor_colnr>0)
	{//delete character
		cursor_colnr--;
		if (backup) addundo(new TSC_source_undoaction_del(this,1));
		QString strl=*lines[cursor_linenr];
		strl.substring(0,cursor_colnr-1);
		QString strr=*lines[cursor_linenr];
		strr.substring(cursor_colnr+1,strr.G_length());
		strl+=strr;
		*lines[cursor_linenr]=strl;
		modified=true;
	}
	else
	{
		if (cursor_linenr>0)
		{
			cursor_linenr--;cursor_colnr=lines[cursor_linenr]->G_length();
			cmd_mergeline(backup);
		}
	}
}

void TSC_source::cmd_insertline(bool autoindent, bool backup)
{
	if (!G_canmodify()) return;
	if ((cursor_linenr<0)||(cursor_linenr>=G_linecount())) return;

	if (autoindent&&backup) undo_begin();

	if (backup) addundo(new TSC_source_undoaction_insertline(this));

	QString strl=*lines[cursor_linenr];
	strl.substring(0,cursor_colnr-1);
	QString strr=*lines[cursor_linenr];
	strr.substring(cursor_colnr,strr.G_length());
	*lines[cursor_linenr]=strl;
	lines.insertbefore(cursor_linenr+1,new QString(strr));
	cursor_linenr++;cursor_colnr=0;

	if (autoindent)
	{
		QString indent;
		for (int i=0; (i<G_line(cursor_linenr-1)->G_length())&&(G_line(cursor_linenr-1)->G_char(i)==' '); i++)
			indent+=' ';
		if (indent.G_length()>0) insertstring(indent,backup);
	}

	if (autoindent&&backup) undo_end();

	modified=true;
}

void TSC_source::cmd_cutselblock(bool backup)
{
	int selline1,selcol1,selline2,selcol2;
	if (!G_sel(selline1,selcol1,selline2,selcol2)) return;
	if (backup) undo_begin();
	if (selline1==selline2)
	{
		cursor_linenr=selline1;cursor_colnr=selcol1;
		cmd_del(selcol2-selcol1+1);
	}
	else
	{
		cursor_linenr=selline1;cursor_colnr=selcol1;
		if (cursor_colnr<G_line(selline1)->G_length()) cmd_del(G_line(selline1)->G_length()-cursor_colnr,backup);
		while (selline2>selline1+1)
		{
			cmd_mergeline(backup);
			if (cursor_colnr<G_line(selline1)->G_length()) cmd_del(G_line(selline1)->G_length()-cursor_colnr,backup);
			selline2--;
		}
		cursor_linenr=selline2;cursor_colnr=0;
		if (selcol2>=0) cmd_del(selcol2+1,backup);
		Set_cursor(selline1,selcol1);
		cmd_mergeline(backup);
	}
	Set_sel(cursor_linenr,cursor_colnr,cursor_linenr,cursor_colnr);
	if (backup) undo_end();
}


void TSC_source::changecursorpos(int incrline, int incrcol, bool resetsel)
{
	if (G_linecount()<=0) return;
	if ((incrline<0)&&(cursor_linenr>0)) Set_cursor(max(0,cursor_linenr+incrline),cursor_colnr,resetsel);
	if ((incrline>0)&&(cursor_linenr<G_linecount()-1)) Set_cursor(min(cursor_linenr+incrline,G_linecount()-1),cursor_colnr,resetsel);
	if (G_line(cursor_linenr)!=NULL)
	{
		if (incrcol>0)
		{
			if (cursor_colnr<G_line(cursor_linenr)->G_length()) Set_cursor(cursor_linenr,cursor_colnr+1,resetsel); 
			else if (cursor_linenr<G_linecount()-1) Set_cursor(cursor_linenr+1,0,resetsel); 
		}
		if (incrcol<0)
		{
			if (cursor_colnr>0) Set_cursor(cursor_linenr,cursor_colnr-1,resetsel); 
			else if (cursor_linenr>0) Set_cursor(cursor_linenr-1,G_line(cursor_linenr-1)->G_length(),resetsel); 
		}
	}
}

void TSC_source::jumpcursorpos(int tpe, bool resetsel)
{
	if (G_linecount()<=0) return;
	if (G_line(cursor_linenr)!=NULL)
	{
		if (tpe==-1) Set_cursor(cursor_linenr,0,resetsel);
		if (tpe==+1) Set_cursor(cursor_linenr,G_line(cursor_linenr)->G_length(),resetsel);
		if (tpe==-2) Set_cursor(0,0,resetsel);
		if (tpe==+2) Set_cursor(G_linecount()-1,G_line(G_linecount()-1)->G_length(),resetsel);
	}
}



void TSC_source::cmd_del(int cnt, bool backup)
{
	if (!G_canmodify()) return;
	if ((cursor_linenr<0)||(cursor_linenr>=G_linecount())) return;


	if (cursor_colnr<lines[cursor_linenr]->G_length())
	{//delete character
		if (cursor_colnr+cnt>G_line(cursor_linenr)->G_length()) cnt=G_line(cursor_linenr)->G_length()-cursor_colnr;
		if (cnt<=0) return;
		if (backup) addundo(new TSC_source_undoaction_del(this,cnt));
		QString strl=*lines[cursor_linenr];
		strl.substring(0,cursor_colnr-1);
		QString strr=*lines[cursor_linenr];
		strr.substring(cursor_colnr+cnt,strr.G_length());
		strl+=strr;
		*lines[cursor_linenr]=strl;
	}
	else cmd_mergeline(backup);

	modified=true;
}

void TSC_source::cmd_tab(bool invert, bool backup)
{
	QString str;
	int selline1,selcol1,selline2,selcol2;
	if (!G_sel(selline1,selcol1,selline2,selcol2))
	{
		//try to indent
		int linenr=G_cursor_linenr();
		if ((linenr<0)||(linenr>=G_linecount())) return;
		int colnr=G_cursor_colnr();
		if ((colnr<G_line(linenr)->G_length())) return;
		if (backup) undo_begin();
		(*lines[linenr])+=_qstr("   ");
		if (backup) addundo(new TSC_source_undoaction_insertstring(this,_qstr("   ")));
		cursor_colnr+=3;
		if (backup) undo_end();
		modified=true;
		return;
	}

	//shift group block
	if (backup) undo_begin();
	int ct1=selline1;
	int ct2=selline2;
	if (selcol2<0) ct2=selline2-1;
	for (int i=ct1; i<=ct2; i++)
	{
		if (!invert)
		{
			cursor_linenr=i;cursor_colnr=0;
			str=_qstr(" ");
			str+=*lines[cursor_linenr];
			if (backup) addundo(new TSC_source_undoaction_insertstring(this,_qstr(" ")));
			*lines[cursor_linenr]=str;
		}
		else
		{
			cursor_linenr=i;cursor_colnr=0;
			str=*lines[cursor_linenr];
			if (str.G_char(0)==' ')
			{
				str.substring(1,str.G_length()-1);
				if (backup) addundo(new TSC_source_undoaction_del(this,1));
				*lines[cursor_linenr]=str;
			}
		}
	}
	if (backup) undo_end();
	modified=true;
}



void TSC_source::cmd_copy()
{
	int line1,col1,line2,col2;

	if (!G_sel(line1,col1,line2,col2)) return;

	QString str,st0;
	while ( (line1<line2) || ((line1==line2)&&(col1<=col2)) )
	{
		if (col1<G_line(line1)->G_length())
		{
			st0.clear();st0+=G_line(line1)->G_char(col1);
			str+=st0;
		}
		col1++;
		if (col1>=G_line(line1)->G_length())
		{
			line1++;col1=0;
			if (line1<=line2) str+=_qstr("\n");
		}
	}

	copyclipboard(str);
}

void TSC_source::cmd_paste()
{
	QString str;
	StrChar ch;

	if (!pasteclipboard(str)) return;

	if (G_issel()) cmd_cutselblock();

	undo_begin();

	QString istr;
	for (int i=0; i<str.G_length(); i++)
	{
		ch=str[i];
		if ((ch!=10)&&(ch!=13)) istr+=ch;
		if (ch==10)
		{
			if (istr.G_length()>0) { insertstring(istr); istr.clear(); }
			cmd_insertline(false);
		}
	}
	if (istr.G_length()>0) { insertstring(istr); istr.clear(); }

	undo_end();
}



void TSC_source::insertstring(StrPtr str, bool backup)
{
	if (!G_canmodify()) return;
	if ((cursor_linenr<0)||(cursor_linenr>=G_linecount())) return;

	if (G_issel()) cmd_cutselblock(backup);


	QString strl=*lines[cursor_linenr];
	strl.substring(0,cursor_colnr-1);
	QString strr=*lines[cursor_linenr];
	strr.substring(cursor_colnr,strr.G_length());

	if (backup) addundo(new TSC_source_undoaction_insertstring(this,str));

	strl+=str;strl+=strr;
	*lines[cursor_linenr]=strl;
	cursor_colnr+=qstrlen(str);

	Set_sel(cursor_linenr,cursor_colnr,cursor_linenr,cursor_colnr);

	modified=true;
}

void TSC_source::find(StrPtr istr)
{
	QString str;str=istr;str.ToUpper();

	if ((cursor_linenr<0)||(cursor_linenr>=G_linecount())) return;
	int testlinenr=cursor_linenr;
	int testcolnr=cursor_colnr;

	while (testlinenr<G_linecount())
	{
		if ((testcolnr>=0)&&(testcolnr<lines[testlinenr]->G_length()))
		{
			QString st2=*lines[testlinenr];
			st2.substring(testcolnr,st2.G_length());
			st2.ToUpper();
			int ps=st2.findfirst(str);
			if (ps>=0)
			{
				sel1_linenr=testlinenr;sel2_linenr=testlinenr;
				sel1_colnr=testcolnr+ps;
				sel2_colnr=sel1_colnr+qstrlen(str);
				cursor_linenr=testlinenr;
				cursor_colnr=sel2_colnr;
				return;
			}
		}
		testlinenr++;
		testcolnr=0;
	}
}


void TSC_source::addundo(TSC_source_undoaction *undo)
{
	while (undos.G_count()>undos_current+1) undos.del(undos.G_count()-1);
	bool wrap=false;
	if ((!undo_busy)&&(!undo->isstart)&&(!undo->isstop)) wrap=true;
	if (wrap) undo_begin();
	undos.add(undo);undos_current=undos.G_count()-1;
	if (wrap) undo_end();
	
}

void TSC_source::undos_reset()
{
	undos.reset();
	undos_current=-1;
	undo_busy=false;
}



bool TSC_source::G_canundo()
{
	return (undos_current>=0)&&(undos_current<undos.G_count());
}

bool TSC_source::G_canredo()
{
	return undos_current<undos.G_count()-1;
}


void TSC_source::undo_begin()
{
	undo_busy=true;
	addundo(new TSC_source_undoaction_start(cursor_linenr,cursor_colnr));
}

void TSC_source::undo_end()
{
	addundo(new TSC_source_undoaction_stop(cursor_linenr,cursor_colnr));
	undo_busy=false;
}



void TSC_source::cmd_undo()
{
	if (!G_canmodify()) return;
	if (!G_canundo()) return;
	ASSERT(undos[undos_current]->isstop);
	do {
		undos[undos_current]->undo();
		undos_current--;
	} while ((undos_current>=0)&&(!undos[undos_current]->isstart));
	ASSERT(undos[undos_current]->isstart);
	cursor_linenr=undos[undos_current]->cursor_linenr;
	cursor_colnr=undos[undos_current]->cursor_colnr;
	undos_current--;
	if (undos_current>=0) ASSERT(undos[undos_current]->isstop);
}

void TSC_source::cmd_redo()
{
	if (!G_canmodify()) return;
	if (!G_canredo()) return;
	if (undos_current>=0) ASSERT(undos[undos_current]->isstop);
	do {
		undos_current++;
		undos[undos_current]->redo();
	} while ((undos_current<undos.G_count()-1)&&(!undos[undos_current]->isstop));
	ASSERT(undos[undos_current]->isstop);
	cursor_linenr=undos[undos_current]->cursor_linenr;
	cursor_colnr=undos[undos_current]->cursor_colnr;
}
