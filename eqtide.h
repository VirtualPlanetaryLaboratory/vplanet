/***************** EQTIDE.H *********************** 
 *
 * Rory Barnes, Wed May  7 13:57:29 PDT 2014
 *
 * This header file contains all the subroutines in
 * file eqtide.c.
 *
*/

void  InitializeControlEqtide(CONTROL*);

/* Options Info */

#define OPTSTARTEQTIDE          1000 /* Start of Eqtide options */
#define OPTENDEQTIDE            1100 /* End of Eqtide options */

#define OPT_DISCRETEROT         1005
#define OPT_FORCEEQSPIN         1010 
#define OPT_HALTDBLSYNC	        1015
#define OPT_HALTLOCK            1020
#define OPT_HALTSYNC            1025
#define OPT_K2                  1027
#define OPT_MAXLOCKDIFF         1030
#define OPT_SYNCECC             1035
#define OPT_TIDEMODEL		1040
#define OPT_TIDALTAU		1045
#define OPT_TIDALQ		1050

/* Options Functions */
void HelpOptionsEqtide(OPTIONS*);
void ReadDiscreteRot(CONTROL*,BODY*,SYSTEM*,OPTIONS*,FILES*,int) ;
void ReadHaltDblSync(CONTROL*,BODY*,SYSTEM*,OPTIONS*,FILES*,int);
void ReadForceEqSpin(CONTROL*,BODY*,SYSTEM*,OPTIONS*,FILES*,int); 
void ReadHaltTideLock(CONTROL*,BODY*,SYSTEM*,OPTIONS*,FILES*,int);
void ReadHaltSyncRot(CONTROL*,BODY*,SYSTEM*,OPTIONS*,FILES*,int);
void ReadK2(CONTROL*,BODY*,SYSTEM*,OPTIONS*,FILES*,int);
void ReadMaxLockDiff(CONTROL*,BODY*,SYSTEM*,OPTIONS*,FILES*,int);
void ReadSyncEcc(CONTROL*,BODY*,SYSTEM*,OPTIONS*,FILES*,int);
void ReadTideModel(CONTROL*,BODY*,SYSTEM*,OPTIONS*,FILES*,int);
void ReadTidalQ(CONTROL*,BODY*,SYSTEM*,OPTIONS*,FILES*,int);
void ReadTidalTau(CONTROL*,BODY*,SYSTEM*,OPTIONS*,FILES*,int);
void InitializeOptionsEqtide(OPTIONS*,fnReadOption[]);
void ReadOptionsEqtide(CONTROL*,OPTIONS*,BODY*,SYSTEM*,FILES*,fnReadOption[]);

/* Halt Functions */
#define EQTIDEHALTSYSEND       5
#define EQTIDEHALTBODYEND      5

int HaltDblSync(CONTROL*,BODY*,UPDATE*);
int HaltTideLock(CONTROL*,BODY*,UPDATE*,int);
int HaltSyncRot(CONTROL*,BODY*,UPDATE*,int);
void InitializeHaltEqtide(HALT*,VPLANET*,int,int*,int*);

/* Verify Functions */

void VerifyRotationEqtideWarning(char[],char[],char[],int,int,int);
void VerifyRotationEqtide(CONTROL*,BODY*,OPTIONS*,char[],int); 
void VerifyEqtide(CONTROL*,OUTPUT*,FILES*,BODY*,OPTIONS*,fnUpdateVariable***,UPDATE*);
void InitializeVplanetEqtide(CONTROL*,VPLANET*);

/* Output Functinos */

/* EQTIDE 1000 - 1999 */
/* System properties 1000-1049, body properties 1050-1099 */
#define OUTSTARTEQTIDE          1000
#define OUTBODYSTARTEQTIDE      1050
#define OUTENDEQTIDE            1100

#define OUT_DSEMIDTEQTIDE	1010
#define OUT_DECCDTEQTIDE	1015
#define OUT_DMMDTEQTIDE	        1020
#define OUT_DORBPERDTEQTIDE	1025
#define OUT_DORBEPSILON		1030
#define OUT_ECCTIMEEQTIDE       1035
#define OUT_SEMITIMEEQTIDE      1040

/* Body Properties due to tidal evolution */

#define OUT_BODYDSEMIDTEQTIDE	1052
#define OUT_BODYDECCDTEQTIDE	1054

