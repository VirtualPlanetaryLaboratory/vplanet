/********************** LASKAR.C **********************/
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
#include "lagrange.h"
#include "laskar.h"

void InitializeControlLaskar(CONTROL *control) {
  /* Not sure if I need anything here yet */
}

void InitializeModuleLaskar(CONTROL *control,MODULE *module) {
  /* Anything here? */
}

void BodyCopyLaskar(BODY *dest,BODY *src,int iTideModel,int iBody) {
  int iIndex,iPert;

  dest[iBody].dXobl = src[iBody].dXobl;
  dest[iBody].dYobl = src[iBody].dYobl;
  dest[iBody].dZobl = src[iBody].dZobl;
  dest[iBody].dDynEllip = src[iBody].dDynEllip;

}

void InitializeBodyLaskar(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
}

void InitializeUpdateTmpBodyLaskar(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
}

/**************** LASKAR options ********************/

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

void InitializeOptionsLaskar(OPTIONS *options,fnReadOption fnRead[]) {
  
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

void ReadOptionsLaskar(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTLASKAR;iOpt<OPTENDLASKAR;iOpt++) { 
      if (options[iOpt].iType != -1) {
	fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
      }
  }
}
    

/******************* Verify LASKAR ******************/
void CalcXYZobl(BODY *body, int iBody) {
  body[iBody].dXobl = sin(body[iBody].dObliquity)*cos(body[iBody].dPrecA);
  body[iBody].dYobl = sin(body[iBody].dObliquity)*sin(body[iBody].dPrecA);
  body[iBody].dZobl = cos(body[iBody].dObliquity);
}

/* In the following, iBody is the current body number that is getting assigned,
   iPert counts the number of bodies perturbing iBody, and iBodyPert is the
   body number of the current perturbing body. */
void InitializeXoblLaskar(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]] = 2;
  update[iBody].padDXoblDtLaskar[iPert] = &update[iBody].daDerivProc[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]];
  update[iBody].iNumBodies[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]]=2;
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeYoblLaskar(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]] = 2;
  update[iBody].padDYoblDtLaskar[iPert] = &update[iBody].daDerivProc[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]];
  update[iBody].iNumBodies[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]]=2;
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeZoblLaskar(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iZobl][update[iBody].iaZoblLaskar[iPert]] = 2;
  update[iBody].padDZoblDtLaskar[iPert] = &update[iBody].daDerivProc[update[iBody].iZobl][update[iBody].iaZoblLaskar[iPert]];
  update[iBody].iNumBodies[update[iBody].iZobl][update[iBody].iaZoblLaskar[iPert]]=2;
  update[iBody].iaBody[update[iBody].iZobl][update[iBody].iaZoblLaskar[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iZobl][update[iBody].iaZoblLaskar[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iZobl][update[iBody].iaZoblLaskar[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iZobl][update[iBody].iaZoblLaskar[iPert]][1] = body[iBody].iaGravPerts[iPert];
}

void InitializeXoblLaskarStar(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]] = 2;
  update[iBody].padDXoblDtLaskar[iPert] = &update[iBody].daDerivProc[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]];
  update[iBody].iNumBodies[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]]=2;
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]][1] = 0;
}

void InitializeYoblLaskarStar(BODY *body,UPDATE *update,int iBody,int iPert) {
  update[iBody].iaType[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]] = 2;
  update[iBody].padDYoblDtLaskar[iPert] = &update[iBody].daDerivProc[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]];
  update[iBody].iNumBodies[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]]=2;
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]] = malloc(update[iBody].iNumBodies[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]][0] = iBody;
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]][1] = 0;
}
 
