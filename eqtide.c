/********************** EQTIDE.C **********************/
/*
 * Rory Barnes, Wed May 29 14:56:21 PDT 2014
 *
 * Subroutines that control the integration of the tidal
 * model. Also includes subroutines that switch between
 * the two models.
*/

/* Testing changes */

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"

void  InitializeControlEqtide(CONTROL *control) {

  control->bForceEqSpin=malloc(control->iNumBodies*sizeof(int));
  control->dMaxLockDiff=malloc(control->iNumBodies*sizeof(double));
  control->dSyncEcc=malloc(control->iNumBodies*sizeof(double));
}


/**************** EQTIDE options ********************/

void HelpOptionsEqtide(OPTIONS *options) {
  int i;

  printf("\n ------ EQTIDE options ------\n");
  for (i=OPTSTARTEQTIDE;i<OPTENDEQTIDE;i++) 
    WriteHelpOption(options[i]);
}

/* Discrete Rotation */

void ReadDiscreteRot(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    control->Integr.bDiscreteRot = bTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    /* Set to default */
    AssignDefaultInt(&control->Integr.bDiscreteRot,*options,files->iNumInputs);
}

/* Double Synchronous */

void ReadHaltDblSync(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    control->Halt.bDblSync = bTmp;
    control->Halt.bHalt = 1;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultInt(&control->Halt.bDblSync,*options,files->iNumInputs); 
}

/* Force Equilibrium Spin Rate? */

void ReadForceEqSpin(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* Cannot exist in primary input file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,options->iLine[iNumFile],control->iVerbose);
    control->bForceEqSpin[iNumFile-1] = bTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    if (iNumFile > 0)
      /* Set to default */
      AssignDefaultInt(&control->bForceEqSpin[iNumFile-1],*options,files->iNumInputs);
}

/* Primary Tide-locked */

void ReadHaltTideLock(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    control->Halt.bTideLock[iNumFile-1] = bTmp;
    control->Halt.bHalt = 1;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      control->Halt.bTideLock[iNumFile-1] = atoi(options->cDefault);
}

/* Halt when Synchronous Rotation */

void ReadHaltSyncRot(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file */
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    control->Halt.bSync[iNumFile-1] = bTmp;
    control->Halt.bHalt = 1;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    if (iNumFile > 0)
      control->Halt.bSync[iNumFile-1] = atoi(options->cDefault);
}

/* k_2 - Love Number of degree 2 */

void ReadK2(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    body[iNumFile-1].dK2 = dTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dK2 = options->dDefault;
}

/* Maximum allowable offset between primary's spin period and its
   equilibrium spin period. */

