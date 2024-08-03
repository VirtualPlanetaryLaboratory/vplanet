/********************** MAGMOC.C **********************/
/*
 * Patrick Barth, Wed May 16 13:37 PDT 2018
 *
 * Subroutines that control the thermal evolution of the
 * magma ocean as well as the geochemistry.
 *
 */

#include "vplanet.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// all variables!
void BodyCopyMagmOc(BODY *dest, BODY *src, int foo, int iNumBodies, int iBody) {
  /* Primary variables */
  dest[iBody].dPotTemp           = src[iBody].dPotTemp;
  dest[iBody].dSurfTemp          = src[iBody].dSurfTemp;
  dest[iBody].dSolidRadius       = src[iBody].dSolidRadius;
  dest[iBody].dWaterMassMOAtm    = src[iBody].dWaterMassMOAtm;
  dest[iBody].dWaterMassSol      = src[iBody].dWaterMassSol;
  dest[iBody].dOxygenMassMOAtm   = src[iBody].dOxygenMassMOAtm;
  dest[iBody].dOxygenMassSol     = src[iBody].dOxygenMassSol;
  dest[iBody].dHydrogenMassSpace = src[iBody].dHydrogenMassSpace;
  dest[iBody].dOxygenMassSpace   = src[iBody].dOxygenMassSpace;
  dest[iBody].dCO2MassMOAtm      = src[iBody].dCO2MassMOAtm;
  dest[iBody].dCO2MassSol        = src[iBody].dCO2MassSol;
  /* Input variables */
  dest[iBody].dCoreRadius     = src[iBody].dCoreRadius;
  dest[iBody].dWaterMassAtm   = src[iBody].dWaterMassAtm;
  dest[iBody].dManMeltDensity = src[iBody].dManMeltDensity;
  dest[iBody].dMassFracFeOIni = src[iBody].dMassFracFeOIni;
  dest[iBody].dWaterPartCoeff = src[iBody].dWaterPartCoeff;
  dest[iBody].dDepthMO        = src[iBody].dDepthMO;
  /* Other variables Thermal model */
  dest[iBody].dGravAccelSurf    = src[iBody].dGravAccelSurf;
  dest[iBody].dSolidRadiusLocal = src[iBody].dSolidRadiusLocal;
  dest[iBody].dTransDepthSol    = src[iBody].dTransDepthSol;
  dest[iBody].dPrefactorA       = src[iBody].dPrefactorA;
  dest[iBody].dPrefactorB       = src[iBody].dPrefactorB;
  dest[iBody].dMeltFraction     = src[iBody].dMeltFraction;
  dest[iBody].dMeltFracSurf     = src[iBody].dMeltFracSurf;
  dest[iBody].dKinemViscos      = src[iBody].dKinemViscos;
  dest[iBody].dFactorDerivative = src[iBody].dFactorDerivative;
  dest[iBody].dManHeatFlux      = src[iBody].dManHeatFlux;
  dest[iBody].dRadioHeat        = src[iBody].dRadioHeat;
  dest[iBody].dTidalHeat        = src[iBody].dTidalHeat;
  dest[iBody].dNetFluxAtmo      = src[iBody].dNetFluxAtmo;
  dest[iBody].dAlbedo           = src[iBody].dAlbedo;
  /* Other variables Volatile model */
  dest[iBody].dPressWaterAtm        = src[iBody].dPressWaterAtm;
  dest[iBody].dPartialPressWaterAtm = src[iBody].dPartialPressWaterAtm;
  dest[iBody].dPressOxygenAtm       = src[iBody].dPressOxygenAtm;
  dest[iBody].dPressCO2Atm          = src[iBody].dPressCO2Atm;
  dest[iBody].dPartialPressCO2Atm   = src[iBody].dPartialPressCO2Atm;
  dest[iBody].dMassMagmOcLiq        = src[iBody].dMassMagmOcLiq;
  dest[iBody].dMassMagmOcCry        = src[iBody].dMassMagmOcCry;
  dest[iBody].dWaterFracMelt        = src[iBody].dWaterFracMelt;
  dest[iBody].dCO2FracMelt          = src[iBody].dCO2FracMelt;
  dest[iBody].dFracFe2O3Man         = src[iBody].dFracFe2O3Man;
  dest[iBody].dOxygenMassAtm        = src[iBody].dOxygenMassAtm;
  dest[iBody].dAveMolarMassMan      = src[iBody].dAveMolarMassMan;
  /* Connection to AtmEsc */
  dest[iBody].dWaterMassEsc  = src[iBody].dWaterMassEsc;
  dest[iBody].dOxygenMassEsc = src[iBody].dOxygenMassEsc;
  /* Boolean */
  dest[iBody].bManSolid         = src[iBody].bManSolid;
  dest[iBody].bAllFeOOxid       = src[iBody].bAllFeOOxid;
  dest[iBody].bLowPressSol      = src[iBody].bLowPressSol;
  dest[iBody].bManStartSol      = src[iBody].bManStartSol;
  dest[iBody].bCalcFugacity     = src[iBody].bCalcFugacity;
  dest[iBody].bPlanetDesiccated = src[iBody].bPlanetDesiccated;
  dest[iBody].bManQuasiSol      = src[iBody].bManQuasiSol;
  dest[iBody].bMagmOcHaltSolid  = src[iBody].bMagmOcHaltSolid;
  dest[iBody].bMagmOcHaltDesicc = src[iBody].bMagmOcHaltDesicc;
  dest[iBody].bEscapeStop       = src[iBody].bEscapeStop;
  dest[iBody].bCO2InAtmosphere  = src[iBody].bCO2InAtmosphere;
  /* Model indicators */
  dest[iBody].iRadioHeatModel = src[iBody].iRadioHeatModel;
  dest[iBody].iMagmOcAtmModel = src[iBody].iMagmOcAtmModel;
}

/**************** MAGMOC options ********************/
// read input: first function with read command
/* FeO */

void ReadMassFracFeOIni(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dMassFracFeOIni =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dMassFracFeOIni = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].dMassFracFeOIni = options->dDefault;
  }
}

/* Water */
void ReadWaterMassAtm(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dWaterMassAtm =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dWaterMassAtm = fdUnitsMass(dTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile >
        0) { // if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile - 1].dWaterMassAtm = options->dDefault;
    }
  }
}

/* CO2 Mass */
void ReadCO2MassMOAtm(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dCO2MassMOAtm =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dCO2MassMOAtm = fdUnitsMass(dTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile >
        0) { // if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile - 1].dCO2MassMOAtm = options->dDefault;
    }
  }
}

/* Temperature */

void ReadSurfTemp(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    // use build-in conversion file -> distorb.c e.g.
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dSurfTemp =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dSurfTemp =
            fdUnitsTemp(dTmp, control->Units[iFile].iTemp, 0);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile >
        0) { // if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile - 1].dSurfTemp = options->dDefault;
    }
  }
}

/* Density */

void ReadManMeltDensity(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dManMeltDensity =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dManMeltDensity = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile >
        0) { // if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile - 1].dManMeltDensity = options->dDefault;
    }
  }
}

/* Water partition coefficient */

void ReadWaterPartCoeff(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dWaterPartCoeff =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dWaterPartCoeff = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile >
        0) { // if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile - 1].dWaterPartCoeff = options->dDefault;
    }
  }
}

/* Magma Ocean Depth */
void ReadDepthMO(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].dDepthMO =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dDepthMO = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile >
        0) { // if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile - 1].dDepthMO = options->dDefault;
    }
  }
}

/*
 * Read halt options
 */

/* Halt when mantle solidified */

void ReadHaltMantleSolidified(BODY *body, CONTROL *control, FILES *files,
                              OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bHaltMantleSolidified = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &control->Halt[iFile - 1].bHaltMantleSolidified,
                       files->iNumInputs);
    }
  }
}

void ReadHaltMantleMeltFracLow(BODY *body, CONTROL *control, FILES *files,
                               OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bHaltMantleMeltFracLow = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options,
                       &control->Halt[iFile - 1].bHaltMantleMeltFracLow,
                       files->iNumInputs);
    }
  }
}

void ReadHaltAtmDesiSurfCool(BODY *body, CONTROL *control, FILES *files,
                             OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bHaltAtmDesiSurfCool = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &control->Halt[iFile - 1].bHaltAtmDesiSurfCool,
                       files->iNumInputs);
    }
  }
}

void ReadHaltEnterHabZone(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bHaltEnterHabZone = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &control->Halt[iFile - 1].bHaltEnterHabZone,
                       files->iNumInputs);
    }
  }
}

void ReadHaltAllPlanetsSolid(BODY *body, CONTROL *control, FILES *files,
                             OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bHaltAllPlanetsSolid = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &control->Halt[iFile - 1].bHaltAllPlanetsSolid,
                       files->iNumInputs);
    }
  }
}

void ReadHaltAllPlanetsDesicc(BODY *body, CONTROL *control, FILES *files,
                              OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bHaltAllPlanetsDesicc = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &control->Halt[iFile - 1].bHaltAllPlanetsDesicc,
                       files->iNumInputs);
    }
  }
}

/*
 * Read model options
 */

void ReadRadioHeatModel(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "schaefer", 4)) {
      body[iFile - 1].iRadioHeatModel = MAGMOC_SCHAEFER;
    } else if (!memcmp(sLower(cTmp), "none", 4)) {
      body[iFile - 1].iRadioHeatModel = MAGMOC_NONE;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iRadioHeatModel = MAGMOC_NONE;
  }
}

void ReadMagmOcAtmModel(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "petit", 4)) {
      body[iFile - 1].iMagmOcAtmModel = MAGMOC_PETIT;
    } else if (!memcmp(sLower(cTmp), "grey", 4)) {
      body[iFile - 1].iMagmOcAtmModel = MAGMOC_GREY;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iMagmOcAtmModel = MAGMOC_GREY;
  }
}

void ReadMantleQuasiSolid(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bOptManQuasiSol = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &body[iFile - 1].bOptManQuasiSol,
                       files->iNumInputs);
    }
  }
}

