/**
  @file flare.h

  @brief Subroutines that control the output of XUV flares.

  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

  @date Mar 25 2016

*/

#define MINMASSFLARE          0.07 // Minimum stellar mass for flare module (solar)
#define MAXMASSFLARE          1.9 // Maximum stellar mass for flare module (solar)
#define FLARE_FFD_DAVENPORT  0
#define FLARE_FFD_LACY       1
#define FLARE_FFD_OBSERV     2

#define FLARE_ACTIVE          0
#define FLARE_INACTIVE        1
#define FLARE_CONSTANT        2
#define FLARE_ALLMDWARFS      3
#define FLARE_TILLEY          4

#define FLARE_KEPLER      0
#define FLARE_UV          1
#define FLARE_GOES        2
#define FLARE_SXR         3
#define FLARE_TESS_UV     4
#define FLARE_BOLOMETRIC  5

#define FLARE_SLOPE_SEC      0
#define FLARE_SLOPE_DAY      1
#define FLARE_SLOPE_HOUR     2
#define FLARE_SLOPE_MINUTE   3

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
#define OPT_FLAREYINTERRORUPPER 2011
#define OPT_FLAREYINTERRORLOWER 2012
#define OPT_FLARESLOPE        2020
#define OPT_FLARESLOPEERRORUPPER 2021
#define OPT_FLARESLOPEERRORLOWER 2022
#define OPT_FLAREC            2030
#define OPT_FLAREK            2040
#define OPT_FLAREVISWIDTH     2050
#define OPT_FLAREXUVWIDTH     2060
#define OPT_FLAREMINENERGY    2070
#define OPT_FLAREMAXENERGY    2080
#define OPT_FLAREACTIVITY     2085
#define OPT_FLAREFFD         2086
#define OPT_FLARESLOPEUNITS   2090
#define OPT_FLAREENERGYBIN    2091
#define OPT_FLAREOBSENERGY    2087
#define OPT_FLAREOBSFREQ      2088
#define OPT_FLAREDELTAT       2089
#define OPT_FLAREBANDPASS     2032

//define OPT_LAURA             2090     // LAURA'S VARIABLE


/* Output Functinos */

/* FLARE 1900 - 1999 */
#define OUTSTARTFLARE         2000
#define OUTENDFLARE           2100

/* Body Properties due to flaring */
#define OUT_LXUVFLARE         2010
#define OUT_LXUVFLAREUPPER    2011
#define OUT_LXUVFLARELOWER    2012
//#define OUT_LAURA             2090
//#define OUT_FFD               2091
#define OUT_FLAREDURATION1      2041
#define OUT_FLAREDURATION2      2042
#define OUT_FLAREDURATION3      2043
#define OUT_FLAREDURATION4      2044
#define OUT_FLAREFREQ1      2092
#define OUT_FLAREFREQ2      2093
#define OUT_FLAREFREQ3      2094
#define OUT_FLAREFREQ4      2095
#define OUT_FLAREFREQ5      2083
#define OUT_FLAREFREQ6      2082
#define OUT_FLAREENERGY1    2096
#define OUT_FLAREENERGY2    2097
#define OUT_FLAREENERGY3    2098
#define OUT_FLAREENERGY4    2099
#define OUT_FLAREENERGY5    2084
#define OUT_FLAREENERGY6    2081
/* @cond DOXYGEN_OVERRIDE */

void InitializeControlFlare(CONTROL*);
void AddModuleFlare(CONTROL*,MODULE*,int,int);
void BodyCopyFlare(BODY*,BODY*,int,int,int);
void InitializeBodyFlare(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyFlare(BODY*,CONTROL*,UPDATE*,int);

void HelpOptionsFlare(OPTIONS*);
void InitializeOptionsFlare(OPTIONS*,fnReadOption[]);
void ReadOptionsFlare(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

void PropsAuxFlare(BODY*,EVOLVE*,IO*,UPDATE*,int);
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
void WriteFlareMinFreq(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareMaxFreq(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsFlare(CONTROL*,FILE*);
void LogFlare(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyFlare(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* FLARE functions */
double fdLXUVFlare(BODY*,double,int);
double fdDavenport(double,double,double,double,double);
double fdFFD(BODY*,int,double,double, double);
double fdBandPassKepler(BODY*,int,double);
double fdBandPassXUV(BODY*,int ,double);
double fdEnergyJoulesXUV(double);
//double * fdFlareFreq(double,double,double,double,double,double);
//double fdDLXUVFlareDt(BODY*,SYSTEM*,int*);

/* @endcond */
