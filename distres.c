/********************** DISTRES.C **********************/
/*
 * Russell Deitrick, February 2017
 *
 * Subroutines that control the integration of the resoanant orbital model. 
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"

void BodyCopyDistRes(BODY *dest,BODY *src,int iTideModel,int iNumBodies,int iBody) {
  int iIndex,iPert;

  dest[iBody].dMeanL = src[iBody].dMeanL;
//   dest[iBody].dMeanA = src[iBody].dMeanA;
}

void InitializeBodyDistRes(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {

}

void InitializeUpdateTmpBodyDistRes(BODY *body,CONTROL *control,UPDATE *update,int iBody) {

}

/**************** DISTRES options ********************/

void ReadMeanL(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

    body[iFile-1].dMeanL = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dMeanL = options->dDefault;
}

void ReadMeanA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

    body[iFile-1].dMeanA = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dMeanA = options->dDefault;
}

void InitializeOptionsDistRes(OPTIONS *options,fnReadOption fnRead[]) {
  sprintf(options[OPT_MEANL].cName,"dMeanL");
  sprintf(options[OPT_MEANL].cDescr,"Mean Longitude");
  sprintf(options[OPT_MEANL].cDefault,"0");
  options[OPT_MEANL].dDefault = 0.0;
  options[OPT_MEANL].iType = 2;
  options[OPT_MEANL].iMultiFile = 1;
  fnRead[OPT_MEANL] = &ReadMeanL;
  
  sprintf(options[OPT_MEANA].cName,"dMeanA");
  sprintf(options[OPT_MEANA].cDescr,"Mean Anomaly");
  sprintf(options[OPT_MEANA].cDefault,"0");
  options[OPT_MEANA].dDefault = 0.0;
  options[OPT_MEANA].iType = 2;
  options[OPT_MEANA].iMultiFile = 1;
  fnRead[OPT_MEANA] = &ReadMeanA;
}

void ReadOptionsDistRes(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTDISTRES;iOpt<OPTENDDISTRES;iOpt++) { 
      if (options[iOpt].iType != -1) {
        fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
      }
  }
}
    

/******************* Verify DISTRES ******************/

/* In the following, iBody is the current body number that is getting assigned,
   iPert counts the number of bodies perturbing iBody, and iBodyPert is the
   body number of the current perturbing body. */

/* The indexing gets a bit confusing here. iPert = 0 to iGravPerts-1 correspond
   to all perturbing planets,  iPert = iGravPerts corresponds to the stellar general
   relativistic correction, if applied. */

void VerifyMeanLongAnom(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody,int iVerbose) {
  /* First see if mean longitude set and mean anomaly not set, i.e. the user input the default parameters */
  if (options[OPT_MEANL].iLine[iBody+1] > -1 && options[OPT_MEANA].iLine[iBody+1] == -1) {
    body[iBody].dMeanA = body[iBody].dMeanL - body[iBody].dLongP;
    while (body[iBody].dMeanA < 0) {
      body[iBody].dMeanA += 2*PI;
    } 
    return;
  }

  /* If none are set, raise error */
  if (options[OPT_MEANA].iLine[iBody+1] == -1 && options[OPT_MEANL].iLine[iBody+1] == -1) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Must set one of %s and %s in File: %s.\n",options[OPT_MEANA].cName,options[OPT_MEANL].cName, cFile);
    exit(EXIT_INPUT);
  }

  /* Were both set? */
  if (options[OPT_MEANA].iLine[iBody+1] > -1 && options[OPT_MEANL].iLine[iBody+1] > -1) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Cannot set both of %s and %s in File: %s.\n",options[OPT_MEANA].cName,options[OPT_MEANL].cName, cFile);
    exit(EXIT_INPUT);
  }

  /* Was MeanA set? */
  if (options[OPT_MEANA].iLine[iBody+1] > -1) {
    body[iBody].dMeanL = body[iBody].dMeanA + body[iBody].dLongP;
    while (body[iBody].dMeanL > 2*PI) {
      body[iBody].dMeanL -= 2*PI;
    }     
    return;
  }
}

