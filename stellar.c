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
  dest[iBody].dTemperature = src[iBody].dTemperature;
  dest[iBody].dSatXUVFrac = src[iBody].dSatXUVFrac;
  dest[iBody].iStellarModel = src[iBody].iStellarModel;
  dest[iBody].iWindModel = src[iBody].iWindModel;
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

void ReadWindModel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"re",2)) {
      body[iFile-1].iWindModel = STELLAR_MODEL_REINERS;
    } else if (!memcmp(sLower(cTmp),"no",2)) {
      body[iFile-1].iWindModel = STELLAR_MODEL_NONE;
    } else {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are REINERS or NONE.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);	
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].iWindModel = STELLAR_MODEL_REINERS;
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

void ReadTemperature(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dTemperature = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dTemperature = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dTemperature = options->dDefault;
}

/* Halts */

void ReadHaltEndBaraffeGrid(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Halt[iFile-1].bEndBaraffeGrid = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultInt(options,&control->Halt[iFile-1].bEndBaraffeGrid,files->iNumInputs); 
  }
}

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

  sprintf(options[OPT_WINDMODEL].cName,"sWindModel");
  sprintf(options[OPT_WINDMODEL].cDescr,"Wind Angular Momentum Loss Model");
  sprintf(options[OPT_WINDMODEL].cDefault,"REINERS");
  options[OPT_WINDMODEL].iType = 3;
  options[OPT_WINDMODEL].iMultiFile = 1;
  fnRead[OPT_WINDMODEL] = &ReadWindModel;

  sprintf(options[OPT_LUMINOSITY].cName,"dLuminosity");
  sprintf(options[OPT_LUMINOSITY].cDescr,"Initial Luminosity");
  sprintf(options[OPT_LUMINOSITY].cDefault,"LSUN");
  options[OPT_LUMINOSITY].dDefault = LSUN;
  options[OPT_LUMINOSITY].iType = 0;
  options[OPT_LUMINOSITY].iMultiFile = 1;
  options[OPT_LUMINOSITY].dNeg = LSUN;
  sprintf(options[OPT_LUMINOSITY].cNeg,"Solar Luminosity (LSUN)");
  fnRead[OPT_LUMINOSITY] = &ReadLuminosity;

  sprintf(options[OPT_TEMPERATURE].cName,"dTemperature");
  sprintf(options[OPT_TEMPERATURE].cDescr,"Initial Effective Temperature");
  sprintf(options[OPT_TEMPERATURE].cDefault,"TSUN");
  options[OPT_TEMPERATURE].dDefault = TSUN;
  options[OPT_TEMPERATURE].iType = 0;
  options[OPT_TEMPERATURE].iMultiFile = 1;
  fnRead[OPT_TEMPERATURE] = &ReadTemperature;

  sprintf(options[OPT_HALTENDBARAFFEFGRID].cName,"bHaltEndBaraffeGrid");
  sprintf(options[OPT_HALTENDBARAFFEFGRID].cDescr,"Halt when we reach the end of the Baraffe+15 grid?");
  sprintf(options[OPT_HALTENDBARAFFEFGRID].cDefault,"1");
  options[OPT_HALTENDBARAFFEFGRID].iType = 0;
  fnRead[OPT_HALTENDBARAFFEFGRID] = &ReadHaltEndBaraffeGrid;

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
  
  // DEPRECATED! Let's get rid of this...
  
}

