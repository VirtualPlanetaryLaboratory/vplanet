/**
    @file stellar.c
    @brief Subroutines that control the evolution of the star.
    @author Rodrigo Luger ([rodluger@gmail.com](mailto:rodluger@gmail.com>))
    @date May 12 2015

    @par Description
    \rst
        This module implements the Baraffe stellar evolution tracks
        :cite:`Baraffe15`, the Ribas XUV evolution model :cite:`Ribas05`,
        and a simple wind/magnetic braking model.
    \endrst

*/

#include "vplanet.h"

void BodyCopyStellar(BODY *dest, BODY *src, int foo, int iNumBodies,
                     int iBody) {
  dest[iBody].dLuminosity      = src[iBody].dLuminosity;
  dest[iBody].dTemperature     = src[iBody].dTemperature;
  dest[iBody].dSatXUVFrac      = src[iBody].dSatXUVFrac;
  dest[iBody].dSatXUVTime      = src[iBody].dSatXUVTime;
  dest[iBody].dXUVBeta         = src[iBody].dXUVBeta;
  dest[iBody].iStellarModel    = src[iBody].iStellarModel;
  dest[iBody].iWindModel       = src[iBody].iWindModel;
  dest[iBody].iXUVModel        = src[iBody].iXUVModel;
  dest[iBody].iMagBrakingModel = src[iBody].iMagBrakingModel;
  dest[iBody].dLXUV            = src[iBody].dLXUV;
  dest[iBody].bRossbyCut       = src[iBody].bRossbyCut;
  dest[iBody].bEvolveRG        = src[iBody].bEvolveRG;
}

/**************** STELLAR options ********************/

void ReadSatXUVFrac(BODY *body, CONTROL *control, FILES *files,
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
        fprintf(stderr, "ERROR: %s must be greater than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dSatXUVFrac = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dSatXUVFrac = options->dDefault;
  }
}

void ReadSatXUVTime(BODY *body, CONTROL *control, FILES *files,
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
      body[iFile - 1].dSatXUVTime =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dSatXUVTime =
            dTmp * fdUnitsTime(control->Units[iFile].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dSatXUVTime = options->dDefault;
  }
}

void ReadXUVBeta(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
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
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be >= 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dXUVBeta = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVBeta = options->dDefault;
  }
}

void ReadStellarModel(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "ba", 2)) {
      body[iFile - 1].iStellarModel = STELLAR_MODEL_BARAFFE;
    } else if (!memcmp(sLower(cTmp), "no", 2)) {
      body[iFile - 1].iStellarModel = STELLAR_MODEL_NONE;
    } else if (!memcmp(sLower(cTmp), "pr", 2)) {
      body[iFile - 1].iStellarModel = STELLAR_MODEL_PROXIMACEN;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are BARAFFE, "
                "PROXIMACEN, or NONE.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iStellarModel = STELLAR_MODEL_BARAFFE;
  }
}

void ReadMagBrakingModel(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "re", 2)) {
      body[iFile - 1].iMagBrakingModel = STELLAR_DJDT_RM12;
    } else if (!memcmp(sLower(cTmp), "no", 2)) {
      body[iFile - 1].iMagBrakingModel = STELLAR_DJDT_NONE;
    } else if (!memcmp(sLower(cTmp), "sk", 2)) {
      body[iFile - 1].iMagBrakingModel = STELLAR_DJDT_SK72;
    } else if (!memcmp(sLower(cTmp), "ma", 2)) {
      body[iFile - 1].iMagBrakingModel = STELLAR_DJDT_MA15;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are REINERS, "
                "SKUMANICH, MATT, or NONE.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iMagBrakingModel =
          STELLAR_DJDT_RM12; // Default to Reiners & Mohanty 2012 model
  }
}

void ReadWindModel(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "re", 2)) {
      body[iFile - 1].iWindModel = STELLAR_MODEL_REINERS;
    } else if (!memcmp(sLower(cTmp), "no", 2)) {
      body[iFile - 1].iWindModel = STELLAR_MODEL_NONE;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are REINERS or "
                "NONE.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iWindModel = STELLAR_MODEL_REINERS;
  }
}

void ReadXUVModel(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "ri", 2)) {
      body[iFile - 1].iXUVModel = STELLAR_MODEL_RIBAS;
    } else if (!memcmp(sLower(cTmp), "no", 2)) {
      body[iFile - 1].iXUVModel = STELLAR_MODEL_NONE;
    } else if (!memcmp(sLower(cTmp), "re", 2)) {
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr, "WARNING: The REINERS XUV model has serious issues. "
                        "The recommended model is RIBAS.\n");
      }
      body[iFile - 1].iXUVModel = STELLAR_MODEL_REINERS;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are RIBAS, REINERS "
                "or NONE.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iXUVModel = STELLAR_MODEL_RIBAS;
  }
}

void ReadHZModel(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "k", 1)) {
      body[iFile - 1].iXUVModel = HZ_MODEL_KOPPARAPU;
      // We should add more!
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are KOPPARAPU13.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iHZModel = HZ_MODEL_KOPPARAPU;
  }
}

void ReadRossbyCut(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bRossbyCut = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].bRossbyCut = 0; // Default to NOT using Rossby cut model
    }
  }
}


void ReadEvolveRG(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bEvolveRG = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].bEvolveRG = 1; // Default to evolve RG
    }
  }
}

/* Halts */

void ReadHaltEndBaraffeGrid(BODY *body, CONTROL *control, FILES *files,
                            OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bEndBaraffeGrid = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &control->Halt[iFile - 1].bEndBaraffeGrid,
                       files->iNumInputs);
    }
  }
}

