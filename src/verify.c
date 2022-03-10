/**
  @file verify.c
  @brief Validate the options. This will become a mess!
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date May 7 2014
*/

#include "vplanet.h"

/*
 * Utility functions
 */

/*! Check to see if two decimals numbers are equal (1) or not (0) */
int bFloatComparison(double x, double y) {
  double dBigger;
  double dRel_Tol;
  if (fabs(x) > fabs(y)) {
    dBigger = fabs(x);
  } else {
    dBigger = fabs(y);
  }

  dRel_Tol = 5 * dBigger * DBL_EPSILON;

  if (dRel_Tol <= 10 * dTINY) {
    dRel_Tol = 10 * dTINY;
  }

  if (fabs(x - y) <= dRel_Tol) {
    return 1;
  } else {
    return 0;
  }
}

/*
 * Exit Calls
 */

int bFileExists(const char *filename) {
  FILE *file;
  if ((file = fopen(filename, "r"))) {
    fclose(file);
    return 1;
  }
  return 0;
}

void OverwriteExit(char cName[], char cFile[]) {
  fprintf(stderr, "ERROR: %s is false and %s exists.\n", cName, cFile);
  fprintf(stderr, "\tOveride with \"-f\" on the command line.\n");
  exit(EXIT_INPUT);
}

/* XXX Should these be iLine+1? */
void DoubleLineExit(char cFile1[], char cFile2[], int iLine1, int iLine2) {
  fprintf(stderr, "\tFile: %s, Line: %d.\n", cFile1, iLine1 + 1);
  fprintf(stderr, "\tFile: %s, Line: %d.\n", cFile2, iLine2 + 1);
  exit(EXIT_INPUT);
}

void VerifyOrbitExit(char cName1[], char cName2[], char cFile1[], char cFile2[],
                     int iLine1, int iLine2, int iVerbose) {
  if (iVerbose >= VERBERR) {
    fprintf(stderr, "ERROR: Cannot set both %s and %s.\n", cName1, cName2);
    fprintf(stderr, "\tFile: %s, Line: %d.\n", cFile1, iLine1);
    fprintf(stderr, "\tFile: %s, Line: %d.\n", cFile2, iLine2);
  }
  exit(EXIT_INPUT);
}

void VerifyBodyExit(char cName1[], char cName2[], char cFile[], int iLine1,
                    int iLine2, int iVerbose) {

  if (iVerbose >= VERBERR) {
    fprintf(stderr, "ERROR: Cannot set both %s and %s in same file.\n", cName1,
            cName2);
    fprintf(stderr, "\tFile: %s, Lines: %d and %d\n", cFile, iLine1, iLine2);
  }
  exit(EXIT_INPUT);
}

/** Print three lines that are in conflict
    Only called if iVerbose >= VERBERR
*/

void TripleLineExit(char cFile[], int iLine1, int iLine2, int iLine3) {
  fprintf(stderr, "\tFile: %s, Lines: %d, %d and %d.\n", cFile, iLine1, iLine2,
          iLine3);
  exit(EXIT_INPUT);
}

/* Do we need both these? */
void VerifyTripleExit(char cName1[], char cName2[], char cName3[], int iLine1,
                      int iLine2, int iLine3, char cFile[], int iVerbose) {
  if (iVerbose >= VERBERR) {
    fprintf(stderr, "ERROR: Cannot set %s, %s, and %s simultaneously.\n",
            cName1, cName2, cName3);
    TripleLineExit(cFile, iLine1, iLine2, iLine3);
  }
}

void VerifyTwoOfThreeExit(char cName1[], char cName2[], char cName3[],
                          int iLine1, int iLine2, int iLine3, char cFile[],
                          int iVerbose) {
  if (iVerbose >= VERBERR) {
    fprintf(stderr, "ERROR: Can only set 2 of %s, %s, and %s.\n", cName1,
            cName2, cName3);
  }
  TripleLineExit(cFile, iLine1, iLine2, iLine3);
}

void VerifyDynEllip(BODY *body, CONTROL *control, OPTIONS *options,
                    char cFile[], int iBody, int iVerbose) {
  if (body[iBody].bCalcDynEllip == 1) {
    /* check if bCalcDynEllip and dDynEllip are both set */
    if (options[OPT_DYNELLIP].iLine[iBody + 1] > -1) {
      if (iVerbose >= VERBINPUT) {
        fprintf(stderr,
                "INFO: %s set in file %s, but %s set to 1. %s will be "
                "overridden.\n",
                options[OPT_DYNELLIP].cName, cFile,
                options[OPT_CALCDYNELLIP].cName, options[OPT_DYNELLIP].cName);
      }
    }
    body[iBody].dDynEllip = CalcDynEllipEq(body, iBody);
  }
}

/**
  Verify the user did not use the same name for two bodies.
  */

void VerifyNames(BODY *body, CONTROL *control, OPTIONS *options) {
  int iBody, jBody;

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    if (strlen(body[iBody].cName) == 0) {
      if (control->Io.iVerbose > VERBINPUT) {
        fprintf(stderr, "INFO: No input to %s in file %s, defaulting to %d/\n",
                options[OPT_BODYNAME].cName,
                options[OPT_BODYNAME].cFile[iBody] + 1, iBody);
      }
      sprintf(body[iBody].cName, "%d", iBody + 1);
    }
    for (jBody = iBody + 1; jBody < control->Evolve.iNumBodies; jBody++) {
      if (strcmp(body[iBody].cName, body[jBody].cName) == 0) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: Two bodies have the same name.\n");
          DoubleLineExit(options[OPT_BODYNAME].cFile[iBody + 1],
                         options[OPT_BODYNAME].cFile[jBody + 1],
                         options[OPT_BODYNAME].iLine[iBody + 1],
                         options[OPT_BODYNAME].iLine[jBody + 1]);
        }
      }
    }
  }
}

/*
 *
 * Verify Orbit
 *
 */

