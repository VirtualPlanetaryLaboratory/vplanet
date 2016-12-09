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

void BodyCopyDistOrb(BODY *dest,BODY *src,int iTideModel,int iNumBodies,int iBody) {
  int iIndex,iPert;

  dest[iBody].dPinc = src[iBody].dPinc;
  dest[iBody].dQinc = src[iBody].dQinc;

  dest[iBody].iGravPerts = src[iBody].iGravPerts;
//   dest[iBody].iaGravPerts = malloc(dest[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<src[iBody].iGravPerts;iPert++)
    dest[iBody].iaGravPerts[iPert] = src[iBody].iaGravPerts[iPert];
}

void InitializeBodyDistOrb(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
  if (body[iBody].bDistOrb) {
    if (control->Evolve.iDistOrbModel == RD4) {
      body[iBody].iGravPerts = control->Evolve.iNumBodies - 2;
      body[iBody].iDistOrbModel = RD4;
    } else if (control->Evolve.iDistOrbModel == LL2) {
      /* "Perturbers" in LL2 correspond to eigenfrequencies, not planet pairs. 
         Number of eigenfrequencies = number of planets. */
      body[iBody].iGravPerts = control->Evolve.iNumBodies - 1;
      body[iBody].iDistOrbModel = LL2;
    }
  }
  
  body[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
}

void InitializeUpdateTmpBodyDistOrb(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
  control->Evolve.tmpBody[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
}

/**************** DISTORB options ********************/




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
    body[iFile-1].bGRCorr = atoi(options->cDefault);
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

void ReadOutputLapl(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    control->bOutputLapl = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&control->bOutputLapl,files->iNumInputs);
}

void ReadOverrideMaxEcc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    control->Halt[iFile-1].bOverrideMaxEcc = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    control->Halt[iFile-1].bOverrideMaxEcc = options->dDefault;
//     AssignDefaultInt(options,&control->Halt[iFile-1].bOverrideMaxEcc,files->iNumInputs);
}

void ReadHaltHillStab(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    control->Halt[iFile-1].bHillStab = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    control->Halt[iFile-1].bHillStab = options->dDefault;
}

void ReadHaltCloseEnc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    control->Halt[iFile-1].bCloseEnc = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    control->Halt[iFile-1].bCloseEnc = options->dDefault;
}

void ReadEigenSet(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    body[iFile-1].bEigenSet = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    body[iFile-1].bEigenSet = options->dDefault;
//     AssignDefaultInt(options,&control->Halt[iFile-1].bOverrideMaxEcc,files->iNumInputs);
} 

void ReadEigenvalue(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    
    dTmp *= DEGRAD/3600./YEARSEC;
    
    body[iFile-1].dEigenvalue = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dEigenvalue = options->dDefault;
}  

void ReadEigenvector(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    
    body[iFile-1].dEigenvector = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dEigenvector = options->dDefault;
}  

void ReadOrbitModel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  char cTmp[OPTLEN];

  /* Tide Model, use #defined variables */

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* This parameter can not appear in the primary input,
       as it is module specific (it's easier to code this
       way. It should also only appear in one body file
       so as different tidal models cannot be called. */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"ll2",3)) {
      control->Evolve.iDistOrbModel = LL2;
    } else if (!memcmp(sLower(cTmp),"rd4",3)) {
      control->Evolve.iDistOrbModel = RD4;
    } else {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are ll2 or rd4.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);  
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&control->Evolve.iDistOrbModel,files->iNumInputs);
}


