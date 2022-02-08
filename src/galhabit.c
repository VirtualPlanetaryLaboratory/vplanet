/**
   @file galhabit.c

   @brief Subroutines that simulate the galactic habitat of a stellar system.

   @author Russell Deitrick ([deitrr](https://github.com/deitrr/))

   @date July 7 2015

*/

/* note August 15: system->dScalingF added to NearbyStarDist(), VelocityDisp(),
 * VerifyGalHabit(), ForceBehavior(), CalcEncounterRate */

#include "vplanet.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void BodyCopyGalHabit(BODY *dest, BODY *src, int iTideModel, int iNumBodies,
                      int iBody) {
  dest[iBody].dPeriQ        = src[iBody].dPeriQ;
  dest[iBody].dArgP         = src[iBody].dArgP;
  dest[iBody].dLongP        = src[iBody].dLongP;
  dest[iBody].dInc          = src[iBody].dInc;
  dest[iBody].dLongA        = src[iBody].dLongA;
  dest[iBody].dEccX         = src[iBody].dEccX;
  dest[iBody].dEccY         = src[iBody].dEccY;
  dest[iBody].dEccZ         = src[iBody].dEccZ;
  dest[iBody].dAngM         = src[iBody].dAngM;
  dest[iBody].dAngMX        = src[iBody].dAngMX;
  dest[iBody].dAngMY        = src[iBody].dAngMY;
  dest[iBody].dAngMZ        = src[iBody].dAngMZ;
  dest[iBody].dEccXTmp      = src[iBody].dEccXTmp;
  dest[iBody].dEccYTmp      = src[iBody].dEccYTmp;
  dest[iBody].dEccZTmp      = src[iBody].dEccZTmp;
  dest[iBody].dAngMXTmp     = src[iBody].dAngMXTmp;
  dest[iBody].dAngMYTmp     = src[iBody].dAngMYTmp;
  dest[iBody].dAngMZTmp     = src[iBody].dAngMZTmp;
  dest[iBody].dMassInterior = src[iBody].dMassInterior;
}

/**************** GALHABIT options ********************/

void ReadGalacDensity(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than or equal to 0.\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    system->dGalacDensity = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &system->dGalacDensity, files->iNumInputs);
  }
}

void ReadRForm(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than or equal to 0.\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    system->dRForm = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &system->dRForm, files->iNumInputs);
  }
}

void ReadStarScaleL(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than or equal to 0.\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    system->dStarScaleL = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &system->dStarScaleL, files->iNumInputs);
  }
}

void ReadTMigration(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than or equal to 0.\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    system->dTMigration = dTmp * fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &system->dTMigration, files->iNumInputs);
  }
}

void ReadRadialMigr(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    /* Option was found */
    system->bRadialMigr = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &system->bRadialMigr, files->iNumInputs);
  }
}

void ReadGasDensity(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than or equal to 0.\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    system->dGasDensity = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &system->dGasDensity, files->iNumInputs);
  }
}

void ReadDMDensity(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than or equal to 0.\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    system->dDMDensity = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &system->dDMDensity, files->iNumInputs);
  }
}

void ReadRandSeed(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int iTmp;

  AddOptionInt(files->Infile[iFile].cIn, options->cName, &iTmp, &lTmp,
               control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    system->iSeed = iTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &system->iSeed, files->iNumInputs);
  }
}


void ReadEncounterRad(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    //     NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      system->dEncounterRad =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      system->dEncounterRad =
            dTmp * fdUnitsLength(control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    AssignDefaultDouble(options, &system->dEncounterRad, files->iNumInputs);
  }
}

void ReadStellarEnc(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    /* Option was found */
    system->bStellarEnc = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &system->bStellarEnc, files->iNumInputs);
  }
}

void ReadOutputEnc(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    /* Option was found */
    system->bOutputEnc = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &system->bOutputEnc, files->iNumInputs);
  }
}

void ReadTimeEvolVelDisp(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    /* Option was found */
    system->bTimeEvolVelDisp = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &system->bTimeEvolVelDisp, files->iNumInputs);
  }
}

void ReadHostBinary(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    body[iFile - 1].bHostBinary = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bHostBinary, files->iNumInputs);
  }
}

void ReadGalacTides(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    body[iFile - 1].bGalacTides = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &body[iFile - 1].bGalacTides, files->iNumInputs);
  }
}

// void ReadMinAllowed(BODY *body,CONTROL *control,FILES *files,OPTIONS
// *options,SYSTEM *system,int iFile) {
//   /* This parameter cannot exist in primary file */
//   int lTmp=-1;
//   double dTmp;
//
//   AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
//   if (lTmp >= 0) {
//     NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
//     if (dTmp < 0)
//       body[iFile-1].dMinAllowed =
//       dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
//     else
//       body[iFile-1].dMinAllowed =
//       dTmp*fdUnitsLength(control->Units[iFile].iLength);
//     UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
//   } else
//     if (iFile > 0)
//       body[iFile-1].dMinAllowed = options->dDefault;
// }

void ReadHostBinSemi(BODY *body, CONTROL *control, FILES *files,
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
      body[iFile - 1].dHostBinSemi =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dHostBinSemi =
            dTmp * fdUnitsLength(control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dHostBinSemi = options->dDefault;
  }
}

void ReadMinStellarApproach(BODY *body, CONTROL *control, FILES *files,
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
      body[iFile - 1].dMinStellarApproach =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dMinStellarApproach =
            dTmp * fdUnitsLength(control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dMinStellarApproach = options->dDefault;
  }
}

void ReadHostBinEcc(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* Cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0 || dTmp >= 1) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be in the range [0,1).\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dHostBinEcc = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);

  } else if (iFile > 0) {
    AssignDefaultDouble(options, &body[iFile - 1].dHostBinEcc,
                        files->iNumInputs);
  }
}

/* argument of pericenter */
void ReadHostBinArgP(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2 * PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,2*PI].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,360].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dHostBinArgP = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dHostBinArgP = options->dDefault;
  }
}

void ReadHostBinLongA(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2 * PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,2*PI].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,360].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dHostBinLongA = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dHostBinLongA = options->dDefault;
  }
}


void ReadHostBinInc(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,PI].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,180].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dHostBinInc = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dHostBinInc = options->dDefault;
  }
}

void ReadHostBinMass1(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with Radius, Density and MassRad */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dHostBinMass1 =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dHostBinMass1 =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dHostBinMass1 = options->dDefault;
  }
}

void InitializeOptionsGalHabit(OPTIONS *options, fnReadOption fnRead[]) {
  sprintf(options[OPT_GALACDENSITY].cName, "dGalacDensity");
  sprintf(options[OPT_GALACDENSITY].cDescr, "Density of galactic environment");
  sprintf(options[OPT_GALACDENSITY].cDefault,
          "0.102"); // need to find updated value
  sprintf(options[OPT_GALACDENSITY].cDimension, "mass/length^3");
  options[OPT_GALACDENSITY].dDefault   = 0.102;
  options[OPT_GALACDENSITY].iType      = 2;
  options[OPT_GALACDENSITY].bMultiFile = 0;
  fnRead[OPT_GALACDENSITY]             = &ReadGalacDensity;

  sprintf(options[OPT_RANDSEED].cName, "iRandSeed");
  sprintf(options[OPT_RANDSEED].cDescr,
          "Seed for random number generator (stellar encounters)");
  sprintf(options[OPT_RANDSEED].cDefault, "42");
  options[OPT_RANDSEED].dDefault   = 42;
  options[OPT_RANDSEED].iType      = 1;
  options[OPT_RANDSEED].bMultiFile = 0;
  fnRead[OPT_RANDSEED]             = &ReadRandSeed;

  sprintf(options[OPT_ENCOUNTERRAD].cName, "dEncounterRad");
  sprintf(options[OPT_ENCOUNTERRAD].cDescr,
          "Radius at which stellar encounters occur");
  sprintf(options[OPT_ENCOUNTERRAD].cDefault, "206265 AU");
  sprintf(options[OPT_ENCOUNTERRAD].cDimension, "length");
  options[OPT_ENCOUNTERRAD].dDefault   = 206265.0 * AUM;
  options[OPT_ENCOUNTERRAD].iType      = 2;
  options[OPT_ENCOUNTERRAD].bMultiFile = 0;
  fnRead[OPT_ENCOUNTERRAD]             = &ReadEncounterRad;

  sprintf(options[OPT_RFORM].cName, "dRForm");
  sprintf(options[OPT_RFORM].cDescr, "Galactic formation radius");
  sprintf(options[OPT_RFORM].cDefault, "4.5 kpc");
  sprintf(options[OPT_RFORM].cDimension, "length");
  options[OPT_RFORM].dDefault   = 4.5;
  options[OPT_RFORM].iType      = 2;
  options[OPT_RFORM].bMultiFile = 0;
  fnRead[OPT_RFORM]             = &ReadRForm;

  sprintf(options[OPT_TMIGRATION].cName, "dTMigration");
  sprintf(options[OPT_TMIGRATION].cDescr, "Time of radial migration");
  sprintf(options[OPT_TMIGRATION].cDefault, "3 Gy");
  sprintf(options[OPT_TMIGRATION].cDimension, "time");
  options[OPT_TMIGRATION].dDefault   = 3e9 * YEARSEC;
  options[OPT_TMIGRATION].iType      = 2;
  options[OPT_TMIGRATION].bMultiFile = 0;
  options[OPT_TMIGRATION].dNeg       = YEARSEC;
  sprintf(options[OPT_TMIGRATION].cNeg, "Years");
  fnRead[OPT_TMIGRATION] = &ReadTMigration;

  sprintf(options[OPT_RADIALMIGR].cName, "bRadialMigr");
  sprintf(options[OPT_RADIALMIGR].cDescr, "Use radial migration?");
  sprintf(options[OPT_RADIALMIGR].cDefault, "0");
  options[OPT_RADIALMIGR].dDefault   = 0;
  options[OPT_RADIALMIGR].iType      = 0;
  options[OPT_RADIALMIGR].bMultiFile = 0;
  fnRead[OPT_RADIALMIGR]             = &ReadRadialMigr;

  sprintf(options[OPT_GASDENSITY].cName, "dGasDensity");
  sprintf(options[OPT_GASDENSITY].cDescr, "Local ISM density");
  sprintf(options[OPT_GASDENSITY].cDefault, "0.05 Msun pc^3");
  sprintf(options[OPT_GASDENSITY].cDimension, "mass/length^3");
  options[OPT_GASDENSITY].dDefault   = 0.05;
  options[OPT_GASDENSITY].iType      = 2;
  options[OPT_GASDENSITY].bMultiFile = 0;
  fnRead[OPT_GASDENSITY]             = &ReadGasDensity;

  sprintf(options[OPT_DMDENSITY].cName, "dDMDensity");
  sprintf(options[OPT_DMDENSITY].cDescr, "Local dark matter density");
  sprintf(options[OPT_DMDENSITY].cDefault, "0.01 Msun pc^3");
  sprintf(options[OPT_DMDENSITY].cDimension, "mass/length^3");
  options[OPT_DMDENSITY].dDefault   = 0.01;
  options[OPT_DMDENSITY].iType      = 2;
  options[OPT_DMDENSITY].bMultiFile = 0;
  fnRead[OPT_DMDENSITY]             = &ReadDMDensity;

  sprintf(options[OPT_STARSCALEL].cName, "dStarScaleL");
  sprintf(options[OPT_STARSCALEL].cDescr, "Stellar radial scale length in MW");
  sprintf(options[OPT_STARSCALEL].cDefault, "2.4 kpc");
  sprintf(options[OPT_STARSCALEL].cDimension, "length");
  options[OPT_STARSCALEL].dDefault   = 2.4;
  options[OPT_STARSCALEL].iType      = 2;
  options[OPT_STARSCALEL].bMultiFile = 0;
  fnRead[OPT_STARSCALEL]             = &ReadStarScaleL;


  sprintf(options[OPT_STELLARENC].cName, "bStellarEnc");
  sprintf(options[OPT_STELLARENC].cDescr, "Model stellar encounters?");
  sprintf(options[OPT_STELLARENC].cDefault, "1");
  options[OPT_STELLARENC].dDefault   = 1;
  options[OPT_STELLARENC].iType      = 0;
  options[OPT_STELLARENC].bMultiFile = 0;
  fnRead[OPT_STELLARENC]             = &ReadStellarEnc;

  sprintf(options[OPT_OUTPUTENC].cName, "bOutputEnc");
  sprintf(options[OPT_OUTPUTENC].cDescr,
          "Output stellar encounter information?");
  sprintf(options[OPT_OUTPUTENC].cDefault, "0");
  options[OPT_OUTPUTENC].dDefault   = 0;
  options[OPT_OUTPUTENC].iType      = 0;
  options[OPT_OUTPUTENC].bMultiFile = 0;
  fnRead[OPT_OUTPUTENC]             = &ReadOutputEnc;

  sprintf(options[OPT_TIMEEVOLVELDISP].cName, "bTimeEvolVelDisp");
  sprintf(options[OPT_TIMEEVOLVELDISP].cDescr,
          "Scale velocity dispersion of stars with sqrt(t)?");
  sprintf(options[OPT_TIMEEVOLVELDISP].cDefault, "1");
  sprintf(options[OPT_TIMEEVOLVELDISP].cDimension, "length/time");
  options[OPT_TIMEEVOLVELDISP].dDefault   = 1;
  options[OPT_TIMEEVOLVELDISP].iType      = 0;
  options[OPT_TIMEEVOLVELDISP].bMultiFile = 0;
  fnRead[OPT_TIMEEVOLVELDISP]             = &ReadTimeEvolVelDisp;

  sprintf(options[OPT_HOSTBINARY].cName, "bHostBinary");
  sprintf(options[OPT_HOSTBINARY].cDescr,
          "Include 3 body interactions with binary star");
  sprintf(options[OPT_HOSTBINARY].cDefault, "0");
  options[OPT_HOSTBINARY].dDefault   = 0;
  options[OPT_HOSTBINARY].iType      = 0;
  options[OPT_HOSTBINARY].bMultiFile = 0;
  fnRead[OPT_HOSTBINARY]             = &ReadHostBinary;

  sprintf(options[OPT_GALACTIDES].cName, "bGalacTides");
  sprintf(options[OPT_GALACTIDES].cDescr, "Include galactic tides");
  sprintf(options[OPT_GALACTIDES].cDefault, "1");
  options[OPT_GALACTIDES].dDefault   = 1;
  options[OPT_GALACTIDES].iType      = 0;
  options[OPT_GALACTIDES].bMultiFile = 0;
  fnRead[OPT_GALACTIDES]             = &ReadGalacTides;

  sprintf(options[OPT_MINSTELLARAPPROACH].cName, "dMinStellarApproach");
  sprintf(options[OPT_MINSTELLARAPPROACH].cDescr,
          "Minimum close approach distance to primary");
  sprintf(options[OPT_MINSTELLARAPPROACH].cDefault, "1 AU");
  sprintf(options[OPT_MINSTELLARAPPROACH].cDimension, "length");
  options[OPT_MINSTELLARAPPROACH].dDefault   = AUM;
  options[OPT_MINSTELLARAPPROACH].iType      = 2;
  options[OPT_MINSTELLARAPPROACH].bMultiFile = 0;
  options[OPT_MINSTELLARAPPROACH].dNeg       = AUM;
  sprintf(options[OPT_MINSTELLARAPPROACH].cNeg, "AU");
  fnRead[OPT_MINSTELLARAPPROACH] = &ReadMinStellarApproach;

  sprintf(options[OPT_HOSTBINECC].cName, "dHostBinEcc");
  sprintf(options[OPT_HOSTBINECC].cDescr, "eccentricity of host binary");
  sprintf(options[OPT_HOSTBINECC].cDefault, "0.51");
  sprintf(options[OPT_HOSTBINECC].cDimension, "nd");
  options[OPT_HOSTBINECC].dDefault   = 0.51;
  options[OPT_HOSTBINECC].iType      = 2;
  options[OPT_HOSTBINECC].bMultiFile = 0;
  fnRead[OPT_HOSTBINECC]             = &ReadHostBinEcc;

  sprintf(options[OPT_HOSTBINSEMI].cName, "dHostBinSemi");
  sprintf(options[OPT_HOSTBINSEMI].cDescr, "Semi-major of host binary");
  sprintf(options[OPT_HOSTBINSEMI].cDefault, "17.57 AU");
  sprintf(options[OPT_HOSTBINSEMI].cDimension, "length");
  options[OPT_HOSTBINSEMI].dDefault   = 17.57 * AUM;
  options[OPT_HOSTBINSEMI].iType      = 2;
  options[OPT_HOSTBINSEMI].bMultiFile = 0;
  options[OPT_HOSTBINSEMI].dNeg       = AUM;
  sprintf(options[OPT_HOSTBINSEMI].cNeg, "AU");
  fnRead[OPT_HOSTBINSEMI] = &ReadHostBinSemi;

  sprintf(options[OPT_HOSTBININC].cName, "dHostBinInc");
  sprintf(options[OPT_HOSTBININC].cDescr, "inclination of host binary");
  sprintf(options[OPT_HOSTBININC].cDefault, "60.0 deg");
  sprintf(options[OPT_HOSTBININC].cDimension, "angle");
  options[OPT_HOSTBININC].dDefault   = 60.0 * DEGRAD;
  options[OPT_HOSTBININC].iType      = 2;
  options[OPT_HOSTBININC].bMultiFile = 0;
  fnRead[OPT_HOSTBININC]             = &ReadHostBinInc;

  sprintf(options[OPT_HOSTBINARGP].cName, "dHostBinArgP");
  sprintf(options[OPT_HOSTBINARGP].cDescr, "Arg periapse of host binary");
  sprintf(options[OPT_HOSTBINARGP].cDefault, "0.0");
  sprintf(options[OPT_HOSTBINARGP].cDimension, "angle");
  options[OPT_HOSTBINARGP].dDefault   = 0.0;
  options[OPT_HOSTBINARGP].iType      = 2;
  options[OPT_HOSTBINARGP].bMultiFile = 0;
  fnRead[OPT_HOSTBINARGP]             = &ReadHostBinArgP;

  sprintf(options[OPT_HOSTBINLONGA].cName, "dHostBinLongA");
  sprintf(options[OPT_HOSTBINLONGA].cDescr,
          "Long of ascending node of host binary");
  sprintf(options[OPT_HOSTBINLONGA].cDefault, "0.0");
  sprintf(options[OPT_HOSTBINLONGA].cDimension, "angle");
  options[OPT_HOSTBINLONGA].dDefault   = 0.0;
  options[OPT_HOSTBINLONGA].iType      = 2;
  options[OPT_HOSTBINLONGA].bMultiFile = 0;
  fnRead[OPT_HOSTBINLONGA]             = &ReadHostBinLongA;

  sprintf(options[OPT_HOSTBINMASS1].cName, "dHostBinMass1");
  sprintf(options[OPT_HOSTBINMASS1].cDescr, "mass of larger host binary star");
  sprintf(options[OPT_HOSTBINMASS1].cDefault, "1.1 Msun");
  sprintf(options[OPT_HOSTBINMASS1].cDimension, "mass");
  options[OPT_HOSTBINMASS1].dDefault   = 1.1 * MSUN;
  options[OPT_HOSTBINMASS1].iType      = 2;
  options[OPT_HOSTBINMASS1].bMultiFile = 0;
  fnRead[OPT_HOSTBINMASS1]             = &ReadHostBinMass1;
}

