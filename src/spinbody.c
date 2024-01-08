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
#define dot(a,b)        (a[0]*b[0]+a[1]*b[1]+a[2]*b[2])
#define magnitudeSquared(a) (a[0]*a[0] + a[1]*a[1] + a[2]*a[2])
#define magnitude(a) (sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]))
#define NumElements(x)  (sizeof(x) / sizeof(x[0])) // finds number of elements in an array

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
    if (!system->bBarycentric){
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    }
    // MeanA can have limited domain when orbit is bound
    // MeanA should have units only in radians if orbit is unbound
    if (body[iFile - 1].dEcc < 1) {                         
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
    }
    body[iFile - 1].dMeanA = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dMeanA = options->dDefault;
  }
}

void ReadMeanL(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    if (!system->bBarycentric){
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    }
    // MeanA can have limited domain when orbit is bound
    // MeanA should have units only in radians if orbit is unbound
    if (body[iFile - 1].dEcc < 1) {                    
      if (control->Units[iFile].iAngle == 0) {
        // Need to move this condition elsewhere. Unbound orbits don't limit MeanL
        
        if (dTmp < 0 || dTmp > 2 * PI) {
          if (control->Io.iVerbose >= VERBERR) {
            fprintf(stderr, "ERROR: %s must be in the range [0,2*PI].\n",
                    options->cName);
          }
          LineExit(files->Infile[iFile].cIn, lTmp);
        }
      } else {
        // Need to move this condition elsewhere. Unbound orbits don't limit MeanL
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
    }
    body[iFile - 1].dMeanL = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dMeanL = options->dDefault;
  }
}

void ReadUseOrbParams(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp = 0;

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

void ReadBarycentric(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp = 0;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp, VERBALL);
    system->bBarycentric = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (system->bBarycentric == bTmp) {
    system->bBarycentric = options->dDefault;
  }
}

void ReadExcludeFromBarycenter(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp = 0;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bExcludeFromBarycenter = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    body[iFile - 1].bExcludeFromBarycenter = options->dDefault;
  }
}

void ReadOutputLocalBaryCoords(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp = 0;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp, VERBALL);
    system->bOutputLocalBaryCoords = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (system->bBarycentric == bTmp) {
    system->bOutputLocalBaryCoords = options->dDefault;
  }
}

void ReadInputCommonBaryCoords(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp = 0;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp, VERBALL);
    system->bInputCommonBaryCoords = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (system->bBarycentric == bTmp) {
    system->bInputCommonBaryCoords = options->dDefault;
  }
}

void ReadLocalInvPlane(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp = 0;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp, VERBALL);
    system->bLocalInvPlane = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (system->bBarycentric == bTmp) {
    system->bLocalInvPlane = options->dDefault;
  }
}

void ReadRemainInLocalBaryIfEjected(BODY *body, CONTROL *control, FILES *files,
                                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp = 0;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bRemainInLocalBaryIfEjected = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    body[iFile - 1].bRemainInLocalBaryIfEjected = options->dDefault;
  }
}

void ReadCalcCoordsFromCenterOfMass(BODY *body, CONTROL *control, FILES *files,
                                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp = 0;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp, VERBALL);
    body[iFile - 1].bCalcCoordsFromCenterOfMass = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    body[iFile - 1].bCalcCoordsFromCenterOfMass = options->dDefault;
  }
}

void ReadHaltBodyUnbound(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp = 0;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    body[iFile - 1].bHaltBodyUnbound = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    body[iFile - 1].bHaltBodyUnbound = options->dDefault;
  }
}

void InitializeOptionsSpiNBody(OPTIONS *options, fnReadOption fnRead[]) {
  int iOpt, iFile;


  // XXX All of these should be deprecated so that it doesn't matter if the user
  // calls DistOrb or SpiNBody
  sprintf(options[OPT_POSITIONXSPINBODY].cName, "dPositionXSpiNBody");
  sprintf(options[OPT_POSITIONXSPINBODY].cDescr, "X position of the body");
  sprintf(options[OPT_POSITIONXSPINBODY].cDefault, "0");
  sprintf(options[OPT_POSITIONXSPINBODY].cDimension, "length");
  options[OPT_POSITIONXSPINBODY].dDefault   = 0.0;
  options[OPT_POSITIONXSPINBODY].iType      = 2;
  options[OPT_POSITIONXSPINBODY].bMultiFile = 1;
  fnRead[OPT_POSITIONXSPINBODY]             = &ReadPositionX;

  sprintf(options[OPT_POSITIONYSPINBODY].cName, "dPositionYSpiNBody");
  sprintf(options[OPT_POSITIONYSPINBODY].cDescr, "Y position of the body");
  sprintf(options[OPT_POSITIONYSPINBODY].cDefault, "0");
  sprintf(options[OPT_POSITIONYSPINBODY].cDimension, "length");
  options[OPT_POSITIONYSPINBODY].dDefault   = 0.0;
  options[OPT_POSITIONYSPINBODY].iType      = 2;
  options[OPT_POSITIONYSPINBODY].bMultiFile = 1;
  fnRead[OPT_POSITIONYSPINBODY]             = &ReadPositionY;

  sprintf(options[OPT_POSITIONZSPINBODY].cName, "dPositionZSpiNBody");
  sprintf(options[OPT_POSITIONZSPINBODY].cDescr, "Z position of the body");
  sprintf(options[OPT_POSITIONZSPINBODY].cDefault, "0");
  sprintf(options[OPT_POSITIONZSPINBODY].cDimension, "length");
  options[OPT_POSITIONZSPINBODY].dDefault   = 0.0;
  options[OPT_POSITIONZSPINBODY].iType      = 2;
  options[OPT_POSITIONZSPINBODY].bMultiFile = 1;
  fnRead[OPT_POSITIONZSPINBODY]             = &ReadPositionZ;

  sprintf(options[OPT_VELXSPINBODY].cName, "dVelXSpiNBody");
  sprintf(options[OPT_VELXSPINBODY].cDescr, "X velocity of the body");
  sprintf(options[OPT_VELXSPINBODY].cDefault, "0");
  sprintf(options[OPT_VELXSPINBODY].cDimension, "length/time");
  options[OPT_VELXSPINBODY].dDefault   = 0.0;
  options[OPT_VELXSPINBODY].iType      = 2;
  options[OPT_VELXSPINBODY].bMultiFile = 1;
  fnRead[OPT_VELXSPINBODY]             = &ReadVelX;

  sprintf(options[OPT_VELYSPINBODY].cName, "dVelYSpiNBody");
  sprintf(options[OPT_VELYSPINBODY].cDescr, "Y velocity of the body");
  sprintf(options[OPT_VELYSPINBODY].cDefault, "0");
  sprintf(options[OPT_VELYSPINBODY].cDimension, "length/time");
  options[OPT_VELYSPINBODY].dDefault   = 0.0;
  options[OPT_VELYSPINBODY].iType      = 2;
  options[OPT_VELYSPINBODY].bMultiFile = 1;
  fnRead[OPT_VELYSPINBODY]             = &ReadVelY;

  sprintf(options[OPT_VELZSPINBODY].cName, "dVelZSpiNBody");
  sprintf(options[OPT_VELZSPINBODY].cDescr, "Z velocity of the body");
  sprintf(options[OPT_VELZSPINBODY].cDefault, "0");
  sprintf(options[OPT_VELZSPINBODY].cDimension, "length/time");
  options[OPT_VELZSPINBODY].dDefault   = 0.0;
  options[OPT_VELZSPINBODY].iType      = 2;
  options[OPT_VELZSPINBODY].bMultiFile = 1;
  fnRead[OPT_VELZSPINBODY]             = &ReadVelZ;

  sprintf(options[OPT_MEANA].cName, "dMeanA");
  sprintf(options[OPT_MEANA].cDescr, "Mean anomaly");
  sprintf(options[OPT_MEANA].cDefault, "0.0");
  sprintf(options[OPT_MEANA].cDimension, "angle");
  options[OPT_MEANA].dDefault   = 0.0;
  options[OPT_MEANA].iType      = 2;
  options[OPT_MEANA].bMultiFile = 1;
  fnRead[OPT_MEANA]             = &ReadMeanA;

  sprintf(options[OPT_MEANL].cName, "dMeanL");
  sprintf(options[OPT_MEANL].cDescr, "Mean longitude");
  sprintf(options[OPT_MEANL].cDefault, "0.0");
  sprintf(options[OPT_MEANL].cDimension, "angle");
  options[OPT_MEANL].dDefault   = 0.0;
  options[OPT_MEANL].iType      = 2;
  options[OPT_MEANL].bMultiFile = 1;
  fnRead[OPT_MEANL]             = &ReadMeanL;

  sprintf(options[OPT_USEORBPARAMS].cName, "bUseOrbParams");
  sprintf(options[OPT_USEORBPARAMS].cDescr,
          "Flag to use orbital parameters as inputs");
  sprintf(options[OPT_USEORBPARAMS].cDefault, "0");
  options[OPT_USEORBPARAMS].dDefault   = 0;
  options[OPT_USEORBPARAMS].iType      = 0;
  options[OPT_USEORBPARAMS].bMultiFile = 1;
  options[OPT_USEORBPARAMS].bNeg       = 0;
  options[OPT_USEORBPARAMS].iFileType  = 1;
  fnRead[OPT_USEORBPARAMS]             = &ReadUseOrbParams;

  sprintf(options[OPT_BARYCENTRIC].cName, "bBarycentric");
  sprintf(options[OPT_BARYCENTRIC].cDescr, "Coordinate system's origin at the barycenter");
  sprintf(options[OPT_BARYCENTRIC].cDefault, "0");
  options[OPT_BARYCENTRIC].dDefault = 0;
  options[OPT_BARYCENTRIC].iType = 0;
  options[OPT_BARYCENTRIC].bMultiFile = 0;
  options[OPT_BARYCENTRIC].bNeg = 0;
  options[OPT_BARYCENTRIC].iFileType = 2;
  options[OPT_BARYCENTRIC].iModuleBit = SPINBODY;
  fnRead[OPT_BARYCENTRIC]             = &ReadBarycentric;

  sprintf(options[OPT_EXCLUDEFROMBARYCENTER].cName, "bExcludeFromBarycenter");
  sprintf(options[OPT_EXCLUDEFROMBARYCENTER].cDescr,
          "Excludes body from Barycenter calculation");
  sprintf(options[OPT_EXCLUDEFROMBARYCENTER].cDefault, "0");
  options[OPT_EXCLUDEFROMBARYCENTER].dDefault   = 0;
  options[OPT_EXCLUDEFROMBARYCENTER].iType      = 0;
  options[OPT_EXCLUDEFROMBARYCENTER].bMultiFile = 1;
  options[OPT_EXCLUDEFROMBARYCENTER].bNeg       = 0;
  options[OPT_EXCLUDEFROMBARYCENTER].iFileType  = 1;
  options[OPT_EXCLUDEFROMBARYCENTER].iModuleBit = SPINBODY;
  fnRead[OPT_EXCLUDEFROMBARYCENTER]             = &ReadExcludeFromBarycenter;

  sprintf(options[OPT_OUTPUTLOCALBARYCOORDS].cName, "bOutputLocalBaryCoords");
  sprintf(options[OPT_OUTPUTLOCALBARYCOORDS].cDescr, 
  "Outputs Barycentric Cartesian Coordinates and Orbital Elements w.r.t Local Barycenter.");
  sprintf(options[OPT_OUTPUTLOCALBARYCOORDS].cDefault, "0");
  options[OPT_OUTPUTLOCALBARYCOORDS].dDefault   = 0;
  options[OPT_OUTPUTLOCALBARYCOORDS].iType      = 0;
  options[OPT_OUTPUTLOCALBARYCOORDS].bMultiFile = 0;
  options[OPT_OUTPUTLOCALBARYCOORDS].bNeg       = 0;
  options[OPT_OUTPUTLOCALBARYCOORDS].iFileType  = 2;
  options[OPT_OUTPUTLOCALBARYCOORDS].iModuleBit = SPINBODY;
  fnRead[OPT_OUTPUTLOCALBARYCOORDS]             = &ReadOutputLocalBaryCoords;

  sprintf(options[OPT_INPUTCOMMONBARYCOORDS].cName, "bInputCommonBaryCoords");
  sprintf(options[OPT_INPUTCOMMONBARYCOORDS].cDescr, 
  "Input values w.r.t common barycenter even when bOutputLocalBaryCoords is on.");
  sprintf(options[OPT_INPUTCOMMONBARYCOORDS].cDefault, "0");
  options[OPT_INPUTCOMMONBARYCOORDS].dDefault   = 0;
  options[OPT_INPUTCOMMONBARYCOORDS].iType      = 0;
  options[OPT_INPUTCOMMONBARYCOORDS].bMultiFile = 0;
  options[OPT_INPUTCOMMONBARYCOORDS].bNeg       = 0;
  options[OPT_INPUTCOMMONBARYCOORDS].iFileType  = 2;
  options[OPT_INPUTCOMMONBARYCOORDS].iModuleBit = SPINBODY;
  fnRead[OPT_INPUTCOMMONBARYCOORDS]             = &ReadInputCommonBaryCoords;

  sprintf(options[OPT_LOCALINVPLANE].cName,"bLocalInvPlane");
  sprintf(options[OPT_LOCALINVPLANE].cDescr, 
  "Outputs Local Barycentric Coordinates w.r.t Local Invariable Plane.");
  sprintf(options[OPT_LOCALINVPLANE].cDefault, "0");
  options[OPT_LOCALINVPLANE].dDefault   = 0;
  options[OPT_LOCALINVPLANE].iType      = 0;
  options[OPT_LOCALINVPLANE].bMultiFile = 0;
  options[OPT_LOCALINVPLANE].bNeg       = 0;
  options[OPT_LOCALINVPLANE].iFileType  = 2;
  options[OPT_LOCALINVPLANE].iModuleBit = SPINBODY;
  fnRead[OPT_LOCALINVPLANE]             = &ReadLocalInvPlane;  


  sprintf(options[OPT_REMAININLOCALBARYIFEJECTED].cName, "bRemainInLocalBaryIfEjected");
  sprintf(options[OPT_REMAININLOCALBARYIFEJECTED].cDescr,
          "Body contributes to Local Barycenter even if ejected.");
  sprintf(options[OPT_REMAININLOCALBARYIFEJECTED].cDefault, "0");
  options[OPT_REMAININLOCALBARYIFEJECTED].dDefault   = 0;
  options[OPT_REMAININLOCALBARYIFEJECTED].iType      = 0;
  options[OPT_REMAININLOCALBARYIFEJECTED].bMultiFile = 1;
  options[OPT_REMAININLOCALBARYIFEJECTED].bNeg       = 0;
  options[OPT_REMAININLOCALBARYIFEJECTED].iFileType  = 1;
  options[OPT_REMAININLOCALBARYIFEJECTED].iModuleBit = SPINBODY;
  fnRead[OPT_REMAININLOCALBARYIFEJECTED]             = &ReadRemainInLocalBaryIfEjected;     

  sprintf(options[OPT_CALCCOORDSFROMCENTEROFMASS].cName, "bCalcCoordsFromCenterOfMass");
  sprintf(options[OPT_CALCCOORDSFROMCENTEROFMASS].cDescr, 
  "Notes the body contributing to the barycenter having undefined barycentric inputs.");
  sprintf(options[OPT_CALCCOORDSFROMCENTEROFMASS].cDefault, "0");
  options[OPT_CALCCOORDSFROMCENTEROFMASS].dDefault = 0;
  options[OPT_CALCCOORDSFROMCENTEROFMASS].iType = 0;
  options[OPT_CALCCOORDSFROMCENTEROFMASS].bMultiFile = 0;
  options[OPT_CALCCOORDSFROMCENTEROFMASS].bNeg = 0;
  options[OPT_CALCCOORDSFROMCENTEROFMASS].iFileType = 1;
  options[OPT_CALCCOORDSFROMCENTEROFMASS].iModuleBit = SPINBODY;
  fnRead[OPT_CALCCOORDSFROMCENTEROFMASS]        = &ReadCalcCoordsFromCenterOfMass;

  sprintf(options[OPT_HALTBODYUNBOUND].cName, "bHaltBodyUnbound");
  sprintf(options[OPT_HALTBODYUNBOUND].cDescr, 
  "Simulation halts if the chosen body becomes unbound (Eccentricity >= 1).");
  sprintf(options[OPT_HALTBODYUNBOUND].cDefault, "0");
  options[OPT_HALTBODYUNBOUND].dDefault = 0;
  options[OPT_HALTBODYUNBOUND].iType = 0;
  options[OPT_HALTBODYUNBOUND].bMultiFile = 1;
  options[OPT_HALTBODYUNBOUND].bNeg = 0;
  options[OPT_HALTBODYUNBOUND].iFileType = 1;
  options[OPT_HALTBODYUNBOUND].iModuleBit = SPINBODY;
  fnRead[OPT_HALTBODYUNBOUND]        = &ReadHaltBodyUnbound;    
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

    //All of this should be moved to verifyspinbody or deleted


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
            //fvOrbElems2HelioCart(body, iTmpBody);
          }
        }
      }

      //fvHelioCart2BaryCart(body, control->Evolve.iNumBodies, iBody);
      /*
      body[iBody].dPositionX = body[iBody].dBCartPos[0];
      body[iBody].dPositionY = body[iBody].dBCartPos[1];
      body[iBody].dPositionZ = body[iBody].dBCartPos[2];
      body[iBody].dVelX      = body[iBody].dBCartVel[0];
      body[iBody].dVelY      = body[iBody].dBCartVel[1];
      body[iBody].dVelZ      = body[iBody].dBCartVel[2];
      */

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

