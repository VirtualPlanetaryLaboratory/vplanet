/**
   @file distorb.c

   @brief Subroutines that control the integration of the orbital model.

   @author Russell Deitrick ([deitrr](https://github.com/deitrr/))

   @date April 28 2015

*/

#include "vplanet.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void BodyCopyDistOrb(BODY *dest, BODY *src, int iTideModel, int iNumBodies,
                     int iBody) {
  int iIndex, iPert;

  dest[iBody].dPinc = src[iBody].dPinc;
  dest[iBody].dQinc = src[iBody].dQinc;

  dest[iBody].iGravPerts = src[iBody].iGravPerts;
  //   dest[iBody].iaGravPerts = malloc(dest[iBody].iGravPerts*sizeof(int));
  for (iPert = 0; iPert < src[iBody].iGravPerts; iPert++) {
    dest[iBody].iaGravPerts[iPert] = src[iBody].iaGravPerts[iPert];
  }
}

void InitializeBodyDistOrb(BODY *body, CONTROL *control, UPDATE *update,
                           int iBody, int iModule) {
  if (body[iBody].bDistOrb) {
    if (control->Evolve.iDistOrbModel == RD4) {
      body[iBody].iGravPerts    = control->Evolve.iNumBodies - 2;
      body[iBody].iDistOrbModel = RD4;
    } else if (control->Evolve.iDistOrbModel == LL2) {
      /* "Perturbers" in LL2 correspond to eigenfrequencies, not planet pairs.
         Number of eigenfrequencies = number of planets. */
      body[iBody].iGravPerts    = control->Evolve.iNumBodies - 1;
      body[iBody].iDistOrbModel = LL2;
    }
  }

  body[iBody].iaGravPerts = malloc(body[iBody].iGravPerts * sizeof(int));
}

void InitializeUpdateTmpBodyDistOrb(BODY *body, CONTROL *control,
                                    UPDATE *update, int iBody) {
  control->Evolve.tmpBody[iBody].iaGravPerts =
        malloc(body[iBody].iGravPerts * sizeof(int));
}

/**************** DISTORB options ********************/

void ReadDfCrit(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
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
    system->dDfcrit = dTmp;
    if (system->dDfcrit > 1 && control->Io.iVerbose >= VERBALL) {
      fprintf(stderr, "WARNING: %s > 1 is not advised (%s:%d).\n",
              options->cName, files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &system->dDfcrit, files->iNumInputs);
  }
}


void ReadInvPlane(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    /* Option was found */
    control->bInvPlane = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &control->bInvPlane, files->iNumInputs);
  }
}

void ReadOutputLapl(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    /* Option was found */
    control->bOutputLapl = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &control->bOutputLapl, files->iNumInputs);
  }
}

void ReadOutputEigen(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    /* Option was found */
    control->bOutputEigen = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &control->bOutputEigen, files->iNumInputs);
  }
}

void ReadOverrideMaxEcc(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    control->Halt[iFile - 1].bOverrideMaxEcc = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    control->Halt[iFile - 1].bOverrideMaxEcc = options->dDefault;
  }
  //     AssignDefaultInt(options,&control->Halt[iFile-1].bOverrideMaxEcc,files->iNumInputs);
}

void ReadHaltHillStab(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    /* Option was found */
    control->Halt[iFile - 1].bHillStab = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    control->Halt[iFile - 1].bHillStab = options->dDefault;
  }
}

void ReadHaltCloseEnc(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    /* Option was found */
    control->Halt[iFile - 1].bCloseEnc = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    control->Halt[iFile - 1].bCloseEnc = options->dDefault;
  }
}

/* For LL2 (eigenvalue) solution only, we could add these option to let the
user specify the eigenvalues instead of calculating them from scratch. This is
useful if you want to use, for example, an N-body code to compute frequencies
that are accurate to higher orders. One could conceivably fake the higher order
evolution using only the LL2 solution by doing this.


void ReadEigenSet(BODY *body,CONTROL *control,FILES *files,OPTIONS
*options,SYSTEM *system,int iFile) { int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    / * Option was found * /
    body[iFile-1].bEigenSet = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    body[iFile-1].bEigenSet = options->dDefault;
//
AssignDefaultInt(options,&control->Halt[iFile-1].bOverrideMaxEcc,files->iNumInputs);
}

void ReadEigenvalue(BODY *body,CONTROL *control,FILES *files,OPTIONS
*options,SYSTEM *system,int iFile) { / * This parameter cannot exist in the
primary file * / int lTmp=-1; double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);

    dTmp *= DEGRAD/3600./YEARSEC;

    body[iFile-1].dEigenvalue = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dEigenvalue = options->dDefault;
}

void ReadEigenvector(BODY *body,CONTROL *control,FILES *files,OPTIONS
*options,SYSTEM *system,int iFile) { / * This parameter cannot exist in the
primary file * / int lTmp=-1; double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);

    body[iFile-1].dEigenvector = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dEigenvector = options->dDefault;
}
*/

void ReadOrbitModel(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
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
    if (!memcmp(sLower(cTmp), "ll2", 3)) {
      control->Evolve.iDistOrbModel = LL2;
    } else if (!memcmp(sLower(cTmp), "rd4", 3)) {
      control->Evolve.iDistOrbModel = RD4;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are ll2 or rd4.\n",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &control->Evolve.iDistOrbModel,
                     files->iNumInputs);
  }
}


void InitializeOptionsDistOrb(OPTIONS *options, fnReadOption fnRead[]) {
  sprintf(options[OPT_DFCRIT].cName, "dDfcrit");
  sprintf(options[OPT_DFCRIT].cDescr,
          "Tolerance parameter for recalculating semi-major axis functions");
  sprintf(options[OPT_DFCRIT].cDefault, "0.1");
  sprintf(options[OPT_DFCRIT].cDimension, "nd");
  options[OPT_DFCRIT].dDefault   = 0.1;
  options[OPT_DFCRIT].iType      = 2;
  options[OPT_DFCRIT].bMultiFile = 0;
  fnRead[OPT_DFCRIT]             = &ReadDfCrit;
  sprintf(options[OPT_DFCRIT].cLongDescr,
          "When running DistOrb with other modules that modify the semi-major "
          "axis,\n"
          "set his argument to be the maximum relative change in the Laplace\n"
          "coefficients before recalculating the values. Setting this value to "
          "be\n"
          "low can cause the simulation to run very slowly, with negligible "
          "gain in\n"
          "accuracy.");

  sprintf(options[OPT_INVPLANE].cName, "bInvPlane");
  sprintf(options[OPT_INVPLANE].cDescr,
          "Convert input coordinates to invariable plane coordinates");
  sprintf(options[OPT_INVPLANE].cDefault, "0");
  options[OPT_INVPLANE].dDefault   = 0;
  options[OPT_INVPLANE].iType      = 0;
  options[OPT_INVPLANE].bMultiFile = 0;
  fnRead[OPT_INVPLANE]             = &ReadInvPlane;

  sprintf(options[OPT_ORBITMODEL].cName, "sOrbitModel");
  sprintf(options[OPT_ORBITMODEL].cDescr,
          "Orbit Model: LL2 [2nd order] RD4 [4th order]");
  sprintf(options[OPT_ORBITMODEL].cDefault, "rd4");
  options[OPT_ORBITMODEL].dDefault = RD4;
  options[OPT_ORBITMODEL].iType    = 3;
  fnRead[OPT_ORBITMODEL]           = &ReadOrbitModel;
  sprintf(options[OPT_ORBITMODEL].cLongDescr,
          "The secular orbital evolution model used with DistOrb. Option LL2 "
          "is the\n"
          "Laplace-Lagrange (eigenvalue) solution that is valid for small "
          "values of\n"
          "of eccentricity (<0.1) and inclination (<10 deg). The RD4 method is "
          "a 4th\n"
          "order solution of the distrubing function that is accurante up to e "
          "~ 0.3\n"
          "and i ~ 30 deg. Note, however that setting both parameters to "
          "moderate\n"
          "values can still cause inaccurate evolution. Unless %s is set, this "
          "model\n"
          "will halt if the eccentricity exceeds ~0.69, at which point the "
          "solution\n"
          "does not converge.",
          options[OPT_ORMAXECC].cName);

  sprintf(options[OPT_ORMAXECC].cName, "bOverrideMaxEcc");
  sprintf(options[OPT_ORMAXECC].cDescr,
          "Override default maximum eccentricity (MAXORBDISTORB) in DistOrb?");
  sprintf(options[OPT_ORMAXECC].cDefault, "0");
  options[OPT_ORMAXECC].dDefault   = 0;
  options[OPT_ORMAXECC].iType      = 0;
  options[OPT_ORMAXECC].bMultiFile = 0;
  fnRead[OPT_ORMAXECC]             = &ReadOverrideMaxEcc;
  sprintf(options[OPT_ORBITMODEL].cLongDescr,
          "If RD4 is selected for %s, the code will halt if an eccentricity "
          "reaches\n"
          "~0.69 unless this flag is set to true.",
          options[OPT_ORBITMODEL].cName);

  sprintf(options[OPT_HALTHILLSTAB].cName, "bHaltHillStab");
  sprintf(options[OPT_HALTHILLSTAB].cDescr, "Halt if Hill unstable?");
  sprintf(options[OPT_HALTHILLSTAB].cDefault, "0");
  options[OPT_HALTHILLSTAB].dDefault   = 0;
  options[OPT_HALTHILLSTAB].iType      = 0;
  options[OPT_HALTHILLSTAB].bMultiFile = 0;
  fnRead[OPT_HALTHILLSTAB]             = &ReadHaltHillStab;
  sprintf(
        options[OPT_HALTHILLSTAB].cLongDescr,
        "If two planets come within the Hill stability criterion, the code will"
        "halt if this parameter is set");

  sprintf(options[OPT_HALTCLOSEENC].cName, "bHaltCloseEnc");
  sprintf(options[OPT_HALTCLOSEENC].cDescr, "Halt if orbits get too close");
  sprintf(options[OPT_HALTCLOSEENC].cDefault, "0");
  options[OPT_HALTCLOSEENC].dDefault   = 0;
  options[OPT_HALTCLOSEENC].iType      = 0;
  options[OPT_HALTCLOSEENC].bMultiFile = 0;
  fnRead[OPT_HALTCLOSEENC]             = &ReadHaltCloseEnc;
  sprintf(
        options[OPT_HALTCLOSEENC].cLongDescr,
        "Halt the code if the apocenter of an interior planets is less than 4 "
        "mutual Hill radii from the pericenter of an outer planet.");

  /* For LL2 (eigenvalue) solution only, we could add these option to let the
  user specify the eigenvalues instead of calculating them from scratch. This is
  useful if you want to use, for example, an N-body code to compute frequencies
  that are accurate to higher orders. One could conceivably fake the higher
  order evolution using only the LL2 solution by doing this.

  sprintf(options[OPT_EIGENSET].cName,"bEigenSet");
  sprintf(options[OPT_EIGENSET].cDescr,"Read in eigenvalues/vectors for
  DistOrb?"); sprintf(options[OPT_EIGENSET].cDefault,"0");
  options[OPT_EIGENSET].dDefault = 0;
  options[OPT_EIGENSET].iType = 0;
  options[OPT_EIGENSET].bMultiFile = 0;
  fnRead[OPT_EIGENSET] = &ReadEigenSet;

  sprintf(options[OPT_EIGENVALUE].cName,"dEigenvalue");
  sprintf(options[OPT_EIGENVALUE].cDescr,"Set this to provide
  eigenvalues/vectors at input"); sprintf(options[OPT_EIGENVALUE].cDefault,"0");
  sprintf(options[OPT_EIGENVALUE].cDimension,"nd");
  options[OPT_EIGENVALUE].dDefault = 0;
  options[OPT_EIGENVALUE].iType = 0;
  options[OPT_EIGENVALUE].bMultiFile = 0;
  fnRead[OPT_EIGENVALUE] = &ReadEigenvalue;

  sprintf(options[OPT_EIGENVECTOR].cName,"dEigenvector");
  sprintf(options[OPT_EIGENVECTOR].cDescr,"Set this to provide
  eigenvalues/vectors at input");
  sprintf(options[OPT_EIGENVECTOR].cDefault,"0");
  sprintf(options[OPT_EIGENVECTOR].cDimension,"nd");
  options[OPT_EIGENVECTOR].dDefault = 0;
  options[OPT_EIGENVECTOR].iType = 0;
  options[OPT_EIGENVECTOR].bMultiFile = 0;
  fnRead[OPT_EIGENVECTOR] = &ReadEigenvector;
*/

  sprintf(options[OPT_OUTPUTLAPL].cName, "bOutputLapl");
  sprintf(options[OPT_OUTPUTLAPL].cDescr,
          "Output Laplace functions and related data?");
  sprintf(options[OPT_OUTPUTLAPL].cDefault, "0");
  options[OPT_OUTPUTLAPL].dDefault   = 0;
  options[OPT_OUTPUTLAPL].iType      = 0;
  options[OPT_OUTPUTLAPL].bMultiFile = 0;
  fnRead[OPT_OUTPUTLAPL]             = &ReadOutputLapl;
  sprintf(
        options[OPT_OUTPUTLAPL].cLongDescr,
        "Write files that contain the Laplace coefficients and their "
        "derivatives\n"
        "in DistOrb. This file can be used to check that they are recomputed \n"
        "frequently enough when damping (e.g. EqTide) is included.");

  sprintf(options[OPT_OUTPUTEIGEN].cName, "bOutputEigen");
  sprintf(options[OPT_OUTPUTEIGEN].cDescr, "Output Eigenvalues?");
  sprintf(options[OPT_OUTPUTEIGEN].cDefault, "0");
  options[OPT_OUTPUTEIGEN].dDefault   = 0;
  options[OPT_OUTPUTEIGEN].iType      = 0;
  options[OPT_OUTPUTEIGEN].bMultiFile = 0;
  fnRead[OPT_OUTPUTEIGEN]             = &ReadOutputEigen;
  sprintf(
        options[OPT_OUTPUTLAPL].cLongDescr,
        "Write special files that contain the eigenvalues and eigenvectors of "
        "the\n"
        "system with DistOrb. In the LL2 solution, these are already computed. "
        "In\n"
        "the RD4 solution, they are computed at time of output. These can be\n"
        "useful for interpretating results.");
}

void ReadOptionsDistOrb(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, fnReadOption fnRead[],
                        int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTDISTORB; iOpt < OPTENDDISTORB; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}


/******************* Verify DISTORB ******************/


/*
 *
 * Pericenter/Ascending node
 *
 */

double fndCorrectDomain(double angle) {
  while (angle >= 2 * PI) {
    angle -= 2 * PI;
  }
  while (angle < 0.0) {
    angle += 2 * PI;
  }
  return angle;
}

double fndCalcLongP(double dLongA, double dArgP) {
  double varpi;
  varpi = fndCorrectDomain(dLongA + dArgP);
  return varpi;
}

double fndCalcLongA(double dLongP, double dArgP) {
  double Omega;
  Omega = fndCorrectDomain(dLongP - dArgP);
  return Omega;
}

void VerifyOrbitModel(CONTROL *control, FILES *files, OPTIONS *options) {
  int iFile, iFound = 0;
  char cTmp[8];

  for (iFile = 0; iFile < files->iNumInputs; iFile++) {
    if (options[OPT_ORBITMODEL].iLine[iFile] >= 0) {
      iFound++;
    }
  }

  if (iFound > 1) {
    if (control->Io.iVerbose > VERBERR) {
      fprintf(stderr, "ERROR: Option %s set multiple times.\n",
              options[OPT_ORBITMODEL].cName);
      for (iFile = 0; iFile < files->iNumInputs; iFile++) {
        if (options[OPT_ORBITMODEL].iLine[iFile] >= 0) {
          fprintf(stderr, "\tFile %s, Line: %d\n", files->Infile[0].cIn,
                  options[OPT_ORBITMODEL].iLine[iFile]);
        }
      }
    }
    exit(EXIT_INPUT);
  }

  if (iFound == 0) {
    strcpy(cTmp, options[OPT_ORBITMODEL].cDefault);
    if (!memcmp(sLower(cTmp), "ll2", 3)) {
      control->Evolve.iDistOrbModel = LL2;
    } else if (!memcmp(sLower(cTmp), "rd4", 3)) {
      control->Evolve.iDistOrbModel = RD4;
    }
    if (control->Io.iVerbose >= VERBINPUT) {
      fprintf(stderr, "INFO: %s not set in any file, defaulting to %s.\n",
              options[OPT_ORBITMODEL].cName, options[OPT_ORBITMODEL].cDefault);
    }

    /* Chicanery. Since I only want this set once, I will
       make it seem like the user set it. */
    options[OPT_ORBITMODEL].iLine[0] = 1;
  }
}


// void VerifyStabilityHalts(CONTROL *control,FILES *files,OPTIONS *options) {
//   int iFile, iHillSet, iCloseSet;
//   /* make sure user doesn't set both bHaltHillStab and bHaltCloseEnc */
//
// }

void VerifyPericenter(BODY *body, CONTROL *control, OPTIONS *options,
                      char cFile[], int iBody, int iVerbose) {
  /* First see if longitude of ascending node and longitude of pericenter and
   * nothing else set, i.e. the user input the default parameters */
  if (options[OPT_LONGA].iLine[iBody + 1] > -1 &&
      options[OPT_LONGP].iLine[iBody + 1] > -1 &&
      options[OPT_ARGP].iLine[iBody + 1] == -1) {
    return;
  }

  /* If none are set, raise error */
  if (options[OPT_LONGA].iLine[iBody + 1] == -1 &&
      options[OPT_LONGP].iLine[iBody + 1] == -1 &&
      options[OPT_ARGP].iLine[iBody + 1] == -1) {
    if (iVerbose >= VERBERR) {
      fprintf(stderr, "ERROR: Must set two of %s, %s, and %s in File: %s.\n",
              options[OPT_LONGA].cName, options[OPT_LONGP].cName,
              options[OPT_ARGP].cName, cFile);
    }
    exit(EXIT_INPUT);
  }

  /* At least 2 must be set */
  if ((options[OPT_LONGA].iLine[iBody + 1] == -1 &&
       options[OPT_LONGP].iLine[iBody + 1] == -1) ||
      (options[OPT_LONGA].iLine[iBody + 1] == -1 &&
       options[OPT_ARGP].iLine[iBody + 1] == -1) ||
      (options[OPT_LONGP].iLine[iBody + 1] == -1 &&
       options[OPT_ARGP].iLine[iBody + 1] == -1)) {
    if (iVerbose >= VERBERR) {
      fprintf(stderr, "ERROR: Must set two of %s, %s, and %s in File: %s.\n",
              options[OPT_LONGA].cName, options[OPT_LONGP].cName,
              options[OPT_ARGP].cName, cFile);
    }
    exit(EXIT_INPUT);
  }

  /* Were all set? */
  if (options[OPT_LONGA].iLine[iBody + 1] > -1 &&
      options[OPT_LONGP].iLine[iBody + 1] > -1 &&
      options[OPT_ARGP].iLine[iBody + 1] > -1) {
    VerifyTripleExit(options[OPT_LONGA].cName, options[OPT_LONGP].cName,
                     options[OPT_ARGP].cName,
                     options[OPT_LONGA].iLine[iBody + 1],
                     options[OPT_LONGP].iLine[iBody + 1],
                     options[OPT_ARGP].iLine[iBody + 1], cFile, iVerbose);
    exit(EXIT_INPUT);
  }

  /* Was LONGA set? */
  if (options[OPT_LONGA].iLine[iBody + 1] > -1) {

    if (options[OPT_LONGP].iLine[iBody + 1] > -1) {
      /* LONGA and LONGP were the only two set - Nothing to do */
      return;
    }
    if (options[OPT_ARGP].iLine[iBody + 1] > -1) {
      /* Must get radius from density */
      body[iBody].dLongP = fndCalcLongP(body[iBody].dLongA, body[iBody].dArgP);
    }
    return;
  }

  /* Was LongP set? */
  if (options[OPT_LONGP].iLine[iBody + 1] > -1) {

    if (options[OPT_LONGA].iLine[iBody + 1] > -1) {
      /* LONGA and LONGP were the only two set - Nothing to do */
      return;
    }
    if (options[OPT_ARGP].iLine[iBody + 1] > -1) {
      /* Must get radius from density */
      body[iBody].dLongA = fndCalcLongA(body[iBody].dLongP, body[iBody].dArgP);
    }
    return;
  }
}

/* In the following, iBody is the current body number that is getting assigned,
   iPert counts the number of bodies perturbing iBody, and iBodyPert is the
   body number of the current perturbing body. */

/* The indexing gets a bit confusing here. iPert = 0 to iGravPerts-1 correspond
   to all perturbing planets,  iPert = iGravPerts corresponds to the stellar
   general relativistic correction, if applied. */

