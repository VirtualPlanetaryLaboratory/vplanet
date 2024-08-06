/**
  @file spinbody.c
  @brief Subroutines that control the integration of the N Body simulation
  @author Hayden Smotherman ([smotherh](https://github.com/smotherh/))

  @date Feb 21 2017
*/

#include "vplanet.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void BodyCopySpiNBody(BODY *dest, BODY *src, int iFoo, int iNumBodies,
                      int iBody) {
  int jBody, iGravPerts;
  dest[iBody].dVelX      = src[iBody].dVelX;
  dest[iBody].dVelY      = src[iBody].dVelY;
  dest[iBody].dVelZ      = src[iBody].dVelZ;
  dest[iBody].dPositionX = src[iBody].dPositionX;
  dest[iBody].dPositionY = src[iBody].dPositionY;
  dest[iBody].dPositionZ = src[iBody].dPositionZ;

  dest[iBody].iGravPertsSpiNBody = src[iBody].iGravPertsSpiNBody;

  iGravPerts = src[iBody].iGravPertsSpiNBody;
  for (jBody = 0; jBody < iGravPerts; jBody++) {
    // dest[iBody].dDistance3[jBody]  = src[iBody].dDistance3[jBody];
    dest[iBody].dDistanceX[jBody] = src[iBody].dDistanceX[jBody];
    dest[iBody].dDistanceY[jBody] = src[iBody].dDistanceY[jBody];
    dest[iBody].dDistanceZ[jBody] = src[iBody].dDistanceZ[jBody];
  }
}

void InitializeUpdateTmpBodySpiNBody(BODY *body, CONTROL *control,
                                     UPDATE *update, int iBody) {
  int jBody;
  // This replaces malloc'ing the destination body in BodyCopySpiNBody
  control->Evolve.tmpBody[iBody].dDistance3 =
        malloc(control->Evolve.iNumBodies * sizeof(double));
  control->Evolve.tmpBody[iBody].dDistanceX =
        malloc(control->Evolve.iNumBodies * sizeof(double));
  control->Evolve.tmpBody[iBody].dDistanceY =
        malloc(control->Evolve.iNumBodies * sizeof(double));
  control->Evolve.tmpBody[iBody].dDistanceZ =
        malloc(control->Evolve.iNumBodies * sizeof(double));

  for (jBody = 0; jBody < control->Evolve.iNumBodies; jBody++) {
    // body[iBody].dDistance3[jBody] = 0;
    body[iBody].dDistanceX[jBody] = 0;
    body[iBody].dDistanceY[jBody] = 0;
    body[iBody].dDistanceZ[jBody] = 0;
  }
}

//================================== Read Inputs ===============================
void ReadPositionX(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    // if (dTmp < 0) //No Negative option right now
    //  body[iFile-1].dPositionX =
    //  dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    // else
    body[iFile - 1].dPositionX = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dPositionX = options->dDefault;
  }
}

void ReadPositionY(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dPositionY = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dPositionY = options->dDefault;
  }
}

void ReadPositionZ(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dPositionZ = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dPositionZ = options->dDefault;
  }
}

void ReadVelX(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dVelX = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dVelX = options->dDefault;
  }
}

void ReadVelY(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dVelY = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dVelY = options->dDefault;
  }
}

void ReadVelZ(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].dVelZ = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dVelZ = options->dDefault;
  }
}

void ReadMeanA(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
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

    body[iFile - 1].dMeanA = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dMeanA = options->dDefault;
  }
}

void ReadUseOrbParams(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bUseOrbParams = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    body[iFile - 1].bUseOrbParams = options->dDefault;
  }
}

