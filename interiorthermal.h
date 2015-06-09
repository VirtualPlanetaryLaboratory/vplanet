/***************** INTERIORTHERMAL.H *********************** 
 *
 * Peter Driscoll 6/1/15
 *
 * This header file contains all the subroutines in
 * file interiorthermal.c.
 *
*/

// UNITS CONSTANTS (for converting SI to cgs (swallow barf in mouth))
#define ERG_JOULE        1d7       //=erg/J
#define ERG_S_WATT       1d7       //=erg/s/W (W=J/s)
#define G_KG             1d3       //=g/kg
#define CM_M             1d2       //=cm/m
#define CM_KM            1d3*CM_M  //=cm/km
#define DENS_SI_CGS      G_KG/(CM_M)^3 //=(g/cm^3)/(kg/m^3) to convert SI to cgs.

// FUNDAMENTAL CONSTANTS
#define LORENTZNUMBER_SI 2.5d-8                   //[W Ohm/K] Lorentz Number
#define MAGPERM_SI       4*PI*1d-7                //[H/m] magnetic permeability constant in SI units.
#define RAYLEIGHCRIT     660.                     //[-] critical rayleigh number for spherical convection.
#define GASCONSTANT_SI   8.3144621                //[J/mol/K] gas constant in SI units.      

// E MASS RESERVOIRS
#define TMAN             20                       //index of TMAN variable.
#define EMASSFRACMAN     0.67                     //mass fraction of E mantle
#define EMASSMAN         EMASSFRACMAN*MEARTH      //[g] mass of E mantle
#define EMASSFRACCORE    0.32                     //mass frac E core
#define EMASSCORE        EMASSFRACCORE*MEARTH     //[g] mass of E core
#define EMASSIC          9.7d25                   //[g] mass of present IC.
#define EMASSOC          EMASSCORE-EMASSIC        //[g] mass of present OC.
#define EMASSOCEAN       1.4d24                   //[g] mass of E oceans
#define EMASSFRACOCEAN   EMASSOCEAN/MEARTH        //ocean mass frac
#define EMASSFRACCRUST   1-EMASSFRACMAN-EMASSFRACCORE-EMASSFRACOCEAN  //crust is remaining bit.
#define EMASSCRUST       EMASSFRACCRUST*MEARTH    //[g] mass E crust today.

// INTERNAL STRUCTURE E
#define ERADIUS          6371*CM_KM          //[cm] radius of E.
#define ERMAN            6371*CM_KM          //[cm] radius of E mantle.
#define ERCORE           3481*CM_KM          //[cm] radius Core
#define ERICB            1221*CM_KM          //[cm] present day ICB radius.
#define EVOL             4/3*PI*ERADIUS^3    //[cm^3] volume of E.
#define EVOLIC           4/3*PI*ERICB^3      //[cm^3] volume of present-day IC.
#define EVOLCORE         4/3*PI*ERCORE^3     //[cm^3] volume of total core.
#define EVOLOC           EVOLCORE-EVOLIC     //[cm^3] volume of OC today.
#define EVOLEARTH        4/3*PI*ERADIUS^3    //[cm^3] volume of E.
#define EVOLMAN          EVOLEARTH-EVOLCORE  //[cm^3] volume of Mantle.
// DENSITIES - derived from mass and radius dfns.
#define EDENS            MEARTH/EVOL         //[g/cm^3] density of E
#define EDENSMAN         EMASSMAN/EVOLMAN    //[g/cm^3] density of E mantle.
#define EDENSCORE        EMASSCORE/EVOLCORE  //[g/cm^3] density of E core.
#define EDENSOC          EMASSOC/EVOLCORE    //[g/cm^3] density of E core.
#define EDENSIC          EMASSIC/EVOLIC      //[g/cm^3] density of E IC.
#define DENSOCLE         700*DENS_SI_CGS     //[g/cm^3] density diff of OC light elements.

