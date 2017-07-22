/***************** ATMESC.H *********************** 
 *
 * Rodrigo Luger, Tue Apr 28 11:04:00 PDT 2014
 *
 * This header file contains all the subroutines in
 * file atmesc.c.
 *
*/

#define TOMASS                  1.39e21     // Mass of one terrestrial ocean in kg (TO)
#define TOHMASS                 TOMASS/9.   // Hydrogen mass in one TO
#define ATMESC_LB15             0
#define ATMESC_LBEXACT          1
#define ATMESC_TIAN             2
#define ATMESC_ELIM             3
#define ATMESC_DIFFLIM          4
#define ATMESC_NONE             5
#define ATMESC_LOP12            6
#define ATMESC_PROXCENB         7
#define ATMESC_LEHMER17			8
#define THERMT                  400.                          // Average thermospheric temperature (K, Venus)
#define BDIFF                   4.8e19 * pow(THERMT, 0.75)    // Binary diffusion coefficient of H through O (m^-1 s^-1)
#define QOH                     16.                           // Atomic mass ratio oxygen/hydrogen

void AddModuleAtmEsc(MODULE*,int,int);
void BodyCopyAtmEsc(BODY*,BODY*,int,int,int);

/* Options Info */
#define OPTSTARTATMESC          1200 /* Start of AtmEsc options */
#define OPTENDATMESC            1300 /* End of AtmEsc options */

#define OPT_SURFACEWATERMASS    1210 // Initial surface water mass
#define OPT_HALTDESICCATED      1211 // Halt if desiccated?
#define OPT_MINSURFACEWATERMASS 1212 // Minimum surface water mass (desiccated below this)
#define OPT_XFRAC               1213 // X-ray absorption radius as a fraction of planet radius
#define OPT_ATMXABSEFFH         1214 // Hydrogen Absorption efficiency (epsilon)
#define OPT_ENVELOPEMASS        1215 // Initial envelope mass
#define OPT_HALTENVELOPEGONE    1216 // Halt if evaporated?
#define OPT_MINENVELOPEMASS     1217 // Minimum envelope mass (evaporated below this)
#define OPT_OXYGENMASS          1218 // Initial oxygen mass
#define OPT_WATERLOSSMODEL      1219 // Oxygen buildup / water loss model
#define OPT_PLANETRADIUSMODEL   1220 // Gaseous planet radius model (for atmesc)
#define OPT_INSTANTO2SINK       1221 // Gaseous planet radius model (for atmesc)
#define OPT_ATMXABSEFFH2O       1222 // Water Absorption efficiency (epsilon)
#define OPT_OXYGENMANTLEMASS    1223 // Initial oxygen mass in mantle

#define OPT_THERMTEMP			1224 // thermosphere temperature
#define OPT_PRESXUV				1225 // Pressure at bottom of thermosphere
#define OPT_ATMGASCONST			1226 // Atmospheric Gas Constant

/* Options Functions */
void HelpOptionsAtmEsc(OPTIONS*);
void InitializeOptionsAtmEsc(OPTIONS*,fnReadOption[]);
void ReadOptionsAtmEsc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Halt Functions */
#define ATMESCHALTSYSEND       5
#define ATMESCHALTBODYEND      5

int fbHaltSurfaceDesiccated(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
int fbHaltEnvelopeGone(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
void CountHaltsAtmEsc(HALT*,int*);

/* Verify Functions */
void VerifyAtmEsc(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void VerifyRotationAtmEsc(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetAtmEsc(CONTROL*,MODULE*);
void VerifyHaltAtmEsc(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationAtmEsc(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateAtmEsc(BODY*,UPDATE*,int);
void FinalizeUpdateEccAtmEsc(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateNumIsotopeAtmEsc(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateOblAtmEsc(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateRotAtmEsc(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateSemiAtmEsc(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateSurfaceWaterMassAtmEsc(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateOxygenMassAtmEsc(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateOxygenMantleMassAtmEsc(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateRadiusAtmEsc(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateEnvelopeMassAtmEsc(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateMassAtmEsc(BODY*,UPDATE*,int*,int,int,int);

/* Output Functinos */

/* AtmEsc 1200 - 1299 */
#define OUTSTARTATMESC         1200
#define OUTENDATMESC           1300

/* Body Properties due to atmospheric escape */
#define OUT_SURFACEWATERMASS	 1210
#define OUT_ENVELOPEMASS	     1211
#define OUT_OXYGENMASS         1212
#define OUT_RGLIMIT            1213
#define OUT_XO                 1214
#define OUT_ETAO               1215
#define OUT_PLANETRADIUS       1216
#define OUT_OXYGENMANTLEMASS   1217
#define OUT_PLANETRADXUV		1218

void HelpOutputAtmEsc(OUTPUT*);
void InitializeOutputAtmEsc(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionAtmEsc(OUTPUT*,int,int);
void FinalizeOutputFunctionAtmEsc(OUTPUT*,int,int);

void WriteSurfaceWaterMass(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteOxygenMass(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteOxygenMantleMass(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteEnvelopeMass(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRGLimit(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteOxygenMixingRatio(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteOxygenEta(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsAtmEsc(CONTROL*,FILE*);
void LogAtmEsc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyAtmEsc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* AtmEsc functions */
void fnForceBehaviorAtmEsc(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void fnPropertiesAtmEsc(BODY*,EVOLVE*,UPDATE*,int);
double fdDSurfaceWaterMassDt(BODY*,SYSTEM*,int*);
double fdDEnvelopeMassDt(BODY*,SYSTEM*,int*);
double fdHZRG14(double,double,double,double);
void fvLinearFit(double*,double*,int,double*);
double fdDOxygenMassDt(BODY*,SYSTEM*,int*);
double fdDOxygenMantleMassDt(BODY*,SYSTEM*,int*);
double fdAtomicOxygenMixingRatio(double,double);
double fdInsolation(BODY*,int,int);
int fbDoesWaterEscape(BODY*,int);
double fdPlanetRadius(BODY*,SYSTEM*,int*);

/* Dummy functions */
double fdSurfEnFluxAtmEsc(BODY*,SYSTEM*,UPDATE*,int,int);
