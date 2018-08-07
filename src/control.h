/**
   @file control.h

   @brief These subroutines deal with control, including I/O, units, and files.

   @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

   @date Jan 7 2015

 */

/* @cond DOXYGEN_OVERRIDE */

void BodyCopyNULL(BODY*,BODY*,int,int,int);

void InitializeControl(CONTROL*,MODULE*);
void InitializeControlEvolve(CONTROL*,MODULE*,UPDATE*);

void WriteHelpOption(OPTIONS*, int);
void WriteHelpOutput(OUTPUT*, int);
void HelpOptions(OPTIONS*, int);
void HelpOutput(OUTPUT*, int);
void Help(OPTIONS*,OUTPUT*,char[]);
void LongHelp(OPTIONS*,OUTPUT*,char[]);

void LineExit(char[],int);
char *sLower(char[]);
void fprintd(FILE*,double,int,int);

double fdUnitsLength(int);
double fdUnitsTime(int);
double fdUnitsMass(int);
double fdUnitsTemp(double,int,int);
double fdUnitsAngle(int);
double fdUnitsPower(int,int,int);
double fdUnitsEnergy(int,int,int);
double fdUnitsEnergyFlux(int,int,int);

void fsUnitsLength(int,char[]);
void fsUnitsTime(int,char[]);
void fsUnitsAngle(int,char[]);
void fsUnitsMass(int,char[]);
void fsUnitsTemp(int,char[]);
void fsUnitsTempRate(int,char[]);
void fsUnitsAngMom(UNITS*,char[]);
void fsUnitsDensity(UNITS*,char[]);
void fsUnitsVel(UNITS*,char[]);
void fsUnitsRate(int,char[]);
void fsUnitsAngRate(UNITS*,char[]);
void fsUnitsPower(UNITS*,char[]);
void fsUnitsEnergy(UNITS*,char[]);
void fsUnitsEnergyFlux(UNITS*,char[]);
void fsUnitsViscosity(UNITS*,char[]);

void InfileCopy(INFILE*,INFILE*);

/* @endcond */