void InitializeHeccDistOrbRD4(BODY *body, UPDATE *update, int iBody,
                              int iPert) {
  update[iBody]
        .iaType[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = 2;
  update[iBody].padDHeccDtDistOrb[iPert] =
        &update[iBody].daDerivProc[update[iBody].iHecc]
                                  [update[iBody].iaHeccDistOrb[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iHecc]
                                       [update[iBody].iaHeccDistOrb[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][1] =
        body[iBody].iaGravPerts[iPert];
}

void InitializeKeccDistOrbRD4(BODY *body, UPDATE *update, int iBody,
                              int iPert) {
  update[iBody]
        .iaType[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = 2;
  update[iBody].padDKeccDtDistOrb[iPert] =
        &update[iBody].daDerivProc[update[iBody].iKecc]
                                  [update[iBody].iaKeccDistOrb[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iKecc]
                                       [update[iBody].iaKeccDistOrb[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]][1] =
        body[iBody].iaGravPerts[iPert];
}

void InitializeHeccDistOrbGR(BODY *body, UPDATE *update, int iBody, int iPert) {
  update[iBody]
        .iaType[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = 2;
  update[iBody].padDHeccDtDistOrb[iPert] =
        &update[iBody].daDerivProc[update[iBody].iHecc]
                                  [update[iBody].iaHeccDistOrb[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iHecc]
                                       [update[iBody].iaHeccDistOrb[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][1] = 0;
}

void InitializeKeccDistOrbGR(BODY *body, UPDATE *update, int iBody, int iPert) {
  update[iBody]
        .iaType[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = 2;
  update[iBody].padDKeccDtDistOrb[iPert] =
        &update[iBody].daDerivProc[update[iBody].iKecc]
                                  [update[iBody].iaKeccDistOrb[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iKecc]
                                       [update[iBody].iaKeccDistOrb[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]][1] = 0;
}

void InitializePincDistOrbRD4(BODY *body, UPDATE *update, int iBody,
                              int iPert) {
  update[iBody]
        .iaType[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] = 2;
  update[iBody].padDPincDtDistOrb[iPert] =
        &update[iBody].daDerivProc[update[iBody].iPinc]
                                  [update[iBody].iaPincDistOrb[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iPinc]
                                       [update[iBody].iaPincDistOrb[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]][1] =
        body[iBody].iaGravPerts[iPert];
}

void InitializeQincDistOrbRD4(BODY *body, UPDATE *update, int iBody,
                              int iPert) {
  update[iBody]
        .iaType[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] = 2;
  update[iBody].padDQincDtDistOrb[iPert] =
        &update[iBody].daDerivProc[update[iBody].iQinc]
                                  [update[iBody].iaQincDistOrb[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iQinc]
                                       [update[iBody].iaQincDistOrb[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]][1] =
        body[iBody].iaGravPerts[iPert];
}

void VerifyPerturbersDistOrbRD4(BODY *body, int iNumBodies, int iBody) {
  int iPert = 0, j;

  //   body[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
  for (j = 1; j < iNumBodies; j++) {
    if (j != iBody) {
      if (body[j].bDistOrb == 0) {
        fprintf(stderr, "ERROR: DistOrb must be the called for all planets\n");
        exit(EXIT_INPUT);
      }
      body[iBody].iaGravPerts[iPert] = j;
      iPert++;
    }
  }
}

void InitializeHeccDistOrbLL2(BODY *body, UPDATE *update, int iBody,
                              int iPert) {
  if (body[iBody].bEqtide) {
    update[iBody]
          .iaType[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = 2;
  } else {
    update[iBody]
          .iaType[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] = 3;
  }
  update[iBody].padDHeccDtDistOrb[iPert] =
        &update[iBody].daDerivProc[update[iBody].iHecc]
                                  [update[iBody].iaHeccDistOrb[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iHecc]
                                       [update[iBody].iaHeccDistOrb[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iHecc][update[iBody].iaHeccDistOrb[iPert]][1] =
        body[iBody].iaGravPerts[iPert];
}

void InitializeKeccDistOrbLL2(BODY *body, UPDATE *update, int iBody,
                              int iPert) {
  if (body[iBody].bEqtide) {
    update[iBody]
          .iaType[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = 2;
  } else {
    update[iBody]
          .iaType[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] = 3;
  }
  update[iBody].padDKeccDtDistOrb[iPert] =
        &update[iBody].daDerivProc[update[iBody].iKecc]
                                  [update[iBody].iaKeccDistOrb[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iKecc]
                                       [update[iBody].iaKeccDistOrb[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iKecc][update[iBody].iaKeccDistOrb[iPert]][1] =
        body[iBody].iaGravPerts[iPert];
}

void InitializePincDistOrbLL2(BODY *body, UPDATE *update, int iBody,
                              int iPert) {
  update[iBody]
        .iaType[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] = 3;
  update[iBody].padDPincDtDistOrb[iPert] =
        &update[iBody].daDerivProc[update[iBody].iPinc]
                                  [update[iBody].iaPincDistOrb[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iPinc]
                                       [update[iBody].iaPincDistOrb[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]][1] =
        body[iBody].iaGravPerts[iPert];
}

void InitializeQincDistOrbLL2(BODY *body, UPDATE *update, int iBody,
                              int iPert) {
  update[iBody]
        .iaType[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] = 3;
  update[iBody].padDQincDtDistOrb[iPert] =
        &update[iBody].daDerivProc[update[iBody].iQinc]
                                  [update[iBody].iaQincDistOrb[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iQinc]
                                       [update[iBody].iaQincDistOrb[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]][1] =
        body[iBody].iaGravPerts[iPert];
}

void VerifyPerturbersDistOrbLL2(BODY *body, int iNumBodies, int iBody) {
  int iPert = 0, j;

  //   body[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
  for (j = 1; j < iNumBodies; j++) {
    if (body[j].bDistOrb == 0) {
      fprintf(stderr, "ERROR: DistOrb must be the called for all planets\n");
      exit(EXIT_INPUT);
    }
    body[iBody].iaGravPerts[iPert] = j;
    iPert++;
  }
}

void VerifyGRCorrLL2(BODY *body, int iNumBodies) {
  int iBody;

  for (iBody = 2; iBody < iNumBodies; iBody++) {
    if (body[iBody].bGRCorr != body[1].bGRCorr) {
      fprintf(stderr, "ERROR: bGRCorr must be the same for all planets in "
                      "DistOrb LL2 model\n");
      fprintf(stderr,"\t%s: %d\n",body[1].cName,body[1].bGRCorr);
      fprintf(stderr,"\t%s: %d\n",body[iBody].cName,body[iBody].bGRCorr);
      exit(EXIT_INPUT);
    }
  }
}

void AssignDistOrbDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                              fnUpdateVariable ***fnUpdate, int iBody) {
  int iPert;
  if (evolve->iDistOrbModel == RD4) {
    body[iBody].iGravPerts =
          evolve->iNumBodies -
          2; // will need to change this for zero mass particles in future
    /* The indexing gets a bit confusing here. iPert = 0 to iGravPerts-1
     * correspond to all perturbing planets, iPert = iGravPerts corresponds to
     * the stellar general relativistic correction, if applied */

    /* Setup Semi-major axis functions (LaplaceF) for secular terms*/
    if (iBody >= 1) {
      /* Body updates */
      for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
        /* h = Ecc*sin(LongP) */
        fnUpdate[iBody][update[iBody].iHecc]
                [update[iBody].iaHeccDistOrb[iPert]] = &fndDistOrbRD4DhDt;

        /* k = Ecc*cos(LongP) */
        fnUpdate[iBody][update[iBody].iKecc]
                [update[iBody].iaKeccDistOrb[iPert]] = &fndDistOrbRD4DkDt;

        /* p = s*sin(LongA) */
        fnUpdate[iBody][update[iBody].iPinc]
                [update[iBody].iaPincDistOrb[iPert]] = &fndDistOrbRD4DpDt;

        /* q = s*cos(LongA) */
        fnUpdate[iBody][update[iBody].iQinc]
                [update[iBody].iaQincDistOrb[iPert]] = &fndDistOrbRD4DqDt;
      }
      if (body[iBody].bGRCorr) {
        /* Body updates for general relativistic correction, indexing star as a
         * "perturber"*/
        fnUpdate[iBody][update[iBody].iHecc]
                [update[iBody].iaHeccDistOrb[body[iBody].iGravPerts]] =
                      &fndApsidalGRDhDt;

        fnUpdate[iBody][update[iBody].iKecc]
                [update[iBody].iaKeccDistOrb[body[iBody].iGravPerts]] =
                      &fndApsidalGRDkDt;
      }
    }
  } else if (evolve->iDistOrbModel == LL2) {
    body[iBody].iGravPerts = evolve->iNumBodies - 1;
    VerifyPerturbersDistOrbLL2(body, evolve->iNumBodies, iBody);

    for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
      if (body[iBody].bEqtide) {
        fnUpdate[iBody][update[iBody].iHecc]
                [update[iBody].iaHeccDistOrb[iPert]] = &fndDistOrbLL2DhDt;
        fnUpdate[iBody][update[iBody].iKecc]
                [update[iBody].iaKeccDistOrb[iPert]] = &fndDistOrbLL2DkDt;
      } else {
        fnUpdate[iBody][update[iBody].iHecc]
                [update[iBody].iaHeccDistOrb[iPert]] = &fndDistOrbLL2Hecc;
        fnUpdate[iBody][update[iBody].iKecc]
                [update[iBody].iaKeccDistOrb[iPert]] = &fndDistOrbLL2Kecc;
      }

      /* p = s*sin(LongA) */
      fnUpdate[iBody][update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] =
            &fndDistOrbLL2Pinc;

      /* q = s*cos(LongA) */
      fnUpdate[iBody][update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] =
            &fndDistOrbLL2Qinc;
    }
  }
}

void NullDistOrbDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                            fnUpdateVariable ***fnUpdate, int iBody) {
  int iPert;
  if (evolve->iDistOrbModel == RD4) {
    body[iBody].iGravPerts =
          evolve->iNumBodies -
          2; // will need to change this for zero mass particles in future
    /* The indexing gets a bit confusing here. iPert = 0 to iGravPerts-1
     * correspond to all perturbing planets, iPert = iGravPerts corresponds to
     * the stellar general relativistic correction, if applied */

    /* Setup Semi-major axis functions (LaplaceF) for secular terms*/
    if (iBody >= 1) {
      /* Body updates */
      for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
        /* h = Ecc*sin(LongP) */
        fnUpdate[iBody][update[iBody].iHecc]
                [update[iBody].iaHeccDistOrb[iPert]] = &fndUpdateFunctionTiny;

        /* k = Ecc*cos(LongP) */
        fnUpdate[iBody][update[iBody].iKecc]
                [update[iBody].iaKeccDistOrb[iPert]] = &fndUpdateFunctionTiny;

        /* p = s*sin(LongA) */
        fnUpdate[iBody][update[iBody].iPinc]
                [update[iBody].iaPincDistOrb[iPert]] = &fndUpdateFunctionTiny;

        /* q = s*cos(LongA) */
        fnUpdate[iBody][update[iBody].iQinc]
                [update[iBody].iaQincDistOrb[iPert]] = &fndUpdateFunctionTiny;
      }
      if (body[iBody].bGRCorr) {
        /* Body updates for general relativistic correction, indexing star as a
         * "perturber"*/
        fnUpdate[iBody][update[iBody].iHecc]
                [update[iBody].iaHeccDistOrb[body[iBody].iGravPerts]] =
                      &fndUpdateFunctionTiny;

        fnUpdate[iBody][update[iBody].iKecc]
                [update[iBody].iaKeccDistOrb[body[iBody].iGravPerts]] =
                      &fndUpdateFunctionTiny;
      }
    }
  } else if (evolve->iDistOrbModel == LL2) {
    body[iBody].iGravPerts = evolve->iNumBodies - 1;
    VerifyPerturbersDistOrbLL2(body, evolve->iNumBodies, iBody);

    for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
      if (body[iBody].bEqtide) {
        fnUpdate[iBody][update[iBody].iHecc]
                [update[iBody].iaHeccDistOrb[iPert]] = &fndUpdateFunctionTiny;
        fnUpdate[iBody][update[iBody].iKecc]
                [update[iBody].iaKeccDistOrb[iPert]] = &fndUpdateFunctionTiny;
      } else {
        fnUpdate[iBody][update[iBody].iHecc]
                [update[iBody].iaHeccDistOrb[iPert]] = &fndUpdateFunctionTiny;
        fnUpdate[iBody][update[iBody].iKecc]
                [update[iBody].iaKeccDistOrb[iPert]] = &fndUpdateFunctionTiny;
      }

      /* p = s*sin(LongA) */
      fnUpdate[iBody][update[iBody].iPinc][update[iBody].iaPincDistOrb[iPert]] =
            &fndUpdateFunctionTiny;

      /* q = s*cos(LongA) */
      fnUpdate[iBody][update[iBody].iQinc][update[iBody].iaQincDistOrb[iPert]] =
            &fndUpdateFunctionTiny;
    }
  }
}

void VerifyDistOrb(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   OUTPUT *output, SYSTEM *system, UPDATE *update, int iBody,
                   int iModule) {
  int i, j = 0, iPert = 0, jBody = 0, kBody;

  VerifyOrbitModel(control, files, options);
  //   VerifyStabilityHalts(control,files,options);

  body[iBody].dMeanA = 0.0;
  body[iBody].dEccSq = body[iBody].dEcc * body[iBody].dEcc;

  if (control->Evolve.iDistOrbModel == RD4) {
    /* The indexing gets a bit confusing here. iPert = 0 to iGravPerts-1
     * correspond to all perturbing planets, iPert = iGravPerts corresponds to
     * the stellar general relativistic correction, if applied */

    /* Setup Semi-major axis functions (LaplaceF) for secular terms*/
    if (iBody == 1) {
      system->daLOrb = malloc(3 * sizeof(double));

      system->fnLaplaceF     = malloc(LAPLNUM * sizeof(fnLaplaceFunction *));
      system->fnLaplaceDeriv = malloc(LAPLNUM * sizeof(fnLaplaceFunction *));
      for (j = 0; j < LAPLNUM; j++) {
        system->fnLaplaceF[j]     = malloc(1 * sizeof(fnLaplaceFunction));
        system->fnLaplaceDeriv[j] = malloc(1 * sizeof(fnLaplaceFunction));
      }
      system->fnLaplaceF[0][0]  = &fndSemiMajAxF1;
      system->fnLaplaceF[1][0]  = &fndSemiMajAxF2;
      system->fnLaplaceF[2][0]  = &fndSemiMajAxF3;
      system->fnLaplaceF[3][0]  = &fndSemiMajAxF4;
      system->fnLaplaceF[4][0]  = &fndSemiMajAxF5;
      system->fnLaplaceF[5][0]  = &fndSemiMajAxF6;
      system->fnLaplaceF[6][0]  = &fndSemiMajAxF7;
      system->fnLaplaceF[7][0]  = &fndSemiMajAxF8;
      system->fnLaplaceF[8][0]  = &fndSemiMajAxF9;
      system->fnLaplaceF[9][0]  = &fndSemiMajAxF10;
      system->fnLaplaceF[10][0] = &fndSemiMajAxF11;
      system->fnLaplaceF[11][0] = &fndSemiMajAxF12;
      system->fnLaplaceF[12][0] = &fndSemiMajAxF13;
      system->fnLaplaceF[13][0] = &fndSemiMajAxF14;
      system->fnLaplaceF[14][0] = &fndSemiMajAxF15;
      system->fnLaplaceF[15][0] = &fndSemiMajAxF16;
      system->fnLaplaceF[16][0] = &fndSemiMajAxF17;
      system->fnLaplaceF[17][0] = &fndSemiMajAxF18;
      system->fnLaplaceF[18][0] = &fndSemiMajAxF19;
      system->fnLaplaceF[19][0] = &fndSemiMajAxF20;
      system->fnLaplaceF[20][0] = &fndSemiMajAxF21;
      system->fnLaplaceF[21][0] = &fndSemiMajAxF22;
      system->fnLaplaceF[22][0] = &fndSemiMajAxF23;
      system->fnLaplaceF[23][0] = &fndSemiMajAxF24;
      system->fnLaplaceF[24][0] = &fndSemiMajAxF25;
      system->fnLaplaceF[25][0] = &fndSemiMajAxF26;

      system->fnLaplaceDeriv[0][0]  = &fndDSemiF1Dalpha;
      system->fnLaplaceDeriv[1][0]  = &fndDSemiF2Dalpha;
      system->fnLaplaceDeriv[2][0]  = &fndDSemiF3Dalpha;
      system->fnLaplaceDeriv[3][0]  = &fndDSemiF4Dalpha;
      system->fnLaplaceDeriv[4][0]  = &fndDSemiF5Dalpha;
      system->fnLaplaceDeriv[5][0]  = &fndDSemiF6Dalpha;
      system->fnLaplaceDeriv[6][0]  = &fndDSemiF7Dalpha;
      system->fnLaplaceDeriv[7][0]  = &fndDSemiF8Dalpha;
      system->fnLaplaceDeriv[8][0]  = &fndDSemiF9Dalpha;
      system->fnLaplaceDeriv[9][0]  = &fndDSemiF10Dalpha;
      system->fnLaplaceDeriv[10][0] = &fndDSemiF11Dalpha;
      system->fnLaplaceDeriv[11][0] = &fndDSemiF12Dalpha;
      system->fnLaplaceDeriv[12][0] = &fndDSemiF13Dalpha;
      system->fnLaplaceDeriv[13][0] = &fndDSemiF14Dalpha;
      system->fnLaplaceDeriv[14][0] = &fndDSemiF15Dalpha;
      system->fnLaplaceDeriv[15][0] = &fndDSemiF16Dalpha;
      system->fnLaplaceDeriv[16][0] = &fndDSemiF17Dalpha;
      system->fnLaplaceDeriv[17][0] = &fndDSemiF18Dalpha;
      system->fnLaplaceDeriv[18][0] = &fndDSemiF19Dalpha;
      system->fnLaplaceDeriv[19][0] = &fndDSemiF20Dalpha;
      system->fnLaplaceDeriv[20][0] = &fndDSemiF21Dalpha;
      system->fnLaplaceDeriv[21][0] = &fndDSemiF22Dalpha;
      system->fnLaplaceDeriv[22][0] = &fndDSemiF23Dalpha;
      system->fnLaplaceDeriv[23][0] = &fndDSemiF24Dalpha;
      system->fnLaplaceDeriv[24][0] = &fndDSemiF25Dalpha;
      system->fnLaplaceDeriv[25][0] = &fndDSemiF26Dalpha;

      system->daLaplaceC = malloc((1) * sizeof(double **));
      system->daLaplaceD = malloc((1) * sizeof(double **));
      system->daAlpha0   = malloc((1) * sizeof(double **));

      system->daLaplaceC[0] = malloc(
            fniNchoosek(control->Evolve.iNumBodies - 1, 2) * sizeof(double *));
      system->daLaplaceD[0] = malloc(
            fniNchoosek(control->Evolve.iNumBodies - 1, 2) * sizeof(double *));
      system->daAlpha0[0] = malloc(
            fniNchoosek(control->Evolve.iNumBodies - 1, 2) * sizeof(double *));

      for (i = 0; i < fniNchoosek(control->Evolve.iNumBodies - 1, 2); i++) {
        system->daLaplaceC[0][i] = malloc(LAPLNUM * sizeof(double));
        system->daLaplaceD[0][i] = malloc(LAPLNUM * sizeof(double));
        system->daAlpha0[0][i]   = malloc(LAPLNUM * sizeof(double));
      }

      system->iaLaplaceN = malloc((control->Evolve.iNumBodies) * sizeof(int *));
      for (i = 1; i < control->Evolve.iNumBodies; i++) {
        system->iaLaplaceN[i] =
              malloc((control->Evolve.iNumBodies) * sizeof(int));
      }
      if (control->bOutputEigen) {
        system->daEigenValEcc = malloc(2 * sizeof(double *));
        system->daEigenVecEcc =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double *));
        system->daEigenValInc = malloc(2 * sizeof(double *));
        system->daEigenVecInc =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double *));
        system->daEigenPhase = malloc(2 * sizeof(double *));

        system->daA =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double *));
        system->daB =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double *));
        system->daAsoln =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
        system->daBsoln =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
        system->daAcopy =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double *));

        for (jBody = 0; jBody < (control->Evolve.iNumBodies - 1); jBody++) {
          system->daA[jBody] =
                malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
          system->daB[jBody] =
                malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
          system->daEigenVecEcc[jBody] =
                malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
          system->daEigenVecInc[jBody] =
                malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
          system->daAcopy[jBody] =
                malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
        }

        for (i = 0; i < 2; i++) {
          system->daEigenValEcc[i] =
                malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
          system->daEigenValInc[i] =
                malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
          system->daEigenPhase[i] =
                malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
        }
        system->daScale =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
        system->iaRowswap =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(int));
      }
    }
    if (iBody >= 1) {
      VerifyPericenter(body, control, options, files->Infile[iBody + 1].cIn,
                       iBody, control->Io.iVerbose);
      body[iBody].iGravPerts =
            control->Evolve.iNumBodies -
            2; // will need to change this for zero mass particles in future

      VerifyPerturbersDistOrbRD4(body, control->Evolve.iNumBodies, iBody);
      control->fnPropsAux[iBody][iModule] = &PropsAuxDistOrb;

      CalcHK(body, iBody);
      CalcPQ(body, iBody);

      body[iBody].daLOrb    = malloc(3 * sizeof(double));
      body[iBody].daLOrbTmp = malloc(3 * sizeof(double));

      /* Body updates */
      for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
        /* h = Ecc*sin(LongP) */
        InitializeHeccDistOrbRD4(body, update, iBody, iPert);

        /* k = Ecc*cos(LongP) */
        InitializeKeccDistOrbRD4(body, update, iBody, iPert);

        /* p = s*sin(LongA) */
        InitializePincDistOrbRD4(body, update, iBody, iPert);

        /* q = s*cos(LongA) */
        InitializeQincDistOrbRD4(body, update, iBody, iPert);

        jBody = body[iBody].iaGravPerts[iPert];

        for (j = 0; j < 26; j++) {
          if (body[iBody].dSemi < body[jBody].dSemi) {
            system->iaLaplaceN[iBody][jBody] =
                  fniCombCount(iBody, jBody, control->Evolve.iNumBodies - 1);
            system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][j] =
                  system->fnLaplaceF[j][0](
                        body[iBody].dSemi / body[jBody].dSemi, 0);
            system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][j] =
                  system->fnLaplaceDeriv[j][0](
                        body[iBody].dSemi / body[jBody].dSemi, 0);
            system->daAlpha0[0][system->iaLaplaceN[iBody][jBody]][j] =
                  body[iBody].dSemi / body[jBody].dSemi;
          } else if (body[iBody].dSemi > body[jBody].dSemi) {
            system->iaLaplaceN[iBody][jBody] =
                  fniCombCount(jBody, iBody, control->Evolve.iNumBodies - 1);
            system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][j] =
                  system->fnLaplaceF[j][0](
                        body[jBody].dSemi / body[iBody].dSemi, 0);
            system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][j] =
                  system->fnLaplaceDeriv[j][0](
                        body[jBody].dSemi / body[iBody].dSemi, 0);
            system->daAlpha0[0][system->iaLaplaceN[iBody][jBody]][j] =
                  body[jBody].dSemi / body[iBody].dSemi;
          }
        }
      }
      if (iBody == control->Evolve.iNumBodies - 1) {
        if (control->bInvPlane) {
          /* Must initialize dMeanA to prevent memory corruption. This
             parameter has no real meaning in DistOrb runs, but inv_plave
             requires it. I will set it to zero for each body. --RKB */
          for (kBody = 0; kBody < control->Evolve.iNumBodies; kBody++) {
            body[kBody].dMeanA = 0;
          }
          inv_plane(body, system, control->Evolve.iNumBodies);
        }
      }
      if (body[iBody].bGRCorr) {
        /* Body updates for general relativistic correction, indexing star as a
         * "perturber"*/
        InitializeHeccDistOrbGR(body, update, iBody, body[iBody].iGravPerts);

        InitializeKeccDistOrbGR(body, update, iBody, body[iBody].iGravPerts);
      }
    }

    /* If the mutual inclination of any object gets above MAXMUTUALINCRD4,
       print a warning message. */
    // control->Io.dMaxMutualInc = MAXMUTUALINCRD4 * PI/180.;
    control->Io.dMaxMutualInc = 35 * 3.1415926535 / 180;
    double dFoo               = 35 * 3.1415926535 / 180;

  } else if (control->Evolve.iDistOrbModel == LL2) {
    VerifyPericenter(body, control, options, files->Infile[iBody + 1].cIn,
                     iBody, control->Io.iVerbose);
    control->fnPropsAux[iBody][iModule] = &PropsAuxDistOrb;

    CalcHK(body, iBody);
    CalcPQ(body, iBody);
  
    if (iBody == 1) {
      system->daLOrb = malloc(3 * sizeof(double));

      system->daLaplaceD = malloc(1 * sizeof(double *));
      system->daAlpha0   = malloc(1 * sizeof(double *));

      system->iaLaplaceN =
            malloc((control->Evolve.iNumBodies) * sizeof(int *));
      system->daAlpha0[0] =
            malloc(fniNchoosek(control->Evolve.iNumBodies - 1, 2) *
                    sizeof(double *));
      system->daLaplaceD[0] =
            malloc(fniNchoosek(control->Evolve.iNumBodies - 1, 2) *
                    sizeof(double *));
      for (j = 0; j < fniNchoosek(control->Evolve.iNumBodies - 1, 2); j++) {
        system->daLaplaceD[0][j] = malloc(2 * sizeof(double));
        system->daAlpha0[0][j]   = malloc(1 * sizeof(double));
      }
      for (j = 1; j < (control->Evolve.iNumBodies); j++) {
        system->iaLaplaceN[j] =
              malloc((control->Evolve.iNumBodies) * sizeof(int));
      }
    }

    for (jBody = 1; jBody < (control->Evolve.iNumBodies); jBody++) {
      body[jBody].daLOrb    = malloc(3 * sizeof(double));
      body[jBody].daLOrbTmp = malloc(3 * sizeof(double));

      if (body[iBody].dSemi < body[jBody].dSemi) {
        system->iaLaplaceN[iBody][jBody] =
              fniCombCount(iBody, jBody, control->Evolve.iNumBodies - 1);
        system->daAlpha0[0][system->iaLaplaceN[iBody][jBody]][0] =
              body[iBody].dSemi / body[jBody].dSemi;
        system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][0] =
              fndDerivLaplaceCoeff(1, body[iBody].dSemi / body[jBody].dSemi,
                                    1, 1.5);
        system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][1] =
              fndDerivLaplaceCoeff(1, body[iBody].dSemi / body[jBody].dSemi,
                                    2, 1.5);
      } else if (body[iBody].dSemi > body[jBody].dSemi) {
        system->iaLaplaceN[iBody][jBody] =
              fniCombCount(jBody, iBody, control->Evolve.iNumBodies - 1);
        system->daAlpha0[0][system->iaLaplaceN[iBody][jBody]][0] =
              body[jBody].dSemi / body[iBody].dSemi;
        system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][0] =
              fndDerivLaplaceCoeff(1, body[jBody].dSemi / body[iBody].dSemi,
                                    1, 1.5);
        system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][1] =
              fndDerivLaplaceCoeff(1, body[jBody].dSemi / body[iBody].dSemi,
                                    2, 1.5);
      }
    }

    if (iBody == (control->Evolve.iNumBodies - 1)) {
      VerifyGRCorrLL2(body, control->Evolve.iNumBodies);
      if (control->bInvPlane) {
        inv_plane(body, system, control->Evolve.iNumBodies);
      }
      system->daEigenValEcc = malloc(2 * sizeof(double *));
      system->daEigenVecEcc =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double *));
      system->daEigenValInc = malloc(2 * sizeof(double *));
      system->daEigenVecInc =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double *));
      system->daEigenPhase = malloc(2 * sizeof(double *));

      system->daA =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double *));
      system->daB =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double *));
      system->daAsoln =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
      system->daBsoln =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
      system->daAcopy =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double *));

      for (jBody = 0; jBody < (control->Evolve.iNumBodies - 1); jBody++) {
        system->daA[jBody] =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
        system->daB[jBody] =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
        system->daEigenVecEcc[jBody] =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
        system->daEigenVecInc[jBody] =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
        system->daAcopy[jBody] =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
      }

      for (i = 0; i < 2; i++) {
        system->daEigenValEcc[i] =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
        system->daEigenValInc[i] =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
        system->daEigenPhase[i] =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
      }
      system->daScale =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
      system->iaRowswap =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(int));


      SolveEigenVal(body, &control->Evolve, system);

      system->daetmp =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double *));
      system->daitmp =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double *));
      system->iaRowswap =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(int));
      system->dah0 =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
      system->dak0 =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
      system->dap0 =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
      system->daq0 =
            malloc((control->Evolve.iNumBodies - 1) * sizeof(double));

      for (i = 0; i < (control->Evolve.iNumBodies - 1); i++) {
        system->daetmp[i] =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
        system->daitmp[i] =
              malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
      }
      system->daS = malloc((control->Evolve.iNumBodies - 1) * sizeof(double));
      system->daT = malloc((control->Evolve.iNumBodies - 1) * sizeof(double));

      ScaleEigenVec(body, &control->Evolve, system);
    }


    body[iBody].iGravPerts = control->Evolve.iNumBodies - 1;
    VerifyPerturbersDistOrbLL2(body, control->Evolve.iNumBodies, iBody);

    for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
      /* h = Ecc*sin(LongP) */
      InitializeHeccDistOrbLL2(body, update, iBody, iPert);

      /* k = Ecc*cos(LongP) */
      InitializeKeccDistOrbLL2(body, update, iBody, iPert);

      /* p = s*sin(LongA) */
      InitializePincDistOrbLL2(body, update, iBody, iPert);

      /* q = s*cos(LongA) */
      InitializeQincDistOrbLL2(body, update, iBody, iPert);
    //}

      /* If the mutual inclination of any object gets above MAXMUTUALINCLL2,
        print a warning message. */
    control->Io.dMaxMutualInc = MAXMUTUALINCLL2 * PI / 180;

      // if (body[iBody].bGRCorr) {
      //       fprintf(stderr,"ERROR: %s cannot be used in LL2 orbital
      //       solution.\n",options[OPT_GRCORR].cName);
      //       LineExit(files->Infile[iBody+1].cIn,options[OPT_GRCORR].iLine[iBody+1]);
    }
  } // DistorbModel == LL2

  control->fnForceBehavior[iBody][iModule]   = &ForceBehaviorDistOrb;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyDistOrb;
}


