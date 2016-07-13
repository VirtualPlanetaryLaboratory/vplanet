/******************** VPLANET.H ***********************/
/*
 * Rory Barnes, Wed May  7 16:04:27 PDT 2014
 *
 */

 
/*! Top-level declarations */

/* How many modules are available? */
#define EQTIDE        2
#define RADHEAT       4
#define ATMESC        8
#define DISTORB       16
#define DISTROT       32
#define STELLAR       64
#define THERMINT      128
#define POISE         256
#define FLARE         512
#define BINARY        1024

/* Fundamental constants; Some of these are taken from the IAU working
 group on Fundamental constants, as described in Prsa et al. 2016. */

#define BIGG          6.67428e-11  // From Luzum et al., 2011; value recommended by IAU NSFA in Prsa et al. 2016
#define PI            3.1415926535

#define KGAUSS        0.01720209895
#define S0           0 //-0.422e-6     //delta S0 from Armstrong 2014-used in central torque calculation

#define EPS           1e-10       // Precision for difference of doubles to be effectively 0

/* Units: Calculations are done in SI */
#define cLIGHT        299792458.0 
#define MEARTH        5.972186e24 // Prsa et al. 2016   
#define MSUN          1.988416e30 // Prsa et al. 2016
#define AUCM          1.49598e11  // XXX Change to AUM
#define RSUN          6.957e8     // Prsa et al. 2016
#define YEARSEC       3.15576e7
#define DAYSEC        86400
#define REARTH        6.3781e6    // Equatorial; Prsa et al. 2016
#define RJUP          7.1492e7    // Equatorial; Prsa et al. 2016
#define MJUP          1.898130e27 // Prsa et al. 2016
#define RNEP          2.4764e7
#define MNEP          1.0244e26
#define RHOEARTH      5515
#define eEARTH        0.016710219
#define YEARDAY       365.25
#define MSAT          5.6851e26
#define DEGRAD        0.017453292519444445
#define ATOMMASS      1.660538921e-27
#define SIGMA         5.670367e-8
#define LFICE         3.34e5
#define RHOICE        916.7   //density of ice kg/m^3
#define MOCEAN        1.4e21  //mass of earth ocean in kg
#define a1ICE         3.615e-13  //coeff of ice deformability at T<263K (Pa^-3 s^-1)
#define a2ICE         1.733e3    //coeff of ice deformability at T>=263K (Pa^-3 s^-1)
#define Q1ICE         6e4        //energy in ice deformation at T<263K (J/mol)
#define Q2ICE         13.9e4     //energy in ice deformation at T>=263 (J/mol)
#define RGAS          8.3144598  //gas constant in J K^-1 mol^-1
#define nGLEN         3.0  //Glen's law coefficient
#define RHOSED        2390  //sediment density from Huybers&Tziperman08
#define RHOH2O        1000
#define SEDPHI        (22.0*PI/180.0)  //angle of internal friction (sediment)
#define SEDH          10      //depth of sediment layer (m)
#define SEDD0         2.5e-14  //reference deformation rate for sediment (s^-1)
#define SEDMU         3e9     //reference viscosity for sediment (Pa s)
#define RHOBROCK      3370
#define BROCKTIME     5000  //relaxation timescale for bedrock 

/* Exit Status */

#define EXIT_EXE      1
#define EXIT_INPUT    2
#define EXIT_UNITS    3
#define EXIT_WRITE    4
#define EXIT_INT      5
#define EXIT_OUTPUT   6

/* Verbosity Level */

#define VERBERR       1
#define VERBPROG      2    
#define VERBINPUT     3
#define VERBUNITS     4
#define VERBALL       5

/* File Limits */

#define NUMOUT        2100  /* Number of output parameters 2000->2100 for binary */
#define MAXBODIES     10
#define OPTLEN        24    /* Maximum length of an option */
#define OPTDESCR      128    /* Number of characters in option description */
#define OUTLEN        48     /* Maximum number of characters in an output column header */
#define LINE          128   /* Maximum number of characters in a line */
#define NAMELEN       100

#define MAXFILES      24    /* Maximum number of input files */
#define MAXARRAY      64    /* Maximum number of options in 
			     * an option array */
#define NUMOPT	      1000  /* Number of options that could be 
			     * in MODULE */
#define MAXLINES      256   /* Maximum Number of Lines in an 
			     * input file */
#define OPTEND        2100  /* Last output number of module options
			     * Binary is highest for this compiltion */

#define TINY          (1./HUGE)

/* 0 => Not input by user, verify assigns default */
#define EULER         1
#define RUNGEKUTTA    2

/* Indices for variables in the update struct. These are the primary 
   variables. */
#define VSEMI        1001
#define VECC         1002
#define VROT         1003
#define VOBL         1004
#define VRADIUS      1005
#define VMASS        1006

// RADHEAT
#define VNUM40KMAN      1101
#define VNUM232THMAN    1102
#define VNUM238UMAN     1103
#define VNUM235UMAN     1104
#define VNUM40KCORE     1105
#define VNUM232THCORE   1106
#define VNUM238UCORE    1107
#define VNUM235UCORE    1108
/* INTERIOR THERMAL */   // Use 1200's ok??
#define VTMAN     1201
#define VTCORE    1202

//DistOrb
#define VHECC           1301
#define VKECC           1302
#define VPINC           1303
#define VQINC           1304

//DISTROT
#define VXOBL           1401
#define VYOBL           1402
#define VZOBL           1403
#define VDYNELLIP       1404

/* Semi-major axis functions in DistOrb */
#define LAPLNUM 	      26

// ATMESC
#define VSURFACEWATERMASS  1202
#define VENVELOPEMASS      1203

// STELLAR
#define VLUMINOSITY     1502
#define VTEMPERATURE    1503

// POISE
#define VICEMASS        1851

// BINARY: 2000-2999, inclusive
// Primary variables that control CBP's cylindrical positions, velocities
#define VCBPR              2000
#define VCBPPHI            2010
#define VCBPZ              2020
#define VCBPRDOT           2030
#define VCBPPHIDOT         2040
#define VCBPZDOT           2050

// FLARE
#define VLXUV           1901

/* Now define the structs */

/*!
 * BODY contains all the physical parameters for every body 
 * Why won't these lines be added?
 */

#define MAXSPECIES       100
typedef struct {
  double dInitTimeStep;
  double dMaxSteps;
} PHOTOCHEM;