/* Initiatlize Input Options */
// initialize input = tell code what he is reading in
void InitializeOptionsMagmOc(OPTIONS *options, fnReadOption fnRead[]) {

  /* FeO */

  fvFormattedString(&options[OPT_FEO].cName, "dMassFracFeOIni"); // name of the variable
  fvFormattedString(&options[OPT_FEO].cDescr,
          "Initial Mass Fraction of FeO in the "
          "mantle"); // description that will be shown for vplanet -h
  fvFormattedString(&options[OPT_FEO].cDefault,
          "BSE Earth: 0.0788"); // comment what the default value will be
  fvFormattedString(&options[OPT_FEO].cDimension, "nd");
  options[OPT_FEO].iType      = 2; // type of the variable: double??
  options[OPT_FEO].bMultiFile = 1; // can it be used in multiple files? 1 = yes
  options[OPT_FEO].dNeg = 1; // is there a unit other than the SI unit? factor
                             // to convert between both units
  options[OPT_FEO].dDefault = 0.0788;        // default value
  fvFormattedString(&options[OPT_FEO].cNeg, "no unit"); // specify unit (for help)
  fnRead[OPT_FEO] = &ReadMassFracFeOIni;     // link read function from above

  /* Water */

  // XXX This probably overlaps with dSurfWaterMass in AtmEsc
  fvFormattedString(&options[OPT_WATERMASSATM].cName, "dWaterMassAtm");
  fvFormattedString(&options[OPT_WATERMASSATM].cDescr,
          "Initial Water Mass in the atmosphere");
  fvFormattedString(&options[OPT_WATERMASSATM].cDefault, "1 Terrestrial Ocean");
  fvFormattedString(&options[OPT_WATERMASSATM].cDimension, "mass");
  options[OPT_WATERMASSATM].iType      = 2;
  options[OPT_WATERMASSATM].bMultiFile = 1;
  options[OPT_WATERMASSATM].dNeg =
        TOMASS; // for input: factor to mulitply for SI - for output: divide
                // (e.g. 1/TOMASS)
  options[OPT_WATERMASSATM].dDefault = TOMASS;
  fvFormattedString(&options[OPT_WATERMASSATM].cNeg, "TO");
  fnRead[OPT_WATERMASSATM] = &ReadWaterMassAtm;

  /* CO2 */

  fvFormattedString(&options[OPT_CO2MASSMOATM].cName, "dCO2MassMOAtm");
  fvFormattedString(&options[OPT_CO2MASSMOATM].cDescr, "Initial CO2 mass in the system");
  fvFormattedString(&options[OPT_CO2MASSMOATM].cDefault, "0 TO");
  fvFormattedString(&options[OPT_CO2MASSMOATM].cDimension, "mass");
  options[OPT_CO2MASSMOATM].iType      = 2;
  options[OPT_CO2MASSMOATM].bMultiFile = 1;
  options[OPT_CO2MASSMOATM].dNeg =
        TOMASS; // for input: factor to mulitply for SI - for output: divide
                // (e.g. 1/TOMASS)
  options[OPT_CO2MASSMOATM].dDefault = 0;
  fvFormattedString(&options[OPT_CO2MASSMOATM].cNeg, "TO");
  fnRead[OPT_CO2MASSMOATM] = &ReadCO2MassMOAtm;

  /* Temperature */

  fvFormattedString(&options[OPT_SURFTEMP].cName, "dSurfTemp");
  fvFormattedString(&options[OPT_SURFTEMP].cDescr, "Initial surface temp");
  fvFormattedString(&options[OPT_SURFTEMP].cDefault, "4000 K");
  fvFormattedString(&options[OPT_SURFTEMP].cDimension, "temperature");
  options[OPT_SURFTEMP].iType      = 2;
  options[OPT_SURFTEMP].bMultiFile = 1;
  options[OPT_SURFTEMP].dNeg       = 1;
  options[OPT_SURFTEMP].dDefault   = 4000;
  fvFormattedString(&options[OPT_SURFTEMP].cNeg, "Kelvin");
  fnRead[OPT_SURFTEMP] = &ReadSurfTemp;

  /* Density */

  fvFormattedString(&options[OPT_MANMELTDENSITY].cName, "dManMeltDensity");
  fvFormattedString(&options[OPT_MANMELTDENSITY].cDescr, "Density of the molten mantle");
  fvFormattedString(&options[OPT_MANMELTDENSITY].cDefault, "4000 kg/m^3");
  fvFormattedString(&options[OPT_MANMELTDENSITY].cDimension, "mass/length^3");
  options[OPT_MANMELTDENSITY].iType      = 2;
  options[OPT_MANMELTDENSITY].bMultiFile = 1;
  options[OPT_MANMELTDENSITY].dNeg       = 1;
  options[OPT_MANMELTDENSITY].dDefault   = 4000;
  fvFormattedString(&options[OPT_MANMELTDENSITY].cNeg, "kg/m^3");
  fnRead[OPT_MANMELTDENSITY] = &ReadManMeltDensity;

  /* Water partition coefficient */

  fvFormattedString(&options[OPT_WATERPARTCOEFF].cName, "dWaterPartCoeff");
  fvFormattedString(&options[OPT_WATERPARTCOEFF].cDescr,
          "Water partition coefficient between melt and solid");
  fvFormattedString(&options[OPT_WATERPARTCOEFF].cDefault, "0.01");
  fvFormattedString(&options[OPT_WATERPARTCOEFF].cDimension, "nd");
  options[OPT_WATERPARTCOEFF].iType      = 2;
  options[OPT_WATERPARTCOEFF].bMultiFile = 1;
  options[OPT_WATERPARTCOEFF].dNeg       = 1;
  options[OPT_WATERPARTCOEFF].dDefault   = 0.01;
  fvFormattedString(&options[OPT_WATERPARTCOEFF].cNeg, "no unit");
  fnRead[OPT_WATERPARTCOEFF] = &ReadWaterPartCoeff;

  /* Magma Ocean Depth */

  fvFormattedString(&options[OPT_DEPTHMO].cName, "dDepthMO");
  fvFormattedString(&options[OPT_DEPTHMO].cDescr, "Initial depth of the magma ocean");
  fvFormattedString(&options[OPT_DEPTHMO].cDefault, "core radius");
  fvFormattedString(&options[OPT_DEPTHMO].cDimension, "length");
  options[OPT_DEPTHMO].iType      = 2;
  options[OPT_DEPTHMO].bMultiFile = 1;
  options[OPT_DEPTHMO].dNeg       = 1e3;
  options[OPT_DEPTHMO].dDefault   = 1e6;
  fvFormattedString(&options[OPT_DEPTHMO].cNeg, "km");
  fnRead[OPT_DEPTHMO] = &ReadDepthMO;

  /* Halts */

  fvFormattedString(&options[OPT_HALTMANTLESOLIDIFIED].cName, "bHaltMantleSolidified");
  fvFormattedString(&options[OPT_HALTMANTLESOLIDIFIED].cDescr,
          "Halt when mantle solidified?");
  fvFormattedString(&options[OPT_HALTMANTLESOLIDIFIED].cDefault, "0");
  options[OPT_HALTMANTLESOLIDIFIED].iType = 0;
  fnRead[OPT_HALTMANTLESOLIDIFIED]        = &ReadHaltMantleSolidified;

  fvFormattedString(&options[OPT_HALTMANTLEMELTFRACLOW].cName, "bHaltMantleMeltFracLow");
  fvFormattedString(&options[OPT_HALTMANTLEMELTFRACLOW].cDescr,
          "Halt when mantle mostly solidified?");
  fvFormattedString(&options[OPT_HALTMANTLEMELTFRACLOW].cDefault, "0");
  options[OPT_HALTMANTLEMELTFRACLOW].iType = 0;
  fnRead[OPT_HALTMANTLEMELTFRACLOW]        = &ReadHaltMantleMeltFracLow;

  fvFormattedString(&options[OPT_HALTATMDESISRUFCOOL].cName, "bHaltAtmDesiSurfCool");
  fvFormattedString(&options[OPT_HALTATMDESISRUFCOOL].cDescr,
          "Halt when atmosphere desiccated and surface below 1000K?");
  fvFormattedString(&options[OPT_HALTATMDESISRUFCOOL].cDefault, "0");
  options[OPT_HALTATMDESISRUFCOOL].iType = 0;
  fnRead[OPT_HALTATMDESISRUFCOOL]        = &ReadHaltAtmDesiSurfCool;

  fvFormattedString(&options[OPT_HALTENTERHABZONE].cName, "bHaltEnterHabZone");
  fvFormattedString(&options[OPT_HALTENTERHABZONE].cDescr,
          "Halt when planet enters habitable zone?");
  fvFormattedString(&options[OPT_HALTENTERHABZONE].cDefault, "0");
  options[OPT_HALTENTERHABZONE].iType = 0;
  fnRead[OPT_HALTENTERHABZONE]        = &ReadHaltEnterHabZone;

  fvFormattedString(&options[OPT_HALTALLPLANETSSOLID].cName, "bHaltAllPlanetsSolid");
  fvFormattedString(&options[OPT_HALTALLPLANETSSOLID].cDescr,
          "Halt when all planets solidified?");
  fvFormattedString(&options[OPT_HALTALLPLANETSSOLID].cDefault, "0");
  options[OPT_HALTALLPLANETSSOLID].iType = 0;
  fnRead[OPT_HALTALLPLANETSSOLID]        = &ReadHaltAllPlanetsSolid;

  // XXX Overlap with bHaltSurfaceDesiccated in AtmEsc
  fvFormattedString(&options[OPT_HALTALLPLANETSDESICC].cName, "bHaltAllPlanetsDesicc");
  fvFormattedString(&options[OPT_HALTALLPLANETSDESICC].cDescr,
          "Halt when all planets desiccated?");
  fvFormattedString(&options[OPT_HALTALLPLANETSDESICC].cDefault, "0");
  options[OPT_HALTALLPLANETSDESICC].iType = 0;
  fnRead[OPT_HALTALLPLANETSDESICC]        = &ReadHaltAllPlanetsDesicc;

  /* Model options */

  fvFormattedString(&options[OPT_RADIOHEATMODEL].cName, "sRadioHeatModel");
  fvFormattedString(&options[OPT_RADIOHEATMODEL].cDescr, "Radiogenic heating model");
  fvFormattedString(&options[OPT_RADIOHEATMODEL].cDefault, "NONE");
  options[OPT_RADIOHEATMODEL].iType      = 3;
  options[OPT_RADIOHEATMODEL].bMultiFile = 1;
  fnRead[OPT_RADIOHEATMODEL]             = &ReadRadioHeatModel;

  fvFormattedString(&options[OPT_MAGMOCATMMODEL].cName, "sMagmOcAtmModel");
  fvFormattedString(&options[OPT_MAGMOCATMMODEL].cDescr,
          "Atmospheric flux model: Grey or Petit");
  fvFormattedString(&options[OPT_MAGMOCATMMODEL].cDefault, "GREY");
  options[OPT_MAGMOCATMMODEL].iType      = 3;
  options[OPT_MAGMOCATMMODEL].bMultiFile = 1;
  fnRead[OPT_MAGMOCATMMODEL]             = &ReadMagmOcAtmModel;

  fvFormattedString(&options[OPT_MANQUASISOL].cName, "bOptManQuasiSol");
  fvFormattedString(&options[OPT_MANQUASISOL].cDescr, "Solidify when melt frac = 0.4?");
  fvFormattedString(&options[OPT_MANQUASISOL].cDefault, "0");
  options[OPT_MANQUASISOL].iType      = 0;
  options[OPT_MANQUASISOL].bMultiFile = 1;
  fnRead[OPT_MANQUASISOL]             = &ReadMantleQuasiSolid;
}

// Don't change this
void ReadOptionsMagmOc(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, fnReadOption fnRead[],
                       int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTMAGMOC; iOpt < OPTENDMAGMOC; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}

