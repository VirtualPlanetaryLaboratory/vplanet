/**
   @file galhabit.h

   @brief Subroutines that simulate the galactic habitat of a stellar system.

   @author Russell Deitrick ([deitrr](https://github.com/deitrr/))

   @date July 7 2015

*/

/* Options Info */

#define OPTSTARTGALHABIT        2200 /* Start of GALHABIT options */
#define OPTENDGALHABIT          2300 /* End of GALHABIT options */

#define OPT_GALACDENSITY       2201
#define OPT_RANDSEED           2202
#define OPT_ENCOUNTERRAD       2203
#define OPT_RFORM              2204
#define OPT_TMIGRATION         2205
#define OPT_RADIALMIGR         2206
#define OPT_GASDENSITY         2207
#define OPT_DMDENSITY          2208
#define OPT_STARSCALEL         2209
#define OPT_STELLARENC         2210
#define OPT_TIMEEVOLVELDISP    2211
#define OPT_OUTPUTENC          2212


/* Options Functions */
#define OPT_PERIQ              2221
#define OPT_HOSTBINARY         2250
#define OPT_HOSTBINSEMI        2251
#define OPT_HOSTBINECC         2252
#define OPT_HOSTBININC         2253
#define OPT_HOSTBINARGP        2254
#define OPT_HOSTBINLONGA       2255
#define OPT_HOSTBINMASS1       2256
#define OPT_MINALLOWED         2257
#define OPT_GALACTIDES         2258

/* Output Functinos */

/* GALHABIT 2200-2300 */
/* System properties 2200-2219, body properties 2220-2299 */

#define OUTSTARTGALHABIT        2200 /* Start of GALHABIT options */
#define OUTENDGALHABIT          2300 /* End of GALHABIT options */
#define OUTBODYSTARTGALHABIT    2220 /* Start of GALHABIT BODY options */

#define OUT_PERIQ               2220
#define OUT_NENCOUNTERS         2221
#define OUT_FVELDISP            2222
#define OUT_ECCX                2225
#define OUT_ECCY                2226
#define OUT_ECCZ                2227
#define OUT_ANGMX               2228
#define OUT_ANGMY               2229
#define OUT_ANGMZ               2230
#define OUT_NBADIMPULSE         2231

#define OUT_DECCDTGALHTIDAL     2250
#define OUT_DINCDTGALHTIDAL     2251
#define OUT_DLONGADTGALHTIDAL   2252
#define OUT_DARGPDTGALHTIDAL    2253

/* @cond DOXYGEN_OVERRIDE */

