/***************** LASKAR.H *********************** 
 *
 * Russell Deitrick, July 7, 2015
 *
 * This header file contains all the subroutines in
 * file laskar.c.
 *
*/

void InitializeControlLaskar(CONTROL*);
void AddModuleLaskar(MODULE*,int,int);
void BodyCopyLaskar(BODY*,BODY*,int,int);
void InitializeBodyLaskar(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyLaskar(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */

#define OPTSTARTLASKAR        1400 /* Start of LASKAR options */
#define OPTENDLASKAR          1500 /* End of LASKAR options */

#define OPT_PRECA                1401
#define OPT_DYNELLIP              1402


/* Options Functions */
void ReadPrecA(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadDynEllip(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void InitializeOptionsLaskar(OPTIONS*, fnReadOption[]);
void ReadOptionsLaskar(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Verify Functions */
void VerifyLaskar(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* Update Functions */

void InitializeUpdateLaskar(BODY*,UPDATE*,int);
void FinalizeUpdateXoblLaskar(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateYoblLaskar(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateZoblLaskar(BODY*,UPDATE*,int*,int,int);

/* Output Functinos */

/* LASKAR 1400 - 1499 */
/* System properties 1400-1419, body properties 1420-1499 */

#define OUTSTARTLASKAR        1400 /* Start of LASKAR options */
#define OUTENDLASKAR          1500 /* End of LASKAR options */
#define OUTBODYSTARTLASKAR    1420 /* Start of LASKAR BODY options */


#define OUT_PRECA               1421
#define OUT_XOBL                1422
#define OUT_YOBL                1423
#define OUT_ZOBL                1424

#define OUT_DOBLDTLASKAR        1440
#define OUT_DPRECADTLASKAR      1441
#define OUT_DXOBLDTLASKAR       1442
#define OUT_DYOBLDTLASKAR       1443
#define OUT_DZOBLDTLASKAR       1444

void HelpOutputLaskar(OUTPUT*);
void WriteBodyDOblDtLaskar(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDPrecADtLaskar(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDXoblDtLaskar(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDYoblADtLaskar(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDZoblDtLaskar(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyPrecA(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyXobl(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyYobl(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyZobl(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void InitializeOutputLaskar(OUTPUT*,fnWriteOutput[]);

/* Logging Functions */
void LogOptionsLaskar(CONTROL*,FILE*);
void LogLaskar(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyLaskar(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Laskar Functions */
void PropertiesLaskar(BODY*,UPDATE*,int);
void ForceBehaviorLaskar(BODY*,EVOLVE*,IO*,int,int);

/* Laskar's equations in h,k,p,q */
double fdLaskarDxDt(BODY*, SYSTEM*, int*);
double fdLaskarDyDt(BODY*, SYSTEM*, int*);
double fdLaskarDzDt(BODY*, SYSTEM*, int*);