void InitializeOptionsDistOrb(OPTIONS *options,fnReadOption fnRead[]) {
  sprintf(options[OPT_DFCRIT].cName,"dDfcrit");
  sprintf(options[OPT_DFCRIT].cDescr,"Tolerance parameter for recalculating semi- functions");
  sprintf(options[OPT_DFCRIT].cDefault,"0.1");
  options[OPT_DFCRIT].dDefault = 0.1;
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
  
  sprintf(options[OPT_ORBITMODEL].cName,"sOrbitModel");
  sprintf(options[OPT_ORBITMODEL].cDescr,"Orbit Model: ll2 [laplace-lagrange (eigen), 2nd order] rd4 [direct dist-fxn, 4th order]");
  sprintf(options[OPT_ORBITMODEL].cDefault,"rd4");
  options[OPT_ORBITMODEL].dDefault = RD4;
  options[OPT_ORBITMODEL].iType = 1;
  fnRead[OPT_ORBITMODEL] = &ReadOrbitModel;
  
  sprintf(options[OPT_ORMAXECC].cName,"bOverrideMaxEcc");
  sprintf(options[OPT_ORMAXECC].cDescr,"Override default maximum eccentricity in DistOrb (MaxEcc = MAXORBDISTORB)");
  sprintf(options[OPT_ORMAXECC].cDefault,"0");
  options[OPT_ORMAXECC].dDefault = 0;
  options[OPT_ORMAXECC].iType = 0;  
  options[OPT_ORMAXECC].iMultiFile = 0; 
  fnRead[OPT_ORMAXECC] = &ReadOverrideMaxEcc;
  
  sprintf(options[OPT_HALTHILLSTAB].cName,"bHaltHillStab");
  sprintf(options[OPT_HALTHILLSTAB].cDescr,"Enforce Hill stability criterion (halt if failed)");
  sprintf(options[OPT_HALTHILLSTAB].cDefault,"0");
  options[OPT_HALTHILLSTAB].dDefault = 0;
  options[OPT_HALTHILLSTAB].iType = 0;  
  options[OPT_HALTHILLSTAB].iMultiFile = 0; 
  fnRead[OPT_HALTHILLSTAB] = &ReadHaltHillStab;
  
  sprintf(options[OPT_HALTCLOSEENC].cName,"bHaltCloseEnc");
  sprintf(options[OPT_HALTCLOSEENC].cDescr,"Halt if orbits get too close");
  sprintf(options[OPT_HALTCLOSEENC].cDefault,"0");
  options[OPT_HALTCLOSEENC].dDefault = 0;
  options[OPT_HALTCLOSEENC].iType = 0;  
  options[OPT_HALTCLOSEENC].iMultiFile = 0; 
  fnRead[OPT_HALTCLOSEENC] = &ReadHaltCloseEnc;
  
  sprintf(options[OPT_EIGENSET].cName,"bEigenSet");
  sprintf(options[OPT_EIGENSET].cDescr,"Set this to provide eigenvalues/vectors at input");
  sprintf(options[OPT_EIGENSET].cDefault,"0");
  options[OPT_EIGENSET].dDefault = 0;
  options[OPT_EIGENSET].iType = 0;  
  options[OPT_EIGENSET].iMultiFile = 0; 
  fnRead[OPT_EIGENSET] = &ReadEigenSet;
  
  sprintf(options[OPT_EIGENVALUE].cName,"dEigenvalue");
  sprintf(options[OPT_EIGENVALUE].cDescr,"Set this to provide eigenvalues/vectors at input");
  sprintf(options[OPT_EIGENVALUE].cDefault,"0");
  options[OPT_EIGENVALUE].dDefault = 0;
  options[OPT_EIGENVALUE].iType = 0;  
  options[OPT_EIGENVALUE].iMultiFile = 0; 
  fnRead[OPT_EIGENVALUE] = &ReadEigenvalue;
  
  sprintf(options[OPT_EIGENVECTOR].cName,"dEigenvector");
  sprintf(options[OPT_EIGENVECTOR].cDescr,"Set this to provide eigenvalues/vectors at input");
  sprintf(options[OPT_EIGENVECTOR].cDefault,"0");
  options[OPT_EIGENVECTOR].dDefault = 0;
  options[OPT_EIGENVECTOR].iType = 0;  
  options[OPT_EIGENVECTOR].iMultiFile = 0; 
  fnRead[OPT_EIGENVECTOR] = &ReadEigenvector;
  
  sprintf(options[OPT_OUTPUTLAPL].cName,"bOutputLapl");
  sprintf(options[OPT_OUTPUTLAPL].cDescr,"Output Laplace functions and related data");
  sprintf(options[OPT_OUTPUTLAPL].cDefault,"0");
  options[OPT_OUTPUTLAPL].dDefault = 0;
  options[OPT_OUTPUTLAPL].iType = 0;  
  options[OPT_OUTPUTLAPL].iMultiFile = 0; 
  fnRead[OPT_OUTPUTLAPL] = &ReadOutputLapl;
  
  
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

void VerifyOrbitModel(CONTROL *control,FILES *files,OPTIONS *options) {
  int iFile,iFound=0;
  char cTmp[8];

  for (iFile=0;iFile<files->iNumInputs;iFile++) {
    if (options[OPT_ORBITMODEL].iLine[iFile] >= 0)
      iFound++;
  }

  if (iFound > 1) {
    if (control->Io.iVerbose > VERBERR) {
      fprintf(stderr,"ERROR: Option %s set multiple times.\n",options[OPT_ORBITMODEL].cName);
      for (iFile=0;iFile<files->iNumInputs;iFile++) {
        if (options[OPT_ORBITMODEL].iLine[iFile] >= 0)
          fprintf(stderr,"\tFile %s, Line: %d\n",files->Infile[0].cIn,options[OPT_ORBITMODEL].iLine[iFile]);
      }
    }
    exit(EXIT_INPUT);
  }

  if (iFound == 0) {
    strcpy(cTmp,options[OPT_ORBITMODEL].cDefault);
    if (!memcmp(sLower(cTmp),"ll2",3)) {
      control->Evolve.iDistOrbModel = LL2;
    } else if (!memcmp(sLower(cTmp),"rd4",3)) {
      control->Evolve.iDistOrbModel = RD4;
    }
    if (control->Io.iVerbose >= VERBINPUT) 
      fprintf(stderr,"WARNING: %s not set in any file, defaulting to %s.\n",options[OPT_ORBITMODEL].cName,options[OPT_ORBITMODEL].cDefault);

    /* Chicanery. Since I only want this set once, I will
       make it seem like the user set it. */
    options[OPT_ORBITMODEL].iLine[0] = 1;
  }  
}


// void VerifyStabilityHalts(CONTROL *control,FILES *files,OPTIONS *options) {
//   int iFile, iHillSet, iCloseSet;
//   /* make sure user doesn't set both bHaltHillStab and bHaltCloseEnc */
//   
// }

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

void InitializeHeccDistOrbRD4(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = 2;
  update[iBody].padDHeccDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeKeccDistOrbRD4(BODY *body,UPDATE *update,int iBody,int iPert) {
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

void InitializePincDistOrbRD4(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] = 2;
  update[iBody].padDPincDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeQincDistOrbRD4(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] = 2;
  update[iBody].padDQincDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void VerifyPerturbersDistOrbRD4(BODY *body,int iNumBodies,int iBody) {
  int iPert=0, j;
  
//   body[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
  for (j=1;j<iNumBodies;j++) {
    if (j != iBody) {
      if (body[j].bDistOrb == 0) {
        fprintf(stderr,"ERROR: DistOrb must be the called for all planets\n");
        exit(EXIT_INPUT);
      }
      body[iBody].iaGravPerts[iPert] = j;
      iPert++;
    }
  }
}

void InitializeHeccDistOrbLL2(BODY *body,UPDATE *update,int iBody,int iPert) {
  if (body[iBody].bEqtide) {
    update[iBody].iaType[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = 2;
  } else {
    update[iBody].iaType[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = 3;
  }
  update[iBody].padDHeccDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeKeccDistOrbLL2(BODY *body,UPDATE *update,int iBody,int iPert) {
  if (body[iBody].bEqtide) {
    update[iBody].iaType[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = 2;
  } else {
    update[iBody].iaType[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = 3;
  }
  update[iBody].padDKeccDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializePincDistOrbLL2(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] = 3;
  update[iBody].padDPincDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeQincDistOrbLL2(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] = 3;
  update[iBody].padDQincDtDistOrb[iPert] = &update[iBody].daDerivProc[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]];
  update[iBody].iNumBodies[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]]=2;
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void VerifyPerturbersDistOrbLL2(BODY *body,int iNumBodies,int iBody) {
  int iPert=0, j;
  
//   body[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
  for (j=1;j<iNumBodies;j++) {
    if (body[j].bDistOrb == 0) {
        fprintf(stderr,"ERROR: DistOrb must be the called for all planets\n");
        exit(EXIT_INPUT);
    }
    body[iBody].iaGravPerts[iPert] = j;
    iPert++;
  }
}

void VerifyGRCorrLL2(BODY *body, int iNumBodies) {
  int iBody;
  
  for (iBody=2;iBody<iNumBodies;iBody++) {
    if (body[iBody].bGRCorr != body[1].bGRCorr) {
      fprintf(stderr,"ERROR: bGRCorr must be the same for all planets in DistOrb LL2 model\n");
      exit(EXIT_INPUT);
    }
  }
}

void VerifyDistOrb(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int i, j=0, iPert=0, jBody=0,kBody;
  
  VerifyOrbitModel(control,files,options);
//   VerifyStabilityHalts(control,files,options);
  body[iBody].dLOrb = malloc(3*sizeof(double));
  body[iBody].dLOrbTmp = malloc(3*sizeof(double));
  if (iBody == 1) system->dLOrb = malloc(3*sizeof(double));
  body[iBody].dMeanA = 0.0;
  
  if (control->Evolve.iDistOrbModel == RD4) {
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

      VerifyPerturbersDistOrbRD4(body,control->Evolve.iNumBodies,iBody);
      control->fnPropsAux[iBody][iModule] = &PropsAuxDistOrb;
      
      CalcHK(body,iBody);
      CalcPQ(body,iBody);
      /* Body updates */
      for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
        /* h = Ecc*sin(LongP) */
        InitializeHeccDistOrbRD4(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = &fdDistOrbRD4DhDt;
        
        /* k = Ecc*cos(LongP) */
        InitializeKeccDistOrbRD4(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = &fdDistOrbRD4DkDt;
        
        /* p = s*sin(LongA) */
        InitializePincDistOrbRD4(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] = &fdDistOrbRD4DpDt;
        
        /* q = s*cos(LongA) */
        InitializeQincDistOrbRD4(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] = &fdDistOrbRD4DqDt;
        
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
	  /* Must initialize dMeanA to prevent memory corruption. This 
	     parameter has no real meaning in DistOrb runs, but inv_plave
	     requires it. I will set it to zero for each body. --RKB */
	  for (kBody=0;kBody<control->Evolve.iNumBodies;kBody++)
	    body[kBody].dMeanA = 0;
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
  } else if (control->Evolve.iDistOrbModel == LL2) {
    VerifyPericenter(body,control,options,files->Infile[iBody+1].cIn,iBody,control->Io.iVerbose);
    control->fnPropsAux[iBody][iModule] = &PropsAuxDistOrb;
    
    CalcHK(body,iBody);
    CalcPQ(body,iBody);
    
    if (body[iBody].bEigenSet == 1) {
      /* XXX really stupid hack */
      system->dmEigenValEcc = malloc(2*sizeof(double*));
      system->dmEigenValInc = malloc(2*sizeof(double*));
      system->dmEigenVecEcc = malloc((control->Evolve.iNumBodies-1)*sizeof(double*));
      system->dmEigenVecInc = malloc((control->Evolve.iNumBodies-1)*sizeof(double*));
      system->dmEigenPhase = malloc(2*sizeof(double*));
      
      system->dmEigenValEcc[0] = malloc(1*sizeof(double));
      system->dmEigenValInc[0] = malloc(1*sizeof(double));
      system->dmEigenVecEcc[0] = malloc(1*sizeof(double));
      system->dmEigenVecInc[0] = malloc(1*sizeof(double));
      system->dmEigenPhase[0] = malloc(1*sizeof(double));
      system->dmEigenPhase[1] = malloc(1*sizeof(double));
            
      system->dmEigenValEcc[0][0] = 0.0;
      //system->dmEigenValInc[0][0] = -0.000123627489;
      system->dmEigenValInc[0][0] = -0.000119874715;
      system->dmEigenVecEcc[0][0] = 0.0;
      //system->dmEigenVecInc[0][0] = 0.00506143322;
      system->dmEigenVecInc[0][0] = 0.0036367199;
      
      system->dmEigenPhase[0][0] = 0.0;
//       system->dmEigenPhase[1][0] = atan2(body[iBody].dHecc,body[iBody].dKecc);
      system->dmEigenPhase[1][0] = 2.6348951757;
      
    } else {
      /* Conditions for recalc'ing eigenvalues */
      if (iBody == 1) {
        system->imLaplaceN = malloc((control->Evolve.iNumBodies)*sizeof(int*));
        system->dmAlpha0 = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(double*));
        system->dmLaplaceD = malloc(Nchoosek(control->Evolve.iNumBodies-1,2)*sizeof(double*));
        for (j=0;j<Nchoosek(control->Evolve.iNumBodies-1,2);j++) {
          system->dmLaplaceD[j] = malloc(2*sizeof(double));
          system->dmAlpha0[j] = malloc(1*sizeof(double));
        }
        for (j=1;j<(control->Evolve.iNumBodies);j++) {
          system->imLaplaceN[j] = malloc((control->Evolve.iNumBodies)*sizeof(int));
        }
      }

      for (jBody=1;jBody<(control->Evolve.iNumBodies);jBody++) {
          if (body[iBody].dSemi < body[jBody].dSemi) {  
              system->imLaplaceN[iBody][jBody] = CombCount(iBody,jBody,control->Evolve.iNumBodies-1);
              system->dmAlpha0[system->imLaplaceN[iBody][jBody]][0] = body[iBody].dSemi/body[jBody].dSemi;
              system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][0] = fdDerivLaplaceCoeff(1,body[iBody].dSemi/body[jBody].dSemi,1,1.5);
              system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][1] = fdDerivLaplaceCoeff(1,body[iBody].dSemi/body[jBody].dSemi,2,1.5);
          } else if (body[iBody].dSemi > body[jBody].dSemi) {
              system->imLaplaceN[iBody][jBody] = CombCount(jBody,iBody,control->Evolve.iNumBodies-1); 
              system->dmAlpha0[system->imLaplaceN[iBody][jBody]][0] = body[jBody].dSemi/body[iBody].dSemi;
              system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][0] = fdDerivLaplaceCoeff(1,body[jBody].dSemi/body[iBody].dSemi,1,1.5);
              system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][1] = fdDerivLaplaceCoeff(1,body[jBody].dSemi/body[iBody].dSemi,2,1.5);
          }
      }
    
//     body[iBody].dSemiPrev = body[iBody].dSemiPrev;
        
      if (iBody == (control->Evolve.iNumBodies-1)) {
        VerifyGRCorrLL2(body,control->Evolve.iNumBodies);
        if (control->bInvPlane) {
          inv_plane(body, system, control->Evolve.iNumBodies);
        }
        system->dmEigenValEcc = malloc(2*sizeof(double*));
        system->dmEigenVecEcc = malloc((control->Evolve.iNumBodies-1)*sizeof(double*));
        system->dmEigenValInc = malloc(2*sizeof(double*));
        system->dmEigenVecInc = malloc((control->Evolve.iNumBodies-1)*sizeof(double*));
        system->dmEigenPhase = malloc(2*sizeof(double*));
      
        system->A = malloc((control->Evolve.iNumBodies-1)*sizeof(double*));
        system->B = malloc((control->Evolve.iNumBodies-1)*sizeof(double*));
        system->Asoln = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
        system->Bsoln = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
        system->Acopy = malloc((control->Evolve.iNumBodies-1)*sizeof(double*));
       
        for (iBody=0;iBody<(control->Evolve.iNumBodies-1);iBody++) {
           system->A[iBody] = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
           system->B[iBody] = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
           system->dmEigenVecEcc[iBody] = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
           system->dmEigenVecInc[iBody] = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
           system->Acopy[iBody] = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
        }
    
        for (i=0;i<2;i++) {
           system->dmEigenValEcc[i] = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
           system->dmEigenValInc[i] = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
           system->dmEigenPhase[i] = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
        }
        system->scale = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
        system->rowswap = malloc((control->Evolve.iNumBodies-1)*sizeof(int));

        
        SolveEigenVal(body,&control->Evolve,system);
      
        system->etmp = malloc((control->Evolve.iNumBodies-1)*sizeof(double*));
        system->itmp = malloc((control->Evolve.iNumBodies-1)*sizeof(double*));
        system->rowswap = malloc((control->Evolve.iNumBodies-1)*sizeof(int));
        system->h0 = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
        system->k0 = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
        system->p0 = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
        system->q0 = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
  
        for (i=0;i<(control->Evolve.iNumBodies-1);i++) {
          system->etmp[i] = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
          system->itmp[i] = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
        }
        system->S = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
        system->T = malloc((control->Evolve.iNumBodies-1)*sizeof(double));
      
        ScaleEigenVec(body,&control->Evolve,system);
      }
    }
    
    body[iBody].iGravPerts = control->Evolve.iNumBodies - 1; 
    VerifyPerturbersDistOrbLL2(body,control->Evolve.iNumBodies,iBody);
    
    for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
        /* h = Ecc*sin(LongP) */
        InitializeHeccDistOrbLL2(body,update,iBody,iPert);
        
        /* k = Ecc*cos(LongP) */
        InitializeKeccDistOrbLL2(body,update,iBody,iPert);
        if (body[iBody].bEqtide) {
          fnUpdate[iBody][update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = &fdDistOrbLL2DhDt;
          fnUpdate[iBody][update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = &fdDistOrbLL2DkDt;
        } else {
          fnUpdate[iBody][update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = &fdDistOrbLL2Hecc;
          fnUpdate[iBody][update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = &fdDistOrbLL2Kecc;
        }
        
        /* p = s*sin(LongA) */
        InitializePincDistOrbLL2(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] = &fdDistOrbLL2Pinc;
        
        /* q = s*cos(LongA) */
        InitializeQincDistOrbLL2(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] = &fdDistOrbLL2Qinc;
    }
    
    // if (body[iBody].bGRCorr) {
//       fprintf(stderr,"ERROR: %s cannot be used in LL2 orbital solution.\n",options[OPT_GRCORR].cName);
//       LineExit(files->Infile[iBody+1].cIn,options[OPT_GRCORR].iLine[iBody+1]);
//     }
  }

  control->fnForceBehavior[iBody][iModule]=&ForceBehaviorDistOrb;
  control->Evolve.fnBodyCopy[iBody][iModule]=&BodyCopyDistOrb;

  if (iBody==7)
    exit(1);
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
      if (body[iBody].iDistOrbModel == RD4) {
        update[iBody].iNumHecc += 1;
        update[iBody].iNumKecc += 1;
      }
    }
  }
}

void FinalizeUpdateHeccDistOrb(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
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

void FinalizeUpdateKeccDistOrb(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
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

void FinalizeUpdatePincDistOrb(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;
  
  update[iBody].padDPincDtDistOrb = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaPincDistOrb = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTORB;
    update[iBody].iaPincDistOrb[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateQincDistOrb(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
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
  if (halt->bOverrideMaxEcc == 0) {
    if (halt->dMaxEcc==1) {
      (*iNumHalts)++;
    }
  }
  /* halt for close encounters */
  if (halt->bHillStab || halt->bCloseEnc)
    (*iNumHalts)++;
}

void VerifyHaltDistOrb(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
  int iHaltMaxEcc=0,iNumMaxEcc=0; // If set for one body, set for all

  /* Mandatory halt for DistOrb */
  if (body[iBody].bDistOrb) {
    if (control->Halt[iBody].bOverrideMaxEcc==0) {
      /* If you don't override max ecc, and you HAVEN'T set it manually for this body, default to MAXECCDISTORB (== 0.6627434) */
      if (control->Halt[iBody].dMaxEcc==1) {
        control->Halt[iBody].dMaxEcc = MAXECCDISTORB;
        control->fnHalt[iBody][(*iHalt)++] = &HaltMaxEcc;
      }
    }
//     control->Halt[iBody].bHillStab = HILLSTABDISTORB;
    if (control->Halt[iBody].bHillStab) {
      control->fnHalt[iBody][(*iHalt)++] = &HaltHillStab;
    } else if (control->Halt[iBody].bCloseEnc) {
      control->fnHalt[iBody][(*iHalt)++] = &HaltCloseEnc;
    }
  }
}

int HaltHillStab(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  int iPert, jBody;
  double mu1, mu2, alpha, gamma1, gamma2, delta, crit, hill;
  
  if (halt->bHillStab == 1) {
    for (iBody=1;iBody<evolve->iNumBodies;iBody++) {
      /* I have to loop over iBody here, ultimately because I want to have to set bHaltHillStab
         only once, not in every file. There is no easy way to set bHillStab for one body and
         distribute that to all others before the number of halts are counted. So, I just set it
         once and check Hill stability for all planets during the one call of HaltHillStab(). */
      for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
        jBody = body[iBody].iaGravPerts[iPert];
        if (body[jBody].dSemi < body[iBody].dSemi) {
          //jBody is the inner planet
          mu1 = body[jBody].dMass/body[0].dMass;
          mu2 = body[iBody].dMass/body[0].dMass;
          gamma1 = sqrt(1-(pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2)));
          gamma2 = sqrt(1-(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2)));
          delta = sqrt(body[iBody].dSemi/body[jBody].dSemi);
        } else if (body[jBody].dSemi > body[iBody].dSemi) {
          //jBody is the outer planet
          mu2 = body[jBody].dMass/body[0].dMass;
          mu1 = body[iBody].dMass/body[0].dMass;
          gamma2 = sqrt(1-(pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2)));
          gamma1 = sqrt(1-(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2)));
          delta = sqrt(body[jBody].dSemi/body[iBody].dSemi);
        }
        alpha = mu1 + mu2;
        crit = 1.0 + pow((3./alpha),(4./3))*mu1*mu2;
        hill = pow(alpha,-3)*(mu1+mu2/pow(delta,2))*pow((mu1*gamma1+mu2*gamma2*delta),2);
        if (hill < crit) {
          if (io->iVerbose >= VERBPROG) {
            printf("HALT: hill stability criterion failed for planets %d and %d",iBody,jBody);
            printf(" at %.2e years\n",evolve->dTime/YEARSEC);
          }
          return 1;
        }
      }
    }
  }
  return 0;
}

int HaltCloseEnc(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  int iPert, jBody;
  double dDR;
  
  if (halt->bCloseEnc == 1) {
    for (iBody=1;iBody<evolve->iNumBodies;iBody++) {
      for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
        jBody = body[iBody].iaGravPerts[iPert];
        if (body[jBody].dSemi < body[iBody].dSemi) {
          //comparing apocentre of inner planet with pericentre of outer
          dDR = fabs(body[iBody].dRPeri-body[jBody].dRApo);
          //dDR = MinOrbitSep3D(body,iBody,jBody);
          if (dDR < 4*MutualHillRad(body,iBody,jBody)) {
            if (io->iVerbose >= VERBPROG) {
              printf("HALT: close encounter between planets %d and %d = ",iBody,jBody);
              printf(" at %.2e years\n",evolve->dTime/YEARSEC);
            }
            return 1;
          }
        } else if (body[jBody].dSemi > body[iBody].dSemi) {
          //comparing apocentre of inner planet with pericentre of outer
          dDR = fabs(body[jBody].dRPeri-body[iBody].dRApo);
          // dDR = MinOrbitSep3D(body,iBody,jBody);
          if (dDR < 4*MutualHillRad(body,iBody,jBody)) {
            if (io->iVerbose >= VERBPROG) {
              printf("HALT: close encounter between planets %d and %d = ",iBody,jBody);
              printf(" at %.2e years\n",evolve->dTime/YEARSEC);
            }
            return 1;
          }
        }
      }
    }
  }
  
  return 0;
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
    *dTmp /= fdUnitsAngle(units->iAngle);
//     fsUnitsAngle(units->iAngle,cUnit);
    fsUnitsAngRate(units,cUnit);
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
    *dTmp /= fdUnitsAngle(units->iAngle);
//     fsUnitsAngle(units->iAngle,cUnit);
    fsUnitsAngRate(units,cUnit);
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
    *dTmp /= fdUnitsAngle(units->iAngle);
//     fsUnitsAngle(units->iAngle,cUnit);
    fsUnitsAngRate(units,cUnit);
  }
} 

void WriteBodySinc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = sqrt(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2));
  strcpy(cUnit,"");
}  