void fvAssignHelioOrbElems(BODY *body, int iBody) {
    body[iBody].dHelioSemi = body[iBody].dSemi;
    body[iBody].dHelioEcc = body[iBody].dEcc;
    body[iBody].dHelioInc = body[iBody].dInc;
    body[iBody].dHelioArgP = body[iBody].dArgP;
    body[iBody].dHelioLongA = body[iBody].dLongA;
    body[iBody].dHelioLongP = body[iBody].dLongP;
    body[iBody].dHelioMeanA = body[iBody].dMeanA;
    body[iBody].dHelioMeanL =  body[iBody].dMeanL;
}

void fvAssignBaryOrbElems(BODY *body, SYSTEM *system, int iBody){
  if (system->bBarycentric) {
    body[iBody].dBarySemi = body[iBody].dSemi;
    body[iBody].dBaryEcc = body[iBody].dEcc;
    body[iBody].dBaryInc = body[iBody].dInc;
    body[iBody].dBaryArgP = body[iBody].dArgP;
    body[iBody].dBaryLongA = body[iBody].dLongA;
    body[iBody].dBaryLongP = body[iBody].dLongP;
    body[iBody].dBaryMeanA = body[iBody].dMeanA;
    body[iBody].dBaryMeanL = body[iBody].dMeanL;
  }
}

void fvAssignCartOutputs2BaryCart(BODY *body, int iBody) {
  body[iBody].dPositionX = body[iBody].dBCartPos[0];
  body[iBody].dPositionY = body[iBody].dBCartPos[1];
  body[iBody].dPositionZ = body[iBody].dBCartPos[2];

  body[iBody].dVelX = body[iBody].dBCartVel[0];
  body[iBody].dVelY = body[iBody].dBCartVel[1];
  body[iBody].dVelZ = body[iBody].dBCartVel[2];
}

void fvAssignLocalBaryCart(BODY *body, int iBody) {
  int i = 0;
  for (i = 0; i < 3; i++) {
    body[iBody].dLocalBaryCartPos[i] = body[iBody].dBCartPos[i];
    body[iBody].dLocalBaryCartVel[i] = body[iBody].dBCartVel[i];
  }
}

void fvMoveOptions2Helio(BODY *body, int iBody) {
  /*
  When activating HelioCart, dPosition and dVel arrays are temporarily defined as HelioCart
  since the user meant to define these variables in HelioCart initially.
  They shortly get redefined to BaryCart after BaryCart values are calculated.
  */
  body[iBody].dHCartPos[0] = body[iBody].dPositionX;
  body[iBody].dHCartPos[1] = body[iBody].dPositionY;
  body[iBody].dHCartPos[2] = body[iBody].dPositionZ;

  body[iBody].dHCartVel[0] = body[iBody].dVelX;
  body[iBody].dHCartVel[1] = body[iBody].dVelY;
  body[iBody].dHCartVel[2] = body[iBody].dVelZ;
}

void fvAssignBaryCart(BODY *body, int iBody) {
  body[iBody].dBCartPos[0] = body[iBody].dPositionX;
  body[iBody].dBCartPos[1] = body[iBody].dPositionY;
  body[iBody].dBCartPos[2] = body[iBody].dPositionZ;

  body[iBody].dBCartVel[0] = body[iBody].dVelX;
  body[iBody].dBCartVel[1] = body[iBody].dVelY;
  body[iBody].dBCartVel[2] = body[iBody].dVelZ;
}

void fvAssignBaryCart2BaryCart(BODY *body, int iBody) {
  /*
  I didn't know what else to call this function, but we assign 
  these functions below if we want BaryCart outputs
  */
  body[iBody].dBaryPosX = body[iBody].dBCartPos[0];
  body[iBody].dBaryPosY = body[iBody].dBCartPos[1];
  body[iBody].dBaryPosZ = body[iBody].dBCartPos[2];

  body[iBody].dBaryVelX = body[iBody].dBCartVel[0];
  body[iBody].dBaryVelY = body[iBody].dBCartVel[1];
  body[iBody].dBaryVelZ = body[iBody].dBCartVel[2];
}

void fvAssignBaryCart2HelioCart(BODY *body, int iBody) {
  body[iBody].dHelioPosX = body[iBody].dHCartPos[0];
  body[iBody].dHelioPosY = body[iBody].dHCartPos[1];
  body[iBody].dHelioPosZ = body[iBody].dHCartPos[2];

  body[iBody].dHelioVelX = body[iBody].dHCartVel[0];
  body[iBody].dHelioVelY = body[iBody].dHCartVel[1];
  body[iBody].dHelioVelZ = body[iBody].dHCartVel[2];
}

// Outputting the sign of a double. Zero is defaulted as zero.
double sign(double value) {
  if (value > 0.0) {
      return 1.0;
  } else if (value < 0.0) {
      return -1.0;
  } else {
      return 0.0;
  }
}

//positiveModulus() restricts inputs of Orbital Element in the domain [0, 2*PI)
double positiveModulus(double x, double y) { //returns modulo_{y}(x) remaining in the domain 0 <= x < y
  double result;
  result = fmod(x, y);
  while (result < 0.0) {
    result += y;
  }
  return result;
}

double positiveModulus2PI(double x) {
  double dAngle = positiveModulus(x, 2.0 * PI);
  return dAngle;
}

double atan2Continuous(double dSinAngle, double dCosAngle, double dMinimumValue) {
  double dAngle = 0.0;
  if (fabs(dCosAngle) > dMinimumValue) {
    dAngle = positiveModulus2PI(atan2(dSinAngle, dCosAngle));
  } else {
    dAngle = positiveModulus2PI(sign(dSinAngle) * PI / 2.0);
  }

  return dAngle;
}

void CalculateCartesianComponentsFromCenterOfMassEquation(BODY *body, int iNumBodies, 
                                                          int iUnassignedBody, int iDimension) {
  int iTmpBody = 0;                                                            
  for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
    if (iTmpBody != iUnassignedBody && !body[iTmpBody].bExcludeFromBarycenter) {
      double dMassRatio = body[iTmpBody].dMass / body[iUnassignedBody].dMass;
      /* 
      If barycentric coordinates and finding the position and velocity components of one body:
      BarycentricMass*BarycentricComponent = 0 = \sum_{i=0}^{N-1}(Mass_i*Compenent_i)
      0 = Mass_{iBody}*Component_{iBody} + \sum_{i != iBody}^{N-1}(Mass_i*Component_i)
      Component_{iBody} = -\sum_{i != iBody}^{N-1}(Mass_i*Component_i/Mass_iBody)
      */
      body[iUnassignedBody].dBCartPos[iDimension] -= dMassRatio * body[iTmpBody].dBCartPos[iDimension];
      body[iUnassignedBody].dBCartVel[iDimension] -= dMassRatio * body[iTmpBody].dBCartVel[iDimension];
    }
  } 
}

void CalculateBarycentricCoordinatesOfUnassignedBody(BODY *body, int iNumBodies) {
  int iTmpBody = 0;
  for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
    if (body[iTmpBody].bCalcCoordsFromCenterOfMass) {
      if (body[iTmpBody].bExcludeFromBarycenter) {
        printf("ERROR: Cannot turn on bCalcCoordsFromCenterOfMass and bExcludeFromBarycenter in the same body.\n");
        exit(1);
      }
      int iDimension = 0;
      for (iDimension = 0; iDimension < 3; iDimension++) {
        CalculateCartesianComponentsFromCenterOfMassEquation(body, iNumBodies, iTmpBody, iDimension); 
      }
      fvAssignCartOutputs2BaryCart(body, iTmpBody);
      break;
    }
  }
}

void CalculateBaryPositionSquaredAndVelocitySquared(BODY *body, int iBody, int iNumBodies, double* dMagnitudeSquaredPosition, double* dMagnitudeSquaredVelocity) {
  double bBodyExcludedFromBarycenter = 0;
  int iTmpBody = 0;
  for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
    if (body[iTmpBody].bExcludeFromBarycenter) {
      bBodyExcludedFromBarycenter += 1;
      break;
    }
  }
  if (bBodyExcludedFromBarycenter) {
    int i = 0;
    for (i = 0; i < 3; i++) {
      *dMagnitudeSquaredPosition += body[iBody].dLocalBaryCartPos[i] * body[iBody].dLocalBaryCartPos[i];
      *dMagnitudeSquaredVelocity += body[iBody].dLocalBaryCartVel[i] * body[iBody].dLocalBaryCartVel[i];      
    }
  } else {
    int i = 0;
    for (i = 0; i < 3; i++) {
      *dMagnitudeSquaredPosition += body[iBody].dBCartPos[i] * body[iBody].dBCartPos[i];
      *dMagnitudeSquaredVelocity += body[iBody].dBCartVel[i] * body[iBody].dBCartVel[i];      
    }
  }  
}

double CalculateEscapeVelocitySquared(double dGravitationalParameter, double dMagnitudeSquaredPosition) {
  double dMagnitudePosition = sqrt(dMagnitudeSquaredPosition);
  double dEscapeVelocitySquared = 2.0 * dGravitationalParameter / dMagnitudePosition;
  return dEscapeVelocitySquared;
}

void fvRemoveFromLocalBaryCenterIfEjected(BODY *body, int iNumBodies) {
  int iTmpBody = 0;
  for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
    if (!body[iTmpBody].bExcludeFromBarycenter && !body[iTmpBody].bRemainInLocalBaryIfEjected) {
      double dMagnitudeSquaredPosition = 0, dMagnitudeSquaredVelocity = 0;
      double dLocalBaryMu = CalculateBarycentricGravitationalParameter(body, iNumBodies, iTmpBody);
      CalculateBaryPositionSquaredAndVelocitySquared(body, iTmpBody, iNumBodies, &dMagnitudeSquaredPosition, &dMagnitudeSquaredVelocity);
      double dSquaredEscapeVelocity = CalculateEscapeVelocitySquared(dLocalBaryMu, dMagnitudeSquaredPosition);
      if (dMagnitudeSquaredVelocity >= dSquaredEscapeVelocity) {
        body[iTmpBody].bExcludeFromBarycenter = 1;
      }
    }
  }
}

void fvLocalBarycenter2CommonBarycenter(BODY *body, int iNumBodies) {
  double *dCommonBarycenterPosition, *dCommonBarycenterVelocity;
   double dCommonBarycenterMass = TotalMass(body, iNumBodies);
  dCommonBarycenterPosition = malloc(3 * sizeof(double));
  dCommonBarycenterVelocity = malloc(3 * sizeof(double));
  int i = 0;
  for (i = 0; i < 3; i++) {
    dCommonBarycenterPosition[i] = 0;
    dCommonBarycenterVelocity[i] = 0;
  }
  int iTmpBody = 0;
  for (iTmpBody  = 0; iTmpBody < iNumBodies; iTmpBody++) {
    int i = 0;
    for (i = 0; i < 3; i++) {
      dCommonBarycenterPosition[i] += body[iTmpBody].dMass * body[iTmpBody].dLocalBaryCartPos[i] / dCommonBarycenterMass;
      dCommonBarycenterVelocity[i] += body[iTmpBody].dMass * body[iTmpBody].dLocalBaryCartVel[i] / dCommonBarycenterMass;
    }
  }
  for (iTmpBody  = 0; iTmpBody < iNumBodies; iTmpBody++) {
    int i = 0;
    for (i = 0; i < 3; i++) {
      body[iTmpBody].dBCartPos[i] = body[iTmpBody].dLocalBaryCartPos[i] - dCommonBarycenterPosition[i];
      body[iTmpBody].dBCartVel[i] = body[iTmpBody].dLocalBaryCartVel[i] - dCommonBarycenterVelocity[i];
    }
  }
  free(dCommonBarycenterPosition), free(dCommonBarycenterVelocity);
}

