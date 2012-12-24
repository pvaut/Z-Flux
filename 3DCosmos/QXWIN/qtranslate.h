#pragma once

#include "tools.h"
#include "qstring.h"

class QTranslate
{
private:
	QString activelanguage;
	Tbaltree index;
	Tarray<QString> translated;
	Tarray<QString> languages;
private:
	void load();
public:
	QTranslate();
	void loadlanguages();
	static QTranslate& Get()
	{
		static QTranslate sset;
		return sset;
	}
	StrPtr GetLanguage();
	void SetLanguage(StrPtr ilanguage);
	StrPtr Translate(StrPtr inp);

	int G_languagecount() const;
	StrPtr G_language(int nr) const;
};