/* Body Structure */
typedef struct {
  char cName[NAMELEN];   /**< Body's Name */
  int iBodyType;        /**< Body's type: 0 for planet, 1 for star */
  /**< Type of object: 0=star, 1=rocky planet, 2 = giant */
  char iType; 

  /* Body Properties */
  double dAge;           /**< Body's Age */
  double dMass;		 /**< Body's Mass */
  double dRadius;	 /**< Radius of body */
  double dDensity;       /**< Bulk density of body*/
  double dGravAccel;     /**< Body's gravitational acceleration */
  double dK2;		 /**< Body's Love number */
  double dObliquity;     /**< Body's Obliquity */
  double dRotRate;       /**< Body's Rotation Rate */
  double dRotPer;        /**< Body's Rotation Period */
  double dRotVel;        /**< Body's Rotational Velocity */
  double dRadGyra;       /**< Body's Radius of Gyration */
  double dPowRadiogCore; /**< Body's Core's  Radiogenic Power */
  double dPowRadiogMan;  /**< Body's Mantle's  Radiogenic Power */
  double dPowCoreRadiog; /**< Body's Core's  Radiogenic Power */
  double dPowManRadiog;  /**< Body's Mantle's  Radiogenic Power */
  char cColor[OPTLEN];   /**< Body color (for plotting) */
  double *daSED;         /**< Body's spectral energy distribution by wavelength N/I */

  /* Orbital Properties. By convention, these are stored in the
   * second element in the BODY array and, if using binary
   * in the secondary (1st (0, 1, ..)) body*/
  double dSemi;          /**< Body's Semi-major Axis */
  double dEcc;           /**< Body's Eccentricity */
  double dMeanMotion;    /**< Body's Mean Motion */
  double dOrbPeriod;     /**< Body's Orbital Period */
  double dEccSq;         /**< Eccentricity squared */

  /* DISTORB parameters */
  int bDistOrb;         /**< Has module DISTORB been implemented */ 
  double dHecc;           /**< Poincare H */
  double dKecc;           /**< Poincare K */
  double dPinc;           /**< Poincare P */
  double dQinc;           /**< Poincare Q */
  double dSinc;          /**< sin(0.5*Inclination) */
  double dLongA;         /**< Longitude of ascending node */
  double dArgP;          /**< Argument of pericenter */
  double dLongP;         /**< Longitude of pericenter */
  double dMeanA;         /**< Mean anomaly (currently only used for inv plane calculation) */
  double dTrueL;         /**< True longitude (currently only used for insolation calculation */
  double dEccA;          /**< Eccentric anomaly (currently only used for inv plane calculation) */
  double *dCartPos;      /**< Cartesian position of body (currently only used for inv plane calculation) */
  double *dCartVel;      /**< Cartesian velocity of body (currently only used for inv plane calculation) */
  int iGravPerts;        /**< Number of bodies which perturb the body */
  int *iaGravPerts;      /**< Which bodies are perturbers of the body */
  int iEigFreqs;         /**< Number of eigenfrequencies that control the body's motion */
  int *iaEigFreqs;       /**< Indices of eigenfrequencies */
  int bGRCorr;           /**< Use general relativistic correction in DistOrb+DistRot (1=yes)*/
  int iDistOrbModel;     /**< Which orbital model to use (RD4 or LL2) */
  double dSemiPrev;      /**< Semi-major axis at which LL2 eigensolution was calc'd */
  double dEigenvalue; 
  double dEigenvector;
  int bEigenSet;
  double *dLOrb;
  double *dLOrbTmp;
  double dRPeri;
  double dRApo;

  /* BINARY parameters */
  int bBinary;          /** Apply BINARY module? */
  int bBinaryUseMatrix; /** Include eqns in matrix or solve for main variables on the fly? */
  double dR0;           /**< Guiding Radius,initially equal to dSemi */
  double dCBPR;         /** < CBP radius */
  double dCBPZ;         /** < CBP height above/below the orbital plane */
  double dCBPPhi;       /** < CBP azimuthal angle in orbital plane */
  double dCBPRDot;      /** < CBP radial orbital velocity */
  double dCBPZDot;      /** < CBP z orbital velocity */
  double dCBPPhiDot;    /** < CBP phi angular orbital velocity */
  double dFreeEcc;      /**< CBP's free eccentricity */
  double dFreeInc;      /**< CBP's free inclination, or binary's inclination */
  double dInc;          /**< CBP's actual inclication */
  double dLL13N0;       /**< CBP's Mean motion defined in LL13 eqn 12 */
  double dLL13K0;       /**< CBP's radial epicyclic frequency defined in LL13 eqn 26 */
  double dLL13V0;       /**< CBP's vertical epicyclic frequency defined in LL13 eqn 36 */
  double dLL13PhiAB;    /**< Binary's initial mean anomaly */
  double dCBPM0;        /**< CBP's initial mean anomaly */
  double dCBPZeta;      /**< CBP's z oscillation angle (see LL13 eqn 35) */
  double dCBPPsi;       /**< CBP's R, phi oscillation phase angle (see LL13 eqn 27) */

  /* DISTROT parameters */
  int bDistRot;
  double dPrecA;         /**< Precession angle */
  double dTrueApA;       /**< True anomaly at equinox (used for invariable plane conversion) */        
  double dDynEllip;      /**< Dynamical ellipticity */
  double dYobl;          /**< sin(obliq)*sin(preca) */
  double dXobl;          /**< sin(obliq)*cos(preca) */
  double dZobl;           /**< cos(obliq) */
  double *dLRot;
  double *dLRotTmp;
  int bForcePrecRate;
  double dPrecRate;
  int bCalcDynEllip;
  int bRelaxDynEllip;    /**< shape of planet relaxes when spun down */
  
  
  /* EQTIDE Parameters */
  int bEqtide;           /**< Apply Module EQTIDE? */
  int iTidePerts;        /**< Number of Tidal Perturbers */
  int *iaTidePerts;      /**< Body #'s of Tidal Perturbers */
  char saTidePerts[MAXARRAY][NAMELEN];  /**< Names of Tidal Perturbers */
  //char **saTidePerts;
  double dImK2;          /**< Imaginary part of Love's K_2 */
  double dTidalQ;	 /**< Body's Tidal Q */
  double dTidalTau;      /**< Body's Tidal Time Lag */
  //double dTidePower;   deprecated to allow communication with thermint
  double *dTidalZ;       /**< As Defined in \cite HellerEtal2011 */
  double *dTidalChi;     /**< As Defined in \cite HellerEtal2011 */
  double **dTidalF;      /**< As Defined in \cite HellerEtal2011 */
  double *dTidalBeta;    /**< As Defined in \cite HellerEtal2011 */
  int **iTidalEpsilon;   /**< Signs of Phase Lags */
  double dDeccDtEqtide;  /**< Eccentricity time rate of change */
  double *daDoblDtEqtide;  /**< Obliquity time rate of change */
  // XXX dRotRateDtEqtide???, dSemiDtEqtide?


  /* RADHEAT Parameters: H = Const*exp[-Time/HalfLife] */
  int bRadheat;             /**< Apply Module RADHEAT? */
  double d40KConstMan;      /**< Body's Mantle Potassium-40 Decay Constant */
  double d40KNumMan;        /**< Body's Mantle Number of Potassium-40 Atoms */
  double d40KPowerMan;      /**< Body's Mantle Internal Power Due to Potassium-40 Decay */
  double d40KMassMan;       /**< Body's Mantle Total Mass of Potassium-40 */
  double d40KConstCore;       
  double d40KNumCore;
  double d40KPowerCore;
  double d40KMassCore;
  double d232ThConstMan;    /**< Body's Thorium-232 Decay Constant */
  double d232ThNumMan;      /**< Body's Number of Thorium-232 Atoms */
  double d232ThPowerMan;    /**< Body's Internal Power Due to Thorium-232 Decay */
  double d232ThMassMan;     /**< Body's Total Mass of Thorium-232 Atoms */
  double d232ThConstCore;
  double d232ThNumCore;
  double d232ThPowerCore;
  double d232ThMassCore;
  double d238UConstMan;     /**< Body's Uranium-238 Decay Constant */
  double d238UNumMan;       /**< Body's Number of Uranium-238 Atoms */
  double d238UPowerMan;     /**< Body's Internal Power Due to Uranium-238 Decay */
  double d238UMassMan;      /**< Body's Total Mass of Uranium-238 Atoms */
  double d238UConstCore;
  double d238UNumCore;
  double d238UPowerCore;
  double d238UMassCore;
  double d235UConstMan; 
  double d235UNumMan;
  double d235UPowerMan;
  double d235UMassMan;
  double d235UConstCore; 
  double d235UNumCore;
  double d235UPowerCore;
  double d235UMassCore;
  double dRadPowerTotal;   /**< Total planet Radiogenic Power */
  double dRadPowerMan;   /**< Total Mantle Radiogenic Power */
  double dRadPowerCore;   /**< Total Core Radiogenic Power */
  double dRadPowerCrust;   /**< Total Crust Radiogenic Power */

  /* Thermint Parameters */
  int bThermint;           /**< Apply Module THERMINT? */
  double dTMan;            /**< Temperature Mantle AVE */
  double dTCore;           /**< Temperature Core AVE */
  double dTUMan;           /**< Temperature UMTBL */
  double dTLMan;           /**< Temperature LMTBL */
  double dTCMB;            /**< Temperature CMB */
  double dTICB;            /**< Temperature ICB */
  double dBLUMan;          /**< UM TBL thickness */
  double dBLLMan;          /**< LM TBL thickness */
  double dTJumpUMan;       /**< Abs Temperature Jump across UMTBL */
  double dTJumpLMan;       /**< Abs Temperature Jump across LMTBL */
  double dSignTJumpUMan;   /**< Sign of Temperature Jump across UMTBL */
  double dSignTJumpLMan;   /**< Sign of Temperature Jump across LMTBL */
  double dViscUManArr;     /**< Viscosity UMTBL Arrhenius Law */
  double dViscUMan;        /**< Viscosity UMTBL */
  double dViscLMan;        /**< Viscosity LMTBL */
  double dShmodUMan;       /**< Shear modulus UMTBL */
  double dShmodLMan;       /**< Shear modulus LMTBL */
  double dTsolUMan;        /**< Solidus Temperature UMTBL */
  double dTliqUMan;        /**< Liquidus Temperature UMTBL */
  double dTsolLMan;        /**< Solidus Temperature LMTBL */
  double dTliqLMan;        /**< Liquidus Temperature LMTBL */
  double dFMeltUMan;       /**< Melt fraction UMTBL */
  double dFMeltLMan;       /**< Melt fraction LMTBL */
  double dMeltfactorUMan;  /**< Melt Phase Factor for Rheology */
  double dMeltfactorLMan;  /**< Melt Phase Factor for Rheology */
  double dDepthMeltMan;    /**< Depth to base of UM Melt layer */
  double dTDepthMeltMan;   /**< Temp at base of UM Melt layer */
  double dTJumpMeltMan;    /**< Temp Jump to base of UM Melt layer */
  double dK2Man;           /**< Mantle k2 love number */
  double dImk2Man;         /**< Mantle Im(k2) love number */
  /* Time Derivatives & Gradients */
  double dTDotMan;         /**< Time deriv of mean mantle temp */
  double dTDotCore;        /**< time deriv of mean core temp */
  double dHfluxUMan;       /**< hflux upper mantle thermal boundary layer (UMTBL) */
  double dHflowUMan;       /**< hflow UMTBL */
  double dHfluxLMan;       /**< hflux lower mantle thermal boundary layer (UMTBL) */
  double dHflowLMan;       /**< hflow LMTBL */
  double dHfluxCMB;        /**< hflux CMB */
  double dHflowCMB;        /**< hflow CMB */
  double dHflowTidalMan;   /**< hflow tidal dissipation in mantle */
  double dHflowTidalCore;  /**< hflow tidal dissipation in core */
  double dHflowLatentMan;  /**< latent hflow from solidification of mantle */
  double dHflowMeltMan;    /**< Eruptive Melt Hflow from mantle */
  double dMassICDot;       /**< Mass Growth Rate of IC */
  double dHflowLatentIC;   /**< latent hflow from solidification of IC */
  double dPowerGravIC;     /**< latent hflow from solidification of IC */
  double dHflowICB;        /**< hflow across ICB */
  double dHfluxSurf;       /**< hflux surface of mantle */
  double dHflowSurf;       /**< hflow surface of mantle */
  double dTidalPowMan;     /**< Tidal Dissipation Power in Mantle */
  /* Core Variables */
  double dRIC;             /**< IC radius */
  double dDRICDTCMB;       /**< d(R_ic)/d(T_cmb) */
  double dDOC;             /**< OC shell thickness */
  double dThermConductOC;  /**< Thermal conductivity OC */
  double dThermConductIC;  /**< Thermal conductivity IC */
  double dChiOC;           /**< OC light element concentration chi. */
  double dChiIC;           /**< IC light element concentration chi. */
  double dMassOC;          /**< OC Mass. */
  double dMassIC;          /**< IC Mass. */
  double dMassChiOC;       /**< OC Chi Mass. */
  double dMassChiIC;       /**< IC Chi Mass. */
  double dDTChi;           /**< Core Liquidus Depression */
  /* Constants */
  double dViscRatioMan;    /**< Viscosity Ratio Man */
  double dEruptEff;        /**< Mantle melt eruption efficiency */
  double dViscRef;         /**< Mantle Viscosity Reference (coefficient) */
  double dTrefLind;         /**< Core Liquidus Lindemann Reference (coefficient) */
  double dDTChiRef;        /**< Core Liquidus Depression Reference (E) */
  
  /* ATMESC Parameters */
  int bAtmEsc;           /**< Apply Module ATMESC? */
  double dSurfaceWaterMass;
  double dMinSurfaceWaterMass;
  double dEnvelopeMass;
  double dMinEnvelopeMass;
  double dXFrac;
  double dAtmXAbsEff;

  /* STELLAR Parameters */
  int bStellar;
  double dLuminosity;
  double dTemperature;
  double dSatXUVFrac;
  double dSatXUVTime;
  double dXUVBeta;
  int iStellarModel;
  int iWindModel;
  int iXUVModel;
  double dLXUV; // Not really a STELLAR parameter

  /* PHOTOCHEM Parameters */
  PHOTOCHEM Photochem;   /**< Properties for PHOTOCHEM module N/I */
  double dNumAtmLayers;
  double dNumAtmMolecules;
  char saMoleculeList[MAXSPECIES][NAMELEN];
  double **daAtmConcentrations; /* TBA: OceanConcentration, IntConcentration */
  double dTropoHeight;
  double dAtmHeight;
  double dInsolation;   /* Orbit-averaged Insolation */
  double dSurfPressure;
  //double dSurfAlbedo;   /* Bolometric, ultimately will be array */
  int iResolveSeasons;  /* ISEASON in PHOTOCHEM.f */
  double dPhotoZenithAngle;
  int iVaryZenithAngle; /* IZYO2 in PHOTOCHEM.f */
  int iHiResGrid;       /* LGRID in PHOTOCHEM.f (default = 0, but if hi res., must change INO and IO2) */
  int iOxyAbsCoeffApprox; /* IO2 in PHOTOCHEM.f */
  int iNitroAbsCoeffApprox; /* INO in PHOTOCHEM.f */
  double dJacobianPerturbFact; /* EPSJ */
  int bLightning;
  double dLightningAmount;
  int iAerScattering; /* How do aerosols scatter? Mie or fractal. Must read files -- talk to Giada */
  int iMaxNumReactions; /* Total number of reactions possible for input species list */
  int iMaxWavelengthBin; /* Maximum number of wavelength bins -- not all species are created equal */
  int iNumPhotolysisRx; /* Number of photochemical reactions */
  int iNumPhotoSpecies;
  int iNumAqueousSpecies; /* Species that dissolve in rain --- must read file */
  int iML; /* =12; Shawn doesn't know what these are */
  int iML1; /* =ML+1 */
  int iML2; /* =2*ML */
  double daAtmTempProfile;
  double daAtmPressProfile;
  double ***daParticleInfo; /* First three dimensions are aerosol species, layer, and property, where property = number density, fall velocity, radius */

  double daEddyDiffProfile;

  int bAtmSulfur;
  int bAtmHydrogen;
  int bAtmOxygen;
  int bAtmCarbon;
  int bAtmNitrogen;
  int bAtmChlorine;

  /* CLIMA Parameters */
  double dArgonPressure;
  double dClimaZenithAngle;
  
  /* POISE parameters */
  int bPoise;                /**< Apply POISE module? */

  double dAlbedoGlobal;     /**< Global average albedo (Bond albedo) */
  double dAlbedoGlobalTmp;
  double dAlbedoLand;
  int iAlbedoType;            /**< type of water albedo used (fix or tay) */
  double dAlbedoWater;  
  int bAlbedoZA;             /**< Use albedo based on zenith angle */
  double dAstroDist;         /**< Distance between primary and planet */
  int bCalcAB;               /**< Calc A and B from Williams & Kasting 1997 */
  int bClimateModel;
  int bColdStart;            /**< Start from global glaciation (snowball state) conditions */
  double dCw_dt;
  double dDiffCoeff;         /**< Diffusion coefficient set by user */
  double dFixIceLat;         /**< Fixes ice line latitude to user set value */
  double dFluxInGlobal;      /**< Global mean of incoming flux */
  double dFluxInGlobalTmp;
  double dFluxOutGlobal;     /**< Global mean of outgoing flux */ 
  double dFluxOutGlobalTmp;
  double dFrzTSeaIce;         /**< Freezing temperature of sea water */
  int iGeography;
  int bHadley;               /**< Use Hadley circulation when calculating diffusion? */
  double dHeatCapAnn;        /**< Surface heat capacity in annual model */
  double dHeatCapLand;        /**< Heat capacity of land */
  double dHeatCapWater;       /**< Heat capacity of water */
  double dIceAlbedo;
  double dIceBalanceTot;
  double dIceDepRate;
  double dIceFlowTot;
  double dIceMassTot;
  int bIceSheets;
  int iIceTimeStep;
  double dInitIceHeight;
  double dInitIceLat;
  int bJormungand;           /**< Use with dFixIceLat to enforce cold equator conditions */
  double dLatentHeatIce;      /**< Latent heat of fusion of ice over mixing depth*/
  double dLatFHeatCp;         /**< Latent heat of ice/heat capacity */
  int bMEPDiff;              /**< Compute Diffusion from maximum entropy production (D = B/4) */
  double dMixingDepth;        /**< Depth of mixing layer of ocean (for thermal inertia)*/
  int iNDays;                /**< Number of days in planet's year */
  int iNStepInYear;        /**< Number of time steps in a year */  
  double dNuLandWater;        /**< Land-ocean interaction term */
  int iNumLats;              /**< Number of latitude cells */
  int iNumYears;           /**< Number of years to run seasonal model */
  double dpCO2;              /**< Partial pressure of CO2 in atmos only used if bCalcAB = 1 */
  double dPlanckA;           /**< Constant term in Blackbody linear approximation */
  double dPlanckB;           /**< Linear coeff in Blackbody linear approx (sensitivity) */
  double dPrecA0;            /**< Initial pA value used when distrot is not called */
  int iReRunSeas;
  double dSeaIceConduct;
  int bSeaIceModel;
  double dSeasDeltat;
  double dSeasDeltax;
  double dSeasOutputTime;
  double dSeasNextOutput;
  int bSkipSeas;
  int bSkipSeasEnabled;
  int bSnowball;
  double dSurfAlbedo;
  double dTGlobal;           /**< Global mean temperature at surface */
  double dTGlobalInit;
  double dTGlobalTmp;
  int iWriteLat;             /**< Stores index of latitude to be written in write function */

  /* Arrays used by seasonal and annual */
  double *daAnnualInsol;     /**< Annually averaged insolation at each latitude */
  double *daDivFlux;         /**< Divergence of surface flux */
  double *daDMidPt;
  double **daInsol;           /**< Daily insolation at each latitude */
  double *daFlux;            /**< Meridional surface heat flux */
  double *daFluxIn;          /**< Incoming surface flux (insolation) */
  double *daFluxOut;         /**< Outgoing surface flux (longwave) */
  double *daLats;            /**< Latitude of each cell (centered) */
  double *daTGrad;           /**< Gradient of temperature (meridional) */

  /* Arrays for annual model */
  double *daAlbedoAnn;          /**< Albedo of each cell */
  double *daDiffusionAnn;       /**< Diffusion coefficient of each latitude boundary */
  double **dMEulerAnn;
  double **dMEulerCopyAnn;
  double **dInvMAnn;
  double *daLambdaAnn;
  double **dMClim;
  double **dMDiffAnn;
  double *daPlanckAAnn;
  double *daPlanckBAnn;
  int *rowswapAnn;
  double *scaleAnn;
  double *daSourceF;
  double *daTempAnn;            /**< Surface temperature in each cell */
  double *daTempTerms;
  double *daTmpTempAnn;
  double *daTmpTempTerms;
  double *dUnitVAnn;

  /* Arrays for seasonal model */
  double *daAlbedoAvg;
  double *daAlbedoAvgL;
  double *daAlbedoAvgW;
  double *daAlbedoLand;
  double *daAlbedoLW;
  double *daAlbedoWater;
  double *daBasalFlow;        /**< basal flow d(u*h)/dy */
  double *daBasalFlowMid;     /**< basal flow d(u*h)/dy (midpoints) */
  double *daBasalVel;         /**< Basal velocity of ice */
  double *daBedrockH;         /**< Height of bedrock (can be negative) */
  double *daBedrockHEq;       /**< Equilibrium height of bedrock */
  double *daDeclination;           /**< Daily solar declination */
  double *daDeltaTempL;
  double *daDeltaTempW;       /**< Keep track of temperature change for energy check */
  double *daDIceHeightDy;
  double *daDiffusionSea;
  double *daDivFluxAvg;
  double **daDivFluxDaily;
  double *daEnergyResL;
  double *daEnergyResW;       /**< Energy residuals */
  double *daEnerResLAnn;
  double *daEnerResWAnn;      /**< Annually averaged energy residuals */
  double *daFluxAvg;
  double *daFluxOutAvg;
  double **daFluxDaily;
  double *daFluxInAvg;
  double **daFluxInDaily;
  double *daFluxInLand;
  double *daFluxInWater;
  double **daFluxOutDaily;
  double *daFluxOutLand;
  double *daFluxOutWater;
  double *daFluxSeaIce;
  double **daIceBalance;
  double *daIceBalanceAnnual;
  double *daIceBalanceAvg;
  double *daIceBalanceTmp;
  double *daIceFlow;
  double *daIceFlowAvg;
  double *daIceFlowMid;
  double *daIceGamTmp;
  double *daIceHeight; 
  double *daIceMass;
  double *daIceMassTmp;
  double *daIcePropsTmp;
  double *daIceSheetDiff;
  double **daIceSheetMat;
  double **dInvMSea;
  double *daLambdaSea;
  double *daLandFrac;         /**< Fraction of cell which is land */
  double **dMDiffSea;
  double **dMEulerCopySea;
  double **dMEulerSea;
  double **dMInit;
  double **dMLand;
  double **dMWater;
  double *daPlanckASea;
  double *daPlanckBSea;
  int *rowswapSea;
  double *scaleSea;
  double *daSeaIceHeight;     /**< Sea ice height by latitude */
  double *daSeaIceK;
  double *daSedShear;         /**< sediment shear stress (for ice sheets) */
  double *daSourceL;       /**< Land source function: PlanckA - (1-albedo)*Insolation */
  double *daSourceLW;     /**< Combined source function what matrix operates on */
  double *daSourceW;       /**< Water source function: PlanckA - (1-albedo)*Insolation */
  double *daTempAvg;
  double *daTempAvgL;
  double *daTempAvgW;
  double **daTempDaily;
  double *daTempLand;         /**< Temperature over land (by latitude) */
  double *daTempLW;            /**< Surface temperature in each cell (avg over land & water) */
  double *daTempWater;        /**< Temperature over ocean (by lat) */
  double *daTmpTempSea;
  double *dUnitVSea;
  double *daWaterFrac;        /**< Fraction of cell which is water */
  double *daXBoundary;
  double *daYBoundary;

  // FLARE
  int bFlare;
  double dFlareConst;
  double dFlareExp;
  double dLXUVFlare;
  
} BODY;

