/**
   @file eqtide.h

   @brief Subroutines that control the integration of the tidal
          model. Also includes subroutines that switch between the two models.

   @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

   @date May 29 2014

*/

/* Tidal Model */

#define CPL 0
#define CTL 1
#define DB15 2

/* Darwin-Kaula tidal constituents resolved by the CPL model.

   CPL expands the tide-raising potential into a discrete set of harmonics,
   each with its own frequency sigma_i and phase lag epsilon_i. The code has
   always computed the sigma_i inside fiaCPLEpsilon(), but retained only
   fiSign(sigma_i); the indices below name the six constituents so that their
   frequencies, amplitudes and dissipated powers can be reported individually.

   The TIDE_ index is the constituent number used by the output routines; the
   epsilon index is the (sparse, historical) index into iTidalEpsilon.

   Doodson analogues below are for the Earth-Sun system, where Omega is the
   sidereal rotation rate and n the annual mean motion; see
   examples/SolarConstituents. They are NOT general: a synchronous rotator
   such as Enceladus has Omega = n, which collapses every constituent onto
   the orbital frequency. */

#define TIDE_SEMIDIURN 0 /* eps_0: 2*Omega - 2*n   Earth-Sun: S2 (12.0000 h) */
#define TIDE_ECCPLUS 1   /* eps_1: 2*Omega - 3*n   Earth-Sun: T2 (12.0164 h) */
#define TIDE_ECCMINUS 2  /* eps_2: 2*Omega -   n   Earth-Sun: R2 (11.9836 h) */
/* Sa is a loose match: this line sits at the Keplerian mean motion (the
   sidereal year), whereas Doodson's Sa is defined on the tropical year. */
#define TIDE_RADIAL 3    /* eps_5:              n  Earth-Sun: Sa (365.26 d)  */
#define TIDE_OBLDIURN 4  /* eps_8:   Omega  - 2*n  Earth-Sun: P1 (24.0659 h) */
#define TIDE_OBLSID 5    /* eps_9:   Omega         Earth-Sun: K1 (23.9345 h) */

#define NUM_TIDE_CONSTITUENTS 6

/* Options Info */

#define OPTSTARTEQTIDE 1000 /* Start of Eqtide options */
#define OPTENDEQTIDE 1100   /* End of Eqtide options */

#define OPT_USETIDALRADIUS 1001
#define OPT_TIDALRADIUS 1002
#define OPT_DISCRETEROT 1005
#define OPT_FIXORBIT 1007
#define OPT_FORCEEQSPIN 1010
#define OPT_HALTDBLSYNC 1015
#define OPT_HALTTIDELOCK 1020
#define OPT_HALTSYNCROT 1025
#define OPT_H2 1026 /* Displacement Love number h_2 */
#define OPT_K2 1027 /* Change to LOVEK2 */
#define OPT_K2OCEAN 1028
#define OPT_K2ENV 1029
#define OPT_MAXLOCKDIFF 1030
#define OPT_SYNCECC 1035
#define OPT_TIDEMODEL 1040
#define OPT_TIDEPERTS 1042
#define OPT_TIDALTAU 1045
#define OPT_TIDALQOCEAN 1051
#define OPT_OCEANTIDES 1052
#define OPT_TIDALQENV 1053
#define OPT_ENVTIDES 1054
#define OUT_ECCA 1055
#define OPT_MANTLETIDES 1057
#define OPT_TIDALQMANTLE 1058
#define OPT_K2MANTLE 1059

/* Halt Functions */
#define EQTIDEHALTSYSEND 5
#define EQTIDEHALTBODYEND 5

/* EQTIDE 1000 - 1999 */
/* System properties 1000-1039, body properties 1040-1099 */
#define OUTSTARTEQTIDE 1000
#define OUT_TIDALRADIUS 1005
/* The body sub-range was widened from 1050 to 1040 to make room for the
   per-constituent ocean diagnostics; the three obliquity-derivative system
   outputs that used to sit at 1045-1047 moved down to 1032-1034 to stay
   below the boundary. These IDs are internal array indices only -- input
   files and log files refer to outputs by name -- so the renumbering is not
   user-visible. */
#define OUTBODYSTARTEQTIDE 1040
#define OUTENDEQTIDE 1100

