/***************** DISTRES.H *********************** 
 *
 * Russell Deitrick, February 2017
 *
 * This header file contains all the subroutines in
 * file distres.c.
 *
*/

#define RESNUM      10
#define RESMAX      8

void AddModuleDistRes(MODULE*,int,int);
void BodyCopyDistRes(BODY*,BODY*,int,int,int);

/* Options Info */

#define OPTSTARTDISTRES       2300 /* Start of DISTRES options */
#define OPTENDDISTRES         2400 /* End of DISTRES options */

#define OPT_MEANL             2301
#define OPT_RESAVG            2399
// #define OPT_MEANA             2302

/* Options Functions */

void InitializeOptionsDistRes(OPTIONS*, fnReadOption[]);
void ReadOptionsDistRes(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);
void InitializeUpdateTmpBodyDistRes(BODY*,CONTROL*,UPDATE*,int);

/* Verify Functions */
void VerifyDistRes(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* Update Functions */


/* Output Functinos */

/* DISTRES 2300 - 2399 */
#define OUTSTARTDISTRES        2300  /* Start of DISTRES options */
#define OUTENDDISTRES          2400 /* End of DISTRES options */
#define OUTBODYSTARTDISTRES    2320 /* Start of DISTRES BODY options */

#define OUT_MEANL               2320
#define OUT_DMEANLDTDISTRES     2321

void HelpOutputDistRes(OUTPUT*);


void InitializeOutputDistRes(OUTPUT*,fnWriteOutput[]);

/* Logging Functions */
void LogOptionsDistRes(CONTROL*,FILE*);
void LogDistRes(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyDistRes(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* DistRes Functions */
void PropsAuxDistRes(BODY*,EVOLVE*,UPDATE*,int);
void ForceBehaviorDistRes(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

double fdSemiMajAxF27(double, int);
double fdSemiMajAxF28(double, int);
double fdSemiMajAxF29(double, int);
double fdSemiMajAxF30(double, int);
double fdSemiMajAxF31(double, int);
double fdSemiMajAxF32(double, int);
double fdSemiMajAxF33(double, int);
double fdSemiMajAxF34(double, int);
double fdSemiMajAxF35(double, int);
double fdSemiMajAxF36(double, int);
double fdSemiMajAxF37(double, int);
double fdSemiMajAxF38(double, int);
double fdSemiMajAxF39(double, int);
double fdSemiMajAxF40(double, int);
double fdSemiMajAxF41(double, int);
double fdSemiMajAxF42(double, int);
double fdSemiMajAxF43(double, int);
double fdSemiMajAxF44(double, int);
double fdSemiMajAxF45(double, int);
double fdSemiMajAxF49(double, int);
double fdSemiMajAxF53(double, int);
double fdSemiMajAxF57(double, int);
double fdSemiMajAxF62(double, int);
double fdSemiMajAxF82(double, int);
double fdSemiMajAxF83(double, int);
double fdSemiMajAxF84(double, int);
double fdSemiMajAxF85(double, int);
double fdSemiMajAxF86(double, int);
double fdSemiMajAxF87(double, int);
double fdSemiMajAxF88(double, int);
double fdSemiMajAxF89(double, int);

double fdDSemiF27Dalpha(double, int);
double fdDSemiF28Dalpha(double, int);
double fdDSemiF29Dalpha(double, int);
double fdDSemiF30Dalpha(double, int);
double fdDSemiF31Dalpha(double, int);
double fdDSemiF32Dalpha(double, int);
double fdDSemiF33Dalpha(double, int);
double fdDSemiF34Dalpha(double, int);
double fdDSemiF35Dalpha(double, int);
double fdDSemiF36Dalpha(double, int);
double fdDSemiF37Dalpha(double, int);
double fdDSemiF38Dalpha(double, int);
double fdDSemiF39Dalpha(double, int);
double fdDSemiF40Dalpha(double, int);
double fdDSemiF41Dalpha(double, int);
double fdDSemiF42Dalpha(double, int);
double fdDSemiF43Dalpha(double, int);
double fdDSemiF44Dalpha(double, int);
double fdDSemiF45Dalpha(double, int);
double fdDSemiF49Dalpha(double, int);
double fdDSemiF53Dalpha(double, int);
double fdDSemiF57Dalpha(double, int);
double fdDSemiF62Dalpha(double, int);
double fdDSemiF82Dalpha(double, int);
double fdDSemiF83Dalpha(double, int);
double fdDSemiF84Dalpha(double, int);
double fdDSemiF85Dalpha(double, int);
double fdDSemiF86Dalpha(double, int);
double fdDSemiF87Dalpha(double, int);
double fdDSemiF88Dalpha(double, int);
double fdDSemiF89Dalpha(double, int);

double fdDistfCos(BODY*,SYSTEM*,int,int,int,int);
double fdDistfSin(BODY*,SYSTEM*,int,int,int,int);
double fdLibrFreq2(BODY*,SYSTEM*,int,int,int);
double fdCircFreq(BODY*,SYSTEM*,int,int,int);

void RecalcLaplaceDistRes(BODY*,EVOLVE*,SYSTEM*,int);
void CheckResonance(BODY*,EVOLVE*,SYSTEM*);
/* DistRes's equations */
double fdDistResRD2DpDt(BODY*, SYSTEM*, int*);
double fdDistResRD2DqDt(BODY*, SYSTEM*, int*);
double fdDistResRD2DhDt(BODY*, SYSTEM*, int*);
double fdDistResRD2DkDt(BODY*, SYSTEM*, int*);
double fdDistResRD2DaDt(BODY*, SYSTEM*, int*);
double fdDistResRD2DlDt(BODY*, SYSTEM*, int*);
