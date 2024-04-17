/**
  @file vplanet.h
  @brief The main entry point for the code. All the magic starts here.
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date May 7 2014
*/


#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#ifdef __x86_64__
#include <xmmintrin.h>
#endif

// Windows-specific
#ifdef VPLANET_ON_WINDOWS
#define unlink _unlink
#else
#include <unistd.h>
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*! Top-level declarations */

/* Implemented Moduules
  The number is a bit value that can be used to uniquely identify the modules
  that have been applied to a specific body. The value is stored in
  module.iModuleBitSum.
 */
#define EQTIDE 2
#define RADHEAT 4
#define ATMESC 8
#define DISTORB 16
#define DISTROT 32
#define STELLAR 64
#define THERMINT 128
#define POISE 256
#define FLARE 512
#define BINARY 1024
#define GALHABIT 2048
#define SPINBODY 4096
#define DISTRES 8192
#define MAGMOC 16384

/* Fundamental constants; Some of these are taken from the IAU working
 group on Fundamental constants, as described in Prsa et al. 2016. */
/* Units: Calculations are done in SI */
#define BIGG                                                                   \
  6.67428e-11                // From Luzum et al., 2011; value recommended     \
                             // by IAU NSFA in Prsa et al. 2016
#define PI M_PI              // Quick fix
#define KGAUSS 0.01720209895 // Gauss' Gravitational Constamt
#define EPS 1e-10 // Precision for difference of doubles to be effectively 0
#define AUM 1.49597870700e11   // Exact m/AU per 31 AUG 2012 IAU resolution B2
#define AUPC 206265.0          // AU per parsec
#define LIGHTSPEED 299792458.0 // Speed of light
#define MEARTH 5.972186e24     // Earth's mass; Prsa et al. 2016
#define MSUN 1.988416e30       // Sun's mass; Prsa et al. 2016
#define RSUN 6.957e8           // Sun's radius; Prsa et al. 2016
#define YEARSEC 3.15576e7      // Seconds per year
#define DAYSEC 86400           // Seconds per day
#define REARTH 6.3781e6        // Earth's Equatorial Radius; Prsa et al. 2016
#define RJUP 7.1492e7          // Jupiter's Equatorial Radius; Prsa et al. 2016
#define RNEP 2.4764e7          // Neptune's Radius
#define MNEP 1.0244e26         // Neptune's Mass
#define RHOEARTH 5515          // Earth's Density
#define eEARTH 0.016710219     // Earth's Eccentricity
#define MJUP 1.898130e27       // Jupiter's mass; Prsa et al. 2016
#define YEARDAY 365.25         // Days per year
#define MSAT 5.6851e26         // Saturns' Mass
#define DEGRAD 0.017453292519444445 // Degrees per radian
#define TOMASS 1.39e21              // Mass of one terrestrial ocean in kg (TO)
#define ATOMMASS 1.660538921e-27    // Atomic Mass
#define OXYMASS 2.6568622736e-26    // Mass of Atomic Oxygen 16*ATOMMASS
#define PROTONMASS 1.6726219e-27    // Proton mass kg
#define SIGMA 5.670367e-8           // Stefan-Boltzmann Constant
#define RGAS 8.3144598              // gas constant in J K^-1 mol^-1
#define KBOLTZ 1.38064852e-23       // Boltzmann constant, J/K
#define ALPHA_STRUCT                                                           \
  0.6 // Structural constant for spherical mass                              \
        //distribution potential energy (E_pot = -ALPHA*BIGG*M^2/R)

// Angle unit IDs
#define U_RADIANS 0
#define U_DEGREES 1

// Length unit IDs
#define U_METER 0
#define U_CENTIMETER 1
#define U_KILOMETER 2
#define U_SOLARRADIUS 3
#define U_EARTHRADIUS 4
#define U_JUPRADIUS 5
#define U_AU 6

// Mass unit IDs
#define U_GRAM 0
#define U_KILOGRAM 1
#define U_SOLARMASS 2
#define U_EARTHMASS 3
#define U_JUPITERMASS 4
#define U_NEPTUNEMASS 5

// Temperature unit IDs
#define U_KELVIN 0
#define U_CELSIUS 1
#define U_FARENHEIT 2

// Time unit IDs
#define U_SECOND 0
#define U_DAY 1
#define U_YEAR 2
#define U_MYR 3
#define U_GYR 4

/* Do not change these declarations */
extern const double dHUGE;
extern const double dTINY;
/* Do not change these declarations */

// IO limits for files, lines, and names
#define OPTLEN 48        /* Maximum length of an option */
#define OPTDESCR 128     /* Number of characters in option description */
#define OPTLONDESCR 2048 /* Number of characters in option long description */
/* !!! Hack to get long description and module list formatting for Long Help.
   It'd be better to keep LINE back at 256 for memory, and malloc the char's,
   but I don't know how to do that. */
#define LINE 2048 /* Maximum number of characters in a line */
#define NAMELEN 100
#define MAXFILES 128 /* Maximum number of input files */
#define MAXARRAY                                                               \
  128 /* Maximum number of options in                                          \
       * an option array */
#define NUMOPT                                                                 \
  1000 /* Number of options that could be                                      \
        * in MODULE */
#define MAXLINES                                                               \
  256 /* Maximum Number of Lines in an                                         \
       * input file */
#define OUTLEN                                                                 \
  48 /* Maximum number of characters in an output column header                \
      */
#define OUTDESCR 256     /* Number of characters in output description */
#define OUTLONDESCR 2048 /* Number of characters in output long description */


/* Forward declaration of structs.
This is necessary in order to add pointers to structs into typedef'd functions
*/

typedef struct BODY BODY;
typedef struct CONTROL CONTROL;
typedef struct EVOLVE EVOLVE;
typedef struct FILES FILES;
typedef struct HALT HALT;
typedef struct INFILE INFILE;
typedef struct IO IO;
typedef struct MODULE MODULE;
typedef struct OPTIONS OPTIONS;
typedef struct OUTFILE OUTFILE;
typedef struct OUTPUT OUTPUT;
typedef struct SYSTEM SYSTEM;
typedef struct UNITS UNITS;
typedef struct UPDATE UPDATE;
typedef struct VERIFY VERIFY;

/*! \brief BODY contains all the physical parameters for every object in the
 * system.
 */
struct BODY {
  /* Body Properties */
  char cName[NAMELEN]; /**< Body's Name */
  char sColor[OPTLEN]; /**< Body color (for plotting) */

  int bMantle; /**< Is there a mantle? */
  int bOcean;  /**< Is there an ocean? */
  int bEnv;    /**< Is there an envelope? */

  int iBodyType; /**< Type of object: 0=star, 1=rocky planet, 2 = giant */

  double dAge;       /**< Body's Age */
  double dMass;      /**< Body's Mass */
  double dSolidMass; /**< Mass of a body's solid component */
  double dRadius;    /**< Radius of body */
  double dDensity;   /**< Bulk density of body*/
  double dGravAccel; /**< Body's gravitational acceleration */
  double dK2;        /**< Body's Total Love number */
  double dImK2;      /**< Imaginary part of Love's k_2 (total) */
  double dObliquity; /**< Body's Obliquity */
  double dCosObl;    /**< Cosine of body's obliquity */
  double dRotRate;   /**< Body's Rotation Rate */
  double dRotPer;    /**< Body's Rotation Period */
  double dRotVel;    /**< Body's Rotational Velocity */
  double dRadGyra;   /**< Body's Radius of Gyration */

  /* Orbital Properties. By convention, these are stored in the
   * second element in the BODY array and, if using binary
   * in the secondary (1st (0, 1, ..)) body*/
  double dSemi;       /**< Body's Semi-major Axis */
  double dEcc;        /**< Body's Eccentricity */
  double dMeanMotion; /**< Body's Mean Motion */
  double dOrbPeriod;  /**< Body's Orbital Period */
  double dEccSq;      /**< Eccentricity squared */

  /* ATMESC Parameters */
  int bAtmEsc;        /**< Apply Module ATMESC? */
  int bInstantO2Sink; /**< Is oxygen absorbed instantaneously at the surface? */
  int bRunaway;       /**< Is the planet experiencing a runaway greenhouse? */
  int bCalcFXUV;      /**< Does incidenx XUV flow need to be calculated every
                              time step? */
  int bEnvelopeLostMessage; /**< Has the envelope lost message been printed? */
  int bIgnoreRocheLobe;     /**< Ignore Roche lobe overflow? */
  int bUseEnergyLimited;    /**< Use energy-limited escape */
  int bUseBondiLimited;     /**< Use Bondi-limited H mass loss */
  int bUseRRLimited;  /**< Use radiation/recombination-limited H mass loss */
  int bAtmEscAuto;    /**< Transition H escape regime depending on physics */
  int bAutoThermTemp; /**< Calculate thermal temperature from environemnt? */
  int bStopWaterLossInHZ; /**< Stop water loss once planet enters habitable
                             zone? */

  int iWaterLossModel;     /**< Water Loss and Oxygen Buildup Model */
  int iAtmXAbsEffH2OModel; /**< Water X-ray/XUV absorption efficiency evolution
                              model */
  int iPlanetRadiusModel;  /**< Planet Radius model. */
  int iWaterEscapeRegime;  /**< Track water escape regime */
  int iHEscapeRegime;      /**< Tracks H escape regime */

  double dSurfaceWaterMass;    /**< Surface water mass */
  double dMinSurfaceWaterMass; /**< Minimum surface water to avoid a halt */
  double dOxygenMass;          /**< Atmospheric oxygen mass */
  double dOxygenMantleMass;    /**< Mass of oxygen in the mantle */
  double dEnvelopeMass;        /**< Envelope mass */
  double dMinEnvelopeMass;     /**< Minimum envelope mass to avoid a halt */
  double dXFrac;               /**< XUV radius in units of planet's radius */
  double dAtmXAbsEffH; /**< Effective XUV absorpation efficiency for hydrogen */
  double dAtmXAbsEffH2O; /**< Effective XUV absorpation efficiency for water */
  double dRGDuration;    /**< Duration of runaway greenhouse phase */
  double dKTide;         /**< Tidal enhancement factor for mass loss */
  double dMinKTide;      /**< Minimum allowed value for KTide */
  double dAtmEscXi;      /**< Ratio of Roche radius to XUV radius */
  double dMDotWater;     /**< Water mass loss rate */
  double dFHRef;         /**< Reference hydrogen escape value */
  double dOxygenEta;     /**< Factor for drag of oxygen by hydrogen */
  double dCrossoverMass; /**< Atomic mass at which drag begins */
  double dFHDiffLim;     /**< Diffusion-limited H escape rate */
  double dRadXUV;        /**< XUV Radius in Lehmer-Catling model */
  double dRadSolid;      /**< Solid planet radius in Lehmer-Catling model */
  double dPresSurf;      /**< Surface Pressure in Lehmer-Catling model */
  double dPresXUV;       /**< Pressure at XUV radius in Lehmer-Catling model */
  double dScaleHeight;   /**< Scale Height used in Lehmer-Catling model */
  double dThermTemp; /**< Thermosphere's temperature in Lehmer-Catling model */
  double dAtmGasConst; /**< Atmosphere's gas constant in Lehmer-Catling model */
  double dFXUV;        /**< XUV Flux at planet's atmosphere */
  double dJeansTime;   /**< Jeans timescale for atmospheric escape */
  double dFlowTemp;    /**< Temperature of the hydrodynamic flow */
  double dRocheRadius; /**< Radius of the Roche lobe */
  double dBondiRadius; /**< Bondi (Sonic) Radius */
  double dEnvMassDt;   /**< Time derivative of H envelope mass */

  /* BINARY parameters */
  int bBinary;       /**< Apply BINARY module? */
  double dR0;        /**< Guiding Radius,initially equal to dSemi */
  double dCBPR;      /**< CBP orbital radius */
  double dCBPZ;      /**< CBP height above/below the orbital plane */
  double dCBPPhi;    /**< CBP azimuthal angle in orbital plane */
  double dCBPRDot;   /**< CBP radial orbital velocity */
  double dCBPZDot;   /**< CBP z orbital velocity */
  double dCBPPhiDot; /**< CBP phi angular orbital velocity */
  double dFreeEcc;   /**< CBP's free eccentricity */
  double dFreeInc;   /**< CBP's free inclination, or binary's inclination */
  double dInc;       /**< Orbital inclication */
  double dLL13N0;    /**< CBP's Mean motion defined in LL13 eqn 12 */
  double
        dLL13K0; /**< CBP's radial epicyclic frequency defined in LL13 eqn 26 */
  double dLL13V0; /**< CBP's vertical epicyclic frequency defined in LL13 eqn 36
                   */
  double dLL13PhiAB; /**< Binary's initial mean anomaly */
  double dCBPM0;     /**< CBP's initial mean anomaly */
  double dCBPZeta;   /**< CBP's z oscillation angle (see LL13 eqn 35) */
  double dCBPPsi; /**< CBP's R, phi oscillation phase angle (see LL13 eqn 27) */

  /* SPINBODY parameters */
  int bSpiNBody;        /**< Has module SPINBODY been implemented */
  double dVelX;         /**< x Component of the body's velocity */
  double dVelY;         /**< y Component of the body's velocity */
  double dVelZ;         /**< z Component of the body's velocity */
  double dPositionX;    /**< x Component of the body's position */
  double dPositionY;    /**< y Component of the body's position */
  double dPositionZ;    /**< z Component of the body's position */
  double bUseOrbParams; /**< Boolean flag to use orbital parameters as inputs */
  double *dDistance3;   /**< Distance cubed to different perturbers */
  double *dDistanceX;   /**< X Distance between two bodies */
  double *dDistanceY;   /**< Y Distance between two bodies */
  double *dDistanceZ;   /**< Z Distance between two bodies */
  double *dHCartPos;    /**< Heliocentric Cartesian Position used for orbital
                           element calculations */
  double *dHCartVel;    /**< Heliocentric Cartesian Velocity used for orbital
                           element calculations */
  double *dBCartPos;    /**< Barycentric Cartesian Position used for orbital
                           element calculations */
  double *dBCartVel;    /**< Barycentric Cartesian Velocity used for orbital
                           element calculations */
  double dGM;           /**< GM for the star */
  double dMu;           /**< G(M+m) */
  int iGravPertsSpiNBody; /**< Number of bodies that are orbitally relevent
                             (equal to for evolve->iNumBodies) */

  /* DISTORB parameters */
  int bDistOrb;  /**< Has module DISTORB been implemented */
  double dHecc;  /**< Poincare H */
  double dKecc;  /**< Poincare K */
  double dPinc;  /**< Poincare P */
  double dQinc;  /**< Poincare Q */
  double dSinc;  /**< sin(0.5*Inclination) */
  double dLongA; /**< Longitude of ascending node */
  double dArgP;  /**< Argument of pericenter */
  double dLongP; /**< Longitude of pericenter */
  double dMeanA; /**< Mean anomaly (only used for inv plane calculation) */
  double dTrueL; /**< True longitude (only used for insolation calculation */
  double dEccA;  /**< Eccentric anomaly (only used for inv plane calculation) */
  double *daCartPos; /**< Cartesian position of body (only used for inv plane
                        calc) */
  double *daCartVel; /**< Cartesian velocity of body (only used for inv plane
                        calc) */
  int iGravPerts;    /**< Number of bodies which perturb the body */
  int *iaGravPerts;  /**< Which bodies are perturbers of the body */
  int iEigFreqs; /**< Number of eigenfrequencies that control the body's motion
                  */
  int *iaEigFreqs; /**< Indices of eigenfrequencies */
  int bGRCorr; /**< Use general relativistic correction in DistOrb+DistRot?*/
  int iDistOrbModel; /**< Which orbital model to use (RD4 or LL2) */
  double
        dSemiPrev; /**< Semi-major axis at which LL2 eigensolution was calc'd */
  double dEigenvalue; /**< User input eigenvalue (diagnostic only) */
  double
        dEigenvector; /**< User input eigenvector amplitude (diagnostic only) */
  int bEigenSet;      /**< Manually set an eigenvalue/frequency */
  double *daLOrb;     /**< Orbital angular momentum */
  double *daLOrbTmp;  /**< Temp copy of orbital angular momentum */
  double dRPeri;      /**< Pericenter distance */
  double dRApo;       /**< Apocenter distance */

