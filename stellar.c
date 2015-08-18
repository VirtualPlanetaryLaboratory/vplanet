/********************** STELLAR.C **********************/
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

void  InitializeControlStellar(CONTROL *control) {
  /* Nothing for now, but this subroutine is necessary for module loops. */
}

void BodyCopyStellar(BODY *dest,BODY *src,int foo,int iBody) {
  dest[iBody].dLuminosity = src[iBody].dLuminosity;
  dest[iBody].dSatXUVFrac = src[iBody].dSatXUVFrac;
  dest[iBody].iStellarModel = src[iBody].iStellarModel;
  dest[iBody].dLXUV = src[iBody].dLXUV;
}

void InitializeBodyStellar(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
  //
}

void InitializeUpdateTmpBodyStellar(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
  //
}

/**************** STELLAR options ********************/

void ReadSatXUVFrac(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: %s must be >= 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);	
    }
    body[iFile-1].dSatXUVFrac = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dSatXUVFrac = options->dDefault;
}

void ReadStellarModel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"ba",2)) {
      body[iFile-1].iStellarModel = STELLAR_MODEL_BARAFFE;
    } else if (!memcmp(sLower(cTmp),"no",2)) {
      body[iFile-1].iStellarModel = STELLAR_MODEL_NONE;
    } else {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are BARAFFE or NONE.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);	
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].iStellarModel = STELLAR_MODEL_BARAFFE;
}

void ReadLuminosity(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dLuminosity = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dLuminosity = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dLuminosity = options->dDefault;
}

/* Halts */

// Nothing for now

void InitializeOptionsStellar(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  sprintf(options[OPT_SATXUVFRAC].cName,"dSatXUVFrac");
  sprintf(options[OPT_SATXUVFRAC].cDescr,"Saturated XUV Luminosity Fraction");
  sprintf(options[OPT_SATXUVFRAC].cDefault,"1e-3");
  options[OPT_SATXUVFRAC].dDefault = 1.e-3;
  options[OPT_SATXUVFRAC].iType = 0;
  options[OPT_SATXUVFRAC].iMultiFile = 1;
  fnRead[OPT_SATXUVFRAC] = &ReadSatXUVFrac;

  sprintf(options[OPT_STELLARMODEL].cName,"sStellarModel");
  sprintf(options[OPT_STELLARMODEL].cDescr,"Luminosity Evolution Model");
  sprintf(options[OPT_STELLARMODEL].cDefault,"BARAFFE");
  options[OPT_STELLARMODEL].iType = 3;
  options[OPT_STELLARMODEL].iMultiFile = 1;
  fnRead[OPT_STELLARMODEL] = &ReadStellarModel;

  sprintf(options[OPT_LUMINOSITY].cName,"dLuminosity");
  sprintf(options[OPT_LUMINOSITY].cDescr,"Initial Luminosity");
  sprintf(options[OPT_LUMINOSITY].cDefault,"LSUN");
  options[OPT_LUMINOSITY].dDefault = LSUN;
  options[OPT_LUMINOSITY].iType = 0;
  options[OPT_LUMINOSITY].iMultiFile = 1;
  options[OPT_LUMINOSITY].dNeg = LSUN;
  sprintf(options[OPT_LUMINOSITY].cNeg,"Solar Luminosity (LSUN)");
  fnRead[OPT_LUMINOSITY] = &ReadLuminosity;

}

