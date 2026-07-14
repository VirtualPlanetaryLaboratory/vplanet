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
  dest[iBody].dLuminosity          = src[iBody].dLuminosity;
  dest[iBody].dTemperature         = src[iBody].dTemperature;
  dest[iBody].dSatXUVFrac          = src[iBody].dSatXUVFrac;
  dest[iBody].dSatXUVTime          = src[iBody].dSatXUVTime;
  dest[iBody].dXUVBeta             = src[iBody].dXUVBeta;
  dest[iBody].iStellarModel        = src[iBody].iStellarModel;
  dest[iBody].iWindModel           = src[iBody].iWindModel;
  dest[iBody].iXUVModel            = src[iBody].iXUVModel;
  dest[iBody].iMagBrakingModel     = src[iBody].iMagBrakingModel;
  dest[iBody].dLXUV                = src[iBody].dLXUV;
  dest[iBody].bRossbyCut           = src[iBody].bRossbyCut;
  dest[iBody].bEvolveRG            = src[iBody].bEvolveRG;
  dest[iBody].dLuminosityInitial   = src[iBody].dLuminosityInitial;
  dest[iBody].dLuminosityAmplitude = src[iBody].dLuminosityAmplitude;
  dest[iBody].dLuminosityFrequency = src[iBody].dLuminosityFrequency;
  dest[iBody].dLuminosityPhase     = src[iBody].dLuminosityPhase;

  dest[iBody].dXUVEngleEarlyA = src[iBody].dXUVEngleEarlyA;
  dest[iBody].dXUVEngleEarlyB = src[iBody].dXUVEngleEarlyB;
  dest[iBody].dXUVEngleEarlyC = src[iBody].dXUVEngleEarlyC;
  dest[iBody].dXUVEngleEarlyD = src[iBody].dXUVEngleEarlyD;

  dest[iBody].dXUVEngleMidLateA = src[iBody].dXUVEngleMidLateA;
  dest[iBody].dXUVEngleMidLateB = src[iBody].dXUVEngleMidLateB;
  dest[iBody].dXUVEngleMidLateC = src[iBody].dXUVEngleMidLateC;
  dest[iBody].dXUVEngleMidLateD = src[iBody].dXUVEngleMidLateD;

  dest[iBody].dXUVEngleScatterIntercept =
        src[iBody].dXUVEngleScatterIntercept;
  dest[iBody].dXUVEngleScatterSlope = src[iBody].dXUVEngleScatterSlope;
  dest[iBody].dXUVEngleScatterPivot = src[iBody].dXUVEngleScatterPivot;
  dest[iBody].dXUVEngleScatterScale = src[iBody].dXUVEngleScatterScale;
  dest[iBody].dXUVEngleScatterZ     = src[iBody].dXUVEngleScatterZ;

  dest[iBody].dRotEngleEarlyA = src[iBody].dRotEngleEarlyA;
  dest[iBody].dRotEngleEarlyB = src[iBody].dRotEngleEarlyB;
  dest[iBody].dRotEngleEarlyC = src[iBody].dRotEngleEarlyC;
  dest[iBody].dRotEngleEarlyD = src[iBody].dRotEngleEarlyD;

  dest[iBody].dRotEngleMidA = src[iBody].dRotEngleMidA;
  dest[iBody].dRotEngleMidB = src[iBody].dRotEngleMidB;
  dest[iBody].dRotEngleMidC = src[iBody].dRotEngleMidC;
  dest[iBody].dRotEngleMidD = src[iBody].dRotEngleMidD;

  dest[iBody].dRotEngleLateA = src[iBody].dRotEngleLateA;
  dest[iBody].dRotEngleLateB = src[iBody].dRotEngleLateB;
  dest[iBody].dRotEngleLateC = src[iBody].dRotEngleLateC;
  dest[iBody].dRotEngleLateD = src[iBody].dRotEngleLateD;
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
    } else if (!memcmp(sLower(cTmp), "si", 2)) {
      body[iFile - 1].iStellarModel = STELLAR_MODEL_SINEWAVE;
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
    } else if (!memcmp(sLower(cTmp), "br", 2)) {
      body[iFile - 1].iMagBrakingModel = STELLAR_DJDT_BR21;
    } else if (!memcmp(sLower(cTmp), "engle23e", 8)) {
      body[iFile - 1].iMagBrakingModel = STELLAR_DJDT_ENGLE23EARLY;
    } else if (!memcmp(sLower(cTmp), "engle23m", 8)) {
      body[iFile - 1].iMagBrakingModel = STELLAR_DJDT_ENGLE23MID;
    } else if (!memcmp(sLower(cTmp), "engle23l", 8)) {
      body[iFile - 1].iMagBrakingModel = STELLAR_DJDT_ENGLE23LATE;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are REINERS, "
                "SKUMANICH, MATT, BREIMANN21, ENGLE23EARLY, ENGLE23MID, "
                "ENGLE23LATE, or NONE.\n",
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
                        "The recommended models are RIBAS, ENGLE24EARLY, or "
                        "ENGLE24MIDLATE.\n");
      }
      body[iFile - 1].iXUVModel = STELLAR_MODEL_REINERS;
    } else if (!memcmp(sLower(cTmp), "engle24e", 8)) {
      body[iFile - 1].iXUVModel = STELLAR_MODEL_ENGLE24EARLY;
    } else if (!memcmp(sLower(cTmp), "engle24m", 8)) {
      body[iFile - 1].iXUVModel = STELLAR_MODEL_ENGLE24MIDLATE;
    } else if (!memcmp(sLower(cTmp), "engle24a", 8)) {
      body[iFile - 1].iXUVModel = STELLAR_MODEL_ENGLE24AUTO;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are RIBAS, REINERS, "
                "ENGLE24EARLY, ENGLE24MIDLATE, ENGLE24AUTO, or NONE.\n",
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

void ReadLuminosityAmplitude(BODY *body, CONTROL *control, FILES *files,
                             OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile - 1].dLuminosityAmplitude =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    else
      body[iFile - 1].dLuminosityAmplitude =
            dTmp * fdUnitsPower(control->Units[iFile - 1].iTime,
                                control->Units[iFile - 1].iMass,
                                control->Units[iFile - 1].iLength);
    ;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].dLuminosityAmplitude = options->dDefault;
}

/* The luminosity period is read, which is converted to frequency for more
 * efficient computation later. */
void ReadLuminosityPeriod(BODY *body, CONTROL *control, FILES *files,
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
      body[iFile - 1].dLuminosityFrequency =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dLuminosityFrequency =
            dTmp * fdUnitsTime(control->Units[iFile - 1].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dLuminosityFrequency = options->dDefault;
    }
  }
  body[iFile - 1].dLuminosityFrequency =
        2 * PI / body[iFile - 1].dLuminosityFrequency;
}

void ReadLuminosityPhase(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile - 1].dLuminosityPhase =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    else
      body[iFile - 1].dLuminosityPhase =
            dTmp * fdUnitsAngle(control->Units[iFile - 1].iAngle);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].dLuminosityPhase = options->dDefault;
}

void ReadXUVEngleEarlyA(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleEarlyA = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleEarlyA = options->dDefault;
  }
}

void ReadXUVEngleEarlyB(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleEarlyB = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleEarlyB = options->dDefault;
  }
}

void ReadXUVEngleEarlyC(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleEarlyC = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleEarlyC = options->dDefault;
  }
}

void ReadXUVEngleEarlyD(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleEarlyD = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleEarlyD = options->dDefault;
  }
}

void ReadXUVEngleMidLateA(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleMidLateA = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleMidLateA = options->dDefault;
  }
}

void ReadXUVEngleMidLateB(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleMidLateB = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleMidLateB = options->dDefault;
  }
}

void ReadXUVEngleMidLateC(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleMidLateC = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleMidLateC = options->dDefault;
  }
}

