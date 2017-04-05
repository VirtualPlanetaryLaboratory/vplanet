/************************ ORBIT.C *******************/
/*
 * Rory Barnes, Wed May  7 14:59:37 PDT 2014
 *
 * This file contains subroutines associated with
 * orbital properties. Note that element 1 in the
 * BODY array contains the up-to-date orbital
 * properties.
*/

/* where something like iBody == 0 occurs
 * ~50
 * ~74
 * ~110
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

/* Compute the total orbital angular momentum in the system
 * as J = mu*sqrt(GMA(1-e^2)) for each orbiting body
 */
double fdOrbAngMom(BODY *body, CONTROL *control) {

  double dTot = 0.0;
  double dMass; // Mass of central body or bodies if using binary and not secondary star

  int iBody;
  for(iBody = 1; iBody < control->Evolve.iNumBodies; iBody++)
  {

    // Figure out central body mass
    // If using binary, you orbit 2 stars
    if(body[iBody].bBinary)
    {
      if(iBody > 1) // Only planets orbit two stars, stars orbit barycenter
      {
        dMass = body[0].dMass + body[1].dMass;
      }
      else
      {
        dMass = body[0].dMass;
      }
    }
    else
    {
      dMass = body[0].dMass;
    }

    dTot += dMass*body[iBody].dMass*sqrt(BIGG*(dMass+body[iBody].dMass)*body[iBody].dSemi*(1.0-body[iBody].dEcc*body[iBody].dEcc))/(dMass+body[iBody].dMass);
  }

  return dTot;
}

/* Compute the total angular momentum in the system, including lost angular momentum */
double fdTotAngMom(BODY *body, CONTROL *control, SYSTEM *system) {
  double dTot = 0.0;
  int iBody;

  // Total orbital angular momentum
  dTot = fdOrbAngMom(body,control);

  // Add all rotational angular momentum, angular momentum lost
  for(iBody = 0; iBody < control->Evolve.iNumBodies; iBody++)
  {
    dTot += fdRotAngMom(body[iBody].dRadGyra,body[iBody].dMass,body[iBody].dRadius,body[iBody].dRotRate);
    dTot += body[iBody].dLostAngMom;
  }

  return dTot;
}

/*
 *
 * Energy equations
 *
 */

/*! Compute orbital potential energy neglecting planet-planet potential energy */
double fdOrbPotEnergy(BODY *body, CONTROL *control, SYSTEM *system, int iBody) {
  double dMass; // Mass of central body or bodies if using binary and not secondary star

  // Figure out central body mass
  // If using binary, you orbit 2 stars
  if(body[iBody].bBinary)
  {
    if(iBody > 1) // Only planets orbit two stars, stars orbit barycenter
    {
      dMass = body[0].dMass + body[1].dMass;
    }
    else
    {
      dMass = body[0].dMass;
    }
  }
  else
  {
    dMass = body[0].dMass;
  }

  return -BIGG*dMass*body[iBody].dMass/body[iBody].dSemi;
}

/*! Compute orbital kinetic energy of a body */
double fdOrbKinEnergy(BODY *body, CONTROL *control, SYSTEM *system, int iBody) {
  double dMass;

  // Figure out central body mass
  // If using binary, you orbit 2 stars
  if(body[iBody].bBinary)
  {
    if(iBody > 1) // Only planets orbit two stars, stars orbit barycenter
    {
      dMass = body[0].dMass + body[1].dMass;
    }
    else
    {
      dMass = body[0].dMass;
    }
  }
  else
  {
    dMass = body[0].dMass;
  }

  return 0.5*BIGG*dMass*body[iBody].dMass/body[iBody].dSemi;
}

/*! Compute total orbital energy for a given body */
double fdOrbEnergy(BODY *body, CONTROL *control, SYSTEM *system, int iBody) {

  return fdOrbKinEnergy(body,control,system,iBody) + fdOrbPotEnergy(body,control,system,iBody);
}

/*! Compute total non-orbital kinetic energy of a body  */
double fdKinEnergy(BODY *body, CONTROL *control, SYSTEM *system, int iBody) {

  return fdRotKinEnergy(body[iBody].dMass,body[iBody].dRadius,body[iBody].dRadGyra,body[iBody].dRotRate);
}

/*! Compute a body's non-orbital potential energy */
double fdPotEnergy(BODY *body, CONTROL *control, SYSTEM *system, int iBody) {

  return fdBodyPotEnergy(body[iBody].dMass,body[iBody].dRadius);
}

/*! Compute the total energy in the entire system, include lost energy */
double fdTotEnergy(BODY *body, CONTROL *control, SYSTEM *system) {
  double dTot = 0.0;
  int iBody;

  // Add all rotational angular momentum, angular momentum lost
  for(iBody = 0; iBody < control->Evolve.iNumBodies; iBody++)
  {
    // Add body's potential, kinetic energy
    dTot += fdPotEnergy(body,control,system,iBody);
    dTot += fdKinEnergy(body,control,system,iBody);

    // Add body's total orbital energy
    dTot += fdOrbEnergy(body,control,system,iBody);

    // Add back in lost energy
    dTot += body[iBody].dLostEng;
  }

  return dTot;
}

