/********************** EQTIDE.C **********************/
/*
 * Rory Barnes, Wed May 29 14:56:21 PDT 2014
 *
 * Subroutines that control the integration of the tidal
 * model. Also includes subroutines that switch between
 * the two models.
*/


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
}

void InitializeModuleEqtide(CONTROL *control,MODULE *module) {
  /* Anything here? */
}

void BodyCopyEqtide(BODY *dest,BODY *src,int iTideModel,int iBody) {
  int iIndex,iPert;

  dest[iBody].dTidalZ = src[iBody].dTidalZ;
  dest[iBody].dTidalChi = src[iBody].dTidalChi;
  dest[iBody].iTidePerts = src[iBody].iTidePerts;

  for (iPert=0;iPert<src[iBody].iTidePerts;iPert++)
    dest[iBody].iaTidePerts[iPert] = src[iBody].iaTidePerts[iPert];

  if (iTideModel == CPL) {
    dest[iBody].dTidalQ = src[iBody].dTidalQ;
    for (iPert=0;iPert<src[iBody].iTidePerts;iPert++)
      for (iIndex=0;iIndex<10;iIndex++)
	dest[iBody].iTidalEpsilon[iPert][iIndex] = src[iBody].iTidalEpsilon[iPert][iIndex];
  }    
  if (iTideModel == CTL) {
    dest[iBody].dTidalTau = src[iBody].dTidalTau;
    dest[iBody].dTidalBeta = src[iBody].dTidalBeta;
    for (iIndex=0;iIndex<5;iIndex++)
      dest[iBody].dTidalF[iIndex] = src[iBody].dTidalF[iIndex];
  }
}

void InitializeBodyEqtide(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
  body[iBody].iaTidePerts = malloc(body[iBody].iTidePerts*sizeof(int));

}

void InitializeUpdateTmpBodyEqtide(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
  int iBodyPert;

  control->Evolve.tmpBody[iBody].iTidalEpsilon = malloc(control->Evolve.iNumBodies*sizeof(int*));
  control->Evolve.tmpBody[iBody].dTidalChi = malloc(control->Evolve.iNumBodies*sizeof(double));
  control->Evolve.tmpBody[iBody].dTidalZ = malloc(control->Evolve.iNumBodies*sizeof(double));
  
  control->Evolve.tmpBody[iBody].iaTidePerts = malloc(body[iBody].iTidePerts);

  for (iBodyPert=0;iBodyPert<control->Evolve.iNumBodies;iBodyPert++)
    control->Evolve.tmpBody[iBody].iTidalEpsilon[iBodyPert] = malloc(10*sizeof(int));
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

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0 || dTmp > 1) {
      if (control->Io.iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: %s must be in the range [0,1].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);	
    }
    control->Evolve.dSyncEcc[iFile] = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      control->Evolve.dSyncEcc[iFile] = options->dDefault;
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
    if (body[iFile-1].dTidalQ < 0) {
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
    //body[iFile-1].saTidePerts = malloc(iNumIndices*sizeof(char*));
    for (iBody=0;iBody<=iNumIndices;iBody++) {
      //body[iFile-1].saTidePerts[iBody] = malloc(NAMELEN);
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
  int iOpt,iFile;

  sprintf(options[OPT_DISCRETEROT].cName,"bDiscreteRot");
  sprintf(options[OPT_DISCRETEROT].cDescr,"Use Discrete Rotation Model (Phase lag only)");
  sprintf(options[OPT_DISCRETEROT].cDefault,"1");
  options[OPT_DISCRETEROT].iType = 0;
  fnRead[OPT_DISCRETEROT] = &ReadDiscreteRot;
  
  sprintf(options[OPT_FORCEEQSPIN].cName,"bForceEqSpin");
  sprintf(options[OPT_FORCEEQSPIN].cDescr,"Force Spin Rate to Equilibrium");
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

void ReadOptionsEqtide(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[]) {
  int iFile,iOpt;

  /* For each option, search the file for its occurence */
  /* is iLine necessary? */

  for (iFile=0;iFile<files->iNumInputs;iFile++) {
    /* First get eqtide options */
    for (iOpt=OPTSTARTEQTIDE;iOpt<OPTENDEQTIDE;iOpt++) 
      if (options[iOpt].iType != -1) {
	fnRead[iOpt](body,control,files,&options[iOpt],system,iFile);
      }
  }
}
    

/******************* Verify EQTIDE ******************/


void VerifyRotationEqtideWarning(char cName1[],char cName2[],char cFile[],int iLine1,int iLine2, int iVerbose) {
  if (iVerbose >= VERBINPUT) {
    fprintf(stderr,"WARNING: %s and %s are both set. Rotation rate will be synchronous.\n",cName1,cName2);
    fprintf(stderr,"File: %s, Lines %d and %d\n",cFile,iLine1,iLine2);
  }
}

void VerifyRotationEqtide(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody) {
  double dMeanMotion;
  
  if (options[OPT_FORCEEQSPIN].iLine[iBody] >= 0) {
    dMeanMotion=fdSemiToMeanMotion(body[1].dSemi,body[0].dMass+body[1].dMass);

    if (options[OPT_ROTPER].iLine[iBody] >= 0) 
      VerifyRotationEqtideWarning(options[OPT_FORCEEQSPIN].cName,options[OPT_ROTPER].cName,cFile,options[OPT_FORCEEQSPIN].iLine[iBody],options[OPT_ROTPER].iLine[iBody],control->Io.iVerbose);
      
    if (options[OPT_ROTRATE].iLine[iBody] >= 0) 
      VerifyRotationEqtideWarning(options[OPT_FORCEEQSPIN].cName,options[OPT_ROTRATE].cName,cFile,options[OPT_FORCEEQSPIN].iLine[iBody],options[OPT_ROTRATE].iLine[iBody],control->Io.iVerbose);

    if (options[OPT_ROTVEL].iLine[iBody] >= 0) 
      VerifyRotationEqtideWarning(options[OPT_FORCEEQSPIN].cName,options[OPT_ROTVEL].cName,cFile,options[OPT_FORCEEQSPIN].iLine[iBody],options[OPT_ROTVEL].iLine[iBody],control->Io.iVerbose);

    /* Done with warnings, do the assignment */
    body[iBody].dRotRate = fdEqRotRate(body[iBody],dMeanMotion,body[1].dEcc,control->Evolve.iEqtideModel,control->Evolve.bDiscreteRot);
  }
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
void InitializeOblEqtide(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iObl][update[iBody].iaOblEqtide[iPert]] = 1;
  update[iBody].padDoblDtEqtide[iPert] = &update[iBody].daDerivProc[update[iBody].iObl][update[iBody].iaOblEqtide[iPert]];
  update[iBody].iNumBodies[update[iBody].iObl][update[iBody].iaOblEqtide[iPert]]=2;
  update[iBody].iaBody[update[iBody].iObl][update[iBody].iaOblEqtide[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iObl][update[iBody].iaOblEqtide[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iObl][update[iBody].iaOblEqtide[iPert]][0]=iBody;
  update[iBody].iaBody[update[iBody].iObl][update[iBody].iaOblEqtide[iPert]][1] = iPert;
}

/*
void InitializeIntEnEqtide(UPDATE *update,BODY *body,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iIntEn][update[iBody].iaIntEnEqtide[iPert]] = 1;
  update[iBody].padDrotDtEqtide[iPert] = &update[iBody].daDerivProc[update[iBody].iIntEn][update[iBody].iaIntEnEqtide[iPert]];
  update[iBody].iNumBodies[update[iBody].iIntEn][update[iBody].iaIntEnEqtide[iPert]]=2;
  update[iBody].iaBody[update[iBody].iIntEn][update[iBody].iaIntEnEqtide[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iIntEn][update[iBody].iaIntEnEqtide[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iIntEn][update[iBody].iaIntEnEqtide[iPert]][0]=iBody;
  update[iBody].iaBody[update[iBody].iIntEn][update[iBody].iaIntEnEqtide[iPert]][1]=iBodyPert;
}
*/

void InitializeRotEqtide(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] = 1;
  update[iBody].padDrotDtEqtide[iPert] = &update[iBody].daDerivProc[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]];
  update[iBody].iNumBodies[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]]=2;
  update[iBody].iaBody[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]][0]=iBody;

  update[iBody].iaBody[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]][1] = iPert;
}

