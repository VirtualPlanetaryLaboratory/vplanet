
/********************** BODY.H ***********************/
/*
 * Rory Barnes, Wed May  7 13:53:38 PDT 2014
 *
 * This header file contains all the subroutines in
 * file body.c.
 *
*/

int fiSign(double);
double fdFreqToPer(double);
double fdTimescale(double,double);
double fdTimescaleMulti(double,double*,int);
double fdPerToFreq(double);
double fdRotVel(double,double);
double fdDPerDt(double,double);

double fdBodyPotEnergy(BODY);
double fdRotAngMom(double,double,double,double);
double fdRotKinEnergy(double,double,double,double);

double fdDensityMassToRadius(double,double);
double fdMassFromRadiusDensity(double,double);
double fdMassToRad(double,double);
double fdRadToMass(double,double);
double fdRadiusFreqToRotVel(double,double);
double fdRadiusRotVelToFreq(double,double);
double fdDensityMassToRadius(double,double);
double fdMassFromRadiusDensity(double,double);

double fdMassToRad_BaylessOrosz06(double); 
double fdMassToRad_GordaSvech99(double);
double fdMassToRad_ReidHawley(double);
double fdMassToRad_Sotin07(double);
double fdRadToMass_BaylessOrosz06(double); 
double fdRadToMass_GordaSvech99(double);
double fdRadToMass_ReidHawley(double);
double fdRadToMass_Sotin07(double);

void BodyCopy(BODY*,BODY*,EVOLVE*);

void CalcXYZobl(BODY*,int);
