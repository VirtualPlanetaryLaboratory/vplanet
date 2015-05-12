/********************** ATMESC.C **********************/
/*
 * Rodrigo Luger, Tue May 12 11:11:38 PDT 2015
 *
 * Subroutines that control the integration of the 
 * atmospheric escape model.
*/
// NOTE: Need to edit module.c, end of options.c and output.c

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void  InitializeControlAtmEsc(CONTROL *control) {
  /* Nothing for now, but this subroutine is necessary for module loops. */
}

void BodyCopyAtmEsc(BODY *dest,BODY *src,int foo,int iBody) {
  dest[iBody].d40KNum = src[iBody].d40KNum;
}

void InitializeBodyAtmEsc(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
}

void InitializeUpdateTmpBodyAtmEsc(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
}

/**************** ATMESC options ********************/

void Read40KMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 40KPower and 40KNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d40KMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d40KMass = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d40KMass = options->dDefault;
}

void Read40KNum(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 40KPower and 40KNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d40KNum = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].d40KNum = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d40KNum = options->dDefault;
}


/* Halts */

void InitializeOptionsAtmEsc(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  sprintf(options[OPT_40KMASS].cName,"d40KMass");
  sprintf(options[OPT_40KMASS].cDescr,"Initial Mass of 40K");
  sprintf(options[OPT_40KMASS].cDefault,"Primordial Earth: xxx");
  options[OPT_40KMASS].iType = 2;
  options[OPT_40KMASS].iMultiFile = 1;
  options[OPT_40KMASS].dNeg = MEARTH;
  sprintf(options[OPT_40KMASS].cNeg,"Earth Masses");
  fnRead[OPT_40KMASS] = &Read40KMass;
  
  sprintf(options[OPT_40KNUM].cName,"d40KNum");
  sprintf(options[OPT_40KNUM].cDescr,"Initial Number of 40K Atoms");
  sprintf(options[OPT_40KNUM].cDefault,"Primordial Earth: xxx");
  options[OPT_40KNUM].dDefault = 0;
  options[OPT_40KNUM].iType = 2;
  options[OPT_40KNUM].iMultiFile = 1;
  options[OPT_40KNUM].dNeg = NEARTH40K;
  sprintf(options[OPT_40KNUM].cNeg,"Primordial Earth 40K Number");
  fnRead[OPT_40KNUM] = &Read40KNum;

}

void ReadOptionsAtmEsc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTATMESC;iOpt<OPTENDATMESC;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify ATMESC ******************/

void VerifyRotationAtmEsc(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody) {
  /* Nothing */
}

void NotMassAndNum(OPTIONS *options,int iMass,int iNum,int iBody) {
    if (options[iMass].iLine[iBody] >= 0 && options[iNum].iLine[iBody] >= 0) 
      DoubleLineExit(options[iMass].cFile[iBody],options[iNum].cFile[iBody],options[iMass].iLine[iBody],options[iNum].iLine[iBody]);
}

void Assign40KNum(BODY *body,OPTIONS *options,double dAge,int iBody) {

  if (options[OPT_40KMASS].iLine[iBody+1] >= 0) {
    printf("40KMass not implemented.\n");
    exit(1);
  }

  if (options[OPT_40KNUM].iLine[iBody+1] >= 0) {
    body[iBody].d40KConst = fd40KConstant(body[iBody].d40KNum,dAge);
  }

  if (options[OPT_40KPOWER].iLine[iBody+1] >= 0) {
    printf("40KPower not implemented.\n");
    exit(1);
  }
}

void Verify40K(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  Assign40KNum(body,options,dAge,iBody);

  update[iBody].iaType[update[iBody].i40K][0] = 1;
  update[iBody].iNumBodies[update[iBody].i40K][0]=1;
  update[iBody].iaBody[update[iBody].i40K][0] = malloc(update[iBody].iNumBodies[update[iBody].i40K][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].i40K][0][0]=iBody;

  update[iBody].pdD40KNumDt = &update[iBody].daDerivProc[update[iBody].i40K][0];
  fnUpdate[iBody][update[iBody].i40K][0] = &fdD40KNumDt;
}

void fnPropertiesAtmEsc(BODY *body,int iBody) {
  /* Nothing */
}