  /* DISTROT parameters */
  int bDistRot;
  double dPrecA;      /**< Precession angle */
  double dTrueApA;    /**< True anomaly at equinox (used for invariable plane
                         conversion) */
  double dDynEllip;   /**< Dynamical ellipticity */
  double dYobl;       /**< sin(obliq)*sin(preca) */
  double dXobl;       /**< sin(obliq)*cos(preca) */
  double dZobl;       /**< cos(obliq) */
  double *daLRot;     /**< Spin angular momentum vector */
  double *daLRotTmp;  /**< Temp copy of spin angular momentum vector */
  int bForcePrecRate; /**< Set precession rate to a fixed value */
  double dPrecRate;   /**< Value to set fixed precession rate to */
  int bCalcDynEllip; /**< Calc dyn ellipticity from spin, radius, mass, inertia?
                      */
  int bRelaxDynEllip; /**< shape of planet relaxes when spun down */
  int bReadOrbitData; /**< Use orbit data from file rather than distorb */
  char cFileOrbitData[NAMELEN]; /**< read orbital data from this file
                                   (distorb=0) */
  double *daTimeSeries;         /**< time series for orbital data */
  double *daSemiSeries;         /**< time series for orbital data */
  double *daEccSeries;          /**< time series for orbital data */
  double *daIncSeries;          /**< time series for orbital data */
  double *daArgPSeries;         /**< time series for orbital data */
  double *daLongASeries;        /**< time series for orbital data */
  double *daMeanASeries;        /**< time series for orbital data */
  int iCurrentStep;             /**< index for time series arrays */
  double *daHeccSeries;         /**< time series for orbital data */
  double *daKeccSeries;         /**< time series for orbital data */
  double *daPincSeries;         /**< time series for orbital data */
  double *daQincSeries;         /**< time series for orbital data */
  double dPdot;           /**< inclination derivative used for obliquity evol */
  double dQdot;           /**< inclination derivative used for obliquity evol */
  int iNLines;            /**< Number of lines of orbital data file */
  double dSpecMomInertia; /**< C/M/R^2 used for dynamical ellipticity
                             calculation */

  /* EQTIDE Parameters */
  int bEqtide;         /**< Apply Module EQTIDE? */
  int bTideLock;       /**< Is a body tidally locked? */
  double dLockTime;    /**< Time when body tidally-locked */
  int bUseTidalRadius; /**< Set a fixed tidal radius? */
  int bUseOuterTidalQ; /**< Set total Q to outer layer's value? */
  double dTidalRadius; /**< Radius used by tidal evoltion equations (CPL only
                          currently) */
  int iTidePerts;      /**< Number of Tidal Perturbers */
  int *iaTidePerts;    /**< Body #'s of Tidal Perturbers */
  char saTidePerts[MAXARRAY][NAMELEN]; /**< Names of Tidal Perturbers */
  double dK2Man;                       /**< Mantle k2 love number */
  double dK2Ocean;                     /**< Ocean's Love Number */
  double dK2Env;                       /**< Envelope's Love Number */
  double dTidalQMan;                   /**< Tidal Q of the Mantle */
  double dTidalQOcean;                 /**< Body's Ocean Component to Tidal Q */
  double dTidalQEnv;       /**< Body's Envelope Component to Tidal Q */
  double dImK2Man;         /**< Mantle Im(k2) love number */
  double dImK2ManOrbModel; /**< Mantle Im(k2) model for DB15 orbital eqns */
  double dImK2Ocean;  /**< Envelope Component to Imaginary part of Love's K_2 */
  double dImK2Env;    /**< Envelope Component to Imaginary part of Love's K_2 */
  double dTidalQ;     /**< Body's Tidal Q */
  double dTidalTau;   /**< Body's Tidal Time Lag */
  double *dTidalZ;    /**< As Defined in \cite HellerEtal2011 */
  double *dTidalChi;  /**< As Defined in \cite HellerEtal2011 */
  double **dTidalF;   /**< As Defined in \cite HellerEtal2011 */
  double *dTidalBeta; /**< As Defined in \cite HellerEtal2011 */
  int **iTidalEpsilon;    /**< Signs of Phase Lags */
  double dDeccDtEqtide;   /**< Eccentricity time rate of change */
  double *daDoblDtEqtide; /**< Obliquity time rate of change */

  /* RADHEAT Parameters: H = Const*exp[-Time/HalfLife] */
  int bRadheat;          /**< Apply Module RADHEAT? */
  double d26AlConstMan;  /**< Body's Mantle 26Al Decay Constant */
  double d26AlMassMan;   /**< Body's Mantle Mass of 26Al */
  double d26AlNumMan;    /**< Body's Mantle Number of 26Al Atoms */
  double d26AlPowerMan;  /**< Body's Mantle Internal Power Due to 26Al Decay */
  double d26AlConstCore; /**< Body's Core 26Al Decay Constant */
  double d26AlMassCore;  /**< Body's Core Mass in 26Al */
  double d26AlNumCore;   /**< Body's Core Number of 26Al Atoms */
  double d26AlPowerCore; /**< Body's Core Power from 26Al */

  double d40KConstMan;   /**< Body's Mantle 40K Decay Constant */
  double d40KMassMan;    /**< Body's Mantle Mass of 40K */
  double d40KNumMan;     /**< Body's Mantle Number of 40K Atoms */
  double d40KPowerMan;   /**< Body's Mantle Internal Power Due to 40K Decay */
  double d40KConstCore;  /**< Body's Core 40K Decay Constant */
  double d40KNumCore;    /**< Body's Core Number of 40K Atoms */
  double d40KPowerCore;  /**< Body's Core Power due to 40K */
  double d40KMassCore;   /**< Body's Core Mass of 40K */
  double d40KConstCrust; /**< Body's Crust 40K Decay Constant */
  double d40KNumCrust;   /**< Body's Crust Number of 40K Atoms */
  double d40KPowerCrust; /**< Body's Crust Power due to 40K */
  double d40KMassCrust;  /**< Body's Crust Mass of 40K */

  double d232ThConstMan; /**< Body's Thorium-232 Decay Constant */
  double d232ThNumMan;   /**< Body's Number of Thorium-232 Atoms */
  double d232ThPowerMan; /**< Body's Internal Power Due to Thorium-232 Decay */
  double d232ThMassMan;  /**< Body's Total Mass of Thorium-232 Atoms */
  double d232ThConstCore;
  double d232ThNumCore;
  double d232ThPowerCore;
  double d232ThMassCore;
  double d232ThConstCrust;
  double d232ThNumCrust;
  double d232ThPowerCrust;
  double d232ThMassCrust;
  double d238UConstMan; /**< Body's Uranium-238 Decay Constant */
  double d238UNumMan;   /**< Body's Number of Uranium-238 Atoms */
  double d238UPowerMan; /**< Body's Internal Power Due to Uranium-238 Decay */
  double d238UMassMan;  /**< Body's Total Mass of Uranium-238 Atoms */
  double d238UConstCore;
  double d238UNumCore;
  double d238UPowerCore;
  double d238UMassCore;
  double d238UConstCrust;
  double d238UNumCrust;
  double d238UPowerCrust;
  double d238UMassCrust;
  double d235UConstMan;
  double d235UNumMan;
  double d235UPowerMan;
  double d235UMassMan;
  double d235UConstCore;
  double d235UNumCore;
  double d235UPowerCore;
  double d235UMassCore;
  double d235UConstCrust;
  double d235UNumCrust;
  double d235UPowerCrust;
  double d235UMassCrust;
  double dRadPowerTotal; /**< Total planet Radiogenic Power */
  double dRadPowerMan;   /**< Total Mantle Radiogenic Power */
  double dRadPowerCore;  /**< Total Core Radiogenic Power */
  double dRadPowerCrust; /**< Total Crust Radiogenic Power */

  /* Thermint Parameters */
  int bThermint;             /**< Apply Module THERMINT? */
  double dTSurf;             /**< Surface Temperature */
  double dTMan;              /**< Temperature Mantle AVE */
  double dTCore;             /**< Temperature Core AVE */
  double dTUMan;             /**< Temperature UMTBL */
  double dTLMan;             /**< Temperature LMTBL */
  double dTCMB;              /**< Temperature CMB */
  double dTICB;              /**< Temperature ICB */
  double dBLUMan;            /**< UM TBL thickness */
  double dBLLMan;            /**< LM TBL thickness */
  double dTJumpUMan;         /**< Abs Temperature Jump across UMTBL */
  double dTJumpLMan;         /**< Abs Temperature Jump across LMTBL */
  double dSignTJumpUMan;     /**< Sign of Temperature Jump across UMTBL */
  double dSignTJumpLMan;     /**< Sign of Temperature Jump across LMTBL */
  double dViscUManArr;       /**< Viscosity UMTBL Arrhenius Law */
  double dViscUMan;          /**< Viscosity UMTBL */
  double dViscLMan;          /**< Viscosity LMTBL */
  double dViscMMan;          /**< Viscosity Mid (ave) mantle */
  double dDynamViscos;       /**< Dynamic viscosity of the mantle */
  double dViscJumpMan;       /**< Viscosity Jump UM to LM */
  double dShmodUMan;         /**< Shear modulus UMTBL */
  double dShmodLMan;         /**< Shear modulus LMTBL */
  double dTsolUMan;          /**< Solidus Temperature UMTBL */
  double dTliqUMan;          /**< Liquidus Temperature UMTBL */
  double dTsolLMan;          /**< Solidus Temperature LMTBL */
  double dTliqLMan;          /**< Liquidus Temperature LMTBL */
  double dFMeltUMan;         /**< Melt fraction UMTBL */
  double dFMeltLMan;         /**< Melt fraction LMTBL */
  double dMeltfactorUMan;    /**< Melt Phase Factor for Rheology */
  double dMeltfactorLMan;    /**< Melt Phase Factor for Rheology */
  double dFixMeltfactorUMan; /**< Melt Phase Factor for Rheology */
  double dViscMeltB;         /**< Viscosity Melt Factor B */
  double dViscMeltGamma;     /**< Viscosity Melt Factor Gamma */
  double dViscMeltDelta;     /**< Viscosity Melt Factor Delta */
  double dViscMeltXi;        /**< Viscosity Melt Factor Xi */
  double dViscMeltPhis;      /**< Viscosity Melt Factor Phis */
  double dDepthMeltMan;      /**< Depth to base of UM Melt layer */
  double dTDepthMeltMan;     /**< Temp at base of UM Melt layer */
  double dTJumpMeltMan;      /**< Temp Jump to base of UM Melt layer */
  double dMeltMassFluxMan;   /**< Mantle upwelling melt mass flux */
  double dRayleighMan;       /**< Mantle Rayleigh Number */
  /* Time Derivatives & Gradients */
  double dTDotMan;   /**< Time deriv of mean mantle temp */
  double dTDotCore;  /**< time deriv of mean core temp */
  double dHfluxUMan; /**< hflux upper mantle thermal boundary layer (UMTBL) */
  double dHflowUMan; /**< hflow UMTBL */
  double dHfluxLMan; /**< hflux lower mantle thermal boundary layer (UMTBL) */
  double dHflowLMan; /**< hflow LMTBL */
  double dHfluxCMB;  /**< hflux CMB */
  double dHflowCMB;  /**< hflow CMB */
  double dHflowTidalMan;  /**< hflow tidal dissipation in mantle */
  double dHflowTidalCore; /**< hflow tidal dissipation in core */
  double dHflowLatentMan; /**< latent hflow from solidification of mantle */
  double dHflowMeltMan;   /**< Eruptive Melt Hflow from mantle */
  double dHflowSecMan;    /**< Mantle Secular Heat flow */
  double dMassICDot;      /**< Mass Growth Rate of IC */
  double dHflowLatentIC;  /**< latent hflow from solidification of IC */
  double dPowerGravIC;    /**< latent hflow from solidification of IC */
  double dHflowICB;       /**< hflow across ICB */
  double dHfluxSurf;      /**< hflux surface of mantle */
  double dHflowSurf;      /**< hflow surface of mantle */
  double dTidalPowMan;    /**< Tidal Dissipation Power in Mantle */
  /* Core Variables */
  double dRIC;            /**< IC radius */
  double dDRICDTCMB;      /**< d(R_ic)/d(T_cmb) */
  double dDOC;            /**< OC shell thickness */
  double dThermConductOC; /**< Thermal conductivity OC */
  double dThermConductIC; /**< Thermal conductivity IC */
  double dChiOC;          /**< OC light element concentration chi. */
  double dChiIC;          /**< IC light element concentration chi. */
  double dMassOC;         /**< OC Mass. */
  double dMassIC;         /**< IC Mass. */
  double dMassChiOC;      /**< OC Chi Mass. */
  double dMassChiIC;      /**< IC Chi Mass. */
  double dDTChi;          /**< Core Liquidus Depression */
  double dHfluxCMBAd;     /**< CMB Adiabatic Heat flux. */
  double dHfluxCMBConv;   /**< CMB Convective (super-adiabatic) Heat flux. */
  double dCoreBuoyTherm;  /**< Core Thermal buoyancy flux */
  double dCoreBuoyCompo;  /**< Core Compositional buoyancy flux */
  double dCoreBuoyTotal;  /**< Core total (therm+compo) buoyancy flux */
  double dGravICB;        /**< Gravity at ICB */
  double dDensAnomICB; /**< Density anomaly across ICB (Delta rho_chi in DB14).
                        */
  double dRICDot;      /**< Inner core growth rate */
  /* Magnetic Field */
  double dMagMom;      /**< Core Dynamo Magnetic Moment scaling law. */
  double dMagMomCoef;  /**< Dynamo magnetic moment scaling law dipolarity
                          coefficient (gamma_d in DB14) */
  double dPresSWind;   /**< Stellar wind pressure at planets orbit. */
  double dMagPauseRad; /**< Magnetopause stand-off radius from center of planet
                        */
  /* Constants */
  double dViscRatioMan; /**< Viscosity Ratio Man */
  double dEruptEff;     /**< Mantle melt eruption efficiency */
  double dViscRef;      /**< Mantle Viscosity Reference (coefficient) */
  double dTrefLind;     /**< Core Liquidus Lindemann Reference (coefficient) */
  double dDTChiRef;     /**< Core Liquidus Depression Reference (E) */
  double dStagLid;      /**< Stagnant Lid heat flow switch (0 or 1)*/
  double dManHFlowPref; /**< Mantle Hflow Prefix */

