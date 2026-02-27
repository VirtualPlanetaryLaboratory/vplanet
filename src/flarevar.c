/**
  @file flarevar.c
  @brief Coarse, mean-preserving stellar XUV variability module.

  FLAREVAR applies a two-state multiplier to stellar XUV luminosity using
  absolute simulation time so behavior is independent of adaptive timesteps.
  The HIGH/LOW multipliers are constrained to preserve the long-time mean.
*/

#include "vplanet.h"

static unsigned long long ullFlareVarSplitMix64(unsigned long long x) {
  x += 0x9E3779B97F4A7C15ULL;
  x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ULL;
  x = (x ^ (x >> 27)) * 0x94D049BB133111EBULL;
  return x ^ (x >> 31);
}

static double fdFlareVarLowMultiplier(BODY *body, int iBody) {
  return 1. - body[iBody].dFlareVarAmpHigh * body[iBody].dFlareVarDuty /
                    (1. - body[iBody].dFlareVarDuty);
}

static double fdFlareVarHighMultiplier(BODY *body, int iBody) {
  return 1. + body[iBody].dFlareVarAmpHigh;
}

static double fdFlareVarRandom01(int iSeed, long long iBin) {
  unsigned long long ullSeed = (unsigned long long)(unsigned int)iSeed;
  unsigned long long ullBin  = (unsigned long long)iBin;
  unsigned long long ullHash = ullFlareVarSplitMix64(
        ullSeed ^ (ullBin + 0x9E3779B97F4A7C15ULL));

  /* Keep 53 random bits to map exactly into IEEE double mantissa range. */
  return (ullHash >> 11) * (1.0 / 9007199254740992.0);
}

void BodyCopyFlareVar(BODY *dest, BODY *src, int foo, int iNumBodies,
                      int iBody) {
  dest[iBody].bFlareVar        = src[iBody].bFlareVar;
  dest[iBody].iFlareVarModel   = src[iBody].iFlareVarModel;
  dest[iBody].dFlareVarDuty    = src[iBody].dFlareVarDuty;
  dest[iBody].dFlareVarAmpHigh = src[iBody].dFlareVarAmpHigh;
  dest[iBody].dFlareVarDtBin   = src[iBody].dFlareVarDtBin;
  dest[iBody].iFlareVarSeed    = src[iBody].iFlareVarSeed;
  dest[iBody].dFlareVarT0      = src[iBody].dFlareVarT0;
  dest[iBody].dFlareVarMult    = src[iBody].dFlareVarMult;
  dest[iBody].dLXUVBase        = src[iBody].dLXUVBase;
  dest[iBody].dLXUVTotal       = src[iBody].dLXUVTotal;
}

void InitializeBodyFlareVar(BODY *body, CONTROL *control, UPDATE *update,
                            int iBody, int iModule) {
  body[iBody].dFlareVarMult = 1.0;
  body[iBody].dLXUVBase     = body[iBody].dLXUV;
  body[iBody].dLXUVTotal    = body[iBody].dLXUV;
}

void InitializeUpdateTmpBodyFlareVar(BODY *body, CONTROL *control,
                                     UPDATE *update, int iBody) {
  /* Nothing to do. */
}

void ReadFlareVarModel(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (!strcmp(sLower(cTmp), "periodic")) {
      body[iFile - 1].iFlareVarModel = FLAREVAR_MODEL_PERIODIC;
    } else if (!strcmp(sLower(cTmp), "random_bin") ||
               !strcmp(sLower(cTmp), "randombin") ||
               !strcmp(sLower(cTmp), "random")) {
      body[iFile - 1].iFlareVarModel = FLAREVAR_MODEL_RANDOM_BIN;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are PERIODIC or "
                "RANDOM_BIN.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iFlareVarModel = FLAREVAR_MODEL_PERIODIC;
  }
}

void ReadFlareVarDuty(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dFlareVarDuty = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareVarDuty = options->dDefault;
  }
}

