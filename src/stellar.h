/***************** STELLAR.H ***********************
 *
 * Rodrigo Luger, Tue Apr 28 11:04:00 PDT 2014
 *
 * This header file contains all the subroutines in
 * file stellar.c.
 *
*/

#define LSUN                          3.846e26          // Solar luminosity (W)
#define TSUN                          5778.             // Solar TEff (K)
#define RM12OMEGACRIT                 8.56e-6           // Critical angular velocity (1/s) from Reiners & Mohanty (2012)
#define RM12OMEGACRITFULLYCONVEC      1.82e-6           // Critical angular velocity (1/s) for fully convective stars, Reiners & Mohanty (2012)
#define RM12CONST                     (2.66e3 * 46.416) // dJ/dt constant [(kg^5 m^-10 s^-3)^1/3] from Reiners & Mohanty (2012)
#define SK72CONST                     5.0e-25 // dJ/dt constant from Skumanich 1972 (Repetto+2014)
#define STELLAR_MODEL_NONE            0
#define STELLAR_MODEL_BARAFFE         1
#define STELLAR_MODEL_REINERS         2
#define STELLAR_MODEL_CONST           3
#define STELLAR_MODEL_RIBAS           4
#define STELLAR_MODEL_PROXIMACEN      5

#define STELLAR_DJDT_NONE             0 // No stellar angular momentum loss via magnetic braking
#define STELLAR_DJDT_RM12             1 // dJ/dt according to Reiners & Mohanty 2012
#define STELLAR_DJDT_SK72             2 // dJ/dt according to Skumanich 1972 empirical law

#define HZ_MODEL_KOPPARAPU            1
#define DRYRGFLUX                     415 // W/m^2 from Abe et al. (2011)

void InitializeControlStellar(CONTROL*);
void AddModuleStellar(MODULE*,int,int);
void BodyCopyStellar(BODY*,BODY*,int,int,int);
void InitializeUpdateTmpBodyStellar(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */
#define OPTSTARTSTELLAR         1500 /* Start of Stellar options */
#define OPTENDSTELLAR           1600 /* End of Stellar options */

#define OPT_LUMINOSITY          1510 // (Initial) luminosity
#define OPT_SATXUVFRAC          1511 // Saturation XUV luminosity fraction
#define OPT_STELLARMODEL        1512 // Luminosity evolution model
#define OPT_TEMPERATURE         1513 // Stellar effective temperature (initial)
#define OPT_WINDMODEL           1514 // Wind model
#define OPT_XUVMODEL            1515 // XUV evol model
#define OPT_HALTENDBARAFFEFGRID 1516 // Halt when we reach the end of the Baraffe grid?
#define OPT_SATXUVTIME          1517 // XUV saturation time
#define OPT_XUVBETA             1518 // XUV power law decay exponent

#define OPT_HZMODEL             1520 // Habitable zone model
#define OPT_MAGBRAKINGMODEL           1521 // Magnetic braking model

/* Options Functions */
void HelpOptionsStellar(OPTIONS*);
void InitializeOptionsStellar(OPTIONS*,fnReadOption[]);
void ReadOptionsStellar(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Halt Functions */
#define STELLARHALTSYSEND       5
#define STELLARHALTBODYEND      5

int fbHaltEndBaraffeGrid(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
void CountHaltsStellar(HALT*,int*);

/* Verify Functions */
void VerifyStellar(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,int,int);
void VerifyRotationStellar(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetStellar(CONTROL*,MODULE*);
void fnPropertiesStellar(BODY*,EVOLVE*,UPDATE*,int);
void VerifyHaltStellar(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationStellar(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateStellar(BODY*,UPDATE*,int);
void FinalizeUpdateEccStellar(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateNumIsotopeStellar(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateOblStellar(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateRotStellar(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateSemiStellar(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateLuminosityStellar(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateTemperatureStellar(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateRadiusStellar(BODY*,UPDATE*,int*,int,int,int);

/* Output Functinos */

/* Stellar 1200 - 1299 */
#define OUTSTARTSTELLAR         1500
#define OUTENDSTELLAR           1600

/* Body Properties due to stellar evol */
#define OUT_LUMINOSITY	        1510
#define OUT_LXUV	              1511
#define OUT_TEMPERATURE	        1512
#define OUT_LXUVFRAC            1513

#define OUT_HZLIMRECVENUS       1520 // Recent Venus HZ Limit
#define OUT_HZLIMRUNAWAY         1522 // Recent Venus HZ Limit
#define OUT_HZLIMMOIST          1524 // Recent Venus HZ Limit
#define OUT_HZLIMMAX            1526 // Recent Venus HZ Limit
#define OUT_HZLIMEARLYMARS      1528 // Recent Venus HZ Limit

void HelpOutputStellar(OUTPUT*);
void InitializeOutputStellar(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionStellar(OUTPUT*,int,int);
void FinalizeOutputFunctionStellar(OUTPUT*,int,int);

void WriteLuminosity(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTemperature(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteLXUV(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteLXUVFrac(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsStellar(CONTROL*,FILE*);
void LogStellar(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyStellar(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Stellar functions */
void fnForceBehaviorStellar(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable ***fnUpdate,int,int);
double fdLuminosity(BODY*,SYSTEM*,int*);
double fdLuminosityFunctionBaraffe(double, double);
double fdLuminosityFunctionProximaCen(double,double);
double fdRadius(BODY*,SYSTEM*,int*);
double fdRadiusFunctionBaraffe(double, double);
double fdRadiusFunctionProximaCen(double,double);
double fdDRotRateDt(BODY*,SYSTEM*,int*);
double fdDRotRateDtCon(BODY*,SYSTEM*,int*);
double fdDRotRateDtMagBrake(BODY*,SYSTEM*,int*);
double fdTemperature(BODY*,SYSTEM*,int*);
double fdTemperatureFunctionBaraffe(double, double);
double fdTemperatureFunctionProximaCen(double,double);
void fdHabitableZoneKopparapu2013(double,double,double*);
double fdDJDtMagBrakingStellar(BODY*,SYSTEM*,int*);
double fdDRadiusDtStellar(BODY*,SYSTEM*,int*);
double fdDEDtPotConStellar(BODY*,SYSTEM*,int*);
double fdDEDtRotConStellar(BODY*,SYSTEM*,int*);
double fdDEDtRotBrakeStellar(BODY*,SYSTEM*,int*);
double fdDEDtStellar(BODY*,SYSTEM*,int*);

/* Dummy functions */
double fdSurfEnFluxStellar(BODY*,SYSTEM*,UPDATE*,int,int);
