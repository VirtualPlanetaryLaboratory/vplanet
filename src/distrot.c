/**
   @file distrot.c
   @brief Subroutines that control the integration of the obliquity model.
   @author Russell Deitrick ([deitrr](https://github.com/deitrr/))
   @date July 7 2015
*/

/* lines where something like iBody == 0 occurs
 * ~149
 * ~227
 */

#include "vplanet.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void BodyCopyDistRot(BODY *dest, BODY *src, int iTideModel, int iNumBodies,
                     int iBody) {
  int iIndex, iPert;

  dest[iBody].dXobl           = src[iBody].dXobl;
  dest[iBody].dYobl           = src[iBody].dYobl;
  dest[iBody].dZobl           = src[iBody].dZobl;
  dest[iBody].dDynEllip       = src[iBody].dDynEllip;
  dest[iBody].bForcePrecRate  = src[iBody].bForcePrecRate;
  dest[iBody].dPrecRate       = src[iBody].dPrecRate;
  dest[iBody].iCurrentStep    = src[iBody].iCurrentStep;
  dest[iBody].bReadOrbitData  = src[iBody].bReadOrbitData;
  dest[iBody].bCalcDynEllip   = src[iBody].bCalcDynEllip;
  dest[iBody].dSpecMomInertia = src[iBody].dSpecMomInertia;
}

void InitializeUpdateTmpBodyDistRot(BODY *body, CONTROL *control,
                                    UPDATE *update, int iBody) {
  if (body[iBody].bReadOrbitData) {
    int iLine;

    control->Evolve.tmpBody[iBody].daSemiSeries =
          malloc(body[iBody].iNLines * sizeof(double));
    control->Evolve.tmpBody[iBody].daHeccSeries =
          malloc(body[iBody].iNLines * sizeof(double));
    control->Evolve.tmpBody[iBody].daKeccSeries =
          malloc(body[iBody].iNLines * sizeof(double));
    control->Evolve.tmpBody[iBody].daPincSeries =
          malloc(body[iBody].iNLines * sizeof(double));
    control->Evolve.tmpBody[iBody].daQincSeries =
          malloc(body[iBody].iNLines * sizeof(double));

    for (iLine = 0; iLine < body[iBody].iNLines; iLine++) {
      control->Evolve.tmpBody[iBody].daSemiSeries[iLine] =
            body[iBody].daSemiSeries[iLine];
      control->Evolve.tmpBody[iBody].daHeccSeries[iLine] =
            body[iBody].daHeccSeries[iLine];
      control->Evolve.tmpBody[iBody].daKeccSeries[iLine] =
            body[iBody].daKeccSeries[iLine];
      control->Evolve.tmpBody[iBody].daPincSeries[iLine] =
            body[iBody].daPincSeries[iLine];
      control->Evolve.tmpBody[iBody].daQincSeries[iLine] =
            body[iBody].daQincSeries[iLine];
    }
  }
}

/**************** DISTROT options ********************/

void ReadForcePrecRate(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].bForcePrecRate = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    body[iFile - 1].bForcePrecRate = options->dDefault;
  }
}

void ReadPrecRate(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* Cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);

    body[iFile - 1].dPrecRate = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &body[iFile - 1].dPrecRate, files->iNumInputs);
  }
}

void ReadSpecMomInertia(BODY *body, CONTROL *control, FILES *files,
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
    body[iFile - 1].dSpecMomInertia = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &body[iFile - 1].dSpecMomInertia,
                        files->iNumInputs);
  }
}

void ReadOrbitData(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].bReadOrbitData = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    body[iFile - 1].bReadOrbitData = options->dDefault;
  }
}

void ReadFileOrbitData(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file -- Each body has an output file */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    strcpy(body[iFile - 1].cFileOrbitData, cTmp);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    //       sprintf(body[iFile-1].cFileOrbitData,"%s",options[OPT_FILEORBITDATA].cDefault);
    strcpy(body[iFile - 1].cFileOrbitData, options->cDefault);
  }
}

void InitializeOptionsDistRot(OPTIONS *options, fnReadOption fnRead[]) {

  sprintf(options[OPT_DYNELLIP].cName, "dDynEllip");
  sprintf(options[OPT_DYNELLIP].cDescr, "Planet's dynamical ellipticity");
  sprintf(options[OPT_DYNELLIP].cDefault, "0.00328");
  sprintf(options[OPT_DYNELLIP].cDimension, "nd");
  options[OPT_DYNELLIP].dDefault   = 0.00328;
  options[OPT_DYNELLIP].iType      = 2;
  options[OPT_DYNELLIP].bMultiFile = 1;
  fnRead[OPT_DYNELLIP]             = &ReadDynEllip;

  sprintf(options[OPT_CALCDYNELLIP].cName, "bCalcDynEllip");
  sprintf(options[OPT_CALCDYNELLIP].cDescr,
          "Calculate dynamical ellipticity from RotRate");
  sprintf(options[OPT_CALCDYNELLIP].cDefault, "0");
  options[OPT_CALCDYNELLIP].dDefault   = 0;
  options[OPT_CALCDYNELLIP].iType      = 0;
  options[OPT_CALCDYNELLIP].bMultiFile = 1;
  fnRead[OPT_CALCDYNELLIP]             = &ReadCalcDynEllip;
  sprintf(
        options[OPT_CALCDYNELLIP].cLongDescr,
        "In DistRot, set this to 1 to force the equilibrium shape (dynamical \n"
        "ellipticity) to be calculated based on the current rotation rate and "
        "\n"
        "hydrostatic equilibrium. Note that once the planet's rotation period "
        "\n"
        "exceeds 13 days, then the dynamical ellipticity will be held fixed \n"
        "to match Venus' value. This calculation does not include tides. "
        "Setting \n"
        "this to 1 overrides %s.",
        options[OPT_DYNELLIP].cName // XXX Check this!
  );

  sprintf(options[OPT_FORCEPRECRATE].cName, "bForcePrecRate");
  sprintf(options[OPT_FORCEPRECRATE].cDescr,
          "Set the axial precession to a fixed rate?");
  sprintf(options[OPT_FORCEPRECRATE].cDefault, "0");
  options[OPT_FORCEPRECRATE].dDefault   = 0;
  options[OPT_FORCEPRECRATE].iType      = 0;
  options[OPT_FORCEPRECRATE].bMultiFile = 1;
  fnRead[OPT_FORCEPRECRATE]             = &ReadForcePrecRate;
  sprintf(options[OPT_FORCEPRECRATE].cLongDescr,
          "In DisRot, set the axial precession rate to a fixed value. This "
          "option \n"
          "can mimic the forcing of a natural satellite, or be used for "
          "testing.");

  sprintf(options[OPT_PRECRATE].cName, "dPrecRate");
  sprintf(options[OPT_PRECRATE].cDescr,
          "Fixed rate of axial precession (angle/s)");
  sprintf(options[OPT_PRECRATE].cDefault, "7.7261e-12");
  sprintf(options[OPT_PRECRATE].cDimension, "angle/time");
  options[OPT_PRECRATE].dDefault   = 7.7261e-12;
  options[OPT_PRECRATE].iType      = 2;
  options[OPT_PRECRATE].bMultiFile = 1;
  fnRead[OPT_PRECRATE]             = &ReadPrecRate;
  sprintf(options[OPT_READORBITDATA].cLongDescr,
          "Value of the body's axial precession frequency if %s is set to 1.\n"
          "Default value is the modern Earth's value as driven by the Moon.",
          options[OPT_FORCEPRECRATE].cName);

  sprintf(options[OPT_SPECMOMINERTIA].cName, "dSpecMomInertia");
  sprintf(options[OPT_SPECMOMINERTIA].cDescr,
          "Specific moment of inertia of polar axis");
  sprintf(options[OPT_SPECMOMINERTIA].cDefault, "0.33");
  sprintf(options[OPT_SPECMOMINERTIA].cDimension, "nd");
  options[OPT_SPECMOMINERTIA].dDefault   = 0.33;
  options[OPT_SPECMOMINERTIA].iType      = 2;
  options[OPT_SPECMOMINERTIA].bMultiFile = 1;
  fnRead[OPT_SPECMOMINERTIA]             = &ReadSpecMomInertia;

  sprintf(options[OPT_FILEORBITDATA].cName, "sFileOrbitData");
  // Define OPT_READORBITDATA so it can be used in the long help
  sprintf(options[OPT_READORBITDATA].cName, "bReadOrbitData");
  sprintf(options[OPT_FILEORBITDATA].cDescr,
          "Name of file containing orbit time series");
  sprintf(options[OPT_FILEORBITDATA].cDefault, "orbit.txt");
  options[OPT_FILEORBITDATA].iType = 3;
  fnRead[OPT_FILEORBITDATA]        = &ReadFileOrbitData;
  sprintf(
        options[OPT_FILEORBITDATA].cLongDescr,
        "File containing pre-computed orbital data. The file must have the \n"
        "following format: Time SemiMajorAxis Eccentricity Inclination \n"
        "ArgPericenter LongAscNode MeanAnomaly. The units of those \n"
        "parameters is assumed to be the same as body being simulated. When \n"
        "using this option, the integration must used a fixed timestep \n"
        "(%s = 0), and the timestep (%s) must equal the cadence in the file, \n"
        "with time units in the %s file assumed to be the same as for the \n"
        "body file. See %s for more information.",
        options[OPT_VARDT].cName, options[OPT_TIMESTEP].cName,
        options[OPT_FILEORBITDATA].cName, options[OPT_READORBITDATA].cName);

  // cName defined above
  sprintf(options[OPT_READORBITDATA].cDescr,
          "Read in orbital data for use with distrot?");
  sprintf(options[OPT_READORBITDATA].cDefault, "0");
  options[OPT_READORBITDATA].dDefault   = 0;
  options[OPT_READORBITDATA].iType      = 0;
  options[OPT_READORBITDATA].bMultiFile = 1;
  fnRead[OPT_READORBITDATA]             = &ReadOrbitData;
  sprintf(options[OPT_READORBITDATA].cLongDescr,
          "Rather than calculate orbital evolution with DistOrb or SpiNBody, "
          "users \n"
          "may read in a previously run simulation. See %s for more \n"
          "information.",
          options[OPT_FILEORBITDATA].cName);
}

void ReadOptionsDistRot(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, fnReadOption fnRead[],
                        int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTDISTROT; iOpt < OPTENDDISTROT; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}


