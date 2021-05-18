/**
  @file flare.c

  @brief Subroutines that control the output of XUV flares.

  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

  @date Mar 25 2016

*/

#include "vplanet.h"

void BodyCopyFlare(BODY *dest, BODY *src, int foo, int iNumBodies, int iBody) {
  /* Fake flare parameters -- leave in for now
  dest[iBody].dFlareConst = src[iBody].dFlareConst;
  dest[iBody].dFlareExp = src[iBody].dFlareExp;
  */
}

/**************** FLARE options ********************/


/* Fake flare constant stuff Leaving in for now
void ReadFlareConst(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // This parameter cannot exist in primary file
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
  // This parameter cannot exist in primary file
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
*/

void ReadFlareYInt(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dFlareYInt = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareYInt = options->dDefault;
}
}

void ReadFlareSlope(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dFlareSlope = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareSlope = options->dDefault;
}
}

void ReadFlareC(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dFlareC = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareC = options->dDefault;
}
}

void ReadFlareK(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dFlareK = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareK = options->dDefault;
}
}

void ReadFlareMinEnergy(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareMinEnergy =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareMinEnergy = dTmp;
}
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareMinEnergy = options->dDefault;
}
}

void ReadFlareMaxEnergy(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareMaxEnergy =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareMaxEnergy = dTmp;
}
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareMaxEnergy = options->dDefault;
}
}

void ReadFlareVisWidth(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareVisWidth =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareVisWidth = dTmp;
}
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareVisWidth = options->dDefault;
}
}

void ReadFlareXUVWidth(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareXUVWidth =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareXUVWidth = dTmp;
}
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareXUVWidth = options->dDefault;
}
}

/* Initiatlize Input Options */