  double dActViscMan;   /**< Mantle viscosity activation energy */
  double dShModRef;     /**< reference kinematic mantle shear modulus */
  double dStiffness;    /**< effective stiffness of mantle */
  double dDLind;        /**< lindemann's law length scale for iron liquidus*/
  double dDAdCore;      /**< liq iron core adiabatic length scale */
  double dAdJumpM2UM;   /**< adiabatic temp jump from ave mantle to UM */
  double dAdJumpM2LM;   /**< adiabatic temp jump from ave mantle to LM */
  double dAdJumpC2CMB;  /**< adiabatic temp jump from ave core to CMB */
  double dElecCondCore; /**< electrical conductivity of core */

  /* STELLAR Parameters */
  int bStellar;
  double dLuminosity;
  double dTemperature;
  double dSatXUVFrac;
  double dSatXUVTime;
  double dXUVBeta;
  int iStellarModel;
  int iMagBrakingModel;
  int iWindModel;
  int iXUVModel;
  double dLXUV; // Not really a STELLAR parameter
  double iHZModel;
  double dLostAngMom; /**< Angular momemntum lost to space via magnetic braking
                       */
  double dLostEng;    /**< Energy lost to space, i.e. via stellar contraction */
  int bRossbyCut;     /**< Whether or not to shut off magnetic braking for
                         Ro>ROSSBYCRIT */
  int bEvolveRG; /**< Whether or not to evolve radius of gyration? Defaults to 0
                  */
  double dLuminosityInitial;           
  double dLuminosityAmplitude;
  double dLuminosityFrequency;
  double dLuminosityPhase;

  /* POISE parameters */
  int bPoise; /**< Apply POISE module? */

  double dAblateFF;  /**< Scaling factor for ice ablation rate */
  int bAccuracyMode; /**< This forces EBM to re-invert matrix every time step */
  double dAlbedoGlobal;    /**< Global average albedo (Bond albedo) */
  double dAlbedoGlobalTmp; /**< A copy of global average albedo (sometimes
                              needed) */
  double dAlbedoLand;      /**< Sets base albedo of land (sea model) */
  double dAlbedoWater;     /**< Sets base albedo of water (sea model) */
  int bAlbedoZA;           /**< Use albedo based on zenith angle (ann model) */
  double dAreaIceCov; /**< Tracks area of surface covered in permanent ice*/
  double dAstroDist;  /**< Distance between primary and planet */
  int bCalcAB;        /**< Calc A and B from Williams & Kasting 1997 */
  int iClimateModel;  /**< Which EBM to be used (ann or sea) */
  int bColdStart;     /**< Start from global glaciation (snowball) conditions */
  double dCw_dt;      /**< Heat capacity of water / EBM time step */
  double dDiffCoeff;  /**< Diffusion coefficient set by user */
  int bDiffRot;       /**< Adjust heat diffusion for rotation rate */
  int bElevFB;        /**< Apply elevation feedback to ice ablation */
  double dFixIceLat;  /**< Fixes ice line latitude to user set value */
  double dFluxInGlobal;     /**< Global mean of incoming flux */
  double dFluxInGlobalTmp;  /**< Copy of global mean incoming flux */
  double dFluxOutGlobal;    /**< Global mean of outgoing flux */
  double dFluxOutGlobalTmp; /**< Copy of global mean outgoing flux */
  int bForceObliq;          /**< Force obliquity to evolve sinusoidally */
  int bForceEcc;            /**< Force eccentricity to evolve sinusoidally */
  double dFrzTSeaIce;       /**< Freezing temperature of sea water */
  int iGeography;           /**< Type of geography to use (uni3 or modn) */
  int bHadley; /**< Use Hadley circ in tropics when calc'ing diffusion? */
  double dHeatCapAnn;    /**< Surface heat capacity in annual model */
  double dHeatCapLand;   /**< Heat capacity of land */
  double dHeatCapWater;  /**< Heat capacity of water */
  double dIceAlbedo;     /**< Base albedo of ice covered surfaces */
  double dIceBalanceTot; /**< Total gain/loss in ice globally */
  double dIceDepRate;    /**< Snow deposition rate when below freezing */
  double dIceFlowTot;    /**< Total flow of ice (should be zero) */
  double dIceMassTot;    /**< Total ice mass over entire globe */
  int bIceSheets;        /**< Use ice sheet model? */
  int iIceTimeStep; /**< Time step of ice sheet model (should be > iNumYears) */
  double dInitIceHeight; /**< Initial height of ice sheet */
  double dInitIceLat;    /**< Initial latitude of ice line (ice cap only) */
  double dLapseR; /**< Lapse rate used for elevation feedback of ice sheet */
  double dLatentHeatIce; /**< Latent heat of fusion of ice over mixing depth*/
  double dLatFHeatCp;    /**< Latent heat of ice/heat capacity */
  int bMEPDiff;        /**< Compute diff from maximum entropy prod (D = B/4) */
  double dMixingDepth; /**< Depth of mixing layer of ocean (for thermal
                          inertia)*/
  int iNDays;          /**< Number of days in planet's year/orbit */
  int iNStepInYear;    /**< Number of time steps in a year/orbit */
  double dNuLandWater; /**< Land-ocean interaction term */
  int iNumLats;        /**< Number of latitude cells */
  int iNumYears;       /**< Number of orbits!!! to run seasonal model */
  double dObliqAmp;    /**< Amplitude of forced obliquity oscillation */
  double dObliqPer;    /**< Period of force obliquity oscillation */
  double dObliq0;      /**< Start obliquity for forced oscillation */
  double dEccAmp;      /**< Amplitude of forced eccentricity oscillation */
  double dEccPer;      /**< Period of force eccentricity oscillation */
  double dEcc0;        /**< Start eccentricity for forced oscillation */
  int iOLRModel;   /**< OLR fit (use with bCalcAB=1) from Kasting or Spiegel */
  double dpCO2;    /**< Partial pressure of CO2 (only if bCalcAB = 1) */
  double dPlanckA; /**< Constant term in Blackbody linear approximation */
  double dPlanckB; /**< Linear coeff in Blackbody linear approx (sensitivity) */
  double dPrecA0;  /**< Initial pA value used when distrot is not called */
  int bReadOrbitOblData; /**< Use orbit and obliquity data from file rather
                             than distrot */
  char cFileOrbitOblData[NAMELEN]; /**< read orbital and obliquity data from
                                       this file (distorb=0) */
  double *daOblSeries;             /**< time series for obliquity data */
  double *daPrecASeries;           /**< time series for obliquity data */
  double dRefHeight;      /**< Ref height of "surface" in elevation feedback */
  int iReRunSeas;         /**< When to rerun EBM in ice sheet model */
  double dSeaIceConduct;  /**< Conductivity of sea ice */
  int bSeaIceModel;       /**< Use sea ice model? */
  double dSeasDeltat;     /**< Time step of seasonal model */
  double dSeasDeltax;     /**< Spacing of grid points in seasonal model */
  double dSeasOutputTime; /**< When to output seasonal data */
  double dSeasNextOutput; /**< Next time step to output seasonal data */
  int bSkipSeas;          /**< Ann model will be used if in snowball state */
  int bSkipSeasEnabled; /**< Allow ann model to be used if in snowball state? */
  int bSnowball;        /**< Is planet in snowball state (oceans are frozen)? */
  double
        dSpinUpTol; /**< Tolerance for mean global temp change during spin up */
  double dSurfAlbedo;  /**< Base surface albedo used in ann model */
  double dTGlobal;     /**< Global mean temperature at surface */
  double dTGlobalInit; /**< Initial estimate of global surface temperature */
  double dTGlobalTmp;  /**< Mean global surface temp */
  int iWriteLat; /**< Stores index of latitude to be written in write fxn */
  double dMinIceHeight; /**< Minimum ice thickness to count as icy */

  /* Arrays used by seasonal and annual */
  double *daAnnualInsol; /**< Annually averaged insolation at each latitude */
  double *daDivFlux;     /**< Divergence of surface flux */
  double *daDMidPt;      /**< Diffusion at edges of grid points */
  double **daInsol;      /**< Daily insolation at each latitude */
  double *daFlux;        /**< Meridional surface heat flux */
  double *daFluxIn;      /**< Incoming surface flux (insolation) */
  double *daFluxOut;     /**< Outgoing surface flux (longwave) */
  double *daLats;      /**< Latitude of each cell (centered); South Pole is 0 */
  double *daPeakInsol; /**< Annually averaged insolation at each latitude */
  double *daTGrad;     /**< Gradient of temperature (meridional) */

  /* Arrays for annual model */
  double *daAlbedoAnn; /**< Albedo of each cell */
  double
        *daDiffusionAnn; /**< Diffusion coefficient of each latitude boundary */
  double **daMEulerAnn;  /**< Matrix used for Euler step in annual model */
  double **daMEulerCopyAnn; /**< Temp copy of Euler matrix */
  double **daInvMAnn;       /**< Inverted matrix for annual model */
  double *daLambdaAnn;      /**< Diffusion terms for annual matrix */
  double **daMClim;         /**< Raw climate matrix for annual model */
  double **daMDiffAnn;      /**< Diffusion matrix for annual model */
  double *daPlanckAAnn;     /**< Array of Planck A values for ann model */
  double *daPlanckBAnn;     /**< Array of Planck B values for ann model */
  int *iaRowswapAnn;      /**< Array of interchanged rows in matrix inversion */
  double *daScaleAnn;     /**< Used in matrix inversion routine */
  double *daSourceF;      /**< Heating terms in EBM */
  double *daTempAnn;      /**< Surface temperature in each cell */
  double *daTempTerms;    /**< Temperature dependent terms in matrix */
  double *daTmpTempAnn;   /**< Temporary copy of temperature */
  double *daTmpTempTerms; /**< Temporary copy of temp dependent terms */
  double *daUnitVAnn;     /**< Unit vector used in matrix inversion */

  /* Arrays for seasonal model */
  double *daAlbedoAvg;    /**< Orbit average albedo by latitude */
  double *daAlbedoAvgL;   /**< Orbit average albedo by latitude on land */
  double *daAlbedoAvgW;   /**< Orbit average albedo by latitude on water */
  double *daAlbedoLand;   /**< Albedo of land by latitude */
  double *daAlbedoLW;     /**< Land-water averaged albedo */
  double *daAlbedoWater;  /**< Albedo of land by latitude */
  double *daBasalFlow;    /**< Basal flow of ice = d(u*h)/dy */
  double *daBasalFlowMid; /**< Basal flow of ice d(u*h)/dy (midpoints) */
  double *daBasalVel;     /**< Basal velocity of ice */
  double *daBedrockH;     /**< Height of bedrock (can be negative) */
  double *daBedrockHEq;   /**< Equilibrium height of bedrock */
  double *daDeclination;  /**< Daily solar declination */
  double *daDeltaTempL; /**< Keeps track of temp change on land for energy check
                         */
  double *daDeltaTempW; /**< Keeps track of temp change on water for energy
                           check */
  double *daDIceHeightDy;  /**< Gradient of ice height */
  double *daDiffusionSea;  /**< Diffusion coefficient for seasonal model */
  double *daDivFluxAvg;    /**< Divergence of flux averaged over orbit */
  double **daDivFluxDaily; /**< Daily values of divergence of flux */
  double *daEnergyResL;    /**< Energy residuals on land */
  double *daEnergyResW;    /**< Energy residuals over water */
  double *daEnerResLAnn;   /**< Annually averaged energy residuals on land */
  double *daEnerResWAnn;   /**< Annually averaged energy residuals over water */
  double *daFluxAvg;       /**< Annually averaged meridional flux */
  double *daFluxOutAvg;    /**< Annually averaged outgoing flux */
  double **daFluxDaily;    /**< Daily meridional flux values */
  double *daFluxInAvg;     /**< Annually averaged incoming flux */
  double **daFluxInDaily;  /**< Daily incoming flux values */
  double *daFluxInLand;    /**< Annually averaged incoming flux on land */
  double *daFluxInWater;   /**< Annually averaged incoming flux on water */
  double **daFluxOutDaily; /**< Daily  outgoing flux values */
  double *daFluxOutLand;   /**< Annually averaged outgoing flux on land */
  double *daFluxOutWater;  /**< Annually averaged outgoing flux on water */
  double *daFluxSeaIce;    /**< Heat flux through sea ice */
  double **daIceBalance;   /**< Gain/loss of ice at each latitude and day */
  double *daIceAblateTot;  /**< Total ice loss per orbit */
  double *daIceAccumTot;   /**< Total ice gain per orbit */
  double *daIceBalanceAnnual; /**< Net ice gain/loss over orbit */
  double *daIceBalanceAvg;    /**< Average ice gain/loss over orbit */
  double *daIceBalanceTmp;    /**< Temporary (current) ice gain/loss */
  double *daIceFlow;          /**< Flow of ice */
  double *daIceFlowAvg;       /**< Average flow of ice over orbit */
  double *daIceFlowMid;       /**< Flow of ice at boundaries of grid points */
  double *daIceGamTmp;      /**< Temporary variable used in ice sheet matrix */
  double *daIceHeight;      /**< Height of ice sheet */
  double *daIceMass;        /**< Ice mass per area */
  double *daIceMassTmp;     /**< Temporary copy of ice mass per area */
  double *daIcePropsTmp;    /**< Temporary array used in ice sheet matrix */
  double *daIceSheetDiff;   /**< Diffusion coefficient of ice sheet flow */
  double **daIceSheetMat;   /**< Matrix used in ice sheet flow */
  double **daInvMSea;       /**< Inverted matrix in seasonal EBM */
  double *daLambdaSea;      /**< Diffusion terms in seasonal EBM matrix */
  double dLandFrac;         /**< Land fraction input by user */
  double *daLandFrac;       /**< Fraction of cell which is land */
  double **daMDiffSea;      /**< Diffusion only matrix in seasonal EBM */
  double **daMEulerCopySea; /**< Temporary copy of Euler time step matrix
                               (seasonal) */
  double **daMEulerSea;     /**< Euler time step matrix in seasonal EBM */
  double **daMInit;  /**< Temporary matrix used in constructing Euler matrix */
  double **daMLand;  /**< Land terms in seasonal matrix */
  double **daMWater; /**< Water terms in seasonal matrix */
  double *daPlanckASea;    /**< Array of Planck A values in seasonal model */
  double *daPlanckBSea;    /**< Array of Planck B values in seasonal model */
  double **daPlanckBDaily; /**< Array of Planck B values over seasonal cycle */
  double *daPlanckBAvg; /**< Orbit averaged Planck B values in seasonal model */
  int *iaRowswapSea;    /**< Interchanged rows in seasonal matrix inversion */
  double *daScaleSea;   /**< Used in matrix inversion routine */
  double *daSeaIceHeight; /**< Sea ice height by latitude */
  double *daSeaIceK;      /**< Heat conductivity of sea ice */
  double *daSedShear;     /**< sediment shear stress (for ice sheets) */
  double *daSourceL;      /**< Land heating terms: PlanckA - (1-albedo)*Insol */
  double *
        daSourceLW; /**< Combined heat terms what inverser matrix operates on */
  double *daSourceW;     /**< Water heating terms: PlanckA - (1-albedo)*Insol */
  double *daTempAvg;     /**< Temperature averaged over orbit and land/water */
  double *daTempAvgL;    /**< Land temp averaged over orbit */
  double *daTempAvgW;    /**< Water temp averaged over orbit */
  double **daTempDaily;  /**< Daily temp over seasonal cycle */
  double *daTempLand;    /**< Temperature over land (by latitude) */
  double *daTempLW;      /**< Surface temperature (avg over land & water) */
  double *daTempMaxLW;   /**< Maximum temperature over year */
  double *daTempMaxLand; /**< Maximum temperature over year over land */
  double *daTempMaxWater; /**< Maximum temperature over year over water */
  double *daTempMinLW;    /**< Minimum temperature over year */
  double *daTempWater;    /**< Temperature over ocean (by lat) */
  double *daTmpTempSea; /**< Temporary copy of temp dependent terms (sea EBM)*/
  double *daUnitVSea;   /**< Unit vector used in matrix routines */
  double *daWaterFrac;  /**< Fraction of cell which is water */
  double *daXBoundary;  /**< Locations of grid boundaries in x = sin(lat) */
  double *daYBoundary;  /**< Locations of grid boundaries in y = R*lat */

