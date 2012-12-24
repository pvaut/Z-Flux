#pragma once

class TQXColor
{
private:
	float r,g,b,a,f;
public:
	TQXColor();
	TQXColor(COLORREF cl);
	TQXColor(const TQXColor& icol);
	TQXColor(float ir, float ig, float ib, float ia=1.0, float iif=1.0);
	void set(float ir, float ig, float ib, float ia=1.0, float iif=1.0);
	float G_r() const;
	float G_g() const;
	float G_b() const;
	float G_a() const;
	TQXColor operator+(const TQXColor &arg) const;
	operator COLORREF() const;
	TQXColor transp(double fr) const;
	TQXColor norm() const;
	TQXColor IntensIncr(double fr) const;
	TQXColor IntensDecr(double fr) const;
	TQXColor SatIncr(double fr) const;
	TQXColor SatDecr(double fr) const;
	TQXColor IntensChange(double fr) const;//>0: increase 0<=decrease
public:
	friend TQXColor operator*(int fc, const TQXColor &cl);
	friend TQXColor operator*(double fc, const TQXColor &cl);
};

TQXColor operator*(int fc, const TQXColor &cl);
