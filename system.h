/******************** ORBIT.H **********************/
/*
 * Rory Barnes, Wed May  7 13:48:54 PDT 2014
 *
 * This header file contains all the subroutines in
 * file orbit.c.
 *
*/

double fdSemiToPeriod(double,double);
double fdPeriodToSemi(double,double);
double fdSemiToMeanMotion(double,double);
double fdMeanMotionToSemi(double,double,double);
double fdMeanMotionToSemi(double,double,double);

double fdOrbAngMom(BODY *);
double fdTotAngMom(BODY *);


double fdOrbPotEnergy(double,double,double);
double fdOrbKinEnergy(double,double,double);
double fdKinEnergy(BODY*);
double fdPotEnergy(BODY*);

int bPrimary(BODY*,int);

double fdOrbEnergy(BODY*,int);
double fdTotEnergy(BODY *);

void CalcHK(BODY*,int);
void CalcPQ(BODY*,int);
