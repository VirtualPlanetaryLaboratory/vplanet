/**
  @file system.c
  @brief Subroutines that control system-wide properties.
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date May 7 2014
*/

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
  return pow(BIGG*dMass/(dSemi*dSemi*dSemi),0.5);
}

/*
 * Angular Momentum
 */

/*! Compute the orbital angular momentum of the iBodyth body
 * as J = mu*sqrt(GMA(1-e^2)) for each orbiting body
 */
double * fdOrbAngMom(BODY *body, CONTROL *control, int iBody) {

  double dMass, mu; // Mass of central body or bodies if using binary and not secondary star

  if (body[iBody].bSpiNBody) {
    // For SpiNBody, we just want to make L = m(r x v)
    // It is the responsibility of the caller to free this memory
    double * pdOrbMom = malloc(sizeof(double)*3);

    //Calculate the x, y, and z components of orb mom
    pdOrbMom[0] =    body[iBody].dMass * (body[iBody].dPositionY*body[iBody].dVelZ - body[iBody].dPositionZ*body[iBody].dVelY);
    pdOrbMom[1] = -1*body[iBody].dMass * (body[iBody].dPositionX*body[iBody].dVelZ - body[iBody].dPositionZ*body[iBody].dVelX);
    pdOrbMom[2] =    body[iBody].dMass * (body[iBody].dPositionX*body[iBody].dVelY - body[iBody].dPositionY*body[iBody].dVelX);
    return pdOrbMom;
  }
  else {
    // It is the responsibility of the caller to free this memory
    double * pdNetOrbMom = malloc(sizeof(double));

    // Central body (or primary binary star) doesn't orbit itself
    if(iBody < 1 || !control->bOrbiters)
    {
      *pdNetOrbMom = 0.0;
      return pdNetOrbMom;
    }

    // Figure out central body mass
    // If using binary, you orbit 2 stars
    if(body[iBody].bBinary)
    {
      if(iBody > 1) // Panets orbit two stars
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

    // Compute reduced mass
    mu = dMass*body[iBody].dMass/(dMass+body[iBody].dMass);

    *pdNetOrbMom = mu*sqrt(BIGG*(dMass+body[iBody].dMass)*body[iBody].dSemi*(1.0-body[iBody].dEcc*body[iBody].dEcc));
    return pdNetOrbMom;
  }
}

/* Compute the total angular momentum in the system, including lost angular momentum */
double fdTotAngMom(BODY *body, CONTROL *control, SYSTEM *system) {
  double dTot = 0.0;
  // Added the vectorized components of total angular momentum for SpiNBody
  double daOrbTot[] = {0.0,0.0,0.0};
  double *pdaTmp;
  int iBody, i;
  int bUsingSpiNBody=0;

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    if (body[iBody].bSpiNBody) {
      bUsingSpiNBody=1;
    }
  }

  // Add all rotational, orbital angular momentum, angular momentum lost
    //SpiNBody has direct x,y,z components for position and velocity
  if (bUsingSpiNBody){
    for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
      pdaTmp = fdOrbAngMom(body,control,iBody);
      for (i=0; i<3; i++) {
        daOrbTot[i] += pdaTmp[i];
      }
      //dTot += fdRotAngMom(body[iBody].dRadGyra,body[iBody].dMass,body[iBody].dRadius,body[iBody].dRotRate);
      //dTot += body[iBody].dLostAngMom;
    }
    dTot += sqrt(daOrbTot[0]*daOrbTot[0]+daOrbTot[1]*daOrbTot[1]+daOrbTot[2]*daOrbTot[2]);
    free(pdaTmp);
  }
  else {
    for(iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
      pdaTmp = fdOrbAngMom(body,control,iBody);
      dTot += *pdaTmp;
      dTot += fdRotAngMom(body[iBody].dRadGyra,body[iBody].dMass,body[iBody].dRadius,body[iBody].dRotRate);
      dTot += body[iBody].dLostAngMom;
      free(pdaTmp);
    }
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
  int i;
  double PotEnergy = 0;
  double Distance = dHUGE;

  if (body[iBody].bSpiNBody) {
    //For SpiNBody, find the body-body distance then return the potential.
    for (i = iBody+1; i < control->Evolve.iNumBodies; i++) {
      Distance = sqrt((body[iBody].dPositionX-body[i].dPositionX)*(body[iBody].dPositionX-body[i].dPositionX)
          +(body[iBody].dPositionY-body[i].dPositionY)*(body[iBody].dPositionY-body[i].dPositionY)
          +(body[iBody].dPositionZ-body[i].dPositionZ)*(body[iBody].dPositionZ-body[i].dPositionZ));
      PotEnergy += -BIGG*body[i].dMass*body[iBody].dMass/Distance;
    }
    return(PotEnergy);
  }
  // Ignore for central body or if there's no orbiting bodies
  if(iBody < 1 || !control->bOrbiters) {
    return 0.0;
  }

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
  double dOrbKinEnergy;

  if (body[iBody].bSpiNBody) {
    //Energy is calculated in a barycentric reference frame.
    double Velocity2 = (body[iBody].dVelX)*(body[iBody].dVelX)
        +(body[iBody].dVelY)*(body[iBody].dVelY)
        +(body[iBody].dVelZ)*(body[iBody].dVelZ);
    dOrbKinEnergy = .5*body[iBody].dMass*Velocity2;
    return(dOrbKinEnergy);
  }
  // Ignore for central body or if there's no orbiting bodies
  if(iBody < 1 || !control->bOrbiters) {
    return 0.0;
  }

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
  double dOrbEnergy;
  dOrbEnergy = fdOrbKinEnergy(body,control,system,iBody) + fdOrbPotEnergy(body,control,system,iBody);
  return (dOrbEnergy);
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
  for(iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    // Add body's potential, kinetic energy
    dTot += fdPotEnergy(body,control,system,iBody);
    dTot += fdKinEnergy(body,control,system,iBody);

    // Add body's total orbital energy
    dTot += fdOrbEnergy(body,control,system,iBody);

    // Add back in lost energy
    dTot += body[iBody].dLostEng;
    /*
    printf("%lf\n",body[iBody].dLostEng);
    printf("%lf\n",dTot);
    fflush(stdout);
    */
  }

  return dTot;
}