/***************** DISTORB Update *****************/
void InitializeUpdateDistOrb(BODY *body, UPDATE *update, int iBody) {
  if (iBody > 0) {
    if (update[iBody].iNumHecc == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumHecc += body[iBody].iGravPerts;

    if (update[iBody].iNumKecc == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumKecc += body[iBody].iGravPerts;

    if (update[iBody].iNumPinc == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumPinc += body[iBody].iGravPerts;

    if (update[iBody].iNumQinc == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumQinc += body[iBody].iGravPerts;

    if (body[iBody].bGRCorr) {
      if (body[iBody].iDistOrbModel == RD4) {
        update[iBody].iNumHecc += 1;
        update[iBody].iNumKecc += 1;
      }
    }
  }
}

void FinalizeUpdateHeccDistOrb(BODY *body, UPDATE *update, int *iEqn, int iVar,
                               int iBody, int iFoo) {
  /* The indexing gets a bit confusing here. iPert = 0 to iGravPerts-1
   * correspond to all perturbing planets, iPert = iGravPerts corresponds to the
   * stellar general relativistic correction, if applied */
   /* XXX This should be changed to body[iBody].iGravPerts knows before hecc and kecc 
   are initialized in update.c if the GR correction has been applied! */

  int iPert;

  update[iBody].iaModule[iVar][*iEqn] = DISTORB;
  if (body[iBody].bGRCorr) {
    update[iBody].padDHeccDtDistOrb =
          malloc((body[iBody].iGravPerts + 1) * sizeof(double *));
    update[iBody].iaHeccDistOrb =
          malloc((body[iBody].iGravPerts + 1) * sizeof(int));
    for (iPert = 0; iPert < body[iBody].iGravPerts + 1; iPert++) {

      update[iBody].iaHeccDistOrb[iPert]  = (*iEqn)++;
    }
  } else {
    update[iBody].padDHeccDtDistOrb =
          malloc(body[iBody].iGravPerts * sizeof(double *));
    update[iBody].iaHeccDistOrb = malloc(body[iBody].iGravPerts * sizeof(int));
    for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
      update[iBody].iaHeccDistOrb[iPert]  = (*iEqn)++;
    }
  }
}

void FinalizeUpdateKeccDistOrb(BODY *body, UPDATE *update, int *iEqn, int iVar,
                               int iBody, int iFoo) {
  /* The indexing gets a bit confusing here. iPert = 0 to iGravPerts-1
   * correspond to all perturbing planets, iPert = iGravPerts corresponds to the
   * stellar general relativistic correction, if applied */

  int iPert;

  update[iBody].iaModule[iVar][*iEqn] = DISTORB;
  if (body[iBody].bGRCorr) {
    update[iBody].padDKeccDtDistOrb =
          malloc((body[iBody].iGravPerts + 1) * sizeof(double *));
    update[iBody].iaKeccDistOrb =
          malloc((body[iBody].iGravPerts + 1) * sizeof(int));
    for (iPert = 0; iPert < body[iBody].iGravPerts + 1; iPert++) {
 
      update[iBody].iaKeccDistOrb[iPert]  = (*iEqn)++;
    }
  } else {
    update[iBody].padDKeccDtDistOrb =
          malloc(body[iBody].iGravPerts * sizeof(double *));
    update[iBody].iaKeccDistOrb = malloc(body[iBody].iGravPerts * sizeof(int));
    for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
      update[iBody].iaKeccDistOrb[iPert]  = (*iEqn)++;
    }
  }
}

void FinalizeUpdatePincDistOrb(BODY *body, UPDATE *update, int *iEqn, int iVar,
                               int iBody, int iFoo) {
  int iPert;

  update[iBody].padDPincDtDistOrb =
        malloc(body[iBody].iGravPerts * sizeof(double *));
  update[iBody].iaPincDistOrb = malloc(body[iBody].iGravPerts * sizeof(int));
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTORB;
    update[iBody].iaPincDistOrb[iPert]  = (*iEqn)++;
  }
}

void FinalizeUpdateQincDistOrb(BODY *body, UPDATE *update, int *iEqn, int iVar,
                               int iBody, int iFoo) {
  int iPert;

  update[iBody].padDQincDtDistOrb =
        malloc(body[iBody].iGravPerts * sizeof(double *));
  update[iBody].iaQincDistOrb = malloc(body[iBody].iGravPerts * sizeof(int));
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTORB;
    update[iBody].iaQincDistOrb[iPert]  = (*iEqn)++;
  }
}


/***************** DISTORB Halts *****************/

void CountHaltsDistOrb(HALT *halt, int *iNumHalts) {
  if (halt->bOverrideMaxEcc == 0) {
    if (halt->dMaxEcc == 1) {
      (*iNumHalts)++;
    }
  }
  /* halt for close encounters */
  if (halt->bHillStab || halt->bCloseEnc) {
    (*iNumHalts)++;
  }
}

void VerifyHaltDistOrb(BODY *body, CONTROL *control, OPTIONS *options,
                       int iBody, int *iHalt) {
  int iHaltMaxEcc = 0, iNumMaxEcc = 0; // If set for one body, set for all

  /* Mandatory halt for DistOrb */
  if (body[iBody].bDistOrb) {
    if (control->Halt[iBody].bOverrideMaxEcc == 0) {
      /* If you don't override max ecc, and you HAVEN'T set it manually for this
       * body, default to MAXECCDISTORB (== 0.6627434) */
      if (control->Halt[iBody].dMaxEcc == 1) {
        control->Halt[iBody].dMaxEcc       = MAXECCDISTORB;
        control->fnHalt[iBody][(*iHalt)++] = &fniHaltMaxEcc;
      }
    }
    //     control->Halt[iBody].bHillStab = HILLSTABDISTORB;
    if (control->Halt[iBody].bHillStab) {
      control->fnHalt[iBody][(*iHalt)++] = &fniHaltHillStab;
    } else if (control->Halt[iBody].bCloseEnc) {
      control->fnHalt[iBody][(*iHalt)++] = &fniHaltCloseEnc;
    }
  }
}

int fniHaltHillStab(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                    UPDATE *update, fnUpdateVariable ***fnUpdate, int iBody) {
  int iPert, jBody;
  double mu1, mu2, alpha, gamma1, gamma2, delta, crit, hill;

  if (halt->bHillStab == 1) {
    for (iBody = 1; iBody < evolve->iNumBodies; iBody++) {
      /* I have to loop over iBody here, ultimately because I want to have to
         set bHaltHillStab only once, not in every file. There is no easy way to
         set bHillStab for one body and distribute that to all others before the
         number of halts are counted. So, I just set it once and check Hill
         stability for all planets during the one call of HaltHillStab(). */
      for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
        jBody = body[iBody].iaGravPerts[iPert];
        if (body[jBody].dSemi < body[iBody].dSemi) {
          // jBody is the inner planet
          mu1    = body[jBody].dMass / body[0].dMass;
          mu2    = body[iBody].dMass / body[0].dMass;
          gamma1 = sqrt(1 - (body[jBody].dHecc * body[jBody].dHecc +
                             body[jBody].dKecc * body[jBody].dKecc));
          gamma2 = sqrt(1 - (body[iBody].dHecc * body[iBody].dHecc +
                             body[iBody].dKecc * body[iBody].dKecc));
          delta  = sqrt(body[iBody].dSemi / body[jBody].dSemi);
        } else {
          // jBody is the outer planet
          mu2    = body[jBody].dMass / body[0].dMass;
          mu1    = body[iBody].dMass / body[0].dMass;
          gamma2 = sqrt(1 - (body[jBody].dHecc * body[jBody].dHecc +
                             body[jBody].dKecc * body[jBody].dKecc));
          gamma1 = sqrt(1 - (body[iBody].dHecc * body[iBody].dHecc +
                             body[iBody].dKecc * body[iBody].dKecc));
          delta  = sqrt(body[jBody].dSemi / body[iBody].dSemi);
        }
        alpha = mu1 + mu2;
        crit  = 1.0 + pow((3. / alpha), (4. / 3)) * mu1 * mu2;
        hill  = pow(alpha, -3) * (mu1 + mu2 / (delta * delta)) *
               (mu1 * gamma1 + mu2 * gamma2 * delta) *
               (mu1 * gamma1 + mu2 * gamma2 * delta);
        if (hill < crit) {
          if (io->iVerbose >= VERBPROG) {
            printf(
                  "HALT: hill stability criterion failed for planets %d and %d",
                  iBody, jBody);
            printf(" at %.2e years\n", evolve->dTime / YEARSEC);
          }
          return 1;
        }
      }
    }
  }
  return 0;
}

int fniHaltCloseEnc(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                    UPDATE *update, fnUpdateVariable ***fnUpdate, int iBody) {
  int iPert, jBody;
  double dDR;

  if (halt->bCloseEnc == 1) {
    for (iBody = 1; iBody < evolve->iNumBodies; iBody++) {
      for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
        jBody = body[iBody].iaGravPerts[iPert];
        if (body[jBody].dSemi < body[iBody].dSemi) {
          // comparing apocentre of inner planet with pericentre of outer
          dDR = fabs(body[iBody].dRPeri - body[jBody].dRApo);
          if (dDR < 4 * fndMutualHillRad(body, iBody, jBody)) {
            if (io->iVerbose >= VERBPROG) {
              printf("HALT: close encounter between planets %d and %d = ",
                     iBody, jBody);
              printf(" at %.2e years\n", evolve->dTime / YEARSEC);
            }
            return 1;
          }
        } else if (body[jBody].dSemi > body[iBody].dSemi) {
          // comparing apocentre of inner planet with pericentre of outer
          dDR = fabs(body[jBody].dRPeri - body[iBody].dRApo);
          if (dDR < 4 * fndMutualHillRad(body, iBody, jBody)) {
            if (io->iVerbose >= VERBPROG) {
              printf("HALT: close encounter between planets %d and %d = ",
                     iBody, jBody);
              printf(" at %.2e years\n", evolve->dTime / YEARSEC);
            }
            return 1;
          }
        }
      }
    }
  }

  return 0;
}

/**
  Check the maximum allowed mutual inclination when DistOrb is active.

@param body A pointer to the current BODY instance
@param evolve A pointer to the integration EVOLVE instance
@param halt A pointer to the HALT instance
@param io A pointer to the IO instance
@param update A pointer to the UPDATE instance
@param iBody The current index in the BODY instance, irrelevant in this case
  because mutual inclination is by definition a multi-body variable

@return TRUE if one mutual incliantion in a system is larger than
  dHaltMaxMutualInc, FALSE if not
*/
int fbHaltMaxMutualIncDistorb(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                              UPDATE *update, fnUpdateVariable ***fnUpdate,
                              int iBody) {

  int jBody;

  for (iBody = 1; iBody < evolve->iNumBodies; iBody++) {
    for (jBody = iBody + 1; jBody < evolve->iNumBodies; jBody++) {
      // 0 is to check for halt, not progress
      if (fbCheckMaxMutualInc(body, evolve, halt, io, iBody, jBody, 0)) {
        return 1;
      }
    }
  }

  return 0;
}

/**
  Check if mutual inclination exceeds the warning limit

@param body A pointer to the current BODY instance
@param evolve A pointer to the integration EVOLVE instance
@param halt A pointer to the HALT instance
@param io A pointer to the IO instance
@param update A pointer to the UPDATE instance
@param iBody The current index in the BODY instance, irrelevant in this case
  because mutual inclination is by definition a multi-body variable

@return TRUE if one mutual incliantion in a system is larger than
  dHaltMaxMutualInc, FALSE if not
*/
int fbCheckMutualIncDistorb(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                            UPDATE *update, int iBody) {

  int jBody;

  for (iBody = 0; iBody < evolve->iNumBodies; iBody++) {
    for (jBody = iBody + 1; jBody < evolve->iNumBodies; jBody++) {
      // 1 is to check for halt, not progress
      if (fbCheckMaxMutualInc(body, evolve, halt, io, iBody, jBody, 1)) {
        return 1;
      }
    }
  }

  return 0;
}


/************* DISTORB Outputs ******************/

void WriteEigen(CONTROL *control, SYSTEM *system) {
  char cEccEigFile[2 * NAMELEN], cIncEigFile[2 * NAMELEN];
  int iBody;
  FILE *fecc, *finc;

  sprintf(cEccEigFile, "%s.Ecc.Eigen", system->cName);
  sprintf(cIncEigFile, "%s.Inc.Eigen", system->cName);

  if (control->Evolve.dTime == 0) {
    fecc = fopen(cEccEigFile, "w");
    finc = fopen(cIncEigFile, "w");
  } else {
    fecc = fopen(cEccEigFile, "a");
    finc = fopen(cIncEigFile, "a");
  }

  fprintd(fecc, control->Evolve.dTime / fdUnitsTime(control->Units[1].iTime),
          control->Io.iSciNot, control->Io.iDigits);
  fprintf(fecc, " ");
  fprintd(finc, control->Evolve.dTime / fdUnitsTime(control->Units[1].iTime),
          control->Io.iSciNot, control->Io.iDigits);
  fprintf(finc, " ");

  for (iBody = 1; iBody < (control->Evolve.iNumBodies); iBody++) {
    fprintd(fecc, system->daEigenValEcc[0][iBody - 1], control->Io.iSciNot,
            control->Io.iDigits);
    fprintf(fecc, " ");
    fprintd(finc, system->daEigenValInc[0][iBody - 1], control->Io.iSciNot,
            control->Io.iDigits);
    fprintf(finc, " ");
  }
  fprintf(fecc, "\n");
  fprintf(finc, "\n");
  fclose(fecc);
  fclose(finc);
}

void WriteBodyDEccDtDistOrb(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    dDeriv += 1. /
              sqrt(body[iBody].dHecc * body[iBody].dHecc +
                   body[iBody].dKecc * body[iBody].dKecc) *
              (body[iBody].dHecc * (*(update[iBody].padDHeccDtDistOrb[iPert])) +
               body[iBody].dKecc * (*(update[iBody].padDKeccDtDistOrb[iPert])));
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

void WriteBodyDSincDtDistOrb(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    if (body[iBody].dPinc != 0 && body[iBody].dQinc != 0 &&
        *(update[iBody].padDPincDtDistOrb[iPert]) != 0 &&
        *(update[iBody].padDQincDtDistOrb[iPert])) {
      dDeriv +=
            1. /
            sqrt(body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc) *
            (body[iBody].dPinc * (*(update[iBody].padDPincDtDistOrb[iPert])) +
             body[iBody].dQinc * (*(update[iBody].padDQincDtDistOrb[iPert])));
    } else {
      dDeriv = 0;
    }
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

void WriteBodyDLongPDtDistOrb(BODY *body, CONTROL *control, OUTPUT *output,
                              SYSTEM *system, UNITS *units, UPDATE *update,
                              int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    dDeriv += 1. /
              (body[iBody].dHecc * body[iBody].dHecc +
               body[iBody].dKecc * body[iBody].dKecc) *
              (body[iBody].dKecc * (*(update[iBody].padDHeccDtDistOrb[iPert])) -
               body[iBody].dHecc * (*(update[iBody].padDKeccDtDistOrb[iPert])));
  }

  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    *dTmp /= fdUnitsAngle(units->iAngle);
    //     fsUnitsAngle(units->iAngle,cUnit);
    fsUnitsAngRate(units, cUnit);
  }
}

void WriteBodyDLongADtDistOrb(BODY *body, CONTROL *control, OUTPUT *output,
                              SYSTEM *system, UNITS *units, UPDATE *update,
                              int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    if (body[iBody].dPinc != 0 && body[iBody].dQinc != 0 &&
        *(update[iBody].padDPincDtDistOrb[iPert]) != 0 &&
        *(update[iBody].padDQincDtDistOrb[iPert])) {
      dDeriv +=
            1. /
            (body[iBody].dPinc * body[iBody].dPinc +
             body[iBody].dQinc * body[iBody].dQinc) *
            (body[iBody].dQinc * (*(update[iBody].padDPincDtDistOrb[iPert])) -
             body[iBody].dPinc * (*(update[iBody].padDQincDtDistOrb[iPert])));
    } else {
      dDeriv = 0;
    }
  }
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    *dTmp /= fdUnitsAngle(units->iAngle);
    //     fsUnitsAngle(units->iAngle,cUnit);
    fsUnitsAngRate(units, cUnit);
  }
}

void WriteBodyDIncDtDistOrb(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    if (body[iBody].dPinc != 0 && body[iBody].dQinc != 0 &&
        *(update[iBody].padDPincDtDistOrb[iPert]) != 0 &&
        *(update[iBody].padDQincDtDistOrb[iPert])) {
      dDeriv +=
            2. /
            sqrt((1 - (body[iBody].dPinc * body[iBody].dPinc +
                       body[iBody].dQinc * body[iBody].dQinc)) *
                 (body[iBody].dPinc * body[iBody].dPinc +
                  body[iBody].dQinc * body[iBody].dQinc)) *
            (body[iBody].dPinc * (*(update[iBody].padDPincDtDistOrb[iPert])) +
             body[iBody].dQinc * (*(update[iBody].padDQincDtDistOrb[iPert])));
    } else {
      dDeriv = 0;
    }
  }
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    *dTmp /= fdUnitsAngle(units->iAngle);
    //     fsUnitsAngle(units->iAngle,cUnit);
    fsUnitsAngRate(units, cUnit);
  }
}

void WriteBodySinc(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  *dTmp = sqrt(body[iBody].dPinc * body[iBody].dPinc +
               body[iBody].dQinc * body[iBody].dQinc);

  strcpy(cUnit, "");
}

/*
void WriteBodyLongA(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM
*system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) { if
(body[iBody].bDistOrb) { *dTmp = atan2(body[iBody].dPinc, body[iBody].dQinc); }
else if (body[iBody].bGalHabit) { *dTmp = body[iBody].dLongA;
  }

  while (*dTmp < 0.0) {
    *dTmp += 2*PI;
  }
  while (*dTmp > 2*PI) {
    *dTmp -= 2*PI;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}
*/

void WriteBodyPinc(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  *dTmp = body[iBody].dPinc;
  strcpy(cUnit, "");
}

void WriteBodyQinc(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  *dTmp = body[iBody].dQinc;
  strcpy(cUnit, "");
}

