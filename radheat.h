/***************** RADHEAT.H *********************** 
 *
 * Rory Barnes, Sat Jun 28 09:51:13 PDT 2014
 *
 * This header file contains all the subroutines in
 * file radheat.c.
 *
*/

/* 
 * Earth conversions are subjective. Different options are available
 * I assume Earth's mantle is 1.4e-4 
 *
 * PD NOTE: Default Radiogenic concentrations are from my radheat.pro, where values are based on 
 * Korenaga (2006), Arevalo (2009), Huang (2013).
 * ENERGY values are Rory's (from Wikipedia?).  They seem ok.
 */


#define RAD40K           10
#define HALFLIFE40K      5.6860542e+16  //[seconds]  really =tau_half/ln(2)=1/lambda
#define MASS40K          39.96399848*ATOMMASS  // =6.6361772e-23 [g]  Mass of a 40K atom.
#define EMASSMAN40K      8.2845415e+20  //[g] mass in Earth's mantle of 40K at 4.5 Ga (t=0)
#define ENUMMAN40K       1.2483906e+43  //[num] number of 40K atoms in E mantle at 4.5Ga
#define ENERGY40K        2.13371243669e-6 // [erg] -- weighting both Ar and Ca decay chains
#define EPOWERMAN40K     4.6846309e+20  //[erg/s]  Power from 40K in Earth's Mantle at 4.5 Ga

#define RAD232TH         11
#define HALFLIFE232TH    6.3752726e+17 //4.4338428e17   // seconds
#define MASS232TH        232.0377*ATOMMASS
#define EMASSMAN232TH    2.1017728e+20  //2.5982745e+20  //3.1e20
#define ENUMMAN232TH     5.4547913e+41  //6.5205779e+41  //1.45e42
#define ENERGY232TH      6.833606619959e-5 // ergs
#define EPOWERMAN232TH   5.8469497e+19  //erg/s

#define RAD238U          12
#define HALFLIFE238U     2.0359742e+17  //1.409993568e17 // seconds
#define MASS238U         238.050788268*ATOMMASS
#define EMASSMAN238U     8.3866516e+19  //1.0367830e+20  //8e19
#define ENUMMAN238U      2.1216313e+41  //2.3878961e+41  //7.5e41
#define ENERGY238U       8.28293620554e-5 // ergs
#define EPOWERMAN238U    8.6314141e+19  //erg/s

#define RAD235U          13     //Variable added PED
#define HALFLIFE235U     3.2038174e+16 // seconds
#define MASS235U         235.0439299*ATOMMASS
#define EMASSMAN235U     2.5403288e+19  //3.1404304e+19
#define ENUMMAN235U      6.5086633e+40  //8.7334519e+40
#define ENERGY235U       6.55516e-05 // ergs
#define EPOWERMAN235U    1.3317029e+20  //erg/s

