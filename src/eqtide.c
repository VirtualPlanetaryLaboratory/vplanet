/**
   @file eqtide.c

   @brief Subroutines that control the integration of the tidal
          model. Also includes subroutines that switch between the two models.

   @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

   @date May 29 2014

*/

#include "vplanet.h"

void InitializeControlEqtide(CONTROL *control, int iBody) {

  /* We only want to initialize these values once, but if the user fails to
     instantiate eqtide for body 0, then the code segaults and fixing this is
     hard. So we just re-malloc.
     */
  control->Evolve.bForceEqSpin =
        malloc(control->Evolve.iNumBodies * sizeof(int));
  control->Evolve.dMaxLockDiff =
        malloc(control->Evolve.iNumBodies * sizeof(double));
  control->Evolve.dSyncEcc =
        malloc(control->Evolve.iNumBodies * sizeof(double));
  control->Evolve.bFixOrbit = malloc(control->Evolve.iNumBodies * sizeof(int));
}

/* All the auxiliary properties for EQTIDE calculations need to be included
   in this subroutine! */

void BodyCopyEqtide(BODY *dest, BODY *src, int iTideModel, int iNumBodies,
                    int iBody) {
  int iIndex, iPert;

  dest[iBody].dTidalPowMan = src[iBody].dTidalPowMan;

  dest[iBody].iTidePerts = src[iBody].iTidePerts;

  dest[iBody].dImK2      = src[iBody].dImK2;
  dest[iBody].dImK2Man   = src[iBody].dImK2Man;
  dest[iBody].dImK2Ocean = src[iBody].dImK2Ocean;
  dest[iBody].dImK2Env   = src[iBody].dImK2Env;

  dest[iBody].dK2      = src[iBody].dK2;
  dest[iBody].dK2Man   = src[iBody].dK2Man;
  dest[iBody].dK2Ocean = src[iBody].dK2Ocean;
  dest[iBody].dK2Env   = src[iBody].dK2Env;

  dest[iBody].dObliquity      = src[iBody].dObliquity;
  dest[iBody].dPrecA          = src[iBody].dPrecA;
  dest[iBody].bUseTidalRadius = src[iBody].bUseTidalRadius;
  dest[iBody].dTidalRadius    = src[iBody].dTidalRadius;
  dest[iBody].bTideLock       = src[iBody].bTideLock;
  dest[iBody].bEqtide         = src[iBody].bEqtide;

  if (iBody > 0) {
    dest[iBody].dEccSq        = src[iBody].dEccSq;
    dest[iBody].dLongP        = src[iBody].dLongP;
    dest[iBody].dMeanMotion   = src[iBody].dMeanMotion;
    dest[iBody].dDeccDtEqtide = src[iBody].dDeccDtEqtide;
  }

  if (iTideModel == CPL || iTideModel == DB15) {
    dest[iBody].dTidalQ      = src[iBody].dTidalQ;
    dest[iBody].dTidalQMan   = src[iBody].dTidalQMan;
    dest[iBody].dTidalQOcean = src[iBody].dTidalQOcean;
    dest[iBody].dTidalQEnv   = src[iBody].dTidalQEnv;
  }

  if (iTideModel == CTL) {
    dest[iBody].dTidalTau  = src[iBody].dTidalTau;
    dest[iBody].dTidalBeta = src[iBody].dTidalBeta;
  }

  for (iPert = 0; iPert < src[iBody].iTidePerts; iPert++) {
    dest[iBody].iaTidePerts[iPert] = src[iBody].iaTidePerts[iPert];
  }

  for (iPert = 0; iPert < iNumBodies; iPert++) {
    dest[iBody].daDoblDtEqtide[iPert] = src[iBody].daDoblDtEqtide[iPert];
    if (iTideModel == CPL) {
      dest[iBody].dTidalZ[iPert]   = src[iBody].dTidalZ[iPert];
      dest[iBody].dTidalChi[iPert] = src[iBody].dTidalChi[iPert];
      for (iIndex = 0; iIndex < 10; iIndex++) {
        dest[iBody].iTidalEpsilon[iPert][iIndex] =
              src[iBody].iTidalEpsilon[iPert][iIndex];
      }
    }
    if (iTideModel == CTL) {
      for (iIndex = 0; iIndex < 5; iIndex++) {
        dest[iBody].dTidalF[iPert][iIndex] = src[iBody].dTidalF[iPert][iIndex];
      }
    }
  }
}

void InitializeBodyEqtide(BODY *body, CONTROL *control, UPDATE *update,
                          int iBody, int iModule) {
  body[iBody].iaTidePerts = malloc(body[iBody].iTidePerts * sizeof(int));
  body[iBody].daDoblDtEqtide =
        malloc(control->Evolve.iNumBodies * sizeof(double));
}

void InitializeUpdateTmpBodyEqtide(BODY *body, CONTROL *control, UPDATE *update,
                                   int iBody) {
  int iPert;

  control->Evolve.tmpBody[iBody].dTidalChi =
        malloc(control->Evolve.iNumBodies * sizeof(double));
  control->Evolve.tmpBody[iBody].dTidalZ =
        malloc(control->Evolve.iNumBodies * sizeof(double));

  control->Evolve.tmpBody[iBody].iaTidePerts =
        malloc(body[iBody].iTidePerts * sizeof(int));
  control->Evolve.tmpBody[iBody].daDoblDtEqtide =
        malloc(control->Evolve.iNumBodies * sizeof(double));

  if (control->Evolve.iEqtideModel == CPL) {
    control->Evolve.tmpBody[iBody].iTidalEpsilon =
          malloc(control->Evolve.iNumBodies * sizeof(int *));
    for (iPert = 0; iPert < control->Evolve.iNumBodies; iPert++) {
      control->Evolve.tmpBody[iBody].iTidalEpsilon[iPert] =
            malloc(10 * sizeof(int));
    }
  }

  if (control->Evolve.iEqtideModel == CTL) {
    control->Evolve.tmpBody[iBody].dTidalF =
          malloc(control->Evolve.iNumBodies * sizeof(double *));
    control->Evolve.tmpBody[iBody].dTidalBeta =
          malloc(control->Evolve.iNumBodies * sizeof(double));
    for (iPert = 0; iPert < control->Evolve.iNumBodies; iPert++) {
      control->Evolve.tmpBody[iBody].dTidalF[iPert] =
            malloc(5 * sizeof(double));
    }
  }
}

/**************** EQTIDE options ********************/

/* Tidal Q of Rocky Body */

void ReadTidalQMantle(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dTidalQMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dTidalQMan = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dTidalQMan = options->dDefault;
  }
}

/* */

/* Discrete Rotation */

void ReadDiscreteRot(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    control->Evolve.bDiscreteRot = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    /* Set to default */
    AssignDefaultInt(options, &control->Evolve.bDiscreteRot, files->iNumInputs);
  }
}

/* Double Synchronous */

void ReadHaltDblSync(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    control->Halt[iFile - 1].bDblSync = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &control->Halt[iFile - 1].bDblSync,
                       files->iNumInputs);
    }
  }
}

/* Fix Orbits -- for testing purposes */

void ReadFixOrbit(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* Cannot exist in primary input file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn,
                    options->iLine[iFile], control->Io.iVerbose);
    control->Evolve.bFixOrbit[iFile - 1] = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    /* Set to default */
    //       AssignDefaultInt(options,&control->Evolve.bFixOrbit[iFile-1],files->iNumInputs);
    control->Evolve.bFixOrbit[iFile - 1] = 0;
  }
}

/* Force Equilibrium Spin Rate? */

void ReadForceEqSpin(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* Cannot exist in primary input file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn,
                    options->iLine[iFile], control->Io.iVerbose);
    control->Evolve.bForceEqSpin[iFile - 1] = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    /* Set to default */
    AssignDefaultInt(options, &control->Evolve.bForceEqSpin[iFile - 1],
                     files->iNumInputs);
  }
}

/* Primary Tide-locked */

void ReadHaltTideLock(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    control->Halt[iFile - 1].bTideLock = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    control->Halt[iFile - 1].bTideLock = atoi(options->cDefault);
  }
}

/* Halt when Synchronous Rotation */

void ReadHaltSyncRot(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bSync = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      control->Halt[iFile - 1].bSync = atoi(options->cDefault);
    }
  }
}

/* k_2 - Love Number of degree 2 */
void ReadK2(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
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
        fprintf(stderr, "ERROR: %s must be greater than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dK2 = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dK2 = options->dDefault;
  }
}

void ReadTidalRadius(BODY *body, CONTROL *control, FILES *files,
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
      body[iFile - 1].dTidalRadius =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dTidalRadius =
            dTmp * fdUnitsLength(control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dTidalRadius = options->dDefault;
  }
}

/* Love number of degree 2 for the ocean */
void ReadK2Ocean(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
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
    body[iFile - 1].dK2Ocean = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dK2Ocean = options->dDefault;
  }
}

/* Love number of degree 2 for the gaseous envelope */
void ReadK2Env(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
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
    body[iFile - 1].dK2Env = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dK2Env = options->dDefault;
  }
}

void ReadK2Mantle(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
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
    body[iFile - 1].dK2Man = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dK2Man = options->dDefault;
  }
}

/* Maximum allowable offset between primary's spin period and its
   equilibrium spin period. */

void ReadMaxLockDiff(BODY *body, CONTROL *control, FILES *files,
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
        fprintf(stderr, "ERROR: %s must be > 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    control->Evolve.dMaxLockDiff[iFile - 1] = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      control->Evolve.dMaxLockDiff[iFile - 1] = options->dDefault;
    }
  }
}

/* Maximum Eccentricity for Synchronous Rotation */

void ReadSyncEcc(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
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
    control->Evolve.dSyncEcc[iFile - 1] = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      control->Evolve.dSyncEcc[iFile - 1] = options->dDefault;
    }
  }
}

/* Tidal Q of the ocean */
void ReadTidalQOcean(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
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

    body[iFile - 1].dTidalQOcean = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dTidalQOcean = options->dDefault;
    }
  }
}

/* Tidal Q of the envelope */
void ReadTidalQEnv(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
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

    body[iFile - 1].dTidalQEnv = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dTidalQEnv = options->dDefault;
    }
  }
}

/* Time lag */

void ReadTidalTau(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dTidalTau =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      /* Convert timestep to cgs */
      body[iFile - 1].dTidalTau =
            dTmp * fdUnitsTime(control->Units[iFile].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dTidalTau = options->dDefault;
    }
  }
}


/* Tidal Model */

void ReadTideModel(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  char cTmp[OPTLEN];

  /* Tide Model, use #defined variables */

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* This parameter can not appear in the primary input,
       as it is module specific (it's easier to code this
       way. It should also only appear in one body file
       so as different tidal models cannot be called. */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "p2", 2)) {
      control->Evolve.iEqtideModel = CPL;
    } else if (!memcmp(sLower(cTmp), "t8", 2)) {
      control->Evolve.iEqtideModel = CTL;
    } else if (!memcmp(sLower(cTmp), "db15", 4)) {
      control->Evolve.iEqtideModel = DB15;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are p2, t8 or "
                "DB15.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  }
}

void ReadTidePerts(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  int iBody, iNumIndices = 0, iNumLines = 0;
  int *lTmp;
  char saTmp[MAXARRAY][OPTLEN];

  lTmp = malloc(MAXLINES * sizeof(int));

  AddOptionStringArray(files->Infile[iFile].cIn, options->cName, saTmp,
                       &iNumIndices, &iNumLines, lTmp, control->Io.iVerbose);
  if (lTmp[0] >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp[0],
                    control->Io.iVerbose);
    /* Now do some initializing */
    body[iFile - 1].iTidePerts = iNumIndices;
    for (iBody = 0; iBody < iNumIndices; iBody++) {
      memset(body[iFile - 1].saTidePerts[iBody], '\0', NAMELEN);
      strcpy(body[iFile - 1].saTidePerts[iBody], saTmp[iBody]);
    }
    UpdateFoundOptionMulti(&files->Infile[iFile], options, lTmp, iNumLines,
                           iFile);
  } else {
    if (iFile > 0 && control->Io.iVerbose >= VERBINPUT) {
      fprintf(stderr, "ERROR: Eqtide called for body %s, but %s not set.\n",
              body[iFile - 1].cName, options->cName);
      LineExit(files->Infile[iFile].cIn, lTmp[0]);
    }
  }
  free(lTmp);
}

// Include effects of ocean tides?
void ReadEqtideOceanTides(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bOcean = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].bOcean = 0; // Default to no ocean tides
  }
}

// Include effects of mantle tides?
void ReadEqtideMantleTides(BODY *body, CONTROL *control, FILES *files,
                           OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bMantle = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].bMantle = atoi(options->cDefault);
  }
}

// Use fixed tidal radius?
void ReadUseTidalRadius(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bUseTidalRadius = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].bUseTidalRadius = atoi(options->cDefault);
  }
}

// Include effects of envelope tides?
void ReadEqtideEnvTides(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bEnv = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].bEnv = 0; // Default to no envelope tides
  }
}

