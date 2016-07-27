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

/* Options Functions */
#define OPT_PERIQ              2201

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

void HelpOutputGalHabit(OUTPUT*);
void WriteBodyPeriQGalHabit(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void InitializeOutputGalHabit(OUTPUT*,fnWriteOutput[]);

/* Logging Functions */
void LogOptionsGalHabit(CONTROL*,FILE*);
void LogGalHabit(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyGalHabit(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* GalHabit Functions */
void PropsAuxGalHabit(BODY*,EVOLVE*,UPDATE*,int);
void ForceBehaviorGalHabit(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* GalHabit derivatives */
double fdGalHabitDPeriQDt(BODY*, SYSTEM*, int*);
double fdGalHabitDArgPDt(BODY*, SYSTEM*, int*);