void VerifyOrbit(BODY *body, CONTROL *control, FILES files, OPTIONS *options,
                 int iBody) {
  int iFile    = iBody + 1, iVerbose;
  double dSemi = 0, dMeanMotion = 0, dPeriod = 0;

  // Body 0 is never an orbiter
  if (iBody == 0) {
    return;
  }

  iVerbose = control->Io.iVerbose;

  /* !!!!! ------ Semi IS ALWAYS CORRECT AND IN BODY[iBody] ------- !!!!!! */

  if (options[OPT_ORBSEMI].iLine[iFile] > -1 &&
      options[OPT_ORBMEANMOTION].iLine[iFile] == -1 &&
      options[OPT_ORBPER].iLine[iFile] == -1) {
    dSemi = body[iBody].dSemi;
  }

  if (options[OPT_ORBSEMI].iLine[iFile] == -1 &&
      options[OPT_ORBMEANMOTION].iLine[iFile] > -1 &&
      options[OPT_ORBPER].iLine[iFile] == -1) {
    dMeanMotion = body[iBody].dMeanMotion;
  }

  if (options[OPT_ORBSEMI].iLine[iFile] == -1 &&
      options[OPT_ORBMEANMOTION].iLine[iFile] == -1 &&
      options[OPT_ORBPER].iLine[iFile] > -1) {
    dPeriod = body[iBody].dOrbPeriod;
  }

  /* Was Semi set and nothing else? */
  if (dSemi > 0 && dMeanMotion == 0 && dPeriod == 0) {
    if (body[iBody].bPoise) {
      if (body[iBody].bBinary == 0) { // Not binary, regular single-star orbit
        body[iBody].dMeanMotion = fdSemiToMeanMotion(
              body[iBody].dSemi, body[0].dMass + body[iBody].dMass);
      } else if (body[iBody].bBinary &&
                 body[iBody].iBodyType ==
                       0) { // Set mean motion for CBP
                            // (primary,iBody==0;planet,iBody > 1)
        body[iBody].dMeanMotion = fdSemiToMeanMotion(
              body[iBody].dSemi,
              body[0].dMass + body[1].dMass + body[iBody].dMass);
      } else if (body[iBody].bBinary && body[iBody].iBodyType == 1 &&
                 iBody == 1) { // Set mean motion for binary, info in secondary
        body[iBody].dMeanMotion = fdSemiToMeanMotion(
              body[iBody].dSemi, body[0].dMass + body[iBody].dMass);
      }
    } else if (body[iBody].bPoise == 0) {
      if (body[iBody].bBinary != 1) {
        if (iBody > 0) // No binary, regular planet
        {
          body[iBody].dMeanMotion = fdSemiToMeanMotion(
                body[iBody].dSemi, body[0].dMass + body[iBody].dMass);
        }
      } else // Doing binary
      {
        if (body[iBody].iBodyType == 0) // CBP
        {
          body[iBody].dMeanMotion = fdSemiToMeanMotion(
                body[iBody].dSemi,
                body[0].dMass + body[1].dMass + body[iBody].dMass);
        } else if (body[iBody].iBodyType == 1 &&
                   iBody == 1) // binary orbit info in secondary
        {
          body[iBody].dMeanMotion = fdSemiToMeanMotion(
                body[iBody].dSemi, body[0].dMass + body[1].dMass);
        }
      }
    }

    return;
  }

  /* Was anything set? */
  if (dSemi == 0 && dMeanMotion == 0 && dPeriod == 0) {
    fprintf(stderr, "ERROR: Must set one of %s, %s or %s.\n",
            options[OPT_ORBSEMI].cName, options[OPT_ORBMEANMOTION].cName,
            options[OPT_ORBPER].cName);
    exit(EXIT_INPUT);
  }

  /* If Semi set, was anything else? */
  if (dSemi > 0) {
    if (dMeanMotion > 0) {
      VerifyOrbitExit(options[OPT_ORBSEMI].cName,
                      options[OPT_ORBMEANMOTION].cName, files.Infile[iFile].cIn,
                      files.Infile[iFile].cIn,
                      options[OPT_ORBSEMI].iLine[iFile],
                      options[OPT_ORBMEANMOTION].iLine[iFile], iVerbose);
    }

    if (dPeriod > 0) {
      VerifyOrbitExit(options[OPT_ORBSEMI].cName, options[OPT_ORBPER].cName,
                      files.Infile[iFile].cIn, files.Infile[iFile].cIn,
                      options[OPT_ORBSEMI].iLine[iFile],
                      options[OPT_ORBPER].iLine[iFile], iVerbose);
    }
  }

  /* Were MeanMotion and OrbPeriod both set? */
  if (dPeriod > 0 && dMeanMotion > 0) {
    VerifyOrbitExit(options[OPT_ORBMEANMOTION].cName, options[OPT_ORBPER].cName,
                    files.Infile[iFile].cIn, files.Infile[iFile].cIn,
                    options[OPT_ORBMEANMOTION].iLine[iFile],
                    options[OPT_ORBPER].iLine[iFile], iVerbose);
  }

  /* Only one option set */

  if (dMeanMotion > 0) {
    if (body[iBody].bBinary && body[iBody].iBodyType == 0) { // CBP
      body[iBody].dSemi = fdMeanMotionToSemi(body[0].dMass + body[1].dMass,
                                             body[iBody].dMass, dMeanMotion);
    } else if (body[iBody].bBinary && iBody == 1) { // binary
      body[iBody].dSemi =
            fdMeanMotionToSemi(body[0].dMass, body[1].dMass, dMeanMotion);
    } else if (body[iBody].bBinary && body[iBody].iBodyType == 1 &&
               iBody == 0) {
    } else {
      body[iBody].dSemi =
            fdMeanMotionToSemi(body[0].dMass, body[iBody].dMass, dMeanMotion);
    }
  }
  if (dPeriod > 0) {
    if (body[iBody].bBinary && body[iBody].iBodyType == 0) { // CBP
      body[iBody].dSemi = fdPeriodToSemi(
            dPeriod, (body[0].dMass + body[1].dMass + body[iBody].dMass));
    } else if (body[iBody].bBinary && body[iBody].iBodyType == 1 &&
               iBody == 1) { // binary
      body[iBody].dSemi =
            fdPeriodToSemi(dPeriod, (body[0].dMass + body[iBody].dMass));
    } else if (body[iBody].bBinary && body[iBody].iBodyType == 1 &&
               iBody == 0) {
    } else {
      body[iBody].dSemi =
            fdPeriodToSemi(dPeriod, (body[0].dMass + body[iBody].dMass));
    }
  }
  if (dSemi > 0) {
    if (body[iBody].bBinary && body[iBody].iBodyType == 0) { // CBP
      body[iBody].dSemi = dSemi;
    } else if (body[iBody].bBinary && body[iBody].iBodyType == 1 &&
               iBody == 1) { // binary
      body[iBody].dSemi = dSemi;
    } else if (body[iBody].bBinary && body[iBody].iBodyType == 1 &&
               iBody == 0) {
    } else {
      body[iBody].dSemi = dSemi;
    }
  }
  if (dMeanMotion == 0) {
    if (body[iBody].bBinary && body[iBody].iBodyType == 0) { // CBP
      body[iBody].dMeanMotion = fdSemiToMeanMotion(
            body[iBody].dSemi,
            body[0].dMass + body[1].dMass + body[iBody].dMass);
    } else if (body[iBody].bBinary && body[iBody].iBodyType == 1 &&
               iBody == 1) { // Primary
      body[iBody].dMeanMotion = fdSemiToMeanMotion(
            body[iBody].dSemi, body[0].dMass + body[1].dMass);
    } else if (body[iBody].bBinary && body[iBody].iBodyType == 1 &&
               iBody == 0) {
    } else {
      body[iBody].dMeanMotion = fdSemiToMeanMotion(
            body[iBody].dSemi, body[0].dMass + body[iBody].dMass);
    }
  }
}