/* SYSTEM contains properties of the system that pertain to
   every BODY */

/* Pointer to Laplace semi-major axis functions in DistOrb */
typedef double (*fnLaplaceFunction)(double,int);

typedef struct {
  char cName[NAMELEN];	 /**< System's Name */
  double dTotAngMomInit; /**< System's Initial Angular Momentum */

  double dTotAngMom;     /**< System's Current Angular Momentum */
  
  /* DISTORB tools */
  fnLaplaceFunction **fnLaplaceF; /**< Pointers to semi-major axis functions for each pair of bodies */
  fnLaplaceFunction **fnLaplaceDeriv; /**< Pointers to semi-major axis derivatives for pair of bodies */
  double **dmLaplaceC;  /**< Values of semi-major axis functions for each pair of bodies */
  double **dmLaplaceD;  /**< Values of semi-major axis derivatives for each pair of bodies */
  double **dmAlpha0;  /**< Semi-major axis ratio for each pair of bodies, at the time LaplaceC is determined */
  int **imLaplaceN;   /**< Indices for dmLaplaceC corresponding to iBody, jBody */
  double dDfcrit;     /**< Semi-maj functions will be updated based on this value, set by user */
  double dThetaInvP;  /**< Azimuthal angle of invariable plane relative to input plane */
  double dPhiInvP;    /**< Altitude angle of invariable plane relative to input plane */
  double **dmEigenValEcc; /**< Matrix of eccentricity Eigenvalues in Laplace-Lagrange solution */
  double **dmEigenValInc; /**< Matrix of inclination Eigenvalues in Laplace-Lagrange solution */
  double **dmEigenVecEcc; /**< Matrix of eccentricity Eigenvectors in Laplace-Lagrange solution */
  double **dmEigenVecInc; /**< Matrix of inclination Eigenvectors in Laplace-Lagrange solution */
  double **dmEigenPhase; /**< Phase angles used in Laplace-Lagrange solution */
  double **A;
  double **B;
  double *Asoln;
  double *Bsoln;
  double **etmp;
  double **itmp;
  double *h0;
  double *k0;
  double *p0;
  double *q0;
  double *S;
  double *T;
  int *rowswap;
  double **Acopy;
  double *scale;
  double *dLOrb;
  
  double dTotEnInit;     /**< System's Initial Energy */

} SYSTEM;