void InitializeSemiDistRes(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]] = 1;
  update[iBody].padDSemiDtDistRes[iPert] = &update[iBody].daDerivProc[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]];
  update[iBody].iNumBodies[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]]=2;
  update[iBody].iaBody[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeMeanLDistRes(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]] = 2;
  update[iBody].padDMeanLDtDistRes[iPert] = &update[iBody].daDerivProc[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]];
  update[iBody].iNumBodies[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]]=2;
  update[iBody].iaBody[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeHeccDistRes(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]] = 2;
  update[iBody].padDHeccDtDistRes[iPert] = &update[iBody].daDerivProc[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]];
  update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]]=2;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeKeccDistRes(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]] = 2;
  update[iBody].padDKeccDtDistRes[iPert] = &update[iBody].daDerivProc[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]];
  update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]]=2;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializePincDistRes(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]] = 2;
  update[iBody].padDPincDtDistRes[iPert] = &update[iBody].daDerivProc[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]];
  update[iBody].iNumBodies[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]]=2;
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeQincDistRes(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]] = 2;
  update[iBody].padDQincDtDistRes[iPert] = &update[iBody].daDerivProc[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]];
  update[iBody].iNumBodies[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]]=2;
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void VerifyDistRes(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int iPert;
  
  VerifyMeanLongAnom(body,control,options,files->Infile[iBody+1].cIn,iBody,control->Io.iVerbose);
  
  if (iBody == 1) {
    for (j=26;j<LAPLNUM;j++) {
        system->fnLaplaceF[j] = malloc(1*sizeof(fnLaplaceFunction));
        system->fnLaplaceDeriv[j] = malloc(1*sizeof(fnLaplaceFunction));
      } 
      system->fnLaplaceF[0][0] = &fdSemiMajAxF1;
      system->fnLaplaceF[1][0] = &fdSemiMajAxF2;
      system->fnLaplaceDeriv[0][0] = &fdDSemiF1Dalpha;
      system->fnLaplaceDeriv[1][0] = &fdDSemiF2Dalpha;
  }
  
  if (iBody > 0) {
    for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
        /* semi major axis */
        InitializeSemiDistRes(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iSemi][update[iBody].iaSemiDistRes[iPert]] = &fdDistResRD2DaDt;
        
        /* mean longitude */
        InitializeMeanLDistRes(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iMeanL][update[iBody].iaMeanLDistRes[iPert]] = &fdDistResRD2DlDt;
        
        /* h = Ecc*sin(LongP) */
        InitializeHeccDistRes(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iHecc][update[iBody].iaHeccDistRes[iPert]] = &fdDistResRD2DhDt;
        
        /* k = Ecc*cos(LongP) */
        InitializeKeccDistRes(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iKecc][update[iBody].iaKeccDistRes[iPert]] = &fdDistResRD2DkDt;
        
        /* p = s*sin(LongA) */
        InitializePincDistRes(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iPinc][update[iBody].iaPincDistRes[iPert]] = &fdDistResRD2DpDt;
        
        /* q = s*cos(LongA) */
        InitializeQincDistRes(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iQinc][update[iBody].iaQincDistRes[iPert]] = &fdDistResRD2DqDt;
    }
  }
  
  control->fnForceBehavior[iBody][iModule]=&ForceBehaviorDistRes;
  control->Evolve.fnBodyCopy[iBody][iModule]=&BodyCopyDistRes;
}