void ReadMaxLockDiff(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) {
      if (control->iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: %s must be > 0.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    control->dMaxLockDiff[iNumFile-1] = dTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      control->dMaxLockDiff[iNumFile-1] = options->dDefault;
}

/* Maximum Eccentricity for Synchronous Rotation */

void ReadSyncEcc(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0 || dTmp > 1) {
      if (control->iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: %s must be in the range [0,1].\n",options->cName);
        LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    control->dSyncEcc[iNumFile] = dTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      control->dSyncEcc[iNumFile] = options->dDefault;
}    

/* Tidal Q */

void ReadTidalQ(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (body[iNumFile-1].dTidalQ < 0) {
      if (control->iVerbose >= VERBERR)
	  fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }

    body[iNumFile-1].dTidalQ = dTmp; 
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dTidalQ = options->dDefault;
}  

/* Time lag */

void ReadTidalTau(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) 
      body[iNumFile-1].dTidalTau = dTmp*dNegativeDouble(*options,files->Infile[iNumFile].cIn,control->iVerbose);
    else
      /* Convert timestep to cgs */
      body[iNumFile-1].dTidalTau = dTmp*fdTimeUnit(control->Units[iNumFile].iTime);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    if (iNumFile > 0)
      body[iNumFile-1].dTidalTau = options->dDefault;
}


/* Tidal Model */

void ReadTideModel(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  char cTmp[OPTLEN];

  /* Tide Model, use #defined variables */

  AddOptionString(files->Infile[iNumFile].cIn,options->cName,cTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (!memcmp(sLower(cTmp),"p2",2)) {
      control->iTideModel = CPL;
    } else if (!memcmp(sLower(cTmp),"t8",2)) {
      control->iTideModel=CTL;
    } else {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are p2 or t8.\n",options->cName,cTmp);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  }
}

void InitializeOptionsEqtide(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  for (iOpt=OPTSTARTEQTIDE;iOpt<OPTENDEQTIDE;iOpt++) {
    sprintf(options[iOpt].cName,"null");
    options[iOpt].iLine = malloc(MAXFILES*sizeof(int));
    options[iOpt].iMultiFile=0;
    options[iOpt].iMultiIn=0;
    options[iOpt].iType = -1;
    
    for (iFile=0;iFile<MAXFILES;iFile++) {
      options[iOpt].iLine[iFile] = -1;
      sprintf(options[iOpt].cFile[iFile],"null");
    }
  }

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
  
  sprintf(options[OPT_HALTLOCK].cName,"bHaltTideLock");
  sprintf(options[OPT_HALTLOCK].cDescr,"Halt if Tide-Locked?");
  sprintf(options[OPT_HALTLOCK].cDefault,"0");
  options[OPT_HALTLOCK].iType = 0;
  options[OPT_HALTLOCK].iMultiFile = 1;
  fnRead[OPT_HALTLOCK] = &ReadHaltTideLock;
  
  sprintf(options[OPT_HALTSYNC].cName,"bHaltSync");
  sprintf(options[OPT_HALTSYNC].cDescr,"Halt if Secondary's rotation becomes syncrhonous?");
  sprintf(options[OPT_HALTSYNC].cDefault,"0");
  options[OPT_HALTSYNC].iType = 0;
  options[OPT_HALTSYNC].iMultiFile = 1;
  fnRead[OPT_HALTSYNC] = &ReadHaltSyncRot;
  
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
}

void ReadOptionsEqtide(CONTROL *control,OPTIONS *options,BODY *body,SYSTEM *system,FILES *files,fnReadOption fnRead[]) {
  int iFile,iOpt;

  /* For each option, search the file for its occurence */
  /* is iLine necessary? */

  for (iFile=0;iFile<files->iNumInputs;iFile++) {
    /* First get eqtide options */
    for (iOpt=OPTSTARTEQTIDE;iOpt<OPTENDEQTIDE;iOpt++) 
      if (options[iOpt].iType != -1) {
	fnRead[iOpt](control,body,system,&options[iOpt],files,iFile);
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

void VerifyRotationEqtide(CONTROL *control,BODY *body,OPTIONS *options,char cFile[],int iBody) {
  double dMeanMotion;
  
  if (options[OPT_FORCEEQSPIN].iLine[iBody] >= 0) {
    dMeanMotion=fdSemiToMeanMotion(body[1].dSemi,body[0].dMass+body[1].dMass);

    if (options[OPT_ROTPER].iLine[iBody] >= 0) 
      VerifyRotationEqtideWarning(options[OPT_FORCEEQSPIN].cName,options[OPT_ROTPER].cName,cFile,options[OPT_FORCEEQSPIN].iLine[iBody],options[OPT_ROTPER].iLine[iBody],control->iVerbose);
      
    if (options[OPT_ROTRATE].iLine[iBody] >= 0) 
      VerifyRotationEqtideWarning(options[OPT_FORCEEQSPIN].cName,options[OPT_ROTRATE].cName,cFile,options[OPT_FORCEEQSPIN].iLine[iBody],options[OPT_ROTRATE].iLine[iBody],control->iVerbose);

    if (options[OPT_ROTVEL].iLine[iBody] >= 0) 
      VerifyRotationEqtideWarning(options[OPT_FORCEEQSPIN].cName,options[OPT_ROTVEL].cName,cFile,options[OPT_FORCEEQSPIN].iLine[iBody],options[OPT_ROTVEL].iLine[iBody],control->iVerbose);

    /* Done with warnings, do the assignment */
    body[iBody].dRotRate = fdEqRotRate(body[iBody],dMeanMotion,body[1].dEcc,control->iTideModel,control->Integr.bDiscreteRot);
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

void InitializeCTL(UPDATE *update,BODY *body) {
  
  /* Body #0 updates */
  update[0].iNum = 2;
  InitializeUpdate(&update[0]);
  
  /* 0 -> Obliquity */
  update[0].iType[0] = 1;
  update[0].pdVar[0] = &body[0].dObliquity;
  update[0].iNumEqns[0] = 1;
  update[0].dDeriv[0]=malloc(update[0].iNumEqns[0]*sizeof(double));
  
  /* 1 -> Rotation Rate */
  update[0].iType[1] = 1;
  update[0].pdVar[1] = &body[0].dRotRate;
  update[0].iNumEqns[1] = 1;
  update[0].dDeriv[1]=malloc(update[0].iNumEqns[1]*sizeof(double));
  
  /* Body #1 updates */
  update[1].iNum = 4;
  InitializeUpdate(&update[1]);
  
  /* 0 -> Obliquity */
  update[1].iType[0] = 1;
  update[1].pdVar[0] = &body[1].dObliquity;
  update[1].iNumEqns[0] = 1;
  update[1].dDeriv[0]=malloc(update[1].iNumEqns[0]*sizeof(double));
  
  /* 1 -> Rotation Rate */
  update[1].iType[1] = 1;
  update[1].pdVar[1] = &body[1].dRotRate;
  update[1].iNumEqns[1] = 1;
  update[1].dDeriv[1]=malloc(update[1].iNumEqns[1]*sizeof(double));
  
  /* 2 -> Semi-major axis */
  update[1].iType[2] = 1;
  update[1].pdVar[2] = &body[1].dSemi;
  update[1].iNumEqns[2] = 1;
  update[1].dDeriv[2]=malloc(update[1].iNumEqns[2]*sizeof(double));
  
  /* 3 -> Eccentricity */
  update[1].iType[3] = 1;
  update[1].pdVar[3] = &body[1].dEcc;
  update[1].iNumEqns[3] = 1;
  update[1].dDeriv[3]=malloc(update[1].iNumEqns[3]*sizeof(double));
}

void InitializeCPL(UPDATE *update,BODY *body) {
  
  /* Body #0 updates */
  update[0].iNum = 2;
  InitializeUpdate(&update[0]);
  
  /* 0 -> Obliquity */
  update[0].iType[0] = 1;
  update[0].pdVar[0] = &body[0].dObliquity;
  update[0].iNumEqns[0] = 1;
  update[0].dDeriv[0]=malloc(update[0].iNumEqns[0]*sizeof(double));
  
  /* 1 -> Rotation Rate */
  update[0].iType[1] = 1;
  update[0].pdVar[1] = &body[0].dRotRate;
  update[0].iNumEqns[1] = 1;
  update[0].dDeriv[1]=malloc(update[0].iNumEqns[1]*sizeof(double));
  
  /* Body #1 updates */
  update[1].iNum = 4;
  InitializeUpdate(&update[1]);
  
  /* 0 -> Obliquity */
  update[1].iType[0] = 1;
  update[1].pdVar[0] = &body[1].dObliquity;
  update[1].iNumEqns[0] = 1;
  update[1].dDeriv[0]=malloc(update[1].iNumEqns[0]*sizeof(double));
  
  /* 1 -> Rotation Rate */
  update[1].iType[1] = 1;
  update[1].pdVar[1] = &body[1].dRotRate;
  update[1].iNumEqns[1] = 1;
  update[1].dDeriv[1]=malloc(update[1].iNumEqns[1]*sizeof(double));
  
  /* 2 -> Semi-Major Axis */
  update[1].iType[2] = 1;
  update[1].pdVar[2] = &body[1].dSemi;
  update[1].iNumEqns[2] = 1;
  update[1].dDeriv[2]=malloc(update[1].iNumEqns[2]*sizeof(double));
  
  /* 3 -> Eccentricity */
  update[1].iType[3] = 1;
  update[1].pdVar[3] = &body[1].dEcc;
  update[1].iNumEqns[3] = 1;
  update[1].dDeriv[3]=malloc(update[1].iNumEqns[3]*sizeof(double));
}

void VerifyEqtide(CONTROL *control,OUTPUT *output,FILES *files,BODY *body,OPTIONS *options,fnUpdateVariable ***fnUpdate,UPDATE *update) {
  int iBody,iTideLine,iEqn,iTideFile,iCol,iFile;

  /* Verify input set correctly and assign update functions */

  if (control->iTideModel == CTL) {
    for (iBody=0;iBody<control->iNumBodies;iBody++) {
      /* CTL model, but Q's set? */
      if (options[OPT_TIDALQ].iLine[iBody+1] >= 0) {
	/* Tidal Q was set */
	if (options[OPT_TIDALTAU].iLine[iBody+1] >= 0) {
	  /* Tidal Tau was also set */
	  if (control->iVerbose >= VERBINPUT) 
	    fprintf(stderr,"WARNING: Time lag model selected, %s and %s set in file %s. Using %s = %lf and ignoring %s.\n",options[OPT_TIDALTAU].cName,options[OPT_TIDALQ].cName,options[OPT_TIDALTAU].cFile[iBody+1],options[OPT_TIDALTAU].cName,body[iBody+1].dTidalTau,options[OPT_TIDALQ].cName);
	} else {
	  /* Tidal Tau was not set */
	  if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: Time lag model selected, but only %s was set in file %s.\n",options[OPT_TIDALQ].cName,options[OPT_TIDALQ].cFile[iBody]);
	  exit(EXIT_INPUT);
	}

      }

      /* Can't set bDiscreteRot in the CTL model */
      if (options[OPT_DISCRETEROT].iLine[iBody+1] >= 0) 
      VerifyBodyExit(options[OPT_TIDALTAU].cName,options[OPT_DISCRETEROT].cName,options[OPT_DISCRETEROT].cFile[iBody],options[OPT_TIDALTAU].iLine[iBody],options[OPT_DISCRETEROT].iLine[iBody+1],control->iVerbose);

      /* Verify output contains no CPL-specific parameters */ 
      
      if (files->Outfile[iBody].iNumCols > 0) {
	for (iCol=0;iCol<files->Outfile[iBody].iNumCols;iCol++) {

	  if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_EQROTPERCONT].cName,strlen(output[OUT_EQROTPERCONT].cName)) == 0) {
	    if (control->iVerbose >= VERBINPUT) 
	      fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_EQROTPERCONT].cName);
	    /* Must find file that declares tidel model */
	    iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
	    DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
	  }

	  if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_EQROTPERDISCRETE].cName,strlen(output[OUT_EQROTPERDISCRETE].cName)) == 0) {
	    if (control->iVerbose >= VERBINPUT) 
	      fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_EQROTPERDISCRETE].cName);
	    /* Must find file that declares tidel model */
	    iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
	    DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
	  }

	  if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_EQROTRATECONT].cName,strlen(output[OUT_EQROTRATECONT].cName)) == 0) {
	    if (control->iVerbose >= VERBINPUT) 
	      fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_EQROTRATECONT].cName);
	    /* Must find file that declares tidel model */
	    iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
	    DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
	  }

	  if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_EQROTRATEDISCRETE].cName,strlen(output[OUT_EQROTRATEDISCRETE].cName)) == 0) {
	    if (control->iVerbose >= VERBINPUT) 
	      fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_EQROTRATEDISCRETE].cName);
	    /* Must find file that declares tidel model */
	    iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
	    DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
	  }

	  if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_GAMMAORB].cName,strlen(output[OUT_GAMMAORB].cName)) == 0) {
	    if (control->iVerbose >= VERBINPUT) 
	      fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_GAMMAORB].cName);
	    /* Must find file that declares tidel model */
	    iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
	    DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
	  }

	  if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_GAMMAROT].cName,strlen(output[OUT_GAMMAROT].cName)) == 0) {
	    if (control->iVerbose >= VERBINPUT) 
	      fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_GAMMAROT].cName);
	    /* Must find file that declares tidel model */
	    iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
	    DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
	  }

	  if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_TIDALQ].cName,strlen(output[OUT_TIDALQ].cName)) == 0) {
	    if (control->iVerbose >= VERBINPUT) 
	      fprintf(stderr,"ERROR: Time lag model selected; output %s is not allowed.\n",output[OUT_TIDALQ].cName);
	    /* Must find file that declares tidel model */
	    iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine,files->iNumInputs);
	    DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],options[OPT_OUTPUTORDER].cFile[iBody+1],options[OPT_TIDEMODEL].iLine[iTideFile],options[OPT_OUTPUTORDER].iLine[iBody+1]);
	  }


	}
      }
    }

    /* Everything OK, assign Updates */
    
    /* Body #0 updates */
    InitializeCTL(update,body);

    /* 0 -> Obliquity */
    update[0].pdDoblDt = &update[0].dDeriv[0][0];
    fnUpdate[0][0][0] = &fdCTLDobliquityDt;
    
    /* 1 -> Rotation Rate */
    update[0].pdDrotDt = &update[0].dDeriv[1][0];
    fnUpdate[0][1][0] = &fdCTLDrotrateDt;
    

    /* Body #1 updates */
    
    /* 0 -> Obliquity */
    update[1].pdDoblDt = &update[1].dDeriv[0][0];
    fnUpdate[1][0][0] = &fdCTLDobliquityDt;
    
    /* 1 -> Rotation Rate */
    update[1].pdDrotDt = &update[1].dDeriv[1][0];
    fnUpdate[1][1][0] = &fdCTLDrotrateDt;
    
    /* 2 -> Semi-major axis */
    update[1].pdDsemiDt = &update[1].dDeriv[2][0];
    fnUpdate[1][2][0] = &fdCTLDsemiDt;
    
    /* 3 -> Eccentricity */
    update[1].pdDeccDt = &update[1].dDeriv[3][0];
    fnUpdate[1][3][0] = &fdCTLDeccDt;
    
  }

  /* Now verify CPL options */

  if (control->iTideModel == CPL) {
    if (body[1].dEcc > pow(2./19,0.5) && control->Integr.bDiscreteRot) {
      if (control->iVerbose >= VERBINPUT)
	fprintf(stderr,"WARNING: Setting %s to 1 is not advised for eccentricities larger than %.3lf\n",options[OPT_DISCRETEROT].cName,pow(2./19,0.5));
    }

    for (iBody=0;iBody<control->iNumBodies;iBody++) {
      /* CPL model, but tau's set? */
      if (options[OPT_TIDALTAU].iLine[iBody+1] >= 0) {
	/* Tidal tau was set */
	if (options[OPT_TIDALQ].iLine[iBody+1] >= 0) {
	  /* Tidal Q was also set */
	  if (control->iVerbose >= VERBINPUT) 
	    fprintf(stderr,"WARNING: Phase lag model selected, %s and %s set in file %s. Using %s = %lf and ignoring %s.\n",options[OPT_TIDALTAU].cName,options[OPT_TIDALQ].cName,options[OPT_TIDALTAU].cFile[iBody+1],options[OPT_TIDALQ].cName,body[iBody+1].dTidalQ,options[OPT_TIDALTAU].cName);
	} else {
	  /* Tidal Tau was not set */
	  if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: Time lag model selected, but only %s was set in file %s.\n",options[OPT_TIDALQ].cName,options[OPT_TIDALQ].cFile[iBody]);
	  exit(EXIT_INPUT);
	}
      }

      /* Verify output contains no CTL-specific parameters */ 
      
      if (files->Outfile[iBody].iNumCols > 0) {
	for (iCol=0;iCol<files->Outfile[iBody].iNumCols;iCol++) {

	  if (memcmp(files->Outfile[iBody].caCol[iCol],output[OUT_TIDALQ].cName,strlen(output[OUT_TIDALQ].cName)) == 0) {
	    if (control->iVerbose >= VERBINPUT) 
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
    
    /* Beware redudancy with above!! */

    /* Body #0 updates */
    InitializeCPL(update,body);

    /* 0 -> Obliquity */
    update[0].pdDoblDt = &update[0].dDeriv[0][0];
    fnUpdate[0][0][0] = &fdCPLDobliquityDt;
    
    /* 1 -> Rotation Rate */
    update[0].pdDrotDt = &update[0].dDeriv[1][0];
    fnUpdate[0][1][0] = &fdCPLDrotrateDt;
    
    /* Body #1 updates */

    /* 0 -> obliquity */
    update[1].pdDoblDt = &update[1].dDeriv[0][0];
    fnUpdate[1][0][0] = &fdCPLDobliquityDt;
    
    /* 1 -> Rotation Rate */
    update[1].pdDrotDt = &update[1].dDeriv[1][0];
    fnUpdate[1][1][0] = &fdCPLDrotrateDt;
    
    /* 2 -> Semi-Major Axis */
    update[1].pdDsemiDt = &update[1].dDeriv[2][0];
    fnUpdate[1][2][0] = &fdCPLDsemiDt;
    
    /* 3 -> Eccentricity */
    update[1].pdDeccDt = &update[1].dDeriv[3][0];
    fnUpdate[1][3][0] = &fdCPLDeccDt;
  }
}


void InitializeVplanetEqtide(CONTROL *control,VPLANET *vplanet) {

  if (control->iTideModel == CTL)
    InitializeCTL(vplanet->tmpUpdate,vplanet->tmpBody);
  if (control->iTideModel == CPL)
    InitializeCPL(vplanet->tmpUpdate,vplanet->tmpBody);
}

/***************** EQTIDE Halts *****************/

/* Double Synchronous? */

int HaltDblSync(CONTROL *control,BODY *body,UPDATE *update) {

  /* dMeanMotion set by call to fvTidalProperties in Evolve() */
  if (control->Halt.bDblSync && (body[0].dRotRate == body[1].dMeanMotion) && (body[1].dRotRate == body[1].dMeanMotion)) {
    if (control->iVerbose >= VERBPROG) {
      printf("HALT: Double Synchronous at ");
      fprintd(stdout,control->Integr.dTime/YEARSEC,control->iSciNot,control->iDigits);
      printf(" years.\n");
    }
    return 1;
  }

  return 0;
}  

/* Tide-locked? */
int HaltTideLock(CONTROL *control,BODY *body,UPDATE *update,int iBody) {
  if (control->bForceEqSpin[iBody] && control->Halt.bTideLock[iBody]) {
    if (control->iVerbose >= VERBPROG) {
      printf("HALT: %s tide-locked at ",body[iBody].cName);
      fprintd(stdout,control->Integr.dTime/YEARSEC,control->iSciNot,control->iDigits);
      printf(" years.\n");
    }
    return 1;
  }

  return 0;
}

/* Synchronous Rotation? */
int HaltSyncRot(CONTROL *control,BODY *body,UPDATE *update,int iBody) {
  if (control->Halt.bSync[iBody] && (body[iBody].dRotRate == body[1].dMeanMotion)) {
    if (control->iVerbose >= VERBPROG) {
      printf("HALT: %s's rotation is synchronous (e = ",body[iBody].cName);
      fprintd(stdout,body[1].dEcc,control->iSciNot,control->iDigits);
      printf(") at ");
      fprintd(stdout,control->Integr.dTime/YEARSEC,control->iSciNot,control->iDigits);
      printf(" years.\n");
    }
    return 1;
  }

  return 0;
}

void InitializeHaltEqtide(HALT *halt,VPLANET *vplanet,int iNumInputs,int *iHaltSys,int *iHaltBody) {

  halt->bTideLock=malloc(iNumInputs*sizeof(int));
  halt->bSync=malloc(iNumInputs*sizeof(int));

  vplanet->fnHaltSys[(*iHaltSys)++] = &HaltDblSync;

  vplanet->fnHaltBody[(*iHaltBody)++] = &HaltTideLock;
  vplanet->fnHaltBody[(*iHaltBody)++] = &HaltSyncRot;

}

/************* EQTIDE Outputs ******************/

void HelpOutputEqtide(OUTPUT *output) {
  int i;

  printf("\n ------ EQTIDE output ------\n");
  for (i=OUTSTARTEQTIDE;i<OUTENDEQTIDE;i++) 
    WriteHelpOutput(output[i]);
}

/* 
 * B
 */

void WriteBodyDsemiDtEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  int iPert;

  if (iBody == 0) 
    iPert=1;
  else
    iPert=0;

  if (control->iTideModel == CPL) {
    fvCPLEpsilon(body[iBody].dRotRate,body[1].dSemi,body[iBody].iTidalEpsilon);
    fvCPLZ(body);
    *dTmp = fdCPLDsemiDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEcc);
  } else if (control->iTideModel == CTL) {
    fvCTLZ(body);
    *dTmp = fdCTLDsemiDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEcc,body[iBody].dObliquity,body[iBody].dRotRate);
  }
		 
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime)/fdLengthUnit(units->iLength);
    fvVelUnit(units,cUnit);
  }
}

void WriteBodyDeccDtEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  int iPert;

  if (iBody == 0) 
    iPert=1;
  else
    iPert=0;

  if (control->iTideModel == CPL) 
    *dTmp = fdCPLDeccDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEcc);
  else
    *dTmp = fdCTLDeccDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEcc);

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime);
    fvRateUnit(units->iTime,cUnit);
  }
}

void WriteDOblDtEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dFoo;

  /* Ensure that we don't overwrite pdDoblDt */
  dFoo = *(update[iBody].pdDoblDt);
  *dTmp = dFoo;

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime)/fdAngleUnit(units->iAngle);
    fvAngRateUnit(units,cUnit);
  }
}

void WriteDSemiDtEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dFoo;

  /* Ensure that we don't overwrite pdDsemiDt */
  dFoo = *(update[1].pdDsemiDt); 
  *dTmp = dFoo;

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime)/fdLengthUnit(units->iLength);
    fvVelUnit(units,cUnit);
  }
}

void WriteDEccDtEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dFoo;
  
  /* Ensure that we don't overwrite pdDeccDt */
  dFoo = *(update[1].pdDeccDt); 
  *dTmp = dFoo;

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime);
    fvRateUnit(units->iTime,cUnit);
  }
}

void WriteDMeanMotionDtEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = -1.5*pow(BIGG*(body[0].dMass+body[1].dMass)/pow(body[1].dSemi,5),0.5)*(*(update[1].pdDsemiDt));

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime)/fdLengthUnit(units->iLength);
    fvRateUnit(units->iTime,cUnit);
    strcat(cUnit,"^2");
  }
}

void WriteDOrbPerDtEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = 1.5*pow(4*PI*PI*body[1].dSemi/(BIGG*(body[0].dMass+body[1].dMass)),0.5)*(*(update[1].pdDsemiDt));

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime)/fdLengthUnit(units->iLength);
    sprintf(cUnit,"");
  }
}



void WriteDRotPerDtEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dFoo;

  /* Ensure that we don't overwrite pdDrotDt */

  dFoo = *(update[iBody].pdDrotDt);
  *dTmp = dFoo;

  /* Multiply by dP/domega to get dP/dt */
  *dTmp = dFoo * (-2*PI/(body[iBody].dRotRate*body[iBody].dRotRate));
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  }  else {
    strcat(cUnit,"");
  }
}

void WriteDRotRateDtEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dFoo;

  /* Ensure that we don't overwrite pdDrotDt */
  dFoo = *(update[iBody].pdDrotDt);
  *dTmp = dFoo;

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  }  else {
    *dTmp *= fdTimeUnit(units->iTime)*fdTimeUnit(units->iTime);
    fvRateUnit(units->iTime,cUnit);
    strcat(cUnit,"^2");
  }
}

/*
 * E
 */

void WriteEccTimescaleEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdTimescale(body[1].dEcc,*(update[1].pdDeccDt));

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}