void InitializeOptionsStellar(OPTIONS *options, fnReadOption fnRead[]) {
  int iOpt, iFile;

  sprintf(options[OPT_SATXUVFRAC].cName, "dSatXUVFrac");
  sprintf(options[OPT_SATXUVFRAC].cDescr, "Saturated XUV luminosity fraction");
  sprintf(options[OPT_SATXUVFRAC].cDefault, "1e-3");
  sprintf(options[OPT_SATXUVFRAC].cDimension, "nd");
  options[OPT_SATXUVFRAC].dDefault   = 1.e-3;
  options[OPT_SATXUVFRAC].iType      = 2;
  options[OPT_SATXUVFRAC].bMultiFile = 1;
  fnRead[OPT_SATXUVFRAC]             = &ReadSatXUVFrac;
  sprintf(
        options[OPT_SATXUVFRAC].cLongDescr,
        "After formation stars emit a nearly constant amount of XUV radiation\n"
        "for a time called the \"saturated\" phase. This parameter sets that "
        "value\n"
        "relative to the total (bolometric) luminosity. Must lie in range "
        "[0,1].");

  sprintf(options[OPT_SATXUVTIME].cName, "dSatXUVTime");
  sprintf(options[OPT_SATXUVTIME].cDescr, "XUV saturation time");
  sprintf(options[OPT_SATXUVTIME].cDefault, "0.1 Gyr");
  sprintf(options[OPT_SATXUVTIME].cDimension, "time");
  options[OPT_SATXUVTIME].dDefault   = 1.e8 * YEARSEC;
  options[OPT_SATXUVTIME].iType      = 0;
  options[OPT_SATXUVTIME].bMultiFile = 1;
  options[OPT_SATXUVTIME].dNeg       = 1e9 * YEARSEC;
  sprintf(options[OPT_SATXUVTIME].cNeg, "Gyr");
  fnRead[OPT_SATXUVTIME] = &ReadSatXUVTime;
  sprintf(options[OPT_SATXUVTIME].cLongDescr,
          "The time a star will remain in its \"saturated\" phase.");

  sprintf(options[OPT_XUVBETA].cName, "dXUVBeta");
  sprintf(options[OPT_XUVBETA].cDescr, "XUV decay power law exponent");
  sprintf(options[OPT_XUVBETA].cDefault, "1.23");
  sprintf(options[OPT_XUVBETA].cDimension, "nd");
  options[OPT_XUVBETA].dDefault   = 1.23;
  options[OPT_XUVBETA].iType      = 2;
  options[OPT_XUVBETA].bMultiFile = 1;
  fnRead[OPT_XUVBETA]             = &ReadXUVBeta;
  sprintf(options[OPT_XUVBETA].cLongDescr,
          "After the \"saturation\" phase, the ratio of the XUV to total "
          "luminosity\n"
          "will follow a power law followinfg this exponent. Units are "
          "gigayears.");

  sprintf(options[OPT_STELLARMODEL].cName, "sStellarModel");
  sprintf(options[OPT_STELLARMODEL].cDescr, "Luminosity evolution model");
  sprintf(options[OPT_STELLARMODEL].cDefault, "BARAFFE");
  sprintf(options[OPT_STELLARMODEL].cValues, "BARAFFE PROXIMA NONE");
  options[OPT_STELLARMODEL].iType      = 3;
  options[OPT_STELLARMODEL].bMultiFile = 1;
  fnRead[OPT_STELLARMODEL]             = &ReadStellarModel;
  sprintf(options[OPT_STELLARMODEL].cLongDescr,
          "If BARAFFE is selected, luminosity, effective temperature, radius, "
          "and\n"
          "radius of gyration will follow the model of Baraffe, I. et al.\n"
          "(2015, A&A, 577, 42). PROXIMA will employ the model from Barnes, R. "
          "et al.\n"
          "(2016, arXiv:1608.06919). NONE will leave them constant.\n");

  sprintf(options[OPT_MAGBRAKINGMODEL].cName, "sMagBrakingModel");
  sprintf(options[OPT_MAGBRAKINGMODEL].cDescr, "Magnetic braking model.");
  sprintf(options[OPT_MAGBRAKINGMODEL].cDefault, "REINERS");
  sprintf(options[OPT_MAGBRAKINGMODEL].cValues,
          "REINERS, SKUMANICH, MATT, NONE");
  options[OPT_MAGBRAKINGMODEL].iType      = 3;
  options[OPT_MAGBRAKINGMODEL].bMultiFile = 1;
  fnRead[OPT_MAGBRAKINGMODEL]             = &ReadMagBrakingModel;
  sprintf(options[OPT_STELLARMODEL].cLongDescr,
          "If REINERS is selected, the stellar magnetic braking model of\n"
          "Reiners & Mohanty (2012, ApJ, 746, 43) is used to modify the "
          "rotation rate.\n"
          "SKUMANICH uses the model from Skumanich, A. (1972, ApJ, 171, 565).\n"
          "MATT uses the model from Matt, S. et al. (2015, ApJ, 799, 23).\n"
          "NONE applies no magnetic torque.\n");

  sprintf(options[OPT_WINDMODEL].cName, "sWindModel");
  sprintf(options[OPT_WINDMODEL].cDescr, "Wind Angular Momentum Loss Model");
  sprintf(options[OPT_WINDMODEL].cDefault, "REINERS");
  options[OPT_WINDMODEL].iType      = 3;
  options[OPT_WINDMODEL].bMultiFile = 1;
  fnRead[OPT_WINDMODEL]             = &ReadWindModel;
  sprintf(options[OPT_WINDMODEL].cLongDescr,
          "If REINERS is selected, the stellar wind model of Reiners and "
          "Mohanty\n"
          "(2012, ApJ, 746, 43) is used to modify the rotation rate.");

  sprintf(options[OPT_XUVMODEL].cName, "sXUVModel");
  sprintf(options[OPT_XUVMODEL].cDescr, "XUV Evolution Model");
  sprintf(options[OPT_XUVMODEL].cDefault, "RIBAS");
  sprintf(options[OPT_XUVMODEL].cValues, "RIBAS REINERS NONE");
  options[OPT_XUVMODEL].iType      = 3;
  options[OPT_XUVMODEL].bMultiFile = 1;
  options[OPT_XUVMODEL].iModuleBit = STELLAR;
  fnRead[OPT_XUVMODEL]             = &ReadXUVModel;
  sprintf(
        options[OPT_XUVMODEL].cLongDescr,
        "This parameter sets the XUV evolution model used in STELLAR. Setting\n"
        "this to RIBAS (default) will evolve the XUV luminosity according to \n"
        "the saturated power law of Ribas et al (2005, ApJ, 611, 680),\n"
        "while setting it to REINERS will use the empirical relations of\n"
        "Reiners, Schussler and Passegger (2014, ApJ, 794, 144). Please note "
        "that\n"
        "the latter model has not been fully vetted. Users may also set this\n"
        "parameter to NONE, in which case the XUV luminosity will remain "
        "constant.");

  sprintf(options[OPT_HZMODEL].cName, "sHZModel");
  sprintf(options[OPT_HZMODEL].cDescr, "Habitable Zone Model: Kopparapu13");
  sprintf(options[OPT_HZMODEL].cDefault, "Kopparapu13");
  options[OPT_HZMODEL].iType      = 3;
  options[OPT_HZMODEL].bMultiFile = 1;
  fnRead[OPT_HZMODEL]             = &ReadHZModel;
  sprintf(options[OPT_HZMODEL].cLongDescr,
          "If KOPPARAPU13 is selected then the Recent Venus, Runaway "
          "Greenhouse,\n"
          "Maximum Greenhouse, and Early Mars habitable zone limits will be\n"
          "calculated from Kopparapu, R. et al. (2013, ApJ, 765, 131).");

  sprintf(options[OPT_HALTENDBARAFFEFGRID].cName, "bHaltEndBaraffeGrid");
  sprintf(options[OPT_HALTENDBARAFFEFGRID].cDescr,
          "Halt when we reach the end of the Baraffe+15 grid?");
  sprintf(options[OPT_HALTENDBARAFFEFGRID].cDefault, "1");
  options[OPT_HALTENDBARAFFEFGRID].iType = 0;
  fnRead[OPT_HALTENDBARAFFEFGRID]        = &ReadHaltEndBaraffeGrid;
  sprintf(options[OPT_HALTENDBARAFFEFGRID].cLongDescr,
          "The BARRAFFE stellar model will only compute parameters until the "
          "end of\n"
          "the main sequence. Setting this flag to 1 will halt the code if the "
          "end\n"
          "of the model grid is reached.");

  sprintf(options[OPT_ROSSBYCUT].cName, "bRossbyCut");
  sprintf(options[OPT_ROSSBYCUT].cDescr,
          "Terminate magnetic braking when Rossby number > 2.08?");
  sprintf(options[OPT_ROSSBYCUT].cDefault, "0"); // XXX Units?
  options[OPT_ROSSBYCUT].iType      = 0;
  options[OPT_ROSSBYCUT].bMultiFile = 1;
  options[OPT_ROSSBYCUT].iModuleBit = STELLAR;
  fnRead[OPT_ROSSBYCUT]             = &ReadRossbyCut;
  sprintf(options[OPT_ROSSBYCUT].cLongDescr,
          "Van Saders, J. et al. (2019, ApJ, 872, 128) find that when the "
          "stellar\n"
          "Rossby number exceeds 2.08, then the magnetic braking is quenched. "
          "This\n"
          "flag enforces that behavior.");

  sprintf(options[OPT_EVOVLERG].cName, "bEvolveRG");
  sprintf(options[OPT_EVOVLERG].cDescr, "Evolve stellar radius of gyration?");
  sprintf(options[OPT_EVOVLERG].cDefault, "1");
  options[OPT_EVOVLERG].iType      = 0;
  options[OPT_EVOVLERG].bMultiFile = 1;
  options[OPT_EVOVLERG].iModuleBit = STELLAR;
  fnRead[OPT_EVOVLERG]             = &ReadEvolveRG;
  sprintf(options[OPT_EVOVLERG].cLongDescr,
          "Set this flag to 0 to ignore the role of mass concentration in "
          "stellar\n"
          "evolution. Only useful for testing purposes.");
}

void ReadOptionsStellar(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, fnReadOption fnRead[],
                        int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTSTELLAR; iOpt < OPTENDSTELLAR; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}

