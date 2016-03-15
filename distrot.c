/********************** DISTROT.C **********************/
/*
 * Russell Deitrick, July 7, 2015
 *
 * Subroutines that control the integration of the obliquity model. 
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"

void InitializeControlDistRot(CONTROL *control) {
  /* Not sure if I need anything here yet */
}

void InitializeModuleDistRot(CONTROL *control,MODULE *module) {
  /* Anything here? */
}

void BodyCopyDistRot(BODY *dest,BODY *src,int iTideModel,int iBody) {
  int iIndex,iPert;

  dest[iBody].dXobl = src[iBody].dXobl;
  dest[iBody].dYobl = src[iBody].dYobl;
  dest[iBody].dZobl = src[iBody].dZobl;
  dest[iBody].dDynEllip = src[iBody].dDynEllip;
  dest[iBody].bForcePrecRate = src[iBody].bForcePrecRate;
  dest[iBody].dPrecRate = src[iBody].dPrecRate;

}

void InitializeBodyDistRot(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
}

void InitializeUpdateTmpBodyDistRot(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
}

/**************** DISTROT options ********************/

void ReadDynEllip(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* Cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0 || dTmp >= 1) {
      if (control->Io.iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: %s must be in the range [0,1).\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);  
    }
    body[iFile-1].dDynEllip = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);

  } else
    AssignDefaultDouble(options,&body[iFile-1].dDynEllip,files->iNumInputs);
    
}

void ReadCalcDynEllip(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].bCalcDynEllip = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bCalcDynEllip,files->iNumInputs);
}

void ReadForcePrecRate(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].bForcePrecRate = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bForcePrecRate,files->iNumInputs);
}

void ReadPrecRate(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* Cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    
    body[iFile-1].dPrecRate = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultDouble(options,&body[iFile-1].dPrecRate,files->iNumInputs);
    
}

void InitializeOptionsDistRot(OPTIONS *options,fnReadOption fnRead[]) {
  
  sprintf(options[OPT_DYNELLIP].cName,"dDynEllip");
  sprintf(options[OPT_DYNELLIP].cDescr,"Planet's dynamical ellipticity");
  sprintf(options[OPT_DYNELLIP].cDefault,"0");
  options[OPT_DYNELLIP].dDefault = 0.0;
  options[OPT_DYNELLIP].iType = 2;  
  options[OPT_DYNELLIP].iMultiFile = 1;   
  fnRead[OPT_DYNELLIP] = &ReadDynEllip;
  
  sprintf(options[OPT_CALCDYNELLIP].cName,"bCalcDynEllip");
  sprintf(options[OPT_CALCDYNELLIP].cDescr,"Calculate dynamical ellipticity from RotRate");
  sprintf(options[OPT_CALCDYNELLIP].cDefault,"0");
  options[OPT_CALCDYNELLIP].dDefault = 0;
  options[OPT_CALCDYNELLIP].iType = 0;  
  options[OPT_CALCDYNELLIP].iMultiFile = 1; 
  fnRead[OPT_CALCDYNELLIP] = &ReadCalcDynEllip;
  
  sprintf(options[OPT_FORCEPRECRATE].cName,"bForcePrecRate");
  sprintf(options[OPT_FORCEPRECRATE].cDescr,"Set the axial precession to a fixed rate");
  sprintf(options[OPT_FORCEPRECRATE].cDefault,"0");
  options[OPT_FORCEPRECRATE].dDefault = 0;
  options[OPT_FORCEPRECRATE].iType = 0;  
  options[OPT_FORCEPRECRATE].iMultiFile = 1; 
  fnRead[OPT_FORCEPRECRATE] = &ReadForcePrecRate;
  
  sprintf(options[OPT_PRECRATE].cName,"dPrecRate");
  sprintf(options[OPT_PRECRATE].cDescr,"Fixed rate of axial precession (rad/s)");
  sprintf(options[OPT_PRECRATE].cDefault,"7.7261e-12");
  options[OPT_PRECRATE].dDefault = 7.7261e-12;
  options[OPT_PRECRATE].iType = 2;  
  options[OPT_PRECRATE].iMultiFile = 1;   
  fnRead[OPT_PRECRATE] = &ReadPrecRate;
  
}

void ReadOptionsDistRot(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTDISTROT;iOpt<OPTENDDISTROT;iOpt++) { 
      if (options[iOpt].iType != -1) {
        fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
      }
  }
}
    

/******************* Verify DISTROT ******************/

/* In the following, iBody is the current body number that is getting assigned,
   iPert counts the number of bodies perturbing iBody, and iBodyPert is the
   body number of the current perturbing body. */

/* The indexing gets REEAAALLY confusing here. iPert = 0 to iGravPerts-1 correspond
   to all perturbing planets, iPert = iGravPerts corresponds to the stellar torque, and
   iPert = iGravPerts+1 to the stellar general relativistic correction, if applied */

