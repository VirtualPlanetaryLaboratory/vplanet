/********************** DISTORB.C **********************/
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

void InitializeControlDistOrb(CONTROL *control) {
  /* Not sure if I need anything here yet */
}

void InitializeModuleDistOrb(CONTROL *control,MODULE *module) {
  /* Anything here? */
}

void BodyCopyDistOrb(BODY *dest,BODY *src,int iTideModel,int iBody) {
  int iIndex,iPert;

  dest[iBody].dPinc = src[iBody].dPinc;
  dest[iBody].dQinc = src[iBody].dQinc;

  for (iPert=0;iPert<src[iBody].iGravPerts;iPert++)
    dest[iBody].iaGravPerts[iPert] = src[iBody].iaGravPerts[iPert];
}

void InitializeBodyDistOrb(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
  body[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
}

void InitializeUpdateTmpBodyDistOrb(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
  int iBodyPert;
  
  control->Evolve.tmpBody[iBody].iaGravPerts = malloc(body[iBody].iGravPerts);

}

/**************** DISTORB options ********************/

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
    printf(" ");
  } else 
    if (iFile > 0)
      body[iFile-1].dArgP = options->dDefault;
}  

void ReadDfCrit(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be greater than or equal to 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    system->dDfcrit = dTmp;
    if (system->dDfcrit > 1 && control->Io.iVerbose >= VERBALL)
      fprintf(stderr,"WARNING: %s > 1 is not advised (%s:%d).\n",options->cName,files->Infile[iFile].cIn,lTmp);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    AssignDefaultDouble(options,&system->dDfcrit,files->iNumInputs);
}

void ReadGRCorr(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].bGRCorr = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bGRCorr,files->iNumInputs);
}

void ReadInvPlane(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    control->bInvPlane = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&control->bInvPlane,files->iNumInputs);
}

void InitializeOptionsDistOrb(OPTIONS *options,fnReadOption fnRead[]) {
  
  sprintf(options[OPT_INC].cName,"dInc");
  sprintf(options[OPT_INC].cDescr,"Inclination of planet's orbital plane");
  sprintf(options[OPT_INC].cDefault,"0");
  options[OPT_INC].dDefault = 0.0;
  options[OPT_INC].iType = 2;  
  options[OPT_INC].iMultiFile = 1; 
//   options[OPT_INC].dNeg = DEGRAD;
//   sprintf(options[OPT_INC].cNeg,"Degrees");
  fnRead[OPT_INC] = &ReadInc;
  
  sprintf(options[OPT_LONGA].cName,"dLongA");
  sprintf(options[OPT_LONGA].cDescr,"Longitude of ascending node of planet's orbital plane");
  sprintf(options[OPT_LONGA].cDefault,"0");
  options[OPT_LONGA].dDefault = 0.0;
  options[OPT_LONGA].iType = 2;  
  options[OPT_LONGA].iMultiFile = 1; 
//   options[OPT_LONGA].dNeg = DEGRAD;
//   sprintf(options[OPT_LONGA].cNeg,"Degrees");
  fnRead[OPT_LONGA] = &ReadLongA;
  
  sprintf(options[OPT_ARGP].cName,"dArgP");
  sprintf(options[OPT_ARGP].cDescr,"Argument of pericenter of planet's orbit");
  sprintf(options[OPT_ARGP].cDefault,"0");
  options[OPT_ARGP].dDefault = 0.0;
  options[OPT_ARGP].iType = 2;  
  options[OPT_ARGP].iMultiFile = 1; 
//   options[OPT_ARGP].dNeg = DEGRAD;
//   sprintf(options[OPT_ARGP].cNeg,"Degrees");
  fnRead[OPT_ARGP] = &ReadArgP;
  
  sprintf(options[OPT_DFCRIT].cName,"dDfcrit");
  sprintf(options[OPT_DFCRIT].cDescr,"Tolerance parameter for recalculating semi- functions");
  sprintf(options[OPT_DFCRIT].cDefault,"0");
  options[OPT_DFCRIT].dDefault = 0.0;
  options[OPT_DFCRIT].iType = 2;  
  options[OPT_DFCRIT].iMultiFile = 0; 
  fnRead[OPT_DFCRIT] = &ReadDfCrit;
  
  sprintf(options[OPT_GRCORR].cName,"bGRCorr");
  sprintf(options[OPT_GRCORR].cDescr,"Use general relativity correction");
  sprintf(options[OPT_GRCORR].cDefault,"0");
  options[OPT_GRCORR].dDefault = 0;
  options[OPT_GRCORR].iType = 0;  
  options[OPT_GRCORR].iMultiFile = 1; 
  fnRead[OPT_GRCORR] = &ReadGRCorr;
  
  sprintf(options[OPT_INVPLANE].cName,"bInvPlane");
  sprintf(options[OPT_INVPLANE].cDescr,"Convert input coordinates to invariable plane coordinates");
  sprintf(options[OPT_INVPLANE].cDefault,"0");
  options[OPT_INVPLANE].dDefault = 0;
  options[OPT_INVPLANE].iType = 0;  
  options[OPT_INVPLANE].iMultiFile = 0; 
  fnRead[OPT_INVPLANE] = &ReadInvPlane;
}

void ReadOptionsDistOrb(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTDISTORB;iOpt<OPTENDDISTORB;iOpt++) { 
      if (options[iOpt].iType != -1) {
	fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
      }
  }
}
    

/******************* Verify DISTORB ******************/


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
  if (options[OPT_LONGA].iLine[iBody+1] > -1 && options[OPT_LONGP].iLine[iBody+1] > -1 && options[OPT_ARGP].iLine[iBody+1] == -1) 
    return;

  /* If none are set, raise error */
  if (options[OPT_LONGA].iLine[iBody+1] == -1 && options[OPT_LONGP].iLine[iBody+1] == -1 && options[OPT_ARGP].iLine[iBody+1] == -1) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Must set two of %s, %s, and %s in File: %s.\n",options[OPT_LONGA].cName,options[OPT_LONGP].cName,options[OPT_ARGP].cName, cFile);
    exit(EXIT_INPUT);
  }
  
  /* At least 2 must be set */
  if ((options[OPT_LONGA].iLine[iBody+1] == -1 && options[OPT_LONGP].iLine[iBody+1] == -1) || (options[OPT_LONGA].iLine[iBody+1] == -1 && options[OPT_ARGP].iLine[iBody+1] == -1) || (options[OPT_LONGP].iLine[iBody+1] == -1 && options[OPT_ARGP].iLine[iBody+1] == -1)) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Must set two of %s, %s, and %s in File: %s.\n",options[OPT_LONGA].cName,options[OPT_LONGP].cName,options[OPT_ARGP].cName, cFile);
    exit(EXIT_INPUT);
  }

  /* Were all set? */
  if (options[OPT_LONGA].iLine[iBody+1] > -1 && options[OPT_LONGP].iLine[iBody+1] > -1 && options[OPT_ARGP].iLine[iBody+1] > -1) {
    VerifyTripleExit(options[OPT_LONGA].cName,options[OPT_LONGP].cName,options[OPT_ARGP].cName,options[OPT_LONGA].iLine[iBody+1],options[OPT_LONGP].iLine[iBody+1],options[OPT_ARGP].iLine[iBody+1],cFile,iVerbose);
    exit(EXIT_INPUT);
  }

  /* Was LONGA set? */
  if (options[OPT_LONGA].iLine[iBody+1] > -1) {
    
    if (options[OPT_LONGP].iLine[iBody+1] > -1)
      /* LONGA and LONGP were the only two set - Nothing to do */
	 return;
    if (options[OPT_ARGP].iLine[iBody+1] > -1) 
      /* Must get radius from density */
      body[iBody].dLongP = fdCalcLongP(body[iBody].dLongA,body[iBody].dArgP);    
    return;
  }

  /* Was LongP set? */
  if (options[OPT_LONGP].iLine[iBody+1] > -1) {
    
    if (options[OPT_LONGA].iLine[iBody+1] > -1)
      /* LONGA and LONGP were the only two set - Nothing to do */
	 return;
    if (options[OPT_ARGP].iLine[iBody+1] > -1) 
      /* Must get radius from density */
      body[iBody].dLongA = fdCalcLongA(body[iBody].dLongP,body[iBody].dArgP);    
    return;
  }
}

/* In the following, iBody is the current body number that is getting assigned,
   iPert counts the number of bodies perturbing iBody, and iBodyPert is the
   body number of the current perturbing body. */

/* The indexing gets a bit confusing here. iPert = 0 to iGravPerts-1 correspond
   to all perturbing planets,  iPert = iGravPerts corresponds to the stellar general
   relativistic correction, if applied. */

void InitializeHeccDistOrb(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = 2;
  update[iBody].padDHeccDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeKeccDistOrb(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = 2;
  update[iBody].padDKeccDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeHeccDistOrbGR(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = 2;
  update[iBody].padDHeccDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][1] = 0;
}

void InitializeKeccDistOrbGR(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = 2;
  update[iBody].padDKeccDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]][1] = 0;
}

void InitializePincDistOrb(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] = 2;
  update[iBody].padDPincDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeQincDistOrb(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] = 2;
  update[iBody].padDQincDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void VerifyPerturbersDistOrb(BODY *body,int iNumBodies,int iBody) {
  int iPert=0, j;
  
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
  /* Russell's not sure if this is necessary. XXX
  if (x == 0) {
    x = 1.3;
  }
  */
  return N*(x-1) + (y-1) - Nchoosek(x+1, 2);
} 



