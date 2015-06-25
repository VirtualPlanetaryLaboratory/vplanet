/********************** LAGRANGE.C **********************/
/*
 * Russell Deitrick, April 28, 2015
 *
 * Subroutines that control the integration of the orbital model. 
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"
#include "lagrange.h"

void InitializeControlLagrange(CONTROL *control) {
  /* Not sure if I need anything here yet */
}

void InitializeModuleLagrange(CONTROL *control,MODULE *module) {
  /* Anything here? */
}

void BodyCopyLagrange(BODY *dest,BODY *src,int iBody) {
  int iIndex,iPert;

  dest[iBody].dHecc = src[iBody].dHecc;
  dest[iBody].dKecc = src[iBody].dKecc;
  dest[iBody].dPinc = src[iBody].dPinc;
  dest[iBody].dQinc = src[iBody].dQinc;

  for (iPert=0;iPert<src[iBody].iGravPerts;iPert++)
    dest[iBody].iaGravPerts[iPert] = src[iBody].iaGravPerts[iPert];
}

void InitializeBodyLagrange(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
  body[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
}

void InitializeUpdateTmpBodyLagrange(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
  int iBodyPert;
  
  control->Evolve.tmpBody[iBody].iaGravPerts = malloc(body[iBody].iGravPerts);

}

/**************** LAGRANGE options ********************/

/* Inclination */

void ReadInc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
	if (control->Io.iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
	if (control->Io.iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,180].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    //body[iFile-1].dInc = dTmp; 
    body[iFile-1].dSinc = sin(0.5*dTmp);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dSinc = options->dDefault;
}  

/* Longitude of ascending node */

void ReadLongA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->Io.iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->Io.iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile-1].dLongA = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dLongA = options->dDefault;
}  


/* Longitude of pericenter */

void ReadLongP(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->Io.iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->Io.iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    
    body[iFile-1].dLongP = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dLongP = options->dDefault;
}  


/* Argument of pericenter */

void ReadArgP(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->Io.iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->Io.iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    
    body[iFile-1].dArgP = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dArgP = options->dDefault;
}  


void InitializeOptionsLagrange(OPTIONS *options,fnReadOption fnRead[]) {
  
  sprintf(options[OPT_INC].cName,"dInc");
  sprintf(options[OPT_INC].cDescr,"Inclination of planet's orbital plane");
  sprintf(options[OPT_INC].cDefault,"0");
  options[OPT_INC].dDefault = 0.0;
  options[OPT_INC].iType = 2;  
  options[OPT_INC].iMultiFile = 1;  
  fnRead[OPT_INC] = &ReadInc;
  
  sprintf(options[OPT_LONGA].cName,"dLongA");
  sprintf(options[OPT_LONGA].cDescr,"Longitude of ascending node of planet's orbital plane");
  sprintf(options[OPT_LONGA].cDefault,"0");
  options[OPT_LONGA].dDefault = 0.0;
  options[OPT_LONGA].iType = 2;  
  options[OPT_LONGA].iMultiFile = 1; 
  fnRead[OPT_LONGA] = &ReadLongA;
  
  sprintf(options[OPT_LONGP].cName,"dLongP");
  sprintf(options[OPT_LONGP].cDescr,"Longitude of pericenter of planet's orbit");
  sprintf(options[OPT_LONGP].cDefault,"0");
  options[OPT_LONGP].dDefault = 0.0;
  options[OPT_LONGP].iType = 2;  
  options[OPT_LONGP].iMultiFile = 1;   
  fnRead[OPT_LONGP] = &ReadLongP;
  
  sprintf(options[OPT_ARGP].cName,"dArgP");
  sprintf(options[OPT_ARGP].cDescr,"Argument of pericenter of planet's orbit");
  sprintf(options[OPT_ARGP].cDefault,"0");
  options[OPT_ARGP].dDefault = 0.0;
  options[OPT_ARGP].iType = 2;  
  options[OPT_ARGP].iMultiFile = 1;   
  fnRead[OPT_ARGP] = &ReadArgP;
 
}

void ReadOptionsLagrange(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTLAGRANGE;iOpt<OPTENDLAGRANGE;iOpt++) { 
      if (options[iOpt].iType != -1) {
	fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
      }
  }
}
    

/******************* Verify LAGRANGE ******************/
// void InitializeLagrange(BODY *body,CONTROL *control,UPDATE *update) {
//   int iBody,jBody,i,j;
//    
//   
//   /* Body #0 updates */
//   update[0].iNum = 0;   //Can this be zero? Central body does not evolve in Lagrange.
//   InitializeUpdate(&update[0]);
//   
//   for (iBody=1; iBody<body[0].iNumBodies; iBody++) {
//     /* Body #1 updatesla */
//     if (body[iBody].bObliqEvol == 1) {
//       if (control->bUseShadow == 1) {
// 	update[iBody].iNum = 9;
//       } else {
// 	update[iBody].iNum = 7;
//       }
//     } else {
//       update[iBody].iNum = 4;
//     }
//     InitializeUpdate(&update[iBody]);
//     
//     /* 0 -> h = e*sin(varpi) */
//     update[iBody].iType[0] = 1;
//     update[iBody].pdVar[0] = &body[iBody].dHecc;
//     update[iBody].iNumEqns[0] = 1;
//     update[iBody].dDeriv[0]=malloc(update[iBody].iNumEqns[0]*sizeof(double));
//     update[iBody].bAng[0] = 0;   //do not set *both* bAng and bPolar = 1
//     update[iBody].bPolar[0] = 1;
//     
//     /* 1 -> k = e*cos(varpi) */
//     update[iBody].iType[1] = 1;
//     update[iBody].pdVar[1] = &body[iBody].dKecc;
//     update[iBody].iNumEqns[1] = 1;
//     update[iBody].dDeriv[1]=malloc(update[iBody].iNumEqns[1]*sizeof(double));
//     update[iBody].bAng[1] = 0;    //do not set *both* bAng and bPolar = 1
//     update[iBody].bPolar[1] = 1;
//     
//     /* 2 -> p = s*sin(Omega) */
//     update[iBody].iType[2] = 1;
//     update[iBody].pdVar[2] = &body[iBody].dPinc;
//     update[iBody].iNumEqns[2] = 1;
//     update[iBody].dDeriv[2]=malloc(update[iBody].iNumEqns[2]*sizeof(double));
//     update[iBody].bAng[2] = 0;   //do not set *both* bAng and bPolar = 1
//     update[iBody].bPolar[2] = 1;
//     
//     /* 3 -> q = s*cos(Omega) */
//     update[iBody].iType[3] = 1;
//     update[iBody].pdVar[3] = &body[iBody].dQinc;
//     update[iBody].iNumEqns[3] = 1;
//     update[iBody].dDeriv[3]=malloc(update[iBody].iNumEqns[3]*sizeof(double));
//     update[iBody].bAng[3] = 0;   //do not set *both* bAng and bPolar = 1
//     update[iBody].bPolar[3] = 1;
//     
//     if (body[iBody].bObliqEvol == 1) {
//       /* 4 -> beta = sin(obliq)*sin(precA) */
//       update[iBody].iType[4] = 1;
//       update[iBody].pdVar[4] = &body[iBody].dbeta;
//       update[iBody].iNumEqns[4] = 1;
//       update[iBody].dDeriv[4]=malloc(update[iBody].iNumEqns[4]*sizeof(double));
//       update[iBody].bAng[4] = 0;   //do not set *both* bAng and bPolar = 1
//       update[iBody].bPolar[4] = 1;
//     
//       /* 5 -> gamma = sin(obliq)*cos(precA) */
//       update[iBody].iType[5] = 1;
//       update[iBody].pdVar[5] = &body[iBody].dgamma;
//       update[iBody].iNumEqns[5] = 1;
//       update[iBody].dDeriv[5]=malloc(update[iBody].iNumEqns[5]*sizeof(double));
//       update[iBody].bAng[5] = 0;   //do not set *both* bAng and bPolar = 1
//       update[iBody].bPolar[5] = 1;
//       
//       /* 6 -> eta = cos(obliq) */
//       update[iBody].iType[6] = 1;
//       update[iBody].pdVar[6] = &body[iBody].deta;
//       update[iBody].iNumEqns[6] = 1;
//       update[iBody].dDeriv[6]=malloc(update[iBody].iNumEqns[6]*sizeof(double));
//       update[iBody].bAng[6] = 0;   //do not set *both* bAng and bPolar = 1
//       update[iBody].bPolar[6] = 1;
//       
//       if (control->bUseShadow == 1) {
// 	  /* 7 -> shadow beta */
// 	update[iBody].iType[7] = 1;
// 	update[iBody].pdVar[7] = &body[iBody].dShadowbeta;
// 	update[iBody].iNumEqns[7] = 1;
// 	update[iBody].dDeriv[7]=malloc(update[iBody].iNumEqns[7]*sizeof(double));
// 	update[iBody].bAng[7] = 0;   //do not set *both* bAng and bPolar = 1
// 	update[iBody].bPolar[7] = 1;
//       
// 	/* 8 -> shadow gamma */
// 	update[iBody].iType[8] = 1;
// 	update[iBody].pdVar[8] = &body[iBody].dShadowgamma;
// 	update[iBody].iNumEqns[8] = 1;
// 	update[iBody].dDeriv[8]=malloc(update[iBody].iNumEqns[8]*sizeof(double));
// 	update[iBody].bAng[8] = 0;   //do not set *both* bAng and bPolar = 1
// 	update[iBody].bPolar[8] = 1;
//       }
//     }
//     
//     body[iBody].dPosition = malloc(3*sizeof(double));
//     body[iBody].dVelocity = malloc(3*sizeof(double));
//     //body[iBody].dLaplaceC = malloc((body[0].iNumBodies)*sizeof(double));
//     
//     
//   }
// 
// }