/* 
 * Updates: Struct that contains all the variables to be updated and the functions to be called to be updated, fnUpdate. 
 */

typedef struct {  
  /* N.B. that pdVar points to the same memory location as
   * body.x, where x=semi, ecc, etc. */
  double **pdVar;       /**< Pointers to Primary Variables */
  int iNumVars;         /**< Number of Update-able Variables */

  /*! The "type" refers to how the variable is updated. If 0, then 
      the variable is assumed to be an explicit function of age. The 
      first timestep is then a bit dodgy as the rate is not initially
      known. The suggested timestep will be dEta*dTimestep, so runs 
      with a Type 0 variable must account for  the evolution with 
      dTimeStep. 
  */
  int **iaType;         /**< Variable type affecting timestep (0 = explicit function of age, 1 = normal quantity with time derivative, 2 = polar/sinusoidal quantity with time derivative, 3 = sinusoidal quantity with explicit function of age) */
  double *daDeriv;      /**< Array of Total Derivative Values for each Primary Variable */
  double **daDerivProc; /**< Array of Derivative Values Due to a Process */
  double *dVar;         
  double dZero;         /**< Sometimes you need a pointer to zero */

  /*! The body #s to calculate the derivative. First dimension is 
      the Primary Variable #, second is the process #, third is the 
      list body #s. 
  */
  int ***iaBody;        
  int **iNumBodies;     /**< Number of Bodies Affecting a Process */

  /* These keep track of the variable and modules */
  int iNumModules;      /**< Number of Modules Affecting a Body */
  int *iNumEqns;        /**< Number of Equations That Modify a Primary Variable */
  int *iaVar;           /**< Primary Variable # */
  /*! The Module # responsible for a given process. The first dimension 
    is the Primary Variable #. Second is the Equation. */
  int **iaModule;      


  /* Number of eqns to modify a parameter */
  int iNumRot;          /**< Number of Equations Affecting Rotation Rate */
  int iNumSemi;         /**< Number of Equations Affecting Semi-Major Axis */
  int iNumRadius;
  int iNumMass;

  /* These are the variables that the update matrix modifies */
  // Eccentricity is now split into Hecc and Kecc to accomodate Lagrange
  // Obliquity is now split into Xobl, Yobl and Zobl to accomedate Laskar
  int iRot;             /**< Variable # Corresponding to Rotation Rate */
  double dDRotDt;       /**< Total Rotation Rate Derivative */
  int iSemi;            /**< Variable # Corresponding to Semi-major Axis */
  double dDSemiDt;      /**< Total Semi-Major Axis Derivative */
  int iRadius;
  int iMass;

  /* Next comes the identifiers for the module that modifies a variable */

  /* EQTIDE */
  //  int iEccEqtide;       /**< Equation # Corresponding to EQTIDE's Change to Eccentricity */
  int iHeccEqtide;      /**< Equation # Corresponding to EQTIDE's Change to Poincare's h */
  int iKeccEqtide;      /**< Equation # Corresponding to EQTIDE's Change to Poincare's k */
  int *iaXoblEqtide;     /**< Equation #s Corresponding to EQTIDE's Change to Laskar's X */
  int *iaYoblEqtide;     /**< Equation #s Corresponding to EQTIDE's Change to Laskar's Y */
  int *iaZoblEqtide;     /**< Equation #s Corresponding to EQTIDE's Change to Laskar's Z */
  int *iaRotEqtide;     /**< Equation #s Corresponding to EQTIDE's Change to Rotation Rate */
  int iSemiEqtide;      /**< Equation # Corresponding to EQTIDE's Change to Semi-major Axis */

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

  /* RADHEAT */
  int i40KMan;             /**< Variable # Corresponding to Potassium-40 */
  int i232ThMan;           /**< Variable # Corresponding to Thorium-232 */
  int i238UMan;            /**< Variable # Corresponding to Uranium-238 */
  int i235UMan;
  int iNum40KMan;          /**< Number of Equations Affecting Potassium-40 [1] */
  int iNum232ThMan;        /**< Number of Equations Affecting Thorium-232 [1] */
  int iNum238UMan;         /**< Number of Equations Affecting Uranium-238 [1] */
  int iNum235UMan;
  double dD40KNumManDt;    /**< Total Potassium-40 Derivative */
  double dD232ThNumManDt;  /**< Total Thorium-232 Derivative */
  double dD238UNumManDt;   /**< Total Uranium-238 Derivative */
  double dD235UNumManDt; 
  double *pdD40KNumManDt;
  double *pdD232ThNumManDt;
  double *pdD238UNumManDt;
  double *pdD235UNumManDt;

  /* RADHEAT CORE */
  int i40KCore;
  int i232ThCore;
  int i238UCore;
  int i235UCore;
  int iNum40KCore;
  int iNum232ThCore;
  int iNum238UCore;
  int iNum235UCore; 
  double dD40KNumCoreDt;
  double dD232ThNumCoreDt;
  double dD238UNumCoreDt;
  double dD235UNumCoreDt; 
  double *pdD40KNumCoreDt;
  double *pdD232ThNumCoreDt;
  double *pdD238UNumCoreDt;
  double *pdD235UNumCoreDt;
  
  /* THERMINT */
  int iTMan;          /**< Variable # Corresponding to Tman */
  int iNumTMan;       /**< Number of Equations Affecting TMan */
  double dTDotMan;    /**< TMan time Derivative */
  double *pdTDotMan;
  int iTCore;          /**< Variable # Corresponding to Tman */
  int iNumTCore;       /**< Number of Equations Affecting TCore */
  double dTDotCore;    /**< TCore time Derivative */
  double *pdTDotCore;
  
  /* DISTORB */
  /* Number of eqns to modify a parameter */
  int iNumHecc;          /**< Number of Equations Affecting h = e*sin(longp) */
  int iNumKecc;          /**< Number of Equations Affecting k = e*cos(longp) */
  int iNumPinc;          /**< Number of Equations Affecting p = s*sin(longa) */
  int iNumQinc;         /**< Number of Equations Affecting q = s*cos(longa) */
  
  int iHecc;             /**< Variable # Corresponding to h = e*sin(longp) */
  double dDHeccDt;       /**< Total h Derivative */
  int iKecc;             /**< Variable # Corresponding to k = e*cos(longp) */
  double dDKeccDt;       /**< Total k Derivative */
  int iPinc;             /**< Variable # Corresponding to p = s*sin(longa) */
  double dDPincDt;       /**< Total p Derivative */
  int iQinc;             /**< Variable # Corresponding to q = s*cos(longa) */
  double dDQincDt;       /**< Total q Derivative */
  int *iaHeccDistOrb;       /**< Equation # Corresponding to DistOrb's change to h = e*sin(longp) */
  int *iaKeccDistOrb;     /**< Equation #s Corresponding to DistOrb's change to k = e*cos(longp) */
  int *iaPincDistOrb;     /**< Equation #s Corresponding to DistOrb's change to  p = s*sin(longa) */
  int *iaQincDistOrb;     /**< Equation #s Corresponding to DistOrb's change to  q = s*cos(longa) */
     
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
  int iNumXobl;          /**< Number of Equations Affecting x = sin(obl)*cos(pA) */
  int iNumYobl;          /**< Number of Equations Affecting y = sin(obl)*sin(pA) */
  int iNumZobl;          /**< Number of Equations Affecting z = cos(obl) */
  int iNumDynEllip;      /**< Number of Equations Affecting Dynamical Ellipticity */
  
  int iXobl;             /**< Variable # Corresponding to x = sin(obl)*cos(pA) */
  double dDXoblDt;       /**< Total x Derivative */
  int iYobl;             /**< Variable # Corresponding to y = sin(obl)*sin(pA) */
  double dDYoblDt;       /**< Total y Derivative */
  int iZobl;             /**< Variable # Corresponding to z = cos(obl) */
  double dDZoblDt;       /**< Total z Derivative */
  int iDynEllip;         /**< Variable # Corresponding to dynamical ellipticity */
  double dDDynEllipDt;   /**< Dynamical Ellipticity Derivative */
  int *iaXoblDistRot;     /**< Equation # Corresponding to DistRot's change to x = sin(obl)*cos(pA) */
  int *iaYoblDistRot;     /**< Equation #s Corresponding to DistRot's change to y = sin(obl)*sin(pA) */
  int *iaZoblDistRot;     /**< Equation #s Corresponding to DistRot's change to z = cos(obl) */

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the 
      xi = sin(obliq)*sin(pA) derivative due to DISTROT. */
  double **padDXoblDtDistRot;
  
  /*! Points to the element in UPDATE's daDerivProc matrix that contains the 
      zeta = sin(obliq)*cos(pA) derivative due to DISTROT. */
  double **padDYoblDtDistRot;
  
  /*! Points to the element in UPDATE's daDerivProc matrix that contains the 
      chi = cos(obliq) derivative due to DISTROT. */
  double **padDZoblDtDistRot;

  /* ATMESC */         
  int iSurfaceWaterMass;     /**< Variable # Corresponding to the surface water mass */
  int iNumSurfaceWaterMass;  /**< Number of Equations Affecting surface water [1] */
  int iEnvelopeMass;     /**< Variable # Corresponding to the envelope mass */
  int iNumEnvelopeMass;  /**< Number of Equations Affecting envelope mass [1] */
  
  /*! Points to the element in UPDATE's daDerivProc matrix that contains the 
      derivative of these variables due to ATMESC. */
  double *pdDSurfaceWaterMassDtAtmesc;
  double *pdDEnvelopeMassDtAtmesc;
  double *pdDMassDtAtmesc;

  /* BINARY */
  int iCBPR; /**< Variable # Corresponding to the CBP's orbital radius */
  int iNumCBPR; /**< Number of Equations Affecting CBP orbital radius [1] */  
  int iCBPZ; /**< Variable # corresponding to the CBP's cylindrical Z positions */
  int iNumCBPZ; /**< Number of Equations Affecting CBP cylindrical Z position [1] */
  int iCBPPhi; /**< Variable # Corresponding to the CBP's orbital azimuthal angle */
  int iNumCBPPhi; /**< NUmber of equations Affecting CBP orbital azimuthal angle [1] */
  int iCBPRDot; /**< Variable # Corresponding to the CBP's radial velocity */
  int iNumCBPRDot; /**< Number of equations affecting CBP radial velocity [1] */
  int iCBPZDot; /** < Variable # Corresponding to the CBP's Z orbital velocity */
  int iNumCBPZDot; /**< Number of equations affecting CBP z orbital velocity [1] */
  int iCBPPhiDot; /** < Variable # Corresponding to the CBP's Phi orbital angular velocity */
  int iNumCBPPhiDot; /**< Number of equations affecting CBP phi orbital velocity [1] */

  /* Points to the element in UPDATE's daDerivProc matrix that contains the 
   * derivative of these variables due to BINARY. */
  double *pdCBPRBinary; // Equation that governs CBP orbital radius
  double *pdCBPZBinary; // Equation that governs CBP cylindrical position Z
  double *pdCBPPhiBinary; // Equation that governs CBP orbital azimuthal angle
  double *pdCBPRDotBinary; // Equation that governs CBP radial orbital velocity
  double *pdCBPZDotBinary; // Equation that governs CBP z orbital velocity
  double *pdCBPPhiDotBinary; // Equation that governs CBP phi orbital velocity

  /* STELLAR */ 
  int iLuminosity;           /**< Variable # Corresponding to the luminosity */
  int iNumLuminosity;        /**< Number of Equations Affecting luminosity [1] */
  int iTemperature;
  int iNumTemperature;

  int iRotStellar;           /**< iEqn number for the evolution of rotation in STELLAR */
  
  /*! Points to the element in UPDATE's daDerivProc matrix that contains the 
      function that returns these variables due to STELLAR evolution. */
  double *pdLuminosityStellar;
  double *pdTemperatureStellar;
  double *pdRadiusStellar;
  
  double *pdRotRateStellar;

  /* POISE */
  int *iaIceMass;  /**< Variable number of ice mass of each latitude */
  int iNumIceMass; /**< Number of equations in Poise that affect each latitudes' ice */
  double ***padDIceMassDtPoise;
  int *iaIceMassDepMelt;
  int *iaIceMassFlow;
  int iIceMass;

  /* FLARE */
  int iLXUV;
  int iNumLXUV;
  double *pdDLXUVFlareDt;

} UPDATE;