void ReadOptionsGalHabit(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system,
                         fnReadOption fnRead[], int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTGALHABIT; iOpt < OPTENDGALHABIT; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}


/******************* Verify GALHABIT ******************/

void InitializeEccXGalHabit(BODY *body, UPDATE *update, int iBody, int iEqn) {
  update[iBody]
        .iaType[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]] = 2;
  update[iBody].padDEccXDtGalHabit[iEqn] =
        &update[iBody].daDerivProc[update[iBody].iEccX]
                                  [update[iBody].iaEccXGalHabit[iEqn]];
  update[iBody]
        .iNumBodies[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]] =
        malloc(update[iBody].iNumBodies[update[iBody].iEccX]
                                       [update[iBody].iaEccXGalHabit[iEqn]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]][0] =
        iBody;
  if (iBody == 1) {
    update[iBody]
          .iaBody[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]][1] =
          2;
  } else if (iBody == 2) {
    update[iBody]
          .iaBody[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]][1] =
          1;
  }
}

void InitializeEccYGalHabit(BODY *body, UPDATE *update, int iBody, int iEqn) {
  update[iBody]
        .iaType[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]] = 2;
  update[iBody].padDEccYDtGalHabit[iEqn] =
        &update[iBody].daDerivProc[update[iBody].iEccY]
                                  [update[iBody].iaEccYGalHabit[iEqn]];
  update[iBody]
        .iNumBodies[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]] =
        malloc(update[iBody].iNumBodies[update[iBody].iEccY]
                                       [update[iBody].iaEccYGalHabit[iEqn]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]][0] =
        iBody;
  if (iBody == 1) {
    update[iBody]
          .iaBody[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]][1] =
          2;
  } else if (iBody == 2) {
    update[iBody]
          .iaBody[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]][1] =
          1;
  }
}


void InitializeEccZGalHabit(BODY *body, UPDATE *update, int iBody, int iEqn) {
  update[iBody]
        .iaType[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]] = 2;
  update[iBody].padDEccZDtGalHabit[iEqn] =
        &update[iBody].daDerivProc[update[iBody].iEccZ]
                                  [update[iBody].iaEccZGalHabit[iEqn]];
  update[iBody]
        .iNumBodies[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]] =
        malloc(update[iBody].iNumBodies[update[iBody].iEccZ]
                                       [update[iBody].iaEccZGalHabit[iEqn]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]][0] =
        iBody;
  if (iBody == 1) {
    update[iBody]
          .iaBody[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]][1] =
          2;
  } else if (iBody == 2) {
    update[iBody]
          .iaBody[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]][1] =
          1;
  }
}

void InitializeAngMXGalHabit(BODY *body, UPDATE *update, int iBody, int iEqn) {
  update[iBody]
        .iaType[update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]] = 2;
  update[iBody].padDAngMXDtGalHabit[iEqn] =
        &update[iBody].daDerivProc[update[iBody].iAngMX]
                                  [update[iBody].iaAngMXGalHabit[iEqn]];
  update[iBody]
        .iNumBodies[update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]] =
        malloc(update[iBody].iNumBodies[update[iBody].iAngMX]
                                       [update[iBody].iaAngMXGalHabit[iEqn]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]][0] =
        iBody;
  if (iBody == 1) {
    update[iBody].iaBody[update[iBody].iAngMX]
                        [update[iBody].iaAngMXGalHabit[iEqn]][1] = 2;
  } else if (iBody == 2) {
    update[iBody].iaBody[update[iBody].iAngMX]
                        [update[iBody].iaAngMXGalHabit[iEqn]][1] = 1;
  }
}

void InitializeAngMYGalHabit(BODY *body, UPDATE *update, int iBody, int iEqn) {
  update[iBody]
        .iaType[update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]] = 2;
  update[iBody].padDAngMYDtGalHabit[iEqn] =
        &update[iBody].daDerivProc[update[iBody].iAngMY]
                                  [update[iBody].iaAngMYGalHabit[iEqn]];
  update[iBody]
        .iNumBodies[update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]] =
        malloc(update[iBody].iNumBodies[update[iBody].iAngMY]
                                       [update[iBody].iaAngMYGalHabit[iEqn]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]][0] =
        iBody;
  if (iBody == 1) {
    update[iBody].iaBody[update[iBody].iAngMY]
                        [update[iBody].iaAngMYGalHabit[iEqn]][1] = 2;
  } else if (iBody == 2) {
    update[iBody].iaBody[update[iBody].iAngMY]
                        [update[iBody].iaAngMYGalHabit[iEqn]][1] = 1;
  }
}

void InitializeAngMZGalHabit(BODY *body, UPDATE *update, int iBody) {
  update[iBody].iaType[update[iBody].iAngMZ][0] = 2;
  update[iBody].padDAngMZDtGalHabit[0] =
        &update[iBody].daDerivProc[update[iBody].iAngMZ]
                                  [update[iBody].iaAngMZGalHabit[0]];
  update[iBody]
        .iNumBodies[update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[0]] = 2;
  update[iBody].iaBody[update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[0]] =
        malloc(update[iBody].iNumBodies[update[iBody].iAngMZ]
                                       [update[iBody].iaAngMZGalHabit[0]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[0]][0] =
        iBody;
  if (iBody == 1) {
    update[iBody]
          .iaBody[update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[0]][1] =
          2;
  } else if (iBody == 2) {
    update[iBody]
          .iaBody[update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[0]][1] =
          1;
  }
}

void VerifyTidesBinary(BODY *body, CONTROL *control, OPTIONS *options,
                       char cFile[], int iBody, int iVerbose) {
  int i;
  if (body[iBody].bGalacTides == 0 && body[iBody].bHostBinary == 0) {
    if (iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: If using GalHabit, must set %s = 1 and/or %s = 1 in "
              "File: %s.\n",
              options[OPT_GALACTIDES].cName, options[OPT_HOSTBINARY].cName,
              cFile);
    }
    exit(EXIT_INPUT);
  }
  if (body[iBody].bHostBinary) {
    if (control->Evolve.iNumBodies != 3) {
      if (iVerbose >= VERBERR) {
        fprintf(
              stderr,
              "ERROR: %s can only be used with exactly 3 bodies in GalHabit\n",
              options[OPT_HOSTBINARY].cName);
      }
      exit(EXIT_INPUT);
    }
    if (body[1].bHostBinary == 1 && body[2].bHostBinary == 0) {
      if (iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: %s must be called for both body 1 and body 2 in "
                "GalHabit\n",
                options[OPT_HOSTBINARY].cName);
      }
      exit(EXIT_INPUT);
    } else if (body[1].bHostBinary == 0 && body[2].bHostBinary == 1) {
      if (iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: %s must be called for both body 1 and body 2 in "
                "GalHabit\n",
                options[OPT_HOSTBINARY].cName);
      }
      exit(EXIT_INPUT);
    }
  }
  if (body[iBody].bGalacTides) {
    body[iBody].dMassInterior = 0;
    for (i = 0; i < iBody; i++) {
      body[iBody].dMassInterior += body[i].dMass;
    }
  }
}

void AssignGalHabitDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                               fnUpdateVariable ***fnUpdate, int iBody) {
  int iEqn;
  if (iBody >= 1) {
    iEqn = 0;
    if (body[iBody].bGalacTides) {
      fnUpdate[iBody][update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]] =
            &fndGalHabitDEccXDtTidal;
      fnUpdate[iBody][update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]] =
            &fndGalHabitDEccYDtTidal;
      fnUpdate[iBody][update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]] =
            &fndGalHabitDEccZDtTidal;
      fnUpdate[iBody][update[iBody].iAngMX]
              [update[iBody].iaAngMXGalHabit[iEqn]] = &fndGalHabitDAngMXDtTidal;
      fnUpdate[iBody][update[iBody].iAngMY]
              [update[iBody].iaAngMYGalHabit[iEqn]] = &fndGalHabitDAngMYDtTidal;
      iEqn++;
    }

    if (body[iBody].bHostBinary) {
      fnUpdate[iBody][update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]] =
            &fndGalHabitDEccXDtBV;
      fnUpdate[iBody][update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]] =
            &fndGalHabitDEccYDtBV;
      fnUpdate[iBody][update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]] =
            &fndGalHabitDEccZDtBV;
      fnUpdate[iBody][update[iBody].iAngMX]
              [update[iBody].iaAngMXGalHabit[iEqn]] = &fndGalHabitDAngMXDtBV;
      fnUpdate[iBody][update[iBody].iAngMY]
              [update[iBody].iaAngMYGalHabit[iEqn]] = &fndGalHabitDAngMYDtBV;
      fnUpdate[iBody][update[iBody].iAngMZ]
              [update[iBody].iaAngMZGalHabit[iEqn]] = &fndGalHabitDAngMZDtBV;
    }
  }
}

void NullGalHabitDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody) {
  int iEqn;
  if (iBody >= 1) {
    iEqn = 0;
    if (body[iBody].bGalacTides) {
      fnUpdate[iBody][update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]] =
            &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]] =
            &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]] =
            &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iAngMX]
              [update[iBody].iaAngMXGalHabit[iEqn]] = &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iAngMY]
              [update[iBody].iaAngMYGalHabit[iEqn]] = &fndUpdateFunctionTiny;
      iEqn++;
    }

    if (body[iBody].bHostBinary) {
      fnUpdate[iBody][update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]] =
            &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]] =
            &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]] =
            &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iAngMX]
              [update[iBody].iaAngMXGalHabit[iEqn]] = &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iAngMY]
              [update[iBody].iaAngMYGalHabit[iEqn]] = &fndUpdateFunctionTiny;
      fnUpdate[iBody][update[iBody].iAngMZ]
              [update[iBody].iaAngMZGalHabit[iEqn]] = &fndUpdateFunctionTiny;
    }
  }
}

void VerifyGalHabit(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, OUTPUT *output, SYSTEM *system,
                    UPDATE *update, int iBody, int iModule) {
  int i, iEqn;
  int n;
  double dSigma, dDMR, dStarR, dGasR, dCurrentAge;
  char cOut[3 * NAMELEN];
  FILE *fOut;

  srand(system->iSeed);

  VerifyTidesBinary(body, control, options, files->Infile[iBody + 1].cIn, iBody,
                    control->Io.iVerbose);

  if (iBody == 1) {
    body[iBody].dMeanA     = 0.0;
    system->dGalaxyAge     = 1e10 * YEARSEC;
    dCurrentAge            = system->dGalaxyAge - control->Evolve.dStopTime;
    system->daPassingStarR = malloc(3 * sizeof(double));
    system->daPassingStarV = malloc(3 * sizeof(double));
    system->daPassingStarImpact = malloc(3 * sizeof(double));
    system->daHostApexVel       = malloc(3 * sizeof(double));
    system->daRelativeVel       = malloc(3 * sizeof(double));
    system->daRelativePos       = malloc(3 * sizeof(double));
    if (system->bRadialMigr) {
      dDMR = fndDarkMatterDensity(system, system->dRForm);
      dStarR =
            (system->dGalacDensity - system->dGasDensity - system->dDMDensity) *
            exp(-(system->dRForm - 8) / system->dStarScaleL);
      dGasR                = fndISMDensity(system, system->dRForm);
      system->dScalingFTot = (dDMR + dStarR + dGasR) /
                             system->dGalacDensity; // scale factor for total
                                                    // density (star+gas+dm)
      system->dScalingFStars =
            dStarR /
            (system->dGalacDensity - system->dGasDensity - system->dDMDensity);
      // system->dScalingFVelDisp = 1; //temporary, just to get solar
      // neighborhood velocity dispersion
      //       CalcMeanVelDispSolar(system);
      system->dScalingFVelDisp =
            exp(-(system->dRForm - 8) /
                (2 * system->dStarScaleL)); // based on Minchev+ 2012
    } else {
      system->dScalingFTot     = 1.0;
      system->dScalingFStars   = 1.0;
      system->dScalingFVelDisp = 1.0;
    }
    if (system->bTimeEvolVelDisp) {
      system->dScalingFVelDisp *= sqrt(dCurrentAge / system->dGalaxyAge);
    }

    system->daGSNumberDens     = malloc(13 * sizeof(double));
    system->daGSNumberDens[0]  = 0.43e-3;
    system->daGSNumberDens[1]  = 3e-3;
    system->daGSNumberDens[2]  = 0.06e-3;
    system->daGSNumberDens[3]  = 0.27e-3;
    system->daGSNumberDens[4]  = 0.44e-3;
    system->daGSNumberDens[5]  = 1.42e-3;
    system->daGSNumberDens[6]  = 0.64e-3;
    system->daGSNumberDens[7]  = 1.52e-3;
    system->daGSNumberDens[8]  = 2.34e-3;
    system->daGSNumberDens[9]  = 2.68e-3;
    system->daGSNumberDens[10] = 5.26e-3;
    system->daGSNumberDens[11] = 8.72e-3;
    system->daGSNumberDens[12] = 41.55e-3;

    system->daGSBinMag     = malloc(13 * sizeof(double));
    system->daGSBinMag[0]  = -7.0;
    system->daGSBinMag[1]  = -6.0;
    system->daGSBinMag[2]  = -5.0;
    system->daGSBinMag[3]  = 0.0;
    system->daGSBinMag[4]  = 2.0;
    system->daGSBinMag[5]  = 3.0;
    system->daGSBinMag[6]  = 3.8;
    system->daGSBinMag[7]  = 4.2;
    system->daGSBinMag[8]  = 5.0;
    system->daGSBinMag[9]  = 6.0;
    system->daGSBinMag[10] = 7.0;
    system->daGSBinMag[11] = 9.0;
    system->daGSBinMag[12] = 13.0;

    system->daEncounterRateMV = malloc(13 * sizeof(double));
    // Russell noted that this may need to updated
    CalcEncounterRate(system);
    system->dDeltaTEnc = 0.0;
    // system->dMinAllowed = 40.0*AUM; //set to 40 au for now...
    system->dLastEncTime  = 0.0;
    system->dCloseEncTime = 0.0;
    system->iNEncounters  = 0;
    NextEncounterTime(system, &control->Evolve, 0);
  }

  if (iBody >= 1) {
    if (system->bOutputEnc) {
      sprintf(cOut, "%s.%s.Encounters", system->cName, body[iBody].cName);
      fOut = fopen(cOut, "w");
      fprintf(fOut,
              "#time encdt tstart MV mass sigma impx impy impz u_rel v_rel "
              "w_rel u_apex v_apex w_apex x_rel y_rel z_rel bbodyx bbodyy "
              "bbodyx a1 e1 i1 argp1 longa1 meana1 af ef if argpf longaf\n");
      fclose(fOut);
    }

    CalcEccVec(body, iBody);
    CalcAngMVec(body, iBody);
    body[iBody].dCosArgP         = cos(body[iBody].dArgP);
    body[iBody].dPeriQ           = body[iBody].dSemi * (1.0 - body[iBody].dEcc);
    body[iBody].daRelativeImpact = malloc(3 * sizeof(double));
    body[iBody].daRelativeVel    = malloc(3 * sizeof(double));


    control->fnPropsAux[iBody][iModule] = &PropsAuxGalHabit;

    iEqn = 0;
    if (body[iBody].bGalacTides) {
      InitializeEccXGalHabit(body, update, iBody, iEqn);

      InitializeEccYGalHabit(body, update, iBody, iEqn);

      InitializeEccZGalHabit(body, update, iBody, iEqn);

      InitializeAngMXGalHabit(body, update, iBody, iEqn);

      InitializeAngMYGalHabit(body, update, iBody, iEqn);

      iEqn++;
    }

    if (body[iBody].bHostBinary) {
      Rot2Bin(body, iBody);
      InitializeEccXGalHabit(body, update, iBody, iEqn);

      InitializeEccYGalHabit(body, update, iBody, iEqn);

      InitializeEccZGalHabit(body, update, iBody, iEqn);

      InitializeAngMXGalHabit(body, update, iBody, iEqn);

      InitializeAngMYGalHabit(body, update, iBody, iEqn);

      InitializeAngMZGalHabit(body, update, iBody);
    }

    control->fnForceBehavior[iBody][iModule]   = &ForceBehaviorGalHabit;
    control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyGalHabit;
  }
}


/***************** GALHABIT Update *****************/
void InitializeUpdateGalHabit(BODY *body, UPDATE *update, int iBody) {
  if (iBody > 0) {
    if (body[iBody].bGalacTides || body[iBody].bHostBinary) {
      if (update[iBody].iNumEccX == 0) {
        update[iBody].iNumVars++;
      }

      if (update[iBody].iNumEccY == 0) {
        update[iBody].iNumVars++;
      }

      if (update[iBody].iNumEccZ == 0) {
        update[iBody].iNumVars++;
      }

      if (update[iBody].iNumAngMX == 0) {
        update[iBody].iNumVars++;
      }

      if (update[iBody].iNumAngMY == 0) {
        update[iBody].iNumVars++;
      }
    }

    if (body[iBody].bGalacTides) {
      update[iBody].iNumEccX += 1;
      update[iBody].iNumEccY += 1;
      update[iBody].iNumEccZ += 1;
      update[iBody].iNumAngMX += 1;
      update[iBody].iNumAngMY += 1;
    }

    if (body[iBody].bHostBinary) {
      update[iBody].iNumEccX += 1;
      update[iBody].iNumEccY += 1;
      update[iBody].iNumEccZ += 1;
      update[iBody].iNumAngMX += 1;
      update[iBody].iNumAngMY += 1;
      if (update[iBody].iNumAngMZ == 0) {
        update[iBody].iNumVars++;
      }
      update[iBody].iNumAngMZ += 1;
    }
  }
}