void InitializeEccEqtide(BODY *body,UPDATE *update,int iBody) {
  update[iBody].iaType[update[iBody].iEcc][update[iBody].iEccEqtide] = 1;
  update[iBody].pdDeccDtEqtide = &update[iBody].daDerivProc[update[iBody].iEcc][update[iBody].iEccEqtide];
  update[iBody].iNumBodies[update[iBody].iEcc][update[iBody].iEccEqtide]=2;
  update[iBody].iaBody[update[iBody].iEcc][update[iBody].iEccEqtide] = malloc(update[iBody].iNumBodies[update[iBody].iEcc][update[iBody].iEccEqtide]*sizeof(int));
  /* Ecc is stored in iBody, and the perturbing body is the central object,
     which is body 0 */
  update[iBody].iaBody[update[iBody].iEcc][update[iBody].iEccEqtide][0]=iBody;
  update[iBody].iaBody[update[iBody].iEcc][update[iBody].iEccEqtide][1]=0;
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
    update[iBody].padDoblDtEqtide = malloc(body[iBody].iTidePerts*sizeof(double*));
    InitializeOblEqtide(body,update,iBody,iPert);
    fnUpdate[iBody][update[iBody].iObl][update[iBody].iaOblEqtide[iPert]] = &fdCTLDobliquityDt;
    
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
    /* Semi-major axis */
    InitializeSemiEqtide(body,update,iBody);
    fnUpdate[iBody][update[iBody].iSemi][update[iBody].iSemiEqtide] = &fdCTLDsemiDt;
    
    /* Eccentricity */
    InitializeEccEqtide(body,update,iBody);
    fnUpdate[iBody][update[iBody].iEcc][update[iBody].iEccEqtide] = &fdCTLDeccDt;
  }

  control->Evolve.fnAuxProps[iBody][iModule]=&PropertiesCTL;
  }

