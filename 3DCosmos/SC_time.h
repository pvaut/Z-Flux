#pragma once

#include "sc_value.h"
#include "qbintagfile.h"

#include "tobject.h"


class TSC_time : public TObject
{
private:
	const static int TMFU=1000000;
private:
	int idx1,idx2,idx3;
	double JD;
	int year,month,day,hour,min,sec,secfr;
	double T,st0,st00,tau,delt;
	double st0_uniform;//like st0, but not clipped to 24h
public:
	static StrPtr GetClassName() { return SC_valname_time; }
public:
	TSC_time();
	double G_JD0() const { return JD; }
	double G_JD() const;
	void Set_JD(double iJD);
	double G_tau() const { return tau; }
	double G_T() const { return T; }
	void calcidx();
	void calccal();
	void calcsiderial();
	double diffsecs(TSC_time &starttm);
	double diffdays(TSC_time &starttm);
	double diffanglefrac(const TSC_time &starttm, double period) const;
	void addseconds(double seconds);
	void adddays(double days);
	void tostring(QString &str, int tpe) const;
	void fromstring(StrPtr str);
	void fillwithnowUT();
	void operator=(const TSC_time &v);
public:
	static bool compare(TSC_time *v1, TSC_time *v2);
	static bool comparesize(TSC_time *v1, TSC_time *v2);

public:
	int G_year() const { return year; }
	int G_month() const { return month; }
	int G_day() const { return day; }
	int G_hour() const { return hour; }
	int G_min() const { return min; }
	double G_sec() const { return sec+secfr*1.0/TMFU; }
	double G_ST0_uniform() const { return st0_uniform; } 

	void Set_year(int ivl) { year=ivl; calcidx(); }
	void Set_month(int ivl) { month=ivl; calcidx(); }
	void Set_day(int ivl) { day=ivl; calcidx(); }
	void Set_hour(int ivl) { hour=ivl; calcidx(); }
	void Set_min(int ivl) { min=ivl; calcidx(); }
	void Set_sec(double ivl) { sec=(int)(ivl); secfr=(int)((ivl-(int)(ivl))*TMFU); calcidx(); }

	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);

};
/*
idx1=day idx2=seconds inside day idx3=fraction of seconds, measured by multiples of TMFU
*/



class TSC_chrono : public TObject
{
private:
	double rendertimerfreq;
	LARGE_INTEGER startidx;
	double timeroffset;
	bool pauzed;
public:
	static StrPtr GetClassName() { return SC_valname_chrono; }
public:
	TSC_chrono();
	void tostring(QString &str, int tpe) const;
	void fromstring(StrPtr str);
	void operator=(const TSC_chrono &v);
public:
	static bool compare(TSC_chrono *v1, TSC_chrono *v2) { return false; }
	static bool comparesize(TSC_chrono *v1, TSC_chrono *v2)
	{
		throw QError(_text("Unable to compare this data type"));
	}
	void streamout(QBinTagWriter &writer);
	void streamin(QBinTagReader &reader);
public:
	void start();
	void pauze();
	void resume();
	void set(double ival);
	double G_elapsed() const;
};
