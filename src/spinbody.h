/**
  @file spinbody.h
  @brief Subroutines that control the integration of the N Body simulation
  @author Hayden Smotherman ([smotherh](https://github.com/smotherh/))
  @date Feb 21 2017
*/

// Option numbers
#define OPTSTARTSPINBODY 1600
#define OPTENDSPINBODY 1700

#define OPT_POSITIONXSPINBODY 1610
#define OPT_POSITIONYSPINBODY 1611
#define OPT_POSITIONZSPINBODY 1612

#define OPT_VELXSPINBODY 1620
#define OPT_VELYSPINBODY 1621
#define OPT_VELZSPINBODY 1622

#define OPT_MEANA 1630
#define OPT_MEANL 1631

#define OPT_USEORBPARAMS 1640
#define OPT_EXCLUDEFROMBARYCENTER 1641
#define OPT_OUTPUTLOCALBARYCOORDS 1642
#define OPT_INPUTCOMMONBARYCOORDS 1643
#define OPT_REMAININLOCALBARYIFEJECTED 1644
#define OPT_CALCCOORDSFROMCENTEROFMASS 1645
#define OPT_HALTBODYUNBOUND 1646
//#define OPT_OUTPUTCOORDBIT 1641

// Output numbers
#define OUTSTARTSPINBODY 1600
#define OUTENDSPINBODY 1700
#define OUTBODYSTARTSPINBODY 1610

/*
#define OUT_POSITIONXSPINBODY 1610
#define OUT_POSITIONYSPINBODY 1611
#define OUT_POSITIONZSPINBODY 1612

#define OUT_VELXSPINBODY 1620
#define OUT_VELYSPINBODY 1621
#define OUT_VELZSPINBODY 1622

#define OUT_INCSPINBODY 1630
#define OUT_LONGASPINBODY 1631
*/

#define OUT_HELIOPOSX 1650
#define OUT_HELIOPOSY 1651
#define OUT_HELIOPOSZ 1652
#define OUT_HELIOVELX 1653
#define OUT_HELIOVELY 1654
#define OUT_HELIOVELZ 1655

#define OUT_BARYPOSX 1660
#define OUT_BARYPOSY 1661
#define OUT_BARYPOSZ 1662
#define OUT_BARYVELX 1663
#define OUT_BARYVELY 1664
#define OUT_BARYVELZ 1665

#define OUT_HELIOSEMI 1670
#define OUT_HELIOECC 1671
#define OUT_HELIOINC 1672
#define OUT_HELIOLONGA 1673
#define OUT_HELIOARGP 1674
#define OUT_HELIOLONGP 1675
#define OUT_HELIOMEANA 1676
#define OUT_HELIOMEANL 1677
#define OUT_HELIOECCA 1678
#define OUT_HELIOHYPA 1679


#define OUT_BARYSEMI 1680
#define OUT_BARYECC 1681
#define OUT_BARYINC 1682
#define OUT_BARYLONGA 1683
#define OUT_BARYARGP 1684
#define OUT_BARYLONGP 1685
#define OUT_BARYMEANA 1686
#define OUT_BARYMEANL 1687
#define OUT_BARYECCA 1688
#define OUT_BARYHYPA 1689

//Other Misc. variables be low
#define OUT_BARYMU 1690
#define OUT_BARYECCSQ 1691
#define OUT_BARYMEANMOTION 1692
#define OUT_BARYORBPERIOD 1693
#define OUT_BARYSINC 1694
#define OUT_HELIOTRUEA 1695
#define OUT_BARYTRUEA 1696

/* @cond DOXYGEN_OVERRIDE */

void AddModuleSpiNBody(CONTROL *, MODULE *, int, int);
void BodyCopySpiNBody(BODY *, BODY *, int, int, int);

void InitializeOptionsSpiNBody(OPTIONS *options, fnReadOption fnRead[]);
void ReadOptionsSpiNBody(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system,
                         fnReadOption fnRead[], int iBody);
void ReadPositionX(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile);
void ReadPositionY(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile);
void ReadPositionZ(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile);
void ReadVelX(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile);
void ReadVelY(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile);
void ReadVelZ(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile);

void InitializeBodySpiNBody(BODY *body, CONTROL *control, UPDATE *update,
                            int iBody, int iModule);
void InitializeUpdateSpiNBody(BODY *body, UPDATE *update, int iBody);
void InitializeUpdateTmpBodySpiNBody(BODY *body, CONTROL *control,
                                     UPDATE *update, int iBody);

void VerifyPositionX(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                     int iBody);
void VerifyPositionY(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                     int iBody);
void VerifyPositionZ(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                     int iBody);
void VerifyVelX(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                int iBody);
void VerifyVelY(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                int iBody);
void VerifyVelZ(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                int iBody);

void VerifyPerturbersSpiNBody(BODY *body, int iNumBodies, int iBody);

void VerifySpiNBody(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, OUTPUT *output, SYSTEM *system,
                    UPDATE *update, int iBody, int iModule);

