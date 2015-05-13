/******************** VPLANET.H ***********************/
/*
 * Rory Barnes, Wed May  7 16:04:27 PDT 2014
 *
 */

/* How many modules are available? */
#define NUMMODULES    2
#define EQTIDE        0
#define RADHEAT       1

/* Fundamental constants */

#define BIGG          6.672e-8
#define PI            3.1415926535

/* Units */

#define MEARTH        5.9742e27
#define MSUN          1.98892e33
#define AUCM          1.49598e13
#define RSUN          6.955e10
#define YEARSEC       3.15576e7
#define DAYSEC        86400
#define REARTH        6.3781e8
#define RJUP          7.1492e9
#define MJUP          1.8987e30
#define RNEP          2.4764e9
#define MNEP          1.0244e29
#define RHOEARTH      5.52                  // cgs
#define eEARTH        0.016710219
#define YEARDAY       365.25
#define MSAT          5.6851e29
#define DEGRAD        0.017453292519444445
#define ATOMMASS      1.660538921e-24       // g

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
#define VSEMI        1
#define VECC         2
#define VROT         3
#define VOBL         4
#define VNUM40KMAN      5
#define VNUM232THMAN    6
#define VNUM238UMAN     7
#define VNUM235UMAN     8 
#define VNUM40KCORE     9
#define VNUM232THCORE   10
#define VNUM238UCORE    11
#define VNUM235UCORE    12 


/* Now define the structs */


/* 
 * BODY contains all the physical parameters for every body 
 */

#define MAXSPECIES       100
typedef struct {
  double dInitTimeStep;
  double dMaxSteps;
} PHOTOCHEM;

typedef struct {
  char cName[NAMELEN];   /* Name */
  char cType[OPTLEN];    /* What kind of object */

  /* Body Properties */
  double dMass;		 /* Mass */
  double dRadius;	 /* Radius */
  double dDensity;       /* Bulk density */
  double dGravAccel;     /* Gravitational acceleration */
  double dK2;		 /* Love number */
  double dObliquity;     /* Obliquity */
  double dRotRate;       /* Rotation Rate */
  double dRotPer;        /* Rotation Period */
  double dRotVel;        /* Rotational Velocity */
  double dRadGyra;       /* Radius of Gyration */
  double dIntEn;         /* Total Internal Energy */

  double *daSED;

  /* Orbital Properties. By convention, these are stored in the
   * second element in the BODY array.   */
  double dSemi;          /* Semi-major axis */
  double dEcc;           /* Eccentricity */
  double dMeanMotion;    /* Mean Motion */
  double dOrbPeriod;     /* Orbital Period */

  /* EQTIDE Parameters */
  int bEqtide;
  int iTidePerts;        /* Number of tidal perturbers */
  int *iaTidePerts;      /* Body #'s of tidal perturbers */
  char saTidePerts[MAXARRAY][NAMELEN];
  //char **saTidePerts;
  double dTidalQ;	 /* Tidal Q */
  double dTidalTau;      /* Time lag */
  double *dTidalZ;       /* Combination of variables for easier computing */
  double *dTidalChi;     /* Ditto */
  double **dTidalF;      /* Ditto */
  double *dTidalBeta;    /* Ditto */
  int **iTidalEpsilon;   /* Signs of phase lags */

  /* RADHEAT Parameters: H = Const*exp[-Time/HalfLife] */
    int bRadheat;
    double dIsotope; // XXX Hack. Prolly need to just use the isotopes
    double d40KConstMan;       
    double d40KNumMan;
    double d40KPowerMan;
    double d40KMassMan;
    double d40KConstCore;       
    double d40KNumCore;
    double d40KPowerCore;
    double d40KMassCore;
    double d232ThConstMan;
    double d232ThNumMan;
    double d232ThPowerMan;
    double d232ThMassMan;
    double d232ThConstCore;
    double d232ThNumCore;
    double d232ThPowerCore;
    double d232ThMassCore;
    double d238UConstMan;
    double d238UNumMan;
    double d238UPowerMan;
    double d238UMassMan;
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

  /* PHOTOCHEM Parameters */
  PHOTOCHEM Photochem;
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
  char cName[NAMELEN];	 /* System name */
  double dAge;           /* System Age */
  double dTotAngMomInit; /* Initial angular momentum */
  double dTotAngMom;     /* Change in angular momentum: (l-l0)/l0 */
} SYSTEM;

/* 
 * Updates: Struct that contains all the variables to be updated and the functions to be called to be updated, fnUpdate. 
 */