/******************* Verify STELLAR ******************/

void VerifyRotRate(BODY *body, CONTROL *control, OPTIONS *options,
                   UPDATE *update, double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iRot][update[iBody].iRotStellar]     = 1;
  update[iBody].iNumBodies[update[iBody].iRot][update[iBody].iRotStellar] = 1;
  update[iBody].iaBody[update[iBody].iRot][update[iBody].iRotStellar] = malloc(
        update[iBody]
              .iNumBodies[update[iBody].iRot][update[iBody].iRotStellar] *
        sizeof(int));
  update[iBody].iaBody[update[iBody].iRot][update[iBody].iRotStellar][0] =
        iBody;

  update[iBody].pdRotRateStellar =
        &update[iBody]
               .daDerivProc[update[iBody].iRot][update[iBody].iRotStellar];
}

void VerifyLostAngMomStellar(BODY *body, CONTROL *control, OPTIONS *options,
                             UPDATE *update, double dAge, int iBody) {
  update[iBody]
        .iaType[update[iBody].iLostAngMom][update[iBody].iLostAngMomStellar] =
        1;
  update[iBody].iNumBodies[update[iBody].iLostAngMom]
                          [update[iBody].iLostAngMomStellar] = 1;
  update[iBody]
        .iaBody[update[iBody].iLostAngMom][update[iBody].iLostAngMomStellar] =
        malloc(update[iBody].iNumBodies[update[iBody].iLostAngMom]
                                       [update[iBody].iLostAngMomStellar] *
               sizeof(int));
  update[iBody].iaBody[update[iBody].iLostAngMom]
                      [update[iBody].iLostAngMomStellar][0] = iBody;

  update[iBody].pdLostAngMomStellar =
        &update[iBody].daDerivProc[update[iBody].iLostAngMom]
                                  [update[iBody].iLostAngMomStellar];
}

void VerifyLostEngStellar(BODY *body, CONTROL *control, OPTIONS *options,
                          UPDATE *update, double dAge, int iBody) {
  update[iBody].iaType[update[iBody].iLostEng][update[iBody].iLostEngStellar] =
        5;
  update[iBody]
        .iNumBodies[update[iBody].iLostEng][update[iBody].iLostEngStellar] = 1;
  update[iBody].iaBody[update[iBody].iLostEng][update[iBody].iLostEngStellar] =
        malloc(update[iBody].iNumBodies[update[iBody].iLostEng]
                                       [update[iBody].iLostEngStellar] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iLostEng][update[iBody].iLostEngStellar][0] =
        iBody;

  update[iBody].pdLostEngStellar =
        &update[iBody].daDerivProc[update[iBody].iLostEng]
                                  [update[iBody].iLostEngStellar];
}

void VerifyLuminosity(BODY *body, CONTROL *control, OPTIONS *options,
                      UPDATE *update, double dAge, int iBody) {

  // Assign luminosity
  if (body[iBody].iStellarModel == STELLAR_MODEL_BARAFFE) {
    body[iBody].dLuminosity =
          fdLuminosityFunctionBaraffe(body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_LUMINOSITY].iLine[iBody + 1] >= 0) {
      // User specified luminosity, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) {
        printf("INFO: Luminosity set for body %d, but this value will be "
               "computed from the grid.\n",
               iBody);
      }
    }
  } else if (body[iBody].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    body[iBody].dLuminosity =
          fdLuminosityFunctionProximaCen(body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_LUMINOSITY].iLine[iBody + 1] >= 0) {
      // User specified luminosity, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) {
        printf("INFO: Luminosity set for body %d, but this value will be "
               "computed from the grid.\n",
               iBody);
      }
    }
  } else if (body[iBody].iStellarModel == STELLAR_MODEL_NONE) {
    if (options[OPT_LUMINOSITY].iLine[iBody + 1] == -1) {
      // Luminosity must be input if sStellarModel is set to NONE
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr,
                "ERROR: If STELLAR model NONE is selected, then %s must be "
                "set.\n",
                options[OPT_LUMINOSITY].cName);
        exit(EXIT_INPUT);
      }
    }
  }

  update[iBody].iaType[update[iBody].iLuminosity][0]     = 0;
  update[iBody].iNumBodies[update[iBody].iLuminosity][0] = 1;
  update[iBody].iaBody[update[iBody].iLuminosity][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iLuminosity][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iLuminosity][0][0] = iBody;

  update[iBody].pdLuminosityStellar =
        &update[iBody].daDerivProc[update[iBody].iLuminosity]
                                  [0]; // NOTE: This points to the VALUE of the
                                       // luminosity
}

void VerifyRadius(BODY *body, CONTROL *control, OPTIONS *options,
                  UPDATE *update, double dAge, int iBody) {

  // Assign radius
  if (body[iBody].iStellarModel == STELLAR_MODEL_BARAFFE) {
    body[iBody].dRadius =
          fdRadiusFunctionBaraffe(body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_RADIUS].iLine[iBody + 1] >= 0) {
      // User specified radius, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) {
        printf("INFO: Radius set for body %d, but this value will be computed "
               "from the grid.\n",
               iBody);
      }
    }
  } else if (body[iBody].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    body[iBody].dRadius =
          fdRadiusFunctionProximaCen(body[iBody].dAge, body[iBody].dMass);
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

  update[iBody].pdRadiusStellar =
        &update[iBody]
               .daDerivProc[update[iBody].iRadius]
                           [0]; // NOTE: This points to the VALUE of the radius
}

void VerifyRadGyra(BODY *body, CONTROL *control, OPTIONS *options,
                   UPDATE *update, double dAge, int iBody) {

  // If evolving radius of gyration, stellar model must be set
  if (body[iBody].bEvolveRG) {
    // Assign radius
    if (body[iBody].iStellarModel == STELLAR_MODEL_BARAFFE) {
      body[iBody].dRadGyra =
            fdRadGyraFunctionBaraffe(body[iBody].dAge, body[iBody].dMass);
      if (options[OPT_RG].iLine[iBody + 1] >= 0) {
        // User specified radius of gyration, but we're reading it from the
        // grid!
        if (control->Io.iVerbose >= VERBINPUT) {
          printf("INFO: Radius of Gyration set for body %d, but this value "
                 "will be computed from the grid.\n",
                 iBody);
        }
      }
    } else if (body[iBody].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
      if (options[OPT_RG].iLine[iBody + 1] < 0) {
        // User specified radius, but we're reading it from the grid!
        if (control->Io.iVerbose >= VERBINPUT) {
          printf("ERROR: Must set radius of gyration for body %d when using "
                 "Proxima Cen stellar model.\n",
                 iBody);
        }
        exit(1);
      }
    }

    update[iBody].iaType[update[iBody].iRadGyra][0]     = 0;
    update[iBody].iNumBodies[update[iBody].iRadGyra][0] = 1;
    update[iBody].iaBody[update[iBody].iRadGyra][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].iRadGyra][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].iRadGyra][0][0] = iBody;

    update[iBody].pdRadGyraStellar =
          &update[iBody].daDerivProc[update[iBody].iRadGyra]
                                    [0]; // NOTE: This points to the VALUE of
                                         // the radius of gyration
  }
  // Not evolving RG, must be supplied by user
  else {
    if (options[OPT_RG].iLine[iBody + 1] < 0) {
      // User specified radius, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) {
        printf("ERROR: Must set radius of gyration for body %d when its "
               "bEvolveRG = 0.\n",
               iBody);
      }
      exit(1);
    }
  }
}

