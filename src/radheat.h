/**
 @file radheat.h

 @brief Subroutines that control the integration of the
        radiogenic heating model. Also includes subroutines
        that switch between the two models.

 @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

 @date Jun 28 2014

*/

/*
 * Earth conversions are subjective. Different options are available
 * I assume Earth's mantle is 1.4e-4
 *
 * PD NOTE: Default Radiogenic concentrations are from my radheat.pro, where values are based on
 * Korenaga (2006), Arevalo (2009), Huang (2013).
 * ENERGY values are Rory's (from Wikipedia?).  They seem ok.
 */

// PD NOTE: NUM is the fundamental parameter, pow and mass should reflect num.
// Changing the concentration requires changing all of these: pow, num, and mass.

// Relations: P=p*M, [P]=W, [p]=W/kg, [M]=kg
// M=N*m ,[N]=number, [m]=kg/1 atom
// p=e/(m*tau) ,[e]=J, [tau]=s

// 26Al -- Masses, numbers and powers from Wikipedia XXX Check!
#define ENERGY26AL        6.4153392709791e-13 // [J]
#define MASS26AL          26*ATOMMASS // [kg]  Mass of a 26Al atom.
#define HALFLIFE26AL      2.26267992e13  // [s]
/* Mantle */
#define EMASSMAN26AL      4.6e18      //9.9866e22   //9.854e22  //[kg]  mass of 26Mg on Earth
#define ENUMMAN26AL       1.0654e41   //2.31311e45  //2.2824e48  //[num] number of 26Mg atoms on Earth
#define EPOWERMAN26AL     2.0944e15   //4.54700e19  //1.009e35  //[W]  Power from 26Al if all Earth's 26Mg was 26Al
/* Core */
#define EMASSCORE26AL     1e-2*(EMASSMAN26AL)  //2.1417e20  //9.854e19  //[kg]
#define ENUMCORE26AL      1e-2*(ENUMMAN26AL)  //4.9606e42  //2.2824e45  //[num]
#define EPOWERCORE26AL    1e-2*(EPOWERMAN26AL)  //9.7513e16  //1.009e35  //[W]

// 40K
#define ENERGY40K        2.13371243669e-13 // [J?] -- weighting both Ar and Ca decay chains
#define MASS40K          39.96399848*ATOMMASS  // =6.6361772e-26 [kg]  Mass of a 40K atom.
#define HALFLIFE40K      5.736527e+16  //3.9762576e+16  //[s]
/* Mantle */
#define EMASSMAN40K      1.11075e+18  //[kg]  mass in Earths mantle of 40K at 4.5 Ga (t=0)
#define ENUMMAN40K       8.8692227e+42  //[num] number of 40K atoms in E mantle at 4.5Ga
#define EPOWERMAN40K     3.29892e+13  //[W]  Power from 40K in Earths Mantle at 4.5 Ga
/* Core */
#define EMASSCORE40K     3.90916e+17  //[kg]
#define ENUMCORE40K      3.1214255e+42  //[num]
#define EPOWERCORE40K    1.16102e+13  //[W]
/* Crust (assuming crustal mass constant) */
#define EMASSCRUST40K    4.67540e+17  //[kg]
#define ENUMCRUST40K     3.7332574e+42  //[num]
#define EPOWERCRUST40K   1.38859e+13  //[W]

//232TH
#define HALFLIFE232TH    6.3752726e+17 //4.4338428e17   // seconds
#define MASS232TH        232.0377*ATOMMASS
#define ENERGY232TH      6.833606619959e-12 // W
/* Mantle */
#define EMASSMAN232TH    2.31439e+17  //[kg]
#define ENUMMAN232TH     5.5262845e+41  //[num]
#define EPOWERMAN232TH   5.92484e+12  //[W]
/* Core */
#define EMASSCORE232TH   1.92682e+15  //[kg]
#define ENUMCORE232TH    4.6008515e+39  //[num]
#define EPOWERCORE232TH  4.93267e+10  //[W]
/* Crust (assuming crustal mass constant) */
#define EMASSCRUST232TH  1.49638e+17  //[kg]
#define ENUMCRUST232TH   3.5730363e+41  //[num]
#define EPOWERCRUST232TH 3.83072e+12  //[W]

//238U
#define HALFLIFE238U     2.0359742e+17  //1.409993568e17 // seconds
#define MASS238U         238.050788268*ATOMMASS
#define ENERGY238U       8.28293620554e-12 // [J]
/* Mantle */
#define EMASSMAN238U     1.16*1.00111e+17  //[kg]
#define ENUMMAN238U      1.16*2.2505054e+41  //[num]
#define EPOWERMAN238U    1.16*9.18017e+12  //[W]
/* Core */
#define EMASSCORE238U    7.83780e+14  //[kg]
#define ENUMCORE238U     1.7619456e+39  //[num]
#define EPOWERCORE238U   7.18726e+10  //[W]
/* Crust (assuming crustal mass constant) */
#define EMASSCRUST238U  5.64467e+16  //[kg]
#define ENUMCRUST238U   1.2689272e+41  //[num]
#define EPOWERCRUST238U  5.17616e+12  //[W]

