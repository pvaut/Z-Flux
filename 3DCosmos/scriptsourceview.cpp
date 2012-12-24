#include "stdafx.h"
#include "qstring.h"
#include "qxscrollbar.h"
#include "scriptsourceview.h"

#include "3DScene.h"


//*****************************************************************************************
// Tscriptsourceview
//*****************************************************************************************



Tscriptsourceview::Tscriptsourceview(StrPtr iname, Tmainwin *imainwin) : TQXview(iname)
{
	mainwin=imainwin;
	script=NULL;
	varview=NULL;
	pressingdown=false;
}

Tscriptsourceview::~Tscriptsourceview()
{
}

void Tscriptsourceview::Set_script(TSC_script *iscript)
{
	script=iscript;
	redraw();
	if (varview!=NULL) varview->redraw();
}


void Tscriptsourceview::PostCreate()
{
}


void Tscriptsourceview::updatescrollbars()
{
	if (G_scrollbar_vert()!=NULL)
	{
		int sizey=0;
		if (G_script()!=NULL) sizey=G_script()->G_source().G_linecount()*ysep;
		G_scrollbar_vert()->Set_totsize(sizey);
	}
	if (G_scrollbar_hor()!=NULL)
	{
		G_scrollbar_hor()->Set_totsize(7000);
	}
}

void Tscriptsourceview::tabchanged(TQXheadercomp_tab *tabcontrol, int prenr, int postnr)
{
	if (G_script()!=NULL) G_script()->G_source().scrollposy=G_scrollbar_vert()->G_curpos();
	TSC_script *script=mainwin->G_script(postnr);
	Set_script(script);
	update_postchange();
	if (script!=NULL)
		G_scrollbar_vert()->setscrollpos(script->G_source().scrollposy);
}





void Tscriptsourceview::SizeChanged()
{
}

void Tscriptsourceview::KeyPressed(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool shiftpressed=((GetKeyState(VK_SHIFT)>>1)!=0);
	bool controlpressed=((GetKeyState(VK_CONTROL)>>1)!=0);
	if (script==NULL) return;

	if ((nChar==VK_DELETE)&&(!controlpressed)&&(!shiftpressed))
	{
		if (script->G_source().G_issel()) script->G_source().cmd_cutselblock();
		else script->G_source().cmd_del(1);
		update_postchange();
	}
	if (nChar==VK_BACK)
	{
		script->G_source().cmd_backspace();
		update_postchange();
	}
	if (nChar==VK_RETURN)
	{
		script->G_source().cmd_insertline(true);
		update_postchange();
	}
	if (nChar==VK_HOME)
	{
		if (!controlpressed) script->G_source().jumpcursorpos(-1,!shiftpressed);
		else script->G_source().jumpcursorpos(-2,!shiftpressed);
		update_postchange();
	}
	if (nChar==VK_END)
	{
		if (!controlpressed) script->G_source().jumpcursorpos(+1,!shiftpressed);
		else script->G_source().jumpcursorpos(+2,!shiftpressed);
		update_postchange();
	}
	if (nChar==VK_RIGHT)
	{
		script->G_source().changecursorpos(0,+1,!shiftpressed);
		update_postchange();
	}
	if (nChar==VK_LEFT)
	{
		script->G_source().changecursorpos(0,-1,!shiftpressed);
		update_postchange();
	}
	if (nChar==VK_UP)
	{
		script->G_source().changecursorpos(-1,0,!shiftpressed);
		update_postchange();
	}
	if (nChar==VK_DOWN)
	{
		script->G_source().changecursorpos(+1,0,!shiftpressed);
		update_postchange();
	}
	if (nChar==VK_PRIOR)
	{
		script->G_source().changecursorpos(-8,0,!shiftpressed);
		update_postchange();
	}
	if (nChar==VK_NEXT)
	{
		script->G_source().changecursorpos(+8,0,!shiftpressed);
		update_postchange();
	}
	if (nChar==VK_TAB)
	{
		script->G_source().cmd_tab(shiftpressed);
		update_postchange();
	}

	if ((nChar==VK_INSERT)&&(controlpressed)&&(!shiftpressed)) CharPressed('C'-'A'+1,0,0);
	if ((nChar==VK_INSERT)&&(!controlpressed)&&(shiftpressed)) CharPressed('V'-'A'+1,0,0);
	if ((nChar==VK_DELETE)&&(!controlpressed)&&(shiftpressed)) CharPressed('X'-'A'+1,0,0);
}