void InitializeOptionsEqtide(OPTIONS *options, fnReadOption fnRead[]) {

  sprintf(options[OPT_DISCRETEROT].cName, "bDiscreteRot");
  sprintf(options[OPT_DISCRETEROT].cDescr,
          "Use Discrete Rotation Model (Phase lag only)");
  sprintf(options[OPT_DISCRETEROT].cDefault, "1");
  options[OPT_DISCRETEROT].iType = 0;
  fnRead[OPT_DISCRETEROT]        = &ReadDiscreteRot;
  sprintf(
        options[OPT_DISCRETEROT].cLongDescr,
        "In the CPL model of EqTide, this option toggles between two ways in \n"
        "which the rotation rate behaves once it has damped to equilibrium. \n"
        "The rigorous model only has two states: 1:1 and 3:2 frequency ratios, "
        "\n"
        "but this is due to truncation in the solution; the equilibrium value "
        "\n"
        "is a continuous function of eccentricity. If this option is set to 1, "
        "\n"
        "then only the 1:1 and 3:2 states are availeble, with a boundary at \n"
        "e = sqrt(1/19); if set to 0, the rotation rate is (1+9.5e^2) times "
        "the \n"
        "mean motion.");

  sprintf(options[OPT_FIXORBIT].cName, "bFixOrbit");
  sprintf(options[OPT_FIXORBIT].cDescr, "Fix Orbital Elements?");
  sprintf(options[OPT_FIXORBIT].cDefault, "0");
  options[OPT_FIXORBIT].iType      = 0;
  options[OPT_FIXORBIT].bMultiFile = 1;
  fnRead[OPT_FIXORBIT]             = &ReadFixOrbit;
  sprintf(options[OPT_FIXORBIT].cLongDescr,
          "In EqTide, setting this value to 1 holds the eccentricity and "
          "semi-major \n"
          "axis fixed during the evolution. This option can be helpful for "
          "testing \n"
          "or for faking planet-planet perturbations that maintain an "
          "eccentricity. \n"
          "(Although a bFixEcc option would probably be better!)\n");

  sprintf(options[OPT_MAXLOCKDIFF].cName, "dMaxLockDiff");
  sprintf(options[OPT_FORCEEQSPIN].cName, "bForceEqSpin"); // for LongDescr
  sprintf(options[OPT_MAXLOCKDIFF].cDescr,
          "Maximum relative difference between spin and equilibrium spin rates "
          "to force equilibrium rate");
  sprintf(options[OPT_MAXLOCKDIFF].cDefault, "0");
  sprintf(options[OPT_MAXLOCKDIFF].cDimension, "nd");
  options[OPT_MAXLOCKDIFF].dDefault   = 0;
  options[OPT_MAXLOCKDIFF].iType      = 2;
  options[OPT_MAXLOCKDIFF].bMultiFile = 1;
  fnRead[OPT_MAXLOCKDIFF]             = &ReadMaxLockDiff;
  sprintf(options[OPT_MAXLOCKDIFF].cLongDescr,
          "In EqTide, this is the maximum relative difference between the "
          "actual \n"
          "spin rate and the equilibrium spin rate without tidal locking. See "
          "%s \n"
          "for more information. %s must be set to 1 to enable this option.",
          options[OPT_FORCEEQSPIN].cName, options[OPT_FORCEEQSPIN].cName);

  // cName defined above
  sprintf(options[OPT_FORCEEQSPIN].cDescr, "Force Spin Rate to Equilibrium?");
  sprintf(options[OPT_FORCEEQSPIN].cDefault, "0");
  options[OPT_FORCEEQSPIN].iType      = 0;
  options[OPT_FORCEEQSPIN].bMultiFile = 1;
  fnRead[OPT_FORCEEQSPIN]             = &ReadForceEqSpin;
  sprintf(
        options[OPT_FORCEEQSPIN].cLongDescr,
        "Set this option to 1 to force the rotation rate to the equilibrium \n"
        "tidal value in EqTide. If set to 0, the rotation rate may \"bounce\" "
        "\n"
        "back and forth across the equilibrium value due to numerical issues. "
        "\n"
        "Setting to 0 may result in a simulation that requires orders of \n"
        "magnitude more computational time. The threshold to force the "
        "equilibrium \n"
        "value is set by %s, but the rotation rate will only be fixed if the \n"
        "derivative on the other side of the equilibrium value send the spin "
        "rate \n"
        "toward equilibrium. This latter functionality enables pre-MS stars to "
        "\n"
        "not tidally lock if the torque due to contraction exceeds the tidal \n"
        "torque.",
        options[OPT_MAXLOCKDIFF].cName);

  sprintf(options[OPT_HALTDBLSYNC].cName, "bHaltDblSync");
  sprintf(options[OPT_HALTDBLSYNC].cDescr, "Halt at Double Synchronous State?");
  sprintf(options[OPT_HALTDBLSYNC].cDefault, "0");
  options[OPT_HALTDBLSYNC].iType = 0;
  fnRead[OPT_HALTDBLSYNC]        = &ReadHaltDblSync;

  sprintf(options[OPT_HALTTIDELOCK].cName, "bHaltTideLock");
  sprintf(options[OPT_HALTTIDELOCK].cDescr, "Halt if Tide-Locked?");
  sprintf(options[OPT_HALTTIDELOCK].cDefault, "0");
  options[OPT_HALTTIDELOCK].iType      = 0;
  options[OPT_HALTTIDELOCK].bMultiFile = 1;
  fnRead[OPT_HALTTIDELOCK]             = &ReadHaltTideLock;

  sprintf(options[OPT_TIDALRADIUS].cName, "dTidalRadius");
  sprintf(options[OPT_TIDALRADIUS].cDescr, "Eqtide Tidal Radius");
  sprintf(options[OPT_TIDALRADIUS].cDefault, "1 Earth Radius");
  sprintf(options[OPT_TIDALRADIUS].cDimension, "length");
  options[OPT_TIDALRADIUS].dDefault   = REARTH;
  options[OPT_TIDALRADIUS].iType      = 2;
  options[OPT_TIDALRADIUS].bMultiFile = 1;
  options[OPT_TIDALRADIUS].dNeg       = REARTH;
  sprintf(options[OPT_TIDALRADIUS].cNeg, "Rearth");
  fnRead[OPT_TIDALRADIUS] = &ReadTidalRadius;

  sprintf(options[OPT_HALTSYNCROT].cName, "bHaltSyncRot");
  sprintf(options[OPT_HALTSYNCROT].cDescr,
          "Halt if the rotation becomes syncrhonous?");
  sprintf(options[OPT_HALTSYNCROT].cDefault, "0");
  options[OPT_HALTSYNCROT].iType      = 0;
  options[OPT_HALTSYNCROT].bMultiFile = 1;
  fnRead[OPT_HALTSYNCROT]             = &ReadHaltSyncRot;

  sprintf(options[OPT_K2].cName, "dK2");
  sprintf(options[OPT_K2].cDescr, "Love Number of Degree 2");
  sprintf(options[OPT_K2].cDefault, "1");
  sprintf(options[OPT_K2].cDimension, "nd");
  options[OPT_K2].dDefault   = 1;
  options[OPT_K2].iType      = 2;
  options[OPT_K2].bMultiFile = 1;
  fnRead[OPT_K2]             = &ReadK2;

  sprintf(options[OPT_K2OCEAN].cName, "dK2Ocean");
  sprintf(options[OPT_K2OCEAN].cDescr, "Ocean's Love Number of Degree 2");
  sprintf(options[OPT_K2OCEAN].cDefault, "0.05");
  sprintf(options[OPT_K2OCEAN].cDimension, "nd");
  options[OPT_K2OCEAN].dDefault   = 0.01;
  options[OPT_K2OCEAN].iType      = 2;
  options[OPT_K2OCEAN].bMultiFile = 1;
  fnRead[OPT_K2OCEAN]             = &ReadK2Ocean;

  sprintf(options[OPT_K2ENV].cName, "dK2Env");
  sprintf(options[OPT_K2ENV].cDescr, "Envelope's Love Number of Degree 2");
  sprintf(options[OPT_K2ENV].cDefault, "0.01");
  sprintf(options[OPT_K2ENV].cDimension, "nd");
  options[OPT_K2ENV].dDefault   = 0.01;
  options[OPT_K2ENV].iType      = 2;
  options[OPT_K2ENV].bMultiFile = 1;
  fnRead[OPT_K2ENV]             = &ReadK2Env;

  sprintf(options[OPT_MAXLOCKDIFF].cName, "dMaxLockDiff");
  sprintf(options[OPT_MAXLOCKDIFF].cDescr,
          "Maximum relative difference between spin and equilibrium spin rates "
          "to force equilibrium spin rate");
  sprintf(options[OPT_MAXLOCKDIFF].cDefault, "0");
  sprintf(options[OPT_MAXLOCKDIFF].cDimension, "nd");
  options[OPT_MAXLOCKDIFF].dDefault   = 0;
  options[OPT_MAXLOCKDIFF].iType      = 2;
  options[OPT_MAXLOCKDIFF].bMultiFile = 1;
  fnRead[OPT_MAXLOCKDIFF]             = &ReadMaxLockDiff;

  sprintf(options[OPT_OCEANTIDES].cName, "bOceanTides");
  sprintf(options[OPT_OCEANTIDES].cDescr, "Include effects of ocean tides?");
  sprintf(options[OPT_OCEANTIDES].cDefault, "0");
  options[OPT_OCEANTIDES].iType      = 0;
  options[OPT_OCEANTIDES].bMultiFile = 1;
  fnRead[OPT_OCEANTIDES]             = &ReadEqtideOceanTides;

  sprintf(options[OPT_MANTLETIDES].cName, "bMantleTides");
  sprintf(options[OPT_MANTLETIDES].cDescr, "Include effects of mantle tides?");
  sprintf(options[OPT_MANTLETIDES].cDefault, "0");
  options[OPT_MANTLETIDES].iType      = 0;
  options[OPT_MANTLETIDES].bMultiFile = 1;
  fnRead[OPT_MANTLETIDES]             = &ReadEqtideMantleTides;

  // XXX What does this do?
  sprintf(options[OPT_USETIDALRADIUS].cName, "bUseTidalRadius");
  sprintf(options[OPT_USETIDALRADIUS].cDescr,
          "Fix radius used for CPL tidal equations?");
  sprintf(options[OPT_USETIDALRADIUS].cDefault, "0");
  options[OPT_USETIDALRADIUS].iType      = 0;
  options[OPT_USETIDALRADIUS].bMultiFile = 1;
  fnRead[OPT_USETIDALRADIUS]             = &ReadUseTidalRadius;

  sprintf(options[OPT_ENVTIDES].cName, "bEnvTides");
  sprintf(options[OPT_ENVTIDES].cDescr,
          "Include effects of gaseous envelope tides?");
  sprintf(options[OPT_ENVTIDES].cDefault, "0");
  options[OPT_ENVTIDES].iType      = 0;
  options[OPT_ENVTIDES].bMultiFile = 1;
  fnRead[OPT_ENVTIDES]             = &ReadEqtideEnvTides;

  sprintf(options[OPT_SYNCECC].cName, "dSyncEcc");
  sprintf(options[OPT_SYNCECC].cDescr,
          "Minimum Eccentricity for Non-Synchronous Rotation");
  sprintf(options[OPT_SYNCECC].cDefault, "0");
  sprintf(options[OPT_SYNCECC].cDimension, "nd");
  options[OPT_SYNCECC].dDefault   = 0;
  options[OPT_SYNCECC].iType      = 2;
  options[OPT_SYNCECC].bMultiFile = 1;
  fnRead[OPT_SYNCECC]             = &ReadSyncEcc;

  sprintf(options[OPT_TIDALQ].cName, "dTidalQ");
  sprintf(options[OPT_TIDALQ].cDescr, "Tidal Quality Factor");
  sprintf(options[OPT_TIDALQ].cDefault, "1e6");
  sprintf(options[OPT_TIDALQ].cDimension, "nd");
  options[OPT_TIDALQ].dDefault   = 1e6;
  options[OPT_TIDALQ].iType      = 2;
  options[OPT_TIDALQ].bMultiFile = 1;
  fnRead[OPT_TIDALQ]             = &ReadTidalQ;

  sprintf(options[OPT_TIDALQOCEAN].cName, "dTidalQOcean");
  sprintf(options[OPT_TIDALQOCEAN].cDescr, "Ocean Tidal Quality Factor");
  sprintf(options[OPT_TIDALQOCEAN].cDefault, "12");
  sprintf(options[OPT_TIDALQOCEAN].cDimension, "nd");
  options[OPT_TIDALQOCEAN].dDefault   = 12;
  options[OPT_TIDALQOCEAN].iType      = 2;
  options[OPT_TIDALQOCEAN].bMultiFile = 1;
  fnRead[OPT_TIDALQOCEAN]             = &ReadTidalQOcean;

  sprintf(options[OPT_TIDALQENV].cName, "dTidalQEnv");
  sprintf(options[OPT_TIDALQENV].cDescr, "Envelope Tidal Quality Factor");
  sprintf(options[OPT_TIDALQENV].cDefault, "1.0e4");
  sprintf(options[OPT_TIDALQENV].cDimension, "nd");
  options[OPT_TIDALQENV].dDefault   = 1.0e4;
  options[OPT_TIDALQENV].iType      = 2;
  options[OPT_TIDALQENV].bMultiFile = 1;
  fnRead[OPT_TIDALQENV]             = &ReadTidalQEnv;

  sprintf(options[OPT_TIDALTAU].cName, "dTidalTau");
  sprintf(options[OPT_TIDALTAU].cDescr, "Tidal Time Lag");
  sprintf(options[OPT_TIDALTAU].cDefault, "1 Second");
  sprintf(options[OPT_TIDALTAU].cDimension, "time");
  options[OPT_TIDALTAU].dDefault   = 1;
  options[OPT_TIDALTAU].iType      = 2;
  options[OPT_TIDALTAU].bMultiFile = 1;
  options[OPT_TIDALTAU].dNeg       = 1;
  sprintf(options[OPT_TIDALTAU].cNeg, "Seconds");
  fnRead[OPT_TIDALTAU] = &ReadTidalTau;

  sprintf(options[OPT_TIDEMODEL].cName, "sTideModel");
  sprintf(options[OPT_TIDEMODEL].cDescr,
          "Tidal Model: p2 [constant-phase-lag, 2nd order] t8 "
          "[constant-time-lag, 8th order]");
  sprintf(options[OPT_TIDEMODEL].cDefault, "p2");
  options[OPT_TIDEMODEL].iType = 3;
  fnRead[OPT_TIDEMODEL]        = &ReadTideModel;

  sprintf(options[OPT_TIDEPERTS].cName, "saTidePerts");
  sprintf(options[OPT_TIDEPERTS].cDescr,
          "Names of bodies engaged in tidal evolution");
  sprintf(options[OPT_TIDEPERTS].cDefault, "none");
  options[OPT_TIDEPERTS].iType = 13;
  fnRead[OPT_TIDEPERTS]        = &ReadTidePerts;

  sprintf(options[OPT_OCEANTIDES].cName, "bOceanTides");
  sprintf(options[OPT_OCEANTIDES].cDescr,
          "Include tidal dissapation due to oceans?");
  sprintf(options[OPT_OCEANTIDES].cDefault, "0");
  options[OPT_OCEANTIDES].iType      = 0;
  options[OPT_OCEANTIDES].bMultiFile = 1;
  fnRead[OPT_OCEANTIDES]             = &ReadEqtideOceanTides;

  sprintf(options[OPT_TIDALQMANTLE].cName, "dTidalQMantle");
  sprintf(options[OPT_TIDALQMANTLE].cDescr, "Tidal Q of Mantle");
  sprintf(options[OPT_TIDALQMANTLE].cDefault, "100");
  sprintf(options[OPT_TIDALQMANTLE].cDimension, "nd");
  options[OPT_TIDALQMANTLE].dDefault   = 100;
  options[OPT_TIDALQMANTLE].iType      = 2;
  options[OPT_TIDALQMANTLE].bMultiFile = 1;
  fnRead[OPT_TIDALQMANTLE]             = &ReadTidalQMantle;

  sprintf(options[OPT_K2MANTLE].cName, "dK2Mantle");
  sprintf(options[OPT_K2MANTLE].cDescr, "Mantle's Love Number of Degree 2");
  sprintf(options[OPT_K2MANTLE].cDefault, "0.01");
  sprintf(options[OPT_K2MANTLE].cDimension, "nd");
  options[OPT_K2MANTLE].dDefault   = 0.01;
  options[OPT_K2MANTLE].iType      = 2;
  options[OPT_K2MANTLE].bMultiFile = 1;
  fnRead[OPT_K2MANTLE]             = &ReadK2Mantle;
}

void ReadOptionsEqtide(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, fnReadOption fnRead[],
                       int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTEQTIDE; iOpt < OPTENDEQTIDE; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}


/******************* Verify EQTIDE ******************/

void VerifyRotationEqtideWarning(char cName1[], char cName2[], char cFile[],
                                 int iLine1, int iLine2, int iVerbose) {
  if (iVerbose >= VERBINPUT) {
    fprintf(stderr,
            "INFO: %s and %s are both set. Rotation rate will be in "
            "equilibrium.\n",
            cName1, cName2);
    fprintf(stderr, "\tFile: %s, Lines %d and %d\n", cFile, iLine1, iLine2);
  }
}

void VerifyLostEngEqtide(BODY *body, UPDATE *update, CONTROL *control,
                         OPTIONS *options, int iBody) {
  update[iBody].iaType[update[iBody].iLostEng][update[iBody].iLostEngEqtide] =
        5;
  update[iBody]
        .iNumBodies[update[iBody].iLostEng][update[iBody].iLostEngEqtide] = 1;
  update[iBody].iaBody[update[iBody].iLostEng][update[iBody].iLostEngEqtide] =
        malloc(update[iBody].iNumBodies[update[iBody].iLostEng]
                                       [update[iBody].iLostEngEqtide] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iLostEng][update[iBody].iLostEngEqtide][0] =
        iBody;

  update[iBody].pdLostEngEqtide =
        &update[iBody].daDerivProc[update[iBody].iLostEng]
                                  [update[iBody].iLostEngEqtide];
}

void VerifyRotationEqtide(BODY *body, CONTROL *control, UPDATE *update,
                          OPTIONS *options, char cFile[], int iBody) {
  double dMeanMotion;
  int iOrbiter;

  // Default value
  body[iBody].bTideLock = 0;
  body[iBody].dLockTime = -1;

  if (options[OPT_FORCEEQSPIN].iLine[iBody + 1] >= 0) {

    if (iBody > 0) {
      // If ForceEqSpin, tidally locked to begin the simulation
      body[iBody].bTideLock = 1;
      body[iBody].dLockTime = 0.0;
    }

    if (body[iBody].iTidePerts > 1) {
      fprintf(stderr,
              "ERROR: %s cannot be true is %s has more than 1 argument.\n",
              options[OPT_FORCEEQSPIN].cName, options[OPT_TIDEPERTS].cName);
      DoubleLineExit(options[OPT_FORCEEQSPIN].cFile[iBody + 1],
                     options[OPT_TIDEPERTS].cFile[iBody + 1],
                     options[OPT_FORCEEQSPIN].iLine[iBody + 1],
                     options[OPT_TIDEPERTS].iLine[iBody + 1]);
    }

    if (options[OPT_ROTPER].iLine[iBody + 1] >= 0) {
      VerifyRotationEqtideWarning(
            options[OPT_FORCEEQSPIN].cName, options[OPT_ROTPER].cName, cFile,
            options[OPT_FORCEEQSPIN].iLine[iBody + 1],
            options[OPT_ROTPER].iLine[iBody + 1], control->Io.iVerbose);
    }

    if (options[OPT_ROTRATE].iLine[iBody + 1] >= 0) {
      VerifyRotationEqtideWarning(
            options[OPT_FORCEEQSPIN].cName, options[OPT_ROTRATE].cName, cFile,
            options[OPT_FORCEEQSPIN].iLine[iBody + 1],
            options[OPT_ROTRATE].iLine[iBody + 1], control->Io.iVerbose);
    }

    if (options[OPT_ROTVEL].iLine[iBody + 1] >= 0) {
      VerifyRotationEqtideWarning(
            options[OPT_FORCEEQSPIN].cName, options[OPT_ROTVEL].cName, cFile,
            options[OPT_FORCEEQSPIN].iLine[iBody + 1],
            options[OPT_ROTVEL].iLine[iBody + 1], control->Io.iVerbose);
    }

    /* Done with warnings, do the assignment */
    if (bPrimary(body, iBody)) {
      iOrbiter = body[iBody].iaTidePerts[0];
    } else {
      iOrbiter = iBody;
    }

    dMeanMotion = fdSemiToMeanMotion(
          body[iOrbiter].dSemi,
          body[iBody].dMass + body[body[iBody].iaTidePerts[0]].dMass);
    body[iBody].dRotRate = fdEqRotRate(
          body, iBody, dMeanMotion, body[iOrbiter].dEccSq,
          control->Evolve.iEqtideModel, control->Evolve.bDiscreteRot);
  } else { // Not tidally locked to begin with
    body[iBody].bTideLock = 0;
  }
  CalcXYZobl(body, iBody);
}

/*
 *
 * Verify Tidal Model
 *
 * Includes initialization of the fnUpdate matrix.
 *
 */

/* Get file where sTideModel set */
int fiTideFile(int *iLine, int iNumFiles) {
  int iFile;

  for (iFile = 0; iFile < iNumFiles; iFile++) {
    if (iLine[iFile] != -1) {
      return iFile;
    }
  }

  assert(0);
  return 0; // To avoid compiler warnings
}

/* In the following, iBody is the current body number that is getting assigned,
   iPert counts the number of bodies perturbing iBody, and iBodyPert is the
   body number of the current perturbing body. */

void InitializeXoblEqtide(BODY *body, UPDATE *update, int iBody, int iPert) {
  update[iBody].iaType[update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]] =
        2;
  update[iBody].padDXoblDtEqtide[iPert] =
        &update[iBody].daDerivProc[update[iBody].iXobl]
                                  [update[iBody].iaXoblEqtide[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]] = 2;
  update[iBody].iaBody[update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iXobl]
                                       [update[iBody].iaXoblEqtide[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]][1] =
        body[iBody].iaTidePerts[iPert];
}

void InitializeYoblEqtide(BODY *body, UPDATE *update, int iBody, int iPert) {
  update[iBody].iaType[update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]] =
        2;
  update[iBody].padDYoblDtEqtide[iPert] =
        &update[iBody].daDerivProc[update[iBody].iYobl]
                                  [update[iBody].iaYoblEqtide[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]] = 2;
  update[iBody].iaBody[update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iYobl]
                                       [update[iBody].iaYoblEqtide[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]][1] =
        body[iBody].iaTidePerts[iPert];
}

void InitializeZoblEqtide(BODY *body, UPDATE *update, int iBody, int iPert) {
  update[iBody].iaType[update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]] =
        2;
  update[iBody].padDZoblDtEqtide[iPert] =
        &update[iBody].daDerivProc[update[iBody].iZobl]
                                  [update[iBody].iaZoblEqtide[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]] = 2;
  update[iBody].iaBody[update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iZobl]
                                       [update[iBody].iaZoblEqtide[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]][1] =
        body[iBody].iaTidePerts[iPert];
}

void InitializeRotEqtide(BODY *body, UPDATE *update, int iBody, int iPert) {
  update[iBody].iaType[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] =
        1;
  update[iBody].padDrotDtEqtide[iPert] =
        &update[iBody].daDerivProc[update[iBody].iRot]
                                  [update[iBody].iaRotEqtide[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] = 2;
  update[iBody].iaBody[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iRot]
                                       [update[iBody].iaRotEqtide[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]][0] =
        iBody;

  update[iBody]
        .iaBody[update[iBody].iRot][update[iBody].iaRotEqtide[iPert]][1] =
        body[iBody].iaTidePerts[iPert];
}

void InitializeHeccEqtide(BODY *body, UPDATE *update, int iBody) {
  update[iBody].iaType[update[iBody].iHecc][update[iBody].iHeccEqtide] = 2;
  update[iBody].pdDHeccDtEqtide =
        &update[iBody]
               .daDerivProc[update[iBody].iHecc][update[iBody].iHeccEqtide];
  update[iBody].iNumBodies[update[iBody].iHecc][update[iBody].iHeccEqtide] = 2;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iHeccEqtide] = malloc(
        update[iBody]
              .iNumBodies[update[iBody].iHecc][update[iBody].iHeccEqtide] *
        sizeof(int));
  /* Ecc is stored in iBody, and the perturbing body is the central object,
     which is body 0 */
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iHeccEqtide][0] =
        iBody;
  update[iBody].iaBody[update[iBody].iHecc][update[iBody].iHeccEqtide][1] = 0;
}

void InitializeKeccEqtide(BODY *body, UPDATE *update, int iBody) {
  update[iBody].iaType[update[iBody].iKecc][update[iBody].iKeccEqtide] = 2;
  update[iBody].pdDKeccDtEqtide =
        &update[iBody]
               .daDerivProc[update[iBody].iKecc][update[iBody].iKeccEqtide];
  update[iBody].iNumBodies[update[iBody].iKecc][update[iBody].iKeccEqtide] = 2;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iKeccEqtide] = malloc(
        update[iBody]
              .iNumBodies[update[iBody].iKecc][update[iBody].iKeccEqtide] *
        sizeof(int));
  /* Ecc is stored in iBody, and the perturbing body is the central object,
     which is body 0 */
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iKeccEqtide][0] =
        iBody;
  update[iBody].iaBody[update[iBody].iKecc][update[iBody].iKeccEqtide][1] = 0;
}

void InitializeSemiEqtide(BODY *body, UPDATE *update, int iBody) {
  update[iBody].iaType[update[iBody].iSemi][update[iBody].iSemiEqtide] = 1;
  update[iBody].pdDsemiDtEqtide =
        &update[iBody]
               .daDerivProc[update[iBody].iSemi][update[iBody].iSemiEqtide];
  update[iBody].iNumBodies[update[iBody].iSemi][update[iBody].iSemiEqtide] = 2;
  update[iBody].iaBody[update[iBody].iSemi][update[iBody].iSemiEqtide] = malloc(
        update[iBody]
              .iNumBodies[update[iBody].iSemi][update[iBody].iSemiEqtide] *
        sizeof(int));
  /* Semi is stored in iBody, and the perturbing body is the central object,
     which is body 0 */
  update[iBody].iaBody[update[iBody].iSemi][update[iBody].iSemiEqtide][0] =
        iBody;
  update[iBody].iaBody[update[iBody].iSemi][update[iBody].iSemiEqtide][1] = 0;
}