// THERMAL PROPERTIES
#define SPECHEATMAN_SI   1265                     //[J/kg/K] specfic heat mantle.
#define SPECHEATMAN      CPMAN_SI*ERG_JOULE/G_KG  //[erg/g/K]
#define SPECHEATCORE_SI  840                      //[J/kg/K] sp heat core.
#define SPECHEATCORE     CPCORE_SI*ERG_JOULE/G_KG //[erg/g/K]
#define THERMEXPANMAN    3d-5                     //[K^-1] thermal expansivity of mantle.
#define THERMEXPANCORE   1d-5                     //[K^-1] thermal expansivity of core.
#define THERMCONDUMAN_SI 4.2                      //[W/m/K] thermal conductivity of Upper mantle.
#define THERMCONDLMAN_SI 10.0                     //[W/m/K] thermal conductivity of Lower mantle.
#define THERMALCONDUMAN  THERMALCONDUMAN_SI*ERG_S_WATT/CM_M  //[erg/s/cm/K]
#define THERMALCONDLMAN  THERMALCONDLMAN_SI*ERG_S_WATT/CM_M  //[erg/s/cm/K]
#define THERMALDIFFUMAN  THERMALCONDUMAN/(EDENSMAN*CPMAN)    //[cm^2/s] thermal diffusivity mantle.
#define ELECCONDCORE_SI  10d5                     //[S/m]  electrical conductivity of core.  How does this convert to cgs??
#define SPECLATENTICB_SI 750d3                    //[J/kg] specific latent heat release at ICB
#define SPECLATENTMAN_SI 320d3                    //[J/kg] specific latent heat release at ICB
#define SPECLATENTICB    SPECLATENTICB_SI*ERG_JOULE/G_KG //[erg/J/g] in cgs.
#define SPECLATENTMAN    SPECLATENTMAN_SI*ERG_JOULE/G_KG //[erg/J/g] in cgs.
// VISCOSITY PROPERTIES
#define ACTIVENGMAN_SI   3d5                      //[J/mol]


void InitializeControlInteriorthermal(CONTROL*);
void AddModuleInteriorthermal(MODULE*,int,int);
void BodyCopyInteriorthermal(BODY*,BODY*,int,int);
void InitializeBodyEqtide(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyInteriorthermal(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */

#define OPTSTARTINTERIORTHERMAL         1200 /* Start of INTERIORTHERMAL options */
#define OPTENDINTERIORTHERMAL           1400 /* End of INTERIORTHERMAL options */

#define OPT_40KMASSMAN            1110
#define OPT_40KMASSCORE           1111
#define OPT_40KNUMMAN             1112
#define OPT_40KNUMCORE            1113 
#define OPT_40KPOWERMAN           1114
#define OPT_40KPOWERCORE          1115
#define OPT_87RBMASS              1120
#define OPT_87RBNUM               1122
#define OPT_87RBPOWER             1124
#define OPT_232THMASSMAN          1130
#define OPT_232THMASSCORE         1131
#define OPT_232THNUMMAN           1132
#define OPT_232THNUMCORE          1133
#define OPT_232THPOWERMAN         1134
#define OPT_232THPOWERCORE        1135
#define OPT_238UMASSMAN           1140
#define OPT_238UMASSCORE          1141
#define OPT_238UNUMMAN            1142
#define OPT_238UNUMCORE           1143
#define OPT_238UPOWERMAN          1144
#define OPT_238UPOWERCORE         1145
#define OPT_235UMASSMAN           1150  
#define OPT_235UMASSCORE          1151  
#define OPT_235UNUMMAN            1152
#define OPT_235UNUMCORE           1153
#define OPT_235UPOWERMAN          1154
#define OPT_235UPOWERCORE         1155
#define OPT_HALTMIN40KPOWERMAN    1170
#define OPT_HALTMIN40KPOWERCORE   1171
#define OPT_HALTMIN87RBPOWER      1172
#define OPT_HALTMIN232THPOWERMAN  1174
#define OPT_HALTMIN232THPOWERCORE 1175
#define OPT_HALTMIN238UPOWERMAN	  1176
#define OPT_HALTMIN238UPOWERCORE  1177
#define OPT_HALTMIN235UPOWERMAN   1178  
#define OPT_HALTMIN235UPOWERCORE  1179  


/* Options Functions */
void HelpOptionsInteriorthermal(OPTIONS*);
void Read40KPowerMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KMassMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KNumMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThPowerMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int); 
void Read232ThMassMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int); 
void Read232ThNumMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UPowerMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UMassMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UNumMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read235UPowerMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);  //PED
void Read235UMassMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);  //PED
void Read235UNumMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);  //PED

