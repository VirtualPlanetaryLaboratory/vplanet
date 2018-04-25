/***************** EVOLVE.H ***********************
 *
 * Rory Barnes, 2014
 *
 * This header file contains all the subroutines in
 * file evolve.c.
 *
*/

void PropertiesAuxiliary(BODY*,CONTROL*,UPDATE*);
void fdGetUpdateInfo(BODY*,CONTROL*,SYSTEM*,UPDATE*,fnUpdateVariable***);
double fdGetTimeStep(BODY*,CONTROL*,SYSTEM*,UPDATE*,fnUpdateVariable***);

void Evolve(BODY*,CONTROL*,FILES*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,fnWriteOutput*,fnIntegrate);

void EulerStep(BODY*,CONTROL*,SYSTEM*,UPDATE*,fnUpdateVariable***,double*,int);

void RungeKutta4Step(BODY*,CONTROL*,SYSTEM*,UPDATE*,fnUpdateVariable***,double*,int);