void ReadXUVEngleMidLateD(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleMidLateD = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleMidLateD = options->dDefault;
  }
}

void ReadXUVEngleScatterIntercept(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleScatterIntercept = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleScatterIntercept = options->dDefault;
  }
}

void ReadXUVEngleScatterSlope(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleScatterSlope = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleScatterSlope = options->dDefault;
  }
}

void ReadXUVEngleScatterPivot(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleScatterPivot = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleScatterPivot = options->dDefault;
  }
}

void ReadXUVEngleScatterScale(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleScatterScale = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleScatterScale = options->dDefault;
  }
}

void ReadXUVEngleScatterZ(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dXUVEngleScatterZ = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dXUVEngleScatterZ = options->dDefault;
  }
}

void ReadRotEngleEarlyA(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRotEngleEarlyA = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRotEngleEarlyA = options->dDefault;
  }
}

void ReadRotEngleEarlyB(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRotEngleEarlyB = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRotEngleEarlyB = options->dDefault;
  }
}

void ReadRotEngleEarlyC(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRotEngleEarlyC = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRotEngleEarlyC = options->dDefault;
  }
}

void ReadRotEngleEarlyD(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRotEngleEarlyD = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRotEngleEarlyD = options->dDefault;
  }
}

void ReadRotEngleMidA(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRotEngleMidA = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRotEngleMidA = options->dDefault;
  }
}

void ReadRotEngleMidB(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRotEngleMidB = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRotEngleMidB = options->dDefault;
  }
}

void ReadRotEngleMidC(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRotEngleMidC = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRotEngleMidC = options->dDefault;
  }
}

void ReadRotEngleMidD(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRotEngleMidD = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRotEngleMidD = options->dDefault;
  }
}

void ReadRotEngleLateA(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRotEngleLateA = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRotEngleLateA = options->dDefault;
  }
}

void ReadRotEngleLateB(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRotEngleLateB = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRotEngleLateB = options->dDefault;
  }
}

void ReadRotEngleLateC(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRotEngleLateC = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRotEngleLateC = options->dDefault;
  }
}

