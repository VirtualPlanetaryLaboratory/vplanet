/***************** MAGMOC.H ***********************
 *
 * Patrick Barth, THursday May 17 14:41 PDT 2018
 *
 * This header file contains all the subroutines in
 * file magmoc.c.
 *
*/


// 26Al -- Masses, numbers and powers from Wikipedia XXX Check!
#define ENERGY26AL        6.4153392709791e-13 // [J]

void AddModuleMagmoc(MODULE*,int,int);

/* Options Info */

#define OPTSTARTMAGMOC            2300 /* Start of Magmoc options */
#define OPTENDMAGMOC              2400 /* End of Magmoc options */

#define OPT_FEO                   2310

/* Options Functions */
void HelpOptionsMagmoc(OPTIONS*);
void ReadFeO(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;

/* Halt Functions */
#define MAGMOCHALTSYSEND       5
#define MAGMOCHALTBODYEND      5

/* Verify Functions */
void VerifyMagmoc(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,int,int);
void VerifyRotationMagmoc(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetMagmoc(CONTROL*,MODULE*);
void PropsAuxMagmoc(BODY*,EVOLVE*,UPDATE*,int);
void VerifyHaltMagmoc(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationMagmoc(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateMagmoc(BODY*,UPDATE*,int);
void FinalizeUpdateEccMagmoc(BODY*,UPDATE*,int*,int,int,int);

/* Output Functions */

/* Magmoc 1100 - 1199 */
#define OUTSTARTMAGMOC         1100
#define OUTENDMAGMOC           1200


void HelpOutputMagmoc(OUTPUT*);
void InitializeOutputMagmoc(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionMagmoc(OUTPUT*,int,int);
void FinalizeOutputFunctionMagmoc(OUTPUT*,int,int);

/* Logging Functions */
void LogOptionsMagmoc(CONTROL*,FILE*);
void LogMagmoc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyMagmoc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Magmoc functions */
void fnForceBehaviorMagmoc(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable ***fnUpdate,int,int);