void WriteBodyInc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  if (body[iBody].bDistOrb) {
    *dTmp = 2.*asin(sqrt(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2)));  
  } else if (body[iBody].bGalHabit) {
    *dTmp = body[iBody].dInc;
  }
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}  

void WriteBodyLongA(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  if (body[iBody].bDistOrb) {
    *dTmp = atan2(body[iBody].dPinc, body[iBody].dQinc);
  } else if (body[iBody].bGalHabit) {
    *dTmp = body[iBody].dLongA;
  }
  
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
  output[OUT_DECCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DECCDTDISTORB] = &WriteBodyDEccDtDistOrb;
  
  sprintf(output[OUT_DSINCDTDISTORB].cName,"DSincDtDistOrb");
  sprintf(output[OUT_DSINCDTDISTORB].cDescr,"Body's dsin(.5*Inc)/dt in DistOrb");
  sprintf(output[OUT_DSINCDTDISTORB].cNeg,"1/year");
  output[OUT_DSINCDTDISTORB].bNeg = 1;
  output[OUT_DSINCDTDISTORB].dNeg = YEARSEC;
  output[OUT_DSINCDTDISTORB].iNum = 1;
  output[OUT_DSINCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DSINCDTDISTORB] = &WriteBodyDSincDtDistOrb;
 
  sprintf(output[OUT_DINCDTDISTORB].cName,"DIncDtDistOrb");
  sprintf(output[OUT_DINCDTDISTORB].cDescr,"Body's dInc/dt in DistOrb");
  sprintf(output[OUT_DINCDTDISTORB].cNeg,"deg/year");
  output[OUT_DINCDTDISTORB].bNeg = 1;
  output[OUT_DINCDTDISTORB].dNeg = YEARSEC/DEGRAD;
  output[OUT_DINCDTDISTORB].iNum = 1;
  output[OUT_DINCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DINCDTDISTORB] = &WriteBodyDIncDtDistOrb;
  
  sprintf(output[OUT_DLONGPDTDISTORB].cName,"DLongPDtDistOrb");
  sprintf(output[OUT_DLONGPDTDISTORB].cDescr,"Body's dvarpi/dt in DistOrb");
  sprintf(output[OUT_DLONGPDTDISTORB].cNeg,"deg/yr");
  output[OUT_DLONGPDTDISTORB].bNeg = 1;
  output[OUT_DLONGPDTDISTORB].dNeg = YEARSEC/DEGRAD;
  output[OUT_DLONGPDTDISTORB].iNum = 1;
  output[OUT_DLONGPDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DLONGPDTDISTORB] = &WriteBodyDLongPDtDistOrb;
  
  sprintf(output[OUT_DLONGADTDISTORB].cName,"DLongADtDistOrb");
  sprintf(output[OUT_DLONGADTDISTORB].cDescr,"Body's dOmega/dt in DistOrb");
  sprintf(output[OUT_DLONGADTDISTORB].cNeg,"deg/yr");
  output[OUT_DLONGADTDISTORB].bNeg = 1;
  output[OUT_DLONGADTDISTORB].dNeg = YEARSEC/DEGRAD;
  output[OUT_DLONGADTDISTORB].iNum = 1;
  output[OUT_DLONGADTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DLONGADTDISTORB] = &WriteBodyDLongADtDistOrb;
  
  sprintf(output[OUT_INC].cName,"Inc");
  sprintf(output[OUT_INC].cDescr,"Body's Inclination in DistOrb");
  sprintf(output[OUT_INC].cNeg,"Deg");
  output[OUT_INC].bNeg = 1;
  output[OUT_INC].dNeg = 1./DEGRAD;
  output[OUT_INC].iNum = 1;
  output[OUT_INC].iModuleBit = DISTORB+GALHABIT;
  fnWrite[OUT_INC] = &WriteBodyInc;
  
  sprintf(output[OUT_SINC].cName,"Sinc");
  sprintf(output[OUT_SINC].cDescr,"Body's sin(1/2*Inclination) in DistOrb");
  output[OUT_SINC].iNum = 1;
  output[OUT_SINC].iModuleBit = DISTORB;
  fnWrite[OUT_SINC] = &WriteBodySinc;
  
  sprintf(output[OUT_LONGA].cName,"LongA");
  sprintf(output[OUT_LONGA].cDescr,"Body's Longitude of ascending node in DistOrb");
  sprintf(output[OUT_LONGA].cNeg,"Deg");
  output[OUT_LONGA].bNeg = 1;
  output[OUT_LONGA].dNeg = 1./DEGRAD;
  output[OUT_LONGA].iNum = 1;
  output[OUT_LONGA].iModuleBit = DISTORB+GALHABIT;
  fnWrite[OUT_LONGA] = &WriteBodyLongA; 

  sprintf(output[OUT_PINC].cName,"Pinc");
  sprintf(output[OUT_PINC].cDescr,"Body's p = s*sin(Omega) in DistOrb");
  output[OUT_PINC].iNum = 1;
  output[OUT_PINC].iModuleBit = DISTORB;
  fnWrite[OUT_PINC] = &WriteBodyPinc;
  
  sprintf(output[OUT_QINC].cName,"Qinc");
  sprintf(output[OUT_QINC].cDescr,"Body's q = s* cos(Omega) in DistOrb");
  output[OUT_QINC].iNum = 1;
  output[OUT_QINC].iModuleBit = DISTORB;
  fnWrite[OUT_QINC] = &WriteBodyQinc;
  
  sprintf(output[OUT_DHECCDTDISTORB].cName,"DHeccDtDistOrb");
  sprintf(output[OUT_DHECCDTDISTORB].cDescr,"Body's dh/dt in DistOrb");
  sprintf(output[OUT_DHECCDTDISTORB].cNeg,"1/year");
  output[OUT_DHECCDTDISTORB].bNeg = 1;
  output[OUT_DHECCDTDISTORB].dNeg = YEARSEC;
  output[OUT_DHECCDTDISTORB].iNum = 1;
  output[OUT_DHECCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DHECCDTDISTORB] = &WriteBodyDHeccDtDistOrb;
  
  sprintf(output[OUT_DKECCDTDISTORB].cName,"DKeccDtDistOrb");
  sprintf(output[OUT_DKECCDTDISTORB].cDescr,"Body's dk/dt in DistOrb");
  sprintf(output[OUT_DKECCDTDISTORB].cNeg,"1/year");
  output[OUT_DKECCDTDISTORB].bNeg = 1;
  output[OUT_DKECCDTDISTORB].dNeg = YEARSEC;
  output[OUT_DKECCDTDISTORB].iNum = 1;
  output[OUT_DKECCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DKECCDTDISTORB] = &WriteBodyDKeccDtDistOrb;
  
  sprintf(output[OUT_DPINCDTDISTORB].cName,"DPincDtDistOrb");
  sprintf(output[OUT_DPINCDTDISTORB].cDescr,"Body's dp/dt in DistOrb");
  sprintf(output[OUT_DPINCDTDISTORB].cNeg,"1/year");
  output[OUT_DPINCDTDISTORB].bNeg = 1;
  output[OUT_DPINCDTDISTORB].dNeg = YEARSEC;
  output[OUT_DPINCDTDISTORB].iNum = 1;
  output[OUT_DPINCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DPINCDTDISTORB] = &WriteBodyDPincDtDistOrb;
  
  sprintf(output[OUT_DQINCDTDISTORB].cName,"DQincDtDistOrb");
  sprintf(output[OUT_DQINCDTDISTORB].cDescr,"Body's dq/dt in DistOrb");
  sprintf(output[OUT_DQINCDTDISTORB].cNeg,"1/year");
  output[OUT_DQINCDTDISTORB].bNeg = 1;
  output[OUT_DQINCDTDISTORB].dNeg = YEARSEC;
  output[OUT_DQINCDTDISTORB].iNum = 1;
  output[OUT_DQINCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DQINCDTDISTORB] = &WriteBodyDQincDtDistOrb;
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

  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyDistOrb;
  module->fnCountHalts[iBody][iModule] = &CountHaltsDistOrb;
  module->fnLogBody[iBody][iModule] = &LogBodyDistOrb;

  module->fnReadOptions[iBody][iModule] = &ReadOptionsDistOrb;
  module->fnVerify[iBody][iModule] = &VerifyDistOrb;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltDistOrb;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyDistOrb;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateDistOrb;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputDistOrb;
  module->fnFinalizeUpdateHecc[iBody][iModule] = &FinalizeUpdateHeccDistOrb;
  module->fnFinalizeUpdateKecc[iBody][iModule] = &FinalizeUpdateKeccDistOrb;
  module->fnFinalizeUpdatePinc[iBody][iModule] = &FinalizeUpdatePincDistOrb;
  module->fnFinalizeUpdateQinc[iBody][iModule] = &FinalizeUpdateQincDistOrb;
}