void InitializeOptionsSpiNBody(OPTIONS *options, fnReadOption fnRead[]) {
  int iOpt, iFile;


  // XXX All of these should be deprecated so that it doesn't matter if the user
  // calls DistOrb or SpiNBody
  fvFormattedString(&options[OPT_POSITIONXSPINBODY].cName, "dPositionXSpiNBody");
  fvFormattedString(&options[OPT_POSITIONXSPINBODY].cDescr, "X position of the body");
  fvFormattedString(&options[OPT_POSITIONXSPINBODY].cDefault, "0");
  fvFormattedString(&options[OPT_POSITIONXSPINBODY].cDimension, "length");
  options[OPT_POSITIONXSPINBODY].dDefault   = 0.0;
  options[OPT_POSITIONXSPINBODY].iType      = 2;
  options[OPT_POSITIONXSPINBODY].bMultiFile = 1;
  fnRead[OPT_POSITIONXSPINBODY]             = &ReadPositionX;

  fvFormattedString(&options[OPT_POSITIONYSPINBODY].cName, "dPositionYSpiNBody");
  fvFormattedString(&options[OPT_POSITIONYSPINBODY].cDescr, "Y position of the body");
  fvFormattedString(&options[OPT_POSITIONYSPINBODY].cDefault, "0");
  fvFormattedString(&options[OPT_POSITIONYSPINBODY].cDimension, "length");
  options[OPT_POSITIONYSPINBODY].dDefault   = 0.0;
  options[OPT_POSITIONYSPINBODY].iType      = 2;
  options[OPT_POSITIONYSPINBODY].bMultiFile = 1;
  fnRead[OPT_POSITIONYSPINBODY]             = &ReadPositionY;

  fvFormattedString(&options[OPT_POSITIONZSPINBODY].cName, "dPositionZSpiNBody");
  fvFormattedString(&options[OPT_POSITIONZSPINBODY].cDescr, "Z position of the body");
  fvFormattedString(&options[OPT_POSITIONZSPINBODY].cDefault, "0");
  fvFormattedString(&options[OPT_POSITIONZSPINBODY].cDimension, "length");
  options[OPT_POSITIONZSPINBODY].dDefault   = 0.0;
  options[OPT_POSITIONZSPINBODY].iType      = 2;
  options[OPT_POSITIONZSPINBODY].bMultiFile = 1;
  fnRead[OPT_POSITIONZSPINBODY]             = &ReadPositionZ;

  fvFormattedString(&options[OPT_VELXSPINBODY].cName, "dVelXSpiNBody");
  fvFormattedString(&options[OPT_VELXSPINBODY].cDescr, "X velocity of the body");
  fvFormattedString(&options[OPT_VELXSPINBODY].cDefault, "0");
  fvFormattedString(&options[OPT_VELXSPINBODY].cDimension, "length/time");
  options[OPT_VELXSPINBODY].dDefault   = 0.0;
  options[OPT_VELXSPINBODY].iType      = 2;
  options[OPT_VELXSPINBODY].bMultiFile = 1;
  fnRead[OPT_VELXSPINBODY]             = &ReadVelX;

  fvFormattedString(&options[OPT_VELYSPINBODY].cName, "dVelYSpiNBody");
  fvFormattedString(&options[OPT_VELYSPINBODY].cDescr, "Y velocity of the body");
  fvFormattedString(&options[OPT_VELYSPINBODY].cDefault, "0");
  fvFormattedString(&options[OPT_VELYSPINBODY].cDimension, "length/time");
  options[OPT_VELYSPINBODY].dDefault   = 0.0;
  options[OPT_VELYSPINBODY].iType      = 2;
  options[OPT_VELYSPINBODY].bMultiFile = 1;
  fnRead[OPT_VELYSPINBODY]             = &ReadVelY;

  fvFormattedString(&options[OPT_VELZSPINBODY].cName, "dVelZSpiNBody");
  fvFormattedString(&options[OPT_VELZSPINBODY].cDescr, "Z velocity of the body");
  fvFormattedString(&options[OPT_VELZSPINBODY].cDefault, "0");
  fvFormattedString(&options[OPT_VELZSPINBODY].cDimension, "length/time");
  options[OPT_VELZSPINBODY].dDefault   = 0.0;
  options[OPT_VELZSPINBODY].iType      = 2;
  options[OPT_VELZSPINBODY].bMultiFile = 1;
  fnRead[OPT_VELZSPINBODY]             = &ReadVelZ;

  fvFormattedString(&options[OPT_MEANA].cName, "dMeanA");
  fvFormattedString(&options[OPT_MEANA].cDescr, "Mean anomaly");
  fvFormattedString(&options[OPT_MEANA].cDefault, "0.0");
  fvFormattedString(&options[OPT_MEANA].cDimension, "angle");
  options[OPT_MEANA].dDefault   = 0.0;
  options[OPT_MEANA].iType      = 2;
  options[OPT_MEANA].bMultiFile = 1;
  fnRead[OPT_MEANA]             = &ReadMeanA;

  fvFormattedString(&options[OPT_USEORBPARAMS].cName, "bUseOrbParams");
  fvFormattedString(&options[OPT_USEORBPARAMS].cDescr,
          "Flag to use orbital parameters as inputs");
  fvFormattedString(&options[OPT_USEORBPARAMS].cDefault, "0");
  options[OPT_USEORBPARAMS].dDefault   = 0;
  options[OPT_USEORBPARAMS].iType      = 0;
  options[OPT_USEORBPARAMS].bMultiFile = 1;
  fnRead[OPT_USEORBPARAMS]             = &ReadUseOrbParams;
}

void ReadOptionsSpiNBody(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system,
                         fnReadOption fnRead[], int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTSPINBODY; iOpt < OPTENDSPINBODY; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}

//============================ End Read Inputs =================================

void InitializeBodySpiNBody(BODY *body, CONTROL *control, UPDATE *update,
                            int iBody, int iModule) {
  int iTmpBody = 0, jBody;
  if (body[iBody].bSpiNBody) {
    body[iBody].iGravPertsSpiNBody =
          control->Evolve
                .iNumBodies; // All bodies except the body itself are perturbers
    // body[iBody].dDistance3 =
    // malloc(control->Evolve.iNumBodies*sizeof(double));
    body[iBody].dDistanceX =
          malloc(control->Evolve.iNumBodies * sizeof(double));
    body[iBody].dDistanceY =
          malloc(control->Evolve.iNumBodies * sizeof(double));
    body[iBody].dDistanceZ =
          malloc(control->Evolve.iNumBodies * sizeof(double));

    for (jBody = 0; jBody < control->Evolve.iNumBodies; jBody++) {
      // body[iBody].dDistance3[jBody] = 0;
      body[iBody].dDistanceX[jBody] = 0;
      body[iBody].dDistanceY[jBody] = 0;
      body[iBody].dDistanceZ[jBody] = 0;
    }

    // If orbital parameters are defined, then we want to set position and
    // velocity based on those
    if (body[iBody].bUseOrbParams) {
      if (iBody == 0) { // Only want to do this once
        for (iTmpBody = 0; iTmpBody < control->Evolve.iNumBodies; iTmpBody++) {
          if (iTmpBody != 0) {
            body[iTmpBody].dMu = BIGG * (body[0].dMass + body[iTmpBody].dMass);
          } else {
            body[iTmpBody].dMu = 0;
          }
          body[iTmpBody].dHCartPos = malloc(3 * sizeof(double));
          body[iTmpBody].dHCartVel = malloc(3 * sizeof(double));
          body[iTmpBody].dBCartPos = malloc(3 * sizeof(double));
          body[iTmpBody].dBCartVel = malloc(3 * sizeof(double));

          // Convert all bodies w/ orbital elements to Heliocentric
          if (body[iTmpBody].bUseOrbParams) {
            body[iTmpBody].dSinc = sin(0.5 * body[iTmpBody].dInc);
            body[iTmpBody].dPinc =
                  body[iTmpBody].dSinc * sin(body[iTmpBody].dLongA);
            body[iTmpBody].dQinc =
                  body[iTmpBody].dSinc * cos(body[iTmpBody].dLongA);
            body[iTmpBody].dHecc =
                  body[iTmpBody].dEcc * sin(body[iTmpBody].dLongP);
            body[iTmpBody].dKecc =
                  body[iTmpBody].dEcc * cos(body[iTmpBody].dLongP);
            OrbElems2Helio(body, iTmpBody);
          }
        }
      }

      Helio2Bary(body, control->Evolve.iNumBodies, iBody);
      body[iBody].dPositionX = body[iBody].dBCartPos[0] * AUM;
      body[iBody].dPositionY = body[iBody].dBCartPos[1] * AUM;
      body[iBody].dPositionZ = body[iBody].dBCartPos[2] * AUM;
      body[iBody].dVelX      = body[iBody].dBCartVel[0] * AUM / DAYSEC;
      body[iBody].dVelY      = body[iBody].dBCartVel[1] * AUM / DAYSEC;
      body[iBody].dVelZ      = body[iBody].dBCartVel[2] * AUM / DAYSEC;
    }
  }
}

