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

// all variables!
void BodyCopyMagmOc(BODY *dest,BODY *src,int foo,int iNumBodies,int iBody) {
  /* Primary variables */
  dest[iBody].dPotTemp          = src[iBody].dPotTemp;
  dest[iBody].dSurfTemp         = src[iBody].dSurfTemp;
  dest[iBody].dSolidRadius      = src[iBody].dSolidRadius;
  dest[iBody].dWaterMassMOAtm   = src[iBody].dWaterMassMOAtm;
  dest[iBody].dWaterMassSol     = src[iBody].dWaterMassSol;
  dest[iBody].dOxygenMassMOAtm  = src[iBody].dOxygenMassMOAtm;
  dest[iBody].dOxygenMassSol    = src[iBody].dOxygenMassSol;
  /* Input variables */
  dest[iBody].dPlanetFormTime   = src[iBody].dPlanetFormTime;
	dest[iBody].dCoreRadius       = src[iBody].dCoreRadius;
	dest[iBody].dWaterMassAtm     = src[iBody].dWaterMassAtm;
  dest[iBody].dManMeltDensity   = src[iBody].dManMeltDensity;
  dest[iBody].dFeO              = src[iBody].dFeO;
	/* Other variables Thermal model */
	dest[iBody].dGravAccelSurf    = src[iBody].dGravAccelSurf;
	dest[iBody].dSolidRadiusLocal = src[iBody].dSolidRadiusLocal;
  dest[iBody].dPrefactorA       = src[iBody].dPrefactorA;
  dest[iBody].dPrefactorB       = src[iBody].dPrefactorB;
  dest[iBody].dMeltFraction     = src[iBody].dMeltFraction;
  dest[iBody].dKinemViscos      = src[iBody].dKinemViscos;
	dest[iBody].dFactorDerivative = src[iBody].dFactorDerivative;
	dest[iBody].dManHeatFlux      = src[iBody].dManHeatFlux;
	dest[iBody].dRadioHeat        = src[iBody].dRadioHeat;
	dest[iBody].dNetFluxAtmo      = src[iBody].dNetFluxAtmo;
	dest[iBody].dAlbedo			 		  = src[iBody].dAlbedo;
	/* Other variables Volatile model */
	dest[iBody].dPressWaterAtm    = src[iBody].dPressWaterAtm;
	dest[iBody].dMassMagmOcLiq 	  = src[iBody].dMassMagmOcLiq;
	dest[iBody].dMassMagmOcCry 	  = src[iBody].dMassMagmOcCry;
	dest[iBody].dWaterFracMelt    = src[iBody].dWaterFracMelt;
	dest[iBody].dFracFe2O3Man     = src[iBody].dFracFe2O3Man;
	dest[iBody].dEscRateHydro     = src[iBody].dEscRateHydro;
	dest[iBody].dEscRateOxy  		  = src[iBody].dEscRateOxy;
	dest[iBody].dOxygenMassAtm    = src[iBody].dOxygenMassAtm;
	dest[iBody].dOxyFugNewMax     = src[iBody].dOxyFugNewMax;
	dest[iBody].dOxyFugFactor     = src[iBody].dOxyFugFactor;
	dest[iBody].dPressAtmTot      = src[iBody].dPressAtmTot;
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

// Don't change this
void ReadOptionsMagmOc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTMAGMOC;iOpt<OPTENDMAGMOC;iOpt++) {
    if (options[iOpt].iType != -1)
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}

// Initilaization of variables
void InitializeBodyMagmOc(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
  // primary variables: HARD CODED INITIAL VALUES
  body[iBody].dPotTemp         = 4000; //body[iBody].dSurfTemp; // initial potential temp = initial surface temp
  body[iBody].dSurfTemp        = 4000;
  body[iBody].dSolidRadius     = body[iBody].dRadius * RADCOREEARTH / REARTH; // initial solid. rad. = core radius
  body[iBody].dWaterMassMOAtm  = 1*TOMASS; // body[iBody].dWaterMassAtm; // initial water mass in MO&Atm is equal to inital Water mass in atmosphere
  body[iBody].dWaterMassSol    = 0; // initial water mass in solid = 0
  body[iBody].dOxygenMassMOAtm = 0; // initial oxygen mass in MO&Atm = 0
  body[iBody].dOxygenMassSol   = 0; // initial oxygen mass in solid = 0

  // other variables
  body[iBody].dPlanetFormTime  = 5e5 * YEARSEC; // the planet is younger than the star (by this time)
  body[iBody].dManMeltDensity  = 4000;
  body[iBody].dCoreRadius      = body[iBody].dRadius * RADCOREEARTH / REARTH; // same relative core radius as Earth
  body[iBody].dPrefactorA      = AHIGHPRESSURE;
  body[iBody].dPrefactorB      = BHIGHPRESSURE;
  body[iBody].dAlbedo          = ALBEDOWATERATMOS;
  body[iBody].dGravAccelSurf   = BIGG * body[iBody].dMass / pow(body[iBody].dRadius,2);
}

/******************* Verify MAGMOC ******************/



/* Assign Nums */

/*
 * Equations needed in PropsAuxMagmOc
 */

/**
Bisection method to find root

@param (*f) function pointer to the function the root of which should be found
@param body A pointer to the current BODY instance
@param dXl the lower boundary of the root finder
@param dXu the upper boundary of the root finder
@param iBody The current BODY number

@return dXm the root of function (*f)
*/
double fndBisection(double (*f)(BODY*,double,int), BODY *body,double dXl, double dXu,int iBody) {
  double dXm,dEpsilon,dProd,dFxm,dFxl;
  dEpsilon = 1;
  while(dEpsilon>1e-5) {
    dXm      = (dXl + dXu)/2.;
    dFxm     = (*f)(body,dXm,iBody);
    if (fabs(dFxm) < 1e-5) {
      return dXm;
    }
    dFxl     = (*f)(body,dXl,iBody);
    if (fabs(dFxl) < 1e-5) {
      return dXl;
    }
    dProd    = (dFxl / fabs(dFxl)) * (dFxm / fabs(dFxm));
    if (dProd < 0) {
      dXu = dXm;
    } else {
      dXl = dXm;
    }
    dEpsilon = fabs((*f)(body,dXm,iBody));
  }
  return dXm;
}

