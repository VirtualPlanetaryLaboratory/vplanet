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

// what was this again for? primary variable??
void BodyCopyMagmOc(BODY *dest,BODY *src,int foo,int iNumBodies,int iBody) {
  dest[iBody].dFeO = src[iBody].dFeO;
  dest[iBody].dSurfTemp = src[iBody].dSurfTemp;
  dest[iBody].dPotTemp = src[iBody].dPotTemp;
  dest[iBody].dWaterMassMOAtm = src[iBody].dWaterMassMOAtm;
  dest[iBody].dWaterMassSol = src[iBody].dWaterMassSol;
  dest[iBody].dSolidRadius = src[iBody].dSolidRadius;
}

/**************** MAGMOC options ********************/
// read input: first function with read command
/* FeO */

void ReadFeO(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dFeO = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dFeO = options->dDefault;
}

/* Water */
void ReadWaterMassAtm(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dWaterMassAtm = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dWaterMassAtm = options->dDefault;
}

/* Temperature */

void ReadSurfTemp(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dSurfTemp = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dSurfTemp = options->dDefault;
}

/* Density */

void ReadManMeltDensity(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dManMeltDensity = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dManMeltDensity = options->dDefault;
}

/* Initiatlize Input Options */
// initialize input = tell code what he is reading in
void InitializeOptionsMagmOc(OPTIONS *options,fnReadOption fnRead[]) {

  /* FeO */

  sprintf(options[OPT_FEO].cName,"dFeO"); //name of the variable
  sprintf(options[OPT_FEO].cDescr,"Initial Mass Fraction of FeO in the mantle"); //description that will be shown for vplanet -h
  sprintf(options[OPT_FEO].cDefault,"BSE Earth: 0.08"); //comment what the default value will be
  options[OPT_FEO].iType = 2; //type of the variable: double??
  options[OPT_FEO].iMultiFile = 1; //can it be used in multiple files? 1 = yes
  options[OPT_FEO].dNeg = 1; //is there a unit other than the SI unit? factor to convert between both units
  options[OPT_FEO].dDefault = 0.08; // default value
  sprintf(options[OPT_FEO].cNeg,"no unit"); // specify unit (for help)
  fnRead[OPT_FEO] = &ReadFeO; //link read function from above

  /* Water */

  sprintf(options[OPT_WATERMASSATM].cName,"dWaterMassAtm");
  sprintf(options[OPT_WATERMASSATM].cDescr,"Initial Water Mass in the atmosphere");
  sprintf(options[OPT_WATERMASSATM].cDefault,"1 Terrestrial Ocean");
  options[OPT_WATERMASSATM].iType = 2;
  options[OPT_WATERMASSATM].iMultiFile = 1;
  options[OPT_WATERMASSATM].dNeg = TOMASS; // for input: factor to mulitply for SI - for output: divide (e.g. 1/TOMASS)
  options[OPT_WATERMASSATM].dDefault = 1;
  sprintf(options[OPT_WATERMASSATM].cNeg,"Terrestrial Oceans");
  fnRead[OPT_WATERMASSATM] = &ReadWaterMassAtm;

  /* Temperature */

  sprintf(options[OPT_SURFTEMP].cName,"dSurfTemp");
  sprintf(options[OPT_SURFTEMP].cDescr,"Initial surface temp");
  sprintf(options[OPT_SURFTEMP].cDefault,"4000 K");
  options[OPT_SURFTEMP].iType = 2;
  options[OPT_SURFTEMP].iMultiFile = 1;
  options[OPT_SURFTEMP].dNeg = 1;
  options[OPT_SURFTEMP].dDefault = 4000;
  sprintf(options[OPT_SURFTEMP].cNeg,"Kelvin");
  fnRead[OPT_SURFTEMP] = &ReadSurfTemp;

  /* Density */

  sprintf(options[OPT_MANMELTDENSITY].cName,"dManMeltDensity");
  sprintf(options[OPT_MANMELTDENSITY].cDescr,"Density of the molten mantle");
  sprintf(options[OPT_MANMELTDENSITY].cDefault,"4000 kg/m^3");
  options[OPT_MANMELTDENSITY].iType = 2;
  options[OPT_MANMELTDENSITY].iMultiFile = 1;
  options[OPT_MANMELTDENSITY].dNeg = 1;
  options[OPT_MANMELTDENSITY].dDefault = 4000;
  sprintf(options[OPT_MANMELTDENSITY].cNeg,"kg/m^3");
  fnRead[OPT_MANMELTDENSITY] = &ReadManMeltDensity;

}