void VerifyDistOrb(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int i, j=0, iPert=0, jBody=0;
      
  /* The indexing gets a bit confusing here. iPert = 0 to iGravPerts-1 correspond to all perturbing planets, iPert = iGravPerts corresponds to the stellar general relativistic correction, if applied */
  
  /* Setup Semi-major axis functions (LaplaceF) for secular terms*/
  if (iBody == 1) {
    system->fnLaplaceF = malloc(LAPLNUM*sizeof(fnLaplaceFunction*));
    system->fnLaplaceDeriv = malloc(LAPLNUM*sizeof(fnLaplaceFunction*));
    for (j=0;j<LAPLNUM;j++) {
      system->fnLaplaceF[j] = malloc(1*sizeof(fnLaplaceFunction));
      system->fnLaplaceDeriv[j] = malloc(1*sizeof(fnLaplaceFunction));
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
    
    system->fnLaplaceDeriv[0][0] = &fdDSemiF1Dalpha;
    system->fnLaplaceDeriv[1][0] = &fdDSemiF2Dalpha;
    system->fnLaplaceDeriv[2][0] = &fdDSemiF3Dalpha;
    system->fnLaplaceDeriv[3][0] = &fdDSemiF4Dalpha;
    system->fnLaplaceDeriv[4][0] = &fdDSemiF5Dalpha;
    system->fnLaplaceDeriv[5][0] = &fdDSemiF6Dalpha;
    system->fnLaplaceDeriv[6][0] = &fdDSemiF7Dalpha;
    system->fnLaplaceDeriv[7][0] = &fdDSemiF8Dalpha;
    system->fnLaplaceDeriv[8][0] = &fdDSemiF9Dalpha;
    system->fnLaplaceDeriv[9][0] = &fdDSemiF10Dalpha;
    system->fnLaplaceDeriv[10][0] = &fdDSemiF11Dalpha;
    system->fnLaplaceDeriv[11][0] = &fdDSemiF12Dalpha;
    system->fnLaplaceDeriv[12][0] = &fdDSemiF13Dalpha;
    system->fnLaplaceDeriv[13][0] = &fdDSemiF14Dalpha;
    system->fnLaplaceDeriv[14][0] = &fdDSemiF15Dalpha;
    system->fnLaplaceDeriv[15][0] = &fdDSemiF16Dalpha;
    system->fnLaplaceDeriv[16][0] = &fdDSemiF17Dalpha;
    system->fnLaplaceDeriv[17][0] = &fdDSemiF18Dalpha;
    system->fnLaplaceDeriv[18][0] = &fdDSemiF19Dalpha;
    system->fnLaplaceDeriv[19][0] = &fdDSemiF20Dalpha;
    system->fnLaplaceDeriv[20][0] = &fdDSemiF21Dalpha;
    system->fnLaplaceDeriv[21][0] = &fdDSemiF22Dalpha;
    system->fnLaplaceDeriv[22][0] = &fdDSemiF23Dalpha;
    system->fnLaplaceDeriv[23][0] = &fdDSemiF24Dalpha;
    system->fnLaplaceDeriv[24][0] = &fdDSemiF25Dalpha;
    system->fnLaplaceDeriv[25][0] = &fdDSemiF26Dalpha;
    
    system->dmLaplaceC = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(double*));
    system->dmLaplaceD = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(double*));
    system->dmAlpha0 = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(double*));
    for (i=0;i<Nchoosek(control->Evolve.iNumBodies-1,2);i++) {
      system->dmLaplaceC[i] = malloc(LAPLNUM*sizeof(double));
      system->dmLaplaceD[i] = malloc(LAPLNUM*sizeof(double));
      system->dmAlpha0[i] = malloc(LAPLNUM*sizeof(double));
    }
    
    system->imLaplaceN = malloc((control->Evolve.iNumBodies)*sizeof(int*));
    for (i=1;i<control->Evolve.iNumBodies;i++) {
      system->imLaplaceN[i] = malloc((control->Evolve.iNumBodies)*sizeof(int));
    }
    
  }
  if (iBody >= 1) {
    VerifyPericenter(body,control,options,files->Infile[iBody+1].cIn,iBody,control->Io.iVerbose);
    body[iBody].iGravPerts = control->Evolve.iNumBodies - 2; //will need to change this for zero mass particles in future

    VerifyPerturbersDistOrb(body,control->Evolve.iNumBodies,iBody);
    control->Evolve.fnPropsAux[iBody][iModule] = &PropsAuxDistOrb;
    
    CalcHK(body,iBody);
    CalcPQ(body,iBody);
    /* Body updates */
    for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
      /* h = Ecc*sin(LongP) */
      InitializeHeccDistOrb(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = &fdDistOrbDhDt;
      
      /* k = Ecc*cos(LongP) */
      InitializeKeccDistOrb(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = &fdDistOrbDkDt;
      
      /* p = s*sin(LongA) */
      InitializePincDistOrb(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] = &fdDistOrbDpDt;
      
      /* q = s*cos(LongA) */
      InitializeQincDistOrb(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] = &fdDistOrbDqDt;
      
      jBody = body[iBody].iaGravPerts[iPert];
      
      for (j=0;j<LAPLNUM;j++) {
	if (body[iBody].dSemi < body[jBody].dSemi) {  
	    system->imLaplaceN[iBody][jBody] = CombCount(iBody,jBody,control->Evolve.iNumBodies-1);
	    system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceF[j][0](body[iBody].dSemi/body[jBody].dSemi, 0);
	    system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceDeriv[j][0](body[iBody].dSemi/body[jBody].dSemi, 0);    
	    system->dmAlpha0[system->imLaplaceN[iBody][jBody]][j] = body[iBody].dSemi/body[jBody].dSemi;
	} else if (body[iBody].dSemi > body[jBody].dSemi) {
	    system->imLaplaceN[iBody][jBody] = CombCount(jBody,iBody,control->Evolve.iNumBodies-1);
	    system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceF[j][0](body[jBody].dSemi/body[iBody].dSemi, 0);
	    system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceDeriv[j][0](body[jBody].dSemi/body[iBody].dSemi, 0);  
	    system->dmAlpha0[system->imLaplaceN[iBody][jBody]][j] = body[jBody].dSemi/body[iBody].dSemi;
	}
      }
    } 
    if (iBody == control->Evolve.iNumBodies-1) {
      if (control->bInvPlane) {
	inv_plane(body, system, control->Evolve.iNumBodies);
      }
    }
    if (body[iBody].bGRCorr) {
      /* Body updates for general relativistic correction, indexing star as a "perturber"*/
      InitializeHeccDistOrbGR(body,update,iBody,body[iBody].iGravPerts);
      fnUpdate[iBody][update[iBody].iHecc][update[iBody].iaHeccDistOrb[body[iBody].iGravPerts]] = &fdApsidalGRDhDt;
      
      InitializeKeccDistOrbGR(body,update,iBody,body[iBody].iGravPerts);
      fnUpdate[iBody][update[iBody].iKecc][update[iBody].iaKeccDistOrb[body[iBody].iGravPerts]] = &fdApsidalGRDkDt;
    }
  }
  control->fnForceBehavior[iBody][iModule]=&ForceBehaviorDistOrb;
  control->Evolve.fnBodyCopy[iBody][iModule]=&BodyCopyDistOrb;
}


/***************** DISTORB Update *****************/
void InitializeUpdateDistOrb(BODY *body,UPDATE *update,int iBody) {
  if (iBody > 0) {
    if (update[iBody].iNumHecc == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumHecc += body[iBody].iGravPerts;

    if (update[iBody].iNumKecc == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumKecc += body[iBody].iGravPerts;

    if (update[iBody].iNumPinc == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumPinc += body[iBody].iGravPerts;

    if (update[iBody].iNumQinc == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumQinc += body[iBody].iGravPerts;

    if (body[iBody].bGRCorr) {
      update[iBody].iNumHecc += 1;
      update[iBody].iNumKecc += 1;
    }
  }
}

void FinalizeUpdateHeccDistOrb(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* The indexing gets a bit confusing here. iPert = 0 to iGravPerts-1 correspond to all perturbing planets, iPert = iGravPerts corresponds to the stellar general relativistic correction, if applied */

  int iPert;
  
  if (body[iBody].bGRCorr) {
    update[iBody].padDHeccDtDistOrb = malloc((body[iBody].iGravPerts+1)*sizeof(double*));
    update[iBody].iaHeccDistOrb = malloc((body[iBody].iGravPerts+1)*sizeof(int));
    for (iPert=0;iPert<body[iBody].iGravPerts+1;iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTORB;
      update[iBody].iaHeccDistOrb[iPert] = (*iEqn)++;
    }
  } else {
    update[iBody].padDHeccDtDistOrb = malloc(body[iBody].iGravPerts*sizeof(double*));
    update[iBody].iaHeccDistOrb = malloc(body[iBody].iGravPerts*sizeof(int));
    for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTORB;
      update[iBody].iaHeccDistOrb[iPert] = (*iEqn)++;
    }
  } 
}

void FinalizeUpdateKeccDistOrb(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* The indexing gets a bit confusing here. iPert = 0 to iGravPerts-1 correspond to all perturbing planets, iPert = iGravPerts corresponds to the stellar general relativistic correction, if applied */

  int iPert;
  
  if (body[iBody].bGRCorr) {
    update[iBody].padDKeccDtDistOrb = malloc((body[iBody].iGravPerts+1)*sizeof(double*));
    update[iBody].iaKeccDistOrb = malloc((body[iBody].iGravPerts+1)*sizeof(int));
    for (iPert=0;iPert<body[iBody].iGravPerts+1;iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTORB;
      update[iBody].iaKeccDistOrb[iPert] = (*iEqn)++;
    }
  } else {
    update[iBody].padDKeccDtDistOrb = malloc(body[iBody].iGravPerts*sizeof(double*));
    update[iBody].iaKeccDistOrb = malloc(body[iBody].iGravPerts*sizeof(int));
    for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTORB;
      update[iBody].iaKeccDistOrb[iPert] = (*iEqn)++;
    }
  }
}

void FinalizeUpdatePincDistOrb(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  int iPert;
  
  update[iBody].padDPincDtDistOrb = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaPincDistOrb = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTORB;
    update[iBody].iaPincDistOrb[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateQincDistOrb(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  int iPert;
  
  update[iBody].padDQincDtDistOrb = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaQincDistOrb = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTORB;
    update[iBody].iaQincDistOrb[iPert] = (*iEqn)++;
  }
}


/***************** DISTORB Halts *****************/

void CountHaltsDistOrb(HALT *halt,int *iNumHalts) { 
}

void VerifyHaltDistOrb(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
}


/************* DISTORB Outputs ******************/

void WriteBodyDEccDtDistOrb(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += 1./sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2))*(body[iBody].dHecc*(*(update[iBody].padDHeccDtDistOrb[iPert]))+body[iBody].dKecc*(*(update[iBody].padDKeccDtDistOrb[iPert])));
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteBodyDSincDtDistOrb(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += 1./sqrt(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*(body[iBody].dPinc*(*(update[iBody].padDPincDtDistOrb[iPert]))+body[iBody].dQinc*(*(update[iBody].padDQincDtDistOrb[iPert])));
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}  
  
void WriteBodyDLongPDtDistOrb(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += 1./(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2))*(body[iBody].dKecc*(*(update[iBody].padDHeccDtDistOrb[iPert]))-body[iBody].dHecc*(*(update[iBody].padDKeccDtDistOrb[iPert])));
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}  

void WriteBodyDLongADtDistOrb(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += 1./(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2))*(body[iBody].dQinc*(*(update[iBody].padDPincDtDistOrb[iPert]))-body[iBody].dPinc*(*(update[iBody].padDQincDtDistOrb[iPert])));
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
} 

void WriteBodyDIncDtDistOrb(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += 2./sqrt((1-(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2)))*(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2)))*(body[iBody].dPinc*(*(update[iBody].padDPincDtDistOrb[iPert]))+body[iBody].dQinc*(*(update[iBody].padDQincDtDistOrb[iPert])));
  
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