/**
Mass of water in the mo+atm system to get the water frac in the magmoc
Will be used in PropsAuxMagmOc to find its root with fndBisection

@param body A pointer to the current BODY instance
@param dFrac water mass fraction in the magma oean
@param iBody The current BODY number

@return Water mass for a given dFrac - actual water mass in mo+atm
*/
double fndWaterMassMOTime(BODY *body, double dFrac, int iBody) {
  return 1e-19 * (  \
                    WATERPARTCOEFF*dFrac*body[iBody].dMassMagmOcCry \
                    + dFrac*body[iBody].dMassMagmOcLiq \
                    + ( 4*PI*pow(body[iBody].dRadius,2) / body[iBody].dGravAccelSurf ) * pow((dFrac/3.44e-8),1/0.74) \
                    - body[iBody].dWaterMassMOAtm \
                 );
}

/**
Fugacity equation for Fe2O3/FeO_tot = 0.3 -> get critical fugacity
Will be used in PropsAuxMagmOc to find its root with fndBisection

@param body A pointer to the current BODY instance
@param dFug oxygen fugacity
@param iBody The current BODY number

@return 0 if dFug = critical fugacity
*/
double fndCriticalOxygenFugacity(BODY *body, double dFug, int iBody) {
  return 0.196*log(dFug*1e-5) + 11492/body[iBody].dPotTemp - 6.675 \
           - 2.243 * MOLFRACAL2O3 - 1.828 * MOLFRACFEO \
           + 3.201 * MOLFRACCAO   + 5.854 * MOLFRACNA2O \
           + 6.215 * MOLFRACK2O \
           - 3.36  * (1 - 1673/body[iBody].dPotTemp - log(body[iBody].dPotTemp/1673)) \
           - 7.01e-7  * (dFug + body[iBody].dPressWaterAtm)*1e-5 / body[iBody].dPotTemp \
           - 1.54e-10 * (body[iBody].dPotTemp-1673) * (dFug + body[iBody].dPressWaterAtm)*1e-5 / body[iBody].dPotTemp \
           + 3.85e-17 * pow(((dFug + body[iBody].dPressWaterAtm)*1e-5),2) / body[iBody].dPotTemp \
           + 1.735;
}

/**
Oxygen equilibrium between magma ocean and atmosphere
Will be used in PropsAuxMagmOc to find its root with fndBisection

@param body A pointer to the current BODY instance
@param dDelFrac delta mass fraction of Fe2O3 in magmoc
@param iBody The current BODY number

@return 0 if oxygen in atm and mo are in equilibrium
*/
double fndOxygenEquiManAtm(BODY *body, double dDelFrac, int iBody) {
  double dChem, dA, dB, dC, dFracNewMax, dDelFug;

  dChem = 11492/body[iBody].dPotTemp - 6.675 - 2.243 * MOLFRACAL2O3 - 1.828 * MOLFRACFEO \
           + 3.201 * MOLFRACCAO + 5.854 * MOLFRACNA2O + 6.215 * MOLFRACK2O \
           - 3.36  * (1 - 1673/body[iBody].dPotTemp - log(body[iBody].dPotTemp/1673)) \
           - log(MOLWEIGHTFEO/(2*MOLWEIGHTFEO15));
  dA    = - 7.01e-7  / body[iBody].dPotTemp;
  dB    = - 1.54e-10 * (body[iBody].dPotTemp-1673) / body[iBody].dPotTemp;
  dC    =   3.85e-17 / body[iBody].dPotTemp;

  dDelFug = body[iBody].dOxyFugNewMax - dDelFrac * body[iBody].dOxyFugFactor;

  return 0.196 * log((body[iBody].dPressAtmTot-body[iBody].dPressWaterAtm+dDelFug)*1e-5) + dChem \
          + dA * (body[iBody].dPressAtmTot+dDelFug)*1e-5 \
          + dB * (body[iBody].dPotTemp-1673) * (body[iBody].dPressAtmTot+dDelFug)*1e-5 \
          + dC * pow(((body[iBody].dPressAtmTot+dDelFug)*1e-5),2) \
          + log( ( TOTMASSFRACFEO / (body[iBody].dFracFe2O3Man+dDelFrac) ) - 1 );
}

/* Auxs Props */
/* auxiliarie parameters/variables that need to be calculated in order to calculate the primary variable
 * (or at least simplify reading/understanding of the code)
 * calculated every quarter step for Runge-Kutta
 * if needed in other parts of the code, or to be printed: body[iBody]!!! otherwise it will be deleted after the
 * end of this equation
 */
