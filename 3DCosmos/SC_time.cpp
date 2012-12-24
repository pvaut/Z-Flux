#include "stdafx.h"
#include "SC_env.h"
#include "SC_time.h"
#include "SC_func.h"

TSC_time::TSC_time()
{
	year=2000;month=1;day=1;hour=0;min=0;sec=0;secfr=0;
	calcidx();
}

double TSC_time::G_JD() const
{
	double vl=secfr*1.0/TMFU;
	vl=(vl+sec)/60.0;
	vl=(vl+min)/60.0;
	vl=(vl+hour)/24.0;
	return JD+vl;
}

void TSC_time::Set_JD(double iJD)
{
  idx1= (int)(iJD - 2415020.5);
  JD=idx1+2415020.5;
  double fr=iJD-JD;
  idx2=(int)(fr*24*60*60);
  idx3=0;
  calccal();
}


void TSC_time::calcidx()
{
	int yy,mm,A,B,y,m;
	double dd;

	yy=year;mm=month;
	dd=day;
	if (mm>2) { y=yy; m=mm; }
	else { y=yy-1; m=mm+12; }
	if (yy>1582)
	  {
	  A=(int)floor(y*1.0/100.0);
	  B=(int)(2.0-A+floor(A*1.0/4.0));
	}
	else { B=0;}
	JD =  floor(365.25*(y+4716));
	JD += floor(30.6001*(m+1));
	JD += dd;
	JD += -1524.5;
	JD += B;
	idx1= (int)(JD - 2415020.5);
	idx2= sec + min*60 + hour*60*60;
	idx3= secfr;

	calccal();
}

void TSC_time::calccal()
{
	int    A,Z,al,B,C,D,E,mm,yy,sc;
	double JDl,F,dd;

	JD= idx1+2415020.5;
	JDl=JD+0.5;
	Z=(int)floor(JDl);
	F=(int)(JDl-Z);
	if (Z<2299161) A=Z;
	else
	{
	  al=(int)floor((Z-1867216.25)/36524.25);
	  A=(int)(Z+1+al-floor(al/4.0));
	}
	B=A+1524;
	C=(int)floor((B-122.1)/365.25);
	D=(int)floor(365.25*C);
	E=(int)floor((B-D)/30.6001);
	dd=(int)(B-D-floor(30.6001*E)+F);
	if (E<14) mm=E-1;
	else      mm=E-13;
	if (mm>2) yy=C-4716;
	else      yy=C-4715;

	year=yy;
	month=mm;
	day=(int)floor(dd);

	secfr=idx3;
	sc=idx2;
	sec=  sc%60; sc= sc/60;
	min=  sc%60; sc= sc/60;
	hour= sc%24;

	calcsiderial();
}

void TSC_time::calcsiderial()
{
	double r,rs1,th;

	T=(G_JD()-2451545.0)/36525.0;
	delt=(102.3+123.5*T+32.5*T*T)/3600/24.0;
	T=(G_JD()+delt-2451545.0)/36525;
	tau=(G_JD()+delt-2451545.0)/365250;


	st0=280.46061837+360.98564736629*(G_JD()-2451545.0)+
	   0.000387*T*T-T*T*T/38710000;
	st0_uniform=st0/180*Pi;
	r=int(st0/360)*360;
	st0=st0-r;
	st0=st0/360*24;
	if (st0<0) st0=st0+24;

	rs1=(G_JD0()-2415020.0)/36525.0;
	th=0.276919398+100.0021359*rs1+0.000001075*rs1*rs1;
	th=th-floor(th);
	st00=th*24.0;

	st0=st0/12*Pi;
	st00=st00/12*Pi;
}


double TSC_time::diffsecs(TSC_time &starttm)
{
	double diffsec;
	diffsec= (idx3-starttm.idx3)*1.0/TMFU
		    +(idx2-starttm.idx2)*1.0
		    +(idx1-starttm.idx1)*24.0*60.0*60.0;
	return diffsec;
}

double TSC_time::diffdays(TSC_time &starttm)
{
	return idx1-starttm.idx1
			+(idx2-starttm.idx2)*1.0/(24*60*60);
}

double TSC_time::diffanglefrac(const TSC_time &starttm, double period) const
{
	double df1=(idx3-starttm.idx3)*1.0/TMFU;
	double df2=(idx2-starttm.idx2)*1.0;
	double df3=(idx1-starttm.idx1)*24.0*60.0*60.0;
	df1/=period;df2/=period;df3/=period;
	df1-=floor(df1);df2-=floor(df2);df3-=floor(df3);
	double dff=df1+df2+df3;
	while (dff<0) dff+=1;
	while (dff>1) dff-=1;
	return dff;
}



