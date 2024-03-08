/**
    @file stellar.h
    @brief Subroutines that control the evolution of the star.
    @author Rodrigo Luger ([rodluger@gmail.com](mailto:rodluger@gmail.com>))
    @date May 12 2015
*/

#define LSUN 3.846e26 /**< Solar luminosity (W) */
#define TSUN 5778.    /**< Solar TEff (K) */
#define RM12OMEGACRIT                                                          \
  8.56e-6 /**< Critical angular velocity (1/s) from Reiners & Mohanty (2012)   \
           */
#define RM12OMEGACRITFULLYCONVEC                                               \
  1.82e-6 /**< Critical angular velocity (1/s) for fully convective stars,     \
             Reiners & Mohanty (2012) */
#define RM12CONST                                                              \
  (2.66e3 * 46.416) /**< dJ/dt constant [(kg^5 m^-10 s^-3)^1/3] from Reiners & \
                       Mohanty (2012) */
#define SK72CONST                                                              \
  5.0e-25 /**< dJ/dt constant from Skumanich 1972 (Repetto+2014) */
#define MATT15T0 6.3e23     /**< Matt+2015 T0 constant in Joules */
#define MATT15X 10.0        /**< Matt+2015 saturation ratio threshold */
#define MATT15R0SUN 2.168   /**< Matt+2015 solar Rossby number */
#define MATT15TAUCZ 1.115e6 /**< Matt+2015 solar TauCZ value in s */
#define MATT15OMEGASUN                                                         \
  2.6e-6 /**< Matt+2015 solar angular rotation frequency in s^-1 */
#define ROSSBYCRIT                                                             \
  2.08 /**< Critical Rossby number above which magnetic braking shuts off      \
          according to van Saders+2018 */
#define STELLAR_MODEL_NONE 0
#define STELLAR_MODEL_BARAFFE 1
#define STELLAR_MODEL_REINERS 2
#define STELLAR_MODEL_CONST 3
#define STELLAR_MODEL_RIBAS 4
#define STELLAR_MODEL_PROXIMACEN 5
#define STELLAR_MODEL_SINEWAVE 6

#define STELLAR_DJDT_NONE                                                      \
  0 /**< No stellar angular momentum loss via magnetic braking */
#define STELLAR_DJDT_RM12 1 /**< dJ/dt according to Reiners & Mohanty 2012 */
#define STELLAR_DJDT_SK72                                                      \
  2 /**< dJ/dt according to Skumanich 1972 empirical law */
#define STELLAR_DJDT_MA15 3 /**< dJ/dt according to Matt+2015 */

#define HZ_MODEL_KOPPARAPU 1
#define DRYRGFLUX 415 /**< W/m^2 from Abe et al. (2011) */

/* Options Info */
#define OPTSTARTSTELLAR 1500 /**< Start of Stellar options */
#define OPTENDSTELLAR 1600   /**< End of Stellar options */

#define OPT_SATXUVFRAC 1511   /**< Saturation XUV luminosity fraction */
#define OPT_STELLARMODEL 1512 /**< Luminosity evolution model */
#define OPT_WINDMODEL 1514    /**< Wind model */
#define OPT_XUVMODEL 1515     /**< XUV evol model */
#define OPT_HALTENDBARAFFEFGRID                                                \
  1516 /**< Halt when we reach the end of the Baraffe grid? */
#define OPT_SATXUVTIME 1517 /**< XUV saturation time */
#define OPT_XUVBETA 1518    /**< XUV power law decay exponent */
#define OPT_ROSSBYCUT 1519  /** Whether or not to apply Rossby cut to dJ/dt */

#define OPT_HZMODEL 1520         /**< Habitable zone model */
#define OPT_MAGBRAKINGMODEL 1521 /**< Magnetic braking model */

#define OPT_EVOVLERG                                                           \
  1522 /**< Whether or not to evolve stellar radius of gyration */

#define OPT_LUMAMPLITUDE 1550
#define OPT_LUMPERIOD 1555
#define OPT_LUMPHASE 1560

/* Halt Functions */
#define STELLARHALTSYSEND 5
#define STELLARHALTBODYEND 5

/* Output Functinos */

/* Stellar 1200 - 1299 */
#define OUTSTARTSTELLAR 1500
#define OUTENDSTELLAR 1600

/* Body Properties due to stellar evol */
#define OUT_LUMINOSITY 1510
#define OUT_LXUV 1511
#define OUT_TEMPERATURE 1512
#define OUT_LXUVFRAC 1513
#define OUT_ROSSBYNUMBER 1514
#define OUT_DROTPERDTSTELLAR 1515