void PropsAuxMagmOc(BODY *body,EVOLVE *evolve,UPDATE *update,int iBody) {

  /*
   * No negative masses!
   */
  if (body[iBody].dWaterMassMOAtm  < 0) {body[iBody].dWaterMassMOAtm  = 0;}
  if (body[iBody].dWaterMassSol    < 0) {body[iBody].dWaterMassSol    = 0;}
  if (body[iBody].dOxygenMassMOAtm < 0) {body[iBody].dOxygenMassMOAtm = 0;}
  if (body[iBody].dOxygenMassSol   < 0) {body[iBody].dOxygenMassSol   = 0;}

  /*
   * Calculation of melt fraction and kinematic viscosity
   * Function melt_fraction() in functions_rk.py
   */
  double daRadius[101];      // radius
  double daTrad[101];        // radius dep. temperature
  double daMelt_frac_r[101]; // radius dep. melt_frac
  double dMelt_vol;         // melt volume
  double dA, dB;             // prefactors for linear solidus
  double dTsol;             // solidus temperature
  double dEta_a;            // liquid viscosity
  double dEta_b;            // solid viscosity
  int iItr;

  dMelt_vol = 0;

  for (iItr=0; iItr<101; iItr=iItr+1){
    daRadius[iItr] = iItr*(body[iBody].dRadius-body[iBody].dCoreRadius)/100 +body[iBody].dCoreRadius;
  }

  for (iItr=0; iItr<100; iItr=iItr+1){
    if (body[iBody].dRadius-daRadius[iItr] < 1e5) {
      dA = ALOWPRESSURE;
      dB = BLOWPRESSURE;
    } else {
      dA = AHIGHPRESSURE;
      dB = BHIGHPRESSURE;
    }
    dTsol        = dA*body[iBody].dManMeltDensity*body[iBody].dGravAccelSurf*(body[iBody].dRadius-daRadius[iItr])+dB;
    daTrad[iItr] = body[iBody].dPotTemp*(1+(THERMALEXPANCOEFF*body[iBody].dGravAccelSurf/SILICATEHEATCAP)*(body[iBody].dRadius-daRadius[iItr]));
    daMelt_frac_r[iItr] = (daTrad[iItr]-dTsol)/600;
    if (daMelt_frac_r[iItr] > 1) {
      daMelt_frac_r[iItr] = 1; // melt fraction can't be larger than 1
    } else if (daMelt_frac_r[iItr] < 0) {
      daMelt_frac_r[iItr] = 0; // melt fraction can't be smaller than 0
    }
    dMelt_vol = dMelt_vol + daMelt_frac_r[iItr]*(pow(daRadius[iItr+1],3)-pow(daRadius[iItr],3));
  }

  body[iBody].dSolidRadiusLocal = body[iBody].dRadius - ( (body[iBody].dPrefactorB-body[iBody].dPotTemp) / (body[iBody].dGravAccelSurf*(body[iBody].dPotTemp*THERMALEXPANCOEFF/SILICATEHEATCAP - body[iBody].dPrefactorA*body[iBody].dManMeltDensity)));

  if (body[iBody].dSolidRadiusLocal < body[iBody].dCoreRadius) {
    body[iBody].dSolidRadiusLocal = body[iBody].dCoreRadius;
    body[iBody].dMeltFraction     = dMelt_vol / (pow(body[iBody].dRadius,3)-pow(body[iBody].dSolidRadiusLocal,3));
  } else if (body[iBody].bManSolid) {
    body[iBody].dSolidRadiusLocal = body[iBody].dRadius;
    body[iBody].dMeltFraction     = 0;
  } else {
    body[iBody].dMeltFraction     = dMelt_vol / (pow(body[iBody].dRadius,3)-pow(body[iBody].dSolidRadiusLocal,3));
  }

  if (body[iBody].dMeltFraction > 1) {
    body[iBody].dMeltFraction = 1;
  } else if (body[iBody].dMeltFraction < 0) {
    body[iBody].dMeltFraction = 0;
  }

  if (body[iBody].dMeltFraction > CRITMELTFRAC) {
    dEta_a = 0.00024 * exp(4600/(body[iBody].dPotTemp-1000)) / pow((1-(1-body[iBody].dMeltFraction)/(1-CRITMELTFRAC)),2.5);
    dEta_b = DYNVISCSOLID * exp(-ACTIVENERGY/(RGAS*body[iBody].dPotTemp));
    body[iBody].dKinemViscos = fmin(dEta_a,dEta_b) / body[iBody].dManMeltDensity;
  } else {
    body[iBody].dKinemViscos = DYNVISCSOLID * exp(-ACTIVENERGY/(RGAS*body[iBody].dPotTemp)) / body[iBody].dManMeltDensity;
  }
  // END of melt_fraction(): returns Melt fraction and Kinematic viscosity

  /*
   * Calculation of the mantle heat flux
   * Function mantle_flux() in functions_rk.py
   */
  if (body[iBody].dPotTemp > body[iBody].dSurfTemp) {
    body[iBody].dManHeatFlux = THERMALCONDUC * pow((body[iBody].dPotTemp-body[iBody].dSurfTemp),1.33) \
                               * pow((THERMALEXPANCOEFF*body[iBody].dGravAccelSurf/(CRITRAYLEIGHNO*THERMALDIFFUS*body[iBody].dKinemViscos)),0.33);
  } else {
    body[iBody].dManHeatFlux = THERMALCONDUC * body[iBody].dPotTemp * THERMALEXPANCOEFF * body[iBody].dGravAccelSurf / SILICATEHEATCAP;
  }
  // END of mantle_flux(): return mantle heat flux

  /*
   * Calculation of the radiogenic heating rate
   * Function rad_heat() in functions_rk.py
   */
  body[iBody].dRadioHeat = 30.8e-9 * 9.46e-5 * exp(1.55e-10*(4.6e9-body[iBody].dAge/YEARSEC)) + \
                           0.22e-9 * 5.69e-4 * exp(9.85e-10*(4.6e9-body[iBody].dAge/YEARSEC)) + \
                           0124e-9 * 2.64e-5 * exp(4.95e-11*(4.6e9-body[iBody].dAge/YEARSEC)) + \
                           36.9e-9 * 2.92e-5 * exp(5.55e-10*(4.6e9-body[iBody].dAge/YEARSEC));
  // End of rad_heat()

  /*
   * Calculation of the water mass fraction in the magma ocean
   * Function water_fraction() in functions_rk.py
   */
  double dMassMagmOcTot; //total mass of magma ocean
  dMassMagmOcTot             = 4./3 * PI * body[iBody].dManMeltDensity * (pow(body[iBody].dRadius,3)-pow(body[iBody].dSolidRadiusLocal,3));
  body[iBody].dMassMagmOcLiq = body[iBody].dMeltFraction * dMassMagmOcTot;
  body[iBody].dMassMagmOcCry = (1 - body[iBody].dMeltFraction) * dMassMagmOcTot;

  if (fabs(fndWaterMassMOTime(body, 0, iBody)) < 1e-5) {
    body[iBody].dWaterFracMelt = 0;
  } else if (fabs(fndWaterMassMOTime(body, 1, iBody)) < 1e-5) {
    body[iBody].dWaterFracMelt = 1;
  } else {
    body[iBody].dWaterFracMelt = fndBisection(fndWaterMassMOTime,body,0,1,iBody);
  }
  // End of water_fraction()

  /* Get water pressure and water mass in the atmosphere */
  body[iBody].dPressWaterAtm = pow((body[iBody].dWaterFracMelt/3.44e-8),1/0.74);
  body[iBody].dWaterMassAtm  = 4 * PI * pow(body[iBody].dRadius,2) * body[iBody].dPressWaterAtm / body[iBody].dGravAccelSurf;

  /*
   * Calculation of the flux from the star
   * Function bol_flux() in functions_rk.py
   */
  double dBolFlux; // bolometric flux from the start
  double dTimeGyr; // time in Gyr
  dTimeGyr = 1e-9 * (body[iBody].dAge + body[iBody].dPlanetFormTime) / YEARSEC;
  if (log10(dTimeGyr) < -0.782) {
    dBolFlux = pow((-0.73 * log10(dTimeGyr) + 3.81),10);
  } else {
    dBolFlux = pow(4.38,10);
  }
  // End of bol_flux()

  /*
   * Calculation of the flux from the atmosphere
   * Function OLR_Elkins() in functions_rk.py
   */
  double dEffTempAtm; // effective temperature of the atmosphere
  double dOptDep;     // optical depth
  dEffTempAtm              = pow((dBolFlux * (1 - body[iBody].dAlbedo) / (4 * SIGMA) ),0.25);
  dOptDep                  = body[iBody].dPressWaterAtm * pow((0.75*ABSORPCOEFFH2O/body[iBody].dGravAccelSurf/REFPRESSUREOPACITY),0.5);
  body[iBody].dNetFluxAtmo = 2 / (2 + dOptDep) * SIGMA * (pow(body[iBody].dSurfTemp,4) - pow(dEffTempAtm,4));
  // End of OLR_Elkins()

  /* Factor in the derivative of Tpot and Rsol */
  body[iBody].dFactorDerivative = SILICATEHEATCAP * (body[iBody].dPrefactorB*THERMALEXPANCOEFF - body[iBody].dPrefactorA*body[iBody].dManMeltDensity*SILICATEHEATCAP) / (body[iBody].dGravAccelSurf*pow((body[iBody].dPrefactorA*body[iBody].dManMeltDensity*SILICATEHEATCAP - THERMALEXPANCOEFF*body[iBody].dPotTemp),2));

  /*
   * Calculation of the atmospheric escape rates
   * Function atmesc() in atmesc_function_rk.py
   * Later replaced by module ATMESC!
   */
  if (body[iBody].dWaterMassMOAtm >= 0) {
    body[iBody].dEscRateHydro = 1e-10;   // hard coded. first 1e-7
    body[iBody].dEscRateOxy   = 1e-11;   // first 1e-6
  } else {
    body[iBody].dEscRateHydro = 0;   // hard coded. first 1e-7
    body[iBody].dEscRateOxy   = 0;   // first 1e-6
  }
  // End of atmesc()

  /*
   * Calculation of the mass fraction of Fe2O3 in the manlte and the mass of oxygen in the atmosphere
   * Function oxygen_mo() in oxygen_rk.py
   */
  double dOxygenMassAtmNew; // mass of oxygen produced
  double dNumFeO15Man;      // corresponding mass of FeO1.5 in the mantle
  double dOxyFugacity;      // Oxygen fugacity = pressure of oxygen in atm
  double dFracFe2O3ManNew;  // New Fe2O3 in mantle
  double dOxyFugacityCrit;  // Critical fugacity

  dOxygenMassAtmNew = 4*PI*pow(body[iBody].dRadius,2) * evolve->dTimeStep * (8*body[iBody].dEscRateHydro - body[iBody].dEscRateOxy);
  dNumFeO15Man      = 2 * dOxygenMassAtmNew / (MOLWEIGHTOXYGEN*ATOMMASS);
  dOxyFugacity      = body[iBody].dOxygenMassAtm * body[iBody].dGravAccelSurf / (4*PI*pow(body[iBody].dRadius,2));

  body[iBody].dPressAtmTot  = dOxyFugacity + body[iBody].dPressWaterAtm;
  body[iBody].dOxyFugNewMax = dOxygenMassAtmNew * body[iBody].dGravAccelSurf / (4*PI*pow(body[iBody].dRadius,2));
  body[iBody].dOxyFugFactor = MOLWEIGHTOXYGEN/(2*MOLWEIGHTFEO15) * body[iBody].dManMeltDensity*body[iBody].dGravAccelSurf * (pow(body[iBody].dRadius,3)-pow(body[iBody].dSolidRadiusLocal,3))/(3*pow(body[iBody].dRadius,2));

  if (body[iBody].bManSolid) {
    dFracFe2O3ManNew = 0;
  } else {
    dFracFe2O3ManNew = MOLWEIGHTFEO15*ATOMMASS*dNumFeO15Man / (body[iBody].dManMeltDensity*PI*(pow(body[iBody].dRadius,3)-pow(body[iBody].dSolidRadiusLocal,3))*4/3);
  }

  /* critical fugacity (corresponding to x=0.3) */
  if (fabs(fndCriticalOxygenFugacity(body,1e10,iBody)) < 1e-5) {
    dOxyFugacityCrit = 1e10;
  } else if (fabs(fndCriticalOxygenFugacity(body,1e-20,iBody)) < 1e-5) {
    dOxyFugacityCrit = 1e-20;
  } else {
    dOxyFugacityCrit = fndBisection(fndCriticalOxygenFugacity,body,1e10,1e-20,iBody);
  }

  if (dOxygenMassAtmNew <= 0) { // net loss of oxygen/no new oxygen
    body[iBody].dFracFe2O3Man  = body[iBody].dFracFe2O3Man;
    body[iBody].dOxygenMassAtm = body[iBody].dOxygenMassAtm + dOxygenMassAtmNew;
  } else if (body[iBody].dFracFe2O3Man >= TOTMASSFRACFEO) { // all FeO oxidized to Fe2O3
    body[iBody].dFracFe2O3Man  = TOTMASSFRACFEO;
    body[iBody].dOxygenMassAtm = body[iBody].dOxygenMassAtm + dOxygenMassAtmNew;
  } else if (body[iBody].dFracFe2O3Man/TOTMASSFRACFEO < 0.3) { // X < 0.3 -> assuming no O2 buildup in atm
    body[iBody].dFracFe2O3Man  = body[iBody].dFracFe2O3Man + dFracFe2O3ManNew;
  } else if (dOxyFugacity <= dOxyFugacityCrit) { // all O stays in atmosphere
    body[iBody].dFracFe2O3Man  = body[iBody].dFracFe2O3Man;
    body[iBody].dOxygenMassAtm = body[iBody].dOxygenMassAtm + dOxygenMassAtmNew;
  } else { // x > 0.3 and fug > fug_crit
    dFracFe2O3ManNew  = fndBisection(fndOxygenEquiManAtm,body,0,1,iBody);
    body[iBody].dFracFe2O3Man  = body[iBody].dFracFe2O3Man + dFracFe2O3ManNew;
    dOxygenMassAtmNew = (body[iBody].dOxyFugNewMax - body[iBody].dFracFe2O3Man*body[iBody].dOxyFugFactor) * 4*PI*pow(body[iBody].dRadius,2) / body[iBody].dGravAccelSurf;
    body[iBody].dOxygenMassAtm = body[iBody].dOxygenMassAtm + dOxygenMassAtmNew;
  }

  if (body[iBody].dOxygenMassAtm < 0) {body[iBody].dOxygenMassAtm = 0;}


  body[iBody].dPressOxygenAtm = body[iBody].dOxygenMassAtm * body[iBody].dGravAccelSurf / (4*PI*pow(body[iBody].dRadius,2));
  // END of oxygen_mo()
}