typedef struct {
  int iNumHalts;       /**< Total Number of Halts */
  int bMerge;          /**< Halt for Merge? */
  double dMinSemi;     /**< Halt at this Semi-major Axis */
  double dMinObl;      /**< Halt at this Obliquity */
  double dMaxEcc;      /**< Halt at this Eccentricity */
  double dMinEcc;      /**< Halt at this Eccentricity */
  int bPosDeDt;        /**< Halt if Eccentricity Derivative is Positive */
  int dMinIntEn;       /**< Halt at this Internal Power */

  /* EQTIDE */
  int bDblSync;         /**< Halt if Double Synchronous? */
  int bTideLock;        /**< Halt if Tide-locked? */
  int bSync;            /**< Halt if Rotation Becomes Synchronous? */

  /* RADHEAT */
  double dMin40KPower;     /**< Halt at this Potassium-40 Power */
  double dMin232ThPower;   /**< Halt at this Thorium-232 Power */
  double dMin238UPower;    /**< Halt at this Uranium-238 Power */
  double dMin235UPower;
  double dMinRadPower;

  /* ATMESC */
  int bSurfaceDesiccated;         /**< Halt if dry?*/ 
  int bEnvelopeGone;              /**< Halt if evaporated?*/
  
  /* STELLAR */
  int bEndBaraffeGrid;            /***< Halt if we reached the end of the luminosity grid? */

  /* THERMINT */
  double dMinTMan;     /**< Halt at this TMan */
  double dMinTCore;     /**< Halt at this TCore */
  
  /* DISTORB */
  int bOverrideMaxEcc;  /**< 1 = tells DistOrb not to halt at maximum eccentricity = 0.6627434 */
  int bHillStab;       /**< halt if 2 planets fail Hill stability crit (technically valid for only 2 planets)*/
  int bCloseEnc;       /**< halt if any planet pair has orbits too close (crudely comparing inner's apocenter and outer's pericenter)*/

  /* POISE */
  int bHaltMinIceDt;  /**< Halt if ice flow time-step falls below a minimum value */
  int iMinIceDt;    /**< Halt if ice flow time-step falls below this value (number of orbital periods)*/

  /* BINARY */
  int bHaltHolmanUnstable; /** if CBP.dSemi < holman_crit_a, CBP dynamically unstable -> halt */
} HALT;

