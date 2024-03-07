/**
   @file poise.h

   @brief Subroutines that control the energy balance model for climate

   @author Russell Deitrick ([deitrr](https://github.com/deitrr/))

   @date Sep 10 2015

*/

#include <stdio.h>
#include <stdlib.h>

/* Climate model */
#define ANN 0
#define SEA 1

/* OLR model */
#define WK97 0
#define HM16 1
#define SMS09 2

/* Water albedo type */
#define ALBFIXED 0
#define ALBTAYLOR 1

/* Land Geography */
#define UNIFORM3 0
#define MODERN 1

// Constants for the ice model
#define LFICE 3.34e5  // ???
#define RHOICE 916.7  // density of ice I
#define MOCEAN 1.4e21 // mass of earth ocean in kg (ref?)
#define a1ICE                                                                  \
  3.615e-13 // coeff of ice deformability at T<263K (Pa^-3 s^-1 - ref?)
#define a2ICE                                                                  \
  1.733e3         // coeff of ice deformability at T>=263K (Pa^-3 s^-1 - ref?)
#define Q1ICE 6e4 // energy in ice deformation at T<263K (J/mol)
#define Q2ICE 13.9e4 // energy in ice deformation at T>=263 (J/mol)

// Constant for the lithospheric model
#define nGLEN 3.0                  // Glen's law coefficient
#define RHOSED 2390                // sediment density from Huybers&Tziperman08
#define RHOH2O 1000                // Density of liquid water
#define SEDPHI (22.0 * PI / 180.0) // angle of internal friction (sediment)
#define SEDH 10                    // depth of sediment layer (m)
#define SEDD0 7.9e-7 // reference deformation rate for sediment (s^-1)
#define SEDMU 3e9    // reference viscosity for sediment (Pa s)
#define RHOBROCK 3370
#define BROCKTIME 5000 // relaxation timescale for bedrock

/* Options Info */
#define OPTSTARTPOISE 1900 /* Start of POISE options */
#define OPTENDPOISE 2000   /* End of POISE options */

#define OPT_LATCELLNUM 1901
#define OPT_PLANCKA 1902
#define OPT_PLANCKB 1903
#define OPT_DIFFUSION 1904
#define OPT_HEATCAPANN 1905
#define OPT_HADLEY 1906
#define OPT_COLDSTART 1907
#define OPT_FIXICELAT 1908
#define OPT_ALBEDOZA 1909
#define OPT_JORMUNGAND 1910
#define OPT_CALCAB 1911
#define OPT_TGLOBALEST 1912
#define OPT_PCO2 1913
#define OPT_MEPDIFF 1914
#define OPT_ICESHEETS 1915
#define OPT_INITICELAT 1916
#define OPT_INITICEHEIGHT 1917
#define OPT_ICEALBEDO 1918
#define OPT_SURFALBEDO 1919
#define OPT_ICEDEPRATE 1920
#define OPT_SKIPSEASENABLED 1921
#define OPT_DIFFROT 1922
#define OPT_SPINUPTOL 1923
#define OPT_READORBITOBLDATA  1924
#define OPT_FILEORBITOBLDATA  1925

#define OPT_LANDFRAC 1940
#define OPT_HEATCAPLAND 1942
#define OPT_HEATCAPWATER 1943
#define OPT_FRZTSEAICE 1944
//#define OPT_LATENTHEAT    1945
#define OPT_ICECONDUCT 1946
#define OPT_MIXINGDEPTH 1947
#define OPT_NULANDWATER 1948
#define OPT_NSTEPINYEAR 1949
#define OPT_NUMYEARS 1950
#define OPT_SEAICEMODEL 1951
#define OPT_ALBEDOLAND 1952
#define OPT_ALBEDOWATER 1953
#define OPT_ICEDT 1954
#define OPT_RERUNSEAS 1955
#define OPT_ALBEDOTYPE 1956
#define OPT_GEOGRAPHY 1957
#define OPT_SEASOUTPUTTIME 1958
#define OPT_FORCEOBLIQ 1959
#define OPT_OBLIQAMP 1960
#define OPT_OBLIQPER 1961
#define OPT_ACCUMODE 1962
#define OPT_ELEVFB 1963
#define OPT_LAPSER 1964
#define OPT_REFHEIGHT 1965
#define OPT_ABLATEFF 1966
#define OPT_FORCEECC 1967
#define OPT_ECCAMP 1968
#define OPT_ECCPER 1969
#define OPT_MINICEHEIGHT 1970