void VerifyRotRate(BODY *body, CONTROL *control, OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
  update[iBody].iaType[update[iBody].iRot][update[iBody].iRotStellar] = 1;
  update[iBody].iNumBodies[update[iBody].iRot][update[iBody].iRotStellar] = 1;
  update[iBody].iaBody[update[iBody].iRot][update[iBody].iRotStellar] = malloc(update[iBody].iNumBodies[update[iBody].iRot][update[iBody].iRotStellar]*sizeof(int));
  update[iBody].iaBody[update[iBody].iRot][update[iBody].iRotStellar][0] = iBody;

  update[iBody].pdRotRateStellar = &update[iBody].daDerivProc[update[iBody].iRot][update[iBody].iRotStellar];
  fnUpdate[iBody][update[iBody].iRot][update[iBody].iRotStellar] = &fdDRotRateDt; 
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

void VerifyTemperature(BODY *body, CONTROL *control, OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {


  // Assign temperature
  if (body[iBody].iStellarModel == STELLAR_MODEL_BARAFFE) {
    body[iBody].dTemperature = fdTemperatureFunctionBaraffe(body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_TEMPERATURE].iLine[iBody+1] >= 0) {
      // User specified temperature, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) 
        printf("WARNING: Temperature set for body %d, but this value will be computed from the grid.\n", iBody);
    }
  }

  update[iBody].iaType[update[iBody].iTemperature][0] = 0;
  update[iBody].iNumBodies[update[iBody].iTemperature][0] = 1;
  update[iBody].iaBody[update[iBody].iTemperature][0] = malloc(update[iBody].iNumBodies[update[iBody].iTemperature][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iTemperature][0][0] = iBody;

  update[iBody].pdTemperatureStellar = &update[iBody].daDerivProc[update[iBody].iTemperature][0];  // NOTE: This points to the VALUE of the temperature
  fnUpdate[iBody][update[iBody].iTemperature][0] = &fdTemperature;                                 // NOTE: Same here!
}

void fnPropertiesStellar(BODY *body, UPDATE *update, int iBody) {

  // TODO TODO TODO: Fix kink! Check saturation criteria.
  
  // Update LXUV
  double dPer, dLXRay, dLEUV;
  dPer = 2 * PI / body[iBody].dRotRate;
  // Unsaturated regime (Reiners, Schussler & Passegger 2014, eqn. (11))
  dLXRay = 1.e-7 * pow(10., 30.71 - 2.01 * log10(dPer / DAYSEC));

  if (log10(dLXRay / body[iBody].dLuminosity) > log10(body[iBody].dSatXUVFrac)){
    // Saturated regime (Reiners, Schussler & Passegger 2014)
    dLXRay = body[iBody].dLuminosity * pow(10., -3.12 - 0.11 * log10(dPer / DAYSEC));
  }
  // Sanz-Forcada et al. (2011), eqn (3)
  dLEUV = 1.e7 * pow(10., 4.80 + 0.860 * log10(dLXRay * 1.e-7));
  // Based on Miles Timpe's thesis:
  body[iBody].dLXUV = dLEUV + dLXRay;

}

void fnForceBehaviorStellar(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,int iBody,int iModule) {
  // Nothing
}

void VerifyStellar(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int bStellar=0;

  /* Stellar is active for this body if this subroutine is called. */
  
  if (update[iBody].iNumLuminosity > 1) {
    if (control->Io.iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Looks like there's more than one equation trying to set dLuminosity for body %d!", iBody);
    exit(EXIT_INPUT);
  }
  VerifyLuminosity(body,control,options,update,body[iBody].dAge,fnUpdate,iBody);
  bStellar = 1;
  
  if (update[iBody].iNumRadius > 1) {
    if (control->Io.iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Looks like there's more than one equation trying to set dRadius for body %d!", iBody);
    exit(EXIT_INPUT);
  }
  VerifyRadius(body,control,options,update,body[iBody].dAge,fnUpdate,iBody);
  
  VerifyRotRate(body,control,options,update,body[iBody].dAge,fnUpdate,iBody);
  
  if (update[iBody].iNumTemperature > 1) {
    if (control->Io.iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Looks like there's more than one equation trying to set dTemperature for body %d!", iBody);
    exit(EXIT_INPUT);
  }
  VerifyTemperature(body,control,options,update,body[iBody].dAge,fnUpdate,iBody);

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorStellar;
  control->Evolve.fnPropsAux[iBody][iModule] = &fnPropertiesStellar;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyStellar;

}

void InitializeModuleStellar(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** STELLAR update ****************/

void InitializeUpdateStellar(BODY *body,UPDATE *update,int iBody) {  
  if (body[iBody].dLuminosity > 0) {
    if (update[iBody].iNumLuminosity == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumLuminosity++;
  }
  
  if (body[iBody].dRadius > 0) {
    if (update[iBody].iNumRadius == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumRadius++;
  }

  // NOTE: Rory and I decided to ALWAYS track the rotation evolution of the star,
  // so I'm not going to check whether dRotRate is zero here. If it is, it gets set
  // to its default value, and we track angular momentum conservation from there.
  if (update[iBody].iNumRot == 0)
    update[iBody].iNumVars++;
  update[iBody].iNumRot++;
  
  if (body[iBody].dTemperature > 0) {
    if (update[iBody].iNumTemperature == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumTemperature++;
  }
}

void FinalizeUpdateEccStellar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  /* Nothing */
}

void FinalizeUpdateLuminosityStellar(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iNumLuminosity = (*iEqn)++;
}
     
void FinalizeUpdateRadiusStellar(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iNumRadius = (*iEqn)++;
}

void FinalizeUpdateRotRateStellar(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iRotStellar = (*iEqn)++;
}

void FinalizeUpdateTemperatureStellar(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iNumTemperature = (*iEqn)++;
}
 
void FinalizeUpdateOblStellar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  /* Nothing */
}

void FinalizeUpdateSemiStellar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  /* Nothing */
}


/***************** STELLAR Halts *****************/

int fbHaltEndBaraffeGrid(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  if (body[iBody].iStellarModel == STELLAR_MODEL_CONST) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s reached the edge of the luminosity grid at ", body[iBody].cName);
      fprintd(stdout, body[iBody].dAge/YEARSEC,io->iSciNot,io->iDigits);
      printf(" years.\n");
    }
    return 1;
  }
  return 0;
}

void CountHaltsStellar(HALT *halt,int *iHalt) {
  if (halt->bEndBaraffeGrid)
    (*iHalt)++;
}

void VerifyHaltStellar(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
  if (control->Halt[iBody].bEndBaraffeGrid)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltEndBaraffeGrid;
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

void WriteTemperature(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dTemperature;
  // Kelvin only
  fsUnitsTemp(0,cUnit);
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

  sprintf(output[OUT_TEMPERATURE].cName,"Temperature");
  sprintf(output[OUT_TEMPERATURE].cDescr,"Effective Temperature");
  output[OUT_TEMPERATURE].bNeg = 0;
  output[OUT_TEMPERATURE].iNum = 1;
  fnWrite[OUT_TEMPERATURE] = &WriteTemperature;
  
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
  module->fnFinalizeUpdateRot[iBody][iModule] = &FinalizeUpdateRotRateStellar;
  module->fnFinalizeUpdateTemperature[iBody][iModule] = &FinalizeUpdateTemperatureStellar;

  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionStellar;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionStellar;

}

/************* STELLAR Functions ************/

double fdLuminosity(BODY *body,SYSTEM *system,int *iaBody) {
  double foo;
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    foo = fdLuminosityFunctionBaraffe(body[iaBody[0]].dAge, body[iaBody[0]].dMass);
    if (!isnan(foo)) return foo;
    else body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
  }
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_NONE || body[iaBody[0]].iStellarModel == STELLAR_MODEL_CONST)
    return body[iaBody[0]].dLuminosity;
  else
    return 0;
}

double fdRadius(BODY *body,SYSTEM *system,int *iaBody) {
  double foo;
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    foo = fdRadiusFunctionBaraffe(body[iaBody[0]].dAge, body[iaBody[0]].dMass);
    if (!isnan(foo)) return foo;
    else body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
  }
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_NONE || body[iaBody[0]].iStellarModel == STELLAR_MODEL_CONST)
    return body[iaBody[0]].dRadius;
  else
    return 0;
}

double fdDRotRateDt(BODY *body,SYSTEM *system,int *iaBody) {
  
  double dDRadiusDt = 0;
  double dDJDt = 0;
  
  // First, let's calculate dR/dt due to contraction/expansion
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    // Compute a very simple derivative. NOTE: This won't work if variables like the
    // stellar mass are changing, too! Perhaps it's better to keep track of the previous
    // values of the radius and compute the derivative from those? TODO: Check this.
    
    // Delta t = 10 years. TODO: Check this.
    double eps = 10 * YEARDAY * DAYSEC;
    double dRadMinus, dRadPlus;
    
    dRadMinus = fdRadiusFunctionBaraffe(body[iaBody[0]].dAge - eps, body[iaBody[0]].dMass);
    dRadPlus = fdRadiusFunctionBaraffe(body[iaBody[0]].dAge + eps, body[iaBody[0]].dMass);
    dDRadiusDt = (dRadPlus - dRadMinus) /  (2. * eps);
  }
  
  // Now, let's calculate dJ/dt due to magnetic braking
  // This is from Reiners & Mohanty (2012); see eqn. (2.14) in Miles Timpe's Master's Thesis
  // Note that we force dJ/dt = 0 in the first 1e6 years, since the stellar rotation
  // is likely locked to the disk rotation (Kevin Covey's suggestion).
  if (body[iaBody[0]].iWindModel == STELLAR_MODEL_REINERS) {
    if (body[iaBody[0]].dAge > 1.e6 * YEARSEC) {
      if (body[iaBody[0]].dRotRate >= RM12OMEGACRIT) {
        dDJDt = -RM12CONST * body[iaBody[0]].dRotRate * pow(body[iaBody[0]].dRadius, 16. / 3.) 
                          * pow(body[iaBody[0]].dMass, -2. / 3);
      } else {
        dDJDt = -RM12CONST * pow(body[iaBody[0]].dRotRate / RM12OMEGACRIT, 4.) * body[iaBody[0]].dRotRate
                          * pow(body[iaBody[0]].dRadius, 16. / 3.) * pow(body[iaBody[0]].dMass, -2. / 3);
      }
    }
  }
  
  return 2.5 * dDJDt / (body[iaBody[0]].dMass * body[iaBody[0]].dRadius * body[iaBody[0]].dRadius) 
             - 2 * body[iaBody[0]].dRotRate / body[iaBody[0]].dRadius * dDRadiusDt;

}