void TSC_time::addseconds(double seconds)
{
	double secintpart,secfracpart;
	secfracpart=modf(seconds,&secintpart);

	__int64 tmpidx2=idx2+(int)floor(secintpart+0.000001);
	__int64 tmpidx3=idx3+(int)((secfracpart)*TMFU);

	while (tmpidx3<0)
	{
		tmpidx3+=TMFU;
		tmpidx2--;
	}
	while (tmpidx3>=TMFU)
	{
		tmpidx3-=TMFU;
		tmpidx2++;
	}

	idx3=tmpidx3;

	__int64 addidx1=0;
	while (tmpidx2<0)
	{
		tmpidx2+=(__int64)(24*60*60);
		addidx1--;
	}

	idx2=(int)(tmpidx2%(__int64)(24*60*60));
	idx1+=(int)(addidx1+tmpidx2/(__int64)(24*60*60));
	calccal();
}

void TSC_time::adddays(double days)
{
	idx1+=(int)(days);
	idx2+=(int)((days-(int)days)*24*60*60);
	if (idx2<0)
	{
		idx1--;
		idx2+=24*60*60;
	}
	idx1+=idx2/(24*60*60);
	idx2=idx2%(24*60*60);
	calccal();
}

void TSC_time::tostring(QString &str, int tpe) const
{
	if (tpe==1)
	{
		CString st;
		st.Format(_qstr("%.2dh%.2dm%.2ds"),hour,min,sec);
		str=(StrPtr)st;
		return;
	}
	if (tpe==2)
	{
		CString st;
		st.Format(_qstr("%.4d-%.2d-%.2d"),year,month,day);
		str=(StrPtr)st;
		return;
	}

	CString st;
	st.Format(_qstr("%.4d-%.2d-%.2d %.2dh%.2dm%.2ds"),year,month,day,hour,min,sec);
	str=(StrPtr)st;
}

void TSC_time::fromstring(StrPtr str)
{
}


void TSC_time::fillwithnowUT()
{
	struct tm *ltm;
	time_t long_time;
	int yr;
	time(&long_time);
	ltm=gmtime(&long_time);
	yr=ltm->tm_year;
	if (yr<95) yr+=100;
	yr+=1900;
	year=yr;
	month=ltm->tm_mon+1;
	day=ltm->tm_mday;
	hour=ltm->tm_hour;
	min=ltm->tm_min;
	sec=ltm->tm_sec;
	calcidx();
}


void TSC_time::operator=(const TSC_time &v)
{
	idx1=v.idx1;
	idx2=v.idx2;
	idx3=v.idx3;
	calccal();
}

bool TSC_time::compare(TSC_time *v1, TSC_time *v2)
{
	return (v1->idx1==v2->idx1)&&(v1->idx2==v2->idx2)&&(v1->idx3=v2->idx3);
}

bool TSC_time::comparesize(TSC_time *v1, TSC_time *v2)
{
	if (v1->idx1<v2->idx1) return true;
	if (v1->idx2<v2->idx2) return true;
	if (v1->idx3<v2->idx3) return true;
	return false;
}



void TSC_time::streamout(QBinTagWriter &writer)
{
	writer.write_int32(idx1);
	writer.write_int32(idx2);
	writer.write_int32(idx3);
}
void TSC_time::streamin(QBinTagReader &reader)
{
	idx1=reader.read_int32();
	idx2=reader.read_int32();
	idx3=reader.read_int32();
	calccal();
}



TSC_chrono::TSC_chrono()
{
	LARGE_INTEGER ifreq;
	QueryPerformanceFrequency(&ifreq);
	rendertimerfreq=(double)ifreq.QuadPart;
	pauzed=false;
	timeroffset=0;

	start();
}
void TSC_chrono::tostring(QString &str, int tpe) const
{
	str=TFormatString(_qstr("^1s"),G_elapsed());
}

void TSC_chrono::fromstring(StrPtr str)
{
	throw QError(_text("Unable to convert chrono from string"));
}

void TSC_chrono::operator=(const TSC_chrono &v)
{
	startidx=v.startidx;
}


void TSC_chrono::streamout(QBinTagWriter &writer)
{
	throw QError(_text("Unable to stream chrono objects"));
}
void TSC_chrono::streamin(QBinTagReader &reader)
{
	throw QError(_text("Unable to stream chrono objects"));
}

