#include "stdafx.h"

#include "qerror.h"
#include "qstringformat.h"

#include "formattextequation.h"


// TxtEquationIndex

TxtEquationIndex::TxtEquationIndex(TxtEnv *ienv, const TxtFont &ifnt)
: TxtComponent(ienv,ifnt)
{
	comptpe=TXTCOMP_INDEX;
	base=NULL;sub=NULL;super=NULL;
}

TxtEquationIndex::~TxtEquationIndex()
{
	if (base!=NULL) delete base;
	if (sub!=NULL) delete sub;
	if (super!=NULL) delete super;
}

void TxtEquationIndex::Set_base(TxtComponent *icomp)
{
	if (base!=NULL) throw QError(_text("Index: base component already defined"));
	base=icomp;
}
void TxtEquationIndex::Set_sub(TxtComponent *icomp)
{
	if (sub!=NULL) throw QError(_text("Index: sub component already defined"));
	sub=icomp;
}
void TxtEquationIndex::Set_super(TxtComponent *icomp)
{
	if (super!=NULL) throw QError(_text("Index: super component already defined"));
	super=icomp;
}


void TxtEquationIndex::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	if (base==NULL) throw QError(_text("Invalid index component: base is not defined"));
	base->calc(renderer,maxsizex);
	if (sub!=NULL) sub->calc(renderer,maxsizex);
	if (super!=NULL) super->calc(renderer,maxsizex);

	if (!base->G_writesubscriptunder())
	{
		double idxlen=0;
		if ((sub!=NULL)&&(sub->G_sizex()>idxlen)) idxlen=sub->G_sizex();
		if ((super!=NULL)&&(super->G_sizex()>idxlen)) idxlen=super->G_sizex();

		double xsep=renderer.G_fontsize(fnt)/10;
		sizex=base->G_sizex()+idxlen+xsep;

		if (super!=NULL) 
		{
			super->Set_posx(base->G_sizex()+xsep);
			double psy=0.8*renderer.G_fontsize(super->G_fnt())-super->G_sizey();
			if (psy<0) { base->Set_posy(-psy); psy=0; }
			super->Set_posy(psy);
		}

		baseline=base->G_posy()+base->G_baseline();

		if (sub!=NULL)
		{
			sub->Set_posx(base->G_sizex()+xsep);
			double psy=base->G_posy()+base->G_baseline()-renderer.G_fontsize(sub->G_fnt())/3;
			sub->Set_posy(psy);
		}
		sizey=base->G_posy()+base->G_sizey();
		if (super!=NULL) sizey=max(sizey,super->G_posy()+super->G_sizey());
		if (sub!=NULL) sizey=max(sizey,sub->G_posy()+sub->G_sizey());
	}
	else
	{
		sizey=0;
		if (super!=NULL) sizey+=super->G_sizey();
		base->Set_posy(sizey);
		baseline=base->G_baseline()+sizey;
		sizey+=base->G_sizey();
		if (sub!=NULL) { sub->Set_posy(sizey);sizey+=sub->G_sizey(); }
		sizex=base->G_sizex();
		if ((super!=NULL)&&(sizex<super->G_sizex())) sizex=super->G_sizex();
		if ((sub!=NULL)&&(sizex<sub->G_sizex())) sizex=sub->G_sizex();
		base->Set_posx((sizex-base->G_sizex())/2);
		if (sub!=NULL) sub->Set_posx((sizex-sub->G_sizex())/2);
		if (super!=NULL) super->Set_posx((sizex-super->G_sizex())/2);
	}

}

void TxtEquationIndex::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	if (base==NULL) throw QError(_text("Invalid index component: base is not defined"));
	base->render(xoffs+base->G_posx(),yoffs+base->G_posy(),renderer);
	if (sub!=NULL) sub->render(xoffs+sub->G_posx(),yoffs+sub->G_posy(),renderer);
	if (super!=NULL) super->render(xoffs+super->G_posx(),yoffs+super->G_posy(),renderer);
}



/////////////////////////////////////////////////////////////////////////////////
// TxtEquationFrac
/////////////////////////////////////////////////////////////////////////////////

TxtEquationFrac::TxtEquationFrac(TxtEnv *ienv, const TxtFont &ifnt, TxtComponent *icomp1, TxtComponent *icomp2)
: TxtComponent(ienv,ifnt)
{
	comp1=icomp1;
	comp2=icomp2;
}

TxtEquationFrac::~TxtEquationFrac()
{
	delete comp1;
	delete comp2;
}