/******************* Verify DISTROT ******************/

/* In the following, iBody is the current body number that is getting assigned,
   iPert counts the number of bodies perturbing iBody, and iBodyPert is the
   body number of the current perturbing body. */

/* The indexing gets REEAAALLY confusing here. iPert = 0 to iGravPerts-1
   correspond to all perturbing planets, iPert = iGravPerts corresponds to the
   stellar torque, and
   iPert = iGravPerts+1 to the stellar general relativistic correction, if
   applied */

void InitializeXoblDistRot(BODY *body, UPDATE *update, int iBody, int iPert) {
  update[iBody]
        .iaType[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] = 2;
  update[iBody].padDXoblDtDistRot[iPert] =
        &update[iBody].daDerivProc[update[iBody].iXobl]
                                  [update[iBody].iaXoblDistRot[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iXobl]
                                       [update[iBody].iaXoblDistRot[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]][1] =
        body[iBody].iaGravPerts[iPert];
}

void InitializeYoblDistRot(BODY *body, UPDATE *update, int iBody, int iPert) {
  update[iBody]
        .iaType[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] = 2;
  update[iBody].padDYoblDtDistRot[iPert] =
        &update[iBody].daDerivProc[update[iBody].iYobl]
                                  [update[iBody].iaYoblDistRot[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iYobl]
                                       [update[iBody].iaYoblDistRot[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]][1] =
        body[iBody].iaGravPerts[iPert];
}

void InitializeZoblDistRot(BODY *body, UPDATE *update, int iBody, int iPert) {
  update[iBody]
        .iaType[update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]] = 2;
  update[iBody].padDZoblDtDistRot[iPert] =
        &update[iBody].daDerivProc[update[iBody].iZobl]
                                  [update[iBody].iaZoblDistRot[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iZobl]
                                       [update[iBody].iaZoblDistRot[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iZobl][update[iBody].iaZoblDistRot[iPert]][1] =
        body[iBody].iaGravPerts[iPert];
}

void InitializeXoblDistRotStar(BODY *body, UPDATE *update, int iBody,
                               int iPert) {
  update[iBody]
        .iaType[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] = 2;
  update[iBody].padDXoblDtDistRot[iPert] =
        &update[iBody].daDerivProc[update[iBody].iXobl]
                                  [update[iBody].iaXoblDistRot[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iXobl]
                                       [update[iBody].iaXoblDistRot[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iXobl][update[iBody].iaXoblDistRot[iPert]][1] = 0;
}

void InitializeYoblDistRotStar(BODY *body, UPDATE *update, int iBody,
                               int iPert) {
  update[iBody]
        .iaType[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] = 2;
  update[iBody].padDYoblDtDistRot[iPert] =
        &update[iBody].daDerivProc[update[iBody].iYobl]
                                  [update[iBody].iaYoblDistRot[iPert]];
  update[iBody]
        .iNumBodies[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] =
        2;
  update[iBody]
        .iaBody[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]] =
        malloc(update[iBody].iNumBodies[update[iBody].iYobl]
                                       [update[iBody].iaYoblDistRot[iPert]] *
               sizeof(int));
  update[iBody]
        .iaBody[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]][0] =
        iBody;
  update[iBody]
        .iaBody[update[iBody].iYobl][update[iBody].iaYoblDistRot[iPert]][1] = 0;
}

void VerifyOrbitData(BODY *body, CONTROL *control, OPTIONS *options,
                     int iBody) {
  int iNLines, iLine, c, iNumColsFound, bFoo, iNumCols = 7;
  double dttmp, datmp, detmp, ditmp, daptmp, dlatmp, dmatmp;
  FILE *fileorb;
  char cLine[LINE], cFoo[MAXARRAY][OPTLEN];

  if (body[iBody].bReadOrbitData) {
    if (options[OPT_FILEORBITDATA].iLine[iBody + 1] == -1) {
      fprintf(stderr, "ERROR: Must set %s if using %s for file %s\n",
              options[OPT_FILEORBITDATA].cName,
              options[OPT_READORBITDATA].cName, body[iBody].cName);
      exit(EXIT_INPUT);
    } else {
      fileorb = fopen(body[iBody].cFileOrbitData, "r");
      if (fileorb == NULL) {
        printf("ERROR: File %s not found.\n", body[iBody].cFileOrbitData);
        exit(EXIT_INPUT);
      }
      // Check file has exactly 7 columns
      fgets(cLine, LINE, fileorb);
      GetWords(cLine, cFoo, &iNumColsFound, &bFoo);
      if (iNumCols != iNumColsFound) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr,
                  "ERROR: Incorrect number of columns (%d) in %s file %s. "
                  "Must be exactly %d.\n",
                  iNumColsFound, options[OPT_READORBITDATA].cName,
                  body[iBody].cFileOrbitData, iNumCols);
        }
        exit(EXIT_INPUT);
      }


      iNLines = 1;
      while ((c = getc(fileorb)) != EOF) {
        if (c == '\n') {
          iNLines++; // add 1 for each new line
        }
      }
      rewind(fileorb);

      body[iBody].iNLines       = iNLines;
      body[iBody].daTimeSeries  = malloc(iNLines * sizeof(double));
      body[iBody].daSemiSeries  = malloc(iNLines * sizeof(double));
      body[iBody].daEccSeries   = malloc(iNLines * sizeof(double));
      body[iBody].daIncSeries   = malloc(iNLines * sizeof(double));
      body[iBody].daArgPSeries  = malloc(iNLines * sizeof(double));
      body[iBody].daLongASeries = malloc(iNLines * sizeof(double));
      body[iBody].daMeanASeries = malloc(iNLines * sizeof(double));
      body[iBody].daHeccSeries  = malloc(iNLines * sizeof(double));
      body[iBody].daKeccSeries  = malloc(iNLines * sizeof(double));
      body[iBody].daPincSeries  = malloc(iNLines * sizeof(double));
      body[iBody].daQincSeries  = malloc(iNLines * sizeof(double));

      iLine = 0;
      while (feof(fileorb) == 0) {
        fscanf(fileorb, "%lf %lf %lf %lf %lf %lf %lf\n", &dttmp, &datmp, &detmp,
               &ditmp, &daptmp, &dlatmp, &dmatmp);
        body[iBody].daTimeSeries[iLine] =
              dttmp * fdUnitsTime(control->Units[iBody + 1].iTime);
        body[iBody].daSemiSeries[iLine] =
              datmp * fdUnitsLength(control->Units[iBody + 1].iLength);
        body[iBody].daEccSeries[iLine] = detmp;

        if (control->Units[iBody + 1].iAngle == 0) {
          body[iBody].daIncSeries[iLine]   = ditmp;
          body[iBody].daArgPSeries[iLine]  = daptmp;
          body[iBody].daLongASeries[iLine] = dlatmp;
          body[iBody].daMeanASeries[iLine] = dmatmp;
        } else {
          body[iBody].daIncSeries[iLine]   = ditmp * DEGRAD;
          body[iBody].daArgPSeries[iLine]  = daptmp * DEGRAD;
          body[iBody].daLongASeries[iLine] = dlatmp * DEGRAD;
          body[iBody].daMeanASeries[iLine] = dmatmp * DEGRAD;
        }

        body[iBody].daHeccSeries[iLine] = body[iBody].daEccSeries[iLine] *
                                          sin(body[iBody].daArgPSeries[iLine] +
                                              body[iBody].daLongASeries[iLine]);
        body[iBody].daKeccSeries[iLine] = body[iBody].daEccSeries[iLine] *
                                          cos(body[iBody].daArgPSeries[iLine] +
                                              body[iBody].daLongASeries[iLine]);
        body[iBody].daPincSeries[iLine] =
              sin(0.5 * body[iBody].daIncSeries[iLine]) *
              sin(body[iBody].daLongASeries[iLine]);
        body[iBody].daQincSeries[iLine] =
              sin(0.5 * body[iBody].daIncSeries[iLine]) *
              cos(body[iBody].daLongASeries[iLine]);

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
                "ERROR: Time step size (%s = %lf) must match orbital data "
                "output time "
                "(%lf) if %s = 1\n",
                options[OPT_TIMESTEP].cName, control->Evolve.dTimeStep,
                body[iBody].daTimeSeries[1], options[OPT_READORBITDATA].cName);
        exit(EXIT_INPUT);
      }
    } else if (control->Evolve.bDoBackward) {
      if (body[iBody].daTimeSeries[1] != -1 * control->Evolve.dTimeStep) {
        fprintf(stderr,
                "ERROR: Time step size (%s = %lf) must match orbital data "
                "output time "
                "(%lf) if %s = 1\n",
                options[OPT_TIMESTEP].cName, control->Evolve.dTimeStep,
                body[iBody].daTimeSeries[1], options[OPT_READORBITDATA].cName);
        exit(EXIT_INPUT);
      }
    }
    if (iNLines < (control->Evolve.dStopTime / control->Evolve.dTimeStep + 1)) {
      fprintf(stderr,
              "ERROR: Final time in %s is less than %s; simulation cannot be "
              "completed.\n",
              options[OPT_FILEORBITDATA].cName, options[OPT_STOPTIME].cName);
      exit(EXIT_INPUT); // Should really be a DoubleLineExit
    }
  }
}

void AssignDistRotDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                              fnUpdateVariable ***fnUpdate, int iBody) {
  int i, j = 0, iPert = 0, jBody = 0;

  /* The indexing gets REEAAALLY confusing here. iPert = 0 to iGravPerts-1
   * correspond to all perturbing planets, iPert = iGravPerts corresponds to the
   * stellar torque, and iPert = iGravPerts+1 to the stellar general
   * relativistic correction, if applied */

  if (iBody >= 1) {
    if (body[iBody].bReadOrbitData) {
      fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblDistRot[0]] =
            &fndDistRotExtDxDt;

      fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblDistRot[0]] =
            &fndDistRotExtDyDt;

      fnUpdate[iBody][update[iBody].iZobl][update[iBody].iaZoblDistRot[0]] =
            &fndDistRotExtDzDt;
    } else {
      if (evolve->iDistOrbModel == RD4) {
        /* Body updates */
        for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
          /* x = sin(obl)*cos(pA) */
          fnUpdate[iBody][update[iBody].iXobl]
                  [update[iBody].iaXoblDistRot[iPert]] = &fndDistRotRD4DxDt;

          /* y = sin(obl)*sin(pA) */
          fnUpdate[iBody][update[iBody].iYobl]
                  [update[iBody].iaYoblDistRot[iPert]] = &fndDistRotRD4DyDt;

          /* z = cos(obl) */
          fnUpdate[iBody][update[iBody].iZobl]
                  [update[iBody].iaZoblDistRot[iPert]] = &fndDistRotRD4DzDt;
        }
        /* Body updates for stellar torque, treating star as "perturber" (only
         * needed for x and y -> pA) */
        /* x = sin(obl)*cos(pA) */
        fnUpdate[iBody][update[iBody].iXobl]
                [update[iBody].iaXoblDistRot[body[iBody].iGravPerts]] =
                      &fndDistRotRD4DxDt;

        /* y = sin(obl)*sin(pA) */
        fnUpdate[iBody][update[iBody].iYobl]
                [update[iBody].iaYoblDistRot[body[iBody].iGravPerts]] =
                      &fndDistRotRD4DyDt;

      } else if (evolve->iDistOrbModel == LL2) {
        /* Body updates */
        for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
          /* x = sin(obl)*cos(pA) */
          fnUpdate[iBody][update[iBody].iXobl]
                  [update[iBody].iaXoblDistRot[iPert]] = &fndDistRotLL2DxDt;

          /* y = sin(obl)*sin(pA) */
          fnUpdate[iBody][update[iBody].iYobl]
                  [update[iBody].iaYoblDistRot[iPert]] = &fndDistRotLL2DyDt;

          /* z = cos(obl) */
          fnUpdate[iBody][update[iBody].iZobl]
                  [update[iBody].iaZoblDistRot[iPert]] = &fndDistRotLL2DzDt;
        }
        /* Body updates for stellar torque, treating star as "perturber" (only
         * needed for x and y -> pA) */
        /* x = sin(obl)*cos(pA) */
        fnUpdate[iBody][update[iBody].iXobl]
                [update[iBody].iaXoblDistRot[body[iBody].iGravPerts]] =
                      &fndDistRotLL2DxDt;

        /* y = sin(obl)*sin(pA) */
        fnUpdate[iBody][update[iBody].iYobl]
                [update[iBody].iaYoblDistRot[body[iBody].iGravPerts]] =
                      &fndDistRotLL2DyDt;
      }
    }
    if (body[iBody].bGRCorr) {
      fnUpdate[iBody][update[iBody].iXobl]
              [update[iBody].iaXoblDistRot[body[iBody].iGravPerts + 1]] =
                    &fndAxialGRDxDt;

      fnUpdate[iBody][update[iBody].iYobl]
              [update[iBody].iaYoblDistRot[body[iBody].iGravPerts + 1]] =
                    &fndAxialGRDyDt;
    }
  }
}

void NullDistRotDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                            fnUpdateVariable ***fnUpdate, int iBody) {
  int i, j = 0, iPert = 0, jBody = 0;

  /* The indexing gets REEAAALLY confusing here. iPert = 0 to iGravPerts-1
   * correspond to all perturbing planets, iPert = iGravPerts corresponds to the
   * stellar torque, and iPert = iGravPerts+1 to the stellar general
   * relativistic correction, if applied */

  if (iBody >= 1) {
    if (body[iBody].bReadOrbitData) {
      fnUpdate[iBody][update[iBody].iXobl][update[iBody].iaXoblDistRot[0]] =
            &fndUpdateFunctionTiny;

      fnUpdate[iBody][update[iBody].iYobl][update[iBody].iaYoblDistRot[0]] =
            &fndUpdateFunctionTiny;

      fnUpdate[iBody][update[iBody].iZobl][update[iBody].iaZoblDistRot[0]] =
            &fndUpdateFunctionTiny;
    } else {
      if (evolve->iDistOrbModel == RD4) {
        /* Body updates */
        for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
          /* x = sin(obl)*cos(pA) */
          fnUpdate[iBody][update[iBody].iXobl]
                  [update[iBody].iaXoblDistRot[iPert]] = &fndUpdateFunctionTiny;

          /* y = sin(obl)*sin(pA) */
          fnUpdate[iBody][update[iBody].iYobl]
                  [update[iBody].iaYoblDistRot[iPert]] = &fndUpdateFunctionTiny;

          /* z = cos(obl) */
          fnUpdate[iBody][update[iBody].iZobl]
                  [update[iBody].iaZoblDistRot[iPert]] = &fndUpdateFunctionTiny;
        }
        /* Body updates for stellar torque, treating star as "perturber" (only
         * needed for x and y -> pA) */
        /* x = sin(obl)*cos(pA) */
        fnUpdate[iBody][update[iBody].iXobl]
                [update[iBody].iaXoblDistRot[body[iBody].iGravPerts]] =
                      &fndUpdateFunctionTiny;

        /* y = sin(obl)*sin(pA) */
        fnUpdate[iBody][update[iBody].iYobl]
                [update[iBody].iaYoblDistRot[body[iBody].iGravPerts]] =
                      &fndUpdateFunctionTiny;

      } else if (evolve->iDistOrbModel == LL2) {
        /* Body updates */
        for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
          /* x = sin(obl)*cos(pA) */
          fnUpdate[iBody][update[iBody].iXobl]
                  [update[iBody].iaXoblDistRot[iPert]] = &fndUpdateFunctionTiny;

          /* y = sin(obl)*sin(pA) */
          fnUpdate[iBody][update[iBody].iYobl]
                  [update[iBody].iaYoblDistRot[iPert]] = &fndUpdateFunctionTiny;

          /* z = cos(obl) */
          fnUpdate[iBody][update[iBody].iZobl]
                  [update[iBody].iaZoblDistRot[iPert]] = &fndUpdateFunctionTiny;
        }
        /* Body updates for stellar torque, treating star as "perturber" (only
         * needed for x and y -> pA) */
        /* x = sin(obl)*cos(pA) */
        fnUpdate[iBody][update[iBody].iXobl]
                [update[iBody].iaXoblDistRot[body[iBody].iGravPerts]] =
                      &fndUpdateFunctionTiny;

        /* y = sin(obl)*sin(pA) */
        fnUpdate[iBody][update[iBody].iYobl]
                [update[iBody].iaYoblDistRot[body[iBody].iGravPerts]] =
                      &fndUpdateFunctionTiny;
      }
    }
    if (body[iBody].bGRCorr) {
      fnUpdate[iBody][update[iBody].iXobl]
              [update[iBody].iaXoblDistRot[body[iBody].iGravPerts + 1]] =
                    &fndUpdateFunctionTiny;

      fnUpdate[iBody][update[iBody].iYobl]
              [update[iBody].iaYoblDistRot[body[iBody].iGravPerts + 1]] =
                    &fndUpdateFunctionTiny;
    }
  }
}

void VerifyDistRot(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   OUTPUT *output, SYSTEM *system, UPDATE *update, int iBody,
                   int iModule) {
  int i, j = 0, iPert = 0, jBody = 0;

  VerifyOrbitData(body, control, options, iBody);

  /* The indexing gets REEAAALLY confusing here. iPert = 0 to iGravPerts-1
   * correspond to all perturbing planets, iPert = iGravPerts corresponds to the
   * stellar torque, and iPert = iGravPerts+1 to the stellar general
   * relativistic correction, if applied */

  if (iBody >= 1) {
    control->fnPropsAux[iBody][iModule] = &PropsAuxDistRot;
    VerifyDynEllip(body, control, options, files->Infile[iBody + 1].cIn, iBody,
                   control->Io.iVerbose);

    if (body[iBody].bReadOrbitData) {
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(
              stderr,
              "INFO: When reading in using %s to calculate "
              "rotational evolution, Cassini parameters may not be correct.\n",
              options[OPT_READORBITDATA].cName);
      }
      system->daLOrb        = malloc(3 * sizeof(double));
      body[iBody].daLOrb    = malloc(3 * sizeof(double));
      body[iBody].daLOrbTmp = malloc(3 * sizeof(double));
    }

    CalcXYZobl(body, iBody);

    body[iBody].daLRot    = malloc(3 * sizeof(double));
    body[iBody].daLRotTmp = malloc(3 * sizeof(double));

    if (body[iBody].bReadOrbitData) {
      InitializeXoblDistRot(body, update, iBody, 0);

      InitializeYoblDistRot(body, update, iBody, 0);

      InitializeZoblDistRot(body, update, iBody, 0);

    } else {
      if (control->Evolve.iDistOrbModel == RD4) {
        /* Body updates */
        for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
          /* x = sin(obl)*cos(pA) */
          InitializeXoblDistRot(body, update, iBody, iPert);

          /* y = sin(obl)*sin(pA) */
          InitializeYoblDistRot(body, update, iBody, iPert);

          /* z = cos(obl) */
          InitializeZoblDistRot(body, update, iBody, iPert);
        }
        /* Body updates for stellar torque, treating star as "perturber" (only
         * needed for x and y -> pA) */
        /* x = sin(obl)*cos(pA) */
        InitializeXoblDistRotStar(body, update, iBody, body[iBody].iGravPerts);

        /* y = sin(obl)*sin(pA) */
        InitializeYoblDistRotStar(body, update, iBody, body[iBody].iGravPerts);

      } else if (control->Evolve.iDistOrbModel == LL2) {
        /* Body updates */
        for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
          /* x = sin(obl)*cos(pA) */
          InitializeXoblDistRot(body, update, iBody, iPert);

          /* y = sin(obl)*sin(pA) */
          InitializeYoblDistRot(body, update, iBody, iPert);

          /* z = cos(obl) */
          InitializeZoblDistRot(body, update, iBody, iPert);
        }
        /* Body updates for stellar torque, treating star as "perturber" (only
         * needed for x and y -> pA) */
        /* x = sin(obl)*cos(pA) */
        InitializeXoblDistRotStar(body, update, iBody, body[iBody].iGravPerts);

        /* y = sin(obl)*sin(pA) */
        InitializeYoblDistRotStar(body, update, iBody, body[iBody].iGravPerts);
      }
    }
    if (body[iBody].bGRCorr) {
      InitializeXoblDistRotStar(body, update, iBody,
                                body[iBody].iGravPerts + 1);

      InitializeYoblDistRotStar(body, update, iBody,
                                body[iBody].iGravPerts + 1);
    }
  }

  control->fnForceBehavior[iBody][iModule]   = &ForceBehaviorDistRot;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyDistRot;
}