void ReadFlareVarAmpHigh(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dFlareVarAmpHigh = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareVarAmpHigh = options->dDefault;
  }
}

void ReadFlareVarDtBin(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareVarDtBin =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareVarDtBin =
            dTmp * fdUnitsTime(control->Units[iFile].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareVarDtBin = options->dDefault;
  }
}

void ReadFlareVarSeed(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  int iTmp;

  AddOptionInt(files->Infile[iFile].cIn, options->cName, &iTmp, &lTmp,
               control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].iFlareVarSeed = iTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iFlareVarSeed = (int)options->dDefault;
  }
}

void ReadFlareVarT0(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareVarT0 =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareVarT0 =
            dTmp * fdUnitsTime(control->Units[iFile].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFlareVarT0 = options->dDefault;
  }
}

void InitializeOptionsFlareVar(OPTIONS *options, fnReadOption fnRead[]) {
  fvFormattedString(&options[OPT_FLAREVARMODEL].cName, "sFlareVarModel");
  fvFormattedString(&options[OPT_FLAREVARMODEL].cDescr,
                    "Coarse flare variability scheduling mode");
  fvFormattedString(&options[OPT_FLAREVARMODEL].cDefault, "PERIODIC");
  fvFormattedString(&options[OPT_FLAREVARMODEL].cValues,
                    "PERIODIC RANDOM_BIN");
  options[OPT_FLAREVARMODEL].iType      = 3;
  options[OPT_FLAREVARMODEL].bMultiFile = 1;
  options[OPT_FLAREVARMODEL].iModuleBit = FLAREVAR;
  fnRead[OPT_FLAREVARMODEL]             = &ReadFlareVarModel;
  fvFormattedString(&options[OPT_FLAREVARMODEL].cLongDescr,
                    "Set the coarse two-state modulation schedule for stellar "
                    "XUV: PERIODIC or RANDOM_BIN.");

  fvFormattedString(&options[OPT_FLAREVARDUTY].cName, "dFlareVarDuty");
  fvFormattedString(&options[OPT_FLAREVARDUTY].cDescr,
                    "Duty cycle of the HIGH state");
  fvFormattedString(&options[OPT_FLAREVARDUTY].cDefault, "0.1");
  fvFormattedString(&options[OPT_FLAREVARDUTY].cDimension, "nd");
  options[OPT_FLAREVARDUTY].dDefault   = 0.1;
  options[OPT_FLAREVARDUTY].iType      = 2;
  options[OPT_FLAREVARDUTY].bMultiFile = 1;
  options[OPT_FLAREVARDUTY].iModuleBit = FLAREVAR;
  fnRead[OPT_FLAREVARDUTY]             = &ReadFlareVarDuty;
  fvFormattedString(
        &options[OPT_FLAREVARDUTY].cLongDescr,
        "Fraction of each coarse bin in the HIGH state. Must satisfy 0 < f < "
        "1.");

  fvFormattedString(&options[OPT_FLAREVARAMPHIGH].cName, "dFlareVarAmpHigh");
  fvFormattedString(&options[OPT_FLAREVARAMPHIGH].cDescr,
                    "HIGH-state amplitude (M_high = 1 + a)");
  fvFormattedString(&options[OPT_FLAREVARAMPHIGH].cDefault, "1.0");
  fvFormattedString(&options[OPT_FLAREVARAMPHIGH].cDimension, "nd");
  options[OPT_FLAREVARAMPHIGH].dDefault   = 1.0;
  options[OPT_FLAREVARAMPHIGH].iType      = 2;
  options[OPT_FLAREVARAMPHIGH].bMultiFile = 1;
  options[OPT_FLAREVARAMPHIGH].iModuleBit = FLAREVAR;
  fnRead[OPT_FLAREVARAMPHIGH]             = &ReadFlareVarAmpHigh;
  fvFormattedString(
        &options[OPT_FLAREVARAMPHIGH].cLongDescr,
        "Amplitude of the HIGH state. LOW state is solved analytically to "
        "preserve mean XUV.");

  fvFormattedString(&options[OPT_FLAREVARDTBIN].cName, "dFlareVarDtBin");
  fvFormattedString(&options[OPT_FLAREVARDTBIN].cDescr,
                    "Coarse variability bin/cycle duration");
  fvFormattedString(&options[OPT_FLAREVARDTBIN].cDefault, "1e5 years");
  fvFormattedString(&options[OPT_FLAREVARDTBIN].cDimension, "time");
  options[OPT_FLAREVARDTBIN].dDefault   = 1e5 * YEARSEC;
  options[OPT_FLAREVARDTBIN].iType      = 0;
  options[OPT_FLAREVARDTBIN].bMultiFile = 1;
  options[OPT_FLAREVARDTBIN].dNeg       = YEARSEC;
  fvFormattedString(&options[OPT_FLAREVARDTBIN].cNeg, "years");
  options[OPT_FLAREVARDTBIN].iModuleBit = FLAREVAR;
  fnRead[OPT_FLAREVARDTBIN]             = &ReadFlareVarDtBin;
  fvFormattedString(&options[OPT_FLAREVARDTBIN].cLongDescr,
                    "Duration of one variability bin/cycle.");

  fvFormattedString(&options[OPT_FLAREVARSEED].cName, "iFlareVarSeed");
  fvFormattedString(&options[OPT_FLAREVARSEED].cDescr,
                    "Deterministic seed for RANDOM_BIN mode");
  fvFormattedString(&options[OPT_FLAREVARSEED].cDefault, "42");
  options[OPT_FLAREVARSEED].dDefault   = 42;
  options[OPT_FLAREVARSEED].iType      = 1;
  options[OPT_FLAREVARSEED].bMultiFile = 1;
  options[OPT_FLAREVARSEED].iModuleBit = FLAREVAR;
  fnRead[OPT_FLAREVARSEED]             = &ReadFlareVarSeed;
  fvFormattedString(
        &options[OPT_FLAREVARSEED].cLongDescr,
        "Seed used to derive deterministic per-bin random states in "
        "RANDOM_BIN mode.");

  fvFormattedString(&options[OPT_FLAREVART0].cName, "dFlareVarT0");
  fvFormattedString(&options[OPT_FLAREVART0].cDescr,
                    "Reference epoch for bin/cycle phase");
  fvFormattedString(&options[OPT_FLAREVART0].cDefault, "0 years");
  fvFormattedString(&options[OPT_FLAREVART0].cDimension, "time");
  options[OPT_FLAREVART0].dDefault   = 0.0;
  options[OPT_FLAREVART0].iType      = 0;
  options[OPT_FLAREVART0].bMultiFile = 1;
  options[OPT_FLAREVART0].dNeg       = YEARSEC;
  fvFormattedString(&options[OPT_FLAREVART0].cNeg, "years");
  options[OPT_FLAREVART0].iModuleBit = FLAREVAR;
  fnRead[OPT_FLAREVART0]             = &ReadFlareVarT0;
  fvFormattedString(&options[OPT_FLAREVART0].cLongDescr,
                    "Reference epoch used to phase bins/cycles.");
}

