
/********************** BODY.H ***********************/
/*
 * Rory Barnes, Wed May  7 13:53:38 PDT 2014
 *
 * This header file contains all the subroutines in
 * file body.c.
 *
*/

// Mass-Radius identifiers
#define REIDHAWLEY        0
#define GORDASVECH99      1
#define BAYLESSOROSZ06    2
#define SOTIN07           3

int fiSign(double);
double fdFreqToPer(double);
double fdTimescale(double,double);
double fdTimescaleMulti(double,double*,int);
double fdPerToFreq(double);
double fdRotVel(double,double);
double fdDPerDt(double,double);

double fdBodyPotEnergy(double,double);
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
double fdSphereDensity(double,double);

double fdMassToRad_BaylessOrosz06(double);
double fdMassToRad_GordaSvech99(double);
double fdMassToRad_ReidHawley(double);
double fdMassToRad_Sotin07(double);
double fdRadToMass_BaylessOrosz06(double);
double fdRadToMass_GordaSvech99(double);
double fdRadToMass_ReidHawley(double);
double fdRadToMass_Sotin07(double);
double fdLehmerRadius(double Menv, double lil_g, double RadSurf, double Pxuv, double H, int toggle);

void BodyCopy(BODY*,BODY*,EVOLVE*);

void CalcXYZobl(BODY*,int);
double CalcDynEllipEq(BODY*,int);
