/***************** THERMINT.H *********************** 
 *
 * Peter Driscoll 6/1/15
 *
 * This header file contains all the subroutines in
 * file thermint.c.
 *
*/

// GLOBAL VARIABLE INDEXES
#define TMAN             20                    //index of TMAN variable.
#define TCORE            21                    //index of TCORE variable.

// !!!HACK!!!  Assume a constant surf temp for now.
#define TSURF  300.0  
// UNITS CONSTANTS
#define KM               1e3       //[m] 1 km in m

// FUNDAMENTAL CONSTANTS
#define GASCONSTANT      8.3144621             //[J/mol/K] gas constant in SI units.      

// E MASS RESERVOIRS
#define EMASS            5.9742e24             //[kg] mass of E
#define EMASSFRACMAN     0.67                  //mass fraction of E mantle
#define EMASSMAN         EMASSFRACMAN*EMASS    //[kg] mass of E mantle
#define EMASSFRACCORE    0.32                  //mass frac E core
#define EMASSCORE        EMASSFRACCORE*EMASS   //[kg] mass of E core
#define EMASSIC          9.7e22                //[kg] mass of present IC.
#define EMASSOC          EMASSCORE-EMASSIC     //[kg] mass of present OC.
#define EMASSOCEAN       1.4e21                //[kg] mass of E oceans
#define EMASSFRACOCEAN   EMASSOCEAN/EMASS      //ocean mass frac
#define EMASSFRACCRUST   1-EMASSFRACMAN-EMASSFRACCORE-EMASSFRACOCEAN  //crust is remaining bit of mass.
#define EMASSCRUST       EMASSFRACCRUST*EMASS  //[kg] mass E crust today.

// INTERNAL STRUCTURE E
#define ERADIUS          6371.0*KM          //[m] radius of E.
#define ERMAN            6371.0*KM          //[m] radius of E mantle.
#define ERCORE           3481.0*KM          //[m] radius Core
#define ERICB            1221.0*KM          //[m] present day ICB radius.
#define EDMAN            ERADIUS-ERCORE     //[m] mantle shell thickness
#define EDOC             ERCORE-ERICB       //[m] present day core shell thickness
#define EVOL             (4.0)/(3)*PI*ERADIUS*ERADIUS*ERADIUS //[m^3] volume of E.
#define EVOLIC           (4.0)/(3)*PI*ERICB*ERICB*ERICB   //[m^3] volume of present-day IC.
#define EVOLCORE         (4.0)/(3)*PI*ERCORE*ERCORE*ERCORE  //[m^3] volume of total core.
#define EVOLOC           EVOLCORE-EVOLIC    //[m^3] volume of OC today.
#define EVOLMAN          EVOL-EVOLCORE      //[m^3] volume of Mantle.
#define AREASURF         4.0*PI*ERADIUS*ERADIUS //[m2] area of surface
#define AREACMB          4.0*PI*ERCORE*ERCORE   //[m2] area of CMB.
// DENSITIES - derived from mass and radius dfns.
#define EDENS            (EMASS)/(EVOL)         //[kg/m^3] density of E
#define EDENSMAN         (EMASSMAN)/(EVOLMAN)   //[kg/m^3] density of E mantle.
#define EDENSCORE        (EMASSCORE)/(EVOLCORE) //[kg/m^3] density of E core.
#define EDENSOC          (EMASSOC)/(EVOLOC)     //[kg/m^3] density of E core.
#define EDENSIC          (EMASSIC)/(EVOLIC)     //[kg/m^3] density of E IC.
#define DENSOCLE         700.0              //[kg/m^3] density diff of OC light elements.
// Gravity
#define GRAVSURF         9.8                //[m/s2] Surf gravity in SI
#define GRAVUMAN         10.5               //[m/s2] UM gravity in SI
#define GRAVLMAN         10.5               //[m/s2] LM gravity in SI