// Initilaization of variables
void InitializeBodyMagmOc(BODY *body, CONTROL *control, UPDATE *update,
                          int iBody, int iModule) {
  double dSolidRadiusLocalLow, dSolidRadiusLocalHigh;

  // primary variables: HARD CODED INITIAL VALUES
  body[iBody].dPotTemp =
        body[iBody].dSurfTemp; // initial potential temp = initial surface temp
  body[iBody].dCoreRadius = body[iBody].dRadius * RADCOREEARTH /
                            REARTH; // same relative core radius as Earth
  body[iBody].dWaterMassMOAtm =
        body[iBody].dWaterMassAtm;  // initial water mass in MO&Atm is equal to
                                    // inital Water mass in atmosphere
  body[iBody].dWaterMassSol    = 0; // initial water mass in solid = 0
  body[iBody].dOxygenMassMOAtm = 0; // initial oxygen mass in MO&Atm = 0
  body[iBody].dOxygenMassSol   = 0; // initial oxygen mass in solid = 0
  body[iBody].dGravAccelSurf =
        BIGG * body[iBody].dMass / pow(body[iBody].dRadius, 2);

  dSolidRadiusLocalLow =
        body[iBody].dRadius -
        ((BLOWPRESSURE - body[iBody].dPotTemp) /
         (body[iBody].dGravAccelSurf *
          (body[iBody].dPotTemp * THERMALEXPANCOEFF / SILICATEHEATCAP -
           ALOWPRESSURE * body[iBody].dManMeltDensity)));
  dSolidRadiusLocalHigh =
        body[iBody].dRadius -
        ((BHIGHPRESSURE - body[iBody].dPotTemp) /
         (body[iBody].dGravAccelSurf *
          (body[iBody].dPotTemp * THERMALEXPANCOEFF / SILICATEHEATCAP -
           AHIGHPRESSURE * body[iBody].dManMeltDensity)));
  body[iBody].dSolidRadius = fmin(dSolidRadiusLocalLow, dSolidRadiusLocalHigh);

  if (body[iBody].dSolidRadius < body[iBody].dCoreRadius) {
    body[iBody].dSolidRadius = body[iBody].dCoreRadius;
  }

  // if (body[iBody].dDepthMO < 9e8) {
  //   body[iBody].dSolidRadius = body[iBody].dRadius - body[iBody].dDepthMO;
  // }

  // other variables
  double dTransPressSol =
        5.19964e9; // pressure at which to swith from low to high pressure
                   // treatment of solidus (Hirschmann, 2000) in Pa
  body[iBody].dPrefactorA     = AHIGHPRESSURE;
  body[iBody].dPrefactorB     = BHIGHPRESSURE;
  body[iBody].dAlbedo         = ALBEDOWATERATMOS;
  body[iBody].dFracFe2O3Man   = 0;
  body[iBody].dPressOxygenAtm = 0;
  body[iBody].dTransDepthSol =
        body[iBody].dRadius -
        pow((pow(body[iBody].dRadius, 2) -
             2 * body[iBody].dRadius * dTransPressSol /
                   (body[iBody].dManMeltDensity * body[iBody].dGravAccelSurf)),
            0.5);
  body[iBody].dHydrogenMassSpace = 0;
  body[iBody].dOxygenMassSpace   = 0;

  // CO2
  body[iBody].dPressCO2Atm = body[iBody].dCO2MassMOAtm *
                             body[iBody].dGravAccelSurf /
                             (4 * PI *
                              pow(body[iBody].dRadius,
                                  2)); // initial CO2 mass in MO&Atm is equal to
                                       // inital CO2 mass in atmosphere
  body[iBody].dCO2MassSol = 0;         // initial water mass in solid = 0
  if (body[iBody].dCO2MassMOAtm < 1) {
    body[iBody].bCO2InAtmosphere = 0;
  } else {
    body[iBody].bCO2InAtmosphere = 1;
  }
  body[iBody].dCO2FracMelt =
        body[iBody].dCO2MassMOAtm /
        (4 / 3 * PI * body[iBody].dManMeltDensity *
         (pow(body[iBody].dRadius, 3) - pow(body[iBody].dCoreRadius, 3)));


  // initialize water pressure in atmosphere to avoid deviding by 0. Use 1 % of
  // initial water mass
  body[iBody].dPressWaterAtm = body[iBody].dWaterMassAtm *
                               body[iBody].dGravAccelSurf /
                               (4 * PI * pow(body[iBody].dRadius, 2));

  // initialize bools
  body[iBody].bManSolid   = 0; // start with a (partially) molten mantle
  body[iBody].bAllFeOOxid = 0; // start with a unoxidized FeO
  if (body[iBody].dPressWaterAtm >= PRESSWATERMIN) {
    body[iBody].bPlanetDesiccated =
          0; // start with water in the atm + mo system
  } else {
    body[iBody].bPlanetDesiccated = 1; // desiccated from start
  }
  body[iBody].bManStartSol      = 0; // mantle does not solidify from beginning
  body[iBody].bLowPressSol      = 0; // start high pressure region of mantle
  body[iBody].bManQuasiSol      = 0; // start with a (partially) molten mantle
  body[iBody].bEscapeStop       = 0; // start with atmospheric escaped
  body[iBody].bMagmOcHaltSolid  = 0; // no halt at beginning
  body[iBody].bMagmOcHaltDesicc = 0; // no halt at beginning

  double dMassMantle;
  double dManMolNum;
  double dMolNumAl2O3, dMolNumCaO, dMolNumNa2O, dMolNumK2O, dMolNumFeO;
  double dMolNumSiO2, dMolNumMgO, dMolNumTiO2, dMolNumP2O5;

  dMassMantle =
        4. / 3 * PI * body[iBody].dManMeltDensity *
        (pow(body[iBody].dRadius, 3) - pow(body[iBody].dSolidRadius, 3));
  dMolNumAl2O3 = dMassMantle * MASSFRACAL2O3 / MOLWEIGHTAL2O3;
  dMolNumCaO   = dMassMantle * MASSFRACCAO / MOLWEIGHTCAO;
  dMolNumNa2O  = dMassMantle * MASSFRACNA2O / MOLWEIGHTNA2O;
  dMolNumK2O   = dMassMantle * MASSFRACK2O / MOLWEIGHTK2O;
  dMolNumFeO   = dMassMantle * body[iBody].dMassFracFeOIni / MOLWEIGHTFEO;
  dMolNumSiO2  = dMassMantle * MASSFRACSIO2 / MOLWEIGHTSIO2;
  dMolNumMgO   = dMassMantle * MASSFRACMGO / MOLWEIGHTMGO;
  dMolNumTiO2  = dMassMantle * MASSFRACTIO2 / MOLWEIGHTTIO2;
  dMolNumP2O5  = dMassMantle * MASSFRACP2O5 / MOLWEIGHTP2O5;
  dManMolNum   = dMolNumAl2O3 + dMolNumCaO + dMolNumNa2O + dMolNumK2O +
               dMolNumFeO + dMolNumSiO2 + dMolNumMgO + dMolNumTiO2 +
               dMolNumP2O5;
  body[iBody].dAveMolarMassMan =
        (MOLWEIGHTAL2O3 * dMolNumAl2O3 + MOLWEIGHTCAO * dMolNumCaO +
         MOLWEIGHTNA2O * dMolNumNa2O + MOLWEIGHTK2O * dMolNumK2O +
         MOLWEIGHTFEO * dMolNumFeO + MOLWEIGHTSIO2 * dMolNumSiO2 +
         MOLWEIGHTMGO * dMolNumMgO + MOLWEIGHTTIO2 * dMolNumTiO2 +
         MOLWEIGHTP2O5 * dMolNumP2O5) /
        dManMolNum;

  if (!body[0].bStellar) {
    printf("Module STELLAR not used for star. Flux only for GJ1132. \n");
  }

  if (body[iBody].bCO2InAtmosphere &&
      body[iBody].iMagmOcAtmModel == MAGMOC_PETIT) {
    printf("WARNING: When including CO2, petit atmosphere model cannot be "
           "used! Set to grey. \n");
    body[iBody].iMagmOcAtmModel = MAGMOC_GREY;
  }

  if (body[iBody].iMagmOcAtmModel == MAGMOC_PETIT) {
    printf("WARNING: petit atmosphere model can only be used when modelling "
           "GJ1132b! \n");
  }
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
double fndBisection(double (*f)(BODY *, double, int), BODY *body, double dXl,
                    double dXu, double dEps, int iBody) {
  double dXm, dEpsilon, dProd, dFxm, dFxl;
  dEpsilon = 10 * dEps;
  if (dEpsilon > dEps) {
    while (dEpsilon > dEps) {
      dXm  = (dXl + dXu) / 2.;
      dFxm = (*f)(body, dXm, iBody);
      if (fabs(dFxm) < dEps) {
        return dXm;
      }
      dFxl = (*f)(body, dXl, iBody);
      if (fabs(dFxl) < dEps) {
        return dXl;
      }
      dProd = (dFxl / fabs(dFxl)) * (dFxm / fabs(dFxm));
      if (dProd < 0) {
        dXu = dXm;
      } else {
        dXl = dXm;
      }
      dEpsilon = fabs((*f)(body, dXm, iBody));
    }
    return dXm;
  } else {
    fprintf(stderr,"ERROR: Tolerance factor <= 0 in fndBisection.");
    exit(EXIT_INT);
  }  
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
  return 1e-19 *
         (body[iBody].dWaterPartCoeff * dFrac * body[iBody].dMassMagmOcCry +
          dFrac * body[iBody].dMassMagmOcLiq +
          (4 * PI * pow(body[iBody].dRadius, 2) / body[iBody].dGravAccelSurf) *
                pow((dFrac / 3.44e-8), 1 / 0.74) -
          body[iBody].dWaterMassMOAtm);
}
// return 1e-19 * (  WATERPARTCOEFF*dFrac*body[iBody].dMassMagmOcCry \
//                   + dFrac*body[iBody].dMassMagmOcLiq \
//                   + ( 4*PI*pow(body[iBody].dRadius,2) / body[iBody].dGravAccelSurf ) * pow((dFrac/3.44e-8),1/0.74) \
//                   - body[iBody].dWaterMassMOAtm );
// }

/**
Mass of CO2 in the mo+atm system to get the water frac in the magmoc
Will be used in PropsAuxMagmOc to find its root with fndBisection

@param body A pointer to the current BODY instance
@param dFracCO2 CO2 mass fraction in the magma oean
@param iBody The current BODY number

@return CO2 mass for a given dFracCO2 - actual CO2 mass in mo+atm
*/
double fndCO2MassMOTime(BODY *body, double dFracCO2, int iBody) {
  double dPartialPressCO2AtmTemp;
  double dPressCO2AtmTemp;

  dPartialPressCO2AtmTemp = pow(((100 * dFracCO2 - 0.05) / 2.08e-4), 1 / 0.45);
  dPressCO2AtmTemp =
        1 / (2 * MOLWEIGHTCO2) *
        ((-1) * pow(pow(-MOLWEIGHTCO2 * dPartialPressCO2AtmTemp +
                              MOLWEIGHTWATER * body[iBody].dPressWaterAtm +
                              2 * MOLWEIGHTOXYGEN * body[iBody].dPressOxygenAtm,
                        2) +
                          4 * pow(MOLWEIGHTCO2, 2) * dPartialPressCO2AtmTemp *
                                (body[iBody].dPressOxygenAtm +
                                 body[iBody].dPressWaterAtm),
                    0.5) +
         dPartialPressCO2AtmTemp * MOLWEIGHTCO2 -
         MOLWEIGHTWATER * body[iBody].dPressWaterAtm -
         2 * MOLWEIGHTOXYGEN * body[iBody].dPressOxygenAtm);

  return 1e-19 *
         (CO2PARTCOEFF * dFracCO2 * body[iBody].dMassMagmOcCry +
          dFracCO2 * body[iBody].dMassMagmOcLiq +
          (4 * PI * pow(body[iBody].dRadius, 2) / body[iBody].dGravAccelSurf) *
                dPressCO2AtmTemp -
          body[iBody].dCO2MassMOAtm);
}

/**
Physical pressure of CO2 in the atmosphere
Will be used in PropsAuxMagmOc to find its root with fndBisection

@param body A pointer to the current BODY instance
@param dPhysPressCO2 Physical CO2 pressure in the atmosphere
@param iBody The current BODY number

@return 0
*/
double fndPhysPressCO2(BODY *body, double dPhysPressCO2, int iBody) {
  double dAveMolarMassAtm;

  dAveMolarMassAtm = (MOLWEIGHTWATER * body[iBody].dPressWaterAtm +
                      2 * MOLWEIGHTOXYGEN * body[iBody].dPressOxygenAtm +
                      MOLWEIGHTCO2 * dPhysPressCO2) /
                     (body[iBody].dPressWaterAtm + body[iBody].dPressOxygenAtm +
                      dPhysPressCO2);
  return body[iBody].dPartialPressCO2Atm * MOLWEIGHTCO2 / dAveMolarMassAtm -
         dPhysPressCO2;
}

/**
Radiogenic heating used in Schaefer et al. (2016)
Earth like composition

@param body A pointer to the current BODY instance
@param iBody The current BODY number

@return radiogenic heating rate
*/
double fndRadioHeatingEarth(BODY *body, int iBody) {

  return 30.8e-9 * 9.46e-5 *
               exp(1.55e-10 * (4.6e9 - body[iBody].dAge / YEARSEC)) +
         0.22e-9 * 5.69e-4 *
               exp(9.85e-10 * (4.6e9 - body[iBody].dAge / YEARSEC)) +
         0124e-9 * 2.64e-5 *
               exp(4.95e-11 * (4.6e9 - body[iBody].dAge / YEARSEC)) +
         36.9e-9 * 2.92e-5 *
               exp(5.55e-10 * (4.6e9 - body[iBody].dAge / YEARSEC));
}

/**
Bolometric flux of GJ1132 used in Schaefer et al. (2016)
Fit to Schaefer Fig. 2 (Baraffe, Mstar=0.18Msun, orbit of GJ1132b)

@param body A pointer to the current BODY instance
@param iBody The current BODY number

@return bolometric flux at GJ1132b's orbit
*/
double fndBolFluxSchaefer(BODY *body, int iBody) {

  double dTimeGyr; // time in Gyr

  dTimeGyr = 1e-9 * (body[iBody].dAge) / YEARSEC;

  if (log10(dTimeGyr) < -0.782) {
    return pow(10, (-0.73 * log10(dTimeGyr) + 3.81));
  } else {
    return pow(10, 4.38);
  }
}

/**
Atmospheric net flux with a grey atmosphere
From Elkins-Tanton (2008) & Carone et al. (2014)

@param body A pointer to the current BODY instance
@param iBody The current BODY number

@return Atmospheric net flux
*/
double fndNetFluxAtmGrey(BODY *body, int iBody) {


  if ((body[iBody].dPotTemp <= 1800) && (body[iBody].dPotTemp >= 600) &&
      (body[iBody].dPressWaterAtm >= PRESSWATERMIN)) {
    double dOLR =
          280; // Outgoing Longwave Radiation, Runaway Greenhouse (W/m^2)
    double dASR = pow(body[iBody].dEffTempAtm, 4) *
                  SIGMA; // Absorbed Stellar Radiation (W/m^2)

    return dOLR - dASR;

  } else {
    double dOptDep; // optical depth
    dOptDep = body[iBody].dPartialPressWaterAtm *
                    pow((0.75 * ABSORPCOEFFH2O / body[iBody].dGravAccelSurf /
                         REFPRESSUREOPACITY),
                        0.5) +
              body[iBody].dPartialPressCO2Atm *
                    pow((0.75 * ABSORPCOEFFCO2 / body[iBody].dGravAccelSurf /
                         REFPRESSUREOPACITY),
                        0.5);

    return 2 / (2 + dOptDep) * SIGMA *
           (pow(body[iBody].dPotTemp, 4) - pow(body[iBody].dEffTempAtm, 4));
  }
}

/**
Atmospheric net flux with the petitCODE
From Molliere et al. (2015)
Only for GJ1132b with 100% Water atmosphere

@param body A pointer to the current BODY instance
@param iBody The current BODY number

@return Atmospheric net flux
*/
double fndNetFluxAtmPetit(BODY *body, double dTime, int iBody) {
  double dLogP, dTsurf; // log10 of water pressure, surface temperature,
  double dLogF_ms, dLogF_pms,
        dLogF; // log10 of atmospheric net flux (main sequence, pre-main
               // sequence, acutal time)
  double dTimeMain = 1.647e8;
  double dFNetPetit;

  double dOLR = 280; // Outgoing Longwave Radiation, Runaway Greenhouse (W/m^2)
  double dASR = pow(body[iBody].dEffTempAtm, 4) *
                SIGMA; // Absorbed Stellar Radiation (W/m^2)

  if ((body[iBody].dPotTemp <= 1800) && (body[iBody].dPotTemp >= 600) &&
      (body[iBody].dPressWaterAtm >= PRESSWATERMIN)) {
    return dOLR - dASR;
  } else {
    dLogP  = log10(body[iBody].dPressWaterAtm / 1e5);
    dTsurf = body[iBody].dPotTemp;

    dLogF_ms = -8.03520391e-02 + 3.08508158e-03 * dTsurf -
               6.96356770e-01 * dLogP - 3.09084067e-07 * pow(dTsurf, 2) +
               2.38672208e-08 * pow(dTsurf, 2) * dLogP -
               2.58853235e-08 * pow(dTsurf, 2) * pow(dLogP, 2) -
               3.60631795e-01 * pow(dLogP, 2) +
               1.90372485e-04 * dTsurf * pow(dLogP, 2) -
               1.63177944e-04 * dTsurf * dLogP;

    dLogF_pms = -8.40997236e+00 + 7.66867497e-03 * dTsurf -
                4.43217915e-01 * dLogP - 9.48344751e-07 * pow(dTsurf, 2) +
                5.70475594e-08 * pow(dTsurf, 2) * dLogP -
                2.62351040e-08 * pow(dTsurf, 2) * pow(dLogP, 2) -
                1.88040467e-01 * pow(dLogP, 2) +
                1.45691797e-04 * dTsurf * pow(dLogP, 2) -
                3.61617207e-04 * dTsurf * dLogP;

    dLogF = dLogF_pms + (dTime - 1) * (dLogF_ms - dLogF_pms) / (dTimeMain - 1);

    dFNetPetit = pow(10, dLogF);

    if ((dFNetPetit < (dOLR - dASR)) &&
        (body[iBody].dPressWaterAtm >= PRESSWATERMIN)) {
      dFNetPetit = dOLR - dASR;
    }

    return dFNetPetit;
  }
}

/**
Calculation of Fe2O3 mass fraction in the m.o. and oxygen mass in the
atmosphere. Used by Schaefer et al. (2016)

@param body A pointer to the current BODY instance
@param iBody The current BODY number
*/
void fndFe2O3MassFracOxyMass(BODY *body, int iBody) {

  double dFracFe2O3Max; // Max Fe2O3 mass fraction (all oxygen in MO+ATM in
                        // Fe2O3)
  // double dFracFe2O3New;     // New Fe2O3 mass fraction
  double dUpperBound; // Upper boundary for root finding of fndOxygenEquiManAtm
  double dMagmOcFull; // Upper boundary for root (dUpperBound too large -> too
                      // slow)
  double dOxygenMassNew; // New oxygen mass in atmosphere
  double dOxygenMassMO;  // Oxygen mass in magma ocean

  // dFracFe2O3Max = body[iBody].dOxygenMassMOAtm *
  // 2*MOLWEIGHTFEO15/MOLWEIGHTOXYGEN / body[iBody].dMassMagmOcLiq ; dMagmOcFull
  // = body[iBody].dMassFracFeOIni  *   MOLWEIGHTFEO15/MOLWEIGHTFEO - 1e-15;
  // dUpperBound   = fmin(dFracFe2O3Max,dMagmOcFull);
  /* Don't use fugacity but oxidize all FeO to Fe2O3 before building up O in atm
   */
  if (body[iBody].bManSolid) {
    body[iBody].dFracFe2O3Man  = body[iBody].dFracFe2O3Man;
    body[iBody].dOxygenMassAtm = body[iBody].dOxygenMassMOAtm;
  } else if (body[iBody].bAllFeOOxid) {
    body[iBody].dFracFe2O3Man = body[iBody].dFracFe2O3Man;
    dOxygenMassMO             = body[iBody].dFracFe2O3Man * MOLWEIGHTOXYGEN /
                    (2 * MOLWEIGHTFEO15) *
                    (body[iBody].dMassMagmOcLiq + body[iBody].dMassMagmOcCry);
    body[iBody].dOxygenMassAtm = body[iBody].dOxygenMassMOAtm - dOxygenMassMO;
  } else {
    dFracFe2O3Max = body[iBody].dOxygenMassMOAtm * 2 * MOLWEIGHTFEO15 /
                    MOLWEIGHTOXYGEN /
                    (body[iBody].dMassMagmOcLiq + body[iBody].dMassMagmOcCry);
    dMagmOcFull = body[iBody].dMassFracFeOIni * MOLWEIGHTFEO15 / MOLWEIGHTFEO;
    dUpperBound = fmin(dFracFe2O3Max, dMagmOcFull);
    body[iBody].dFracFe2O3Man = fmax(body[iBody].dFracFe2O3Man, dUpperBound);
    // dOxygenMassMO = body[iBody].dFracFe2O3Man *
    // MOLWEIGHTOXYGEN/(2*MOLWEIGHTFEO15) * body[iBody].dMassMagmOcLiq;
    body[iBody].dOxygenMassAtm =
          0; // fmax(0,body[iBody].dOxygenMassMOAtm - dOxygenMassMO);
  }
  if (body[iBody].dOxygenMassAtm < 0) {
    body[iBody].dOxygenMassAtm = 0;
  }
}

/**
Calculation of melt fraction (average over magma ocean)
and kinematic viscosity (uppermost layer)
Function melt_fraction() in functions_rk.py

Calculation of the mantle heat flux
Function mantle_flux() in functions_rk.py

@param body A pointer to the current BODY instance
@param iBody The current BODY number
*/
void fndMeltFracMan(BODY *body, int iBody) {
  double daRadius[101];      // radius
  double daTrad[100];        // radius dep. temperature
  double daMelt_frac_r[100]; // radius dep. melt_frac
  double dMelt_frac_surf;    // melt frac at surface
  double dMelt_vol;          // melt volume
  double dA, dB;             // prefactors for linear solidus
  double dTsol;              // solidus temperature
  double dEta_a;             // liquid viscosity
  double dEta_b;             // solid viscosity
  double dSolidRadiusLocalLow,
        dSolidRadiusLocalHigh; // local solidus radius for
                               // high & low pressure
  int iItr;

  dMelt_vol = 0;

  for (iItr = 0; iItr < 101; iItr = iItr + 1) {
    daRadius[iItr] =
          iItr * (body[iBody].dRadius - body[iBody].dSolidRadius) / 100 +
          body[iBody].dSolidRadius;
  }

  for (iItr = 0; iItr < 100; iItr = iItr + 1) {
    if ((body[iBody].dRadius - daRadius[iItr]) < body[iBody].dTransDepthSol) {
      dA = ALOWPRESSURE;
      dB = BLOWPRESSURE;
    } else {
      dA = AHIGHPRESSURE;
      dB = BHIGHPRESSURE;
    }
    dTsol = dA * body[iBody].dManMeltDensity * body[iBody].dGravAccelSurf *
                  (body[iBody].dRadius - daRadius[iItr]) +
            dB;
    daTrad[iItr] = body[iBody].dPotTemp *
                   (1 + (THERMALEXPANCOEFF * body[iBody].dGravAccelSurf /
                         SILICATEHEATCAP) *
                              (body[iBody].dRadius - daRadius[iItr]));
    daMelt_frac_r[iItr] = (daTrad[iItr] - dTsol) / 600;
    if (daMelt_frac_r[iItr] > 1) {
      daMelt_frac_r[iItr] = 1; // melt fraction can't be larger than 1
    } else if (daMelt_frac_r[iItr] < 0) {
      daMelt_frac_r[iItr] = 0; // melt fraction can't be smaller than 0
    }
    dMelt_vol = dMelt_vol + daMelt_frac_r[iItr] * (pow(daRadius[iItr + 1], 3) -
                                                   pow(daRadius[iItr], 3));
  }

  // get kinematic viscosity at surface
  body[iBody].dMeltFracSurf = (body[iBody].dPotTemp - BLOWPRESSURE) / 600;
  if (body[iBody].dMeltFracSurf > 1) {
    body[iBody].dMeltFracSurf = 1;
  } else if (body[iBody].dMeltFracSurf < 0) {
    body[iBody].dMeltFracSurf = 0;
  }

  if (body[iBody].dMeltFracSurf > CRITMELTFRAC) {
    dEta_a =
          0.00024 * exp(4600 / (body[iBody].dPotTemp - 1000)) /
          pow((1 - (1 - body[iBody].dMeltFracSurf) / (1 - CRITMELTFRAC)), 2.5);
    dEta_b = DYNVISCSOLID * exp(ACTIVENERGY / (RGAS * body[iBody].dPotTemp));
    body[iBody].dKinemViscos =
          fmin(dEta_a, dEta_b) / body[iBody].dManMeltDensity;
  } else {
    body[iBody].dKinemViscos =
          DYNVISCSOLID * exp(ACTIVENERGY / (RGAS * body[iBody].dPotTemp)) /
          body[iBody].dManMeltDensity;
  }

  dSolidRadiusLocalLow =
        body[iBody].dRadius -
        ((BLOWPRESSURE - body[iBody].dPotTemp) /
         (body[iBody].dGravAccelSurf *
          (body[iBody].dPotTemp * THERMALEXPANCOEFF / SILICATEHEATCAP -
           ALOWPRESSURE * body[iBody].dManMeltDensity)));
  dSolidRadiusLocalHigh =
        body[iBody].dRadius -
        ((BHIGHPRESSURE - body[iBody].dPotTemp) /
         (body[iBody].dGravAccelSurf *
          (body[iBody].dPotTemp * THERMALEXPANCOEFF / SILICATEHEATCAP -
           AHIGHPRESSURE * body[iBody].dManMeltDensity)));
  body[iBody].dSolidRadiusLocal =
        fmin(dSolidRadiusLocalLow, dSolidRadiusLocalHigh);

  if (body[iBody].dSolidRadiusLocal < body[iBody].dCoreRadius) {
    body[iBody].dSolidRadiusLocal = body[iBody].dCoreRadius;
    body[iBody].dMeltFraction =
          dMelt_vol /
          (pow(body[iBody].dRadius, 3) - pow(body[iBody].dSolidRadiusLocal, 3));
  } else if (body[iBody].bManSolid) {
    body[iBody].dSolidRadiusLocal = body[iBody].dRadius;
    body[iBody].dMeltFraction     = 0;
  } else {
    body[iBody].dMeltFraction =
          dMelt_vol /
          (pow(body[iBody].dRadius, 3) - pow(body[iBody].dSolidRadiusLocal, 3));
  }

  if (body[iBody].dMeltFraction > 1) {
    body[iBody].dMeltFraction = 1;
  } else if (body[iBody].dMeltFraction < 0) {
    body[iBody].dMeltFraction = 0;
  }
  // END of melt_fraction(): returns Melt fraction and Kinematic viscosity
  // body[iBody].dManHeatFlux = THERMALCONDUC *
  // pow(fabs(body[iBody].dPotTemp-body[iBody].dSurfTemp),1.33) \
  //                            * pow((THERMALEXPANCOEFF*body[iBody].dGravAccelSurf/(CRITRAYLEIGHNO*THERMALDIFFUS*body[iBody].dKinemViscos)),0.33);
  //
  // if (body[iBody].dPotTemp < body[iBody].dSurfTemp) {
  //   body[iBody].dManHeatFlux = - body[iBody].dManHeatFlux;
  // }

  // if (body[iBody].dPotTemp > body[iBody].dSurfTemp) {
  //   body[iBody].dManHeatFlux = body[iBody].dManHeatFlux;
  // } else {
  //   body[iBody].dManHeatFlux = THERMALCONDUC * body[iBody].dPotTemp *
  //   THERMALEXPANCOEFF * body[iBody].dGravAccelSurf / SILICATEHEATCAP;
  // }
  // END of mantle_flux(): return mantle heat flux
}

/**
Calculation of the water and CO2 mass fraction in the magma ocean
Function water_fraction() in functions_rk.py

@param body A pointer to the current BODY instance
@param iBody The current BODY number
*/
void fndWaterFracMelt(BODY *body, int iBody) {
  double dMassMagmOcTot;   // total mass of magma ocean
  double dAveMolarMassAtm; // average molar mass of atmosphere

  // CO2:
  double dMassFracCO2Current, dCO2MassMO, dMassFracCO2Old, dLow, dUp;
  int iIteration;

  dMassMagmOcTot =
        4. / 3 * PI * body[iBody].dManMeltDensity *
        (pow(body[iBody].dRadius, 3) - pow(body[iBody].dSolidRadius, 3));
  body[iBody].dMassMagmOcLiq = body[iBody].dMeltFraction * dMassMagmOcTot;
  body[iBody].dMassMagmOcCry = (1 - body[iBody].dMeltFraction) * dMassMagmOcTot;

  if (body[iBody].bPlanetDesiccated) {
    body[iBody].dWaterFracMelt = 0;
  } else {
    if (fabs(fndWaterMassMOTime(body, 0, iBody)) < 1e-5) {
      body[iBody].dWaterFracMelt = 0;
    } else if (fabs(fndWaterMassMOTime(body, 1, iBody)) < 1e-5) {
      body[iBody].dWaterFracMelt = 1;
    } else {
      body[iBody].dWaterFracMelt =
            fndBisection(fndWaterMassMOTime, body, 0, 1, 1e-2, iBody);
    }
  }
  // Water pressure in atmosphere [Schaefer+ (2016), Eq. 19]
  body[iBody].dPressWaterAtm =
        pow((body[iBody].dWaterFracMelt / 3.44e-8), 1 / 0.74);

  // CO2 mass fraction in the magma ocean
  if (body[iBody].bCO2InAtmosphere) {

    // dMassFracCO2Current = body[iBody].dCO2MassMOAtm /
    // (body[iBody].dMassMagmOcCry + body[iBody].dMassMagmOcLiq);
    dMassFracCO2Current = body[iBody].dCO2FracMelt;
    if (dMassFracCO2Current <= 5e-4) {
      body[iBody].dCO2FracMelt        = dMassFracCO2Current;
      body[iBody].dPartialPressCO2Atm = 0;
      body[iBody].dPressCO2Atm        = 0;
      // if (body[iBody].dPressOxygenAtm + body[iBody].dPressWaterAtm > 1) {
      //   dAveMolarMassAtm = (MOLWEIGHTWATER * body[iBody].dPressWaterAtm +
      //   2*MOLWEIGHTOXYGEN *
      //   body[iBody].dPressOxygenAtm)/(body[iBody].dPressWaterAtm +
      //   body[iBody].dPressOxygenAtm);
      // }
    } else {
      dMassFracCO2Old = 0;
      iIteration      = 0;

      while (fabs(dMassFracCO2Current - dMassFracCO2Old) > 1e-7) {
        // when mass of CO2 in m.o. and atm become similar, loop will not
        // converge without following lines:
        if (dMassFracCO2Current < 0) {
          dMassFracCO2Current = dMassFracCO2Current * (-1);
        }

        if (iIteration > 0) {
          // dCO2MassMO = dMassFracCO2Current * (body[iBody].dMassMagmOcCry +
          // body[iBody].dMassMagmOcLiq); if (dCO2MassMO < 0.6 *
          // body[iBody].dCO2MassMOAtm) {
          if (dMassFracCO2Current > dMassFracCO2Old) {
            dMassFracCO2Current = 1.1 * dMassFracCO2Old;
          } else {
            dMassFracCO2Current = 0.9 * dMassFracCO2Old;
          }
          // }
        }

        // mass frac from last iteration
        dMassFracCO2Old = dMassFracCO2Current;

        // partial pressure of CO2 [from Elkins-Tanton (2008)]
        if (dMassFracCO2Current <= 5e-4) {
          body[iBody].dPartialPressCO2Atm = 0;
          body[iBody].dPressCO2Atm        = 0;
        } else {
          body[iBody].dPartialPressCO2Atm =
                pow(((100 * dMassFracCO2Current - 0.05) / 2.08e-4), (1 / 0.45));

          // get physical CO2 pressure with bisection root finder
          dLow = body[iBody].dPartialPressCO2Atm;
          dUp = body[iBody].dPartialPressCO2Atm * MOLWEIGHTCO2 / MOLWEIGHTWATER;
          if (fabs(fndPhysPressCO2(body, dLow, iBody)) < 1e-3 * dLow) {
            body[iBody].dPressCO2Atm = dLow;
          } else if (fabs(fndPhysPressCO2(body, dUp, iBody)) < 1e-3 * dLow) {
            body[iBody].dPressCO2Atm = dUp;
          } else {
            body[iBody].dPressCO2Atm = fndBisection(fndPhysPressCO2, body, dLow,
                                                    dUp, 1e-3 * dLow, iBody);
          }
        }
        dMassFracCO2Current =
              (body[iBody].dCO2MassMOAtm - body[iBody].dPressCO2Atm * 4 * PI *
                                                 pow(body[iBody].dRadius, 2) /
                                                 body[iBody].dGravAccelSurf) /
              (CO2PARTCOEFF * body[iBody].dMassMagmOcCry +
               body[iBody].dMassMagmOcLiq);

        if (dMassFracCO2Current < 0) {
          dMassFracCO2Current = 0;
        }
        // after 1000 iterations: use middle between F_old and F_new to avoid
        // endless loop
        if (iIteration > 1000) {
          dMassFracCO2Current = (dMassFracCO2Current + dMassFracCO2Old) / 2;
          if ((dMassFracCO2Current + dMassFracCO2Old) / dMassFracCO2Old < 0.1) {
            break;
          }
        }
        iIteration++;
      }
      body[iBody].dCO2FracMelt = dMassFracCO2Current;
      // get average molar mass
      // dAveMolarMassAtm = body[iBody].dPartialPressCO2Atm * MOLWEIGHTCO2 /
      // body[iBody].dPressCO2Atm;
    }
  } else { // No CO2 in atmosphere
    body[iBody].dPartialPressCO2Atm = 0;
    body[iBody].dPressCO2Atm        = 0;
  }

  if (body[iBody].dPressCO2Atm > 0) {
    dAveMolarMassAtm = body[iBody].dPartialPressCO2Atm * MOLWEIGHTCO2 /
                       body[iBody].dPressCO2Atm;
  } else if (body[iBody].dPressOxygenAtm > 1) {
    body[iBody].dPressCO2Atm = 0;
    dAveMolarMassAtm =
          (MOLWEIGHTWATER * body[iBody].dPressWaterAtm +
           2 * MOLWEIGHTOXYGEN * body[iBody].dPressOxygenAtm) /
          (body[iBody].dPressWaterAtm + body[iBody].dPressOxygenAtm);
  } else {
    body[iBody].dPressCO2Atm = 0;
    dAveMolarMassAtm         = MOLWEIGHTWATER;
  }

  body[iBody].dPartialPressWaterAtm =
        body[iBody].dPressWaterAtm * dAveMolarMassAtm / MOLWEIGHTWATER;
}

/* Auxs Props */
/* auxiliarie parameters/variables that need to be calculated in order to
 * calculate the primary variable (or at least simplify reading/understanding of
 * the code) calculated every quarter step for Runge-Kutta if needed in other
 * parts of the code, or to be printed: body[iBody]!!! otherwise it will be
 * deleted after the end of this equation
 */
void PropsAuxMagmOc(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                    int iBody) {
  double dCurrentTime     = evolve->dTime;
  double dCurrentTimeStep = evolve->dTimeStep;
  double dAveMolarMassAtm;

  // body[iBody].dSurfTemp = body[iBody].dPotTemp;

  /*
   * No negative masses!
   */
  if (body[iBody].dWaterMassMOAtm < 0) {
    body[iBody].dWaterMassMOAtm = 0;
  }
  if (body[iBody].dWaterMassSol < 0) {
    body[iBody].dWaterMassSol = 0;
  }
  if (body[iBody].dOxygenMassMOAtm < 0) {
    body[iBody].dOxygenMassMOAtm = 0;
  }
  if (body[iBody].dOxygenMassSol < 0) {
    body[iBody].dOxygenMassSol = 0;
  }

  /*
   * Melt fraction, kinematic viscosity, and mantle heat flux
   */
  if (!body[iBody].bManSolid) {
    fndMeltFracMan(body, iBody);
  } else {
    body[iBody].dMeltFraction = 0;
  }

  /*
   * Radiogenic heating: RadioHeat in W/kg
   */
  if (body[iBody].iRadioHeatModel == MAGMOC_SCHAEFER) {
    body[iBody].dRadioHeat =
          fndRadioHeatingEarth(body, iBody) *
          (4 / 3 * PI * body[iBody].dManMeltDensity *
           (pow(body[iBody].dRadius, 3) - pow(body[iBody].dCoreRadius, 3)));
  } else if (body[iBody].bRadheat) {
    body[iBody].dRadioHeat = fdRadPowerMan(update, iBody);
    // body[iBody].dRadioHeat = dPowerRadio /
    // (4/3*PI*body[iBody].dManMeltDensity*(pow(body[iBody].dRadius,3)-pow(body[iBody].dCoreRadius,3)));
    // // add here RADHEAT
  } else {
    body[iBody].dRadioHeat = 0;
  }

  /*
   * Tidal heating: TidalHeat in W/kg
   */
  if (body[iBody].bEqtide) {
    body[iBody].dTidalHeat = fdTidePower(body, iBody, evolve->iEqtideModel);
    // body[iBody].dTidalHeat = dTidalPower /
    // (4/3*PI*body[iBody].dManMeltDensity*(pow(body[iBody].dRadius,3)-pow(body[iBody].dCoreRadius,3)));
    // // add here RADHEAT
  } else {
    body[iBody].dTidalHeat = 0;
  }

  /*
   * Water fraction in magma ocean and water pressure in atmosphere
   */
  if (!body[iBody].bManSolid) {
    fndWaterFracMelt(body, iBody);
  } else {
    body[iBody].dPressWaterAtm = body[iBody].dWaterMassMOAtm *
                                 body[iBody].dGravAccelSurf /
                                 (4 * PI * pow(body[iBody].dRadius, 2));
    body[iBody].dPressCO2Atm = body[iBody].dCO2MassMOAtm *
                               body[iBody].dGravAccelSurf /
                               (4 * PI * pow(body[iBody].dRadius, 2));
    body[iBody].dPressOxygenAtm = body[iBody].dOxygenMassMOAtm *
                                  body[iBody].dGravAccelSurf /
                                  (4 * PI * pow(body[iBody].dRadius, 2));
    if ((body[iBody].dPressWaterAtm + body[iBody].dPressCO2Atm +
         body[iBody].dPressOxygenAtm) > 1) {
      dAveMolarMassAtm =
            (MOLWEIGHTWATER * body[iBody].dPressWaterAtm +
             MOLWEIGHTCO2 * body[iBody].dPressCO2Atm +
             2 * MOLWEIGHTOXYGEN * body[iBody].dPressOxygenAtm) /
            (body[iBody].dPressWaterAtm + body[iBody].dPressCO2Atm +
             body[iBody].dPressOxygenAtm);
      body[iBody].dPartialPressWaterAtm =
            body[iBody].dPressWaterAtm * dAveMolarMassAtm / MOLWEIGHTWATER;
      body[iBody].dPartialPressCO2Atm =
            body[iBody].dPressCO2Atm * dAveMolarMassAtm / MOLWEIGHTCO2;
    } else {
      body[iBody].dPartialPressWaterAtm = 0;
      body[iBody].dPartialPressCO2Atm   = 0;
    }
  }

  /*
   * Bolometric flux from the star (at the orbit of the planet)
   */
  double dBolFlux; // bolometric flux from the star

  if (!body[0]
             .bStellar) { //|| body[iBody].iRadioHeatModel == MAGMOC_SCHAEFER) {
    dBolFlux = fndBolFluxSchaefer(body, iBody);
  } else {
    dBolFlux = body[0].dLuminosity /
               (4 * PI * body[iBody].dSemi * body[iBody].dSemi);
  }

  /*
   * Net flux leaving the atmosphere
   */

  body[iBody].dEffTempAtm =
        pow((dBolFlux * (1 - body[iBody].dAlbedo) / (4 * SIGMA)), 0.25);

  if (body[iBody].iMagmOcAtmModel == MAGMOC_GREY ||
      body[iBody].dPressWaterAtm <= PRESSWATERMIN) {
    body[iBody].dNetFluxAtmo = fndNetFluxAtmGrey(body, iBody);
  } else if (body[iBody].iMagmOcAtmModel == MAGMOC_PETIT) {
    body[iBody].dNetFluxAtmo =
          fndNetFluxAtmPetit(body, dCurrentTime / 3.15576e7, iBody);
  }

  /*
   * Mass fraction of Fe2O3 in the mantle and mass of oxygen in the atmosphere
   */
  if (body[iBody].bManSolid) {
    body[iBody].dOxygenMassAtm = body[iBody].dOxygenMassMOAtm;
  } else {
    fndFe2O3MassFracOxyMass(body, iBody);
    /* Factor in the derivative of Tpot and Rsol */
    body[iBody].dFactorDerivative =
          SILICATEHEATCAP *
          (body[iBody].dPrefactorB * THERMALEXPANCOEFF -
           body[iBody].dPrefactorA * body[iBody].dManMeltDensity *
                 SILICATEHEATCAP) /
          (body[iBody].dGravAccelSurf *
           pow((body[iBody].dPrefactorA * body[iBody].dManMeltDensity *
                      SILICATEHEATCAP -
                THERMALEXPANCOEFF * body[iBody].dPotTemp),
               2));
  }

  body[iBody].dPressOxygenAtm = body[iBody].dOxygenMassAtm *
                                body[iBody].dGravAccelSurf /
                                (4 * PI * pow(body[iBody].dRadius, 2));
}


/* Only updated every full step. Use for check of different behaviors and force
 * parameters to a value */
void fnForceBehaviorMagmOc(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                           SYSTEM *system, UPDATE *update,
                           fnUpdateVariable ***fnUpdate, int iBody,
                           int iModule) {

  /* Mantle starts to solidify */
  if (!body[iBody].bManStartSol &&
      (body[iBody].dSolidRadius - body[iBody].dCoreRadius) > 1e-5) {
    body[iBody].bManStartSol = 1;
    if (io->iVerbose >= VERBPROG) {
      printf("%s's mantle starts to solidify after %f years. \n",
             body[iBody].cName, evolve->dTime / YEARSEC);
    }
  }

  /* Switch from high to low pressure regime of the solidus */
  if (!body[iBody].bLowPressSol &&
      (body[iBody].dRadius - body[iBody].dSolidRadius) <
            body[iBody].dTransDepthSol) {
    body[iBody].dPrefactorA  = ALOWPRESSURE;
    body[iBody].dPrefactorB  = BLOWPRESSURE;
    body[iBody].bLowPressSol = 1;
    if (io->iVerbose >= VERBPROG) {
      printf("%s: Switch to low-pressure treatment of solidus after %f years. "
             "\n",
             body[iBody].cName, evolve->dTime / YEARSEC);
    }
  }

  /* All FeO in mantle oxidized to Fe2O3 */
  if ((!body[iBody].bAllFeOOxid) &&
      (body[iBody].dFracFe2O3Man >=
       (body[iBody].dMassFracFeOIni * MOLWEIGHTFEO15 / MOLWEIGHTFEO - 1e-14))) {
    body[iBody].bAllFeOOxid = 1;
    if (io->iVerbose >= VERBPROG) {
      printf("%s: All FeO in magma ocean oxidized to Fe2O3 after %f years. \n",
             body[iBody].cName, evolve->dTime / YEARSEC);
    }
  }

  /* Mantle solidified */
  if ((!body[iBody].bManSolid) &&
      (body[iBody].dSolidRadius >= (0.9999 * body[iBody].dRadius))) {
    body[iBody].bManSolid = 1;
    // body[iBody].dManMeltDensity = 4200;
    body[iBody].dSolidRadius = body[iBody].dRadius;
    /*
     * When mantle solidified:
     * Stop updating PotTemp, SurfTemp, SolidRadius, WaterMassSol, OxygenMassSol
     * But continue to update WaterMassMOAtm, OxygenMassMOAtm,
     * HydrogenMassSpace, OxygenMassSpace
     */
    SetDerivTiny(fnUpdate, iBody, update[iBody].iPotTemp,
                 update[iBody].iPotTempMagmOc);
    SetDerivTiny(fnUpdate, iBody, update[iBody].iSurfTemp,
                 update[iBody].iSurfTempMagmOc);
    SetDerivTiny(fnUpdate, iBody, update[iBody].iSolidRadius,
                 update[iBody].iSolidRadiusMagmOc);
    SetDerivTiny(fnUpdate, iBody, update[iBody].iWaterMassSol,
                 update[iBody].iWaterMassSolMagmOc);
    SetDerivTiny(fnUpdate, iBody, update[iBody].iOxygenMassSol,
                 update[iBody].iOxygenMassSolMagmOc);
    SetDerivTiny(fnUpdate, iBody, update[iBody].iCO2MassSol,
                 update[iBody].iCO2MassSolMagmOc);
    SetDerivTiny(fnUpdate, iBody, update[iBody].iCO2MassMOAtm,
                 update[iBody].iCO2MassMOAtmMagmOc);

    if (io->iVerbose >= VERBPROG) {
      printf("%s's mantle solidified after %f years. \n", body[iBody].cName,
             evolve->dTime / YEARSEC);
    }
  }

  /* Planet desiccated = no water left in atmosphere (less than 10 mbar) */
  if ((!body[iBody].bPlanetDesiccated) &&
      (body[iBody].dPressWaterAtm <= PRESSWATERMIN)) {
    body[iBody].bPlanetDesiccated = 1;
    body[iBody].dWaterMassEsc     = 0;
    body[iBody].dOxygenMassEsc    = 0;
    SetDerivTiny(fnUpdate, iBody, update[iBody].iWaterMassMOAtm,
                 update[iBody].iWaterMassMOAtmMagmOc);
    if (io->iVerbose >= VERBPROG) {
      printf("%s's atmosphere desiccated after %f years. \n", body[iBody].cName,
             evolve->dTime / YEARSEC);
    }
  }

  /* Treat mantle as solidified when melt fraction at surface smaller than 0.4
   * Set mantle completely solid & water into atm.
   */
  // /* When planet desiccated and T_surf below 1000K treat mantle as solidified
  // */ if ((!body[iBody].bManQuasiSol) && (body[iBody].bPlanetDesiccated) &&
  // (body[iBody].dSurfTemp <= 1000)) {
  if (body[iBody].bOptManQuasiSol && (!body[iBody].bManQuasiSol) &&
      (body[iBody].dMeltFracSurf < CRITMELTFRAC)) {
    double dOxygenMassMO;
    double dDeltaWaterMass;
    double dDeltaCO2Mass;

    body[iBody].bManQuasiSol = 1;

    // body[iBody].dManMeltDensity = 4200;
    body[iBody].dSolidRadius = body[iBody].dRadius;

    dDeltaWaterMass = body[iBody].dWaterPartCoeff * body[iBody].dWaterFracMelt *
                      (body[iBody].dMassMagmOcCry + body[iBody].dMassMagmOcLiq);
    // dDeltaWaterMass =
    // WATERPARTCOEFF*body[iBody].dWaterFracMelt*(body[iBody].dMassMagmOcCry +
    // body[iBody].dMassMagmOcLiq);
    body[iBody].dWaterMassSol   = body[iBody].dWaterMassSol + dDeltaWaterMass;
    body[iBody].dWaterMassMOAtm = body[iBody].dWaterMassMOAtm - dDeltaWaterMass;

    dDeltaCO2Mass = CO2PARTCOEFF * body[iBody].dCO2FracMelt *
                    (body[iBody].dMassMagmOcCry + body[iBody].dMassMagmOcLiq);
    body[iBody].dCO2MassSol   = body[iBody].dCO2MassSol + dDeltaCO2Mass;
    body[iBody].dCO2MassMOAtm = body[iBody].dCO2MassMOAtm - dDeltaCO2Mass;

    if (!body[iBody].bAllFeOOxid) {
      body[iBody].dOxygenMassAtm = 0;
      body[iBody].dOxygenMassSol =
            body[iBody].dOxygenMassSol + body[iBody].dOxygenMassMOAtm;
    } else {
      dOxygenMassMO = body[iBody].dFracFe2O3Man * MOLWEIGHTOXYGEN /
                      (2 * MOLWEIGHTFEO15) *
                      (body[iBody].dMassMagmOcLiq + body[iBody].dMassMagmOcCry);
      body[iBody].dOxygenMassSol = body[iBody].dOxygenMassSol + dOxygenMassMO;
      body[iBody].dOxygenMassAtm = body[iBody].dOxygenMassMOAtm - dOxygenMassMO;
    }

    body[iBody].dPressWaterAtm = body[iBody].dWaterMassMOAtm *
                                 body[iBody].dGravAccelSurf /
                                 (4 * PI * pow(body[iBody].dRadius, 2));
    body[iBody].dPressCO2Atm = body[iBody].dCO2MassMOAtm *
                               body[iBody].dGravAccelSurf /
                               (4 * PI * pow(body[iBody].dRadius, 2));
    body[iBody].dPressOxygenAtm = body[iBody].dOxygenMassAtm *
                                  body[iBody].dGravAccelSurf /
                                  (4 * PI * pow(body[iBody].dRadius, 2));


    /*
     * When mantle solidified:
     * Stop updating PotTemp, SurfTemp, SolidRadius, WaterMassSol, OxygenMassSol
     * But continue to update WaterMassMOAtm, OxygenMassMOAtm,
     * HydrogenMassSpace, OxygenMassSpace
     */
    // SetDerivTiny(fnUpdate,iBody,update[iBody].iPotTemp
    // ,update[iBody].iPotTempMagmOc      );
    // SetDerivTiny(fnUpdate,iBody,update[iBody].iSurfTemp
    // ,update[iBody].iSurfTempMagmOc     );
    SetDerivTiny(fnUpdate, iBody, update[iBody].iSolidRadius,
                 update[iBody].iSolidRadiusMagmOc);
    SetDerivTiny(fnUpdate, iBody, update[iBody].iWaterMassSol,
                 update[iBody].iWaterMassSolMagmOc);
    SetDerivTiny(fnUpdate, iBody, update[iBody].iOxygenMassSol,
                 update[iBody].iOxygenMassSolMagmOc);
    SetDerivTiny(fnUpdate, iBody, update[iBody].iCO2MassSol,
                 update[iBody].iCO2MassSolMagmOc);
    SetDerivTiny(fnUpdate, iBody, update[iBody].iCO2MassMOAtm,
                 update[iBody].iCO2MassMOAtmMagmOc);


    if (io->iVerbose >= VERBPROG) {
      printf("Surface melt fraction of %s's smaller than 0.4 after %f years - "
             "mantle set to solidified. \n",
             body[iBody].cName, evolve->dTime / YEARSEC);
      // printf("%s's atmosphere desiccated & surface temperature below 1000K
      // after %f years. \n",body[iBody].cName,evolve->dTime/YEARSEC);
    }
  } else if ((!body[iBody].bOptManQuasiSol) && (!body[iBody].bManQuasiSol) &&
             (body[iBody].dMeltFracSurf < CRITMELTFRAC)) {
    body[iBody].bManQuasiSol = 1;
    if (io->iVerbose >= VERBPROG) {
      printf("Surface melt fraction of %s's smaller than 0.4 after %f years \n",
             body[iBody].cName, evolve->dTime / YEARSEC);
    }
  }

  /* When planet enters habitable zone atmospheric escape stops */
  if ((!body[iBody].bEscapeStop) &&
      (body[iBody].dHZInnerEdge <= body[iBody].dSemi)) {
    SetDerivTiny(fnUpdate, iBody, update[iBody].iOxygenMassSpace,
                 update[iBody].iOxygenMassSpaceMagmOc);
    SetDerivTiny(fnUpdate, iBody, update[iBody].iHydrogenMassSpace,
                 update[iBody].iHydrogenMassSpaceMagmOc);
    body[iBody].bEscapeStop = 1;
    if (io->iVerbose >= VERBPROG) {
      printf("%s enters habitable zone after %f years. \n", body[iBody].cName,
             evolve->dTime / YEARSEC);
    }
  }

  /*
   * For multiple planet systems
   */

  /* Mantle solidified or planet desiccated with T_surf below 1000K */
  if (!body[iBody].bMagmOcHaltSolid) {
    if (body[iBody].bManQuasiSol || body[iBody].bManSolid) {
      body[iBody].bMagmOcHaltSolid = 1;
    }
  }

  /* Planet desiccated or atmospheric escape stopped due to entry into HZ */
  if (!body[iBody].bMagmOcHaltDesicc) {
    if (body[iBody].bPlanetDesiccated || !body[iBody].bRunaway) {
      body[iBody].bMagmOcHaltDesicc = 1;
    }
  }

  /* No CO2 */
  if (!body[iBody].bCO2InAtmosphere) {
    /*
     * When no CO2 included:
     * Stop updating CO2MassMOAtm, CO2MassSol
     */
    SetDerivTiny(fnUpdate, iBody, update[iBody].iCO2MassMOAtm,
                 update[iBody].iCO2MassMOAtmMagmOc);
    SetDerivTiny(fnUpdate, iBody, update[iBody].iCO2MassSol,
                 update[iBody].iCO2MassSolMagmOc);
  }
}

void MagmOcExit(FILES *files, char cSpecies[16], int iFile) {
}

void VerifyPotTemp(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                   int iBody) {
  update[iBody].iaType[update[iBody].iPotTemp][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iPotTemp][0] = 1;
  update[iBody].iaBody[update[iBody].iPotTemp][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iPotTemp][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iPotTemp][0][0] = iBody;

  update[iBody].pdDPotTemp =
        &update[iBody].daDerivProc[update[iBody].iPotTemp][0];
}

void VerifySurfTemp(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                    int iBody) {
  update[iBody].iaType[update[iBody].iSurfTemp][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iSurfTemp][0] = 1;
  update[iBody].iaBody[update[iBody].iSurfTemp][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iSurfTemp][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iSurfTemp][0][0] = iBody;

  update[iBody].pdDSurfTemp =
        &update[iBody].daDerivProc[update[iBody].iSurfTemp][0];
}

void VerifySolidRadius(BODY *body, OPTIONS *options, UPDATE *update,
                       double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iSolidRadius][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iSolidRadius][0] = 1;
  update[iBody].iaBody[update[iBody].iSolidRadius][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iSolidRadius][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iSolidRadius][0][0] = iBody;

  update[iBody].pdDSolidRadius =
        &update[iBody].daDerivProc[update[iBody].iSolidRadius][0];
}