void InitializeControlRadheat(CONTROL*);
void AddModuleRadheat(MODULE*,int,int);
void BodyCopyRadheat(BODY*,BODY*,int,int);
void InitializeBodyEqtide(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyRadheat(BODY*,CONTROL*,UPDATE*,int);


/* Options Info */

#define OPTSTARTRADHEAT         1100 /* Start of RADHEAT options */
#define OPTENDRADHEAT           1200 /* End of RADHEAT options */

#define OPT_87RBMASS            1120
#define OPT_87RBNUM             1122
#define OPT_87RBPOWER            1124

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
void HelpOptionsRadheat(OPTIONS*);
void Read40KPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KMass(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KNum(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int); 
void Read232ThMass(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int); 
void Read232ThNum(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UMass(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UNum(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read235UPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);  //PED
void Read235UMass(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);  //PED
void Read235UNum(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);  //PED
void ReadHaltMin40KPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMin232ThPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMin238UPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMin235UPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);  //PED
void InitializeOptionsRadheat(OPTIONS*,fnReadOption[]);
void ReadOptionsRadheat(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Halt Functions */
#define RADHEATHALTSYSEND       5
#define RADHEATHALTBODYEND      5

int HaltMin40KPower(BODY*,CONTROL*,UPDATE*,int);
int HaltMin232ThPower(BODY*,CONTROL*,UPDATE*,int);
int HaltMin238UPower(BODY*,CONTROL*,UPDATE*,int);
int HaltMin235UPower(BODY*,CONTROL*,UPDATE*,int);  //PED
void CountHaltsRadHeat(HALT*,int*);


/* Verify Functions */

void VerifyRadheat(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void VerifyRotationRadheat(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetRadheat(CONTROL*,MODULE*);
void PropertiesRadheat(BODY*,int);
void VerifyHaltRadheat(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationRadheat(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateRadheat(BODY*,UPDATE*,int);
void FinalizeUpdateEccRadheat(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateNumIsotopeRadheat(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateOblRadheat(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateRotRadheat(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateSemiRadheat(BODY*,UPDATE*,int*,int,int);

/* Output Functinos */

/* RADHEAT 1100 - 1199 */
#define OUTSTARTRADHEAT         1100
#define OUTENDRADHEAT           1200

/* Body Properties due to radiogenic heating */

/*
#define OUT_40KPOWER	        1110
#define OUT_40KENFLUX	        1112
#define OUT_40KDPOWERDT	        1114
#define OUT_40KDNUMDT	        1115
#define OUT_40KPOWERIME	        1116
#define OUT_40KMASS	        1118
#define OUT_40KNUM              1120
#define OUT_40KTIME             1125
#define OUT_232THPOWER	        1130
#define OUT_232THENFLUX       1132
#define OUT_232THPOWERDT	        1134
#define OUT_232THDNUMDT	        1135
#define OUT_232THPOWERIME	1136
#define OUT_232THMASS	        1138
#define OUT_232THNUM            1140
#define OUT_232THTIME           1145
#define OUT_238UPOWER	        1150
#define OUT_238UENFLUX	        1152
#define OUT_D238UPOWERDT	1154
#define OUT_238UDNUMDT	        1155
#define OUT_238UPOWERIME	1156
#define OUT_238UMASS	        1158
#define OUT_238UNUM             1160
#define OUT_238UTIME            1165
#define OUT_235UPOWER	        1170  //PED
#define OUT_235UENFLUX	        1171
#define OUT_D235UPOWERDT	1172
#define OUT_235UDNUMDT	        1173  //PED
#define OUT_235UPOWERIME	1174
#define OUT_235UMASS	        1175
#define OUT_235UNUM             1176  //PED
#define OUT_235UTIME            1177
*/
#define OUT_40KPOWERMAN	        1110
#define OUT_40KENFLUX	        1112
#define OUT_40KDPOWERDT	        1114
#define OUT_40KDNUMDT           1115
#define OUT_40KPOWERIME	        1116
#define OUT_40KMASSMAN	        1118
#define OUT_40KNUMMAN           1120
#define OUT_40KTIME             1125

#define OUT_232THPOWERMAN       1130
#define OUT_232THENFLUX         1132
#define OUT_232THPOWERDT        1134
#define OUT_232THDNUMDT	        1135
#define OUT_232THPOWERIME	1136
#define OUT_232THMASSMAN        1138
#define OUT_232THNUMMAN         1140
#define OUT_232THTIME           1145

#define OUT_238UPOWERMAN        1150
#define OUT_238UENFLUX	        1152
#define OUT_D238UPOWERDT	1154
#define OUT_238UDNUMDT	        1155
#define OUT_238UPOWERIME	1156
#define OUT_238UMASSMAN	        1158
#define OUT_238UNUMMAN          1160
#define OUT_238UTIME            1165

#define OUT_235UPOWERMAN        1170  //PED
#define OUT_235UENFLUX	        1171
#define OUT_D235UPOWERDT	1172
#define OUT_235UDNUMDT	        1173  //PED
#define OUT_235UPOWERIME	1174
#define OUT_235UMASSMAN	        1175
#define OUT_235UNUMMAN          1176  //PED
#define OUT_235UTIME            1177

#define OUT_RADPOWERMAN            1180  //1175
#define OUT_SURFENFLUXRADHEAT   1181  //1180

void HelpOutputRadheat(OUTPUT*);
void InitializeOutputRadheat(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionRadheat(OUTPUT*,int,int);
void FinalizeOutputFunctionRadheat(OUTPUT*,int,int);

void Write40KPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD40KPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void Write232ThPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD232ThPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void Write238UPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD238UPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]); //PED

void Write235UPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);  //PED
void Write235UEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);  //PED
void WriteD235UPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);  //PED
void Write235UTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);  //PED
void Write235UMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);  //PED
void Write235UNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]); //PED

void WriteRadPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRadEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsRadheat(CONTROL*,FILE*);
void LogRadheat(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyRadheat(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* RadHeat functions */

/* Needed? 
void fdRadPower(BODY*,int);
void fd40KConstant(BODY*,int);
void fd232ThConstant(BODY*,int);
void fd238UConstant(BODY*,int);
*/

double fdSurfEnFluxRadheat(BODY*,SYSTEM*,UPDATE *update,int,int);
double fdInternalPowerRadheat(BODY*,SYSTEM*,UPDATE *update,int,int);
double fdRadPower(double,double,double); // XXX

void fnForceBehaviorRadheat(BODY*,EVOLVE*,IO*,int,int);

/*
double fd40KConstant(double,double);
double fd232ThConstant(double,double);
double fd238UConstant(double,double);
double fd235UConstant(double,double);  //PED
double fd40KPower(BODY*,SYSTEM*,int*,int);
double fd232ThPower(BODY*,SYSTEM*,int);
double fd238UPower(BODY*,SYSTEM*,int);
double fd235UPower(BODY*,SYSTEM*,int);   //PED
double fdTotalRadPower(BODY*,int);
*/
double fd40KConstantMan(double,double);
double fd232ThConstantMan(double,double);
double fd238UConstantMan(double,double);
double fd235UConstantMan(double,double);  //PED
double fd40KPowerMan(BODY*,SYSTEM*,int*,int);
double fd232ThPowerMan(BODY*,SYSTEM*,int);
double fd238UPowerMan(BODY*,SYSTEM*,int);
double fd235UPowerMan(BODY*,SYSTEM*,int);   //PED
double fdTotalRadPowerMan(BODY*,int);
double fdD40KNumManDt(BODY*,SYSTEM*,int*,int);
double fdD232ThNumManDt(BODY*,SYSTEM*,int*,int);
double fdD238UNumManDt(BODY*,SYSTEM*,int*,int);
double fdD235UNumManDt(BODY*,SYSTEM*,int*,int);  //PED