void InitializeXoblDistRot(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] = 2;
  update[iBody].padDXoblDtDistRot[iPert] = &update[iBody].daDerivProc[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]];
  update[iBody].iNumBodies[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]]=2;
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeYoblDistRot(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] = 2;
  update[iBody].padDYoblDtDistRot[iPert] = &update[iBody].daDerivProc[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]];
  update[iBody].iNumBodies[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]]=2;
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeZoblDistRot(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]] = 2;
  update[iBody].padDZoblDtDistRot[iPert] = &update[iBody].daDerivProc[update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]];
  update[iBody].iNumBodies[update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]]=2;
  update[iBody].iaBody[update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeXoblDistRotStar(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] = 2;
  update[iBody].padDXoblDtDistRot[iPert] = &update[iBody].daDerivProc[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]];
  update[iBody].iNumBodies[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]]=2;
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]][1] = 0;
}

void InitializeYoblDistRotStar(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] = 2;
  update[iBody].padDYoblDtDistRot[iPert] = &update[iBody].daDerivProc[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]];
  update[iBody].iNumBodies[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]]=2;
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]][1] = 0;
}

void VerifyDynEllip(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody,int iVerbose) {
  if (body[iBody].bCalcDynEllip == 1) {
    /* check if bCalcDynEllip and dDynEllip are both set */
    if (options[OPT_DYNELLIP].iLine[iBody+1] > -1) {
      fprintf(stderr,"WARNING: %s set in file %s, but %s set to 1. %s will be overridden.\n",options[OPT_DYNELLIP].cName,cFile,options[OPT_CALCDYNELLIP].cName,options[OPT_DYNELLIP].cName);
    } 
    CalcDynEllip(body,iBody);
  }
}
 
void VerifyDistRot(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int i, j=0, iPert=0, jBody=0;
     
  /* The indexing gets REEAAALLY confusing here. iPert = 0 to iGravPerts-1 correspond to all perturbing planets, iPert = iGravPerts corresponds to the stellar torque, and iPert = iGravPerts+1 to the stellar general relativistic correction, if applied */
  
  if (iBody >= 1) {
    control->Evolve.fnPropsAux[iBody][iModule] = &PropertiesDistRot;
    VerifyDynEllip(body,control,options,files->Infile[iBody+1].cIn,iBody,control->Io.iVerbose);
    
    CalcXYZobl(body, iBody);
    
    body[iBody].dLRot = malloc(3*sizeof(double));
    body[iBody].dLRotTmp = malloc(3*sizeof(double));
    
    if (control->Evolve.iDistOrbModel==RD4) {
      /* Body updates */
      for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
        /* x = sin(obl)*cos(pA) */
        InitializeXoblDistRot(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] = &fdDistRotRD4DxDt;
        
        /* y = sin(obl)*sin(pA) */
        InitializeYoblDistRot(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] = &fdDistRotRD4DyDt;
        
        /* z = cos(obl) */
        InitializeZoblDistRot(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]] = &fdDistRotRD4DzDt;
          
      }
      /* Body updates for stellar torque, treating star as "perturber" (only needed for x and y -> pA) */
      /* x = sin(obl)*cos(pA) */
      InitializeXoblDistRotStar(body,update,iBody,body[iBody].iGravPerts);
      fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblDistRot[body[iBody].iGravPerts]] = &fdDistRotRD4DxDt;
        
      /* y = sin(obl)*sin(pA) */
      InitializeYoblDistRotStar(body,update,iBody,body[iBody].iGravPerts);
      fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblDistRot[body[iBody].iGravPerts]] = &fdDistRotRD4DyDt;
      
    } else if (control->Evolve.iDistOrbModel==LL2) {
      /* Body updates */
      for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
        /* x = sin(obl)*cos(pA) */
        InitializeXoblDistRot(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] = &fdDistRotLL2DxDt;
        
        /* y = sin(obl)*sin(pA) */
        InitializeYoblDistRot(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] = &fdDistRotLL2DyDt;
        
        /* z = cos(obl) */
        InitializeZoblDistRot(body,update,iBody,iPert);
        fnUpdate[iBody][update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]] = &fdDistRotLL2DzDt;
          
      }
      /* Body updates for stellar torque, treating star as "perturber" (only needed for x and y -> pA) */
      /* x = sin(obl)*cos(pA) */
      InitializeXoblDistRotStar(body,update,iBody,body[iBody].iGravPerts);
      fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblDistRot[body[iBody].iGravPerts]] = &fdDistRotLL2DxDt;
        
      /* y = sin(obl)*sin(pA) */
      InitializeYoblDistRotStar(body,update,iBody,body[iBody].iGravPerts);
      fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblDistRot[body[iBody].iGravPerts]] = &fdDistRotLL2DyDt;
    }
    if (body[iBody].bGRCorr) {
      InitializeXoblDistRotStar(body,update,iBody,body[iBody].iGravPerts+1);
      fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblDistRot[body[iBody].iGravPerts+1]] = &fdAxialGRDxDt;
 
      InitializeYoblDistRotStar(body,update,iBody,body[iBody].iGravPerts+1);
      fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblDistRot[body[iBody].iGravPerts+1]] = &fdAxialGRDyDt;
    }
  }
  
  control->fnForceBehavior[iBody][iModule]=&ForceBehaviorDistRot;
  control->Evolve.fnBodyCopy[iBody][iModule]=&BodyCopyDistRot;
}


