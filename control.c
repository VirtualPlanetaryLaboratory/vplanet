/********************** CONTROL.C ***********************/
/*
 * Rory Barnes, Wed Jan  7 15:48:08 PST 2015
 *
 * This subroutines deal with control, including I/O, units, and files.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "vplanet.h"
/*
 * Struct Initialization
 */

void InitializeControl(CONTROL *control,MODULE *module) {
  int iBody,iModule;

  control->iMassRad = malloc(control->Evolve.iNumBodies*sizeof(int));
  control->fnForceBehavior = malloc(control->Evolve.iNumBodies*sizeof(fnForceBehaviorModule*));
  
  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    control->fnForceBehavior[iBody] = malloc(module->iNumModules[iBody]*sizeof(fnForceBehaviorModule));
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
      module->fnInitializeControl[iBody][iModule](control);
  }
}

/* This is called from Verify, after the update matrix has been
   initialized. */
void InitializeControlEvolve(CONTROL *control,MODULE *module,UPDATE *update) {
  int iBody,iSubStep;

  control->Evolve.fnAuxProps = malloc(control->Evolve.iNumBodies*sizeof(fnAuxPropsModule*));
  control->Evolve.fnBodyCopy = malloc(control->Evolve.iNumBodies*sizeof(fnBodyCopyModule*));
  control->Evolve.iNumModules = malloc(control->Evolve.iNumBodies*sizeof(int));
  control->Evolve.tmpUpdate = malloc(control->Evolve.iNumBodies*sizeof(UPDATE));

  control->Evolve.tmpBody = malloc(control->Evolve.iNumBodies*sizeof(BODY));
  control->Evolve.tmpUpdate = malloc(control->Evolve.iNumBodies*sizeof(UPDATE));

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    control->Evolve.fnAuxProps[iBody] = malloc(module->iNumModules[iBody]*sizeof(fnAuxPropsModule));
    control->Evolve.fnBodyCopy[iBody] = malloc(module->iNumModules[iBody]*sizeof(fnBodyCopyModule));
  }

  /* Currently this only matters for RK4 integration. This should
     be generalized for any integration method. */
  if (control->Evolve.iOneStep == RUNGEKUTTA) {
    control->Evolve.daDeriv = malloc(4*sizeof(double**));
    for (iSubStep=0;iSubStep<4;iSubStep++) {
      control->Evolve.daDeriv[iSubStep] = malloc(control->Evolve.iNumBodies*sizeof(double*));
    }
  }
}

/* 
 * Help functions 
 */

void WriteHelpOption(OPTIONS *options) {
  char ESC=27;

  if (memcmp(options->cName,"null",4)) {
    printf("%c[1m%s%c[0m (",ESC,options->cName,ESC);
    if (options->iType == 0)
      printf("Bool");
    else if (options->iType == 1) 
      printf("Int");
    else if (options->iType == 2)
      printf("Double");
    else if (options->iType == 3)
      printf("String");
    else if (options->iType == 4)
      printf("Array");
    printf(") -- %s (Default = %s).\n",options->cDescr,options->cDefault);
  }
}

void WriteHelpOutput(OUTPUT *output) {
  char ESC=27;

  if (memcmp(output->cName,"null",4)) {
    if (output->bNeg == 1)
      printf("%c[1m[-]%c[0m",ESC,ESC);
    printf("%c[1m%s%c[0m -- %s.",ESC,output->cName,ESC,output->cDescr);
    if (output->bNeg == 1)
      printf(" [%s]",output->cNeg);
    printf("\n");
    }
}

void HelpOptions(OPTIONS *options) {
  int iOpt,iModule;
  
  printf("----- Input Options -----\n\n");
  for (iOpt=0;iOpt<MODULEOPTEND;iOpt++) 
    WriteHelpOption(&options[iOpt]);
  
}

void HelpOutput(OUTPUT *output) {
  int iOut,iModule;
  
  for (iOut=0;iOut<MODULEOUTEND;iOut++) 
    WriteHelpOutput(&output[iOut]);
  
}

