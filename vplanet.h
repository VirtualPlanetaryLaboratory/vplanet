/******************** VPLANET.H ***********************/
/*
 * Rory Barnes, Wed May  7 16:04:27 PDT 2014
 *
 */

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

#define BIGG          6.672e-8
#define PI            3.1415926535

#define RNEP          2.4764e9
#define MNEP          1.0244e29
#define RHOEARTH      5.52
#define eEARTH        0.016710219
#define YEARDAY       365.25
#define MSAT          5.6851e29
#define DEGRAD        0.017453292519444445

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
			     * in VPLANET */
#define MAXLINES      256   /* Maximum Number of Lines in an 
			     * input file */
#define OPTEND        1100  /* Last output number of module options
			     * EQTIDE is highest for this compiltion */

#define TINY          (1./HUGE)

/* 0 => Not input by user, verify assigns default */
#define EULER         1
#define RUNGEKUTTA    2

/* Tidal Model */

#define CPL           0
#define CTL           1

/* 
 * BODY contains all the physical parameters for every body 
 */

typedef struct {
  char cName[NAMELEN];        /* Name */
  char cType[OPTLEN];    /* What kind of object */

  /* Body Properties */
  double dMass;		 /* Mass */
  double dRadius;	 /* Radius */
  double dDensity;       /* Bulk density */
  double dK2;		 /* Love number */
  double dObliquity;     /* Obliquity */
  double dRotRate;       /* Rotation Rate */
  double dRotPer;        /* Rotation Period */
  double dRotVel;        /* Rotational Velocity */
  double dRadGyra;       /* Radius of Gyration */
  double dTidalQ;	 /* Tidal Q */
  double dTidalTau;      /* Time lag */

  /* Orbital Properties. By convention, these are stored in the
   * second element in the BODY array.   */
  double dSemi;          /* Semi-major axis */
  double dEcc;           /* Eccentricity */
  double dMeanMotion;    /* Mean Motion */
  double dOrbPeriod;     /* Orbital Period */

  /* Auxiliary Parameters */
  double dTidalZ;
  double dTidalChi;
  double dTidalF[5];
  double dTidalBeta;
  int iTidalEpsilon[10]; /* Signs of phase lags */

} BODY;

/* HALT struct contains all dynamical stopping conditions */

typedef struct {
  int bHalt;            /* Any halts? */
  int bDblSync;
  int bMerge;           /* Halt for merge? */
  double dMinSemi;	/* Halt at this semi-major axis */
  double *dMinObl;	/* Halt at this obliquity */
  double dMaxEcc;	/* Halt at this eccentricity */
  double dMinEcc;	/* Halt at this eccentricity */
  int bPosDeDt;         /* Halt when de/dt > 0 */
  int *bTideLock;       /* Halt primary becomes tide locked? */
  int *bSync;           /* Halt if rotation becomes synchronous? */
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
  int iMass;               /* Mass Unit for input/output */
  int iLength;             /* Length Unit for input/output */
  int iAngle;              /* Angle Unit for input/output */
  int iTime;               /* Time Unit for input/output */
} UNITS;

/* Integration parameters */
typedef struct {
  int bDoForward;		/* Perform forward integration? */
  int bDoBackward;		/* Perform backward integration? */
  double dTime;
  double dEta;                  /* Variable Timestep coefficients */
  double dStopTime;	        /* Integration time */
  double dTimeStep;	        /* Time step */
  int bVarDt;                   /* Use variable timestep? */
  int nSteps;
  int bDiscreteRot;		/* Use Discrete Rotation model (CPL) */
  double dMinValue;             /* Minimum value for e and obl */

} INTEGR;

/* 
 * Updates: Struct that contains all the variables to be updated and the functions to be called to be updated, fnUpdate. 
 */

typedef struct {  
  /* N.B. that pdVar points to the same memory location as
   * body.x, where x=semi, ecc, etc. */
  double **pdVar;
  int iNum;
  int *iType;
  int *iNumEqns;
  double **dUpdate;
  double *dVar;
  
  double *pdDsemiDt;
  double *pdDeccDt;
  double *pdDoblDt;
  double *pdDrotDt;

} UPDATE;

/* The CONTROL struct contains all the parameters that
 * control program flow. */

typedef struct {
  HALT Halt;
  UNITS *Units;
  INTEGR Integr;

  int iVerbose;           /* Verbosity: 0=no STDOUT, 5=all */
  int iNumBodies;         /* Number of bodies, always 2 for eqtide */
  double dOutputTime;	  /* Output interval */

  int bLog;               /* Write log file? */

  /* Output Notation */
  int iDigits;
  int iSciNot;

  int *iMassRad;           /* Mass-Radius relationship identifier */

  int iOneStep;           /* Integer version of integration method */

  int bOverwrite;         /* Allow files to be overwritten ? */

  /* EQTIDE parameters */
  int iTideModel;         /* Integer version of tide model */
  int *bForceEqSpin;       /* Force spin rate to be equilibrium? */
  double *dMaxLockDiff;    /* When to set spin rate to equilibrium */
  double *dSyncEcc;

} CONTROL;