/*
 *
 * Pericenter/Ascending node 
 *
 */

double fdCorrectDomain(double angle) {
  while (angle >= 2*PI) {
    angle -= 2*PI;
  }
  while (angle < 0.0) {
    angle += 2*PI;
  }
  return angle;
}

double fdCalcLongP(double dLongA, double dArgP) {
  double varpi;
  varpi = fdCorrectDomain(dLongA + dArgP);
  return varpi;
}

double fdCalcLongA(double dLongP, double dArgP) {
  double Omega;
  Omega = fdCorrectDomain(dLongP - dArgP);
  return Omega;
}

void VerifyPericenter(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody,int iVerbose) {
  /* First see if longitude of ascending node and longitude of pericenter and nothing else set, i.e. the user input the default parameters */
  if (options[OPT_LONGA].iLine[iBody] > -1 && options[OPT_LONGP].iLine[iBody] > -1 && options[OPT_ARGP].iLine[iBody] == -1) 
    return;

  /* If none are set, raise error */
  if (options[OPT_LONGA].iLine[iBody] == -1 && options[OPT_LONGP].iLine[iBody] == -1 && options[OPT_ARGP].iLine[iBody] == -1) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Must set at least two of %s, %s, and %s in File: %s.\n",options[OPT_LONGA].cName,options[OPT_LONGP].cName,options[OPT_ARGP].cName, cFile);
    exit(EXIT_INPUT);
  }
  
  /* At least 2 must be set */
  if ((options[OPT_LONGA].iLine[iBody] == -1 && options[OPT_LONGP].iLine[iBody] == -1) || (options[OPT_LONGA].iLine[iBody] == -1 && options[OPT_ARGP].iLine[iBody] == -1) || (options[OPT_LONGP].iLine[iBody] == -1 && options[OPT_ARGP].iLine[iBody] == -1)) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Must set at least two of %s, %s, and %s in File: %s.\n",options[OPT_LONGA].cName,options[OPT_LONGP].cName,options[OPT_ARGP].cName, cFile);
    exit(EXIT_INPUT);
  }

  /* Were all set? */
  if (options[OPT_LONGA].iLine[iBody] > -1 && options[OPT_LONGP].iLine[iBody] > -1 && options[OPT_ARGP].iLine[iBody] > -1) {
    VerifyTripleExit(options[OPT_LONGA].cName,options[OPT_LONGP].cName,options[OPT_ARGP].cName,options[OPT_LONGA].iLine[iBody],options[OPT_LONGP].iLine[iBody],options[OPT_ARGP].iLine[iBody],cFile,iVerbose);
    exit(EXIT_INPUT);
  }

  /* Was LONGA set? */
  if (options[OPT_LONGA].iLine[iBody] > -1) {
    
    if (options[OPT_LONGP].iLine[iBody] > -1)
      /* LONGA and LONGP were the only two set - Nothing to do */
	 return;
    if (options[OPT_ARGP].iLine[iBody] > -1) 
      /* Must get radius from density */
      body->dLongP = fdCalcLongP(body->dLongA,body->dArgP);    
    return;
  }

  /* Was LongP set? */
  if (options[OPT_LONGP].iLine[iBody] > -1) {
    
    if (options[OPT_LONGA].iLine[iBody] > -1)
      /* LONGA and LONGP were the only two set - Nothing to do */
	 return;
    if (options[OPT_ARGP].iLine[iBody] > -1) 
      /* Must get radius from density */
      body->dLongA = fdCalcLongA(body->dLongP,body->dArgP);    
    return;
  }
}

void CalcHKPQ(BODY *body, int iBody) {
  body[iBody].dHecc = body[iBody].dEcc*sin(body[iBody].dLongP);
  body[iBody].dKecc = body[iBody].dEcc*cos(body[iBody].dLongP);
  body[iBody].dPinc = body[iBody].dSinc*sin(body[iBody].dLongA);
  body[iBody].dQinc = body[iBody].dSinc*cos(body[iBody].dLongA);
}

/* In the following, iBody is the current body number that is getting assigned,
   iPert counts the number of bodies perturbing iBody, and iBodyPert is the
   body number of the current perturbing body. */