void FinalizeUpdateEccXGalHabit(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo) {
  if (body[iBody].bHostBinary && body[iBody].bGalacTides) {
    update[iBody].padDEccXDtGalHabit    = malloc(2 * sizeof(double *));
    update[iBody].iaEccXGalHabit        = malloc(2 * sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaEccXGalHabit[0]     = (*iEqn)++;
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaEccXGalHabit[1]     = (*iEqn)++;
  } else {
    update[iBody].padDEccXDtGalHabit    = malloc(1 * sizeof(double *));
    update[iBody].iaEccXGalHabit        = malloc(1 * sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaEccXGalHabit[0]     = (*iEqn)++;
  }
}

void FinalizeUpdateEccYGalHabit(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo) {
  if (body[iBody].bHostBinary && body[iBody].bGalacTides) {
    update[iBody].padDEccYDtGalHabit    = malloc(2 * sizeof(double *));
    update[iBody].iaEccYGalHabit        = malloc(2 * sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaEccYGalHabit[0]     = (*iEqn)++;
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaEccYGalHabit[1]     = (*iEqn)++;
  } else {
    update[iBody].padDEccYDtGalHabit    = malloc(1 * sizeof(double *));
    update[iBody].iaEccYGalHabit        = malloc(1 * sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaEccYGalHabit[0]     = (*iEqn)++;
  }
}

void FinalizeUpdateEccZGalHabit(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo) {
  if (body[iBody].bHostBinary && body[iBody].bGalacTides) {
    update[iBody].padDEccZDtGalHabit    = malloc(2 * sizeof(double *));
    update[iBody].iaEccZGalHabit        = malloc(2 * sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaEccZGalHabit[0]     = (*iEqn)++;
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaEccZGalHabit[1]     = (*iEqn)++;
  } else {
    update[iBody].padDEccZDtGalHabit    = malloc(1 * sizeof(double *));
    update[iBody].iaEccZGalHabit        = malloc(1 * sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaEccZGalHabit[0]     = (*iEqn)++;
  }
}

void FinalizeUpdateAngMXGalHabit(BODY *body, UPDATE *update, int *iEqn,
                                 int iVar, int iBody, int iFoo) {
  if (body[iBody].bHostBinary && body[iBody].bGalacTides) {
    update[iBody].padDAngMXDtGalHabit   = malloc(2 * sizeof(double *));
    update[iBody].iaAngMXGalHabit       = malloc(2 * sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaAngMXGalHabit[0]    = (*iEqn)++;
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaAngMXGalHabit[1]    = (*iEqn)++;
  } else {
    update[iBody].padDAngMXDtGalHabit   = malloc(1 * sizeof(double *));
    update[iBody].iaAngMXGalHabit       = malloc(1 * sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaAngMXGalHabit[0]    = (*iEqn)++;
  }
}

void FinalizeUpdateAngMYGalHabit(BODY *body, UPDATE *update, int *iEqn,
                                 int iVar, int iBody, int iFoo) {
  if (body[iBody].bHostBinary && body[iBody].bGalacTides) {
    update[iBody].padDAngMYDtGalHabit   = malloc(2 * sizeof(double *));
    update[iBody].iaAngMYGalHabit       = malloc(2 * sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaAngMYGalHabit[0]    = (*iEqn)++;
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaAngMYGalHabit[1]    = (*iEqn)++;
  } else {
    update[iBody].padDAngMYDtGalHabit   = malloc(1 * sizeof(double *));
    update[iBody].iaAngMYGalHabit       = malloc(1 * sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaAngMYGalHabit[0]    = (*iEqn)++;
  }
}

void FinalizeUpdateAngMZGalHabit(BODY *body, UPDATE *update, int *iEqn,
                                 int iVar, int iBody, int iFoo) {
  if (body[iBody].bHostBinary) {
    update[iBody].padDAngMZDtGalHabit   = malloc(1 * sizeof(double *));
    update[iBody].iaAngMZGalHabit       = malloc(1 * sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaAngMZGalHabit[0]    = (*iEqn)++;
  }
}


/***************** GALHABIT Halts *****************/

/* disruption? */
int HaltDisrupt(BODY *body, EVOLVE *evolve, HALT *halt, IO *io, UPDATE *update,
                fnUpdateVariable ***fnUpdate, int iBody) {

  if (body[iBody].iDisrupt == 1) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: body %d disrupted", iBody);
      printf(" at %.2e years\n", evolve->dTime / YEARSEC);
    }
    return 1;
  }

  return 0;
}

/* bad impulse? */
int HaltBadImpulse(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                   UPDATE *update, fnUpdateVariable ***fnUpdate, int iBody) {

  if (body[iBody].iBadImpulse == 1) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: body %d moving too fast for impulse approx", iBody);
      printf(" at %.2e years\n", evolve->dTime / YEARSEC);
    }
    return 1;
  }

  return 0;
}

void CountHaltsGalHabit(HALT *halt, int *iNumHalts) {
  (*iNumHalts)++;
  //   (*iNumHalts)++;
}

void VerifyHaltGalHabit(BODY *body, CONTROL *control, OPTIONS *options,
                        int iBody, int *iHalt) {
  control->fnHalt[iBody][(*iHalt)++] = &HaltDisrupt;
  //   control->fnHalt[iBody][(*iHalt)++] = &HaltBadImpulse;
  body[iBody].iDisrupt    = 0;
  body[iBody].iBadImpulse = 0;
}


/************* GALHABIT Outputs ******************/

void WriteBodyPeriQ(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dSemi * (1.0 - body[iBody].dEcc);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

void WriteBodyEccX(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  *dTmp = body[iBody].dEccX;
}

void WriteBodyEccY(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  *dTmp = body[iBody].dEccY;
}

void WriteBodyEccZ(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  *dTmp = body[iBody].dEccZ;
}

void WriteBodyAngMX(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dAngMX;
}

void WriteBodyAngMY(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dAngMY;
}

void WriteBodyAngMZ(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dAngMZ;
}


void WriteNEncounters(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {

  *dTmp = (double)system->iNEncounters;
}

void WriteNBadImpulse(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {

  *dTmp = (double)body[iBody].iBadImpulse;
}

void WriteFVelDisp(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  *dTmp = system->dScalingFVelDisp;
}

void WriteDEccDtGalHTidal(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  double dex, dey, dez;

  if (body[iBody].bGalacTides) {
    dex = *(update[iBody].padDEccXDtGalHabit[0]); // safe to assume iEqn = 0
                                                  // here, since tides
    dey = *(update[iBody].padDEccYDtGalHabit[0]); // come first if at all
    dez = *(update[iBody].padDEccZDtGalHabit[0]);

    *dTmp = (dex * body[iBody].dEccX + dey * body[iBody].dEccY +
             dez * body[iBody].dEccZ) /
            body[iBody].dEcc;
  } else {
    *dTmp = 0;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteDIncDtGalHTidal(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  double dhx, dhy, dhz, qx, qy, qz;

  if (body[iBody].bGalacTides) {
    dhx = *(update[iBody].padDAngMXDtGalHabit[0]); // safe to assume iEqn = 0
                                                   // here, since tides
    dhy = *(update[iBody].padDAngMYDtGalHabit[0]); // come first if at all
    dhz = 0.0;

    qx = (body[iBody].dAngMY * body[iBody].dEccZ -
          body[iBody].dAngMZ * body[iBody].dEccY) /
         body[iBody].dAngM;
    qy = (body[iBody].dAngMZ * body[iBody].dEccX -
          body[iBody].dAngMX * body[iBody].dEccZ) /
         body[iBody].dAngM;
    qz = (body[iBody].dAngMX * body[iBody].dEccY -
          body[iBody].dAngMY * body[iBody].dEccX) /
         body[iBody].dAngM;

    *dTmp = -((sin(body[iBody].dArgP) * body[iBody].dEccX +
               cos(body[iBody].dArgP) * qx) *
                    dhx +
              (sin(body[iBody].dArgP) * body[iBody].dEccY +
               cos(body[iBody].dArgP) * qy) *
                    dhy +
              (sin(body[iBody].dArgP) * body[iBody].dEccZ +
               cos(body[iBody].dArgP) * qz) *
                    dhz) /
            (body[iBody].dEcc * body[iBody].dAngM);
  } else {
    *dTmp = 0;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngRate(units, cUnit);
  }
}

void WriteDLongADtGalHTidal(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  double dhx, dhy, dhz, qx, qy, qz;

  if (body[iBody].bGalacTides) {
    dhx = *(update[iBody].padDAngMXDtGalHabit[0]); // safe to assume iEqn = 0
                                                   // here, since tides
    dhy = *(update[iBody].padDAngMYDtGalHabit[0]); // come first if at all
    dhz = 0.0;

    qx = (body[iBody].dAngMY * body[iBody].dEccZ -
          body[iBody].dAngMZ * body[iBody].dEccY) /
         body[iBody].dAngM;
    qy = (body[iBody].dAngMZ * body[iBody].dEccX -
          body[iBody].dAngMX * body[iBody].dEccZ) /
         body[iBody].dAngM;
    qz = (body[iBody].dAngMX * body[iBody].dEccY -
          body[iBody].dAngMY * body[iBody].dEccX) /
         body[iBody].dAngM;

    *dTmp = ((cos(body[iBody].dArgP) * body[iBody].dEccX -
              sin(body[iBody].dArgP) * qx) *
                   dhx +
             (cos(body[iBody].dArgP) * body[iBody].dEccY -
              sin(body[iBody].dArgP) * qy) *
                   dhy +
             (cos(body[iBody].dArgP) * body[iBody].dEccZ -
              sin(body[iBody].dArgP) * qz) *
                   dhz) /
            (body[iBody].dEcc * body[iBody].dAngM * sin(body[iBody].dInc));
  } else {
    *dTmp = 0;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngRate(units, cUnit);
  }
}

void WriteDArgPDtGalHTidal(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  double dex, dey, dez, dhx, dhy, dhz, qx, qy, qz, dLAdt, dedt;

  if (body[iBody].bGalacTides) {
    dhx = *(update[iBody].padDAngMXDtGalHabit[0]); // safe to assume iEqn = 0
                                                   // here, since tides
    dhy = *(update[iBody].padDAngMYDtGalHabit[0]); // come first if at all
    dhz = 0.0;

    qx = (body[iBody].dAngMY * body[iBody].dEccZ -
          body[iBody].dAngMZ * body[iBody].dEccY) /
         body[iBody].dAngM;
    qy = (body[iBody].dAngMZ * body[iBody].dEccX -
          body[iBody].dAngMX * body[iBody].dEccZ) /
         body[iBody].dAngM;
    qz = (body[iBody].dAngMX * body[iBody].dEccY -
          body[iBody].dAngMY * body[iBody].dEccX) /
         body[iBody].dAngM;

    dLAdt = ((cos(body[iBody].dArgP) * body[iBody].dEccX -
              sin(body[iBody].dArgP) * qx) *
                   dhx +
             (cos(body[iBody].dArgP) * body[iBody].dEccY -
              sin(body[iBody].dArgP) * qy) *
                   dhy +
             (cos(body[iBody].dArgP) * body[iBody].dEccZ -
              sin(body[iBody].dArgP) * qz) *
                   dhz) /
            (body[iBody].dEcc * body[iBody].dAngM * sin(body[iBody].dInc));

    dex = *(update[iBody].padDEccXDtGalHabit[0]); // safe to assume iEqn = 0
                                                  // here, since tides
    dey = *(update[iBody].padDEccYDtGalHabit[0]); // come first if at all
    dez = *(update[iBody].padDEccZDtGalHabit[0]);

    *dTmp = (dex * qx + dey * qy + dez * qz) / pow(body[iBody].dEcc, 2);
    *dTmp -= dLAdt * cos(body[iBody].dInc);
  } else {
    *dTmp = 0;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngRate(units, cUnit);
  }
}

void InitializeOutputGalHabit(OUTPUT *output, fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_PERIQ].cName, "PeriQ");
  sprintf(output[OUT_PERIQ].cDescr, "Pericenter distance");
  sprintf(output[OUT_PERIQ].cNeg, "AU");
  output[OUT_PERIQ].bNeg       = 1;
  output[OUT_PERIQ].dNeg       = 1. / AUM;
  output[OUT_PERIQ].iNum       = 1;
  output[OUT_PERIQ].iModuleBit = GALHABIT;
  fnWrite[OUT_PERIQ]           = &WriteBodyPeriQ;

  sprintf(output[OUT_ECCX].cName, "EccX");
  sprintf(output[OUT_ECCX].cDescr, "X component of ecc vector");
  sprintf(output[OUT_ECCX].cNeg, " ");
  output[OUT_ECCX].bNeg       = 0;
  output[OUT_ECCX].iNum       = 1;
  output[OUT_ECCX].iModuleBit = GALHABIT;
  fnWrite[OUT_ECCX]           = &WriteBodyEccX;

  sprintf(output[OUT_ECCY].cName, "EccY");
  sprintf(output[OUT_ECCY].cDescr, "Y component of ecc vector");
  sprintf(output[OUT_ECCY].cNeg, " ");
  output[OUT_ECCY].bNeg       = 0;
  output[OUT_ECCY].iNum       = 1;
  output[OUT_ECCY].iModuleBit = GALHABIT;
  fnWrite[OUT_ECCY]           = &WriteBodyEccY;

  sprintf(output[OUT_ECCZ].cName, "EccZ");
  sprintf(output[OUT_ECCZ].cDescr, "Z component of ecc vector");
  sprintf(output[OUT_ECCZ].cNeg, " ");
  output[OUT_ECCZ].bNeg       = 0;
  output[OUT_ECCZ].iNum       = 1;
  output[OUT_ECCZ].iModuleBit = GALHABIT;
  fnWrite[OUT_ECCZ]           = &WriteBodyEccZ;

  sprintf(output[OUT_ANGMX].cName, "AngMX");
  sprintf(output[OUT_ANGMX].cDescr, "X component of angular mom vector");
  sprintf(output[OUT_ANGMX].cNeg, " ");
  output[OUT_ANGMX].bNeg       = 0;
  output[OUT_ANGMX].iNum       = 1;
  output[OUT_ANGMX].iModuleBit = GALHABIT;
  fnWrite[OUT_ANGMX]           = &WriteBodyAngMX;

  sprintf(output[OUT_ANGMY].cName, "AngMY");
  sprintf(output[OUT_ANGMY].cDescr, "Y component of angular mom vector");
  sprintf(output[OUT_ANGMY].cNeg, " ");
  output[OUT_ANGMY].bNeg       = 0;
  output[OUT_ANGMY].iNum       = 1;
  output[OUT_ANGMY].iModuleBit = GALHABIT;
  fnWrite[OUT_ANGMY]           = &WriteBodyAngMY;

  sprintf(output[OUT_ANGMZ].cName, "AngMZ");
  sprintf(output[OUT_ANGMZ].cDescr, "Z component of angular mom vector");
  sprintf(output[OUT_ANGMZ].cNeg, " ");
  output[OUT_ANGMZ].bNeg       = 0;
  output[OUT_ANGMZ].iNum       = 1;
  output[OUT_ANGMZ].iModuleBit = GALHABIT;
  fnWrite[OUT_ANGMZ]           = &WriteBodyAngMZ;

  sprintf(output[OUT_NENCOUNTERS].cName, "NEncounters");
  sprintf(output[OUT_NENCOUNTERS].cDescr,
          "cumulative number of stellar encounters");
  sprintf(output[OUT_NENCOUNTERS].cNeg, " ");
  output[OUT_NENCOUNTERS].bNeg       = 0;
  output[OUT_NENCOUNTERS].iNum       = 1;
  output[OUT_NENCOUNTERS].iModuleBit = GALHABIT;
  fnWrite[OUT_NENCOUNTERS]           = &WriteNEncounters;

  sprintf(output[OUT_NBADIMPULSE].cName, "NBadImpulse");
  sprintf(output[OUT_NBADIMPULSE].cDescr,
          "cumulative number of questionable encounters");
  sprintf(output[OUT_NBADIMPULSE].cNeg, " ");
  output[OUT_NBADIMPULSE].bNeg       = 0;
  output[OUT_NBADIMPULSE].iNum       = 1;
  output[OUT_NBADIMPULSE].iModuleBit = GALHABIT;
  fnWrite[OUT_NBADIMPULSE]           = &WriteNBadImpulse;

  sprintf(output[OUT_FVELDISP].cName, "FVelDisp");
  sprintf(output[OUT_FVELDISP].cDescr,
          "Scaling factor for velocity dispersion");
  sprintf(output[OUT_FVELDISP].cNeg, " ");
  output[OUT_FVELDISP].bNeg       = 0;
  output[OUT_FVELDISP].iNum       = 1;
  output[OUT_FVELDISP].iModuleBit = GALHABIT;
  fnWrite[OUT_FVELDISP]           = &WriteFVelDisp;

  sprintf(output[OUT_DECCDTGALHTIDAL].cName, "DEccDtGalHTidal");
  sprintf(output[OUT_DECCDTGALHTIDAL].cDescr,
          "Body's tidal decc/dt in GalHabit");
  sprintf(output[OUT_DECCDTGALHTIDAL].cNeg, "1/year");
  output[OUT_DECCDTGALHTIDAL].bNeg       = 1;
  output[OUT_DECCDTGALHTIDAL].dNeg       = YEARSEC;
  output[OUT_DECCDTGALHTIDAL].iModuleBit = GALHABIT;
  fnWrite[OUT_DECCDTGALHTIDAL]           = &WriteDEccDtGalHTidal;

  sprintf(output[OUT_DINCDTGALHTIDAL].cName, "DIncDtGalHTidal");
  sprintf(output[OUT_DINCDTGALHTIDAL].cDescr,
          "Body's tidal dinc/dt in GalHabit");
  sprintf(output[OUT_DINCDTGALHTIDAL].cNeg, "1/year");
  output[OUT_DINCDTGALHTIDAL].bNeg       = 1;
  output[OUT_DINCDTGALHTIDAL].dNeg       = YEARSEC / DEGRAD;
  output[OUT_DINCDTGALHTIDAL].iModuleBit = GALHABIT;
  fnWrite[OUT_DINCDTGALHTIDAL]           = &WriteDIncDtGalHTidal;

  sprintf(output[OUT_DARGPDTGALHTIDAL].cName, "DArgPDtGalHTidal");
  sprintf(output[OUT_DARGPDTGALHTIDAL].cDescr,
          "Body's tidal dargp/dt in GalHabit");
  sprintf(output[OUT_DARGPDTGALHTIDAL].cNeg, "1/year");
  output[OUT_DARGPDTGALHTIDAL].bNeg       = 1;
  output[OUT_DARGPDTGALHTIDAL].dNeg       = YEARSEC / DEGRAD;
  output[OUT_DARGPDTGALHTIDAL].iModuleBit = GALHABIT;
  fnWrite[OUT_DARGPDTGALHTIDAL]           = &WriteDArgPDtGalHTidal;

  sprintf(output[OUT_DLONGADTGALHTIDAL].cName, "DLongADtGalHTidal");
  sprintf(output[OUT_DLONGADTGALHTIDAL].cDescr,
          "Body's tidal dlonga/dt in GalHabit");
  sprintf(output[OUT_DLONGADTGALHTIDAL].cNeg, "1/year");
  output[OUT_DLONGADTGALHTIDAL].bNeg       = 1;
  output[OUT_DLONGADTGALHTIDAL].dNeg       = YEARSEC / DEGRAD;
  output[OUT_DLONGADTGALHTIDAL].iModuleBit = GALHABIT;
  fnWrite[OUT_DLONGADTGALHTIDAL]           = &WriteDLongADtGalHTidal;
}

/************ GALHABIT Logging Functions **************/

void LogOptionsGalHabit(CONTROL *control, FILE *fp) {

  fprintf(fp, "-------- GALHABIT Options -----\n\n");
}

void LogGalHabit(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                 UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {
  int iOut;

  fprintf(fp, "\n----- GALHABIT PARAMETERS ------\n");
  for (iOut = OUTSTARTGALHABIT; iOut < OUTBODYSTARTGALHABIT; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, 0);
    }
  }
}

void LogBodyGalHabit(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UPDATE *update, fnWriteOutput fnWrite[],
                     FILE *fp, int iBody) {
  int iOut;

  fprintf(fp, "-----GALHABIT PARAMETERS (%s)------\n", body[iBody].cName);
  for (iOut = OUTBODYSTARTGALHABIT; iOut < OUTENDGALHABIT; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
  }
}

/************* MODULE Functions ***********/

void AddModuleGalHabit(CONTROL *control, MODULE *module, int iBody,
                       int iModule) {

  module->iaModule[iBody][iModule] = GALHABIT;

  module->fnCountHalts[iBody][iModule] = &CountHaltsGalHabit;
  module->fnLogBody[iBody][iModule]    = &LogBodyGalHabit;

  module->fnReadOptions[iBody][iModule]       = &ReadOptionsGalHabit;
  module->fnVerify[iBody][iModule]            = &VerifyGalHabit;
  module->fnAssignDerivatives[iBody][iModule] = &AssignGalHabitDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullGalHabitDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltGalHabit;

  module->fnInitializeUpdate[iBody][iModule]    = &InitializeUpdateGalHabit;
  module->fnInitializeOutput[iBody][iModule]    = &InitializeOutputGalHabit;
  module->fnFinalizeUpdateEccX[iBody][iModule]  = &FinalizeUpdateEccXGalHabit;
  module->fnFinalizeUpdateEccY[iBody][iModule]  = &FinalizeUpdateEccYGalHabit;
  module->fnFinalizeUpdateEccZ[iBody][iModule]  = &FinalizeUpdateEccZGalHabit;
  module->fnFinalizeUpdateAngMX[iBody][iModule] = &FinalizeUpdateAngMXGalHabit;
  module->fnFinalizeUpdateAngMY[iBody][iModule] = &FinalizeUpdateAngMYGalHabit;
  module->fnFinalizeUpdateAngMZ[iBody][iModule] = &FinalizeUpdateAngMZGalHabit;
}

/************* GALHABIT Functions ***********/
void PropsAuxGalHabit(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                      int iBody) {
  double sinw, cosw, cosw_alt, sign, dMu, dL;

  /* calculate osculating elements */
  body[iBody].dEcc =
        sqrt(pow(body[iBody].dEccX, 2) + pow(body[iBody].dEccY, 2) +
             pow(body[iBody].dEccZ, 2));
  //   body[iBody].dAngM = sqrt(pow(body[iBody].dAngMX,2)+pow(body[iBody].dAngMY,2)+\
                                                          pow(body[iBody].dAngMZ,2));
  dMu = BIGG * (body[iBody].dMassInterior +
                body[iBody].dMass); // calculate mass coefficient for
                                    // primary/primary+secondary
  dL                = sqrt(dMu * body[iBody].dSemi);
  body[iBody].dAngM = sqrt(1.0 - pow(body[iBody].dEcc, 2));

  body[iBody].dInc   = acos(body[iBody].dAngMZ / body[iBody].dAngM);
  body[iBody].dLongA = atan2(body[iBody].dAngMX, -body[iBody].dAngMY);

  sinw = (-body[iBody].dEccX * body[iBody].dAngMX * body[iBody].dAngMZ -
          body[iBody].dEccY * body[iBody].dAngMY * body[iBody].dAngMZ +
          body[iBody].dEccZ *
                (pow(body[iBody].dAngMX, 2) + pow(body[iBody].dAngMY, 2))) /
         body[iBody].dAngM;
  cosw = (-body[iBody].dEccX * body[iBody].dAngMY +
          body[iBody].dEccY * body[iBody].dAngMX);
  body[iBody].dCosArgP =
        cosw / (body[iBody].dEcc *
                sqrt(pow(body[iBody].dAngMX, 2) + pow(body[iBody].dAngMY, 2)));

  body[iBody].dArgP = atan2(sinw, cosw);

  // if (body[iBody].bHostBinary) {
  //     Rot2Bin(body,iBody);
  //   }

  body[iBody].dLongP = body[iBody].dLongA + body[iBody].dArgP;
  while (body[iBody].dArgP > 2 * PI) {
    body[iBody].dArgP -= 2 * PI;
  }
  while (body[iBody].dArgP < 0) {
    body[iBody].dArgP += 2 * PI;
  }
  while (body[iBody].dLongP > 2 * PI) {
    body[iBody].dLongP -= 2 * PI;
  }
  while (body[iBody].dLongP < 0) {
    body[iBody].dLongP += 2 * PI;
  }
  while (body[iBody].dLongA > 2 * PI) {
    body[iBody].dLongA -= 2 * PI;
  }
  while (body[iBody].dLongA < 0) {
    body[iBody].dLongA += 2 * PI;
  }

  //   body[iBody].dEcc = 1.0 - body[iBody].dPeriQ/body[iBody].dSemi;
}

void ForceBehaviorGalHabit(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                           SYSTEM *system, UPDATE *update,
                           fnUpdateVariable ***fnUpdate, int iBody,
                           int iModule) {
  double dp, dkzi, dVMax, dCurrentAge, dMeanATmp, C;
  double sinw, cosw, cosw_alt, sign;
  char cOut[3 * NAMELEN];
  int idr;
  FILE *fOut;

  dCurrentAge = system->dGalaxyAge - evolve->dStopTime + evolve->dTime;
  if (system->bTimeEvolVelDisp) {
    system->dScalingFVelDisp *=
          sqrt((dCurrentAge + evolve->dCurrentDt) / dCurrentAge);
  }

  if (system->bRadialMigr) {
    if (evolve->dTime >= system->dTMigration) {
      // time of migration passed? move to solar neighborhood
      system->dScalingFTot   = 1.0;
      system->dScalingFStars = 1.0;
      if (system->bTimeEvolVelDisp) {
        system->dScalingFVelDisp = sqrt(dCurrentAge / system->dGalaxyAge);
      } else {
        system->dScalingFVelDisp = 1.0;
      }
      CalcEncounterRate(system);
      system->bRadialMigr = 0; // don't recalculate this stuff again
    }
  }

  /* calculate osculating elements */
  body[iBody].dEcc =
        sqrt(pow(body[iBody].dEccX, 2) + pow(body[iBody].dEccY, 2) +
             pow(body[iBody].dEccZ, 2));
  body[iBody].dAngM =
        sqrt(pow(body[iBody].dAngMX, 2) + pow(body[iBody].dAngMY, 2) +
             pow(body[iBody].dAngMZ, 2));

  body[iBody].dInc   = acos(body[iBody].dAngMZ / body[iBody].dAngM);
  body[iBody].dLongA = atan2(body[iBody].dAngMX, -body[iBody].dAngMY);

  while (body[iBody].dLongA > 2 * PI) {
    body[iBody].dLongA -= 2 * PI;
  }
  while (body[iBody].dLongA < 0) {
    body[iBody].dLongA += 2 * PI;
  }

  // if (body[iBody].dEcc != body[iBody].dEcc) {
  //     printf("stop\n");
  //   }

  sinw = (-body[iBody].dEccX * body[iBody].dAngMX * body[iBody].dAngMZ -
          body[iBody].dEccY * body[iBody].dAngMY * body[iBody].dAngMZ +
          body[iBody].dEccZ *
                (pow(body[iBody].dAngMX, 2) + pow(body[iBody].dAngMY, 2))) /
         body[iBody].dAngM;
  cosw = (-body[iBody].dEccX * body[iBody].dAngMY +
          body[iBody].dEccY * body[iBody].dAngMX);
  body[iBody].dCosArgP =
        cosw / (body[iBody].dEcc *
                sqrt(pow(body[iBody].dAngMX, 2) + pow(body[iBody].dAngMY, 2)));

  body[iBody].dArgP = atan2(sinw, cosw);

  //  if (body[iBody].dArgP > PI/2.) {
  //     printf("stop here\n");
  //   }

  while (body[iBody].dArgP > 2 * PI) {
    body[iBody].dArgP -= 2 * PI;
  }
  while (body[iBody].dArgP < 0) {
    body[iBody].dArgP += 2 * PI;
  }

  body[iBody].dLongP = body[iBody].dLongA + body[iBody].dArgP;

  while (body[iBody].dLongP > 2 * PI) {
    body[iBody].dLongP -= 2 * PI;
  }
  while (body[iBody].dLongP < 0) {
    body[iBody].dLongP += 2 * PI;
  }
  // if (body[iBody].bHostBinary) {
  //     Rot2Bin(body,iBody);
  //   }

  body[iBody].iDisrupt = fniCheck_disrupt(body, system, iBody);

  if (evolve->dTime + evolve->dCurrentDt >= system->dNextEncT) {
    system->dCloseEncTime = evolve->dTime + evolve->dCurrentDt;
    GetStarPosition(system);
    GetStarMass(system);
    system->dRelativeVelRad = 1.0;
    system->dRelativeVelMag = 1.0;
    dkzi                    = 10.0;
    dVMax                   = 1.0;
    while (dkzi > system->dRelativeVelMag / dVMax ||
           system->dRelativeVelRad >= 0) {
      GetStarVelocity(system);
      //  system->daPassingStarV[0] = 17000.0;
      //       system->daPassingStarV[2] = -1000.0;
      GetRelativeVelocity(system);
      dkzi = fndRandom_double();
      dVMax =
            system->dHostApexVelMag + 3.0 * system->dPassingStarSigma * 1000.0;
    }

    /* then move the orbiter, get all distances/velocities, check for disruption
     */
    AdvanceMA(body, system, iBody);
    body[iBody].dSinc = sin(0.5 * body[iBody].dInc);
    // Russell noted we may need to convert to barycentric?
    osc2cart(body, evolve->iNumBodies);

    /* next calculate impact parameter */
    CalcImpactParam(body, system, iBody);

    body[iBody].iBadImpulse += fniCheck_dr(body, evolve, system, iBody);

    /* write out encounter info */
    if (system->bOutputEnc) {
      sprintf(cOut, "%s.%s.Encounters", system->cName, body[iBody].cName);
      fOut = fopen(cOut, "a");
      // fprintf(fOut,"#time MV mass sigma impx impy impz u_s v_s w_s u_r v_r
      // w_r u_sun v_sun w_sun Rx Ry Rz\n");

      fprintd(fOut, evolve->dTime / YEARSEC, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->dEncDT, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->dTStart, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->dPassingStarMagV, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->dPassingStarMass, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->dPassingStarSigma, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->daPassingStarImpact[0], 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->daPassingStarImpact[1], 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->daPassingStarImpact[2], 4, 6);
      fprintf(fOut, " ");
      // fprintd(fOut,system->daPassingStarV[0],4,6);
      //       fprintf(fOut," ");
      //       fprintd(fOut,system->daPassingStarV[1],4,6);
      //       fprintf(fOut," ");
      //       fprintd(fOut,system->daPassingStarV[2],4,6);
      //       fprintf(fOut," ");
      fprintd(fOut, system->daRelativeVel[0], 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->daRelativeVel[1], 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->daRelativeVel[2], 4, 6);
      fprintf(fOut, " ");

      // fprintd(fOut,system->daRelativePos[0],4,6);
      //       fprintf(fOut," ");
      //       fprintd(fOut,system->daRelativePos[1],4,6);
      //       fprintf(fOut," ");
      //       fprintd(fOut,system->daRelativePos[2],4,6);
      //       fprintf(fOut," ");
      fprintd(fOut, system->daHostApexVel[0], 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->daHostApexVel[1], 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->daHostApexVel[2], 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->daPassingStarR[0], 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->daPassingStarR[1], 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, system->daPassingStarR[2], 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, body[iBody].daRelativeImpact[0], 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, body[iBody].daRelativeImpact[1], 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, body[iBody].daRelativeImpact[2], 4, 6);
      fprintf(fOut, " ");
      //     fprintd(fOut,body[iBody].daRelativeVel[0],4,6);
      //       fprintf(fOut," ");
      //       fprintd(fOut,body[iBody].daRelativeVel[1],4,6);
      //       fprintf(fOut," ");
      //       fprintd(fOut,body[iBody].daRelativeVel[2],4,6);
      //       fprintf(fOut," ");

      // fprintd(fOut,body[iBody].daCartPos[0]*AUM,4,6);
      //       fprintf(fOut," ");
      //       fprintd(fOut,body[iBody].daCartPos[1]*AUM,4,6);
      //       fprintf(fOut," ");
      //       fprintd(fOut,body[iBody].daCartPos[2]*AUM,4,6);
      //       fprintf(fOut," ");
      //       fprintd(fOut,body[iBody].daCartVel[0]*AUM/DAYSEC,4,6);
      //       fprintf(fOut," ");
      //       fprintd(fOut,body[iBody].daCartVel[1]*AUM/DAYSEC,4,6);
      //       fprintf(fOut," ");
      //       fprintd(fOut,body[iBody].daCartVel[2]*AUM/DAYSEC,4,6);

      fprintd(fOut, body[iBody].dSemi, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, body[iBody].dEcc, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, body[iBody].dInc / DEGRAD, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, body[iBody].dArgP / DEGRAD, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, body[iBody].dLongA / DEGRAD, 4, 6);
      fprintf(fOut, " ");
      dMeanATmp =
            body[iBody].dMeanA - body[iBody].dMeanMotion * system->dTStart;
      while (dMeanATmp < 0.0) {
        dMeanATmp += 2 * PI;
      }
      fprintd(fOut, dMeanATmp / DEGRAD, 4, 6);
      fprintf(fOut, " ");
      //       fprintf(fOut,"\n");

      fclose(fOut);
    }

    /* apply the impulse */
    ApplyDeltaV(body, system, iBody);
    // Vis viva integral
    C = 0.5 * (pow(body[iBody].daCartVel[0], 2) +
               pow(body[iBody].daCartVel[1], 2) +
               pow(body[iBody].daCartVel[2], 2)) -
        KGAUSS * KGAUSS * (body[iBody].dMassInterior + body[iBody].dMass) /
              MSUN /
              sqrt(pow(body[iBody].daCartPos[0], 2) +
                   pow(body[iBody].daCartPos[1], 2) +
                   pow(body[iBody].daCartPos[2], 2));

    if (C >= 0) {
      body[iBody].iDisrupt = 1;
    }

    cart2osc(body, evolve->iNumBodies);
    if (body[iBody].dEcc >= 1) {
      body[iBody].iDisrupt = 1;
    }
    body[iBody].dInc        = 2 * asin(body[iBody].dSinc);
    body[iBody].dPeriQ      = body[iBody].dSemi * (1.0 - body[iBody].dEcc);
    body[iBody].dMeanMotion = fdSemiToMeanMotion(
          body[iBody].dSemi, body[iBody].dMassInterior + body[iBody].dMass);
    CalcEccVec(body, iBody);
    CalcAngMVec(body, iBody);

    system->dLastEncTime = system->dCloseEncTime;
    system->iNEncounters += 1;
    NextEncounterTime(system, evolve, system->dCloseEncTime);

    if (system->bOutputEnc) {
      fOut = fopen(cOut, "a");
      fprintd(fOut, body[iBody].dSemi, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, body[iBody].dEcc, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, body[iBody].dInc / DEGRAD, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, body[iBody].dArgP / DEGRAD, 4, 6);
      fprintf(fOut, " ");
      fprintd(fOut, body[iBody].dLongA / DEGRAD, 4, 6);
      fprintf(fOut, "\n");

      fclose(fOut);
    }
  }
}

void Rot2Bin(BODY *body, int iBody) {
  double sinw, cosw;

  body[iBody].dEccXTmp = body[iBody].dEccX * cos(body[iBody].dHostBinLongA) +
                         body[iBody].dEccY * sin(body[iBody].dHostBinLongA);
  body[iBody].dEccYTmp = -body[iBody].dEccX * cos(body[iBody].dHostBinInc) *
                               sin(body[iBody].dHostBinLongA) +
                         body[iBody].dEccY * cos(body[iBody].dHostBinInc) *
                               cos(body[iBody].dHostBinLongA) +
                         body[iBody].dEccZ * sin(body[iBody].dHostBinInc);
  body[iBody].dEccZTmp = body[iBody].dEccX * sin(body[iBody].dHostBinInc) *
                               sin(body[iBody].dHostBinLongA) -
                         body[iBody].dEccY * sin(body[iBody].dHostBinInc) *
                               cos(body[iBody].dHostBinLongA) +
                         body[iBody].dEccZ * cos(body[iBody].dHostBinInc);

  body[iBody].dAngMXTmp = body[iBody].dAngMX * cos(body[iBody].dHostBinLongA) +
                          body[iBody].dAngMY * sin(body[iBody].dHostBinLongA);
  body[iBody].dAngMYTmp = -body[iBody].dAngMX * cos(body[iBody].dHostBinInc) *
                                sin(body[iBody].dHostBinLongA) +
                          body[iBody].dAngMY * cos(body[iBody].dHostBinInc) *
                                cos(body[iBody].dHostBinLongA) +
                          body[iBody].dAngMZ * sin(body[iBody].dHostBinInc);
  body[iBody].dAngMZTmp = body[iBody].dAngMX * sin(body[iBody].dHostBinInc) *
                                sin(body[iBody].dHostBinLongA) -
                          body[iBody].dAngMY * sin(body[iBody].dHostBinInc) *
                                cos(body[iBody].dHostBinLongA) +
                          body[iBody].dAngMZ * cos(body[iBody].dHostBinInc);

  /* calculate osculating elements */
  //   body[iBody].dEcc =
  //   sqrt(pow(body[iBody].dEccX,2)+pow(body[iBody].dEccY,2)+pow(body[iBody].dEccZ,2));
  //   body[iBody].dAngM =
   //   sqrt(pow(body[iBody].dAngMX,2)+pow(body[iBody].dAngMY,2)+\
                                                          pow(body[iBody].dAngMZ,2));

  body[iBody].dIncTmp   = acos(body[iBody].dAngMZTmp / body[iBody].dAngM);
  body[iBody].dLongATmp = atan2(body[iBody].dAngMXTmp, -body[iBody].dAngMYTmp);

  sinw = -body[iBody].dEccXTmp * body[iBody].dAngMXTmp * body[iBody].dAngMZTmp -
         body[iBody].dEccYTmp * body[iBody].dAngMYTmp * body[iBody].dAngMZTmp +
         body[iBody].dEccZTmp *
               (pow(body[iBody].dAngMXTmp, 2) + pow(body[iBody].dAngMYTmp, 2));
  cosw = body[iBody].dAngM * (-body[iBody].dEccXTmp * body[iBody].dAngMYTmp +
                              body[iBody].dEccYTmp * body[iBody].dAngMXTmp);
  //   body[iBody].dCosArgP = cosw*body[iBody].dAngM/(body[iBody].dEcc*\
                          sqrt(pow(body[iBody].dAngMX,2)+pow(body[iBody].dAngMY,2)));

  body[iBody].dArgPTmp = atan2(sinw, cosw);


  while (body[iBody].dLongATmp > 2 * PI) {
    body[iBody].dLongATmp -= 2 * PI;
  }
  while (body[iBody].dLongATmp < 0) {
    body[iBody].dLongATmp += 2 * PI;
  }

  while (body[iBody].dArgPTmp > 2 * PI) {
    body[iBody].dArgPTmp -= 2 * PI;
  }
  while (body[iBody].dArgPTmp < 0) {
    body[iBody].dArgPTmp += 2 * PI;
  }
}

void CalcEccVec(BODY *body, int iBody) {
  body[iBody].dEccX =
        body[iBody].dEcc * (cos(body[iBody].dLongA) * cos(body[iBody].dArgP) -
                            sin(body[iBody].dLongA) * sin(body[iBody].dArgP) *
                                  cos(body[iBody].dInc));
  body[iBody].dEccY =
        body[iBody].dEcc * (sin(body[iBody].dLongA) * cos(body[iBody].dArgP) +
                            cos(body[iBody].dLongA) * sin(body[iBody].dArgP) *
                                  cos(body[iBody].dInc));
  body[iBody].dEccZ =
        body[iBody].dEcc * sin(body[iBody].dArgP) * sin(body[iBody].dInc);
}

void CalcAngMVec(BODY *body, int iBody) {
  double dMu, dL;
  dMu = BIGG * (body[iBody].dMassInterior); //+body[iBody].dMass);
  dL  = sqrt(dMu * body[iBody].dSemi);

  body[iBody].dAngM = sqrt((1.0 - pow(body[iBody].dEcc, 2)));
  body[iBody].dAngMX =
        body[iBody].dAngM * (sin(body[iBody].dLongA) * sin(body[iBody].dInc));
  body[iBody].dAngMY =
        -body[iBody].dAngM * (cos(body[iBody].dLongA) * sin(body[iBody].dInc));
  body[iBody].dAngMZ = body[iBody].dAngM * cos(body[iBody].dInc);
}

int fniCheck_disrupt(BODY *body, SYSTEM *system, int iBody) {
  /* rather crude for the time being. checks that pericenter and apocenter are
     ok and e < 1 */
  double apo, peri;

  apo  = body[iBody].dSemi * (1.0 + body[iBody].dEcc);
  peri = body[iBody].dSemi * (1.0 - body[iBody].dEcc);

  if (peri < body[iBody].dMinStellarApproach) {
    return 1;
  } else if (apo > system->dEncounterRad) {
    return 1;
  } else if (body[iBody].dEcc > 1.0) {
    return 1;
  } else {
    return 0;
  }
}

double fndRandom_double() {
  double n;

  n = (double)rand() / RAND_MAX;
  return n;
}

int fniRandom_int(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Chop off all of the values that would cause skew...
    long end = RAND_MAX / n; // truncate skew
    assert(end > 0L);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end) {
      ;
    }

    return r % n;
  }
}

int fniCheck_dr(BODY *body, EVOLVE *evolve, SYSTEM *system, int iBody) {
  double r1x, r1y, r1z, dt, dr, r1, r2, dcross, P;

  //   r1x = body[iBody].daCartPos[0];
  //   r1y = body[iBody].daCartPos[1];
  //   r1z = body[iBody].daCartPos[2];
  //   r1 = sqrt(pow(r1x,2)+pow(r1y,2)+pow(r1z,2));
  //
  //   dcross = (system->daPassingStarR[0]*system->dRelativeVel[0]+\
//             system->daPassingStarR[1]*system->dRelativeVel[1]+\
//             system->daPassingStarR[2]*system->dRelativeVel[2])/ \
//             (system->dRelativeVelMag);
  //   dt = 2*fabs(dcross)/system->dRelativeVelMag;
  dt = system->dEncDT;

  //   body[iBody].dMeanA += body[iBody].dMeanMotion*dt;
  //   osc2cart(body,evolve->iNumBodies);
  //
  //   dr =
  //   sqrt(pow(r1x-body[iBody].daCartPos[0],2)+pow(r1y-body[iBody].daCartPos[1],2)+pow(r1z-body[iBody].daCartPos[2],2));
  //   r2 =
  //   sqrt(pow(body[iBody].daCartPos[0],2)+pow(body[iBody].daCartPos[1],2)+pow(body[iBody].daCartPos[2],2));
  //
  //   //move secondary back to original position
  //   body[iBody].dMeanA -= body[iBody].dMeanMotion*dt;
  //   osc2cart(body,evolve->iNumBodies);

  P = 2 * PI / body[1].dMeanMotion;
  if ((dt / P >= 0.1) || (dt / P >= 0.1)) {
    return 1;
  } else {
    return 0;
  }
}

void GetStarVelocity(SYSTEM *system) {
  /* get passing star velocities from dispersion = dSigma, using Box-Muller
   * method*/
  double u1, u2, z0, z1, dSigma;
  int i;
  VelocityDisp(system);
  dSigma = system->dPassingStarSigma /
           sqrt(3.); // sqrt(3) to account for 3 dimensions

  u1 = fndRandom_double();
  u2 = fndRandom_double();

  z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * PI * u2);
  z1 = sqrt(-2.0 * log(u1)) * sin(2.0 * PI * u2);

  system->daPassingStarV[0] =
        z0 * dSigma * 1000.0; // scale with sigma and convert to m/s
  system->daPassingStarV[1] = z1 * dSigma * 1000.0;

  u1 = fndRandom_double();
  u2 = fndRandom_double();

  z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * PI * u2);

  system->daPassingStarV[2] = z0 * dSigma * 1000.0;
  system->dPassingStarVRad  = 0;
  for (i = 0; i <= 2; i++) {
    system->dPassingStarVRad +=
          system->daPassingStarV[i] * system->daPassingStarR[i];
  }
  system->dPassingStarVRad /= system->dPassingStarRMag;
}

double fndNsMinus6to15(double dMagV) {
  /* distribution of stars with mag: -6 <= MV <= 15 from Heisler, Tremaine &
     Alcock 1987
     !!! may need to be updated !!! */
  double dNs;

  dNs = 0.00358 * pow(10., (0.04 * dMagV)) /
        pow(1.0 + 1.8345 * pow(10., (-0.21 * dMagV)), 3.4) *
        (1.0 - 0.15 * pow(10., (-0.25 * fabs(dMagV - 15.0))));

  return dNs;
}

double fndMag2mass(double dMagV) {
  double dlogMass;

  if (dMagV > 10 && dMagV < 15) {
    dlogMass = 1e-3 * (0.3 + 1.87 * dMagV + 7.614 * pow(dMagV, 2) -
                       1.698 * pow(dMagV, 3) + 0.06096 * pow(dMagV, 4));
  } else if (dMagV >= 15) {
    dlogMass = 1e-3 * (0.3 + 1.87 * 15 + 7.614 * pow(15, 2) -
                       1.698 * pow(15, 3) + 0.06096 * pow(15, 4));
  } else if (dMagV <= 10 && dMagV >= -5.7) {
    dlogMass = 0.477 - 0.135 * dMagV + 1.228e-2 * pow(dMagV, 2) -
               6.734e-4 * pow(dMagV, 3);
  } else if (dMagV < -5.7 && dMagV >= -6.7) {
    dlogMass = log10(0.9); // white dwarfs
  } else if (dMagV < -6.7) {
    dlogMass = log10(4.0); // giants
  } else {
    fprintf(stderr, "ERROR: Unknown object in galhabit.c:fndMag2mass.\n");
    exit(EXIT_INT);
  }

  return pow(10.0, dlogMass);
}

void CalcEncounterRate(SYSTEM *system) {
  double dEncR = 0, dn, dVRel;
  int i;

  for (i = 0; i <= 12; i++) {
    system->dPassingStarMagV = system->daGSBinMag[i];
    VelocityDisp(system);
    VelocityApex(system);
    dn    = system->dScalingFStars * system->daGSNumberDens[i];
    dVRel = sqrt(pow(system->dHostApexVelMag / 1000, 2) +
                 pow(system->dPassingStarSigma, 2));

    system->daEncounterRateMV[i] = PI * pow(system->dEncounterRad, 2) * dVRel *
                                   1000 * dn * pow(AUM * 206265, -3.0) *
                                   YEARSEC * 1e6;
    dEncR += dVRel * 1000 * dn * pow(AUM * 206265, -3.0);
  }

  dEncR *= PI * pow(system->dEncounterRad, 2);

  system->dEncounterRate = dEncR;
}

void GetStarMass(SYSTEM *system) {
  double fs = 0, dTmp = 100, dMagV, dMaxN;
  dMaxN = system->daEncounterRateMV[12];

  while (dTmp > fs) {
    // dMagV = (double)(random_int(20)-4); //draw stellar magnitude
    // (-3<dMagV<15)
    dMagV = (fndRandom_double() * 25.7 - 7.7);
    dTmp  = fndRandom_double() *
           dMaxN; // if dTmp exceeds the frequency, reject dMagV
    fs = fndNearbyStarFrEnc(system, dMagV); // get frequency at dMagV
  }

  system->dPassingStarMagV = dMagV;
  // now get the mass of the star
  system->dPassingStarMass = fndMag2mass(dMagV) * MSUN;
}

void CalcMeanVelDispSolar(SYSTEM *system) {
  double dMeanS = 0, dn, dnTot = 0;
  int i;

  for (i = -4; i <= 15; i++) {
    system->dPassingStarMagV = (double)i;
    VelocityDisp(system);
    dn = fndNearbyStarDist(system->dPassingStarMagV);

    dMeanS += system->dPassingStarSigma * dn;
    dnTot += dn;
  }
  dMeanS /= dnTot;

  system->dVelDispSolar = dMeanS;
}

void VelocityDisp(SYSTEM *system) {
  double dSigma, dMagV;

  dMagV = system->dPassingStarMagV;

  if (dMagV >= -5.7 && dMagV <= -0.2) {
    dSigma = 14.7;
  } else if (dMagV > -0.2 && dMagV <= 1.3) {
    dSigma = 19.7;
  } else if (dMagV > 1.3 && dMagV <= 2.4) {
    dSigma = 23.7;
  } else if (dMagV > 2.4 && dMagV <= 3.6) {
    dSigma = 29.1;
  } else if (dMagV > 3.6 && dMagV <= 4.0) {
    dSigma = 36.2;
  } else if (dMagV > 4.0 && dMagV <= 4.7) {
    dSigma = 37.4;
  } else if (dMagV > 4.7 && dMagV <= 5.5) {
    dSigma = 39.2;
  } else if (dMagV > 5.5 && dMagV <= 6.4) {
    dSigma = 34.1;
  } else if (dMagV > 6.4 && dMagV <= 8.1) {
    dSigma = 43.4;
  } else if (dMagV > 8.1 && dMagV <= 9.9) {
    dSigma = 42.7;
  } else if (dMagV > 9.9) {
    dSigma = 41.8;
  } else if (dMagV < -5.7 && dMagV >= -6.7) {
    dSigma = 63.4; // white dwarfs
  } else if (dMagV < -6.7) {
    dSigma = 41.0; // giants
  } else {
    fprintf(stderr, "ERROR: Unknown object in galhabit.c:VelocityDisp.\n");
    exit(EXIT_INT);
  }

  system->dPassingStarSigma = system->dScalingFVelDisp * dSigma;
}

void VelocityApex(SYSTEM *system) {
  double dVel, dMagV, phi, theta;

  dMagV = system->dPassingStarMagV;

  if (dMagV >= -5.7 && dMagV <= -0.2) {
    dVel = 18.6;
  } else if (dMagV > -0.2 && dMagV <= 1.3) {
    dVel = 17.1;
  } else if (dMagV > 1.3 && dMagV <= 2.4) {
    dVel = 13.7;
  } else if (dMagV > 2.4 && dMagV <= 3.6) {
    dVel = 17.1;
  } else if (dMagV > 3.6 && dMagV <= 4.0) {
    dVel = 17.1;
  } else if (dMagV > 4.0 && dMagV <= 4.7) {
    dVel = 26.4;
  } else if (dMagV > 4.7 && dMagV <= 5.5) {
    dVel = 23.9;
  } else if (dMagV > 5.5 && dMagV <= 6.4) {
    dVel = 19.8;
  } else if (dMagV > 6.4 && dMagV <= 8.1) {
    dVel = 25.0;
  } else if (dMagV > 8.1 && dMagV <= 9.9) {
    dVel = 17.3;
  } else if (dMagV > 9.9) {
    dVel = 23.3;
  } else if (dMagV < -5.7 && dMagV >= -6.7) {
    dVel = 38.3; // white dwarfs
  } else if (dMagV < -6.7) {
    dVel = 21.0; // giants
  } else {
    fprintf(stderr, "ERROR: Unknown object in galhabit.c:VelocityApex.\n");
    exit(EXIT_INT);
  }

  dVel *= 1000.0;
  system->dHostApexVelMag = dVel;

  phi                      = fndRandom_double() * PI;
  theta                    = fndRandom_double() * 2 * PI;
  system->daHostApexVel[0] = dVel * sin(phi) * cos(theta);
  system->daHostApexVel[1] = dVel * sin(phi) * sin(theta);
  system->daHostApexVel[2] = dVel * cos(phi);
}

void GetRelativeVelocity(SYSTEM *system) {
  int i;
  double dVsq;
  VelocityApex(system);

  system->daRelativeVel[0] =
        system->daPassingStarV[0] - system->daHostApexVel[0];
  system->daRelativeVel[1] =
        system->daPassingStarV[1] - system->daHostApexVel[1];
  system->daRelativeVel[2] =
        system->daPassingStarV[2] - system->daHostApexVel[2];
  dVsq                    = 0;
  system->dRelativeVelRad = 0;
  for (i = 0; i <= 2; i++) {
    dVsq += pow(system->daRelativeVel[i], 2);
    system->dRelativeVelRad +=
          system->daRelativeVel[i] * system->daPassingStarR[i];
  }
  system->dRelativeVelMag = sqrt(dVsq);
  system->dRelativeVelRad /= system->dPassingStarRMag;
}

double fndNearbyStarDist(double dMagV) {
  double dNs, w;

  if (dMagV >= -5.7 && dMagV <= -0.2) {
    w   = (5.7 - 0.2);
    dNs = 0.06 / w;
  } else if (dMagV > -0.2 && dMagV <= 1.3) {
    w   = (1.3 + 0.2);
    dNs = 0.27 / w;
  } else if (dMagV > 1.3 && dMagV <= 2.4) {
    w   = 2.4 - 1.3;
    dNs = 0.44 / w;
  } else if (dMagV > 2.4 && dMagV <= 3.6) {
    w   = 3.6 - 2.4;
    dNs = 1.42 / w;
  } else if (dMagV > 3.6 && dMagV <= 4.0) {
    w   = 4. - 3.6;
    dNs = 0.64 / w;
  } else if (dMagV > 4.0 && dMagV <= 4.7) {
    w   = 4.7 - 4.;
    dNs = 1.52 / w;
  } else if (dMagV > 4.7 && dMagV <= 5.5) {
    w   = 5.5 - 4.7;
    dNs = 2.34 / w;
  } else if (dMagV > 5.5 && dMagV <= 6.4) {
    w   = 6.4 - 5.5;
    dNs = 2.68 / w;
  } else if (dMagV > 6.4 && dMagV <= 8.1) {
    w   = 8.1 - 6.4;
    dNs = 5.26 / w;
  } else if (dMagV > 8.1 && dMagV <= 9.9) {
    w   = 9.9 - 8.1;
    dNs = 8.72 / w;
  } else if (dMagV > 9.9) {
    w   = 18 - 9.9;
    dNs = 41.55 / w;
  } else if (dMagV < -5.7 && dMagV >= -6.7) {
    dNs = 3.0; // white dwarfs
  } else if (dMagV < -6.7) {
    dNs = 0.43; // giants
  } else {
    fprintf(stderr, "ERROR: Unknown object in galhabit.c:fndNearbyStarDist.\n");
    exit(EXIT_INT);
  }


  return dNs / 1000; // divide by 1000 to get number/pc^3
}

double fndNearbyStarFrEnc(SYSTEM *system, double dMagV) {
  double dFs, w;

  if (dMagV >= -5.7 && dMagV <= -0.2) {
    dFs = system->daEncounterRateMV[2] / (5.7 - 0.2);
  } else if (dMagV > -0.2 && dMagV <= 1.3) {
    dFs = system->daEncounterRateMV[3] / (1.3 + 0.2);
  } else if (dMagV > 1.3 && dMagV <= 2.4) {
    dFs = system->daEncounterRateMV[4] / (2.4 - 1.3);
  } else if (dMagV > 2.4 && dMagV <= 3.6) {
    dFs = system->daEncounterRateMV[5] / (3.6 - 2.4);
  } else if (dMagV > 3.6 && dMagV <= 4.0) {
    dFs = system->daEncounterRateMV[6] / (4.0 - 3.6);
  } else if (dMagV > 4.0 && dMagV <= 4.7) {
    dFs = system->daEncounterRateMV[7] / (4.7 - 4.);
  } else if (dMagV > 4.7 && dMagV <= 5.5) {
    dFs = system->daEncounterRateMV[8] / (5.5 - 4.7);
  } else if (dMagV > 5.5 && dMagV <= 6.4) {
    dFs = system->daEncounterRateMV[9] / (6.4 - 5.5);
  } else if (dMagV > 6.4 && dMagV <= 8.1) {
    dFs = system->daEncounterRateMV[10] / (8.1 - 6.4);
  } else if (dMagV > 8.1 && dMagV <= 9.9) {
    dFs = system->daEncounterRateMV[11] / (9.9 - 8.1);
  } else if (dMagV > 9.9) {
    dFs = system->daEncounterRateMV[12] / (18.0 - 9.9);
  } else if (dMagV < -5.7 && dMagV >= -6.7) {
    dFs = system->daEncounterRateMV[1]; // white dwarfs
  } else if (dMagV < -6.7) {
    dFs = system->daEncounterRateMV[0]; // giants
  } else {
    fprintf(stderr,
            "ERROR: Unknown object in galhabit.c:fndNearbyStarFrEnc.\n");
    exit(EXIT_INT);
  }

  return dFs;
}


void GetStarPosition(SYSTEM *system) {
  double r = system->dEncounterRad, costheta, phi, sintheta;

  costheta = fndRandom_double() * 2 - 1;
  sintheta = sqrt(fabs(1.0 - pow(costheta, 2)));
  phi      = fndRandom_double() * 2 * PI;

  system->daPassingStarR[0] = r * sintheta * cos(phi);
  system->daPassingStarR[1] = r * sintheta * sin(phi);
  system->daPassingStarR[2] = r * costheta;
  system->dPassingStarRMag  = r;
}

void CalcImpactParam(BODY *body, SYSTEM *system, int iBody) {
  double vsq = 0.0, dtime1 = 0.0, dtime2 = 0.0, imp1, imp2;
  double x, y, z, r;
  double xcom, ycom, zcom;
  int i;
  //
  //   xcom =
  //   body[iBody].dMass*body[iBody].daCartPos[0]/(body[iBody].dMassInterior+body[iBody].dMass)*AUM;
  //   ycom =
  //   body[iBody].dMass*body[iBody].daCartPos[1]/(body[iBody].dMassInterior+body[iBody].dMass)*AUM;
  //   zcom =
  //   body[iBody].dMass*body[iBody].daCartPos[2]/(body[iBody].dMassInterior+body[iBody].dMass)*AUM;
  //
  //   system->daRelativePos[0] = system->daPassingStarR[0] + xcom;
  //   system->daRelativePos[1] = system->daPassingStarR[1] + ycom;
  //   system->daRelativePos[2] = system->daPassingStarR[2] + zcom;

  // calculate impact param for primary
  for (i = 0; i <= 2; i++) {
    vsq += pow(system->daRelativeVel[i], 2);
    dtime1 += -system->daPassingStarR[i] * system->daRelativeVel[i];
  }
  dtime1 /= vsq;
  system->dTStart = fabs(dtime1);

  system->daPassingStarImpact[0] =
        system->daRelativeVel[0] * dtime1 + system->daPassingStarR[0];
  system->daPassingStarImpact[1] =
        system->daRelativeVel[1] * dtime1 + system->daPassingStarR[1];
  system->daPassingStarImpact[2] =
        system->daRelativeVel[2] * dtime1 + system->daPassingStarR[2];
  imp1 = sqrt(pow(system->daPassingStarImpact[0], 2) +
              pow(system->daPassingStarImpact[1], 2) +
              pow(system->daPassingStarImpact[2], 2));

  // now calculate impact parameter for secondary
  dtime2 = 0;
  for (i = 0; i <= 2; i++) {
    //     vsq += pow(system->daRelativeVel[i],2);
    dtime2 += -(system->daPassingStarR[i] - body[iBody].daCartPos[i] * AUM) *
              system->daRelativeVel[i];
  }
  dtime2 /= vsq;

  body[iBody].daRelativeImpact[0] = system->daRelativeVel[0] * dtime2 +
                                    system->daPassingStarR[0] -
                                    body[iBody].daCartPos[0] * AUM;
  body[iBody].daRelativeImpact[1] = system->daRelativeVel[1] * dtime2 +
                                    system->daPassingStarR[1] -
                                    body[iBody].daCartPos[1] * AUM;
  body[iBody].daRelativeImpact[2] = system->daRelativeVel[2] * dtime2 +
                                    system->daPassingStarR[2] -
                                    body[iBody].daCartPos[2] * AUM;
  imp2 = sqrt(pow(body[iBody].daRelativeImpact[0], 2) +
              pow(body[iBody].daRelativeImpact[1], 2) +
              pow(body[iBody].daRelativeImpact[2], 2));

  if (imp1 < imp2) {
    system->dEncDT = 2 * imp1 / system->dRelativeVelMag;
  } else {
    system->dEncDT = 2 * imp2 / system->dRelativeVelMag;
  }
}

void ApplyDeltaV(BODY *body, SYSTEM *system, int iBody) {
  double dRelativeImpactx, dRelativeImpacty, dRelativeImpactz,
        dRelativeImpactrsq;
  double dRelativeVx, dRelativeVy, dRelativeVz, dRelativeV;
  double dPassingStarImpactrsq, dPassingStarV;
  double dDeltaVx, dDeltaVy, dDeltaVz;

  dRelativeImpactx   = body[iBody].daRelativeImpact[0];
  dRelativeImpacty   = body[iBody].daRelativeImpact[1];
  dRelativeImpactz   = body[iBody].daRelativeImpact[2];
  dRelativeImpactrsq = pow(dRelativeImpactx, 2) + pow(dRelativeImpacty, 2) +
                       pow(dRelativeImpactz, 2);

  dRelativeVx =
        system->daRelativeVel[0] - body[iBody].daCartVel[0] * AUM / DAYSEC;
  dRelativeVy =
        system->daRelativeVel[1] - body[iBody].daCartVel[1] * AUM / DAYSEC;
  dRelativeVz =
        system->daRelativeVel[2] - body[iBody].daCartVel[2] * AUM / DAYSEC;

  body[iBody].daRelativeVel[0] = dRelativeVx;
  body[iBody].daRelativeVel[1] = dRelativeVy;
  body[iBody].daRelativeVel[2] = dRelativeVz;

  dRelativeV =
        sqrt(pow(dRelativeVx, 2) + pow(dRelativeVy, 2) + pow(dRelativeVz, 2));

  dPassingStarImpactrsq = pow(system->daPassingStarImpact[0], 2) +
                          pow(system->daPassingStarImpact[1], 2) +
                          pow(system->daPassingStarImpact[2], 2);
  dPassingStarV = system->dRelativeVelMag;

  dDeltaVx = 2 * BIGG * system->dPassingStarMass *
             (1.0 / (dRelativeV * dRelativeImpactrsq) * dRelativeImpactx -
              1.0 / (dPassingStarV * dPassingStarImpactrsq) *
                    system->daPassingStarImpact[0]);
  dDeltaVy = 2 * BIGG * system->dPassingStarMass *
             (1.0 / (dRelativeV * dRelativeImpactrsq) * dRelativeImpacty -
              1.0 / (dPassingStarV * dPassingStarImpactrsq) *
                    system->daPassingStarImpact[1]);
  dDeltaVz = 2 * BIGG * system->dPassingStarMass *
             (1.0 / (dRelativeV * dRelativeImpactrsq) * dRelativeImpactz -
              1.0 / (dPassingStarV * dPassingStarImpactrsq) *
                    system->daPassingStarImpact[2]);

  body[iBody].daCartVel[0] += dDeltaVx / AUM * DAYSEC;
  body[iBody].daCartVel[1] += dDeltaVy / AUM * DAYSEC;
  body[iBody].daCartVel[2] += dDeltaVz / AUM * DAYSEC;
}

void AdvanceMA(BODY *body, SYSTEM *system, int iBody) {
  double dTmp, dTime;

  dTmp  = body[iBody].dMeanA;
  dTime = system->dCloseEncTime - system->dLastEncTime;

  body[iBody].dMeanA = (dTmp + body[iBody].dMeanMotion * dTime);
  while (body[iBody].dMeanA >= 2 * PI) {
    body[iBody].dMeanA -= 2 * PI;
  }
}

void NextEncounterTime(SYSTEM *system, EVOLVE *evolve, double dTime) {
  double dp;

  if (system->bStellarEnc) {
    dp                = fndRandom_double();
    system->dNextEncT = dTime - log(dp) / system->dEncounterRate;
  } else {
    system->dNextEncT = evolve->dStopTime * 1.10;
  }
}

void testrand(SYSTEM *system) {
  char cOut[NAMELEN];
  FILE *fOut;
  double n = 0, m, y = 100;
  int i;

  sprintf(cOut, "randoms");
  fOut = fopen(cOut, "w");

  for (i = 0; i <= 100000; i++) {
    while (y > n) {
      m = (fndRandom_double() * 23.7 - 5.7);
      y = fndRandom_double() * 20;
      n = fndNearbyStarDist(m);
    }

    fprintd(fOut, m, 4, 6);
    fprintf(fOut, "\n");
    y = 100;
    n = 0;
  }
  fclose(fOut);

  // return 0;
}

double fndDarkMatterDensity(SYSTEM *system, double dRad) {
  /* halo model from Kordopatis+ 2015 */
  double dR0 = 8.0, gamma = -2.0, beta = 2.21, q = 0.8, z = 0.025, r0 = 1.09,
         rc = 1000.0;
  double dmR, dm8, drho_R;
  dm8 = sqrt(pow(8, 2) + pow(z / q, 2));
  dmR = sqrt(pow(dRad, 2) + pow(z / q, 2));

  drho_R = pow(dm8 / dmR, gamma) *
           pow((1 + dm8 / r0) / (1 + dmR / r0), beta - gamma) *
           exp(-1 * (pow(dmR, 2) - pow(dm8, 2)) / pow(rc, 2));

  return drho_R * system->dDMDensity; // convert to Msun pc^-3
}

double fndISMDensity(SYSTEM *system, double dRad) {
  double dRd = 2 * system->dStarScaleL, dRh = 4.0, drho_R;

  drho_R = exp(-(dRad - 8) / dRd - dRh / dRad + dRh / 8) * system->dGasDensity;
  return drho_R;
}

//--------------Chain rule derivatives for variable
// transformations-----------------------------
double fndDexDJ(BODY *body, int iBody) {
  return (-sqrt((1.0 - pow(body[iBody].dEcc, 2.))) * body[iBody].dEccX /
                pow(body[iBody].dEcc, 2) * pow(sin(body[iBody].dInc), 2) +
          body[iBody].dEccZ / (sqrt((1.0 - pow(body[iBody].dEcc, 2.)))) *
                sin(body[iBody].dLongA) * cos(body[iBody].dInc) *
                sin(body[iBody].dInc));
}

double fndDexDla(double dEccY) {
  return -dEccY;
}

double fndDexDap(BODY *body, int iBody) {
  // return -dEccZ*cos(dLongA)*sin(dInc) - dEccY*cos(dInc);
  double h = sqrt(pow(body[iBody].dAngMX, 2) + pow(body[iBody].dAngMY, 2) +
                  pow(body[iBody].dAngMZ, 2));

  return body[iBody].dEccZ * body[iBody].dAngMY / h -
         body[iBody].dEccY * body[iBody].dAngMZ / h;
}

double fndDeyDJ(BODY *body, int iBody) {
  return (-sqrt((1.0 - pow(body[iBody].dEcc, 2.))) * body[iBody].dEccY /
                pow(body[iBody].dEcc, 2) * pow(sin(body[iBody].dInc), 2) -
          body[iBody].dEccZ / (sqrt((1.0 - pow(body[iBody].dEcc, 2.)))) *
                cos(body[iBody].dLongA) * cos(body[iBody].dInc) *
                sin(body[iBody].dInc));
}

double fndDeyDla(double dEccX) {
  return dEccX;
}

double fndDeyDap(BODY *body, int iBody) {
  //   return dEccX*cos(dInc) - dEccZ*sin(dLongA)*sin(dInc);
  double h = sqrt(pow(body[iBody].dAngMX, 2) + pow(body[iBody].dAngMY, 2) +
                  pow(body[iBody].dAngMZ, 2));

  return body[iBody].dEccX * body[iBody].dAngMZ / h -
         body[iBody].dEccZ * body[iBody].dAngMX / h;
}

double fndDezDJ(BODY *body, int iBody) {
  return (pow(body[iBody].dEcc, 2) - pow(sin(body[iBody].dInc), 2)) *
         body[iBody].dEccZ /
         (sqrt((1.0 - pow(body[iBody].dEcc, 2))) * pow(body[iBody].dEcc, 2));
}

double fndDezDap(double dArgP, double dEcc, double dInc) {
  return dEcc * cos(dArgP) * sin(dInc);
}

//--------------Galactic
// tides!--------------------------------------------------------------

double fndGalHabitDJDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dRho = system->dScalingFTot * system->dGalacDensity / pow(AUPC, 3),
         dMu, dL;
  dMu = KGAUSS * KGAUSS *
        (body[iaBody[0]].dMassInterior + body[iaBody[0]].dMass) /
        MSUN; // calculate mass coefficient for primary/primary+secondary
  dL = sqrt(dMu * body[iaBody[0]].dSemi / AUM);

  return -5.0 * PI * KGAUSS * KGAUSS * dRho *
         pow(body[iaBody[0]].dSemi / AUM * body[iaBody[0]].dEcc, 2.) *
         sin(2 * body[iaBody[0]].dArgP) / dL / DAYSEC;
}

double fndGalHabitDArgPDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dRho = system->dScalingFTot * system->dGalacDensity / pow(AUPC, 3),
         dMu, dEcc;
  dMu = KGAUSS * KGAUSS *
        (body[iaBody[0]].dMassInterior + body[iaBody[0]].dMass) /
        MSUN; // calculate mass coefficient for primary/primary+secondary
  dEcc = body[iaBody[0]].dEcc; // calculate orbiter's eccentricity

  return 2 * PI * KGAUSS * KGAUSS * dRho *
         sqrt(pow(body[iaBody[0]].dSemi / AUM, 3) /
              (dMu * (1.0 - pow(dEcc, 2)))) *
         (1. - pow(dEcc, 2) -
          5. * (1. - pow(dEcc, 2) - pow(cos(body[iaBody[0]].dInc), 2)) *
                pow(sin(body[iaBody[0]].dArgP), 2.0)) /
         DAYSEC;
}

