void InitializeControl(CONTROL*,MODULE*);
void InitializeControlEvolve(CONTROL*,MODULE*,UPDATE*);

void WriteHelpOption(OPTIONS*);
void WriteHelpOutput(OUTPUT*);
void HelpOptions(OPTIONS*);
void HelpOutput(OUTPUT*);
void Help(OPTIONS*,OUTPUT*,char[]);

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
void fsUnitsAngMom(UNITS*,char[]);
void fsUnitsVel(UNITS*,char[]);
void fsUnitsRate(int,char[]);
void fsUnitsAngRate(UNITS*,char[]);
void fsUnitsPower(UNITS*,char[]);
void fsUnitsEnergy(UNITS*,char[]);
void fsUnitsEnergyFlux(UNITS*,char[]);

void InfileCopy(INFILE*,INFILE*);
