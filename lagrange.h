/***************** LAGRANGE.H *********************** 
 *
 * Russell Deitrick, June 24, 2015
 *
 * This header file contains all the subroutines in
 * file lagrange.c.
 *
*/

void InitializeControlLagrange(CONTROL*);
void AddModuleLagrange(MODULE*,int,int);
void BodyCopyLagrange(BODY*,BODY*,int,int);
void InitializeBodyLagrange(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyLagrange(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */

#define OPTSTARTLAGRANGE        1300 /* Start of LAGRANGE options */
#define OPTENDLAGRANGE          1400 /* End of LAGRANGE options */

#define OPT_INC                1301
#define OPT_LONGA              1302
#define OPT_LONGP              1303
#define OPT_ARGP               1304

/* Options Functions */
void ReadInc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadLongA(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadLongP(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadArgP(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void InitializeOptionsLagrange(OPTIONS*, fnReadOption[]);
void ReadOptionsLagrange(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Verify Functions */
void VerifyPericenter(BODY*,CONTROL*,OPTIONS*,char[],int,int);
void VerifyLagrange(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* Update Functions */

void InitializeUpdateLagrange(BODY*,UPDATE*,int);
void FinalizeUpdateHeccLagrange(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateKeccLagrange(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdatePincLagrange(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateQincLagrange(BODY*,UPDATE*,int*,int,int);

/* Output Functinos */

/* LAGRANGE 1300 - 1399 */
/* System properties 1300-1319, body properties 1320-1399 */

#define OUTSTARTLAGRANGE        1300 /* Start of LAGRANGE options */
#define OUTENDLAGRANGE          1400 /* End of LAGRANGE options */
#define OUTBODYSTARTLAGRANGE    1320 /* Start of LAGRANGE BODY options */


#define OUT_INC                 1321
#define OUT_SINC                1322
#define OUT_LONGA               1323
#define OUT_LONGP               1324
#define OUT_ARGP                1325

#define OUT_HECC                1331
#define OUT_KECC                1332
#define OUT_PINC                1333
#define OUT_QINC                1334

#define OUT_DECCDTLAGRANGE       1340
#define OUT_DINCDTLAGRANGE       1341
#define OUT_DSINCDTLAGRANGE      1342
#define OUT_DLONGADTLAGRANGE     1343
#define OUT_DLONGPDTLAGRANGE     1344

#define OUT_DHECCDTLAGRANGE      1351
#define OUT_DKECCDTLAGRANGE      1352
#define OUT_DPINCDTLAGRANGE      1353
#define OUT_DQINCDTLAGRANGE      1354

void HelpOutputLagrange(OUTPUT*);
void WriteBodyDEccDtLagrange(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDSincDtLagrange(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDLongPDtLagrange(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDLongADtLagrange(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDIncDtLagrange(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodySinc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyInc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyLongA(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyLongP(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyArgP(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyHecc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyKecc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyPinc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyQinc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDHeccDtLagrange(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDKeccDtLagrange(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDPincDtLagrange(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDQincDtLagrange(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void InitializeOutputLagrange(OUTPUT*,fnWriteOutput[]);

/* Logging Functions */
void LogOptionsLagrange(CONTROL*,FILE*);
void LogLagrange(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyLagrange(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Lagrange Functions */
void PropertiesLagrange(BODY*,UPDATE*,int);
void ForceBehaviorLagrange(BODY*,EVOLVE*,IO*,int,int);

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

/* Lagrange's equations in h,k,p,q */
double fdLagrangeDhDt(BODY*, SYSTEM*, int*);
double fdLagrangeDkDt(BODY*, SYSTEM*, int*);
double fdLagrangeDpDt(BODY*, SYSTEM*, int*);
double fdLagrangeDqDt(BODY*, SYSTEM*, int*);
