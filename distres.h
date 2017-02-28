/***************** DISTRES.H *********************** 
 *
 * Russell Deitrick, February 2017
 *
 * This header file contains all the subroutines in
 * file distres.c.
 *
*/

void AddModuleDistRes(MODULE*,int,int);
void BodyCopyDistRes(BODY*,BODY*,int,int,int);

/* Options Info */

#define OPTSTARTDISTRES       2300 /* Start of DISTRES options */
#define OPTENDDISTRES         2400 /* End of DISTRES options */

#define OPT_MEANL             2301
#define OPT_MEANA             2302

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

void HelpOutputDistRes(OUTPUT*);


void InitializeOutputDistRes(OUTPUT*,fnWriteOutput[]);

/* Logging Functions */
void LogOptionsDistRes(CONTROL*,FILE*);
void LogDistRes(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyDistRes(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* DistRes Functions */
void PropertiesDistRes(BODY*,EVOLVE*,UPDATE*,int);
void ForceBehaviorDistRes(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* DistRes's equations */
double fdDistResRD2DpDt(BODY*, SYSTEM*, int*);
double fdDistResRD2DqDt(BODY*, SYSTEM*, int*);
double fdDistResRD2DhDt(BODY*, SYSTEM*, int*);
double fdDistResRD2DkDt(BODY*, SYSTEM*, int*);
double fdDistResRD2DaDt(BODY*, SYSTEM*, int*);
double fdDistResRD2DlDt(BODY*, SYSTEM*, int*);