void WriteEqRotPer(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdFreqToPer(fdEqRotRate(body[iBody],body[1].dMeanMotion,body[1].dEcc,control->iTideModel,control->Integr.bDiscreteRot));

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  }  else {
    *dTmp /= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}

void WriteEqRotPerCont(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdFreqToPer(fdCPLEqRotRateCont(body[1].dMeanMotion,body[1].dEcc));

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}

void WriteEqRotPerDiscrete(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdFreqToPer(fdCPLEqRotRateDiscrete(body[1].dMeanMotion,body[1].dEcc));
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}

void WriteEqRotRate(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdEqRotRate(body[iBody],body[1].dMeanMotion,body[1].dEcc,control->iTideModel,control->Integr.bDiscreteRot);

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime);
    fvRateUnit(units->iTime,cUnit);
  }
}

void WriteEqRotRateCont(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdCPLEqRotRateCont(body[1].dMeanMotion,body[1].dEcc);

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime);
    fvRateUnit(units->iTime,cUnit);
  }
}

void WriteEqRotRateDiscrete(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdCPLEqRotRateDiscrete(body[1].dMeanMotion,body[1].dEcc);
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime);
    fvRateUnit(units->iTime,cUnit);
  }      
}

/*
 * G
 */

void WriteGammaOrb(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = fdGammaOrb(body[1].dEcc,body[iBody].dObliquity,body[iBody].iTidalEpsilon);

  /* Negative option? */
  strcat(cUnit,"cgs");
}

void WriteGammaRot(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdGammaRot(body[1].dEcc,body[iBody].dObliquity,body[iBody].iTidalEpsilon);

  /* Negative option? */
  strcat(cUnit,"cgs");
}

void WriteOblTimescaleEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdTimescale(body[iBody].dObliquity,*(update[iBody].pdDoblDt));

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}

void WriteRotTimescaleEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdTimescale(body[iBody].dRotRate,*(update[iBody].pdDrotDt));

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}

/*
 *S 
 */

void WriteSemiTimescaleEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = fdTimescale(body[1].dSemi,*(update[1].pdDsemiDt));

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp *= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}

/*
 * T
 */

void WriteHeatEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  /* Get total tidal heat */

  *dTmp = fdTideHeat(body[iBody],body[1].dEcc,body[1].dMeanMotion,body[1].dSemi,control->iTideModel);

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdEnergyUnit(units->iTime,units->iMass,units->iLength);
    fvEnergyUnit(units,cUnit);
  }
}

void WriteHeatFluxEqtide(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  /* Get total tidal heat */

  *dTmp = fdTideHeatFlux(body[iBody],body[1].dEcc,body[1].dMeanMotion,body[1].dSemi,control->iTideModel);

  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdEnergyFluxUnit(units->iTime,units->iMass,units->iLength);
    fvEnergyFluxUnit(units,cUnit);
  }
}

void WriteTidalQ(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dTidalQ;
  strcat(cUnit,"");
}

void WriteTidalTau(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dTidalTau;
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdTimeUnit(units->iTime);
    fvTimeUnit(units->iTime,cUnit);
  }
}

void WriteTideLock(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = control->bForceEqSpin[iBody];
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
  
  sprintf(output[OUT_ECCTIMEEQTIDE].cName,"EccTimeEqtime");
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
  
  sprintf(output[OUT_HEATFLUXEQTIDE].cName,"HeatFluxEqtide");
  sprintf(output[OUT_HEATFLUXEQTIDE].cDescr,"Surface Heat Flux due to Tides in EQTIDE");
  sprintf(output[OUT_HEATFLUXEQTIDE].cNeg,"W/m^2");
  output[OUT_HEATFLUXEQTIDE].bNeg = 1;
  output[OUT_HEATFLUXEQTIDE].dNeg = 1e-3;
  output[OUT_HEATFLUXEQTIDE].iNum = 1;
  fnWrite[OUT_HEATFLUXEQTIDE] = &WriteHeatFluxEqtide;
  
  sprintf(output[OUT_HEATEQTIDE].cName,"HeatEqtide");
  sprintf(output[OUT_HEATEQTIDE].cDescr,"Internal Heat due to Tides in EQTIDE");
  sprintf(output[OUT_HEATEQTIDE].cNeg,"W/m^2");
  output[OUT_HEATEQTIDE].bNeg = 1;
  output[OUT_HEATEQTIDE].dNeg = 1e-3;
  output[OUT_HEATEQTIDE].iNum = 1;
  fnWrite[OUT_HEATEQTIDE] = &WriteHeatFluxEqtide;
  
  sprintf(output[OUT_TIDELOCK].cName,"TideLock");
  sprintf(output[OUT_TIDELOCK].cDescr,"Tidally Locked?");
  output[OUT_TIDELOCK].bNeg = 0;
  output[OUT_TIDELOCK].iNum = 1;
  fnWrite[OUT_TIDELOCK] = &WriteTideLock;
    
}

/************ EQTIDE Logging Functions **************/

void LogOptionsEqtide(CONTROL *control, FILE *fp) {

  fprintf(fp,"-------- EQTIDE Options -----\n\n");
  /* Tidal Model */
  fprintf(fp,"Tidal Model: ");
  if (control->iTideModel == CPL) {
    fprintf(fp,"Constant-Phase-Lag, 2nd order\n");
    fprintf(fp,"Use Discrete Rotation Rate Model: %d\n",control->Integr.bDiscreteRot);
  } 

  if (control->iTideModel == CTL) {
    fprintf(fp,"Constant-Time-Lag, 8th order\n");
  }
}

