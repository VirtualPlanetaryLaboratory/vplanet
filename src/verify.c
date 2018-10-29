/**
  @file verify.c
  @brief Validate the options. This will become a mess!
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date May 7 2014
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "vplanet.h"

/*
 * Utility functions
 */

/*! Check to see if two decimals numbers are equal (1) or not (0) */
int bFloatComparison(double x, double y) {
  double dBigger;
  double dRel_Tol;
  if (fabs(x)>fabs(y)) {
    dBigger = fabs(x);
  } else {
    dBigger = fabs(y);
  }

  dRel_Tol = 5*dBigger*DBL_EPSILON;

  if (dRel_Tol <= 10*dTINY) {
    dRel_Tol = 10*dTINY;
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

int bFileExists(const char *filename){
  FILE *file;
  if ((file = fopen(filename, "r"))) {
    fclose(file);
    return 1;
  }
  return 0;
}

void OverwriteExit(char cName[],char cFile[]) {
  fprintf(stderr,"ERROR: %s is false and %s exists.\n",cName,cFile);
  fprintf(stderr,"\tOveride with \"-f\" on the command line.\n");
  exit(EXIT_INPUT);
}

/* XXX Should these be iLine+1? */
void DoubleLineExit(char cFile1[],char cFile2[],int iLine1,int iLine2) {
  fprintf(stderr,"\tFile: %s, Line: %d.\n",cFile1,iLine1+1);
  fprintf(stderr,"\tFile: %s, Line: %d.\n",cFile2,iLine2+1);
  exit(EXIT_INPUT);
}

void VerifyOrbitExit(char cName1[],char cName2[],char cFile1[],char cFile2[],int iLine1,int iLine2,int iVerbose) {
  if (iVerbose >= VERBERR) {
    fprintf(stderr,"ERROR: Cannot set both %s and %s.\n",cName1,cName2);
    fprintf(stderr,"\tFile: %s, Line: %d.\n",cFile1,iLine1);
    fprintf(stderr,"\tFile: %s, Line: %d.\n",cFile2,iLine2);
  }
  exit(EXIT_INPUT);
}

void VerifyBodyExit(char cName1[],char cName2[],char cFile[],int iLine1,int iLine2,int iVerbose) {

  if (iVerbose >= VERBERR) {
    fprintf(stderr,"ERROR: Cannot set both %s and %s in same file.\n",cName1,cName2);
    fprintf(stderr,"\tFile: %s, Lines: %d and %d\n",cFile,iLine1,iLine2);
  }
  exit(EXIT_INPUT);
}

/** Print three lines that are in conflict
    Only called if iVerbose >= VERBERR
*/

void TripleLineExit(char cFile[],int iLine1,int iLine2,int iLine3) {
  fprintf(stderr,"\tFile: %s, Lines: %d, %d and %d.\n",cFile,iLine1,iLine2,iLine3);
  exit(EXIT_INPUT);
}

/* Do we need both these? */
void VerifyTripleExit(char cName1[],char cName2[],char cName3[],int iLine1,int iLine2,int iLine3,char cFile[],int iVerbose) {
  if (iVerbose >= VERBERR) {
    fprintf(stderr,"ERROR: Cannot set %s, %s, and %s simultaneously.\n",cName1,cName2,cName3);
    TripleLineExit(cFile,iLine1,iLine2,iLine3);
  }
}

void VerifyTwoOfThreeExit(char cName1[],char cName2[],char cName3[],int iLine1,int iLine2,int iLine3,char cFile[],int iVerbose) {

  if (iVerbose >= VERBERR) {
    fprintf(stderr,"ERROR: Can only set 2 of %s, %s, and %s.\n",cName1,cName2,cName3);
    TripleLineExit(cFile,iLine1,iLine2,iLine3);
  }
}

void VerifyDynEllip(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody,int iVerbose) {
  if (body[iBody].bCalcDynEllip == 1) {
    /* check if bCalcDynEllip and dDynEllip are both set */
    if (options[OPT_DYNELLIP].iLine[iBody+1] > -1) {
      if (iVerbose >= VERBINPUT)
        fprintf(stderr,"WARNING: %s set in file %s, but %s set to 1. %s will be overridden.\n",options[OPT_DYNELLIP].cName,cFile,options[OPT_CALCDYNELLIP].cName,options[OPT_DYNELLIP].cName);
    }
    body[iBody].dDynEllip = CalcDynEllipEq(body,iBody);
  }
}

/**
  Verify the user did not use the same name for two bodies.
  */

void VerifyNames(BODY *body,CONTROL *control,OPTIONS *options) {
  int iBody,jBody;

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    if (strlen(body[iBody].cName) == 0) {
      if (control->Io.iVerbose > VERBINPUT) {
        fprintf(stderr,"WARNING: No input to %s in file %s, defaulting to %d/\n",options[OPT_BODYNAME].cName,options[OPT_BODYNAME].cFile[iBody]+1,iBody);
      }
      sprintf(body[iBody].cName,"%d",iBody+1);
    }
    for (jBody = iBody+1;jBody<control->Evolve.iNumBodies;jBody++) {
      if (strcmp(body[iBody].cName,body[jBody].cName) == 0) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr,"ERROR: Two bodies have the same name.\n");
          DoubleLineExit(options[OPT_BODYNAME].cFile[iBody+1],options[OPT_BODYNAME].cFile[jBody+1],options[OPT_BODYNAME].iLine[iBody+1],options[OPT_BODYNAME].iLine[jBody+1]);
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

void VerifyOrbit(BODY *body, FILES files,OPTIONS *options,CONTROL *control, int iBody,int iVerbose) {
  int iFile=iBody+1;
  double dSemi=0,dMeanMotion=0,dPeriod=0;

  // Body 0 is never an orbiter
  if(iBody == 0)
    return;

  /* !!!!! ------ Semi IS ALWAYS CORRECT AND IN BODY[iBody] ------- !!!!!! */

  if (options[OPT_ORBSEMI].iLine[iFile] > -1 && options[OPT_ORBMEANMOTION].iLine[iFile] == -1 && options[OPT_ORBPER].iLine[iFile] == -1)
    dSemi = body[iBody].dSemi;

  if (options[OPT_ORBSEMI].iLine[iFile] == -1 && options[OPT_ORBMEANMOTION].iLine[iFile] > -1 && options[OPT_ORBPER].iLine[iFile] == -1)
      dMeanMotion = body[iBody].dMeanMotion;

  if (options[OPT_ORBSEMI].iLine[iFile] == -1 && options[OPT_ORBMEANMOTION].iLine[iFile] == -1 && options[OPT_ORBPER].iLine[iFile] > -1)
    dPeriod = body[iBody].dOrbPeriod;

  /* Was Semi set and nothing else? */
  if (dSemi > 0 && dMeanMotion == 0 && dPeriod == 0) {
    if (body[iBody].bPoise) {
      if(body[iBody].bBinary == 0){ // Not binary, regular single-star orbit
        body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[iBody].dMass);
      }
      else if(body[iBody].bBinary && body[iBody].iBodyType == 0){ // Set mean motion for CBP (primary,iBody==0;planet,iBody > 1)
        body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[1].dMass+body[iBody].dMass);
      }
      else if(body[iBody].bBinary && body[iBody].iBodyType == 1 && iBody == 1) { // Set mean motion for binary, info in secondary
        body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[iBody].dMass);
      }
    }
    else if(body[iBody].bPoise == 0)
    {
      if(body[iBody].bBinary != 1)
      {
        if(iBody > 0) // No binary, regular planet
        {
          body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[iBody].dMass);
        }
      }
      else // Doing binary
      {
        if(body[iBody].iBodyType == 0) // CBP
        {
          body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[1].dMass+body[iBody].dMass);
        }
        else if(body[iBody].iBodyType == 1 && iBody == 1) // binary orbit info in secondary
        {
          body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[1].dMass);
        }
      }
    }

    return;
  }

  /* Was anything set? */
  if (dSemi == 0 && dMeanMotion == 0 && dPeriod == 0) {
    fprintf(stderr,"ERROR: Must set one of %s, %s or %s.\n",options[OPT_ORBSEMI].cName,options[OPT_ORBMEANMOTION].cName,options[OPT_ORBPER].cName);
    exit(EXIT_INPUT);
  }

  /* If Semi set, was anything else? */
  if (dSemi > 0) {
    if (dMeanMotion > 0)
      VerifyOrbitExit(options[OPT_ORBSEMI].cName,options[OPT_ORBMEANMOTION].cName,files.Infile[iFile].cIn,files.Infile[iFile].cIn,options[OPT_ORBSEMI].iLine[iFile],options[OPT_ORBMEANMOTION].iLine[iFile],iVerbose);

    if (dPeriod > 0)
      VerifyOrbitExit(options[OPT_ORBSEMI].cName,options[OPT_ORBPER].cName,files.Infile[iFile].cIn,files.Infile[iFile].cIn,options[OPT_ORBSEMI].iLine[iFile],options[OPT_ORBPER].iLine[iFile],iVerbose);

  }

  /* Were MeanMotion and OrbPeriod both set? */
  if (dPeriod > 0 && dMeanMotion > 0)
    VerifyOrbitExit(options[OPT_ORBMEANMOTION].cName,options[OPT_ORBPER].cName,files.Infile[iFile].cIn,files.Infile[iFile].cIn,options[OPT_ORBMEANMOTION].iLine[iFile],options[OPT_ORBPER].iLine[iFile],iVerbose);

  /* Only one option set */

  if (dMeanMotion > 0) {
    if(body[iBody].bBinary && body[iBody].iBodyType == 0) // CBP
      body[iBody].dSemi = fdMeanMotionToSemi(body[0].dMass+body[1].dMass,body[iBody].dMass,dMeanMotion);
    else if(body[iBody].bBinary && iBody == 1) // binary
      body[iBody].dSemi = fdMeanMotionToSemi(body[0].dMass,body[1].dMass,dMeanMotion);
    else if(body[iBody].bBinary && body[iBody].iBodyType == 1 && iBody == 0)
    {}
    else
      body[iBody].dSemi = fdMeanMotionToSemi(body[0].dMass,body[iBody].dMass,dMeanMotion);
  }
  if (dPeriod > 0) {
    if(body[iBody].bBinary && body[iBody].iBodyType == 0) // CBP
      body[iBody].dSemi = fdPeriodToSemi(dPeriod,(body[0].dMass+body[1].dMass+body[iBody].dMass));
    else if(body[iBody].bBinary && body[iBody].iBodyType == 1 && iBody == 1) // binary
      body[iBody].dSemi = fdPeriodToSemi(dPeriod,(body[0].dMass+body[iBody].dMass));
    else if(body[iBody].bBinary && body[iBody].iBodyType == 1 && iBody == 0)
    {}
    else
      body[iBody].dSemi = fdPeriodToSemi(dPeriod,(body[0].dMass+body[iBody].dMass));
  }
  if (dSemi > 0) {
    if(body[iBody].bBinary && body[iBody].iBodyType == 0) // CBP
      body[iBody].dSemi = dSemi;
    else if(body[iBody].bBinary && body[iBody].iBodyType == 1 && iBody == 1) // binary
      body[iBody].dSemi = dSemi;
    else if(body[iBody].bBinary && body[iBody].iBodyType == 1 && iBody == 0)
    {}
    else
      body[iBody].dSemi = dSemi;
  }
  if (dMeanMotion == 0) {
    if(body[iBody].bBinary && body[iBody].iBodyType == 0) // CBP
      body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[1].dMass+body[iBody].dMass);
    else if(body[iBody].bBinary && body[iBody].iBodyType == 1 && iBody == 1) // Primary
      body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[1].dMass);
    else if(body[iBody].bBinary && body[iBody].iBodyType == 1 && iBody == 0)
    {}
    else
      body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[iBody].dMass);
  }

}