/* THERMAL PROPERTIES */
#define SPECHEATMAN      1265.0             //[J/kg/K] specfic heat mantle.
#define SPECHEATCORE     840.0              //[J/kg/K] sp heat core.
#define THERMEXPANMAN    3e-5               //[K^-1] thermal expansivity of mantle.
#define THERMEXPANCORE   1e-5               //[K^-1] thermal expansivity of core.
#define THERMCONDUMAN    4.2                //[W/m/K] thermal conductivity of Upper mantle.
#define THERMCONDLMAN    10.0               //[W/m/K] thermal conductivity of Lower mantle.
#define THERMDIFFUMAN    THERMCONDUMAN/(EDENSMAN*SPECHEATMAN) //[m^2/s] thermal diffusivity mantle.
#define THERMDIFFLMAN    THERMDIFFUMAN
#define SPECLATENTICB    750e3              //[J/kg] specific latent heat release at ICB
#define SPECLATENTMAN    320e3              //[J/kg] specific latent heat release by mantle solidification.
#define SPECPOWGRAVIC    3e5           //[J/kg]  Gravitational power release during IC growth.
/* CONVECTION CONSTANTS */
#define RACRIT           660.          //[-] critical rayleigh number for spherical convection.
#define CONVEXPON        1./3          //[nd] convective cooling nusselt exponent "beta"
/* VISCOSITY PROPERTIES */
#define ACTVISCMAN       3e5           //[J/mol] viscosity activation energy mantle
#define ACTSHMODMAN      2e5           //[J/mol] shear modulus activation energy mantle
#define VISCREF          7e7 //6e7           //[m2/s] reference kinematic mantle viscosity
#define VISCJUMPULM      5.6  //2.0           //[nd] viscosity jump from upper to lower mantle
#define SHMODREF         6.24e4        //[Pa] reference kinematic mantle shear modulus
#define MELTB            2.5           //[nd] viscosity-melt reduction coefficient "B" (DB15 eq 8)
#define MELTPHISTAR      0.8           //[nd] viscosity-melt reduction coefficient "phi*" (DB15 eq 8)
#define MELTDELTA        6.0           //[nd] viscosity-melt reduction coefficient "delta" (DB15 eq 8)
#define MELTGAMMA        6.0           //[nd] viscosity-melt reduction coefficient "gamma" (DB15 eq 9)
#define MELTXI           5e-4          //[nd] viscosity-melt reduction coefficient "Xi" (DB15 eq 9)
/* TIDAL PROPERTIES */
#define STIFFNESS        3.1217e11     //1.71e4*1e9    //[Pa] effective stiffness of mantle (calibrated to k2=0.3, Q=100)
/* MELTING CONSTANTS */
#define ASOLIDUS         -1.160e-16    //[K/m3] mantle solidus coefficient Tsol(r)=A*r^3+B*r^2+C*r+D
#define BSOLIDUS         +1.708e-9     //[K/m2] B coefficient
#define CSOLIDUS         -9.074e-3     //[K/m] C coefficient
#define DSOLIDUS         +1.993e4      //[K] D coefficient
#define DTLIQMAN         +500.0        //[K] mantle liquidus offset, T_liq=T_sol+DTLIQMAN
#define DVLIQDTM         +8e17         //[m3/K] change in mantle liquid volume with T_m approximated (Driscoll&B 2015)
#define DLIND            7000.0*KM     //[m] lindemann's law length scale for iron liquidus "D_Fe" (DB15 A23)
#define TREFLIND         5600.0        //[K] lindemann's law reference temp. "T_Fe0" (DB15 A23)
#define DVLIQDTEMP       8e17          //[m^3/K] approximation of mantle DV_liq/DT.
/* ADIABATIC PROPERTIES */
#define ADGRADMAN        (0.5)/(KM)    //[K/m] mantle linear adiabatic gradient =0.5K/km  (DB15 eq A18)
#define DADCORE          6340.0*KM     //[m] liq iron core adiabatic length scale (DB15 eq A22)
#define GRUNEISEN        1.3           //[nd] core gruneisen parameter
#define ADJUMPM2UM       0.7           //[nd] adiabatic temp jump from ave mantle to UM. "epsilon_UM"
#define ADJUMPM2LM       1.3           //[nd] adiabatic temp jump from ave mantle to LM. "epsilon_LM"
#define ADJUMPC2CMB      0.8           //[nd] adiabatic temp jump from ave core to CMB. "epsilon_c"
/* MAGNETIC DYNAMO PROPERTIES */
#define MAGPERM          4*PI*1e-7                //[H/m] magnetic permeability constant in SI units.
#define GAMMADIPOLE      0.2                      //[nd] saturation constant for fast rot dipolar dynamos (OC2006)
#define ELECCONDCORE     10e5                     //[S/m]  electrical conductivity of core.  How does this convert to cgs??
#define LORENTZNUM       2.5e-8                   //[W Ohm/K] lorentz number, relates thermal and electrical conductivity.        

