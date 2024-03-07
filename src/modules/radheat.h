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
 * PD NOTE: Default Radiogenic concentrations are from my radheat.pro, where
 * values are based on Korenaga (2006), Arevalo (2009), Huang (2013). ENERGY
 * values are Rory's (from Wikipedia?).  They seem ok.
 */

// PD NOTE: NUM is the fundamental parameter, pow and mass should reflect num.
// Changing the concentration requires changing all of these: pow, num, and
// mass.

// Relations: P=p*M, [P]=W, [p]=W/kg, [M]=kg
// M=N*m ,[N]=number, [m]=kg/1 atom
// p=e/(m*tau) ,[e]=J, [tau]=s

/* 26Al -- Masses, numbers and powers from Wikipedia XXX Check! */
#define ENERGY26AL 6.4153392709791e-13 /**< [J] Decay energy of Aluminum-26 */
#define MASS26AL 26 * ATOMMASS         /**< [kg] Mass of a 26Al atom */
#define HALFLIFE26AL 2.26267992e13     /**< [s] Half-life of 26Al */
/* Mantle */
#define EMASSMAN26AL 4.6e18 /**< [kg]  Default mantle mass of 26Al */
#define ENUMMAN26AL                                                            \
  1.0654e41 /**< [num] Default mantle number of 26Al atoms                     \
             */
#define EPOWERMAN26AL                                                          \
  2.0944e15 /**< [W] Default mantle power from 26Al if all 26Mg from 26Al      \
               decay */
/* Core */
#define EMASSCORE26AL                                                          \
  1e-2 * (EMASSMAN26AL) /**< [kg]  Default core mass of 26Al */
#define ENUMCORE26AL                                                           \
  1e-2 * (ENUMMAN26AL) /**< [num] Default core number of 26Al atoms */
#define EPOWERCORE26AL                                                         \
  1e-2 * (EPOWERMAN26AL) /**< [W] Default core power from 26Al if all 26Mg     \
                            from 26Al decay */

/* 40K */
#define ENERGY40K 2.13371243669e-13    /**< [J] Decay energy of Potassium-40 */
#define MASS40K 39.96399848 * ATOMMASS /**< [kg] Mass of a 40K atom */
#define HALFLIFE40K 5.736527e+16       /**< [s] Half-life of 40K */
/* Mantle */
#define EMASSMAN40K 6.45089e+17 /**< [kg]  Default mantle mass of 40K */
#define ENUMMAN40K                                                             \
  9.72110e+42 /**< [num] Default mantle number of 40K atoms                    \
               */
#define EPOWERMAN40K                                                           \
  3.61578e+13 /**< [W] Default mantle power from 40K decay                     \
               */
/* Core */
#define EMASSCORE40K 6.04046e+17 /**< [kg]  Default core mass of 40K */
#define ENUMCORE40K 9.10261e+42  /**< [num] Default core number of 40K atoms */
#define EPOWERCORE40K                                                          \
  3.38573e+13 /**< [W] Default core power from 40K decay                       \
               */
/* Crust (assuming crustal mass constant) */
#define EMASSCRUST40K 2.47738e+17 /**< [kg]  Default crust mass of 40K */
#define ENUMCRUST40K                                                           \
  3.73326e+42 /**< [num] Default crust number of 40K atoms                     \
               */
#define EPOWERCRUST40K                                                         \
  1.38859e+13 /**< [W] Default crust power from 40K decay */

/* 232TH */
#define ENERGY232TH 6.833606619959e-12 /**< [J] Decay energy of 232Th */
#define MASS232TH 232.0377 * ATOMMASS  /**< [kg] Mass of a 232Th atom */
#define HALFLIFE232TH 6.3752726e+17    /**< [s] Half-life of 232Th */
/* Mantle */
#define EMASSMAN232TH 2.34211e+17 /**< [kg]  Default mantle mass of 232Th */
#define ENUMMAN232TH                                                           \
  6.07873e+41 /**< [num] Default mantle number of 232Th atoms */
#define EPOWERMAN232TH                                                         \
  6.51575e+12 /**< [W] Default mantle power from 232Th decay */
/* Core */
#define EMASSCORE232TH 5.22682e+15 /**< [kg]  Default core mass of 232Th */
#define ENUMCORE232TH                                                          \
  1.35657e+40 /**< [num] Default core number of 232Th atoms */
#define EPOWERCORE232TH                                                        \
  1.45410e+11 /**< [W] Default core power from 232Th decay */
