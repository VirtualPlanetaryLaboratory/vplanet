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
#define FLARE_FFD_NONE       2

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

#define OPT_FLAREYINT           2030
#define OPT_FLAREYINTERRORUPPER 2031
#define OPT_FLAREYINTERRORLOWER 2032
#define OPT_FLARESLOPE          2033
#define OPT_FLARESLOPEERRORUPPER 2034
#define OPT_FLARESLOPEERRORLOWER 2035
#define OPT_FLAREMINENERGY       2036
#define OPT_FLAREMAXENERGY       2037
#define OPT_FLAREFFD             2038
#define OPT_FLARESLOPEUNITS      2039
#define OPT_FLAREENERGYBIN       2040
#define OPT_FLAREBANDPASS        2041
#define OPT_LXUVFLARECONST       2042

/* Output Functinos */

/* FLARE 1900 - 1999 */
#define OUTSTARTFLARE         2000
#define OUTENDFLARE           2100

/* Body Properties due to flaring */
#define OUT_LXUVFLARE         2010
#define OUT_LXUVFLAREUPPER    2011
#define OUT_LXUVFLARELOWER    2012
#define OUT_FLAREFREQ1      2013
#define OUT_FLAREFREQ2      2014
#define OUT_FLAREFREQ3      2015
#define OUT_FLAREFREQ4      2016
#define OUT_FLAREFREQ5      2017
#define OUT_FLAREFREQ6      2018
#define OUT_FLAREFREQMAX    2019
#define OUT_FLAREENERGY1    2020
#define OUT_FLAREENERGY2    2021
#define OUT_FLAREENERGY3    2022
#define OUT_FLAREENERGY4    2023
#define OUT_FLAREENERGY5    2024
#define OUT_FLAREENERGY6    2025
#define OUT_FLAREENERGYMAX  2026
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
void WriteFlareFreq1(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareFreq2(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareFreq3(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareFreq4(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareFreq5(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareFreq6(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareFreqMax(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareEnergy1(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareEnergy2(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareEnergy3(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareEnergy4(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareEnergy5(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareEnergy6(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFlareEnergyMax(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

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

/* @endcond */