/* The INFILE struct contains all the information 
 * regarding the files that read in. */

typedef struct {
  char cIn[NAMELEN];
  int *bLineOK;
  int iNumLines;
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
  OUTFILE Outfile[MAXBODIES];  /* Forward integration */
  char cLog[NAMELEN];               /* Log file */
  INFILE Infile[MAXFILES];     /* Array of input file info */
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

/* SYSTEM contains properties of the system that pertain to
   every BODY */

typedef struct {
  char cName[NAMELEN];	 /* System name */
  double dAge;           /* System Age */
  double dTotAngMomInit; /* Initial angular momentum */
  double dTotAngMom;     /* Change in angular momentum: (l-l0)/l0 */
} SYSTEM;

/* OUTPUT contains the data regarding every output parameters */

typedef struct {
  char cName[OPTLEN];    /* Name */
  char cDescr[LINE];      /* Description */
  int bNeg;              /* Is there a negative option? */
  int *bDoNeg;            /* Should the output use "negative" units? */
  char cNeg[NAMELEN];         /* Units of negative option */
  double dNeg;           /* Conversion factor for negative option */
  int iNum;              /* Number of columns for output */
} OUTPUT;

/* fnReadOption is a pointer to a function that reads in an OPTION
 * parameter. */
typedef void (*fnReadOption)(CONTROL *,BODY *,SYSTEM *,OPTIONS *,FILES *,int);

/* fnWriteOutput is a pointer to a function that writes out an
 * OUTPUT parameter. */
typedef void (*fnWriteOutput)(CONTROL*,UNITS*,OUTPUT,BODY *,SYSTEM*,UPDATE*,int,double *,char []);

/* fnUpdate Variable is a pointer to a function that calculates
 * derivatives. */
typedef double (*fnUpdateVariable)(BODY*,int);

/* fnIntegrate is a pointer to a function that performs 
 * integration. */
typedef void (*fnIntegrate)(CONTROL*,BODY*,UPDATE*,fnUpdateVariable***,int,double*);


typedef int (*fnHaltSystem)(CONTROL*,BODY*,UPDATE*);
typedef int (*fnHaltBod)(CONTROL*,BODY*,UPDATE*,int);

typedef struct {
  fnHaltSystem *fnHaltSys;
  fnHaltBod *fnHaltBody;
} FNHALT;

/* 
 * Major Functions 
 */

void VerifyOptions(OPTIONS*,OUTPUT*,BODY*,SYSTEM*,FILES*,CONTROL*,fnIntegrate*,UPDATE*,fnUpdateVariable***); 

void Evolve(CONTROL*,BODY*,SYSTEM*,OUTPUT*,FILES*,UPDATE*,fnUpdateVariable***,FNHALT*,fnWriteOutput*,fnIntegrate);

void WriteOutput(CONTROL*,OUTPUT*,BODY*,SYSTEM*,FILES*,fnWriteOutput*,double,double,UPDATE*);

void ReadOptions(OPTIONS*,CONTROL*,FNHALT*,BODY*,FILES*,SYSTEM*,OUTPUT*,fnReadOption*);

void WriteLog(CONTROL*,BODY*,SYSTEM*,OUTPUT*,OPTIONS*,FILES*,fnWriteOutput*,UPDATE*,fnUpdateVariable***,int);

void InitializeOptions(OPTIONS *,fnReadOption[]);

void InitializeOutput(OUTPUT *,fnWriteOutput[]);

void InitializeUpdate(UPDATE *update);



/* 
 * Integration Methods 
 */
void EulerStep(CONTROL*,BODY*,UPDATE*,fnUpdateVariable***,int,double *);

void RungeKutta4Step(CONTROL*,BODY*,UPDATE*,fnUpdateVariable***,int,double *);

/* 
 * Other Header Files - These are primarily for function declarations
 */

#include "body.h"
#include "eqtide.h"
#include "halt.h"
#include "orbit.h"
#include "util.h"
#include "verify.h"

/* This needs to be vectorized VPL 

#define MODULEOPTEND        EQTIDEOPTEND
#define MODULEOUTEND        EQTIDEOUTEND
#define MODULEHALTSYSEND    EQTIDEHALTSYSEND
#define MODULEHALTBODYEND   EQTIDEHALTBODYEND
*/

#define MODULEOPTEND        1100
#define MODULEOUTEND        1100
#define MODULEHALTSYSEND    5
#define MODULEHALTBODYEND   3