/*
 *
 *   Verify Integration
 *
 */

void IntegrationWarning(char cName1[],char cName2[],char cName3[],char cFile[],int iLine,int iVerbose) {
  if (iVerbose > VERBINPUT)
    fprintf(stderr,"WARNING: %s set, but neither %s nor %s set (%s: Line %d).\n",cName1,cName2,cName3,cFile,iLine);
  /* Backward file name */
}

void VerifyIntegration(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnIntegrate *fnOneStep) {
  int iFile,iFile1,iFile2;
  char cTmp[OPTLEN];


  // Initialize iDir to 0, i.e. assume no integrations requested to start
  control->Evolve.iDir=0;

  /* Were both Forward and Backward Set? */
  if (control->Evolve.bDoBackward && control->Evolve.bDoForward) {
    fprintf(stderr,"ERROR: Both %s and %s set. Only one is allowed.\n",options[OPT_BACK].cName,options[OPT_FORW].cName);
    /* Now identify which files contained the two offending options */
    for (iFile=0;iFile<files->iNumInputs;iFile++) {
      if (options[OPT_BACK].iLine[iFile] > 0)
	iFile1=iFile; // Error of multiple occurences checked in Read
      if (options[OPT_FORW].iLine[iFile] > 0)
	iFile2=iFile; // Error of multiple occurences checked in Read
    }
    DoubleLineExit(options[OPT_BACK].cFile[iFile1],options[OPT_FORW].cFile[iFile2],options[OPT_BACK].iLine[iFile1],options[OPT_FORW].iLine[iFile2]);
  }

  /* Fix backward output file */
  if (control->Evolve.bDoBackward) {
    for (iFile=1;iFile<files->iNumInputs;iFile++) {
      if (options[OPT_OUTFILE].iLine[iFile] == -1) {
        sprintf(files->Outfile[iFile-1].cOut,"%s.%s.backward",system->cName,body[iFile-1].cName);
        if (control->Io.iVerbose >= VERBINPUT)
          fprintf(stderr,"WARNING: %s not set, defaulting to %s.\n",options[OPT_OUTFILE].cName,files->Outfile[iFile-1].cOut);
      }
    }
    control->Evolve.iDir = -1;
  }

  /* Fix forward output file */
  if (control->Evolve.bDoForward) {
    for (iFile=1;iFile<files->iNumInputs;iFile++) {
      if (options[OPT_OUTFILE].iLine[iFile] == -1) {
        sprintf(files->Outfile[iFile-1].cOut,"%s.%s.forward",system->cName,body[iFile-1].cName);
        if (control->Io.iVerbose >= VERBINPUT)
          fprintf(stderr,"WARNING: %s not set, defaulting to %s.\n",options[OPT_OUTFILE].cName,files->Outfile[iFile-1].cOut);
      }
    }
    control->Evolve.iDir = 1;
  }

  /* Check for file existence */
  for (iFile=0;iFile<files->iNumInputs-1;iFile++) {
    if (bFileExists(files->Outfile[iFile].cOut)) {
      if (!control->Io.bOverwrite) {
        OverwriteExit(options[OPT_OVERWRITE].cName,files->Outfile[iFile].cOut);
      }
      if (control->Io.iVerbose >= VERBINPUT)
        fprintf(stderr,"WARNING: %s exists.\n",files->Outfile[iFile].cOut);
      unlink(files->Outfile[iFile].cOut);
    }
  }

  /* Was DoBackward or DoForward NOT set? */
  if (!control->Evolve.bDoBackward && !control->Evolve.bDoForward) {
    for (iFile=0;iFile<files->iNumInputs;iFile++) {
      if (options[OPT_ETA].iLine[iFile] > -1)
        IntegrationWarning(options[OPT_ETA].cName,options[OPT_BACK].cName,options[OPT_FORW].cName,options[OPT_ETA].cFile[iFile],options[OPT_ETA].iLine[iFile],control->Io.iVerbose);

      if (options[OPT_OUTPUTTIME].iLine[iFile] > -1)
        IntegrationWarning(options[OPT_OUTPUTTIME].cName,options[OPT_BACK].cName,options[OPT_FORW].cName,options[OPT_OUTPUTTIME].cFile[iFile],options[OPT_OUTPUTTIME].iLine[iFile],control->Io.iVerbose);

      if (options[OPT_STOPTIME].iLine[iFile] > -1)
        IntegrationWarning(options[OPT_STOPTIME].cName,options[OPT_BACK].cName,options[OPT_FORW].cName,options[OPT_STOPTIME].cFile[iFile],options[OPT_STOPTIME].iLine[iFile],control->Io.iVerbose);

      if (options[OPT_TIMESTEP].iLine[iFile] > -1)
        IntegrationWarning(options[OPT_TIMESTEP].cName,options[OPT_BACK].cName,options[OPT_FORW].cName,options[OPT_TIMESTEP].cFile[iFile],options[OPT_TIMESTEP].iLine[iFile],control->Io.iVerbose);

      if (options[OPT_VARDT].iLine[iFile] > -1)
        IntegrationWarning(options[OPT_VARDT].cName,options[OPT_BACK].cName,options[OPT_FORW].cName,options[OPT_VARDT].cFile[iFile],options[OPT_VARDT].iLine[iFile],control->Io.iVerbose);

      if (options[OPT_OUTPUTORDER].iLine[iFile] > -1)
        IntegrationWarning(options[OPT_OUTPUTORDER].cName,options[OPT_BACK].cName,options[OPT_FORW].cName,options[OPT_OUTPUTORDER].cFile[iFile],options[OPT_OUTPUTORDER].iLine[iFile],control->Io.iVerbose);
    }
  }

  if (control->Evolve.iOneStep == EULER)
    *fnOneStep = &EulerStep;
  else if (control->Evolve.iOneStep == RUNGEKUTTA)
    *fnOneStep = &RungeKutta4Step;
  else {
    /* Assign Default */
    strcpy(cTmp,options[OPT_INTEGRATIONMETHOD].cDefault);
    if (control->Io.iVerbose >= VERBINPUT)
      fprintf(stderr,"WARNING: %s not set, defaulting to %s.\n",options[OPT_INTEGRATIONMETHOD].cName,options[OPT_INTEGRATIONMETHOD].cDefault);
    if (memcmp(sLower(cTmp),"e",1) == 0) {
      control->Evolve.iOneStep = EULER;
      *fnOneStep = &EulerStep;
    }
    if (memcmp(sLower(cTmp),"r",1) == 0) {
      control->Evolve.iOneStep = RUNGEKUTTA;
      *fnOneStep = &RungeKutta4Step;
    }
  }

  /* Make sure output interval is less than stop time */
  if (control->Evolve.dStopTime < control->Io.dOutputTime) {
    fprintf(stderr,"ERROR: %s < %s is not allowed.\n",options[OPT_STOPTIME].cName,options[OPT_OUTPUTTIME].cName);
    /* Now identify which files contained the two offending options */
    for (iFile=0;iFile<files->iNumInputs;iFile++) {
      if (options[OPT_STOPTIME].iLine[iFile] > 0)
	iFile1=iFile; // Error of multiple occurences checked in Read
      if (options[OPT_OUTPUTTIME].iLine[iFile] > 0)
	iFile2=iFile; // Error of multiple occurences checked in Read
    }
    DoubleLineExit(options[OPT_STOPTIME].cFile[iFile1],options[OPT_STOPTIME].cFile[iFile2],options[OPT_STOPTIME].iLine[iFile1],options[OPT_OUTPUTTIME].iLine[iFile2]);
  }
}