double fdTotOrbEnergy(BODY *body, CONTROL *control, SYSTEM *system){
  double dTot = 0.0;
  int iBody;

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++){
    dTot += fdOrbEnergy(body,control,system,iBody);
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

/* Instellation */
double fdInstellation(BODY *body,int iBody) {
  double dInstell;

  if (body[iBody].bBinary && body[iBody].iBodyType == 0) {
    // Body orbits two stars
    dInstell = fndFluxExactBinary(body,iBody,body[0].dLuminosity,body[1].dLuminosity);
  } else {
    // Body orbits one star
    if (iBody > 0)
      dInstell = body[0].dLuminosity/(4*PI*body[iBody].dSemi*body[iBody].dSemi*
        sqrt(1-body[iBody].dEcc*body[iBody].dEcc));
    else { // Central body can't have instellation (for now)
      dInstell = -1;
    }
  }

  return dInstell;
}

/**
Compute the XUV Flux.

@param body A pointer to the current BODY instance
@param iBody The current BODY index
@param iXUV Integer describing the XUV model
*/
double fdXUVFlux(BODY *body, int iBody) {

  double flux;

  if (body[iBody].bBinary && body[iBody].iBodyType == 0) {
    // Body orbits two stars
    flux = fndFluxExactBinary(body,iBody,body[0].dLXUV,body[1].dLXUV);
  } else {
    // Body orbits one star
    if (iBody > 0)
      flux = body[0].dLXUV / (4 * PI * pow(body[iBody].dSemi, 2) *
             pow((1 - body[iBody].dEcc * body[iBody].dEcc), 0.5));
    else { // Central body can't have XUV flux (for now)
      flux = -1;
    }
  }

  return flux;
}

/**
  Calculate the mutual or relative inclination of the orbital planes of iBody
  and jBody.

@param body A pointer to the current BODY instance
@param iBody The index of the BODY struct for the 1st orbit
@param jBody The index of the BODY struct for the 2nd orbit

@return The mutual inclination
*/

double fdMutualInclination(BODY *body,int iBody,int jBody) {
  double dInc1,dInc2,dLongA1,dLongA2;
  double dMutualInc;

  dInc1 = fdInclination(body,iBody);
  dInc2 = fdInclination(body,jBody);
  dLongA1 = fdLongA(body,iBody);
  dLongA2 = fdLongA(body,iBody);

  dMutualInc = acos( cos(dInc1)*cos(dInc2) + sin(dInc1)*sin(dInc2)*
      cos(dLongA1 - dLongA2) );

  return dMutualInc;
}

/**
  Check the maximum allowed mutual inclination.

@param body A pointer to the BODY instance
@param evolve A pointer to the integration EVOLVE instance
@param halt A pointer to the HALT instance
@param io A pointer to the IO instance
@param update A pointer to the UPDATE instance
@param iBody The current index in the BODY instance, irrelevant in this case
  because mutual inclination is by definition a multi-body variable
@param iReason Was the subroutine called from CheckHalts (0) or CheckProgress
  (1)?

@return TRUE if the mutual incliantion is larger than dMaxMutualInc, FALSE
  if not
*/
int fbCheckMaxMutualInc(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,int iBody,
    int jBody,int iReason) {

  double dMutualInc,dMaxMutualInc;

  if (iReason == 0) {
    // Called to check halt
    dMaxMutualInc = halt[0].dMaxMutualInc;
  } else if (iReason == 1) {
    // Called from CheckProgress
    dMaxMutualInc = io->dMaxMutualInc;
  }

  dMutualInc = fdMutualInclination(body,iBody,jBody);

  if (dMutualInc >= dMaxMutualInc) {
    if (io->iVerbose >= VERBPROG) {
      if (iReason == 0) {
        // Called to check halt
        printf("HALT: ");
      } else if (iReason == 1) {
        // Called from CheckProgress
        printf("WARNING: ");
      }
      printf("Mutual Inclination[%s - %s] = ",body[iBody].cName,
          body[jBody].cName);
      fprintd(stdout,dMutualInc,io->iSciNot,io->iDigits);
      printf(", > max = ");
      fprintd(stdout,dMaxMutualInc,io->iSciNot,io->iDigits);
      printf(" at %.2e years.\n",evolve->dTime/YEARSEC);
    }
    return 1;
  } else {
    return 0;
  }
}


/*
 *
 * Multi-body matrix equations
 *
 */

/* EQTIDE - STELLAR COUPLING */

/*
 * Change in semi-major axis due to eqtide - stellar coupling
 * due to stellar evolution (magnetic braking and radius contraction)
 * Note: body 1 has orbital information for binary system
 */

/*! Compute rate of change of semi-major axis for tidally locked star(s)
 * if iNumLocked = 2, both are locked, otherwise, iBody gives body struct
 * index for star that is tidally locked
 */
double fdSemiTidalLockEqSt(BODY *body, int iNumLocked, int iBody)
{
  double adot = 0.0;
  double Jdot = 0.0;
  double edot = 0.0;
  double R1dot, R2dot, Rdot, RG1dot, RG2dot, RGdot;
  double num, denom, tmp;
  double M = body[0].dMass + body[1].dMass;
  double mu = body[0].dMass*body[1].dMass/M;
  double dMeanMotion = body[1].dMeanMotion;
  double J = mu*sqrt(BIGG*M*body[1].dSemi*(1.0-body[1].dEcc*body[1].dEcc)); // Orbital angular momentum
  SYSTEM *system; // Dummy system struct
  int iaBody[1] = {0};

  // Both tidally locked
  if(iNumLocked > 1) {
    // Compute change in angular momentum due to magnetic braking for both stars
    // and compute star's change in radii

    // Is body 0 a star? If not, not undergoing stellar evolution
    if(body[0].bStellar) {

      iaBody[0] = 0;

      Jdot += fdDJDtMagBrakingStellar(body,system,iaBody);
      R1dot = fdDRadiusDtStellar(body,system,iaBody);
      RG1dot = fdDRadGyraDtStellar(body,system,iaBody);
    }
    else {
      Jdot += 0.0;
      R1dot = 0.0;
      RG1dot = 0.0;
    }

    // Is body 1 a star? If not, not undergoing stellar evolution
    if(body[1].bStellar) {

      iaBody[0] = 1;

      Jdot += fdDJDtMagBrakingStellar(body,system,iaBody);
      R2dot = fdDRadiusDtStellar(body,system,iaBody);
      RG2dot = fdDRadGyraDtStellar(body,system,iaBody);
    }
    else {
      Jdot += 0.0;
      R2dot = 0.0;
      RG2dot = 0.0;
    }

    edot = 0.0; // No effect produces a de/dt term

    tmp = body[0].dMass*body[0].dRadGyra*body[0].dRadGyra*body[0].dRadius*R1dot;
    tmp += body[1].dMass*body[1].dRadGyra*body[1].dRadGyra*body[1].dRadius*R2dot;
    tmp += body[0].dMass*body[0].dRadGyra*body[0].dRadius*body[0].dRadius*RG1dot;
    tmp += body[1].dMass*body[1].dRadGyra*body[1].dRadius*body[1].dRadius*RG2dot;

    num = -Jdot - 2.0*body[0].dRotRate*tmp + mu*mu*BIGG*M*body[1].dSemi*body[1].dEcc*edot/J;

    tmp = body[0].dMass*body[0].dRadGyra*body[0].dRadGyra*body[0].dRadius*body[0].dRadius;
    tmp += body[1].dMass*body[1].dRadGyra*body[1].dRadGyra*body[1].dRadius*body[1].dRadius;
    tmp *= 1.5*body[0].dRotRate/body[1].dSemi;
    denom = mu*mu*BIGG*M*(1.0-body[1].dEcc*body[1].dEcc)/(2.0*J) - tmp;

    adot = num/denom;
  }
  // Just one (body[iBody]) is tidally locked
  else if(iNumLocked == 1) {

    // Only applies if stellar evolution is occurring.
    if(body[iBody].bStellar) {
      int iaBody[1] = {iBody};

      edot = 0.0; // No effect produces a de/dt term

      Jdot = fdDJDtMagBrakingStellar(body,system,iaBody);
      Rdot = fdDRadiusDtStellar(body,system,iaBody);
      RGdot = fdDRadGyraDtStellar(body,system,iaBody);

      tmp = body[iBody].dMass*body[iBody].dRadGyra*body[iBody].dRadGyra*body[iBody].dRadius*Rdot;
      tmp += body[iBody].dMass*body[iBody].dRadGyra*body[iBody].dRadius*body[iBody].dRadius*RGdot;

      num = -Jdot - 2.0*body[iBody].dRotRate*tmp + mu*mu*BIGG*M*body[1].dSemi*body[1].dEcc*edot/J;

      tmp = body[iBody].dMass*body[iBody].dRadGyra*body[iBody].dRadGyra*body[iBody].dRadius*body[iBody].dRadius;
      tmp *= 1.5*body[iBody].dRotRate/body[1].dSemi;
      denom = mu*mu*BIGG*M*(1.0-body[1].dEcc*body[1].dEcc)/(2.0*J) - tmp;

      adot = num/denom;
    }
    else {
      adot = 0.0;
    }
  }
  else {
    adot = 0.0;
  }

  return adot;
}

/*! Compute change in binary semi-major axis when circular, tidally locked, w ~ n
 * when EQTIDE and STELLAR are active
 */
double fdSemiDtEqSt(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0]; // Secondary body
  int iTmp;
  double adot = 0.0;

  // Both are tidally locked
  if(body[0].bTideLock && body[1].bTideLock) {
    return fdSemiTidalLockEqSt(body,2,-1);
  }
  // Primary is tidally locked
  else if(body[0].bTideLock && !body[1].bTideLock) {
    return fdSemiTidalLockEqSt(body,1,0);
  }
  // Secondary is tidally locked
  else if(!body[0].bTideLock && body[1].bTideLock) {
    return fdSemiTidalLockEqSt(body,1,1);
  }
  else {
    return 0.0;
  }
}