//235U
#define HALFLIFE235U     3.2038174e+16  // seconds
#define MASS235U         235.0439299*ATOMMASS
#define ENERGY235U       6.55516e-12    // J
/* Mantle */
#define EMASSMAN235U     3.20325e+16  //[kg]
#define ENUMMAN235U      9.0058843e+40  //[num]
#define EPOWERMAN235U    1.84187e+13  //[W]
/* Core */
#define EMASSCORE235U    2.50786e+14  //[kg]
#define ENUMCORE235U     7.0508058e+38  //[num]
#define EPOWERCORE235U   1.44202e+11  //[W]
/* Crust (assuming crustal mass constant) */
#define EMASSCRUST235U   1.80612e+16  //[kg]
#define ENUMCRUST235U    5.0778866e+40  //[num]
#define EPOWERCRUST235U  1.03852e+13  //[W]

//Continental Crust Mass Earth today.  Note we don't define an oceanic crust reservoir (bc it's recycled and therefore part of the convecting mantle) so "crust" really means "continental crust".
#define EPOWERCRUSTTOTAL 7e12  //[W] 7 TW (Jaupart et al 2007)  or 6.8 TW (Huang 2013)
#define ECRUSTRADPOWDEN  (EPOWERCRUSTTOTAL)/(ECRUSTMASS)  //[W/kg] crustal heat production density.

/* Options Info */

#define OPTSTARTRADHEAT            1100 /* Start of RADHEAT options */
#define OPTENDRADHEAT              1200 /* End of RADHEAT options */

#define OPT_26ALMASSMAN            1110
#define OPT_26ALMASSCORE           1111
#define OPT_26ALNUMMAN             1112
#define OPT_26ALNUMCORE            1113
#define OPT_26ALPOWERMAN           1114
#define OPT_26ALPOWERCORE          1115

#define OPT_40KMASSMAN             1120
#define OPT_40KMASSCORE            1121
#define OPT_40KNUMMAN              1122
#define OPT_40KNUMCORE             1123
#define OPT_40KPOWERMAN            1124
#define OPT_40KPOWERCORE           1125
#define OPT_40KMASSCRUST           1126
#define OPT_40KNUMCRUST            1127
#define OPT_40KPOWERCRUST          1128

// Not currently coded
#define OPT_87RBMASS               1130
#define OPT_87RBNUM                1131
#define OPT_87RBPOWER              1132

#define OPT_232THMASSMAN           1135
#define OPT_232THMASSCORE          1136
#define OPT_232THNUMMAN            1137
#define OPT_232THNUMCORE           1138
#define OPT_232THPOWERMAN          1139
#define OPT_232THPOWERCORE         1140
#define OPT_232THMASSCRUST         1141
#define OPT_232THNUMCRUST          1142
#define OPT_232THPOWERCRUST        1143

#define OPT_235UMASSMAN            1145
#define OPT_235UMASSCORE           1146
#define OPT_235UNUMMAN             1147
#define OPT_235UNUMCORE            1148
#define OPT_235UPOWERMAN           1149
#define OPT_235UPOWERCORE          1150
#define OPT_235UMASSCRUST          1151
#define OPT_235UNUMCRUST           1152
#define OPT_235UPOWERCRUST         1153

#define OPT_238UMASSMAN            1155
#define OPT_238UMASSCORE           1156
#define OPT_238UNUMMAN             1157
#define OPT_238UNUMCORE            1158
#define OPT_238UPOWERMAN           1159
#define OPT_238UPOWERCORE          1160
#define OPT_238UMASSCRUST          1161
#define OPT_238UNUMCRUST           1162
#define OPT_238UPOWERCRUST         1163

#define OPT_HALT40KPOWER           1190
#define OPT_HALTMIN87RBPOWER       1191
#define OPT_HALT232THPOWER         1192
#define OPT_HALT238UPOWER	   1193
#define OPT_HALT235UPOWER          1194
#define OPT_HALTRADPOWER           1195

/* Halt Functions */
#define RADHEATHALTSYSEND       5
#define RADHEATHALTBODYEND      5

/* Output Functinos */

/* RADHEAT 1100 - 1199 */
#define OUTSTARTRADHEAT         1100
#define OUTENDRADHEAT           1200

