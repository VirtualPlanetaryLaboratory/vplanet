/***************** GALHABIT.H *********************** 
 *
 * Russell Deitrick, June 24, 2015
 *
 * This header file contains all the subroutines in
 * file galhabit.c.
 *
*/


void AddModuleGalHabit(MODULE*,int,int);
void BodyCopyGalHabit(BODY*,BODY*,int,int,int);
void InitializeBodyGalHabit(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyGalHabit(BODY*,CONTROL*,UPDATE*,int);

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

/* Options Functions */
#define OPT_PERIQ              2221
#define OPT_HOSTBINARY         2250
#define OPT_HOSTBINSEMI        2251
#define OPT_HOSTBINECC         2252
#define OPT_HOSTBININC         2253
#define OPT_HOSTBINARGP        2254
#define OPT_HOSTBINLONGA       2255

// void ReadInc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
// 
// void ReadArgP(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void InitializeOptionsGalHabit(OPTIONS*, fnReadOption[]);
void ReadOptionsGalHabit(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Verify Functions */
// void VerifyPericenter(BODY*,CONTROL*,OPTIONS*,char[],int,int);
void VerifyGalHabit(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* Update Functions */

void InitializeUpdateGalHabit(BODY*,UPDATE*,int);
void FinalizeUpdatePeriQGalHabit(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateArgPGalHabit(BODY*,UPDATE*,int*,int,int,int);

/* Output Functinos */

/* GALHABIT 2200-2300 */
/* System properties 2200-2219, body properties 2220-2299 */

#define OUTSTARTGALHABIT        2200 /* Start of GALHABIT options */
#define OUTENDGALHABIT          2300 /* End of GALHABIT options */
#define OUTBODYSTARTGALHABIT    2220 /* Start of GALHABIT BODY options */

#define OUT_PERIQ               2220
#define OUT_NENCOUNTERS         2221

void HelpOutputGalHabit(OUTPUT*);
void WriteBodyPeriQGalHabit(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void InitializeOutputGalHabit(OUTPUT*,fnWriteOutput[]);

/* Logging Functions */
void LogOptionsGalHabit(CONTROL*,FILE*);
void LogGalHabit(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyGalHabit(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* GalHabit Functions */
void PropertiesGalHabit(BODY*,EVOLVE*,UPDATE*,int);
void ForceBehaviorGalHabit(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
double random_double();
void testrand(SYSTEM*);
double NearbyStarDist(double);
int check_disrupt(BODY*,SYSTEM*,int);
double DarkMatterDensity(SYSTEM*,double);
double ISMDensity(SYSTEM*,double);
void CalcMeanVelDispSolar(SYSTEM*);


/* GalHabit derivatives */
double fdGalHabitDPeriQDt(BODY*, SYSTEM*, int*);
double fdGalHabitDArgPDt(BODY*, SYSTEM*, int*);
double random_double();
double mag2mass(double);
void GetStarMass(SYSTEM*);
void GetStarVelocity(SYSTEM*);
void GetStarPosition(SYSTEM*);
void VelocityDisp(SYSTEM*);
void CalcImpactParam(SYSTEM*);
void ApplyDeltaV(BODY*,SYSTEM*,int);
void AdvanceMA(BODY*,SYSTEM*,int);
void NextEncounterTime(SYSTEM*,double);
void CalcEncounterRate(SYSTEM*);