void InitializeUpdateSpiNBody(BODY *body, UPDATE *update, int iBody) {
  if (iBody >= 0) {
    if (update[iBody].iNumPositionX == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumPositionX++;

    if (update[iBody].iNumPositionY == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumPositionY++;

    if (update[iBody].iNumPositionZ == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumPositionZ++;

    if (update[iBody].iNumVelX == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumVelX++;

    if (update[iBody].iNumVelY == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumVelY++;

    if (update[iBody].iNumVelZ == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumVelZ++;
  }
}

//======================== Verify Variable Functions ===========================
void VerifyPositionX(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                     int iBody) {

  update[iBody].iaType[update[iBody].iPositionX][0]     = 7;
  update[iBody].iNumBodies[update[iBody].iPositionX][0] = 1;
  update[iBody].iaBody[update[iBody].iPositionX][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iPositionX][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iPositionX][0][0] = iBody;

  update[iBody].pdDPositionX =
        &update[iBody].daDerivProc[update[iBody].iPositionX][0];
}

void VerifyPositionY(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                     int iBody) {

  update[iBody].iaType[update[iBody].iPositionY][0]     = 7;
  update[iBody].iNumBodies[update[iBody].iPositionY][0] = 1;
  update[iBody].iaBody[update[iBody].iPositionY][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iPositionY][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iPositionY][0][0] = iBody;

  update[iBody].pdDPositionY =
        &update[iBody].daDerivProc[update[iBody].iPositionY][0];
}

void VerifyPositionZ(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                     int iBody) {

  update[iBody].iaType[update[iBody].iPositionZ][0]     = 7;
  update[iBody].iNumBodies[update[iBody].iPositionZ][0] = 1;
  update[iBody].iaBody[update[iBody].iPositionZ][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iPositionZ][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iPositionZ][0][0] = iBody;

  update[iBody].pdDPositionZ =
        &update[iBody].daDerivProc[update[iBody].iPositionZ][0];
}

void VerifyVelX(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                int iBody) {

  update[iBody].iaType[update[iBody].iVelX][0]     = 7;
  update[iBody].iNumBodies[update[iBody].iVelX][0] = 1;
  update[iBody].iaBody[update[iBody].iVelX][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iVelX][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iVelX][0][0] = iBody;

  update[iBody].pdDVelX = &update[iBody].daDerivProc[update[iBody].iVelX][0];
}

void VerifyVelY(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                int iBody) {

  update[iBody].iaType[update[iBody].iVelY][0]     = 7;
  update[iBody].iNumBodies[update[iBody].iVelY][0] = 1;
  update[iBody].iaBody[update[iBody].iVelY][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iVelY][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iVelY][0][0] = iBody;

  update[iBody].pdDVelY = &update[iBody].daDerivProc[update[iBody].iVelY][0];
}

void VerifyVelZ(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                int iBody) {

  update[iBody].iaType[update[iBody].iVelZ][0]     = 7;
  update[iBody].iNumBodies[update[iBody].iVelZ][0] = 1;
  update[iBody].iaBody[update[iBody].iVelZ][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iVelZ][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iVelZ][0][0] = iBody;

  update[iBody].pdDVelZ = &update[iBody].daDerivProc[update[iBody].iVelZ][0];
}

void VerifyGM(BODY *body, CONTROL *control) {
  int iBody;

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    body[iBody].dGM = BIGG * body[iBody].dMass;
  }
}

void AssignSpiNBodyDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                               fnUpdateVariable ***fnUpdate, int iBody) {
  fnUpdate[iBody][update[iBody].iPositionX][0] = &fdDPositionXDt;
  fnUpdate[iBody][update[iBody].iPositionY][0] = &fdDPositionYDt;
  fnUpdate[iBody][update[iBody].iPositionZ][0] = &fdDPositionZDt;
  fnUpdate[iBody][update[iBody].iVelX][0]      = &fdDVelXDt;
  fnUpdate[iBody][update[iBody].iVelY][0]      = &fdDVelYDt;
  fnUpdate[iBody][update[iBody].iVelZ][0]      = &fdDVelZDt;
}

void NullSpiNBodyDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody) {
  fnUpdate[iBody][update[iBody].iPositionX][0] = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iPositionY][0] = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iPositionZ][0] = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iVelX][0]      = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iVelY][0]      = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iVelZ][0]      = &fndUpdateFunctionTiny;
}

void VerifySpiNBody(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, OUTPUT *output, SYSTEM *system,
                    UPDATE *update, int iBody, int iModule) {

  VerifyVelX(body, options, update, body[iBody].dAge, iBody);
  VerifyVelY(body, options, update, body[iBody].dAge, iBody);
  VerifyVelZ(body, options, update, body[iBody].dAge, iBody);
  VerifyPositionX(body, options, update, body[iBody].dAge, iBody);
  VerifyPositionY(body, options, update, body[iBody].dAge, iBody);
  VerifyPositionZ(body, options, update, body[iBody].dAge, iBody);

  // VerifyGM(body,control);

  control->fnForceBehavior[iBody][iModule]   = &fnForceBehaviorSpiNBody;
  control->fnPropsAux[iBody][iModule]        = &PropsAuxSpiNBody;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopySpiNBody;
}

//========================== End Verify Functions ==============================

//========================== Start Halt Functions ==============================