double fndUpdateSpiNBodyCoords(BODY *body,EVOLVE *evolve) {
  int iBody;

  for (iBody = 0; iBody<evolve->iNumBodies; iBody++) {
    if (iBody != 0) {
      // If iBody is a planet, then calculate various orbital elements
      body[iBody].dMu = BIGG*(body[0].dMass+body[iBody].dMass);

      body[iBody].dMeanL += sqrt(body[iBody].dMu/(body[iBody].dSemi*body[iBody].dSemi*body[iBody].dSemi))*evolve->dTime;
      body[iBody].dMeanL = fmod(body[iBody].dMeanL,2*PI);

      // Convert all bodies w/ orbital elements to Heliocentric

      // First find osc elems from DistOrb primary variables
      body[iBody].dLongP = atan2(body[iBody].dHecc,body[iBody].dKecc);
      body[iBody].dLongA = atan2(body[iBody].dPinc,body[iBody].dQinc);
      body[iBody].dInc = 2*asin(sqrt(body[iBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[iBody].dQinc));
      body[iBody].dEcc = sqrt(body[iBody].dKecc*body[iBody].dKecc+body[iBody].dHecc*body[iBody].dHecc);

      body[iBody].dMeanA = body[iBody].dMeanL - body[iBody].dLongP;
    } else {
      // If iBody is the star, zero out everything
      body[iBody].dMu = 0;
      body[iBody].dMeanL = 0;
      body[iBody].dLongP = 0;
      body[iBody].dLongA = 0;
      body[iBody].dInc = 0;
      body[iBody].dEcc = 0;
      body[iBody].dMeanA = 0;
    }

    OrbElems2Helio(body, iBody);
  }

  for (iBody = 0; iBody<evolve->iNumBodies; iBody++) {
    // Calculate Barycentric Cartesian coords:

    Helio2Bary(body, evolve->iNumBodies, iBody);
    body[iBody].dPositionX = body[iBody].dBCartPos[0]*AUM;
    body[iBody].dPositionY = body[iBody].dBCartPos[1]*AUM;
    body[iBody].dPositionZ = body[iBody].dBCartPos[2]*AUM;
    body[iBody].dVelX      = body[iBody].dBCartVel[0]*AUM/DAYSEC;
    body[iBody].dVelY      = body[iBody].dBCartVel[1]*AUM/DAYSEC;
    body[iBody].dVelZ      = body[iBody].dBCartVel[2]*AUM/DAYSEC;
  }

  // TODO: Should this function be *void*?
  return 0;
}

double fdLuminosityTotal(BODY *body,int iNumBodies) {
  int iBody;
  double dLumTot = 0;

  for (iBody=0;iBody<iNumBodies;iBody++) {
    if (body[iBody].bStellar) {
      dLumTot += body[iBody].dLuminosity;
    }
  }

  return dLumTot;
}