void VerifyTemperature(BODY *body, CONTROL *control, OPTIONS *options,
                       UPDATE *update, double dAge, int iBody) {

  // Assign temperature
  if (body[iBody].iStellarModel == STELLAR_MODEL_BARAFFE) {
    body[iBody].dTemperature =
          fdTemperatureFunctionBaraffe(body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_TEMPERATURE].iLine[iBody + 1] >= 0) {
      // User specified temperature, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) {
        printf("INFO: Temperature set for body %d, but this value will be "
               "computed from the grid.\n",
               iBody);
      }
    }
  } else if (body[iBody].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    body[iBody].dTemperature =
          fdTemperatureFunctionProximaCen(body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_TEMPERATURE].iLine[iBody + 1] >= 0) {
      // User specified temperature, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) {
        printf("INFO: Temperature set for body %d, but this value will be "
               "computed from the grid.\n",
               iBody);
      }
    }
  }

  update[iBody].iaType[update[iBody].iTemperature][0]     = 0;
  update[iBody].iNumBodies[update[iBody].iTemperature][0] = 1;
  update[iBody].iaBody[update[iBody].iTemperature][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iTemperature][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iTemperature][0][0] = iBody;

  update[iBody].pdTemperatureStellar =
        &update[iBody].daDerivProc[update[iBody].iTemperature]
                                  [0]; // NOTE: This points to the VALUE of the
                                       // temperature
}

void fnPropsAuxStellar(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                       int iBody) {

  // Set rotation period for rossby number calculations
  body[iBody].dRotPer = fdFreqToPer(body[iBody].dRotRate);

  // Update LXUV
  if (body[iBody].iXUVModel == STELLAR_MODEL_REINERS) {

    // REINERS wind model
    double dPer, dLXRay, dLXRaySat, dLEUV;
    dPer = 2 * PI / body[iBody].dRotRate;

    // Unsaturated regime (Reiners, Schussler & Passegger 2014, eqn. (11))
    dLXRay = 1.e-7 * pow(10., 30.71 - 2.01 * log10(dPer / DAYSEC));

    // Saturated regime (Reiners, Schussler & Passegger 2014)
    dLXRaySat = body[iBody].dLuminosity *
                pow(10., -3.12 - 0.11 * log10(dPer / DAYSEC));

    // Take the lesser value
    if (dLXRay > dLXRaySat) {
      dLXRay = dLXRaySat;
    }

    // Sanz-Forcada et al. (2011), eqn (3)
    dLEUV = 1.e7 * pow(10., 4.80 + 0.860 * log10(dLXRay * 1.e-7));

    // NOTE: We should add XRay and EUV to get XUV, but the Sanz-Forcada
    // model above yields unrealistically high EUV luminosities for M dwarfs.
    body[iBody].dLXUV = dLXRay;

  } else if (body[iBody].iXUVModel == STELLAR_MODEL_RIBAS) {

    // RIBAS power-law decay model
    double dAge  = body[iBody].dAge / (1.e9 * YEARSEC);
    double dTMin = body[iBody].dSatXUVTime / (1.e9 * YEARSEC);
    if (dAge >= dTMin) {
      body[iBody].dLXUV = body[iBody].dSatXUVFrac * body[iBody].dLuminosity *
                          pow(dAge / dTMin, -body[iBody].dXUVBeta);
    } else {
      /* No evolution at times earlier than dSatXUVTime */
      body[iBody].dLXUV = body[iBody].dSatXUVFrac * body[iBody].dLuminosity;
    }

  } else {

    // Constant XUV fraction
    body[iBody].dLXUV = body[iBody].dSatXUVFrac * body[iBody].dLuminosity;
  }
}

void fnForceBehaviorStellar(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                            SYSTEM *system, UPDATE *update,
                            fnUpdateVariable ***fnUpdate, int iBody,
                            int iModule) {
  // Nothing
}

void AssignStellarDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                              fnUpdateVariable ***fnUpdate, int iBody) {
  fnUpdate[iBody][update[iBody].iRot][update[iBody].iRotStellar] =
        &fdDRotRateDt;
  fnUpdate[iBody][update[iBody].iLostAngMom][update[iBody].iLostAngMomStellar] =
        &fdDJDtMagBrakingStellar;
  fnUpdate[iBody][update[iBody].iLostEng][update[iBody].iLostEngStellar] =
        &fdDEDtStellar;
  fnUpdate[iBody][update[iBody].iLuminosity][0] =
        &fdLuminosity; // NOTE: This points to the value of the Luminosity!
  fnUpdate[iBody][update[iBody].iRadius][0] =
        &fdRadius; // NOTE: This points to the value of the Radius!
  fnUpdate[iBody][update[iBody].iTemperature][0] =
        &fdTemperature; // NOTE: This points to the value of the Temperature!
  if (body[iBody].bEvolveRG) {
    fnUpdate[iBody][update[iBody].iRadGyra][0] =
          &fdRadGyra; // NOTE: This points to the value of the Radius of
                      // Gyration!
  }
}

void NullStellarDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                            fnUpdateVariable ***fnUpdate, int iBody) {
  fnUpdate[iBody][update[iBody].iRot][update[iBody].iRotStellar] =
        &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iLostAngMom][update[iBody].iLostAngMomStellar] =
        &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iLostEng][update[iBody].iLostEngStellar] =
        &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iLuminosity][0] =
        &fndUpdateFunctionTiny; // NOTE: This points to the value of the
                                // Luminosity!
  fnUpdate[iBody][update[iBody].iRadius][0] =
        &fndUpdateFunctionTiny; // NOTE: This points to the value of the Radius!
  fnUpdate[iBody][update[iBody].iTemperature][0] =
        &fndUpdateFunctionTiny; // NOTE: This points to the value of the
                                // Temperature!
  if (body[iBody].bEvolveRG) {
    fnUpdate[iBody][update[iBody].iRadGyra][0] =
          &fndUpdateFunctionTiny; // NOTE: This points to the value of the
                                  // Radius of Gyration!
  }
}

void VerifyStellar(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   OUTPUT *output, SYSTEM *system, UPDATE *update, int iBody,
                   int iModule) {
  /* Stellar is active for this body if this subroutine is called. */

  if (update[iBody].iNumLuminosity > 1) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Looks like there's more than one equation trying to set "
              "dLuminosity for body %d!",
              iBody);
    }
    exit(EXIT_INPUT);
  }
  VerifyLuminosity(body, control, options, update, body[iBody].dAge, iBody);

  if (update[iBody].iNumRadius > 1) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Looks like there's more than one equation trying to set "
              "dRadius for body %d!",
              iBody);
    }
    exit(EXIT_INPUT);
  }

  if (update[iBody].iNumRadGyra > 1) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Looks like there's more than one equation trying to set "
              "dRadGyra for body %d!",
              iBody);
    }
    exit(EXIT_INPUT);
  }

  VerifyRadius(body, control, options, update, body[iBody].dAge, iBody);
  VerifyRadGyra(body, control, options, update, body[iBody].dAge, iBody);
  VerifyRotRate(body, control, options, update, body[iBody].dAge, iBody);

  if (update[iBody].iNumTemperature > 1) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Looks like there's more than one equation trying to set "
              "dTemperature for body %d!",
              iBody);
    }
    exit(EXIT_INPUT);
  }
  VerifyTemperature(body, control, options, update, body[iBody].dAge, iBody);
  VerifyLostAngMomStellar(body, control, options, update, body[iBody].dAge,
                          iBody);
  VerifyLostEngStellar(body, control, options, update, body[iBody].dAge, iBody);

  control->fnForceBehavior[iBody][iModule]   = &fnForceBehaviorStellar;
  control->fnPropsAux[iBody][iModule]        = &fnPropsAuxStellar;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyStellar;
}

void InitializeModuleStellar(CONTROL *control, MODULE *module) {
  /* Anything Here? */
}

/**************** STELLAR update ****************/

