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

void InitializeOptionsDistRot(OPTIONS *options,fnReadOption fnRead[]) {
  
  sprintf(options[OPT_DYNELLIP].cName,"dDynEllip");
  sprintf(options[OPT_DYNELLIP].cDescr,"Planet's dynamical ellipticity");
  sprintf(options[OPT_DYNELLIP].cDefault,"0");
  options[OPT_DYNELLIP].dDefault = 0.0;
  options[OPT_DYNELLIP].iType = 2;  
  options[OPT_DYNELLIP].iMultiFile = 1;   
//   options[OPT_LONGP].dNeg = DEGRAD;
//   sprintf(options[OPT_LONGP].cNeg,"Degrees");
  fnRead[OPT_DYNELLIP] = &ReadDynEllip;
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
 
void VerifyDistRot(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int i, j=0, iPert=0, jBody=0;
     
  /* The indexing gets REEAAALLY confusing here. iPert = 0 to iGravPerts-1 correspond to all perturbing planets, iPert = iGravPerts corresponds to the stellar torque, and iPert = iGravPerts+1 to the stellar general relativistic correction, if applied */
  
  if (iBody >= 1) {
    control->Evolve.fnPropsAux[iBody][iModule] = &PropertiesDistRot;
    
    CalcXYZobl(body, iBody);
    
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
      
      if (body[iBody].bGRCorr) {
        InitializeXoblDistRotStar(body,update,iBody,body[iBody].iGravPerts+1);
        fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblDistRot[body[iBody].iGravPerts+1]] = &fdAxialGRDxDt;
        
        InitializeYoblDistRotStar(body,update,iBody,body[iBody].iGravPerts+1);
        fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblDistRot[body[iBody].iGravPerts+1]] = &fdAxialGRDyDt;
      }
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

void FinalizeUpdateXoblDistRot(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
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

void FinalizeUpdateYoblDistRot(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
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

void FinalizeUpdateZoblDistRot(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
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
}

void ForceBehaviorDistRot(BODY *body,EVOLVE *evolve,IO *io,int iBody,int iModule) {
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
    return body[iaBody[0]].dXobl*fdCentralTorqueR(body,iaBody[0]);
  } else if (iaBody[1] >= 1) {
    y = fabs(1.0 - pow(body[iaBody[0]].dXobl,2) - pow(body[iaBody[0]].dYobl,2));
    return -fdObliquityBRD4(body,system,iaBody)*sqrt(y) - body[iaBody[0]].dXobl*2.*fdObliquityCRD4(body,system,iaBody);
  }
  assert(0);
  return 0;
}

double fdDistRotRD4DxDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;
  
  if (iaBody[1] == 0) {
    return -body[iaBody[0]].dYobl*fdCentralTorqueR(body,iaBody[0]);
  } else if (iaBody[1] >= 1) {
    y = fabs(1.0 - pow(body[iaBody[0]].dXobl,2) - pow(body[iaBody[0]].dYobl,2));
    return fdObliquityARD4(body,system,iaBody)*sqrt(y) + body[iaBody[0]].dYobl*2.*fdObliquityCRD4(body,system,iaBody);
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