void ReadRotEngleLateD(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRotEngleLateD = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRotEngleLateD = options->dDefault;
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

  fvFormattedString(&options[OPT_SATXUVFRAC].cName, "dSatXUVFrac");
  fvFormattedString(&options[OPT_SATXUVFRAC].cDescr,
                    "Saturated XUV luminosity fraction");
  fvFormattedString(&options[OPT_SATXUVFRAC].cDefault, "1e-3");
  fvFormattedString(&options[OPT_SATXUVFRAC].cDimension, "nd");
  options[OPT_SATXUVFRAC].dDefault   = 1.e-3;
  options[OPT_SATXUVFRAC].iType      = 2;
  options[OPT_SATXUVFRAC].bMultiFile = 1;
  fnRead[OPT_SATXUVFRAC]             = &ReadSatXUVFrac;
  fvFormattedString(
        &options[OPT_SATXUVFRAC].cLongDescr,
        "After formation stars emit a nearly constant amount of XUV radiation\n"
        "for a time called the \"saturated\" phase. This parameter sets that "
        "value\n"
        "relative to the total (bolometric) luminosity. Must lie in range "
        "[0,1].");

  fvFormattedString(&options[OPT_SATXUVTIME].cName, "dSatXUVTime");
  fvFormattedString(&options[OPT_SATXUVTIME].cDescr, "XUV saturation time");
  fvFormattedString(&options[OPT_SATXUVTIME].cDefault, "0.1 Gyr");
  fvFormattedString(&options[OPT_SATXUVTIME].cDimension, "time");
  options[OPT_SATXUVTIME].dDefault   = 1.e8 * YEARSEC;
  options[OPT_SATXUVTIME].iType      = 0;
  options[OPT_SATXUVTIME].bMultiFile = 1;
  options[OPT_SATXUVTIME].dNeg       = 1e9 * YEARSEC;
  fvFormattedString(&options[OPT_SATXUVTIME].cNeg, "Gyr");
  fnRead[OPT_SATXUVTIME] = &ReadSatXUVTime;
  fvFormattedString(&options[OPT_SATXUVTIME].cLongDescr,
                    "The time a star will remain in its \"saturated\" phase.");

  fvFormattedString(&options[OPT_XUVBETA].cName, "dXUVBeta");
  fvFormattedString(&options[OPT_XUVBETA].cDescr,
                    "XUV decay power law exponent");
  fvFormattedString(&options[OPT_XUVBETA].cDefault, "1.23");
  fvFormattedString(&options[OPT_XUVBETA].cDimension, "nd");
  options[OPT_XUVBETA].dDefault   = 1.23;
  options[OPT_XUVBETA].iType      = 2;
  options[OPT_XUVBETA].bMultiFile = 1;
  fnRead[OPT_XUVBETA]             = &ReadXUVBeta;
  fvFormattedString(
        &options[OPT_XUVBETA].cLongDescr,
        "After the \"saturation\" phase, the ratio of the XUV to total "
        "luminosity\n"
        "will follow a power law followinfg this exponent. Units are "
        "gigayears.");

  fvFormattedString(&options[OPT_STELLARMODEL].cName, "sStellarModel");
  fvFormattedString(&options[OPT_STELLARMODEL].cDescr,
                    "Stellar evolution model");
  fvFormattedString(&options[OPT_STELLARMODEL].cDefault, "BARAFFE");
  fvFormattedString(&options[OPT_STELLARMODEL].cValues,
                    "BARAFFE PROXIMA SINEWAVE NONE");
  options[OPT_STELLARMODEL].iType      = 3;
  options[OPT_STELLARMODEL].bMultiFile = 1;
  fnRead[OPT_STELLARMODEL]             = &ReadStellarModel;
  fvFormattedString(
        &options[OPT_STELLARMODEL].cLongDescr,
        "If BARAFFE is selected, luminosity, effective temperature, radius, "
        "and\n"
        "radius of gyration will follow the model of Baraffe, I. et al.\n"
        "(2015, A&A, 577, 42). PROXIMA will employ the model from Barnes, R. "
        "et al.\n"
        "(2016, arXiv:1608.06919). SINEWAVE produces oscillatory luminosity.\n"
        "NONE will leave them constant.\n");

  fvFormattedString(&options[OPT_MAGBRAKINGMODEL].cName, "sMagBrakingModel");
  fvFormattedString(&options[OPT_MAGBRAKINGMODEL].cDescr,
                    "Magnetic braking model.");
  fvFormattedString(&options[OPT_MAGBRAKINGMODEL].cDefault, "REINERS");
  fvFormattedString(&options[OPT_MAGBRAKINGMODEL].cValues,
                    "REINERS, SKUMANICH, MATT, NONE");
  options[OPT_MAGBRAKINGMODEL].iType      = 3;
  options[OPT_MAGBRAKINGMODEL].bMultiFile = 1;
  fnRead[OPT_MAGBRAKINGMODEL]             = &ReadMagBrakingModel;
  fvFormattedString(
        &options[OPT_STELLARMODEL].cLongDescr,
        "If REINERS is selected, the stellar magnetic braking model of\n"
        "Reiners & Mohanty (2012, ApJ, 746, 43) is used to modify the "
        "rotation rate.\n"
        "SKUMANICH uses the model from Skumanich, A. (1972, ApJ, 171, 565).\n"
        "MATT uses the model from Matt, S. et al. (2015, ApJ, 799, 23).\n"
        "NONE applies no magnetic torque.\n");

  fvFormattedString(&options[OPT_WINDMODEL].cName, "sWindModel");
  fvFormattedString(&options[OPT_WINDMODEL].cDescr,
                    "Wind Angular Momentum Loss Model");
  fvFormattedString(&options[OPT_WINDMODEL].cDefault, "REINERS");
  options[OPT_WINDMODEL].iType      = 3;
  options[OPT_WINDMODEL].bMultiFile = 1;
  fnRead[OPT_WINDMODEL]             = &ReadWindModel;
  fvFormattedString(
        &options[OPT_WINDMODEL].cLongDescr,
        "If REINERS is selected, the stellar wind model of Reiners and "
        "Mohanty\n"
        "(2012, ApJ, 746, 43) is used to modify the rotation rate.");

  fvFormattedString(&options[OPT_XUVMODEL].cName, "sXUVModel");
  fvFormattedString(&options[OPT_XUVMODEL].cDescr, "XUV Evolution Model");
  fvFormattedString(&options[OPT_XUVMODEL].cDefault, "RIBAS");
  fvFormattedString(&options[OPT_XUVMODEL].cValues, "RIBAS REINERS NONE");
  options[OPT_XUVMODEL].iType      = 3;
  options[OPT_XUVMODEL].bMultiFile = 1;
  options[OPT_XUVMODEL].iModuleBit = STELLAR;
  fnRead[OPT_XUVMODEL]             = &ReadXUVModel;
  fvFormattedString(
        &options[OPT_XUVMODEL].cLongDescr,
        "This parameter sets the XUV evolution model used in STELLAR. Setting\n"
        "this to RIBAS (default) will evolve the XUV luminosity according to \n"
        "the saturated power law of Ribas et al (2005, ApJ, 611, 680),\n"
        "while setting it to REINERS will use the empirical relations of\n"
        "Reiners, Schussler and Passegger (2014, ApJ, 794, 144). Please note "
        "that\n"
        "the latter model has not been fully vetted. Users may also set this\n"
        "parameter to NONE, in which case the XUV luminosity will remain "
        "constant.");

  fvFormattedString(&options[OPT_HZMODEL].cName, "sHZModel");
  fvFormattedString(&options[OPT_HZMODEL].cDescr,
                    "Habitable Zone Model: Kopparapu13");
  fvFormattedString(&options[OPT_HZMODEL].cDefault, "Kopparapu13");
  options[OPT_HZMODEL].iType      = 3;
  options[OPT_HZMODEL].bMultiFile = 1;
  fnRead[OPT_HZMODEL]             = &ReadHZModel;
  fvFormattedString(
        &options[OPT_HZMODEL].cLongDescr,
        "If KOPPARAPU13 is selected then the Recent Venus, Runaway "
        "Greenhouse,\n"
        "Maximum Greenhouse, and Early Mars habitable zone limits will be\n"
        "calculated from Kopparapu, R. et al. (2013, ApJ, 765, 131).");

  fvFormattedString(&options[OPT_HALTENDBARAFFEFGRID].cName,
                    "bHaltEndBaraffeGrid");
  fvFormattedString(&options[OPT_HALTENDBARAFFEFGRID].cDescr,
                    "Halt when we reach the end of the Baraffe+15 grid?");
  fvFormattedString(&options[OPT_HALTENDBARAFFEFGRID].cDefault, "1");
  options[OPT_HALTENDBARAFFEFGRID].iType = 0;
  fnRead[OPT_HALTENDBARAFFEFGRID]        = &ReadHaltEndBaraffeGrid;
  fvFormattedString(
        &options[OPT_HALTENDBARAFFEFGRID].cLongDescr,
        "The BARRAFFE stellar model will only compute parameters until the "
        "end of\n"
        "the main sequence. Setting this flag to 1 will halt the code if the "
        "end\n"
        "of the model grid is reached.");

  fvFormattedString(&options[OPT_ROSSBYCUT].cName, "bRossbyCut");
  fvFormattedString(&options[OPT_ROSSBYCUT].cDescr,
                    "Terminate magnetic braking when Rossby number > 2.08?");
  fvFormattedString(&options[OPT_ROSSBYCUT].cDefault, "0"); // XXX Units?
  options[OPT_ROSSBYCUT].iType      = 0;
  options[OPT_ROSSBYCUT].bMultiFile = 1;
  options[OPT_ROSSBYCUT].iModuleBit = STELLAR;
  fnRead[OPT_ROSSBYCUT]             = &ReadRossbyCut;
  fvFormattedString(
        &options[OPT_ROSSBYCUT].cLongDescr,
        "Van Saders, J. et al. (2019, ApJ, 872, 128) find that when the "
        "stellar\n"
        "Rossby number exceeds 2.08, then the magnetic braking is quenched. "
        "This\n"
        "flag enforces that behavior.");

  fvFormattedString(&options[OPT_EVOVLERG].cName, "bEvolveRG");
  fvFormattedString(&options[OPT_EVOVLERG].cDescr,
                    "Evolve stellar radius of gyration?");
  fvFormattedString(&options[OPT_EVOVLERG].cDefault, "1");
  options[OPT_EVOVLERG].iType      = 0;
  options[OPT_EVOVLERG].bMultiFile = 1;
  options[OPT_EVOVLERG].iModuleBit = STELLAR;
  fnRead[OPT_EVOVLERG]             = &ReadEvolveRG;
  fvFormattedString(
        &options[OPT_EVOVLERG].cLongDescr,
        "Set this flag to 0 to ignore the role of mass concentration in "
        "stellar\n"
        "evolution. Only useful for testing purposes.");

  fvFormattedString(&options[OPT_LUMAMPLITUDE].cName, "dLuminosityAmplitude");
  fvFormattedString(
        &options[OPT_LUMAMPLITUDE].cDescr,
        "Amplitude of luminosity oscillation for SINEWAVE stellar model");
  fvFormattedString(&options[OPT_LUMAMPLITUDE].cDefault, "0.001");
  options[OPT_LUMAMPLITUDE].dDefault   = 0.001;
  options[OPT_LUMAMPLITUDE].iType      = 0;
  options[OPT_LUMAMPLITUDE].bMultiFile = 1;
  options[OPT_LUMAMPLITUDE].iModuleBit = STELLAR;
  options[OPT_LUMAMPLITUDE].dNeg       = LSUN;
  fvFormattedString(&options[OPT_LUMAMPLITUDE].cNeg, "Solar Luminosity (LSUN)");
  fnRead[OPT_LUMAMPLITUDE] = &ReadLuminosityAmplitude;

  fvFormattedString(&options[OPT_LUMPERIOD].cName, "dLuminosityPeriod");
  fvFormattedString(
        &options[OPT_LUMPERIOD].cDescr,
        "Period of luminosity oscillation for SINEWAVE stellar model");
  fvFormattedString(&options[OPT_LUMPERIOD].cDefault, "0.001");
  options[OPT_LUMPERIOD].dDefault   = 0.001;
  options[OPT_LUMPERIOD].iType      = 0;
  options[OPT_LUMPERIOD].bMultiFile = 1;
  options[OPT_LUMPERIOD].iModuleBit = STELLAR;
  options[OPT_LUMPERIOD].dNeg       = YEARSEC;
  fvFormattedString(&options[OPT_LUMPERIOD].cNeg, "Years");
  fnRead[OPT_LUMPERIOD] = &ReadLuminosityPeriod;

  fvFormattedString(&options[OPT_LUMPHASE].cName, "dLuminosityPhase");
  fvFormattedString(
        &options[OPT_LUMPHASE].cDescr,
        "Phase of luminosity oscillation at age=0 for SINEWAVE stellar model");
  fvFormattedString(&options[OPT_LUMPHASE].cDefault, "0");
  options[OPT_LUMPHASE].dDefault   = 0;
  options[OPT_LUMPHASE].iType      = 0;
  options[OPT_LUMPHASE].bMultiFile = 1;
  options[OPT_LUMPHASE].iModuleBit = STELLAR;
  options[OPT_LUMPHASE].dNeg       = DEGRAD;
  fvFormattedString(&options[OPT_LUMPHASE].cNeg, "Degrees");
  fnRead[OPT_LUMPHASE] = &ReadLuminosityPhase;

  iOpt = OPT_XUVENGLEEARLYA;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleEarlyA");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of a in the Engle (2024) empirical age-XUV "
                    "luminosity relationship for M0-2 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-0.4896");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -0.4896;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleEarlyA;

  iOpt = OPT_XUVENGLEEARLYB;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleEarlyB");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of b in the Engle (2024) empirical age-XUV "
                    "luminosity relationship for M0-2 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-3.2128");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -3.2128;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleEarlyB;

  iOpt = OPT_XUVENGLEEARLYC;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleEarlyC");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of c in the Engle (2024) empirical age-XUV "
                    "luminosity relationship for M0-2 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-0.4469");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -0.4469;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleEarlyC;

  iOpt = OPT_XUVENGLEEARLYD;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleEarlyD");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of d in the Engle (2024) empirical age-XUV "
                    "luminosity relationship for M0-2 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-0.2985");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -0.2985;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleEarlyD;

  iOpt = OPT_XUVENGLEMIDLATEA;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleMidLateA");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of a in the Engle (2024) empirical age-XUV "
                    "luminosity relationship for M2.6-6.5 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-0.1456");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -0.1456;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleMidLateA;

  iOpt = OPT_XUVENGLEMIDLATEB;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleMidLateB");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of b in the Engle (2024) empirical age-XUV "
                    "luminosity relationship for M2.6-6.5 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-2.8876");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -2.8876;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleMidLateB;

  iOpt = OPT_XUVENGLEMIDLATEC;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleMidLateC");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of c in the Engle (2024) empirical age-XUV "
                    "luminosity relationship for M2.6-6.5 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-1.8187");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -1.8187;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleMidLateC;

  iOpt = OPT_XUVENGLEMIDLATED;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleMidLateD");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of d in the Engle (2024) empirical age-XUV "
                    "luminosity relationship for M2.6-6.5 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "0.3545");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = 0.3545;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleMidLateD;

  iOpt = OPT_XUVENGLESCATTERINTERCEPT;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleScatterIntercept");
  fvFormattedString(&options[iOpt].cDescr,
                    "log of the Engle XUV intrinsic-scatter law at the pivot age");
  fvFormattedString(&options[iOpt].cDefault, "0");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = 0.0;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleScatterIntercept;

  iOpt = OPT_XUVENGLESCATTERSLOPE;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleScatterSlope");
  fvFormattedString(&options[iOpt].cDescr,
                    "Slope of the Engle XUV intrinsic-scatter law in scaled log age");
  fvFormattedString(&options[iOpt].cDefault, "0");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = 0.0;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleScatterSlope;

  iOpt = OPT_XUVENGLESCATTERPIVOT;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleScatterPivot");
  fvFormattedString(&options[iOpt].cDescr,
                    "Pivot [log10(age/Gyr)] of the Engle XUV intrinsic-scatter law");
  fvFormattedString(&options[iOpt].cDefault, "0");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = 0.0;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleScatterPivot;

  iOpt = OPT_XUVENGLESCATTERSCALE;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleScatterScale");
  fvFormattedString(&options[iOpt].cDescr,
                    "Abscissa scale of the Engle XUV intrinsic-scatter law");
  fvFormattedString(&options[iOpt].cDefault, "1");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = 1.0;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleScatterScale;

  iOpt = OPT_XUVENGLESCATTERZ;
  fvFormattedString(&options[iOpt].cName, "dXUVEngleScatterZ");
  fvFormattedString(&options[iOpt].cDescr,
                    "Per-realization offset within the Engle XUV intrinsic scatter (0 disables the scatter term exactly)");
  fvFormattedString(&options[iOpt].cDefault, "0");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = 0.0;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadXUVEngleScatterZ;

  iOpt = OPT_ROTENGLEEARLYA;
  fvFormattedString(&options[iOpt].cName, "dRotEngleEarlyA");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of a in the Engle & Guinan (2023) empirical "
                    "age-rotation relationship for M0-2 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "0.0621");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = 0.0621;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadRotEngleEarlyA;

  iOpt = OPT_ROTENGLEEARLYB;
  fvFormattedString(&options[iOpt].cName, "dRotEngleEarlyB");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of b in the Engle & Guinan (2023) empirical "
                    "age-rotation relationship for M0-2 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-1.0437");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -1.0437;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadRotEngleEarlyB;

  iOpt = OPT_ROTENGLEEARLYC;
  fvFormattedString(&options[iOpt].cName, "dRotEngleEarlyC");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of c in the Engle & Guinan (2023) empirical "
                    "age-rotation relationship for M0-2 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-0.0528");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -0.0528;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadRotEngleEarlyC;

  iOpt = OPT_ROTENGLEEARLYD;
  fvFormattedString(&options[iOpt].cName, "dRotEngleEarlyD");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of d in the Engle & Guinan (2023) empirical "
                    "age-rotation relationship for M0-2 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-23.4933");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -23.4933;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadRotEngleEarlyD;

  iOpt = OPT_ROTENGLEMIDA;
  fvFormattedString(&options[iOpt].cName, "dRotEngleMidA");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of a in the Engle & Guinan (2023) empirical "
                    "age-rotation relationship for M2.5-M3.5 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "0.0561");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = 0.0561;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadRotEngleMidA;

  iOpt = OPT_ROTENGLEMIDB;
  fvFormattedString(&options[iOpt].cName, "dRotEngleMidB");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of b in the Engle & Guinan (2023) empirical "
                    "age-rotation relationship for M2.5-M3.5 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-0.8900");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -0.8900;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadRotEngleMidB;

  iOpt = OPT_ROTENGLEMIDC;
  fvFormattedString(&options[iOpt].cName, "dRotEngleMidC");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of c in the Engle & Guinan (2023) empirical "
                    "age-rotation relationship for M2.5-M3.5 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-0.0521");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -0.0521;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadRotEngleMidC;

  iOpt = OPT_ROTENGLEMIDD;
  fvFormattedString(&options[iOpt].cName, "dRotEngleMidD");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of d in the Engle & Guinan (2023) empirical "
                    "age-rotation relationship for M2.5-M3.5 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-24.1888");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -24.1888;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadRotEngleMidD;

  iOpt = OPT_ROTENGLELATEA;
  fvFormattedString(&options[iOpt].cName, "dRotEngleLateA");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of a in the Engle & Guinan (2023) empirical "
                    "age-rotation relationship for M4-M6.5 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "0.0251");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = 0.0251;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadRotEngleLateA;

  iOpt = OPT_ROTENGLELATEB;
  fvFormattedString(&options[iOpt].cName, "dRotEngleLateB");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of b in the Engle & Guinan (2023) empirical "
                    "age-rotation relationship for M4-M6.5 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-0.1615");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -0.1615;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadRotEngleLateB;

  iOpt = OPT_ROTENGLELATEC;
  fvFormattedString(&options[iOpt].cName, "dRotEngleLateC");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of c in the Engle & Guinan (2023) empirical "
                    "age-rotation relationship for M4-M6.5 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-0.0212");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -0.0212;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadRotEngleLateC;

  iOpt = OPT_ROTENGLELATED;
  fvFormattedString(&options[iOpt].cName, "dRotEngleLateD");
  fvFormattedString(&options[iOpt].cDescr,
                    "Value of d in the Engle & Guinan (2023) empirical "
                    "age-rotation relationship for M4-M6.5 dwarfs");
  fvFormattedString(&options[iOpt].cDefault, "-25.45");
  fvFormattedString(&options[iOpt].cDimension, "nd");
  options[iOpt].dDefault   = -25.45;
  options[iOpt].iType      = 2;
  options[iOpt].bMultiFile = 1;
  options[iOpt].iModuleBit = STELLAR;
  options[iOpt].bNeg       = 0;
  fnRead[iOpt]             = &ReadRotEngleLateD;
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