/*
 *
 *   Verify Integration
 *
 */

void IntegrationWarning(char cName1[], char cName2[], char cName3[],
                        char cFile[], int iLine, int iVerbose) {
  if (iVerbose > VERBINPUT) {
    fprintf(stderr,
            "WARNING: %s set, but neither %s nor %s set (%s: Line %d).\n",
            cName1, cName2, cName3, cFile, iLine);
  }
  /* Backward file name */
}

void VerifyIntegration(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system,
                       fnIntegrate *fnOneStep) {
  int iFile, iFile1, iFile2;
  char cTmp[OPTLEN];


  // Initialize iDir to 0, i.e. assume no integrations requested to start
  control->Evolve.iDir = 0;

  /* Were both Forward and Backward Set? */
  if (control->Evolve.bDoBackward && control->Evolve.bDoForward) {
    fprintf(stderr, "ERROR: Both %s and %s set. Only one is allowed.\n",
            options[OPT_BACK].cName, options[OPT_FORW].cName);
    /* Now identify which files contained the two offending options */
    for (iFile = 0; iFile < files->iNumInputs; iFile++) {
      if (options[OPT_BACK].iLine[iFile] > 0) {
        iFile1 = iFile; // Error of multiple occurences checked in Read
      }
      if (options[OPT_FORW].iLine[iFile] > 0) {
        iFile2 = iFile; // Error of multiple occurences checked in Read
      }
    }
    DoubleLineExit(
          options[OPT_BACK].cFile[iFile1], options[OPT_FORW].cFile[iFile2],
          options[OPT_BACK].iLine[iFile1], options[OPT_FORW].iLine[iFile2]);
  }

  /* Fix backward output file */
  if (control->Evolve.bDoBackward) {
    for (iFile = 1; iFile < files->iNumInputs; iFile++) {
      if (options[OPT_OUTFILE].iLine[iFile] == -1) {
        sprintf(files->Outfile[iFile - 1].cOut, "%s.%s.backward", system->cName,
                body[iFile - 1].cName);
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr, "INFO: %s not set, defaulting to %s.\n",
                  options[OPT_OUTFILE].cName, files->Outfile[iFile - 1].cOut);
        }
      }
    }
    control->Evolve.iDir = -1;
  }

  /* Fix forward output file */
  if (control->Evolve.bDoForward) {
    for (iFile = 1; iFile < files->iNumInputs; iFile++) {
      if (options[OPT_OUTFILE].iLine[iFile] == -1) {
        sprintf(files->Outfile[iFile - 1].cOut, "%s.%s.forward", system->cName,
                body[iFile - 1].cName);
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr, "INFO: %s not set, defaulting to %s.\n",
                  options[OPT_OUTFILE].cName, files->Outfile[iFile - 1].cOut);
        }
      }
    }
    control->Evolve.iDir = 1;
  }

  /* Check for file existence */
  for (iFile = 0; iFile < files->iNumInputs - 1; iFile++) {
    if (bFileExists(files->Outfile[iFile].cOut)) {
      if (!control->Io.bOverwrite) {
        OverwriteExit(options[OPT_OVERWRITE].cName, files->Outfile[iFile].cOut);
      }
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr, "WARNING: %s exists.\n", files->Outfile[iFile].cOut);
      }
      unlink(files->Outfile[iFile].cOut);
    }
  }

  /* Was DoBackward or DoForward NOT set? */
  if (!control->Evolve.bDoBackward && !control->Evolve.bDoForward) {
    for (iFile = 0; iFile < files->iNumInputs; iFile++) {
      if (options[OPT_ETA].iLine[iFile] > -1) {
        IntegrationWarning(options[OPT_ETA].cName, options[OPT_BACK].cName,
                           options[OPT_FORW].cName,
                           options[OPT_ETA].cFile[iFile],
                           options[OPT_ETA].iLine[iFile], control->Io.iVerbose);
      }

      if (options[OPT_OUTPUTTIME].iLine[iFile] > -1) {
        IntegrationWarning(
              options[OPT_OUTPUTTIME].cName, options[OPT_BACK].cName,
              options[OPT_FORW].cName, options[OPT_OUTPUTTIME].cFile[iFile],
              options[OPT_OUTPUTTIME].iLine[iFile], control->Io.iVerbose);
      }

      if (options[OPT_STOPTIME].iLine[iFile] > -1) {
        IntegrationWarning(
              options[OPT_STOPTIME].cName, options[OPT_BACK].cName,
              options[OPT_FORW].cName, options[OPT_STOPTIME].cFile[iFile],
              options[OPT_STOPTIME].iLine[iFile], control->Io.iVerbose);
      }

      if (options[OPT_TIMESTEP].iLine[iFile] > -1) {
        IntegrationWarning(
              options[OPT_TIMESTEP].cName, options[OPT_BACK].cName,
              options[OPT_FORW].cName, options[OPT_TIMESTEP].cFile[iFile],
              options[OPT_TIMESTEP].iLine[iFile], control->Io.iVerbose);
      }

      if (options[OPT_VARDT].iLine[iFile] > -1) {
        IntegrationWarning(
              options[OPT_VARDT].cName, options[OPT_BACK].cName,
              options[OPT_FORW].cName, options[OPT_VARDT].cFile[iFile],
              options[OPT_VARDT].iLine[iFile], control->Io.iVerbose);
      }

      if (options[OPT_OUTPUTORDER].iLine[iFile] > -1) {
        IntegrationWarning(
              options[OPT_OUTPUTORDER].cName, options[OPT_BACK].cName,
              options[OPT_FORW].cName, options[OPT_OUTPUTORDER].cFile[iFile],
              options[OPT_OUTPUTORDER].iLine[iFile], control->Io.iVerbose);
      }
    }
  }

  if (control->Evolve.iOneStep == EULER) {
    *fnOneStep = &EulerStep;
  } else if (control->Evolve.iOneStep == RUNGEKUTTA) {
    *fnOneStep = &RungeKutta4Step;
  } else {
    /* Assign Default */
    strcpy(cTmp, options[OPT_INTEGRATIONMETHOD].cDefault);
    if (control->Io.iVerbose >= VERBINPUT) {
      fprintf(stderr, "INFO: %s not set, defaulting to %s.\n",
              options[OPT_INTEGRATIONMETHOD].cName,
              options[OPT_INTEGRATIONMETHOD].cDefault);
    }
    if (memcmp(sLower(cTmp), "e", 1) == 0) {
      control->Evolve.iOneStep = EULER;
      *fnOneStep               = &EulerStep;
    }
    if (memcmp(sLower(cTmp), "r", 1) == 0) {
      control->Evolve.iOneStep = RUNGEKUTTA;
      *fnOneStep               = &RungeKutta4Step;
    }
  }

  /* Make sure output interval is less than stop time */
  if (control->Evolve.dStopTime < control->Io.dOutputTime) {
    fprintf(stderr, "ERROR: %s < %s is not allowed.\n",
            options[OPT_STOPTIME].cName, options[OPT_OUTPUTTIME].cName);
    /* Now identify which files contained the two offending options */
    for (iFile = 0; iFile < files->iNumInputs; iFile++) {
      if (options[OPT_STOPTIME].iLine[iFile] > 0) {
        iFile1 = iFile; // Error of multiple occurences checked in Read
      }
      if (options[OPT_OUTPUTTIME].iLine[iFile] > 0) {
        iFile2 = iFile; // Error of multiple occurences checked in Read
      }
    }
    DoubleLineExit(options[OPT_STOPTIME].cFile[iFile1],
                   options[OPT_STOPTIME].cFile[iFile2],
                   options[OPT_STOPTIME].iLine[iFile1],
                   options[OPT_OUTPUTTIME].iLine[iFile2]);
  }
}

