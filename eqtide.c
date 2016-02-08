/********************** EQTIDE.C **********************/
/*
 * Rory Barnes, Wed May 29 14:56:21 PDT 2014
 *
 * Subroutines that control the integration of the tidal
 * model. Also includes subroutines that switch between
 * the two models.
*/ 
//testing a change


#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void InitializeControlEqtide(CONTROL *control) {

  control->Evolve.bForceEqSpin=malloc(control->Evolve.iNumBodies*sizeof(int));
  control->Evolve.dMaxLockDiff=malloc(control->Evolve.iNumBodies*sizeof(double));
  control->Evolve.dSyncEcc=malloc(control->Evolve.iNumBodies*sizeof(double));
  control->Evolve.bFixOrbit=malloc(control->Evolve.iNumBodies*sizeof(int));
}

void InitializeModuleEqtide(CONTROL *control,MODULE *module) {
  /* Anything here? */
}

/* All the auxiliary properties for EQTIDE calculations need to be included
   in this subroutine! */

void BodyCopyEqtide(BODY *dest,BODY *src,int iTideModel,int iBody) {
  int iIndex,iPert;

  dest[iBody].iTidePerts = src[iBody].iTidePerts;
  dest[iBody].dImK2 = src[iBody].dImK2;
  dest[iBody].dK2 = src[iBody].dK2;
  dest[iBody].dObliquity = src[iBody].dObliquity;
  dest[iBody].dPrecA = src[iBody].dPrecA;

  if (iBody > 0) {
    dest[iBody].dEcc = src[iBody].dEcc;
    dest[iBody].dEccSq = src[iBody].dEccSq;
    dest[iBody].dLongP = src[iBody].dLongP;
    dest[iBody].dMeanMotion = src[iBody].dMeanMotion;
    dest[iBody].dDeccDtEqtide = src[iBody].dDeccDtEqtide;
  }

  for (iPert=0;iPert<src[iBody].iTidePerts;iPert++) {
    dest[iBody].dTidalZ[iPert] = src[iBody].dTidalZ[iPert];
    dest[iBody].dTidalChi[iPert] = src[iBody].dTidalChi[iPert];
    dest[iBody].iaTidePerts[iPert] = src[iBody].iaTidePerts[iPert];
    dest[iBody].daDoblDtEqtide[iPert] = src[iBody].daDoblDtEqtide[iPert];

    if (iTideModel == CPL) {
      dest[iBody].dTidalQ = src[iBody].dTidalQ;
      for (iIndex=0;iIndex<10;iIndex++)
          dest[iBody].iTidalEpsilon[iPert][iIndex] = src[iBody].iTidalEpsilon[iPert][iIndex];
    }    
    if (iTideModel == CTL) {
      dest[iBody].dTidalTau = src[iBody].dTidalTau;
      dest[iBody].dTidalBeta = src[iBody].dTidalBeta;
      for (iIndex=0;iIndex<5;iIndex++)
          dest[iBody].dTidalF[iPert][iIndex] = src[iBody].dTidalF[iPert][iIndex];
    }
  }
}

void InitializeBodyEqtide(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
  body[iBody].iaTidePerts = malloc(body[iBody].iTidePerts*sizeof(int));
  body[iBody].daDoblDtEqtide = malloc(control->Evolve.iNumBodies*sizeof(double));
}

void InitializeUpdateTmpBodyEqtide(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
  int iBodyPert;

  control->Evolve.tmpBody[iBody].dTidalChi = malloc(control->Evolve.iNumBodies*sizeof(double));
  control->Evolve.tmpBody[iBody].dTidalZ = malloc(control->Evolve.iNumBodies*sizeof(double));
  
  control->Evolve.tmpBody[iBody].iaTidePerts = malloc(body[iBody].iTidePerts*sizeof(int));
  control->Evolve.tmpBody[iBody].daDoblDtEqtide = malloc(control->Evolve.iNumBodies*sizeof(double));

  if (control->Evolve.iEqtideModel == CPL) {
    control->Evolve.tmpBody[iBody].iTidalEpsilon = malloc(control->Evolve.iNumBodies*sizeof(int*));
    for (iBodyPert=0;iBodyPert<control->Evolve.iNumBodies;iBodyPert++)
      control->Evolve.tmpBody[iBody].iTidalEpsilon[iBodyPert] = malloc(10*sizeof(int));
  }

  if (control->Evolve.iEqtideModel == CTL) {
    control->Evolve.tmpBody[iBody].dTidalF = malloc(control->Evolve.iNumBodies*sizeof(double*));
    control->Evolve.tmpBody[iBody].dTidalBeta = malloc(control->Evolve.iNumBodies*sizeof(double));
    for (iBodyPert=0;iBodyPert<control->Evolve.iNumBodies;iBodyPert++)
      control->Evolve.tmpBody[iBody].dTidalF[iBodyPert] = malloc(5*sizeof(double));
  }

}


/**************** EQTIDE options ********************/

/* Discrete Rotation */

void ReadDiscreteRot(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Evolve.bDiscreteRot = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    /* Set to default */
    AssignDefaultInt(options,&control->Evolve.bDiscreteRot,files->iNumInputs);
}

/* Double Synchronous */

void ReadHaltDblSync(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Halt[iFile-1].bDblSync = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultInt(options,&control->Halt[iFile-1].bDblSync,files->iNumInputs); 
  }
}

/* Fix Orbits -- for testing purposes */

void ReadFixOrbit(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* Cannot exist in primary input file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,options->iLine[iFile],control->Io.iVerbose);
    control->Evolve.bFixOrbit[iFile-1] = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      /* Set to default */
      AssignDefaultInt(options,&control->Evolve.bFixOrbit[iFile-1],files->iNumInputs);
}

/* Force Equilibrium Spin Rate? */

void ReadForceEqSpin(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* Cannot exist in primary input file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,options->iLine[iFile],control->Io.iVerbose);
    control->Evolve.bForceEqSpin[iFile-1] = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      /* Set to default */
      AssignDefaultInt(options,&control->Evolve.bForceEqSpin[iFile-1],files->iNumInputs);
}

/* Primary Tide-locked */

void ReadHaltTideLock(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    control->Halt[iFile-1].bTideLock = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      control->Halt[iFile-1].bTideLock = atoi(options->cDefault);
}

/* Halt when Synchronous Rotation */

void ReadHaltSyncRot(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Halt[iFile-1].bSync = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      control->Halt[iFile-1].bSync = atoi(options->cDefault);
  }
}

/* k_2 - Love Number of degree 2 */

void ReadK2(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    body[iFile-1].dK2 = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dK2 = options->dDefault;
}

/* Maximum allowable offset between primary's spin period and its
   equilibrium spin period. */

void ReadMaxLockDiff(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: %s must be > 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    control->Evolve.dMaxLockDiff[iFile-1] = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      control->Evolve.dMaxLockDiff[iFile-1] = options->dDefault;
  }
}

/* Maximum Eccentricity for Synchronous Rotation */

void ReadSyncEcc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile-1].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile-1].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0 || dTmp > 1) {
      if (control->Io.iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: %s must be in the range [0,1].\n",options->cName);
        LineExit(files->Infile[iFile-1].cIn,lTmp);
    }
    control->Evolve.dSyncEcc[iFile-1] = dTmp;
    UpdateFoundOption(&files->Infile[iFile-1],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      control->Evolve.dSyncEcc[iFile-1] = options->dDefault;
  }
}    

/* Tidal Q */

void ReadTidalQ(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }

    body[iFile-1].dTidalQ = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      body[iFile-1].dTidalQ = options->dDefault;
  }
}  

/* Time lag */

void ReadTidalTau(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) 
      body[iFile-1].dTidalTau = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      /* Convert timestep to cgs */
      body[iFile-1].dTidalTau = dTmp*fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      body[iFile-1].dTidalTau = options->dDefault;
  }
}


/* Tidal Model */

void ReadTideModel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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
    if (!memcmp(sLower(cTmp),"p2",2)) {
      control->Evolve.iEqtideModel = CPL;
    } else if (!memcmp(sLower(cTmp),"t8",2)) {
      control->Evolve.iEqtideModel = CTL;
    } else {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are p2 or t8.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);  
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  }
}

void ReadTidePerts(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int iBody,iNumIndices=0,iNumLines=0;
  int *lTmp;
  char saTmp[MAXARRAY][OPTLEN];

  lTmp=malloc(MAXLINES*sizeof(int));

  AddOptionStringArray(files->Infile[iFile].cIn,options->cName,saTmp,&iNumIndices,&iNumLines,lTmp,control->Io.iVerbose);
  if (lTmp[0] >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp[0],control->Io.iVerbose);
    /* Now do some initializing */
    body[iFile-1].iTidePerts=iNumIndices;
    for (iBody=0;iBody<iNumIndices;iBody++) {
      memset(body[iFile-1].saTidePerts[iBody],'\0',NAMELEN);
      strcpy(body[iFile-1].saTidePerts[iBody],saTmp[iBody]);
    }
    UpdateFoundOptionMulti(&files->Infile[iFile],options,lTmp,iNumLines,iFile);
  } else 
    if (iFile > 0) {
      body[iFile-1].iTidePerts=0;
    }
  free(lTmp);
}

void InitializeOptionsEqtide(OPTIONS *options,fnReadOption fnRead[]){

  sprintf(options[OPT_DISCRETEROT].cName,"bDiscreteRot");
  sprintf(options[OPT_DISCRETEROT].cDescr,"Use Discrete Rotation Model (Phase lag only)");
  sprintf(options[OPT_DISCRETEROT].cDefault,"1");
  options[OPT_DISCRETEROT].iType = 0;
  fnRead[OPT_DISCRETEROT] = &ReadDiscreteRot;
  
  sprintf(options[OPT_FIXORBIT].cName,"bFixOrbit");
  sprintf(options[OPT_FIXORBIT].cDescr,"Fix Orbital Elements?");
  sprintf(options[OPT_FIXORBIT].cDefault,"0");
  options[OPT_FIXORBIT].iType = 0;
  options[OPT_FIXORBIT].iMultiFile = 1;
  fnRead[OPT_FIXORBIT] = &ReadFixOrbit;
  
  sprintf(options[OPT_FORCEEQSPIN].cName,"bForceEqSpin");
  sprintf(options[OPT_FORCEEQSPIN].cDescr,"Force Spin Rate to Equilibrium?");
  sprintf(options[OPT_FORCEEQSPIN].cDefault,"0");
  options[OPT_FORCEEQSPIN].iType = 0;
  options[OPT_FORCEEQSPIN].iMultiFile = 1;
  fnRead[OPT_FORCEEQSPIN] = &ReadForceEqSpin;
  
  sprintf(options[OPT_HALTDBLSYNC].cName,"bHaltDblSync");
  sprintf(options[OPT_HALTDBLSYNC].cDescr,"Halt at Double Synchronous State?");
  sprintf(options[OPT_HALTDBLSYNC].cDefault,"0");
  options[OPT_HALTDBLSYNC].iType = 0;
  fnRead[OPT_HALTDBLSYNC] = &ReadHaltDblSync; 
  
  sprintf(options[OPT_HALTTIDELOCK].cName,"bHaltTideLock");
  sprintf(options[OPT_HALTTIDELOCK].cDescr,"Halt if Tide-Locked?");
  sprintf(options[OPT_HALTTIDELOCK].cDefault,"0");
  options[OPT_HALTTIDELOCK].iType = 0;
  options[OPT_HALTTIDELOCK].iMultiFile = 1;
  fnRead[OPT_HALTTIDELOCK] = &ReadHaltTideLock;
  
  sprintf(options[OPT_HALTSYNCROT].cName,"bHaltSyncRot");
  sprintf(options[OPT_HALTSYNCROT].cDescr,"Halt if Secondary's rotation becomes syncrhonous?");
  sprintf(options[OPT_HALTSYNCROT].cDefault,"0");
  options[OPT_HALTSYNCROT].iType = 0;
  options[OPT_HALTSYNCROT].iMultiFile = 1;
  fnRead[OPT_HALTSYNCROT] = &ReadHaltSyncRot;
  
  sprintf(options[OPT_K2].cName,"dK2");
  sprintf(options[OPT_K2].cDescr,"Love Number of Degree 2");
  sprintf(options[OPT_K2].cDefault,"1");
  options[OPT_K2].dDefault = 1;
  options[OPT_K2].iType = 2;
  options[OPT_K2].iMultiFile = 1;
  fnRead[OPT_K2] = &ReadK2;
  
  sprintf(options[OPT_MAXLOCKDIFF].cName,"dMaxLockDiff");
  sprintf(options[OPT_MAXLOCKDIFF].cDescr,"Maximum relative difference between spin and equilibrium spin rates to force equilibrium spin rate");
  sprintf(options[OPT_MAXLOCKDIFF].cDefault,"0");
  options[OPT_MAXLOCKDIFF].dDefault = 0;
  options[OPT_MAXLOCKDIFF].iType = 2;
  options[OPT_MAXLOCKDIFF].iMultiFile = 1;
  fnRead[OPT_MAXLOCKDIFF] = &ReadMaxLockDiff;
  
  sprintf(options[OPT_SYNCECC].cName,"dSyncEcc");
  sprintf(options[OPT_SYNCECC].cDescr,"Minimum Eccentricity for Non-Synchronous Rotation");
  sprintf(options[OPT_SYNCECC].cDefault,"0");
  options[OPT_SYNCECC].dDefault = 0;
  options[OPT_SYNCECC].iType = 2;
  options[OPT_SYNCECC].iMultiFile = 1;
  fnRead[OPT_SYNCECC] = &ReadSyncEcc;
  
  sprintf(options[OPT_TIDALQ].cName,"dTidalQ");
  sprintf(options[OPT_TIDALQ].cDescr,"Tidal Quality Factor");
  sprintf(options[OPT_TIDALQ].cDefault,"1e6");
  options[OPT_TIDALQ].dDefault = 1e6;
  options[OPT_TIDALQ].iType = 2;
  options[OPT_TIDALQ].iMultiFile = 1;
  fnRead[OPT_TIDALQ] = &ReadTidalQ;
  
  sprintf(options[OPT_TIDALTAU].cName,"dTidalTau");
  sprintf(options[OPT_TIDALTAU].cDescr,"Tidal Time Lag");
  sprintf(options[OPT_TIDALTAU].cDefault,"1 Second");
  options[OPT_TIDALTAU].dDefault = 1;    
  options[OPT_TIDALTAU].iType = 2;
  options[OPT_TIDALTAU].iMultiFile = 1;
  options[OPT_TIDALTAU].dNeg = 1;
  sprintf(options[OPT_TIDALTAU].cNeg,"Seconds");
  fnRead[OPT_TIDALTAU] = &ReadTidalTau;
  
  sprintf(options[OPT_TIDEMODEL].cName,"sTideModel");
  sprintf(options[OPT_TIDEMODEL].cDescr,"Tidal Model: p2 [constant-phase-lag, 2nd order] t8 [constant-time-lag, 8th order]");
  sprintf(options[OPT_TIDEMODEL].cDefault,"p2");
  options[OPT_TIDEMODEL].iType = 3;
  fnRead[OPT_TIDEMODEL] = &ReadTideModel;

  sprintf(options[OPT_TIDEPERTS].cName,"saTidePerts");
  sprintf(options[OPT_TIDEPERTS].cDescr,"Names of bodies engaged in tidal evolution");
  sprintf(options[OPT_TIDEPERTS].cDefault,"none");
  options[OPT_TIDEPERTS].iType = 13;
  fnRead[OPT_TIDEPERTS] = &ReadTidePerts;
}