/* @cond DOXYGEN_OVERRIDE */

void InitializeControlStellar(CONTROL *);
void AddModuleStellar(CONTROL *, MODULE *, int, int);
void BodyCopyStellar(BODY *, BODY *, int, int, int);
void InitializeUpdateTmpBodyStellar(BODY *, CONTROL *, UPDATE *, int);

/* Options Functions */
void HelpOptionsStellar(OPTIONS *);
void InitializeOptionsStellar(OPTIONS *, fnReadOption[]);
void ReadOptionsStellar(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                        fnReadOption[], int);

int fbHaltEndBaraffeGrid(BODY *, EVOLVE *, HALT *, IO *, UPDATE *,
                         fnUpdateVariable ***, int);
void CountHaltsStellar(HALT *, int *);

/* Verify Functions */
void VerifyStellar(BODY *, CONTROL *, FILES *, OPTIONS *, OUTPUT *, SYSTEM *,
                   UPDATE *, int, int);
void VerifyRotationStellar(BODY *, CONTROL *, OPTIONS *, char[], int);
void InitializeVplanetStellar(CONTROL *, MODULE *);
void fnPropsAuxStellar(BODY *, EVOLVE *, IO *, UPDATE *, int);
void VerifyHaltStellar(BODY *, CONTROL *, OPTIONS *, int, int *);
void VerifyRotationStellar(BODY *, CONTROL *, OPTIONS *, char[], int);

/* Update functions */
void InitializeUpdateStellar(BODY *, UPDATE *, int);
void FinalizeUpdateEccStellar(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateNumIsotopeStellar(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateOblStellar(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateRotStellar(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateSemiStellar(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateLuminosityStellar(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateTemperatureStellar(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateRadiusStellar(BODY *, UPDATE *, int *, int, int, int);

void InitializeOutputStellar(OUTPUT *, fnWriteOutput[]);
void InitializeOutputFunctionStellar(OUTPUT *, int, int);
void FinalizeOutputFunctionStellar(OUTPUT *, int, int);

void WriteLuminosity(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                     int, double *, char[]);
void WriteTemperature(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                      int, double *, char[]);
void WriteLXUV(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
               double *, char[]);
void WriteLXUVFrac(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char[]);
void WriteRossbyNumber(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char[]);
void WriteDRotPerDtStellar(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char[]);

/* Logging Functions */
void LogOptionsStellar(CONTROL *, FILE *);
void LogStellar(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                fnWriteOutput[], FILE *);
void LogBodyStellar(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                    fnWriteOutput[], FILE *, int);

/* Stellar functions */
void fnForceBehaviorStellar(BODY *, MODULE *, EVOLVE *, IO *, SYSTEM *,
                            UPDATE *, fnUpdateVariable ***fnUpdate, int, int);
double fdLuminosity(BODY *, SYSTEM *, int *);
double fdLuminosityFunctionBaraffe(double, double);
double fdLuminosityFunctionProximaCen(double, double);
double fdLuminosityFunctionSineWave(BODY *, int);
double fdRadius(BODY *, SYSTEM *, int *);
double fdRadGyra(BODY *, SYSTEM *, int *);
double fdRadiusFunctionBaraffe(double, double);
double fdRadGyraFunctionBaraffe(double, double);
double fdRadiusFunctionProximaCen(double, double);
double fdDRotRateDt(BODY *, SYSTEM *, int *);
double fdDRotRateDtCon(BODY *, SYSTEM *, int *);
double fdDRotRateDtRadGyra(BODY *, SYSTEM *, int *);
double fdDRotRateDtMagBrake(BODY *, SYSTEM *, int *);
double fdTemperature(BODY *, SYSTEM *, int *);
double fdTemperatureFunctionBaraffe(double, double);
double fdTemperatureFunctionProximaCen(double, double);
double fdDJDtMagBrakingStellar(BODY *, SYSTEM *, int *);
double fdDRadiusDtStellar(BODY *, SYSTEM *, int *);
double fdDRadGyraDtStellar(BODY *, SYSTEM *, int *);
double fdDEDtPotConStellar(BODY *, SYSTEM *, int *);
double fdDEDtRotConStellar(BODY *, SYSTEM *, int *);
double fdDEDtRotRadGyraStellar(BODY *, SYSTEM *, int *);
double fdDEDtRotBrakeStellar(BODY *, SYSTEM *, int *);
double fdDEDtStellar(BODY *, SYSTEM *, int *);
double fdCranmerSaar2011TauCZ(double);

/* Dummy functions */
double fdSurfEnFluxStellar(BODY *, SYSTEM *, UPDATE *, int, int);

/* @endcond */
