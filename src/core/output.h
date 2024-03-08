/**
  @file output.h
  @brief Subroutines for output and logging.
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date May 7 2014

  To add a new output:
  1) Add new definition here
  2) Add output properties to InitializeOutput
  3) Write new subroutine that returns desired output
  4) Write new fnWriteOutput function

*/

/* Exit Status */

#define EXIT_EXE 1
#define EXIT_INPUT 2
#define EXIT_UNITS 3
#define EXIT_WRITE 4
#define EXIT_INT 5
#define EXIT_OUTPUT 6

/* Verbosity Level */

#define VERBERR 1
#define VERBPROG 2
#define VERBINPUT 3
#define VERBUNITS 4
#define VERBALL 5

/* General Outuput 0-999 */
/* System properties 0-499, body properties 500-999 */
#define OUTSTART 0
#define OUTBODYSTART 500
#define OUTEND 1000

#define OUT_AGE 160
#define OUT_TIME 170

#define OUT_TOTENERGY 190

#define OUT_POTENERGY 191
#define OUT_KINENERGY 192
#define OUT_TOTORBENERGY 193

#define OUT_DT 200

#define OUT_MASS 500
#define OUT_OBL 510
#define OUT_PRECA 511
#define OUT_XOBL 513
#define OUT_YOBL 515
#define OUT_ZOBL 517

#define OUT_RADIUS 520
#define OUT_RADGYRA 521
#define OUT_ROTANGMOM 530
#define OUT_ROTKINENERGY 535
#define OUT_ROTVEL 540
#define OUT_BODYTYPE 545
#define OUT_ROTRATE 550
#define OUT_ROTPER 560
#define OUT_DENSITY 570

#define OUT_SURFENFLUX 580
#define OUT_TIDALQ 581
#define OUT_IMK2 582
#define OUT_K2 583

#define OUT_K2MAN 590   // Mantle k2 love number
#define OUT_IMK2MAN 592 // Mantle Im(k2) love number
#define OUT_TIDALQMAN 593
#define OUT_VISCUMAN 595 // Viscosity UMTBL

#define OUT_HECC 600
#define OUT_HZLIMDRYRUNAWAY 602 /**< Recent Venus HZ Limit */
#define OUT_HZLIMRECVENUS 604   /**< Recent Venus HZ Limit */
#define OUT_HZLIMRUNAWAY 605    /**< Runaway Greenhouse HZ Limit */
#define OUT_HZLIMMOIST 606      /**< Moist Greenhouse HZ Limit */
#define OUT_HZLIMMAX 607        /**< Maximum Venus HZ Limit */
#define OUT_HZLIMEARLYMARS 608  /**< Early Mars HZ Limit */
#define OUT_INSTELLATION 609

#define OUT_KECC 610
#define OUT_ORBECC 615
#define OUT_ORBEN 620
#define OUT_ORBMEANMOTION 630
#define OUT_ORBPER 640
#define OUT_ORBSEMI 650
#define OUT_CRITSEMI 651
#define OUT_COPP 655
#define OUT_ORBANGMOM 660
#define OUT_ARGP 661
#define OUT_MEANA 662
#define OUT_INC 663
#define OUT_LONGA 664
#define OUT_MEANL 665
#define OUT_LONGP 667

#define OUT_LXUVTOT 668 /**< Total XUV Luminosity from the star */

#define OUT_ORBKINENERGY 670
#define OUT_ORBPOTENERGY 680
#define OUT_ORBENERGY 690

#define OUT_LOSTENG 691
#define OUT_LOSTANGMOM 692

#define OUT_ANGMOMTOT 700
#define OUT_ANGMOMTOTREFX 701
#define OUT_ANGMOMTOTREFY 702
#define OUT_ANGMOMTOTREFZ 703
#define OUT_ANGMOMTOTREFUNITX 705
#define OUT_ANGMOMTOTREFUNITY 706
#define OUT_ANGMOMTOTREFUNITZ 707

#define OUT_ANGMOMTOTINVX 711
#define OUT_ANGMOMTOTINVY 712
#define OUT_ANGMOMTOTINVZ 713
#define OUT_ANGMOMTOTINVUNITX 715
#define OUT_ANGMOMTOTINVUNITY 716
#define OUT_ANGMOMTOTINVUNITZ 717

#define OUT_ANGMOMORBREF 720
#define OUT_ANGMOMORBREFX 721
#define OUT_ANGMOMORBREFY 722
#define OUT_ANGMOMORBREFZ 723
#define OUT_ANGMOMORBREFUNITX 725
#define OUT_ANGMOMORBREFUNITY 726
#define OUT_ANGMOMORBREFUNITZ 727

#define OUT_ANGMOMORBINV 730
#define OUT_ANGMOMORBINVX 731
#define OUT_ANGMOMORBINVY 732
#define OUT_ANGMOMORBINVZ 733
#define OUT_ANGMOMORBINVUNITX 735
#define OUT_ANGMOMORBINVUNITY 736
#define OUT_ANGMOMORBINVUNITZ 737

#define OUT_ANGMOMROTREF 740
#define OUT_ANGMOMROTREFX 741
#define OUT_ANGMOMROTREFY 742
#define OUT_ANGMOMROTREFZ 743
#define OUT_ANGMOMROTREFUNITX 745
#define OUT_ANGMOMROTREFUNITY 746
#define OUT_ANGMOMROTREFUNITZ 747

#define OUT_ANGMOMROTINV 750
#define OUT_ANGMOMROTINVX 751
#define OUT_ANGMOMROTINVY 752
#define OUT_ANGMOMROTINVZ 753
#define OUT_ANGMOMROTINVUNITX 755
#define OUT_ANGMOMROTINVUNITY 756
#define OUT_ANGMOMROTINVUNITZ 757

#define OUT_ANGMOMROTORB 760
#define OUT_ANGMOMROTORBX 761
#define OUT_ANGMOMROTORBY 762
#define OUT_ANGMOMROTORBZ 763
#define OUT_ANGMOMROTORBUNITX 765
#define OUT_ANGMOMROTORBUNITY 766
#define OUT_ANGMOMROTORBUNITZ 767


/* @cond DOXYGEN_OVERRIDE */

void WriteBodyPrecA(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                    int, double *, char[]);
void Bary2OrbElems(BODY *body, int iBody);
void WriteLogEntry(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                   fnWriteOutput, FILE *, int);

void InitializeOutputFunctions(MODULE *, OUTPUT *, int);
void WriteOutput(BODY *, CONTROL *, FILES *, OUTPUT *, SYSTEM *, UPDATE *,
                 fnWriteOutput *);
void WriteLog(BODY *, CONTROL *, FILES *, MODULE *, OPTIONS *, OUTPUT *,
              SYSTEM *, UPDATE *, fnUpdateVariable ***, fnWriteOutput *, int);
void InitializeOutput(FILES*,OUTPUT *, fnWriteOutput *);
void WriteTime(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *, int,
               double *, char[]);

/* @endcond */
