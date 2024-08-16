/**
    @file atmesc.c
    @brief Subroutines that control the integration of the
    atmospheric escape model.
    @author Rodrigo Luger ([rodluger@gmail.com](mailto:rodluger@gmail.com>))
    @date May 12 2015

    @par Description
    \rst
   This module defines differential equations controlling the evolution
   of planetary atmospheres under intense extreme ultraviolet (XUV)
   stellar irradiation. The `atmesc <atmesc.html>`_ module implements
   energy-limited and diffusion-limited escape for hydrogen/helium atmospheres
   and water vapor atmospheres following :cite:`Luger2015`,
   :cite:`LugerBarnes2015`, and :cite:`LehmerCatling17`. \endrst

*/

#include "vplanet.h"

/**
Create a copy of the body at index \p iBody. Used during integration
of the differential equations.

@param dest The body copy
@param src The original body instance
@param foo Who knows!?
@param iNumBodies Number of bodies
@param iBody Current body index
*/
void BodyCopyAtmEsc(BODY *dest, BODY *src, int foo, int iNumBodies, int iBody) {
  dest[iBody].dSurfaceWaterMass    = src[iBody].dSurfaceWaterMass;
  dest[iBody].dOxygenMass          = src[iBody].dOxygenMass;
  dest[iBody].dOxygenMantleMass    = src[iBody].dOxygenMantleMass;
  dest[iBody].dEnvelopeMass        = src[iBody].dEnvelopeMass;
  dest[iBody].dXFrac               = src[iBody].dXFrac;
  dest[iBody].dAtmXAbsEffH         = src[iBody].dAtmXAbsEffH;
  dest[iBody].dAtmXAbsEffH2O       = src[iBody].dAtmXAbsEffH2O;
  dest[iBody].dMinSurfaceWaterMass = src[iBody].dMinSurfaceWaterMass;
  dest[iBody].dMinEnvelopeMass     = src[iBody].dMinEnvelopeMass;
  dest[iBody].iWaterLossModel      = src[iBody].iWaterLossModel;
  dest[iBody].iAtmXAbsEffH2OModel  = src[iBody].iAtmXAbsEffH2OModel;
  dest[iBody].dKTide               = src[iBody].dKTide;
  dest[iBody].dMDotWater           = src[iBody].dMDotWater;
  dest[iBody].dFHRef               = src[iBody].dFHRef;
  dest[iBody].dOxygenEta           = src[iBody].dOxygenEta;
  dest[iBody].dCrossoverMass       = src[iBody].dCrossoverMass;
  dest[iBody].bRunaway             = src[iBody].bRunaway;
  dest[iBody].iWaterEscapeRegime   = src[iBody].iWaterEscapeRegime;
  dest[iBody].iHEscapeRegime       = src[iBody].iHEscapeRegime;
  dest[iBody].dFHDiffLim           = src[iBody].dFHDiffLim;
  dest[iBody].iPlanetRadiusModel   = src[iBody].iPlanetRadiusModel;
  dest[iBody].bInstantO2Sink       = src[iBody].bInstantO2Sink;
  dest[iBody].dRGDuration          = src[iBody].dRGDuration;
  dest[iBody].dRadXUV              = src[iBody].dRadXUV;
  dest[iBody].dRadSolid            = src[iBody].dRadSolid;
  dest[iBody].dPresXUV             = src[iBody].dPresXUV;
  dest[iBody].dPresSurf            = src[iBody].dPresSurf;
  dest[iBody].dScaleHeight         = src[iBody].dScaleHeight;
  dest[iBody].dThermTemp           = src[iBody].dThermTemp;
  dest[iBody].dFlowTemp            = src[iBody].dFlowTemp;
  dest[iBody].dAtmGasConst         = src[iBody].dAtmGasConst;
  dest[iBody].dFXUV                = src[iBody].dFXUV;
  dest[iBody].bCalcFXUV            = src[iBody].bCalcFXUV;
  dest[iBody].dJeansTime           = src[iBody].dJeansTime;
  dest[iBody].dRocheRadius         = src[iBody].dRocheRadius;
  dest[iBody].dMinKTide            = src[iBody].dMinKTide;
  dest[iBody].dAtmEscXi            = src[iBody].dAtmEscXi;
  dest[iBody].dBondiRadius         = src[iBody].dBondiRadius;
  dest[iBody].bUseEnergyLimited    = src[iBody].bUseEnergyLimited;
  dest[iBody].bUseRRLimited        = src[iBody].bUseRRLimited;
  dest[iBody].bUseBondiLimited     = src[iBody].bUseBondiLimited;
  dest[iBody].bAtmEscAuto          = src[iBody].bAtmEscAuto;
  dest[iBody].dEnvMassDt           = src[iBody].dEnvMassDt;
  dest[iBody].bAutoThermTemp       = src[iBody].bAutoThermTemp;
  dest[iBody].bStopWaterLossInHZ   = src[iBody].bStopWaterLossInHZ;
}

/**************** ATMESC options ********************/

/**
Read the XUV flux from the input file.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadFXUV(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFXUV =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFXUV = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFXUV = options->dDefault;
  }
}

/**
\rst
Read the thermospheric temperature for the :cite:`LehmerCatling17` atmospheric
escape model. \endrst

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadThermTemp(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dThermTemp =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dThermTemp = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
    body[iFile - 1].bAutoThermTemp = 0;
  } else {
    if (iFile > 0) {
      /* Note that here we both assign the default and force the code to
      calculate the thermal temperature automatically */
      body[iFile - 1].bAutoThermTemp = 1;
      body[iFile - 1].dThermTemp     = options->dDefault;
    }
  }
}

/**
\rst
Read the temperature of the hydro flow for the Luger+Barnes escape model.
\endrst

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadFlowTemp(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlowTemp =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlowTemp = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlowTemp = options->dDefault;
  }
}

/**
Read the atmospheric gas constant the Lehmer and Catling (2017) atmospheric
escape model.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadAtmGasConst(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dAtmGasConst =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dAtmGasConst = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dAtmGasConst = options->dDefault;
  }
}

/**
Read the Jeans time, the time at which the flow transitions from hydrodynamic
to ballistic.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadJeansTime(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dJeansTime =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dJeansTime =
            dTmp * fdUnitsTime(control->Units[iFile].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr,
                "INFO: %s not set for body %s, defaulting to %.2e seconds.\n",
                options->cName, body[iFile - 1].cName, options->dDefault);
      }
      body[iFile - 1].dJeansTime = options->dDefault;
    }
  }
}

/**
Read the effective XUV absorption pressure for the Lehmner and Catling (2017)
model.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadPresXUV(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dPresXUV =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dPresXUV = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dPresXUV = options->dDefault;
  }
}

/**
Read the water loss model for the Luger and Barnes (2015) atmospheric escape
model.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadWaterLossModel(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "lb15", 4)) {
      body[iFile - 1].iWaterLossModel = ATMESC_LB15;
    } else if (!memcmp(sLower(cTmp), "lbex", 4)) {
      body[iFile - 1].iWaterLossModel = ATMESC_LBEXACT;
    } else if (!memcmp(sLower(cTmp), "tian", 4)) {
      body[iFile - 1].iWaterLossModel = ATMESC_TIAN;
    } else if (!memcmp(sLower(cTmp), "ls16", 4)) {
      body[iFile - 1].iWaterLossModel = ATMESC_LS2016;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are LB15, LBEXACT, "
                "or TIAN.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iWaterLossModel = ATMESC_LBEXACT;
  }
}

/**
Read the XUV absorption efficiency model for the Luger and Barnes (2015)
atmospheric escape model.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadAtmXAbsEffH2OModel(BODY *body, CONTROL *control, FILES *files,
                            OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "bolm", 4)) {
      body[iFile - 1].iAtmXAbsEffH2OModel = ATMESC_BOL16;
    } else if (!memcmp(sLower(cTmp), "none", 4)) {
      body[iFile - 1].iAtmXAbsEffH2OModel = ATMESC_NONE;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are BOLMONT16 or "
                "NONE.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iAtmXAbsEffH2OModel = ATMESC_NONE;
  }
}

/**
Read the planet radius model.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadPlanetRadiusModel(BODY *body, CONTROL *control, FILES *files,
                           OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "lo", 2)) {
      body[iFile - 1].iPlanetRadiusModel = ATMESC_LOP12;
    } else if (!memcmp(sLower(cTmp), "le", 2)) {
      body[iFile - 1].iPlanetRadiusModel = ATMESC_LEHMER17;
    } else if (!memcmp(sLower(cTmp), "pr", 2)) {
      body[iFile - 1].iPlanetRadiusModel = ATMESC_PROXCENB;
    } else if (!memcmp(sLower(cTmp), "no", 2)) {
      body[iFile - 1].iPlanetRadiusModel = ATMESC_NONE;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are LOPEZ12, "
                "PROXCENB, LEHMER17 or NONE.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iPlanetRadiusModel = ATMESC_NONE;
  }
}

/**
Read the parameter that controls surface O2 sinks for the Luger and Barnes
(2015) model.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadInstantO2Sink(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bInstantO2Sink = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    AssignDefaultInt(options, &body[iFile - 1].bInstantO2Sink,
                     files->iNumInputs);
  }
}

/**
Read the parameter that controls water loss once planet reaches HZ.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadStopWaterLossInHZ(BODY *body, CONTROL *control, FILES *files,
                           OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bStopWaterLossInHZ = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &body[iFile - 1].bStopWaterLossInHZ,
                       files->iNumInputs);
    }
  }
}


/**
Read the parameter that controls whether or not to limit envelope mass loss at
the Bondi limit

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadBondiLimited(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bUseBondiLimited = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    AssignDefaultInt(options, &body[iFile - 1].bUseBondiLimited,
                     files->iNumInputs);
  }
}


/**
Read the parameter that controls whether or not to use energy-limited escape
for H envelope mass loss

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadEnergyLimited(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bUseEnergyLimited = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    AssignDefaultInt(options, &body[iFile - 1].bUseEnergyLimited,
                     files->iNumInputs);
  }
}


/**
Read the parameter that controls whether or not to use
radiation/recombination-limited escape

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadRRLimited(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bUseRRLimited = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    AssignDefaultInt(options, &body[iFile - 1].bUseRRLimited,
                     files->iNumInputs);
  }
}

/**
Read the parameter that controls whether or not to let atmesc determine which
escape regime the planet is in

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadAtmEscAuto(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bAtmEscAuto = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    AssignDefaultInt(options, &body[iFile - 1].bAtmEscAuto, files->iNumInputs);
  }
}

/**
Read the planet's effective XUV radius.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadXFrac(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0 || dTmp > 1) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must lie in the range [0,1].\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dXFrac = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXFrac = options->dDefault;
  }
}

/**
Read the XUV absorption efficiency for hydrogen.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadAtmXAbsEffH(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0 || dTmp > 1) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be in the range [0,1].\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dAtmXAbsEffH = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dAtmXAbsEffH = options->dDefault;
  }
}

/**
Minimum value for KTide

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadMinKTide(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0 || dTmp > 1) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be in the range [0,1].\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dMinKTide = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dMinKTide = options->dDefault;
  }
}

/**
Read the XUV absorption efficiency for water.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadAtmXAbsEffH2O(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0 || dTmp > 1) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be in the rane [0,1].\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dAtmXAbsEffH2O = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dAtmXAbsEffH2O = options->dDefault;
  }
}

// ReadEnvelopeMass is in options.c to avoid memory leaks in verifying envelope.

/**
Read the planet's initial atmospheric oxygen mass (Luger and Barnes 2015 model).

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadOxygenMass(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be >= 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dOxygenMass = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dOxygenMass = options->dDefault;
  }
}

/**
Read the planet's initial mantle oxygen mass (Luger and Barnes 2015 model).

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadOxygenMantleMass(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be >= 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dOxygenMantleMass = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dOxygenMantleMass = options->dDefault;
  }
}

/* Halts */

/**
Read the parameter that controls whether the code halts when the planet is
desiccated.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadHaltMinSurfaceWaterMass(BODY *body, CONTROL *control, FILES *files,
                                 OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bSurfaceDesiccated = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &control->Halt[iFile - 1].bSurfaceDesiccated,
                       files->iNumInputs);
    }
  }
}

/* ReadMinSurfaceWaterMass is in options.c to avoid memory leaks when verifying
  ocean. */

/* ReadSurfaceWaterMass is in options.c to avoid memory leaks when verifying
  ocean. */