int bPrimary(BODY *body,int iBody) {
  int iBodyPert,bPrimary=1;  /* Assume primary body to start */

  for (iBodyPert=0;iBodyPert<body[iBody].iTidePerts;iBodyPert++) {
    if (body[iBody].iaTidePerts[iBodyPert] < iBody)
      bPrimary=0;
  }

  return bPrimary;
}

/* Orbital Relationships */

void CalcHK(BODY *body, int iBody) {
  body[iBody].dHecc = body[iBody].dEcc*sin(body[iBody].dLongP);
  body[iBody].dKecc = body[iBody].dEcc*cos(body[iBody].dLongP);
}

void CalcPQ(BODY *body, int iBody) {
  body[iBody].dPinc = body[iBody].dSinc*sin(body[iBody].dLongA);
  body[iBody].dQinc = body[iBody].dSinc*cos(body[iBody].dLongA);
}

/*
 *
 * Multi-body matrix equations
 *
 */

/* BINARY - EQTIDE - STELLAR COUPLING */

/*
 * Change in semi-major axis due to binary - eqtide - stellar coupling
 * Right now, applies to tidally-locked binary stars (since the angular
 * momentum from spinning-down stars must go somewhere)
 * Note: body 1 has orbital information for binary
 *
 * When circular, da/dt = 2 * a * (1/J_orb)*dJ_star/dt for angular momentum J
 * When eccentric, de/dt = (1-e^2) * (dJ_star/dt)/(J * e)
 */

/*! Compute rate of change in star's radius*/
double fdRadiusStarDt(BODY *body, int iBody)
{
  double eps = 10 * YEARDAY * DAYSEC;

  double dRadMinus = fdRadiusFunctionBaraffe(body[iBody].dAge - eps, body[iBody].dMass);
  double dRadPlus = fdRadiusFunctionBaraffe(body[iBody].dAge + eps, body[iBody].dMass);
  return (dRadPlus - dRadMinus) /  (2. * eps);
}

/*! Compute change in star's angular momentum due to magnetic braking */
double fdJBrakingStarDt(BODY *body, int iBody)
{
  double Jdot;
  double dOmegaCrit;

  // Now, let's calculate dJ/dt due to magnetic braking
  // This is from Reiners & Mohanty (2012); see eqn. (2.14) in Miles Timpe's Master's Thesis
  // This dJ/dt takes angular momentum from star, star can't lose it, so orbit does
  if (body[iBody].dMass > 0.35 * MSUN) dOmegaCrit = RM12OMEGACRIT;
  else dOmegaCrit = RM12OMEGACRITFULLYCONVEC;
  if (body[iBody].iWindModel == STELLAR_MODEL_REINERS) {
    if (body[iBody].dRotRate >= dOmegaCrit) {
      Jdot = -RM12CONST * body[iBody].dRotRate * pow(body[iBody].dRadius, 16. / 3.)
                        * pow(body[iBody].dMass, -2. / 3);
    } else {
      Jdot = -RM12CONST * pow(body[iBody].dRotRate / dOmegaCrit, 4.) * body[iBody].dRotRate
                        * pow(body[iBody].dRadius, 16. / 3.) * pow(body[iBody].dMass, -2. / 3);
    }
  }

  return Jdot;
}

/* Compute rate of change of semi-major axis for tidally locked star(s)
 * if iNumLocked = 2, both are locked, otherwise, iBody gives body struct
 * index for star that is tidally locked
 */
double fdSemiTidalLockBinEqSt(BODY *body, int iNumLocked, int iBody)
{
  double adot = 0.0;
  double Jdot;
  double R1dot, R2dot, Rdot;
  double num, denom, tmp;
  double M = body[0].dMass + body[1].dMass;
  double mu = body[0].dMass*body[1].dMass/M;

  /* XXX BROKEN XXX */
  return 0.0;

  // Both tidally locked
  if(iNumLocked > 1)
  {
    // Compute change in angular momentum due to magnetic braking for both stars
    Jdot = fdJBrakingStarDt(body,0);
    Jdot += fdJBrakingStarDt(body,1);

    // Compute star's change in radii
    R1dot = fdRadiusStarDt(body,0);
    R2dot = fdRadiusStarDt(body,1);

    tmp = body[0].dMass*body[0].dRadGyra*body[0].dRadGyra*body[0].dRadius*R1dot;
    tmp += body[1].dMass*body[1].dRadGyra*body[1].dRadGyra*body[1].dRadius*R2dot;

    num = Jdot - 2.0*body[0].dRotRate*tmp;

    tmp = body[0].dMass*body[0].dRadGyra*body[0].dRadGyra*body[0].dRadius*body[0].dRadius;
    tmp += body[1].dMass*body[1].dRadGyra*body[1].dRadGyra*body[1].dRadius*body[1].dRadius;
    tmp *= 3.0*BIGG*M/(2.0*body[0].dRotRate*pow(body[1].dSemi,4));
    denom = mu*BIGG*M/(2.0*sqrt(BIGG*M*body[1].dSemi)) - tmp;

    adot = num/denom;
  }
  // Just one is tidally locked
  else
  {
    // Compute change in angular momentum due to magnetic braking for tidally-locked star
    Jdot = fdJBrakingStarDt(body,iBody);

    // Compute star's change in radii
    Rdot = fdRadiusStarDt(body,iBody);

    tmp = body[iBody].dMass*body[iBody].dRadGyra*body[iBody].dRadGyra*body[iBody].dRadius*Rdot;

    num = Jdot - 2.0*body[iBody].dRotRate*tmp;

    tmp = body[iBody].dMass*body[iBody].dRadGyra*body[iBody].dRadGyra*body[iBody].dRadius*body[iBody].dRadius;
    tmp *= 3.0*BIGG*M/(2.0*body[iBody].dRotRate*pow(body[1].dSemi,4));
    denom = mu*BIGG*M/(2.0*sqrt(BIGG*M*body[1].dSemi)) - tmp;

    adot = num/denom;
  }

  return adot;
}