void VerifyWaterMassMOAtm(BODY *body, OPTIONS *options, UPDATE *update,
                          double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iWaterMassMOAtm][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iWaterMassMOAtm][0] = 1;
  update[iBody].iaBody[update[iBody].iWaterMassMOAtm][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iWaterMassMOAtm][0] *
               sizeof(int));
  update[iBody].iaBody[update[iBody].iWaterMassMOAtm][0][0] = iBody;

  update[iBody].pdDWaterMassMOAtm =
        &update[iBody].daDerivProc[update[iBody].iWaterMassMOAtm][0];
}

void VerifyWaterMassSol(BODY *body, OPTIONS *options, UPDATE *update,
                        double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iWaterMassSol][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iWaterMassSol][0] = 1;
  update[iBody].iaBody[update[iBody].iWaterMassSol][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iWaterMassSol][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iWaterMassSol][0][0] = iBody;

  update[iBody].pdDWaterMassSol =
        &update[iBody].daDerivProc[update[iBody].iWaterMassSol][0];
}

void VerifyCO2MassMOAtm(BODY *body, OPTIONS *options, UPDATE *update,
                        double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iCO2MassMOAtm][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iCO2MassMOAtm][0] = 1;
  update[iBody].iaBody[update[iBody].iCO2MassMOAtm][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iCO2MassMOAtm][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iCO2MassMOAtm][0][0] = iBody;

  update[iBody].pdDCO2MassMOAtm =
        &update[iBody].daDerivProc[update[iBody].iCO2MassMOAtm][0];
}

