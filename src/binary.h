/***************** BINARY.H ***********************
 *
 * David Fleming (dflemin3), Tue Jan 12 10:23am PDT 2016
 *
 * This header file contains all the subroutines in
 * file binary.c.
 *
*/


#define K_MAX                   4   /* Max term to sum to */
#define FLUX_INT_MAX            20 /* How many CBP positions per orbit to integrate over */
#define KEQNTOL                 1.0e-3 /* Tolerance for Kepler eqn computation */
#define MAX_KEPLER_ITERS        30 /* Maximum number of iterations for Kepler eqn. solver */
#define FLUX_EARTH              1366 /* Insolation received by Earth in W/m^2 */

/* Options Info */
/* For options and output, binary has 2100-2200 */
#define OPTSTARTBINARY          2100 /* Start of Binary options */
#define OPTENDBINARY            2200 /* End of Binary options */

#define OPT_FREEECC             2110 // Free eccentricity
#define OPT_FREEINC             2120 // Free inclination
#define OPT_LL13N0              2130 // LL13 Mean Motion
#define OPT_LL13K0              2140 // LL13 Radial epicyclic frequency
#define OPT_LL13V0              2150 // LL13 Vertical epicyclic frequency
#define OPT_LL13PHIAB           2151 // LL13 Binary Mean Anomaly Initial value
#define OPT_CBPM0               2152 // CBP Initial mean anomaly
#define OPT_CBPZETA             2153 // CBP z oscillation phase angle
#define OPT_CBPPSI              2154 // CBP R, phi oscillation phase angle
#define OPT_HALTHOLMAN          2170 // Holman+Wiegert 1999 Instability limit
#define OPT_HALTROCHELOBE           2175 // Halt if roche lobe crossing occurs
#define OPT_BINUSEMATRIX        2180 // Whether or not to include eqns in matrix

/* Output Info */

#define OUTSTARTBINARY          2100
#define OUTENDBINARY            2200

// Naming convention same as for OPT_* constants
#define OUT_FREEECC             2110
#define OUT_FREEINC             2120
#define OUT_LL13N0              2130
#define OUT_LL13K0              2140
#define OUT_LL13V0              2150
#define OUT_CYLPOS              2160
#define OUT_CBPR                2161
#define OUT_CBPZ                2162
#define OUT_CBPPHI              2163
#define OUT_CBPRDOT             2164
#define OUT_CBPZDOT             2165
#define OUT_CBPPHIDOT           2166
#define OUT_CBPR0               2167
#define OUT_CBPINSOL            2168

void InitializeModuleBinary(CONTROL*,MODULE*);
void AddModuleBinary(MODULE*,int,int);
void BodyCopyBinary(BODY*,BODY*,int,int,int);
void InitializeBodyBinary(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyBinary(BODY*,CONTROL*,UPDATE*,int);
void fnPropertiesBinary(BODY*,EVOLVE*,UPDATE*,int);
void fnForceBehaviorBinary(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* Options Functions */
void HelpOptionsBinary(OPTIONS*);
void InitializeOptionsBinary(OPTIONS*,fnReadOption[]);
void ReadOptionsBinary(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);
void ReadFreeEcc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadFreeInc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadInc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadLL13N0(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadLL13K0(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadLL13V0(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadLL13PhiAB(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadCBPM0(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadCBPZeta(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadCBPPsi(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltHolmanUnstable(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltRocheLobe(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadBinaryUseMatrix(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);

/* Halt Functions */
int fbHaltHolmanUnstable(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody);
int fbHaltRocheLobe(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody);
void VerifyHaltBinary(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt);
void CountHaltsBinary(HALT*,int*);

/* Verify Functions */
void VerifyBinary(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,int,int);
void VerifyCBPR(BODY*,OPTIONS*,UPDATE*,double,int);
void VerifyCBPZ(BODY*,OPTIONS*,UPDATE*,double,int);
void VerifyCBPPhi(BODY*,OPTIONS*,UPDATE*,double,int);
void VerifyCBPRDot(BODY*,OPTIONS*,UPDATE*,double,int);
void VerifyCBPZDot(BODY*,OPTIONS*,UPDATE*,double,int);
void VerifyCBPPhiDot(BODY*,OPTIONS*,UPDATE*,double,int);
void InitializeVplanetBinary(CONTROL*,MODULE*);

/* Update functions */
void InitializeUpdateBinary(BODY*,UPDATE*,int);
void FinalizeUpdateCBPRBinary(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateCBPZBinary(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateCBPPhiBinary(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateCBPRDotBinary(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateCBPZDotBinary(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateCBPPhiDotBinary(BODY*,UPDATE*,int*,int,int,int);

/* Output Functinos */
void HelpOutputBinary(OUTPUT*);
void InitializeOutputFunctionBinary(OUTPUT*,int,int);
void InitializeOutputBinary(OUTPUT*,fnWriteOutput[]);
void FinalizeOutputFunctionBinary(OUTPUT*,int,int);
void WriteFreeEccBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFreeIncBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteLL13N0Binary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteLL13K0Binary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteLL13V0Binary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteCBPRBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteCBPR0Binary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteCBPZBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteCBPZDotBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteCBPPhiBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteCBPRDotBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteCBPPhiDotBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteCBPInsol(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsBinary(CONTROL*,FILE*);
void LogBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Math Functions */
int fniDelta(int,int);
double fndDot(double*,double*);
void fnvCylToCartPos(double*,double*);
void fnvCylToCartVel(double*,double*,double*);
void fnvSpecificAngMom(double*,double*,double*);
double fndSpecificOrbEng(BODY*,int);

/* Orbital Element Functions */
void fnvAssignOrbitalElements(BODY*,int);
double fndComputeSemi(BODY*,int);
double fndComputeEcc(BODY*,int);
double fndComputeInc(BODY*,int);
double fndComputeLongA(BODY*,int);
void fnvComputeEccVector(BODY*,double*,int);
double fndComputeArgPeri(BODY*,int);
double fndHolmanStability(BODY*);
double fndRocheLobe(BODY*);
double fndMeanAnomaly(double,double,double);
double fndMeanToEccentric(double,double);
double fndEccToTrue(double,double);

/* Binary functions */
double fndMeanMotionBinary(BODY*,int);
double fndEpiFreqK(BODY*,int);
double fndEpiFreqV(BODY*,int);
double fndPhi0(double,double,double);
double fndPot0(int,int,double,BODY*);
double fndPot0dR(int,int,double,BODY*);
double fndPot1(int,int,double,BODY*);
double fndPot1dR(int,int,double,BODY*);
double fndn(double,BODY*);
double fndC0(BODY*,int);
double fndC0k(int,BODY*,int);
double fndCPk(int,BODY*,int);
double fndCMk(int,BODY*,int);
double fndD0(BODY*,int);
double fndDk0(int,BODY*,int);
double fndDPk(int,BODY*,int);
double fndDMk(int,BODY*,int);
double fndCBPRBinary(BODY*,SYSTEM*,int*);
double fndCBPPhiBinary(BODY*,SYSTEM*,int*);
double fndCBPZBinary(BODY*,SYSTEM*,int*);
double fndCBPRDotBinary(BODY*,SYSTEM*,int*);
double fndCBPZDotBinary(BODY*,SYSTEM*,int*);
double fndCBPPhiDotBinary(BODY*,SYSTEM*,int*);

/* Misc functions */
double fndFluxExactBinary(BODY*,int,double,double);
double fndFluxApproxBinary(BODY*,int);
double fndApproxEqTemp(BODY*,int,double);
double fndApproxInsol(BODY*,int);
void fnvBinaryDebug(BODY*);