/***************** DISTROT Update *****************/
void InitializeUpdateDistRot(BODY *body, UPDATE *update, int iBody) {
  if (iBody > 0) {
    if (body[iBody].bReadOrbitData) {
      body[iBody].iGravPerts     = 0;
      body[iBody].iaGravPerts    = malloc(1 * sizeof(int));
      body[iBody].iaGravPerts[0] = 0;
    }

    if (update[iBody].iNumXobl == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumXobl += body[iBody].iGravPerts + 1;

    if (update[iBody].iNumYobl == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumYobl += body[iBody].iGravPerts + 1;

    if (update[iBody].iNumZobl == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumZobl += body[iBody].iGravPerts;

    if (body[iBody].bGRCorr) {
      update[iBody].iNumXobl += 1;
      update[iBody].iNumYobl += 1;
    }
    if (body[iBody].bReadOrbitData) {
      update[iBody].iNumZobl += 1;
    }
  }
}

void FinalizeUpdateXoblDistRot(BODY *body, UPDATE *update, int *iEqn, int iVar,
                               int iBody, int iFoo) {
  int iPert;

  /* The indexing gets REEAAALLY confusing here. iPert = 0 to iGravPerts-1
   * correspond to all perturbing planets, iPert = iGravPerts corresponds to the
   * stellar torque, and iPert = iGravPerts+1 to the stellar general
   * relativistic correction, if applied */

  if (body[iBody].bGRCorr) {
    update[iBody].padDXoblDtDistRot =
          malloc((body[iBody].iGravPerts + 2) * sizeof(double *));
    update[iBody].iaXoblDistRot =
          malloc((body[iBody].iGravPerts + 2) * sizeof(int));
    for (iPert = 0; iPert < body[iBody].iGravPerts + 2; iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTROT;
      update[iBody].iaXoblDistRot[iPert]  = (*iEqn)++;
    }
  } else {
    update[iBody].padDXoblDtDistRot =
          malloc((body[iBody].iGravPerts + 1) * sizeof(double *));
    update[iBody].iaXoblDistRot =
          malloc((body[iBody].iGravPerts + 1) * sizeof(int));
    for (iPert = 0; iPert < body[iBody].iGravPerts + 1; iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTROT;
      update[iBody].iaXoblDistRot[iPert]  = (*iEqn)++;
    }
  }
}

void FinalizeUpdateYoblDistRot(BODY *body, UPDATE *update, int *iEqn, int iVar,
                               int iBody, int iFoo) {
  int iPert;

  /* The indexing gets REEAAALLY confusing here. iPert = 0 to iGravPerts-1
   * correspond to all perturbing planets, iPert = iGravPerts corresponds to the
   * stellar torque, and iPert = iGravPerts+1 to the stellar general
   * relativistic correction, if applied */

  if (body[iBody].bGRCorr) {
    update[iBody].padDYoblDtDistRot =
          malloc((body[iBody].iGravPerts + 2) * sizeof(double *));
    update[iBody].iaYoblDistRot =
          malloc((body[iBody].iGravPerts + 2) * sizeof(int));
    for (iPert = 0; iPert < body[iBody].iGravPerts + 2; iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTROT;
      update[iBody].iaYoblDistRot[iPert]  = (*iEqn)++;
    }
  } else {
    update[iBody].padDYoblDtDistRot =
          malloc((body[iBody].iGravPerts + 1) * sizeof(double *));
    update[iBody].iaYoblDistRot =
          malloc((body[iBody].iGravPerts + 1) * sizeof(int));
    for (iPert = 0; iPert < body[iBody].iGravPerts + 1; iPert++) {
      update[iBody].iaModule[iVar][*iEqn] = DISTROT;
      update[iBody].iaYoblDistRot[iPert]  = (*iEqn)++;
    }
  }
}

void FinalizeUpdateZoblDistRot(BODY *body, UPDATE *update, int *iEqn, int iVar,
                               int iBody, int iFoo) {
  int iPert;

  update[iBody].padDZoblDtDistRot =
        malloc((body[iBody].iGravPerts) * sizeof(double *));
  update[iBody].iaZoblDistRot = malloc((body[iBody].iGravPerts) * sizeof(int));
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    update[iBody].iaModule[iVar][*iEqn] = DISTROT;
    update[iBody].iaZoblDistRot[iPert]  = (*iEqn)++;
  }
}


/***************** DISTROT Halts *****************/

void CountHaltsDistRot(HALT *halt, int *iNumHalts) {
}

void VerifyHaltDistRot(BODY *body, CONTROL *control, OPTIONS *options,
                       int iBody, int *iHalt) {
}


/************* DISTROT Outputs ******************/

void WriteBodyDOblDtDistRot(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  double dDeriv, dObldx, dObldy, dObldz;
  int iPert;

  if ((body[iBody].dXobl * body[iBody].dXobl +
       body[iBody].dYobl * body[iBody].dYobl) > 0) {
    dObldx = body[iBody].dXobl * body[iBody].dZobl /
             (sqrt(body[iBody].dXobl * body[iBody].dXobl +
                   body[iBody].dYobl * body[iBody].dYobl) *
              (body[iBody].dXobl * body[iBody].dXobl +
               body[iBody].dYobl * body[iBody].dYobl +
               body[iBody].dZobl * body[iBody].dZobl));
    dObldy = body[iBody].dYobl * body[iBody].dZobl /
             (sqrt(body[iBody].dXobl * body[iBody].dXobl +
                   body[iBody].dYobl * body[iBody].dYobl) *
              (body[iBody].dXobl * body[iBody].dXobl +
               body[iBody].dYobl * body[iBody].dYobl +
               body[iBody].dZobl * body[iBody].dZobl));

  } else {
    dObldx = 0.0;
    dObldy = 0.0;
  }
  dObldz = -sqrt(body[iBody].dXobl * body[iBody].dXobl +
                 body[iBody].dYobl * body[iBody].dYobl) /
           (body[iBody].dXobl * body[iBody].dXobl +
            body[iBody].dYobl * body[iBody].dYobl +
            body[iBody].dZobl * body[iBody].dZobl);

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    dDeriv += dObldx * (*(update[iBody].padDXoblDtDistRot[iPert])) +
              dObldy * (*(update[iBody].padDYoblDtDistRot[iPert])) +
              dObldz * (*(update[iBody].padDZoblDtDistRot[iPert]));
  }

  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteOblTimeDistRot(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  double dDeriv, dObldx, dObldy, dObldz;
  int iPert;

  if ((body[iBody].dXobl * body[iBody].dXobl +
       body[iBody].dYobl * body[iBody].dYobl) > 0) {
    dObldx = body[iBody].dXobl * body[iBody].dZobl /
             (sqrt(body[iBody].dXobl * body[iBody].dXobl +
                   body[iBody].dYobl * body[iBody].dYobl) *
              (body[iBody].dXobl * body[iBody].dXobl +
               body[iBody].dYobl * body[iBody].dYobl +
               body[iBody].dZobl * body[iBody].dZobl));
    dObldy = body[iBody].dYobl * body[iBody].dZobl /
             (sqrt(body[iBody].dXobl * body[iBody].dXobl +
                   body[iBody].dYobl * body[iBody].dYobl) *
              (body[iBody].dXobl * body[iBody].dXobl +
               body[iBody].dYobl * body[iBody].dYobl +
               body[iBody].dZobl * body[iBody].dZobl));
  } else {
    dObldx = 0.0;
    dObldy = 0.0;
  }

  dObldz = -sqrt(body[iBody].dXobl * body[iBody].dXobl +
                 body[iBody].dYobl * body[iBody].dYobl) /
           (body[iBody].dXobl * body[iBody].dXobl +
            body[iBody].dYobl * body[iBody].dYobl +
            body[iBody].dZobl * body[iBody].dZobl);

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    dDeriv += dObldx * (*(update[iBody].padDXoblDtDistRot[iPert])) +
              dObldy * (*(update[iBody].padDYoblDtDistRot[iPert])) +
              dObldz * (*(update[iBody].padDZoblDtDistRot[iPert]));
  }

  *dTmp = fabs(PI / dDeriv);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WritePrecATimeDistRot(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  double dDeriv, dpAdx, dpAdy;
  int iPert;

  if ((body[iBody].dXobl * body[iBody].dXobl +
       body[iBody].dYobl * body[iBody].dYobl) != 0) {
    dpAdx = -body[iBody].dYobl / (body[iBody].dXobl * body[iBody].dXobl +
                                  body[iBody].dYobl * body[iBody].dYobl);
    dpAdy = body[iBody].dXobl / (body[iBody].dXobl * body[iBody].dXobl +
                                 body[iBody].dYobl * body[iBody].dYobl);
  } else {
    dpAdx = 0.0;
    dpAdy = 0.0;
  }

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert <= body[iBody].iGravPerts; iPert++) {
    dDeriv += dpAdx * (*(update[iBody].padDXoblDtDistRot[iPert])) +
              dpAdy * (*(update[iBody].padDYoblDtDistRot[iPert]));
  }

  *dTmp = fabs(2 * PI / dDeriv);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
    //     *dTmp /= fdUnitsAngle(units->iAngle);
    //     fsUnitsAngle(units->iAngle,cUnit);
  }
}