void Help(OPTIONS *options,OUTPUT *output,char exe[]) {
  int i;
  char ESC=27; 

  printf("\n\t\t\tHelp Message for %s\n",exe);
  printf("\nWritten by Rory Barnes\n");
  printf("\n%s is a general purpose tidal integrator. It takes ",exe);
  printf("an input file consisting of options and initial conditions ");
  printf("and simulates tidal evolution, along with other secondary ");
  printf("parameters, forward and/or backward in time. This help ");
  printf("describes the basics of the input file.\n\n");
  
  printf("----- Command Line Options -----\n\n");
  printf("%c[1m-v, -verbose%c[0m -- Maximum verbosity, i.e. display all warnings and updates.\n",ESC,ESC);
  printf("%c[1m-q, -quiet%c[0m   -- No verbosity, i.e. nothing printed to device.\n",ESC,ESC);
  printf("%c[1m-h, -help%c[0m    -- Display this message.\n\n",ESC,ESC);
  
  printf("----- Input File -----\n\n");
  printf("Comments/White Space: Any characters to the right of a # sign ");
  printf("are treated as a comment and are ignored. All white space is ");
  printf("ignored.\n");
  printf("Options that take an array may span multiple lines if a $ is ");
  printf("placed at the end of the line.\n");
  printf("Options must be the first string on any line, and must be ");
  printf("written exactly as listed below.\n");
  printf("Arguments may have any format, and need only be unambiguous.\n\n");
  
  HelpOptions(options);
  
  printf("\n----- Output Options -----\n\n");
  printf("These options follow the argument %s.\n",options[OPT_OUTPUTORDER].cName);
  HelpOutput(output);

  exit(0);

}

/*
 * I/O
 */

void LineExit(char cFile[],int iLine) {
  fprintf(stderr,"\t%s: Line %d\n",cFile,iLine);
  exit(EXIT_INPUT);
}

char *sLower(char cString[]) {
  int iPos;
  for (iPos=0;cString[iPos];iPos++)
  cString[iPos] = tolower(cString[iPos]);
    
  return cString;
}

void fprintd(FILE *fp,double x,int iExp,int iDig) {
  double dMin,dMax;
  
  dMax=pow(10,iExp);
  dMin=pow(10,-iExp);
  if ((fabs(x) > dMax || fabs(x) < dMin) && x != 0) {
    if (iDig == 0) 
      fprintf(fp,"%.0e",x);
    if (iDig == 1) 
      fprintf(fp,"%.1e",x);
    if (iDig == 2) 
      fprintf(fp,"%.2e",x);
    if (iDig == 3) 
      fprintf(fp,"%.3e",x);
    if (iDig == 4) 
      fprintf(fp,"%.4e",x);
    if (iDig == 5) 
      fprintf(fp,"%.5e",x);
    if (iDig == 6) 
      fprintf(fp,"%.6e",x);
    if (iDig == 7) 
      fprintf(fp,"%.7e",x);
    if (iDig == 8) 
      fprintf(fp,"%.8e",x);
    if (iDig == 9) 
      fprintf(fp,"%.9e",x);
    if (iDig == 10) 
      fprintf(fp,"%.10e",x);
    if (iDig == 11) 
      fprintf(fp,"%.11e",x);
    if (iDig == 12) 
      fprintf(fp,"%.12e",x);
    if (iDig == 13) 
      fprintf(fp,"%.13e",x);
    if (iDig == 14) 
      fprintf(fp,"%.14e",x);
    if (iDig == 15) 
      fprintf(fp,"%.15e",x);
    if (iDig == 16) 
      fprintf(fp,"%.16e",x);
  } else {	
    if (iDig == 0) 
      fprintf(fp,"%.0lf",x);
    if (iDig == 1) 
      fprintf(fp,"%.1lf",x);
    if (iDig == 2) 
      fprintf(fp,"%.2lf",x);
    if (iDig == 3) 
      fprintf(fp,"%.3lf",x);
    if (iDig == 4) 
      fprintf(fp,"%.4lf",x);
    if (iDig == 5) 
      fprintf(fp,"%.5lf",x);
    if (iDig == 6) 
      fprintf(fp,"%.6lf",x);
    if (iDig == 7) 
      fprintf(fp,"%.7lf",x);
    if (iDig == 8) 
      fprintf(fp,"%.8lf",x);
    if (iDig == 9) 
      fprintf(fp,"%.9lf",x);
    if (iDig == 10) 
      fprintf(fp,"%.10lf",x);
    if (iDig == 11) 
      fprintf(fp,"%.11lf",x);
    if (iDig == 12) 
      fprintf(fp,"%.12lf",x);
    if (iDig == 13) 
      fprintf(fp,"%.13lf",x);
    if (iDig == 14) 
      fprintf(fp,"%.14lf",x);
    if (iDig == 15) 
      fprintf(fp,"%.15lf",x);
    if (iDig == 16) 
      fprintf(fp,"%.16lf",x);
  }
}