void TxtEquationFrac::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	comp1->calc(renderer,maxsizex);
	comp2->calc(renderer,maxsizex);

	sep=renderer.G_fontsize(fnt)/10;
	comp1->Set_posy(0);
	baseline=comp1->G_sizey()+2*sep;
	comp2->Set_posy(baseline);
	sizey=baseline+sep+comp2->G_sizey();

	sizex=2*sep+max(comp1->G_sizex(),comp2->G_sizex());

	comp1->Set_posx((sizex-comp1->G_sizex())/2);
	comp2->Set_posx((sizex-comp2->G_sizex())/2);
}

void TxtEquationFrac::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	comp1->render(xoffs+comp1->G_posx(),yoffs+comp1->G_posy(),renderer);
	comp2->render(xoffs+comp2->G_posx(),yoffs+comp2->G_posy(),renderer);

	renderer.render_horizontalline(xoffs+sep,yoffs+baseline-sep,sizex-2*sep,renderer.G_fontsize(fnt)/14);
}



TxtEquationMatrix::TxtEquationMatrix(TxtEnv *ienv, const TxtFont &ifnt, StrPtr icontent, StrPtr alignstring)
: TxtComponent(ienv,ifnt)
{
	for (int i=0; i<qstrlen(alignstring); i++)
	{
		if (alignstring[i]=='l') colaligns.add(-1);
		else if (alignstring[i]=='r') colaligns.add(+1);
		else colaligns.add(0);
	}

	QString content;content=icontent;
	while (content.G_length()>0)
	{
		QString linetxt;
		content.splitstring(_qstr("\\\\"),linetxt);
		lines.add(new Tarray<TxtComponent>);
		while (linetxt.G_length()>0)
		{
			QString celltxt;
			linetxt.splitstring(_qstr("&"),celltxt);
			TxtEquation *cell=new TxtEquation(env,fnt);
			cell->parsefrom(celltxt);
			lines[lines.G_count()-1]->add(cell);
		}
	}
}

TxtComponent* TxtEquationMatrix::G_cell(int linenr, int colnr)
{
	if (lines[linenr]==NULL) return NULL;
	return lines[linenr]->get(colnr);
}

const TxtComponent* TxtEquationMatrix::G_cell(int linenr, int colnr) const
{
	if (lines[linenr]==NULL) return NULL;
	return lines[linenr]->get(colnr);
}


void TxtEquationMatrix::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	Tdoublearray lineh,colw;
	int linecount=lines.G_count();


	int colcount=0;
	for (int linenr=0; linenr<linecount; linenr++)
		if (lines[linenr]->G_count()>colcount) colcount=lines[linenr]->G_count();

	for (int linenr=0; linenr<linecount; linenr++) lineh.add(0);
	for (int colnr=0; colnr<colcount; colnr++) colw.add(0);

	for (int linenr=0; linenr<linecount; linenr++)
	{
		if (lines[linenr]->G_count()>colcount) colcount=lines[linenr]->G_count();
		double maxlineh=0;
		for (int colnr=0; colnr<lines[linenr]->G_count(); colnr++)
		{
			G_cell(linenr,colnr)->calc(renderer,maxsizex);
			if (G_cell(linenr,colnr)->G_sizey()>lineh[linenr]) lineh[linenr]=G_cell(linenr,colnr)->G_sizey();
			if (G_cell(linenr,colnr)->G_sizex()>colw[colnr]) colw[colnr]=G_cell(linenr,colnr)->G_sizex();
		}
	}

	double spacing=renderer.G_fontsize(fnt)/2;

	Tdoublearray lineoffsy,coloffsx;
	sizey=0;
	for (int linenr=0; linenr<linecount; linenr++)
	{
		lineoffsy.add(sizey);
		sizey+=lineh[linenr]+spacing;
	}
	lineoffsy.add(sizey);
	sizex=0;
	for (int colnr=0; colnr<colcount; colnr++)
	{
		coloffsx.add(sizex);
		sizex+=colw[colnr]+spacing;
	}
	coloffsx.add(sizex);
	baseline=sizey/2;

	double xp=0;
	for (int linenr=0; linenr<linecount; linenr++)
		for (int colnr=0; colnr<lines[linenr]->G_count(); colnr++)
		{
			if (colaligns[colnr]==-1) xp=coloffsx[colnr]+spacing/2;
			if (colaligns[colnr]== 0) xp=(coloffsx[colnr]+coloffsx[colnr+1])/2-G_cell(linenr,colnr)->G_sizex()/2;
			if (colaligns[colnr]==+1) xp=coloffsx[colnr+1]-G_cell(linenr,colnr)->G_sizex()-spacing/2;
			G_cell(linenr,colnr)->Set_posx(xp);
			G_cell(linenr,colnr)->Set_posy((lineoffsy[linenr]+lineoffsy[linenr+1])/2-G_cell(linenr,colnr)->G_sizey()/2);
		}

	//sizey+=spacing;


}