void VerifyLaskar(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int i, j=0, iPert=0, jBody=0;
     
  if (iBody >= 1) {
    control->Evolve.fnAuxProps[iBody][iModule] = &PropertiesLaskar;
    
    CalcXYZobl(body, iBody);
    /* Body updates */
    for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
      /* x = sin(obl)*cos(pA) */
      InitializeXoblLaskar(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblLaskar[iPert]] = &fdLaskarDxDt;
      
      /* y = sin(obl)*sin(pA) */
      InitializeYoblLaskar(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblLaskar[iPert]] = &fdLaskarDyDt;
      
      /* z = cos(obl) */
      InitializeZoblLaskar(body,update,iBody,iPert);
      fnUpdate[iBody][update[iBody].iZobl][update[iBody].iaZoblLaskar[iPert]] = &fdLaskarDzDt;
        
    }
    /* Body updates treating star as perturber (only needed for x and y -> pA) */
    /* x = sin(obl)*cos(pA) */
    InitializeXoblLaskarStar(body,update,iBody,body[iBody].iGravPerts);
    fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblLaskar[body[iBody].iGravPerts]] = &fdLaskarDxDt;
      
    /* y = sin(obl)*sin(pA) */
    InitializeYoblLaskarStar(body,update,iBody,body[iBody].iGravPerts);
    fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblLaskar[body[iBody].iGravPerts]] = &fdLaskarDyDt;
  }
  
  control->fnForceBehavior[iBody][iModule]=&ForceBehaviorLaskar;
  control->Evolve.fnBodyCopy[iBody][iModule]=&BodyCopyLaskar;
}


/***************** LASKAR Update *****************/
void InitializeUpdateLaskar(BODY *body,UPDATE *update,int iBody) {
  if (iBody > 0) {
    update[iBody].iNumXobl += body[iBody].iGravPerts+1;
    update[iBody].iNumYobl += body[iBody].iGravPerts+1;
    update[iBody].iNumZobl += body[iBody].iGravPerts;
    update[iBody].iNumVars += 3;
  }
}

void FinalizeUpdateXoblLaskar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  int iPert;
  
  update[iBody].padDXoblDtLaskar = malloc((body[iBody].iGravPerts+1)*sizeof(double*));
  update[iBody].iaXoblLaskar = malloc((body[iBody].iGravPerts+1)*sizeof(int));
  for (iPert=0;iPert<=body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = LASKAR;
    update[iBody].iaXoblLaskar[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateYoblLaskar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  int iPert;
  
  update[iBody].padDYoblDtLaskar = malloc((body[iBody].iGravPerts+1)*sizeof(double*));
  update[iBody].iaYoblLaskar = malloc((body[iBody].iGravPerts+1)*sizeof(int));
  for (iPert=0;iPert<=body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = LASKAR;
    update[iBody].iaYoblLaskar[iPert] = (*iEqn)++;
  }
}

void FinalizeUpdateZoblLaskar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  int iPert;
  
  update[iBody].padDZoblDtLaskar = malloc((body[iBody].iGravPerts)*sizeof(double*));
  update[iBody].iaZoblLaskar = malloc((body[iBody].iGravPerts)*sizeof(int));
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = LASKAR;
    update[iBody].iaZoblLaskar[iPert] = (*iEqn)++;
  }
}


/***************** LASKAR Halts *****************/

void CountHaltsLaskar(HALT *halt,int *iNumHalts) { 
}

void VerifyHaltLaskar(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
}


/************* LASKAR Outputs ******************/

void WriteBodyDOblDtLaskar(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
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
     dDeriv += dObldx*(*(update[iBody].padDXoblDtLaskar[iPert])) + dObldy*(*(update[iBody].padDYoblDtLaskar[iPert]))\
	+ dObldz*(*(update[iBody].padDZoblDtLaskar[iPert]));
 
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

void WriteBodyDPrecADtLaskar(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv, dpAdx, dpAdy;
  int iPert;

  dpAdx = - body[iBody].dYobl/(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2));
  dpAdy = body[iBody].dXobl/(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2));
  
  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<=body[iBody].iGravPerts;iPert++) 
    dDeriv += dpAdx*(*(update[iBody].padDXoblDtLaskar[iPert])) + dpAdy*(*(update[iBody].padDYoblDtLaskar[iPert]));

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
  