void InitializeOptionsFlare(OPTIONS *options, fnReadOption fnRead[]) {
  /* Fake flare constant stuff

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
  */

  sprintf(options[OPT_FLAREYINT].cName, "dFlareYInt");
  sprintf(options[OPT_FLAREYINT].cDescr, "Y-Intercept for Flare Frequency");
  sprintf(options[OPT_FLAREYINT].cDefault, "20.9 (Proxima)");
  sprintf(options[OPT_FLAREYINT].cDimension, "nd");
  options[OPT_FLAREYINT].dDefault   = 20.9;
  options[OPT_FLAREYINT].iType      = 2;
  options[OPT_FLAREYINT].bMultiFile = 1;
  fnRead[OPT_FLAREYINT]             = &ReadFlareYInt;

  sprintf(options[OPT_FLARESLOPE].cName, "dFlareSlope");
  sprintf(options[OPT_FLARESLOPE].cDescr, "Slope for Flare Frequency");
  sprintf(options[OPT_FLARESLOPE].cDefault, "-0.68 (Proxima)");
  sprintf(options[OPT_FLARESLOPE].cDimension, "nd");
  options[OPT_FLARESLOPE].dDefault   = 20.9;
  options[OPT_FLARESLOPE].iType      = 2;
  options[OPT_FLARESLOPE].bMultiFile = 1;
  fnRead[OPT_FLARESLOPE]             = &ReadFlareSlope;

  sprintf(options[OPT_FLAREC].cName, "dFlareC");
  sprintf(options[OPT_FLAREC].cDescr, "10^c for U-XUV Flare Relation");
  sprintf(options[OPT_FLAREC].cDefault, "1.08");
  sprintf(options[OPT_FLAREC].cDimension, "nd");
  options[OPT_FLAREC].dDefault   = 1.08;
  options[OPT_FLAREC].iType      = 2;
  options[OPT_FLAREC].bMultiFile = 1;
  fnRead[OPT_FLAREC]             = &ReadFlareC;

  sprintf(options[OPT_FLAREK].cName, "dFlareK");
  sprintf(options[OPT_FLAREK].cDescr, "E^k for U-XUV Flare Relation");
  sprintf(options[OPT_FLAREK].cDefault, "-4.4");
  sprintf(options[OPT_FLAREK].cDimension, "nd");
  options[OPT_FLAREK].dDefault   = -4.4;
  options[OPT_FLAREK].iType      = 2;
  options[OPT_FLAREK].bMultiFile = 1;
  fnRead[OPT_FLAREK]             = &ReadFlareK;

  sprintf(options[OPT_FLAREVISWIDTH].cName, "dFlareVisWidth");
  sprintf(options[OPT_FLAREVISWIDTH].cDescr,
          "Width of Visible Band for Flare Relation");
  sprintf(options[OPT_FLAREVISWIDTH].cDefault, "3000 Angstroms");
  sprintf(options[OPT_FLAREVISWIDTH].cDimension, "nd");
  options[OPT_FLAREVISWIDTH].dDefault   = 3e-7;
  options[OPT_FLAREVISWIDTH].iType      = 2;
  options[OPT_FLAREVISWIDTH].bMultiFile = 1;
  options[OPT_FLAREVISWIDTH].dNeg       = 1e-10;
  sprintf(options[OPT_FLAREVISWIDTH].cNeg, "Angstroms");
  fnRead[OPT_FLAREVISWIDTH] = &ReadFlareVisWidth;

  sprintf(options[OPT_FLAREXUVWIDTH].cName, "dFlareXUVWidth");
  sprintf(options[OPT_FLAREXUVWIDTH].cDescr, "Width of XUV for Flare Relation");
  sprintf(options[OPT_FLAREXUVWIDTH].cDefault, "1000 Angstroms");
  sprintf(options[OPT_FLAREXUVWIDTH].cDimension, "nd");
  options[OPT_FLAREXUVWIDTH].dDefault   = 1e-7;
  options[OPT_FLAREXUVWIDTH].iType      = 2;
  options[OPT_FLAREXUVWIDTH].bMultiFile = 1;
  options[OPT_FLAREXUVWIDTH].dNeg       = 1e-10;
  sprintf(options[OPT_FLAREXUVWIDTH].cNeg, "Angstroms");
  fnRead[OPT_FLAREXUVWIDTH] = &ReadFlareXUVWidth;

  sprintf(options[OPT_FLAREMINENERGY].cName, "dFlareMinEnergy");
  sprintf(options[OPT_FLAREMINENERGY].cDescr,
          "Minimum Flare Energy to Consider");
  sprintf(options[OPT_FLAREMINENERGY].cDefault, "10^29 ergs");
  sprintf(options[OPT_FLAREMINENERGY].cDimension, "nd");
  options[OPT_FLAREMINENERGY].dDefault   = 1e22;
  options[OPT_FLAREMINENERGY].iType      = 2;
  options[OPT_FLAREMINENERGY].bMultiFile = 1;
  options[OPT_FLAREMINENERGY].dNeg       = 1e-7;
  sprintf(options[OPT_FLAREMINENERGY].cNeg, "ergs");
  fnRead[OPT_FLAREMINENERGY] = &ReadFlareMinEnergy;

  sprintf(options[OPT_FLAREMAXENERGY].cName, "dFlareMaxEnergy");
  sprintf(options[OPT_FLAREMAXENERGY].cDescr,
          "Maximum Flare Energy to Consider");
  sprintf(options[OPT_FLAREMAXENERGY].cDefault, "10^33 ergs");
  sprintf(options[OPT_FLAREMAXENERGY].cDimension, "nd");
  options[OPT_FLAREMAXENERGY].dDefault   = 1e26;
  options[OPT_FLAREMAXENERGY].iType      = 2;
  options[OPT_FLAREMAXENERGY].bMultiFile = 1;
  options[OPT_FLAREMAXENERGY].dNeg       = 1e-7;
  sprintf(options[OPT_FLAREMAXENERGY].cNeg, "ergs");
  fnRead[OPT_FLAREMAXENERGY] = &ReadFlareMaxEnergy;
}

void ReadOptionsFlare(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, fnReadOption fnRead[],
                      int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTFLARE; iOpt < OPTENDFLARE; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
}
  }
}

/******************* Verify FLARE ******************/

void PropsAuxFlare(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                   int iBody) {
  /* Nothing */
}

void fnForceBehaviorFlare(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                          SYSTEM *system, UPDATE *update,
                          fnUpdateVariable ***fnUpdate, int iBody,
                          int iModule) {

  if (body[iBody].dLXUVFlare < 0) {
    body[iBody].dLXUVFlare = 0;
  } else {
    body[iBody].dLXUVFlare = fdLXUVFlare(body, evolve->dTimeStep, iBody);
}
}