void ReadOptionsStellar(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTSTELLAR;iOpt<OPTENDSTELLAR;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify STELLAR ******************/

void VerifyRotationStellar(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody) {
  /* Nothing */
}

void VerifyLuminosity(BODY *body, CONTROL *control, OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {


  // Assign luminosity
  if (body[iBody].iStellarModel == STELLAR_MODEL_BARAFFE) {
    body[iBody].dLuminosity = fdLuminosityFunctionBaraffe(body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_LUMINOSITY].iLine[iBody+1] >= 0) {
      // User specified luminosity, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) 
        printf("WARNING: Luminosity set for body %d, but this value will be computed from the grid.\n", iBody);
    }
  }

  update[iBody].iaType[update[iBody].iLuminosity][0] = 0;
  update[iBody].iNumBodies[update[iBody].iLuminosity][0] = 1;
  update[iBody].iaBody[update[iBody].iLuminosity][0] = malloc(update[iBody].iNumBodies[update[iBody].iLuminosity][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iLuminosity][0][0] = iBody;

  update[iBody].pdLuminosityStellar = &update[iBody].daDerivProc[update[iBody].iLuminosity][0];  // NOTE: This points to the VALUE of the luminosity
  fnUpdate[iBody][update[iBody].iLuminosity][0] = &fdLuminosity;                                 // NOTE: Same here!
}

void VerifyRadius(BODY *body, CONTROL *control, OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {


  // Assign radius
  if (body[iBody].iStellarModel == STELLAR_MODEL_BARAFFE) {
    body[iBody].dRadius = fdRadiusFunctionBaraffe(body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_RADIUS].iLine[iBody+1] >= 0) {
      // User specified radius, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) 
        printf("WARNING: Radius set for body %d, but this value will be computed from the grid.\n", iBody);
    }
  }

  update[iBody].iaType[update[iBody].iRadius][0] = 0;
  update[iBody].iNumBodies[update[iBody].iRadius][0] = 1;
  update[iBody].iaBody[update[iBody].iRadius][0] = malloc(update[iBody].iNumBodies[update[iBody].iRadius][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iRadius][0][0] = iBody;

  update[iBody].pdRadiusStellar = &update[iBody].daDerivProc[update[iBody].iRadius][0];  // NOTE: This points to the VALUE of the radius
  fnUpdate[iBody][update[iBody].iRadius][0] = &fdRadius;                                 // NOTE: Same here!
}

void fnPropertiesStellar(BODY *body, UPDATE *update, int iBody) {
  /* Nothing */
}

void fnForceBehaviorStellar(BODY *body,EVOLVE *evolve,IO *io,int iBody,int iModule) {
  // Nothing
}

void VerifyStellar(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int bStellar=0;

  /* Stellar is active for this body if this subroutine is called. */
  
  if (update[iBody].iNumLuminosity > 1) {
    if (control->Io.iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Since iaType is 0 for dLuminosity, cannot have more than one equation affecting it!");
    exit(EXIT_INPUT);
  }
  VerifyLuminosity(body,control,options,update,body[iBody].dAge,fnUpdate,iBody);
  bStellar = 1;
  
  if (update[iBody].iNumRadius > 1) {
    if (control->Io.iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Since iaType is 0 for dRadius, cannot have more than one equation affecting it!");
    exit(EXIT_INPUT);
  }
  VerifyRadius(body,control,options,update,body[iBody].dAge,fnUpdate,iBody);

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorStellar;
  control->Evolve.fnAuxProps[iBody][iModule] = &fnPropertiesStellar;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyStellar;

}

void InitializeModuleStellar(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** STELLAR update ****************/

void InitializeUpdateStellar(BODY *body,UPDATE *update,int iBody) {  
  if (body[iBody].dLuminosity > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNumLuminosity++;
  }
  
  if (body[iBody].dRadius > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNumRadius++;
  }
}

void FinalizeUpdateEccStellar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateLuminosityStellar(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iNumLuminosity = (*iEqn)++;
}
     
void FinalizeUpdateRadiusStellar(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iNumRadius = (*iEqn)++;
}
 
void FinalizeUpdateOblStellar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateRotStellar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateSemiStellar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}


/***************** STELLAR Halts *****************/

void CountHaltsStellar(HALT *halt,int *iHalt) {
  // Nothing
}

void VerifyHaltStellar(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
  // Nothing
}

/************* STELLAR Outputs ******************/

void HelpOutputStellar(OUTPUT *output) {
  int iOut;

  printf("\n ------ STELLAR output ------\n");
  for (iOut=OUTSTARTSTELLAR;iOut<OUTENDSTELLAR;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

void WriteLuminosity(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dLuminosity;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units,cUnit);
  }

}

void WriteLXUV(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dLXUV;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units,cUnit);
  }

}

void InitializeOutputStellar(OUTPUT *output,fnWriteOutput fnWrite[]) {
  
  sprintf(output[OUT_LUMINOSITY].cName,"Luminosity");
  sprintf(output[OUT_LUMINOSITY].cDescr,"Luminosity");
  sprintf(output[OUT_LUMINOSITY].cNeg,"LSUN");
  output[OUT_LUMINOSITY].bNeg = 1;
  output[OUT_LUMINOSITY].dNeg = 1./LSUN;
  output[OUT_LUMINOSITY].iNum = 1;
  fnWrite[OUT_LUMINOSITY] = &WriteLuminosity;
  
  sprintf(output[OUT_LXUV].cName,"LXUV");
  sprintf(output[OUT_LXUV].cDescr,"X-ray/XUV Luminosity");
  sprintf(output[OUT_LXUV].cNeg,"LSUN");
  output[OUT_LXUV].bNeg = 1;
  output[OUT_LXUV].dNeg = 1./LSUN;
  output[OUT_LXUV].iNum = 1;
  fnWrite[OUT_LXUV] = &WriteLXUV;

}

void FinalizeOutputFunctionStellar(OUTPUT *output,int iBody,int iModule) {
  output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxStellar;
}

/************ STELLAR Logging Functions **************/

void LogOptionsStellar(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- STELLAR Options -----\n\n");
  */
}

void LogStellar(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {

  /* Anything here? 
  int iOut;

  fprintf(fp,"\n----- STELLAR PARAMETERS ------\n");
  for (iOut=OUTSTARTSTELLAR;iOut<OUTBODYSTARTSTELLAR;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyStellar(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;  
  fprintf(fp,"----- STELLAR PARAMETERS (%s)------\n",body[iBody].cName);
  
  for (iOut=OUTSTARTSTELLAR;iOut<OUTENDSTELLAR;iOut++) {
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

void AddModuleStellar(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = STELLAR;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlStellar;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyStellar;

  module->fnCountHalts[iBody][iModule] = &CountHaltsStellar;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsStellar;
  module->fnLogBody[iBody][iModule] = &LogBodyStellar;
  module->fnVerify[iBody][iModule] = &VerifyStellar;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltStellar;
  module->fnVerifyRotation[iBody][iModule] = &VerifyRotationStellar;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyStellar;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateStellar;
  module->fnFinalizeUpdateLuminosity[iBody][iModule] = &FinalizeUpdateLuminosityStellar;
  module->fnFinalizeUpdateRadius[iBody][iModule] = &FinalizeUpdateRadiusStellar;

  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionStellar;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionStellar;

}

/************* STELLAR Functions ************/

double fdLuminosity(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE)
    return fdLuminosityFunctionBaraffe(body[iaBody[0]].dAge, body[iaBody[0]].dMass);
  else if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_NONE)
    return body[iaBody[0]].dLuminosity;
}

double fdRadius(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE)
    return fdRadiusFunctionBaraffe(body[iaBody[0]].dAge, body[iaBody[0]].dMass);
  else if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_NONE)
    return body[iaBody[0]].dRadius;
}

double fdLuminosityFunctionBaraffe(double dAge, double dMass) {
  int iError;
  double L = fdBaraffe(STELLAR_L, dAge, dMass, 3, &iError);
  if ((iError == STELLAR_ERR_NONE) || (iError == STELLAR_ERR_LINEAR))
    return L;
  else {
    if (iError == STELLAR_ERR_WAY_OUTOFBOUNDS)
      fprintf(stderr,"ERROR: Interpolation way out of bounds in routine fdBaraffe().\n");
    else if (iError == STELLAR_ERR_OUTOFBOUNDS_LO)
      fprintf(stderr,"ERROR: Out of bounds (low) in fdBaraffe().\n");
    else if (iError == STELLAR_ERR_OUTOFBOUNDS_HI)
      fprintf(stderr,"ERROR: Out of bounds (high) in fdBaraffe().\n");
    else if (iError == STELLAR_ERR_ISNAN)
      fprintf(stderr,"ERROR: Routine fdBaraffe() returned NaN.\n");
    else if (iError == STELLAR_ERR_FILE)
      fprintf(stderr,"ERROR: File access error in routine fdBaraffe().\n");
    else if (iError == STELLAR_ERR_BADORDER)
      fprintf(stderr,"ERROR: Bad interpolation order in routine fdBaraffe().\n");
    else
      fprintf(stderr,"ERROR: Undefined error in fdBaraffe().\n");
    exit(EXIT_INT);
  }
}

double fdRadiusFunctionBaraffe(double dAge, double dMass) {
  int iError;
  double R = fdBaraffe(STELLAR_R, dAge, dMass, 3, &iError);
  if ((iError == STELLAR_ERR_NONE) || (iError == STELLAR_ERR_LINEAR))
    return R;
  else {
    if (iError == STELLAR_ERR_WAY_OUTOFBOUNDS)
      fprintf(stderr,"ERROR: Interpolation way out of bounds in routine fdBaraffe().\n");
    else if (iError == STELLAR_ERR_OUTOFBOUNDS_LO)
      fprintf(stderr,"ERROR: Out of bounds (low) in fdBaraffe().\n");
    else if (iError == STELLAR_ERR_OUTOFBOUNDS_HI)
      fprintf(stderr,"ERROR: Out of bounds (high) in fdBaraffe().\n");
    else if (iError == STELLAR_ERR_ISNAN)
      fprintf(stderr,"ERROR: Routine fdBaraffe() returned NaN.\n");
    else if (iError == STELLAR_ERR_FILE)
      fprintf(stderr,"ERROR: File access error in routine fdBaraffe().\n");
    else if (iError == STELLAR_ERR_BADORDER)
      fprintf(stderr,"ERROR: Bad interpolation order in routine fdBaraffe().\n");
    else
      fprintf(stderr,"ERROR: Undefined error in fdBaraffe().\n");
    exit(EXIT_INT);
  }
}

double fdSurfEnFluxStellar(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  // This is silly, but necessary!
}