/* Units. These can be different for different bodies. If set
 * in the primary input file, the values are propogated to
 * all the bodies. Note that for most variables, an index of 0
 * corresponds to the first body read in. For units, that is not
 * the case, as the index refers to the file number, i.e. 0 to 
 * the primary input file, 1 to the first body read in, etc. This
 * feature allows for the units to be propogated to other files, 
 * but is sure to result in some bugs. Be careful!
 */

typedef struct {
  int iMass;          /**< 0=gm; 1=kg; 2=solar; 3=Earth; 4=Jup; 5=Nep */
  int iLength;        /**< 0=cm; 1=m; 2=km; 3=R_sun; 4=R_earth; 5=R_Jup; 6=AU */ 
  int iAngle;         /**< 0=rad; 1=deg */ 
  int iTime;          /**< 0=sec; 1=day; 2=yr; 3=Myr; 4=Gyr */
  int iTemp;
} UNITS;

typedef void (*fnPropsAuxModule)(BODY*,UPDATE*,int);
/* Note this hack -- the second int is for iEqtideModel. This may 
   have to be generalized for other modules. */
typedef void (*fnBodyCopyModule)(BODY*,BODY*,int,int,int);

/* Integration parameters */
typedef struct {
  int bDoForward;	 /**< Perform Forward Integration? */
  int bDoBackward;	 /**< Perform Backward Integration? */
  double dTime;          /**< Integration Time */
  double dEta;           /**< Variable Timestep Coefficient */
  double dStopTime;	 /**< Integration Stop Time */
  double dTimeStep;	 /**< Integration Time step */
  int bVarDt;            /**< Use Variable Timestep? */
  int nSteps;            /**< Number of Steps Since Last Output */
  double dMinValue;      /**< Minimum Value for Eccentricity and Obliquity to be Integrated */
  int bFirstStep;        /**< Has the First Dtep Been Taken? */
  int iNumBodies;        /**< Number of Bodies to be Integrated */
  int iOneStep;          /**< Integration Method # */
  double dCurrentDt;

  // These are to store midpoint derivative info in RK4.
  BODY *tmpBody;         /**< Temporary BODY struct */
  UPDATE *tmpUpdate;     /**< Temporary UPDATE struct */
  double ***daDeriv;     /**< The Matrix of Time Derivatives. First dimension is Body #, second is the Primary Variable #, third is the Equation #.  */

  // Module-specific parameters
  int *iNumModules;      /**< Number of Modules per Primary Variable */
  int *iNumMultiProps;   /**< Number of Multi-module PropsAux functions */

  /* EQTIDE */
  int iEqtideModel;      /**< EQTIDE Model # */
  int bDiscreteRot;	 /**< Use Discrete Rotation Model (CPL)? */
  int *bForceEqSpin;     /**< Force Rotation Rate to be Equilibrium? */
  int *bFixOrbit;        /**< Fix Orbit? */
  double *dMaxLockDiff;  /**< Fractional Difference from Tidal Equilibrium Rate to Force Equilibrium. */
  double *dSyncEcc;     

  /* RADHEAT */
  /* Nothing? */

  /* DISTORB */
  int iDistOrbModel;
  
  fnPropsAuxModule **fnPropsAux; /**< Function Pointers to Auxiliary Properties */
  fnPropsAuxModule **fnPropsAuxMulti;  /**< Function pointers to Auxiliary Properties for multi-module interdependancies. */
  fnBodyCopyModule **fnBodyCopy; /**< Function Pointers to Body Copy */
} EVOLVE;

