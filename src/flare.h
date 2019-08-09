/**
  @file flare.h

  @brief Subroutines that control the output of XUV flares.

  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

  @date Mar 25 2016

*/

#define MINMASSFLARE        0.07 // Minimum stellar mass for flare module (solar)
#define MAXMASSFLARE        0.7 // Maximum stellar mass for flare module (solar)

/* Options Info */

#define OPTSTARTFLARE         2000 /* Start of FLARE options */
#define OPTENDFLARE           2100 /* End of FLARE options */

/*
#define OPT_FLARECONST        2010
#define OPT_FLAREEXP          2020
*/
/*
const int OPT_FLAREYINT =     2010;
const int OPT_FLARESLOPE =    2020;
const int OPT_FLAREC =        2030;
const int OPT_FLAREK =        2040;
const int OPT_FLAREVISWIDTH = 2050;
const int OPT_FLAREXUVWIDTH = 2060;
const int OPT_FLAREMINENERGY= 2070;
const int OPT_FLAREMAXENERGY= 2080;
*/
#define OPT_FLAREYINT         2010
#define OPT_FLARESLOPE        2020
#define OPT_FLAREC            2030
#define OPT_FLAREK            2040
#define OPT_FLAREVISWIDTH     2050
#define OPT_FLAREXUVWIDTH     2060
#define OPT_FLAREMINENERGY    2070
#define OPT_FLAREMAXENERGY    2080

/* Output Functinos */

/* FLARE 1900 - 1999 */
#define OUTSTARTFLARE         2000
#define OUTENDFLARE           2100

/* Body Properties due to flaring */
#define OUT_LXUVFLARE         2010

/* @cond DOXYGEN_OVERRIDE */

void InitializeControlFlare(CONTROL*);
void AddModuleFlare(CONTROL*,MODULE*,int,int);
void BodyCopyFlare(BODY*,BODY*,int,int,int);
void InitializeBodyFlare(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyFlare(BODY*,CONTROL*,UPDATE*,int);

void HelpOptionsFlare(OPTIONS*);
void InitializeOptionsFlare(OPTIONS*,fnReadOption[]);
void ReadOptionsFlare(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

void PropertiesFlare(BODY*,EVOLVE*,UPDATE*,int);
void fnForceBehaviorFlare(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* Options Functions */
void HelpOptionsFlare(OPTIONS*);

/* Halt Functions */

/* Verify Functions */

void VerifyFlare(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,int,int);

/* Update functions */
void InitializeUpdateFlare(BODY*,UPDATE*,int);
void FinalizeUpdateLXUVFlare(BODY*,UPDATE*,int*,int,int,int);

void InitializeOutputFlare(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionFlare(OUTPUT*,int,int);
void FinalizeOutputFunctionFlare(OUTPUT*,int,int);

void WriteLXUVFlare(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsFlare(CONTROL*,FILE*);
void LogFlare(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyFlare(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* FLARE functions */
double fdLXUVFlare(BODY*,double,int);

double fdDLXUVFlareDt(BODY*,SYSTEM*,int*);

/* @endcond */