void InitializeUpdateStellar(BODY *body, UPDATE *update, int iBody) {
  // if (body[iBody].dLuminosity > 0) {
  if (update[iBody].iNumLuminosity == 0) {
    update[iBody].iNumVars++;
  }
  update[iBody].iNumLuminosity++;
  //}

  if (body[iBody].dRadius > 0) {
    if (update[iBody].iNumRadius == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumRadius++;
  }

  if (body[iBody].dRadGyra > 0 && body[iBody].bEvolveRG) {
    if (update[iBody].iNumRadGyra == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumRadGyra++;
  }

  // NOTE: Rory and Rodrigo decided to ALWAYS track the rotation evolution of
  // the star, so I'm not going to check whether dRotRate is zero here. If it
  // is, it gets set to its default value, and we track angular momentum
  // conservation from there.
  if (update[iBody].iNumRot == 0) {
    update[iBody].iNumVars++;
  }
  update[iBody].iNumRot++;

  if (update[iBody].iNumLostAngMom == 0) {
    update[iBody].iNumVars++;
  }
  update[iBody].iNumLostAngMom++;

  if (update[iBody].iNumLostEng == 0) {
    update[iBody].iNumVars++;
  }
  update[iBody].iNumLostEng++;

  if (body[iBody].dTemperature > 0) {
    if (update[iBody].iNumTemperature == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumTemperature++;
  }
}

void FinalizeUpdateEccStellar(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  /* Nothing */
}

void FinalizeUpdateLuminosityStellar(BODY *body, UPDATE *update, int *iEqn,
                                     int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iNumLuminosity        = (*iEqn)++;
}

void FinalizeUpdateRadiusStellar(BODY *body, UPDATE *update, int *iEqn,
                                 int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iNumRadius            = (*iEqn)++;
}

void FinalizeUpdateRadGyraStellar(BODY *body, UPDATE *update, int *iEqn,
                                  int iVar, int iBody, int iFoo) {
  if (body[iBody].bEvolveRG) {
    update[iBody].iaModule[iVar][*iEqn] = STELLAR;
    update[iBody].iNumRadGyra           = (*iEqn)++;
  }
}

void FinalizeUpdateRotRateStellar(BODY *body, UPDATE *update, int *iEqn,
                                  int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iRotStellar           = (*iEqn)++;
}

void FinalizeUpdateLostAngMomStellar(BODY *body, UPDATE *update, int *iEqn,
                                     int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iLostAngMomStellar    = (*iEqn)++;
}

void FinalizeUpdateLostEngStellar(BODY *body, UPDATE *update, int *iEqn,
                                  int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iLostEngStellar       = (*iEqn)++;
}

void FinalizeUpdateTemperatureStellar(BODY *body, UPDATE *update, int *iEqn,
                                      int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iNumTemperature       = (*iEqn)++;
}

void FinalizeUpdateOblStellar(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  /* Nothing */
}

void FinalizeUpdateSemiStellar(BODY *body, UPDATE *update, int *iEqn, int iVar,
                               int iBody, int iFoo) {
  /* Nothing */
}


/***************** STELLAR Halts *****************/

int fbHaltEndBaraffeGrid(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                         UPDATE *update, fnUpdateVariable ***fnUpdate,
                         int iBody) {
  if (body[iBody].iStellarModel == STELLAR_MODEL_CONST) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s reached the edge of the luminosity grid at ",
             body[iBody].cName);
      fprintd(stdout, body[iBody].dAge / YEARSEC, io->iSciNot, io->iDigits);
      printf(" years.\n");
    }
    return 1;
  }
  return 0;
}

void CountHaltsStellar(HALT *halt, int *iHalt) {
  if (halt->bEndBaraffeGrid) {
    (*iHalt)++;
  }
}

void VerifyHaltStellar(BODY *body, CONTROL *control, OPTIONS *options,
                       int iBody, int *iHalt) {
  if (control->Halt[iBody].bEndBaraffeGrid) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltEndBaraffeGrid;
  }
}

/************* STELLAR Outputs ******************/


void WriteLuminosity(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dLuminosity;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

void WriteTemperature(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dTemperature;
  // Kelvin only
  fsUnitsTemp(0, cUnit);
}

void WriteLXUV(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char cUnit[]) {
  *dTmp = body[iBody].dLXUV;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

void WriteLXUVFrac(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {
  *dTmp = body[iBody].dLXUV / body[iBody].dLuminosity;
  strcpy(cUnit, "");
}

void WriteRossbyNumber(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  *dTmp =
        body[iBody].dRotPer / fdCranmerSaar2011TauCZ(body[iBody].dTemperature);
  strcpy(cUnit, "");
}

void WriteDRotPerDtStellar(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  int iaBody[1] = {iBody};
  dDeriv        = fdDRotRateDt(body, system, iaBody);

  /* Multiply by dP/domega to get dP/dt */
  *dTmp = dDeriv * (-2 * PI / (body[iBody].dRotRate * body[iBody].dRotRate));
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    strcpy(cUnit, "");
  }
}

void InitializeOutputStellar(OUTPUT *output, fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_LUMINOSITY].cName, "Luminosity");
  sprintf(output[OUT_LUMINOSITY].cDescr, "Luminosity");
  sprintf(output[OUT_LUMINOSITY].cNeg, "LSUN");
  output[OUT_LUMINOSITY].bNeg       = 1;
  output[OUT_LUMINOSITY].dNeg       = 1. / LSUN;
  output[OUT_LUMINOSITY].iNum       = 1;
  output[OUT_LUMINOSITY].iModuleBit = STELLAR;
  fnWrite[OUT_LUMINOSITY]           = &WriteLuminosity;

  // Maybe change to TEFF? XXX
  sprintf(output[OUT_TEMPERATURE].cName, "Temperature");
  sprintf(output[OUT_TEMPERATURE].cDescr, "Effective Temperature");
  output[OUT_TEMPERATURE].bNeg       = 0;
  output[OUT_TEMPERATURE].iNum       = 1;
  output[OUT_TEMPERATURE].iModuleBit = STELLAR;
  fnWrite[OUT_TEMPERATURE]           = &WriteTemperature;

  sprintf(output[OUT_LXUV].cName, "LXUVStellar");
  sprintf(output[OUT_LXUV].cDescr, "Base X-ray/XUV Luminosity");
  sprintf(output[OUT_LXUV].cNeg, "LSUN");
  output[OUT_LXUV].bNeg       = 1;
  output[OUT_LXUV].dNeg       = 1. / LSUN;
  output[OUT_LXUV].iNum       = 1;
  output[OUT_LXUV].iModuleBit = STELLAR;
  fnWrite[OUT_LXUV]           = &WriteLXUV;

  sprintf(output[OUT_LXUVFRAC].cName, "LXUVFrac");
  sprintf(output[OUT_LXUVFRAC].cDescr, "Fraction of luminosity in XUV");
  output[OUT_LXUVFRAC].bNeg       = 0;
  output[OUT_LXUVFRAC].iNum       = 1;
  output[OUT_LXUVFRAC].iModuleBit = STELLAR;
  fnWrite[OUT_LXUVFRAC]           = &WriteLXUVFrac;

  sprintf(output[OUT_ROSSBYNUMBER].cName, "RossbyNumber");
  sprintf(output[OUT_ROSSBYNUMBER].cDescr, "Rossby Number");
  output[OUT_ROSSBYNUMBER].bNeg       = 0;
  output[OUT_ROSSBYNUMBER].iNum       = 1;
  output[OUT_ROSSBYNUMBER].iModuleBit = STELLAR;
  fnWrite[OUT_ROSSBYNUMBER]           = &WriteRossbyNumber;

  sprintf(output[OUT_DROTPERDTSTELLAR].cName, "DRotPerDtStellar");
  sprintf(output[OUT_DROTPERDTSTELLAR].cDescr,
          "Time Rate of Change of Rotation Period in STELLAR");
  sprintf(output[OUT_DROTPERDTSTELLAR].cNeg, "days/Myr");
  output[OUT_DROTPERDTSTELLAR].bNeg       = 1;
  output[OUT_DROTPERDTSTELLAR].dNeg       = DAYSEC / (YEARSEC * 1e6);
  output[OUT_DROTPERDTSTELLAR].iNum       = 1;
  output[OUT_DROTPERDTSTELLAR].iModuleBit = STELLAR;
  fnWrite[OUT_DROTPERDTSTELLAR]           = &WriteDRotPerDtStellar;
}

/************ STELLAR Logging Functions **************/

void LogOptionsStellar(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- STELLAR Options -----\n\n");
  */
}