void LogEqtide(CONTROL *control,OUTPUT *output,BODY *body,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  fprintf(fp,"\n----- EQTIDE PARAMETERS ------\n");
  for (iOut=OUTSTARTEQTIDE;iOut<OUTBODYSTARTEQTIDE;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
}

void LogBodyEqtide(CONTROL *control,OUTPUT *output,BODY *body,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- EQTIDE PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTEQTIDE;iOut<OUTENDEQTIDE;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,iBody);
  }
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

void fvChi(BODY *body) {
  double dMeanMotion;

  dMeanMotion = fdSemiToMeanMotion(body[1].dSemi,body[0].dMass+body[1].dMass);

  body[0].dTidalChi=body[0].dRadGyra*body[0].dRadGyra*body[0].dRadius*body[0].dRadius*body[0].dRotRate*body[1].dSemi*dMeanMotion/(BIGG*body[1].dMass);
  body[1].dTidalChi=body[1].dRadGyra*body[1].dRadGyra*body[1].dRadius*body[1].dRadius*body[1].dRotRate*body[1].dSemi*dMeanMotion/(BIGG*body[0].dMass);
}

int fbTidalLock(CONTROL *control,BODY *body,int iBody) {
  double dEqRate,dDiff;

  dEqRate = fdEqRotRate(body[iBody],body[1].dMeanMotion,body[1].dEcc,control->iTideModel,control->Integr.bDiscreteRot);

  dDiff = fabs(body[iBody].dRotRate - dEqRate)/dEqRate;

  if (dDiff < control->dMaxLockDiff[iBody]) {
    if (control->iVerbose >= VERBPROG) {
      printf("%s spin locked at ",body[iBody].cName);
      fprintd(stdout,control->Integr.dTime/YEARSEC,control->iSciNot,control->iDigits);
      printf(" years.\n");
    }
    return 1; /* This forces it to be locked from now on */
  }
  /* Not tidally locked */
  return 0;
}      

void fvPropertiesEqtide(CONTROL *control,BODY *body) {
  int i;

  body[1].dMeanMotion = fdSemiToMeanMotion(body[1].dSemi,(body[0].dMass+body[1].dMass));

  if (control->iTideModel == CPL) {
    for (i=0;i<2;i++)
      fvCPLEpsilon(body[i].dRotRate,body[1].dMeanMotion,body[i].iTidalEpsilon);
    fvCPLZ(body);
  } else if (control->iTideModel == CTL) {
    fvCTLZ(body);
    body[1].dTidalBeta = fdCTLBeta(body[1].dEcc);
    fvCTLF(&body[1]);
  }

  fvChi(body);
}

double fdTideHeat(BODY body,double dEcc,double dMeanMotion,double dSemi,int iTideModel) {

  /* Get total tidal heat */
  if (iTideModel == CPL)
    return fdCPLTideHeat(body,dMeanMotion,dEcc);
  else if (iTideModel == CTL)
    return fdCTLTideHeat(body,dMeanMotion,dEcc);

  /* Whoops! */
  assert(0);
}

double fdTideHeatFlux(BODY body,double dEcc,double dMeanMotion,double dSemi,int iTideModel) {
  double dHeat;

  dHeat = fdTideHeat(body,dEcc,dMeanMotion,dSemi,iTideModel); 
  
  return dHeat/(4*PI*body.dRadius*body.dRadius);
}

/*
 * Alter the simulation is a specific way. Possibilities are 
 * stored in the PARAM struct. 
*/

void ForceBehaviorEqtide(BODY *body,CONTROL *control) {
  int iBody;

  fvPropertiesEqtide(control,body);

  /* If small enough, set some quantities to zero */
  if (control->Integr.dMinValue > 0) {
    if (body[1].dEcc < control->Integr.dMinValue)
      body[1].dEcc = 0;
    for (iBody=0;iBody<2;iBody++) 
      if (body[iBody].dObliquity < control->Integr.dMinValue)
	body[iBody].dObliquity = 0;
  }

  /* Tidally Locked? */
  for (iBody=0;iBody<2;iBody++) {
    if (!control->bForceEqSpin[iBody])
      control->bForceEqSpin[iBody] = fbTidalLock(control,body,iBody);
  }


  /* If tidally locked, assign equilibrium rotational frequency */
  for (iBody=0;iBody<2;iBody++) {
    if (control->bForceEqSpin[iBody])
      body[iBody].dRotRate = fdEqRotRate(body[iBody],body[1].dMeanMotion,body[1].dEcc,control->iTideModel,control->Integr.bDiscreteRot);
  }
}



/*
 ************************ CPL Functions ******************
*/


/* Auxiliary Parameters */

double fdCPLTideHeat(BODY body,double dMeanMotion,double dEcc) {

  /* Heller, Leconte & Barnes (2011), Eqn 9-11. */

  double dOrbE,dRotE;

  dOrbE = -body.dTidalZ/8 * (4*body.iTidalEpsilon[0] + dEcc*dEcc*(-20*body.iTidalEpsilon[0] + 147./2*body.iTidalEpsilon[1] + 0.5*body.iTidalEpsilon[2] - 3*body.iTidalEpsilon[5]) - 4*sin(body.dObliquity)*sin(body.dObliquity)*(body.iTidalEpsilon[0] - body.iTidalEpsilon[8]));

  dRotE = body.dTidalZ*body.dRotRate/(8*dMeanMotion) * (4*body.iTidalEpsilon[0] + dEcc*dEcc*(-20*body.iTidalEpsilon[0] + 49*body.iTidalEpsilon[1] + body.iTidalEpsilon[2]) + 2*sin(body.dObliquity)*sin(body.dObliquity)*(-2*body.iTidalEpsilon[0] + body.iTidalEpsilon[8] + body.iTidalEpsilon[9]));

  return -(dOrbE + dRotE);
}

double fdGammaRot(double dEcc,double dPsi,int *epsilon) {
  return 4*epsilon[0] + dEcc*dEcc*(-20*epsilon[0] + 49*epsilon[1] + epsilon[2]) + 2*sin(dPsi)*sin(dPsi)*(-2*epsilon[0] + epsilon[8] + epsilon[9]);
}

double fdGammaOrb(double dEcc,double dPsi,int *epsilon) {
    return 4*epsilon[0] + dEcc*dEcc*(-20*epsilon[0] + 147./2*epsilon[1] + 0.5*epsilon[2] - 3*epsilon[5]) - 4*sin(dPsi)*sin(dPsi)*(epsilon[0]-epsilon[8]);
}

