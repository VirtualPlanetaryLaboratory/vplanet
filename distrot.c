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

/* Precession parameter */

void ReadPrecA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

    body[iFile-1].dPrecA = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dPrecA = options->dDefault;
}  

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
  
  sprintf(options[OPT_PRECA].cName,"dPrecA");
  sprintf(options[OPT_PRECA].cDescr,"Planet's precession parameter");
  sprintf(options[OPT_PRECA].cDefault,"0");
  options[OPT_PRECA].dDefault = 0.0;
  options[OPT_PRECA].iType = 2;  
  options[OPT_PRECA].iMultiFile = 1; 
//   options[OPT_LONGA].dNeg = DEGRAD;
//   sprintf(options[OPT_LONGA].cNeg,"Degrees");
  fnRead[OPT_PRECA] = &ReadPrecA;
  
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
void CalcXYZobl(BODY *body, int iBody) {
  body[iBody].dXobl = sin(body[iBody].dObliquity)*cos(body[iBody].dPrecA);
  body[iBody].dYobl = sin(body[iBody].dObliquity)*sin(body[iBody].dPrecA);
  body[iBody].dZobl = cos(body[iBody].dObliquity);
}

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
    /* Body updates */
    for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
      /* x = sin(obl)*cos(pA) */
      InitializeXoblDistRot(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] = &fdDistRotDxDt;
      
      /* y = sin(obl)*sin(pA) */
      InitializeYoblDistRot(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] = &fdDistRotDyDt;
      
      /* z = cos(obl) */
      InitializeZoblDistRot(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]] = &fdDistRotDzDt;
        
    }
    /* Body updates for stellar torque, treating star as "perturber" (only needed for x and y -> pA) */
    /* x = sin(obl)*cos(pA) */
    InitializeXoblDistRotStar(body,update,iBody,body[iBody].iGravPerts);
    fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblDistRot[body[iBody].iGravPerts]] = &fdDistRotDxDt;
      
    /* y = sin(obl)*sin(pA) */
    InitializeYoblDistRotStar(body,update,iBody,body[iBody].iGravPerts);
    fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblDistRot[body[iBody].iGravPerts]] = &fdDistRotDyDt;
    
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
    update[iBody].iNumXobl += body[iBody].iGravPerts+1;
    update[iBody].iNumYobl += body[iBody].iGravPerts+1;
    update[iBody].iNumZobl += body[iBody].iGravPerts;
    update[iBody].iNumVars += 3;
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
    for (iPert=0;iPert<=body[iBody].iGravPerts+2;iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTROT;
      update[iBody].iaXoblDistRot[iPert] = (*iEqn)++;
    }
  } else {
    update[iBody].padDXoblDtDistRot = malloc((body[iBody].iGravPerts+1)*sizeof(double*));
    update[iBody].iaXoblDistRot = malloc((body[iBody].iGravPerts+1)*sizeof(int));
    for (iPert=0;iPert<=body[iBody].iGravPerts+1;iPert++) {
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
    for (iPert=0;iPert<=body[iBody].iGravPerts+2;iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTROT;
      update[iBody].iaYoblDistRot[iPert] = (*iEqn)++;
    }
  } else { 
    update[iBody].padDYoblDtDistRot = malloc((body[iBody].iGravPerts+1)*sizeof(double*));
    update[iBody].iaYoblDistRot = malloc((body[iBody].iGravPerts+1)*sizeof(int));
    for (iPert=0;iPert<=body[iBody].iGravPerts+1;iPert++) {
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

void WriteBodyXobl(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dXobl;
  strcpy(cUnit,"");
}  

void WriteBodyYobl(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dYobl;
  strcpy(cUnit,"");
}  

void WriteBodyZobl(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dZobl;
  strcpy(cUnit,"");
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
  
  sprintf(output[OUT_XOBL].cName,"Xobl");
  sprintf(output[OUT_XOBL].cDescr,"Body's sin(obl)*cos(pA)");
  output[OUT_XOBL].iNum = 1;
  fnWrite[OUT_XOBL] = &WriteBodyXobl;
  
  sprintf(output[OUT_YOBL].cName,"Yobl");
  sprintf(output[OUT_YOBL].cDescr,"Body's sin(obl)*sin(pA)");
  output[OUT_YOBL].iNum = 1;
  fnWrite[OUT_YOBL] = &WriteBodyYobl;
  
  sprintf(output[OUT_ZOBL].cName,"Zobl");
  sprintf(output[OUT_ZOBL].cDescr,"Body's cos(obl)");
  output[OUT_ZOBL].iNum = 1;
  fnWrite[OUT_ZOBL] = &WriteBodyZobl;  
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

double fdObliquityC(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dQinc*fdDistOrbDpDt(body,system,iaBody) - body[iaBody[0]].dPinc*fdDistOrbDqDt(body,system,iaBody);
}

double fdObliquityA(BODY *body, SYSTEM *system, int *iaBody) {
  return 2.0/sqrt(1-pow(body[iaBody[0]].dPinc,2)-pow(body[iaBody[0]].dQinc,2)) * ( fdDistOrbDqDt(body,system,iaBody) + body[iaBody[0]].dPinc*fdObliquityC(body,system,iaBody) );
}

double fdObliquityB(BODY *body, SYSTEM *system, int *iaBody) {
  return 2.0/sqrt(1-pow(body[iaBody[0]].dPinc,2)-pow(body[iaBody[0]].dQinc,2)) * ( fdDistOrbDpDt(body,system,iaBody) - body[iaBody[0]].dQinc*fdObliquityC(body,system,iaBody) );
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

//--------------Obliquity/spin evolution---------------------------------------------------------------

double fdDistRotDyDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;
  
  if (iaBody[1] == 0) {
    return body[iaBody[0]].dXobl*fdCentralTorqueR(body,iaBody[0]);
  } else if (iaBody[1] >= 1) {
    y = fabs(1.0 - pow(body[iaBody[0]].dXobl,2) - pow(body[iaBody[0]].dYobl,2));
    return -fdObliquityB(body,system,iaBody)*sqrt(y) - body[iaBody[0]].dXobl*2.*fdObliquityC(body,system,iaBody);
  }
}

double fdDistRotDxDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;
  
  if (iaBody[1] == 0) {
    return -body[iaBody[0]].dYobl*fdCentralTorqueR(body,iaBody[0]);
  } else if (iaBody[1] >= 1) {
    y = fabs(1.0 - pow(body[iaBody[0]].dXobl,2) - pow(body[iaBody[0]].dYobl,2));
    return fdObliquityA(body,system,iaBody)*sqrt(y) + body[iaBody[0]].dYobl*2.*fdObliquityC(body,system,iaBody);
  }
}

double fdDistRotDzDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dYobl*fdObliquityB(body,system,iaBody) - body[iaBody[0]].dXobl*fdObliquityA(body,system,iaBody);
}