void LogStellar(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {

  /* Anything here?
  int iOut;

  fprintf(fp,"\n----- STELLAR PARAMETERS ------\n");
  for (iOut=OUTSTARTSTELLAR;iOut<OUTBODYSTARTSTELLAR;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyStellar(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UPDATE *update, fnWriteOutput fnWrite[],
                    FILE *fp, int iBody) {
  int iOut;
  fprintf(fp, "----- STELLAR PARAMETERS (%s)------\n", body[iBody].cName);

  for (iOut = OUTSTARTSTELLAR; iOut < OUTENDSTELLAR; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
  }
}

void AddModuleStellar(CONTROL *control, MODULE *module, int iBody,
                      int iModule) {

  module->iaModule[iBody][iModule] = STELLAR;

  module->fnCountHalts[iBody][iModule]        = &CountHaltsStellar;
  module->fnReadOptions[iBody][iModule]       = &ReadOptionsStellar;
  module->fnLogBody[iBody][iModule]           = &LogBodyStellar;
  module->fnVerify[iBody][iModule]            = &VerifyStellar;
  module->fnAssignDerivatives[iBody][iModule] = &AssignStellarDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullStellarDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltStellar;

  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateStellar;
  module->fnFinalizeUpdateLuminosity[iBody][iModule] =
        &FinalizeUpdateLuminosityStellar;
  module->fnFinalizeUpdateRadius[iBody][iModule] = &FinalizeUpdateRadiusStellar;
  module->fnFinalizeUpdateRadGyra[iBody][iModule] =
        &FinalizeUpdateRadGyraStellar;
  module->fnFinalizeUpdateRot[iBody][iModule] = &FinalizeUpdateRotRateStellar;
  module->fnFinalizeUpdateTemperature[iBody][iModule] =
        &FinalizeUpdateTemperatureStellar;
  module->fnFinalizeUpdateLostAngMom[iBody][iModule] =
        &FinalizeUpdateLostAngMomStellar;
  module->fnFinalizeUpdateLostEng[iBody][iModule] =
        &FinalizeUpdateLostEngStellar;
}

/************* STELLAR Functions ************/

double fdLuminosity(BODY *body, SYSTEM *system, int *iaBody) {
  double foo;
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    foo = fdLuminosityFunctionBaraffe(body[iaBody[0]].dAge,
                                      body[iaBody[0]].dMass);
    if (!isnan(foo)) {
      return foo;
    } else {
      body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
    }
  } else if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    foo = fdLuminosityFunctionProximaCen(body[iaBody[0]].dAge,
                                         body[iaBody[0]].dMass);
    if (!isnan(foo)) {
      return foo;
    } else {
      body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
    }
  }
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_NONE ||
      body[iaBody[0]].iStellarModel == STELLAR_MODEL_CONST) {
    return body[iaBody[0]].dLuminosity;
  } else {
    return 0;
  }
}

double fdRadius(BODY *body, SYSTEM *system, int *iaBody) {
  double foo;
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    foo = fdRadiusFunctionBaraffe(body[iaBody[0]].dAge, body[iaBody[0]].dMass);
    if (!isnan(foo)) {
      return foo;
    } else {
      body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
    }
  } else if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    foo = fdRadiusFunctionProximaCen(body[iaBody[0]].dAge,
                                     body[iaBody[0]].dMass);
    if (!isnan(foo)) {
      return foo;
    } else {
      body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
    }
  }
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_NONE ||
      body[iaBody[0]].iStellarModel == STELLAR_MODEL_CONST) {
    return body[iaBody[0]].dRadius;
  } else {
    return 0;
  }
}

double fdTemperature(BODY *body, SYSTEM *system, int *iaBody) {
  double foo;
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    foo = fdTemperatureFunctionBaraffe(body[iaBody[0]].dAge,
                                       body[iaBody[0]].dMass);
    if (!isnan(foo)) {
      return foo;
    } else {
      body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
    }
  } else if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    foo = fdTemperatureFunctionProximaCen(body[iaBody[0]].dAge,
                                          body[iaBody[0]].dMass);
    if (!isnan(foo)) {
      return foo;
    } else {
      body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
    }
  }
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_NONE ||
      body[iaBody[0]].iStellarModel == STELLAR_MODEL_CONST) {
    return body[iaBody[0]].dTemperature;
  } else {
    return 0;
  }
}

double fdRadGyra(BODY *body, SYSTEM *system, int *iaBody) {

  // If not evolving the radius of gyration, ignore rest of function
  if (!body[iaBody[0]].bEvolveRG) {
    return body[iaBody[0]].dRadGyra;
  }

  double foo;
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    foo = fdRadGyraFunctionBaraffe(body[iaBody[0]].dAge, body[iaBody[0]].dMass);
    if (!isnan(foo)) {
      return foo;
    } else {
      body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
    }
  } else if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    foo = body[iaBody[0]].dRadGyra; // XXX Not implemented!
    if (!isnan(foo)) {
      return foo;
    } else {
      body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
    }
  }
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_NONE ||
      body[iaBody[0]].iStellarModel == STELLAR_MODEL_CONST) {
    return body[iaBody[0]].dRadGyra;
  } else {
    return 0;
  }
}

/*! Compute the instataneous change in stellar radius according to the Baraffe
 * models. Valid for the Baraffe stellar models
 */
double fdDRadiusDtStellar(BODY *body, SYSTEM *system, int *iaBody) {
  // Note: Compute a very simple derivative. NOTE: This won't work if variables
  // like the stellar mass are changing, too! Perhaps it's better to keep track
  // of the previous values of the radius and compute the derivative from those?
  // TODO: Check this.

  if (body[iaBody[0]].iStellarModel != STELLAR_MODEL_BARAFFE) {
    return dTINY;
  }

  // Delta t = 10 years since 10 yr << typical stellar evolution timescales
  double eps = 10.0 * YEARDAY * DAYSEC;
  double dRadMinus, dRadPlus;

  dRadMinus = fdRadiusFunctionBaraffe(body[iaBody[0]].dAge - eps,
                                      body[iaBody[0]].dMass);
  dRadPlus  = fdRadiusFunctionBaraffe(body[iaBody[0]].dAge + eps,
                                      body[iaBody[0]].dMass);

  return (dRadPlus - dRadMinus) / (2. * eps);
}

/*! Compute the instataneous change in stellar radius according to the Baraffe
 * models. Valid for the Baraffe stellar models
 */
double fdDRadGyraDtStellar(BODY *body, SYSTEM *system, int *iaBody) {
  // Note: Compute a very simple derivative. NOTE: This won't work if variables
  // like the stellar mass are changing, too! Perhaps it's better to keep track
  // of the previous values of the radius of gyration and compute the derivative
  // from those? TODO: Check this.

  // If not evolving RG or not using Baraffe+2015 models, ignore RG evolution
  if (body[iaBody[0]].iStellarModel != STELLAR_MODEL_BARAFFE ||
      !body[iaBody[0]].bEvolveRG) {
    return dTINY;
  }

  // Delta t = 10 years since  10 yr << typical stellar evolution timescales
  double eps = 10.0 * YEARDAY * DAYSEC;
  double dRGMinus, dRGPlus;

  dRGMinus = fdRadGyraFunctionBaraffe(body[iaBody[0]].dAge - eps,
                                      body[iaBody[0]].dMass);
  dRGPlus  = fdRadGyraFunctionBaraffe(body[iaBody[0]].dAge + eps,
                                      body[iaBody[0]].dMass);

  return (dRGPlus - dRGMinus) / (2. * eps);
}

/*! Compute instataneous change in potential energy due to stellar radius
 * evolution Note that this energy is released as radiation
 */
double fdDEDtPotConStellar(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0];
  double dDRadiusDt, dEdt;

  // Compute the instataneous change in stellar radius
  dDRadiusDt = fdDRadiusDtStellar(body, system, iaBody);

  dEdt = ALPHA_STRUCT * BIGG * body[iBody].dMass * body[iBody].dMass *
         dDRadiusDt / (body[iBody].dRadius * body[iBody].dRadius);

  return -dEdt; // Negative because I want to store energy removed from system
                // as positive quantity
}