/* Only updated every full step. Use for check of different behaviors and force parameters to a value,
e.g. if all the hydrogen is lost to space (= no more water in atmosphere) -> set albedo to pure rock
*/
void fnForceBehaviorMagmOc(BODY *body,MODULE *module,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  /* Mantle solidified? */
  if (body[iBody].dSolidRadiusLocal >= body[iBody].dRadius) {
    body[iBody].bManSolid = 1;
  }
  /* High or low pressure regime of the solidus? */
  if ( (body[iBody].dRadius-body[iBody].dSolidRadius) < 1e5) {
    body[iBody].dPrefactorA = ALOWPRESSURE;
    body[iBody].dPrefactorB = BLOWPRESSURE;
  }
  /* Albedo Rock or Water Vapor? */
  if (body[iBody].dPressWaterAtm < 1) {
    body[iBody].dAlbedo = ALBEDOROCK;
  }
}

// ??
void MagmOcExit(FILES *files,char cSpecies[16],int iFile) {

}

// ??
void VerifyPotTemp(BODY *body, OPTIONS *options, UPDATE *update, double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iPotTemp][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iPotTemp][0] = 1;
  update[iBody].iaBody[update[iBody].iPotTemp][0]     = malloc(update[iBody].iNumBodies[update[iBody].iPotTemp][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iPotTemp][0][0]  = iBody;

  update[iBody].pdDPotTemp = &update[iBody].daDerivProc[update[iBody].iPotTemp][0];
}

