/**
   @file distorb.h

   @brief Subroutines that control the integration of the orbital model.

   @author Russell Deitrick ([deitrr](https://github.com/deitrr/))

   @date April 28 2015

*/

/* Orbital model */
#define RD4 0
#define LL2 1

// Constants
#define TEENY 1e-20 // Already a dTINY in vplanet.h! XXX
#define MAXECCDISTORB 0.6627434
#define MAXMUTUALINCRD4 35 // Degrees
#define MAXMUTUALINCLL2 10 // Degrees

#define RADIX 2.0 // factor used by matrix solver in LL2 solution
#define SWAP(g, h)                                                             \
  {                                                                            \
    y   = (g);                                                                 \
    (g) = (h);                                                                 \
    (h) = y;                                                                   \
  }

#define A(iIndexJ) dAxRatio, iIndexJ, 0.5
#define B(iIndexJ) dAxRatio, iIndexJ, 1.5
#define C(iIndexJ) dAxRatio, iIndexJ, 2.5

/* For semi-major axis functions */
#define LAPLNUM 26

// A -> s = 1/2
// B -> s = 3/2
// C -> s = 5/2

/* Options Info */

#define OPTSTARTDISTORB 1300 /* Start of DISTORB options */
#define OPTENDDISTORB 1400   /* End of DISTORB options */

#define OPT_DFCRIT 1350
#define OPT_INVPLANE 1352
#define OPT_ORMAXECC 1353
#define OPT_HALTHILLSTAB 1354
#define OPT_HALTCLOSEENC 1355
#define OPT_EIGENSET 1370
#define OPT_EIGENVALUE 1371
#define OPT_EIGENVECTOR 1372
#define OPT_OUTPUTEIGEN 1397
#define OPT_OUTPUTLAPL 1398
#define OPT_ORBITMODEL 1399

/* Output Functinos */

/* DISTORB 1300 - 1399 */
/* System properties 1300-1319, body properties 1320-1399 */

#define OUTSTARTDISTORB 1300     /* Start of DISTORB options */
#define OUTENDDISTORB 1400       /* End of DISTORB options */
#define OUTBODYSTARTDISTORB 1320 /* Start of DISTORB BODY options */

#define OUT_SINC 1322

#define OUT_PINC 1333
#define OUT_QINC 1334

#define OUT_DECCDTDISTORB 1340
#define OUT_DINCDTDISTORB 1341
#define OUT_DSINCDTDISTORB 1342
#define OUT_DLONGADTDISTORB 1343
#define OUT_DLONGPDTDISTORB 1344

#define OUT_DHECCDTDISTORB 1351
#define OUT_DKECCDTDISTORB 1352
#define OUT_DPINCDTDISTORB 1353
#define OUT_DQINCDTDISTORB 1354


/* @cond DOXYGEN_OVERRIDE */

void AddModuleDistOrb(CONTROL *, MODULE *, int, int);
void BodyCopyDistOrb(BODY *, BODY *, int, int, int);
void InitializeBodyDistOrb(BODY *, CONTROL *, UPDATE *, int, int);
void InitializeUpdateTmpBodyDistOrb(BODY *, CONTROL *, UPDATE *, int);