double fndGalHabitDLongADt(BODY *body, SYSTEM *system, int *iaBody) {
  double dRho = system->dScalingFTot * system->dGalacDensity / pow(AUPC, 3),
         dMu, dEcc, dL, dJ, dJz;
  dMu = KGAUSS * KGAUSS *
        (body[iaBody[0]].dMassInterior + body[iaBody[0]].dMass) /
        MSUN; // calculate mass coefficient for primary/primary+secondary
  dEcc = body[iaBody[0]].dEcc; // calculate orbiter's eccentricity
  dL   = sqrt(dMu * body[iaBody[0]].dSemi / AUM);
  dJ   = dL * sqrt(1.0 - pow(dEcc, 2));
  dJz  = dJ * cos(body[iaBody[0]].dInc);

  return -2. * PI * KGAUSS * KGAUSS * dRho / pow(dMu, 2) * pow(dL / dJ, 2) *
         dJz *
         (pow(dJ, 2) +
          5. * (pow(dL, 2) - pow(dJ, 2)) * pow(sin(body[iaBody[0]].dArgP), 2)) /
         DAYSEC;
}

double fndGalHabitDEccXDtTidal(BODY *body, SYSTEM *system, int *iaBody) {
  return fndDexDJ(body, iaBody[0]) * fndGalHabitDJDt(body, system, iaBody) +
         fndDexDla(body[iaBody[0]].dEccY) *
               fndGalHabitDLongADt(body, system, iaBody) +
         fndDexDap(body, iaBody[0]) * fndGalHabitDArgPDt(body, system, iaBody);
}