void fvCommonBarycenter2LocalBarycenter(BODY *body, int iNumBodies) {
  double dLocalBarycenterMass = TotalBarycentricMass(body, iNumBodies);
  double *dLocalBarycenterPosition, *dLocalBarycenterVelocity;
  dLocalBarycenterPosition = malloc(3 * sizeof(double));
  dLocalBarycenterVelocity = malloc(3 * sizeof(double));
  int i = 0;
  for (i = 0; i < 3; i++) {
    dLocalBarycenterPosition[i] = 0;
    dLocalBarycenterVelocity[i] = 0;
  }
  int iTmpBody = 0;
  for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
    if (!body[iTmpBody].bExcludeFromBarycenter) {
      int i = 0;
      for (i = 0; i < 3; i++) {
        dLocalBarycenterPosition[i] += body[iTmpBody].dMass * body[iTmpBody].dBCartPos[i] / dLocalBarycenterMass;
        dLocalBarycenterVelocity[i] += body[iTmpBody].dMass * body[iTmpBody].dBCartVel[i] / dLocalBarycenterMass;
      }
    }
  }
  for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
    int i = 0;
    for (i = 0; i < 3; i++) {
    body[iTmpBody].dLocalBaryCartPos[i] = body[iTmpBody].dBCartPos[i] - dLocalBarycenterPosition[i];
    body[iTmpBody].dLocalBaryCartVel[i] = body[iTmpBody].dBCartVel[i] - dLocalBarycenterVelocity[i];
    }
  }

  free(dLocalBarycenterPosition), free(dLocalBarycenterVelocity);
}

void CalculateLocalSpecificAngularMomentum(BODY *body, SYSTEM *system, double *dSpecificAngularMomentum, int iNumBodies) {
  int i, iBody;
  double *dSpecificAngularMomentumBody;
  dSpecificAngularMomentumBody = malloc(3 * sizeof(double));
  for (iBody = 0; iBody < iNumBodies; iBody++) {
    if (!body[iBody].bExcludeFromBarycenter) {
      cross(body[iBody].dLocalBaryCartPos, body[iBody].dLocalBaryCartVel, dSpecificAngularMomentumBody);
      for (i = 0; i < 3; i++) {
        dSpecificAngularMomentum[i] += dSpecificAngularMomentumBody[i];
      } 
    }
  }

  free(dSpecificAngularMomentumBody);
}

void SphericalRotation(double *dRotatingVector, double dTheta, double dPhi) {
  double *dTmpVector;
  dTmpVector = malloc(3 * sizeof(double));

  dTmpVector[0] = dRotatingVector[0] * cos(dTheta) + dRotatingVector[1] * sin(dTheta);
  dTmpVector[1] = -dRotatingVector[0] * sin(dTheta) + dRotatingVector[1] * cos(dTheta);
  dTmpVector[2] = dRotatingVector[2];

  dRotatingVector[0] = dTmpVector[0] * cos(dPhi) - dTmpVector[2] * sin(dPhi);
  dRotatingVector[1] = dTmpVector[1];
  dRotatingVector[2] = dTmpVector[0] * sin(dPhi) + dTmpVector[2] * cos(dPhi);

  free(dTmpVector);
}

void RotateToLocalInvariablePlane(BODY *body, SYSTEM *system, int iNumBodies) {
  int iBody;
  double dMinimumValue = 1.0e-15;
  double dSpecificAngularMomentum[3] = {0.0, 0.0, 0.0}; /* locally allocates this memory */

  CalculateLocalSpecificAngularMomentum(body, system, dSpecificAngularMomentum, iNumBodies);
  double dSpecificAngularMomentumXYPlaneSquared = 0;
  int i = 0;
  for (i = 0; i < 2; i++) {
    dSpecificAngularMomentumXYPlaneSquared += dSpecificAngularMomentum[i] * dSpecificAngularMomentum[i];
  }
  double dSpecificAngularMomentumXYPlane = sqrt(dSpecificAngularMomentumXYPlaneSquared);
  if (dSpecificAngularMomentumXYPlane == 0) {
    system->dThetaInvP = 0;
    system->dPhiInvP = 0;
  } else {
    system->dThetaInvP = atan2Continuous(dSpecificAngularMomentum[1], dSpecificAngularMomentum[0], dMinimumValue);
    system->dPhiInvP = atan2Continuous(dSpecificAngularMomentumXYPlane, dSpecificAngularMomentum[2], dMinimumValue);
  }

  for (iBody = 0; iBody < iNumBodies; iBody++) {
    SphericalRotation(body[iBody].dLocalBaryCartPos, system->dThetaInvP, system->dPhiInvP);
    SphericalRotation(body[iBody].dLocalBaryCartVel, system->dThetaInvP, system->dPhiInvP);
  }    
}

void AssignAllCoords(BODY *body, CONTROL *control, FILES *files, SYSTEM *system, int iBody) {
  double iNumBodies = control->Evolve.iNumBodies;

  if (iBody == 0) { // Doing this only once
    int iTmpBody = 0;
    for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {

      // Need Gravitational Parameter to be assigned
      // Use BaryMu because iterated coordinates are Barycentric
      body[iTmpBody].dMu = CalculateCommonBarycentricGravitationalParameter(body, iNumBodies, iBody);
      // Do a check if orbelems need to be converted to inv_plane
      // inv_plane only accepts barycart coords 

      if (!body[iTmpBody].bUseOrbParams) { // If input was BaryCart or HelioCart
        fvAssignBaryCart(body, iTmpBody); // We assign the vectors named dBCart to the input values
      }
    }
    if (system->bOutputLocalBaryCoords) {           
      fvCommonBarycenter2LocalBarycenter(body, iNumBodies);
      fvRemoveFromLocalBaryCenterIfEjected(body, iNumBodies); // only if body[iTmpBody].bRemainInLocalBaryIfEjected == 0
      /*
      if (system->bLocalInvPlane) {
        RotateToLocalInvariablePlane(body, system, iNumBodies);
      }
      */
    }
  }

  if (files->Outfile[iBody].iOutputCoordBit & HELIOCART) {
    fvBaryCart2HelioCart(body, iBody);
    fvAssignBaryCart2HelioCart(body, iBody);
  }
  if (files->Outfile[iBody].iOutputCoordBit & HELIOELEMS) {
    fvBaryCart2HelioOrbElems(body, iNumBodies, iBody);
    fvAssignHelioOrbElems(body, iBody);
  }
  if (files->Outfile[iBody].iOutputCoordBit & BARYELEMS) {
    fvBaryCart2BaryOrbElems(body, system, iNumBodies, iBody);
    //fvAssignBaryOrbElems(body, system, iBody);
  }
}

void VerifyInputCoords(BODY *body, CONTROL *control, OPTIONS *options, SYSTEM *system, int iBody) {
  int iNumBodies = control->Evolve.iNumBodies; // Shorten the notation for clarity
  // Allocate dHCartPos, dBCartPos, dHCartVel, dBCartVel
  // We only want to allocate all of the positions and velocities for every body, so we do this one time at iBody = 0;
  if (iBody == 0) {
    int iTmpBody = 0;
    for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++){
      body[iTmpBody].dBCartPos = malloc(3 * sizeof(double));
      body[iTmpBody].dBCartVel = malloc(3 * sizeof(double));
      body[iTmpBody].dHCartPos = malloc(3 * sizeof(double));
      body[iTmpBody].dHCartVel = malloc(3 * sizeof(double));
    }
  }
  if (body[iBody].bUseOrbParams) {  
    if(!system->bBarycentric) {
      printf("HelioOrbElems. Converting to HelioCart...\n");
      if (iBody == 0) {
        int iTmpBody = 0;
        for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
          fvAssignHelioOrbElems(body, iTmpBody);
          if (iTmpBody > 0) { // The central body already has all OrbElems as zero.
            VerifyAltOrbElems(body, options, iTmpBody);
          }
          fvHelioOrbElems2HelioCart(body, iNumBodies, iTmpBody);
        }
      } 
      fvHelioCart2BaryCart(body, iNumBodies, iBody);
      // fvAssignSpinBodyVariables();
    } else if (!body[iBody].bCalcCoordsFromCenterOfMass) {
      printf("BaryOrbElems. Converting to BaryCart...\n");
      fvAssignBaryOrbElems(body, system, iBody);
      VerifyAltOrbElems(body, options, iBody);
      fvBaryOrbElems2BaryCart(body, iNumBodies, iBody);
    } 
    fvAssignCartOutputs2BaryCart(body, iBody); // Example: dPositionX = dBCartPos[0];
  } else {
    if(!system->bBarycentric) {
      printf("HelioCart. Converting to BaryCart...\n");
      // dPos variables get reassigned later as BaryCart values. Only use fvMoveOptions2Helio initially
      if (body[iBody].dPositionX == 0 && body[iBody].dPositionY == 0 && body[iBody].dPositionZ == 0) {
        int iTmpBody = 0;
        for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
          fvMoveOptions2Helio(body, iTmpBody);  // Example: dHCartPos[0] = dPositionX;
        }
      }
      fvHelioCart2BaryCart(body, iNumBodies, iBody);
      fvAssignCartOutputs2BaryCart(body, iBody);
      // fvAssignSpinBodyVariables();
    } else if (!body[iBody].bCalcCoordsFromCenterOfMass) {    
      // BaryCart was the input. No conversions occurred, so we need to assign BaryCart Pos/Vel to dPosition/dVel
      fvAssignBaryCart(body, iBody); // Example: dBCartPos[0] = dPositionX;
    }
    printf("BaryCart. No Conversion needed.\n");
  }

  if (body[iBody].bCalcCoordsFromCenterOfMass && !system->bBarycentric) {
    printf("ERROR: Cannot simultaneously turn on bCalcCoordsFromCenterOfMass and turn off bBarycentric.\n");
    exit(1);    
  }

  if (system->bBarycentric && iBody == iNumBodies - 1) {
    CalculateBarycentricCoordinatesOfUnassignedBody(body, iNumBodies);
    if (system->bOutputLocalBaryCoords) {
      int iTmpBody = 0;
      for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
        if (body[iTmpBody].bExcludeFromBarycenter) {
          int jTmpBody = 0;
          for (jTmpBody = 0; jTmpBody < iNumBodies; jTmpBody++) {
            body[jTmpBody].dLocalBaryCartPos = malloc(3 * sizeof(double));
            body[jTmpBody].dLocalBaryCartVel = malloc(3 * sizeof(double));
          }
          if (system->bInputCommonBaryCoords) {
            fvCommonBarycenter2LocalBarycenter(body, iNumBodies);
          } else {
            int jTmpBody = 0;
            for (jTmpBody = 0; jTmpBody < iNumBodies; jTmpBody++) {
              fvAssignLocalBaryCart(body, jTmpBody);
            }
            if (system->bLocalInvPlane) {
              RotateToLocalInvariablePlane(body, system, iNumBodies);
            }
            fvLocalBarycenter2CommonBarycenter(body, iNumBodies);
            for (jTmpBody = 0; jTmpBody < iNumBodies; jTmpBody++) {
              fvAssignCartOutputs2BaryCart(body, jTmpBody);
            }           
          }             
          break;
        }
      }
    }   
  }
  

  // At this point dPosition and dVelocity are in BaryCart coordinates
  // One more possible transformation involves the invariable plane
  // The second condition means only do this transformation once.
  if (control->bInvPlane && iBody == iNumBodies - 1) {
    // Rotate barycentric coordinates to invariable plane
    inv_plane(body, system, iNumBodies);
    // Update dPosition and dVel variables to invariable plane
    for (int iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
      fvAssignCartOutputs2BaryCart(body, iTmpBody);
    }
  }
}

void VerifyOutputCoords(BODY *body, FILES *files, OUTPUT *output, int iBody) {
  int iCol, iValue; // for loop indices  
  // Arrays that identify the output variables
  int iBARYCART_Outputs[6] = {OUT_BARYPOSX, OUT_BARYPOSY, OUT_BARYPOSZ, 
                            OUT_BARYVELX, OUT_BARYVELY, OUT_BARYVELZ};

  int iHELIOCART_Outputs[6] = {OUT_HELIOPOSX, OUT_HELIOPOSY, OUT_HELIOPOSZ, 
                            OUT_HELIOVELX, OUT_HELIOVELY, OUT_HELIOVELZ};

  int iHELIOELEMS_Outputs[10] = {OUT_HELIOSEMI, OUT_HELIOECC, OUT_HELIOINC, OUT_HELIOLONGP,
                            OUT_HELIOLONGA, OUT_HELIOARGP, OUT_HELIOMEANA, OUT_HELIOMEANL,
                            OUT_HELIOECCA, OUT_HELIOHYPA};

  int iBARYELEMS_Outputs[15] = {OUT_BARYSEMI, OUT_BARYECC, OUT_BARYINC, OUT_BARYLONGP,
                            OUT_BARYLONGA, OUT_BARYARGP, OUT_BARYMEANA, OUT_BARYMEANL,
                            OUT_BARYECCA, OUT_BARYHYPA, OUT_BARYMU, OUT_BARYECCSQ, 
                            OUT_BARYMEANMOTION, OUT_BARYORBPERIOD, OUT_BARYSINC};
  // Should be defined as something before being summed
  files->Outfile[iBody].iOutputCoordBit = 1; // Identifies output variable's coordinates 
  
  // for loops find variables in the .in files
  for (iCol = 0;  iCol < files->Outfile[iBody].iNumCols; iCol++) {
    for (iValue = 0; iValue < NumElements(iBARYCART_Outputs); iValue++) {
      if (memcmp(files->Outfile[iBody].caCol[iCol], output[iBARYCART_Outputs[iValue]].cName,
                        strlen(output[iBARYCART_Outputs[iValue]].cName)) == 0) {
          files->Outfile[iBody].iOutputCoordBit += BARYCART; // BARYCART output found
          iCol = files->Outfile[iBody].iNumCols; // breaks out of nested for loop
          break;                          
      }
    }
  }
  for (iCol = 0;  iCol < files->Outfile[iBody].iNumCols; iCol++) {
    for (iValue = 0; iValue < NumElements(iHELIOCART_Outputs); iValue++) {
      if (memcmp(files->Outfile[iBody].caCol[iCol], output[iHELIOCART_Outputs[iValue]].cName,
                        strlen(output[iHELIOCART_Outputs[iValue]].cName)) == 0) {
          files->Outfile[iBody].iOutputCoordBit += HELIOCART; // HELIOCART output found
          iCol = files->Outfile[iBody].iNumCols; // breaks out of nested for loop
          break;                          
      }
    }
  }
  for (iCol = 0;  iCol < files->Outfile[iBody].iNumCols; iCol++) {
    for (iValue = 0; iValue < NumElements(iHELIOELEMS_Outputs); iValue++) {
      if (memcmp(files->Outfile[iBody].caCol[iCol], output[iHELIOELEMS_Outputs[iValue]].cName,
                        strlen(output[iHELIOELEMS_Outputs[iValue]].cName)) == 0) {
          files->Outfile[iBody].iOutputCoordBit += HELIOELEMS; // HELIOELEMS output found
          iCol = files->Outfile[iBody].iNumCols; // breaks out of nested for loop
          break;                          
      }
    }
  }
  for (iCol = 0;  iCol < files->Outfile[iBody].iNumCols; iCol++) {
    for (iValue = 0; iValue < NumElements(iBARYELEMS_Outputs); iValue++) {
      if (memcmp(files->Outfile[iBody].caCol[iCol], output[iBARYELEMS_Outputs[iValue]].cName,
                        strlen(output[iBARYELEMS_Outputs[iValue]].cName)) == 0) {
          files->Outfile[iBody].iOutputCoordBit += BARYELEMS; // BARYELEMS output found
          iCol = files->Outfile[iBody].iNumCols; // breaks out of nested for loop
          break;                          
      }
    }
  }
}