// ??
void ReadOptionsMagmOc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTMAGMOC;iOpt<OPTENDMAGMOC;iOpt++) {
    if (options[iOpt].iType != -1)
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}

// Initilaization of a primary variable
void InitializeBodyMagmOc(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
  body[iBody].dWaterMassMOAtm = body[iBody].dWaterMassAtm; // initial water mass in MO&Atm is equal to inital Water mass in atmosphere
  body[iBody].dPotTemp = body[iBody].dSurfTemp; // initial potential temp = initial surface temp
  body[iBody].dWaterMassSol = 0; // initial water mass in solid = 0
  body[iBody].dSolidRadius = body[iBody].dRadius * RADCOREEARTH / REARTH; // initial solid. rad. = core radius
  body[iBody].dPrefactorA = AHIGHPRESSURE;
  body[iBody].dPrefactorB = BHIGHPRESSURE;
}

/******************* Verify MAGMOC ******************/



/* Assign Nums */


/* Auxs Props */
/* auxiliarie parameters/variables that need to be calculated in order to calculate the primary variable
(or at least simplify reading/understanding of the code)
calculated every quarter step for Runge-Kutta
if needed in other parts of the code, or to be printed: body[iBody]!!! otherwise it will be deleted after the
end of this equation */
void PropsAuxMagmOc(BODY *body,EVOLVE *evolve,UPDATE *update,int iBody) {

  // define help variables to calculate melt_frac
  double radius[101];      // radius
  double Trad[101];        // radius dep. temperature
  double melt_frac_r[101]; // radius dep. melt_frac
  double melt_vol;         // melt volume
  double r_c;              // core radius
  double a, b;             // prefactors for linear solidus
  double Tsol;             // solidus temperature
  int j;

  r_c = body[iBody].dRadius * RADCOREEARTH / REARTH;
  melt_vol = 0;

  for (j=0; j<100; j=j+1){
    radius[j] = j*(body[iBody].dRadius-r_c)/100 +r_c;
    if (body[iBody].dRadius-radius[j] < 1e5) {
      a = ALOWPRESSURE;
      b = BLOWPRESSURE;
    } else {
      a = AHIGHPRESSURE;
      b = BHIGHPRESSURE;
    }
    Tsol = a*body[iBody].dManMeltDensity*body[iBody].dGravAccel*(body[iBody].dRadius-radius[j])+b;
    Trad[j] = body[iBody].dPotTemp*(1+(THERMALEXPANCOEFF*body[iBody].dGravAccel/SILICATEHEATCAP)*body[iBody].dRadius-radius[j]);
    melt_frac_r[j] = (Trad[j]-Tsol)/600;
    if (melt_frac_r[j] > 1) {
      melt_frac_r[j] = 1; // melt fraction can't be larger than 1
    } else if (melt_frac_r[j] < 0) {
      melt_frac_r[j] = 0; // melt fraction can't be smaller than 0
    }
    melt_vol = melt_vol + melt_frac_r[j]*(pow(radius[j+1],3)-pow(radius[j],3));
  }

  body[iBody].dSolidRadius = body[iBody].dRadius - ( (body[iBody].dPrefactorB-body[iBody].dPotTemp) / (body[iBody].dGravAccel*(body[iBody].dPotTemp*THERMALEXPANCOEFF/SILICATEHEATCAP - body[iBody].dPrefactorA*body[iBody].dManMeltDensity)));

  if (body[iBody].dSolidRadius < r_c) {
    body[iBody].dSolidRadius = r_c;
    body[iBody].dMeltFraction = melt_vol / (pow(body[iBody].dRadius,3)-pow(body[iBody].dSolidRadius,3));
  } else if (body[iBody].dSolidRadius >= body[iBody].dRadius) {
    body[iBody].dSolidRadius = body[iBody].dRadius;
    body[iBody].dMeltFraction = 0;
  } else {
    body[iBody].dMeltFraction = melt_vol / (pow(body[iBody].dRadius,3)-pow(body[iBody].dSolidRadius,3));
  }

  if (body[iBody].dMeltFraction > 1) {
    body[iBody].dMeltFraction = 1;
  } else if (body[iBody].dMeltFraction < 0) {
    body[iBody].dMeltFraction = 0;
  }

  if (body[iBody].dMeltFraction > CRITMELTFRAC) {
    body[iBody].dDynamViscos = DYNVISCLIQUID / pow((1-(1-body[iBody].dMeltFraction)/(1-CRITMELTFRAC)),2.5);
  } else {
    body[iBody].dDynamViscos = DYNVISCSOLID * exp(-ACTIVENERGY/(IDEALGASCONST*body[iBody].dPotTemp));
  }

}