/* 
 * Unit Conversions
 */

/* Return proper length conversion */
double fdUnitsLength(int iType) {
  if (iType == 0)
    return 1;
  else if (iType == 1)
    return 100;
  else if (iType == 2)
    return 1e5;
  else if (iType == 3)
    return RSUN;
  else if (iType == 4)
    return REARTH;
  else if (iType == 5)
    return RJUP;
  else if (iType == 6)
    return AUCM;
  else {
    fprintf(stderr,"ERROR: Unknown iUnitLength %d.\n",iType);
    exit(EXIT_UNITS);      
  }
}

void fsUnitsLength(int iType,char cUnit[]) {
  if (iType == 0)
    sprintf(cUnit,"cm");
  else if (iType == 1)
    sprintf(cUnit,"m");
  else if (iType == 2)
    sprintf(cUnit,"km");
  else if (iType == 3)
    sprintf(cUnit,"solar");
  else if (iType == 4)
    sprintf(cUnit,"Earth");
  else if (iType == 5)
    sprintf(cUnit,"Jupiter");
  else if (iType == 6)
    sprintf(cUnit,"AU");
  else {
    fprintf(stderr,"ERROR: Unknown iUnitLength %d.\n",iType);
    exit(EXIT_UNITS);      
  }
}

/* Return proper time conversion */
double fdUnitsTime(int iType) {
  if (iType == 0)
    return 1;
  else if (iType == 1)
    return DAYSEC;
  else if (iType == 2)
    return YEARSEC;
  else if (iType == 3)
    return 1e6*YEARSEC;
  else if (iType == 4)
    return 1e9*YEARSEC;
  else {
    fprintf(stderr,"ERROR: Unknown iUnitTime: %d.\n",iType);
    exit(EXIT_UNITS);      
  }
}

void fsUnitsTime(int iType,char cUnit[]) {
  if (iType == 0)
    sprintf(cUnit,"sec");
  else if (iType == 1)
    sprintf(cUnit,"day");
  else if (iType == 2)
    sprintf(cUnit,"year");
  else if (iType == 3)
    sprintf(cUnit,"Myr");
  else if (iType == 4)
    sprintf(cUnit,"Gyr");
  else {
    fprintf(stderr,"ERROR: Unknown iUnitTime: %d.\n",iType);
    exit(EXIT_UNITS);      
  }
}

double fdUnitsMass(int iType) {
  if (iType == 0)
    return 1;
  else if (iType == 1)
    return 100;
  else if (iType == 2)
    return MSUN;
  else if (iType == 3)
    return MEARTH;
  else if (iType == 4)
    return MJUP;
  else if (iType == 5)
    return MNEP;
  else {
    fprintf(stderr,"ERROR: Unknown iUnitMass: %d.\n",iType);
    exit(EXIT_UNITS);      
  }
}