void ReadOptionsEqtide(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTEQTIDE;iOpt<OPTENDEQTIDE;iOpt++) {
    if (options[iOpt].iType != -1)
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    

/******************* Verify EQTIDE ******************/


void VerifyRotationEqtideWarning(char cName1[],char cName2[],char cFile[],int iLine1,int iLine2, int iVerbose) {
  if (iVerbose >= VERBINPUT) {
    fprintf(stderr,"WARNING: %s and %s are both set. Rotation rate will be in equilibrium.\n",cName1,cName2);
    fprintf(stderr,"\tFile: %s, Lines %d and %d\n",cFile,iLine1,iLine2);
  }
}

void VerifyRotationEqtide(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody) {
  double dMeanMotion;
  
  if (options[OPT_FORCEEQSPIN].iLine[iBody+1] >= 0) {
    dMeanMotion=fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[iBody].dMass);

    if (options[OPT_ROTPER].iLine[iBody+1] >= 0) 
      VerifyRotationEqtideWarning(options[OPT_FORCEEQSPIN].cName,options[OPT_ROTPER].cName,cFile,options[OPT_FORCEEQSPIN].iLine[iBody+1],options[OPT_ROTPER].iLine[iBody+1],control->Io.iVerbose);
      
    if (options[OPT_ROTRATE].iLine[iBody+1] >= 0) 
      VerifyRotationEqtideWarning(options[OPT_FORCEEQSPIN].cName,options[OPT_ROTRATE].cName,cFile,options[OPT_FORCEEQSPIN].iLine[iBody+1],options[OPT_ROTRATE].iLine[iBody+1],control->Io.iVerbose);

    if (options[OPT_ROTVEL].iLine[iBody+1] >= 0) 
      VerifyRotationEqtideWarning(options[OPT_FORCEEQSPIN].cName,options[OPT_ROTVEL].cName,cFile,options[OPT_FORCEEQSPIN].iLine[iBody+1],options[OPT_ROTVEL].iLine[iBody+1],control->Io.iVerbose);

    /* Done with warnings, do the assignment */

    body[iBody].dRotRate = fdEqRotRate(body[iBody],dMeanMotion,body[iBody].dEccSq,control->Evolve.iEqtideModel,control->Evolve.bDiscreteRot);
  }
  CalcXYZobl(body,iBody);
}

/*
 *
 * Verify Tidal Model
 *
 * Includes initialization of the fnUpdate matrix.
 *
 */

/* Get file where sTideModel set */
int fiTideFile(int *iLine,int iNumFiles) { 
  int iFile;

  for (iFile=0;iFile<iNumFiles;iFile++) {
    if (iLine[iFile] != -1) 
      return iFile;
  }

  assert(0);
}

/* In the following, iBody is the current body number that is getting assigned,
   iPert counts the number of bodies perturbing iBody, and iBodyPert is the
   body number of the current perturbing body. */

void InitializeXoblEqtide(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]] = 2;
  update[iBody].padDXoblDtEqtide[iPert] = &update[iBody].daDerivProc[update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]];
  update[iBody].iNumBodies[update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]]=2;
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]][0]=iBody;
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]][1] = body[iBody].iaTidePerts[iPert];
}

void InitializeYoblEqtide(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]] = 2;
  update[iBody].padDYoblDtEqtide[iPert] = &update[iBody].daDerivProc[update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]];
  update[iBody].iNumBodies[update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]]=2;
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]][0]=iBody;
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]][1] = body[iBody].iaTidePerts[iPert];
}

void InitializeZoblEqtide(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]] = 2;
  update[iBody].padDZoblDtEqtide[iPert] = &update[iBody].daDerivProc[update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]];
  update[iBody].iNumBodies[update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]]=2;
  update[iBody].iaBody[update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]][0]=iBody;
  update[iBody].iaBody[update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]][1] = body[iBody].iaTidePerts[iPert];
}

void InitializeRotEqtide(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] = 1;
  update[iBody].padDrotDtEqtide[iPert] = &update[iBody].daDerivProc[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]];
  update[iBody].iNumBodies[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]]=2;
  update[iBody].iaBody[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]][0]=iBody;

  update[iBody].iaBody[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]][1] = body[iBody].iaTidePerts[iPert];
}

void InitializeHeccEqtide(BODY *body,UPDATE *update,int iBody) {
  update[iBody].iaType[update[iBody].iHecc][update[iBody].iHeccEqtide] = 2;
  update[iBody].pdDHeccDtEqtide = &update[iBody].daDerivProc[update[iBody].iHecc][update[iBody].iHeccEqtide];
  update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iHeccEqtide]=2;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iHeccEqtide] = malloc(update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iHeccEqtide]*sizeof(int));
  /* Ecc is stored in iBody, and the perturbing body is the central object,
     which is body 0 */
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iHeccEqtide][0]=iBody;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iHeccEqtide][1]=0;
}

void InitializeKeccEqtide(BODY *body,UPDATE *update,int iBody) {
  update[iBody].iaType[update[iBody].iKecc][update[iBody].iKeccEqtide] = 2;
  update[iBody].pdDKeccDtEqtide = &update[iBody].daDerivProc[update[iBody].iKecc][update[iBody].iKeccEqtide];
  update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iKeccEqtide]=2;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iKeccEqtide] = malloc(update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iKeccEqtide]*sizeof(int));
  /* Ecc is stored in iBody, and the perturbing body is the central object,
     which is body 0 */
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iKeccEqtide][0]=iBody;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iKeccEqtide][1]=0;
}

void InitializeSemiEqtide(BODY *body,UPDATE *update,int iBody) {
  update[iBody].iaType[update[iBody].iSemi][update[iBody].iSemiEqtide] = 1;
  update[iBody].pdDsemiDtEqtide = &update[iBody].daDerivProc[update[iBody].iSemi][update[iBody].iSemiEqtide];
  update[iBody].iNumBodies[update[iBody].iSemi][update[iBody].iSemiEqtide]=2;
  update[iBody].iaBody[update[iBody].iSemi][update[iBody].iSemiEqtide] = malloc(update[iBody].iNumBodies[update[iBody].iSemi][update[iBody].iSemiEqtide]*sizeof(int));
  /* Semi is stored in iBody, and the perturbing body is the central object,
     which is body 0 */
  update[iBody].iaBody[update[iBody].iSemi][update[iBody].iSemiEqtide][0]=iBody;
  update[iBody].iaBody[update[iBody].iSemi][update[iBody].iSemiEqtide][1]=0;
}

void VerifyCTL(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int iPert,iTideFile,iCol,iFile;

  /* CTL model, but Q's set? */
  if (options[OPT_TIDALQ].iLine[iBody+1] >= 0) {
    /* Tidal Q was set */
    if (options[OPT_TIDALTAU].iLine[iBody+1] >= 0) {
      /* Tidal Tau was also set */
      if (control->Io.iVerbose >= VERBINPUT) 
      fprintf(stderr,"WARNING: Time lag model selected, %s and %s set in file %s. Using %s = %lf and ignoring %s.\n",options[OPT_TIDALTAU].cName,options[OPT_TIDALQ].cName,options[OPT_TIDALTAU].cFile[iBody+1],options[OPT_TIDALTAU].cName,body[iBody+1].dTidalTau,options[OPT_TIDALQ].cName);
    } else {
      /* Tidal Tau was not set */
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: Time lag model selected, but only %s was set in file %s.\n",options[OPT_TIDALQ].cName,files->Infile[iBody+1].cIn);
      exit(EXIT_INPUT);
    }
    
  }
    
  /* Can't set bDiscreteRot in the CTL model */
  if (options[OPT_DISCRETEROT].iLine[iBody+1] >= 0) 
    VerifyBodyExit(options[OPT_TIDALTAU].cName,options[OPT_DISCRETEROT].cName,options[OPT_DISCRETEROT].cFile[iBody],options[OPT_TIDALTAU].iLine[iBody],options[OPT_DISCRETEROT].iLine[iBody+1],control->Io.iVerbose);
  
  /* Verify output contains no CPL-specific parameters */ 
  
  if (files->Outfile[iBody].iNumCols > 0) {
    for (iCol=0;iCol<files->Outfile[iBody].iNumCols;iCol++) {
      
      if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_EQROTPERCONT].cName,strlen(output[OUT_EQROTPERCONT].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) 
          fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_EQROTPERCONT].cName);
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
      }
      
      if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_EQROTPERDISCRETE].cName,strlen(output[OUT_EQROTPERDISCRETE].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) 
          fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_EQROTPERDISCRETE].cName);
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
      }
      
      if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_EQROTRATECONT].cName,strlen(output[OUT_EQROTRATECONT].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) 
          fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_EQROTRATECONT].cName);
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
      }
      
      if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_EQROTRATEDISCRETE].cName,strlen(output[OUT_EQROTRATEDISCRETE].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) 
          fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_EQROTRATEDISCRETE].cName);
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
      }
      
      if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_GAMMAORB].cName,strlen(output[OUT_GAMMAORB].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) 
          fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_GAMMAORB].cName);
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
      }
      
      if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_GAMMAROT].cName,strlen(output[OUT_GAMMAROT].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) 
          fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_GAMMAROT].cName);
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
      }
      
      if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_TIDALQ].cName,strlen(output[OUT_TIDALQ].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) 
          fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_TIDALQ].cName);
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
      }      
    }
  }
  
  /* Everything OK, assign Updates */
  
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    
    /* Obliquity */
    //update[iBody].padDoblDtEqtide = malloc(body[iBody].iTidePerts*sizeof(double*));
    //InitializeOblEqtide(body,update,iBody,iPert);
    //fnUpdate[iBody][update[iBody].iObl][update[iBody].iaOblEqtide[iPert]] = &fdCTLDobliquityDt;
    
    /* Rotation Rate */
    InitializeRotEqtide(body,update,iBody,iPert);
    fnUpdate[iBody][update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] = &fdCTLDrotrateDt;
    
    /* Internal Energy XXX 
    InitializeIntEnEqtide(update,iBody,iPert,body[iBody].iaTidePerts[iPert]);
    fnUpdate[iBody][update[iBody].iIntEn][update[iBody].iaIntEnEqtide[iPert]] = &fdCTLDrotrateDt;
    */  
  }      

  /* Is this the secondary body, and hence we assign da/dt and de/dt? */
  /* For now exomoons are disallowed, so if body[iBody].iTidePerts > 1, 
     then this is the central body. 
     N.B.: It's fishy to assume the equilibrium tidal model will work with
     multiple perturbers. When do tidal waves cancel out? I think we wave our
     hands and say in an orbit-averaged sense the torques tide raised by the 
     other body average to 0. */
  
  if (!bPrimary(body,iBody)) {
    if (!control->Evolve.bFixOrbit[iBody]) {
      /* Semi-major axis */
      InitializeSemiEqtide(body,update,iBody);
      fnUpdate[iBody][update[iBody].iSemi][update[iBody].iSemiEqtide] = &fdCTLDsemiDt;
      
      /* Eccentricity */
      //InitializeEccEqtide(body,update,iBody);
      //fnUpdate[iBody][update[iBody].iEcc][update[iBody].iEccEqtide] = &fdCTLDeccDt;
    } else {
      // XXX This won't work if more than one module is affecting the orbit
      //update[iBody].iNumEqns[update[iBody].iEcc]--;
      update[iBody].iNumEqns[update[iBody].iSemi]--;
    }
  }

  /* Malloc body memory 
     N.B. that space is allocated for all planets, even though only
     iTidePerts are actually modeled. This deicision makes coding 
     the derivatives easier. iaBody can point to the element of the 
     body struct, and non-perturbing body elements are never touched.
     The alternative is to use a new array, maybe iIndex, that 
     connects the element # in the body struct to the element number
     in the TidalZ array. That seems too cumbersome to me, but this 
     solution definitely wastes memory. 
  */

  body[iBody].dTidalF = malloc(control->Evolve.iNumBodies*sizeof(double*));
  body[iBody].dTidalBeta = malloc(control->Evolve.iNumBodies*sizeof(double));
  for (iPert=0;iPert<control->Evolve.iNumBodies;iPert++)
    body[iBody].dTidalF[iPert]=malloc(5*sizeof(double));

  control->Evolve.fnPropsAux[iBody][iModule]=&PropsAuxCTL;

  /* Now remove output options unique to CPL. This will prevent segmentation
     faults as memory will not be allocated to some parameters unless CPL
     is chosen. */
  output[OUT_EQROTRATEDISCRETE].iNum = 0;
  output[OUT_EQROTPERDISCRETE].iNum = 0; 
  output[OUT_EQROTRATECONT].iNum = 0;
  output[OUT_EQROTPERCONT].iNum =0;
  output[OUT_GAMMAROT].iNum = 0;
  output[OUT_GAMMAORB].iNum = 0;
  output[OUT_TIDALQ].iNum = 0;
}