void WriteBodyDPrecADtDistRot(BODY *body, CONTROL *control, OUTPUT *output,
                              SYSTEM *system, UNITS *units, UPDATE *update,
                              int iBody, double *dTmp, char cUnit[]) {
  double dDeriv, dpAdx, dpAdy;
  int iPert;

  if ((body[iBody].dXobl * body[iBody].dXobl +
       body[iBody].dYobl * body[iBody].dYobl) != 0) {
    dpAdx = -body[iBody].dYobl / (body[iBody].dXobl * body[iBody].dXobl +
                                  body[iBody].dYobl * body[iBody].dYobl);
    dpAdy = body[iBody].dXobl / (body[iBody].dXobl * body[iBody].dXobl +
                                 body[iBody].dYobl * body[iBody].dYobl);
  } else {
    dpAdx = 0.0;
    dpAdy = 0.0;
  }

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert <= body[iBody].iGravPerts; iPert++) {
    dDeriv += dpAdx * (*(update[iBody].padDXoblDtDistRot[iPert])) +
              dpAdy * (*(update[iBody].padDYoblDtDistRot[iPert]));
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

void WriteBodyDXoblDtDistRot(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert <= body[iBody].iGravPerts; iPert++) {
    dDeriv += *(update[iBody].padDXoblDtDistRot[iPert]);
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

void WriteBodyDYoblDtDistRot(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert <= body[iBody].iGravPerts; iPert++) {
    dDeriv += *(update[iBody].padDYoblDtDistRot[iPert]);
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

void WriteBodyDZoblDtDistRot(BODY *body, CONTROL *control, OUTPUT *output,
                             SYSTEM *system, UNITS *units, UPDATE *update,
                             int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    dDeriv += *(update[iBody].padDZoblDtDistRot[iPert]);
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

void WriteXoblTimeDistRot(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert <= body[iBody].iGravPerts; iPert++) {
    dDeriv += *(update[iBody].padDXoblDtDistRot[iPert]);
  }

  *dTmp = fabs(1. / dDeriv);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteYoblTimeDistRot(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert <= body[iBody].iGravPerts; iPert++) {
    dDeriv += *(update[iBody].padDYoblDtDistRot[iPert]);
  }

  *dTmp = fabs(1. / dDeriv);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteZoblTimeDistRot(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  double dDeriv;
  int iPert;

  /* Ensure that we don't overwrite derivative */
  dDeriv = 0;
  for (iPert = 0; iPert < body[iBody].iGravPerts; iPert++) {
    dDeriv += *(update[iBody].padDZoblDtDistRot[iPert]);
  }

  *dTmp = fabs(1. / dDeriv);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}


void WriteBodyCassOne(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {

  double h, inc, longa, Lnorm = 0.0, obliq, eqnode;
  int i, jBody;

  for (i = 0; i < 3; i++) {
    system->daLOrb[i] = 0.0;
  }
  for (jBody = 1; jBody < control->Evolve.iNumBodies; jBody++) {
    h = body[jBody].dMass / MSUN * KGAUSS *
        sqrt((body[0].dMass + body[jBody].dMass) / MSUN * body[jBody].dSemi /
             AUM *
             (1. - (body[jBody].dHecc * body[jBody].dHecc) -
              (body[jBody].dKecc * body[jBody].dKecc)));

    body[jBody].daLOrb[0] = 0.0;
    body[jBody].daLOrb[1] = 0.0;
    body[jBody].daLOrb[2] = h;

    inc = 2 * asin(sqrt((body[jBody].dPinc * body[jBody].dPinc) +
                        (body[jBody].dQinc * body[jBody].dQinc)));

    // rotate about x by inc angle
    RotateVector(body[jBody].daLOrb, body[jBody].daLOrbTmp, inc, 0);
    longa = atan2(body[jBody].dPinc, body[jBody].dQinc);
    // rotate about z by Omega
    RotateVector(body[jBody].daLOrbTmp, body[jBody].daLOrb, longa, 2);
    for (i = 0; i < 3; i++) {
      system->daLOrb[i] += body[jBody].daLOrb[i];
    }
  }
  Lnorm = sqrt(system->daLOrb[0] * system->daLOrb[0] +
               system->daLOrb[1] * system->daLOrb[1] +
               system->daLOrb[2] * system->daLOrb[2]);
  for (i = 0; i < 3; i++) {
    system->daLOrb[i] /= Lnorm;
  }
  Lnorm = sqrt(body[iBody].daLOrb[0] * body[iBody].daLOrb[0] +
               body[iBody].daLOrb[1] * body[iBody].daLOrb[1] +
               body[iBody].daLOrb[2] * body[iBody].daLOrb[2]);

  for (i = 0; i < 3; i++) {
    body[iBody].daLOrb[i] /= Lnorm;
  }
  body[iBody].daLRot[0] = 0.0;
  body[iBody].daLRot[1] = 0.0;
  body[iBody].daLRot[2] = 1.0;
  obliq                 = atan2(sqrt(body[iBody].dXobl * body[iBody].dXobl +
                                     body[iBody].dYobl * body[iBody].dYobl),
                                body[iBody].dZobl);

  inc   = 2 * asin(sqrt((body[iBody].dPinc * body[iBody].dPinc) +
                        (body[iBody].dQinc * body[iBody].dQinc)));
  longa = atan2(body[iBody].dPinc, body[iBody].dQinc);
  RotateVector(body[iBody].daLRot, body[iBody].daLRotTmp, -obliq, 0);
  eqnode = 2 * PI - atan2(body[iBody].dYobl, body[iBody].dXobl) - longa;
  RotateVector(body[iBody].daLRotTmp, body[iBody].daLRot, eqnode, 2);
  RotateVector(body[iBody].daLRot, body[iBody].daLRotTmp, inc, 0);
  RotateVector(body[iBody].daLRotTmp, body[iBody].daLRot, longa, 2);

  cross(body[iBody].daLRot, body[iBody].daLOrb, body[iBody].daLRotTmp);
  Lnorm = sqrt(body[iBody].daLRotTmp[0] * body[iBody].daLRotTmp[0] +
               body[iBody].daLRotTmp[1] * body[iBody].daLRotTmp[1] +
               body[iBody].daLRotTmp[2] * body[iBody].daLRotTmp[2]);

  // Probably should assign array elements to 0 if Lnorm = 0
  if (Lnorm != 0) {
    for (i = 0; i < 3; i++) {
      body[iBody].daLRotTmp[i] /= Lnorm;
    }
  }

  cross(system->daLOrb, body[iBody].daLOrb, body[iBody].daLOrbTmp);
  Lnorm = sqrt(body[iBody].daLOrbTmp[0] * body[iBody].daLOrbTmp[0] +
               body[iBody].daLOrbTmp[1] * body[iBody].daLOrbTmp[1] +
               body[iBody].daLOrbTmp[2] * body[iBody].daLOrbTmp[2]);
  if (Lnorm != 0) {
    for (i = 0; i < 3; i++) {
      body[iBody].daLOrbTmp[i] /= Lnorm;
    }
  }

  cross(body[iBody].daLOrbTmp, body[iBody].daLRotTmp, system->daLOrb);
  *dTmp = sqrt(system->daLOrb[0] * system->daLOrb[0] +
               system->daLOrb[1] * system->daLOrb[1] +
               system->daLOrb[2] * system->daLOrb[2]);
  if (body[iBody].dInc == 0 && body[iBody].dObliquity == 0) {
    if (control->Io.iVerbose >= VERBPROG &&
        !control->Io.baCassiniOneMessage[iBody]) {
      fprintf(stderr,
              "INFO: The inclination and obliqutiy of %s are both 0,"
              " therefore its %s is defined to be nan.\n",
              body[iBody].cName, output->cName);
      control->Io.baCassiniOneMessage[iBody] = 1;
    }
  }
}

void WriteBodyCassTwo(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {

  double h, inc, longa, Lnorm = 0.0, obliq, eqnode;
  int i, jBody;

  for (i = 0; i < 3; i++) {
    system->daLOrb[i] = 0.0;
  }

  for (jBody = 1; jBody < control->Evolve.iNumBodies; jBody++) {
    h = body[jBody].dMass / MSUN * KGAUSS *
        sqrt((body[0].dMass + body[jBody].dMass) / MSUN * body[jBody].dSemi /
             AUM *
             (1. - (body[jBody].dHecc * body[jBody].dHecc) -
              (body[jBody].dKecc * body[jBody].dKecc)));

    body[jBody].daLOrb[0] = 0.0;
    body[jBody].daLOrb[1] = 0.0;
    body[jBody].daLOrb[2] = h;

    inc = 2 * asin(sqrt((body[jBody].dPinc * body[jBody].dPinc) +
                        (body[jBody].dQinc * body[jBody].dQinc)));

    // rotate about x by inc angle
    RotateVector(body[jBody].daLOrb, body[jBody].daLOrbTmp, inc, 0);
    longa = atan2(body[jBody].dPinc, body[jBody].dQinc);
    // rotate about z by Omega
    RotateVector(body[jBody].daLOrbTmp, body[jBody].daLOrb, longa, 2);
    for (i = 0; i < 3; i++) {
      system->daLOrb[i] += body[jBody].daLOrb[i];
    }
  }
  Lnorm = sqrt(system->daLOrb[0] * system->daLOrb[0] +
               system->daLOrb[1] * system->daLOrb[1] +
               system->daLOrb[2] * system->daLOrb[2]);

  for (i = 0; i < 3; i++) {
    system->daLOrb[i] /= Lnorm;
  }
  Lnorm = sqrt(body[iBody].daLOrb[0] * body[iBody].daLOrb[0] +
               body[iBody].daLOrb[1] * body[iBody].daLOrb[1] +
               body[iBody].daLOrb[2] * body[iBody].daLOrb[2]);

  for (i = 0; i < 3; i++) {
    body[iBody].daLOrb[i] /= Lnorm;
  }
  body[iBody].daLRot[0] = 0.0;
  body[iBody].daLRot[1] = 0.0;
  body[iBody].daLRot[2] = 1.0;
  obliq                 = atan2(sqrt(body[iBody].dXobl * body[iBody].dXobl +
                                     body[iBody].dYobl * body[iBody].dYobl),
                                body[iBody].dZobl);

  inc = 2 * asin(sqrt((body[iBody].dPinc * body[iBody].dPinc) +
                      (body[iBody].dQinc * body[iBody].dQinc)));

  longa = atan2(body[iBody].dPinc, body[iBody].dQinc);
  RotateVector(body[iBody].daLRot, body[iBody].daLRotTmp, -obliq, 0);
  eqnode = 2 * PI - atan2(body[iBody].dYobl, body[iBody].dXobl) - longa;
  RotateVector(body[iBody].daLRotTmp, body[iBody].daLRot, eqnode, 2);
  RotateVector(body[iBody].daLRot, body[iBody].daLRotTmp, inc, 0);
  RotateVector(body[iBody].daLRotTmp, body[iBody].daLRot, longa, 2);

  cross(body[iBody].daLRot, body[iBody].daLOrb, body[iBody].daLRotTmp);
  Lnorm = sqrt(body[iBody].daLRotTmp[0] * body[iBody].daLRotTmp[0] +
               body[iBody].daLRotTmp[1] * body[iBody].daLRotTmp[1] +
               body[iBody].daLRotTmp[2] * body[iBody].daLRotTmp[2]);

  if (Lnorm != 0) {
    for (i = 0; i < 3; i++) {
      body[iBody].daLRotTmp[i] /= Lnorm;
    }
  }

  cross(system->daLOrb, body[iBody].daLOrb, body[iBody].daLOrbTmp);
  Lnorm = sqrt(body[iBody].daLOrbTmp[0] * body[iBody].daLOrbTmp[0] +
               body[iBody].daLOrbTmp[1] * body[iBody].daLOrbTmp[1] +
               body[iBody].daLOrbTmp[2] * body[iBody].daLOrbTmp[2]);

  if (Lnorm != 0) {
    for (i = 0; i < 3; i++) {
      body[iBody].daLOrbTmp[i] /= Lnorm;
    }
  }
  *dTmp = 0.0;
  for (i = 0; i < 3; i++) {
    *dTmp += body[iBody].daLRotTmp[i] * body[iBody].daLOrbTmp[i];
  }
  if (body[iBody].dInc == 0 && body[iBody].dObliquity == 0) {
    if (control->Io.iVerbose >= VERBPROG &&
        !control->Io.baCassiniTwoMessage[iBody]) {
      fprintf(stderr,
              "INFO: The inclination and obliqutiy of %s are both 0,"
              " therefore its %s is defined to be nan.\n",
              body[iBody].cName, output->cName);

      control->Io.baCassiniTwoMessage[iBody] = 1;
    }
  }
}

void WriteDynEllip(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  if (iBody > 0) {
    *dTmp = body[iBody].dDynEllip;
  } else {
    *dTmp = -1;
  }
  sprintf(cUnit, "%s", "");
}

void WritePrecFNat(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {
  *dTmp = fndCentralTorqueR(body, iBody);

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

void InitializeOutputDistRot(OUTPUT *output, fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_DOBLDTDISTROT].cName, "DOblDtDistRot");
  sprintf(output[OUT_DOBLDTDISTROT].cDescr, "Body's dobl/dt in DistRot");
  sprintf(output[OUT_DOBLDTDISTROT].cNeg, "deg/year");
  output[OUT_DOBLDTDISTROT].bNeg       = 1;
  output[OUT_DOBLDTDISTROT].dNeg       = YEARSEC / DEGRAD;
  output[OUT_DOBLDTDISTROT].iNum       = 1;
  output[OUT_DOBLDTDISTROT].iModuleBit = DISTROT;
  fnWrite[OUT_DOBLDTDISTROT]           = &WriteBodyDOblDtDistRot;

  sprintf(output[OUT_DPRECADTDISTROT].cName, "DPrecADtDistRot");
  sprintf(output[OUT_DPRECADTDISTROT].cDescr, "Body's dpA/dt in DistRot");
  sprintf(output[OUT_DPRECADTDISTROT].cNeg, "deg/year");
  output[OUT_DPRECADTDISTROT].bNeg       = 1;
  output[OUT_DPRECADTDISTROT].dNeg       = YEARSEC / DEGRAD;
  output[OUT_DPRECADTDISTROT].iNum       = 1;
  output[OUT_DPRECADTDISTROT].iModuleBit = DISTROT;
  fnWrite[OUT_DPRECADTDISTROT]           = &WriteBodyDPrecADtDistRot;

  sprintf(output[OUT_DXOBLDTDISTROT].cName, "DXoblDtDistRot");
  sprintf(output[OUT_DXOBLDTDISTROT].cDescr, "Body's dXobl/dt in DistRot");
  sprintf(output[OUT_DXOBLDTDISTROT].cNeg, "1/year");
  output[OUT_DXOBLDTDISTROT].bNeg       = 1;
  output[OUT_DXOBLDTDISTROT].dNeg       = YEARSEC;
  output[OUT_DXOBLDTDISTROT].iNum       = 1;
  output[OUT_DXOBLDTDISTROT].iModuleBit = DISTROT;
  fnWrite[OUT_DXOBLDTDISTROT]           = &WriteBodyDXoblDtDistRot;

  sprintf(output[OUT_DYOBLDTDISTROT].cName, "DYoblDtDistRot");
  sprintf(output[OUT_DYOBLDTDISTROT].cDescr, "Body's dYobl/dt in DistRot");
  sprintf(output[OUT_DYOBLDTDISTROT].cNeg, "1/year");
  output[OUT_DYOBLDTDISTROT].bNeg       = 1;
  output[OUT_DYOBLDTDISTROT].dNeg       = YEARSEC;
  output[OUT_DYOBLDTDISTROT].iNum       = 1;
  output[OUT_DYOBLDTDISTROT].iModuleBit = DISTROT;
  fnWrite[OUT_DYOBLDTDISTROT]           = &WriteBodyDYoblDtDistRot;

  sprintf(output[OUT_DZOBLDTDISTROT].cName, "DZoblDtDistRot");
  sprintf(output[OUT_DZOBLDTDISTROT].cDescr, "Body's dZobl/dt in DistRot");
  sprintf(output[OUT_DZOBLDTDISTROT].cNeg, "1/year");
  output[OUT_DZOBLDTDISTROT].bNeg       = 1;
  output[OUT_DZOBLDTDISTROT].dNeg       = YEARSEC;
  output[OUT_DZOBLDTDISTROT].iNum       = 1;
  output[OUT_DZOBLDTDISTROT].iModuleBit = DISTROT;
  fnWrite[OUT_DZOBLDTDISTROT]           = &WriteBodyDZoblDtDistRot;

  sprintf(output[OUT_XOBLTIMEDISTROT].cName, "XoblTimeDistRot");
  sprintf(output[OUT_XOBLTIMEDISTROT].cDescr, "Xobl timescale in DistRot");
  sprintf(output[OUT_XOBLTIMEDISTROT].cNeg, "year");
  output[OUT_XOBLTIMEDISTROT].bNeg       = 1;
  output[OUT_XOBLTIMEDISTROT].dNeg       = 1. / YEARSEC;
  output[OUT_XOBLTIMEDISTROT].iNum       = 1;
  output[OUT_XOBLTIMEDISTROT].iModuleBit = DISTROT;
  fnWrite[OUT_XOBLTIMEDISTROT]           = &WriteXoblTimeDistRot;

  sprintf(output[OUT_YOBLTIMEDISTROT].cName, "YoblTimeDistRot");
  sprintf(output[OUT_YOBLTIMEDISTROT].cDescr, "Yobl timescale in DistRot");
  sprintf(output[OUT_YOBLTIMEDISTROT].cNeg, "year");
  output[OUT_YOBLTIMEDISTROT].bNeg       = 1;
  output[OUT_YOBLTIMEDISTROT].dNeg       = 1. / YEARSEC;
  output[OUT_YOBLTIMEDISTROT].iNum       = 1;
  output[OUT_YOBLTIMEDISTROT].iModuleBit = DISTROT;
  fnWrite[OUT_YOBLTIMEDISTROT]           = &WriteYoblTimeDistRot;

  sprintf(output[OUT_ZOBLTIMEDISTROT].cName, "ZoblTimeDistRot");
  sprintf(output[OUT_ZOBLTIMEDISTROT].cDescr, "Zobl timescale in DistRot");
  sprintf(output[OUT_ZOBLTIMEDISTROT].cNeg, "year");
  output[OUT_ZOBLTIMEDISTROT].bNeg       = 1;
  output[OUT_ZOBLTIMEDISTROT].dNeg       = 1. / YEARSEC;
  output[OUT_ZOBLTIMEDISTROT].iNum       = 1;
  output[OUT_ZOBLTIMEDISTROT].iModuleBit = DISTROT;
  fnWrite[OUT_ZOBLTIMEDISTROT]           = &WriteZoblTimeDistRot;

  sprintf(output[OUT_OBLTIMEDISTROT].cName, "OblTimeDistRot");
  sprintf(output[OUT_OBLTIMEDISTROT].cDescr, "Obliquity timescale in DistRot");
  sprintf(output[OUT_OBLTIMEDISTROT].cNeg, "year");
  output[OUT_OBLTIMEDISTROT].bNeg       = 1;
  output[OUT_OBLTIMEDISTROT].dNeg       = 1. / YEARSEC;
  output[OUT_OBLTIMEDISTROT].iNum       = 1;
  output[OUT_OBLTIMEDISTROT].iModuleBit = DISTROT;
  fnWrite[OUT_OBLTIMEDISTROT]           = &WriteOblTimeDistRot;

  sprintf(output[OUT_PRECATIMEDISTROT].cName, "PrecATimeDistRot");
  sprintf(output[OUT_PRECATIMEDISTROT].cDescr,
          "Axial precession timescale in DistRot");
  sprintf(output[OUT_PRECATIMEDISTROT].cNeg, "year");
  output[OUT_PRECATIMEDISTROT].bNeg       = 1;
  output[OUT_PRECATIMEDISTROT].dNeg       = 1. / YEARSEC;
  output[OUT_PRECATIMEDISTROT].iNum       = 1;
  output[OUT_PRECATIMEDISTROT].iModuleBit = DISTROT;
  fnWrite[OUT_PRECATIMEDISTROT]           = &WritePrecATimeDistRot;

  sprintf(output[OUT_CASS1].cName, "CassiniOne");
  sprintf(output[OUT_CASS1].cDescr,
          "First Cassini parameter (misalignment of Cassini state vectors)");
  output[OUT_CASS1].bNeg       = 0;
  output[OUT_CASS1].iNum       = 1;
  output[OUT_CASS1].iModuleBit = DISTROT;
  fnWrite[OUT_CASS1]           = &WriteBodyCassOne;

  sprintf(output[OUT_CASS2].cName, "CassiniTwo");
  sprintf(output[OUT_CASS2].cDescr,
          "Second Cassini parameter (alignment of Cassini state vectors");
  output[OUT_CASS2].bNeg       = 0;
  output[OUT_CASS2].iNum       = 1;
  output[OUT_CASS2].iModuleBit = DISTROT;
  fnWrite[OUT_CASS2]           = &WriteBodyCassTwo;

  sprintf(output[OUT_DYNELLIP].cName, "DynEllip");
  sprintf(output[OUT_DYNELLIP].cDescr, "dynamical ellipticity of planet");
  output[OUT_DYNELLIP].bNeg       = 0;
  output[OUT_DYNELLIP].iNum       = 1;
  output[OUT_DYNELLIP].iModuleBit = DISTROT;
  fnWrite[OUT_DYNELLIP]           = &WriteDynEllip;

  sprintf(output[OUT_PRECFNAT].cName, "PrecFNat");
  sprintf(output[OUT_PRECFNAT].cDescr, "natural precession freq of planet");
  output[OUT_PRECFNAT].bNeg       = 0;
  output[OUT_PRECFNAT].iNum       = 1;
  output[OUT_PRECFNAT].iModuleBit = DISTROT;
  fnWrite[OUT_PRECFNAT]           = &WritePrecFNat;
}

/************ DISTROT Logging Functions **************/

void LogOptionsDistRot(CONTROL *control, FILE *fp) {

  fprintf(fp, "-------- DISTROT Options -----\n\n");
}

void LogDistRot(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {
  int iOut;

  fprintf(fp, "\n----- DISTROT PARAMETERS ------\n");
  for (iOut = OUTSTARTDISTROT; iOut < OUTBODYSTARTDISTROT; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, 0);
    }
  }
}

void LogBodyDistRot(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UPDATE *update, fnWriteOutput fnWrite[],
                    FILE *fp, int iBody) {
  int iOut;

  fprintf(fp, "-----DISTROT PARAMETERS (%s)------\n", body[iBody].cName);
  for (iOut = OUTBODYSTARTDISTROT; iOut < OUTENDDISTROT; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
  }
}

/************* MODULE Functions ***********/

void AddModuleDistRot(CONTROL *control, MODULE *module, int iBody,
                      int iModule) {

  module->iaModule[iBody][iModule] = DISTROT;

  module->fnInitializeUpdateTmpBody[iBody][iModule] =
        &InitializeUpdateTmpBodyDistRot;
  module->fnCountHalts[iBody][iModule] = &CountHaltsDistRot;
  module->fnLogBody[iBody][iModule]    = &LogBodyDistRot;

  module->fnReadOptions[iBody][iModule]       = &ReadOptionsDistRot;
  module->fnVerify[iBody][iModule]            = &VerifyDistRot;
  module->fnAssignDerivatives[iBody][iModule] = &AssignDistRotDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullDistRotDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltDistRot;

  module->fnInitializeUpdate[iBody][iModule]   = &InitializeUpdateDistRot;
  module->fnInitializeOutput[iBody][iModule]   = &InitializeOutputDistRot;
  module->fnFinalizeUpdateXobl[iBody][iModule] = &FinalizeUpdateXoblDistRot;
  module->fnFinalizeUpdateYobl[iBody][iModule] = &FinalizeUpdateYoblDistRot;
  module->fnFinalizeUpdateZobl[iBody][iModule] = &FinalizeUpdateZoblDistRot;
}

/************* DISTROT Functions ***********/

void UpdateOrbitData(BODY *body, EVOLVE *evolve, int iBody) {
  body[iBody].dSemi = body[iBody].daSemiSeries[body[iBody].iCurrentStep];
  body[iBody].dHecc = body[iBody].daHeccSeries[body[iBody].iCurrentStep];
  body[iBody].dKecc = body[iBody].daKeccSeries[body[iBody].iCurrentStep];
  body[iBody].dPinc = body[iBody].daPincSeries[body[iBody].iCurrentStep];
  body[iBody].dQinc = body[iBody].daQincSeries[body[iBody].iCurrentStep];
  body[iBody].dEcc  = sqrt(body[iBody].dHecc * body[iBody].dHecc +
                           body[iBody].dKecc * body[iBody].dKecc);

  /* numerical derivatives of p and q */
  if (body[iBody].iCurrentStep == 0) {
    body[iBody].dPdot =
          (body[iBody].daPincSeries[body[iBody].iCurrentStep + 1] -
           body[iBody].daPincSeries[body[iBody].iCurrentStep]) /
          evolve->dTimeStep;
    body[iBody].dQdot =
          (body[iBody].daQincSeries[body[iBody].iCurrentStep + 1] -
           body[iBody].daQincSeries[body[iBody].iCurrentStep]) /
          evolve->dTimeStep;
  } else {
    body[iBody].dPdot =
          (body[iBody].daPincSeries[body[iBody].iCurrentStep + 1] -
           body[iBody].daPincSeries[body[iBody].iCurrentStep - 1]) /
          (2 * evolve->dTimeStep);
    body[iBody].dQdot =
          (body[iBody].daQincSeries[body[iBody].iCurrentStep + 1] -
           body[iBody].daQincSeries[body[iBody].iCurrentStep - 1]) /
          (2 * evolve->dTimeStep);
  }
}

void PropsAuxDistRot(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                     int iBody) {
  // if (body[iBody].bForcePrecRate) {
  //     body[iBody].dObliquity =
  //     atan2(sqrt(body[iBody].dXobl*body[iBody].dXobl+body[iBody].dYobl*body[iBody].dYobl),body[iBody].dZobl);
  //     body[iBody].dPrecA = atan2(body[iBody].dYobl,body[iBody].dXobl);
  //   }


  if (body[iBody].bReadOrbitData) {
    UpdateOrbitData(body, evolve, iBody);
  }

  if ((fabs(body[iBody].dXobl) > 1) || fabs(body[iBody].dYobl) > 1 ||
      fabs(body[iBody].dZobl) > 1) {
    if (io->iVerbose >= VERBERR) {
      fprintf(stderr, "ERROR: %s's obliquity out of bounds at %.2e years.\n",
              body[iBody].cName, evolve->dTime / YEARSEC);
      fprintf(stderr, "\tXobl: %.6e\n", body[iBody].dXobl);
      fprintf(stderr, "\tYobl: %.6e\n", body[iBody].dYobl);
      fprintf(stderr, "\tZobl: %.6e\n", body[iBody].dZobl);
    }
    exit(EXIT_INT);
  }

  body[iBody].dObliquity = atan2(sqrt(body[iBody].dXobl * body[iBody].dXobl +
                                      body[iBody].dYobl * body[iBody].dYobl),
                                 body[iBody].dZobl);
}

void ForceBehaviorDistRot(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                          SYSTEM *system, UPDATE *update,
                          fnUpdateVariable ***fnUpdate, int iBody,
                          int iModule) {
  if (body[iBody].bReadOrbitData) {
    body[iBody].iCurrentStep++;
  }
}

void RotateVector(double *v1, double *v2, double theta, int axis) {
  if (axis == 0) {
    v2[0] = v1[0];
    v2[1] = cos(theta) * v1[1] - sin(theta) * v1[2];
    v2[2] = sin(theta) * v1[1] + cos(theta) * v1[2];
  } else if (axis == 1) {
    v2[0] = cos(theta) * v1[0] + sin(theta) * v1[2];
    v2[1] = v1[1];
    v2[2] = -sin(theta) * v1[0] + cos(theta) * v1[2];
  } else if (axis == 2) {
    v2[0] = cos(theta) * v1[0] - sin(theta) * v1[1];
    v2[1] = sin(theta) * v1[0] + cos(theta) * v1[1];
    v2[2] = v1[2];
  }
}

/* Equations used to calculate obliquity/spin evolution */

/**
Correction to axial precession rate for eccentricity (and possible additional
effects)
@param body Struct containing all body information and variables
@param iBody Body in question
@return Correction to precession rate
*/
double fndCentralTorqueSfac(BODY *body, int iBody) {
  return 0.5 * pow(1. - (body[iBody].dHecc * body[iBody].dHecc) -
                         (body[iBody].dKecc * body[iBody].dKecc),
                   -1.5);
}

/**
Natural axial precession rate due to host star (alpha*cos(obliquity))
@param body Struct containing all body information and variables
@param iBody Body in question
@return Axial precession rate (rad/sec)
*/
double fndCentralTorqueR(BODY *body, int iBody) {
  double obliq, tmp;

  return 3 * (KGAUSS * KGAUSS) * body[0].dMass / MSUN /
         ((body[iBody].dSemi / AUM * body[iBody].dSemi / AUM *
           body[iBody].dSemi / AUM) *
          body[iBody].dRotRate * DAYSEC) *
         body[iBody].dDynEllip * fndCentralTorqueSfac(body, iBody) *
         body[iBody].dZobl / DAYSEC;
}

/**
C(p,q) function in obliquity evol equations if RD4 orbital model is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return C(p,q) function
*/
double fndObliquityCRD4(BODY *body, SYSTEM *system, int *iaBody) {
  double dObliquityCRD4, dDistOrbRD4DpDt, dDistOrbRD4DqDt;

  dDistOrbRD4DpDt = fndDistOrbRD4DpDt(body, system, iaBody);
  dDistOrbRD4DqDt = fndDistOrbRD4DqDt(body, system, iaBody);

  dObliquityCRD4 = body[iaBody[0]].dQinc * dDistOrbRD4DpDt -
                   body[iaBody[0]].dPinc * dDistOrbRD4DqDt;

  return dObliquityCRD4;
}

/**
A(p,q) function in obliquity evol equations if RD4 orbital model is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return A(p,q) function
*/
double fndObliquityARD4(BODY *body, SYSTEM *system, int *iaBody) {
  double dObliquityARD4, dDistOrbRD4DqDt, dObliquityCRD4;

  dDistOrbRD4DqDt = fndDistOrbRD4DqDt(body, system, iaBody);
  dObliquityCRD4  = fndObliquityCRD4(body, system, iaBody);

  dObliquityARD4 = 2.0 /
                   sqrt(1 - (body[iaBody[0]].dPinc * body[iaBody[0]].dPinc) -
                        (body[iaBody[0]].dQinc * body[iaBody[0]].dQinc)) *
                   (dDistOrbRD4DqDt + body[iaBody[0]].dPinc * dObliquityCRD4);

  return dObliquityARD4;
}

/**
B(p,q) function in obliquity evol equations if RD4 orbital model is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return B(p,q) function
*/
double fndObliquityBRD4(BODY *body, SYSTEM *system, int *iaBody) {
  double dDistOrbRD4DpDt, dObliquityCRD4, dObliquityBRD4;

  dDistOrbRD4DpDt = fndDistOrbRD4DpDt(body, system, iaBody);
  dObliquityCRD4  = fndObliquityCRD4(body, system, iaBody);

  dObliquityBRD4 = 2.0 /
                   sqrt(1 - (body[iaBody[0]].dPinc * body[iaBody[0]].dPinc) -
                        (body[iaBody[0]].dQinc * body[iaBody[0]].dQinc)) *
                   (dDistOrbRD4DpDt - body[iaBody[0]].dQinc * dObliquityCRD4);

  return dObliquityBRD4;
}

/**
C(p,q) function in obliquity evol equations if LL2 orbital model is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return C(p,q) function
*/
double fndObliquityCLL2(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dQinc * fndDistOrbLL2DpDt(body, system, iaBody) -
         body[iaBody[0]].dPinc * fndDistOrbLL2DqDt(body, system, iaBody);
}

/**
A(p,q) function in obliquity evol equations if LL2 orbital model is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return A(p,q) function
*/
double fndObliquityALL2(BODY *body, SYSTEM *system, int *iaBody) {
  return 2.0 /
         sqrt(1 - (body[iaBody[0]].dPinc * body[iaBody[0]].dPinc) -
              (body[iaBody[0]].dQinc * body[iaBody[0]].dQinc)) *
         (fndDistOrbLL2DqDt(body, system, iaBody) +
          body[iaBody[0]].dPinc * fndObliquityCLL2(body, system, iaBody));
}

/**
B(p,q) function in obliquity evol equations if LL2 orbital model is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return B(p,q) function
*/
double fndObliquityBLL2(BODY *body, SYSTEM *system, int *iaBody) {
  return 2.0 /
         sqrt(1 - (body[iaBody[0]].dPinc * body[iaBody[0]].dPinc) -
              (body[iaBody[0]].dQinc * body[iaBody[0]].dQinc)) *
         (fndDistOrbLL2DpDt(body, system, iaBody) -
          body[iaBody[0]].dQinc * fndObliquityCLL2(body, system, iaBody));
}

/**
C(p,q) function in obliquity evol equations if external orbital data is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return C(p,q) function
*/
double fndObliquityCExt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dQinc * body[iaBody[0]].dPdot -
         body[iaBody[0]].dPinc * body[iaBody[0]].dQdot;
}

/**
A(p,q) function in obliquity evol equations if external orbital data is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return A(p,q) function
*/
double fndObliquityAExt(BODY *body, SYSTEM *system, int *iaBody) {
  return 2.0 /
         sqrt(1 - (body[iaBody[0]].dPinc * body[iaBody[0]].dPinc) -
              (body[iaBody[0]].dQinc * body[iaBody[0]].dQinc)) *
         (body[iaBody[0]].dQdot +
          body[iaBody[0]].dPinc * fndObliquityCExt(body, system, iaBody));
}

/**
B(p,q) function in obliquity evol equations if external orbital data is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return B(p,q) function
*/
double fndObliquityBExt(BODY *body, SYSTEM *system, int *iaBody) {
  return 2.0 /
         sqrt(1 - (body[iaBody[0]].dPinc * body[iaBody[0]].dPinc) -
              (body[iaBody[0]].dQinc * body[iaBody[0]].dQinc)) *
         (body[iaBody[0]].dPdot -
          body[iaBody[0]].dQinc * fndObliquityCExt(body, system, iaBody));
}

//----------Relativistic correction-------------------------------------

/**
GR correction to axial precession rate
@param body Struct containing all body information and variables
@param iaBody Array containing indices of bodies associated with interaction
@return Correction to axial precession rate d(PrecA)/dt
*/
double fndAxialGRCorrection(BODY *body, int *iaBody) {
  return fndApsidalGRCorrection(body, iaBody) / 2.;
}

/**
GR correction to derivative of x = sin(obliquity)*cos(preca)
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Correction to derivative dx/dt
*/
double fndAxialGRDxDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dYobl * fndAxialGRCorrection(body, iaBody);
}

/**
GR correction to derivative of y = sin(obliquity)*sin(preca)
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Correction to derivative dy/dt
*/
double fndAxialGRDyDt(BODY *body, SYSTEM *system, int *iaBody) {
  return -body[iaBody[0]].dXobl * fndAxialGRCorrection(body, iaBody);
}

//--------------Obliquity/spin
// evolution--------------------------------------------------------------

/**
Derivative of y = sin(obliquity)*sin(preca) when RD4 orbital model is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dy/dt
*/
double fndDistRotRD4DyDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;

  if (iaBody[1] == 0) {
    if (body[iaBody[0]].bForcePrecRate == 0) {
      return body[iaBody[0]].dXobl * fndCentralTorqueR(body, iaBody[0]);
    } else {
      return body[iaBody[0]].dXobl * body[iaBody[0]].dPrecRate;
    }
  } else if (iaBody[1] >= 1) {
    y = fabs(1.0 - (body[iaBody[0]].dXobl * body[iaBody[0]].dXobl) -
             (body[iaBody[0]].dYobl * body[iaBody[0]].dYobl));
    return -fndObliquityBRD4(body, system, iaBody) * sqrt(y) -
           body[iaBody[0]].dXobl * 2. * fndObliquityCRD4(body, system, iaBody);
  }
  assert(0);
  return 0;
}

/**
Derivative of x = sin(obliquity)*cos(preca) when RD4 orbital model is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dx/dt
*/
double fndDistRotRD4DxDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;

  if (iaBody[1] == 0) {
    if (body[iaBody[0]].bForcePrecRate == 0) {
      return -body[iaBody[0]].dYobl * fndCentralTorqueR(body, iaBody[0]);
    } else {
      return -body[iaBody[0]].dYobl * body[iaBody[0]].dPrecRate;
    }
  } else if (iaBody[1] >= 1) {
    y = fabs(1.0 - (body[iaBody[0]].dXobl * body[iaBody[0]].dXobl) -
             (body[iaBody[0]].dYobl * body[iaBody[0]].dYobl));
    return fndObliquityARD4(body, system, iaBody) * sqrt(y) +
           body[iaBody[0]].dYobl * 2. * fndObliquityCRD4(body, system, iaBody);
  }
  assert(0);
  return 0;
}

/**
Derivative of z = cos(obliquity) when RD4 orbital model is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dz/dt
*/
double fndDistRotRD4DzDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dObliquityBRD4, dObliquityARD4, dDistRotRD4DzDt;

  dObliquityBRD4 = fndObliquityBRD4(body, system, iaBody);
  dObliquityARD4 = fndObliquityARD4(body, system, iaBody);

  dDistRotRD4DzDt = body[iaBody[0]].dYobl * dObliquityBRD4 -
                    body[iaBody[0]].dXobl * dObliquityARD4;

  return dDistRotRD4DzDt;
}