void ReadOptionsFlareVar(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system,
                         fnReadOption fnRead[], int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTFLAREVAR; iOpt < OPTENDFLAREVAR; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}

void AssignFlareVarDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                               fnUpdateVariable ***fnUpdate, int iBody) {
  /* No differential equations in FLAREVAR. */
}

void NullFlareVarDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody) {
  /* No differential equations in FLAREVAR. */
}

void VerifyFlareVar(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                    OUTPUT *output, SYSTEM *system, UPDATE *update, int iBody,
                    int iModule) {
  double dMLow;

  if (!body[iBody].bStellar) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Must include module STELLAR to run module FLAREVAR.\n");
    }
    LineExit(files->Infile[iBody + 1].cIn, options[OPT_MODULES].iLine[iBody + 1]);
  }

  if (body[iBody].dFlareVarDuty <= 0. || body[iBody].dFlareVarDuty >= 1.) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr, "ERROR: %s must satisfy 0 < f < 1.\n",
              options[OPT_FLAREVARDUTY].cName);
    }
    LineExit(files->Infile[iBody + 1].cIn,
             options[OPT_FLAREVARDUTY].iLine[iBody + 1]);
  }

  if (body[iBody].dFlareVarAmpHigh < 0.) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr, "ERROR: %s must be >= 0.\n",
              options[OPT_FLAREVARAMPHIGH].cName);
    }
    LineExit(files->Infile[iBody + 1].cIn,
             options[OPT_FLAREVARAMPHIGH].iLine[iBody + 1]);
  }

  if (body[iBody].dFlareVarDtBin <= 0.) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr, "ERROR: %s must be > 0.\n",
              options[OPT_FLAREVARDTBIN].cName);
    }
    LineExit(files->Infile[iBody + 1].cIn,
             options[OPT_FLAREVARDTBIN].iLine[iBody + 1]);
  }

  dMLow = fdFlareVarLowMultiplier(body, iBody);
  if (dMLow < 0.) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Reduce a or f. The implied LOW-state multiplier is "
              "negative.\n");
    }
    if (options[OPT_FLAREVARDUTY].iLine[iBody + 1] >= 0) {
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_FLAREVARDUTY].iLine[iBody + 1]);
    }
    if (options[OPT_FLAREVARAMPHIGH].iLine[iBody + 1] >= 0) {
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_FLAREVARAMPHIGH].iLine[iBody + 1]);
    }
    exit(EXIT_INPUT);
  }

  control->fnForceBehavior[iBody][iModule]   = &fnForceBehaviorFlareVar;
  control->fnPropsAux[iBody][iModule]        = &fnPropsAuxFlareVar;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyFlareVar;

  body[iBody].dFlareVarMult = 1.0;
  body[iBody].dLXUVBase     = body[iBody].dLXUV;
  body[iBody].dLXUVTotal    = body[iBody].dLXUV;
}

