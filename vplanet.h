/******************** VPLANET.H ***********************/
/*
 * Rory Barnes, Wed May  7 16:04:27 PDT 2014
 *
 */

 
/*! Top-level declarations */

/* How many modules are available? */
#define EQTIDE        0
#define RADHEAT       1
#define ATMESC        2
#define LAGRANGE      3
#define LASKAR        4
#define STELLAR       5
#define DYNAMO        6
#define THERMAL       7
#define EBM           8

/* Fundamental constants */

#define BIGG          6.672e-11
#define PI            3.1415926535

/* Units: Calculations are done in SI */

#define MEARTH        5.9742e24
#define MSUN          1.98892e30
#define AUCM          1.49598e11
#define RSUN          6.955e8
#define YEARSEC       3.15576e7
#define DAYSEC        86400
#define REARTH        6.3781e6
#define RJUP          7.1492e7
#define MJUP          1.8987e27
#define RNEP          2.4764e7
#define MNEP          1.0244e26
#define RHOEARTH      5515
#define eEARTH        0.016710219
#define YEARDAY       365.25
#define MSAT          5.6851e26
#define DEGRAD        0.017453292519444445
#define ATOMMASS      1.660538921e-27

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

#define NUMOUT        2000  /* Number of output parameters */
#define MAXBODIES     2
#define OPTLEN        24    /* Maximum length of an option */
#define OPTDESCR      64    /* Number of characters in option
			     * description */
#define LINE          128   /* Maximum number of characters 
			     * in a line */
#define NAMELEN       24

#define MAXFILES      24    /* Maximum number of input files */
#define MAXARRAY      64    /* Maximum number of options in 
			     * an option array */
#define NUMOPT	      1000  /* Number of options that could be 
			     * in MODULE */
#define MAXLINES      256   /* Maximum Number of Lines in an 
			     * input file */
#define OPTEND        1100  /* Last output number of module options
			     * EQTIDE is highest for this compiltion */

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

// RADHEAT
#define VNUM40KMAN      1101
#define VNUM232THMAN    1102
#define VNUM238UMAN     1103
#define VNUM235UMAN     1104
#define VNUM40KCORE     1105
#define VNUM232THCORE   1106
#define VNUM238UCORE    1107
#define VNUM235UCORE    1108

// ATMESC
#define VSURFACEWATERMASS  1202

// STELLAR
#define VLUMINOSITY     1502

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

typedef struct {
  char cName[NAMELEN];   /**< Body's Name */
  char cType[OPTLEN];    /**< Type of object N/I */

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

  double *daSED;         /**< Body's spectral energy distribution by wavelength N/I */

  /* Orbital Properties. By convention, these are stored in the
   * second element in the BODY array.   */
  double dSemi;          /**< Body's Semi-major Axis */
  double dEcc;           /**< Body's Eccentricity */
  double dMeanMotion;    /**< Body's Mean Motion */
  double dOrbPeriod;     /**< Body's Orbital Period */
  double dEccSq;         /**< Eccentricity squared */

  /* LAGRANGE parameters */
  int bLagrange;         /**< Has module LAGRANGE been implemented */ 
  double dHEcc;           /**< Poincare H */
  double dKEcc;           /**< Poincare K */
  double dPInc;           /**< Poincare P */
  double dQInc;           /**< Poincare Q */

  /* LASKAR parameters */
  int bLaskar;

  /* EQTIDE Parameters */
  int bEqtide;           /**< Apply Module EQTIDE? */
  int iTidePerts;        /**< Number of Tidal Perturbers */
  int *iaTidePerts;      /**< Body #'s of Tidal Perturbers */
  char saTidePerts[MAXARRAY][NAMELEN];  /**< Names of Tidal Perturbers */
  //char **saTidePerts;
  double dTidalQ;	 /**< Boyd's Tidal Q */
  double dTidalTau;      /**< Body's Tidal Time Lag */
  double *dTidalZ;       /**< As Defined in \cite HellerEtal2011 */
  double *dTidalChi;     /**< As Defined in \cite HellerEtal2011 */
  double **dTidalF;      /**< As Defined in \cite HellerEtal2011 */
  double *dTidalBeta;    /**< As Defined in \cite HellerEtal2011 */
  int **iTidalEpsilon;   /**< Signs of Phase Lags */

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

  int bThermint;

  /* ATMESC Parameters */
  int bAtmEsc;           /**< Apply Module ATMESC? */
  double dSurfaceWaterMass;
  double dXFrac;
  double dAtmXAbsEff;

  /* STELLAR Parameters */
  int bStellar;
  double dLuminosity;
  double dLXUV;
  double dSatXUVFrac;

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
  double dSurfAlbedo;   /* Bolometric, ultimately will be array */
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

} BODY;

/* SYSTEM contains properties of the system that pertain to
   every BODY */