/**
Read the parameter that controls whether the code halts when the planet's
envelope is fully evaporated.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadHaltMinEnvelopeMass(BODY *body, CONTROL *control, FILES *files,
                             OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bEnvelopeGone = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &control->Halt[iFile - 1].bEnvelopeGone,
                       files->iNumInputs);
    }
  }
}

// ReadMinEnvelopeMass is in options.c to eliminate memory leak in verifying
// envelope

/**
Initialize the user options for the atmospheric escape model.

@param options A pointer to the OPTIONS instance
@param fnRead Array of pointers to the functions that read in the options
*/
void InitializeOptionsAtmEsc(OPTIONS *options, fnReadOption fnRead[]) {
  int iOpt, iFile;

  fvFormattedString(&options[OPT_XFRAC].cName, "dXFrac");
  fvFormattedString(&options[OPT_XFRAC].cDescr, "Fraction of planet radius in X-ray/UV");
  fvFormattedString(&options[OPT_XFRAC].cDefault, "1");
  fvFormattedString(&options[OPT_XFRAC].cDimension, "nd");
  options[OPT_XFRAC].dDefault   = 1.0;
  options[OPT_XFRAC].iType      = 2;
  options[OPT_XFRAC].bMultiFile = 1;
  fnRead[OPT_XFRAC]             = &ReadXFrac;
  fvFormattedString(
        &options[OPT_XFRAC].cLongDescr,
        "Ratio of the planet's XUV radius to its total radius. The XUV radius "
        "is\n"
        "defined to be the distance between the center of the planet and the\n"
        "absorbing layer, defined to be where the optical depth is 1. Should "
        "be\n"
        "in the range (0,1].");

  fvFormattedString(&options[OPT_ATMXABSEFFH].cName, "dAtmXAbsEffH");
  fvFormattedString(&options[OPT_ATMXABSEFFH].cDescr,
          "Hydrogen X-ray/UV absorption efficiency (epsilon)");
  fvFormattedString(&options[OPT_ATMXABSEFFH].cDefault, "0.15");
  fvFormattedString(&options[OPT_ATMXABSEFFH].cDimension, "nd");
  options[OPT_ATMXABSEFFH].dDefault   = 0.15;
  options[OPT_ATMXABSEFFH].iType      = 2;
  options[OPT_ATMXABSEFFH].bMultiFile = 1;
  fnRead[OPT_ATMXABSEFFH]             = &ReadAtmXAbsEffH;
  fvFormattedString(&options[OPT_ATMXABSEFFH].cLongDescr,
          "XUV absoprtion efficiency parameter, epsilon_{XUV}, in Eq. (A1) in\n"
          "Barnes et al. (2019). Must lie in the range [0,1].");

  fvFormattedString(&options[OPT_ATMXABSEFFH2O].cName, "dAtmXAbsEffH2O");
  fvFormattedString(&options[OPT_ATMXABSEFFH2O].cDescr,
          "Water X-ray/UV absorption efficiency (epsilon)");
  fvFormattedString(&options[OPT_ATMXABSEFFH2O].cDefault, "0.30");
  fvFormattedString(&options[OPT_ATMXABSEFFH2O].cDimension, "nd");
  options[OPT_ATMXABSEFFH2O].dDefault   = 0.30;
  options[OPT_ATMXABSEFFH2O].iType      = 2;
  options[OPT_ATMXABSEFFH2O].bMultiFile = 1;
  fnRead[OPT_ATMXABSEFFH2O]             = &ReadAtmXAbsEffH2O;
  fvFormattedString(&options[OPT_ATMXABSEFFH2O].cLongDescr,
          "XUV absoprtion efficiency parameter for water vapor as defined in\n"
          "Luger & Barnes (2015, AsBio, 15, 57). Must lie in range [0,1].");

  fvFormattedString(&options[OPT_ATMXABSEFFH2OMODEL].cName, "sAtmXAbsEffH2OModel");
  fvFormattedString(&options[OPT_ATMXABSEFFH2OMODEL].cDescr,
          "Water X-ray/XUV absorption efficiency evolution model");
  fvFormattedString(&options[OPT_ATMXABSEFFH2OMODEL].cDefault, "NONE");
  fvFormattedString(&options[OPT_ATMXABSEFFH2OMODEL].cValues, "BOLMONT16 NONE");
  fvFormattedString(&options[OPT_ATMXABSEFFH2OMODEL].cDimension, "nd");
  options[OPT_ATMXABSEFFH2OMODEL].iType      = 3;
  options[OPT_ATMXABSEFFH2OMODEL].bMultiFile = 1;
  fnRead[OPT_ATMXABSEFFH2OMODEL]             = &ReadAtmXAbsEffH2OModel;
  fvFormattedString(&options[OPT_ATMXABSEFFH2OMODEL].cLongDescr,
          "If BOLMONT16 is selected, then the value of %s will follow the "
          "model of\n"
          "Bolmont et al. (2017, MNRAS, 464, 3728). NONE will not change the "
          "input\n"
          "value for %s.",
          options[OPT_ATMXABSEFFH2O].cName, options[OPT_ATMXABSEFFH2O].cName);

  fvFormattedString(&options[OPT_OXYGENMASS].cName, "dOxygenMass");
  fvFormattedString(&options[OPT_OXYGENMASS].cDescr,
          "The initial oxygen mass in the atmosphere.");
  fvFormattedString(&options[OPT_OXYGENMASS].cDefault, "0");
  fvFormattedString(&options[OPT_OXYGENMASS].cDimension, "mass");
  options[OPT_OXYGENMASS].dDefault   = 0;
  options[OPT_OXYGENMASS].iType      = 2;
  options[OPT_OXYGENMASS].bMultiFile = 1;
  fnRead[OPT_OXYGENMASS]             = &ReadOxygenMass;
  // No LongDescr needed

  fvFormattedString(&options[OPT_OXYGENMANTLEMASS].cName, "dOxygenMantleMass");
  fvFormattedString(&options[OPT_OXYGENMANTLEMASS].cDescr,
          "Initial Oxygen Mass in the Mantle");
  fvFormattedString(&options[OPT_OXYGENMANTLEMASS].cDefault, "0");
  fvFormattedString(&options[OPT_OXYGENMANTLEMASS].cDimension, "mass");
  options[OPT_OXYGENMANTLEMASS].dDefault   = 0;
  options[OPT_OXYGENMANTLEMASS].iType      = 2;
  options[OPT_OXYGENMANTLEMASS].bMultiFile = 1;
  fnRead[OPT_OXYGENMANTLEMASS]             = &ReadOxygenMantleMass;
  // No LongDescr needed

  fvFormattedString(&options[OPT_WATERLOSSMODEL].cName, "sWaterLossModel");
  fvFormattedString(&options[OPT_WATERLOSSMODEL].cDescr,
          "Water loss and oxygen buildup model");
  fvFormattedString(&options[OPT_WATERLOSSMODEL].cDefault, "LBEXACT");
  fvFormattedString(&options[OPT_WATERLOSSMODEL].cValues, "LB15 LBEXACT TIAN LS16");
  fvFormattedString(&options[OPT_WATERLOSSMODEL].cDimension, "nd");
  options[OPT_WATERLOSSMODEL].iType      = 3;
  options[OPT_WATERLOSSMODEL].bMultiFile = 1;
  fnRead[OPT_WATERLOSSMODEL]             = &ReadWaterLossModel;
  fvFormattedString(&options[OPT_WATERLOSSMODEL].cLongDescr,
          "The water loss rate will be determined by the selected model.\n"
          "The options are LB15, LBEXACT, TIAN, and LS16.\n");

  fvFormattedString(&options[OPT_PLANETRADIUSMODEL].cName, "sPlanetRadiusModel");
  fvFormattedString(&options[OPT_PLANETRADIUSMODEL].cDescr, "Gaseous Planet Radius Model");
  fvFormattedString(&options[OPT_PLANETRADIUSMODEL].cDefault, "NONE");
  fvFormattedString(&options[OPT_PLANETRADIUSMODEL].cValues,
          "LOPEZ12 PROXCENB LEHMER17 NONE.");
  options[OPT_PLANETRADIUSMODEL].iType      = 3;
  options[OPT_PLANETRADIUSMODEL].bMultiFile = 1;
  fnRead[OPT_PLANETRADIUSMODEL]             = &ReadPlanetRadiusModel;
  fvFormattedString(
        &options[OPT_PLANETRADIUSMODEL].cLongDescr,
        "If LOPEZ12 is selected, the planet radius will follow the model in\n"
        "Lopez et al. (2012, ApJ, 761, 59). PROXCENB will use the model for\n"
        "Proxima b in Barnes et al. (2016, arXiv:1608.06919). LEHMER17 is the\n"
        "Lehmer & Catling (2017, ApJ, 845, 130). NONE will cause the radius "
        "to\n"
        "remain constant.\n");

  fvFormattedString(&options[OPT_INSTANTO2SINK].cName, "bInstantO2Sink");
  fvFormattedString(&options[OPT_INSTANTO2SINK].cDescr,
          "Is oxygen absorbed instantaneously at the surface?");
  fvFormattedString(&options[OPT_INSTANTO2SINK].cDefault, "0");
  options[OPT_INSTANTO2SINK].iType      = 0;
  options[OPT_INSTANTO2SINK].bMultiFile = 1;
  fnRead[OPT_INSTANTO2SINK]             = &ReadInstantO2Sink;
  fvFormattedString(
        &options[OPT_INSTANTO2SINK].cLongDescr,
        "If set to 1, then all oxygen released by photolysis is immediately\n"
        "removed from the atmosphere. This mimics rapid surface oxidation.\n");

  fvFormattedString(&options[OPT_STOPWATERLOSSINHZ].cName, "bStopWaterLossInHZ");
  fvFormattedString(&options[OPT_STOPWATERLOSSINHZ].cDescr,
          "Stop water photolysis and H escape in the HZ?");
  fvFormattedString(&options[OPT_STOPWATERLOSSINHZ].cDefault, "1");
  options[OPT_STOPWATERLOSSINHZ].iType      = 0;
  options[OPT_STOPWATERLOSSINHZ].bMultiFile = 1;
  fnRead[OPT_STOPWATERLOSSINHZ]             = &ReadStopWaterLossInHZ;
  fvFormattedString(&options[OPT_STOPWATERLOSSINHZ].cLongDescr,
          "If set to 1, then all water photolysis and hydrogen escape will not "
          "occur\n"
          "for a planet in the habitable zone, defined to be when the "
          "instellation\n."
          "is less than the runaway greenhouse threshold.\n");

  fvFormattedString(&options[OPT_BONDILIMITED].cName, "bUseBondiLimited");
  fvFormattedString(&options[OPT_BONDILIMITED].cDescr,
          "Use Bondi-limited escape for H envelope?");
  fvFormattedString(&options[OPT_BONDILIMITED].cDefault, "0");
  options[OPT_BONDILIMITED].iType      = 0;
  options[OPT_BONDILIMITED].bMultiFile = 1;
  fnRead[OPT_BONDILIMITED]             = &ReadBondiLimited;
  fvFormattedString(&options[OPT_BONDILIMITED].cLongDescr,
          "Force the atmospheric erosion of a H envelope to be Bondi-limited "
          "where the\n"
          "mass loss is regulated by the sound speed at the sonic point "
          "following Equation 4\n"
          "from Owen & Wu (2016). Note we compute the sound speed by assuming "
          "an isothermal\n"
          "atmosphere composed of diatomic hydrogen and that the local "
          "temperature is set by\n"
          "the local blackbody equlibrium temperature (Owen & Wu 2016, "
          "Equation 2).\n");

  fvFormattedString(&options[OPT_ENERGYLIMITED].cName, "bUseEnergyLimited");
  fvFormattedString(&options[OPT_ENERGYLIMITED].cDescr,
          "Use energy-limited escape for H envelope?");
  fvFormattedString(&options[OPT_ENERGYLIMITED].cDefault, "0");
  options[OPT_ENERGYLIMITED].iType      = 0;
  options[OPT_ENERGYLIMITED].bMultiFile = 1;
  fnRead[OPT_ENERGYLIMITED]             = &ReadEnergyLimited;
  fvFormattedString(&options[OPT_ENERGYLIMITED].cLongDescr,
          "Force the atmospheric erosion of a H envelope to be energy-limited, "
          "i.e. the\n"
          "mass loss is directly proportional to the incident XUV flux "
          "(Equation 5\n"
          "from Luger et al. (2015)).\n");

  fvFormattedString(&options[OPT_RRLIMITED].cName, "bUseRRLimited");
  fvFormattedString(&options[OPT_RRLIMITED].cDescr,
          "Use radiation/recombination-limited escape for H envelope?");
  fvFormattedString(&options[OPT_RRLIMITED].cDefault, "0");
  options[OPT_RRLIMITED].iType      = 0;
  options[OPT_RRLIMITED].bMultiFile = 1;
  fnRead[OPT_RRLIMITED]             = &ReadRRLimited;
  fvFormattedString(&options[OPT_RRLIMITED].cLongDescr,
          "Force the atmospheric erosion of a H envelope to be "
          "radiation/recombination-limited, i.e. the\n"
          "mass loss is directly proportional to the sqrt of the incident XUV "
          "flux (Equation 13\n"
          "from Luger et al. (2015) and Murray-Clay et al. (2009).\n");

  fvFormattedString(&options[OPT_ATMESCAUTO].cName, "bAtmEscAuto");
  fvFormattedString(&options[OPT_ATMESCAUTO].cDescr,
          "Let atmesc determine H envelope escape regime?");
  fvFormattedString(&options[OPT_ATMESCAUTO].cDefault, "0");
  options[OPT_ATMESCAUTO].iType      = 0;
  options[OPT_ATMESCAUTO].bMultiFile = 1;
  fnRead[OPT_ATMESCAUTO]             = &ReadAtmEscAuto;
  fvFormattedString(&options[OPT_ATMESCAUTO].cLongDescr,
          "Let AtmEsc determine the proper atmospheric escape regime for an "
          "eroding H envelope.\n"
          "If the planetary radius exceeds the Roche lobe, the unbound "
          "material is assumed to be\n"
          "Bondi-limited and mass loss proceeds following Equation 4 from Owen "
          "& Wu (2016).\n"
          "For planetary radius less than the Roche limit, if the incident XUV "
          "flux exceeds\n"
          "the critical flux (Equation A25 from Luger et. al. 2015), the mass "
          "loss is\n"
          "radiation/recombination-limited and scales as XUV^0.5. Otherwise, "
          "the loss\n"
          "is energy-limited and is linearly proportional to the incident XUV "
          "flux.\n");

  fvFormattedString(&options[OPT_HALTDESICCATED].cName, "bHaltSurfaceDesiccated");
  fvFormattedString(&options[OPT_HALTDESICCATED].cDescr, "Halt at Desiccation?");
  fvFormattedString(&options[OPT_HALTDESICCATED].cDefault, "0");
  options[OPT_HALTDESICCATED].iType      = 0;
  options[OPT_HALTDESICCATED].bMultiFile = 1;
  fnRead[OPT_HALTDESICCATED]             = &ReadHaltMinSurfaceWaterMass;

  fvFormattedString(&options[OPT_HALTENVELOPEGONE].cName, "bHaltEnvelopeGone");
  fvFormattedString(&options[OPT_HALTENVELOPEGONE].cDescr,
          "Halt When Envelope Evaporates?");
  fvFormattedString(&options[OPT_HALTENVELOPEGONE].cDefault, "0");
  options[OPT_HALTENVELOPEGONE].bMultiFile = 1;
  options[OPT_HALTENVELOPEGONE].iType      = 0;
  fnRead[OPT_HALTENVELOPEGONE]             = &ReadHaltMinEnvelopeMass;

  fvFormattedString(&options[OPT_THERMTEMP].cName, "dThermTemp");
  fvFormattedString(&options[OPT_THERMTEMP].cDescr, "Thermosphere temperature");
  fvFormattedString(&options[OPT_THERMTEMP].cDefault, "400");
  fvFormattedString(&options[OPT_THERMTEMP].cDimension, "temperature");
  options[OPT_THERMTEMP].dDefault   = 400;
  options[OPT_THERMTEMP].iType      = 2;
  options[OPT_THERMTEMP].bMultiFile = 1;
  fnRead[OPT_THERMTEMP]             = &ReadThermTemp;
  fvFormattedString(&options[OPT_THERMTEMP].cLongDescr,
          "The thermal temperature of a planet heated by radiation from the "
          "primary.\n"
          "The user may set a value which will then remain constant for the "
          "simulation.\n"
          "If the user does not specify a value, then it will be calculated "
          "automatically\n"
          "from the formula T = (F(1-A)/sigma)^0.25, where F is the incident "
          "radiation,\n"
          "A is albedo and sigma is the Steffan-Boltzman constant.\n");

  fvFormattedString(&options[OPT_FLOWTEMP].cName, "dFlowTemp");
  fvFormattedString(&options[OPT_FLOWTEMP].cDescr, "Temperature of the hydrodynamic flow");
  fvFormattedString(&options[OPT_FLOWTEMP].cDefault, "400");
  fvFormattedString(&options[OPT_FLOWTEMP].cDimension, "temperature");
  options[OPT_FLOWTEMP].dDefault   = 400;
  options[OPT_FLOWTEMP].iType      = 2;
  options[OPT_FLOWTEMP].bMultiFile = 1;
  fnRead[OPT_FLOWTEMP]             = &ReadFlowTemp;

  fvFormattedString(&options[OPT_JEANSTIME].cName, "dJeansTime");
  fvFormattedString(&options[OPT_JEANSTIME].cDescr,
          "Time at which flow transitions to Jeans escape");
  fvFormattedString(&options[OPT_JEANSTIME].cDefault, "1 Gyr");
  fvFormattedString(&options[OPT_JEANSTIME].cDimension, "time");
  options[OPT_JEANSTIME].dDefault   = 1.e9 * YEARSEC;
  options[OPT_JEANSTIME].iType      = 2;
  options[OPT_JEANSTIME].bMultiFile = 1;
  options[OPT_JEANSTIME].dNeg       = 1.e9 * YEARSEC;
  fvFormattedString(&options[OPT_JEANSTIME].cNeg, "Gyr");
  fnRead[OPT_JEANSTIME] = &ReadJeansTime;

  fvFormattedString(&options[OPT_PRESXUV].cName, "dPresXUV");
  fvFormattedString(&options[OPT_PRESXUV].cDescr, "Pressure at base of thermosphere");
  fvFormattedString(&options[OPT_PRESXUV].cDefault, "5 Pa");
  fvFormattedString(&options[OPT_PRESXUV].cDimension, "pressure");
  options[OPT_PRESXUV].dDefault   = 5.0;
  options[OPT_PRESXUV].iType      = 2;
  options[OPT_PRESXUV].bMultiFile = 1;
  fnRead[OPT_PRESXUV]             = &ReadPresXUV;

  fvFormattedString(&options[OPT_ATMGASCONST].cName, "dAtmGasConst");
  fvFormattedString(&options[OPT_ATMGASCONST].cDescr, "Atmospheric Gas Constant");
  fvFormattedString(&options[OPT_ATMGASCONST].cDefault, "4124");
  fvFormattedString(&options[OPT_ATMGASCONST].cDimension, "energy/temperature/mass");
  options[OPT_ATMGASCONST].dDefault   = 4124.0;
  options[OPT_ATMGASCONST].iType      = 2;
  options[OPT_ATMGASCONST].bMultiFile = 1;
  fnRead[OPT_ATMGASCONST]             = &ReadAtmGasConst;

  fvFormattedString(&options[OPT_FXUV].cName, "dFXUV");
  fvFormattedString(&options[OPT_FXUV].cDescr, "XUV flux at the body's orbit");
  fvFormattedString(&options[OPT_FXUV].cDimension, "energyflux");
  options[OPT_FXUV].iType      = 2;
  options[OPT_FXUV].bMultiFile = 1;
  options[OPT_FXUV].dNeg       = 1;
  fvFormattedString(&options[OPT_FXUV].cNeg, "W/m^2");
  fnRead[OPT_FXUV] = &ReadFXUV;

  fvFormattedString(&options[OPT_MINKTIDE].cName, "dMinKTide");
  fvFormattedString(&options[OPT_MINKTIDE].cDescr,
          "Minimum value for stellar gravitaitonal enhancement of mass loss");
  fvFormattedString(&options[OPT_MINKTIDE].cDimension, "nd");
  fvFormattedString(&options[OPT_MINKTIDE].cDefault, "0.1");
  options[OPT_MINKTIDE].iType      = 2;
  options[OPT_MINKTIDE].dDefault   = 0.1;
  options[OPT_MINKTIDE].bMultiFile = 1;
  fnRead[OPT_MINKTIDE]             = &ReadMinKTide;
}

/**
Loops through the input files and reads all user options for the atmospheric
escape model.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param fnRead Array of pointers to the functions that read in the options
@param iBody The current BODY number
*/
void ReadOptionsAtmEsc(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, fnReadOption fnRead[],
                       int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTATMESC; iOpt < OPTENDATMESC; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}

/******************* Verify ATMESC ******************/

/**
Initializes the differential equation matrix for the surface water mass.

@param body A pointer to the current BODY instance
@param options A pointer to the OPTIONS instance
@param update A pointer to the UPDATE instance
@param dAge The current age of the system
@param iBody The current BODY number
*/
void VerifySurfaceWaterMass(BODY *body, OPTIONS *options, UPDATE *update,
                            double dAge, int iBody) {

  update[iBody].iaType[update[iBody].iSurfaceWaterMass][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iSurfaceWaterMass][0] = 1;
  update[iBody].iaBody[update[iBody].iSurfaceWaterMass][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iSurfaceWaterMass][0] *
               sizeof(int));
  update[iBody].iaBody[update[iBody].iSurfaceWaterMass][0][0] = iBody;

  update[iBody].pdDSurfaceWaterMassDtAtmesc =
        &update[iBody].daDerivProc[update[iBody].iSurfaceWaterMass][0];
}

/**
Initializes the differential equation matrix for the atmospheric oxygen mass.

@param body A pointer to the current BODY instance
@param options A pointer to the OPTIONS instance
@param update A pointer to the UPDATE instance
@param dAge The current age of the system
@param iBody The current BODY number
*/
void VerifyOxygenMass(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                      int iBody) {

  update[iBody].iaType[update[iBody].iOxygenMass][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iOxygenMass][0] = 1;
  update[iBody].iaBody[update[iBody].iOxygenMass][0]     = malloc(
        update[iBody].iNumBodies[update[iBody].iOxygenMass][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iOxygenMass][0][0] = iBody;

  update[iBody].pdDOxygenMassDtAtmesc =
        &update[iBody].daDerivProc[update[iBody].iOxygenMass][0];
}

/**
Initializes the differential equation matrix for the mantle oxygen mass.

@param body A pointer to the current BODY instance
@param options A pointer to the OPTIONS instance
@param update A pointer to the UPDATE instance
@param dAge The current age of the system
@param iBody The current BODY number
*/
void VerifyOxygenMantleMass(BODY *body, OPTIONS *options, UPDATE *update,
                            double dAge, int iBody) {

  update[iBody].iaType[update[iBody].iOxygenMantleMass][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iOxygenMantleMass][0] = 1;
  update[iBody].iaBody[update[iBody].iOxygenMantleMass][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iOxygenMantleMass][0] *
               sizeof(int));
  update[iBody].iaBody[update[iBody].iOxygenMantleMass][0][0] = iBody;

  update[iBody].pdDOxygenMantleMassDtAtmesc =
        &update[iBody].daDerivProc[update[iBody].iOxygenMantleMass][0];
}