double fndGalHabitDEccYDtTidal(BODY *body, SYSTEM *system, int *iaBody) {
  return fndDeyDJ(body, iaBody[0]) * fndGalHabitDJDt(body, system, iaBody) +
         fndDeyDla(body[iaBody[0]].dEccX) *
               fndGalHabitDLongADt(body, system, iaBody) +
         fndDeyDap(body, iaBody[0]) * fndGalHabitDArgPDt(body, system, iaBody);
}

double fndGalHabitDEccZDtTidal(BODY *body, SYSTEM *system, int *iaBody) {
  return fndDezDJ(body, iaBody[0]) * fndGalHabitDJDt(body, system, iaBody) +
         fndDezDap(body[iaBody[0]].dArgP, body[iaBody[0]].dEcc,
                   body[iaBody[0]].dInc) *
               fndGalHabitDArgPDt(body, system, iaBody);
}

double fndGalHabitDAngMXDtTidal(BODY *body, SYSTEM *system, int *iaBody) {
  double dMu, dJ, dL;
  dMu = BIGG * (body[iaBody[0]].dMassInterior); //+body[iaBody[0]].dMass);
  dL  = sqrt(dMu * body[iaBody[0]].dSemi);
  dJ  = sqrt((1.0 - pow(body[iaBody[0]].dEcc, 2)));

  return sin(body[iaBody[0]].dLongA) * sin(body[iaBody[0]].dInc) *
               fndGalHabitDJDt(body, system, iaBody) +
         dJ * sin(body[iaBody[0]].dInc) * cos(body[iaBody[0]].dLongA) *
               fndGalHabitDLongADt(body, system, iaBody);
}

