/**
   @file poise.c

   @brief Subroutines that control the energy balance model for climate

   @author Russell Deitrick ([deitrr](https://github.com/deitrr/))

   @date Sep 10 2015

*/

#include "vplanet.h"

void BodyCopyPoise(BODY *dest, BODY *src, int iTideModel, int iNumBodies,
                   int iBody) {
  dest[iBody].bReadOrbitOblData = src[iBody].bReadOrbitOblData;
}

void InitializeUpdateTmpBodyPoise(BODY *body, CONTROL *control, UPDATE *update,
                                  int iBody) {
  if (body[iBody].bReadOrbitOblData) {
    int iLine;

    control->Evolve.tmpBody[iBody].daSemiSeries =
          malloc(body[iBody].iNLines * sizeof(double));
    control->Evolve.tmpBody[iBody].daHeccSeries =
          malloc(body[iBody].iNLines * sizeof(double));
    control->Evolve.tmpBody[iBody].daKeccSeries =
          malloc(body[iBody].iNLines * sizeof(double));


    for (iLine = 0; iLine < body[iBody].iNLines; iLine++) {
      control->Evolve.tmpBody[iBody].daSemiSeries[iLine] =
            body[iBody].daSemiSeries[iLine];
      control->Evolve.tmpBody[iBody].daHeccSeries[iLine] =
            body[iBody].daHeccSeries[iLine];
      control->Evolve.tmpBody[iBody].daKeccSeries[iLine] =
            body[iBody].daKeccSeries[iLine];
    }
  }
}

/**************** POISE options ********************/

void ReadLatCellNum(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, iTmp;
  AddOptionInt(files->Infile[iFile].cIn, options->cName, &iTmp, &lTmp,
               control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].iNumLats = iTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].iNumLats, files->iNumInputs);
  }
}

void ReadPlanckA(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dPlanckA = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dPlanckA = options->dDefault;
    }
  }
}

void ReadPlanckB(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dPlanckB = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dPlanckB = options->dDefault;
    }
  }
}

void ReadSurfAlbedo(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dSurfAlbedo = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dSurfAlbedo = options->dDefault;
    }
  }
}

void ReadOrbitOblData(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].bReadOrbitOblData = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else
    body[iFile - 1].bReadOrbitOblData = options->dDefault;
}

void ReadFileOrbitOblData(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file -- Each body has an output file */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    strcpy(body[iFile - 1].cFileOrbitOblData, cTmp);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    strcpy(body[iFile - 1].cFileOrbitOblData, options->cDefault);
}

void ReadIceAlbedo(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dIceAlbedo = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dIceAlbedo = options->dDefault;
    }
  }
}

void ReadTGlobalInit(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dTGlobalInit = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dTGlobalInit = options->dDefault;
    }
  }
}

void ReadPCO2(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dpCO2 = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dpCO2 = options->dDefault;
    }
  }
}

void ReadDiffusion(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dDiffCoeff = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dDiffCoeff = options->dDefault;
    }
  }
}

void ReadFixIceLat(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dFixIceLat = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dFixIceLat = options->dDefault;
    }
  }
}

void ReadHeatCapAnn(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dHeatCapAnn = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dHeatCapAnn = options->dDefault;
    }
  }
}


void ReadIceDepRate(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dIceDepRate = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dIceDepRate = options->dDefault;
    }
  }
}

void ReadHadley(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].bHadley = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bHadley, files->iNumInputs);
  }
}

void ReadCalcAB(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].bCalcAB = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bCalcAB, files->iNumInputs);
  }
}

void ReadColdStart(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].bColdStart = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bColdStart, files->iNumInputs);
  }
}

void ReadAlbedoZA(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].bAlbedoZA = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bAlbedoZA, files->iNumInputs);
  }
}


void ReadSeaIceModel(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bSeaIceModel = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bSeaIceModel, files->iNumInputs);
  }
}

void ReadMEPDiff(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bMEPDiff = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bMEPDiff, files->iNumInputs);
  }
}

void ReadIceSheets(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bIceSheets = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bIceSheets, files->iNumInputs);
  }
}

void ReadSkipSeasEnabled(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bSkipSeasEnabled = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bSkipSeasEnabled,
                     files->iNumInputs);
  }
}

void ReadAccuracyMode(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bAccuracyMode = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bAccuracyMode,
                     files->iNumInputs);
  }
}

void ReadElevFB(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bElevFB = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bElevFB, files->iNumInputs);
  }
}

void ReadForceObliq(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bForceObliq = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bForceObliq, files->iNumInputs);
  }
}

void ReadForceEcc(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bForceEcc = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bForceEcc, files->iNumInputs);
  }
}

void ReadDiffRot(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bDiffRot = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bDiffRot, files->iNumInputs);
  }
}

void ReadClimateModel(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  char cTmp[OPTLEN];

  /* Climate Model, use #defined variables */

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* This parameter can not appear in the primary input,
       as it is module specific (it's easier to code this
       way. It should also only appear in one body file
       so as different tidal models cannot be called. */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    // CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,
    //                 control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "ann", 3)) {
      body[iFile - 1].iClimateModel = ANN;
    } else if (!memcmp(sLower(cTmp), "sea", 3)) {
      body[iFile - 1].iClimateModel = SEA;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s."
                " Options are annual or seasonal.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].iClimateModel,
                     files->iNumInputs);
  }
}

void ReadOLRModel(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  char cTmp[OPTLEN];

  /* OLR model, use #defined variables (used with CalcAB = 1)*/

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    // CheckDuplication(files,options,files->Infile[iFile].cIn,
    // lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "wk97", 3)) {
      body[iFile - 1].iOLRModel = WK97;
    } else if (!memcmp(sLower(cTmp), "hm16", 3)) {
      body[iFile - 1].iOLRModel = HM16;
    } else if (!memcmp(sLower(cTmp), "sms09", 3)) {
      body[iFile - 1].iOLRModel = SMS09;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s."
                " Options are wk97 or hm16.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].iOLRModel, files->iNumInputs);
  }
}

void ReadGeography(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  char cTmp[OPTLEN];

  /* Albedo type, use #defined variables */

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* This parameter can not appear in the primary input,
       as it is module specific (it's easier to code this
       way. It should also only appear in one body file
       so as different tidal models cannot be called. */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    //CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,\
                       control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "uni3", 4)) {
      body[iFile - 1].iGeography = UNIFORM3;
    } else if (!memcmp(sLower(cTmp), "modn", 4)) {
      body[iFile - 1].iGeography = MODERN;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s."
                " Options are uni3 or modn.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    body[iFile - 1].iGeography = options->dDefault;
  }
}


void ReadInitIceLat(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dInitIceLat = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dInitIceLat = options->dDefault;
    }
  }
}

void ReadInitIceHeight(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dInitIceHeight = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dInitIceHeight = options->dDefault;
    }
  }
}

void ReadHeatCapLand(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dHeatCapLand = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dHeatCapLand = options->dDefault;
    }
  }
}

void ReadSeaIceConduct(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dSeaIceConduct = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dSeaIceConduct = options->dDefault;
    }
  }
}

void ReadHeatCapWater(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dHeatCapWater = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dHeatCapWater = options->dDefault;
    }
  }
}

void ReadAlbedoLand(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dAlbedoLand = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dAlbedoLand = options->dDefault;
    }
  }
}

void ReadSeasOutputTime(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dSeasOutputTime =
          dTmp * fdUnitsTime(control->Units[iFile].iTime);
    ;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dSeasOutputTime = options->dDefault;
    }
  }
}

void ReadAlbedoWater(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dAlbedoWater = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dAlbedoWater = options->dDefault;
    }
  }
}

void ReadObliqAmp(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dObliqAmp = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dObliqAmp = options->dDefault;
    }
  }
}

void ReadObliqPer(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dObliqPer = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dObliqPer = options->dDefault;
    }
  }
}

void ReadEccAmp(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dEccAmp = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dEccAmp = options->dDefault;
    }
  }
}

void ReadEccPer(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dEccPer = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dEccPer = options->dDefault;
    }
  }
}

void ReadMixingDepth(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dMixingDepth = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dMixingDepth = options->dDefault;
    }
  }
}

void ReadFrzTSeaIce(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dFrzTSeaIce = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dFrzTSeaIce = options->dDefault;
    }
  }
}

void ReadLandFrac(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp > 1 || dTmp < 0) {
      fprintf(stderr, "ERROR: %s must be in the range [0,1].\n",
              options->cName);
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dLandFrac = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dLandFrac = options->dDefault;
    }
  }
}

void ReadNuLandWater(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dNuLandWater = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dNuLandWater = options->dDefault;
    }
  }
}

void ReadLapseR(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dLapseR = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dLapseR = options->dDefault;
    }
  }
}

void ReadRefHeight(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dRefHeight = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dRefHeight = options->dDefault;
    }
  }
}

void ReadSpinUpTol(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dSpinUpTol = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dSpinUpTol = options->dDefault;
    }
  }
}

void ReadAblateFF(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dAblateFF = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dAblateFF = options->dDefault;
    }
  }
}

void ReadNStepInYear(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, iTmp;
  AddOptionInt(files->Infile[iFile].cIn, options->cName, &iTmp, &lTmp,
               control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].iNStepInYear = iTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].iNStepInYear, files->iNumInputs);
  }
}

void ReadNumYears(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  int lTmp = -1, iTmp;
  AddOptionInt(files->Infile[iFile].cIn, options->cName, &iTmp, &lTmp,
               control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].iNumYears = iTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].iNumYears, files->iNumInputs);
  }
}

void ReadIceDt(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
  int lTmp = -1, iTmp;
  AddOptionInt(files->Infile[iFile].cIn, options->cName, &iTmp, &lTmp,
               control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].iIceTimeStep = iTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].iIceTimeStep, files->iNumInputs);
  }
}

void ReadReRunSeas(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  int lTmp = -1, iTmp;
  AddOptionInt(files->Infile[iFile].cIn, options->cName, &iTmp, &lTmp,
               control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].iReRunSeas = iTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].iReRunSeas, files->iNumInputs);
  }
}

void ReadMinIceSheetHeight(BODY *body, CONTROL *control, FILES *files,
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
      body[iFile - 1].dMinIceHeight =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dMinIceHeight =
            dTmp * fdUnitsLength(control->Units[iFile].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dMinIceHeight = options->dDefault;
    }
  }
}

void InitializeOptionsPoise(OPTIONS *options, fnReadOption fnRead[]) {
  sprintf(options[OPT_LATCELLNUM].cName, "iLatCellNum");
  sprintf(options[OPT_LATCELLNUM].cDescr, "Number of latitude cells used in"
                                          " climate model");
  sprintf(options[OPT_LATCELLNUM].cDefault, "50");
  options[OPT_LATCELLNUM].dDefault   = 50;
  options[OPT_LATCELLNUM].iType      = 1;
  options[OPT_LATCELLNUM].bMultiFile = 1;
  fnRead[OPT_LATCELLNUM]             = &ReadLatCellNum;

  sprintf(options[OPT_READORBITOBLDATA].cName, "bReadOrbitOblData");
  sprintf(options[OPT_READORBITOBLDATA].cDescr, "Read in orbital and obliquity \
    data and use with poise");
  sprintf(options[OPT_READORBITOBLDATA].cDefault, "0");
  options[OPT_READORBITOBLDATA].dDefault   = 0;
  options[OPT_READORBITOBLDATA].iType      = 0;
  options[OPT_READORBITOBLDATA].bMultiFile = 1;
  fnRead[OPT_READORBITOBLDATA]             = &ReadOrbitOblData;

  sprintf(options[OPT_FILEORBITOBLDATA].cName, "sFileOrbitOblData");
  sprintf(options[OPT_FILEORBITOBLDATA].cDescr, "Name of file containing orbit\
    and obliquity time series");
  sprintf(options[OPT_FILEORBITOBLDATA].cDefault, "Obl_data.txt");
  options[OPT_FILEORBITOBLDATA].iType = 3;
  fnRead[OPT_FILEORBITOBLDATA]        = &ReadFileOrbitOblData;

  sprintf(options[OPT_PLANCKA].cName, "dPlanckA");
  sprintf(options[OPT_PLANCKA].cDescr, "Constant 'A' used in OLR calculation");
  sprintf(options[OPT_PLANCKA].cDefault, "203.3");
  options[OPT_PLANCKA].dDefault   = 203.3;
  options[OPT_PLANCKA].iType      = 2;
  options[OPT_PLANCKA].bMultiFile = 1;
  fnRead[OPT_PLANCKA]             = &ReadPlanckA;

  sprintf(options[OPT_PLANCKB].cName, "dPlanckB");
  sprintf(options[OPT_PLANCKB].cDescr, "Sensitivity 'B' used in"
                                       " OLR calculation");
  sprintf(options[OPT_PLANCKB].cDefault, "2.09");
  sprintf(options[OPT_PLANCKB].cDimension, "nd");
  options[OPT_PLANCKB].dDefault   = 2.09;
  options[OPT_PLANCKB].iType      = 2;
  options[OPT_PLANCKB].bMultiFile = 1;
  fnRead[OPT_PLANCKB]             = &ReadPlanckB;

  sprintf(options[OPT_ICEALBEDO].cName, "dIceAlbedo");
  sprintf(options[OPT_ICEALBEDO].cDescr, "Albedo of ice");
  sprintf(options[OPT_ICEALBEDO].cDefault, "0.6");
  sprintf(options[OPT_ICEALBEDO].cDimension, "nd");
  options[OPT_ICEALBEDO].dDefault   = 0.6;
  options[OPT_ICEALBEDO].iType      = 2;
  options[OPT_ICEALBEDO].bMultiFile = 1;
  fnRead[OPT_ICEALBEDO]             = &ReadIceAlbedo;

  sprintf(options[OPT_SURFALBEDO].cName, "dSurfAlbedo");
  sprintf(options[OPT_SURFALBEDO].cDescr, "Albedo of (ice-free) surface");
  sprintf(options[OPT_SURFALBEDO].cDefault, "0.3");
  sprintf(options[OPT_SURFALBEDO].cDimension, "nd");
  options[OPT_SURFALBEDO].dDefault   = 0.3;
  options[OPT_SURFALBEDO].iType      = 2;
  options[OPT_SURFALBEDO].bMultiFile = 1;
  fnRead[OPT_SURFALBEDO]             = &ReadSurfAlbedo;

  sprintf(options[OPT_TGLOBALEST].cName, "dTGlobalInit");
  sprintf(options[OPT_TGLOBALEST].cDescr, "Estimate of initial global"
                                          " temperature");
  sprintf(options[OPT_TGLOBALEST].cDefault, "14.85");
  sprintf(options[OPT_TGLOBALEST].cDimension, "temperature");
  options[OPT_TGLOBALEST].dDefault   = 14.85;
  options[OPT_TGLOBALEST].iType      = 2;
  options[OPT_TGLOBALEST].bMultiFile = 1;
  fnRead[OPT_TGLOBALEST]             = &ReadTGlobalInit;

  sprintf(options[OPT_PCO2].cName, "dpCO2");
  sprintf(options[OPT_PCO2].cDescr, "Partial pressure of CO2 in atmosphere");
  sprintf(options[OPT_PCO2].cDefault, "3.3e-4");
  sprintf(options[OPT_PCO2].cDimension, "nd");
  options[OPT_PCO2].dDefault   = 3.3e-4;
  options[OPT_PCO2].iType      = 2;
  options[OPT_PCO2].bMultiFile = 1;
  fnRead[OPT_PCO2]             = &ReadPCO2;

  sprintf(options[OPT_CALCAB].cName, "bCalcAB");
  sprintf(options[OPT_CALCAB].cDescr, "Calculate A and B in OLR function, from"
                                      " (T & pCO2)");
  sprintf(options[OPT_CALCAB].cDefault, "0");
  sprintf(options[OPT_CALCAB].cDimension, "nd");
  options[OPT_CALCAB].dDefault   = 0;
  options[OPT_CALCAB].iType      = 0;
  options[OPT_CALCAB].bMultiFile = 1;
  fnRead[OPT_CALCAB]             = &ReadCalcAB;

  sprintf(options[OPT_DIFFUSION].cName, "dDiffusion");
  sprintf(options[OPT_DIFFUSION].cDescr, "Heat diffusion coefficient");
  sprintf(options[OPT_DIFFUSION].cDefault, "0.44");
  sprintf(options[OPT_DIFFUSION].cDimension, "nd");
  options[OPT_DIFFUSION].dDefault   = 0.44;
  options[OPT_DIFFUSION].iType      = 2;
  options[OPT_DIFFUSION].bMultiFile = 1;
  fnRead[OPT_DIFFUSION]             = &ReadDiffusion;

  sprintf(options[OPT_HADLEY].cName, "bHadley");
  sprintf(options[OPT_HADLEY].cDescr, "Enable Hadley circulation");
  sprintf(options[OPT_HADLEY].cDefault, "0");
  options[OPT_HADLEY].dDefault   = 0;
  options[OPT_HADLEY].iType      = 0;
  options[OPT_HADLEY].bMultiFile = 1;
  fnRead[OPT_HADLEY]             = &ReadHadley;

  sprintf(options[OPT_COLDSTART].cName, "bColdStart");
  sprintf(options[OPT_COLDSTART].cDescr, "Start from snowball Earth"
                                         " conditions");
  sprintf(options[OPT_COLDSTART].cDefault, "0");
  options[OPT_COLDSTART].dDefault   = 0;
  options[OPT_COLDSTART].iType      = 0;
  options[OPT_COLDSTART].bMultiFile = 1;
  fnRead[OPT_COLDSTART]             = &ReadColdStart;

  sprintf(options[OPT_FIXICELAT].cName, "dFixIceLat");
  sprintf(options[OPT_FIXICELAT].cDescr,
          "Force ice cap latitude to this value");
  sprintf(options[OPT_FIXICELAT].cDefault, "None");
  sprintf(options[OPT_FIXICELAT].cDimension, "nd");
  options[OPT_FIXICELAT].dDefault   = 0;
  options[OPT_FIXICELAT].iType      = 2;
  options[OPT_FIXICELAT].bMultiFile = 1;
  fnRead[OPT_FIXICELAT]             = &ReadFixIceLat;

  sprintf(options[OPT_ALBEDOZA].cName, "bAlbedoZA");
  sprintf(options[OPT_ALBEDOZA].cDescr, "Use albedo based on zenith angle");
  sprintf(options[OPT_ALBEDOZA].cDefault, "0");
  options[OPT_ALBEDOZA].dDefault   = 0;
  options[OPT_ALBEDOZA].iType      = 0;
  options[OPT_ALBEDOZA].bMultiFile = 1;
  fnRead[OPT_ALBEDOZA]             = &ReadAlbedoZA;

  sprintf(options[OPT_MEPDIFF].cName, "bMEPDiff");
  sprintf(options[OPT_MEPDIFF].cDescr, "Calculate diffusion from max entropy"
                                       " production (D=B/4)?");
  sprintf(options[OPT_MEPDIFF].cDefault, "0");
  options[OPT_MEPDIFF].dDefault   = 0;
  options[OPT_MEPDIFF].iType      = 0;
  options[OPT_MEPDIFF].bMultiFile = 1;
  fnRead[OPT_MEPDIFF]             = &ReadMEPDiff;

  sprintf(options[OPT_HEATCAPANN].cName, "dHeatCapAnn");
  sprintf(options[OPT_HEATCAPANN].cDescr, "Surface heat capacity in annual"
                                          " model");
  sprintf(options[OPT_HEATCAPANN].cDefault, "0.2");
  sprintf(options[OPT_HEATCAPANN].cDimension, "energy/temperature");
  options[OPT_HEATCAPANN].dDefault   = 0.2;
  options[OPT_HEATCAPANN].iType      = 2;
  options[OPT_HEATCAPANN].bMultiFile = 1;
  fnRead[OPT_HEATCAPANN]             = &ReadHeatCapAnn;

  sprintf(options[OPT_ICEDEPRATE].cName, "dIceDepRate");
  sprintf(options[OPT_ICEDEPRATE].cDescr, "Deposition rate of ice/snow to form"
                                          " ice sheets");
  sprintf(options[OPT_ICEDEPRATE].cDefault, "2.9e-5");
  sprintf(options[OPT_ICEDEPRATE].cDimension, "length/time");
  options[OPT_ICEDEPRATE].dDefault   = 2.9e-5;
  options[OPT_ICEDEPRATE].iType      = 2;
  options[OPT_ICEDEPRATE].bMultiFile = 1;
  fnRead[OPT_ICEDEPRATE]             = &ReadIceDepRate;

  sprintf(options[OPT_ICESHEETS].cName, "bIceSheets");
  sprintf(options[OPT_ICESHEETS].cDescr, "Include ice sheets?");
  sprintf(options[OPT_ICESHEETS].cDefault, "0");
  options[OPT_ICESHEETS].dDefault   = 0;
  options[OPT_ICESHEETS].iType      = 0;
  options[OPT_ICESHEETS].bMultiFile = 1;
  fnRead[OPT_ICESHEETS]             = &ReadIceSheets;

  sprintf(options[OPT_INITICELAT].cName, "dInitIceLat");
  sprintf(options[OPT_INITICELAT].cDescr, "Sets initial ice sheet latitude");
  sprintf(options[OPT_INITICELAT].cDefault, "90");
  sprintf(options[OPT_INITICELAT].cDimension, "angle");
  options[OPT_INITICELAT].dDefault   = 90.0;
  options[OPT_INITICELAT].iType      = 2;
  options[OPT_INITICELAT].bMultiFile = 1;
  fnRead[OPT_INITICELAT]             = &ReadInitIceLat;

  sprintf(options[OPT_INITICEHEIGHT].cName, "dInitIceHeight");
  sprintf(options[OPT_INITICEHEIGHT].cDescr, "Sets initial ice sheet height");
  sprintf(options[OPT_INITICEHEIGHT].cDefault, "50"); // 50 meters
  sprintf(options[OPT_INITICEHEIGHT].cDimension, "length");
  options[OPT_INITICEHEIGHT].dDefault   = 50.0;
  options[OPT_INITICEHEIGHT].iType      = 2;
  options[OPT_INITICEHEIGHT].bMultiFile = 1;
  fnRead[OPT_INITICEHEIGHT]             = &ReadInitIceHeight;

  sprintf(options[OPT_CLIMATEMODEL].cName, "sClimateModel");
  sprintf(options[OPT_CLIMATEMODEL].cDescr, "Use annual or seasonal model");
  sprintf(options[OPT_CLIMATEMODEL].cDefault, "ann");
  options[OPT_CLIMATEMODEL].dDefault   = ANN;
  options[OPT_CLIMATEMODEL].iType      = 3;
  options[OPT_CLIMATEMODEL].bMultiFile = 1;
  fnRead[OPT_CLIMATEMODEL]             = &ReadClimateModel;

  sprintf(options[OPT_OLRMODEL].cName, "iOLRModel");
  sprintf(options[OPT_OLRMODEL].cDescr, "Outgoing longwave rad model");
  sprintf(options[OPT_OLRMODEL].cDefault, "sms09");
  options[OPT_OLRMODEL].dDefault   = SMS09;
  options[OPT_OLRMODEL].iType      = 1;
  options[OPT_OLRMODEL].bMultiFile = 1;
  fnRead[OPT_OLRMODEL]             = &ReadOLRModel;

  sprintf(options[OPT_SKIPSEASENABLED].cName, "bSkipSeasEnabled");
  sprintf(options[OPT_SKIPSEASENABLED].cDescr, "Run annual before seasonal and"
                                               " allow skip seas?");
  sprintf(options[OPT_SKIPSEASENABLED].cDefault, "0");
  options[OPT_SKIPSEASENABLED].dDefault   = 0;
  options[OPT_SKIPSEASENABLED].iType      = 0;
  options[OPT_SKIPSEASENABLED].bMultiFile = 1;
  fnRead[OPT_SKIPSEASENABLED]             = &ReadSkipSeasEnabled;

  sprintf(options[OPT_HEATCAPLAND].cName, "dHeatCapLand");
  sprintf(options[OPT_HEATCAPLAND].cDescr, "Land heat capacity in seasonal"
                                           " model");
  sprintf(options[OPT_HEATCAPLAND].cDefault, "1.42e7"); // XXX What units?
  sprintf(options[OPT_HEATCAPLAND].cDimension, "energy/temperature");
  options[OPT_HEATCAPLAND].dDefault   = 1.42e7;
  options[OPT_HEATCAPLAND].iType      = 2;
  options[OPT_HEATCAPLAND].bMultiFile = 1;
  fnRead[OPT_HEATCAPLAND]             = &ReadHeatCapLand;

  sprintf(options[OPT_HEATCAPWATER].cName, "dHeatCapWater");
  sprintf(options[OPT_HEATCAPWATER].cDescr, "Water heat capacity per meter in"
                                            " seasonal model");
  sprintf(options[OPT_HEATCAPWATER].cDefault, "4.2e6"); // XXX What units
  sprintf(options[OPT_HEATCAPWATER].cDimension, "energy/temperature");
  options[OPT_HEATCAPWATER].dDefault   = 4.2e6;
  options[OPT_HEATCAPWATER].iType      = 2;
  options[OPT_HEATCAPWATER].bMultiFile = 1;
  fnRead[OPT_HEATCAPWATER]             = &ReadHeatCapWater;

  sprintf(options[OPT_MIXINGDEPTH].cName, "dMixingDepth");
  sprintf(options[OPT_MIXINGDEPTH].cDescr, "Mixing depth of ocean in seasonal"
                                           " model");
  sprintf(options[OPT_MIXINGDEPTH].cDefault, "70"); // meters
  sprintf(options[OPT_MIXINGDEPTH].cDimension, "length");
  options[OPT_MIXINGDEPTH].dDefault   = 70.;
  options[OPT_MIXINGDEPTH].iType      = 2;
  options[OPT_MIXINGDEPTH].bMultiFile = 1;
  fnRead[OPT_MIXINGDEPTH]             = &ReadMixingDepth;

  sprintf(options[OPT_FRZTSEAICE].cName, "dFrzTSeaIce");
  sprintf(options[OPT_FRZTSEAICE].cDescr, "Temp of sea ice formation in"
                                          " seasonal model");
  sprintf(options[OPT_FRZTSEAICE].cDefault, "-2 deg C");
  sprintf(options[OPT_FRZTSEAICE].cDimension, "temperature");
  options[OPT_FRZTSEAICE].dDefault   = -2.;
  options[OPT_FRZTSEAICE].iType      = 2;
  options[OPT_FRZTSEAICE].bMultiFile = 1;
  fnRead[OPT_FRZTSEAICE]             = &ReadFrzTSeaIce;

  sprintf(options[OPT_NULANDWATER].cName, "dNuLandWater");
  sprintf(options[OPT_NULANDWATER].cDescr, "Coefficient of land-ocean heat"
                                           " flux");
  sprintf(options[OPT_NULANDWATER].cDefault, "0.81");
  sprintf(options[OPT_NULANDWATER].cDimension, "energy/length^2");
  options[OPT_NULANDWATER].dDefault   = 0.81;
  options[OPT_NULANDWATER].iType      = 2;
  options[OPT_NULANDWATER].bMultiFile = 1;
  fnRead[OPT_NULANDWATER]             = &ReadNuLandWater;

  sprintf(options[OPT_LANDFRAC].cName, "dLandFrac");
  sprintf(options[OPT_LANDFRAC].cDescr, "Fraction of land on the planetary"
                                        " surface");
  sprintf(options[OPT_LANDFRAC].cDefault, "0.34");
  sprintf(options[OPT_LANDFRAC].cDimension, "nd");
  options[OPT_LANDFRAC].dDefault   = 0.34;
  options[OPT_LANDFRAC].iType      = 2;
  options[OPT_LANDFRAC].bMultiFile = 1;
  fnRead[OPT_LANDFRAC]             = &ReadLandFrac;

  sprintf(options[OPT_NSTEPINYEAR].cName, "iNStepInYear");
  sprintf(options[OPT_NSTEPINYEAR].cDescr, "Number of time-steps/year in"
                                           " seasonal model");
  sprintf(options[OPT_NSTEPINYEAR].cDefault, "60");
  options[OPT_NSTEPINYEAR].dDefault   = 60;
  options[OPT_NSTEPINYEAR].iType      = 1;
  options[OPT_NSTEPINYEAR].bMultiFile = 1;
  fnRead[OPT_NSTEPINYEAR]             = &ReadNStepInYear;

  sprintf(options[OPT_NUMYEARS].cName, "iNumYears");
  sprintf(options[OPT_NUMYEARS].cDescr, "Number of years to run seasonal"
                                        " model");
  sprintf(options[OPT_NUMYEARS].cDefault, "10");
  options[OPT_NUMYEARS].dDefault   = 10;
  options[OPT_NUMYEARS].iType      = 1;
  options[OPT_NUMYEARS].bMultiFile = 1;
  fnRead[OPT_NUMYEARS]             = &ReadNumYears;

  sprintf(options[OPT_SEAICEMODEL].cName, "bSeaIceModel");
  sprintf(options[OPT_SEAICEMODEL].cDescr, "model sea ice dynamics and heat"
                                           " flow?");
  sprintf(options[OPT_SEAICEMODEL].cDefault, "1");
  options[OPT_SEAICEMODEL].dDefault   = 1;
  options[OPT_SEAICEMODEL].iType      = 0;
  options[OPT_SEAICEMODEL].bMultiFile = 1;
  fnRead[OPT_SEAICEMODEL]             = &ReadSeaIceModel;

  sprintf(options[OPT_ICECONDUCT].cName, "dSeaIceConduct");
  sprintf(options[OPT_ICECONDUCT].cDescr, "Heat conductivity of sea ice");
  sprintf(options[OPT_ICECONDUCT].cDefault, "2");
  sprintf(options[OPT_ICECONDUCT].cDimension, "nd");
  options[OPT_ICECONDUCT].dDefault   = 2.;
  options[OPT_ICECONDUCT].iType      = 2;
  options[OPT_ICECONDUCT].bMultiFile = 1;
  fnRead[OPT_ICECONDUCT]             = &ReadSeaIceConduct;

  sprintf(options[OPT_ALBEDOLAND].cName, "dAlbedoLand");
  sprintf(options[OPT_ALBEDOLAND].cDescr, "albedo of dry land");
  sprintf(options[OPT_ALBEDOLAND].cDefault, "0.363");
  sprintf(options[OPT_ALBEDOLAND].cDimension, "nd");
  options[OPT_ALBEDOLAND].dDefault   = 0.363;
  options[OPT_ALBEDOLAND].iType      = 2;
  options[OPT_ALBEDOLAND].bMultiFile = 1;
  fnRead[OPT_ALBEDOLAND]             = &ReadAlbedoLand;

  sprintf(options[OPT_ALBEDOWATER].cName, "dAlbedoWater");
  sprintf(options[OPT_ALBEDOWATER].cDescr, "albedo of open water");
  sprintf(options[OPT_ALBEDOWATER].cDefault, "0.263");
  sprintf(options[OPT_ALBEDOWATER].cDimension, "nd");
  options[OPT_ALBEDOWATER].dDefault   = 0.263;
  options[OPT_ALBEDOWATER].iType      = 2;
  options[OPT_ALBEDOWATER].bMultiFile = 1;
  fnRead[OPT_ALBEDOWATER]             = &ReadAlbedoWater;

  sprintf(options[OPT_ICEDT].cName, "iIceDt");
  sprintf(options[OPT_ICEDT].cDescr, "Minimum ice sheet timestep (unit orbital"
                                     " period)");
  sprintf(options[OPT_ICEDT].cDefault, "5");
  options[OPT_ICEDT].dDefault   = 5;
  options[OPT_ICEDT].iType      = 1;
  options[OPT_ICEDT].bMultiFile = 1;
  fnRead[OPT_ICEDT]             = &ReadIceDt;

  sprintf(options[OPT_RERUNSEAS].cName, "iReRunSeas");
  sprintf(options[OPT_RERUNSEAS].cDescr, "how often to rerun seasonal in ice"
                                         " sheet model, in number of orbital"
                                         " periods");
  sprintf(options[OPT_RERUNSEAS].cDefault, "500");
  options[OPT_RERUNSEAS].dDefault   = 5;
  options[OPT_RERUNSEAS].iType      = 1;
  options[OPT_RERUNSEAS].bMultiFile = 1;
  fnRead[OPT_RERUNSEAS]             = &ReadReRunSeas;

  sprintf(options[OPT_GEOGRAPHY].cName, "sGeography");
  sprintf(options[OPT_GEOGRAPHY].cDescr, "Type of land distribution");
  sprintf(options[OPT_GEOGRAPHY].cDefault, "uni3");
  options[OPT_GEOGRAPHY].dDefault   = UNIFORM3;
  options[OPT_GEOGRAPHY].iType      = 3;
  options[OPT_GEOGRAPHY].bMultiFile = 1;
  fnRead[OPT_GEOGRAPHY]             = &ReadGeography;

  sprintf(options[OPT_SEASOUTPUTTIME].cName, "dSeasOutputTime");
  sprintf(options[OPT_SEASOUTPUTTIME].cDescr, "Output interval for seasonal"
                                              " parameters");
  sprintf(options[OPT_SEASOUTPUTTIME].cDefault, "0");
  sprintf(options[OPT_SEASOUTPUTTIME].cDimension, "nd");
  options[OPT_SEASOUTPUTTIME].dDefault   = 0;
  options[OPT_SEASOUTPUTTIME].iType      = 2;
  options[OPT_SEASOUTPUTTIME].bMultiFile = 1;
  fnRead[OPT_SEASOUTPUTTIME]             = &ReadSeasOutputTime;

  sprintf(options[OPT_FORCEOBLIQ].cName, "bForceObliq");
  sprintf(options[OPT_FORCEOBLIQ].cDescr, "Force obliquity to evolve"
                                          " sinusoidally?");
  sprintf(options[OPT_FORCEOBLIQ].cDefault, "0");
  options[OPT_FORCEOBLIQ].dDefault   = 0;
  options[OPT_FORCEOBLIQ].iType      = 0;
  options[OPT_FORCEOBLIQ].bMultiFile = 1;
  fnRead[OPT_FORCEOBLIQ]             = &ReadForceObliq;

  sprintf(options[OPT_DIFFROT].cName, "bDiffRot");
  sprintf(options[OPT_DIFFROT].cDescr, "Adjust heat diffusion for rotation"
                                       " rate?");
  sprintf(options[OPT_DIFFROT].cDefault, "0");
  options[OPT_DIFFROT].dDefault   = 0;
  options[OPT_DIFFROT].iType      = 0;
  options[OPT_DIFFROT].bMultiFile = 1;
  fnRead[OPT_DIFFROT]             = &ReadDiffRot;

  sprintf(options[OPT_OBLIQAMP].cName, "dObliqAmp");
  sprintf(options[OPT_OBLIQAMP].cDescr, "Amplitude of forced obliquity oscill");
  sprintf(options[OPT_OBLIQAMP].cDefault, "50 deg");
  sprintf(options[OPT_OBLIQAMP].cDimension, "angle");
  options[OPT_OBLIQAMP].dDefault   = 50;
  options[OPT_OBLIQAMP].iType      = 2;
  options[OPT_OBLIQAMP].bMultiFile = 1;
  fnRead[OPT_OBLIQAMP]             = &ReadObliqAmp;

  sprintf(options[OPT_OBLIQPER].cName, "dObliqPer");
  sprintf(options[OPT_OBLIQPER].cDescr, "Period of forced obliquity oscill");
  sprintf(options[OPT_OBLIQPER].cDefault, "50000"); // XXX What units?
  sprintf(options[OPT_OBLIQPER].cDimension, "time");
  options[OPT_OBLIQPER].dDefault   = 50000;
  options[OPT_OBLIQPER].iType      = 2;
  options[OPT_OBLIQPER].bMultiFile = 1;
  fnRead[OPT_OBLIQPER]             = &ReadObliqPer;

  sprintf(options[OPT_FORCEECC].cName, "bForceEcc");
  sprintf(options[OPT_FORCEECC].cDescr, "Force Eccentricity to evolve"
                                        " sinusoidally?");
  sprintf(options[OPT_FORCEECC].cDefault, "0");
  options[OPT_FORCEECC].dDefault   = 0;
  options[OPT_FORCEECC].iType      = 0;
  options[OPT_FORCEECC].bMultiFile = 1;
  fnRead[OPT_FORCEECC]             = &ReadForceEcc;

  sprintf(options[OPT_ECCAMP].cName, "dEccAmp");
  sprintf(options[OPT_ECCAMP].cDescr, "Amplitude of forced eccentricity"
                                      " oscill");
  sprintf(options[OPT_ECCAMP].cDefault, "0.1");
  sprintf(options[OPT_ECCAMP].cDimension, "nd");
  options[OPT_ECCAMP].dDefault   = 0.1;
  options[OPT_ECCAMP].iType      = 2;
  options[OPT_ECCAMP].bMultiFile = 1;
  fnRead[OPT_ECCAMP]             = &ReadEccAmp;

  sprintf(options[OPT_ECCPER].cName, "dEccPer");
  sprintf(options[OPT_ECCPER].cDescr, "Period of forced eccentricity oscill");
  sprintf(options[OPT_ECCPER].cDefault, "50000"); // !!! What units?
  sprintf(options[OPT_ECCPER].cDimension, "time");
  options[OPT_ECCPER].dDefault   = 50000;
  options[OPT_ECCPER].iType      = 2;
  options[OPT_ECCPER].bMultiFile = 1;
  fnRead[OPT_ECCPER]             = &ReadEccPer;

  sprintf(options[OPT_ACCUMODE].cName, "bAccuracyMode");
  sprintf(options[OPT_ACCUMODE].cDescr,
          "Re-invert matrix every EBM time step?");
  sprintf(options[OPT_ACCUMODE].cDefault, "0");
  options[OPT_ACCUMODE].dDefault   = 0;
  options[OPT_ACCUMODE].iType      = 0;
  options[OPT_ACCUMODE].bMultiFile = 1;
  fnRead[OPT_ACCUMODE]             = &ReadAccuracyMode;

  sprintf(options[OPT_ELEVFB].cName, "bElevFB");
  sprintf(options[OPT_ELEVFB].cDescr, "Use elevation feedback for ice sheet"
                                      " ablation?");
  sprintf(options[OPT_ELEVFB].cDefault, "0");
  options[OPT_ELEVFB].dDefault   = 0;
  options[OPT_ELEVFB].iType      = 0;
  options[OPT_ELEVFB].bMultiFile = 1;
  fnRead[OPT_ELEVFB]             = &ReadElevFB;

  sprintf(options[OPT_LAPSER].cName, "dLapseR");
  sprintf(options[OPT_LAPSER].cDescr, "Dry adiabatic lapse rate (for elev"
                                      " feedback)");
  sprintf(options[OPT_LAPSER].cDefault, "9.8e-3 C/m");
  sprintf(options[OPT_LAPSER].cDimension, "temperature/length");
  options[OPT_LAPSER].dDefault   = 9.8e-3;
  options[OPT_LAPSER].iType      = 2;
  options[OPT_LAPSER].bMultiFile = 1;
  fnRead[OPT_LAPSER]             = &ReadLapseR;

  sprintf(options[OPT_REFHEIGHT].cName, "dRefHeight");
  sprintf(options[OPT_REFHEIGHT].cDescr, "Reference height of atmos temp (for"
                                         " elev feedback)");
  sprintf(options[OPT_REFHEIGHT].cDefault, "1000 m");
  sprintf(options[OPT_REFHEIGHT].cDimension, "length");
  options[OPT_REFHEIGHT].dDefault   = 1000.0;
  options[OPT_REFHEIGHT].iType      = 2;
  options[OPT_REFHEIGHT].bMultiFile = 1;
  fnRead[OPT_REFHEIGHT]             = &ReadRefHeight;

  sprintf(options[OPT_ABLATEFF].cName, "dAblateFF");
  sprintf(options[OPT_ABLATEFF].cDescr, "Ice ablation fudge factor");
  sprintf(options[OPT_ABLATEFF].cDefault, "2.3");
  sprintf(options[OPT_ABLATEFF].cDimension, "nd");
  options[OPT_ABLATEFF].dDefault   = 2.3;
  options[OPT_ABLATEFF].iType      = 2;
  options[OPT_ABLATEFF].bMultiFile = 1;
  fnRead[OPT_ABLATEFF]             = &ReadAblateFF;

  sprintf(options[OPT_SPINUPTOL].cName, "dSpinUpTol");
  sprintf(options[OPT_SPINUPTOL].cDescr, "Tolerance for spin up phase");
  sprintf(options[OPT_SPINUPTOL].cDefault, "0.1 deg C");
  sprintf(options[OPT_SPINUPTOL].cDimension, "temperature");
  options[OPT_SPINUPTOL].dDefault   = 0.1;
  options[OPT_SPINUPTOL].iType      = 2;
  options[OPT_SPINUPTOL].bMultiFile = 1;
  fnRead[OPT_SPINUPTOL]             = &ReadSpinUpTol;

  sprintf(options[OPT_MINICEHEIGHT].cName, "dMinIceSheetHeight");
  sprintf(options[OPT_MINICEHEIGHT].cDescr, "Minimum ice sheet height for a"
                                            " latitude to be considered"
                                            " ice-covered");
  sprintf(options[OPT_MINICEHEIGHT].cDefault, "0.001"); // What units? XXX
  sprintf(options[OPT_MINICEHEIGHT].cDimension, "length");
  options[OPT_MINICEHEIGHT].dDefault   = 0.001;
  options[OPT_MINICEHEIGHT].iType      = 2;
  options[OPT_MINICEHEIGHT].bMultiFile = 1;
  options[OPT_MINICEHEIGHT].dNeg       = 1; // Convert to SI
  sprintf(options[OPT_MINICEHEIGHT].cNeg, "meters");
  fnRead[OPT_MINICEHEIGHT] = &ReadMinIceSheetHeight;
  sprintf(options[OPT_MINICEHEIGHT].cLongDescr,
          "The minimum thickness of permanent ice in a latitude bin for it to "
          "be\n"
          "labeled ice-covered. In some cases, such as rapid thawing, a "
          "latituden\n"
          "can have a very low value of ice height, primarily for numerical\n"
          "reasons. Parameter forces unphysically small values of the ice "
          "height\n"
          "to be ignored.\n");
}

