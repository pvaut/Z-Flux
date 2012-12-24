#include "stdafx.h"
#include <stdio.h>
#include <math.h>

#include "mmsystem.h"

#include "tools.h"

#include "joystick.h"

#include "qerror.h"




Tjoystickinfo::Tjoystickinfo()
{
	int i;
	lastrockerpos=-1;rockerpos=-1;
	for (i=0; i<JOYSTICKBUTTONCOUNT; i++)
	{
		buttondown[i]=false;
		prevbuttondown[i]=false;
		lastbuttondowntick[i]=0;
		lastbuttonuptick[i]=0;
	}
	xp=0;yp=0;zp=0;rp=0;up=0;vp=0;
	lastrockertesttick=GetTickCount();
}


bool Tjoystickinfo::read(int nr)
{
	xp=0;
	yp=0;
	zp=0;
	rp=0;

	JOYINFOEX ji;
	ji.dwSize=sizeof(ji);
	ji.dwFlags=JOY_RETURNALL|/*JOY_USEDEADZONE|*/JOY_RETURNCENTERED|JOY_RETURNPOVCTS;
	if (joyGetPosEx(nr,(&ji))!=JOYERR_NOERROR) return false;


	xp=+(ji.dwXpos-65536.0/2)/(65535.0/2.0);
	yp=-(ji.dwYpos-65536.0/2)/(65535.0/2.0);
	zp=-(ji.dwRpos-65536.0/2)/(65535.0/2.0);
	rp=+(ji.dwZpos-65536.0/2)/(65535.0/2.0);
	up=+(ji.dwUpos-65536.0/2)/(65535.0/2.0);
	vp=+(ji.dwVpos-65536.0/2)/(65535.0/2.0);

	for (int i=0; i<JOYSTICKBUTTONCOUNT; i++)
	{
		buttondoubleclick[i]=false;
		buttonshortclick[i]=false;
		buttonlongclick[i]=false;
		prevbuttondown[i]=buttondown[i];
		buttondown[i]=(ji.dwButtons&(1<<i))>0;
		if (buttondown[i])
		{
			if (((GetTickCount()-lastbuttondowntick[i]<300))&&(!prevbuttondown[i]))
				buttondoubleclick[i]=true;
			lastbuttondowntick[i]=GetTickCount();
		}
		else
		{
			if (((GetTickCount()-lastbuttonuptick[i]<500))&&(prevbuttondown[i]))
				buttonshortclick[i]=true;
			if (((GetTickCount()-lastbuttonuptick[i]>500))&&(prevbuttondown[i]))
				buttonlongclick[i]=true;
			lastbuttonuptick[i]=GetTickCount();
		}
	}

	lastrockerpos=rockerpos;
	rockerpos=-1;
	if (ji.dwPOV>=0) rockerpos=ji.dwPOV/100;

	return true;
}


double Tjoystickinfo::G_xpcorr()
{
	double o=0.1;
	if (fabs(xp)<o) return 0;
	else
	{
		if (xp>0) return sqr((xp-o)/(1-o));
		else return -sqr(-(xp+o)/(1-o));
	}
}

double Tjoystickinfo::G_ypcorr()
{
	double o=0.1;
	if (fabs(yp)<o) return 0;
	else
	{
		if (yp>0) return sqr((yp-o)/(1-o));
		else return -sqr(-(yp+o)/(1-o));
	}
}

double Tjoystickinfo::G_zpcorr()
{
	double o=0.1;
	if (fabs(zp)<o) return 0;
	else
	{
		if (zp>0) return sqr((zp-o)/(1-o));
		else return -sqr(-(zp+o)/(1-o));
	}
}

double Tjoystickinfo::G_rpcorr()
{
	double o=0.1;
	if (fabs(rp)<o) return 0;
	else
	{
		if (rp>0) return sqr((rp-o)/(1-o));
		else return -sqr(-(rp+o)/(1-o));
	}
}

double Tjoystickinfo::G_upcorr()
{
	double o=0.1;
	if (fabs(up)<o) return 0;
	else
	{
		if (up>0) return sqr((up-o)/(1-o));
		else return -sqr(-(up+o)/(1-o));
	}
}

double Tjoystickinfo::G_vpcorr()
{
	double o=0.1;
	if (fabs(vp)<o) return 0;
	else
	{
		if (vp>0) return sqr((vp-o)/(1-o));
		else return -sqr(-(vp+o)/(1-o));
	}
}

double Tjoystickinfo::G_axis(int axisnr, bool corrected)
{
	if (corrected)
	{
		if (axisnr==0) return G_xpcorr();
		if (axisnr==1) return G_ypcorr();
		if (axisnr==2) return G_zpcorr();
		if (axisnr==3) return G_rpcorr();
		if (axisnr==4) return G_upcorr();
		if (axisnr==5) return G_vpcorr();
	}
	else
	{
		if (axisnr==0) return xp;
		if (axisnr==1) return yp;
		if (axisnr==2) return zp;
		if (axisnr==3) return rp;
		if (axisnr==4) return up;
		if (axisnr==5) return vp;
	}
	throw QError(_text("Invalid joystick axis identifier"));
}



bool Tjoystickinfo::G_buttondown(int nr)
{
	return buttondown[nr];
}

bool Tjoystickinfo::G_buttonclicked(int nr)
{
	return buttondown[nr]&&(!prevbuttondown[nr]);
}

bool Tjoystickinfo::G_buttondoubleclicked(int nr)
{
	return buttondoubleclick[nr];
}

bool Tjoystickinfo::G_buttonshortclicked(int nr)
{
	return buttonshortclick[nr];
}

bool Tjoystickinfo::G_buttonlongclicked(int nr)
{
	return buttonlongclick[nr];
}

int Tjoystickinfo::G_rockerclicked()//0=none 1=up 2=right 3=down 4=left
{
	long prevtick=lastrockertesttick;
	long lastrockertesttick=GetTickCount();
	if (G_rockerposchanged()) lastrockerchanged=lastrockertesttick;
	if (G_rockerpos()==0)
	{
		if (G_rockerposchanged()) return 1;
		if ((lastrockertesttick>lastrockerchanged+350)&&(lastrockertesttick>prevtick+120)) return 1;
	}
	if (G_rockerpos()==90)
	{
		if (G_rockerposchanged()) return 2;
		if ((lastrockertesttick>lastrockerchanged+350)&&(lastrockertesttick>prevtick+120)) return 2;
	}
	if (G_rockerpos()==180)
	{
		if (G_rockerposchanged()) return 3;
		if ((lastrockertesttick>lastrockerchanged+350)&&(lastrockertesttick>prevtick+120)) return 3;
	}
	if (G_rockerpos()==270)
	{
		if (G_rockerposchanged()) return 4;
		if ((lastrockertesttick>lastrockerchanged+350)&&(lastrockertesttick>prevtick+120)) return 4;
	}

	return 0;
}