/************* DistOrb Functions ************/
void PropsAuxDistOrb(BODY *body,EVOLVE *evolve,UPDATE *update,int iBody) { 
  /* Conflict XXX -- Hopefully this is wrong as there should be no calls to POISE in DISTORB 
  if (body[iBody].bPoise) {
    body[iBody].dLongP = atan2(body[iBody].dHecc,body[iBody].dKecc);
    body[iBody].dEcc = sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2));
  }
*/
  body[iBody].dLongP = atan2(body[iBody].dHecc,body[iBody].dKecc);
  body[iBody].dEcc = sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2));
  body[iBody].dSinc = sqrt(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2));
    
  body[iBody].dRPeri = (1.0-sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2)))* \
                          body[iBody].dSemi;
  body[iBody].dRApo = (1.0+sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2)))* \
                          body[iBody].dSemi;
}

void ForceBehaviorDistOrb(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
}

double MinOrbitSep2D(BODY *body, int iBody, int jBody) {
  double dTheta=0, dMin, dR1, dR2, dDR;
  int i;
  
  dMin = fabs(body[iBody].dSemi-body[jBody].dSemi);
  for (i=0;i<360;i++) {
    dR1 = body[iBody].dSemi*(1-pow(body[iBody].dHecc,2)-pow(body[iBody].dKecc,2))/ \
         (1+sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2))*cos(dTheta-body[iBody].dLongP));
    dR2 = body[jBody].dSemi*(1-pow(body[jBody].dHecc,2)-pow(body[jBody].dKecc,2))/ \
         (1+sqrt(pow(body[jBody].dHecc,2)+pow(body[jBody].dKecc,2))*cos(dTheta-body[jBody].dLongP));
    dDR = fabs(dR1-dR2);
    if (dDR < dMin) {
      dMin = dDR;
    }
    dTheta += PI/180.;
  }
  
  return dMin;
}