/*
 *
 * Mass-Radius Relationships
 *
 */

void VerifyMassRad(BODY *body, CONTROL *control, OPTIONS *options, char cFile[],
                   int iBody) {
  int iFile = iBody + 1, iVerbose;

  iVerbose = control->Io.iVerbose;

  /* !!!!!! --- Mass and Radius ARE ALWAYS UPDATED AND CORRECT --- !!!!!! */

  /* First see if mass and radius and nothing else set, i.e. the user input the
   * default parameters */
  if (options[OPT_MASS].iLine[iFile] > -1 &&
      options[OPT_RADIUS].iLine[iFile] > -1 &&
      options[OPT_DENSITY].iLine[iFile] == -1 &&
      options[OPT_MASSRAD].iLine[iFile] == -1) {
    return;
  }

  /* Was anything set> */
  if (options[OPT_MASS].iLine[iFile] == -1 &&
      options[OPT_RADIUS].iLine[iFile] == -1 &&
      options[OPT_DENSITY].iLine[iFile] == -1) {
    if (iVerbose >= VERBERR) {
      fprintf(stderr, "ERROR: Must set at least one of %s, %s, and %s.\n",
              options[OPT_MASS].cName, options[OPT_RADIUS].cName,
              options[OPT_DENSITY].cName);
    }
    exit(EXIT_INPUT);
  }

  /* Were all set? */
  if (options[OPT_MASS].iLine[iFile] > -1 &&
      options[OPT_RADIUS].iLine[iFile] > -1 &&
      options[OPT_DENSITY].iLine[iFile] > -1) {
    VerifyTripleExit(options[OPT_MASS].cName, options[OPT_RADIUS].cName,
                     options[OPT_DENSITY].cName, options[OPT_MASS].iLine[iFile],
                     options[OPT_RADIUS].iLine[iFile],
                     options[OPT_DENSITY].iLine[iFile], cFile, iVerbose);
    exit(EXIT_INPUT);
  }

  /* Was mass set? */
  if (options[OPT_MASS].iLine[iFile] > -1) {

    /* Can only set 1 other */
    if (options[OPT_RADIUS].iLine[iFile] > -1 &&
        options[OPT_MASSRAD].iLine[iFile] > -1) {
      VerifyTwoOfThreeExit(options[OPT_MASS].cName, options[OPT_RADIUS].cName,
                           options[OPT_MASSRAD].cName,
                           options[OPT_MASS].iLine[iFile],
                           options[OPT_RADIUS].iLine[iFile],
                           options[OPT_MASSRAD].iLine[iFile], cFile, iVerbose);
    }

    if (options[OPT_RADIUS].iLine[iFile] > -1 &&
        options[OPT_DENSITY].iLine[iFile] > -1) {
      VerifyTwoOfThreeExit(options[OPT_MASS].cName, options[OPT_RADIUS].cName,
                           options[OPT_DENSITY].cName,
                           options[OPT_MASS].iLine[iFile],
                           options[OPT_RADIUS].iLine[iFile],
                           options[OPT_DENSITY].iLine[iFile], cFile, iVerbose);
    }

    if (options[OPT_MASSRAD].iLine[iFile] > -1 &&
        options[OPT_DENSITY].iLine[iFile] > -1) {
      VerifyTwoOfThreeExit(options[OPT_MASS].cName, options[OPT_MASSRAD].cName,
                           options[OPT_DENSITY].cName,
                           options[OPT_MASS].iLine[iFile],
                           options[OPT_MASSRAD].iLine[iFile],
                           options[OPT_DENSITY].iLine[iFile], cFile, iVerbose);
    }

    /* Only Mass and something else set */

    if (options[OPT_RADIUS].iLine[iFile] > -1) {
      /* Mass and radius were the only two set - Nothing to do */
      return;
    }
    if (options[OPT_DENSITY].iLine[iFile] > -1) {
      /* Must get radius from density */
      body->dRadius = fdDensityMassToRadius(body->dDensity, body->dMass);
    }
    if (options[OPT_MASSRAD].iLine[iFile] > -1) {
      /* Must get radius from relationship */
      body->dRadius = fdMassToRad(body->dMass, control->iMassRad[iBody]);
    }

    return;
  }

  /* Was radius set, but not mass? */
  if (options[OPT_RADIUS].iLine[iFile] > -1) {
    if (options[OPT_MASSRAD].iLine[iFile] >= -1 &&
        options[OPT_DENSITY].iLine[iFile] >= -1) {
      VerifyTwoOfThreeExit(options[OPT_MASS].cName, options[OPT_MASSRAD].cName,
                           options[OPT_DENSITY].cName,
                           options[OPT_MASS].iLine[iFile],
                           options[OPT_MASSRAD].iLine[iFile],
                           options[OPT_DENSITY].iLine[iFile], cFile, iVerbose);
    }

    /* Only Radius and something else set */

    if (options[OPT_MASSRAD].iLine[iFile] > -1) {
      /* Must get mass from relationship */
      body->dMass = fdRadToMass(body->dRadius, control->iMassRad[iBody]);
    }
    if (options[OPT_DENSITY].iLine[iFile] > -1) {
      /* Must get mass from density */
      body->dMass = fdMassFromRadiusDensity(body->dRadius, body->dDensity);
    }
  }
}