/* Only updated every full step. Use for check of different behaviors and force parameters to a value,
e.g. if all the hydrogen is lost to space (= no more water in atmosphere) -> set albedo to pure rock
*/
void fnForceBehaviorMagmOc(BODY *body,MODULE *module,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  if ( (body[iBody].dRadius-body[iBody].dSolidRadius) < 1e5) {
    body[iBody].dPrefactorA = ALOWPRESSURE;
    body[iBody].dPrefactorB = BLOWPRESSURE;
  }
}

// ??
void MagmOcExit(FILES *files,char cSpecies[16],int iFile) {

}

// ??
void VerifyWaterMassMOAtm(BODY *body, OPTIONS *options, UPDATE *update, double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iWaterMassMOAtm][0] = 1;
  update[iBody].iNumBodies[update[iBody].iWaterMassMOAtm][0] = 1;
  update[iBody].iaBody[update[iBody].iWaterMassMOAtm][0] = malloc(update[iBody].iNumBodies[update[iBody].iWaterMassMOAtm][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iWaterMassMOAtm][0][0] = iBody;

  update[iBody].pdDWaterMassMOAtm = &update[iBody].daDerivProc[update[iBody].iWaterMassMOAtm][0];

}

// assign a derivativ to the primary variable
void AssignMagmOcDerivatives(BODY *body,EVOLVE *evolve,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody) {
  fnUpdate[iBody][update[iBody].iWaterMassMOAtm][0] = &fdDWaterMassMOAtm;
  /* HERE all derivatives*/
}

// derivative for minimal change??
void NullMagmOcDerivatives(BODY *body,EVOLVE *evolve,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody) {
  fnUpdate[iBody][update[iBody].iWaterMassMOAtm][0] = &fndUpdateFunctionTiny;
}

// call steps to execute next time step??
void VerifyMagmOc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,int iBody,int iModule) {
  VerifyWaterMassMOAtm(body, options, update, body[iBody].dAge, iBody);

  control->fnForceBehavior[iBody][iModule]   = &fnForceBehaviorMagmOc;
  control->fnPropsAux[iBody][iModule]        = &PropsAuxMagmOc;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyMagmOc;

}
/**************** MAGMOC update ****************/