/* Body Properties due to radiogenic heating */
#define OUT_26ALPOWERMAN	1110
#define OUT_26ALPOWERCORE       1111
#define OUT_26ALENFLUX	        1112
#define OUT_26ALDPOWERDT	1113
#define OUT_26ALDNUMDT          1114
#define OUT_26ALPOWERTIME       1115
#define OUT_26ALMASSMAN	        1116
#define OUT_26ALMASSCORE	1117
#define OUT_26ALNUMMAN          1118
#define OUT_26ALNUMCORE         1119
#define OUT_26ALTIME            1120

#define OUT_40KPOWERMAN	        1125
#define OUT_40KPOWERCORE        1126
#define OUT_40KENFLUX	        1127
#define OUT_40KDPOWERDT	        1128
#define OUT_40KDNUMDT           1129
#define OUT_40KPOWERTIME        1130
#define OUT_40KMASSMAN	        1131
#define OUT_40KMASSCORE	        1132
#define OUT_40KNUMMAN           1133
#define OUT_40KNUMCORE          1134
#define OUT_40KTIME             1135
#define OUT_40KPOWERCRUST       1136
#define OUT_40KMASSCRUST        1137
#define OUT_40KNUMCRUST         1138

#define OUT_232THPOWERMAN       1140
#define OUT_232THPOWERCORE      1141
#define OUT_232THENFLUX         1142
#define OUT_232THPOWERDT        1143
#define OUT_232THDNUMDT	        1144
#define OUT_232THPOWERTIME	1145
#define OUT_232THMASSMAN        1146
#define OUT_232THMASSCORE       1147
#define OUT_232THNUMMAN         1148
#define OUT_232THNUMCORE        1149
#define OUT_232THTIME           1150
#define OUT_232THPOWERCRUST     1151
#define OUT_232THMASSCRUST      1152
#define OUT_232THNUMCRUST       1153

#define OUT_238UPOWERMAN        1155
#define OUT_238UPOWERCORE       1156
#define OUT_238UENFLUX	        1157
#define OUT_D238UPOWERDT	1158
#define OUT_238UDNUMDT	        1159
#define OUT_238UPOWERTIME	1160
#define OUT_238UMASSMAN	        1161
#define OUT_238UMASSCORE        1162
#define OUT_238UNUMMAN          1163
#define OUT_238UNUMCORE         1164
#define OUT_238UTIME            1165
#define OUT_238UPOWERCRUST      1166
#define OUT_238UMASSCRUST       1167
#define OUT_238UNUMCRUST        1168

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
#define OUT_235UPOWERCRUST      1186
#define OUT_235UMASSCRUST       1187
#define OUT_235UNUMCRUST        1188

#define OUT_RADPOWERMAN         1190
#define OUT_RADPOWERCORE        1191
#define OUT_RADPOWERCRUST       1192
#define OUT_RADPOWERTOTAL       1193
#define OUT_SURFENFLUXRADTOTAL  1195

/* @cond DOXYGEN_OVERRIDE */

void AddModuleRadheat(MODULE*,int,int);
void BodyCopyRadheat(BODY*,BODY*,int,int,int);

/* Options Functions */
void HelpOptionsRadheat(OPTIONS*);
void Read40KPowerMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KMassMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KNumMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read26AlPowerMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read26AlMassMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read26AlNumMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThPowerMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThMassMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThNumMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UPowerMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UMassMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UNumMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read235UPowerMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read235UMassMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read235UNumMan(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);

void Read26AlPowerCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read26AlMassCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read26AlNumCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read40KPowerCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KMassCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KNumCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThPowerCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThMassCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThNumCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UPowerCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UMassCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UNumCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read235UPowerCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read235UMassCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read235UNumCore(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);