/**
  Check the maximum allowed mutual inclination.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number

@return TRUE if one mutual incliantion in a system is larger than
  dHaltMaxMutualInc, FALSE if not
*/
int fbHaltMaxMutualIncSpiNBody(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                               UPDATE *update, fnUpdateVariable ***fnUpdate,
                               int iBody) {

  int jBody;

  // Calculate orbital elements
  for (iBody = 0; iBody < evolve->iNumBodies; iBody++) {
    cart2osc(body, iBody);
  }

  for (iBody = 0; iBody < evolve->iNumBodies; iBody++) {
    for (jBody = iBody + 1; jBody < evolve->iNumBodies; jBody++) {
      // 0 is to check for halt, not progress
      if (fbCheckMaxMutualInc(body, evolve, halt, io, iBody, jBody, 0)) {
        return 1;
      }
    }
  }

  return 0;
}

//========================== Start Halt Functions ==============================

//========================== Coordinate Changes ================================
void OrbElems2Helio(BODY *body, int iBody) {
  double xtmp, ytmp, vxtmp, vytmp;

  if (iBody == 0) {
    body[iBody].dHCartPos[0] = 0;
    body[iBody].dHCartPos[1] = 0;
    body[iBody].dHCartPos[2] = 0;

    body[iBody].dHCartVel[0] = 0;
    body[iBody].dHCartVel[1] = 0;
    body[iBody].dHCartVel[2] = 0;
  } else {
    kepler_eqn(body, iBody);
    xtmp  = fndXinit(body, iBody);
    ytmp  = fndYinit(body, iBody);
    vxtmp = fndVxi(body, iBody);
    vytmp = fndVyi(body, iBody);

    body[iBody].dHCartPos[0] =
          xtmp * (fndXangle1(body, iBody)) + ytmp * (fndXangle2(body, iBody));
    body[iBody].dHCartPos[1] =
          xtmp * (fndYangle1(body, iBody)) + ytmp * (fndYangle2(body, iBody));
    body[iBody].dHCartPos[2] =
          xtmp * (fndZangle1(body, iBody)) + ytmp * (fndZangle2(body, iBody));

    body[iBody].dHCartVel[0] =
          vxtmp * (fndXangle1(body, iBody)) + vytmp * (fndXangle2(body, iBody));
    body[iBody].dHCartVel[1] =
          vxtmp * (fndYangle1(body, iBody)) + vytmp * (fndYangle2(body, iBody));
    body[iBody].dHCartVel[2] =
          vxtmp * (fndZangle1(body, iBody)) + vytmp * (fndZangle2(body, iBody));
  }
}

void Helio2Bary(BODY *body, int iNumBodies, int iBody) {
  int i, iTmpBody;
  double *xcom, *vcom, mtotal;
  xcom   = malloc(3 * sizeof(double));
  vcom   = malloc(3 * sizeof(double));
  mtotal = 0;
  for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
    mtotal += body[iTmpBody].dMass;
  }

  for (i = 0; i < 3; i++) {
    xcom[i] = 0;
    vcom[i] = 0;
    for (iTmpBody = 1; iTmpBody < iNumBodies; iTmpBody++) {
      xcom[i] += (body[iTmpBody].dMass * body[iTmpBody].dHCartPos[i] / mtotal);
      vcom[i] += (body[iTmpBody].dMass * body[iTmpBody].dHCartVel[i] / mtotal);
    }
  }

  for (i = 0; i < 3; i++) {
    body[iBody].dBCartPos[i] = body[iBody].dHCartPos[i] - xcom[i];
    body[iBody].dBCartVel[i] = body[iBody].dHCartVel[i] - vcom[i];
  }

  free(xcom);
  free(vcom);
}

void Bary2Helio(BODY *body, int iBody) {
  // Calculate heliocentric cartesian position
  body[iBody].dHCartPos[0] = body[iBody].dPositionX - body[0].dPositionX;
  body[iBody].dHCartPos[1] = body[iBody].dPositionY - body[0].dPositionY;
  body[iBody].dHCartPos[2] = body[iBody].dPositionZ - body[0].dPositionZ;

  // Calculate heliocentric cartesian velocity
  body[iBody].dHCartVel[0] = body[iBody].dVelX - body[0].dVelX;
  body[iBody].dHCartVel[1] = body[iBody].dVelY - body[0].dVelY;
  body[iBody].dHCartVel[2] = body[iBody].dVelZ - body[0].dVelZ;
}

