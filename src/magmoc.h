/***************** MAGMOC.H ***********************
 *
 * Patrick Barth, THursday May 17 14:41 PDT 2018
 *
 * This header file contains all the subroutines in
 * file magmoc.c.
 *
*/


// 26Al -- Masses, numbers and powers from Wikipedia XXX Check!

void InitializeOptionsMagmOc(OPTIONS*,fnReadOption[]);
void AddModuleMagmOc(MODULE*,int,int);

/* Options Info */

#define OPTSTARTMAGMOC            2300 /* Start of Magmoc options */
#define OPTENDMAGMOC              2400 /* End of Magmoc options */

#define OPT_FEO                   2310
#define OPT_WATERMASSATM          2311
#define OPT_SURFTEMP              2312
#define OPT_MANMELTDENSITY        2313

/* Options Functions */
void HelpOptionsMagmOc(OPTIONS*);
void ReadFeO(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadWaterMassAtm(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadSurfTemp(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadManMeltDensity(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);

/* Halt Functions */
#define MAGMOCHALTSYSEND       5
#define MAGMOCHALTBODYEND      5

/* Verify Functions */
void VerifyMagmOc(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,int,int);
void VerifyRotationMagmOc(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetMagmOc(CONTROL*,MODULE*);
void PropsAuxMagmOc(BODY*,EVOLVE*,UPDATE*,int);
void VerifyHaltMagmOc(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationMagmOc(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateMagmOc(BODY*,UPDATE*,int);
void FinalizeUpdateEccMagmOc(BODY*,UPDATE*,int*,int,int,int);

/* Output Functions */

/* MagmOc 1100 - 1199 */
#define OUTSTARTMAGMOC         2300
#define OUTENDMAGMOC           2400

#define OUT_FE2O3              2310
#define OUT_WATERMASSMOATM     2311


void HelpOutputMagmOc(OUTPUT*);
void InitializeOutputMagmOc(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionMagmOc(OUTPUT*,int,int);
void FinalizeOutputFunctionMagmOc(OUTPUT*,int,int);

// void WriteFe2O3(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);


/* Logging Functions */
void LogOptionsMagmOc(CONTROL*,FILE*);
void LogMagmOc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyMagmOc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* MagmOc functions */
void fnForceBehaviorMagmOc(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable ***fnUpdate,int,int);
double fdDWaterMassMOAtm(BODY*, SYSTEM*, int*);