void InitializeHeccLagrange(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iHecc][update[iBody].iaHeccLagrange[iPert]] = 1;
  update[iBody].padDHeccDtLagrange[iPert] = &update[iBody].daDerivProc[update[iBody].iHecc][update[iBody].iaHeccLagrange[iPert]];
  update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccLagrange[iPert]]=2;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccLagrange[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccLagrange[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccLagrange[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccLagrange[iPert]][1] = iPert;
}

void InitializeKeccLagrange(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iKecc][update[iBody].iaKeccLagrange[iPert]] = 1;
  update[iBody].padDKeccDtLagrange[iPert] = &update[iBody].daDerivProc[update[iBody].iKecc][update[iBody].iaKeccLagrange[iPert]];
  update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iaKeccLagrange[iPert]]=2;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccLagrange[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iaKeccLagrange[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccLagrange[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccLagrange[iPert]][1] = iPert;
}

void InitializePincLagrange(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iPinc][update[iBody].iaPincLagrange[iPert]] = 1;
  update[iBody].padDPincDtLagrange[iPert] = &update[iBody].daDerivProc[update[iBody].iPinc][update[iBody].iaPincLagrange[iPert]];
  update[iBody].iNumBodies[update[iBody].iPinc][update[iBody].iaPincLagrange[iPert]]=2;
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincLagrange[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iPinc][update[iBody].iaPincLagrange[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincLagrange[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincLagrange[iPert]][1] = iPert;
}

void InitializeQincLagrange(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iQinc][update[iBody].iaQincLagrange[iPert]] = 1;
  update[iBody].padDQincDtLagrange[iPert] = &update[iBody].daDerivProc[update[iBody].iQinc][update[iBody].iaQincLagrange[iPert]];
  update[iBody].iNumBodies[update[iBody].iQinc][update[iBody].iaQincLagrange[iPert]]=2;
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincLagrange[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iQinc][update[iBody].iaQincLagrange[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincLagrange[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincLagrange[iPert]][1] = iPert;
}

void VerifyPerturbersLagrange(BODY *body,int iNumBodies,int iBody) {
  int iPert=0, j;
  
  body[iBody].iGravPerts = iNumBodies - 2; //will need to change this for zero mass particles in future
  body[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
  for (j=1;j<iNumBodies;j++) {
    if (j != iBody) {
      body[iBody].iaGravPerts[iPert] = j;
      iPert++;
    }
  }
}
  
/* Factorial function. Nuff sed. */
unsigned long int factorial(unsigned int n)
{
  unsigned long int result;
  
  if (n == 0)
    result = 1;
  else 
    result = n * factorial(n - 1);
  return result;
}

/* Number of combinations of k in N */
int Nchoosek(int N, int k) {
  if (N < 0 || k < 0 || N > 10 || k > N) {
    printf("Error: received N = %d, k = %d\n",N,k);
  } 
  return factorial(N) / (factorial(k)*factorial(N-k));
}

/* Gives the index of a pair of values in N choose 2.
* For example, for 4 planets, the index for the pair 
* (1,2) -> 0, (1,3) -> 1, (1,4) -> 2, (2,3) -> 3, etc. */  
int CombCount(int x, int y, int N) {
  if (x == 0) {
    x = 1.3;
  }
  return N*(x-1) + (y-1) - Nchoosek(x+1, 2);
} 


void VerifyLagrange(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int i, j, iPert;
  
  VerifyPericenter(body,control,options,files->Infile[iBody+1].cIn,iBody,control->Io.iVerbose);
  VerifyPerturbersLagrange(body,control->Evolve.iNumBodies,iBody);
  
  /* Setup Semi-major axis functions (LaplaceF) for secular terms*/
  if (bPrimary(body,iBody)) {
    system->fnLaplaceF = malloc(LAPLNUM*sizeof(fnLaplaceFunction*));
    for (j=0;j<LAPLNUM;j++) {
      system->fnLaplaceF[j] = malloc(1*sizeof(fnLaplaceFunction));
    }
    system->fnLaplaceF[0][0] = &fdSemiMajAxF1;
    system->fnLaplaceF[1][0] = &fdSemiMajAxF2;
    system->fnLaplaceF[2][0] = &fdSemiMajAxF3;
    system->fnLaplaceF[3][0] = &fdSemiMajAxF4;
    system->fnLaplaceF[4][0] = &fdSemiMajAxF5;
    system->fnLaplaceF[5][0] = &fdSemiMajAxF6;
    system->fnLaplaceF[6][0] = &fdSemiMajAxF7;
    system->fnLaplaceF[7][0] = &fdSemiMajAxF8;
    system->fnLaplaceF[8][0] = &fdSemiMajAxF9;
    system->fnLaplaceF[9][0] = &fdSemiMajAxF10;
    system->fnLaplaceF[10][0] = &fdSemiMajAxF11;
    system->fnLaplaceF[11][0] = &fdSemiMajAxF12;
    system->fnLaplaceF[12][0] = &fdSemiMajAxF13;
    system->fnLaplaceF[13][0] = &fdSemiMajAxF14;
    system->fnLaplaceF[14][0] = &fdSemiMajAxF15;
    system->fnLaplaceF[15][0] = &fdSemiMajAxF16;
    system->fnLaplaceF[16][0] = &fdSemiMajAxF17;
    system->fnLaplaceF[17][0] = &fdSemiMajAxF18;
    system->fnLaplaceF[18][0] = &fdSemiMajAxF19;
    system->fnLaplaceF[19][0] = &fdSemiMajAxF20;
    system->fnLaplaceF[20][0] = &fdSemiMajAxF21;
    system->fnLaplaceF[21][0] = &fdSemiMajAxF22;
    system->fnLaplaceF[22][0] = &fdSemiMajAxF23;
    system->fnLaplaceF[23][0] = &fdSemiMajAxF24;
    system->fnLaplaceF[24][0] = &fdSemiMajAxF25;
    system->fnLaplaceF[25][0] = &fdSemiMajAxF26;
    
    system->dmLaplaceC = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(double*));
    for (i=0;i<Nchoosek(control->Evolve.iNumBodies-1,2);i++) {
      system->dmLaplaceC[i] = malloc(LAPLNUM*sizeof(double));  
    }
    
    system->imLaplaceN = malloc((control->Evolve.iNumBodies)*sizeof(int*));
    for (i=1;i<control->Evolve.iNumBodies;i++) {
      system->imLaplaceN[i] = malloc((control->Evolve.iNumBodies)*sizeof(int));
    }
  } else {
    /* Body updates */
    for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
      /* h = Ecc*sin(LongP) */
      InitializeHeccLagrange(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iHecc][update[iBody].iaHeccLagrange[iPert]] = &fdLagrangeDhDt;
      
      /* k = Ecc*cos(LongP) */
      InitializeKeccLagrange(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iKecc][update[iBody].iaKeccLagrange[iPert]] = &fdLagrangeDkDt;
      
      /* p = s*sin(LongA) */
      InitializePincLagrange(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iPinc][update[iBody].iaPincLagrange[iPert]] = &fdLagrangeDpDt;
      
      /* q = s*cos(LongA) */
      InitializeQincLagrange(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iQinc][update[iBody].iaQincLagrange[iPert]] = &fdLagrangeDqDt;
      
      for (j=0;j<LAPLNUM;j++) {
	if (body[iBody].dSemi < body[iPert].dSemi) {
	    system->imLaplaceN[iBody][iPert] = CombCount(iBody,iPert,control->Evolve.iNumBodies-1);
	    system->dmLaplaceC[system->imLaplaceN[iBody][iPert]][j] = system->fnLaplaceF[j][0](body[iBody].dSemi/body[iPert].dSemi, 0);
	} else if (body[iBody].dSemi > body[iPert].dSemi) {
	    system->imLaplaceN[iBody][iPert] = CombCount(iBody,iPert,control->Evolve.iNumBodies-1);
	    system->dmLaplaceC[system->imLaplaceN[iBody][iPert]][j] = system->fnLaplaceF[j][0](body[iPert].dSemi/body[iBody].dSemi, 0);
	}
      }
    }
  }
}


/***************** LAGRANGE Update *****************/
void InitializeUpdateLagrange(BODY *body,UPDATE *update,int iBody) {
  if (!bPrimary(body,iBody)) {
    update[iBody].iNumHecc++;
    update[iBody].iNumKecc++;
    update[iBody].iNumPinc++;
    update[iBody].iNumQinc++;
    update[iBody].iNumVars += 4;
  }
}

void FinalizeUpdateHeccLagrange(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  int iPert;
  
  update[iBody].padDHeccDtLagrange = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaHeccLagrange = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = LAGRANGE;
    update[iBody].iaHeccLagrange[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateKeccLagrange(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  int iPert;
  
  update[iBody].padDKeccDtLagrange = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaKeccLagrange = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = LAGRANGE;
    update[iBody].iaKeccLagrange[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdatePincLagrange(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  int iPert;
  
  update[iBody].padDPincDtLagrange = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaPincLagrange = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = LAGRANGE;
    update[iBody].iaPincLagrange[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateQincLagrange(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  int iPert;
  
  update[iBody].padDQincDtLagrange = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaQincLagrange = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = LAGRANGE;
    update[iBody].iaQincLagrange[iPert] = (*iEqn)++;
  }
}


/***************** LAGRANGE Halts *****************/

void CountHaltsLagrange(HALT *halt,int *iNumHalts) { 
}

void VerifyHaltLagrange(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
}


/************* LAGRANGE Outputs ******************/

void WriteBodyDEccDtLagrange(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += 1./sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2))*(body[iBody].dHecc*(*(update[iBody].padDHeccDtLagrange[iPert]))+body[iBody].dKecc*(*(update[iBody].padDKeccDtLagrange[iPert])));
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteBodyDSincDtLagrange(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += 1./sqrt(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*(body[iBody].dPinc*(*(update[iBody].padDPincDtLagrange[iPert]))+body[iBody].dQinc*(*(update[iBody].padDQincDtLagrange[iPert])));
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}  
  
void WriteBodyDLongPDtLagrange(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += 1./(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2))*(body[iBody].dKecc*(*(update[iBody].padDHeccDtLagrange[iPert]))-body[iBody].dHecc*(*(update[iBody].padDKeccDtLagrange[iPert])));
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}  

void WriteBodyDLongADtLagrange(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += 1./(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*(body[iBody].dQinc*(*(update[iBody].padDPincDtLagrange[iPert]))-body[iBody].dPinc*(*(update[iBody].padDQincDtLagrange[iPert])));
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
} 

void WriteBodyDIncDtLagrange(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += 2./sqrt((1-(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2)))*(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2)))*(body[iBody].dPinc*(*(update[iBody].padDPincDtLagrange[iPert]))+body[iBody].dQinc*(*(update[iBody].padDQincDtLagrange[iPert])));
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
} 

void WriteBodySinc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = sqrt(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2));
  strcpy(cUnit,"");
}  

void WriteBodyInc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = 2.*asin(sqrt(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2)));  
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}  

void WriteBodyLongA(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = atan2(body[iBody].dPinc, body[iBody].dQinc);
  
  while (*dTmp < 0.0) {
    *dTmp += 2*PI;
  }
  while (*dTmp > 2*PI) {
    *dTmp -= 2*PI;
  }
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}  

void WriteBodyLongP(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = atan2(body[iBody].dHecc, body[iBody].dKecc);
  
  while (*dTmp < 0.0) {
    *dTmp += 2*PI;
  }
  while (*dTmp > 2*PI) {
    *dTmp -= 2*PI;
  }
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}  

void WriteBodyArgP(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double varpi, Omega;
  
  varpi = atan2(body[iBody].dHecc, body[iBody].dKecc);
  Omega = atan2(body[iBody].dPinc, body[iBody].dQinc);
  *dTmp = varpi - Omega;
  
  while (*dTmp < 0.0) {
    *dTmp += 2*PI;
  }
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}    

void WriteBodyHecc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dHecc;
  strcpy(cUnit,"");
}  

void WriteBodyKecc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dKecc;
  strcpy(cUnit,"");
} 

void WriteBodyPinc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dPinc;
  strcpy(cUnit,"");
} 

void WriteBodyQinc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dQinc;
  strcpy(cUnit,"");
} 

