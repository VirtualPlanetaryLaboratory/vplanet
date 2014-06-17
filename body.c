/******************* BODY.C ***********************/
/*
 * Rory Barnes, Wed May  7 14:40:51 PDT 2014
 *
 * This file contains subroutines that describe 
 * physical properties of any body. This include
 * conversions between the option parameter (a property
 * that may be used at input) and the system parameter 
 * (the property in the BODY struct that is always 
 * up-to-date). If unsure between here and orbit.c, put 
 * here. Also includes mathemtatical relationships.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "vplanet.h"


/* 
 * Mathematical Relationships 
 */

int fiSign(double x) {
  double foo;

  if (x != 0) 
    foo = x/fabs(x);
  else 
    foo = 0;
  return foo;
}

double fdDPerDt(double dRotRate, double dDrotrateDt) {
    return -2*PI*dDrotrateDt/(dRotRate*dRotRate);
}

double fdTimescale(double dVar,double dDeriv) {
  if (dDeriv != 0)
    return fabs(dVar/dDeriv);
  else
    return 0;
}

/* Convert an angular frequency to a period */
double fdFreqToPer(double dFreq) {
  return 2*PI/dFreq;
}

/* Convert a period to an angular frequency */
double fdPerToFreq(double dPeriod) {
  return 2*PI/dPeriod;
}

/* 
 * Physical Relationships 
 */

double fdBodyPotEnergy(BODY body) {
  return -0.6*BIGG*body.dMass*body.dMass/body.dRadius;
}

double fdRotAngMom(double dRadGyra,double dMass,double dRad,double dOmega) {
  return dRadGyra*dRadGyra*dMass*dRad*dRad*dOmega;
}

double fdRotKinEnergy(double dMass,double dRadius,double dRadGyra,double dOmega) {
  return 0.5*dRadGyra*dRadGyra*dMass*dRadius*dRadius*dOmega*dOmega;
}

double fdRadiusFreqToRotVel(double dRadius,double dFreq) {
  return dRadius*dFreq;
}

double fdRadiusRotVelToFreq(double dRotVel,double dRadius){
  return dRotVel/dRadius;
}

double fdDensityMassToRadius(double dDensity,double dMass) {
  return pow( (3*dDensity/(4*PI*dMass)), (1./3) );
}

double fdMassFromRadiusDensity(double dRadius,double dDensity){
  return 4*PI*pow(dRadius,3)/(3*dDensity);
}

double fdRotVel(double dRadius,double dRotRate) {
  return dRadius*dRotRate;
}


/*
 * Published Mass - Radius Relationships
 */

/* Stellar mass-radius relationships from New Light on 
 * Dark Stars, Table 4.1. 
 * See Barnes et al. (2013) Astrobiology 13:225-250.  */

double fdRadToMass_ReidHawley(double dRadius) {
    double x,y;
    
    x = log10(dRadius/RSUN);
    y = 0.1277 + 2.185*x + 3.135*x*x + 1.9031*x*x*x;
    
    return pow(10,y)*MSUN;
}

double fdMassToRad_ReidHawley(double dMass) {
  double x,y;

  x = log10(dMass/MSUN);
  y = 0.1424 + 1.568*x - 0.2342*x*x - 0.5581*x*x*x;
  
  return pow(10,y)*RSUN;
}

/* Stellar mass-radius relationship from
 * Gorda, S. Yu. & Svechnikov, M. A. 1999, Astronomy 
 * Reports, 43, 521-525 */ 

double fdMassToRad_GordaSvech99(double dMass) {
  dMass = log10(dMass/MSUN);

  if (dMass > 0.14)
    return pow(10,(0.096 + 0.652*log10(dMass)))*RSUN;
  else
    return pow(10,(0.1 + 1.03*log10(dMass)))*RSUN;
}

/* Reverse fit from
 * Barnes et al. (2013) Astrobiology 13:225-250.  */

double fdRadToMass_GordaSvech99(double dRadius) {
    double x,y;
    
    x = log10(dRadius/RSUN);
    y = -0.09709 + 0.9709*x - 2.502e-5*x*x - 1.34e-5*x*x*x;
    
    return pow(10,y);
}


/* Stellar mass-radius relationships from 
 * Bayless, A.J. & Orosz, J.A. 2006, ApJ, 651, 1155-1165 */

double fdMassToRad_BaylessOrosz06(double dMass) {
    double dRadius;
    
    dMass = dMass/MSUN;
    dRadius = 0.0324 + 0.9343*dMass + 0.0374*dMass*dMass;
    
    return dRadius*RSUN;
}

double fdRadToMass_BaylessOrosz06(double dRadius) {
    double dMass;

    dRadius = dRadius/RSUN;
    dMass = -0.03477 + 1.07146*dRadius - 8.171*dRadius*dRadius -0.0412*dRadius*dRadius*dRadius;

    return dMass*MSUN;
}


/* Terrestrial planet mass-radius relationships from 
 * Sotin et al 2007, Icarus, 191, 337-351. */

double fdMassToRad_Sotin07(double dMass) {
    return pow(dMass/MEARTH,0.272)*REARTH;
}

double fdRadToMass_Sotin07(double dRadius) {
    return pow(dRadius/REARTH,3.6765)*MEARTH;
}

double fdMassToRad(double dMass,double iRelation) {

  if (iRelation == 0) 
    return fdMassToRad_ReidHawley(dMass);
  else if (iRelation == 1)
    return fdMassToRad_GordaSvech99(dMass);
  else if (iRelation == 2)
    return fdMassToRad_BaylessOrosz06(dMass);
  else if (iRelation == 3)
    return fdMassToRad_Sotin07(dMass);
 
  /* Need to add more! */
 
  /* Whoops! */
  return 1./0;
}

double fdRadToMass(double dMass,double iRelation) {

  if (iRelation == 0) 
    return fdRadToMass_ReidHawley(dMass);
  else if (iRelation == 1)
    return fdRadToMass_GordaSvech99(dMass);
  else if (iRelation == 2)
    return fdRadToMass_BaylessOrosz06(dMass);
  else if (iRelation == 3)
    return fdRadToMass_Sotin07(dMass);
 
  /* Need to add more! */
 
  /* Whoops! */
  return 1./0;
}

void fvBodyCopy(BODY *src,BODY *dest,CONTROL *control) {
  int iBody;

  for (iBody=0;iBody<control->iNumBodies;iBody++) {
    dest[iBody].dMass = src[iBody].dMass;
    dest[iBody].dRadius = src[iBody].dRadius;
    dest[iBody].dRadGyra = src[iBody].dRadGyra;
    dest[iBody].dK2 = src[iBody].dK2;
    dest[iBody].dObliquity = src[iBody].dObliquity;
    dest[iBody].dRotRate = src[iBody].dRotRate;
    if (control->iTideModel == CPL)
      dest[iBody].dTidalQ = src[iBody].dTidalQ;
    if (control->iTideModel == CTL)
      dest[iBody].dTidalTau = src[iBody].dTidalTau;
  }

  dest[1].dEcc = src[1].dEcc;
  dest[1].dSemi = src[1].dSemi;
  dest[1].dMeanMotion = src[1].dMeanMotion;
}