/*! Compute instataneous change in rotational energy due to stellar radius
 * evolution and considering angular momentum conservation
 */
double fdDEDtRotConStellar(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0];
  double dDRadiusDt, dEdt;

  // Compute the instataneous change in stellar radius
  dDRadiusDt = fdDRadiusDtStellar(body, system, iaBody);

  dEdt = -body[iBody].dMass * body[iBody].dRadGyra * body[iBody].dRadGyra *
         body[iBody].dRadius * dDRadiusDt * body[iBody].dRotRate *
         body[iBody].dRotRate;

  return -dEdt; // If E_rot increases, energy un-lost, so negative sign
}

/*! Compute instataneous change in rotational energy due to stellar radius of
 *  gyration evolution and considering angular momentum conservation
 */
double fdDEDtRotRadGyraStellar(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0];
  double dDRGDt, dEdt;

  // Compute the instataneous change in stellar radius
  dDRGDt = fdDRadGyraDtStellar(body, system, iaBody);

  dEdt = -body[iBody].dMass * body[iBody].dRadGyra * body[iBody].dRadius *
         body[iBody].dRadius * dDRGDt * body[iBody].dRotRate *
         body[iBody].dRotRate;

  return -dEdt; // If E_rot increases, energy un-lost, so negative sign
}

/*! Compute instataneous change in rotational energy due to stellar magnetic
 * braking */
double fdDEDtRotBrakeStellar(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0];
  double dJDt, dEdt;

  // Compute the instataneous change in stellar angular momentum
  dJDt = -fdDJDtMagBrakingStellar(body, system, iaBody);

  dEdt = body[iBody].dRotRate * dJDt;

  // dJ/dt < 0 -> lose energy, so store positive amount of lost energy
  return -dEdt;
}

/*! Compute total energy lost due to stellar evolution
 */
double fdDEDtStellar(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDEDtRotBrakeStellar(body, system, iaBody) +
         fdDEDtRotConStellar(body, system, iaBody) +
         fdDEDtPotConStellar(body, system, iaBody) +
         fdDEDtRotRadGyraStellar(body, system, iaBody);
}

/*! Calculate dJ/dt due to magnetic braking.  This is from Reiners & Mohanty
 * (2012); see eqn. (2.14) in Miles Timpe's Master's Thesis. Can also selected
 * Skumanich 1972 model or no model
 */
double fdDJDtMagBrakingStellar(BODY *body, SYSTEM *system, int *iaBody) {
  double dDJDt = 0.0;
  double dOmegaCrit;
  double dTauCZ;
  double dT0;
  double dR0; // Rossby number

  // If using bRossbyCut, magnetic braking terminates when the rossby number
  // exceeds 2.08 following the model of van Saders et al. (2018)
  if (body[iaBody[0]].bRossbyCut) {
    dR0 = body[iaBody[0]].dRotPer /
          fdCranmerSaar2011TauCZ(body[iaBody[0]].dTemperature);

    // Rossby number exceeds threshold: cease magnetic braking
    if (dR0 > ROSSBYCRIT) {
      return dTINY;
    }
  }

  // No magnetic braking
  if (body[iaBody[0]].iMagBrakingModel == STELLAR_DJDT_NONE) {
    return dTINY;
  }

  // Reiners & Mohanty 2012 magnetic braking model
  if (body[iaBody[0]].iMagBrakingModel == STELLAR_DJDT_RM12) {

    if (body[iaBody[0]].dMass > 0.35 * MSUN) {
      dOmegaCrit = RM12OMEGACRIT;
    } else {
      dOmegaCrit = RM12OMEGACRITFULLYCONVEC;
    }
    if (body[iaBody[0]].iWindModel == STELLAR_MODEL_REINERS) {
      if (body[iaBody[0]].dRotRate >= dOmegaCrit) {
        dDJDt = -RM12CONST * body[iaBody[0]].dRotRate *
                pow(body[iaBody[0]].dRadius, 16. / 3.) *
                pow(body[iaBody[0]].dMass, -2. / 3);
      } else {
        dDJDt = -RM12CONST * pow(body[iaBody[0]].dRotRate / dOmegaCrit, 4.) *
                body[iaBody[0]].dRotRate *
                pow(body[iaBody[0]].dRadius, 16. / 3.) *
                pow(body[iaBody[0]].dMass, -2. / 3);
      }
    } else {
      fprintf(stderr, "ERROR! Must set iWindModel to REINERS if using REINERTS "
                      "magnetic braking model!\n");
      exit(1);
    }

    return -dDJDt; // Return positive amount of lost angular momentum
  }
  // Skumanich 1972 empirical model
  else if (body[iaBody[0]].iMagBrakingModel == STELLAR_DJDT_SK72) {
    dDJDt = SK72CONST * body[iaBody[0]].dMass * body[iaBody[0]].dRadius *
            body[iaBody[0]].dRadius;
    dDJDt *= body[iaBody[0]].dRadius * body[iaBody[0]].dRadius *
             body[iaBody[0]].dRadGyra * body[iaBody[0]].dRadGyra;
    dDJDt *= body[iaBody[0]].dRotRate * body[iaBody[0]].dRotRate *
             body[iaBody[0]].dRotRate;

    return dDJDt; // Return positive amount of los angular momentum
  } else if (body[iaBody[0]].iMagBrakingModel == STELLAR_DJDT_MA15) {

    // Compute convective turnover timescale and normalized torque
    dTauCZ = fdCranmerSaar2011TauCZ(body[iaBody[0]].dTemperature);

    // Compute Rossby number
    dR0 = body[iaBody[0]].dRotPer / dTauCZ;

    // Compute Matt+2015 normalized torque
    dT0 = MATT15T0 * pow(body[iaBody[0]].dRadius / RSUN, 3.1) *
          sqrt(body[iaBody[0]].dMass / MSUN);

    // Is the magnetic braking saturated?
    if (dR0 <= MATT15R0SUN / MATT15X) {
      // Saturated
      dDJDt = -dT0 * MATT15X * MATT15X *
              (body[iaBody[0]].dRotRate / MATT15OMEGASUN);
    } else {
      // Unsaturated
      dDJDt = -dT0 * (dTauCZ / MATT15TAUCZ) * (dTauCZ / MATT15TAUCZ) *
              pow(body[iaBody[0]].dRotRate / MATT15OMEGASUN, 3);
    }

    return -dDJDt; // Return positive amount of lost angular momentum
  }
  // No magnetic braking
  else {
    return dTINY;
  }
}

/*! Compute the change in rotation rate when the radius of gyration changes via
 * conservation of angular momentum: dw/dt = -2 dRG/dt * w/RG
 */
double fdDRotRateDtRadGyra(BODY *body, SYSTEM *system, int *iaBody) {

  double dDRGDt;

  // Note that we force dRotRate/dt = 0 in the first 1e6 years, since the
  // stellar rotation is likely locked to the disk rotation (Kevin Covey's
  // suggestion). Also, only applies when you're using a stellar model!
  if (body[iaBody[0]].iStellarModel != STELLAR_MODEL_BARAFFE) {
    return dTINY;
  }

  // Compute the instataneous change in stellar radius
  dDRGDt = fdDRadGyraDtStellar(body, system, iaBody);

  return -2.0 * dDRGDt * body[iaBody[0]].dRotRate / body[iaBody[0]].dRadGyra;
}

/*! Compute the change in rotation rate when the radius changes via conservation
 * of angular momentum:
 * dw/dt = -2 dR/dt * w/R
 */