/**
Derivative of y = sin(obliquity)*sin(preca) when LL2 orbital model is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dy/dt
*/
double fndDistRotLL2DyDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;

  if (iaBody[1] == 0) {
    return body[iaBody[0]].dXobl * fndCentralTorqueR(body, iaBody[0]);
  } else if (iaBody[1] >= 1) {
    y = fabs(1.0 - (body[iaBody[0]].dXobl * body[iaBody[0]].dXobl) -
             (body[iaBody[0]].dYobl * body[iaBody[0]].dYobl));
    return -fndObliquityBLL2(body, system, iaBody) * sqrt(y) -
           body[iaBody[0]].dXobl * 2. * fndObliquityCLL2(body, system, iaBody);
  }
  assert(0);
  return 0;
}

/**
Derivative of x = sin(obliquity)*cos(preca) when LL2 orbital model is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dx/dt
*/
double fndDistRotLL2DxDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;

  if (iaBody[1] == 0) {
    return -body[iaBody[0]].dYobl * fndCentralTorqueR(body, iaBody[0]);
  } else if (iaBody[1] >= 1) {
    y = fabs(1.0 - (body[iaBody[0]].dXobl * body[iaBody[0]].dXobl) -
             (body[iaBody[0]].dYobl * body[iaBody[0]].dYobl));
    return fndObliquityALL2(body, system, iaBody) * sqrt(y) +
           body[iaBody[0]].dYobl * 2. * fndObliquityCLL2(body, system, iaBody);
  }
  assert(0);
  return 0;
}