void fvCPLEpsilon(double dRotRate,double dMeanMotion,int *iEpsilon) {

  iEpsilon[0]=fiSign(2*dRotRate-2*dMeanMotion);
  iEpsilon[1]=fiSign(2*dRotRate-3*dMeanMotion);
  iEpsilon[2]=fiSign(2*dRotRate-dMeanMotion);
  iEpsilon[5]=fiSign(dMeanMotion);
  iEpsilon[8]=fiSign(dRotRate-2*dMeanMotion);
  iEpsilon[9]=fiSign(dRotRate);
  
}

void fvCPLZ(BODY *body) {
  double dMeanMotion;

  dMeanMotion = fdSemiToMeanMotion(body[1].dSemi,body[0].dMass+body[1].dMass);

    body[0].dTidalZ = 3*BIGG*BIGG*body[0].dK2*body[1].dMass*body[1].dMass*(body[0].dMass+body[1].dMass)*pow(body[0].dRadius,5)/(pow(body[1].dSemi,9)*dMeanMotion*body[0].dTidalQ);

    body[1].dTidalZ = 3*BIGG*BIGG*body[1].dK2*body[0].dMass*body[0].dMass*(body[0].dMass+body[1].dMass)*pow(body[1].dRadius,5)/(pow(body[1].dSemi,9)*dMeanMotion*body[1].dTidalQ);

    return;
}

/*
 * Equilibrium Parameters
 */

