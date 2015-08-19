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
#define OPT_MODULES             20

#define OPT_SYSTEMNAME		30

#define OPT_UNITANGLE		40
#define OPT_UNITLENGTH		50
#define OPT_UNITMASS		60
#define OPT_UNITTIME	        70
#define OPT_UNITTEMP	        71
#define OPT_VERBOSE		80

// Regular Options

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
#define OPT_LONGP               515

#define OPT_MASS		520
#define OPT_MASSRAD	        525
#define OPT_MINVALUE            530

#define OPT_ORBECC		535
#define OPT_ORBMEANMOTION	540
#define OPT_ORBPER		550
#define OPT_ORBSEMI		560

#define OPT_OBL	                640
#define OPT_OUTDIGITS           570
#define OPT_OUTPUTORDER		580
#define OPT_OUTSCINOT	        590
#define OPT_OVERWRITE           595

#define OPT_PRECA               610
#define OPT_PRILUM	        615

#define OPT_RADIUS              650
#define OPT_RG		        655

#define OPT_ROTPER	        660
#define OPT_ROTRATE	        665
#define OPT_ROTVEL              680

#define OUT_XOBL                1422
#define OUT_YOBL                1423
#define OUT_ZOBL                1424

void InitializeOptions(OPTIONS*,fnReadOption*);
void ReadOptions(BODY**,CONTROL*,FILES*,MODULE*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE**,fnReadOption*,char[]);

double dNegativeDouble(OPTIONS,char[],int);
void AddOptionStringArray(char[],char[],char[MAXARRAY][OPTLEN],int*,int*,int*,int);
void AddOptionDoubleArray(char[],char[],double*,int*,int*,int*,int);
void NotPrimaryInput(int,char[],char[],int,int);
void AddOptionDouble(char[],char[],double*,int*,int);
void AddOptionInt(char[],char[],int*,int*,int);
void AddOptionString(char[],char[],char[],int*,int);
void AddOptionBool(char[],char[],int*,int*,int);
void CheckDuplication(FILES*,OPTIONS*,char[],int,int);
void UpdateFoundOption(INFILE*,OPTIONS*,int,int);
void UpdateFoundOptionMulti(INFILE*,OPTIONS*,int*,int,int);
void AssignDefaultDouble(OPTIONS*,double*,int);
void AssignDefaultInt(OPTIONS*,int*,int);
void AssignDefaultString(OPTIONS*,char[],int);