/**
Derivative of z = cos(obliquity) when LL2 orbital model is used
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dz/dt
*/
double fndDistRotLL2DzDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dYobl * fndObliquityBLL2(body, system, iaBody) -
         body[iaBody[0]].dXobl * fndObliquityALL2(body, system, iaBody);
}

/**
Derivative of dynamical ellipticity when coupled to eqtide and thermint
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative d(dynellip)/dt
*/
double fndDistRotDDynEllipDt(BODY *body, SYSTEM *system, int *iaBody) {
  return -EDMAN * EDMAN / body[iaBody[0]].dViscUMan *
         (body[iaBody[0]].dDynEllip - CalcDynEllipEq(body, iaBody[0]));
}

/**
Derivative of x = sin(obliquity)*cos(preca) when orbital data is input from
external model
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dx/dt
*/
double fndDistRotExtDxDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;
  y = fabs(1.0 - (body[iaBody[0]].dXobl * body[iaBody[0]].dXobl) -
           (body[iaBody[0]].dYobl * body[iaBody[0]].dYobl));

  return fndObliquityAExt(body, system, iaBody) * sqrt(y) +
         body[iaBody[0]].dYobl * 2. * fndObliquityCExt(body, system, iaBody) -
         body[iaBody[0]].dYobl * fndCentralTorqueR(body, iaBody[0]);
}

/**
Derivative of y = sin(obliquity)*sin(preca) when orbital data is input from
external model
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dy/dt
*/
double fndDistRotExtDyDt(BODY *body, SYSTEM *system, int *iaBody) {
  double y;
  y = fabs(1.0 - (body[iaBody[0]].dXobl * body[iaBody[0]].dXobl) -
           (body[iaBody[0]].dYobl * body[iaBody[0]].dYobl));

  return -fndObliquityBExt(body, system, iaBody) * sqrt(y) -
         body[iaBody[0]].dXobl * 2. * fndObliquityCExt(body, system, iaBody) +
         body[iaBody[0]].dXobl * fndCentralTorqueR(body, iaBody[0]);
}

/**
Derivative of z = cos(obliquity) when orbital data is input from external model
@param body Struct containing all body information and variables
@param system Struct containing system information
@param iaBody Array containing indices of bodies associated with interaction
@return Derivative dz/dt
*/
double fndDistRotExtDzDt(BODY *body, SYSTEM *system, int *iaBody) {
  return body[iaBody[0]].dYobl * fndObliquityBExt(body, system, iaBody) -
         body[iaBody[0]].dXobl * fndObliquityAExt(body, system, iaBody);
}
