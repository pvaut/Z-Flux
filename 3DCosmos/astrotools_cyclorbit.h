


/*
typedef struct {
  int maxorder;
  struct {
    int    count;
    double A[200],B[200],C[200];
  } terms[20];
} Tplanexpans;

class Tcyclorbit
{
private:
	double scalefactor;
	Tplanexpans expa[4];  // 1=L 2=B 3=R
public:
	Tcyclorbit(StrPtr ifilename, double iscalefactor);
	void calcposit(const TSC_time& time, Tvertex &posit);
	void getspeed(const TSC_time& time, Tvector &spd);
	double G_period() const;//in days
};
*/


class Tcyclorbit2comp
{
public:
	Tarray<Tdoublearray> A,B,C;//first index=order
	double accuracy,maxA;
	bool fixedaccuracy;
	int usedtermcount,rejectedtermcount;
public:
	Tcyclorbit2comp();
	void reset(double iaccuracy);
	void try_addterm(int order, double iA, double iB, double iC);
	double calc(double tau);
};

class Tcyclorbit2
{
private:
	double scalefactor;
	Tcyclorbit2comp expa_L,expa_B,expa_R;
public:
	Tcyclorbit2(StrPtr ifilename, double accuracy, double iscalefactor);
	void calcposit(const TSC_time& time, Tvertex &posit);
	void getspeed(const TSC_time& time, Tvector &spd);
	double G_period() const;//in days
};

Tcyclorbit2* G_stockcyclorbit(StrPtr name);



class Tlunaorbit
{
private:
private:
	int    term1count,term2count;
	struct {
		int    i1,i2,i3,i4;
		int    j1,j2;
	} term1[200];
	struct {
		int    i1,i2,i3,i4;
		int    j1;
	} term2[200];
public:
	double l,b,d;
	double Xrel,Yrel,Zrel;//in au
public:
	Tlunaorbit();
	void calcposit(const TSC_time& time, Tvertex &posit, double &rotatangle);
	void getspeed(const TSC_time& time, Tvector &spd);
};