/***************** DISTROT Update *****************/
void InitializeUpdateDistRot(BODY *body,UPDATE *update,int iBody) {
  if (iBody > 0) {
    if (update[iBody].iNumXobl == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumXobl += body[iBody].iGravPerts+1;

    if (update[iBody].iNumYobl == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumYobl += body[iBody].iGravPerts+1;

    if (update[iBody].iNumZobl == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumZobl += body[iBody].iGravPerts;
    
    if (body[iBody].bGRCorr) {
      update[iBody].iNumXobl += 1;
      update[iBody].iNumYobl += 1;
    }
  }
}

void FinalizeUpdateXoblDistRot(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;
  
  /* The indexing gets REEAAALLY confusing here. iPert = 0 to iGravPerts-1 correspond to all perturbing planets, iPert = iGravPerts corresponds to the stellar torque, and iPert = iGravPerts+1 to the stellar general relativistic correction, if applied */
  
  if (body[iBody].bGRCorr) {
    update[iBody].padDXoblDtDistRot = malloc((body[iBody].iGravPerts+2)*sizeof(double*));
    update[iBody].iaXoblDistRot = malloc((body[iBody].iGravPerts+2)*sizeof(int));
    for (iPert=0;iPert<body[iBody].iGravPerts+2;iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTROT;
      update[iBody].iaXoblDistRot[iPert] = (*iEqn)++;
    }
  } else {
    update[iBody].padDXoblDtDistRot = malloc((body[iBody].iGravPerts+1)*sizeof(double*));
    update[iBody].iaXoblDistRot = malloc((body[iBody].iGravPerts+1)*sizeof(int));
    for (iPert=0;iPert<body[iBody].iGravPerts+1;iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTROT;
      update[iBody].iaXoblDistRot[iPert] = (*iEqn)++;
    }
  }
}

void FinalizeUpdateYoblDistRot(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;
  
  /* The indexing gets REEAAALLY confusing here. iPert = 0 to iGravPerts-1 correspond to all perturbing planets, iPert = iGravPerts corresponds to the stellar torque, and iPert = iGravPerts+1 to the stellar general relativistic correction, if applied */
  
  if (body[iBody].bGRCorr) {
    update[iBody].padDYoblDtDistRot = malloc((body[iBody].iGravPerts+2)*sizeof(double*));
    update[iBody].iaYoblDistRot = malloc((body[iBody].iGravPerts+2)*sizeof(int));
    for (iPert=0;iPert<body[iBody].iGravPerts+2;iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTROT;
      update[iBody].iaYoblDistRot[iPert] = (*iEqn)++;
    }
  } else { 
    update[iBody].padDYoblDtDistRot = malloc((body[iBody].iGravPerts+1)*sizeof(double*));
    update[iBody].iaYoblDistRot = malloc((body[iBody].iGravPerts+1)*sizeof(int));
    for (iPert=0;iPert<body[iBody].iGravPerts+1;iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTROT;
      update[iBody].iaYoblDistRot[iPert] = (*iEqn)++;
    }
  }
}

void FinalizeUpdateZoblDistRot(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  int iPert;
  
  update[iBody].padDZoblDtDistRot = malloc((body[iBody].iGravPerts)*sizeof(double*));
  update[iBody].iaZoblDistRot = malloc((body[iBody].iGravPerts)*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTROT;
    update[iBody].iaZoblDistRot[iPert] = (*iEqn)++;
  }
}


/***************** DISTROT Halts *****************/

void CountHaltsDistRot(HALT *halt,int *iNumHalts) { 
}

void VerifyHaltDistRot(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
}


/************* DISTROT Outputs ******************/

void WriteBodyDOblDtDistRot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv, dObldx, dObldy, dObldz;
  int iPert;

  dObldx = body[iBody].dXobl*body[iBody].dZobl/(sqrt(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)) * \
    (pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)+pow(body[iBody].dZobl,2)));
  dObldy = body[iBody].dYobl*body[iBody].dZobl/(sqrt(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)) * \
    (pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)+pow(body[iBody].dZobl,2)));
  dObldz = - sqrt(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)) / \
    (pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)+pow(body[iBody].dZobl,2));
    
  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
     dDeriv += dObldx*(*(update[iBody].padDXoblDtDistRot[iPert])) + dObldy*(*(update[iBody].padDYoblDtDistRot[iPert]))\
        + dObldz*(*(update[iBody].padDZoblDtDistRot[iPert]));
 
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}

