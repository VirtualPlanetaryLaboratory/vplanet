/********************** OPTIONS.H *********************/
/*
 * Rory Barnes, Wed May  7 16:31:58 PDT 2014
 *
 * Header file for options. To add a new option:
 * 1) Add integer definition here
 * 2) Add new option properties in InitializeOptions
 * 3) Write new fnReadOptions function
 * 3) If necessary, modify the ReadOptions subroutine
 * 4) If necessary, modify the WriteLog subroutine
 *
 */

/* Initial Options */
#define OPT_BODYFILES           10
#define OPT_SYSTEMNAME		20

#define OPT_UNITANGLE		30
#define OPT_UNITLENGTH		40
#define OPT_UNITMASS		50
#define OPT_UNITTIME	        60
#define OPT_VERBOSE		70

#define OPT_AGE			100

#define OPT_BACK		110
#define OPT_OUTFILE   	        120
#define OPT_ETA                 130
#define OPT_OUTPUTTIME  	140
#define OPT_STOPTIME 	        150
#define OPT_TIMESTEP	        160
#define OPT_VARDT	        170
#define OPT_BODYNAME            180

#define OPT_DENSITY             190

#define OPT_FORW		200

#define OPT_HALTMAXECC          320
#define OPT_HALTMERGE           325
#define OPT_HALTMINECC          330
#define OPT_HALTMINOBL          335
#define OPT_HALTMINSEMI		345
#define OPT_HALTPOSDEDT         350 

#define OPT_INTEGRATIONMETHOD   400

#define OPT_LOG		        500

#define OPT_LOGFILE		510

#define OPT_MASS		620
#define OPT_MASSRAD	        630
#define OPT_MINVALUE            520

#define OPT_ORBECC		530
#define OPT_ORBMEANMOTION	540
#define OPT_ORBPER		550
#define OPT_ORBSEMI		560

#define OPT_OBL	                640
#define OPT_OUTDIGITS           570
#define OPT_OUTPUTORDER		580
#define OPT_OUTSCINOT	        590
#define OPT_OVERWRITE           595

#define OPT_PRILUM	        615

#define OPT_RADIUS              650
#define OPT_RG		        655

#define OPT_ROTPER	        660
#define OPT_ROTRATE	        665
#define OPT_ROTVEL              680

double dNegativeDouble(OPTIONS,char[],int);
void AddOptionStringArray(char[],char[],char[MAXARRAY][OPTLEN],int*,int*,int*,int);
void AddOptionDoubleArray(char[],char[],double*,int*,int*,int*,int);
void NotPrimaryInput(int,char[],char[],int,int);
void AddOptionDouble(char[],char[],double*,int*,int);
void AddOptionInt(char[],char[],int*,int*,int);
void AddOptionString(char[],char[],char[],int*,int);
void AddOptionBool(char[],char[],int*,int*,int);
void CheckDuplication(OPTIONS,FILES,char[],int,int);
void UpdateFoundOption(INFILE*,OPTIONS*,int,int);
void AssignDefaultDouble(double*,OPTIONS,int);
void AssignDefaultInt(int*,OPTIONS,int);
void AssignDefaultString(char[],OPTIONS,int);

/* This struct is for redundant variables */

typedef struct {
  double dPeriod;
  int lPeriod;    
  double dSemi;
  int lSemi;  
  double dMeanMotion;
  int lMeanMotion;
    
  double dSpinPer;
  int lSpinPer;
  double dSpinRate;
  int lSpinRate;
  double dVRot;
  int lVrot;
  double bForceEqSpin;
  int lForceEqSpin;

  double dMass;
  int lMass;
  double dRadius;
  int lRadius;
  double dDensity;
  int lDensity;
  int iMassRad;
  int lMassRad;

} INPUT;