void WriteBodyDHeccDtLagrange(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* need to put check for star's output options in verify */
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDHeccDtLagrange[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteBodyDKeccDtLagrange(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* need to put check for star's output options in verify */
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDKeccDtLagrange[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteBodyDPincDtLagrange(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* need to put check for star's output options in verify */
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDPincDtLagrange[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteBodyDQincDtLagrange(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* need to put check for star's output options in verify */
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDQincDtLagrange[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}


void InitializeOutputLagrange(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_DECCDTLAGRANGE].cName,"DEccDtLagrange");
  sprintf(output[OUT_DECCDTLAGRANGE].cDescr,"Body's decc/dt in Lagrange");
  sprintf(output[OUT_DECCDTLAGRANGE].cNeg,"1/year");
  output[OUT_DECCDTLAGRANGE].bNeg = 1;
  output[OUT_DECCDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_DECCDTLAGRANGE].iNum = 1;
  fnWrite[OUT_DECCDTLAGRANGE] = &WriteBodyDEccDtLagrange;
  
  sprintf(output[OUT_DSINCDTLAGRANGE].cName,"DsIncDtLagrange");
  sprintf(output[OUT_DSINCDTLAGRANGE].cDescr,"Body's dsin(.5*Inc)/dt in Lagrange");
  sprintf(output[OUT_DSINCDTLAGRANGE].cNeg,"1/year");
  output[OUT_DSINCDTLAGRANGE].bNeg = 1;
  output[OUT_DSINCDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_DSINCDTLAGRANGE].iNum = 1;
  fnWrite[OUT_DSINCDTLAGRANGE] = &WriteBodyDSincDtLagrange;
 
  sprintf(output[OUT_DINCDTLAGRANGE].cName,"DIncDtLagrange");
  sprintf(output[OUT_DINCDTLAGRANGE].cDescr,"Body's dInc/dt in Lagrange");
  sprintf(output[OUT_DINCDTLAGRANGE].cNeg,"deg/year");
  output[OUT_DINCDTLAGRANGE].bNeg = 1;
  output[OUT_DINCDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_DINCDTLAGRANGE].iNum = 1;
  fnWrite[OUT_DINCDTLAGRANGE] = &WriteBodyDIncDtLagrange;
  
  sprintf(output[OUT_DLONGPDTLAGRANGE].cName,"DLongPDtLagrange");
  sprintf(output[OUT_DLONGPDTLAGRANGE].cDescr,"Body's dvarpi/dt in Lagrange");
  sprintf(output[OUT_DLONGPDTLAGRANGE].cNeg,"deg/yr");
  output[OUT_DLONGPDTLAGRANGE].bNeg = 1;
  output[OUT_DLONGPDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_DLONGPDTLAGRANGE].iNum = 1;
  fnWrite[OUT_DLONGPDTLAGRANGE] = &WriteBodyDLongPDtLagrange;
  
  sprintf(output[OUT_DLONGADTLAGRANGE].cName,"DLongADtLagrange");
  sprintf(output[OUT_DLONGADTLAGRANGE].cDescr,"Body's dOmega/dt in Lagrange");
  sprintf(output[OUT_DLONGADTLAGRANGE].cNeg,"deg/yr");
  output[OUT_DLONGADTLAGRANGE].bNeg = 1;
  output[OUT_DLONGADTLAGRANGE].dNeg = DEGRAD;
  output[OUT_DLONGADTLAGRANGE].iNum = 1;
  fnWrite[OUT_DLONGADTLAGRANGE] = &WriteBodyDLongADtLagrange;
  
  sprintf(output[OUT_INC].cName,"Inc");
  sprintf(output[OUT_INC].cDescr,"Body's Inclination in Lagrange");
  sprintf(output[OUT_INC].cNeg,"Deg");
  output[OUT_INC].bNeg = 1;
  output[OUT_INC].dNeg = 1./DEGRAD;
  output[OUT_INC].iNum = 1;
  fnWrite[OUT_INC] = &WriteBodyInc;
  
  sprintf(output[OUT_SINC].cName,"sInc");
  sprintf(output[OUT_SINC].cDescr,"Body's sin(1/2*Inclination) in Lagrange");
  output[OUT_SINC].iNum = 1;
  fnWrite[OUT_SINC] = &WriteBodySinc;
  
  sprintf(output[OUT_LONGA].cName,"LongA");
  sprintf(output[OUT_LONGA].cDescr,"Body's Longitude of ascending node in Lagrange");
  sprintf(output[OUT_LONGA].cNeg,"Deg");
  output[OUT_LONGA].bNeg = 1;
  output[OUT_LONGA].dNeg = 1./DEGRAD;
  output[OUT_LONGA].iNum = 1;
  fnWrite[OUT_LONGA] = &WriteBodyLongA;
  
  sprintf(output[OUT_LONGP].cName,"LongP");
  sprintf(output[OUT_LONGP].cDescr,"Body's Longitude of pericenter in Lagrange");
  sprintf(output[OUT_LONGP].cNeg,"Deg");
  output[OUT_LONGP].bNeg = 1;
  output[OUT_LONGP].dNeg = 1./DEGRAD;
  output[OUT_LONGP].iNum = 1;
  fnWrite[OUT_LONGP] = &WriteBodyLongP; 
  
  sprintf(output[OUT_ARGP].cName,"ArgP");
  sprintf(output[OUT_ARGP].cDescr,"Body's argument of pericenter in Lagrange");
  sprintf(output[OUT_ARGP].cNeg,"Deg");
  output[OUT_ARGP].bNeg = 1;
  output[OUT_ARGP].dNeg = 1./DEGRAD;
  output[OUT_ARGP].iNum = 1;
  fnWrite[OUT_ARGP] = &WriteBodyArgP; 
  
  sprintf(output[OUT_HECC].cName,"Hecc");
  sprintf(output[OUT_HECC].cDescr,"Body's h = e*sin(varpi) in Lagrange");
  output[OUT_HECC].iNum = 1;
  fnWrite[OUT_HECC] = &WriteBodyHecc;
  
  sprintf(output[OUT_KECC].cName,"Kecc");
  sprintf(output[OUT_KECC].cDescr,"Body's k = e*cos(varpi) in Lagrange");
  output[OUT_KECC].iNum = 1;
  fnWrite[OUT_KECC] = &WriteBodyKecc;
  
  sprintf(output[OUT_PINC].cName,"Pinc");
  sprintf(output[OUT_PINC].cDescr,"Body's p = s*sin(Omega) in Lagrange");
  output[OUT_PINC].iNum = 1;
  fnWrite[OUT_PINC] = &WriteBodyPinc;
  
  sprintf(output[OUT_QINC].cName,"Qinc");
  sprintf(output[OUT_QINC].cDescr,"Body's q = s* cos(Omega) in Lagrange");
  output[OUT_QINC].iNum = 1;
  fnWrite[OUT_QINC] = &WriteBodyQinc;
  
  sprintf(output[OUT_DHECCDTLAGRANGE].cName,"DHeccDtLagrange");
  sprintf(output[OUT_DHECCDTLAGRANGE].cDescr,"Body's dh/dt in Lagrange");
  sprintf(output[OUT_DHECCDTLAGRANGE].cNeg,"1/year");
  output[OUT_DHECCDTLAGRANGE].bNeg = 1;
  output[OUT_DHECCDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_DHECCDTLAGRANGE].iNum = 1;
  fnWrite[OUT_DHECCDTLAGRANGE] = &WriteBodyDHeccDtLagrange;
  
  sprintf(output[OUT_DKECCDTLAGRANGE].cName,"DKeccDtLagrange");
  sprintf(output[OUT_DKECCDTLAGRANGE].cDescr,"Body's dk/dt in Lagrange");
  sprintf(output[OUT_DKECCDTLAGRANGE].cNeg,"1/year");
  output[OUT_DKECCDTLAGRANGE].bNeg = 1;
  output[OUT_DKECCDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_DKECCDTLAGRANGE].iNum = 1;
  fnWrite[OUT_DKECCDTLAGRANGE] = &WriteBodyDKeccDtLagrange;
  
  sprintf(output[OUT_DPINCDTLAGRANGE].cName,"DPincDtLagrange");
  sprintf(output[OUT_DPINCDTLAGRANGE].cDescr,"Body's dp/dt in Lagrange");
  sprintf(output[OUT_DPINCDTLAGRANGE].cNeg,"1/year");
  output[OUT_DPINCDTLAGRANGE].bNeg = 1;
  output[OUT_DPINCDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_DPINCDTLAGRANGE].iNum = 1;
  fnWrite[OUT_DPINCDTLAGRANGE] = &WriteBodyDPincDtLagrange;
  
  sprintf(output[OUT_DQINCDTLAGRANGE].cName,"DQincDtLagrange");
  sprintf(output[OUT_DQINCDTLAGRANGE].cDescr,"Body's dq/dt in Lagrange");
  sprintf(output[OUT_DQINCDTLAGRANGE].cNeg,"1/year");
  output[OUT_DQINCDTLAGRANGE].bNeg = 1;
  output[OUT_DQINCDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_DQINCDTLAGRANGE].iNum = 1;
  fnWrite[OUT_DQINCDTLAGRANGE] = &WriteBodyDQincDtLagrange;
}

void FinalizeOutputFunctionLagrange(OUTPUT *output,int iBody,int iModule) {
  
}


/************ LAGRANGE Logging Functions **************/

void LogOptionsLagrange(CONTROL *control, FILE *fp) {

  fprintf(fp,"-------- LAGRANGE Options -----\n\n");
  
}

void LogLagrange(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  fprintf(fp,"\n----- LAGRANGE PARAMETERS ------\n");
  for (iOut=OUTSTARTLAGRANGE;iOut<OUTBODYSTARTLAGRANGE;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }
}

void LogBodyLagrange(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- LAGRANGE PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTLAGRANGE;iOut<OUTENDLAGRANGE;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

/************* MODULE Functions ***********/

void AddModuleLagrange(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = LAGRANGE;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlLagrange;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyLagrange;
  module->fnCountHalts[iBody][iModule] = &CountHaltsLagrange;
  module->fnLogBody[iBody][iModule] = &LogBodyLagrange;

  module->fnReadOptions[iBody][iModule] = &ReadOptionsLagrange;
  module->fnVerify[iBody][iModule] = &VerifyLagrange;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltLagrange;
//   module->fnVerifyRotation[iBody][iModule] = &VerifyRotationLagrange;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyLagrange;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateLagrange;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputLagrange;
  module->fnFinalizeUpdateHecc[iBody][iModule] = &FinalizeUpdateHeccLagrange;
  module->fnFinalizeUpdateKecc[iBody][iModule] = &FinalizeUpdateKeccLagrange;
  module->fnFinalizeUpdatePinc[iBody][iModule] = &FinalizeUpdatePincLagrange;
  module->fnFinalizeUpdateQinc[iBody][iModule] = &FinalizeUpdateQincLagrange;

  //module->fnInitializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionEqtide;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionLagrange;

}

/************* Lagrange Functions ************/

void PropertiesLagrange(BODY *body,int iBody) {
  
}

// 
/* 
 * Semi-major axis functions
 */

#define A(iIndexJ) dAxRatio,iIndexJ,0.5
#define B(iIndexJ) dAxRatio,iIndexJ,1.5
#define C(iIndexJ) dAxRatio,iIndexJ,2.5

// A -> s = 1/2
// B -> s = 3/2
// C -> s = 5/2

double fdLaplaceCoeff(double dAxRatio, int iIndexJ, double dIndexS) {
/* Calculates Laplace coefficients via series form (M&D eqn 6.68) taking dAxRatio = ratio of semi-major axes and j and s as arguments */
  double fac = 1.0, sum = 1.0, term = 1.0;
  int k, n = 1;
  if (iIndexJ == 1)
    fac = dIndexS * dAxRatio;
  else {
    for (k = 1; k <= iIndexJ; k++)
      fac *= (dIndexS + k - 1.0) / (k) * dAxRatio;
  }
 
  while (term >= 1.0e-15*sum) {
    term = 1.0;
    for (k = 1; k <= n; k++) {
      term *= (dIndexS + k - 1.0) * (dIndexS + iIndexJ + k - 1.0) / (k * (iIndexJ + k)) * dAxRatio*dAxRatio;
    }
    sum = sum + term;
    
    n++;
  }
  return 2.0*fac*sum;
}

double fdDerivLaplaceCoeff(int iNthDeriv, double dAxRatio, int iIndexJ, double dIndexS) {
/* Calculates nth order derivative of Laplace coefficient using a recursive scheme */
  double result;

  if (iNthDeriv == 1)
    result = dIndexS * (fdLaplaceCoeff(dAxRatio, abs(iIndexJ-1), dIndexS + 1.0) - 2 * dAxRatio * \
      fdLaplaceCoeff(dAxRatio, iIndexJ, dIndexS + 1.0) + fdLaplaceCoeff(dAxRatio, iIndexJ + 1, dIndexS + 1.0));
  else if (iNthDeriv == 2)
    result = dIndexS * (fdDerivLaplaceCoeff(1,dAxRatio,abs(iIndexJ-1),dIndexS+1.) - 2 * dAxRatio * \
      fdDerivLaplaceCoeff(1,dAxRatio,iIndexJ,dIndexS+1.)+fdDerivLaplaceCoeff(1,dAxRatio,iIndexJ+1,dIndexS+1.) -\
      2 * fdLaplaceCoeff(dAxRatio,iIndexJ,dIndexS+1.));
  else
    result = dIndexS * (fdDerivLaplaceCoeff(iNthDeriv-1,dAxRatio,abs(iIndexJ-1),dIndexS+1.) - 2 * dAxRatio * \
      fdDerivLaplaceCoeff(iNthDeriv-1,dAxRatio,iIndexJ,dIndexS+1.)+fdDerivLaplaceCoeff(iNthDeriv-1,dAxRatio,iIndexJ+1,dIndexS+1.) - 2 * (iNthDeriv-1) * fdDerivLaplaceCoeff(iNthDeriv-2,dAxRatio,iIndexJ,dIndexS+1.));
  return result;
}

double fdSemiMajAxF1(double dAxRatio, int iIndexJ) {
  return 1./2 * fdLaplaceCoeff(A(iIndexJ));
}

double fdSemiMajAxF2(double dAxRatio, int iIndexJ) {
  return 1./8* (-4.*iIndexJ*iIndexJ * fdLaplaceCoeff(A(iIndexJ)) + 2.*dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ))\
    + dAxRatio*dAxRatio * fdDerivLaplaceCoeff(2,A(iIndexJ)) );
}

double fdSemiMajAxF3(double dAxRatio, int iIndexJ) {
  return -1./4*dAxRatio * ( fdLaplaceCoeff(B(abs(iIndexJ-1))) + fdLaplaceCoeff(B(iIndexJ+1)) );
}

double fdSemiMajAxF4(double dAxRatio, int iIndexJ) {
  return 1./128 * ( (-9.*iIndexJ*iIndexJ + 16.*pow(iIndexJ,4)) * fdLaplaceCoeff(A(iIndexJ))  \
    -8.*iIndexJ*iIndexJ*dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ)) \
    - 8.*iIndexJ*iIndexJ*pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,A(iIndexJ)) \
    + 4.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ)) );
}