#define OPT_OLRMODEL 1998
#define OPT_CLIMATEMODEL 1999

/* POISE 1800 - 1899 */
/* System properties 1800-1819, body properties 1820-1899 */

#define OUTSTARTPOISE 1900     /* Start of POISE options */
#define OUTENDPOISE 2000       /* End of POISE options */
#define OUTBODYSTARTPOISE 1920 /* Start of POISE BODY options */

#define OUT_TGLOBAL 1921
#define OUT_ALBEDOGLOBAL 1922
#define OUT_FLUXINGLOBAL 1923
#define OUT_FLUXOUTGLOBAL 1924
#define OUT_TOTICEMASS 1925
#define OUT_TOTICEFLOW 1926
#define OUT_TOTICEBALANCE 1927
#define OUT_SKIPSEAS 1928
#define OUT_AREAICECOV 1929

#define OUT_LATITUDE 1930
#define OUT_TEMPLAT 1931
#define OUT_ALBEDOLAT 1932
#define OUT_ANNUALINSOL 1933
#define OUT_DAILYINSOL 1934
#define OUT_FLUXMERID 1935
#define OUT_FLUXIN 1936
#define OUT_FLUXOUT 1937
#define OUT_DIVFLUX 1938
#define OUT_ICEMASS 1939
#define OUT_ICEHEIGHT 1940
#define OUT_DICEMASSDT 1941
#define OUT_ICEFLOW 1942
#define OUT_ENERGYRESL 1943
#define OUT_ENERGYRESW 1944
#define OUT_BEDROCKH 1945
#define OUT_TEMPLANDLAT 1946
#define OUT_TEMPWATERLAT 1947
#define OUT_ALBEDOLANDLAT 1948
#define OUT_ALBEDOWATERLAT 1949
#define OUT_TEMPMINLAT 1950
#define OUT_TEMPMAXLAT 1951
#define OUT_SNOWBALL 1952
#define OUT_PLANCKBAVG 1953
#define OUT_ICEACCUM 1954
#define OUT_ICEABLATE 1955
#define OUT_TEMPMAXLAND 1956
#define OUT_TEMPMAXWATER 1957
#define OUT_PEAKINSOL 1958
#define OUT_NORTHICECAPLAND 1959
#define OUT_NORTHICECAPSEA 1960
#define OUT_SOUTHICECAPLAND 1961
#define OUT_SOUTHICECAPSEA 1962
#define OUT_ICEBELTLAND 1963
#define OUT_ICEBELTSEA 1964
#define OUT_SNOWBALLLAND 1965
#define OUT_SNOWBALLSEA 1966
#define OUT_ICEFREE 1967
#define OUT_NORTHICECAPLATLAND 1968
#define OUT_NORTHICECAPLATSEA 1969
#define OUT_SOUTHICECAPLATLAND 1970
#define OUT_SOUTHICECAPLATSEA 1971
#define OUT_NORTHICEBELTLATLAND 1972
#define OUT_NORTHICEBELTLATSEA 1973
#define OUT_SOUTHICEBELTLATLAND 1974
#define OUT_SOUTHICEBELTLATSEA 1975

/* @cond DOXYGEN_OVERRIDE */

void AddModulePoise(CONTROL *, MODULE *, int, int);
void BodyCopyPoise(BODY *, BODY *, int, int, int);
void InitializeUpdateTmpBodyPoise(BODY *, CONTROL *, UPDATE *, int);

/* Options Functions */
void HelpOptionsPoise(OPTIONS *);
void InitializeOptionsPoise(OPTIONS *, fnReadOption[]);
void ReadOptionsPoise(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                      fnReadOption[], int);
void ReadOrbitOblData(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);

/* Verify Functions */
void VerifyPoise(BODY *, CONTROL *, FILES *, OPTIONS *, OUTPUT *, SYSTEM *,
                 UPDATE *, int, int);

/* Update Functions */

void InitializeUpdatePoise(BODY *, UPDATE *, int);

/* Output Functinos */

void InitializeOptionsPoise(OPTIONS *, fnReadOption[]);
void FinalizeUpdateIceMassPoise(BODY *, UPDATE *, int *, int, int, int);