void Tscriptsourceview::update_postchange()
{
	int px,py;
	redraw(true);
	G_cursorpos(px,py);
	scrollinview_vert(py,py+ysep);
	scrollinview_horz(px,px+30);
	updatescrollbars();
}

void Tscriptsourceview::insertstring(StrPtr content)
{
	if (script==NULL) return;
	script->G_source().insertstring(content);
	redraw(true);
}

void Tscriptsourceview::cmd_find(StrPtr str)
{
	if (script==NULL) return;
	script->G_source().find(str);
	scrollinside_sel();
	redraw(true);
}



void Tscriptsourceview::scrollinside_debugline()
{
	if (script==NULL) return;
	int linenr=script->G_source().G_debugline();
	if (linenr>=0)
		scrollinview_vert(linenr*ysep,linenr*ysep+ysep);
}

void Tscriptsourceview::scrollinside_sel()
{
	int line1,col1,line2,col2;
	if (script==NULL) return;
	if (!script->G_source().G_sel(line1,col1,line2,col2)) return;
	scrollinview_vert(line1*ysep,line2*ysep+ysep);

	scrollinview_horz(col1*xsep,col2*xsep);
}

void Tscriptsourceview::G_cursorpos(int &px, int &py)
{
	if (script==NULL) return;
	px=script->G_source().G_cursor_colnr()*xsep;
	py=script->G_source().G_cursor_linenr()*ysep;
}



void Tscriptsourceview::CharPressed(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (script!=NULL)
	{
		if (nChar=='Z'-'A'+1) { script->G_source().cmd_undo(); update_postchange(); }
		if (nChar=='Y'-'A'+1) { script->G_source().cmd_redo(); update_postchange(); }
		if (nChar=='C'-'A'+1) { script->G_source().cmd_copy(); }
		if (nChar=='V'-'A'+1) { script->G_source().cmd_paste(); update_postchange(); }
		if (nChar=='X'-'A'+1) { script->G_source().cmd_copy(); script->G_source().cmd_cutselblock(); update_postchange(); }
	}
	if ((nChar>27)&&(script!=NULL))
	{
		StrChar chr=nChar;
		QString str;str+=chr;
		script->G_source().insertstring(str);
		redraw();
	}
}


void Tscriptsourceview::pixel2textpos(int px, int py, int &linenr, int &colnr)
{
	if (script==NULL) return;
	TSC_source &source=script->G_source();
	if (source.G_linecount()<=0) return;

	linenr=(py+G_offsety())/ysep;
	colnr=(px-marginx+G_offsetx()+xsep/2)/xsep;
	if (linenr>=source.G_linecount()) linenr=source.G_linecount()-1;
	if (linenr<0) linenr=0;
	if (colnr>source.G_line(linenr)->G_length()) colnr=source.G_line(linenr)->G_length();
	if (colnr<0) colnr=0;
}


void Tscriptsourceview::MouseLButtonDown(UINT nFlags, CPoint point)
{
	int linenr,colnr;
	if (script==NULL) return;
	TSC_source &source=script->G_source();
	if (source.G_linecount()<=0) return;

	pixel2textpos(point.x,point.y,linenr,colnr);
	source.Set_cursor(linenr,colnr);
	pressingdown=true;
	SetCapture();
	redraw();
}

void Tscriptsourceview::MouseLButtonDblClk(UINT nFlags, CPoint point)
{
	if (script==NULL) return;
	TSC_source &source=script->G_source();
	if (source.G_linecount()<=0) return;
	source.cmd_selectblock();
	redraw();
}


void Tscriptsourceview::MouseLButtonUp(UINT nFlags, CPoint point)
{
	if (pressingdown)
	{
		pressingdown=false;
		ReleaseCapture();
	}
}


void Tscriptsourceview::MouseMove(UINT nFlags, CPoint point)
{
	int linenr,colnr;
	if (script==NULL) return;
	TSC_source &source=script->G_source();

	if (pressingdown)
	{
		pixel2textpos(point.x,point.y,linenr,colnr);
		source.Set_cursor(linenr,colnr,false);
		redraw();
		update_postchange();
	}
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_IBEAM));
}





void Tscriptsourceview::draw(CDC *dc, RECT &updaterect)
{
	int xp,yp,kwstart,kwend;
	StrChar ch;
	bool insel;
	QString kw;
	TQXColor curtextcol,keywordcol;
	bool inkeyword,instring,istextstring,incomment,textstarted;

	TQXColor backcolor=G_QXSys().G_color(TQXSys::CL_WINDOWBACK1);
	TQXColor textcolor=G_QXSys().G_color(TQXSys::CL_TEXT);
//	TQXColor selcolor(0.6f,0.8f,1.0f);
	TQXColor selcolor=G_QXSys().G_color(TQXSys::CL_WINDOWHIGHLIGHT);
	TQXColor stringcolor1(0,0.6f,0);
	TQXColor stringcolor2(0.3f,0.6f,0);
	TQXColor commentcolor(0.7f,0,0);
	TQXColor signcolor=2*backcolor+textcolor;
	dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_FIXED));
	dc->SetBkMode(TRANSPARENT);
