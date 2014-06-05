/******************** UTIL.H ************************/ 
/*
 * Rory Barnes, Wed May  7 13:45:45 PDT 2014
 *
 * This header file contains all the subroutines in
 * file util.c.
 *
*/

void WriteHelpOption(OPTIONS);
void WriteHelpOutput(OUTPUT);
void HelpOptions(OPTIONS*);
void HelpOutput(OUTPUT*);
void Help(OUTPUT*,OPTIONS*,char[]);

void LineExit(char[],int);
char *sLower(char[]);
void fprintd(FILE*,double,int,int);

double fdLengthUnit(int);
double fdTimeUnit(int);
double fdMassUnit(int);
double fdTempUnit(double,int,int);
double fdAngleUnit(int);
double fdEnergyUnit(int,int,int);
double fdEnergyFluxUnit(int,int,int);

void fvLengthUnit(int,char[]);
void fvTimeUnit(int,char[]);
void fvAngleUnit(int,char[]);
void fvMassUnit(int,char[]);
void fvAngMomUnit(UNITS*,char[]);
void fvVelUnit(UNITS*,char[]);
void fvRateUnit(int,char[]);
void fvAngRateUnit(UNITS*,char[]);
void fvEnergyUnit(UNITS*,char[]);
void fvEnergyFluxUnit(UNITS*,char[]);

