#pragma once

#include "tobject.h"
#include "qstring.h"
#include "qbintagfile.h"
#include "tools.h"
#include "QError.h"
#include "valnames.h"

class Thomogcoord;
class Taffinetransformation;

class TMatrix : public TObject
{
private:
	int dim1,dim2;//dim1=row dim2=col
	mutable Tarray<Tdoublearray> vals;
public:
	static StrPtr GetClassName() { return SC_valname_matrix; }
	TMatrix();
	void redim(int idim1, int idim2);
	void makeunit(int idim);
	int G_dim1() const { return dim1; }
	int G_dim2() const { return dim2; }
	void addscalar(double offset);
	double G_minval() const;
	double G_maxval() const;
	double Get(int i1, int i2) const
	{
		if ((i1<0)||(i1>=dim1)||(i2<0)||(i2>=dim2))
			throw QError(_qstr("Invalid matrix index"));
		return vals[i1]->get(i2);
	}
	void Set(int i1, int i2, double vl)
	{
		if ((i1<0)||(i1>=dim1)||(i2<0)||(i2>=dim2))
			throw QError(_qstr("Invalid matrix index"));
		return vals[i1]->set(i2,vl);
	}
	void tostring(QString &str, int tpe) const;
	void fromstring(StrPtr str);
	static bool compare(TMatrix *v1, TMatrix *v2) { return false; }
	static bool comparesize(TMatrix *v1, TMatrix *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
	void operator=(const TMatrix &v);
	void copyfrom(const Thomogcoord *coord);
	void copyto(Thomogcoord *coord) const;

	void loadfromfile(StrPtr filename, int res1, int idx1min, int idx1max, int res2, int idx2min, int idx2max, int bytecount);

public:
	void calctranspose(const TMatrix *m);
	void mulfrom(const TMatrix *m, double vl);
	void mulby(double vl);
	void add(const TMatrix *m1, const TMatrix *m2);
	void subtract(const TMatrix *m1, const TMatrix *m2);
	void multiply(const TMatrix *m1, const TMatrix *m2);
	void divide(const TMatrix *b, const TMatrix *A);

public:
	void mul(const Thomogcoord *arg, Thomogcoord *result) const;
	void multransp(const Thomogcoord *arg, Thomogcoord *result) const;
	void SVD(TMatrix *U, Tdoublearray *eigvals, TMatrix *V) const;
	void calcinverse(const TMatrix *M);

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

};