  // FLARE
  int bFlare;
  double dFlareYInt; /**< Flare function Y intercept /FFD linear coefficient*/
  // double dFlareYIntErrorUpper; /**< Upper error of the Y intercept /FFD
  // linear coefficient*/
  // double dFlareYIntErrorLower; /**< Lower error of the Y intercept /FFD
  // linear coefficient*/
  double dFlareSlope; /**< Flare function slope /FFD angular coefficient*/
  // double dFlareSlopeErrorUpper; /**< Upper error of slope /FFD angular
  //                                  coefficient*/
  // double dFlareSlopeErrorLower; /**< Lower error of slope /FFD angular
  //                                  coefficient*/
  double dFlareMinEnergy; /**< Flare minimum energy value to calculate the FFD*/
  double dFlareMaxEnergy; /**< Flare maximum energy value to calculate the FFD*/
  double dFlareFreq1;     /**< First value of flare frequency range*/
  double dFlareFreq2;     /**< Second value of flare frequency range*/
  double dFlareFreq3;     /**< Third value of flare frequency range*/
  double dFlareFreq4;     /**< Fourth value of flare frequency range*/
  double dFlareFreqMin;   /**< Flare frequency of the flares with the lowest
                             energy*/
  double dFlareFreqMid;   /**< Flare frequency of the flares with the central
                             energy value in the energy range*/
  double dFlareFreqMax;   /**< Flare frequency of the flares with the highest
                             energy*/
  double dFlareEnergy1;   /**< First value of flare energy range*/
  double dFlareEnergy2;   /**< Second value of flare energy range*/
  double dFlareEnergy3;   /**< Third value of flare energy range*/
  double dFlareEnergy4;   /**< Fourth value of flare energy range*/
  double dFlareEnergyMin; /**< Minimum value of flare energy in the energy
                             range*/
  double dFlareEnergyMid; /**< Central value of flare energy in the energy
                             range*/
  double dFlareEnergyMax; /**< Maximum value of flare energy in the energy
                             range*/
  double dLXUVFlare;      /**< XUV luminosity by flare*/
  // double dLXUVFlareUpper; /**< Upper value of XUV luminosity by flare when
  // the
  //                            user include the slope and Y-intercept errors */
  // double dLXUVFlareLower; /**< Lower value of XUV luminosity by flare when
  // the
  //                            user include the slope and Y-intercept errors */
  double dLXUVTot;        /**< XUV luminosity total, flare + stellar*/
  double dLXUVFlareConst; /**< XUV luminosity given by the user*/
  int iFlareFFD;          /**< Flare mode*/
  int iFlareBandPass; /**< Option to choose in which band pass the input energy
                         are*/
  int iFlareSlopeUnits; /**< Mode to choose in which units the FFD slopes are*/
  int iEnergyBin;       /**< Number of energies consider between the minimum and
                              maximum energies to calculate the luminosity by flares*/
  double *daEnergyERG;
  double *daEnergyJOU;
  double *daLogEner;
  double *daEnerJOU;

  double *daEnergyJOUXUV;
  double *daEnergyERGXUV;
  double *daLogEnerXUV;
  double *daFFD;
  double *daLXUVFlare;


  // GALHABIT
  int bGalHabit;        /**< Use galhabit module */
  double dPeriQ;        /**< Pericenter distance */
  int iDisrupt;         /**< Secondary body has been disrupted */
  int bGalacTides;      /**< Enable galactic tides */
  double dHostBinSemi;  /**< Semi-major axis of host binary */
  double dHostBinEcc;   /**< Eccentricity of host binary */
  double dHostBinInc;   /**< Inclination of host binary */
  double dHostBinArgP;  /**< Arg pericenter of host binary */
  double dHostBinLongA; /**< Long asc node of host binary */
  double dHostBinMass1; /**< Mass of large host binary star */
  int bHostBinary;      /**< Model dynamics of inner host binary */
  double
        *daRelativeImpact; /**< Impact param of passing star relative to body */
  double *daRelativeVel;   /**< Velocity of passing star relative to body */
  double dEccX;            /**< X component of eccentricity vector */
  double dEccY;            /**< Y component of eccentricity vector */
  double dEccZ;            /**< Z component of eccentricity vector */
  double dAngMX;           /**< X component of orbital momentum vector */
  double dAngMY;           /**< Y component of orbital momentum vector */
  double dAngMZ;           /**< Z component of orbital momentum vector */
  double dAngM;            /**< Magnitude of orbital momentum vector */
  double dEccXTmp;         /**< Ecc X in the host binary reference plane */
  double dEccYTmp;         /**< Ecc Y in the host binary reference plane */
  double dEccZTmp;         /**< Ecc Z in the host binary reference plane */
  double dAngMXTmp;        /**< AngM X in the host binary reference plane */
  double dAngMYTmp;        /**< AngM Y in the host binary reference plane */
  double dAngMZTmp;        /**< AngM Z in the host binary reference plane */
  double dArgPTmp;  /**< Arg pericenter in the host binary reference plane */
  double dLongATmp; /**< Long asc node in the host binary reference plane */
  double dIncTmp;   /**< Inclination in the host binary reference plane */
  double dCosArgP;  /**< Cosine of arg pericenter */
  double dMinStellarApproach; /**< minimum allowed close approach of body to
                                 host */
  double dMassInterior;       /**< Total mass of bodies interior to body */
  int iBadImpulse;            /**< Was there a bad impulse? */

  double dMeanL; /**< Body's mean longitude */

  // MAGMOC
  /* HERE
   * declare all variables used
   */
  int bMagmOc;           /**< Use magmoc model */
  int bManSolid;         /**< Mantle solidified */
  int bAllFeOOxid;       /**< All FeO in manlte oxidized to Fe2O3 */
  int bLowPressSol;      /**< Switch to low pressure treatment of solidus */
  int bManStartSol;      /**< Mantle starts to solidify */
  int bCalcFugacity;     /**< Need to calc oxygen fugacity */
  int bPlanetDesiccated; /**< Atmosphere desiccated */
  int bManQuasiSol;      /**< Atmosphere desiccated & T_surf below 1000K */
  int bMagmOcHaltSolid;  /**< Mantle solidifed or atm desiccated */
  int bMagmOcHaltDesicc; /**< Atm desiccated or escape stopped*/
  int bEscapeStop;       /**< Atmospheric escaped stopped */
  int bCO2InAtmosphere;  /**< Is CO2 present in the atmopshere? */
  int iRadioHeatModel;   /**< Which Radiogenic Heating model to use */
  int iMagmOcAtmModel;   /**< Which Atmopsheric Flux model to use */
  int bOptManQuasiSol;   /**< Solidify mantle inst. when melt frac = 0.4 at surf
                          */

  /* Primary variables */
  double dPotTemp;         /**< Potential Temp of the mantle [K] */
  double dSurfTemp;        /**< Surface Temp of the planet [K] */
  double dSolidRadius;     /**< Solidification radius of the mantle [m] */
  double dWaterMassMOAtm;  /**< Water mass in magma ocean and atmosphere [kg] */
  double dWaterMassSol;    /**< Water mass in the solidified mantle [kg] */
  double dOxygenMassMOAtm; /**< Water mass in magma ocean and atmosphere [kg] */
  double dOxygenMassSol;   /**< Water mass in the solidified mantle [kg] */
  double dHydrogenMassSpace; /**< Mass of hydrogen that is lost to space */
  double dOxygenMassSpace;   /**< Mass of oxygen that is lost to space */
  double dCO2MassMOAtm; /**< Mass of CO2 in magma ocean and atmosphere [kg] */
  double dCO2MassSol;   /**< Mass of CO2 in solidified mantle [kg] */
  /* Input variables */
  double dCoreRadius;     /**< Core radius of the planet [m] */
  double dWaterMassAtm;   /**< Water mass in the atmosphere [kg] */
  double dManMeltDensity; /**< Density of the molten mantle [km/m^3] */
  double dMassFracFeOIni; /**< Initial FeO mass fraction in the mantle */
  double dWaterPartCoeff; /**< Water partition coefficient between melt and
                             solid */
  double dDepthMO;        /**< Initial depth of Magma Ocean [km] */
  /* Other variables Thermal model */
  double
        dGravAccelSurf; /**< Graviational acceleration at the surface [m/s^2] */
  double dSolidRadiusLocal; /**< Local variable for solidification radius of the
                               mantle [m] */
  double dTransDepthSol;    /**< Depth of transition from low to high pressure
                               solidus [Pa] */
  double dPrefactorA;       /**< Prefactor for linear solidus */
  double dPrefactorB;       /**< Prefactor for linear solidus */
  double dMeltFraction;     /**< Melt fraction of the mantle */
  double dMeltFracSurf;     /**< Melt fraction at the surface */
  double dKinemViscos;      /**< Kinematic viscosity of the mantle [m/s^2] */
  double dFactorDerivative; /**< Factor to calculate the derivatives of Tpot and
                               Rsol */
  double dManHeatFlux;      /**< Mantle heat flux [W/m^2] */
  double dRadioHeat;   /**< Radiogenic heating rate GET FROM RADHEAT [W/kg] */
  double dTidalHeat;   /**< Tidal heating rate GET FROM EQTIDE [W/kg] */
  double dNetFluxAtmo; /**< Net atmospheric flux OLR-ASR [W/m^2] */
  double dAlbedo;      /**< Albedo of the planet */
  double dEffTempAtm;  /**< Effective temperature of the planet's atmosphere */
  /* Other variables Volatile model */
  double dPressWaterAtm; /**< Water pressure in atmosphere [Pa] */
  double
        dPartialPressWaterAtm; /**< Partial Water pressure in atmosphere [Pa] */
  double dPressCO2Atm;         /**< CO2 pressure in atmosphere [Pa] */
  double dPartialPressCO2Atm;  /**< Partial CO2 pressure in atmosphere [Pa] */
  double dPressOxygenAtm;      /**< Oxygen pressure in atmosphere [Pa] */
  double dMassMagmOcLiq;       /**< liquid mass of magma ocean [kg] */
  double dMassMagmOcCry;       /**< crystal mass of magma ocean [kg] */
  double dWaterFracMelt;       /**< Mass fraction of water in the magma ocean */
  double dCO2FracMelt;         /**< Mass fraction of CO2 in the magma ocean */
  double dFracFe2O3Man;        /**< Mass fraction of Fe2O3 in the mantle */
  double dOxygenMassAtm;       /**< Oxygen mass in the atmosphere [kg] */
  double dAveMolarMassMan;     /**< Average molar mass of the mantle */
  /* Variables for the connection between magmoc and atmesc */
  double dWaterMassEsc;  /**< Water mass escaped per time */
  double dOxygenMassEsc; /**< Oxygen mass escaped per time */
  double dHZInnerEdge;   /**< Inner edge of habitable zone (runaway) */
};

/* SYSTEM contains properties of the system that pertain to
   every BODY */

/* Pointer to Laplace semi-major axis functions in DistOrb */
typedef double (*fnLaplaceFunction)(double, int);

struct SYSTEM {
  char cName[NAMELEN]; /**< System's Name */

  int iNumBodies; /** Number of bodies in the system; redundant with Evolve! */

  double dTotAngMomInit; /**< System's Initial Angular Momentum */
  double dTotAngMom;     /**< System's Current Angular Momentum */

  /* DISTORB tools */
  fnLaplaceFunction **fnLaplaceF; /**< Pointers to semi-major axis functions  */
  fnLaplaceFunction *
        *fnLaplaceDeriv; /**< Pointers to semi-major axis derivatives */
  double ***daLaplaceC;  /**< Values of semi-major axis functions */
  double ***daLaplaceD;  /**< Values of semi-major axis derivatives */
  double ***daAlpha0;    /**< Semi-major axis ratio at the time LaplaceC is
                            determined */
  int **iaLaplaceN; /**< Indices for dmLaplaceC corresponding to iBody, jBody */
  double dDfcrit; /**< Semi-maj functions will be updated based on this value */
  double
        dThetaInvP; /**< Azimuthal angle of inv plane relative to input plane */
  double dPhiInvP;  /**< Altitude angle of inv plane relative to input plane */
  double **daEigenValEcc; /**< Matrix of eccentricity Eigenvalues in
                             Laplace-Lagrange */
  double **daEigenValInc; /**< Matrix of inclination Eigenvalues in
                             Laplace-Lagrange */
  double **daEigenVecEcc; /**< Matrix of eccentricity Eigenvectors in
                             Laplace-Lagrange */
  double **daEigenVecInc; /**< Matrix of inclination Eigenvectors in
                             Laplace-Lagrange */
  double **daEigenPhase;  /**< Phase angles used in Laplace-Lagrange solution */
  double **daA;     /**< Matrix used for finding eigenvalues for eccentricity */
  double **daB;     /**< Matrix used for finding eigenvalues for inclination */
  double *daAsoln;  /**< RHS of eigenvalue problem for ecc */
  double *daBsoln;  /**< RHS of eigenvalue problem for inc */
  double **daetmp;  /**< Temporary matrix used in eigenvalue routine */
  double **daitmp;  /**< Temporary matrix used in eigenvalue routine */
  double *dah0;     /**< Initial value of Hecc in LL2 solution */
  double *dak0;     /**< Initial value of Kecc in LL2 solution */
  double *dap0;     /**< Initial value of Pinc in LL2 solution */
  double *daq0;     /**< Initial value of Qinc in LL2 solution */
  double *daS;      /**< Scaling factor for ecc eigenvectors */
  double *daT;      /**< Scaling factor for inc eigenvectors */
  int *iaRowswap;   /**< Row interchange array used in eigenvector routine */
  double **daAcopy; /**< Copy of eigenvalue matrix for eccentricity */
  double *daScale;  /**< Used in matrix inversion */
  double *daLOrb;   /**< Total angular momentum of system */

  double dTotEnInit; /**< System's Initial Energy */
  double dTotEn;     /** < System's total energy */