/**
Initializes the differential equation matrix for the gaseous envelope mass.

@param body A pointer to the current BODY instance
@param options A pointer to the OPTIONS instance
@param update A pointer to the UPDATE instance
@param dAge The current age of the system
@param iBody The current BODY number
*/
void VerifyEnvelopeMass(BODY *body, OPTIONS *options, UPDATE *update,
                        double dAge, int iBody) {

  update[iBody].iaType[update[iBody].iEnvelopeMass][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iEnvelopeMass][0] = 1;
  update[iBody].iaBody[update[iBody].iEnvelopeMass][0]     = malloc(
        update[iBody].iNumBodies[update[iBody].iEnvelopeMass][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iEnvelopeMass][0][0] = iBody;

  update[iBody].pdDEnvelopeMassDtAtmesc =
        &update[iBody].daDeriv[update[iBody].iEnvelopeMass];
}

/**
Initializes the differential equation matrix for the planet mass.

@param body A pointer to the current BODY instance
@param options A pointer to the OPTIONS instance
@param update A pointer to the UPDATE instance
@param dAge The current age of the system
@param iBody The current BODY number
*/
void VerifyMassAtmEsc(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                      int iBody) {

  update[iBody].iaType[update[iBody].iMass][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iMass][0] = 1;
  update[iBody].iaBody[update[iBody].iMass][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iMass][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iMass][0][0] = iBody;

  update[iBody].pdDMassDtAtmesc =
        &update[iBody].daDerivProc[update[iBody].iMass][0];
}

/**
Initializes the differential equation matrix for the planet radius.

@param body A pointer to the current BODY instance
@param control A pointer to the CONTROL instance
@param options A pointer to the OPTIONS instance
@param update A pointer to the UPDATE instance
@param dAge The current age of the system
@param iBody The current BODY number
*/
void VerifyRadiusAtmEsc(BODY *body, CONTROL *control, OPTIONS *options,
                        UPDATE *update, double dAge, int iBody) {

  // Assign radius
  if (body[iBody].iPlanetRadiusModel == ATMESC_LOP12) {
    body[iBody].dRadius = fdLopezRadius(
          body[iBody].dMass, body[iBody].dEnvelopeMass / body[iBody].dMass, 1.,
          body[iBody].dAge, 0);

    // If there is no envelope and Lopez Radius specified, use Sotin+2007
    // radius!
    if (body[iBody].dEnvelopeMass <= body[iBody].dMinEnvelopeMass) {
      if (control->Io.iVerbose >= VERBINPUT) {
        printf("INFO: Lopez+2012 Radius model specified, but no envelope "
               "present. Using Sotin+2007 Mass-radius relation to compute "
               "planet's solid radius.\n");
      }

      // Set radius using Sotin+2007 model
      body[iBody].dRadius = fdMassToRad_Sotin07(body[iBody].dMass);
    }

    if (options[OPT_RADIUS].iLine[iBody + 1] >= 0) {
      // User specified radius, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) {
        printf("INFO: Radius set for body %d, but this value will be computed "
               "from the grid.\n",
               iBody);
      }
    }
  } else if (body[iBody].iPlanetRadiusModel == ATMESC_PROXCENB) {
    body[iBody].dRadius =
          fdProximaCenBRadius(body[iBody].dEnvelopeMass / body[iBody].dMass,
                              body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_RADIUS].iLine[iBody + 1] >= 0) {
      // User specified radius, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) {
        printf("INFO: Radius set for body %d, but this value will be computed "
               "from the grid.\n",
               iBody);
      }
    }
  }

  update[iBody].iaType[update[iBody].iRadius][0]     = 0;
  update[iBody].iNumBodies[update[iBody].iRadius][0] = 1;
  update[iBody].iaBody[update[iBody].iRadius][0]     = malloc(
        update[iBody].iNumBodies[update[iBody].iRadius][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iRadius][0][0] = iBody;

  update[iBody].pdRadiusAtmesc =
        &update[iBody]
               .daDerivProc[update[iBody].iRadius]
                           [0]; // NOTE: This points to the VALUE of the radius
}

void EnvelopeLost(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                  fnUpdateVariable ***fnUpdate, int iBody) {
  body[iBody].iHEscapeRegime                      = ATMESC_NONE;
  body[iBody].dEnvelopeMass                       = 0.;
  body[iBody].dEnvMassDt                          = 0.0;
  fnUpdate[iBody][update[iBody].iEnvelopeMass][0] = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iMass][0]         = &fndUpdateFunctionTiny;

  // printf("Envelope Lost!");

  // Let user know what's happening
  if (io->iVerbose >= VERBPROG && !body[iBody].bEnvelopeLostMessage) {
    printf("%s's envelope removed after %.3lf million years. ",
           body[iBody].cName, evolve->dTime / (1e6 * YEARSEC));
    if (body[iBody].iPlanetRadiusModel == ATMESC_LOP12) {
      printf("Switching to Sotin+2007 model for solid planet radius.\n");
    } else {
      printf("\n");
    }
    body[iBody].bEnvelopeLostMessage = 1;
  }

  // Update mass and radius
  body[iBody].dMass = body[iBody].dSolidMass;
  // If using Lopez+2012 radius model, set radius to Sotin+2007 radius
  if (body[iBody].iPlanetRadiusModel == ATMESC_LOP12) {
    body[iBody].dRadius = fdMassToRad_Sotin07(body[iBody].dMass);
  }
}

double fdAtmEscXi(BODY *body, int iBody) {
  double dXi =
        body[iBody].dRocheRadius / (body[iBody].dRadius * body[iBody].dXFrac);
  return dXi;
}

double fdKTide(BODY *body, IO *io, int iNumBodies, int iBody) {
  double dKTide;

  // For stars and circumbinary planets, assume no Ktide enhancement
  if (body[iBody].bBinary && body[iBody].iBodyType == 0) {
    dKTide = 1.0;
  } else {
    if (iNumBodies > 1) {
      if (body[iBody].dAtmEscXi > 1) {
        dKTide = (1 - 3 / (2 * body[iBody].dAtmEscXi) +
                  1 / (2 * pow(body[iBody].dAtmEscXi, 3)));
        if (dKTide < body[iBody].dMinKTide) {
          dKTide = body[iBody].dMinKTide;
        }
      } else {
        if (!io->baRocheMessage[iBody] && io->iVerbose >= VERBINPUT &&
            (!body[iBody].bUseBondiLimited && !body[iBody].bAtmEscAuto)) {
          fprintf(stderr,
                  "WARNING: Roche lobe radius is larger than %s's XUV radius. "
                  "Evolution may not be accurate.\n",
                  body[iBody].cName);
          fprintf(stderr, "Consider setting bUseBondiLimited = 1 or bAtmEscAuto "
                          "= 1 to limit envelope mass loss.\n");
          io->baRocheMessage[iBody] = 1;
        }
        // Fix dKTide to prevent infs when in Roche Lobe overflow
        dKTide = 1.0;
      }
    } else {
      dKTide = 1.0;
    }
  }

  return dKTide;
}

/**
If necessary, change how the code handle hydrogen envelope escape

@param body A pointer to the current BODY instance
@param module A pointer to the MODULE instance
@param evolve A pointer to the EVOLVE instance
@param io A pointer to the IO instance
@param system A pointer to the SYSTEM instance
@param update A pointer to the UPDATE instance
@param fnUpdate A triple-pointer to the function that updates each variable
@param iBody The current BODY number
@param iModule The current MODULE number
*/
void ForceBehaviorEnvelopeEscape(BODY *body, MODULE *module, EVOLVE *evolve,
                                 IO *io, SYSTEM *system, UPDATE *update,
                                 fnUpdateVariable ***fnUpdate, int iBody,
                                 int iModule) {

  // If time > jeans time, transition to ballistic regime and halt the escape
  if ((body[iBody].dEnvelopeMass > body[iBody].dMinEnvelopeMass) &&
      (body[iBody].dAge > body[iBody].dJeansTime) &&
      (body[iBody].iHEscapeRegime != ATMESC_BALLISTIC)) {
    // Reassign regime and derivatives to prevent further evolution
    body[iBody].iHEscapeRegime                      = ATMESC_BALLISTIC;
    fnUpdate[iBody][update[iBody].iEnvelopeMass][0] = &fndUpdateFunctionTiny;
    fnUpdate[iBody][update[iBody].iMass][0]         = &fndUpdateFunctionTiny;
  }

  /* In some cases, the final mass loss of an envelope can become very large,
     resulting in the apparent loss of the solid planet. In those cases, set the
     envelope mass to 0, mass to dSolidMass, and prevent envelope loss.
  if (body[iBody].dEnvelopeMass < 0) {
    body[iBody].dMass = body[iBody].dSolidMass;
    EnvelopeLost(body,evolve,io,update,fnUpdate,iBody);
  }
  */

  // If envelope is below minimum value, but still present, set its mass to 0
  // and prevent further evolution
  /*
  if ((body[iBody].dEnvelopeMass <= body[iBody].dMinEnvelopeMass) &&
      (body[iBody].dEnvelopeMass > 0.)) {
  */
  if (body[iBody].dEnvelopeMass <= body[iBody].dMinEnvelopeMass) {
    // Let's remove its envelope and prevent further evolution.
    EnvelopeLost(body, evolve, io, update, fnUpdate, iBody);
  }

  // if (body[iBody].dEnvelopeMass == 0) {
  //   fprintf(stderr,"Planet %s's envelope lost!",body[iBody].cName);
  // }

  // Using variable evolution: determine proper escape regime and set
  // H envelope mass loss derivatives accordingly (if H envelope exists)
  if (body[iBody].bAtmEscAuto &&
      body[iBody].dEnvelopeMass > body[iBody].dMinEnvelopeMass &&
      body[iBody].iHEscapeRegime != ATMESC_NONE) {
    // If currently energy-limited, see if we should switch to another regime
    if (body[iBody].iHEscapeRegime == ATMESC_ELIM) {
      // Is the flux RR-limited?
      if (fbRRCriticalFlux(body, iBody)) {
        // Switch regime, derivatives
        if (io->iVerbose >= VERBPROG) {
          fvAtmEscRegimeChangeOutput(body[iBody].iHEscapeRegime, ATMESC_RRLIM,
                                     evolve->dTime / (1e6 * YEARSEC));
        }
        body[iBody].iHEscapeRegime = ATMESC_RRLIM;
        fnUpdate[iBody][update[iBody].iEnvelopeMass][0] =
              &fdDEnvelopeMassDtRRLimited;
        fnUpdate[iBody][update[iBody].iMass][0] = &fdDEnvelopeMassDtRRLimited;
      }
      // Is the flux Bondi-limited?
      if (fbBondiCriticalDmDt(body, iBody)) {
        // Switch regime, derivatives
        if (io->iVerbose >= VERBPROG) {
          fvAtmEscRegimeChangeOutput(body[iBody].iHEscapeRegime,
                                     ATMESC_BONDILIM,
                                     evolve->dTime / (1e6 * YEARSEC));
        }
        body[iBody].iHEscapeRegime = ATMESC_BONDILIM;
        fnUpdate[iBody][update[iBody].iEnvelopeMass][0] =
              &fdDEnvelopeMassDtBondiLimited;
        fnUpdate[iBody][update[iBody].iMass][0] =
              &fdDEnvelopeMassDtBondiLimited;
      }
    }
    // If currently RR-limited, see if we should switch to another regime
    else if (body[iBody].iHEscapeRegime == ATMESC_RRLIM) {
      // Is the escape now energy-limited?
      if (!fbRRCriticalFlux(body, iBody)) {
        // Switch regime, derivatives
        if (io->iVerbose >= VERBPROG) {
          fvAtmEscRegimeChangeOutput(body[iBody].iHEscapeRegime, ATMESC_ELIM,
                                     evolve->dTime / (1e6 * YEARSEC));
        }
        body[iBody].iHEscapeRegime                      = ATMESC_ELIM;
        fnUpdate[iBody][update[iBody].iEnvelopeMass][0] = &fdDEnvelopeMassDt;
        fnUpdate[iBody][update[iBody].iMass][0]         = &fdDEnvelopeMassDt;
      }

      // Is the escape now Bondi-limited?
      if (fbBondiCriticalDmDt(body, iBody)) {
        // Switch regime, derivatives
        if (io->iVerbose >= VERBPROG) {
          fvAtmEscRegimeChangeOutput(body[iBody].iHEscapeRegime,
                                     ATMESC_BONDILIM,
                                     evolve->dTime / (1e6 * YEARSEC));
        }
        body[iBody].iHEscapeRegime = ATMESC_BONDILIM;
        fnUpdate[iBody][update[iBody].iEnvelopeMass][0] =
              &fdDEnvelopeMassDtBondiLimited;
        fnUpdate[iBody][update[iBody].iMass][0] =
              &fdDEnvelopeMassDtBondiLimited;
      }
    }
    // Flux currently Bondi-limited, but should we switch?
    else if (body[iBody].iHEscapeRegime == ATMESC_BONDILIM) {
      // No longer Bondi-limited, see if EL or RR-limited
      if (!fbBondiCriticalDmDt(body, iBody)) {
        // RR-limited?
        if (fbRRCriticalFlux(body, iBody)) {
          // Switch regime, derivatives
          if (io->iVerbose >= VERBPROG) {
            fvAtmEscRegimeChangeOutput(body[iBody].iHEscapeRegime, ATMESC_RRLIM,
                                       evolve->dTime / (1e6 * YEARSEC));
          }
          body[iBody].iHEscapeRegime = ATMESC_RRLIM;
          fnUpdate[iBody][update[iBody].iEnvelopeMass][0] =
                &fdDEnvelopeMassDtRRLimited;
          fnUpdate[iBody][update[iBody].iMass][0] = &fdDEnvelopeMassDtRRLimited;
        }
        // Energy-limited!
        else {
          // Switch regime, derivatives
          if (io->iVerbose >= VERBPROG) {
            fvAtmEscRegimeChangeOutput(body[iBody].iHEscapeRegime, ATMESC_ELIM,
                                       evolve->dTime / (1e6 * YEARSEC));
          }
          body[iBody].iHEscapeRegime                      = ATMESC_ELIM;
          fnUpdate[iBody][update[iBody].iEnvelopeMass][0] = &fdDEnvelopeMassDt;
          fnUpdate[iBody][update[iBody].iMass][0]         = &fdDEnvelopeMassDt;
        }
      }
    } else {
      // Undefined regime! Warn user and switch to energy-limited
      fprintf(stderr, "WARNING: Undefined iHEscapeRegime = %d for body %s!\n",
              body[iBody].iHEscapeRegime, body[iBody].cName);
      fprintf(stderr, "Switching to default energy-limited escape.\n");

      body[iBody].iHEscapeRegime                      = ATMESC_ELIM;
      fnUpdate[iBody][update[iBody].iEnvelopeMass][0] = &fdDEnvelopeMassDt;
      fnUpdate[iBody][update[iBody].iMass][0]         = &fdDEnvelopeMassDt;
    }
  }
}

/**
If necessary, change how the code handle hwater loss

@param body A pointer to the current BODY instance
@param module A pointer to the MODULE instance
@param evolve A pointer to the EVOLVE instance
@param io A pointer to the IO instance
@param system A pointer to the SYSTEM instance
@param update A pointer to the UPDATE instance
@param fnUpdate A triple-pointer to the function that updates each variable
@param iBody The current BODY number
@param iModule The current MODULE number
*/
void ForceBehaviorWaterEscape(BODY *body, MODULE *module, EVOLVE *evolve,
                              IO *io, SYSTEM *system, UPDATE *update,
                              fnUpdateVariable ***fnUpdate, int iBody,
                              int iModule) {

  //
  if ((body[iBody].dSurfaceWaterMass <= body[iBody].dMinSurfaceWaterMass) &&
      (body[iBody].dSurfaceWaterMass > 0.)) {
    // Let's desiccate this planet.
    body[iBody].dSurfaceWaterMass = 0.;
  }
}

/**
This function is run during every step of the integrator to
perform checks and force certain non-diffeq behavior.

@param body A pointer to the current BODY instance
@param module A pointer to the MODULE instance
@param evolve A pointer to the EVOLVE instance
@param io A pointer to the IO instance
@param system A pointer to the SYSTEM instance
@param update A pointer to the UPDATE instance
@param fnUpdate A triple-pointer to the function that updates each variable
@param iBody The current BODY number
@param iModule The current MODULE number
*/
void fnForceBehaviorAtmEsc(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                           SYSTEM *system, UPDATE *update,
                           fnUpdateVariable ***fnUpdate, int iBody,
                           int iModule) {

  if (body[iBody].dEnvelopeMass > 0) {
    ForceBehaviorEnvelopeEscape(body, module, evolve, io, system, update,
                                fnUpdate, iBody, iModule);
  } else if (body[iBody].dSurfaceWaterMass > 0) {
    ForceBehaviorWaterEscape(body, module, evolve, io, system, update, fnUpdate,
                             iBody, iModule);
  }
}

void AuxPropsLehmer17(BODY *body, EVOLVE *evolve, int iBody) {
  if (body[iBody].bAutoThermTemp) {
    body[iBody].dThermTemp = fdThermalTemp(body, iBody);
  }
  body[iBody].dGravAccel = BIGG *
                           (body[iBody].dMass - body[iBody].dEnvelopeMass) /
                           (body[iBody].dRadSolid * body[iBody].dRadSolid);
  body[iBody].dScaleHeight = body[iBody].dAtmGasConst * body[iBody].dThermTemp /
                             body[iBody].dGravAccel;
  body[iBody].dPresSurf =
        fdLehmerPres(body[iBody].dEnvelopeMass, body[iBody].dGravAccel,
                     body[iBody].dRadSolid);
  body[iBody].dRadXUV = fdLehmerRadius(body, evolve->iNumBodies, iBody);
  body[iBody].dRadius = body[iBody].dRadXUV / body[iBody].dXFrac;
}


/**
Initializes several helper variables and properties used in the integration.

@param body A pointer to the current BODY instance
@param evolve A pointer to the EVOLVE instance
@param update A pointer to the UPDATE instance
@param iBody The current BODY number
*/