void CoordinateMessage(BODY *body, CONTROL *control, int iBody, char *cMessage) {
  if (iBody == control->Evolve.iNumBodies - 1){
        printf(cMessage);
      }
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


  VerifyInputCoords(body, control, options, system, iBody);
  VerifyOutputCoords(body, files, output, iBody);

  // VerifyGM(body,control);

  control->fnForceBehavior[iBody][iModule]   = &fnForceBehaviorSpiNBody;
  control->fnPropsAux[iBody][iModule]        = &PropsAuxSpiNBody;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopySpiNBody;

   
  if (system->bBarycentric) {
    //if (iBody == control->Evolve.iNumBodies - 1){
    //  printf("Using barycentric coordinates.\n");
    //}
    CoordinateMessage(body, control, iBody, "Using barycentric coordinates.\n");
    if (body[iBody].bUseOrbParams) {
      printf("Using orbital elements.\n"); // INSIDE BARYELEMS
      //fvBaryCart2BaryOrbElems(body, control->Evolve.iNumBodies, iBody);
    } else {
      printf("Using Cartesian coordinates.\n"); // INSIDE BARYCART
      

    }  
  } else {
    printf("Using heliocentric coordinates.\n");
    //fvBaryCart2HelioCart(body, iBody);
    //fvAssignBaryCart2HelioCart(body, iBody);
    if (body[iBody].bUseOrbParams) {
      printf("Using orbital elements.\n"); // INSIDE HELIOELEMS
      //convertHelioElems2BaryCart(body, control, system);
    } else {
      int jBody = 0;
      for (jBody = 0; jBody < control->Evolve.iNumBodies; jBody++) {
        //fvHelioCart2BaryCart(body, control->Evolve.iNumBodies, jBody);
      }
      printf("Using Cartesian coordinates.\n"); // INSIDE HELIOCART
      //convertHelioCart2BaryCart(body, control, system);
    }
  }

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

double LimitRounding(double x) {
  /* 
  Machine precision can round inaccurate values that eventually become present
  the more calculations vplanet makes. 
  */
  if (fabs(x) < 1e-15) {
    x = 0.0;
  }
  return x;
}

double TotalMass(BODY *body, int iNumBodies) {
  double dTmpTotalMass = 0.0;
  int iTmpBody = 0;
  for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
    dTmpTotalMass += body[iTmpBody].dMass;
  }
  return dTmpTotalMass;
}

double SumOfBodyMassesExcludedFromBarycenter(BODY *body, int iNumBodies) {
  double dSumOfBodyMassesExcluded = 0.0;
  int iTmpBody = 0;
  for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
    if (body[iTmpBody].bExcludeFromBarycenter) {
      dSumOfBodyMassesExcluded += body[iTmpBody].dMass;
    }
  }
  return dSumOfBodyMassesExcluded;
}

double TotalBarycentricMass(BODY *body, int iNumBodies) {
  return TotalMass(body, iNumBodies) - SumOfBodyMassesExcludedFromBarycenter(body, iNumBodies);
}

double CalculateBarycentricFactor(BODY *body, int iNumBodies, int iBody) {
  double dBarycentricFactor = 1.0;
  double dTotalBarycentricMass = TotalBarycentricMass(body, iNumBodies);
  dBarycentricFactor = cube(1.0 - body[iBody].dMass / dTotalBarycentricMass);
  return dBarycentricFactor;
}

double CalculateCommonBarycentricFactor(BODY *body, int iNumBodies, int iBody) {
  double dCommonBarycentricFactor = 1.0;
  double dTotalMass = TotalMass(body, iNumBodies);
  dCommonBarycentricFactor = cube(1.0 - body[iBody].dMass / dTotalMass);
  return dCommonBarycentricFactor;
}

double CalculateBarycentricGravitationalParameter(BODY *body, int iNumBodies, int iBody) {
  double dTotalBarycenterMass = TotalBarycentricMass(body, iNumBodies);
  double dTotalContributingMass = dTotalBarycenterMass;
  double dBarycentricFactor = 1.0;

  if (body[iBody].bExcludeFromBarycenter) {
    dTotalContributingMass += body[iBody].dMass;
  } else {
    dBarycentricFactor = CalculateBarycentricFactor(body, iNumBodies, iBody);
  }
  double dGravitationalParameter = BIGG * dBarycentricFactor * dTotalContributingMass;
  return dGravitationalParameter;
}

double CalculateCommonBarycentricGravitationalParameter(BODY *body, int iNumBodies, int iBody) {
  double dCommonBarycenterMass = TotalMass(body, iNumBodies);
  double dBarycentricFactor = CalculateCommonBarycentricFactor(body, iNumBodies, iBody);
  double dLocalGravitationalParameter = BIGG * dBarycentricFactor * dCommonBarycenterMass;
  return dLocalGravitationalParameter;
}

double CalculateTrigFunctionEdgeCase(double dMinimumValue, double dTrigfunction) {
  double dResult = dTrigfunction;
  if (fabs(dTrigfunction) < dMinimumValue) {
    dResult = 0.0;
  }
  if (fabs(fabs(dTrigfunction) - 1.0) < dMinimumValue) {
    dResult = sign(dTrigfunction);
  }
  return dResult;
}

void CalculateEulerAngleTrigFunctions(double dMinimumValue, 
                                      double dInclination, double dLongitudeAscendingNode, double dArgumentPericenter,
                                      double *dSinInclination, double *dCosInclination,  
                                      double *dSinLongitudeAscendingNode, double *dCosLongitudeAscendingNode,  
                                      double *dSinArgumentPericenter, double *dCosArgumentPericenter) {
  double dMaximumValue = PI - dMinimumValue;
  *dSinArgumentPericenter = CalculateTrigFunctionEdgeCase(dMinimumValue, LimitRounding(sin(dArgumentPericenter)));
  *dCosArgumentPericenter = CalculateTrigFunctionEdgeCase(dMinimumValue, LimitRounding(cos(dArgumentPericenter)));

  if (dMinimumValue < dInclination && dInclination < dMaximumValue) {
    *dSinInclination = CalculateTrigFunctionEdgeCase(dMinimumValue, LimitRounding(sin(dInclination))); 
    *dCosInclination = CalculateTrigFunctionEdgeCase(dMinimumValue, LimitRounding(cos(dInclination)));

    *dSinLongitudeAscendingNode = CalculateTrigFunctionEdgeCase(dMinimumValue, LimitRounding(sin(dLongitudeAscendingNode)));
    *dCosLongitudeAscendingNode = CalculateTrigFunctionEdgeCase(dMinimumValue, LimitRounding(cos(dLongitudeAscendingNode)));                                            
  } else if (fabs(dInclination - PI) < dMinimumValue) {
    *dCosInclination = -1.0;
  }
}

void OrbElems2Cart(ELEMS elems, double *dPos, double *dVel) {
  double dEccA, dCosEccA, dSinEccA, dHypA, dCoshHypA, dSinhHypA, 
          dL1, dM1, dN1, dL2, dM2, dN2, dSemi, dEcc, 
          dMu, mTotal, dBaryRelation, dXi, dEta, dVelScl;
  double dSini = 0.0, dCosi = 1.0, dSinLongA = 0.0, dCosLongA = 1.0, 
          dSinArgP = 0.0, dCosArgP = 1.0;
  // Note: Improved work can be made by introducing parabolic orbits.
  /*
  Most of the if statements below were made in consequence of the machine 
  precision errors present. If a future developer fixes these tiny errors,
  then the if statements would not be necessary.
  */
  double dMinimumValue = 1.0e-15;
  CalculateEulerAngleTrigFunctions(dMinimumValue, elems.dInc, elems.dLongA, elems.dArgP, 
                                    &dSini, &dCosi, &dSinLongA, &dCosLongA, &dSinArgP, &dCosArgP);

  dSemi = elems.dSemi;
  dEcc = elems.dEcc;
  if (dEcc > 1) {
    // By convention the semi-major axis is negative in unbound orbits
    dSemi = -dSemi;
  }
  dMu = elems.dMu;
  

  // Solving Kepler's Equation

  // Matrix transformation angles
  dL1 = dCosLongA * dCosArgP - dSinLongA * dSinArgP * dCosi;
  dM1 = dSinLongA * dCosArgP + dCosLongA * dSinArgP * dCosi;
  dN1 = dSinArgP * dSini;

  dL2 = -dCosLongA * dSinArgP - dSinLongA * dCosArgP * dCosi;
  dM2 = -dSinLongA * dSinArgP + dCosLongA * dCosArgP * dCosi;
  dN2 = dCosArgP * dSini;

  if (dEcc < 1) {
    dEccA = elems.dEccA;

    dCosEccA = cos(dEccA);
    dSinEccA = sin(dEccA);

    dXi = dSemi * (dCosEccA - dEcc);
    dEta = dSemi * pow(1. - dEcc * dEcc, 0.5) * dSinEccA;
  }
  if (dEcc > 1) {
    dHypA = elems.dHypA;

    dCoshHypA = cosh(dHypA);
    dSinhHypA = sinh(dHypA);

    dXi = dSemi * (dCoshHypA - dEcc);
    dEta = -dSemi * pow(dEcc * dEcc - 1., 0.5) * dSinhHypA;
  }

  //Defining Cartesian Coordinates
  dPos[0] = dL1 * dXi + dL2 * dEta;
  dPos[1] = dM1 * dXi + dM2 * dEta;
  dPos[2] = dN1 * dXi + dN2 * dEta;

  if (dEcc < 1) {
    dVelScl = pow(dMu * dSemi / dot(dPos, dPos), 0.5); // Time derivative of dEccA times semi-major axis
    dXi = -dVelScl * dSinEccA; // x-component of velocity
    dEta = dVelScl* pow(1. - dEcc * dEcc, 0.5) * dCosEccA; // y-component of velocity
  }
  if (dEcc > 1) {
    dVelScl = pow(-dMu * dSemi / dot(dPos, dPos), 0.5); // Time derivative of dHypA times semi-major axis
    dXi = -dVelScl * dSinhHypA; // x-component of velocity
    dEta = dVelScl * pow(dEcc * dEcc - 1.0, 0.5) * dCoshHypA; // y-component of velocity
  }
  dVel[0] = dL1 * dXi + dL2 * dEta;
  dVel[1] = dM1 * dXi + dM2 * dEta;
  dVel[2] = dN1 * dXi + dN2 * dEta;
  
}

double GetEccAnom(ELEMS elems) {
  double dMeanA, dEccA; 
  dMeanA = positiveModulus2PI(elems.dMeanA);

  if (dMeanA == 0) {
    dEccA = 0; // Trivial solution for the Eccentric Anomaly
  } else if (dMeanA == PI) {
    dEccA = PI; // Trivial solution for the Eccentric Anomaly
  } else {
    double dEcc, dSinMeanA, dLow, dUp, dSinEccAe, dCosEccAe,
            f, fp, fpp, fppp, dx, dNext;
    dEcc = elems.dEcc;
    dSinMeanA = sin(dMeanA); 
    if (dSinMeanA > 0.0) {
      // 0.85 best value for numerical convergence according to Danby 1982 
      dEccA = dMeanA + 0.85 * dEcc; // A good guess for Newton-Rhapson method
      } else {
      dEccA = dMeanA - 0.85 * dEcc;
    }
    dLow = -2. * PI; 
    dUp = 2. * PI; // I do not understand these values yet...

    int i;
    for (i = 0; i < 32; i++) {
      dSinEccAe = sin(dEccA) * dEcc;
      dCosEccAe = cos(dEccA) * dEcc;
      f = dEccA - dSinEccAe - dMeanA; // function used for Newton–Raphson method
      fp = 1.0 - dCosEccAe; // p stands for prime. Derivative of f w.r.t dEccA
      fpp = dSinEccAe; // 2nd derivative
      fppp = dCosEccAe; // 3rd derivative

      if (f > 0.0) {
        dUp = dEccA;
      } else {
        dLow = dEccA;
      }

      dx = -f / fp; //First iteration of Newton-Rhapson 
      dx = -f / (fp + dx * fpp / 2.); //Second iteration of Newton-Rhapson 
      dx = -f / (fp + dx * fpp / 2. + dx * dx * fppp / 6.); //Third iteration of Newton-Rhapson 

      dNext = dEccA + dx;
      if (dEccA == dNext) {
        break;
      }
      if ((dNext > dLow) && (dNext < dUp)) {
        dEccA = dNext;
      } else {
        dEccA = (dLow + dUp) / 2.;
      }
      if ((dEccA == dLow) || (dEccA == dUp)) {
        break;
      }

      if (i > 31) {
        printf("Kepler solution failed\n");
        exit(1);
      } 
    }
  }
  return dEccA;
}

double GetHypAnom(ELEMS elems) {
  double dMeanA, dHypA;
  dMeanA = elems.dMeanA;
  if (dMeanA == 0.0) { // Trivial solution for the hyperbolic Hyperbolic Anomaly.
    dHypA = 0.0;
  } else {
    double dEcc, dSignMeanA, dLow, dUp, dSinhHypAe, dCoshHypAe,
            f, fp, fpp, fppp, dx, dNext;
    dEcc = elems.dEcc;
    // Found in Danby 1962 and other sources. They both claim that 1.8 is the best estimated guess
    // The Guess for dHypA is large but the constant accomodates for small values
    dSignMeanA = fiSign(dMeanA);
    if (dSignMeanA == 0) { // Can become 0 if fabs(dMeanA) < EPS defined in vplanet.h
      // Dervied expression for 0 < dHypA << 1
      //dHypA = 3 * (dEcc - 1.0) / (2 * dMeanA);
      dHypA = 0; // Defined as zero for now
    } else {
      dHypA = dSignMeanA * log(dSignMeanA * (2*dMeanA / dEcc) + 1.8);
    }

    int i;
    for (i = 0; i < 32; i++) {
      dSinhHypAe = sinh(dHypA) * dEcc;
      dCoshHypAe = cosh(dHypA) * dEcc;

      f = dSinhHypAe - dHypA - dMeanA; // function used for Newton–Raphson method
      fp = dCoshHypAe - 1.0; // p stands for prime. Derivative of f w.r.t dEccA
      fpp = dSinhHypAe; // 2nd derivative
      fppp = dCoshHypAe; // 3rd derivative


      dx = -f / fp; //First iteration of Newton-Rhapson 
      dx = -f / (fp + dx * fpp / 2.); //Second iteration of Newton-Rhapson 
      dx = -f / (fp + dx * fpp / 2. + dx * dx * fppp / 6.); //Third iteration of Newton-Rhapson 

      dNext = dHypA + dx;
      if (dHypA == dNext) { // If convergence, stop iterating
        break;
      } else {
        dHypA = dNext; // Goes to next iteration
      }
      
      if (i > 31) {
        printf("Kepler soln failed\n");
        exit(1);
      }
    }

  }
  return dHypA;
}

