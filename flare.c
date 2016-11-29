/********************** FLARE.C **********************/
/*
 * Rory Barnes, Thu Mar 24 15:39:45 PDT 2016
 *
 * Subroutines that control the XUV output of flares.
 *
 */

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void BodyCopyFlare(BODY *dest,BODY *src,int foo,int iNumBodies,int iBody) {
  dest[iBody].dFlareConst = src[iBody].dFlareConst;
  dest[iBody].dFlareExp = src[iBody].dFlareExp;
}

/**************** FLARE options ********************/

void ReadFlareConst(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dFlareConst = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else 
      body[iFile-1].dFlareConst = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dFlareConst = options->dDefault;
}

void ReadFlareExp(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) 
      body[iFile-1].dFlareExp = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else 
      body[iFile-1].dFlareExp = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dFlareConst = options->dDefault;
}

/* Initiatlize Input Options */

void InitializeOptionsFlare(OPTIONS *options,fnReadOption fnRead[]) {
  sprintf(options[OPT_FLARECONST].cName,"dFlareConst");
  sprintf(options[OPT_FLARECONST].cDescr,"Coefficient for flare evolution");
  sprintf(options[OPT_FLARECONST].cDefault,"1e-3 L_Sun");
  options[OPT_FLARECONST].dDefault = 1.e-3*LSUN;
  options[OPT_FLARECONST].iType = 2;
  options[OPT_FLARECONST].iMultiFile = 1;
  options[OPT_FLARECONST].dNeg = LSUN;
  sprintf(options[OPT_FLARECONST].cNeg,"LSUN");
  fnRead[OPT_FLARECONST] = &ReadFlareConst;

  sprintf(options[OPT_FLAREEXP].cName,"dFlareExp");
  sprintf(options[OPT_FLAREEXP].cDescr,"Exponent for flare evolution");
  sprintf(options[OPT_FLAREEXP].cDefault,"1 /Gyr");
  options[OPT_FLAREEXP].dDefault = 1e-9*YEARSEC;
  options[OPT_FLAREEXP].iType = 2;
  options[OPT_FLAREEXP].iMultiFile = 1;
  options[OPT_FLAREEXP].dNeg = 1./(YEARSEC*1e9);
  sprintf(options[OPT_FLAREEXP].cNeg,"/Gyr");
  fnRead[OPT_FLAREEXP] = &ReadFlareExp;

}