void NoSineWaveOptions(BODY *body, CONTROL *control, OPTIONS *options,
                       int iBody) {
  if (options[OPT_LUMAMPLITUDE].iLine[iBody + 1] >= 0) {
    if (control->Io.iVerbose >= VERBINPUT) {
      printf("ERROR: Option %s is only allowed when option %s is set to "
             "SINEWAVE.\n",
             options[OPT_LUMAMPLITUDE].cName, options[OPT_STELLARMODEL].cName);
    }
    DoubleLineExit(options[OPT_LUMAMPLITUDE].cFile[iBody + 1],
                   options[OPT_STELLARMODEL].cFile[iBody + 1],
                   options[OPT_LUMAMPLITUDE].iLine[iBody + 1],
                   options[OPT_STELLARMODEL].iLine[iBody + 1]);
  }

  if (options[OPT_LUMPERIOD].iLine[iBody + 1] >= 0) {
    if (control->Io.iVerbose >= VERBINPUT) {
      printf("ERROR: Option %s is only allowed when option %s is set to "
             "SINEWAVE.\n",
             options[OPT_LUMPERIOD].cName, options[OPT_STELLARMODEL].cName);
    }
    DoubleLineExit(options[OPT_LUMPERIOD].cFile[iBody + 1],
                   options[OPT_STELLARMODEL].cFile[iBody + 1],
                   options[OPT_LUMPERIOD].iLine[iBody + 1],
                   options[OPT_STELLARMODEL].iLine[iBody + 1]);
  }

  if (options[OPT_LUMPHASE].iLine[iBody + 1] >= 0) {
    if (control->Io.iVerbose >= VERBINPUT) {
      printf("ERROR: Option %s is only allowed when option %s is set to "
             "SINEWAVE.\n",
             options[OPT_LUMPHASE].cName, options[OPT_STELLARMODEL].cName);
    }
    DoubleLineExit(options[OPT_LUMPHASE].cFile[iBody + 1],
                   options[OPT_STELLARMODEL].cFile[iBody + 1],
                   options[OPT_LUMPHASE].iLine[iBody + 1],
                   options[OPT_STELLARMODEL].iLine[iBody + 1]);
  }
}