void fnForceBehaviorSpiNBody(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                             SYSTEM *system, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody,
                             int iModule);
void PropsAuxSpiNBody(BODY *body, EVOLVE *evolve, IO *, UPDATE *update,
                      int iBody);

void FinalizeUpdateVelXSpiNBody(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo);
void FinalizeUpdateVelYSpiNBody(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo);
void FinalizeUpdateVelZSpiNBody(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo);
void FinalizeUpdatePositionXSpiNBody(BODY *body, UPDATE *update, int *iEqn,
                                     int iVar, int iBody, int iFoo);
void FinalizeUpdatePositionYSpiNBody(BODY *body, UPDATE *update, int *iEqn,
                                     int iVar, int iBody, int iFoo);
void FinalizeUpdatePositionZSpiNBody(BODY *body, UPDATE *update, int *iEqn,
                                     int iVar, int iBody, int iFoo);

void WritePositionX(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]);
void WritePositionY(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]);
void WritePositionZ(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                    double *dTmp, char cUnit[]);
void WriteVelX(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char cUnit[]);
void WriteVelY(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char cUnit[]);
void WriteVelZ(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UNITS *units, UPDATE *update, int iBody, double *dTmp,
               char cUnit[]);
void WriteInclinationSpinBody(BODY *body, CONTROL *control, OUTPUT *output,
                              SYSTEM *system, UNITS *units, UPDATE *update,
                              int iBody, double *dTmp, char cUnit[]);
void WriteLongASpinBody(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]);
void InitializeOutputSpiNBody(OUTPUT *output, fnWriteOutput fnWrite[]);

int fbHaltMaxMutualIncSpiNBody(BODY *, EVOLVE *, HALT *, IO *, UPDATE *,
                               fnUpdateVariable ***, int);

// Addressing total mass of the system for Barycentric Coordinates
double TotalMass(BODY *body, int iNumBodies);
double SumOfBodyMassesExcludedFromBarycenter(BODY *body, int iNumBodies);
double TotalBarycentricMass(BODY *body, int iNumBodies);
// Addressing Barycentric Gravitational Parameter
double CalculateBarycentricGravitationalParameter(BODY *body, int iNumBodies, int iBody);
double CalculateCommonBarycentricFactor(BODY *body, int iNumBodies, int iBody);
double CalculateCommonBarycentricGravitationalParameter(BODY *body, int iNumBodies, int iBody);
// Coordinate Changes
void fvBaryOrbElems2BaryCart(BODY *body, int iNumBodies, int iBody);
void fvHelioOrbElems2HelioCart(BODY *body, int iNumBodies, int iBody);
void fvHelioCart2BaryCart(BODY *body, int iNumBodies, int iBody);
void fvAssignBaryCart(BODY *body, int iBody);
void fvAssignBaryCart2BaryCart(BODY *body, int iBody);
void fvAssignCartOutputs2BaryCart(BODY *body, int iBody);
void fvBaryCart2HelioOrbElems(BODY *body, int iNumBodies, int iBody);
void fvBaryCart2HelioCart(BODY *body, int iBody);
void fvBaryCart2BaryOrbElems(BODY *body, SYSTEM *system, int iNumBodies, int iBody);

void fvAssignLocalBaryCart(BODY *body, int iBody);
void fvCommonBarycenter2LocalBarycenter(BODY *body, int iNumBodies);
double CalculateEscapeVelocitySquared(double dGravitationalParameter, double dMagnitudeSquaredPosition);
void CalculateBaryPositionSquaredAndVelocitySquared(BODY *body, int iBody, int iNumBodies, 
                          double* dMagnitudeSquaredPosition, double* dMagnitudeSquaredVelocity);

void AssignAllCoords(BODY *body, CONTROL *control, FILES *files, SYSTEM *system, int iBody);
// These functions are defined in distorb.c, but needed in SpiNBody
// Relocate to system.c?

void cross(double *r, double *v, double *h); // h = r X v
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
double positiveModulus(double x, double y);
double positiveModulus2PI(double x);

// Halts
void CountHaltsSpiNBody(HALT *, int *);
void VerifyHaltSpiNBody(BODY *body, CONTROL *control, OPTIONS *options,
                        int iBody, int *iHalt);

void LogBodySpiNBody(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UPDATE *update, fnWriteOutput fnWrite[],
                     FILE *fp, int iBody);
void LogSpiNBody(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                 fnWriteOutput[], FILE *);

double fdDPositionXDt(BODY *body, SYSTEM *system, int *iaBody);
double fdDPositionYDt(BODY *body, SYSTEM *system, int *iaBody);
double fdDPositionZDt(BODY *body, SYSTEM *system, int *iaBody);
double fdDVelXDt(BODY *body, SYSTEM *system, int *iaBody);
double fdDVelYDt(BODY *body, SYSTEM *system, int *iaBody);
double fdDVelZDt(BODY *body, SYSTEM *system, int *iaBody);

/* @endcond */