void WriteBodyDHeccDtDistOrb(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  /* need to put check for star's output options in verify */
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    dDeriv += *(update[iBody].padDHeccDtDistOrb[iPert]);
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

void WriteBodyDKeccDtDistOrb(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  /* need to put check for star's output options in verify */
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    dDeriv += *(update[iBody].padDKeccDtDistOrb[iPert]);
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

void WriteBodyDPincDtDistOrb(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  /* need to put check for star's output options in verify */
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    dDeriv += *(update[iBody].padDPincDtDistOrb[iPert]);
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

void WriteBodyDQincDtDistOrb(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  /* need to put check for star's output options in verify */
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite pdDrotDt */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    dDeriv += *(update[iBody].padDQincDtDistOrb[iPert]);
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


void InitializeOutputDistOrb(OUTPUT *output, fnWriteOutput fnWrite[]) {

  // XXX Many of these should be moved to OutputGeneral as they apply to both
  // DistOrb and SpiNBody

  sprintf(output[OUT_DECCDTDISTORB].cName, "DEccDtDistOrb");
  sprintf(output[OUT_DECCDTDISTORB].cDescr,
          "Body's eccentricity derivative in DistOrb");
  sprintf(output[OUT_DECCDTDISTORB].cNeg, "1/year");
  output[OUT_DECCDTDISTORB].bNeg       = 1;
  output[OUT_DECCDTDISTORB].dNeg       = YEARSEC;
  output[OUT_DECCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DECCDTDISTORB]           = &WriteBodyDEccDtDistOrb;

  sprintf(output[OUT_DSINCDTDISTORB].cName, "DSincDtDistOrb");
  sprintf(output[OUT_DSINCDTDISTORB].cDescr,
          "Body's sin(Inc/2) derivative in DistOrb");
  sprintf(output[OUT_DSINCDTDISTORB].cNeg, "1/year");
  output[OUT_DSINCDTDISTORB].bNeg       = 1;
  output[OUT_DSINCDTDISTORB].dNeg       = YEARSEC;
  output[OUT_DSINCDTDISTORB].iNum       = 1;
  output[OUT_DSINCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DSINCDTDISTORB]           = &WriteBodyDSincDtDistOrb;

  sprintf(output[OUT_DINCDTDISTORB].cName, "DIncDtDistOrb");
  sprintf(output[OUT_DINCDTDISTORB].cDescr,
          "Body's inclination derivative in DistOrb");
  sprintf(output[OUT_DINCDTDISTORB].cNeg, "deg/year");
  output[OUT_DINCDTDISTORB].bNeg       = 1;
  output[OUT_DINCDTDISTORB].dNeg       = YEARSEC / DEGRAD;
  output[OUT_DINCDTDISTORB].iNum       = 1;
  output[OUT_DINCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DINCDTDISTORB]           = &WriteBodyDIncDtDistOrb;

  sprintf(output[OUT_DLONGPDTDISTORB].cName, "DLongPDtDistOrb");
  sprintf(output[OUT_DLONGPDTDISTORB].cDescr,
          "Body's longitude of pericenter derivative in DistOrb");
  sprintf(output[OUT_DLONGPDTDISTORB].cNeg, "deg/yr");
  output[OUT_DLONGPDTDISTORB].bNeg       = 1;
  output[OUT_DLONGPDTDISTORB].dNeg       = YEARSEC / DEGRAD;
  output[OUT_DLONGPDTDISTORB].iNum       = 1;
  output[OUT_DLONGPDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DLONGPDTDISTORB]           = &WriteBodyDLongPDtDistOrb;

  sprintf(output[OUT_DLONGADTDISTORB].cName, "DLongADtDistOrb");
  sprintf(output[OUT_DLONGADTDISTORB].cDescr,
          "Body's longitude of ascending node derivative in DistOrb");
  sprintf(output[OUT_DLONGADTDISTORB].cNeg, "deg/yr");
  output[OUT_DLONGADTDISTORB].bNeg       = 1;
  output[OUT_DLONGADTDISTORB].dNeg       = YEARSEC / DEGRAD;
  output[OUT_DLONGADTDISTORB].iNum       = 1;
  output[OUT_DLONGADTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DLONGADTDISTORB]           = &WriteBodyDLongADtDistOrb;

  sprintf(output[OUT_SINC].cName, "Sinc");
  sprintf(output[OUT_SINC].cDescr, "Body's sin(Inc/2) in DistOrb");
  output[OUT_SINC].iNum       = 1;
  output[OUT_SINC].iModuleBit = DISTORB;
  fnWrite[OUT_SINC]           = &WriteBodySinc;

  sprintf(output[OUT_PINC].cName, "Pinc");
  sprintf(output[OUT_PINC].cDescr, "Body's Poincare p in DistOrb");
  output[OUT_PINC].iNum       = 1;
  output[OUT_PINC].iModuleBit = DISTORB;
  fnWrite[OUT_PINC]           = &WriteBodyPinc;

  sprintf(output[OUT_QINC].cName, "Qinc");
  sprintf(output[OUT_QINC].cDescr, "Body's Poincare q in DistOrb");
  output[OUT_QINC].iNum       = 1;
  output[OUT_QINC].iModuleBit = DISTORB;
  fnWrite[OUT_QINC]           = &WriteBodyQinc;

  sprintf(output[OUT_DHECCDTDISTORB].cName, "DHeccDtDistOrb");
  sprintf(output[OUT_DHECCDTDISTORB].cDescr,
          "Body's Poincare h derivative in DistOrb");
  sprintf(output[OUT_DHECCDTDISTORB].cNeg, "1/year");
  output[OUT_DHECCDTDISTORB].bNeg       = 1;
  output[OUT_DHECCDTDISTORB].dNeg       = YEARSEC;
  output[OUT_DHECCDTDISTORB].iNum       = 1;
  output[OUT_DHECCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DHECCDTDISTORB]           = &WriteBodyDHeccDtDistOrb;

  sprintf(output[OUT_DKECCDTDISTORB].cName, "DKeccDtDistOrb");
  sprintf(output[OUT_DKECCDTDISTORB].cDescr, "Body's Poincare k in DistOrb");
  sprintf(output[OUT_DKECCDTDISTORB].cNeg, "1/year");
  output[OUT_DKECCDTDISTORB].bNeg       = 1;
  output[OUT_DKECCDTDISTORB].dNeg       = YEARSEC;
  output[OUT_DKECCDTDISTORB].iNum       = 1;
  output[OUT_DKECCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DKECCDTDISTORB]           = &WriteBodyDKeccDtDistOrb;

  sprintf(output[OUT_DPINCDTDISTORB].cName, "DPincDtDistOrb");
  sprintf(output[OUT_DPINCDTDISTORB].cDescr, "Body's Poincare p in DistOrb");
  sprintf(output[OUT_DPINCDTDISTORB].cNeg, "1/year");
  output[OUT_DPINCDTDISTORB].bNeg       = 1;
  output[OUT_DPINCDTDISTORB].dNeg       = YEARSEC;
  output[OUT_DPINCDTDISTORB].iNum       = 1;
  output[OUT_DPINCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DPINCDTDISTORB]           = &WriteBodyDPincDtDistOrb;

  sprintf(output[OUT_DQINCDTDISTORB].cName, "DQincDtDistOrb");
  sprintf(output[OUT_DQINCDTDISTORB].cDescr, "Body's Poincare q in DistOrb");
  sprintf(output[OUT_DQINCDTDISTORB].cNeg, "1/year");
  output[OUT_DQINCDTDISTORB].bNeg       = 1;
  output[OUT_DQINCDTDISTORB].dNeg       = YEARSEC;
  output[OUT_DQINCDTDISTORB].iNum       = 1;
  output[OUT_DQINCDTDISTORB].iModuleBit = DISTORB;
  fnWrite[OUT_DQINCDTDISTORB]           = &WriteBodyDQincDtDistOrb;
}

/************ DISTORB Logging Functions **************/

void LogOptionsDistOrb(CONTROL *control, FILE *fp) {

  fprintf(fp, "-------- DISTORB Options -----\n\n");
}

void LogDistOrb(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {
  int iOut;

  fprintf(fp, "\n----- DISTORB PARAMETERS ------\n");
  for (iOut = OUTSTARTDISTORB; iOut < OUTBODYSTARTDISTORB; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, 0);
    }
  }
}

void LogBodyDistOrb(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UPDATE *update, fnWriteOutput fnWrite[],
                    FILE *fp, int iBody) {
  int iOut;

  fprintf(fp, "----- DISTORB PARAMETERS (%s)------\n", body[iBody].cName);
  for (iOut = OUTBODYSTARTDISTORB; iOut < OUTENDDISTORB; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
  }
}

/************* MODULE Functions ***********/

void AddModuleDistOrb(CONTROL *control, MODULE *module, int iBody,
                      int iModule) {

  module->iaModule[iBody][iModule] = DISTORB;

  module->fnInitializeUpdateTmpBody[iBody][iModule] =
        &InitializeUpdateTmpBodyDistOrb;
  module->fnCountHalts[iBody][iModule] = &CountHaltsDistOrb;
  module->fnLogBody[iBody][iModule]    = &LogBodyDistOrb;

  module->fnReadOptions[iBody][iModule]       = &ReadOptionsDistOrb;
  module->fnVerify[iBody][iModule]            = &VerifyDistOrb;
  module->fnAssignDerivatives[iBody][iModule] = &AssignDistOrbDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullDistOrbDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltDistOrb;

  module->fnInitializeBody[iBody][iModule]     = &InitializeBodyDistOrb;
  module->fnInitializeUpdate[iBody][iModule]   = &InitializeUpdateDistOrb;
  module->fnInitializeOutput[iBody][iModule]   = &InitializeOutputDistOrb;
  module->fnFinalizeUpdateHecc[iBody][iModule] = &FinalizeUpdateHeccDistOrb;
  module->fnFinalizeUpdateKecc[iBody][iModule] = &FinalizeUpdateKeccDistOrb;
  module->fnFinalizeUpdatePinc[iBody][iModule] = &FinalizeUpdatePincDistOrb;
  module->fnFinalizeUpdateQinc[iBody][iModule] = &FinalizeUpdateQincDistOrb;
}

/************* DistOrb Functions ************/
void PropsAuxDistOrb(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                     int iBody) {
  body[iBody].dLongP = atan2(body[iBody].dHecc, body[iBody].dKecc);
  body[iBody].dEcc   = sqrt(body[iBody].dHecc * body[iBody].dHecc +
                            body[iBody].dKecc * body[iBody].dKecc);
  body[iBody].dSinc  = sqrt(body[iBody].dPinc * body[iBody].dPinc +
                            body[iBody].dQinc * body[iBody].dQinc);

  body[iBody].dRPeri = (1.0 - sqrt(body[iBody].dHecc * body[iBody].dHecc +
                                   body[iBody].dKecc * body[iBody].dKecc)) *
                       body[iBody].dSemi;
  body[iBody].dRApo = (1.0 + sqrt(body[iBody].dHecc * body[iBody].dHecc +
                                  body[iBody].dKecc * body[iBody].dKecc)) *
                      body[iBody].dSemi;
}


void ForceBehaviorDistOrb(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                          SYSTEM *system, UPDATE *update,
                          fnUpdateVariable ***fnUpdate, int iBody,
                          int iModule) {
}

/* Factorial function. Nuff sed. */
unsigned long int fniFactorial(unsigned int n) {
  unsigned long int result;

  if (n == 0) {
    result = 1;
  } else {
    result = n * fniFactorial(n - 1);
  }
  return result;
}

/**
Number of combinations of k in N

@param N Size of set
@param k Size of desired subset
@return Binomial coefficient N choose k
*/
int fniNchoosek(int N, int k) {
  if (N < 0 || k < 0 || N > 10 || k > N) {
    printf("Error: received N = %d, k = %d\n", N, k);
  }
  return fniFactorial(N) / (fniFactorial(k) * fniFactorial(N - k));
}

/**
Gives the index of a pair of values in N choose 2.
For example, for 4 planets, the index for the pair
(1,2) -> 0, (1,3) -> 1, (1,4) -> 2, (2,3) -> 3, etc.

@param x Index of first planet
@param y Index of second planet
@param N Number of planets in system
@return Index corresponding to planet pair (x,y)
*/
int fniCombCount(int x, int y, int N) {
  if (x < y) {
    return N * (x - 1) + (y - 1) - fniNchoosek(x + 1, 2);
  } else {
    return N * (y - 1) + (x - 1) - fniNchoosek(y + 1, 2);
  }
}

/**
Calculates components AB matrices to find eigenvalues in LL2 problem

@param body Struct containing all body information and variables
@param j Index j in Laplace coefficients
@param jBody Index of perturbed body
@param kBody Index of perturbing body
@return Component of A or B matrix in rad/year
*/
double fndABmatrix(BODY *body, int j, int jBody, int kBody) {
  double AB, alpha, abar, b, n;

  if (body[jBody].dSemi > body[kBody].dSemi) {
    alpha = body[kBody].dSemi / body[jBody].dSemi; // internal perturber
    abar  = 1.0;
  } else if (body[jBody].dSemi < body[kBody].dSemi) {
    alpha = body[jBody].dSemi / body[kBody].dSemi; // external perturber
    abar  = alpha;
  } else {
    fprintf(stderr,
            "ERROR: Body %s and body %s have the same semi-major axis.\n",
            body[kBody].cName, body[jBody].cName);
    exit(EXIT_INT);
  }

  n = KGAUSS * sqrt((body[0].dMass + body[jBody].dMass) / MSUN /
                    (body[jBody].dSemi / AUM * body[jBody].dSemi / AUM *
                     body[jBody].dSemi / AUM));

  b  = fndLaplaceCoeff(alpha, j, 1.5);
  AB = n / 4.0 * body[kBody].dMass / (body[0].dMass + body[jBody].dMass) *
       alpha * abar * b;
  return AB * 365.25; // returns in units of rad/year
}

/**
Calculates mutual hill radii between two bodies

@param body Struct containing all body information and variables
@param iBody Index of interior body
@param jBody Index of exterior body
@return Mutual hill radii in meters
*/
double fndMutualHillRad(BODY *body, int iBody, int jBody) {
  return 0.5 *
         pow((body[iBody].dMass + body[jBody].dMass) / body[0].dMass, 1. / 3) *
         (body[iBody].dSemi + body[jBody].dSemi);
}

/**
Post-Newtonian correction to AB matrix in LL2 solution

@param body Struct containing all body information and variables
@param jBody Index of perturbed body
@param kBody Index of perturbing body
@return Correction to component of AB matrix (added to A and B) in rad/year
*/
double fndGRCorrMatrix(BODY *body, int jBody, int kBody) {
  double n, GRC;

  n = KGAUSS * sqrt((body[0].dMass + body[jBody].dMass) / MSUN /
                    (body[jBody].dSemi / AUM * body[jBody].dSemi / AUM *
                     body[jBody].dSemi / AUM));
  if (jBody == kBody) {
    GRC = 3 * n * n * n * body[jBody].dSemi / AUM * body[jBody].dSemi / AUM /
          (LIGHTSPEED / AUM * DAYSEC * LIGHTSPEED / AUM * DAYSEC *
           (1.0 - body[jBody].dHecc * body[jBody].dHecc -
            body[jBody].dKecc * body[jBody].dKecc));
    return GRC * 365.25;

  } else {
    return 0.0;
  }
}

/**
Finds all eigenvalues of an upper Hess. matrix amat

@param amat Matrix to find eigenvalues of
@param origsize Size of original matrix
@param real The real components of the eigenvalues
@param imag The imaginary components of the eigenvalues (usually ~ 0)
*/
void HessEigen(double **amat, int origsize, double real[], double imag[]) {
  int size, m, smallsub, k, j, iterations, i, mmin;
  double radic, ulcorner, lrcorner, hhvector, v, u, exshift, s, r, q, p, anorm,
        cond, value;
  // s, r, q, and p are defined in numerical recipes eqns 11.6.23, 11.6.25

  anorm = fabs(amat[0][0]);
  for (i = 1; i <= origsize - 1; i++) {
    for (j = (i - 1); j <= origsize - 1; j++) {
      anorm += fabs(amat[i][j]);
    }
  }

  size    = origsize - 1;
  exshift = 0.0;
  while (size >= 0) {
    iterations = 0;
    do {
      for (smallsub = size; smallsub >= 1; smallsub--) {
        s = fabs(amat[smallsub - 1][smallsub - 1]) +
            fabs(amat[smallsub][smallsub]);
        if (s == 0.0) {
          s = anorm;
        }
        cond = fabs(amat[smallsub][smallsub - 1]) + s;
        if ((float)cond == (float)s) {
          break;
        }
      }
      lrcorner = amat[size][size];
      if (smallsub == size) {
        real[size]   = lrcorner + exshift;
        imag[size--] = 0.0;
      } else {
        ulcorner = amat[size - 1][size - 1];
        hhvector = amat[size][size - 1] * amat[size - 1][size];
        if (smallsub == (size - 1)) {
          p     = 0.5 * (ulcorner - lrcorner);
          q     = p * p + hhvector;
          radic = sqrt(fabs(q));
          lrcorner += exshift;
          if (q >= 0.0) {
            radic          = p + (double)fiSign(p) * radic;
            real[size - 1] = real[size] = lrcorner + radic;
            if (radic) {
              real[size] = lrcorner - hhvector / radic;
            }
            imag[size - 1] = imag[size] = 0.0;
          } else {
            real[size - 1] = real[size] = lrcorner + p;
            imag[size - 1]              = -(imag[size] = radic);
          }
          size -= 2;
        } else {
          if (iterations == 30) {
            fprintf(stderr, "Too many iterations in HessEigen routine\n");
            exit(EXIT_INPUT);
          }
          if (iterations == 10 || iterations == 20) {
            exshift += lrcorner;
            for (i = 0; i <= size; i++) {
              amat[i][i] -= lrcorner;
            }
            s = fabs(amat[size][size - 1]) + fabs(amat[size - 1][size - 2]);
            ulcorner = lrcorner = 0.75 * s;
            hhvector            = -0.4375 * s * s;
          }
          ++iterations;


          for (m = (size - 2); m >= smallsub; m--) {
            radic = amat[m][m];
            r     = lrcorner - radic;
            s     = ulcorner - radic;
            p     = (r * s - hhvector) / amat[m + 1][m] + amat[m][m + 1];
            q     = amat[m + 1][m + 1] - radic - r - s;
            r     = amat[m + 2][m + 1];
            s     = fabs(p) + fabs(q) + fabs(r);
            p /= s;
            q /= s;
            r /= s;
            if (m == smallsub) {
              break;
            }
            u = fabs(amat[m][m - 1]) * (fabs(q) + fabs(r));
            v = fabs(p) * (fabs(amat[m - 1][m - 1]) + fabs(radic) +
                           fabs(amat[m + 1][m + 1]));
            if ((float)(u + v) == (float)v) {
              break;
            }
          }
          for (i = m + 2; i <= size; i++) {
            amat[i][i - 2] = 0.0;
            if (i != (m + 2)) {
              amat[i][i - 3] = 0.0;
            }
          }
          for (k = m; k <= size - 1; k++) {
            if (k != m) {
              p = amat[k][k - 1];
              q = amat[k + 1][k - 1];
              r = 0.0;
              if (k != (size - 1)) {
                r = amat[k + 2][k - 1];
              }
              if ((lrcorner = fabs(p) + fabs(q) + fabs(r)) != 0.0) {
                p /= lrcorner;
                q /= lrcorner;
                r /= lrcorner;
              }
            } 
            value = sqrt(p * p + q * q + r * r);
            s     = (double)fiSign(p) * value;
            if (s != 0.0) {
              if (k == m) {
                if (smallsub != m) {
                  amat[k][k - 1] = -amat[k][k - 1];
                }
              } else {
                amat[k][k - 1] = -s * lrcorner;
              }
              p += s;
              lrcorner = p / s;
              ulcorner = q / s;
              radic    = r / s;
              q /= p;
              r /= p;
              for (j = k; j <= size; j++) {
                p = amat[k][j] + q * amat[k + 1][j];
                if (k != (size - 1)) {
                  p += r * amat[k + 2][j];
                  amat[k + 2][j] -= p * radic;
                }
                amat[k + 1][j] -= p * ulcorner;
                amat[k][j] -= p * lrcorner;
              }
              mmin = size < k + 3 ? size : k + 3;
              for (i = smallsub; i <= mmin; i++) {
                p = lrcorner * amat[i][k] + ulcorner * amat[i][k + 1];
                if (k != (size - 1)) {
                  p += radic * amat[i][k + 2];
                  amat[i][k + 2] -= p * r;
                }
                amat[i][k + 1] -= p * q;
                amat[i][k] -= p;
              }
            }
          }
        }
      }
    } while (smallsub < size - 1);
  }
}

/**
Swaps two rows in a matrix

@param matrix Matrix in question
@param size The number of rows/columns in matrix (square)
@param i One of the rows to be swapped
@param j The other row to be swapped
*/
void RowSwap(double **matrix, int size, int i, int j) {
  /* swap the ith and jth rows in matrix of size size*/
  int k;
  double dummy;

  for (k = 0; k < size; k++) {
    dummy        = matrix[i][k];
    matrix[i][k] = matrix[j][k];
    matrix[j][k] = dummy;
  }
}

/**
Swaps two columns in a matrix

@param matrix Matrix in question
@param size The number of rows/columns in matrix (square)
@param i One of the columns to be swapped
@param j The other column to be swapped
*/
void ColSwap(double **matrix, int size, int i, int j) {
  /* swap the ith and jth rows in matrix of size size*/
  int k;
  double dummy;

  for (k = 0; k < size; k++) {
    dummy        = matrix[k][i];
    matrix[k][i] = matrix[k][j];
    matrix[k][j] = dummy;
  }
}

/**
Reduces a matrix to Upper Hessenberg form

@param a Matrix in question
@param size The number of rows/columns in matrix a (square)
*/
void HessReduce(double **a, int size) {
  int r, rp, rmax, i, j;
  double max, n;

  for (r = 0; r < size; r++) {
    max = 0;
    rmax = r+1;
    for (rp = r + 1; rp < size; rp++) {
      if (fabs(a[rp][r]) > max) {
        max  = fabs(a[rp][r]);
        rmax = rp;
      }
    }
    if (max) {
      RowSwap(a, size, rmax, r + 1);
      ColSwap(a, size, rmax, r + 1);

      for (i = r + 2; i < size; i++) {
        n = a[i][r] / a[r + 1][r];

        for (j = 0; j < size; j++) {
          a[i][j] -= n * a[r + 1][j];
        }
        for (j = 0; j < size; j++) {
          a[j][r + 1] += n * a[j][i];
        }
      }
    }
  }
}

/**
Balances a matrix

@param a Matrix to be balanced
@param size The number of rows/columns in matrix a (square)
*/

void BalanceMatrix(double **a, int size) {
  int i, j, end = 0;
  double rownorm, colnorm, factor;

  while (end == 0) {
    for (i = 0; i < size; i++) {
      rownorm = 0.0;
      colnorm = 0.0;
      for (j = 0; j < size; j++) {
        rownorm += a[i][j] * a[i][j];
        colnorm += a[j][i] * a[j][i];
      }
      rownorm = pow(rownorm, 0.5);
      colnorm = pow(colnorm, 0.5);

      factor = sqrt(rownorm / colnorm);

      for (j = 0; j < size; j++) {
        a[i][j] /= factor;
        a[j][i] *= factor;
      }

      if ((factor * colnorm * factor * colnorm +
           rownorm / factor * rownorm / factor) >
          0.95 * (colnorm * colnorm + rownorm * rownorm)) {

        end = 1;
      }
    }
  }
}

/**
Decomposes matrix to LU form

@param amat Matrix to be LU'ed
@param copy Copy of matrix containing LU decomposition
@param scale Vector of scale factors used in decomposition
@param rowswap Indices of swapped rows
@param size Size of matrix (square)
*/
void LUDecomp(double **amat, double **copy, double *scale, int *rowswap,
              int size) {

  double sumk, scaletmp, dummy;
  int i, j, k, swapi;

  for (i = 0; i < size; i++) {
    scale[i] = 0.0;
    for (j = 0; j < size; j++) {
      if (fabs(amat[i][j]) > scale[i]) {
        scale[i] = fabs(amat[i][j]);
      }
    }
    if (scale[i] == 0.0) {
      fprintf(stderr, "Singular matrix in routine LUDecomp");
      exit(EXIT_INPUT);
    }
    for (j = 0; j < size; j++) {
      copy[i][j] = amat[i][j];
    }
    scale[i] = 1.0 / scale[i];
  }

  for (j = 0; j < size; j++) {
    scaletmp = 0.0;
    swapi    = j;

    for (i = 0; i < size; i++) {
      sumk = 0.0;
      if (i < j) {
        for (k = 0; k < i; k++) {
          sumk += copy[i][k] * copy[k][j];
        }
      } else {
        for (k = 0; k < j; k++) {
          sumk += copy[i][k] * copy[k][j];
        }
      }
      copy[i][j] -= sumk;

      if (i >= j) {
        if (fabs(scale[i] * copy[i][j]) >= scaletmp) {
          scaletmp = fabs(scale[i] * copy[i][j]);
          swapi    = i;
        }
      }
    }
    if (swapi != j) {
      RowSwap(copy, size, swapi, j);
      dummy        = scale[j];
      scale[j]     = scale[swapi];
      scale[swapi] = dummy;
    }

    if (copy[j][j] == 0) {
      copy[j][j] = TEENY;
    }
    for (i = j + 1; i < size; i++) {
      copy[i][j] /= copy[j][j];
    }
    rowswap[j] = swapi;
  }
}

/**
Solves system of equations involving an LU matrix

@param lumat LU matrix
@param soln Vector containing output solution
@param swap Indices of swapped rows
@param size Size of matrix (square)
*/
void LUSolve(double **lumat, double *soln, int *swap, int size) {
  int i, j;
  double dummy, sumj;

  for (i = 0; i < size; i++) {
    if (swap[i] != i) {
      dummy         = soln[i];
      soln[i]       = soln[swap[i]];
      soln[swap[i]] = dummy;
    }
  }

  for (i = 0; i < size; i++) {
    sumj = 0.0;
    for (j = 0; j < i; j++) {
      sumj += lumat[i][j] * soln[j];
    }
    soln[i] =
          soln[i] -
          sumj; // diagonals of L matrix are all = 1, so division is unnecessary
  }

  for (i = (size - 1); i >= 0; i--) {
    sumj = 0.0;
    for (j = i + 1; j < size; j++) {
      sumj += lumat[i][j] * soln[j];
    }
    soln[i] = (soln[i] - sumj) / lumat[i][i];
  }
}

/**
Find eccentricity eigenvectors for LL2 solution

@param system Struct containing system information
@param count Index of eigenvalue
@param pls Number of planets
*/
void FindEigenVecEcc(SYSTEM *system, int count, int pls) {

  int jj, i, iter = 5; // iter = # of iterations of inverse routine
  float d;             // parity for LU factorization
  double mag;          // normalization for eigenvector

  // Subtracts eigenvalue from diagonal elements
  for (jj = 0; jj <= (pls - 1); jj++) {
    system->daA[jj][jj] -= system->daEigenValEcc[0][count - 1];
    system->daAsoln[jj] = 1.0 / sqrt(pls);
    for (i = 0; i < pls; i++) {
      system->daAcopy[jj][i] = system->daA[jj][i];
    }
  }

  LUDecomp(system->daA, system->daAcopy, system->daScale, system->iaRowswap,
           pls);

  // Finds eigenvectors by inverse iteration, normalizing at each step
  for (i = 1; i <= iter; i++) {
    LUSolve(system->daAcopy, system->daAsoln, system->iaRowswap, pls);

    mag = 0.0;
    for (jj = 0; jj <= (pls - 1); jj++) {
      mag += system->daAsoln[jj] * system->daAsoln[jj];
    }

    for (jj = 0; jj <= (pls - 1); jj++) {
      system->daAsoln[jj] /= sqrt(mag);
    }
  }
}

/**
Find inclination eigenvectors for LL2 solution

@param system Struct containing system information
@param count Index of eigenvalue
@param pls Number of planets
*/
void FindEigenVecInc(SYSTEM *system, int count, int pls) {

  int jj, i, iter = 5; // iter = # of iterations of inverse routine
  float d;             // parity for LU factorization
  double mag;          // normalization for eigenvector

  // Subtracts eigenvalue from diagonal elements
  for (jj = 0; jj <= (pls - 1); jj++) {
    system->daB[jj][jj] -= system->daEigenValInc[0][count - 1];
    system->daBsoln[jj] = 1.0 / sqrt(pls);
    for (i = 0; i < pls; i++) {
      system->daAcopy[jj][i] = system->daB[jj][i];
    }
  }

  LUDecomp(system->daB, system->daAcopy, system->daScale, system->iaRowswap,
           pls);

  // Finds eigenvectors by inverse iteration, normalizing at each step
  for (i = 1; i <= iter; i++) {
    LUSolve(system->daAcopy, system->daBsoln, system->iaRowswap, pls);

    mag = 0.0;
    for (jj = 0; jj <= (pls - 1); jj++) {
      mag += system->daBsoln[jj] * system->daBsoln[jj];
    }

    for (jj = 0; jj <= (pls - 1); jj++) {
      system->daBsoln[jj] /= sqrt(mag);
    }
  }
}

/**
Find eigenvalues for LL2 solution

@param body Struct containing all body information and variables
@param evolve Struct containing evolve information
@param system Struct containing system information
*/
void SolveEigenVal(BODY *body, EVOLVE *evolve, SYSTEM *system) {
  /* This solves the eigenvalue problem and provides an explicit solution
       to the orbital evolution */
  double parity;
  int j, k, count, i, iBody;

  /*First pass through calculates matrices and eigenvalues. Each subsequent pass
     redefines the matrices because they are destroyed by eigenvalue routines,
     then calculates eigenvectors. */
  for (count = 0; count < (evolve->iNumBodies); count++) {
    /* Calculate the initial matrix */
    for (j = 0; j < (evolve->iNumBodies - 1); j++) {
      system->daA[j][j] = 0.0;
      system->daB[j][j] = 0.0;
      for (k = 0; k < (evolve->iNumBodies - 1); k++) {
        if (j != k) {
          system->daA[j][j] += fndABmatrix(body, 1, j + 1, k + 1);
          system->daA[j][k] = -fndABmatrix(body, 2, j + 1, k + 1);
          system->daB[j][j] += -fndABmatrix(body, 1, j + 1, k + 1);
          system->daB[j][k] = fndABmatrix(body, 1, j + 1, k + 1);
        }
      }
      if (body[j + 1].bGRCorr) {
        system->daA[j][j] += fndGRCorrMatrix(body, j + 1, j + 1);
      }
    }

    if (count == 0) {
      BalanceMatrix(system->daA, (evolve->iNumBodies - 1)); // balance matrix
      HessReduce(system->daA,
                 (evolve->iNumBodies - 1)); // reduce to upper Hess form

      BalanceMatrix(system->daB, (evolve->iNumBodies - 1)); // balance matrix
      HessReduce(system->daB,
                 (evolve->iNumBodies - 1)); // reduce to upper Hess form

      HessEigen(system->daA, (evolve->iNumBodies - 1), system->daEigenValEcc[0],
                system->daEigenValEcc[1]);
      HessEigen(system->daB, (evolve->iNumBodies - 1), system->daEigenValInc[0],
                system->daEigenValInc[1]);

    } else {
      FindEigenVecEcc(system, count, (evolve->iNumBodies - 1));
      FindEigenVecInc(system, count, (evolve->iNumBodies - 1));

      for (j = 0; j < (evolve->iNumBodies - 1); j++) {
        system->daEigenVecEcc[j][count - 1] = system->daAsoln[j];
        system->daEigenVecInc[j][count - 1] = system->daBsoln[j];
      }
    }
  }
}

/**
Scales eigenvectors to initial conditions

@param body Struct containing all body information and variables
@param evolve Struct containing evolve information
@param system Struct containing system information
*/
void ScaleEigenVec(BODY *body, EVOLVE *evolve, SYSTEM *system) {
  int i, j, count;
  float parity;

  for (i = 0; i < (evolve->iNumBodies - 1); i++) {
    system->dah0[i] = body[i + 1].dHecc;
    system->dak0[i] = body[i + 1].dKecc;
    system->dap0[i] = body[i + 1].dPinc;
    system->daq0[i] = body[i + 1].dQinc;

    for (j = 0; j < (evolve->iNumBodies - 1); j++) {
      system->daetmp[i][j] = system->daEigenVecEcc[i][j];
      system->daitmp[i][j] = system->daEigenVecInc[i][j];
    }
  }

  LUDecomp(system->daEigenVecEcc, system->daetmp, system->daScale,
           system->iaRowswap, (evolve->iNumBodies - 1));
  LUSolve(system->daetmp, system->dah0, system->iaRowswap,
          (evolve->iNumBodies - 1));
  LUSolve(system->daetmp, system->dak0, system->iaRowswap,
          (evolve->iNumBodies - 1));

  LUDecomp(system->daEigenVecInc, system->daitmp, system->daScale,
           system->iaRowswap, (evolve->iNumBodies - 1));

  LUSolve(system->daitmp, system->dap0, system->iaRowswap,
          (evolve->iNumBodies - 1));
  LUSolve(system->daitmp, system->daq0, system->iaRowswap,
          (evolve->iNumBodies - 1));


  for (i = 0; i < (evolve->iNumBodies - 1); i++) {
    system->daS[i] = sqrt(system->dah0[i] * system->dah0[i] +
                          system->dak0[i] * system->dak0[i]);
    system->daT[i] = sqrt(system->dap0[i] * system->dap0[i] +
                          system->daq0[i] * system->daq0[i]);

    for (j = 0; j < (evolve->iNumBodies - 1); j++) {
      system->daEigenVecEcc[j][i] *= system->daS[i];
      system->daEigenVecInc[j][i] *= system->daT[i];
    }

    system->daEigenPhase[0][i] = atan2(system->dah0[i], system->dak0[i]);
    system->daEigenPhase[1][i] = atan2(system->dap0[i], system->daq0[i]);
  }
}

/**
Recalculates Semi-major axis terms in case where RD4 solution is coupled to
eqtide

@param body Struct containing all body information and variables
@param evolve Struct containing evolve information
@param system Struct containing system information
@param iVerbose Verbosity level of output (currently not in use)
*/
void RecalcLaplace(BODY *body, EVOLVE *evolve, SYSTEM *system, int iVerbose) {
  double alpha1, dalpha;
  int j, iBody, jBody, done = 0;

  j = 0;
  for (iBody = 1; iBody < evolve->iNumBodies - 1; iBody++) {
    for (jBody = iBody + 1; jBody < evolve->iNumBodies; jBody++) {
      if (body[iBody].dSemi < body[jBody].dSemi) {
        alpha1 = body[iBody].dSemi / body[jBody].dSemi;
      } else if (body[iBody].dSemi > body[jBody].dSemi) {
        alpha1 = body[jBody].dSemi / body[iBody].dSemi;
      } else {
        fprintf(stderr,"ERROR: Semi-major axes cannot be identical in RecalcLaplace.");
        exit(EXIT_INPUT);
      }

      for (j = 0; j < 26; j++) {
        dalpha = fabs(alpha1 -
                      system->daAlpha0[0][system->iaLaplaceN[iBody][jBody]][j]);
        if (dalpha >
            fabs(system->dDfcrit /
                 system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][j])) {
          system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][j] =
                system->fnLaplaceF[j][0](alpha1, 0);

          system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][j] =
                system->fnLaplaceDeriv[j][0](alpha1, 0);

          system->daAlpha0[0][system->iaLaplaceN[iBody][jBody]][j] = alpha1;
          // if (iVerbose > VERBPROG)
          // //     printf("Laplace function %d recalculated for bodies (%d, %d)
          // at %f years\n",j+1,iBody,jBody,evolve->dTime/YEARSEC);
        }
      }
    }
  }
}