void TxtEquationMatrix::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	for (int linenr=0; linenr<lines.G_count(); linenr++)
	{
		for (int colnr=0; colnr<lines[linenr]->G_count(); colnr++)
			G_cell(linenr,colnr)->render(xoffs+G_cell(linenr,colnr)->G_posx(),yoffs+G_cell(linenr,colnr)->G_posy(),renderer);
	}
}



/////////////////////////////////////////////////////////////////////////////////
// TxtEquationBracket
/////////////////////////////////////////////////////////////////////////////////

TxtEquationBracket::TxtEquationBracket(TxtEnv *ienv, const TxtFont &ifnt, TxtComponent *isubcomp, int itpe)
: TxtComponent(ienv,ifnt)
{
	subcomp=isubcomp;
	tpe=itpe;
}

TxtEquationBracket::~TxtEquationBracket()
{
	delete subcomp;
}


void TxtEquationBracket::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	subcomp->calc(renderer,maxsizex);
	sizey=subcomp->G_sizey();
	baseline=subcomp->G_baseline();
	bsizex=sizey/8;
	if (tpe==1) bsizex=sizey/4;
	if (tpe==2) bsizex=sizey/12;
	sizex=subcomp->G_sizex()+2*bsizex;
	subcomp->Set_posx(bsizex);
}

void TxtEquationBracket::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	subcomp->render(xoffs+subcomp->G_posx(),yoffs+subcomp->G_posy(),renderer);
	double linewidth=renderer.G_fontsize(fnt)/14;

	if (tpe==0)
	{
		{//left
			Tdoublearray px,py;
			for (int i=0; i<=10; i++)
			{
				double fr=i/10.0;
				py.add(yoffs+fr*sizey);
				px.add(xoffs+linewidth+(bsizex-linewidth)*(4*(fr-0.5)*(fr-0.5)));
			}
			renderer.render_polyline(px,py,linewidth);
		}
		{//right
			Tdoublearray px,py;
			for (int i=0; i<=10; i++)
			{
				double fr=i/10.0;
				py.add(yoffs+fr*sizey);
				px.add(xoffs+sizex-linewidth-(bsizex-linewidth)*(4*(fr-0.5)*(fr-0.5)));
			}
			renderer.render_polyline(px,py,linewidth);
		}
	}

	if (tpe==1)
	{
		{//left
			Tdoublearray px,py;
			px.add(xoffs+bsizex);  py.add(yoffs);
			px.add(xoffs+linewidth);  py.add(yoffs);
			px.add(xoffs+linewidth);  py.add(yoffs+sizey);
			px.add(xoffs+bsizex);  py.add(yoffs+sizey);
			renderer.render_polyline(px,py,linewidth);
		}
		{//right
			Tdoublearray px,py;
			px.add(xoffs+sizex-bsizex);  py.add(yoffs);
			px.add(xoffs+sizex-linewidth);  py.add(yoffs);
			px.add(xoffs+sizex-linewidth);  py.add(yoffs+sizey);
			px.add(xoffs+sizex-bsizex);  py.add(yoffs+sizey);
			renderer.render_polyline(px,py,linewidth);
		}
	}

	if (tpe==2)
	{
		{//left
			Tdoublearray px,py;
			px.add(xoffs+bsizex/2);  py.add(yoffs);
			px.add(xoffs+bsizex/2);  py.add(yoffs+sizey);
			renderer.render_polyline(px,py,linewidth);
		}
		{//right
			Tdoublearray px,py;
			px.add(xoffs+sizex-bsizex/2);  py.add(yoffs);
			px.add(xoffs+sizex-bsizex/2);  py.add(yoffs+sizey);
			renderer.render_polyline(px,py,linewidth);
		}
	}

}


/////////////////////////////////////////////////////////////////////////////////
// TxtEquationSqrt
/////////////////////////////////////////////////////////////////////////////////

TxtEquationSqrt::TxtEquationSqrt(TxtEnv *ienv, const TxtFont &ifnt, TxtComponent *isubcomp)
: TxtComponent(ienv,ifnt)
{
	subcomp=isubcomp;
}

TxtEquationSqrt::~TxtEquationSqrt()
{
	delete subcomp;
}