void TSC_chrono::start()
{
	timeroffset=0;pauzed=false;
	QueryPerformanceCounter(&startidx);
}

double TSC_chrono::G_elapsed() const
{
	if (!pauzed)
	{
		LARGE_INTEGER curidx;
		QueryPerformanceCounter(&curidx);
		return timeroffset+(curidx.QuadPart-startidx.QuadPart)/rendertimerfreq;
	}
	else return timeroffset;
}

void TSC_chrono::pauze()
{
	timeroffset=G_elapsed();
	pauzed=true;
}

void TSC_chrono::resume()
{
	pauzed=false;
	QueryPerformanceCounter(&startidx);
}

void TSC_chrono::set(double ival)
{
	timeroffset=ival;
	QueryPerformanceCounter(&startidx);
}


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_time,time)
{
	setreturntype(SC_valname_time);
	setclasspath_fromtype(SC_valname_time);
	addvar(_qstr("year"),SC_valname_scalar,false);
	addvar(_qstr("month"),SC_valname_scalar,false);
	addvar(_qstr("day"),SC_valname_scalar,false);
	addvar(_qstr("hour"),SC_valname_scalar,false);
	addvar(_qstr("min"),SC_valname_scalar,false);
	addvar(_qstr("sec"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(retval);
	if (arglist->G_ispresent(0)) time->Set_year(arglist->get(0)->G_content_scalar()->G_intval());
	if (arglist->G_ispresent(1)) time->Set_month(arglist->get(1)->G_content_scalar()->G_intval());
	if (arglist->G_ispresent(2)) time->Set_day(arglist->get(2)->G_content_scalar()->G_intval());
	if (arglist->G_ispresent(3)) time->Set_hour(arglist->get(3)->G_content_scalar()->G_intval());
	if (arglist->G_ispresent(4)) time->Set_min(arglist->get(4)->G_content_scalar()->G_intval());
	if (arglist->G_ispresent(5)) time->Set_sec(arglist->get(5)->G_content_scalar()->G_intval());
}
ENDFUNCTION(func_time_time)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_timeJD,timeJD)
{
	setreturntype(SC_valname_time);
	setclasspath_fromtype(SC_valname_time);
	addvar(_qstr("JD"),SC_valname_scalar,false);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(retval);
	time->Set_JD(arglist->get(0)->G_content_scalar()->G_val());
}
ENDFUNCTION(func_time_timeJD)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_timeCurrent,CurrentTimeUT)
{
	setreturntype(SC_valname_time);
	setclasspath_fromtype(SC_valname_time);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(retval);
	time->fillwithnowUT();
}
ENDFUNCTION(func_time_timeCurrent)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_JD,JD)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_time);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(owner);
	if (assigntoval!=NULL) time->Set_JD(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(time->G_JD());
}
ENDFUNCTION(func_time_JD)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_year,Year)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_time);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(owner);
	if (assigntoval!=NULL) time->Set_year(assigntoval->G_content_scalar()->G_intval());
	retval->G_content_scalar()->copyfrom(time->G_year());
}
ENDFUNCTION(func_time_year)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_month,Month)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_time);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(owner);
	if (assigntoval!=NULL) time->Set_month(assigntoval->G_content_scalar()->G_intval());
	retval->G_content_scalar()->copyfrom(time->G_month());
}
ENDFUNCTION(func_time_month)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_day,Day)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_time);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(owner);
	if (assigntoval!=NULL) time->Set_day(assigntoval->G_content_scalar()->G_intval());
	retval->G_content_scalar()->copyfrom(time->G_day());
}
ENDFUNCTION(func_time_day)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_hour,Hour)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_time);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(owner);
	if (assigntoval!=NULL) time->Set_hour(assigntoval->G_content_scalar()->G_intval());
	retval->G_content_scalar()->copyfrom(time->G_hour());
}
ENDFUNCTION(func_time_hour)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_min,Min)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_time);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(owner);
	if (assigntoval!=NULL) time->Set_min(assigntoval->G_content_scalar()->G_intval());
	retval->G_content_scalar()->copyfrom(time->G_min());
}
ENDFUNCTION(func_time_min)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_sec,Sec)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_time);
	setcanassign();
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(owner);
	if (assigntoval!=NULL) time->Set_sec(assigntoval->G_content_scalar()->G_val());
	retval->G_content_scalar()->copyfrom(time->G_sec());
}
ENDFUNCTION(func_time_sec)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_addsec,AddSeconds)
{
	setmemberfunction(SC_valname_time);
	addvar(_qstr("seconds"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(owner);
	time->addseconds(arglist->get(0)->G_content_scalar()->G_val());
}
ENDFUNCTION(func_time_addsec)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_adddays,AddDays)
{
	setmemberfunction(SC_valname_time);
	addvar(_qstr("days"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(owner);
	time->adddays(arglist->get(0)->G_content_scalar()->G_val());
}
ENDFUNCTION(func_time_adddays)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_diffdays,DiffDays)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_time);
	addvar(_qstr("reftime"),SC_valname_time);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(owner);
	TSC_time *time0=G_valuecontent<TSC_time>(arglist->get(0));
	retval->G_content_scalar()->copyfrom(time->diffdays(*time0));
}
ENDFUNCTION(func_time_diffdays)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_time_diffsecs,DiffSecs)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_time);
	addvar(_qstr("reftime"),SC_valname_time);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_time *time=G_valuecontent<TSC_time>(owner);
	TSC_time *time0=G_valuecontent<TSC_time>(arglist->get(0));
	retval->G_content_scalar()->copyfrom(time->diffsecs(*time0));
}
ENDFUNCTION(func_time_diffsecs)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_min_time,9,SC_valname_time,SC_valname_time,SC_valname_scalar)
{
	setclasspath_fromtype(SC_valname_time);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	TSC_time *time1=G_valuecontent<TSC_time>(val1);
	TSC_time *time0=G_valuecontent<TSC_time>(val2);
	retval->G_content_scalar()->copyfrom(time1->diffsecs(*time0));
}
ENDOPERATOR(op_min_time)


