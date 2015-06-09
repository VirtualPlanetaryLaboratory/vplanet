/********************** ATMESC.C **********************/
/*
 * Rodrigo Luger, Tue May 12 11:11:38 PDT 2015
 *
 * Subroutines that control the integration of the 
 * atmospheric escape model.
*/
// NOTE: Still need to edit module.c, update.c

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
  dest[iBody].dNumberOfOrcs = src[iBody].dNumberOfOrcs;
  dest[iBody].dSurfaceWaterMass = src[iBody].dSurfaceWaterMass;
}

void InitializeBodyAtmEsc(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
  //
}

void InitializeUpdateTmpBodyAtmEsc(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
  //
}

/**************** ATMESC options ********************/

void ReadNumberOfOrcs(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dNumberOfOrcs = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dNumberOfOrcs = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dNumberOfOrcs = options->dDefault;
}

void ReadSurfaceWaterMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dSurfaceWaterMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dSurfaceWaterMass = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dSurfaceWaterMass = options->dDefault;
}

void ReadHaltMinSurfaceWaterMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Halt[iFile-1].bSurfaceDesiccated = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultInt(options,&control->Halt[iFile-1].bSurfaceDesiccated,files->iNumInputs); 
  }
}

/* Halts */

void InitializeOptionsAtmEsc(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;
  
  sprintf(options[OPT_NUMBEROFORCS].cName,"dNumberOfOrcs");
  sprintf(options[OPT_NUMBEROFORCS].cDescr,"Initial Number of Orcs");
  sprintf(options[OPT_NUMBEROFORCS].cDefault,"Ten thousand strong at least");
  options[OPT_NUMBEROFORCS].dDefault = HELMSDEEPARMY;
  options[OPT_NUMBEROFORCS].iType = 2;
  options[OPT_NUMBEROFORCS].iMultiFile = 1;
  options[OPT_NUMBEROFORCS].dNeg = HELMSDEEPARMY;
  sprintf(options[OPT_NUMBEROFORCS].cNeg,"Number of Helms Deep Armies");
  fnRead[OPT_NUMBEROFORCS] = &ReadNumberOfOrcs;

  sprintf(options[OPT_SURFACEWATERMASS].cName,"dSurfWaterMass");
  sprintf(options[OPT_SURFACEWATERMASS].cDescr,"Initial Surface Water Mass");
  sprintf(options[OPT_SURFACEWATERMASS].cDefault,"1 TO");
  options[OPT_SURFACEWATERMASS].dDefault = TOMASS;
  options[OPT_SURFACEWATERMASS].iType = 2;
  options[OPT_SURFACEWATERMASS].iMultiFile = 1;
  options[OPT_SURFACEWATERMASS].dNeg = TOMASS;
  sprintf(options[OPT_SURFACEWATERMASS].cNeg,"Terrestrial Oceans (TO)");
  fnRead[OPT_SURFACEWATERMASS] = &ReadSurfaceWaterMass;

  sprintf(options[OPT_MINSURFACEWATERMASS].cName,"bSurfaceDesiccated");
  sprintf(options[OPT_MINSURFACEWATERMASS].cDescr,"Halt at Desiccation?");
  sprintf(options[OPT_MINSURFACEWATERMASS].cDefault,"0");
  options[OPT_MINSURFACEWATERMASS].iType = 0;
  fnRead[OPT_MINSURFACEWATERMASS] = &ReadHaltSurfaceDesiccated;

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

void VerifyOrcs(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iOrcs][0] = 1;
  update[iBody].iNumBodies[update[iBody].iOrcs][0] = 1;
  update[iBody].iaBody[update[iBody].iOrcs][0] = malloc(update[iBody].iNumBodies[update[iBody].iOrcs][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iOrcs][0][0] = iBody;

  update[iBody].pdDNumberOfOrcsDt = &update[iBody].daDerivProc[update[iBody].iOrcs][0];
  fnUpdate[iBody][update[iBody].iOrcs][0] = &fdDNumberOfOrcsDt;
  
}

void VerifySurfaceWaterMass(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iSurfaceWater][0] = 1;
  update[iBody].iNumBodies[update[iBody].iSurfaceWater][0] = 1;
  update[iBody].iaBody[update[iBody].iSurfaceWater][0] = malloc(update[iBody].iNumBodies[update[iBody].iSurfaceWater][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iSurfaceWater][0][0] = iBody;

  update[iBody].pdDSurfaceWaterMassDt = &update[iBody].daDerivProc[update[iBody].iSurfaceWaterMass][0];
  fnUpdate[iBody][update[iBody].iSurfaceWater][0] = &fdDSurfaceWaterMassDt;
}

void fnPropertiesAtmEsc(BODY *body,int iBody) {
  /* Nothing */
}

void fnForceBehaviorAtmEsc(BODY *body,EVOLVE *evolve,IO *io,int iBody,int iModule) {
  if (body[iBody].dNumberOfOrcs < 0)
    // Can't have negative orcs!
    body[iBody].dNumberOfOrcs = 0;
  
  if (body[iBody].dSurfaceWaterMass < 0)
    // Can't have negative water!
    body[iBody].dSurfaceWaterMass = 0;
}

void VerifyAtmEsc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int bAtmEsc=0;

  /* AtmEsc is active for this body if this subroutine is called. */

  if (body[iBody].dNumberOfOrcs > 0) {
    VerifyOrcs(body,options,update,system->dAge,fnUpdate,iBody);
    bAtmEsc = 1;
  }
  
  if (body[iBody].dSurfaceWaterMass > 0) {
    VerifySurfaceWaterMass(body,options,update,system->dAge,fnUpdate,iBody);
    bAtmEsc = 1;
  }
  
  if (!bAtmEsc && control->Io.iVerbose >= VERBINPUT) 
    fprintf(stderr,"WARNING: ATMESC called for body %s, but no atmosphere/water present!\n",body[iBody].cName);

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorAtmEsc;
  control->Evolve.fnAuxProps[iBody][iModule] = &fnPropertiesAtmEsc;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyAtmEsc;

}