typedef struct {
  char cName[NAMELEN];	 /**< System's Name */
  double dTotAngMomInit; /**< System's Initial Angular Momentum */
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
  int **iaType;        
  double *daDeriv;      /**< Array of Total Derivative Values for each Primary Variable */
  double **daDerivProc; /**< Array of Derivative Values Due to a Process */
  double *dVar;         

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
  int iNumEcc;          /**< Number of Equations Affecting Eccentricity */
  int iNumObl;          /**< Number of Equations Affecting Obliquity */
  int iNumRot;          /**< Number of Equations Affecting Rotation Rate */
  int iNumSemi;         /**< Number of Equations Affecting Semi-Major Axis */

  /* These are the variables that the update matrix modifies */
  int iEcc;             /**< Variable # Corresponding to Eccentricity */ 
  double dDEccDt;       /**< Total Eccentricity Derivative */
  int iObl;             /**< Variable # Corresponding to Obliquity */
  double dDOblDt;       /**< Total Obliquity Derivative */
  int iRot;             /**< Variable # Corresponding to Rotation Rate */
  double dDRotDt;       /**< Total Rotation Rate Derivative */
  int iSemi;            /**< Variable # Corresponding to Semi-major Axis */
  double dDSemiDt;      /**< Total Semi-Major Axis Derivative */

  /* Next comes the identifiers for the module that modifies a variable */

  /* EQTIDE */
  int iEccEqtide;       /**< Equation # Corresponding to EQTIDE's Change to Eccentricity */
  int *iaOblEqtide;     /**< Equation #s Corresponding to EQTIDE's Change to Obliquity */
  int *iaRotEqtide;     /**< Equation #s Corresponding to EQTIDE's Change to Rotation Rate */
  int iSemiEqtide;      /**< Equation # Corresponding to EQTIDE's Change to Semi-major Axis */

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the 
      semi-major axis' derivative due to EQTIDE. */
  double *pdDsemiDtEqtide; 

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the 
      eccentricity' derivative due to EQTIDE. */
  double *pdDeccDtEqtide;

  /*! Points to the elements in UPDATE's daDerivProc matrix that contains the 
      obliquities' derivatives due to EQTIDE. */
  double **padDoblDtEqtide;

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

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the 
      potassium-40's derivative due to RADHEAT. */
  double *pdD40KNumDt;  

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the 
      thorium-232's derivative due to RADHEAT. */
  double *pdD232ThNumDt;

  /*! Points to the element in UPDATE's daDerivProc matrix that contains the 
      uranium-40's derivative due to RADHEAT. */
  double *pdD238UNumDt;

  /* ATMESC */         
  int iSurfaceWaterMass;     /**< Variable # Corresponding to the surface water mass */
  int iNumSurfaceWaterMass;  /**< Number of Equations Affecting surface water [1] */
  
  /*! Points to the element in UPDATE's daDerivProc matrix that contains the 
      derivative of these variables due to ATMESC. */
  double *pdDSurfaceWaterMassDt;

  /* STELLAR */ 
  int iLuminosity;           /**< Variable # Corresponding to the luminosity */
  int iNumLuminosity;        /**< Number of Equations Affecting luminosity [1] */
  
  /*! Points to the element in UPDATE's daDerivProc matrix that contains the 
      derivative of these variables due to ATMESC. */
  double *pdLuminosity;

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
  int dMin40KPower;     /**< Halt at this Potassium-40 Power */
  int dMin232ThPower;   /**< Halt at this Thorium-232 Power */
  int dMin238UPower;    /**< Halt at this Uranium-238 Power */
  int dMin235UPower;

  /* ATMESC */
  int bSurfaceDesiccated;         /**< Halt if dry?*/ 
  double dMinSurfaceWaterMass;
  
  /* STELLAR */
  // Nothing

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
} UNITS;

typedef void (*fnAuxPropsModule)(BODY*,UPDATE*,int);
/* Note this hack -- the second int is for iEqtideModel. This may 
   have to be generalized for other modules. */
typedef void (*fnBodyCopyModule)(BODY*,BODY*,int,int);

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

  // These are to store midpoint derivative info in RK4.
  BODY *tmpBody;         /**< Temporary BODY struct */
  UPDATE *tmpUpdate;     /**< Temporary UPDATE struct */
  double ***daDeriv;     /**< The Matrix of Time Derivatives. First dimension is Body #, second is the Primary Variable #, third is the Equation #.  */

  // Module-specific parameters
  int *iNumModules;      /**< Number of Modules per Primary Variable */
  int *iNumMulti;        /**< Number of Multi-module AuxProps functions */

  /* EQTIDE */
  int iEqtideModel;      /**< EQTIDE Model # */
  int bDiscreteRot;	 /**< Use Discrete Rotation Model (CPL)? */
  int *bForceEqSpin;     /**< Force Rotation Rate to be Equilibrium? */
  double *dMaxLockDiff;  /**< Fractional Difference from Tidal Equilibrium Rate to Force Equilibrium. */
  double *dSyncEcc;     

  /* RADHEAT */
  /* Nothing? */

  fnAuxPropsModule **fnAuxProps; /**< Function Pointers to Auxiliary Properties */
  fnAuxPropsModule **fnAuxPropsMulti;  /**< Function pointers to Auxiliary Properties for multi-module interdependancies. */
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