void ReadOptionsPoise(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, fnReadOption fnRead[],
                      int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTPOISE; iOpt < OPTENDPOISE; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
  //   ReadGridOutput(files,options,output,iBody+1,control->Io.iVerbose);
}


/******************* Verify POISE ******************/
void VerifyAlbedo(BODY *body, OPTIONS *options, char cFile[], int iBody,
                  int iVerbose) {
  if (body[iBody].iClimateModel == ANN) {
    /* If all of bColdstart, dFixIceLat, and bAlbedoZA are set, exit */
    if (options[OPT_COLDSTART].iLine[iBody + 1] > -1 &&
        options[OPT_FIXICELAT].iLine[iBody + 1] > -1 &&
        options[OPT_ALBEDOZA].iLine[iBody + 1] > -1) {
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Only one of %s, %s, and %s can be set in File:"
                " %s\n",
                options[OPT_COLDSTART].cName, options[OPT_FIXICELAT].cName,
                options[OPT_ALBEDOZA].cName, cFile);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }

    /* Any two of bColdstart, dFixIceLat, and bAlbedoZA are set, exit */
    if (options[OPT_COLDSTART].iLine[iBody + 1] > -1 &&
        options[OPT_FIXICELAT].iLine[iBody + 1] > -1 &&
        options[OPT_ALBEDOZA].iLine[iBody + 1] == -1) {
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Only one of %s and %s can be set in File:"
                " %s\n",
                options[OPT_COLDSTART].cName, options[OPT_FIXICELAT].cName,
                cFile);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }

    if (options[OPT_COLDSTART].iLine[iBody + 1] > -1 &&
        options[OPT_FIXICELAT].iLine[iBody + 1] == -1 &&
        options[OPT_ALBEDOZA].iLine[iBody + 1] > -1) {
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Only one of %s and %s can be set in File:"
                " %s\n",
                options[OPT_COLDSTART].cName, options[OPT_ALBEDOZA].cName,
                cFile);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }

    if (options[OPT_COLDSTART].iLine[iBody + 1] == -1 &&
        options[OPT_FIXICELAT].iLine[iBody + 1] > -1 &&
        options[OPT_ALBEDOZA].iLine[iBody + 1] > -1) {
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Only one of %s and %s can be set in File:"
                " %s\n",
                options[OPT_FIXICELAT].cName, options[OPT_ALBEDOZA].cName,
                cFile);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }

    if (options[OPT_ALBEDOLAND].iLine[iBody + 1] > -1 ||
        options[OPT_ALBEDOWATER].iLine[iBody + 1] > -1) {
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Cannot set %s or %s for annual model in file"
                " %s\n. Please use option %s\n",
                options[OPT_ALBEDOLAND].cName, options[OPT_ALBEDOWATER].cName,
                cFile, options[OPT_SURFALBEDO].cName);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }

  } else if (body[iBody].iClimateModel == SEA) {
    if (options[OPT_SURFALBEDO].iLine[iBody + 1] > -1) {
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Cannot set %s for seasonal model in file\
                %s\nPlease use options %s and %s\n",
                options[OPT_SURFALBEDO].cName, cFile,
                options[OPT_ALBEDOLAND].cName, options[OPT_ALBEDOWATER].cName);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }
  }
}

void VerifyIceSheets(BODY *body, OPTIONS *options, char cFile[], int iBody,
                     int iVerbose) {
  if (body[iBody].iClimateModel == ANN) {
    // LCOV_EXCL_START
    if (iVerbose >= VERBERR) {
      fprintf(stderr, "ERROR: Cannot set %s in annual model in File:%s\n",
              options[OPT_ICESHEETS].cName, cFile);
    }
    exit(EXIT_INPUT);
    // LCOV_EXCL_STOP
  }
}

void VerifyOLR(BODY *body, OPTIONS *options, char cFile[], int iBody,
               int iVerbose) {
  if (body[iBody].bCalcAB) {
    if (options[OPT_PLANCKA].iLine[iBody + 1] > -1 ||
        options[OPT_PLANCKB].iLine[iBody + 1] > -1) {
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Cannot set %s or %s when setting bCalcAB = 1 \
                in File:%s\n",
                options[OPT_PLANCKA].cName, options[OPT_PLANCKB].cName, cFile);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }

  } else {
    if (options[OPT_PCO2].iLine[iBody + 1] > -1) {
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Cannot set %s unless setting bCalcAB = 1 \
                in File:%s\n",
                options[OPT_PCO2].cName, cFile);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }
  }
}

void VerifyOrbitOblData(BODY *body, CONTROL *control, OPTIONS *options,
                        int iBody) {
  int iNLines, iLine, c;
  double dttmp, datmp, detmp, ditmp, daptmp, dlatmp, dmatmp, dobltmp, dprecatmp;
  FILE *fileorb;

  if (body[iBody].bReadOrbitOblData) {
    if (options[OPT_FILEORBITOBLDATA].iLine[iBody + 1] == -1) {
      fprintf(stderr, "ERROR: Must set %s if using %s for file %s\n",
              options[OPT_FILEORBITOBLDATA].cName,
              options[OPT_READORBITOBLDATA].cName, body[iBody].cName);
      exit(EXIT_INPUT);
    } else {
      fileorb = fopen(body[iBody].cFileOrbitOblData, "r");
      if (fileorb == NULL) {
        printf("ERROR: File %s not found.\n", body[iBody].cFileOrbitOblData);
        exit(EXIT_INPUT);
      }
      iNLines = 0;
      while ((c = getc(fileorb)) != EOF) {
        if (c == '\n')
          iNLines++; // add 1 for each new line
      }
      rewind(fileorb);

      body[iBody].iNLines       = iNLines;
      body[iBody].daTimeSeries  = malloc(iNLines * sizeof(double));
      body[iBody].daSemiSeries  = malloc(iNLines * sizeof(double));
      body[iBody].daEccSeries   = malloc(iNLines * sizeof(double));
      body[iBody].daArgPSeries  = malloc(iNLines * sizeof(double));
      body[iBody].daLongASeries = malloc(iNLines * sizeof(double));
      body[iBody].daOblSeries   = malloc(iNLines * sizeof(double));
      body[iBody].daPrecASeries = malloc(iNLines * sizeof(double));

      body[iBody].daHeccSeries = malloc(iNLines * sizeof(double));
      body[iBody].daKeccSeries = malloc(iNLines * sizeof(double));

      printf("file open\n");

      iLine = 0;
      while (feof(fileorb) == 0) {
        if (fscanf(fileorb, "%lf %lf %lf %lf %lf %lf %lf", &dttmp, &datmp, &detmp,
              &daptmp, &dlatmp, &dobltmp, &dprecatmp) != 7) {
                fprintf(stderr,"ERROR: Incorrect number of columns in orbit-obliquity file.");
                exit(EXIT_INPUT);
        }

        body[iBody].daTimeSeries[iLine] =
              dttmp * fdUnitsTime(control->Units[iBody + 1].iTime);
        body[iBody].daSemiSeries[iLine] =
              datmp * fdUnitsLength(control->Units[iBody + 1].iLength);
        body[iBody].daEccSeries[iLine] = detmp;

        if (control->Units[iBody + 1].iAngle == 0) {
          body[iBody].daArgPSeries[iLine]  = daptmp;
          body[iBody].daLongASeries[iLine] = dlatmp;
          body[iBody].daOblSeries[iLine]   = dobltmp;
          body[iBody].daPrecASeries[iLine] = dprecatmp;
        } else {
          body[iBody].daArgPSeries[iLine]  = daptmp * DEGRAD;
          body[iBody].daLongASeries[iLine] = dlatmp * DEGRAD;
          body[iBody].daOblSeries[iLine]   = dobltmp * DEGRAD;
          body[iBody].daPrecASeries[iLine] = dprecatmp * DEGRAD;
        }
        body[iBody].daHeccSeries[iLine] = body[iBody].daEccSeries[iLine] *
                                          sin(body[iBody].daArgPSeries[iLine] +
                                              body[iBody].daLongASeries[iLine]);
        body[iBody].daKeccSeries[iLine] = body[iBody].daEccSeries[iLine] *
                                          cos(body[iBody].daArgPSeries[iLine] +
                                              body[iBody].daLongASeries[iLine]);

        iLine++;
      }
      fclose(fileorb);
    }
    body[iBody].iCurrentStep = 0;
    if (control->Evolve.bVarDt) {
      fprintf(stderr,
              "ERROR: Cannot use variable time step (%s = 1) if %s = 1\n",
              options[OPT_VARDT].cName, options[OPT_READORBITDATA].cName);
      exit(EXIT_INPUT);
    }
    if (control->Evolve.bDoForward) {
      if (body[iBody].daTimeSeries[1] != control->Evolve.dTimeStep) {
        fprintf(stderr,
                "ERROR: Time step size (%s = 1) must match orbital data if %s "
                "= 1\n",
                options[OPT_TIMESTEP].cName, options[OPT_READORBITDATA].cName);
        exit(EXIT_INPUT);
      }
    } else if (control->Evolve.bDoBackward) {
      if (body[iBody].daTimeSeries[1] != -1 * control->Evolve.dTimeStep) {
        fprintf(stderr,
                "ERROR: Time step size (%s = 1) must match orbital data if %s "
                "= 1\n",
                options[OPT_TIMESTEP].cName, options[OPT_READORBITDATA].cName);
        exit(EXIT_INPUT);
      }
    }
    if (iNLines < (control->Evolve.dStopTime / control->Evolve.dTimeStep + 1)) {
      fprintf(stderr,
              "ERROR: Input orbit data must at least as long as vplanet "
              "integration (%f years)\n",
              control->Evolve.dStopTime / YEARSEC);
      exit(EXIT_INPUT);
    }
  }
}

void VerifyNStepSeasonal(BODY *body, int iBody) {
  if (body[iBody].iNStepInYear > body[iBody].iNDays) {
    body[iBody].iNStepInYear = body[iBody].iNDays;
  }
}

void InitializeLatGrid(BODY *body, int iBody) {
  double dDelta_x, SinLat;
  int iLats;
  dDelta_x = 2.0 / body[iBody].iNumLats;

  body[iBody].daLats = malloc(body[iBody].iNumLats * sizeof(double));

  for (iLats = 0; iLats < body[iBody].iNumLats; iLats++) {
    SinLat                    = (-1.0 + dDelta_x / 2.) + iLats * dDelta_x;
    body[iBody].daLats[iLats] = asin(SinLat);
  }
}

void InitializeLandWater(BODY *body, int iBody) {
  int iLat;

  body[iBody].daLandFrac  = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daWaterFrac = malloc(body[iBody].iNumLats * sizeof(double));

  if (body[iBody].iGeography == UNIFORM3) {
    for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
      body[iBody].daLandFrac[iLat]  = body[iBody].dLandFrac;
      body[iBody].daWaterFrac[iLat] = 1.0 - body[iBody].daLandFrac[iLat];
    }
  } else if (body[iBody].iGeography == MODERN) {
    for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
      if (body[iBody].daLats[iLat] * 180. / PI <= -60) {
        body[iBody].daLandFrac[iLat] = 0.95 / 1.0094;
      } else if (body[iBody].daLats[iLat] * 180. / PI > -60 &&
                 body[iBody].daLats[iLat] * 180. / PI <= -40) {
        body[iBody].daLandFrac[iLat] = 0.05 / 1.0094;
      } else if (body[iBody].daLats[iLat] * 180. / PI > -40 &&
                 body[iBody].daLats[iLat] * 180. / PI <= 20) {
        body[iBody].daLandFrac[iLat] = 0.25 / 1.0094;
      } else if (body[iBody].daLats[iLat] * 180. / PI > 20 &&
                 body[iBody].daLats[iLat] * 180. / PI <= 70) {
        body[iBody].daLandFrac[iLat] = 0.5 / 1.0094;
      } else {
        body[iBody].daLandFrac[iLat] = 0.38 / 1.0094;
      }
      body[iBody].daWaterFrac[iLat] = 1.0 - body[iBody].daLandFrac[iLat];
    }
  }
}


void DampTemp(BODY *body, double dTGlobalTmp, int iBody) {
  int iLat;
  double deltaT;
  deltaT = (dTGlobalTmp - body[iBody].dTGlobal) / 5.0;

  body[iBody].dAlbedoGlobal = 0.0;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    body[iBody].daTempLand[iLat] += deltaT;
    body[iBody].daTempWater[iLat] += deltaT;
  }
  /* since fvAlbedoSeasonal is executed only once, we need to multiply
     dAlbedoGlobal by iNStepInYear below */
  fvAlbedoSeasonal(body, iBody, 0);
  body[iBody].dAlbedoGlobal =
        body[iBody].dAlbedoGlobal * body[iBody].iNStepInYear;
  ;
}


void InitializeClimateParams(BODY *body, int iBody, int iVerbose) {
  int iLat, jLat, count, iRun;
  double Toffset, dXBoundary, RunningMeanTmp;
  double *daRunningMean;

  body[iBody].dIceMassTot   = 0.0;
  body[iBody].daInsol       = malloc(body[iBody].iNumLats * sizeof(double *));
  body[iBody].daAnnualInsol = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daPeakInsol   = malloc(body[iBody].iNumLats * sizeof(double));
  // number of days in year
  body[iBody].iNDays =
        (int)floor(body[iBody].dRotRate / body[iBody].dMeanMotion);
  body[iBody].daFlux      = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daFluxIn    = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daFluxOut   = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daDivFlux   = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daIceHeight = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daIceFlow   = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daIceFlowMid =
        malloc((body[iBody].iNumLats + 1) * sizeof(double));
  body[iBody].daDIceHeightDy = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daDeclination  = malloc(body[iBody].iNDays * sizeof(double));
  body[iBody].daTGrad        = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daDMidPt       = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daDeltaTempL   = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daDeltaTempW   = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daEnergyResL   = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daEnergyResW   = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daEnerResLAnn  = malloc(body[iBody].iNumLats * sizeof(double));
  body[iBody].daEnerResWAnn  = malloc(body[iBody].iNumLats * sizeof(double));

  /* initialize some output quantities to zero to avoid memory errors */
  body[iBody].dFluxInGlobal  = 0;
  body[iBody].bSkipSeas      = 0;
  body[iBody].dFluxOutGlobal = 0;
  body[iBody].dIceMassTot    = 0;
  body[iBody].dIceFlowTot    = 0;
  body[iBody].dIceBalanceTot = 0;
  /*-------------------------------------------------------------------*/

  body[iBody].bSnowball      = 0;
  body[iBody].dFluxInGlobal  = 0;
  body[iBody].dIceFlowTot    = 0;
  body[iBody].dIceBalanceTot = 0;
  body[iBody].iWriteLat      = 0;

  if (body[iBody].bColdStart) {
    Toffset = -40.0;
  } else {
    Toffset = 0.0;
  }
  body[iBody].dTGlobal      = 0.0;
  body[iBody].dAlbedoGlobal = 0.0;

  if (body[iBody].bDistRot == 0) {
    body[iBody].dPrecA0 = body[iBody].dPrecA;
  }

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeasEnabled) {
    body[iBody].daDiffusionAnn =
          malloc((body[iBody].iNumLats + 1) * sizeof(double));
    body[iBody].daLambdaAnn =
          malloc((body[iBody].iNumLats + 1) * sizeof(double));
    body[iBody].daAlbedoAnn = malloc(body[iBody].iNumLats * sizeof(double));
    // body[iBody].iNDays = 360;
    // matrix of heat diffusion terms only
    body[iBody].daMDiffAnn = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daTempAnn  = malloc(body[iBody].iNumLats * sizeof(double));

    body[iBody].daPlanckAAnn = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daPlanckBAnn = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daMClim      = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daMEulerAnn  = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daMEulerCopyAnn =
          malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daInvMAnn    = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daSourceF    = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTempTerms  = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTmpTempAnn = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTmpTempTerms = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].iaRowswapAnn   = malloc(body[iBody].iNumLats * sizeof(int));
    body[iBody].daScaleAnn     = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daUnitVAnn     = malloc(body[iBody].iNumLats * sizeof(double));

    body[iBody].daDiffusionAnn[0] = body[iBody].dDiffCoeff;
    for (iLat = 0; iLat <= body[iBody].iNumLats; iLat++) {
      if (iLat != body[iBody].iNumLats) {
        body[iBody].daTempAnn[iLat] =
              20. * (1.0 - 1.5 * sin(body[iBody].daLats[iLat]) *
                                 sin(body[iBody].daLats[iLat])) +
              Toffset;
        body[iBody].dTGlobal +=
              body[iBody].daTempAnn[iLat] / body[iBody].iNumLats;
        body[iBody].daInsol[iLat] = malloc(body[iBody].iNDays * sizeof(double));
        body[iBody].daMClim[iLat] =
              malloc(body[iBody].iNumLats * sizeof(double));
        body[iBody].daMDiffAnn[iLat] =
              malloc(body[iBody].iNumLats * sizeof(double));
        body[iBody].daMEulerAnn[iLat] =
              malloc(body[iBody].iNumLats * sizeof(double));
        body[iBody].daMEulerCopyAnn[iLat] =
              malloc(body[iBody].iNumLats * sizeof(double));
        body[iBody].daInvMAnn[iLat] =
              malloc(body[iBody].iNumLats * sizeof(double));
      }

      if (body[iBody].bCalcAB) {
        /* Calculate A and B from williams and kasting 97 result */
        body[iBody].daPlanckBAnn[iLat] = fdOLRdTwk97(body, iBody, iLat, ANN);
        body[iBody].daPlanckAAnn[iLat] =
              fdOLRwk97(body, iBody, iLat, ANN) -
              body[iBody].daPlanckBAnn[iLat] * (body[iBody].daTempAnn[iLat]);
      } else {
        body[iBody].daPlanckBAnn[iLat] = body[iBody].dPlanckB;
        body[iBody].daPlanckAAnn[iLat] = body[iBody].dPlanckA;
      }

      if (body[iBody].bMEPDiff) {
        if (iLat == 0) {
          body[iBody].daDiffusionAnn[iLat] =
                body[iBody].daPlanckBAnn[iLat] / 4.0;
        } else if (iLat == body[iBody].iNumLats) {
          body[iBody].daDiffusionAnn[iLat] =
                body[iBody].daPlanckBAnn[iLat - 1] / 4.0;
        } else {
          body[iBody].daDiffusionAnn[iLat] =
                (body[iBody].daPlanckBAnn[iLat] +
                 body[iBody].daPlanckBAnn[iLat - 1]) /
                8.0;
        }
      } else if (body[iBody].bDiffRot) {
        body[iBody].daDiffusionAnn[iLat] =
              body[iBody].dDiffCoeff /
              (body[iBody].dRotRate * body[iBody].dRotRate /
               (4 * PI * PI / DAYSEC / DAYSEC));
      } else {
        body[iBody].daDiffusionAnn[iLat] = body[iBody].dDiffCoeff;
      }

      if (body[iBody].bHadley) {
        // XXX not self-consistent with rotation rate!
        dXBoundary = -1.0 + iLat * 2.0 / body[iBody].iNumLats;
        body[iBody].daDiffusionAnn[iLat] +=
              body[iBody].dDiffCoeff * 9. *
              exp(-pow((dXBoundary / sin(25. * DEGRAD)), 6));
      }
    }

    if (body[iBody].iClimateModel == SEA) {
      body[iBody].dSurfAlbedo =
            (body[iBody].dAlbedoLand + body[iBody].dAlbedoWater) / 2.0;
    }

    fvAlbedoAnnual(body, iBody);
    for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
      body[iBody].dAlbedoGlobal += body[iBody].daAlbedoAnn[iLat];
    }
    body[iBody].dAlbedoGlobal /= body[iBody].iNumLats;
    PoiseAnnual(body, iBody);
  }

  if (body[iBody].iClimateModel == SEA) {
    /* oh yeah, seasonal model, oh yeah!
    'tis the season to model ice sheets, fa la la la la, la la la la */

    /* First, if annual predict RGH or snowball, skip seasonal */
    if (body[iBody].bSkipSeasEnabled) {
      if (body[iBody].dFluxOutGlobal >= 300 ||
          body[iBody].dAlbedoGlobal >= body[iBody].dIceAlbedo) {
        body[iBody].bSkipSeas = 1;
      } else {
        body[iBody].bSkipSeas = 0;
      }
    } else {
      body[iBody].bSkipSeas = 0;
    }
    VerifyNStepSeasonal(body, iBody);
    body[iBody].dHeatCapWater *= body[iBody].dMixingDepth;
    body[iBody].dTGlobal    = 0.0;
    body[iBody].dSeasDeltax = 2.0 / body[iBody].iNumLats;
    body[iBody].dSeasDeltat = 1. / body[iBody].iNStepInYear;
    body[iBody].daXBoundary =
          malloc((body[iBody].iNumLats + 1) * sizeof(double));
    body[iBody].daTempLand     = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTempWater    = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTempLW       = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTempMaxLW    = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTempMaxLand  = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTempMaxWater = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTempMinLW    = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTempDaily   = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daFluxOutLand = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daFluxOutWater = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daFluxInLand   = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daFluxInWater  = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daSeaIceHeight = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daMEulerSea =
          malloc(2 * body[iBody].iNumLats * sizeof(double *));
    body[iBody].daMEulerCopySea =
          malloc(2 * body[iBody].iNumLats * sizeof(double *));
    body[iBody].daMInit   = malloc(2 * body[iBody].iNumLats * sizeof(double *));
    body[iBody].daInvMSea = malloc(2 * body[iBody].iNumLats * sizeof(double *));
    body[iBody].daSourceL = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daSourceW = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daSourceLW = malloc(2 * body[iBody].iNumLats * sizeof(double));
    body[iBody].daMLand    = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daMWater   = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daMDiffSea = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].iaRowswapSea = malloc(2 * body[iBody].iNumLats * sizeof(int));
    body[iBody].daScaleSea = malloc(2 * body[iBody].iNumLats * sizeof(double));
    body[iBody].daUnitVSea = malloc(2 * body[iBody].iNumLats * sizeof(double));
    body[iBody].daAlbedoLand  = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daAlbedoWater = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daLambdaSea =
          malloc((body[iBody].iNumLats + 1) * sizeof(double));
    body[iBody].daAlbedoLW = malloc(body[iBody].iNumLats * sizeof(double));
    // body[iBody].daAlbedoGlobalTmp =
    // malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daTmpTempSea =
          malloc(2 * body[iBody].iNumLats * sizeof(double));
    // body[iBody].daTGlobalTmp = malloc(body[iBody].iNDays*sizeof(double));
    body[iBody].daSeaIceHeight = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daSeaIceK      = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daFluxSeaIce   = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTempAvg      = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daAlbedoAvg    = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daFluxAvg      = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daFluxInAvg    = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daDivFluxAvg   = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daFluxOutAvg   = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTempAvgL     = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daAlbedoAvgL   = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daTempAvgW     = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daAlbedoAvgW   = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daFluxAvg      = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daFluxDaily   = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daFluxInAvg   = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daFluxInDaily = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daDivFluxAvg  = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daDivFluxDaily =
          malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daFluxOutAvg = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daFluxOutDaily =
          malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daIceBalance = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daIceMass    = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daIceBalanceAnnual =
          malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daIceMassTmp = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daSedShear   = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daBasalVel   = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daBasalFlow  = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daBasalFlowMid =
          malloc((body[iBody].iNumLats + 1) * sizeof(double));
    body[iBody].daIcePropsTmp = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daIceGamTmp   = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daIceSheetDiff =
          malloc((body[iBody].iNumLats + 1) * sizeof(double));
    body[iBody].daIceSheetMat = malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daIceBalanceTmp = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daYBoundary =
          malloc((body[iBody].iNumLats + 1) * sizeof(double));
    body[iBody].daIceBalanceAvg = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daIceFlowAvg    = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daBedrockH      = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daBedrockHEq    = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daPlanckASea    = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daPlanckBSea    = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daDiffusionSea =
          malloc((body[iBody].iNumLats + 1) * sizeof(double));
    body[iBody].daPlanckBDaily =
          malloc(body[iBody].iNumLats * sizeof(double *));
    body[iBody].daPlanckBAvg   = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daIceAccumTot  = malloc(body[iBody].iNumLats * sizeof(double));
    body[iBody].daIceAblateTot = malloc(body[iBody].iNumLats * sizeof(double));

    InitializeLandWater(body, iBody);
    body[iBody].dLatFHeatCp    = 83.5; // CC sez this is about right
    body[iBody].dLatentHeatIce = body[iBody].dHeatCapWater *
                                 body[iBody].dLatFHeatCp /
                                 body[iBody].dMixingDepth;

    for (iLat = 0; iLat <= body[iBody].iNumLats; iLat++) {
      if (iLat != body[iBody].iNumLats) {
        body[iBody].daTempLand[iLat] =
              (body[iBody].dTGlobalInit - 7.35) +
              20. * (1.0 - 2 * sin(body[iBody].daLats[iLat]) *
                                 sin(body[iBody].daLats[iLat])) +
              Toffset;
        body[iBody].daTempWater[iLat] =
              (body[iBody].dTGlobalInit - 7.35) +
              20. * (1.0 - 2 * sin(body[iBody].daLats[iLat]) *
                                 sin(body[iBody].daLats[iLat])) +
              Toffset;
        body[iBody].daTempLW[iLat] =
              body[iBody].daLandFrac[iLat] * body[iBody].daTempLand[iLat] +
              body[iBody].daWaterFrac[iLat] * body[iBody].daTempWater[iLat];
        if ((body[iBody].daTempWater[iLat] < body[iBody].dFrzTSeaIce) &&
            (body[iBody].bSeaIceModel)) {
          body[iBody].daSeaIceHeight[iLat] = 2.0;
        } else {
          body[iBody].daSeaIceHeight[iLat] = 0.0;
        }

        body[iBody].dTGlobal +=
              (body[iBody].daLandFrac[iLat] * body[iBody].daTempLand[iLat] +
               body[iBody].daWaterFrac[iLat] * body[iBody].daTempWater[iLat]) /
              body[iBody].iNumLats;
        body[iBody].daTempDaily[iLat] =
              malloc(body[iBody].iNumYears * body[iBody].iNStepInYear *
                     sizeof(double));
        body[iBody].daFluxDaily[iLat] =
              malloc(body[iBody].iNumYears * body[iBody].iNStepInYear *
                     sizeof(double));
        body[iBody].daFluxInDaily[iLat] =
              malloc(body[iBody].iNumYears * body[iBody].iNStepInYear *
                     sizeof(double));
        body[iBody].daFluxOutDaily[iLat] =
              malloc(body[iBody].iNumYears * body[iBody].iNStepInYear *
                     sizeof(double));
        body[iBody].daDivFluxDaily[iLat] =
              malloc(body[iBody].iNumYears * body[iBody].iNStepInYear *
                     sizeof(double));
        body[iBody].daIceBalance[iLat] =
              malloc(body[iBody].iNStepInYear * sizeof(double));
        body[iBody].daMLand[iLat] =
              malloc(body[iBody].iNumLats * sizeof(double));
        body[iBody].daMWater[iLat] =
              malloc(body[iBody].iNumLats * sizeof(double));
        body[iBody].daMDiffSea[iLat] =
              malloc(body[iBody].iNumLats * sizeof(double));
        body[iBody].daIceSheetMat[iLat] =
              malloc(body[iBody].iNumLats * sizeof(double));
        body[iBody].daInsol[iLat] = malloc(body[iBody].iNDays * sizeof(double));
        body[iBody].daPlanckBDaily[iLat] =
              malloc(body[iBody].iNumYears * body[iBody].iNStepInYear *
                     sizeof(double));

        /* Seasonal matrix is 2n x 2n to couple land and ocean */
        body[iBody].daMEulerSea[2 * iLat] =
              malloc(2 * body[iBody].iNumLats * sizeof(double));
        body[iBody].daMInit[2 * iLat] =
              malloc(2 * body[iBody].iNumLats * sizeof(double));
        body[iBody].daMEulerCopySea[2 * iLat] =
              malloc(2 * body[iBody].iNumLats * sizeof(double));
        body[iBody].daInvMSea[2 * iLat] =
              malloc(2 * body[iBody].iNumLats * sizeof(double));
        body[iBody].daMEulerSea[2 * iLat + 1] =
              malloc(2 * body[iBody].iNumLats * sizeof(double));
        body[iBody].daMInit[2 * iLat + 1] =
              malloc(2 * body[iBody].iNumLats * sizeof(double));
        body[iBody].daMEulerCopySea[2 * iLat + 1] =
              malloc(2 * body[iBody].iNumLats * sizeof(double));
        body[iBody].daInvMSea[2 * iLat + 1] =
              malloc(2 * body[iBody].iNumLats * sizeof(double));

        body[iBody].daIceMassTmp[iLat]    = 0.0;
        body[iBody].daIceBalanceAvg[iLat] = 0.0;
        body[iBody].daIceFlowAvg[iLat]    = 0.0;
        body[iBody].daBedrockH[iLat]      = 0.0;
        body[iBody].daBedrockHEq[iLat]    = 0.0;
        body[iBody].daIceHeight[iLat]     = 0.0;
        body[iBody].daDIceHeightDy[iLat]  = 0.0;

        body[iBody].daScaleSea[2 * iLat]     = 0.;
        body[iBody].daScaleSea[2 * iLat + 1] = 0.;
        for (jLat = 0; jLat < 2 * body[iBody].iNumLats; jLat++) {
          body[iBody].daMInit[2 * iLat][jLat]             = 0.;
          body[iBody].daMEulerSea[2 * iLat][jLat]         = 0.;
          body[iBody].daMEulerCopySea[2 * iLat][jLat]     = 0.;
          body[iBody].daMInit[2 * iLat + 1][jLat]         = 0.;
          body[iBody].daMEulerSea[2 * iLat + 1][jLat]     = 0.;
          body[iBody].daMEulerCopySea[2 * iLat + 1][jLat] = 0.;
        }

        if (body[iBody].bIceSheets) {
          if (fabs(body[iBody].daLats[iLat]) >=
              (body[iBody].dInitIceLat * DEGRAD)) {
            body[iBody].daBedrockH[iLat]   = 0.0;
            body[iBody].daBedrockHEq[iLat] = 0.0;
            body[iBody].daIceMass[iLat] = body[iBody].dInitIceHeight * RHOICE;
            // needs to be set if starting with ice, since this is the array
            // that fvAlbedoSeasonal uses
            body[iBody].daIceMassTmp[iLat] = body[iBody].daIceMass[iLat];
            // XXX only works if all lat cells are equal area!!
            body[iBody].dIceMassTot +=
                  body[iBody].daIceMass[iLat] *
                  (2 * PI * body[iBody].dRadius * body[iBody].dRadius *
                   (sin(body[iBody].daLats[1]) - sin(body[iBody].daLats[0])));
            //           body[iBody].daIceHeight[iLat] =
            //           body[iBody].dInitIceHeight;
          } else {
            body[iBody].daIceMass[iLat] = 0.0;
            //           body[iBody].daIceHeight[iLat] = 0.0;
          }
        }
        if (body[iBody].bCalcAB) {
          if (body[iBody].iOLRModel == WK97) {
            /* Calculate A and B from williams and kasting 97 result */
            body[iBody].daPlanckBSea[iLat] =
                  fdOLRdTwk97(body, iBody, iLat, SEA);
            body[iBody].daPlanckBAvg[iLat] = body[iBody].daPlanckBSea[iLat];
            body[iBody].daPlanckASea[iLat] =
                  fdOLRwk97(body, iBody, iLat, SEA) -
                  body[iBody].daPlanckBSea[iLat] * (body[iBody].daTempLW[iLat]);
          } else if (body[iBody].iOLRModel == HM16) {
            /* Calculate A and B from haqq-misra+ 2016 result */
            body[iBody].daPlanckBSea[iLat] =
                  fdOLRdThm16(body, iBody, iLat, SEA);
            body[iBody].daPlanckBAvg[iLat] = body[iBody].daPlanckBSea[iLat];
            body[iBody].daPlanckASea[iLat] =
                  fdOLRhm16(body, iBody, iLat, SEA) -
                  body[iBody].daPlanckBSea[iLat] * (body[iBody].daTempLW[iLat]);
          } else {
            /* Calculate A and B from haqq-misra+ 2016 result */
            body[iBody].daPlanckBSea[iLat] =
                  fdOLRdTsms09(body, iBody, iLat, SEA);
            body[iBody].daPlanckBAvg[iLat] = body[iBody].daPlanckBSea[iLat];
            body[iBody].daPlanckASea[iLat] =
                  fdOLRsms09(body, iBody, iLat, SEA) -
                  body[iBody].daPlanckBSea[iLat] * (body[iBody].daTempLW[iLat]);
          }
        } else {
          body[iBody].daPlanckBSea[iLat] = body[iBody].dPlanckB;
          body[iBody].daPlanckBAvg[iLat] = body[iBody].daPlanckBSea[iLat];
          body[iBody].daPlanckASea[iLat] = body[iBody].dPlanckA;
        }
      }

      if (body[iBody].bMEPDiff) {
        if (iLat == 0) {
          body[iBody].daDiffusionSea[iLat] =
                body[iBody].daPlanckBSea[iLat] / 4.0;
        } else if (iLat == body[iBody].iNumLats) {
          body[iBody].daDiffusionSea[iLat] =
                body[iBody].daPlanckBSea[iLat - 1] / 4.0;
        } else {
          body[iBody].daDiffusionSea[iLat] =
                (body[iBody].daPlanckBSea[iLat] +
                 body[iBody].daPlanckBSea[iLat - 1]) /
                8.0;
        }
      } else if (body[iBody].bDiffRot) {
        body[iBody].daDiffusionSea[iLat] =
              body[iBody].dDiffCoeff /
              (body[iBody].dRotRate * body[iBody].dRotRate /
               (4 * PI * PI / DAYSEC / DAYSEC));
      } else {
        body[iBody].daDiffusionSea[iLat] = body[iBody].dDiffCoeff;
      }

      body[iBody].daXBoundary[iLat] = -1.0 + iLat * 2.0 / body[iBody].iNumLats;
      body[iBody].daYBoundary[iLat] =
            body[iBody].dRadius * body[iBody].dSeasDeltax /
            sqrt(1.0 -
                 body[iBody].daXBoundary[iLat] * body[iBody].daXBoundary[iLat]);
      if (body[iBody].bHadley) {
        // XXX not self-consistent with rotation rate!
        body[iBody].daDiffusionSea[iLat] +=
              body[iBody].dDiffCoeff * 9. *
              exp(-pow((body[iBody].daXBoundary[iLat] / sin(25. * DEGRAD)), 6));
      }
    }

    if (body[iBody].bSkipSeas == 0) {
      fvAnnualInsolation(body, iBody);
      /* since this is executed only once, we need to multiply dAlbedoGlobal
         by iNStepInYear below */
      fvAlbedoSeasonal(body, iBody, 0);
      body[iBody].dAlbedoGlobal =
            body[iBody].dAlbedoGlobal * body[iBody].iNStepInYear;
      fvMatrixSeasonal(body, iBody);
      fvSourceFSeas(body, iBody, 0);
      if (body[iBody].bSeaIceModel) {
        fvSeaIce(body, iBody);
      }

      /* "burn in" to a quasi equilibrium */
      // TGlobalTmp = 0;
      /* This was old code -- Is it needed?
      while (fabs(TGlobalTmp - body[iBody].dTGlobal) > 0.01) {
        TGlobalTmp = body[iBody].dTGlobal;
        PoiseSeasonal(body,iBody);
        printf("TGlobal = %f\n",TGlobalTmp);
      */
      count                 = 0;
      int RunLen            = 5;
      int iMaxIteration     = 2 * RunLen;
      daRunningMean         = malloc((RunLen + 1) * sizeof(double));
      daRunningMean[RunLen] = 0;
      RunningMeanTmp        = daRunningMean[RunLen];
      while (fabs(RunningMeanTmp - daRunningMean[RunLen]) >
                   body[iBody].dSpinUpTol ||
             count <= iMaxIteration) {
        RunningMeanTmp = daRunningMean[RunLen];
        PoiseSeasonal(body, iBody);
        fvMatrixSeasonal(body, iBody);
        fvSnowball(body, iBody);

        if (count < RunLen) {
          daRunningMean[count] = body[iBody].dTGlobal;
          if (iVerbose >= VERBINPUT) {
            printf("TGlobal = %f\n", daRunningMean[count]);
          }
        } else {
          //           DampTemp(body,RunningMeanTmp,iBody);
          daRunningMean[RunLen] = 0;
          for (iRun = 0; iRun < RunLen - 1; iRun++) {
            daRunningMean[iRun] = daRunningMean[iRun + 1];
            daRunningMean[RunLen] += daRunningMean[iRun];
          }
          daRunningMean[RunLen - 1] = body[iBody].dTGlobal;
          daRunningMean[RunLen] += daRunningMean[RunLen - 1];
          daRunningMean[RunLen] /= RunLen;
          //           TotalMean *= (float)(count-RunLen)/(count-RunLen+1);
          //           TotalMean += daRunningMean[RunLen-1]/(count-RunLen+1);
          if (iVerbose >= VERBINPUT) {
            printf("TGlobal = %f; Prev RunningMean = %f; Curr RunningMean = "
                   "%f\n",
                   daRunningMean[RunLen - 1], RunningMeanTmp,
                   daRunningMean[RunLen]);
          }
        }
        count += 1;
        /*
        if (count >= iMaxIteration) {
          if (iVerbose > VERBPROG) {
            fprintf(stderr,"ERROR: Initial climate state failed to
        converge.\n");
          }
          exit(EXIT_INPUT);
        }
        */
      }
      free(daRunningMean);

    } else if (body[iBody].bSkipSeas == 1) {
      printf("Planet started in RGH or snowball, skipping Seasonal model\n");
    }
  }
}

