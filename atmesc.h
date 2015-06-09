/***************** ATMESC.H *********************** 
 *
 * Rodrigo Luger, Tue Apr 28 11:04:00 PDT 2014
 *
 * This header file contains all the subroutines in
 * file atmesc.c.
 *
*/

#define HELMSDEEPARMY           10000 // Strong
#define ORCMASS                 80000 // grams

#define TOMASS                  1.39e24     // Mass of one terrestrial ocean (TO)
#define TOHMASS                 TOMASS/9.   // Hydrogen mass in one TO

void InitializeControlAtmEsc(CONTROL*);
void AddModuleAtmEsc(MODULE*,int,int);
void BodyCopyAtmEsc(BODY*,BODY*,int,int);
void InitializeBodyEqtide(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyAtmEsc(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */
#define OPTSTARTATMESC          1200 /* Start of AtmEsc options */
#define OPTENDATMESC            1300 /* End of AtmEsc options */

#define OPT_NUMBEROFORCS        1201
#define OPT_SURFACEWATERMASS    1210 // Initial surface water mass

/* Options Functions */
void HelpOptionsAtmEsc(OPTIONS*);
void ReadNumberOfOrcs(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void InitializeOptionsAtmEsc(OPTIONS*,fnReadOption[]);
void ReadOptionsAtmEsc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Halt Functions */
#define ATMESCHALTSYSEND       5
#define ATMESCHALTBODYEND      5
int fbHaltTooManyOrcs(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
int fbHaltSurfaceDesiccated(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
void CountHaltsAtmEsc(HALT*,int*);

/* Verify Functions */
void VerifyAtmEsc(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void VerifyRotationAtmEsc(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetAtmEsc(CONTROL*,MODULE*);
void PropertiesAtmEsc(BODY*,int);
void VerifyHaltAtmEsc(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationAtmEsc(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateAtmEsc(BODY*,UPDATE*,int);
void FinalizeUpdateEccAtmEsc(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateNumIsotopeAtmEsc(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateOblAtmEsc(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateRotAtmEsc(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateSemiAtmEsc(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateNumberOfOrcsAtmEsc(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateSurfaceWaterMassAtmEsc(BODY*,UPDATE*,int*,int,int);

/* Output Functinos */

/* AtmEsc 1200 - 1299 */
#define OUTSTARTATMESC         1200
#define OUTENDATMESC           1300

/* Body Properties due to atmospheric escape */
#define OUT_NUMBEROFORCS	     1201
#define OUT_SURFACEWATERMASS	 1210

void HelpOutputAtmEsc(OUTPUT*);
void InitializeOutputAtmEsc(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionAtmEsc(OUTPUT*,int,int);
void FinalizeOutputFunctionAtmEsc(OUTPUT*,int,int);

void WriteNumberOfOrcs(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteSurfaceWaterMass(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsAtmEsc(CONTROL*,FILE*);
void LogAtmEsc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyAtmEsc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* AtmEsc functions */
void fnForceBehaviorAtmEsc(BODY*,EVOLVE*,IO*,int,int);
double fdDNumberOfOrcsDt(BODY*,SYSTEM*,int*,int);
double fdDSurfaceWaterMassDt(BODY*,SYSTEM*,int*,int);

/* Dummy functions */
double fdSurfEnFluxAtmEsc(BODY*,SYSTEM*,UPDATE*,int,int);