double GetMeanA(ELEMS elems) {
  double dMeanA, dEcc, dEccA, dHypA;
  dEcc = elems.dEcc;
  dEccA = elems.dEccA;
  dHypA = elems.dHypA;

  if (dEcc < 1.0) {
    dMeanA = dEccA - dEcc * sin(dEccA);
  }
  if (dEcc > 1.0) {
    dMeanA = dEcc * sinh(dHypA) - dHypA;
  }
  // Add options for dEcc == 1.0, question? I guess that's up for the next developer.
  return dMeanA;
}

void AssignBody2Elems(BODY *body, ELEMS elems, int iBody) {
  // use elems struct to generalize coordinates of different orbital elements
  elems.dSemi = body[iBody].dSemi;
  elems.dEcc = body[iBody].dEcc;
  elems.dInc = body[iBody].dInc;
  elems.dArgP = body[iBody].dArgP;
  elems.dLongP = body[iBody].dLongP;
  elems.dLongA = body[iBody].dLongA;
  elems.dMeanA = body[iBody].dMeanA;
  elems.dMeanL = body[iBody].dMeanL;
  elems.dEccA = GetEccAnom(elems); 
}

void fvBaryOrbElems2BaryCart(BODY *body, int iNumBodies, int iBody) {
  ELEMS elems;
  double *dPos, *dVel;
  dPos = malloc(3 * sizeof(double));
  dVel = malloc(3 * sizeof(double));
  // Switching and assigning elems struct
  // AssignBody2Elems(body, elems, iBody); // This function doesn't work. Help
  // use elems struct to generalize coordinates of different orbital elements
  elems.dSemi = body[iBody].dSemi;
  elems.dEcc = body[iBody].dEcc;
  elems.dInc = body[iBody].dInc;
  elems.dArgP = body[iBody].dArgP;
  elems.dLongP = body[iBody].dLongP;
  elems.dLongA = body[iBody].dLongA;
  elems.dMeanA = body[iBody].dMeanA;
  elems.dMeanL = body[iBody].dMeanL;
  if (elems.dEcc < 1) {
    elems.dEccA = GetEccAnom(elems);
  }
  if (elems.dEcc > 1) {
    elems.dHypA = GetHypAnom(elems);
  }

  body[iBody].dBaryMu = CalculateBarycentricGravitationalParameter(body, iNumBodies, iBody);
  elems.dMu = body[iBody].dBaryMu;
  OrbElems2Cart(elems, dPos, dVel);
  int i = 0;
  for (i = 0; i < 3; i++) {
    body[iBody].dBCartPos[i] = dPos[i];
    body[iBody].dBCartVel[i] = dVel[i];
  }

  free(dPos);
  free(dVel);  
}

void fvHelioOrbElems2HelioCart(BODY *body, int iNumBodies, int iBody) {
  ELEMS elems;
  double *dPos, *dVel, mTotal, dHelioMu;
  dPos = malloc(3 * sizeof(double));
  dVel = malloc(3 * sizeof(double));
  // Code below tries to replicate the above commented code
  // Note to self: Keep a similar format to this function but in vplanet units
  if (iBody == 0) {
    int i = 0;
    for (i = 0; i < 3; i++) {
    body[iBody].dHCartPos[i] = 0;
    body[iBody].dHCartVel[i] = 0;
    }
  } else { 
    // Switching and assigning elems struct
    // AssignBody2Elems(body, elems, iBody); // So far this function doesn't work. Help!
    // use elems struct to generalize coordinates of different orbital elements
    elems.dSemi = body[iBody].dSemi;
    elems.dEcc = body[iBody].dEcc;
    elems.dInc = body[iBody].dInc;
    elems.dArgP = body[iBody].dArgP;
    elems.dLongP = body[iBody].dLongP;
    elems.dLongA = body[iBody].dLongA;
    elems.dMeanA = body[iBody].dMeanA;
    elems.dMeanL = body[iBody].dMeanL;
    if (elems.dEcc < 1) {
      elems.dEccA = GetEccAnom(elems);
    }
    if (elems.dEcc > 1) {
      elems.dHypA = GetHypAnom(elems);
    }    

    dHelioMu = BIGG * (body[0].dMass + body[iBody].dMass);
    elems.dMu = dHelioMu;
    OrbElems2Cart(elems, dPos, dVel);
    int i = 0;
    for (i = 0; i < 3; i++) {
      body[iBody].dHCartPos[i] = dPos[i];
      body[iBody].dHCartVel[i] = dVel[i];
    }
  }
  free(dPos);
  free(dVel);
}

void fvHelioCart2BaryCart(BODY *body, int iNumBodies, int iBody) {
  double *dCenterOfMassPositionVector, *dCenterOfMassVelocityVector, dTotalBarycentricMass;
  dCenterOfMassPositionVector = malloc(3 * sizeof(double));
  dCenterOfMassVelocityVector = malloc(3 * sizeof(double));
  dTotalBarycentricMass = TotalBarycentricMass(body, iNumBodies);

  int i = 0;
  for (i = 0; i < 3; i++) {
    dCenterOfMassPositionVector[i] = 0;
    dCenterOfMassVelocityVector[i] = 0;
    int iTmpBody = 0;
    for (iTmpBody = 0; iTmpBody < iNumBodies; iTmpBody++) {
      if (!body[iTmpBody].bExcludeFromBarycenter) {
        dCenterOfMassPositionVector[i] += (body[iTmpBody].dMass * body[iTmpBody].dHCartPos[i] / dTotalBarycentricMass);
        dCenterOfMassVelocityVector[i] += (body[iTmpBody].dMass * body[iTmpBody].dHCartVel[i] / dTotalBarycentricMass);
      }
    }
  }
  for (i = 0; i < 3; i++) {
    body[iBody].dBCartPos[i] = body[iBody].dHCartPos[i] - dCenterOfMassPositionVector[i];
    body[iBody].dBCartVel[i] = body[iBody].dHCartVel[i] - dCenterOfMassVelocityVector[i];
  }

  free(dCenterOfMassPositionVector), free(dCenterOfMassVelocityVector);
}

void fvBaryCart2HelioCart(BODY *body, int iBody) {
  int i = 0;
  for (i = 0; i < 3; i++) {
    body[iBody].dHCartPos[i] = body[iBody].dBCartPos[i] - body[0].dBCartPos[i];
    body[iBody].dHCartVel[i] = body[iBody].dBCartVel[i] - body[0].dBCartVel[i];
  }
}

double CalculateSemiMajorAxis(double dMu,
                              double *dPositionVector, double *dVelocityVector, 
                              double dMagnitudePosition) {
  double dMagnitudeVelocitySquared = magnitudeSquared(dVelocityVector);
  double dSemiMajorAxis = 1 / (2 / dMagnitudePosition - dMagnitudeVelocitySquared / dMu);
  return dSemiMajorAxis;
}

void CalculateEccentricityVector(double dMu, 
                        double *dPositionVector, double *dVelocityVector, 
                        double *dSpecificAngularMomentum,
                        double *dEccentricityVector, 
                        double dMagnitudePosition) {
  // First term of Eccentricity vector calculated here ((v x h)/Mu)
  cross(dVelocityVector, dSpecificAngularMomentum, dEccentricityVector);

  // Second term of Eccentricity vector added here (subtract r-hat)
  int i = 0;
  for (i = 0; i < 3; i++) {
    // Divide by dMu here for the first term
    dEccentricityVector[i] /= dMu;
    // Introduce second term here to subtract from
    dEccentricityVector[i] -= dPositionVector[i] / dMagnitudePosition;
  }
}

double CalculateInclination(double *dSpecificAngularMomentumVector, 
                            double dMagnitudeSpecificAngularMomentum) {
  double dInclination = acos(dSpecificAngularMomentumVector[2] / dMagnitudeSpecificAngularMomentum);
  return dInclination;
}

double CalculateLongitudeAscendingNode(double dMinimumValue, 
                                      double *dSpecificAngularMomentumVector, 
                                      double dInclination) {
  double dLongitudeAscendingNode = 0.0;
  if (dMinimumValue < fabs(dInclination) && fabs(dInclination) < PI - dMinimumValue) {
    dLongitudeAscendingNode = atan2Continuous(dSpecificAngularMomentumVector[0], 
                                              -dSpecificAngularMomentumVector[1], 
                                              dMinimumValue);
  }
  return dLongitudeAscendingNode;
}

double SineAngleBetweenNodalVectorAndInputVector(double *dVector, double dMinimumValue, 
                                              double dInclination, double dLongitudeAscendingNode) {
  double dSinAngle = 0.0;
  double dMagnitudeVector = magnitude(dVector);
  if (dMinimumValue < fabs(dInclination) && fabs(dInclination) < PI - dMinimumValue) {
    /*
    Example case between Eccentricity Vector and Nodal Vector: Danby pg. 205
    Use 1st or 2nd component of equality n-hat x dVector = dMagntiudeVector * sin(dAngle) * h-hat
    where n is the nodal vector, h is the specific angular momentum, 
    and the -hat suffixes are unit vectors.
    */
    dSinAngle = dVector[2] / (dMagnitudeVector * sin(dInclination));
  } else {
    // Found using same equality as above using the z-component
    dSinAngle = dVector[1] / dMagnitudeVector; 
    if (fabs(dInclination) > PI - dMinimumValue) {
      dSinAngle *= -1;
    }
  }
  return dSinAngle;                                                      
}

double CalculateArgumentPericenter(double dMinimumValue, double *dSpecificAngularMomentumVector, 
                                  double *dEccentricityVector, double *dUnitNodalVector, 
                                  double dEccentricity, double dInclination, double dLongitudeAscendingNode) {
  // *** Solve for Argument of Pericenter ***
  double dArgumentPericenter = 0.0; // best to initialize the variable first
  // dArgP only exists if the orbit is eccentric
  if (dEccentricity > dMinimumValue) {
    // Finding Argument of Pericenter using the Eccentricity Vector: Danby pg. 205
    double dSinArgumentPericenter, dCosArgumentPericenter;
    dSinArgumentPericenter = SineAngleBetweenNodalVectorAndInputVector(dEccentricityVector, dMinimumValue, 
                                                                    dInclination, dLongitudeAscendingNode);
    dCosArgumentPericenter = dot(dUnitNodalVector, dEccentricityVector) / dEccentricity;
    dArgumentPericenter = atan2Continuous(dSinArgumentPericenter, dCosArgumentPericenter, dMinimumValue);
  }
  return dArgumentPericenter;
}

double CalculateTrueAnomaly(double dMinimumValue, double dMu, 
                            double *dPositionVector, double *dVelocityVector, double *dEccentricityVector,
                            double *dSpecificAngularMomentumVector, double *dUnitNodalVector, 
                            double dMagnitudePosition, double dMagnitudeSpecificAngularMomentum, 
                            double dEccentricity, double dInclination, double dLongitudeAscendingNode) {
  // *** Solve for True Anomaly ***
  double dTrueAnomaly = 0.0, dSinTrueAnomaly = 0.0, dCosTrueAnomaly = 1.0; // initializing variable
  if (dEccentricity < dMinimumValue) {
    dSinTrueAnomaly = SineAngleBetweenNodalVectorAndInputVector(dPositionVector, dMinimumValue, 
                                                              dInclination, dLongitudeAscendingNode);
    dCosTrueAnomaly = dot(dUnitNodalVector, dPositionVector) / dMagnitudePosition;
  } else { // Applies to non-zero dEccentricity and for all values of dInclination
    // Solving from dSinTrueAnomaly * h-hat = e-hat x r-hat simplifies to the expression below
    double dUnitPositionVectorDotVelocityVector = dot(dPositionVector, dVelocityVector) / dMagnitudePosition;
    dSinTrueAnomaly =  dUnitPositionVectorDotVelocityVector * dMagnitudeSpecificAngularMomentum / (dMu * dEccentricity);
    // Taking the dot product to find the cosine angle
    dCosTrueAnomaly = dot(dEccentricityVector, dPositionVector) / (dEccentricity * dMagnitudePosition);

    if (fabs(dCosTrueAnomaly) > 1.0 && fabs(dCosTrueAnomaly) - 1.0 < dMinimumValue) {
      dSinTrueAnomaly = 0.0;
      dCosTrueAnomaly = sign(dCosTrueAnomaly);
    }
  }

  // There are rounding errors at such precise values. We try to avoid dCosf > 1
  if (fabs(fabs(dCosTrueAnomaly) - 1) < dMinimumValue || fabs(dSinTrueAnomaly) < dMinimumValue) { 
    dCosTrueAnomaly = sign(dCosTrueAnomaly);
    dSinTrueAnomaly = 0.0;  
  }
  dTrueAnomaly = atan2Continuous(dSinTrueAnomaly, dCosTrueAnomaly, dMinimumValue);
  return dTrueAnomaly;
}

double CalculateEccentricAnomaly(double dMinimumValue, double dEccentricity, double dTrueAnomaly) {
  double dEccentricAnomaly = 0.0;
  double dSinEccentricAnomaly = 0.0, dCosEccentricAnomaly = 1.0;
  double dEccentricitySquared = dEccentricity * dEccentricity;
  double dSinTrueAnomaly = sin(dTrueAnomaly), dCosTrueAnomaly = cos(dTrueAnomaly);
  
  if (1.0 > dEccentricity && dEccentricity >= dMinimumValue) {
    dSinEccentricAnomaly = sqrt(1.0 - dEccentricitySquared) * dSinTrueAnomaly / (1.0 + dEccentricity * dCosTrueAnomaly);
    dCosEccentricAnomaly = (dCosTrueAnomaly + dEccentricity) / (1.0 + dEccentricity * dCosTrueAnomaly);
    if (fabs(fabs(dCosEccentricAnomaly) - 1.0) < dMinimumValue || fabs(dSinEccentricAnomaly) < dMinimumValue) {
      dCosEccentricAnomaly = sign(dCosEccentricAnomaly);
      dSinEccentricAnomaly = 0.0;
    }
    dEccentricAnomaly = atan2Continuous(dSinEccentricAnomaly, dCosEccentricAnomaly, dMinimumValue);

  } else if (dEccentricity < dMinimumValue) {
    dEccentricAnomaly = dTrueAnomaly;
  }
  return dEccentricAnomaly;
}