double fdSemiMajAxF5(double dAxRatio, int iIndexJ) {
  return 1./32 * ( 16.*pow(iIndexJ,4) * fdLaplaceCoeff(A(iIndexJ)) \
    + (4. - 16.*iIndexJ*iIndexJ) * dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ)) \
    + (14. - 8.*iIndexJ*iIndexJ) * pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,A(iIndexJ)) \
    + 8.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ)) );
}

double fdSemiMajAxF6(double dAxRatio, int iIndexJ) {
  return 1./128 * ( (-17.*iIndexJ*iIndexJ + 16.*pow(iIndexJ,4)) * fdLaplaceCoeff(A(iIndexJ)) \
    + (1. - iIndexJ*iIndexJ) * 24. * dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ)) \
    + (36. - 8.*iIndexJ*iIndexJ) * pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,A(iIndexJ)) \
    + 12.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ)) );
}

double fdSemiMajAxF7(double dAxRatio, int iIndexJ) {
  return 1./16*( (-2.+4.*iIndexJ*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(abs(iIndexJ-1)))+fdLaplaceCoeff(B(iIndexJ+1)))
    - 4.*pow(dAxRatio,2) * (fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) + fdDerivLaplaceCoeff(1,B(iIndexJ+1))) \
    - pow(dAxRatio,3) * (fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) + fdDerivLaplaceCoeff(2,B(iIndexJ+1))) );
}

double fdSemiMajAxF8(double dAxRatio, int iIndexJ) {
  return 3./16 * dAxRatio*dAxRatio * ( fdLaplaceCoeff(C(abs(iIndexJ-2))) \
    + 4. * fdLaplaceCoeff(C(iIndexJ)) + fdLaplaceCoeff(C(iIndexJ+2)) );
}

double fdSemiMajAxF9(double dAxRatio, int iIndexJ) {
  return 1./4 * dAxRatio * (fdLaplaceCoeff(B(abs(iIndexJ-1))) + fdLaplaceCoeff(B(iIndexJ+1))) \
    + 3./8 * pow(dAxRatio,2) * ( fdLaplaceCoeff(C(abs(iIndexJ-2))) + 10. * fdLaplaceCoeff(C(iIndexJ)) \
    + fdLaplaceCoeff(C(iIndexJ+2)) );
}

double fdSemiMajAxF10(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (2. + 6.*iIndexJ + 4.*iIndexJ*iIndexJ) * fdLaplaceCoeff(A(iIndexJ+1)) \
    - 2. * dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ+1)) \
    - pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,A(iIndexJ+1)) );
}

double fdSemiMajAxF11(double dAxRatio, int iIndexJ) {
  return 1./32*((-6.*iIndexJ-26.*iIndexJ*iIndexJ-36.*pow(iIndexJ,3) \
    -16*pow(iIndexJ,4))*fdLaplaceCoeff(A(iIndexJ+1))\
    + dAxRatio * (6*iIndexJ + 12*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(1,A(iIndexJ+1)) \
    + pow(dAxRatio,2) * (-4. + 7*iIndexJ + 8*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(2,A(iIndexJ+1)) \
    - 6.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ+1)) \
    - pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ+1)) );
}

double fdSemiMajAxF12(double dAxRatio, int iIndexJ) {
  return 1./32 * ( (4. + 2.*iIndexJ - 22.*iIndexJ*iIndexJ - 36.*pow(iIndexJ,3) - 16.*pow(iIndexJ,4)) * \
    fdLaplaceCoeff(A(iIndexJ+1)) \
    + dAxRatio * (-4. + 22.*iIndexJ + 20.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(1,A(iIndexJ+1)) \
    + pow(dAxRatio,2) * (-22. + 7.*iIndexJ + 8.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(2,A(iIndexJ+1)) \
    - 10.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ+1)) \
    - pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ+1)) );
}

double fdSemiMajAxF13(double dAxRatio, int iIndexJ) {
  return 1./8*((-6.*iIndexJ-4.*iIndexJ*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(iIndexJ))+fdLaplaceCoeff(B(iIndexJ+2)))\
    + 4.*pow(dAxRatio,2) * (fdDerivLaplaceCoeff(1,B(iIndexJ)) + fdDerivLaplaceCoeff(1,B(iIndexJ+2)))\
    + pow(dAxRatio,3) * (fdDerivLaplaceCoeff(2,B(iIndexJ)) + fdDerivLaplaceCoeff(2,B(iIndexJ+2))) );
}

double fdSemiMajAxF14(double dAxRatio, int iIndexJ) {
  return dAxRatio * fdLaplaceCoeff(B(iIndexJ+1));
}

double fdSemiMajAxF15(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (2. - 4.*iIndexJ*iIndexJ) * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) \
   + 4.*pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ+1)) \
   + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ+1)) );
}

double fdSemiMajAxF16(double dAxRatio, int iIndexJ) {
  return -1./2 * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) \
    -3.* pow(dAxRatio,2) * fdLaplaceCoeff(C(iIndexJ)) - 3./2 * pow(dAxRatio,2) * fdLaplaceCoeff(C(iIndexJ+2));
}

double fdSemiMajAxF17(double dAxRatio, int iIndexJ) {
  return 1./64 * ( (12. + 64.*iIndexJ + 109.*iIndexJ*iIndexJ + 72.*pow(iIndexJ,3) + 16.*pow(iIndexJ,4)) \
    * fdLaplaceCoeff(A(iIndexJ+2)) \
    - dAxRatio * (12. + 28.*iIndexJ + 16.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(1,A(iIndexJ+2)) \
    + pow(dAxRatio,2) * (6. - 14.*iIndexJ - 8.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(2,A(iIndexJ+2)) \
    + 8.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ+2)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ+2)) );
}

double fdSemiMajAxF18(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (12. - 15.*iIndexJ + 4.*iIndexJ*iIndexJ) * dAxRatio * fdLaplaceCoeff(B(abs(iIndexJ-1))) \
    + pow(dAxRatio,2) * (8. - 4.*iIndexJ) * fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) \
    + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) );
}

double fdSemiMajAxF19(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (6. - 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ)) \
   + pow(dAxRatio,2) * (-4. + 4.*iIndexJ) * fdDerivLaplaceCoeff(1,B(iIndexJ)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ)) );
}

double fdSemiMajAxF20(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (3. + 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) \
   - 4.*iIndexJ * pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ+1)) \
   + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ+1)) );
}

double fdSemiMajAxF21(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (-12. + 15.*iIndexJ - 4.*iIndexJ*iIndexJ) * dAxRatio * fdLaplaceCoeff(B(abs(iIndexJ-1))) \
   + pow(dAxRatio,2) * (-8. + 4.*iIndexJ) * fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) );
}

double fdSemiMajAxF22(double dAxRatio, int iIndexJ) {
  return 1./4 * ( dAxRatio * iIndexJ * (6. + 4.*iIndexJ) * fdLaplaceCoeff(B(iIndexJ)) \
   - 4. * pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ)) );
}

double fdSemiMajAxF23(double dAxRatio, int iIndexJ) {
  return 1./4 * ( dAxRatio * iIndexJ * (6. + 4.*iIndexJ) * fdLaplaceCoeff(B(iIndexJ+2)) \
   - 4. * pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ+2)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ+2)) );
}