void VerifyCPL(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int iPert,iTideFile,iCol,iFile;

  //XXX Is dEccSq set here?
  // Body 0 has no orbit parameters see VerifyOrbitEqtide().
  if (iBody != 0 && body[iBody].dEccSq > (2./19) && control->Evolve.bDiscreteRot) {
    if (control->Io.iVerbose >= VERBINPUT)
      fprintf(stderr,"WARNING: Setting %s to 1 is not advised for eccentricities larger than %.3lf\n",options[OPT_DISCRETEROT].cName,pow(2./19,0.5));
  }
  
  /* CPL model, but tau's set? */
  if (options[OPT_TIDALTAU].iLine[iBody+1] >= 0) {
    /* Tidal tau was set */
    if (options[OPT_TIDALQ].iLine[iBody+1] >= 0) {
      /* Tidal Q was also set */
      if (control->Io.iVerbose >= VERBINPUT) 
        fprintf(stderr,"WARNING: Phase lag model selected, %s and %s set in file %s. Using %s = %lf and ignoring %s.\n",options[OPT_TIDALTAU].cName,options[OPT_TIDALQ].cName,options[OPT_TIDALTAU].cFile[iBody+1],options[OPT_TIDALQ].cName,body[iBody+1].dTidalQ,options[OPT_TIDALTAU].cName);
    } else {
      /* Tidal Tau was not set */
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: Phase lag model selected, but only %s was set in file %s.\n",options[OPT_TIDALTAU].cName,files->Infile[iBody+1].cIn);
      exit(EXIT_INPUT);
    }
    
    /* Verify output contains no CTL-specific parameters */ 
    
    if (files->Outfile[iBody].iNumCols > 0) {
      for (iCol=0;iCol<files->Outfile[iBody].iNumCols;iCol++) {
        
        if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_TIDALQ].cName,strlen(output[OUT_TIDALQ].cName)) == 0) {
          if (control->Io.iVerbose >= VERBINPUT) 
            fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_TIDALQ].cName);
          /* Must find file that declares tidel model */
          iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
          DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
        }
      }
    }
    /* XXX Should re-"null" the outputs */
  }
  
  /* Everything OK, assign Updates */
  
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    
    /* Obliquity
    InitializeOblEqtide(body,update,iBody,iPert);
    fnUpdate[iBody][update[iBody].iObl][update[iBody].iaOblEqtide[iPert]] = &fdCPLDobliquityDt;
     */

    // Xobl
    InitializeXoblEqtide(body,update,iBody,iPert);
    fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]] = &fdCPLDXoblDt;

    // Yobl
    InitializeYoblEqtide(body,update,iBody,iPert);
    fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]] = &fdCPLDYoblDt;

    // Zobl
    InitializeZoblEqtide(body,update,iBody,iPert);
    fnUpdate[iBody][update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]] = &fdCPLDZoblDt;

    /* Rotation Rate */
    InitializeRotEqtide(body,update,iBody,iPert);
    fnUpdate[iBody][update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] = &fdCPLDrotrateDt;
  }  
  
  /* Is this the secondary body, and hence we assign da/dt and de/dt? */
  if (!bPrimary(body,iBody)) {
    if (!control->Evolve.bFixOrbit[iBody]) {
      /* Semi-major axis */
      InitializeSemiEqtide(body,update,iBody);
      fnUpdate[iBody][update[iBody].iSemi][update[iBody].iSemiEqtide] = &fdCPLDsemiDt;
      
      /* Eccentricity */
      //InitializeEccEqtide(body,update,iBody);
      //fnUpdate[iBody][update[iBody].iEcc][update[iBody].iEccEqtide] = &fdCPLDeccDt;
      InitializeHeccEqtide(body,update,iBody);
      fnUpdate[iBody][update[iBody].iHecc][update[iBody].iHeccEqtide] = &fdCPLDHeccDt;
      InitializeKeccEqtide(body,update,iBody);
      fnUpdate[iBody][update[iBody].iKecc][update[iBody].iKeccEqtide] = &fdCPLDKeccDt;
    } else {
      // XXX This won't work if more than one module is affecting the orbit
      update[iBody].iNumEqns[update[iBody].iHecc]--;
      update[iBody].iNumEqns[update[iBody].iKecc]--;
      update[iBody].iNumEqns[update[iBody].iSemi]--;
    }
  }

  /* Malloc body memory 
     N.B. that space is allocated for all planets, even though only
     iTidePerts are actually modeled. This deicision makes coding 
     the derivatives easier. iaBody can point to the element of the 
     body struct, and non-perturbing body elements are never touched.
     The alternative is to use a new array, maybe iIndex, that 
     connects the element # in the body struct to the element number
     in the TidalZ array. That seems too cumbersome to me, but this 
     solution definitely wastes memory. 
  */

  body[iBody].iTidalEpsilon = malloc(control->Evolve.iNumBodies*sizeof(int*));
  for (iPert=0;iPert<control->Evolve.iNumBodies;iPert++)
    body[iBody].iTidalEpsilon[iPert]=malloc(10*sizeof(int));
  control->Evolve.fnPropsAux[iBody][iModule]=&PropsAuxCPL;

  /* Now remove output options unique to CTL. This will prevent segmentation
     faults as memory will not be allocated to some parameters unless CTL
     is chosen. */
  output[OUT_TIDALTAU].iNum = 0;
}

/** Verify all arguments to saTidePerturbers. This subroutine will called 
   from each body using module eqtide, but we must make sure that each pair 
   of perturbing bodies points to each other, so we must loop through verify 
   all the bodies at the same time. This means all these lines will be 
   repeated for each tidally evolving body. But, if it's verified the first 
   time, it should verify every time! */

void VerifyPerturbersEqtide(BODY *body,FILES *files,OPTIONS *options,UPDATE *update,int iNumBodies,int iBody) {
  int iPert,iBodyPert,iVar,ok;
  int bFound[iNumBodies];

  for (iBody=0;iBody<iNumBodies;iBody++) {
    
    if (body[iBody].iTidePerts > 0) {
      for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
        bFound[iPert] = 0;
        for (iBodyPert=0;iBodyPert<iNumBodies;iBodyPert++) {
          if (iBodyPert != iBody) {
            if (strncmp(body[iBody].saTidePerts[iPert],body[iBodyPert].cName,sizeof(body[iBody].saTidePerts[iPert])) == 0) {
              /* This parameter contains the body # of the "iPert-th" 
                 tidal perturber */
              body[iBody].iaTidePerts[iPert]=iBodyPert;
              bFound[iPert]=1;
            }
          }
        }
      }
      
      /* Were all tidal perturbers identified? */
      ok=1;
      for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
        if (bFound[iPert] == 0) {
          fprintf(stderr,"ERROR: Unknown tidal perturber to body %s: %s\n",body[iBody].cName,body[iBody].saTidePerts[iPert]);
          fprintf(stderr,"\tFile: %s, Line: %d\n",options[OPT_TIDEPERTS].cFile[iBody+1],options[OPT_TIDEPERTS].iLine[iBody+1]);
          ok=0;
        }
      }
      
      /* Was the same perturber listed multiple times? */
      
      for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
        for (iBodyPert=iPert+1;iBodyPert<body[iBody].iTidePerts;iBodyPert++) {
          if (body[iBody].iaTidePerts[iPert] == body[iBody].iaTidePerts[iBodyPert]) {
            fprintf(stderr,"ERROR: Body %s listed mulitple times to argument %s.\n",body[iBody].saTidePerts[iPert],options[OPT_TIDEPERTS].cName);
            fprintf(stderr,"\tFile: %s, Line: %d\n",options[OPT_TIDEPERTS].cFile[iBody+1],options[OPT_TIDEPERTS].iLine[iBody+1]);
            ok=0;
          }
        }
      }
      
      for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
        if (!(body[body[iBody].iaTidePerts[iPert]].bEqtide)) {
          fprintf(stderr,"ERROR: Eqtide called for body %s, but option %s not set.\n",body[iBody].cName,options[OPT_TIDEPERTS].cName);
          ok=0;
        }
      }
      
      if (!ok)
        exit(EXIT_INPUT);
    }
  }

  /* All entries to saTidePerts are known bodies, does each point to the other? 
     Exomoon ready! */
  for (iBody=0;iBody<iNumBodies;iBody++) {
    ok=0;
    for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
      for (iBodyPert=0;iBodyPert<body[body[iBody].iaTidePerts[iPert]].iTidePerts;iBodyPert++) {
        if (iBody == body[body[iBody].iaTidePerts[iPert]].iaTidePerts[iBodyPert])
          /* Match */
          ok=1;
      }
      if (!ok) {
        fprintf(stderr,"ERROR: %s tidally perturbs %s, but %s does NOT tidally perturb %s\n",body[iBody].cName,body[body[iBody].iaTidePerts[iPert]].cName,body[body[iBody].iaTidePerts[iPert]].cName,body[iBody].cName);
        fprintf(stderr,"\tFile: %s, Line: %d\n",files->Infile[iBody+1].cIn,options[OPT_TIDEPERTS].iLine[iBody+1]);
        fprintf(stderr,"\tFile: %s, Line: %d\n",files->Infile[body[iBody].iaTidePerts[iPert]+1].cIn,options[OPT_TIDEPERTS].iLine[iPert+1]);
        exit(EXIT_INPUT);
      }
    }
  }
}

void VerifyOrbitEqtide(BODY *body,CONTROL *control,FILES *files,OPTIONS *options) {
  int iBody;

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    if (iBody == 0) {
      /* Orbital parameters cannot be input for the central body */
      if (options[OPT_ORBECC].iLine[iBody+1] > -1) {
        fprintf(stderr,"ERROR: %s cannot be set for the central body.\n",options[OPT_ORBECC].cName);
        LineExit(files->Infile[iBody+1].cIn,options[OPT_ORBECC].iLine[iBody+1]);
      }
      if (options[OPT_ORBSEMI].iLine[iBody+1] > -1) {
        fprintf(stderr,"ERROR: %s cannot be set for the central body.\n",options[OPT_ORBSEMI].cName);
        LineExit(files->Infile[iBody+1].cIn,options[OPT_ORBSEMI].iLine[iBody+1]);
      }
    } else {
      body[iBody].dEccSq = body[iBody].dEcc*body[iBody].dEcc;
      CalcHK(body,iBody);
    }
  }
}

int fiGetModuleIntEqtide(MODULE *module,int iBody) {
  int iModule;

  for (iModule=0;iModule<module->iNumModules[iBody];iModule++) {
    if (module->iaModule[iBody][iModule] == EQTIDE)
      return iModule;
  }

  fprintf(stderr,"ERROR: Eqtide not found for body #%d.\n",iBody);
  exit(1);
}

void VerifyTideModel(CONTROL *control,FILES *files,OPTIONS *options) {
  int iFile,iFound=0;
  char cTmp[8];

  for (iFile=0;iFile<files->iNumInputs;iFile++) {
    if (options[OPT_TIDEMODEL].iLine[iFile] >= 0)
      iFound++;
  }

  if (iFound > 1) {
    if (control->Io.iVerbose > VERBERR) {
      fprintf(stderr,"ERROR: Option %s set multiple times.\n",options[OPT_TIDEMODEL].cName);
      for (iFile=0;iFile<files->iNumInputs;iFile++) {
        if (options[OPT_TIDEMODEL].iLine[iFile] >= 0)
          fprintf(stderr,"\tFile %s, Line: %d\n",files->Infile[0].cIn,options[OPT_TIDEMODEL].iLine[iFile]);
      }
    }
    exit(EXIT_INPUT);
  }

  if (iFound == 0) {
    strcpy(cTmp,options[OPT_TIDEMODEL].cDefault);
    if (!memcmp(sLower(cTmp),"p2",2)) {
      control->Evolve.iEqtideModel = CPL;
    } else if (!memcmp(sLower(cTmp),"t8",2)) {
      control->Evolve.iEqtideModel = CTL;
    }
    if (control->Io.iVerbose >= VERBINPUT) 
      fprintf(stderr,"WARNING: %s not set in any file, defaulting to %s.\n",options[OPT_TIDEMODEL].cName,options[OPT_TIDEMODEL].cDefault);

    /* Chicanery. Since I only want this set once, I will
       make it seem like the user set it. */
    options[OPT_TIDEMODEL].iLine[0] = 1;
  }  
}


void VerifyEqtide(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  //int iModule;

  /* This function only called if Eqtide is active for this body */

  /* Get module number */
  //iModule = fdGetModuleIntEqtide(update,iBody);

  VerifyTideModel(control,files,options);

  VerifyOrbitEqtide(body,control,files,options);

  VerifyPerturbersEqtide(body,files,options,update,control->Evolve.iNumBodies,iBody);

  /* Verify input set correctly and assign update functions */
  if (control->Evolve.iEqtideModel == CTL)
    VerifyCTL(body,control,files,options,output,update,fnUpdate,iBody,iModule);
  
  if (control->Evolve.iEqtideModel == CPL)
    VerifyCPL(body,control,files,options,output,update,fnUpdate,iBody,iModule);

  body[iBody].dTidalZ=malloc(control->Evolve.iNumBodies*sizeof(double));
  body[iBody].dTidalChi=malloc(control->Evolve.iNumBodies*sizeof(double));
  control->fnForceBehavior[iBody][iModule]=&ForceBehaviorEqtide;
  control->Evolve.fnBodyCopy[iBody][iModule]=&BodyCopyEqtide;
}

/**************** EQTIDE Update ***************/