void TxtEquationSqrt::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	subcomp->calc(renderer,maxsizex);
	ysep=renderer.G_fontsize(fnt)/8;
	xsep=renderer.G_fontsize(fnt)*0.7;

	sizey=subcomp->G_sizey()+ysep;
	baseline=subcomp->G_baseline()+ysep;
	sizex=subcomp->G_sizex()+xsep;
	subcomp->Set_posy(ysep);
	subcomp->Set_posx(xsep);
}

void TxtEquationSqrt::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	subcomp->render(xoffs+subcomp->G_posx(),yoffs+subcomp->G_posy(),renderer);
	double linewidth=renderer.G_fontsize(fnt)/13;

	Tdoublearray px,py;
		
	px.add(xoffs+sizex);py.add(yoffs+linewidth);
	px.add(xoffs+xsep-linewidth);py.add(yoffs+linewidth);

	px.add(xoffs+xsep/2);py.add(yoffs+sizey*0.9);
	px.add(xoffs+linewidth);py.add(yoffs+sizey*0.4);

	renderer.render_polyline(px,py,linewidth);
}



/////////////////////////////////////////////////////////////////////////////////
// TxtEquationVector
/////////////////////////////////////////////////////////////////////////////////

TxtEquationVector::TxtEquationVector(TxtEnv *ienv, const TxtFont &ifnt, TxtComponent *isubcomp)
: TxtComponent(ienv,ifnt)
{
	subcomp=isubcomp;
}

TxtEquationVector::~TxtEquationVector()
{
	delete subcomp;
}


void TxtEquationVector::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	subcomp->calc(renderer,maxsizex);
	ysep=renderer.G_fontsize(fnt)/16;

	sizey=subcomp->G_sizey()+ysep;
	baseline=subcomp->G_baseline()+ysep;
	sizex=subcomp->G_sizex()+renderer.G_fontsize(fnt)/7;
	subcomp->Set_posy(ysep);
}

void TxtEquationVector::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	subcomp->render(xoffs+subcomp->G_posx(),yoffs+subcomp->G_posy(),renderer);

	double linewidth=renderer.G_fontsize(fnt)/14;

	Tdoublearray px,py;
		
	px.add(xoffs+sizex);py.add(yoffs+linewidth);
	px.add(xoffs);py.add(yoffs+linewidth);

	renderer.render_horizontalarrow(xoffs,yoffs+2*linewidth,sizex,linewidth);
}



/////////////////////////////////////////////////////////////////////////////////
// TxtEquationOverline
/////////////////////////////////////////////////////////////////////////////////

TxtEquationOverline::TxtEquationOverline(TxtEnv *ienv, const TxtFont &ifnt, TxtComponent *isubcomp)
: TxtComponent(ienv,ifnt)
{
	subcomp=isubcomp;
}

TxtEquationOverline::~TxtEquationOverline()
{
	delete subcomp;
}


void TxtEquationOverline::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	subcomp->calc(renderer,maxsizex);
	ysep=0;//renderer.G_fontsize(fnt)/20;

	sizey=subcomp->G_sizey()+ysep;
	baseline=subcomp->G_baseline()+ysep;
	sizex=subcomp->G_sizex()+renderer.G_fontsize(fnt)/7;
	subcomp->Set_posy(ysep);
}

void TxtEquationOverline::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	subcomp->render(xoffs+subcomp->G_posx(),yoffs+subcomp->G_posy(),renderer);

	double linewidth=renderer.G_fontsize(fnt)/14;

	renderer.render_horizontalline(xoffs,yoffs+2*linewidth,sizex,linewidth);
}



///////////////////////////////////////////////////////////////////
// TxtEquationVar
///////////////////////////////////////////////////////////////////

TxtEquationVar::TxtEquationVar(TxtEnv *ienv, const TxtFont &ifnt, StrPtr icontent)
: TxtComponent(ienv,ifnt)
{
	fnt.makeitalic();
	content=icontent;
}

void TxtEquationVar::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	sizex=renderer.G_textsizex(fnt,content);
	sizey=renderer.G_textsizey(fnt,content);
	baseline=renderer.G_textbaseline(fnt,content);
}

void TxtEquationVar::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	renderer.showtext(fnt,xoffs,yoffs,content);
}



///////////////////////////////////////////////////////////////////
// TxtEquationChar
///////////////////////////////////////////////////////////////////

TxtEquationChar::TxtEquationChar(TxtEnv *ienv, const TxtFont &ifnt, StrPtr icontent, bool issymbol)
: TxtComponent(ienv,ifnt)
{
	if (issymbol) fnt.makesymbol();
	content=icontent;
}

