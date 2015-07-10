/********************** OUTPUT.C ********************/ 
/*
 * Rory Barnes, Wed May  7 16:39:18 PDT 2014 
 * 
 * Integers for output parameters. To add a new output:
 * 1) Add new definition here
 * 2) Add output properties to InitializeOutput 
 * 3) Write new subroutine that returns desired output
 * 4) Write new fnWriteOutput function
 *
*/

/* General Outuput 0-999 */
/* System properties 0-499, body properties 500-999 */
#define OUTSTART                0
#define OUTBODYSTART            500
#define OUTEND                  1000

#define OUT_AGE			160
#define OUT_TIME		170
#define OUT_TOTANGMOM		180

#define OUT_TOTENERGY		190

#define OUT_POTENERGY           191
#define OUT_KINENERGY           192

#define OUT_DT			200

#define OUT_MASS                500
#define OUT_OBL                 510
#define OUT_RADIUS              520
#define OUT_ROTANGMOM		530
#define OUT_ROTKINENERGY        535
#define OUT_ROTVEL		540
#define OUT_ROTRATE             550
#define OUT_ROTPER              560

#define OUT_SURFENFLUX          580

#define OUT_ORBECC		610
#define OUT_ORBEN		620 
#define OUT_ORBMEANMOTION	630
#define OUT_ORBPER		640
#define OUT_ORBSEMI		650
#define OUT_ORBANGMOM		660

#define OUT_ORBKINENERGY        670
#define OUT_ORBPOTENERGY        680
#define OUT_ORBENERGY           690

void WriteLogEntry(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput,FILE*,int);

void InitializeOutputFunctions(MODULE*,OUTPUT*,int);
void WriteOutput(BODY*,CONTROL*,FILES*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput*,double,double);
void WriteLog(BODY*,CONTROL*,FILES*,MODULE*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,fnWriteOutput*,int);
void InitializeOutput(OUTPUT*,fnWriteOutput*);