/**
Recalculates eigenvalues in case where LL2 solution is coupled to eqtide

@param body Struct containing all body information and variables
@param evolve Struct containing evolve information
@param system Struct containing system information
*/
void RecalcEigenVals(BODY *body, EVOLVE *evolve, SYSTEM *system) {
  int iBody, jBody, j, done = 0;
  double alpha1 = 0, dalpha = -1, dalphaTmp;

  for (iBody = 1; iBody < evolve->iNumBodies - 1; iBody++) {
    for (jBody = iBody + 1; jBody < evolve->iNumBodies; jBody++) {
      if (body[iBody].dSemi < body[jBody].dSemi) {
        alpha1 = body[iBody].dSemi / body[jBody].dSemi;
      } else if (body[iBody].dSemi > body[jBody].dSemi) {
        alpha1 = body[jBody].dSemi / body[iBody].dSemi;
      } else {
        fprintf(stderr,"ERROR: Semi-major axes cannot be identical in RecalcEigenVals.");
        exit(EXIT_INPUT);
      }
      for (j = 0; j < 2; j++) {
        dalphaTmp =
              fabs((alpha1 -
                    system->daAlpha0[0][system->iaLaplaceN[iBody][jBody]][0]) *
                   system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][j]);
        if (dalphaTmp > dalpha) {
          dalpha = dalphaTmp;
        }
      }
    }
  }

  if (dalpha > system->dDfcrit) {
    SolveEigenVal(body, evolve, system);
    ScaleEigenVec(body, evolve, system);
    for (iBody = 1; iBody < evolve->iNumBodies - 1; iBody++) {
      for (jBody = iBody + 1; jBody < evolve->iNumBodies; jBody++) {
        for (j = 0; j < 2; j++) {
          system->daLaplaceD[0][system->iaLaplaceN[iBody][jBody]][j] =
                fndDerivLaplaceCoeff(1, alpha1, j + 1, 1.5);
          system->daAlpha0[0][system->iaLaplaceN[iBody][jBody]][j] = alpha1;
        }
      }
    }
    //     printf("Eigenvalues recalculated at %f
    //     years\n",evolve->dTime/YEARSEC);
  }
}

/*
 * Invariable plane calculations
 */

/**
First x-term associated rotation into 3D Cartesian coordinates

@param body Struct containing all body information and variables
@param iBody Index of body in question
@return First x-term in rotation into 3D coordinates
*/
double fndXangle1(BODY *body, int iBody) {
  return cos(body[iBody].dLongA) *
               cos(body[iBody].dLongP - body[iBody].dLongA) -
         sin(body[iBody].dLongA) *
               sin(body[iBody].dLongP - body[iBody].dLongA) *
               (1.0 - 2. * body[iBody].dSinc * body[iBody].dSinc);
}

/**
Second x-term associated rotation into 3D Cartesian coordinates

@param body Struct containing all body information and variables
@param iBody Index of body in question
@return Second x-term in rotation into 3D coordinates
*/
double fndXangle2(BODY *body, int iBody) {
  return -cos(body[iBody].dLongA) *
               sin(body[iBody].dLongP - body[iBody].dLongA) -
         sin(body[iBody].dLongA) *
               cos(body[iBody].dLongP - body[iBody].dLongA) *
               (1.0 - 2. * body[iBody].dSinc * body[iBody].dSinc);
}

/**
First y-term associated rotation into 3D Cartesian coordinates

@param body Struct containing all body information and variables
@param iBody Index of body in question
@return First y-term in rotation into 3D coordinates
*/
double fndYangle1(BODY *body, int iBody) {
  return sin(body[iBody].dLongA) *
               cos(body[iBody].dLongP - body[iBody].dLongA) +
         cos(body[iBody].dLongA) *
               sin(body[iBody].dLongP - body[iBody].dLongA) *
               (1.0 - 2. * body[iBody].dSinc * body[iBody].dSinc);
}

/**
Second y-term associated rotation into 3D Cartesian coordinates

@param body Struct containing all body information and variables
@param iBody Index of body in question
@return Second y-term in rotation into 3D coordinates
*/
double fndYangle2(BODY *body, int iBody) {
  return -sin(body[iBody].dLongA) *
               sin(body[iBody].dLongP - body[iBody].dLongA) +
         cos(body[iBody].dLongA) *
               cos(body[iBody].dLongP - body[iBody].dLongA) *
               (1.0 - 2. * body[iBody].dSinc * body[iBody].dSinc);
}

/**
First z-term associated rotation into 3D Cartesian coordinates

@param body Struct containing all body information and variables
@param iBody Index of body in question
@return First z-term in rotation into 3D coordinates
*/
double fndZangle1(BODY *body, int iBody) {
  return sin(body[iBody].dLongP - body[iBody].dLongA) *
         (2. * body[iBody].dSinc *
          sqrt(1.0 - body[iBody].dSinc * body[iBody].dSinc));
}

/**
Second z-term associated rotation into 3D Cartesian coordinates

@param body Struct containing all body information and variables
@param iBody Index of body in question
@return Second z-term in rotation into 3D coordinates
*/
double fndZangle2(BODY *body, int iBody) {
  return cos(body[iBody].dLongP - body[iBody].dLongA) *
         (2. * body[iBody].dSinc *
          sqrt(1.0 - body[iBody].dSinc * body[iBody].dSinc));
}

/**
Calculates x-position in orbital plane

@param body Struct containing all body information and variables
@param iBody Index of body in question
@return Position of planet in x direction (au)
*/
double fndXinit(BODY *body, int iBody) {
  return body[iBody].dSemi / AUM * (cos(body[iBody].dEccA) - body[iBody].dEcc);
}

/**
Calculates y-position in orbital plane

@param body Struct containing all body information and variables
@param iBody Index of body in question
@return Position of planet in y direction (au/day)
*/
double fndYinit(BODY *body, int iBody) {
  return body[iBody].dSemi / AUM *
         sqrt(1.0 - body[iBody].dEcc * body[iBody].dEcc) *
         sin(body[iBody].dEccA);
}

/**
Calculates x-velocity in orbital plane

@param body Struct containing all body information and variables
@param iBody Index of body in question
@return Velocity of planet in x direction (au/day)
*/
double fndVxi(BODY *body, int iBody) {
  double x, y, mu, n;
  x  = fndXinit(body, iBody);
  y  = fndYinit(body, iBody);
  mu = KGAUSS * KGAUSS * (body[0].dMass + body[iBody].dMass) / MSUN;
  n  = sqrt(mu / (body[iBody].dSemi / AUM * body[iBody].dSemi / AUM *
                 body[iBody].dSemi / AUM));
  return -body[iBody].dSemi / AUM * body[iBody].dSemi / AUM * n *
         sin(body[iBody].dEccA) / sqrt(x * x + y * y);
}

/**
Calculates y-velocity in orbital plane

@param body Struct containing all body information and variables
@param iBody Index of body in question
@return Velocity of planet in y direction (au/day)
*/
double fndVyi(BODY *body, int iBody) {
  double x, y, mu, n, v;
  x  = fndXinit(body, iBody);
  y  = fndYinit(body, iBody);
  mu = KGAUSS * KGAUSS * (body[0].dMass + body[iBody].dMass) / MSUN;
  n  = sqrt(mu / (body[iBody].dSemi / AUM * body[iBody].dSemi / AUM *
                 body[iBody].dSemi / AUM));
  v  = body[iBody].dSemi / AUM * body[iBody].dSemi / AUM * n *
      sqrt((1.0 - body[iBody].dEcc * body[iBody].dEcc) / (x * x + y * y)) *
      cos(body[iBody].dEccA);
  return v;
}


/**
Calculates cross product of vectors

@param a First vector of cross prodect
@param b Second vector of cross product
@param c Resulting product containing cross product
*/
void cross(double *a, double *b, double *c) {
  c[0] = a[1] * b[2] - b[1] * a[2];
  c[1] = a[2] * b[0] - b[2] * a[0];
  c[2] = a[0] * b[1] - b[0] * a[1];
}


//
/*
 * Semi-major axis functions
 */


/**
Laplace coefficient used in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)
@param dIndexS Index s of Laplace Coefficients (usually s = 1/2, 3/2, or 5/2)

@return Laplace coefficient
*/
double fndLaplaceCoeff(double dAxRatio, int iIndexJ, double dIndexS) {
  /* Calculates Laplace coefficients via series form (M&D eqn 6.68) taking
   * dAxRatio = ratio of semi-major axes and j and s as arguments */
  double fac = 1.0, sum = 1.0, term = 1.0;
  int k, n = 1;
  if (iIndexJ == 1) {
    fac = dIndexS * dAxRatio;
  } else {
    for (k = 1; k <= iIndexJ; k++) {
      fac *= (dIndexS + k - 1.0) / (k)*dAxRatio;
    }
  }

  while (term >= 1.0e-15 * sum) {
    term = 1.0;
    for (k = 1; k <= n; k++) {
      term *= (dIndexS + k - 1.0) * (dIndexS + iIndexJ + k - 1.0) /
              (k * (iIndexJ + k)) * dAxRatio * dAxRatio;
    }
    sum = sum + term;

    n++;
  }
  return 2.0 * fac * sum;
}

/**
Derivative in d/d(alpha) of Laplace coefficient used in disturbing function

@param iNthDeriv Order of derivative to be taken
@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)
@param dIndexS Index s of Laplace Coefficients (usually s = 1/2, 3/2, or 5/2)

@return Laplace coefficient
*/
double fndDerivLaplaceCoeff(int iNthDeriv, double dAxRatio, int iIndexJ,
                            double dIndexS) {
  /* Calculates nth order derivative of Laplace coefficient using a recursive
   * scheme */
  double result;

  if (iNthDeriv == 1) {
    result = dIndexS *
             (fndLaplaceCoeff(dAxRatio, abs(iIndexJ - 1), dIndexS + 1.0) -
              2 * dAxRatio * fndLaplaceCoeff(dAxRatio, iIndexJ, dIndexS + 1.0) +
              fndLaplaceCoeff(dAxRatio, iIndexJ + 1, dIndexS + 1.0));
  } else if (iNthDeriv == 2) {
    result =
          dIndexS *
          (fndDerivLaplaceCoeff(1, dAxRatio, abs(iIndexJ - 1), dIndexS + 1.) -
           2 * dAxRatio *
                 fndDerivLaplaceCoeff(1, dAxRatio, iIndexJ, dIndexS + 1.) +
           fndDerivLaplaceCoeff(1, dAxRatio, iIndexJ + 1, dIndexS + 1.) -
           2 * fndLaplaceCoeff(dAxRatio, iIndexJ, dIndexS + 1.));
  } else {
    result = dIndexS * (fndDerivLaplaceCoeff(iNthDeriv - 1, dAxRatio,
                                             abs(iIndexJ - 1), dIndexS + 1.) -
                        2 * dAxRatio *
                              fndDerivLaplaceCoeff(iNthDeriv - 1, dAxRatio,
                                                   iIndexJ, dIndexS + 1.) +
                        fndDerivLaplaceCoeff(iNthDeriv - 1, dAxRatio,
                                             iIndexJ + 1, dIndexS + 1.) -
                        2 * (iNthDeriv - 1) *
                              fndDerivLaplaceCoeff(iNthDeriv - 2, dAxRatio,
                                                   iIndexJ, dIndexS + 1.));
  }
  return result;
}