void WriteBodyDXoblDtLaskar(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<=body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDXoblDtLaskar[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}  

void WriteBodyDYoblDtLaskar(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<=body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDYoblDtLaskar[iPert]);
  
  *dTmp = dDeriv;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
} 

void WriteBodyDZoblDtLaskar(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv=0;
  for (iPert=0;iPert<body[iBody].iGravPerts;iPert++) 
    dDeriv += *(update[iBody].padDZoblDtLaskar[iPert]);
  
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
  
void InitializeOutputLaskar(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_DOBLDTLASKAR].cName,"DOblDtLaskar");
  sprintf(output[OUT_DOBLDTLASKAR].cDescr,"Body's dobl/dt in Laskar");
  sprintf(output[OUT_DOBLDTLASKAR].cNeg,"deg/year");
  output[OUT_DOBLDTLASKAR].bNeg = 1;
  output[OUT_DOBLDTLASKAR].dNeg = YEARSEC/DEGRAD;
  output[OUT_DOBLDTLASKAR].iNum = 1;
  fnWrite[OUT_DOBLDTLASKAR] = &WriteBodyDOblDtLaskar;
  
  sprintf(output[OUT_DPRECADTLASKAR].cName,"DPrecADtLaskar");
  sprintf(output[OUT_DPRECADTLASKAR].cDescr,"Body's dpA/dt in Laskar");
  sprintf(output[OUT_DPRECADTLASKAR].cNeg,"deg/year");
  output[OUT_DPRECADTLASKAR].bNeg = 1;
  output[OUT_DPRECADTLASKAR].dNeg = YEARSEC/DEGRAD;
  output[OUT_DPRECADTLASKAR].iNum = 1;
  fnWrite[OUT_DPRECADTLASKAR] = &WriteBodyDPrecADtLaskar;
 
  sprintf(output[OUT_DXOBLDTLASKAR].cName,"DXoblDtLaskar");
  sprintf(output[OUT_DXOBLDTLASKAR].cDescr,"Body's dXobl/dt in Laskar");
  sprintf(output[OUT_DXOBLDTLASKAR].cNeg,"1/year");
  output[OUT_DXOBLDTLASKAR].bNeg = 1;
  output[OUT_DXOBLDTLASKAR].dNeg = YEARSEC;
  output[OUT_DXOBLDTLASKAR].iNum = 1;
  fnWrite[OUT_DXOBLDTLASKAR] = &WriteBodyDXoblDtLaskar;
  
  sprintf(output[OUT_DYOBLDTLASKAR].cName,"DYoblDtLaskar");
  sprintf(output[OUT_DYOBLDTLASKAR].cDescr,"Body's dYobl/dt in Laskar");
  sprintf(output[OUT_DYOBLDTLASKAR].cNeg,"1/yr");
  output[OUT_DYOBLDTLASKAR].bNeg = 1;
  output[OUT_DYOBLDTLASKAR].dNeg = YEARSEC;
  output[OUT_DYOBLDTLASKAR].iNum = 1;
  fnWrite[OUT_DYOBLDTLASKAR] = &WriteBodyDYoblDtLaskar;
  
  sprintf(output[OUT_DZOBLDTLASKAR].cName,"DZoblDtLaskar");
  sprintf(output[OUT_DZOBLDTLASKAR].cDescr,"Body's dZobl/dt in Laskar");
  sprintf(output[OUT_DZOBLDTLASKAR].cNeg,"1/yr");
  output[OUT_DZOBLDTLASKAR].bNeg = 1;
  output[OUT_DZOBLDTLASKAR].dNeg = YEARSEC;
  output[OUT_DZOBLDTLASKAR].iNum = 1;
  fnWrite[OUT_DZOBLDTLASKAR] = &WriteBodyDZoblDtLaskar;
  
  sprintf(output[OUT_PRECA].cName,"PrecA");
  sprintf(output[OUT_PRECA].cDescr,"Body's precession parameter in Laskar");
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

void FinalizeOutputFunctionLaskar(OUTPUT *output,int iBody,int iModule) {
  
}