void VerifyCPL(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int iPert,iTideFile,iCol,iFile;

  if (body[iBody].dEcc > pow(2./19,0.5) && control->Evolve.bDiscreteRot) {
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
    
    /* Obliquity */
    InitializeOblEqtide(body,update,iBody,iPert);
    fnUpdate[iBody][update[iBody].iObl][update[iBody].iaOblEqtide[iPert]] = &fdCPLDobliquityDt;
    
    /* Rotation Rate */
    InitializeRotEqtide(body,update,iBody,iPert);
    fnUpdate[iBody][update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] = &fdCPLDrotrateDt;
    
    /* Internal Energy
    InitializeIsotopeEqtide(body,update,iBody,iPert);
    fnUpdate[iBody][update[iBody].iIntEn][update[iBody].iaIntEnEqtide[iPert]] = &fdCPLDrotrateDt;
    */
  }  
  
  /* Is this the secondary body, and hence we assign da/dt and de/dt? */
  if (!bPrimary(body,iBody)) {
    /* Semi-major axis */
    InitializeSemiEqtide(body,update,iBody);
    fnUpdate[iBody][update[iBody].iSemi][update[iBody].iSemiEqtide] = &fdCPLDsemiDt;
    
    /* Eccentricity */
    InitializeEccEqtide(body,update,iBody);
    fnUpdate[iBody][update[iBody].iEcc][update[iBody].iEccEqtide] = &fdCPLDeccDt;
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
  control->Evolve.fnAuxProps[iBody][iModule]=&PropertiesCPL;

}

void VerifyPerturbersEqtide(BODY *body,FILES *files,OPTIONS *options,UPDATE *update,int iNumBodies,int iBody) {
  int iPert,iBodyPert,iVar,ok;
  int bFound[iNumBodies];

  /* Verify all arguments to saTidePerturbers. This subroutine will called 
     from each body using module eqtide, but we must make sure that each pair 
     of perturbing bodies points to each other, so we must loop through verify 
     all the bodies at the same time. This means all these lines will be 
     repeated for each tidally evolving body. But, if it's verified the first 
     time, it should verify every time! */

  for (iBody=0;iBody<iNumBodies;iBody++) {
    
    if (body[iBody].iTidePerts > 0) {
      for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
	bFound[iPert] = 0;
	for (iBodyPert=0;iBodyPert<iNumBodies;iBodyPert++) {
	  if (iBodyPert != iBody) {
	    if (memcmp(body[iBody].saTidePerts[iPert],body[iBodyPert].cName,sizeof(body[iBody].saTidePerts[iPert])) == 0) {
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
      
      if (!(body[body[iBody].iaTidePerts[iPert]].bEqtide)) {
	fprintf(stderr,"ERROR: Eqtide called for body %s, but option %s not set.\n",body[iBody].cName,options[OPT_TIDEPERTS].cName);
	ok=0;
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

int fiGetModuleIntEqtide(UPDATE *update,int iBody) {
  int iModule;

  for (iModule=0;iModule<update[iBody].iNumModules;iModule++) {
    if (update->iaModule[iModule] == EQTIDE)
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

  VerifyPerturbersEqtide(body,files,options,update,control->Evolve.iNumBodies,iBody);

  /* Verify input set correctly and assign update functions */
  if (control->Evolve.iEqtideModel == CTL)
    VerifyCTL(body,control,files,options,output,update,fnUpdate,iBody,iModule);
  
  if (control->Evolve.iEqtideModel == CPL)
    VerifyCPL(body,control,files,options,output,update,fnUpdate,iBody,iModule);

  body[iBody].dTidalZ=malloc(body[iBody].iTidePerts*sizeof(double));
  body[iBody].dTidalChi=malloc(body[iBody].iTidePerts*sizeof(double));
  control->fnForceBehavior[iBody][iModule]=&ForceBehaviorEqtide;
  control->Evolve.fnBodyCopy[iBody][iModule]=&BodyCopyEqtide;
}

/**************** EQTIDE Update ***************/

void InitializeUpdateEqtide(BODY *body,UPDATE *update,int iBody) {
  /* All active bodies can have their obliquity and rotation tracked. */
  update[iBody].iNumObl += body[iBody].iTidePerts;
  update[iBody].iNumRot += body[iBody].iTidePerts;;
  update[iBody].iNumVars += 2;
  
  /* Eqtide is complicated by the possibility that 1 body could be perturbed 
     by multiple others. saEqtidePertubers is the list of those bodies 
     which are tidally interacting with it, and body->iaTidePert contains
     the body #s which are the perturbers. For now, the higher body number
     always carries a and e. In other words, right now only 1 central body
     is allowed. The addition of exomoons will make this more complicated.
  */
  
  //if (!bPrimary(body,iBody)) {
  if (iBody > 0) {
    update[iBody].iNumSemi++;
    update[iBody].iNumEcc++;
    update[iBody].iNumVars += 2;
  }
}

void FinalizeUpdateEccEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
  update[iBody].iEccEqtide=*iEqn;
  (*iEqn)++;
}

/*
void FinalizeUpdateIntEnEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  int iPert;

   Multiple bodies could be tidally perturbing iBody. 
  update[iBody].padDIntEnDtEqtide = malloc(body[iBody].iTidePerts*sizeof(double*));
  update[iBody].iaIntEnEqtide = malloc(body[iBody].iTidePerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
    update[iBody].iaIntEnEqtide[iPert] = *iEqn;
    (*iEqn)++;
  }
}

*/

void FinalizeUpdateIsotopeEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateOblEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  int iPert;

  update[iBody].padDoblDtEqtide = malloc(body[iBody].iTidePerts*sizeof(double*));
  update[iBody].iaOblEqtide = malloc(body[iBody].iTidePerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
    update[iBody].iaOblEqtide[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateRotEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  int iPert;

  update[iBody].padDrotDtEqtide = malloc(body[iBody].iTidePerts*sizeof(double*));
  update[iBody].iaRotEqtide = malloc(body[iBody].iTidePerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
    update[iBody].iaRotEqtide[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateSemiEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
  update[iBody].iSemiEqtide = *iEqn;
  (*iEqn)++;
}

void FinalizeUpdate40KNumEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdate232ThNumEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdate238UNumEqtide(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
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
      fprintd(stdout,body[iBody].dEcc,io->iSciNot,io->iDigits);
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

void HelpOutputEqtide(OUTPUT *output) {
  int iOut;

  printf("\n ------ EQTIDE output ------\n");
  for (iOut=OUTSTARTEQTIDE;iOut<OUTENDEQTIDE;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

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
    *dTmp = fdCPLDsemiDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEcc);
  else if (control->Evolve.iEqtideModel == CTL) 
    *dTmp = fdCTLDsemiDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEcc,body[iBody].dObliquity,body[iBody].dRotRate);
		 
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

  /* XXX Broken */

  if (control->Evolve.iEqtideModel == CPL) 
    *dTmp = fdCPLDeccDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEcc);
  else
    *dTmp = fdCTLDeccDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEcc);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void WriteDOblDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dFoo;
  int iPert;

  /* Ensure that we don't overwrite pdDoblDt */
  dFoo = 0;
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) 
    dFoo += *(update[iBody].padDoblDtEqtide[iPert]);

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
  double dDeriv;
  
  /* Ensure that we don't overwrite pdDeccDt */
  dDeriv = *(update[iBody].pdDeccDtEqtide); 
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
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
    strcat(cUnit,"");
  }
}

void WriteDRotRateDtEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) 
    dDeriv = *(update[iBody].padDrotDtEqtide[iPert]);
  
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

/*
 * E
 */

void WriteEccTimescaleEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdTimescale(body[iBody].dEcc,*(update[iBody].pdDeccDtEqtide));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteEqRotPer(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdFreqToPer(fdEqRotRate(body[iBody],body[iBody].dMeanMotion,body[iBody].dEcc,control->Evolve.iEqtideModel,control->Evolve.bDiscreteRot));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }  else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteEqRotPerCont(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdFreqToPer(fdCPLEqRotRateCont(body[iBody].dMeanMotion,body[iBody].dEcc));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteEqRotPerDiscrete(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdFreqToPer(fdCPLEqRotRateDiscrete(body[iBody].dMeanMotion,body[iBody].dEcc));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
}

void WriteEqRotRate(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdEqRotRate(body[iBody],body[1].dMeanMotion,body[1].dEcc,control->Evolve.iEqtideModel,control->Evolve.bDiscreteRot);

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
  *dTmp = fdCPLEqRotRateCont(body[1].dMeanMotion,body[1].dEcc);

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
  *dTmp = fdCPLEqRotRateDiscrete(body[1].dMeanMotion,body[1].dEcc);

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

  *dTmp = fdGammaOrb(body[1].dEcc,body[iBody].dObliquity,body[iBody].iTidalEpsilon[0]);

  /* Negative option? */
  strcat(cUnit,"cgs");
}

void WriteGammaRot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  /* XXX Broken */

  *dTmp = fdGammaRot(body[1].dEcc,body[iBody].dObliquity,body[iBody].iTidalEpsilon[0]);

  /* Negative option? */
  strcat(cUnit,"cgs");
}

void WriteOblTimescaleEqtide(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdTimescaleMulti(body[iBody].dObliquity,*(update[iBody].padDoblDtEqtide),body[iBody].iTidePerts);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
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

void WriteTidalQ(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dTidalQ;
  strcat(cUnit,"");
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
  
  sprintf(output[OUT_OBLTIMEEQTIDE].cName,"OblTimeEqtide");
  sprintf(output[OUT_OBLTIMEEQTIDE].cDescr,"Timescale for Obliquity Evolution in EQTIDE");
  sprintf(output[OUT_OBLTIMEEQTIDE].cNeg,"years");
  output[OUT_OBLTIMEEQTIDE].bNeg = 1;
  output[OUT_OBLTIMEEQTIDE].dNeg = 1./YEARSEC;
  output[OUT_OBLTIMEEQTIDE].iNum = 1;
  fnWrite[OUT_OBLTIMEEQTIDE] = &WriteOblTimescaleEqtide;
  
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
  
  sprintf(output[OUT_TIDALQ].cName,"TidalQ");
  sprintf(output[OUT_TIDALQ].cDescr,"Tidal Q");
  output[OUT_TIDALQ].bNeg = 0;
  output[OUT_TIDALQ].iNum = 1;
  fnWrite[OUT_TIDALQ] = WriteTidalQ;
  
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
  output[OUT_ENFLUXEQTIDE].dNeg = 1e-3;
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
  module->fnFinalizeUpdateEcc[iBody][iModule] = &FinalizeUpdateEccEqtide;
  module->fnFinalizeUpdateObl[iBody][iModule] = &FinalizeUpdateOblEqtide;
  module->fnFinalizeUpdateRot[iBody][iModule] = &FinalizeUpdateRotEqtide;
  module->fnFinalizeUpdateSemi[iBody][iModule] = &FinalizeUpdateSemiEqtide;

  // Now include other primary variables not used by EQTIDE
  module->fnFinalizeUpdate40KNum[iBody][iModule] = &FinalizeUpdate40KNumEqtide;
  module->fnFinalizeUpdate232ThNum[iBody][iModule] = &FinalizeUpdate232ThNumEqtide;
  module->fnFinalizeUpdate238UNum[iBody][iModule] = &FinalizeUpdate238UNumEqtide;

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

  dEqRate = fdEqRotRate(body[iBody],body[iOrbiter].dMeanMotion,body[iOrbiter].dEcc,evolve->iEqtideModel,evolve->bDiscreteRot);

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

void PropertiesCPL(BODY *body,int iBody) {
  int iPert,iIndex;
  /* dMeanMotion claculated in PropsAuxGeneral */

  if (iBody == 0) {
    /* Central body can be perturbed by multiple bodies -- 
       should generalize for exomoons */

    for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
      iIndex = body[iBody].iaTidePerts[iPert];
      fiaCPLEpsilon(body[iBody].dRotRate,body[iIndex].dMeanMotion,body[iBody].iTidalEpsilon[iPert]);
      fdCPLZ(body,body[iIndex].dMeanMotion,body[iIndex].dSemi,iBody,iPert);
      fdaChi(body,body[iIndex].dMeanMotion,body[iIndex].dSemi,iBody,iPert);
    }
  } else { 
    fiaCPLEpsilon(body[iBody].dRotRate,body[iBody].dMeanMotion,body[iBody].iTidalEpsilon[0]);
    fdCPLZ(body,body[iBody].dMeanMotion,body[iBody].dSemi,iBody,0);
    fdaChi(body,body[iBody].dMeanMotion,body[iBody].dSemi,iBody,0);
  }
}

void PropertiesCTL(BODY *body,int iBody) {

  /* XXX Need more than 2 bodies */
  body[1].dMeanMotion = fdSemiToMeanMotion(body[1].dSemi,(body[0].dMass+body[1].dMass));

  fdaCTLZ(body);
  body[1].dTidalBeta[0] = fdCTLBeta(body[1].dEcc);
  fdaCTLF(&body[1]);
  fdaChi(&body[iBody],body[1].dMeanMotion,body[1].dSemi,body[0].dMass,0);
}

double fdTidePower(BODY *body,SYSTEM *foo,UPDATE *bar,int iBody,int iTideModel) {

  /* Get total tidal power */
  if (iTideModel == CPL)
    return fdCPLTidePower(body,foo,bar,iBody,iBody);
  else if (iTideModel == CTL)
    return fdCTLTidePower(*body,body[0].dMeanMotion,body[0].dEcc); // Hack!

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
 * Alter the simulation is a specific way. Possibilities are 
 * stored in the CONTROL struct. 
*/

void ForceBehaviorEqtide(BODY *body,EVOLVE *evolve,IO *io,int iBody,int iModule) {
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
      body[iBody].dRotRate = fdEqRotRate(body[iBody],body[iOrbiter].dMeanMotion,body[iOrbiter].dEcc,evolve->iEqtideModel,evolve->bDiscreteRot);

    /* Tidally Locked? */
    else
      evolve->bForceEqSpin[iBody] = fbTidalLock(body,evolve,io,iBody,iOrbiter);
  }
}

/*
 ************************ CPL Functions ******************
*/

/* Auxiliary Parameters */

/* This is a member of output->fnSurfEnFlux, so must have appropriate
   input parameters.*/

double fdCPLTidePower(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {

  /* Heller, Leconte & Barnes (2011), Eqn 9-11. */
  int iPert,iOrbiter;
  double dOrbE=0,dRotE=0;

  for (iPert=0;iPert<body[iBody].iTidePerts;iPert++) {
    if (iBody == 0) 
      iOrbiter = iPert;
    else
      iOrbiter = iBody;

    dOrbE += -body[iBody].dTidalZ[iPert]/8 * (4*body[iBody].iTidalEpsilon[iPert][0] + body[iOrbiter].dEcc*body[iOrbiter].dEcc*(-20*body[iBody].iTidalEpsilon[iPert][0] + 147./2*body[iBody].iTidalEpsilon[iPert][1] + 0.5*body[iBody].iTidalEpsilon[iPert][2] - 3*body[iBody].iTidalEpsilon[iPert][5]) - 4*sin(body[iBody].dObliquity)*sin(body[iBody].dObliquity)*(body[iBody].iTidalEpsilon[iPert][0] - body[iBody].iTidalEpsilon[iPert][8]));

    dRotE += body[iBody].dTidalZ[iPert]*body[iBody].dRotRate/(8*body[iOrbiter].dMeanMotion) * (4*body[iBody].iTidalEpsilon[iPert][0] + body[iOrbiter].dEcc*body[iOrbiter].dEcc*(-20*body[iBody].iTidalEpsilon[iPert][0] + 49*body[iBody].iTidalEpsilon[iPert][1] + body[iBody].iTidalEpsilon[iPert][2]) + 2*sin(body[iBody].dObliquity)*sin(body[iBody].dObliquity)*(-2*body[iBody].iTidalEpsilon[iPert][0] + body[iBody].iTidalEpsilon[iPert][8] + body[iBody].iTidalEpsilon[iPert][9]));
  }

  return dOrbE + dRotE;
}

double fdGammaRot(double dEcc,double dPsi,int *epsilon) {
  return 4*epsilon[0] + dEcc*dEcc*(-20*epsilon[0] + 49*epsilon[1] + epsilon[2]) + 2*sin(dPsi)*sin(dPsi)*(-2*epsilon[0] + epsilon[8] + epsilon[9]);
}

double fdGammaOrb(double dEcc,double dPsi,int *epsilon) {
    return 4*epsilon[0] + dEcc*dEcc*(-20*epsilon[0] + 147./2*epsilon[1] + 0.5*epsilon[2] - 3*epsilon[5]) - 4*sin(dPsi)*sin(dPsi)*(epsilon[0]-epsilon[8]);
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

  body[iBody].dTidalZ[iPert] = 3*BIGG*BIGG*body[iBody].dK2*body[iPert].dMass*body[iPert].dMass*(body[iBody].dMass+body[iPert].dMass)*pow(body[iBody].dRadius,5)/(pow(dSemi,9)*dMeanMotion*body[iBody].dTidalQ);
}


/*
 * Equilibrium Parameters
 */

double fdCPLTidePowerEq(double dTidalZ,double dEcc,double dMeanMotion,double dObliquity,int bDiscrete) {
  /* Heller, Leconte  Barnes 2011, Eq. 13 */
  double dGammaOrb,dGammaRot,dRotRateEq;
  int *iEpsilon;

  iEpsilon = malloc(9*sizeof(int));

  /* Must reset spin rate in order to get the phase lags correctly */

  dRotRateEq = fdCPLEqRotRate(dMeanMotion,dEcc,bDiscrete);

  fiaCPLEpsilon(dRotRateEq,dMeanMotion,iEpsilon);
  
  dGammaRot = fdGammaRot(dEcc,dObliquity,iEpsilon);
  dGammaOrb = fdGammaOrb(dEcc,dObliquity,iEpsilon);

  /* return z/8 * ((om/dN)*grot - gorb); */
  return dTidalZ/8 * ((1+9.5*dEcc*dEcc)*dGammaRot - dGammaOrb);

  free(iEpsilon);
}

double fdCPLEqRotRateCont(double n,double e) {
  return (1 + 9.5*e*e)*n;
}

double fdCPLEqRotRateDiscrete(double n,double e) {
  if (e <= pow(1./19,0.5)) {
    return n;
  } else {
    return 1.5*n;
  }
}

double fdCPLEqRotRate(double dEcc,double dMeanMotion,int bDiscrete) {

  if (bDiscrete)
    return fdCPLEqRotRateDiscrete(dMeanMotion,dEcc);
  else 
    return fdCPLEqRotRateCont(dMeanMotion,dEcc);
}

/*
 * Derivatives
 */

double fdCPLDsemiDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  /* This routine should only be called for the orbiters. iaBody[0] = the orbiter, iaBody[0] = central body */
  double dSum=0;

    /* Old sum
     dSum += body[iaBody[iBody]].dTidalZ*(4*body[iaBody[iBody]].iTidalEpsilon[0] + body[1].dEcc*body[1].dEcc*(-20*body[iaBody[iBody]].iTidalEpsilon[0] + 147./2*body[iaBody[iBody]].iTidalEpsilon[1] + 0.5*body[iaBody[iBody]].iTidalEpsilon[2] - 3*body[iaBody[iBody]].iTidalEpsilon[5]) - 4*sin(dPsi[iBody])*sin(dPsi[iBody])*(body[iaBody[iBody]].iTidalEpsilon[0]-body[iaBody[iBody]].iTidalEpsilon[8]));
    */

  // Contribution from orbiter
  dSum += body[iaBody[0]].dTidalZ[iaBody[1]]*(4*body[iaBody[0]].iTidalEpsilon[iaBody[1]][0] + body[iaBody[0]].dEcc*body[iaBody[0]].dEcc*(-20*body[iaBody[0]].iTidalEpsilon[iaBody[1]][0] + 147./2*body[iaBody[0]].iTidalEpsilon[iaBody[1]][1] + 0.5*body[iaBody[0]].iTidalEpsilon[iaBody[1]][2] - 3*body[iaBody[0]].iTidalEpsilon[iaBody[1]][5]) - 4*sin(body[iaBody[0]].dObliquity)*sin(body[iaBody[0]].dObliquity)*(body[iaBody[0]].iTidalEpsilon[iaBody[1]][0]-body[iaBody[0]].iTidalEpsilon[iaBody[1]][8]));

  // Contribution from central body
  dSum += body[iaBody[1]].dTidalZ[iaBody[0]]*(4*body[iaBody[1]].iTidalEpsilon[iaBody[0]][0] + body[iaBody[0]].dEcc*body[iaBody[0]].dEcc*(-20*body[iaBody[1]].iTidalEpsilon[iaBody[0]][0] + 147./2*body[iaBody[1]].iTidalEpsilon[iaBody[0]][1] + 0.5*body[iaBody[1]].iTidalEpsilon[iaBody[0]][2] - 3*body[iaBody[1]].iTidalEpsilon[iaBody[0]][5]) - 4*sin(body[iaBody[1]].dObliquity)*sin(body[iaBody[1]].dObliquity)*(body[iaBody[1]].iTidalEpsilon[iaBody[0]][0]-body[iaBody[1]].iTidalEpsilon[iaBody[0]][8]));
  
  return body[iaBody[0]].dSemi*body[iaBody[0]].dSemi/(4*BIGG*body[iaBody[0]].dMass*body[iaBody[1]].dMass)*dSum;
}

double fdCPLDeccDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  /* This routine should only be called for the orbiters. 
     iaBody[0] = the orbiter, iaBody[0] = central body */
  double dSum=0;

  /* old sum
    dSum += body[iaBody[iBody]].dTidalZ*(2*body[iaBody[iBody]].iTidalEpsilon[0] - 49./2*body[iaBody[iBody]].iTidalEpsilon[1] + 0.5*body[iaBody[iBody]].iTidalEpsilon[2] + 3*body[iaBody[iBody]].iTidalEpsilon[5]);
  */

  // Contribution from Central Body
  dSum += body[iaBody[1]].dTidalZ[iaBody[0]]*(2*body[iaBody[1]].iTidalEpsilon[iaBody[0]][0] - 49./2*body[iaBody[1]].iTidalEpsilon[iaBody[0]][1] + 0.5*body[iaBody[1]].iTidalEpsilon[iaBody[0]][2] + 3*body[iaBody[1]].iTidalEpsilon[iaBody[0]][5]);

  // Contribution from Orbiter
  dSum += body[iaBody[0]].dTidalZ[iaBody[1]]*(2*body[iaBody[0]].iTidalEpsilon[iaBody[0]][0] - 49./2*body[iaBody[0]].iTidalEpsilon[iaBody[1]][1] + 0.5*body[iaBody[0]].iTidalEpsilon[iaBody[1]][2] + 3*body[iaBody[0]].iTidalEpsilon[iaBody[1]][5]);
  
  return -body[iaBody[0]].dSemi*body[iaBody[0]].dEcc/(8*BIGG*body[iaBody[0]].dMass*body[iaBody[1]].dMass) * dSum;
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

double fdCPLDrotrateDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  /* Don't know if this is the central body or orbiter, but orbital
     info stored in body[iOrbter], so must figure this out. 
     Is there a faster way to do this? Note that forcing iaBody[0]
     to always be central body in VerifyCPL makes the semi and ecc
     derivatives need this determination. */
  int iOrbiter;

  if (bPrimary(body,iaBody[0]))
    iOrbiter = body[iaBody[0]].iaTidePerts[iaBody[1]];
  else
    iOrbiter = iaBody[0];

  /* Note if tidally locked, ForceBehavior should fix the rotation
     rate and override this derivative. XXX This derivative should
     be removed from the update matrix in that case*/

  return -body[iaBody[0]].dTidalZ[iaBody[1]]/(8*body[iaBody[0]].dMass*body[iaBody[0]].dRadGyra*body[iaBody[0]].dRadGyra*body[iaBody[0]].dRadius*body[iaBody[0]].dRadius*body[iOrbiter].dMeanMotion)*(4*body[iaBody[0]].iTidalEpsilon[iaBody[1]][0] + body[iOrbiter].dEcc*body[iOrbiter].dEcc*(-20*body[iaBody[0]].iTidalEpsilon[iaBody[1]][0] + 49*body[iaBody[0]].iTidalEpsilon[iaBody[1]][1] + body[iaBody[0]].iTidalEpsilon[iaBody[1]][2]) + 2*sin(body[iaBody[0]].dObliquity)*sin(body[iaBody[0]].dObliquity)*(-2*body[iaBody[0]].iTidalEpsilon[iaBody[1]][0]+body[iaBody[0]].iTidalEpsilon[iaBody[1]][8]+body[iaBody[0]].iTidalEpsilon[iaBody[1]][9]));
}

double fdCPLDobliquityDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  int iOrbiter;

  if (bPrimary(body,iaBody[0]))
    iOrbiter = body[iaBody[0]].iaTidePerts[iaBody[1]];
  else
    iOrbiter = iaBody[0];

  return body[iaBody[0]].dTidalZ[iaBody[1]]*sin(body[iaBody[0]].dObliquity)/(4*body[iaBody[0]].dMass*body[iaBody[0]].dRadGyra*body[iaBody[0]].dRadGyra*body[iaBody[0]].dRadius*body[iaBody[0]].dRadius*body[iOrbiter].dMeanMotion*body[iaBody[0]].dRotRate) * (body[iaBody[0]].iTidalEpsilon[iaBody[1]][0]*(1-body[iaBody[0]].dTidalChi[iaBody[1]]) + (body[iaBody[0]].iTidalEpsilon[iaBody[1]][8]-body[iaBody[0]].iTidalEpsilon[iaBody[1]][9])*(1 + body[iaBody[0]].dTidalChi[iaBody[1]]));
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

void fdaCTLF(BODY *body) {
  // hack XXX
  body->dTidalF[0][0] = fdCTLF1(body->dEcc);
  body->dTidalF[0][1] = fdCTLF2(body->dEcc);
  body->dTidalF[0][2] = fdCTLF3(body->dEcc);
  body->dTidalF[0][3] = fdCTLF4(body->dEcc);
  body->dTidalF[0][4] = fdCTLF5(body->dEcc);
}

double fdCTLTidePower(BODY body,double dMeanMotion,double dEcc) {

  body.dTidalBeta[0] = fdCTLBeta(dEcc);
  fdaCTLF(&body);

  // hack
  return body.dTidalZ[0]*(body.dTidalF[0][0]/pow(body.dTidalBeta[0],15) - 2*body.dTidalF[0][1]*cos(body.dObliquity)*body.dRotRate/(pow(body.dTidalBeta[0],12)*dMeanMotion) + ((1+cos(body.dObliquity)*cos(body.dObliquity))/2)*body.dTidalF[0][4]*body.dRotRate*body.dRotRate/(pow(body.dTidalBeta[0],9)*dMeanMotion*dMeanMotion));

}

/*
 * Equilibrium Parameters
 */

double fdCTLTidePowerEq(BODY body,double dEcc) {
  body.dTidalBeta[0] = fdCTLBeta(dEcc);
  fdaCTLF(&body);

  return body.dTidalZ[0]/pow(body.dTidalBeta[0],15) * (body.dTidalF[0][0] - body.dTidalF[0][1]*body.dTidalF[0][1]/body.dTidalF[0][4] * (2*cos(body.dObliquity)*cos(body.dObliquity))/(1+cos(body.dObliquity)*cos(body.dObliquity)));
}

double fdCTLEqRotRate(double dEcc,double dObliquity,double dMeanMotion) {
  double dBeta,f2,f5;
  
  dBeta=fdCTLBeta(dEcc);
  f2=fdCTLF2(dEcc);
  f5=fdCTLF5(dEcc);
  
  return f2/(pow(dBeta,3)*f5) * 2*cos(dObliquity)/(1+cos(dObliquity)*cos(dObliquity)) * dMeanMotion;
}

void fdaCTLZ(BODY *body) {
  body[0].dTidalZ[0] = 3*BIGG*BIGG*body[0].dK2*body[1].dMass*body[1].dMass*(body[0].dMass+body[1].dMass)*pow(body[0].dRadius,5)*body[0].dTidalTau/pow(body[1].dSemi,9);
  body[1].dTidalZ[0] = 3*BIGG*BIGG*body[1].dK2*body[0].dMass*body[0].dMass*(body[0].dMass+body[1].dMass)*pow(body[1].dRadius,5)*body[1].dTidalTau/pow(body[1].dSemi,9);
}

/*
 * Derivatives
 */

double fdCTLDsemiDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  int iBody;
  double dSum;
  // Broken XXX

  dSum=0;
  for (iBody=0;iBody<iNumBodies;iBody++) 
    /* XXX Sum the body functions? */
      dSum += body[iBody].dTidalZ[0]*(cos(body[iBody].dObliquity)*body[1].dTidalF[0][1]*body[iBody].dRotRate/(pow(body[1].dTidalBeta[0],12)*body[1].dMeanMotion) - body[1].dTidalF[0][0]/pow(body[1].dTidalBeta[0],15));

  return 2*body[1].dSemi*body[1].dSemi/(BIGG*body[0].dMass*body[1].dMass)*dSum;
}

double fdCTLDeccDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  int iBody;
  double dSum;

  // Broken
  dSum=0;
  for (iBody=0;iBody<iNumBodies;iBody++) 
    dSum += body[iBody].dTidalZ[0]*(cos(body[iBody].dObliquity)*body[1].dTidalF[0][3]*body[iBody].dRotRate/(pow(body[1].dTidalBeta[0],10)*body[1].dMeanMotion) - 18*body[1].dTidalF[0][2]/(11*pow(body[1].dTidalBeta[0],13)));
  
  return 11*body[1].dSemi*body[1].dEcc/(2*BIGG*body[0].dMass*body[1].dMass)*dSum;
}

double fdCTLDsemiDtBody(BODY body,double dMassPert,double dSemi,double dEcc,double dObliquity,double dRotRate) {
  double foo,dMeanMotion,dBeta;

  dMeanMotion=fdSemiToMeanMotion(dSemi,(body.dMass+dMassPert));
  dBeta=fdCTLBeta(dEcc);
  body.dEcc=dEcc;
  fdaCTLF(&body);
  foo = body.dTidalZ[0]*(cos(dObliquity)*body.dTidalF[0][1]*dRotRate/(pow(dBeta,12)*dMeanMotion) - body.dTidalF[0][0]/pow(dBeta,15));

  return 2*dSemi*dSemi/(BIGG*body.dMass*dMassPert)*foo;
}

double fdCTLDeccDtBody(BODY body,double dMassPert,double dSemi,double dEcc) {
  double foo;
  double dMeanMotion,dBeta;

  dMeanMotion = fdSemiToMeanMotion(dSemi,(body.dMass+dMassPert));
  dBeta=fdCTLBeta(dEcc);
  body.dEcc=dEcc;
  fdaCTLF(&body);
  foo = body.dTidalZ[0]*(cos(body.dObliquity)*body.dTidalF[0][3]*body.dRotRate/(pow(dBeta,10)*dMeanMotion) - 18*body.dTidalF[0][2]/(11*pow(dBeta,13)));
  
  return 11*dSemi*dEcc/(2*BIGG*dMassPert*body.dMass)*foo;
}

double fdCTLDrotrateDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  /* Note if tidally locked, ForceBehavior will fix the rotation
     rate and override this derivative. */
  int iBody=iaBody[0];

  return body[iBody].dTidalZ[0]/(2*body[iBody].dMass*body[iBody].dRadGyra*body[iBody].dRadGyra*body[iBody].dRadius*body[iBody].dRadius*body[1].dMeanMotion) * (2*cos(body[iBody].dObliquity)*body[1].dTidalF[0][1]/pow(body[1].dTidalBeta[0],12) - (1+cos(body[iBody].dObliquity)*cos(body[iBody].dObliquity))*body[1].dTidalF[0][4]*body[iBody].dRotRate/(pow(body[1].dTidalBeta[0],9)*body[1].dMeanMotion));
}

double fdCTLDobliquityDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  int iBody=iaBody[0];

  return (body[iBody].dTidalZ[0]*sin(body[iBody].dObliquity))/(2*body[iBody].dMass*body[iBody].dRadGyra*body[iBody].dRadGyra*body[iBody].dRadius*body[iBody].dRadius*body[1].dMeanMotion*body[iBody].dRotRate) * ((cos(body[iBody].dObliquity) - body[iBody].dTidalChi[1]/body[1].dTidalBeta[0])*body[1].dTidalF[0][4]*body[iBody].dRotRate/(pow(body[1].dTidalBeta[0],9)*body[1].dMeanMotion) - 2*body[1].dTidalF[0][1]/pow(body[1].dTidalBeta[0],12));
}