/*--------- f1 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF1(double dAxRatio, int iIndexJ) {
  return 1. / 2 * fndLaplaceCoeff(A(iIndexJ));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF1Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 2 * fndDerivLaplaceCoeff(1, A(iIndexJ));
}

/*--------- f2 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF2(double dAxRatio, int iIndexJ) {
  return 1. / 8 *
         (-4. * iIndexJ * iIndexJ * fndLaplaceCoeff(A(iIndexJ)) +
          2. * dAxRatio * fndDerivLaplaceCoeff(1, A(iIndexJ)) +
          dAxRatio * dAxRatio * fndDerivLaplaceCoeff(2, A(iIndexJ)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF2Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 8 *
         ((2. - 4. * iIndexJ * iIndexJ) * fndDerivLaplaceCoeff(1, A(iIndexJ)) +
          4. * dAxRatio * fndDerivLaplaceCoeff(2, A(iIndexJ)) +
          dAxRatio * dAxRatio * fndDerivLaplaceCoeff(3, A(iIndexJ)));
}

/*--------- f3 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF3(double dAxRatio, int iIndexJ) {
  return -1. / 4 * dAxRatio *
         (fndLaplaceCoeff(B(abs(iIndexJ - 1))) +
          fndLaplaceCoeff(B(iIndexJ + 1)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF3Dalpha(double dAxRatio, int iIndexJ) {
  return -1. / 4 *
         ((fndLaplaceCoeff(B(abs(iIndexJ - 1))) +
           fndLaplaceCoeff(B(iIndexJ + 1))) +
          dAxRatio * (fndDerivLaplaceCoeff(1, B(abs(iIndexJ - 1))) +
                      fndDerivLaplaceCoeff(1, B(iIndexJ + 1))));
}

/*--------- f4 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF4(double dAxRatio, int iIndexJ) {
  return 1. / 128 *
         ((-9. * iIndexJ * iIndexJ +
           16. * iIndexJ * iIndexJ * iIndexJ * iIndexJ) *
                fndLaplaceCoeff(A(iIndexJ)) -
          8. * iIndexJ * iIndexJ * dAxRatio *
                fndDerivLaplaceCoeff(1, A(iIndexJ)) -
          8. * iIndexJ * iIndexJ * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, A(iIndexJ)) +
          4. * dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, A(iIndexJ)) +
          dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(4, A(iIndexJ)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF4Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 128 *
         ((-17. * iIndexJ * iIndexJ +
           16. * iIndexJ * iIndexJ * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, A(iIndexJ)) -
          24. * iIndexJ * iIndexJ * dAxRatio *
                fndDerivLaplaceCoeff(2, A(iIndexJ)) +
          (12. - 8. * iIndexJ * iIndexJ) * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, A(iIndexJ)) +
          (8. * dAxRatio * dAxRatio * dAxRatio) *
                fndDerivLaplaceCoeff(4, A(iIndexJ)) +
          dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(5, A(iIndexJ)));
}

/*--------- f5 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF5(double dAxRatio, int iIndexJ) {
  return 1. / 32 *
         (16. * iIndexJ * iIndexJ * iIndexJ * iIndexJ *
                fndLaplaceCoeff(A(iIndexJ)) +
          (4. - 16. * iIndexJ * iIndexJ) * dAxRatio *
                fndDerivLaplaceCoeff(1, A(iIndexJ)) +
          (14. - 8. * iIndexJ * iIndexJ) * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, A(iIndexJ)) +
          8. * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, A(iIndexJ)) +
          dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(4, A(iIndexJ)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF5Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 32 *
         ((4. - 16. * iIndexJ * iIndexJ +
           16. * iIndexJ * iIndexJ * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, A(iIndexJ)) +
          (32. - 32. * iIndexJ * iIndexJ) * dAxRatio *
                fndDerivLaplaceCoeff(2, A(iIndexJ)) +
          (38. - 8. * iIndexJ * iIndexJ) * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, A(iIndexJ)) +
          12. * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(4, A(iIndexJ)) +
          dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(5, A(iIndexJ)));
}

/*--------- f6 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF6(double dAxRatio, int iIndexJ) {
  return 1. / 128 *
         ((-17. * iIndexJ * iIndexJ +
           16. * iIndexJ * iIndexJ * iIndexJ * iIndexJ) *
                fndLaplaceCoeff(A(iIndexJ)) +
          (1. - iIndexJ * iIndexJ) * 24. * dAxRatio *
                fndDerivLaplaceCoeff(1, A(iIndexJ)) +
          (36. - 8. * iIndexJ * iIndexJ) * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, A(iIndexJ)) +
          12. * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, A(iIndexJ)) +
          dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(4, A(iIndexJ)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF6Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 128 *
         ((24. - 41. * iIndexJ * iIndexJ +
           16. * iIndexJ * iIndexJ * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, A(iIndexJ)) +
          (96. - 40. * iIndexJ * iIndexJ) * dAxRatio *
                fndDerivLaplaceCoeff(2, A(iIndexJ)) +
          (72. - 8. * iIndexJ * iIndexJ) * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, A(iIndexJ)) +
          (16. * dAxRatio * dAxRatio * dAxRatio) *
                fndDerivLaplaceCoeff(4, A(iIndexJ)) +
          dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(5, A(iIndexJ)));
}

/*--------- f7 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF7(double dAxRatio, int iIndexJ) {
  return 1. / 16 *
         ((-2. + 4. * iIndexJ * iIndexJ) * dAxRatio *
                (fndLaplaceCoeff(B(abs(iIndexJ - 1))) +
                 fndLaplaceCoeff(B(iIndexJ + 1))) -
          4. * dAxRatio * dAxRatio *
                (fndDerivLaplaceCoeff(1, B(abs(iIndexJ - 1))) +
                 fndDerivLaplaceCoeff(1, B(iIndexJ + 1))) -
          dAxRatio * dAxRatio * dAxRatio *
                (fndDerivLaplaceCoeff(2, B(abs(iIndexJ - 1))) +
                 fndDerivLaplaceCoeff(2, B(iIndexJ + 1))));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF7Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 16 *
         ((-2. + 4. * iIndexJ * iIndexJ) *
                (fndLaplaceCoeff(B(abs(iIndexJ - 1))) +
                 fndLaplaceCoeff(B(iIndexJ + 1))) -
          (10. - 4. * iIndexJ * iIndexJ) * dAxRatio *
                (fndDerivLaplaceCoeff(1, B(abs(iIndexJ - 1))) +
                 fndDerivLaplaceCoeff(1, B(iIndexJ + 1))) -
          7. * dAxRatio * dAxRatio *
                (fndDerivLaplaceCoeff(2, B(abs(iIndexJ - 1))) +
                 fndDerivLaplaceCoeff(2, B(iIndexJ + 1))) -
          dAxRatio * dAxRatio * dAxRatio *
                (fndDerivLaplaceCoeff(3, B(abs(iIndexJ - 1))) +
                 fndDerivLaplaceCoeff(3, B(iIndexJ + 1))));
}

/*--------- f8 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF8(double dAxRatio, int iIndexJ) {
  return 3. / 16 * dAxRatio * dAxRatio *
         (fndLaplaceCoeff(C(abs(iIndexJ - 2))) +
          4. * fndLaplaceCoeff(C(iIndexJ)) + fndLaplaceCoeff(C(iIndexJ + 2)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF8Dalpha(double dAxRatio, int iIndexJ) {
  return 3. / 16 * dAxRatio *
         (2 * (fndLaplaceCoeff(C(abs(iIndexJ - 2))) +
               4. * fndLaplaceCoeff(C(iIndexJ)) +
               fndLaplaceCoeff(C(iIndexJ + 2))) +
          dAxRatio * (fndDerivLaplaceCoeff(1, C(abs(iIndexJ - 2))) +
                      4. * fndDerivLaplaceCoeff(1, C(iIndexJ)) +
                      fndDerivLaplaceCoeff(1, C(iIndexJ + 2))));
}

/*--------- f9 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF9(double dAxRatio, int iIndexJ) {
  return 1. / 4 * dAxRatio *
               (fndLaplaceCoeff(B(abs(iIndexJ - 1))) +
                fndLaplaceCoeff(B(iIndexJ + 1))) +
         3. / 8 * dAxRatio * dAxRatio *
               (fndLaplaceCoeff(C(abs(iIndexJ - 2))) +
                10. * fndLaplaceCoeff(C(iIndexJ)) +
                fndLaplaceCoeff(C(iIndexJ + 2)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF9Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 4 *
               ((fndLaplaceCoeff(B(abs(iIndexJ - 1))) +
                 fndLaplaceCoeff(B(iIndexJ + 1))) +
                dAxRatio * (fndDerivLaplaceCoeff(1, B(abs(iIndexJ - 1))) +
                            fndDerivLaplaceCoeff(1, B(iIndexJ + 1)))) +
         3. / 8 * dAxRatio *
               (2 * (fndLaplaceCoeff(C(abs(iIndexJ - 2))) +
                     10. * fndLaplaceCoeff(C(iIndexJ)) +
                     fndLaplaceCoeff(C(iIndexJ + 2))) +
                dAxRatio * (fndDerivLaplaceCoeff(1, C(abs(iIndexJ - 2))) +
                            10. * fndDerivLaplaceCoeff(1, C(iIndexJ)) +
                            fndDerivLaplaceCoeff(1, C(iIndexJ + 2))));
}

/*--------- f10 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF10(double dAxRatio, int iIndexJ) {
  return 1. / 4 *
         ((2. + 6. * iIndexJ + 4. * iIndexJ * iIndexJ) *
                fndLaplaceCoeff(A(iIndexJ + 1)) -
          2. * dAxRatio * fndDerivLaplaceCoeff(1, A(iIndexJ + 1)) -
          dAxRatio * dAxRatio * fndDerivLaplaceCoeff(2, A(iIndexJ + 1)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF10Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 4 *
         ((6. * iIndexJ + 4. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, A(iIndexJ + 1)) -
          4. * dAxRatio * fndDerivLaplaceCoeff(2, A(iIndexJ + 1)) -
          dAxRatio * dAxRatio * fndDerivLaplaceCoeff(3, A(iIndexJ + 1)));
}


/*--------- f11 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF11(double dAxRatio, int iIndexJ) {
  return 1. / 32 *
         ((-6. * iIndexJ - 26. * iIndexJ * iIndexJ -
           36. * iIndexJ * iIndexJ * iIndexJ -
           16 * iIndexJ * iIndexJ * iIndexJ * iIndexJ) *
                fndLaplaceCoeff(A(iIndexJ + 1)) +
          dAxRatio * (6 * iIndexJ + 12 * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, A(iIndexJ + 1)) +
          dAxRatio * dAxRatio * (-4. + 7 * iIndexJ + 8 * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(2, A(iIndexJ + 1)) -
          6. * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, A(iIndexJ + 1)) -
          dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(4, A(iIndexJ + 1)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF11Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 32 *
         ((-14. * iIndexJ * iIndexJ - 36. * iIndexJ * iIndexJ * iIndexJ -
           16 * iIndexJ * iIndexJ * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, A(iIndexJ + 1)) +
          dAxRatio * (-8. + 20. * iIndexJ + 28. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(2, A(iIndexJ + 1)) +
          dAxRatio * dAxRatio * (-22. + 7. * iIndexJ + 8. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(3, A(iIndexJ + 1)) -
          10. * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(4, A(iIndexJ + 1)) -
          dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(5, A(iIndexJ + 1)));
}

/*--------- f12 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF12(double dAxRatio, int iIndexJ) {
  return 1. / 32 *
         ((4. + 2. * iIndexJ - 22. * iIndexJ * iIndexJ -
           36. * iIndexJ * iIndexJ * iIndexJ -
           16. * iIndexJ * iIndexJ * iIndexJ * iIndexJ) *
                fndLaplaceCoeff(A(iIndexJ + 1)) +
          dAxRatio * (-4. + 22. * iIndexJ + 20. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, A(iIndexJ + 1)) +
          dAxRatio * dAxRatio * (-22. + 7. * iIndexJ + 8. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(2, A(iIndexJ + 1)) -
          10. * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, A(iIndexJ + 1)) -
          dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(4, A(iIndexJ + 1)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF12Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 32 *
         ((24. * iIndexJ - 2. * iIndexJ * iIndexJ -
           36. * iIndexJ * iIndexJ * iIndexJ -
           16. * iIndexJ * iIndexJ * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, A(iIndexJ + 1)) +
          dAxRatio * (-48. + 36. * iIndexJ + 36. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(2, A(iIndexJ + 1)) +
          dAxRatio * dAxRatio * (-52. + 7. * iIndexJ + 8. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(3, A(iIndexJ + 1)) -
          14. * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(4, A(iIndexJ + 1)) -
          dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(5, A(iIndexJ + 1)));
}

/*--------- f13 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF13(double dAxRatio, int iIndexJ) {
  return 1. / 8 *
         ((-6. * iIndexJ - 4. * iIndexJ * iIndexJ) * dAxRatio *
                (fndLaplaceCoeff(B(iIndexJ)) +
                 fndLaplaceCoeff(B(iIndexJ + 2))) +
          4. * dAxRatio * dAxRatio *
                (fndDerivLaplaceCoeff(1, B(iIndexJ)) +
                 fndDerivLaplaceCoeff(1, B(iIndexJ + 2))) +
          dAxRatio * dAxRatio * dAxRatio *
                (fndDerivLaplaceCoeff(2, B(iIndexJ)) +
                 fndDerivLaplaceCoeff(2, B(iIndexJ + 2))));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF13Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 8 *
         ((-6. * iIndexJ - 4. * iIndexJ * iIndexJ) *
                (fndLaplaceCoeff(B(iIndexJ)) +
                 fndLaplaceCoeff(B(iIndexJ + 2))) +
          (8. - 6. * iIndexJ - 4. * iIndexJ * iIndexJ) * dAxRatio *
                (fndDerivLaplaceCoeff(1, B(iIndexJ)) +
                 fndDerivLaplaceCoeff(1, B(iIndexJ + 2))) +
          7. * dAxRatio * dAxRatio *
                (fndDerivLaplaceCoeff(2, B(iIndexJ)) +
                 fndDerivLaplaceCoeff(2, B(iIndexJ + 2))) +
          dAxRatio * dAxRatio * dAxRatio *
                (fndDerivLaplaceCoeff(3, B(iIndexJ)) +
                 fndDerivLaplaceCoeff(3, B(iIndexJ + 2))));
}

/*--------- f14 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF14(double dAxRatio, int iIndexJ) {
  return dAxRatio * fndLaplaceCoeff(B(iIndexJ + 1));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF14Dalpha(double dAxRatio, int iIndexJ) {
  return fndLaplaceCoeff(B(iIndexJ + 1)) +
         dAxRatio * fndDerivLaplaceCoeff(1, B(iIndexJ + 1));
}

/*--------- f15 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF15(double dAxRatio, int iIndexJ) {
  return 1. / 4 *
         ((2. - 4. * iIndexJ * iIndexJ) * dAxRatio *
                fndLaplaceCoeff(B(iIndexJ + 1)) +
          4. * dAxRatio * dAxRatio * fndDerivLaplaceCoeff(1, B(iIndexJ + 1)) +
          dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, B(iIndexJ + 1)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF15Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 4 *
         ((2. - 4. * iIndexJ * iIndexJ) * fndLaplaceCoeff(B(iIndexJ + 1)) +
          (10. - 4. * iIndexJ * iIndexJ) * dAxRatio *
                fndDerivLaplaceCoeff(1, B(iIndexJ + 1)) +
          7. * dAxRatio * dAxRatio * fndDerivLaplaceCoeff(2, B(iIndexJ + 1)) +
          dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, B(iIndexJ + 1)));
}

/*--------- f16 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF16(double dAxRatio, int iIndexJ) {
  return -1. / 2 * dAxRatio * fndLaplaceCoeff(B(iIndexJ + 1)) -
         3. * dAxRatio * dAxRatio * fndLaplaceCoeff(C(iIndexJ)) -
         3. / 2 * dAxRatio * dAxRatio * fndLaplaceCoeff(C(iIndexJ + 2));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF16Dalpha(double dAxRatio, int iIndexJ) {
  return -1. / 2 *
               (fndLaplaceCoeff(B(iIndexJ + 1)) +
                dAxRatio * fndDerivLaplaceCoeff(1, B(iIndexJ + 1))) -
         3. * dAxRatio *
               (2. * (fndLaplaceCoeff(C(iIndexJ)) +
                      1. / 2 * fndLaplaceCoeff(C(iIndexJ + 2))) +
                dAxRatio * (fndDerivLaplaceCoeff(1, C(iIndexJ)) +
                            1. / 2 * fndDerivLaplaceCoeff(1, C(iIndexJ + 2))));
}

/*--------- f17 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF17(double dAxRatio, int iIndexJ) {
  return 1. / 64 *
         ((12. + 64. * iIndexJ + 109. * iIndexJ * iIndexJ +
           72. * iIndexJ * iIndexJ * iIndexJ +
           16. * iIndexJ * iIndexJ * iIndexJ * iIndexJ) *
                fndLaplaceCoeff(A(iIndexJ + 2)) -
          dAxRatio * (12. + 28. * iIndexJ + 16. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, A(iIndexJ + 2)) +
          dAxRatio * dAxRatio * (6. - 14. * iIndexJ - 8. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(2, A(iIndexJ + 2)) +
          8. * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, A(iIndexJ + 2)) +
          dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(4, A(iIndexJ + 2)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF17Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 64 *
         ((36. * iIndexJ + 93. * iIndexJ * iIndexJ +
           72. * iIndexJ * iIndexJ * iIndexJ * iIndexJ +
           16. * iIndexJ * iIndexJ * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, A(iIndexJ + 2)) -
          dAxRatio * (56. * iIndexJ + 32. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(2, A(iIndexJ + 2)) +
          dAxRatio * dAxRatio * (30. - 14. * iIndexJ - 8. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(3, A(iIndexJ + 2)) +
          12. * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(4, A(iIndexJ + 2)) +
          dAxRatio * dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(5, A(iIndexJ + 2)));
}

/*--------- f18 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF18(double dAxRatio, int iIndexJ) {
  return 1. / 16 *
         ((12. - 15. * iIndexJ + 4. * iIndexJ * iIndexJ) * dAxRatio *
                fndLaplaceCoeff(B(abs(iIndexJ - 1))) +
          dAxRatio * dAxRatio * (8. - 4. * iIndexJ) *
                fndDerivLaplaceCoeff(1, B(abs(iIndexJ - 1))) +
          dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, B(abs(iIndexJ - 1))));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF18Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 16 *
         ((12. - 15. * iIndexJ + 4. * iIndexJ * iIndexJ) *
                fndLaplaceCoeff(B(abs(iIndexJ - 1))) +
          dAxRatio * (28. - 23. * iIndexJ + 4. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, B(abs(iIndexJ - 1))) +
          (11. - 4. * iIndexJ) * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, B(abs(iIndexJ - 1))) +
          dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, B(abs(iIndexJ - 1))));
}

/*--------- f19 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF19(double dAxRatio, int iIndexJ) {
  return 1. / 8 *
         ((6. - 4. * iIndexJ) * iIndexJ * dAxRatio *
                fndLaplaceCoeff(B(iIndexJ)) +
          dAxRatio * dAxRatio * (-4. + 4. * iIndexJ) *
                fndDerivLaplaceCoeff(1, B(iIndexJ)) -
          dAxRatio * dAxRatio * dAxRatio * fndDerivLaplaceCoeff(2, B(iIndexJ)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF19Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 8 *
         ((6. - 4. * iIndexJ) * iIndexJ * fndLaplaceCoeff(B(iIndexJ)) +
          dAxRatio * (-8. + 14. * iIndexJ - 4. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, B(iIndexJ)) +
          (-7. + 4. * iIndexJ) * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, B(iIndexJ)) -
          dAxRatio * dAxRatio * dAxRatio * fndDerivLaplaceCoeff(3, B(iIndexJ)));
}

/*--------- f20 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF20(double dAxRatio, int iIndexJ) {
  return 1. / 16 *
         ((3. + 4. * iIndexJ) * iIndexJ * dAxRatio *
                fndLaplaceCoeff(B(iIndexJ + 1)) -
          4. * iIndexJ * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(1, B(iIndexJ + 1)) +
          dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, B(iIndexJ + 1)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF20Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 16 *
         ((3. + 4. * iIndexJ) * iIndexJ * fndLaplaceCoeff(B(iIndexJ + 1)) +
          (-5. * iIndexJ + 4. * iIndexJ * iIndexJ) * dAxRatio *
                fndDerivLaplaceCoeff(1, B(iIndexJ + 1)) +
          (3. - 4. * iIndexJ) * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, B(iIndexJ + 1)) +
          dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, B(iIndexJ + 1)));
}

/*--------- f21 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF21(double dAxRatio, int iIndexJ) {
  return 1. / 8 *
         ((-12. + 15. * iIndexJ - 4. * iIndexJ * iIndexJ) * dAxRatio *
                fndLaplaceCoeff(B(abs(iIndexJ - 1))) +
          dAxRatio * dAxRatio * (-8. + 4. * iIndexJ) *
                fndDerivLaplaceCoeff(1, B(abs(iIndexJ - 1))) -
          dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, B(abs(iIndexJ - 1))));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF21Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 8 *
         ((-12. + 15. * iIndexJ - 4. * iIndexJ * iIndexJ) *
                fndLaplaceCoeff(B(abs(iIndexJ - 1))) +
          dAxRatio * (-28. + 23. * iIndexJ - 4. * iIndexJ * iIndexJ) *
                fndDerivLaplaceCoeff(1, B(abs(iIndexJ - 1))) +
          (-11. + 4. * iIndexJ) * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, B(abs(iIndexJ - 1))) -
          dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, B(abs(iIndexJ - 1))));
}

/*--------- f22 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF22(double dAxRatio, int iIndexJ) {
  return 1. / 4 *
         (dAxRatio * iIndexJ * (6. + 4. * iIndexJ) *
                fndLaplaceCoeff(B(iIndexJ)) -
          4. * dAxRatio * dAxRatio * fndDerivLaplaceCoeff(1, B(iIndexJ)) -
          dAxRatio * dAxRatio * dAxRatio * fndDerivLaplaceCoeff(2, B(iIndexJ)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF22Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 4 *
         (iIndexJ * (6. + 4. * iIndexJ) * fndLaplaceCoeff(B(iIndexJ)) +
          (-8. + 6. * iIndexJ + 4. * iIndexJ * iIndexJ) * dAxRatio *
                fndDerivLaplaceCoeff(1, B(iIndexJ)) -
          7. * dAxRatio * dAxRatio * fndDerivLaplaceCoeff(2, B(iIndexJ)) -
          dAxRatio * dAxRatio * dAxRatio * fndDerivLaplaceCoeff(3, B(iIndexJ)));
}

/*--------- f23 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF23(double dAxRatio, int iIndexJ) {
  return 1. / 4 *
         (dAxRatio * iIndexJ * (6. + 4. * iIndexJ) *
                fndLaplaceCoeff(B(iIndexJ + 2)) -
          4. * dAxRatio * dAxRatio * fndDerivLaplaceCoeff(1, B(iIndexJ + 2)) -
          dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, B(iIndexJ + 2)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF23Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 4 *
         (iIndexJ * (6. + 4. * iIndexJ) * fndLaplaceCoeff(B(iIndexJ + 2)) +
          (-8. + 6. * iIndexJ + 4. * iIndexJ * iIndexJ) * dAxRatio *
                fndDerivLaplaceCoeff(1, B(iIndexJ + 2)) -
          7. * dAxRatio * dAxRatio * fndDerivLaplaceCoeff(2, B(iIndexJ + 2)) -
          dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, B(iIndexJ + 2)));
}

/*--------- f24 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF24(double dAxRatio, int iIndexJ) {
  return 1. / 4 *
         ((-6. + 4. * iIndexJ) * iIndexJ * dAxRatio *
                fndLaplaceCoeff(B(iIndexJ)) +
          4. * dAxRatio * dAxRatio * (1. - iIndexJ) *
                fndDerivLaplaceCoeff(1, B(iIndexJ)) +
          dAxRatio * dAxRatio * dAxRatio * fndDerivLaplaceCoeff(2, B(iIndexJ)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF24Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 4 *
         ((-6. + 4. * iIndexJ) * iIndexJ * fndLaplaceCoeff(B(iIndexJ)) +
          (8. - 14. * iIndexJ + 4. * iIndexJ * iIndexJ) * dAxRatio *
                fndDerivLaplaceCoeff(1, B(iIndexJ)) +
          (7. - 4. * iIndexJ) * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, B(iIndexJ)) +
          dAxRatio * dAxRatio * dAxRatio * fndDerivLaplaceCoeff(3, B(iIndexJ)));
}

/*--------- f25 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF25(double dAxRatio, int iIndexJ) {
  return 1. / 8 *
         ((-3. - 4. * iIndexJ) * iIndexJ * dAxRatio *
                fndLaplaceCoeff(B(iIndexJ + 1)) +
          4. * iIndexJ * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(1, B(iIndexJ + 1)) -
          dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, B(iIndexJ + 1)));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF25Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 8 *
         ((-3. - 4. * iIndexJ) * iIndexJ * fndLaplaceCoeff(B(iIndexJ + 1)) +
          (5. * iIndexJ - 4. * iIndexJ * iIndexJ) * dAxRatio *
                fndDerivLaplaceCoeff(1, B(iIndexJ + 1)) +
          (-3. + 4. * iIndexJ) * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(2, B(iIndexJ + 1)) -
          dAxRatio * dAxRatio * dAxRatio *
                fndDerivLaplaceCoeff(3, B(iIndexJ + 1)));
}

/*--------- f26 ----------------------*/

/**
Semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Semi-major axis term
*/
double fndSemiMajAxF26(double dAxRatio, int iIndexJ) {
  return 1. / 2 * dAxRatio * fndLaplaceCoeff(B(iIndexJ + 1)) +
         3. / 4 * dAxRatio * dAxRatio * fndLaplaceCoeff(C(iIndexJ)) +
         3. / 2 * dAxRatio * dAxRatio * fndLaplaceCoeff(C(iIndexJ + 2));
}

/**
Derivative in d/d(alpha) of semi-major axis term in disturbing function

@param dAxRatio Ratio of inner planet's semi to outer planet's (must be < 1)
@param iIndexJ Index j of Laplace Coefficients (j = 0 for secular model)

@return Derivative of semi-major axis term
*/
double fndDSemiF26Dalpha(double dAxRatio, int iIndexJ) {
  return 1. / 2 *
               (fndLaplaceCoeff(B(iIndexJ + 1)) +
                dAxRatio * fndDerivLaplaceCoeff(1, B(iIndexJ + 1))) +
         3. / 4 * dAxRatio *
               (2. * (fndLaplaceCoeff(C(iIndexJ)) +
                      2. * fndLaplaceCoeff(C(iIndexJ + 2))) +
                dAxRatio * (fndDerivLaplaceCoeff(1, C(iIndexJ)) +
                            2. * fndDerivLaplaceCoeff(1, C(iIndexJ + 2))));
}


//----------------Disturbing function h k p
// q----------------------------------------------
//--------dR/dh-----------(inner
// body)------------------------------------------------------

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dh term for interior body
*/
double fndDdistDhDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dHecc *
         (system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][1] +
          2 *
                (body[iBody].dHecc * body[iBody].dHecc +
                 body[iBody].dKecc * body[iBody].dKecc) *
                system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][3] +
          (body[jBody].dHecc * body[jBody].dHecc +
           body[jBody].dKecc * body[jBody].dKecc) *
                system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][4] +
          (body[iBody].dPinc * body[iBody].dPinc +
           body[iBody].dQinc * body[iBody].dQinc +
           body[jBody].dPinc * body[jBody].dPinc +
           body[jBody].dQinc * body[jBody].dQinc) *
                system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][6]);
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dh term for interior body
*/
double fndDdistDhDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dHecc *
               (system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][9] +
                (body[jBody].dHecc * body[jBody].dHecc +
                 body[jBody].dKecc * body[jBody].dKecc) *
                      system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]]
                                        [11] +
                (body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc +
                 body[jBody].dPinc * body[jBody].dPinc +
                 body[jBody].dQinc * body[jBody].dQinc) *
                      system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]]
                                        [12]) +
         (2 * body[iBody].dHecc * body[iBody].dKecc * body[jBody].dKecc +
          3 * body[iBody].dHecc * body[iBody].dHecc * body[jBody].dHecc +
          body[jBody].dHecc * body[iBody].dKecc * body[iBody].dKecc) *
               system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][10];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dh term for interior body
*/
double fndDdistDhDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dHecc *
         (body[iBody].dPinc * body[jBody].dPinc +
          body[iBody].dQinc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][14];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dh term for interior body