void WriteBodyDPrecADtDistRot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv, dpAdx, dpAdy;
  int iPert;

  dpAdx = - body[iBody].dYobl/(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2));
  dpAdy = body[iBody].dXobl/(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2));
  
  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<=body[iBody].iGravPerts;iPert++) 
    dDeriv += dpAdx*(*(update[iBody].padDXoblDtDistRot[iPert])) + dpAdy*(*(update[iBody].padDYoblDtDistRot[iPert]));

  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}  
  
void WriteBodyDXoblDtDistRot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<=body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDXoblDtDistRot[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}  

void WriteBodyDYoblDtDistRot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<=body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDYoblDtDistRot[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
} 

void WriteBodyDZoblDtDistRot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDZoblDtDistRot[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
} 

void WriteBodyPrecA(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = atan2(body[iBody].dYobl,body[iBody].dXobl);  
  
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
  
void WriteBodyCassOne(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double h, inc, longa, Lnorm=0.0, obliq, eqnode;
  int i, jBody;
  
  for (i=0;i<3;i++) system->dLOrb[i] = 0.0;
  
  for (jBody=1;jBody<control->Evolve.iNumBodies;jBody++) {
    h = body[jBody].dMass/MSUN*KGAUSS*sqrt((body[0].dMass+body[jBody].dMass)/MSUN*\
        body[jBody].dSemi/AUCM* (1.-pow(body[jBody].dHecc,2)-pow(body[jBody].dKecc,2)));
    body[jBody].dLOrb[0] = 0.0;
    body[jBody].dLOrb[1] = 0.0;
    body[jBody].dLOrb[2] = h;
  
    inc = 2*asin(sqrt(pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2)));
    RotateVector(body[jBody].dLOrb,body[jBody].dLOrbTmp,inc,0); //rotate about x by inc angle
    longa = atan2(body[jBody].dPinc,body[jBody].dQinc);
    RotateVector(body[jBody].dLOrbTmp,body[jBody].dLOrb,longa,2); //rotate about z by Omega
    for (i=0;i<3;i++)
      system->dLOrb[i] += body[jBody].dLOrb[i];
  }
  Lnorm = sqrt(pow(system->dLOrb[0],2)+pow(system->dLOrb[1],2)+pow(system->dLOrb[2],2));
  for (i=0;i<3;i++) system->dLOrb[i] /= Lnorm;
  
  Lnorm = sqrt(pow(body[iBody].dLOrb[0],2)+pow(body[iBody].dLOrb[1],2)+pow(body[iBody].dLOrb[2],2));
  for (i=0;i<3;i++) body[iBody].dLOrb[i] /= Lnorm;
  
  body[iBody].dLRot[0] = 0.0;
  body[iBody].dLRot[1] = 0.0;
  body[iBody].dLRot[2] = 1.0;
  obliq = atan2(sqrt(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)),body[iBody].dZobl);

  inc = 2*asin(sqrt(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2)));
  longa = atan2(body[iBody].dPinc,body[iBody].dQinc);
  RotateVector(body[iBody].dLRot,body[iBody].dLRotTmp,-obliq,0);
  eqnode = 2*PI - atan2(body[iBody].dYobl,body[iBody].dXobl) - longa; 
  RotateVector(body[iBody].dLRotTmp,body[iBody].dLRot,eqnode,2);
  RotateVector(body[iBody].dLRot,body[iBody].dLRotTmp,inc,0);
  RotateVector(body[iBody].dLRotTmp,body[iBody].dLRot,longa,2);

  cross(body[iBody].dLRot,body[iBody].dLOrb,body[iBody].dLRotTmp);
  Lnorm = sqrt(pow(body[iBody].dLRotTmp[0],2)+pow(body[iBody].dLRotTmp[1],2)+pow(body[iBody].dLRotTmp[2],2));
  for (i=0;i<3;i++) body[iBody].dLRotTmp[i] /= Lnorm;
  
  cross(system->dLOrb,body[iBody].dLOrb,body[iBody].dLOrbTmp);
  Lnorm = sqrt(pow(body[iBody].dLOrbTmp[0],2)+pow(body[iBody].dLOrbTmp[1],2)+pow(body[iBody].dLOrbTmp[2],2));
  for (i=0;i<3;i++) body[iBody].dLOrbTmp[i] /= Lnorm;
  
  cross(body[iBody].dLOrbTmp,body[iBody].dLRotTmp,system->dLOrb);
  *dTmp = sqrt(pow(system->dLOrb[0],2)+pow(system->dLOrb[1],2)+pow(system->dLOrb[2],2));
}  
  