void InitializeControlThermint(CONTROL*);
void AddModuleThermint(MODULE*,int,int);
void BodyCopyThermint(BODY*,BODY*,int,int);
void InitializeBodyEqtide(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyThermint(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */

#define OPTSTARTTHERMINT         1200 /* Start of THERMINT options */
#define OPTENDTHERMINT           1400 /* End of THERMINT options */

/* Scalar Properties */
#define OPT_TMAN            1210   //Temperature Mantle AVE
#define OPT_TUMAN           1211   //Temperature UMTBL
#define OPT_TLMAN           1212   //Temperature LMTBL
#define OPT_TCORE           1213   //Temperature Core AVE
#define OPT_TCMB            1214   //Temperature CMB
#define OPT_TICB            1215   //Temperature ICB
#define OPT_BLUMAN          1216   //UM TBL thickness
#define OPT_BLLMAN          1217   //LM TBL thickness
#define OPT_TJUMPUMAN       1218   //Temperature Jump across UMTBL
#define OPT_TJUMPLMAN       1219   //Temperature Jump across LMTBL
#define OPT_VISCUMAN        1220   //Viscosity UMTBL
#define OPT_VISCLMAN        1221   //Viscosity LMTBL
#define OPT_VISCRATIOMAN    1222   //Viscosity ratio UM 2 LM
#define OPT_SHMODUMAN       1223   //Shear modulus UMTBL
#define OPT_SHMODLMAN       1224   //Shear modulus LMTBL
#define OPT_FMELTUMAN       1225   //Melt fraction UMTBL
#define OPT_FMELTLMAN       1226   //Melt fraction LMTBL
#define OPT_MELTFACTORUMAN  1227   //Melt fraction UMTBL
#define OPT_DEPTHMELTMAN    1228   //Depth to base of UM melt region.
#define OPT_TDEPTHMELTMAN   1229   //Temp at base of UM melt region.
#define OPT_TJUMPMELTMAN    1230   //Temp jump across UM melt region.
#define OPT_K2MAN           1231   //Mantle k2 love number
#define OPT_IMK2MAN         1232   //Mantle Im(k2) love number
#define OPT_ERUPTEFF        1233   //Mantle Melt Eruption Efficiency
/* Time Derivatives & Gradients */
#define OPT_TDOTMAN         1240   //Time deriv of mean mantle temp
#define OPT_TDOTCORE        1241   //time deriv of mean core temp
#define OPT_HFLUXUMAN       1242   //hflux upper mantle thermal boundary layer (UMTBL)
#define OPT_HFLOWUMAN       1243   //hflow UMTBL
#define OPT_HFLUXLMAN       1244   //hflux upper mantle thermal boundary layer (UMTBL)
#define OPT_HFLOWLMAN       1245   //hflow UMTBL
#define OPT_HFLUXCMB        1246   //hflux lower mantle TBL = CMB
#define OPT_HFLOWCMB        1247   //hflow LMTBL=CMB
#define OPT_HFLOWTIDALMAN   1248   //hflow tidal dissipation in mantle
#define OPT_HFLOWTIDALCORE  1249   //hflow tidal dissipation in core
#define OPT_HFLOWLATENTMAN  1250   //latent hflow from solidification of mantle
#define OPT_HFLOWMELTMAN    1251   //Melt hflow from mantle
#define OPT_HFLOWLATENTIC   1252   //latent hflow from solidification of IC
#define OPT_POWERGRAVIC     1253   //Gravitation Power release from solidification of IC
#define OPT_HFLOWICB        1254   //hflow across ICB
#define OPT_HFLUXSURF       1260   //hflux surface of mantle
#define OPT_HFLOWSURF       1261   //hflow surface of mantle
#define OPT_TIDALPOWMAN     1262   //Tidal Power Mantle
/* Core Variables */
#define OPT_RIC             1270   //IC radius
#define OPT_DOC             1271   //OC shell thickness
#define OPT_CHIOC           1280   //OC light element concentration chi.
#define OPT_CHIIC           1281   //IC light element concentration chi.
#define OPT_THERMCONDUCTOC  1282   //Thermal conductivity OC
#define OPT_THERMCONDUCTIC  1283   //Thermal conductivity IC

#define OPT_HALTMINTMAN     1290
#define OPT_HALTMINTCORE    1291

/* Options Functions */
void HelpOptionsThermint(OPTIONS*);
void ReadTMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadTCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadViscRatioMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadEruptEff(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;

void ReadHaltMinTMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMinTCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);

void InitializeOptionsThermint(OPTIONS*,fnReadOption[]);
void ReadOptionsThermint(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Halt Functions */
#define THERMINTHALTSYSEND       6
#define THERMINTHALTBODYEND      6

int fbHaltMinTMan(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);  //syntax now is same as fnHaltModule in vplanet.h
int fbHaltMinTCore(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
void CountHaltsThermint(HALT*,int*);

/* Verify Functions */

void VerifyThermint(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void VerifyRotationThermint(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetThermint(CONTROL*,MODULE*);
void PropertiesThermint(BODY*,int);
void VerifyHaltThermint(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationThermint(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateThermint(BODY*,UPDATE*,int);

/* Auxiliary Properties */
void fnPropertiesThermint(BODY*,UPDATE*,int);

/* Output Functinos */

/* THERMINT */
#define OUTSTARTTHERMINT         1200
#define OUTENDTHERMINT           1400

/* Output Body Properties */
/* Scalar Properties */
#define OUT_TMAN            1210   //Temperature Mantle AVE
#define OUT_TUMAN           1211   //Temperature UMTBL
#define OUT_TLMAN           1212   //Temperature LMTBL
#define OUT_TCORE           1213   //Temperature Core AVE
#define OUT_TCMB            1214   //Temperature CMB
#define OUT_TICB            1215   //Temperature ICB
#define OUT_BLUMAN          1216   //UM TBL thickness
#define OUT_BLLMAN          1217   //LM TBL thickness
#define OUT_TJUMPUMAN       1218   //Temperature Jump across UMTBL
#define OUT_TJUMPLMAN       1219   //Temperature Jump across LMTBL
#define OUT_SIGNTJUMPUMAN   1220   //Temperature Jump across UMTBL
#define OUT_SIGNTJUMPLMAN   1221   //Temperature Jump across LMTBL
#define OUT_VISCUMAN        1222   //Viscosity UMTBL
#define OUT_VISCLMAN        1223   //Viscosity LMTBL
#define OUT_VISCRATIOMAN    1224   //Viscosity ratio UM 2 LM
#define OUT_SHMODUMAN       1225   //Shear modulus UMTBL
#define OUT_SHMODLMAN       1226   //Shear modulus LMTBL
#define OUT_FMELTUMAN       1227   //Melt fraction UMTBL
#define OUT_FMELTLMAN       1228   //Melt fraction LMTBL
#define OUT_MELTFACTORUMAN  1229   //Melt factor UMTBL
#define OUT_DEPTHMELTMAN    1230   //Depth to base of UM melt region.
#define OUT_TDEPTHMELTMAN   1231   //Temp at base of UM melt region.
#define OUT_TJUMPMELTMAN    1232   //Temp jump across UM melt region.
#define OUT_K2MAN           1233   //Mantle k2 love number
#define OUT_IMK2MAN         1234   //Mantle Im(k2) love number
#define OUT_ERUPTEFF        1235   //Mantle Melt Eruption Efficiency
/* Time Derivatives & Gradients */
#define OUT_TDOTMAN         1240   //Time deriv of mean mantle temp
#define OUT_TDOTCORE        1241   //time deriv of mean core temp
#define OUT_HFLUXUMAN       1242   //hflux upper mantle thermal boundary layer (UMTBL)
#define OUT_HFLOWUMAN       1243   //hflow UMTBL
#define OUT_HFLUXLMAN       1244   //hflux upper mantle thermal boundary layer (UMTBL)
#define OUT_HFLOWLMAN       1245   //hflow UMTBL
#define OUT_HFLUXCMB        1246   //hflux lower mantle TBL = CMB
#define OUT_HFLOWCMB        1247   //hflow LMTBL=CMB
#define OUT_HFLOWTIDALMAN   1248   //hflow tidal dissipation in mantle
#define OUT_HFLOWTIDALCORE  1249   //hflow tidal dissipation in core
#define OUT_HFLOWLATENTMAN  1250   //latent hflow from solidification of mantl
#define OUT_HFLOWMELTMAN    1251   //Melt hflow from mantle
#define OUT_HFLOWLATENTIC   1252   //latent hflow from solidification of IC
#define OUT_POWERGRAVIC     1253   //Gravitation Power release from solidification of IC
#define OUT_HFLOWICB        1254   //hflow across ICB
#define OUT_HFLUXSURF       1260   //hflux surface of mantle
#define OUT_HFLOWSURF       1261   //hflow surface of mantle
#define OUT_TIDALPOWMAN     1262   //Tidal Power Mantle
/* Core Variables */
#define OUT_RIC             1270   //IC radius
#define OUT_DOC             1271   //OC shell thickness
#define OUT_DRICDTCMB       1272   //d(R_ic)/d(T_cmb)
#define OUT_CHIOC           1280   //OC light element concentration chi.
#define OUT_CHIIC           1281   //IC light element concentration chi.
#define OUT_THERMCONDUCTOC  1282   //Thermal conductivity OC
#define OUT_THERMCONDUCTIC  1283   //Thermal conductivity IC


void HelpOutputThermint(OUTPUT*);
void InitializeOutputThermint(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionThermint(OUTPUT*,int,int);
void FinalizeOutputFunctionThermint(OUTPUT*,int,int);

void WriteTMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTJumpUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTJumpLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteSignTJumpUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteSignTJumpLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTCMB(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTICB(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteViscUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteViscLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteShmodUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteShmodLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFMeltUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteFMeltLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteMeltfactorUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteDepthMeltMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTDepthMeltMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTJumpMeltMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTDotMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTDotCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHfluxUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowUMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHfluxLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowLMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowTidalMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowTidalCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowLatentMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowMeltMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowLatentIC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WritePowerGravIC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowICB(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteHflowSurf(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteTidalPowMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRIC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteDRICDTCMB(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteChiOC(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsThermint(CONTROL*,FILE*);
void LogThermint(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyThermint(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Thermal Functions */
double fdTDotMan(BODY*,SYSTEM*,int*,int);
double fdTDotCore(BODY*,SYSTEM*,int*,int);
double fdTUMan(BODY*,int);
double fdTLMan(BODY*,int);
double fdTCMB(BODY*,int);
double fdTICB(BODY*,int);
double fdTJumpUMan(BODY*,int);
double fdTJumpLMan(BODY*,int);
double fdSignTJumpUMan(BODY*,int);
double fdSignTJumpLMan(BODY*,int);
double fdViscUMan(BODY*,int);
double fdViscLMan(BODY*,int);
double fdViscRatioMan(BODY*,int);
double fdBLUMan(BODY*,int);
double fdBLLMan(BODY*,int);
double fdShmodUMan(BODY*,int);
double fdShmodLMan(BODY*,int);
double fdFMeltUMan(BODY*,int);
double fdMeltfactorUMan(BODY*,int);
double fdTsolUMan(BODY*,int);  //solidus temp at UMBL
double fdTliqUMan(BODY*,int);  //liquidus at UMBL
double fdTsolLMan(BODY*,int);
double fdTliqLMan(BODY*,int);
double fdDepthMeltMan(BODY*,int);  //depth to bottom of UM melt region (where solidus and geother intersect).
double fdTDepthMeltMan(BODY*,int);  //T at bottom of UM melt region.
double fdSolTempDiffMan(double,BODY*,int);  //difference between solidus and geotherm at a given depth.
double fdSolidusMan(double);   //solidus at a given depth.
double fdTJumpMeltMan(BODY*,int);  //temp jump across UM melt region.
double fdImk2Man(BODY*,int);
double fdK2Man(BODY*,int);
double fdHfluxUMan(BODY*,int);
double fdHfluxLMan(BODY*,int);
double fdHfluxCMB(BODY*,int);
double fdHflowUMan(BODY*,int);
double fdHflowLMan(BODY*,int);
double fdHflowCMB(BODY*,int);
double fdHflowMeltMan(BODY*,int);
double fdHflowLatentMan(BODY*,UPDATE*,int);
double fdMassICDot(BODY*,UPDATE*,int);
double fdHflowLatentIC(BODY*,UPDATE*,int);
double fdPowerGravIC(BODY*,UPDATE*,int);
double fdTidalPowMan(BODY*,int);
double fdHflowSurfMan(BODY*,int);
double fdRIC(BODY*,int);
double fdDRICDTCMB(BODY*,int);

void fnForceBehaviorThermint(BODY*,EVOLVE*,IO*,int,int);

/* MATH  FUNCTIONS */
double cube(double);
double root(int,BODY*,int,double,double,double,int);
double cubicroot(int,BODY*,int);
#define max(a,b)	       \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