/* Crust (assuming crustal mass constant) */
#define EMASSCRUST232TH 1.37667e+17 /**< [kg]  Default crust mass of 232Th */
#define ENUMCRUST232TH                                                         \
  3.57304e+41 /**< [num] Default crust number of 232Th atoms */
#define EPOWERCRUST232TH                                                       \
  3.82991e+12 /**< [W] Default crust power from 232Th decay */

/* 238U */
#define ENERGY238U 8.28293620554e-12      /**< [J] Decay energy of 238U */
#define MASS238U 238.050788268 * ATOMMASS /**< [kg] Mass of a 238U atom */
#define HALFLIFE238U 2.0359742e+17        /**< [s] Half-life of 238U */
/* Mantle */
#define EMASSMAN238U 1.13378e+17 /**< [kg]  Default mantle mass of 238U */
#define ENUMMAN238U                                                            \
  2.86829e+41 /**< [num] Default mantle number of 238U atoms */
#define EPOWERMAN238U                                                          \
  1.16691e+13 /**< [W] Default mantle power from 238U decay */
/* Core */
#define EMASSCORE238U 1.16964e+15 /**< [kg]  Default core mass of 238U */
#define ENUMCORE238U                                                           \
  2.95900e+39 /**< [num] Default core number of 238U atoms                     \
               */
#define EPOWERCORE238U                                                         \
  1.20381e+11 /**< [W] Default core power from 238U decay */
/* Crust (assuming crustal mass constant) */
#define EMASSCRUST238U 5.01582e+16 /**< [kg]  Default crust mass of 238U */
#define ENUMCRUST238U                                                          \
  1.26893e+41 /**< [num] Default crust number of 238U atoms */
#define EPOWERCRUST238U                                                        \
  5.16237e+12 /**< [W] Default crust power from 238U decay */

/* 235U */
#define HALFLIFE235U 3.2038174e+16      /**< [J] Decay energy of 235U */
#define MASS235U 235.0439299 * ATOMMASS /**< [kg] Mass of a 235U atom */
#define ENERGY235U 6.55516e-12          /**< [s] Half-life of 235U */
/* Mantle */
#define EMASSMAN235U 3.86253e+16 /**< [kg]  Default mantle mass of 235U */
#define ENUMMAN235U                                                            \
  9.89663e+40 /**< [num] Default mantle number of 235U atoms */
#define EPOWERMAN235U                                                          \
  2.02490e+13 /**< [W] Default mantle power from 235U decay */
/* Core */
#define EMASSCORE235U 9.56785e+14 /**< [kg]  Default core mass of 235U */
#define ENUMCORE235U                                                           \
  2.45149e+39 /**< [num] Default core number of 235U atoms                     \
               */
#define EPOWERCORE235U                                                         \
  5.01586e+11 /**< [W] Default core power from 235U decay */
/* Crust (assuming crustal mass constant) */
#define EMASSCRUST235U 1.98184e+16 /**< [kg]  Default crust mass of 235U */
#define ENUMCRUST235U                                                          \
  5.07789e+40 /**< [num] Default crust number of 235U atoms */
#define EPOWERCRUST235U                                                        \
  1.03896e+13 /**< [W] Default crust power from 235U decay */

/* Continental Crust Mass Earth today */
/* Note we don't define an oceanic crust reservoir (bc it's recycled and
 * therefore part of the convecting mantle) so "crust" really means "continental
 * crust". */
#define EPOWERCRUSTTOTAL                                                       \
  7e12 /**< [W] Earth's modern crustal radiogenic power, 7 TW (Jaupart et al   \
          2007) */
#define ECRUSTRADPOWDEN                                                        \
  (EPOWERCRUSTTOTAL) / (ECRUSTMASS) /**< [W/kg] Earth's modern crustal heat    \
                                       production density */

void fvInitializeControlRadheat(CONTROL *, int);
void fvInitializeBodyRadheat(BODY *, CONTROL *, UPDATE *, int, int);
void fvInitializeUpdateTmpBodyEqtide(BODY *, CONTROL *, UPDATE *, int);

void fvAddModuleRadheat(CONTROL *, MODULE *, int, int);
void fvBodyCopyRadheat(BODY *, BODY *, int, int, int);

/* Options Info */
#define OPTSTARTRADHEAT 1100 /* Start of RADHEAT options */
#define OPTENDRADHEAT 1200   /* End of RADHEAT options */

