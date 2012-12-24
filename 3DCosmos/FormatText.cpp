#include "stdafx.h"

#include "formattext.h"
#include "formattextequation.h"
#include "formattexttabular.h"

#include "qerror.h"
#include "qstringformat.h"

#include "qxwin.h"



/////////////////////////////////////////////////////////////////////
// TxtHyperlinklist
/////////////////////////////////////////////////////////////////////

void TxtHyperlinklist::clear()
{
	posx.reset();posy.reset();sizex.reset();sizey.reset();
	ID.reset();
}

void TxtHyperlinklist::add(double iposx, double iposy, double isizex, double isizey, StrPtr iID)
{
	posx.add(iposx);posy.add(iposy);sizex.add(isizex);sizey.add(isizey);
	ID.add(new QString(iID));
}

StrPtr TxtHyperlinklist::G_linkat(double psx, double psy) const
{
	for (int i=0; i<posx.G_count(); i++)
		if ((psx>=posx[i])&&(psy>=posy[i])&&(psx<=posx[i]+sizex[i])&&(psy<=posy[i]+sizey[i]))
			return *ID[i];
	return _qstr("");
}


/////////////////////////////////////////////////////////////////////
// TxtFont
/////////////////////////////////////////////////////////////////////

TxtFont::TxtFont(double isize)
{
	size=isize;
	fontstyle=FNTSTYLE_NORMAL;
	hyperlinkID.clear();
	colormixer=TQXColor(1,1,1,1,0);
}

TxtFont::TxtFont(const TxtFont &ifnt)
{
	copyfrom(ifnt);
}

void TxtFont::operator=(const TxtFont &ifnt)
{
	copyfrom(ifnt);
}

bool TxtFont::operator==(const TxtFont &f2) const
{
	if (size!=f2.size) return false;
	if (fontstyle!=f2.fontstyle) return false;
	return true;
}



void TxtFont::copyfrom(const TxtFont &ifnt)
{
	size=ifnt.size;
	fontstyle=ifnt.fontstyle;
	hyperlinkID=ifnt.hyperlinkID;
	colormixer=ifnt.colormixer;
}

void TxtFont::Set_hyperlink(StrPtr iID)
{
	hyperlinkID=iID;
}


////////////////////////////////////////////////////////
// TxtRenderer
////////////////////////////////////////////////////////

TxtRenderer::TxtRenderer()
{
	maxsizex=200;
}

TxtRenderer::~TxtRenderer()
{
}


////////////////////////////////////////////////////////////////
// TxtComponent
////////////////////////////////////////////////////////////////

TxtComponent::TxtComponent(TxtEnv *ienv, const TxtFont &ifnt)
: fnt(ifnt)
{
	env=ienv;
	startnewlinebefore=false;
	startnewlineafter=false;
	writesubscriptunder=false;
	comptpe=TXTCOMP_UNKNOWN;
	posx=0;posy=0;
	sizex=0;sizey=0;
	baseline=0;
}

TxtComponent::~TxtComponent()
{
}

void TxtComponent::calc(const TxtRenderer &renderer, double maxsizex)
{
	calc_implement(renderer,maxsizex);
}

void TxtComponent::render(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	renderer.render_boundingbox(xoffs,yoffs,sizex,sizey,baseline);
	render_impl(xoffs,yoffs,renderer);
}

void TxtComponent::buildhyperlinklist(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	buildhyperlinklist_impl(list,xoffs,yoffs,renderer);
}



///////////////////////////////////////////////////////////////////
// TxtWord
///////////////////////////////////////////////////////////////////

TxtWord::TxtWord(TxtEnv *ienv, const TxtFont &ifnt, StrPtr icontent)
: TxtComponent(ienv,ifnt)
{
	content=icontent;
}

void TxtWord::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	sizex=renderer.G_textsizex(fnt,content);
	sizey=renderer.G_textsizey(fnt,content);
	baseline=renderer.G_textbaseline(fnt,content);;
}