void WriteBodyPinc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dPinc;
  strcpy(cUnit,"");
} 

void WriteBodyQinc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dQinc;
  strcpy(cUnit,"");
} 

void WriteBodyDHeccDtDistOrb(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* need to put check for star's output options in verify */
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDHeccDtDistOrb[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteBodyDKeccDtDistOrb(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* need to put check for star's output options in verify */
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDKeccDtDistOrb[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteBodyDPincDtDistOrb(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* need to put check for star's output options in verify */
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDPincDtDistOrb[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteBodyDQincDtDistOrb(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* need to put check for star's output options in verify */
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDQincDtDistOrb[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}


void InitializeOutputDistOrb(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_DECCDTDISTORB].cName,"DEccDtDistOrb");
  sprintf(output[OUT_DECCDTDISTORB].cDescr,"Body's decc/dt in DistOrb");
  sprintf(output[OUT_DECCDTDISTORB].cNeg,"1/year");
  output[OUT_DECCDTDISTORB].bNeg = 1;
  output[OUT_DECCDTDISTORB].dNeg = YEARSEC;
  output[OUT_DECCDTDISTORB].iNum = 1;
  fnWrite[OUT_DECCDTDISTORB] = &WriteBodyDEccDtDistOrb;
  
  sprintf(output[OUT_DSINCDTDISTORB].cName,"DSincDtDistOrb");
  sprintf(output[OUT_DSINCDTDISTORB].cDescr,"Body's dsin(.5*Inc)/dt in DistOrb");
  sprintf(output[OUT_DSINCDTDISTORB].cNeg,"1/year");
  output[OUT_DSINCDTDISTORB].bNeg = 1;
  output[OUT_DSINCDTDISTORB].dNeg = YEARSEC;
  output[OUT_DSINCDTDISTORB].iNum = 1;
  fnWrite[OUT_DSINCDTDISTORB] = &WriteBodyDSincDtDistOrb;
 
  sprintf(output[OUT_DINCDTDISTORB].cName,"DIncDtDistOrb");
  sprintf(output[OUT_DINCDTDISTORB].cDescr,"Body's dInc/dt in DistOrb");
  sprintf(output[OUT_DINCDTDISTORB].cNeg,"deg/year");
  output[OUT_DINCDTDISTORB].bNeg = 1;
  output[OUT_DINCDTDISTORB].dNeg = YEARSEC/DEGRAD;
  output[OUT_DINCDTDISTORB].iNum = 1;
  fnWrite[OUT_DINCDTDISTORB] = &WriteBodyDIncDtDistOrb;
  
  sprintf(output[OUT_DLONGPDTDISTORB].cName,"DLongPDtDistOrb");
  sprintf(output[OUT_DLONGPDTDISTORB].cDescr,"Body's dvarpi/dt in DistOrb");
  sprintf(output[OUT_DLONGPDTDISTORB].cNeg,"deg/yr");
  output[OUT_DLONGPDTDISTORB].bNeg = 1;
  output[OUT_DLONGPDTDISTORB].dNeg = YEARSEC/DEGRAD;
  output[OUT_DLONGPDTDISTORB].iNum = 1;
  fnWrite[OUT_DLONGPDTDISTORB] = &WriteBodyDLongPDtDistOrb;
  
  sprintf(output[OUT_DLONGADTDISTORB].cName,"DLongADtDistOrb");
  sprintf(output[OUT_DLONGADTDISTORB].cDescr,"Body's dOmega/dt in DistOrb");
  sprintf(output[OUT_DLONGADTDISTORB].cNeg,"deg/yr");
  output[OUT_DLONGADTDISTORB].bNeg = 1;
  output[OUT_DLONGADTDISTORB].dNeg = YEARSEC/DEGRAD;
  output[OUT_DLONGADTDISTORB].iNum = 1;
  fnWrite[OUT_DLONGADTDISTORB] = &WriteBodyDLongADtDistOrb;
  
  sprintf(output[OUT_INC].cName,"Inc");
  sprintf(output[OUT_INC].cDescr,"Body's Inclination in DistOrb");
  sprintf(output[OUT_INC].cNeg,"Deg");
  output[OUT_INC].bNeg = 1;
  output[OUT_INC].dNeg = 1./DEGRAD;
  output[OUT_INC].iNum = 1;
  fnWrite[OUT_INC] = &WriteBodyInc;
  
  sprintf(output[OUT_SINC].cName,"Sinc");
  sprintf(output[OUT_SINC].cDescr,"Body's sin(1/2*Inclination) in DistOrb");
  output[OUT_SINC].iNum = 1;
  fnWrite[OUT_SINC] = &WriteBodySinc;
  
  sprintf(output[OUT_LONGA].cName,"LongA");
  sprintf(output[OUT_LONGA].cDescr,"Body's Longitude of ascending node in DistOrb");
  sprintf(output[OUT_LONGA].cNeg,"Deg");
  output[OUT_LONGA].bNeg = 1;
  output[OUT_LONGA].dNeg = 1./DEGRAD;
  output[OUT_LONGA].iNum = 1;
  fnWrite[OUT_LONGA] = &WriteBodyLongA;
  
  sprintf(output[OUT_ARGP].cName,"ArgP");
  sprintf(output[OUT_ARGP].cDescr,"Body's argument of pericenter in DistOrb");
  sprintf(output[OUT_ARGP].cNeg,"Deg");
  output[OUT_ARGP].bNeg = 1;
  output[OUT_ARGP].dNeg = 1./DEGRAD;
  output[OUT_ARGP].iNum = 1;
  fnWrite[OUT_ARGP] = &WriteBodyArgP; 

  sprintf(output[OUT_PINC].cName,"Pinc");
  sprintf(output[OUT_PINC].cDescr,"Body's p = s*sin(Omega) in DistOrb");
  output[OUT_PINC].iNum = 1;
  fnWrite[OUT_PINC] = &WriteBodyPinc;
  
  sprintf(output[OUT_QINC].cName,"Qinc");
  sprintf(output[OUT_QINC].cDescr,"Body's q = s* cos(Omega) in DistOrb");
  output[OUT_QINC].iNum = 1;
  fnWrite[OUT_QINC] = &WriteBodyQinc;
  
  sprintf(output[OUT_DHECCDTDISTORB].cName,"DHeccDtDistOrb");
  sprintf(output[OUT_DHECCDTDISTORB].cDescr,"Body's dh/dt in DistOrb");
  sprintf(output[OUT_DHECCDTDISTORB].cNeg,"1/year");
  output[OUT_DHECCDTDISTORB].bNeg = 1;
  output[OUT_DHECCDTDISTORB].dNeg = YEARSEC;
  output[OUT_DHECCDTDISTORB].iNum = 1;
  fnWrite[OUT_DHECCDTDISTORB] = &WriteBodyDHeccDtDistOrb;
  
  sprintf(output[OUT_DKECCDTDISTORB].cName,"DKeccDtDistOrb");
  sprintf(output[OUT_DKECCDTDISTORB].cDescr,"Body's dk/dt in DistOrb");
  sprintf(output[OUT_DKECCDTDISTORB].cNeg,"1/year");
  output[OUT_DKECCDTDISTORB].bNeg = 1;
  output[OUT_DKECCDTDISTORB].dNeg = YEARSEC;
  output[OUT_DKECCDTDISTORB].iNum = 1;
  fnWrite[OUT_DKECCDTDISTORB] = &WriteBodyDKeccDtDistOrb;
  
  sprintf(output[OUT_DPINCDTDISTORB].cName,"DPincDtDistOrb");
  sprintf(output[OUT_DPINCDTDISTORB].cDescr,"Body's dp/dt in DistOrb");
  sprintf(output[OUT_DPINCDTDISTORB].cNeg,"1/year");
  output[OUT_DPINCDTDISTORB].bNeg = 1;
  output[OUT_DPINCDTDISTORB].dNeg = YEARSEC;
  output[OUT_DPINCDTDISTORB].iNum = 1;
  fnWrite[OUT_DPINCDTDISTORB] = &WriteBodyDPincDtDistOrb;
  
  sprintf(output[OUT_DQINCDTDISTORB].cName,"DQincDtDistOrb");
  sprintf(output[OUT_DQINCDTDISTORB].cDescr,"Body's dq/dt in DistOrb");
  sprintf(output[OUT_DQINCDTDISTORB].cNeg,"1/year");
  output[OUT_DQINCDTDISTORB].bNeg = 1;
  output[OUT_DQINCDTDISTORB].dNeg = YEARSEC;
  output[OUT_DQINCDTDISTORB].iNum = 1;
  fnWrite[OUT_DQINCDTDISTORB] = &WriteBodyDQincDtDistOrb;
}

void FinalizeOutputFunctionDistOrb(OUTPUT *output,int iBody,int iModule) {
  
}


/************ DISTORB Logging Functions **************/

void LogOptionsDistOrb(CONTROL *control, FILE *fp) {

  fprintf(fp,"-------- DISTORB Options -----\n\n");
  
}

void LogDistOrb(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  fprintf(fp,"\n----- DISTORB PARAMETERS ------\n");
  for (iOut=OUTSTARTDISTORB;iOut<OUTBODYSTARTDISTORB;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }
}

void LogBodyDistOrb(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- DISTORB PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTDISTORB;iOut<OUTENDDISTORB;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

/************* MODULE Functions ***********/

void AddModuleDistOrb(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = DISTORB;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlDistOrb;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyDistOrb;
  module->fnCountHalts[iBody][iModule] = &CountHaltsDistOrb;
  module->fnLogBody[iBody][iModule] = &LogBodyDistOrb;

  module->fnReadOptions[iBody][iModule] = &ReadOptionsDistOrb;
  module->fnVerify[iBody][iModule] = &VerifyDistOrb;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltDistOrb;
//   module->fnVerifyRotation[iBody][iModule] = &VerifyRotationDistOrb;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyDistOrb;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateDistOrb;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputDistOrb;
  module->fnFinalizeUpdateHecc[iBody][iModule] = &FinalizeUpdateHeccDistOrb;
  module->fnFinalizeUpdateKecc[iBody][iModule] = &FinalizeUpdateKeccDistOrb;
  module->fnFinalizeUpdatePinc[iBody][iModule] = &FinalizeUpdatePincDistOrb;
  module->fnFinalizeUpdateQinc[iBody][iModule] = &FinalizeUpdateQincDistOrb;

  //module->fnInitializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionEqtide;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionDistOrb;

}

/************* DistOrb Functions ************/
void PropsAuxDistOrb(BODY *body,UPDATE *update,int iBody) { 

}

void ForceBehaviorDistOrb(BODY *body,EVOLVE *evolve,IO *io,int iBody,int iModule) {
}

void RecalcLaplace(BODY *body,EVOLVE *evolve,SYSTEM *system) {
  double alpha1, dalpha;
  int j, iBody, jBody;
  
  j = 0;
  for (iBody=1;iBody<evolve->iNumBodies-1;iBody++) {
    for (jBody=iBody+1;jBody<evolve->iNumBodies;jBody++) {
      if (body[iBody].dSemi < body[jBody].dSemi) {
	  alpha1 = body[iBody].dSemi/body[jBody].dSemi;
      } else if (body[iBody].dSemi > body[jBody].dSemi) {
	  alpha1 = body[jBody].dSemi/body[iBody].dSemi;
      }
	
      for (j=0;j<LAPLNUM;j++) {
	dalpha = fabs(alpha1 - system->dmAlpha0[system->imLaplaceN[iBody][jBody]][j]);
	if (dalpha > fabs(system->dDfcrit/system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][j])) {
	    system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][j] = 
	    system->fnLaplaceF[j][0](alpha1, 0);
		    
	    system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][j] = 
	    system->fnLaplaceDeriv[j][0](alpha1, 0);
		    
	    system->dmAlpha0[system->imLaplaceN[iBody][jBody]][j] = alpha1;
	}
      }
    }
  }
}

/*
 * Invariable plane calculations
 */ 

double xangle1(BODY *body, int iBody) {
  return cos(body[iBody].dLongA)*cos(body[iBody].dLongP-body[iBody].dLongA) - sin(body[iBody].dLongA)*sin(body[iBody].dLongP-body[iBody].dLongA)*(1.0-2.*pow(body[iBody].dSinc,2));
}

double xangle2(BODY *body, int iBody) {
  return -cos(body[iBody].dLongA)*sin(body[iBody].dLongP-body[iBody].dLongA) - sin(body[iBody].dLongA)*cos(body[iBody].dLongP-body[iBody].dLongA)*(1.0-2.*pow(body[iBody].dSinc,2));
}

double yangle1(BODY *body, int iBody) {
  return sin(body[iBody].dLongA)*cos(body[iBody].dLongP-body[iBody].dLongA) + cos(body[iBody].dLongA)*sin(body[iBody].dLongP-body[iBody].dLongA)*(1.0-2.*pow(body[iBody].dSinc,2));
}

double yangle2(BODY *body, int iBody) {
  return -sin(body[iBody].dLongA)*sin(body[iBody].dLongP-body[iBody].dLongA) + cos(body[iBody].dLongA)*cos(body[iBody].dLongP-body[iBody].dLongA)*(1.0-2.*pow(body[iBody].dSinc,2));
}

double zangle1(BODY *body, int iBody) {
  return sin(body[iBody].dLongP-body[iBody].dLongA)*(2.*body[iBody].dSinc*sqrt(1.0-pow(body[iBody].dSinc,2)));
}

double zangle2(BODY *body, int iBody) {
  return cos(body[iBody].dLongP-body[iBody].dLongA)*(2.*body[iBody].dSinc*sqrt(1.0-pow(body[iBody].dSinc,2)));
}

double xinit(BODY *body, int iBody) {
  return body[iBody].dSemi/AUCM * (cos(body[iBody].dEccA) - body[iBody].dEcc);
}

double yinit(BODY *body, int iBody) {
  return body[iBody].dSemi/AUCM * sqrt(1.0-pow(body[iBody].dEcc,2)) * sin(body[iBody].dEccA);
}

double vxi(BODY *body, int iBody) {
  double x, y, mu, n;
  x = xinit(body, iBody);
  y = yinit(body, iBody);
  mu = pow(KGAUSS,2)*(body[0].dMass+body[iBody].dMass)/MSUN;
  n = sqrt(mu/pow(body[iBody].dSemi/AUCM,3));
  return -pow(body[iBody].dSemi/AUCM,2)*n*sin(body[iBody].dEccA)/sqrt(pow(x,2)+pow(y,2));
}
  
double vyi(BODY *body, int iBody) {
  double x, y, mu, n, v;
  x = xinit(body, iBody);
  y = yinit(body, iBody);
  mu = pow(KGAUSS,2)*(body[0].dMass+body[iBody].dMass)/MSUN;
  n = sqrt(mu/pow(body[iBody].dSemi/AUCM,3));
  v = pow(body[iBody].dSemi/AUCM,2)*n*sqrt((1.0-pow(body[iBody].dEcc,2))/(pow(x,2)+pow(y,2)))*cos(body[iBody].dEccA);
  return v;
}

double signf(double value) {
  if (value > 0) return 1;
  if (value < 0) return -1;
  return 0;
}

void kepler_eqn(BODY *body, int iBody) {
  double di1, di2, di3, fi, fi1, fi2, fi3;
  if (body[iBody].dMeanA == 0) {
    body[iBody].dEccA = 0;
  } else {
    body[iBody].dEccA = body[iBody].dMeanA + signf(sin(body[iBody].dMeanA))*0.85*body[iBody].dEcc;
    di3 = 1.0;
    
    while (di3 > 1e-15) {
      fi = body[iBody].dEccA - body[iBody].dEcc*sin(body[iBody].dEccA) - body[iBody].dMeanA;
      fi1 = 1.0 - body[iBody].dEcc*cos(body[iBody].dEccA);
      fi2 = body[iBody].dEcc*sin(body[iBody].dEccA);
      fi3 = body[iBody].dEcc*cos(body[iBody].dEccA);
      di1 = -fi/fi1;
      di2 = -fi/(fi1+0.5*di1*fi2);
      di3 = -fi/(fi1+0.5*di2*fi2+1./6.*pow(di2,2)*fi3);
      body[iBody].dEccA += di3;
    }
  }
}

void osc2cart(BODY *body, int iNumBodies) {
  int iBody;
  double xtmp, ytmp, vxtmp, vytmp;
  
  for (iBody=0;iBody<iNumBodies;iBody++) {
    body[iBody].dCartPos = malloc(3*sizeof(double));
    body[iBody].dCartVel = malloc(3*sizeof(double));
    
    if (iBody == 0) {
      body[iBody].dCartPos[0] = 0;
      body[iBody].dCartPos[1] = 0;
      body[iBody].dCartPos[2] = 0;
  
      body[iBody].dCartVel[0] = 0;
      body[iBody].dCartVel[1] = 0;
      body[iBody].dCartVel[2] = 0;
    } else {
      kepler_eqn(body, iBody);
      xtmp = xinit(body, iBody);
      ytmp = yinit(body, iBody);
      vxtmp = vxi(body, iBody);
      vytmp = vyi(body, iBody);
      
      body[iBody].dCartPos[0] = xtmp*(xangle1(body,iBody))+ytmp*(xangle2(body,iBody));
      body[iBody].dCartPos[1] = xtmp*(yangle1(body,iBody))+ytmp*(yangle2(body,iBody));
      body[iBody].dCartPos[2] = xtmp*(zangle1(body,iBody))+ytmp*(zangle2(body,iBody));
  
      body[iBody].dCartVel[0] = vxtmp*(xangle1(body,iBody))+vytmp*(xangle2(body,iBody));
      body[iBody].dCartVel[1] = vxtmp*(yangle1(body,iBody))+vytmp*(yangle2(body,iBody));
      body[iBody].dCartVel[2] = vxtmp*(zangle1(body,iBody))+vytmp*(zangle2(body,iBody));
    }
  }
}

void astro2bary(BODY *body, int iNumBodies) {
  int i, iBody;
  double *xcom, *vcom, mtotal;
  xcom = malloc(3*sizeof(double));
  vcom = malloc(3*sizeof(double));
  mtotal = 0;
  for (iBody=0;iBody<iNumBodies;iBody++) mtotal += body[iBody].dMass;
  
  for (i=0;i<3;i++) {
    xcom[i] = 0;
    vcom[i] = 0;
    for (iBody=1;iBody<iNumBodies;iBody++) {
      xcom[i] += (body[iBody].dMass*body[iBody].dCartPos[i]/mtotal);
      vcom[i] += (body[iBody].dMass*body[iBody].dCartVel[i]/mtotal);
    }
  }
  
  for (i=0;i<3;i++) {
    for (iBody=0;iBody<iNumBodies;iBody++) {
      body[iBody].dCartPos[i] -= xcom[i];
      body[iBody].dCartVel[i] -= vcom[i];
    }
  }
}

void bary2astro(BODY *body, int iNumBodies) {
  int i, iBody;
  double xtmp, vtmp;
  
  for (i=0;i<3;i++) {
    xtmp = body[0].dCartPos[i];
    vtmp = body[0].dCartVel[i];
    for (iBody=0;iBody<iNumBodies;iBody++) {
      body[iBody].dCartPos[i] -= xtmp;
      body[iBody].dCartVel[i] -= vtmp;
    }
  }
}

void cross(double *a, double *b, double *c) {
  c[0] = a[1]*b[2] - b[1]*a[2];
  c[1] = a[2]*b[0] - b[2]*a[0];
  c[2] = a[0]*b[1] - b[0]*a[1];
}

void angularmom(BODY *body, double *AngMom, int iNumBodies) {
  double *rxptmp;
  int i, iBody;
  
  osc2cart(body, iNumBodies);
  astro2bary(body, iNumBodies);
  
  rxptmp = malloc(3*sizeof(double));
  for (iBody=0;iBody<iNumBodies;iBody++) {
    cross(body[iBody].dCartPos, body[iBody].dCartVel, rxptmp);
    for (i=0;i<3;i++) {
      AngMom[i] += body[iBody].dMass/MSUN*rxptmp[i];
    }
  }
}

void rotate_inv(BODY *body, SYSTEM *system, int iNumBodies) {
  double *xtmp, *vtmp;
  int iBody;  
  xtmp = malloc(3*sizeof(double));
  vtmp = malloc(3*sizeof(double));
  
  for (iBody=0;iBody<iNumBodies;iBody++) {
    xtmp[0] = body[iBody].dCartPos[0]*cos(system->dThetaInvP)+body[iBody].dCartPos[1]*sin(system->dThetaInvP);
    xtmp[1] = -body[iBody].dCartPos[0]*sin(system->dThetaInvP)+body[iBody].dCartPos[1]*cos(system->dThetaInvP);
    xtmp[2] = body[iBody].dCartPos[2];
    vtmp[0] = body[iBody].dCartVel[0]*cos(system->dThetaInvP)+body[iBody].dCartVel[1]*sin(system->dThetaInvP);
    vtmp[1] = -body[iBody].dCartVel[0]*sin(system->dThetaInvP)+body[iBody].dCartVel[1]*cos(system->dThetaInvP);
    vtmp[2] = body[iBody].dCartVel[2];
    
    body[iBody].dCartPos[0] = xtmp[0]*cos(system->dPhiInvP)-xtmp[2]*sin(system->dPhiInvP);
    body[iBody].dCartPos[1] = xtmp[1];
    body[iBody].dCartPos[2] = xtmp[0]*sin(system->dPhiInvP)+xtmp[2]*cos(system->dPhiInvP);
    body[iBody].dCartVel[0] = vtmp[0]*cos(system->dPhiInvP)-vtmp[2]*sin(system->dPhiInvP);
    body[iBody].dCartVel[1] = vtmp[1];
    body[iBody].dCartVel[2] = vtmp[0]*sin(system->dPhiInvP)+vtmp[2]*cos(system->dPhiInvP);
  }
}

double normv(double *vector) {
  return sqrt(pow(vector[0],2)+pow(vector[1],2)+pow(vector[2],2));
}

void cart2osc(BODY *body, int iNumBodies) {
  int iBody;
  double r, vsq, rdot, mu, *h, hsq, sinwf, coswf, sinf, cosf, sinw, cosw, cosE, f;
  
  for (iBody=1;iBody<iNumBodies;iBody++) {
    r = normv(body[iBody].dCartPos);
    vsq = pow(normv(body[iBody].dCartVel),2);
    rdot = (body[iBody].dCartPos[0]*body[iBody].dCartVel[0]+body[iBody].dCartPos[1]*body[iBody].dCartVel[1]+\
	    body[iBody].dCartPos[2]*body[iBody].dCartVel[2])/r;
    mu = pow(KGAUSS,2)*(body[0].dMass+body[iBody].dMass)/MSUN;
    h = malloc(3*sizeof(double));
    cross(body[iBody].dCartPos, body[iBody].dCartVel, h);
    hsq = pow(normv(h),2);
    
    body[iBody].dSemi = pow((2.0/r - vsq/mu),-1)*AUCM;
    if (body[iBody].dEcc != 0) {
      body[iBody].dEcc = sqrt(1.0 - hsq/(mu*body[iBody].dSemi/AUCM));
    }
    
    body[iBody].dSinc = sin(0.5 * acos(h[2]/normv(h)));
    body[iBody].dLongA = atan2(h[0],-h[1]);
    if (body[iBody].dLongA < 0) body[iBody].dLongA += 2.0*PI;
    
    sinwf = body[iBody].dCartPos[2] / (r*2.*body[iBody].dSinc*sqrt(1.0-pow(body[iBody].dSinc,2)));
    coswf = (body[iBody].dCartPos[0]/r + sin(body[iBody].dLongA)*sinwf*(1.0-2.*pow(body[iBody].dSinc,2)))/cos(body[iBody].dLongA);
    
    sinf = body[iBody].dSemi/AUCM*(1.0-pow(body[iBody].dEcc,2))*rdot/(normv(h)*body[iBody].dEcc);
    cosf = (body[iBody].dSemi/AUCM*(1.0-pow(body[iBody].dEcc,2))/r - 1.0)/body[iBody].dEcc;
    
    if (body[iBody].dEcc != 0) {
      sinw = sinwf*cosf - coswf*sinf;
      cosw = sinwf*sinf + coswf*cosf;
      body[iBody].dLongP = atan2(sinw, cosw) + body[iBody].dLongA;
      if (body[iBody].dLongP >= 2.*PI) {
        body[iBody].dLongP -= 2.*PI;
      } else if (body[iBody].dLongP < 0.0) {
        body[iBody].dLongP += 2.*PI;
      }
    }
    
    f = atan2(sinf, cosf);
    if (f >= 2.*PI) {
      f -= 2.*PI;
    } else if (f < 0.0) {
      f += 2.*PI;
    }
    cosE = (cos(f)+body[iBody].dEcc) / (1.0+body[iBody].dEcc*cos(f));
    if (f <= PI) body[iBody].dEccA = acos(cosE);
    if (f > PI) body[iBody].dEccA = 2.*PI - acos(cosE);
    
    body[iBody].dMeanA = body[iBody].dEccA - body[iBody].dEcc*sin(body[iBody].dEccA);
    if (body[iBody].dMeanA < 0) body[iBody].dMeanA += 2*PI;
    if (body[iBody].dMeanA >= 2*PI) body[iBody].dMeanA -= 2*PI;
  }
}


void inv_plane(BODY *body, SYSTEM *system, int iNumBodies) {
  int iBody;
  double *AngMom;
  AngMom = malloc(3*sizeof(double));
  
  /* Loop below calculates true anomaly at equinox for planets with DistRot enabled. 
     This angle is invariant under rotations. */
  for (iBody=1;iBody<iNumBodies;iBody++) {
    if (body[iBody].bDistRot) {
      body[iBody].dTrueApA = 2*PI - (body[iBody].dPrecA+body[iBody].dLongP);
      while (body[iBody].dTrueApA<0) {
	body[iBody].dTrueApA += 2*PI;
      }
    }
  }
  
  angularmom(body, AngMom, iNumBodies);
  system->dThetaInvP = atan2(AngMom[1],AngMom[0]);
  system->dPhiInvP = atan2(sqrt(pow(AngMom[0],2)+pow(AngMom[1],2)),AngMom[2]);
  
  rotate_inv(body, system, iNumBodies);
  bary2astro(body, iNumBodies);
  cart2osc(body, iNumBodies);

  /* Loop below recalculates precession param for planets with DistRot enabled.*/
  for (iBody=1;iBody<iNumBodies;iBody++) {
    if (body[iBody].bDistRot) {
      body[iBody].dPrecA = 2*PI - (body[iBody].dTrueApA+body[iBody].dLongP);
      while (body[iBody].dPrecA<0) {
	body[iBody].dPrecA += 2*PI;
      }
      CalcXYZobl(body, iBody);
    }
    CalcHK(body, iBody);
    CalcPQ(body, iBody);
  }
}

// void rotate_rev(BODY *body, SYSTEM *system, int iNumBodies) {
//   double *xtmp, *vtmp;
//   int iBody;
//   
//   xtmp = malloc(3*sizeof(double));
//   vtmp = malloc(3*sizeof(double));
//   
//   for (iBody=0;iBody<iNumBodies;iBody++) {
//     xtmp[0] = body[iBody].dCartPos[0]*cos(-system->dPhiInvP)-body[iBody].dCartPos[2]*sin(-system->dPhiInvP);
//     xtmp[1] = body[iBody].dCartPos[1];
//     xtmp[2] = body[iBody].dCartPos[0]*sin(-system->dPhiInvP)+body[iBody].dCartPos[2]*cos(-system->dPhiInvP);
//     vtmp[0] = body[iBody].dCartVel[0]*cos(-system->dPhiInvP)-body[iBody].dCartVel[2]*sin(-system->dPhiInvP);
//     vtmp[1] = body[iBody].dCartVel[1];
//     vtmp[2] = body[iBody].dCartVel[0]*sin(-system->dPhiInvP)+body[iBody].dCartVel[2]*cos(-system->dPhiInvP);
//     
//     body[iBody].dCartPos[0] = xtmp[0]*cos(-system->dThetaInvP)+xtmp[1]*sin(-system->dThetaInvP);
//     body[iBody].dCartPos[1] = -xtmp[0]*sin(-system->dThetaInvP)+xtmp[1]*cos(-system->dThetaInvP);
//     body[iBody].dCartPos[2] = xtmp[2];
//     body[iBody].dCartVel[0] = vtmp[0]*cos(-system->dThetaInvP)+vtmp[1]*sin(-system->dThetaInvP);
//     body[iBody].dCartVel[1] = -vtmp[0]*sin(-system->dThetaInvP)+vtmp[1]*cos(-system->dThetaInvP);
//     body[iBody].dCartVel[2] = vtmp[2];  
//   }
// }
// 
// void inv2input_plane(BODY *body, SYSTEM *system, int iNumBodies) {
//   osc2cart(body, iNumBodies);
//   astro2bary(body, iNumBodies);
//   rotate_rev(body, system, iNumBodies);
//   bary2astro(body, iNumBodies);
//   cart2osc(body, iNumBodies);
// }



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

/*--------- f1 ----------------------*/
double fdSemiMajAxF1(double dAxRatio, int iIndexJ) {
  return 1./2 * fdLaplaceCoeff(A(iIndexJ));
}

double fdDSemiF1Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2 * fdDerivLaplaceCoeff(1,A(iIndexJ));
}

/*--------- f2 ----------------------*/
double fdSemiMajAxF2(double dAxRatio, int iIndexJ) {
  return 1./8* (-4.*iIndexJ*iIndexJ * fdLaplaceCoeff(A(iIndexJ)) + 2.*dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ))\
    + dAxRatio*dAxRatio * fdDerivLaplaceCoeff(2,A(iIndexJ)) );
}