void VerifySurfTemp(BODY *body, OPTIONS *options, UPDATE *update, double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iSurfTemp][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iSurfTemp][0] = 1;
  update[iBody].iaBody[update[iBody].iSurfTemp][0]     = malloc(update[iBody].iNumBodies[update[iBody].iSurfTemp][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iSurfTemp][0][0]  = iBody;

  update[iBody].pdDSurfTemp = &update[iBody].daDerivProc[update[iBody].iSurfTemp][0];
}

void VerifySolidRadius(BODY *body, OPTIONS *options, UPDATE *update, double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iSolidRadius][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iSolidRadius][0] = 1;
  update[iBody].iaBody[update[iBody].iSolidRadius][0]     = malloc(update[iBody].iNumBodies[update[iBody].iSolidRadius][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iSolidRadius][0][0]  = iBody;

  update[iBody].pdDSolidRadius = &update[iBody].daDerivProc[update[iBody].iSolidRadius][0];
}

void VerifyWaterMassMOAtm(BODY *body, OPTIONS *options, UPDATE *update, double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iWaterMassMOAtm][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iWaterMassMOAtm][0] = 1;
  update[iBody].iaBody[update[iBody].iWaterMassMOAtm][0]     = malloc(update[iBody].iNumBodies[update[iBody].iWaterMassMOAtm][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iWaterMassMOAtm][0][0]  = iBody;

  update[iBody].pdDWaterMassMOAtm = &update[iBody].daDerivProc[update[iBody].iWaterMassMOAtm][0];
}

void VerifyWaterMassSol(BODY *body, OPTIONS *options, UPDATE *update, double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iWaterMassSol][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iWaterMassSol][0] = 1;
  update[iBody].iaBody[update[iBody].iWaterMassSol][0]     = malloc(update[iBody].iNumBodies[update[iBody].iWaterMassSol][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iWaterMassSol][0][0]  = iBody;

  update[iBody].pdDWaterMassSol = &update[iBody].daDerivProc[update[iBody].iWaterMassSol][0];
}

void VerifyOxygenMassMOAtm(BODY *body, OPTIONS *options, UPDATE *update, double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iOxygenMassMOAtm][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iOxygenMassMOAtm][0] = 1;
  update[iBody].iaBody[update[iBody].iOxygenMassMOAtm][0]     = malloc(update[iBody].iNumBodies[update[iBody].iOxygenMassMOAtm][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iOxygenMassMOAtm][0][0]  = iBody;

  update[iBody].pdDOxygenMassMOAtm = &update[iBody].daDerivProc[update[iBody].iOxygenMassMOAtm][0];
}

void VerifyOxygenMassSol(BODY *body, OPTIONS *options, UPDATE *update, double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iOxygenMassSol][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iOxygenMassSol][0] = 1;
  update[iBody].iaBody[update[iBody].iOxygenMassSol][0]     = malloc(update[iBody].iNumBodies[update[iBody].iOxygenMassSol][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iOxygenMassSol][0][0]  = iBody;

  update[iBody].pdDOxygenMassSol = &update[iBody].daDerivProc[update[iBody].iOxygenMassSol][0];
}