/************ LASKAR Logging Functions **************/

void LogOptionsLaskar(CONTROL *control, FILE *fp) {

  fprintf(fp,"-------- LASKAR Options -----\n\n");
  
}

void LogLaskar(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  fprintf(fp,"\n----- LASKAR PARAMETERS ------\n");
  for (iOut=OUTSTARTLASKAR;iOut<OUTBODYSTARTLASKAR;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }
}

void LogBodyLaskar(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- LASKAR PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTLASKAR;iOut<OUTENDLASKAR;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

/************* MODULE Functions ***********/

void AddModuleLaskar(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = LASKAR;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlLaskar;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyLaskar;
  module->fnCountHalts[iBody][iModule] = &CountHaltsLaskar;
  module->fnLogBody[iBody][iModule] = &LogBodyLaskar;

  module->fnReadOptions[iBody][iModule] = &ReadOptionsLaskar;
  module->fnVerify[iBody][iModule] = &VerifyLaskar;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltLaskar;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyLaskar;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateLaskar;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputLaskar;
  module->fnFinalizeUpdateXobl[iBody][iModule] = &FinalizeUpdateXoblLaskar;
  module->fnFinalizeUpdateYobl[iBody][iModule] = &FinalizeUpdateYoblLaskar;
  module->fnFinalizeUpdateZobl[iBody][iModule] = &FinalizeUpdateZoblLaskar;

  //module->fnInitializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionEqtide;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionLaskar;

}

/************* LASKAR Functions ***********/

void PropertiesLaskar(BODY *body,UPDATE *update,int iBody) {  
}

void ForceBehaviorLaskar(BODY *body,EVOLVE *evolve,IO *io,int iBody,int iModule) {
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
  return body[iaBody[0]].dQinc*fdLagrangeDpDt(body,system,iaBody) - body[iaBody[0]].dPinc*fdLagrangeDqDt(body,system,iaBody);
}

double fdObliquityA(BODY *body, SYSTEM *system, int *iaBody) {
  return 2.0/sqrt(1-pow(body[iaBody[0]].dPinc,2)-pow(body[iaBody[0]].dQinc,2)) * ( fdLagrangeDqDt(body,system,iaBody) + body[iaBody[0]].dPinc*fdObliquityC(body,system,iaBody) );
}

double fdObliquityB(BODY *body, SYSTEM *system, int *iaBody) {
  return 2.0/sqrt(1-pow(body[iaBody[0]].dPinc,2)-pow(body[iaBody[0]].dQinc,2)) * ( fdLagrangeDpDt(body,system,iaBody) - body[iaBody[0]].dQinc*fdObliquityC(body,system,iaBody) );
}

//--------------Obliquity/spin evolution---------------------------------------------------------------

double fdLaskarDyDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;
  
  if (iaBody[1] == 0) {
    return body[iaBody[0]].dXobl*fdCentralTorqueR(body,iaBody[0]);
  } else if (iaBody[1] >= 1) {
    y = fabs(1.0 - pow(body[iaBody[0]].dXobl,2) - pow(body[iaBody[0]].dYobl,2));
    return -fdObliquityB(body,system,iaBody)*sqrt(y) - body[iaBody[0]].dXobl*2.*fdObliquityC(body,system,iaBody);
  }
}

double fdLaskarDxDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;
  
  if (iaBody[1] == 0) {
    return -body[iaBody[0]].dYobl*fdCentralTorqueR(body,iaBody[0]);
  } else if (iaBody[1] >= 1) {
    y = fabs(1.0 - pow(body[iaBody[0]].dXobl,2) - pow(body[iaBody[0]].dYobl,2));
    return fdObliquityA(body,system,iaBody)*sqrt(y) + body[iaBody[0]].dYobl*2.*fdObliquityC(body,system,iaBody);
  }
}

double fdLaskarDzDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dYobl*fdObliquityB(body,system,iaBody) - body[iaBody[0]].dXobl*fdObliquityA(body,system,iaBody);
}