void fnForceBehaviorAtmEsc(BODY *body,EVOLVE *evolve,IO *io,int iBody,int iModule) {
  if (body[iBody].d40KNum < 0.5)
    body[iBody].d40KNum = 0;
}

void VerifyAtmEsc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int bAtmEsc=0;

  /* Cannot set 2 or more of Power, Mass and Number for any isotope */

  /*XXX Need a VerifyOneOfThree subroutine */

  /* AtmEsc is active for this body if this subroutine is called. */

  if (body[iBody].d40KNum > 0) {
    NotMassAndNum(options,OPT_40KMASS,OPT_40KNUM,iBody+1);
    Verify40K(body,options,update,system->dAge,fnUpdate,iBody);
    bAtmEsc = 1;
  }
  
  if (!bAtmEsc && control->Io.iVerbose >= VERBINPUT) 
    fprintf(stderr,"WARNING: ATMESC called for body %s, but no radiogenic species present.\n",body[iBody].cName);

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorAtmEsc;
  control->Evolve.fnAuxProps[iBody][iModule] = &fnPropertiesAtmEsc;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyAtmEsc;

}

void InitializeModuleAtmEsc(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** ATMESC update ****************/

void InitializeUpdateAtmEsc(BODY *body,UPDATE *update,int iBody) {
  /* Initially allow all radiogenic heat sources to be present. If any are 0, 
     or < dMinRadPower, they will me removed from update[iBody] in 
     ForceBehavior.
  */
  if (body[iBody].d40KNum > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum40K++;
  }
}

void FinalizeUpdateEccAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdate40KNumAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD40K;
  update[iBody].iNum40K = (*iEqn)++;
}
      
void FinalizeUpdateOblAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateRotAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateSemiAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}


/***************** ATMESC Halts *****************/

/* Minimum 40K Powering? */