/***************** DISTRES Update *****************/
void InitializeUpdateDistRes(BODY *body,UPDATE *update,int iBody) {
  if (iBody > 0) {
    if (update[iBody].iNumSemi == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumSemi += body[iBody].iGravPerts;

    if (update[iBody].iNumMeanL == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumMeanL += body[iBody].iGravPerts;
    
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
  }
}

void FinalizeUpdateHeccDistRes(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;
  
  update[iBody].padDHeccDtDistRes = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaHeccDistRes = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTRES;
    update[iBody].iaHeccDistRes[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateKeccDistRes(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;
  
  update[iBody].padDKeccDtDistRes = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaKeccDistRes = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTRES;
    update[iBody].iaKeccDistRes[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdatePincDistRes(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;
  
  update[iBody].padDPincDtDistRes = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaPincDistRes = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTRES;
    update[iBody].iaPincDistRes[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateQincDistRes(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;
  
  update[iBody].padDQincDtDistRes = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaQincDistRes = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTRES;
    update[iBody].iaQincDistRes[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateSemiDistRes(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;
  
  update[iBody].padDSemiDtDistRes = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaSemiDistRes = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTRES;
    update[iBody].iaSemiDistRes[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateMeanLDistRes(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;
  
  update[iBody].padDMeanLDtDistRes = malloc(body[iBody].iGravPerts*sizeof(double*));
  update[iBody].iaMeanLDistRes = malloc(body[iBody].iGravPerts*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTRES;
    update[iBody].iaMeanLDistRes[iPert] = (*iEqn)++;
  }
}


/***************** DISTRES Halts *****************/

void CountHaltsDistRes(HALT *halt,int *iNumHalts) {

}

void VerifyHaltDistRes(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {

}


/************* DISTRES Outputs ******************/

void WriteMeanL(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dMeanL;

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

void InitializeOutputDistRes(OUTPUT *output,fnWriteOutput fnWrite[]) {
  sprintf(output[OUT_MEANL].cName,"MeanL");
  sprintf(output[OUT_MEANL].cDescr,"Body's Mean Longitude");
  sprintf(output[OUT_MEANL].cNeg,"Deg");
  output[OUT_MEANL].bNeg = 1;
  output[OUT_MEANL].dNeg = 1./DEGRAD;
  output[OUT_MEANL].iNum = 1;
  output[OUT_MEANL].iModuleBit = DISTRES;
  fnWrite[OUT_MEANL] = &WriteMeanL;
}

/************ DISTRES Logging Functions **************/

void LogOptionsDistRes(CONTROL *control, FILE *fp) {

  fprintf(fp,"-------- DISTRES Options -----\n\n");
  
}

void LogDistRes(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  fprintf(fp,"\n----- DISTRES PARAMETERS ------\n");
  for (iOut=OUTSTARTDISTRES;iOut<OUTBODYSTARTDISTRES;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }
}

void LogBodyDistRes(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- DISTRES PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTDISTRES;iOut<OUTENDDISTRES;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

/************* MODULE Functions ***********/

void AddModuleDistRes(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = DISTRES;

  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyDistRes;
  module->fnCountHalts[iBody][iModule] = &CountHaltsDistRes;
  module->fnLogBody[iBody][iModule] = &LogBodyDistRes;

  module->fnReadOptions[iBody][iModule] = &ReadOptionsDistRes;
  module->fnVerify[iBody][iModule] = &VerifyDistRes;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltDistRes;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyDistRes;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateDistRes;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputDistRes;
  module->fnFinalizeUpdateHecc[iBody][iModule] = &FinalizeUpdateHeccDistRes;
  module->fnFinalizeUpdateKecc[iBody][iModule] = &FinalizeUpdateKeccDistRes;
  module->fnFinalizeUpdatePinc[iBody][iModule] = &FinalizeUpdatePincDistRes;
  module->fnFinalizeUpdateQinc[iBody][iModule] = &FinalizeUpdateQincDistRes;
  module->fnFinalizeUpdateSemi[iBody][iModule] = &FinalizeUpdateSemiDistRes;
  module->fnFinalizeUpdateMeanL[iBody][iModule] = &FinalizeUpdateMeanLDistRes;
}

/************* DistRes Functions ************/
void PropsAuxDistRes(BODY *body,EVOLVE *evolve,UPDATE *update,int iBody) { 
//   if (iBody > 0) {
//     body[iBody].dMeanA = body[iBody].dMeanL - body[iBody].dLongP
//   }
}

void ForceBehaviorDistRes(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
}


/* 
 * Semi-major axis functions
 */

#define A(iIndexJ) dAxRatio,iIndexJ,0.5
#define B(iIndexJ) dAxRatio,iIndexJ,1.5
#define C(iIndexJ) dAxRatio,iIndexJ,2.5

// A -> s = 1/2
// B -> s = 3/2
// C -> s = 5/2

/*--------- f27 ---------------------*/
double fdSemiMajAxF27(double dAxRatio, int iIndexJ) {
  return 0.5*(-2.*iIndexJ*fdLaplaceCoeff(A(iIndexJ)) - dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ)));
}

double fdDSemiF27Dalpha(double dAxRatio, int iIndexJ) {
  return 0.5*((-2.*iIndexJ-1.)*fdDerivLaplaceCoeff(1,A(iIndexJ))-dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ)));
}

/*--------- f31 ---------------------*/
double fdSemiMajAxF31(double dAxRatio, int iIndexJ) {
  return 0.5*((-1.0+2.*iIndexJ)*fdLaplaceCoeff(A(iIndexJ-1)) + dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ-1)));
}

double fdDSemiF31Dalpha(double dAxRatio, int iIndexJ) {
  return 0.5*((2.*iIndexJ)*fdDerivLaplaceCoeff(1,A(iIndexJ-1))+dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ-1)));
}

/*--------- f45 ---------------------*/
double fdSemiMajAxF45(double dAxRatio, int iIndexJ) {
  return 1./8*((-5.*iIndexJ+4.*pow(iIndexJ,2))*fdLaplaceCoeff(A(iIndexJ))\
        +(-2.+4.*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ))\
        +pow(dAxRatio,2)*fdDerivLaplaceCoeff(2,A(iIndexJ)));
}

double fdDSemiF45Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8*((-2.-1.*iIndexJ+4.*pow(iIndexJ,2))*fdDerivLaplaceCoeff(1,A(iIndexJ))\
        +(4*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ))\
        +pow(dAxRatio,2)*fdDerivLaplaceCoeff(3,A(iIndexJ)));
}

/*--------- f49 ---------------------*/
double fdSemiMajAxF49(double dAxRatio, int iIndexJ) {
  return 1./4*((-2.+6.*iIndexJ-4.*pow(iIndexJ,2))*fdLaplaceCoeff(A(iIndexJ-1))\
         +(2.-4.*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ-1))\
         -pow(dAxRatio,2)*fdDerivLaplaceCoeff(2,A(iIndexJ-1)));
}

double fdDSemiF49Dalpha(double dAxRatio, int iIndexJ) {
  return 1./4*((2.*iIndexJ-4.*pow(iIndexJ,2))*fdLaplaceCoeff(A(iIndexJ-1))\
         -4.*iIndexJ*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ-1))\
         -pow(dAxRatio,2)*fdDerivLaplaceCoeff(3,A(iIndexJ-1)));
}

/*--------- f53 ---------------------*/
double fdSemiMajAxF53(double dAxRatio, int iIndexJ) {
  return 1./8*((2.-7.*iIndexJ+4.*pow(iIndexJ,2))*fdLaplaceCoeff(A(iIndexJ-2))\
         +(-2.+4*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ-2))
         +pow(dAxRatio,2)*fdDerivLaplaceCoeff(2,A(iIndexJ-2)));
}