void VerifyCO2MassSol(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                      int iBody) {
  update[iBody].iaType[update[iBody].iCO2MassSol][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iCO2MassSol][0] = 1;
  update[iBody].iaBody[update[iBody].iCO2MassSol][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iCO2MassSol][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iCO2MassSol][0][0] = iBody;

  update[iBody].pdDCO2MassSol =
        &update[iBody].daDerivProc[update[iBody].iCO2MassSol][0];
}


void VerifyOxygenMassMOAtm(BODY *body, OPTIONS *options, UPDATE *update,
                           double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iOxygenMassMOAtm][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iOxygenMassMOAtm][0] = 1;
  update[iBody].iaBody[update[iBody].iOxygenMassMOAtm][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iOxygenMassMOAtm][0] *
               sizeof(int));
  update[iBody].iaBody[update[iBody].iOxygenMassMOAtm][0][0] = iBody;

  update[iBody].pdDOxygenMassMOAtm =
        &update[iBody].daDerivProc[update[iBody].iOxygenMassMOAtm][0];
}

void VerifyOxygenMassSol(BODY *body, OPTIONS *options, UPDATE *update,
                         double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iOxygenMassSol][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iOxygenMassSol][0] = 1;
  update[iBody].iaBody[update[iBody].iOxygenMassSol][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iOxygenMassSol][0] *
               sizeof(int));
  update[iBody].iaBody[update[iBody].iOxygenMassSol][0][0] = iBody;

  update[iBody].pdDOxygenMassSol =
        &update[iBody].daDerivProc[update[iBody].iOxygenMassSol][0];
}

