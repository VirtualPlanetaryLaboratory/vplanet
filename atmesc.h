/***************** ATMESC.H *********************** 
 *
 * Rodrigo Luger, Tue Apr 28 11:04:00 PDT 2014
 *
 * This header file contains all the subroutines in
 * file atmesc.c.
 *
*/

#define RAD40K           10

void InitializeControlAtmEsc(CONTROL*);
void AddModuleAtmEsc(MODULE*,int,int);
void BodyCopyAtmEsc(BODY*,BODY*,int,int);
void InitializeBodyEqtide(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyAtmEsc(BODY*,CONTROL*,UPDATE*,int);


/* Options Info */

#define OPTSTARTATMESC         1200 /* Start of AtmEsc options */
#define OPTENDATMESC            1300 /* End of AtmEsc options */

#define OPT_40KMASS             1110
#define OPT_40KNUM              1112 
#define OPT_40KPOWER             1114 
#define OPT_87RBMASS            1120
#define OPT_87RBNUM             1122
#define OPT_87RBPOWER            1124
#define OPT_232THMASS	        1130
#define OPT_232THNUM            1132
#define OPT_232THPOWER           1134
#define OPT_238UMASS            1140
#define OPT_238UNUM             1142
#define OPT_238UPOWER            1144
#define OPT_HALTMIN40KPOWER      1170
#define OPT_HALTMIN87RBPOWER     1172
#define OPT_HALTMIN232THPOWER    1174
#define OPT_HALTMIN238UPOWER	1176

/* Options Functions */
void HelpOptionsAtmEsc(OPTIONS*);
void Read40KPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KMass(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void Read40KNum(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read232ThPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int); 
void Read232ThMass(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int); 
void Read232ThNum(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UMass(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void Read238UNum(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMin40KPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMin232ThPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadHaltMin238UPower(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void InitializeOptionsAtmEsc(OPTIONS*,fnReadOption[]);
void ReadOptionsAtmEsc(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption[],int);

/* Halt Functions */
#define AtmEscHALTSYSEND       5
#define AtmEscHALTBODYEND      5

int HaltMin40KPower(BODY*,CONTROL*,UPDATE*,int);
int HaltMin232ThPower(BODY*,CONTROL*,UPDATE*,int);
int HaltMin238UPower(BODY*,CONTROL*,UPDATE*,int);
void CountHaltsAtmEsc(HALT*,int*);


/* Verify Functions */

void VerifyAtmEsc(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
void VerifyRotationAtmEsc(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetAtmEsc(CONTROL*,MODULE*);
void PropertiesAtmEsc(BODY*,int);
void VerifyHaltAtmEsc(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationAtmEsc(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateAtmEsc(BODY*,UPDATE*,int);
void FinalizeUpdateEccAtmEsc(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateNumIsotopeAtmEsc(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateOblAtmEsc(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateRotAtmEsc(BODY*,UPDATE*,int*,int,int);
void FinalizeUpdateSemiAtmEsc(BODY*,UPDATE*,int*,int,int);

/* Output Functinos */

/* AtmEsc 1100 - 1199 */
#define OUTSTARTAtmEsc         1100
#define OUTENDAtmEsc           1200

/* Body Properties due to radiogenic heating */

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
#define OUT_238UENFLUX	1152
#define OUT_D238UPOWERDT	        1154
#define OUT_238UDNUMDT	        1155
#define OUT_238UPOWERIME	        1156
#define OUT_238UMASS	        1158
#define OUT_238UNUM             1160
#define OUT_238UTIME            1165

#define OUT_RADPOWER            1175
#define OUT_SURFENFLUXAtmEsc   1180

void HelpOutputAtmEsc(OUTPUT*);
void InitializeOutputAtmEsc(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionAtmEsc(OUTPUT*,int,int);
void FinalizeOutputFunctionAtmEsc(OUTPUT*,int,int);

void Write40KPower(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD40KPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KMass(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KNum(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void Write232ThPower(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD232ThPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThMass(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write232ThNum(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void Write238UPower(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteD238UPowerDt(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UTimescale(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write238UMass(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void Write40KNum(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

void WriteRadPower(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);
void WriteRadEnFlux(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);

/* Logging Functions */
void LogOptionsAtmEsc(CONTROL*,FILE*);
void LogAtmEsc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyAtmEsc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* AtmEsc functions */

/* Needed? 
void fdRadPower(BODY*,int);
void fd40KConstant(BODY*,int);
void fd232ThConstant(BODY*,int);
void fd238UConstant(BODY*,int);
*/

double fdSurfEnFluxAtmEsc(BODY*,SYSTEM*,UPDATE *update,int,int);
double fdInternalPowerAtmEsc(BODY*,SYSTEM*,UPDATE *update,int,int);
double fdRadPower(double,double,double); // XXX

void fnForceBehaviorAtmEsc(BODY*,EVOLVE*,IO*,int,int);
double fd40KConstant(double,double);
double fd232ThConstant(double,double);
double fd238UConstant(double,double);

double fd40KPower(BODY*,SYSTEM*,int*,int);
double fd232ThPower(BODY*,SYSTEM*,int);
double fd238UPower(BODY*,SYSTEM*,int);
double fdTotalRadPower(BODY*,int);


double fdD40KNumDt(BODY*,SYSTEM*,int*,int);
double fdD232ThNumDt(BODY*,SYSTEM*,int*,int);
double fdD238UNumDt(BODY*,SYSTEM*,int*,int);