void VerifyLXUVFlare(BODY *body, OPTIONS *options, UPDATE *update, int iBody) {

  /* This may become useful once flare evolution is included
  update[iBody].iaType[update[iBody].iLXUV][0] = 1;
  update[iBody].iNumBodies[update[iBody].iLXUV][0]=1;
  update[iBody].iaBody[update[iBody].iLXUV][0] = malloc(update[iBody].iNumBodies[update[iBody].iLXUV][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iLXUV][0][0]=iBody;
  update[iBody].pdDLXUVFlareDt = &update[iBody].daDerivProc[update[iBody].iLXUV][0];
  */
}

void AssignFlareDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                            fnUpdateVariable ***fnUpdate, int iBody) {
  /* No derivatives yet for flare.
  This may become useful once flare evolution is included
  fnUpdate[iBody][update[iBody].iLXUV][0] = &fdDLXUVFlareDt;
  */
}

void NullFlareDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                          fnUpdateVariable ***fnUpdate, int iBody) {
  /* No derivatives yet for flare.
  This may become useful once flare evolution is included
  fnUpdate[iBody][update[iBody].iLXUV][0] = &fndUpdateFunctionTiny;
  */
}

void VerifyFlare(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 OUTPUT *output, SYSTEM *system, UPDATE *update, int iBody,
                 int iModule) {
  int iFile = iBody + 1;

  /* Mass must be in proper range */

  if (body[iBody].dMass < MINMASSFLARE * MSUN ||
      body[iBody].dMass > MAXMASSFLARE * MSUN) {
    fprintf(stderr, "ERROR: Mass of %s must be between %.3lf and %.3lf\n",
            body[iBody].cName, MINMASSFLARE, MAXMASSFLARE);
    LineExit(files->Infile[iBody + 1].cIn, options[OPT_MASS].iLine[iBody + 1]);
  }

  /* VerifyMultiFlareStellar checks if STELLAR was selected, too. */

  VerifyLXUVFlare(body, options, update, iBody);
  control->fnForceBehavior[iBody][iModule]   = &fnForceBehaviorFlare;
  control->fnPropsAux[iBody][iModule]        = &PropsAuxFlare;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyFlare;

  /* For now, user may only input FlareConst and FlareExp. Eventually, user
     should also be able to input dLXUVFlare, which will require a call to
     NotTwoOfThree. So we must get initial LXUVFlare now. */

  body[iBody].dLXUVFlare = fdLXUVFlare(body, control->Evolve.dTimeStep, iBody);
}


void InitializeModuleFlare(CONTROL *control, MODULE *module) {
  /* Anything Here? */
}

/**************** FLARE update ****************/

void InitializeUpdateFlare(BODY *body, UPDATE *update, int iBody) {
  /* STELLAR calculates LXUV from the star's properties, but FLARE calculates LXUV as a primary variable. It is the only possible update. */

  /* No primary variables for FLARE yet
  update[iBody].iNumVars++;
  update[iBody].iNumLXUV++;
  */
}

void FinalizeUpdateLXUVFlare(BODY *body, UPDATE *update, int *iEqn, int iVar,
                             int iBody, int iFoo) {
  /* No primary variables for FLARE yet
  update[iBody].iaModule[iVar][*iEqn] = FLARE;
  update[iBody].iNumLXUV = (*iEqn)++;
  */
}

/***************** FLARE Halts *****************/

/* Halt for massive flare? No Flares? */

void CountHaltsFlare(HALT *halt, int *iNumHalts) {
}

void VerifyHaltFlare(BODY *body, CONTROL *control, OPTIONS *options, int iBody,
                     int *iHalt) {
}

/************* FLARE Outputs ******************/

/* NOTE: If you write a new Write subroutine here you need to add the associate
   block of initialization in InitializeOutputFlare below */