void VerifyCTL(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               OUTPUT *output, UPDATE *update, int iBody, int iModule) {
  int iPert, iTideFile, iCol, iFile;

  /* CTL model, but Q's set? */
  if (options[OPT_TIDALQ].iLine[iBody + 1] >= 0) {
    /* Tidal Q was set */
    if (options[OPT_TIDALTAU].iLine[iBody + 1] >= 0) {
      /* Tidal Tau was also set */
      if (control->Io.iVerbose >= VERBINPUT) {
        // System units (SI) by here
        fprintf(stderr,
                "INFO: Time lag model selected, %s and %s set in file %s. "
                "Using %s = %lf seconds and ignoring %s.\n",
                options[OPT_TIDALTAU].cName, options[OPT_TIDALQ].cName,
                options[OPT_TIDALTAU].cFile[iBody + 1],
                options[OPT_TIDALTAU].cName, body[iBody].dTidalTau,
                options[OPT_TIDALQ].cName);
      }
    } else {
      /* Tidal Tau was not set */
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Time lag model selected, but only %s was set in file "
                "%s.\n",
                options[OPT_TIDALQ].cName, files->Infile[iBody + 1].cIn);
      }
      exit(EXIT_INPUT);
    }
  }

  /* Can't set bDiscreteRot in the CTL model */
  if (options[OPT_DISCRETEROT].iLine[iBody + 1] >= 0) {
    VerifyBodyExit(options[OPT_TIDALTAU].cName, options[OPT_DISCRETEROT].cName,
                   options[OPT_DISCRETEROT].cFile[iBody],
                   options[OPT_TIDALTAU].iLine[iBody],
                   options[OPT_DISCRETEROT].iLine[iBody + 1],
                   control->Io.iVerbose);
  }

  /* Verify output contains no CPL-specific parameters */

  if (files->Outfile[iBody].iNumCols > 0) {
    for (iCol = 0; iCol < files->Outfile[iBody].iNumCols; iCol++) {

      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_EQROTPERCONT].cName,
                 strlen(output[OUT_EQROTPERCONT].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr,
                  "ERROR: Time lag model selected; output %s is not allowed.\n",
                  output[OUT_EQROTPERCONT].cName);
        }
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine, files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],
                       options[OPT_OUTPUTORDER].cFile[iBody + 1],
                       options[OPT_TIDEMODEL].iLine[iTideFile],
                       options[OPT_OUTPUTORDER].iLine[iBody + 1]);
      }

      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_EQROTPERDISCRETE].cName,
                 strlen(output[OUT_EQROTPERDISCRETE].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr,
                  "ERROR: Time lag model selected; output %s is not allowed.\n",
                  output[OUT_EQROTPERDISCRETE].cName);
        }
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine, files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],
                       options[OPT_OUTPUTORDER].cFile[iBody + 1],
                       options[OPT_TIDEMODEL].iLine[iTideFile],
                       options[OPT_OUTPUTORDER].iLine[iBody + 1]);
      }

      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_EQROTRATECONT].cName,
                 strlen(output[OUT_EQROTRATECONT].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr,
                  "ERROR: Time lag model selected; output %s is not allowed.\n",
                  output[OUT_EQROTRATECONT].cName);
        }
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine, files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],
                       options[OPT_OUTPUTORDER].cFile[iBody + 1],
                       options[OPT_TIDEMODEL].iLine[iTideFile],
                       options[OPT_OUTPUTORDER].iLine[iBody + 1]);
      }

      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_EQROTRATEDISCRETE].cName,
                 strlen(output[OUT_EQROTRATEDISCRETE].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr,
                  "ERROR: Time lag model selected; output %s is not allowed.\n",
                  output[OUT_EQROTRATEDISCRETE].cName);
        }
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine, files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],
                       options[OPT_OUTPUTORDER].cFile[iBody + 1],
                       options[OPT_TIDEMODEL].iLine[iTideFile],
                       options[OPT_OUTPUTORDER].iLine[iBody + 1]);
      }

      if (memcmp(files->Outfile[iBody].caCol[iCol], output[OUT_GAMMAORB].cName,
                 strlen(output[OUT_GAMMAORB].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr,
                  "ERROR: Time lag model selected; output %s is not allowed.\n",
                  output[OUT_GAMMAORB].cName);
        }
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine, files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],
                       options[OPT_OUTPUTORDER].cFile[iBody + 1],
                       options[OPT_TIDEMODEL].iLine[iTideFile],
                       options[OPT_OUTPUTORDER].iLine[iBody + 1]);
      }

      if (memcmp(files->Outfile[iBody].caCol[iCol], output[OUT_GAMMAROT].cName,
                 strlen(output[OUT_GAMMAROT].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr,
                  "ERROR: Time lag model selected; output %s is not allowed.\n",
                  output[OUT_GAMMAROT].cName);
        }
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine, files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],
                       options[OPT_OUTPUTORDER].cFile[iBody + 1],
                       options[OPT_TIDEMODEL].iLine[iTideFile],
                       options[OPT_OUTPUTORDER].iLine[iBody + 1]);
      }

      if (memcmp(files->Outfile[iBody].caCol[iCol], output[OUT_TIDALQ].cName,
                 strlen(output[OUT_TIDALQ].cName)) == 0) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr,
                  "ERROR: Time lag model selected; output %s is not allowed.\n",
                  output[OUT_TIDALQ].cName);
        }
        /* Must find file that declares tidel model */
        iTideFile = fiTideFile(options[OPT_TIDEMODEL].iLine, files->iNumInputs);
        DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],
                       options[OPT_OUTPUTORDER].cFile[iBody + 1],
                       options[OPT_TIDEMODEL].iLine[iTideFile],
                       options[OPT_OUTPUTORDER].iLine[iBody + 1]);
      }
    }
  }

  /* Everything OK, assign Updates */

  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {

    /* Obliquity */
    // Xobl
    InitializeXoblEqtide(body, update, iBody, iPert);
    // Yobl
    InitializeYoblEqtide(body, update, iBody, iPert);
    // Zobl
    InitializeZoblEqtide(body, update, iBody, iPert);
    /* Rotation Rate */
    InitializeRotEqtide(body, update, iBody, iPert);
  }

  /* Is this the secondary body, and hence we assign da/dt and de/dt? */
  if (!bPrimary(body, iBody)) {
    // Initialize Orbital variable for the matrix
    InitializeSemiEqtide(body, update, iBody);
    InitializeHeccEqtide(body, update, iBody);
    InitializeKeccEqtide(body, update, iBody);
  }

  /* Malloc body memory
     N.B. that space is allocated for all planets, even though only
     iTidePerts are actually modeled. This deicision makes coding
     the derivatives easier. iaBody can point to the element of the
     body struct, and non-perturbing body elements are never touched.
     The alternative is to use a new array, maybe iIndex, that
     connects the element # in the body struct to the element number
     in the TidalZ array. That seems too cumbersome to me, but this
     solution definitely wastes memory.
  */

  body[iBody].dTidalF = malloc(control->Evolve.iNumBodies * sizeof(double *));
  body[iBody].dTidalBeta = malloc(control->Evolve.iNumBodies * sizeof(double));
  for (iPert = 0; iPert < control->Evolve.iNumBodies; iPert++) {
    body[iBody].dTidalF[iPert] = malloc(5 * sizeof(double));
  }

  control->fnPropsAux[iBody][iModule] = &PropsAuxCTL;

  /* Now remove output options unique to CPL. This will prevent segmentation
     faults as memory will not be allocated to some parameters unless CPL
     is chosen. */
  output[OUT_EQROTRATEDISCRETE].iNum = 0;
  output[OUT_EQROTPERDISCRETE].iNum  = 0;
  output[OUT_EQROTRATECONT].iNum     = 0;
  output[OUT_EQROTPERCONT].iNum      = 0;
  output[OUT_GAMMAROT].iNum          = 0;
  output[OUT_GAMMAORB].iNum          = 0;
  output[OUT_TIDALQ].iNum            = 0;
}

void VerifyCPL(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               OUTPUT *output, UPDATE *update, int iBody, int iModule) {
  int iPert, iTideFile, iCol, iFile;

  // Body 0 has no orbit parameters see VerifyOrbitEqtide().
  if (iBody != 0 && body[iBody].dEccSq > (2. / 19) &&
      control->Evolve.bDiscreteRot) {
    if (control->Io.iVerbose >= VERBINPUT) {
      fprintf(stderr,
              "WARNING: Setting %s to 1 is not advised for eccentricities "
              "larger than %.3lf\n",
              options[OPT_DISCRETEROT].cName, pow(2. / 19, 0.5));
    }
  }

  /* CPL model, but tau's set? */
  if (options[OPT_TIDALTAU].iLine[iBody + 1] >= 0) {
    /* Tidal tau was set */
    if (options[OPT_TIDALQ].iLine[iBody + 1] >= 0) {
      /* Tidal Q was also set */
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr,
                "WARNING: Phase lag model selected, but both %s and %s set in file %s. "
                "Using %s = %lf and ignoring %s.\n",
                options[OPT_TIDALTAU].cName, options[OPT_TIDALQ].cName,
                options[OPT_TIDALTAU].cFile[iBody + 1],
                options[OPT_TIDALQ].cName, body[iBody].dTidalQ,
                options[OPT_TIDALTAU].cName);
      }
    } else {
      /* Tidal Tau was not set */
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Phase lag model selected, but only %s was set in file "
                "%s.\n",
                options[OPT_TIDALTAU].cName, files->Infile[iBody + 1].cIn);
      }
      exit(EXIT_INPUT);
    }

    /* Verify output contains no CTL-specific parameters */

    if (files->Outfile[iBody].iNumCols > 0) {
      for (iCol = 0; iCol < files->Outfile[iBody].iNumCols; iCol++) {

        if (memcmp(files->Outfile[iBody].caCol[iCol], output[OUT_TIDALQ].cName,
                   strlen(output[OUT_TIDALQ].cName)) == 0) {
          if (control->Io.iVerbose >= VERBINPUT) {
            fprintf(
                  stderr,
                  "ERROR: Time lag model selected; output %s is not allowed.\n",
                  output[OUT_TIDALQ].cName);
          }
          /* Must find file that declares tidel model */
          iTideFile =
                fiTideFile(options[OPT_TIDEMODEL].iLine, files->iNumInputs);
          DoubleLineExit(options[OPT_TIDEMODEL].cFile[iTideFile],
                         options[OPT_OUTPUTORDER].cFile[iBody + 1],
                         options[OPT_TIDEMODEL].iLine[iTideFile],
                         options[OPT_OUTPUTORDER].iLine[iBody + 1]);
        }
      }
    }
  }

  /* Everything OK, assign Updates */

  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {

    /* Obliquity */
    // Xobl
    InitializeXoblEqtide(body, update, iBody, iPert);
    // Yobl
    InitializeYoblEqtide(body, update, iBody, iPert);
    // Zobl
    InitializeZoblEqtide(body, update, iBody, iPert);
    /* Rotation Rate */
    InitializeRotEqtide(body, update, iBody, iPert);
  }

  /* Is this the secondary body, and hence we assign da/dt and de/dt? */
  if (!bPrimary(body, iBody)) {
    // Initialize Orbital variable for the matrix
    InitializeSemiEqtide(body, update, iBody);
    InitializeHeccEqtide(body, update, iBody);
    InitializeKeccEqtide(body, update, iBody);
  }

  /* Malloc body memory
     N.B. that space is allocated for all planets, even though only
     iTidePerts are actually modeled. This deicision makes coding
     the derivatives easier. iaBody can point to the element of the
     body struct, and non-perturbing body elements are never touched.
     The alternative is to use a new array, maybe iIndex, that
     connects the element # in the body struct to the element number
     in the TidalZ array. That seems too cumbersome to me, but this
     solution definitely wastes memory.
  */

  body[iBody].iTidalEpsilon =
        malloc(control->Evolve.iNumBodies * sizeof(int *));
  for (iPert = 0; iPert < control->Evolve.iNumBodies; iPert++) {
    body[iBody].iTidalEpsilon[iPert] = malloc(10 * sizeof(int));
  }
  control->fnPropsAux[iBody][iModule] = &PropsAuxCPL;

  /* Now remove output options unique to CTL. This will prevent segmentation
     faults as memory will not be allocated to some parameters unless CTL
     is chosen. */
  output[OUT_TIDALTAU].iNum = 0;
}

/** Verify all arguments to saTidePerturbers. This subroutine will called
   from each body using module eqtide, but we must make sure that each pair
   of perturbing bodies points to each other, so we must loop through verify
   all the bodies at the same time. This means all these lines will be
   repeated for each tidally evolving body. But, if it's verified the first
   time, it should verify every time! */


void VerifyPerturbersEqtide(BODY *body, FILES *files, OPTIONS *options,
                            UPDATE *update, int iNumBodies, int iBody) {
  int iPert, iBodyPert, iVar, ok;
  int *bFound = malloc(iNumBodies*sizeof(int));

  for (iBody = 0; iBody < iNumBodies; iBody++) {
    fprintf(stderr,"Body: %s\n",body[iBody].cName);
    if (body[iBody].bEqtide) {
      if (body[iBody].iTidePerts > 0) {
        for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
          bFound[iPert] = 0;
          for (iBodyPert = 0; iBodyPert < iNumBodies; iBodyPert++) {
            if (iBodyPert != iBody) {
              if (strncmp(body[iBody].saTidePerts[iPert], body[iBodyPert].cName,
                          sizeof(body[iBody].saTidePerts[iPert])) == 0) {
                /* This parameter contains the body # of the "iPert-th"
                        tidal perturber */
                body[iBody].iaTidePerts[iPert] = iBodyPert;
                bFound[iPert]                  = 1;

                // Was eqtide set for the perturbed body?
                if (!body[iBodyPert].bEqtide) {
                  fprintf(stderr,
                          "ERROR: %s tidally perturbs %s, but module EqTide "
                          "was not selected for body %s.\n",
                          body[iBody].cName, body[iBodyPert].cName,
                          body[iBodyPert].cName);
                  DoubleLineExit(options[OPT_TIDEPERTS].cFile[iBody + 1],
                                 options[OPT_MODULES].cFile[iBodyPert + 1],
                                 options[OPT_TIDEPERTS].iLine[iBody + 1],
                                 options[OPT_MODULES].iLine[iBodyPert + 1]);
                }
              }
            }
          }
        }
      }

      /* Were all tidal perturbers identified? */
      ok = 1;
      for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
        if (bFound[iPert] == 0) {
          fprintf(stderr, "ERROR: Unknown tidal perturber to body %s: %s\n",
                  body[iBody].cName, body[iBody].saTidePerts[iPert]);
          fprintf(stderr, "\tFile: %s, Line: %d\n",
                  options[OPT_TIDEPERTS].cFile[iBody + 1],
                  options[OPT_TIDEPERTS].iLine[iBody + 1]);
          ok = 0;
        }
      }

      /* Was the same perturber listed multiple times? */

      for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
        for (iBodyPert = iPert + 1; iBodyPert < body[iBody].iTidePerts;
             iBodyPert++) {
          if (body[iBody].iaTidePerts[iPert] ==
              body[iBody].iaTidePerts[iBodyPert]) {
            fprintf(stderr,
                    "ERROR: Body %s listed mulitple times to argument %s.\n",
                    body[iBody].saTidePerts[iPert],
                    options[OPT_TIDEPERTS].cName);
            fprintf(stderr, "\tFile: %s, Line: %d\n",
                    options[OPT_TIDEPERTS].cFile[iBody + 1],
                    options[OPT_TIDEPERTS].iLine[iBody + 1]);
            ok = 0;
          }
        }
      }

      // Was saTidePerts set?
      for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
        if (!(body[body[iBody].iaTidePerts[iPert]].bEqtide)) {
          fprintf(stderr,
                  "ERROR: Eqtide called for body %s, but option %s not set.\n",
                  body[iBody].cName, options[OPT_TIDEPERTS].cName);
          ok = 0;
        }
      }

      if (!ok) {
        exit(EXIT_INPUT);
      }
    }
  }

  free(bFound);

  /* All entries to saTidePerts are known bodies, does each point to the other?
     Exomoon ready! */
  for (iBody = 0; iBody < iNumBodies; iBody++) {
    ok = 0;
    if (body[iBody].bEqtide) {
      for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
        for (iBodyPert = 0;
             iBodyPert < body[body[iBody].iaTidePerts[iPert]].iTidePerts;
             iBodyPert++) {
          if (iBody ==
              body[body[iBody].iaTidePerts[iPert]].iaTidePerts[iBodyPert]) {
            /* Match */
            ok = 1;
          }
        }
        if (!ok) {
          fprintf(stderr,
                  "ERROR: %s tidally perturbs %s, but %s does NOT tidally "
                  "perturb %s\n",
                  body[iBody].cName, body[body[iBody].iaTidePerts[iPert]].cName,
                  body[body[iBody].iaTidePerts[iPert]].cName,
                  body[iBody].cName);
          fprintf(stderr, "\tFile: %s, Line: %d\n",
                  files->Infile[iBody + 1].cIn,
                  options[OPT_TIDEPERTS].iLine[iBody + 1]);
          fprintf(stderr, "\tFile: %s, Line: %d\n",
                  files->Infile[body[iBody].iaTidePerts[iPert] + 1].cIn,
                  options[OPT_TIDEPERTS].iLine[iPert + 1]);
          exit(EXIT_INPUT);
        }
      }
    }
  }
}

void VerifyOrbitEqtide(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options) {
  int iBody;

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    if (iBody == 0) {
      /* Orbital parameters cannot be input for the central body */
      if (options[OPT_ORBECC].iLine[iBody + 1] > -1) {
        fprintf(stderr, "ERROR: %s cannot be set for the central body.\n",
                options[OPT_ORBECC].cName);
        LineExit(files->Infile[iBody + 1].cIn,
                 options[OPT_ORBECC].iLine[iBody + 1]);
      }
      if (options[OPT_ORBSEMI].iLine[iBody + 1] > -1) {
        fprintf(stderr, "ERROR: %s cannot be set for the central body.\n",
                options[OPT_ORBSEMI].cName);
        LineExit(files->Infile[iBody + 1].cIn,
                 options[OPT_ORBSEMI].iLine[iBody + 1]);
      }
    }
    body[iBody].dEccSq = body[iBody].dEcc * body[iBody].dEcc;
    CalcHK(body, iBody);
  }
}

int fiGetModuleIntEqtide(MODULE *module, int iBody) {
  int iModule;

  for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
    if (module->iaModule[iBody][iModule] == EQTIDE) {
      return iModule;
    }
  }

  fprintf(stderr, "ERROR: Eqtide not found for body #%d.\n", iBody);
  exit(1);
}

void VerifyTideModel(CONTROL *control, FILES *files, OPTIONS *options) {
  int iFile, iFound = 0;
  char cTmp[8];

  for (iFile = 0; iFile < files->iNumInputs; iFile++) {
    if (options[OPT_TIDEMODEL].iLine[iFile] >= 0) {
      iFound++;
    }
  }

  if (iFound > 1) {
    if (control->Io.iVerbose > VERBERR) {
      fprintf(stderr, "ERROR: Option %s set multiple times.\n",
              options[OPT_TIDEMODEL].cName);
      for (iFile = 0; iFile < files->iNumInputs; iFile++) {
        if (options[OPT_TIDEMODEL].iLine[iFile] >= 0) {
          fprintf(stderr, "\tFile %s, Line: %d\n", files->Infile[0].cIn,
                  options[OPT_TIDEMODEL].iLine[iFile]);
        }
      }
    }
    exit(EXIT_INPUT);
  }

  if (iFound == 0) {
    strcpy(cTmp, options[OPT_TIDEMODEL].cDefault);
    if (!memcmp(sLower(cTmp), "p2", 2)) {
      control->Evolve.iEqtideModel = CPL;
    } else if (!memcmp(sLower(cTmp), "t8", 2)) {
      control->Evolve.iEqtideModel = CTL;
    }
    if (control->Io.iVerbose >= VERBINPUT) {
      fprintf(stderr, "INFO: %s not set in any file, defaulting to %s.\n",
              options[OPT_TIDEMODEL].cName, options[OPT_TIDEMODEL].cDefault);
    }

    /* Chicanery. Since I only want this set once, I will
       make it seem like the user set it. */
    options[OPT_TIDEMODEL].iLine[0] = 1;
  }
}

void AssignEqtideDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody) {
  int iPert;
  if (evolve->iEqtideModel == CPL) {
    fnUpdate[iBody][update[iBody].iLostEng][update[iBody].iLostEngEqtide] =
          &fdDEdTCPLEqtide;
  } else if (evolve->iEqtideModel == CTL) {
    fnUpdate[iBody][update[iBody].iLostEng][update[iBody].iLostEngEqtide] =
          &fdDEdTCTLEqtide;
  } else if (evolve->iEqtideModel == DB15) {
    // Energy not tracked in DB15
    fnUpdate[iBody][update[iBody].iLostEng][update[iBody].iLostEngEqtide] =
          &fndUpdateFunctionTiny;
  } else {
    fprintf(stderr, "ERROR: Must choose CPL, CTL of DB15 tidal model!\n");
    exit(EXIT_INPUT);
  }

  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {

    // CTL Model Variables

    if (evolve->iEqtideModel == CTL) {

      // Xobl
      fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]] =
            &fdCTLDXoblDt;
      // Yobl
      fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]] =
            &fdCTLDYoblDt;
      // Zobl
      fnUpdate[iBody][update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]] =
            &fdCTLDZoblDt;
      /* Rotation Rate */
      if (evolve->bForceEqSpin[iBody]) {
        fnUpdate[iBody][update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] =
              &fndUpdateFunctionTiny;
      } else {
        fnUpdate[iBody][update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] =
              &fdCTLDrotrateDt;
      }
    }

    // CPL Module Derivatives

    if (evolve->iEqtideModel == CPL) {

      // Xobl
      fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]] =
            &fdCPLDXoblDt;
      // Yobl
      fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]] =
            &fdCPLDYoblDt;
      // Zobl
      fnUpdate[iBody][update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]] =
            &fdCPLDZoblDt;
      // Rotation Rate
      if (evolve->bForceEqSpin[iBody]) {
        fnUpdate[iBody][update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] =
              &fndUpdateFunctionTiny;
      } else {
        fnUpdate[iBody][update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] =
              &fdCPLDrotrateDt;
      }
    }

    // DB15 Derivatives

    if (evolve->iEqtideModel == DB15) {

      // DB15 assumes no obliquity => must be initialized to 0, and not evolve
      fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]] =
            &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]] =
            &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]] =
            &fndUpdateFunctionTiny;

      // DB15 assumes rotation rate is fixed at synchronous
      evolve->bForceEqSpin[iBody] = 1;
      fnUpdate[iBody][update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] =
            &fndUpdateFunctionTiny;
    }
  }

  /* Is this the secondary body, and hence we assign da/dt and de/dt? */
  if (!bPrimary(body, iBody)) {
    // If the orbit is allowed to evolve, assign derivative functions
    if (!evolve->bFixOrbit[iBody]) {
      // CTL Model Derivatives
      if (evolve->iEqtideModel == CTL) {

        fnUpdate[iBody][update[iBody].iSemi][update[iBody].iSemiEqtide] =
              &fdCTLDsemiDt;
        fnUpdate[iBody][update[iBody].iHecc][update[iBody].iHeccEqtide] =
              &fdCTLDHeccDt;
        fnUpdate[iBody][update[iBody].iKecc][update[iBody].iKeccEqtide] =
              &fdCTLDKeccDt;
      }

      // CPL Model Derivatives
      if (evolve->iEqtideModel == CPL) {

        fnUpdate[iBody][update[iBody].iSemi][update[iBody].iSemiEqtide] =
              &fdCPLDsemiDt;
        fnUpdate[iBody][update[iBody].iHecc][update[iBody].iHeccEqtide] =
              &fdCPLDHeccDt;
        fnUpdate[iBody][update[iBody].iKecc][update[iBody].iKeccEqtide] =
              &fdCPLDKeccDt;
      }

      // DB15 Model Derivatives
      if (evolve->iEqtideModel == DB15) {

        fnUpdate[iBody][update[iBody].iSemi][update[iBody].iSemiEqtide] =
              &fdDB15DsemiDt;
        fnUpdate[iBody][update[iBody].iHecc][update[iBody].iHeccEqtide] =
              &fdDB15DHeccDt;
        fnUpdate[iBody][update[iBody].iKecc][update[iBody].iKeccEqtide] =
              &fdDB15DKeccDt;
      }
    } else {
      // If the orbit is fixed, function return dTINY
      fnUpdate[iBody][update[iBody].iSemi][update[iBody].iSemiEqtide] =
            &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iHecc][update[iBody].iHeccEqtide] =
            &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iKecc][update[iBody].iKeccEqtide] =
            &fndUpdateFunctionTiny;
    }
  }
}

void NullEqtideDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                           fnUpdateVariable ***fnUpdate, int iBody) {
  int iPert;

  fnUpdate[iBody][update[iBody].iLostEng][update[iBody].iLostEngEqtide] =
        &fndUpdateFunctionTiny;

  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {

    // Xobl
    fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblEqtide[iPert]] =
          &fndUpdateFunctionTiny;
    // Yobl
    fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblEqtide[iPert]] =
          &fndUpdateFunctionTiny;
    // Zobl
    fnUpdate[iBody][update[iBody].iZobl][update[iBody].iaZoblEqtide[iPert]] =
          &fndUpdateFunctionTiny;
    /* Rotation Rate */
    if (evolve->bForceEqSpin[iBody]) {
      fnUpdate[iBody][update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] =
            &fndUpdateFunctionTiny;
    } else {
      fnUpdate[iBody][update[iBody].iRot][update[iBody].iaRotEqtide[iPert]] =
            &fndUpdateFunctionTiny;
    }
  }

  /* Is this the secondary body, and hence we assign da/dt and de/dt? */
  if (!bPrimary(body, iBody)) {
    // If the orbit is allowed to evolve, assign derivative functions
    fnUpdate[iBody][update[iBody].iSemi][update[iBody].iSemiEqtide] =
          &fndUpdateFunctionTiny;
    fnUpdate[iBody][update[iBody].iHecc][update[iBody].iHeccEqtide] =
          &fndUpdateFunctionTiny;
    fnUpdate[iBody][update[iBody].iKecc][update[iBody].iKeccEqtide] =
          &fndUpdateFunctionTiny;
  }
}

void VerifyEqtide(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  OUTPUT *output, SYSTEM *system, UPDATE *update, int iBody,
                  int iModule) {

  VerifyTideModel(control, files, options);

  VerifyOrbitEqtide(body, control, files, options);

  VerifyPerturbersEqtide(body, files, options, update,
                         control->Evolve.iNumBodies, iBody);

  VerifyRotationEqtide(body, control, update, options,
                       files->Infile[iBody + 1].cIn, iBody);

  /* Verify input set correctly and assign update functions */
  if (control->Evolve.iEqtideModel == CTL) {
    VerifyCTL(body, control, files, options, output, update, iBody, iModule);
  }

  if (control->Evolve.iEqtideModel == CPL) {
    VerifyCPL(body, control, files, options, output, update, iBody, iModule);
  }

  if (control->Evolve.iEqtideModel == DB15) {
    // Note that DB15 requires ThermInt, so this function lives in module.c
    VerifyDB15(body, control, files, options, output, update, iBody, iModule);
  }

  VerifyLostEngEqtide(body, update, control, options, iBody);

  body[iBody].dTidalZ   = malloc(control->Evolve.iNumBodies * sizeof(double));
  body[iBody].dTidalChi = malloc(control->Evolve.iNumBodies * sizeof(double));
  control->fnForceBehavior[iBody][iModule]   = &ForceBehaviorEqtide;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyEqtide;
}

/**************** EQTIDE Update ***************/

void InitializeUpdateEqtide(BODY *body, UPDATE *update, int iBody) {
  /* All active bodies can have their obliquity and rotation tracked. */
  if (update[iBody].iNumXobl == 0) {
    update[iBody].iNumVars++;
  }
  update[iBody].iNumXobl += body[iBody].iTidePerts;

  if (update[iBody].iNumYobl == 0) {
    update[iBody].iNumVars++;
  }
  update[iBody].iNumYobl += body[iBody].iTidePerts;

  if (update[iBody].iNumZobl == 0) {
    update[iBody].iNumVars++;
  }
  update[iBody].iNumZobl += body[iBody].iTidePerts;

  if (update[iBody].iNumRot == 0) {
    update[iBody].iNumVars++;
  }
  update[iBody].iNumRot += body[iBody].iTidePerts;

  if (update[iBody].iNumLostEng == 0) {
    update[iBody].iNumVars++;
  }
  update[iBody].iNumLostEng++;

  /* Eqtide is complicated by the possibility that 1 body could be perturbed
     by multiple others. saEqtidePertubers is the list of those bodies
     which are tidally interacting with it, and body->iaTidePert contains
     the body #s which are the perturbers. For now, the higher body number
     always carries a and e. In other words, right now only 1 central body
     is allowed. The addition of exomoons will make this more complicated.
  */

  // if (!bPrimary(body,iBody)) {
  if (iBody > 0) {
    if (update[iBody].iNumSemi == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumSemi++;

    if (update[iBody].iNumHecc == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumHecc++;

    if (update[iBody].iNumKecc == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumKecc++;
  }
}

void FinalizeUpdateHeccEqtide(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
  update[iBody].iHeccEqtide           = *iEqn;
  (*iEqn)++;
}

void FinalizeUpdateLostEngEqtide(BODY *body, UPDATE *update, int *iEqn,
                                 int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
  update[iBody].iLostEngEqtide        = *iEqn;
  (*iEqn)++;
}

void FinalizeUpdateKeccEqtide(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
  update[iBody].iKeccEqtide           = *iEqn;
  (*iEqn)++;
}

void FinalizeUpdateXoblEqtide(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  int iPert;

  update[iBody].padDXoblDtEqtide =
        malloc(body[iBody].iTidePerts * sizeof(double *));
  update[iBody].iaXoblEqtide = malloc(body[iBody].iTidePerts * sizeof(int));
  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
    update[iBody].iaXoblEqtide[iPert]   = (*iEqn)++;
  }
}

void FinalizeUpdateYoblEqtide(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  int iPert;

  update[iBody].padDYoblDtEqtide =
        malloc(body[iBody].iTidePerts * sizeof(double *));
  update[iBody].iaYoblEqtide = malloc(body[iBody].iTidePerts * sizeof(int));
  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
    update[iBody].iaYoblEqtide[iPert]   = (*iEqn)++;
  }
}

void FinalizeUpdateZoblEqtide(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  int iPert;

  update[iBody].padDZoblDtEqtide =
        malloc(body[iBody].iTidePerts * sizeof(double *));
  update[iBody].iaZoblEqtide = malloc(body[iBody].iTidePerts * sizeof(int));
  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
    update[iBody].iaZoblEqtide[iPert]   = (*iEqn)++;
  }
}

void FinalizeUpdateRotEqtide(BODY *body, UPDATE *update, int *iEqn, int iVar,
                             int iBody, int iFoo) {
  int iPert;

  update[iBody].padDrotDtEqtide =
        malloc(body[iBody].iTidePerts * sizeof(double *));
  update[iBody].iaRotEqtide = malloc(body[iBody].iTidePerts * sizeof(int));
  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
    update[iBody].iaRotEqtide[iPert]    = (*iEqn)++;
  }
}

void FinalizeUpdateSemiEqtide(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = EQTIDE;
  update[iBody].iSemiEqtide           = *iEqn;
  (*iEqn)++;
}


/***************** EQTIDE Halts *****************/

/* Double Synchronous? */

int HaltDblSync(BODY *body, EVOLVE *evolve, HALT *halt, IO *io, UPDATE *update,
                fnUpdateVariable ***fnUpdate, int iBody) {

  /* dMeanMotion set by call to TidalProperties in Evolve()
  Note that the two doubles can only equal each other if ForceBehaviorEqtide
  forces the planets to the tidal locking values. This conditional
  therefore ensures that the bTidalLock flag was selected and has been flipped
  to true. This distinction is important because two stars can pass through
  they double synchronous state by chance, about 1 in (10^16)^4 chance, so
  the conditional prevents this numerical artefact. */
  if (halt->bDblSync && (body[0].dRotRate == body[1].dMeanMotion) &&
      (body[1].dRotRate == body[1].dMeanMotion)) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: Double Synchronous at ");
      fprintd(stdout, evolve->dTime / YEARSEC, io->iSciNot, io->iDigits);
      printf(" years.\n");
    }
    return 1;
  }

  return 0;
}

/* Tide-locked? */
int HaltTideLock(BODY *body, EVOLVE *evolve, HALT *halt, IO *io, UPDATE *update,
                 fnUpdateVariable ***fnUpdate, int iBody) {

  if ((body[iBody].dRotRate == body[iBody].dMeanMotion) && halt->bTideLock) {
    // Tidally locked!
    body[iBody].bTideLock = 1;

    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s tide-locked at ", body[iBody].cName);
      fprintd(stdout, evolve->dTime / YEARSEC, io->iSciNot, io->iDigits);
      printf(" years.\n");
    }
    return 1;
  }

  return 0;
}

/* Synchronous Rotation? */
int HaltSyncRot(BODY *body, EVOLVE *evolve, HALT *halt, IO *io, UPDATE *update,
                fnUpdateVariable ***fnUpdate, int iBody) {

  if (halt->bSync && (body[iBody].dRotRate == body[iBody].dMeanMotion)) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's rotation is synchronous (e = ", body[iBody].cName);
      fprintd(stdout, sqrt(body[iBody].dEccSq), io->iSciNot, io->iDigits);
      printf(") at ");
      fprintd(stdout, evolve->dTime / YEARSEC, io->iSciNot, io->iDigits);
      printf(" years.\n");
    }
    return 1;
  }

  return 0;
}

void CountHaltsEqtide(HALT *halt, int *iNumHalts) {
  if (halt->bDblSync) {
    (*iNumHalts)++;
  }
  if (halt->bTideLock) {
    (*iNumHalts)++;
  }
  if (halt->bSync) {
    (*iNumHalts)++;
  }
}

void VerifyHaltEqtide(BODY *body, CONTROL *control, OPTIONS *options, int iBody,
                      int *iHalt) {

  if (control->Halt[iBody].bDblSync) {
    if (control->Evolve.iNumBodies > 2) {
      fprintf(stderr,
              "ERROR: Cannot set %s for systems with more than 2 bodies.\n",
              options[OPT_HALTDBLSYNC].cName);
      DoubleLineExit(options[OPT_BODYFILES].cFile[0],
                     options[OPT_HALTDBLSYNC].cFile[iBody + 1],
                     options[OPT_BODYFILES].iLine[0],
                     options[OPT_HALTDBLSYNC].iLine[iBody + 1]);
      exit(EXIT_INPUT);
    } else {
      control->fnHalt[iBody][(*iHalt)++] = &HaltDblSync;
    }
  }
  if (control->Halt[iBody].bTideLock) {
    if (control->Evolve.iNumBodies > 2 && iBody == 0) {
      fprintf(
            stderr,
            "ERROR: Cannot set %s's %s for systems with more than 2 bodies.\n",
            body[iBody].cName, options[OPT_HALTTIDELOCK].cName);
      DoubleLineExit(options[OPT_BODYFILES].cFile[0],
                     options[OPT_HALTTIDELOCK].cFile[iBody + 1],
                     options[OPT_BODYFILES].iLine[0],
                     options[OPT_HALTTIDELOCK].iLine[iBody + 1]);
      exit(EXIT_INPUT);
    } else {
      control->fnHalt[iBody][(*iHalt)++] = &HaltTideLock;
    }
  }
  if (control->Halt[iBody].bSync) {
    if (control->Evolve.iNumBodies > 2 && iBody == 0) {
      fprintf(
            stderr,
            "ERROR: Cannot set %s's %s for systems with more than 2 bodies.\n",
            body[iBody].cName, options[OPT_HALTSYNCROT].cName);
      DoubleLineExit(options[OPT_BODYFILES].cFile[0],
                     options[OPT_HALTSYNCROT].cFile[iBody + 1],
                     options[OPT_BODYFILES].iLine[0],
                     options[OPT_HALTSYNCROT].iLine[iBody + 1]);
      exit(EXIT_INPUT);
    } else {
      control->fnHalt[iBody][(*iHalt)++] = &HaltSyncRot;
    }
  }
}

/************* EQTIDE Outputs ******************/


/*
 * B
 */

void WriteBodyDsemiDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  /*  Broken

    int iPert;

  if (iBody == 0)
    iPert=1;
  else
    iPert=0;



  if (control->Evolve.iEqtideModel == CPL)
    *dTmp =
  fdCPLDsemiDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEccSq);
  else if (control->Evolve.iEqtideModel == CTL)
    *dTmp =
  fdCTLDsemiDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEccSq,body[iBody].dObliquity,body[iBody].dRotRate);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsLength(units->iLength);
    fsUnitsVel(units,cUnit);
  }
  */

  *dTmp = -1;
}

void WriteBodyDeccDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  /* Broken

  needs to be changed after switch to Hecc + Kecc  int iPert;

  if (iBody == 0)
    iPert=1;
  else
    iPert=0;


  if (control->Evolve.iEqtideModel == CPL)
    *dTmp =
  fdCPLDeccDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEccSq);
  else
    *dTmp =
  fdCTLDeccDtBody(body[iBody],body[iPert].dMass,body[1].dSemi,body[1].dEccSq);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
  */
  *dTmp = -1;
}

void WriteLockTime(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {
  *dTmp = body[iBody].dLockTime;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
  }
}

void WriteTidalRadius(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dTidalRadius;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

void WriteDOblDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  double dFoo;
  int iPert;

  // Total change in dObl/dTime
  dFoo = 0;
  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    dFoo += body[iBody].daDoblDtEqtide[body[iBody].iaTidePerts[iPert]];
  }

  *dTmp = dFoo;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime) / fdUnitsAngle(units->iAngle);
    fsUnitsAngRate(units, cUnit);
  }
}

void WriteTidalQOcean(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {

  /* Return -1 if dImK2Ocean is 0. There may be a better way to do this. */
  if (body[iBody].dImK2Ocean > 0) {
    *dTmp = body[iBody].dK2Ocean / body[iBody].dImK2Ocean;
  } else {
    *dTmp = -1;
  }

  strcpy(cUnit, "");
}

void WriteTidalQEnv(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {

  /* Return -1 if dImK2Env is 0. There may be a better way to do this. */
  if (body[iBody].dImK2Env > 0) {
    *dTmp = body[iBody].dK2Env / body[iBody].dImK2Env;
  } else {
    *dTmp = -1;
  }

  strcpy(cUnit, "");
}

void WriteDSemiDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  double dDeriv;

  /* Ensure that we don't overwrite pdDsemiDt */
  dDeriv = *(update[iBody].pdDsemiDtEqtide);
  *dTmp  = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime) / fdUnitsLength(units->iLength);
    fsUnitsVel(units, cUnit);
  }
}

void WriteDEccDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dDeccDtEqtide;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteDMeanMotionDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                              SYSTEM *system, UNITS *units, UPDATE *update,
                              int iBody, double *dTmp, char cUnit[]) {

  *dTmp = -1.5 *
          pow(BIGG * (body[0].dMass + body[iBody].dMass) /
                    pow(body[iBody].dSemi, 5),
              0.5) *
          (*(update[iBody].pdDsemiDtEqtide));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime) / fdUnitsLength(units->iLength);
    fsUnitsRate(units->iTime, cUnit);
    strcat(cUnit, "^2");
  }
}

void WriteDOrbPerDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {

  *dTmp = 1.5 *
          pow(4 * PI * PI * body[iBody].dSemi /
                    (BIGG * (body[0].dMass + body[iBody].dMass)),
              0.5) *
          (*(update[iBody].pdDsemiDtEqtide));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime) / fdUnitsLength(units->iLength);
    sprintf(cUnit, "%s", "");
  }
}

void WriteDRotPerDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    dDeriv += *(update[iBody].padDrotDtEqtide[iPert]);
  }

  /* Multiply by dP/domega to get dP/dt */
  *dTmp = dDeriv * (-2 * PI / (body[iBody].dRotRate * body[iBody].dRotRate));
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    strcpy(cUnit, "");
  }
}

void WriteDRotRateDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    dDeriv += *(update[iBody].padDrotDtEqtide[iPert]);
  }

  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime) * fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
    strcat(cUnit, "^2");
  }
}

void WriteDHeccDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  double dDeriv;

  /* Ensure that we don't overwrite pdDsemiDt */
  dDeriv = *(update[iBody].pdDHeccDtEqtide);
  *dTmp  = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteDKeccDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  double dDeriv;

  /* Ensure that we don't overwrite pdDsemiDt */
  dDeriv = *(update[iBody].pdDKeccDtEqtide);
  *dTmp  = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteDXoblDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  double dDeriv = 0;
  int iPert;

  /* Ensure that we don't overwrite pdDXoblDt */
  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    dDeriv +=
          (*(update[iBody].padDXoblDtEqtide[body[iBody].iaTidePerts[iPert]]));
  }
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteDYoblDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  double dDeriv = 0;
  int iPert;

  /* Ensure that we don't overwrite pdDYoblDt */
  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    dDeriv +=
          (*(update[iBody].padDYoblDtEqtide[body[iBody].iaTidePerts[iPert]]));
  }
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteDZoblDtEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  double dDeriv = 0;
  int iPert;

  /* Ensure that we don't overwrite pdDsemiDt */
  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    dDeriv +=
          (*(update[iBody].padDZoblDtEqtide[body[iBody].iaTidePerts[iPert]]));
  }
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

/*
 * E
 */

void WriteEccTimescaleEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  double dDeriv, dEcc;

  // Complicated because primary variable is h or k
  // de/dt = dh/dt * de/dh
  if (body[iBody].dLongP == 0) {
    dDeriv = *(update[iBody].pdDKeccDtEqtide) / cos(body[iBody].dLongP);
    dEcc   = body[iBody].dKecc / cos(body[iBody].dLongP);
  } else {
    dDeriv = *(update[iBody].pdDHeccDtEqtide) / sin(body[iBody].dLongP);
    dEcc   = body[iBody].dHecc / sin(body[iBody].dLongP);
  }

  *dTmp = fdTimescale(dEcc, dDeriv);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
  }
}

void WriteEqRotPer(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  int iOrbiter = fiAssignTidalOrbiter(body,iBody);
  *dTmp = fdFreqToPer(fdEqRotRate(
        body, iBody, body[iOrbiter].dMeanMotion, body[iOrbiter].dEccSq,
        control->Evolve.iEqtideModel, control->Evolve.bDiscreteRot));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
  }
}