double fndGalHabitDAngMYDtTidal(BODY *body, SYSTEM *system, int *iaBody) {
  double dMu, dJ, dL;
  dMu = BIGG * (body[iaBody[0]].dMassInterior); //+body[iaBody[0]].dMass);
  dL  = sqrt(dMu * body[iaBody[0]].dSemi);
  dJ  = sqrt((1.0 - pow(body[iaBody[0]].dEcc, 2)));

  return -cos(body[iaBody[0]].dLongA) * sin(body[iaBody[0]].dInc) *
               fndGalHabitDJDt(body, system, iaBody) +
         dJ * sin(body[iaBody[0]].dInc) * sin(body[iaBody[0]].dLongA) *
               fndGalHabitDLongADt(body, system, iaBody);
}

double fndGalHabitDAngMZDtTidal(BODY *body, SYSTEM *system, int *iaBody) {
  return 0; // need to get rid of this eventually!!!!
}

//------------Binary quadrupole moment
//----------------------------------------------


//----Fuck it, let's try these equations (Breiter & Vokrouhlicky
// 2015)-----------------------------
double fndQuadC2(BODY *body, int *iaBody) {
  double M1, X0, X1, a1, a2, m2;

  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    // iaBody[0] = inner body
    M1 = body[0].dMass + body[iaBody[0]].dMass;
    X0 = body[0].dMass / M1;
    X1 = body[iaBody[0]].dMass / M1;
    a1 = body[iaBody[0]].dSemi;
    a2 = body[iaBody[1]].dSemi;
    m2 = body[iaBody[1]].dMass;
  } else {
    // iaBody[0] = outer body
    M1 = body[0].dMass + body[iaBody[1]].dMass;
    X0 = body[0].dMass / M1;
    X1 = body[iaBody[1]].dMass / M1;
    a1 = body[iaBody[1]].dSemi;
    a2 = body[iaBody[0]].dSemi;
    m2 = body[iaBody[0]].dMass;
  }

  return 3. / 8 * KGAUSS * KGAUSS * m2 * M1 / pow(MSUN, 2) / (a2 / AUM) * X0 *
         X1 * pow(a1 / a2, 2.0);
}