#define OPT_26ALMASSMAN 1110
#define OPT_26ALMASSCORE 1111
#define OPT_26ALNUMMAN 1112
#define OPT_26ALNUMCORE 1113
#define OPT_26ALPOWERMAN 1114
#define OPT_26ALPOWERCORE 1115

#define OPT_40KMASSMAN 1120
#define OPT_40KMASSCORE 1121
#define OPT_40KNUMMAN 1122
#define OPT_40KNUMCORE 1123
#define OPT_40KPOWERMAN 1124
#define OPT_40KPOWERCORE 1125
#define OPT_40KMASSCRUST 1126
#define OPT_40KNUMCRUST 1127
#define OPT_40KPOWERCRUST 1128

// Not currently coded
#define OPT_87RBMASS 1130
#define OPT_87RBNUM 1131
#define OPT_87RBPOWER 1132

#define OPT_232THMASSMAN 1135
#define OPT_232THMASSCORE 1136
#define OPT_232THNUMMAN 1137
#define OPT_232THNUMCORE 1138
#define OPT_232THPOWERMAN 1139
#define OPT_232THPOWERCORE 1140
#define OPT_232THMASSCRUST 1141
#define OPT_232THNUMCRUST 1142
#define OPT_232THPOWERCRUST 1143

#define OPT_235UMASSMAN 1145
#define OPT_235UMASSCORE 1146
#define OPT_235UNUMMAN 1147
#define OPT_235UNUMCORE 1148
#define OPT_235UPOWERMAN 1149
#define OPT_235UPOWERCORE 1150
#define OPT_235UMASSCRUST 1151
#define OPT_235UNUMCRUST 1152
#define OPT_235UPOWERCRUST 1153

#define OPT_238UMASSMAN 1155
#define OPT_238UMASSCORE 1156
#define OPT_238UNUMMAN 1157
#define OPT_238UNUMCORE 1158
#define OPT_238UPOWERMAN 1159
#define OPT_238UPOWERCORE 1160
#define OPT_238UMASSCRUST 1161
#define OPT_238UNUMCRUST 1162
#define OPT_238UPOWERCRUST 1163

#define OPT_HALT40KPOWER 1190
#define OPT_HALTMIN87RBPOWER 1191
#define OPT_HALT232THPOWER 1192
#define OPT_HALT238UPOWER 1193
#define OPT_HALT235UPOWER 1194
#define OPT_HALTRADPOWER 1195

/* Options Functions */
void fvHelpOptionsRadheat(OPTIONS *);
void fvRead40KPowerMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead40KMassMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead40KNumMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead26AlPowerMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead26AlMassMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead26AlNumMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead232ThPowerMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead232ThMassMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead232ThNumMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead238UPowerMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead238UMassMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead238UNumMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead235UPowerMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead235UMassMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead235UNumMan(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);

void fvRead26AlPowerCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead26AlMassCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead26AlNumCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead40KPowerCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead40KMassCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead40KNumCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead232ThPowerCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead232ThMassCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead232ThNumCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead238UPowerCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead238UMassCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead238UNumCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead235UPowerCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead235UMassCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead235UNumCore(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);