double fdDSemiF53Dalpha(double dAxRatio, int iIndexJ) {
  return 1./8*((-3.*iIndexJ+4.*pow(iIndexJ,2))*fdDerivLaplaceCoeff(1,A(iIndexJ-2))\
         +(4*iIndexJ)*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ-2))
         +pow(dAxRatio,2)*fdDerivLaplaceCoeff(3,A(iIndexJ-2)));
}

/*--------- f57 ---------------------*/
double fdSemiMajAxF57(double dAxRatio, int iIndexJ) {
  return 0.5*dAxRatio*fdLaplaceCoeff(B(iIndexJ-1));
}

double fdDSemiF57Dalpha(double dAxRatio, int iIndexJ) {
  return 0.5*(fdLaplaceCoeff(B(iIndexJ-1))+dAxRatio*fdDerivLaplaceCoeff(1,B(iIndexJ-1)));
}

/*--------- f62 ---------------------*/
double fdSemiMajAxF62(double dAxRatio, int iIndexJ) {
  return -dAxRatio*fdLaplaceCoeff(B(iIndexJ-1));
}

double fdDSemiF62Dalpha(double dAxRatio, int iIndexJ) {
  return -1.*(fdLaplaceCoeff(B(iIndexJ-1))+dAxRatio*fdDerivLaplaceCoeff(1,B(iIndexJ-1)));
}


//----------------Disturbing function a h k p q l-------------------------------------------
double fdLambdaArg(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ, int iOrder) {
  if (body[iBody].dSemi < body[jBody].dSemi) {
    return iIndexJ*body[jBody].dMeanL + (iOrder-iIndexJ)*body[iBody].dMeanL;
  } else {
    return iIndexJ*body[iBody].dMeanL + (iOrder-iIndexJ)*body[jBody].dMeanL;
  }
}

//--------dR/dh-----------(inner body)------------------------------------------------------

double fdDdistDhDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF27(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
         *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1));
}

double fdDdistDhDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDhDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF45(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *(-2.*body[iBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +body[iBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDhDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF49(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *(-body[jBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +body[jBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDhDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}
  
double fdDdistDhDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}  

double fdDdistDhDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
} 

double fdDdistDhDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}   
  
double fdDdisturbDHecc1st(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDhDir11(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

double fdDdisturbDHecc2nd(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDhDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDhDir22(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dh'------------------------------------------------------------------

double fdDdistDhPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDhPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF31(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) \
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1));
}

double fdDdistDhPrmDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDhPrmDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF49(body[jBody].dSemi/body[iBody].dSemi,iIndexJ)\
          *(-body[jBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +body[jBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDhPrmDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF53(body[jBody].dSemi/body[iBody].dSemi,iIndexJ)\
          *(-2.*body[iBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +body[iBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDhPrmDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDhPrmDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDhPrmDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdisturbDHeccPrime1st(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDhPrmDir12(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

double fdDdisturbDHeccPrime2nd(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDhPrmDir22(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDhPrmDir23(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

//------------dR/dk--------------------------------------------------------------

double fdDdistDkDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF27(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
         *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1));
}

double fdDdistDkDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDkDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF45(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *(2.*body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +body[iBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDkDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF49(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *(body[jBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +body[jBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDkDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDkDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDkDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdistDkDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;
}

double fdDdisturbDKecc1st(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDkDir11(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

double fdDdisturbDKecc2nd(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDkDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDkDir22(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dk'------------------------------------------------------------------

double fdDdistDkPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDkPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF31(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) \
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1));
}

double fdDdistDkPrmDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDkPrmDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF49(body[jBody].dSemi/body[iBody].dSemi,iIndexJ)\
          *(body[jBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +body[jBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDkPrmDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF53(body[jBody].dSemi/body[iBody].dSemi,iIndexJ)\
          *(2.*body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))
          +body[iBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDkPrmDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDkPrmDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDkPrmDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdisturbDKeccPrime1st(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDkPrmDir12(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

double fdDdisturbDKeccPrime2nd(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDkPrmDir22(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDkPrmDir23(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dp------------------------------------------------------------------

double fdDdistDpDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF57(body[iBody].dSemi/body[jBody].dSemi,iIndexJ) \
          *(-2.*body[iBody].dPinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +body[iBody].dQinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDpDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF62(body[iBody].dSemi/body[jBody].dSemi,iIndexJ) \
          *(-body[jBody].dPinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +body[jBody].dQinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDpDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdisturbDPinc1st(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  return 0;
}

double fdDdisturbDPinc2nd(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDpDir24(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpDir25(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dp'------------------------------------------------------------------

double fdDdistDpPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpPrmDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpPrmDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpPrmDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpPrmDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDpPrmDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF62(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) \
          *(-body[jBody].dPinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +body[jBody].dQinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDpPrmDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF57(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) \
          *(-2.*body[iBody].dPinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +body[iBody].dQinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdisturbDPincPrime1st(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  return 0;
}

double fdDdisturbDPincPrime2nd(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDpPrmDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDpPrmDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dq------------------------------------------------------------------

double fdDdistDqDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF57(body[iBody].dSemi/body[jBody].dSemi,iIndexJ) \
          *(2.*body[iBody].dQinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +body[iBody].dPinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDqDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF62(body[iBody].dSemi/body[jBody].dSemi,iIndexJ) \
          *(body[jBody].dQinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +body[jBody].dPinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDqDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdisturbDQinc1st(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  return 0;
}

double fdDdisturbDQinc2nd(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDqDir24(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqDir25(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dq'------------------------------------------------------------------

double fdDdistDqPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqPrmDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqPrmDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqPrmDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqPrmDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 0;  //true to second order (temporary)
}

double fdDdistDqPrmDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF62(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) \
          *(body[jBody].dQinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +body[jBody].dPinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDqPrmDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdSemiMajAxF57(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) \
          *(2.*body[iBody].dQinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +body[iBody].dPinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdisturbDQincPrime1st(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  return 0;
}

double fdDdisturbDQincPrime2nd(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDqPrmDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDqPrmDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dlambda------------------------------------------------------------------

double fdDdistDlDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.-iIndexJ)*fdSemiMajAxF27(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *(-body[iBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))\
          +(body[iBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))));
}

double fdDdistDlDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.-iIndexJ)*fdSemiMajAxF31(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *(-body[jBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))\
          +(body[jBody].dHecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))));
}

double fdDdistDlDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (2.-iIndexJ)*fdSemiMajAxF45(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *(-(pow(body[iBody].dKecc,2)-pow(body[iBody].dHecc,2))\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +(body[iBody].dHecc*body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDlDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (2.-iIndexJ)*fdSemiMajAxF49(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *(-(body[iBody].dKecc*body[jBody].dKecc-body[iBody].dHecc*body[jBody].dHecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +(body[iBody].dHecc*body[jBody].dKecc+body[iBody].dKecc*body[jBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDlDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (2.-iIndexJ)*fdSemiMajAxF45(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *(-(pow(body[jBody].dKecc,2)-pow(body[jBody].dHecc,2))\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +(body[jBody].dHecc*body[jBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDlDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (2.-iIndexJ)*fdSemiMajAxF57(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *(-(pow(body[iBody].dQinc,2)-pow(body[iBody].dPinc,2))\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +(body[iBody].dPinc*body[iBody].dQinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDlDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (2.-iIndexJ)*fdSemiMajAxF62(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *(-(body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +((body[jBody].dPinc*body[iBody].dQinc+body[iBody].dPinc*body[jBody].dQinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDlDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (2.-iIndexJ)*fdSemiMajAxF57(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *(-(pow(body[jBody].dQinc,2)-pow(body[jBody].dPinc,2))\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +(body[jBody].dPinc*body[jBody].dQinc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdisturbDLambda1st(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDlDir11(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlDir12(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

double fdDdisturbDLambda2nd(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDlDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir22(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDlDir23(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir24(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDlDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dlambda'------------------------------------------------------------------

double fdDdistDlPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir11(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(1.-iIndexJ)*fdDdistDlDir12(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir21(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir22(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir23(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir24(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir25(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDlPrmDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return (1.*iIndexJ)/(2.-iIndexJ)*fdDdistDlDir26(body,system,jBody,iBody,iIndexJ);
}

double fdDdisturbDLambdaPrime1st(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDlPrmDir11(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDlPrmDir12(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

double fdDdisturbDLambdaPrime2nd(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDlPrmDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir22(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDlPrmDir23(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir24(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDlPrmDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDlPrmDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/da------------------------------------------------------------------
double fdDdistDaDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./body[jBody].dSemi*fdDSemiF27Dalpha(body[iBody].dSemi/body[jBody].dSemi, iIndexJ) \
         *(body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))\
         +body[iBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./body[jBody].dSemi*fdDSemiF31Dalpha(body[iBody].dSemi/body[jBody].dSemi, iIndexJ) \
         *(body[jBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1))\
         +body[jBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./body[jBody].dSemi*fdDSemiF45Dalpha(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *((pow(body[iBody].dKecc,2)-pow(body[iBody].dHecc,2))\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +(body[iBody].dHecc*body[iBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDaDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./body[jBody].dSemi*fdDSemiF49Dalpha(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *((body[iBody].dKecc*body[jBody].dKecc-body[iBody].dHecc*body[jBody].dHecc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +(body[iBody].dHecc*body[jBody].dKecc+body[iBody].dKecc*body[jBody].dHecc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2)));
}

double fdDdistDaDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./body[jBody].dSemi*fdDSemiF49Dalpha(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *((pow(body[jBody].dKecc,2)-pow(body[jBody].dHecc,2))\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +(body[jBody].dHecc*body[jBody].dKecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDaDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./body[jBody].dSemi*fdDSemiF57Dalpha(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *((pow(body[iBody].dQinc,2)-pow(body[iBody].dPinc,2))\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +(body[iBody].dPinc*body[iBody].dQinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDaDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./body[jBody].dSemi*fdDSemiF62Dalpha(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *((body[iBody].dQinc*body[jBody].dQinc-body[iBody].dPinc*body[jBody].dPinc)\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +((body[jBody].dPinc*body[iBody].dQinc+body[iBody].dPinc*body[jBody].dQinc)\
          *sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdistDaDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return 1./body[jBody].dSemi*fdDSemiF57Dalpha(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)\
          *((pow(body[jBody].dQinc,2)-pow(body[jBody].dPinc,2))\
          *cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))\
          +(body[jBody].dPinc*body[jBody].dQinc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,2))));
}

double fdDdisturbDSemi1st(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDaDir11(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaDir12(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

double fdDdisturbDSemi2nd(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDaDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir22(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDaDir23(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir24(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDaDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/da'------------------------------------------------------------------
double fdDistDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdDSemiF27Dalpha(body[iBody].dSemi/body[jBody].dSemi, iIndexJ) * \
          (body[iBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)) +\
          body[iBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDistDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return fdDSemiF31Dalpha(body[iBody].dSemi/body[jBody].dSemi, iIndexJ) * \
          (body[jBody].dKecc*cos(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)) +\
          body[jBody].dHecc*sin(fdLambdaArg(body,system,iBody,jBody,iIndexJ,1)));
}

double fdDdistDaPrmDir11(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir11(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir11(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDaPrmDir12(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir12(body,system,jBody,iBody,iIndexJ)\
         -1./body[iBody].dSemi*fdDistDir12(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDaPrmDir21(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir21(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDaPrmDir22(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir22(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDaPrmDir23(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir23(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDaPrmDir24(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir24(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDaPrmDir25(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir25(body,system,jBody,iBody,iIndexJ);
}

double fdDdistDaPrmDir26(BODY *body, SYSTEM *system, int iBody, int jBody, int iIndexJ) {
  return -body[jBody].dSemi/body[iBody].dSemi*fdDdistDaDir26(body,system,jBody,iBody,iIndexJ);
}

double fdDdisturbDSemiPrime1st(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDaPrmDir11(body, system, iaBody[0], iaBody[1], iIndexJ)\
      + fdDdistDaPrmDir12(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

double fdDdisturbDSemiPrime2nd(BODY *body, SYSTEM *system, int *iaBody, int iIndexJ) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDaPrmDir21(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir22(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDaPrmDir23(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir24(body, system, iaBody[0], iaBody[1], iIndexJ)
    + fdDdistDaPrmDir25(body, system, iaBody[0], iaBody[1], iIndexJ) \
    + fdDdistDaPrmDir26(body, system, iaBody[0], iaBody[1], iIndexJ) ) * dMfac/dSemiPrm;
  return y;
}

//-------------------DistRes's equations in a h k p q l (2nd order direct integration RD2)--------------------
double fdDistResRD2DaDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;
  //Here, iaBody[0] = body in question, iaBody[1] = perturber
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
  
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += 2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu) * \
          fdDdisturbDLambda1st(body,system,iaBody,3); //let's just try the 3:2 res
  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += 2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu) * \
          fdDdisturbDLambdaPrime1st(body,system,iaBody,3);
  }
  
  return sum/DAYSEC*AUCM;
}

double fdDistResRD2DlDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;
  //Here, iaBody[0] = body in question, iaBody[1] = perturber
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
  
  sum += sqrt(dMu/pow(body[iaBody[0]].dSemi/AUCM,3));
  y = fabs(1.-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2));
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += -2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu)\
          *fdDdisturbDSemi1st(body,system,iaBody,3)\
          +( sqrt(y)/(1.+sqrt(y))
          *(body[iaBody[0]].dHecc*fdDdisturbDHecc1st(body,system,iaBody,3)\
          +body[iaBody[0]].dKecc*fdDdisturbDKecc1st(body,system,iaBody,3))\
          +0.5/sqrt(y)*(body[iaBody[0]].dPinc*fdDdisturbDPinc1st(body,system,iaBody,3)\
          +body[iaBody[0]].dQinc*fdDdisturbDQinc1st(body,system,iaBody,3)) )\
          /sqrt(dMu*body[iaBody[0]].dSemi/AUCM);     
  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += -2.0*sqrt(body[iaBody[0]].dSemi/AUCM/dMu)\
          *fdDdisturbDSemiPrime1st(body,system,iaBody,3)\
          +( sqrt(y)/(1.+sqrt(y))
          *(body[iaBody[0]].dHecc*fdDdisturbDHeccPrime1st(body,system,iaBody,3)\
          +body[iaBody[0]].dKecc*fdDdisturbDKeccPrime1st(body,system,iaBody,3))\
          +0.5/sqrt(y)*(body[iaBody[0]].dPinc*fdDdisturbDPincPrime1st(body,system,iaBody,3)\
          +body[iaBody[0]].dQinc*fdDdisturbDQincPrime1st(body,system,iaBody,3)) )\
          /sqrt(dMu*body[iaBody[0]].dSemi/AUCM);     
  }
  
  return sum/DAYSEC;

}

double fdDistResRD2DhDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;
  //Here, iaBody[0] = body in question, iaBody[1] = perturber

  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
  y = fabs(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2));
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += ( sqrt(y)*fdDdisturbDKecc1st(body, system, iaBody,3) + \
    body[iaBody[0]].dKecc*(body[iaBody[0]].dPinc*fdDdisturbDPinc1st(body, system, iaBody,3)\
    +body[iaBody[0]].dQinc*fdDdisturbDQinc1st(body, system, iaBody,3))/(2*sqrt(y)) \
    -body[iaBody[0]].dHecc*sqrt(y)/(1.+sqrt(y))*fdDdisturbDLambda1st(body,system,iaBody,3) )\
    / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      
  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += ( sqrt(y)*fdDdisturbDKeccPrime1st(body, system, iaBody,3) + \
    body[iaBody[0]].dKecc*(body[iaBody[0]].dPinc*fdDdisturbDPincPrime1st(body, system, iaBody,3)\
    +body[iaBody[0]].dQinc*fdDdisturbDQincPrime1st(body, system, iaBody,3))/(2*sqrt(y)) \
    -body[iaBody[0]].dHecc*sqrt(y)/(1.+sqrt(y))*fdDdisturbDLambdaPrime1st(body,system,iaBody,3) )\
    / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
  }
  
  return sum/DAYSEC;
}

double fdDistResRD2DkDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;
  
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
  y = fabs(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2));
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += -( sqrt(y)*fdDdisturbDHecc1st(body, system, iaBody,3) + \
    body[iaBody[0]].dHecc*(body[iaBody[0]].dPinc*fdDdisturbDPinc1st(body, system, iaBody,3)\
    +body[iaBody[0]].dQinc*fdDdisturbDQinc1st(body, system, iaBody,3))/(2*sqrt(y)) \
    +body[iaBody[0]].dKecc*sqrt(y)/(1.+sqrt(y))*fdDdisturbDLambda1st(body,system,iaBody,3) ) \
    / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      
  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += -( sqrt(y)*fdDdisturbDHeccPrime1st(body, system, iaBody,3) + \
    body[iaBody[0]].dHecc*(body[iaBody[0]].dPinc*fdDdisturbDPincPrime1st(body, system, iaBody,3)\
    +body[iaBody[0]].dQinc*fdDdisturbDQincPrime1st(body, system, iaBody,3))/(2*sqrt(y)) \
    +body[iaBody[0]].dKecc*sqrt(y)/(1.+sqrt(y))*fdDdisturbDLambdaPrime1st(body,system,iaBody,3) )\
    / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
  }      

  return sum/DAYSEC;
}

double fdDistResRD2DpDt(BODY *body, SYSTEM *system, int *iaBody) {
    double sum = 0.0, dMu, y;
    
    dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
    y = fabs(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2));
    if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dPinc*(-body[iaBody[0]].dKecc*\
      fdDdisturbDHecc1st(body,system,iaBody,3)+body[iaBody[0]].dHecc*\
      fdDdisturbDKecc1st(body,system,iaBody,3))+1.0/2.0*fdDdisturbDQinc1st(body,system,iaBody,3)\
      -body[iaBody[0]].dPinc*fdDdisturbDLambda1st(body,system,iaBody,3) )\
      /(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
    } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dPinc*(-body[iaBody[0]].dKecc*\
      fdDdisturbDHeccPrime1st(body,system,iaBody,3)+body[iaBody[0]].dHecc*\
      fdDdisturbDKeccPrime1st(body,system,iaBody,3))+1.0/2.0*\
      fdDdisturbDQincPrime1st(body,system,iaBody,3)\
      -body[iaBody[0]].dPinc*fdDdisturbDLambdaPrime1st(body,system,iaBody,3) )\
      /(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
    }
    
    return sum/DAYSEC;
}


double fdDistResRD2DqDt(BODY *body, SYSTEM *system, int *iaBody) {
    double sum = 0.0, dMu, y;
    
    dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
    y = fabs(1-pow(body[iaBody[0]].dHecc,2)-pow(body[iaBody[0]].dKecc,2));
    if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dQinc*(-body[iaBody[0]].dKecc*\
      fdDdisturbDHecc1st(body,system,iaBody,3)+body[iaBody[0]].dHecc*\
      fdDdisturbDKecc1st(body,system,iaBody,3))-1.0/2.0*fdDdisturbDPinc1st(body,system,iaBody,3)\
      -body[iaBody[0]].dQinc*fdDdisturbDLambda1st(body,system,iaBody,3) )\
      /(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
    } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dQinc*(-body[iaBody[0]].dKecc*\
      fdDdisturbDHeccPrime1st(body,system,iaBody,3)+body[iaBody[0]].dHecc*\
      fdDdisturbDKeccPrime1st(body,system,iaBody,3))-1.0/2.0*\
      fdDdisturbDPincPrime1st(body,system,iaBody,3) \
      -body[iaBody[0]].dQinc*fdDdisturbDLambdaPrime1st(body,system,iaBody,3) )\
      /(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(y)));
    }
   
    return sum/DAYSEC;
}

