/***************** STELLAR.H *********************** 
 *
 * Rodrigo Luger, Tue Apr 28 11:04:00 PDT 2014
 *
 * This header file contains all the subroutines in
 * file stellar.c.
 *
*/

#define LSUN                    3.846e26 // Solar luminosity (W)

void InitializeControlStellar(CONTROL*);
void AddModuleStellar(MODULE*,int,int);
void BodyCopyStellar(BODY*,BODY*,int,int);
void InitializeBodyEqtide(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyStellar(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */
#define OPTSTARTSTELLAR         1500 /* Start of Stellar options */
#define OPTENDSTELLAR           1600 /* End of Stellar options */

#define OPT_LUMINOSITY          1510 // (Initial) luminosity
#define OPT_SATXUVFRAC          1511 // Saturation XUV luminosity fraction

/* Options Functions */
void HelpOptionsStellar(OPTIONS*);
void InitializeOptionsStellar(OPTIONS*,fnReadOption[]);
void ReadOptionsStellar(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Halt Functions */
#define STELLARHALTSYSEND       5
#define STELLARHALTBODYEND      5

int fbHaltSurfaceDesiccated(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
void CountHaltsStellar(HALT*,int*);

/* Verify Functions */
void VerifyStellar(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void VerifyRotationStellar(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetStellar(CONTROL*,MODULE*);
void PropertiesStellar(BODY*,int);
void VerifyHaltStellar(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationStellar(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateStellar(BODY*,UPDATE*,int);
void FinalizeUpdateEccStellar(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateNumIsotopeStellar(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateOblStellar(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateRotStellar(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateSemiStellar(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateLuminosityStellar(BODY*,UPDATE*,int*,int,int);

/* Output Functinos */

/* Stellar 1200 - 1299 */
#define OUTSTARTSTELLAR         1500
#define OUTENDSTELLAR           1600

/* Body Properties due to stellar evol */
#define OUT_LUMINOSITY	        1510
#define OUT_LXUV	              1511

void HelpOutputStellar(OUTPUT*);
void InitializeOutputStellar(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionStellar(OUTPUT*,int,int);
void FinalizeOutputFunctionStellar(OUTPUT*,int,int);

void WriteLuminosity(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteLXUV(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsStellar(CONTROL*,FILE*);
void LogStellar(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyStellar(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Stellar functions */
void fnForceBehaviorStellar(BODY*,EVOLVE*,IO*,int,int);
// TODO! double fdDLuminosityDt(BODY*,SYSTEM*,int*,int);

/* Dummy functions */
double fdSurfEnFluxStellar(BODY*,SYSTEM*,UPDATE*,int,int);