/*
 *
 * Verify Rotation
 *
 */

void VerifyObliquity(BODY *body, OPTIONS *options, int iBody, int iVerbose) {

  if (options[OPT_COSOBL].iLine[iBody + 1] > -1) {
    // Cos(obl) entered for this body
    if (options[OPT_OBL].iLine[iBody + 1] > -1) {
      if (iVerbose > VERBINPUT) {
        fprintf(stderr, "ERROR: Cannot set %s and %s.\n",
                options[OPT_OBL].cName, options[OPT_COSOBL].cName);
      }
      DoubleLineExit(options[OPT_OBL].cFile[iBody + 1],
                     options[OPT_COSOBL].cFile[iBody + 1],
                     options[OPT_OBL].iLine[iBody + 1],
                     options[OPT_COSOBL].iLine[iBody + 1]);
    }
    body[iBody].dObliquity = acos(body[iBody].dCosObl);
  }
}

void VerifyRotationGeneral(BODY *body, OPTIONS *options, char cFile[],
                           int iBody, int iVerbose) {
  /* Add 1, as this information could not have been set in primary input */
  int iFileNum = iBody + 1;
  /* Was more than one version set? */

  /* !!!!!! ------ RotRate IS ALWAYS UPDATED AND CORRECT -------- !!!!!! */

  if (options[OPT_ROTPER].iLine[iFileNum] >= 0) {
    /* Rotation Period set -- if Rate or Vel set, exit */
    if (options[OPT_ROTRATE].iLine[iFileNum] >= 0) {
      VerifyBodyExit(options[OPT_ROTPER].cName, options[OPT_ROTRATE].cName,
                     cFile, options[OPT_ROTPER].iLine[iFileNum],
                     options[OPT_ROTRATE].iLine[iFileNum], iVerbose);
    }

    if (options[OPT_ROTVEL].iLine[iFileNum] >= 0) {
      VerifyBodyExit(options[OPT_ROTPER].cName, options[OPT_ROTVEL].cName,
                     cFile, options[iBody].iLine[iFileNum],
                     options[OPT_ROTVEL].iLine[iFileNum], iVerbose);
    }
  }

  if (options[OPT_ROTVEL].iLine[iFileNum] >= 0) {
    /* Rotational Velocity set -- if Rate set, exit */
    if (options[OPT_ROTRATE].iLine[iFileNum] >= 0) {
      VerifyBodyExit(options[OPT_ROTRATE].cName, options[OPT_ROTVEL].cName,
                     cFile, options[OPT_ROTRATE].iLine[iFileNum],
                     options[OPT_ROTVEL].iLine[iFileNum], iVerbose);
    }
  }

  if (options[OPT_ROTPER].iLine[iFileNum] == -1 &&
      options[OPT_ROTVEL].iLine[iFileNum] == -1 &&
      options[OPT_ROTRATE].iLine[iFileNum] == -1) {
    /* Nothing set, print warning and return */
    if (iVerbose >= VERBINPUT) {
      fprintf(stderr,
              "INFO: No rotational information set in file %s. Defaulting to "
              "%s = %s.\n",
              cFile, options[OPT_ROTRATE].cName, options[OPT_ROTRATE].cDefault);
    }
    body[iBody].dRotRate = options[OPT_ROTRATE].dDefault;
    return;
  }

  /* dRotRate is the master parameter, so set it. */

  if (options[OPT_ROTPER].iLine[iFileNum] >= 0) {
    body[iBody].dRotRate = fdPerToFreq(body[iBody].dRotPer);
  }
  if (options[OPT_ROTVEL].iLine[iFileNum] >= 0) {
    body[iBody].dRotRate =
          fdRadiusRotVelToFreq(body[iBody].dRotVel, body[iBody].dRadius);
  }

  VerifyObliquity(body, options, iBody, iVerbose);
}