typedef void (*fnForceBehaviorModule)(BODY*,EVOLVE*,IO*,int,int);
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
  char cDefault[OPTLEN];       /**< Description of Default Value */
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

typedef struct {
  char cName[OPTLEN];    /**< Output Name */
  char cDescr[LINE];     /**< Output Description */
  int bNeg;              /**< Is There a Negative Option? */
  int *bDoNeg;           /**< Should the Output use "Negative" Units? */
  char cNeg[NAMELEN];    /**< Units of Negative Option */
  double dNeg;           /**< Conversion Factor for Negative Option */
  int iNum;              /**< Number of Columns for Output */

  /* Now add vector output functions */
  fnOutputModule **fnOutput; /**< Function Pointers to Write Output */

} OUTPUT;


typedef double (*fnUpdateVariable)(BODY*,SYSTEM*,int*,int);
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

typedef void (*fnFinalizeUpdateEccModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdateNumIsotopeModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdateOblModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdateRotModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdateSemiModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdate40KNumManModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdate232ThNumManModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdate238UNumManModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdate235UNumManModule)(BODY*,UPDATE*,int*,int,int);  
typedef void (*fnFinalizeUpdate40KNumCoreModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdate232ThNumCoreModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdate238UNumCoreModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdate235UNumCoreModule)(BODY*,UPDATE*,int*,int,int); 
typedef void (*fnFinalizeUpdateSurfaceWaterMassModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdateLuminosityModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnReadOptionsModule)(BODY*,CONTROL*,FILES*,OPTIONS*,SYSTEM*,fnReadOption*,int);
typedef void (*fnVerifyModule)(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);
typedef void (*fnVerifyHaltModule)(BODY*,CONTROL*,OPTIONS*,int,int*);
typedef void (*fnVerifyRotationModule)(BODY*,CONTROL*,OPTIONS*,char[],int);
typedef void (*fnCountHaltsModule)(HALT*,int*);
typedef void (*fnInitializeOutputModule)(OUTPUT*,fnWriteOutput*);
typedef void (*fnLogBodyModule)(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput*,FILE*,int);
typedef void (*fnLogModule)(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput*,FILE*);
typedef void (*fnInitializeOutputFunctionModule)(OUTPUT*,int,int);
typedef void (*fnFinalizeOutputFunctionModule)(OUTPUT*,int,int);

typedef struct {
  int *iNumModules; /**< Number of Modules per Body */
  int **iaModule; /**< Module #s that Apply to the Body */

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

  /*! These functions assign Equation and Module information regarding 
      eccentricity in the UPDATE struct. */ 
  fnFinalizeUpdateEccModule **fnFinalizeUpdateEcc;

  /*! These functions assign Equation and Module information regarding 
      obliquity in the UPDATE struct. */ 
  fnFinalizeUpdateOblModule **fnFinalizeUpdateObl;

  /*! These functions assign Equation and Module information regarding 
      rotation rate in the UPDATE struct. */ 
  fnFinalizeUpdateRotModule **fnFinalizeUpdateRot;

  /*! These functions assign Equation and Module information regarding 
      semi-major axis in the UPDATE struct. */ 
  fnFinalizeUpdateSemiModule **fnFinalizeUpdateSemi;

  /*! These functions assign Equation and Module information regarding 
  potassium-40 in the UPDATE struct. */ 
  fnFinalizeUpdate40KNumManModule **fnFinalizeUpdate40KNumMan;
  /*! These functions assign Equation and Module information regarding 
    thorium-232 in the UPDATE struct. */ 
  fnFinalizeUpdate232ThNumManModule **fnFinalizeUpdate232ThNumMan;
  /*! These functions assign Equation and Module information regarding 
    uranium-238 in the UPDATE struct. */ 

  fnFinalizeUpdate238UNumManModule **fnFinalizeUpdate238UNumMan;
  fnFinalizeUpdate235UNumManModule **fnFinalizeUpdate235UNumMan;  
  fnFinalizeUpdate40KNumCoreModule **fnFinalizeUpdate40KNumCore;
  fnFinalizeUpdate232ThNumCoreModule **fnFinalizeUpdate232ThNumCore;
  fnFinalizeUpdate238UNumCoreModule **fnFinalizeUpdate238UNumCore;
  fnFinalizeUpdate235UNumCoreModule **fnFinalizeUpdate235UNumCore;
  fnFinalizeUpdateSurfaceWaterMassModule **fnFinalizeUpdateSurfaceWaterMass;
  fnFinalizeUpdateLuminosityModule **fnFinalizeUpdateLuminosity;
    
  /*! These functions log module-specific data. */ 
  fnLogBodyModule **fnLogBody;

  /*! These functions read module-specific option. */ 
  fnReadOptionsModule **fnReadOptions;

  /*! These functions verify module-specific options. */ 
  fnVerifyModule **fnVerify;

  /*! These functions verify module-specific halts. */ 
  fnVerifyHaltModule **fnVerifyHalt;

  /*! These functions verify module-specific constraints on rotation rate. */ 
  fnVerifyRotationModule **fnVerifyRotation;

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

#define MODULEOPTEND        1900
#define MODULEOUTEND        1900

