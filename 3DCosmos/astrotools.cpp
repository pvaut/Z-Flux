#include "stdafx.h"
#include "astrotools.h"



void reduc360(double *ang)
{
	*ang=*ang-floor((*ang)/360.0)*360.0;
	for (;*ang>360.0;) *ang -= 360;
	for (;*ang<0.0;)   *ang += 360;
}


void equat2eclipt( double alpha, double delta, double& ecl1, double& ecl2)
{
	double r1,r2;

	r1=coseps*sin(delta)-sineps*cos(delta)*sin(alpha);
	ecl2=asin(r1);

	r1=cos(alpha);
	r2=sin(alpha)*coseps+tan(delta)*sineps;
	ecl1=atan2(r2,r1);
}