double MinOrbitSep3D(BODY *body, int iBody, int jBody) {
  //XXX currently only works if bInvPlane = 1, since dCartPos is not malloc'd otherwise
    
  double dTheta=0, dMin, dR1, dR2, dDR, cosE, f, xtmp, ytmp;
  int i;
  
  dMin = HUGE;
  for (i=0;i<360;i++) {
    f = (dTheta - body[iBody].dLongP);
    while (f<0) f+=2*PI;
    cosE = (cos(f)+body[iBody].dEcc) / (1.0+body[iBody].dEcc*cos(f));
    if (f <= PI) body[iBody].dEccA = acos(cosE);
    if (f > PI) body[iBody].dEccA = 2.*PI - acos(cosE);
  
    xtmp = xinit(body, iBody);
    ytmp = yinit(body, iBody);
    
    body[iBody].dCartPos[0] = xtmp*(xangle1(body,iBody))+ytmp*(xangle2(body,iBody));
    body[iBody].dCartPos[1] = xtmp*(yangle1(body,iBody))+ytmp*(yangle2(body,iBody));
    body[iBody].dCartPos[2] = xtmp*(zangle1(body,iBody))+ytmp*(zangle2(body,iBody));
    
    f = (dTheta - body[jBody].dLongP);
    while (f<0) f+=2*PI;
    cosE = (cos(f)+body[jBody].dEcc) / (1.0+body[jBody].dEcc*cos(f));
    if (f <= PI) body[jBody].dEccA = acos(cosE);
    if (f > PI) body[jBody].dEccA = 2.*PI - acos(cosE);
    
    xtmp = xinit(body, jBody);
    ytmp = yinit(body, jBody);
    
    body[jBody].dCartPos[0] = xtmp*(xangle1(body,jBody))+ytmp*(xangle2(body,jBody));
    body[jBody].dCartPos[1] = xtmp*(yangle1(body,jBody))+ytmp*(yangle2(body,jBody));
    body[jBody].dCartPos[2] = xtmp*(zangle1(body,jBody))+ytmp*(zangle2(body,jBody));
    
    dDR = sqrt(pow(body[iBody].dCartPos[0]-body[jBody].dCartPos[0],2) + \
               pow(body[iBody].dCartPos[1]-body[jBody].dCartPos[1],2) + \
               pow(body[iBody].dCartPos[2]-body[jBody].dCartPos[2],2));
  
    dDR *= AUCM;
    
    if (dDR < dMin) {
      dMin = dDR;
    }
    dTheta += PI/180.;
  }
  
  return dMin;
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
  if (x < y) {
    return N*(x-1) + (y-1) - Nchoosek(x+1, 2);
  } else {
    return N*(y-1) + (x-1) - Nchoosek(y+1, 2);
  }
} 

double ABmatrix(BODY *body, int j, int jBody, int kBody) {
  double AB, alpha, abar, b, n;
  
  if (body[jBody].dSemi > body[kBody].dSemi) {
    alpha = body[kBody].dSemi/body[jBody].dSemi;  //internal perturber
    abar = 1.0;
  } else if (body[jBody].dSemi < body[kBody].dSemi) {
    alpha = body[jBody].dSemi/body[kBody].dSemi;  //external perturber
    abar = alpha;
  }
  
  n = KGAUSS*sqrt((body[0].dMass+body[jBody].dMass)/MSUN/(pow(body[jBody].dSemi/AUCM,3)));
  b = fdLaplaceCoeff(alpha, j, 1.5);
  AB = n/4.0*body[kBody].dMass/(body[0].dMass+body[jBody].dMass)*alpha*abar*b;
  return AB*365.25;  //returns in units of rad/year
}

double MutualHillRad(BODY *body, int iBody, int jBody) {
  return 0.5*pow((body[iBody].dMass+body[jBody].dMass)/body[0].dMass,1./3)*\
          (body[iBody].dSemi+body[jBody].dSemi);
}

double GRCorrMatrix(BODY *body, int jBody, int kBody) {
  double n, GRC;
  
  n = KGAUSS*sqrt((body[0].dMass+body[jBody].dMass)/MSUN/(pow(body[jBody].dSemi/AUCM,3)));
  if (jBody == kBody) {
    GRC = 3*pow(n,3)*pow(body[jBody].dSemi/AUCM,2)/(pow(cLIGHT/AUCM*DAYSEC,2)* (1.0-pow(body[jBody].dHecc,2)-pow(body[jBody].dKecc,2)));
    return GRC*365.25; 
  } else {
    return 0.0;
  }
}

/* XXX HessEigen, ElmHess, BalanceM, ludcmp, lukskb are from Numerical Recipes, Press et al. (1992)
Cannot release code with these functions as they are proprietary.
They will need to be phased out as my new routines are tested and confirmed to work ok. HessEigen has variables renamed from "hqr" but is not fully replaced yet. */  

void HessEigen(double **amat, int origsize, double real[], double imag[])
/*Finds all eigenvalues of an upper Hess. matrix amat */
{
  int size, m, smallsub, k, j, iterations, i, mmin;
  double radic, ulcorner, lrcorner, hhvector, v, u, exshift, s, r, q, p, anorm, cond, value;
  //s, r, q, and p are defined in numerical recipes eqns 11.6.23, 11.6.25

  anorm = fabs(amat[0][0]);
  for (i = 1; i <= origsize-1; i++) 
    for (j = (i-1); j <= origsize-1; j++)
      anorm += fabs(amat[i][j]);
  
  size = origsize-1;
  exshift = 0.0;
  while (size >= 0) {
    iterations = 0;
    do { 
      for (smallsub = size; smallsub >= 1; smallsub--) {
        s = fabs(amat[smallsub-1][smallsub-1]) + fabs(amat[smallsub][smallsub]);
        if (s == 0.0) s = anorm;
        cond = fabs(amat[smallsub][smallsub-1])+s;
        if ((float)cond ==(float) s) break;
      }
      lrcorner = amat[size][size];
      if (smallsub == size) {
        real[size] = lrcorner + exshift;
        imag[size--] = 0.0;
      } else {
        ulcorner = amat[size-1][size-1];
        hhvector = amat[size][size-1]*amat[size-1][size];
        if (smallsub == (size-1)) {
          p = 0.5*(ulcorner-lrcorner);
          q = p*p + hhvector;
          radic = sqrt(fabs(q));
          lrcorner += exshift;
          if (q >= 0.0) {
            radic = p+(double)signf(p)*radic;
            real[size-1] = real[size] = lrcorner+radic;
            if (radic) real[size] = lrcorner-hhvector/radic;
            imag[size-1] = imag[size] = 0.0;
          } else {
            real[size-1] = real[size] = lrcorner + p;
            imag[size-1] = -(imag[size] = radic);
          }
          size -= 2;
        } else {
          if (iterations == 30) {
            fprintf(stderr,"Too many iterations in HessEigen routine\n");
            exit(EXIT_INPUT);
          }
          if (iterations == 10 || iterations == 20) {
            exshift += lrcorner;
            for (i = 0; i <= size; i++) amat[i][i] -= lrcorner;
            s = fabs(amat[size][size-1])+fabs(amat[size-1][size-2]);
            ulcorner = lrcorner = 0.75*s;
            hhvector = -0.4375*s*s;
          }
          ++iterations;
        

          for (m = (size-2); m >= smallsub; m--) {
            radic = amat[m][m];
            r = lrcorner-radic;
            s = ulcorner-radic;
            p = (r*s-hhvector)/amat[m+1][m] + amat[m][m+1];
            q = amat[m+1][m+1] - radic - r - s;
            r = amat[m+2][m+1];
            s = fabs(p) + fabs(q) + fabs(r);
            p /= s;
            q /= s;
            r /= s;
            if (m == smallsub) break;
            u = fabs(amat[m][m-1])*(fabs(q) + fabs(r));
            v = fabs(p)*(fabs(amat[m-1][m-1]) + fabs(radic) + fabs(amat[m+1][m+1]));
            if ((float)(u + v) ==(float) v) break;
          }
          for (i = m+2; i <= size; i++) {
            amat[i][i-2] = 0.0;
            if (i != (m+2)) amat[i][i-3] = 0.0;
          }
          for (k = m; k <= size-1; k++) {
            if (k != m) {
              p = amat[k][k-1];
              q = amat[k+1][k-1];
              r =0.0;
              if (k != (size-1)) r = amat[k+2][k-1];
              if ((lrcorner = fabs(p) + fabs(q) + fabs(r)) != 0.0) {
                p /= lrcorner;
                q /= lrcorner;
                r /= lrcorner;
              }
            }
            value = sqrt(p*p+q*q+r*r);
            s = (double)signf(p)*value;
            if (s != 0.0) {
              if (k == m) {
                if (smallsub != m)
                  amat[k][k-1] = -amat[k][k-1];
              } else
                amat[k][k-1] = -s*lrcorner;
              p += s;
              lrcorner = p/s;
              ulcorner = q/s;
              radic = r/s;
              q /= p;
              r /= p;
              for (j = k; j <= size; j++) {
                p = amat[k][j] + q*amat[k+1][j];
                if (k != (size-1)) {
                  p += r*amat[k+2][j];
                  amat[k+2][j] -= p*radic;
                }
                amat[k+1][j] -= p*ulcorner;
                amat[k][j] -= p*lrcorner;
              }
              mmin = size < k+3 ? size : k+3;
              for (i = smallsub; i <= mmin; i++) {
                p = lrcorner*amat[i][k] + ulcorner*amat[i][k+1];
                if (k != (size-1)) {
                  p += radic*amat[i][k+2];
                  amat[i][k+2] -= p*r;
                }
                amat[i][k+1] -= p*q;
                amat[i][k] -= p;
              }
            }
          }
        }
      }
    } while (smallsub < size-1);
  }
}

// void Eigen2x2(double **matrix, double *real, double *imag, int *size, int *smallsub) {
//   double a, b , c, d, radic;
//   
//   d = matrix[*size-1][*size-1];
//   if (*smallsub == *size-1) {
//     real[*size-1] = d;
//     imag[*size-1] = 0.0;
//     *size -= 1;   //destroy last row and column of matrix
//   } else if (*smallsub == *size-2) {
//     a = matrix[*size-2][*size-2];
//     b = matrix[*size-1][*size-2];
//     c = matrix[*size-2][*size-1];
//     radic = 0.25*pow(a+d,2)-(a*d-b*c);
//     if (radic >= 0.0) {
//       real[*size-1] = 0.5*(a+d) + sqrt(radic);
//       real[*size-2] = 0.5*(a+d) - sqrt(radic);
//       imag[*size-1] = 0.0;
//       imag[*size-2] = 0.0;
//     } else {
//       real[*size-1] = 0.5*(a+d);
//       real[*size-2] = 0.5*(a+d);
//       imag[*size-1] = sqrt(fabs(radic));
//       imag[*size-2] = -sqrt(fabs(radic));
//     }
//     *size -= 2; //destroy last two rows and columns of matrix
//   }
// }