/*
 *
 * Mass-Radius Relationships
 *
 */

void VerifyMassRad(BODY *body,CONTROL *control,OPTIONS *options,int iBody,char cFile[],int iVerbose) {
  int iFile=iBody+1;

  /* !!!!!! --- Mass and Radius ARE ALWAYS UPDATED AND CORRECT --- !!!!!! */

  /* First see if mass and radius and nothing else set, i.e. the user input the default parameters */
  if (options[OPT_MASS].iLine[iFile] > -1 && options[OPT_RADIUS].iLine[iFile] > -1 && options[OPT_DENSITY].iLine[iFile] == -1&& options[OPT_MASSRAD].iLine[iFile] == -1)
    return;

  /* Was anything set> */
  if (options[OPT_MASS].iLine[iFile] == -1 && options[OPT_RADIUS].iLine[iFile] == -1 && options[OPT_DENSITY].iLine[iFile] == -1) {
    if (iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Must set at least one of %s, %s, and %s.\n",options[OPT_MASS].cName,options[OPT_RADIUS].cName,options[OPT_DENSITY].cName);
    exit(EXIT_INPUT);
  }

  /* Were all set? */
  if (options[OPT_MASS].iLine[iFile] > -1 && options[OPT_RADIUS].iLine[iFile] > -1 && options[OPT_DENSITY].iLine[iFile] > -1) {
    VerifyTripleExit(options[OPT_MASS].cName,options[OPT_RADIUS].cName,options[OPT_DENSITY].cName,options[OPT_MASS].iLine[iFile],options[OPT_RADIUS].iLine[iFile],options[OPT_DENSITY].iLine[iFile],cFile,iVerbose);
    exit(EXIT_INPUT);
  }

  /* Was mass set? */
  if (options[OPT_MASS].iLine[iFile] > -1) {

    /* Can only set 1 other */
    if (options[OPT_RADIUS].iLine[iFile] > -1 && options[OPT_MASSRAD].iLine[iFile] > -1)
      VerifyTwoOfThreeExit(options[OPT_MASS].cName,options[OPT_RADIUS].cName,options[OPT_MASSRAD].cName,options[OPT_MASS].iLine[iFile],options[OPT_RADIUS].iLine[iFile],options[OPT_MASSRAD].iLine[iFile],cFile,iVerbose);

    if (options[OPT_RADIUS].iLine[iFile] > -1 && options[OPT_DENSITY].iLine[iFile] > -1)
      VerifyTwoOfThreeExit(options[OPT_MASS].cName,options[OPT_RADIUS].cName,options[OPT_DENSITY].cName,options[OPT_MASS].iLine[iFile],options[OPT_RADIUS].iLine[iFile],options[OPT_DENSITY].iLine[iFile],cFile,iVerbose);

    if (options[OPT_MASSRAD].iLine[iFile] > -1 && options[OPT_DENSITY].iLine[iFile] > -1)
      VerifyTwoOfThreeExit(options[OPT_MASS].cName,options[OPT_MASSRAD].cName,options[OPT_DENSITY].cName,options[OPT_MASS].iLine[iFile],options[OPT_MASSRAD].iLine[iFile],options[OPT_DENSITY].iLine[iFile],cFile,iVerbose);

    /* Only Mass and something else set */

    if (options[OPT_RADIUS].iLine[iFile] > -1)
      /* Mass and radius were the only two set - Nothing to do */
      return;
    if (options[OPT_DENSITY].iLine[iFile] > -1)
      /* Must get radius from density */
      body->dRadius = fdDensityMassToRadius(body->dDensity,body->dMass);
    if (options[OPT_MASSRAD].iLine[iFile] > -1)
      /* Must get radius from relationship */
      body->dRadius = fdMassToRad(body->dMass,control->iMassRad[iBody]);

    return;
  }

  /* Was radius set, but not mass? */
  if (options[OPT_RADIUS].iLine[iFile] > -1) {
    if (options[OPT_MASSRAD].iLine[iFile] >= -1 && options[OPT_DENSITY].iLine[iFile] >= -1)
      VerifyTwoOfThreeExit(options[OPT_MASS].cName,options[OPT_MASSRAD].cName,options[OPT_DENSITY].cName,options[OPT_MASS].iLine[iFile],options[OPT_MASSRAD].iLine[iFile],options[OPT_DENSITY].iLine[iFile],cFile,iVerbose);

    /* Only Radius and something else set */

    if (options[OPT_MASSRAD].iLine[iFile] > -1)
      /* Must get mass from relationship */
      body->dMass=fdRadToMass(body->dRadius,control->iMassRad[iBody]);
    if (options[OPT_DENSITY].iLine[iFile] > -1)
      /* Must get mass from density */
      body->dMass=fdMassFromRadiusDensity(body->dRadius,body->dDensity);
  }
}

/*
 *
 * Verify Rotation
 *
 */

void VerifyRotationGeneral(BODY *body,OPTIONS *options,int iBody,int iVerbose,char cFile[]) {
  /* Add 1, as this information could not have been set in primary input */
  int iFileNum = iBody+1;
  /* Was more than one version set? */

  /* !!!!!! ------ RotRate IS ALWAYS UPDATED AND CORRECT -------- !!!!!! */

  if (options[OPT_ROTPER].iLine[iFileNum] >= 0) {
    /* Rotation Period set -- if Rate or Vel set, exit */
    if (options[OPT_ROTRATE].iLine[iFileNum] >= 0)
      VerifyBodyExit(options[OPT_ROTPER].cName,options[OPT_ROTRATE].cName,cFile,options[OPT_ROTPER].iLine[iFileNum],options[OPT_ROTRATE].iLine[iFileNum],iVerbose);

    if (options[OPT_ROTVEL].iLine[iFileNum] >= 0)
      VerifyBodyExit(options[OPT_ROTPER].cName,options[OPT_ROTVEL].cName,cFile,options[iBody].iLine[iFileNum],options[OPT_ROTVEL].iLine[iFileNum],iVerbose);

  }

  if (options[OPT_ROTVEL].iLine[iFileNum] >= 0) {
    /* Rotational Velocity set -- if Rate set, exit */
    if (options[OPT_ROTRATE].iLine[iFileNum] >= 0)
      VerifyBodyExit(options[OPT_ROTRATE].cName,options[OPT_ROTVEL].cName,cFile,options[OPT_ROTRATE].iLine[iFileNum],options[OPT_ROTVEL].iLine[iFileNum],iVerbose);
  }

  if (options[OPT_ROTPER].iLine[iFileNum] == -1 && options[OPT_ROTVEL].iLine[iFileNum] == -1 && options[OPT_ROTRATE].iLine[iFileNum] == -1) {
    /* Nothing set, print warning and return */
    if (iVerbose >= VERBINPUT)
      fprintf(stderr,"WARNING: No rotational information set in file %s. Defaulting to %s = %s.\n",cFile,options[OPT_ROTRATE].cName,options[OPT_ROTRATE].cDefault);
    body[iBody].dRotRate = options[OPT_ROTRATE].dDefault;
    return;
  }

  /* dRotRate is the master parameter, so set it. */

  if (options[OPT_ROTPER].iLine[iFileNum] >= 0)
    body[iBody].dRotRate = fdPerToFreq(body[iBody].dRotPer);
  if (options[OPT_ROTVEL].iLine[iFileNum] >= 0)
    body[iBody].dRotRate = fdRadiusRotVelToFreq(body[iBody].dRotVel,body[iBody].dRadius);
}

void VerifyInterior(BODY *body,OPTIONS *options,int iBody) {
  // Is this OK? XXX
  if (!body[iBody].bThermint) {
    body[iBody].dK2Man = 0;
    body[iBody].dImk2Man = 0;
  }
}

/*
 *
 * System-level verify routines
 *
 */

/*! Verify Initial angular momentum, energy for conservation checks.  Initialize
 *  here so anything that changes E, J is monitored to ensure conservation */
void VerifySystem(BODY *body,UPDATE *update,CONTROL *control,SYSTEM *system,OPTIONS *options) {
  int iBody;

  // Initially no lost angular momentum, energy
  // Set to dTINY, not 0 since these are integrated
  // and it being 0 can mess up the time step
  for(iBody = 0; iBody < control->Evolve.iNumBodies; iBody++)
  {
    body[iBody].dLostAngMom = 0.0;
    body[iBody].dLostEng = 0.0;
  }

  // Compute initial total angular momentum
  system->dTotAngMomInit = fdTotAngMom(body,control,system);
  system->dTotAngMom = system->dTotAngMomInit;

  // Compute initial total energy
  system->dTotEnInit = fdTotEnergy(body,control,system);
  system->dTotEn = system->dTotEnInit;
}

void fnNullDerivatives(BODY *body,EVOLVE *evolve,MODULE *module,UPDATE *update,fnUpdateVariable ***fnUpdate) {
  int iBody,iVar,iEqn,iNumBodies,iNumVars,iNumEqns; // Dummy counting variables

  iNumBodies = evolve->iNumBodies;
  for (iBody=0;iBody<iNumBodies;iBody++) {
    if (update[iBody].iNumVars > 0) {
      iNumVars = update[iBody].iNumVars;
      for (iVar=0;iVar<iNumVars;iVar++) {
        iNumEqns = update[iBody].iNumEqns[iVar];
        for (iEqn=0;iEqn<iNumEqns;iEqn++) {
          fnUpdate[iBody][iVar][iEqn] = &fndUpdateFunctionTiny;
        }
      }
    }
  }
}

/*
 *
 * Master Verify subroutine
 *
 */

void VerifyOptions(BODY *body,CONTROL *control,FILES *files,MODULE *module,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnIntegrate *fnOneStep,fnUpdateVariable ****fnUpdate) {
  int iBody,iModule;

  VerifyNames(body,control,options);

  VerifyIntegration(body,control,files,options,system,fnOneStep);
  InitializeControlEvolve(control,module,update);

  // Default to no orbiting bodies
  control->bOrbiters = 0;

  /* First we must determine all the primary variables. The user may not
     input them, but instead a redundant variable. Yet these need to be
     defined before we can call InitializeUpdate. */

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    /* First pass NumModules from MODULE -> CONTROL->EVOLVE */
    control->Evolve.iNumModules[iBody] = module->iNumModules[iBody];

    // If any body has an orbit related module initialized, we have orbiters!
    if(body[iBody].bEqtide || body[iBody].bDistOrb || body[iBody].bPoise || body[iBody].bAtmEsc ||
        body[iBody].bGalHabit || body[iBody].bSpiNBody) {
      control->bOrbiters = 1;
    }

    /* Must verify density first: RotVel requires a radius in VerifyRotation */
    VerifyMassRad(&body[iBody],control,options,iBody,files->Infile[iBody].cIn,control->Io.iVerbose);

    for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
      // Must initialize entire body struct before verifying modules
      module->fnInitializeBody[iBody][iModule](body,control,update,iBody,iModule);
    /* Verify Modules */

    VerifyRotationGeneral(body,options,iBody,control->Io.iVerbose,files->Infile[iBody+1].cIn);

    // If any bodies orbit, make sure they do so properly!
    if(control->bOrbiters) {
      VerifyOrbit(body,*files,options,control,iBody,control->Io.iVerbose);
    }

  }

  InitializeUpdate(body,control,module,update,fnUpdate);
  InitializeHalts(control,module);

  VerifyHalts(body,control,module,options);

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    // Now we can verify the modules
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++) {
      module->fnVerify[iBody][iModule](body,control,files,options,output,system,update,iBody,iModule);
    }

    VerifyInterior(body,options,iBody);
    // Verify multi-module couplings
    VerifyModuleMulti(body,update,control,files,module,options,iBody,fnUpdate);

    for (iModule=0;iModule<module->iNumManageDerivs[iBody];iModule++) {
      module->fnAssignDerivatives[iBody][iModule](body,&(control->Evolve),update,*fnUpdate,iBody);
    }

    /* Must allocate memory in control struct for all perturbing bodies */
    if (control->Evolve.iOneStep == RUNGEKUTTA) {
      InitializeUpdateBodyPerts(control,update,iBody);
      InitializeUpdateTmpBody(body,control,module,update,iBody);
    }

  }

  // Verify system (initialize angular momentum, energy) now that everything else
  // has been verified
  VerifySystem(body,update,control,system,options);

}