void InitializeUpdateEqtide(BODY *body,UPDATE *update,int iBody) {
  /* All active bodies can have their obliquity and rotation tracked. */
  if (update[iBody].iNumXobl == 0)
    update[iBody].iNumVars++;
  update[iBody].iNumXobl += body[iBody].iTidePerts;

  if (update[iBody].iNumYobl == 0)
    update[iBody].iNumVars++;
  update[iBody].iNumYobl += body[iBody].iTidePerts;

  if (update[iBody].iNumZobl == 0)
    update[iBody].iNumVars++;
  update[iBody].iNumZobl += body[iBody].iTidePerts;

  if (update[iBody].iNumRot == 0)
    update[iBody].iNumVars++;
  update[iBody].iNumRot += body[iBody].iTidePerts;;
  
  /* Eqtide is complicated by the possibility that 1 body could be perturbed 
     by multiple others. saEqtidePertubers is the list of those bodies 
     which are tidally interacting with it, and body->iaTidePert contains
     the body #s which are the perturbers. For now, the higher body number
     always carries a and e. In other words, right now only 1 central body
     is allowed. The addition of exomoons will make this more complicated.
  */
  
  //if (!bPrimary(body,iBody)) {
  if (iBody > 0) {
    if (update[iBody].iNumSemi == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumSemi++;

    if (update[iBody].iNumHecc == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumHecc++;

    if (update[iBody].iNumKecc == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumKecc++;
  }
}

void FinalizeUpdateHeccEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
  update[iBody].iHeccEqtide=*iEqn;
  (*iEqn)++;
}

void FinalizeUpdateKeccEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
  update[iBody].iKeccEqtide=*iEqn;
  (*iEqn)++;
}

void FinalizeUpdateXoblEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;

  /* Change iTidePerts to iNumBodies XXX */

  update[iBody].padDXoblDtEqtide = malloc(body[iBody].iTidePerts*sizeof(double*));
  update[iBody].iaXoblEqtide = malloc(body[iBody].iTidePerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
    update[iBody].iaXoblEqtide[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateYoblEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;

  /* Change iTidePerts to iNumBodies XXX */

  update[iBody].padDYoblDtEqtide = malloc(body[iBody].iTidePerts*sizeof(double*));
  update[iBody].iaYoblEqtide = malloc(body[iBody].iTidePerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
    update[iBody].iaYoblEqtide[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateZoblEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;

  /* Change iTidePerts to iNumBodies XXX */

  update[iBody].padDZoblDtEqtide = malloc(body[iBody].iTidePerts*sizeof(double*));
  update[iBody].iaZoblEqtide = malloc(body[iBody].iTidePerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
    update[iBody].iaZoblEqtide[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateRotEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;

  update[iBody].padDrotDtEqtide = malloc(body[iBody].iTidePerts*sizeof(double*));
  update[iBody].iaRotEqtide = malloc(body[iBody].iTidePerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
    update[iBody].iaRotEqtide[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateSemiEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
  update[iBody].iSemiEqtide = *iEqn;
  (*iEqn)++;
}


/***************** EQTIDE Halts *****************/

/* Double Synchronous? */

/* How is this handled for multi-planet systems? XXX */
int HaltDblSync(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  /* Forbidden if iNumBodies > 2 XXX Add to VerifyHalts */

  /* dMeanMotion set by call to TidalProperties in Evolve() */
  if (halt->bDblSync && (body[0].dRotRate == body[1].dMeanMotion) && (body[1].dRotRate == body[1].dMeanMotion)) {
    /* XXX Check bForceEqSpin, too? */
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: Double Synchronous at ");
      fprintd(stdout,evolve->dTime/YEARSEC,io->iSciNot,io->iDigits);
      printf(" years.\n");
    }
    return 1;
  }

  return 0;
}  

/* Tide-locked? */
int HaltTideLock(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  /* Forbidden for body 0 if iNumBodies > 2 XXX Add to VerifyHalts*/

  if ((body[iBody].dRotRate == body[iBody].dMeanMotion) && halt->bTideLock) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s tide-locked at ",body[iBody].cName);
      fprintd(stdout,evolve->dTime/YEARSEC,io->iSciNot,io->iDigits);
      printf(" years.\n");
    }
    return 1;
  }

  return 0;
}

/* Synchronous Rotation? */
int HaltSyncRot(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  /* Forbidden for body 0 if iNumBodies > 2 XXX Add to VerifyHalts */

  if (halt->bSync && (body[iBody].dRotRate == body[iBody].dMeanMotion)) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's rotation is synchronous (e = ",body[iBody].cName);
      fprintd(stdout,sqrt(body[iBody].dEccSq),io->iSciNot,io->iDigits);
      printf(") at ");
      fprintd(stdout,evolve->dTime/YEARSEC,io->iSciNot,io->iDigits);
      printf(" years.\n");
    }
    return 1;
  }

  return 0;
}

void CountHaltsEqtide(HALT *halt,int *iNumHalts) {
  if (halt->bDblSync)
    (*iNumHalts)++;
  if (halt->bTideLock)
    (*iNumHalts)++;
  if (halt->bSync)
    (*iNumHalts)++;
}

void VerifyHaltEqtide(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {

  if (control->Halt[iBody].bDblSync) {
    if (control->Evolve.iNumBodies > 2) {
      fprintf(stderr,"ERROR: Cannot set %s for systems with more than 2 bodies.\n",options[OPT_HALTDBLSYNC].cName);
      DoubleLineExit(options[OPT_BODYFILES].cFile[0],options[OPT_HALTDBLSYNC].cFile[iBody+1],options[OPT_BODYFILES].iLine[0],options[OPT_HALTDBLSYNC].iLine[iBody+1]);
      exit(EXIT_INPUT);
    } else
      control->fnHalt[iBody][(*iHalt)++] = &HaltDblSync;
  }
  if (control->Halt[iBody].bTideLock) {
    if (control->Evolve.iNumBodies > 2 && iBody == 0) {
      fprintf(stderr,"ERROR: Cannot set %s's %s for systems with more than 2 bodies.\n",body[iBody].cName,options[OPT_HALTTIDELOCK].cName);
      DoubleLineExit(options[OPT_BODYFILES].cFile[0],options[OPT_HALTTIDELOCK].cFile[iBody+1],options[OPT_BODYFILES].iLine[0],options[OPT_HALTTIDELOCK].iLine[iBody+1]);
      exit(EXIT_INPUT);
    } else
      control->fnHalt[iBody][(*iHalt)++] = &HaltTideLock;
  }
  if (control->Halt[iBody].bSync) {
    if (control->Evolve.iNumBodies > 2 && iBody == 0) {
      fprintf(stderr,"ERROR: Cannot set %s's %s for systems with more than 2 bodies.\n",body[iBody].cName,options[OPT_HALTSYNCROT].cName);
      DoubleLineExit(options[OPT_BODYFILES].cFile[0],options[OPT_HALTSYNCROT].cFile[iBody+1],options[OPT_BODYFILES].iLine[0],options[OPT_HALTSYNCROT].iLine[iBody+1]);
      exit(EXIT_INPUT);
    } else
    control->fnHalt[iBody][(*iHalt)++] = &HaltSyncRot;
  }
}

/************* EQTIDE Outputs ******************/

/* 
 * B
 */

void WriteBodyDsemiDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  int iPert;

  if (iBody == 0) 
    iPert=1;
  else
    iPert=0;
  /* XXX Broken */

  if (control->Evolve.iEqtideModel == CPL) 
    *dTmp = fdCPLDsemiDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEccSq);
  else if (control->Evolve.iEqtideModel == CTL) 
    *dTmp = fdCTLDsemiDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEccSq,body[iBody].dObliquity,body[iBody].dRotRate);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsLength(units->iLength);
    fsUnitsVel(units,cUnit);
  }
}

void WriteBodyDeccDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  int iPert;

  if (iBody == 0) 
    iPert=1;
  else
    iPert=0;

  /* XXX Broken -- needs to be changed after switch to Hecc + Kecc

  if (control->Evolve.iEqtideModel == CPL) 
    *dTmp = fdCPLDeccDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEccSq);
  else
    *dTmp = fdCTLDeccDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEccSq);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
  */
  *dTmp = -1;

}

void WriteDOblDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dFoo;
  int iPert;

  // Total change in dObl/dTime
  dFoo = 0;
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) 
    dFoo += body[iBody].daDoblDtEqtide[body[iBody].iaTidePerts[iPert]];

  *dTmp = dFoo;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsAngle(units->iAngle);
    fsUnitsAngRate(units,cUnit);
  }
}

void WriteDSemiDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;

  /* Ensure that we don't overwrite pdDsemiDt */
  dDeriv = *(update[iBody].pdDsemiDtEqtide); 
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsLength(units->iLength);
    fsUnitsVel(units,cUnit);
  }
}

void WriteDEccDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  int *iaBody;

  iaBody = malloc(2*sizeof(int));
  iaBody[0] = iBody;
  iaBody[1] = 0;

  if (control->Evolve.iEqtideModel == CPL)
    *dTmp = body[iBody].dDeccDtEqtide;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }

  free(iaBody);
}

void WriteDMeanMotionDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = -1.5*pow(BIGG*(body[0].dMass+body[iBody].dMass)/pow(body[iBody].dSemi,5),0.5)*(*(update[iBody].pdDsemiDtEqtide));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsLength(units->iLength);
    fsUnitsRate(units->iTime,cUnit);
    strcat(cUnit,"^2");
  }
}

void WriteDOrbPerDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = 1.5*pow(4*PI*PI*body[iBody].dSemi/(BIGG*(body[0].dMass+body[iBody].dMass)),0.5)*(*(update[iBody].pdDsemiDtEqtide));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsLength(units->iLength);
    sprintf(cUnit,"");
  }
}

void WriteDRotPerDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) 
    dDeriv += *(update[iBody].padDrotDtEqtide[iPert]);

  /* Multiply by dP/domega to get dP/dt */
  *dTmp = dDeriv * (-2*PI/(body[iBody].dRotRate*body[iBody].dRotRate));
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }  else {
    strcpy(cUnit,"");
  }
}

void WriteDRotRateDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) 
    dDeriv += *(update[iBody].padDrotDtEqtide[iPert]);
  
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }  else {
    *dTmp *= fdUnitsTime(units->iTime)*fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
    strcat(cUnit,"^2");
  }
}

void WriteDHeccDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;

  /* Ensure that we don't overwrite pdDsemiDt */
  dDeriv = *(update[iBody].pdDHeccDtEqtide); 
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsLength(units->iLength);
    fsUnitsVel(units,cUnit);
  }
}

void WriteDKeccDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;

  /* Ensure that we don't overwrite pdDsemiDt */
  dDeriv = *(update[iBody].pdDKeccDtEqtide); 
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsLength(units->iLength);
    fsUnitsVel(units,cUnit);
  }
}

void WriteDXoblDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv=0;
  int iPert;

  /* Ensure that we don't overwrite pdDXoblDt */
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++)
    dDeriv += (*(update[iBody].padDXoblDtEqtide[body[iBody].iaTidePerts[iPert]])); 
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteDYoblDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv=0;
  int iPert;

  /* Ensure that we don't overwrite pdDYoblDt */
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++)
    dDeriv += (*(update[iBody].padDYoblDtEqtide[body[iBody].iaTidePerts[iPert]])); 
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteDZoblDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv=0;
  int iPert;

  /* Ensure that we don't overwrite pdDsemiDt */
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++)
    dDeriv += (*(update[iBody].padDZoblDtEqtide[body[iBody].iaTidePerts[iPert]])); 
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

/*
 * E
 */

void WriteEccTimescaleEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv,dEcc;

  // Complicated because primary variable is h or k
  // de/dt = dh/dt * de/dh 
  if (body[iBody].dLongP == 0) {
    dDeriv = *(update[iBody].pdDKeccDtEqtide)/cos(body[iBody].dLongP); 
    dEcc = body[iBody].dKecc/cos(body[iBody].dLongP);
  } else {
    dDeriv = *(update[iBody].pdDHeccDtEqtide)/sin(body[iBody].dLongP); 
    dEcc = body[iBody].dHecc/sin(body[iBody].dLongP);
  }

  *dTmp = fdTimescale(dEcc,dDeriv);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteEqRotPer(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdFreqToPer(fdEqRotRate(body[iBody],body[iBody].dMeanMotion,body[iBody].dEccSq,control->Evolve.iEqtideModel,control->Evolve.bDiscreteRot));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }  else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteEqRotPerCont(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdFreqToPer(fdCPLEqRotRateCont(body[iBody].dMeanMotion,body[iBody].dEccSq));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteEqRotPerDiscrete(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdFreqToPer(fdCPLEqRotRateDiscrete(body[iBody].dMeanMotion,body[iBody].dEccSq));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteEqRotRate(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdEqRotRate(body[iBody],body[1].dMeanMotion,body[1].dEccSq,control->Evolve.iEqtideModel,control->Evolve.bDiscreteRot);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteEqRotRateCont(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* XXX Broken -- needs to allow for multiple bodies */
  *dTmp = fdCPLEqRotRateCont(body[1].dMeanMotion,body[1].dEccSq);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteEqRotRateDiscrete(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* XXX Broken -- needs to allow for multiple bodies */
  *dTmp = fdCPLEqRotRateDiscrete(body[1].dMeanMotion,body[1].dEccSq);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }      
}

/*
 * G
 */

void WriteGammaOrb(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  /* Broken XXX */

  *dTmp = fdGammaOrb(body[1].dEccSq,body[iBody].dObliquity,body[iBody].iTidalEpsilon[0]);

  /* Negative option? */
  strcat(cUnit,"cgs");
}

void WriteGammaRot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  /* XXX Broken */

  *dTmp = fdGammaRot(body[1].dEccSq,body[iBody].dObliquity,body[iBody].iTidalEpsilon[0]);

  /* Negative option? */
  strcat(cUnit,"cgs");
}

void WriteImK2(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dImK2;

  strcpy(cUnit,"");
}

void WriteOblTimescaleEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  /* XXX Need to change after switch to [XYZ]obl
  *dTmp = fdTimescaleMulti(body[iBody].dObliquity,*(update[iBody].padDoblDtEqtide),body[iBody].iTidePerts);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
  */
  *dTmp =-1;
}

void WriteRotTimescaleEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdTimescaleMulti(body[iBody].dRotRate,*(update[iBody].padDrotDtEqtide),body[iBody].iTidePerts);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

/*
 *S 
 */

void WriteSemiTimescaleEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* XXX Broken -- needs to allow for multiple bodies */
  *dTmp = fdTimescale(body[1].dSemi,*(update[1].pdDsemiDtEqtide));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

/*
 * T
 */

void WritePowerEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total tidal power */
  *dTmp = fdTidePower(body,system,update,iBody,control->Evolve.iEqtideModel);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void WriteEnergyFluxEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total tidal surface energy flux */
  *dTmp = fdSurfEnFluxEqtide(body,system,update,iBody,control->Evolve.iEqtideModel);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteTidalTau(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dTidalTau;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteTideLock(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = control->Evolve.bForceEqSpin[iBody];
  strcat(cUnit,"");
}

void InitializeOutputEqtide(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_BODYDSEMIDTEQTIDE].cName,"BodyDsemiDtEqtide");
  sprintf(output[OUT_BODYDSEMIDTEQTIDE].cDescr,"Body's Contribution to dSemi/dt in EQTIDE");
  sprintf(output[OUT_BODYDSEMIDTEQTIDE].cNeg,"AU/Gyr");
  output[OUT_BODYDSEMIDTEQTIDE].bNeg = 1;
  output[OUT_BODYDSEMIDTEQTIDE].dNeg = YEARSEC*1e9/AUCM;
  output[OUT_BODYDSEMIDTEQTIDE].iNum = 1;
  fnWrite[OUT_BODYDSEMIDTEQTIDE] = &WriteBodyDsemiDtEqtide;
  
  sprintf(output[OUT_BODYDECCDTEQTIDE].cName,"BodyDeccDt");
  sprintf(output[OUT_BODYDECCDTEQTIDE].cDescr,"Body's Contribution to dEcc/dt in EQTIDE");
  sprintf(output[OUT_BODYDECCDTEQTIDE].cNeg,"/Gyr");
  output[OUT_BODYDECCDTEQTIDE].bNeg = 1;
  output[OUT_BODYDECCDTEQTIDE].dNeg = YEARSEC*1e9;
  output[OUT_BODYDECCDTEQTIDE].iNum = 1;
  fnWrite[OUT_BODYDECCDTEQTIDE] = &WriteBodyDeccDtEqtide;

  sprintf(output[OUT_DOBLDTEQTIDE].cName,"DOblDtEqtide");
  sprintf(output[OUT_DOBLDTEQTIDE].cDescr,"Time Rate of Change of Obliquity in EQTIDE");
  sprintf(output[OUT_DOBLDTEQTIDE].cNeg,"deg/Gyr");
  output[OUT_DOBLDTEQTIDE].bNeg = 1;
  output[OUT_DOBLDTEQTIDE].dNeg = YEARSEC*1e9/DEGRAD;
  output[OUT_DOBLDTEQTIDE].iNum = 1;
  fnWrite[OUT_DOBLDTEQTIDE] = &WriteDOblDtEqtide;
  
  sprintf(output[OUT_DSEMIDTEQTIDE].cName,"DsemiDtEqtide");
  sprintf(output[OUT_DSEMIDTEQTIDE].cDescr,"Total da/dt in EQTIDE");
  sprintf(output[OUT_DSEMIDTEQTIDE].cNeg,"AU/Gyr");
  output[OUT_DSEMIDTEQTIDE].bNeg = 1;
  output[OUT_DSEMIDTEQTIDE].dNeg = YEARSEC*1e9/AUCM;
  output[OUT_DSEMIDTEQTIDE].iNum = 1;
  fnWrite[OUT_DSEMIDTEQTIDE] = &WriteDSemiDtEqtide;
  
  sprintf(output[OUT_DECCDTEQTIDE].cName,"DeccDtEqtide");
  sprintf(output[OUT_DECCDTEQTIDE].cDescr,"Total de/dt in EQTIDE");
  sprintf(output[OUT_DECCDTEQTIDE].cNeg,"/Gyr");
  output[OUT_DECCDTEQTIDE].bNeg = 1;
  output[OUT_DECCDTEQTIDE].dNeg = YEARSEC*1e9;
  output[OUT_DECCDTEQTIDE].iNum = 1;
  fnWrite[OUT_DECCDTEQTIDE] = &WriteDEccDtEqtide;
  
  sprintf(output[OUT_DMMDTEQTIDE].cName,"DMeanMotionDtEqtide");
  sprintf(output[OUT_DMMDTEQTIDE].cDescr,"Total dMeanMotion/dt in EQTIDE");
  output[OUT_DMMDTEQTIDE].iNum = 1;
  fnWrite[OUT_DMMDTEQTIDE] = &WriteDMeanMotionDtEqtide;
  
  sprintf(output[OUT_DORBPERDTEQTIDE].cName,"DOrbPerDtEqtide");
  sprintf(output[OUT_DORBPERDTEQTIDE].cDescr,"Total dOrbPer/dt in EQTIDE");
  sprintf(output[OUT_DORBPERDTEQTIDE].cNeg,"days/Gyr");
  output[OUT_DORBPERDTEQTIDE].bNeg = 1;
  output[OUT_DORBPERDTEQTIDE].dNeg = YEARSEC*1e9/DAYSEC;
  output[OUT_DORBPERDTEQTIDE].iNum = 1;
  fnWrite[OUT_DORBPERDTEQTIDE] = &WriteDOrbPerDtEqtide;
  
  sprintf(output[OUT_DROTPERDTEQTIDE].cName,"DRotPerDtEqtide");
  sprintf(output[OUT_DROTPERDTEQTIDE].cDescr,"Time Rate of Change of Rotation Period in EQTIDE");
  sprintf(output[OUT_DROTPERDTEQTIDE].cNeg,"days/Myr");
  output[OUT_DROTPERDTEQTIDE].bNeg = 1;
  output[OUT_DROTPERDTEQTIDE].dNeg = DAYSEC/(YEARSEC*1e6);
  output[OUT_DROTPERDTEQTIDE].iNum = 1;
  fnWrite[OUT_DROTPERDTEQTIDE] = &WriteDRotPerDtEqtide;
  
  sprintf(output[OUT_DROTRATEDTEQTIDE].cName,"DRotRateDtEqtide");
  sprintf(output[OUT_DROTRATEDTEQTIDE].cDescr,"Time Rate of Change of Rotation Rate in EQTIDE");
  output[OUT_DROTRATEDTEQTIDE].bNeg = 0;
  output[OUT_DROTRATEDTEQTIDE].iNum = 1;
  fnWrite[OUT_DROTRATEDTEQTIDE] = &WriteDRotRateDtEqtide;

  sprintf(output[OUT_DHECCDTEQTIDE].cName,"DHEccDtEqtide");
  sprintf(output[OUT_DHECCDTEQTIDE].cDescr,"Time Rate of Change of Poincare's h in EQTIDE");
  sprintf(output[OUT_DHECCDTEQTIDE].cNeg,"/Gyr");
  output[OUT_DHECCDTEQTIDE].bNeg = 1;
  output[OUT_DHECCDTEQTIDE].dNeg = 1./(1e9*YEARSEC);
  output[OUT_DHECCDTEQTIDE].iNum = 1;
  fnWrite[OUT_DHECCDTEQTIDE] = &WriteDHeccDtEqtide;

  sprintf(output[OUT_DKECCDTEQTIDE].cName,"DKEccDtEqtide");
  sprintf(output[OUT_DKECCDTEQTIDE].cDescr,"Time Rate of Change of Poincare's k in EQTIDE");
  sprintf(output[OUT_DKECCDTEQTIDE].cNeg,"/Gyr");
  output[OUT_DKECCDTEQTIDE].bNeg = 1;
  output[OUT_DKECCDTEQTIDE].dNeg = 1./(1e9*YEARSEC);
  output[OUT_DKECCDTEQTIDE].iNum = 1;
  fnWrite[OUT_DKECCDTEQTIDE] = &WriteDKeccDtEqtide;

  sprintf(output[OUT_DXOBLDTEQTIDE].cName,"DXoblDtEqtide");
  sprintf(output[OUT_DXOBLDTEQTIDE].cDescr,"Time Rate of Change of Laskar's X in EQTIDE");
  sprintf(output[OUT_DXOBLDTEQTIDE].cNeg,"/Gyr");
  output[OUT_DXOBLDTEQTIDE].bNeg = 1;
  output[OUT_DXOBLDTEQTIDE].dNeg = 1./(1e9*YEARSEC);
  output[OUT_DXOBLDTEQTIDE].iNum = 1;
  fnWrite[OUT_DXOBLDTEQTIDE] = &WriteDXoblDtEqtide;

  sprintf(output[OUT_DYOBLDTEQTIDE].cName,"DYoblDtEqtide");
  sprintf(output[OUT_DYOBLDTEQTIDE].cDescr,"Time Rate of Change of Laskar's Y in EQTIDE");
  sprintf(output[OUT_DYOBLDTEQTIDE].cNeg,"/Gyr");
  output[OUT_DYOBLDTEQTIDE].bNeg = 1;
  output[OUT_DYOBLDTEQTIDE].dNeg = 1./(1e9*YEARSEC);
  output[OUT_DYOBLDTEQTIDE].iNum = 1;
  fnWrite[OUT_DYOBLDTEQTIDE] = &WriteDYoblDtEqtide;

  sprintf(output[OUT_DZOBLDTEQTIDE].cName,"DZoblDtEqtide");
  sprintf(output[OUT_DZOBLDTEQTIDE].cDescr,"Time Rate of Change of Laskar's Z in EQTIDE");
  sprintf(output[OUT_DZOBLDTEQTIDE].cNeg,"/Gyr");
  output[OUT_DZOBLDTEQTIDE].bNeg = 1;
  output[OUT_DZOBLDTEQTIDE].dNeg = 1./(1e9*YEARSEC);
  output[OUT_DZOBLDTEQTIDE].iNum = 1;
  fnWrite[OUT_DZOBLDTEQTIDE] = &WriteDZoblDtEqtide;

  /*
   * E
   */
  
  sprintf(output[OUT_ECCTIMEEQTIDE].cName,"EccTimeEqtide");
  sprintf(output[OUT_ECCTIMEEQTIDE].cDescr,"Timescale for Eccentricity Evolution (e/[de/dt]) in EQTIDE");
  sprintf(output[OUT_ECCTIMEEQTIDE].cNeg,"years");
  output[OUT_ECCTIMEEQTIDE].bNeg = 1;
  output[OUT_ECCTIMEEQTIDE].dNeg = 1./YEARSEC;
  output[OUT_ECCTIMEEQTIDE].iNum = 1;
  fnWrite[OUT_ECCTIMEEQTIDE] = &WriteEccTimescaleEqtide;
  
  sprintf(output[OUT_EQROTPER].cName,"EqRotPer");
  sprintf(output[OUT_EQROTPER].cDescr,"Equilibrium Rotation Period");
  sprintf(output[OUT_EQROTPER].cNeg,"days");
  output[OUT_EQROTPER].bNeg = 1;
  output[OUT_EQROTPER].dNeg = 1./DAYSEC;
  output[OUT_EQROTPER].iNum = 1;
  fnWrite[OUT_EQROTPER] = &WriteEqRotPer;
  
  sprintf(output[OUT_EQROTPERCONT].cName,"EqRotPerCont");
  sprintf(output[OUT_EQROTPERCONT].cDescr,"CPL2 Continuous Equilibrium Rotation Period");
  sprintf(output[OUT_EQROTPERCONT].cNeg,"days");
  output[OUT_EQROTPERCONT].bNeg = 1;
  output[OUT_EQROTPERCONT].dNeg = 1./DAYSEC;
  output[OUT_EQROTPERCONT].iNum = 1;
  fnWrite[OUT_EQROTPERCONT] = &WriteEqRotPerCont;
  
  sprintf(output[OUT_EQROTPERDISCRETE].cName,"EqRotPerDiscrete");
  sprintf(output[OUT_EQROTPERDISCRETE].cDescr,"CPL2 Discrete Equilibrium Spin Period");
  sprintf(output[OUT_EQROTPERDISCRETE].cNeg,"days");
  output[OUT_EQROTPERDISCRETE].bNeg = 1;
  output[OUT_EQROTPERDISCRETE].dNeg = 1./DAYSEC;
  output[OUT_EQROTPERDISCRETE].iNum = 1;
  fnWrite[OUT_EQROTPERDISCRETE] = &WriteEqRotPerDiscrete;
  
  sprintf(output[OUT_EQROTRATE].cName,"EqRotRate");
  sprintf(output[OUT_EQROTRATE].cDescr,"Equilibrium Rotation Rate");
  sprintf(output[OUT_EQROTRATE].cNeg,"/day"); 
  output[OUT_EQROTRATE].bNeg = 1;
  output[OUT_EQROTRATE].dNeg = DAYSEC;
  output[OUT_EQROTRATE].iNum = 1;
  fnWrite[OUT_EQROTRATE] = &WriteEqRotRate;
  
  sprintf(output[OUT_EQROTRATECONT].cName,"EqRotRateCont");
  sprintf(output[OUT_EQROTRATECONT].cDescr,"CPL2 Continuous Equilibrium Spin Rate");
  sprintf(output[OUT_EQROTRATECONT].cNeg,"/day");
  output[OUT_EQROTRATE].bNeg = 1;
  output[OUT_EQROTRATECONT].dNeg = DAYSEC;
  output[OUT_EQROTRATECONT].iNum = 1;
  fnWrite[OUT_EQROTRATECONT] = &WriteEqRotRateCont;
  
  sprintf(output[OUT_EQROTRATEDISCRETE].cName,"EqRotRateDiscrete");
  sprintf(output[OUT_EQROTRATEDISCRETE].cDescr,"CPL2 Discrete Equilibrium Spin Rate");
  sprintf(output[OUT_EQROTRATEDISCRETE].cNeg,"/day");
  output[OUT_EQROTRATEDISCRETE].bNeg = 1;
  output[OUT_EQROTRATEDISCRETE].dNeg = DAYSEC;
  output[OUT_EQROTRATEDISCRETE].iNum = 1;
  fnWrite[OUT_EQROTRATEDISCRETE] = &WriteEqRotRateDiscrete;
  
  /*
   * G
   */
  
  sprintf(output[OUT_GAMMAROT].cName,"GammaRot");
  sprintf(output[OUT_GAMMAROT].cDescr,"Gamma_Rotation");
  output[OUT_GAMMAROT].bNeg = 0;
  output[OUT_GAMMAROT].iNum = 1;
  fnWrite[OUT_GAMMAROT] = &WriteGammaRot;
  
  sprintf(output[OUT_GAMMAORB].cName,"GammaOrb");
  sprintf(output[OUT_GAMMAORB].cDescr,"Gamma_Orbital");
  output[OUT_GAMMAORB].bNeg = 0;
  output[OUT_GAMMAORB].iNum = 1;
  fnWrite[OUT_GAMMAORB] = &WriteGammaOrb;

  sprintf(output[OUT_IMK2].cName,"ImK2");
  sprintf(output[OUT_IMK2].cDescr,"Im(k_2)");
  output[OUT_IMK2].bNeg = 0;
  output[OUT_IMK2].iNum = 1;
  fnWrite[OUT_IMK2] = &WriteImK2;

  /*
   * O
   */
  
  sprintf(output[OUT_OBLTIMEEQTIDE].cName,"OblTimeEqtide");
  sprintf(output[OUT_OBLTIMEEQTIDE].cDescr,"Timescale for Obliquity Evolution in EQTIDE");
  sprintf(output[OUT_OBLTIMEEQTIDE].cNeg,"years");
  output[OUT_OBLTIMEEQTIDE].bNeg = 1;
  output[OUT_OBLTIMEEQTIDE].dNeg = 1./YEARSEC;
  output[OUT_OBLTIMEEQTIDE].iNum = 1;
  fnWrite[OUT_OBLTIMEEQTIDE] = &WriteOblTimescaleEqtide;
  
  /*
   * R
   */

  sprintf(output[OUT_ROTRATETIMEEQTIDE].cName,"RotTimeEqtide");
  sprintf(output[OUT_ROTRATETIMEEQTIDE].cDescr,"Timescale for Rotational Evolution in EQTIDE");
  output[OUT_ROTRATETIMEEQTIDE].bNeg = 0;
  sprintf(output[OUT_ROTRATETIMEEQTIDE].cNeg,"years");
  output[OUT_ROTRATETIMEEQTIDE].bNeg = 1;
  output[OUT_ROTRATETIMEEQTIDE].dNeg = 1./YEARSEC;
  output[OUT_ROTRATETIMEEQTIDE].iNum = 1;
  fnWrite[OUT_ROTRATETIMEEQTIDE] = &WriteRotTimescaleEqtide;
  
  sprintf(output[OUT_DROTPERDTEQTIDE].cName,"DRotPerDtEqtide");
  sprintf(output[OUT_DROTPERDTEQTIDE].cDescr,"Time Rate of Change of Rotation Period in EQTIDE");
  sprintf(output[OUT_DROTPERDTEQTIDE].cNeg,"days/Myr");
  output[OUT_DROTPERDTEQTIDE].bNeg = 1;
  output[OUT_DROTPERDTEQTIDE].dNeg = DAYSEC/(1e6*YEARSEC);
  output[OUT_DROTPERDTEQTIDE].iNum = 1;
  fnWrite[OUT_DROTPERDTEQTIDE] = &WriteDRotPerDtEqtide;
  
  sprintf(output[OUT_DROTRATEDTEQTIDE].cName,"DRotRateDtEqtide");
  sprintf(output[OUT_DROTRATEDTEQTIDE].cDescr,"Time Rate of Change of Rotational Frequency in EQTIDE");
  output[OUT_DROTRATEDTEQTIDE].bNeg = 0;
  output[OUT_DROTRATEDTEQTIDE].iNum = 1;
  fnWrite[OUT_DROTRATEDTEQTIDE] = &WriteDRotRateDtEqtide;
  
  /*
   * S
   */
  
  sprintf(output[OUT_SEMITIMEEQTIDE].cName,"SemiTimeEqtide");
  sprintf(output[OUT_SEMITIMEEQTIDE].cDescr,"Timescale for Semi-major Axis Evolution (a/[da/dt]) in EQTIDE");
  output[OUT_SEMITIMEEQTIDE].bNeg = 0;
  sprintf(output[OUT_SEMITIMEEQTIDE].cNeg,"years");
  output[OUT_SEMITIMEEQTIDE].bNeg = 1;
  output[OUT_SEMITIMEEQTIDE].dNeg = 1./YEARSEC;
  output[OUT_SEMITIMEEQTIDE].iNum = 1;
  fnWrite[OUT_SEMITIMEEQTIDE] = &WriteSemiTimescaleEqtide;
  
  /*
   * T
   */
  
  sprintf(output[OUT_TIDALTAU].cName,"TidalTau");
  sprintf(output[OUT_TIDALTAU].cDescr,"Tidal Time Lag");
  sprintf(output[OUT_TIDALTAU].cNeg,"sec");
  output[OUT_TIDALTAU].bNeg = 1;
  output[OUT_TIDALTAU].dNeg = 1;
  output[OUT_TIDALTAU].iNum = 1;
  fnWrite[OUT_TIDALTAU] = &WriteTidalTau;
  
  sprintf(output[OUT_ENFLUXEQTIDE].cName,"SurfEnFluxEqtide");
  sprintf(output[OUT_ENFLUXEQTIDE].cDescr,"Surface Energy Flux due to Tides in EQTIDE");
  sprintf(output[OUT_ENFLUXEQTIDE].cNeg,"W/m^2");
  output[OUT_ENFLUXEQTIDE].bNeg = 1;
  output[OUT_ENFLUXEQTIDE].dNeg = 1;
  output[OUT_ENFLUXEQTIDE].iNum = 1;
  fnWrite[OUT_ENFLUXEQTIDE] = &WriteEnergyFluxEqtide;
  
  sprintf(output[OUT_POWEREQTIDE].cName,"PowerEqtide");
  sprintf(output[OUT_POWEREQTIDE].cDescr,"Internal Power due to Tides in EQTIDE");
  sprintf(output[OUT_POWEREQTIDE].cNeg,"TW");
  output[OUT_POWEREQTIDE].bNeg = 1;
  output[OUT_POWEREQTIDE].dNeg = 1e-12;
  output[OUT_POWEREQTIDE].iNum = 1;
  fnWrite[OUT_POWEREQTIDE] = &WritePowerEqtide;
  
  sprintf(output[OUT_TIDELOCK].cName,"TideLock");
  sprintf(output[OUT_TIDELOCK].cDescr,"Tidally Locked?");
  output[OUT_TIDELOCK].bNeg = 0;
  output[OUT_TIDELOCK].iNum = 1;
  fnWrite[OUT_TIDELOCK] = &WriteTideLock;

}

