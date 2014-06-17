/************************** VERIFY.C ***********************/
/*
 * Rory Barnes, Wed May  7 16:53:43 PDT 2014
 *
 * Validate the options. This will become a mess!
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"

/*
 * Exit Calls
 */

int bFileExists(const char *filename){
  FILE *file;
  if ((file = fopen(filename, "r")))
    {
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

void DoubleLineExit(char cFile1[],char cFile2[],int iLine1,int iLine2) {
  fprintf(stderr,"\tFile: %s, Line: %d.\n",cFile1,iLine1);
  fprintf(stderr,"\tFile: %s, Line: %d.\n",cFile2,iLine2);
  exit(EXIT_INPUT);
}

void VerifyOrbitExit(char cName1[],char cName2[],char cFile1[],char cFile2[],int iLine1,int iLine2,int iVerbose) {

  if (iVerbose >= VERBERR)
    fprintf(stderr,"ERROR: Cannot set both %s and %s.\n",cName1,cName2);
  fprintf(stderr,"\tFile: %s, Line: %d.\n",cFile1,iLine1);
  fprintf(stderr,"\tFile: %s, Line: %d.\n",cFile2,iLine2);
  exit(EXIT_INPUT);
}

void VerifyBodyExit(char cName1[],char cName2[],char cFile[],int iLine1,int iLine2,int iVerbose) {

  if (iVerbose >= VERBERR)
    fprintf(stderr,"ERROR: Cannot set both %s and %s in same file.\n",cName1,cName2);
  fprintf(stderr,"\tFile: %s, Lines: %d and %d\n",cFile,iLine1,iLine2);
  exit(EXIT_INPUT);
}

void TripleLineExit(char cFile[],int iLine1,int iLine2,int iLine3) {
  fprintf(stderr,"\tFile: %s, Lines: %d, %d and %d.\n",cFile,iLine1,iLine2,iLine3);
    exit(EXIT_INPUT);
}

/* Do we need both these? */
void VerifyTripleExit(char cName1[],char cName2[],char cName3[],int iLine1,int iLine2,int iLine3,char cFile[],int iVerbose) {

  if (iVerbose >= VERBERR) 
    fprintf(stderr,"ERROR: Cannot set %s, %s, and %s simultaneously.\n",cName1,cName2,cName3);
  TripleLineExit(cFile,iLine1,iLine2,iLine3);
}

void VerifyTwoOfThreeExit(char cName1[],char cName2[],char cName3[],int iLine1,int iLine2,int iLine3,char cFile[],int iVerbose) {

  if (iVerbose >= VERBERR) 
    fprintf(stderr,"ERROR: Can only set 2 of %s, %s, and %s.\n",cName1,cName2,cName3);
  TripleLineExit(cFile,iLine1,iLine2,iLine3);
}

/*
 *
 * Verify Orbit
 *
 */

void VerifyOrbit(BODY *body,OPTIONS *options,FILES files,int iVerbose) {
  /* Tricky because these parameters can appear in any file */
  /* But, these parameters cannot be duplicated in VPLANET, so the only 
     problem is if they are set in different files */

  int i;
  double dSemi=0,dMeanMotion=0,dPeriod=0;
  int iSemi=-1,iMeanMotion=-1,iPeriod=-1;

  /* !!!!! ------ Semi IS ALWAYS CORRECT AND IN BODY[1] ------- !!!!!! */

  /* In options.c, these parameters were placed in body[1]. */
  for (i=0;i<files.iNumInputs;i++) {
    if (options[OPT_ORBSEMI].iLine[i] > -1 && options[OPT_ORBMEANMOTION].iLine[i] == -1 && options[OPT_ORBPER].iLine[i] == -1) {
      dSemi = body[1].dSemi;
      iSemi = i;
    }
    if (options[OPT_ORBSEMI].iLine[i] == -1 && options[OPT_ORBMEANMOTION].iLine[i] > -1 && options[OPT_ORBPER].iLine[i] == -1) {
      dMeanMotion = body[1].dMeanMotion;
      iMeanMotion = i;
    }
    if (options[OPT_ORBSEMI].iLine[i] == -1 && options[OPT_ORBMEANMOTION].iLine[i] == -1 && options[OPT_ORBPER].iLine[i] > -1) {
      dPeriod = body[1].dOrbPeriod;
      iPeriod = i;
    }
  }

  /* Was Semi set and nothing else? */
  if (dSemi > 0 && dMeanMotion == 0 && dPeriod == 0) 
    return;

  /* Was anything set? */
  if (dSemi == 0 && dMeanMotion == 0 && dPeriod == 0) {
    fprintf(stderr,"ERROR: Must set one of %s, %s or %s.\n",options[OPT_ORBSEMI].cName,options[OPT_ORBMEANMOTION].cName,options[OPT_ORBPER].cName);
    exit(EXIT_INPUT);
  }

  /* If Semi set, was anything else? */
  if (iSemi > -1) {
    if (iMeanMotion > -1)
      VerifyOrbitExit(options[OPT_ORBSEMI].cName,options[OPT_ORBMEANMOTION].cName,files.Infile[iSemi].cIn,files.Infile[iMeanMotion].cIn,options[OPT_ORBSEMI].iLine[iSemi],options[OPT_ORBMEANMOTION].iLine[iMeanMotion],iVerbose);

    if (iPeriod > -1)
      VerifyOrbitExit(options[OPT_ORBSEMI].cName,options[OPT_ORBPER].cName,files.Infile[iSemi].cIn,files.Infile[iPeriod].cIn,options[OPT_ORBSEMI].iLine[iSemi],options[OPT_ORBPER].iLine[iPeriod],iVerbose);

  }

  /* Were MeanMotion and OrbPeriod both set? */
  if (iPeriod > -1 && iMeanMotion > -1)
      VerifyOrbitExit(options[OPT_ORBMEANMOTION].cName,options[OPT_ORBPER].cName,files.Infile[iMeanMotion].cIn,files.Infile[iPeriod].cIn,options[OPT_ORBMEANMOTION].iLine[iMeanMotion],options[OPT_ORBPER].iLine[iPeriod],iVerbose);

  /* Only one option set */

  if (iMeanMotion > -1)
    body[1].dSemi = fdMeanMotionToSemi(body[0].dMass,body[1].dMass,dMeanMotion);
  if (iPeriod > -1)
    body[1].dSemi = fdPeriodToSemi(dPeriod,(body[0].dMass+body[1].dMass));
  if (iSemi > -1)
    body[1].dSemi = dSemi;
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

void VerifyIntegration(FILES *files,CONTROL *control,SYSTEM *system,OPTIONS *options,BODY *body,fnIntegrate *fnOneStep) {
  int i,iBack,iForw;
  char cTmp[OPTLEN];

  /* Were both Forward and Backward Set? */
  if (control->Integr.bDoBackward && control->Integr.bDoForward) {
    fprintf(stderr,"ERROR: Both %s and %s set. Only one is allowed.\n",options[OPT_BACK].cName,options[OPT_FORW].cName);
    if (options[OPT_BACK].cFile[0] > 0)
      iBack = 0;
    else if (options[OPT_BACK].cFile[1] > 0)
      iBack = 1;
    else if (options[OPT_BACK].cFile[2] > 0)
      iBack = 2;
    else {
      fprintf(stderr,"ERROR: Cannot determine location of %s.\n",options[OPT_BACK].cName);
      exit(EXIT_INPUT);
    }
    if (options[OPT_FORW].cFile[0] > 0)
      iForw = 0;
    else if (options[OPT_FORW].cFile[1] > 0)
      iForw = 1;
    else if (options[OPT_FORW].cFile[2] > 0)
      iForw = 2;
    else {
      fprintf(stderr,"ERROR: Cannot determine location of %s.\n",options[OPT_FORW].cName);
      exit(EXIT_INPUT);
    }
    DoubleLineExit(options[OPT_BACK].cFile[iBack],options[OPT_FORW].cFile[iForw],options[OPT_BACK].iLine[iBack],options[OPT_FORW].iLine[iForw]);
  }

  /* Fix backward output file */
  if (control->Integr.bDoBackward) {
    for (i=1;i<3;i++) {
      if (options[OPT_OUTFILE].iLine[i] == -1) {
	sprintf(files->Outfile[i-1].cOut,"%s.%s.backward",system->cName,body[i-1].cName);
	if (control->iVerbose >= VERBINPUT) 
	  fprintf(stderr,"WARNING: %s not set, defaulting to %s.\n",options[OPT_OUTFILE].cName,files->Outfile[i-1].cOut);
      }
    }
  }

  /* Fix forward output file */
  if (control->Integr.bDoForward) {
    for (i=1;i<3;i++) {
      if (options[OPT_OUTFILE].iLine[i] == -1) {
	sprintf(files->Outfile[i-1].cOut,"%s.%s.forward",system->cName,body[i-1].cName);
	if (control->iVerbose >= VERBINPUT) 
	  fprintf(stderr,"WARNING: %s not set, defaulting to %s.\n",options[OPT_OUTFILE].cName,files->Outfile[i-1].cOut);
      }
    }
  }

  /* Check for file existence */
  for (i=0;i<2;i++) {
    if (bFileExists(files->Outfile[i].cOut)) {
      if (!control->bOverwrite) {
	OverwriteExit(options[OPT_OVERWRITE].cName,files->Outfile[i].cOut);
      } 
      if (control->iVerbose >= VERBINPUT) 
	fprintf(stderr,"WARNING: %s exists.\n",files->Outfile[i].cOut);
      unlink(files->Outfile[i].cOut);
    }
  }

  /* Was DoBackward or DoForward NOT set? */
  if (!control->Integr.bDoBackward && !control->Integr.bDoForward) {
    for (i=0;i<3;i++) {
      if (options[OPT_ETA].iLine[i] > -1) 
	IntegrationWarning(options[OPT_ETA].cName,options[OPT_BACK].cName,options[OPT_FORW].cName,options[OPT_ETA].cFile[i],options[OPT_ETA].iLine[i],control->iVerbose);

      if (options[OPT_OUTPUTTIME].iLine[i] > -1) 
	IntegrationWarning(options[OPT_OUTPUTTIME].cName,options[OPT_BACK].cName,options[OPT_FORW].cName,options[OPT_OUTPUTTIME].cFile[i],options[OPT_OUTPUTTIME].iLine[i],control->iVerbose);

      if (options[OPT_STOPTIME].iLine[i] > -1) 
	IntegrationWarning(options[OPT_STOPTIME].cName,options[OPT_BACK].cName,options[OPT_FORW].cName,options[OPT_STOPTIME].cFile[i],options[OPT_STOPTIME].iLine[i],control->iVerbose);

      if (options[OPT_TIMESTEP].iLine[i] > -1) 
	IntegrationWarning(options[OPT_TIMESTEP].cName,options[OPT_BACK].cName,options[OPT_FORW].cName,options[OPT_TIMESTEP].cFile[i],options[OPT_TIMESTEP].iLine[i],control->iVerbose);

      if (options[OPT_VARDT].iLine[i] > -1) 
	IntegrationWarning(options[OPT_VARDT].cName,options[OPT_BACK].cName,options[OPT_FORW].cName,options[OPT_VARDT].cFile[i],options[OPT_VARDT].iLine[i],control->iVerbose);

      if (options[OPT_OUTPUTORDER].iLine[i] > -1) 
	IntegrationWarning(options[OPT_OUTPUTORDER].cName,options[OPT_BACK].cName,options[OPT_FORW].cName,options[OPT_OUTPUTORDER].cFile[i],options[OPT_OUTPUTORDER].iLine[i],control->iVerbose);
    }
  }     

  if (control->iOneStep == EULER)
    *fnOneStep = &EulerStep;
  else if (control->iOneStep == RUNGEKUTTA)
    *fnOneStep = &RungeKutta4Step;
  else {
    /* Assign Default */
    strcpy(cTmp,options[OPT_INTEGRATIONMETHOD].cDefault);
    if (memcmp(sLower(cTmp),"e",1) == 0)
      *fnOneStep = &EulerStep;
    if (memcmp(sLower(cTmp),"r",1) == 0)
      *fnOneStep = &RungeKutta4Step;
  }

}

/*
 *
 * Mass-Radius Relationships
 *
 */

void VerifyMassRad(CONTROL *control,BODY *body,OPTIONS *options,int iBody,char cFile[],int iVerbose) {


  /* !!!!!! --- Mass and Radius ARE ALWAYS UPDATED AND CORRECT --- !!!!!! */

  /* First see if mass and radius and nothing else set, i.e. the user input the default parameters */
  if (options[OPT_MASS].iLine[iBody] > -1 && options[OPT_RADIUS].iLine[iBody] > -1 && options[OPT_DENSITY].iLine[iBody] == -1&& options[OPT_MASSRAD].iLine[iBody] == -1) 
    return;

  /* Was anything set> */
  if (options[OPT_MASS].iLine[iBody] == -1 && options[OPT_RADIUS].iLine[iBody] == -1 && options[OPT_DENSITY].iLine[iBody] == -1) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Must set at least one of %s, %s, and %s.\n",options[OPT_MASS].cName,options[OPT_RADIUS].cName,options[OPT_DENSITY].cName);
    exit(EXIT_INPUT);
  }

  /* Were all set? */
  if (options[OPT_MASS].iLine[iBody] >= -1 && options[OPT_RADIUS].iLine[iBody] >= -1 && options[OPT_DENSITY].iLine[iBody] >= -1) {
    VerifyTripleExit(options[OPT_MASS].cName,options[OPT_RADIUS].cName,options[OPT_DENSITY].cName,options[OPT_MASS].iLine[iBody],options[OPT_RADIUS].iLine[iBody],options[OPT_DENSITY].iLine[iBody],cFile,iVerbose);
    exit(EXIT_INPUT);
  }

  /* Was mass set? */
  if (options[OPT_MASS].iLine[iBody] > -1) {
    /* Can only set 1 other */
    if (options[OPT_RADIUS].iLine[iBody] >= -1 && options[OPT_MASSRAD].iLine[iBody] >= -1) 
      VerifyTwoOfThreeExit(options[OPT_MASS].cName,options[OPT_RADIUS].cName,options[OPT_MASSRAD].cName,options[OPT_MASS].iLine[iBody],options[OPT_RADIUS].iLine[iBody],options[OPT_MASSRAD].iLine[iBody],cFile,iVerbose);

    if (options[OPT_RADIUS].iLine[iBody] >= -1 && options[OPT_DENSITY].iLine[iBody] >= -1) 
      VerifyTwoOfThreeExit(options[OPT_MASS].cName,options[OPT_RADIUS].cName,options[OPT_DENSITY].cName,options[OPT_MASS].iLine[iBody],options[OPT_RADIUS].iLine[iBody],options[OPT_DENSITY].iLine[iBody],cFile,iVerbose);

    if (options[OPT_MASSRAD].iLine[iBody] >= -1 && options[OPT_DENSITY].iLine[iBody] >= -1) 
      VerifyTwoOfThreeExit(options[OPT_MASS].cName,options[OPT_MASSRAD].cName,options[OPT_DENSITY].cName,options[OPT_MASS].iLine[iBody],options[OPT_MASSRAD].iLine[iBody],options[OPT_DENSITY].iLine[iBody],cFile,iVerbose);
    
    /* Only Mass and something else set */
      

    if (options[OPT_RADIUS].iLine[iBody] > -1)
      /* Mass and radius were the only two set - Nothing to do */
	 return;
    if (options[OPT_DENSITY].iLine[iBody] > -1) 
      /* Must get radius from density */
      body->dRadius = fdDensityMassToRadius(body->dDensity,body->dMass);
    if (options[OPT_MASSRAD].iLine[iBody] > -1) 
      /* Must get radius from relationship */
      body->dRadius = fdMassToRad(body->dMass,control->iMassRad[iBody]);

    return;
  }

  /* Was radius set, but not mass? */
  if (options[OPT_RADIUS].iLine[iBody] > -1) {
    if (options[OPT_MASSRAD].iLine[iBody] >= -1 && options[OPT_DENSITY].iLine[iBody] >= -1) 
      VerifyTwoOfThreeExit(options[OPT_MASS].cName,options[OPT_MASSRAD].cName,options[OPT_DENSITY].cName,options[OPT_MASS].iLine[iBody],options[OPT_MASSRAD].iLine[iBody],options[OPT_DENSITY].iLine[iBody],cFile,iVerbose);

    /* Only Radius and something else set */

    if (options[OPT_MASSRAD].iLine[iBody] > -1) 
      /* Must get mass from relationship */
      body->dMass=fdRadToMass(body->dRadius,control->iMassRad[iBody]);
    if (options[OPT_DENSITY].iLine[iBody] > -1)
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
    /* Default value assigned in options, so nothing to do */
    return;
  }

  /* dRotRate is the master parameter, so set it. */

  if (options[OPT_ROTPER].iLine[iFileNum] >= 0) 
    body[iBody].dRotRate = fdPerToFreq(body[iBody].dRotPer);
  if (options[OPT_ROTVEL].iLine[iFileNum] >= 0) 
    body[iBody].dRotRate = fdRadiusRotVelToFreq(body[iBody].dRotVel,body[iBody].dRadius);
}

void VerifyRotation(CONTROL *control,BODY *body,OPTIONS *options,char cFile[],int iBody) {

  /* !!!!!! ------ RotRate IS ALWAYS UPDATED AND CORRECT -------- !!!!!! */

  VerifyRotationGeneral(body,options,iBody,control->iVerbose,cFile);

  /* Needs to be vectorized VPL */
  VerifyRotationEqtide(control,body,options,cFile,iBody);
}

void InitializeUpdate(UPDATE *update) {

    update->iType = malloc(update->iNum*sizeof(int));
    update->pdVar = malloc(update->iNum*sizeof(double*));
    update->dDeriv=malloc(update->iNum*sizeof(double*));
    update->iNumEqns=malloc(update->iNum*sizeof(double));

}

/*
 *
 * Master Verify subroutine
 *
 */

void VerifyOptions(OPTIONS *options,OUTPUT *output,BODY *body,VPLANET *vplanet,SYSTEM *system,FILES *files,CONTROL *control,fnIntegrate *fnOneStep,UPDATE *update,fnUpdateVariable ***fnUpdate) {
    int i;

    /* Must Vectorize VPL */
    VerifyEqtide(control,output,files,body,options,fnUpdate,update);
    VerifyOrbit(body,options,*files,control->iVerbose);

    VerifyIntegration(files,control,system,options,body,fnOneStep);

    for (i=0;i<control->iNumBodies;i++) {
      /* Must verify density first: RotVel requires a radius in VerifyRotation */
      VerifyMassRad(control,&body[i],options,i+1,files->Infile[i+1].cIn,control->iVerbose);
      VerifyRotation(control,body,options,files->Infile[i+1].cIn,i);

    }
}