  double dGalacDensity;   /**< Density of galactic environment (for GalHabit) */
  double *daPassingStarR; /**< Initial location of passing star */
  double *daPassingStarV; /**< Initial velocity of passing star */
  double dPassingStarVRad; /**< Radial velocity of passing star (wrt origin) */
  double dPassingStarRMag; /**< Distance to passing star */
  double *daPassingStarImpact; /**< 3D impact parameter for passing star */
  double dPassingStarMass;     /**< Mass of passing star */
  double dPassingStarSigma;    /**< Velocity dispersion of passing stars */
  double dPassingStarMagV;     /**< Magnitude of passing star */
  double dEncounterRad;        /**< User-set encounter radius (box size) */
  double dDeltaTEnc;           /**< time since last encounter */
  double dEncounterRate;       /**< characteristic encounter time */
  double dCloseEncTime;        /**< time of new close encounter */
  double dLastEncTime;         /**< time of last encounter */
  double dNextEncT;            /**< Time of next encounter */
  int iNEncounters;            /**< Number of encounters */
  double dRForm;               /**< galactocentric formation radius */
  double dTMigration;          /**< time of radial migration */
  int bRadialMigr;             /**< use radial migration */
  double dScalingFTot;         /**< scaling factor for radial migration */
  double dScalingFStars;       /**< scaling factor for radial migration */
  double dScalingFVelDisp; /**< scaling factor for radial migration (velocity
                              disp) */
  double dGasDensity;      /**< density of local ism */
  double dDMDensity;       /**< density of local dark matter */
  double dStarScaleL;      /**< scale length of stellar disk */
  double dVelDispSolar;    /**< Velocity dispersion in solar neighborhood */
  double dHostApexVelMag;  /**< Magnitude of host star apex velocity */
  double *daHostApexVel;   /**< Host star apex velocity vector */
  double *daRelativeVel;  /**< Relative velocity b/w passing star and orbiter */
  double *daRelativePos;  /**< Distance b/w passing star and orbiter */
  double dRelativeVelRad; /**< Radial component of velocity b/w passing star &
                             orbiter */
  double dRelativeVelMag; /**< Magnitude relative velocity b/w passing star &
                             orbiter */
  double *daGSNumberDens; /**< Number density of stars in solar neighborhood */
  double *daGSBinMag;     /**< Magnitude bins of stars in solar neighborhood */
  double *daEncounterRateMV; /**< Encounter rate of passing stars */
  int iSeed;                 /**< RNG seed for stellar encounters */
  double dGalaxyAge;         /**< present day age of galaxy */
  int bStellarEnc;           /**< model stellar encounters? */
  int bTimeEvolVelDisp;      /**< scale velocity dispersion of passing stars w/
                                sqrt(t)?*/
  int bOutputEnc; /**< output stell encounter info (beware large output!) */
  double dEncDT;  /**< time b/w stell encounter impulses on primary/2ndary */
  double dTStart; /**< time that encounter begins relative to time step */

  int **iaResIndex; /**< j values for resonance (-1 deactivates the resonance)
                     */
  int *iaResOrder;  /**< order of resonance */
  int bResAvg; /**< Average over resonant arguments (suitable for circulation)
                */
  double **daLibrFreq2; /**< Libration frequency of exact resonance via linear
                           theory */
  double **daCircFreq;  /**< Circulation frequency of near resonance */
  double **daDistCos; /**< Cosine prefactors of disturbing fxn resonant terms */
  double **daDistSin; /**< Sine prefactors of disturbing fxn resonant terms */
  double **daDistSec; /**< Pyth sum of prefactors of disturbing fxn resonant
                         terms */
};

/*
 * Updates: Struct that contains all the variables to be updated and the
 * functions to be called to be updated, fnUpdate.
 */

struct UPDATE {
  /* N.B. that pdVar points to the same memory location as
   * body.x, where x=semi, ecc, etc. */
  double **pdVar; /**< Pointers to Primary Variables */
  int iNumVars;   /**< Number of Update-able Variables */

  /*! The "type" refers to how the variable is updated. If 0, then
      the variable is assumed to be an explicit function of age. The
      first timestep is then a bit dodgy as the rate is not initially
      known. The suggested timestep will be dEta*dTimestep, so runs
      with a Type 0 variable must account for  the evolution with
      dTimeStep.
  */
  int **iaType; /**< Variable type affecting timestep (0 = explicit function of
                   age, 1 = normal quantity with time derivative, 2 =
                   polar/sinusoidal quantity with time derivative, 3 =
                   sinusoidal quantity with explicit function of age) */
  double *daDeriv;      /**< Array of Total Derivative Values for each Primary
                           Variable */
  double **daDerivProc; /**< Array of Derivative Values Due to a Process */
  double *dVar;
  double dZero; /**< Sometimes you need a pointer to zero */

  /*! The body numbers to calculate the derivative. First dimension is
      the Primary variable number, second is the process #, third is the
      list body numbers.
  */
  int ***iaBody;
  // XXX Should be iaNumBodies
  int **iNumBodies; /**< Number of Bodies Affecting a Process */

  /* These keep track of the variable and modules */
  int iNumModules; /**< Number of Modules Affecting a Body XXX Obsolete?*/
  int *iNumEqns;   /**< Number of Equations That Modify a Primary Variable */
  int *iaVar;      /**< Primary variable number */
  /*! The Module number responsible for a given process. The first dimension
    is the Primary variable number. Second is the Equation. */
  int **iaModule;


  /* Number of eqns to modify a parameter */
  int iNumRot;  /**< Number of Equations Affecting Rotation Rate */
  int iNumSemi; /**< Number of Equations Affecting Semi-Major Axis */
  int iNumRadius;
  int iNumRadGyra;
  int iNumMass;

  /* These are the variables that the update matrix modifies */

  // Eccentricity is now split into Hecc and Kecc to accomodate Lagrange
  // Obliquity is now split into Xobl, Yobl and Zobl to accomedate Laskar
  int iRot;        /**< variable number Corresponding to Rotation Rate */
  double dDRotDt;  /**< Total Rotation Rate Derivative */
  int iSemi;       /**< variable number Corresponding to Semi-major Axis */
  double dDSemiDt; /**< Total Semi-Major Axis Derivative */
  int iRadius;
  int iRadGyra; /**, variable number corresponding to radius of gyration */
  int iMass;

  /* Next comes the identifiers for the module that modifies a variable */

  /* MAGMOC parameters */
  /* HERE
   * again for primary variables
   */
  int iWaterMassMOAtm;
  int iNumWaterMassMOAtm;
  int iWaterMassSol;
  int iNumWaterMassSol;
  int iSurfTemp;
  int iNumSurfTemp;
  int iPotTemp;
  int iNumPotTemp;
  int iSolidRadius;
  int iNumSolidRadius;
  int iOxygenMassMOAtm;
  int iNumOxygenMassMOAtm;
  int iOxygenMassSol;
  int iNumOxygenMassSol;
  int iOxygenMassSpace;
  int iNumOxygenMassSpace;
  int iHydrogenMassSpace;
  int iNumHydrogenMassSpace;
  int iCO2MassMOAtm;
  int iNumCO2MassMOAtm;
  int iCO2MassSol;
  int iNumCO2MassSol;

  int iWaterMassMOAtmMagmOc;
  int iWaterMassSolMagmOc;
  int iSurfTempMagmOc;
  int iPotTempMagmOc;
  int iSolidRadiusMagmOc;
  int iOxygenMassMOAtmMagmOc;
  int iOxygenMassSolMagmOc;
  int iOxygenMassSpaceMagmOc;
  int iHydrogenMassSpaceMagmOc;
  int iCO2MassMOAtmMagmOc;
  int iCO2MassSolMagmOc;

  double dWaterMassMOAtm;
  double dWaterMassSol;
  double dSurfTemp;
  double dPotTemp;
  double dSolidRadius;
  double dOxygenMassMOAtm;
  double dOxygenMassSol;
  double dHyrdogenMassSpace;
  double dOxygenMassSpace;
  double dCO2MassMOAtm;
  double dCO2MassSol;

  double *pdDWaterMassMOAtm;
  double *pdDWaterMassSol;
  double *pdDSurfTemp;
  double *pdDPotTemp;
  double *pdDSolidRadius;
  double *pdDOxygenMassMOAtm;
  double *pdDOxygenMassSol;
  double *pdDHydrogenMassSpace;
  double *pdDOxygenMassSpace;
  double *pdDCO2MassMOAtm;
  double *pdDCO2MassSol;

  /* SPINBODY parameters */
  int iVelX;
  int iNumVelX;
  int iVelY;
  int iNumVelY;
  int iVelZ;
  int iNumVelZ;
  int iPositionX;
  int iNumPositionX;
  int iPositionY;
  int iNumPositionY;
  int iPositionZ;
  int iNumPositionZ;

  double dVelX;      /**< x Component of the body's velocity */
  double dVelY;      /**< y Component of the body's velocity */
  double dVelZ;      /**< z Component of the body's velocity */
  double dPositionX; /**< x Component of the body's position */
  double dPositionY; /**< y Component of the body's position */
  double dPositionZ; /**< z Component of the body's position */

  double *pdDVelX;
  double *pdDVelY;
  double *pdDVelZ;
  double *pdDPositionX;
  double *pdDPositionY;
  double *pdDPositionZ;

  /* EQTIDE */
  //  int iEccEqtide;       /**< equation number Corresponding to EQTIDE's
  //  Change to Eccentricity */
  int iHeccEqtide;    /**< equation number Corresponding to EQTIDE's Change to
                         Poincare's h */
  int iKeccEqtide;    /**< equation number Corresponding to EQTIDE's Change to
                         Poincare's k */
  int *iaXoblEqtide;  /**< Equation numbers Corresponding to EQTIDE's Change to
                         Laskar's X */
  int *iaYoblEqtide;  /**< Equation numbers Corresponding to EQTIDE's Change to
                         Laskar's Y */
  int *iaZoblEqtide;  /**< Equation numbers Corresponding to EQTIDE's Change to
                         Laskar's Z */
  int *iaRotEqtide;   /**< Equation numbers Corresponding to EQTIDE's Change to
                         Rotation Rate */
  int iSemiEqtide;    /**< equation number Corresponding to EQTIDE's Change to
                         Semi-major Axis */
  int iLostEngEqtide; /**< equation number Corresponding to EQTIDE's lost energy
                         [tidal heating] */

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      semi-major axis' derivative due to EQTIDE. */
  double *pdDsemiDtEqtide;

  /*! Points to the element in UPDATE's daDerivProc matrix that contains
    Poincare's h derivative due to EQTIDE. */
  double *pdDHeccDtEqtide;

  /*! Points to the element in UPDATE's daDerivProc matrix that contains
    Poincare's k derivative due to EQTIDE. */
  double *pdDKeccDtEqtide;

  /*! Points to the elements in UPDATE's daDerivProc matrix that contains
      Laskar's X derivatives due to EQTIDE. */
  double **padDXoblDtEqtide;

  /*! Points to the elements in UPDATE's daDerivProc matrix that contains
      Laskar's Y derivatives due to EQTIDE. */
  double **padDYoblDtEqtide;

  /*! Points to the elements in UPDATE's daDerivProc matrix that contains
      Laskar's Z derivatives due to EQTIDE. */
  double **padDZoblDtEqtide;

  /*! Points to the elements in UPDATE's daDerivProc matrix that contains the
      rotation rates' derivatives due to EQTIDE. */
  double **padDrotDtEqtide;

  /*! Points to the elements in UPDATE's daDerivProc matrix that contains the
      lost energy via tidal heating's derivatives due to EQTIDE. */
  double *pdLostEngEqtide;

  /* RADHEAT Mantle */
  int i26AlMan;  /**< variable number Corresponding to Aluminum-26 */
  int i40KMan;   /**< variable number Corresponding to Potassium-40 */
  int i232ThMan; /**< variable number Corresponding to Thorium-232 */
  int i238UMan;  /**< variable number Corresponding to Uranium-238 */
  int i235UMan;
  int iNum26AlMan;  /**< Number of Equations Affecting Aluminum-26 [1] */
  int iNum40KMan;   /**< Number of Equations Affecting Potassium-40 [1] */
  int iNum232ThMan; /**< Number of Equations Affecting Thorium-232 [1] */
  int iNum238UMan;  /**< Number of Equations Affecting Uranium-238 [1] */
  int iNum235UMan;
  double dD26AlNumManDt;  /**< Total Aluminum-26 Derivative */
  double dD40KNumManDt;   /**< Total Potassium-40 Derivative */
  double dD232ThNumManDt; /**< Total Thorium-232 Derivative */
  double dD238UNumManDt;  /**< Total Uranium-238 Derivative */
  double dD235UNumManDt;
  double *pdD26AlNumManDt;
  double *pdD40KNumManDt;
  double *pdD232ThNumManDt;
  double *pdD238UNumManDt;
  double *pdD235UNumManDt;

  /* RADHEAT Core */
  int i26AlCore;
  int i40KCore;
  int i232ThCore;
  int i238UCore;
  int i235UCore;
  int iNum26AlCore;
  int iNum40KCore;
  int iNum232ThCore;
  int iNum238UCore;
  int iNum235UCore;
  double dD26AlNumCoreDt;
  double dD40KNumCoreDt;
  double dD232ThNumCoreDt;
  double dD238UNumCoreDt;
  double dD235UNumCoreDt;
  double *pdD26AlNumCoreDt;
  double *pdD40KNumCoreDt;
  double *pdD232ThNumCoreDt;
  double *pdD238UNumCoreDt;
  double *pdD235UNumCoreDt;

  /* RADHEAT CRUST */
  int i40KCrust;
  int i232ThCrust;
  int i238UCrust;
  int i235UCrust;
  int iNum40KCrust;
  int iNum232ThCrust;
  int iNum238UCrust;
  int iNum235UCrust;
  double dD40KNumCrustDt;
  double dD232ThNumCrustDt;
  double dD238UNumCrustDt;
  double dD235UNumCrustDt;
  double *pdD40KNumCrustDt;
  double *pdD232ThNumCrustDt;
  double *pdD238UNumCrustDt;
  double *pdD235UNumCrustDt;

  /* THERMINT */
  int iTMan;       /**< variable number Corresponding to Tman */
  int iNumTMan;    /**< Number of Equations Affecting TMan */
  double dTDotMan; /**< TMan time Derivative */
  double *pdTDotMan;
  int iTCore;       /**< variable number Corresponding to Tman */
  int iNumTCore;    /**< Number of Equations Affecting TCore */
  double dTDotCore; /**< TCore time Derivative */
  double *pdTDotCore;
  // double dDynamViscos;

  /* DISTORB */
  /* Number of eqns to modify a parameter */
  int iNumHecc; /**< Number of Equations Affecting h = e*sin(longp) */
  int iNumKecc; /**< Number of Equations Affecting k = e*cos(longp) */
  int iNumPinc; /**< Number of Equations Affecting p = s*sin(longa) */
  int iNumQinc; /**< Number of Equations Affecting q = s*cos(longa) */

  int iHecc;          /**< Variable number Corresponding to h = e*sin(longp) */
  double dDHeccDt;    /**< Total h Derivative */
  int iKecc;          /**< Variable number Corresponding to k = e*cos(longp) */
  double dDKeccDt;    /**< Total k Derivative */
  int iPinc;          /**< Variable number Corresponding to p = s*sin(longa) */
  double dDPincDt;    /**< Total p Derivative */
  int iQinc;          /**< Variable number Corresponding to q = s*cos(longa) */
  double dDQincDt;    /**< Total q Derivative */
  int *iaHeccDistOrb; /**< equation number Corresponding to DistOrb's change to
                         h = e*sin(longp) */
  int *iaKeccDistOrb; /**< Equation numbers Corresponding to DistOrb's change to
                         k = e*cos(longp) */
  int *iaPincDistOrb; /**< Equation numbers Corresponding to DistOrb's change to
                         p = s*sin(longa) */
  int *iaQincDistOrb; /**< Equation numbers Corresponding to DistOrb's change to
                         q = s*cos(longa) */

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      h = e*sin(varpi) derivative due to DistOrb. */
  double **padDHeccDtDistOrb;

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      k = e*cos(varpi) derivative due to DistOrb. */
  double **padDKeccDtDistOrb;

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      p = s*sin(Omega) derivative due to DistOrb. */
  double **padDPincDtDistOrb;

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      q = s*cos(Omega) derivative due to DistOrb. */
  double **padDQincDtDistOrb;