void TxtWord::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	renderer.showtext(fnt,xoffs,yoffs,content);
}

void TxtWord::buildhyperlinklist_impl(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	if (qstrlen(fnt.G_hyperlink())>0)
		list.add(xoffs,yoffs,sizex,sizey,fnt.G_hyperlink());
}


///////////////////////////////////////////////////////////////////
// TxtSpace
///////////////////////////////////////////////////////////////////

TxtSpace::TxtSpace(TxtEnv *ienv, const TxtFont &ifnt)
: TxtComponent(ienv,ifnt)
{
	comptpe=TXTCOMP_SPACE;
}

void TxtSpace::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	sizex=renderer.G_textsizex(fnt,_qstr(" "));
	sizey=renderer.G_textsizey(fnt,_qstr(" "));
	baseline=renderer.G_textbaseline(fnt,_qstr(" "));;
}

void TxtSpace::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	//renderer.render_horizontalline(xoffs,yoffs+5,sizex,1);
}

///////////////////////////////////////////////////////////////////
// TxtLineBreak
///////////////////////////////////////////////////////////////////

TxtLineBreak::TxtLineBreak(TxtEnv *ienv, const TxtFont &ifnt)
: TxtComponent(ienv,ifnt)
{
	startnewlinebefore=true;
}

void TxtLineBreak::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	sizex=0;sizey=0;
}

void TxtLineBreak::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
}


///////////////////////////////////////////////////////////////////
// TxtList
///////////////////////////////////////////////////////////////////

TxtList::TxtList(TxtEnv *ienv, const TxtFont &ifnt, StrPtr icontent, StrPtr ibulletstring)
: TxtComponent(ienv,ifnt), bulletfnt(ifnt)
{
	startnewlinebefore=true;
	startnewlineafter=true;
	bulletstring=ibulletstring;
	QString content;content=icontent;
	QString firstpart;
	content.splitstring(_qstr("\\item"),firstpart);
	while (content.G_length()>0)
	{
		eatstartspaces(content);
		splitblock(content,firstpart,_qstr("\\item"));
		TxtBlock *it=new TxtBlock(env,ifnt);items.add(it);
		it->parsefrom(firstpart);
		
	}
}

void TxtList::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	if (bulletstring.G_length()<=0)
	{
		bulletfnt.makesymbol();
		StrChar st[2];st[0]=0XB7;st[1]=0;
		bulletstring=st;
	}

	itembullets.reset();
	double marginx=0;
	for (int i=0; i<items.G_count(); i++)
	{
		QString locbulletstring=bulletstring;
		locbulletstring.replace(_qstr("$ct$"),TFormatString(_qstr("^1"),i+1));
		itembullets.add(new QString(locbulletstring));
		double lx=renderer.G_textsizex(bulletfnt,locbulletstring);
		if (lx>marginx) marginx=lx;
	}
	marginx+=renderer.G_fontsize(fnt);


	sizey=0;
	sizex=0;
	for (int i=0; i<items.G_count(); i++)
	{
		items[i]->calc(renderer,maxsizex-marginx);
		items[i]->Set_posx(marginx);
		items[i]->Set_posy(sizey);
		sizey+=items[i]->G_sizey();
		if (sizex<marginx+items[i]->G_sizex()) sizex=marginx+items[i]->G_sizex();
	}
}

void TxtList::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	for (int i=0; i<items.G_count(); i++)
	{
		renderer.showtext(bulletfnt,
			xoffs+renderer.G_fontsize(fnt)/4,
			yoffs+items[i]->G_posy()+items[i]->G_baseline()-renderer.G_textbaseline(fnt,*itembullets[i]),
			*itembullets[i]);
		items[i]->render(xoffs+items[i]->G_posx(),yoffs+items[i]->G_posy(),renderer);
	}
}