void VerifyStellarBaraffe(BODY *body, CONTROL *control, OPTIONS *options,
                          int iBody) {
  if (options[OPT_LUMINOSITY].iLine[iBody + 1] >= 0) {
    // User specified luminosity, but we're reading it from the grid!
    if (control->Io.iVerbose >= VERBINPUT)
      printf("INFO: Luminosity set for body %d, but this value will be "
             "computed from the grid.\n",
             iBody);
  }
  NoSineWaveOptions(body, control, options, iBody);

  body[iBody].dLuminosity =
        fdLuminosityFunctionBaraffe(body[iBody].dAge, body[iBody].dMass);
}

void VerifyStellarProximaCen(BODY *body, CONTROL *control, OPTIONS *options,
                             int iBody) {
  if (options[OPT_LUMINOSITY].iLine[iBody + 1] >= 0) {
    // User specified luminosity, but we're reading it from the grid!
    if (control->Io.iVerbose >= VERBINPUT)
      printf("INFO: Luminosity set for body %d, but this value will be "
             "computed from the grid.\n",
             iBody);
  }
  NoSineWaveOptions(body, control, options, iBody);

  body[iBody].dLuminosity =
        fdLuminosityFunctionProximaCen(body[iBody].dAge, body[iBody].dMass);
}

void VerifyStellarSineWave(BODY *body, CONTROL *control, OPTIONS *options,
                           int iBody) {
  if (options[OPT_LUMINOSITY].iLine[iBody + 1] == -1) {
    if (control->Io.iVerbose >= VERBINPUT) {
      printf("ERROR: Must set %s when option %s is set to SINEWAVE.\n",
             options[OPT_LUMINOSITY].cName, options[OPT_STELLARMODEL].cName);
    }
    LineExit(options[OPT_STELLARMODEL].cFile[iBody + 1],
             options[OPT_STELLARMODEL].iLine[iBody + 1]);
  }

  body[iBody].dLuminosityInitial = body[iBody].dLuminosity;
}

void VerifyStellarNone(BODY *body, CONTROL *control, OPTIONS *options,
                       int iBody) {
  if (options[OPT_LUMINOSITY].iLine[iBody + 1] == -1) {
    // Luminosity must be input if sStellarModel is set to NONE
    if (control->Io.iVerbose >= VERBINPUT) {
      fprintf(
            stderr,
            "ERROR: If STELLAR model NONE is selected, then %s must be set.\n",
            options[OPT_LUMINOSITY].cName);
      exit(EXIT_INPUT);
    }
  }
  NoSineWaveOptions(body, control, options, iBody);
}

void InitializeUpdateLuminosity(UPDATE *update, int iBody) {
  update[iBody].iaType[update[iBody].iLuminosity][0]     = 0;
  update[iBody].iNumBodies[update[iBody].iLuminosity][0] = 1;
  update[iBody].iaBody[update[iBody].iLuminosity][0]     = malloc(
        update[iBody].iNumBodies[update[iBody].iLuminosity][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iLuminosity][0][0] = iBody;

  // NOTE: This points to the VALUE of the luminosity!
  update[iBody].pdLuminosityStellar =
        &update[iBody].daDerivProc[update[iBody].iLuminosity][0];
}

void VerifyLuminosity(BODY *body, CONTROL *control, OPTIONS *options,
                      UPDATE *update, double dAge, int iBody) {
  if (body[iBody].iStellarModel == STELLAR_MODEL_BARAFFE) {
    VerifyStellarBaraffe(body, control, options, iBody);
  } else if (body[iBody].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    VerifyStellarProximaCen(body, control, options, iBody);
  } else if (body[iBody].iStellarModel == STELLAR_MODEL_SINEWAVE) {
    VerifyStellarSineWave(body, control, options, iBody);
  } else if (body[iBody].iStellarModel == STELLAR_MODEL_NONE) {
    VerifyStellarNone(body, control, options, iBody);
  }
  InitializeUpdateLuminosity(update, iBody);
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
    double dPer, dLXRay, dLXRaySat;
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

    /* Sanz-Forcada et al. (2011), eqn (3)
    Not used here, but maybe useful elsewhere?
    dLEUV = 1.e7 * pow(10., 4.80 + 0.860 * log10(dLXRay * 1.e-7));
    */

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

  } else if (body[iBody].iXUVModel == STELLAR_MODEL_ENGLE24EARLY) {
    body[iBody].dLXUV = fdLXUVEngle(
          body, body[iBody].dXUVEngleEarlyA, body[iBody].dXUVEngleEarlyB,
          body[iBody].dXUVEngleEarlyC, body[iBody].dXUVEngleEarlyD, iBody);
  } else if (body[iBody].iXUVModel == STELLAR_MODEL_ENGLE24MIDLATE) {
    body[iBody].dLXUV = fdLXUVEngle(
          body, body[iBody].dXUVEngleMidLateA, body[iBody].dXUVEngleMidLateB,
          body[iBody].dXUVEngleMidLateC, body[iBody].dXUVEngleMidLateD, iBody);
  } else if (body[iBody].iXUVModel == STELLAR_MODEL_ENGLE24AUTO) {
    /* Per-timestep dispatch on stellar mass.  Boundary at 0.4 Msun. */
    if (body[iBody].dMass >= 0.4 * MSUN) {
      body[iBody].dLXUV = fdLXUVEngle(
            body, body[iBody].dXUVEngleEarlyA, body[iBody].dXUVEngleEarlyB,
            body[iBody].dXUVEngleEarlyC, body[iBody].dXUVEngleEarlyD, iBody);
    } else {
      body[iBody].dLXUV = fdLXUVEngle(
            body, body[iBody].dXUVEngleMidLateA, body[iBody].dXUVEngleMidLateB,
            body[iBody].dXUVEngleMidLateC, body[iBody].dXUVEngleMidLateD, iBody);
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
  if (body[iBody].iStellarModel == STELLAR_MODEL_SINEWAVE) {
    body[iBody].dLXUV = body[iBody].dSatXUVFrac * body[iBody].dLuminosity;
  }

  if (body[iBody].iMagBrakingModel == STELLAR_DJDT_ENGLE23EARLY ||
      body[iBody].iMagBrakingModel == STELLAR_DJDT_ENGLE23MID ||
      body[iBody].iMagBrakingModel == STELLAR_DJDT_ENGLE23LATE) {
    body[iBody].dRotPer  = fdRotationPeriodEngle(body, iBody);
    body[iBody].dRotRate = fdPerToFreq(body[iBody].dRotPer);
  }
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

void VerifyTwoEngleOptionsNotSet(BODY *body, CONTROL *control, FILES *files,
                                 OPTIONS *options, char cModel[OPTLEN],
                                 int iOptionModel, int iOptionConstant,
                                 int iBody) {
  if (options[iOptionModel].iLine[iBody + 1] >= 0 &&
      options[iOptionConstant].iLine[iBody + 1] >= 0) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr, "ERROR: Cannot set %s if %s is set to %s.\n",
              options[iOptionConstant].cName, options[iOptionModel].cName,
              cModel);
    }
    DoubleLineExit(files->Infile[iBody + 1].cIn, files->Infile[iBody + 1].cIn,
                   options[iOptionConstant].iLine[iBody + 1],
                   options[iOptionModel].iLine[iBody + 1]);
  }
}