void Bary2OrbElems(BODY *body, int iBody) {
  double rsq, normr, vsq, mu, *h, hsq, normh, sinwf, coswf, sinfAngle,
        cosfAngle, rdot, sinw, cosw, f, cosE;

  h = malloc(3 * sizeof(double));
  // First convert from Barycentric to heliocentric
  // Helio values are stored in body[iBody].daCartPos and body[iBody].daCartVel
  Bary2Helio(body, iBody);

  if (iBody == 0) {
    body[iBody].dSemi  = 0;
    body[iBody].dEcc   = 0;
    body[iBody].dInc   = 0;
    body[iBody].dLongA = 0;
    body[iBody].dLongP = 0;
    body[iBody].dMeanA = 0;
  } else {

    // Solve for various values that are used repeatedly
    // Solve for h = r X v
    cross(body[iBody].dHCartPos, body[iBody].dHCartVel, h);
    hsq   = h[0] * h[0] + h[1] * h[1] + h[2] * h[2];            // ||h||^2
    normh = sqrt(hsq);                                          // ||h||
    vsq   = body[iBody].dHCartVel[0] * body[iBody].dHCartVel[0] // ||v||^2
          + body[iBody].dHCartVel[1] * body[iBody].dHCartVel[1] +
          body[iBody].dHCartVel[2] * body[iBody].dHCartVel[2];
    rsq = body[iBody].dHCartPos[0] * body[iBody].dHCartPos[0] // ||r||^2
          + body[iBody].dHCartPos[1] * body[iBody].dHCartPos[1] +
          body[iBody].dHCartPos[2] * body[iBody].dHCartPos[2];
    normr = sqrt(rsq); // ||r||
    rdot  = (body[iBody].dHCartPos[0] * body[iBody].dHCartVel[0] +
            body[iBody].dHCartPos[1] * body[iBody].dHCartVel[1] +
            body[iBody].dHCartPos[2] * body[iBody].dHCartVel[2]) /
           normr;
    mu              = BIGG * (body[iBody].dMass + body[0].dMass); // G(M+m)
    body[iBody].dMu = mu;

    // Solve for semi-major axis
    body[iBody].dSemi = 1 / (2 / normr - vsq / mu);

    // Solve for eccentricity
    body[iBody].dEccSq = 1.0 - hsq / (mu * body[iBody].dSemi);
    body[iBody].dEcc   = sqrt(body[iBody].dEccSq);

    // Solve for inclination
    body[iBody].dInc  = acos(h[2] / normh);
    body[iBody].dSinc = 0.5 * sin(body[iBody].dInc); // For DistOrb usage

    // Solve for longitude of ascending node
    body[iBody].dLongA = atan2(h[0], -h[1]);
    if (body[iBody].dLongA < 0) { // Make sure the signs are all right
      body[iBody].dLongA += 2.0 * PI;
    }

    // Solve for w and f
    sinwf = body[iBody].dHCartPos[2] /
            (normr * sin(body[iBody].dInc)); // Error when dInc=0
    coswf = (body[iBody].dHCartPos[0] / normr +
             sin(body[iBody].dLongA) * sinwf * cos(body[iBody].dInc)) /
            cos(body[iBody].dLongA);
    if (body[iBody].dEcc != 0) { // No true anomaly for circular orbits
      sinfAngle = body[iBody].dSemi * (1 - body[iBody].dEccSq) * rdot /
                  (normh * body[iBody].dEcc);
      cosfAngle = (body[iBody].dSemi * (1 - body[iBody].dEccSq) / normr - 1) /
                  body[iBody].dEcc;
      sinw = sinwf * cosfAngle - coswf * sinfAngle;
      cosw = sinwf * sinfAngle + coswf * cosfAngle;

      body[iBody].dArgP  = atan2(sinw, cosw);
      body[iBody].dLongP = atan2(sinw, cosw) + body[iBody].dLongA;

      // Ensure all angles are in [0,2PI)
      if (body[iBody].dLongP >= 2. * PI) {
        body[iBody].dLongP -= 2. * PI;
      } else if (body[iBody].dLongP < 0.0) {
        body[iBody].dLongP += 2. * PI;
      }
      if (body[iBody].dArgP >= 2. * PI) {
        body[iBody].dArgP -= 2. * PI;
      } else if (body[iBody].dArgP < 0.0) {
        body[iBody].dArgP += 2. * PI;
      }

      f = atan2(sinfAngle, cosfAngle);
      if (f >= 2. * PI) {
        f -= 2. * PI;
      } else if (f < 0.0) {
        f += 2. * PI;
      }

      // Calculate Mean anomaly
      cosE = (cosfAngle + body[iBody].dEcc) /
             (1.0 + body[iBody].dEcc * cosfAngle);
      if (fabs(fabs(cosE) - 1) < 1e-12) {
        /* If there is numerical error such that abs(cosE)>1, then use the small
           angle approximation to find E */
        body[iBody].dEccA =
              (1 + (body[iBody].dEccSq - 1) * (cosfAngle * cosfAngle) -
               body[iBody].dEccSq) /
              (1 + body[iBody].dEcc * cosfAngle);
      } else {
        body[iBody].dEccA = acos(cosE);

        // If the planet is in the second half of the orbit, we need -acos(cosE)
        // + 2PI This keeps Mean A in [0, 2PI]
        if (f > PI) {
          body[iBody].dEccA = -body[iBody].dEccA + 2 * PI;
        }
      }

      body[iBody].dMeanA =
            body[iBody].dEccA - body[iBody].dEcc * sin(body[iBody].dEccA);

      if (body[iBody].dMeanA < 0) {
        body[iBody].dMeanA += 2. * PI;
      } else if (body[iBody].dMeanA >= 2. * PI) {
        body[iBody].dMeanA -= 2. * PI;
      }

      body[iBody].dOrbPeriod  = sqrt(4 * PI * PI * body[iBody].dSemi *
                                     body[iBody].dSemi * body[iBody].dSemi / mu);
      body[iBody].dMeanMotion = 2 * PI / body[iBody].dOrbPeriod;
      body[iBody].dPinc       = body[iBody].dSinc * sin(body[iBody].dLongA);
      body[iBody].dQinc       = body[iBody].dSinc * cos(body[iBody].dLongA);
      body[iBody].dHecc       = body[iBody].dEcc * sin(body[iBody].dLongP);
      body[iBody].dKecc       = body[iBody].dEcc * cos(body[iBody].dLongP);
    }
  }
  free(h);
}
// Functions below are EXACTLY the same as in distorb.c, but needed in SpiNBody
// Should be relocated to system.c?