//	dc->FillSolidRect(0-G_offsetx(),0,300,300,RGB(255,0,0));

	TQXColor debugbackcol=3*backcolor+TQXColor(0,1,0);

	stringcolor1=TQXColor(0,0.75,0.25)+textcolor;
	stringcolor2=TQXColor(0.5,1,0)+textcolor;
	commentcolor=TQXColor(1,0,0)+textcolor;



	if (script==NULL)
	{
		dc->FillSolidRect(&updaterect,G_QXSys().G_color(TQXSys::CL_WINDOWBACK2));
		return;
	}

	TSC_source &source=script->G_source();
	if (source.readonly)
	{
		textcolor=G_QXSys().G_color(TQXSys::CL_TEXT)+backcolor;
	}
	dc->SetTextColor(textcolor);

	int sel1line,sel1col,sel2line,sel2col;
	source.G_sel(sel1line,sel1col,sel2line,sel2col);


	if (Isactiveview())
	{
		yp=source.G_cursor_linenr()*ysep-G_offsety();
		xp=marginx+source.G_cursor_colnr()*xsep-G_offsetx();
		dc->FillSolidRect(xp-0,yp,1,ysep,textcolor);
	}

	for (int linenr=0; linenr<source.G_linecount(); linenr++)
	{
		yp=linenr*ysep-G_offsety();
		if ((yp>=updaterect.top-20)&&(yp<=updaterect.bottom+20))
		{
			if (linenr==source.G_debugline())
				dc->FillSolidRect(0,yp,10000,ysep,debugbackcol);
			const QString *line=source.G_line(linenr);
			dc->FillSolidRect(marginx-G_offsetx()-8,yp+ysep/2,4,1,signcolor);
			dc->FillSolidRect(marginx-G_offsetx()-6,yp+ysep/2-1,1,1,signcolor);
			dc->FillSolidRect(marginx-G_offsetx()-6,yp+ysep/2+1,1,1,signcolor);
			inkeyword=false;instring=false;incomment=false;istextstring=false;
			textstarted=false;
			for (int colnr=0; colnr<line->G_length(); colnr++)
			{
				curtextcol=textcolor;
				xp=marginx+colnr*xsep-G_offsetx();
				insel=true;
				if (linenr<sel1line) insel=false;
				if ((linenr==sel1line)&&(colnr<sel1col)) insel=false;
				if (linenr>sel2line) insel=false;
				if ((linenr==sel2line)&&(colnr>sel2col)) insel=false;
				if (insel) dc->FillSolidRect(xp,yp,xsep,ysep,selcolor);
				ch=line->G_char(colnr);
				if (ch!=' ') textstarted=true;
				if ((!instring)&&(ch=='#')/*&&(line->G_char(colnr+1)=='/')*/)
				{
					incomment=true;
				}
				if (incomment) curtextcol=commentcolor;
				if (instring)
				{
					curtextcol=stringcolor1;
					if (istextstring) curtextcol=stringcolor2;
				}
				if ((!incomment)&&(isquote(ch)))
				{
					if (!instring)
					{
						instring=true;istextstring=false;
						curtextcol=stringcolor1;
						if (isquotesingle(ch))
						{
							istextstring=true;
							curtextcol=stringcolor2;
						}
					}
					else
					{
						if ((colnr==0)||(line->G_char(colnr-1)!=ESCCHAR)) instring=false;
					}
				}
				if ((!instring)&&(!incomment))
				{
					if (Qisalphanumerical(ch))
					{
						if ((colnr==0)||(!Qisalphanumerical(line->G_char(colnr-1))))
						{
							kwstart=colnr;kwend=colnr;
							while ((kwend<line->G_length()-1)&&(Qisalphanumerical(line->G_char(kwend+1)))) kwend++;
							if (kwend>kwstart)
							{
								kw.clear();
								for (int kwct=kwstart; kwct<=kwend; kwct++) kw+=line->G_char(kwct);
								if (script->G_keyword(kw,keywordcol)) inkeyword=true;
							}
						}
					}
					else
					{
						inkeyword=false;
					}
					if (inkeyword) curtextcol=keywordcol;
				}
				if (source.readonly) dc->SetTextColor(2*curtextcol+backcolor);
				else dc->SetTextColor(curtextcol);
				dc->TextOut(xp,yp+4,&ch,1);
				if (ch==' ')
				{
					dc->FillSolidRect(xp+xsep/2,yp+ysep/2,1,1,signcolor);
					if ((!textstarted)&&(colnr%3==2))
						dc->FillSolidRect(xp+xsep/2,yp,1,ysep,signcolor);
				}
			}
		}
	}

}