void TxtList::buildhyperlinklist_impl(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	for (int i=0; i<items.G_count(); i++)
		items[i]->buildhyperlinklist(list,xoffs+items[i]->G_posx(),yoffs+items[i]->G_posy(),renderer);
}




///////////////////////////////////////////////////////////////////
// TxtBlock
///////////////////////////////////////////////////////////////////


TxtBlock::TxtBlock(TxtEnv *ienv, const TxtFont &ifnt)
: TxtComponent(ienv,ifnt)
{
}


void eatstartspaces(QString &content)
{
	while ((content.G_length()>0)&&(content[0]==' ')) content.substring(1,content.G_length());
}

void eatendspaces(QString &content)
{
	while ((content.G_length()>0)&&(content[content.G_length()-1]==' ')) content.substring(0,content.G_length()-2);
}

double ConvertSize(StrPtr content, const TxtFont &ifnt)
{
	int slen=qstrlen(content);
	if ((slen>2)&&(content[slen-2]=='e')&&(content[slen-1]=='x'))
	{
		QString st=content;st.substring(0,slen-3);
		return qstr2int(st)*ifnt.G_size()*0.4;
	}
	throw QError(TFormatString(_text("Invalid size measure ^1"),content));
}


bool trykeyword(QString &content, StrPtr keyword)//determines if a string starts with a keyword, and splits if if true
{
	int kwlen=qstrlen(keyword);
	if (content.G_length()<kwlen) return false;
	if (qstrncmp(content,keyword,kwlen)!=0) return false;
	content.substring(kwlen,content.G_length());
	return true;
}

bool getargument(QString &content, QString &subblock, bool alwaysreturnblock)//splits a subblock at the first position from a string
{
	eatstartspaces(content);
	if (content.G_length()<=0) return false;
	if (content[0]!='{')
	{
		if (alwaysreturnblock)
		{
			StrChar st[2];st[0]=content[0];st[1]=0;
			subblock=st;
			content.substring(1,content.G_length());
			return true;
		}
		else return false;
	}
	content.substring(1,content.G_length());
	int i=0;
	int level=0;
	while ((i<content.G_length())&& (level>=0) )
	{
		i++;
		if (content[i]=='{') level++;
		if (content[i]=='}') level--;
	}
	subblock=content;subblock.substring(0,i-1);
	content.substring(i+1,content.G_length());
	return true;
}


void splitblock_bykeyword(QString &content, QString &subblock, StrPtr openingkeyword, StrPtr closingkeyword)
{
	if (content.G_length()<=0) return;
	int openingkeywordlen=qstrlen(openingkeyword);
	int closingkeywordlen=qstrlen(closingkeyword);
	int i=-1;
	int level=1;
	while ((i<content.G_length())&& (level>0) )
	{
		i++;
		if (qstrncmp(&content[i],openingkeyword,openingkeywordlen)==0) level++;
		if (qstrncmp(&content[i],closingkeyword,closingkeywordlen)==0) level--;
	}
	if (level>0)
		throw QError(TFormatString(_text("^1,^2 mismatch"),openingkeyword,closingkeyword));
	subblock=content;subblock.substring(0,i-1);
	content.substring(i+closingkeywordlen,content.G_length());
}


void splitblock(QString &content, QString &subblock, StrPtr token)
{
	int level=0;
	int tokenlen=qstrlen(token);
	for (int i=0; i<content.G_length(); i++)
	{
		if (qstrncmp(&content[i],_qstr("\\begin{"),7)==0) level++;
		if (qstrncmp(&content[i],_qstr("\\end{"),5)==0) level--;
		if ((level==0)&&(qstrncmp(&content[i],token,tokenlen)==0))
		{
			subblock=content;subblock.substring(0,i-1);
			content.substring(i+tokenlen,content.G_length());
			return;
		}
	}

	//not found!
	subblock=content;
	content.clear();
}