#define OUT_DOBLDTEQTIDE	1056
#define OUT_DROTPERDTEQTIDE	1058
#define OUT_DROTRATEDTEQTIDE	1060
#define OUT_EQROTRATEEQTIDE     1062
#define OUT_EQROTRATEDISCRETE   1064
#define OUT_EQROTPERDISCRETE    1066
#define OUT_EQROTRATECONT	1068
#define OUT_EQROTPERCONT	1070
#define OUT_EQROTPER	        1072
#define OUT_EQROTRATE	        1074

#define OUT_GAMMAROT	        1078
#define OUT_GAMMAORB            1080

#define OUT_TIDALQ              1084
#define OUT_TIDALTAU		1086
#define OUT_TIDELOCK            1088
#define OUT_ROTRATETIMEEQTIDE   1090
#define OUT_OBLTIMEEQTIDE       1092
#define OUT_HEATEQTIDE	        1094
#define OUT_HEATFLUXEQTIDE	1096

void HelpOutputEqtide(OUTPUT*);
void WriteBodyDsemiDtEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteBodyDeccDtEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteDOblDtEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteDSemiDtEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteDEccDtEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteDMeanMotionDtEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteDOrbPerDtEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteDRotPerDtEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteDRotRateDtEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteEccTimescaleEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteEqRotPer(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteEqRotPerCont(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteEqRotPerDiscrete(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteEqRotRate(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteEqRotRateCont(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteEqRotRateDiscrete(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteGammaOrb(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteGammaRot(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteOblTimescaleEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteRotTimescaleEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteSemiTimescaleEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteHeatEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteHeatFluxEqtide(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteEqHeat(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteEqHeatFlux(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteTidalQ(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteTidalTau(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void WriteTideLock(CONTROL*,UNITS*,OUTPUT,BODY*,SYSTEM*,UPDATE*,int,double*,char[]);
void InitializeOutputEqtide(OUTPUT*,fnWriteOutput[]);

/* Logging Functions */
void LogOptionsEqtide(CONTROL*,FILE*);
void LogEqtide(CONTROL*,OUTPUT*,BODY*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyEqtide(CONTROL*,OUTPUT*,BODY*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Functions common to both CPL and CTL */

void fvPropertiesEqtide(CONTROL*,BODY*);
double fdGetUpdateInfo(CONTROL*,BODY*,UPDATE*,fnUpdateVariable***); /* Returns minimum dynamical timescale, x/(dx/dt) */
void fvChi(BODY *);
double fdEqRotRate(BODY,double,double,int,int);
double fdTideHeat(BODY,double,double,double,int);
double fdTideHeatFlux(BODY,double,double,double,int);
void ForceBehaviorEqtide(BODY*,CONTROL*);

/******************** CPL Functions ********************/

/* Auxiliary parameters */
void fvCPLEpsilon(double,double,int*);
void fvCPLZ(BODY*);
double fdGammaRot(double,double,int*);
double fdGammaOrb(double,double,int*);
double fdCPLTideHeat(BODY,double,double);

/* Equilibrium parameters */
double fdCPLEqSpinRate(double,double,int);
double fdCPLTideHeatEq(double,double,double,double,int);
double fdCPLEqRotRate(double,double,int);
double fdCPLEqRotRateDiscrete(double,double);
double fdCPLEqRotRateCont(double,double);

/* Derivatives */
double fdCPLDsemiDt(BODY*,int);
double fdCPLDeccDt(BODY*,int);
double fdCPLDrotrateDt(BODY*,int);
double fdCPLDobliquityDt(BODY*,int);
double fdCPLDsemiDtBody(BODY,double,double,double);
double fdCPLDeccDtBody(BODY,double,double,double);



/******************** CTL Functions ********************/

/* Auxiliary parameters */
void fvCTLF(BODY*);
void fvCTLZ(BODY *);
double fdCTLF1(double);
double fdCTLF2(double);
double fdCTLF3(double);
double fdCTLF4(double);
double fdCTLBeta(double);
double fdCTLF5(double);
double fdCTLTideHeat(BODY,double,double);

/* Equilibrium parameters */
double fdCTLTideHeatEq(BODY,double);
double fdCTLEqRotRate(double,double,double);

double fdCTLDsemiDt(BODY *,int);
double fdCTLDeccDt(BODY *,int);
double fdCTLDrotrateDt(BODY *,int);
double fdCTLDobliquityDt(BODY *,int);
double fdCTLDsemiDtBody(BODY,double,double,double,double,double);
double fdCTLDeccDtBody(BODY,double,double,double);