void fvRead40KPowerCrust(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead40KMassCrust(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead40KNumCrust(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead232ThPowerCrust(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                           int);
void fvRead232ThMassCrust(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead232ThNumCrust(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead238UPowerCrust(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead238UMassCrust(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead238UNumCrust(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead235UPowerCrust(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead235UMassCrust(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void fvRead235UNumCrust(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);

void fvInitializeOptionsRadheat(OPTIONS *, fnReadOption[]);
void fvReadOptionsRadheat(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                          fnReadOption[], int);

int fbHaltMin40KPower(BODY *, EVOLVE *, HALT *, IO *, UPDATE *,
                      fnUpdateVariable ***, int);
int fbHaltMin232ThPower(BODY *, EVOLVE *, HALT *, IO *, UPDATE *,
                        fnUpdateVariable ***, int);
int fbHaltMin238UPower(BODY *, EVOLVE *, HALT *, IO *, UPDATE *,
                       fnUpdateVariable ***, int);
// void CountHaltsRadheat(HALT*,int*);

/* Verify Functions */
void fvNotMassAndNum(OPTIONS *, int, int, int);
void fvAssign26AlNum(BODY *, OPTIONS *, double, int);
void fvAssign40KNum(BODY *, OPTIONS *, double, int);
void fvAssign232ThNum(BODY *, OPTIONS *, double, int);
void fvAssign238UNum(BODY *, OPTIONS *, double, int);
void fvAssign235UNum(BODY *, OPTIONS *, double, int);
void fvVerify26Al(BODY *, OPTIONS *, SYSTEM *, UPDATE *, double, int);
void fvVerify40K(BODY *, OPTIONS *, SYSTEM *, UPDATE *, double, int);
void fvVerify232Th(BODY *, OPTIONS *, SYSTEM *, UPDATE *, double, int);
void fvVerify238U(BODY *, OPTIONS *, SYSTEM *, UPDATE *, double, int);
void fvVerify235U(BODY *, OPTIONS *, SYSTEM *, UPDATE *, double, int);

void fvPropsAuxRadheat(BODY *, EVOLVE *, IO *, UPDATE *, int);
void fvForceBehaviorRadheat(BODY *, MODULE *, EVOLVE *, IO *, SYSTEM *,
                            UPDATE *, fnUpdateVariable ***, int, int);
void fvVerifyRadheat(BODY *, CONTROL *, FILES *, OPTIONS *, OUTPUT *, SYSTEM *,
                     UPDATE *, int, int);
void fvAssignRadheatDerivatives(BODY *, EVOLVE *, UPDATE *,
                                fnUpdateVariable ***, int);
void fvNullRadheatDerivatives(BODY *, EVOLVE *, UPDATE *, fnUpdateVariable ***,
                              int);

/* Update functions */
void fvInitializeUpdateRadheat(BODY *, UPDATE *, int);
void fvFinalizeUpdate26AlNumManRadheat(BODY *, UPDATE *, int *, int, int, int);
void fvFinalizeUpdate40KNumManRadheat(BODY *, UPDATE *, int *, int, int, int);
void fvFinalizeUpdate232ThNumManRadheat(BODY *, UPDATE *, int *, int, int, int);
void fvFinalizeUpdate238UNumManRadheat(BODY *, UPDATE *, int *, int, int, int);
void fvFinalizeUpdate235UNumManRadheat(BODY *, UPDATE *, int *, int, int, int);
void fvFinalizeUpdate26AlNumCoreRadheat(BODY *, UPDATE *, int *, int, int, int);
void fvFinalizeUpdate40KNumCoreRadheat(BODY *, UPDATE *, int *, int, int, int);
void fvFinalizeUpdate232ThNumCoreRadheat(BODY *, UPDATE *, int *, int, int,
                                         int);
void fvFinalizeUpdate238UNumCoreRadheat(BODY *, UPDATE *, int *, int, int, int);
void fvFinalizeUpdate235UNumCoreRadheat(BODY *, UPDATE *, int *, int, int, int);
void fvFinalizeUpdate26AlNumCrustRadheat(BODY *, UPDATE *, int *, int, int,
                                         int);
void fvFinalizeUpdate40KNumCrustRadheat(BODY *, UPDATE *, int *, int, int, int);
void fvFinalizeUpdate232ThNumCrustRadheat(BODY *, UPDATE *, int *, int, int,
                                          int);
void fvFinalizeUpdate238UNumCrustRadheat(BODY *, UPDATE *, int *, int, int,
                                         int);
void fvFinalizeUpdate235UNumCrustRadheat(BODY *, UPDATE *, int *, int, int,
                                         int);

/* Halt Functions */
#define RADHEATHALTSYSEND 5
#define RADHEATHALTBODYEND 5
// XXX Need to add 26Al
void fvReadHaltMin40KPower(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                           int);
void fvReadHaltMin232ThPower(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                             int);
void fvReadHaltMin238UPower(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                            int);
void fvReadHaltMin235UPower(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                            int);
void fvReadHaltMinRadPower(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                           int);
void fvCountHaltsRadheat(HALT *, int *);
void fvVerifyHaltRadheat(BODY *, CONTROL *, OPTIONS *, int, int *);

/* Output Functions */

/* RADHEAT 1100 - 1199 */
#define OUTSTARTRADHEAT 1100
#define OUTENDRADHEAT 1200

/* Body Properties due to radiogenic heating */
#define OUT_26ALPOWERMAN 1110
#define OUT_26ALPOWERCORE 1111
#define OUT_26ALENFLUX 1112
#define OUT_26ALDPOWERDT 1113
#define OUT_26ALDNUMDT 1114
#define OUT_26ALPOWERTIME 1115
#define OUT_26ALMASSMAN 1116
#define OUT_26ALMASSCORE 1117
#define OUT_26ALNUMMAN 1118
#define OUT_26ALNUMCORE 1119
#define OUT_26ALTIME 1120
#define OUT_26ALPOWERTOT 1121

#define OUT_40KPOWERMAN 1125
#define OUT_40KPOWERCORE 1126
#define OUT_40KENFLUX 1127
#define OUT_40KDPOWERDT 1128
#define OUT_40KDNUMDT 1129
#define OUT_40KPOWERTIME 1130
#define OUT_40KMASSMAN 1131
#define OUT_40KMASSCORE 1132
#define OUT_40KNUMMAN 1133
#define OUT_40KNUMCORE 1134
#define OUT_40KTIME 1135
#define OUT_40KPOWERCRUST 1136
#define OUT_40KMASSCRUST 1137
#define OUT_40KNUMCRUST 1138
#define OUT_40KPOWERTOT 1139

#define OUT_232THPOWERMAN 1140
#define OUT_232THPOWERCORE 1141
#define OUT_232THENFLUX 1142
#define OUT_232THPOWERDT 1143
#define OUT_232THDNUMDT 1144
#define OUT_232THPOWERTIME 1145
#define OUT_232THMASSMAN 1146
#define OUT_232THMASSCORE 1147
#define OUT_232THNUMMAN 1148
#define OUT_232THNUMCORE 1149
#define OUT_232THTIME 1150
#define OUT_232THPOWERCRUST 1151
#define OUT_232THMASSCRUST 1152
#define OUT_232THNUMCRUST 1153
#define OUT_232THPOWERTOT 1154

#define OUT_238UPOWERMAN 1155
#define OUT_238UPOWERCORE 1156
#define OUT_238UENFLUX 1157
#define OUT_D238UPOWERDT 1158
#define OUT_238UDNUMDT 1159
#define OUT_238UPOWERTIME 1160
#define OUT_238UMASSMAN 1161
#define OUT_238UMASSCORE 1162
#define OUT_238UNUMMAN 1163
#define OUT_238UNUMCORE 1164
#define OUT_238UTIME 1165
#define OUT_238UPOWERCRUST 1166
#define OUT_238UMASSCRUST 1167
#define OUT_238UNUMCRUST 1168
#define OUT_238UPOWERTOT 1169

#define OUT_235UPOWERMAN 1170
#define OUT_235UPOWERCORE 1171
#define OUT_235UENFLUX 1172
#define OUT_D235UPOWERDT 1174
#define OUT_235UDNUMDT 1175
#define OUT_235UPOWERTIME 1176
#define OUT_235UMASSMAN 1178
#define OUT_235UMASSCORE 1179
#define OUT_235UNUMMAN 1180
#define OUT_235UNUMCORE 1181
#define OUT_235UTIME 1185
#define OUT_235UPOWERCRUST 1186
#define OUT_235UMASSCRUST 1187
#define OUT_235UNUMCRUST 1188
#define OUT_235UPOWERTOT 1189

#define OUT_RADPOWERMAN 1190
#define OUT_RADPOWERCORE 1191
#define OUT_RADPOWERCRUST 1192
#define OUT_RADPOWERTOTAL 1193
#define OUT_SURFENFLUXRADTOTAL 1195

void fvWrite26AlPowerMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite26AlEnFlux(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char[]);
void fvWriteD26AlPowerDt(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite26AlTimescale(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWrite26AlMassMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);
void fvWrite26AlNumMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char[]);
void fvWrite26AlPowerCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWrite26AlMassCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite26AlNumCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);

void fvWrite40KPowerMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);
void fvWrite40KEnFlux(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                      int, double *, char[]);
void fvWriteD40KPowerDt(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);
void fvWrite40KTimescale(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite40KMassMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char[]);
void fvWrite40KNumMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                      int, double *, char[]);
void fvWrite40KPowerCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite40KMassCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);
void fvWrite40KNumCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char[]);
void fvWrite40KPowerCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWrite40KMassCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite40KNumCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);

void fvWrite232ThPowerMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWrite232ThEnFlux(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);
void fvWriteD232ThPowerDt(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWrite232ThTimescale(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char[]);
void fvWrite232ThMassMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite232ThNumMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);
void fvWrite232ThPowerCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char[]);
void fvWrite232ThMassCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWrite232ThNumCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite232ThPowerCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                            UPDATE *, int, double *, char[]);
void fvWrite232ThMassCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char[]);
void fvWrite232ThNumCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);

