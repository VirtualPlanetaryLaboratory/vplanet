/***************** POISE.H *********************** 
 *
 * Russell Deitrick, September 10, 2015
 *
 * This header file contains all the subroutines in
 * file poise.c.
 *
*/

void InitializeControlPoise(CONTROL*);
void AddModulePoise(MODULE*,int,int);
void BodyCopyPoise(BODY*,BODY*,int,int);
void InitializeBodyPoise(BODY*,CONTROL*,UPDATE*,int,int);
void InitializeUpdateTmpBodyPoise(BODY*,CONTROL*,UPDATE*,int);

/* Options Info */

#define OPTSTARTPOISE       1800 /* Start of POISE options */
#define OPTENDPOISE         1900 /* End of POISE options */

#define OPT_LATCELLNUM      1801
#define OPT_PLANCKA         1802
#define OPT_PLANCKB         1803
#define OPT_DIFFUSION       1804
#define OPT_HEATCAPANN      1805
#define OPT_HADLEY          1806
#define OPT_COLDSTART       1807
#define OPT_FIXICELAT       1808
#define OPT_ALBEDOZA        1809
#define OPT_JORMUNGAND      1810

/*#define OPT_LANDGEOM        1840
#define OPT_ICEMODEL        1841
#define OPT_HEATCLAND       1842
#define OPT_HEATCWATER      1843
#define OPT_TFREEZE         1844
#define OPT_LATENTHEAT      1845
#define OPT_CONDUCTIVITY    1846
#define OPT_MLDEPTH         1847
#define OPT_NU              1848 

#define OPT_EBMMODEL        1899*/

/* Options Functions */

/* Verify Functions */
void VerifyPoise(BODY*,CONTROL*,FILES*,OPTIONS*,OUTPUT*,SYSTEM*,UPDATE*,fnUpdateVariable***,int,int);

/* Update Functions */

void InitializeUpdatePoise(BODY*,UPDATE*,int);

/* Output Functinos */

/* POISE 1800 - 1899 */
/* System properties 1800-1819, body properties 1820-1899 */

#define OUTSTARTPOISE        1800 /* Start of DISTROT options */
#define OUTENDPOISE          1900 /* End of DISTROT options */
#define OUTBODYSTARTPOISE    1820 /* Start of DISTROT BODY options */

#define OUT_TGLOBAL          1801 

void HelpOutputPoise(OUTPUT*);
void InitializeOutputPoise(OUTPUT*,fnWriteOutput[]);

/* Logging Functions */
void LogOptionsPoise(CONTROL*,FILE*);
void LogPoise(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*);
void LogBodyPoise(BODY*,CONTROL*,OUTPUT*,SYSTEM*,UPDATE*,fnWriteOutput[],FILE*,int);

/* Poise Functions */
void PropertiesPoise(BODY*,UPDATE*,int);
void ForceBehaviorPoise(BODY*,EVOLVE*,IO*,int,int);
void PoiseClimate(BODY*,int);
