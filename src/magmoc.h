/***************** MAGMOC.H ***********************
 *
 * Patrick Barth, THursday May 17 14:41 PDT 2018
 *
 * This header file contains all the subroutines in
 * file magmoc.c.
 *
*/


/* Parameter */
#define SILICATEHEATCAP           1.2e3  //silicate heat capacity (J/kg/K)
#define WATERHEATCAP              2.08e3 //water heat capacity (J/kg/K)
#define HEATFUSIONSILICATE        4e5    //heat of fusion of soliciates (J/kg)
#define THERMALCONDUC             4.2    //thermal conductivity mantle (W/m/K)
#define THERMALDIFFUS             1e-6   //thermal diffusivity mantle (m^2/s)
#define CRITRAYLEIGHNO            1.1e3  //critical Rayleigh number
#define THERMALEXPANCOEFF         2e-5   //thermal expansion coefficient (1/K)
#define ALBEDOWATERATMOS          0.75   //albedo 100% water atmosphere
#define ALBEDOROCK                0.3    //albedo bare rock
#define WATERPARTCOEFF            0.01   //partition coeff. water between melt & solid
#define MOLWEIGHTWATER            18.01528 //molar weight water
#define MOLWEIGHTHYDROGEN         1.00794  //molar weight hydrogen
#define MOLWEIGHTOXYGEN           15.999   //molar weight oxygen
#define MOLWEIGHTFEO              79.844   //molar weight FeO_1.5
#define CRITMELTFRAC              0.4    //critical melt fraction
#define DYNVISCLIQUID             0.01   //dynamic viscosity of liquid melt
#define DYNVISCSOLID              3.8e9  //dynamic viscosity of solid
#define IDEALGASCONST             8.314  //ideal gas constant
#define ACTIVENERGY               3.5e5  //activation energy viscosity
#define RADCOREEARTH              3.4e6  //core radius Earth (m)

// prefactors for linear solidus (Hirschmann 2000)
#define ALOWPRESSURE              1.0442e-7 //low pressure (K/Pa)
#define BLOWPRESSURE              1420      // (K)
#define AHIGHPRESSURE             0.2653E-7 //high pressure (K/Pa)
#define BHIGHPRESSURE             1825      // (K)
// #define PREFACTORA                0.2653E-7
// #define PREFACTORB                1825

void InitializeOptionsMagmOc(OPTIONS*,fnReadOption[]);
void AddModuleMagmOc(MODULE*,int,int);

/* Options Info */

#define OPTSTARTMAGMOC            2300 /* Start of Magmoc options */
#define OPTENDMAGMOC              2400 /* End of Magmoc options */

#define OPT_FEO                   2310
#define OPT_WATERMASSATM          2311
#define OPT_SURFTEMP              2312
#define OPT_MANMELTDENSITY        2313


/* Options Functions */
void HelpOptionsMagmOc(OPTIONS*);
void ReadFeO(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int) ;
void ReadWaterMassAtm(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadSurfTemp(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
void ReadManMeltDensity(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);

/* Halt Functions */
#define MAGMOCHALTSYSEND       5
#define MAGMOCHALTBODYEND      5

/* Verify Functions */
void VerifyMagmOc(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,int,int);
void VerifyRotationMagmOc(BODY*,CONTROL*,OPTIONS*,char[],int);
void InitializeVplanetMagmOc(CONTROL*,MODULE*);
void PropsAuxMagmOc(BODY*,EVOLVE*,UPDATE*,int);
void VerifyHaltMagmOc(BODY*,CONTROL*,OPTIONS*,int,int*);
void VerifyRotationMagmOc(BODY*,CONTROL*,OPTIONS*,char[],int);

/* Update functions */
void InitializeUpdateMagmOc(BODY*,UPDATE*,int);
void FinalizeUpdateEccMagmOc(BODY*,UPDATE*,int*,int,int,int);

/* Output Functions */

/* MagmOc 1100 - 1199 */
#define OUTSTARTMAGMOC         2300
#define OUTENDMAGMOC           2400

#define OUT_FE2O3              2310
#define OUT_WATERMASSMOATM     2311
#define OUT_POTTEMP            2312
#define OUT_SURFTEMP           2313
#define OUT_WATERMASSSOL       2314


void HelpOutputMagmOc(OUTPUT*);
void InitializeOutputMagmOc(OUTPUT*,fnWriteOutput[]);
void InitializeOutputFunctionMagmOc(OUTPUT*,int,int);
void FinalizeOutputFunctionMagmOc(OUTPUT*,int,int);

// void WriteFe2O3(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double*,char[]);


/* Logging Functions */
void LogOptionsMagmOc(CONTROL*,FILE*);
void LogMagmOc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyMagmOc(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* MagmOc functions */
void fnForceBehaviorMagmOc(BODY*,MODULE*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable ***fnUpdate,int,int);
double fdDWaterMassMOAtm(BODY*, CONTROL*, SYSTEM*, int*);
