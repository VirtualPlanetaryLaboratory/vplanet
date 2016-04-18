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

void VerifyOrbit(BODY *body,FILES files,OPTIONS *options,int iBody,int iVerbose) {
  int iFile=iBody+1;
  double dSemi=0,dMeanMotion=0,dPeriod=0;

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
      body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[iBody].dMass);
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
  
  if (dMeanMotion > 0)
    body[iBody].dSemi = fdMeanMotionToSemi(body[0].dMass,body[iBody].dMass,dMeanMotion);
  if (dPeriod > 0)
    body[iBody].dSemi = fdPeriodToSemi(dPeriod,(body[0].dMass+body[iBody].dMass));
  if (dSemi > 0)
    body[iBody].dSemi = dSemi;

  if (dMeanMotion == 0)
    body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[iBody].dMass);
  //XXX Initialize central body parameters.
  
  
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
  if (options[OPT_MASS].iLine[iBody] > -1 && options[OPT_RADIUS].iLine[iBody] > -1 && options[OPT_DENSITY].iLine[iBody] > -1) {
    VerifyTripleExit(options[OPT_MASS].cName,options[OPT_RADIUS].cName,options[OPT_DENSITY].cName,options[OPT_MASS].iLine[iBody],options[OPT_RADIUS].iLine[iBody],options[OPT_DENSITY].iLine[iBody],cFile,iVerbose);
    exit(EXIT_INPUT);
  }

  /* Was mass set? */
  if (options[OPT_MASS].iLine[iBody] > -1) {
  
    /* Can only set 1 other */
    if (options[OPT_RADIUS].iLine[iBody] > -1 && options[OPT_MASSRAD].iLine[iBody] > -1) 
      VerifyTwoOfThreeExit(options[OPT_MASS].cName,options[OPT_RADIUS].cName,options[OPT_MASSRAD].cName,options[OPT_MASS].iLine[iBody],options[OPT_RADIUS].iLine[iBody],options[OPT_MASSRAD].iLine[iBody],cFile,iVerbose);

    if (options[OPT_RADIUS].iLine[iBody] > -1 && options[OPT_DENSITY].iLine[iBody] > -1) 
      VerifyTwoOfThreeExit(options[OPT_MASS].cName,options[OPT_RADIUS].cName,options[OPT_DENSITY].cName,options[OPT_MASS].iLine[iBody],options[OPT_RADIUS].iLine[iBody],options[OPT_DENSITY].iLine[iBody],cFile,iVerbose);

    if (options[OPT_MASSRAD].iLine[iBody] > -1 && options[OPT_DENSITY].iLine[iBody] > -1) 
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

/*
 *
 * Master Verify subroutine
 *
 */

void VerifyOptions(BODY *body,CONTROL *control,FILES *files,MODULE *module,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnIntegrate *fnOneStep,fnUpdateVariable ****fnUpdate) {
  int iBody,iModule;

  VerifyIntegration(body,control,files,options,system,fnOneStep);
  InitializeControlEvolve(control,module,update);

  /* First we must determine all the primary variables. The user may not
     input them, but instead a redundant variable. Yet these need to be
     defined before we can call InitializeUpdate. */


  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    /* First pass NumModules from MODULE -> CONTROL->EVOLVE */
    control->Evolve.iNumModules[iBody] = module->iNumModules[iBody];

    /* Must verify density first: RotVel requires a radius in VerifyRotation */
    VerifyMassRad(&body[iBody],control,options,iBody+1,files->Infile[iBody+1].cIn,control->Io.iVerbose);

    for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
      // Must initialize entire body struct before verifying modules
      module->fnInitializeBody[iBody][iModule](body,control,update,iBody,iModule);
    /* Verify Modules */

    VerifyRotationGeneral(body,options,iBody,control->Io.iVerbose,files->Infile[iBody+1].cIn);

    /* XXX Only module reference in file -- guess we need VerifyOrbitModule */
    if ((iBody > 0 && body[iBody].bEqtide) || (iBody>0 && body[iBody].bPoise)) {
      VerifyOrbit(body,*files,options,iBody,control->Io.iVerbose);
    }

  }

  InitializeUpdate(body,control,module,update,fnUpdate);
  InitializeHalts(control,module);
  InitializeOutputFunctions(module,output,control->Evolve.iNumBodies);

  VerifyHalts(body,control,module,options);

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++) {
      module->fnFinalizeOutputFunction[iBody][iModule](output,iBody,iModule);
    }
  }

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    // Now we can verify the modules
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
      module->fnVerify[iBody][iModule](body,control,files,options,output,system,update,*fnUpdate,iBody,iModule);
    
    VerifyModuleMulti(body,control,files,module,options,iBody);

    /* Must allocate memory in control struct for all perturbing bodies */
    if (control->Evolve.iOneStep == RUNGEKUTTA) {
      InitializeUpdateBodyPerts(control,update,iBody);
      InitializeUpdateTmpBody(body,control,module,update,iBody);
    }

  }
}