double fdCPLTideHeatEq(double dTidalZ,double dEcc,double dMeanMotion,double dObliquity,int bDiscrete) {
  /* Heller, Leconte  Barnes 2011, Eq. 13 */
  double dGammaOrb,dGammaRot,dRotRateEq;
  int *iEpsilon;

  iEpsilon = malloc(9*sizeof(int));

  /* Must reset spin rate in order to get the phase lags correctly */

  dRotRateEq = fdCPLEqRotRate(dMeanMotion,dEcc,bDiscrete);

  fvCPLEpsilon(dRotRateEq,dMeanMotion,iEpsilon);
  
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

double fdCPLDsemiDt(BODY *body,int iBody) {
  double sum,psi[2];
  int i;
   
  psi[0]=body[0].dObliquity;
  psi[1]=body[1].dObliquity;

  sum=0;
  for (i=0;i<2;i++) 
    sum += body[i].dTidalZ*(4*body[i].iTidalEpsilon[0] + body[1].dEcc*body[1].dEcc*(-20*body[i].iTidalEpsilon[0] + 147./2*body[i].iTidalEpsilon[1] + 0.5*body[i].iTidalEpsilon[2] - 3*body[i].iTidalEpsilon[5]) - 4*sin(psi[i])*sin(psi[i])*(body[i].iTidalEpsilon[0]-body[i].iTidalEpsilon[8]));

  return body[1].dSemi*body[1].dSemi/(4*BIGG*body[0].dMass*body[1].dMass)*sum;

}

double fdCPLDeccDt(BODY *body,int iBody) {
  double sum;
  int i;

  sum=0;
  for (i=0;i<2;i++)
    sum += body[i].dTidalZ*(2*body[i].iTidalEpsilon[0] - 49./2*body[i].iTidalEpsilon[1] + 0.5*body[i].iTidalEpsilon[2] + 3*body[i].iTidalEpsilon[5]);

    return -body[1].dSemi*body[1].dEcc/(8*BIGG*body[0].dMass*body[1].dMass) * sum;
}  


double fdCPLDsemiDtBody(BODY body,double dMassPert,double dSemi,double dEcc) {
  double foo;

    foo = body.dTidalZ*(4*body.iTidalEpsilon[0] + dEcc*dEcc*(-20*body.iTidalEpsilon[0] + 147./2*body.iTidalEpsilon[1] + 0.5*body.iTidalEpsilon[2] - 3*body.iTidalEpsilon[5]) - 4*sin(body.dObliquity)*sin(body.dObliquity)*(body.iTidalEpsilon[0]-body.iTidalEpsilon[8]));

  return dSemi*dSemi/(4*BIGG*body.dMass*dMassPert)*foo;

}

double fdCPLDeccDtBody(BODY body,double dMassPert,double dSemi,double dEcc) {
  double foo;
 
    foo = body.dTidalZ*(2*body.iTidalEpsilon[0] - 49./2*body.iTidalEpsilon[1] + 0.5*body.iTidalEpsilon[2] + 3*body.iTidalEpsilon[5]);

    return -dSemi*dEcc/(8*BIGG*body.dMass*dMassPert) * foo;
}  

double fdCPLDrotrateDt(BODY *body,int iBody) {
  int i,iEpsilon[10];

  /* Note if tidally locked, ForceBehavior will fix the rotation
     rate and override this derivative. */

  return -body[iBody].dTidalZ/(8*body[iBody].dMass*body[iBody].dRadGyra*body[iBody].dRadGyra*body[iBody].dRadius*body[iBody].dRadius*body[1].dMeanMotion)*(4*body[iBody].iTidalEpsilon[0] + body[1].dEcc*body[1].dEcc*(-20*body[iBody].iTidalEpsilon[0] + 49*body[iBody].iTidalEpsilon[1] + body[iBody].iTidalEpsilon[2]) + 2*sin(body[iBody].dObliquity)*sin(body[iBody].dObliquity)*(-2*body[iBody].iTidalEpsilon[0]+body[iBody].iTidalEpsilon[8]+body[iBody].iTidalEpsilon[9]));
}

double fdCPLDobliquityDt(BODY *body, int iBody) {

  return body[iBody].dTidalZ*sin(body[iBody].dObliquity)/(4*body[iBody].dMass*body[iBody].dRadGyra*body[iBody].dRadGyra*body[iBody].dRadius*body[iBody].dRadius*body[1].dMeanMotion*body[iBody].dRotRate) * (body[iBody].iTidalEpsilon[0]*(1-body[iBody].dTidalChi) + (body[iBody].iTidalEpsilon[8]-body[iBody].iTidalEpsilon[9])*(1 + body[iBody].dTidalChi));
}


/*
 ********************** CTL Functions ************************
*/

double fdCTLBeta(double e) {
    return pow(1 - e*e,0.5);
}

double fdCTLF1(double e) {
  return (1 + 31./2*e*e + 255./8*pow(e,4) + 185./16*pow(e,6) + 25./64*pow(e,8));
}

double fdCTLF2(double e) {
  return (1 + 15./2*e*e + 45./8*pow(e,4) + 5./16*pow(e,6));
}

double fdCTLF3(double e) {
  return (1 + 15./4*e*e + 15./8*pow(e,4) + 5./64*pow(e,6));
}

double fdCTLF4(double e) {
  return (1 + 1.5*e*e + 1./8*pow(e,4));
}

double fdCTLF5(double e) {
    return (1 + 3*e*e + 3./8*pow(e,4));
}

void fvCTLF(BODY *body) {
  body->dTidalF[0] = fdCTLF1(body->dEcc);
  body->dTidalF[1] = fdCTLF2(body->dEcc);
  body->dTidalF[2] = fdCTLF3(body->dEcc);
  body->dTidalF[3] = fdCTLF4(body->dEcc);
  body->dTidalF[4] = fdCTLF5(body->dEcc);
}

double fdCTLTideHeat(BODY body,double dMeanMotion,double dEcc) {

  body.dTidalBeta = fdCTLBeta(dEcc);
  fvCTLF(&body);

  return body.dTidalZ*(body.dTidalF[0]/pow(body.dTidalBeta,15) - 2*body.dTidalF[1]*cos(body.dObliquity)*body.dRotRate/(pow(body.dTidalBeta,12)*dMeanMotion) + ((1+cos(body.dObliquity)*cos(body.dObliquity))/2)*body.dTidalF[4]*body.dRotRate*body.dRotRate/(pow(body.dTidalBeta,9)*dMeanMotion*dMeanMotion));

}

/*
 * Equilibrium Parameters
 */

double fdCTLTideHeatEq(BODY body,double dEcc) {

  body.dTidalBeta = fdCTLBeta(dEcc);
  fvCTLF(&body);

  return body.dTidalZ/pow(body.dTidalBeta,15) * (body.dTidalF[0] - body.dTidalF[1]*body.dTidalF[1]/body.dTidalF[4] * (2*cos(body.dObliquity)*cos(body.dObliquity))/(1+cos(body.dObliquity)*cos(body.dObliquity)));
}

double fdCTLEqRotRate(double dEcc,double dObliquity,double dMeanMotion) {
  double dBeta,f2,f5;
  
  dBeta=fdCTLBeta(dEcc);
  f2=fdCTLF2(dEcc);
  f5=fdCTLF5(dEcc);
  
  return f2/(pow(dBeta,3)*f5) * 2*cos(dObliquity)/(1+cos(dObliquity)*cos(dObliquity)) * dMeanMotion;
}

void fvCTLZ(BODY *body) {
  body[0].dTidalZ = 3*BIGG*BIGG*body[0].dK2*body[1].dMass*body[1].dMass*(body[0].dMass+body[1].dMass)*pow(body[0].dRadius,5)*body[0].dTidalTau/pow(body[1].dSemi,9);
  body[1].dTidalZ = 3*BIGG*BIGG*body[1].dK2*body[0].dMass*body[0].dMass*(body[0].dMass+body[1].dMass)*pow(body[1].dRadius,5)*body[1].dTidalTau/pow(body[1].dSemi,9);
}

/*
 * Derivatives
 */

double fdCTLDsemiDt(BODY *body,int iBody) {
  int i;
  double sum;

  sum=0;
  for (i=0;i<2;i++) 
    /* XXX Sum the body functions? */
      sum += body[1].dTidalZ*(cos(body[i].dObliquity)*body[1].dTidalF[1]*body[i].dRotRate/(pow(body[1].dTidalBeta,12)*body[1].dMeanMotion) - body[1].dTidalF[0]/pow(body[1].dTidalBeta,15));

  return 2*body[1].dSemi*body[1].dSemi/(BIGG*body[0].dMass*body[1].dMass)*sum;
}

double fdCTLDeccDt(BODY *body,int iBody) {
  int i;
  double sum;

  sum=0;
  for (i=0;i<2;i++) 
    sum += body[i].dTidalZ*(cos(body[i].dObliquity)*body[1].dTidalF[3]*body[i].dRotRate/(pow(body[1].dTidalBeta,10)*body[1].dMeanMotion) - 18*body[1].dTidalF[2]/(11*pow(body[1].dTidalBeta,13)));
  
  return 11*body[1].dSemi*body[1].dEcc/(2*BIGG*body[0].dMass*body[1].dMass)*sum;
}

double fdCTLDsemiDtBody(BODY body,double dMassPert,double dSemi,double dEcc,double dObliquity,double dRotRate) {
  double foo,dMeanMotion,dBeta;

  dMeanMotion=fdSemiToMeanMotion(dSemi,(body.dMass+dMassPert));
  dBeta=fdCTLBeta(dEcc);
  body.dEcc=dEcc;
  fvCTLF(&body);
  foo = body.dTidalZ*(cos(dObliquity)*body.dTidalF[1]*dRotRate/(pow(dBeta,12)*dMeanMotion) - body.dTidalF[0]/pow(dBeta,15));

  return 2*dSemi*dSemi/(BIGG*body.dMass*dMassPert)*foo;
}


double fdCTLDeccDtBody(BODY body,double dMassPert,double dSemi,double dEcc) {
  double foo;
  double dMeanMotion,dBeta;

  dMeanMotion = fdSemiToMeanMotion(dSemi,(body.dMass+dMassPert));
  dBeta=fdCTLBeta(dEcc);
  body.dEcc=dEcc;
  fvCTLF(&body);
  foo = body.dTidalZ*(cos(body.dObliquity)*body.dTidalF[3]*body.dRotRate/(pow(dBeta,10)*dMeanMotion) - 18*body.dTidalF[2]/(11*pow(dBeta,13)));
  
  return 11*dSemi*dEcc/(2*BIGG*dMassPert*body.dMass)*foo;
}

double fdCTLDrotrateDt(BODY *body,int iBody) {

  /* Note if tidally locked, ForceBehavior will fix the rotation
     rate and override this derivative. */

  return body[iBody].dTidalZ/(2*body[iBody].dMass*body[iBody].dRadGyra*body[iBody].dRadGyra*body[iBody].dRadius*body[iBody].dRadius*body[1].dMeanMotion) * (2*cos(body[iBody].dObliquity)*body[1].dTidalF[1]/pow(body[1].dTidalBeta,12) - (1+cos(body[iBody].dObliquity)*cos(body[iBody].dObliquity))*body[1].dTidalF[4]*body[iBody].dRotRate/(pow(body[1].dTidalBeta,9)*body[1].dMeanMotion));
}

double fdCTLDobliquityDt(BODY *body,int iBody) {

  return (body[iBody].dTidalZ*sin(body[iBody].dObliquity))/(2*body[iBody].dMass*body[iBody].dRadGyra*body[iBody].dRadGyra*body[iBody].dRadius*body[iBody].dRadius*body[1].dMeanMotion*body[iBody].dRotRate) * ((cos(body[iBody].dObliquity) - body[iBody].dTidalChi/body[1].dTidalBeta)*body[1].dTidalF[4]*body[iBody].dRotRate/(pow(body[1].dTidalBeta,9)*body[1].dMeanMotion) - 2*body[1].dTidalF[1]/pow(body[1].dTidalBeta,12));
}

