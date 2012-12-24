
#include "stdafx.h"

#include "qxformattedstring.h"
#include "qxwin.h"
#include "qerror.h"


TQXFormattedString::TQXFormattedString()
{
	clear();
}

TQXFormattedString::TQXFormattedString(const TQXFormattedString &arg)
{
	copyfrom(arg);
}


void TQXFormattedString::clear()
{
	content.clear();
	components.reset();
	currentstate.reset();
	currentstate.startps=0;
	newcomponent();
}

void TQXFormattedString::operator=(const TQXFormattedString &arg)
{
	copyfrom(arg);
}

void TQXFormattedString::copyfrom(const TQXFormattedString &arg)
{
	components.reset();
	for (int i=0; i<arg.components.G_count(); i++)
		components.add(arg.components[i]);
	content=arg.content;
	currentstate=arg.currentstate;
}

TQXFormattedString TQXFormattedString::operator+(const TQXFormattedString &arg) const
{
	TQXFormattedString rs;

	rs.content=content;
	for (int i=0; i<components.G_count(); i++)
		rs.components.add(components[i]);

	for (int i=0; i<arg.components.G_count(); i++)
		rs.components.add(arg.components[i]);

	rs.content+=arg.content;
	rs.currentstate=arg.currentstate;

	return rs;
}


TQXFormattedStringState& TQXFormattedString::G_curcomponent()
{
	ASSERT(components.G_count()>0);
	if (components.G_count()<=0) throw QError(_text("Invalid formatted string component"));
	return components[components.G_count()-1];
}

void TQXFormattedString::newcomponent()
{
	currentstate.startps=content.G_length();
	components.add(currentstate);
}


void TQXFormattedString::append(StrPtr arg)
{
	content+=arg;
}

void TQXFormattedString::appendnewline()
{
	content+='\r';
}


void TQXFormattedString::makebold(bool newstatus)
{
	currentstate.bold=newstatus;
	newcomponent();
}

void TQXFormattedString::makeitalic(bool newstatus)
{
	currentstate.italic=newstatus;
	newcomponent();
}


void TQXFormattedString::ExportLaTex(QString &expstr)//export LaTex style
{
	QString str;
	expstr.clear();
	int nextps;
	for (int compnr=0; compnr<components.G_count(); compnr++)
	{
		nextps=content.G_length()-1;
		if (compnr<components.G_count()-1) nextps=components[compnr+1].startps-1;
		str=content;
		str.substring(components[compnr].startps,nextps);

		if (components[compnr].bold) expstr+=_qstr("\\textbf{");
		if (components[compnr].italic) expstr+=_qstr("\\textit{");

		expstr+=str;

		if (components[compnr].bold) expstr+=_qstr("}");
		if (components[compnr].italic) expstr+=_qstr("}");
	}
}


void TQXFormattedString::draw(CDC *dc, int x0, int y0)
{
	QString str;
	int nextps;
	for (int compnr=0; compnr<components.G_count(); compnr++)
	{
		dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_WINDOW));
		dc->SetTextColor(G_QXSys().G_color(TQXSys::CL_TEXT));
		if (components[compnr].bold)
		{
			dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_WINDOWHEADER));
			dc->SetTextColor(G_QXSys().G_color(TQXSys::CL_TEXT).IntensIncr(0.4));
		}
		if (components[compnr].italic)
		{
			dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_WINDOWITALIC));
		}
		if (components[compnr].gray)
		{
			dc->SetTextColor(2*G_QXSys().G_color(TQXSys::CL_TEXT)+G_QXSys().G_color(TQXSys::CL_WINDOWBACK1));
		}
		nextps=content.G_length()-1;
		if (compnr<components.G_count()-1) nextps=components[compnr+1].startps-1;
		str=content;
		str.substring(components[compnr].startps,nextps);
		dc->TextOut(x0,y0,str);
		x0+=dc->GetTextExtent(str).cx;
	}
}

void TQXFormattedString::drawmultiline(CDC *dc, int x0, int y0, int xlen)
{
	QString str;
	int strlen;

	int xp=0;
	int yp=0;
	int curpos=0;
	int curcomp=0;
	dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_WINDOW));
	while (curpos<content.G_length())
	{
		int curpos2=curpos;
		while ((curpos2<content.G_length())&&(content[curpos2]!=' ')&&(content[curpos2]!=13)) curpos2++;
		str=content;str.substring(curpos,curpos2);
		strlen=dc->GetTextExtent(str).cx;
		if (xp+strlen>xlen) { xp=0;yp+=20; }
		dc->TextOut(x0+xp,y0+yp,str);
		xp+=strlen;
		if ((curpos2<content.G_length())&&(content[curpos2]==13))
		{
			xp=0;yp+=20;
		}
		curpos=curpos2+1;
	}
}