  /* DISTROT */
  int iNumXobl;     /**< Number of Equations Affecting x = sin(obl)*cos(pA) */
  int iNumYobl;     /**< Number of Equations Affecting y = sin(obl)*sin(pA) */
  int iNumZobl;     /**< Number of Equations Affecting z = cos(obl) */
  int iNumDynEllip; /**< Number of Equations Affecting Dynamical Ellipticity */

  int iXobl;       /**< variable number Corresponding to x = sin(obl)*cos(pA) */
  double dDXoblDt; /**< Total x Derivative */
  int iYobl;       /**< variable number Corresponding to y = sin(obl)*sin(pA) */
  double dDYoblDt; /**< Total y Derivative */
  int iZobl;       /**< variable number Corresponding to z = cos(obl) */
  double dDZoblDt; /**< Total z Derivative */
  int iDynEllip; /**< variable number Corresponding to dynamical ellipticity */
  double dDDynEllipDt; /**< Dynamical Ellipticity Derivative */
  int *iaXoblDistRot;  /**< equation number Corresponding to DistRot's change to
                          x = sin(obl)*cos(pA) */
  int *iaYoblDistRot; /**< Equation numbers Corresponding to DistRot's change to
                         y = sin(obl)*sin(pA) */
  int *iaZoblDistRot; /**< Equation numbers Corresponding to DistRot's change to
                         z = cos(obl) */

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      xi = sin(obliq)*sin(pA) derivative due to DISTROT. */
  double **padDXoblDtDistRot;

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      zeta = sin(obliq)*cos(pA) derivative due to DISTROT. */
  double **padDYoblDtDistRot;

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      chi = cos(obliq) derivative due to DISTROT. */
  double **padDZoblDtDistRot;

  /* GALHABIT */
  int iNumEccX; /**< Number of equations for x eccentricity */
  int iNumEccY; /**< Number of equations for y eccentricity */
  int iNumEccZ; /**< Number of equations for z eccentricity */

  int iEccX;       /**< Variable # for x eccentricity */
  int iEccY;       /**< Variable # for y eccentricity */
  int iEccZ;       /**< Variable # for z eccentricity */
  double dDEccXDt; /**< Derivative for ecc x */
  double dDEccYDt; /**< Derivative for ecc y */
  double dDEccZDt; /**< Derivative for ecc z */

  int *iaEccXGalHabit; /**< Equation # for GalHabit's change in x ecc */
  int *iaEccYGalHabit; /**< Equation # for GalHabit's change in y ecc */
  int *iaEccZGalHabit; /**< Equation # for GalHabit's change in z ecc */

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      x component of ecc derivative due to GALHABIT */
  double **padDEccXDtGalHabit;
  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      y component of ecc derivative due to GALHABIT */
  double **padDEccYDtGalHabit;
  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      z component of ecc derivative due to GALHABIT */
  double **padDEccZDtGalHabit;

  int iNumAngMX; /**< Number of equations for x angular momentum*/
  int iNumAngMY; /**< Number of equations for y angular momentum*/
  int iNumAngMZ; /**< Number of equations for z angular momentum*/

  int iAngMX;       /**< Variable # for x angular momentum */
  int iAngMY;       /**< Variable # for y angular momentum */
  int iAngMZ;       /**< Variable # for z angular momentum */
  double dDAngMXDt; /**< Derivative for angular mom x */
  double dDAngMYDt; /**< Derivative for angular mom y */
  double dDAngMZDt; /**< Derivative for angular mom z */

  int *iaAngMXGalHabit; /**< Equation # for GalHabit's change in x ang mom */
  int *iaAngMYGalHabit; /**< Equation # for GalHabit's change in y ang mom */
  int *iaAngMZGalHabit; /**< Equation # for GalHabit's change in z ang mom */

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      x component of angular mom derivative due to GALHABIT */
  double **padDAngMXDtGalHabit;
  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      y component of angular mom derivative due to GALHABIT */
  double **padDAngMYDtGalHabit;
  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      z component of angular mom derivative due to GALHABIT */
  double **padDAngMZDtGalHabit;

  /* ATMESC */
  int iSurfaceWaterMass; /**< variable number Corresponding to the surface water
                            mass */
  int iNumSurfaceWaterMass; /**< Number of Equations Affecting surface water [1]
                             */
  int iEnvelopeMass; /**< variable number Corresponding to the envelope mass */
  int iNumEnvelopeMass; /**< Number of Equations Affecting envelope mass [1] */
  int iOxygenMass;      /**< variable number Corresponding to the oxygen mass */
  int iNumOxygenMass;   /**< Number of Equations Affecting oxygen [1] */
  int iOxygenMantleMass; /**< variable number Corresponding to the oxygen mass
                            in the mantle */
  int iNumOxygenMantleMass; /**< Number of Equations Affecting oxygen mantle
                               mass [1] */

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      derivative of these variables due to ATMESC. */
  double *pdDSurfaceWaterMassDtAtmesc;
  double *pdDEnvelopeMassDtAtmesc;
  double *pdDMassDtAtmesc;
  double *pdDOxygenMassDtAtmesc;
  double *pdDOxygenMantleMassDtAtmesc;
  double *pdRadiusAtmesc;

  /* BINARY */
  int iCBPR; /**< variable number Corresponding to the CBP's orbital radius */
  int iNumCBPR; /**< Number of Equations Affecting CBP orbital radius [1] */
  int iCBPZ;    /**< variable number corresponding to the CBP's cylindrical Z
                   positions */
  int iNumCBPZ; /**< Number of Equations Affecting CBP cylindrical Z position
                   [1] */
  int iCBPPhi; /**< variable number Corresponding to the CBP's orbital azimuthal
                  angle */
  int iNumCBPPhi; /**< NUmber of equations Affecting CBP orbital azimuthal angle
                     [1] */
  int iCBPRDot; /**< variable number Corresponding to the CBP's radial velocity
                 */
  int iNumCBPRDot; /**< Number of equations affecting CBP radial velocity [1] */
  int iCBPZDot;    /** < variable number Corresponding to the CBP's Z orbital
                      velocity */
  int iNumCBPZDot; /**< Number of equations affecting CBP z orbital velocity [1]
                    */
  int iCBPPhiDot;  /** < variable number Corresponding to the CBP's Phi orbital
                      angular velocity */
  int iNumCBPPhiDot; /**< Number of equations affecting CBP phi orbital velocity
                        [1] */

  /* Points to the element in UPDATE's daDerivProc matrix that contains the
   * derivative of these variables due to BINARY. */
  double *pdCBPRBinary;    // Equation that governs CBP orbital radius
  double *pdCBPZBinary;    // Equation that governs CBP cylindrical position Z
  double *pdCBPPhiBinary;  // Equation that governs CBP orbital azimuthal angle
  double *pdCBPRDotBinary; // Equation that governs CBP radial orbital velocity
  double *pdCBPZDotBinary; // Equation that governs CBP z orbital velocity
  double *pdCBPPhiDotBinary; // Equation that governs CBP phi orbital velocity

  /* STELLAR */
  int iLuminosity;    /**< variable number Corresponding to the luminosity */
  int iNumLuminosity; /**< Number of Equations Affecting luminosity [1] */
  int iTemperature;
  int iNumTemperature;

  int iRotStellar; /**< iEqn number for the evolution of rotation in STELLAR */
  int iLostAngMom; /**< iEqn number for the evolution of lost angular momentum
                    */
  int iLostAngMomStellar; /**< iEqn number for the evolution of lost angular
                             momentum in STELLAR */
  int iNumLostAngMom;  /**< Number of Equations Affecting lost angular momentum
                          [1] */
  int iLostEng;        /**< iEqn number for the evolution of lost energy */
  int iLostEngStellar; /**< iEqn number for the evolution of lost energy in
                          STELLAR */
  int iNumLostEng; /**< Number of Equations Affecting lost angular momentum [1]
                    */

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
      function that returns these variables due to STELLAR evolution. */
  double *pdLuminosityStellar;
  double *pdTemperatureStellar;
  double *pdRadiusStellar;
  double *pdRadGyraStellar;
  double *pdRotRateStellar;
  double *pdLostAngMomStellar;
  double *pdLostEngStellar;

  /* FLARE */
  int iLXUV;
  int iLXUVFlare;
  int iEnergyBin;
  double *pdDEnergyBinDt;
  int iNumLXUV;
  double *pdDLXUVFlareDt;

  /* EQTIDE + STELLAR */
  int iSemiEqSt; /**< equation number Corresponding to EQ+ST's Change to
                    Semi-major Axis */

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the
     semi-major axis derivatives due to EQTIDE+STELLAR. */
  double *pdDsemiDtEqSt;
};

struct HALT {
  int iNumHalts;        /**< Total Number of Halts */
  int bMerge;           /**< Halt for Merge? */
  double dMinSemi;      /**< Halt at this Semi-major Axis */
  double dMinObl;       /**< Halt at this Obliquity */
  double dMaxEcc;       /**< Halt at this Eccentricity */
  double dMaxMutualInc; /**< Halt at this mutual incliantion */
  double dMinEcc;       /**< Halt at this Eccentricity */
  int bPosDeDt;         /**< Halt if Eccentricity Derivative is Positive */
  int dMinIntEn;        /**< Halt at this Internal Power */

  /* EQTIDE */
  int bDblSync;  /**< Halt if Double Synchronous? */
  int bTideLock; /**< Halt if Tide-locked? */
  int bSync;     /**< Halt if Rotation Becomes Synchronous? */

  /* RADHEAT */
  double dMin40KPower;   /**< Halt at this Potassium-40 Power */
  double dMin232ThPower; /**< Halt at this Thorium-232 Power */
  double dMin238UPower;  /**< Halt at this Uranium-238 Power */
  double dMin235UPower;
  double dMinRadPower;

  /* ATMESC */
  int bSurfaceDesiccated; /**< Halt if dry?*/
  int bEnvelopeGone;      /**< Halt if evaporated?*/

  /* STELLAR */
  int bEndBaraffeGrid; /***< Halt if we reached the end of the luminosity grid?
                        */

  /* THERMINT */
  double dMinTMan;  /**< Halt at this TMan */
  double dMinTCore; /**< Halt at this TCore */

  /* DISTORB */
  int bOverrideMaxEcc; /**< 1 = tells DistOrb not to halt at maximum
                          eccentricity = 0.6627434 */
  int bHillStab; /**< halt if 2 planets fail Hill stability crit (technically
                    valid for only 2 planets)*/
  int bCloseEnc; /**< halt if any planet pair has orbits too close (crudely
                    comparing inner's apocenter and outer's pericenter)*/

  /* POISE */
  int bHaltMinIceDt; /**< Halt if ice flow time-step falls below a minimum value
                      */
  int iMinIceDt; /**< Halt if ice flow time-step falls below this value (number
                    of orbital periods)*/

  /* BINARY */
  int bHaltHolmanUnstable; /** if CBP.dSemi < holman_crit_a, CBP dynamically
                              unstable -> halt */
  int bHaltRocheLobe; /** if secondary enters the Roche lobe of the primary,
                         HALT! */

  /* MAGMOC */
  int bHaltMantleSolidified;  /**< Halt if mantle completely solidified */
  int bHaltMantleMeltFracLow; /**< Halt if melt fraction drops below 0.4 at
                                 surface */
  int bHaltAtmDesiSurfCool;   /**< Halt if atmosphere desiccated & T_surf below
                                 1000K */
  int bHaltEnterHabZone;      /**< Halt if palenet enters Habitable Zone*/
  int bHaltAllPlanetsSolid; /**< Halt if all planets solidified (for multiplanet
                               system) */
  int bHaltAllPlanetsDesicc; /**< Halt if all planets desiccated (for
                                multiplanet system) */
};

/* Units. These can be different for different bodies. If set
 * in the primary input file, the values are propogated to
 * all the bodies. Note that for most variables, an index of 0
 * corresponds to the first body read in. For units, that is not
 * the case, as the index refers to the file number, i.e. 0 to
 * the primary input file, 1 to the first body read in, etc. This
 * feature allows for the units to be propogated to other files,
 * but is sure to result in some bugs. Be careful!
 */

struct UNITS {
  int iMass;   /**< 0=gm; 1=kg; 2=solar; 3=Earth; 4=Jup; 5=Nep */
  int iLength; /**< 0=cm; 1=m; 2=km; 3=R_sun; 4=R_earth; 5=R_Jup; 6=AU */
  int iAngle;  /**< 0=rad; 1=deg */
  int iTime;   /**< 0=sec; 1=day; 2=yr; 3=Myr; 4=Gyr */
  int iTemp;
};

/* Note this hack -- the second int is for iEqtideModel. This may
   have to be generalized for other modules. */
typedef void (*fnBodyCopyModule)(BODY *, BODY *, int, int, int);

/* Integration parameters */
struct EVOLVE {
  int bDoForward;   /**< Perform Forward Integration? */
  int bDoBackward;  /**< Perform Backward Integration? */
  int iDir;         /**< 1=forward, -1=backward */
  double dTime;     /**< Integration Time */
  double dEta;      /**< Variable Timestep Coefficient */
  double dStopTime; /**< Integration Stop Time */
  double dTimeStep; /**< Integration Time step */
  int bVarDt;       /**< Use Variable Timestep? */
  int iTotalSteps;  /**< Total Number of Steps */
  int iStepsSinceLastOutput;
  double dMinValue;  /**< Minimum Value for Eccentricity and Obliquity to be
                        Integrated */
  int bFirstStep;    /**< Has the First Dtep Been Taken? */
  int iNumBodies;    /**< Number of Bodies to be Integrated */
  int iOneStep;      /**< Integration Method number */
  double dCurrentDt; /**< Current timestep */

  // These are to store midpoint derivative info in RK4.
  BODY *tmpBody;     /**< Temporary BODY struct */
  UPDATE *tmpUpdate; /**< Temporary UPDATE struct */
  double ***daDeriv; /**< The Matrix of Time Derivatives. First dimension is
                        Body #, second is the Primary variable number, third is
                        the equation number.  */
  double ****daDerivProc; /**< Derivatives over a timestep */

  // Module-specific parameters
  int *iNumModules; /**< Number of Modules per Primary Variable */

  /* EQTIDE */
  int iEqtideModel;     /**< EQTIDE Model number */
  int bDiscreteRot;     /**< Use Discrete Rotation Model (CPL)? */
  int *bForceEqSpin;    /**< Force Rotation Rate to be Equilibrium? */
  int *bFixOrbit;       /**< Fix Orbit? */
  double *dMaxLockDiff; /**< Fractional Difference from Tidal Equilibrium Rate
                           to Force Equilibrium. */
  double *dSyncEcc;

  /* RADHEAT */
  /* Nothing? */

