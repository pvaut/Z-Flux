#pragma once

#include "vecmath.h"
#include "matrix.h"
#include "qbintagfile.h"


class Taffinetransformation : public TObject
{
private:
	TMatrix M,Minv;
	bool isrescaling;
public:
	static StrPtr GetClassName() { return SC_valname_affinetransformation; }
	Taffinetransformation();
	void reset();
	void maketranslation(const Tvector *v);
	void makerotation(const Tvector *rotdir, double angle);
	void makerotation_X(double angle);
	void makerotation_Y(double angle);
	void makerotation_Z(double angle);
	void updateinverse();
	void operator=(const Taffinetransformation &v);
	void translatefrom(const Taffinetransformation *tf, const Tvector *v);
	void scale(double iscalefactor);
	void rotatefrom(const Taffinetransformation *tf, const Tvector *rotdir, double angle);
	void invert();
	void copyfrom(const TMatrix *m);
	void copyto(TMatrix *m) const;
	void Set_origin(const Tvertex *v);
	void Set_origin_vector(const Tvector *v);
	void Set_xaxis(const Tvector *rt);
	void Set_yaxis(const Tvector *rt);
	void Set_zaxis(const Tvector *rt);
	double G_scalefactor() const;
	void G_origin(Tvertex *origin) const;
	void G_origin_vector(Tvector *origin) const;
	void G_xaxis(Tvector *rt) const;
	void G_yaxis(Tvector *rt) const;
	void G_zaxis(Tvector *rt) const;
	bool G_isrescaling() const { return isrescaling; }
	void multiply(const Taffinetransformation *tf1, const Taffinetransformation *tf2);
	void getangles(double &ax, double &ay, double &az) const;
	void weight(const Taffinetransformation *tf1, const Taffinetransformation *tf2, double fr);
	void mul(const Thomogcoord *arg, Thomogcoord *result) const;
	void mulnormvec(const Tvector *arg, Tvector *result) const;
	void mulinv(const Thomogcoord *arg, Thomogcoord *result) const;

	void streamout(double *vals);
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr icontent);
	static bool compare(Taffinetransformation *val1, Taffinetransformation *val2)
	{
		return TMatrix::compare(&val1->M,&val2->M);
	}
	static bool comparesize(Taffinetransformation *val1, Taffinetransformation *val2)
	{
		throw QError(_text("Unable to compare this data type"));
	}

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);


};