#define OUT_DSEMIDTEQTIDE 1010
#define OUT_DECCDTEQTIDE 1012
#define OUT_DMMDTEQTIDE 1015
#define OUT_DORBPERDTEQTIDE 1017
#define OUT_DORBEPSILON 1020
#define OUT_ECCTIMEEQTIDE 1023
#define OUT_SEMITIMEEQTIDE 1025
#define OUT_DHECCDTEQTIDE 1028
#define OUT_DKECCDTEQTIDE 1030

#define OUT_DXOBLDTEQTIDE 1032
#define OUT_DYOBLDTEQTIDE 1033
#define OUT_DZOBLDTEQTIDE 1034

/* Body Properties due to tidal evolution */

#define OUT_LOCKTIME 1051
#define OUT_BODYDSEMIDTEQTIDE 1052
#define OUT_BODYDECCDTEQTIDE 1053

#define OUT_DOBLDTEQTIDE 1054
#define OUT_DROTPERDTEQTIDE 1058
#define OUT_DROTRATEDTEQTIDE 1060
#define OUT_EQROTRATEEQTIDE 1062
#define OUT_EQROTRATEDISCRETE 1064
#define OUT_EQROTPERDISCRETE 1066
#define OUT_EQROTRATECONT 1068
#define OUT_EQROTPERCONT 1070
#define OUT_EQROTPER 1072
#define OUT_EQROTRATE 1074
/* Was also 1074, which silently shadowed OUT_EQROTRATE: both IDs index
   output[], so whichever was registered second in InitializeOutputEqtide()
   overwrote the first and the EqRotRate column emitted EqTidePower. */
#define OUT_EQTIDEPOWER 1056

#define OUT_GAMMAROT 1078
#define OUT_GAMMAORB 1080

#define OUT_K2OCEAN 1083

//#define OUT_TIDALQEQTIDE        1084
#define OUT_TIDALQOCEAN 1085
#define OUT_TIDALQENV 1084
#define OUT_TIDALTAU 1086
#define OUT_TIDELOCK 1088
#define OUT_ROTRATETIMEEQTIDE 1090
#define OUT_K2ENV 1091
#define OUT_OBLTIMEEQTIDE 1092
#define OUT_POWEREQTIDE 1094
#define OUT_ENFLUXEQTIDE 1096
//#define OUT_TIDALQ              1097
#define OUT_BENV 1098
#define OUT_BOCEAN 1099

/* Ratio of the ocean tide to the equilibrium tide of the raising potential,
   gamma_2 = 1 + k_2 - h_2. Rheology-independent, so unlike the constituent
   diagnostics below it is registered for every tidal model. */
#define OUT_TIDALDIMINISH 1040

/* Per-constituent tidal diagnostics (CPL only). Ordered to match the
   TIDE_* indices above. The IDs are scattered because they occupy the gaps
   left in the body range. */

#define OUT_TIDALFREQSEMIDIURN 1061
#define OUT_TIDALFREQECCPLUS 1063
#define OUT_TIDALFREQECCMINUS 1065
#define OUT_TIDALFREQRADIAL 1067
#define OUT_TIDALFREQOBLDIURN 1069
#define OUT_TIDALFREQOBLSID 1071

#define OUT_TIDALAMPSEMIDIURN 1073
#define OUT_TIDALAMPECCPLUS 1075
#define OUT_TIDALAMPECCMINUS 1076
#define OUT_TIDALAMPRADIAL 1077
#define OUT_TIDALAMPOBLDIURN 1079
#define OUT_TIDALAMPOBLSID 1081

#define OUT_TIDALPOWERSEMIDIURN 1082
#define OUT_TIDALPOWERECCPLUS 1087
#define OUT_TIDALPOWERECCMINUS 1089
#define OUT_TIDALPOWERRADIAL 1093
#define OUT_TIDALPOWEROBLDIURN 1095
#define OUT_TIDALPOWEROBLSID 1097

/* Ocean tide amplitude of each constituent: the equilibrium tide relative to
   the deforming solid surface, i.e. TidalAmp scaled by gamma_2. This is the
   quantity a tide gauge measures, and the one to hand an ocean model. */

#define OUT_TIDALOCEANAMPSEMIDIURN 1041
#define OUT_TIDALOCEANAMPECCPLUS 1042
#define OUT_TIDALOCEANAMPECCMINUS 1043
#define OUT_TIDALOCEANAMPRADIAL 1044
#define OUT_TIDALOCEANAMPOBLDIURN 1045
#define OUT_TIDALOCEANAMPOBLSID 1046

/* @cond DOXYGEN_OVERRIDE */