// XXX A lot of this function should be moved to ForceBehavior! PropsAux is only
// for computing parameters that make the derivatives easier to interpret
void fnPropsAuxAtmEsc(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                      int iBody) {

  if (body[iBody].iPlanetRadiusModel == ATMESC_LEHMER17) {
    AuxPropsLehmer17(body, evolve, iBody);
  }

  // Compute various radii of interest
  body[iBody].dBondiRadius = fdBondiRadius(body, iBody);
  body[iBody].dRocheRadius = fdRocheRadius(body, evolve->iNumBodies, iBody);
  body[iBody].dAtmEscXi    = fdAtmEscXi(body, iBody);
  body[iBody].dKTide       = fdKTide(body, io, evolve->iNumBodies, iBody);

  // The XUV flux
  if (body[iBody].bCalcFXUV) {
    body[iBody].dFXUV = fdXUVFlux(body, iBody);
  }

  // The H2O XUV escape efficiency
  if (body[iBody].iAtmXAbsEffH2OModel == ATMESC_BOL16) {
    body[iBody].dAtmXAbsEffH2O = fdXUVEfficiencyBolmont2016(body[iBody].dFXUV);
  }

  // Reference hydrogen flux for the water loss
  // LS2016 uses reference equation 6 in paper. 1.00784 factor changes ATOMMASS
  // to mass of H (MTG)
  if (body[iBody].iWaterLossModel == ATMESC_LS2016) {
    body[iBody].dFHRef = (body[iBody].dAtmXAbsEffH2O * body[iBody].dFXUV *
                          body[iBody].dRadius) /
                         (4 * BIGG * body[iBody].dMass * PROTONMASS);
    // Use Below for LBEXACT with MGs diff limited treatment
    // body[iBody].dFHRef =
    //  (body[iBody].dAtmXAbsEffH2O * body[iBody].dFXUV * body[iBody].dRadius) /
    //(4 * BIGG * body[iBody].dMass * body[iBody].dKTide * ATOMMASS);
  } else {
    body[iBody].dFHRef =
          (body[iBody].dAtmXAbsEffH2O * body[iBody].dFXUV *
           body[iBody].dRadius) /
          (4 * BIGG * body[iBody].dMass * body[iBody].dKTide * ATOMMASS);
  }


  // Surface gravity
  double g = (BIGG * body[iBody].dMass) /
             (body[iBody].dRadius * body[iBody].dRadius);

  // Oxygen mixing ratio
  double XO = fdAtomicOxygenMixingRatio(body[iBody].dSurfaceWaterMass,
                                        body[iBody].dOxygenMass);

  // Calculate the free oxygen mixing ratio (not in water molecules) and water
  // mixing for switch to diffusion later on (MTG)
  // XXX Hack until this function can be refactored!
  double NFreeO, Nh2o, XFreeO, Xh2o, Xcompare;
  if (body[iBody].dSurfaceWaterMass > 0) {
    NFreeO   = body[iBody].dOxygenMass / (32 * ATOMMASS);
    Nh2o     = body[iBody].dSurfaceWaterMass / (18 * ATOMMASS);
    XFreeO   = (2 * NFreeO) / ((2 * NFreeO) + (Nh2o));
    Xh2o     = Nh2o / ((2 * NFreeO) + (Nh2o));
    Xcompare = Xh2o; // the mixing ratio XFreeO must be equal to or greater than
                     // for diffusion lim esc
  } else {
    NFreeO   = 0;
    Nh2o     = 0;
    XFreeO   = 0;
    Xh2o     = 0;
    Xcompare = 0;
  }

  // Diffusion-limited H escape rate
  double BDIFF = 4.8e19 * pow(body[iBody].dFlowTemp, 0.75);
  if (XO == 1) {
    body[iBody].dFHDiffLim = 0;
  } else {
  body[iBody].dFHDiffLim =
        BDIFF * g * ATOMMASS * (QOH - 1.) /
        (KBOLTZ * body[iBody].dFlowTemp * (1. + XO / (1. - XO)));
  }

  if (body[iBody].dOxygenMass == 0) {
    body[iBody].dOxygenEta = 0;
  }

  // Is water escaping?
  if (!fbDoesWaterEscape(body, evolve, io, iBody)) {

    body[iBody].dOxygenEta         = 0;
    body[iBody].dCrossoverMass     = 0;
    body[iBody].bRunaway           = 0;
    body[iBody].iWaterEscapeRegime = ATMESC_NONE;
    body[iBody].dMDotWater         = 0;

  } else {

    double FH = body[iBody].dFHRef;

    body[iBody].bRunaway = 1;

    // Select an escape/oxygen buildup model
    if (body[iBody].iWaterLossModel == ATMESC_LB15) {

      // Luger and Barnes (2015)
      double x = (KBOLTZ * body[iBody].dFlowTemp * body[iBody].dFHRef) /
                 (10 * BDIFF * g * ATOMMASS);
      if (x < 1) {
        body[iBody].dOxygenEta = 0;
        body[iBody].dCrossoverMass =
              ATOMMASS + 1.5 * KBOLTZ * body[iBody].dFlowTemp *
                               body[iBody].dFHRef / (BDIFF * g);
      } else {
        body[iBody].dOxygenEta = (x - 1) / (x + 8);
        body[iBody].dCrossoverMass =
              43. / 3. * ATOMMASS + KBOLTZ * body[iBody].dFlowTemp *
                                          body[iBody].dFHRef / (6 * BDIFF * g);
      }

    } else if ((body[iBody].iWaterLossModel == ATMESC_LBEXACT) ||
               (body[iBody].iWaterLossModel == ATMESC_TIAN)) {

      double x = (QOH - 1.) * (1. - XO) * (BDIFF * g * ATOMMASS) /
                 (KBOLTZ * body[iBody].dFlowTemp);
      double FH;
      double rat;

      // Get the crossover mass
      if (body[iBody].dFHRef < x) {

        // mcross < mo
        body[iBody].dCrossoverMass =
              ATOMMASS +
              (1. / (1. - XO)) *
                    (KBOLTZ * body[iBody].dFlowTemp * body[iBody].dFHRef) /
                    (BDIFF * g);
        FH  = body[iBody].dFHRef;
        /* Is this necessary? XXX
        rat = ((body[iBody].dCrossoverMass / ATOMMASS) - QOH) /
              ((body[iBody].dCrossoverMass / ATOMMASS) - 1.);
        body[iBody].dOxygenEta = 0;
        */
      } else {

        // mcross >= mo
        double num = 1. + (XO / (1. - XO)) * QOH * QOH;
        double den = 1. + (XO / (1. - XO)) * QOH;
        body[iBody].dCrossoverMass =
              ATOMMASS * num / den +
              (KBOLTZ * body[iBody].dFlowTemp * body[iBody].dFHRef) /
                    ((1 + XO * (QOH - 1)) * BDIFF * g);
        if (body[iBody].dCrossoverMass != ATOMMASS) {
          rat = (body[iBody].dCrossoverMass / ATOMMASS - QOH) /
              (body[iBody].dCrossoverMass / ATOMMASS - 1.);
          FH = body[iBody].dFHRef * pow(1. + (XO / (1. - XO)) * QOH * rat, -1);
          body[iBody].dOxygenEta = 2 * XO / (1. - XO) * rat;
        } else {
          body[iBody].dOxygenEta = 0;
        }        
      }
    } else if (body[iBody].iWaterLossModel == ATMESC_LS2016) {

      // Use below for LBEXACT equations with schaeffer diff lim
      double x = (QOH - 1.) * (1. - XO) * (BDIFF * g * ATOMMASS) /
                 (KBOLTZ * body[iBody].dFlowTemp);
      double FH;
      double rat;

      // Get the crossover mass
      if (body[iBody].dFHRef < x) {

        // mcross < mo
        body[iBody].dCrossoverMass =
              ATOMMASS +
              (1. / (1. - XO)) *
                    (KBOLTZ * body[iBody].dFlowTemp * body[iBody].dFHRef) /
                    (BDIFF * g);
      } else {

        // mcross >= mo
        double num = 1. + (XO / (1. - XO)) * QOH * QOH;
        double den = 1. + (XO / (1. - XO)) * QOH;
        body[iBody].dCrossoverMass =
              ATOMMASS * num / den +
              (KBOLTZ * body[iBody].dFlowTemp * body[iBody].dFHRef) /
                    ((1 + XO * (QOH - 1)) * BDIFF * g);
      }

      double GPotential =
            (BIGG * body[iBody].dMass * PROTONMASS) / (body[iBody].dRadius);

      double FXUVCritDrag = ((4 * BDIFF * pow(GPotential, 2)) /
                             (body[iBody].dAtmXAbsEffH2O * KBOLTZ *
                              body[iBody].dFlowTemp * body[iBody].dRadius)) *
                            (QOH - 1) * (1 - XO);

      if ((body[iBody].dCrossoverMass >= OXYMASS) &&
          (body[iBody].dFXUV >= FXUVCritDrag)) {
        rat = (body[iBody].dCrossoverMass / ATOMMASS - QOH) /
              (body[iBody].dCrossoverMass / ATOMMASS - 1.);
        FH = body[iBody].dFHRef * pow(1. + (XO / (1. - XO)) * QOH * rat, -1);
        body[iBody].dOxygenEta = 2 * XO / (1. - XO) * rat;
      } else {
        FH  = body[iBody].dFHRef;
        rat = ((body[iBody].dCrossoverMass / ATOMMASS) - QOH) /
              ((body[iBody].dCrossoverMass / ATOMMASS) - 1.);
        body[iBody].dOxygenEta = 0;
      }

      // Eq 5 Schaefer 2016
      // body[iBody].dCrossoverMass =
      //       ATOMMASS + ((KBOLTZ * body[iBody].dFlowTemp * body[iBody].dFHRef)
      //       / (BDIFF * g * (1-XO)));
      // Critical FXUV for drag to occur: (drag now requires u2 < uc and FXUV >=
      // FXUVCritDrag)
      /**double FXUVCritDrag = ((4 * BDIFF * pow(HPOT2, 2)) /
            (body[iBody].dAtmXAbsEffH2O * KBOLTZ * body[iBody].dFlowTemp *
      body[iBody].dRadius)) * (QOH - 1) * (1 - XO);

      if ((body[iBody].dCrossoverMass >= OXYMASS) &&
          (body[iBody].dFXUV >= FXUVCritDrag)) {
            //double aa = KBOLTZ * body[iBody].dFlowTemp * body[iBody].dFHRef;
            //double bb = (15*ATOMMASS) * BDIFF * g * XO * (1 - XO);
            //body[iBody].dOxygenEta = (((aa * XO) - bb) / ((aa * (1-XO)) +
      (QOH*bb))); double bigY = (KBOLTZ * body[iBody].dFlowTemp *
      body[iBody].dFHRef) /
                 ((QOH - 1) * BDIFF * g * (1-XO) * ATOMMASS);
            body[iBody].dOxygenEta = ((bigY - 1) / ((bigY * ((1 - XO) / XO)) +
      QOH)); if (body[iBody].dOxygenEta > 0.5){ body[iBody].dOxygenEta = 0.5;
            }
            FH = body[iBody].dFHRef / (1 + (QOH * (body[iBody].dOxygenEta)));
            body[iBody].dOxygenEta = body[iBody].dOxygenEta*2;
          } else {
            body[iBody].dOxygenEta = 0;
            FH = body[iBody].dFHRef;
          }*/
    }

    // if ((XO > 0.6) && (body[iBody].iWaterLossModel == ATMESC_LBEXACT)) {
    if ((body[iBody].dOxygenEta > 1) &&
        (body[iBody].iWaterLossModel == ATMESC_LBEXACT)) {
      // Schaefer et al. (2016) prescription, section 2.2
      // NOTE: Perhaps a better criterion is (body[iBody].dOxygenEta > 1),
      // which ensures oxygen never escapes faster than it is being produced?
      body[iBody].iWaterEscapeRegime = ATMESC_DIFFLIM;
      body[iBody].dOxygenEta         = 0;
      body[iBody].dMDotWater =
            body[iBody].dFHDiffLim *
            (4 * ATOMMASS * PI * body[iBody].dRadius * body[iBody].dRadius *
             body[iBody].dXFrac * body[iBody].dXFrac);
      //} else if ((XO <= 0.6) && (body[iBody].iWaterLossModel ==
      // ATMESC_LBEXACT)) {
    } else if ((body[iBody].dOxygenEta <= 1) &&
               (body[iBody].iWaterLossModel == ATMESC_LBEXACT)) {
      // In the Tian model, oxygen escapes when it's the dominant species. I
      // think this is wrong...
      body[iBody].iWaterEscapeRegime = ATMESC_ELIM;
      body[iBody].dMDotWater = FH * (4 * ATOMMASS * PI * body[iBody].dRadius *
                                     body[iBody].dRadius * body[iBody].dXFrac *
                                     body[iBody].dXFrac);
    } else if ((XFreeO >= Xcompare) &&
               (body[iBody].iWaterLossModel == ATMESC_LS2016)) {
      body[iBody].iWaterEscapeRegime = ATMESC_DIFFLIM;
      body[iBody].dOxygenEta         = 0;
      body[iBody].dMDotWater =
            body[iBody].dFHDiffLim *
            (4 * ATOMMASS * PI * body[iBody].dRadius * body[iBody].dRadius *
             body[iBody].dXFrac * body[iBody].dXFrac);
    } else if ((XFreeO < Xcompare) &&
               (body[iBody].iWaterLossModel == ATMESC_LS2016)) {
      body[iBody].iWaterEscapeRegime = ATMESC_ELIM;
      body[iBody].dMDotWater = FH * (4 * ATOMMASS * PI * body[iBody].dRadius *
                                     body[iBody].dRadius * body[iBody].dXFrac *
                                     body[iBody].dXFrac);
    } else {
      // In the Tian model, oxygen escapes when it's the dominant species. I
      // think this is wrong...
      body[iBody].iWaterEscapeRegime = ATMESC_ELIM;
      body[iBody].dMDotWater =
            body[iBody].dFHRef *
            (4 * ATOMMASS * PI * body[iBody].dRadius * body[iBody].dRadius *
             body[iBody].dXFrac * body[iBody].dXFrac);
    }
  }

  // Compute current H envelope mass loss (if the envelope exists)
  if (body[iBody].dEnvelopeMass >= body[iBody].dMinEnvelopeMass) {
    body[iBody].dEnvMassDt = *(update[iBody].pdDEnvelopeMassDtAtmesc);
  }
}


/**
Assigns functions returning the time-derivatives of each variable
to the magical matrix of function pointers.

@param body A pointer to the current BODY instance
@param evolve A pointer to the EVOLVE instance
@param update A pointer to the UPDATE instance
@param fnUpdate A triple-pointer to the function that updates each variable
@param iBody The current BODY number
*/
void AssignAtmEscDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody) {
  if (body[iBody].dSurfaceWaterMass > 0) {
    fnUpdate[iBody][update[iBody].iSurfaceWaterMass][0] =
          &fdDSurfaceWaterMassDt;
    fnUpdate[iBody][update[iBody].iOxygenMass][0] = &fdDOxygenMassDt;
    fnUpdate[iBody][update[iBody].iOxygenMantleMass][0] =
          &fdDOxygenMantleMassDt;
  }
  if (body[iBody].dEnvelopeMass > 0) {
    // Set derivative depending on regime
    // Energy-limited (or if, transition start as energy-limited)
    if (body[iBody].bUseEnergyLimited || body[iBody].bAtmEscAuto) {
      body[iBody].iHEscapeRegime                      = ATMESC_ELIM;
      fnUpdate[iBody][update[iBody].iEnvelopeMass][0] = &fdDEnvelopeMassDt;
      fnUpdate[iBody][update[iBody].iMass][0]         = &fdDEnvelopeMassDt;
    }
    // Bondi-limited escape
    else if (body[iBody].bUseBondiLimited) {
      body[iBody].iHEscapeRegime = ATMESC_BONDILIM;
      fnUpdate[iBody][update[iBody].iEnvelopeMass][0] =
            &fdDEnvelopeMassDtBondiLimited;
      fnUpdate[iBody][update[iBody].iMass][0] = &fdDEnvelopeMassDtBondiLimited;
    }
    // Radiation/recombination-limited escape
    else if (body[iBody].bUseRRLimited) {
      body[iBody].iHEscapeRegime = ATMESC_RRLIM;
      fnUpdate[iBody][update[iBody].iEnvelopeMass][0] =
            &fdDEnvelopeMassDtRRLimited;
      fnUpdate[iBody][update[iBody].iMass][0] = &fdDEnvelopeMassDtRRLimited;
    }
    // Default to energy-limited
    else {
      body[iBody].iHEscapeRegime                      = ATMESC_ELIM;
      fnUpdate[iBody][update[iBody].iEnvelopeMass][0] = &fdDEnvelopeMassDt;
      fnUpdate[iBody][update[iBody].iMass][0]         = &fdDEnvelopeMassDt;
    }
  }
  fnUpdate[iBody][update[iBody].iRadius][0] =
        &fdPlanetRadius; // NOTE: This points to the VALUE of the radius!
}

/**
Assigns null functions to the magical matrix of function pointers
for variables that will not get updated.

@param body A pointer to the current BODY instance
@param evolve A pointer to the EVOLVE instance
@param update A pointer to the UPDATE instance
@param fnUpdate A triple-pointer to the function that updates each variable
@param iBody The current BODY number
*/
void NullAtmEscDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                           fnUpdateVariable ***fnUpdate, int iBody) {
  if (body[iBody].dSurfaceWaterMass > 0) {
    fnUpdate[iBody][update[iBody].iSurfaceWaterMass][0] =
          &fndUpdateFunctionTiny;
    fnUpdate[iBody][update[iBody].iOxygenMass][0] = &fndUpdateFunctionTiny;
    fnUpdate[iBody][update[iBody].iOxygenMantleMass][0] =
          &fndUpdateFunctionTiny;
  }
  if (body[iBody].dEnvelopeMass > 0) {
    fnUpdate[iBody][update[iBody].iEnvelopeMass][0] = &fndUpdateFunctionTiny;
    fnUpdate[iBody][update[iBody].iMass][0]         = &fndUpdateFunctionTiny;
  }
  fnUpdate[iBody][update[iBody].iRadius][0] =
        &fndUpdateFunctionTiny; // NOTE: This points to the VALUE of the radius!
}

