/**
  @file update.h
  @brief Subroutines controlling the integration
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date A long, long time ago
*/

/* @cond DOXYGEN_OVERRIDE */

/* Indices for variables in the update struct. These are the primary
   variables. */
#define VSEMI              1001 // Semi-major axis
#define VECC               1002 // Eccentricity
#define VROT               1003 // Rotational Frequency
#define VOBL               1004 // Obliquity
#define VRADIUS            1005 // Radius
#define VMASS              1006 // Mass
#define VRADGYRA           1007 // Radius of Gyration

// RADHEAT
#define VNUM26ALMAN        1100 // 26Al in Mantle
#define VNUM26ALCORE       1105 // 26Al in Core
#define VNUM40KMAN         1110 // 40K in Mantle
#define VNUM40KCORE        1115 // 40K in Core
#define VNUM232THMAN       1120 // 232Th in Mantle
#define VNUM232THCORE      1125 // 232Th in Core
#define VNUM235UMAN        1130 // 235U in Mantle
#define VNUM235UCORE       1135 // 235U in Core
#define VNUM238UMAN        1140 // 238U in Mantle
#define VNUM238UCORE       1145 // 238U in Core

// THERMINT
#define VTMAN              1201 // Mantle Temperature
#define VTCORE             1202 // Core Temperature

//DistOrb
#define VHECC              1301 // Poincare's h
#define VKECC              1302 // Poincare's k
#define VPINC              1303 // Poincare's p
#define VQINC              1304 // Poincare's q

//DISTROT
#define VXOBL              1401 // Detrick's X
#define VYOBL              1402 // Detrick's Y
#define VZOBL              1403 // Detrick's Z
#define VDYNELLIP          1404 // Dynamical Ellipticity

//SPINBODY 1600-1700
#define VVELX              1601 // Cartesian X Velocity
#define VVELY              1602 // Cartesian Y Velocity
#define VVELZ              1603 // Cartesian Z Velocity
#define VPOSITIONX         1604 // Cartesian X Position
#define VPOSITIONY         1605 // Cartesian Y Position
#define VPOSITIONZ         1606 // Cartesian Z Position

// ATMESC
#define VSURFACEWATERMASS  1202 // Surface Water Mass
#define VENVELOPEMASS      1203 // Envelope Mass
#define VOXYGENMASS        1204 // Atmospheric Oxygen Mass
#define VOXYGENMANTLEMASS  1205 // Mantle Oxygen Mass

// STELLAR
#define VLUMINOSITY        1502 // Luminosity
#define VTEMPERATURE       1503 // Temperature
#define VLOSTANGMOM        1504 // Lost Angular Momentum
#define VLOSTENG           1505 // Lost Energy

// POISE
#define VICEMASS           1851 // Ice Mass

// BINARY: 2000-2999, inclusive
// Primary variables that control CBP's cylindrical positions, velocities
#define VCBPR              2000
#define VCBPPHI            2010
#define VCBPZ              2020
#define VCBPRDOT           2030
#define VCBPPHIDOT         2040
#define VCBPZDOT           2050

// FLARE
#define VLXUV              1901 // XUV Luminosity from Flares

//GALHABIT
#define VECCX           2201
#define VECCY           2202
#define VECCZ           2203
#define VANGMX          2204
#define VANGMY          2205
#define VANGMZ          2206

//DISTRES
#define VMEANL          2301

//MAGMOC
#define VWATERMASSMOATM    2302
#define VWATERMASSSOL      2303
#define VSURFTEMP          2304
#define VPOTTEMP           2305
#define VSOLIDRADIUS       2306
#define VOXYGENMASSMOATM   2307
#define VOXYGENMASSSOL     2308
#define VHYDROGENMASSSPACE 2309
#define VOXYGENMASSSPACE   2310
#define VCO2MASSMOATM      2311
#define VCO2MASSSOL        2312

void InitializeUpdateBodyPerts(CONTROL*,UPDATE*,int);
void InitializeUpdateTmpBody(BODY*,CONTROL*,MODULE*,UPDATE*,int);
void UpdateCopy(UPDATE*,UPDATE*,int);
void InitializeUpdate(BODY*,CONTROL*,MODULE*,UPDATE*,fnUpdateVariable****);

/* @endcond */