void fnForceBehaviorFlareVar(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                             SYSTEM *system, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody,
                             int iModule) {
  /* Nothing to force. */
}

double fdFlareVarMultiplier(BODY *body, EVOLVE *evolve, int iBody) {
  double dMHigh = fdFlareVarHighMultiplier(body, iBody);
  double dMLow  = fdFlareVarLowMultiplier(body, iBody);
  double dShift = evolve->dTime - body[iBody].dFlareVarT0;

  if (body[iBody].iFlareVarModel == FLAREVAR_MODEL_PERIODIC) {
    double dPhase = fmod(dShift, body[iBody].dFlareVarDtBin);
    if (dPhase < 0.) {
      dPhase += body[iBody].dFlareVarDtBin;
    }

    if (dPhase < body[iBody].dFlareVarDuty * body[iBody].dFlareVarDtBin) {
      return dMHigh;
    }
    return dMLow;
  }

  if (body[iBody].iFlareVarModel == FLAREVAR_MODEL_RANDOM_BIN) {
    long long iBin = (long long)floor(dShift / body[iBody].dFlareVarDtBin);
    double dRnd    = fdFlareVarRandom01(body[iBody].iFlareVarSeed, iBin);
    if (dRnd < body[iBody].dFlareVarDuty) {
      return dMHigh;
    }
    return dMLow;
  }

  return 1.0;
}

void fnPropsAuxFlareVar(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                        int iBody) {
  body[iBody].dFlareVarMult = fdFlareVarMultiplier(body, evolve, iBody);
  body[iBody].dLXUVTotal    = body[iBody].dLXUVBase * body[iBody].dFlareVarMult;

  if (body[iBody].dLXUVTotal < 0.) {
    body[iBody].dLXUVTotal = 0.;
  }
}

void InitializeUpdateFlareVar(BODY *body, UPDATE *update, int iBody) {
  /* No primary variables in FLAREVAR. */
}

void CountHaltsFlareVar(HALT *halt, int *iNumHalts) {
  /* No halts in FLAREVAR. */
}

