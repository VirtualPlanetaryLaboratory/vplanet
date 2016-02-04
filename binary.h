/***************** BINARY.H *********************** 
 *
 * David Fleming (dflemin3), Tue Jan 12 10:23am PDT 2016
 *
 * This header file contains all the subroutines in
 * file binary.c.
 *
*/

/* Options Info */
/* For options and output, binary has 2000-2100 */
#define OPTSTARTBINARY          2000 /* Start of Binary options */
#define OPTENDBINARY            2100 /* End of Binary options */

#define OPT_FREEECC             2010 // Free eccentricity
#define OPT_FREEINC             2020 // Free inclination
#define OPT_LL13N0              2030 // LL13 Mean Motion

/* Binary 2000 - 2099 */
#define OUTSTARTBINARY          2000
#define OUTENDBINARY            2100

void InitializeModuleBinary(CONTROL*,MODULE*);
void InitializeControlBinary(CONTROL*);
void AddModuleBinary(MODULE*,int,int);
void BodyCopyBinary(BODY*,BODY*,int,int);
void InitializeBodyBinary(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyBinary(BODY*,CONTROL*,UPDATE*,int);



/* Options Functions */
void HelpOptionsBinary(OPTIONS*);
void InitializeOptionsBinary(OPTIONS*,fnReadOption[]);
void ReadOptionsBinary(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);
void ReadFreeEcc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadFreeInc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadLL13N0(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);

/* Halt Functions */

void VerifyHaltBinary(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt);
void CountHaltsBinary(HALT*,int*);

/* Verify Functions */
void VerifyBinary(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void InitializeVplanetBinary(CONTROL*,MODULE*);
void fnPropertiesBinary(BODY*,UPDATE*,int);
void fnForceBehaviorAtmEsc(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,int iBody,int iModule);

/* Update functions */
void InitializeUpdateBinary(BODY*,UPDATE*,int);

/* Output Functinos */

void HelpOutputBinary(OUTPUT*);
void InitializeOutputFunctionBinary(OUTPUT*,int,int);
void InitializeOutputBinary(OUTPUT*,fnWriteOutput[]);
void FinalizeOutputFunctionBinary(OUTPUT*,int,int);

/* Logging Functions */
void LogOptionsBinary(CONTROL*,FILE*);
void LogBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyBinary(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Binary functions */
//TODO