void fsUnitsMass(int iType,char cUnit[]) {
  if (iType == 0)
    sprintf(cUnit,"grams");
  else if (iType == 1)
    sprintf(cUnit,"kg");
  else if (iType == 2)
    sprintf(cUnit,"solar");
  else if (iType == 3)
    sprintf(cUnit,"Earth");
  else if (iType == 4)
    sprintf(cUnit,"Jupiter");
  else if (iType == 5)
    sprintf(cUnit,"Neptune");
  else {
    fprintf(stderr,"ERROR: Unknown iUnitMass: %d.\n",iType);
    exit(EXIT_UNITS);      
  }
}

double fdUnitsAngle(int iType) {
  if (iType == 0)
     return 1;
  else if (iType == 1)
     return DEGRAD;
  else {
    fprintf(stderr,"ERROR: Unknown Angle type %d\n.",iType);
    exit(EXIT_UNITS);
  }
}

void fsUnitsAngle(int iType,char cUnit[]) {
  if (iType == 0)
    sprintf(cUnit,"rad");
  else if (iType == 1)
    sprintf(cUnit,"deg");
  else {
    fprintf(stderr,"ERROR: Unknown Angle type %d\n.",iType);
    exit(EXIT_UNITS);
  }
}

void fsUnitsAngMom(UNITS *units,char cUnit[]) {
  char cTmp[OPTLEN];
  
  fsUnitsMass(units->iMass,cUnit);
  fsUnitsLength(units->iLength,cTmp);
  strcat(cUnit,"*");
  strcat(cUnit,cTmp);
  strcat(cUnit,"^2/");
  fsUnitsTime(units->iTime,cTmp);
  strcat(cUnit,cTmp);
}	

void fsUnitsVel(UNITS *units,char cUnit[]) {
  char cTmp[OPTLEN];
  
  fsUnitsLength(units->iLength,cUnit);
  strcat(cUnit,"/");
  fsUnitsTime(units->iTime,cTmp);
  strcat(cUnit,cTmp);
}

void fsUnitsRate(int iType,char cUnit[]) {
  char cTmp[OPTLEN];
  
  sprintf(cUnit,"/");
  fsUnitsTime(iType,cTmp);
  strcat(cUnit,cTmp);
}

void fsUnitsAngRate(UNITS *units,char cUnit[]) {
  char cTmp[OPTLEN];
  
  fsUnitsAngle(units->iAngle,cUnit);
  strcat(cUnit,"/");
  fsUnitsTime(units->iTime,cTmp);
  strcat(cUnit,cTmp);
}

void fsUnitsEnergy(UNITS *units,char cUnit[]) {
  char cTmp[OPTLEN];

  fsUnitsMass(units->iMass,cUnit);
  strcat(cUnit,"*");
  fsUnitsLength(units->iLength,cTmp);
  strcat(cUnit,cTmp);
  strcat(cUnit,"^2/");
  fsUnitsTime(units->iTime,cTmp);
  strcat(cUnit,cTmp);
  strcat(cUnit,"^2");
}

double fdUnitsEnergy(int iTime,int iMass,int iLength) {
  return fdUnitsMass(iMass)*fdUnitsLength(iLength)*fdUnitsLength(iLength)/(fdUnitsTime(iTime)*fdUnitsTime(iTime));
}

void fsUnitsPower(UNITS *units,char cUnit[]) {
  char cTmp[OPTLEN];

  fsUnitsMass(units->iMass,cUnit);
  strcat(cUnit,"*");
  fsUnitsLength(units->iLength,cTmp);
  strcat(cUnit,cTmp);
  strcat(cUnit,"^2/");
  fsUnitsTime(units->iTime,cTmp);
  strcat(cUnit,cTmp);
  strcat(cUnit,"^3");
}