void TxtEquationChar::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	sizex=renderer.G_textsizex(fnt,content);
	sizey=renderer.G_textsizey(fnt,content);
	baseline=renderer.G_textbaseline(fnt,content);
}

void TxtEquationChar::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	renderer.showtext(fnt,xoffs,yoffs,content);
}


///////////////////////////////////////////////////////////////////
// TxtEquationSum
///////////////////////////////////////////////////////////////////

TxtEquationSum::TxtEquationSum(TxtEnv *ienv, const TxtFont &ifnt, StrPtr isign)
: TxtComponent(ienv,ifnt)
{
	sign=isign;
	writesubscriptunder=true;
	fnt.makesymbol();
	fnt.changesize(2.5);
}

void TxtEquationSum::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	sizex=renderer.G_textsizex(fnt,sign);
	sizey=renderer.G_textsizey(fnt,sign)*0.6;
	baseline=renderer.G_textbaseline(fnt,sign)*0.55;
}

void TxtEquationSum::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	renderer.showtext(fnt,xoffs,yoffs-0.35*sizey,sign);
}


///////////////////////////////////////////////////////////////////
// TxtEquationIntegral
///////////////////////////////////////////////////////////////////

TxtEquationIntegral::TxtEquationIntegral(TxtEnv *ienv, const TxtFont &ifnt)
: TxtComponent(ienv,ifnt)
{
	StrChar st[2];st[0]=0xF2;st[1]=0;
	sign=st;
	writesubscriptunder=true;
	fnt.makesymbol();
	fnt.changesize(2.5);
}

void TxtEquationIntegral::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	sizex=renderer.G_textsizex(fnt,sign);
	sizey=renderer.G_textsizey(fnt,sign)*0.9;
	baseline=renderer.G_textbaseline(fnt,sign)*0.7;
}

void TxtEquationIntegral::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	renderer.showtext(fnt,xoffs,yoffs-0.05*sizey,sign);
}



////////////////////////////////////////////////////////////////////////
// TxtEquation
////////////////////////////////////////////////////////////////////////