//
// void kepler_eqn(BODY *body, int iBody) { //Same as in distorb.c
//   double di1, di2, di3, fi, fi1, fi2, fi3;
//   if (body[iBody].dMeanA == 0) {
//     body[iBody].dEccA = 0;
//   } else {
//     body[iBody].dEccA = body[iBody].dMeanA +
//     signf(sin(body[iBody].dMeanA))*0.85*body[iBody].dEcc; di3 = 1.0;
//
//     while (di3 > 1e-15) {
//       fi = body[iBody].dEccA - body[iBody].dEcc*sin(body[iBody].dEccA) -
//       body[iBody].dMeanA; fi1 = 1.0 -
//       body[iBody].dEcc*cos(body[iBody].dEccA); fi2 =
//       body[iBody].dEcc*sin(body[iBody].dEccA); fi3 =
//       body[iBody].dEcc*cos(body[iBody].dEccA); di1 = -fi/fi1; di2 =
//       -fi/(fi1+0.5*di1*fi2); di3 =
//       -fi/(fi1+0.5*di2*fi2+1./6.*pow(di2,2)*fi3); body[iBody].dEccA += di3;
//     }
//   }
// }
//
// double signf(double value) { //Same as in distorb.c
//   if (value > 0) return 1.;
//   if (value < 0) return -1.;
//   return 0;
// }
//
// double xinit(BODY *body, int iBody) {
//   return body[iBody].dSemi/AUM * (cos(body[iBody].dEccA) - body[iBody].dEcc);
// }
//
// double yinit(BODY *body, int iBody) {
//   return body[iBody].dSemi/AUM * sqrt(1.0-pow(body[iBody].dEcc,2)) *
//   sin(body[iBody].dEccA);
// }
//
// double vxi(BODY *body, int iBody) {
//   double x, y, mu, n;
//   x = xinit(body, iBody);
//   y = yinit(body, iBody);
//   mu = pow(KGAUSS,2)*(body[0].dMass+body[iBody].dMass)/MSUN;
//   n = sqrt(mu/pow(body[iBody].dSemi/AUM,3));
//   return
//   -pow(body[iBody].dSemi/AUM,2)*n*sin(body[iBody].dEccA)/sqrt(pow(x,2)+pow(y,2));
// }
//
// double vyi(BODY *body, int iBody) {
//   double x, y, mu, n, v;
//   x = xinit(body, iBody);
//   y = yinit(body, iBody);
//   mu = pow(KGAUSS,2)*(body[0].dMass+body[iBody].dMass)/MSUN;
//   n = sqrt(mu/pow(body[iBody].dSemi/AUM,3));
//   v =
//   pow(body[iBody].dSemi/AUM,2)*n*sqrt((1.0-pow(body[iBody].dEcc,2))/(pow(x,2)+pow(y,2)))*cos(body[iBody].dEccA);
//   return v;
// }
//
// double xangle1(BODY *body, int iBody) {
//   return cos(body[iBody].dLongA)*cos(body[iBody].dLongP-body[iBody].dLongA) -
//   sin(body[iBody].dLongA)*sin(body[iBody].dLongP-body[iBody].dLongA)*(1.0-2.*pow(body[iBody].dSinc,2));
// }
//
// double xangle2(BODY *body, int iBody) {
//   return -cos(body[iBody].dLongA)*sin(body[iBody].dLongP-body[iBody].dLongA)
//   -
//   sin(body[iBody].dLongA)*cos(body[iBody].dLongP-body[iBody].dLongA)*(1.0-2.*pow(body[iBody].dSinc,2));
// }
//
// double yangle1(BODY *body, int iBody) {
//   return sin(body[iBody].dLongA)*cos(body[iBody].dLongP-body[iBody].dLongA) +
//   cos(body[iBody].dLongA)*sin(body[iBody].dLongP-body[iBody].dLongA)*(1.0-2.*pow(body[iBody].dSinc,2));
// }
//
// double yangle2(BODY *body, int iBody) {
//   return -sin(body[iBody].dLongA)*sin(body[iBody].dLongP-body[iBody].dLongA)
//   +
//   cos(body[iBody].dLongA)*cos(body[iBody].dLongP-body[iBody].dLongA)*(1.0-2.*pow(body[iBody].dSinc,2));
// }
//
// double zangle1(BODY *body, int iBody) {
//   return
//   sin(body[iBody].dLongP-body[iBody].dLongA)*(2.*body[iBody].dSinc*sqrt(1.0-pow(body[iBody].dSinc,2)));
// }
//
// double zangle2(BODY *body, int iBody) {
//   return
//   cos(body[iBody].dLongP-body[iBody].dLongA)*(2.*body[iBody].dSinc*sqrt(1.0-pow(body[iBody].dSinc,2)));
// }

//========================== End Coordinate Changes ============================

void CountHaltsSpiNBody(HALT *halt, int *iHalt) {
  // What halts do we want? Collision?
}

void VerifyHaltSpiNBody(BODY *body, CONTROL *control, OPTIONS *options,
                        int iBody, int *iHalt) {
}

//============================ Writing Functions ===============================

void WritePositionX(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dPositionX;
  fvFormattedString(cUnit, "");
}

void WritePositionY(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dPositionY;
  fvFormattedString(cUnit, "");
}

void WritePositionZ(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dPositionZ;
  fvFormattedString(cUnit, "");
}

void WriteVelX(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char **cUnit) {
  *dTmp = body[iBody].dVelX;
  fvFormattedString(cUnit, "");
}

void WriteVelY(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char **cUnit) {
  *dTmp = body[iBody].dVelY;
  fvFormattedString(cUnit, "");
}

void WriteVelZ(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char **cUnit) {
  *dTmp = body[iBody].dVelZ;
  fvFormattedString(cUnit, "");
}