double CalculateHyperbolicAnomaly(double dEccentricity, double dTrueAnomaly) {
  double dHyperbolicAnomaly = 0.0;
  double dSinhHyperbolicAnomaly = 0.0;
  if (dEccentricity > 1.0) {
    double dEccentricitySquared = dEccentricity * dEccentricity;
    dSinhHyperbolicAnomaly = sqrt(dEccentricitySquared - 1) * sin(dTrueAnomaly) / (1.0 + dEccentricity * cos(dTrueAnomaly));
    dHyperbolicAnomaly = asinh(dSinhHyperbolicAnomaly);
  }
  return dHyperbolicAnomaly;
}

double CalculateMeanAnomaly(double dMinimumValue, double dEccentricity, double dTrueAnomaly) {
  double dMeanAnomaly = 0.0;
  // Using Kepler's Equation to find dMeanAnomaly
  if (dEccentricity < dMinimumValue) {
    dMeanAnomaly = dTrueAnomaly;
  } else if (1.0 > dEccentricity && dEccentricity >= dMinimumValue) {
    double dEccentricAnomaly = CalculateEccentricAnomaly(dMinimumValue, dEccentricity, dTrueAnomaly);
    dMeanAnomaly = dEccentricAnomaly - dEccentricity * sin(dEccentricAnomaly);
    dMeanAnomaly = positiveModulus2PI(dMeanAnomaly);
  } else if (1.0 < dEccentricity) {
    double dHyperbolicAnomaly = CalculateHyperbolicAnomaly(dEccentricity, dTrueAnomaly);
    dMeanAnomaly = dEccentricity * sinh(dHyperbolicAnomaly) - dHyperbolicAnomaly;
  }
  return dMeanAnomaly;
}

double CalculateLongitudePericenter(double dArgumentPericenter, double dLongitudeAscendingNode, double dEccentricity) {
  double dLongitudePericenter = dArgumentPericenter + dLongitudeAscendingNode;
  if (dEccentricity < 1.0) {
    dLongitudePericenter = positiveModulus2PI(dLongitudePericenter);
  }
  return dLongitudePericenter;
}

double CalculateMeanLongitude(double dMeanAnomaly, double dArgumentPericenter, 
                              double dLongitudeAscendingNode, double dEccentricity) {
  double dLongitudePericenter = dArgumentPericenter + dLongitudeAscendingNode;
  double dMeanLongitude = dMeanAnomaly + dLongitudePericenter;
  if (dEccentricity < 1.0) {
    dMeanLongitude = positiveModulus2PI(dMeanLongitude);
  }
  return dMeanLongitude;
}

double CalculateMeanMotion(double dMu, double dSemiMajorAxis) {
  double dMeanMotion = sqrt(dMu / fabs(cube(dSemiMajorAxis)));
  return dMeanMotion;
}

double CalculateOrbitalPeriod(double dMu, double dSemiMajorAxis) {
  double dMeanMotion = CalculateMeanMotion(dMu, dSemiMajorAxis);
  double dOrbitalPeriod;
  if (dSemiMajorAxis > 0) {
    dOrbitalPeriod = 2.0 * PI / dMeanMotion;
  }
  return dOrbitalPeriod;
}

ELEMS fvFindPoincareVariables(double dEccentricity, double dInclination, 
                              double dLongitudeAscendingNode, double dLongitudePericenter) {
  // These have not been tested for unbound orbits.
  ELEMS elems = {0};
  elems.dSinc = 0.5 * sin(dInclination);
  elems.dPinc = elems.dSinc * sin(dLongitudeAscendingNode);
  elems.dQinc = elems.dSinc * cos(dLongitudeAscendingNode);
  elems.dHecc = dEccentricity * sin(dLongitudePericenter);
  elems.dKecc = dEccentricity * cos(dLongitudePericenter);
  return elems;
}
void CreateCardinalUnitVector(int iDimension, int iNumDimensions, double *dUnitVector) {
  if (iDimension < iNumDimensions) {
    int i = 0;
    for (i = 0; i < iNumDimensions; i++) {
      if (i == iDimension) {
        dUnitVector[i] = 1.0;
      } else {
        dUnitVector[i] = 0.0;
      }
    }
  } else {
      printf("ERROR: Component iDimension higher than iNumDimensions.\n");
      exit(1);
  }
}
void CalculateUnitNodalVector(double *dSpecificAngularMomentumVector, double *dUnitNodalVector) {
  double *dUnitVectorZ = malloc(3 * sizeof(double));
  double *dNodalVector = malloc(3 * sizeof(double));
  int iDirectionX = 0, iDirectionZ = 2;
  int iNumDimensions = 3;
  CreateCardinalUnitVector(iDirectionZ, iNumDimensions, dUnitVectorZ);
  cross(dUnitVectorZ, dSpecificAngularMomentumVector, dNodalVector);
  double dMagnitudeNodalVector = magnitude(dNodalVector);
  if (dMagnitudeNodalVector > 0) { // Nodal Vector exists
    int i = 0;
    for (i = 0; i < iNumDimensions; i++) {
      dUnitNodalVector[i] = dNodalVector[i] / dMagnitudeNodalVector; 
    }
  } else { // Nodal Vector doesn't exist, but origin is placed in the x-direction
    CreateCardinalUnitVector(iDirectionX, iNumDimensions, dUnitNodalVector);
  }
  free(dUnitVectorZ);
  free(dNodalVector);
}

void ApproximateToStandardBasisVector(double dMinimumValue, double *dVector) {
  int i = 0;
  int iNonZeroComponent = 0;
  int bUnitComponentFound = 0;
  double dMagnitudeVector = magnitude(dVector);
  for (i = 0; i < 3; i++) {
    double dAbsoluteRatio = fabs(dVector[i] / dMagnitudeVector);    
    if (dAbsoluteRatio < dMinimumValue) {
      dVector[i] = 0;
    }
    if (1.0 - dMinimumValue <= dAbsoluteRatio && dAbsoluteRatio <= 1.0) {
      iNonZeroComponent = i;
      bUnitComponentFound += 1;
      break;
    }
  }
  for (i = 0; i < 3; i++) {
    if (i != iNonZeroComponent && bUnitComponentFound) {
      dVector[i] = 0;
    }
  }  
}

ELEMS fvCart2OrbElems(double dMinimumValue, double dMu, double *dPositionVector, double *dVelocityVector) {
  double *dSpecificAngularMomentumVector, *dEccentricityVector, *dUnitNodalVector; // vectors

  // mallocing empty vectors
  dSpecificAngularMomentumVector = malloc(3 * sizeof(double));
  dEccentricityVector = malloc(3 * sizeof(double));
  dUnitNodalVector = malloc(3 * sizeof(double));
  
  double dMagnitudePosition = magnitude(dPositionVector);

  cross(dPositionVector, dVelocityVector, dSpecificAngularMomentumVector);
  ApproximateToStandardBasisVector(dMinimumValue, dSpecificAngularMomentumVector);

  double dMagnitudeSpecificAngularMomentum = magnitude(dSpecificAngularMomentumVector);

  CalculateEccentricityVector(dMu, dPositionVector, dVelocityVector, 
                              dSpecificAngularMomentumVector, dEccentricityVector, 
                              dMagnitudePosition);
  ApproximateToStandardBasisVector(dMinimumValue, dEccentricityVector);                              
  CalculateUnitNodalVector(dSpecificAngularMomentumVector, dUnitNodalVector);
  
  // We define all orbital elements to ELEMS
  ELEMS elems = {0};
  // *** Solve the orbital elements ***
  elems.dSemi = CalculateSemiMajorAxis(dMu, dPositionVector, dVelocityVector, dMagnitudePosition);

  elems.dEcc = magnitude(dEccentricityVector);

  elems.dInc = CalculateInclination(dSpecificAngularMomentumVector, dMagnitudeSpecificAngularMomentum); 

  elems.dLongA = CalculateLongitudeAscendingNode(dMinimumValue, dSpecificAngularMomentumVector, elems.dInc);

  elems.dArgP = CalculateArgumentPericenter(dMinimumValue, dSpecificAngularMomentumVector, 
                                      dEccentricityVector, dUnitNodalVector, 
                                      elems.dEcc, elems.dInc, elems.dLongA);

  elems.dTrueA = CalculateTrueAnomaly(dMinimumValue, dMu, 
                                dPositionVector, dVelocityVector, dEccentricityVector, 
                                dSpecificAngularMomentumVector, dUnitNodalVector, 
                                dMagnitudePosition, dMagnitudeSpecificAngularMomentum, 
                                elems.dEcc, elems.dInc, elems.dLongA);

  elems.dMeanA = CalculateMeanAnomaly(dMinimumValue, elems.dEcc, elems.dTrueA);

  free(dSpecificAngularMomentumVector);
  free(dEccentricityVector);
  free(dUnitNodalVector);
  return elems;
}

void fvBaryCart2BaryOrbElems(BODY *body, SYSTEM *system, int iNumBodies, int iBody) {
  ELEMS elems = {0};
  double *dPos, *dVel;
  dPos = malloc(3 * sizeof(double));
  dVel = malloc(3 * sizeof(double));
  if (system->bOutputLocalBaryCoords) {
    body[iBody].dBaryMu = CalculateBarycentricGravitationalParameter(body, iNumBodies, iBody);
    int i = 0;
    for (i = 0; i < 3; i++) {
      dPos[i] = body[iBody].dLocalBaryCartPos[i];
      dVel[i] = body[iBody].dLocalBaryCartVel[i];
    }
  } else {
    body[iBody].dBaryMu = CalculateCommonBarycentricGravitationalParameter(body, iNumBodies, iBody);
    int i = 0;
    for (i = 0; i < 3; i++) {
      dPos[i] = body[iBody].dBCartPos[i];
      dVel[i] = body[iBody].dBCartVel[i];
    }
  }
  
  // Need to declare min value to avoid inaccurate outputs
  double dMinimumValue = 1.0e-15;
  // Finding the orbital elements here
  elems = fvCart2OrbElems(dMinimumValue, body[iBody].dBaryMu, dPos, dVel);


  // Assigning elems variables to proper body[iBody] variables
  body[iBody].dBarySemi = elems.dSemi;
  body[iBody].dBaryEcc = elems.dEcc;
  body[iBody].dBaryInc = elems.dInc;
  body[iBody].dBaryArgP = LimitRounding(elems.dArgP);
  body[iBody].dBaryLongA = elems.dLongA;
  body[iBody].dBaryMeanA = elems.dMeanA;

  body[iBody].dBaryLongP = CalculateLongitudePericenter(elems.dArgP, elems.dLongA, elems.dEcc);
  body[iBody].dBaryMeanL = CalculateMeanLongitude(elems.dMeanA, elems.dArgP, elems.dLongA, elems.dEcc);

  if (elems.dEcc < 1.0) {
    body[iBody].dBaryEccA = CalculateEccentricAnomaly(dMinimumValue, elems.dEcc, elems.dTrueA);
  } else {
    body[iBody].dBaryHypA = CalculateHyperbolicAnomaly(elems.dEcc, elems.dTrueA);
  }

  body[iBody].dBaryMeanMotion = CalculateMeanMotion(body[iBody].dBaryMu, elems.dSemi);
  body[iBody].dBaryOrbPeriod = CalculateOrbitalPeriod(body[iBody].dBaryMu, elems.dSemi);

  
  /* For the future developer wanting to create these
  body[iBody].dBarySinc = elems.dSinc; 
  body[iBody].dBaryPinc = elems.dPinc;
  body[iBody].dBaryQinc = elems.dQinc;
  body[iBody].dBaryHecc = elems.dHecc;
  body[iBody].dBaryKecc = elems.dKecc;
  */

  free(dPos);
  free(dVel);
}