void WriteEqRotPerCont(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {

  int iOrbiter = fiAssignTidalOrbiter(body,iBody);

  // To CPL, or to CTL? That is the question
  if (control->Evolve.iEqtideModel == CPL) {
    *dTmp = fdFreqToPer(fdCPLEqRotRateCont(body[iOrbiter].dMeanMotion,
                                           body[iOrbiter].dEccSq));
  } else {
    *dTmp = fdFreqToPer(fdCTLEqRotRate(body[iOrbiter].dMeanMotion,
                                       body[iBody].dObliquity,
                                       body[iOrbiter].dEccSq));
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
  }
}

void WriteEqRotPerDiscrete(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {

  int iOrbiter = fiAssignTidalOrbiter(body,iBody);
  if (control->Evolve.iEqtideModel == CPL) {
    *dTmp = fdFreqToPer(fdCPLEqRotRateDiscrete(body[iOrbiter].dMeanMotion,
                                               body[iOrbiter].dEccSq));
  } else {
    *dTmp = -1;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
  }
}

void WriteEqRotRate(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {

  int iOrbiter = fiAssignTidalOrbiter(body,iBody);
  *dTmp = fdEqRotRate(body, iBody, body[iOrbiter].dMeanMotion,
                      body[iOrbiter].dEccSq, control->Evolve.iEqtideModel,
                      control->Evolve.bDiscreteRot);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteEqRotRateCont(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {

  int iOrbiter = fiAssignTidalOrbiter(body,iBody);

  // To CPL, or to CTL? That is the question
  if (control->Evolve.iEqtideModel == CPL) {
    *dTmp =
          fdCPLEqRotRateCont(body[iOrbiter].dMeanMotion, body[iOrbiter].dEccSq);
  } else {
    *dTmp = fdCTLEqRotRate(body[iOrbiter].dMeanMotion, body[iBody].dObliquity,
                           body[iOrbiter].dEccSq);
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteEqRotRateDiscrete(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  int iOrbiter = fiAssignTidalOrbiter(body,iBody);

  if (control->Evolve.iEqtideModel == CPL) {
    *dTmp = fdCPLEqRotRateDiscrete(body[iOrbiter].dMeanMotion,
                                   body[iOrbiter].dEccSq);
  } else {
    *dTmp = -1;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteEqTidePower(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {

  int iOrbiter = fiAssignTidalOrbiter(body,iBody);
  // Why is this Eq??????? XXX
  if (control->Evolve.iEqtideModel == CPL) {
    *dTmp = fdCPLTidePowerEq(body[iBody].dTidalZ[iOrbiter], body[iBody].dEccSq,
                             body[iBody].dMeanMotion, body[iBody].dObliquity,
                             control->Evolve.bDiscreteRot);
  } else {
    // XXX Add CTL functions
    *dTmp = -1;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

/*
 * G
 */

void WriteGammaOrb(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  /* Broken */
  //*dTmp =
  // fdGammaOrb(body[iBody].dEccSq,body[iBody].dObliquity,body[iBody].iTidalEpsilon[0]);
  *dTmp = -1;

  /* Negative option? */
  strcat(cUnit, "sec");
}

void WriteGammaRot(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  /*
  Broken

  *dTmp =
  fdGammaRot(body[1].dEccSq,body[iBody].dObliquity,body[iBody].iTidalEpsilon[0]);
  */
  *dTmp = -1;

  /* Negative option? */
  strcat(cUnit, "sec");
}

void WriteK2Ocean(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {

  *dTmp = body[iBody].dK2Ocean;

  strcpy(cUnit, "");
}

void WriteK2Env(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                UNITS *units, UPDATE *update, int iBody, double *dTmp,
                char cUnit[]) {

  *dTmp = body[iBody].dK2Env;

  strcpy(cUnit, "");
}

void WriteOblTimescaleEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {

  /* Broken


  Need to change after switch to [XYZ]obl
  *dTmp =
  fdTimescaleMulti(body[iBody].dObliquity,*(update[iBody].padDoblDtEqtide),body[iBody].iTidePerts);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime,cUnit);
  }
  */
  *dTmp = -1;
  strcpy(cUnit, "");
}

void WriteRotTimescaleEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  *dTmp =
        fdTimescaleMulti(body[iBody].dRotRate, *(update[iBody].padDrotDtEqtide),
                         body[iBody].iTidePerts);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
  }
}

/*
 *S
 */

void WriteSemiTimescaleEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                              SYSTEM *system, UNITS *units, UPDATE *update,
                              int iBody, double *dTmp, char cUnit[]) {
  *dTmp = fdTimescale(body[iBody].dSemi, *(update[iBody].pdDsemiDtEqtide));

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
  }
}

/*
 * T
 */

void WritePowerEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  /* Get total tidal power */
  *dTmp = fdTidePower(body, iBody, control->Evolve.iEqtideModel);
  //*dTmp = body[iBody].dTidalPowMan;


  int jBody=0;
  if (iBody==0)
    jBody++;
    /*
  fprintf(stderr,"\niBody: %d\n",iBody);
  fprintf(stderr,"\njBody: %d\n",jBody);
  fprintf(stderr,"TidalZ: %lf\n",body[iBody].dTidalZ[jBody]);
*/

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

void WriteEnergyFluxEqtide(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  /* Get total tidal surface energy flux */
  *dTmp = fdSurfEnFluxEqtide(body, system, update, iBody,
                             control->Evolve.iEqtideModel);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

void WriteTidalTau(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {
  *dTmp = body[iBody].dTidalTau;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
  }
}

void WriteTideLock(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  *dTmp = control->Evolve.bForceEqSpin[iBody];
  strcat(cUnit, "");
}

void InitializeOutputEqtide(OUTPUT *output, fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_BODYDSEMIDTEQTIDE].cName, "BodyDsemiDtEqtide");
  sprintf(output[OUT_BODYDSEMIDTEQTIDE].cDescr,
          "Body's Contribution to dSemi/dt in EqTide");
  sprintf(output[OUT_BODYDSEMIDTEQTIDE].cNeg, "AU/Gyr");
  output[OUT_BODYDSEMIDTEQTIDE].bNeg       = 1;
  output[OUT_BODYDSEMIDTEQTIDE].dNeg       = YEARSEC * 1e9 / AUM;
  output[OUT_BODYDSEMIDTEQTIDE].iNum       = 1;
  output[OUT_BODYDSEMIDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_BODYDSEMIDTEQTIDE]           = &WriteBodyDsemiDtEqtide;
  sprintf(
        output[OUT_BODYDSEMIDTEQTIDE].cDescr,
        "In EqTide, the total change in semi-major axis is due to dissipation "
        "in "
        "both bodies. This output parameter returns the change due to current "
        "body.");

  sprintf(output[OUT_BODYDECCDTEQTIDE].cName, "BodyDeccDt");
  sprintf(output[OUT_BODYDECCDTEQTIDE].cDescr,
          "Body's Contribution to dEcc/dt in EqTide");
  sprintf(output[OUT_BODYDECCDTEQTIDE].cNeg, "/Gyr");
  output[OUT_BODYDECCDTEQTIDE].bNeg       = 1;
  output[OUT_BODYDECCDTEQTIDE].dNeg       = YEARSEC * 1e9;
  output[OUT_BODYDECCDTEQTIDE].iNum       = 1;
  output[OUT_BODYDECCDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_BODYDECCDTEQTIDE]           = &WriteBodyDeccDtEqtide;
  sprintf(
        output[OUT_BODYDECCDTEQTIDE].cDescr,
        "In EqTide, the total change in eccentricity is due to towrues on both "
        "bodies. This output parameter returns the change due to current "
        "body.");

  sprintf(output[OUT_DOBLDTEQTIDE].cName, "DOblDtEqtide");
  sprintf(output[OUT_DOBLDTEQTIDE].cDescr,
          "Time Rate of Change of Obliquity in EqTide");
  sprintf(output[OUT_DOBLDTEQTIDE].cNeg, "deg/Gyr");
  output[OUT_DOBLDTEQTIDE].bNeg       = 1;
  output[OUT_DOBLDTEQTIDE].dNeg       = YEARSEC * 1e9 / DEGRAD;
  output[OUT_DOBLDTEQTIDE].iNum       = 1;
  output[OUT_DOBLDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DOBLDTEQTIDE]           = &WriteDOblDtEqtide;

  sprintf(output[OUT_TIDALQOCEAN].cName, "OceanTidalQ");
  sprintf(output[OUT_TIDALQOCEAN].cDescr, "Ocean Tidal Q");
  output[OUT_TIDALQOCEAN].bNeg       = 0;
  output[OUT_TIDALQOCEAN].iNum       = 1;
  output[OUT_TIDALQOCEAN].iModuleBit = EQTIDE;
  fnWrite[OUT_TIDALQOCEAN]           = WriteTidalQOcean;

  sprintf(output[OUT_TIDALRADIUS].cName, "TidalRadius");
  sprintf(output[OUT_TIDALRADIUS].cDescr, "Tidal Radius");
  sprintf(output[OUT_TIDALRADIUS].cNeg, "Rearth");
  output[OUT_TIDALRADIUS].bNeg       = 1;
  output[OUT_TIDALRADIUS].dNeg       = 1. / REARTH;
  output[OUT_TIDALRADIUS].iNum       = 1;
  output[OUT_TIDALRADIUS].iModuleBit = EQTIDE;
  fnWrite[OUT_TIDALRADIUS]           = &WriteTidalRadius;
  sprintf(
        output[OUT_TIDALRADIUS].cDescr,
        "In EqTide, the value of the radius that enters the tidal evolution "
        "equations. This parameter is distinct from others, such as the radius "
        "of XUV absorption or a pressure level in an atmosphere.");

  sprintf(output[OUT_TIDALQENV].cName, "EnvTidalQ");
  sprintf(output[OUT_TIDALQENV].cDescr, "Envelope Tidal Q");
  output[OUT_TIDALQENV].bNeg       = 0;
  output[OUT_TIDALQENV].iNum       = 1;
  output[OUT_TIDALQENV].iModuleBit = EQTIDE;
  fnWrite[OUT_TIDALQENV]           = WriteTidalQEnv;

  sprintf(output[OUT_DSEMIDTEQTIDE].cName, "DsemiDtEqtide");
  sprintf(output[OUT_DSEMIDTEQTIDE].cDescr, "Total da/dt from EqTide");
  sprintf(output[OUT_DSEMIDTEQTIDE].cNeg, "AU/Gyr");
  output[OUT_DSEMIDTEQTIDE].bNeg       = 1;
  output[OUT_DSEMIDTEQTIDE].dNeg       = (YEARSEC * 1e9) / AUM;
  output[OUT_DSEMIDTEQTIDE].iNum       = 1;
  output[OUT_DSEMIDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DSEMIDTEQTIDE]           = &WriteDSemiDtEqtide;

  sprintf(output[OUT_DECCDTEQTIDE].cName, "DeccDtEqtide");
  sprintf(output[OUT_DECCDTEQTIDE].cDescr, "Total de/dt from EqTide");
  sprintf(output[OUT_DECCDTEQTIDE].cNeg, "/Gyr");
  output[OUT_DECCDTEQTIDE].bNeg       = 1;
  output[OUT_DECCDTEQTIDE].dNeg       = YEARSEC * 1e9;
  output[OUT_DECCDTEQTIDE].iNum       = 1;
  output[OUT_DECCDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DECCDTEQTIDE]           = &WriteDEccDtEqtide;

  sprintf(output[OUT_DMMDTEQTIDE].cName, "DMeanMotionDtEqtide");
  sprintf(output[OUT_DMMDTEQTIDE].cDescr, "Total dMeanMotion/dt from EqTide");
  output[OUT_DMMDTEQTIDE].iNum       = 1;
  output[OUT_DMMDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DMMDTEQTIDE]           = &WriteDMeanMotionDtEqtide;

  sprintf(output[OUT_DORBPERDTEQTIDE].cName, "DOrbPerDtEqtide");
  sprintf(output[OUT_DORBPERDTEQTIDE].cDescr, "Total dOrbPer/dt from EqTide");
  sprintf(output[OUT_DORBPERDTEQTIDE].cNeg, "days/Gyr");
  output[OUT_DORBPERDTEQTIDE].bNeg       = 1;
  output[OUT_DORBPERDTEQTIDE].dNeg       = YEARSEC * 1e9 / DAYSEC;
  output[OUT_DORBPERDTEQTIDE].iNum       = 1;
  output[OUT_DORBPERDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DORBPERDTEQTIDE]           = &WriteDOrbPerDtEqtide;

  sprintf(output[OUT_DROTPERDTEQTIDE].cName, "DRotPerDtEqtide");
  sprintf(output[OUT_DROTPERDTEQTIDE].cDescr,
          "Time Rate of Change of Rotation Period in EqTide");
  sprintf(output[OUT_DROTPERDTEQTIDE].cNeg, "days/Myr");
  output[OUT_DROTPERDTEQTIDE].bNeg       = 1;
  output[OUT_DROTPERDTEQTIDE].dNeg       = DAYSEC / (YEARSEC * 1e6);
  output[OUT_DROTPERDTEQTIDE].iNum       = 1;
  output[OUT_DROTPERDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DROTPERDTEQTIDE]           = &WriteDRotPerDtEqtide;

  sprintf(output[OUT_DROTRATEDTEQTIDE].cName, "DRotRateDtEqtide");
  sprintf(output[OUT_DROTRATEDTEQTIDE].cDescr,
          "Time Rate of Change of Rotation Rate in EqTide");
  output[OUT_DROTRATEDTEQTIDE].bNeg       = 0;
  output[OUT_DROTRATEDTEQTIDE].iNum       = 1;
  output[OUT_DROTRATEDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DROTRATEDTEQTIDE]           = &WriteDRotRateDtEqtide;

  sprintf(output[OUT_DHECCDTEQTIDE].cName, "DHEccDtEqtide");
  sprintf(output[OUT_DHECCDTEQTIDE].cDescr,
          "Time Rate of Change of Poincare's h in EqTide");
  sprintf(output[OUT_DHECCDTEQTIDE].cNeg, "/Gyr");
  output[OUT_DHECCDTEQTIDE].bNeg       = 1;
  output[OUT_DHECCDTEQTIDE].dNeg       = (1e9 * YEARSEC);
  output[OUT_DHECCDTEQTIDE].iNum       = 1;
  output[OUT_DHECCDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DHECCDTEQTIDE]           = &WriteDHeccDtEqtide;

  sprintf(output[OUT_DKECCDTEQTIDE].cName, "DKEccDtEqtide");
  sprintf(output[OUT_DKECCDTEQTIDE].cDescr,
          "Time Rate of Change of Poincare's k in EqTide");
  sprintf(output[OUT_DKECCDTEQTIDE].cNeg, "/Gyr");
  output[OUT_DKECCDTEQTIDE].bNeg       = 1;
  output[OUT_DKECCDTEQTIDE].dNeg       = (1e9 * YEARSEC);
  output[OUT_DKECCDTEQTIDE].iNum       = 1;
  output[OUT_DKECCDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DKECCDTEQTIDE]           = &WriteDKeccDtEqtide;

  sprintf(output[OUT_DXOBLDTEQTIDE].cName, "DXoblDtEqtide");
  sprintf(output[OUT_DXOBLDTEQTIDE].cDescr,
          "Time Rate of Change of Laskar's X in EqTide");
  sprintf(output[OUT_DXOBLDTEQTIDE].cNeg, "/Gyr");
  output[OUT_DXOBLDTEQTIDE].bNeg       = 1;
  output[OUT_DXOBLDTEQTIDE].dNeg       = (1e9 * YEARSEC);
  output[OUT_DXOBLDTEQTIDE].iNum       = 1;
  output[OUT_DXOBLDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DXOBLDTEQTIDE]           = &WriteDXoblDtEqtide;

  sprintf(output[OUT_DYOBLDTEQTIDE].cName, "DYoblDtEqtide");
  sprintf(output[OUT_DYOBLDTEQTIDE].cDescr,
          "Time Rate of Change of Laskar's Y in EqTide");
  sprintf(output[OUT_DYOBLDTEQTIDE].cNeg, "/Gyr");
  output[OUT_DYOBLDTEQTIDE].bNeg       = 1;
  output[OUT_DYOBLDTEQTIDE].dNeg       = (1e9 * YEARSEC);
  output[OUT_DYOBLDTEQTIDE].iNum       = 1;
  output[OUT_DYOBLDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DYOBLDTEQTIDE]           = &WriteDYoblDtEqtide;

  sprintf(output[OUT_DZOBLDTEQTIDE].cName, "DZoblDtEqtide");
  sprintf(output[OUT_DZOBLDTEQTIDE].cDescr,
          "Time Rate of Change of Laskar's Z in EqTide");
  sprintf(output[OUT_DZOBLDTEQTIDE].cNeg, "/Gyr");
  output[OUT_DZOBLDTEQTIDE].bNeg       = 1;
  output[OUT_DZOBLDTEQTIDE].dNeg       = (1e9 * YEARSEC);
  output[OUT_DZOBLDTEQTIDE].iNum       = 1;
  output[OUT_DZOBLDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DZOBLDTEQTIDE]           = &WriteDZoblDtEqtide;

  /*
   * E
   */

  sprintf(output[OUT_ECCTIMEEQTIDE].cName, "EccTimeEqtide");
  sprintf(output[OUT_ECCTIMEEQTIDE].cDescr,
          "Timescale for Eccentricity Evolution (e/[de/dt]) in EqTide");
  sprintf(output[OUT_ECCTIMEEQTIDE].cNeg, "years");
  output[OUT_ECCTIMEEQTIDE].bNeg       = 1;
  output[OUT_ECCTIMEEQTIDE].dNeg       = 1. / YEARSEC;
  output[OUT_ECCTIMEEQTIDE].iNum       = 1;
  output[OUT_ECCTIMEEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_ECCTIMEEQTIDE]           = &WriteEccTimescaleEqtide;

  sprintf(output[OUT_EQROTPER].cName, "EqRotPer");
  sprintf(output[OUT_EQROTPER].cDescr, "Equilibrium Rotation Period");
  sprintf(output[OUT_EQROTPER].cNeg, "days");
  output[OUT_EQROTPER].bNeg       = 1;
  output[OUT_EQROTPER].dNeg       = 1. / DAYSEC;
  output[OUT_EQROTPER].iNum       = 1;
  output[OUT_EQROTPER].iModuleBit = EQTIDE;
  fnWrite[OUT_EQROTPER]           = &WriteEqRotPer;

  sprintf(output[OUT_EQROTPERCONT].cName, "EqRotPerCont");
  sprintf(output[OUT_EQROTPERCONT].cDescr,
          "CPL2 Continuous Equilibrium Rotation Period");
  sprintf(output[OUT_EQROTPERCONT].cNeg, "days");
  output[OUT_EQROTPERCONT].bNeg       = 1;
  output[OUT_EQROTPERCONT].dNeg       = 1. / DAYSEC;
  output[OUT_EQROTPERCONT].iNum       = 1;
  output[OUT_EQROTPERCONT].iModuleBit = EQTIDE;
  fnWrite[OUT_EQROTPERCONT]           = &WriteEqRotPerCont;

  sprintf(output[OUT_EQROTPERDISCRETE].cName, "EqRotPerDiscrete");
  sprintf(output[OUT_EQROTPERDISCRETE].cDescr,
          "CPL2 Discrete Equilibrium Spin Period");
  sprintf(output[OUT_EQROTPERDISCRETE].cNeg, "days");
  output[OUT_EQROTPERDISCRETE].bNeg       = 1;
  output[OUT_EQROTPERDISCRETE].dNeg       = 1. / DAYSEC;
  output[OUT_EQROTPERDISCRETE].iNum       = 1;
  output[OUT_EQROTPERDISCRETE].iModuleBit = EQTIDE;
  fnWrite[OUT_EQROTPERDISCRETE]           = &WriteEqRotPerDiscrete;

  sprintf(output[OUT_EQROTRATE].cName, "EqRotRate");
  sprintf(output[OUT_EQROTRATE].cDescr, "Equilibrium Rotation Rate");
  sprintf(output[OUT_EQROTRATE].cNeg, "/day");
  output[OUT_EQROTRATE].bNeg       = 1;
  output[OUT_EQROTRATE].dNeg       = DAYSEC;
  output[OUT_EQROTRATE].iNum       = 1;
  output[OUT_EQROTRATE].iModuleBit = EQTIDE;
  fnWrite[OUT_EQROTRATE]           = &WriteEqRotRate;

  sprintf(output[OUT_EQROTRATECONT].cName, "EqRotRateCont");
  sprintf(output[OUT_EQROTRATECONT].cDescr,
          "CPL2 Continuous Equilibrium Spin Rate");
  sprintf(output[OUT_EQROTRATECONT].cNeg, "/day");
  output[OUT_EQROTRATE].bNeg           = 1;
  output[OUT_EQROTRATECONT].dNeg       = DAYSEC;
  output[OUT_EQROTRATECONT].iNum       = 1;
  output[OUT_EQROTRATECONT].iModuleBit = EQTIDE;
  fnWrite[OUT_EQROTRATECONT]           = &WriteEqRotRateCont;

  sprintf(output[OUT_EQROTRATEDISCRETE].cName, "EqRotRateDiscrete");
  sprintf(output[OUT_EQROTRATEDISCRETE].cDescr,
          "CPL2 Discrete Equilibrium Spin Rate");
  sprintf(output[OUT_EQROTRATEDISCRETE].cNeg, "/day");
  output[OUT_EQROTRATEDISCRETE].bNeg       = 1;
  output[OUT_EQROTRATEDISCRETE].dNeg       = DAYSEC;
  output[OUT_EQROTRATEDISCRETE].iNum       = 1;
  output[OUT_EQROTRATEDISCRETE].iModuleBit = EQTIDE;
  fnWrite[OUT_EQROTRATEDISCRETE]           = &WriteEqRotRateDiscrete;

  sprintf(output[OUT_EQTIDEPOWER].cName, "EqTidePower");
  sprintf(output[OUT_EQTIDEPOWER].cDescr, "Equilibrium Power from Tides");
  sprintf(output[OUT_EQTIDEPOWER].cNeg, "/day");
  output[OUT_EQTIDEPOWER].bNeg       = 1;
  output[OUT_EQTIDEPOWER].dNeg       = DAYSEC;
  output[OUT_EQTIDEPOWER].iNum       = 1;
  output[OUT_EQTIDEPOWER].iModuleBit = EQTIDE;
  fnWrite[OUT_EQTIDEPOWER]           = &WriteEqTidePower;


  /*
   * G
   */

  sprintf(output[OUT_GAMMAROT].cName, "GammaRot");
  sprintf(output[OUT_GAMMAROT].cDescr, "Gamma_Rotation");
  output[OUT_GAMMAROT].bNeg       = 0;
  output[OUT_GAMMAROT].iNum       = 1;
  output[OUT_GAMMAROT].iModuleBit = EQTIDE;
  fnWrite[OUT_GAMMAROT]           = &WriteGammaRot;

  sprintf(output[OUT_GAMMAORB].cName, "GammaOrb");
  sprintf(output[OUT_GAMMAORB].cDescr, "Gamma_Orbital");
  output[OUT_GAMMAORB].bNeg       = 0;
  output[OUT_GAMMAORB].iNum       = 1;
  output[OUT_GAMMAORB].iModuleBit = EQTIDE;
  fnWrite[OUT_GAMMAORB]           = &WriteGammaOrb;

  sprintf(output[OUT_K2OCEAN].cName, "OceanK2");
  sprintf(output[OUT_K2OCEAN].cDescr, "Ocean's Love number k_2");
  output[OUT_K2OCEAN].bNeg       = 0;
  output[OUT_K2OCEAN].iNum       = 1;
  output[OUT_K2OCEAN].iModuleBit = EQTIDE;
  fnWrite[OUT_K2OCEAN]           = &WriteK2Ocean;

  sprintf(output[OUT_K2ENV].cName, "EnvK2");
  sprintf(output[OUT_K2ENV].cDescr, "Envelope's Love number k_2");
  output[OUT_K2ENV].bNeg       = 0;
  output[OUT_K2ENV].iNum       = 1;
  output[OUT_K2ENV].iModuleBit = EQTIDE;
  fnWrite[OUT_K2ENV]           = &WriteK2Env;

  /*
   * O
   */

  sprintf(output[OUT_OBLTIMEEQTIDE].cName, "OblTimeEqtide");
  sprintf(output[OUT_OBLTIMEEQTIDE].cDescr,
          "Timescale for Obliquity Evolution in EqTide");
  sprintf(output[OUT_OBLTIMEEQTIDE].cNeg, "years");
  output[OUT_OBLTIMEEQTIDE].bNeg       = 1;
  output[OUT_OBLTIMEEQTIDE].dNeg       = 1. / YEARSEC;
  output[OUT_OBLTIMEEQTIDE].iNum       = 1;
  output[OUT_OBLTIMEEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_OBLTIMEEQTIDE]           = &WriteOblTimescaleEqtide;

  /*
   * R
   */

  sprintf(output[OUT_ROTRATETIMEEQTIDE].cName, "RotTimeEqtide");
  sprintf(output[OUT_ROTRATETIMEEQTIDE].cDescr,
          "Timescale for Rotational Evolution in EqTide");
  output[OUT_ROTRATETIMEEQTIDE].bNeg = 0;
  sprintf(output[OUT_ROTRATETIMEEQTIDE].cNeg, "years");
  output[OUT_ROTRATETIMEEQTIDE].bNeg       = 1;
  output[OUT_ROTRATETIMEEQTIDE].dNeg       = 1. / YEARSEC;
  output[OUT_ROTRATETIMEEQTIDE].iNum       = 1;
  output[OUT_ROTRATETIMEEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_ROTRATETIMEEQTIDE]           = &WriteRotTimescaleEqtide;

  sprintf(output[OUT_DROTPERDTEQTIDE].cName, "DRotPerDtEqtide");
  sprintf(output[OUT_DROTPERDTEQTIDE].cDescr,
          "Time Rate of Change of Rotation Period in EqTide");
  sprintf(output[OUT_DROTPERDTEQTIDE].cNeg, "days/Myr");
  output[OUT_DROTPERDTEQTIDE].bNeg       = 1;
  output[OUT_DROTPERDTEQTIDE].dNeg       = DAYSEC / (1e6 * YEARSEC);
  output[OUT_DROTPERDTEQTIDE].iNum       = 1;
  output[OUT_DROTPERDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DROTPERDTEQTIDE]           = &WriteDRotPerDtEqtide;

  sprintf(output[OUT_DROTRATEDTEQTIDE].cName, "DRotRateDtEqtide");
  sprintf(output[OUT_DROTRATEDTEQTIDE].cDescr,
          "Time Rate of Change of Rotational Frequency in EqTide");
  output[OUT_DROTRATEDTEQTIDE].bNeg       = 0;
  output[OUT_DROTRATEDTEQTIDE].iNum       = 1;
  output[OUT_DROTRATEDTEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_DROTRATEDTEQTIDE]           = &WriteDRotRateDtEqtide;

  /*
   * S
   */

  sprintf(output[OUT_SEMITIMEEQTIDE].cName, "SemiTimeEqtide");
  sprintf(output[OUT_SEMITIMEEQTIDE].cDescr,
          "Timescale for Semi-major Axis Evolution (a/[da/dt]) in EqTide");
  output[OUT_SEMITIMEEQTIDE].bNeg = 0;
  sprintf(output[OUT_SEMITIMEEQTIDE].cNeg, "years");
  output[OUT_SEMITIMEEQTIDE].bNeg       = 1;
  output[OUT_SEMITIMEEQTIDE].dNeg       = 1. / YEARSEC;
  output[OUT_SEMITIMEEQTIDE].iNum       = 1;
  output[OUT_SEMITIMEEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_SEMITIMEEQTIDE]           = &WriteSemiTimescaleEqtide;

  /*
   * T
   */

  sprintf(output[OUT_TIDALTAU].cName, "TidalTau");
  sprintf(output[OUT_TIDALTAU].cDescr, "Tidal Time Lag");
  sprintf(output[OUT_TIDALTAU].cNeg, "sec");
  output[OUT_TIDALTAU].bNeg       = 1;
  output[OUT_TIDALTAU].dNeg       = 1;
  output[OUT_TIDALTAU].iNum       = 1;
  output[OUT_TIDALTAU].iModuleBit = EQTIDE;
  fnWrite[OUT_TIDALTAU]           = &WriteTidalTau;

  sprintf(output[OUT_ENFLUXEQTIDE].cName, "SurfEnFluxEqtide");
  sprintf(output[OUT_ENFLUXEQTIDE].cDescr,
          "Surface Energy Flux due to Tides in EqTide");
  sprintf(output[OUT_ENFLUXEQTIDE].cNeg, "W/m^2");
  output[OUT_ENFLUXEQTIDE].bNeg       = 1;
  output[OUT_ENFLUXEQTIDE].dNeg       = 1;
  output[OUT_ENFLUXEQTIDE].iNum       = 1;
  output[OUT_ENFLUXEQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_ENFLUXEQTIDE]           = &WriteEnergyFluxEqtide;

  sprintf(output[OUT_POWEREQTIDE].cName, "PowerEqtide");
  sprintf(output[OUT_POWEREQTIDE].cDescr,
          "Internal Power due to Tides in EqTide");
  sprintf(output[OUT_POWEREQTIDE].cNeg, "TW");
  output[OUT_POWEREQTIDE].bNeg       = 1;
  output[OUT_POWEREQTIDE].dNeg       = 1e-12;
  output[OUT_POWEREQTIDE].iNum       = 1;
  output[OUT_POWEREQTIDE].iModuleBit = EQTIDE;
  fnWrite[OUT_POWEREQTIDE]           = &WritePowerEqtide;

  sprintf(output[OUT_TIDELOCK].cName, "TideLock");
  sprintf(output[OUT_TIDELOCK].cDescr, "Tidally Locked?");
  output[OUT_TIDELOCK].bNeg       = 0;
  output[OUT_TIDELOCK].iNum       = 1;
  output[OUT_TIDELOCK].iModuleBit = EQTIDE;
  fnWrite[OUT_TIDELOCK]           = &WriteTideLock;

  sprintf(output[OUT_LOCKTIME].cName, "LockTime");
  sprintf(output[OUT_LOCKTIME].cDescr, "Time when body tidally locked.");
  sprintf(output[OUT_LOCKTIME].cNeg, "Myr");
  output[OUT_LOCKTIME].bNeg       = 1;
  output[OUT_LOCKTIME].dNeg       = 1. / (YEARSEC * 1e6);
  output[OUT_LOCKTIME].iNum       = 1;
  output[OUT_LOCKTIME].iModuleBit = EQTIDE;
  fnWrite[OUT_LOCKTIME]           = &WriteLockTime;
}

/************ EQTIDE Logging Functions **************/

void LogOptionsEqtide(CONTROL *control, FILE *fp) {

  fprintf(fp, "-------- EQTIDE Options -----\n\n");
  /* Tidal Model */
  fprintf(fp, "Tidal Model: ");
  if (control->Evolve.iEqtideModel == CPL) {
    fprintf(fp, "Constant-Phase-Lag, 2nd order\n");
    fprintf(fp, "Use Discrete Rotation Rate Model: %d\n",
            control->Evolve.bDiscreteRot);
  }

  if (control->Evolve.iEqtideModel == CTL) {
    fprintf(fp, "Constant-Time-Lag, 8th order\n");
  }
}

void LogEqtide(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {
  int iOut;

  fprintf(fp, "\n----- EQTIDE PARAMETERS ------\n");
  for (iOut = OUTSTARTEQTIDE; iOut < OUTBODYSTARTEQTIDE; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, 0);
    }
  }
}

void LogBodyEqtide(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UPDATE *update, fnWriteOutput fnWrite[], FILE *fp,
                   int iBody) {
  int iOut, iPert, iStart;

  // This distinction because the central body does not have an orbit

  if (iBody > 0) {
    iStart = OUTSTARTEQTIDE;
  } else {
    iStart = OUTBODYSTARTEQTIDE;
  }

  fprintf(fp, "----- EQTIDE PARAMETERS (%s)------\n", body[iBody].cName);
  for (iOut = iStart; iOut < OUTENDEQTIDE; iOut++) {
    if (output[iOut].iNum > 0) {
      //fprintf(stderr,"iOut = %d.\n",iOut);
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
  }
  fprintf(fp, "Tidal Perturbers:");
  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    fprintf(fp, " %s", body[body[iBody].iaTidePerts[iPert]].cName);
    if (iPert < body[iBody].iTidePerts - 1) {
      fprintf(fp, ",");
    }
  }
  fprintf(fp, "\n");
}

/************* MODULE Functions ***********/

void AddModuleEqtide(CONTROL *control, MODULE *module, int iBody, int iModule) {

  module->iaModule[iBody][iModule] = EQTIDE;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlEqtide;
  module->fnInitializeUpdateTmpBody[iBody][iModule] =
        &InitializeUpdateTmpBodyEqtide;
  module->fnCountHalts[iBody][iModule] = &CountHaltsEqtide;
  module->fnLogBody[iBody][iModule]    = &LogBodyEqtide;

  module->fnReadOptions[iBody][iModule]       = &ReadOptionsEqtide;
  module->fnVerify[iBody][iModule]            = &VerifyEqtide;
  module->fnAssignDerivatives[iBody][iModule] = &AssignEqtideDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullEqtideDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltEqtide;

  module->fnInitializeBody[iBody][iModule]     = &InitializeBodyEqtide;
  module->fnInitializeUpdate[iBody][iModule]   = &InitializeUpdateEqtide;
  module->fnInitializeOutput[iBody][iModule]   = &InitializeOutputEqtide;
  module->fnFinalizeUpdateHecc[iBody][iModule] = &FinalizeUpdateHeccEqtide;
  module->fnFinalizeUpdateKecc[iBody][iModule] = &FinalizeUpdateKeccEqtide;
  module->fnFinalizeUpdateRot[iBody][iModule]  = &FinalizeUpdateRotEqtide;
  module->fnFinalizeUpdateSemi[iBody][iModule] = &FinalizeUpdateSemiEqtide;
  module->fnFinalizeUpdateXobl[iBody][iModule] = &FinalizeUpdateXoblEqtide;
  module->fnFinalizeUpdateYobl[iBody][iModule] = &FinalizeUpdateYoblEqtide;
  module->fnFinalizeUpdateZobl[iBody][iModule] = &FinalizeUpdateZoblEqtide;
  module->fnFinalizeUpdateLostEng[iBody][iModule] =
        &FinalizeUpdateLostEngEqtide;
}

/************* EQTIDE Functions ************/

double fdEqRotRate(BODY *body, int iBody, double dMeanMotion, double dEccSq,
                   int iTideModel, int bDiscreteRot) {

  if (iTideModel == CPL ||
      iTideModel == DB15) { // Does DB15 = CPL for tidally locked rotation?
    return fdCPLEqRotRate(dEccSq, dMeanMotion, bDiscreteRot);
  } else if (iTideModel == CTL) {
    return fdCTLEqRotRate(dEccSq, body[iBody].dObliquity, dMeanMotion);
  }
  /* Whoops! */
  assert(0);
  return dMeanMotion;
}

void fdaChi(BODY *body, double dMeanMotion, double dSemi, int iBody,
            int iPert) {
  body[iBody].dTidalChi[iPert] =
        body[iBody].dRadGyra * body[iBody].dRadGyra * body[iBody].dTidalRadius *
        body[iBody].dTidalRadius * body[iBody].dRotRate * dSemi * dMeanMotion /
        (BIGG * body[iPert].dMass);
}

int fbTidalLock(BODY *body, EVOLVE *evolve, IO *io, int iBody, int iOrbiter,
                UPDATE *update, fnUpdateVariable ***fnUpdate, SYSTEM *system) {
  double dEqRate, dDiff;
  double dOldRotRate, dTmpDeriv;
  int iPert, iEqn, iVar;

  // Compute current equilibrium rotation rate
  dEqRate = fdEqRotRate(body, iBody, body[iOrbiter].dMeanMotion,
                        body[iOrbiter].dEccSq, evolve->iEqtideModel,
                        evolve->bDiscreteRot);

  // Body wasn't tidally locked, but is it now?
  if (!body[iBody].bTideLock) {

    // If Peq(1-eps) < Prot < Peq(1+eps), it's locked!
    dDiff = fabs(body[iBody].dRotRate - dEqRate) / dEqRate;

    // Is RotRate close enough to equilibrium RotRate?
    if (dDiff < evolve->dMaxLockDiff[iBody]) {

      // Only tidally lock if dw/dt points to tidally locked states on both
      // sides of equilibrium rotation rate

      // Consider all variables that could impact rotation rates
      iVar = update[iBody].iRot;

      // Cache current dRotRate
      dOldRotRate = body[iBody].dRotRate;

      // Case 1 w >= w_eq: -> Perturb Prot to w = w * (1 + 2eps)
      body[iBody].dRotRate =
            (1.0 + 2.0 * evolve->dMaxLockDiff[iBody]) * dEqRate;

      // Update PropsAux
      if (evolve->iEqtideModel == CPL) {
        PropsAuxCPL(body, evolve, io, update, iBody);
      } else if (evolve->iEqtideModel == CTL) {
        PropsAuxCTL(body, evolve, io, update, iBody);
      } // DB15 assumes tidal locking

      // Recompute, sum up new derivatives using perturbed dRotRate
      dTmpDeriv = 0.0;
      for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
        update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](
              body, system, update[iBody].iaBody[iVar][iEqn]);
        dTmpDeriv += update[iBody].daDerivProc[iVar][iEqn];
      }

      // Is upper gradient pointing towards tidally locked state?
      if (dTmpDeriv < 0.0) {
        // Case 2 < w_eq: -> Perturb Prot to w = w * (1 - 2eps)
        body[iBody].dRotRate =
              (1.0 - 2.0 * evolve->dMaxLockDiff[iBody]) * dEqRate;

        // Update PropsAux
        if (evolve->iEqtideModel == CPL) {
          PropsAuxCPL(body, evolve, io, update, iBody);
        } else if (evolve->iEqtideModel == CTL) {
          PropsAuxCTL(body, evolve, io, update, iBody);
        } // DB15 assumes tidal locking

        // Recompute, sum up new derivatives using perturbed dRotRate
        dTmpDeriv = 0.0;
        for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
          update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](
                body, system, update[iBody].iaBody[iVar][iEqn]);
          dTmpDeriv += update[iBody].daDerivProc[iVar][iEqn];
        }

        if (dTmpDeriv > 0.0) {
          // Gradient points toward tidally locked state -> Tidally locked!
          body[iBody].bTideLock = 1;
        }
        // Not tidally locked
        else {
          body[iBody].bTideLock = 0;
        }
      }
      // Not tidally locked
      else {
        body[iBody].bTideLock = 0;
      }

      // Reset dRotRate
      body[iBody].dRotRate = dOldRotRate;

      // Update PropsAux
      if (evolve->iEqtideModel == CPL) {
        PropsAuxCPL(body, evolve, io, update, iBody);
      } else if (evolve->iEqtideModel == CTL) {
        PropsAuxCTL(body, evolve, io, update, iBody);
      } // DB15 assumes tidal locking

      // Reset derivatives
      dTmpDeriv = 0.0;
      for (iEqn = 0; iEqn < update[iBody].iNumEqns[iVar]; iEqn++) {
        update[iBody].daDerivProc[iVar][iEqn] = fnUpdate[iBody][iVar][iEqn](
              body, system, update[iBody].iaBody[iVar][iEqn]);
      }
    }
    // Not tidally locked
    else {
      body[iBody].bTideLock = 0;
    }
  }
  // Is tidally locked, but will it unlock?
  else {
    // TODO check to see if it tidally unlocks
  }


  // Ok, so is it tidally locked?
  if (body[iBody].bTideLock) {
    // Save time when body locked
    body[iBody].dLockTime = evolve->dTime;

    if (io->iVerbose >= VERBPROG) {
      printf("%s spin locked at ", body[iBody].cName);
      fprintd(stdout, evolve->dTime / YEARSEC, io->iSciNot, io->iDigits);
      printf(" years.\n");
    }
    return 1; /* Tidally locked */
  } else {
    /* Not tidally locked */
    return 0;
  }
}

/* Auxiliary properties required for the CPL calculations. N.B.: These
   parameters also need to be included in BodyCopyEqtide!!! */
void PropsAuxEqtide(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                    int iBody) {

  if (evolve->iEqtideModel == CPL) {
    PropsAuxCPL(body, evolve, io, update, iBody);
  } else if (evolve->iEqtideModel == CTL) {
    PropsAuxCTL(body, evolve, io, update, iBody);
  } else if (evolve->iEqtideModel == DB15) {
    PropsAuxDB15(body, evolve, io, update, iBody);
  }
}

void PropsAuxOrbiterGeneral(BODY *body, int iBody) {
  body[iBody].dEccSq = body[iBody].dHecc * body[iBody].dHecc +
                       body[iBody].dKecc * body[iBody].dKecc;
  body[iBody].dEcc = sqrt(body[iBody].dEccSq);
  // LongP is needed for Hecc and Kecc calculations
  body[iBody].dLongP = atan2(body[iBody].dHecc, body[iBody].dKecc);
  /*
  printf("%e\n",body[iBody].dHecc);
  printf("%e\n",body[iBody].dKecc);
  fflush(stdout);
  */
}


void PropsAuxOrbiterCPL(BODY *body, UPDATE *update, int iBody) {

  PropsAuxOrbiterGeneral(body, iBody);
  body[iBody].dDeccDtEqtide = fdCPLDeccDt(
        body,
        update[iBody].iaBody[update[iBody].iHecc][update[iBody].iHeccEqtide]);
}

void PropsAuxOrbiterCTL(BODY *body, UPDATE *update, int iBody) {

  PropsAuxOrbiterGeneral(body, iBody);
  body[iBody].dDeccDtEqtide = fdCTLDeccDt(
        body, update,
        update[iBody].iaBody[update[iBody].iHecc][update[iBody].iHeccEqtide]);
}

void PropsAuxOrbiterDB15(BODY *body, UPDATE *update, int iBody) {

  PropsAuxOrbiterGeneral(body, iBody);
  body[iBody].dDeccDtEqtide = fdDB15DeccDt(
        body, update,
        update[iBody].iaBody[update[iBody].iHecc][update[iBody].iHeccEqtide]);
}

void PropsAuxCPL(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                 int iBody) {
  int iPert, iIndex;
  /* dMeanMotion claculated in PropsAuxGeneral */
  int iOrbiter;

  body[iBody].dObliquity =
        atan2(sqrt(pow(body[iBody].dXobl, 2) + pow(body[iBody].dYobl, 2)),
              body[iBody].dZobl);
  body[iBody].dPrecA = atan2(body[iBody].dYobl, body[iBody].dXobl);

  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    iIndex = body[iBody].iaTidePerts[iPert];
    if (bPrimary(body, iIndex)) {
      iOrbiter = iBody;
    } else {
      iOrbiter = iIndex;
    }

    /* If tidally locked, assign equilibrium rotational frequency? */
    if (evolve->bForceEqSpin[iBody]) {
      body[iBody].dRotRate = fdEqRotRate(
            body, iBody, body[iOrbiter].dMeanMotion, body[iOrbiter].dEccSq,
            evolve->iEqtideModel, evolve->bDiscreteRot);
    }

    fiaCPLEpsilon(body[iBody].dRotRate, body[iOrbiter].dMeanMotion,
                  body[iOrbiter].dObliquity, body[iBody].iTidalEpsilon[iIndex]);
    fdCPLZ(body, body[iOrbiter].dMeanMotion, body[iOrbiter].dSemi, iBody,
           iIndex);
    fdaChi(body, body[iOrbiter].dMeanMotion, body[iOrbiter].dSemi, iBody,
           iIndex);

    if (iBody > 0) {
      PropsAuxOrbiterCPL(body, update, iBody);
    }
  }

  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    iIndex                             = body[iBody].iaTidePerts[iPert];
    body[iBody].daDoblDtEqtide[iIndex] = fdCPLDoblDt(
          body, update[iBody].iaBody[update[iBody].iXobl]
                                    [update[iBody].iaXoblEqtide[iPert]]);
  }
}

void PropsAuxCTL(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                 int iBody) {
  int iPert, iIndex;
  /* dMeanMotion claculated in PropsAuxGeneral */
  int iOrbiter;

  body[iBody].dObliquity =
        atan2(sqrt(pow(body[iBody].dXobl, 2) + pow(body[iBody].dYobl, 2)),
              body[iBody].dZobl);
  body[iBody].dPrecA = atan2(body[iBody].dYobl, body[iBody].dXobl);

  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    iIndex = body[iBody].iaTidePerts[iPert];
    if (bPrimary(body, iIndex)) {
      iOrbiter = iBody;
    } else {
      iOrbiter = iIndex;
    }

    /* If tidally locked, assign equilibrium rotational frequency? */
    if (evolve->bForceEqSpin[iBody]) {
      body[iBody].dRotRate = fdEqRotRate(
            body, iBody, body[iOrbiter].dMeanMotion, body[iOrbiter].dEccSq,
            evolve->iEqtideModel, evolve->bDiscreteRot);
    }
  }

  if (iBody == 0) {
    // Central body can be perturbed by multiple bodies

    for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
      iIndex = body[iBody].iaTidePerts[iPert];
      fdaCTLF(body, body[iIndex].dEcc, iBody, iIndex);
      fdaCTLZ(body, body[iIndex].dSemi, iBody, iIndex);
      body[iBody].dTidalBeta[iIndex] = fdCTLBeta(body[iIndex].dEcc);
      fdaChi(body, body[iIndex].dMeanMotion, body[iIndex].dSemi, iBody, iIndex);
    }
  } else { // Orbiter
    fdaCTLF(body, body[iBody].dEcc, iBody, 0);
    fdaCTLZ(body, body[iBody].dSemi, iBody, 0);
    body[iBody].dTidalBeta[0] = fdCTLBeta(body[iBody].dEcc);
    fdaChi(body, body[iBody].dMeanMotion, body[iBody].dSemi, iBody, 0);
    PropsAuxOrbiterCTL(body, update, iBody);
  }

  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    iIndex                             = body[iBody].iaTidePerts[iPert];
    body[iBody].daDoblDtEqtide[iIndex] = fdCTLDoblDt(
          body, update[iBody].iaBody[update[iBody].iXobl]
                                    [update[iBody].iaXoblEqtide[iPert]]);
  }
}