void Read40KPowerCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KMassCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KNumCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThPowerCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int); 
void Read232ThMassCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int); 
void Read232ThNumCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UPowerCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UMassCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UNumCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read235UPowerCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);  //PED
void Read235UMassCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);  //PED
void Read235UNumCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);  //PED

void ReadHaltMin40KPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMin232ThPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMin238UPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMin235UPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);  //PED
void InitializeOptionsInteriorthermal(OPTIONS*,fnReadOption[]);
void ReadOptionsInteriorthermal(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Halt Functions */
#define INTERIORTHERMALHALTSYSEND       6
#define INTERIORTHERMALHALTBODYEND      6

//int fbHaltMin40KPower(BODY*,CONTROL*,UPDATE*,int);
//int fbHaltMin232ThPower(BODY*,CONTROL*,UPDATE*,int);
//int fbHaltMin238UPower(BODY*,CONTROL*,UPDATE*,int);
int fbHaltMin40KPower(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);  //syntax now is same as fnHaltModule in vplanet.h
int fbHaltMin232ThPower(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
int fbHaltMin238UPower(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
void CountHaltsRadHeat(HALT*,int*);


/* Verify Functions */

void VerifyInteriorthermal(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void VerifyRotationInteriorthermal(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetInteriorthermal(CONTROL*,MODULE*);
void PropertiesInteriorthermal(BODY*,int);
void VerifyHaltInteriorthermal(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationInteriorthermal(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateInteriorthermal(BODY*,UPDATE*,int);
void FinalizeUpdateEccInteriorthermal(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdate40KNumManInteriorthermal(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdate232ThNumManInteriorthermal(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdate238UNumManInteriorthermal(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdate235UNumManInteriorthermal(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateOblInteriorthermal(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateRotInteriorthermal(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateSemiInteriorthermal(BODY*,UPDATE*,int*,int,int);

/* Output Functinos */

/* INTERIORTHERMAL 1100 - 1199 */
#define OUTSTARTINTERIORTHERMAL         1200
#define OUTENDINTERIORTHERMAL           1400

/* Body Properties due to radiogenic heating */
#define OUT_40KPOWERMAN	        1110
#define OUT_40KPOWERCORE        1111
#define OUT_40KENFLUX	        1112
#define OUT_40KDPOWERDT	        1114
#define OUT_40KDNUMDT           1115
#define OUT_40KPOWERTIME        1116
#define OUT_40KMASSMAN	        1118
#define OUT_40KMASSCORE	        1119
#define OUT_40KNUMMAN           1120
#define OUT_40KNUMCORE          1121
#define OUT_40KTIME             1125

#define OUT_232THPOWERMAN       1130
#define OUT_232THPOWERCORE      1131
#define OUT_232THENFLUX         1132
#define OUT_232THPOWERDT        1134
#define OUT_232THDNUMDT	        1135
#define OUT_232THPOWERTIME	1136
#define OUT_232THMASSMAN        1138
#define OUT_232THMASSCORE       1139
#define OUT_232THNUMMAN         1140
#define OUT_232THNUMCORE        1141
#define OUT_232THTIME           1145

#define OUT_238UPOWERMAN        1150
#define OUT_238UPOWERCORE       1151
#define OUT_238UENFLUX	        1152
#define OUT_D238UPOWERDT	1154
#define OUT_238UDNUMDT	        1155
#define OUT_238UPOWERTIME	1156
#define OUT_238UMASSMAN	        1158
#define OUT_238UMASSCORE        1159
#define OUT_238UNUMMAN          1160
#define OUT_238UNUMCORE         1161
#define OUT_238UTIME            1165

#define OUT_235UPOWERMAN        1170  
#define OUT_235UPOWERCORE       1171  
#define OUT_235UENFLUX	        1172
#define OUT_D235UPOWERDT	1174
#define OUT_235UDNUMDT	        1175 
#define OUT_235UPOWERTIME	1176
#define OUT_235UMASSMAN	        1178
#define OUT_235UMASSCORE        1179
#define OUT_235UNUMMAN          1180
#define OUT_235UNUMCORE         1181 
#define OUT_235UTIME            1185

#define OUT_RADPOWERMAN         1190  
#define OUT_RADPOWERCORE        1191
#define OUT_RADPOWERTOTAL       1192  
#define OUT_SURFENFLUXINTERIORTHERMAL   1195

void HelpOutputInteriorthermal(OUTPUT*);
void InitializeOutputInteriorthermal(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionInteriorthermal(OUTPUT*,int,int);
void FinalizeOutputFunctionInteriorthermal(OUTPUT*,int,int);

void Write40KPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD40KPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KPowerCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KMassCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KNumCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void Write232ThPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD232ThPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThPowerCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThMassCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThNumCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void Write238UPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD238UPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]); 
void Write238UPowerCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UMassCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UNumCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void Write235UPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);  
void WriteD235UPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]); 
void Write235UNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]); 
void Write235UPowerCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UMassCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]); 
void Write235UNumCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]); 

void WriteRadPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRadPowerCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRadPowerTotal(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRadEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsInteriorthermal(CONTROL*,FILE*);
void LogInteriorthermal(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyInteriorthermal(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* RadHeat functions */
double fdSurfEnFluxInteriorthermal(BODY*,SYSTEM*,UPDATE *update,int,int);
double fdRadPowerMan(BODY*,SYSTEM*,UPDATE *update,int,int);
double fdRadPowerCore(BODY*,SYSTEM*,UPDATE *update,int,int);
double fdRadPowerTotal(BODY*,SYSTEM*,UPDATE *update,int,int);
double fdRadPower(double,double,double); // XXX

void fnForceBehaviorInteriorthermal(BODY*,EVOLVE*,IO*,int,int);

double fd40KConstant(double,double);
double fd232ThConstant(double,double);
double fd238UConstant(double,double);
double fd235UConstant(double,double);  
double fd40KPowerMan(BODY*,SYSTEM*,int*,int);
double fd232ThPowerMan(BODY*,SYSTEM*,int);
double fd238UPowerMan(BODY*,SYSTEM*,int);
double fd235UPowerMan(BODY*,SYSTEM*,int); 
double fdTotalRadPowerMan(BODY*,int);
double fdD40KNumManDt(BODY*,SYSTEM*,int*,int);
double fdD232ThNumManDt(BODY*,SYSTEM*,int*,int);
double fdD238UNumManDt(BODY*,SYSTEM*,int*,int);
double fdD235UNumManDt(BODY*,SYSTEM*,int*,int); 

double fd40KPowerCore(BODY*,SYSTEM*,int*,int);
double fd232ThPowerCore(BODY*,SYSTEM*,int);
double fd238UPowerCore(BODY*,SYSTEM*,int);
double fd235UPowerCore(BODY*,SYSTEM*,int);
double fdTotalRadPowerCore(BODY*,int);
double fdD40KNumCoreDt(BODY*,SYSTEM*,int*,int);
double fdD232ThNumCoreDt(BODY*,SYSTEM*,int*,int);
double fdD238UNumCoreDt(BODY*,SYSTEM*,int*,int);
double fdD235UNumCoreDt(BODY*,SYSTEM*,int*,int); 