double fdDSemiF2Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (2.-4.*pow(iIndexJ,2))*fdDerivLaplaceCoeff(1,A(iIndexJ)) + 4.*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ)) + pow(dAxRatio,2)*fdDerivLaplaceCoeff(3,A(iIndexJ)) );
}

/*--------- f3 ----------------------*/
double fdSemiMajAxF3(double dAxRatio, int iIndexJ) {
  return -1./4*dAxRatio * ( fdLaplaceCoeff(B(abs(iIndexJ-1))) + fdLaplaceCoeff(B(iIndexJ+1)) );
}

double fdDSemiF3Dalpha(double dAxRatio, int iIndexJ) {
  return -1./4*( (fdLaplaceCoeff(B(abs(iIndexJ-1))) + fdLaplaceCoeff(B(iIndexJ+1))) + dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) + fdDerivLaplaceCoeff(1,B(iIndexJ+1))) );
}

/*--------- f4 ----------------------*/
double fdSemiMajAxF4(double dAxRatio, int iIndexJ) {
  return 1./128 * ( (-9.*iIndexJ*iIndexJ + 16.*pow(iIndexJ,4)) * fdLaplaceCoeff(A(iIndexJ))  \
    -8.*iIndexJ*iIndexJ*dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ)) \
    - 8.*iIndexJ*iIndexJ*pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,A(iIndexJ)) \
    + 4.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ)) );
}