// int HaltMinIceDt(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);

void HelpOutputPoise(OUTPUT *);
void WriteTGlobal(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
                  double *, char[]);
void WriteAlbedoGlobal(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char[]);
void WriteTempLat(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
                  double *, char[]);
void WriteTempMinLW(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                    int, double *, char[]);
void WriteTempMaxLW(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                    int, double *, char[]);
void WriteAlbedoLat(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                    int, double *, char[]);
void WriteAnnualInsol(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                      int, double *, char[]);
void WriteDailyInsol(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                     int, double *, char[]);
void WritePlanckB(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
                  double *, char[]);
void WritePlanckBAvg(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                     int, double *, char[]);
void WriteSeasonalTemp(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                       int, double *, char[]);
void WriteSeasonalFluxes(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                         UPDATE *, int, double *, char[]);
void WriteSeasonalIceBalance(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                             UPDATE *, int, double *, char[]);
void WriteFluxMerid(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                    int, double *, char[]);
void WriteFluxIn(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
                 double *, char[]);
void WriteFluxOut(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
                  double *, char[]);
void WriteDivFlux(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
                  double *, char[]);
void InitializeOutputPoise(OUTPUT *, fnWriteOutput[]);

/* Logging Functions */
void LogOptionsPoise(CONTROL *, FILE *);
void LogPoise(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *, fnWriteOutput[],
              FILE *);
void LogBodyPoise(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                  fnWriteOutput[], FILE *, int);

/* Poise Functions */
void PropsAuxPoise(BODY *, EVOLVE *, IO *, UPDATE *, int);
void ForceBehaviorPoise(BODY *, MODULE *, EVOLVE *, IO *, SYSTEM *, UPDATE *,
                        fnUpdateVariable ***, int, int);
void fvAlbedoAnnual(BODY *, int);
void fvAlbedoSeasonal(BODY *, int, int);
void fvAnnualInsolation(BODY *, int);
double fdOLRdTwk97(BODY *, int, int, int);
double fdOLRwk97(BODY *, int, int, int);
double fdOLRdThm16(BODY *, int, int, int);
double fdOLRhm16(BODY *, int, int, int);
double fdOLRdTsms09(BODY *, int, int, int);
double fdOLRsms09(BODY *, int, int, int);
void fvAreaIceCovered(BODY *, int);

void PoiseAnnual(BODY *, int);
void PoiseSeasonal(BODY *, int);
void PoiseIceSheets(BODY *, EVOLVE *, int);
void fvSeaIce(BODY *, int);
void fvMatrixSeasonal(BODY *, int);
void fvMatrixInvertSeasonal(BODY *, int);
void fvTempGradientAnn(BODY *, double, int);
void fvTempGradientSea(BODY *, double, int);
void fvMatrixAnnual(BODY *, int);

void fvSourceFSeas(BODY *, int, int);
void fvSnowball(BODY *, int); // XXX Should change to int fbSnowball
int fbSnowballLand(BODY *, int);
int fbSnowballSea(BODY *, int);
int fbIceFree(BODY *, int);
void fvNorthIceCapLand(BODY *, int, double *, int *, int *);
void fvNorthIceCapSea(BODY *, int, double *, int *, int *);
void fvSouthIceCapLand(BODY *, int, double *, int *, int *);
void fvSouthIceCapSea(BODY *, int, double *, int *, int *);
void fvIceBeltLand(BODY *, int, double *, double *, int *, int *, int *);
void fvIceBeltSea(BODY *, int, double *, double *, int *, int *, int *);
void fvPrecessionExplicit(BODY *, EVOLVE *, int);
void fvPropsAuxPoise(BODY *, EVOLVE *, IO *, UPDATE *, int);
void fvForceObliq(BODY *, EVOLVE *, int);
void fvForceEcc(BODY *, EVOLVE *, int);
void fvDailyInsolation(BODY *, int, int);
void fvAlbedoTOAhm16(BODY *, double, int, int);


double fdIceMassBalance(BODY *, int, int);

double fdPoiseDIceMassDtDepMelt(BODY *, SYSTEM *, int *);
double fdPoiseDIceMassDtFlow(BODY *, SYSTEM *, int *);

double fdEccTrueAnomaly(double, double);
double fdAlbedoTOA350(double, double, double, double);

/* @endcond */