void fvBaryCart2HelioOrbElems(BODY *body, int iNumBodies, int iBody) {
  if (iBody == 0) { // Assign all of the central body's elements to zero
    body[iBody].dSemi = 0;
    body[iBody].dEcc = 0;
    body[iBody].dInc = 0;
    body[iBody].dArgP = 0;
    body[iBody].dLongA = 0;
    body[iBody].dMeanA = 0;

    body[iBody].dMeanMotion = 0;
    body[iBody].dOrbPeriod = 0;

    body[iBody].dSinc = 0;
    body[iBody].dPinc = 0;
    body[iBody].dQinc = 0;
    body[iBody].dHecc = 0;
    body[iBody].dKecc = 0;
  } else {
    ELEMS elems = {0};
    fvBaryCart2HelioCart(body, iBody);

    double dMu, *dPos, *dVel;
    dPos = malloc(3 * sizeof(double));
    dVel = malloc(3 * sizeof(double));

    int i = 0;
    for (i = 0; i < 3; i++) {
      dPos[i] = body[iBody].dHCartPos[i];
      dVel[i] = body[iBody].dHCartVel[i];
    }

    dMu = BIGG * (body[iBody].dMass + body[0].dMass);
    // Need to declare min value to avoid inaccurate outputs
    double dMinimumValue = 1.0e-15;
    // Finding the orbital elements here
    elems = fvCart2OrbElems(dMinimumValue, dMu, dPos, dVel);


    // Assigning elems variables to proper body[iBody] variables
    body[iBody].dSemi = elems.dSemi;
    body[iBody].dEcc = elems.dEcc;
    body[iBody].dInc = elems.dInc;
    body[iBody].dArgP = elems.dArgP;
    body[iBody].dLongA = elems.dLongA;
    body[iBody].dMeanA = elems.dMeanA;
 
    body[iBody].dLongP = CalculateLongitudePericenter(elems.dArgP, elems.dLongA, elems.dEcc);
    body[iBody].dMeanL = CalculateMeanLongitude(elems.dMeanA, elems.dArgP, elems.dLongA, elems.dEcc);

    if (elems.dEcc < 1.0) {
      body[iBody].dEccA = CalculateEccentricAnomaly(dMinimumValue, elems.dEcc, elems.dTrueA);
    } else {
      body[iBody].dHypA = CalculateHyperbolicAnomaly(elems.dEcc, elems.dTrueA);
    }    

    body[iBody].dMeanMotion = CalculateMeanMotion(dMu, elems.dSemi);
    body[iBody].dOrbPeriod = CalculateOrbitalPeriod(dMu, elems.dSemi);

    body[iBody].dSinc = elems.dSinc;
    body[iBody].dPinc = elems.dPinc;
    body[iBody].dQinc = elems.dQinc;
    body[iBody].dHecc = elems.dHecc;
    body[iBody].dKecc = elems.dKecc;

    free(dPos);
    free(dVel);
  }
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

//HelioCart writeout

void WriteHelioPosX(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  //*dTmp = body[iBody].dHelioPosX;
  *dTmp = body[iBody].dHCartPos[0];
  strcpy(cUnit, "");
}

void WriteHelioPosY(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  //*dTmp = body[iBody].dHelioPosY;
  *dTmp = body[iBody].dHCartPos[1];
  strcpy(cUnit, "");
}

void WriteHelioPosZ(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  //*dTmp = body[iBody].dHelioPosZ;
  *dTmp = body[iBody].dHCartPos[2];
  strcpy(cUnit, "");
}

void WriteHelioVelX(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char cUnit[]) {
  //*dTmp = body[iBody].dHelioVelX;
  *dTmp = body[iBody].dHCartVel[0];
  strcpy(cUnit, "");
}

void WriteHelioVelY(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char cUnit[]) {
  //*dTmp = body[iBody].dHelioVelY;
  *dTmp = body[iBody].dHCartVel[1];
  strcpy(cUnit, "");
}

void WriteHelioVelZ(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char cUnit[]) {
  //*dTmp = body[iBody].dHelioVelZ;
  *dTmp = body[iBody].dHCartVel[2];
  strcpy(cUnit, "");
}

//BaryCart writeout

void WriteBaryPosX(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  //*dTmp = body[iBody].dPositionX;
  if (system->bOutputLocalBaryCoords) {
    *dTmp = body[iBody].dLocalBaryCartPos[0];
  } else {
    *dTmp = body[iBody].dBCartPos[0];
  }
  strcpy(cUnit, "");
}

void WriteBaryPosY(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  //*dTmp = body[iBody].dPositionY;
  if (system->bOutputLocalBaryCoords) {
    *dTmp = body[iBody].dLocalBaryCartPos[1];
  } else {
    *dTmp = body[iBody].dBCartPos[1];
  }
  strcpy(cUnit, "");
}

void WriteBaryPosZ(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  //*dTmp = body[iBody].dPositionZ;
  if (system->bOutputLocalBaryCoords) {
    *dTmp = body[iBody].dLocalBaryCartPos[2];
  } else {
    *dTmp = body[iBody].dBCartPos[2];
  }
  strcpy(cUnit, "");
}

void WriteBaryVelX(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char cUnit[]) {
  //*dTmp = body[iBody].dVelX;
  if (system->bOutputLocalBaryCoords) {
    *dTmp = body[iBody].dLocalBaryCartVel[0];
  } else {
    *dTmp = body[iBody].dBCartVel[0];
  }  
  strcpy(cUnit, "");
}

void WriteBaryVelY(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char cUnit[]) {
  //*dTmp = body[iBody].dVelY;
  if (system->bOutputLocalBaryCoords) {
    *dTmp = body[iBody].dLocalBaryCartVel[1];
  } else {
    *dTmp = body[iBody].dBCartVel[1];
  } 
  strcpy(cUnit, "");
}

void WriteBaryVelZ(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char cUnit[]) {
  //*dTmp = body[iBody].dVelZ;
  if (system->bOutputLocalBaryCoords) {
    *dTmp = body[iBody].dLocalBaryCartVel[2];
  } else {
    *dTmp = body[iBody].dBCartVel[2];
  } 
  strcpy(cUnit, "");
}


//HelioOrbElems Writeout


void WriteHelioSemi(BODY *body, CONTROL *control, OUTPUT *output,
                              SYSTEM *system, UNITS *units, UPDATE *update,
                              int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHelioSemi;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
       strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

void WriteHelioEcc(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHelioEcc;
}

void WriteHelioInc(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHelioInc;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteHelioLongA(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHelioLongA;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteHelioArgP(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHelioArgP;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteHelioLongP(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHelioLongP;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteHelioMeanA(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHelioMeanA;
  // MeanA only represented in degrees in a bound orbit
  if (body[iBody].dEcc < 1) {
    if (output->bDoNeg[iBody]) {
      *dTmp *= output->dNeg;
      strcpy(cUnit, output->cNeg);
    } else {
      *dTmp /= fdUnitsAngle(units->iAngle);
      fsUnitsAngle(units->iAngle, cUnit);
    }
  } // In a hyperbolic orbit MeanL is represented as an area in radians
}

void WriteHelioMeanL(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHelioMeanL;
  // MeanL only represented in degrees in a bound orbit
  if (body[iBody].dEcc < 1) {
    if (output->bDoNeg[iBody]) {
      *dTmp *= output->dNeg;
      strcpy(cUnit, output->cNeg);
    } else {
      *dTmp /= fdUnitsAngle(units->iAngle);
      fsUnitsAngle(units->iAngle, cUnit);
    }
  } // In a hyperbolic orbit MeanL is represented as an area in radians
}

void WriteHelioTrueA(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dHelioTrueA;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteHelioEccA(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {                      
  *dTmp = body[iBody].dHelioEccA;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteHelioHypA(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {               
  *dTmp = body[iBody].dHelioHypA;
  // Do not convert to degrees
  // Hyperbolic Anomaly is a ratio of areas represented only in radians
}


//BaryOrbElems Writeout


void WriteBarySemi(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dBarySemi;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
       strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

void WriteBaryEcc(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dBaryEcc;
}

void WriteBaryInc(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dBaryInc;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteBaryLongA(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dBaryLongA;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteBaryArgP(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dBaryArgP;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteBaryLongP(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {        
  *dTmp = body[iBody].dBaryLongP;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteBaryMeanA(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dBaryMeanA;
  // MeanA only represented in degrees in a bound orbit
  if (body[iBody].dBaryEcc < 1) {
    if (output->bDoNeg[iBody]) {
      *dTmp *= output->dNeg;
      strcpy(cUnit, output->cNeg);
    } else {
      *dTmp /= fdUnitsAngle(units->iAngle);
      fsUnitsAngle(units->iAngle, cUnit);
    }
  } // In a hyperbolic orbit MeanA is represented as an area in radians
}

void WriteBaryMeanL(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {                      
  *dTmp = body[iBody].dBaryMeanL;
  // MeanL only represented in degrees in a bound orbit
  if (body[iBody].dBaryEcc < 1) {
    if (output->bDoNeg[iBody]) {
      *dTmp *= output->dNeg;
      strcpy(cUnit, output->cNeg);
    } else {
      *dTmp /= fdUnitsAngle(units->iAngle);
      fsUnitsAngle(units->iAngle, cUnit);
    }
  } // In a hyperbolic orbit MeanL is represented as an area in radians
}

void WriteBaryTrueA(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dBaryTrueA;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteBaryEccA(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dBaryEccA;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteBaryHypA(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]) {                      
  *dTmp = body[iBody].dBaryHypA;
  // Do not convert to degrees
  // Hyperbolic Anomaly is a ratio of areas represented only in radians
}

//Other Barycentric variable Write functions below

void WriteBaryMu(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS
               *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dBaryMu;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass); //MUST CHANGE UNITS HERE!!!
    fsUnitsMass(units->iMass,cUnit);
  }
}

void WriteBaryEccSq(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dBaryEccSq;
}

void WriteBaryMeanMotion(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dBaryMeanMotion;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }  
}

void WriteBaryOrbPeriod(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {                           
  *dTmp = body[iBody].dBaryOrbPeriod;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteBarySinc(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  // A future developer might need to take care of this if needed.
  *dTmp = body[iBody].dBarySinc;
}



void InitializeOutputSpiNBody(OUTPUT *output, fnWriteOutput fnWrite[]) {
  // Output example for dPositionX variable
  // XXX All of these should be deprecated so that it doesn't matter if the user
  // calls DistOrb or SpiNBody

  // HelioCart values 


  sprintf(output[OUT_HELIOPOSX].cName, "HelioPosX");
  sprintf(output[OUT_HELIOPOSX].cDescr,
          "Body's x position where origin is centered at the primary mass.");
  // sprintf(output[OUT_HELIOPOSX].cNeg," ");
  output[OUT_HELIOPOSX].bNeg = 0;
  // output[OUT_HELIOPOSX].dNeg = 1;
  output[OUT_HELIOPOSX].iNum       = 1;
  output[OUT_HELIOPOSX].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOPOSX]           = &WriteHelioPosX;

  sprintf(output[OUT_HELIOPOSY].cName, "HelioPosY");
  sprintf(output[OUT_HELIOPOSY].cDescr,
          "Body's y position where origin is centered at the primary mass.");
  // sprintf(output[OUT_HELIOPOSY].cNeg," ");
  output[OUT_HELIOPOSY].bNeg = 0;
  // output[OUT_HELIOPOSY].dNeg = 1;
  output[OUT_HELIOPOSY].iNum       = 1;
  output[OUT_HELIOPOSY].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOPOSY]           = &WriteHelioPosY;

  sprintf(output[OUT_HELIOPOSZ].cName, "HelioPosZ");
  sprintf(output[OUT_HELIOPOSZ].cDescr,
          "Body's z position where origin is centered at the primary mass.");
  // sprintf(output[OUT_HELIOPOSZ].cNeg," ");
  output[OUT_HELIOPOSZ].bNeg = 0;
  // output[OUT_HELIOPOSZ].dNeg = 1;
  output[OUT_HELIOPOSZ].iNum       = 1;
  output[OUT_HELIOPOSZ].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOPOSZ]           = &WriteHelioPosZ;

  sprintf(output[OUT_HELIOVELX].cName, "HelioVelX");
  sprintf(output[OUT_HELIOVELX].cDescr, 
          "Body's x velocity where origin is centered at the primary mass.");
  // sprintf(output[OUT_HELIOVELX].cNeg," ");
  output[OUT_HELIOVELX].bNeg = 0;
  // output[OUT_HELIOVELX].dNeg = 1;
  output[OUT_HELIOVELX].iNum       = 1;
  output[OUT_HELIOVELX].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOVELX]           = &WriteHelioVelX;

  sprintf(output[OUT_HELIOVELY].cName, "HelioVelY");
  sprintf(output[OUT_HELIOVELY].cDescr, 
          "Body's y velocity where origin is centered at the primary mass.");
  // sprintf(output[OUT_HELIOVELY].cNeg," ");
  output[OUT_HELIOVELY].bNeg = 0;
  // output[OUT_HELIOVELY].dNeg = 1;
  output[OUT_HELIOVELY].iNum       = 1;
  output[OUT_HELIOVELY].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOVELY]           = &WriteHelioVelY;

  sprintf(output[OUT_HELIOVELZ].cName, "HelioVelZ");
  sprintf(output[OUT_HELIOVELZ].cDescr, 
          "Body's z velocity where origin is centered at the primary mass.");
  // sprintf(output[OUT_HELIOVELZ].cNeg," ");
  output[OUT_HELIOVELZ].bNeg = 0;
  // output[OUT_HELIOVELZ].dNeg = 1;
  output[OUT_HELIOVELZ].iNum       = 1;
  output[OUT_HELIOVELZ].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOVELZ]           = &WriteHelioVelZ;

  // HelioOrbElems values 

  sprintf(output[OUT_HELIOSEMI].cName, "HelioSemi");
  sprintf(output[OUT_HELIOSEMI].cDescr, 
          "Body's heliocentric semi-major axis where origin is centered at the primary mass.");
  sprintf(output[OUT_HELIOSEMI].cNeg, "AU");
  output[OUT_HELIOSEMI].bNeg       = 1;
  output[OUT_HELIOSEMI].dNeg       = 1. / AUM;
  output[OUT_HELIOSEMI].iNum       = 1;
  output[OUT_HELIOSEMI].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOSEMI]           = &WriteHelioSemi;

  sprintf(output[OUT_HELIOECC].cName, "HelioEcc");
  sprintf(output[OUT_HELIOECC].cDescr, 
          "Body's heliocentric eccentricity where the origin is centered at the primary mass.");
  output[OUT_HELIOECC].bNeg       = 0;
  output[OUT_HELIOECC].iNum       = 1;
  output[OUT_HELIOECC].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOECC]           = &WriteHelioEcc;
  
  sprintf(output[OUT_HELIOINC].cName, "HelioInc");
  sprintf(output[OUT_HELIOINC].cDescr, 
          "Body's heliocentric inclination where origin is centered at the primary mass.");
  sprintf(output[OUT_HELIOINC].cNeg, "Deg");
  output[OUT_HELIOINC].bNeg       = 1;
  output[OUT_HELIOINC].dNeg       = 1. / DEGRAD;
  output[OUT_HELIOINC].iNum       = 1;
  output[OUT_HELIOINC].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOINC]           = &WriteHelioInc;

  sprintf(output[OUT_HELIOLONGA].cName, "HelioLongA");
  sprintf(output[OUT_HELIOLONGA].cDescr, 
          "Body's heliocentric longitude of ascending node where origin is centered at the primary mass.");
  sprintf(output[OUT_HELIOLONGA].cNeg, "Deg");
  output[OUT_HELIOLONGA].bNeg       = 1;
  output[OUT_HELIOLONGA].dNeg       = 1. / DEGRAD;
  output[OUT_HELIOLONGA].iNum       = 1;
  output[OUT_HELIOLONGA].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOLONGA]           = &WriteHelioLongA;

  sprintf(output[OUT_HELIOARGP].cName, "HelioArgP");
  sprintf(output[OUT_HELIOARGP].cDescr, "The body's heliocentric Argument of Pericenter.");
  sprintf(output[OUT_HELIOARGP].cNeg, "Deg");
  output[OUT_HELIOARGP].bNeg       = 1;
  output[OUT_HELIOARGP].dNeg       = 1. / DEGRAD;
  output[OUT_HELIOARGP].iNum       = 1;
  output[OUT_HELIOARGP].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOARGP]           = &WriteHelioArgP;

  sprintf(output[OUT_HELIOLONGP].cName, "HelioLongP");
  sprintf(output[OUT_HELIOLONGP].cDescr, 
          "Body's  heliocntric longitude of pericenter where origin is centered at the primary mass.");
  sprintf(output[OUT_HELIOLONGP].cNeg, "Deg");
  output[OUT_HELIOLONGP].bNeg       = 1;
  output[OUT_HELIOLONGP].dNeg       = 1. / DEGRAD;
  output[OUT_HELIOLONGP].iNum       = 1;
  output[OUT_HELIOLONGP].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOLONGP]           = &WriteHelioLongP;

  sprintf(output[OUT_HELIOMEANA].cName, "HelioMeanA");
  sprintf(output[OUT_HELIOMEANA].cDescr, 
          "Body's heliocntric mean anamoly where origin is centered at the primary mass.");
  sprintf(output[OUT_HELIOMEANA].cNeg, "Deg");
  output[OUT_HELIOMEANA].bNeg       = 1;
  output[OUT_HELIOMEANA].dNeg       = 1. / DEGRAD;
  output[OUT_HELIOMEANA].iNum       = 1;
  output[OUT_HELIOMEANA].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOMEANA]           = &WriteHelioMeanA;

  sprintf(output[OUT_HELIOMEANL].cName, "HelioMeanL");
  sprintf(output[OUT_HELIOMEANL].cDescr, 
          "Body's heliocentric Mean Longitude where origin is centered at the primary mass.");
  sprintf(output[OUT_HELIOMEANL].cNeg, "Deg");
  output[OUT_HELIOMEANL].bNeg       = 1;
  output[OUT_HELIOMEANL].dNeg       = 1. / DEGRAD;
  output[OUT_HELIOMEANL].iNum       = 1;
  output[OUT_HELIOMEANL].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOMEANL]           = &WriteHelioMeanL;

  sprintf(output[OUT_HELIOTRUEA].cName, "HelioTrueA");
  sprintf(output[OUT_HELIOTRUEA].cDescr, 
          "Body's heliocentric True Anomaly where origin is centered at the primary mass.");
  sprintf(output[OUT_HELIOTRUEA].cNeg, "Deg");
  output[OUT_HELIOTRUEA].bNeg       = 1;
  output[OUT_HELIOTRUEA].dNeg       = 1. / DEGRAD;
  output[OUT_HELIOTRUEA].iNum       = 1;
  output[OUT_HELIOTRUEA].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOTRUEA]           = &WriteHelioTrueA;

  sprintf(output[OUT_HELIOECCA].cName, "HelioEccA");
  sprintf(output[OUT_HELIOECCA].cDescr, 
          "Body's heliocentric Eccentric Anomaly where origin is centered at the primary mass.");
  sprintf(output[OUT_HELIOECCA].cNeg, "Deg");
  output[OUT_HELIOECCA].bNeg       = 1;
  output[OUT_HELIOECCA].dNeg       = 1. / DEGRAD;
  output[OUT_HELIOECCA].iNum       = 1;
  output[OUT_HELIOECCA].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOECCA]           = &WriteHelioEccA;

  sprintf(output[OUT_HELIOHYPA].cName, "HelioHypA");
  sprintf(output[OUT_HELIOHYPA].cDescr, 
          "Body's heliocentric Hyperbolic Anomaly where origin is centered at the primary mass.");
  sprintf(output[OUT_HELIOHYPA].cNeg, "Deg");
  output[OUT_HELIOHYPA].bNeg       = 1;
  output[OUT_HELIOHYPA].dNeg       = 1. / DEGRAD;
  output[OUT_HELIOHYPA].iNum       = 1;
  output[OUT_HELIOHYPA].iModuleBit = SPINBODY;
  fnWrite[OUT_HELIOHYPA]           = &WriteHelioHypA;
  


  // BaryCart values 


  sprintf(output[OUT_BARYPOSX].cName, "BaryPosX");
  sprintf(output[OUT_BARYPOSX].cDescr,
          "Body's x position where origin is centered at the system's barycenter.");
  // sprintf(output[OUT_BARYPOSX].cNeg," ");
  output[OUT_BARYPOSX].bNeg = 0;
  // output[OUT_BARYPOSX].dNeg = 1;
  output[OUT_BARYPOSX].iNum       = 1;
  output[OUT_BARYPOSX].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYPOSX]           = &WriteBaryPosX;

  sprintf(output[OUT_BARYPOSY].cName, "BaryPosY");
  sprintf(output[OUT_BARYPOSY].cDescr,
          "Body's y position where origin is centered at the system's barycenter.");
  // sprintf(output[OUT_BARYPOSY].cNeg," ");
  output[OUT_BARYPOSY].bNeg = 0;
  // output[OUT_BARYPOSY].dNeg = 1;
  output[OUT_BARYPOSY].iNum       = 1;
  output[OUT_BARYPOSY].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYPOSY]           = &WriteBaryPosY;

  sprintf(output[OUT_BARYPOSZ].cName, "BaryPosZ");
  sprintf(output[OUT_BARYPOSZ].cDescr,
          "Body's z position where origin is centered at the system's barycenter.");
  // sprintf(output[OUT_BARYPOSZ].cNeg," ");
  output[OUT_BARYPOSZ].bNeg = 0;
  // output[OUT_BARYPOSZ].dNeg = 1;
  output[OUT_BARYPOSZ].iNum       = 1;
  output[OUT_BARYPOSZ].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYPOSZ]           = &WriteBaryPosZ;

  sprintf(output[OUT_BARYVELX].cName, "BaryVelX");
  sprintf(output[OUT_BARYVELX].cDescr, 
          "Body's x velocity where origin is centered at the system's barycenter.");
  // sprintf(output[OUT_BARYVELX].cNeg," ");
  output[OUT_BARYVELX].bNeg = 0;
  // output[OUT_BARYVELX].dNeg = 1;
  output[OUT_BARYVELX].iNum       = 1;
  output[OUT_BARYVELX].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYVELX]           = &WriteBaryVelX;

  sprintf(output[OUT_BARYVELY].cName, "BaryVelY");
  sprintf(output[OUT_BARYVELY].cDescr, 
          "Body's y velocity where origin is centered at the system's barycenter.");
  // sprintf(output[OUT_BARYVELY].cNeg," ");
  output[OUT_BARYVELY].bNeg = 0;
  // output[OUT_BARYVELY].dNeg = 1;
  output[OUT_BARYVELY].iNum       = 1;
  output[OUT_BARYVELY].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYVELY]           = &WriteBaryVelY;

  sprintf(output[OUT_BARYVELZ].cName, "BaryVelZ");
  sprintf(output[OUT_BARYVELZ].cDescr, 
          "Body's z velocity where origin is centered at the system's barycenter.");
  // sprintf(output[OUT_BARYVELZ].cNeg," ");
  output[OUT_BARYVELZ].bNeg = 0;
  // output[OUT_BARYVELZ].dNeg = 1;
  output[OUT_BARYVELZ].iNum       = 1;
  output[OUT_BARYVELZ].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYVELZ]           = &WriteBaryVelZ;

  // BaryOrbElems values 

  sprintf(output[OUT_BARYSEMI].cName, "BarySemi");
  sprintf(output[OUT_BARYSEMI].cDescr, 
          "Body's semi-major axis where origin is centered at the system's barycenter.");
  sprintf(output[OUT_BARYSEMI].cNeg, "AU");
  output[OUT_BARYSEMI].bNeg       = 1;
  output[OUT_BARYSEMI].dNeg       = 1. / AUM;
  output[OUT_BARYSEMI].iNum       = 1;
  output[OUT_BARYSEMI].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYSEMI]           = &WriteBarySemi;

  sprintf(output[OUT_BARYECC].cName, "BaryEcc");
  sprintf(output[OUT_BARYECC].cDescr, 
          "Body's eccentricity where the origin is centered at the system's barycenter.");
  output[OUT_BARYECC].bNeg       = 0;
  output[OUT_BARYECC].iNum       = 1;
  output[OUT_BARYECC].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYECC]           = &WriteBaryEcc;

  sprintf(output[OUT_BARYINC].cName, "BaryInc");
  sprintf(output[OUT_BARYINC].cDescr, 
          "Body's inclination where origin is centered at the system's barycenter.");
  sprintf(output[OUT_BARYINC].cNeg, "Deg");
  output[OUT_BARYINC].bNeg       = 1;
  output[OUT_BARYINC].dNeg       = 1. / DEGRAD;
  output[OUT_BARYINC].iNum       = 1;
  output[OUT_BARYINC].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYINC]           = &WriteBaryInc;

  sprintf(output[OUT_BARYLONGA].cName, "BaryLongA");
  sprintf(output[OUT_BARYLONGA].cDescr, 
          "Body's longitude of ascending node where origin is centered at the system's barycenter.");
  sprintf(output[OUT_BARYLONGA].cNeg, "Deg");
  output[OUT_BARYLONGA].bNeg       = 1;
  output[OUT_BARYLONGA].dNeg       = 1. / DEGRAD;
  output[OUT_BARYLONGA].iNum       = 1;
  output[OUT_BARYLONGA].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYLONGA]           = &WriteBaryLongA;

  sprintf(output[OUT_BARYARGP].cName, "BaryArgP");
  sprintf(output[OUT_BARYARGP].cDescr, "The body's barycentric Argument of Pericenter.");
  sprintf(output[OUT_BARYARGP].cNeg, "Deg");
  output[OUT_BARYARGP].bNeg       = 1;
  output[OUT_BARYARGP].dNeg       = 1. / DEGRAD;
  output[OUT_BARYARGP].iNum       = 1;
  output[OUT_BARYARGP].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYARGP]           = &WriteBaryArgP;

  sprintf(output[OUT_BARYLONGP].cName, "BaryLongP");
  sprintf(output[OUT_BARYLONGP].cDescr, 
          "Body's longitude of pericenter where origin is centered at the system's barycenter.");
  sprintf(output[OUT_BARYLONGP].cNeg, "Deg");
  output[OUT_BARYLONGP].bNeg       = 1;
  output[OUT_BARYLONGP].dNeg       = 1. / DEGRAD;
  output[OUT_BARYLONGP].iNum       = 1;
  output[OUT_BARYLONGP].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYLONGP]           = &WriteBaryLongP;

  sprintf(output[OUT_BARYMEANA].cName, "BaryMeanA");
  sprintf(output[OUT_BARYMEANA].cDescr, 
          "Body's mean anamoly where origin is centered at the system's barycenter.");
  sprintf(output[OUT_BARYMEANA].cNeg, "Deg");
  output[OUT_BARYMEANA].bNeg       = 1;
  output[OUT_BARYMEANA].dNeg       = 1. / DEGRAD;
  output[OUT_BARYMEANA].iNum       = 1;
  output[OUT_BARYMEANA].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYMEANA]           = &WriteBaryMeanA;

  sprintf(output[OUT_BARYMEANL].cName, "BaryMeanL");
  sprintf(output[OUT_BARYMEANL].cDescr, 
          "Body's barycentric Mean Longitude where origin is centered at the system's barycenter.");
  sprintf(output[OUT_BARYMEANL].cNeg, "Deg");
  output[OUT_BARYMEANL].bNeg       = 1;
  output[OUT_BARYMEANL].dNeg       = 1. / DEGRAD;
  output[OUT_BARYMEANL].iNum       = 1;
  output[OUT_BARYMEANL].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYMEANL]           = &WriteBaryMeanL;

  sprintf(output[OUT_BARYTRUEA].cName, "BaryTrueA");
  sprintf(output[OUT_BARYTRUEA].cDescr, 
          "Body's barycentric True Anomaly where origin is centered at the system's barycenter.");
  sprintf(output[OUT_BARYTRUEA].cNeg, "Deg");
  output[OUT_BARYTRUEA].bNeg       = 1;
  output[OUT_BARYTRUEA].dNeg       = 1. / DEGRAD;
  output[OUT_BARYTRUEA].iNum       = 1;
  output[OUT_BARYTRUEA].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYTRUEA]           = &WriteBaryTrueA;

  sprintf(output[OUT_BARYECCA].cName, "BaryEccA");
  sprintf(output[OUT_BARYECCA].cDescr, 
          "Body's barycentric Eccentric Anomaly where origin is centered at the system's barycenter");
  sprintf(output[OUT_BARYECCA].cNeg, "Deg");
  output[OUT_BARYECCA].bNeg       = 1;
  output[OUT_BARYECCA].dNeg       = 1. / DEGRAD;
  output[OUT_BARYECCA].iNum       = 1;
  output[OUT_BARYECCA].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYECCA]           = &WriteBaryEccA;

  sprintf(output[OUT_BARYHYPA].cName, "BaryHypA");
  sprintf(output[OUT_BARYHYPA].cDescr, 
          "Body's barycentric Hyperbolic Anomaly where origin is centered at the system's barycenter.");
  sprintf(output[OUT_BARYHYPA].cNeg, "Deg");
  output[OUT_BARYHYPA].bNeg       = 1;
  output[OUT_BARYHYPA].dNeg       = 1. / DEGRAD;
  output[OUT_BARYHYPA].iNum       = 1;
  output[OUT_BARYHYPA].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYHYPA]           = &WriteBaryHypA;

  //Other Barycentric variables
  //body[iBody].dBaryMu
  //body[iBody].dBaryEccSq
  //body[iBody].dBaryArgP
  //body[iBody].dBaryEccA
  //body[iBody].dBaryOrbPeriod


  sprintf(output[OUT_BARYMU].cName, "BaryMu");
  sprintf(output[OUT_BARYMU].cDescr, "The system's Barycenter mass.");
  sprintf(output[OUT_BARYMU].cNeg, "Mearth");
  output[OUT_BARYMU].bNeg         = 1;
  output[OUT_BARYMU].dNeg         = 1./ MEARTH; // MUST CHANGE UNITS!!
  output[OUT_BARYMU].iNum         = 1;
  output[OUT_BARYMU].iModuleBit   = SPINBODY;
  fnWrite[OUT_BARYMU]             = &WriteBaryMu;

  sprintf(output[OUT_BARYECCSQ].cName, "BaryEccSq");
  sprintf(output[OUT_BARYECCSQ].cDescr, "The body's barycentric eccentricity squared.");
  output[OUT_BARYECCSQ].bNeg       = 0;
  output[OUT_BARYECCSQ].iNum       = 1;
  output[OUT_BARYECCSQ].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYECCSQ]           = &WriteBaryEccSq;

  sprintf(output[OUT_BARYMEANMOTION].cName, "BaryMeanMotion");
  sprintf(output[OUT_BARYMEANMOTION].cDescr, "The body's Mean Motion with respect to the system's barycenter.");
  sprintf(output[OUT_BARYMEANMOTION].cNeg, "/day");
  output[OUT_BARYMEANMOTION].bNeg       = 1;
  output[OUT_BARYMEANMOTION].dNeg       = DAYSEC;
  output[OUT_BARYMEANMOTION].iNum       = 1;
  output[OUT_BARYMEANMOTION].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYMEANMOTION]           = &WriteBaryMeanMotion;

  sprintf(output[OUT_BARYORBPERIOD].cName, "BaryOrbPeriod");
  sprintf(output[OUT_BARYORBPERIOD].cDescr, "The body's Orbital Period with respect to the system's barycenter.");
  sprintf(output[OUT_BARYORBPERIOD].cNeg, "days");
  output[OUT_BARYORBPERIOD].bNeg        = 1;
  output[OUT_BARYORBPERIOD].dNeg        = 1. / DAYSEC;
  output[OUT_BARYORBPERIOD].iNum        = 1;
  output[OUT_BARYORBPERIOD].iModuleBit = SPINBODY;
  fnWrite[OUT_BARYORBPERIOD]            = &WriteBaryOrbPeriod;

  sprintf(output[OUT_BARYSINC].cName, "BarySinc");
  sprintf(output[OUT_BARYSINC].cDescr, "Sin(BaryInc / 2.0)");
  output[OUT_BARYSINC].bNeg           = 1;
  output[OUT_BARYSINC].iNum           = 1;
  output[OUT_BARYSINC].iModuleBit     = SPINBODY;
  fnWrite[OUT_BARYSINC]               = &WriteBarySinc;

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