/* Now assign output function pointers */

void FinalizeOutputFunctionEqtide(OUTPUT *output,int iBody,int iModule) {
  output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxEqtide;
}

/************ EQTIDE Logging Functions **************/

void LogOptionsEqtide(CONTROL *control, FILE *fp) {

  fprintf(fp,"-------- EQTIDE Options -----\n\n");
  /* Tidal Model */
  fprintf(fp,"Tidal Model: ");
  if (control->Evolve.iEqtideModel == CPL) {
    fprintf(fp,"Constant-Phase-Lag, 2nd order\n");
    fprintf(fp,"Use Discrete Rotation Rate Model: %d\n",control->Evolve.bDiscreteRot);
  } 

  if (control->Evolve.iEqtideModel == CTL) {
    fprintf(fp,"Constant-Time-Lag, 8th order\n");
  }
}

void LogEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  /* Cant this be cut? XXX */

  fprintf(fp,"\n----- EQTIDE PARAMETERS ------\n");
  for (iOut=OUTSTARTEQTIDE;iOut<OUTBODYSTARTEQTIDE;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }
}

void LogBodyEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut,iPert;

  fprintf(fp,"----- EQTIDE PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTEQTIDE;iOut<OUTENDEQTIDE;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
  fprintf(fp,"Tidal Perturbers:");
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    fprintf(fp," %s",body[body[iBody].iaTidePerts[iPert]].cName);
    if (iPert<body[iBody].iTidePerts-1)
      fprintf(fp,",");
  }
  fprintf(fp,"\n"); 
}

/************* MODULE Functions ***********/

void AddModuleEqtide(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = EQTIDE;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlEqtide;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyEqtide;
  module->fnCountHalts[iBody][iModule] = &CountHaltsEqtide;
  module->fnLogBody[iBody][iModule] = &LogBodyEqtide;

  module->fnReadOptions[iBody][iModule] = &ReadOptionsEqtide;
  module->fnVerify[iBody][iModule] = &VerifyEqtide;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltEqtide;
  module->fnVerifyRotation[iBody][iModule] = &VerifyRotationEqtide;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyEqtide;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateEqtide;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputEqtide;
  module->fnFinalizeUpdateHecc[iBody][iModule] = &FinalizeUpdateHeccEqtide;
  module->fnFinalizeUpdateKecc[iBody][iModule] = &FinalizeUpdateKeccEqtide;
  module->fnFinalizeUpdateRot[iBody][iModule] = &FinalizeUpdateRotEqtide;
  module->fnFinalizeUpdateSemi[iBody][iModule] = &FinalizeUpdateSemiEqtide;
  module->fnFinalizeUpdateXobl[iBody][iModule] = &FinalizeUpdateXoblEqtide;
  module->fnFinalizeUpdateYobl[iBody][iModule] = &FinalizeUpdateYoblEqtide;
  module->fnFinalizeUpdateZobl[iBody][iModule] = &FinalizeUpdateZoblEqtide;

  //module->fnInitializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionEqtide;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionEqtide;

}

/************* EQTIDE Functions ************/

double fdEqRotRate(BODY body,double dMeanMotion,double dEcc,int iTideModel,int bDiscreteRot) {
  
  if (iTideModel == CPL)
    return fdCPLEqRotRate(dEcc,dMeanMotion,bDiscreteRot);
  else if (iTideModel == CTL)
    return fdCTLEqRotRate(dEcc,body.dObliquity,dMeanMotion);

  /* Whoops! */
  assert(0);
}

void fdaChi(BODY *body,double dMeanMotion,double dSemi,int iBody,int iPert) {
  body[iBody].dTidalChi[iPert] = body[iBody].dRadGyra*body[iBody].dRadGyra*body[iBody].dRadius*body[iBody].dRadius*body[iBody].dRotRate*dSemi*dMeanMotion/(BIGG*body[iPert].dMass);
}

int fbTidalLock(BODY *body,EVOLVE *evolve,IO *io,int iBody,int iOrbiter) {
  double dEqRate,dDiff;

  dEqRate = fdEqRotRate(body[iBody],body[iOrbiter].dMeanMotion,body[iOrbiter].dEccSq,evolve->iEqtideModel,evolve->bDiscreteRot);

  dDiff = fabs(body[iBody].dRotRate - dEqRate)/dEqRate;

  if (dDiff < evolve->dMaxLockDiff[iBody]) {
    if (io->iVerbose >= VERBPROG) {
      printf("%s spin locked at ",body[iBody].cName);
      fprintd(stdout,evolve->dTime/YEARSEC,io->iSciNot,io->iDigits);
      printf(" years.\n");
    }
    return 1; /* Tidally locked */
  }
  /* Not tidally locked */
  return 0;
}      

/* Auxiliary properties required for the CPL calculations. N.B.: These 
   parameters also need to be included in BodyCopyEqtide!!! */

void PropsAuxOrbiter(BODY *body,UPDATE *update,int iBody) {
  body[iBody].dEccSq = body[iBody].dHecc*body[iBody].dHecc + body[iBody].dKecc*body[iBody].dKecc;
  body[iBody].dEcc = sqrt(body[iBody].dEccSq);
  // LongP is needed for Hecc and Kecc calculations
  body[iBody].dLongP = atan2(body[iBody].dHecc,body[iBody].dKecc);
  // PrecA is needed for Xobl,Yobl,Zobl calculations

  body[iBody].dDeccDtEqtide = fdCPLDeccDt(body,update,update[iBody].iaBody[update[iBody].iHecc][update[iBody].iHeccEqtide]);

}

void PropsAuxCPL(BODY *body,UPDATE *update,int iBody) {
  int iPert,iIndex;
  /* dMeanMotion claculated in PropsAuxGeneral */
  int iOrbiter;

  body[iBody].dObliquity = atan2(sqrt(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)),body[iBody].dZobl); //acos(body[iBody].dZobl);
  body[iBody].dPrecA = atan2(body[iBody].dYobl,body[iBody].dXobl);  

  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    iIndex = body[iBody].iaTidePerts[iPert];
    if (bPrimary(body,iIndex))
      iOrbiter = iBody;
    else
      iOrbiter = iIndex;
    
    fiaCPLEpsilon(body[iBody].dRotRate,body[iOrbiter].dMeanMotion,body[iBody].iTidalEpsilon[iIndex]);
    fdCPLZ(body,body[iOrbiter].dMeanMotion,body[iOrbiter].dSemi,iBody,iIndex);
    fdaChi(body,body[iOrbiter].dMeanMotion,body[iOrbiter].dSemi,iBody,iIndex);

    if (iBody > 0) 
      PropsAuxOrbiter(body,update,iBody);
  }

  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    iIndex = body[iBody].iaTidePerts[iPert];
    body[iBody].daDoblDtEqtide[iIndex] = fdCPLDoblDt(body,update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblEqtide[iIndex]]);
  }
}

void PropsAuxCTL(BODY *body,UPDATE *update,int iBody) {
  int iPert,iIndex;

  if (iBody == 0) {
    /* Central body can be perturbed by multiple bodies -- 
       should generalize for exomoons */

    for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
      iIndex = body[iBody].iaTidePerts[iPert];
      fdaCTLF(body,body[iIndex].dEccSq,iBody,iIndex);
      fdaCTLZ(body,body[iIndex].dSemi,iBody,iIndex);
      body[iBody].dTidalBeta[iIndex] = fdCTLBeta(body[iIndex].dEccSq);
      fdaChi(body,body[iIndex].dMeanMotion,body[iIndex].dSemi,iBody,iIndex);
    }
  } else { /* Orbiter */
    fdaCTLF(body,body[iBody].dEccSq,iBody,0);
    fdaCTLZ(body,body[iBody].dSemi,iBody,0);
    body[iBody].dTidalBeta[0] = fdCTLBeta(body[iBody].dEccSq);
    fdaChi(body,body[iBody].dMeanMotion,body[iBody].dSemi,iBody,0);
  }
}