void VerifyHydrogenMassSpace(BODY *body, OPTIONS *options, UPDATE *update,
                             double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iHydrogenMassSpace][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iHydrogenMassSpace][0] = 1;
  update[iBody].iaBody[update[iBody].iHydrogenMassSpace][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iHydrogenMassSpace][0] *
               sizeof(int));
  update[iBody].iaBody[update[iBody].iHydrogenMassSpace][0][0] = iBody;

  update[iBody].pdDHydrogenMassSpace =
        &update[iBody].daDerivProc[update[iBody].iHydrogenMassSpace][0];
}

void VerifyOxygenMassSpace(BODY *body, OPTIONS *options, UPDATE *update,
                           double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iOxygenMassSpace][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iOxygenMassSpace][0] = 1;
  update[iBody].iaBody[update[iBody].iOxygenMassSpace][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iOxygenMassSpace][0] *
               sizeof(int));
  update[iBody].iaBody[update[iBody].iOxygenMassSpace][0][0] = iBody;

  update[iBody].pdDOxygenMassSpace =
        &update[iBody].daDerivProc[update[iBody].iOxygenMassSpace][0];
}

// assign a derivativ to the primary variable
void AssignMagmOcDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody) {
  fnUpdate[iBody][update[iBody].iPotTemp][0]           = &fdDPotTemp;
  fnUpdate[iBody][update[iBody].iSurfTemp][0]          = &fdDSurfTemp;
  fnUpdate[iBody][update[iBody].iSolidRadius][0]       = &fdDSolidRadius;
  fnUpdate[iBody][update[iBody].iWaterMassMOAtm][0]    = &fdDWaterMassMOAtm;
  fnUpdate[iBody][update[iBody].iWaterMassSol][0]      = &fdDWaterMassSol;
  fnUpdate[iBody][update[iBody].iCO2MassMOAtm][0]      = &fdDCO2MassMOAtm;
  fnUpdate[iBody][update[iBody].iCO2MassSol][0]        = &fdDCO2MassSol;
  fnUpdate[iBody][update[iBody].iOxygenMassMOAtm][0]   = &fdDOxygenMassMOAtm;
  fnUpdate[iBody][update[iBody].iOxygenMassSol][0]     = &fdDOxygenMassSol;
  fnUpdate[iBody][update[iBody].iHydrogenMassSpace][0] = &fdDHydrogenMassSpace;
  fnUpdate[iBody][update[iBody].iOxygenMassSpace][0]   = &fdDOxygenMassSpace;
  /* HERE all derivatives*/
}

// derivative for minimal change??
void NullMagmOcDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                           fnUpdateVariable ***fnUpdate, int iBody) {
  fnUpdate[iBody][update[iBody].iPotTemp][0]           = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iSurfTemp][0]          = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iSolidRadius][0]       = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iWaterMassMOAtm][0]    = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iWaterMassSol][0]      = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iCO2MassMOAtm][0]      = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iCO2MassSol][0]        = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iOxygenMassMOAtm][0]   = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iOxygenMassSol][0]     = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iHydrogenMassSpace][0] = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iOxygenMassSpace][0]   = &fndUpdateFunctionTiny;
}

// call steps to execute next time step??
void VerifyMagmOc(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  OUTPUT *output, SYSTEM *system, UPDATE *update, int iBody,
                  int iModule) {
  VerifyPotTemp(body, options, update, body[iBody].dAge, iBody);
  VerifySurfTemp(body, options, update, body[iBody].dAge, iBody);
  VerifySolidRadius(body, options, update, body[iBody].dAge, iBody);
  VerifyWaterMassMOAtm(body, options, update, body[iBody].dAge, iBody);
  VerifyWaterMassSol(body, options, update, body[iBody].dAge, iBody);
  VerifyCO2MassMOAtm(body, options, update, body[iBody].dAge, iBody);
  VerifyCO2MassSol(body, options, update, body[iBody].dAge, iBody);
  VerifyOxygenMassMOAtm(body, options, update, body[iBody].dAge, iBody);
  VerifyOxygenMassSol(body, options, update, body[iBody].dAge, iBody);
  VerifyHydrogenMassSpace(body, options, update, body[iBody].dAge, iBody);
  VerifyOxygenMassSpace(body, options, update, body[iBody].dAge, iBody);

  control->fnForceBehavior[iBody][iModule]   = &fnForceBehaviorMagmOc;
  control->fnPropsAux[iBody][iModule]        = &PropsAuxMagmOc;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyMagmOc;
}
/**************** MAGMOC update ****************/

// ??
void InitializeUpdateMagmOc(BODY *body, UPDATE *update, int iBody) {
  if (iBody >= 0) {
    if (update[iBody].iNumPotTemp == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumPotTemp++;

    if (update[iBody].iNumSurfTemp == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumSurfTemp++;

    if (update[iBody].iNumSolidRadius == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumSolidRadius++;

    if (update[iBody].iNumWaterMassMOAtm == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumWaterMassMOAtm++;

    if (update[iBody].iNumWaterMassSol == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumWaterMassSol++;

    if (update[iBody].iNumCO2MassMOAtm == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumCO2MassMOAtm++;

    if (update[iBody].iNumCO2MassSol == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumCO2MassSol++;

    if (update[iBody].iNumOxygenMassMOAtm == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumOxygenMassMOAtm++;

    if (update[iBody].iNumOxygenMassSol == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumOxygenMassSol++;

    if (update[iBody].iNumOxygenMassSpace == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumOxygenMassSpace++;

    if (update[iBody].iNumHydrogenMassSpace == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumHydrogenMassSpace++;
  }
}

/***************** MAGMOC Halts *****************/

/*
 * Checks if mantle soldified
 */
int fbHaltMantleSolidified(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                           UPDATE *update, fnUpdateVariable ***fnUpdate,
                           int iBody) {
  if (body[iBody].bManSolid) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's mantle completely solidified after %f years. \n",
             body[iBody].cName, evolve->dTime / YEARSEC);
    }
    return 1;
  }
  return 0;
}

int fbHaltMantleMeltFracLow(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                            UPDATE *update, fnUpdateVariable ***fnUpdate,
                            int iBody) {
  if (body[iBody].bManQuasiSol) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's mantle mostly solidified after %f years. \n",
             body[iBody].cName, evolve->dTime / YEARSEC);
    }
    return 1;
  }
  return 0;
}

int fbHaltAtmDesiSurfCool(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                          UPDATE *update, fnUpdateVariable ***fnUpdate,
                          int iBody) {
  if (body[iBody].bPlanetDesiccated) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's atmosphere desiccated after %f years. \n",
             body[iBody].cName, evolve->dTime / YEARSEC);
    }
    return 1;
  }
  return 0;
}

int fbHaltEnterHabZone(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                       UPDATE *update, fnUpdateVariable ***fnUpdate,
                       int iBody) {
  if (body[iBody].bEscapeStop) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s enters habitable zone after %f years. \n",
             body[iBody].cName, evolve->dTime / YEARSEC);
    }
    return 1;
  }
  return 0;
}

int fbHaltAllPlanetsSolid(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                          UPDATE *update, fnUpdateVariable ***fnUpdate,
                          int iBody) {
  double dCountSolid = 0;
  int iBodyTemp;

  if (body[iBody].bMagmOcHaltSolid) {
    for (iBodyTemp = 1; iBodyTemp < evolve->iNumBodies; iBodyTemp++) {
      if (body[iBodyTemp].bMagmOcHaltSolid) {
        dCountSolid++;
      }
    }
    if (dCountSolid == (evolve->iNumBodies - 1)) {
      if (io->iVerbose >= VERBPROG) {
        printf("HALT: All planets solidified after %f years. \n",
               evolve->dTime / YEARSEC);
      }
      return 1;
    }
  }
  return 0;
}

int fbHaltAllPlanetsDesicc(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                           UPDATE *update, fnUpdateVariable ***fnUpdate,
                           int iBody) {
  double dCountSolid = 0;
  int iBodyTemp;

  if (body[iBody].bMagmOcHaltDesicc) {
    for (iBodyTemp = 1; iBodyTemp < evolve->iNumBodies; iBodyTemp++) {
      if (body[iBodyTemp].bMagmOcHaltDesicc) {
        dCountSolid++;
      }
    }
    if (dCountSolid == (evolve->iNumBodies - 1)) {
      if (io->iVerbose >= VERBPROG) {
        printf("HALT: All planets desiccated or reached HZ after %f years. \n",
               evolve->dTime / YEARSEC);
      }
      return 1;
    }
  }
  return 0;
}