// assign a derivativ to the primary variable
void AssignMagmOcDerivatives(BODY *body,EVOLVE *evolve,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody) {
  fnUpdate[iBody][update[iBody].iPotTemp][0]         = &fdDPotTemp;
  fnUpdate[iBody][update[iBody].iSurfTemp][0]        = &fdDSurfTemp;
  fnUpdate[iBody][update[iBody].iSolidRadius][0]     = &fdDSolidRadius;
  fnUpdate[iBody][update[iBody].iWaterMassMOAtm][0]  = &fdDWaterMassMOAtm;
  fnUpdate[iBody][update[iBody].iWaterMassSol][0]    = &fdDWaterMassSol;
  fnUpdate[iBody][update[iBody].iOxygenMassMOAtm][0] = &fdDOxygenMassMOAtm;
  fnUpdate[iBody][update[iBody].iOxygenMassSol][0]   = &fdDOxygenMassSol;
  /* HERE all derivatives*/
}

// derivative for minimal change??
void NullMagmOcDerivatives(BODY *body,EVOLVE *evolve,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody) {
  fnUpdate[iBody][update[iBody].iPotTemp][0]         = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iSurfTemp][0]        = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iSolidRadius][0]     = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iWaterMassMOAtm][0]  = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iWaterMassSol][0]    = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iOxygenMassMOAtm][0] = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iOxygenMassSol][0]   = &fndUpdateFunctionTiny;
}

// call steps to execute next time step??
void VerifyMagmOc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,int iBody,int iModule) {
  VerifyPotTemp(body, options, update, body[iBody].dAge, iBody);
  VerifySurfTemp(body, options, update, body[iBody].dAge, iBody);
  VerifySolidRadius(body, options, update, body[iBody].dAge, iBody);
  VerifyWaterMassMOAtm(body, options, update, body[iBody].dAge, iBody);
  VerifyWaterMassSol(body, options, update, body[iBody].dAge, iBody);
  VerifyOxygenMassMOAtm(body, options, update, body[iBody].dAge, iBody);
  VerifyOxygenMassSol(body, options, update, body[iBody].dAge, iBody);

  control->fnForceBehavior[iBody][iModule]   = &fnForceBehaviorMagmOc;
  control->fnPropsAux[iBody][iModule]        = &PropsAuxMagmOc;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyMagmOc;

}
/**************** MAGMOC update ****************/