/**
Verify all the inputs for the atmospheric escape module.

@param body A pointer to the current BODY instance
@param control A pointer to the CONTROL instance
@param files A pointer to the FILES instance
@param options A pointer to the OPTIONS instance
@param output A pointer to the OUTPUT instance
@param system A pointer to the SYSTEM instance
@param update A pointer to the UPDATE instance
@param iBody The current BODY number
@param iModule The current MODULE number
*/
void VerifyAtmEsc(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  OUTPUT *output, SYSTEM *system, UPDATE *update, int iBody,
                  int iModule) {
  int bAtmEsc = 0;

  body[iBody].iHEscapeRegime =
        ATMESC_NONE; // Default to no H escape - updated if envelope is present
  body[iBody].bEnvelopeLostMessage = 0;
  body[iBody].dEnvMassDt = 0.0; // Assume no H envelope mass loss at first

  // Is FXUV specified in input file?
  if (options[OPT_FXUV].iLine[iBody + 1] > -1) {
    body[iBody].bCalcFXUV = 0;
  } else {
    body[iBody].bCalcFXUV = 1;
  }

  if (body[iBody].iPlanetRadiusModel == ATMESC_LEHMER17) {
    if (body[0].bStellar) {
      if (body[iBody].dEnvelopeMass > 0.5 * body[iBody].dMass) {
        fprintf(stderr,
                "ERROR: %s's Envelope mass is greater than 50%% of its total "
                "mass, which ",
                body[iBody].cName);
        fprintf(
              stderr,
              "is not allowed  for the Lehmer-Catling (2017) envelope model.\n");
        DoubleLineExit(files->Infile[iBody + 1].cIn, files->Infile[iBody + 1].cIn,
                      options[OPT_ENVELOPEMASS].iLine[iBody + 1],
                      options[OPT_MASS].iLine[iBody + 1]);
      }
      if (body[iBody].dEnvelopeMass >= 0.1 * body[iBody].dMass) {
        fprintf(
              stderr,
              "WARNING: Envelope masses more than 10%% of the total mass are not "
              "recommended for the Lehmer-Catling (2017) envelope model. %s's "
              "envelope ",
              body[iBody].cName);
        fprintf(stderr, "mass exceeds this threshold.\n");
      }

      // Calculate auxiliary properties
      body[iBody].dRadSolid = fdMassToRad_LehmerCatling17(body[iBody].dMass - body[iBody].dEnvelopeMass);
      AuxPropsLehmer17(body,&(control->Evolve), iBody);      
    } else {
      fprintf(stderr,
              "ERROR: The Lehmer & Catling (2017) model requires a star.\n");
      exit(EXIT_INPUT);
    }
  } else {
    int iCol, bError = 0;
    for (iCol = 0; iCol < files->Outfile[iBody].iNumCols; iCol++) {
      // if (memcmp(files->Outfile[iBody].caCol[iCol],
      //            output[OUT_PLANETRADXUV].cName,
      //            strlen(files->Outfile[iBody].caCol[iCol])) == 0) {
        if (strcmp(files->Outfile[iBody].caCol[iCol],output[OUT_PLANETRADXUV].cName) == 0) {
        /* Match! */
        fprintf(stderr,
                "ERROR: Output option %s only allowed with AtmEsc's LEHMER17 "
                "model.\n",
                output[OUT_PLANETRADXUV].cName);
        bError = 1;
      }
      // if (memcmp(files->Outfile[iBody].caCol[iCol], output[OUT_RADSOLID].cName,
      //            strlen(files->Outfile[iBody].caCol[iCol])) == 0) {
        if (strcmp(files->Outfile[iBody].caCol[iCol], output[OUT_RADSOLID].cName) == 0) {
        /* Match! */
        fprintf(stderr,
                "ERROR: Output option %s only allowed with AtmEsc's LEHMER17 "
                "model.\n",
                output[OUT_RADSOLID].cName);
        bError = 1;
      }
      // if (memcmp(files->Outfile[iBody].caCol[iCol],
      //            output[OUT_SCALEHEIGHT].cName,
      //            strlen(files->Outfile[iBody].caCol[iCol])) == 0) {
        if (strcmp(files->Outfile[iBody].caCol[iCol],output[OUT_SCALEHEIGHT].cName) == 0) {
        /* Match! */
        fprintf(stderr,
                "ERROR: Output option %s only allowed with AtmEsc's LEHMER17 "
                "model.\n",
                output[OUT_SCALEHEIGHT].cName);
        bError = 1;
      }
      // if (memcmp(files->Outfile[iBody].caCol[iCol], output[OUT_PRESSURF].cName,
      //            strlen(files->Outfile[iBody].caCol[iCol])) == 0) {
        if (strcmp(files->Outfile[iBody].caCol[iCol], output[OUT_PRESSURF].cName) == 0) {
        /* Match! */
        fprintf(stderr,
                "ERROR: Output option %s only allowed with AtmEsc's LEHMER17 "
                "model.\n",
                output[OUT_PRESSURF].cName);
        bError = 1;
      }
    }

    if (bError) {
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_OUTPUTORDER].iLine[iBody + 1]);
    }
    /* Must initialized the above values to avoid memory leaks. */
    body[iBody].dRadXUV      = -1;
    body[iBody].dRadSolid    = -1;
    body[iBody].dScaleHeight = -1;
    body[iBody].dPresSurf    = -1;
  }
  // Should there be additional checks for PROXCEN and NONE???


  if (body[iBody].dSurfaceWaterMass > 0) {
    VerifySurfaceWaterMass(body, options, update, body[iBody].dAge, iBody);
    VerifyOxygenMass(body, options, update, body[iBody].dAge, iBody);
    VerifyOxygenMantleMass(body, options, update, body[iBody].dAge, iBody);
    bAtmEsc = 1;
  }

  // If envelope is present, pick correct derivative and ensure settings
  // conflicts don't exist, i.e. can't set energy and Bondi-limited escape!
  if (body[iBody].dEnvelopeMass > 0) {

    // Ensure only 1 escape regime is set
    int iRegimeCounter = 0;

    if (body[iBody].bUseEnergyLimited) {
      iRegimeCounter += 1;
    }
    if (body[iBody].bUseRRLimited) {
      iRegimeCounter += 1;
    }
    if (body[iBody].bUseBondiLimited) {
      iRegimeCounter += 1;
    }
    if (body[iBody].bAtmEscAuto) {
      iRegimeCounter += 1;
    }

    // If more than one is set, let the user know what's wrong and quit.
    if (iRegimeCounter > 1) {
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(
              stderr,
              "ERROR: Multiple H envelope atmospheric regimes are enabled for "
              "body %s!\n",
              body[iBody].cName);
        fprintf(stderr, "\tbUseEnergyLimited = %d\n",
                body[iBody].bUseEnergyLimited);
        fprintf(stderr, "\tbUseRRLimited = %d\n", body[iBody].bUseRRLimited);
        fprintf(stderr, "\tbUseBondiLimited = %d\n",
                body[iBody].bUseBondiLimited);
        fprintf(stderr, "\tbAtmEscAuto = %d\n", body[iBody].bAtmEscAuto);
      }
      exit(EXIT_INPUT);
    } else if (iRegimeCounter == 0) {
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr,
                "WARNING: No H envelope escape regime set for body %s!\n",
                body[iBody].cName);
        fprintf(
              stderr,
              "Defaulting to energy-limited escape: bUseEnergyLimited = 1.\n");
      }
      body[iBody].bUseEnergyLimited = 1;
    }

    // Verify and set derivatives (including correct H envelope escape regime!)
    VerifyEnvelopeMass(body, options, update, body[iBody].dAge, iBody);
    VerifyMassAtmEsc(body, options, update, body[iBody].dAge, iBody);
    bAtmEsc = 1;
  } else {
    // No H enevelope but Bondi Limited escape is set - warn user and use
    // energy-limited escape
    if (body[iBody].bUseBondiLimited || body[iBody].bUseRRLimited ||
        body[iBody].bAtmEscAuto) {
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr,
                "WARNING: No H envelope present but "
                "Bondi/Radiation-recombination-limited escape is set for body "
                "%s!\n",
                body[iBody].cName);
      }
      fprintf(stderr, "AtmEsc currently supports only energy-limited escape "
                      "for H20 loss calculations.");
    }
    // Point pdDEnvelopeMassDtAtmesc to zero. This should be done better.
    update[iBody].pdDEnvelopeMassDtAtmesc = &update[iBody].dZero;
  }

  // Ensure envelope mass is physical
  if (body[iBody].dEnvelopeMass > body[iBody].dMass) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr, "ERROR: %s cannot be greater than %s in file %s.\n",
              options[OPT_ENVELOPEMASS].cName, options[OPT_MASS].cName,
              files->Infile[iBody + 1].cIn);
    }
    exit(EXIT_INPUT);
  }

  // Initialize rg duration
  body[iBody].dRGDuration = 0.;

  if (!bAtmEsc && control->Io.iVerbose >= VERBINPUT) {
    fprintf(stderr,
            "WARNING: AtmEsc called for body %s, but no atmosphere/water "
            "present!\n",
            body[iBody].cName);
  }

  // Radius evolution
  if (update[iBody].iNumRadius > 1) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(
            stderr,
            "ERROR: More than one module is trying to set dRadius for body %d!",
            iBody);
    }
    exit(EXIT_INPUT);
  }

  // If envelope mass exists, compute mass of the solid planet
  if (body[iBody].dEnvelopeMass > 0) {
    body[iBody].dSolidMass = body[iBody].dMass - body[iBody].dEnvelopeMass;
  }

  // Setup radius and other radii of interest
  VerifyRadiusAtmEsc(body, control, options, update, body[iBody].dAge, iBody);
  body[iBody].dBondiRadius = fdBondiRadius(body, iBody);
  body[iBody].dRocheRadius = fdRocheRadius(body, control->Evolve.iNumBodies, iBody);

  control->fnForceBehavior[iBody][iModule]   = &fnForceBehaviorAtmEsc;
  control->fnPropsAux[iBody][iModule]        = &fnPropsAuxAtmEsc;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyAtmEsc;
}

/**************** ATMESC update ****************/

/**
Internal housekeeping function that determines
which variables get updated every time step.

@param body A pointer to the current BODY instance
@param update A pointer to the UPDATE instance
@param iBody The current BODY number
*/
void InitializeUpdateAtmEsc(BODY *body, UPDATE *update, int iBody) {
  if (body[iBody].dSurfaceWaterMass > 0) {
    if (update[iBody].iNumSurfaceWaterMass == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumSurfaceWaterMass++;

    if (update[iBody].iNumOxygenMass == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumOxygenMass++;

    if (update[iBody].iNumOxygenMantleMass == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumOxygenMantleMass++;
  }

  if (body[iBody].dEnvelopeMass > 0) {
    if (update[iBody].iNumEnvelopeMass == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumEnvelopeMass++;

    if (update[iBody].iNumMass == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumMass++;
  }

  if (body[iBody].dRadius > 0) {
    if (update[iBody].iNumRadius == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumRadius++;
  }
}

/**
Internal housekeeping function that determines
which variables get updated every time step.

@param body A pointer to the current BODY instance
@param update A pointer to the UPDATE instance
@param iEqn The current equation number
@param iVar The current variable number
@param iBody The current BODY number
@param iFoo ?!
*/
void FinalizeUpdateEccAtmEsc(BODY *body, UPDATE *update, int *iEqn, int iVar,
                             int iBody, int iFoo) {
  /* Nothing */
}

/**
Internal housekeeping function that determines
which variables get updated every time step.

@param body A pointer to the current BODY instance
@param update A pointer to the UPDATE instance
@param iEqn The current equation number
@param iVar The current variable number
@param iBody The current BODY number
@param iFoo ?!
*/
void FinalizeUpdateSurfaceWaterMassAtmEsc(BODY *body, UPDATE *update, int *iEqn,
                                          int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumSurfaceWaterMass  = (*iEqn)++;
}

/**
Internal housekeeping function that determines
which variables get updated every time step.

@param body A pointer to the current BODY instance
@param update A pointer to the UPDATE instance
@param iEqn The current equation number
@param iVar The current variable number
@param iBody The current BODY number
@param iFoo ?!
*/
void FinalizeUpdateOxygenMassAtmEsc(BODY *body, UPDATE *update, int *iEqn,
                                    int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumOxygenMass        = (*iEqn)++;
}

/**
Internal housekeeping function that determines
which variables get updated every time step.

@param body A pointer to the current BODY instance
@param update A pointer to the UPDATE instance
@param iEqn The current equation number
@param iVar The current variable number
@param iBody The current BODY number
@param iFoo ?!
*/
void FinalizeUpdateOxygenMantleMassAtmEsc(BODY *body, UPDATE *update, int *iEqn,
                                          int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumOxygenMantleMass  = (*iEqn)++;
}

/**
Internal housekeeping function that determines
which variables get updated every time step.

@param body A pointer to the current BODY instance
@param update A pointer to the UPDATE instance
@param iEqn The current equation number
@param iVar The current variable number
@param iBody The current BODY number
@param iFoo ?!
*/
void FinalizeUpdateEnvelopeMassAtmEsc(BODY *body, UPDATE *update, int *iEqn,
                                      int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumEnvelopeMass      = (*iEqn)++;
}

/**
Internal housekeeping function that determines
which variables get updated every time step.

@param body A pointer to the current BODY instance
@param update A pointer to the UPDATE instance
@param iEqn The current equation number
@param iVar The current variable number
@param iBody The current BODY number
@param iFoo ?!
*/
void FinalizeUpdateMassAtmEsc(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumMass              = (*iEqn)++;
}

/**
Internal housekeeping function that determines
which variables get updated every time step.

@param body A pointer to the current BODY instance
@param update A pointer to the UPDATE instance
@param iEqn The current equation number
@param iVar The current variable number
@param iBody The current BODY number
@param iFoo ?!
*/
void FinalizeUpdateOblAtmEsc(BODY *body, UPDATE *update, int *iEqn, int iVar,
                             int iBody, int iFoo) {
  /* Nothing */
}

/**
Internal housekeeping function that determines
which variables get updated every time step.

@param body A pointer to the current BODY instance
@param update A pointer to the UPDATE instance
@param iEqn The current equation number
@param iVar The current variable number
@param iBody The current BODY number
@param iFoo ?!
*/
void FinalizeUpdateRotAtmEsc(BODY *body, UPDATE *update, int *iEqn, int iVar,
                             int iBody, int iFoo) {
  /* Nothing */
}

/**
Internal housekeeping function that determines
which variables get updated every time step.

@param body A pointer to the current BODY instance
@param update A pointer to the UPDATE instance
@param iEqn The current equation number
@param iVar The current variable number
@param iBody The current BODY number
@param iFoo ?!
*/
void FinalizeUpdateSemiAtmEsc(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  /* Nothing */
}

/**
Internal housekeeping function that determines
which variables get updated every time step.

@param body A pointer to the current BODY instance
@param update A pointer to the UPDATE instance
@param iEqn The current equation number
@param iVar The current variable number
@param iBody The current BODY number
@param iFoo ?!
*/
void FinalizeUpdateRadiusAtmEsc(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumRadius            = (*iEqn)++;
}

/***************** ATMESC Halts *****************/

/**
Checks for surface desiccation and halts if necessary.

@param body A pointer to the current BODY instance
@param evolve A pointer to the EVOLVE instance
@param halt A pointer to the HALT instance
@param io A pointer to the IO instance
@param update A pointer to the UPDATE instance
@param iBody The current BODY number
*/
int fbHaltSurfaceDesiccated(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                            UPDATE *update, fnUpdateVariable ***fnUpdate,
                            int iBody) {

  if (body[iBody].dSurfaceWaterMass <= body[iBody].dMinSurfaceWaterMass) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's surface water mass =  ", body[iBody].cName);
      fprintd(stdout, body[iBody].dSurfaceWaterMass / TOMASS, io->iSciNot,
              io->iDigits);
      printf("TO.\n");
    }
    return 1;
  }
  return 0;
}

/**
Checks for envelope evaporation and halts if necessary.

@param body A pointer to the current BODY instance
@param evolve A pointer to the EVOLVE instance
@param halt A pointer to the HALT instance
@param io A pointer to the IO instance
@param update A pointer to the UPDATE instance
@param iBody The current BODY number
*/
int fbHaltEnvelopeGone(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                       UPDATE *update, fnUpdateVariable ***fnUpdate,
                       int iBody) {

  if (body[iBody].dEnvelopeMass <= body[iBody].dMinEnvelopeMass) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's envelope mass =  ", body[iBody].cName);
      fprintd(stdout, body[iBody].dEnvelopeMass / MEARTH, io->iSciNot,
              io->iDigits);
      printf("Earth Masses.\n");
    }
    return 1;
  }
  return 0;
}

/**
Count the number of halting conditions.

@param halt A pointer to the HALT instance
@param iHalt The current HALT number
*/
void CountHaltsAtmEsc(HALT *halt, int *iHalt) {
  if (halt->bSurfaceDesiccated) {
    (*iHalt)++;
  }
  if (halt->bEnvelopeGone) {
    (*iHalt)++;
  }
}

/**
Check whether the user wants to halt on certain conditions.

@param body A pointer to the current BODY instance
@param control A pointer to the CONTROL instance
@param options A pointer to the OPTIONS instance
@param iBody The current BODY number
@param iHalt The current HALT number
*/
void VerifyHaltAtmEsc(BODY *body, CONTROL *control, OPTIONS *options, int iBody,
                      int *iHalt) {

  if (control->Halt[iBody].bSurfaceDesiccated) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltSurfaceDesiccated;
  }

  if (control->Halt[iBody].bEnvelopeGone) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltEnvelopeGone;
  }
}

/************* ATMESC Outputs ******************/

