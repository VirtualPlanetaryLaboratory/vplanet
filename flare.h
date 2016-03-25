/***************** FLARE.H *********************** 
 *
 * Rory Barnes, Fri Mar 25 11:10:52 PDT 2016
 *
 * This header file contains all the subroutines in
 * file flare.c.
 *
*/

#define MINMASSFLARE        0.07 // Minimum stellar mass for flare module (solar)
#define MAXMASSFLARE        0.7 // Maximum stellar mass for flare module (solar)


void InitializeControlFlare(CONTROL*);
void AddModuleFlare(MODULE*,int,int);
void BodyCopyFlare(BODY*,BODY*,int,int,int);
void InitializeBodyFlare(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyFlare(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */

#define OPTSTARTFLARE         1900 /* Start of FLARE options */
#define OPTENDFLARE           2000 /* End of FLARE options */

#define OPT_FLARECONST        1910
#define OPT_FLAREEXP          1920

void HelpOptionsFlare(OPTIONS*);
void InitializeOptionsFlare(OPTIONS*,fnReadOption[]);
void ReadOptionsFlare(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

void PropertiesFlare(BODY*,UPDATE*,int);
void fnForceBehaviorFlare(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,int,int);

/* Options Functions */
void HelpOptionsFlare(OPTIONS*);

/* Halt Functions */

/* Verify Functions */

void VerifyFlare(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* Update functions */
void InitializeUpdateFlare(BODY*,UPDATE*,int);
void FinalizeUpdateLXUVFlare(BODY*,UPDATE*,int*,int,int,int);

/* Output Functinos */

/* FLARE 1900 - 1999 */
#define OUTSTARTFLARE         1900
#define OUTENDFLARE           2000

/* Body Properties due to flaring */
#define OUT_LXUVFLARE         1910

void HelpOutputFlare(OUTPUT*);
void InitializeOutputFlare(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionFlare(OUTPUT*,int,int);
void FinalizeOutputFunctionFlare(OUTPUT*,int,int);
void AddModuleFlare(MODULE*,int,int);

void WriteLXUVFlare(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsFlare(CONTROL*,FILE*);
void LogFlare(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyFlare(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* RadHeat functions */
double fdLXUVFlare(BODY*,SYSTEM*,UPDATE*,int,int);

void fnForceBehaviorFlare(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,int,int);

double fdDLXUVFlareDt(BODY*,SYSTEM*,int*); 