void CountHaltsMagmOc(HALT *halt, int *iNumHalts) {
  if (halt->bHaltMantleSolidified) {
    (*iNumHalts)++;
  }
  if (halt->bHaltMantleMeltFracLow) {
    (*iNumHalts)++;
  }
  if (halt->bHaltAtmDesiSurfCool) {
    (*iNumHalts)++;
  }
  if (halt->bHaltEnterHabZone) {
    (*iNumHalts)++;
  }
  if (halt->bHaltAllPlanetsSolid) {
    (*iNumHalts)++;
  }
  if (halt->bHaltAllPlanetsDesicc) {
    (*iNumHalts)++;
  }
}

void VerifyHaltMagmOc(BODY *body, CONTROL *control, OPTIONS *options, int iBody,
                      int *iHalt) {
  if (control->Halt[iBody].bHaltMantleSolidified) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMantleSolidified;
  }
  if (control->Halt[iBody].bHaltMantleMeltFracLow) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMantleMeltFracLow;
  }
  if (control->Halt[iBody].bHaltAtmDesiSurfCool) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltAtmDesiSurfCool;
  }
  if (control->Halt[iBody].bHaltEnterHabZone) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltEnterHabZone;
  }
  if (control->Halt[iBody].bHaltAllPlanetsSolid) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltAllPlanetsSolid;
  }
  if (control->Halt[iBody].bHaltAllPlanetsDesicc) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltAllPlanetsDesicc;
  }
}

/************* MAGMOC Outputs ******************/
// similar to read input

/* NOTE: If you write a new Write subroutine here you need to add the associate
   block of initialization in InitializeOutputMagmOc below */

void WritePotTemp(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char **cUnit) {
  *dTmp = body[iBody].dPotTemp;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsTemp(units->iTemp, 1, 0);
    fsUnitsTemp(units->iTemp, cUnit);
  }
}

void WriteSurfTemp(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char **cUnit) {
  *dTmp = body[iBody].dSurfTemp;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsTemp(units->iTemp, 1, 0);
    fsUnitsTemp(units->iTemp, cUnit);
  }
}

void WriteSolidRadius(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dSolidRadius;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

void WriteWaterMassMOAtm(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dWaterMassMOAtm;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

void WriteWaterMassSol(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dWaterMassSol;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

void WriteCO2MassMOAtm(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dCO2MassMOAtm;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

void WriteCO2MassSol(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dCO2MassSol;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

void WriteOxygenMassMOAtm(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dOxygenMassMOAtm;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

void WriteOxygenMassSol(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dOxygenMassSol;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

void WritePressWaterAtm(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dPressWaterAtm;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
  }
}

void WritePressCO2Atm(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dPressCO2Atm;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
  }
}

void WritePressOxygenAtm(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dPressOxygenAtm;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
  }
}

void WriteHydrogenMassSpace(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dHydrogenMassSpace;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

void WriteOxygenMassSpace(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dOxygenMassSpace;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

void WriteFracFe2O3Man(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dFracFe2O3Man;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
  }
}

void WriteNetFluxAtmo(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dNetFluxAtmo;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
  }
}

void WriteWaterFracMelt(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dWaterFracMelt;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
  }
}

void WriteCO2FracMelt(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dCO2FracMelt;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
  }
}

void WriteRadioPower(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char **cUnit) {
  *dTmp =
        body[iBody]
              .dRadioHeat; //* (4/3*PI*body[iBody].dManMeltDensity*(pow(body[iBody].dRadius,3)-pow(body[iBody].dCoreRadius,3)));
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

void WriteTidalPower(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char **cUnit) {
  *dTmp =
        body[iBody]
              .dTidalHeat; // * (4/3*PI*body[iBody].dManMeltDensity*(pow(body[iBody].dRadius,3)-pow(body[iBody].dCoreRadius,3)));
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

void WriteHZInnerEdge(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dHZInnerEdge;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

void WriteMeltFraction(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dMeltFraction;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
  }
}

// similar to initialize options
void InitializeOutputMagmOc(OUTPUT *output, fnWriteOutput fnWrite[]) {

  fvFormattedString(&output[OUT_POTTEMP].cName, "PotTemp");
  fvFormattedString(&output[OUT_POTTEMP].cDescr, "Potential temperature magma ocean");
  fvFormattedString(&output[OUT_POTTEMP].cNeg, "K");
  output[OUT_POTTEMP].bNeg = 1;
  output[OUT_POTTEMP].dNeg =
        1; // division factor to get from SI to desired unit
  output[OUT_POTTEMP].iNum       = 1;
  output[OUT_POTTEMP].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_POTTEMP]           = &WritePotTemp;

  fvFormattedString(&output[OUT_SURFTEMP].cName, "SurfTemp");
  fvFormattedString(&output[OUT_SURFTEMP].cDescr, "Surface temperature magma ocean");
  fvFormattedString(&output[OUT_SURFTEMP].cNeg, "K");
  output[OUT_SURFTEMP].bNeg = 1;
  output[OUT_SURFTEMP].dNeg =
        1; // division factor to get from SI to desired unit
  output[OUT_SURFTEMP].iNum       = 1;
  output[OUT_SURFTEMP].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_SURFTEMP]           = &WriteSurfTemp;

  fvFormattedString(&output[OUT_SOLIDRADIUS].cName, "SolidRadius");
  fvFormattedString(&output[OUT_SOLIDRADIUS].cDescr,
          "Solidification radius of the mantle");
  fvFormattedString(&output[OUT_SOLIDRADIUS].cNeg, "Rearth");
  output[OUT_SOLIDRADIUS].bNeg = 1;
  output[OUT_SOLIDRADIUS].dNeg =
        1 / REARTH; // division factor to get from SI to desired unit
  output[OUT_SOLIDRADIUS].iNum       = 1;
  output[OUT_SOLIDRADIUS].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_SOLIDRADIUS]           = &WriteSolidRadius;

  fvFormattedString(&output[OUT_WATERMASSMOATM].cName, "WaterMassMOAtm");
  fvFormattedString(&output[OUT_WATERMASSMOATM].cDescr,
          "Water mass in magma ocean and atmosphere");
  fvFormattedString(&output[OUT_WATERMASSMOATM].cNeg, "TO");
  output[OUT_WATERMASSMOATM].bNeg = 1;
  output[OUT_WATERMASSMOATM].dNeg =
        1 / TOMASS; // division factor to get from SI to desired unit
  output[OUT_WATERMASSMOATM].iNum       = 1;
  output[OUT_WATERMASSMOATM].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_WATERMASSMOATM]           = &WriteWaterMassMOAtm;

  fvFormattedString(&output[OUT_WATERMASSSOL].cName, "WaterMassSol");
  fvFormattedString(&output[OUT_WATERMASSSOL].cDescr, "Water mass in solidified mantle");
  fvFormattedString(&output[OUT_WATERMASSSOL].cNeg, "TO");
  output[OUT_WATERMASSSOL].bNeg = 1;
  output[OUT_WATERMASSSOL].dNeg =
        1 / TOMASS; // division factor to get from SI to desired unit
  output[OUT_WATERMASSSOL].iNum       = 1;
  output[OUT_WATERMASSSOL].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_WATERMASSSOL]           = &WriteWaterMassSol;

  fvFormattedString(&output[OUT_CO2MASSMOATM].cName, "CO2MassMOAtm");
  fvFormattedString(&output[OUT_CO2MASSMOATM].cDescr,
          "CO2 mass in magma ocean and atmosphere");
  fvFormattedString(&output[OUT_CO2MASSMOATM].cNeg, "kg");
  output[OUT_CO2MASSMOATM].bNeg = 1;
  output[OUT_CO2MASSMOATM].dNeg =
        1; // division factor to get from SI to desired unit
  output[OUT_CO2MASSMOATM].iNum       = 1;
  output[OUT_CO2MASSMOATM].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_CO2MASSMOATM]           = &WriteCO2MassMOAtm;

  fvFormattedString(&output[OUT_CO2MASSSOL].cName, "CO2MassSol");
  fvFormattedString(&output[OUT_CO2MASSSOL].cDescr, "CO2 mass in solidified mantle");
  fvFormattedString(&output[OUT_CO2MASSSOL].cNeg, "kg");
  output[OUT_CO2MASSSOL].bNeg = 1;
  output[OUT_CO2MASSSOL].dNeg =
        1; // division factor to get from SI to desired unit
  output[OUT_CO2MASSSOL].iNum       = 1;
  output[OUT_CO2MASSSOL].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_CO2MASSSOL]           = &WriteCO2MassSol;

  fvFormattedString(&output[OUT_OXYGENMASSMOATM].cName, "OxygenMassMOAtm");
  fvFormattedString(&output[OUT_OXYGENMASSMOATM].cDescr,
          "Oxygen mass in magma ocean and atmosphere");
  fvFormattedString(&output[OUT_OXYGENMASSMOATM].cNeg, "kg");
  output[OUT_OXYGENMASSMOATM].bNeg = 1;
  output[OUT_OXYGENMASSMOATM].dNeg =
        1; // division factor to get from SI to desired unit
  output[OUT_OXYGENMASSMOATM].iNum       = 1;
  output[OUT_OXYGENMASSMOATM].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_OXYGENMASSMOATM]           = &WriteOxygenMassMOAtm;

  fvFormattedString(&output[OUT_OXYGENMASSSOL].cName, "OxygenMassSol");
  fvFormattedString(&output[OUT_OXYGENMASSSOL].cDescr, "Oxygen Mass in solidified mantle");
  fvFormattedString(&output[OUT_OXYGENMASSSOL].cNeg, "kg");
  output[OUT_OXYGENMASSSOL].bNeg = 1;
  output[OUT_OXYGENMASSSOL].dNeg =
        1; // division factor to get from SI to desired unit
  output[OUT_OXYGENMASSSOL].iNum       = 1;
  output[OUT_OXYGENMASSSOL].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_OXYGENMASSSOL]           = &WriteOxygenMassSol;

  fvFormattedString(&output[OUT_PRESSWATERATM].cName, "PressWaterAtm");
  fvFormattedString(&output[OUT_PRESSWATERATM].cDescr, "Water pressure in atmosphere");
  fvFormattedString(&output[OUT_PRESSWATERATM].cNeg, "bar");
  output[OUT_PRESSWATERATM].bNeg = 1;
  output[OUT_PRESSWATERATM].dNeg =
        1 / 1e5; // division factor to get from SI to desired unit
  output[OUT_PRESSWATERATM].iNum       = 1;
  output[OUT_PRESSWATERATM].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_PRESSWATERATM]           = &WritePressWaterAtm;

  fvFormattedString(&output[OUT_PRESSCO2ATM].cName, "PressCO2Atm");
  fvFormattedString(&output[OUT_PRESSCO2ATM].cDescr, "CO2 pressure in atmosphere");
  fvFormattedString(&output[OUT_PRESSCO2ATM].cNeg, "bar");
  output[OUT_PRESSCO2ATM].bNeg = 1;
  output[OUT_PRESSCO2ATM].dNeg =
        1 / 1e5; // division factor to get from SI to desired unit
  output[OUT_PRESSCO2ATM].iNum       = 1;
  output[OUT_PRESSCO2ATM].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_PRESSCO2ATM]           = &WritePressCO2Atm;

  fvFormattedString(&output[OUT_PRESSOXYGENATM].cName, "PressOxygenAtm");
  fvFormattedString(&output[OUT_PRESSOXYGENATM].cDescr, "Oxygen pressure in atmosphere");
  fvFormattedString(&output[OUT_PRESSOXYGENATM].cNeg, "bar");
  output[OUT_PRESSOXYGENATM].bNeg = 1;
  output[OUT_PRESSOXYGENATM].dNeg =
        1 / 1e5; // division factor to get from SI to desired unit
  output[OUT_PRESSOXYGENATM].iNum       = 1;
  output[OUT_PRESSOXYGENATM].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_PRESSOXYGENATM]           = &WritePressOxygenAtm;

  fvFormattedString(&output[OUT_HYDROGENMASSSPACE].cName, "HydrogenMassSpace");
  fvFormattedString(&output[OUT_HYDROGENMASSSPACE].cDescr, "H mass lost to space");
  fvFormattedString(&output[OUT_HYDROGENMASSSPACE].cNeg, "kg");
  output[OUT_HYDROGENMASSSPACE].bNeg = 1;
  output[OUT_HYDROGENMASSSPACE].dNeg =
        1; // division factor to get from SI to desired unit
  output[OUT_HYDROGENMASSSPACE].iNum       = 1;
  output[OUT_HYDROGENMASSSPACE].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_HYDROGENMASSSPACE]           = &WriteHydrogenMassSpace;

  fvFormattedString(&output[OUT_OXYGENMASSSPACE].cName, "OxygenMassSpace");
  fvFormattedString(&output[OUT_OXYGENMASSSPACE].cDescr, "O atoms lost to space");
  fvFormattedString(&output[OUT_OXYGENMASSSPACE].cNeg, "kg");
  output[OUT_OXYGENMASSSPACE].bNeg = 1;
  output[OUT_OXYGENMASSSPACE].dNeg =
        1; // division factor to get from SI to desired unit
  output[OUT_OXYGENMASSSPACE].iNum       = 1;
  output[OUT_OXYGENMASSSPACE].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_OXYGENMASSSPACE]           = &WriteOxygenMassSpace;

  fvFormattedString(&output[OUT_FRACFE2O3MAN].cName, "FracFe2O3Man");
  fvFormattedString(&output[OUT_FRACFE2O3MAN].cDescr,
          "Fe2O3 mass fraction in magma ocean");
  output[OUT_FRACFE2O3MAN].bNeg       = 1;
  output[OUT_FRACFE2O3MAN].iNum       = 1;
  output[OUT_FRACFE2O3MAN].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_FRACFE2O3MAN]           = &WriteFracFe2O3Man;

  fvFormattedString(&output[OUT_NETFLUXATMO].cName, "NetFluxAtmo");
  fvFormattedString(&output[OUT_NETFLUXATMO].cDescr, "Atmospheric Net Flux (OLR-ASR)");
  fvFormattedString(&output[OUT_NETFLUXATMO].cNeg, "W/m^2");
  output[OUT_NETFLUXATMO].bNeg       = 1;
  output[OUT_NETFLUXATMO].iNum       = 1;
  output[OUT_NETFLUXATMO].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_NETFLUXATMO]           = &WriteNetFluxAtmo;

  fvFormattedString(&output[OUT_WATERFRACMELT].cName, "WaterFracMelt");
  fvFormattedString(&output[OUT_WATERFRACMELT].cDescr,
          "water mass fraction in magma ocean");
  output[OUT_WATERFRACMELT].bNeg       = 1;
  output[OUT_WATERFRACMELT].iNum       = 1;
  output[OUT_WATERFRACMELT].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_WATERFRACMELT]           = &WriteWaterFracMelt;

  fvFormattedString(&output[OUT_CO2FRACMELT].cName, "CO2FracMelt");
  fvFormattedString(&output[OUT_CO2FRACMELT].cDescr, "CO2 mass fraction in magma ocean");
  output[OUT_CO2FRACMELT].bNeg       = 1;
  output[OUT_CO2FRACMELT].iNum       = 1;
  output[OUT_CO2FRACMELT].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_CO2FRACMELT]           = &WriteCO2FracMelt;

  // XXX Overlap with RadPowerMan from thermint
  fvFormattedString(&output[OUT_RADIOPOWER].cName, "RadioPower");
  fvFormattedString(&output[OUT_RADIOPOWER].cDescr,
          "Power from radiogenic heating in the mantle");
  fvFormattedString(&output[OUT_RADIOPOWER].cNeg, "TW");
  output[OUT_RADIOPOWER].bNeg       = 1;
  output[OUT_RADIOPOWER].dNeg       = 1e-12;
  output[OUT_RADIOPOWER].iNum       = 1;
  output[OUT_RADIOPOWER].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_RADIOPOWER]           = &WriteRadioPower;

  // XXX Overlap with PowerTidal from Eqtide
  fvFormattedString(&output[OUT_TIDALPOWER].cName, "TidalPower");
  fvFormattedString(&output[OUT_TIDALPOWER].cDescr,
          "Power from tidal heating in the mantle");
  fvFormattedString(&output[OUT_TIDALPOWER].cNeg, "TW");
  output[OUT_TIDALPOWER].bNeg       = 1;
  output[OUT_TIDALPOWER].dNeg       = 1e-12;
  output[OUT_TIDALPOWER].iNum       = 1;
  output[OUT_TIDALPOWER].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_TIDALPOWER]           = &WriteTidalPower;

  // XXX Overlap with HZ outputs in output.c
  fvFormattedString(&output[OUT_HZINNEREDGE].cName, "HZInnerEdge");
  fvFormattedString(&output[OUT_HZINNEREDGE].cDescr,
          "Inner Edge of the Habitable Zone (Runaway Greenhouse)");
  fvFormattedString(&output[OUT_HZINNEREDGE].cNeg, "AU");
  output[OUT_HZINNEREDGE].bNeg = 1;
  output[OUT_HZINNEREDGE].dNeg =
        1 / AUM; // division factor to get from SI to desired unit
  output[OUT_HZINNEREDGE].iNum       = 1;
  output[OUT_HZINNEREDGE].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_HZINNEREDGE]           = &WriteHZInnerEdge;

  fvFormattedString(&output[OUT_MELTFRACTION].cName, "MeltFraction");
  fvFormattedString(&output[OUT_MELTFRACTION].cDescr, "Melt fraction of the magma ocean");
  output[OUT_MELTFRACTION].bNeg       = 1;
  output[OUT_MELTFRACTION].iNum       = 1;
  output[OUT_MELTFRACTION].iModuleBit = MAGMOC; // name of module
  fnWrite[OUT_MELTFRACTION]           = &WriteMeltFraction;
}