void WriteBodyCassTwo(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double h, inc, longa, Lnorm=0.0, obliq, eqnode;
  int i, jBody;
  
  for (i=0;i<3;i++) system->dLOrb[i] = 0.0;
  
  for (jBody=1;jBody<control->Evolve.iNumBodies;jBody++) {
    h = body[jBody].dMass/MSUN*KGAUSS*sqrt((body[0].dMass+body[jBody].dMass)/MSUN*\
        body[jBody].dSemi/AUCM* (1.-pow(body[jBody].dHecc,2)-pow(body[jBody].dKecc,2)));
    body[jBody].dLOrb[0] = 0.0;
    body[jBody].dLOrb[1] = 0.0;
    body[jBody].dLOrb[2] = h;
  
    inc = 2*asin(sqrt(pow(body[jBody].dPinc,2)+pow(body[jBody].dQinc,2)));
    RotateVector(body[jBody].dLOrb,body[jBody].dLOrbTmp,inc,0); //rotate about x by inc angle
    longa = atan2(body[jBody].dPinc,body[jBody].dQinc);
    RotateVector(body[jBody].dLOrbTmp,body[jBody].dLOrb,longa,2); //rotate about z by Omega
    for (i=0;i<3;i++)
      system->dLOrb[i] += body[jBody].dLOrb[i];
  }
  Lnorm = sqrt(pow(system->dLOrb[0],2)+pow(system->dLOrb[1],2)+pow(system->dLOrb[2],2));
  for (i=0;i<3;i++) system->dLOrb[i] /= Lnorm;
  
  Lnorm = sqrt(pow(body[iBody].dLOrb[0],2)+pow(body[iBody].dLOrb[1],2)+pow(body[iBody].dLOrb[2],2));
  for (i=0;i<3;i++) body[iBody].dLOrb[i] /= Lnorm;
  
  body[iBody].dLRot[0] = 0.0;
  body[iBody].dLRot[1] = 0.0;
  body[iBody].dLRot[2] = 1.0;
  obliq = atan2(sqrt(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)),body[iBody].dZobl);

  inc = 2*asin(sqrt(pow(body[iBody].dPinc,2)+pow(body[iBody].dQinc,2)));
  longa = atan2(body[iBody].dPinc,body[iBody].dQinc);
  RotateVector(body[iBody].dLRot,body[iBody].dLRotTmp,-obliq,0);
  eqnode = 2*PI - atan2(body[iBody].dYobl,body[iBody].dXobl) - longa; 
  RotateVector(body[iBody].dLRotTmp,body[iBody].dLRot,eqnode,2);
  RotateVector(body[iBody].dLRot,body[iBody].dLRotTmp,inc,0);
  RotateVector(body[iBody].dLRotTmp,body[iBody].dLRot,longa,2);

  cross(body[iBody].dLRot,body[iBody].dLOrb,body[iBody].dLRotTmp);
  Lnorm = sqrt(pow(body[iBody].dLRotTmp[0],2)+pow(body[iBody].dLRotTmp[1],2)+pow(body[iBody].dLRotTmp[2],2));
  for (i=0;i<3;i++) body[iBody].dLRotTmp[i] /= Lnorm;
  
  cross(system->dLOrb,body[iBody].dLOrb,body[iBody].dLOrbTmp);
  Lnorm = sqrt(pow(body[iBody].dLOrbTmp[0],2)+pow(body[iBody].dLOrbTmp[1],2)+pow(body[iBody].dLOrbTmp[2],2));
  for (i=0;i<3;i++) body[iBody].dLOrbTmp[i] /= Lnorm;
  
  *dTmp = 0.0;
  for (i=0;i<3;i++) *dTmp += body[iBody].dLRotTmp[i]*body[iBody].dLOrbTmp[i];  
}  
  