void VerifyEngleMassSpectralType(BODY *body, CONTROL *control, FILES *files,
                                 OPTIONS *options, char cModel[LINE],
                                 double dMaxMass, double dMinMass,
                                 int iOptionModel, int iBody) {

  if (body[iBody].dMass > dMaxMass * MSUN ||
      body[iBody].dMass <= dMinMass * MSUN) {
    if (control->Io.iVerbose >= VERBINPUT) {
      fprintf(stderr,
              "ERROR: Stellar mass must be between %.2e and %.2e solar "
              "masses to use the %s model.",
              dMinMass, dMaxMass, cModel);
    }
    DoubleLineExit(files->Infile[iBody + 1].cIn, files->Infile[iBody + 1].cIn,
                   options[iOptionModel].iLine[iBody + 1],
                   options[OPT_MASS].iLine[iBody + 1]);
  }
}

void VerifyNoRotEngleEarlyOptions(BODY *body, CONTROL *control, FILES *files,
                                  OPTIONS *options, char cModel[LINE],
                                  int iBody) {
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_MAGBRAKINGMODEL, OPT_ROTENGLEEARLYA, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_MAGBRAKINGMODEL, OPT_ROTENGLEEARLYB, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_MAGBRAKINGMODEL, OPT_ROTENGLEEARLYC, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_MAGBRAKINGMODEL, OPT_ROTENGLEEARLYD, iBody);
}

void VerifyNoRotEngleMidOptions(BODY *body, CONTROL *control, FILES *files,
                                OPTIONS *options, char cModel[LINE],
                                int iBody) {
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_MAGBRAKINGMODEL, OPT_ROTENGLEMIDA, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_MAGBRAKINGMODEL, OPT_ROTENGLEMIDB, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_MAGBRAKINGMODEL, OPT_ROTENGLEMIDC, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_MAGBRAKINGMODEL, OPT_ROTENGLEMIDD, iBody);
}

void VerifyNoRotEngleLateOptions(BODY *body, CONTROL *control, FILES *files,
                                 OPTIONS *options, char cModel[LINE],
                                 int iBody) {
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_MAGBRAKINGMODEL, OPT_ROTENGLELATEA, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_MAGBRAKINGMODEL, OPT_ROTENGLELATEB, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_MAGBRAKINGMODEL, OPT_ROTENGLELATEC, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_MAGBRAKINGMODEL, OPT_ROTENGLELATED, iBody);
}

void VerifyRotRateEngleEarly(BODY *body, CONTROL *control, FILES *files,
                             OPTIONS *options, int iBody) {
  char cModel[LINE] = "ENGLE23EARLY magnetic braking";
  double dMinMass   = 0.4;
  double dMaxMass   = 0.6;

  VerifyEngleMassSpectralType(body, control, files, options, cModel, dMaxMass,
                              dMinMass, OPT_MAGBRAKINGMODEL, iBody);
  VerifyNoRotEngleMidOptions(body, control, files, options, cModel, iBody);
  VerifyNoRotEngleLateOptions(body, control, files, options, cModel, iBody);
}

void VerifyRotRateEngleMid(BODY *body, CONTROL *control, FILES *files,
                           OPTIONS *options, int iBody) {
  char cModel[LINE] = "ENGLE23MID magnetic braking";
  double dMinMass   = 0.2;
  double dMaxMass   = 0.4;

  VerifyEngleMassSpectralType(body, control, files, options, cModel, dMaxMass,
                              dMinMass, OPT_MAGBRAKINGMODEL, iBody);
  VerifyNoRotEngleEarlyOptions(body, control, files, options, cModel, iBody);
  VerifyNoRotEngleLateOptions(body, control, files, options, cModel, iBody);
}

void VerifyRotRateEngleLate(BODY *body, CONTROL *control, FILES *files,
                            OPTIONS *options, int iBody) {
  char cModel[LINE] = "ENGLE23LATE magnetic braking";
  double dMinMass   = 0.1;
  double dMaxMass   = 0.2;

  VerifyEngleMassSpectralType(body, control, files, options, cModel, dMaxMass,
                              dMinMass, OPT_MAGBRAKINGMODEL, iBody);
  VerifyNoRotEngleEarlyOptions(body, control, files, options, cModel, iBody);
  VerifyNoRotEngleMidOptions(body, control, files, options, cModel, iBody);
}

void VerifyRotRateEngle(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, int iBody) {
  if (body[iBody].iMagBrakingModel == STELLAR_DJDT_ENGLE23EARLY) {
    VerifyRotRateEngleEarly(body, control, files, options, iBody);
  } else if (body[iBody].iMagBrakingModel == STELLAR_DJDT_ENGLE23MID) {
    VerifyRotRateEngleMid(body, control, files, options, iBody);
  } else if (body[iBody].iMagBrakingModel == STELLAR_DJDT_ENGLE23LATE) {
    VerifyRotRateEngleLate(body, control, files, options, iBody);
  }
}

void VerifyXUVEngleEarly(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, int iBody) {
  char cModel[LINE] = "ENGLE24EARLY XUV";
  double dMinMass   = 0.4;
  double dMaxMass   = 0.6;

  VerifyEngleMassSpectralType(body, control, files, options, cModel, dMaxMass,
                              dMinMass, OPT_XUVMODEL, iBody);

  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_XUVMODEL, OPT_XUVENGLEMIDLATEA, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_XUVMODEL, OPT_XUVENGLEMIDLATEB, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_XUVMODEL, OPT_XUVENGLEMIDLATEC, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_XUVMODEL, OPT_XUVENGLEMIDLATED, iBody);
}