void TxtBlock::parsesub(const TxtFont &currentfnt, StrPtr icontent)
{
	QString content=icontent;
	eatstartspaces(content);
	QString word;
	while (content.G_length()>0)
	{
		bool processed=false;

		if ((!processed)&&(trykeyword(content,_qstr("\\textbf"))))
		{
			QString subblock;
			if (getargument(content,subblock,false))
			{
				TxtFont newfont(currentfnt);newfont.makebold();
				parsesub(newfont,subblock);
				processed=true;
			}
		}

		if ((!processed)&&(trykeyword(content,_qstr("\\textit"))))
		{
			QString subblock;
			if (getargument(content,subblock,false))
			{
				TxtFont newfont(currentfnt);newfont.makeitalic();
				parsesub(newfont,subblock);
				processed=true;
			}
		}

		if ((!processed)&&(trykeyword(content,_qstr("\\texttt"))))
		{
			QString subblock;
			if (getargument(content,subblock,false))
			{
				TxtFont newfont(currentfnt);newfont.makefixed();
				parsesub(newfont,subblock);
				processed=true;
			}
		}


		if ((!processed)&&(trykeyword(content,_qstr("\\sourcecode"))))
		{
			QString subblock;
			if (getargument(content,subblock,false))
			{
				TxtFont newfont(currentfnt);newfont.makefixed();
				newfont.Set_color(TQXColor(0.8,0.4,0.2));
				parsesub(newfont,subblock);
				processed=true;
			}
		}


		if ((!processed)&&(trykeyword(content,_qstr("\\var"))))
		{
			QString subblock,subblock2;
			if (getargument(content,subblock,false))
			{
				TxtFont newfont(currentfnt);newfont.makeitalic();
				newfont.Set_color(TQXColor(0.9,0.5,0.9));
				subblock2+=subblock;
				parsesub(newfont,subblock2);
				processed=true;
			}
		}

		if ((!processed)&&(trykeyword(content,_qstr("\\emph"))))
		{
			QString subblock;
			if (getargument(content,subblock,false))
			{
				TxtFont newfont(currentfnt);newfont.makeitalic();newfont.makebold();
				parsesub(newfont,subblock);
				processed=true;
			}
		}

		if ((!processed)&&(trykeyword(content,_qstr("\\large"))))
		{
			QString subblock;
			if (getargument(content,subblock,false))
			{
				TxtFont newfont(currentfnt);newfont.changesize(1.2);
				parsesub(newfont,subblock);
				processed=true;
			}
		}

		if ((!processed)&&(trykeyword(content,_qstr("\\Large"))))
		{
			QString subblock;
			if (getargument(content,subblock,false))
			{
				TxtFont newfont(currentfnt);newfont.changesize(1.4);
				parsesub(newfont,subblock);
				processed=true;
			}
		}

		if ((!processed)&&(trykeyword(content,_qstr("\\LARGE"))))
		{
			QString subblock;
			if (getargument(content,subblock,false))
			{
				TxtFont newfont(currentfnt);newfont.changesize(1.8);
				parsesub(newfont,subblock);
				processed=true;
			}
		}

		if ((!processed)&&(trykeyword(content,_qstr("\\hyperlink"))))
		{
			QString linkid,linkcontent;
			if (!getargument(content,linkid,false)) throw QError(_text("No link id provided"));
			if (!getargument(content,linkcontent,false)) throw QError(_text("No link content provided"));
			TxtFont newfont(currentfnt);
			newfont.Set_hyperlink(linkid);
			parsesub(newfont,linkcontent);
			processed=true;
		}


		for (int custnr=0; custnr<env->G_customkeywordcount(); custnr++)
		{
			if ((!processed)&&(trykeyword(content,env->G_customkeyword(custnr))))
			{
				QString subblock,out;
				getargument(content,subblock,false);
				env->processcustomkeyword(env->G_customkeyword(custnr),subblock,out);
				if (out.G_length()>0) parsesub(currentfnt,out);
				processed=true;
			}
		}


		if ((!processed)&&trykeyword(content,_qstr("\\begin{tabular}")))
		{
			QString arg;
			getargument(content,arg,false);
			QString subcontent;
			splitblock_bykeyword(content,subcontent,_qstr("\\begin{tabular}"),_qstr("\\end{tabular}"));

			TxtTabular *tb=new TxtTabular(env,currentfnt);components.add(tb);
			tb->parsefrom(arg,subcontent);
			tb->makestandaloneblock();
			processed=true;
			eatstartspaces(content);
		}

		if ((!processed)&&(content[0]=='$'))//math equation, inline
		{
			int ps;
			for (ps=1; (ps<content.G_length())&&(content[ps]!='$'); ps++);
			QString subblock=content;subblock.substring(1,ps-1);
			TxtEquation *eq=new TxtEquation(env,currentfnt);components.add(eq);
			eq->parsefrom(subblock);
			content.substring(ps+1,content.G_length());
			processed=true;
		}

		if ((!processed)&&trykeyword(content,_qstr("\\begin{equation}")))//math equation, display stule
		{
			QString subcontent;
			splitblock_bykeyword(content,subcontent,_qstr("\\begin{equation}"),_qstr("\\end{equation}"));

			TxtEquation *eq=new TxtEquation(env,currentfnt);components.add(eq);
			eq->parsefrom(subcontent);
			eq->makestandaloneblock();
			processed=true;
			eatstartspaces(content);
		}


		if ((!processed)&&trykeyword(content,_qstr("\\ ")))
		{
			TxtSpace *ws=new TxtSpace(env,currentfnt);components.add(ws);
			processed=true;
			eatstartspaces(content);
		}


		if ((!processed)&&trykeyword(content,_qstr("\\\\")))
		{
			TxtLineBreak *cm=new TxtLineBreak(env,currentfnt);components.add(cm);
			processed=true;
			eatstartspaces(content);
		}

		if ((!processed)&&trykeyword(content,_qstr("\\begin{itemize}")))
		{
			QString subcontent;
			splitblock_bykeyword(content,subcontent,_qstr("\\begin{itemize}"),_qstr("\\end{itemize}"));
			TxtList *ls=new TxtList(env,currentfnt,subcontent,_qstr(""));
			components.add(ls);
			processed=true;
			eatstartspaces(content);
		}

		if ((!processed)&&trykeyword(content,_qstr("\\begin{enumerate}")))
		{
			QString subcontent;
			splitblock_bykeyword(content,subcontent,_qstr("\\begin{enumerate}"),_qstr("\\end{enumerate}"));
			TxtList *ls=new TxtList(env,currentfnt,subcontent,_qstr("($ct$)"));
			components.add(ls);
			processed=true;
			eatstartspaces(content);
		}

		if ((!processed)&&trykeyword(content,_qstr("\\begin{list}")))
		{
			QString subcontent,arg1;
			getargument(content,arg1,false);
			splitblock_bykeyword(content,subcontent,_qstr("\\begin{list}"),_qstr("\\end{list}"));
			TxtList *ls=new TxtList(env,currentfnt,subcontent,arg1);
			components.add(ls);
			processed=true;
			eatstartspaces(content);
		}


		if (!processed)
		{
			content.splitstring(_qstr(" "),word);
			TxtWord *wrd=new TxtWord(env,currentfnt,word);components.add(wrd);
			if (content.G_length()>0) components.add(new TxtSpace(env,currentfnt));
			processed=true;
		}

	}
}