///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_time_addscalar,8,SC_valname_time,SC_valname_scalar,SC_valname_time)
{
	setclasspath_fromtype(SC_valname_time);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	TSC_time *time1=G_valuecontent<TSC_time>(val1);
	double vl=val2->G_content_scalar()->G_val();
	*G_valuecontent<TSC_time>(retval)=*time1;
	G_valuecontent<TSC_time>(retval)->addseconds(vl);
}
ENDOPERATOR(op_time_addscalar)

///////////////////////////////////////////////////////////////////////////////////
OPERATOR(op_time_subtrscalar,9,SC_valname_time,SC_valname_scalar,SC_valname_time)
{
	setclasspath_fromtype(SC_valname_time);
}
void execute_implement(TSC_value *val1, TSC_value *val2, TSC_value *retval)
{
	TSC_time *time1=G_valuecontent<TSC_time>(val1);
	double vl=val2->G_content_scalar()->G_val();
	*G_valuecontent<TSC_time>(retval)=*time1;
	G_valuecontent<TSC_time>(retval)->addseconds(-vl);
}
ENDOPERATOR(op_time_subtrscalar)




///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_chrono_start,Start)
{
	setmemberfunction(SC_valname_chrono);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_chrono *chrono=G_valuecontent<TSC_chrono>(owner);
	chrono->start();
}
ENDFUNCTION(func_chrono_start)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_chrono_pauze,Pauze)
{
	setmemberfunction(SC_valname_chrono);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_chrono *chrono=G_valuecontent<TSC_chrono>(owner);
	chrono->pauze();
}
ENDFUNCTION(func_chrono_pauze)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_chrono_resume,Resume)
{
	setmemberfunction(SC_valname_chrono);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_chrono *chrono=G_valuecontent<TSC_chrono>(owner);
	chrono->resume();
}
ENDFUNCTION(func_chrono_resume)

///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_chrono_set,Set)
{
	setmemberfunction(SC_valname_chrono);
	addvar(_qstr("time"),SC_valname_scalar);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_chrono *chrono=G_valuecontent<TSC_chrono>(owner);
	double vl=arglist->get(0)->G_content_scalar()->G_val();
	chrono->set(vl);
}
ENDFUNCTION(func_chrono_set)


///////////////////////////////////////////////////////////////////////////////////
FUNCTION(func_chrono_elapsed,Elapsed)
{
	setreturntype(SC_valname_scalar);
	setmemberfunction(SC_valname_chrono);
}
void execute_implement(TSC_funcarglist *arglist, TSC_value *retval, TSC_value *assigntoval, TSC_value *owner)
{
	TSC_chrono *chrono=G_valuecontent<TSC_chrono>(owner);
	retval->G_content_scalar()->copyfrom(chrono->G_elapsed());
}
ENDFUNCTION(func_chrono_elapsed)