double fdDRotRateDtCon(BODY *body, SYSTEM *system, int *iaBody) {

  double dDRadiusDt;

  // Note that we force dRotRate/dt = 0 in the first 1e6 years, since the
  // stellar rotation is likely locked to the disk rotation (Kevin Covey's
  // suggestion). Also, only applies when you're using a stellar model!
  if (body[iaBody[0]].iStellarModel != STELLAR_MODEL_BARAFFE) {
    return dTINY;
  }

  // Compute the instataneous change in stellar radius
  dDRadiusDt = fdDRadiusDtStellar(body, system, iaBody);

  return -2.0 * dDRadiusDt * body[iaBody[0]].dRotRate / body[iaBody[0]].dRadius;
}

/*! Compute the change in rotation rate due to magnetic braking via
 * dw/dt = dJ/dt / I for moment of inertia I
 */
double fdDRotRateDtMagBrake(BODY *body, SYSTEM *system, int *iaBody) {

  double dDJDt, dMomIn;

  // Now, let's calculate dJ/dt due to magnetic braking.  Negative since star
  // loses it
  dDJDt = -fdDJDtMagBrakingStellar(body, system, iaBody);

  // Calculate moment of inertia
  dMomIn = body[iaBody[0]].dMass * body[iaBody[0]].dRadGyra *
           body[iaBody[0]].dRadGyra * body[iaBody[0]].dRadius *
           body[iaBody[0]].dRadius;

  return dDJDt / dMomIn;
}

/*! Compute the change in rotation rate when the radius and total angular
 * momentum are changing.
 */
double fdDRotRateDt(BODY *body, SYSTEM *system, int *iaBody) {

  // Contributions due to contraction, r_g changes, and magnetic braking via
  // chain rule dw_net/dt = dw_contraction/dt + dw_mag_braking/dt +
  // dw_rg_changes/dt
  return fdDRotRateDtCon(body, system, iaBody) +
         fdDRotRateDtMagBrake(body, system, iaBody) +
         fdDRotRateDtRadGyra(body, system, iaBody);
}

double fdLuminosityFunctionBaraffe(double dAge, double dMass) {
  int iError;
  double L = fdBaraffe(STELLAR_L, dAge, dMass, 3, &iError);
  if ((iError == STELLAR_ERR_NONE) || (iError == STELLAR_ERR_LINEAR)) {
    return L;
  } else if (iError == STELLAR_ERR_OUTOFBOUNDS_HI ||
             iError == STELLAR_ERR_ISNAN) {
    return NAN;
  } else {
    if (iError == STELLAR_ERR_OUTOFBOUNDS_LO) {
      fprintf(stderr,
              "ERROR: Luninosity out of bounds (low) in fdBaraffe().\n");
    } else if (iError == STELLAR_ERR_FILE) {
      fprintf(stderr,
              "ERROR: File access error in Luminosity routine fdBaraffe().\n");
    } else if (iError == STELLAR_ERR_BADORDER) {
      fprintf(stderr,
              "ERROR: Bad Luminosity interpolation order in routine "
              "fdBaraffe().\n");
    } else {
      fprintf(stderr, "ERROR: Undefined Luminosity error in fdBaraffe().\n");
    }
    exit(EXIT_INT);
  }
}

double fdRadiusFunctionBaraffe(double dAge, double dMass) {
  int iError;
  double R = fdBaraffe(STELLAR_R, dAge, dMass, 3, &iError);
  if ((iError == STELLAR_ERR_NONE) || (iError == STELLAR_ERR_LINEAR)) {
    return R;
  } else if (iError == STELLAR_ERR_OUTOFBOUNDS_HI ||
             iError == STELLAR_ERR_ISNAN) {
    return NAN;
  } else {
    if (iError == STELLAR_ERR_OUTOFBOUNDS_LO) {
      fprintf(stderr, "ERROR: Radius out of bounds (low) in fdBaraffe().\n");
    } else if (iError == STELLAR_ERR_FILE) {
      fprintf(stderr,
              "ERROR: File access error in radius routine fdBaraffe().\n");
    } else if (iError == STELLAR_ERR_BADORDER) {
      fprintf(
            stderr,
            "ERROR: Bad radius interpolation order in routine fdBaraffe().\n");
    } else {
      fprintf(stderr, "ERROR: Undefined radius error in fdBaraffe().\n");
    }
    exit(EXIT_INT);
  }
}

double fdRadGyraFunctionBaraffe(double dAge, double dMass) {
  int iError;
  double rg = fdBaraffe(STELLAR_RG, dAge, dMass, 3, &iError);
  if ((iError == STELLAR_ERR_NONE) || (iError == STELLAR_ERR_LINEAR)) {
    return rg;
  } else if (iError == STELLAR_ERR_OUTOFBOUNDS_HI ||
             iError == STELLAR_ERR_ISNAN) {
    return NAN;
  } else {
    if (iError == STELLAR_ERR_OUTOFBOUNDS_LO) {
      fprintf(
            stderr,
            "ERROR: Radius of gyration out of bounds (low) in fdBaraffe().\n");
    } else if (iError == STELLAR_ERR_FILE) {
      fprintf(stderr, "ERROR: File access error in radius of gyration routine "
                      "fdBaraffe().\n");
    } else if (iError == STELLAR_ERR_BADORDER) {
      fprintf(stderr,
              "ERROR: Bad radius of gyration interpolation order in routine "
              "fdBaraffe().\n");
    } else {
      fprintf(stderr,
              "ERROR: Undefined radius of gyration error in fdBaraffe().\n");
    }
    exit(EXIT_INT);
  }
}

double fdTemperatureFunctionBaraffe(double dAge, double dMass) {
  int iError;
  double T = fdBaraffe(STELLAR_T, dAge, dMass, 3, &iError);
  if ((iError == STELLAR_ERR_NONE) || (iError == STELLAR_ERR_LINEAR)) {
    return T;
  } else if (iError == STELLAR_ERR_OUTOFBOUNDS_HI ||
             iError == STELLAR_ERR_ISNAN) {
    return NAN;
  } else {
    if (iError == STELLAR_ERR_OUTOFBOUNDS_LO) {
      fprintf(stderr,
              "ERROR: Temperature out of bounds (low) in fdBaraffe().\n");
    } else if (iError == STELLAR_ERR_FILE) {
      fprintf(stderr,
              "ERROR: File access error in temperature routine fdBaraffe().\n");
    } else if (iError == STELLAR_ERR_BADORDER) {
      fprintf(stderr,
              "ERROR: Bad temperature interpolation order in routine "
              "fdBaraffe().\n");
    } else {
      fprintf(stderr, "ERROR: Undefined temperature error in fdBaraffe().\n");
    }
    exit(EXIT_INT);
  }
}

double fdLuminosityFunctionProximaCen(double dAge, double dMass) {
  int iError;
  double L = fdProximaCenStellar(PROXIMACEN_L, dAge, dMass, &iError);
  if (iError == PROXIMACEN_ERROR) {
    return NAN;
  } else {
    return L;
  }
}

double fdTemperatureFunctionProximaCen(double dAge, double dMass) {
  int iError;
  double L = fdProximaCenStellar(PROXIMACEN_T, dAge, dMass, &iError);
  if (iError == PROXIMACEN_ERROR) {
    return NAN;
  } else {
    return L;
  }
}

double fdRadiusFunctionProximaCen(double dAge, double dMass) {
  int iError;
  double L = fdProximaCenStellar(PROXIMACEN_R, dAge, dMass, &iError);
  if (iError == PROXIMACEN_ERROR) {
    return NAN;
  } else {
    return L;
  }
}

/* Compute the convective turnover timescale (in seconds) based on Equation 36
   from Cranmer and Saar 2011.  This equation is valid for 3300 <= Teff <= 7000
   K, which pretty much brackets all stellar evolution models considered in
   stellar.
*/
double fdCranmerSaar2011TauCZ(double Teff) {
  double tau =
        314.24 * exp(-(Teff / 1952.5) - pow((Teff / 6250.0), 18)) + 0.002;
  return tau * 86400.0;
}

double fdSurfEnFluxStellar(BODY *body, SYSTEM *system, UPDATE *update,
                           int iBody, int iFoo) {
  // This is silly, but necessary!
  // RORY: I don't think so! -- This function should be set to ReturnOutputZero
  return 0;
}