void AddModuleGalHabit(MODULE*,int,int);
void BodyCopyGalHabit(BODY*,BODY*,int,int,int);
void InitializeBodyGalHabit(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyGalHabit(BODY*,CONTROL*,UPDATE*,int);

// void ReadInc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
//
// void ReadArgP(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void InitializeOptionsGalHabit(OPTIONS*, fnReadOption[]);
void ReadOptionsGalHabit(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Verify Functions */
// void VerifyPericenter(BODY*,CONTROL*,OPTIONS*,char[],int,int);
void VerifyGalHabit(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,int,int);

/* Update Functions */

void InitializeUpdateGalHabit(BODY*,UPDATE*,int);
void FinalizeUpdatePeriQGalHabit(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateArgPGalHabit(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateIncGalHabit(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateLongAGalHabit(BODY*,UPDATE*,int*,int,int,int);

void HelpOutputGalHabit(OUTPUT*);
void WriteBodyPeriQ(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteDEccDtGalHTidal(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteDIncDtGalHTidal(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteDArgPDtGalHTidal(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteDLongADtGalHTidal(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void InitializeOutputGalHabit(OUTPUT*,fnWriteOutput[]);
void WriteBodyEccX(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyEccY(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyEccZ(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyAngMX(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyAngMY(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyAngMZ(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsGalHabit(CONTROL*,FILE*);
void LogGalHabit(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyGalHabit(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* GalHabit Functions */
void PropertiesGalHabit(BODY*,EVOLVE*,SYSTEM*,UPDATE*,int);
void ForceBehaviorGalHabit(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
double fndRandom_double();
void testrand(SYSTEM*);
double fndNearbyStarDist(double);
int fniCheck_disrupt(BODY*,SYSTEM*,int);
double fndDarkMatterDensity(SYSTEM*,double);
double fndISMDensity(SYSTEM*,double);
void CalcMeanVelDispSolar(SYSTEM*);
void VelocityApex(SYSTEM*);
void GetRelativeVelocity(SYSTEM*);
double fndNearbyStarFrEnc(SYSTEM*,double);
int fniCheck_dr(BODY*,EVOLVE*,SYSTEM*,int);

double fndDexDJ(BODY*, int);
double fndDexDla(double);
double fndDexDap(BODY*, int);
double fndDeyDJ(BODY*, int);
double fndDeyDla(double);
double fndDeyDap(BODY*,int);
double fndDezDJ(BODY*, int);
double fndDezDap(double,double,double);
void CalcEccVec(BODY*,int);
void CalcAngMVec(BODY*,int);
void Rot2Bin(BODY*,int);

/* GalHabit derivatives */
double fndGalHabitDArgPDt(BODY*, SYSTEM*, int*);
double fndGalHabitDIncDt(BODY*, SYSTEM*, int*);
double fndGalHabitDLongADt(BODY*, SYSTEM*, int*);
double fndGalHabitDEccXDtTidal(BODY*, SYSTEM*, int*);
double fndGalHabitDEccYDtTidal(BODY*, SYSTEM*, int*);
double fndGalHabitDEccZDtTidal(BODY*, SYSTEM*, int*);
double fndGalHabitDAngMXDtTidal(BODY*, SYSTEM*, int*);
double fndGalHabitDAngMYDtTidal(BODY*, SYSTEM*, int*);
double fndGalHabitDAngMZDtTidal(BODY*, SYSTEM*, int*);

double fndQuadC2(BODY*, int*);
double fndDQuadDEccXInner(BODY*, int*);
double fndDQuadDEccYInner(BODY*, int*);
double fndDQuadDEccZInner(BODY*, int*);
double fndDQuadDAngMXInner(BODY*, int*);
double fndDQuadDAngMYInner(BODY*, int*);
double fndDQuadDAngMZInner(BODY*, int*);
double fndDQuadDAngMXOuter(BODY*, int*);
double fndDQuadDAngMYOuter(BODY*, int*);
double fndDQuadDAngMZOuter(BODY*, int*);

double fndGalHabitDEccXDtBV(BODY*, SYSTEM*, int*);
double fndGalHabitDEccYDtBV(BODY*, SYSTEM*, int*);
double fndGalHabitDEccZDtBV(BODY*, SYSTEM*, int*);
double fndGalHabitDAngMXDtBV(BODY*, SYSTEM*, int*);
double fndGalHabitDAngMYDtBV(BODY*, SYSTEM*, int*);
double fndGalHabitDAngMZDtBV(BODY*, SYSTEM*, int*);

double fndOctC3(BODY*, int*);
double fndDOctDEccXInner(BODY*, int*);
double fndDOctDEccYInner(BODY*, int*);
double fndDOctDEccZInner(BODY*, int*);
double fndDOctDAngMXInner(BODY*, int*);
double fndDOctDAngMYInner(BODY*, int*);
double fndDOctDAngMZInner(BODY*, int*);
double fndDOctDEccXOuter(BODY*, int*);
double fndDOctDEccYOuter(BODY*, int*);
double fndDOctDEccZOuter(BODY*, int*);
double fndDOctDAngMXOuter(BODY*, int*);
double fndDOctDAngMYOuter(BODY*, int*);
double fndDOctDAngMZOuter(BODY*, int*);

double fndMag2mass(double);
void GetStarMass(SYSTEM*);
void GetStarVelocity(SYSTEM*);
void GetStarPosition(SYSTEM*);
void VelocityDisp(SYSTEM*);
void CalcImpactParam(BODY*, SYSTEM*,int);
void ApplyDeltaV(BODY*,SYSTEM*,int);
void AdvanceMA(BODY*,SYSTEM*,int);
void NextEncounterTime(SYSTEM*,EVOLVE*,double);
void CalcEncounterRate(SYSTEM*);

/* @endcond */