// ??
void InitializeUpdateMagmOc(BODY *body,UPDATE *update,int iBody) {
  if (iBody >= 0) {
    if (update[iBody].iNumPotTemp == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumPotTemp++;

    if (update[iBody].iNumSurfTemp == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumSurfTemp++;

    if (update[iBody].iNumSolidRadius == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumSolidRadius++;

    if (update[iBody].iNumWaterMassMOAtm == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumWaterMassMOAtm++;
    /* may use different condition for WaterMassSol*/
    if (update[iBody].iNumWaterMassSol == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumWaterMassSol++;

    if (update[iBody].iNumOxygenMassMOAtm == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumOxygenMassMOAtm++;
    /* may use different condition for OxygenMassSol*/
    if (update[iBody].iNumOxygenMassSol == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumOxygenMassSol++;
  }
}

/***************** MAGMOC Halts *****************/

/*
 * Checks if mantle soldified
 */
int fbHaltMantleSolidifed(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  if (body[iBody].bManSolid) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's mantle completely solidified. \n",body[iBody].cName);
    }
    return 1;
  }
  return 0;
}

void CountHaltsMagmOc(HALT *halt,int *iNumHalts) {
  if (halt->bMantleSolidifed)
    (*iNumHalts)++;
}

void VerifyHaltMagmOc(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
  if (control->Halt[iBody].bMantleSolidifed)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMantleSolidifed;
}

/************* MAGMOC Outputs ******************/
// similar to read input

/* NOTE: If you write a new Write subroutine here you need to add the associate
   block of initialization in InitializeOutputMagmOc below */

void WritePotTemp(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dPotTemp;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTemp(units->iTemp, 1, 0);
    fsUnitsTemp(units->iTemp,cUnit);
  }
}

void WriteSurfTemp(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dSurfTemp;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsTemp(units->iTemp, 1, 0);
    fsUnitsTemp(units->iTemp,cUnit);
  }
}

void WriteSolidRadius(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dSolidRadius;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}

void WriteWaterMassMOAtm(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dWaterMassMOAtm;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void WriteWaterMassSol(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dWaterMassSol;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void WriteOxygenMassMOAtm(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dOxygenMassMOAtm;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void WriteOxygenMassSol(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dOxygenMassSol;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void WritePressWaterAtm(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dPressWaterAtm;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WritePressOxygenAtm(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dPressOxygenAtm;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

// similar to initialize options
void InitializeOutputMagmOc(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_POTTEMP].cName,"PotTemp");
  sprintf(output[OUT_POTTEMP].cDescr,"Potential temperature magma ocean");
  sprintf(output[OUT_POTTEMP].cNeg,"K");
  output[OUT_POTTEMP].bNeg = 1;
  output[OUT_POTTEMP].dNeg = 1; // division factor to get from SI to desired unit
  output[OUT_POTTEMP].iNum = 1;
  output[OUT_POTTEMP].iModuleBit = MAGMOC; //name of module
  fnWrite[OUT_POTTEMP] = &WritePotTemp;

  sprintf(output[OUT_SURFTEMP].cName,"SurfTemp");
  sprintf(output[OUT_SURFTEMP].cDescr,"Surface temperature magma ocean");
  sprintf(output[OUT_SURFTEMP].cNeg,"K");
  output[OUT_SURFTEMP].bNeg = 1;
  output[OUT_SURFTEMP].dNeg = 1; // division factor to get from SI to desired unit
  output[OUT_SURFTEMP].iNum = 1;
  output[OUT_SURFTEMP].iModuleBit = MAGMOC; //name of module
  fnWrite[OUT_SURFTEMP] = &WriteSurfTemp;

  sprintf(output[OUT_SOLIDRADIUS].cName,"SolidRadius");
  sprintf(output[OUT_SOLIDRADIUS].cDescr,"Solidification radius of the mantle");
  sprintf(output[OUT_SOLIDRADIUS].cNeg,"Earth radii");
  output[OUT_SOLIDRADIUS].bNeg = 1;
  output[OUT_SOLIDRADIUS].dNeg = 1/REARTH; // division factor to get from SI to desired unit
  output[OUT_SOLIDRADIUS].iNum = 1;
  output[OUT_SOLIDRADIUS].iModuleBit = MAGMOC; //name of module
  fnWrite[OUT_SOLIDRADIUS] = &WriteSolidRadius;

  sprintf(output[OUT_WATERMASSMOATM].cName,"WaterMassMOAtm");
  sprintf(output[OUT_WATERMASSMOATM].cDescr,"Water mass in magma ocean and atmosphere");
  sprintf(output[OUT_WATERMASSMOATM].cNeg,"TO");
  output[OUT_WATERMASSMOATM].bNeg = 1;
  output[OUT_WATERMASSMOATM].dNeg = 1/TOMASS; // division factor to get from SI to desired unit
  output[OUT_WATERMASSMOATM].iNum = 1;
  output[OUT_WATERMASSMOATM].iModuleBit = MAGMOC; //name of module
  fnWrite[OUT_WATERMASSMOATM] = &WriteWaterMassMOAtm;

  sprintf(output[OUT_WATERMASSSOL].cName,"WaterMassSol");
  sprintf(output[OUT_WATERMASSSOL].cDescr,"Water mass in solidified mantle");
  sprintf(output[OUT_WATERMASSSOL].cNeg,"TO");
  output[OUT_WATERMASSSOL].bNeg = 1;
  output[OUT_WATERMASSSOL].dNeg = 1/TOMASS; // division factor to get from SI to desired unit
  output[OUT_WATERMASSSOL].iNum = 1;
  output[OUT_WATERMASSSOL].iModuleBit = MAGMOC; //name of module
  fnWrite[OUT_WATERMASSSOL] = &WriteWaterMassSol;

  sprintf(output[OUT_OXYGENMASSMOATM].cName,"OxygenMassMOAtm");
  sprintf(output[OUT_OXYGENMASSMOATM].cDescr,"Oxygen mass in magma ocean and atmosphere");
  sprintf(output[OUT_OXYGENMASSMOATM].cNeg,"kg");
  output[OUT_OXYGENMASSMOATM].bNeg = 1;
  output[OUT_OXYGENMASSMOATM].dNeg = 1; // division factor to get from SI to desired unit
  output[OUT_OXYGENMASSMOATM].iNum = 1;
  output[OUT_OXYGENMASSMOATM].iModuleBit = MAGMOC; //name of module
  fnWrite[OUT_OXYGENMASSMOATM] = &WriteOxygenMassMOAtm;

  sprintf(output[OUT_OXYGENMASSSOL].cName,"OxygenMassSol");
  sprintf(output[OUT_OXYGENMASSSOL].cDescr,"Oxygen Mass in solidified mantle");
  sprintf(output[OUT_OXYGENMASSSOL].cNeg,"kg");
  output[OUT_OXYGENMASSSOL].bNeg = 1;
  output[OUT_OXYGENMASSSOL].dNeg = 1; // division factor to get from SI to desired unit
  output[OUT_OXYGENMASSSOL].iNum = 1;
  output[OUT_OXYGENMASSSOL].iModuleBit = MAGMOC; //name of module
  fnWrite[OUT_OXYGENMASSSOL] = &WriteOxygenMassSol;

  sprintf(output[OUT_PRESSWATERATM].cName,"PressWaterAtm");
  sprintf(output[OUT_PRESSWATERATM].cDescr,"Water pressure in atmosphere");
  sprintf(output[OUT_PRESSWATERATM].cNeg,"bar");
  output[OUT_PRESSWATERATM].bNeg = 1;
  output[OUT_PRESSWATERATM].dNeg = 1/1e5; // division factor to get from SI to desired unit
  output[OUT_PRESSWATERATM].iNum = 1;
  output[OUT_PRESSWATERATM].iModuleBit = MAGMOC; //name of module
  fnWrite[OUT_PRESSWATERATM] = &WritePressWaterAtm;

  sprintf(output[OUT_PRESSOXYGENATM].cName,"PressOxygenAtm");
  sprintf(output[OUT_PRESSOXYGENATM].cDescr,"Oxygen pressure in atmosphere");
  sprintf(output[OUT_PRESSOXYGENATM].cNeg,"bar");
  output[OUT_PRESSOXYGENATM].bNeg = 1;
  output[OUT_PRESSOXYGENATM].dNeg = 1/1e5; // division factor to get from SI to desired unit
  output[OUT_PRESSOXYGENATM].iNum = 1;
  output[OUT_PRESSOXYGENATM].iModuleBit = MAGMOC; //name of module
  fnWrite[OUT_PRESSOXYGENATM] = &WritePressOxygenAtm;

}

//========================= Finalize Variable Functions ========================
// ??
void FinalizeUpdatePotTemp(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iNumPotTemp = (*iEqn)++;
}

void FinalizeUpdateSurfTemp(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iNumSurfTemp = (*iEqn)++;
}

void FinalizeUpdateSolidRadius(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iNumSolidRadius = (*iEqn)++;
}

void FinalizeUpdateWaterMassMOAtm(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iNumWaterMassMOAtm = (*iEqn)++;
}

void FinalizeUpdateWaterMassSol(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iNumWaterMassSol = (*iEqn)++;
}

void FinalizeUpdateOxygenMassMOAtm(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iNumOxygenMassMOAtm = (*iEqn)++;
}

void FinalizeUpdateOxygenMassSol(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iNumOxygenMassSol = (*iEqn)++;
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

  module->iaModule[iBody][iModule]                        = MAGMOC;

  module->fnCountHalts[iBody][iModule]                    = &CountHaltsMagmOc;
  module->fnReadOptions[iBody][iModule]                   = &ReadOptionsMagmOc;
  module->fnLogBody[iBody][iModule]                       = &LogBodyMagmOc;
  module->fnVerify[iBody][iModule]                        = &VerifyMagmOc;
  module->fnAssignDerivatives[iBody][iModule]             = &AssignMagmOcDerivatives;
  module->fnNullDerivatives[iBody][iModule]               = &NullMagmOcDerivatives;
  module->fnVerifyHalt[iBody][iModule]                    = &VerifyHaltMagmOc;

  module->fnInitializeUpdate[iBody][iModule]              = &InitializeUpdateMagmOc;
  module->fnInitializeBody[iBody][iModule]                = &InitializeBodyMagmOc;
  module->fnInitializeOutput[iBody][iModule]              = &InitializeOutputMagmOc;

  module->fnFinalizeUpdatePotTemp[iBody][iModule]         = &FinalizeUpdatePotTemp;
  module->fnFinalizeUpdateSurfTemp[iBody][iModule]        = &FinalizeUpdateSurfTemp;
  module->fnFinalizeUpdateSolidRadius[iBody][iModule]     = &FinalizeUpdateSolidRadius;
  module->fnFinalizeUpdateWaterMassMOAtm[iBody][iModule]  = &FinalizeUpdateWaterMassMOAtm;
  module->fnFinalizeUpdateWaterMassSol[iBody][iModule]    = &FinalizeUpdateWaterMassSol;
  module->fnFinalizeUpdateOxygenMassMOAtm[iBody][iModule] = &FinalizeUpdateOxygenMassMOAtm;
  module->fnFinalizeUpdateOxygenMassSol[iBody][iModule]   = &FinalizeUpdateOxygenMassSol;
  /* HERE */

}

/************* MAGMOC Functions ************/
/* real physic is happening here: calculation of the derivatives of the primary variables */
double fdDPotTemp(BODY *body, SYSTEM *system, int *iaBody) {
  double dRadioPart,dManFluxPart,dRsolPart,dFusionPart;
  dRadioPart   = body[iaBody[0]].dRadioHeat * (pow(body[iaBody[0]].dRadius,3) - pow(body[iaBody[0]].dCoreRadius,3));
  dManFluxPart = 3 * pow(body[iaBody[0]].dRadius,2) * body[iaBody[0]].dManHeatFlux / body[iaBody[0]].dManMeltDensity;
  dRsolPart    = SILICATEHEATCAP * (pow(body[iaBody[0]].dRadius,3) - pow(body[iaBody[0]].dSolidRadiusLocal,3));
  dFusionPart  = 3 * HEATFUSIONSILICATE * pow(body[iaBody[0]].dSolidRadiusLocal,2) * body[iaBody[0]].dFactorDerivative;
  return (dRadioPart - dManFluxPart)/(dRsolPart - dFusionPart);
}

double fdDSurfTemp(BODY *body, SYSTEM *system, int *iaBody) {
  double dFluxPart,dWaterPart,dManPart,dTempPart;
  dFluxPart  = body[iaBody[0]].dManHeatFlux - body[iaBody[0]].dNetFluxAtmo;
  dWaterPart = WATERHEATCAP * body[iaBody[0]].dPressWaterAtm / body[iaBody[0]].dGravAccelSurf;
  dManPart   = SILICATEHEATCAP * body[iaBody[0]].dManMeltDensity / (3*pow(body[iaBody[0]].dRadius,2));
  dTempPart  = pow(body[iaBody[0]].dRadius,3) - (body[iaBody[0]].dPotTemp-body[iaBody[0]].dSurfTemp) * THERMALEXPANCOEFF * body[iaBody[0]].dGravAccelSurf / (CRITRAYLEIGHNO*THERMALDIFFUS*body[iaBody[0]].dKinemViscos);
  return dFluxPart / (dWaterPart + dManPart * dTempPart);
}

double fdDSolidRadius(BODY *body, SYSTEM *system, int *iaBody) {
  double dDerivative;
  if (body[iaBody[0]].bManSolid) {
    dDerivative = 0;
  } else if (body[iaBody[0]].dSolidRadiusLocal <= body[iaBody[0]].dCoreRadius) {
    dDerivative = 0;
  } else {
    dDerivative = body[iaBody[0]].dFactorDerivative * fdDPotTemp(body,system,iaBody);
  }
  if (dDerivative < 0) {
    return 0;
  }
  return dDerivative;
}

double fdDWaterMassSol(BODY *body, SYSTEM *system, int *iaBody) {
  return 4*PI*body[iaBody[0]].dManMeltDensity*WATERPARTCOEFF*body[iaBody[0]].dWaterFracMelt*pow(body[iaBody[0]].dSolidRadius,2)*fdDSolidRadius(body,system,iaBody);
}

double fdDWaterMassMOAtm(BODY *body, SYSTEM *system, int *iaBody) {
  if (body[iaBody[0]].dWaterMassMOAtm <= 0) {
    return 0;
  }
  return -fdDWaterMassSol(body,system,iaBody) - 4*PI*pow(body[iaBody[0]].dRadius,2)*body[iaBody[0]].dEscRateHydro*MOLWEIGHTWATER/(2*MOLWEIGHTHYDROGEN);
}

double fdDOxygenMassSol(BODY *body, SYSTEM *system, int *iaBody) {
  return 4*PI*body[iaBody[0]].dManMeltDensity*body[iaBody[0]].dFracFe2O3Man*pow(body[iaBody[0]].dSolidRadius,2) * fdDSolidRadius(body,system,iaBody)*MOLWEIGHTOXYGEN/(2*MOLWEIGHTFEO15);
}

double fdDOxygenMassMOAtm(BODY *body, SYSTEM *system, int *iaBody) {
  return 4*PI*pow(body[iaBody[0]].dRadius,2) * ( body[iaBody[0]].dEscRateHydro*MOLWEIGHTOXYGEN/(2*MOLWEIGHTHYDROGEN) - body[iaBody[0]].dEscRateOxy ) - fdDOxygenMassSol(body,system,iaBody);
}