/**
Logs the surface water mass.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteSurfaceWaterMass(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dSurfaceWaterMass;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

/**
Logs the H escape regime

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteHEscapeRegime(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char **cUnit) {
  *dTmp = body[iBody].iHEscapeRegime;
  fvFormattedString(cUnit, "");
}

/**
Logs the atmospheric oxygen mass.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteOxygenMass(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dOxygenMass;

  if (output->bDoNeg[iBody]) {
    *dTmp *= 1.e-5 * ((BIGG * body[iBody].dMass) /
                      (4. * PI * pow(body[iBody].dRadius, 4)));
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

/**
Logs Ktide, the gravitational enhancement of mass loss.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteKTide(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                UNITS *units, UPDATE *update, int iBody, double *dTmp,
                char **cUnit) {
  *dTmp = body[iBody].dKTide;

  fvFormattedString(cUnit, "");
}

/**
Logs the mantle oxygen mass.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteOxygenMantleMass(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dOxygenMantleMass;

  if (output->bDoNeg[iBody]) {
    *dTmp *= 1.e-5 * ((BIGG * body[iBody].dMass) /
                      (4. * PI * pow(body[iBody].dRadius, 4)));
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

/**
Logs the planet radius.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WritePlanetRadius(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dRadius;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

/**
Logs the envelope mass.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteEnvelopeMass(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dEnvelopeMass;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

/**
Logs the semi-major axis corresponding to the current runaway greenhouse limit.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteRGLimit(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char **cUnit) {

  // Get the RG flux
  double flux = fdHZRG14(body, iBody);

  // Convert to semi-major axis *at current eccentricity!*
  if (body[0].dLuminosity == 0) {
    *dTmp = -1;
  } else {
    *dTmp = pow(4 * PI * flux /
                (body[0].dLuminosity *
                pow((1 - body[iBody].dEcc * body[iBody].dEcc), 0.5)),
                -0.5);
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

/**
Logs the oxygen mixing ratio at the base of the hydrodynamic wind.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteOxygenMixingRatio(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char **cUnit) {
  *dTmp = fdAtomicOxygenMixingRatio(body[iBody].dSurfaceWaterMass,
                                    body[iBody].dOxygenMass);
  fvFormattedString(cUnit, "");
}

/**
Logs the oxygen eta parameter from Luger and Barnes (2015).

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteOxygenEta(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dOxygenEta;
  fvFormattedString(cUnit, "");
}

/**
Logs the XUV absorption efficiency for water.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteAtmXAbsEffH2O(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dAtmXAbsEffH2O;
  fvFormattedString(cUnit, "");
}

/**
Logs the planet's radius in the XUV.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WritePlanetRadXUV(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dRadXUV;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

/**
Logs the atmospheric mass loss rate.

\warning This routine is currently broken.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteDEnvMassDt(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char **cUnit) {
  *dTmp = *(update[iBody].pdDEnvelopeMassDtAtmesc);

  //*dTmp = fnUpdate[iBody][update[iBody].iEnvelopeMass][0];

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    //*dTmp *= fdUnitsTime(units->iTime)/fdUnitsMass(units->iMass);
    fvFormattedString(cUnit, "kg/s"); // XXX Why isn't the other line commented out???
  }
}

/**
Logs the thermospheric temperature.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteThermTemp(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dThermTemp;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    // System units are Kelvins
    *dTmp = fdUnitsTemp(*dTmp, U_KELVIN, units->iTemp);
    fsUnitsTemp(0, cUnit);
  }
}

/**
Logs the temperature of the flow.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteFlowTemp(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char **cUnit) {
  *dTmp = body[iBody].dFlowTemp;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    // System units are Kelvins
    *dTmp = fdUnitsTemp(*dTmp, U_KELVIN, units->iTemp);
    fsUnitsTemp(units->iTemp, cUnit);
  }
}

/**
Logs the surface pressure.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WritePresSurf(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char **cUnit) {
  *dTmp = body[iBody].dPresSurf;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    fvFormattedString(cUnit, "Pa"); // XXX
  }
}

/**
Logs the pressure at the XUV absorption radius.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WritePresXUV(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char **cUnit) {
  *dTmp = body[iBody].dPresXUV;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    //*dTmp /= fdUnitsPressure(units->iLength);
    // fsUnitsLength(units->iLength,cUnit);
  }
}

/**
Logs the time at which the flow transitioned to Jeans escape.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteJeansTime(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dJeansTime;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
  }
}

/**
Logs the atmospheric scale height.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteScaleHeight(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dScaleHeight;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

/**
Logs the gas constant.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteAtmGasConst(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dAtmGasConst;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
  }
}

/**
Logs the planet's solid radius.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteRadSolid(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char **cUnit) {
  *dTmp = body[iBody].dRadSolid;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

/**
Logs the planet's Roche radius.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteRocheRadius(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char **cUnit) {

  *dTmp = body[iBody].dRocheRadius;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}


/**
Logs the planet's Bondi radius.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteBondiRadius(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char **cUnit) {

  *dTmp = body[iBody].dBondiRadius;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

/**
Logs the XUV flux received by the planet.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteFXUV(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char **cUnit) {
  *dTmp = body[iBody].dFXUV;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    fvFormattedString(cUnit, "W/m^2");
  }
}

/**
Logs the critical XUV flux that separates the radiation/recombination-limited
and energy-limited H envelope escape regimes

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteRRCriticalFlux(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char **cUnit) {

  // Calculate critical flux for body
  *dTmp = fdRRCriticalFlux(body, iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    fvFormattedString(cUnit, "W/m^2");
  }
}

/**
Logs the crossover mass.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteCrossoverMass(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dCrossoverMass;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

/**
Logs the Water escape regime

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteWaterEscapeRegime(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char **cUnit) {
  *dTmp = body[iBody].iWaterEscapeRegime;
  fvFormattedString(cUnit, "");
}


/**
Logs the XUV flux received by the planet.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteFXUVCRITDRAG(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char **cUnit) {
  double BDIFF = 4.8e19 * pow(body[iBody].dFlowTemp, 0.75);
  double XO    = fdAtomicOxygenMixingRatio(body[iBody].dSurfaceWaterMass,
                                           body[iBody].dOxygenMass);
  double GPotential =
        (BIGG * body[iBody].dMass * PROTONMASS) / (body[iBody].dRadius);

  if (body[iBody].dAtmXAbsEffH2O > 0 && body[iBody].dFlowTemp > 0 && body[iBody].dRadius > 0) {
    *dTmp = ((4 * BDIFF * pow(GPotential, 2)) /
            (body[iBody].dAtmXAbsEffH2O * KBOLTZ * body[iBody].dFlowTemp *
              body[iBody].dRadius)) *
            (QOH - 1) * (1 - XO);
  } else {
    *dTmp = -1;
  }
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    fvFormattedString(cUnit, "W/m^2"); // XXX
  }
}

/**
Logs the Hydrogen Reference Flux

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteHREFFLUX(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char **cUnit) {
  *dTmp = body[iBody].dFHRef;
  fvFormattedString(cUnit, "m^-2 s^-1"); // XXX
}

/**
Logs the H flux if it was diffusion limited (or the true flux in the diffusion
limited regime)

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteHDiffLimFlux(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dFHDiffLim;
  fvFormattedString(cUnit, "m^-2 s^-1"); // XXX
}

/**
Modifier for H Ref Flux to include oxygen drag at a snapshot in time

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteHRefODragMod(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char **cUnit) {
  // XXX This should probably all just be moved into a function
  if (body[iBody].dCrossoverMass / ATOMMASS - 1. != 0) {
    double rat = (body[iBody].dCrossoverMass / ATOMMASS - QOH) /
                (body[iBody].dCrossoverMass / ATOMMASS - 1.);
    double XO = fdAtomicOxygenMixingRatio(body[iBody].dSurfaceWaterMass,
                                          body[iBody].dOxygenMass);
    if (XO == 1) {
      *dTmp = 0;
    } else {
      *dTmp     = pow(1. + (XO / (1. - XO)) * QOH * rat, -1);
    }
  } else {
    *dTmp = -1;
  }
  fvFormattedString(cUnit, "");
}


/**
Logs the molecular oxygen mixing ratio.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteMolecOxygenMixingRatio(BODY *body, CONTROL *control, OUTPUT *output,
                                 SYSTEM *system, UNITS *units, UPDATE *update,
                                 int iBody, double *dTmp, char **cUnit) {
  *dTmp = fdMolecOxygenMixingRatio(body[iBody].dSurfaceWaterMass,
                                   body[iBody].dOxygenMass);
  fvFormattedString(cUnit, "");
}

/**
Logs the water mixing ratio.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param units A pointer to the current UNITS instance
@param update A pointer to the current UPDATE instance
@param iBody The current body Number
@param dTmp Temporary variable used for unit conversions
@param cUnit The unit for this variable
*/
void WriteWaterAtmMixingRatio(BODY *body, CONTROL *control, OUTPUT *output,
                              SYSTEM *system, UNITS *units, UPDATE *update,
                              int iBody, double *dTmp, char **cUnit) {
  *dTmp = fdWaterAtmMixingRatio(body[iBody].dSurfaceWaterMass,
                                body[iBody].dOxygenMass);
  fvFormattedString(cUnit, "");
}

/**
Set up stuff to be logged for atmesc.

@param output A pointer to the current OUTPUT instance
@param fnWrite A pointer to the function that does the logging
*/
void InitializeOutputAtmEsc(OUTPUT *output, fnWriteOutput fnWrite[]) {

  fvFormattedString(&output[OUT_SURFACEWATERMASS].cName, "SurfWaterMass");
  fvFormattedString(&output[OUT_SURFACEWATERMASS].cDescr, "Surface water mass");
  fvFormattedString(&output[OUT_SURFACEWATERMASS].cNeg, "TO");
  output[OUT_SURFACEWATERMASS].bNeg       = 1;
  output[OUT_SURFACEWATERMASS].dNeg       = 1. / TOMASS;
  output[OUT_SURFACEWATERMASS].iNum       = 1;
  output[OUT_SURFACEWATERMASS].iModuleBit = ATMESC;
  fnWrite[OUT_SURFACEWATERMASS]           = &WriteSurfaceWaterMass;

  fvFormattedString(&output[OUT_PLANETRADIUS].cName, "PlanetRadius");
  fvFormattedString(&output[OUT_PLANETRADIUS].cDescr, "Planet radius");
  fvFormattedString(&output[OUT_PLANETRADIUS].cNeg, "Rearth");
  output[OUT_PLANETRADIUS].bNeg       = 1;
  output[OUT_PLANETRADIUS].dNeg       = 1. / REARTH;
  output[OUT_PLANETRADIUS].iNum       = 1;
  output[OUT_PLANETRADIUS].iModuleBit = ATMESC;
  fnWrite[OUT_PLANETRADIUS]           = &WritePlanetRadius;

  fvFormattedString(&output[OUT_OXYGENMASS].cName, "OxygenMass");
  fvFormattedString(&output[OUT_OXYGENMASS].cDescr, "Oxygen mass in the atmosphere");
  fvFormattedString(&output[OUT_OXYGENMASS].cNeg, "bars");
  output[OUT_OXYGENMASS].bNeg       = 1;
  output[OUT_OXYGENMASS].dNeg       = 1;
  output[OUT_OXYGENMASS].iNum       = 1;
  output[OUT_OXYGENMASS].iModuleBit = ATMESC;
  fnWrite[OUT_OXYGENMASS]           = &WriteOxygenMass;

  fvFormattedString(&output[OUT_OXYGENMANTLEMASS].cName, "OxygenMantleMass");
  fvFormattedString(&output[OUT_OXYGENMANTLEMASS].cDescr, "Mass of oxygen in mantle");
  fvFormattedString(&output[OUT_OXYGENMANTLEMASS].cNeg, "bars");
  output[OUT_OXYGENMANTLEMASS].bNeg       = 1;
  output[OUT_OXYGENMANTLEMASS].dNeg       = 1;
  output[OUT_OXYGENMANTLEMASS].iNum       = 1;
  output[OUT_OXYGENMANTLEMASS].iModuleBit = ATMESC;
  fnWrite[OUT_OXYGENMANTLEMASS]           = &WriteOxygenMantleMass;

  fvFormattedString(&output[OUT_RGLIMIT].cName, "RGLimit");
  fvFormattedString(&output[OUT_RGLIMIT].cDescr, "Runaway greenhouse semi-major axis");
  fvFormattedString(&output[OUT_RGLIMIT].cNeg, "AU");
  output[OUT_RGLIMIT].bNeg       = 1;
  output[OUT_RGLIMIT].dNeg       = 1. / AUM;
  output[OUT_RGLIMIT].iNum       = 1;
  output[OUT_RGLIMIT].iModuleBit = ATMESC;
  fnWrite[OUT_RGLIMIT]           = &WriteRGLimit;

  fvFormattedString(&output[OUT_XO].cName, "XO");
  fvFormattedString(&output[OUT_XO].cDescr,
          "Atomic oxygen mixing ratio in upper atmosphere");
  output[OUT_XO].bNeg       = 0;
  output[OUT_XO].iNum       = 1;
  output[OUT_XO].iModuleBit = ATMESC;
  fnWrite[OUT_XO]           = &WriteOxygenMixingRatio;

  fvFormattedString(&output[OUT_ETAO].cName, "EtaO");
  fvFormattedString(&output[OUT_ETAO].cDescr,
          "Oxygen eta parameter (Luger and Barnes 2015)");
  output[OUT_ETAO].bNeg       = 0;
  output[OUT_ETAO].iNum       = 1;
  output[OUT_ETAO].iModuleBit = ATMESC;
  fnWrite[OUT_ETAO]           = &WriteOxygenEta;

  fvFormattedString(&output[OUT_KTIDE].cName, "KTide");
  fvFormattedString(&output[OUT_KTIDE].cDescr,
          "Mass loss enhancement due to stellar gravity");
  output[OUT_KTIDE].bNeg       = 0;
  output[OUT_KTIDE].iNum       = 1;
  output[OUT_KTIDE].iModuleBit = ATMESC;
  fnWrite[OUT_KTIDE]           = &WriteKTide;

  fvFormattedString(&output[OUT_EPSH2O].cName, "AtmXAbsEffH2O");
  fvFormattedString(&output[OUT_EPSH2O].cDescr,
          "XUV atmospheric escape efficiency for H2O");
  output[OUT_EPSH2O].bNeg       = 0;
  output[OUT_EPSH2O].iNum       = 1;
  output[OUT_EPSH2O].iModuleBit = ATMESC;
  fnWrite[OUT_EPSH2O]           = &WriteAtmXAbsEffH2O;

  fvFormattedString(&output[OUT_ENVELOPEMASS].cName, "EnvelopeMass");
  fvFormattedString(&output[OUT_ENVELOPEMASS].cDescr, "Envelope mass");
  fvFormattedString(&output[OUT_ENVELOPEMASS].cNeg, "Mearth");
  output[OUT_ENVELOPEMASS].bNeg       = 1;
  output[OUT_ENVELOPEMASS].dNeg       = 1. / MEARTH;
  output[OUT_ENVELOPEMASS].iNum       = 1;
  output[OUT_ENVELOPEMASS].iModuleBit = ATMESC;
  fnWrite[OUT_ENVELOPEMASS]           = &WriteEnvelopeMass;

  fvFormattedString(&output[OUT_PLANETRADXUV].cName, "RadXUV");
  fvFormattedString(&output[OUT_PLANETRADXUV].cDescr,
          "XUV radius separating hydro. dyn. escape and equilibrium");
  fvFormattedString(&output[OUT_PLANETRADXUV].cNeg, "Rearth");
  output[OUT_PLANETRADXUV].bNeg       = 1;
  output[OUT_PLANETRADXUV].dNeg       = 1. / REARTH;
  output[OUT_PLANETRADXUV].iNum       = 1;
  output[OUT_PLANETRADXUV].iModuleBit = ATMESC;
  fnWrite[OUT_PLANETRADXUV]           = &WritePlanetRadXUV;

  fvFormattedString(&output[OUT_BONDIRADIUS].cName, "BondiRadius");
  fvFormattedString(&output[OUT_BONDIRADIUS].cDescr, "Bondi Radius");
  output[OUT_BONDIRADIUS].bNeg = 1;
  fvFormattedString(&output[OUT_BONDIRADIUS].cNeg, "Rearth");
  output[OUT_BONDIRADIUS].dNeg       = 1. / REARTH;
  output[OUT_BONDIRADIUS].iNum       = 1;
  output[OUT_BONDIRADIUS].iModuleBit = ATMESC;
  fnWrite[OUT_BONDIRADIUS]           = &WriteBondiRadius;

  fvFormattedString(&output[OUT_ROCHERADIUS].cName, "RocheRadius");
  fvFormattedString(&output[OUT_ROCHERADIUS].cDescr, "Roche Lobe Radius");
  output[OUT_ROCHERADIUS].bNeg = 1;
  fvFormattedString(&output[OUT_ROCHERADIUS].cNeg, "Rearth");
  output[OUT_ROCHERADIUS].dNeg       = 1. / REARTH;
  output[OUT_ROCHERADIUS].iNum       = 1;
  output[OUT_ROCHERADIUS].iModuleBit = ATMESC;
  fnWrite[OUT_ROCHERADIUS]           = &WriteRocheRadius;

  fvFormattedString(&output[OUT_DENVMASSDT].cName, "DEnvMassDt");
  fvFormattedString(&output[OUT_DENVMASSDT].cDescr, "Envelope Mass Loss Rate");
  fvFormattedString(&output[OUT_DENVMASSDT].cNeg, "Mearth/Myr");
  output[OUT_DENVMASSDT].bNeg       = 1;
  output[OUT_DENVMASSDT].dNeg       = (YEARSEC * 1e6) / MEARTH;
  output[OUT_DENVMASSDT].iNum       = 1;
  output[OUT_DENVMASSDT].iModuleBit = ATMESC;
  fnWrite[OUT_DENVMASSDT]           = &WriteDEnvMassDt;

  fvFormattedString(&output[OUT_THERMTEMP].cName, "ThermTemp");
  fvFormattedString(&output[OUT_THERMTEMP].cDescr, "Isothermal atmospheric temperature");
  fvFormattedString(&output[OUT_THERMTEMP].cNeg, "K");
  output[OUT_THERMTEMP].bNeg       = 1;
  output[OUT_THERMTEMP].dNeg       = 1; // default units are K.
  output[OUT_THERMTEMP].iNum       = 1;
  output[OUT_THERMTEMP].iModuleBit = ATMESC;
  fnWrite[OUT_THERMTEMP]           = &WriteThermTemp;

  fvFormattedString(&output[OUT_PRESSURF].cName, "PresSurf");
  fvFormattedString(&output[OUT_PRESSURF].cDescr, "Surface pressure due to atmosphere");
  fvFormattedString(&output[OUT_PRESSURF].cNeg, "GPa");
  output[OUT_PRESSURF].bNeg       = 1;
  output[OUT_PRESSURF].dNeg       = 1e-9;
  output[OUT_PRESSURF].iNum       = 1;
  output[OUT_PRESSURF].iModuleBit = ATMESC;
  fnWrite[OUT_PRESSURF]           = &WritePresSurf;

  fvFormattedString(&output[OUT_PRESXUV].cName, "PresXUV");
  fvFormattedString(&output[OUT_PRESXUV].cDescr, "Pressure at base of thermosphere");
  fvFormattedString(&output[OUT_PRESXUV].cNeg, "Pa");
  output[OUT_PRESXUV].bNeg       = 1;
  output[OUT_PRESXUV].dNeg       = 1;
  output[OUT_PRESXUV].iNum       = 1;
  output[OUT_PRESXUV].iModuleBit = ATMESC;
  fnWrite[OUT_PRESXUV]           = &WritePresXUV;

  fvFormattedString(&output[OUT_SCALEHEIGHT].cName, "ScaleHeight");
  fvFormattedString(&output[OUT_SCALEHEIGHT].cDescr,
          "Scale height in Lehmer & Catling (2016) model");
  fvFormattedString(&output[OUT_SCALEHEIGHT].cNeg, "km");
  output[OUT_SCALEHEIGHT].bNeg       = 1;
  output[OUT_SCALEHEIGHT].dNeg       = 0.001;
  output[OUT_SCALEHEIGHT].iNum       = 1;
  output[OUT_SCALEHEIGHT].iModuleBit = ATMESC;
  fnWrite[OUT_SCALEHEIGHT]           = &WriteScaleHeight;

  fvFormattedString(&output[OUT_ATMGASCONST].cName, "AtmGasConst");
  fvFormattedString(&output[OUT_ATMGASCONST].cDescr, "Atmospheric gas constant");
  fvFormattedString(&output[OUT_ATMGASCONST].cNeg, "J / K kg");
  output[OUT_ATMGASCONST].bNeg       = 1;
  output[OUT_ATMGASCONST].dNeg       = 1;
  output[OUT_ATMGASCONST].iNum       = 1;
  output[OUT_ATMGASCONST].iModuleBit = ATMESC;
  fnWrite[OUT_ATMGASCONST]           = &WriteAtmGasConst;

  fvFormattedString(&output[OUT_RADSOLID].cName, "RadSolid");
  fvFormattedString(&output[OUT_RADSOLID].cDescr, "Radius to the solid surface");
  fvFormattedString(&output[OUT_RADSOLID].cNeg, "Rearth");
  output[OUT_RADSOLID].bNeg       = 1;
  output[OUT_RADSOLID].dNeg       = 1. / REARTH;
  output[OUT_RADSOLID].iNum       = 1;
  output[OUT_RADSOLID].iModuleBit = ATMESC;
  fnWrite[OUT_RADSOLID]           = &WriteRadSolid;

  // What the difference between this and XUVFlux?
  fvFormattedString(&output[OUT_FXUV].cName, "FXUV");
  fvFormattedString(&output[OUT_FXUV].cDescr, "XUV flux");
  fvFormattedString(&output[OUT_FXUV].cNeg, "W/m^2");
  output[OUT_FXUV].bNeg       = 1;
  output[OUT_FXUV].dNeg       = 1;
  output[OUT_FXUV].iNum       = 1;
  output[OUT_FXUV].iModuleBit = ATMESC;
  fnWrite[OUT_FXUV]           = &WriteFXUV;

  fvFormattedString(&output[OUT_HESCAPEREGIME].cName, "HEscapeRegime");
  fvFormattedString(&output[OUT_HESCAPEREGIME].cDescr,
          "Integer flag for H envelope escape regime");
  output[OUT_HESCAPEREGIME].bNeg       = 0;
  output[OUT_HESCAPEREGIME].iNum       = 1;
  output[OUT_HESCAPEREGIME].iModuleBit = ATMESC;
  fnWrite[OUT_HESCAPEREGIME]           = &WriteHEscapeRegime;

  fvFormattedString(&output[OUT_RRCRITICALFLUX].cName, "RRCriticalFlux");
  fvFormattedString(&output[OUT_RRCRITICALFLUX].cDescr,
          "Critical XUV Flux that separates RR and energy-limited escape");
  fvFormattedString(&output[OUT_RRCRITICALFLUX].cNeg, "W/m^2");
  output[OUT_RRCRITICALFLUX].bNeg       = 1;
  output[OUT_RRCRITICALFLUX].dNeg       = 1;
  output[OUT_RRCRITICALFLUX].iNum       = 1;
  output[OUT_RRCRITICALFLUX].iModuleBit = ATMESC;
  fnWrite[OUT_RRCRITICALFLUX]           = &WriteRRCriticalFlux;

  fvFormattedString(&output[OUT_CROSSOVERMASS].cName, "CrossoverMass");
  fvFormattedString(&output[OUT_CROSSOVERMASS].cDescr,
          "Crossover mass, defines oxygen is escaping if mc > mo");
  fvFormattedString(&output[OUT_CROSSOVERMASS].cNeg, "amu");
  output[OUT_CROSSOVERMASS].bNeg       = 1;
  output[OUT_CROSSOVERMASS].dNeg       = 1. / ATOMMASS;
  output[OUT_CROSSOVERMASS].iNum       = 1;
  output[OUT_CROSSOVERMASS].iModuleBit = ATMESC;
  fnWrite[OUT_CROSSOVERMASS]           = &WriteCrossoverMass;

  fvFormattedString(&output[OUT_WATERESCAPEREGIME].cName, "WaterEscapeRegime");
  fvFormattedString(&output[OUT_WATERESCAPEREGIME].cDescr,
          "What water escape regime the code is currently in");
  output[OUT_WATERESCAPEREGIME].bNeg       = 0;
  output[OUT_WATERESCAPEREGIME].iNum       = 1;
  output[OUT_WATERESCAPEREGIME].iModuleBit = ATMESC;
  fnWrite[OUT_WATERESCAPEREGIME]           = &WriteWaterEscapeRegime;

  fvFormattedString(&output[OUT_HREFODRAGMOD].cName, "HRefODragMod");
  fvFormattedString(&output[OUT_HREFODRAGMOD].cDescr,
          "Multiply by H ref flux to get flux with drag");
  output[OUT_HREFODRAGMOD].bNeg       = 0;
  output[OUT_HREFODRAGMOD].iNum       = 1;
  output[OUT_HREFODRAGMOD].iModuleBit = ATMESC;
  fnWrite[OUT_HREFODRAGMOD]           = &WriteHRefODragMod;

  fvFormattedString(&output[OUT_FXUVCRITDRAG].cName, "FXUVCRITDRAG");
  fvFormattedString(&output[OUT_FXUVCRITDRAG].cDescr, "Critical Drag XUV flux");
  fvFormattedString(&output[OUT_FXUVCRITDRAG].cNeg, "W/m^2");
  output[OUT_FXUVCRITDRAG].bNeg       = 1;
  output[OUT_FXUVCRITDRAG].dNeg       = 1;
  output[OUT_FXUVCRITDRAG].iNum       = 1;
  output[OUT_FXUVCRITDRAG].iModuleBit = ATMESC;
  fnWrite[OUT_FXUVCRITDRAG]           = &WriteFXUVCRITDRAG;

  fvFormattedString(&output[OUT_HREFFLUX].cName, "HREFFLUX");
  fvFormattedString(&output[OUT_HREFFLUX].cDescr,
          "The Hydrogen Reference Flux (m^-2 s^-1)");
  output[OUT_HREFFLUX].bNeg       = 0;
  output[OUT_HREFFLUX].iNum       = 1;
  output[OUT_HREFFLUX].iModuleBit = ATMESC;
  fnWrite[OUT_HREFFLUX]           = &WriteHREFFLUX;

  fvFormattedString(&output[OUT_HDIFFFLUX].cName, "HDiffFlux");
  fvFormattedString(&output[OUT_HDIFFFLUX].cDescr,
          "The Hydrogen Diffusion Flux (m^-2 s^-1)");
  output[OUT_HDIFFFLUX].bNeg       = 0;
  output[OUT_HDIFFFLUX].iNum       = 1;
  output[OUT_HDIFFFLUX].iModuleBit = ATMESC;
  fnWrite[OUT_HDIFFFLUX]           = &WriteHDiffLimFlux;

  fvFormattedString(&output[OUT_XO2].cName, "XO2");
  fvFormattedString(&output[OUT_XO2].cDescr,
          "Molecular oxygen mixing ratio in atmosphere");
  output[OUT_XO2].bNeg       = 0;
  output[OUT_XO2].iNum       = 1;
  output[OUT_XO2].iModuleBit = ATMESC;
  fnWrite[OUT_XO2]           = &WriteMolecOxygenMixingRatio;

  fvFormattedString(&output[OUT_XH2O].cName, "XH2O");
  fvFormattedString(&output[OUT_XH2O].cDescr, "Water mixing ratio in upper atmosphere");
  output[OUT_XH2O].bNeg       = 0;
  output[OUT_XH2O].iNum       = 1;
  output[OUT_XH2O].iModuleBit = ATMESC;
  fnWrite[OUT_XH2O]           = &WriteWaterAtmMixingRatio;
}

