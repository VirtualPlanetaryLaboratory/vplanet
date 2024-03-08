/**
  @file system.h
  @brief Subroutines that control system-wide properties.
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date May 7 2014
*/

/* @cond DOXYGEN_OVERRIDE */

double fdSemiToPeriod(double, double);
double fdPeriodToSemi(double, double);
double fdSemiToMeanMotion(double, double);
double fdMeanMotionToSemi(double, double, double);
double fdMeanMotionToSemi(double, double, double);

double *fdOrbAngMom(BODY *, CONTROL *, int);
double fdTotAngMom(BODY *, CONTROL *, SYSTEM *);

double fdOrbPotEnergy(BODY *, CONTROL *, SYSTEM *, int);
double fdOrbKinEnergy(BODY *, CONTROL *, SYSTEM *, int);
double fdOrbEnergy(BODY *, CONTROL *, SYSTEM *, int);
double fdTotOrbEnergy(BODY *, CONTROL *, SYSTEM *);
double fdKinEnergy(BODY *, CONTROL *, SYSTEM *, int);
double fdPotEnergy(BODY *, CONTROL *, SYSTEM *, int);
double fdTotEnergy(BODY *, CONTROL *, SYSTEM *);

int bPrimary(BODY *, int);

void CalcHK(BODY *, int);
void CalcPQ(BODY *, int);

void inv_plane(BODY *, SYSTEM *, int);
double fdMutualInclination(BODY *, int, int);
int fbCheckMaxMutualInc(BODY *, EVOLVE *, HALT *, IO *, int, int, int);
void kepler_eqn(BODY *, int);
void cross(double *, double *, double *);
void osc2cart(BODY *, int);
void cart2osc(BODY *, int);

double fdInstellation(BODY *, int);
double fdXUVFlux(BODY *, int);
double fdLuminosityTotal(BODY *, int);

/* Multi-body derivative equations */
double fdSemiDtEqSt(BODY *, SYSTEM *, int *);
double fdJStarDt(BODY *, int);
double fdJBrakingStarDt(BODY *, int);
double fdRadiusStarDt(BODY *, int);
double fdSemiTidalLockBinEqSt(BODY *, int, int);
double fndUpdateSpiNBodyCoords(BODY *, EVOLVE *);

void fdMergePlanet(BODY *, UPDATE *, fnUpdateVariable ***, int);

double fdOrbitalAngularMomentum(BODY *, int);
double fdTotalAngularMomentum(BODY *, CONTROL *);
double *fdaOrbitalAngularMomentumOrbFrame(BODY *, int);
// double* fdaOrbitalAngularMomentum(BODY*,CONTROL*,SYSTEM*,int);
double *fdaOrbitalAngularMomentumRefFrame(BODY *, int);
double *fdaOrbitalAngularMomentumRefFrameUnitVector(BODY *, int);
double *fdaTotalOrbitalAngularMomentumRefFrame(BODY *, CONTROL *);
double *fdaTotalOrbitalAngularMomentumRefFrameUnitVector(BODY *, CONTROL *);
double *fdaOrbitalAngularMomentumInvFrame(BODY *, int);
double *fdaTotalOrbitalAngularMomentumInvFrame(BODY *, CONTROL *);
double *fdaTotalOrbitalAngularMomentumInvFrameUnitVector(BODY *, CONTROL *);
double *fdaRotationalAngularMomentumRefFrame(BODY *, int);
double *fdaRotationalAngularMomentumRefFrameUnitVector(BODY *, int);
double *fdaTotalRotationalAngularMomentumRefFrame(BODY *, CONTROL *);
double *fdaTotalRotationalAngularMomentumRefFrameUnitVector(BODY *, CONTROL *);
double *fdaRotationalAngularMomentumInvFrame(BODY *, int);
double *fdaTotalRotationalAngularMomentumInvFrame(BODY *, CONTROL *);
double *fdaTotalRotationalAngularMomentumRefFrameUnitVector(BODY *, CONTROL *);
double *fdaTotalAngularMomentumRefFrame(BODY *, CONTROL *);
double *fdaTotalAngularMomentumRefFrameUnitVector(BODY *, CONTROL *);


void fvCassiniVectors(BODY *, CONTROL *, double *, double *, double *, double *,
                      double *, int);

/* @endcond */