//////////////////////////////////////////////////////////////////////////////////////////////////
// Tscriptvarview
//////////////////////////////////////////////////////////////////////////////////////////////////

Tscriptvarview::Tscriptvarview(StrPtr iname, Tscriptsourceview *iscriptwin) : TQXview(iname)
{
	scriptwin=iscriptwin;
}


void Tscriptvarview::draw(CDC *dc, RECT &updaterect)
{
	Tarray<QString> stack;

	Tcopyarray<TSC_script_var> varlist;
	Tintarray vardepthlist;
	TSC_script *script=scriptwin->G_script();
	if (script==NULL) return;
	script->exec_buildstack(stack);
	script->exec_buildvarlist(varlist,vardepthlist);

	dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_WINDOW));
	dc->SetBkMode(TRANSPARENT);
	dc->SetTextColor(G_QXSys().G_color(TQXSys::CL_TEXT));

	int yp=5-G_offsety();
	int xp=5-G_offsetx();

	for (int i=0; i<stack.G_count(); i++)
	{
		dc->TextOut(xp+150,yp,*stack[i]);
		yp+=20;
	}
	yp+=20;

	QString str,varname;
	for (int i=0; i<varlist.G_count(); i++)
	{
		TSC_script_var *var=varlist[i];
		varname.clear();
		for (int ct=0; ct<vardepthlist[i]; ct++) varname+=_qstr("»  ");
		varname=varname+var->G_name();
		dc->TextOut(xp,yp,varname,qstrlen(varname));
		TSC_value *val=var->G_value();
		TSC_datatype *datatype=val->G_datatype();
		str=_qstr("-?-");
		if (datatype!=NULL) str=datatype->G_name();
		if (val==NULL) dc->TextOut(xp+150,yp,_qstr("-"),1);
		else
		{
			if (val->G_isreference()) str+=_qstr("*");
			dc->TextOut(xp+150,yp,str);
			val->tostring(str);
			dc->TextOut(xp+250,yp,str);
		}
		yp+=20;
	}
}

void Tscriptvarview::SizeChanged()
{
}

void Tscriptvarview::updatescrollbars()
{
	Tcopyarray<TSC_script_var> varlist;
	Tintarray vardepthlist;
	TSC_script *script=scriptwin->G_script();
	if (script==NULL) return;
	script->exec_buildvarlist(varlist,vardepthlist);

	if (G_scrollbar_vert()!=NULL)
	{
		int sizey=0;
		sizey=varlist.G_count()*20;
		G_scrollbar_vert()->Set_totsize(sizey);
	}
	if (G_scrollbar_hor()!=NULL)
	{
		G_scrollbar_hor()->Set_totsize(7000);
	}
}




//////////////////////////////////////////////////////////////////////////////////////////////////
// Toutputview
//////////////////////////////////////////////////////////////////////////////////////////////////

Toutputview::Toutputview(StrPtr iname, Tscriptsourceview *iscriptwin) : TQXview(iname)
{
	scriptwin=iscriptwin;
}


void Toutputview::draw(CDC *dc, RECT &updaterect)
{
	TSC_script *script=scriptwin->G_script();
	if (script==NULL) return;

	dc->SelectObject(G_QXSys().G_font(TQXSys::FNT_WINDOW));
	dc->SetBkMode(TRANSPARENT);
	dc->SetTextColor(G_QXSys().G_color(TQXSys::CL_TEXT));

	int yp=5-G_offsety();
	int xp=5-G_offsetx();
	QString str,varname;
	for (int i=0; i<script->G_output_linecount(); i++)
	{
		dc->TextOut(xp,yp,script->G_output_line(i),qstrlen(script->G_output_line(i)));
		yp+=20;
	}
}


void Toutputview::SizeChanged()
{
	updatescrollbars();
}


void Toutputview::updatescrollbars()
{
	TSC_script *script=scriptwin->G_script();
	if (script==NULL) return;

	if (G_scrollbar_vert()!=NULL)
	{
		int sizey=0;
		sizey=script->G_output_linecount()*20;
		G_scrollbar_vert()->Set_totsize(sizey);
	}
}
