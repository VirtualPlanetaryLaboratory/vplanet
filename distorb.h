/***************** DISTORB.H *********************** 
 *
 * Russell Deitrick, June 24, 2015
 *
 * This header file contains all the subroutines in
 * file distorb.c.
 *
*/

/* Orbital model */
#define LL2           1
#define RD4           0
#define TEENY				1e-20 // Already a TINY in vplanet.h! XXX
#define MAXECCDISTORB 0.6627434


#define RADIX 2.0   //factor used by matrix solver in LL2 solution
#define SWAP(g,h) {y = (g); (g) = (h); (h) = y;}

void AddModuleDistOrb(MODULE*,int,int);
void BodyCopyDistOrb(BODY*,BODY*,int,int,int);
void InitializeBodyDistOrb(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyDistOrb(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */

#define OPTSTARTDISTORB        1300 /* Start of DISTORB options */
#define OPTENDDISTORB          1400 /* End of DISTORB options */

#define OPT_LONGA              1302
#define OPT_DFCRIT             1350
#define OPT_GRCORR             1351
#define OPT_INVPLANE           1352
#define OPT_ORMAXECC           1353
#define OPT_HALTHILLSTAB       1354
#define OPT_HALTCLOSEENC       1355
#define OPT_EIGENSET           1370
#define OPT_EIGENVALUE				 1371
#define OPT_EIGENVECTOR				 1372
#define OPT_OUTPUTLAPL         1398
#define OPT_ORBITMODEL         1399

/* Options Functions */
void ReadInc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadLongA(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadLongP(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadArgP(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void InitializeOptionsDistOrb(OPTIONS*, fnReadOption[]);
void ReadOptionsDistOrb(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Verify Functions */
void VerifyPericenter(BODY*,CONTROL*,OPTIONS*,char[],int,int);
void VerifyDistOrb(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* Update Functions */

void InitializeUpdateDistOrb(BODY*,UPDATE*,int);
void FinalizeUpdateHeccDistOrb(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateKeccDistOrb(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdatePincDistOrb(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateQincDistOrb(BODY*,UPDATE*,int*,int,int,int);

/* Output Functinos */

/* DISTORB 1300 - 1399 */
/* System properties 1300-1319, body properties 1320-1399 */

#define OUTSTARTDISTORB        1300 /* Start of DISTORB options */
#define OUTENDDISTORB          1400 /* End of DISTORB options */
#define OUTBODYSTARTDISTORB    1320 /* Start of DISTORB BODY options */

#define OUT_INC                 1321
#define OUT_SINC                1322
#define OUT_LONGA               1323

#define OUT_PINC                1333
#define OUT_QINC                1334

#define OUT_DECCDTDISTORB       1340
#define OUT_DINCDTDISTORB       1341
#define OUT_DSINCDTDISTORB      1342
#define OUT_DLONGADTDISTORB     1343
#define OUT_DLONGPDTDISTORB     1344

#define OUT_DHECCDTDISTORB      1351
#define OUT_DKECCDTDISTORB      1352
#define OUT_DPINCDTDISTORB      1353
#define OUT_DQINCDTDISTORB      1354

int HaltCloseEnc(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
int HaltHillStab(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);

void HelpOutputDistOrb(OUTPUT*);
void WriteBodyDEccDtDistOrb(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDSincDtDistOrb(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDLongPDtDistOrb(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDLongADtDistOrb(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDIncDtDistOrb(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodySinc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyInc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyLongA(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyLongP(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyArgP(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyHecc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyKecc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyPinc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyQinc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDHeccDtDistOrb(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDKeccDtDistOrb(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDPincDtDistOrb(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDQincDtDistOrb(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void InitializeOutputDistOrb(OUTPUT*,fnWriteOutput[]);

/* Logging Functions */
void LogOptionsDistOrb(CONTROL*,FILE*);
void LogDistOrb(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyDistOrb(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* DistOrb Functions */
double MutualHillRad(BODY*,int,int);
double MinOrbitSep2D(BODY*,int,int);
double MinOrbitSep3D(BODY*,int,int);
double signf(double);
double fdLaplaceCoeff(double,int,double);
double fdDerivLaplaceCoeff(int,double,int,double);
unsigned long int factorial(unsigned int);
int Nchoosek(int, int);
int CombCount(int,int,int);
double ABmatrix(BODY*,int,int,int);
double GRCorrMatrix(BODY*,int,int);
void HessEigen(double**,int,double[],double[]);
void ElmHess(double**,int);
void BalanceM(double**,int);
void ludcmp(double**,int,int*,float*);

void LUDecomp(double**,double**,double*,int*,int);

void LUSolve(double**,double*,int*,int);
void BalanceMatrix(double**,int);
void lubksb(double**,int,int*,double[]);
void FindEigenVecEcc(SYSTEM*,int,int);
void FindEigenVecInc(SYSTEM*,int,int);
void SolveEigenVal(BODY*,EVOLVE*,SYSTEM*);
void ScaleEigenVec(BODY*,EVOLVE*,SYSTEM*);

void RecalcLaplace(BODY*,EVOLVE*,SYSTEM*,int);
void RecalcEigenVals(BODY*,EVOLVE*,SYSTEM*);

void kepler_eqn(BODY*, int);
void inv_plane(BODY*,SYSTEM*,int);
void cross(double*,double*,double*);
void osc2cart(BODY*, int);
void cart2osc(BODY*, int);

void PropsAuxDistOrb(BODY*,EVOLVE*,UPDATE*,int);
void ForceBehaviorDistOrb(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

double xinit(BODY*,int);
double yinit(BODY*,int);
double xangle1(BODY*,int);
double xangle2(BODY*,int);
double yangle1(BODY*,int);
double yangle2(BODY*,int);
double zangle1(BODY*,int);
double zangle2(BODY*,int);

double fdSemiMajAxF1(double, int);
double fdSemiMajAxF2(double, int);
double fdSemiMajAxF3(double, int);
double fdSemiMajAxF4(double, int);
double fdSemiMajAxF5(double, int);
double fdSemiMajAxF6(double, int);
double fdSemiMajAxF7(double, int);
double fdSemiMajAxF8(double, int);
double fdSemiMajAxF9(double, int);
double fdSemiMajAxF10(double, int);
double fdSemiMajAxF11(double, int);
double fdSemiMajAxF12(double, int);
double fdSemiMajAxF13(double, int);
double fdSemiMajAxF14(double, int);
double fdSemiMajAxF15(double, int);
double fdSemiMajAxF16(double, int);
double fdSemiMajAxF17(double, int);
double fdSemiMajAxF18(double, int);
double fdSemiMajAxF19(double, int);
double fdSemiMajAxF20(double, int);
double fdSemiMajAxF21(double, int);
double fdSemiMajAxF22(double, int);
double fdSemiMajAxF23(double, int);
double fdSemiMajAxF24(double, int);
double fdSemiMajAxF25(double, int);
double fdSemiMajAxF26(double, int);

double fdDSemiF1Dalpha(double, int);
double fdDSemiF2Dalpha(double, int);
double fdDSemiF3Dalpha(double, int);
double fdDSemiF4Dalpha(double, int);
double fdDSemiF5Dalpha(double, int);
double fdDSemiF6Dalpha(double, int);
double fdDSemiF7Dalpha(double, int);
double fdDSemiF8Dalpha(double, int);
double fdDSemiF9Dalpha(double, int);
double fdDSemiF10Dalpha(double, int);
double fdDSemiF11Dalpha(double, int);
double fdDSemiF12Dalpha(double, int);
double fdDSemiF13Dalpha(double, int);
double fdDSemiF14Dalpha(double, int);
double fdDSemiF15Dalpha(double, int);
double fdDSemiF16Dalpha(double, int);
double fdDSemiF17Dalpha(double, int);
double fdDSemiF18Dalpha(double, int);
double fdDSemiF19Dalpha(double, int);
double fdDSemiF20Dalpha(double, int);
double fdDSemiF21Dalpha(double, int);
double fdDSemiF22Dalpha(double, int);
double fdDSemiF23Dalpha(double, int);
double fdDSemiF24Dalpha(double, int);
double fdDSemiF25Dalpha(double, int);
double fdDSemiF26Dalpha(double, int);

double fdApsidalGRCorrection(BODY*, int*);
double fdApsidalGRDhDt(BODY*, SYSTEM*, int*);
double fdApsidalGRDkDt(BODY*, SYSTEM*, int*);

/* DistOrb's equations in h,k,p,q */
double fdDistOrbRD4DhDt(BODY*, SYSTEM*, int*);
double fdDistOrbRD4DkDt(BODY*, SYSTEM*, int*);
double fdDistOrbRD4DpDt(BODY*, SYSTEM*, int*);
double fdDistOrbRD4DqDt(BODY*, SYSTEM*, int*);

double fdDistOrbLL2Hecc(BODY*, SYSTEM*, int*);
double fdDistOrbLL2Kecc(BODY*, SYSTEM*, int*);
double fdDistOrbLL2Pinc(BODY*, SYSTEM*, int*);
double fdDistOrbLL2Qinc(BODY*, SYSTEM*, int*);
double fdDistOrbLL2DhDt(BODY*, SYSTEM*, int*);
double fdDistOrbLL2DkDt(BODY*, SYSTEM*, int*);
double fdDistOrbLL2DpDt(BODY*, SYSTEM*, int*);
double fdDistOrbLL2DqDt(BODY*, SYSTEM*, int*);

