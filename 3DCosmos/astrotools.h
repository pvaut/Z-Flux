#pragma once

#define h2rad 0.261799387
#define deg2rad 0.0174532925199


//radius of the Earth (in km)
#define c_RE 6378.0
//mass of the Earth (in kg)
#define c_ME 5.977E24
//number of kilometers in one astronomical unit
#define au2km 149598000.0

//Newton's Universal gravity constant, in N M^2 / kg^2
#define gravconst 6.668e-11


//Earth inclination
#define eps 23.4457889
//#define coseps 0.91748
//#define sineps 0.39777
#define coseps 0.9174369381
#define sineps 0.3978812030


void reduc360(double *ang);
void equat2eclipt( double alpha, double delta, double& ecl1, double& ecl2);