double fndDQuadDEccXInner(BODY *body, int *iaBody) {
  // iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ13;

  dEta2 = body[iaBody[1]].dAngM;
  dQ13  = body[iaBody[0]].dEccX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dAngMZ;

  return -2 * fndQuadC2(body, iaBody) / pow(dEta2, 5.0) *
         (2 * pow(dEta2, 2.0) * body[iaBody[0]].dEccX -
          5 * dQ13 * body[iaBody[1]].dAngMX);
}

double fndDQuadDEccYInner(BODY *body, int *iaBody) {
  // iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ13;

  dEta2 = body[iaBody[1]].dAngM;
  dQ13  = body[iaBody[0]].dEccX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dAngMZ;

  return -2 * fndQuadC2(body, iaBody) / pow(dEta2, 5.0) *
         (2 * pow(dEta2, 2.0) * body[iaBody[0]].dEccY -
          5 * dQ13 * body[iaBody[1]].dAngMY);
}

double fndDQuadDEccZInner(BODY *body, int *iaBody) {
  // iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ13;

  dEta2 = body[iaBody[1]].dAngM;
  dQ13  = body[iaBody[0]].dEccX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dAngMZ;

  return -2 * fndQuadC2(body, iaBody) / pow(dEta2, 5.0) *
         (2 * pow(dEta2, 2.0) * body[iaBody[0]].dEccZ -
          5 * dQ13 * body[iaBody[1]].dAngMZ);
}

double fndDQuadDAngMXInner(BODY *body, int *iaBody) {
  // iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ33;

  dEta2 = body[iaBody[1]].dAngM;
  dQ33  = body[iaBody[0]].dAngMX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dAngMZ;

  return -2 * fndQuadC2(body, iaBody) / pow(dEta2, 5.0) * dQ33 *
         body[iaBody[1]].dAngMX;
}

double fndDQuadDAngMYInner(BODY *body, int *iaBody) {
  // iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ33;

  dEta2 = body[iaBody[1]].dAngM;
  dQ33  = body[iaBody[0]].dAngMX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dAngMZ;

  return -2 * fndQuadC2(body, iaBody) / pow(dEta2, 5.0) * dQ33 *
         body[iaBody[1]].dAngMY;
}

double fndDQuadDAngMZInner(BODY *body, int *iaBody) {
  // iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ33;

  dEta2 = body[iaBody[1]].dAngM;
  dQ33  = body[iaBody[0]].dAngMX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dAngMZ;

  return -2 * fndQuadC2(body, iaBody) / pow(dEta2, 5.0) * dQ33 *
         body[iaBody[1]].dAngMZ;
}

double fndDQuadDAngMXOuter(BODY *body, int *iaBody) {
  // iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ13, dQ33;

  dEta2 = body[iaBody[0]].dAngM;
  dQ13  = body[iaBody[1]].dEccX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dEccY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dEccZ * body[iaBody[0]].dAngMZ;
  dQ33 = body[iaBody[1]].dAngMX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dAngMY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dAngMZ * body[iaBody[0]].dAngMZ;

  return fndQuadC2(body, iaBody) / pow(dEta2, 5.0) *
         (10.0 * dQ13 * body[iaBody[1]].dEccX -
          2. * dQ33 * body[iaBody[1]].dAngMX -
          (1.0 - 6. * pow(body[iaBody[1]].dEcc, 2) +
           5. / pow(dEta2, 2) * (5 * pow(dQ13, 2) - pow(dQ33, 2))) *
                body[iaBody[0]].dAngMX);
}

double fndDQuadDAngMYOuter(BODY *body, int *iaBody) {
  // iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ13, dQ33;

  dEta2 = body[iaBody[0]].dAngM;
  dQ13  = body[iaBody[1]].dEccX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dEccY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dEccZ * body[iaBody[0]].dAngMZ;
  dQ33 = body[iaBody[1]].dAngMX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dAngMY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dAngMZ * body[iaBody[0]].dAngMZ;

  return fndQuadC2(body, iaBody) / pow(dEta2, 5.0) *
         (10.0 * dQ13 * body[iaBody[1]].dEccY -
          2. * dQ33 * body[iaBody[1]].dAngMY -
          (1.0 - 6. * pow(body[iaBody[1]].dEcc, 2) +
           5. / pow(dEta2, 2) * (5 * pow(dQ13, 2) - pow(dQ33, 2))) *
                body[iaBody[0]].dAngMY);
}

double fndDQuadDAngMZOuter(BODY *body, int *iaBody) {
  // iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ13, dQ33;

  dEta2 = body[iaBody[0]].dAngM;
  dQ13  = body[iaBody[1]].dEccX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dEccY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dEccZ * body[iaBody[0]].dAngMZ;
  dQ33 = body[iaBody[1]].dAngMX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dAngMY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dAngMZ * body[iaBody[0]].dAngMZ;

  return fndQuadC2(body, iaBody) / pow(dEta2, 5.0) *
         (10.0 * dQ13 * body[iaBody[1]].dEccZ -
          2. * dQ33 * body[iaBody[1]].dAngMZ -
          (1.0 - 6. * pow(body[iaBody[1]].dEcc, 2) +
           5. / pow(dEta2, 2) * (5 * pow(dQ13, 2) - pow(dQ33, 2))) *
                body[iaBody[0]].dAngMZ);
}

double fndGalHabitDAngMXDtBV(BODY *body, SYSTEM *system, int *iaBody) {
  double dL, dHdeY, dHdeZ, dHdKY, dHdKZ, dFirstTerm, dSecondTerm;

  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    // iaBody[0] is the inner body
    dL = (body[0].dMass * body[iaBody[0]].dMass) / pow(MSUN, 2) *
         sqrt(KGAUSS * KGAUSS * body[iaBody[0]].dSemi / AUM /
              ((body[0].dMass + body[iaBody[0]].dMass) / MSUN));
    dHdeY = fndDQuadDEccYInner(body, iaBody) + fndDOctDEccYInner(body, iaBody);
    dHdeZ = fndDQuadDEccZInner(body, iaBody) + fndDOctDEccZInner(body, iaBody);
    dHdKY =
          fndDQuadDAngMYInner(body, iaBody) + fndDOctDAngMYInner(body, iaBody);
    dHdKZ =
          fndDQuadDAngMZInner(body, iaBody) + fndDOctDAngMZInner(body, iaBody);
  } else {
    // iaBody[0] is the outer body
    dL = ((body[0].dMass + body[iaBody[1]].dMass) * body[iaBody[0]].dMass) /
         pow(MSUN, 2) *
         sqrt(KGAUSS * KGAUSS * body[iaBody[0]].dSemi / AUM /
              ((body[0].dMass + body[iaBody[0]].dMass + body[iaBody[1]].dMass) /
               MSUN));
    dHdeY = fndDOctDEccYOuter(body, iaBody);
    dHdeZ = fndDOctDEccZOuter(body, iaBody);
    dHdKY =
          fndDQuadDAngMYOuter(body, iaBody) + fndDOctDAngMYOuter(body, iaBody);
    dHdKZ =
          fndDQuadDAngMZOuter(body, iaBody) + fndDOctDAngMZOuter(body, iaBody);
  }
  dFirstTerm  = body[iaBody[0]].dEccY * dHdeZ - body[iaBody[0]].dEccZ * dHdeY;
  dSecondTerm = body[iaBody[0]].dAngMY * dHdKZ - body[iaBody[0]].dAngMZ * dHdKY;

  return -1.0 / dL * (dFirstTerm + dSecondTerm) / DAYSEC;
}

double fndGalHabitDAngMYDtBV(BODY *body, SYSTEM *system, int *iaBody) {
  double dL, dHdeX, dHdeZ, dHdKX, dHdKZ, dFirstTerm, dSecondTerm;

  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    dL = (body[0].dMass * body[iaBody[0]].dMass) / pow(MSUN, 2) *
         sqrt(KGAUSS * KGAUSS * body[iaBody[0]].dSemi / AUM /
              ((body[0].dMass + body[iaBody[0]].dMass) / MSUN));
    // iaBody[0] is the inner body
    dHdeX = fndDQuadDEccXInner(body, iaBody) + fndDOctDEccXInner(body, iaBody);
    dHdeZ = fndDQuadDEccZInner(body, iaBody) + fndDOctDEccZInner(body, iaBody);
    dHdKX =
          fndDQuadDAngMXInner(body, iaBody) + fndDOctDAngMXInner(body, iaBody);
    dHdKZ =
          fndDQuadDAngMZInner(body, iaBody) + fndDOctDAngMZInner(body, iaBody);
  } else {
    // iaBody[0] is the outer body
    dL = ((body[0].dMass + body[iaBody[1]].dMass) * body[iaBody[0]].dMass) /
         pow(MSUN, 2) *
         sqrt(KGAUSS * KGAUSS * body[iaBody[0]].dSemi / AUM /
              ((body[0].dMass + body[iaBody[0]].dMass + body[iaBody[1]].dMass) /
               MSUN));
    dHdeX = fndDOctDEccXOuter(body, iaBody);
    dHdeZ = fndDOctDEccZOuter(body, iaBody);
    dHdKX =
          fndDQuadDAngMXOuter(body, iaBody) + fndDOctDAngMXOuter(body, iaBody);
    dHdKZ =
          fndDQuadDAngMZOuter(body, iaBody) + fndDOctDAngMZOuter(body, iaBody);
  }
  dFirstTerm  = body[iaBody[0]].dEccZ * dHdeX - body[iaBody[0]].dEccX * dHdeZ;
  dSecondTerm = body[iaBody[0]].dAngMZ * dHdKX - body[iaBody[0]].dAngMX * dHdKZ;

  return -1.0 / dL * (dFirstTerm + dSecondTerm) / DAYSEC;
}

double fndGalHabitDAngMZDtBV(BODY *body, SYSTEM *system, int *iaBody) {
  double dL, dHdeX, dHdeY, dHdKX, dHdKY, dFirstTerm, dSecondTerm;

  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    dL = (body[0].dMass * body[iaBody[0]].dMass) / pow(MSUN, 2) *
         sqrt(KGAUSS * KGAUSS * body[iaBody[0]].dSemi / AUM /
              ((body[0].dMass + body[iaBody[0]].dMass) / MSUN));
    // iaBody[0] is the inner body
    dHdeX = fndDQuadDEccXInner(body, iaBody) + fndDOctDEccXInner(body, iaBody);
    dHdeY = fndDQuadDEccYInner(body, iaBody) + fndDOctDEccYInner(body, iaBody);
    dHdKX =
          fndDQuadDAngMXInner(body, iaBody) + fndDOctDAngMXInner(body, iaBody);
    dHdKY =
          fndDQuadDAngMYInner(body, iaBody) + fndDOctDAngMYInner(body, iaBody);
  } else {
    // iaBody[0] is the outer body
    dL = ((body[0].dMass + body[iaBody[1]].dMass) * body[iaBody[0]].dMass) /
         pow(MSUN, 2) *
         sqrt(KGAUSS * KGAUSS * body[iaBody[0]].dSemi / AUM /
              ((body[0].dMass + body[iaBody[0]].dMass + body[iaBody[1]].dMass) /
               MSUN));
    dHdeX = fndDOctDEccXOuter(body, iaBody);
    dHdeY = fndDOctDEccYOuter(body, iaBody);
    dHdKX =
          fndDQuadDAngMXOuter(body, iaBody) + fndDOctDAngMXOuter(body, iaBody);
    dHdKY =
          fndDQuadDAngMYOuter(body, iaBody) + fndDOctDAngMYOuter(body, iaBody);
  }
  dFirstTerm  = body[iaBody[0]].dEccX * dHdeY - body[iaBody[0]].dEccY * dHdeX;
  dSecondTerm = body[iaBody[0]].dAngMX * dHdKY - body[iaBody[0]].dAngMY * dHdKX;

  return -1.0 / dL * (dFirstTerm + dSecondTerm) / DAYSEC;
}

double fndGalHabitDEccXDtBV(BODY *body, SYSTEM *system, int *iaBody) {
  double dL, dHdeY, dHdeZ, dHdKY, dHdKZ, dFirstTerm, dSecondTerm;

  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    // iaBody[0] is the inner body
    dL = (body[0].dMass * body[iaBody[0]].dMass) / pow(MSUN, 2) *
         sqrt(KGAUSS * KGAUSS * body[iaBody[0]].dSemi / AUM /
              ((body[0].dMass + body[iaBody[0]].dMass) / MSUN));
    dHdeY = fndDQuadDEccYInner(body, iaBody) + fndDOctDEccYInner(body, iaBody);
    dHdeZ = fndDQuadDEccZInner(body, iaBody) + fndDOctDEccZInner(body, iaBody);
    dHdKY =
          fndDQuadDAngMYInner(body, iaBody) + fndDOctDAngMYInner(body, iaBody);
    dHdKZ =
          fndDQuadDAngMZInner(body, iaBody) + fndDOctDAngMZInner(body, iaBody);
  } else {
    // iaBody[0] is the outer body
    dL = ((body[0].dMass + body[iaBody[1]].dMass) * body[iaBody[0]].dMass) /
         pow(MSUN, 2) *
         sqrt(KGAUSS * KGAUSS * body[iaBody[0]].dSemi / AUM /
              ((body[0].dMass + body[iaBody[0]].dMass + body[iaBody[1]].dMass) /
               MSUN));
    dHdeY = fndDOctDEccYOuter(body, iaBody);
    dHdeZ = fndDOctDEccZOuter(body, iaBody);
    dHdKY =
          fndDQuadDAngMYOuter(body, iaBody) + fndDOctDAngMYOuter(body, iaBody);
    dHdKZ =
          fndDQuadDAngMZOuter(body, iaBody) + fndDOctDAngMZOuter(body, iaBody);
  }
  dFirstTerm  = body[iaBody[0]].dAngMY * dHdeZ - body[iaBody[0]].dAngMZ * dHdeY;
  dSecondTerm = body[iaBody[0]].dEccY * dHdKZ - body[iaBody[0]].dEccZ * dHdKY;

  return -1.0 / dL * (dFirstTerm + dSecondTerm) / DAYSEC;
}

