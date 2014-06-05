/************************** UTIL.C ******************/
/*
 * Rory Barnes, Wed May  7 15:05:50 PDT 2014
 *
 * This file contains random but useful subroutines,
 * including unit conversions,and I/O.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"

void WriteHelpOption(OPTIONS options) {
  char ESC=27;

  if (memcmp(options.cName,"null",4)) {
    printf("%c[1m%s%c[0m (",ESC,options.cName,ESC);
    if (options.iType == 0)
      printf("Bool");
    else if (options.iType == 1) 
      printf("Int");
    else if (options.iType == 2)
      printf("Double");
    else if (options.iType == 3)
      printf("String");
    else if (options.iType == 4)
      printf("Array");
    printf(") -- %s (Default = %s).\n",options.cDescr,options.cDefault);
  }
}

void WriteHelpOutput(OUTPUT output) {
  char ESC=27;

  if (memcmp(output.cName,"null",4)) {
    if (output.bNeg == 0)
      printf("%c[1m[-]%c[0m",ESC,ESC);
      printf("%c[1m%s%c[0m -- %s.",ESC,output.cName,ESC,output.cDescr);
      if (output.bNeg == 0)
	printf(" [%s]",output.cNeg);
      printf("\n");
    }
}

void HelpOptions(OPTIONS *options) {
  int i;

  printf("----- Input Options -----\n\n");
  for (i=0;i<OPTEND;i++) 
    WriteHelpOption(options[i]);
  
  /* Needs to be vectorized VPL */
  HelpOptionsEqtide(options);
}
  
void HelpOutput(OUTPUT *output) {
  int i;

  for (i=0;i<NUMOUT;i++) 
    WriteHelpOutput(output[i]);

  /* Needs to be vectorized VPL */
  HelpOutputEqtide(output);
}

void Help(OUTPUT *output,OPTIONS *options,char exe[]) {
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
}

/*
 * File Control
 */

void LineExit(char cFile[],int iLine) {
  fprintf(stderr,"\t%s: Line %d\n",cFile,iLine);
  exit(EXIT_INPUT);
}

/*
 * Formatting
 */

char *sLower(char str[]) {
  int i;
  for (i=0;str[i];i++)
  str[i] = tolower(str[i]);
    
  return str;
}

void fprintd(FILE *fp,double x,int iExp,int iDig) {
    double min,max;

    max=pow(10,iExp);
    min=pow(10,-iExp);
    if ((fabs(x) > max || fabs(x) < min) && x != 0) {
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
double fdLengthUnit(int iType) {
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

void fvLengthUnit(int iType,char cUnit[]) {
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
double fdTimeUnit(int iType) {
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

void fvTimeUnit(int iType,char cUnit[]) {
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

double fdMassUnit(int iType) {
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

void fvMassUnit(int iType,char cUnit[]) {
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

double fdAngleUnit(int iType) {
  if (iType == 0)
     return 1;
  else if (iType == 1)
     return DEGRAD;
  else {
    fprintf(stderr,"ERROR: Unknown Angle type %d\n.",iType);
    exit(EXIT_UNITS);
  }
}

void fvAngleUnit(int iType,char cUnit[]) {
  if (iType == 0)
    sprintf(cUnit,"rad");
  else if (iType == 1)
    sprintf(cUnit,"deg");
  else {
    fprintf(stderr,"ERROR: Unknown Angle type %d\n.",iType);
    exit(EXIT_UNITS);
  }
}

void fvAngMomUnit(UNITS *units,char cUnit[]) {
  char cTmp[OPTLEN];
  
  fvMassUnit(units->iMass,cUnit);
  fvLengthUnit(units->iLength,cTmp);
  strcat(cUnit,"*");
  strcat(cUnit,cTmp);
  strcat(cUnit,"^2/");
  fvTimeUnit(units->iTime,cTmp);
  strcat(cUnit,cTmp);
}	

void fvVelUnit(UNITS *units,char cUnit[]) {
  char cTmp[OPTLEN];
  
  fvLengthUnit(units->iLength,cUnit);
  strcat(cUnit,"/");
  fvTimeUnit(units->iTime,cTmp);
  strcat(cUnit,cTmp);
}

void fvRateUnit(int iType,char cUnit[]) {
  char cTmp[OPTLEN];
  
  sprintf(cUnit,"/");
  fvTimeUnit(iType,cTmp);
  strcat(cUnit,cTmp);
}

void fvAngRateUnit(UNITS *units,char cUnit[]) {
  char cTmp[OPTLEN];
  
  fvAngleUnit(units->iAngle,cUnit);
  strcat(cUnit,"/");
  fvTimeUnit(units->iTime,cTmp);
  strcat(cUnit,cTmp);
}

void fvEnergyUnit(UNITS *units,char cUnit[]) {
  char cTmp[OPTLEN];

  fvMassUnit(units->iMass,cUnit);
  strcat(cUnit,"*");
  fvLengthUnit(units->iLength,cTmp);
  strcat(cUnit,cTmp);
  strcat(cUnit,"^2/");
  fvTimeUnit(units->iTime,cTmp);
  strcat(cUnit,cTmp);
  strcat(cUnit,"^2");
}

double fdEnergyUnit(int iTime,int iMass,int iLength) {
  return fdMassUnit(iMass)*fdLengthUnit(iLength)*fdLengthUnit(iLength)/(fdTimeUnit(iTime)*fdTimeUnit(iTime));
}

void fvEnergyFluxUnit(UNITS *units,char cUnit[]) {
  char cTmp[OPTLEN];

  fvEnergyUnit(units,cUnit);
  strcat(cUnit,"/(");
  fvLengthUnit(units->iLength,cTmp);
  strcat(cUnit,cTmp);
  strcat(cUnit,"^2*");
  fvTimeUnit(units->iTime,cTmp);
  strcat(cUnit,cTmp);
  strcat(cUnit,")");
}

double fdEnergyFluxUnit(int iTime,int iMass,int iLength) {
  double dTmp;

  dTmp=fdEnergyUnit(iTime,iMass,iLength);  
  return dTmp/(fdLengthUnit(iLength)*fdLengthUnit(iLength)*fdTimeUnit(iTime));
}	