double fdSemiMajAxF24(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (-6. + 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ))	\
   + 4.*pow(dAxRatio,2) * (1. - iIndexJ) * fdDerivLaplaceCoeff(1,B(iIndexJ))    \
   + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ)) );
}

double fdSemiMajAxF25(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (-3. - 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) \
   + 4.*iIndexJ*pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ+1)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ+1)) );
}

double fdSemiMajAxF26(double dAxRatio, int iIndexJ) {
  return 1./2 * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) + 3./4 * pow(dAxRatio,2) * fdLaplaceCoeff(C(iIndexJ)) \
    + 3./2 * pow(dAxRatio,2) * fdLaplaceCoeff(C(iIndexJ+2));
}


//----------------Disturbing function h k p q----------------------------------------------
//--------dR/dh-----------(inner body)------------------------------------------------------
double fdDdistDhDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dHecc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][1] + 2*(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][3] + (pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][4] + (pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2)+pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][6] );
}

double fdDdistDhDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dHecc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][9] + (pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][11] + (pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2)+pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][12] ) + (2*body[iBody].dHecc*body[iBody].dKecc*body[jBody].dKecc + 3*pow(body[iBody].dHecc,2)*body[jBody].dHecc+body[jBody].dHecc*pow(body[iBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][10];
} 
 
double fdDdistDhDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dHecc*(body[iBody].dPinc*body[jBody].dPinc+body[iBody].dQinc*body[jBody].dQinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][14];
}

double fdDdistDhDir04(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2*body[iBody].dHecc*(pow(body[jBody].dHecc,2)-pow(body[jBody].dKecc,2))+4*body[jBody].dHecc*body[iBody].dKecc*body[jBody].dKecc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][16];
}

double fdDdistDhDir05(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2*body[iBody].dHecc*(pow(body[iBody].dPinc,2)-pow(body[iBody].dQinc,2))+4*body[iBody].dPinc*body[iBody].dQinc*body[iBody].dKecc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDhDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dHecc*(pow(body[iBody].dPinc,2)-pow(body[iBody].dQinc,2))+2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][18];
}

double fdDdistDhDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*(body[iBody].dHecc*(body[iBody].dPinc*body[jBody].dPinc-body[iBody].dQinc*body[jBody].dQinc) + body[iBody].dKecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][20];
}

double fdDdistDhDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dHecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc) + body[jBody].dKecc*(body[iBody].dPinc*body[jBody].dQinc-body[iBody].dQinc*body[jBody].dPinc) )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][21];
}

double fdDdistDhDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dHecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc) + body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc) )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][22];
}

double fdDdistDhDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dHecc*(body[iBody].dPinc*body[jBody].dPinc-body[iBody].dQinc*body[jBody].dQinc) + body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc+body[iBody].dPinc*body[jBody].dQinc) )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][23];
}
  
double fdDdistDhDir013(BODY *body, SYSTEM *system, int iBody, int jBody) {  
  return ( 2*body[iBody].dHecc*(pow(body[jBody].dPinc,2)-pow(body[jBody].dQinc,2)) + 4*body[jBody].dPinc*body[jBody].dQinc*body[iBody].dKecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][17];
}
  
double fdDdistDhDir014(BODY *body, SYSTEM *system, int iBody, int jBody) { 
  return ( body[jBody].dHecc*(pow(body[jBody].dPinc,2)-pow(body[jBody].dQinc,2)) + 2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][18];
}
  
double fdDdisturbDHecc(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDhDir01(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir04(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir05(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir06(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir08(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir013(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir014(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dh'------------------------------------------------------------------

double fdDdistDhPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dHecc*(system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][1] + (pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][4] + 2*(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][5] + (pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2) + pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][6]);
}

double fdDdistDhPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dHecc*(system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][9] + (pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][10] + (pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2)+pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][12]) + (2*body[iBody].dHecc*body[jBody].dKecc*body[iBody].dKecc + 3*pow(body[iBody].dHecc,2)*body[jBody].dHecc + body[jBody].dHecc*pow(body[iBody].dKecc,2)) * system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][11];
}

double fdDdistDhPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dHecc*(body[iBody].dPinc*body[jBody].dPinc+body[iBody].dQinc*body[jBody].dQinc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][14];
}

double fdDdistDhPrmDir04(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2*body[iBody].dHecc*(pow(body[jBody].dHecc,2)-pow(body[jBody].dKecc,2)) + 4*body[jBody].dHecc*body[jBody].dKecc*body[iBody].dKecc) * system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][16];
}

double fdDdistDhPrmDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dHecc*(pow(body[jBody].dPinc,2)-pow(body[jBody].dQinc,2)) + 2*body[jBody].dKecc*body[jBody].dPinc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDhPrmDir07(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( 2*body[iBody].dHecc*(pow(body[jBody].dPinc,2)-pow(body[jBody].dQinc,2)) + 4*body[jBody].dPinc*body[jBody].dQinc*body[iBody].dKecc ) * system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][19];
}

double fdDdistDhPrmDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dHecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc) + body[jBody].dKecc*(body[iBody].dPinc*body[jBody].dQinc-body[iBody].dQinc*body[jBody].dPinc) ) * system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][21];
}

double fdDdistDhPrmDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dHecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc) + body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc) ) * system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][22];
}

double fdDdistDhPrmDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dHecc*(-body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc) + body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dPinc+body[iBody].dPinc*body[jBody].dQinc) )* system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][23];
}

double fdDdistDhPrmDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dHecc*(body[iBody].dPinc*body[jBody].dPinc-body[iBody].dQinc*body[jBody].dQinc) + body[iBody].dKecc*(body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc) )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][24];
}

double fdDdistDhPrmDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dHecc*(pow(body[iBody].dPinc,2)-pow(body[iBody].dQinc,2)) + 2*body[jBody].dKecc*body[iBody].dPinc*body[iBody].dQinc ) *system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDhPrmDir015(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dHecc*(pow(body[iBody].dPinc,2)-pow(body[iBody].dQinc,2)) + 2*body[iBody].dPinc*body[iBody].dQinc*body[iBody].dKecc ) * system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][19];
}

double fdDdisturbDHeccPrime(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDhPrmDir01(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir04(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir06(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir07(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir012(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir014(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir015(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//------------dR/dk--------------------------------------------------------------

double fdDdistDkDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dKecc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][1] + 2*(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][3] + (pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][4] + (pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2) + pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][6] );
}

double fdDdistDkDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dKecc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][9] + (pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][11] + (pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2)+pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][12] ) + (3*pow(body[iBody].dKecc,2)*body[jBody].dKecc+body[jBody].dKecc*pow(body[iBody].dHecc,2)+2*body[iBody].dKecc*body[iBody].dHecc*body[jBody].dHecc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][10];
}

double fdDdistDkDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dKecc*(body[iBody].dPinc*body[jBody].dPinc+body[iBody].dQinc*body[jBody].dQinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][14];
}

double fdDdistDkDir04(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( 2*body[iBody].dKecc*(pow(body[jBody].dKecc,2)-pow(body[jBody].dHecc,2)) + 4*body[iBody].dHecc*body[jBody].dHecc*body[jBody].dKecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][16];
}

double fdDdistDkDir05(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( 2*body[iBody].dKecc*(pow(body[iBody].dQinc,2)-pow(body[iBody].dPinc,2)) + 4*body[iBody].dPinc*body[iBody].dQinc*body[iBody].dHecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDkDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*(pow(body[iBody].dQinc,2)-pow(body[iBody].dPinc,2)) + 2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc )* system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][18];
}

double fdDdistDkDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc - body[jBody].dPinc*body[iBody].dPinc) + body[iBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc + body[jBody].dQinc*body[iBody].dPinc) ) * system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][20];
}

double fdDdistDkDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc + body[jBody].dPinc*body[iBody].dPinc) + body[jBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc - body[jBody].dQinc*body[iBody].dPinc) ) * system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][21];
}

double fdDdistDkDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc + body[jBody].dPinc*body[iBody].dPinc) + body[jBody].dHecc*(body[jBody].dQinc*body[iBody].dPinc - body[jBody].dPinc*body[iBody].dQinc) ) * system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][22];
}

double fdDdistDkDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc) + body[jBody].dHecc*(body[jBody].dPinc*body[iBody].dQinc+body[jBody].dQinc*body[iBody].dPinc) ) * system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][23];
}

double fdDdistDkDir013(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( 2*body[iBody].dKecc*(pow(body[jBody].dQinc,2)-pow(body[jBody].dPinc,2)) + 4*body[jBody].dPinc*body[jBody].dQinc*body[iBody].dHecc ) * system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDkDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*(pow(body[jBody].dQinc,2)-pow(body[jBody].dPinc,2)) + 2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc ) * system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][18];
}

double fdDdisturbDKecc(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDkDir01(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir04(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir05(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir06(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir08(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir013(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir014(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//----------dR/dk'------------------------------------------------------------
double fdDdistDkPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dKecc*(system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][1] + (pow(body[jBody].dHecc,2) + pow(body[jBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][4] + 2*(pow(body[iBody].dHecc,2) + pow(body[iBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][5] + (pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2) + pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][6]);
}

double fdDdistDkPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dKecc*(system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][9] + (pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][10] + (pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2) + pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][12]) + (pow(body[iBody].dHecc,2)*body[jBody].dKecc+3*body[jBody].dKecc*pow(body[iBody].dKecc,2)+2*body[iBody].dKecc*body[jBody].dHecc*body[iBody].dHecc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][11];
}

double fdDdistDkPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dKecc*(body[jBody].dPinc*body[iBody].dPinc+body[iBody].dQinc*body[jBody].dQinc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][14];
}

double fdDdistDkPrmDir04(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dKecc*(pow(body[jBody].dKecc,2)-pow(body[jBody].dHecc,2)) + 2*body[jBody].dHecc*body[iBody].dHecc*body[jBody].dKecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][16];
}

double fdDdistDkPrmDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*(pow(body[jBody].dQinc,2) - pow(body[jBody].dPinc,2)) + 2*body[jBody].dHecc*body[jBody].dPinc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDkPrmDir07(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dKecc*(pow(body[jBody].dQinc,2)-pow(body[jBody].dPinc,2)) + 2*body[jBody].dPinc*body[jBody].dQinc*body[iBody].dHecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][19];
}

double fdDdistDkPrmDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc+body[iBody].dPinc*body[jBody].dPinc) - body[jBody].dHecc*(body[iBody].dPinc*body[jBody].dQinc-body[iBody].dQinc*body[jBody].dPinc) )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][21];
}

double fdDdistDkPrmDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc + body[iBody].dPinc*body[jBody].dPinc) - body[jBody].dHecc*(body[iBody].dQinc*body[jBody].dPinc-body[iBody].dPinc*body[jBody].dQinc) )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][22];
}

double fdDdistDkPrmDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*(body[jBody].dQinc*body[iBody].dQinc-body[jBody].dPinc*body[iBody].dPinc) + body[jBody].dHecc*(body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc) )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][23];
}

double fdDdistDkPrmDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dKecc*(body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc) + body[iBody].dHecc*(body[iBody].dPinc*body[jBody].dQinc+body[iBody].dQinc*body[jBody].dPinc) )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][24];
}

double fdDdistDkPrmDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*(pow(body[iBody].dQinc,2)-pow(body[iBody].dPinc,2)) + 2*body[jBody].dHecc*body[iBody].dPinc*body[iBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDkPrmDir015(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dKecc*(pow(body[iBody].dQinc,2)-pow(body[iBody].dPinc,2)) + 2*body[iBody].dPinc*body[iBody].dQinc*body[iBody].dHecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][19];
}

double fdDdisturbDKeccPrime(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDkPrmDir01(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir04(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir06(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir07(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir012(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir014(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir015(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//------------dR/dp---------------------------------------------------------//
double fdDdistDpDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dPinc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][2] + (pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2)+pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][6] + 2*(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][7] + (pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][8] );
}

double fdDdistDpDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dPinc*(body[iBody].dHecc*body[jBody].dHecc + body[iBody].dKecc*body[jBody].dKecc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][12];
}

double fdDdistDpDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dPinc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][13] + (pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2)+pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][14] + (pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2)+pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][15] ) + 2*body[iBody].dPinc*(body[iBody].dPinc*body[jBody].dPinc+body[iBody].dQinc*body[jBody].dQinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][15];
}

double fdDdistDpDir05(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dPinc*pow(body[iBody].dHecc,2)-body[iBody].dPinc*pow(body[iBody].dKecc,2)+2*body[iBody].dQinc*body[iBody].dHecc*body[iBody].dKecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDpDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dHecc*body[jBody].dHecc*body[iBody].dPinc - body[iBody].dKecc*body[jBody].dKecc*body[iBody].dPinc + body[jBody].dHecc*body[iBody].dKecc*body[iBody].dQinc + body[jBody].dKecc*body[iBody].dHecc*body[iBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][18];
}

double fdDdistDpDir07(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*(body[iBody].dPinc*pow(body[jBody].dHecc,2) - body[iBody].dPinc*pow(body[jBody].dKecc,2) + 2*body[iBody].dQinc*body[jBody].dHecc*body[jBody].dKecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][19];
}

double fdDdistDpDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dPinc*pow(body[iBody].dHecc,2)-body[jBody].dPinc*pow(body[iBody].dKecc,2)+2*body[iBody].dHecc*body[iBody].dKecc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][20];
}

double fdDdistDpDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*body[iBody].dKecc*body[jBody].dPinc + body[jBody].dHecc*body[iBody].dHecc*body[jBody].dPinc - body[jBody].dHecc*body[iBody].dKecc*body[jBody].dQinc + body[jBody].dKecc*body[iBody].dHecc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][21];
}

double fdDdistDpDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*body[iBody].dKecc*body[jBody].dPinc + body[jBody].dHecc*body[iBody].dHecc*body[jBody].dPinc + body[jBody].dHecc*body[iBody].dKecc*body[jBody].dQinc - body[jBody].dKecc*body[iBody].dHecc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][22];
}

double fdDdistDpDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( -body[jBody].dKecc*body[iBody].dKecc*body[jBody].dPinc + body[jBody].dHecc*body[iBody].dHecc*body[jBody].dPinc + body[jBody].dHecc*body[iBody].dKecc*body[jBody].dQinc + body[jBody].dKecc*body[iBody].dHecc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][23];
}

double fdDdistDpDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( -pow(body[jBody].dKecc,2)*body[jBody].dPinc + pow(body[jBody].dHecc,2)*body[jBody].dPinc + 2*body[jBody].dHecc*body[jBody].dKecc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][24];
}

double fdDdistDpDir016(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*(body[iBody].dPinc*pow(body[jBody].dPinc,2)-body[iBody].dPinc*pow(body[jBody].dQinc,2)+2*body[jBody].dPinc*body[jBody].dQinc*body[iBody].dQinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][25];
}

double fdDdisturbDPinc(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDpDir01(body, system, iaBody[0], iaBody[1]) \
     + fdDdistDpDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir05(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir06(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir07(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir08(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir012(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir016(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//-------dR/dp'------------------------------------------------------------//
double fdDdistDpPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dPinc*( system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][2] + (pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2)+pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][6] + 2*(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][7] + (pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][8] );
}

double fdDdistDpPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dPinc*(body[jBody].dHecc*body[iBody].dHecc+body[jBody].dKecc*body[iBody].dKecc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][12];
}

double fdDdistDpPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dPinc*( system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][13] + (pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2)+pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][14] + (pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2)+pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][15] ) + 2*body[iBody].dPinc*(body[jBody].dPinc*body[iBody].dPinc+body[jBody].dQinc*body[iBody].dQinc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][15];
}

double fdDdistDpPrmDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( -pow(body[jBody].dKecc,2)*body[jBody].dPinc+pow(body[jBody].dHecc,2)*body[jBody].dPinc+2*body[jBody].dHecc*body[jBody].dKecc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][20];
}

double fdDdistDpPrmDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[iBody].dKecc*body[jBody].dKecc*body[jBody].dPinc + body[iBody].dHecc*body[jBody].dHecc*body[jBody].dPinc + body[iBody].dHecc*body[jBody].dKecc*body[jBody].dQinc - body[iBody].dKecc*body[jBody].dHecc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][21];
}

double fdDdistDpPrmDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[iBody].dKecc*body[jBody].dKecc*body[jBody].dPinc + body[iBody].dHecc*body[jBody].dHecc*body[jBody].dPinc - body[iBody].dHecc*body[jBody].dKecc*body[jBody].dQinc + body[iBody].dKecc*body[jBody].dHecc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][22];
}

double fdDdistDpPrmDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( -body[iBody].dKecc*body[jBody].dKecc*body[jBody].dPinc + body[iBody].dHecc*body[jBody].dHecc*body[jBody].dPinc + body[iBody].dHecc*body[jBody].dKecc*body[jBody].dQinc + body[iBody].dKecc*body[jBody].dHecc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][23];
}

double fdDdistDpPrmDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( -pow(body[iBody].dKecc,2)*body[jBody].dPinc + pow(body[iBody].dHecc,2)*body[jBody].dPinc + 2*body[iBody].dHecc*body[iBody].dKecc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][24];
}

double fdDdistDpPrmDir013(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( -body[iBody].dPinc*pow(body[jBody].dKecc,2) + body[iBody].dPinc*pow(body[jBody].dHecc,2) + 2*body[iBody].dQinc*body[jBody].dHecc*body[jBody].dKecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][17];
}

double fdDdistDpPrmDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( -body[iBody].dKecc*body[jBody].dKecc*body[iBody].dPinc + body[iBody].dHecc*body[jBody].dHecc*body[iBody].dPinc + body[iBody].dHecc*body[jBody].dKecc*body[iBody].dQinc + body[iBody].dKecc*body[jBody].dHecc*body[iBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDpPrmDir015(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( -body[iBody].dPinc*pow(body[iBody].dKecc,2) + body[iBody].dPinc*pow(body[iBody].dHecc,2) + 2*body[iBody].dQinc*body[iBody].dHecc*body[iBody].dKecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][19];
}

double fdDdistDpPrmDir016(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( -body[iBody].dPinc*pow(body[jBody].dQinc,2) + pow(body[jBody].dPinc,2)*body[iBody].dPinc + 2*body[iBody].dQinc*body[jBody].dPinc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][25];
}

double fdDdisturbDPincPrime(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDpPrmDir01(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir08(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir012(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir013(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir014(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir015(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir016(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//--------------dR/dq-------------------------------------------------------//
double fdDdistDqDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dQinc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][2] + (pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2)+pow(body[jBody].dKecc,2)+pow(body[jBody].dHecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][6] + 2*(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][7] + (pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][8] );
}

double fdDdistDqDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dQinc*(body[iBody].dHecc*body[jBody].dHecc+body[iBody].dKecc*body[jBody].dKecc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][12];
}

double fdDdistDqDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dQinc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][13] + (pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2)+pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][14] + (pow(body[iBody].dPinc,2) + pow(body[iBody].dQinc,2) + pow(body[jBody].dPinc,2) + pow(body[jBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][15] ) + 2*body[iBody].dQinc*(body[iBody].dPinc*body[jBody].dPinc+body[iBody].dQinc*body[jBody].dQinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][15];
}

double fdDdistDqDir05(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dQinc*pow(body[iBody].dKecc,2) - body[iBody].dQinc*pow(body[iBody].dHecc,2) + 2*body[iBody].dPinc*body[iBody].dHecc*body[iBody].dKecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDqDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[jBody].dKecc*body[iBody].dKecc*body[iBody].dQinc - body[jBody].dHecc*body[iBody].dHecc*body[iBody].dQinc + body[jBody].dHecc*body[iBody].dKecc*body[iBody].dPinc + body[jBody].dKecc*body[iBody].dHecc*body[iBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][18];
}

double fdDdistDqDir07(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dQinc*pow(body[jBody].dKecc,2) - body[iBody].dQinc*pow(body[jBody].dHecc,2) + 2*body[iBody].dPinc*body[jBody].dHecc*body[jBody].dKecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][19];
}

double fdDdistDqDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( pow(body[iBody].dKecc,2)*body[jBody].dQinc - pow(body[iBody].dHecc,2)*body[jBody].dQinc + 2*body[iBody].dHecc*body[iBody].dKecc*body[jBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][20];
}

double fdDdistDqDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*body[iBody].dKecc*body[jBody].dQinc + body[jBody].dHecc*body[iBody].dHecc*body[jBody].dQinc + body[jBody].dHecc*body[iBody].dKecc*body[jBody].dPinc - body[jBody].dKecc*body[iBody].dHecc*body[jBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][21];
}

double fdDdistDqDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*body[iBody].dKecc*body[jBody].dQinc + body[iBody].dHecc*body[jBody].dHecc*body[jBody].dQinc - body[jBody].dHecc*body[iBody].dKecc*body[jBody].dPinc + body[jBody].dKecc*body[iBody].dHecc*body[jBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][22];
}

double fdDdistDqDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dKecc*body[iBody].dKecc*body[jBody].dQinc - body[jBody].dHecc*body[iBody].dHecc*body[jBody].dQinc + body[jBody].dHecc*body[iBody].dKecc*body[jBody].dPinc + body[jBody].dKecc*body[iBody].dHecc*body[jBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][23];
}

double fdDdistDqDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( pow(body[jBody].dKecc,2)*body[jBody].dQinc - pow(body[jBody].dHecc,2)*body[jBody].dQinc + 2*body[jBody].dHecc*body[jBody].dKecc*body[jBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][24];
}

double fdDdistDqDir016(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dQinc*pow(body[jBody].dQinc,2) - body[iBody].dQinc*pow(body[jBody].dPinc,2) + 2*body[jBody].dPinc*body[jBody].dQinc*body[iBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][25];
}

  
double fdDdisturbDQinc(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDqDir01(body, system, iaBody[0], iaBody[1]) \
     + fdDdistDqDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir05(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir06(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir07(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir08(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir012(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir016(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//------dR/dq'--------------------------------------------------------------//
double fdDdistDqPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dQinc*( system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][2] + (pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2)+pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][6] + 2*(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][7] + (pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][8] );
}

double fdDdistDqPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dQinc*(body[jBody].dHecc*body[iBody].dHecc+body[jBody].dKecc*body[iBody].dKecc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][12];
}

double fdDdistDqPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dQinc*( system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][13] + (pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2)+pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][14] + (pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2)+pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][15] ) + 2*body[iBody].dQinc*(body[jBody].dPinc*body[iBody].dPinc+body[jBody].dQinc*body[iBody].dQinc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][15];
}

double fdDdistDqPrmDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( pow(body[jBody].dKecc,2)*body[jBody].dQinc - pow(body[jBody].dHecc,2)*body[jBody].dQinc + 2*body[jBody].dHecc*body[jBody].dKecc*body[jBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][20];
}

double fdDdistDqPrmDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[iBody].dKecc*body[jBody].dKecc*body[jBody].dQinc + body[iBody].dHecc*body[jBody].dHecc*body[jBody].dQinc - body[iBody].dHecc*body[jBody].dKecc*body[jBody].dPinc + body[iBody].dKecc*body[jBody].dHecc*body[jBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][21];
}

double fdDdistDqPrmDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[iBody].dKecc*body[jBody].dKecc*body[jBody].dQinc + body[iBody].dHecc*body[jBody].dHecc*body[jBody].dQinc + body[iBody].dHecc*body[jBody].dKecc*body[jBody].dPinc - body[iBody].dKecc*body[jBody].dHecc*body[jBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][22];
}

double fdDdistDqPrmDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[iBody].dKecc*body[jBody].dKecc*body[jBody].dQinc - body[iBody].dHecc*body[jBody].dHecc*body[jBody].dQinc + body[iBody].dHecc*body[jBody].dKecc*body[jBody].dPinc + body[iBody].dKecc*body[jBody].dHecc*body[jBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][23];
}

double fdDdistDqPrmDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( pow(body[iBody].dKecc,2)*body[jBody].dQinc - pow(body[iBody].dHecc,2)*body[jBody].dQinc + 2*body[iBody].dHecc*body[iBody].dKecc*body[jBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][24];
}
  
double fdDdistDqPrmDir013(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dQinc*pow(body[jBody].dKecc,2) - body[iBody].dQinc*pow(body[jBody].dHecc,2) + 2*body[iBody].dPinc*body[jBody].dHecc*body[jBody].dKecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][17];
}

double fdDdistDqPrmDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dQinc*body[jBody].dKecc*body[iBody].dKecc - body[iBody].dQinc*body[jBody].dHecc*body[iBody].dHecc + body[iBody].dHecc*body[jBody].dKecc*body[iBody].dPinc + body[iBody].dKecc*body[jBody].dHecc*body[iBody].dPinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDqPrmDir015(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dQinc*pow(body[iBody].dKecc,2) - body[iBody].dQinc*pow(body[iBody].dHecc,2) + 2*body[iBody].dPinc*body[iBody].dHecc*body[iBody].dKecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][19];
}

double fdDdistDqPrmDir016(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( pow(body[jBody].dQinc,2)*body[iBody].dQinc - pow(body[jBody].dPinc,2)*body[iBody].dQinc + 2*body[iBody].dPinc*body[jBody].dPinc*body[jBody].dQinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][25];
} 
    
double fdDdisturbDQincPrime(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDqPrmDir01(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir08(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir012(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir013(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir014(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir015(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir016(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}



//-------------------Lagrange's equations in h k p q -------------------------------------
double fdLagrangeDhDt(BODY *body, SYSTEM *system, int *iaBody) {
  int i;
  double sum = 0.0, dMu;
  //Here, iaBody[0] = body in question, iaBody[1] = perturber

  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += ( sqrt(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2))*fdDdisturbDKecc(body, system, iaBody) + body[iaBody[0]].dKecc*(body[iaBody[0]].dPinc*fdDdisturbDPinc(body, system, iaBody) +body[iaBody[0]].dQinc*fdDdisturbDQinc(body, system, iaBody))/(2*sqrt(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2))) ) / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      
  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += ( sqrt(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2))*fdDdisturbDKeccPrime(body, system, iaBody) + body[iaBody[0]].dKecc*(body[iaBody[0]].dPinc*fdDdisturbDPincPrime(body, system, iaBody) +body[iaBody[0]].dQinc*fdDdisturbDQincPrime(body, system, iaBody))/(2*sqrt(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2))) ) / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
  }
  
  return sum/DAYSEC;
}

double fdLagrangeDkDt(BODY *body, SYSTEM *system, int *iaBody) {
  int i;
  double sum = 0.0, dMu;
  
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += -( sqrt(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2))*fdDdisturbDHecc(body, system, iaBody) + body[iaBody[0]].dHecc*(body[iaBody[0]].dPinc*fdDdisturbDPinc(body, system, iaBody) +body[iaBody[0]].dQinc*fdDdisturbDQinc(body, system, iaBody))/(2*sqrt(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2))) ) / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      
  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += -( sqrt(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2))*fdDdisturbDHeccPrime(body, system, iaBody) + body[iaBody[0]].dHecc*(body[iaBody[0]].dPinc*fdDdisturbDPincPrime(body, system, iaBody) +body[iaBody[0]].dQinc*fdDdisturbDQincPrime(body, system, iaBody))/(2*sqrt(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2))) ) / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
  }      

  return sum/DAYSEC;
}

double fdLagrangeDpDt(BODY *body, SYSTEM *system, int *iaBody) {
    int i;
    double sum = 0.0, dMu;
    
    dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
    if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dPinc*(-body[iaBody[0]].dKecc*fdDdisturbDHecc(body, system, iaBody)+body[iaBody[0]].dHecc*fdDdisturbDKecc(body, system, iaBody)) + 1.0/2.0*fdDdisturbDQinc(body, system, iaBody) )/(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2))));
    } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dPinc*(-body[iaBody[0]].dKecc*fdDdisturbDHeccPrime(body, system, iaBody)+body[iaBody[0]].dHecc*fdDdisturbDKeccPrime(body, system, iaBody)) + 1.0/2.0*fdDdisturbDQincPrime(body, system, iaBody) )/(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2))));
    }
    
    return sum/DAYSEC;
}


double fdLagrangeDqDt(BODY *body, SYSTEM *system, int *iaBody) {
    int i;
    double sum = 0.0, dMu;
    
    dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
    if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dQinc*(-body[iaBody[0]].dKecc*fdDdisturbDHecc(body, system, iaBody)+body[iaBody[0]].dHecc*fdDdisturbDKecc(body, system, iaBody)) - 1.0/2.0*fdDdisturbDPinc(body, system, iaBody) )/(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2))));
    } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dQinc*(-body[iaBody[0]].dKecc*fdDdisturbDHeccPrime(body, system, iaBody)+body[iaBody[0]].dHecc*fdDdisturbDKeccPrime(body, system, iaBody)) - 1.0/2.0*fdDdisturbDPincPrime(body, system, iaBody) )/(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2))));
    }
   
    return sum/DAYSEC;
}