void VerifyImK2Env(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iBody) {

  // Is there an envelope? If so, fix its Im(k2)
  if (body[iBody].bEnv) {

    // they better have defined k2Env, tidalqenv, denvmass
    if (options[OPT_TIDALQENV].iLine[iBody + 1] == -1) {
      fprintf(stderr, "ERROR: %s = 1, but %s not set.\n",
              options[OPT_ENVTIDES].cName, options[OPT_TIDALQENV].cName);
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_ENVTIDES].iLine[iBody + 1]);
    }
    if (options[OPT_K2ENV].iLine[iBody + 1] == -1) {
      fprintf(stderr, "ERROR: %s = 1, but %s not set.\n",
              options[OPT_ENVTIDES].cName, options[OPT_K2ENV].cName);
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_ENVTIDES].iLine[iBody + 1]);
    }
    if (options[OPT_ENVELOPEMASS].iLine[iBody + 1] == -1) {
      fprintf(stderr, "ERROR: %s = 1, but %s not set.\n",
              options[OPT_ENVTIDES].cName, options[OPT_ENVELOPEMASS].cName);
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_ENVTIDES].iLine[iBody + 1]);
    }

    // Cannot set TidalQ and TidalQEnv
    if (options[OPT_TIDALQ].iLine[iBody + 1] > -1 &&
        options[OPT_TIDALQENV].iLine[iBody + 1] > -1) {
      fprintf(stderr, "ERROR: Cannot set both %s and %s.\n",
              options[OPT_TIDALQ].cName, options[OPT_TIDALQENV].cName);
      DoubleLineExit(options[OPT_TIDALQ].cFile[iBody + 1],
                     options[OPT_TIDALQENV].cFile[iBody + 1],
                     options[OPT_TIDALQ].iLine[iBody + 1],
                     options[OPT_TIDALQENV].iLine[iBody + 1]);
    }
    // Cannot set K2 and K2Env
    if (options[OPT_K2].iLine[iBody + 1] > -1 &&
        options[OPT_K2ENV].iLine[iBody + 1] > -1) {
      fprintf(stderr, "ERROR: Cannot set both %s and %s.\n",
              options[OPT_K2].cName, options[OPT_K2ENV].cName);
      DoubleLineExit(options[OPT_K2].cFile[iBody + 1],
                     options[OPT_K2ENV].cFile[iBody + 1],
                     options[OPT_K2].iLine[iBody + 1],
                     options[OPT_K2ENV].iLine[iBody + 1]);
    }

    // Everything OK, set dImK2Env
    // Defining this here is OK until we have a real envelope model
    body[iBody].dImK2Env = -body[iBody].dK2Env / body[iBody].dTidalQEnv;

  } else {
    // Set dImK2Env to 0, i.e. no Dissipation
    body[iBody].dImK2Env = 0;
  }
}

void VerifyImK2Ocean(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iBody) {

  // Is there an ocean? If so, fix its Im(k2)
  if (body[iBody].bOcean) {

    // they better have defined k2Ocean, tidalqOcean, dOceanmass
    if (options[OPT_TIDALQOCEAN].iLine[iBody + 1] == -1) {
      fprintf(stderr, "ERROR: %s = 1, but %s not set.\n",
              options[OPT_OCEANTIDES].cName, options[OPT_TIDALQOCEAN].cName);
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_OCEANTIDES].iLine[iBody + 1]);
    }
    if (options[OPT_K2OCEAN].iLine[iBody + 1] == -1) {
      fprintf(stderr, "ERROR: %s = 1, but %s not set.\n",
              options[OPT_OCEANTIDES].cName, options[OPT_K2OCEAN].cName);
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_OCEANTIDES].iLine[iBody + 1]);
    }
    if (options[OPT_SURFACEWATERMASS].iLine[iBody + 1] == -1) {
      fprintf(stderr, "ERROR: %s = 1, but %s not set.\n",
              options[OPT_OCEANTIDES].cName,
              options[OPT_SURFACEWATERMASS].cName);
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_OCEANTIDES].iLine[iBody + 1]);
    }

    // Cannot set TidalQ and TidalQOcean
    if (options[OPT_TIDALQ].iLine[iBody + 1] > -1 &&
        options[OPT_TIDALQOCEAN].iLine[iBody + 1] > -1) {
      fprintf(stderr, "ERROR: Cannot set both %s and %s.\n",
              options[OPT_TIDALQ].cName, options[OPT_TIDALQOCEAN].cName);
      DoubleLineExit(options[OPT_TIDALQ].cFile[iBody + 1],
                     options[OPT_TIDALQOCEAN].cFile[iBody + 1],
                     options[OPT_TIDALQ].iLine[iBody + 1],
                     options[OPT_TIDALQOCEAN].iLine[iBody + 1]);
    }
    // Cannot set K2 and K2Ocean
    if (options[OPT_K2].iLine[iBody + 1] > -1 &&
        options[OPT_K2OCEAN].iLine[iBody + 1] > -1) {
      fprintf(stderr, "ERROR: Cannot set both %s and %s.\n",
              options[OPT_K2].cName, options[OPT_K2OCEAN].cName);
      DoubleLineExit(options[OPT_K2].cFile[iBody + 1],
                     options[OPT_K2OCEAN].cFile[iBody + 1],
                     options[OPT_K2].iLine[iBody + 1],
                     options[OPT_K2OCEAN].iLine[iBody + 1]);
    }

    // Everything OK, set dImK2Ocean
    // Defining this here is OK until we have a real ocean model
    body[iBody].dImK2Ocean = -body[iBody].dK2Ocean / body[iBody].dTidalQOcean;

  } else {
    // Set dImK2Ocean to 0, i.e. no Dissipation
    body[iBody].dImK2Ocean = 0;
  }
}

void VerifyImK2Mantle(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, UPDATE *update,
                      int iBody) {

  // Is there a mantle? If so, fix its Im(k2)
  if (body[iBody].bMantle) {

    if (options[OPT_TIDALQ].iLine[iBody + 1] > -1 &&
        options[OPT_TIDALQMANTLE].iLine[iBody + 1] > -1) {
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr, "ERROR: Cannot set both %s and %s.\n",
                options[OPT_TIDALQ].cName, options[OPT_TIDALQMANTLE].cName);
      }
      DoubleLineExit(options[OPT_TIDALQ].cFile[iBody + 1],
                     options[OPT_TIDALQMANTLE].cFile[iBody + 1],
                     options[OPT_TIDALQ].iLine[iBody + 1],
                     options[OPT_TIDALQMANTLE].iLine[iBody + 1]);
    }
    // Cannot set K2 and K2Mantle
    if (options[OPT_K2].iLine[iBody + 1] > -1 &&
        options[OPT_K2MANTLE].iLine[iBody + 1] > -1) {
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr, "ERROR: Cannot set both %s and %s.\n",
                options[OPT_K2].cName, options[OPT_K2MANTLE].cName);
      }
      DoubleLineExit(options[OPT_K2].cFile[iBody + 1],
                     options[OPT_K2MANTLE].cFile[iBody + 1],
                     options[OPT_K2].iLine[iBody + 1],
                     options[OPT_K2MANTLE].iLine[iBody + 1]);
    }

    if (body[iBody].bThermint) {
      // User set dTidalQMan
      if (options[OPT_TIDALQMANTLE].iLine[iBody + 1] == -1) {
        body[iBody].dTidalQMan = body[iBody].dTidalQ;
        if (control->Io.iVerbose >= VERBALL) {
          fprintf(stderr,
                  "INFO: %s set, but ThermInt computes it. Input value will be "
                  "ignored.\n",
                  options[OPT_TIDALQMANTLE].cName);
        }
      }

      body[iBody].dK2Man   = fdK2Man(body, iBody);
      body[iBody].dImK2Man = fdImK2Man(body, iBody);
    } else {
      body[iBody].dImK2Man     = -body[iBody].dK2Man / body[iBody].dTidalQMan;
      body[iBody].dShmodUMan   = 1; // Set to avoid division by zero in log file
      body[iBody].dDynamViscos = 1; // Also used in log file
      body[iBody].dStiffness   = 1; // Ditto
    }
  } else {
    body[iBody].dImK2Man = 0;
  }
}