// void QR4Hess(double **matrix, double *real, double *imag, int size) {
//   /* XXX algorithm is not finished!!! do not eat...er, use */
//   int i, j, smallsub, sizeprev;
//   double norm;
//   
//   norm = 0.0;
//   for (i=0;i<size;i++) {
//     for (j=0;j<size;j++) {
//       norm += pow(matrix[i][j],2);
//     }
//   }
//   norm = sqrt(norm);
//   
//   for (i=size-1;i>0;i--) {
//     cond = (fabs(matrix[i][i-1])+norm);
//     if ((float)cond == (float)norm) {
//       smallsub = i;  //row containing a small sub diagonal
//       break;
//     }
//   }
//   
//   sizeprev = size;
//   Eigen2x2(matrix, real, imag, &size, &smallsub);
//   
//   while (size>0) {
//     if (size==sizeprev) {
//       for (m=(size-3);m>=smallsub;m--) {
//         p = ( (matrix[size-1][size-1]-matrix[m][m])*(matrix[size-2][size-2]-matrix[m][m]) \
//             - matrix[size-2][size-1]*matrix[size-1][size-2] )/matrix[m+1][m] + \
//               matrix[m][m+1];
//         q = matrix[m+1][m+1] - matrix[m][m] - (matrix[size-1][size-1]-matrix[m][m]) - \
//             (matrix[size-2][size-2]-matrix[m][m]);
//         r = matrix[m+2][m+1];      
//       }
//     } else { 
//       Eigen2x2(matrix,real,imag,&size);
//     }  
//   }  
//   
// }

void ElmHess(double **a, int n)
/*Reduction to Hessenberg by elimination. On output, Hess. matrix is in elements a[i][j] with i <= j+1. Elements with i > j+1 are to be thought of as zero but are returned with random values */
{
  int m, j, i, q, p;
  double y, x;

  for (m = 1; m < n-1; m++) {               // m = r+1 (from text)
    x = 0.0;
    i = m;
    for (j = m; j <= n-1; j++) {            // finds the pivot
      if (fabs(a[j][m-1]) > fabs(x)) {
        x = a[j][m-1];
        i = j;
      }
    }
    if (i != m) {                         //interchange rows and columns
      for (j = m-1; j <= n-1; j++) SWAP(a[i][j],a[m][j]);
      for (j = 0; j <= n-1; j++) SWAP(a[j][i],a[j][m]);
    }
    if (x) {                              //elimination
      for (i = m+1; i <= n-1; i++) {
        if ((y = a[i][m-1]) != 0.0) {
          y /= x;
          a[i][m-1] = y;
          for (j = m; j <= n-1; j++)
            a[i][j] -= y*a[m][j];
          for (j = 0; j <= n-1; j++)
            a[j][m] += y*a[j][i];
        }
      }
    }
  }
  for (q = 2; q <= n-1; q++) {
    for (p = 0; p <= q - 2; p++) a[q][p] = 0.0;
  }
}


void BalanceM(double **a, int n)
/*Given a matrix a[0..n-1][0..n-1], this routine replaces it by a balanced matrix with identical eigenvalues. */
{
  int last, j, i;
  double s, r, g, f, c, sqrdx;

  sqrdx = RADIX*RADIX;
  last = 0;
  while (last == 0) {
    last = 1;

    for (i = 0; i <= n-1; i++) {
      r = c = 0.0;
      for (j = 0; j <= n-1; j++)
    if (j != i) {
      c += fabs(a[j][i]);   //norm of off-diag elements in column i
      r += fabs(a[i][j]);   //norm of off-diag elements in row i
    }

      if (c && r) {             //if c and r are nonzero
    g = r/RADIX;
    f = 1.0;
    s = c+r;
    while (c < g) {
      f *= RADIX;
      c *= sqrdx;
    }

    g = r*RADIX;
    while (c > g) {
      f /= RADIX;
      c /= sqrdx;
    }

    if ((c + r)/f < 0.95*s) {
      last = 0;
      g = 1.0/f;
      for (j = 0; j <= n-1; j++) a[i][j] *= g;
      for (j = 0; j <= n-1; j++) a[j][i] *= f;
    }
      }
    }
  }
}

void RowSwap(double **matrix, int size, int i, int j) {
  /* swap the ith and jth rows in matrix of size size*/
  int k;
  double dummy;
  
  for (k=0;k<size;k++) {
    dummy = matrix[i][k];
    matrix[i][k] = matrix[j][k];
    matrix[j][k] = dummy;
  }
}  

void ColSwap(double **matrix, int size, int i, int j) {
  /* swap the ith and jth rows in matrix of size size*/
  int k;
  double dummy;
  
  for (k=0;k<size;k++) {
    dummy = matrix[k][i];
    matrix[k][i] = matrix[k][j];
    matrix[k][j] = dummy;
  }
}  

void HessReduce(double **a, int size) {
  int r, rp, rmax, i, j;
  double max, n;
  
  for (r=0;r<size;r++) { 
    max = 0;
    for (rp=r+1;rp<size;rp++) {
      if (fabs(a[rp][r])>max) {
        max = fabs(a[rp][r]);
        rmax = rp;
      }
    }
    if (max) {
      RowSwap(a,size,rmax,r+1);
      ColSwap(a,size,rmax,r+1);
      
      for (i=r+2;i<size;i++) {
        n = a[i][r]/a[r+1][r];
        
        for (j=0;j<size;j++) {
          a[i][j] -= n*a[r+1][j];
        }
        for (j=0;j<size;j++) {
          a[j][r+1] += n*a[j][i];
        }
      }
    }
  }
}

void BalanceMatrix(double **a, int size) {
  int i, j, end = 0;
  double rownorm, colnorm, factor;
  
  while (end == 0) {
    for (i=0;i<size;i++) {
      rownorm = 0.0;
      colnorm = 0.0;
      for (j=0;j<size;j++) {
        rownorm += pow(a[i][j],2);
        colnorm += pow(a[j][i],2);
      }
      rownorm = pow(rownorm,0.5);
      colnorm = pow(colnorm,0.5);
    
      factor = sqrt(rownorm/colnorm);
    
      for (j=0;j<size;j++) {
        a[i][j] /= factor;
        a[j][i] *= factor;
      }
    
      if ((pow(factor*colnorm,2)+pow(rownorm/factor,2)) > 0.95*(pow(colnorm,2)+pow(rownorm,2))) {
        end = 1;
      }
    }
  }
}    

void ludcmp(double **a, int n, int *indx, float *d)
/*Given a matrix a[0...n-1][0..n-1] this routine replaces by the LU decomp of a rowwise permutatio of itself. a and n are input. a is output, arranged with U as the upper triangular components and L as the lower triangular components BELOW the diagonal (diagonal elements of L are all = 1). indx[0..n-1] is an output vector that records the row permutation effected by partial pivoting; d is output as +/- 1 depending on whether the number of row interchanges was even(+) or odd(-). Used in combination with lubksb to solve linear eqns.*/
{
  int i, imax, j, k;
  double big, dum, sum, temp;
  double *vv;                       //stores implicit scaling of each row

  vv = malloc(n*sizeof(double));
  *d = 1.0;
  for (i = 0; i <= n-1; i++) {
    big = 0.0;
    for (j = 0; j <= n-1; j++)
      if ((temp = fabs(a[i][j])) > big) big = temp;
    if (big == 0.0) {
      fprintf(stderr,"Singular matrix in routine ludcmp");
      exit(EXIT_INPUT);
    }
    vv[i] = 1.0/big;
  }
  for (j = 0; j <= n-1; j++) {
    for (i = 0; i < j; i++) {
      sum = a[i][j];
      for (k = 0; k < i; k++) sum -= a[i][k]*a[k][j];
      a[i][j] = sum;
    }
    big = 0.0;
    for (i = j; i <= n-1; i++) {
      sum = a[i][j];
      for (k = 0; k < j; k++)
        sum -= a[i][k]*a[k][j];
      a[i][j] = sum;
      dum = vv[i]*fabs(sum);
      if (dum >= big) {
        big = dum;
        imax = i;
      }
    }
    if (j != imax) {
      for (k = 0; k <= n-1; k++) {
        dum = a[imax][k];
        a[imax][k] = a[j][k];
        a[j][k] = dum;
      }
      *d = -(*d);
      vv[imax] = vv[j];
    }
    indx[j] = imax;
    if (a[j][j] == 0.0) a[j][j] = TEENY; //if pivot = 0, matrix is singular
    if (j != n) {
      dum = 1.0/(a[j][j]);
      for (i = j+1; i <= n-1; i++) a[i][j] *= dum;
    }
  }
  free(vv);
}

void LUDecomp(double **amat, double **copy, double *scale, int *rowswap, int size) {

  double sumk, scaletmp, dummy;
  int i, j, k, swapi;
  
  for (i=0;i<size;i++) {
    scale[i] = 0.0;
    for (j=0;j<size;j++) {
      if (fabs(amat[i][j]) > scale[i]) {
        scale[i] = fabs(amat[i][j]);
      }
    }
    if (scale[i] == 0.0) {
      fprintf(stderr,"Singular matrix in routine LUDecomp");
      exit(EXIT_INPUT);
    }
    for (j=0;j<size;j++) {
      copy[i][j] = amat[i][j];
    }
    scale[i] = 1.0/scale[i];
  }
  
  for (j=0;j<size;j++) {
    scaletmp = 0.0;
    swapi = j;

    for (i=0;i<size;i++) {
      sumk = 0.0;
      if (i<j) {
        for (k=0;k<i;k++) {
          sumk += copy[i][k]*copy[k][j];
        }
      } else {
        for (k=0;k<j;k++) {
          sumk += copy[i][k]*copy[k][j];
        }
      } 
      copy[i][j] -= sumk;
      
      if (i>=j) {
        if (fabs(scale[i]*copy[i][j]) >= scaletmp) {
          scaletmp = fabs(scale[i]*copy[i][j]);
          swapi = i;
        }
      }
    }  
    if (swapi != j) {
          RowSwap(copy,size,swapi,j);
          dummy=scale[j];
          scale[j]=scale[swapi];
          scale[swapi]=dummy;
    }
    
    if (copy[j][j] == 0) {
      copy[j][j] = TEENY;
    }
    for (i=j+1;i<size;i++) { 
      copy[i][j] /= copy[j][j];
    }
    rowswap[j] = swapi;
  }
}

