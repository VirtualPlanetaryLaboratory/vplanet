/**
  @file evolve.h

  @brief This file contains all the core VPLANET integration routines including
  the timestepping algorithm and the Runge-Kutta Integration scheme.

  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

  @date May 2014

*/

/* 0 => Not input by user, verify assigns default */
#define EULER 1
#define RUNGEKUTTA 2

/* @cond DOXYGEN_OVERRIDE */

void PropertiesAuxiliary(BODY *, CONTROL *, SYSTEM *, UPDATE *);
void fdGetUpdateInfo(BODY *, CONTROL *, SYSTEM *, UPDATE *,
                     fnUpdateVariable ***);
double fdGetTimeStep(BODY *, CONTROL *, SYSTEM *, UPDATE *,
                     fnUpdateVariable ***);
void CalculateDerivatives(BODY *, SYSTEM *, UPDATE *, fnUpdateVariable ***,
                          int);

void Evolve(BODY *, CONTROL *, FILES *, MODULE *, OUTPUT *, SYSTEM *, UPDATE *,
            fnUpdateVariable ***, fnWriteOutput *, fnIntegrate);

void EulerStep(BODY *, CONTROL *, SYSTEM *, UPDATE *, fnUpdateVariable ***,
               double *, int);

void RungeKutta4Step(BODY *, CONTROL *, SYSTEM *, UPDATE *,
                     fnUpdateVariable ***, double *, int);

/* @endcond */