int fbHaltMin40KPower(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (body[iBody].d40KPower < halt->dMin40KPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 40K Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d40KPower,io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMin40KPower,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}           

void CountHaltsAtmEsc(HALT *halt,int *iHalt) {
  if (halt->dMin40KPower >= 0)
    (iHalt)++;
  if (halt->dMin232ThPower >=0)
    (iHalt)++;
  if (halt->dMin238UPower >= 0)
    (iHalt)++;
}

void VerifyHaltAtmEsc(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
  if (control->Halt[iBody].dMin40KPower > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin40KPower;
}

/************* ATMESC Outputs ******************/

void HelpOutputAtmEsc(OUTPUT *output) {
  int iOut;

  printf("\n ------ ATMESC output ------\n");
  for (iOut=OUTSTARTATMESC;iOut<OUTENDATMESC;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

void Write40KMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].d40KNum*MASS40K;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void Write40KNum(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d40KNum;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

void InitializeOutputAtmEsc(OUTPUT *output,fnWriteOutput fnWrite[]) {
  
  sprintf(output[OUT_40KMASS].cName,"40KMass");
  sprintf(output[OUT_40KMASS].cDescr,"Total Mass of 40K");
  sprintf(output[OUT_40KMASS].cNeg,"Earth Masses");
  output[OUT_40KMASS].bNeg = 1;
  output[OUT_40KMASS].dNeg = MEARTH;
  output[OUT_40KMASS].iNum = 1;
  fnWrite[OUT_40KMASS] = &Write40KMass;
  
  sprintf(output[OUT_40KNUM].cName,"40KNumber");
  sprintf(output[OUT_40KNUM].cDescr,"Total Number of 40K Atoms");
  sprintf(output[OUT_40KNUM].cNeg,"Initial Primordial Earth Number");
  output[OUT_40KNUM].bNeg = 1;
  output[OUT_40KNUM].dNeg = MEARTH40K/MASS40K;
  output[OUT_40KNUM].iNum = 1;
  fnWrite[OUT_40KNUM] = &Write40KNum;

}

void FinalizeOutputFunctionAtmEsc(OUTPUT *output,int iBody,int iModule) {
  //output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxAtmEsc;
}

/************ ATMESC Logging Functions **************/

void LogOptionsAtmEsc(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- ATMESC Options -----\n\n");
  */
}

void LogAtmEsc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {

  /* Anything here? 
  int iOut;

  fprintf(fp,"\n----- ATMESC PARAMETERS ------\n");
  for (iOut=OUTSTARTATMESC;iOut<OUTBODYSTARTATMESC;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyAtmEsc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- ATMESC PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTSTARTATMESC;iOut<OUTENDATMESC;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
    /*
    fprintf(fp,"40K Constant: ");
    fprintd(fp,body[iBody].d40KConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");

    fprintf(fp,"232Th Constant: ");
    fprintd(fp,body[iBody].d232ThConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");

    fprintf(fp,"238U Constant: ");
    fprintd(fp,body[iBody].d238UConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");
    */
  }
}

void AddModuleAtmEsc(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = ATMESC;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlAtmEsc;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyAtmEsc;

  module->fnCountHalts[iBody][iModule] = &CountHaltsAtmEsc;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsAtmEsc;
  module->fnLogBody[iBody][iModule] = &LogBodyAtmEsc;
  module->fnVerify[iBody][iModule] = &VerifyAtmEsc;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltAtmEsc;
  module->fnVerifyRotation[iBody][iModule] = &VerifyRotationAtmEsc;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyAtmEsc;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateAtmEsc;

  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionAtmEsc;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionAtmEsc;

}

/************* ATMESC Functions ************/

// N = N_0 * exp(-t/lambda)
// dN/dt = -(N_0/lambda) * exp(-t/lambda)

double fdInternalPowerAtmEsc(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {

  return -(*(update[iBody].pdD238UNumDt))*ENERGY238U - (*(update[iBody].pdD232ThNumDt))*ENERGY232TH - (*(update[iBody].pdD40KNumDt))*ENERGY40K;
}

/* This is part of output[OUT_SURFENFLUX].fnOutput */
double fdSurfEnFluxAtmEsc(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  double dPower;

  dPower = fdInternalPowerAtmEsc(body,system,update,iBody,iFoo);

  return dPower/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
}

double fdAtmEscConst(double dNum,double dAge,double dHalfLife) {
  return dNum/(exp(-dAge/dHalfLife));
}

double fd40KConstant(double dPower,double dAge) {
  return fdAtmEscConst(dPower,dAge,HALFLIFE40K);
}

double fd232ThConstant(double dPower,double dAge) {
  return fdAtmEscConst(dPower,dAge,HALFLIFE232TH);
}

double fd238UConstant(double dPower,double dAge) {
  return fdAtmEscConst(dPower,dAge,HALFLIFE238U);
}

double fdDNumRadDt(double dConst,double dHalfLife,double dAge) {
  return -dConst/dHalfLife*exp(-dAge/dHalfLife);
}

double fdRadPower(double dConst,double dHalfLife,double dAge) {
  return dConst*exp(-dHalfLife/dAge);
}

double fdRadEnFlux(double dConst,double dHalfLife,double dAge,double dRadius) {
  return fdRadPower(dConst,dHalfLife,dAge)/(4*PI*dRadius*dRadius);
}

double fd40KPower(BODY *body,SYSTEM *system,int *iaBody,int iBody) {
  return fdRadPower(body[iBody].d40KConst,HALFLIFE40K,system->dAge);
}

double fd232ThPower(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d232ThConst,HALFLIFE232TH,system->dAge);
}

double fd238UPower(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d238UConst,HALFLIFE238U,system->dAge);
}

double fd40KEnFlux(BODY *body,SYSTEM *system,int *iaBody,int iBody) {
  return fdRadEnFlux(body[iBody].d40KConst,HALFLIFE40K,system->dAge,body[iBody].dRadius);
}

double fd232ThEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d232ThConst,HALFLIFE232TH,system->dAge,body[iBody].dRadius);
}

double fd238UEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d238UConst,HALFLIFE238U,system->dAge,body[iBody].dRadius);
}

double fdD40KNumDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d40KConst,HALFLIFE40K,system->dAge);
}

double fdD232ThNumDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d232ThConst,HALFLIFE232TH,system->dAge);
}

double fdD238UNumDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d238UConst,HALFLIFE238U,system->dAge);
}


/* Number derivatives? */
