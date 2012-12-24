#include "stdafx.h"

#include "qerror.h"
#include "qstringformat.h"

#include "formattexttabular.h"



////////////////////////////////////////////////////////////////////////
// TabularCell
////////////////////////////////////////////////////////////////////////

TabularCell::TabularCell(TxtEnv *ienv, const TxtFont &ifnt)
: TxtBlock(ienv,ifnt)
{
	align=-2;
}



////////////////////////////////////////////////////////////////////////
// TxtTabular
////////////////////////////////////////////////////////////////////////

TxtTabular::TxtTabular(TxtEnv *ienv, const TxtFont &ifnt)
: TxtComponent(ienv,ifnt)
{
}



void TxtTabular::parsefrom(StrPtr icols, StrPtr icontent)
{
	QString cols=icols;
	colaligns.reset();
	while (qstrlen(cols)>0)
	{
		bool processed=false;
		if ((!processed)&&(cols[0]=='|'))
		{
			vertlineleftfrom.add(colaligns.G_count());
			cols.substring(1,cols.G_length());
			processed=true;
		}
		if ((!processed)&&(cols[0]=='l'))
		{ 
			colaligns.add(-1); fixedcolsizes.add(-1); 
			cols.substring(1,cols.G_length());
			processed=true;
		}
		if ((!processed)&&(cols[0]=='c'))
		{ 
			colaligns.add(0); fixedcolsizes.add(-1); 
			cols.substring(1,cols.G_length());
			processed=true;
		}
		if ((!processed)&&(cols[0]=='r'))
		{ 
			colaligns.add(+1); fixedcolsizes.add(-1); 
			cols.substring(1,cols.G_length());
			processed=true;
		}
		if ((!processed)&&(cols[0]=='p'))
		{
			cols.substring(1,cols.G_length());
			QString substr;
			colaligns.add(-1);
			getargument(cols,substr,true);
			fixedcolsizes.add(ConvertSize(substr,fnt));
			processed=true;
		}
	}

	colcount=colaligns.G_count();

	QString content;content=icontent;

	while (content.G_length()>0)
	{
		QString linetxt;
		content.splitstring(_qstr("\\\\"),linetxt);
		eatstartspaces(linetxt);
		if (trykeyword(linetxt,_qstr("\\hline")))
		{
			horlinetopof.add(lines.G_count());
			eatstartspaces(linetxt);
		}
		if (linetxt.G_length()>0)
		{
			TabularLine *line=new TabularLine;lines.add(line);
			int curcolnr=0;
			while (linetxt.G_length()>0)
			{
				QString celltxt;
				linetxt.splitstring(_qstr("&"),celltxt);
				eatstartspaces(celltxt);eatendspaces(celltxt);
				TabularCell *cell=new TabularCell(env,fnt);
				cell->colnr_start=curcolnr;cell->colnr_end=curcolnr;
				if (trykeyword(celltxt,_qstr("\\multicolumn")))
				{
					QString arg_num,arg_align,arg_content;
					getargument(celltxt,arg_num,true);
					getargument(celltxt,arg_align,true);
					getargument(celltxt,arg_content,true);
					if (arg_align[0]=='l') cell->align=-1;
					if (arg_align[0]=='c') cell->align= 0;
					if (arg_align[0]=='r') cell->align=+1;
					cell->colnr_end=cell->colnr_start+qstr2int(arg_num)-1;
					celltxt=arg_content;
				}
				cell->parsefrom(celltxt);
				line->cells.add(cell);
				curcolnr++;
				if (curcolnr>colcount) colcount=curcolnr;
			}
		}
	}

}

