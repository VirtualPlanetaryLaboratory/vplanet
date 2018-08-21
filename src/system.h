/**
  @file system.h
  @brief Subroutines that control system-wide properties.
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date May 7 2014
*/

/* @cond DOXYGEN_OVERRIDE */

double fdSemiToPeriod(double,double);
double fdPeriodToSemi(double,double);
double fdSemiToMeanMotion(double,double);
double fdMeanMotionToSemi(double,double,double);
double fdMeanMotionToSemi(double,double,double);

double * fdOrbAngMom(BODY*,CONTROL*,int);
double fdTotAngMom(BODY*,CONTROL*,SYSTEM*);

double fdOrbPotEnergy(BODY*,CONTROL*,SYSTEM*,int);
double fdOrbKinEnergy(BODY*,CONTROL*,SYSTEM*,int);
double fdOrbEnergy(BODY*,CONTROL*,SYSTEM*,int);
double fdTotOrbEnergy(BODY*, CONTROL*, SYSTEM*);
double fdKinEnergy(BODY*,CONTROL*,SYSTEM*,int);
double fdPotEnergy(BODY*,CONTROL*,SYSTEM*,int);
double fdTotEnergy(BODY*,CONTROL*,SYSTEM*);

int bPrimary(BODY*,int);

void CalcHK(BODY*,int);
void CalcPQ(BODY*,int);

/* Multi-body derivative equations */
double fdSemiDtEqSt(BODY*,SYSTEM*,int*);
double fdJStarDt(BODY*,int);
double fdJBrakingStarDt(BODY*,int);
double fdRadiusStarDt(BODY*,int);
double fdSemiTidalLockBinEqSt(BODY*,int,int);
double fndUpdateSpiNBodyCoords(BODY*,EVOLVE*);

/* @endcond */
