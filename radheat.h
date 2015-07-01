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

// 40K
#define RAD40KMAN        10
#define HALFLIFE40K      5.6860542e+16  //[seconds]  really =tau_half/ln(2)=1/lambda
#define ENERGY40K        2.13371243669e-6 // [erg] -- weighting both Ar and Ca decay chains
#define MASS40K          39.96399848*ATOMMASS  // =6.6361772e-23 [g]  Mass of a 40K atom.
#define EMASSMAN40K      8.2845415e+20  //gm  mass in Earths mantle of 40K at 4.5 Ga (t=0)
#define ENUMMAN40K       1.2483906e+43  //[num] number of 40K atoms in E mantle at 4.5Ga
#define EPOWERMAN40K     4.6846309e+20  //[erg/s]  Power from 40K in Earths Mantle at 4.5 Ga =    4.6846309e+13 [W]

#define RAD40KCORE       11
#define EMASSCORE40K     2.0162503e+20  //gm
#define ENUMCORE40K      3.0382705e+42  //num 40K
#define EPOWERCORE40K    1.1401220e+20  //erg/s =    1.1401220e+13 [W]

//232TH
#define RAD232THMAN      12
#define HALFLIFE232TH    6.3752726e+17 //4.4338428e17   // seconds
#define MASS232TH        232.0377*ATOMMASS
#define ENERGY232TH      6.833606619959e-5 // ergs
#define EMASSMAN232TH    2.1017728e+20  //2.5982745e+20  //3.1e20
#define ENUMMAN232TH     5.4547913e+41  //6.5205779e+41  //1.45e42
#define EPOWERMAN232TH   5.8469497e+19  //erg/s

#define RAD232THCORE     13
#define EMASSCORE232TH   1.0230379e+18  //gm
#define ENUMCORE232TH    2.6551196e+39  //num 232Th
#define EPOWERCORE232TH  2.8460027e+17  //erg/s =    2.8460027e+10 [W]


//238U
#define RAD238UMAN       14
#define HALFLIFE238U     2.0359742e+17  //1.409993568e17 // seconds
#define MASS238U         238.050788268*ATOMMASS
#define ENERGY238U       8.28293620554e-5 // ergs
#define EMASSMAN238U     8.3866516e+19  //1.0367830e+20  //8e19
#define ENUMMAN238U      2.1216313e+41  //2.3878961e+41  //7.5e41
#define EPOWERMAN238U    8.6314141e+19  //erg/s

#define RAD238UCORE      15
#define EMASSCORE238U    4.0822026e+17  //gm
#define ENUMCORE238U     1.0327040e+39  //num 238U
#define EPOWERCORE238U   4.2013407e+17  //erg/s =    4.2013407e+10 [W]

//235U
#define RAD235UMAN       16
#define HALFLIFE235U     3.2038174e+16 // seconds
#define MASS235U         235.0439299*ATOMMASS
#define ENERGY235U       6.55516e-05 // ergs
#define EMASSMAN235U     2.5403288e+19  //3.1404304e+19
#define ENUMMAN235U      6.5086633e+40  //8.7334519e+40
#define EPOWERMAN235U    1.3317029e+20  //erg/s

#define RAD235UCORE      17
#define EMASSCORE235U    1.2365050e+17  //gm
#define ENUMCORE235U     3.1680918e+38  //num 235U
#define EPOWERCORE235U   6.4820636e+17  //erg/s =    6.4820636e+10 [W]

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
void InitializeOptionsRadheat(OPTIONS*,fnReadOption[]);
void ReadOptionsRadheat(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Halt Functions */
#define RADHEATHALTSYSEND       5
#define RADHEATHALTBODYEND      5

//int fbHaltMin40KPower(BODY*,CONTROL*,UPDATE*,int);
//int fbHaltMin232ThPower(BODY*,CONTROL*,UPDATE*,int);
//int fbHaltMin238UPower(BODY*,CONTROL*,UPDATE*,int);
int fbHaltMin40KPower(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);  //syntax now is same as fnHaltModule in vplanet.h
int fbHaltMin232ThPower(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
int fbHaltMin238UPower(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
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
void FinalizeUpdate40KNumManRadheat(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdate232ThNumManRadheat(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdate238UNumManRadheat(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdate235UNumManRadheat(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateOblRadheat(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateRotRadheat(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateSemiRadheat(BODY*,UPDATE*,int*,int,int);

/* Output Functinos */

/* RADHEAT 1100 - 1199 */
#define OUTSTARTRADHEAT         1100
#define OUTENDRADHEAT           1200

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
#define OUT_SURFENFLUXRADHEAT   1195

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
void LogOptionsRadheat(CONTROL*,FILE*);
void LogRadheat(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyRadheat(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* RadHeat functions */
double fdSurfEnFluxRadheat(BODY*,SYSTEM*,UPDATE*,int,int);
double fdRadPowerMan(BODY*,UPDATE*,int);
double fdRadPowerCore(BODY*,UPDATE*,int);
double fdRadPowerTotal(BODY*,UPDATE*,int);
double fdRadPower(double,double,double); // XXX

void fnForceBehaviorRadheat(BODY*,EVOLVE*,IO*,int,int);

double fd40KConstant(double,double);
double fd232ThConstant(double,double);
double fd238UConstant(double,double);
double fd235UConstant(double,double);  
double fd40KPowerMan(BODY*,SYSTEM*,int*,int);
double fd232ThPowerMan(BODY*,SYSTEM*,int);
double fd238UPowerMan(BODY*,SYSTEM*,int);
double fd235UPowerMan(BODY*,SYSTEM*,int); 
double fdTotalRadPowerMan(BODY*,int);
double fdD40KNumManDt(BODY*,SYSTEM*,int*);
double fdD232ThNumManDt(BODY*,SYSTEM*,int*);
double fdD238UNumManDt(BODY*,SYSTEM*,int*);
double fdD235UNumManDt(BODY*,SYSTEM*,int*); 

double fd40KPowerCore(BODY*,SYSTEM*,int*,int);
double fd232ThPowerCore(BODY*,SYSTEM*,int);
double fd238UPowerCore(BODY*,SYSTEM*,int);
double fd235UPowerCore(BODY*,SYSTEM*,int);
double fdTotalRadPowerCore(BODY*,int);
double fdD40KNumCoreDt(BODY*,SYSTEM*,int*);
double fdD232ThNumCoreDt(BODY*,SYSTEM*,int*);
double fdD238UNumCoreDt(BODY*,SYSTEM*,int*);
double fdD235UNumCoreDt(BODY*,SYSTEM*,int*); 