void TxtTabular::calc_implement(const TxtRenderer &renderer, double maxsizex)
{
	//calculate cell sizes
	for (int linenr=0; linenr<lines.G_count(); linenr++)
		for (int cellnr=0; cellnr<lines[linenr]->cells.G_count(); cellnr++)
		{
			TabularCell *cell=lines[linenr]->cells[cellnr];
			double lmaxsize=0;
			for (int i=cell->colnr_start; i<=cell->colnr_end; i++) lmaxsize+=fixedcolsizes[i];
			if (lmaxsize<0.00001) lmaxsize=maxsizex;
			cell->calc(renderer,lmaxsize);
		}


	//calculate line heights
	Tdoublearray lineheights;
	for (int linenr=0; linenr<lines.G_count(); linenr++)
	{
		double lh=0;
		for (int cellnr=0; cellnr<lines[linenr]->cells.G_count(); cellnr++)
		{
			TabularCell *cell=lines[linenr]->cells[cellnr];
			if (lh<cell->G_sizey()) lh=cell->G_sizey();
		}
		lineheights.add(lh);
	}

	//calculate line positions
	double cury=0;
	for (int linenr=0; linenr<lines.G_count(); linenr++)
	{
		lineposy.add(cury);
		cury+=lineheights[linenr]+fnt.G_size()/4.0;
	}
	lineposy.add(cury);


	//calculate column widths
	Tdoublearray colsizes;
	for (int colnr=0; colnr<colcount; colnr++) colsizes.add(0);
	for (int linenr=0; linenr<lines.G_count(); linenr++)
		for (int cellnr=0; cellnr<lines[linenr]->cells.G_count(); cellnr++)
		{
			TabularCell *cell=lines[linenr]->cells[cellnr];
			double curspansize=0;
			for (int i=cell->colnr_start; i<=cell->colnr_end; i++) curspansize+=colsizes[i];
			if (curspansize<cell->G_sizex())
			{
				double incr=(cell->G_sizex()-curspansize)/(cell->colnr_end-cell->colnr_start+1);
				for (int i=cell->colnr_start; i<=cell->colnr_end; i++) colsizes[i]=colsizes[i]+incr;
			}
		}

	//calculate column positions
	double colsep=fnt.G_size();
	double curx=0;
	for (int colnr=0; colnr<colcount; colnr++)
	{
		colposx.add(curx);
		curx+=colsizes[colnr]+colsep;
	}
	colposx.add(curx);

	//set cell positions
	for (int linenr=0; linenr<lines.G_count(); linenr++)
		for (int cellnr=0; cellnr<lines[linenr]->cells.G_count(); cellnr++)
		{
			TabularCell *cell=lines[linenr]->cells[cellnr];
			int align=colaligns[cell->colnr_start];
			if (cell->align>-2) align=cell->align;
			if (align==-1) cell->Set_posx(colposx[cell->colnr_start]+colsep/2);
			if (align== 0) cell->Set_posx((colposx[cell->colnr_start]+colposx[cell->colnr_end+1]-cell->G_sizex())/2);
			if (align==+1) cell->Set_posx(colposx[cell->colnr_end+1]-colsep/2-cell->G_sizex());
			cell->Set_posy((lineposy[linenr]+lineposy[linenr+1]-cell->G_sizey())/2);
		}

	sizex=colposx[colcount];
	sizey=lineposy[lines.G_count()];
}

void TxtTabular::render_impl(double xoffs, double yoffs, const TxtRenderer &renderer) const
{
	for (int linenr=0; linenr<lines.G_count(); linenr++)
		for (int cellnr=0; cellnr<lines[linenr]->cells.G_count(); cellnr++)
		{
			TabularCell *cell=lines[linenr]->cells[cellnr];
			cell->render(xoffs+cell->G_posx(),yoffs+cell->G_posy(),renderer);
		}

	for (int i=0; i<horlinetopof.G_count(); i++)
	{
		renderer.render_horizontalline(xoffs,yoffs+lineposy[horlinetopof[i]],sizex,fnt.G_size()/14.0);
	}

	for (int i=0; i<vertlineleftfrom.G_count(); i++)
	{
		renderer.render_verticalline(xoffs+colposx[vertlineleftfrom[i]],yoffs,sizey,fnt.G_size()/14.0);
	}
}