void fvWrite238UPowerMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite238UEnFlux(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char[]);
void fvWriteD238UPowerDt(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite238UTimescale(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWrite238UMassMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);
void fvWrite238UNumMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char[]);
void fvWrite238UPowerCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWrite238UMassCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite238UNumCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);
void fvWrite238UPowerCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char[]);
void fvWrite238UMassCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWrite238UNumCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);

void fvWrite235UPowerMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite235UEnFlux(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char[]);
void fvWriteD235UPowerDt(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite235UTimescale(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWrite235UMassMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);
void fvWrite235UNumMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char[]);
void fvWrite235UPowerCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWrite235UMassCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWrite235UNumCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);
void fvWrite235UPowerCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                           UPDATE *, int, double *, char[]);
void fvWrite235UMassCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWrite235UNumCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);

void fvWriteRadPowerMan(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                        UPDATE *, int, double *, char[]);
void fvWriteRadPowerCore(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void fvWriteRadPowerCrust(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWriteRadPowerTotal(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                          UPDATE *, int, double *, char[]);
void fvWriteRadEnFlux(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                      int, double *, char[]);

void fvHelpOutputRadheat(OUTPUT *);
void fvInitializeOutputRadheat(OUTPUT *, fnWriteOutput[]);

/* Logging Functions */
void fvLogOptionsRadheat(CONTROL *, FILE *);
void fvLogRadheat(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                  fnWriteOutput[], FILE *);
void fvLogBodyRadheat(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                      fnWriteOutput[], FILE *, int);

/* RadHeat functions */
double fd26AlConstant(double, double);
double fd40KConstant(double, double);
double fd232ThConstant(double, double);
double fd238UConstant(double, double);
double fd235UConstant(double, double);

double fd26AlPowerMan(UPDATE *, int);
double fd40KPowerMan(UPDATE *, int);
double fd232ThPowerMan(UPDATE *, int);
double fd238UPowerMan(UPDATE *, int);
double fd235UPowerMan(UPDATE *, int);

double fdD26AlNumManDt(BODY *, SYSTEM *, int *);
double fdD40KNumManDt(BODY *, SYSTEM *, int *);
double fdD232ThNumManDt(BODY *, SYSTEM *, int *);
double fdD238UNumManDt(BODY *, SYSTEM *, int *);
double fdD235UNumManDt(BODY *, SYSTEM *, int *);

double fd26AlPowerCore(UPDATE *, int);
double fd40KPowerCore(UPDATE *, int);
double fd232ThPowerCore(UPDATE *, int);
double fd238UPowerCore(UPDATE *, int);
double fd235UPowerCore(UPDATE *, int);

double fd40KPowerCrust(UPDATE *, int);
double fd232ThPowerCrust(UPDATE *, int);
double fd238UPowerCrust(UPDATE *, int);
double fd235UPowerCrust(UPDATE *, int);

double fd26AlPower(UPDATE *, int);
double fd40KPower(UPDATE *, int);
double fd232ThPower(UPDATE *, int);
double fd235UPower(UPDATE *, int);
double fd238UPower(UPDATE *, int);

double fdRadPowerTotal(BODY *, int);
double fdRadPowerMan(UPDATE *, int);
double fdRadPowerCore(UPDATE *, int);
double fdRadPowerCrust(UPDATE *, int);

double fd26AlEnFlux(BODY *, UPDATE *, int);
double fd40KEnFlux(BODY *, UPDATE *, int);
double fd232ThEnFlux(BODY *, UPDATE *, int);
double fd235UEnFlux(BODY *, UPDATE *, int);
double fd238UEnFlux(BODY *, UPDATE *, int);
double fdSurfEnFluxRadTotal(BODY *, SYSTEM *, UPDATE *, int, int);

double fdD26AlNumCoreDt(BODY *, SYSTEM *, int *);
double fdD40KNumCoreDt(BODY *, SYSTEM *, int *);
double fdD232ThNumCoreDt(BODY *, SYSTEM *, int *);
double fdD238UNumCoreDt(BODY *, SYSTEM *, int *);
double fdD235UNumCoreDt(BODY *, SYSTEM *, int *);

double fdD40KNumCrustDt(BODY *, SYSTEM *, int *);
double fdD232ThNumCrustDt(BODY *, SYSTEM *, int *);
double fdD238UNumCrustDt(BODY *, SYSTEM *, int *);
double fdD235UNumCrustDt(BODY *, SYSTEM *, int *);

/* @endcond */