/************ ATMESC Logging Functions **************/

/**
Log the global atmesc options.

\warning This routine currently does nothing!

@param control A pointer to the current CONTROL instance
@param fp A FILE pointer
*/
void LogOptionsAtmEsc(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- ATMESC Options -----\n\n");
  */
}

/**
Log the global atmesc parameters.

\warning This routine currently does nothing!

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param update A pointer to the current UPDATE instance
@param fnWrite A pointer to the function doing the logging
@param fp A FILE pointer
*/
void LogAtmEsc(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {

  /* Anything here?
  int iOut;

  fprintf(fp,"\n----- ATMESC PARAMETERS ------\n");
  for (iOut=OUTSTARTATMESC;iOut<OUTBODYSTARTATMESC;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

/**
Log the body-specific atmesc parameters.

@param body A pointer to the current BODY instance
@param control A pointer to the current CONTROL instance
@param output A pointer to the current OUTPUT instance
@param system A pointer to the current SYSTEM instance
@param update A pointer to the current UPDATE instance
@param fnWrite A pointer to the function doing the logging
@param fp A FILE pointer
@param iBody The current BODY number
*/
void LogBodyAtmEsc(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UPDATE *update, fnWriteOutput fnWrite[], FILE *fp,
                   int iBody) {
  int iOut;
  fprintf(fp, "----- ATMESC PARAMETERS (%s)------\n", body[iBody].cName);

  for (iOut = OUTSTARTATMESC; iOut < OUTENDATMESC; iOut++) {
    if (output[iOut].iNum > 0) {
      // fprintf(stderr,"%d\n",iOut);
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
  }

  // TODO: Log this the standard way
  fprintf(fp, "(RGDuration) Runaway Greenhouse Duration [years]: %.5e\n",
          body[iBody].dRGDuration / YEARSEC);
}

/**
Adds atmesc to the current array of MODULEs.

@param module A pointer to the current array of MODULE instances
@param iBody The current BODY number
@param iModule The current MODULE number
*/
void AddModuleAtmEsc(CONTROL *control, MODULE *module, int iBody, int iModule) {

  module->iaModule[iBody][iModule] = ATMESC;

  module->fnCountHalts[iBody][iModule]        = &CountHaltsAtmEsc;
  module->fnReadOptions[iBody][iModule]       = &ReadOptionsAtmEsc;
  module->fnLogBody[iBody][iModule]           = &LogBodyAtmEsc;
  module->fnVerify[iBody][iModule]            = &VerifyAtmEsc;
  module->fnAssignDerivatives[iBody][iModule] = &AssignAtmEscDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullAtmEscDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltAtmEsc;

  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateAtmEsc;
  module->fnFinalizeUpdateSurfaceWaterMass[iBody][iModule] =
        &FinalizeUpdateSurfaceWaterMassAtmEsc;
  module->fnFinalizeUpdateOxygenMass[iBody][iModule] =
        &FinalizeUpdateOxygenMassAtmEsc;
  module->fnFinalizeUpdateOxygenMantleMass[iBody][iModule] =
        &FinalizeUpdateOxygenMantleMassAtmEsc;
  module->fnFinalizeUpdateEnvelopeMass[iBody][iModule] =
        &FinalizeUpdateEnvelopeMassAtmEsc;
  module->fnFinalizeUpdateMass[iBody][iModule] =
        &FinalizeUpdateEnvelopeMassAtmEsc;
  module->fnFinalizeUpdateRadius[iBody][iModule] = &FinalizeUpdateRadiusAtmEsc;
}

/************* ATMESC Functions ************/

/**
The rate of change of the surface water mass.

@param body A pointer to the current BODY instance
@param system A pointer to the current SYSTEM instance
@param iaBody An array of body indices. The current body is index 0.
*/
double fdDSurfaceWaterMassDt(BODY *body, SYSTEM *system, int *iaBody) {

  if ((body[iaBody[0]].bRunaway) && (body[iaBody[0]].dSurfaceWaterMass > 0)) {
    // This takes care of both energy-limited and diffusion limited escape!
    return -(9. / (1 + 8 * body[iaBody[0]].dOxygenEta)) *
           body[iaBody[0]].dMDotWater;
  } else {

    return 0.;
  }
}

/**
The rate of change of the oxygen mass in the atmosphere.

@param body A pointer to the current BODY instance
@param system A pointer to the current SYSTEM instance
@param iaBody An array of body indices. The current body is index 0.
*/
double fdDOxygenMassDt(BODY *body, SYSTEM *system, int *iaBody) {

  if ((body[iaBody[0]].bRunaway) && (!body[iaBody[0]].bInstantO2Sink) &&
      (body[iaBody[0]].dSurfaceWaterMass > 0)) {

    if (body[iaBody[0]].iWaterLossModel == ATMESC_LB15) {

      // Rodrigo and Barnes (2015)
      if (body[iaBody[0]].dCrossoverMass >= 16 * ATOMMASS) {
        double BDIFF = 4.8e19 * pow(body[iaBody[0]].dFlowTemp, 0.75);
        return (320. * PI * BIGG * ATOMMASS * ATOMMASS * BDIFF *
                body[iaBody[0]].dMass) /
               (KBOLTZ * body[iaBody[0]].dFlowTemp);
      } else {
        return (8 - 8 * body[iaBody[0]].dOxygenEta) /
               (1 + 8 * body[iaBody[0]].dOxygenEta) *
               body[iaBody[0]].dMDotWater;
      }
      /**    } else if (body[iaBody[0]].iWaterLossModel == ATMESC_LS2016) {
              return (8 - 8 * (body[iaBody[0]].dOxygenEta)) /
                      (1 + 8 * (body[iaBody[0]].dOxygenEta)) *
         body[iaBody[0]].dMDotWater; double BDIFF = 4.8e19 *
         pow(body[iaBody[0]].dFlowTemp, 0.75); double g = (BIGG *
         body[iaBody[0]].dMass) / (body[iaBody[0]].dRadius *
         body[iaBody[0]].dRadius); double XO =
         fdAtomicOxygenMixingRatio(body[iaBody[0]].dSurfaceWaterMass,
                                          body[iaBody[0]].dOxygenMass);
              return
         ((BDIFF*PROTONMASS*g)/(KBOLTZ*body[iaBody[0]].dFlowTemp))*15*(1-XO); */
    } else if (body[iaBody[0]].iWaterLossModel == ATMESC_LS2016) {
      // Add the 2s here meg
      return (8 - 8 * (body[iaBody[0]].dOxygenEta)) /
             (1 + 8 * (body[iaBody[0]].dOxygenEta)) *
             body[iaBody[0]].dMDotWater;
    } else {

      // Exact
      return (8 - 8 * body[iaBody[0]].dOxygenEta) /
             (1 + 8 * body[iaBody[0]].dOxygenEta) * body[iaBody[0]].dMDotWater;
    }
  } else {

    return 0.;
  }
}

/**
The rate of change of the oxygen mass in the mantle.

@param body A pointer to the current BODY instance
@param system A pointer to the current SYSTEM instance
@param iaBody An array of body indices. The current body is index 0.
*/
double fdDOxygenMantleMassDt(BODY *body, SYSTEM *system, int *iaBody) {

  if ((body[iaBody[0]].bRunaway) && (body[iaBody[0]].bInstantO2Sink) &&
      (body[iaBody[0]].dSurfaceWaterMass > 0)) {

    if (body[iaBody[0]].iWaterLossModel == ATMESC_LB15) {

      // Rodrigo and Barnes (2015)
      if (body[iaBody[0]].dCrossoverMass >= 16 * ATOMMASS) {
        double BDIFF = 4.8e19 * pow(body[iaBody[0]].dFlowTemp, 0.75);
        return (320. * PI * BIGG * ATOMMASS * ATOMMASS * BDIFF *
                body[iaBody[0]].dMass) /
               (KBOLTZ * body[iaBody[0]].dFlowTemp);
      } else {
        return (8 - 8 * body[iaBody[0]].dOxygenEta) /
               (1 + 8 * body[iaBody[0]].dOxygenEta) *
               body[iaBody[0]].dMDotWater;
      }
    } else if (body[iaBody[0]].iWaterLossModel == ATMESC_LS2016) {
      // Add the 2s here meg
      return (8 - 8 * (body[iaBody[0]].dOxygenEta)) /
             (1 + 8 * (body[iaBody[0]].dOxygenEta)) *
             body[iaBody[0]].dMDotWater;
    } else {
      // Exact
      return (8 - 8 * body[iaBody[0]].dOxygenEta) /
             (1 + 8 * body[iaBody[0]].dOxygenEta) * body[iaBody[0]].dMDotWater;
    }
  } else {

    return 0.;
  }
}

/**
The rate of change of the envelope mass given energy-limited escape.

@param body A pointer to the current BODY instance
@param system A pointer to the current SYSTEM instance
@param iaBody An array of body indices. The current body is index 0.

*/
double fdDEnvelopeMassDt(BODY *body, SYSTEM *system, int *iaBody) {

  double dMassDt = dTINY;
  dMassDt =
        -body[iaBody[0]].dFHRef *
        (body[iaBody[0]].dAtmXAbsEffH / body[iaBody[0]].dAtmXAbsEffH2O) *
        (4 * ATOMMASS * PI * body[iaBody[0]].dRadius * body[iaBody[0]].dRadius *
         body[iaBody[0]].dXFrac * body[iaBody[0]].dXFrac);

  return dMassDt;
}


/**
The rate of change of the envelope mass given Bondi-limited escape.

@param body A pointer to the current BODY instance
@param system A pointer to the current SYSTEM instance
@param iaBody An array of body indices. The current body is index 0.

*/
double fdDEnvelopeMassDtBondiLimited(BODY *body, SYSTEM *system, int *iaBody) {

  // Compute Bondi-limited mass loss rate where body 0 is always the host star
  double dMDt = -1.9e15 * (body[iaBody[0]].dMass / (10.0 * MEARTH)) /
                sqrt(body[0].dTemperature / 5800.0);
  dMDt = dMDt * pow(body[iaBody[0]].dSemi / (0.1 * AUM), 0.25) /
         pow(body[0].dRadius / RSUN, 0.25);

  return dMDt;
}

/**
The rate of change of the envelope mass given raditation/recombination-limited
escape.

@param body A pointer to the current BODY instance
@param system A pointer to the current SYSTEM instance
@param iaBody An array of body indices. The current body is index 0.

*/
double fdDEnvelopeMassDtRRLimited(BODY *body, SYSTEM *system, int *iaBody) {

  // Compute radiation/recombination-limited mass loss rate using
  // equation 13 from Luger+2015 (note: factor of 10^3 converts between cgs and
  // vplanet internal mks units for scaling relation derivative calculated here)
  double dMassDt = -7.11e4 * sqrt(body[iaBody[0]].dFXUV * 1000.0) *
                   pow(body[iaBody[0]].dRadius / REARTH, 1.5);

  return dMassDt;
}

/**
This function does nothing in atmesc.

@param body A pointer to the current BODY instance
@param system A pointer to the current SYSTEM instance
@param update A pointer to the current UPDATE instance
@param iBody The current body index
@param iFoo An example of pretty lousy programming
*/
double fdSurfEnFluxAtmEsc(BODY *body, SYSTEM *system, UPDATE *update, int iBody,
                          int iFoo) {
  // This is silly, but necessary!
  return 0;
}


/**
Returns the planet radius at the current time.

@param body A pointer to the current BODY instance
@param system A pointer to the current SYSTEM instance
@param iaBody An array of body indices. The current body is index 0.
*/
double fdPlanetRadius(BODY *body, SYSTEM *system, int *iaBody) {

  if (body[iaBody[0]].iPlanetRadiusModel == ATMESC_LEHMER17) {
    body[iaBody[0]].dPresSurf =
          fdLehmerPres(body[iaBody[0]].dEnvelopeMass,
                       body[iaBody[0]].dGravAccel, body[iaBody[0]].dRadSolid);
    body[iaBody[0]].dRadXUV = fdLehmerRadius(body, system->iNumBodies, iaBody[0]);
  }

  double foo;
  if (body[iaBody[0]].iPlanetRadiusModel == ATMESC_LOP12) {
    // If no envelope, return solid body radius according to Sotin+2007 model
    if (body[iaBody[0]].dEnvelopeMass <= body[iaBody[0]].dMinEnvelopeMass) {
      foo = fdMassToRad_Sotin07(body[iaBody[0]].dMass);
    }
    // Envelope present: estimate planetary radius using Lopez models
    else {
      foo = fdLopezRadius(body[iaBody[0]].dMass,
                          body[iaBody[0]].dEnvelopeMass / body[iaBody[0]].dMass,
                          1., body[iaBody[0]].dAge, 0);
    }
    if (!isnan(foo)) {
      return foo;
    } else {
      return body[iaBody[0]].dRadius;
    }
  } else if (body[iaBody[0]].iPlanetRadiusModel == ATMESC_PROXCENB) {
    return fdProximaCenBRadius(body[iaBody[0]].dEnvelopeMass /
                                     body[iaBody[0]].dMass,
                               body[iaBody[0]].dAge, body[iaBody[0]].dMass);
  } else {
    return body[iaBody[0]].dRadius;
  }
}

/************* ATMESC Helper Functions ************/

/**
Computes whether or not water is escaping.

@param body A pointer to the current BODY instance
@param iBody The current BODY index
*/
int fbDoesWaterEscape(BODY *body, EVOLVE *evolve, IO *io, int iBody) {
  // TODO: The checks below need to be moved. Ideally we'd
  // just remove this equation from the matrix if the
  // escape conditions are not met.

  // 1. Check if there's hydrogen to be lost; this happens first
  if (body[iBody].dEnvelopeMass > 0) {
    // (But let's still check whether the RG phase has ended)
    if ((body[iBody].dRGDuration == 0.) &&
        (fdInstellation(body, iBody) < fdHZRG14(body, iBody))) {
      body[iBody].dRGDuration = body[iBody].dAge;
    }
    return 0;
  }

  /* If the central body is not a star, then allow water to escape */
  if (!body[0].bStellar) {
    return 1;
  }

  // 2. Check if planet is beyond RG limit; if user requested water loss to stop
  // (the cold trap prevents water loss) then water does not escape.
  // NOTE: The RG flux limit below is calculated based on body zero's
  // spectrum! The Kopparapu+14 limit is for a single star only. This
  // approximation for a binary is only valid if the two stars have
  // similar spectral types, or if body zero dominates the flux.
  double dInstellation = fdInstellation(body, iBody);
  if (dInstellation == -1 && body[iBody].bCalcFXUV == 0) {
    // Constant XUV flux, so set water to escape
    return 1;
  } else {
    double dRunawayGreenhouseFlux = fdHZRG14(body, iBody);
    if (dInstellation < dRunawayGreenhouseFlux) {
      if (body[iBody].dRGDuration == 0.) {
        body[iBody].dRGDuration = body[iBody].dAge;
        if (io->iVerbose > VERBPROG && !io->baEnterHZMessage[iBody]) {
          printf("%s enters the habitable zone at %.2lf Myr.\n",
                body[iBody].cName, evolve->dTime / (YEARSEC * 1e6));
          io->baEnterHZMessage[iBody] = 1;
        }
      }
      // Only stop water loss if user requested, which is default
      if (body[iBody].bStopWaterLossInHZ) {
        return 0;
      }
    }
  }

  // 3. Is there still water to be lost?
  if (body[iBody].dSurfaceWaterMass <= 0) {
    return 0;
  }

  // 4. Are we in the ballistic (Jeans) escape limit?
  if (body[iBody].dAge > body[iBody].dJeansTime) {
    return 0;
  }

  return 1;
}

/**
Computes the atomic oxygen mixing ratio in the hydrodynamic flow.

@param dSurfaceWaterMass The amount of water in the atmosphere
@param dOxygenMass The amount of oxygen in the atmosphere
*/
double fdAtomicOxygenMixingRatio(double dSurfaceWaterMass, double dOxygenMass) {
  // Mixing ratio X_O of atomic oxygen in the upper atmosphere
  // assuming atmosphere is well-mixed up to the photolysis layer
  double NO2  = dOxygenMass / (32 * ATOMMASS);
  double NH2O = dSurfaceWaterMass / (18 * ATOMMASS);
  if (NH2O > 0) {
    return 1. / (1 + 1. / (0.5 + NO2 / NH2O));
  } else {
    if (NO2 > 0) {
      return 1.;
    } else {
      return 0.;
    }
  }
}

/**
Computes the molecular oxygen mixing ratio in atmosphere. ASSUMING PURE
WATER/OXYGEN ATMOSPHERE

@param dSurfaceWaterMass The amount of water in the atmosphere
@param dOxygenMass The amount of oxygen in the atmosphere
*/
double fdMolecOxygenMixingRatio(double dSurfaceWaterMass, double dOxygenMass) {
  // Mixing ratio X_O of atomic oxygen in the upper atmosphere
  // assuming atmosphere is well-mixed up to the photolysis layer
  // XXX Hack to make envelope losses behave
  if (dSurfaceWaterMass > 0) {
    double NO2  = dOxygenMass / (32 * ATOMMASS);
    double NH2O = dSurfaceWaterMass / (18 * ATOMMASS);
    return NO2 / (NO2 + NH2O);
  } else {
    return 0;
  }
}

/**
Computes the water mixing ratio in atmosphere. ASSUMING PURE WATER/OXYGEN
ATMOSPHERE

@param dSurfaceWaterMass The amount of water in the atmosphere
@param dOxygenMass The amount of oxygen in the atmosphere
*/
double fdWaterAtmMixingRatio(double dSurfaceWaterMass, double dOxygenMass) {
  // Mixing ratio X_O of atomic oxygen in the upper atmosphere
  // assuming atmosphere is well-mixed up to the photolysis layer
  // XXX Hack to make envelope losses behave
  if (dSurfaceWaterMass > 0) {
    double NO2  = dOxygenMass / (32 * ATOMMASS);
    double NH2O = dSurfaceWaterMass / (18 * ATOMMASS);
    return NH2O / (NO2 + NH2O);
  } else {
    return 0;
  }
}

/**
Performs a simple log-linear fit to the Kopparapu et al. (2014) mass-dependent
runaway greenhouse limit.

\warning  Something is wrong with this linear fit in the first 5 Myr or so, as
it diverges.

@param dLuminosity The stellar luminosity
@param dTeff The stellar effective temperature
@param dEcc The planet's eccentricity -- RB: Why is ecc passed?
@param dPlanetMass The planet mass
*/

// Why isn't this in system.c?
double fdHZRG14(BODY *body, int iBody) {
  // double dLuminosity, double dTeff, double dEcc, double dPlanetMass) {
  // Do a simple log-linear fit to the Kopparapu+14 mass-dependent RG limit
  int i;
  double seff[3];
  double daCoeffs[2];
  double dHZRG14Limit;

  double tstar      = body[0].dTemperature - 5780;
  double daLogMP[3] = {-1.0, 0., 0.69897};
  double seffsun[3] = {0.99, 1.107, 1.188};
  double a[3]       = {1.209e-4, 1.332e-4, 1.433e-4};
  double b[3]       = {1.404e-8, 1.58e-8, 1.707e-8};
  double c[3]       = {-7.418e-12, -8.308e-12, -8.968e-12};
  double d[3]       = {-1.713e-15, -1.931e-15, -2.084e-15};

  for (i = 0; i < 3; i++) {
    seff[i] = seffsun[i] + a[i] * tstar + b[i] * tstar * tstar +
              c[i] * pow(tstar, 3) + d[i] * pow(tstar, 4);
  }

  fvLinearFit(daLogMP, seff, 3, daCoeffs);

  dHZRG14Limit =
        (daCoeffs[0] * log10(body[iBody].dMass / MEARTH) + daCoeffs[1]) * LSUN /
        (4 * PI * AUM * AUM);

  return dHZRG14Limit;
}

/**
Computes the XUV absorption efficiency for a water vapor atmosphere
based on a fit to the figure in Bolmont et al. (2017).

@param dFXUV The XUV flux incident on the planet.
*/
double fdXUVEfficiencyBolmont2016(double dFXUV) {

  // Based on a piecewise polynomial fit to Figure 2
  // in Bolmont et al. (2017), the XUV escape efficiency
  // as a function of XUV flux for the TRAPPIST-1 planets.

  // Polynomial coefficients
  double a0 = 1.49202;
  double a1 = 5.57875;
  double a2 = 2.27482;
  double b0 = 0.59182134;
  double b1 = -0.36140798;
  double b2 = -0.04011933;
  double b3 = -0.8988;
  double c0 = -0.00441536;
  double c1 = -0.03068399;
  double c2 = 0.04946948;
  double c3 = -0.89880083;

  // Convert to erg/cm^2/s and take the log
  double dWaterEscapeEfficiency;

  if (dFXUV > 0) {
    double x = log10(dFXUV * 1.e3);

    // Piecewise polynomial fit and handle edge cases
    if ((x >= -2) && (x < -1)) {
      dWaterEscapeEfficiency = pow(10, a0 * x * x + a1 * x + a2);
    } else if ((x >= -1) && (x < 0)) {
      dWaterEscapeEfficiency = pow(10, b0 * x * x * x + b1 * x * x + b2 * x + b3);
    } else if ((x >= 0) && (x <= 5)) {
      dWaterEscapeEfficiency = pow(10, c0 * x * x * x + c1 * x * x + c2 * x + c3);
    } else if (x < -2) { // Lower flux bound
      dWaterEscapeEfficiency = 0.001;
    } else if (x > 5) { // Upper flux bound
      dWaterEscapeEfficiency = 0.01;
    } else { // Base case that never happens but DPF likes code symmetry
      dWaterEscapeEfficiency = 0.1;
    }
  } else {
    dWaterEscapeEfficiency = 0; // No escape if F_XUV = 0
  }
  return dWaterEscapeEfficiency;
}

/**
Performs a really simple linear least-squares fit on data.

@param x The independent coordinates
@param y The dependent coordinates
@param iLen The length of the arrays
@param daCoeffs The slope and the intercept of the fit
*/

// should be in another file. control?
void fvLinearFit(double *x, double *y, int iLen, double *daCoeffs) {
  // Simple least squares linear regression, y(x) = mx + b
  // from http://en.wikipedia.org/wiki/Simple_linear_regression
  double num = 0, den = 0;
  double xavg = 0, yavg = 0;
  double m, b;
  int i;
  for (i = 0; i < iLen; i++) {
    xavg += x[i];
    yavg += y[i];
  }
  xavg /= iLen;
  yavg /= iLen;
  for (i = 0; i < iLen; i++) {
    num += (x[i] - xavg) * (y[i] - yavg);
    den += (x[i] - xavg) * (x[i] - xavg);
  }
  daCoeffs[0] = num / den;                 // Slope
  daCoeffs[1] = yavg - daCoeffs[0] * xavg; // Intercept
}



/**
 Calculate the whether or not incident XUV flux exceeds critical flux between
 the radiation/recombination-limited and energy-limited H envelope escape
 regimes following Luger+2015 Eqn. 13

 @param body BODY struct
 @param iBody int body indentifier

 @return whether or not the flux is radiation/recombination-limited
*/
int fbRRCriticalFlux(BODY *body, int iBody) {

  // Calculate critical flux for this planet
  double dFCrit = fdRRCriticalFlux(body, iBody);

  // If FXUV > Fcrit -> RR-limited escape
  if (body[iBody].dFXUV >= dFCrit) {
    return 1;
  } else {
    return 0;
  }
}

/**
 Calculate the critical flux between the radiation/recombination-limited and
 energy-limited H envelope escape regimes following Luger+2015 Eqn. A23-25

 @param body BODY struct
 @param iBody int body indentifier

 @return critical flux between radiation/recombination and Energy-limited
 regimes
*/
double fdRRCriticalFlux(BODY *body, int iBody) {

  // Compute critical flux using Eqns. A23-25 from Luger+2015
  double dA = PI * body[iBody].dAtmXAbsEffH *
              pow(body[iBody].dRadius * body[iBody].dXFrac, 3);
  dA /= (BIGG * body[iBody].dMass * body[iBody].dKTide);
  double dB     = 2.248e6 * pow(body[iBody].dRadius / REARTH, 1.5);
  double dFCrit = pow(dB / dA, 2);

  return dFCrit;
}

/**
 Estimate if mass loss is Bondi-limited, i.e. is planet in Roche lobe overflow

 @param body BODY struct
 @param iBody int body indentifier

 @return whether or not the flux is Bondi-limited
*/
int fbBondiCriticalDmDt(BODY *body, int iBody) {

  // If the planetary radius exceeds the roche radius, assume Bondi-limited mass
  // loss at the Bondi radius
  if (body[iBody].dRocheRadius < body[iBody].dRadius) {
    return 1;
  } else {
    return 0;
  }
}

/**
 Determine correct print statement to identify atmospheric escape regime change
 string

 @param iRegimeOld int previoius atmospheric escape regime
 @param iRegimeNew int new atmospheric escape regime
 @param dTime double time when regime change occurs

 @return None
*/
void fvAtmEscRegimeChangeOutput(int iRegimeOld, int iRegimeNew, double dTime) {

  // Define strings to represent atmospheric escape regime name
  char saBondi[]  = "Bondi-Limited Escape";
  char saEnergy[] = "Energy-Limited Escape";
  char saRR[]     = "Radiation/Recombination-Limited Escape";
  char saNone[]   = "No Escape";

  // Initially energy-limited escape
  if (iRegimeOld == ATMESC_ELIM) {
    if (iRegimeNew == ATMESC_RRLIM) {
      fprintf(stdout, "Switching from %s to %s at t = %.4lf Myr.\n", saEnergy,
              saRR, dTime);
    } else if (iRegimeNew == ATMESC_BONDILIM) {
      fprintf(stdout, "Switching from %s to %s at t = %.4lf Myr.\n", saEnergy,
              saBondi, dTime);
    } else if (iRegimeNew == ATMESC_NONE) {
      fprintf(stdout, "Switching from %s to %s at t = %.4lf Myr.\n", saEnergy,
              saNone, dTime);
    }
  }
  // Initially RR-limited escape
  else if (iRegimeOld == ATMESC_RRLIM) {
    if (iRegimeNew == ATMESC_ELIM) {
      fprintf(stdout, "Switching from %s to %s at t = %.4lf Myr.\n", saRR,
              saEnergy, dTime);
    } else if (iRegimeNew == ATMESC_BONDILIM) {
      fprintf(stdout, "Switching from %s to %s at t = %.4lf Myr.\n", saRR,
              saBondi, dTime);
    } else if (iRegimeNew == ATMESC_NONE) {
      fprintf(stdout, "Switching from %s to %s at t = %.4lf Myr.\n", saRR,
              saNone, dTime);
    }
  }
  // Initially Bondi-limited escape
  else if (iRegimeOld == ATMESC_BONDILIM) {
    if (iRegimeNew == ATMESC_ELIM) {
      fprintf(stdout, "Switching from %s to %s at t = %.4lf Myr.\n", saBondi,
              saEnergy, dTime);
    } else if (iRegimeNew == ATMESC_RRLIM) {
      fprintf(stdout, "Switching from %s to %s at t = %.4lf Myr.\n", saBondi,
              saRR, dTime);
    } else if (iRegimeNew == ATMESC_NONE) {
      fprintf(stdout, "Switching from %s to %s at t = %.4lf Myr.\n", saBondi,
              saNone, dTime);
    }
  }
  // Initially None
  else if (iRegimeOld == ATMESC_NONE) {
    if (iRegimeNew == ATMESC_ELIM) {
      fprintf(stdout, "Switching from %s to %s at t = %.4lf Myr.\n", saNone,
              saEnergy, dTime);
    } else if (iRegimeNew == ATMESC_RRLIM) {
      fprintf(stdout, "Switching from %s to %s at t = %.4lf Myr.\n", saNone,
              saRR, dTime);
    } else if (iRegimeNew == ATMESC_BONDILIM) {
      fprintf(stdout, "Switching from %s to %s at t = %.4lf Myr.\n", saNone,
              saBondi, dTime);
    }
  } else {
    fprintf(stderr, "ERROR: unknown initial atmospheric escape regime: %d\n",
            iRegimeOld);
    exit(1);
  }
}