double fdDSemiF4Dalpha(double dAxRatio, int iIndexJ) {
  return 1./128 * ( (-17.*pow(iIndexJ,2)+16.*pow(iIndexJ,4))*fdDerivLaplaceCoeff(1,A(iIndexJ)) \
    - 24.*pow(iIndexJ,2)*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ)) \
    + (12.-8.*pow(iIndexJ,2))*pow(dAxRatio,2)*fdDerivLaplaceCoeff(3,A(iIndexJ)) \
    + (8.*pow(dAxRatio,3))*fdDerivLaplaceCoeff(4,A(iIndexJ)) \
    + pow(dAxRatio,4)*fdDerivLaplaceCoeff(5,A(iIndexJ)) );
}

/*--------- f5 ----------------------*/
double fdSemiMajAxF5(double dAxRatio, int iIndexJ) {
  return 1./32 * ( 16.*pow(iIndexJ,4) * fdLaplaceCoeff(A(iIndexJ)) \
    + (4. - 16.*iIndexJ*iIndexJ) * dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ)) \
    + (14. - 8.*iIndexJ*iIndexJ) * pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,A(iIndexJ)) \
    + 8.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ)) );
}

double fdDSemiF5Dalpha(double dAxRatio, int iIndexJ) {
  return 1./32 * ( (4.-16.*pow(iIndexJ,2)+16.*pow(iIndexJ,4))*fdDerivLaplaceCoeff(1,A(iIndexJ)) \
    + (32.-32.*pow(iIndexJ,2))*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ)) \
    + (38.-8.*pow(iIndexJ,2))*pow(dAxRatio,2)*fdDerivLaplaceCoeff(3,A(iIndexJ)) \
    + 12.*pow(dAxRatio,3)*fdDerivLaplaceCoeff(4,A(iIndexJ)) \
    + pow(dAxRatio,4)*fdDerivLaplaceCoeff(5,A(iIndexJ)) );
}