void VerifyHaltFlareVar(BODY *body, CONTROL *control, OPTIONS *options,
                        int iBody, int *iHalt) {
  /* No halts in FLAREVAR. */
}

void WriteFlareVarMult(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dFlareVarMult;
  fvFormattedString(cUnit, "");
}

void WriteLXUVBaseFlareVar(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dLXUVBase;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

void WriteLXUVTotalFlareVar(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dLXUVTotal;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

void InitializeOutputFlareVar(OUTPUT *output, fnWriteOutput fnWrite[]) {
  fvFormattedString(&output[OUT_FLAREVARMULT].cName, "FlareVarMult");
  fvFormattedString(&output[OUT_FLAREVARMULT].cDescr,
                    "Instantaneous FLAREVAR XUV multiplier");
  output[OUT_FLAREVARMULT].bNeg       = 0;
  output[OUT_FLAREVARMULT].iNum       = 1;
  output[OUT_FLAREVARMULT].iModuleBit = STELLAR + FLAREVAR;
  fnWrite[OUT_FLAREVARMULT]           = &WriteFlareVarMult;

  fvFormattedString(&output[OUT_LXUVBASE].cName, "LXUVBase");
  fvFormattedString(&output[OUT_LXUVBASE].cDescr,
                    "Baseline stellar XUV luminosity before FLAREVAR");
  fvFormattedString(&output[OUT_LXUVBASE].cNeg, "LSUN");
  output[OUT_LXUVBASE].bNeg       = 1;
  output[OUT_LXUVBASE].dNeg       = 1. / LSUN;
  output[OUT_LXUVBASE].iNum       = 1;
  output[OUT_LXUVBASE].iModuleBit = STELLAR + FLAREVAR;
  fnWrite[OUT_LXUVBASE]           = &WriteLXUVBaseFlareVar;

  fvFormattedString(&output[OUT_LXUVTOTAL].cName, "LXUVTotal");
  fvFormattedString(&output[OUT_LXUVTOTAL].cDescr,
                    "Effective stellar XUV luminosity after FLAREVAR");
  fvFormattedString(&output[OUT_LXUVTOTAL].cNeg, "LSUN");
  output[OUT_LXUVTOTAL].bNeg       = 1;
  output[OUT_LXUVTOTAL].dNeg       = 1. / LSUN;
  output[OUT_LXUVTOTAL].iNum       = 1;
  output[OUT_LXUVTOTAL].iModuleBit = STELLAR + FLAREVAR;
  fnWrite[OUT_LXUVTOTAL]           = &WriteLXUVTotalFlareVar;
}

void LogOptionsFlareVar(CONTROL *control, FILE *fp) {
  /* Nothing to log here. */
}

void LogBodyFlareVar(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UPDATE *update, fnWriteOutput fnWrite[],
                     FILE *fp, int iBody) {
  int iOut;

  fprintf(fp, "----- FLAREVAR PARAMETERS (%s)------\n", body[iBody].cName);
  for (iOut = OUTSTARTFLAREVAR; iOut < OUTENDFLAREVAR; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update,
                    fnWrite[iOut], fp, iBody);
    }
  }
}

void AddModuleFlareVar(CONTROL *control, MODULE *module, int iBody,
                       int iModule) {
  module->iaModule[iBody][iModule] = FLAREVAR;

  module->fnCountHalts[iBody][iModule]        = &CountHaltsFlareVar;
  module->fnReadOptions[iBody][iModule]       = &ReadOptionsFlareVar;
  module->fnLogBody[iBody][iModule]           = &LogBodyFlareVar;
  module->fnVerify[iBody][iModule]            = &VerifyFlareVar;
  module->fnAssignDerivatives[iBody][iModule] = &AssignFlareVarDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullFlareVarDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltFlareVar;

  module->fnInitializeBody[iBody][iModule]    = &InitializeBodyFlareVar;
  module->fnInitializeUpdate[iBody][iModule]  = &InitializeUpdateFlareVar;
}