void VerifyAstro(BODY *body, OPTIONS *options, char cFile[], int iBody,
                 int iVerbose) {
  if (body[iBody].bEqtide == 0) {
    if (body[iBody].bDistOrb == 0) {
      CalcHK(body, iBody);
    }
    if (body[iBody].bDistRot == 0) {
      CalcXYZobl(body, iBody);
    }
  }
  if (body[iBody].bForceObliq == 1) {
    if (body[iBody].bDistRot == 1) { // XXX Can't reference other modules here!
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Cannot set %s == 1 when using DistRot in File:\
                %s\n",
                options[OPT_FORCEOBLIQ].cName, cFile);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    } else {
      body[iBody].dObliq0 = body[iBody].dObliquity;
    }
    if (body[iBody].dObliqAmp * DEGRAD > 2 * body[iBody].dObliq0) {
      fprintf(stderr, "ERROR: %s > (2 x %s) is not allowed.\n",
              options[OPT_OBLIQAMP].cName, options[OPT_OBL].cName);
      DoubleLineExit(cFile, cFile, options[OPT_OBLIQAMP].iLine[iBody + 1],
                     options[OPT_OBL].iLine[iBody + 1]);
    }
  }
  if (body[iBody].bForceEcc == 1) {
    if (body[iBody].bDistRot == 1) {
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Cannot set %s == 1 when using DistRot in File:\
                %s\n",
                options[OPT_FORCEECC].cName, cFile);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    } else {
      body[iBody].dEcc0 = body[iBody].dEcc;
    }
    if (body[iBody].dEccAmp > 2 * body[iBody].dEcc) {
      fprintf(stderr, "ERROR: %s > (2 x %s) is not allowed.\n",
              options[OPT_ECCAMP].cName, options[OPT_ORBECC].cName);
      DoubleLineExit(cFile, cFile, options[OPT_ECCAMP].iLine[iBody + 1],
                     options[OPT_ORBECC].iLine[iBody + 1]);
    }
    if (body[iBody].dEcc + 0.5 * body[iBody].dEccAmp >= 1) {
      if (iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: The sum of %s and %s must be less than 1.",
                options[OPT_ECCAMP].cName, options[OPT_ORBECC].cName);
      }
      DoubleLineExit(cFile, cFile, options[OPT_ECCAMP].iLine[iBody + 1],
                     options[OPT_ORBECC].iLine[iBody + 1]);
    }
  }
}

void VerifyDiffusion(BODY *body, OPTIONS *options, char cFile[], int iBody,
                     int iVerbose) {
  if (body[iBody].bMEPDiff) {
    if (options[OPT_DIFFUSION].iLine[iBody + 1] > -1) {
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Cannot set %s when setting bMEPDiff = 1 in "
                "File:%s\n",
                options[OPT_DIFFUSION].cName, cFile);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }
    if (body[iBody].bHadley) {
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Cannot set both bHadley = 1 and bMEPDiff = 1 "
                "in File:%s\n",
                cFile);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }
  }
}

void VerifySeasOutputTime(BODY *body, CONTROL *control, OPTIONS *options,
                          char cFile[], int iBody, int iVerbose) {
  if (body[iBody].dSeasOutputTime != 0) {
    if (body[iBody].dSeasOutputTime < control->Io.dOutputTime) {
      // LCOV_EXCL_START
      if (iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: %s in file %s must be greater than or equal to"
                "%s \n",
                options[OPT_SEASOUTPUTTIME].cName, cFile,
                options[OPT_OUTPUTTIME].cName);
      }
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }
  }
}

void AssignPoiseDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                            fnUpdateVariable ***fnUpdate, int iBody) {
  //  Nothing here, because entire climate simulation is ran in ForceBehavior
}

void NullPoiseDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                          fnUpdateVariable ***fnUpdate, int iBody) {
  //  Nothing here, because entire climate simulation is ran in ForceBehavior
}

void VerifyPoise(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 OUTPUT *output, SYSTEM *system, UPDATE *update, int iBody,
                 int iModule) {
  VerifyAlbedo(body, options, files->Infile[iBody + 1].cIn, iBody,
               control->Io.iVerbose);
  VerifyAstro(body, options, files->Infile[iBody + 1].cIn, iBody,
              control->Io.iVerbose);
  VerifyOLR(body, options, files->Infile[iBody + 1].cIn, iBody,
            control->Io.iVerbose);
  VerifyDiffusion(body, options, files->Infile[iBody + 1].cIn, iBody,
                  control->Io.iVerbose);
  VerifySeasOutputTime(body, control, options, files->Infile[iBody + 1].cIn,
                       iBody, control->Io.iVerbose);
  if (body[iBody].bDistRot == 0) {
    VerifyDynEllip(body, control, options, files->Infile[iBody + 1].cIn, iBody,
                   control->Io.iVerbose);
  }

  /* Initialize climate arrays */
  InitializeLatGrid(body, iBody);
  InitializeClimateParams(body, iBody, control->Io.iVerbose);

  /* The climate simulation is done entirely in ForceBehavior. */
  if (body[iBody].bIceSheets) {
    VerifyIceSheets(body, options, files->Infile[iBody + 1].cIn, iBody,
                    control->Io.iVerbose);
  }

  control->fnPropsAux[iBody][iModule] = &fvPropsAuxPoise;

  control->fnForceBehavior[iBody][iModule]   = &ForceBehaviorPoise;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyPoise;
}


/***************** POISE Update *****************/
void InitializeUpdatePoise(BODY *body, UPDATE *update, int iBody) {
}

void FinalizeUpdateIceMassPoise(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iLat) {
}

/***************** POISE Halts *****************/

void CountHaltsPoise(HALT *halt, int *iNumHalts) {
}

void VerifyHaltPoise(BODY *body, CONTROL *control, OPTIONS *options, int iBody,
                     int *iHalt) {
}

/* Minimum ice flow time-step? */
// int HaltMinIceDt(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,
//                  int iBody) {
// }

/************* POISE Outputs ******************/