void VerifyXUVEngleMidLate(BODY *body, CONTROL *control, FILES *files,
                           OPTIONS *options, int iBody) {
  char cModel[LINE] = "ENGLE24MIDLATE XUV";
  double dMinMass   = 0.1;
  double dMaxMass   = 0.4;

  VerifyEngleMassSpectralType(body, control, files, options, cModel, dMaxMass,
                              dMinMass, OPT_XUVMODEL, iBody);

  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_XUVMODEL, OPT_XUVENGLEEARLYA, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_XUVMODEL, OPT_XUVENGLEEARLYB, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_XUVMODEL, OPT_XUVENGLEEARLYC, iBody);
  VerifyTwoEngleOptionsNotSet(body, control, files, options, cModel,
                              OPT_XUVMODEL, OPT_XUVENGLEEARLYD, iBody);
}

void VerifyXUVEngleAuto(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, int iBody) {
  /* Engle24Auto: pick Early or MidLate per-timestep based on dMass.  Allow
     up to 25% extrapolation outside the calibrated union range
     [0.1, 0.6] M_sun, but exit for masses more than 25% out of bounds. */
  char cModel[LINE] = "ENGLE24AUTO XUV";
  double dMinSafe   = 0.075;
  double dMaxSafe   = 0.750;

  VerifyEngleMassSpectralType(body, control, files, options, cModel,
                              dMaxSafe, dMinSafe, OPT_XUVMODEL, iBody);
  /* No VerifyTwoEngleOptionsNotSet calls — Auto requires BOTH Early
     and MidLate coefficient blocks to be present in star.in. */
}

void VerifyXUVEngle(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, int iBody) {
  if (body[iBody].iXUVModel == STELLAR_MODEL_ENGLE24EARLY) {
    VerifyXUVEngleEarly(body, control, files, options, iBody);
  } else if (body[iBody].iXUVModel == STELLAR_MODEL_ENGLE24MIDLATE) {
    VerifyXUVEngleMidLate(body, control, files, options, iBody);
  } else if (body[iBody].iXUVModel == STELLAR_MODEL_ENGLE24AUTO) {
    VerifyXUVEngleAuto(body, control, files, options, iBody);
  }
}