/**

  Verify all input parameters related to tidal energy dissipation (Power) into
  all regions of a planet. The parameter Im(k2) is set by the orbit and the
  internal temperature in the layer, which is set by the tidal, secular, and
  radiogenic heating rates. The heating is divided into layers: 1) Mantle, 2)
  Ocean, and 3) Envelope, and may be calculated by the EqTide, RadHeat, and/or
  ThermInt modules. Those modules in turn have different possibilities. By the
  end of this function, the values of Im(k2) in all layers and the body as a
  whole are set. */
void VerifyImK2(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, UPDATE *update, int iBody) {

  // First gather auxiliary properties for relevant modules
  PropsAuxEqtide(body, &control->Evolve, &control->Io, update, iBody);
  if (body[iBody].bThermint) {
    fvPropsAuxThermint(body, &control->Evolve, &control->Io, update, iBody);
  }

  VerifyImK2Env(body, control, files, options, system, iBody);

  VerifyImK2Ocean(body, control, files, options, system, iBody);

  VerifyImK2Mantle(body, control, files, options, system, update, iBody);

  // fdImK2Total determines which layers set (if any), and returns appropriate
  // Im(k_2)
  body[iBody].dImK2 = fdImK2Total(body, iBody);

  if (control->Io.iVerbose > VERBPROG) {
    fprintf(stderr, "%s's Im(k_2) verified.\n", body[iBody].cName);
  }
  /*
    // now lets check there's actually an envelope
    // there is not an envelope!!
    if (!(body[iBody].dEnvelopeMass > body[iBody].dMinEnvelopeMass)) {
      body[iBody].bEnv = 0;
    }
    else {
      body[iBody].bEnv = 1;
      body[iBody].dImK2Env = body[iBody].dK2Env / body[iBody].dTidalQEnv;
    }
    // what about an ocean?
    if (!(body[iBody].dSurfaceWaterMass > body[iBody].dMinSurfaceWaterMass)) {
      body[iBody].bOcean = 0;
    }
    else {
      body[iBody].bOcean = 1;
      body[iBody].dImK2Ocean = body[iBody].dK2Ocean / body[iBody].dTidalQOcean;
    }

    // there's definitely at least gonna be some rock...
    body[iBody].dImK2Rock = body[iBody].dK2Rock / body[iBody].dTidalQRock;

    // if there is an envelope/ocean, we calculate ImK2Env/ImK2Ocean
    if (body[iBody].bEnv && (body[iBody].dTidalQ != body[iBody].dTidalQEnv)) {
      if (control->Io.iVerbose > 1) {
        fprintf(stderr,"Using dTidalQEnv for %s.\n",body[iBody].cName);
      }
      body[iBody].dTidalQ = body[iBody].dTidalQEnv;
      body[iBody].dK2 = body[iBody].dK2Env;
      body[iBody].dImK2Env = body[iBody].dK2Env / body[iBody].dTidalQEnv;
      body[iBody].dImK2 = body[iBody].dImK2Env;
    }
    else {
      if (body[iBody].bOcean && (body[iBody].dTidalQ !=
  body[iBody].dTidalQOcean)) { fprintf(stderr,"Using dTidalQOcean for
  %s.\n",body[iBody].cName); body[iBody].dTidalQ = body[iBody].dTidalQOcean;
        body[iBody].dImK2Ocean = body[iBody].dK2Ocean /
  body[iBody].dTidalQOcean; body[iBody].dImK2 = body[iBody].dImK2Ocean;
        body[iBody].dK2 = body[iBody].dK2Ocean;
      }
      else if (!body[iBody].bEnv && !body[iBody].bOcean && (body[iBody].dTidalQ
  != body[iBody].dTidalQRock) && (iBody != 0)){ fprintf(stderr,"Using
  dTidalQRock for %s.\n",body[iBody].cName);
        // now we just use dTidalQRock and dK2Rock
        body[iBody].dImK2Rock = body[iBody].dK2Rock / body[iBody].dTidalQRock;
        body[iBody].dTidalQ = body[iBody].dTidalQRock;
        body[iBody].dK2 = body[iBody].dK2Rock;
        body[iBody].dImK2 = body[iBody].dImK2Rock;
      }
  }
*/
}


/*
 *
 * System-level verify routines
 *
 */

/*! Initialize angular momentum for energy for conservation checks.  Initialize
 *  here so anything that changes E, J is monitored to ensure conservation */
void InitializeConstants(BODY *body, UPDATE *update, CONTROL *control,
                         SYSTEM *system, OPTIONS *options) {
  int iBody;

  // Initially no lost angular momentum, energy
  // Set to dTINY, not 0 since these are integrated
  // and it being 0 can mess up the time step
  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    body[iBody].dLostAngMom = dTINY;
    body[iBody].dLostEng    = dTINY;
  }

  // Compute initial total angular momentum
  system->dTotAngMomInit = fdTotAngMom(body, control, system);
  system->dTotAngMom     = system->dTotAngMomInit;

  // Compute initial total energy
  system->dTotEnInit = fdTotEnergy(body, control, system);
  system->dTotEn     = system->dTotEnInit;
}