typedef struct {  
  /* N.B. that pdVar points to the same memory location as
   * body.x, where x=semi, ecc, etc. */
  double **pdVar;
  int iNumVars;     // Number of update-able variables for a given body

  /* The "type" refers to how the variable is updated. If 0, then 
     the variable is assumed to be an explicit function of age. The 
     first timestep is then a bit dodgy as the rate is not initially
     known. The suggested timestep will be dEta*dTimestep, so runs 
     with a Type 0 variable must account for  the evolution with 
     dTimeStep. */
  int **iaType;     
  double *daDeriv;      // Array of derivative values
  double **daDerivProc; // Array of derivative values due to each process
  double *dVar;         //
  int ***iaBody;        // The body numbers needed to calculate the derivative
  int **iNumBodies;     // Number of bodies relevant to calculate the derivative

  /* These keep track of the variable and modules */
  int iNumModules;
  int *iNumEqns; // Number of equations that can modify the parameter
  int *iaVar;
  int **iaModule;

  /* Number of eqns to modify a parameter */
  int iNumEcc;
  int iNumObl;
  int iNumRot;
  int iNumSemi;

  /* These are the variables that the update matrix modifies */
  int iEcc;
  double dDEccDt;
  int iObl;
  double dDOblDt;
  int iRot;
  double dDRotDt;
  int iSemi;
  double dDSemiDt;

  /* Next comes the identifiers for the module that modifies a variable */

  /* EQTIDE */
  int iEccEqtide;
  int *iaOblEqtide;
  int *iaRotEqtide;
  int iSemiEqtide;
  double *pdDsemiDtEqtide;
  double *pdDeccDtEqtide;
  double **padDoblDtEqtide;
  double **padDrotDtEqtide;

  /* RADHEAT */
    int i40KMan;
    int i232ThMan;
    int i238UMan;
    int i235UMan;
    int iNum40KMan;
    int iNum232ThMan;
    int iNum238UMan;
    int iNum235UMan;
    double dD40KNumManDt;
    double dD232ThNumManDt;
    double dD238UNumManDt;
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

} UPDATE;

typedef struct {
  int iNumHalts;

  int bMerge;          /* Halt for merge? */
  double dMinSemi;     /* Halt at this semi-major axis */
  double dMinObl;      /* Halt at this obliquity */
  double dMaxEcc;      /* Halt at this eccentricity */
  double dMinEcc;      /* Halt at this eccentricity */
  int bPosDeDt;        /* Halt when de/dt > 0 */
  int dMinIntEn;       /* Minimum internal energy */

  /* EQTIDE */
  int bDblSync;         /* Double Synchronous? (e=obl1=obl2=0) */
  int bTideLock;        /* Halt primary becomes tide locked? */
  int bSync;            /* Halt if rotation becomes synchronous? */

  /* RADHEAT */
    int dMin40KPower;
    int dMin232ThPower;
    int dMin238UPower;
    int dMin235UPower;  //PED
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
  int iMass;            /* Mass Unit for input/output */
  int iLength;          /* Length Unit for input/output */
  int iAngle;           /* Angle Unit for input/output */
  int iTime;            /* Time Unit for input/output */
} UNITS;

typedef void (*fnAuxPropsModule)(BODY*,int);
/* Note this hack -- the second int is for iEqtideModel. This may 
   have to be generalized for other modules. */
typedef void (*fnBodyCopyModule)(BODY*,BODY*,int,int);

/* Integration parameters */
typedef struct {
  int bDoForward;	 /* Perform forward integration? */
  int bDoBackward;	 /* Perform backward integration? */
  double dTime;
  double dEta;           /* Variable Timestep coefficients */
  double dStopTime;	 /* Integration time */
  double dTimeStep;	 /* Time step */
  int bVarDt;            /* Use variable timestep? */
  int nSteps;            /* Number of steps since last output */
  double dMinValue;      /* Minimum value for e and obl */
  int bFirstStep;        /* Has the first step been taken? */
  int iNumBodies;        /* Number of bodies */
  int iOneStep;          /* Integer version of integration method */

  // These are to store midpoint derivative info in RK4.
  BODY *tmpBody;
  UPDATE *tmpUpdate;
  double ***daDeriv;

  // Module-specific parameters
  int *iNumModules;

  /* EQTIDE */
  int iEqtideModel;      /* Integer version of tide model */
  int bDiscreteRot;	 /* Use Discrete Rotation model (CPL) */
  int *bForceEqSpin;     /* Force spin rate to be equilibrium? */
  double *dMaxLockDiff;  /* When to set spin rate to equilibrium */
  double *dSyncEcc;

  /* RADHEAT */
  /* Nothing? */

  fnAuxPropsModule **fnAuxProps;
  fnBodyCopyModule **fnBodyCopy;
} EVOLVE;

/* The CONTROL struct contains all the parameters that
 * control program flow. */

typedef struct {
  int iVerbose;           /* Verbosity: 0=no STDOUT, 5=all */
  double dOutputTime;	  /* Output interval */

  int bLog;               /* Write log file? */

  /* Output Notation */
  int iDigits;
  int iSciNot;

  int bOverwrite;         /* Allow files to be overwritten ? */
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
  int *iMassRad;           /* Mass-Radius relationship identifier */

  fnHaltModule **fnHalt;
  fnForceBehaviorModule **fnForceBehavior;

} CONTROL;

