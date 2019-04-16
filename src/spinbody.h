/**
  @file spinbody.h
  @brief Subroutines that control the integration of the N Body simulation
  @author Hayden Smotherman ([smotherh](https://github.com/smotherh/))
  @date Feb 21 2017
*/

//Option numbers
#define OPTSTARTSPINBODY        1600
#define OPTENDSPINBODY          1700

#define OPT_POSITIONXSPINBODY   1610
#define OPT_POSITIONYSPINBODY   1611
#define OPT_POSITIONZSPINBODY   1612

#define OPT_VELXSPINBODY        1620
#define OPT_VELYSPINBODY        1621
#define OPT_VELZSPINBODY        1622

#define OPT_MEANA               1630

#define OPT_USEORBPARAMS        1640

//Output numbers
#define OUTSTARTSPINBODY        1600
#define OUTENDSPINBODY          1700
#define OUTBODYSTARTSPINBODY    1610

#define OUT_POSITIONXSPINBODY   1610
#define OUT_POSITIONYSPINBODY   1611
#define OUT_POSITIONZSPINBODY   1612

#define OUT_VELXSPINBODY        1620
#define OUT_VELYSPINBODY        1621
#define OUT_VELZSPINBODY        1622

#define OUT_INCSPINBODY         1630
#define OUT_LONGASPINBODY       1631

/* @cond DOXYGEN_OVERRIDE */

void AddModuleSpiNBody(CONTROL*,MODULE*,int,int);
void BodyCopySpiNBody(BODY*,BODY*,int,int,int);

void InitializeOptionsSpiNBody(OPTIONS *options,fnReadOption fnRead[]);
void ReadOptionsSpiNBody(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody);
void ReadPositionX(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile);
void ReadPositionY(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile);
void ReadPositionZ(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile);
void ReadVelX(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile);
void ReadVelY(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile);
void ReadVelZ(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile);

void InitializeBodySpiNBody(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule);
void InitializeUpdateSpiNBody(BODY *body,UPDATE *update,int iBody);
void InitializeUpdateTmpBodySpiNBody(BODY *body,CONTROL *control,UPDATE *update,int iBody);

void VerifyPositionX(BODY *body,OPTIONS *options, UPDATE *update, double dAge, int iBody);
void VerifyPositionY(BODY *body,OPTIONS *options, UPDATE *update, double dAge, int iBody);
void VerifyPositionZ(BODY *body,OPTIONS *options, UPDATE *update, double dAge, int iBody);
void VerifyVelX(BODY *body,OPTIONS *options, UPDATE *update, double dAge, int iBody);
void VerifyVelY(BODY *body,OPTIONS *options, UPDATE *update, double dAge, int iBody);
void VerifyVelZ(BODY *body,OPTIONS *options, UPDATE *update, double dAge, int iBody);

void VerifyPerturbersSpiNBody(BODY *body,int iNumBodies,int iBody);

void VerifySpiNBody(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,int iBody,int iModule);

void fnForceBehaviorSpiNBody(BODY *body,MODULE *module,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule);
void PropertiesSpiNBody(BODY *body, EVOLVE *evolve, UPDATE *update, int iBody);

void FinalizeUpdateVelXSpiNBody(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo);
void FinalizeUpdateVelYSpiNBody(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo);
void FinalizeUpdateVelZSpiNBody(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo);
void FinalizeUpdatePositionXSpiNBody(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo);
void FinalizeUpdatePositionYSpiNBody(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo);
void FinalizeUpdatePositionZSpiNBody(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo);

void WritePositionX(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]);
void WritePositionY(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]);
void WritePositionZ(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]);
void WriteVelX(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]);
void WriteVelY(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]);
void WriteVelZ(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]);
void WriteInclinationSpinBody(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]);
void WriteLongASpinBody(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]);
void InitializeOutputSpiNBody(OUTPUT *output,fnWriteOutput fnWrite[]);

// Coordinate Changes
void OrbElems2Helio(BODY *body, int iBody);
void Helio2Bary(BODY *body, int iNumBodies, int iBody);
void Bary2OrbElems(BODY *body, int iBody);
void Bary2Helio(BODY *body, int iBody);
// These functions are defined in distorb.c, but needed in SpiNBody
// Relocate to system.c?

void cross(double* r,double* v,double* h); //h = r X v
void kepler_eqn(BODY *body, int iBody);
double signf(double value);
double xinit(BODY *body, int iBody);
double yinit(BODY *body, int iBody);
double vxi(BODY *body, int iBody);
double vyi(BODY *body, int iBody);
double xangle1(BODY *body, int iBody);
double xangle2(BODY *body, int iBody);
double yangle1(BODY *body, int iBody);
double yangle2(BODY *body, int iBody);
double zangle1(BODY *body, int iBody);
double zangle2(BODY *body, int iBody);

// Halts
void CountHaltsSpiNBody(HALT*,int*);
void VerifyHaltSpiNBody(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt);

void LogBodySpiNBody(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody);
void LogSpiNBody(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);

double fdDPositionXDt(BODY *body, SYSTEM *system, int *iaBody);
double fdDPositionYDt(BODY *body, SYSTEM *system, int *iaBody);
double fdDPositionZDt(BODY *body, SYSTEM *system, int *iaBody);
double fdDVelXDt(BODY *body, SYSTEM *system, int *iaBody);
double fdDVelYDt(BODY *body, SYSTEM *system, int *iaBody);
double fdDVelZDt(BODY *body, SYSTEM *system, int *iaBody);

/* @endcond */