double fdTidePower(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iTideModel) {

  /* Get total tidal power */
  if (iTideModel == CPL)
    return fdCPLTidePower(body,iBody);
  else if (iTideModel == CTL)
    return fdCTLTidePower(body,iBody);

  /* Whoops! */
  assert(0);
}

/* This is part of output[OUT_SURFENFLUX].fnOutput */
double fdSurfEnFluxEqtide(BODY *body,SYSTEM *foo,UPDATE *bar,int iBody,int iTideModel) {
  double dPower;

  dPower = fdTidePower(body,foo,bar,iBody,iTideModel); 
  
  return dPower/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
}

/*
 * Alter the simulation in a specific way. Possibilities are 
 * stored in the CONTROL struct. 
*/

void ForceBehaviorEqtide(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,int iBody,int iModule) {
  int iOrbiter;
  if (body[iBody].iTidePerts == 1) {
    /* Don't check for tidal locking if more than 1 tidal perturber. Maybe 
       change this later so the dominant perturber can lock it? */
      if (iBody > 0)
        iOrbiter = iBody;
      else
        iOrbiter = body[iBody].iaTidePerts[0];

    /* If tidally locked, assign equilibrium rotational frequency? */
    if (evolve->bForceEqSpin[iBody]) 
      body[iBody].dRotRate = fdEqRotRate(body[iBody],body[iOrbiter].dMeanMotion,body[iOrbiter].dEccSq,evolve->iEqtideModel,evolve->bDiscreteRot);

    /* Tidally Locked? */
    else
      evolve->bForceEqSpin[iBody] = fbTidalLock(body,evolve,io,iBody,iOrbiter);
  }

  /* If small enough, set some quantities to zero */
  /* Generalize! fnPropsAuxMinMax? */
  if (evolve->dMinValue > 0) {
    if (body[iBody].dEcc < evolve->dMinValue) {
      body[iBody].dHecc = 0;
      body[iBody].dKecc = 0;
    }
    /* XXX Need to sort out what Xobl, Yobl and Zobl should be
    if (body[iBody].dObliquity < control->Evolve.dMinValue)
      body[iBody].dObliquity = 0;
    }
    */
  }

}

/*
 ************************ CPL Functions ******************
*/

/* XXX Add intermediate parameters! They will be optimized away */

/* Auxiliary Parameters */

/* This is a member of output->fnSurfEnFlux, so must have appropriate
   input parameters.*/

double fdCPLTidePower(BODY *body,int iBody) {
  /* Heller, Leconte & Barnes (2011), Eqn 9-11. */
  int iPert,iOrbiter,iIndex;
  double dOrbPow=0,dRotPow=0;

  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    if (iBody == 0) 
      iOrbiter = body[iBody].iaTidePerts[iPert];
    else
      iOrbiter = iBody;
    iIndex = body[iBody].iaTidePerts[iPert];

    dOrbPow += -body[iBody].dTidalZ[iIndex]/8 * (4*body[iBody].iTidalEpsilon[iIndex][0] + body[iOrbiter].dEccSq*(-20*body[iBody].iTidalEpsilon[iIndex][0] + 147./2*body[iBody].iTidalEpsilon[iIndex][1] + 0.5*body[iBody].iTidalEpsilon[iIndex][2] - 3*body[iBody].iTidalEpsilon[iIndex][5]) - 4*sin(body[iBody].dObliquity)*sin(body[iBody].dObliquity)*(body[iBody].iTidalEpsilon[iIndex][0] - body[iBody].iTidalEpsilon[iIndex][8]));

    dRotPow += body[iBody].dTidalZ[iIndex]*body[iBody].dRotRate/(8*body[iOrbiter].dMeanMotion) * (4*body[iBody].iTidalEpsilon[iIndex][0] + body[iOrbiter].dEccSq*(-20*body[iBody].iTidalEpsilon[iIndex][0] + 49*body[iBody].iTidalEpsilon[iIndex][1] + body[iBody].iTidalEpsilon[iIndex][2]) + 2*sin(body[iBody].dObliquity)*sin(body[iBody].dObliquity)*(-2*body[iBody].iTidalEpsilon[iIndex][0] + body[iBody].iTidalEpsilon[iIndex][8] + body[iBody].iTidalEpsilon[iIndex][9]));
  }

  return dOrbPow + dRotPow;
}

double fdGammaRot(double dEccSq,double dPsi,int *epsilon) {
  return 4*epsilon[0] + dEccSq*(-20*epsilon[0] + 49*epsilon[1] + epsilon[2]) + 2*sin(dPsi)*sin(dPsi)*(-2*epsilon[0] + epsilon[8] + epsilon[9]);
}

double fdGammaOrb(double dEccSq,double dPsi,int *epsilon) {
    return 4*epsilon[0] + dEccSq*(-20*epsilon[0] + 147./2*epsilon[1] + 0.5*epsilon[2] - 3*epsilon[5]) - 4*sin(dPsi)*sin(dPsi)*(epsilon[0]-epsilon[8]);
}

void fiaCPLEpsilon(double dRotRate,double dMeanMotion,int *iEpsilon) {

  iEpsilon[0]=fiSign(2*dRotRate-2*dMeanMotion);
  iEpsilon[1]=fiSign(2*dRotRate-3*dMeanMotion);
  iEpsilon[2]=fiSign(2*dRotRate-dMeanMotion);
  iEpsilon[5]=fiSign(dMeanMotion);
  iEpsilon[8]=fiSign(dRotRate-2*dMeanMotion);
  iEpsilon[9]=fiSign(dRotRate);
  
}

void fdCPLZ(BODY *body,double dMeanMotion,double dSemi,int iBody,int iPert) {

  /* Note that this is different from Heller et al (2011) because 
     we now use Im(k_2) which equals k_2/Q. The value of Im(k_2) is set in
     VerifyEqtideTherming in module.c 
  */
  body[iBody].dTidalZ[iPert] = 3.*body[iBody].dImK2*BIGG*BIGG*body[iPert].dMass*body[iPert].dMass*(body[iBody].dMass+body[iPert].dMass)*pow(body[iBody].dRadius,5)/(pow(dSemi,9)*dMeanMotion); 
}

/*
 * Equilibrium Parameters
 */

double fdCPLTidePowerEq(double dTidalZ,double dEccSq,double dMeanMotion,double dObliquity,int bDiscrete) {
  /* Heller, Leconte  Barnes 2011, Eq. 13 */
  /* Needs to be added to InitializeOutput, etc XXX */
  double dGammaOrb,dGammaRot,dRotRateEq;
  int *iEpsilon;

  iEpsilon = malloc(9*sizeof(int));

  /* Must reset spin rate in order to get the phase lags correctly */

  dRotRateEq = fdCPLEqRotRate(dMeanMotion,dEccSq,bDiscrete);

  fiaCPLEpsilon(dRotRateEq,dMeanMotion,iEpsilon);
  
  dGammaRot = fdGammaRot(dEccSq,dObliquity,iEpsilon);
  dGammaOrb = fdGammaOrb(dEccSq,dObliquity,iEpsilon);

  /* return z/8 * ((om/dN)*grot - gorb); */
  return dTidalZ/8 * ((1+9.5*dEccSq)*dGammaRot - dGammaOrb);

  free(iEpsilon);
}

double fdCPLEqRotRateCont(double dMeanMotion,double dEccSq) {
  return (1 + 9.5*dEccSq)*dMeanMotion;
}

double fdCPLEqRotRateDiscrete(double dMeanMotion,double dEccSq) {
  if (dEccSq <= (1./19)) {
    return dMeanMotion;
  } else {
    return 1.5*dMeanMotion;
  }
}

double fdCPLEqRotRate(double dEccSq,double dMeanMotion,int bDiscrete) {

  if (bDiscrete)
    return fdCPLEqRotRateDiscrete(dMeanMotion,dEccSq);
  else 
    return fdCPLEqRotRateCont(dMeanMotion,dEccSq);
}

/*
 * Derivatives
 */

double fdCPLDsemiDt(BODY *body,SYSTEM *system,int *iaBody) {
  int iB0=iaBody[0],iB1=iaBody[1];

  /* This routine should only be called for the orbiters. iaBody[0] = the orbiter, iaBody[0] = central body */

  double dSum=0;

  // Contribution from orbiter
  dSum += body[iB0].dTidalZ[iB1]*(4*body[iB0].iTidalEpsilon[iB1][0] + body[iB0].dEccSq*(-20*body[iB0].iTidalEpsilon[iB1][0] + 147./2*body[iB0].iTidalEpsilon[iB1][1] + 0.5*body[iB0].iTidalEpsilon[iB1][2] - 3*body[iB0].iTidalEpsilon[iB1][5]) - 4*sin(body[iB0].dObliquity)*sin(body[iB0].dObliquity)*(body[iB0].iTidalEpsilon[iB1][0]-body[iB0].iTidalEpsilon[iB1][8]));

  // Contribution from central body
  dSum += body[iB1].dTidalZ[iB0]*(4*body[iB1].iTidalEpsilon[iB0][0] + body[iB0].dEccSq*(-20*body[iB1].iTidalEpsilon[iB0][0] + 147./2*body[iB1].iTidalEpsilon[iB0][1] + 0.5*body[iB1].iTidalEpsilon[iB0][2] - 3*body[iB1].iTidalEpsilon[iB0][5]) - 4*sin(body[iB1].dObliquity)*sin(body[iB1].dObliquity)*(body[iB1].iTidalEpsilon[iB0][0]-body[iB1].iTidalEpsilon[iB0][8]));
  
  return body[iB0].dSemi*body[iB0].dSemi/(4*BIGG*body[iB0].dMass*body[iB1].dMass)*dSum;
}

/* Hecc and Kecc calculated by chain rule, e.g. dh/dt = de/dt * dh/de. 
   XXX This should get moved to PropsAux. Then create a generic EqtideDHeccDt
   with body.dEccDt set by model ID in PropsAux. */

double fdCPLDeccDt(BODY *body,UPDATE *update,int *iaBody) {
  double dSum;
  int iB0=iaBody[0],iB1=iaBody[1];

  // Contribution from Central Body
  dSum = body[iB1].dTidalZ[iB0]*(2*body[iB1].iTidalEpsilon[iB0][0] - 49./2*body[iB1].iTidalEpsilon[iB0][1] + 0.5*body[iB1].iTidalEpsilon[iB0][2] + 3*body[iB1].iTidalEpsilon[iB0][5]);

  // Contribution from Orbiter
  dSum += body[iB0].dTidalZ[iB1]*(2*body[iB0].iTidalEpsilon[iB1][0] - 49./2*body[iB0].iTidalEpsilon[iB1][1] + 0.5*body[iB0].iTidalEpsilon[iB1][2] + 3*body[iB0].iTidalEpsilon[iB1][5]);
  
  return  -body[iB0].dSemi*sqrt(body[iB0].dEccSq)/(8*BIGG*body[iB0].dMass*body[iB1].dMass)*dSum;
}

double fdCPLDHeccDt(BODY *body,SYSTEM *system,int *iaBody) {
  /* This routine should only be called for the orbiters. 
     iaBody[0] = the orbiter, iaBody[0] = central body */

  return body[iaBody[0]].dDeccDtEqtide * sin(body[iaBody[0]].dLongP);
}  

double fdCPLDKeccDt(BODY *body,SYSTEM *system,int *iaBody) {
  /* This routine should only be called for the orbiters. 
     iaBody[0] = the orbiter, iaBody[0] = central body */
  
  return body[iaBody[0]].dDeccDtEqtide * cos(body[iaBody[0]].dLongP);
}  

double fdCPLDsemiDtBody(BODY body,double dMassPert,double dSemi,double dEcc) {
  double foo;

  /* XXX Not fixed */
  // foo = body.dTidalZ[0]*(4*body.iTidalEpsilon[0][0] + dEcc*dEcc*(-20*body.iTidalEpsilon[0][0] + 147./2*body.iTidalEpsilon[0][1] + 0.5*body.iTidalEpsilon[0][2] - 3*body.iTidalEpsilon[0][5]) - 4*sin(body.dObliquity)*sin(body.dObliquity)*(body.iTidalEpsilon[0][0]-body.iTidalEpsilon[0][8]));

  return -1;
  //return dSemi*dSemi/(4*BIGG*body.dMass*dMassPert)*foo;

}

double fdCPLDeccDtBody(BODY body,double dMassPert,double dSemi,double dEcc) {
  double foo;
 
  /* XXX Not fixed */
  //foo = body.dTidalZ*(2*body.iTidalEpsilon[0] - 49./2*body.iTidalEpsilon[1] + 0.5*body.iTidalEpsilon[2] + 3*body.iTidalEpsilon[5]);

  //return -dSemi*dEcc/(8*BIGG*body.dMass*dMassPert) * foo;
  return -1;
}  

double fdCPLDrotrateDt(BODY *body,SYSTEM *system,int *iaBody) {
  int iB0=iaBody[0],iB1=iaBody[1];

  /* Don't know if this is the central body or orbiter, but orbital
     info stored in body[iOrbter], so must figure this out. 
     Is there a faster way to do this? Note that forcing iaBody[0]
     to always be central body in VerifyCPL makes the semi and ecc
     derivatives need this determination. */
  int iOrbiter;

  if (bPrimary(body,iB0))
    iOrbiter = iB1;
  else
    iOrbiter = iB0;

  /* Note if tidally locked, ForceBehavior should fix the rotation
     rate and override this derivative. XXX This derivative should
     be removed from the update matrix in that case*/

  return -body[iB0].dTidalZ[iB1]/(8*body[iB0].dMass*body[iB0].dRadGyra*body[iB0].dRadGyra*body[iB0].dRadius*body[iB0].dRadius*body[iOrbiter].dMeanMotion)*(4*body[iB0].iTidalEpsilon[iB1][0] + body[iOrbiter].dEccSq*(-20*body[iB0].iTidalEpsilon[iB1][0] + 49*body[iB0].iTidalEpsilon[iB1][1] + body[iB0].iTidalEpsilon[iB1][2]) + 2*sin(body[iB0].dObliquity)*sin(body[iB0].dObliquity)*(-2*body[iB0].iTidalEpsilon[iB1][0]+body[iB0].iTidalEpsilon[iB1][8]+body[iB0].iTidalEpsilon[iB1][9]));
}