TxtEquation::TxtEquation(TxtEnv *ienv, const TxtFont &ifnt)
: TxtComponent(ienv,ifnt)
{
	addtoken(_qstr("-"),_qstr("-"),true);
	addtoken(_qstr("+"),_qstr("+"),true);
	addtoken(_qstr("\\times"),_qstr("´"),true);

//	addtoken(_qstr("\\alpha"),_qstr("a"),true);
 
	addtoken(_qstr("\\Alpha"),	_qstr("A"),true);
	addtoken(_qstr("\\Beta"),	_qstr("B"),true);
	addtoken(_qstr("\\Gamma"),	_qstr("G"),true);
	addtoken(_qstr("\\Delta"),	_qstr("D"),true);
	addtoken(_qstr("\\Epsilon"),_qstr("E"),true);
	addtoken(_qstr("\\Zeta"),	_qstr("Z"),true);
	addtoken(_qstr("\\Eta"),	_qstr("H"),true);
	addtoken(_qstr("\\Theta"),	_qstr("Q"),true);
	addtoken(_qstr("\\Iota"),	_qstr("I"),true);
	addtoken(_qstr("\\Kappa"),	_qstr("K"),true);
	addtoken(_qstr("\\Lambda"),	_qstr("L"),true);
	addtoken(_qstr("\\Mu"),		_qstr("M"),true);
	addtoken(_qstr("\\Nu"),		_qstr("N"),true);
	addtoken(_qstr("\\Xi"),		_qstr("X"),true);
	addtoken(_qstr("\\Omicron"),_qstr("O"),true);
	addtoken(_qstr("\\Pi"),		_qstr("P"),true);
	addtoken(_qstr("\\Rho"),	_qstr("R"),true);
	addtoken(_qstr("\\Sigma"),	_qstr("S"),true);
	addtoken(_qstr("\\Tau"),	_qstr("T"),true);
	addtoken(_qstr("\\Upsilon"),_qstr("Y"),true);
	addtoken(_qstr("\\Phi"),	_qstr("J"),true);
	addtoken(_qstr("\\Chi"),	_qstr("C"),true);
	addtoken(_qstr("\\Psi"),	_qstr("Y"),true);
	addtoken(_qstr("\\Omega"),	_qstr("W"),true);

	addtoken(_qstr("\\alpha"),	_qstr("a"),true);
	addtoken(_qstr("\\beta"),	_qstr("b"),true);
	addtoken(_qstr("\\gamma"),	_qstr("g"),true);
	addtoken(_qstr("\\delta"),	_qstr("d"),true);
	addtoken(_qstr("\\epsilon"),_qstr("e"),true);
	addtoken(_qstr("\\zeta"),	_qstr("z"),true);
	addtoken(_qstr("\\eta"),	_qstr("h"),true);
	addtoken(_qstr("\\theta"),	_qstr("q"),true);
	addtoken(_qstr("\\iota"),	_qstr("i"),true);
	addtoken(_qstr("\\kappa"),	_qstr("k"),true);
	addtoken(_qstr("\\lambda"),	_qstr("l"),true);
	addtoken(_qstr("\\mu"),		_qstr("m"),true);
	addtoken(_qstr("\\nu"),		_qstr("n"),true);
	addtoken(_qstr("\\xi"),		_qstr("x"),true);
	addtoken(_qstr("\\omicron"),_qstr("o"),true);
	addtoken(_qstr("\\pi"),		_qstr("p"),true);
	addtoken(_qstr("\\rho"),	_qstr("r"),true);
	addtoken(_qstr("\\sigma"),	_qstr("s"),true);
	addtoken(_qstr("\\tau"),	_qstr("t"),true);
	addtoken(_qstr("\\upsilon"),_qstr("y"),true);
	addtoken(_qstr("\\phi"),	_qstr("j"),true);
	addtoken(_qstr("\\chi"),	_qstr("c"),true);
	addtoken(_qstr("\\psi"),	_qstr("y"),true);
	addtoken(_qstr("\\omega"),	_qstr("w"),true);

	addtoken(_qstr("\\varpi"),	_qstr("v"),true);


	addtoken(_qstr("\\sin"),_qstr("sin"),false);
	addtoken(_qstr("\\cos"),_qstr("cos"),false);
	addtoken(_qstr("\\exp"),_qstr("exp"),false);
	addtoken(_qstr("\\log"),_qstr("log"),false);
	addtoken(_qstr("\\ln"),_qstr("ln"),false);

	addtoken(_qstr("\\,"),_qstr(" "),false);
	addtoken(_qstr("\\;"),_qstr("  "),false);
		

	addtoken(_qstr("\\pm"),_qstr("±"),false);
	addtoken(_qstr("\\cdot"),_qstr("·"),false);

	addtoken(_qstr("\\nabla"),	_qstr("Ñ"),true);
	addtoken(_qstr("\\infty"),	_qstr("µ"),true);
	addtoken(_qstr("\\partial"),_qstr("¶"),true);

	addtoken(_qstr("\\forall"),_qstr("\""),true);
	addtoken(_qstr("\\exists"),_qstr("$"),true);
	addtoken(_qstr("\\angle"),_qstr("Ð"),true);
	addtoken(_qstr("\\neg"),_qstr("Ø"),true);
	addtoken(_qstr("\\empty"),_qstr("Æ"),true);

	addtoken(_qstr("\\bullet"),_qstr("·"),true);
	addtoken(_qstr("\\cup"),_qstr("È"),true);
	addtoken(_qstr("\\cap"),_qstr("Ç"),true);
	addtoken(_qstr("\\leq"),_qstr("£"),true);
	addtoken(_qstr("\\geq"),_qstr("³"),true);
	addtoken(_qstr("\\neq"),_qstr("¹"),true);
	addtoken(_qstr("\\sim"),_qstr("~"),true);
	addtoken(_qstr("\\in"),_qstr("Î"),true);
	addtoken(_qstr("\\subseteq"),_qstr("Í"),true);
	addtoken(_qstr("\\subset"),_qstr("Ì"),true);
	addtoken(_qstr("\\cong"),_qstr("@"),true);
	addtoken(_qstr("\\approx"),_qstr("»"),true);

	addtoken(_qstr("\\rightarrow"),_qstr("®"),true);
	addtoken(_qstr("\\Rightarrow"),_qstr("Þ"),true);
	addtoken(_qstr("\\leftrightarrow"),_qstr("«"),true);
	addtoken(_qstr("\\Leftrightarrow"),_qstr("Û"),true);


}


void TxtEquation::addtoken(StrPtr token, StrPtr word, bool issymbol)
{
	tokens.add(new TxtEqToken(token,word,issymbol));
}


TxtEquationIndex* TxtEquation::create_indexcomponent()//wraps the last component in an index component
{
	if (components.G_count()<=0) throw QError(_text("Invalid index component: no component available"));
	TxtComponent *curcmp=components[components.G_count()-1];
	if (curcmp->G_type()==TXTCOMP_INDEX) return (TxtEquationIndex*)curcmp;
	TxtEquationIndex *idx=new TxtEquationIndex(env,fnt);
	components.set(components.G_count()-1,idx);
	idx->Set_base(curcmp);
	return idx;
}