/*--------- f6 ----------------------*/
double fdSemiMajAxF6(double dAxRatio, int iIndexJ) {
  return 1./128 * ( (-17.*iIndexJ*iIndexJ + 16.*pow(iIndexJ,4)) * fdLaplaceCoeff(A(iIndexJ)) \
    + (1. - iIndexJ*iIndexJ) * 24. * dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ)) \
    + (36. - 8.*iIndexJ*iIndexJ) * pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,A(iIndexJ)) \
    + 12.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ)) );
}

double fdDSemiF6Dalpha(double dAxRatio, int iIndexJ) {
  return 1./128 * ( (24.-41.*pow(iIndexJ,2)+16.*pow(iIndexJ,4))*fdDerivLaplaceCoeff(1,A(iIndexJ)) \
    + (96.- 40.*pow(iIndexJ,2))*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ)) \
    + (72.-8.*pow(iIndexJ,2))*pow(dAxRatio,2)*fdDerivLaplaceCoeff(3,A(iIndexJ)) \
    + (16.*pow(dAxRatio,3))*fdDerivLaplaceCoeff(4,A(iIndexJ)) \
    + pow(dAxRatio,4)*fdDerivLaplaceCoeff(5,A(iIndexJ)) );
}

/*--------- f7 ----------------------*/
double fdSemiMajAxF7(double dAxRatio, int iIndexJ) {
  return 1./16*( (-2.+4.*iIndexJ*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(abs(iIndexJ-1)))+fdLaplaceCoeff(B(iIndexJ+1)))
    - 4.*pow(dAxRatio,2) * (fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) + fdDerivLaplaceCoeff(1,B(iIndexJ+1))) \
    - pow(dAxRatio,3) * (fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) + fdDerivLaplaceCoeff(2,B(iIndexJ+1))) );
}

double fdDSemiF7Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (-2.+4.*pow(iIndexJ,2))*(fdLaplaceCoeff(B(abs(iIndexJ-1))) + fdLaplaceCoeff(B(iIndexJ+1))) \
  -(10.-4.*pow(iIndexJ,2))*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))+fdDerivLaplaceCoeff(1,B(iIndexJ+1)))\
  - 7.*pow(dAxRatio,2)*(fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1)))+fdDerivLaplaceCoeff(2,B(iIndexJ+1)))\
  - pow(dAxRatio,3)*(fdDerivLaplaceCoeff(3,B(abs(iIndexJ-1)))+fdDerivLaplaceCoeff(3,B(iIndexJ+1))) );
}
  
/*--------- f8 ----------------------*/
double fdSemiMajAxF8(double dAxRatio, int iIndexJ) {
  return 3./16 * dAxRatio*dAxRatio * ( fdLaplaceCoeff(C(abs(iIndexJ-2))) \
    + 4. * fdLaplaceCoeff(C(iIndexJ)) + fdLaplaceCoeff(C(iIndexJ+2)) );
}

double fdDSemiF8Dalpha(double dAxRatio, int iIndexJ) {
  return 3./16 * dAxRatio* ( 2*(fdLaplaceCoeff(C(abs(iIndexJ-2))) \
    + 4. * fdLaplaceCoeff(C(iIndexJ)) + fdLaplaceCoeff(C(iIndexJ+2))) \
    + dAxRatio*(fdDerivLaplaceCoeff(1,C(abs(iIndexJ-2))) \
    + 4. * fdDerivLaplaceCoeff(1,C(iIndexJ)) + fdDerivLaplaceCoeff(1,C(iIndexJ+2))) );
}

/*--------- f9 ----------------------*/
double fdSemiMajAxF9(double dAxRatio, int iIndexJ) {
  return 1./4 * dAxRatio * (fdLaplaceCoeff(B(abs(iIndexJ-1))) + fdLaplaceCoeff(B(iIndexJ+1))) \
    + 3./8 * pow(dAxRatio,2) * ( fdLaplaceCoeff(C(abs(iIndexJ-2))) + 10. * fdLaplaceCoeff(C(iIndexJ)) \
    + fdLaplaceCoeff(C(iIndexJ+2)) );
}