/* The CONTROL struct contains all the parameters that
 * control program flow. */

typedef struct {
  int iVerbose;           /**< Verbosity Level. 0=none; 1=error; 2=progress; 3=input; 4=units; 5=all */
  double dOutputTime;	  /**< Integration Output Interval */

  int bLog;               /**< Write Log File? */

  /* Output Notation */
  int iDigits;            /**< Number of Digits After Decimal */
  int iSciNot;            /**< Crossover Decade to Switch between Standard and Scientific Notation */

  int bOverwrite;         /**< Allow files to be overwritten? */
} IO;
  
/* The CONTROL struct contains all the parameters that
 * control program flow. */
/* CONTROL contains all parameters that control program flow, including I/O,
   halts, units, and the integration, including manipulating the UPDATE
   matrix through fnForceBehavior. */

typedef double (*fnUpdateVariable)(BODY*,SYSTEM*,int*);
typedef void (*fnForceBehaviorModule)(BODY*,EVOLVE*,IO*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
/* HALT struct contains all stopping conditions, other than reaching the end
   of the integration. */

typedef int (*fnHaltModule)(BODY*,EVOLVE*,HALT*,IO*,UPDATE*,int);

typedef struct {
  EVOLVE Evolve;
  HALT *Halt;
  IO Io;
  UNITS *Units;

  /* Move to BODY */
  int *iMassRad;           /**< Mass-Radius Relationship */

  fnHaltModule **fnHalt;   /**< Function Pointers to Halt Checks */
  fnForceBehaviorModule **fnForceBehavior; /**< Function Pointers to Force Behaviors */
  fnForceBehaviorModule **fnForceBehaviorMulti; /**< Function Pointers to Force Behaviors */
  int *iNumMultiForce;    /**< Number of multi-module ForceBahevior functions */

  /* Things for DistOrb */
  double dAngNum;         /**< Value used in calculating timestep from angle variable */
  int bSemiMajChange;         /**< 1 if semi-major axis can change (DistOrb will recalc Laplace coeff functions) */
  int bInvPlane;       /**< 1 = change input coordinates to invariable plane coordinate */
  int bOutputLapl;     /**< 1 = output laplace functions and related data */
} CONTROL;

/* The INFILE struct contains all the information 
 * regarding the files that read in. */

typedef struct {
  char cIn[NAMELEN];       /**< File Name */
  int *bLineOK;            /**< Line # Format OK? */
  int iNumLines;           /**< Number of Input Lines */
  /* Species file for PHOTOCHEM */
  char cSpecies[NAMELEN];  /**< Name of Chemical Species N/I */
  /* Reaction file for PHOTOCHEM */
  char cReactions[NAMELEN]; /**< Names of Chemical Reactions N/I */

  /* Aerosol scattering files */
  /* Aqueous file -- add to SpeciesFile? */
  /* Array of Vapor pressure file */

} INFILE;

/* The OUTFILE struct contains all the information 
 * regarding the output files. */

typedef struct {
  char cOut[NAMELEN];       /**< Output File Name */
  int iNumCols;             /**< Number of Columns in Output File */
  char caCol[NUMOUT][OPTLEN];  /**< Output Value Name */
  int bNeg[NUMOUT];         /**< Use Negative Option Units? */
  int iNumGrid;             /**< Number of grid outputs */
  char caGrid[NUMOUT][OPTLEN];  /**< Gridded output name */
} OUTFILE;


/* The FILES struct contains all the information 
 * regarding every file. */

typedef struct {
  char cExe[LINE];             /**< Name of Executable */
  OUTFILE *Outfile;            /**< Output File Name for Forward Integration */
  char cLog[NAMELEN];          /**< Log File Name */
  INFILE *Infile;              
  int iNumInputs;              /**< Number of Input Files */
} FILES;

/* The OPTIONS struct contains all the information
 * regarding the options, including their file data. */

typedef struct {
  char cName[OPTLEN];          /**< Option Name */
  char cDescr[OPTDESCR];       /**< Brief Description of Option */
  int iType;                   /**< Cast of input. 0=bool; 1=int; 2=double; 3=string; +10 for array. */
  char cDefault[OPTDESCR];     /**< Description of Default Value */
  double dDefault;             /**< Default Value */
  int iMultiFile;              /**< Option Permitted in Multiple Inpute Files?  (b?) */
  int iMultiIn;
  int *iLine;                  /**< Option's Line # in Input File */ 
  char *iFile;
  char cFile[MAXFILES][OPTLEN]; /**< File Name Where Set */
  char cNeg[OPTDESCR];         /**< Description of Negative Unit Conversion */
  double dNeg;                 /**< Conversion Factor to System Units */
} OPTIONS;

/* OUTPUT contains the data regarding every output parameters */

/* Some output variables must combine output from different modules.
 * These functions do that combining. */
 
typedef double (*fnOutputModule)(BODY*,SYSTEM*,UPDATE*,int,int);

/* GRIDOUTPUT will be part of OPTIONS, and contains data for latitudinal parameters in POISE */
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
 
typedef struct {
  char cName[OPTLEN];    /**< Output Name */
  char cDescr[LINE];     /**< Output Description */
  int bNeg;              /**< Is There a Negative Option? */
  int iModuleBit;              /**< Bit flag for module to check output parameters */
  int *bDoNeg;           /**< Should the Output use "Negative" Units? */
  char cNeg[NAMELEN];    /**< Units of Negative Option */
  double dNeg;           /**< Conversion Factor for Negative Option */
  int iNum;              /**< Number of Columns for Output */
  int bGrid;             /**< Is output quantity gridded (e.g. a function of latitude)? */

//   GRIDOUTPUT *GridOutput;     /**< Output for latitudinal climate params, etc */
  /* Now add vector output functions */
  fnOutputModule **fnOutput; /**< Function Pointers to Write Output */

} OUTPUT;

typedef void (*fnReadOption)(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,int);
typedef void (*fnWriteOutput)(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UNITS*,UPDATE*,int,double *,char []);


/*
 * Module-level function pointers
 */

typedef void (*fnInitializeOptions)(OPTIONS*,fnReadOption*);
typedef void (*fnInitializeBodyModule)(BODY*,CONTROL*,UPDATE*,int,int);
typedef void (*fnInitializeControlModule)(CONTROL*);
typedef void (*fnInitializeOptionsModule)(OPTIONS*,fnReadOption*);
typedef void (*fnInitializeUpdateModule)(BODY*,UPDATE*,int);
typedef void (*fnInitializeUpdateTmpBodyModule)(BODY*,CONTROL*,UPDATE*,int);

//All primary variables need a FinalizeUpdate function
//typedef void (*fnFinalizeUpdateEccModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdate40KNumCoreModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdate40KNumManModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdate232ThNumCoreModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdate232ThNumManModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdate235UNumCoreModule)(BODY*,UPDATE*,int*,int,int,int); 
typedef void (*fnFinalizeUpdate235UNumManModule)(BODY*,UPDATE*,int*,int,int,int);  
typedef void (*fnFinalizeUpdate238UNumCoreModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdate238UNumManModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateCBPRModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateCBPZModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateCBPRDotModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateCBPPhiModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateCBPZDotModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateCBPPhiDotModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateDynEllipModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateEnvelopeMassModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateHeccModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateIceMassModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateKeccModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateLuminosityModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateLXUVModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateMassModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdatePincModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateQincModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateRadiusModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateRotModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateSemiModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateSurfaceWaterMassModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateTemperatureModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateTCoreModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateTManModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateXoblModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateYoblModule)(BODY*,UPDATE*,int*,int,int,int);
typedef void (*fnFinalizeUpdateZoblModule)(BODY*,UPDATE*,int*,int,int,int);

typedef void (*fnReadOptionsModule)(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption*,int);
typedef void (*fnVerifyModule)(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
typedef void (*fnVerifyHaltModule)(BODY*,CONTROL*,OPTIONS*,int,int*);
typedef void (*fnCountHaltsModule)(HALT*,int*);
typedef void (*fnInitializeOutputModule)(OUTPUT*,fnWriteOutput*);
typedef void (*fnLogBodyModule)(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput*,FILE*,int);
typedef void (*fnLogModule)(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput*,FILE*);
typedef void (*fnInitializeOutputFunctionModule)(OUTPUT*,int,int);
typedef void (*fnFinalizeOutputFunctionModule)(OUTPUT*,int,int);

typedef struct {
  int *iNumModules; /**< Number of Modules per Body */
  int **iaModule; /**< Module #s that Apply to the Body */
  int *iBitSum;

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
  /*! Function pointers to finalize Core's potassium-40 */ 
  fnFinalizeUpdate40KNumCoreModule **fnFinalizeUpdate40KNumCore;
  /*! Function pointers to finalize Mantle's potassium-40 */ 
  fnFinalizeUpdate40KNumManModule **fnFinalizeUpdate40KNumMan;
  /*! Function pointers to finalize Core's thorium-232 */ 
  fnFinalizeUpdate232ThNumManModule **fnFinalizeUpdate232ThNumMan;
  /*! Function pointers to finalize Mantle's thorium-232 */ 
  fnFinalizeUpdate232ThNumCoreModule **fnFinalizeUpdate232ThNumCore;
  /*! Function pointers to finalize Core's uranium-235 */ 
  fnFinalizeUpdate235UNumCoreModule **fnFinalizeUpdate235UNumCore;
 
  /*! Function pointers to finalize Mantle's uranium-235 */ 
  fnFinalizeUpdate235UNumManModule **fnFinalizeUpdate235UNumMan;  
  /*! Function pointers to finalize Core's uranium-238 */ 
  fnFinalizeUpdate238UNumCoreModule **fnFinalizeUpdate238UNumCore;
  /*! Function pointers to finalize Mantle's uranium-238 */ 
  fnFinalizeUpdate238UNumManModule **fnFinalizeUpdate238UNumMan;
  
  
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
  /*! Function pointers to finalize Mass */ 
  fnFinalizeUpdateMassModule **fnFinalizeUpdateMass; 
  /*! Function pointers to finalize Rotation Rate */ 
  fnFinalizeUpdateRotModule **fnFinalizeUpdateRot;
  /*! Function pointers to finalize Semi-major Axis */ 
  fnFinalizeUpdateSemiModule **fnFinalizeUpdateSemi;
  /*! Function pointers to finalize Surface Water */ 
  fnFinalizeUpdateSurfaceWaterMassModule **fnFinalizeUpdateSurfaceWaterMass;
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

  fnFinalizeUpdateIceMassModule **fnFinalizeUpdateIceMass;
  fnFinalizeUpdateLXUVModule **fnFinalizeUpdateLXUV;
 
  /*! These functions log module-specific data. */ 
  fnLogBodyModule **fnLogBody;

  /*! These functions read module-specific option. */ 
  fnReadOptionsModule **fnReadOptions;

  /*! These functions verify module-specific options. */ 
  fnVerifyModule **fnVerify;

  /*! These functions verify module-specific halts. */ 
  fnVerifyHaltModule **fnVerifyHalt;

  /*! These functions adds subroutines to the output functions that require
      module-specific values. */ 
  fnFinalizeOutputFunctionModule **fnFinalizeOutputFunction;
  
  
} MODULE;

/* fnIntegrate is a pointer to a function that performs 
 * integration. */
typedef void (*fnIntegrate)(BODY*,CONTROL*,SYSTEM*,UPDATE*,fnUpdateVariable***,double*,int);


/* 
 * Other Header Files - These are primarily for function declarations
 */

#include <assert.h>

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
#include "eqtide.h"
#include "radheat.h"
#include "atmesc.h"
#include "stellar.h"
#include "baraffe2015.h"
#include "distorb.h"
#include "thermint.h"
#include "distrot.h"
#include "poise.h"
#include "binary.h"
#include "flare.h"

/* Do this stuff with a few functions and some global variables? XXX */

/* This needs to be vectorized VPL 

#define MODULEOPTEND        EQTIDEOPTEND
#define MODULEOUTEND        EQTIDEOUTEND
#define MODULEHALTSYSEND    EQTIDEHALTSYSEND
#define MODULEHALTBODYEND   EQTIDEHALTBODYEND
*/

/********************
 * ADJUST AS NEEDED *       XXX And fix sometime!
 ********************/

// XXX Obsolete?
// Note: not obsolete! needed for new module
// Otherwise,segfaults
// Increased from 1900->2100 for binary
#define MODULEOPTEND        2100
#define MODULEOUTEND        2100