void TxtEquation::parsesub(const TxtFont &currentfnt, StrPtr icontent)
{
	QString content=icontent;
	while (content.G_length()>0)
	{
		while (content[0]==' ') content.substring(1,content.G_length());

		bool processed=false;

		if (content[0]=='_')
		{
			QString subcontent;
			content.substring(1,content.G_length());
			if (getargument(content,subcontent,true))
			{
				TxtFont subfont(currentfnt);subfont.Set_size(0.75*currentfnt.G_size());
				TxtEquation *subcomp=new TxtEquation(env,subfont);
				subcomp->parsefrom(subcontent);
				create_indexcomponent()->Set_sub(subcomp);
				processed=true;
			}
		}

		if (content[0]=='^')
		{
			QString subcontent;
			content.substring(1,content.G_length());
			if (getargument(content,subcontent,true))
			{
				TxtFont subfont(currentfnt);subfont.Set_size(0.75*currentfnt.G_size());
				TxtEquation *subcomp=new TxtEquation(env,subfont);
				subcomp->parsefrom(subcontent);
				create_indexcomponent()->Set_super(subcomp);
				processed=true;
			}
		}

		if (trykeyword(content,_qstr("\\frac")))
		{
			QString subcontent1,subcontent2;
			if (!getargument(content,subcontent1,false)) throw QError(_text("Invalid frac syntax"));
			if (!getargument(content,subcontent2,false)) throw QError(_text("Invalid frac syntax"));
			TxtEquation *subcomp1=new TxtEquation(env,fnt);subcomp1->parsefrom(subcontent1);
			TxtEquation *subcomp2=new TxtEquation(env,fnt);subcomp2->parsefrom(subcontent2);
			TxtEquationFrac *frc=new TxtEquationFrac(env,fnt,subcomp1,subcomp2);components.add(frc);
			processed=true;
		}

		if (trykeyword(content,_qstr("\\begin{matrix}")))
		{
			QString subcontent;
			splitblock_bykeyword(content,subcontent,_qstr("\\begin{matrix}"),_qstr("\\end{matrix}"));
			components.add(new TxtEquationMatrix(env,currentfnt,subcontent,_qstr("")));
			processed=true;
		}

		if (trykeyword(content,_qstr("\\begin{array}")))
		{
			QString subcontent;
			QString alignstring;
			getargument(content,alignstring,false);
			splitblock_bykeyword(content,subcontent,_qstr("\\begin{array}"),_qstr("\\end{array}"));
			components.add(new TxtEquationMatrix(env,currentfnt,subcontent,alignstring));
			processed=true;
		}


		if (trykeyword(content,_qstr("\\mbox")))
		{
			QString subcontent;
			if (!getargument(content,subcontent,false)) throw QError(_text("Invalid mbox argument"));
			components.add(new TxtEquationChar(env,currentfnt,subcontent,false));
			processed=true;
		}

		if (trykeyword(content,_qstr("\\textbf")))
		{
			QString subcontent;
			if (!getargument(content,subcontent,false)) throw QError(_text("Invalid textbf argument"));
			TxtFont newfnt=currentfnt;
			newfnt.makebold();
			components.add(new TxtEquationChar(env,newfnt,subcontent,false));
			processed=true;
		}


		if (trykeyword(content,_qstr("\\sqrt")))
		{
			QString subcontent;
			if (!getargument(content,subcontent,false)) throw QError(_text("Invalid sqrt argument"));
			TxtEquation *subcomp=new TxtEquation(env,fnt);subcomp->parsefrom(subcontent);
			components.add(new TxtEquationSqrt(env,currentfnt,subcomp));
			processed=true;
		}

		if (trykeyword(content,_qstr("\\vec")))
		{
			QString subcontent;
			if (!getargument(content,subcontent,false)) throw QError(_text("Invalid vec argument"));
			TxtEquation *subcomp=new TxtEquation(env,fnt);subcomp->parsefrom(subcontent);
			components.add(new TxtEquationVector(env,currentfnt,subcomp));
			processed=true;
		}

		if (trykeyword(content,_qstr("\\overline")))
		{
			QString subcontent;
			if (!getargument(content,subcontent,false)) throw QError(_text("Invalid vec argument"));
			TxtEquation *subcomp=new TxtEquation(env,fnt);subcomp->parsefrom(subcontent);
			components.add(new TxtEquationOverline(env,currentfnt,subcomp));
			processed=true;
		}

		if (trykeyword(content,_qstr("\\left(")))
		{
			QString subcontent;
			splitblock_bykeyword(content,subcontent,_qstr("\\left("),_qstr("\\right)"));
			TxtEquation *subcomp=new TxtEquation(env,fnt);subcomp->parsefrom(subcontent);
			components.add(new TxtEquationBracket(env,currentfnt,subcomp,0));
			processed=true;
		}

		if (trykeyword(content,_qstr("\\left[")))
		{
			QString subcontent;
			splitblock_bykeyword(content,subcontent,_qstr("\\left["),_qstr("\\right]"));
			TxtEquation *subcomp=new TxtEquation(env,fnt);subcomp->parsefrom(subcontent);
			components.add(new TxtEquationBracket(env,currentfnt,subcomp,1));
			processed=true;
		}

		if (trykeyword(content,_qstr("\\left|")))
		{
			QString subcontent;
			splitblock_bykeyword(content,subcontent,_qstr("\\left|"),_qstr("\\right|"));
			TxtEquation *subcomp=new TxtEquation(env,fnt);subcomp->parsefrom(subcontent);
			components.add(new TxtEquationBracket(env,currentfnt,subcomp,2));
			processed=true;
		}

		if (trykeyword(content,_qstr("\\sum")))
		{
			components.add(new TxtEquationSum(env,currentfnt,_qstr("S")));
			processed=true;
		}

		if (trykeyword(content,_qstr("\\prod")))
		{
			components.add(new TxtEquationSum(env,currentfnt,_qstr("P")));
			processed=true;
		}

		if (trykeyword(content,_qstr("\\int")))
		{
			components.add(new TxtEquationIntegral(env,currentfnt));
			processed=true;
		}

		for (int toknr=0; (!processed)&&(toknr<tokens.G_count()); toknr++)
		{
			if (trykeyword(content,tokens[toknr]->token))
			{
				components.add(new TxtEquationChar(env,fnt,tokens[toknr]->word,tokens[toknr]->issymbol));
				processed=true;
			}
		}

		if ((!processed) && ( ((content[0]>='a')&&(content[0]<='z')) || ((content[0]>='A')&&(content[0]<='Z')) ) )
		{
			StrChar st[3];st[0]=content[0];st[1]=0;
			content.substring(1,content.G_length());
			if (content[0]=='\'')
			{ 
				st[1]=content[0];st[2]=0; 
				content.substring(1,content.G_length());
			}
			components.add(new TxtEquationVar(env,currentfnt,st));
			processed=true;
		}

		if ((!processed) && ( ((content[0]>='0')&&(content[0]<='9'))  ) )
		{
			QString st;
			while ( ((content[0]>='0')&&(content[0]<='9')) || (content[0]=='.') || (content[0]==',') )
			{
				st+=content[0];
				content.substring(1,content.G_length());
			}
			components.add(new TxtEquationChar(env,currentfnt,st,false));
			processed=true;
		}


		if (!processed)
		{
			StrChar st[2];st[0]=content[0];st[1]=0;
			components.add(new TxtEquationChar(env,currentfnt,st,false));
			content.substring(1,content.G_length());
		}
	}
}