double fdDSemiF9Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (fdLaplaceCoeff(B(abs(iIndexJ-1))) + fdLaplaceCoeff(B(iIndexJ+1))) \
    + dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) + fdDerivLaplaceCoeff(1,B(iIndexJ+1))) ) \
    + 3./8 * dAxRatio * ( 2*(fdLaplaceCoeff(C(abs(iIndexJ-2))) + 10. * fdLaplaceCoeff(C(iIndexJ)) \
    + fdLaplaceCoeff(C(iIndexJ+2))) + dAxRatio*(fdDerivLaplaceCoeff(1,C(abs(iIndexJ-2)))\
    + 10. * fdDerivLaplaceCoeff(1,C(iIndexJ)) + fdDerivLaplaceCoeff(1,C(iIndexJ+2))) );
}

/*--------- f10 ----------------------*/
double fdSemiMajAxF10(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (2. + 6.*iIndexJ + 4.*iIndexJ*iIndexJ) * fdLaplaceCoeff(A(iIndexJ+1)) \
    - 2. * dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ+1)) \
    - pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,A(iIndexJ+1)) );
}

double fdDSemiF10Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (6.*iIndexJ + 4.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(1,A(iIndexJ+1)) \
    - 4. * dAxRatio * fdDerivLaplaceCoeff(2,A(iIndexJ+1)) \
    - pow(dAxRatio,2) * fdDerivLaplaceCoeff(3,A(iIndexJ+1)) );
}
  
  
/*--------- f11 ----------------------*/
double fdSemiMajAxF11(double dAxRatio, int iIndexJ) {
  return 1./32*((-6.*iIndexJ-26.*iIndexJ*iIndexJ-36.*pow(iIndexJ,3) \
    -16*pow(iIndexJ,4))*fdLaplaceCoeff(A(iIndexJ+1))\
    + dAxRatio * (6*iIndexJ + 12*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(1,A(iIndexJ+1)) \
    + pow(dAxRatio,2) * (-4. + 7*iIndexJ + 8*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(2,A(iIndexJ+1)) \
    - 6.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ+1)) \
    - pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ+1)) );
}

double fdDSemiF11Dalpha(double dAxRatio, int iIndexJ) {
  return 1./32*((-14.*iIndexJ*iIndexJ-36.*pow(iIndexJ,3)-16*pow(iIndexJ,4))*fdDerivLaplaceCoeff(1,A(iIndexJ+1))\
    + dAxRatio * (-8.+20.*iIndexJ+28.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(2,A(iIndexJ+1)) \
    + pow(dAxRatio,2) * (-22.+7.*iIndexJ+8.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(3,A(iIndexJ+1)) \
    - 10.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(4,A(iIndexJ+1)) \
    - pow(dAxRatio,4) * fdDerivLaplaceCoeff(5,A(iIndexJ+1)) );
}
  
/*--------- f12 ----------------------*/
double fdSemiMajAxF12(double dAxRatio, int iIndexJ) {
  return 1./32 * ( (4. + 2.*iIndexJ - 22.*iIndexJ*iIndexJ - 36.*pow(iIndexJ,3) - 16.*pow(iIndexJ,4)) * \
    fdLaplaceCoeff(A(iIndexJ+1)) \
    + dAxRatio * (-4. + 22.*iIndexJ + 20.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(1,A(iIndexJ+1)) \
    + pow(dAxRatio,2) * (-22. + 7.*iIndexJ + 8.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(2,A(iIndexJ+1)) \
    - 10.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ+1)) \
    - pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ+1)) );
}

double fdDSemiF12Dalpha(double dAxRatio, int iIndexJ) {
  return 1./32 * ( (24.*iIndexJ-2.*iIndexJ*iIndexJ-36.*pow(iIndexJ,3)-16.*pow(iIndexJ,4)) * \
    fdDerivLaplaceCoeff(1,A(iIndexJ+1)) \
    + dAxRatio * (-48. + 36.*iIndexJ + 36.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(2,A(iIndexJ+1)) \
    + pow(dAxRatio,2) * (-52. + 7.*iIndexJ + 8.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(3,A(iIndexJ+1)) \
    - 14.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(4,A(iIndexJ+1)) \
    - pow(dAxRatio,4) * fdDerivLaplaceCoeff(5,A(iIndexJ+1)) );
}

/*--------- f13 ----------------------*/
double fdSemiMajAxF13(double dAxRatio, int iIndexJ) {
  return 1./8*((-6.*iIndexJ-4.*iIndexJ*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(iIndexJ))+fdLaplaceCoeff(B(iIndexJ+2)))\
    + 4.*pow(dAxRatio,2) * (fdDerivLaplaceCoeff(1,B(iIndexJ)) + fdDerivLaplaceCoeff(1,B(iIndexJ+2)))\
    + pow(dAxRatio,3) * (fdDerivLaplaceCoeff(2,B(iIndexJ)) + fdDerivLaplaceCoeff(2,B(iIndexJ+2))) );
}

double fdDSemiF13Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8*( (-6.*iIndexJ-4.*iIndexJ*iIndexJ)*(fdLaplaceCoeff(B(iIndexJ))+fdLaplaceCoeff(B(iIndexJ+2)))\
    + (8.-6.*iIndexJ-4.*pow(iIndexJ,2))*dAxRatio \
    * (fdDerivLaplaceCoeff(1,B(iIndexJ))+fdDerivLaplaceCoeff(1,B(iIndexJ+2)))\
    + 7.*pow(dAxRatio,2) * (fdDerivLaplaceCoeff(2,B(iIndexJ)) + fdDerivLaplaceCoeff(2,B(iIndexJ+2))) \
    + pow(dAxRatio,3) * (fdDerivLaplaceCoeff(3,B(iIndexJ)) + fdDerivLaplaceCoeff(3,B(iIndexJ+2))) );
}

/*--------- f14 ----------------------*/
double fdSemiMajAxF14(double dAxRatio, int iIndexJ) {
  return dAxRatio * fdLaplaceCoeff(B(iIndexJ+1));
}

double fdDSemiF14Dalpha(double dAxRatio, int iIndexJ) {
  return fdLaplaceCoeff(B(iIndexJ+1)) + dAxRatio*fdDerivLaplaceCoeff(1,B(iIndexJ+1));
}
  
/*--------- f15 ----------------------*/
double fdSemiMajAxF15(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (2. - 4.*iIndexJ*iIndexJ) * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) \
   + 4.*pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ+1)) \
   + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ+1)) );
}

double fdDSemiF15Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (2.-4.*iIndexJ*iIndexJ) * fdLaplaceCoeff(B(iIndexJ+1)) \
   + (10.-4.*pow(iIndexJ,2))*dAxRatio * fdDerivLaplaceCoeff(1,B(iIndexJ+1)) \
   + 7.*pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,B(iIndexJ+1)) \
   + pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,B(iIndexJ+1)) );
}
  
/*--------- f16 ----------------------*/
double fdSemiMajAxF16(double dAxRatio, int iIndexJ) {
  return -1./2 * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) \
    -3.* pow(dAxRatio,2) * fdLaplaceCoeff(C(iIndexJ)) - 3./2 * pow(dAxRatio,2) * fdLaplaceCoeff(C(iIndexJ+2));
}

double fdDSemiF16Dalpha(double dAxRatio, int iIndexJ) {
  return -1./2 * ( fdLaplaceCoeff(B(iIndexJ+1)) + dAxRatio*fdDerivLaplaceCoeff(1,B(iIndexJ+1)) ) \
    -3.* dAxRatio * ( 2.*(fdLaplaceCoeff(C(iIndexJ)) + 1./2 * fdLaplaceCoeff(C(iIndexJ+2))) \
    + dAxRatio*(fdDerivLaplaceCoeff(1,C(iIndexJ)) + 1./2 * fdDerivLaplaceCoeff(1,C(iIndexJ+2))) );
}
  
/*--------- f17 ----------------------*/
double fdSemiMajAxF17(double dAxRatio, int iIndexJ) {
  return 1./64 * ( (12. + 64.*iIndexJ + 109.*iIndexJ*iIndexJ + 72.*pow(iIndexJ,3) + 16.*pow(iIndexJ,4)) \
    * fdLaplaceCoeff(A(iIndexJ+2)) \
    - dAxRatio * (12. + 28.*iIndexJ + 16.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(1,A(iIndexJ+2)) \
    + pow(dAxRatio,2) * (6. - 14.*iIndexJ - 8.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(2,A(iIndexJ+2)) \
    + 8.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ+2)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ+2)) );
}

double fdDSemiF17Dalpha(double dAxRatio, int iIndexJ) {
  return 1./64 * ( (36.*iIndexJ + 93.*iIndexJ*iIndexJ + 72.*pow(iIndexJ,3) + 16.*pow(iIndexJ,4)) \
    * fdDerivLaplaceCoeff(1,A(iIndexJ+2)) \
    - dAxRatio * (56.*iIndexJ + 32.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(2,A(iIndexJ+2)) \
    + pow(dAxRatio,2) * (30. - 14.*iIndexJ - 8.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(3,A(iIndexJ+2)) \
    + 12.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(4,A(iIndexJ+2)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(5,A(iIndexJ+2)) );
}
  
/*--------- f18 ----------------------*/
double fdSemiMajAxF18(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (12. - 15.*iIndexJ + 4.*iIndexJ*iIndexJ) * dAxRatio * fdLaplaceCoeff(B(abs(iIndexJ-1))) \
    + pow(dAxRatio,2) * (8. - 4.*iIndexJ) * fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) \
    + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) );
}

double fdDSemiF18Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (12. - 15.*iIndexJ + 4.*iIndexJ*iIndexJ) * fdLaplaceCoeff(B(abs(iIndexJ-1))) \
    + dAxRatio * (28. - 23.*iIndexJ + 4.*pow(iIndexJ,2)) * fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) \
    + (11.-4.*iIndexJ)*pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) \
    + pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,B(abs(iIndexJ-1))));
}

/*--------- f19 ----------------------*/
double fdSemiMajAxF19(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (6. - 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ)) \
   + pow(dAxRatio,2) * (-4. + 4.*iIndexJ) * fdDerivLaplaceCoeff(1,B(iIndexJ)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ)) );
}