double fdUnitsPower(int iTime,int iMass,int iLength) {
  return fdUnitsMass(iMass)*fdUnitsLength(iLength)*fdUnitsLength(iLength)/(fdUnitsTime(iTime)*fdUnitsTime(iTime)*fdUnitsTime(iTime));
}

void fsUnitsEnergyFlux(UNITS *units,char cUnit[]) {
  char cTmp[OPTLEN];

  fsUnitsEnergy(units,cUnit);
  strcat(cUnit,"/(");
  fsUnitsLength(units->iLength,cTmp);
  strcat(cUnit,cTmp);
  strcat(cUnit,"^2*");
  fsUnitsTime(units->iTime,cTmp);
  strcat(cUnit,cTmp);
  strcat(cUnit,")");
}

double fdUnitsEnergyFlux(int iTime,int iMass,int iLength) {
  double dTmp;

  dTmp=fdUnitsPower(iTime,iMass,iLength);  
  return dTmp/(fdUnitsLength(iLength)*fdUnitsLength(iLength));
}	

double fdUnitsTemp(double dTemp,int iOldType,int iNewType) {
  if (iOldType == 0) {
    if (iNewType == 1) {
      /* Kelvin -> Celsius */
      return dTemp - 273;
    } else if (iNewType == 2) {
      /* Kelvin to Farenheit */
      return (dTemp - 273)*1.8 + 32;
    } else if (iNewType == 0) {
      return dTemp;
    } else {
      fprintf(stderr,"ERROR: Unknown Temperature type %d.\n",iNewType);
      exit(EXIT_UNITS);
    }
  } else if (iOldType == 1) {
    if (iNewType == 0) {
      /* Celsius -> Kelvin */
      return dTemp + 273;
    } else if (iNewType == 2) {
      /* Celsius -> Farenheit */
      return (1.8*dTemp) + 32;
    } else if (iNewType == 1) {
      return dTemp;
    } else {
      fprintf(stderr,"ERROR: Unknown Temperature type %d.\n",iNewType);
      exit(EXIT_UNITS);
    }
  } else if (iOldType == 2) {
    if (iNewType == 0) {
      /* Farenheit -> Kelvin */
      return 5/9*(dTemp - 32) + 273;
    } else if (iNewType == 1) {
      /* Farenheit -> Celsius */
      return 5/9*(dTemp - 32);
    } else if (iNewType == 2) {
      return dTemp;
    } else {
      fprintf(stderr,"ERROR: Unknown Temperature type %d.\n",iNewType);
      exit(EXIT_UNITS);
    }
  } else {
    fprintf(stderr,"ERROR: Unknown Temperature type %d.\n",iOldType);
    exit(EXIT_UNITS);
  }
}

void fsUnitsTemp(int iType,char cUnit[]) {
  if (iType == 0)
    sprintf(cUnit,"K");
  else if (iType == 1)
    sprintf(cUnit,"C");
  else if (iType == 2)
    sprintf(cUnit,"F");
  else {
    fprintf(stderr,"ERROR: Unknown iUnitTemp %d.\n",iType);
    exit(EXIT_UNITS);      
  }
}

void fsUnitsTempRate(int iType,char cUnit[]) {
  if (iType == 0)
    sprintf(cUnit,"K/s");
  else if (iType == 1)
    sprintf(cUnit,"C/s");
  else if (iType == 2)
    sprintf(cUnit,"F/s");
  else {
    fprintf(stderr,"ERROR: Unknown iUnitTempRate %d.\n",iType);
    exit(EXIT_UNITS);      
  }
}

/*
 * FILES struct functions 
 */

void InfileCopy(INFILE *dest,INFILE *src) {
  int iLine;

  strcpy(dest->cIn,src->cIn);
  for (iLine=0;iLine<src->iNumLines;iLine++)
    dest->bLineOK[iLine] = src->bLineOK[iLine];

  /* PHOTOCHEM fields */
  strcpy(dest->cSpecies,src->cSpecies);
  strcpy(dest->cReactions,src->cReactions);
}