void InitializeOutputDistRot(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_DOBLDTDISTROT].cName,"DOblDtDistRot");
  sprintf(output[OUT_DOBLDTDISTROT].cDescr,"Body's dobl/dt in DistRot");
  sprintf(output[OUT_DOBLDTDISTROT].cNeg,"deg/year");
  output[OUT_DOBLDTDISTROT].bNeg = 1;
  output[OUT_DOBLDTDISTROT].dNeg = YEARSEC/DEGRAD;
  output[OUT_DOBLDTDISTROT].iNum = 1;
  fnWrite[OUT_DOBLDTDISTROT] = &WriteBodyDOblDtDistRot;
  
  sprintf(output[OUT_DPRECADTDISTROT].cName,"DPrecADtDistRot");
  sprintf(output[OUT_DPRECADTDISTROT].cDescr,"Body's dpA/dt in DistRot");
  sprintf(output[OUT_DPRECADTDISTROT].cNeg,"deg/year");
  output[OUT_DPRECADTDISTROT].bNeg = 1;
  output[OUT_DPRECADTDISTROT].dNeg = YEARSEC/DEGRAD;
  output[OUT_DPRECADTDISTROT].iNum = 1;
  fnWrite[OUT_DPRECADTDISTROT] = &WriteBodyDPrecADtDistRot;
 
  sprintf(output[OUT_DXOBLDTDISTROT].cName,"DXoblDtDistRot");
  sprintf(output[OUT_DXOBLDTDISTROT].cDescr,"Body's dXobl/dt in DistRot");
  sprintf(output[OUT_DXOBLDTDISTROT].cNeg,"1/year");
  output[OUT_DXOBLDTDISTROT].bNeg = 1;
  output[OUT_DXOBLDTDISTROT].dNeg = YEARSEC;
  output[OUT_DXOBLDTDISTROT].iNum = 1;
  fnWrite[OUT_DXOBLDTDISTROT] = &WriteBodyDXoblDtDistRot;
  
  sprintf(output[OUT_DYOBLDTDISTROT].cName,"DYoblDtDistRot");
  sprintf(output[OUT_DYOBLDTDISTROT].cDescr,"Body's dYobl/dt in DistRot");
  sprintf(output[OUT_DYOBLDTDISTROT].cNeg,"1/yr");
  output[OUT_DYOBLDTDISTROT].bNeg = 1;
  output[OUT_DYOBLDTDISTROT].dNeg = YEARSEC;
  output[OUT_DYOBLDTDISTROT].iNum = 1;
  fnWrite[OUT_DYOBLDTDISTROT] = &WriteBodyDYoblDtDistRot;
  
  sprintf(output[OUT_DZOBLDTDISTROT].cName,"DZoblDtDistRot");
  sprintf(output[OUT_DZOBLDTDISTROT].cDescr,"Body's dZobl/dt in DistRot");
  sprintf(output[OUT_DZOBLDTDISTROT].cNeg,"1/yr");
  output[OUT_DZOBLDTDISTROT].bNeg = 1;
  output[OUT_DZOBLDTDISTROT].dNeg = YEARSEC;
  output[OUT_DZOBLDTDISTROT].iNum = 1;
  fnWrite[OUT_DZOBLDTDISTROT] = &WriteBodyDZoblDtDistRot;
  
  sprintf(output[OUT_PRECA].cName,"PrecA");
  sprintf(output[OUT_PRECA].cDescr,"Body's precession parameter in DistRot");
  sprintf(output[OUT_PRECA].cNeg,"Deg");
  output[OUT_PRECA].bNeg = 1;
  output[OUT_PRECA].dNeg = 1./DEGRAD;
  output[OUT_PRECA].iNum = 1;
  fnWrite[OUT_PRECA] = &WriteBodyPrecA;
  
  sprintf(output[OUT_CASS1].cName,"CassiniOne");
  sprintf(output[OUT_CASS1].cDescr,"First Cassini parameter (misalignment of Cassini state vectors)");
  output[OUT_CASS1].bNeg = 0;
  output[OUT_CASS1].iNum = 1;
  fnWrite[OUT_CASS1] = &WriteBodyCassOne;
  
  sprintf(output[OUT_CASS2].cName,"CassiniTwo");
  sprintf(output[OUT_CASS2].cDescr,"Second Cassini parameter (alignment of Cassini state vectors");
  output[OUT_CASS2].bNeg = 0;
  output[OUT_CASS2].iNum = 1;
  fnWrite[OUT_CASS2] = &WriteBodyCassTwo;
}

void FinalizeOutputFunctionDistRot(OUTPUT *output,int iBody,int iModule) {
  
}


/************ DISTROT Logging Functions **************/

void LogOptionsDistRot(CONTROL *control, FILE *fp) {

  fprintf(fp,"-------- DISTROT Options -----\n\n");
  
}

void LogDistRot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  fprintf(fp,"\n----- DISTROT PARAMETERS ------\n");
  for (iOut=OUTSTARTDISTROT;iOut<OUTBODYSTARTDISTROT;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }
}

