
/************************ ORBIT.C *******************/
/*
 * Rory Barnes, Wed May  7 14:59:37 PDT 2014
 *
 * This file contains subroutines associated with 
 * orbital properties. Note that element 1 in the 
 * BODY array contains the up-to-date orbital 
 * properties.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vplanet.h"

/* 
 * Physical Relationships 
*/

double fdMeanMotionToSemi(double dMass1,double dMass2,double dMeanMotion) {
  return pow( (BIGG*(dMass1+dMass2)/(dMeanMotion*dMeanMotion)), (1./3) );
}

/* Convert period to Semi-major axis */
double fdPeriodToSemi(double dPeriod,double dMass) {
  return pow(BIGG*dMass/(4*PI*PI)*dPeriod*dPeriod,(1.0/3));
}

double fdSemiToPeriod(double dSemi,double dMass) {
  return pow(4*PI*PI*dSemi*dSemi*dSemi/(BIGG*dMass),0.5);
}

/* Convert semi-major axis to mean motion */
double fdSemiToMeanMotion(double dSemi,double dMass) {
  return pow(BIGG*dMass/pow(dSemi,3),0.5);
}

/* 
 * Angular Momentum
 */

double fdOrbAngMom(BODY *body) {

  return body[0].dMass*body[1].dMass*pow(BIGG*body[1].dSemi*(1-body[1].dEcc*body[1].dEcc)/(body[0].dMass+body[1].dMass),0.5);
}

double fdTotAngMom(BODY *body) {
  double dTot = 0;
  int i;

  dTot = fdOrbAngMom(body);
  for (i=0;i<2;i++)
    dTot += fdRotAngMom(body[i].dRadGyra,body[i].dMass,body[i].dRadius,body[i].dRotRate);
  return dTot;
}

double fdOrbPotEnergy(double dMass1, double dMass2,double dSemi) {
  return -BIGG*dMass1*dMass2/dSemi;
}

double fdOrbKinEnergy(double dMass1,double dMass2,double dSemi) {
  return 0.5*BIGG*dMass1*dMass2/dSemi;
}

double fdOrbEnergy(BODY *body,int iBody) {
  return fdOrbKinEnergy(body[0].dMass,body[1].dMass,body[1].dSemi) + fdOrbPotEnergy(body[0].dMass,body[1].dMass,body[1].dSemi);
}
    
double fdKinEnergy(BODY *body) {
  double dKE;
  int iBody;

  dKE = fdOrbKinEnergy(body[0].dMass,body[1].dMass,body[1].dSemi);
  for (iBody=0;iBody<2;iBody++) 
    dKE += fdRotKinEnergy(body[iBody].dMass,body[iBody].dRadius,body[iBody].dRadGyra,body[iBody].dRotRate);

  return dKE;
}

double fdPotEnergy(BODY *body) {
  double dPE;
  int iBody;

  dPE = fdOrbPotEnergy(body[0].dMass,body[1].dMass,body[1].dSemi);
  for (iBody=0;iBody<2;iBody++) 
    dPE += fdBodyPotEnergy(body[iBody]);

  return dPE;
}

double fdTotEnergy(BODY *body) {
  return fdKinEnergy(body) + fdPotEnergy(body);
}