void InitializeControlEqtide(CONTROL *, int);
void AddModuleEqtide(CONTROL *, MODULE *, int, int);
void BodyCopyEqtide(BODY *, BODY *, int, int, int);
void InitializeBodyEqtide(BODY *, CONTROL *, UPDATE *, int, int);
void InitializeUpdateTmpBodyEqtide(BODY *, CONTROL *, UPDATE *, int);
int fiGetModuleIntEqtide(MODULE *, int);

void InitializeXoblEqtide(BODY *, UPDATE *, int, int);
void InitializeYoblEqtide(BODY *, UPDATE *, int, int);
void InitializeZoblEqtide(BODY *, UPDATE *, int, int);
void InitializeRotEqtide(BODY *, UPDATE *, int, int);
void InitializeHeccEqtide(BODY *, UPDATE *, int);
void InitializeKeccEqtide(BODY *, UPDATE *, int);
void InitializeSemiEqtide(BODY *, UPDATE *, int);

/* Options Functions */
void HelpOptionsEqtide(OPTIONS *);
void ReadDiscreteRot(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadFixOrbit(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadForceEqSpin(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadK2(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadK2Ocean(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadK2Env(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadMaxLockDiff(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadSyncEcc(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadTideModel(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadTidalQ(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadTidalQOcean(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadTidalQEnv(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadTidalRadius(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadTidalTau(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void InitializeOptionsEqtide(OPTIONS *, fnReadOption[]);
void ReadOptionsEqtide(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                       fnReadOption[], int);
void ReadEqtideOceanTides(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadEqtideEnvTides(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadUseTidalRadius(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadHaltTideLock(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadHaltSyncRot(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadHaltDblSync(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);

int HaltDblSync(BODY *, EVOLVE *, HALT *, IO *, UPDATE *, fnUpdateVariable ***,
                int);
int HaltTideLock(BODY *, EVOLVE *, HALT *, IO *, UPDATE *, fnUpdateVariable ***,
                 int);
int HaltSyncRot(BODY *, EVOLVE *, HALT *, IO *, UPDATE *, fnUpdateVariable ***,
                int);


void CountHaltsEqtide(HALT *, int *);
void VerifyHaltEqtide(BODY *, CONTROL *, OPTIONS *, int, int *);
void InitializeHaltEqtide(HALT *, MODULE *, int, int *, int *);

/* Verify Functions */

void VerifyRotationEqtideWarning(char[], char[], char[], int, int, int);
void VerifyRotationEqtide(BODY *, CONTROL *, UPDATE *, OPTIONS *, char[], int);
void VerifyEqtide(BODY *, CONTROL *, FILES *, OPTIONS *, OUTPUT *, SYSTEM *,
                  UPDATE *, int, int);
void InitializeModuleEqtide(CONTROL *, MODULE *);
void VerifyDB15(BODY *, CONTROL *, FILES *, OPTIONS *, OUTPUT *, UPDATE *, int,
                int);

/* Update Functions */

void InitializeUpdateEqtide(BODY *, UPDATE *, int);
void FinalizeUpdateEccEqtide(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateIsotopeEqtide(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateOblEqtide(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateRotEqtide(BODY *, UPDATE *, int *, int, int, int);
void FinalizeUpdateSemiEqtide(BODY *, UPDATE *, int *, int, int, int);

/* Output Functinos */

void HelpOutputEqtide(OUTPUT *);
void WriteBodyDsemiDtEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                            UPDATE *, int, double *, char**);
void WriteBodyDeccDtEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char**);
void WriteDOblDtEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char**);
void WriteDSemiDtEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char**);
void WriteDEccDtEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char**);
void WriteDMeanMotionDtEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                              UPDATE *, int, double *, char**);
void WriteDOrbPerDtEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char**);
void WriteDRotPerDtEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char**);
void WriteDRotRateDtEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char**);
void WriteEccTimescaleEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                             UPDATE *, int, double *, char**);
void WriteEqRotPer(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char**);
void WriteEqRotPerCont(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char**);
void WriteEqRotPerDiscrete(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char**);
void WriteEqRotRate(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                    int, double *, char**);
void WriteEqRotRateCont(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char**);
void WriteEqRotRateDiscrete(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                            UPDATE *, int, double *, char**);
void WriteGammaOrb(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char**);
void WriteGammaRot(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char**);
void WriteOblTimescaleEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                             UPDATE *, int, double *, char**);
void WriteRotTimescaleEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                             UPDATE *, int, double *, char**);
void WriteSemiTimescaleEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                              UPDATE *, int, double *, char**);
void WritePowerEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                      int, double *, char**);
void WriteSurfEnFluxEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char**);
void WriteEqPower(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
                  double *, char**);
void WriteEqSurfEnFlux(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char**);
void WriteTidalQ(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
                 double *, char**);
void WriteTidalTau(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char**);
void WriteTideLock(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char**);
void WriteLockTime(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                   int, double *, char**);
// void
// WriteImK2(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteK2Ocean(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
                  double *, char**);
void WriteK2Env(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
                double *, char**);
void WriteTidalQOcean(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                      int, double *, char**);
void WriteTidalQEnv(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                    int, double *, char**);
void InitializeOutputEqtide(OUTPUT *, fnWriteOutput[]);
void InitializeOutputEqtideConstituents(OUTPUT *, fnWriteOutput[]);
void WriteTidalRadius(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                      int, double *, char**);

/* Per-constituent diagnostics */
int fbTidalConstituentNA(CONTROL *, OUTPUT *, double *, char **);
void fvWriteTidalFreq(BODY *, CONTROL *, OUTPUT *, UNITS *, int, int, double *,
                      char **);
void fvWriteTidalAmp(BODY *, CONTROL *, OUTPUT *, UNITS *, int, int, double *,
                     char **);
void fvWriteTidalPower(BODY *, CONTROL *, OUTPUT *, UNITS *, int, int, double *,
                       char **);
void WriteTidalFreqSemiDiurn(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                             UPDATE *, int, double *, char**);
void WriteTidalFreqEccPlus(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char**);
void WriteTidalFreqEccMinus(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                            UPDATE *, int, double *, char**);
void WriteTidalFreqRadial(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char**);
void WriteTidalFreqOblDiurn(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                            UPDATE *, int, double *, char**);
void WriteTidalFreqOblSid(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char**);

void WriteTidalAmpSemiDiurn(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                            UPDATE *, int, double *, char**);
void WriteTidalAmpEccPlus(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char**);
void WriteTidalAmpEccMinus(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char**);
void WriteTidalAmpRadial(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char**);
void WriteTidalAmpOblDiurn(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char**);
void WriteTidalAmpOblSid(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char**);

void WriteTidalPowerSemiDiurn(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                              UPDATE *, int, double *, char**);
void WriteTidalPowerEccPlus(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                            UPDATE *, int, double *, char**);
void WriteTidalPowerEccMinus(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                             UPDATE *, int, double *, char**);
void WriteTidalPowerRadial(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char**);
void WriteTidalPowerOblDiurn(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                             UPDATE *, int, double *, char**);
void WriteTidalPowerOblSid(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char**);

void fvWriteTidalOceanAmp(BODY *, CONTROL *, OUTPUT *, UNITS *, int, int,
                          double *, char **);
void WriteTidalOceanAmpSemiDiurn(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                                 UPDATE *, int, double *, char **);
void WriteTidalOceanAmpEccPlus(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                               UPDATE *, int, double *, char **);
void WriteTidalOceanAmpEccMinus(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                                UPDATE *, int, double *, char **);
void WriteTidalOceanAmpRadial(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                              UPDATE *, int, double *, char **);
void WriteTidalOceanAmpOblDiurn(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                                UPDATE *, int, double *, char **);
void WriteTidalOceanAmpOblSid(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                              UPDATE *, int, double *, char **);
void WriteTidalDiminish(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char **);

/* Logging Functions */
void LogOptionsEqtide(CONTROL *, FILE *);
void LogEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *, fnWriteOutput[],
               FILE *);
void LogBodyEqtide(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                   fnWriteOutput[], FILE *, int);

/* Functions common to both CPL and CTL */

void PropertiesEqtide(BODY *, int);
void fdaChi(BODY *, double, double, int, int);
double fdEqRotRate(BODY *, int, double, double, int, int);
double fdTidePower(BODY *, int, int);
double fdSurfEnFluxEqtide(BODY *, SYSTEM *, UPDATE *, int, int);
void ForceBehaviorEqtide(BODY *, MODULE *, EVOLVE *, IO *, SYSTEM *, UPDATE *,
                         fnUpdateVariable ***, int, int);

void PropsAuxEqtide(BODY *, EVOLVE *, IO *, UPDATE *, int);

int fiAssignTidalPerturber(BODY*,int);
int fiAssignTidalOrbiter(BODY*,int);

/******************** CPL Functions ********************/

/* Auxiliary parameters */
void fiaCPLEpsilon(double, double, double, int *);
void fdCPLZ(BODY *, double, double, int, int);
double fdGammaRot(double, double, int *);
double fdGammaOrb(double, double, int *);
double fdCPLTidePower(BODY *, int);
void PropsAuxCPL(BODY *, EVOLVE *, IO *, UPDATE *, int);

/* Darwin-Kaula constituent decomposition */
double fdCPLTidalFreq(BODY *, int, int, int);
double fdCPLTidalAmpCoeff(BODY *, int, int, int);
double fdCPLTidalAmp(BODY *, int, int, int, int);
double fdCPLTidalAmpOcean(BODY *, int, int, int, int);
double fdCPLTidalPowerConst(BODY *, int, int);
int fiTidalEpsilonIndex(int);

/* Love numbers */
double fdLoveH2(BODY *, int);
double fdTidalDiminish(BODY *, int);

/* Equilibrium parameters */
double fdCPLEqSpinRate(double, double, int);
double fdCPLTidePowerEq(double, double, double, double, int);
double fdCPLEqRotRate(double, double, int);
double fdCPLEqRotRateDiscrete(double, double);
double fdCPLEqRotRateCont(double, double);

/* Derivatives */
double fdCPLDsemiDt(BODY *, SYSTEM *, int *);
double fdCPLDHeccDt(BODY *, SYSTEM *, int *);
double fdCPLDKeccDt(BODY *, SYSTEM *, int *);
double fdCPLDrotrateDt(BODY *, SYSTEM *, int *);
double fdCPLDoblDt(BODY *, int *);
double fdCPLDXoblDt(BODY *, SYSTEM *, int *);
double fdCPLDYoblDt(BODY *, SYSTEM *, int *);
double fdCPLDZoblDt(BODY *, SYSTEM *, int *);
double fdCPLDsemiDtBody(BODY, double, double, double);
double fdCPLDeccDt(BODY *, int *);
double fdDEdTCPLEqtide(BODY *, SYSTEM *, int *);
double fdCPLDeccDtLocked(BODY *, UPDATE *, int *);

/******************** CTL Functions ********************/

/* Auxiliary parameters */
void fdaCTLF(BODY *, double, int, int);
void fdaCTLZ(BODY *, double, int, int);
double fdCTLF1(double);
double fdCTLF2(double);
double fdCTLF3(double);
double fdCTLF4(double);
double fdCTLBeta(double);
double fdCTLF5(double);
double fdCTLTidePower(BODY *, int);
void PropsAuxCTL(BODY *, EVOLVE *, IO *, UPDATE *, int);
double fdSurfEnFluxOcean(BODY *, int);
double fdTidePowerOcean(BODY *, int);

/* Equilibrium parameters */
double fdCTLTidePowerEq(BODY, double);
double fdCTLEqRotRate(double, double, double);

double fdCTLDsemiDt(BODY *, SYSTEM *, int *);
double fdCTLDeccDt(BODY *, UPDATE *, int *);
double fdCTLDrotrateDt(BODY *, SYSTEM *, int *);
double fdCTLDXoblDt(BODY *, SYSTEM *, int *);
double fdCTLDYoblDt(BODY *, SYSTEM *, int *);
double fdCTLDZoblDt(BODY *, SYSTEM *, int *);
double fdCTLDsemiDtBody(BODY, double, double, double, double, double);
double fdCTLDeccDtBody(BODY, double, double, double);
double fdCTLDHeccDt(BODY *, SYSTEM *, int *);
double fdCTLDKeccDt(BODY *, SYSTEM *, int *);
double fdDEdTCTLEqtide(BODY *, SYSTEM *, int *);
double fdCTLDoblDt(BODY *, int *);

/******************** DB15 Functions ********************/
double fdPowerEqtideDB15(BODY *, int);
void PropsAuxDB15(BODY *, EVOLVE *, IO *, UPDATE *, int);


double fdDB15DsemiDt(BODY *, SYSTEM *, int *);
double fdDB15DeccDt(BODY *, UPDATE *, int *);
double fdDB15DHeccDt(BODY *, SYSTEM *, int *);
double fdDB15DKeccDt(BODY *, SYSTEM *, int *);


/* @endcond */