void Read40KPowerCrust(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KMassCrust(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KNumCrust(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThPowerCrust(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThMassCrust(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThNumCrust(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UPowerCrust(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UMassCrust(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UNumCrust(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read235UPowerCrust(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read235UMassCrust(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read235UNumCrust(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);

// XXX Need to add 26Al
void ReadHaltMin40KPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMin232ThPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMin238UPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMin235UPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMinRadPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void InitializeOptionsRadheat(OPTIONS*,fnReadOption[]);
void ReadOptionsRadheat(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

//int fbHaltMin40KPower(BODY*,CONTROL*,UPDATE*,int);
//int fbHaltMin232ThPower(BODY*,CONTROL*,UPDATE*,int);
//int fbHaltMin238UPower(BODY*,CONTROL*,UPDATE*,int);
int fbHaltMin40KPower(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);  //syntax now is same as fnHaltModule in vplanet.h
int fbHaltMin232ThPower(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
int fbHaltMin238UPower(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);
void CountHaltsRadHeat(HALT*,int*);


/* Verify Functions */

void VerifyRadheat(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,int,int);
void VerifyRotationRadheat(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetRadheat(CONTROL*,MODULE*);
void PropsAuxRadheat(BODY*,EVOLVE*,UPDATE*,int);
void VerifyHaltRadheat(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationRadheat(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateRadheat(BODY*,UPDATE*,int);
void FinalizeUpdateEccRadheat(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdate26AlNumManRadheat(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdate40KNumManRadheat(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdate232ThNumManRadheat(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdate238UNumManRadheat(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdate235UNumManRadheat(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateOblRadheat(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateRotRadheat(BODY*,UPDATE*,int*,int,int,int);
void FinalizeUpdateSemiRadheat(BODY*,UPDATE*,int*,int,int,int);

void HelpOutputRadheat(OUTPUT*);
void InitializeOutputRadheat(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionRadheat(OUTPUT*,int,int);
void FinalizeOutputFunctionRadheat(OUTPUT*,int,int);

void Write26AlPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write26AlEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD26AlPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write26AlTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write26AlMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write26AlNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write26AlPowerCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write26AlMassCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write26AlNumCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void Write40KPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD40KPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KPowerCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KMassCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KNumCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KPowerCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KMassCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KNumCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void Write232ThPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD232ThPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThPowerCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThMassCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThNumCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThPowerCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThMassCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThNumCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void Write238UPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD238UPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UPowerCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UMassCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UNumCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UPowerCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UMassCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UNumCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void Write235UPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD235UPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UMassMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UNumMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UPowerCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UMassCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UNumCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UPowerCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UMassCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write235UNumCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void WriteRadPowerMan(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRadPowerCore(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRadPowerCrust(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRadPowerTotal(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRadEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsRadheat(CONTROL*,FILE*);
void LogRadheat(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyRadheat(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* RadHeat functions */
void fnForceBehaviorRadheat(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable ***fnUpdate,int,int);

double fd26AlConstant(double,double);
double fd40KConstant(double,double);
double fd232ThConstant(double,double);
double fd238UConstant(double,double);
double fd235UConstant(double,double);

double fd26AlPowerMan(UPDATE*,int);
double fd40KPowerMan(UPDATE*,int);
double fd232ThPowerMan(UPDATE*,int);
double fd238UPowerMan(UPDATE*,int);
double fd235UPowerMan(UPDATE*,int);

double fdD26AlNumManDt(BODY*,SYSTEM*,int*);
double fdD40KNumManDt(BODY*,SYSTEM*,int*);
double fdD232ThNumManDt(BODY*,SYSTEM*,int*);
double fdD238UNumManDt(BODY*,SYSTEM*,int*);
double fdD235UNumManDt(BODY*,SYSTEM*,int*);

double fd26AlPowerCore(UPDATE*,int);
double fd40KPowerCore(UPDATE*,int);
double fd232ThPowerCore(UPDATE*,int);
double fd238UPowerCore(UPDATE*,int);
double fd235UPowerCore(UPDATE*,int);

double fd40KPowerCrust(UPDATE*,int);
double fd232ThPowerCrust(UPDATE*,int);
double fd238UPowerCrust(UPDATE*,int);
double fd235UPowerCrust(UPDATE*,int);

double fd26AlPower(UPDATE*,int);
double fd40KPower(UPDATE*,int);
double fd232ThPower(UPDATE*,int);
double fd235UPower(UPDATE*,int);
double fd238UPower(UPDATE*,int);

double fdRadPowerTotal(BODY*,int);
double fdRadPowerMan(UPDATE*,int);
double fdRadPowerCore(UPDATE*,int);
double fdRadPowerCrust(UPDATE*,int);

double fd26AlEnFlux(BODY*,UPDATE*,int);
double fd40KEnFlux(BODY*,UPDATE*,int);
double fd232ThEnFlux(BODY*,UPDATE*,int);
double fd235UEnFlux(BODY*,UPDATE*,int);
double fd238UEnFlux(BODY*,UPDATE*,int);
double fdSurfEnFluxRadTotal(BODY*,SYSTEM*,UPDATE*,int,int);

double fdD26AlNumCoreDt(BODY*,SYSTEM*,int*);
double fdD40KNumCoreDt(BODY*,SYSTEM*,int*);
double fdD232ThNumCoreDt(BODY*,SYSTEM*,int*);
double fdD238UNumCoreDt(BODY*,SYSTEM*,int*);
double fdD235UNumCoreDt(BODY*,SYSTEM*,int*);

double fdD40KNumCrustDt(BODY*,SYSTEM*,int*);
double fdD232ThNumCrustDt(BODY*,SYSTEM*,int*);
double fdD238UNumCrustDt(BODY*,SYSTEM*,int*);
double fdD235UNumCrustDt(BODY*,SYSTEM*,int*);

/* @endcond */
