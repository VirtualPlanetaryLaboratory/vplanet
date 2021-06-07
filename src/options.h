/**
  @file options.h
  @brief All subroutines necessary to read in all options. Also
        monitor input files for mistakes and log all option data.
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date May 7 2014

  To add a new option:
  1) Add integer definition here
  2) Add new option properties in InitializeOptions
  3) Write new fnReadOptions function
  4) If necessary, modify the ReadOptions subroutine
  5) If necessary, modify the Verify process

*/

/* Initial Options */
#define OPT_BODYFILES 10
#define OPT_MODULES 20

#define OPT_SYSTEMNAME 30

#define OPT_UNITANGLE 40
#define OPT_UNITLENGTH 50
#define OPT_UNITMASS 60
#define OPT_UNITTIME 70
#define OPT_UNITTEMP 71
#define OPT_VERBOSE 80

// Regular Options

#define OPT_AGE 100
#define OPT_ALBEDOGLOBAL 105

#define OPT_BACK 110
#define OPT_OUTFILE 120
#define OPT_ETA 130
#define OPT_OUTPUTTIME 140
#define OPT_STOPTIME 150
#define OPT_TIMESTEP 160
#define OPT_VARDT 170
#define OPT_BODYNAME 180

#define OPT_COLOR 185

#define OPT_DENSITY 190

#define OPT_FORW 200

#define OPT_HALTMAXECC 320
#define OPT_HALTMAXMUTUALINC 322
#define OPT_HALTMERGE 325
#define OPT_HALTMINECC 330
#define OPT_HALTMINOBL 335
#define OPT_HALTMINSEMI 345
#define OPT_HALTPOSDEDT 350

#define OPT_HECC 360

#define OPT_INTEGRATIONMETHOD 400

#define OPT_KECC 450

#define OPT_LOG 500
#define OPT_LOGFILE 510
#define OPT_LONGP 515
#define OPT_LXUV 517

#define OPT_MASS 520
#define OPT_MASSRAD 525
#define OPT_MINVALUE 530

#define OPT_ORBECC 535
#define OPT_ORBMEANMOTION 540
#define OPT_BODYTYPE 545
#define OPT_ORBPER 550
#define OPT_ORBSEMI 560
#define OPT_INC 561
#define OPT_ARGP 562
#define OPT_LONGA 563
#define OPT_GRCORR 564
#define OPT_LUMINOSITY 575 /**< (Initial) luminosity */
#define OPT_TIDALQ 577
#define OPT_USEOUTERTIDALQ 579

#define OPT_OUTDIGITS 570
#define OPT_OUTPUTORDER 580
#define OPT_GRIDOUTPUT 585
#define OPT_OUTSCINOT 590
#define OPT_OVERWRITE 595

#define OPT_PRECA 610
#define OPT_DYNELLIP 611
#define OPT_CALCDYNELLIP 612
#define OPT_PRILUM 615

#define OPT_VISCUMAN 630 // Viscosity UMTBL
#define OPT_OBL 640
#define OPT_COSOBL 645
#define OPT_RADIUS 650
#define OPT_RG 655

#define OPT_ROTPER 660
#define OPT_ROTRATE 665
#define OPT_ROTVEL 680

#define OPT_TEMPERATURE 690 /**< Effective temperature (initial) */

#define OPT_XOBL 700
#define OPT_YOBL 710
#define OPT_ZOBL 720

#define OPT_SURFACEWATERMASS 810 /**< Initial surface water mass */
#define OPT_MINSURFACEWATERMASS                                                \
  812 /**< Minimum surface water mass (desiccated below this) */
#define OPT_ENVELOPEMASS 815 /**< Initial envelope mass */
#define OPT_MINENVELOPEMASS                                                    \
  817 /**< Minimum envelope mass (evaporated below this) */

/* @cond DOXYGEN_OVERRIDE */

void GetWords(char cLine[],char[MAXARRAY][OPTLEN],int*,int*);

void InitializeOptions(OPTIONS *, fnReadOption *);
void ReadOptions(BODY **, CONTROL *, FILES *, MODULE *, OPTIONS *, OUTPUT *,
                 SYSTEM *, UPDATE **, fnReadOption *, char[]);

double dNegativeDouble(OPTIONS, char[], int);
void AddOptionStringArray(char[], char[], char[MAXARRAY][OPTLEN], int *, int *,
                          int *, int);
void AddOptionDoubleArray(char[], char[], double *, int *, int *, int *, int);
void NotPrimaryInput(int, char[], char[], int, int);
void AddOptionDouble(char[], char[], double *, int *, int);
void AddOptionInt(char[], char[], int *, int *, int);
void AddOptionString(char[], char[], char[], int *, int);
void AddOptionBool(char[], char[], int *, int *, int);
void CheckDuplication(FILES *, OPTIONS *, char[], int, int);
void UpdateFoundOption(INFILE *, OPTIONS *, int, int);
void UpdateFoundOptionMulti(INFILE *, OPTIONS *, int *, int, int);
void AssignDefaultDouble(OPTIONS *, double *, int);
void AssignDefaultInt(OPTIONS *, int *, int);
void AssignDefaultString(OPTIONS *, char[], int);

/* @endcond */