//------------------------- Finalize Variable Functions ------------------------
// ??
void FinalizeUpdatePotTemp(BODY *body, UPDATE *update, int *iEqn, int iVar,
                           int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iPotTempMagmOc        = (*iEqn)++;
}

void FinalizeUpdateSurfTemp(BODY *body, UPDATE *update, int *iEqn, int iVar,
                            int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iSurfTempMagmOc       = (*iEqn)++;
}

void FinalizeUpdateSolidRadius(BODY *body, UPDATE *update, int *iEqn, int iVar,
                               int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iSolidRadiusMagmOc    = (*iEqn)++;
}

void FinalizeUpdateWaterMassMOAtm(BODY *body, UPDATE *update, int *iEqn,
                                  int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iWaterMassMOAtmMagmOc = (*iEqn)++;
}

void FinalizeUpdateWaterMassSol(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iWaterMassSolMagmOc   = (*iEqn)++;
}

void FinalizeUpdateCO2MassMOAtm(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iCO2MassMOAtmMagmOc   = (*iEqn)++;
}

void FinalizeUpdateCO2MassSol(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iCO2MassSolMagmOc     = (*iEqn)++;
}

void FinalizeUpdateOxygenMassMOAtm(BODY *body, UPDATE *update, int *iEqn,
                                   int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn]  = MAGMOC;
  update[iBody].iOxygenMassMOAtmMagmOc = (*iEqn)++;
}

void FinalizeUpdateOxygenMassSol(BODY *body, UPDATE *update, int *iEqn,
                                 int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = MAGMOC;
  update[iBody].iOxygenMassSolMagmOc  = (*iEqn)++;
}

void FinalizeUpdateHydrogenMassSpace(BODY *body, UPDATE *update, int *iEqn,
                                     int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn]    = MAGMOC;
  update[iBody].iHydrogenMassSpaceMagmOc = (*iEqn)++;
}

void FinalizeUpdateOxygenMassSpace(BODY *body, UPDATE *update, int *iEqn,
                                   int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn]  = MAGMOC;
  update[iBody].iOxygenMassSpaceMagmOc = (*iEqn)++;
}

/************ MAGMOC Logging Functions **************/

void LogOptionsMagmOc(CONTROL *control, FILE *fp) {
}

// PED: this would be for global rad heat parameters, but this is blank bc rad
// is only relevant to each individual body.
void LogMagmOc(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {
}

void LogBodyMagmOc(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UPDATE *update, fnWriteOutput fnWrite[], FILE *fp,
                   int iBody) {
  int iOut;
  fprintf(fp, "----- MAGMOC PARAMETERS (%s)------\n", body[iBody].cName);

  for (iOut = OUTSTARTMAGMOC; iOut < OUTENDMAGMOC; iOut++) {
    if (output[iOut].iNum > 0) {
      // Useful for debugging
      // fprintf(stderr,"%d %d\n",iBody,iOut);
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
  }
}

void AddModuleMagmOc(MODULE *module, int iBody, int iModule) {

  module->iaModule[iBody][iModule] = MAGMOC;

  module->fnCountHalts[iBody][iModule]        = &CountHaltsMagmOc;
  module->fnReadOptions[iBody][iModule]       = &ReadOptionsMagmOc;
  module->fnLogBody[iBody][iModule]           = &LogBodyMagmOc;
  module->fnVerify[iBody][iModule]            = &VerifyMagmOc;
  module->fnAssignDerivatives[iBody][iModule] = &AssignMagmOcDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullMagmOcDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltMagmOc;

  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateMagmOc;
  module->fnInitializeBody[iBody][iModule]   = &InitializeBodyMagmOc;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputMagmOc;

  module->fnFinalizeUpdatePotTemp[iBody][iModule]  = &FinalizeUpdatePotTemp;
  module->fnFinalizeUpdateSurfTemp[iBody][iModule] = &FinalizeUpdateSurfTemp;
  module->fnFinalizeUpdateSolidRadius[iBody][iModule] =
        &FinalizeUpdateSolidRadius;
  module->fnFinalizeUpdateWaterMassMOAtm[iBody][iModule] =
        &FinalizeUpdateWaterMassMOAtm;
  module->fnFinalizeUpdateWaterMassSol[iBody][iModule] =
        &FinalizeUpdateWaterMassSol;
  module->fnFinalizeUpdateCO2MassMOAtm[iBody][iModule] =
        &FinalizeUpdateCO2MassMOAtm;
  module->fnFinalizeUpdateCO2MassSol[iBody][iModule] =
        &FinalizeUpdateCO2MassSol;
  module->fnFinalizeUpdateOxygenMassMOAtm[iBody][iModule] =
        &FinalizeUpdateOxygenMassMOAtm;
  module->fnFinalizeUpdateOxygenMassSol[iBody][iModule] =
        &FinalizeUpdateOxygenMassSol;
  module->fnFinalizeUpdateHydrogenMassSpace[iBody][iModule] =
        &FinalizeUpdateHydrogenMassSpace;
  module->fnFinalizeUpdateOxygenMassSpace[iBody][iModule] =
        &FinalizeUpdateOxygenMassSpace;
  /* HERE */
}

/************* MAGMOC Functions ************/
/* real physic is happening here: calculation of the derivatives of the primary
 * variables */
double fdDPotTemp(BODY *body, SYSTEM *system, int *iaBody) {
  double dRadioPart, dManFluxPart, dRsolPart, dFusionPart;
  dRadioPart =
        (body[iaBody[0]].dRadioHeat + body[iaBody[0]].dTidalHeat) / (4 * PI);
  // dManFluxPart = pow(body[iaBody[0]].dRadius,2) *
  // body[iaBody[0]].dManHeatFlux;
  dManFluxPart = pow(body[iaBody[0]].dRadius, 2) * body[iaBody[0]].dNetFluxAtmo;
  dRsolPart    = body[iaBody[0]].dManMeltDensity * EPSILONMANTLE *
              SILICATEHEATCAP *
              (pow(body[iaBody[0]].dRadius, 3) -
               pow(body[iaBody[0]].dCoreRadius, 3)) /
              3;
  dFusionPart = body[iaBody[0]].dManMeltDensity * HEATFUSIONSILICATE *
                pow(body[iaBody[0]].dSolidRadius, 2) *
                body[iaBody[0]].dFactorDerivative;
  return (dRadioPart - dManFluxPart) / (dRsolPart - dFusionPart);
}

double fdDSurfTemp(BODY *body, SYSTEM *system, int *iaBody) {
  double dFluxPart, dWaterPart, dManPart, dTempPart, dTBL, dReturn;
  // dTBL       = fabs((body[iaBody[0]].dPotTemp-body[iaBody[0]].dSurfTemp) *
  // THERMALCONDUC / body[iaBody[0]].dManHeatFlux); dTBL       =
  // (body[iaBody[0]].dPotTemp-body[iaBody[0]].dSurfTemp) * THERMALCONDUC /
  // body[iaBody[0]].dManHeatFlux; if
  // (dTBL>(body[iaBody[0]].dRadius-body[iaBody[0]].dSolidRadius)) {
  //   dTBL = body[iaBody[0]].dRadius-body[iaBody[0]].dSolidRadius;
  // }
  // dFluxPart  = body[iaBody[0]].dManHeatFlux - body[iaBody[0]].dNetFluxAtmo;
  // dWaterPart = WATERHEATCAP * (body[iaBody[0]].dPressWaterAtm +
  // body[iaBody[0]].dPressOxygenAtm) / body[iaBody[0]].dGravAccelSurf; dManPart
  // = SILICATEHEATCAP * body[iaBody[0]].dManMeltDensity /
  // (3*pow(body[iaBody[0]].dRadius,2)); dTempPart  =
  // pow(body[iaBody[0]].dRadius,3) - pow((body[iaBody[0]].dRadius-dTBL),3);
  // // dTempPart  = pow(body[iaBody[0]].dRadius,3) - pow(dTBL,3);
  // // dTempPart  = 3*pow(body[iaBody[0]].dRadius,2)*dTBL;
  // // dTempPart  = pow(body[iaBody[0]].dRadius,3) -
  // pow((body[iaBody[0]].dRadius-body[iaBody[0]].dSolidRadius),3);

  // dReturn    = dFluxPart / (dWaterPart + dManPart * dTempPart);
  dReturn = 0;

  return dReturn;
}

double fdDSolidRadius(BODY *body, SYSTEM *system, int *iaBody) {
  double dDerivative;
  if (body[iaBody[0]].bManSolid) {
    dDerivative = 0;
  } else if (body[iaBody[0]].dSolidRadiusLocal <= body[iaBody[0]].dCoreRadius) {
    dDerivative = 0;
  } else {
    dDerivative =
          body[iaBody[0]].dFactorDerivative * fdDPotTemp(body, system, iaBody);
  }
  return dDerivative;
}

double fdDWaterMassSol(BODY *body, SYSTEM *system, int *iaBody) {
  return 4 * PI * body[iaBody[0]].dManMeltDensity *
         body[iaBody[0]].dWaterPartCoeff * body[iaBody[0]].dWaterFracMelt *
         pow(body[iaBody[0]].dSolidRadius, 2) *
         fdDSolidRadius(body, system, iaBody);
  // return
  // 4*PI*body[iaBody[0]].dManMeltDensity*WATERPARTCOEFF*body[iaBody[0]].dWaterFracMelt*pow(body[iaBody[0]].dSolidRadius,2)*fdDSolidRadius(body,system,iaBody);
}

double fdDWaterMassMOAtm(BODY *body, SYSTEM *system, int *iaBody) {
  if (body[iaBody[0]].dWaterMassMOAtm <= 0) {
    return 0;
  }
  return body[iaBody[0]].dWaterMassEsc - fdDWaterMassSol(body, system, iaBody);
}

double fdDCO2MassSol(BODY *body, SYSTEM *system, int *iaBody) {
  return 4 * PI * body[iaBody[0]].dManMeltDensity * CO2PARTCOEFF *
         body[iaBody[0]].dCO2FracMelt * pow(body[iaBody[0]].dSolidRadius, 2) *
         fdDSolidRadius(body, system, iaBody);
}

double fdDCO2MassMOAtm(BODY *body, SYSTEM *system, int *iaBody) {
  return -fdDCO2MassSol(body, system, iaBody);
}

double fdDOxygenMassSol(BODY *body, SYSTEM *system, int *iaBody) {
  return 4 * PI * body[iaBody[0]].dManMeltDensity *
         body[iaBody[0]].dFracFe2O3Man * pow(body[iaBody[0]].dSolidRadius, 2) *
         fdDSolidRadius(body, system, iaBody) * MOLWEIGHTOXYGEN /
         (2 * MOLWEIGHTFEO15);
}

double fdDOxygenMassMOAtm(BODY *body, SYSTEM *system, int *iaBody) {
  if (body[iaBody[0]].bPlanetDesiccated) {
    body[iaBody[0]].dOxygenMassEsc = 0;
  }
  return body[iaBody[0]].dOxygenMassEsc -
         fdDOxygenMassSol(body, system, iaBody);
}

double fdDHydrogenMassSpace(BODY *body, SYSTEM *system, int *iaBody) {
  return -body[iaBody[0]].dWaterMassEsc * 2 * MOLWEIGHTHYDROGEN /
         MOLWEIGHTWATER;
}

double fdDOxygenMassSpace(BODY *body, SYSTEM *system, int *iaBody) {
  return -body[iaBody[0]].dWaterMassEsc * MOLWEIGHTOXYGEN / MOLWEIGHTWATER -
         body[iaBody[0]].dOxygenMassEsc;
}
