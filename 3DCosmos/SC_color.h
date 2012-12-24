#pragma once

#include "qbintagfile.h"

#include "valnames.h"
#include "SC_value.h"

class TSC_color : public TObject
{
private:
	double R,G,B,A,F;
public:
	static StrPtr GetClassName() { return SC_valname_color; }
	TSC_color()
	{
		R=1;G=1;B=1;A=1;F=1;
	}
	TSC_color(double iR, double iG, double iB, double iA)
	{
		R=iR;
		G=iG;
		B=iB;
		A=iA;
		F=1;
	}
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr icontent);
	void copyfrom(double iR, double iG, double iB, double iA)
	{
		R=iR;
		G=iG;
		B=iB;
		A=iA;
		F=1;
	}
	void copyfrom(TSC_color *ivl)
	{
		R=ivl->R;
		G=ivl->G;
		B=ivl->B;
		A=ivl->A;
		F=ivl->F;
	}
	static bool compare(TSC_color *val1, TSC_color *val2)
	{
		if (fabs(val1->G_R()-val2->G_R())>0.00001) return false;
		if (fabs(val1->G_G()-val2->G_G())>0.00001) return false;
		if (fabs(val1->G_B()-val2->G_B())>0.00001) return false;
		if (fabs(val1->G_A()-val2->G_A())>0.00001) return false;
		return true;
	}
	static bool comparesize(TSC_color *val1, TSC_color *val2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
	void operator=(const TSC_color &obj)
	{
		R=obj.R;
		G=obj.G;
		B=obj.B;
		A=obj.A;
		F=obj.F;
	}
	double G_R() const { return R/F; }
	double G_G() const { return G/F; }
	double G_B() const { return B/F; }
	double G_A() const { return A/F; }
	void Set_R(double ival) { R=ival*F; }
	void Set_G(double ival) { G=ival*F; }
	void Set_B(double ival) { B=ival*F; }
	void Set_A(double ival) { A=ival*F; }

	void mulwith(const TSC_color *col);
	void addfrom(TSC_color *col1, TSC_color *col2);
	void mulfrom(double vl, TSC_color *col);

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

};