void TxtBlock::parsefrom(StrPtr icontent)
{
	parsesub(fnt,icontent);
}


void TxtBlock::format_addline(const TxtRenderer &renderer, int i1, int i2, double yp, double &maxh)
{
	double baselineoffset=0;
	for (int i=i1; i<=i2; i++)
		if (baselineoffset<components[i]->G_baseline()) baselineoffset=components[i]->G_baseline();
	if (calcfirstline) baseline=baselineoffset;
	double xp=0;
	maxh=0.00001;
	for (int i=i1; i<=i2; i++)
	{
		components[i]->Set_posx(xp);
		components[i]->Set_posy(yp+baselineoffset-components[i]->G_baseline());
		xp+=components[i]->G_sizex();
		if (maxh<components[i]->G_posy()+components[i]->G_sizey()-yp)
			maxh=components[i]->G_posy()+components[i]->G_sizey()-yp;
	}
	maxh+=renderer.G_textsizey(fnt,_qstr(" "))/6;
	calcfirstline=false;
}


void TxtBlock::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	for (int i=0; i<components.G_count(); i++) components[i]->calc(renderer,maxsizex);
	int i1=0,i2=0;
	double xp=0,yp=0;
	double lineh;
	sizex=1;
	calcfirstline=true;
	while (i2<components.G_count())
	{
		if ( ((xp+components[i2]->G_sizex()>maxsizex)&&(components[i2]->G_type()!=TXTCOMP_SPACE)) || 
			 (components[i2]->G_startnewlinebefore()) ||
			 ((i2>0)&&(components[i2-1]->G_startnewlineafter()))   )
		{
			format_addline(renderer,i1,i2-1,yp,lineh);
			yp+=lineh;
			xp=0;
			sizex=maxsizex;
			i1=i2;
		}
		xp+=components[i2]->G_sizex();
		i2++;
	}
	if (i1<components.G_count())
	{
		format_addline(renderer,i1,components.G_count()-1,yp,lineh);
		yp+=lineh;
	}
	if (xp>sizex) sizex=xp;
	sizey=yp;
}