double fdCPLDoblDt(BODY *body,int *iaBody) {
  int iOrbiter,iB0=iaBody[0],iB1=iaBody[1];

  if (bPrimary(body,iB0))
    iOrbiter = iB1;
  else
    iOrbiter = iB0;

  return body[iB0].dTidalZ[iB1]*sin(body[iB0].dObliquity)/(4*body[iB0].dMass*body[iB0].dRadGyra*body[iB0].dRadGyra*body[iB0].dRadius*body[iB0].dRadius*body[iOrbiter].dMeanMotion*body[iB0].dRotRate) * (body[iB0].iTidalEpsilon[iB1][0]*(1-body[iB0].dTidalChi[iB1]) + (body[iB0].iTidalEpsilon[iB1][8]-body[iB0].iTidalEpsilon[iB1][9])*(1 + body[iB0].dTidalChi[iB1]));
}

double fdCPLDXoblDt(BODY *body,SYSTEM *system,int *iaBody) {

  return body[iaBody[0]].daDoblDtEqtide[iaBody[1]]*cos(body[iaBody[0]].dObliquity)*cos(body[iaBody[0]].dPrecA);
}

double fdCPLDYoblDt(BODY *body,SYSTEM *system,int *iaBody) {

  return body[iaBody[0]].daDoblDtEqtide[iaBody[1]]*cos(body[iaBody[0]].dObliquity)*sin(body[iaBody[0]].dPrecA);
}

double fdCPLDZoblDt(BODY *body,SYSTEM *system,int *iaBody) {

  return -body[iaBody[0]].daDoblDtEqtide[iaBody[1]]*sin(body[iaBody[0]].dObliquity);
}

/*
 ********************** CTL Functions ************************
*/

double fdCTLBeta(double dEcc) {
    return pow(1 - dEcc*dEcc,0.5);
}

double fdCTLF1(double dEcc) {
  return (1 + 31./2*dEcc*dEcc + 255./8*pow(dEcc,4) + 185./16*pow(dEcc,6) + 25./64*pow(dEcc,8));
}

double fdCTLF2(double dEcc) {
  return (1 + 15./2*dEcc*dEcc + 45./8*pow(dEcc,4) + 5./16*pow(dEcc,6));
}

double fdCTLF3(double dEcc) {
  return (1 + 15./4*dEcc*dEcc + 15./8*pow(dEcc,4) + 5./64*pow(dEcc,6));
}

double fdCTLF4(double dEcc) {
  return (1 + 1.5*dEcc*dEcc + 1./8*pow(dEcc,4));
}

double fdCTLF5(double dEcc) {
    return (1 + 3*dEcc*dEcc + 3./8*pow(dEcc,4));
}

void fdaCTLF(BODY *body,double dEcc,int iBody,int iPert) {

  body[iBody].dTidalF[iPert][0] = fdCTLF1(dEcc);
  body[iBody].dTidalF[iPert][1] = fdCTLF2(dEcc);
  body[iBody].dTidalF[iPert][2] = fdCTLF3(dEcc);
  body[iBody].dTidalF[iPert][3] = fdCTLF4(dEcc);
  body[iBody].dTidalF[iPert][4] = fdCTLF5(dEcc);
}

double fdCTLTidePower(BODY *body,int iBody) {
  int iPert,iOrbiter,iIndex;
  double dPower=0;

  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    if (iBody == 0) 
      iOrbiter = iPert;
    else
      iOrbiter = iBody;
    iIndex = body[iBody].iaTidePerts[iPert];

    dPower += body[iBody].dTidalZ[iIndex]*(body[iOrbiter].dTidalF[iIndex][0]/pow(body[iBody].dTidalBeta[iIndex],15) - 2*body[iBody].dTidalF[iIndex][1]*cos(body[iBody].dObliquity)*body[iBody].dRotRate/(pow(body[iBody].dTidalBeta[iIndex],12)*body[iOrbiter].dMeanMotion) + ((1+cos(body[iBody].dObliquity)*cos(body[iBody].dObliquity))/2)*body[iBody].dTidalF[iIndex][4]*body[iBody].dRotRate*body[iBody].dRotRate/(pow(body[iBody].dTidalBeta[iIndex],9)*body[iOrbiter].dMeanMotion*body[iOrbiter].dMeanMotion));

  }

  return dPower;
}

/*
 * Equilibrium Parameters
 */

double fdCTLTidePowerEq(BODY body,double dEcc) {

  return body.dTidalZ[0]/pow(body.dTidalBeta[0],15) * (body.dTidalF[0][0] - body.dTidalF[0][1]*body.dTidalF[0][1]/body.dTidalF[0][4] * (2*cos(body.dObliquity)*cos(body.dObliquity))/(1+cos(body.dObliquity)*cos(body.dObliquity)));
}

double fdCTLEqRotRate(double dEcc,double dObliquity,double dMeanMotion) {
  double dBeta,f2,f5;
  
  dBeta=fdCTLBeta(dEcc);
  f2=fdCTLF2(dEcc);
  f5=fdCTLF5(dEcc);
  
  return f2/(pow(dBeta,3)*f5) * 2*cos(dObliquity)/(1+cos(dObliquity)*cos(dObliquity)) * dMeanMotion;
}

void fdaCTLZ(BODY *body,double dSemi,int iBody,int iPert) {
  body[iBody].dTidalZ[iPert] = 3*BIGG*BIGG*body[iBody].dK2*body[iPert].dMass*body[iPert].dMass*(body[iBody].dMass+body[iPert].dMass)*pow(body[iBody].dRadius,5)*body[iBody].dTidalTau/pow(dSemi,9);
 }

/*
 * Derivatives
 */

double fdCTLDsemiDt(BODY *body,SYSTEM *system,int *iaBody) {
  /* This routine should only be called for the orbiters. iaBody[0] = the orbiter, iaBody[1] = central body */
  double dSum=0;

  /*

  int iBody;
  double dSum;
  // Broken XXX

  dSum=0;

//   for (iBody=0;iBody<iNumBodies;iBody++) 

      dSum += body[iBody].dTidalZ[0]*(cos(body[iBody].dObliquity)*body[1].dTidalF[0][1]*body[iBody].dRotRate/(pow(body[1].dTidalBeta[0],12)*body[1].dMeanMotion) - body[1].dTidalF[0][0]/pow(body[1].dTidalBeta[0],15));

  return 2*body[1].dSemi*body[1].dSemi/(BIGG*body[0].dMass*body[1].dMass)*dSum;
  */

  dSum=0;

  // Contribution from orbiter
  dSum += body[iaBody[0]].dTidalZ[iaBody[1]]*(cos(body[iaBody[0]].dObliquity)*body[iaBody[0]].dTidalF[iaBody[1]][1]*body[iaBody[0]].dRotRate/(pow(body[iaBody[0]].dTidalBeta[iaBody[1]],12)*body[iaBody[0]].dMeanMotion) - body[iaBody[0]].dTidalF[iaBody[1]][0]/pow(body[iaBody[0]].dTidalBeta[iaBody[1]],15));

  // Contribution from central body
  dSum += body[iaBody[1]].dTidalZ[iaBody[0]]*(cos(body[iaBody[1]].dObliquity)*body[iaBody[0]].dTidalF[iaBody[1]][1]*body[iaBody[1]].dRotRate/(pow(body[iaBody[0]].dTidalBeta[iaBody[1]],12)*body[iaBody[0]].dMeanMotion) - body[iaBody[0]].dTidalF[iaBody[1]][0]/pow(body[iaBody[0]].dTidalBeta[iaBody[1]],15));


  return 2*body[iaBody[0]].dSemi*body[iaBody[1]].dSemi/(BIGG*body[iaBody[0]].dMass*body[iaBody[1]].dMass)*dSum;
}

double fdCTLDeccDt(BODY *body,SYSTEM *system,int *iaBody) {
  /* This routine should only be called for the orbiters. iaBody[0] = the orbiter, iaBody[1] = central body */
  double dSum=0;

  /*
  int iBody;
  double dSum;

  // Broken
  dSum=0;
//   for (iBody=0;iBody<iNumBodies;iBody++) 
    dSum += body[iBody].dTidalZ[0]*(cos(body[iBody].dObliquity)*body[1].dTidalF[0][3]*body[iBody].dRotRate/(pow(body[1].dTidalBeta[0],10)*body[1].dMeanMotion) - 18*body[1].dTidalF[0][2]/(11*pow(body[1].dTidalBeta[0],13)));
  
  return 11*body[1].dSemi*body[1].dEcc/(2*BIGG*body[0].dMass*body[1].dMass)*dSum;
  */

  dSum=0;

  // Contribution from orbiter
  dSum += body[iaBody[0]].dTidalZ[iaBody[1]]*(cos(body[iaBody[0]].dObliquity)*body[iaBody[0]].dTidalF[iaBody[1]][3]*body[iaBody[0]].dRotRate/(pow(body[iaBody[0]].dTidalBeta[iaBody[1]],10)*body[iaBody[0]].dMeanMotion) - 18*body[iaBody[0]].dTidalF[iaBody[1]][2]/(11*pow(body[iaBody[0]].dTidalBeta[iaBody[1]],13)));

  // Contribution from central body
  dSum += body[iaBody[1]].dTidalZ[iaBody[0]]*(cos(body[iaBody[1]].dObliquity)*body[iaBody[0]].dTidalF[iaBody[1]][3]*body[iaBody[1]].dRotRate/(pow(body[iaBody[0]].dTidalBeta[iaBody[1]],10)*body[iaBody[0]].dMeanMotion) - 18*body[iaBody[0]].dTidalF[iaBody[1]][2]/(11*pow(body[iaBody[0]].dTidalBeta[iaBody[1]],13)));

  return 11*body[iaBody[0]].dSemi*sqrt(body[iaBody[0]].dEccSq)/(2*BIGG*body[iaBody[0]].dMass*body[iaBody[1]].dMass)*dSum;
}

double fdCTLDsemiDtBody(BODY body,double dMassPert,double dSemi,double dEcc,double dObliquity,double dRotRate) {
  // XXX Broken
  double foo,dMeanMotion,dBeta;

  foo = body.dTidalZ[0]*(cos(dObliquity)*body.dTidalF[0][1]*dRotRate/(pow(dBeta,12)*dMeanMotion) - body.dTidalF[0][0]/pow(dBeta,15));

  return 2*dSemi*dSemi/(BIGG*body.dMass*dMassPert)*foo;
}

double fdCTLDeccDtBody(BODY body,double dMassPert,double dSemi,double dEcc) {
  // XXX Broken
  double foo;
  double dMeanMotion,dBeta;

  foo = body.dTidalZ[0]*(cos(body.dObliquity)*body.dTidalF[0][3]*body.dRotRate/(pow(dBeta,10)*dMeanMotion) - 18*body.dTidalF[0][2]/(11*pow(dBeta,13)));
  
  return 11*dSemi*dEcc/(2*BIGG*dMassPert*body.dMass)*foo;
}

double fdCTLDrotrateDt(BODY *body,SYSTEM *system,int *iaBody) {
  /* Note if tidally locked, ForceBehavior will fix the rotation
     rate and override this derivative. XXX TBC */
  int iOrbiter;

  if (bPrimary(body,iaBody[0]))
    iOrbiter = iaBody[1];
  else
    iOrbiter = iaBody[0];

  return body[iaBody[0]].dTidalZ[iaBody[1]]/(2*body[iaBody[0]].dMass*body[iaBody[0]].dRadGyra*body[iaBody[0]].dRadGyra*body[iaBody[0]].dRadius*body[iaBody[0]].dRadius*body[iOrbiter].dMeanMotion) * (2*cos(body[iaBody[0]].dObliquity)*body[iaBody[0]].dTidalF[iaBody[1]][1]/pow(body[iaBody[0]].dTidalBeta[iaBody[1]],12) - (1+cos(body[iaBody[0]].dObliquity)*cos(body[iaBody[0]].dObliquity))*body[iaBody[0]].dTidalF[iaBody[1]][4]*body[iaBody[0]].dRotRate/(pow(body[iaBody[0]].dTidalBeta[iaBody[1]],9)*body[iOrbiter].dMeanMotion));
}


double fdCTLDobliquityDt(BODY *body,SYSTEM *system,int *iaBody) {
  /* Note if tidally locked, ForceBehavior will fix the rotation
     rate and override this derivative. XXX TBC */
  int iOrbiter;

  if (bPrimary(body,iaBody[0]))
    iOrbiter = iaBody[1];
  else
    iOrbiter = iaBody[0];


  int iBody=iaBody[0];

  return (body[iaBody[0]].dTidalZ[iaBody[1]]*sin(body[iaBody[0]].dObliquity))/(2*body[iaBody[0]].dMass*body[iaBody[0]].dRadGyra*body[iaBody[0]].dRadGyra*body[iaBody[0]].dRadius*body[iaBody[0]].dRadius*body[iOrbiter].dMeanMotion*body[iaBody[0]].dRotRate) * ((cos(body[iaBody[0]].dObliquity) - body[iaBody[0]].dTidalChi[iaBody[1]]/body[iaBody[0]].dTidalBeta[iaBody[1]])*body[iaBody[0]].dTidalF[iaBody[1]][4]*body[iaBody[0]].dRotRate/(pow(body[iaBody[0]].dTidalBeta[iaBody[1]],9)*body[iOrbiter].dMeanMotion) - 2*body[iaBody[0]].dTidalF[iaBody[1]][1]/pow(body[iaBody[0]].dTidalBeta[iaBody[1]],12));
}