void fnNullDerivatives(BODY *body, EVOLVE *evolve, MODULE *module,
                       UPDATE *update, fnUpdateVariable ***fnUpdate) {
  int iBody, iVar, iEqn, iNumBodies, iNumVars,
        iNumEqns; // Dummy counting variables

  iNumBodies = evolve->iNumBodies;
  for (iBody = 0; iBody < iNumBodies; iBody++) {
    if (update[iBody].iNumVars > 0) {
      iNumVars = update[iBody].iNumVars;
      for (iVar = 0; iVar < iNumVars; iVar++) {
        iNumEqns = update[iBody].iNumEqns[iVar];
        for (iEqn = 0; iEqn < iNumEqns; iEqn++) {
          fnUpdate[iBody][iVar][iEqn] = &fndUpdateFunctionTiny;
        }
      }
    }
  }
}

void VerifyMantle(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  int iBody) {

  // XXX This is broken, user should be able to set mantle properties w/o
  // thermint
  if (body[iBody].bThermint) {
    body[iBody].bMantle = 1;
  }
  /* See above
  else {
    body[iBody].bMantle = 0;
  }
*/
}

void VerifyOcean(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 int iBody) {

  if (body[iBody].dSurfaceWaterMass < body[iBody].dMinSurfaceWaterMass) {
    body[iBody].bOcean = 0;
    if (control->Io.iVerbose && body[iBody].bAtmEsc) {
      fprintf(stderr,
              "INFO: %s < %s. No envelope evolution will be included.\n",
              options[OPT_SURFACEWATERMASS].cName,
              options[OPT_MINSURFACEWATERMASS].cName);
    }
  }
}

void VerifyEnvelope(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, int iBody) {

  if (body[iBody].dEnvelopeMass < body[iBody].dMinEnvelopeMass) {
    body[iBody].bEnv = 0;
    if (control->Io.iVerbose >= VERBINPUT && body[iBody].bAtmEsc) {
      fprintf(stderr,
              "INFO: %s < %s. No envelope evolution will be included.\n",
              options[OPT_ENVELOPEMASS].cName,
              options[OPT_MINENVELOPEMASS].cName);
    }
  }
}

void VerifyLayers(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  int iBody) {

  VerifyMantle(body, control, files, options, iBody);
  VerifyOcean(body, control, files, options, iBody);
  VerifyEnvelope(body, control, files, options, iBody);
}

void VerifyAge(BODY *body, CONTROL *control, OPTIONS *options) {
  int bAgeSet, iBody;
  double dAge;

  // Assume age wasn't set
  bAgeSet = 0;
  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    if (options[OPT_AGE].iLine[iBody + 1] > -1) {
      // Age was set!
      bAgeSet = 1;
      dAge    = body[iBody].dAge;
    }
  }

  // For now, all bodies must be the same age
  if (bAgeSet) {
    if (control->Io.iVerbose == VERBALL) {
      printf("INFO: Age set in one file, all bodies will have this age.\n");
    }
    for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
      body[iBody].dAge = dAge;
    }
  }
}
/**

 * Master Verify subroutine

   This function validates the input options. After this function, the structs
 are prepared for integration.
 */

void VerifyOptions(BODY *body, CONTROL *control, FILES *files, MODULE *module,
                   OPTIONS *options, OUTPUT *output, SYSTEM *system,
                   UPDATE *update, fnIntegrate *fnOneStep,
                   fnUpdateVariable ****fnUpdate) {

  int iBody, iModule;

  control->Evolve.dTime  = 0;
  control->Evolve.nSteps = 0;

  VerifyAge(body, control, options);
  VerifyNames(body, control, options);

  // Need to know integration type before we can initialize CONTROL
  VerifyIntegration(body, control, files, options, system, fnOneStep);
  InitializeControlEvolve(body, control, module, update);

  // Allocate all memory for the BODY struct, based on selected modules
  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
      module->fnInitializeBody[iBody][iModule](body, control, update, iBody,
                                               iModule);
    }
  }

  // Verify physical and orbital properties
  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    /* Must verify density first: RotVel requires a radius in VerifyRotation */
    VerifyMassRad(&body[iBody], control, options, files->Infile[iBody].cIn,
                  iBody);
    VerifyRotationGeneral(body, options, files->Infile[iBody + 1].cIn, iBody,
                          control->Io.iVerbose);

    // If any bodies orbit, make sure they do so properly!
    if (control->bOrbiters) {
      VerifyOrbit(body, control, *files, options, iBody);
    }

    VerifyLayers(body, control, files, options, iBody);
  }

  InitializeUpdate(body, control, module, update, fnUpdate);
  InitializeHalts(control, module);

  VerifyHalts(body, control, module, options);

  // Now verify the modules' parameters
  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
      module->fnVerify[iBody][iModule](body, control, files, options, output,
                                       system, update, iBody, iModule);
    }

    // Verify multi-module couplings
    VerifyModuleMulti(body, update, control, files, module, options, iBody,
                      fnUpdate);

    for (iModule = 0; iModule < module->iNumManageDerivs[iBody]; iModule++) {
      module->fnAssignDerivatives[iBody][iModule](body, &(control->Evolve),
                                                  update, *fnUpdate, iBody);
    }

    /* Must allocate memory in control struct for all perturbing bodies */
    if (control->Evolve.iOneStep == RUNGEKUTTA) {
      InitializeUpdateBodyPerts(control, update, iBody);
      InitializeUpdateTmpBody(body, control, module, update, iBody);
    }
  }

  // Verify multi-module parameters
  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    if (body[iBody].bEqtide) {
      VerifyImK2(body, control, files, options, system, update, iBody);
    }
  }

  // Initialize angular momentum and energy prior to logging/integration
  InitializeConstants(body, update, control, system, options);

  // Set next output time so logging does not contain a memory leak
  // control->Io.dNextOutput = control->Evolve.dTime + control->Io.dOutputTime;

  // Finally, initialize derivative values -- this avoids leaks while logging
  PropertiesAuxiliary(body, control, system, update);
  CalculateDerivatives(body, system, update, *fnUpdate,
                       control->Evolve.iNumBodies);

  control->Io.dNextOutput = control->Evolve.dTime + control->Io.dOutputTime;
}