void TxtEquation::parsefrom(StrPtr icontent)
{
	parsesub(fnt,icontent);
}

void TxtEquation::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	for (int i=0; i<components.G_count(); i++) components[i]->calc(renderer,maxsizex);
	baseline=0;
	for (int i=0; i<components.G_count(); i++)
		if (baseline<components[i]->G_baseline()) baseline=components[i]->G_baseline();
	sizex=0;
	sizey=0.0001;
	for (int i=0; i<components.G_count(); i++)
	{
		components[i]->Set_posx(sizex);
		components[i]->Set_posy(baseline-components[i]->G_baseline());
		sizex+=components[i]->G_sizex();
		if (i<components.G_count()-1)
			sizex+=renderer.G_fontsize(fnt)/12;
		if (sizey<components[i]->G_posy()+components[i]->G_sizey())
			sizey=components[i]->G_posy()+components[i]->G_sizey();
	}

	if (startnewlinebefore)
	{
		double shift=renderer.G_fontsize(fnt)/2;
		for (int i=0; i<components.G_count(); i++) components[i]->Set_posy(components[i]->G_posy()+shift);
		Set_sizey(G_sizey()+2*shift);
	}

}

void TxtEquation::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	for (int i=0; i<components.G_count(); i++)
		components[i]->render(xoffs+components[i]->G_posx(),yoffs+components[i]->G_posy(),renderer);
}
