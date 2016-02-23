/***************** BINARY.H *********************** 
 *
 * David Fleming (dflemin3), Tue Jan 12 10:23am PDT 2016
 *
 * This header file contains all the subroutines in
 * file binary.c.
 *
*/


#define K_MAX                   4   /* Max term to sum to */

/* Options Info */
/* For options and output, binary has 2000-2100 */
#define OPTSTARTBINARY          2000 /* Start of Binary options */
#define OPTENDBINARY            2100 /* End of Binary options */

#define OPT_FREEECC             2010 // Free eccentricity
#define OPT_FREEINC             2020 // Free inclination
#define OPT_LL13N0              2030 // LL13 Mean Motion
#define OPT_LL13K0              2040 // LL13 Radial epicyclic frequency
#define OPT_LL13V0              2050 // LL13 Vertical epicyclic frequency
#define OPT_CYLPOS              2060 // Cylindrical position [r,phi,z]

/* Output Info */

#define OUTSTARTBINARY          2000
#define OUTENDBINARY            2100

// Naming convention same as for OPT_* constants
#define OUT_FREEECC             2010 
#define OUT_FREEINC             2020
#define OUT_BININC                 2025
#define OUT_LL13N0              2030
#define OUT_LL13K0              2040
#define OUT_LL13V0              2050
#define OUT_CYLPOS              2060

void InitializeModuleBinary(CONTROL*,MODULE*);
void InitializeControlBinary(CONTROL*);
void AddModuleBinary(MODULE*,int,int);
void BodyCopyBinary(BODY*,BODY*,int,int);
void InitializeBodyBinary(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyBinary(BODY*,CONTROL*,UPDATE*,int);

/* Options Functions */
void HelpOptionsBinary(OPTIONS*);
void InitializeOptionsBinary(OPTIONS*,fnReadOption[]);
void ReadOptionsBinary(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);
void ReadFreeEcc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadFreeInc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadLL13N0(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadLL13K0(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadLL13V0(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);

/* Halt Functions */

void VerifyHaltBinary(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt);
void CountHaltsBinary(HALT*,int*);

/* Verify Functions */
void VerifyBinary(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void InitializeVplanetBinary(CONTROL*,MODULE*);
void fnPropertiesBinary(BODY*,UPDATE*,int);
void fnForceBehaviorAtmEsc(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,int iBody,int iModule);

/* Update functions */
void InitializeUpdateBinary(BODY*,UPDATE*,int);

/* Output Functinos */
void HelpOutputBinary(OUTPUT*);
void InitializeOutputFunctionBinary(OUTPUT*,int,int);
void InitializeOutputBinary(OUTPUT*,fnWriteOutput[]);
void FinalizeOutputFunctionBinary(OUTPUT*,int,int);
void WriteFreeEcc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFreeInc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteInc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteLL13N0(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteLL13K0(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteLL13V0(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsBinary(CONTROL*,FILE*);
void LogBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Math Functions */
int fiDelta(int,int);
double fdDot(double*,double*);
void fvCylToCartPos(double*,double*);
void fvCylToCartVel(double*,double*,double*);
void fvSpecificAngMom(double*,double*,double*);
double fdSpecificOrbEng(BODY*);

/* Orbital Element Functions */
double fdComputeSemi(BODY*);
double fdComputeEcc(BODY*);
double fdComputeInc(BODY*);
double fdComputeLongA(BODY*);
void fdComputeEccVector(BODY*,double*);
double fdComputeArgPeri(BODY*);
double fdHolmanStability(BODY*);
double fdMeanAnomaly(double,double,double);

/* Binary functions */

double fdMeanMotion(BODY*);
double fdEpiFreqK(BODY*);
double fdEpiFreqV(BODY*);
double fdPhi0(double,double,double);
double fdPot0(int,int,double,BODY*);
double fdPot0dR(int,int,double,BODY*);
double fdPot1(int,int,double,BODY*);
double fdPot1dR(int,int,double,BODY*);
double fdn(double,BODY*);
double fdC0(BODY*);
double fdC0k(int,BODY*);
double fdCPk(int,BODY*);
double fdCMk(int,BODY*);
double fdD0(BODY*);
double fdDk0(int,BODY*);
double fdDPk(int,BODY*);
double fdDMk(int,BODY*);
double calculate_R(double,BODY*,double);
double calculate_Phi(double,BODY*,double);
double calculate_Z(double,BODY*,double);
double calculate_Rdot(double,BODY*,double,double);
double calculate_Phidot(double,BODY*,double,double);
double calculate_Zdot(double,BODY*,double);

/* Misc functions */
void fDebugBinary(BODY*);