  /* DISTORB */
  int iDistOrbModel;
  int bSpiNBodyDistOrb;
  int bUsingDistOrb;
  int bUsingSpiNBody;

  fnBodyCopyModule **fnBodyCopy; /**< Function Pointers to Body Copy */
};

/* The CONTROL struct contains all the parameters that
 * control program flow. */

struct IO {
  int iVerbose; /**< Verbosity Level. 0=none; 1=error; 2=progress; 3=input;
                   4=units; 5=all */
  double dOutputTime; /**< Integration Output Interval */
  double dNextOutput; /**< Time of next output */

  int bLog; /**< Write Log File? */

  /* Output Notation */
  int iDigits; /**< Number of Digits After Decimal */
  int iSciNot; /**< Crossover Decade to Switch between Standard and Scientific
                  Notation */

  int bOverwrite; /**< Allow files to be overwritten? */

  /* The following record whether an error message that should only be reported
     once has been printed. */
  /*! Has the message for DeltaTime on the first timestep been printed? */
  int bDeltaTimeMessage;
  /*! Has the large mutual inclination message been printed? */
  int bMutualIncMessage;
  /*! Print warning message if mutual inc exceeds this value */
  double dMaxMutualInc;
  int *baRocheMessage;      /**< Has the Roche lobe message been printed? */
  int *baCassiniOneMessage; /**< Has the CassiniOne message been printed? */
  int *baCassiniTwoMessage; /**< Has the CassiniTwo message been printed? */
  int *baEnterHZMessage; /**< Has the Entering the HZ message been printed? */
};

/* The CONTROL struct contains all the parameters that
 * control program flow. */
/* CONTROL contains all parameters that control program flow, including I/O,
   halts, units, and the integration, including manipulating the UPDATE
   matrix through fnForceBehavior. */

typedef double (*fnUpdateVariable)(BODY *, SYSTEM *, int *);
typedef void (*fnPropsAuxModule)(BODY *, EVOLVE *, IO *, UPDATE *, int);
typedef void (*fnForceBehaviorModule)(BODY *, MODULE *, EVOLVE *, IO *,
                                      SYSTEM *, UPDATE *, fnUpdateVariable ***,
                                      int, int);
/* HALT struct contains all stopping conditions, other than reaching the end
   of the integration. */

typedef int (*fnHaltModule)(BODY *, EVOLVE *, HALT *, IO *, UPDATE *,
                            fnUpdateVariable ***, int);

struct CONTROL {
  EVOLVE Evolve;
  HALT *Halt;
  IO Io;
  UNITS *Units;

  char sGitVersion[64];

  /* Move to BODY */
  int *iMassRad; /**< Mass-Radius Relationship */

  fnHaltModule **fnHalt; /**< Function Pointers to Halt Checks */
  fnForceBehaviorModule *
        *fnForceBehavior; /**< Function Pointers to Force Behaviors */
  fnForceBehaviorModule *
        *fnForceBehaviorMulti; /**< Function Pointers to Force Behaviors */
  int *iNumMultiForce; /**< Number of multi-module ForceBahevior functions */

  fnPropsAuxModule *
        *fnPropsAux; /**< Function Pointers to Auxiliary Properties */
  fnPropsAuxModule *
        *fnPropsAuxMulti; /**< Function pointers to Auxiliary Properties for
                             multi-module interdependancies. */
  int *iNumMultiProps;    /**< Number of Multi-module PropsAux functions */

  /* Things for DistOrb */
  double dAngNum; /**< Value used in calculating timestep from angle variable */
  int bSemiMajChange; /**< 1 if semi-major axis can change (DistOrb will recalc
                         Laplace coeff functions) */
  int bInvPlane;      /**< 1 = change input coordinates to invariable plane
                         coordinate */
  int bOutputLapl;    /**< 1 = output laplace functions and related data */
  int bOutputEigen;   /**< Output eigen values? */

  int bOrbiters; /**< Does this simulation have orbiting bodies? */
};


/************ FILE INFO *******************/

/* Modules read in and write out over a range of integers. The largest integer
depends on the total number of modules available. */

/********************
 * ADJUST AS NEEDED *
 ********************/

/* Module limits:
 * General: 0 - 1000
 * EQTIDE: 1000 - 1100
 * RADHEAT: 1100 - 1200
 * ATMESC: 1200 - 1300
 * DISTORB: 1300 - 1400
 * DISTROT: 1400 - 1500
 * STELLAR: 1500 - 1600
 * SPINBODY: 1600 - 1700
 * THERMINT: 1700 - 1900
 * POISE: 1900 - 2000
 * FLARE: 2000 - 2100
 * BINARY: 2100 - 2200
 * GALHABIT: 2200 - 2300
 * MAGMOC: 2300 - 2400
 */
// These need to be set to the largest previous limit
#define MODULEOPTEND 2400
#define MODULEOUTEND 2400

/* The INFILE struct contains all the information
 * regarding the files that read in. */

struct INFILE {
  char cIn[NAMELEN]; /**< File Name */
  int *bLineOK;      /**< Line number Format OK? */
  int iNumLines;     /**< Number of Input Lines */
  /* Species file for PHOTOCHEM */
  char cSpecies[NAMELEN]; /**< Name of Chemical Species N/I */
  /* Reaction file for PHOTOCHEM */
  char cReactions[NAMELEN]; /**< Names of Chemical Reactions N/I */

  /* Aerosol scattering files */
  /* Aqueous file -- add to SpeciesFile? */
  /* Array of Vapor pressure file */
};

/* The OUTFILE struct contains all the information
 * regarding the output files. */

struct OUTFILE {
  char cOut[2 * NAMELEN + 10];       /**< Output File Name */
  int iNumCols;                      /**< Number of Columns in Output File
                                        (system.planet+.forward/backward) */
  char caCol[MODULEOUTEND][OPTLEN];  /**< Output Value Name */
  int bNeg[MODULEOUTEND];            /**< Use Negative Option Units? */
  int iNumGrid;                      /**< Number of grid outputs */
  char caGrid[MODULEOUTEND][OPTLEN]; /**< Gridded output name */
};


/* The FILES struct contains all the information
 * regarding every file. */

struct FILES {
  char cExe[LINE];        /**< Name of Executable */
  OUTFILE *Outfile;       /**< Output File Name for Forward Integration */
  char cLog[NAMELEN + 4]; /**< Log File Name (+4 to allow for ".log" suffix) */
  INFILE *Infile;
  int iNumInputs; /**< Number of Input Files */
};

/* The OPTIONS struct contains all the information
 * regarding the options, including their file data. */

struct OPTIONS {
  char cName[OPTLEN];           /**< Option Name */
  char cDescr[OPTDESCR];        /**< Brief Description of Option */
  char cLongDescr[OPTLONDESCR]; /**< Long Description of Option */
  char cValues[OPTDESCR];       /**< Description of permitted values / ranges */
  int iType; /**< Cast of input. 0=bool; 1=int; 2=double; 3=string; +10 for
                array. */
  char cDefault[OPTDESCR]; /**< Description of Default Value */
  /** Qualitative description of the option, included for connection wtih
      bigplanet. Options are: time, length, mass, angle, energy, pressure,
      amperes. Units may be combined with standard mathematical operations, e.g.
      energy/time, or mass*length/time^2.
   */
  char cDimension[OPTDESCR];
  double dDefault; /**< Default Value */
  int iModuleBit;  /**< Bitwise sum of modules permitted to read option */
  int bMultiFile;  /**< Option Permitted in Multiple Input Files? */
  int iMultiIn;
  int *iLine; /**< Option's Line number in Input File */
  char *iFile;
  char cFile[MAXFILES][OPTLEN]; /**< File Name Where Set */
  int bNeg;                     /**< Is There a Negative Option? */
  char cNeg[OPTDESCR];          /**< Description of Negative Unit Conversion */
  int iFileType; /**< What type of file can option be in? 0 = primary only, 1 =
                    body file only, 2 = any file */
  double dNeg;   /**< Conversion Factor to System Units */
};

/* OUTPUT contains the data regarding every output parameters */

/* Some output variables must combine output from different modules.
 * These functions do that combining. XXX I think this is defunct!

 typedef double (*fnOutputModule)(BODY*,SYSTEM*,UPDATE*,int,int); */

/* GRIDOUTPUT will be part of OPTIONS, and contains data for latitudinal
 * parameters in POISE */
// typedef struct {
//   char cName[OPTLEN];    /**< Output Name */
//   char cDescr[LINE];     /**< Output Description */
//   int bNeg;              /**< Is There a Negative Option? */
//   int *bDoNeg;           /**< Should the Output use "Negative" Units? */
//   char cNeg[NAMELEN];    /**< Units of Negative Option */
//   double dNeg;           /**< Conversion Factor for Negative Option */
//   int iNum;              /**< Number of Columns for Output */
//
//   /* Now add vector output functions */
//   fnOutputModule **fnOutput; /**< Function Pointers to Write Output */
//
// } GRIDOUTPUT;

struct OUTPUT {
  char cName[OPTLEN];           /**< Output Name */
  char cDescr[OUTDESCR];        /**< Output Description */
  char cLongDescr[OUTLONDESCR]; /**< Output Long Description */
  int bNeg;                     /**< Is There a Negative Option? */
  int iModuleBit;      /**< Bit flag for module to check output parameters */
  int *bDoNeg;         /**< Should the Output use "Negative" Units? */
  char cNeg[OUTDESCR]; /**< Units of Negative Option */
  double dNeg;         /**< Conversion Factor for Negative Option */
  int iNum;            /**< Number of Columns for Output */
  int bGrid; /**< Is output quantity gridded (e.g. a function of latitude)? */

  //   GRIDOUTPUT *GridOutput;     /**< Output for latitudinal climate params,
  //   etc */
};

typedef void (*fnReadOption)(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                             int);
typedef void (*fnWriteOutput)(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *,
                              UPDATE *, int, double *, char[]);


/*
 * Module-level function pointers
 */

typedef void (*fnInitializeOptions)(OPTIONS *, fnReadOption *);
typedef void (*fnInitializeBodyModule)(BODY *, CONTROL *, UPDATE *, int, int);
typedef void (*fnInitializeControlModule)(CONTROL *, int);
typedef void (*fnInitializeOptionsModule)(OPTIONS *, fnReadOption *);
typedef void (*fnInitializeUpdateModule)(BODY *, UPDATE *, int);
typedef void (*fnInitializeUpdateTmpBodyModule)(BODY *, CONTROL *, UPDATE *,
                                                int);

// All primary variables need a FinalizeUpdate function
// typedef void (*fnFinalizeUpdateEccModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdateVelXModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateVelYModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateVelZModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdatePositionXModule)(BODY *, UPDATE *, int *, int,
                                                int, int);
typedef void (*fnFinalizeUpdatePositionYModule)(BODY *, UPDATE *, int *, int,
                                                int, int);
typedef void (*fnFinalizeUpdatePositionZModule)(BODY *, UPDATE *, int *, int,
                                                int, int);
typedef void (*fnFinalizeUpdate26AlNumCoreModule)(BODY *, UPDATE *, int *, int,
                                                  int, int);
typedef void (*fnFinalizeUpdate26AlNumManModule)(BODY *, UPDATE *, int *, int,
                                                 int, int);
typedef void (*fnFinalizeUpdate40KNumCoreModule)(BODY *, UPDATE *, int *, int,
                                                 int, int);
typedef void (*fnFinalizeUpdate40KNumCrustModule)(BODY *, UPDATE *, int *, int,
                                                  int, int);
typedef void (*fnFinalizeUpdate40KNumManModule)(BODY *, UPDATE *, int *, int,
                                                int, int);
typedef void (*fnFinalizeUpdate232ThNumCoreModule)(BODY *, UPDATE *, int *, int,
                                                   int, int);
typedef void (*fnFinalizeUpdate232ThNumCrustModule)(BODY *, UPDATE *, int *,
                                                    int, int, int);
typedef void (*fnFinalizeUpdate232ThNumManModule)(BODY *, UPDATE *, int *, int,
                                                  int, int);
typedef void (*fnFinalizeUpdate235UNumCoreModule)(BODY *, UPDATE *, int *, int,
                                                  int, int);
typedef void (*fnFinalizeUpdate235UNumCrustModule)(BODY *, UPDATE *, int *, int,
                                                   int, int);
typedef void (*fnFinalizeUpdate235UNumManModule)(BODY *, UPDATE *, int *, int,
                                                 int, int);
typedef void (*fnFinalizeUpdate238UNumCoreModule)(BODY *, UPDATE *, int *, int,
                                                  int, int);
typedef void (*fnFinalizeUpdate238UNumCrustModule)(BODY *, UPDATE *, int *, int,
                                                   int, int);
typedef void (*fnFinalizeUpdate238UNumManModule)(BODY *, UPDATE *, int *, int,
                                                 int, int);
typedef void (*fnFinalizeUpdateCBPRModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateCBPZModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateCBPRDotModule)(BODY *, UPDATE *, int *, int, int,
                                              int);
typedef void (*fnFinalizeUpdateCBPPhiModule)(BODY *, UPDATE *, int *, int, int,
                                             int);
typedef void (*fnFinalizeUpdateCBPZDotModule)(BODY *, UPDATE *, int *, int, int,
                                              int);
typedef void (*fnFinalizeUpdateCBPPhiDotModule)(BODY *, UPDATE *, int *, int,
                                                int, int);
typedef void (*fnFinalizeUpdateDynEllipModule)(BODY *, UPDATE *, int *, int,
                                               int, int);
typedef void (*fnFinalizeUpdateEnvelopeMassModule)(BODY *, UPDATE *, int *, int,
                                                   int, int);
typedef void (*fnFinalizeUpdateHeccModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateIceMassModule)(BODY *, UPDATE *, int *, int, int,
                                              int); // deprecated
typedef void (*fnFinalizeUpdateKeccModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateLuminosityModule)(BODY *, UPDATE *, int *, int,
                                                 int, int);
typedef void (*fnFinalizeUpdateLXUVModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateMassModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdatePincModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateQincModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateRadiusModule)(BODY *, UPDATE *, int *, int, int,
                                             int);
typedef void (*fnFinalizeUpdateRadGyraModule)(BODY *, UPDATE *, int *, int, int,
                                              int);
typedef void (*fnFinalizeUpdateRotModule)(BODY *, UPDATE *, int *, int, int,
                                          int);
typedef void (*fnFinalizeUpdateSemiModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateSurfaceWaterMassModule)(BODY *, UPDATE *, int *,
                                                       int, int, int);
typedef void (*fnFinalizeUpdateOxygenMassModule)(BODY *, UPDATE *, int *, int,
                                                 int, int);
typedef void (*fnFinalizeUpdateOxygenMantleMassModule)(BODY *, UPDATE *, int *,
                                                       int, int, int);
typedef void (*fnFinalizeUpdateTemperatureModule)(BODY *, UPDATE *, int *, int,
                                                  int, int);
typedef void (*fnFinalizeUpdateTCoreModule)(BODY *, UPDATE *, int *, int, int,
                                            int);
typedef void (*fnFinalizeUpdateTManModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateXoblModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateYoblModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateZoblModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateEccXModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateEccYModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateEccZModule)(BODY *, UPDATE *, int *, int, int,
                                           int);
typedef void (*fnFinalizeUpdateAngMXModule)(BODY *, UPDATE *, int *, int, int,
                                            int);