// ??
void InitializeUpdateMagmOc(BODY *body,UPDATE *update,int iBody) {
  if (iBody >= 0) {
    if (update[iBody].iNumWaterMassMOAtm == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumWaterMassMOAtm++;
  }
}

/***************** MAGMOC Halts *****************/



void CountHaltsMagmOc(HALT *halt,int *iNumHalts) {

}

void VerifyHaltMagmOc(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {

}

/************* MAGMOC Outputs ******************/
// similar to read input

/* NOTE: If you write a new Write subroutine here you need to add the associate
   block of initialization in InitializeOutputMagmOc below */

 // void WriteFe2O3(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
 //   /* Get total power from 26Al
 //   *dTmp = -(*(update[iBody].pdD26AlNumManDt))*ENERGY26Al;  */
 //   *dTmp = fdFe2O3(update,iBody);
 //   if (output->bDoNeg[iBody]) {
 //     *dTmp *= output->dNeg;
 //     strcpy(cUnit,output->cNeg);
 //   } else {
 //     *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
 //     fsUnitsPower(units,cUnit);
 //   }
 // }

 void WriteWaterMassMOAtm(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
   /* Get total power from 26Al
   *dTmp = -(*(update[iBody].pdD26AlNumManDt))*ENERGY26Al;  */
   *dTmp = body[iBody].dWaterMassMOAtm;
   if (output->bDoNeg[iBody]) {
     *dTmp *= output->dNeg;
     strcpy(cUnit,output->cNeg);
   } else {
     *dTmp /= fdUnitsMass(units->iMass);
     fsUnitsMass(units->iMass,cUnit);
   }
 }

// similar to initialize options
void InitializeOutputMagmOc(OUTPUT *output,fnWriteOutput fnWrite[]) {

  // sprintf(output[OUT_FE2O3].cName,"Fe2O3");
  // sprintf(output[OUT_FE2O3].cDescr,"Mass fraction of Fe2O3 in the mantle");
  // sprintf(output[OUT_FE2O3].cNeg,"0");
  // output[OUT_FE2O3].bNeg = 1;
  // output[OUT_FE2O3].dNeg = 1;
  // output[OUT_FE2O3].iNum = 1;
  // output[OUT_FE2O3].iModuleBit = MAGMOC;
  // fnWrite[OUT_FE2O3] = &WriteFe2O3;

  sprintf(output[OUT_WATERMASSMOATM].cName,"WaterMassMOAtm");
  sprintf(output[OUT_WATERMASSMOATM].cDescr,"Watermass in magma ocean and atmosphere");
  sprintf(output[OUT_WATERMASSMOATM].cNeg,"Terrestrial Oceans");
  output[OUT_WATERMASSMOATM].bNeg = 1;
  output[OUT_WATERMASSMOATM].dNeg = 1/TOMASS; // division factor to get from SI to desired unit
  output[OUT_WATERMASSMOATM].iNum = 1;
  output[OUT_WATERMASSMOATM].iModuleBit = MAGMOC; //name of module
  fnWrite[OUT_WATERMASSMOATM] = &WriteWaterMassMOAtm;

}

//========================= Finalize Variable Functions ========================
// ??
void FinalizeUpdateWaterMassMOAtm(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iNumWaterMassMOAtm = (*iEqn)++;
}

/************ MAGMOC Logging Functions **************/

void LogOptionsMagmOc(CONTROL *control, FILE *fp) {

}

//PED: this would be for global rad heat parameters, but this is blank bc rad is only relevant to each individual body.
void LogMagmOc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {

}

void LogBodyMagmOc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {

}

void AddModuleMagmOc(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule]                      = MAGMOC;

  module->fnCountHalts[iBody][iModule]                  = &CountHaltsMagmOc;
  module->fnReadOptions[iBody][iModule]                 = &ReadOptionsMagmOc;
  module->fnLogBody[iBody][iModule]                     = &LogBodyMagmOc;
  module->fnVerify[iBody][iModule]                      = &VerifyMagmOc;
  module->fnAssignDerivatives[iBody][iModule]           = &AssignMagmOcDerivatives;
  module->fnNullDerivatives[iBody][iModule]             = &NullMagmOcDerivatives;
  module->fnVerifyHalt[iBody][iModule]                  = &VerifyHaltMagmOc;

  module->fnInitializeUpdate[iBody][iModule]            = &InitializeUpdateMagmOc;
  module->fnInitializeBody[iBody][iModule]              = &InitializeBodyMagmOc;
  module->fnInitializeOutput[iBody][iModule]            = &InitializeOutputMagmOc;

  module->fnFinalizeUpdateWaterMassMOAtm[iBody][iModule] = &FinalizeUpdateWaterMassMOAtm;
  /* HERE */

}

/************* MAGMOC Functions ************/
// real physic is happening here: calculation of the derivatives of the primary variable!
double fdDWaterMassMOAtm(BODY *body,CONTROL *control, SYSTEM *system, int *iaBody) {
  return 1e3 * sin(1e-4 * control->Evolve.dTime);
}
