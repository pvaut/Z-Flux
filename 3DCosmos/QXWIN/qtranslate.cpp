#include "stdafx.h"

#include "qstring.h"
#include "qstringformat.h"
#include "qfile.h"
#include "tools.h"
#include "qtranslate.h"

StrPtr G_datadir();


QTranslate::QTranslate()
{
}

void QTranslate::loadlanguages()
{
	QFileFind fnd;
	QString filename;
	bool isdir,ishidden;
	fnd.start(TFormatString(_qstr("^1\\languages\\*.*"),G_datadir()));
	while (fnd.getnext(filename,isdir,ishidden))
		if ((isdir)&&(!issame(filename,_qstr(".")))&&(!issame(filename,_qstr(".."))))
			languages.add(new QString(filename));
}


void QTranslate::load()
{
	index.reset();
	translated.reset();

	if (qstrlen(activelanguage)>0)
	{
		QFileFind fnd;
		QString filename,line,part1;
		bool isdir,ishidden;
		fnd.start(TFormatString(_qstr("^1\\languages\\^2\\*.txt"),G_datadir(),activelanguage));
		while (fnd.getnext(filename,isdir,ishidden))
		{
			if (!isdir)
			{
				QString filepath=TFormatString(_qstr("^1\\languages\\^2\\^3"),G_datadir(),activelanguage,filename);
				QTextfile fle;
				fle.openread(filepath);
				while (!fle.isend())
				{
					fle.readline(line);
					line.splitstring(_qstr("\t"),part1);
					if ((qstrlen(part1)>0)&&(qstrlen(line)>0))
					{
						index.add(part1,translated.G_count());
						translated.add(new QString(line));
					}
				}
			}
		}
	}

}


int QTranslate::G_languagecount() const
{
	return languages.G_count();
}

StrPtr QTranslate::G_language(int nr) const
{
	if ((nr<0)||(nr>=languages.G_count())) return _qstr("");
	return *languages[nr];
}


StrPtr QTranslate::GetLanguage()
{
	if (qstrlen(activelanguage)<=0) return _qstr("English");
	return activelanguage;
}

void QTranslate::SetLanguage(StrPtr ilanguage)
{
	if (issame(activelanguage,ilanguage)) return;
	activelanguage=ilanguage;
	load();
}

StrPtr QTranslate::Translate(StrPtr inp)
{
	int nr=index.get(inp);
	if (nr<0) return inp;
	return *translated[nr];
}

StrPtr _TRL(StrPtr inp)
{
	return QTranslate::Get().Translate(inp);
}