typedef void (*fnFinalizeUpdateAngMYModule)(BODY *, UPDATE *, int *, int, int,
                                            int);
typedef void (*fnFinalizeUpdateAngMZModule)(BODY *, UPDATE *, int *, int, int,
                                            int);
typedef void (*fnFinalizeUpdateMeanLModule)(BODY *, UPDATE *, int *, int, int,
                                            int);
typedef void (*fnFinalizeUpdateLostAngMomModule)(BODY *, UPDATE *, int *, int,
                                                 int, int);
typedef void (*fnFinalizeUpdateLostEngModule)(BODY *, UPDATE *, int *, int, int,
                                              int);
/* HERE
 * declare Finalize Update for primary parameters
 */
typedef void (*fnFinalizeUpdateWaterMassMOAtmModule)(BODY *, UPDATE *, int *,
                                                     int, int, int);
typedef void (*fnFinalizeUpdateWaterMassSolModule)(BODY *, UPDATE *, int *, int,
                                                   int, int);
typedef void (*fnFinalizeUpdateSurfTempModule)(BODY *, UPDATE *, int *, int,
                                               int, int);
typedef void (*fnFinalizeUpdatePotTempModule)(BODY *, UPDATE *, int *, int, int,
                                              int);
typedef void (*fnFinalizeUpdateSolidRadiusModule)(BODY *, UPDATE *, int *, int,
                                                  int, int);
typedef void (*fnFinalizeUpdateOxygenMassMOAtmModule)(BODY *, UPDATE *, int *,
                                                      int, int, int);
typedef void (*fnFinalizeUpdateOxygenMassSolModule)(BODY *, UPDATE *, int *,
                                                    int, int, int);
typedef void (*fnFinalizeUpdateHydrogenMassSpaceModule)(BODY *, UPDATE *, int *,
                                                        int, int, int);
typedef void (*fnFinalizeUpdateOxygenMassSpaceModule)(BODY *, UPDATE *, int *,
                                                      int, int, int);
typedef void (*fnFinalizeUpdateCO2MassMOAtmModule)(BODY *, UPDATE *, int *, int,
                                                   int, int);
typedef void (*fnFinalizeUpdateCO2MassSolModule)(BODY *, UPDATE *, int *, int,
                                                 int, int);

typedef void (*fnReadOptionsModule)(BODY *, CONTROL *, FILES *, OPTIONS *,
                                    SYSTEM *, fnReadOption *, int);
typedef void (*fnVerifyModule)(BODY *, CONTROL *, FILES *, OPTIONS *, OUTPUT *,
                               SYSTEM *, UPDATE *, int, int);
typedef void (*fnManageModuleDerivatives)(BODY *, EVOLVE *, UPDATE *,
                                          fnUpdateVariable ***, int);
typedef void (*fnVerifyHaltModule)(BODY *, CONTROL *, OPTIONS *, int, int *);
typedef void (*fnCountHaltsModule)(HALT *, int *);
typedef void (*fnInitializeOutputModule)(OUTPUT *, fnWriteOutput *);
typedef void (*fnLogBodyModule)(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                                fnWriteOutput *, FILE *, int);
typedef void (*fnLogModule)(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                            fnWriteOutput *, FILE *);

struct MODULE {
  int *iNumModules; /**< Number of Modules per Body */
  int *iNumManageDerivs;
  int **iaModule; /**< Module numbers that Apply to the Body */
  int *iBitSum;

  int *iaEqtide;
  int *iaDistOrb;
  int *iaDistRot;
  int *iaRadheat;
  int *iaThermint;
  int *iaAtmEsc;
  int *iaStellar;
  int *iaPoise;
  int *iaBinary;
  int *iaFlare;
  int *iaGalHabit;
  int *iaSpiNBody;
  int *iaMagmOc;
  int *iaEqtideStellar;

  /*! These functions count the number of applicable halts for each body. */
  fnCountHaltsModule **fnCountHalts;

  /*! These functions allocate memory to module-specific arrays
      inside the CONTROL struct */
  fnInitializeControlModule **fnInitializeControl;

  /*! These functions allocate memory to module-specific arrays
       inside the UPDATE struct */
  fnInitializeUpdateModule **fnInitializeUpdate;

  /*! These functions allocate memory to module-specific arrays
      inside the BODY struct */
  fnInitializeBodyModule **fnInitializeBody;

  /*! These functions allocate memory to module-specific arrays
       inside the OUTPUT struct */
  fnInitializeOutputModule **fnInitializeOutput;

  /*! These functions allocate memory to module-specific arrays
       inside the BODY struct */
  fnInitializeUpdateTmpBodyModule **fnInitializeUpdateTmpBody;

  // Finalize Primary Variable function pointers

  /*! SpiNBody variable finalize functions */
  fnFinalizeUpdateVelXModule **fnFinalizeUpdateVelX;
  fnFinalizeUpdateVelYModule **fnFinalizeUpdateVelY;
  fnFinalizeUpdateVelZModule **fnFinalizeUpdateVelZ;
  fnFinalizeUpdatePositionXModule **fnFinalizeUpdatePositionX;
  fnFinalizeUpdatePositionYModule **fnFinalizeUpdatePositionY;
  fnFinalizeUpdatePositionZModule **fnFinalizeUpdatePositionZ;

  /*! Function pointers to finalize Core's potassium-40 */
  fnFinalizeUpdate26AlNumCoreModule **fnFinalizeUpdate26AlNumCore;
  /*! Function pointers to finalize Mantle's potassium-40 */
  fnFinalizeUpdate26AlNumManModule **fnFinalizeUpdate26AlNumMan;
  /*! Function pointers to finalize Core's potassium-40 */
  fnFinalizeUpdate40KNumCoreModule **fnFinalizeUpdate40KNumCore;
  /*! Function pointers to finalize Crust's potassium-40 */
  fnFinalizeUpdate40KNumCrustModule **fnFinalizeUpdate40KNumCrust;
  /*! Function pointers to finalize Mantle's potassium-40 */
  fnFinalizeUpdate40KNumManModule **fnFinalizeUpdate40KNumMan;
  /*! Function pointers to finalize Core's thorium-232 */
  fnFinalizeUpdate232ThNumCoreModule **fnFinalizeUpdate232ThNumCore;
  /*! Function pointers to finalize Crust's thorium-232 */
  fnFinalizeUpdate232ThNumCrustModule **fnFinalizeUpdate232ThNumCrust;
  /*! Function pointers to finalize Mantle's thorium-232 */
  fnFinalizeUpdate232ThNumManModule **fnFinalizeUpdate232ThNumMan;
  /*! Function pointers to finalize Core's uranium-235 */
  fnFinalizeUpdate235UNumCoreModule **fnFinalizeUpdate235UNumCore;
  /*! Function pointers to finalize Crust's uranium-235 */
  fnFinalizeUpdate235UNumCrustModule **fnFinalizeUpdate235UNumCrust;
  /*! Function pointers to finalize Mantle's uranium-235 */
  fnFinalizeUpdate235UNumManModule **fnFinalizeUpdate235UNumMan;
  /*! Function pointers to finalize Core's uranium-238 */
  fnFinalizeUpdate238UNumCoreModule **fnFinalizeUpdate238UNumCore;
  /*! Function pointers to finalize Crust's uranium-238 */
  fnFinalizeUpdate238UNumCrustModule **fnFinalizeUpdate238UNumCrust;
  /*! Function pointers to finalize Mantle's uranium-238 */
  fnFinalizeUpdate238UNumManModule **fnFinalizeUpdate238UNumMan;
  /*! Function pointers to finalize lost angular momentum */
  fnFinalizeUpdateLostAngMomModule **fnFinalizeUpdateLostAngMom;
  /*! Function pointers to finalize lost energy */
  fnFinalizeUpdateLostEngModule **fnFinalizeUpdateLostEng;


  /*! These functions assign Equation and Module information regarding
      DistOrb h,k,p,q variables in the UPDATE struct. */
  /*! Function pointers to finalize Poincare's h */
  fnFinalizeUpdateHeccModule **fnFinalizeUpdateHecc;
  /*! Function pointers to finalize Poincare's k */
  fnFinalizeUpdateKeccModule **fnFinalizeUpdateKecc;
  /*! Function pointers to finalize Luminosity */
  fnFinalizeUpdateLuminosityModule **fnFinalizeUpdateLuminosity;
  /*! Function pointers to finalize Poincare's p */
  fnFinalizeUpdatePincModule **fnFinalizeUpdatePinc;
  /*! Function pointers to finalize Poincare's q */
  fnFinalizeUpdateQincModule **fnFinalizeUpdateQinc;
  /*! Function pointers to finalize Radius */
  fnFinalizeUpdateRadiusModule **fnFinalizeUpdateRadius;
  /*! Function pointers to finalize Radius of gyration */
  fnFinalizeUpdateRadGyraModule **fnFinalizeUpdateRadGyra;
  /*! Function pointers to finalize Mass */
  fnFinalizeUpdateMassModule **fnFinalizeUpdateMass;
  /*! Function pointers to finalize Rotation Rate */
  fnFinalizeUpdateRotModule **fnFinalizeUpdateRot;
  /*! Function pointers to finalize Semi-major Axis */
  fnFinalizeUpdateSemiModule **fnFinalizeUpdateSemi;
  /*! Function pointers to finalize Surface Water */
  fnFinalizeUpdateSurfaceWaterMassModule **fnFinalizeUpdateSurfaceWaterMass;
  /*! Function pointers to finalize oxygen */
  fnFinalizeUpdateOxygenMassModule **fnFinalizeUpdateOxygenMass;
  /*! Function pointers to finalize mantle oxygen */
  fnFinalizeUpdateOxygenMantleMassModule **fnFinalizeUpdateOxygenMantleMass;
  /*! Function pointers to finalize Envelope Mass */
  fnFinalizeUpdateEnvelopeMassModule **fnFinalizeUpdateEnvelopeMass;
  /*! Function pointers to finalize Core Temperature */
  fnFinalizeUpdateTCoreModule **fnFinalizeUpdateTCore;
  /*! Function pointers to finalize Temperature */
  fnFinalizeUpdateTemperatureModule **fnFinalizeUpdateTemperature;
  /*! Function pointers to finalize Mantle Temperature */
  fnFinalizeUpdateTManModule **fnFinalizeUpdateTMan;

  /* Function points to finalize binary update functions */
  /* CBP R, Z, Phi, and their time derivaties */
  fnFinalizeUpdateCBPRModule **fnFinalizeUpdateCBPR;
  fnFinalizeUpdateCBPZModule **fnFinalizeUpdateCBPZ;
  fnFinalizeUpdateCBPPhiModule **fnFinalizeUpdateCBPPhi;
  fnFinalizeUpdateCBPRDotModule **fnFinalizeUpdateCBPRDot;
  fnFinalizeUpdateCBPZDotModule **fnFinalizeUpdateCBPZDot;
  fnFinalizeUpdateCBPPhiDotModule **fnFinalizeUpdateCBPPhiDot;

  /*! These functions assign Equation and Module information regarding
      DistRot x,y,z variables in the UPDATE struct. */
  /*! Function pointers to finalize distrot's X */
  fnFinalizeUpdateXoblModule **fnFinalizeUpdateXobl;
  /*! Function pointers to finalize distrot's Y */
  fnFinalizeUpdateYoblModule **fnFinalizeUpdateYobl;
  /*! Function pointers to finalize distrot's Z */
  fnFinalizeUpdateZoblModule **fnFinalizeUpdateZobl;
  /*! Function pointers to finalize dynamical ellipticity */
  fnFinalizeUpdateDynEllipModule **fnFinalizeUpdateDynEllip;

  /*! These functions assign Equation and Module information regarding
      GalHabit's ecc and angm variables in the UPDATE struct. */
  /*! Function pointers to finalize galhabit's eccx */
  fnFinalizeUpdateEccXModule **fnFinalizeUpdateEccX;
  /*! Function pointers to finalize galhabit's eccy */
  fnFinalizeUpdateEccYModule **fnFinalizeUpdateEccY;
  /*! Function pointers to finalize galhabit's eccz */
  fnFinalizeUpdateEccZModule **fnFinalizeUpdateEccZ;
  /*! Function pointers to finalize galhabit's angmx */
  fnFinalizeUpdateAngMXModule **fnFinalizeUpdateAngMX;
  /*! Function pointers to finalize galhabit's angmy */
  fnFinalizeUpdateAngMYModule **fnFinalizeUpdateAngMY;
  /*! Function pointers to finalize galhabit's angmz */
  fnFinalizeUpdateAngMZModule **fnFinalizeUpdateAngMZ;


  fnFinalizeUpdateLXUVModule **fnFinalizeUpdateLXUV;

  /*! Function pointers to finalize magmoc functions */
  /* HERE
   * Finalize Update for primary variables
   */
  fnFinalizeUpdateWaterMassMOAtmModule **fnFinalizeUpdateWaterMassMOAtm;
  fnFinalizeUpdateWaterMassSolModule **fnFinalizeUpdateWaterMassSol;
  fnFinalizeUpdateSurfTempModule **fnFinalizeUpdateSurfTemp;
  fnFinalizeUpdatePotTempModule **fnFinalizeUpdatePotTemp;
  fnFinalizeUpdateSolidRadiusModule **fnFinalizeUpdateSolidRadius;
  fnFinalizeUpdateOxygenMassMOAtmModule **fnFinalizeUpdateOxygenMassMOAtm;
  fnFinalizeUpdateOxygenMassSolModule **fnFinalizeUpdateOxygenMassSol;
  fnFinalizeUpdateHydrogenMassSpaceModule **fnFinalizeUpdateHydrogenMassSpace;
  fnFinalizeUpdateOxygenMassSpaceModule **fnFinalizeUpdateOxygenMassSpace;
  fnFinalizeUpdateCO2MassMOAtmModule **fnFinalizeUpdateCO2MassMOAtm;
  fnFinalizeUpdateCO2MassSolModule **fnFinalizeUpdateCO2MassSol;

  /*! These functions log module-specific data. */
  fnLogBodyModule **fnLogBody;

  /*! These functions read module-specific option. */
  fnReadOptionsModule **fnReadOptions;

  /*! These functions verify module-specific options. */
  fnVerifyModule **fnVerify;

  /*! These functions add derivatives to the fnUpdate matrix */
  fnManageModuleDerivatives **fnAssignDerivatives;
  fnManageModuleDerivatives **fnNullDerivatives;

  /*! These functions verify module-specific halts. */
  fnVerifyHaltModule **fnVerifyHalt;
};

/* fnIntegrate is a pointer to a function that performs
 * integration. */
typedef void (*fnIntegrate)(BODY *, CONTROL *, SYSTEM *, UPDATE *,
                            fnUpdateVariable ***, double *, int);

/*
 * Other Header Files - These are primarily for function declarations
 */

/* Top-level files */
#include "body.h"
#include "control.h"
#include "evolve.h"
#include "halt.h"
#include "module.h"
#include "options.h"
#include "output.h"
#include "system.h"
#include "update.h"
#include "verify.h"

/* module files */
#include "atmesc.h"
#include "binary.h"
#include "distorb.h"
#include "distrot.h"
#include "eqtide.h"
#include "flare.h"
#include "galhabit.h"
#include "magmoc.h"
#include "poise.h"
#include "radheat.h"
#include "spinbody.h"
#include "stellar.h"
#include "thermint.h"