void LogBodyDistRot(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"-----DISTROT PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTDISTROT;iOut<OUTENDDISTROT;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

/************* MODULE Functions ***********/

void AddModuleDistRot(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = DISTROT;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlDistRot;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyDistRot;
  module->fnCountHalts[iBody][iModule] = &CountHaltsDistRot;
  module->fnLogBody[iBody][iModule] = &LogBodyDistRot;

  module->fnReadOptions[iBody][iModule] = &ReadOptionsDistRot;
  module->fnVerify[iBody][iModule] = &VerifyDistRot;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltDistRot;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyDistRot;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateDistRot;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputDistRot;
  module->fnFinalizeUpdateXobl[iBody][iModule] = &FinalizeUpdateXoblDistRot;
  module->fnFinalizeUpdateYobl[iBody][iModule] = &FinalizeUpdateYoblDistRot;
  module->fnFinalizeUpdateZobl[iBody][iModule] = &FinalizeUpdateZoblDistRot;

  //module->fnInitializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionEqtide;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionDistRot;

}

/************* DISTROT Functions ***********/

void PropertiesDistRot(BODY *body,UPDATE *update,int iBody) {
  // if (body[iBody].bForcePrecRate) {
//     body[iBody].dObliquity = atan2(sqrt(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)),body[iBody].dZobl);
//     body[iBody].dPrecA = atan2(body[iBody].dYobl,body[iBody].dXobl);
//   }
}

void ForceBehaviorDistRot(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,int iBody,int iModule) {
}

void RotateVector(double *v1, double *v2, double theta, int axis) {
  if (axis == 0) {
    v2[0] = v1[0];
    v2[1] = cos(theta)*v1[1] - sin(theta)*v1[2];
    v2[2] = sin(theta)*v1[1] + cos(theta)*v1[2];
  } else if (axis == 1) {
    v2[0] = cos(theta)*v1[0] + sin(theta)*v1[2];
    v2[1] = v1[1];
    v2[2] = -sin(theta)*v1[0] + cos(theta)*v1[2];
  } else if (axis == 2) {
    v2[0] = cos(theta)*v1[0] - sin(theta)*v1[1];
    v2[1] = sin(theta)*v1[0] + cos(theta)*v1[1];
    v2[2] = v1[2];
  }
}

void CalcDynEllip(BODY *body, int iBody) {
  double J2Earth = 1.08262668e-3, J2Venus = 4.56e-6, CEarth = 8.034e37;
  double nuEarth, EdEarth, EdVenus, dTmp;
  
  EdEarth = J2Earth*MEARTH*pow(REARTH,2)/CEarth;
  EdVenus = J2Venus/0.336;
  nuEarth = 2*PI/(DAYSEC);
  
  dTmp = EdEarth*MEARTH/(pow(nuEarth,2)*pow(REARTH,3));
  
  body[iBody].dDynEllip = dTmp*pow(body[iBody].dRotRate,2)*pow(body[iBody].dRadius,3)/\
                            body[iBody].dMass;
  
  if (body[iBody].dDynEllip < EdVenus) body[iBody].dDynEllip = EdVenus;
}

/* Equations used to calculate obliquity/spin evolution */
double fdCentralTorqueSfac(BODY *body, int iBody) {
  return 0.5*pow(1.-pow(body[iBody].dHecc,2)-pow(body[iBody].dKecc,2),-1.5) - S0;
}
  
double fdCentralTorqueR(BODY *body, int iBody) {
  double obliq;
  obliq = atan2(sqrt(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)),body[iBody].dZobl);
  
  return 3*pow(KGAUSS,2)*body[0].dMass/MSUN/(pow(body[iBody].dSemi/AUCM,3)*body[iBody].dRotRate*DAYSEC)*body[iBody].dDynEllip*fdCentralTorqueSfac(body, iBody)*cos(obliq)/DAYSEC;
}

double fdObliquityCRD4(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dQinc*fdDistOrbRD4DpDt(body,system,iaBody) - body[iaBody[0]].dPinc*fdDistOrbRD4DqDt(body,system,iaBody);
}

double fdObliquityARD4(BODY *body, SYSTEM *system, int *iaBody) {
  return 2.0/sqrt(1-pow(body[iaBody[0]].dPinc,2)-pow(body[iaBody[0]].dQinc,2)) * ( fdDistOrbRD4DqDt(body,system,iaBody) + body[iaBody[0]].dPinc*fdObliquityCRD4(body,system,iaBody) );
}

double fdObliquityBRD4(BODY *body, SYSTEM *system, int *iaBody) {
  return 2.0/sqrt(1-pow(body[iaBody[0]].dPinc,2)-pow(body[iaBody[0]].dQinc,2)) * ( fdDistOrbRD4DpDt(body,system,iaBody) - body[iaBody[0]].dQinc*fdObliquityCRD4(body,system,iaBody) );
}

double fdObliquityCLL2(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dQinc*fdDistOrbLL2DpDt(body,system,iaBody) - body[iaBody[0]].dPinc*fdDistOrbLL2DqDt(body,system,iaBody);
}

double fdObliquityALL2(BODY *body, SYSTEM *system, int *iaBody) {
  return 2.0/sqrt(1-pow(body[iaBody[0]].dPinc,2)-pow(body[iaBody[0]].dQinc,2)) * ( fdDistOrbLL2DqDt(body,system,iaBody) + body[iaBody[0]].dPinc*fdObliquityCLL2(body,system,iaBody) );
}