void WriteTGlobal(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {
  /* Get TGlobal */
  *dTmp = body[iBody].dTGlobal;
  if (output->bDoNeg[iBody]) {
    /* Units already in Celsius (POISE uses Celsius) */
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp = fdUnitsTemp(*dTmp, U_CELSIUS, U_KELVIN);
    fsUnitsTime(0, cUnit);
  }
}

void WriteAlbedoGlobal(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  /* Get AlbedoGlobal */
  *dTmp = body[iBody].dAlbedoGlobal;
  strcpy(cUnit, "");
}

void WriteSnowball(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {
  /* Get snowball status */
  *dTmp = (double)body[iBody].bSnowball;
  strcpy(cUnit, "");
}

void WriteIceCapNorthLand(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  int iLatIceEdge, bCap;
  double dLat;

  fvNorthIceCapLand(body, iBody, &dLat, &iLatIceEdge, &bCap);

  *dTmp = (double)bCap;
  strcpy(cUnit, "");
}

void WriteIceCapNorthLatLand(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  int iLatIceEdge, bCap;
  double dLat;

  fvNorthIceCapLand(body, iBody, &dLat, &iLatIceEdge, &bCap);

  *dTmp = dLat;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteIceCapNorthSea(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  int iLatIceEdge, bCap;
  double dLat;

  fvNorthIceCapSea(body, iBody, &dLat, &iLatIceEdge, &bCap);
  *dTmp = (double)bCap;
  strcpy(cUnit, "");
}

void WriteIceCapNorthLatSea(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  int iLatIceEdge, bCap;
  double dLat;

  fvNorthIceCapSea(body, iBody, &dLat, &iLatIceEdge, &bCap);

  *dTmp = dLat;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteIceCapSouthLand(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  int iLatIceEdge, bCap;
  double dLat;

  fvSouthIceCapLand(body, iBody, &dLat, &iLatIceEdge, &bCap);
  *dTmp = (double)bCap;
  strcpy(cUnit, "");
}

void WriteIceCapSouthLatLand(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  int iLatIceEdge, bCap;
  double dLat;

  fvSouthIceCapLand(body, iBody, &dLat, &iLatIceEdge, &bCap);

  *dTmp = dLat;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteIceCapSouthSea(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  int iLatIceEdge, bCap;
  double dLat;

  fvSouthIceCapSea(body, iBody, &dLat, &iLatIceEdge, &bCap);
  *dTmp = (double)bCap;
  strcpy(cUnit, "");
}

void WriteIceCapSouthLatSea(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {

  int iLatIceEdge, bCap;
  double dLat;

  fvSouthIceCapSea(body, iBody, &dLat, &iLatIceEdge, &bCap);

  *dTmp = dLat;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteIceBeltLand(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  int iLatNorth, iLatSouth, bBelt;
  double dLatNorth, dLatSouth;

  fvIceBeltLand(body, iBody, &dLatNorth, &dLatSouth, &iLatNorth, &iLatSouth,
                &bBelt);

  *dTmp = (double)bBelt;
  strcpy(cUnit, "");
}

void WriteIceBeltNorthLatLand(BODY *body, CONTROL *control, OUTPUT *output,
                              SYSTEM *system, UNITS *units, UPDATE *update,
                              int iBody, double *dTmp, char cUnit[]) {
  int iLatNorth, iLatSouth, bBelt;
  double dLatNorth, dLatSouth;

  fvIceBeltLand(body, iBody, &dLatNorth, &dLatSouth, &iLatNorth, &iLatSouth,
                &bBelt);

  *dTmp = dLatNorth;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteIceBeltSouthLatLand(BODY *body, CONTROL *control, OUTPUT *output,
                              SYSTEM *system, UNITS *units, UPDATE *update,
                              int iBody, double *dTmp, char cUnit[]) {
  int iLatNorth, iLatSouth, bBelt;
  double dLatSouth, dLatNorth;

  fvIceBeltLand(body, iBody, &dLatNorth, &dLatSouth, &iLatNorth, &iLatSouth,
                &bBelt);

  *dTmp = dLatSouth;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteIceBeltSea(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {
  int iLatNorth, iLatSouth, bBelt;
  double dLatNorth, dLatSouth;

  fvIceBeltSea(body, iBody, &dLatNorth, &dLatSouth, &iLatNorth, &iLatSouth,
               &bBelt);

  *dTmp = (double)bBelt;
  strcpy(cUnit, "");
}

void WriteIceBeltNorthLatSea(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  int iLatNorth, iLatSouth, bBelt;
  double dLatNorth, dLatSouth;

  fvIceBeltSea(body, iBody, &dLatNorth, &dLatSouth, &iLatNorth, &iLatSouth,
               &bBelt);

  *dTmp = dLatNorth;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteIceBeltSouthLatSea(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  int iLatNorth, iLatSouth, bBelt;
  double dLatNorth, dLatSouth;

  fvIceBeltSea(body, iBody, &dLatNorth, &dLatSouth, &iLatNorth, &iLatSouth,
               &bBelt);

  *dTmp = dLatSouth;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteSnowballLand(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {

  *dTmp = (double)fbSnowballLand(body, iBody);
  strcpy(cUnit, "");
}

void WriteSnowballSea(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {

  *dTmp = (double)fbSnowballSea(body, iBody);
  strcpy(cUnit, "");
}

void WriteIceFree(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {

  *dTmp = (double)fbIceFree(body, iBody);
  strcpy(cUnit, "");
}

void WriteSkipSeas(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {
  /* Get AlbedoGlobal */
  *dTmp = body[iBody].bSkipSeas;
  strcpy(cUnit, "");
}

void WriteTempLat(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daTempAnn[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daTempAvg[body[iBody].iWriteLat];
  }

  if (output->bDoNeg[iBody]) {
    /* Units already in Celsius (POISE uses Celsius) */
    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp = fdUnitsTemp(*dTmp, U_CELSIUS, U_KELVIN);
    fsUnitsTime(0, cUnit);
  }
}

void WriteTempMinLat(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daTempAnn[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daTempMinLW[body[iBody].iWriteLat];
  }

  if (output->bDoNeg[iBody]) {

    /* Units already in Celsius (POISE uses Celsius) */
    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp = fdUnitsTemp(*dTmp, U_CELSIUS, U_KELVIN);
    fsUnitsTime(0, cUnit);
  }
}

void WriteTempMaxLat(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daTempAnn[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daTempMaxLW[body[iBody].iWriteLat];
  }

  if (output->bDoNeg[iBody]) {
    /* Units already in Celsius (POISE uses Celsius) */
    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp = fdUnitsTemp(*dTmp, U_CELSIUS, U_KELVIN);
    fsUnitsTime(0, cUnit);
  }
}

void WriteTempMaxLand(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daTempAnn[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daTempMaxLand[body[iBody].iWriteLat];
  }

  if (output->bDoNeg[iBody]) {
    /* Units already in Celsius (POISE uses Celsius) */
    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp = fdUnitsTemp(*dTmp, U_CELSIUS, U_KELVIN);
    fsUnitsTime(0, cUnit);
  }
}

void WriteTempMaxWater(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daTempAnn[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daTempMaxWater[body[iBody].iWriteLat];
  }

  if (output->bDoNeg[iBody]) {
    /* Units already in Celsius (POISE uses Celsius) */
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp = fdUnitsTemp(*dTmp, U_CELSIUS, U_KELVIN);
    fsUnitsTime(0, cUnit);
  }
}

void WriteTempLandLat(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daTempAnn[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daTempAvgL[body[iBody].iWriteLat];
  }

  if (output->bDoNeg[iBody]) {
    /* Units already in Celsius (POISE uses Celsius) */
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp = fdUnitsTemp(*dTmp, U_CELSIUS, U_KELVIN);
    fsUnitsTime(0, cUnit);
  }
}

void WriteTempWaterLat(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daTempAnn[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daTempAvgW[body[iBody].iWriteLat];
  }

  if (output->bDoNeg[iBody]) {
    /* Units already in Celsius (POISE uses Celsius) */
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp = fdUnitsTemp(*dTmp, U_CELSIUS, U_KELVIN);
    fsUnitsTime(0, cUnit);
  }
}

void WriteLatitude(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  *dTmp = body[iBody].daLats[body[iBody].iWriteLat];

  if (output->bDoNeg[iBody]) {

    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteAlbedoLat(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daAlbedoAnn[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daAlbedoAvg[body[iBody].iWriteLat];
  }

  strcpy(cUnit, "");
}

void WriteAlbedoLandLat(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daAlbedoAnn[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daAlbedoAvgL[body[iBody].iWriteLat];
  }
  strcpy(cUnit, "");
}

void WriteAlbedoWaterLat(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daAlbedoAnn[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daAlbedoAvgW[body[iBody].iWriteLat];
  }
  strcpy(cUnit, "");
}

void WriteFluxInGlobal(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dFluxInGlobal;

  if (output->bDoNeg[iBody]) {
    // Negative option is SI
    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

void WriteFluxOutGlobal(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dFluxOutGlobal;

  if (output->bDoNeg[iBody]) {

    // Negative option is SI
    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

void WriteTotIceMass(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dIceMassTot;

  if (output->bDoNeg[iBody]) {

    // Negative option is SI
    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

void WriteIceFlowTot(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dIceFlowTot;

  if (output->bDoNeg[iBody]) {
    // Negative option is SI
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

void WriteAreaIceCov(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {

  fvAreaIceCovered(body, iBody);
  *dTmp = body[iBody].dAreaIceCov;

  // if (output->bDoNeg[iBody]) {
  //     // Negative option is SI
  //     strcpy(cUnit,output->cNeg);
  //   } else {
  //     *dTmp /= fdUnitsMass(units->iMass);
  //     fsUnitsMass(units->iMass,cUnit);
  //   }
}

void WriteIceBalanceTot(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dIceBalanceTot;

  if (output->bDoNeg[iBody]) {

    // Negative option is SI
    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

void WriteAnnualInsol(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].daAnnualInsol[body[iBody].iWriteLat];

  if (output->bDoNeg[iBody]) {

    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

void WritePeakInsol(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].daPeakInsol[body[iBody].iWriteLat];

  if (output->bDoNeg[iBody]) {

    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

void WriteDailyInsol(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {

  char cOut[3 * NAMELEN];
  FILE *fp;
  int iLat, iDay;
  double dTime;

  struct stat st = {0};
  if (stat("SeasonalClimateFiles", &st) == -1) {

#ifdef _WIN32
    mkdir("SeasonalClimateFiles");
#else
    mkdir("SeasonalClimateFiles", 0700);
#endif
  }

  dTime = control->Evolve.dTime / fdUnitsTime(units->iTime);

  if (dTime == 0) {

    sprintf(cOut, "SeasonalClimateFiles/%s.%s.DailyInsol.0", system->cName,
            body[iBody].cName);

  } else if (dTime < 10000) {

    sprintf(cOut, "SeasonalClimateFiles/%s.%s.DailyInsol.%.0f", system->cName,
            body[iBody].cName, dTime);

  } else {

    sprintf(cOut, "SeasonalClimateFiles/%s.%s.DailyInsol.%.2e", system->cName,
            body[iBody].cName, dTime);
  }

  fp = fopen(cOut, "w");
  for (iDay = 0; iDay < body[iBody].iNDays; iDay++) {

    for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {

      fprintd(fp, body[iBody].daInsol[iLat][iDay], control->Io.iSciNot,
              control->Io.iDigits);
      fprintf(fp, " ");
    }

    fprintf(fp, "\n");
  }

  fclose(fp);
}

void WritePlanckB(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {

  char cOut[3 * NAMELEN];
  FILE *fp;
  int iLat, iDay;
  double dTime;

  struct stat st = {0};
  if (stat("SeasonalClimateFiles", &st) == -1) {

#ifdef _WIN32
    mkdir("SeasonalClimateFiles");
#else
    mkdir("SeasonalClimateFiles", 0700);
#endif
  }

  dTime = control->Evolve.dTime / fdUnitsTime(units->iTime);

  if (dTime == 0) {

    sprintf(cOut, "SeasonalClimateFiles/%s.%s.PlanckB.0", system->cName,
            body[iBody].cName);

  } else if (dTime < 10000) {

    sprintf(cOut, "SeasonalClimateFiles/%s.%s.PlanckB.%.0f", system->cName,
            body[iBody].cName, dTime);

  } else {

    sprintf(cOut, "SeasonalClimateFiles/%s.%s.PlanckB.%.2e", system->cName,
            body[iBody].cName, dTime);
  }

  fp = fopen(cOut, "w");
  for (iDay = 0; iDay < body[iBody].iNumYears * body[iBody].iNStepInYear;
       iDay++) {

    for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {

      fprintd(fp, body[iBody].daPlanckBDaily[iLat][iDay], control->Io.iSciNot,
              control->Io.iDigits);
      fprintf(fp, " ");
    }

    fprintf(fp, "\n");
  }

  fclose(fp);
}

void WriteSeasonalTemp(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {

  char cOut[3 * NAMELEN];
  FILE *fp;
  int iLat, iDay;
  double dTime;

  struct stat st = {0};
  if (stat("SeasonalClimateFiles", &st) == -1) {

#ifdef _WIN32
    mkdir("SeasonalClimateFiles");
#else
    mkdir("SeasonalClimateFiles", 0700);
#endif
  }

  dTime = control->Evolve.dTime / fdUnitsTime(units->iTime);

  if (dTime == 0) {

    sprintf(cOut, "SeasonalClimateFiles/%s.%s.SeasonalTemp.0", system->cName,
            body[iBody].cName);

  } else if (dTime < 10000) {

    sprintf(cOut, "SeasonalClimateFiles/%s.%s.SeasonalTemp.%.0f", system->cName,
            body[iBody].cName, dTime);

  } else {

    sprintf(cOut, "SeasonalClimateFiles/%s.%s.SeasonalTemp.%.2e", system->cName,
            body[iBody].cName, dTime);
  }

  fp = fopen(cOut, "w");
  for (iDay = 0; iDay < body[iBody].iNumYears * body[iBody].iNStepInYear;
       iDay++) {

    for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {

      // printf("%d %d\n",iLat,iDay);
      // printf("%d %d %lf\n",iLat,iDay,body[iBody].daTempDaily[iLat][iDay]);
      fprintd(fp, body[iBody].daTempDaily[iLat][iDay], control->Io.iSciNot,
              control->Io.iDigits);
      fprintf(fp, " ");
    }

    fprintf(fp, "\n");
  }

  fclose(fp);
}

void WriteSeasonalFluxes(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {

  char cOutM[3 * NAMELEN], cOutI[3 * NAMELEN], cOutO[3 * NAMELEN],
        cOutD[3 * NAMELEN];
  FILE *fpM, *fpI, *fpO, *fpD;
  int iLat, iDay;
  double dTime;

  struct stat st = {0};
  if (stat("SeasonalClimateFiles", &st) == -1) {

#ifdef _WIN32
    mkdir("SeasonalClimateFiles");
#else
    mkdir("SeasonalClimateFiles", 0700);
#endif
  }

  dTime = control->Evolve.dTime / fdUnitsTime(units->iTime);

  if (dTime == 0) {

    sprintf(cOutM, "SeasonalClimateFiles/%s.%s.SeasonalFMerid.0", system->cName,
            body[iBody].cName);
    sprintf(cOutI, "SeasonalClimateFiles/%s.%s.SeasonalFIn.0", system->cName,
            body[iBody].cName);
    sprintf(cOutO, "SeasonalClimateFiles/%s.%s.SeasonalFOut.0", system->cName,
            body[iBody].cName);
    sprintf(cOutD, "SeasonalClimateFiles/%s.%s.SeasonalDivF.0", system->cName,
            body[iBody].cName);

  } else if (dTime < 10000) {

    sprintf(cOutM, "SeasonalClimateFiles/%s.%s.SeasonalFMerid.%.0f",
            system->cName, body[iBody].cName, dTime);
    sprintf(cOutI, "SeasonalClimateFiles/%s.%s.SeasonalFIn.%.0f", system->cName,
            body[iBody].cName, dTime);
    sprintf(cOutO, "SeasonalClimateFiles/%s.%s.SeasonalFOut.%.0f",
            system->cName, body[iBody].cName, dTime);
    sprintf(cOutD, "SeasonalClimateFiles/%s.%s.SeasonalDivF.%.0f",
            system->cName, body[iBody].cName, dTime);

  } else {

    sprintf(cOutM, "SeasonalClimateFiles/%s.%s.SeasonalFMerid.%.2e",
            system->cName, body[iBody].cName, dTime);
    sprintf(cOutI, "SeasonalClimateFiles/%s.%s.SeasonalFIn.%.2e", system->cName,
            body[iBody].cName, dTime);
    sprintf(cOutO, "SeasonalClimateFiles/%s.%s.SeasonalFOut.%.2e",
            system->cName, body[iBody].cName, dTime);
    sprintf(cOutD, "SeasonalClimateFiles/%s.%s.SeasonalDivF.%.2e",
            system->cName, body[iBody].cName, dTime);
  }

  fpM = fopen(cOutM, "w");
  fpI = fopen(cOutI, "w");
  fpO = fopen(cOutO, "w");
  fpD = fopen(cOutD, "w");

  for (iDay = 0; iDay < body[iBody].iNumYears * body[iBody].iNStepInYear;
       iDay++) {

    for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {

      // printf("%d %d\n",iLat,iDay);
      // printf("%d %d %lf\n",iLat,iDay,body[iBody].daTempDaily[iLat][iDay]);
      fprintd(fpM, body[iBody].daFluxDaily[iLat][iDay], control->Io.iSciNot,
              control->Io.iDigits);
      fprintf(fpM, " ");

      fprintd(fpI, body[iBody].daFluxInDaily[iLat][iDay], control->Io.iSciNot,
              control->Io.iDigits);
      fprintf(fpI, " ");

      fprintd(fpO, body[iBody].daFluxOutDaily[iLat][iDay], control->Io.iSciNot,
              control->Io.iDigits);
      fprintf(fpO, " ");

      fprintd(fpD, body[iBody].daDivFluxDaily[iLat][iDay], control->Io.iSciNot,
              control->Io.iDigits);
      fprintf(fpD, " ");
    }

    fprintf(fpM, "\n");
    fprintf(fpI, "\n");
    fprintf(fpO, "\n");
    fprintf(fpD, "\n");
  }

  fclose(fpM);
  fclose(fpI);
  fclose(fpO);
  fclose(fpD);
}

void WriteSeasonalIceBalance(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {

  char cOut[3 * NAMELEN];
  FILE *fp;
  int iLat, iDay;
  double dTime;

  struct stat st = {0};
  if (stat("SeasonalClimateFiles", &st) == -1) {

#ifdef _WIN32
    mkdir("SeasonalClimateFiles");
#else
    mkdir("SeasonalClimateFiles", 0700);
#endif
  }

  dTime = control->Evolve.dTime / fdUnitsTime(units->iTime);

  if (dTime == 0) {

    sprintf(cOut, "SeasonalClimateFiles/%s.%s.SeasonalIceBalance.0",
            system->cName, body[iBody].cName);

  } else if (dTime < 10000) {

    sprintf(cOut, "SeasonalClimateFiles/%s.%s.SeasonalIceBalance.%.0f",
            system->cName, body[iBody].cName, dTime);

  } else {

    sprintf(cOut, "SeasonalClimateFiles/%s.%s.SeasonalIceBalance.%.2e",
            system->cName, body[iBody].cName, dTime);
  }

  fp = fopen(cOut, "w");
  for (iDay = 0; iDay < body[iBody].iNStepInYear; iDay++) {

    for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {

      fprintd(fp, body[iBody].daIceBalance[iLat][iDay], control->Io.iSciNot,
              control->Io.iDigits);
      fprintf(fp, " ");
    }

    fprintf(fp, "\n");
  }

  fclose(fp);
}

void WriteFluxMerid(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daFlux[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daFluxAvg[body[iBody].iWriteLat];
  }

  if (output->bDoNeg[iBody]) {

    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}

void WriteFluxIn(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                 UNITS *units, UPDATE *update, int iBody, double *dTmp,
                 char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daFluxIn[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daFluxInAvg[body[iBody].iWriteLat];
  }

  if (output->bDoNeg[iBody]) {

    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

void WriteFluxOut(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {

    *dTmp = body[iBody].daFluxOut[body[iBody].iWriteLat];

  } else if (body[iBody].iClimateModel == SEA) {

    *dTmp = body[iBody].daFluxOutAvg[body[iBody].iWriteLat];
  }

  if (output->bDoNeg[iBody]) {

    strcpy(cUnit, output->cNeg);

  } else {

    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

void WriteDivFlux(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeas == 1) {
    *dTmp = body[iBody].daDivFlux[body[iBody].iWriteLat];
  } else if (body[iBody].iClimateModel == SEA) {
    *dTmp = body[iBody].daDivFluxAvg[body[iBody].iWriteLat];
  }
  if (output->bDoNeg[iBody]) {
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

void WriteIceMass(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UNITS *units, UPDATE *update, int iBody, double *dTmp,
                  char cUnit[]) {

  if (body[iBody].bIceSheets) {
    *dTmp = body[iBody].daIceMass[body[iBody].iWriteLat];

  } else {
    *dTmp = 0.0;
  }

  if (output->bDoNeg[iBody]) {
    strcpy(cUnit, output->cNeg);

  } else {
    //*dTmp /= fdUnitsMass(units->iMass)/pow(fdUnitsLength(units->iLength),2);
    // fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteIceHeight(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {

  if (body[iBody].bIceSheets) {
    *dTmp = body[iBody].daIceMass[body[iBody].iWriteLat] / RHOICE;
  } else {
    *dTmp = 0.0;
  }

  if (output->bDoNeg[iBody]) {
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

void WriteBedrockH(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  if (body[iBody].bIceSheets) {
    *dTmp = body[iBody].daBedrockH[body[iBody].iWriteLat];
  } else {
    *dTmp = 0.0;
  }

  if (output->bDoNeg[iBody]) {
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

void WritePlanckBAvg(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {

  if (body[iBody].bIceSheets) {
    *dTmp = body[iBody].daPlanckBAvg[body[iBody].iWriteLat];
  } else {
    *dTmp = 0.0;
  }

  // if (output->bDoNeg[iBody]) {
  //     strcpy(cUnit,output->cNeg);
  //   } else {
  //     *dTmp /= fdUnitsLength(units->iLength);
  //     fsUnitsLength(units->iLength,cUnit);
  //   }
}

void WriteDIceMassDt(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {

  //*dTmp = body[iBody].daIceBalanceAnnual[body[iBody].iWriteLat];
  if (body[iBody].bIceSheets) {
    *dTmp = body[iBody].daIceBalanceAvg[body[iBody].iWriteLat] * RHOICE;
  } else {
    *dTmp = 0.0;
  }

  if (output->bDoNeg[iBody]) {
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

void WriteIceAccum(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  //*dTmp = body[iBody].daIceBalanceAnnual[body[iBody].iWriteLat];
  if (body[iBody].bIceSheets) {
    *dTmp = body[iBody].daIceAccumTot[body[iBody].iWriteLat] / RHOICE;
  } else {
    *dTmp = 0.0;
  }

  // if (output->bDoNeg[iBody]) {
  //     strcpy(cUnit,output->cNeg);
  //   } else {
  //     *dTmp /= fdUnitsLength(units->iLength);
  //     fsUnitsLength(units->iLength,cUnit);
  //   }
}

void WriteIceAblate(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {

  //*dTmp = body[iBody].daIceBalanceAnnual[body[iBody].iWriteLat];
  if (body[iBody].bIceSheets) {
    *dTmp = body[iBody].daIceAblateTot[body[iBody].iWriteLat] / RHOICE;
  } else {
    *dTmp = 0.0;
  }

  // if (output->bDoNeg[iBody]) {
  //     strcpy(cUnit,output->cNeg);
  //   } else {
  //     *dTmp /= fdUnitsLength(units->iLength);
  //     fsUnitsLength(units->iLength,cUnit);
  //   }
}


void WriteDIceMassDtFlow(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {

  // *dTmp = body[iBody].daIceBalanceAnnual[body[iBody].iWriteLat];
  if (body[iBody].bIceSheets) {
    *dTmp = body[iBody].daIceFlowAvg[body[iBody].iWriteLat];
  } else {
    *dTmp = 0.0;
  }

  if (output->bDoNeg[iBody]) {
    strcpy(cUnit, output->cNeg);
  } else {
    // *dTmp /= fdUnitsMass(units->iMass)/pow(fdUnitsLength(units->iLength),2);
    // fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteEnergyResL(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].daEnergyResL[body[iBody].iWriteLat];

  if (output->bDoNeg[iBody]) {
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

void WriteEnergyResW(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].daEnergyResW[body[iBody].iWriteLat];

  if (output->bDoNeg[iBody]) {
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

void InitializeOutputPoise(OUTPUT *output, fnWriteOutput fnWrite[]) {
  sprintf(output[OUT_TGLOBAL].cName, "TGlobal");
  sprintf(output[OUT_TGLOBAL].cDescr, "Global mean temperature from POISE");
  sprintf(output[OUT_TGLOBAL].cNeg, "Celsius");
  output[OUT_TGLOBAL].bNeg = 1;
  // conversion is hardcoded in write function
  output[OUT_TGLOBAL].dNeg       = 1;
  output[OUT_TGLOBAL].iNum       = 1;
  output[OUT_TGLOBAL].iModuleBit = POISE;
  fnWrite[OUT_TGLOBAL]           = &WriteTGlobal;

  sprintf(output[OUT_ALBEDOGLOBAL].cName, "AlbedoGlobal");
  sprintf(output[OUT_ALBEDOGLOBAL].cDescr,
          "Global mean bond albedo from POISE");
  output[OUT_ALBEDOGLOBAL].bNeg       = 0;
  output[OUT_ALBEDOGLOBAL].iNum       = 1;
  output[OUT_ALBEDOGLOBAL].iModuleBit = POISE;
  fnWrite[OUT_ALBEDOGLOBAL]           = &WriteAlbedoGlobal;

  sprintf(output[OUT_SNOWBALL].cName, "Snowball");
  sprintf(output[OUT_SNOWBALL].cDescr, "Is the planet in a snowball state?");
  output[OUT_SNOWBALL].bNeg       = 0;
  output[OUT_SNOWBALL].iNum       = 1;
  output[OUT_SNOWBALL].iModuleBit = POISE;
  fnWrite[OUT_SNOWBALL]           = &WriteSnowball;

  sprintf(output[OUT_TOTICEMASS].cName, "TotIceMass");
  sprintf(output[OUT_TOTICEMASS].cDescr, "Global total ice mass in ice sheets");
  sprintf(output[OUT_TOTICEMASS].cNeg, "kg");
  output[OUT_TOTICEMASS].bNeg       = 1;
  output[OUT_TOTICEMASS].iNum       = 1;
  output[OUT_TOTICEMASS].iModuleBit = POISE;
  fnWrite[OUT_TOTICEMASS]           = &WriteTotIceMass;

  sprintf(output[OUT_TOTICEFLOW].cName, "TotIceFlow");
  sprintf(output[OUT_TOTICEFLOW].cDescr,
          "Global total ice flow in ice sheets (should = 0)");
  sprintf(output[OUT_TOTICEFLOW].cNeg, "kg");
  output[OUT_TOTICEFLOW].bNeg       = 1;
  output[OUT_TOTICEFLOW].iNum       = 1;
  output[OUT_TOTICEFLOW].iModuleBit = POISE;
  fnWrite[OUT_TOTICEFLOW]           = &WriteIceFlowTot;

  sprintf(output[OUT_TOTICEBALANCE].cName, "TotIceBalance");
  sprintf(output[OUT_TOTICEBALANCE].cDescr,
          "Global total ice balance in ice sheets (this time step)");
  sprintf(output[OUT_TOTICEBALANCE].cNeg, "kg");
  output[OUT_TOTICEBALANCE].bNeg       = 1;
  output[OUT_TOTICEBALANCE].iNum       = 1;
  output[OUT_TOTICEBALANCE].iModuleBit = POISE;
  fnWrite[OUT_TOTICEBALANCE]           = &WriteIceBalanceTot;

  sprintf(output[OUT_FLUXINGLOBAL].cName, "FluxInGlobal");
  sprintf(output[OUT_FLUXINGLOBAL].cDescr,
          "Global mean flux in (insol*(1-albedo)) from POISE");
  /* Sadly, Russell, we must set the negative option to W/m^2.
  sprintf(output[OUT_FLUXINGLOBAL].cNeg,"pirate-ninjas/m^2");
  output[OUT_FLUXINGLOBAL].dNeg = 1/40.55185;
  */
  output[OUT_FLUXINGLOBAL].bNeg = 1;
  output[OUT_FLUXINGLOBAL].dNeg = 1; // Just in case
  sprintf(output[OUT_FLUXINGLOBAL].cNeg, "W/m^2");
  output[OUT_FLUXINGLOBAL].iNum       = 1;
  output[OUT_FLUXINGLOBAL].iModuleBit = POISE;
  fnWrite[OUT_FLUXINGLOBAL]           = &WriteFluxInGlobal;

  sprintf(output[OUT_FLUXOUTGLOBAL].cName, "FluxOutGlobal");
  sprintf(output[OUT_FLUXOUTGLOBAL].cDescr, "Global mean flux out from POISE");
  /* Here, too
  sprintf(output[OUT_FLUXOUTGLOBAL].cNeg,"pirate-ninjas/m^2");
  output[OUT_FLUXOUTGLOBAL].dNeg = 1/40.55185;
  */
  output[OUT_FLUXOUTGLOBAL].bNeg = 1;
  output[OUT_FLUXOUTGLOBAL].dNeg = 1;
  sprintf(output[OUT_FLUXOUTGLOBAL].cNeg, "W/m^2");
  output[OUT_FLUXOUTGLOBAL].iNum       = 1;
  output[OUT_FLUXOUTGLOBAL].iModuleBit = POISE;
  fnWrite[OUT_FLUXOUTGLOBAL]           = &WriteFluxOutGlobal;

  sprintf(output[OUT_TEMPLAT].cName, "TempLat");
  sprintf(output[OUT_TEMPLAT].cDescr, "Surface temperature by latitude.");
  sprintf(output[OUT_TEMPLAT].cNeg, "Celsius");
  output[OUT_TEMPLAT].bNeg = 1;
  // conversion is hardcoded in write function
  output[OUT_TEMPLAT].dNeg       = 1;
  output[OUT_TEMPLAT].iNum       = 1;
  output[OUT_TEMPLAT].bGrid      = 1;
  output[OUT_TEMPLAT].iModuleBit = POISE;
  fnWrite[OUT_TEMPLAT]           = &WriteTempLat;

  sprintf(output[OUT_TEMPMINLAT].cName, "TempMinLat");
  sprintf(output[OUT_TEMPMINLAT].cDescr,
          "Minimum surface temperature over a year by latitude.");
  sprintf(output[OUT_TEMPMINLAT].cNeg, "Celsius");
  output[OUT_TEMPMINLAT].bNeg = 1;
  // conversion is hardcoded in write function
  output[OUT_TEMPMINLAT].dNeg       = 1;
  output[OUT_TEMPMINLAT].iNum       = 1;
  output[OUT_TEMPMINLAT].bGrid      = 1;
  output[OUT_TEMPMINLAT].iModuleBit = POISE;
  fnWrite[OUT_TEMPMINLAT]           = &WriteTempMinLat;

  sprintf(output[OUT_TEMPMAXLAT].cName, "TempMaxLat");
  sprintf(output[OUT_TEMPMAXLAT].cDescr,
          "Maximum surface temperature over a year by latitude.");
  sprintf(output[OUT_TEMPMAXLAT].cNeg, "Celsius");
  output[OUT_TEMPMAXLAT].bNeg = 1;
  // conversion is hardcoded in write function
  output[OUT_TEMPMAXLAT].dNeg       = 1;
  output[OUT_TEMPMAXLAT].iNum       = 1;
  output[OUT_TEMPMAXLAT].bGrid      = 1;
  output[OUT_TEMPMAXLAT].iModuleBit = POISE;
  fnWrite[OUT_TEMPMAXLAT]           = &WriteTempMaxLat;

  sprintf(output[OUT_TEMPMAXLAND].cName, "TempMaxLand");
  sprintf(output[OUT_TEMPMAXLAND].cDescr,
          "Maximum surface temperature on land");
  sprintf(output[OUT_TEMPMAXLAND].cNeg, "Celsius");
  output[OUT_TEMPMAXLAND].bNeg = 1;
  // conversion is hardcoded in write function
  output[OUT_TEMPMAXLAND].dNeg       = 1;
  output[OUT_TEMPMAXLAND].iNum       = 1;
  output[OUT_TEMPMAXLAND].bGrid      = 1;
  output[OUT_TEMPMAXLAND].iModuleBit = POISE;
  fnWrite[OUT_TEMPMAXLAND]           = &WriteTempMaxLand;

  sprintf(output[OUT_TEMPMAXWATER].cName, "TempMaxWater");
  sprintf(output[OUT_TEMPMAXWATER].cDescr,
          "Maximum surface temperature on water");
  sprintf(output[OUT_TEMPMAXWATER].cNeg, "Celsius");
  output[OUT_TEMPMAXWATER].bNeg = 1;
  // conversion is hardcoded in write function
  output[OUT_TEMPMAXWATER].dNeg       = 1;
  output[OUT_TEMPMAXWATER].iNum       = 1;
  output[OUT_TEMPMAXWATER].bGrid      = 1;
  output[OUT_TEMPMAXWATER].iModuleBit = POISE;
  fnWrite[OUT_TEMPMAXWATER]           = &WriteTempMaxWater;

  sprintf(output[OUT_TEMPLANDLAT].cName, "TempLandLat");
  sprintf(output[OUT_TEMPLANDLAT].cDescr,
          "Land surface temperature by latitude.");
  sprintf(output[OUT_TEMPLANDLAT].cNeg, "Celsius");
  output[OUT_TEMPLANDLAT].bNeg = 1;
  // conversion is hardcoded in write function
  output[OUT_TEMPLANDLAT].dNeg  = 1;
  output[OUT_TEMPLANDLAT].iNum  = 1;
  output[OUT_TEMPLANDLAT].bGrid = 1;
  fnWrite[OUT_TEMPLANDLAT]      = &WriteTempLandLat;

  sprintf(output[OUT_TEMPWATERLAT].cName, "TempWaterLat");
  sprintf(output[OUT_TEMPWATERLAT].cDescr,
          "Water surface temperature by latitude.");
  sprintf(output[OUT_TEMPWATERLAT].cNeg, "Celsius");
  output[OUT_TEMPWATERLAT].bNeg = 1;
  // conversion is hardcoded in write function
  output[OUT_TEMPWATERLAT].dNeg  = 1;
  output[OUT_TEMPWATERLAT].iNum  = 1;
  output[OUT_TEMPWATERLAT].bGrid = 1;
  fnWrite[OUT_TEMPWATERLAT]      = &WriteTempWaterLat;

  sprintf(output[OUT_LATITUDE].cName, "Latitude");
  sprintf(output[OUT_LATITUDE].cDescr, "Latitude.");
  sprintf(output[OUT_LATITUDE].cNeg, "deg");
  output[OUT_LATITUDE].bNeg       = 1;
  output[OUT_LATITUDE].dNeg       = 1 / DEGRAD;
  output[OUT_LATITUDE].iNum       = 1;
  output[OUT_LATITUDE].bGrid      = 1;
  output[OUT_LATITUDE].iModuleBit = POISE;
  fnWrite[OUT_LATITUDE]           = &WriteLatitude;

  sprintf(output[OUT_ALBEDOLAT].cName, "AlbedoLat");
  sprintf(output[OUT_ALBEDOLAT].cDescr, "Surface albedo by latitude.");
  output[OUT_ALBEDOLAT].bNeg       = 0;
  output[OUT_ALBEDOLAT].iNum       = 1;
  output[OUT_ALBEDOLAT].bGrid      = 1;
  output[OUT_ALBEDOLAT].iModuleBit = POISE;
  fnWrite[OUT_ALBEDOLAT]           = &WriteAlbedoLat;

  sprintf(output[OUT_ALBEDOLANDLAT].cName, "AlbedoLandLat");
  sprintf(output[OUT_ALBEDOLANDLAT].cDescr, "Land surface albedo by latitude.");
  output[OUT_ALBEDOLANDLAT].bNeg  = 0;
  output[OUT_ALBEDOLANDLAT].iNum  = 1;
  output[OUT_ALBEDOLANDLAT].bGrid = 1;
  fnWrite[OUT_ALBEDOLANDLAT]      = &WriteAlbedoLandLat;

  sprintf(output[OUT_ALBEDOWATERLAT].cName, "AlbedoWaterLat");
  sprintf(output[OUT_ALBEDOWATERLAT].cDescr,
          "Water surface albedo by latitude.");
  output[OUT_ALBEDOWATERLAT].bNeg  = 0;
  output[OUT_ALBEDOWATERLAT].iNum  = 1;
  output[OUT_ALBEDOWATERLAT].bGrid = 1;
  fnWrite[OUT_ALBEDOWATERLAT]      = &WriteAlbedoWaterLat;

  sprintf(output[OUT_ANNUALINSOL].cName, "AnnInsol");
  sprintf(output[OUT_ANNUALINSOL].cDescr, "Annual insolation by latitude.");
  sprintf(output[OUT_ANNUALINSOL].cNeg, "W/m^2");
  output[OUT_ANNUALINSOL].bNeg       = 1;
  output[OUT_ANNUALINSOL].dNeg       = 1 / 40.55185;
  output[OUT_ANNUALINSOL].iNum       = 1;
  output[OUT_ANNUALINSOL].bGrid      = 1;
  output[OUT_ANNUALINSOL].iModuleBit = POISE;
  fnWrite[OUT_ANNUALINSOL]           = &WriteAnnualInsol;

  sprintf(output[OUT_PEAKINSOL].cName, "PeakInsol");
  sprintf(output[OUT_PEAKINSOL].cDescr, "Peak insolation by latitude.");
  sprintf(output[OUT_PEAKINSOL].cNeg, "W/m^2");
  output[OUT_PEAKINSOL].bNeg       = 1;
  output[OUT_PEAKINSOL].dNeg       = 1 / 40.55185;
  output[OUT_PEAKINSOL].iNum       = 1;
  output[OUT_PEAKINSOL].bGrid      = 1;
  output[OUT_PEAKINSOL].iModuleBit = POISE;
  fnWrite[OUT_PEAKINSOL]           = &WritePeakInsol;

  sprintf(output[OUT_FLUXMERID].cName, "FluxMerid");
  sprintf(output[OUT_FLUXMERID].cDescr,
          "Total meridional (northward) flux by latitude");
  sprintf(output[OUT_FLUXMERID].cNeg, "PW");
  output[OUT_FLUXMERID].bNeg       = 1;
  output[OUT_FLUXMERID].dNeg       = 1e-15;
  output[OUT_FLUXMERID].iNum       = 1;
  output[OUT_FLUXMERID].bGrid      = 1;
  output[OUT_FLUXMERID].iModuleBit = POISE;
  fnWrite[OUT_FLUXMERID]           = &WriteFluxMerid;

  sprintf(output[OUT_FLUXIN].cName, "FluxIn");
  sprintf(output[OUT_FLUXIN].cDescr, "Incoming flux by latitude");
  sprintf(output[OUT_FLUXIN].cNeg, "W/m^2");
  output[OUT_FLUXIN].bNeg       = 1;
  output[OUT_FLUXIN].dNeg       = 1;
  output[OUT_FLUXIN].iNum       = 1;
  output[OUT_FLUXIN].bGrid      = 1;
  output[OUT_FLUXIN].iModuleBit = POISE;
  fnWrite[OUT_FLUXIN]           = &WriteFluxIn;

  sprintf(output[OUT_FLUXOUT].cName, "FluxOut");
  sprintf(output[OUT_FLUXOUT].cDescr, "Outgoing (spaceward) flux by latitude");
  sprintf(output[OUT_FLUXOUT].cNeg, "W/m^2");
  output[OUT_FLUXOUT].bNeg       = 1;
  output[OUT_FLUXOUT].dNeg       = 1;
  output[OUT_FLUXOUT].iNum       = 1;
  output[OUT_FLUXOUT].bGrid      = 1;
  output[OUT_FLUXOUT].iModuleBit = POISE;
  fnWrite[OUT_FLUXOUT]           = &WriteFluxOut;

  sprintf(output[OUT_DIVFLUX].cName, "DivFlux");
  sprintf(output[OUT_DIVFLUX].cDescr,
          "Divergence of flux (flow into adjacent cells) by latitude");
  sprintf(output[OUT_DIVFLUX].cNeg, "W/m^2");
  output[OUT_DIVFLUX].bNeg       = 1;
  output[OUT_DIVFLUX].dNeg       = 1;
  output[OUT_DIVFLUX].iNum       = 1;
  output[OUT_DIVFLUX].bGrid      = 1;
  output[OUT_DIVFLUX].iModuleBit = POISE;
  fnWrite[OUT_DIVFLUX]           = &WriteDivFlux;

  sprintf(output[OUT_ICEMASS].cName, "IceMass");
  sprintf(output[OUT_ICEMASS].cDescr, "Mass of ice sheets/area by latitude");
  sprintf(output[OUT_ICEMASS].cNeg, "kg/m^2");
  output[OUT_ICEMASS].bNeg       = 1;
  output[OUT_ICEMASS].dNeg       = 1;
  output[OUT_ICEMASS].iNum       = 1;
  output[OUT_ICEMASS].bGrid      = 1;
  output[OUT_ICEMASS].iModuleBit = POISE;
  fnWrite[OUT_ICEMASS]           = &WriteIceMass;

  sprintf(output[OUT_ICEHEIGHT].cName, "IceHeight");
  sprintf(output[OUT_ICEHEIGHT].cDescr, "Height of ice sheets");
  sprintf(output[OUT_ICEHEIGHT].cNeg, "m");
  output[OUT_ICEHEIGHT].bNeg       = 1;
  output[OUT_ICEHEIGHT].dNeg       = 1;
  output[OUT_ICEHEIGHT].iNum       = 1;
  output[OUT_ICEHEIGHT].bGrid      = 1;
  output[OUT_ICEHEIGHT].iModuleBit = POISE;
  fnWrite[OUT_ICEHEIGHT]           = &WriteIceHeight;

  sprintf(output[OUT_DICEMASSDT].cName, "DIceMassDt");
  sprintf(output[OUT_DICEMASSDT].cDescr,
          "derivative of mass of ice sheets/area by latitude");
  sprintf(output[OUT_DICEMASSDT].cNeg, "kg/m^2/s");
  output[OUT_DICEMASSDT].bNeg       = 1;
  output[OUT_DICEMASSDT].dNeg       = 1;
  output[OUT_DICEMASSDT].iNum       = 1;
  output[OUT_DICEMASSDT].bGrid      = 1;
  output[OUT_DICEMASSDT].iModuleBit = POISE;
  fnWrite[OUT_DICEMASSDT]           = &WriteDIceMassDt;

  sprintf(output[OUT_ICEACCUM].cName, "IceAccum");
  sprintf(output[OUT_ICEACCUM].cDescr,
          "Ice growth per orbit (accumulation only)");
  sprintf(output[OUT_ICEACCUM].cNeg, "m/orbit");
  output[OUT_ICEACCUM].bNeg       = 1;
  output[OUT_ICEACCUM].dNeg       = 1;
  output[OUT_ICEACCUM].iNum       = 1;
  output[OUT_ICEACCUM].bGrid      = 1;
  output[OUT_ICEACCUM].iModuleBit = POISE;
  fnWrite[OUT_ICEACCUM]           = &WriteIceAccum;

  sprintf(output[OUT_ICEABLATE].cName, "IceAblate");
  sprintf(output[OUT_ICEABLATE].cDescr, "ice decay per orbit (ablation only)");
  sprintf(output[OUT_ICEABLATE].cNeg, "m/orbit");
  output[OUT_ICEABLATE].bNeg       = 1;
  output[OUT_ICEABLATE].dNeg       = 1;
  output[OUT_ICEABLATE].iNum       = 1;
  output[OUT_ICEABLATE].bGrid      = 1;
  output[OUT_ICEABLATE].iModuleBit = POISE;
  fnWrite[OUT_ICEABLATE]           = &WriteIceAblate;

  sprintf(output[OUT_ICEFLOW].cName, "IceFlow");
  sprintf(output[OUT_ICEFLOW].cDescr, "flow of ice sheets/area by latitude");
  sprintf(output[OUT_ICEFLOW].cNeg, "m/s");
  output[OUT_ICEFLOW].bNeg       = 1;
  output[OUT_ICEFLOW].dNeg       = 1;
  output[OUT_ICEFLOW].iNum       = 1;
  output[OUT_ICEFLOW].bGrid      = 1;
  output[OUT_ICEFLOW].iModuleBit = POISE;
  fnWrite[OUT_ICEFLOW]           = &WriteDIceMassDtFlow;

  sprintf(output[OUT_BEDROCKH].cName, "BedrockH");
  sprintf(output[OUT_BEDROCKH].cDescr, "height/depth of bedrock");
  sprintf(output[OUT_BEDROCKH].cNeg, "m");
  output[OUT_BEDROCKH].bNeg       = 1;
  output[OUT_BEDROCKH].dNeg       = 1;
  output[OUT_BEDROCKH].iNum       = 1;
  output[OUT_BEDROCKH].bGrid      = 1;
  output[OUT_BEDROCKH].iModuleBit = POISE;
  fnWrite[OUT_BEDROCKH]           = &WriteBedrockH;

  sprintf(output[OUT_ENERGYRESL].cName, "EnergyResL");
  sprintf(output[OUT_ENERGYRESL].cDescr, "Energy residual on land");
  sprintf(output[OUT_ENERGYRESL].cNeg, "W/m^2");
  output[OUT_ENERGYRESL].bNeg       = 1;
  output[OUT_ENERGYRESL].dNeg       = 1;
  output[OUT_ENERGYRESL].iNum       = 1;
  output[OUT_ENERGYRESL].bGrid      = 1;
  output[OUT_ENERGYRESL].iModuleBit = POISE;
  fnWrite[OUT_ENERGYRESL]           = &WriteEnergyResL;

  sprintf(output[OUT_ENERGYRESW].cName, "EnergyResW");
  sprintf(output[OUT_ENERGYRESW].cDescr, "Energy residual over water");
  sprintf(output[OUT_ENERGYRESW].cNeg, "W/m^2");
  output[OUT_ENERGYRESW].bNeg       = 1;
  output[OUT_ENERGYRESW].dNeg       = 1;
  output[OUT_ENERGYRESW].iNum       = 1;
  output[OUT_ENERGYRESW].bGrid      = 1;
  output[OUT_ENERGYRESW].iModuleBit = POISE;
  fnWrite[OUT_ENERGYRESW]           = &WriteEnergyResW;

  sprintf(output[OUT_SKIPSEAS].cName, "SkipSeas");
  sprintf(output[OUT_SKIPSEAS].cDescr,
          "Is Seasonal model skipped due to RGH or snowball?");
  output[OUT_SKIPSEAS].bNeg       = 0;
  output[OUT_SKIPSEAS].iNum       = 1;
  output[OUT_SKIPSEAS].iModuleBit = POISE;
  fnWrite[OUT_SKIPSEAS]           = &WriteSkipSeas;

  sprintf(output[OUT_PLANCKBAVG].cName, "PlanckBAvg");
  sprintf(output[OUT_PLANCKBAVG].cDescr, "Annually averaged Planck B coeff");
  sprintf(output[OUT_PLANCKBAVG].cNeg, "W/m^2/C");
  output[OUT_PLANCKBAVG].bNeg       = 0;
  output[OUT_PLANCKBAVG].iNum       = 1;
  output[OUT_PLANCKBAVG].bGrid      = 1;
  output[OUT_PLANCKBAVG].iModuleBit = POISE;
  fnWrite[OUT_PLANCKBAVG]           = &WritePlanckBAvg;

  sprintf(output[OUT_AREAICECOV].cName, "AreaIceCov");
  sprintf(output[OUT_AREAICECOV].cDescr, "Fractional area ice covered");
  sprintf(output[OUT_AREAICECOV].cNeg, " ");
  output[OUT_AREAICECOV].bNeg       = 1;
  output[OUT_AREAICECOV].iNum       = 1;
  output[OUT_AREAICECOV].iModuleBit = POISE;
  fnWrite[OUT_AREAICECOV]           = &WriteAreaIceCov;

  sprintf(output[OUT_NORTHICECAPLAND].cName, "IceCapNorthLand");
  sprintf(output[OUT_NORTHICECAPLAND].cDescr,
          "Does the planet have a northern polar ice cap on land?");
  output[OUT_NORTHICECAPLAND].bNeg       = 0;
  output[OUT_NORTHICECAPLAND].iNum       = 1;
  output[OUT_NORTHICECAPLAND].iModuleBit = POISE;
  fnWrite[OUT_NORTHICECAPLAND]           = &WriteIceCapNorthLand;

  sprintf(output[OUT_NORTHICECAPSEA].cName, "IceCapNorthSea");
  sprintf(output[OUT_NORTHICECAPSEA].cDescr,
          "Does the planet have a northern polar sea ice cap");
  output[OUT_NORTHICECAPSEA].bNeg       = 0;
  output[OUT_NORTHICECAPSEA].iNum       = 1;
  output[OUT_NORTHICECAPSEA].iModuleBit = POISE;
  fnWrite[OUT_NORTHICECAPSEA]           = &WriteIceCapNorthSea;

  sprintf(output[OUT_SOUTHICECAPLAND].cName, "IceCapSouthLand");
  sprintf(output[OUT_SOUTHICECAPLAND].cDescr,
          "Does the planet have a southern polar ice cap on land?");
  output[OUT_SOUTHICECAPLAND].bNeg       = 0;
  output[OUT_SOUTHICECAPLAND].iNum       = 1;
  output[OUT_SOUTHICECAPLAND].iModuleBit = POISE;
  fnWrite[OUT_SOUTHICECAPLAND]           = &WriteIceCapSouthLand;

  sprintf(output[OUT_SOUTHICECAPSEA].cName, "IceCapSouthSea");
  sprintf(output[OUT_SOUTHICECAPSEA].cDescr,
          "Does the planet have a southern polar sea ice cap?");
  output[OUT_SOUTHICECAPSEA].bNeg       = 0;
  output[OUT_SOUTHICECAPSEA].iNum       = 1;
  output[OUT_SOUTHICECAPSEA].iModuleBit = POISE;
  fnWrite[OUT_SOUTHICECAPSEA]           = &WriteIceCapSouthSea;

  sprintf(output[OUT_ICEBELTLAND].cName, "IceBeltLand");
  sprintf(output[OUT_ICEBELTLAND].cDescr,
          "Does the planet have a land ice belt?");
  output[OUT_ICEBELTLAND].bNeg       = 0;
  output[OUT_ICEBELTLAND].iNum       = 1;
  output[OUT_ICEBELTLAND].iModuleBit = POISE;
  fnWrite[OUT_ICEBELTLAND]           = &WriteIceBeltLand;

  sprintf(output[OUT_ICEBELTSEA].cName, "IceBeltSea");
  sprintf(output[OUT_ICEBELTSEA].cDescr,
          "Does the planet have a sea ice belt?");
  output[OUT_ICEBELTSEA].bNeg       = 0;
  output[OUT_ICEBELTSEA].iNum       = 1;
  output[OUT_ICEBELTSEA].iModuleBit = POISE;
  fnWrite[OUT_ICEBELTSEA]           = &WriteIceBeltSea;

  sprintf(output[OUT_SNOWBALLLAND].cName, "SnowballLand");
  sprintf(output[OUT_SNOWBALLLAND].cDescr, "Is all land covered in ice?");
  output[OUT_SNOWBALLLAND].bNeg       = 0;
  output[OUT_SNOWBALLLAND].iNum       = 1;
  output[OUT_SNOWBALLLAND].iModuleBit = POISE;
  fnWrite[OUT_SNOWBALLLAND]           = &WriteSnowballLand;

  sprintf(output[OUT_SNOWBALLSEA].cName, "SnowballSea");
  sprintf(output[OUT_SNOWBALLSEA].cDescr, "Is all sea covered in ice?");
  output[OUT_SNOWBALLSEA].bNeg       = 0;
  output[OUT_SNOWBALLSEA].iNum       = 1;
  output[OUT_SNOWBALLSEA].iModuleBit = POISE;
  fnWrite[OUT_SNOWBALLSEA]           = &WriteSnowballSea;

  sprintf(output[OUT_ICEFREE].cName, "IceFree");
  sprintf(output[OUT_ICEFREE].cDescr,
          "Is the planet free of sea and land ice?");
  output[OUT_ICEFREE].bNeg       = 0;
  output[OUT_ICEFREE].iNum       = 1;
  output[OUT_ICEFREE].iModuleBit = POISE;
  fnWrite[OUT_ICEFREE]           = &WriteIceFree;

  sprintf(output[OUT_NORTHICECAPLATLAND].cName, "IceCapNorthLatLand");
  sprintf(output[OUT_NORTHICECAPLATLAND].cDescr,
          "Southernmost extent of northern land ice cap.");
  output[OUT_NORTHICECAPLATLAND].bNeg       = 0;
  output[OUT_NORTHICECAPLATLAND].iNum       = 1;
  output[OUT_NORTHICECAPLATLAND].iModuleBit = POISE;
  fnWrite[OUT_NORTHICECAPLATLAND]           = &WriteIceCapNorthLatLand;
  sprintf(output[OUT_NORTHICECAPLATLAND].cLongDescr,
          "If a northern land ice cap is present, return the latitude of its "
          "southern edge. If not present, return +100 degrees.");

  sprintf(output[OUT_NORTHICECAPLATSEA].cName, "IceCapNorthLatSea");
  sprintf(output[OUT_NORTHICECAPLATSEA].cDescr,
          "Southernmost extent of northern sea ice cap.");
  output[OUT_NORTHICECAPLATSEA].bNeg       = 0;
  output[OUT_NORTHICECAPLATSEA].iNum       = 1;
  output[OUT_NORTHICECAPLATSEA].iModuleBit = POISE;
  fnWrite[OUT_NORTHICECAPLATSEA]           = &WriteIceCapNorthLatSea;
  sprintf(output[OUT_NORTHICECAPLATSEA].cLongDescr,
          "If a northern sea ice cap is present, return the latitude of its "
          "southern edge. If not present, return +100 degrees.");

  sprintf(output[OUT_SOUTHICECAPLATLAND].cName, "IceCapSouthLatLand");
  sprintf(output[OUT_SOUTHICECAPLATLAND].cDescr,
          "Northernmost extent of southern land ice cap.");
  output[OUT_SOUTHICECAPLATLAND].bNeg       = 0;
  output[OUT_SOUTHICECAPLATLAND].iNum       = 1;
  output[OUT_SOUTHICECAPLATLAND].iModuleBit = POISE;
  fnWrite[OUT_SOUTHICECAPLATLAND]           = &WriteIceCapSouthLatLand;
  sprintf(output[OUT_SOUTHICECAPLATLAND].cLongDescr,
          "If a southern land ice cap is present, return the latitude of its "
          "northern edge. If not present, return -100 degrees.");

  sprintf(output[OUT_SOUTHICECAPLATSEA].cName, "IceCapSouthLatSea");
  sprintf(output[OUT_SOUTHICECAPLATSEA].cDescr,
          "Northernmost extent of southern sea ice cap.");
  output[OUT_SOUTHICECAPLATSEA].bNeg       = 0;
  output[OUT_SOUTHICECAPLATSEA].iNum       = 1;
  output[OUT_SOUTHICECAPLATSEA].iModuleBit = POISE;
  fnWrite[OUT_SOUTHICECAPLATSEA]           = &WriteIceCapSouthLatLand;
  sprintf(output[OUT_SOUTHICECAPLATSEA].cLongDescr,
          "If a southern sea ice cap is present, return the latitude of its "
          "northern edge. If not present, return -100 degrees.");

  sprintf(output[OUT_NORTHICEBELTLATLAND].cName, "IceBeltNorthLatLand");
  sprintf(output[OUT_NORTHICEBELTLATLAND].cDescr,
          "Northernmost extent of land ice belt.");
  output[OUT_NORTHICEBELTLATLAND].bNeg       = 0;
  output[OUT_NORTHICEBELTLATLAND].iNum       = 1;
  output[OUT_NORTHICEBELTLATLAND].iModuleBit = POISE;
  fnWrite[OUT_NORTHICEBELTLATLAND]           = &WriteIceBeltNorthLatLand;
  sprintf(
        output[OUT_NORTHICEBELTLATLAND].cLongDescr,
        "If a land ice belt is present, return the latitude of its northern "
        "edge. "
        "If not present, return 0. Note that some ice belts may in fact have a "
        "northern edge at the equator.");

  sprintf(output[OUT_NORTHICEBELTLATSEA].cName, "IceBeltNorthLatSea");
  sprintf(output[OUT_NORTHICEBELTLATSEA].cDescr,
          "Northernmost extent of sea ice belt.");
  output[OUT_NORTHICEBELTLATSEA].bNeg       = 0;
  output[OUT_NORTHICEBELTLATSEA].iNum       = 1;
  output[OUT_NORTHICEBELTLATSEA].iModuleBit = POISE;
  fnWrite[OUT_NORTHICEBELTLATSEA]           = &WriteIceBeltNorthLatSea;
  sprintf(
        output[OUT_NORTHICEBELTLATSEA].cLongDescr,
        "If a sea ice belt is present, return the latitude of its northern "
        "edge. "
        "If not present, return 0. Note that some ice belts may in fact have a "
        "northern edge at the equator.");

  sprintf(output[OUT_SOUTHICEBELTLATLAND].cName, "IceBeltSouthLatLand");
  sprintf(output[OUT_SOUTHICEBELTLATLAND].cDescr,
          "Southernmost extent of land ice belt.");
  output[OUT_SOUTHICEBELTLATLAND].bNeg       = 0;
  output[OUT_SOUTHICEBELTLATLAND].iNum       = 1;
  output[OUT_SOUTHICEBELTLATLAND].iModuleBit = POISE;
  fnWrite[OUT_SOUTHICEBELTLATLAND]           = &WriteIceBeltSouthLatLand;
  sprintf(
        output[OUT_SOUTHICEBELTLATLAND].cLongDescr,
        "If a land ice belt is present, return the latitude of its southern "
        "edge. "
        "If not present, return 0. Note that some ice belts may in fact have a "
        "southern edge at the equator.");

  sprintf(output[OUT_SOUTHICEBELTLATSEA].cName, "IceBeltSouthLatSea");
  sprintf(output[OUT_SOUTHICEBELTLATSEA].cDescr,
          "Southernmost extent of sea ice belt.");
  output[OUT_SOUTHICEBELTLATSEA].bNeg       = 0;
  output[OUT_SOUTHICEBELTLATSEA].iNum       = 1;
  output[OUT_SOUTHICEBELTLATSEA].iModuleBit = POISE;
  fnWrite[OUT_SOUTHICEBELTLATSEA]           = &WriteIceBeltSouthLatSea;
  sprintf(
        output[OUT_SOUTHICEBELTLATSEA].cLongDescr,
        "If a sea ice belt is present, return the latitude of its southern "
        "edge. "
        "If not present, return 0. Note that some ice belts may in fact have a "
        "southern edge at the equator.");
}

/************ POISE Logging Functions **************/

void LogOptionsPoise(CONTROL *control, FILE *fp) {
  fprintf(fp, "-------- POISE Options -----\n\n");
}

void LogPoise(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
              UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {

  int iOut;

  fprintf(fp, "\n----- POISE PARAMETERS ------\n");
  for (iOut = OUTSTARTPOISE; iOut < OUTBODYSTARTPOISE; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, 0);
    }
  }
}

void LogBodyPoise(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UPDATE *update, fnWriteOutput fnWrite[], FILE *fp,
                  int iBody) {

  int iOut;

  fprintf(fp, "-----POISE PARAMETERS (%s)------\n", body[iBody].cName);
  for (iOut = OUTBODYSTARTPOISE; iOut < OUTENDPOISE; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
  }
}

/************* MODULE Functions ***********/

void AddModulePoise(CONTROL *control, MODULE *module, int iBody, int iModule) {

  module->iaModule[iBody][iModule] = POISE;

  module->fnInitializeUpdateTmpBody[iBody][iModule] =
        &InitializeUpdateTmpBodyPoise;
  module->fnCountHalts[iBody][iModule] = &CountHaltsPoise;
  module->fnLogBody[iBody][iModule]    = &LogBodyPoise;

  module->fnReadOptions[iBody][iModule]       = &ReadOptionsPoise;
  module->fnVerify[iBody][iModule]            = &VerifyPoise;
  module->fnAssignDerivatives[iBody][iModule] = &AssignPoiseDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullPoiseDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltPoise;

  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdatePoise;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputPoise;
}

/************* POISE Functions ***********/
void UpdateOrbitOblData(BODY *body, EVOLVE *evolve, int iBody) {
  body[iBody].dSemi      = body[iBody].daSemiSeries[body[iBody].iCurrentStep];
  body[iBody].dEcc       = body[iBody].daEccSeries[body[iBody].iCurrentStep];
  body[iBody].dArgP      = body[iBody].daArgPSeries[body[iBody].iCurrentStep];
  body[iBody].dLongA     = body[iBody].daLongASeries[body[iBody].iCurrentStep];
  body[iBody].dObliquity = body[iBody].daOblSeries[body[iBody].iCurrentStep];
  body[iBody].dPrecA     = body[iBody].daPrecASeries[body[iBody].iCurrentStep];

  body[iBody].dXobl = sin(body[iBody].dObliquity) * cos(body[iBody].dPrecA);
  body[iBody].dYobl = sin(body[iBody].dObliquity) * sin(body[iBody].dPrecA);
  body[iBody].dZobl = cos(body[iBody].dObliquity);

  body[iBody].dHecc = body[iBody].daHeccSeries[body[iBody].iCurrentStep];
  body[iBody].dKecc = body[iBody].daKeccSeries[body[iBody].iCurrentStep];
}

/**
Calculates flow at base of ice sheet

@param body Struct containing all body information and variables
@param iBody Body in question
@param iLat Index of current latitude cell
@return Flow rate at base of ice sheet (velocity*height -> m^2/s)
*/
double fdBasalFlow(BODY *body, int iBody, int iLat) {

  double dAsed, dBsed, dTmp, dM, dMinv, dGrav;

  dM = 1.25;

  if (body[iBody].daSedShear[iLat] == 0) {
    return 0.0;
  } else {
    dAsed = body[iBody].daSedShear[iLat];
    dGrav = BIGG * body[iBody].dMass /
            (body[iBody].dRadius * body[iBody].dRadius);
    dBsed = (RHOSED - RHOH2O) * dGrav * tan(SEDPHI);

    if (fabs(dAsed) / dBsed <= SEDH) {
      dMinv = 0;
    } else {
      dMinv = 1 - dBsed / fabs(dAsed) * SEDH;
    }

    dTmp = 2 *
           (SEDD0 * RHOICE * dGrav *
            (body[iBody].daIceHeight[iLat] * body[iBody].daIceHeight[iLat])) /
           ((dM + 1) * dBsed) * pow(fabs(dAsed) / (2 * SEDD0 * SEDMU), dM) *
           (1.0 - pow(dMinv, dM + 1));

    return dTmp;
  }
}

/**
If a test for a belt is false, assign these values to the referenced
parameters.

@param dLatIceEdgeNorth Northern latitude of ice edge
@param dLatIceEdgeSouth Southern latitude of ice edge
@param iLatIceEdgeNorth Index of northern ice edge latitude
@param iLatIceEdgeNorth Index of southern ice edge latitude
@param bBelt Boolean for ice belt presence
*/
void fvNoIceBelt(double *dLatIceEdgeNorth, double *dLatIceEdgeSouth,
                 int *iLatIceEdgeNorth, int *iLatIceEdgeSouth, int *bBelt) {

  *iLatIceEdgeNorth = 0;
  *iLatIceEdgeSouth = 0;
  *dLatIceEdgeNorth = 100;
  *dLatIceEdgeSouth = 100;
  *bBelt            = 0;
}

/**
If a test for a cap  is false, assign these values to the referenced
parameters.

@param dLatIceEdge Latitude of ice edge
@param iLatIceEdge Index of ice edge latitude
@param bBoolean Boolean for ice cap presence
*/
void fvNoIceCap(double *dLatIceEdge, int *iLatIceEdge, int *bCap) {

  *iLatIceEdge = 0;
  *dLatIceEdge = 100;
  *bCap        = 0;
}

/**
Is a specific latitude's sea component covered in ice?

@param body Struct containing all body information and variables
@param iBody Body in question
*/
double fbIceLatSea(BODY *body, int iBody, int iLat) {

  if (body[iBody].daSeaIceHeight[iLat] >= body[iBody].dMinIceHeight ||
      body[iBody].daTempMaxWater[iLat] < body[iBody].dFrzTSeaIce) {
    return 1;
  }

  return 0;
}

/**
Is a specific latitude's land component covered in ice?

@param body Struct containing all body information and variables
@param iBody Body in question
*/
double fbIceLatLand(BODY *body, int iBody, int iLat) {
  if (body[iBody].daIceHeight[iLat] >= body[iBody].dMinIceHeight ||
      body[iBody].daTempMaxLand[iLat] < 0) {
    return 1;
  }

  return 0;
}

/**
Determines if planet has no surface ice

@param body Struct containing all body information and variables
@param iBody Body in question
@return 1 for no sea or land ice free, 0 for ice

*/
int fbIceFree(BODY *body, int iBody) {

  int iLat, bSea, bLand;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    bSea  = fbIceLatSea(body, iBody, iLat);
    bLand = fbIceLatLand(body, iBody, iLat);
    if (bSea || bLand) {
      return 0;
    }
  }

  // No ice found
  return 1;
}

/**
Determines if planet has no sland ice

@param body Struct containing all body information and variables
@param iBody Body in question
@return 1 for no sea or land ice free, 0 for ice

*/
int fbIceFreeLand(BODY *body, int iBody) {

  int iLat;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (fbIceLatLand(body, iBody, iLat)) {
      return 0;
    }
  }

  // No ice found
  return 1;
}

/**
Determines if planet has no sea ice

@param body Struct containing all body information and variables
@param iBody Body in question
@return 1 for no sea or land ice free, 0 for ice

*/
int fbIceFreeSea(BODY *body, int iBody) {

  int iLat;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (fbIceLatSea(body, iBody, iLat)) {
      return 0;
    }
  }

  // No ice found
  return 1;
}

/**
Determines if planet has entered snowball state

@param body Struct containing all body information and variables
@param iBody Body in question

XXX This should really be removed or modified in order to be consistent with
other ice coverage functions!!!! XXX
*/
void fvSnowball(BODY *body, int iBody) {
  int iLat, iNum = 0;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (body[iBody].bSeaIceModel) {
      if (body[iBody].daSeaIceHeight[iLat] >= body[iBody].dMinIceHeight) {
        iNum++;
      }
    } else {
      if (body[iBody].daTempMaxWater[iLat] <= body[iBody].dFrzTSeaIce) {
        iNum++;
      }
    }
  }
  if (iNum == body[iBody].iNumLats) {
    body[iBody].bSnowball = 1;
  } else {
    body[iBody].bSnowball = 0;
  }
}

/**
Is all the land on the planet covered in ice?

@param body Struct containing all body information and variables
@param iBody Body in question
*/
int fbSnowballLand(BODY *body, int iBody) {

  int iLat, iNum;
  iNum = 0;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (fbIceLatLand(body, iBody, iLat)) {
      iNum++;
    }
  }

  if (iNum == body[iBody].iNumLats) {
    return 1;
  }

  return 0;
}

/**
Are the planet's seas all ice-covered?

@param body Struct containing all body information and variables
@param iBody Body in question
*/
int fbSnowballSea(BODY *body, int iBody) {

  int iLat, iNum;
  iNum = 0;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (fbIceLatSea(body, iBody, iLat)) {
      iNum++;
    }
  }

  if (iNum == body[iBody].iNumLats) {
    return 1;
  }

  return 0;
}

/**
Determine if planet has a northern polar ice cap on land and the extent of the
cap.

@param body Struct containing all body information and variables
@param iBody Body in question
@param iLatIceEdge Index of ice edge latitude
@param dLatIceEdge Latitude of ice edge
@param bCap 1 for northern polar ice cap, 0 for ice free north pole
*/
void fvNorthIceCapLand(BODY *body, int iBody, double *dLatIceEdge,
                       int *iLatIceEdge, int *bCap) {

  int iLat;

  // Check for ice at north pole; no ice at +90 => No ice cap
  if (!fbIceLatLand(body, iBody, body[iBody].iNumLats - 1)) {
    // Assign values for no cap
    fvNoIceCap(dLatIceEdge, iLatIceEdge, bCap);
    return;
  }

  // Icy north pole; does ice extend to other pole?
  if (fbSnowballLand(body, iBody)) {
    // Assign values for no cap
    fvNoIceCap(dLatIceEdge, iLatIceEdge, bCap);
    return;
  }

  // If made it here, must be a northern polar cap
  *bCap = 1;
  // Now find ice cap extent
  for (iLat = body[iBody].iNumLats - 1; iLat >= 0; iLat--) {
    if (!fbIceLatLand(body, iBody, iLat)) {
      // Found edge!
      *iLatIceEdge = iLat;
      *dLatIceEdge = body[iBody].daLats[iLat];
      return;
    }
  }
  // LCOV_EXCL_START
  fprintf(stderr, "ERROR: Failure in fvNorthIceCapLand.\n");
  exit(EXIT_INT);
  // LCOV_EXCL_STOP
}

/**
Determines if planet has a northern polar sea ice cap and the extent of the cap.

@param body Struct containing all body information and variables
@param iBody Body in question
@param iLatIceEdge Index of ice edge latitude
@param dLatIceEdge Latitude of ice edge
@param bCap 1 for northern polar ice cap, 0 for ice free north pole
*/
void fvNorthIceCapSea(BODY *body, int iBody, double *dLatIceEdge,
                      int *iLatIceEdge, int *bCap) {

  int iLat;

  // Check for ice at north pole; no ice at +90 => No ice cap
  if (!fbIceLatSea(body, iBody, body[iBody].iNumLats - 1)) {
    // Assign values for no cap
    fvNoIceCap(dLatIceEdge, iLatIceEdge, bCap);
    return;
  }

  // Icy north pole; does ice extend to other pole?
  if (fbSnowballSea(body, iBody)) {
    // Assign values for no cap
    fvNoIceCap(dLatIceEdge, iLatIceEdge, bCap);
    return;
  }

  // If made it here, must be a northern polar cap
  *bCap = 1;
  // Now find ice cap extent
  for (iLat = body[iBody].iNumLats - 1; iLat >= 0; iLat--) {
    if (!fbIceLatSea(body, iBody, iLat)) {
      // Found edge!
      *iLatIceEdge = iLat;
      *dLatIceEdge = body[iBody].daLats[iLat];
      return;
    }
  }
  // LCOV_EXCL_START
  fprintf(stderr, "ERROR: Failure in fvNorthIceCapSea.\n");
  exit(EXIT_INT);
  // LCOV_EXCL_STOP
}

/**
Determines if planet has a southern polar ice cap on land and the extent of the
cap.

@param body Struct containing all body information and variables
@param iBody Body in question
@param iLatIceEdge Index of ice edge latitude
@param dLatIceEdge Latitude of ice edge
@param bCap 1 for northern polar ice cap, 0 for ice free north pole
*/
void fvSouthIceCapLand(BODY *body, int iBody, double *dLatIceEdge,
                       int *iLatIceEdge, int *bCap) {

  int iLat;

  // Check for ice at south pole; no ice at -90 => No ice cap
  if (!fbIceLatLand(body, iBody, 0)) {
    // Assign values for no cap
    fvNoIceCap(dLatIceEdge, iLatIceEdge, bCap);
    return;
  }

  // Icy south pole; does ice extend to other pole?
  if (fbSnowballLand(body, iBody)) {
    // Assign values for no cap
    fvNoIceCap(dLatIceEdge, iLatIceEdge, bCap);
    return;
  }

  // If made it here, must be a southern polar cap
  *bCap = 1;
  // Now find ice cap extent
  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (!fbIceLatLand(body, iBody, iLat)) {
      // Found edge!
      *iLatIceEdge = iLat;
      *dLatIceEdge = body[iBody].daLats[iLat];
      return;
    }
  }
  // LCOV_EXCL_START
  fprintf(stderr, "ERROR: Failure in fvSouthIceCapLand.\n");
  exit(EXIT_INT);
  // LCOV_EXCL_STOP
}

/**
Determines if planet has a southern polar sea ice cap

@param body Struct containing all body information and variables
@param iBody Body in question
@param iLatIceEdge Index of ice edge latitude
@param dLatIceEdge Latitude of ice edge
@param bCap 1 for northern polar ice cap, 0 for ice free north pole
*/
void fvSouthIceCapSea(BODY *body, int iBody, double *dLatIceEdge,
                      int *iLatIceEdge, int *bCap) {

  int iLat;

  // Check for ice at south pole; no ice at -90 => No ice cap
  if (!fbIceLatSea(body, iBody, 0)) {
    // Assign values for no cap
    fvNoIceCap(dLatIceEdge, iLatIceEdge, bCap);
    return;
  }

  // Icy south pole; does ice extend to other pole?
  if (fbSnowballSea(body, iBody)) {
    // Assign values for no cap
    fvNoIceCap(dLatIceEdge, iLatIceEdge, bCap);
    return;
  }

  // If made it here, must be a southern polar cap
  *bCap = 1;
  // Now find ice cap extent
  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (!fbIceLatSea(body, iBody, iLat)) {
      // Found edge!
      *iLatIceEdge = iLat;
      *dLatIceEdge = body[iBody].daLats[iLat];
      return;
    }
  }
  // LCOV_EXCL_START
  fprintf(stderr, "ERROR: Failure in fvSouthIceCapSea.\n");
  exit(EXIT_INT);
  // LCOV_EXCL_STOP
}

/**
Determines if planet has an equatorial ice belt on land

@param body Struct containing all body information and variables
@param iBody Body in question
@param iLatIceEdgeNorth Index of northern ice edge latitude
@param iLatIceEdgeSouth Index of southern ice edge latitude
@param dLatIceEdgeNorth Latitude of northern ice edge
@param dLatIceEdgeSouth Latitude of southern ice edge
@param bBelt 1 for northern polar ice cap, 0 for ice free north pole
*/
void fvIceBeltLand(BODY *body, int iBody, double *dLatIceEdgeNorth,
                   double *dLatIceEdgeSouth, int *iLatIceEdgeNorth,
                   int *iLatIceEdgeSouth, int *bBelt) {

  int bCapNorth, bCapSouth, iIce;
  int iLat, iLatStart, iLatEnd, iLatCapNorth, iLatCapSouth;
  double dLatCapNorth, dLatCapSouth;

  // If IceFree or Snowball, no ice belt
  if (fbSnowballLand(body, iBody)) {
    // Assign values for no belt
    fvNoIceBelt(dLatIceEdgeNorth, dLatIceEdgeSouth, iLatIceEdgeNorth,
                iLatIceEdgeSouth, bBelt);
    return;
  }
  if (fbIceFreeLand(body, iBody)) {
    // Assign values for no belt
    fvNoIceBelt(dLatIceEdgeNorth, dLatIceEdgeSouth, iLatIceEdgeNorth,
                iLatIceEdgeSouth, bBelt);
    return;
  }

  // If made it here, belt is possible, so let's look!
  *iLatIceEdgeNorth = 0;
  *iLatIceEdgeSouth = 0;

  // Get parameters for Northern Ice Cap
  fvNorthIceCapLand(body, iBody, &dLatCapNorth, &iLatCapNorth, &bCapNorth);
  fvSouthIceCapLand(body, iBody, &dLatCapSouth, &iLatCapSouth, &bCapSouth);

  // Assign starting and ending latitudes for belt search
  if (bCapSouth) {
    iLatStart = iLatCapSouth;
  } else {
    iLatStart = 0;
  }
  if (bCapNorth) {
    // Add 1 so as to check final latitude in for loop
    iLatEnd = iLatCapNorth + 1;
  } else {
    iLatEnd = body[iBody].iNumLats;
  }

  // Now start at NP and search for ice belt
  // Start at ice free latitutde
  iIce = 0;
  for (iLat = iLatStart; iLat < iLatEnd; iLat++) {
    if (iIce == 0 && fbIceLatLand(body, iBody, iLat)) {
      // Found southern edge of ice belt!
      *iLatIceEdgeSouth = iLat;
      *dLatIceEdgeSouth = body[iBody].daLats[iLat];
      // Now into ice belt
      iIce = 1;
    }

    if (iIce == 1 && !fbIceLatLand(body, iBody, iLat)) {
      // Found southern edge of ice belt!
      *iLatIceEdgeNorth = iLat;
      *dLatIceEdgeNorth = body[iBody].daLats[iLat];
      // No longer search for ice belt
      iIce = -1;
    }
  }

  // Was a belt found?
  if (*iLatIceEdgeNorth != 0 && *iLatIceEdgeSouth != 0) {
    *bBelt = 1;
  } else {
    // Assign values for no belt
    fvNoIceBelt(dLatIceEdgeNorth, dLatIceEdgeSouth, iLatIceEdgeNorth,
                iLatIceEdgeSouth, bBelt);
  }
}

/**
Determines if planet has an equatorial ice belt on sea

@param body Struct containing all body information and variables
@param iBody Body in question
@param iLatIceEdgeNorth Index of northern ice edge latitude
@param iLatIceEdgeSouth Index of southern ice edge latitude
@param dLatIceEdgeNorth Latitude of northern ice edge
@param dLatIceEdgeSouth Latitude of southern ice edge
@param bBelt 1 for northern polar ice cap, 0 for ice free north pole
*/
void fvIceBeltSea(BODY *body, int iBody, double *dLatIceEdgeNorth,
                  double *dLatIceEdgeSouth, int *iLatIceEdgeNorth,
                  int *iLatIceEdgeSouth, int *bBelt) {

  int bCapNorth, bCapSouth, bIce;
  int iLat, iEquator, iLatStart, iLatEnd, iLatCapNorth, iLatCapSouth;
  double dLatCapNorth, dLatCapSouth;

  // If IceFree or Snowball, no ice belt
  if (fbSnowballSea(body, iBody)) {

    // Assign values for no belt
    fvNoIceBelt(dLatIceEdgeNorth, dLatIceEdgeSouth, iLatIceEdgeNorth,
                iLatIceEdgeSouth, bBelt);

    return;
  }
  if (fbIceFreeSea(body, iBody)) {

    // Assign values for no belt
    fvNoIceBelt(dLatIceEdgeNorth, dLatIceEdgeSouth, iLatIceEdgeNorth,
                iLatIceEdgeSouth, bBelt);
    return;
  }

  // If made it here, belt is possible, so let's look!
  *iLatIceEdgeNorth = 0;
  *iLatIceEdgeSouth = 0;
  iEquator          = (int)(body[iBody].iNumLats / 2);

  // Get parameters for Northern Ice Cap
  fvNorthIceCapSea(body, iBody, &dLatCapNorth, &iLatCapNorth, &bCapNorth);
  fvSouthIceCapSea(body, iBody, &dLatCapSouth, &iLatCapSouth, &bCapSouth);

  // Assign starting and ending latitudes for belt search
  if (bCapNorth) {
    iLatStart = iLatCapNorth;
  } else {
    iLatStart = 0;
  }

  // Assign starting and ending latitudes for belt search
  if (bCapSouth) {
    iLatEnd = iLatCapSouth;
  } else {
    iLatEnd = body[iBody].iNumLats;
  }

  // Now start at NP and search for ice belt
  // Start at ice free latitutde
  bIce = 0;
  for (iLat = iLatStart; iLat < iLatEnd; iLat++) {
    if (!bIce && fbIceLatSea(body, iBody, iLat)) {
      // Found northern edge of ice belt!
      *iLatIceEdgeNorth = iLat;
      // Are we above or below equator?
      if (iLat < iEquator) {
        // Northern hemisphere
        *dLatIceEdgeNorth = iLat / body[iBody].iNumLats * PI;
      } else if (iLat < iEquator) {
        // Southern hemisphere
        *dLatIceEdgeNorth =
              -(body[iBody].iNumLats - iLat - 1) / body[iBody].iNumLats * PI;
      } else {
        // Equator
        *dLatIceEdgeNorth = 0;
      }
      // Now into ice belt
      bIce = 1;
    }

    if (bIce && !fbIceLatSea(body, iBody, iLat)) {
      // Found southern edge of ice belt!
      *iLatIceEdgeSouth = iLat;

      // Are we above or below equator?
      if (iLat < iEquator) {
        // Northern hemisphere
        *dLatIceEdgeSouth = iLat / body[iBody].iNumLats * PI;

      } else if (iLat < iEquator) {
        // Southern hemisphere
        *dLatIceEdgeSouth =
              -(body[iBody].iNumLats - iLat - 1) / body[iBody].iNumLats * PI;

      } else {
        // Equator
        *dLatIceEdgeSouth = 0;
      }
    }
  }

  // Was a belt found?
  if (*iLatIceEdgeNorth != 0 && *iLatIceEdgeSouth != 0) {
    *bBelt = 1;

  } else {
    // Assign values for no belt
    fvNoIceBelt(dLatIceEdgeNorth, dLatIceEdgeSouth, iLatIceEdgeNorth,
                iLatIceEdgeSouth, bBelt);
  }
}

/**
Standard properties function for POISE. Updates auxiliary quantities
(currently just the dynamical ellipticity when DistRot is not enabled).

@param body Struct containing all body information and variables
@param evolve Struct containing evolve information and variables
@param update Struct containing update information and variables
@param iBody Body in question

XXX References to other modules are forbidden in this file!
*/
void fvPropsAuxPoise(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                     int iBody) {
  if (body[iBody].bEqtide && body[iBody].bCalcDynEllip) {
    if (body[iBody].bDistRot == 0) {
      body[iBody].dDynEllip = CalcDynEllipEq(body, iBody);
    }
  }
}

/**
Calculates precession of the spin axis when DistRot is disabled

@param body Struct containing all body information and variables
@param evolve Struct containing evolve information and variables
@param iBody Body in question
*/
void fvPrecessionExplicit(BODY *body, EVOLVE *evolve, int iBody) {
  /* calculates precession angle as an explicit function of time
     for use only when distrot is not called! */

  double ecc2;
  ecc2 = body[iBody].dHecc * body[iBody].dHecc +
         body[iBody].dKecc * body[iBody].dKecc;
  body[iBody].dPrecA =
        3. * BIGG * body[0].dMass /
              (2. *
               (body[iBody].dSemi * body[iBody].dSemi * body[iBody].dSemi) *
               body[iBody].dRotRate) *
              body[iBody].dDynEllip * pow((1.0 - ecc2), -3. / 2) *
              cos(body[iBody].dObliquity) * evolve->dTime +
        body[iBody].dPrecA0;
  while (body[iBody].dPrecA >= 2 * PI) {
    body[iBody].dPrecA -= 2 * PI;
  }
  while (body[iBody].dPrecA < 0) {
    body[iBody].dPrecA += 2 * PI;
  }
}

/**
Forces obliquity oscillation (bForceObliq = 1).

@param body Struct containing all body information and variables
@param evolve Struct containing evolve information and variables
@param iBody Body in question
*/
void fvForceObliq(BODY *body, EVOLVE *evolve, int iBody) {

  double dAmp, dPeriod, dInital;
  dPeriod = body[iBody].dObliqPer * YEARSEC;
  dAmp    = body[iBody].dObliqAmp;
  dInital = body[iBody].dObliq0;

  body[iBody].dObliquity =
        (0.5 * dAmp * sin(2 * PI * evolve->dTime / dPeriod)) * DEGRAD + dInital;
  CalcXYZobl(body, iBody);
}

/**
Forces eccentricity oscillation (bForceEcc = 1).

@param body Struct containing all body information and variables
@param evolve Struct containing evolve information and variables
@param iBody Body in question
*/
void fvForceEcc(BODY *body, EVOLVE *evolve, int iBody) {

  double dAmp, dPeriod, dInital;
  dPeriod = body[iBody].dEccPer * YEARSEC;
  dAmp    = body[iBody].dEccAmp;
  dInital = body[iBody].dEcc0;

  body[iBody].dEcc =
        (0.5 * dAmp * sin(2 * PI * evolve->dTime / dPeriod)) + dInital;
  CalcHK(body, iBody);
}

/**
Calculates the fractional ice covered area of the planet

@param body Struct containing all body information and variables
@param iBody Body in question

XXX This looks wrong and inconsistent with other ice outputs. This returns the
fraction of latitudes that are ice-covered, not the fractional area.
*/
void fvAreaIceCovered(BODY *body, int iBody) {
  // area permanently ice covered (i.e., all year round)

  int iLat;

  body[iBody].dAreaIceCov = 0;
  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {

    if (body[iBody].iClimateModel == SEA) {

      if (body[iBody].daIceMass[iLat] > 0 ||
          body[iBody].daTempMaxLand[iLat] <= -2.0) {

        body[iBody].dAreaIceCov += body[iBody].daLandFrac[iLat];
      }
    } else {

      if (body[iBody].daTempMaxLand[iLat] <= -10.0) {

        body[iBody].dAreaIceCov += body[iBody].daLandFrac[iLat];
      }
    }
    if (body[iBody].daTempMaxWater[iLat] < -2) {

      body[iBody].dAreaIceCov += body[iBody].daWaterFrac[iLat];
    }
  }

  body[iBody].dAreaIceCov /= body[iBody].iNumLats;
}

/**
Force behavior function for POISE. This is where the magic happens--
POISE works entirely in force behavior because it needs to solve several
PDEs (not well modeled with RK4). First, the EBM (climate) is run for several
orbits, then the ice sheet flow. The EBM can be periodically rerun needed.

@param body Struct containing all body information and variables
@param module Struct containing module/intermodule quantities
@param evolve Struct containing evolve information and variables
@param io Struct containing read/write information
@param system Struct containing system-wide information
@param update Struct containing update information and variables
@param fnUpdate Array of function pointers to derivatives
@param iBody Body in question
@param iModule Module in question
*/
void ForceBehaviorPoise(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                        SYSTEM *system, UPDATE *update,
                        fnUpdateVariable ***fnUpdate, int iBody, int iModule) {

  int iLat;

  // XXX Conflicts here

  if (body[iBody].bEqtide) {

    body[iBody].dMeanMotion = fdSemiToMeanMotion(
          body[iBody].dSemi, body[0].dMass + body[iBody].dMass);
    body[iBody].iNDays =
          (int)floor(body[iBody].dRotRate / body[iBody].dMeanMotion);
    if (body[iBody].iClimateModel == SEA) {

      VerifyNStepSeasonal(body, iBody);
    }
  }

  if (body[iBody].bDistRot == 0) {

    fvPrecessionExplicit(body, evolve, iBody);
    if (body[iBody].bForceObliq) {

      fvForceObliq(body, evolve, iBody);
    }
    if (body[iBody].bForceEcc) {

      fvForceEcc(body, evolve, iBody);
    }
  }

  if (body[iBody].iClimateModel == ANN || body[iBody].bSkipSeasEnabled) {
    if (body[iBody].iClimateModel == SEA) {

      body[iBody].dSurfAlbedo =
            (body[iBody].dAlbedoLand + body[iBody].dAlbedoWater) / 2.0;
    }

    PoiseAnnual(body, iBody);
  }

  if (body[iBody].iClimateModel == SEA) {

    if (body[iBody].bSkipSeas == 0) {
      // total change in ice mass this time step
      body[iBody].dIceBalanceTot = 0.0;
      // total ice flow (should equal zero)
      body[iBody].dIceFlowTot = 0.0;
      body[iBody].dIceMassTot = 0.0;

      if (body[iBody].bIceSheets) {
        PoiseIceSheets(body, evolve, iBody);
      }
    }

    if (body[iBody].bSkipSeasEnabled) {

      if (body[iBody].dFluxOutGlobal >= 300 ||
          body[iBody].dAlbedoGlobal >= body[iBody].dIceAlbedo) {

        body[iBody].bSkipSeas = 1;

      } else {

        body[iBody].bSkipSeas = 0;
      }
    }

    if (body[iBody].bSkipSeas == 0) {

      fvAnnualInsolation(body, iBody);
      fvMatrixSeasonal(body, iBody);
      PoiseSeasonal(body, iBody);
    }
  }

  if (body[iBody].bSkipSeas == 0) {

    if (body[iBody].bIceSheets) {

      for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {

        if (body[iBody].daIceMass[iLat] < 0) {

          body[iBody].daIceMass[iLat] = 0.0;
        }
        if (body[iBody].iClimateModel == SEA) {

          body[iBody].dIceMassTot +=
                body[iBody].daIceMass[iLat] *
                (2 * PI * (body[iBody].dRadius * body[iBody].dRadius) *
                 (sin(body[iBody].daLats[1]) - sin(body[iBody].daLats[0]))) *
                body[iBody].daLandFrac[iLat];

          body[iBody].dIceBalanceTot += body[iBody].daIceBalanceAvg[iLat];
          body[iBody].dIceFlowTot += body[iBody].daIceFlowAvg[iLat];
        }
      }
    }
  }
}


/**
Calculates eccentric anomaly from true anomaly

@param dTrueA Planet's true anomaly
@param dEcc Planet's orbital eccentricity
@return Eccentric anomaly
*/
double fdEccTrueAnomaly(double dTrueA, double dEcc) {

  double dCosE;
  dCosE = (cos(dTrueA) + dEcc) / (1.0 + dEcc * cos(dTrueA));

  if (dTrueA < PI) {

    return acos(dCosE);
  } else {

    return (2 * PI - acos(dCosE));
  }
}

/**
Calculates insolation at each latitude for a single "iNday" of the year/orbit

@param body Struct containing all body information and variables
@param iBody Body in question
@param iDay Day of the year
*/
void fvDailyInsolation(BODY *body, int iBody, int iDay) {

  int iLat;
  double dSconst, dSinDelta, dCosDelta, dTanDelta, dDelta, HA;

  // HA = dHourAngle ,dSconst = dStellarConst,dSinDelta = dSinDelta,
  // dCosDelta =

  dSconst = body[0].dLuminosity /
            (4. * PI * (body[iBody].dSemi * body[iBody].dSemi));

  dSinDelta = sin(body[iBody].dObliquity) * sin(body[iBody].dTrueL);
  dCosDelta = sqrt(1.0 - (dSinDelta * dSinDelta));
  dTanDelta = dSinDelta / dCosDelta;
  dDelta    = asin(dSinDelta);
  body[iBody].daDeclination[iDay] = dDelta;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (dDelta > 0.0) {
      /* Northern summer */
      if (body[iBody].daLats[iLat] >= (PI / 2. - dDelta)) {
        /* white night/no sunset */
        body[iBody].daInsol[iLat][iDay] =
              dSconst * sin(body[iBody].daLats[iLat]) * dSinDelta /
              (body[iBody].dAstroDist * body[iBody].dAstroDist);

      } else if (-body[iBody].daLats[iLat] >= (PI / 2. - dDelta) &&
                 body[iBody].daLats[iLat] < 0.0) {
        /* polar darkness/no sunrise */
        body[iBody].daInsol[iLat][iDay] = 0.0;
      } else {
        /* regular iNday/night cycle */
        HA = acos(-tan(body[iBody].daLats[iLat]) * dTanDelta);
        body[iBody].daInsol[iLat][iDay] =
              dSconst *
              (HA * sin(body[iBody].daLats[iLat]) * dSinDelta +
               cos(body[iBody].daLats[iLat]) * dCosDelta * sin(HA)) /
              (PI * (body[iBody].dAstroDist * body[iBody].dAstroDist));
      }
    } else {
      /* Southern summer */
      if (-body[iBody].daLats[iLat] >= (PI / 2. + dDelta) &&
          body[iBody].daLats[iLat] < 0.0) {
        /* white night/no sunset */
        body[iBody].daInsol[iLat][iDay] =
              dSconst * sin(body[iBody].daLats[iLat]) * dSinDelta /
              (body[iBody].dAstroDist * body[iBody].dAstroDist);

      } else if (body[iBody].daLats[iLat] >= (PI / 2. + dDelta)) {
        /* polar darkness/no sunrise */
        body[iBody].daInsol[iLat][iDay] = 0.0;

      } else {

        /* regular iNday/night cycle */
        HA = acos(-tan(body[iBody].daLats[iLat]) * dTanDelta);
        body[iBody].daInsol[iLat][iDay] =
              dSconst *
              (HA * sin(body[iBody].daLats[iLat]) * dSinDelta +
               cos(body[iBody].daLats[iLat]) * dCosDelta * sin(HA)) /
              (PI * (body[iBody].dAstroDist * body[iBody].dAstroDist));
      }
    }
  }
}

/**
Calculates insolation at each latitude over the entire year/orbit

@param body Struct containing all body information and variables
@param iBody Body in question
*/
void fvAnnualInsolation(BODY *body, int iBody) {
  int iDays, iLat;
  double dLongP, dTrueA, dEccA, dMeanL, dEcc;
  // dLongP
  // Pericenter, relative to direction of planet at spring equinox
  dLongP = body[iBody].dLongP + body[iBody].dPrecA + PI;
  dEcc   = sqrt(body[iBody].dHecc * body[iBody].dHecc +
                body[iBody].dKecc * body[iBody].dKecc);

  // starts the year at the (northern) winter solstice
  body[iBody].dTrueL = -PI / 2;
  dTrueA             = body[iBody].dTrueL - dLongP;

  while (dTrueA < 0.0) {
    dTrueA += 2 * PI;
  }
  body[iBody].dObliquity = atan2(sqrt((body[iBody].dXobl * body[iBody].dXobl) +
                                      (body[iBody].dYobl * body[iBody].dYobl)),
                                 body[iBody].dZobl);
  dEccA                  = fdEccTrueAnomaly(dTrueA, dEcc);
  dMeanL                 = dEccA - dEcc * sin(dEccA) + dLongP;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    body[iBody].daAnnualInsol[iLat] = 0.0;
    body[iBody].daPeakInsol[iLat]   = 0.0;
  }

  for (iDays = 0; iDays < body[iBody].iNDays; iDays++) {
    if (iDays != 0) {

      dMeanL = dMeanL + 2 * PI / body[iBody].iNDays;
      /* This will only work for the secular orbital model.
         Will need to be changed when/if resonances are added. */
      body[iBody].dMeanA = dMeanL - dLongP;
      kepler_eqn(body, iBody);
      dEccA = body[iBody].dEccA;
      while (dEccA >= 2 * PI) {
        dEccA -= 2 * PI;
      }
      while (dEccA < 0.0) {
        dEccA += 2 * PI;
      }
      if (dEccA > PI) {
        dTrueA = 2 * PI - acos((cos(dEccA) - dEcc) / (1.0 - dEcc * cos(dEccA)));
      } else {
        dTrueA = acos((cos(dEccA) - dEcc) / (1.0 - dEcc * cos(dEccA)));
      }
      body[iBody].dTrueL = dTrueA + dLongP;
    }

    while (body[iBody].dTrueL > 2 * PI) {
      body[iBody].dTrueL -= 2 * PI;
    }
    while (body[iBody].dTrueL < 0.0) {
      body[iBody].dTrueL += 2 * PI;
    }

    // planet-star distance (units of semi-major axis):
    body[iBody].dAstroDist = (1.0 - (dEcc * dEcc)) / (1.0 + dEcc * cos(dTrueA));

    fvDailyInsolation(body, iBody, iDays);

    for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
      body[iBody].daAnnualInsol[iLat] +=
            body[iBody].daInsol[iLat][iDays] / ((double)body[iBody].iNDays);

      if (body[iBody].daInsol[iLat][iDays] > body[iBody].daPeakInsol[iLat]) {
        body[iBody].daPeakInsol[iLat] = body[iBody].daInsol[iLat][iDays];
      }
    }
  }
}

/**
Calculates/updates the albedo at each latitude for the annual EBM

@param body Struct containing all body information and variables
@param iBody Body in question
*/
void fvAlbedoAnnual(BODY *body, int iBody) {
  int iLat;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (body[iBody].dFixIceLat) {

      /* Cold poles */
      if (fabs(body[iBody].daLats[iLat]) > body[iBody].dFixIceLat) {
        body[iBody].daAlbedoAnn[iLat] = body[iBody].dIceAlbedo;
      } else {
        body[iBody].daAlbedoAnn[iLat] = body[iBody].dSurfAlbedo;
      }
    } else if (body[iBody].bAlbedoZA) {
      /* Use albedo based on zenith angle */
      body[iBody].daAlbedoAnn[iLat] =
            0.31 + 0.04 * (3 * (sin(body[iBody].daLats[iLat]) *
                                sin(body[iBody].daLats[iLat])) -
                           1);
    } else {
      // !!! Does this next line mean sea ice only freezes it T<-10? Seems cold!
      if (body[iBody].daTempAnn[iLat] <= -10.0) {
        body[iBody].daAlbedoAnn[iLat] = body[iBody].dIceAlbedo;
      } else {
        body[iBody].daAlbedoAnn[iLat] = body[iBody].dSurfAlbedo;
      }
    }
  }
}

/**
Inverts the matrix for the annual EBM

@param body Struct containing all body information and variables
@param iBody Body in question
*/
void fvMatrixInvertAnnual(BODY *body, int iBody) {
  int iLat, jLat;

  LUDecomp(body[iBody].daMEulerAnn, body[iBody].daMEulerCopyAnn,
           body[iBody].daScaleAnn, body[iBody].iaRowswapAnn,
           body[iBody].iNumLats);

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    for (jLat = 0; jLat < body[iBody].iNumLats; jLat++) {
      if (jLat == iLat) {
        body[iBody].daUnitVAnn[jLat] = 1.0;
      } else {
        body[iBody].daUnitVAnn[jLat] = 0.0;
      }
    }

    LUSolve(body[iBody].daMEulerCopyAnn, body[iBody].daUnitVAnn,
            body[iBody].iaRowswapAnn, body[iBody].iNumLats);

    for (jLat = 0; jLat < body[iBody].iNumLats; jLat++) {
      body[iBody].daInvMAnn[jLat][iLat] = body[iBody].daUnitVAnn[jLat];
    }
  }
}

/**
Inverts the matrix for the seasonal EBM

@param body Struct containing all body information and variables
@param iBody Body in question
*/
void fvMatrixInvertSeasonal(BODY *body, int iBody) {
  int iLat, jLat;

  LUDecomp(body[iBody].daMEulerSea, body[iBody].daMEulerCopySea,
           body[iBody].daScaleSea, body[iBody].iaRowswapSea,
           2 * body[iBody].iNumLats);

  for (iLat = 0; iLat < 2 * body[iBody].iNumLats; iLat++) {
    for (jLat = 0; jLat < 2 * body[iBody].iNumLats; jLat++) {
      if (jLat == iLat) {
        body[iBody].daUnitVSea[jLat] = 1.0;
      } else {
        body[iBody].daUnitVSea[jLat] = 0.0;
      }
    }

    LUSolve(body[iBody].daMEulerCopySea, body[iBody].daUnitVSea,
            body[iBody].iaRowswapSea, 2 * body[iBody].iNumLats);

    for (jLat = 0; jLat < 2 * body[iBody].iNumLats; jLat++) {
      body[iBody].daInvMSea[jLat][iLat] = body[iBody].daUnitVSea[jLat];
    }
  }
}

/**
Calculates the approximate temperature gradient in latitude for the annual EBM

@param body Struct containing all body information and variables
@param dDelta_x Spacing of grid (x = sin(latitude))
@param iBody Body in question
*/
void fvTempGradientAnn(BODY *body, double dDelta_x, int iBody) {
  int iLat;

  body[iBody].daTGrad[0] =
        (body[iBody].daTempAnn[1] - body[iBody].daTempAnn[0]) / (dDelta_x);

  for (iLat = 1; iLat < body[iBody].iNumLats - 1; iLat++) {
    body[iBody].daTGrad[iLat] =
          (body[iBody].daTempAnn[iLat + 1] - body[iBody].daTempAnn[iLat - 1]) /
          (2 * dDelta_x);
  }
  body[iBody].daTGrad[body[iBody].iNumLats - 1] =
        (body[iBody].daTempAnn[body[iBody].iNumLats - 1] -
         body[iBody].daTempAnn[body[iBody].iNumLats - 2]) /
        (dDelta_x);
}

/**
Calculates the approximate temperature gradient in latitude for the seasonal EBM

@param body Struct containing all body information and variables
@param dDelta_x Spacing of grid (x = sin(latitude))
@param iBody Body in question
*/
void fvTempGradientSea(BODY *body, double dDelta_x, int iBody) {
  int iLat;

  body[iBody].daTGrad[0] =
        (body[iBody].daTempLW[1] - body[iBody].daTempLW[0]) / (dDelta_x);

  for (iLat = 1; iLat < body[iBody].iNumLats - 1; iLat++) {
    body[iBody].daTGrad[iLat] =
          (body[iBody].daTempLW[iLat + 1] - body[iBody].daTempLW[iLat - 1]) /
          (2 * dDelta_x);
  }
  body[iBody].daTGrad[body[iBody].iNumLats - 1] =
        (body[iBody].daTempLW[body[iBody].iNumLats - 1] -
         body[iBody].daTempLW[body[iBody].iNumLats - 2]) /
        (dDelta_x);
}

/**
Calculates the backwards-Euler matrix that integrates the annual EBM. Each
operation of the inverse matrix is a "time-step".

@param body Struct containing all body information and variables
@param iBody Body in question
*/
void fvMatrixAnnual(BODY *body, int iBody) {
  int iLat, jLat;
  double dDelta_t;

  dDelta_t = 1.5 / body[iBody].iNumLats;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    body[iBody].daTempTerms[iLat] = 0.0;

    for (jLat = 0; jLat < body[iBody].iNumLats; jLat++) {
      if (jLat == iLat) {

        body[iBody].daMClim[iLat][jLat] = (-body[iBody].daPlanckBAnn[iLat] -
                                           body[iBody].daLambdaAnn[iLat + 1] -
                                           body[iBody].daLambdaAnn[iLat]) /
                                          body[iBody].dHeatCapAnn;

        body[iBody].daMDiffAnn[iLat][jLat] =
              (-body[iBody].daLambdaAnn[iLat + 1] -
               body[iBody].daLambdaAnn[iLat]);

        body[iBody].daMEulerAnn[iLat][jLat] = -1.0 / dDelta_t;

      } else if (jLat == (iLat + 1)) {
        body[iBody].daMClim[iLat][jLat] =
              body[iBody].daLambdaAnn[jLat] / body[iBody].dHeatCapAnn;

        body[iBody].daMDiffAnn[iLat][jLat]  = body[iBody].daLambdaAnn[jLat];
        body[iBody].daMEulerAnn[iLat][jLat] = 0.0;

      } else if (jLat == (iLat - 1)) {
        body[iBody].daMClim[iLat][jLat] =
              body[iBody].daLambdaAnn[iLat] / body[iBody].dHeatCapAnn;

        body[iBody].daMDiffAnn[iLat][jLat]  = body[iBody].daLambdaAnn[iLat];
        body[iBody].daMEulerAnn[iLat][jLat] = 0.0;

      } else {
        body[iBody].daMClim[iLat][jLat]     = 0.0;
        body[iBody].daMDiffAnn[iLat][jLat]  = 0.0;
        body[iBody].daMEulerAnn[iLat][jLat] = 0.0;
      }
      body[iBody].daMEulerAnn[iLat][jLat] +=
            0.5 * body[iBody].daMClim[iLat][jLat];

      body[iBody].daTempTerms[iLat] +=
            body[iBody].daMClim[iLat][jLat] * body[iBody].daTempAnn[jLat];
    }

    body[iBody].daSourceF[iLat] = ((1.0 - body[iBody].daAlbedoAnn[iLat]) *
                                         body[iBody].daAnnualInsol[iLat] -
                                   body[iBody].daPlanckAAnn[iLat]) /
                                  body[iBody].dHeatCapAnn;

    body[iBody].daTempTerms[iLat] += body[iBody].daSourceF[iLat];
  }
}

/**
Runs the annual EBM, called from ForceBehavior. The annual EBM runs to
steady-state (global temperature change becomes very small), rather than for a
set number of orbits.

@param body Struct containing all body information and variables
@param iBody Body in question
*/
void PoiseAnnual(BODY *body, int iBody) {

  double dDelta_t, dDelta_x, dXBoundary, dTempChange, dTmpTGlobal;
  int iIterMax, iLat, jLat, iIter;

  /* Get current climate parameters */
  fvAlbedoAnnual(body, iBody);
  fvAnnualInsolation(body, iBody);
  if (body[iBody].bCalcAB) {
    for (iLat = 0; iLat <= body[iBody].iNumLats; iLat++) {
      if (iLat != body[iBody].iNumLats) {
        if (body[iBody].iOLRModel == WK97) {
          body[iBody].daPlanckBAnn[iLat] = fdOLRdTwk97(body, iBody, iLat, ANN);
          body[iBody].daPlanckAAnn[iLat] =
                fdOLRwk97(body, iBody, iLat, ANN) -
                body[iBody].daPlanckBAnn[iLat] * (body[iBody].daTempAnn[iLat]);

        } else if (body[iBody].iOLRModel == HM16) {
          body[iBody].daPlanckBAnn[iLat] = fdOLRdThm16(body, iBody, iLat, ANN);
          body[iBody].daPlanckAAnn[iLat] =
                fdOLRhm16(body, iBody, iLat, ANN) -
                body[iBody].daPlanckBAnn[iLat] * (body[iBody].daTempAnn[iLat]);
        } else {
          body[iBody].daPlanckBAnn[iLat] = fdOLRdTsms09(body, iBody, iLat, ANN);
          body[iBody].daPlanckAAnn[iLat] =
                fdOLRsms09(body, iBody, iLat, ANN) -
                body[iBody].daPlanckBAnn[iLat] * (body[iBody].daTempAnn[iLat]);
        }
      }

      if (body[iBody].bMEPDiff) {
        if (iLat == 0) {
          body[iBody].daDiffusionAnn[iLat] =
                body[iBody].daPlanckBAnn[iLat] / 4.0;
        } else if (iLat == body[iBody].iNumLats) {
          body[iBody].daDiffusionAnn[iLat] =
                body[iBody].daPlanckBAnn[iLat - 1] / 4.0;
        } else {
          body[iBody].daDiffusionAnn[iLat] =
                (body[iBody].daPlanckBAnn[iLat] +
                 body[iBody].daPlanckBAnn[iLat - 1]) /
                8.0;
        }
      }
    }
  }

  dDelta_t = 1.5 / body[iBody].iNumLats;
  dDelta_x = 2.0 / body[iBody].iNumLats;
  iIterMax = 2000;

  /* Setup matrices, source function, temperature terms, global mean */
  for (iLat = 0; iLat < body[iBody].iNumLats + 1; iLat++) {
    dXBoundary                    = -1.0 + iLat * 2.0 / body[iBody].iNumLats;
    body[iBody].daLambdaAnn[iLat] = body[iBody].daDiffusionAnn[iLat] *
                                    (1.0 - (dXBoundary * dXBoundary)) /
                                    ((dDelta_x * dDelta_x));
  }

  body[iBody].dTGlobal = 0.0;
  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    body[iBody].dTGlobal += body[iBody].daTempAnn[iLat] / body[iBody].iNumLats;
  }
  fvMatrixAnnual(body, iBody);
  fvMatrixInvertAnnual(body, iBody);

  /* Relaxation to equilibrium */
  iIter       = 1;
  dTempChange = 1.0;
  while (fabs(dTempChange) > 1e-12) {
    dTmpTGlobal = 0.0;
    for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
      body[iBody].daTmpTempAnn[iLat] = 0.0;

      for (jLat = 0; jLat < body[iBody].iNumLats; jLat++) {
        body[iBody].daTmpTempAnn[iLat] +=
              -body[iBody].daInvMAnn[iLat][jLat] *
              (0.5 * (body[iBody].daTempTerms[jLat] +
                      body[iBody].daSourceF[jLat]) +
               body[iBody].daTempAnn[jLat] / dDelta_t);
      }
      dTmpTGlobal += body[iBody].daTmpTempAnn[iLat] / body[iBody].iNumLats;
    }

    for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
      body[iBody].daTmpTempTerms[iLat] = body[iBody].daSourceF[iLat];

      for (jLat = 0; jLat < body[iBody].iNumLats; jLat++) {
        body[iBody].daTmpTempTerms[iLat] +=
              body[iBody].daMClim[iLat][jLat] * body[iBody].daTmpTempAnn[jLat];
      }
    }

    dTempChange = dTmpTGlobal - body[iBody].dTGlobal;

    /* Update albedo, source function, temperature,
       temperature terms, and global mean */
    fvAlbedoAnnual(body, iBody);
    for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
      body[iBody].daSourceF[iLat] = ((1.0 - body[iBody].daAlbedoAnn[iLat]) *
                                           body[iBody].daAnnualInsol[iLat] -
                                     body[iBody].daPlanckAAnn[iLat]) /
                                    body[iBody].dHeatCapAnn;
      body[iBody].daTempAnn[iLat]   = body[iBody].daTmpTempAnn[iLat];
      body[iBody].daTempTerms[iLat] = body[iBody].daTmpTempTerms[iLat];
      body[iBody].dTGlobal          = dTmpTGlobal;
    }
    // LCOV_EXCL_START
    if (iIter >= iIterMax) {
      fprintf(stderr,
              "POISE solution not converged before max iterations reached.\n");
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }
    if (body[iBody].bCalcAB == 1) {
      for (iLat = 0; iLat <= body[iBody].iNumLats; iLat++) {
        if (iLat != body[iBody].iNumLats) {
          if (body[iBody].iOLRModel == WK97) {
            body[iBody].daPlanckBAnn[iLat] =
                  fdOLRdTwk97(body, iBody, iLat, ANN);
            body[iBody].daPlanckAAnn[iLat] =
                  fdOLRwk97(body, iBody, iLat, ANN) -
                  body[iBody].daPlanckBAnn[iLat] *
                        (body[iBody].daTempAnn[iLat]);

          } else if (body[iBody].iOLRModel == HM16) {
            body[iBody].daPlanckBAnn[iLat] =
                  fdOLRdThm16(body, iBody, iLat, ANN);
            body[iBody].daPlanckAAnn[iLat] =
                  fdOLRhm16(body, iBody, iLat, ANN) -
                  body[iBody].daPlanckBAnn[iLat] *
                        (body[iBody].daTempAnn[iLat]);

          } else {
            body[iBody].daPlanckBAnn[iLat] =
                  fdOLRdTsms09(body, iBody, iLat, ANN);
            body[iBody].daPlanckAAnn[iLat] =
                  fdOLRsms09(body, iBody, iLat, ANN) -
                  body[iBody].daPlanckBAnn[iLat] *
                        (body[iBody].daTempAnn[iLat]);
          }
        }

        if (body[iBody].bMEPDiff) {
          if (iLat == 0) {
            body[iBody].daDiffusionAnn[iLat] =
                  body[iBody].daPlanckBAnn[iLat] / 4.0;

          } else if (iLat == body[iBody].iNumLats) {
            body[iBody].daDiffusionAnn[iLat] =
                  body[iBody].daPlanckBAnn[iLat - 1] / 4.0;

          } else {
            body[iBody].daDiffusionAnn[iLat] =
                  (body[iBody].daPlanckBAnn[iLat] +
                   body[iBody].daPlanckBAnn[iLat - 1]) /
                  8.0;
          }
          dXBoundary = -1.0 + iLat * 2.0 / body[iBody].iNumLats;
          body[iBody].daLambdaAnn[iLat] = body[iBody].daDiffusionAnn[iLat] *
                                          (1.0 - (dXBoundary * dXBoundary)) /
                                          ((dDelta_x * dDelta_x));
        }
      }
      fvMatrixAnnual(body, iBody);
      fvMatrixInvertAnnual(body, iBody);
    }
    iIter++;
  }

  /* Calculate some interesting quantities */
  body[iBody].dFluxInGlobal  = 0.0;
  body[iBody].dFluxOutGlobal = 0.0;
  body[iBody].dAlbedoGlobal  = 0.0;
  body[iBody].dIceMassTot    = 0.0;
  fvTempGradientAnn(body, dDelta_x, iBody);
  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    body[iBody].daDMidPt[iLat] = 0.5 * (body[iBody].daDiffusionAnn[iLat + 1] +
                                        body[iBody].daDiffusionAnn[iLat]);

    body[iBody].daFlux[iLat] =
          -2. * PI * (body[iBody].dRadius * body[iBody].dRadius) *
          sqrt(1.0 - (sin(body[iBody].daLats[iLat]) *
                      sin(body[iBody].daLats[iLat]))) *
          body[iBody].daDMidPt[iLat] * body[iBody].daTGrad[iLat];

    body[iBody].daFluxIn[iLat] = (1.0 - body[iBody].daAlbedoAnn[iLat]) *
                                 body[iBody].daAnnualInsol[iLat];

    body[iBody].daFluxOut[iLat] =
          body[iBody].daPlanckAAnn[iLat] +
          body[iBody].daPlanckBAnn[iLat] * body[iBody].daTempAnn[iLat];

    body[iBody].dFluxInGlobal +=
          body[iBody].daFluxIn[iLat] / body[iBody].iNumLats;

    body[iBody].dFluxOutGlobal +=
          body[iBody].daFluxOut[iLat] / body[iBody].iNumLats;

    body[iBody].daDivFlux[iLat] = 0.0;
    for (jLat = 0; jLat < body[iBody].iNumLats; jLat++) {
      body[iBody].daDivFlux[iLat] +=
            -body[iBody].daMDiffAnn[iLat][jLat] * body[iBody].daTempAnn[jLat];
    }
    body[iBody].dAlbedoGlobal +=
          body[iBody].daAlbedoAnn[iLat] / body[iBody].iNumLats;
  }
}

/**
Calculates the OLR from the Haqq-Misra+ 2016 formulae

@param body Struct containing all body information
@param iBody Body in question
@param iLat Latitude at which you want to calculate the OLR
@param bModel Type of EBM (annual or seasonal)
*/


double fdOLRhm16(BODY *body, int iBody, int iLat, int bModel) {
  double phi, Int, tmpk, f, dT;

  phi = log10(body[iBody].dpCO2);
  if (bModel == ANN) {
    dT = body[iBody].daTempAnn[iLat] + 273.15;
  } else {
    dT = body[iBody].daTempLW[iLat] + 273.15;
  }
  if (dT > 150) {
    tmpk = log10(dT);
    f    = 9.12805643869791438760 * (tmpk * tmpk * tmpk * tmpk) +
        4.58408794768168803557 * (tmpk * tmpk * tmpk) * phi -
        8.47261075643147449910e+01 * (tmpk * tmpk * tmpk) +
        4.35517381112690282752e-01 * (tmpk * phi * tmpk * phi) -
        2.86355036260417961103e+01 * (tmpk * tmpk) * phi +
        2.96626642498045896446e+02 * (tmpk * tmpk) -
        6.01082900358299240806e-02 * tmpk * (phi * phi * phi) -
        2.60414691486954641420 * tmpk * (phi * phi) +
        5.69812976563675661623e+01 * tmpk * phi -
        4.62596100127381816947e+02 * tmpk +
        2.18159373001564722491e-03 * (phi * phi * phi * phi) +
        1.61456772400726950023e-01 * (phi * phi * phi) +
        3.75623788187470086797 * (phi * phi) -
        3.53347289223180354156e+01 * phi + 2.75011005409836684521e+02;

    Int = pow(10.0, f) / 1000.;
  } else {
    Int = SIGMA * dT * dT * dT * dT; // very cold brrr....
  }
  return Int;
}

/**
Calculates the slope of the OLR from the Haqq-Misra+ 2016 formulae

@param body Struct containing all body information
@param iBody Body in question
@param iLat Latitude at which you want to calculate the OLR
@param bModel Type of EBM (annual or seasonal)
*/
double fdOLRdThm16(BODY *body, int iBody, int iLat, int bModel) {
  double phi, dI, tmpk, f, dT;

  phi = log10(body[iBody].dpCO2);
  if (bModel == ANN) {
    dT = body[iBody].daTempAnn[iLat] + 273.15;
  } else {
    dT = body[iBody].daTempLW[iLat] + 273.15;
  }
  tmpk = log10(dT);
  f    = 4 * 9.12805643869791438760 * (tmpk * tmpk * tmpk) +
      3 * 4.58408794768168803557 * (tmpk * tmpk) * phi -
      3 * 8.47261075643147449910e+01 * (tmpk * tmpk) +
      2 * 4.35517381112690282752e-01 * tmpk * (phi * phi) -
      2 * 2.86355036260417961103e+01 * tmpk * phi +
      2 * 2.96626642498045896446e+02 * tmpk -
      6.01082900358299240806e-02 * (phi * phi * phi) -
      2.60414691486954641420 * (phi * phi) + 5.69812976563675661623e+01 * phi -
      4.62596100127381816947e+02;
  dI = fdOLRhm16(body, iBody, iLat, bModel) * f /
       (body[iBody].daTempLW[iLat] + 273.15);

  if (dI <= 0) {
    dI = fdOLRdTwk97(body, iBody, iLat, bModel);
  }
  return dI;
}

/**
Calculates the OLR from the Spiegel+ 2009 formulae

@param body Struct containing all body information
@param iBody Body in question
@param iLat Latitude at which you want to calculate the OLR
@param bModel Type of EBM (annual or seasonal)
*/
double fdOLRsms09(BODY *body, int iBody, int iLat, int bModel) {
  double tau, Int, T;

  if (bModel == ANN) {
    T = body[iBody].daTempAnn[iLat] + 273.15;
  } else {
    T = body[iBody].daTempLW[iLat] + 273.15;
  }
  tau = 0.79 * (T / 273.15) * (T / 273.15) * (T / 273.15);

  Int = SIGMA * T * T * T * T / (1.0 + 0.75 * tau);
  return Int;
}

/**
Calculates the slope of the OLR from the Spiegel+ 2009 formulae

@param body Struct containing all body information
@param iBody Body in question
@param iLat Latitude at which you want to calculate the OLR
@param bModel Type of EBM (annual or seasonal)
*/
double fdOLRdTsms09(BODY *body, int iBody, int iLat, int bModel) {
  double tau, dtau, dI, T;

  if (bModel == ANN) {
    T = body[iBody].daTempAnn[iLat] + 273.15;
  } else {
    T = body[iBody].daTempLW[iLat] + 273.15;
  }
  tau  = 0.79 * (T / 273.15) * (T / 273.15) * (T / 273.15);
  dtau = 3.0 * 0.79 * T * T / (273.15 * 273.15 * 273.15);

  dI = SIGMA * (4 * T * T * T / (1.0 + 0.75 * tau) -
                T * T * T * T / (1. + 0.75 * tau) / (1. + 0.75 * tau) * 0.75 *
                      tau * dtau);
  return dI;
}

/**
Calculates the OLR from the Williams & Kasting 1997 formulae

@param body Struct containing all body information
@param iBody Body in question
@param iLat Latitude at which you want to calculate the OLR
@param bModel Type of EBM (annual or seasonal)
*/
double fdOLRwk97(BODY *body, int iBody, int iLat, int bModel) {
  double phi, Int, T;
  // T = temp
  // Int = OLR
  // phi = normalized log partial pressure of CO2
  phi = log(body[iBody].dpCO2 / 3.3e-4);
  if (bModel == ANN) {
    T = body[iBody].daTempAnn[iLat] + 273.15;
  } else {
    T = body[iBody].daTempLW[iLat] + 273.15;
  }
  Int = 9.468980 - 7.714727e-5 * phi - 2.794778 * T - 3.244753e-3 * phi * T -
        3.547406e-4 * (phi * phi) + 2.212108e-2 * (T * T) +
        2.229142e-3 * (phi * phi) * T + 3.088497e-5 * phi * (T * T) -
        2.789815e-5 * (phi * T * phi * T) - 3.442973e-3 * (phi * phi * phi) -
        3.361939e-5 * (T * T * T) + 9.173169e-3 * (phi * phi * phi) * T -
        7.775195e-5 * (phi * phi * phi) * (T * T) -
        1.679112e-7 * phi * (T * T * T) +
        6.590999e-8 * (phi * phi) * (T * T * T) +
        1.528125e-7 * (phi * phi * phi) * (T * T * T) -
        3.367567e-2 * (phi * phi * phi * phi) -
        1.631909e-4 * (phi * phi * phi * phi) * T +
        3.663871e-6 * (phi * phi * phi * phi) * (T * T) -
        9.255646e-9 * (phi * phi * phi * phi) * (T * T * T);
  if (Int >= 300) {
    Int = 300.0;
  }
  if (T < 190) {
    Int = SIGMA * T * T * T * T;
  }
  return Int;
}

/**
Calculates the slope of the  OLR from the Williams & Kasting 1997 formulae

@param body Struct containing all body information
@param iBody Body in question
@param iLat Latitude at which you want to calculate the OLR
@param bModel Type of EBM (annual or seasonal)
*/
double fdOLRdTwk97(BODY *body, int iBody, int iLat, int bModel) {
  double phi, dI, T;

  phi = log(body[iBody].dpCO2 / 3.3e-4);
  if (bModel == ANN) {
    // printf("%lf\n",body[iBody].daTempAnn[iLat]);
    T = body[iBody].daTempAnn[iLat] + 273.15;
  } else {
    // MEM: body[iBody].daTempLW[iLat] is not initialized!
    T = body[iBody].daTempLW[iLat] + 273.15;
  }
  dI = -2.794778 + 2 * 2.212108e-2 * T - 3 * 3.361939e-5 * (T * T) -
       3.244753e-3 * phi + 2 * 3.088497e-5 * phi * T -
       3 * 1.679112e-7 * phi * (T * T) + 2.229142e-3 * (phi * phi) -
       2 * 2.789815e-5 * (phi * phi) * T +
       3 * 6.590999e-8 * (phi * phi) * (T * T) +
       9.173169e-3 * (phi * phi * phi) -
       2 * 7.775195e-5 * (phi * phi * phi) * T +
       3 * 1.528125e-7 * (phi * phi * phi) * (T * T) -
       1.631909e-4 * (phi * phi * phi * phi) +
       2 * 3.663871e-6 * (phi * phi * phi * phi) * T -
       3 * 9.255646e-9 * (phi * phi * phi * phi) * (T * T);
  if (fdOLRwk97(body, iBody, iLat, bModel) >= 300.0) {
    dI = 0.001;
  }

  if (T < 190) {
    dI = 4 * SIGMA * T * T * T;
  }
  return dI;
}

/**
Calculates the "top-of-atmosphere" from the Haqq-Misra+ 2016 formulae at
temperatures < 250 K

@param Temp Temperature at grid point
@param pCO2 Atmospheric partial pressure of CO2
@param zenith Zenith angle of host star on sky
@param albsurf Base albedo quantity of surface (ice/water/land)
*/
double AlbedoTOA250(double Temp, double phi, double zenith, double albsurf) {
  double mu, tmpk, dTmp;
  mu   = cos(zenith);
  tmpk = log10(Temp + 273.15);

  dTmp = -3.64301272050786051349e-01 * mu * mu * mu -
         6.66571453035937344644e-01 * mu * mu * albsurf +
         1.38761634791769922215e-01 * mu * mu * tmpk -
         1.40826323888164368220e-02 * mu * mu * phi +
         9.41440608298288128530e-01 * mu * mu +
         7.10961643487220129600e-02 * mu * albsurf * albsurf -
         2.19180456421237290776e-01 * mu * albsurf * tmpk -
         1.82873271476295846949e-02 * mu * albsurf * phi +
         1.48505536251773073708e+00 * mu * albsurf -
         9.01309617860975631487e-01 * mu * tmpk * tmpk +
         1.92113767482554841093e-02 * mu * tmpk * phi +
         4.11334031794617160926e+00 * mu * tmpk +
         6.80906172782627400891e-04 * mu * phi * phi -
         1.66632232847024261413e-02 * mu * phi -
         6.01321219414692986760e+00 * mu +
         5.20833333338503734478e-02 * albsurf * albsurf * albsurf +
         1.09511892935421337181e-01 * albsurf * albsurf * tmpk +
         1.86369741605604787027e-02 * albsurf * albsurf * phi -
         2.54092206932019781807e-01 * albsurf * albsurf -
         4.00290429315177131997e+00 * albsurf * tmpk * tmpk -
         4.60694421170402754195e-02 * albsurf * tmpk * phi +
         1.79103047870275950970e+01 * albsurf * tmpk -
         1.59834667195196747369e-02 * albsurf * phi * phi -
         1.29954198131196525801e-02 * albsurf * phi -
         1.97041106668471570629e+01 * albsurf -
         9.28987827590191805882e+00 * tmpk * tmpk * tmpk +
         2.33079221557892068972e-01 * tmpk * tmpk * phi +
         6.58750181054108310263e+01 * tmpk * tmpk +
         7.46763857253681870296e-03 * tmpk * phi * phi -
         1.00561681124449076030e+00 * tmpk * phi -
         1.55355955538023465579e+02 * tmpk +
         7.11268878229609079374e-04 * phi * phi * phi -
         3.36136500021004319336e-03 * phi * phi +
         1.13977221457453326003e+00 * phi + 1.22439629486842392225e+02;

  return dTmp;
}

/**
Calculates the "top-of-atmosphere" from the Haqq-Misra+ 2016 formulae at 250 K <
temperatures < 350 K

@param Temp Temperature at grid point
@param pCO2 Atmospheric partial pressure of CO2
@param zenith Zenith angle of host star on sky
@param albsurf Base albedo quantity of surface (ice/water/land)
*/
double fdAlbedoTOA350(double Temp, double phi, double zenith, double albsurf) {
  double mu, tmpk, dTmp;
  mu   = cos(zenith);
  tmpk = log10(Temp + 273.15);

  dTmp = -4.41391619954555503025e-01 * mu * mu * mu -
         2.60017516002879089942e-01 * mu * mu * albsurf +
         1.08110772295329837789e+00 * mu * mu * tmpk -
         3.93863285843020910493e-02 * mu * mu * phi -
         1.46383456258096611435e+00 * mu * mu +
         9.91383778608142668398e-02 * mu * albsurf * albsurf -
         1.45914724229303338632e+00 * mu * albsurf * tmpk -
         2.72769392852398387395e-02 * mu * albsurf * phi +
         3.99933641081463919775e+00 * mu * albsurf +
         1.07231336256525633388e+00 * mu * tmpk * tmpk -
         1.04302520934751417891e-02 * mu * tmpk * phi -
         6.10296439299006454604e+00 * mu * tmpk +
         2.69255203910960137434e-03 * mu * phi * phi +
         9.50143253373007257157e-02 * mu * phi +
         7.37864215757422226005e+00 * mu +
         1.28580729156335171748e-01 * albsurf * albsurf * albsurf -
         3.07800300913486257759e-01 * albsurf * albsurf * tmpk +
         2.27715594632176554502e-02 * albsurf * albsurf * phi +
         6.11699085276039222769e-01 * albsurf * albsurf -
         2.33213409642421742873e+00 * albsurf * tmpk * tmpk +
         2.56011431303802661219e-01 * albsurf * tmpk * phi +
         1.05912148222549546972e+01 * albsurf * tmpk -
         1.85772688884413561539e-02 * albsurf * phi * phi -
         7.55796861024326749323e-01 * albsurf * phi -
         1.16485004141808623501e+01 * albsurf +
         2.74062491988752192640e+01 * tmpk * tmpk * tmpk +
         5.46044240911252587445e-01 * tmpk * tmpk * phi -
         2.05761674358916081928e+02 * tmpk * tmpk +
         5.57943359123403426203e-02 * tmpk * phi * phi -
         2.49880329758542751861e+00 * tmpk * phi +
         5.14448995054491206247e+02 * tmpk +
         2.43702089287719950508e-03 * phi * phi * phi -
         1.09384840764980617589e-01 * phi * phi +
         2.92643187434628071486e+00 * phi - 4.27802454850920923946e+02;

  return dTmp;
}

/**
Calculates the "top-of-atmosphere" from the Williams & Kasting 1997 formulae
at temperatures < 280 K

@param Temp Temperature at grid point
@param pCO2 Atmospheric partial pressure of CO2
@param zenith Zenith angle of host star on sky
@param albsurf Base albedo quantity of surface (ice/water/land)
*/
double AlbedoTOA280(double Temp, double pCO2, double zenith, double albsurf) {
  double mu, T, dTmp;
  mu = cos(zenith);
  T  = Temp + 273.15;

  dTmp = -6.891e-1 + 1.046 * albsurf + 7.8054e-3 * T - 2.8373e-3 * pCO2 -
         2.8899e-1 * mu - 3.7412e-2 * albsurf * pCO2 - 6.3499e-3 * mu * pCO2 +
         2.0122e-1 * albsurf * mu - 1.8508e-3 * albsurf * T +
         1.3649e-4 * mu * T + 9.8581e-5 * pCO2 * T +
         7.3239e-2 * albsurf * albsurf - 1.6555e-5 * (T * T) +
         6.5817e-4 * (pCO2 * pCO2) + 8.1218e-2 * mu * mu;

  return dTmp;
}

/**
Calculates the "top-of-atmosphere" from the Williams & Kasting 1997 formulae
 at 280 K < temperatures < 370 K

@param Temp Temperature at grid point
@param pCO2 Atmospheric partial pressure of CO2
@param zenith Zenith angle of host star on sky
@param albsurf Base albedo quantity of surface (ice/water/land)
*/
double AlbedoTOA370(double Temp, double pCO2, double zenith, double albsurf) {
  double mu, T, dTmp;
  mu = cos(zenith);
  T  = Temp + 273.15;

  dTmp = 1.1082 + 1.5172 * albsurf - 5.7993e-3 * T + 1.9705e-2 * pCO2 -
         1.867e-1 * mu - 3.1355e-2 * albsurf * pCO2 - 1.0214e-2 * mu * pCO2 +
         2.0986e-1 * albsurf * mu - 3.7098e-3 * albsurf * T -
         1.1335e-4 * mu * T + 5.3714e-5 * pCO2 * T +
         7.5887e-2 * albsurf * albsurf + 9.269e-6 * (T * T) -
         4.1327e-4 * (pCO2 * pCO2) + 6.3298e-2 * mu * mu;

  return dTmp;
}

/**
Calculates albedo based on zenith angle (tuned to Earth)

@param zenith Zenith angle of each latitude in radians
@return Albedo
*/
double AlbedoTaylor(double zenith) {
  double mu = cos(zenith);

  if (mu > 0) {
    return 0.037 / (1.1 * pow(mu, 1.4) + 0.15);
  } else {
    return 0.037 / 0.15;
  }
}

/**
Calculates the planetary albedo based on the formulae from Haqq-Misra+ 2016,
accounting for temperature and surface type.

@param body Struct containing all body information
@param zenith Zenith angle of latitude iLat
@param iBody Body in question
@param iLat Latitude cell in question
*/
void AlbedoTOAhm16(BODY *body, double zenith, int iBody, int iLat) {

  double phi, albtmp;
  phi = log10(body[iBody].dpCO2);

  if (body[iBody].daIceMassTmp[iLat] > 0 ||
      body[iBody].daTempLand[iLat] <= -10) {

    albtmp = body[iBody].dIceAlbedo;

  } else {

    albtmp = body[iBody].dAlbedoLand;
  }

  if (body[iBody].daTempLand[iLat] <= (-23.15)) {

    body[iBody].daAlbedoLand[iLat] =
          AlbedoTOA250(body[iBody].daTempLand[iLat], phi, zenith, albtmp);

  } else if (body[iBody].daTempLand[iLat] <= 76.85) {

    body[iBody].daAlbedoLand[iLat] =
          fdAlbedoTOA350(body[iBody].daTempLand[iLat], phi, zenith, albtmp);

  } else {

    body[iBody].daAlbedoLand[iLat] = 0.18;
  }

  if (body[iBody].daTempWater[iLat] <= body[iBody].dFrzTSeaIce) {
    albtmp = body[iBody].dIceAlbedo;
  } else {
    albtmp = AlbedoTaylor(zenith);
  }

  if (body[iBody].daTempWater[iLat] <= (-23.15)) {

    body[iBody].daAlbedoWater[iLat] =
          AlbedoTOA250(body[iBody].daTempWater[iLat], phi, zenith, albtmp);
  } else if (body[iBody].daTempWater[iLat] <= 76.85) {

    body[iBody].daAlbedoWater[iLat] =
          fdAlbedoTOA350(body[iBody].daTempWater[iLat], phi, zenith, albtmp);
  } else {
    // albedo asymptotes to ~0.18 (all surface albedos?)
    body[iBody].daAlbedoWater[iLat] = 0.18;
  }
}


/**
Calculates the planetary albedo based on the formulae from Williams & Kasting
1997, accounting for temperature and surface type.

@param body Struct containing all body information
@param zenith Zenith angle of latitude iLat
@param iBody Body in question
@param iLat Latitude cell in question
*/
void AlbedoTOAwk97(BODY *body, double zenith, int iBody, int iLat) {
  double phi, albtmp;
  phi = body[iBody].dpCO2;

  if (body[iBody].daTempLand[iLat] <= -10) {

    albtmp = body[iBody].dIceAlbedo;

  } else if (body[iBody].daTempLand[iLat] > -10 &&
             body[iBody].daIceMassTmp[iLat] > 0) {

    albtmp = (body[iBody].dIceAlbedo + body[iBody].dAlbedoLand) / 2.0;

  } else if (body[iBody].daTempLand[iLat] <= 0 &&
             body[iBody].daIceMassTmp[iLat] == 0) {

    albtmp = (body[iBody].dIceAlbedo + body[iBody].dAlbedoLand) / 2.0;

  } else {

    albtmp = body[iBody].dAlbedoLand;
  }

  if (body[iBody].daTempLand[iLat] >= -83.15 &&
      body[iBody].daTempLand[iLat] <= (6.85)) {

    body[iBody].daAlbedoLand[iLat] =
          AlbedoTOA280(body[iBody].daTempLand[iLat], phi, zenith, albtmp);

  } else if (body[iBody].daTempLand[iLat] <= 96.85) {

    body[iBody].daAlbedoLand[iLat] =
          AlbedoTOA370(body[iBody].daTempLand[iLat], phi, zenith, albtmp);

  } else if (body[iBody].daTempLand[iLat] < -83.15) {

    body[iBody].daAlbedoLand[iLat] = body[iBody].dIceAlbedo;
  } else {
    // albedo asymptotes to ~0.18 (all surface albedos?)
    body[iBody].daAlbedoLand[iLat] = 0.18;
  }

  if (body[iBody].daTempWater[iLat] <= body[iBody].dFrzTSeaIce) {

    albtmp = body[iBody].dIceAlbedo;

  } else {

    albtmp = AlbedoTaylor(zenith);
  }

  if (body[iBody].daTempWater[iLat] >= -83.15 &&
      body[iBody].daTempWater[iLat] <= (6.85)) {

    body[iBody].daAlbedoWater[iLat] =
          AlbedoTOA280(body[iBody].daTempWater[iLat], phi, zenith, albtmp);

  } else if (body[iBody].daTempWater[iLat] <= 96.85) {

    body[iBody].daAlbedoWater[iLat] =
          AlbedoTOA370(body[iBody].daTempWater[iLat], phi, zenith, albtmp);

  } else if (body[iBody].daTempWater[iLat] < -83.15) {

    body[iBody].daAlbedoWater[iLat] = body[iBody].dIceAlbedo;
  } else {
    // albedo asymptotes to ~0.18 (all surface albedos?)
    body[iBody].daAlbedoWater[iLat] = 0.18;
  }
}

/**
Calculates the planetary albedo based on the formulae from Spiegel+ 2009,
 accounting for temperature and surface type.

@param body Struct containing all body information
@param zenith Zenith angle of latitude iLat
@param iBody Body in question
@param iLat Latitude cell in question
*/
void AlbedoTOAsms09(BODY *body, double zenith, int iBody, int iLat) {
  body[iBody].daAlbedoLand[iLat] =
        0.5 * (body[iBody].dIceAlbedo + body[iBody].dAlbedoLand) -
        0.5 * (body[iBody].dIceAlbedo - body[iBody].dAlbedoLand) *
              tanh((body[iBody].daTempLand[iLat] + 5.15) / 5.15) +
        0.04 * (3 * sin(zenith) * sin(zenith) - 1.);

  body[iBody].daAlbedoWater[iLat] =
        0.5 * (body[iBody].dIceAlbedo + body[iBody].dAlbedoWater) -
        0.5 * (body[iBody].dIceAlbedo - body[iBody].dAlbedoWater) *
              tanh((body[iBody].daTempWater[iLat] + 5.15) / 5.15) +
        0.04 * (3. * sin(zenith) * sin(zenith) - 1.);

  if (body[iBody].daIceMassTmp[iLat] > 0 &&
      body[iBody].daAlbedoLand[iLat] < body[iBody].dIceAlbedo) {

    body[iBody].daAlbedoLand[iLat] = body[iBody].dIceAlbedo;
  }
}

/**
Calculates the planetary albedo for the seasonal EBM at iNday iDay

@param body Struct containing all body information
@param iBody Body in question
@param iDay Day of the planet's year
*/
void fvAlbedoSeasonal(BODY *body, int iBody, int iDay) {
  int iLat;
  double dZenith;

  body[iBody].dAlbedoGlobalTmp = 0;
  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    // zenith angle of sun at noon at each latitude
    dZenith = fabs(body[iBody].daLats[iLat] - body[iBody].daDeclination[iDay]);

    if (body[iBody].bCalcAB == 1) {
      if (body[iBody].iOLRModel == WK97) {
        AlbedoTOAwk97(body, dZenith, iBody, iLat);
      } else if (body[iBody].iOLRModel == HM16) {
        AlbedoTOAhm16(body, dZenith, iBody, iLat);
        //         AlbedoTOAwk97(body, dZenith, iBody, iLat);
      } else {
        AlbedoTOAsms09(body, dZenith, iBody, iLat);
      }
    } else {
      body[iBody].daAlbedoLand[iLat] =
            body[iBody].dAlbedoLand +
            0.08 * (3. * (sin(dZenith) * sin(dZenith)) - 1.) / 2.;

      body[iBody].daAlbedoWater[iLat] =
            body[iBody].dAlbedoWater +
            0.08 * (3. * (sin(dZenith) * sin(dZenith)) - 1.) / 2.;

      if (body[iBody].daIceMassTmp[iLat] > 0 ||
          body[iBody].daTempLand[iLat] <= -2) {

        body[iBody].daAlbedoLand[iLat] = body[iBody].dIceAlbedo;
      }
      if (body[iBody].daTempWater[iLat] <= body[iBody].dFrzTSeaIce) {
        body[iBody].daAlbedoWater[iLat] = body[iBody].dIceAlbedo;
      }
    }
    body[iBody].daAlbedoLW[iLat] =
          body[iBody].daLandFrac[iLat] * body[iBody].daAlbedoLand[iLat] +
          body[iBody].daWaterFrac[iLat] * body[iBody].daAlbedoWater[iLat];

    body[iBody].dAlbedoGlobalTmp +=
          body[iBody].daAlbedoLW[iLat] / body[iBody].iNumLats;
  }
  body[iBody].dAlbedoGlobal +=
        body[iBody].dAlbedoGlobalTmp / (body[iBody].iNStepInYear);
}

/**
Modifies the ocean component to account for effects of sea ice (seasonal EBM
only). This slows the EBM considerably so use with caution.

@param body Struc containing all body information
@param iBody Body in question
*/
void fvSeaIce(BODY *body, int iBody) {
  int iLat, jLat;
  double dNhicearea, dShicearea, dNhtotarea, dShtotarea;
  double dNhfw, dShfw, dNhdW, dShdW, dCw_dt;
  dNhicearea = 0;
  dShicearea = 0;
  dNhtotarea = 0;
  dShtotarea = 0;

  dCw_dt = (body[iBody].dHeatCapWater * body[iBody].dMeanMotion / (2 * PI)) /
           body[iBody].dSeasDeltat;

  for (iLat = 0; iLat < 2 * body[iBody].iNumLats; iLat++) {
    for (jLat = 0; jLat < 2 * body[iBody].iNumLats; jLat++) {
      body[iBody].daMEulerSea[iLat][jLat] = body[iBody].daMInit[iLat][jLat];
    }
  }

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (body[iBody].daTempWater[iLat] <= body[iBody].dFrzTSeaIce) {
      if (body[iBody].daSeaIceHeight[iLat] > 0) {
        body[iBody].daSeaIceK[iLat] =
              body[iBody].dSeaIceConduct / body[iBody].daSeaIceHeight[iLat];

        body[iBody].daSourceLW[2 * iLat + 1] =
              body[iBody].daSeaIceK[iLat] * body[iBody].dFrzTSeaIce -
              body[iBody].daSourceW[iLat];

        body[iBody].daMEulerSea[2 * iLat + 1][2 * iLat + 1] +=
              -dCw_dt + body[iBody].daSeaIceK[iLat];

      } else {
        body[iBody].daSeaIceK[iLat] = 0.0;
      }
    }
  }
  fvMatrixInvertSeasonal(body, iBody);

  for (iLat = 0; iLat < 2 * body[iBody].iNumLats; iLat++) {
    body[iBody].daTmpTempSea[iLat] = 0.0;
    for (jLat = 0; jLat < 2 * body[iBody].iNumLats; jLat++) {
      body[iBody].daTmpTempSea[iLat] +=
            body[iBody].daInvMSea[iLat][jLat] * body[iBody].daSourceLW[jLat];
    }
  }

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    // temp change this time step
    body[iBody].daDeltaTempL[iLat] =
          body[iBody].daTmpTempSea[2 * iLat] - body[iBody].daTempLand[iLat];

    body[iBody].daDeltaTempW[iLat] = body[iBody].daTmpTempSea[2 * iLat + 1] -
                                     body[iBody].daTempWater[iLat];

    body[iBody].daTempLand[iLat]  = body[iBody].daTmpTempSea[2 * iLat];
    body[iBody].daTempWater[iLat] = body[iBody].daTmpTempSea[2 * iLat + 1];

    /* if sea ice present but T > -2, set to -2 */
    if (body[iBody].daSeaIceHeight[iLat] > 0) {
      if (body[iBody].daTempWater[iLat] > -2) {
        body[iBody].daTempWater[iLat] = -2.0;
      }

      body[iBody].daFluxSeaIce[iLat] =
            -body[iBody].daSourceW[iLat] -
            body[iBody].daPlanckBSea[iLat] * body[iBody].daTempWater[iLat] -
            body[iBody].dNuLandWater / body[iBody].daWaterFrac[iLat] *
                  (body[iBody].daTmpTempSea[2 * iLat + 1] -
                   body[iBody].daTempLand[iLat]);

      for (jLat = 0; jLat < body[iBody].iNumLats; jLat++) {
        body[iBody].daFluxSeaIce[iLat] +=
              body[iBody].daMDiffSea[iLat][jLat] *
              body[iBody].daTmpTempSea[2 * jLat + 1];
      }
      if (body[iBody].daLats[iLat] >= 0) {
        dNhicearea += body[iBody].daWaterFrac[iLat];
        dNhtotarea += body[iBody].daWaterFrac[iLat];
      } else {
        dShicearea += body[iBody].daWaterFrac[iLat];
        dShtotarea += body[iBody].daWaterFrac[iLat];
      }
    } else {
      body[iBody].daFluxSeaIce[iLat] = 0.0;
      /* no sea ice, still add to total area */
      if (body[iBody].daLats[iLat] >= 0) {
        dNhtotarea += body[iBody].daWaterFrac[iLat];
      } else {
        dShtotarea += body[iBody].daWaterFrac[iLat];
      }
    }
  }
  dNhfw =
        2.0 * (2.0 - 2.0 * (dNhicearea - body[iBody].dSeasDeltax) / dNhtotarea);
  dShfw =
        2.0 * (2.0 - 2.0 * (dShicearea - body[iBody].dSeasDeltax) / dShtotarea);
  if (dNhfw > 4) {
    dNhfw = 4.0;
  }
  if (dShfw > 4) {
    dShfw = 4.0;
  }
  dNhdW = dNhfw * dNhicearea / (dNhtotarea - dNhicearea) / dCw_dt;
  dShdW = dShfw * dShicearea / (dShtotarea - dShicearea) / dCw_dt;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (body[iBody].daSeaIceHeight[iLat] > 0) {
      if (body[iBody].daLats[iLat] >= 0) {
        body[iBody].daFluxSeaIce[iLat] += dNhfw;
      } else {
        body[iBody].daFluxSeaIce[iLat] += dShfw;
      }
    } else {
      if (body[iBody].daLats[iLat] >= 0) {
        body[iBody].daTempWater[iLat] -= dNhdW;
      } else {
        body[iBody].daTempWater[iLat] -= dShdW;
      }
    }
  }
}

/**
Calculates the backwards-Euler matrix that integrates the seasonal EBM. Each
operation of the inverse matrix is a "time-step".

@param body Struct containing all body information and variables
@param iBody Body in question
*/
void fvMatrixSeasonal(BODY *body, int iBody) {
  int iLat, jLat;
  double dXBoundary, dNu_fl, dNu_fw, dCl_dt, dCw_dt;

  dCl_dt = body[iBody].dHeatCapLand * body[iBody].dMeanMotion / (2 * PI) /
           body[iBody].dSeasDeltat;

  dCw_dt = body[iBody].dHeatCapWater * body[iBody].dMeanMotion / (2 * PI) /
           body[iBody].dSeasDeltat;

  for (iLat = 0; iLat < body[iBody].iNumLats + 1; iLat++) {
    body[iBody].daLambdaSea[iLat] =
          body[iBody].daDiffusionSea[iLat] *
          (1.0 -
           (body[iBody].daXBoundary[iLat] * body[iBody].daXBoundary[iLat])) /
          ((body[iBody].dSeasDeltax * body[iBody].dSeasDeltax));
  }

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    dNu_fl = body[iBody].dNuLandWater / body[iBody].daLandFrac[iLat];
    dNu_fw = body[iBody].dNuLandWater / body[iBody].daWaterFrac[iLat];

    for (jLat = 0; jLat < body[iBody].iNumLats; jLat++) {

      if (jLat == iLat) {
        body[iBody].daMDiffSea[iLat][jLat] =
              (-body[iBody].daLambdaSea[iLat + 1] -
               body[iBody].daLambdaSea[iLat]);

        if (body[iBody].bAccuracyMode) {
          body[iBody].daMLand[iLat][jLat] =
                dCl_dt + body[iBody].daPlanckBSea[iLat] + dNu_fl -
                body[iBody].daMDiffSea[iLat][jLat];

          body[iBody].daMWater[iLat][jLat] =
                dCw_dt + body[iBody].daPlanckBSea[iLat] + dNu_fw -
                body[iBody].daMDiffSea[iLat][jLat];

        } else {
          body[iBody].daMLand[iLat][jLat] =
                dCl_dt + body[iBody].daPlanckBAvg[iLat] + dNu_fl -
                body[iBody].daMDiffSea[iLat][jLat];

          body[iBody].daMWater[iLat][jLat] =
                dCw_dt + body[iBody].daPlanckBAvg[iLat] + dNu_fw -
                body[iBody].daMDiffSea[iLat][jLat];
        }

      } else if (jLat == (iLat + 1)) {
        body[iBody].daMDiffSea[iLat][jLat] = body[iBody].daLambdaSea[jLat];
        body[iBody].daMLand[iLat][jLat]    = -body[iBody].daLambdaSea[jLat];
        body[iBody].daMWater[iLat][jLat]   = -body[iBody].daLambdaSea[jLat];
      } else if (jLat == (iLat - 1)) {
        body[iBody].daMDiffSea[iLat][jLat] = body[iBody].daLambdaSea[iLat];
        body[iBody].daMLand[iLat][jLat]    = -body[iBody].daLambdaSea[iLat];
        body[iBody].daMWater[iLat][jLat]   = -body[iBody].daLambdaSea[iLat];
      } else {
        body[iBody].daMDiffSea[iLat][jLat] = 0.0;
        body[iBody].daMLand[iLat][jLat]    = 0.0;
        body[iBody].daMWater[iLat][jLat]   = 0.0;
      }
    }
    body[iBody].daMInit[2 * iLat][2 * iLat + 1] = -1 * dNu_fl;
    body[iBody].daMInit[2 * iLat + 1][2 * iLat] = -1 * dNu_fw;

    for (jLat = 0; jLat < body[iBody].iNumLats; jLat++) {
      body[iBody].daMInit[2 * iLat][2 * jLat] = body[iBody].daMLand[iLat][jLat];
      body[iBody].daMInit[2 * iLat + 1][2 * jLat + 1] =
            body[iBody].daMWater[iLat][jLat];
    }
  }

  for (iLat = 0; iLat < 2 * body[iBody].iNumLats; iLat++) {
    for (jLat = 0; jLat < 2 * body[iBody].iNumLats; jLat++) {
      body[iBody].daMEulerSea[iLat][jLat] = body[iBody].daMInit[iLat][jLat];
      body[iBody].daMEulerCopySea[iLat][jLat] =
            body[iBody].daMEulerCopySea[iLat][jLat];
    }
  }

  fvMatrixInvertSeasonal(body, iBody);
}

/**
Calculates non-matrix terms in the energy balance equation (I don't remember
why I called these "SourceF"). These terms are the OLR offset and the
absorbed radiation (temperature independent quantities).

@param body Struct containing all body information
@param iBody Body in question
@param iNday Day of the planet's year
*/
void fvSourceFSeas(BODY *body, int iBody, int iNday) {
  int iLat;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    body[iBody].daSourceL[iLat] = body[iBody].daPlanckASea[iLat] -
                                  (1.0 - body[iBody].daAlbedoLand[iLat]) *
                                        body[iBody].daInsol[iLat][iNday];

    body[iBody].daSourceW[iLat] = body[iBody].daPlanckASea[iLat] -
                                  (1.0 - body[iBody].daAlbedoWater[iLat]) *
                                        body[iBody].daInsol[iLat][iNday];

    body[iBody].daSourceLW[2 * iLat] =
          body[iBody].daTempLand[iLat] * body[iBody].dHeatCapLand *
                body[iBody].dMeanMotion / (2 * PI) / body[iBody].dSeasDeltat -
          body[iBody].daSourceL[iLat];

    body[iBody].daSourceLW[2 * iLat + 1] =
          body[iBody].daTempWater[iLat] * body[iBody].dHeatCapWater *
                body[iBody].dMeanMotion / (2 * PI) / body[iBody].dSeasDeltat -
          body[iBody].daSourceW[iLat];
  }
}

/**
Calculates average energy residual or "error" over course of the year. This
should be used as a check on the physics in the EBM (should be ~0).

@param body Struct containing all body information
@param iBody Body in question
@param iNday Day of the planet's year
*/
void EnergyResiduals(BODY *body, int iBody, int iNday) {
  int iLat;
  double dNu_fl, dNu_fw, dCl_dt, dCw_dt;

  dCl_dt = body[iBody].dHeatCapLand * body[iBody].dMeanMotion / (2 * PI) /
           body[iBody].dSeasDeltat;

  dCw_dt = body[iBody].dHeatCapWater * body[iBody].dMeanMotion / (2 * PI) /
           body[iBody].dSeasDeltat;

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    dNu_fl = body[iBody].dNuLandWater / body[iBody].daLandFrac[iLat];
    dNu_fw = body[iBody].dNuLandWater / body[iBody].daWaterFrac[iLat];

    if (iLat == 0) {
      body[iBody].daEnergyResL[iLat] =
            body[iBody].daInsol[iLat][iNday] *
                  (1.0 - body[iBody].daAlbedoLand[iLat]) -
            dCl_dt * (body[iBody].daDeltaTempL[iLat]) -
            body[iBody].daLambdaSea[iLat + 1] *
                  (body[iBody].daTempLand[iLat] -
                   body[iBody].daTempLand[iLat + 1]) -
            dNu_fl * (body[iBody].daTempLand[iLat] -
                      body[iBody].daTempWater[iLat]) -
            body[iBody].daPlanckASea[iLat] -
            body[iBody].daPlanckBSea[iLat] * body[iBody].daTempLand[iLat];

      body[iBody].daEnergyResW[iLat] =
            body[iBody].daInsol[iLat][iNday] *
                  (1.0 - body[iBody].daAlbedoWater[iLat]) -
            dCw_dt * (body[iBody].daDeltaTempW[iLat]) -
            body[iBody].daLambdaSea[iLat + 1] *
                  (body[iBody].daTempWater[iLat] -
                   body[iBody].daTempWater[iLat + 1]) -
            dNu_fw * (body[iBody].daTempWater[iLat] -
                      body[iBody].daTempLand[iLat]) -
            body[iBody].daPlanckASea[iLat] -
            body[iBody].daPlanckBSea[iLat] * body[iBody].daTempWater[iLat];

    } else if (iLat == (body[iBody].iNumLats - 1)) {
      body[iBody].daEnergyResL[iLat] =
            body[iBody].daInsol[iLat][iNday] *
                  (1.0 - body[iBody].daAlbedoLand[iLat]) -
            dCl_dt * (body[iBody].daDeltaTempL[iLat]) -
            body[iBody].daLambdaSea[iLat] * (body[iBody].daTempLand[iLat] -
                                             body[iBody].daTempLand[iLat - 1]) -
            dNu_fl * (body[iBody].daTempLand[iLat] -
                      body[iBody].daTempWater[iLat]) -
            body[iBody].daPlanckASea[iLat] -
            body[iBody].daPlanckBSea[iLat] * body[iBody].daTempLand[iLat];

      body[iBody].daEnergyResW[iLat] =
            body[iBody].daInsol[iLat][iNday] *
                  (1.0 - body[iBody].daAlbedoWater[iLat]) -
            dCw_dt * (body[iBody].daDeltaTempW[iLat]) -
            body[iBody].daLambdaSea[iLat] *
                  (body[iBody].daTempWater[iLat] -
                   body[iBody].daTempWater[iLat - 1]) -
            dNu_fw * (body[iBody].daTempWater[iLat] -
                      body[iBody].daTempLand[iLat]) -
            body[iBody].daPlanckASea[iLat] -
            body[iBody].daPlanckBSea[iLat] * body[iBody].daTempWater[iLat];

    } else {
      body[iBody].daEnergyResL[iLat] =
            body[iBody].daInsol[iLat][iNday] *
                  (1.0 - body[iBody].daAlbedoLand[iLat]) -
            dCl_dt * (body[iBody].daDeltaTempL[iLat]) -
            body[iBody].daLambdaSea[iLat] * (body[iBody].daTempLand[iLat] -
                                             body[iBody].daTempLand[iLat - 1]) -
            body[iBody].daLambdaSea[iLat + 1] *
                  (body[iBody].daTempLand[iLat] -
                   body[iBody].daTempLand[iLat + 1]) -
            dNu_fl * (body[iBody].daTempLand[iLat] -
                      body[iBody].daTempWater[iLat]) -
            body[iBody].daPlanckASea[iLat] -
            body[iBody].daPlanckBSea[iLat] * body[iBody].daTempLand[iLat];

      body[iBody].daEnergyResW[iLat] =
            body[iBody].daInsol[iLat][iNday] *
                  (1.0 - body[iBody].daAlbedoWater[iLat]) -
            dCw_dt * (body[iBody].daDeltaTempW[iLat]) -
            body[iBody].daLambdaSea[iLat] *
                  (body[iBody].daTempWater[iLat] -
                   body[iBody].daTempWater[iLat - 1]) -
            body[iBody].daLambdaSea[iLat + 1] *
                  (body[iBody].daTempWater[iLat] -
                   body[iBody].daTempWater[iLat + 1]) -
            dNu_fw * (body[iBody].daTempWater[iLat] -
                      body[iBody].daTempLand[iLat]) -
            body[iBody].daPlanckASea[iLat] -
            body[iBody].daPlanckBSea[iLat] * body[iBody].daTempWater[iLat];
    }

    body[iBody].daEnerResLAnn[iLat] +=
          body[iBody].daEnergyResL[iLat] / body[iBody].iNStepInYear;

    body[iBody].daEnerResWAnn[iLat] +=
          body[iBody].daEnergyResW[iLat] / body[iBody].iNStepInYear;
  }
}

void fvPoiseSeasonalInitialize(BODY *body, int iBody, int iYear) {
  body[iBody].dTGlobal       = 0.0;
  body[iBody].dFluxInGlobal  = 0.0;
  body[iBody].dFluxOutGlobal = 0.0;

  for (int iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    // start of year, reset annual averages to zero
    body[iBody].daTempAvg[iLat]          = 0.0;
    body[iBody].daPlanckBAvg[iLat]       = 0.0;
    body[iBody].daAlbedoAvg[iLat]        = 0.0;
    body[iBody].daTempAvgL[iLat]         = 0.0;
    body[iBody].daTempAvgW[iLat]         = 0.0;
    body[iBody].daAlbedoAvgL[iLat]       = 0.0;
    body[iBody].daAlbedoAvgW[iLat]       = 0.0;
    body[iBody].daFluxAvg[iLat]          = 0.0;
    body[iBody].daFluxInAvg[iLat]        = 0.0;
    body[iBody].daDivFluxAvg[iLat]       = 0.0;
    body[iBody].daFluxOutAvg[iLat]       = 0.0;
    body[iBody].daIceBalanceAnnual[iLat] = 0.0;
    body[iBody].daEnerResLAnn[iLat]      = 0.0;
    body[iBody].daEnerResWAnn[iLat]      = 0.0;
    body[iBody].daTempMinLW[iLat]        = dHUGE;
    body[iBody].daTempMaxLW[iLat]        = -1 * dHUGE;
    body[iBody].daTempMaxLand[iLat]      = -1 * dHUGE;
    body[iBody].daTempMaxWater[iLat]     = -1 * dHUGE;

    if (iYear == 0) {
      // reset ice sheet stuff only on first year
      if (body[iBody].bIceSheets) {
        body[iBody].daIceMassTmp[iLat]   = body[iBody].daIceMass[iLat];
        body[iBody].daIceAccumTot[iLat]  = 0;
        body[iBody].daIceAblateTot[iLat] = 0;
      }
    }
  }
}

void fvCalcPlanckAB(BODY *body, int iBody, int iLat) {
  body[iBody].daTempLW[iLat] =
        body[iBody].daLandFrac[iLat] * body[iBody].daTempLand[iLat] +
        body[iBody].daWaterFrac[iLat] * body[iBody].daTempWater[iLat];
  body[iBody].dTGlobalTmp += body[iBody].daTempLW[iLat] / body[iBody].iNumLats;

  if (body[iBody].bCalcAB) {
    if (body[iBody].iOLRModel == WK97) {
      /* Calculate A and B from williams and kasting 97 result */
      body[iBody].daPlanckBSea[iLat] = fdOLRdTwk97(body, iBody, iLat, SEA);
      body[iBody].daPlanckASea[iLat] =
            fdOLRwk97(body, iBody, iLat, SEA) -
            body[iBody].daPlanckBSea[iLat] * (body[iBody].daTempLW[iLat]);
    } else if (body[iBody].iOLRModel == HM16) {
      /* Calculate A and B from haqq-misra+ 2016 result */
      body[iBody].daPlanckBSea[iLat] = fdOLRdThm16(body, iBody, iLat, SEA);
      body[iBody].daPlanckASea[iLat] =
            fdOLRhm16(body, iBody, iLat, SEA) -
            body[iBody].daPlanckBSea[iLat] * (body[iBody].daTempLW[iLat]);
    } else {
      /* Calculate A and B from spiegel+ 2009 model */
      body[iBody].daPlanckBSea[iLat] = fdOLRdTsms09(body, iBody, iLat, SEA);
      body[iBody].daPlanckASea[iLat] =
            fdOLRsms09(body, iBody, iLat, SEA) -
            body[iBody].daPlanckBSea[iLat] * (body[iBody].daTempLW[iLat]);
    }

    // XXX This section doesn't seem releated to calculating A and B,
    // but was part of this if-else originally

    if (body[iBody].bMEPDiff) {
      if (iLat == 0) {
        body[iBody].daDiffusionSea[iLat] = body[iBody].daPlanckBSea[iLat] / 4.0;

      } else if (iLat == (body[iBody].iNumLats - 1)) {
        body[iBody].daDiffusionSea[iLat] =
              (body[iBody].daPlanckBSea[iLat] +
               body[iBody].daPlanckBSea[iLat - 1]) /
              8.0;

        body[iBody].daDiffusionSea[iLat + 1] =
              body[iBody].daPlanckBSea[iLat] / 4.0;

      } else {
        body[iBody].daDiffusionSea[iLat] =
              (body[iBody].daPlanckBSea[iLat] +
               body[iBody].daPlanckBSea[iLat - 1]) /
              8.0;
      }
    }
  }
}

void fvFluxesByLatitude(BODY *body, int iBody, int iNyear, int iNstep) {
  for (int iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    body[iBody].daDMidPt[iLat] = 0.5 * (body[iBody].daDiffusionSea[iLat + 1] +
                                        body[iBody].daDiffusionSea[iLat]);
    body[iBody].daFlux[iLat] =
          -2. * PI * (body[iBody].dRadius * body[iBody].dRadius) *
          sqrt(1.0 - (sin(body[iBody].daLats[iLat]) *
                      sin(body[iBody].daLats[iLat]))) *
          body[iBody].daDMidPt[iLat] * body[iBody].daTGrad[iLat];
    body[iBody].daFluxAvg[iLat] +=
          body[iBody].daFlux[iLat] / body[iBody].iNStepInYear;
    body[iBody].daFluxDaily[iLat][iNyear * body[iBody].iNStepInYear + iNstep] =
          body[iBody].daFlux[iLat];

    body[iBody].daDivFlux[iLat] = 0.0;
    for (int jLat = 0; jLat < body[iBody].iNumLats; jLat++) {
      body[iBody].daDivFlux[iLat] +=
            -body[iBody].daMDiffSea[iLat][jLat] * body[iBody].daTempLW[jLat];
    }
    body[iBody].daDivFluxAvg[iLat] +=
          body[iBody].daDivFlux[iLat] / body[iBody].iNStepInYear;
    body[iBody]
          .daDivFluxDaily[iLat][iNyear * body[iBody].iNStepInYear + iNstep] =
          body[iBody].daDivFlux[iLat];
  }
}

// annual averages by latitude
void fvPoiseAnnualAveragesByLatitude(BODY *body, int iBody, int iLat) {
  body[iBody].daTempAvg[iLat] +=
        body[iBody].daTempLW[iLat] / body[iBody].iNStepInYear;
  body[iBody].daPlanckBAvg[iLat] +=
        body[iBody].daPlanckBSea[iLat] / body[iBody].iNStepInYear;
  body[iBody].daTempAvgL[iLat] +=
        body[iBody].daTempLand[iLat] / body[iBody].iNStepInYear;
  body[iBody].daTempAvgW[iLat] +=
        body[iBody].daTempWater[iLat] / body[iBody].iNStepInYear;
  body[iBody].daAlbedoAvgL[iLat] +=
        body[iBody].daAlbedoLand[iLat] / body[iBody].iNStepInYear;
  body[iBody].daAlbedoAvgW[iLat] +=
        body[iBody].daAlbedoWater[iLat] / body[iBody].iNStepInYear;
  body[iBody].daAlbedoAvg[iLat] +=
        body[iBody].daAlbedoLW[iLat] / body[iBody].iNStepInYear;
  body[iBody].daFluxInAvg[iLat] +=
        body[iBody].daFluxIn[iLat] / body[iBody].iNStepInYear;
  body[iBody].daFluxOutAvg[iLat] +=
        body[iBody].daFluxOut[iLat] / body[iBody].iNStepInYear;
}

void fvPoiseDailyProps(BODY *body, int iBody, int iLat, int iNyear,
                       int iNstep) {
  body[iBody].daTempDaily[iLat][iNyear * body[iBody].iNStepInYear + iNstep] =
        body[iBody].daTempLW[iLat];
  body[iBody].daFluxInDaily[iLat][iNyear * body[iBody].iNStepInYear + iNstep] =
        body[iBody].daFluxIn[iLat];
  body[iBody].daFluxOutDaily[iLat][iNyear * body[iBody].iNStepInYear + iNstep] =
        body[iBody].daFluxOut[iLat];
  body[iBody].daPlanckBDaily[iLat][iNyear * body[iBody].iNStepInYear + iNstep] =
        body[iBody].daPlanckBSea[iLat];
}

void fvPoiseSeasonalMinMax(BODY *body, int iBody, int iLat) {
  if (body[iBody].daTempLW[iLat] < body[iBody].daTempMinLW[iLat]) {
    body[iBody].daTempMinLW[iLat] = body[iBody].daTempLW[iLat];
  }
  if (body[iBody].daTempLW[iLat] > body[iBody].daTempMaxLW[iLat]) {
    body[iBody].daTempMaxLW[iLat] = body[iBody].daTempLW[iLat];
  }
  if (body[iBody].daTempLand[iLat] > body[iBody].daTempMaxLand[iLat]) {
    body[iBody].daTempMaxLand[iLat] = body[iBody].daTempLand[iLat];
  }
  if (body[iBody].daTempWater[iLat] > body[iBody].daTempMaxWater[iLat]) {
    body[iBody].daTempMaxWater[iLat] = body[iBody].daTempWater[iLat];
  }
}

void fvCalculateIceSheets(BODY *body, double dStepsize, int iBody, int iLat,
                          int iNstep) {
  // calculate derivative of ice mass density and take an euler step
  body[iBody].daIceBalance[iLat][iNstep] = fdIceMassBalance(body, iBody, iLat);

  body[iBody].daIceMassTmp[iLat] +=
        dStepsize * body[iBody].daIceBalance[iLat][iNstep];

  if (body[iBody].daIceBalance[iLat][iNstep] >= 0) {
    body[iBody].daIceAccumTot[iLat] += dStepsize *
                                       body[iBody].daIceBalance[iLat][iNstep] /
                                       body[iBody].iNumYears;

  } else {
    if (body[iBody].daIceMassTmp[iLat] > 0) {
      if (body[iBody].daIceMassTmp[iLat] >=
          dStepsize * body[iBody].daIceBalance[iLat][iNstep]) {

        body[iBody].daIceAblateTot[iLat] +=
              dStepsize * body[iBody].daIceBalance[iLat][iNstep] /
              body[iBody].iNumYears;

      } else {
        body[iBody].daIceAblateTot[iLat] += body[iBody].daIceMassTmp[iLat];
      }
    }
  }
  if (body[iBody].daIceMassTmp[iLat] < 0.0) {
    body[iBody].daIceMassTmp[iLat] = 0.0;
  } // don't let ice mass become negative
  if (body[iBody].daIceBalance[iLat][iNstep] < 0 &&
      body[iBody].daIceMassTmp[iLat] != 0) {

    if (body[iBody].daIceMassTmp[iLat] <=
        fabs(dStepsize * body[iBody].daIceBalance[iLat][iNstep])) {

      // adjust temperature
      body[iBody].daTempLand[iLat] +=
            -body[iBody].daIceMassTmp[iLat] * LFICE / body[iBody].dHeatCapLand;

    } else {
      body[iBody].daTempLand[iLat] +=
            dStepsize * body[iBody].daIceBalance[iLat][iNstep] * LFICE /
            body[iBody].dHeatCapLand; // adjust temperature
    }
  } else if (body[iBody].daIceBalance[iLat][iNstep] > 0) {
    body[iBody].daTempLand[iLat] +=
          dStepsize * body[iBody].daIceBalance[iLat][iNstep] * LFICE /
          body[iBody].dHeatCapLand; // adjust temperature
  }
}

void fvCalculateFluxesByLatitude(BODY *body, int iBody, int iLat, int iNday) {
  body[iBody].daFluxOutLand[iLat] =
        body[iBody].daPlanckASea[iLat] +
        body[iBody].daPlanckBSea[iLat] * body[iBody].daTempLand[iLat];
  body[iBody].daFluxOutWater[iLat] =
        body[iBody].daPlanckASea[iLat] +
        body[iBody].daPlanckBSea[iLat] * body[iBody].daTempWater[iLat];
  body[iBody].daFluxOut[iLat] =
        body[iBody].daLandFrac[iLat] * body[iBody].daFluxOutLand[iLat] +
        body[iBody].daWaterFrac[iLat] * body[iBody].daFluxOutWater[iLat];
  body[iBody].dFluxOutGlobalTmp +=
        body[iBody].daFluxOut[iLat] / body[iBody].iNumLats;

  body[iBody].daFluxInLand[iLat] = (1.0 - body[iBody].daAlbedoLand[iLat]) *
                                   body[iBody].daInsol[iLat][iNday];
  body[iBody].daFluxInWater[iLat] = (1.0 - body[iBody].daAlbedoWater[iLat]) *
                                    body[iBody].daInsol[iLat][iNday];
  body[iBody].daFluxIn[iLat] =
        body[iBody].daLandFrac[iLat] * body[iBody].daFluxInLand[iLat] +
        body[iBody].daWaterFrac[iLat] * body[iBody].daFluxInWater[iLat];
  body[iBody].dFluxInGlobalTmp +=
        body[iBody].daFluxIn[iLat] / body[iBody].iNumLats;
}

void fvPoiseGlobalProps(BODY *body, int iBody) {
  body[iBody].dTGlobal += body[iBody].dTGlobalTmp / (body[iBody].iNStepInYear);
  body[iBody].dFluxOutGlobal +=
        body[iBody].dFluxOutGlobalTmp / (body[iBody].iNStepInYear);
  body[iBody].dFluxInGlobal +=
        body[iBody].dFluxInGlobalTmp / (body[iBody].iNStepInYear);
}

void fvFinishSeaIce(BODY *body, double dStepsize, int iBody, int iLat,
                    int iNyear, int iNday, int iNstep) {
  fvSnowball(body, iBody);
  // ice growth/ablation
  if (body[iBody].bIceSheets) {
    fvCalculateIceSheets(body, dStepsize, iBody, iLat, iNstep);
  }

  fvCalcPlanckAB(body, iBody, iLat);
  fvCalculateFluxesByLatitude(body, iBody, iLat, iNday);
  fvPoiseAnnualAveragesByLatitude(body, iBody, iLat);
  fvPoiseDailyProps(body, iBody, iLat, iNyear, iNstep);
  fvPoiseSeasonalMinMax(body, iBody, iLat);
}

void fvCalculateSeaIce(BODY *body, double dStepsize, int iBody, int iNyear,
                       int iNday, int iNstep) {
  fvSeaIce(body, iBody);
  for (int iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (body[iBody].daSeaIceHeight[iLat] > 0) {
      // adjust height of present sea ice
      body[iBody].daSeaIceHeight[iLat] -= body[iBody].dSeasDeltat /
                                          body[iBody].dLatentHeatIce *
                                          body[iBody].daFluxSeaIce[iLat];

      if (body[iBody].daSeaIceHeight[iLat] < 0) {
        body[iBody].daSeaIceHeight[iLat] = 0;
      }
    } else {
      if (body[iBody].daTempWater[iLat] < body[iBody].dFrzTSeaIce) {
        // create new sea ice
        body[iBody].daSeaIceHeight[iLat] =
              -body[iBody].dHeatCapWater * body[iBody].dMeanMotion / (2 * PI) /
              body[iBody].dLatentHeatIce *
              (body[iBody].daTempWater[iLat] - body[iBody].dFrzTSeaIce);
      }
    }

    fvFinishSeaIce(body, dStepsize, iBody, iLat, iNyear, iNday, iNstep);
  }
}

void fvStaticSeaIce(BODY *body, double dStepsize, int iBody, int iNyear,
                    int iNday, int iNstep) {
  int iLat, jLat;

  for (iLat = 0; iLat < 2 * body[iBody].iNumLats; iLat++) {
    body[iBody].daTmpTempSea[iLat] = 0.0;
    for (jLat = 0; jLat < 2 * body[iBody].iNumLats; jLat++) {
      body[iBody].daTmpTempSea[iLat] +=
            body[iBody].daInvMSea[iLat][jLat] * body[iBody].daSourceLW[jLat];
    }
  }
  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    // temp change this time step
    body[iBody].daDeltaTempL[iLat] =
          body[iBody].daTmpTempSea[2 * iLat] - body[iBody].daTempLand[iLat];
    body[iBody].daDeltaTempW[iLat] = body[iBody].daTmpTempSea[2 * iLat + 1] -
                                     body[iBody].daTempWater[iLat];
    // calculate temperature and fluxes by latitude and global average
    body[iBody].daTempLand[iLat]  = body[iBody].daTmpTempSea[2 * iLat];
    body[iBody].daTempWater[iLat] = body[iBody].daTmpTempSea[2 * iLat + 1];

    fvFinishSeaIce(body, dStepsize, iBody, iLat, iNyear, iNday, iNstep);
  }
}

/**
Runs the seasonal EBM, called from ForceBehavior. The seasonal EBM runs on a
fixed time-step (of order days) to resolve the seasonal cycle. It can be said
to be in "equilibrium" though not in "steady-state".

@param body Struct containing all body information and variables
@param iBody Body in question
*/
void PoiseSeasonal(BODY *body, int iBody) {
  int iLat, jLat, iNstep, iNyear, iNday;
  double dStepsize;

  dStepsize = 2 * PI / body[iBody].dMeanMotion / body[iBody].iNStepInYear;

  // fvMatrixSeasonal(body,iBody);
  // XXX confusing! Should distinguish b/w years/orbits
  // XXX Needs to be broken down into functions!

  /* main loop */
  for (iNyear = 0; iNyear < body[iBody].iNumYears; iNyear++) {
    fvPoiseSeasonalInitialize(body, iBody, iNyear);

    fvAlbedoSeasonal(body, iBody, 0);
    body[iBody].dAlbedoGlobal = 0.0;

    for (iNstep = 0; iNstep < body[iBody].iNStepInYear; iNstep++) {
      body[iBody].dTGlobalTmp       = 0.0;
      body[iBody].dFluxInGlobalTmp  = 0.0;
      body[iBody].dFluxOutGlobalTmp = 0.0;
      iNday = floor(body[iBody].dSeasDeltat * iNstep * body[iBody].iNDays);

      fvSourceFSeas(body, iBody, iNday);

      if (body[iBody].bSeaIceModel == 1) {
        fvCalculateSeaIce(body, dStepsize, iBody, iNyear, iNday, iNstep);
      } else {
        fvStaticSeaIce(body, dStepsize, iBody, iNyear, iNday, iNstep);
      }

      fvTempGradientSea(body, body[iBody].dSeasDeltax, iBody);
      fvFluxesByLatitude(body, iBody, iNyear, iNstep);
      fvPoiseGlobalProps(body, iBody);

      EnergyResiduals(body, iBody, iNday);
      fvAlbedoSeasonal(body, iBody, iNday);
      if (body[iBody].bAccuracyMode) {
        fvMatrixSeasonal(body, iBody);
      }
    }

    if (body[iBody].bIceSheets) {
      for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
        if (iNyear != 0) {
          body[iBody].daIceBalanceAnnual[iLat] +=
                dStepsize / 2. * (body[iBody].daIceBalance[iLat][0]) /
                (body[iBody].iNumYears * 2 * PI / body[iBody].dMeanMotion);
        }
        for (iNstep = 1; iNstep < body[iBody].iNStepInYear; iNstep++) {
          // trapezoid rule
          // dStepsize = 2*PI/body[iBody].dMeanMotion/body[iBody].iNStepInYear;
          body[iBody].daIceBalanceAnnual[iLat] +=
                dStepsize / 2. *
                (body[iBody].daIceBalance[iLat][iNstep] +
                 body[iBody].daIceBalance[iLat][iNstep - 1]) /
                (body[iBody].iNumYears * 2 * PI / body[iBody].dMeanMotion);
          // above gets yearly average over NumYears
        }
        if (iNyear != (body[iBody].iNumYears - 1)) {
          body[iBody].daIceBalanceAnnual[iLat] +=
                dStepsize / 2. * (body[iBody].daIceBalance[iLat][iNstep - 1]) /
                (body[iBody].iNumYears * 2 * PI / body[iBody].dMeanMotion);
        }
      }
    }
  }
}

/**
Calculates the net balance between melting and accumulation of ice on land
at a given EBM time-step

@param body Struct containing all body information
@param iBody Body in question
@param iLat Latitude cell in question
@return Net gain/loss of ice
*/
double fdIceMassBalance(BODY *body, int iBody, int iLat) {
  double Tice, dTmp, dHeight, dGamma, dTs;
  Tice = 273.15;

  if (body[iBody].bElevFB) {
    // elevation feedback modeled by assuming surface temp is affected by height
    // Very experimental!
    // dry adiabatic lapse rate
    dGamma = body[iBody].dLapseR;
    dHeight =
          (body[iBody].daIceMassTmp[iLat] / RHOICE -
           body[iBody].dRefHeight + // height above or below some ref altitude
           body[iBody].daBedrockH[iLat]);
    //''surface'' temperature of ice
    dTs = (body[iBody].daTempLand[iLat] + 273.15 - dGamma * dHeight);
  } else {
    dTs = body[iBody].daTempLand[iLat] + 273.15;
  }
  /* first, calculate melting/accumulation */
  if (dTs > 273.15) {
    /* Ice melting */
    /* (AblateFF is used to match Huybers&Tziperman's ablation rate)*/
    dTmp = body[iBody].dAblateFF * SIGMA *
           ((Tice * Tice * Tice * Tice) - ((dTs) * (dTs) * (dTs) * (dTs))) /
           LFICE;
  } else {
    if (body[iBody].bSnowball) {
      /* no precip once planet is frozen */
      dTmp = 0.0;
    } else {
      if (body[iBody].dIceMassTot >= MOCEAN) {
        /* ice growth limited by mass of water available */
        dTmp = 0.0;
      } else {
        /* Ice deposits at fixed rate */
        dTmp = body[iBody].dIceDepRate;
      }
    }
  }
  return dTmp;
}

/**
Construct matrix that evolves the ice sheet flow + net balance

@param body Struct containing body information
@param iBody Body in question
*/
void IceSheetTriDiag(BODY *body, int iBody) {
  double bTmp;
  int iLat, iNumLats;
  iNumLats = body[iBody].iNumLats;

  bTmp                       = body[iBody].daIceSheetMat[0][0];
  body[iBody].daIceHeight[0] = body[iBody].daIcePropsTmp[0] / bTmp;
  for (iLat = 1; iLat < iNumLats; iLat++) {
    body[iBody].daIceGamTmp[iLat] =
          body[iBody].daIceSheetMat[iLat - 1][iLat] / bTmp;
    bTmp = body[iBody].daIceSheetMat[iLat][iLat] -
           body[iBody].daIceSheetMat[iLat][iLat - 1] *
                 body[iBody].daIceGamTmp[iLat];
    // LCOV_EXCL_START
    if (bTmp == 0) {
      fprintf(stderr, "Ice sheet tri-diagonal solution failed\n");
      exit(EXIT_INPUT);
      // LCOV_EXCL_STOP
    }
    body[iBody].daIceHeight[iLat] = (body[iBody].daIcePropsTmp[iLat] -
                                     body[iBody].daIceSheetMat[iLat][iLat - 1] *
                                           body[iBody].daIceHeight[iLat - 1]) /
                                    bTmp;
  }
  for (iLat = 1; iLat < iNumLats; iLat++) {
    body[iBody].daIceHeight[iNumLats - iLat - 1] -=
          body[iBody].daIceGamTmp[iNumLats - iLat] *
          body[iBody].daIceHeight[iNumLats - iLat];
  }
}

/**
Main ice sheet routine. Integrates the ice sheets via Crank-Nicholson method in
ForceBehavior in the same fashion as Huybers' model.

@param body Struct containing body information
@param evolve Struct containing evolution information
@param iBody Body in question
*/
void PoiseIceSheets(BODY *body, EVOLVE *evolve, int iBody) {
  /* integrate ice sheets via Crank-Nicholson method in ForceBehavior
     in the same way Huybers' model works */
  int iLat, jLat, skip;
  double IceTime, IceDt, RunSeasNext;
  double deltax, Tice, Aice, dGrav, dHdt;
  IceTime = evolve->dTime;
  Tice    = 270;
  skip    = 1;

  dGrav =
        BIGG * body[iBody].dMass / (body[iBody].dRadius * body[iBody].dRadius);
  /* deformability of ice, dependent on temperature */
  if (Tice < 263) {
    Aice = a1ICE * exp(-Q1ICE / (RGAS * Tice));
  } else {
    Aice = a2ICE * exp(-Q2ICE / (RGAS * Tice));
  }

  for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
    if (body[iBody].daIceMass[iLat] < 1e-30) {
      body[iBody].daIceMass[iLat] = 0.0;
    }
    body[iBody].daIceHeight[iLat]     = body[iBody].daIceMass[iLat] / RHOICE;
    body[iBody].daIceBalanceAvg[iLat] = 0.0;
    body[iBody].daIceFlowAvg[iLat]    = 0.0;
    if (body[iBody].daIceMass[iLat] > 0 ||
        body[iBody].daIceBalanceAnnual[iLat] > 0) {
      skip = 0;
    }
  }

  RunSeasNext =
        IceTime + body[iBody].iReRunSeas * 2 * PI / body[iBody].dMeanMotion;
  IceDt = body[iBody].iIceTimeStep * 2 * PI / body[iBody].dMeanMotion;

  if (body[iBody].iIceTimeStep > 1) {
    if (body[iBody].dTGlobal < 0) {
      /* tip toe into snowball state to prevent instability */
      IceDt = 2 * PI / body[iBody].dMeanMotion;
    }
  }

  if (skip == 0) {
    while (IceTime < evolve->dTime + evolve->dCurrentDt) {
      if (IceTime + IceDt > evolve->dTime + evolve->dCurrentDt) {
        // ice time step carries past start of next RK time step
        IceDt = evolve->dTime + evolve->dCurrentDt - IceTime;
      }

      fvSnowball(body, iBody);
      /* first, get ice balances */
      for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
        if (body[iBody].daIceMass[iLat] <= 0 &&
            body[iBody].daIceBalanceAnnual[iLat] < 0.0) {

          body[iBody].daIceBalanceTmp[iLat] = 0;
        } else if (body[iBody].dIceMassTot >= MOCEAN &&
                   body[iBody].daIceBalanceAnnual[iLat] > 0.0) {

          body[iBody].daIceBalanceTmp[iLat] = 0;
        } else {
          body[iBody].daIceBalanceTmp[iLat] =
                body[iBody].daIceBalanceAnnual[iLat] / RHOICE;
        }
        if (body[iBody].bSnowball == 1) {
          body[iBody].daIceBalanceTmp[iLat] = 0;
        }
      }

      deltax = 2.0 / body[iBody].iNumLats;

      for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
        /* calculate derivative to 2nd order accuracy */
        if (iLat == 0) {
          body[iBody].daDIceHeightDy[iLat] =
                sqrt(1.0 - (body[iBody].daXBoundary[iLat + 1] *
                            body[iBody].daXBoundary[iLat + 1])) *
                (body[iBody].daIceHeight[iLat + 1] +
                 body[iBody].daBedrockH[iLat + 1] -
                 body[iBody].daIceHeight[iLat] - body[iBody].daBedrockH[iLat]) /
                (body[iBody].dRadius * deltax);
        } else if (iLat == (body[iBody].iNumLats - 1)) {
          body[iBody].daDIceHeightDy[iLat] =
                sqrt(1.0 - (body[iBody].daXBoundary[iLat] *
                            body[iBody].daXBoundary[iLat])) *
                (body[iBody].daIceHeight[iLat] + body[iBody].daBedrockH[iLat] -
                 body[iBody].daIceHeight[iLat - 1] -
                 body[iBody].daBedrockH[iLat - 1]) /
                (body[iBody].dRadius * deltax);
        } else {
          body[iBody].daDIceHeightDy[iLat] =
                (sqrt(1.0 - (body[iBody].daXBoundary[iLat + 1] *
                             body[iBody].daXBoundary[iLat + 1])) *
                       (body[iBody].daIceHeight[iLat + 1] +
                        body[iBody].daBedrockH[iLat + 1] -
                        body[iBody].daIceHeight[iLat] -
                        body[iBody].daBedrockH[iLat]) /
                       (body[iBody].dRadius * deltax) +
                 sqrt(1.0 - (body[iBody].daXBoundary[iLat] *
                             body[iBody].daXBoundary[iLat])) *
                       (body[iBody].daIceHeight[iLat] +
                        body[iBody].daBedrockH[iLat] -
                        body[iBody].daIceHeight[iLat - 1] -
                        body[iBody].daBedrockH[iLat - 1]) /
                       (body[iBody].dRadius * deltax)) /
                2.0;
        }

        body[iBody].daIceFlow[iLat] =
              2 * Aice * pow(RHOICE * dGrav, nGLEN) / (nGLEN + 2.0) *
              pow(fabs(body[iBody].daDIceHeightDy[iLat]), nGLEN - 1) *
              pow(body[iBody].daIceHeight[iLat] + body[iBody].daBedrockH[iLat],
                  nGLEN + 2);
        body[iBody].daSedShear[iLat] = RHOICE * dGrav *
                                       body[iBody].daIceHeight[iLat] *
                                       body[iBody].daDIceHeightDy[iLat];
        body[iBody].daBasalFlow[iLat] = fdBasalFlow(body, iBody, iLat);
      }

      for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
        if (IceTime != evolve->dTime) {
          body[iBody].daIceSheetDiff[iLat] =
                -0.5 * (body[iBody].daIceFlowMid[iLat] +
                        body[iBody].daBasalFlowMid[iLat]);
          if (iLat == body[iBody].iNumLats - 1) {
            body[iBody].daIceSheetDiff[iLat + 1] =
                  -0.5 * (body[iBody].daIceFlowMid[iLat + 1] +
                          body[iBody].daBasalFlowMid[iLat + 1]);
          }
        }

        if (iLat == 0) {
          body[iBody].daIceFlowMid[iLat]   = 0;
          body[iBody].daBasalFlowMid[iLat] = 0;
        } else if (iLat == body[iBody].iNumLats - 1) {
          body[iBody].daIceFlowMid[iLat] = (body[iBody].daIceFlow[iLat] +
                                            body[iBody].daIceFlow[iLat - 1]) /
                                           2.0;
          body[iBody].daIceFlowMid[iLat + 1] = 0;
          body[iBody].daBasalFlowMid[iLat] =
                (body[iBody].daBasalFlow[iLat] +
                 body[iBody].daBasalFlow[iLat - 1]) /
                2.0;
          body[iBody].daBasalFlowMid[iLat + 1] = 0;
        } else {
          body[iBody].daIceFlowMid[iLat] = (body[iBody].daIceFlow[iLat] +
                                            body[iBody].daIceFlow[iLat - 1]) /
                                           2.0;
          body[iBody].daBasalFlowMid[iLat] =
                (body[iBody].daBasalFlow[iLat] +
                 body[iBody].daBasalFlow[iLat - 1]) /
                2.0;
        }

        if (IceTime == evolve->dTime) {
          body[iBody].daIceSheetDiff[iLat] = body[iBody].daIceFlowMid[iLat];
          if (iLat == body[iBody].iNumLats - 1) {
            body[iBody].daIceSheetDiff[iLat + 1] =
                  body[iBody].daIceFlowMid[iLat + 1];
          }
          body[iBody].daIcePropsTmp[iLat] =
                body[iBody].daIceHeight[iLat] +
                body[iBody].daIceBalanceTmp[iLat] * IceDt;
        } else {
          body[iBody].daIceSheetDiff[iLat] +=
                1.5 * (body[iBody].daIceFlowMid[iLat] +
                       body[iBody].daBasalFlowMid[iLat]);
          if (iLat == body[iBody].iNumLats - 1) {
            body[iBody].daIceSheetDiff[iLat + 1] +=
                  1.5 * (body[iBody].daIceFlowMid[iLat + 1] +
                         body[iBody].daBasalFlowMid[iLat + 1]);
          }
        }
      }

      for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
        for (jLat = 0; jLat < body[iBody].iNumLats; jLat++) {
          if (jLat == iLat) {
            body[iBody].daIceSheetMat[iLat][jLat] =
                  1.0 + 0.5 * IceDt *
                              (body[iBody].daIceSheetDiff[iLat] /
                                     (body[iBody].daYBoundary[iLat] *
                                      body[iBody].daYBoundary[iLat]) +
                               body[iBody].daIceSheetDiff[iLat + 1] /
                                     (body[iBody].daYBoundary[iLat + 1] *
                                      body[iBody].daYBoundary[iLat + 1]));
            // superdiagonal
          } else if (jLat == iLat + 1) {
            body[iBody].daIceSheetMat[iLat][jLat] =
                  -0.5 * IceDt * body[iBody].daIceSheetDiff[jLat] /
                  (body[iBody].daYBoundary[jLat] *
                   body[iBody].daYBoundary[jLat]);
            // subdiagonal
          } else if (jLat == iLat - 1) {
            body[iBody].daIceSheetMat[iLat][jLat] =
                  -0.5 * IceDt * body[iBody].daIceSheetDiff[iLat] /
                  (body[iBody].daYBoundary[iLat] *
                   body[iBody].daYBoundary[iLat]);
          } else {
            body[iBody].daIceSheetMat[iLat][jLat] = 0.0;
          }
        }
        if (IceTime != evolve->dTime) {
          if (iLat == 0) {
            body[iBody].daIcePropsTmp[iLat] =
                  body[iBody].daIceBalanceTmp[iLat] * IceDt +
                  (1 - 0.5 * IceDt *
                             (body[iBody].daIceSheetDiff[iLat + 1] /
                              (body[iBody].daYBoundary[iLat + 1] *
                               body[iBody].daYBoundary[iLat + 1]))) *
                        body[iBody].daIceHeight[iLat] +
                  0.5 * IceDt * body[iBody].daIceSheetDiff[iLat + 1] *
                        body[iBody].daIceHeight[iLat + 1] /
                        (body[iBody].daYBoundary[iLat + 1] *
                         body[iBody].daYBoundary[iLat + 1]);

          } else if (iLat == body[iBody].iNumLats - 1) {
            body[iBody].daIcePropsTmp[iLat] =
                  body[iBody].daIceBalanceTmp[iLat] * IceDt +
                  (1 - 0.5 * IceDt *
                             (body[iBody].daIceSheetDiff[iLat] /
                              (body[iBody].daYBoundary[iLat] *
                               body[iBody].daYBoundary[iLat]))) *
                        body[iBody].daIceHeight[iLat] +
                  0.5 * IceDt * body[iBody].daIceSheetDiff[iLat] *
                        body[iBody].daIceHeight[iLat - 1] /
                        (body[iBody].daYBoundary[iLat] *
                         body[iBody].daYBoundary[iLat]);
          } else {
            body[iBody].daIcePropsTmp[iLat] =
                  body[iBody].daIceBalanceTmp[iLat] * IceDt +
                  (1 - 0.5 * IceDt *
                             (body[iBody].daIceSheetDiff[iLat] /
                                    (body[iBody].daYBoundary[iLat] *
                                     body[iBody].daYBoundary[iLat]) +
                              body[iBody].daIceSheetDiff[iLat + 1] /
                                    (body[iBody].daYBoundary[iLat + 1] *
                                     body[iBody].daYBoundary[iLat + 1]))) *
                        body[iBody].daIceHeight[iLat] +
                  0.5 * IceDt * body[iBody].daIceSheetDiff[iLat + 1] *
                        body[iBody].daIceHeight[iLat + 1] /
                        (body[iBody].daYBoundary[iLat + 1] *
                         body[iBody].daYBoundary[iLat + 1]) +
                  0.5 * IceDt * body[iBody].daIceSheetDiff[iLat] *
                        body[iBody].daIceHeight[iLat - 1] /
                        (body[iBody].daYBoundary[iLat] *
                         body[iBody].daYBoundary[iLat]);
          }
        }
      }

      IceSheetTriDiag(body, iBody);
      for (iLat = 0; iLat < body[iBody].iNumLats; iLat++) {
        body[iBody].daIceMass[iLat] = body[iBody].daIceHeight[iLat] * RHOICE;
        if (body[iBody].daIceMass[iLat] < 1e-30) {
          body[iBody].daIceMass[iLat]   = 0.0;
          body[iBody].daIceHeight[iLat] = 0.0;
        }
        body[iBody].daIceBalanceAvg[iLat] +=
              body[iBody].daIceBalanceTmp[iLat] * IceDt / evolve->dCurrentDt;
        if (iLat == 0) {
          body[iBody].daIceFlowAvg[iLat] +=
                body[iBody].daIceSheetDiff[iLat + 1] *
                (body[iBody].daIceHeight[iLat + 1] -
                 body[iBody].daIceHeight[iLat]) /
                (body[iBody].daYBoundary[iLat + 1] *
                 body[iBody].daYBoundary[iLat + 1]) *
                IceDt / evolve->dCurrentDt;
        } else if (iLat == body[iBody].iNumLats - 1) {
          body[iBody].daIceFlowAvg[iLat] +=
                -body[iBody].daIceSheetDiff[iLat] *
                (body[iBody].daIceHeight[iLat] -
                 body[iBody].daIceHeight[iLat - 1]) /
                (body[iBody].daYBoundary[iLat] *
                 body[iBody].daYBoundary[iLat]) *
                IceDt / evolve->dCurrentDt;
        } else {
          body[iBody].daIceFlowAvg[iLat] +=
                (body[iBody].daIceSheetDiff[iLat + 1] *
                       (body[iBody].daIceHeight[iLat + 1] -
                        body[iBody].daIceHeight[iLat]) /
                       (body[iBody].daYBoundary[iLat + 1] *
                        body[iBody].daYBoundary[iLat + 1]) -
                 body[iBody].daIceSheetDiff[iLat] *
                       (body[iBody].daIceHeight[iLat] -
                        body[iBody].daIceHeight[iLat - 1]) /
                       (body[iBody].daYBoundary[iLat] *
                        body[iBody].daYBoundary[iLat])) *
                IceDt / evolve->dCurrentDt;
        }
        dHdt = 1. / (BROCKTIME * YEARSEC) *
               (body[iBody].daBedrockHEq[iLat] - body[iBody].daBedrockH[iLat] -
                RHOICE * body[iBody].daIceHeight[iLat] / RHOBROCK);
        body[iBody].daBedrockH[iLat] += dHdt * IceDt;
      }

      IceTime += IceDt;
      if (IceTime >= RunSeasNext) {
        /* rerun the seasonal model to make sure climate params
           don't get too whack */
        PoiseSeasonal(body, iBody);
        RunSeasNext +=
              body[iBody].iReRunSeas * 2 * PI / body[iBody].dMeanMotion;
      }
    }
  }
}