void VerifyStellar(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   OUTPUT *output, SYSTEM *system, UPDATE *update, int iBody,
                   int iModule) {
  /* Stellar is active for this body if this subroutine is called. */

  VerifyXUVEngle(body, control, files, options, iBody);
  VerifyRotRateEngle(body, control, files, options, iBody);


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
                     double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dLuminosity;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

void WriteTemperature(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dTemperature;
  // Kelvin only
  fsUnitsTemp(0, cUnit);
}

void WriteLXUV(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char **cUnit) {
  *dTmp = body[iBody].dLXUV;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

void WriteLXUVFrac(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char **cUnit) {
  *dTmp = body[iBody].dLXUV / body[iBody].dLuminosity;
  fvFormattedString(cUnit, "");
}

void WriteRossbyNumber(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char **cUnit) {
  *dTmp =
        body[iBody].dRotPer / fdCranmerSaar2011TauCZ(body[iBody].dTemperature);
  fvFormattedString(cUnit, "");
}

void WriteWindTorque(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char **cUnit) {
  *dTmp = fdDJDtMagBrakingStellar(body, system, &iBody);
  fvFormattedString(cUnit, "");
}

void WriteDRotPerDtStellar(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char **cUnit) {
  double dDeriv;
  int iPert;

  int iaBody[1] = {iBody};
  dDeriv        = fdDRotRateDt(body, system, iaBody);

  /* Multiply by dP/domega to get dP/dt */
  *dTmp = dDeriv * (-2 * PI / (body[iBody].dRotRate * body[iBody].dRotRate));
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    fvFormattedString(cUnit, "");
  }
}

void InitializeOutputStellar(OUTPUT *output, fnWriteOutput fnWrite[]) {

  fvFormattedString(&output[OUT_LUMINOSITY].cName, "Luminosity");
  fvFormattedString(&output[OUT_LUMINOSITY].cDescr, "Luminosity");
  fvFormattedString(&output[OUT_LUMINOSITY].cNeg, "LSUN");
  output[OUT_LUMINOSITY].bNeg       = 1;
  output[OUT_LUMINOSITY].dNeg       = 1. / LSUN;
  output[OUT_LUMINOSITY].iNum       = 1;
  output[OUT_LUMINOSITY].iModuleBit = STELLAR;
  fnWrite[OUT_LUMINOSITY]           = &WriteLuminosity;

  // Maybe change to TEFF? XXX
  fvFormattedString(&output[OUT_TEMPERATURE].cName, "Temperature");
  fvFormattedString(&output[OUT_TEMPERATURE].cDescr, "Effective Temperature");
  output[OUT_TEMPERATURE].bNeg       = 0;
  output[OUT_TEMPERATURE].iNum       = 1;
  output[OUT_TEMPERATURE].iModuleBit = STELLAR;
  fnWrite[OUT_TEMPERATURE]           = &WriteTemperature;

  fvFormattedString(&output[OUT_LXUV].cName, "LXUVStellar");
  fvFormattedString(&output[OUT_LXUV].cDescr, "Base X-ray/XUV Luminosity");
  fvFormattedString(&output[OUT_LXUV].cNeg, "LSUN");
  output[OUT_LXUV].bNeg       = 1;
  output[OUT_LXUV].dNeg       = 1. / LSUN;
  output[OUT_LXUV].iNum       = 1;
  output[OUT_LXUV].iModuleBit = STELLAR;
  fnWrite[OUT_LXUV]           = &WriteLXUV;

  fvFormattedString(&output[OUT_LXUVFRAC].cName, "LXUVFrac");
  fvFormattedString(&output[OUT_LXUVFRAC].cDescr,
                    "Fraction of luminosity in XUV");
  output[OUT_LXUVFRAC].bNeg       = 0;
  output[OUT_LXUVFRAC].iNum       = 1;
  output[OUT_LXUVFRAC].iModuleBit = STELLAR;
  fnWrite[OUT_LXUVFRAC]           = &WriteLXUVFrac;

  fvFormattedString(&output[OUT_ROSSBYNUMBER].cName, "RossbyNumber");
  fvFormattedString(&output[OUT_ROSSBYNUMBER].cDescr, "Rossby Number");
  output[OUT_ROSSBYNUMBER].bNeg       = 0;
  output[OUT_ROSSBYNUMBER].iNum       = 1;
  output[OUT_ROSSBYNUMBER].iModuleBit = STELLAR;
  fnWrite[OUT_ROSSBYNUMBER]           = &WriteRossbyNumber;

  fvFormattedString(&output[OUT_WINDTORQUE].cName, "WindTorque");
  fvFormattedString(&output[OUT_WINDTORQUE].cDescr, "Stellar Wind Torque");
  output[OUT_WINDTORQUE].bNeg       = 0;
  output[OUT_WINDTORQUE].iNum       = 1;
  output[OUT_WINDTORQUE].iModuleBit = STELLAR;
  fnWrite[OUT_WINDTORQUE]           = &WriteWindTorque;

  fvFormattedString(&output[OUT_DROTPERDTSTELLAR].cName, "DRotPerDtStellar");
  fvFormattedString(&output[OUT_DROTPERDTSTELLAR].cDescr,
                    "Time Rate of Change of Rotation Period in STELLAR");
  fvFormattedString(&output[OUT_DROTPERDTSTELLAR].cNeg, "days/Myr");
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
  double dLuminosity;
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    dLuminosity = fdLuminosityFunctionBaraffe(body[iaBody[0]].dAge,
                                              body[iaBody[0]].dMass);
    if (!isnan(dLuminosity)) {
      return dLuminosity;
    } else {
      body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
    }
  } else if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    dLuminosity = fdLuminosityFunctionProximaCen(body[iaBody[0]].dAge,
                                                 body[iaBody[0]].dMass);
    if (!isnan(dLuminosity)) {
      return dLuminosity;
    } else {
      body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
    }
  } else if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_SINEWAVE) {
    dLuminosity = fdLuminosityFunctionSineWave(body, iaBody[0]);
    return dLuminosity;
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
      body[iaBody[0]].iStellarModel == STELLAR_MODEL_CONST ||
      body[iaBody[0]].iStellarModel == STELLAR_MODEL_SINEWAVE) {
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
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_SINEWAVE) {
    foo = fdEffectiveTemperature(body, iaBody[0]);
    return foo;
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
      body[iaBody[0]].iStellarModel == STELLAR_MODEL_CONST ||
      body[iaBody[0]].iStellarModel == STELLAR_MODEL_SINEWAVE) {
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
  double dRo;        // Rossby number
  double dFracBreak; // fraction of breakup spin rate
  double dbetaSq;    // "beta" factor for Breimann+21 torque, squared
  double dFmag;      // magnetic-activity function for Breimann+21 torque

  // If using bRossbyCut, magnetic braking terminates when the rossby number
  // exceeds 2.08 following the model of van Saders et al. (2018)
  if (body[iaBody[0]].bRossbyCut) {
    dRo = body[iaBody[0]].dRotPer /
          fdCranmerSaar2011TauCZ(body[iaBody[0]].dTemperature);

    // Rossby number exceeds threshold: cease magnetic braking
    if (dRo > ROSSBYCRIT) {
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
  }
  // Matt+15 magnetic braking model
  else if (body[iaBody[0]].iMagBrakingModel == STELLAR_DJDT_MA15) {

    // Compute convective turnover timescale
    dTauCZ = fdCranmerSaar2011TauCZ(body[iaBody[0]].dTemperature);

    // Compute Rossby number
    dRo = body[iaBody[0]].dRotPer / dTauCZ;

    // Compute Matt+2015 normalized torque
    dT0 = MATT15T0 * pow(body[iaBody[0]].dRadius / RSUN, 3.1) *
          sqrt(body[iaBody[0]].dMass / MSUN);

    // Is the magnetic braking saturated?
    if (dRo <= MATT15R0SUN / MATT15X) {
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
  // Breimann+21 magnetic braking model
  else if (body[iaBody[0]].iMagBrakingModel == STELLAR_DJDT_BR21) {

    // Notes:
    //   To be self-consistent (i.e., to reproduce the solar spin rate):
    //   BREIM21TAUSUN should be set to whatever solar value is given by
    //   the turnover timescale used.
    //   Torque normalization BREIM21T0 should be proportional to
    //   the adopted value of solar rotation rate BREIM21OMEGASUN.
    //   To use Breimann+21 "Standard" torque parameters, set:
    //   BREIM21KS = 450.0
    //   BREIM21PS = 0.2
    //   BREIM21P = 2.0
    //   To reduce Breimann+21 torque to Matt+15, set parameters to:
    //   BREIM21KS = 100.0
    //   BREIM21PS = 0.0
    //   BREIM21P = 2.0
    //   dbetaSq = 1.0

    // Compute convective turnover timescale
    dTauCZ = fdCranmerSaar2011TauCZ(body[iaBody[0]].dTemperature);

    // Compute Rossby number, normalized to solar value.
    dRo = BREIM21OMEGASUN * BREIM21TAUSUN / body[iaBody[0]].dRotRate / dTauCZ;

    // Compute fraction of breakup spin rate
    dFracBreak = body[iaBody[0]].dRotRate * pow(body[iaBody[0]].dRadius, 1.5) /
                 sqrt(BIGG * body[iaBody[0]].dMass);

    // Compute beta factor squared (due to centrifugal acceleration of wind)
    dbetaSq = (1.0 + dFracBreak * dFracBreak / (0.0716 * 0.0716));
    // dbetaSq = 1.0;   // to reduce to Matt+15 formulation, for development

    // Compute magnetic-activity function (this captures sat/unsat behavior)
    dFmag = min(BREIM21KS * pow(dRo, BREIM21PS), 1.0 / pow(dRo, BREIM21P));

    // Compute torque normalization.
    dT0 = -BREIM21T0 * 2.0 / BREIM21P *
          pow(body[iaBody[0]].dRadius / RSUN, 3.1) *
          sqrt(body[iaBody[0]].dMass / MSUN);

    // Put it all together for the total torque
    dDJDt = dT0 * body[iaBody[0]].dRotRate / BREIM21OMEGASUN /
            pow(dbetaSq, 0.22) * dFmag;

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

double fdLuminosityFunctionSineWave(BODY *body, int iBody) {
  double dLuminosity =
        body[iBody].dLuminosityInitial +
        body[iBody].dLuminosityAmplitude *
              sin(body[iBody].dAge * body[iBody].dLuminosityFrequency +
                  body[iBody].dLuminosityPhase);
  return dLuminosity;
}

double fdRotationPeriodEngleGeneral(BODY *body, double dA, double dB, double dC,
                                    double dD, int iBody) {
  double dRotationPeriod;

  double dMinRotPerInDays = 0.5;
  double dRotPerInDays    = body[iBody].dRotPer / DAYSEC;
  double dLogAgeInGyr     = log10(body[iBody].dAge / (1e9 * YEARSEC));
  /* D is negative; -D is the rotation-period threshold (days) that selects
     the two-branch fit in Engle & Guinan (2023). */
  if (dRotPerInDays < -dD) {
    dRotationPeriod = (dLogAgeInGyr - dB) / dA;
  } else {
    dRotationPeriod = (dLogAgeInGyr - (dB + dC * dD)) / (dA + dC);
  }

  if (dRotationPeriod < dMinRotPerInDays) {
    dRotationPeriod = dMinRotPerInDays;
  }

  return dRotationPeriod * DAYSEC;
}

double fdRotationPeriodEngle(BODY *body, int iBody) {
  double dRotationPeriod = body[iBody].dRotPer;

  if (body[iBody].iMagBrakingModel == STELLAR_DJDT_ENGLE23EARLY) {
    dRotationPeriod = fdRotationPeriodEngleGeneral(
          body, body[iBody].dRotEngleEarlyA, body[iBody].dRotEngleEarlyB,
          body[iBody].dRotEngleEarlyC, body[iBody].dRotEngleEarlyD, iBody);
  } else if (body[iBody].iMagBrakingModel == STELLAR_DJDT_ENGLE23MID) {
    dRotationPeriod = fdRotationPeriodEngleGeneral(
          body, body[iBody].dRotEngleMidA, body[iBody].dRotEngleMidB,
          body[iBody].dRotEngleMidC, body[iBody].dRotEngleMidD, iBody);
  } else if (body[iBody].iMagBrakingModel == STELLAR_DJDT_ENGLE23LATE) {
    dRotationPeriod = fdRotationPeriodEngleGeneral(
          body, body[iBody].dRotEngleLateA, body[iBody].dRotEngleLateB,
          body[iBody].dRotEngleLateC, body[iBody].dRotEngleLateD, iBody);
  }

  return dRotationPeriod;
}

double fdLXUVEngle(BODY *body, double dA, double dB, double dC, double dD,
                   int iBody) {
  double dLXUV;

  double dAgeInGyr = body[iBody].dAge / (1.e9 * YEARSEC);
  double dLogAge   = log10(dAgeInGyr);
  if (dLogAge >= dD) {
    dLXUV = pow(10., (dA * dLogAge + dB + dC * (dLogAge - dD)));
  } else {
    dLXUV = pow(10., (dA * dLogAge + dB));
  }
  if (body[iBody].dXUVEngleScatterZ != 0) {
    double dSigmaInt =
          exp(body[iBody].dXUVEngleScatterIntercept +
              body[iBody].dXUVEngleScatterSlope *
                    (dLogAge - body[iBody].dXUVEngleScatterPivot) /
                    body[iBody].dXUVEngleScatterScale);
    dLXUV *= pow(10., body[iBody].dXUVEngleScatterZ * dSigmaInt);
  }
  dLXUV *= body[iBody].dLuminosity;

  return dLXUV;
}