double fdObliquityBLL2(BODY *body, SYSTEM *system, int *iaBody) {
  return 2.0/sqrt(1-pow(body[iaBody[0]].dPinc,2)-pow(body[iaBody[0]].dQinc,2)) * ( fdDistOrbLL2DpDt(body,system,iaBody) - body[iaBody[0]].dQinc*fdObliquityCLL2(body,system,iaBody) );
}

//----------Relativistic correction-------------------------------------
double fdAxialGRCorrection(BODY *body, int *iaBody) {
  return fdApsidalGRCorrection(body, iaBody)/2.;
}

double fdAxialGRDxDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dYobl*fdAxialGRCorrection(body,iaBody);
}

double fdAxialGRDyDt(BODY *body, SYSTEM *system, int *iaBody) {
  return -body[iaBody[0]].dXobl*fdAxialGRCorrection(body,iaBody);
}

//--------------Obliquity/spin evolution--------------------------------------------------------------

double fdDistRotRD4DyDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;
  
  if (iaBody[1] == 0) {
    if (body[iaBody[0]].bForcePrecRate == 0) {
      return body[iaBody[0]].dXobl*fdCentralTorqueR(body,iaBody[0]);
    } else {
      return body[iaBody[0]].dXobl*body[iaBody[0]].dPrecRate;
    }
  } else if (iaBody[1] >= 1) {
//     if (body[iaBody[0]].bForcePrecRate == 0) {
      y = fabs(1.0 - pow(body[iaBody[0]].dXobl,2) - pow(body[iaBody[0]].dYobl,2));
      return -fdObliquityBRD4(body,system,iaBody)*sqrt(y) - body[iaBody[0]].dXobl*2.*fdObliquityCRD4(body,system,iaBody);
    // } else {
//       return cos(body[iaBody[0]].dObliquity)*sin(body[iaBody[0]].dPrecA) * \
//         (-fdObliquityBRD4(body,system,iaBody)*sin(body[iaBody[0]].dPrecA) + \
//         fdObliquityARD4(body,system,iaBody)*cos(body[iaBody[0]].dPrecA));
//     }
  }
  assert(0);
  return 0;
}

double fdDistRotRD4DxDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;
  
  if (iaBody[1] == 0) {
    if (body[iaBody[0]].bForcePrecRate == 0) {
      return -body[iaBody[0]].dYobl*fdCentralTorqueR(body,iaBody[0]);
    } else {
      return -body[iaBody[0]].dYobl*body[iaBody[0]].dPrecRate;
    }
  } else if (iaBody[1] >= 1) {
//     if (body[iaBody[0]].bForcePrecRate == 0) {
      y = fabs(1.0 - pow(body[iaBody[0]].dXobl,2) - pow(body[iaBody[0]].dYobl,2));
      return fdObliquityARD4(body,system,iaBody)*sqrt(y) + body[iaBody[0]].dYobl*2.*fdObliquityCRD4(body,system,iaBody);
    // } else {
//       return cos(body[iaBody[0]].dObliquity)*cos(body[iaBody[0]].dPrecA) * \
//         (-fdObliquityBRD4(body,system,iaBody)*sin(body[iaBody[0]].dPrecA) + \
//         fdObliquityARD4(body,system,iaBody)*cos(body[iaBody[0]].dPrecA));
//     }
  }
  assert(0);
  return 0;
}

double fdDistRotRD4DzDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dYobl*fdObliquityBRD4(body,system,iaBody) - body[iaBody[0]].dXobl*fdObliquityARD4(body,system,iaBody);
}


double fdDistRotLL2DyDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;
  
  if (iaBody[1] == 0) {
    return body[iaBody[0]].dXobl*fdCentralTorqueR(body,iaBody[0]);
  } else if (iaBody[1] >= 1) {
    y = fabs(1.0 - pow(body[iaBody[0]].dXobl,2) - pow(body[iaBody[0]].dYobl,2));
    return -fdObliquityBLL2(body,system,iaBody)*sqrt(y) - body[iaBody[0]].dXobl*2.*fdObliquityCLL2(body,system,iaBody);
  }
  assert(0);
  return 0;
}

double fdDistRotLL2DxDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;
  
  if (iaBody[1] == 0) {
    return -body[iaBody[0]].dYobl*fdCentralTorqueR(body,iaBody[0]);
  } else if (iaBody[1] >= 1) {
    y = fabs(1.0 - pow(body[iaBody[0]].dXobl,2) - pow(body[iaBody[0]].dYobl,2));
    return fdObliquityALL2(body,system,iaBody)*sqrt(y) + body[iaBody[0]].dYobl*2.*fdObliquityCLL2(body,system,iaBody);
  }
  assert(0);
  return 0;
}

double fdDistRotLL2DzDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dYobl*fdObliquityBLL2(body,system,iaBody) - body[iaBody[0]].dXobl*fdObliquityALL2(body,system,iaBody);
}