double fdTemperature(BODY *body,SYSTEM *system,int *iaBody) {
  double foo;
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    foo = fdTemperatureFunctionBaraffe(body[iaBody[0]].dAge, body[iaBody[0]].dMass);
    if (!isnan(foo)) return foo;
    else body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
  }
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_NONE || body[iaBody[0]].iStellarModel == STELLAR_MODEL_CONST)
    return body[iaBody[0]].dTemperature;
  else
    return 0;
}

double fdLuminosityFunctionBaraffe(double dAge, double dMass) {
  int iError;
  double L = fdBaraffe(STELLAR_L, dAge, dMass, 3, &iError);
  if ((iError == STELLAR_ERR_NONE) || (iError == STELLAR_ERR_LINEAR))
    return L;
  else if (iError == STELLAR_ERR_OUTOFBOUNDS_HI || iError == STELLAR_ERR_ISNAN)
    return NAN;
  else {
    if (iError == STELLAR_ERR_OUTOFBOUNDS_LO)
      fprintf(stderr,"ERROR: Out of bounds (low) in fdBaraffe().\n");
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
  else if (iError == STELLAR_ERR_OUTOFBOUNDS_HI || iError == STELLAR_ERR_ISNAN)
    return NAN;
  else {
    if (iError == STELLAR_ERR_OUTOFBOUNDS_LO)
      fprintf(stderr,"ERROR: Out of bounds (low) in fdBaraffe().\n");
    else if (iError == STELLAR_ERR_FILE)
      fprintf(stderr,"ERROR: File access error in routine fdBaraffe().\n");
    else if (iError == STELLAR_ERR_BADORDER)
      fprintf(stderr,"ERROR: Bad interpolation order in routine fdBaraffe().\n");
    else
      fprintf(stderr,"ERROR: Undefined error in fdBaraffe().\n");
    exit(EXIT_INT);
  }
}

double fdTemperatureFunctionBaraffe(double dAge, double dMass) {
  int iError;
  double T = fdBaraffe(STELLAR_T, dAge, dMass, 3, &iError);
  if ((iError == STELLAR_ERR_NONE) || (iError == STELLAR_ERR_LINEAR))
    return T;
  else if (iError == STELLAR_ERR_OUTOFBOUNDS_HI || iError == STELLAR_ERR_ISNAN)
    return NAN;
  else {
    if (iError == STELLAR_ERR_OUTOFBOUNDS_LO)
      fprintf(stderr,"ERROR: Out of bounds (low) in fdBaraffe().\n");
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
  return 0;
}