void WriteLXUVFlare(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dLXUVFlare;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

void InitializeOutputFlare(OUTPUT *output, fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_LXUVFLARE].cName, "LXUVFlare");
  sprintf(output[OUT_LXUVFLARE].cDescr, "XUV Luminosity from flares");
  sprintf(output[OUT_LXUVFLARE].cNeg, "LSUN");
  output[OUT_LXUVFLARE].bNeg       = 1;
  output[OUT_LXUVFLARE].dNeg       = 1. / LSUN;
  output[OUT_LXUVFLARE].iNum       = 1;
  output[OUT_LXUVFLARE].iModuleBit = FLARE;
  fnWrite[OUT_LXUVFLARE]           = &WriteLXUVFlare;
}


/************ FLARE Logging Functions **************/

void LogOptionsFlare(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- FLARE Options -----\n\n");
  */
}

void LogFlare(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
              UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {

  /* Anything here?
  int iOut;

  fprintf(fp,"\n----- FLARE PARAMETERS ------\n");
  for (iOut=OUTSTARTRADHEAT;iOut<OUTBODYSTARTRADHEAT;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyFlare(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UPDATE *update, fnWriteOutput fnWrite[], FILE *fp,
                  int iBody) {
  int iOut;

  fprintf(fp, "----- FLARE PARAMETERS (%s)------\n", body[iBody].cName);
  for (iOut = OUTSTARTFLARE; iOut < OUTENDFLARE; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
}
  }
}

void AddModuleFlare(CONTROL *control, MODULE *module, int iBody, int iModule) {

  module->fnCountHalts[iBody][iModule]        = &CountHaltsFlare;
  module->fnReadOptions[iBody][iModule]       = &ReadOptionsFlare;
  module->fnLogBody[iBody][iModule]           = &LogBodyFlare;
  module->fnVerify[iBody][iModule]            = &VerifyFlare;
  module->fnAssignDerivatives[iBody][iModule] = &AssignFlareDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullFlareDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltFlare;

  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateFlare;

  module->fnFinalizeUpdateLXUV[iBody][iModule] = &FinalizeUpdateLXUVFlare;
}

/************* FLARE Functions ************/

/* Fake Flare functions

double fdLXUVFlare(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  return  body[iBody].dFlareConst*exp(-body[iBody].dFlareExp*body[iBody].dAge);
}

double fdDLXUVFlareDt(BODY *body,SYSTEM *system,int *iaBody) {
  // Placeholder for now. L_XUV = const*exp(-const*time)

  return -body[iaBody[0]].dFlareConst*body[iaBody[0]].dFlareExp*exp(-body[iaBody[0]].dFlareExp*body[iaBody[0]].dAge);
}

*/

double fdFlareFrequency(double dYInt, double dSlope, double dLogEnergy) {
  return pow(10, (dSlope * dLogEnergy + dYInt));
}

double fdLXUVFlare(BODY *body, double dDeltaTime, int iBody) {
  double dLogEMin, dLogEMax, dFreqMin, dFreqMax, dWidth, dArea;
  double dEpsVis, dEpsXUV, dLXUVFlare;

  dLogEMin = log10(body[iBody].dFlareMinEnergy * 1e7);
  dLogEMax = log10(body[iBody].dFlareMaxEnergy * 1e7);
  dWidth   = dLogEMax - dLogEMin;

  dFreqMin = fdFlareFrequency(body[iBody].dFlareYInt, body[iBody].dFlareSlope,
                              dLogEMin);
  dFreqMax = fdFlareFrequency(body[iBody].dFlareYInt, body[iBody].dFlareSlope,
                              dLogEMax);

  // Total visible luminosity in log(ergs/day)
  dArea = dWidth * (dFreqMax + 0.5 * (dFreqMin - dFreqMax));

  // Convert to luminosity without powers of 10 and convert to ergs/s
  dArea = pow(10, dArea) / DAYSEC;

  /* The Mitra-Kaev et al. (2005) model is for energy. We therfore are now
     assuming that we are calculating the energy over 1 second */

  // Divide by width of visible in Angstroms
  dEpsVis = dArea / (1e10 * body[iBody].dFlareVisWidth);
  dEpsXUV = pow(10, body[iBody].dFlareC) * pow(dEpsVis, body[iBody].dFlareK);

  // Multiply by width of XUV in Angstroms
  dLXUVFlare = dEpsXUV * body[iBody].dFlareXUVWidth * 1e10;

  // Convert back to Watts and return
  return dLXUVFlare * 1e-7;
}
