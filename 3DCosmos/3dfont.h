#pragma once


class T3DFont
{
private:
	QString name;
	GLYPHMETRICSFLOAT gmf[256];
	int fontid;
public:
	T3DFont(StrPtr iname, int ifontid);
	void init(CDC *dc);
	int G_ID() { return fontid; }
	StrPtr G_name() { return name; }

	double G_textsizex(StrPtr txt);

};