void TxtBlock::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	for (int i=0; i<components.G_count(); i++)
		components[i]->render(xoffs+components[i]->G_posx(),yoffs+components[i]->G_posy(),renderer);
}

void TxtBlock::buildhyperlinklist_impl(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	for (int i=0; i<components.G_count(); i++)
		components[i]->buildhyperlinklist(list,xoffs+components[i]->G_posx(),yoffs+components[i]->G_posy(),renderer);
}


////////////////////////////////////////////////////////////////
// TxtDoc
////////////////////////////////////////////////////////////////


TxtDoc::TxtDoc(TxtEnv *ienv, const TxtFont &ifnt)
: TxtComponent(ienv,ifnt)
{
}

void TxtDoc::addparagraph(StrPtr icontent)
{
	TxtBlock *block=new TxtBlock(env,fnt);components.add(block);
	block->parsefrom(icontent);
}

void TxtDoc::parsefrom(StrPtr icontent)
{
	QString content=icontent;
	QString line,pcontent;
	components.reset();
	while (content.G_length()>0)
	{
		content.splitstring(_qstr("\n"),line);
		if (line.G_length()>0)
		{
			if (pcontent.G_length()>0) pcontent+=_qstr(" ");
			pcontent+=line;
		}
		else
		{
			addparagraph(pcontent);
			pcontent.clear();
		}
	}
	if (pcontent.G_length()>0) addparagraph(pcontent);
}


void TxtDoc::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	sizex=1;sizey=0;
	for (int i=0; i<components.G_count(); i++)
	{
		components[i]->calc(renderer,maxsizex);
		components[i]->Set_posy(sizey);
		sizey+=components[i]->G_sizey();
		if (i<components.G_count()-1)
			sizey+=renderer.G_textsizey(fnt,_qstr(" "))/2;
	}
}

void TxtDoc::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	for (int i=0; i<components.G_count(); i++)
		components[i]->render(xoffs+components[i]->G_posx(),yoffs+components[i]->G_posy(),renderer);
}

void TxtDoc::buildhyperlinklist_impl(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	for (int i=0; i<components.G_count(); i++)
		components[i]->buildhyperlinklist(list,xoffs+components[i]->G_posx(),yoffs+components[i]->G_posy(),renderer);
}



////////////////////////////////////////////////////////////////
// TxtEnv
////////////////////////////////////////////////////////////////

