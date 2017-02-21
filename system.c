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

/* Compute the total orbital angular momentum in the SYSTEM
 * as L = mu*sqrt(GMA(1-e^2)) for each orbiting body
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

    dTot += dMass*body[iBody].dMass*sqrt(BIGG*(dMass+body[iBody].dMass)*body[iBody].dSemi*(1-body[iBody].dEcc*body[iBody].dEcc))/(dMass+body[iBody].dMass);
  }

  return dTot;
}

double fdTotAngMom(BODY *body, CONTROL *control) {
  double dTot = 0.0;
  int iBody;

  // Total orbital angular momentum
  dTot = fdOrbAngMom(body,control);

  // Add all rotational angular momentum
  for(iBody = 0; iBody < control->Evolve.iNumBodies; iBody++)
  {
    dTot += fdRotAngMom(body[iBody].dRadGyra,body[iBody].dMass,body[iBody].dRadius,body[iBody].dRotRate);
  }

  return dTot;
}

/*
 *
 * Energy equations
 *
 */

// Broken XXX: Account for multiple bodies
double fdOrbPotEnergy(double dMass1, double dMass2,double dSemi) {
  return -BIGG*dMass1*dMass2/dSemi;
}

// Broken XXX: Account for multiple bodies
double fdOrbKinEnergy(double dMass1,double dMass2,double dSemi) {
  return 0.5*BIGG*dMass1*dMass2/dSemi;
}

double fdOrbEnergy(BODY *body,int iBody) {
  /* XXX Broken -- needs to include multiple bodies */
  return fdOrbKinEnergy(body[0].dMass,body[1].dMass,body[1].dSemi) + fdOrbPotEnergy(body[0].dMass,body[1].dMass,body[1].dSemi);
}

double fdKinEnergy(BODY *body) {
  double dKE;
  int iBody;

  /* XXX Broken -- needs to include multiple bodies
  dKE = fdOrbKinEnergy(body[0].dMass,body[1].dMass,body[1].dSemi);
  for (iBody=0;iBody<2;iBody++)
    dKE += fdRotKinEnergy(body[iBody].dMass,body[iBody].dRadius,body[iBody].dRadGyra,body[iBody].dRotRate);

  return dKE;
  */
  return 0;
}

double fdPotEnergy(BODY *body) {
  double dPE;
  int iBody;

  /* XXX Broken -- needs to include multiple bodies
  dPE = fdOrbPotEnergy(body[0].dMass,body[1].dMass,body[1].dSemi);
  for (iBody=0;iBody<2;iBody++)
    dPE += fdBodyPotEnergy(body[iBody]);

  return dPE;
  */
  return 0;
}

double fdTotEnergy(BODY *body) {
  /* XXX Broken -- needs to include multiple bodies */
  return fdKinEnergy(body) + fdPotEnergy(body);
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

/*
 * Change in semi-major axis due to binary - eqtide - stellar coupling
 * Right now, applies to tidally-locked binary stars (since the angular
 * momentum from spinning-down stars must go somewhere)
 * Note: body 1 has orbital information for binary
 *
 * When circular, da/dt = 2 * a * (1/J_orb)dJ_star/dt for angular momentum J
 * When eccentric, TODO
 */

double fdSemiDtEqBinSt(BODY *body, SYSTEM *system, int *iaBody) {
  // iaBody [0] is ALWAYS the current body: one of the stars
  int iBody = iaBody[0];

  // Only do stuff if tidally-locked and circular (FOR NOW)
  // Note I used TINY as my small enough condition because if dEcc < dMinEcc,
  // code sets dEcc to 0 so it's fine
  if(body[iBody].bTideLock && body[iBody].dEcc < TINY)
  {
    fprintf(stderr,"DOING STUFF!\n");

    // Compute orbital angular momentum
    double M = body[0].dMass + body[1].dMass;
    double mu = body[0].dMass*body[1].dMass/M;
    double J = mu*sqrt(BIGG*M*body[1].dSemi*(1.0-body[1].dEcc*body[1].dEcc));

    // Compute change in stellar angular momentum for current star (iBodyth star)
    double Jdot = 0.0; // TODO

    // Compute, return change in semi-major axis
    return 2.0*body[1].dSemi*Jdot/J;
  }
  // Not tidally locked, do nothing
  else
  {
    return 0.0;
  }
}

/*
 *
 * Functions required to add multi-body equations to the matrix
 *
 */

/*! Binary-Eqtide-Stellar semi-major axis derivative set-up */
void InitializeUpdateEqBinStSemi(BODY *body,UPDATE *update,int iBody) {
    if (update[iBody].iNumSemi == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumSemi++;
}


void FinalizeUpdateMultiEqBinStSemi(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {

  update[iBody].iaModule[iVar][*iEqn] = BINARY + EQTIDE + STELLAR;
  update[iBody].iNumSemi = (*iEqn)++;
}


/*! Binary-Eqtide-Stellar semi-major axis derivative set-up */
void UpdateMultiEqBinStSemi(BODY *body,UPDATE *update,CONTROL *control) {

  // Fuck malloc-ing, we'll do it live
  /*
  int iEqn, iModule;

  // TODO: wtf do I do for iVar?

  // Semi-major Axis
  update[iBody].iSemi = -1;
  if (update[iBody].iNumSemi) {
    update[iBody].iSemi = iVar;
    update[iBody].iaVar[iVar] = VSEMI;
    update[iBody].iNumEqns[iVar] = update[iBody].iNumSemi;
    update[iBody].pdVar[iVar] = &body[iBody].dSemi;
    update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
    update[iBody].iaBody[iVar] = malloc(update[iBody].iNumSemi*sizeof(int*));
    update[iBody].iaType[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
    update[iBody].iaModule[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));

    if (control->Evolve.iOneStep == RUNGEKUTTA) {
      control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dSemi;
      control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
      control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumSemi*sizeof(double));
      control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
      control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
      control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumSemi*sizeof(int*));
    }

    iEqn=0;
    FinalizeUpdateMultiEqBinStSemi(body,update,&iEqn,iVar,iBody,iFoo);

    (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
    update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
    iVar++;
    */
  }
