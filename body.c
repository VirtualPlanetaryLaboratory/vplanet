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
 * here. Also includes mathemtatical relationships
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


// Return the sign (-1,0, or 1).
int fiSign(double dValue) {
  int iSign;

  if (fabs(dValue) > EPS) // EPS set in vplanet.h
    iSign = (int)(dValue/fabs(dValue));
  else
    iSign = 0;

  return iSign;
}

// Covnert a rotation rate derivative to a period derivative
double fdDPerDt(double dRotRate,double dDrotrateDt) {
    return -2*PI*dDrotrateDt/(dRotRate*dRotRate);
}

// Caclulate the characteristic timescale of a variable (x/(dx/dt))
double fdTimescale(double dVar,double dDeriv) {
  if (dDeriv != 0)
    return fabs(dVar/dDeriv);
  else
    return 0;
}

// Timescale for multi-module process (x/Sum(dx/dt))
double fdTimescaleMulti(double dVar,double *dDeriv,int iNum) {
  double dTime;
  int iPert;

  dTime=0;
  for (iPert=0;iPert<iNum;iPert++) {
    if (dDeriv[iPert] != 0)
      dTime += dDeriv[iPert]; // Note that here dTime is actullay the rate
    dTime = fabs(dVar/dTime);
  }
  return dTime;
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

// A body's gravitational potential energy
double fdBodyPotEnergy(double dMass, double dRadius) {
  /* ALPHA_STRUCT  is structural constant for spherical mass distribution
     potential energy (E_pot = -ALPHA*BIGG*M^2/R = 0.6), see vplanet.h. */
  return -ALPHA_STRUCT*BIGG*dMass*dMass/dRadius;
}

// A body's rotational angular momentum
double fdRotAngMom(double dRadGyra,double dMass,double dRad,double dOmega) {
  return dRadGyra*dRadGyra*dMass*dRad*dRad*dOmega;
}

// A body's rotational kinetic energy
double fdRotKinEnergy(double dMass,double dRadius,double dRadGyra,double dOmega) {
  return 0.5*dRadGyra*dRadGyra*dMass*dRadius*dRadius*dOmega*dOmega;
}

// Convert a rotational frequency to a rotational velocity
double fdRadiusFreqToRotVel(double dRadius,double dFreq) {
  return dRadius*dFreq;
}

// Convert a rotational velocity to a frequency
double fdRadiusRotVelToFreq(double dRotVel,double dRadius){
  return dRotVel/dRadius;
}

// Calculate radius from density and mass
double fdDensityMassToRadius(double dDensity,double dMass) {
  return pow( (3*dDensity/(4*PI*dMass)), (1./3) );
}

// Calculate Mass from Radius and density
double fdMassFromRadiusDensity(double dRadius,double dDensity){
  return 4*PI*pow(dRadius,3)/(3*dDensity);
}

// Calculate rotational velocity from radius and rotation frequency
double fdRotVel(double dRadius,double dRotRate) {
  return dRadius*dRotRate;
}

// Calculate density for a uniform sphere
double fdSphereDensity(double dMass,double dRadius) {
  return 4*PI*dMass/pow(dRadius,3)/3;
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

// Assign Radius based on mass and published relationship
double fdMassToRad(double dMass,double iRelation) {

  if (iRelation == REIDHAWLEY)
    return fdMassToRad_ReidHawley(dMass);
  else if (iRelation == GORDASVECH99)
    return fdMassToRad_GordaSvech99(dMass);
  else if (iRelation == BAYLESSOROSZ06)
    return fdMassToRad_BaylessOrosz06(dMass);
  else if (iRelation == SOTIN07)
    return fdMassToRad_Sotin07(dMass);

  /* Need to add more! */

  /* Whoops! */
  return 1./0;
}

// Assign mass from radius and published relationship
double fdRadToMass(double dMass,double iRelation) {

  if (iRelation == REIDHAWLEY)
    return fdRadToMass_ReidHawley(dMass);
  else if (iRelation == GORDASVECH99)
    return fdRadToMass_GordaSvech99(dMass);
  else if (iRelation == BAYLESSOROSZ06)
    return fdRadToMass_BaylessOrosz06(dMass);
  else if (iRelation == SOTIN07)
    return fdRadToMass_Sotin07(dMass);

  /* Need to add more! XXX */

  /* Whoops! */
  return 1./0;
}

// Copy the body struct from src to dest
void BodyCopy(BODY *dest,BODY *src,EVOLVE *evolve) {
  int iBody,iModule;

  /* This subroutine only includes parameters needed for more than 1 module,
     Module-specific parameters belong in the fnBodyCopy subroutines. */

  for (iBody=0;iBody<evolve->iNumBodies;iBody++) {
    dest[iBody].iBodyType = src[iBody].iBodyType;
    dest[iBody].dMass = src[iBody].dMass;
    dest[iBody].dRadius = src[iBody].dRadius;
    dest[iBody].dRadGyra = src[iBody].dRadGyra;
    dest[iBody].dXobl = src[iBody].dXobl;
    dest[iBody].dYobl = src[iBody].dYobl;
    dest[iBody].dZobl = src[iBody].dZobl;
    dest[iBody].dRotRate = src[iBody].dRotRate;
    dest[iBody].dAge = src[iBody].dAge;
    dest[iBody].dEcc = src[iBody].dEcc; // XXX iBody=0 could be in galhabit?
    dest[iBody].dPrecA = src[iBody].dPrecA;
    dest[iBody].dObliquity = src[iBody].dObliquity;
    dest[iBody].dLostAngMom = src[iBody].dLostAngMom;
    dest[iBody].dLostEng = src[iBody].dLostEng;

    //dest[iBody].dLXUV = src[iBody].dLXUV;

    if (iBody > 0) {
      dest[iBody].dHecc = src[iBody].dHecc;
      dest[iBody].dKecc = src[iBody].dKecc;
      dest[iBody].dSemi = src[iBody].dSemi;
      dest[iBody].dRadius = src[iBody].dRadius;
      dest[iBody].dMeanMotion = src[iBody].dMeanMotion;
    }
    /* Copy module-specific properties */
    for (iModule=0;iModule<evolve->iNumModules[iBody];iModule++)
      evolve->fnBodyCopy[iBody][iModule](dest,src,evolve->iEqtideModel,evolve->iNumBodies,iBody);
  }
}

// Calculate rotational variables from obliquity and precession angle
void CalcXYZobl(BODY *body, int iBody) {
  body[iBody].dXobl = sin(body[iBody].dObliquity)*cos(body[iBody].dPrecA);
  body[iBody].dYobl = sin(body[iBody].dObliquity)*sin(body[iBody].dPrecA);
  body[iBody].dZobl = cos(body[iBody].dObliquity);
}

/* Calculate equilibrium shape of planet using scaling laws and solar system
   values. If the value is less then Venus', return Venus'. */
double CalcDynEllipEq(BODY *body, int iBody) {
  double J2Earth = 1.08262668e-3, J2Venus = 4.56e-6, CEarth = 8.034e37;
  double nuEarth, EdEarth, EdVenus, dTmp, dDynEllip;

  EdEarth = J2Earth*MEARTH*pow(REARTH,2)/CEarth;
  EdVenus = J2Venus/0.336;
  nuEarth = 2*PI/(DAYSEC);

  dTmp = EdEarth*MEARTH/(pow(nuEarth,2)*pow(REARTH,3));

  dDynEllip = dTmp*pow(body[iBody].dRotRate,2)*pow(body[iBody].dRadius,3)/body[iBody].dMass;

  if (dDynEllip < EdVenus)
    dDynEllip = EdVenus;

  return dDynEllip;
}

/* Lehmer+ (2017)'s model for the radius of a planet losing its atmopshere
   due to XUV radiation. */
double fdLehmerRadius(double dMassEnv,double dGravAccel,double dRadSurf,double dPressXUV,double dScaleHeight,int iToggle) {
	double P;		  // pressure at surface due to envelope
	double Rxuv;	// radius from center of planet where optical depth of XUV is unity

	P = dGravAccel * dMassEnv / (4 * PI * dRadSurf * dRadSurf); // [kg/ms2]
	Rxuv = dRadSurf * dRadSurf / (dScaleHeight * log(dPressXUV/P) + dRadSurf);
	if (Rxuv <= dRadSurf) {
		Rxuv = dRadSurf;
	}
	if (iToggle == 1) {
		return P;
	}
	else {
		return Rxuv;
  }
}