/* The INFILE struct contains all the information 
 * regarding the files that read in. */

typedef struct {
  char cIn[NAMELEN];
  int *bLineOK;
  int iNumLines;
  /* Species file for PHOTOCHEM */
  char cSpecies[NAMELEN];
  /* Reaction file for PHOTOCHEM */
  char cReactions[NAMELEN];

  /* Aerosol scattering files */
  /* Aqueous file -- add to SpeciesFile? */
  /* Array of Vapor pressure file */

} INFILE;

/* The OUTFILE struct contains all the information 
 * regarding the output files. */

typedef struct {
  char cOut[NAMELEN];
  int iNumCols;
  char caCol[NUMOUT][OPTLEN];
  int bNeg[NUMOUT];
} OUTFILE;


/* The FILES struct contains all the information 
 * regarding every file. */

typedef struct {
  char cExe[LINE];             /* Name of executable */
  OUTFILE *Outfile;  /* Forward integration */
  char cLog[NAMELEN];               /* Log file */
  INFILE *Infile;     /* Array of input file info */
  int iNumInputs;              /* Number of input files */
} FILES;

/* The OPTIONS struct contains all the information
 * regarding the options, including their file data. */

typedef struct {
  char cName[OPTLEN];
  char cDescr[OPTDESCR];
  int iType;
  char cDefault[OPTLEN];
  double dDefault;
  int iMultiFile;
  int iMultiIn;
  int *iLine;
  char *iFile;
  char cFile[MAXFILES][OPTLEN];
  char cNeg[OPTDESCR];
  double dNeg;
} OPTIONS;

/* OUTPUT contains the data regarding every output parameters */

/* Some output variables must combine output from different modules.
 * These functions do that combining. */

typedef double (*fnOutputModule)(BODY*,SYSTEM*,UPDATE*,int,int);

typedef struct {
  char cName[OPTLEN];    /* Name */
  char cDescr[LINE];      /* Description */
  int bNeg;              /* Is there a negative option? */
  int *bDoNeg;            /* Should the output use "negative" units? */
  char cNeg[NAMELEN];         /* Units of negative option */
  double dNeg;           /* Conversion factor for negative option */
  int iNum;              /* Number of columns for output */

  /* Now add vector output functions */
  fnOutputModule **fnOutput;

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
typedef void (*fnFinalizeUpdate40KNumModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdate232ThNumModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdate238UNumModule)(BODY*,UPDATE*,int*,int,int);
typedef void (*fnFinalizeUpdate235UNumModule)(BODY*,UPDATE*,int*,int,int);  //PED

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
    int *iNumModules;
    int **iaModule;

    fnCountHaltsModule **fnCountHalts;
    
    fnInitializeControlModule **fnInitializeControl;
    fnInitializeUpdateModule **fnInitializeUpdate;
    fnInitializeBodyModule **fnInitializeBody;
    fnInitializeOutputModule **fnInitializeOutput;
    fnInitializeUpdateTmpBodyModule **fnInitializeUpdateTmpBody;

    fnFinalizeUpdateEccModule **fnFinalizeUpdateEcc;
    fnFinalizeUpdateNumIsotopeModule **fnFinalizeUpdateNumIsotope;
    fnFinalizeUpdateOblModule **fnFinalizeUpdateObl;
    fnFinalizeUpdateRotModule **fnFinalizeUpdateRot;
    fnFinalizeUpdateSemiModule **fnFinalizeUpdateSemi;
    fnFinalizeUpdate40KNumModule **fnFinalizeUpdate40KNum;
    fnFinalizeUpdate232ThNumModule **fnFinalizeUpdate232ThNum;
    fnFinalizeUpdate238UNumModule **fnFinalizeUpdate238UNum;
    fnFinalizeUpdate235UNumModule **fnFinalizeUpdate235UNum;  

    fnLogBodyModule **fnLogBody;

    fnReadOptionsModule **fnReadOptions;

    fnVerifyModule **fnVerify;
    fnVerifyHaltModule **fnVerifyHalt;
    fnVerifyRotationModule **fnVerifyRotation;

    /* fnInitializeOutputFunctionModule **fnInitializeOutputFunction; */
    fnFinalizeOutputFunctionModule **fnFinalizeOutputFunction;
  
} MODULE;

/* fnIntegrate is a pointer to a function that performs 
 * integration. */
typedef void (*fnIntegrate)(BODY*,CONTROL*,SYSTEM*,UPDATE*,fnUpdateVariable***,double*,int);

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
#include "eqtide.h"
#include "radheat.h"

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

#define MODULEOPTEND        1200
#define MODULEOUTEND        1200