void InitializeModuleAtmEsc(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** ATMESC update ****************/

void InitializeUpdateAtmEsc(BODY *body,UPDATE *update,int iBody) {
  if (body[iBody].dNumberOfOrcs > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNumOrcs++;
  }
  
  if (body[iBody].dSurfaceWaterMass > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNumSurfaceWaterMass++;
  }
}

void FinalizeUpdateEccAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateNumberOfOrcsAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumOrcs = (*iEqn)++;
}

void FinalizeUpdateSurfaceWaterMassAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumSurfaceWaterMass = (*iEqn)++;
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

int fbHaltTooManyOrcs(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  // BUG! dMaxNumberOfOrcs is never set.

  if (body[iBody].dNumberOfOrcs > halt->dMaxNumberOfOrcs) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's number of orcs =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].dNumberOfOrcs,io->iSciNot,io->iDigits);
      printf(" > ");
      fprintd(stdout,halt->dMaxNumberOfOrcs,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}           

int fbHaltSurfaceDessicated(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (body[iBody].dSurfaceWaterMass <= 0.) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's surface water mass =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].dSurfaceWaterMass/TOMASS,io->iSciNot,io->iDigits);
      printf("TO.");
    }
    return 1;
  }
  return 0;
} 

void CountHaltsAtmEsc(HALT *halt,int *iHalt) {
  if (halt->dMaxNumberOfOrcs >= 0)
    (iHalt)++;
  if (halt->bSurfaceDesiccated)
    (iHalt)++;
}

void VerifyHaltAtmEsc(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
  if (control->Halt[iBody].dMaxNumberOfOrcs > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltTooManyOrcs;
}

/************* ATMESC Outputs ******************/

void HelpOutputAtmEsc(OUTPUT *output) {
  int iOut;

  printf("\n ------ ATMESC output ------\n");
  for (iOut=OUTSTARTATMESC;iOut<OUTENDATMESC;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

void WriteNumberOfOrcs(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dNumberOfOrcs;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }

}

void InitializeOutputAtmEsc(OUTPUT *output,fnWriteOutput fnWrite[]) {
  
  sprintf(output[OUT_NUMBEROFORCS].cName,"NumberOfOrcs");
  sprintf(output[OUT_NUMBEROFORCS].cDescr,"Total Number of Orcs");
  sprintf(output[OUT_NUMBEROFORCS].cNeg,"Helms Deep Armies");
  output[OUT_NUMBEROFORCS].bNeg = 1;
  output[OUT_NUMBEROFORCS].dNeg = HELMSDEEPARMY;
  output[OUT_NUMBEROFORCS].iNum = 1;
  fnWrite[OUT_NUMBEROFORCS] = &WriteNumberOfOrcs;

}

void FinalizeOutputFunctionAtmEsc(OUTPUT *output,int iBody,int iModule) {
  output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxAtmEsc;
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
  module->fnFinalizeUpdateNumberOfOrcs[iBody][iModule] = &FinalizeUpdateNumberOfOrcsAtmEsc;


  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionAtmEsc;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionAtmEsc;

}

/************* ATMESC Functions ************/
double fdDNumberOfOrcsDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return sin(system->dAge/(1e7*YEARSEC));
}

double fdSurfEnFluxAtmEsc(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  // This is silly, but necessary!
}