double fndGalHabitDEccYDtBV(BODY *body, SYSTEM *system, int *iaBody) {
  double dL, dHdeX, dHdeZ, dHdKX, dHdKZ, dFirstTerm, dSecondTerm;

  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    // iaBody[0] is the inner body
    dL = (body[0].dMass * body[iaBody[0]].dMass) / pow(MSUN, 2) *
         sqrt(KGAUSS * KGAUSS * body[iaBody[0]].dSemi / AUM /
              ((body[0].dMass + body[iaBody[0]].dMass) / MSUN));
    dHdeX = fndDQuadDEccXInner(body, iaBody) + fndDOctDEccXInner(body, iaBody);
    dHdeZ = fndDQuadDEccZInner(body, iaBody) + fndDOctDEccZInner(body, iaBody);
    dHdKX =
          fndDQuadDAngMXInner(body, iaBody) + fndDOctDAngMXInner(body, iaBody);
    dHdKZ =
          fndDQuadDAngMZInner(body, iaBody) + fndDOctDAngMZInner(body, iaBody);
  } else {
    // iaBody[0] is the outer body
    dL = ((body[0].dMass + body[iaBody[1]].dMass) * body[iaBody[0]].dMass) /
         pow(MSUN, 2) *
         sqrt(KGAUSS * KGAUSS * body[iaBody[0]].dSemi / AUM /
              ((body[0].dMass + body[iaBody[0]].dMass + body[iaBody[1]].dMass) /
               MSUN));
    dHdeX = fndDOctDEccXOuter(body, iaBody);
    dHdeZ = fndDOctDEccZOuter(body, iaBody);
    dHdKX =
          fndDQuadDAngMXOuter(body, iaBody) + fndDOctDAngMXOuter(body, iaBody);
    dHdKZ =
          fndDQuadDAngMZOuter(body, iaBody) + fndDOctDAngMZOuter(body, iaBody);
  }
  dFirstTerm  = body[iaBody[0]].dAngMZ * dHdeX - body[iaBody[0]].dAngMX * dHdeZ;
  dSecondTerm = body[iaBody[0]].dEccZ * dHdKX - body[iaBody[0]].dEccX * dHdKZ;

  return -1.0 / dL * (dFirstTerm + dSecondTerm) / DAYSEC;
}

double fndGalHabitDEccZDtBV(BODY *body, SYSTEM *system, int *iaBody) {
  double dL, dHdeX, dHdeY, dHdKX, dHdKY, dFirstTerm, dSecondTerm;

  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    // iaBody[0] is the inner body
    dL = (body[0].dMass * body[iaBody[0]].dMass) / pow(MSUN, 2) *
         sqrt(KGAUSS * KGAUSS * body[iaBody[0]].dSemi / AUM /
              ((body[0].dMass + body[iaBody[0]].dMass) / MSUN));
    dHdeX = fndDQuadDEccXInner(body, iaBody) + fndDOctDEccXInner(body, iaBody);
    dHdeY = fndDQuadDEccYInner(body, iaBody) + fndDOctDEccYInner(body, iaBody);
    dHdKX =
          fndDQuadDAngMXInner(body, iaBody) + fndDOctDAngMXInner(body, iaBody);
    dHdKY =
          fndDQuadDAngMYInner(body, iaBody) + fndDOctDAngMYInner(body, iaBody);
  } else {
    // iaBody[0] is the outer body
    dL = ((body[0].dMass + body[iaBody[1]].dMass) * body[iaBody[0]].dMass) /
         pow(MSUN, 2) *
         sqrt(KGAUSS * KGAUSS * body[iaBody[0]].dSemi / AUM /
              ((body[0].dMass + body[iaBody[0]].dMass + body[iaBody[1]].dMass) /
               MSUN));
    dHdeX = fndDOctDEccXOuter(body, iaBody);
    dHdeY = fndDOctDEccYOuter(body, iaBody);
    dHdKX =
          fndDQuadDAngMXOuter(body, iaBody) + fndDOctDAngMXOuter(body, iaBody);
    dHdKY =
          fndDQuadDAngMYOuter(body, iaBody) + fndDOctDAngMYOuter(body, iaBody);
  }
  dFirstTerm  = body[iaBody[0]].dAngMX * dHdeY - body[iaBody[0]].dAngMY * dHdeX;
  dSecondTerm = body[iaBody[0]].dEccX * dHdKY - body[iaBody[0]].dEccY * dHdKX;

  return -1.0 / dL * (dFirstTerm + dSecondTerm) / DAYSEC;
}

double fndOctC3(BODY *body, int *iaBody) {
  double M1, X0, X1, a1, a2;

  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    // iaBody[0] = inner body
    M1 = body[0].dMass + body[iaBody[0]].dMass;
    X0 = body[0].dMass / M1;
    X1 = body[iaBody[0]].dMass / M1;
    a1 = body[iaBody[0]].dSemi;
    a2 = body[iaBody[1]].dSemi;
  } else {
    // iaBody[0] = outer body
    M1 = body[0].dMass + body[iaBody[1]].dMass;
    X0 = body[0].dMass / M1;
    X1 = body[iaBody[1]].dMass / M1;
    a1 = body[iaBody[1]].dSemi;
    a2 = body[iaBody[0]].dSemi;
  }

  return 5. / 8 * fndQuadC2(body, iaBody) * (X0 - X1) * a1 / a2;
}

double fndDOctDEccXInner(BODY *body, int *iaBody) {
  // iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ11, dQ13, dQ31, dQ33;

  dEta2 = body[iaBody[1]].dAngM;
  dQ11  = body[iaBody[0]].dEccX * body[iaBody[1]].dEccX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dEccY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dEccZ;

  dQ13 = body[iaBody[0]].dEccX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dAngMZ;

  dQ31 = body[iaBody[0]].dAngMX * body[iaBody[1]].dEccX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dEccY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dEccZ;

  dQ33 = body[iaBody[0]].dAngMX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dAngMZ;

  return fndOctC3(body, iaBody) / pow(dEta2, 7) *
         (16 * pow(dEta2, 2) * dQ11 * body[iaBody[0]].dEccX +
          10 * (dQ31 * dQ33 - 7 * dQ11 * dQ13) * body[iaBody[1]].dAngMX -
          ((1. - 8 * pow(body[iaBody[0]].dEcc, 2)) * pow(dEta2, 2) +
           35 * pow(dQ13, 2) - 5 * pow(dQ33, 2)) *
                body[iaBody[1]].dEccX);
}

double fndDOctDEccYInner(BODY *body, int *iaBody) {
  // iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ11, dQ13, dQ31, dQ33;

  dEta2 = body[iaBody[1]].dAngM;
  dQ11  = body[iaBody[0]].dEccX * body[iaBody[1]].dEccX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dEccY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dEccZ;

  dQ13 = body[iaBody[0]].dEccX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dAngMZ;

  dQ31 = body[iaBody[0]].dAngMX * body[iaBody[1]].dEccX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dEccY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dEccZ;

  dQ33 = body[iaBody[0]].dAngMX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dAngMZ;

  return fndOctC3(body, iaBody) / pow(dEta2, 7) *
         (16 * pow(dEta2, 2) * dQ11 * body[iaBody[0]].dEccY +
          10 * (dQ31 * dQ33 - 7 * dQ11 * dQ13) * body[iaBody[1]].dAngMY -
          ((1. - 8 * pow(body[iaBody[0]].dEcc, 2)) * pow(dEta2, 2) +
           35 * pow(dQ13, 2) - 5 * pow(dQ33, 2)) *
                body[iaBody[1]].dEccY);
}

double fndDOctDEccZInner(BODY *body, int *iaBody) {
  // iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ11, dQ13, dQ31, dQ33;

  dEta2 = body[iaBody[1]].dAngM;
  dQ11  = body[iaBody[0]].dEccX * body[iaBody[1]].dEccX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dEccY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dEccZ;

  dQ13 = body[iaBody[0]].dEccX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dAngMZ;

  dQ31 = body[iaBody[0]].dAngMX * body[iaBody[1]].dEccX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dEccY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dEccZ;

  dQ33 = body[iaBody[0]].dAngMX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dAngMZ;

  return fndOctC3(body, iaBody) / pow(dEta2, 7) *
         (16 * pow(dEta2, 2) * dQ11 * body[iaBody[0]].dEccZ +
          10 * (dQ31 * dQ33 - 7 * dQ11 * dQ13) * body[iaBody[1]].dAngMZ -
          ((1. - 8 * pow(body[iaBody[0]].dEcc, 2)) * pow(dEta2, 2) +
           35 * pow(dQ13, 2) - 5 * pow(dQ33, 2)) *
                body[iaBody[1]].dEccZ);
}

double fndDOctDAngMXInner(BODY *body, int *iaBody) {
  // iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ11, dQ13, dQ31, dQ33;

  dEta2 = body[iaBody[1]].dAngM;
  dQ11  = body[iaBody[0]].dEccX * body[iaBody[1]].dEccX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dEccY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dEccZ;

  dQ13 = body[iaBody[0]].dEccX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dAngMZ;

  dQ31 = body[iaBody[0]].dAngMX * body[iaBody[1]].dEccX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dEccY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dEccZ;

  dQ33 = body[iaBody[0]].dAngMX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dAngMZ;

  return 10 * fndOctC3(body, iaBody) / pow(dEta2, 7) *
         ((dQ13 * dQ31 + dQ11 * dQ33) * body[iaBody[1]].dAngMX +
          dQ13 * dQ33 * body[iaBody[1]].dEccX);
}

double fndDOctDAngMYInner(BODY *body, int *iaBody) {
  // iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ11, dQ13, dQ31, dQ33;

  dEta2 = body[iaBody[1]].dAngM;
  dQ11  = body[iaBody[0]].dEccX * body[iaBody[1]].dEccX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dEccY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dEccZ;

  dQ13 = body[iaBody[0]].dEccX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dAngMZ;

  dQ31 = body[iaBody[0]].dAngMX * body[iaBody[1]].dEccX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dEccY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dEccZ;

  dQ33 = body[iaBody[0]].dAngMX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dAngMZ;

  return 10 * fndOctC3(body, iaBody) / pow(dEta2, 7) *
         ((dQ13 * dQ31 + dQ11 * dQ33) * body[iaBody[1]].dAngMY +
          dQ13 * dQ33 * body[iaBody[1]].dEccY);
}

double fndDOctDAngMZInner(BODY *body, int *iaBody) {
  // iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ11, dQ13, dQ31, dQ33;

  dEta2 = body[iaBody[1]].dAngM;
  dQ11  = body[iaBody[0]].dEccX * body[iaBody[1]].dEccX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dEccY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dEccZ;

  dQ13 = body[iaBody[0]].dEccX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dEccY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dEccZ * body[iaBody[1]].dAngMZ;

  dQ31 = body[iaBody[0]].dAngMX * body[iaBody[1]].dEccX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dEccY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dEccZ;

  dQ33 = body[iaBody[0]].dAngMX * body[iaBody[1]].dAngMX +
         body[iaBody[0]].dAngMY * body[iaBody[1]].dAngMY +
         body[iaBody[0]].dAngMZ * body[iaBody[1]].dAngMZ;

  return 10 * fndOctC3(body, iaBody) / pow(dEta2, 7) *
         ((dQ13 * dQ31 + dQ11 * dQ33) * body[iaBody[1]].dAngMZ +
          dQ13 * dQ33 * body[iaBody[1]].dEccZ);
}

double fndDOctDEccXOuter(BODY *body, int *iaBody) {
  // iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ13, dQ33;

  dEta2 = body[iaBody[0]].dAngM;

  dQ13 = body[iaBody[1]].dEccX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dEccY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dEccZ * body[iaBody[0]].dAngMZ;

  dQ33 = body[iaBody[1]].dAngMX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dAngMY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dAngMZ * body[iaBody[0]].dAngMZ;

  return fndOctC3(body, iaBody) / pow(dEta2, 7) *
         (10 * dQ13 * dQ33 * body[iaBody[1]].dAngMX -
          ((1. - 8 * pow(body[iaBody[1]].dEcc, 2)) * pow(dEta2, 2) +
           35 * pow(dQ13, 2) - 5 * pow(dQ33, 2)) *
                body[iaBody[1]].dEccX);
}

double fndDOctDEccYOuter(BODY *body, int *iaBody) {
  // iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ13, dQ33;

  dEta2 = body[iaBody[0]].dAngM;

  dQ13 = body[iaBody[1]].dEccX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dEccY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dEccZ * body[iaBody[0]].dAngMZ;

  dQ33 = body[iaBody[1]].dAngMX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dAngMY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dAngMZ * body[iaBody[0]].dAngMZ;

  return fndOctC3(body, iaBody) / pow(dEta2, 7) *
         (10 * dQ13 * dQ33 * body[iaBody[1]].dAngMY -
          ((1. - 8 * pow(body[iaBody[1]].dEcc, 2)) * pow(dEta2, 2) +
           35 * pow(dQ13, 2) - 5 * pow(dQ33, 2)) *
                body[iaBody[1]].dEccY);
}

double fndDOctDEccZOuter(BODY *body, int *iaBody) {
  // iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ13, dQ33;

  dEta2 = body[iaBody[0]].dAngM;

  dQ13 = body[iaBody[1]].dEccX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dEccY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dEccZ * body[iaBody[0]].dAngMZ;

  dQ33 = body[iaBody[1]].dAngMX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dAngMY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dAngMZ * body[iaBody[0]].dAngMZ;

  return fndOctC3(body, iaBody) / pow(dEta2, 7) *
         (10 * dQ13 * dQ33 * body[iaBody[1]].dAngMZ -
          ((1. - 8 * pow(body[iaBody[1]].dEcc, 2)) * pow(dEta2, 2) +
           35 * pow(dQ13, 2) - 5 * pow(dQ33, 2)) *
                body[iaBody[1]].dEccZ);
}

double fndDOctDAngMXOuter(BODY *body, int *iaBody) {
  // iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ11, dQ13, dQ31, dQ33;

  dEta2 = body[iaBody[0]].dAngM;
  dQ11  = body[iaBody[1]].dEccX * body[iaBody[0]].dEccX +
         body[iaBody[1]].dEccY * body[iaBody[0]].dEccY +
         body[iaBody[1]].dEccZ * body[iaBody[0]].dEccZ;

  dQ13 = body[iaBody[1]].dEccX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dEccY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dEccZ * body[iaBody[0]].dAngMZ;

  dQ31 = body[iaBody[1]].dAngMX * body[iaBody[0]].dEccX +
         body[iaBody[1]].dAngMY * body[iaBody[0]].dEccY +
         body[iaBody[1]].dAngMZ * body[iaBody[0]].dEccZ;

  dQ33 = body[iaBody[1]].dAngMX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dAngMY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dAngMZ * body[iaBody[0]].dAngMZ;

  return 5 * fndOctC3(body, iaBody) / pow(dEta2, 7) *
         (2 * (dQ13 * dQ31 + dQ11 * dQ33) * body[iaBody[1]].dAngMX +
          2 * (dQ31 * dQ33 - 7 * dQ11 * dQ13) * body[iaBody[1]].dEccX +
          ((1. - 8 * pow(body[iaBody[1]].dEcc, 2)) * dQ11 +
           7 / pow(dEta2, 2) *
                 (dQ11 * (7 * pow(dQ13, 2) - pow(dQ33, 2)) -
                  2 * dQ13 * dQ31 * dQ33)) *
                body[iaBody[0]].dAngMX);
}

double fndDOctDAngMYOuter(BODY *body, int *iaBody) {
  // iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ11, dQ13, dQ31, dQ33;

  dEta2 = body[iaBody[0]].dAngM;
  dQ11  = body[iaBody[1]].dEccX * body[iaBody[0]].dEccX +
         body[iaBody[1]].dEccY * body[iaBody[0]].dEccY +
         body[iaBody[1]].dEccZ * body[iaBody[0]].dEccZ;

  dQ13 = body[iaBody[1]].dEccX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dEccY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dEccZ * body[iaBody[0]].dAngMZ;

  dQ31 = body[iaBody[1]].dAngMX * body[iaBody[0]].dEccX +
         body[iaBody[1]].dAngMY * body[iaBody[0]].dEccY +
         body[iaBody[1]].dAngMZ * body[iaBody[0]].dEccZ;

  dQ33 = body[iaBody[1]].dAngMX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dAngMY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dAngMZ * body[iaBody[0]].dAngMZ;

  return 5 * fndOctC3(body, iaBody) / pow(dEta2, 7) *
         (2 * (dQ13 * dQ31 + dQ11 * dQ33) * body[iaBody[1]].dAngMY +
          2 * (dQ31 * dQ33 - 7 * dQ11 * dQ13) * body[iaBody[1]].dEccY +
          ((1. - 8 * pow(body[iaBody[1]].dEcc, 2)) * dQ11 +
           7 / pow(dEta2, 2) *
                 (dQ11 * (7 * pow(dQ13, 2) - pow(dQ33, 2)) -
                  2 * dQ13 * dQ31 * dQ33)) *
                body[iaBody[0]].dAngMY);
}

double fndDOctDAngMZOuter(BODY *body, int *iaBody) {
  // iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ11, dQ13, dQ31, dQ33;

  dEta2 = body[iaBody[0]].dAngM;
  dQ11  = body[iaBody[1]].dEccX * body[iaBody[0]].dEccX +
         body[iaBody[1]].dEccY * body[iaBody[0]].dEccY +
         body[iaBody[1]].dEccZ * body[iaBody[0]].dEccZ;

  dQ13 = body[iaBody[1]].dEccX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dEccY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dEccZ * body[iaBody[0]].dAngMZ;

  dQ31 = body[iaBody[1]].dAngMX * body[iaBody[0]].dEccX +
         body[iaBody[1]].dAngMY * body[iaBody[0]].dEccY +
         body[iaBody[1]].dAngMZ * body[iaBody[0]].dEccZ;

  dQ33 = body[iaBody[1]].dAngMX * body[iaBody[0]].dAngMX +
         body[iaBody[1]].dAngMY * body[iaBody[0]].dAngMY +
         body[iaBody[1]].dAngMZ * body[iaBody[0]].dAngMZ;

  return 5 * fndOctC3(body, iaBody) / pow(dEta2, 7) *
         (2 * (dQ13 * dQ31 + dQ11 * dQ33) * body[iaBody[1]].dAngMZ +
          2 * (dQ31 * dQ33 - 7 * dQ11 * dQ13) * body[iaBody[1]].dEccZ +
          ((1. - 8 * pow(body[iaBody[1]].dEcc, 2)) * dQ11 +
           7 / pow(dEta2, 2) *
                 (dQ11 * (7 * pow(dQ13, 2) - pow(dQ33, 2)) -
                  2 * dQ13 * dQ31 * dQ33)) *
                body[iaBody[0]].dAngMZ);
}