*/
double fndDdistDhDir04(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2 * body[iBody].dHecc *
                (body[jBody].dHecc * body[jBody].dHecc -
                 body[jBody].dKecc * body[jBody].dKecc) +
          4 * body[jBody].dHecc * body[iBody].dKecc * body[jBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][16];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dh term for interior body
*/
double fndDdistDhDir05(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2 * body[iBody].dHecc *
                (body[iBody].dPinc * body[iBody].dPinc -
                 body[iBody].dQinc * body[iBody].dQinc) +
          4 * body[iBody].dPinc * body[iBody].dQinc * body[iBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][17];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dh term for interior body
*/
double fndDdistDhDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dHecc * (body[iBody].dPinc * body[iBody].dPinc -
                               body[iBody].dQinc * body[iBody].dQinc) +
          2 * body[jBody].dKecc * body[iBody].dPinc * body[iBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][18];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dh term for interior body
*/
double fndDdistDhDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dHecc * (body[iBody].dPinc * body[jBody].dPinc -
                               body[iBody].dQinc * body[jBody].dQinc) +
          body[iBody].dKecc * (body[jBody].dPinc * body[iBody].dQinc +
                               body[jBody].dQinc * body[iBody].dPinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][20];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dh term for interior body
*/
double fndDdistDhDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dHecc * (body[iBody].dQinc * body[jBody].dQinc +
                               body[iBody].dPinc * body[jBody].dPinc) +
          body[jBody].dKecc * (body[iBody].dPinc * body[jBody].dQinc -
                               body[iBody].dQinc * body[jBody].dPinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][21];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dh term for interior body
*/
double fndDdistDhDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dHecc * (body[iBody].dQinc * body[jBody].dQinc +
                               body[iBody].dPinc * body[jBody].dPinc) +
          body[jBody].dKecc * (body[iBody].dQinc * body[jBody].dPinc -
                               body[iBody].dPinc * body[jBody].dQinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][22];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dh term for interior body
*/
double fndDdistDhDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dHecc * (body[iBody].dPinc * body[jBody].dPinc -
                               body[iBody].dQinc * body[jBody].dQinc) +
          body[jBody].dKecc * (body[iBody].dQinc * body[jBody].dPinc +
                               body[iBody].dPinc * body[jBody].dQinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][23];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dh term for interior body
*/
double fndDdistDhDir013(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2 * body[iBody].dHecc *
                (body[jBody].dPinc * body[jBody].dPinc -
                 body[jBody].dQinc * body[jBody].dQinc) +
          4 * body[jBody].dPinc * body[jBody].dQinc * body[iBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][17];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dh term for interior body
*/
double fndDdistDhDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dHecc * (body[jBody].dPinc * body[jBody].dPinc -
                               body[jBody].dQinc * body[jBody].dQinc) +
          2 * body[jBody].dKecc * body[jBody].dPinc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][18];
}

/**
Sums over secular dR/dh terms of disturbing function for interior body

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction

@return Total dR/dh for interior body
*/
double fndDdisturbDHecc(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac    = KGAUSS * KGAUSS * body[iaBody[1]].dMass / MSUN;
  dSemiPrm = body[iaBody[1]].dSemi / AUM;
  y        = (fndDdistDhDir01(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhDir02(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhDir03(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhDir04(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhDir05(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhDir06(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhDir08(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhDir09(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhDir010(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhDir011(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhDir013(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhDir014(body, system, iaBody[0], iaBody[1])) *
      dMfac / dSemiPrm;
  return y;
}

//---------dR/dh'------------------------------------------------------------------

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dh term for exterior body
*/
double fndDdistDhPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dHecc *
         (system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][1] +
          (body[jBody].dHecc * body[jBody].dHecc +
           body[jBody].dKecc * body[jBody].dKecc) *
                system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][4] +
          2 *
                (body[iBody].dHecc * body[iBody].dHecc +
                 body[iBody].dKecc * body[iBody].dKecc) *
                system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][5] +
          (body[jBody].dPinc * body[jBody].dPinc +
           body[jBody].dQinc * body[jBody].dQinc +
           body[iBody].dPinc * body[iBody].dPinc +
           body[iBody].dQinc * body[iBody].dQinc) *
                system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][6]);
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dh term for exterior body
*/
double fndDdistDhPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dHecc *
               (system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][9] +
                (body[jBody].dHecc * body[jBody].dHecc +
                 body[jBody].dKecc * body[jBody].dKecc) *
                      system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]]
                                        [10] +
                (body[jBody].dPinc * body[jBody].dPinc +
                 body[jBody].dQinc * body[jBody].dQinc +
                 body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc) *
                      system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]]
                                        [12]) +
         (2 * body[iBody].dHecc * body[jBody].dKecc * body[iBody].dKecc +
          3 * body[iBody].dHecc * body[iBody].dHecc * body[jBody].dHecc +
          body[jBody].dHecc * body[iBody].dKecc * body[iBody].dKecc) *
               system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][11];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dh term for exterior body
*/
double fndDdistDhPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dHecc *
         (body[iBody].dPinc * body[jBody].dPinc +
          body[iBody].dQinc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][14];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dh term for exterior body
*/
double fndDdistDhPrmDir04(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2 * body[iBody].dHecc *
                (body[jBody].dHecc * body[jBody].dHecc -
                 body[jBody].dKecc * body[jBody].dKecc) +
          4 * body[jBody].dHecc * body[jBody].dKecc * body[iBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][16];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dh term for exterior body
*/
double fndDdistDhPrmDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dHecc * (body[jBody].dPinc * body[jBody].dPinc -
                               body[jBody].dQinc * body[jBody].dQinc) +
          2 * body[jBody].dKecc * body[jBody].dPinc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][18];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dh term for exterior body
*/
double fndDdistDhPrmDir07(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2 * body[iBody].dHecc *
                (body[jBody].dPinc * body[jBody].dPinc -
                 body[jBody].dQinc * body[jBody].dQinc) +
          4 * body[jBody].dPinc * body[jBody].dQinc * body[iBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][19];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dh term for exterior body
*/
double fndDdistDhPrmDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dHecc * (body[iBody].dQinc * body[jBody].dQinc +
                               body[iBody].dPinc * body[jBody].dPinc) +
          body[jBody].dKecc * (body[iBody].dPinc * body[jBody].dQinc -
                               body[iBody].dQinc * body[jBody].dPinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][21];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dh term for exterior body
*/
double fndDdistDhPrmDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dHecc * (body[iBody].dQinc * body[jBody].dQinc +
                               body[iBody].dPinc * body[jBody].dPinc) +
          body[jBody].dKecc * (body[iBody].dQinc * body[jBody].dPinc -
                               body[iBody].dPinc * body[jBody].dQinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][22];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dh term for exterior body
*/
double fndDdistDhPrmDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dHecc * (-body[iBody].dQinc * body[jBody].dQinc +
                               body[iBody].dPinc * body[jBody].dPinc) +
          body[jBody].dKecc * (body[iBody].dQinc * body[jBody].dPinc +
                               body[iBody].dPinc * body[jBody].dQinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][23];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dh term for exterior body
*/
double fndDdistDhPrmDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dHecc * (body[iBody].dPinc * body[jBody].dPinc -
                               body[iBody].dQinc * body[jBody].dQinc) +
          body[iBody].dKecc * (body[iBody].dPinc * body[jBody].dQinc +
                               body[iBody].dQinc * body[jBody].dPinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][24];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dh term for exterior body
*/
double fndDdistDhPrmDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dHecc * (body[iBody].dPinc * body[iBody].dPinc -
                               body[iBody].dQinc * body[iBody].dQinc) +
          2 * body[jBody].dKecc * body[iBody].dPinc * body[iBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][18];
}

/**
Derivative in d/dh of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dh term for exterior body
*/
double fndDdistDhPrmDir015(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dHecc * (body[iBody].dPinc * body[iBody].dPinc -
                               body[iBody].dQinc * body[iBody].dQinc) +
          2 * body[iBody].dPinc * body[iBody].dQinc * body[iBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][19];
}

/**
Sums over secular dR/dh terms of disturbing function for exterior body

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction

@return Total dR/dh for exterior body
*/
double fndDdisturbDHeccPrime(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac    = KGAUSS * KGAUSS * body[iaBody[1]].dMass / MSUN;
  dSemiPrm = body[iaBody[0]].dSemi / AUM;
  y        = (fndDdistDhPrmDir01(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhPrmDir02(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhPrmDir03(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhPrmDir04(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhPrmDir06(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhPrmDir07(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhPrmDir09(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhPrmDir010(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhPrmDir011(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhPrmDir012(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhPrmDir014(body, system, iaBody[0], iaBody[1]) +
       fndDdistDhPrmDir015(body, system, iaBody[0], iaBody[1])) *
      dMfac / dSemiPrm;
  return y;
}

//------------dR/dk--------------------------------------------------------------

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dk term for interior body
*/
double fndDdistDkDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dKecc *
         (system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][1] +
          2 *
                (body[iBody].dHecc * body[iBody].dHecc +
                 body[iBody].dKecc * body[iBody].dKecc) *
                system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][3] +
          (body[jBody].dHecc * body[jBody].dHecc +
           body[jBody].dKecc * body[jBody].dKecc) *
                system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][4] +
          (body[iBody].dPinc * body[iBody].dPinc +
           body[iBody].dQinc * body[iBody].dQinc +
           body[jBody].dPinc * body[jBody].dPinc +
           body[jBody].dQinc * body[jBody].dQinc) *
                system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][6]);
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dk term for interior body
*/
double fndDdistDkDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dKecc *
               (system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][9] +
                (body[jBody].dHecc * body[jBody].dHecc +
                 body[jBody].dKecc * body[jBody].dKecc) *
                      system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]]
                                        [11] +
                (body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc +
                 body[jBody].dPinc * body[jBody].dPinc +
                 body[jBody].dQinc * body[jBody].dQinc) *
                      system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]]
                                        [12]) +
         (3 * body[iBody].dKecc * body[iBody].dKecc * body[jBody].dKecc +
          body[jBody].dKecc * body[iBody].dHecc * body[iBody].dHecc +
          2 * body[iBody].dKecc * body[iBody].dHecc * body[jBody].dHecc) *
               system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][10];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dk term for interior body
*/
double fndDdistDkDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dKecc *
         (body[iBody].dPinc * body[jBody].dPinc +
          body[iBody].dQinc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][14];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dk term for interior body
*/
double fndDdistDkDir04(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2 * body[iBody].dKecc *
                (body[jBody].dKecc * body[jBody].dKecc -
                 body[jBody].dHecc * body[jBody].dHecc) +
          4 * body[iBody].dHecc * body[jBody].dHecc * body[jBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][16];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dk term for interior body
*/
double fndDdistDkDir05(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2 * body[iBody].dKecc *
                (body[iBody].dQinc * body[iBody].dQinc -
                 body[iBody].dPinc * body[iBody].dPinc) +
          4 * body[iBody].dPinc * body[iBody].dQinc * body[iBody].dHecc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][17];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dk term for interior body
*/
double fndDdistDkDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * (body[iBody].dQinc * body[iBody].dQinc -
                               body[iBody].dPinc * body[iBody].dPinc) +
          2 * body[jBody].dHecc * body[iBody].dPinc * body[iBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][18];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dk term for interior body
*/
double fndDdistDkDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dKecc * (body[jBody].dQinc * body[iBody].dQinc -
                               body[jBody].dPinc * body[iBody].dPinc) +
          body[iBody].dHecc * (body[jBody].dPinc * body[iBody].dQinc +
                               body[jBody].dQinc * body[iBody].dPinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][20];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dk term for interior body
*/
double fndDdistDkDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * (body[jBody].dQinc * body[iBody].dQinc +
                               body[jBody].dPinc * body[iBody].dPinc) +
          body[jBody].dHecc * (body[jBody].dPinc * body[iBody].dQinc -
                               body[jBody].dQinc * body[iBody].dPinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][21];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dk term for interior body
*/
double fndDdistDkDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * (body[jBody].dQinc * body[iBody].dQinc +
                               body[jBody].dPinc * body[iBody].dPinc) +
          body[jBody].dHecc * (body[jBody].dQinc * body[iBody].dPinc -
                               body[jBody].dPinc * body[iBody].dQinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][22];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dk term for interior body
*/
double fndDdistDkDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * (body[jBody].dQinc * body[iBody].dQinc -
                               body[jBody].dPinc * body[iBody].dPinc) +
          body[jBody].dHecc * (body[jBody].dPinc * body[iBody].dQinc +
                               body[jBody].dQinc * body[iBody].dPinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][23];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dk term for interior body
*/
double fndDdistDkDir013(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2 * body[iBody].dKecc *
                (body[jBody].dQinc * body[jBody].dQinc -
                 body[jBody].dPinc * body[jBody].dPinc) +
          4 * body[jBody].dPinc * body[jBody].dQinc * body[iBody].dHecc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][17];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dk term for interior body
*/
double fndDdistDkDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * (body[jBody].dQinc * body[jBody].dQinc -
                               body[jBody].dPinc * body[jBody].dPinc) +
          2 * body[jBody].dHecc * body[jBody].dPinc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][18];
}

/**
Sums over secular dR/dk terms of disturbing function for interior body

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction

@return Total dR/dk for interior body
*/
double fndDdisturbDKecc(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac    = KGAUSS * KGAUSS * body[iaBody[1]].dMass / MSUN;
  dSemiPrm = body[iaBody[1]].dSemi / AUM;
  y        = (fndDdistDkDir01(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkDir02(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkDir03(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkDir04(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkDir05(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkDir06(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkDir08(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkDir09(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkDir010(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkDir011(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkDir013(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkDir014(body, system, iaBody[0], iaBody[1])) *
      dMfac / dSemiPrm;
  return y;
}

//----------dR/dk'------------------------------------------------------------

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dk term for exterior body
*/
double fndDdistDkPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dKecc *
         (system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][1] +
          (body[jBody].dHecc * body[jBody].dHecc +
           body[jBody].dKecc * body[jBody].dKecc) *
                system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][4] +
          2 *
                (body[iBody].dHecc * body[iBody].dHecc +
                 body[iBody].dKecc * body[iBody].dKecc) *
                system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][5] +
          (body[jBody].dPinc * body[jBody].dPinc +
           body[jBody].dQinc * body[jBody].dQinc +
           body[iBody].dPinc * body[iBody].dPinc +
           body[iBody].dQinc * body[iBody].dQinc) *
                system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][6]);
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dk term for exterior body
*/
double fndDdistDkPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dKecc *
               (system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][9] +
                (body[jBody].dHecc * body[jBody].dHecc +
                 body[jBody].dKecc * body[jBody].dKecc) *
                      system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]]
                                        [10] +
                (body[jBody].dPinc * body[jBody].dPinc +
                 body[jBody].dQinc * body[jBody].dQinc +
                 body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc) *
                      system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]]
                                        [12]) +
         (body[iBody].dHecc * body[iBody].dHecc * body[jBody].dKecc +
          3 * body[jBody].dKecc * body[iBody].dKecc * body[iBody].dKecc +
          2 * body[iBody].dKecc * body[jBody].dHecc * body[iBody].dHecc) *
               system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][11];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dk term for exterior body
*/
double fndDdistDkPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dKecc *
         (body[jBody].dPinc * body[iBody].dPinc +
          body[iBody].dQinc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][14];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dk term for exterior body
*/
double fndDdistDkPrmDir04(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dKecc * (body[jBody].dKecc * body[jBody].dKecc -
                               body[jBody].dHecc * body[jBody].dHecc) +
          2 * body[jBody].dHecc * body[iBody].dHecc * body[jBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][16];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dk term for exterior body
*/
double fndDdistDkPrmDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * (body[jBody].dQinc * body[jBody].dQinc -
                               body[jBody].dPinc * body[jBody].dPinc) +
          2 * body[jBody].dHecc * body[jBody].dPinc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][18];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dk term for exterior body
*/
double fndDdistDkPrmDir07(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dKecc * (body[jBody].dQinc * body[jBody].dQinc -
                               body[jBody].dPinc * body[jBody].dPinc) +
          2 * body[jBody].dPinc * body[jBody].dQinc * body[iBody].dHecc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][19];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dk term for exterior body
*/
double fndDdistDkPrmDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * (body[iBody].dQinc * body[jBody].dQinc +
                               body[iBody].dPinc * body[jBody].dPinc) -
          body[jBody].dHecc * (body[iBody].dPinc * body[jBody].dQinc -
                               body[iBody].dQinc * body[jBody].dPinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][21];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dk term for exterior body
*/
double fndDdistDkPrmDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * (body[iBody].dQinc * body[jBody].dQinc +
                               body[iBody].dPinc * body[jBody].dPinc) -
          body[jBody].dHecc * (body[iBody].dQinc * body[jBody].dPinc -
                               body[iBody].dPinc * body[jBody].dQinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][22];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dk term for exterior body
*/
double fndDdistDkPrmDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * (body[jBody].dQinc * body[iBody].dQinc -
                               body[jBody].dPinc * body[iBody].dPinc) +
          body[jBody].dHecc * (body[iBody].dPinc * body[jBody].dQinc +
                               body[iBody].dQinc * body[jBody].dPinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][23];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dk term for exterior body
*/
double fndDdistDkPrmDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dKecc * (body[iBody].dQinc * body[jBody].dQinc -
                               body[iBody].dPinc * body[jBody].dPinc) +
          body[iBody].dHecc * (body[iBody].dPinc * body[jBody].dQinc +
                               body[iBody].dQinc * body[jBody].dPinc)) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][24];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dk term for exterior body
*/
double fndDdistDkPrmDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * (body[iBody].dQinc * body[iBody].dQinc -
                               body[iBody].dPinc * body[iBody].dPinc) +
          2 * body[jBody].dHecc * body[iBody].dPinc * body[iBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][18];
}

/**
Derivative in d/dk of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dk term for exterior body
*/
double fndDdistDkPrmDir015(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dKecc * (body[iBody].dQinc * body[iBody].dQinc -
                               body[iBody].dPinc * body[iBody].dPinc) +
          2 * body[iBody].dPinc * body[iBody].dQinc * body[iBody].dHecc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][19];
}

/**
Sums over secular dR/dk terms of disturbing function for exterior body

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction

@return Total dR/dk for exterior body
*/
double fndDdisturbDKeccPrime(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac    = KGAUSS * KGAUSS * body[iaBody[1]].dMass / MSUN;
  dSemiPrm = body[iaBody[0]].dSemi / AUM;
  y        = (fndDdistDkPrmDir01(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkPrmDir02(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkPrmDir03(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkPrmDir04(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkPrmDir06(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkPrmDir07(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkPrmDir09(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkPrmDir010(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkPrmDir011(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkPrmDir012(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkPrmDir014(body, system, iaBody[0], iaBody[1]) +
       fndDdistDkPrmDir015(body, system, iaBody[0], iaBody[1])) *
      dMfac / dSemiPrm;
  return y;
}

//------------dR/dp---------------------------------------------------------//

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dp term for interior body
*/
double fndDdistDpDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dPinc *
         (system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][2] +
          (body[iBody].dHecc * body[iBody].dHecc +
           body[iBody].dKecc * body[iBody].dKecc +
           body[jBody].dHecc * body[jBody].dHecc +
           body[jBody].dKecc * body[jBody].dKecc) *
                system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][6] +
          2 *
                (body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc) *
                system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][7] +
          (body[jBody].dPinc * body[jBody].dPinc +
           body[jBody].dQinc * body[jBody].dQinc) *
                system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][8]);
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dp term for interior body
*/
double fndDdistDpDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dPinc *
         (body[iBody].dHecc * body[jBody].dHecc +
          body[iBody].dKecc * body[jBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][12];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dp term for interior body
*/
double fndDdistDpDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dPinc *
               (system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][13] +
                (body[iBody].dHecc * body[iBody].dHecc +
                 body[iBody].dKecc * body[iBody].dKecc +
                 body[jBody].dHecc * body[jBody].dHecc +
                 body[jBody].dKecc * body[jBody].dKecc) *
                      system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]]
                                        [14] +
                (body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc +
                 body[jBody].dPinc * body[jBody].dPinc +
                 body[jBody].dQinc * body[jBody].dQinc) *
                      system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]]
                                        [15]) +
         2 * body[iBody].dPinc *
               (body[iBody].dPinc * body[jBody].dPinc +
                body[iBody].dQinc * body[jBody].dQinc) *
               system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][15];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dp term for interior body
*/
double fndDdistDpDir05(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dPinc * body[iBody].dHecc * body[iBody].dHecc -
          body[iBody].dPinc * body[iBody].dKecc * body[iBody].dKecc +
          2 * body[iBody].dQinc * body[iBody].dHecc * body[iBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][17];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dp term for interior body
*/
double fndDdistDpDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dHecc * body[jBody].dHecc * body[iBody].dPinc -
          body[iBody].dKecc * body[jBody].dKecc * body[iBody].dPinc +
          body[jBody].dHecc * body[iBody].dKecc * body[iBody].dQinc +
          body[jBody].dKecc * body[iBody].dHecc * body[iBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][18];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dp term for interior body
*/
double fndDdistDpDir07(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dPinc * body[jBody].dHecc * body[jBody].dHecc -
          body[iBody].dPinc * body[jBody].dKecc * body[jBody].dKecc +
          2 * body[iBody].dQinc * body[jBody].dHecc * body[jBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][19];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dp term for interior body
*/
double fndDdistDpDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dPinc * body[iBody].dHecc * body[iBody].dHecc -
          body[jBody].dPinc * body[iBody].dKecc * body[iBody].dKecc +
          2 * body[iBody].dHecc * body[iBody].dKecc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][20];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dp term for interior body
*/
double fndDdistDpDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * body[iBody].dKecc * body[jBody].dPinc +
          body[jBody].dHecc * body[iBody].dHecc * body[jBody].dPinc -
          body[jBody].dHecc * body[iBody].dKecc * body[jBody].dQinc +
          body[jBody].dKecc * body[iBody].dHecc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][21];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dp term for interior body
*/
double fndDdistDpDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * body[iBody].dKecc * body[jBody].dPinc +
          body[jBody].dHecc * body[iBody].dHecc * body[jBody].dPinc +
          body[jBody].dHecc * body[iBody].dKecc * body[jBody].dQinc -
          body[jBody].dKecc * body[iBody].dHecc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][22];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dp term for interior body
*/
double fndDdistDpDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (-body[jBody].dKecc * body[iBody].dKecc * body[jBody].dPinc +
          body[jBody].dHecc * body[iBody].dHecc * body[jBody].dPinc +
          body[jBody].dHecc * body[iBody].dKecc * body[jBody].dQinc +
          body[jBody].dKecc * body[iBody].dHecc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][23];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dp term for interior body
*/
double fndDdistDpDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (-body[jBody].dKecc * body[jBody].dKecc * body[jBody].dPinc +
          body[jBody].dHecc * body[jBody].dHecc * body[jBody].dPinc +
          2 * body[jBody].dHecc * body[jBody].dKecc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][24];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dp term for interior body
*/
double fndDdistDpDir016(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dPinc * body[jBody].dPinc * body[jBody].dPinc -
          body[iBody].dPinc * body[jBody].dQinc * body[jBody].dQinc +
          2 * body[jBody].dPinc * body[jBody].dQinc * body[iBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][25];
}

/**
Sums over secular dR/dp terms of disturbing function for interior body

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction

@return Total dR/dp for interior body
*/
double fndDdisturbDPinc(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac    = KGAUSS * KGAUSS * body[iaBody[1]].dMass / MSUN;
  dSemiPrm = body[iaBody[1]].dSemi / AUM;
  y        = (fndDdistDpDir01(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpDir02(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpDir03(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpDir05(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpDir06(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpDir07(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpDir08(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpDir09(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpDir010(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpDir011(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpDir012(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpDir016(body, system, iaBody[0], iaBody[1])) *
      dMfac / dSemiPrm;
  return y;
}

//-------dR/dp'------------------------------------------------------------//

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dp term for exterior body
*/
double fndDdistDpPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dPinc *
         (system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][2] +
          (body[jBody].dHecc * body[jBody].dHecc +
           body[jBody].dKecc * body[jBody].dKecc +
           body[iBody].dHecc * body[iBody].dHecc +
           body[iBody].dKecc * body[iBody].dKecc) *
                system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][6] +
          2 *
                (body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc) *
                system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][7] +
          (body[jBody].dPinc * body[jBody].dPinc +
           body[jBody].dQinc * body[jBody].dQinc) *
                system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][8]);
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dp term for exterior body
*/
double fndDdistDpPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dPinc *
         (body[jBody].dHecc * body[iBody].dHecc +
          body[jBody].dKecc * body[iBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][12];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dp term for exterior body
*/
double fndDdistDpPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dPinc *
               (system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][13] +
                (body[jBody].dHecc * body[jBody].dHecc +
                 body[jBody].dKecc * body[jBody].dKecc +
                 body[iBody].dHecc * body[iBody].dHecc +
                 body[iBody].dKecc * body[iBody].dKecc) *
                      system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]]
                                        [14] +
                (body[jBody].dPinc * body[jBody].dPinc +
                 body[jBody].dQinc * body[jBody].dQinc +
                 body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc) *
                      system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]]
                                        [15]) +
         2 * body[iBody].dPinc *
               (body[jBody].dPinc * body[iBody].dPinc +
                body[jBody].dQinc * body[iBody].dQinc) *
               system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][15];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dp term for exterior body
*/
double fndDdistDpPrmDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (-body[jBody].dKecc * body[jBody].dKecc * body[jBody].dPinc +
          body[jBody].dHecc * body[jBody].dHecc * body[jBody].dPinc +
          2 * body[jBody].dHecc * body[jBody].dKecc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][20];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dp term for exterior body
*/
double fndDdistDpPrmDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[iBody].dKecc * body[jBody].dKecc * body[jBody].dPinc +
          body[iBody].dHecc * body[jBody].dHecc * body[jBody].dPinc +
          body[iBody].dHecc * body[jBody].dKecc * body[jBody].dQinc -
          body[iBody].dKecc * body[jBody].dHecc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][21];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dp term for exterior body
*/
double fndDdistDpPrmDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[iBody].dKecc * body[jBody].dKecc * body[jBody].dPinc +
          body[iBody].dHecc * body[jBody].dHecc * body[jBody].dPinc -
          body[iBody].dHecc * body[jBody].dKecc * body[jBody].dQinc +
          body[iBody].dKecc * body[jBody].dHecc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][22];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dp term for exterior body
*/
double fndDdistDpPrmDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (-body[iBody].dKecc * body[jBody].dKecc * body[jBody].dPinc +
          body[iBody].dHecc * body[jBody].dHecc * body[jBody].dPinc +
          body[iBody].dHecc * body[jBody].dKecc * body[jBody].dQinc +
          body[iBody].dKecc * body[jBody].dHecc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][23];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dp term for exterior body
*/
double fndDdistDpPrmDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (-body[iBody].dKecc * body[iBody].dKecc * body[jBody].dPinc +
          body[iBody].dHecc * body[iBody].dHecc * body[jBody].dPinc +
          2 * body[iBody].dHecc * body[iBody].dKecc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][24];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dp term for exterior body