/*! Compute change in binary semi-major axis when circular, tidally locked
 * and BINARY, EQTIDE, and STELLAR are active
 */
double fdSemiDtEqBinSt(BODY *body, SYSTEM *system, int *iaBody) {
  // iaBody [0] is ALWAYS the current body: one of the stars
  int iBody = iaBody[0]; // Secondary body
  int iTmp;
  double adot = 0.0;

  // XXX Probably broken XXX
  return 0.0;

  // If orbit isn't circular, pass
  //if(body[iBody].dEcc > TINY)
  if(0) // HACK
  {
    return 0.0;
  }
  // Circular orbit! If tidally locked, do stuff!
  else
  {
    // Both are tidally locked
    if(body[0].bTideLock && body[1].bTideLock)
    {
      adot = fdSemiTidalLockBinEqSt(body,2,-1);
    }
    // Primary is tidally locked
    else if(body[0].bTideLock && !body[1].bTideLock)
    {
      adot = fdSemiTidalLockBinEqSt(body,1,0);
    }
    // Secondary is tidally locked
    else if(!body[0].bTideLock && body[1].bTideLock)
    {
      adot = fdSemiTidalLockBinEqSt(body,1,1);
    }
    else
    {
      adot = 0.0;
    }

    return adot;
  }
}

/*! Compute change in binary Ecc when eccentric, tidally locked
 * and BINARY, EQTIDE, and STELLAR are active.  Note: This isn't added to matrix
 * but instead computes the de/dt needed for Hecc, Kecc derivatives
 */
double fdEccDtEqBinSt(BODY *body, SYSTEM *system, int *iaBody) {

  /* XXX BROKEN (probably) XXX */

  return 0.0;
  // iaBody [0] is ALWAYS the current body: one of the stars
  int iBody = iaBody[0]; // Secondary body
  int iTmp; // for loop index

  // If orbit is circular, pass
  if(body[iBody].dEcc < TINY)
  {
    return 0.0;
  }
    // Eccentric orbit! If tidally locked, do stuff!
    else
    {
    // Compute current orbital angular momentum
    double M = body[0].dMass + body[1].dMass;
    double mu = body[0].dMass*body[1].dMass/M;
    double J = mu*sqrt(BIGG*M*body[1].dSemi*(1.0-body[1].dEcc*body[1].dEcc));

    // Initial change in angular momentum is null
    double Jdot = 0.0;

    // Loop over stars, Only exchange angular momemntum with orbit if tidally locked
    for(iTmp = 0; iTmp < 2; iTmp++)
    {
      if(body[iBody].bTideLock)
      {
        Jdot += fdJStarDt(body,iTmp);
      }
      // Not tidally locked, do nothing
      else
      {
        continue;
      }
    }

    // Compute, return change in eccentricity
    // since decrease in angular momentum makes orbit more eccentric
    return -(1.0-body[iBody].dEcc*body[iBody].dEcc)*Jdot/(J*body[iBody].dEcc);
  }
}


/*! Compute change in binary Hecc when eccentric, tidally locked
 * and BINARY, EQTIDE, and STELLAR are active.
 */
double fdHeccDtEqBinSt(BODY *body, SYSTEM *system, int *iaBody) {

  // Get change in eccentricity
  double dedt = fdEccDtEqBinSt(body,system,iaBody);

  // Return change in Hecc
  return dedt * sin(body[iaBody[0]].dLongP);
}

/*! Compute change in binary Kecc when eccentric, tidally locked
 * and BINARY, EQTIDE, and STELLAR are active.
 */
double fdKeccDtEqBinSt(BODY *body, SYSTEM *system, int *iaBody) {

  // Get change in eccentricity
  double dedt = fdEccDtEqBinSt(body,system,iaBody);

  // Return change in Kecc
  return dedt * cos(body[iaBody[0]].dLongP);
}