void PropsAuxDB15(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                  int iBody) {

  if (iBody > 0) {
    body[iBody].dTidalPowMan = fdTidePower(body, iBody, evolve->iEqtideModel);
    PropsAuxOrbiterDB15(body, update, iBody);
  }
}

/*! Lost energy due to tidal heating in the CPL model. */
double fdDEdTCPLEqtide(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0];
  double dDEDt;

  dDEDt = fdCPLTidePower(body, iBody);
  return dDEDt;
}


/*! Lost energy due to tidal heating in the CTL model. */
double fdDEdTCTLEqtide(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0];

  return fdCTLTidePower(body, iBody);
}

double fdTidePower(BODY *body, int iBody, int iTideModel) {

  /* Get total tidal power */
  if (iTideModel == CPL) {
    return fdCPLTidePower(body, iBody);
  } else if (iTideModel == CTL) {
    return fdCTLTidePower(body, iBody);
  } else if (iTideModel == DB15) {
    return fdPowerEqtideDB15(body, iBody);
  }

  /* Whoops! */
  assert(0);

  return 0; // To avoid compiler warnings
}

/* This is part of output[OUT_SURFENFLUX].fnOutput */
double fdSurfEnFluxEqtide(BODY *body, SYSTEM *foo, UPDATE *bar, int iBody,
                          int iTideModel) {
  double dPower;

  dPower = fdTidePower(body, iBody, iTideModel);

  // printf("TidePower=%e\n",dPower);

  return dPower / (4 * PI * body[iBody].dRadius * body[iBody].dRadius);
}

