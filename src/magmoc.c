/********************** MAGMOC.C **********************/
/*
 * Patrick Barth, Wed May 16 13:37 PDT 2018
 *
 * Subroutines that control the thermal evolution of the
 * magma ocean as well as the geochemistry.
 *
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void BodyCopyMagmoc(BODY *dest,BODY *src,int foo,int iNumBodies,int iBody) {
  dest[iBody].dFeO = src[iBody].dFeO;

}

/**************** MAGMOC options ********************/

/* FeO */

void ReadFeO(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dFeO = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d26AlPowerMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       //CHANGED units Mass to Power.
       body[iFile-1].dFeO = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dFeO = options->dDefault;
}

/* Initiatlize Input Options */

void InitializeOptionsMagmoc(OPTIONS *options,fnReadOption fnRead[]) {

  /* FeO */

  sprintf(options[OPT_FEO].cName,"dFeO");
  sprintf(options[OPT_FEO].cDescr,"Initial Mass Fraction of FeO in the mantle");
  sprintf(options[OPT_FEO].cDefault,"BSE Earth: 0.08");
  options[OPT_FEO].iType = 2;
  options[OPT_FEO].iMultiFile = 1;
  options[OPT_FEO].dNeg = 1;
  options[OPT_FEO].dDefault = 0.08;
  sprintf(options[OPT_FEO].cNeg,"FeO Mass Fraction");
  fnRead[OPT_FEO] = &ReadFeO;

}

void ReadOptionsMagmoc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTMAGMOC;iOpt<OPTENDMAGMOC;iOpt++) {
    if (options[iOpt].iType != -1)
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}

/******************* Verify MAGMOC ******************/



/* Assign Nums */


/* Auxs Props */
void PropsAuxMagmoc(BODY *body,EVOLVE *evolve,UPDATE *update,int iBody) {

}

void fnForceBehaviorMagmoc(BODY *body,MODULE *module,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {

}

void MagmocExit(FILES *files,char cSpecies[16],int iFile) {

}

void VerifyMagmocDerivatives(BODY *body,CONTROL *control,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody) {

}

void VerifyMagmoc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,int iBody,int iModule) {

}


/**************** MAGMOC update ****************/

void InitializeUpdateMagmoc(BODY *body,UPDATE *update,int iBody) {

}

/***************** MAGMOC Halts *****************/



void CountHaltsMagmoc(HALT *halt,int *iNumHalts) {

}

void VerifyHaltMagmoc(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {

}

/************* MAGMOC Outputs ******************/

/* NOTE: If you write a new Write subroutine here you need to add the associate
   block of initialization in InitializeOutputMagmoc below */


void InitializeOutputMagmoc(OUTPUT *output,fnWriteOutput fnWrite[]) {

}

/************ MAGMOC Logging Functions **************/

void LogOptionsMagmoc(CONTROL *control, FILE *fp) {

}

//PED: this would be for global rad heat parameters, but this is blank bc rad is only relevant to each individual body.
void LogMagmoc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {

}

void LogBodyMagmoc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {

}

void AddModuleMagmoc(MODULE *module,int iBody,int iModule) {

}

/************* MAGMOC Functions ************/
