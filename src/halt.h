/**
 @file halt.h

 @brief This file contains all the checks for dynamical states
        that result in the termination of the program.

 @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

 @date Mar 7 2014

*/

#define max(a,b) \
     ({ __typeof__ (a) _a = (a); \
             __typeof__ (b) _b = (b); \
           _a > _b ? _a : _b; })

/* @cond DOXYGEN_OVERRIDE */

int fbCheckHalt(BODY*,CONTROL*,UPDATE*,fnUpdateVariable***);
void InitializeHalts(CONTROL*,MODULE*);
void VerifyHalts(BODY*,CONTROL*,MODULE*,OPTIONS*);
int fniHaltMaxEcc(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,fnUpdateVariable***,int);

/* @endcond */