TxtEnv::TxtEnv(double ifontsize)
{
	doc=new TxtDoc(this,TxtFont(ifontsize));
}

TxtEnv::~TxtEnv()
{
	delete doc;
}

void TxtEnv::parsefrom(StrPtr icontent)
{
	doc->parsefrom(icontent);
}

void TxtEnv::calc(const TxtRenderer &renderer, double maxsizex)
{
	doc->calc(renderer,maxsizex);
}


void TxtEnv::render(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	doc->render(xoffs,yoffs,renderer);
}

void TxtEnv::buildhyperlinklist(TxtHyperlinklist &list, double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	doc->buildhyperlinklist(list,xoffs,yoffs,renderer);
}

double TxtEnv::G_sizey() const
{
	return doc->G_sizey();
}



void TxtEnv::addcustomkeyword(StrPtr ikeyword)
{
	customkeywords.add(new QString(ikeyword));
}

int TxtEnv::G_customkeywordcount() const
{
	return customkeywords.G_count();
}

StrPtr TxtEnv::G_customkeyword(int nr) const
{
	return *customkeywords[nr];
}



////////////////////////////////////////////////////////////////
// TxtRendererGDI
////////////////////////////////////////////////////////////////


TxtRendererGDI::TxtRendererGDI(CDC *idc)
{
	dc=idc;
}


CFont& TxtRendererGDI::G_font(const TxtFont &fntinfo) const
{
	for (int i=0; i<fontinfos.G_count(); i++)
		if (fntinfo==*fontinfos[i]) return *fonts[i];

	CFont *fnt=new CFont;

	double fontsize=fntinfo.G_size();

	int fontweight=FW_NORMAL;
	if (fntinfo.G_fontstyle()==TxtFont::FNTSTYLE_BOLD) fontweight=FW_BOLD;

	int italic=0;
	if (fntinfo.G_fontstyle()==TxtFont::FNTSTYLE_ITALIC) italic=1;

	//QString fontname=_qstr("Times new roman");
	//QString fontname=_qstr("Verdana");
	QString fontname=_qstr("Lucida");
	if (fntinfo.G_fontstyle()==TxtFont::FNTSTYLE_SYMBOL) fontname=_qstr("Symbol");

	if (fntinfo.G_fontstyle()==TxtFont::FNTSTYLE_FIXED)
	{
		fontname=_qstr("Lucida console");
		//fontsize*=1.1;
	}

	fnt->CreateFont(-(int)(0.5+fontsize),0,0,0,fontweight,italic,0,0,
	   DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_TT_ALWAYS,PROOF_QUALITY,
	   FF_DONTCARE|DEFAULT_PITCH,fontname);

	fontinfos.add(new TxtFont(fntinfo));
	fonts.add(fnt);
	return *fnt;
}

void TxtRendererGDI::selectfont(const TxtFont &fntinfo) const
{
	dc->SelectObject(G_font(fntinfo));
}


double TxtRendererGDI::G_textsizex(const TxtFont &fnt, StrPtr itxt) const
{
	selectfont(fnt);
	return dc->GetTextExtent(itxt,qstrlen(itxt)).cx;
}

double TxtRendererGDI::G_textsizey(const TxtFont &fnt, StrPtr itxt) const
{
	TEXTMETRIC mt; 
	selectfont(fnt);
	dc->GetTextMetrics(&mt);
	return mt.tmHeight;
}

double TxtRendererGDI::G_fontsize(const TxtFont &fnt) const
{
	return fnt.G_size();
}


double TxtRendererGDI::G_textbaseline(const TxtFont &fnt, StrPtr itxt) const
{
	TEXTMETRIC mt; 
	selectfont(fnt);
	dc->GetTextMetrics(&mt);
	return mt.tmAscent;
}


TQXColor TxtRendererGDI::G_textcolor() const
{
	return G_QXSys().G_color(TQXSys::CL_TEXT);
}