double fdDSemiF19Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (6. - 4.*iIndexJ) * iIndexJ * fdLaplaceCoeff(B(iIndexJ)) \
   + dAxRatio * (-8. + 14.*iIndexJ - 4.*pow(iIndexJ,2)) * fdDerivLaplaceCoeff(1,B(iIndexJ)) \
   + (-7.+4.*iIndexJ)*pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,B(iIndexJ)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,B(iIndexJ)) );
}

/*--------- f20 ----------------------*/
double fdSemiMajAxF20(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (3. + 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) \
   - 4.*iIndexJ * pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ+1)) \
   + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ+1)) );
}

double fdDSemiF20Dalpha(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (3. + 4.*iIndexJ) * iIndexJ * fdLaplaceCoeff(B(iIndexJ+1)) \
   + (-5.*iIndexJ + 4.*pow(iIndexJ,2)) * dAxRatio * fdDerivLaplaceCoeff(1,B(iIndexJ+1)) \
   + (3.-4.*iIndexJ)*pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,B(iIndexJ+1)) \
   + pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,B(iIndexJ+1)) );
}

/*--------- f21 ----------------------*/
double fdSemiMajAxF21(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (-12. + 15.*iIndexJ - 4.*iIndexJ*iIndexJ) * dAxRatio * fdLaplaceCoeff(B(abs(iIndexJ-1))) \
   + pow(dAxRatio,2) * (-8. + 4.*iIndexJ) * fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) );
}

double fdDSemiF21Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (-12. + 15.*iIndexJ - 4.*iIndexJ*iIndexJ) * fdLaplaceCoeff(B(abs(iIndexJ-1))) \
   + dAxRatio * (-28. + 23.*iIndexJ -4.*pow(iIndexJ,2)) * fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) \
   + (-11.+4.*iIndexJ)* pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,B(abs(iIndexJ-1))) );
}

/*--------- f22 ----------------------*/
double fdSemiMajAxF22(double dAxRatio, int iIndexJ) {
  return 1./4 * ( dAxRatio * iIndexJ * (6. + 4.*iIndexJ) * fdLaplaceCoeff(B(iIndexJ)) \
   - 4. * pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ)) );
}

double fdDSemiF22Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4 * ( iIndexJ * (6. + 4.*iIndexJ) * fdLaplaceCoeff(B(iIndexJ)) \
   + (-8.+6.*iIndexJ+4.*pow(iIndexJ,2)) * dAxRatio * fdDerivLaplaceCoeff(1,B(iIndexJ)) \
   - 7.*pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,B(iIndexJ)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,B(iIndexJ)) );
}
  
/*--------- f23 ----------------------*/
double fdSemiMajAxF23(double dAxRatio, int iIndexJ) {
  return 1./4 * ( dAxRatio * iIndexJ * (6. + 4.*iIndexJ) * fdLaplaceCoeff(B(iIndexJ+2)) \
   - 4. * pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ+2)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ+2)) );
}

double fdDSemiF23Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4 * ( iIndexJ * (6. + 4.*iIndexJ) * fdLaplaceCoeff(B(iIndexJ+2)) \
   + (-8.+6.*iIndexJ+4.*pow(iIndexJ,2)) * dAxRatio * fdDerivLaplaceCoeff(1,B(iIndexJ+2)) \
   - 7.*pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,B(iIndexJ+2)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,B(iIndexJ+2)) );
}

/*--------- f24 ----------------------*/
double fdSemiMajAxF24(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (-6. + 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ))	\
   + 4.*pow(dAxRatio,2) * (1. - iIndexJ) * fdDerivLaplaceCoeff(1,B(iIndexJ))    \
   + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ)) );
}

double fdDSemiF24Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (-6. + 4.*iIndexJ) * iIndexJ * fdLaplaceCoeff(B(iIndexJ))	\
   + (8.-14.*iIndexJ+4.*pow(iIndexJ,2))*dAxRatio* fdDerivLaplaceCoeff(1,B(iIndexJ))    \
   + (7.-4.*iIndexJ)*pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,B(iIndexJ)) \
   +  pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,B(iIndexJ)) );
}
  
/*--------- f25 ----------------------*/
double fdSemiMajAxF25(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (-3. - 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) \
   + 4.*iIndexJ*pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ+1)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ+1)) );
}
 
double fdDSemiF25Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (-3. - 4.*iIndexJ) * iIndexJ * fdLaplaceCoeff(B(iIndexJ+1)) \
   + (5.*iIndexJ-4.*pow(iIndexJ,2))*dAxRatio * fdDerivLaplaceCoeff(1,B(iIndexJ+1)) \
   + (-3.+4.*iIndexJ)* pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,B(iIndexJ+1)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,B(iIndexJ+1)) );
}
 
/*--------- f26 ----------------------*/
double fdSemiMajAxF26(double dAxRatio, int iIndexJ) {
  return 1./2 * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) + 3./4 * pow(dAxRatio,2) * fdLaplaceCoeff(C(iIndexJ)) \
    + 3./2 * pow(dAxRatio,2) * fdLaplaceCoeff(C(iIndexJ+2));
}

double fdDSemiF26Dalpha(double dAxRatio, int iIndexJ) {
  return 1./2 * ( fdLaplaceCoeff(B(iIndexJ+1)) + dAxRatio*fdDerivLaplaceCoeff(1,B(iIndexJ+1)) ) \
    + 3./4 * dAxRatio * ( 2.*(fdLaplaceCoeff(C(iIndexJ)) + 2.*fdLaplaceCoeff(C(iIndexJ+2))) \
    + dAxRatio*(fdDerivLaplaceCoeff(1,C(iIndexJ)) + 2.*fdDerivLaplaceCoeff(1,C(iIndexJ+2))) ) ;
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

//--------Relativistic correction---------------------------------------------------------
double fdApsidalGRCorrection(BODY *body, int *iaBody) {
  double n;
  n = KGAUSS*sqrt((body[0].dMass+body[iaBody[0]].dMass)/MSUN/(pow(body[iaBody[0]].dSemi/AUCM,3)));
  return 3*pow(n,3)*pow(body[iaBody[0]].dSemi/AUCM,2)/(pow(cLIGHT/AUCM*DAYSEC,2)*(1.0-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2)))/DAYSEC;
}

double fdApsidalGRDhDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dKecc*fdApsidalGRCorrection(body,iaBody);
}

double fdApsidalGRDkDt(BODY *body, SYSTEM *system, int *iaBody) {
  return -body[iaBody[0]].dHecc*fdApsidalGRCorrection(body,iaBody);
}

//-------------------DistOrb's equations in h k p q -------------------------------------
double fdDistOrbDhDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;
  //Here, iaBody[0] = body in question, iaBody[1] = perturber

  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
  y = fabs(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2));
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += ( sqrt(y)*fdDdisturbDKecc(body, system, iaBody) + body[iaBody[0]].dKecc*(body[iaBody[0]].dPinc*fdDdisturbDPinc(body, system, iaBody) +body[iaBody[0]].dQinc*fdDdisturbDQinc(body, system, iaBody))/(2*sqrt(y)) ) / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      
  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += ( sqrt(y)*fdDdisturbDKeccPrime(body, system, iaBody) + body[iaBody[0]].dKecc*(body[iaBody[0]].dPinc*fdDdisturbDPincPrime(body, system, iaBody) +body[iaBody[0]].dQinc*fdDdisturbDQincPrime(body, system, iaBody))/(2*sqrt(y)) ) / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
  }
  
  return sum/DAYSEC;
}

double fdDistOrbDkDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;
  
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
  y = fabs(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2));
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += -( sqrt(y)*fdDdisturbDHecc(body, system, iaBody) + body[iaBody[0]].dHecc*(body[iaBody[0]].dPinc*fdDdisturbDPinc(body, system, iaBody) +body[iaBody[0]].dQinc*fdDdisturbDQinc(body, system, iaBody))/(2*sqrt(y)) ) / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      
  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += -( sqrt(y)*fdDdisturbDHeccPrime(body, system, iaBody) + body[iaBody[0]].dHecc*(body[iaBody[0]].dPinc*fdDdisturbDPincPrime(body, system, iaBody) +body[iaBody[0]].dQinc*fdDdisturbDQincPrime(body, system, iaBody))/(2*sqrt(y)) ) / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
  }      

  return sum/DAYSEC;
}

double fdDistOrbDpDt(BODY *body, SYSTEM *system, int *iaBody) {
    double sum = 0.0, dMu, y;
    
    dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
    y = fabs(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2));
    if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dPinc*(-body[iaBody[0]].dKecc*fdDdisturbDHecc(body, system, iaBody)+body[iaBody[0]].dHecc*fdDdisturbDKecc(body, system, iaBody)) + 1.0/2.0*fdDdisturbDQinc(body, system, iaBody) )/(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
    } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dPinc*(-body[iaBody[0]].dKecc*fdDdisturbDHeccPrime(body, system, iaBody)+body[iaBody[0]].dHecc*fdDdisturbDKeccPrime(body, system, iaBody)) + 1.0/2.0*fdDdisturbDQincPrime(body, system, iaBody) )/(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
    }
    
    return sum/DAYSEC;
}


double fdDistOrbDqDt(BODY *body, SYSTEM *system, int *iaBody) {
    double sum = 0.0, dMu, y;
    
    dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
    y = fabs(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2));
    if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dQinc*(-body[iaBody[0]].dKecc*fdDdisturbDHecc(body, system, iaBody)+body[iaBody[0]].dHecc*fdDdisturbDKecc(body, system, iaBody)) - 1.0/2.0*fdDdisturbDPinc(body, system, iaBody) )/(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
    } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dQinc*(-body[iaBody[0]].dKecc*fdDdisturbDHeccPrime(body, system, iaBody)+body[iaBody[0]].dHecc*fdDdisturbDKeccPrime(body, system, iaBody)) - 1.0/2.0*fdDdisturbDPincPrime(body, system, iaBody) )/(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
    }
   
    return sum/DAYSEC;
}