/*
 * Alter the simulation in a specific way. Possibilities are
 * stored in the CONTROL struct.
 */

void ForceBehaviorEqtide(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                         SYSTEM *system, UPDATE *update,
                         fnUpdateVariable ***fnUpdate, int iBody, int iModule) {
  int iOrbiter;
  if (body[iBody].iTidePerts == 1) {
    /* Don't check for tidal locking if more than 1 tidal perturber. Maybe
       change this later so the dominant perturber can lock it? */
    if (iBody > 0) {
      iOrbiter = iBody;
    } else {
      iOrbiter = body[iBody].iaTidePerts[0];
    }

    // Is the star tidally locked now?
    /*
    body[iBody].bTideLock = fbTidalLock(body,evolve,io,iBody,iOrbiter,update);
    if(body[iBody].bTideLock) {
      evolve->bForceEqSpin[iBody] = 1;
    }
    else {
      evolve->bForceEqSpin[iBody] = 0;
    }
    */

    /* If tidally locked, assign equilibrium rotational frequency? */
    if (evolve->bForceEqSpin[iBody]) {
      body[iBody].dRotRate = fdEqRotRate(
            body, iBody, body[iOrbiter].dMeanMotion, body[iOrbiter].dEccSq,
            evolve->iEqtideModel, evolve->bDiscreteRot);
    }
    /* Tidally Locked? */
    else {
      // Is the body now tidally locked?
      evolve->bForceEqSpin[iBody] = fbTidalLock(
            body, evolve, io, iBody, iOrbiter, update, fnUpdate, system);
      // If so, reset the function pointer to return dTINY for dDRotRateDt
      /* The index of iaRotEqtide must be zero, as locking is only possible
   if there is one tidal perturber */
      if (evolve->bForceEqSpin[iBody]) {
        SetDerivTiny(fnUpdate, iBody, update[iBody].iRot,
                     update[iBody].iaRotEqtide[0]);
      }
    }
  }

  /* If small enough, set some quantities to zero */
  /* Generalize! fnPropsAuxMinMax? */
  // XXX Move this to ForceBehavior General?
  if (evolve->dMinValue > 0) {
    if (body[iBody].dEcc < evolve->dMinValue) {
      body[iBody].dHecc = 0;
      body[iBody].dKecc = 0;
    }
    /* XXX Need to sort out what Xobl, Yobl and Zobl should be
    if (body[iBody].dObliquity < control->Evolve.dMinValue)
      body[iBody].dObliquity = 0;
    }
    */
  }
}

/**
 * Identify and return the index for the perturbing body.
 * 
 * 
*/

int fiAssignTidalPerturber(BODY *body,int iBody) {
  int iPerturber;

  if (!bPrimary(body, iBody)) {
    iPerturber = iBody;
  } else {
    // Without more info, assume closest body dominates
    // XXX Should either be calculable from these variables or be rewritten
    iPerturber = 0;
  }
  return iPerturber;
}

/* Non-central body contains the orbital parameters */

int fiAssignTidalOrbiter(BODY *body,int iBody) {
  int iOrbiter;

  if (!bPrimary(body, iBody)) {
    iOrbiter = iBody;
  } else {
    // Without more info, assume closest body dominates
    // XXX Should either be calculable from these variables or be rewritten
    iOrbiter = 1;
  }
  return iOrbiter;
}

/*
 ************************ CPL Functions ******************
 */

/* Auxiliary Parameters */

/* This is a member of output->fnSurfEnFlux, so must have appropriate
   input parameters.*/

double fdCPLTidePower(BODY *body, int iBody) {
  /* Heller, Leconte & Barnes (2011), Eqn 9-11. */
  int iPert, iOrbiter, iIndex;
  double dOrbPow = 0, dRotPow = 0;

  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    if (iBody == 0) {
      iOrbiter = body[iBody].iaTidePerts[iPert];
    } else {
      iOrbiter = iBody;
    }
    // iIndex should be removed by optimization
    iIndex = body[iBody].iaTidePerts[iPert];

    // Does this work with DF's changes to da/dt with the synchronous case?
    // See Fleming et al., 2018
    //fprintf(stderr,"\niBody: %d\n",iBody);
    //fprintf(stderr,"TidalZ[%d]: %lf\n",iIndex,body[iBody].dTidalZ[iIndex]);

    dOrbPow += -body[iBody].dTidalZ[iIndex] / 8 *
               (4 * body[iBody].iTidalEpsilon[iIndex][0] +
                body[iOrbiter].dEccSq *
                      (-20 * body[iBody].iTidalEpsilon[iIndex][0] +
                       147. / 2 * body[iBody].iTidalEpsilon[iIndex][1] +
                       0.5 * body[iBody].iTidalEpsilon[iIndex][2] -
                       3 * body[iBody].iTidalEpsilon[iIndex][5]) -
                4 * sin(body[iBody].dObliquity) * sin(body[iBody].dObliquity) *
                      (body[iBody].iTidalEpsilon[iIndex][0] -
                       body[iBody].iTidalEpsilon[iIndex][8]));
    dRotPow +=
          body[iBody].dTidalZ[iIndex] * body[iBody].dRotRate /
          (8 * body[iOrbiter].dMeanMotion) *
          (4 * body[iBody].iTidalEpsilon[iIndex][0] +
           body[iOrbiter].dEccSq * (-20 * body[iBody].iTidalEpsilon[iIndex][0] +
                                    49 * body[iBody].iTidalEpsilon[iIndex][1] +
                                    body[iBody].iTidalEpsilon[iIndex][2]) +
           2 * sin(body[iBody].dObliquity) * sin(body[iBody].dObliquity) *
                 (-2 * body[iBody].iTidalEpsilon[iIndex][0] +
                  body[iBody].iTidalEpsilon[iIndex][8] +
                  body[iBody].iTidalEpsilon[iIndex][9]));
  }

  return dOrbPow + dRotPow;
}

/* Tidal Power due to Ocean Tides */
double fdTidePowerOcean(BODY *body, int iBody) {
  // Broken

  // Total CPL Tide Power = Ocean + Man contributions
  // return fdCPLTidePower(body,iBody) - fdTidePower(body,iBody);
  // fprintf(stderr,"ERROR: fdTidePowerOcean called, but it's broken.");
  return -1;
}

/* Surface Energy Flux due to Ocean Tides */
double fdSurfEnFluxOcean(BODY *body, int iBody) {
  // Broken!

  // Total Ocean Tide power / surface area of body
  // return
  // fdTidePowerOcean(body,iBody)/(4.0*PI*body[iBody].dRadius*body[iBody].dRadius);
  return -1;
}

double fdGammaRot(double dEccSq, double dPsi, int *epsilon) {
  return 4 * epsilon[0] +
         dEccSq * (-20 * epsilon[0] + 49 * epsilon[1] + epsilon[2]) +
         2 * sin(dPsi) * sin(dPsi) *
               (-2 * epsilon[0] + epsilon[8] + epsilon[9]);
}

double fdGammaOrb(double dEccSq, double dPsi, int *epsilon) {
  return 4 * epsilon[0] +
         dEccSq * (-20 * epsilon[0] + 147. / 2 * epsilon[1] + 0.5 * epsilon[2] -
                   3 * epsilon[5]) -
         4 * sin(dPsi) * sin(dPsi) * (epsilon[0] - epsilon[8]);
}

void fiaCPLEpsilon(double dRotRate, double dMeanMotion, double dObliquity,
                   int *iEpsilon) {
  // Note: fiSign reurns 0 if the argument is < EPS, see vplanet.h

  /* This needs to be added back in with the retrograde branch! XXX
  if (dObliquity > PI/2) {
    dRotRate = -dRotRate;
  } else if (bFloatComparison(dObliquity,PI/2)) {
    dRotRate = 0;
  }
  */

  iEpsilon[0] = fiSign(2 * dRotRate - 2 * dMeanMotion);
  iEpsilon[1] = fiSign(2 * dRotRate - 3 * dMeanMotion);
  iEpsilon[2] = fiSign(2 * dRotRate - dMeanMotion);
  iEpsilon[5] = fiSign(dMeanMotion);
  iEpsilon[8] = fiSign(dRotRate - 2 * dMeanMotion);
  iEpsilon[9] = fiSign(dRotRate);
}

void fdCPLZ(BODY *body, double dMeanMotion, double dSemi, int iBody,
            int iPert) {

  /* Note that this is different from Heller et al (2011) because
     we now use Im(k_2) which equals -k_2/Q. The value of Im(k_2) is set in
     VerifyEqtideThermint in module.c
  */
  body[iBody].dTidalZ[iPert] =
        -3. * body[iBody].dImK2 * BIGG * BIGG * body[iPert].dMass *
        body[iPert].dMass * (body[iBody].dMass + body[iPert].dMass) *
        pow(body[iBody].dTidalRadius, 5) / (pow(dSemi, 9) * dMeanMotion);
}

/*
 * Equilibrium Parameters
 */

double fdCPLTidePowerEq(double dTidalZ, double dEccSq, double dMeanMotion,
                        double dObliquity, int bDiscrete) {
  /* Heller, Leconte  Barnes 2011, Eq. 13 */
  double dGammaOrb, dGammaRot, dRotRateEq;
  int *iEpsilon;

  // XXX Yikes! This shouldn't be getting mallocked every output!
  iEpsilon = malloc(10 * sizeof(int));

  /* Must reset spin rate in order to get the phase lags correctly */

  dRotRateEq = fdCPLEqRotRate(dMeanMotion, dEccSq, bDiscrete);

  fiaCPLEpsilon(dRotRateEq, dMeanMotion, dObliquity, iEpsilon);

  dGammaRot = fdGammaRot(dEccSq, dObliquity, iEpsilon);
  dGammaOrb = fdGammaOrb(dEccSq, dObliquity, iEpsilon);

  free(iEpsilon);
  return dTidalZ / 8 * ((1 + 9.5 * dEccSq) * dGammaRot - dGammaOrb);
}

double fdCPLEqRotRateCont(double dMeanMotion, double dEccSq) {
  return (1 + 9.5 * dEccSq) * dMeanMotion;
}

double fdCPLEqRotRateDiscrete(double dMeanMotion, double dEccSq) {
  if (dEccSq <= (1. / 19)) {
    return dMeanMotion;
  } else {
    return 1.5 * dMeanMotion;
  }
}

double fdCPLEqRotRate(double dEccSq, double dMeanMotion, int bDiscrete) {

  if (bDiscrete) {
    return fdCPLEqRotRateDiscrete(dMeanMotion, dEccSq);
  } else {
    return fdCPLEqRotRateCont(dMeanMotion, dEccSq);
  }
}

/*
 * Derivatives
 */


/*! Equations governing semi-major axis derivative from Ferraz-Mello 2008
 * Combines tidally-locked and not-tidally locked equations
 */
double fdCPLDsemiDt(BODY *body, SYSTEM *system, int *iaBody) {
  int iB0 = iaBody[0], iB1 = iaBody[1];

  /* This routine should only be called for the orbiters. iaBody[0] = the
orbiter, iaBody[0] = central body */

  double dSum;

  // Primary (central body) is tidally locked
  if (!body[iB0].bTideLock && body[iB1].bTideLock) {
    dSum = 0.0;

    // Contribution from orbiter
    dSum += body[iB0].dSemi * body[iB0].dSemi /
            (4 * BIGG * body[iB0].dMass * body[iB1].dMass) *
            body[iB0].dTidalZ[iB1] *
            (4 * body[iB0].iTidalEpsilon[iB1][0] +
             body[iB0].dEccSq * (-20 * body[iB0].iTidalEpsilon[iB1][0] +
                                 147. / 2 * body[iB0].iTidalEpsilon[iB1][1] +
                                 0.5 * body[iB0].iTidalEpsilon[iB1][2] -
                                 3 * body[iB0].iTidalEpsilon[iB1][5]) -
             4 * sin(body[iB0].dObliquity) * sin(body[iB0].dObliquity) *
                   (body[iB0].iTidalEpsilon[iB1][0] -
                    body[iB0].iTidalEpsilon[iB1][8]));

    // Contribution from the central body
    dSum += -body[iB0].dSemi * body[iB0].dSemi /
            (BIGG * body[iB0].dMass * body[iB1].dMass) *
            body[iB1].dTidalZ[iB0] *
            (7.0 * body[iB0].dEccSq +
             sin(body[iB1].dObliquity) * sin(body[iB1].dObliquity)) *
            body[iB1].iTidalEpsilon[iB0][2];

    return dSum;
  }
  // Secondary (orbiter) is tidally locked
  else if (body[iB0].bTideLock && !body[iB1].bTideLock) {
    dSum = 0.0;

    // Contribution from orbiter
    dSum += -body[iB0].dSemi * body[iB0].dSemi /
            (BIGG * body[iB0].dMass * body[iB1].dMass) *
            body[iB0].dTidalZ[iB1] *
            (7.0 * body[iB0].dEccSq +
             sin(body[iB0].dObliquity) * sin(body[iB0].dObliquity)) *
            body[iB0].iTidalEpsilon[iB1][2];

    // Contribution from central body
    dSum += body[iB0].dSemi * body[iB0].dSemi /
            (4 * BIGG * body[iB0].dMass * body[iB1].dMass) *
            body[iB1].dTidalZ[iB0] *
            (4 * body[iB1].iTidalEpsilon[iB0][0] +
             body[iB0].dEccSq * (-20 * body[iB1].iTidalEpsilon[iB0][0] +
                                 147. / 2 * body[iB1].iTidalEpsilon[iB0][1] +
                                 0.5 * body[iB1].iTidalEpsilon[iB0][2] -
                                 3 * body[iB1].iTidalEpsilon[iB0][5]) -
             4 * sin(body[iB1].dObliquity) * sin(body[iB1].dObliquity) *
                   (body[iB1].iTidalEpsilon[iB0][0] -
                    body[iB1].iTidalEpsilon[iB0][8]));

    return dSum;
  }
  // If primary and secondary are tidally locked
  else if (body[iB0].bTideLock && body[iB1].bTideLock) {
    dSum = 0.0;

    // Contribution from orbiter
    dSum += body[iB0].dTidalZ[iB1] *
            (7.0 * body[iB0].dEccSq +
             sin(body[iB0].dObliquity) * sin(body[iB0].dObliquity)) *
            body[iB0].iTidalEpsilon[iB1][2];

    // Contribution from central body
    dSum += body[iB1].dTidalZ[iB0] *
            (7.0 * body[iB0].dEccSq +
             sin(body[iB1].dObliquity) * sin(body[iB1].dObliquity)) *
            body[iB1].iTidalEpsilon[iB0][2];

    return -body[iB0].dSemi * body[iB0].dSemi /
           (BIGG * body[iB0].dMass * body[iB1].dMass) * dSum;
  }
  // Neither body is tidally locked
  else {
    dSum = 0.0;

    // Contribution from orbiter
    dSum += body[iB0].dTidalZ[iB1] *
            (4 * body[iB0].iTidalEpsilon[iB1][0] +
             body[iB0].dEccSq * (-20 * body[iB0].iTidalEpsilon[iB1][0] +
                                 147. / 2 * body[iB0].iTidalEpsilon[iB1][1] +
                                 0.5 * body[iB0].iTidalEpsilon[iB1][2] -
                                 3 * body[iB0].iTidalEpsilon[iB1][5]) -
             4 * sin(body[iB0].dObliquity) * sin(body[iB0].dObliquity) *
                   (body[iB0].iTidalEpsilon[iB1][0] -
                    body[iB0].iTidalEpsilon[iB1][8]));

    // Contribution from central body
    dSum += body[iB1].dTidalZ[iB0] *
            (4 * body[iB1].iTidalEpsilon[iB0][0] +
             body[iB0].dEccSq * (-20 * body[iB1].iTidalEpsilon[iB0][0] +
                                 147. / 2 * body[iB1].iTidalEpsilon[iB0][1] +
                                 0.5 * body[iB1].iTidalEpsilon[iB0][2] -
                                 3 * body[iB1].iTidalEpsilon[iB0][5]) -
             4 * sin(body[iB1].dObliquity) * sin(body[iB1].dObliquity) *
                   (body[iB1].iTidalEpsilon[iB0][0] -
                    body[iB1].iTidalEpsilon[iB0][8]));

    return body[iB0].dSemi * body[iB0].dSemi /
           (4 * BIGG * body[iB0].dMass * body[iB1].dMass) * dSum;
  }
}

/* Hecc and Kecc calculated by chain rule, e.g. dh/dt = de/dt * dh/de. */

double fdCPLDeccDt(BODY *body, int *iaBody) {
  double dSum;
  int iB0 = iaBody[0], iB1 = iaBody[1];

  // Contribution from Central Body
  dSum = body[iB1].dTidalZ[iB0] * (2 * body[iB1].iTidalEpsilon[iB0][0] -
                                   49. / 2 * body[iB1].iTidalEpsilon[iB0][1] +
                                   0.5 * body[iB1].iTidalEpsilon[iB0][2] +
                                   3 * body[iB1].iTidalEpsilon[iB0][5]);

  // Contribution from Orbiter
  dSum += body[iB0].dTidalZ[iB1] * (2 * body[iB0].iTidalEpsilon[iB1][0] -
                                    49. / 2 * body[iB0].iTidalEpsilon[iB1][1] +
                                    0.5 * body[iB0].iTidalEpsilon[iB1][2] +
                                    3 * body[iB0].iTidalEpsilon[iB1][5]);

  return -dSum * body[iB0].dSemi * body[iB0].dEcc /
         (8 * BIGG * body[iB0].dMass * body[iB1].dMass);
}