TQXColor TxtRendererGDI::G_linkcolor() const
{
	TQXColor cl=1.5*G_QXSys().G_color(TQXSys::CL_WINDOWHIGHLIGHT).IntensIncr(0.3)+G_QXSys().G_color(TQXSys::CL_TEXT);
	return cl.norm();
}


void TxtRendererGDI::showtext(const TxtFont &fnt, double x0, double y0, StrPtr itxt) const
{
	TQXColor cl;
	cl=G_textcolor();
	if (qstrlen(fnt.G_hyperlink())>0)
		cl=G_linkcolor();

	cl=cl+fnt.G_color();
	dc->SetTextColor(cl);
	selectfont(fnt);
	dc->SetTextAlign(TA_LEFT|TA_BASELINE);
	dc->SetBkMode(TRANSPARENT);
	dc->TextOut((int)(0.5+x0),(int)(0.5+y0+G_textbaseline(fnt,itxt)),itxt,qstrlen(itxt));
}

void TxtRendererGDI::render_horizontalline(double x0, double y0, double xlen, double width) const
{
	int lw=(int)(0.5+width);
	dc->FillSolidRect((int)(0.5+x0),(int)(0.5+y0-lw/2),(int)(0.5+xlen),int(0.5+lw),G_textcolor());
}

void TxtRendererGDI::render_verticalline(double x0, double y0, double ylen, double width) const
{
	int lw=(int)(0.5+width);
	dc->FillSolidRect((int)(0.5+x0-lw/2),(int)(0.5+y0),(int)(0.5+lw),int(0.5+ylen),G_textcolor());
}


void TxtRendererGDI::render_horizontalarrow(double x0, double y0, double xlen, double width) const
{
	int x0i=(int)(0.5+x0);
	int y0i=(int)(0.5+y0);

	int lw=(int)(0.5+width);
	int xleni=(int)(0.5+xlen);
	int arsizex=(int)(0.5+3*width);
	int arsizey=(int)(0.5+1.5*width);

	if (arsizey<2) arsizey=2;

	dc->FillSolidRect(x0i,y0i-lw/2,xleni-arsizex,lw,G_textcolor());

	POINT pts[3];
	pts[0].x=x0i+xleni;pts[0].y=y0i;
	pts[1].x=x0i+xleni-arsizex;pts[1].y=y0i+arsizey;
	pts[2].x=x0i+xleni-arsizex;pts[2].y=y0i-arsizey;
	dc->SelectObject(GetStockObject(NULL_PEN));
	CBrush br(G_textcolor());
	dc->SelectObject(br);
	dc->Polygon(pts,3);
}


void TxtRendererGDI::render_polyline(Tdoublearray &px, Tdoublearray &py, double width) const
{
	if (px.G_count()<2) return;
	CPen pn(PS_SOLID,(int)(0.5+width),G_textcolor());
	dc->SelectObject(pn);
	dc->MoveTo((int)(0.5+px[0]),(int)(0.5+py[0]));
	for (int i=1; i<px.G_count(); i++)
		dc->LineTo((int)(0.5+px[i]),(int)(0.5+py[i]));
}


void TxtRendererGDI::render_boundingbox(double x0, double y0, double sizex, double sizey, double baseline) const
{
	return;

	COLORREF cl=RGB(230,230,255);
	int ix0=(int)(0.5+x0);
	int iy0=(int)(0.5+y0);
	int ix1=(int)(0.5+x0+sizex);
	int iy1=(int)(0.5+y0+sizey);
	int iyb=(int)(0.5+y0+baseline);

	dc->FillSolidRect(ix0,iy0,ix1-ix0,1,cl);
	dc->FillSolidRect(ix0,iy1,ix1-ix0,1,cl);
	dc->FillSolidRect(ix0,iy0,1,iy1-iy0,cl);
	dc->FillSolidRect(ix1,iy0,1,iy1-iy0,cl);

	dc->FillSolidRect(ix0,iyb,ix1-ix0,1,RGB(255,220,220));

}
