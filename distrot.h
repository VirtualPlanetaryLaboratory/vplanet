/***************** DISTROT.H *********************** 
 *
 * Russell Deitrick, July 7, 2015
 *
 * This header file contains all the subroutines in
 * file distrot.c.
 *
*/

void InitializeControlDistRot(CONTROL*);
void AddModuleDistRot(MODULE*,int,int);
void BodyCopyDistRot(BODY*,BODY*,int,int);
void InitializeBodyDistRot(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyDistRot(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */

#define OPTSTARTDISTROT        1400 /* Start of DISTROT options */
#define OPTENDDISTROT        1500 /* End of DISTROT options */

#define OPT_PRECA                1401
#define OPT_DYNELLIP              1402


/* Options Functions */
void ReadPrecA(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadDynEllip(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void InitializeOptionsDistRot(OPTIONS*, fnReadOption[]);
void ReadOptionsDistRot(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Verify Functions */
void VerifyDistRot(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* Update Functions */

void InitializeUpdateDistRot(BODY*,UPDATE*,int);
void FinalizeUpdateXoblDistRot(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateYoblDistRot(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateZoblDistRot(BODY*,UPDATE*,int*,int,int);

/* Output Functinos */

/* DISTROT 1400 - 1499 */
/* System properties 1400-1419, body properties 1420-1499 */

#define OUTSTARTDISTROT        1400 /* Start of DISTROT options */
#define OUTENDDISTROT          1500 /* End of DISTROT options */
#define OUTBODYSTARTDISTROT   1420 /* Start of DISTROT BODY options */


#define OUT_PRECA               1421
#define OUT_XOBL                1422
#define OUT_YOBL                1423
#define OUT_ZOBL                1424

#define OUT_DOBLDTDISTROT        1440
#define OUT_DPRECADTDISTROT     1441
#define OUT_DXOBLDTDISTROT       1442
#define OUT_DYOBLDTDISTROT      1443
#define OUT_DZOBLDTDISTROT      1444

void HelpOutputDistRot(OUTPUT*);
void WriteBodyDOblDtDistRot(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDPrecADtDistRot(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDXoblDtDistRot(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDYoblADtDistRot(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyDZoblDtDistRot(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyPrecA(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyXobl(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyYobl(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteBodyZobl(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void InitializeOutputDistRot(OUTPUT*,fnWriteOutput[]);

/* Logging Functions */
void LogOptionsDistRot(CONTROL*,FILE*);
void LogDistRot(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyDistRot(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* DistRot Functions */
void PropertiesDistRot(BODY*,UPDATE*,int);
void ForceBehaviorDistRot(BODY*,EVOLVE*,IO*,int,int);

/* DistRot's equations */
double fdDistRotDxDt(BODY*, SYSTEM*, int*);
double fdDistRotDyDt(BODY*, SYSTEM*, int*);
double fdDistRotDzDt(BODY*, SYSTEM*, int*);
double fdAxialGRDyDt(BODY*, SYSTEM*, int*);
double fdAxialGRDxDt(BODY*, SYSTEM*, int*);
