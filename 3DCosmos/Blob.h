

class TBlobComp
{
public:
	int tpe;//0=sphere 1=segment 2=torus 3=disc
	double R;
	double Rinfl;//influence radius
	double torusradius;
	double w;//weight
	double A;//total final amplitude
	double x0,y0,z0;
	double xd,yd,zd;//direction for segment
	double M;
	double xn,yn,zn;//direction for torus & disc
public:
	TBlobComp();
	void copyfrom(const TBlobComp &icmp);
	void makesphere(double i_x0, double i_y0, double i_z0, double i_R, double i_Rinfl, double i_w);
	void makesegment(double i_x0, double i_y0, double i_z0, double i_x1, double i_y1, double i_z1, double i_R, double i_Rinfl, double i_w);
	void maketorus(double i_x0, double i_y0, double i_z0, double i_nx, double i_ny, double i_nz, double i_torusradius, double i_R, double i_Rinfl, double i_w);
	void makedisc(double i_x0, double i_y0, double i_z0, double i_nx, double i_ny, double i_nz, double i_dradius, double i_R, double i_Rinfl, double i_w);

	void G_boundingbox(Tvertex &minpt, Tvertex &maxpt) const;
	double eval(double x, double y, double z) const;

};

class TBlob : public TObject
{
private:
	Tarray<TBlobComp> comps;
public:
	static StrPtr GetClassName() { return SC_valname_blob; }
	TBlob();
	TBlob(const TBlob &iblob);
	~TBlob();

	void operator=(const TBlob &v);
	void tostring(QString &str, int tpe);
	void fromstring(StrPtr str);
	static bool compare(TBlob *v1, TBlob *v2) { return false; }
	static bool comparesize(TBlob *v1, TBlob *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}

	void reset();
	TBlobComp* addcomp(); 

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);


	void copyfrom(const TBlob &iblob);

	void G_boundingbox(Tvertex &minpt, Tvertex &maxpt) const;
	double eval(double x, double y, double z) const;
};