void ReadOptionsFlare(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTFLARE;iOpt<OPTENDFLARE;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify FLARE ******************/

void PropertiesFlare(BODY *body,EVOLVE *evolve,UPDATE *update,int iBody) {
  /* Nothing */
}

void fnForceBehaviorFlare(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {

  if (body[iBody].dLXUVFlare < 0)
    body[iBody].dLXUVFlare = 0;

}

void VerifyLXUVFlare(BODY *body,OPTIONS *options,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iLXUV][0] = 1;
  update[iBody].iNumBodies[update[iBody].iLXUV][0]=1;
  update[iBody].iaBody[update[iBody].iLXUV][0] = malloc(update[iBody].iNumBodies[update[iBody].iLXUV][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iLXUV][0][0]=iBody;
  update[iBody].pdDLXUVFlareDt = &update[iBody].daDerivProc[update[iBody].iLXUV][0];
  fnUpdate[iBody][update[iBody].iLXUV][0] = &fdDLXUVFlareDt;
}

void VerifyFlare(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int iFile=iBody+1;

  /* Mass must be in proper range */

  if (body[iBody].dMass < MINMASSFLARE*MSUN || body[iBody].dMass > MAXMASSFLARE*MSUN) {
    fprintf(stderr,"ERROR: Mass of %s must be between %.3lf and %.3lf\n",body[iBody].cName,MINMASSFLARE,MAXMASSFLARE);
    LineExit(files->Infile[iBody+1].cIn,options[OPT_MASS].iLine[iBody+1]);
  }

  /* VerifyMultiFlareStellar checks if STELLAR was selected, too. */

  VerifyLXUVFlare(body,options,update,fnUpdate,iBody);
  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorFlare;
  control->fnPropsAux[iBody][iModule] = &PropertiesFlare;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyFlare;

  /* For now, user may only input FlareConst and FlareExp. Eventually, user
     should also be able to input dLXUVFlare, which will require a call to
     NotTwoOfThree. So we must get initial LXUVFlare now. */

  body[iBody].dLXUVFlare = fdLXUVFlare(body,system,update,iBody,iBody);

}


void InitializeModuleFlare(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** FLARE update ****************/

void InitializeUpdateFlare(BODY *body,UPDATE *update,int iBody) {
  /* STELLAR calculates LXUV from the star's properties, but FLARE calculates LXUV as a primary variable. It is the only possible update. */

  update[iBody].iNumVars++;
  update[iBody].iNumLXUV++;
}

void FinalizeUpdateLXUVFlare(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = FLARE;
  update[iBody].iNumLXUV = (*iEqn)++;
}

/***************** FLARE Halts *****************/

/* Halt for massive flare? No Flares? */

void CountHaltsFlare(HALT *halt,int *iNumHalts) {
}

void VerifyHaltFlare(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
}

/************* FLARE Outputs ******************/

/* NOTE: If you write a new Write subroutine here you need to add the associate 
   block of initialization in InitializeOutputFlare below */

void HelpOutputFlare(OUTPUT *output) {
  int iOut;

  printf("\n ------ STELLAR output ------\n");
  for (iOut=OUTSTARTFLARE;iOut<OUTENDFLARE;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

void WriteLXUVFlare(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dLXUVFlare;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { 
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void InitializeOutputFlare(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_LXUVFLARE].cName,"LXUVFlare");
  sprintf(output[OUT_LXUVFLARE].cDescr,"XUV Luminosity from flares");
  sprintf(output[OUT_LXUVFLARE].cNeg,"LSUN");
  output[OUT_LXUVFLARE].bNeg = 1;
  output[OUT_LXUVFLARE].dNeg = 1./LSUN; 
  output[OUT_LXUVFLARE].iNum = 1;
  output[OUT_LXUVFLARE].iModuleBit = FLARE;
  fnWrite[OUT_LXUVFLARE] = &WriteLXUVFlare;
}

void FinalizeOutputFunctionFlare(OUTPUT *output,int iBody,int iModule) {
  output[OUT_LXUVTOT].fnOutput[iBody][iModule] = &fdLXUVFlare;
}

/************ FLARE Logging Functions **************/

void LogOptionsFlare(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- FLARE Options -----\n\n");
  */
}

void LogFlare(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {

 /* Anything here? 
  int iOut;

  fprintf(fp,"\n----- FLARE PARAMETERS ------\n");
  for (iOut=OUTSTARTRADHEAT;iOut<OUTBODYSTARTRADHEAT;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyFlare(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- FLARE PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTSTARTFLARE;iOut<OUTENDFLARE;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

void AddModuleFlare(MODULE *module,int iBody,int iModule) {

  module->fnCountHalts[iBody][iModule] = &CountHaltsFlare;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsFlare;
  module->fnLogBody[iBody][iModule] = &LogBodyFlare;
  module->fnVerify[iBody][iModule] = &VerifyFlare;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltFlare;

  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateFlare;

  module->fnFinalizeUpdateLXUV[iBody][iModule] = &FinalizeUpdateLXUVFlare;

  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionFlare;
}

/************* FLARE Functions ************/

double fdLXUVFlare(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  return  body[iBody].dFlareConst*exp(-body[iBody].dFlareExp*body[iBody].dAge);
}

double fdDLXUVFlareDt(BODY *body,SYSTEM *system,int *iaBody) {
  /* Placeholder for now. L_XUV = const*exp(-const*time) */

  return -body[iaBody[0]].dFlareConst*body[iaBody[0]].dFlareExp*exp(-body[iaBody[0]].dFlareExp*body[iaBody[0]].dAge);
}