/* Options Functions */
void ReadInc(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadLongA(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadLongP(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadArgP(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void InitializeOptionsDistOrb(OPTIONS *, fnReadOption[]);
void ReadOptionsDistOrb(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                        fnReadOption[], int);

/* Verify Functions */
void VerifyPericenter(BODY *, CONTROL *, OPTIONS *, char[], int, int);
void VerifyDistOrb(BODY *, CONTROL *, FILES *, OPTIONS *, OUTPUT *, SYSTEM *,
                   UPDATE *, int, int);

/* Update Functions */

void InitializeUpdateDistOrb(BODY *, UPDATE *, int);
void FinalizeUpdateHeccDistOrb(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateKeccDistOrb(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdatePincDistOrb(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateQincDistOrb(BODY *, UPDATE *, int *, int, int, int);

int fniHaltCloseEnc(BODY *, EVOLVE *, HALT *, IO *, UPDATE *,
                    fnUpdateVariable ***, int);
int fniHaltHillStab(BODY *, EVOLVE *, HALT *, IO *, UPDATE *,
                    fnUpdateVariable ***, int);
int fbHaltMaxMutualIncDistorb(BODY *, EVOLVE *, HALT *, IO *, UPDATE *,
                              fnUpdateVariable ***, int);

int fbCheckMutualIncDistorb(BODY *, EVOLVE *, HALT *, IO *, UPDATE *, int);

void HelpOutputDistOrb(OUTPUT *);
void WriteBodyDEccDtDistOrb(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                            UPDATE *, int, double *, char[]);
void WriteBodyDSincDtDistOrb(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                             UPDATE *, int, double *, char[]);
void WriteBodyDLongPDtDistOrb(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                              UPDATE *, int, double *, char[]);
void WriteBodyDLongADtDistOrb(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                              UPDATE *, int, double *, char[]);
void WriteBodyDIncDtDistOrb(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                            UPDATE *, int, double *, char[]);
void WriteBodySinc(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char[]);
void WriteBodyLongP(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                    int, double *, char[]);
void WriteBodyArgP(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char[]);
void WriteBodyHecc(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char[]);
void WriteBodyKecc(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char[]);
void WriteBodyPinc(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char[]);
void WriteBodyQinc(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char[]);
void WriteBodyDHeccDtDistOrb(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                             UPDATE *, int, double *, char[]);
void WriteBodyDKeccDtDistOrb(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                             UPDATE *, int, double *, char[]);
void WriteBodyDPincDtDistOrb(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                             UPDATE *, int, double *, char[]);
void WriteBodyDQincDtDistOrb(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                             UPDATE *, int, double *, char[]);
void InitializeOutputDistOrb(OUTPUT *, fnWriteOutput[]);
void WriteEigen(CONTROL *, SYSTEM *);

/* Logging Functions */
void LogOptionsDistOrb(CONTROL *, FILE *);
void LogDistOrb(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                fnWriteOutput[], FILE *);
void LogBodyDistOrb(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                    fnWriteOutput[], FILE *, int);

/* DistOrb Functions */
double fndMutualHillRad(BODY *, int, int);
double fndMinOrbitSep2D(BODY *, int, int);
double fndMinOrbitSep3D(BODY *, int, int);
double fndLaplaceCoeff(double, int, double);
double fndDerivLaplaceCoeff(int, double, int, double);
unsigned long int fniFactorial(unsigned int);
int fniNchoosek(int, int);
int fniCombCount(int, int, int);
double fndABmatrix(BODY *, int, int, int);
double fndGRCorrMatrix(BODY *, int, int);
void HessEigen(double **, int, double[], double[]);
void ElmHess(double **, int);
void BalanceM(double **, int);
void ludcmp(double **, int, int *, float *);

void LUDecomp(double **, double **, double *, int *, int);

void LUSolve(double **, double *, int *, int);
void BalanceMatrix(double **, int);
void lubksb(double **, int, int *, double[]);
void FindEigenVecEcc(SYSTEM *, int, int);
void FindEigenVecInc(SYSTEM *, int, int);
void SolveEigenVal(BODY *, EVOLVE *, SYSTEM *);
void ScaleEigenVec(BODY *, EVOLVE *, SYSTEM *);

void RecalcLaplace(BODY *, EVOLVE *, SYSTEM *, int);
void RecalcEigenVals(BODY *, EVOLVE *, SYSTEM *);

void kepler_eqn(BODY *, int);
void cross(double *, double *, double *);
void osc2cart(BODY *, int);
void cart2osc(BODY *, int);

void PropsAuxDistOrb(BODY *, EVOLVE *, IO *, UPDATE *, int);
void ForceBehaviorDistOrb(BODY *, MODULE *, EVOLVE *, IO *, SYSTEM *, UPDATE *,
                          fnUpdateVariable ***, int, int);

double fndXinit(BODY *, int);
double fndYinit(BODY *, int);
double fndVxi(BODY *, int);
double fndVyi(BODY *, int);
double fndXangle1(BODY *, int);
double fndXangle2(BODY *, int);
double fndYangle1(BODY *, int);
double fndYangle2(BODY *, int);
double fndZangle1(BODY *, int);
double fndZangle2(BODY *, int);

double fndSemiMajAxF1(double, int);
double fndSemiMajAxF2(double, int);
double fndSemiMajAxF3(double, int);
double fndSemiMajAxF4(double, int);
double fndSemiMajAxF5(double, int);
double fndSemiMajAxF6(double, int);
double fndSemiMajAxF7(double, int);
double fndSemiMajAxF8(double, int);
double fndSemiMajAxF9(double, int);
double fndSemiMajAxF10(double, int);
double fndSemiMajAxF11(double, int);
double fndSemiMajAxF12(double, int);
double fndSemiMajAxF13(double, int);
double fndSemiMajAxF14(double, int);
double fndSemiMajAxF15(double, int);
double fndSemiMajAxF16(double, int);
double fndSemiMajAxF17(double, int);
double fndSemiMajAxF18(double, int);
double fndSemiMajAxF19(double, int);
double fndSemiMajAxF20(double, int);
double fndSemiMajAxF21(double, int);
double fndSemiMajAxF22(double, int);
double fndSemiMajAxF23(double, int);
double fndSemiMajAxF24(double, int);
double fndSemiMajAxF25(double, int);
double fndSemiMajAxF26(double, int);

double fndDSemiF1Dalpha(double, int);
double fndDSemiF2Dalpha(double, int);
double fndDSemiF3Dalpha(double, int);
double fndDSemiF4Dalpha(double, int);
double fndDSemiF5Dalpha(double, int);
double fndDSemiF6Dalpha(double, int);
double fndDSemiF7Dalpha(double, int);
double fndDSemiF8Dalpha(double, int);
double fndDSemiF9Dalpha(double, int);
double fndDSemiF10Dalpha(double, int);
double fndDSemiF11Dalpha(double, int);
double fndDSemiF12Dalpha(double, int);
double fndDSemiF13Dalpha(double, int);
double fndDSemiF14Dalpha(double, int);
double fndDSemiF15Dalpha(double, int);
double fndDSemiF16Dalpha(double, int);
double fndDSemiF17Dalpha(double, int);
double fndDSemiF18Dalpha(double, int);
double fndDSemiF19Dalpha(double, int);
double fndDSemiF20Dalpha(double, int);
double fndDSemiF21Dalpha(double, int);
double fndDSemiF22Dalpha(double, int);
double fndDSemiF23Dalpha(double, int);
double fndDSemiF24Dalpha(double, int);
double fndDSemiF25Dalpha(double, int);
double fndDSemiF26Dalpha(double, int);

double fndApsidalGRCorrection(BODY *, int *);
double fndApsidalGRDhDt(BODY *, SYSTEM *, int *);
double fndApsidalGRDkDt(BODY *, SYSTEM *, int *);

/* DistOrb's equations in h,k,p,q */
double fndDistOrbRD4DhDt(BODY *, SYSTEM *, int *);
double fndDistOrbRD4DkDt(BODY *, SYSTEM *, int *);
double fndDistOrbRD4DpDt(BODY *, SYSTEM *, int *);
double fndDistOrbRD4DqDt(BODY *, SYSTEM *, int *);

double fndDistOrbLL2Hecc(BODY *, SYSTEM *, int *);
double fndDistOrbLL2Kecc(BODY *, SYSTEM *, int *);
double fndDistOrbLL2Pinc(BODY *, SYSTEM *, int *);
double fndDistOrbLL2Qinc(BODY *, SYSTEM *, int *);
double fndDistOrbLL2DhDt(BODY *, SYSTEM *, int *);
double fndDistOrbLL2DkDt(BODY *, SYSTEM *, int *);
double fndDistOrbLL2DpDt(BODY *, SYSTEM *, int *);
double fndDistOrbLL2DqDt(BODY *, SYSTEM *, int *);

double fdInclination(BODY *, int);
double fdLongA(BODY *, int);
/* @endcond */