double fdCPLDHeccDt(BODY *body, SYSTEM *system, int *iaBody) {
  /* This routine should only be called for the orbiters.
     iaBody[0] = the orbiter, iaBody[1] = central body */

  return body[iaBody[0]].dDeccDtEqtide * sin(body[iaBody[0]].dLongP);
}

double fdCPLDKeccDt(BODY *body, SYSTEM *system, int *iaBody) {
  /* This routine should only be called for the orbiters.
     iaBody[0] = the orbiter, iaBody[1] = central body */

  return body[iaBody[0]].dDeccDtEqtide * cos(body[iaBody[0]].dLongP);
}

double fdCPLDrotrateDt(BODY *body, SYSTEM *system, int *iaBody) {
  int iB0 = iaBody[0], iB1 = iaBody[1];

  /* Don't know if this is the central body or orbiter, but orbital
     info stored in body[iOrbter], so must figure this out.
     Is there a faster way to do this? Note that forcing iaBody[0]
     to always be central body in VerifyCPL makes the semi and ecc
     derivatives need this determination. */
  int iOrbiter;

  if (bPrimary(body, iB0)) {
    iOrbiter = iB1;
  } else {
    iOrbiter = iB0;
  }

  /* Note if tidally locked, ForceBehavior fixes the rotation
     rate and sets the function pointer to dTINY. */

  return -body[iB0].dTidalZ[iB1] /
         (8 * body[iB0].dMass * body[iB0].dRadGyra * body[iB0].dRadGyra *
          body[iB0].dTidalRadius * body[iB0].dTidalRadius *
          body[iOrbiter].dMeanMotion) *
         (4 * body[iB0].iTidalEpsilon[iB1][0] +
          body[iOrbiter].dEccSq * (-20 * body[iB0].iTidalEpsilon[iB1][0] +
                                   49 * body[iB0].iTidalEpsilon[iB1][1] +
                                   body[iB0].iTidalEpsilon[iB1][2]) +
          2 * sin(body[iB0].dObliquity) * sin(body[iB0].dObliquity) *
                (-2 * body[iB0].iTidalEpsilon[iB1][0] +
                 body[iB0].iTidalEpsilon[iB1][8] +
                 body[iB0].iTidalEpsilon[iB1][9]));
}

double fdCPLDoblDt(BODY *body, int *iaBody) {
  int iOrbiter, iB0 = iaBody[0], iB1 = iaBody[1];
  double foo;

  if (bPrimary(body, iB0)) {
    iOrbiter = iB1;
  } else {
    iOrbiter = iB0;
  }

  return body[iB0].dTidalZ[iB1] * sin(body[iB0].dObliquity) /
         (4 * body[iB0].dMass * body[iB0].dRadGyra * body[iB0].dRadGyra *
          body[iB0].dTidalRadius * body[iB0].dTidalRadius *
          body[iOrbiter].dMeanMotion * body[iB0].dRotRate) *
         (body[iB0].iTidalEpsilon[iB1][0] * (1 - body[iB0].dTidalChi[iB1]) +
          (body[iB0].iTidalEpsilon[iB1][8] - body[iB0].iTidalEpsilon[iB1][9]) *
                (1 + body[iB0].dTidalChi[iB1]));
}

double fdCPLDXoblDt(BODY *body, SYSTEM *system, int *iaBody) {

  return body[iaBody[0]].daDoblDtEqtide[iaBody[1]] *
         cos(body[iaBody[0]].dObliquity) * cos(body[iaBody[0]].dPrecA);
}

double fdCPLDYoblDt(BODY *body, SYSTEM *system, int *iaBody) {

  return body[iaBody[0]].daDoblDtEqtide[iaBody[1]] *
         cos(body[iaBody[0]].dObliquity) * sin(body[iaBody[0]].dPrecA);
}

double fdCPLDZoblDt(BODY *body, SYSTEM *system, int *iaBody) {

  return -body[iaBody[0]].daDoblDtEqtide[iaBody[1]] *
         sin(body[iaBody[0]].dObliquity);
}

/*
 ********************** CTL Functions ************************
 */

double fdCTLBeta(double dEcc) {
  return pow(1 - dEcc * dEcc, 0.5);
}

double fdCTLF1(double dEcc) {
  return (1 + 31. / 2 * dEcc * dEcc + 255. / 8 * pow(dEcc, 4) +
          185. / 16 * pow(dEcc, 6) + 25. / 64 * pow(dEcc, 8));
}

double fdCTLF2(double dEcc) {
  return (1 + 15. / 2 * dEcc * dEcc + 45. / 8 * pow(dEcc, 4) +
          5. / 16 * pow(dEcc, 6));
}

double fdCTLF3(double dEcc) {
  return (1 + 15. / 4 * dEcc * dEcc + 15. / 8 * pow(dEcc, 4) +
          5. / 64 * pow(dEcc, 6));
}

double fdCTLF4(double dEcc) {
  return (1 + 1.5 * dEcc * dEcc + 1. / 8 * pow(dEcc, 4));
}

double fdCTLF5(double dEcc) {
  return (1 + 3 * dEcc * dEcc + 3. / 8 * pow(dEcc, 4));
}

void fdaCTLF(BODY *body, double dEcc, int iBody, int iPert) {

  body[iBody].dTidalF[iPert][0] = fdCTLF1(dEcc);
  body[iBody].dTidalF[iPert][1] = fdCTLF2(dEcc);
  body[iBody].dTidalF[iPert][2] = fdCTLF3(dEcc);
  body[iBody].dTidalF[iPert][3] = fdCTLF4(dEcc);
  body[iBody].dTidalF[iPert][4] = fdCTLF5(dEcc);
}

double fdCTLTidePower(BODY *body, int iBody) {
  int iPert, iOrbiter, iIndex;
  double dPower = 0;

  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {
    if (iBody == 0) {
      iOrbiter = body[iBody].iaTidePerts[iPert];
    } else {
      iOrbiter = iBody;
    }
    iIndex = body[iBody].iaTidePerts[iPert];

    dPower += (body[iBody].dTidalF[iIndex][0] /
                     pow(body[iBody].dTidalBeta[iIndex], 15) -
               2 * body[iBody].dTidalF[iIndex][1] *
                     cos(body[iBody].dObliquity) * body[iBody].dRotRate /
                     (pow(body[iBody].dTidalBeta[iIndex], 12) *
                      body[iOrbiter].dMeanMotion));
    dPower += ((1 + cos(body[iBody].dObliquity) * cos(body[iBody].dObliquity)) /
               2) *
              body[iBody].dTidalF[iIndex][4] * body[iBody].dRotRate *
              body[iBody].dRotRate /
              (pow(body[iBody].dTidalBeta[iIndex], 9) *
               body[iOrbiter].dMeanMotion * body[iOrbiter].dMeanMotion);
    dPower *= body[iBody].dTidalZ[iIndex];
  }

  return dPower;
}

/*
 * Equilibrium Parameters
 */

double fdCTLTidePowerEq(BODY body, double dEcc) {

  return body.dTidalZ[0] / pow(body.dTidalBeta[0], 15) *
         (body.dTidalF[0][0] -
          body.dTidalF[0][1] * body.dTidalF[0][1] / body.dTidalF[0][4] *
                (2 * cos(body.dObliquity) * cos(body.dObliquity)) /
                (1 + cos(body.dObliquity) * cos(body.dObliquity)));
}

double fdCTLEqRotRate(double dEccSq, double dObliquity, double dMeanMotion) {
  double dBeta, f2, f5;

  // This model likes e, but CPL/other equations take e^2, so sqrt here
  double dEcc = sqrt(dEccSq);

  dBeta = fdCTLBeta(dEcc);
  f2    = fdCTLF2(dEcc);
  f5    = fdCTLF5(dEcc);

  return f2 / (pow(dBeta, 3) * f5) * 2 * cos(dObliquity) /
         (1 + cos(dObliquity) * cos(dObliquity)) * dMeanMotion;
}

void fdaCTLZ(BODY *body, double dSemi, int iBody, int iPert) {
  body[iBody].dTidalZ[iPert] = 3 * BIGG * BIGG * body[iBody].dK2 *
                               body[iPert].dMass * body[iPert].dMass *
                               (body[iBody].dMass + body[iPert].dMass) *
                               pow(body[iBody].dTidalRadius, 5) *
                               body[iBody].dTidalTau / pow(dSemi, 9);
}

/*
 * Derivatives
 */

double fdCTLDXoblDt(BODY *body, SYSTEM *system, int *iaBody) {

  return body[iaBody[0]].daDoblDtEqtide[iaBody[1]] *
         cos(body[iaBody[0]].dObliquity) * cos(body[iaBody[0]].dPrecA);
}

double fdCTLDYoblDt(BODY *body, SYSTEM *system, int *iaBody) {

  return body[iaBody[0]].daDoblDtEqtide[iaBody[1]] *
         cos(body[iaBody[0]].dObliquity) * sin(body[iaBody[0]].dPrecA);
}

double fdCTLDZoblDt(BODY *body, SYSTEM *system, int *iaBody) {

  return -body[iaBody[0]].daDoblDtEqtide[iaBody[1]] *
         sin(body[iaBody[0]].dObliquity);
}

double fdCTLDHeccDt(BODY *body, SYSTEM *system, int *iaBody) {
  /* This routine should only be called for the orbiters.
  iaBody[0] = the orbiter, iaBody[1] = central body */

  return body[iaBody[0]].dDeccDtEqtide * sin(body[iaBody[0]].dLongP);
}

double fdCTLDKeccDt(BODY *body, SYSTEM *system, int *iaBody) {
  /* This routine should only be called for the orbiters.
    iaBody[0] = the orbiter, iaBody[1] = central body */

  return body[iaBody[0]].dDeccDtEqtide * cos(body[iaBody[0]].dLongP);
}

double fdCTLDsemiDt(BODY *body, SYSTEM *system, int *iaBody) {
  /* This routine should only be called for the orbiters. iaBody[0] = the
   * orbiter, iaBody[1] = central body */
  double dSum = 0;

  // Contribution from orbiter
  dSum += body[iaBody[0]].dTidalZ[iaBody[1]] *
          (cos(body[iaBody[0]].dObliquity) *
                 body[iaBody[0]].dTidalF[iaBody[1]][1] *
                 body[iaBody[0]].dRotRate /
                 (pow(body[iaBody[0]].dTidalBeta[iaBody[1]], 12) *
                  body[iaBody[0]].dMeanMotion) -
           body[iaBody[0]].dTidalF[iaBody[1]][0] /
                 pow(body[iaBody[0]].dTidalBeta[iaBody[1]], 15));

  // Contribution from central body
  dSum += body[iaBody[1]].dTidalZ[iaBody[0]] *
          (cos(body[iaBody[1]].dObliquity) *
                 body[iaBody[0]].dTidalF[iaBody[1]][1] *
                 body[iaBody[1]].dRotRate /
                 (pow(body[iaBody[0]].dTidalBeta[iaBody[1]], 12) *
                  body[iaBody[0]].dMeanMotion) -
           body[iaBody[0]].dTidalF[iaBody[1]][0] /
                 pow(body[iaBody[0]].dTidalBeta[iaBody[1]], 15));


  return 2 * body[iaBody[0]].dSemi * body[iaBody[0]].dSemi /
         (BIGG * body[iaBody[0]].dMass * body[iaBody[1]].dMass) * dSum;
}

double fdCTLDeccDt(BODY *body, UPDATE *update, int *iaBody) {
  /* This routine should only be called for the orbiters.
     iaBody[0] = the orbiter, iaBody[1] = central body */
  double dSum = 0;

  // Contribution from orbiter
  dSum += body[iaBody[0]].dTidalZ[iaBody[1]] *
          (cos(body[iaBody[0]].dObliquity) *
                 body[iaBody[0]].dTidalF[iaBody[1]][3] *
                 body[iaBody[0]].dRotRate /
                 (pow(body[iaBody[0]].dTidalBeta[iaBody[1]], 10) *
                  body[iaBody[0]].dMeanMotion) -
           18 * body[iaBody[0]].dTidalF[iaBody[1]][2] /
                 (11 * pow(body[iaBody[0]].dTidalBeta[iaBody[1]], 13)));

  // Contribution from central body
  dSum += body[iaBody[1]].dTidalZ[iaBody[0]] *
          (cos(body[iaBody[1]].dObliquity) *
                 body[iaBody[0]].dTidalF[iaBody[1]][3] *
                 body[iaBody[1]].dRotRate /
                 (pow(body[iaBody[0]].dTidalBeta[iaBody[1]], 10) *
                  body[iaBody[0]].dMeanMotion) -
           18 * body[iaBody[0]].dTidalF[iaBody[1]][2] /
                 (11 * pow(body[iaBody[0]].dTidalBeta[iaBody[1]], 13)));

  return 11 * body[iaBody[0]].dSemi * body[iaBody[0]].dEcc /
         (2 * BIGG * body[iaBody[0]].dMass * body[iaBody[1]].dMass) * dSum;
}

double fdCTLDrotrateDt(BODY *body, SYSTEM *system, int *iaBody) {
  /* Note if tidally locked, ForceBehavior will fix the rotation
     rate and override this derivative. */
  int iB0 = iaBody[0], iB1 = iaBody[1];

  /* Don't know if this is the central body or orbiter, but orbital
        info stored in body[iOribter], so must figure this out. */
  int iOrbiter;

  if (bPrimary(body, iB0)) {
    iOrbiter = iB1;
  } else {
    iOrbiter = iB0;
  }

  double tmp = 2 * cos(body[iaBody[0]].dObliquity) * body[iB0].dTidalF[iB1][1] /
               pow(body[iB0].dTidalBeta[iB1], 12);
  tmp -= (1 +
          cos(body[iaBody[0]].dObliquity) * cos(body[iaBody[0]].dObliquity)) *
         body[iB0].dTidalF[iB1][4] * body[iaBody[0]].dRotRate /
         (pow(body[iB0].dTidalBeta[iB1], 9) * body[iOrbiter].dMeanMotion);
  tmp *= body[iB0].dTidalZ[iB1] /
         (2 * body[iaBody[0]].dMass * body[iaBody[0]].dRadGyra *
          body[iaBody[0]].dRadGyra * body[iaBody[0]].dTidalRadius *
          body[iaBody[0]].dTidalRadius * body[iOrbiter].dMeanMotion);
  return tmp;
}

double fdCTLDoblDt(BODY *body, int *iaBody) {
  /* Note if tidally locked, ForceBehavior will fix the rotation
     rate and override this derivative. */
  int iB0 = iaBody[0], iB1 = iaBody[1];

  /* Don't know if this is the central body or orbiter, but orbital
        info stored in body[iOribter], so must figure this out. */
  int iOrbiter;

  if (bPrimary(body, iB0)) {
    iOrbiter = iB1;
  } else {
    iOrbiter = iB0;
  }

  double tmp = (body[iB0].dTidalZ[iB1] * sin(body[iaBody[0]].dObliquity)) /
               (2 * body[iaBody[0]].dMass * body[iaBody[0]].dRadGyra *
                body[iaBody[0]].dRadGyra * body[iaBody[0]].dTidalRadius *
                body[iaBody[0]].dTidalRadius * body[iOrbiter].dMeanMotion *
                body[iaBody[0]].dRotRate);
  tmp *= ((cos(body[iaBody[0]].dObliquity) -
           body[iB0].dTidalChi[iB1] / body[iB0].dTidalBeta[iB1]) *
                body[iB0].dTidalF[iB1][4] * body[iaBody[0]].dRotRate /
                (pow(body[iB0].dTidalBeta[iB1], 9) *
                 body[iOrbiter].dMeanMotion) -
          2 * body[iB0].dTidalF[iB1][1] / pow(body[iB0].dTidalBeta[iB1], 12));
  return tmp;
}

/*
 ********************** DB15 Functions ************************
 */

double fdPowerEqtideDB15(BODY *body, int iBody) {

  // Power is only dissipated in planets
  if (iBody > 0) {
    //    return -10.5*body[iBody].dImK2*pow(BIGG,1.5)*pow(body[0].dMass,2.5)*
    //      pow(body[iBody].dRadius,5)*body[iBody].dEccSq*pow(body[iBody].dSemi,-7.5);
    double dPower;
    dPower = -10.5 * body[iBody].dImK2 * pow(BIGG, 1.5) *
             pow(body[0].dMass, 2.5) * pow(body[iBody].dRadius, 5) *
             body[iBody].dEccSq * pow(body[iBody].dSemi, -7.5);

    // printf("PowerEqtideDB15=%e\n",dPower);
    return dPower;
  } else {
    return 0;
  }
}

double fdDB15DsemiDt(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0];
  int iPert = iaBody[1];
  // Only the planet contributes in the DB15 model
  if (iBody > 0) { // Also means iBody is the orbiter
    double imk2;
    /* Set imk2 using appropriate model */
    if (body[iBody].dImK2ManOrbModel == 1) {
      imk2 = body[iBody].dImK2Man;
    }
    if (body[iBody].dImK2ManOrbModel == 2) {
      imk2 = -body[iBody].dK2Man / body[iBody].dTidalQMan;
    }
    return 21 * imk2 * body[iBody].dSemi * body[iPert].dMass /
           body[iBody].dMass *
           pow((body[iBody].dRadius / body[iBody].dSemi), 5) *
           body[iBody].dMeanMotion * body[iBody].dEccSq;
  } else {
    return 0;
  }
}

double fdDB15DeccDt(BODY *body, UPDATE *update, int *iaBody) {
  int iBody = iaBody[0];
  int iPert = iaBody[1];
  // Only the planet contributes in the DB15 model
  if (iBody > 0) {
    double imk2;
    /* Set imk2 using appropriate model */
    if (body[iBody].dImK2ManOrbModel == 1) {
      imk2 = body[iBody].dImK2Man;
    }
    if (body[iBody].dImK2ManOrbModel == 2) {
      imk2 = -body[iBody].dK2Man / body[iBody].dTidalQMan;
    }
    return 21 / 2. * imk2 * pow(body[iPert].dMass, 3. / 2) * pow(BIGG, 1. / 2) *
           pow(body[iBody].dRadius, 5) * body[iBody].dEcc / body[iBody].dMass /
           pow(body[iBody].dSemi, 13. / 2); // DB15 (16)
  } else {
    return 0;
  }
}

/* Hecc and Kecc calculated by chain rule, e.g. dh/dt = de/dt * dh/de. */

double fdDB15DHeccDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dDeccDtEqtide * sin(body[iaBody[0]].dLongP);
}

double fdDB15DKeccDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dDeccDtEqtide * cos(body[iaBody[0]].dLongP);
}

void VerifyDB15(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                OUTPUT *output, UPDATE *update, int iBody, int iModule) {
  int iPert, iIndex;

  // k_2 and (Q || tau) must be set
  if (options[OPT_K2].iLine[iBody + 1] == -1) {
    if (control->Io.iVerbose >= VERBINPUT) {
      fprintf(stderr,
              "ERROR: Module ThermInt *not* selected for %s, but the tidal "
              "model is DB15. ",
              body[iBody].cName);
      fprintf(stderr, "Thefore %s must be set.\n", options[OPT_K2].cName);
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
  }
  if (options[OPT_TIDALQ].iLine[iBody + 1] == -1) {
    if (control->Io.iVerbose >= VERBINPUT) {
      fprintf(stderr,
              "ERROR: Module ThermInt *not* selected for %s, but the tidal "
              "model is DB15. ",
              body[iBody].cName);
      fprintf(stderr, "Thefore %s must be set.\n", options[OPT_TIDALQ].cName);
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_MODULES].iLine[iBody + 1]);
    }
  }


  /* Everything OK, assign Updates */

  for (iPert = 0; iPert < body[iBody].iTidePerts; iPert++) {

    /* Obliquity */
    // Xobl
    InitializeXoblEqtide(body, update, iBody, iPert);
    // Yobl
    InitializeYoblEqtide(body, update, iBody, iPert);
    // Zobl
    InitializeZoblEqtide(body, update, iBody, iPert);
    /* Rotation Rate */
    InitializeRotEqtide(body, update, iBody, iPert);

    /* Although obliquity is not in the orbit-only model, initialize it to avoid
      any memory issues. */
    iIndex                             = body[iBody].iaTidePerts[iPert];
    body[iBody].daDoblDtEqtide[iIndex] = dTINY;
  }

  /* Is this the secondary body, and hence we assign da/dt and de/dt? */
  if (!bPrimary(body, iBody)) {
    // Initialize Orbital variable for the matrix
    InitializeSemiEqtide(body, update, iBody);
    InitializeHeccEqtide(body, update, iBody);
    InitializeKeccEqtide(body, update, iBody);
  }

  control->fnPropsAux[iBody][iModule] = &PropsAuxDB15;

  /* Note that the mantle k_2 and Im(k_2) are calculated in
    VerifyEqtideThermint. This choice is made because they depend on mantle
    properties that are calculated by Thermint, and all individual modules are
    verified before multi-modules are. */
}