void lubksb(double **a, int n, int *indx, double b[])
/*Solves set of n linear eqns A*X = B. a[0..n-1][0..n-1] is input as the LU decompostion from ludcmp. indx[0..n-1] is imput as the permutation vector from ludcmp. b[0..n-1] is input as the RHS vector B and returns with the soln vector X. a, n, and indx are not modified.*/
{
  int i, ii = 0, ip, j;
  double sum;

  for (i = 0; i <= n-1; i++) {
    ip = indx[i];
    sum = b[ip];
    b[ip] = b[i];
    if (ii)
      for (j = ii-1; j <= i-1; j++) sum -= a[i][j]*b[j];
    else if (sum) ii = i+1;
    b[i] = sum;
  }
  for (i = n-1; i >= 0; i--) {
    sum = b[i];
    for (j = i+1; j <= n-1; j++) sum -= a[i][j]*b[j];
    b[i] = sum/a[i][i] ;
  }
}  

void LUSolve(double **lumat, double *soln, int *swap, int size) {
  int i, j;
  double dummy, sumj;
  
  for (i=0;i<size;i++) {
    if (swap[i] != i) {
      dummy = soln[i];
      soln[i] = soln[swap[i]];
      soln[swap[i]] = dummy;
    }
  }
  
  for (i=0;i<size;i++) {
    sumj = 0.0;
    for (j=0;j<i;j++) {
      sumj += lumat[i][j]*soln[j];
    }
    soln[i] = soln[i] - sumj; //diagonals of L matrix are all = 1, so division is unnecessary
  }
  
  for (i=(size-1);i>=0;i--) {
    sumj = 0.0;
    for (j=i+1;j<size;j++) {
      sumj += lumat[i][j]*soln[j];
    }
    soln[i] = (soln[i] - sumj)/lumat[i][i];
  }
}

void FindEigenVecEcc(SYSTEM *system, int count, int pls) {

  int jj, i, iter = 5;  // iter = # of iterations of inverse routine
  float d;                     // parity for LU factorization
  double mag;                 // normalization for eigenvector
  
  // Subtracts eigenvalue from diagonal elements
  for (jj = 0; jj <= (pls-1); jj++) {
    system->A[jj][jj] -= system->dmEigenValEcc[0][count-1];
    system->Asoln[jj] = 1.0 / sqrt(pls);
    for (i=0;i<pls;i++) {
      system->Acopy[jj][i] = system->A[jj][i];
    }
  }

  LUDecomp(system->A, system->Acopy, system->scale, system->rowswap, pls);

  // Finds eigenvectors by inverse iteration, normalizing at each step
  for (i = 1; i <= iter; i++) {  
//     lubksb(A, pls, swap, soln);
    LUSolve(system->Acopy,system->Asoln,system->rowswap,pls);

    mag = 0.0;
    for (jj = 0; jj <= (pls-1); jj++) {
      mag += system->Asoln[jj]*system->Asoln[jj];
    }
    
    for (jj = 0; jj <= (pls-1); jj++) {
      system->Asoln[jj] /= sqrt(mag);
    }
  }
}

void FindEigenVecInc(SYSTEM *system, int count, int pls) {

  int jj, i, iter = 5;  // iter = # of iterations of inverse routine
  float d;                     // parity for LU factorization
  double mag;                 // normalization for eigenvector
  
  // Subtracts eigenvalue from diagonal elements
  for (jj = 0; jj <= (pls-1); jj++) {
    system->B[jj][jj] -= system->dmEigenValInc[0][count-1];
    system->Bsoln[jj] = 1.0 / sqrt(pls);
    for (i=0;i<pls;i++) {
      system->Acopy[jj][i] = system->B[jj][i];
    }
  }

  LUDecomp(system->B, system->Acopy, system->scale, system->rowswap, pls);

  // Finds eigenvectors by inverse iteration, normalizing at each step
  for (i = 1; i <= iter; i++) {  
//     lubksb(A, pls, swap, soln);
    LUSolve(system->Acopy,system->Bsoln,system->rowswap,pls);

    mag = 0.0;
    for (jj = 0; jj <= (pls-1); jj++) {
      mag += system->Bsoln[jj]*system->Bsoln[jj];
    }
    
    for (jj = 0; jj <= (pls-1); jj++) {
      system->Bsoln[jj] /= sqrt(mag);
    }
  }
}  
  
void SolveEigenVal(BODY *body, EVOLVE *evolve, SYSTEM *system) {
  /* This solves the eigenvalue problem and provides an explicit solution 
       to the orbital evolution */
    double parity;
    int j, k, count, i,iBody;
    
    /*First pass through calculates matrices and eigenvalues. Each subsequent pass redefines the matrices
      because they are destroyed by eigenvalue routines, then calculates eigenvectors. */
    for (count=0;count<(evolve->iNumBodies);count++) {
      /* Calculate the initial matrix */
      for (j=0;j<(evolve->iNumBodies-1);j++) {
        system->A[j][j] = 0.0;
        system->B[j][j] = 0.0;
        for (k=0;k<(evolve->iNumBodies-1);k++) {
          if (j!=k) {
            system->A[j][j] += ABmatrix(body,1,j+1,k+1);
            system->A[j][k] = -ABmatrix(body,2,j+1,k+1);            
            system->B[j][j] += -ABmatrix(body,1,j+1,k+1);
            system->B[j][k] = ABmatrix(body,1,j+1,k+1);
          }
        }
        if (body[j+1].bGRCorr)
          system->A[j][j] += GRCorrMatrix(body,j+1,j+1);
      }
      
      if (count==0) {
        BalanceMatrix(system->A, (evolve->iNumBodies-1)); //balance matrix
        HessReduce(system->A, (evolve->iNumBodies-1));  //reduce to upper Hess form
                
        BalanceMatrix(system->B, (evolve->iNumBodies-1)); //balance matrix
        HessReduce(system->B, (evolve->iNumBodies-1));  //reduce to upper Hess form
       //  BalanceM(B, (evolve->iNumBodies-1)); //balance matrix
//         ElmHess(B, (evolve->iNumBodies-1));  //reduce to upper Hess form
        
        HessEigen(system->A, (evolve->iNumBodies-1), system->dmEigenValEcc[0], system->dmEigenValEcc[1]);
        HessEigen(system->B, (evolve->iNumBodies-1), system->dmEigenValInc[0], system->dmEigenValInc[1]);
        // for (i=1;i<(evolve->iNumBodies-1);i++) {
//           printf("s%d = %f\n",i,system->dmEigenValInc[0][i]*YEARSEC);
//         }
      } else {
        FindEigenVecEcc(system,count,(evolve->iNumBodies-1));
        FindEigenVecInc(system,count,(evolve->iNumBodies-1));
      
        for (j=0;j<(evolve->iNumBodies-1);j++) {
          system->dmEigenVecEcc[j][count-1] = system->Asoln[j];
          system->dmEigenVecInc[j][count-1] = system->Bsoln[j];
        }
      }
    }
}

void ScaleEigenVec(BODY *body, EVOLVE *evolve, SYSTEM *system) {
  int i, j, count;
  float parity;  
  
  for (i=0;i<(evolve->iNumBodies-1);i++) {
        system->h0[i] = body[i+1].dHecc;
        system->k0[i] = body[i+1].dKecc;
        system->p0[i] = body[i+1].dPinc;
        system->q0[i] = body[i+1].dQinc;
  
        for (j=0;j<(evolve->iNumBodies-1);j++) {
          system->etmp[i][j] = system->dmEigenVecEcc[i][j];
          system->itmp[i][j] = system->dmEigenVecInc[i][j];
        }
  }
//   ludcmp(etmp,(evolve->iNumBodies-1),rowswap,&parity);
  // lubksb(etmp,(evolve->iNumBodies-1),rowswap,h0);
//   lubksb(etmp,(evolve->iNumBodies-1),rowswap,k0);

  LUDecomp(system->dmEigenVecEcc,system->etmp,system->scale,system->rowswap,(evolve->iNumBodies-1));
  LUSolve(system->etmp,system->h0,system->rowswap,(evolve->iNumBodies-1));
  LUSolve(system->etmp,system->k0,system->rowswap,(evolve->iNumBodies-1));

  
  //ludcmp(itmp,(evolve->iNumBodies-1),rowswap,&parity);
//   lubksb(itmp,(evolve->iNumBodies-1),rowswap,p0);
//   lubksb(itmp,(evolve->iNumBodies-1),rowswap,q0);
 
  LUDecomp(system->dmEigenVecInc,system->itmp,system->scale,system->rowswap,(evolve->iNumBodies-1));

  LUSolve(system->itmp,system->p0,system->rowswap,(evolve->iNumBodies-1));
  LUSolve(system->itmp,system->q0,system->rowswap,(evolve->iNumBodies-1));
      
 
  for (i=0;i<(evolve->iNumBodies-1);i++) {
    system->S[i] = sqrt(pow(system->h0[i],2) + pow(system->k0[i],2));
    system->T[i] = sqrt(pow(system->p0[i],2) + pow(system->q0[i],2));
    
    for (j=0;j<(evolve->iNumBodies-1);j++) {
      system->dmEigenVecEcc[j][i] *= system->S[i];
      system->dmEigenVecInc[j][i] *= system->T[i];
    }
    
    system->dmEigenPhase[0][i] = atan2(system->h0[i],system->k0[i]);
    system->dmEigenPhase[1][i] = atan2(system->p0[i],system->q0[i]);
  }
}