void WriteInclinationSpinBody(BODY *body, CONTROL *control, OUTPUT *output,
                              SYSTEM *system, UNITS *units, UPDATE *update,
                              int iBody, double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dInc;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteLongASpinBody(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char **cUnit) {
  *dTmp = body[iBody].dLongA;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void InitializeOutputSpiNBody(OUTPUT *output, fnWriteOutput fnWrite[]) {
  // Output example for dPositionX variable
  // XXX All of these should be deprecated so that it doesn't matter if the user
  // calls DistOrb or SpiNBody
  fvFormattedString(&output[OUT_POSITIONXSPINBODY].cName, "PositionXSpiNBody");
  fvFormattedString(&output[OUT_POSITIONXSPINBODY].cDescr,
          "Body's x position in SpiNBody");
  // fvFormattedString(output[OUT_POSITIONXSPINBODY].cNeg," ");
  output[OUT_POSITIONXSPINBODY].bNeg = 0;
  // output[OUT_POSITIONXSPINBODY].dNeg = 1;
  output[OUT_POSITIONXSPINBODY].iNum       = 1;
  output[OUT_POSITIONXSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_POSITIONXSPINBODY]           = &WritePositionX;

  fvFormattedString(&output[OUT_POSITIONYSPINBODY].cName, "PositionYSpiNBody");
  fvFormattedString(&output[OUT_POSITIONYSPINBODY].cDescr,
          "Body's y position in SpiNBody");
  // fvFormattedString(output[OUT_POSITIONYSPINBODY].cNeg," ");
  output[OUT_POSITIONYSPINBODY].bNeg = 0;
  // output[OUT_POSITIONYSPINBODY].dNeg = 1;
  output[OUT_POSITIONYSPINBODY].iNum       = 1;
  output[OUT_POSITIONYSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_POSITIONYSPINBODY]           = &WritePositionY;

  fvFormattedString(&output[OUT_POSITIONZSPINBODY].cName, "PositionZSpiNBody");
  fvFormattedString(&output[OUT_POSITIONZSPINBODY].cDescr,
          "Body's z position in SpiNBody");
  // fvFormattedString(output[OUT_POSITIONZSPINBODY].cNeg," ");
  output[OUT_POSITIONZSPINBODY].bNeg = 0;
  // output[OUT_POSITIONZSPINBODY].dNeg = 1;
  output[OUT_POSITIONZSPINBODY].iNum       = 1;
  output[OUT_POSITIONZSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_POSITIONZSPINBODY]           = &WritePositionZ;

  fvFormattedString(&output[OUT_VELXSPINBODY].cName, "VelXSpiNBody");
  fvFormattedString(&output[OUT_VELXSPINBODY].cDescr, "Body's x velocity in SpiNBody");
  // fvFormattedString(output[OUT_VELXSPINBODY].cNeg," ");
  output[OUT_VELXSPINBODY].bNeg = 0;
  // output[OUT_VELXSPINBODY].dNeg = 1;
  output[OUT_VELXSPINBODY].iNum       = 1;
  output[OUT_VELXSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_VELXSPINBODY]           = &WriteVelX;

  fvFormattedString(&output[OUT_VELYSPINBODY].cName, "VelYSpiNBody");
  fvFormattedString(&output[OUT_VELYSPINBODY].cDescr, "Body's y velocity in SpiNBody");
  // fvFormattedString(output[OUT_VELYSPINBODY].cNeg," ");
  output[OUT_VELYSPINBODY].bNeg = 0;
  // output[OUT_VELYSPINBODY].dNeg = 1;
  output[OUT_VELYSPINBODY].iNum       = 1;
  output[OUT_VELYSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_VELYSPINBODY]           = &WriteVelY;

  fvFormattedString(&output[OUT_VELZSPINBODY].cName, "VelZSpiNBody");
  fvFormattedString(&output[OUT_VELZSPINBODY].cDescr, "Body's z velocity in SpiNBody");
  // fvFormattedString(output[OUT_VELZSPINBODY].cNeg," ");
  output[OUT_VELZSPINBODY].bNeg = 0;
  // output[OUT_VELZSPINBODY].dNeg = 1;
  output[OUT_VELZSPINBODY].iNum       = 1;
  output[OUT_VELZSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_VELZSPINBODY]           = &WriteVelZ;

  fvFormattedString(&output[OUT_INCSPINBODY].cName, "SpiNBodyInc");
  fvFormattedString(&output[OUT_INCSPINBODY].cDescr, "Body's inclination in SpiNBody");
  fvFormattedString(&output[OUT_INCSPINBODY].cNeg, "Deg");
  output[OUT_INCSPINBODY].bNeg       = 1;
  output[OUT_INCSPINBODY].dNeg       = 1. / DEGRAD;
  output[OUT_INCSPINBODY].iNum       = 1;
  output[OUT_INCSPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_INCSPINBODY]           = &WriteInclinationSpinBody;

  fvFormattedString(&output[OUT_LONGASPINBODY].cName, "SpiNBodyLongA");
  fvFormattedString(&output[OUT_LONGASPINBODY].cDescr, "Body's inclination in SpiNBody");
  fvFormattedString(&output[OUT_LONGASPINBODY].cNeg, "Deg");
  output[OUT_LONGASPINBODY].bNeg       = 1;
  output[OUT_LONGASPINBODY].dNeg       = 1. / DEGRAD;
  output[OUT_LONGASPINBODY].iNum       = 1;
  output[OUT_LONGASPINBODY].iModuleBit = SPINBODY;
  fnWrite[OUT_LONGASPINBODY]           = &WriteLongASpinBody;
}

//============================ End Writing Functions ===========================

void fnForceBehaviorSpiNBody(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                             SYSTEM *system, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody,
                             int iModule) {
}

void PropsAuxSpiNBody(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                      int iBody) {
  int jBody, iNumBodies;
  double DistanceX, DistanceY, DistanceZ, Distance3;

  iNumBodies      = evolve->iNumBodies;
  body[iBody].dGM = BIGG * body[iBody].dMass;

  for (jBody = 0; jBody < iNumBodies; jBody++) {
    // Calculate the cube of the distance to each perturbing body. Used in Vx,
    // Vy, and Vz calculations.
    if (iBody < jBody) {
      //(body[jBody].dPositionY-body[iaBody[0]].dPositionY)/body[iaBody[0]].dDistance3[jBody];
      DistanceX = body[jBody].dPositionX - body[iBody].dPositionX;
      DistanceY = body[jBody].dPositionY - body[iBody].dPositionY;
      DistanceZ = body[jBody].dPositionZ - body[iBody].dPositionZ;
      /* body[iBody].dDistance3[jBody] = sqrt(DistanceX*DistanceX
            + DistanceY*DistanceY
            + DistanceZ*DistanceZ); */
      Distance3 = sqrt(DistanceX * DistanceX + DistanceY * DistanceY +
                       DistanceZ * DistanceZ);
      // body[iBody].dDistance3[jBody] =
      // 1/(body[iBody].dDistance3[jBody]*body[iBody].dDistance3[jBody]*body[iBody].dDistance3[jBody]);
      Distance3                     = 1 / (Distance3 * Distance3 * Distance3);
      body[iBody].dDistanceX[jBody] = DistanceX * Distance3;
      body[iBody].dDistanceY[jBody] = DistanceY * Distance3;
      body[iBody].dDistanceZ[jBody] = DistanceZ * Distance3;

      // body[jBody].dDistance3[iBody] = body[iBody].dDistance3[jBody];
      body[jBody].dDistanceX[iBody] = -body[iBody].dDistanceX[jBody];
      body[jBody].dDistanceY[iBody] = -body[iBody].dDistanceY[jBody];
      body[jBody].dDistanceZ[iBody] = -body[iBody].dDistanceZ[jBody];
    }
  }
}

//========================= Finalize Variable Functions ========================
void FinalizeUpdateVelXSpiNBody(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = SPINBODY;
  update[iBody].iNumVelX              = (*iEqn)++;
}

void FinalizeUpdateVelYSpiNBody(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = SPINBODY;
  update[iBody].iNumVelY              = (*iEqn)++;
}

void FinalizeUpdateVelZSpiNBody(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = SPINBODY;
  update[iBody].iNumVelZ              = (*iEqn)++;
}

void FinalizeUpdatePositionXSpiNBody(BODY *body, UPDATE *update, int *iEqn,
                                     int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = SPINBODY;
  update[iBody].iNumPositionX         = (*iEqn)++;
}

void FinalizeUpdatePositionYSpiNBody(BODY *body, UPDATE *update, int *iEqn,
                                     int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = SPINBODY;
  update[iBody].iNumPositionY         = (*iEqn)++;
}

void FinalizeUpdatePositionZSpiNBody(BODY *body, UPDATE *update, int *iEqn,
                                     int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = SPINBODY;
  update[iBody].iNumPositionZ         = (*iEqn)++;
}
//======================= End Finalize Variable Functions ======================

//================================ Log Functions ===============================

void LogBodySpiNBody(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UPDATE *update, fnWriteOutput fnWrite[],
                     FILE *fp, int iBody) {
  int iOut;
  fprintf(fp, "----- SPINBODY PARAMETERS (%s)------\n", body[iBody].cName);

  for (iOut = OUTSTARTSPINBODY; iOut < OUTENDSPINBODY; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
  }
}

void LogSpiNBody(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                 UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {
  int iOut;
  // I don't think this is called anywhere
  fprintf(fp, "\n----- SPINBODY PARAMETERS ------\n");
  for (iOut = OUTSTARTSPINBODY; iOut < OUTBODYSTARTSPINBODY; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, 0);
    }
  }
}

//============================= End Log Functions ==============================

// Add Module Function
void AddModuleSpiNBody(CONTROL *control, MODULE *module, int iBody,
                       int iModule) {

  module->iaModule[iBody][iModule] = SPINBODY; // Name

  // Halts
  module->fnCountHalts[iBody][iModule] = &CountHaltsSpiNBody;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltSpiNBody;

  module->fnLogBody[iBody][iModule]           = &LogBodySpiNBody;
  module->fnReadOptions[iBody][iModule]       = &ReadOptionsSpiNBody;
  module->fnVerify[iBody][iModule]            = &VerifySpiNBody;
  module->fnAssignDerivatives[iBody][iModule] = &AssignSpiNBodyDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullSpiNBodyDerivatives;

  module->fnInitializeBody[iBody][iModule]   = &InitializeBodySpiNBody;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateSpiNBody;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputSpiNBody;
  module->fnInitializeUpdateTmpBody[iBody][iModule] =
        &InitializeUpdateTmpBodySpiNBody;

  // Primary Variable Finalizations
  module->fnFinalizeUpdateVelX[iBody][iModule] = &FinalizeUpdateVelXSpiNBody;
  module->fnFinalizeUpdateVelY[iBody][iModule] = &FinalizeUpdateVelYSpiNBody;
  module->fnFinalizeUpdateVelZ[iBody][iModule] = &FinalizeUpdateVelZSpiNBody;
  module->fnFinalizeUpdatePositionX[iBody][iModule] =
        &FinalizeUpdatePositionXSpiNBody;
  module->fnFinalizeUpdatePositionY[iBody][iModule] =
        &FinalizeUpdatePositionYSpiNBody;
  module->fnFinalizeUpdatePositionZ[iBody][iModule] =
        &FinalizeUpdatePositionZSpiNBody;
}

//============================= SpiNBody Functions =============================

double fdDPositionXDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dVelX;
}

double fdDPositionYDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dVelY;
}

double fdDPositionZDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dVelZ;
}

double fdDVelXDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dSumX = 0; // Double used to calculate the net perturbation
  int jBody, iGravPerts;
  iGravPerts = body[iaBody[0]].iGravPertsSpiNBody;

  for (jBody = 0; jBody < iGravPerts; jBody++) {
    if (iaBody[0] != jBody) {
      // dSumX = dSumX +
      // BIGG*body[jBody].dMass*(body[jBody].dPositionX-body[iaBody[0]].dPositionX)/body[iaBody[0]].dDistance3[jBody];
      dSumX = dSumX + body[jBody].dGM * body[iaBody[0]].dDistanceX[jBody];
    }
  }

  return dSumX;
}

double fdDVelYDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dSumY = 0; // Double used to calculate the net perturbation
  int jBody, iGravPerts;
  iGravPerts = body[iaBody[0]].iGravPertsSpiNBody;

  for (jBody = 0; jBody < iGravPerts; jBody++) {
    if (iaBody[0] != jBody) {
      // dSumY = dSumY +
      // BIGG*body[jBody].dMass*(body[jBody].dPositionY-body[iaBody[0]].dPositionY)/body[iaBody[0]].dDistance3[jBody];
      dSumY = dSumY + body[jBody].dGM * body[iaBody[0]].dDistanceY[jBody];
    }
  }

  return dSumY;
}

double fdDVelZDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dSumZ = 0; // Double used to calculate the net perturbation
  int jBody, iGravPerts;
  iGravPerts = body[iaBody[0]].iGravPertsSpiNBody;

  for (jBody = 0; jBody < iGravPerts; jBody++) {
    if (iaBody[0] != jBody) {
      // dSumZ = dSumZ +
      // BIGG*body[jBody].dMass*(body[jBody].dPositionZ-body[iaBody[0]].dPositionZ)/body[iaBody[0]].dDistance3[jBody];
      dSumZ = dSumZ + body[jBody].dGM * body[iaBody[0]].dDistanceZ[jBody];
    }
  }

  return dSumZ;
}
//========================== End SpiNBody Functions ============================