*/
double fndDdistDpPrmDir013(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (-body[iBody].dPinc * body[jBody].dKecc * body[jBody].dKecc +
          body[iBody].dPinc * body[jBody].dHecc * body[jBody].dHecc +
          2 * body[iBody].dQinc * body[jBody].dHecc * body[jBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][17];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dp term for exterior body
*/
double fndDdistDpPrmDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (-body[iBody].dKecc * body[jBody].dKecc * body[iBody].dPinc +
          body[iBody].dHecc * body[jBody].dHecc * body[iBody].dPinc +
          body[iBody].dHecc * body[jBody].dKecc * body[iBody].dQinc +
          body[iBody].dKecc * body[jBody].dHecc * body[iBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][18];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dp term for exterior body
*/
double fndDdistDpPrmDir015(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (-body[iBody].dPinc * body[iBody].dKecc * body[iBody].dKecc +
          body[iBody].dPinc * body[iBody].dHecc * body[iBody].dHecc +
          2 * body[iBody].dQinc * body[iBody].dHecc * body[iBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][19];
}

/**
Derivative in d/dp of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dp term for exterior body
*/
double fndDdistDpPrmDir016(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (-body[iBody].dPinc * body[jBody].dQinc * body[jBody].dQinc +
          body[jBody].dPinc * body[jBody].dPinc * body[iBody].dPinc +
          2 * body[iBody].dQinc * body[jBody].dPinc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][25];
}

/**
Sums over secular dR/dp terms of disturbing function for exterior body

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction

@return Total dR/dp for exterior body
*/
double fndDdisturbDPincPrime(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac    = KGAUSS * KGAUSS * body[iaBody[1]].dMass / MSUN;
  dSemiPrm = body[iaBody[0]].dSemi / AUM;
  y        = (fndDdistDpPrmDir01(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpPrmDir02(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpPrmDir03(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpPrmDir08(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpPrmDir09(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpPrmDir010(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpPrmDir011(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpPrmDir012(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpPrmDir013(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpPrmDir014(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpPrmDir015(body, system, iaBody[0], iaBody[1]) +
       fndDdistDpPrmDir016(body, system, iaBody[0], iaBody[1])) *
      dMfac / dSemiPrm;
  return y;
}

//--------------dR/dq-------------------------------------------------------//

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body
@return dR/dq term for interior body
*/
double fndDdistDqDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dQinc *
         (system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][2] +
          (body[iBody].dHecc * body[iBody].dHecc +
           body[iBody].dKecc * body[iBody].dKecc +
           body[jBody].dKecc * body[jBody].dKecc +
           body[jBody].dHecc * body[jBody].dHecc) *
                system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][6] +
          2 *
                (body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc) *
                system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][7] +
          (body[jBody].dPinc * body[jBody].dPinc +
           body[jBody].dQinc * body[jBody].dQinc) *
                system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][8]);
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body
@return dR/dq term for interior body
*/
double fndDdistDqDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dQinc *
         (body[iBody].dHecc * body[jBody].dHecc +
          body[iBody].dKecc * body[jBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][12];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body

@return dR/dq term for interior body
*/
double fndDdistDqDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dQinc *
               (system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][13] +
                (body[iBody].dHecc * body[iBody].dHecc +
                 body[iBody].dKecc * body[iBody].dKecc +
                 body[jBody].dHecc * body[jBody].dHecc +
                 body[jBody].dKecc * body[jBody].dKecc) *
                      system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]]
                                        [14] +
                (body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc +
                 body[jBody].dPinc * body[jBody].dPinc +
                 body[jBody].dQinc * body[jBody].dQinc) *
                      system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]]
                                        [15]) +
         2 * body[iBody].dQinc *
               (body[iBody].dPinc * body[jBody].dPinc +
                body[iBody].dQinc * body[jBody].dQinc) *
               system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][15];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body
@return dR/dq term for interior body
*/
double fndDdistDqDir05(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dQinc * body[iBody].dKecc * body[iBody].dKecc -
          body[iBody].dQinc * body[iBody].dHecc * body[iBody].dHecc +
          2 * body[iBody].dPinc * body[iBody].dHecc * body[iBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][17];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body
@return dR/dq term for interior body
*/
double fndDdistDqDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[jBody].dKecc * body[iBody].dKecc * body[iBody].dQinc -
          body[jBody].dHecc * body[iBody].dHecc * body[iBody].dQinc +
          body[jBody].dHecc * body[iBody].dKecc * body[iBody].dPinc +
          body[jBody].dKecc * body[iBody].dHecc * body[iBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][18];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body
@return dR/dq term for interior body
*/
double fndDdistDqDir07(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dQinc * body[jBody].dKecc * body[jBody].dKecc -
          body[iBody].dQinc * body[jBody].dHecc * body[jBody].dHecc +
          2 * body[iBody].dPinc * body[jBody].dHecc * body[jBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][19];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body
@return dR/dq term for interior body
*/
double fndDdistDqDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[iBody].dKecc * body[iBody].dKecc * body[jBody].dQinc -
          body[iBody].dHecc * body[iBody].dHecc * body[jBody].dQinc +
          2 * body[iBody].dHecc * body[iBody].dKecc * body[jBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][20];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body
@return dR/dq term for interior body
*/
double fndDdistDqDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * body[iBody].dKecc * body[jBody].dQinc +
          body[jBody].dHecc * body[iBody].dHecc * body[jBody].dQinc +
          body[jBody].dHecc * body[iBody].dKecc * body[jBody].dPinc -
          body[jBody].dKecc * body[iBody].dHecc * body[jBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][21];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body
@return dR/dq term for interior body
*/
double fndDdistDqDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * body[iBody].dKecc * body[jBody].dQinc +
          body[iBody].dHecc * body[jBody].dHecc * body[jBody].dQinc -
          body[jBody].dHecc * body[iBody].dKecc * body[jBody].dPinc +
          body[jBody].dKecc * body[iBody].dHecc * body[jBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][22];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body
@return dR/dq term for interior body
*/
double fndDdistDqDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * body[iBody].dKecc * body[jBody].dQinc -
          body[jBody].dHecc * body[iBody].dHecc * body[jBody].dQinc +
          body[jBody].dHecc * body[iBody].dKecc * body[jBody].dPinc +
          body[jBody].dKecc * body[iBody].dHecc * body[jBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][23];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body
@return dR/dq term for interior body
*/
double fndDdistDqDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * body[jBody].dKecc * body[jBody].dQinc -
          body[jBody].dHecc * body[jBody].dHecc * body[jBody].dQinc +
          2 * body[jBody].dHecc * body[jBody].dKecc * body[jBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][24];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of interior body
@param jBody Index of exterior body
@return dR/dq term for interior body
*/
double fndDdistDqDir016(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dQinc * body[jBody].dQinc * body[jBody].dQinc -
          body[iBody].dQinc * body[jBody].dPinc * body[jBody].dPinc +
          2 * body[jBody].dPinc * body[jBody].dQinc * body[iBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[iBody][jBody]][25];
}

/**
Sums over secular dR/dq terms of disturbing function for interior body

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Total dR/dq for interior body
*/
double fndDdisturbDQinc(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac    = KGAUSS * KGAUSS * body[iaBody[1]].dMass / MSUN;
  dSemiPrm = body[iaBody[1]].dSemi / AUM;
  y        = (fndDdistDqDir01(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqDir02(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqDir03(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqDir05(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqDir06(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqDir07(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqDir08(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqDir09(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqDir010(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqDir011(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqDir012(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqDir016(body, system, iaBody[0], iaBody[1])) *
      dMfac / dSemiPrm;
  return y;
}

//------dR/dq'--------------------------------------------------------------//

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body
@return dR/dq term for exterior body
*/
double fndDdistDqPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dQinc *
         (system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][2] +
          (body[jBody].dHecc * body[jBody].dHecc +
           body[jBody].dKecc * body[jBody].dKecc +
           body[iBody].dHecc * body[iBody].dHecc +
           body[iBody].dKecc * body[iBody].dKecc) *
                system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][6] +
          2 *
                (body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc) *
                system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][7] +
          (body[jBody].dPinc * body[jBody].dPinc +
           body[jBody].dQinc * body[jBody].dQinc) *
                system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][8]);
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body
@return dR/dq term for exterior body
*/
double fndDdistDqPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 * body[iBody].dQinc *
         (body[jBody].dHecc * body[iBody].dHecc +
          body[jBody].dKecc * body[iBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][12];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body
@return dR/dq term for exterior body
*/
double fndDdistDqPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dQinc *
               (system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][13] +
                (body[jBody].dHecc * body[jBody].dHecc +
                 body[jBody].dKecc * body[jBody].dKecc +
                 body[iBody].dHecc * body[iBody].dHecc +
                 body[iBody].dKecc * body[iBody].dKecc) *
                      system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]]
                                        [14] +
                (body[jBody].dPinc * body[jBody].dPinc +
                 body[jBody].dQinc * body[jBody].dQinc +
                 body[iBody].dPinc * body[iBody].dPinc +
                 body[iBody].dQinc * body[iBody].dQinc) *
                      system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]]
                                        [15]) +
         2 * body[iBody].dQinc *
               (body[jBody].dPinc * body[iBody].dPinc +
                body[jBody].dQinc * body[iBody].dQinc) *
               system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][15];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body
@return dR/dq term for exterior body
*/
double fndDdistDqPrmDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dKecc * body[jBody].dKecc * body[jBody].dQinc -
          body[jBody].dHecc * body[jBody].dHecc * body[jBody].dQinc +
          2 * body[jBody].dHecc * body[jBody].dKecc * body[jBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][20];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body
@return dR/dq term for exterior body
*/
double fndDdistDqPrmDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[iBody].dKecc * body[jBody].dKecc * body[jBody].dQinc +
          body[iBody].dHecc * body[jBody].dHecc * body[jBody].dQinc -
          body[iBody].dHecc * body[jBody].dKecc * body[jBody].dPinc +
          body[iBody].dKecc * body[jBody].dHecc * body[jBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][21];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body
@return dR/dq term for exterior body
*/
double fndDdistDqPrmDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[iBody].dKecc * body[jBody].dKecc * body[jBody].dQinc +
          body[iBody].dHecc * body[jBody].dHecc * body[jBody].dQinc +
          body[iBody].dHecc * body[jBody].dKecc * body[jBody].dPinc -
          body[iBody].dKecc * body[jBody].dHecc * body[jBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][22];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body
@return dR/dq term for exterior body
*/
double fndDdistDqPrmDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[iBody].dKecc * body[jBody].dKecc * body[jBody].dQinc -
          body[iBody].dHecc * body[jBody].dHecc * body[jBody].dQinc +
          body[iBody].dHecc * body[jBody].dKecc * body[jBody].dPinc +
          body[iBody].dKecc * body[jBody].dHecc * body[jBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][23];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body
@return dR/dq term for exterior body
*/
double fndDdistDqPrmDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[iBody].dKecc * body[iBody].dKecc * body[jBody].dQinc -
          body[iBody].dHecc * body[iBody].dHecc * body[jBody].dQinc +
          2 * body[iBody].dHecc * body[iBody].dKecc * body[jBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][24];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body
@return dR/dq term for exterior body
*/
double fndDdistDqPrmDir013(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dQinc * body[jBody].dKecc * body[jBody].dKecc -
          body[iBody].dQinc * body[jBody].dHecc * body[jBody].dHecc +
          2 * body[iBody].dPinc * body[jBody].dHecc * body[jBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][17];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body
@return dR/dq term for exterior body
*/
double fndDdistDqPrmDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dQinc * body[jBody].dKecc * body[iBody].dKecc -
          body[iBody].dQinc * body[jBody].dHecc * body[iBody].dHecc +
          body[iBody].dHecc * body[jBody].dKecc * body[iBody].dPinc +
          body[iBody].dKecc * body[jBody].dHecc * body[iBody].dPinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][18];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body
@return dR/dq term for exterior body
*/
double fndDdistDqPrmDir015(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[iBody].dQinc * body[iBody].dKecc * body[iBody].dKecc -
          body[iBody].dQinc * body[iBody].dHecc * body[iBody].dHecc +
          2 * body[iBody].dPinc * body[iBody].dHecc * body[iBody].dKecc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][19];
}

/**
Derivative in d/dq of disturbing function term

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iBody Index of exterior body
@param jBody Index of interior body

@return dR/dq term for exterior body
*/
double fndDdistDqPrmDir016(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2 *
         (body[jBody].dQinc * body[jBody].dQinc * body[iBody].dQinc -
          body[jBody].dPinc * body[jBody].dPinc * body[iBody].dQinc +
          2 * body[iBody].dPinc * body[jBody].dPinc * body[jBody].dQinc) *
         system->daLaplaceC[0][system->iaLaplaceN[jBody][iBody]][25];
}

/**
Sums over secular dR/dq terms of disturbing function for exterior body

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Total dR/dq for exterior body
*/
double fndDdisturbDQincPrime(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac    = KGAUSS * KGAUSS * body[iaBody[1]].dMass / MSUN;
  dSemiPrm = body[iaBody[0]].dSemi / AUM;
  y        = (fndDdistDqPrmDir01(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqPrmDir02(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqPrmDir03(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqPrmDir08(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqPrmDir09(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqPrmDir010(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqPrmDir011(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqPrmDir012(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqPrmDir013(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqPrmDir014(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqPrmDir015(body, system, iaBody[0], iaBody[1]) +
       fndDdistDqPrmDir016(body, system, iaBody[0], iaBody[1])) *
      dMfac / dSemiPrm;
  return y;
}

//--------Relativistic
// correction---------------------------------------------------------

/**
Relativistic precession of periastron

@param body Struct containing all body information and variables
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative d(longp)/dt due to general relativity
*/
double fndApsidalGRCorrection(BODY *body, int *iaBody) {
  double n;
  n = KGAUSS * sqrt((body[0].dMass + body[iaBody[0]].dMass) / MSUN /
                    (body[iaBody[0]].dSemi / AUM * body[iaBody[0]].dSemi / AUM *
                     body[iaBody[0]].dSemi / AUM));
  return 3 * n * n * n * body[iaBody[0]].dSemi / AUM * body[iaBody[0]].dSemi /
         AUM /
         (LIGHTSPEED / AUM * DAYSEC * LIGHTSPEED / AUM * DAYSEC *
          (1.0 - body[iaBody[0]].dHecc * body[iaBody[0]].dHecc -
           body[iaBody[0]].dKecc * body[iaBody[0]].dKecc)) /
         DAYSEC;
}

/**
Relativistic correction to derivative of variable Hecc = e*sin(longp) in RD4
solution

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dh/dt due to general relativity
*/
double fndApsidalGRDhDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dKecc * fndApsidalGRCorrection(body, iaBody);
}

/**
Relativistic correction to derivative of variable Kecc = e*sin(longp) in RD4
solution

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dk/dt due to general relativity
*/
double fndApsidalGRDkDt(BODY *body, SYSTEM *system, int *iaBody) {
  return -body[iaBody[0]].dHecc * fndApsidalGRCorrection(body, iaBody);
}

//-------------------DistOrb's equations in h k p q (4th order direct
// integration RD4)--------------------

/**
Derivative of variable Hecc = e*sin(longp) in RD4 solution

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dh/dt
*/
double fndDistOrbRD4DhDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;
  // Here, iaBody[0] = body in question, iaBody[1] = perturber

  dMu = KGAUSS * KGAUSS * (body[0].dMass + body[iaBody[0]].dMass) / MSUN;
  y   = fabs(1 - body[iaBody[0]].dHecc * body[iaBody[0]].dHecc -
             body[iaBody[0]].dKecc * body[iaBody[0]].dKecc);
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += (sqrt(y) * fndDdisturbDKecc(body, system, iaBody) +
            body[iaBody[0]].dKecc *
                  (body[iaBody[0]].dPinc *
                         fndDdisturbDPinc(body, system, iaBody) +
                   body[iaBody[0]].dQinc *
                         fndDdisturbDQinc(body, system, iaBody)) /
                  (2 * sqrt(y))) /
           sqrt(dMu * body[iaBody[0]].dSemi / AUM);

  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += (sqrt(y) * fndDdisturbDKeccPrime(body, system, iaBody) +
            body[iaBody[0]].dKecc *
                  (body[iaBody[0]].dPinc *
                         fndDdisturbDPincPrime(body, system, iaBody) +
                   body[iaBody[0]].dQinc *
                         fndDdisturbDQincPrime(body, system, iaBody)) /
                  (2 * sqrt(y))) /
           sqrt(dMu * body[iaBody[0]].dSemi / AUM);
  }

  return sum / DAYSEC;
}

/**
Derivative of variable Kecc = e*cos(longp) in RD4 solution

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dk/dt
*/
double fndDistOrbRD4DkDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;

  dMu = KGAUSS * KGAUSS * (body[0].dMass + body[iaBody[0]].dMass) / MSUN;
  y   = fabs(1 - body[iaBody[0]].dHecc * body[iaBody[0]].dHecc -
             body[iaBody[0]].dKecc * body[iaBody[0]].dKecc);
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += -(sqrt(y) * fndDdisturbDHecc(body, system, iaBody) +
             body[iaBody[0]].dHecc *
                   (body[iaBody[0]].dPinc *
                          fndDdisturbDPinc(body, system, iaBody) +
                    body[iaBody[0]].dQinc *
                          fndDdisturbDQinc(body, system, iaBody)) /
                   (2 * sqrt(y))) /
           sqrt(dMu * body[iaBody[0]].dSemi / AUM);

  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += -(sqrt(y) * fndDdisturbDHeccPrime(body, system, iaBody) +
             body[iaBody[0]].dHecc *
                   (body[iaBody[0]].dPinc *
                          fndDdisturbDPincPrime(body, system, iaBody) +
                    body[iaBody[0]].dQinc *
                          fndDdisturbDQincPrime(body, system, iaBody)) /
                   (2 * sqrt(y))) /
           sqrt(dMu * body[iaBody[0]].dSemi / AUM);
  }

  return sum / DAYSEC;
}

/**
Derivative of variable Pinc = sin(inc/2)*sin(longa) in RD4 solution

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dp/dt
*/
double fndDistOrbRD4DpDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;

  dMu = KGAUSS * KGAUSS * (body[0].dMass + body[iaBody[0]].dMass) / MSUN;
  y   = fabs(1 - body[iaBody[0]].dHecc * body[iaBody[0]].dHecc -
             body[iaBody[0]].dKecc * body[iaBody[0]].dKecc);
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += (body[iaBody[0]].dPinc *
                  (-body[iaBody[0]].dKecc *
                         fndDdisturbDHecc(body, system, iaBody) +
                   body[iaBody[0]].dHecc *
                         fndDdisturbDKecc(body, system, iaBody)) +
            1.0 / 2.0 * fndDdisturbDQinc(body, system, iaBody)) /
           (2 * sqrt(dMu * body[iaBody[0]].dSemi / AUM * (y)));
  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += (body[iaBody[0]].dPinc *
                  (-body[iaBody[0]].dKecc *
                         fndDdisturbDHeccPrime(body, system, iaBody) +
                   body[iaBody[0]].dHecc *
                         fndDdisturbDKeccPrime(body, system, iaBody)) +
            1.0 / 2.0 * fndDdisturbDQincPrime(body, system, iaBody)) /
           (2 * sqrt(dMu * body[iaBody[0]].dSemi / AUM * (y)));
  }

  return sum / DAYSEC;
}

/**
Derivative of variable Qinc = sin(inc/2)*cos(longa) in RD4 solution

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dq/dt
*/
double fndDistOrbRD4DqDt(BODY *body, SYSTEM *system, int *iaBody) {
  double sum = 0.0, dMu, y;

  dMu = KGAUSS * KGAUSS * (body[0].dMass + body[iaBody[0]].dMass) / MSUN;
  y   = fabs(1 - body[iaBody[0]].dHecc * body[iaBody[0]].dHecc -
             body[iaBody[0]].dKecc * body[iaBody[0]].dKecc);
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += (body[iaBody[0]].dQinc *
                  (-body[iaBody[0]].dKecc *
                         fndDdisturbDHecc(body, system, iaBody) +
                   body[iaBody[0]].dHecc *
                         fndDdisturbDKecc(body, system, iaBody)) -
            1.0 / 2.0 * fndDdisturbDPinc(body, system, iaBody)) /
           (2 * sqrt(dMu * body[iaBody[0]].dSemi / AUM * (y)));
  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += (body[iaBody[0]].dQinc *
                  (-body[iaBody[0]].dKecc *
                         fndDdisturbDHeccPrime(body, system, iaBody) +
                   body[iaBody[0]].dHecc *
                         fndDdisturbDKeccPrime(body, system, iaBody)) -
            1.0 / 2.0 * fndDdisturbDPincPrime(body, system, iaBody)) /
           (2 * sqrt(dMu * body[iaBody[0]].dSemi / AUM * (y)));
  }

  return sum / DAYSEC;
}

//-------------------DistOrb's equations in h k p q (2nd order Laplace-Lagrange
// LL2)--------------------
/**
Value of variable Hecc = e*sin(longp) at time dAge, in the LL2 solution (not a
derivative)

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Hecc at time dAge
*/
double fndDistOrbLL2Hecc(BODY *body, SYSTEM *system, int *iaBody) {
  return system->daEigenVecEcc[iaBody[0] - 1][iaBody[1] - 1] *
         sin(system->daEigenValEcc[0][iaBody[1] - 1] / YEARSEC *
                   body[iaBody[0]].dAge +
             system->daEigenPhase[0][iaBody[1] - 1]);
}

/**
Value of variable Kecc = e*cos(longp) at time dAge, in the LL2 solution (not a
derivative)

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Kecc at time dAge
*/
double fndDistOrbLL2Kecc(BODY *body, SYSTEM *system, int *iaBody) {
  return system->daEigenVecEcc[iaBody[0] - 1][iaBody[1] - 1] *
         cos(system->daEigenValEcc[0][iaBody[1] - 1] / YEARSEC *
                   body[iaBody[0]].dAge +
             system->daEigenPhase[0][iaBody[1] - 1]);
}

/**
Value of variable Pinc = sin(inc/2)*sin(longp) at time dAge, in the LL2 solution
(not a derivative)

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Pinc at time dAge
*/
double fndDistOrbLL2Pinc(BODY *body, SYSTEM *system, int *iaBody) {
  return system->daEigenVecInc[iaBody[0] - 1][iaBody[1] - 1] *
         sin(system->daEigenValInc[0][iaBody[1] - 1] / YEARSEC *
                   body[iaBody[0]].dAge +
             system->daEigenPhase[1][iaBody[1] - 1]);
}

/**
Value of variable Qinc = sin(inc/2)*cos(longp) at time dAge, in the LL2 solution
(not a derivative)

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction

@return Qinc at time dAge
*/
double fndDistOrbLL2Qinc(BODY *body, SYSTEM *system, int *iaBody) {
  return system->daEigenVecInc[iaBody[0] - 1][iaBody[1] - 1] *
         cos(system->daEigenValInc[0][iaBody[1] - 1] / YEARSEC *
                   body[iaBody[0]].dAge +
             system->daEigenPhase[1][iaBody[1] - 1]);
}

/**
Provides derivative of variable Hecc = e*sin(longp) to couple eqtide to LL2
solution.

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dh/dt
*/
double fndDistOrbLL2DhDt(BODY *body, SYSTEM *system, int *iaBody) {
  return system->daEigenVecEcc[iaBody[0] - 1][iaBody[1] - 1] *
         system->daEigenValEcc[0][iaBody[1] - 1] / YEARSEC *
         cos(system->daEigenValEcc[0][iaBody[1] - 1] / YEARSEC *
                   body[iaBody[0]].dAge +
             system->daEigenPhase[0][iaBody[1] - 1]);
}

/**
Provides derivative of variable Kecc = e*sin(longp) to couple eqtide to LL2
solution.

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dk/dt
*/
double fndDistOrbLL2DkDt(BODY *body, SYSTEM *system, int *iaBody) {
  return -system->daEigenVecEcc[iaBody[0] - 1][iaBody[1] - 1] *
         system->daEigenValEcc[0][iaBody[1] - 1] / YEARSEC *
         sin(system->daEigenValEcc[0][iaBody[1] - 1] / YEARSEC *
                   body[iaBody[0]].dAge +
             system->daEigenPhase[0][iaBody[1] - 1]);
}

/**
Provides derivative of variable Pinc = sin(inc/2)*sin(longa) to couple distrot
to LL2 solution.

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dp/dt
*/
double fndDistOrbLL2DpDt(BODY *body, SYSTEM *system, int *iaBody) {
  /* Derivatives used by DistRot */
  return system->daEigenVecInc[iaBody[0] - 1][iaBody[1] - 1] *
         system->daEigenValInc[0][iaBody[1] - 1] / YEARSEC *
         cos(system->daEigenValInc[0][iaBody[1] - 1] / YEARSEC *
                   body[iaBody[0]].dAge +
             system->daEigenPhase[1][iaBody[1] - 1]);
}

/**
Provides derivative of variable Qinc = sin(inc/2)*cos(longa) to couple distrot
to LL2 solution.

@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dq/dt
*/
double fndDistOrbLL2DqDt(BODY *body, SYSTEM *system, int *iaBody) {
  /* Derivatives used by DistRot */
  return -system->daEigenVecInc[iaBody[0] - 1][iaBody[1] - 1] *
         system->daEigenValInc[0][iaBody[1] - 1] / YEARSEC *
         sin(system->daEigenValInc[0][iaBody[1] - 1] / YEARSEC *
                   body[iaBody[0]].dAge +
             system->daEigenPhase[1][iaBody[1] - 1]);
}

double fdInclination(BODY *body, int iBody) {
  double dInclination;

  dInclination = 2. * asin(sqrt(body[iBody].dPinc * body[iBody].dPinc +
                                body[iBody].dQinc * body[iBody].dQinc));
  return dInclination;
}

double fdLongA(BODY *body, int iBody) {
  double dLongA;

  dLongA = atan2(body[iBody].dPinc, body[iBody].dQinc);
  return dLongA;
}
