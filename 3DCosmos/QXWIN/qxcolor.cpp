#include "stdafx.h"
#include "qxcolor.h"
#include<math.h>


#define Pi 3.1415927


void HSB2RGB(double H, double S, double B, double &r, double &g, double &b)
{
	r=0;g=0;b=0;
	if ((H>=0)&&(H<=2*Pi/3))
	{
		b=(1-S)/3;
		r=(1.0+S*cos(H)/cos(2*Pi/6-H))/3;
		g=1.0-r-b;
	}
	if ((H>=2*Pi/3)&&(H<=4*Pi/3))
	{
		r=(1-S)/3;
		g=(1.0+S*cos(H-2*Pi/3)/cos(2*Pi/6-H+2*Pi/3))/3;
		b=1.0-r-g;
	}
	if ((H>=4*Pi/3)&&(H<=6*Pi/3))
	{
		g=(1-S)/3;
		b=(1.0+S*cos(H-4*Pi/3)/cos(2*Pi/6-H+4*Pi/3))/3;
		r=1.0-g-b;
	}

	H=max(r,max(g,b));
	if (H>0) B=B/H;
	r=r*B;
	g=g*B;
	b=b*B;
}


void HSI2RGB(double H, double S, double I, double &r, double &g, double &b)
{
	r=0;g=0;b=0;
	if ((H>=0)&&(H<=2*Pi/3))
	{
		b=(1-S)/3;
		r=(1.0+S*cos(H)/cos(2*Pi/6-H))/3;
		g=1.0-r-b;
	}
	if ((H>=2*Pi/3)&&(H<=4*Pi/3))
	{
		r=(1-S)/3;
		g=(1.0+S*cos(H-2*Pi/3)/cos(2*Pi/6-H+2*Pi/3))/3;
		b=1.0-r-g;
	}
	if ((H>=4*Pi/3)&&(H<=6*Pi/3))
	{
		g=(1-S)/3;
		b=(1.0+S*cos(H-4*Pi/3)/cos(2*Pi/6-H+4*Pi/3))/3;
		r=1.0-g-b;
	}
	r=r*I*3;
	g=g*I*3;
	b=b*I*3;
	if (r<0) r=0; if (r>1) r=1;
	if (g<0) g=0; if (g>1) g=1;
	if (b<0) b=0; if (b>1) b=1;
}


void RGB2HSI(double r, double g, double b, double &H, double &S, double &I)
{
	I=(r+g+b)/3;
	S=1.0-min(r,min(g,b))*1.0/I;
	double DDD=sqrt((r-g)*(r-g)+(r-b)*(g-b));
	if (DDD<0.001) DDD=0.001;
	double cosH=(2*r-g-b)/(2*DDD);
	double sinH=sqrt(3.0)*(g-b)/(2*DDD);
	H=atan2(sinH,cosH);
	if (H<0) H+=2*Pi;
}

TQXColor::TQXColor()
{
	r=0;g=0;b=0;f=1;a=1;
}

TQXColor::TQXColor(float ir, float ig, float ib, float ia, float iif)
{
	set(ir,ig,ib,ia,iif);
}

TQXColor::TQXColor(const TQXColor& icol)
{
	r=icol.r;g=icol.g;b=icol.b;a=icol.a;f=icol.f;
}

TQXColor::TQXColor(COLORREF cl)
{
	r=GetRValue(cl)/255.0;
	g=GetGValue(cl)/255.0;
	b=GetBValue(cl)/255.0;
	f=1;a=1;
}



void TQXColor::set(float ir, float ig, float ib, float ia, float iif)
{
	r=ir;g=ig;b=ib;a=ia;f=iif;
}

float TQXColor::G_r() const
{
	if (f<=0) return 0; else return r/f;
}

float TQXColor::G_g() const
{
	if (f<=0) return 0; else return g/f;
}

float TQXColor::G_b() const
{
	if (f<=0) return 0; else return b/f;
}

float TQXColor::G_a() const
{
	if (f<=0) return 0; else return a/f;
}


TQXColor TQXColor::operator+(const TQXColor &arg) const
{
	return TQXColor(r+arg.r,g+arg.g,b+arg.b,a+arg.a,f+arg.f);
}

TQXColor::operator COLORREF() const
{
	return RGB((int)(0.5+255*G_r()),(int)(0.5+255*G_g()),(int)(0.5+255*G_b()));
}


TQXColor operator*(int fc, const TQXColor &cl)
{
	return TQXColor(fc*cl.r,fc*cl.g,fc*cl.b,fc*cl.a,fc*cl.f);
}

TQXColor operator*(double fc, const TQXColor &cl)
{
	return TQXColor((float)(fc*cl.r),(float)(fc*cl.g),(float)(fc*cl.b),(float)(fc*cl.a),(float)(fc*cl.f));
}

TQXColor TQXColor::transp(double fr) const
{
	return TQXColor(r/f,g/f,b/f,1.0-fr,1.0f);
}


TQXColor TQXColor::norm() const
{
	if (f<=0) return *this;
	return TQXColor(r/f,g/f,b/f,a/f,1.0f);
}


TQXColor TQXColor::IntensIncr(double fr) const
{
	double H,S,I,rf,gf,bf;
	RGB2HSI(G_r(),G_g(),G_b(),H,S,I);
	I=(1-fr)*I+fr*1;
	HSI2RGB(H,S,I,rf,gf,bf);
	return TQXColor((float)rf,(float)gf,(float)bf,1,1);
}

TQXColor TQXColor::IntensDecr(double fr) const
{
	double H,S,I,rf,gf,bf;
	RGB2HSI(G_r(),G_g(),G_b(),H,S,I);
	I=(1-fr)*I+fr*0;
	HSI2RGB(H,S,I,rf,gf,bf);
	return TQXColor((float)rf,(float)gf,(float)bf,1,1);
}

TQXColor TQXColor::SatIncr(double fr) const
{
	double H,S,I,rf,gf,bf;
	RGB2HSI(G_r(),G_g(),G_b(),H,S,I);
//	S=(1-fr)*S+fr*1;
	S=S*(1+fr);
	if (S>1) S=1;
	HSI2RGB(H,S,I,rf,gf,bf);
	return TQXColor((float)rf,(float)gf,(float)bf,1,1);
}

TQXColor TQXColor::SatDecr(double fr) const
{
	double H,S,I,rf,gf,bf;
	RGB2HSI(G_r(),G_g(),G_b(),H,S,I);
	S=(1-fr)*S+fr*0;
	HSI2RGB(H,S,I,rf,gf,bf);
	return TQXColor((float)rf,(float)gf,(float)bf,1,1);
}

TQXColor TQXColor::IntensChange(double fr) const
{
	if (fr>0.99) fr=0.99;
	if (fr<0) fr=2*fr;
	float rf=G_r();
	float gf=G_g();
	float bf=G_b();
	if (fr!=0)
	{
/*		if (r<0) rf=0;if (r>1) rf=1;
		if (g<0) gf=0;if (g>1) gf=1;
		if (b<0) bf=0;if (b>1) bf=1;*/
		if (r>0) rf=pow(rf,1-(float)fr);
		if (g>0) gf=pow(gf,1-(float)fr);
		if (b>0) bf=pow(bf,1-(float)fr);
	}
	return TQXColor(rf,gf,bf,1,1);
}