void RecalcLaplace(BODY *body,EVOLVE *evolve,SYSTEM *system,int iVerbose) {
  double alpha1, dalpha;
  int j, iBody, jBody, done=0;
  
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
	      if (iVerbose > VERBPROG)
		printf("Laplace function %d recalculated for bodies (%d, %d) at %f years\n",j+1,iBody,jBody,evolve->dTime/YEARSEC);
        }
      }
    }
  }
}

void RecalcEigenVals(BODY *body, EVOLVE *evolve, SYSTEM *system) {
  int iBody, jBody, j, done = 0;
  double alpha1, dalpha=-1, dalphaTmp;

  for (iBody=1;iBody<evolve->iNumBodies-1;iBody++) {
    for (jBody=iBody+1;jBody<evolve->iNumBodies;jBody++) {
      if (body[iBody].dSemi < body[jBody].dSemi) {
        alpha1 = body[iBody].dSemi/body[jBody].dSemi;
      } else if (body[iBody].dSemi > body[jBody].dSemi) {
        alpha1 = body[jBody].dSemi/body[iBody].dSemi;
      }
      for (j=0;j<2;j++) {
        dalphaTmp = fabs((alpha1 - system->dmAlpha0[system->imLaplaceN[iBody][jBody]][0])*system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][j]);
        if (dalphaTmp > dalpha) {
          dalpha = dalphaTmp;
        }
      }
    }
  }
  
  if (dalpha > system->dDfcrit) {
    SolveEigenVal(body,evolve,system);
    ScaleEigenVec(body,evolve,system);
    for (iBody=1;iBody<evolve->iNumBodies-1;iBody++) {
      for (jBody=iBody+1;jBody<evolve->iNumBodies;jBody++) {
        for (j=0;j<2;j++) {
          system->dmLaplaceD[system->imLaplaceN[iBody][jBody]][j] = fdDerivLaplaceCoeff(1,alpha1,j+1,1.5);
          system->dmAlpha0[system->imLaplaceN[iBody][jBody]][j] = alpha1;
        }
      }
    }
//     printf("Eigenvalues recalculated at %f years\n",evolve->dTime/YEARSEC);
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

// XXX There is already a similar function to this in body.c
double signf(double value) {
  if (value > 0) return 1.;
  if (value < 0) return -1.;
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

  free(xcom);
  free(vcom);
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

// XXX Functions like cross and angularmom belong in a general file, like system.c.
void cross(double *a, double *b, double *c) {
  c[0] = a[1]*b[2] - b[1]*a[2];
  c[1] = a[2]*b[0] - b[2]*a[0];
  c[2] = a[0]*b[1] - b[0]*a[1];
}

void angularmom(BODY *body, double *AngMom, int iNumBodies) {
  double *rxptmp;
  int i, iBody;
  
  rxptmp = malloc(3*sizeof(double));

  osc2cart(body, iNumBodies);
  astro2bary(body, iNumBodies);

  for (i=0;i<3;i++)
    AngMom[i]=0;
  
  for (iBody=0;iBody<iNumBodies;iBody++) {
    cross(body[iBody].dCartPos, body[iBody].dCartVel, rxptmp);
    for (i=0;i<3;i++) {
      // XXX Why divide by MSUN and not stellar mass?
      AngMom[i] += body[iBody].dMass/MSUN*rxptmp[i];
    }
  }
  free(rxptmp);
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

  free(xtmp);
  free(vtmp);
}

double normv(double *vector) {
  return sqrt(pow(vector[0],2)+pow(vector[1],2)+pow(vector[2],2));
}

void cart2osc(BODY *body, int iNumBodies) {
  int iBody;
  double r, vsq, rdot, mu, *h, hsq, sinwf, coswf, sinf, cosf, sinw, cosw, cosE, f;

  h = malloc(3*sizeof(double));

  for (iBody=1;iBody<iNumBodies;iBody++) {
    r = normv(body[iBody].dCartPos);
    vsq = pow(normv(body[iBody].dCartVel),2);
    rdot = (body[iBody].dCartPos[0]*body[iBody].dCartVel[0]+body[iBody].dCartPos[1]*body[iBody].dCartVel[1]+\
            body[iBody].dCartPos[2]*body[iBody].dCartVel[2])/r;
    mu = pow(KGAUSS,2)*(body[0].dMass+body[iBody].dMass)/MSUN;
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
      body[iBody].dArgP = atan2(sinw,cosw);
      body[iBody].dLongP = atan2(sinw, cosw) + body[iBody].dLongA;
      if (body[iBody].dLongP >= 2.*PI) {
        body[iBody].dLongP -= 2.*PI;
      } else if (body[iBody].dLongP < 0.0) {
        body[iBody].dLongP += 2.*PI;
      }
      if (body[iBody].dArgP >= 2.*PI) {
        body[iBody].dArgP -= 2.*PI;
      } else if (body[iBody].dArgP < 0.0) {
        body[iBody].dArgP += 2.*PI;
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
  free(h);
}


void inv_plane(BODY *body, SYSTEM *system, int iNumBodies) {
  int iBody;
  // double *AngMom;
//   AngMom = malloc(3*sizeof(double));
  double AngMom[3] = {0.0,0.0,0.0}; /* locally allocates this memory */
  
  /* Loop below calculates true anomaly at equinox for planets with DistRot enabled. 
     This angle is invariant under rotations. */
  for (iBody=1;iBody<iNumBodies;iBody++) {
    if (body[iBody].bDistRot) { // XXX No mention of other modules is allowed!! 
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
  free(AngMom);
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
  return 1./4 * ( (-6. + 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ)) \
   + 4.*pow(dAxRatio,2) * (1. - iIndexJ) * fdDerivLaplaceCoeff(1,B(iIndexJ))    \
   + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ)) );
}

double fdDSemiF24Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (-6. + 4.*iIndexJ) * iIndexJ * fdLaplaceCoeff(B(iIndexJ)) \
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

//-------------------DistOrb's equations in h k p q (4th order direct integration RD4)--------------------
double fdDistOrbRD4DhDt(BODY *body, SYSTEM *system, int *iaBody) {
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

double fdDistOrbRD4DkDt(BODY *body, SYSTEM *system, int *iaBody) {
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

double fdDistOrbRD4DpDt(BODY *body, SYSTEM *system, int *iaBody) {
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


double fdDistOrbRD4DqDt(BODY *body, SYSTEM *system, int *iaBody) {
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

//-------------------DistOrb's equations in h k p q (2nd order Laplace-Lagrange LL2)--------------------
double fdDistOrbLL2Hecc(BODY *body, SYSTEM *system, int *iaBody) {
  return system->dmEigenVecEcc[iaBody[0]-1][iaBody[1]-1]*sin(system->dmEigenValEcc[0][iaBody[1]-1]/YEARSEC*body[iaBody[0]].dAge+system->dmEigenPhase[0][iaBody[1]-1]);
}

double fdDistOrbLL2Kecc(BODY *body, SYSTEM *system, int *iaBody) {
  return system->dmEigenVecEcc[iaBody[0]-1][iaBody[1]-1]*cos(system->dmEigenValEcc[0][iaBody[1]-1]/YEARSEC*body[iaBody[0]].dAge+system->dmEigenPhase[0][iaBody[1]-1]);
}

double fdDistOrbLL2Pinc(BODY *body, SYSTEM *system, int *iaBody) {
  return system->dmEigenVecInc[iaBody[0]-1][iaBody[1]-1]*sin(system->dmEigenValInc[0][iaBody[1]-1]/YEARSEC*body[iaBody[0]].dAge+system->dmEigenPhase[1][iaBody[1]-1]);
}

double fdDistOrbLL2Qinc(BODY *body, SYSTEM *system, int *iaBody) {
  return system->dmEigenVecInc[iaBody[0]-1][iaBody[1]-1]*cos(system->dmEigenValInc[0][iaBody[1]-1]/YEARSEC*body[iaBody[0]].dAge+system->dmEigenPhase[1][iaBody[1]-1]);
}

double fdDistOrbLL2DhDt(BODY *body, SYSTEM *system, int *iaBody) {
  return system->dmEigenVecEcc[iaBody[0]-1][iaBody[1]-1]*system->dmEigenValEcc[0][iaBody[1]-1]/YEARSEC*cos(system->dmEigenValEcc[0][iaBody[1]-1]/YEARSEC*body[iaBody[0]].dAge+system->dmEigenPhase[0][iaBody[1]-1]);
}

double fdDistOrbLL2DkDt(BODY *body, SYSTEM *system, int *iaBody) {
  return -system->dmEigenVecEcc[iaBody[0]-1][iaBody[1]-1]*system->dmEigenValEcc[0][iaBody[1]-1]/YEARSEC*sin(system->dmEigenValEcc[0][iaBody[1]-1]/YEARSEC*body[iaBody[0]].dAge+system->dmEigenPhase[0][iaBody[1]-1]);
}

double fdDistOrbLL2DpDt(BODY *body, SYSTEM *system, int *iaBody) {
  /* Derivatives used by DistRot */
  return system->dmEigenVecInc[iaBody[0]-1][iaBody[1]-1]*system->dmEigenValInc[0][iaBody[1]-1]/YEARSEC*cos(system->dmEigenValInc[0][iaBody[1]-1]/YEARSEC*body[iaBody[0]].dAge+system->dmEigenPhase[1][iaBody[1]-1]);  
}

double fdDistOrbLL2DqDt(BODY *body, SYSTEM *system, int *iaBody) {
  /* Derivatives used by DistRot */
  return -system->dmEigenVecInc[iaBody[0]-1][iaBody[1]-1]*system->dmEigenValInc[0][iaBody[1]-1]/YEARSEC*sin(system->dmEigenValInc[0][iaBody[1]-1]/YEARSEC*body[iaBody[0]].dAge+system->dmEigenPhase[1][iaBody[1]-1]);    
}
