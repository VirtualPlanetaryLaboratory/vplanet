/**
   @file binary.c
   @brief Subroutines that control the integration of the circumbinary planet
   orbital dynamics module.
   @author David Fleming ([dflemin3](https://github.com/dflemin3/))
   @date Jan 12 2016

   @par Description
   \rst

       Module to model circumbinary planet dynamics.

       .. note:: body 0 = primary star, body 1 = secondary star, body 2+ = CBP
   (circumbinary planet(s))

       .. note:: The :cite:`Leung2013` theory ONLY applies to the restricted 3
   body approximation \ and hence the CBPs are not allowed to graviationally
   interact.

   \endrst

*/

#include "vplanet.h"

/** Copy body properties from src to dest for cbp */
void BodyCopyBinary(BODY *dest, BODY *src, int foo, int iNumBodies, int iBody) {

  dest[iBody].iBodyType  = src[iBody].iBodyType;
  dest[iBody].dCBPR      = src[iBody].dCBPR;
  dest[iBody].dCBPZ      = src[iBody].dCBPZ;
  dest[iBody].dCBPPhi    = src[iBody].dCBPPhi;
  dest[iBody].dCBPRDot   = src[iBody].dCBPRDot;
  dest[iBody].dCBPZDot   = src[iBody].dCBPZDot;
  dest[iBody].dCBPPhiDot = src[iBody].dCBPPhiDot;

  dest[iBody].dFreeEcc = src[iBody].dFreeEcc;
  dest[iBody].dFreeInc = src[iBody].dFreeInc;
  dest[iBody].dLL13N0  = src[iBody].dLL13N0;
  dest[iBody].dLL13K0  = src[iBody].dLL13K0;
  dest[iBody].dLL13V0  = src[iBody].dLL13V0;
  dest[iBody].dR0      = src[iBody].dR0;

  dest[iBody].dArgP  = src[iBody].dArgP;
  dest[iBody].dInc   = src[iBody].dInc;
  dest[iBody].dLongA = src[iBody].dLongA;
  dest[iBody].dLongP = src[iBody].dLongP;

  dest[iBody].dLL13PhiAB = src[iBody].dLL13PhiAB;
  dest[iBody].dCBPM0     = src[iBody].dCBPM0;
  dest[iBody].dCBPZeta   = src[iBody].dCBPZeta;
  dest[iBody].dCBPPsi    = src[iBody].dCBPPsi;
}

/** Only use this function for malloc'ing stuff
    Since nothing has to be malloc'ed for binary, do nothing */
void InitializeBodyBinary(BODY *body, CONTROL *control, UPDATE *update,
                          int iBody, int iModule) {
}

/** No need to allocate anything */
void InitializeUpdateTmpBodyBinary(BODY *body, CONTROL *control, UPDATE *update,
                                   int iBody) {
}

/**************** BINARY options ********************/

/** Free eccentricity, can't be in primary file
    Note: Do error checking for negative values */
void ReadFreeEcc(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {

  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0.0 || dTmp >= 1.0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be in range [0,1).\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    } else {
      body[iFile - 1].dFreeEcc = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFreeEcc = options->dDefault;
  }
}

/** Lee + Leung 2013 Mean Motion N0
    This parameter cannot exist in primary file */
void ReadLL13N0(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {

  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dLL13N0 = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultDouble(options, &body[iFile - 1].dLL13N0, files->iNumInputs);
    }
  }
}

/** Lee + Leung 2013 radial epicyclic frequency
    This parameter cannot exist in primary file */
void ReadLL13K0(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {

  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dLL13K0 = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultDouble(options, &body[iFile - 1].dLL13K0, files->iNumInputs);
    }
  }
}

/** Lee + Leung 2013 vertical epicyclic frequency
    This parameter cannot exist in primary file */
void ReadLL13V0(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {

  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dLL13V0 = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultDouble(options, &body[iFile - 1].dLL13V0, files->iNumInputs);
    }
  }
}

/** This parameter cannot exist in the primary file
    Free inclination goes from 0 to 180 degrees */
void ReadFreeInc(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {

  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) { // Input as radians
      if (dTmp < 0 || dTmp > PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,PI].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else { // Input as Degrees
      if (dTmp < 0 || dTmp > 180) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,180].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dFreeInc = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dFreeInc = options->dDefault;
  }
}

/** This parameter cannot exist in the primary file
    PhiAB goes from [0,360) if in degrees */
void ReadLL13PhiAB(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {

  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) { // Input as radians
      if (dTmp < 0 || dTmp >= 2 * PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,2PI).\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else { // Input as Degrees
      if (dTmp < 0 || dTmp >= 360) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,360).\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dLL13PhiAB = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dLL13PhiAB = options->dDefault;
  }
}

/** This parameter cannot exist in the primary file
    CBPM0 goes from [0,360) if in degrees */
void ReadCBPM0(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {

  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) { // Input as radians
      if (dTmp < 0 || dTmp >= 2 * PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,2PI).\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else { // Input as Degrees
      if (dTmp < 0 || dTmp >= 360) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,360).\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dCBPM0 = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dCBPM0 = options->dDefault;
  }
}

/** This parameter cannot exist in the primary file.
    dCBPZeta goes from [0,360) if in degrees */
void ReadCBPZeta(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {

  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) { // Input as radians
      if (dTmp < 0 || dTmp >= 2 * PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,2PI).\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else { // Input as degrees
      if (dTmp < 0 || dTmp >= 360) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,360).\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dCBPZeta = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dCBPZeta = options->dDefault;
  }
}

/** This parameter cannot exist in the primary file.
    dCBPPsi goes from [0,360) if in degrees */
void ReadCBPPsi(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {

  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) { // Input as radians
      if (dTmp < 0 || dTmp >= 2 * PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,2PI).\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else { // Input as degrees
      if (dTmp < 0 || dTmp >= 360) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,360).\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dCBPPsi = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dCBPPsi = options->dDefault;
  }
}

/** This parameter cannot exist in primary file */
void ReadHaltHolmanUnstable(BODY *body, CONTROL *control, FILES *files,
                            OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bHaltHolmanUnstable = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    control->Halt[iFile - 1].bHaltHolmanUnstable = 0;
  }
}

/** This parameter cannot exist in primary file */
void ReadHaltRocheLobe(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bHaltRocheLobe = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    control->Halt[iFile - 1].bHaltRocheLobe = 0;
  }
}

/** Initialization Options for BINARY */
void InitializeOptionsBinary(OPTIONS *options, fnReadOption fnRead[]) {
  int iOpt, iFile;

  sprintf(options[OPT_FREEECC].cName, "dFreeEcc");
  sprintf(options[OPT_FREEECC].cDescr, "Circumbinary planet free eccentricity");
  sprintf(options[OPT_FREEECC].cDefault, "0.0");
  sprintf(options[OPT_FREEECC].cDimension, "nd");
  options[OPT_FREEECC].dDefault   = 0.0;
  options[OPT_FREEECC].iType      = 2;
  options[OPT_FREEECC].bMultiFile = 1;
  fnRead[OPT_FREEECC]             = &ReadFreeEcc;
  sprintf(options[OPT_FREEECC].cLongDescr,
          "The free eccentricity of a circumbinary planet with the BINARY "
          "module.\n"
          "Must be in the range [0,1).");

  sprintf(options[OPT_FREEINC].cName, "dFreeInc");
  sprintf(options[OPT_FREEINC].cDescr, "Circumbinary planet free inclination");
  sprintf(options[OPT_FREEINC].cDefault, "0.0 degrees");
  sprintf(options[OPT_FREEINC].cDimension, "angle");
  options[OPT_FREEINC].dDefault   = 0.0;
  options[OPT_FREEINC].iType      = 2;
  options[OPT_FREEINC].bMultiFile = 1;
  fnRead[OPT_FREEINC]             = &ReadFreeInc;
  sprintf(options[OPT_FREEINC].cLongDescr,
          "The free inclination of a circumbinary planet with the BINARY "
          "module.\n"
          "Must be in the range [0,pi).");

  sprintf(options[OPT_LL13PHIAB].cName, "dLL13PhiAB");
  sprintf(options[OPT_LL13PHIAB].cDescr,
          "Binary Initial Mean Anomaly for use in the BINARY module");
  sprintf(options[OPT_LL13PHIAB].cDefault, "0.0 degrees");
  sprintf(options[OPT_LL13PHIAB].cDimension, "angle");
  options[OPT_LL13PHIAB].dDefault   = 0.0;
  options[OPT_LL13PHIAB].iType      = 2;
  options[OPT_LL13PHIAB].bMultiFile = 1;
  fnRead[OPT_LL13PHIAB]             = &ReadLL13PhiAB;

  sprintf(options[OPT_CBPM0].cName, "dCBPM0");
  sprintf(options[OPT_CBPM0].cDescr,
          "Circumbinary planet initial mean anomaly for use in the BINARY "
          "module");
  sprintf(options[OPT_CBPM0].cDefault, "0.0 degrees");
  sprintf(options[OPT_CBPM0].cDimension, "angle");
  options[OPT_CBPM0].dDefault   = 0.0;
  options[OPT_CBPM0].iType      = 2;
  options[OPT_CBPM0].bMultiFile = 1;
  fnRead[OPT_CBPM0]             = &ReadCBPM0;

  sprintf(options[OPT_CBPZETA].cName, "dCBPZeta");
  sprintf(options[OPT_CBPZETA].cDescr,
          "Circumbinary planet initial z oscillation phase angle");
  sprintf(options[OPT_CBPZETA].cDefault, "0.0 degrees");
  sprintf(options[OPT_CBPZETA].cDimension, "angle");
  options[OPT_CBPZETA].dDefault   = 0.0;
  options[OPT_CBPZETA].iType      = 2;
  options[OPT_CBPZETA].bMultiFile = 1;
  fnRead[OPT_CBPZETA]             = &ReadCBPZeta;

  sprintf(options[OPT_CBPPSI].cName, "dCBPPsi");
  sprintf(options[OPT_CBPPSI].cDescr,
          "Circumbinary planet initial R, phi oscillation phase angle");
  sprintf(options[OPT_CBPPSI].cDefault, "0.0 degrees");
  sprintf(options[OPT_CBPPSI].cDimension, "angle");
  options[OPT_CBPPSI].dDefault   = 0.0;
  options[OPT_CBPPSI].iType      = 2;
  options[OPT_CBPPSI].bMultiFile = 1;
  fnRead[OPT_CBPPSI]             = &ReadCBPPsi;

  /* Note: One should never actually set LL13 values as they are ALWAYS
   * calculated during initialization.  I'll leave them here since there
   * were useful for debugging and could be useful in the future if higher
   * order modifications to the theory are added.
   */

  sprintf(options[OPT_LL13N0].cName, "dLL13N0");
  sprintf(options[OPT_LL13N0].cDescr, "Lee+Leung 2013 Mean Motion");
  sprintf(options[OPT_LL13N0].cDefault, "1 /yr");
  sprintf(options[OPT_LL13N0].cDimension, "time^-1");
  options[OPT_LL13N0].dDefault   = 1. / YEARSEC;
  options[OPT_LL13N0].iType      = 2;
  options[OPT_LL13N0].bMultiFile = 1;
  options[OPT_LL13N0].dNeg       = 1. / YEARSEC;
  sprintf(options[OPT_LL13N0].cNeg, "/Year");
  fnRead[OPT_LL13N0] = &ReadLL13N0;

  sprintf(options[OPT_LL13K0].cName, "dLL13K0");
  sprintf(options[OPT_LL13K0].cDescr,
          "Lee+Leung 2013 Radial Epicyclic Frequency");
  sprintf(options[OPT_LL13K0].cDefault, "1 /yr");
  sprintf(options[OPT_LL13K0].cDimension, "time^-1");
  options[OPT_LL13K0].dDefault   = 1. / YEARSEC;
  options[OPT_LL13K0].iType      = 2;
  options[OPT_LL13K0].bMultiFile = 1;
  options[OPT_LL13K0].dNeg       = 1. / YEARSEC;
  sprintf(options[OPT_LL13K0].cNeg, "/Year");
  fnRead[OPT_LL13K0] = &ReadLL13K0;

  sprintf(options[OPT_LL13V0].cName, "dLL13V0");
  sprintf(options[OPT_LL13V0].cDescr,
          "Lee+Leung 2013 Radial Epicyclic Frequency");
  sprintf(options[OPT_LL13V0].cDefault, "1 /yr");
  sprintf(options[OPT_LL13V0].cDimension, "time^-1");
  options[OPT_LL13V0].dDefault   = 1. / YEARSEC;
  options[OPT_LL13V0].iType      = 2;
  options[OPT_LL13V0].bMultiFile = 1;
  options[OPT_LL13V0].dNeg       = 1. / YEARSEC;
  sprintf(options[OPT_LL13V0].cNeg, "/Year");
  fnRead[OPT_LL13V0] = &ReadLL13V0;

  sprintf(options[OPT_HALTHOLMAN].cName, "bHaltHolmanUnstable");
  sprintf(options[OPT_HALTHOLMAN].cDescr,
          "Halt when CBP is Holman-Wiegert Unstable?");
  sprintf(options[OPT_HALTHOLMAN].cDefault, "0");
  options[OPT_HALTHOLMAN].iType = 0;
  fnRead[OPT_HALTHOLMAN]        = &ReadHaltHolmanUnstable;

  sprintf(options[OPT_HALTROCHELOBE].cName, "bHaltRocheLobe");
  sprintf(options[OPT_HALTROCHELOBE].cDescr,
          "Halt when Roche lobe crossing occurs?");
  sprintf(options[OPT_HALTROCHELOBE].cDefault, "0");
  options[OPT_HALTROCHELOBE].iType = 0;
  fnRead[OPT_HALTROCHELOBE]        = &ReadHaltRocheLobe;
}

/** Read all BINARY input options. */
void ReadOptionsBinary(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, fnReadOption fnRead[],
                       int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTBINARY; iOpt < OPTENDBINARY; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}

/******************* Verify BINARY ******************/

/*
 * Note: update always has [0] as the terminal index since ONLY binary can
 * update these variables.  Theory does NOT apply if another phenomena tries
 * to change a circumbinary planet's orbital motion.  Also, only iaBody eqns
 * care about are their own.  CBP only cares about itself since bodies 0 and 1
 * are assured to be stars in binary, hence binary.  This was all done
 * intentionally to make it more difficult to mess up the binary equations.
 */

void VerifyCBPR(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                int iBody) {

  update[iBody].iaType[update[iBody].iCBPR][0]     = 10;
  update[iBody].iNumBodies[update[iBody].iCBPR][0] = 1;
  update[iBody].iaBody[update[iBody].iCBPR][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iCBPR][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iCBPR][0][0] = iBody;

  update[iBody].pdCBPRBinary =
        &update[iBody].daDerivProc[update[iBody].iCBPR][0];
}

void VerifyCBPZ(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                int iBody) {

  update[iBody].iaType[update[iBody].iCBPZ][0]     = 10;
  update[iBody].iNumBodies[update[iBody].iCBPZ][0] = 1;
  update[iBody].iaBody[update[iBody].iCBPZ][0] =
        malloc(update[iBody].iNumBodies[update[iBody].iCBPZ][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iCBPZ][0][0] = iBody;

  update[iBody].pdCBPZBinary =
        &update[iBody].daDerivProc[update[iBody].iCBPZ][0];
}

void VerifyCBPPhi(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                  int iBody) {

  update[iBody].iaType[update[iBody].iCBPPhi][0]     = 10;
  update[iBody].iNumBodies[update[iBody].iCBPPhi][0] = 1;
  update[iBody].iaBody[update[iBody].iCBPPhi][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iCBPPhi][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iCBPPhi][0][0] = iBody;

  update[iBody].pdCBPPhiBinary =
        &update[iBody].daDerivProc[update[iBody].iCBPPhi][0];
}

void VerifyCBPRDot(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                   int iBody) {

  update[iBody].iaType[update[iBody].iCBPRDot][0]     = 10;
  update[iBody].iNumBodies[update[iBody].iCBPRDot][0] = 1;
  update[iBody].iaBody[update[iBody].iCBPRDot][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iCBPRDot][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iCBPRDot][0][0] = iBody;

  update[iBody].pdCBPRDotBinary =
        &update[iBody].daDerivProc[update[iBody].iCBPRDot][0];
}

void VerifyCBPZDot(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                   int iBody) {

  update[iBody].iaType[update[iBody].iCBPZDot][0]     = 10;
  update[iBody].iNumBodies[update[iBody].iCBPZDot][0] = 1;
  update[iBody].iaBody[update[iBody].iCBPZDot][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iCBPZDot][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iCBPZDot][0][0] = iBody;

  update[iBody].pdCBPZDotBinary =
        &update[iBody].daDerivProc[update[iBody].iCBPZDot][0];
}

void VerifyCBPPhiDot(BODY *body, OPTIONS *options, UPDATE *update, double dAge,
                     int iBody) {

  update[iBody].iaType[update[iBody].iCBPPhiDot][0]     = 10;
  update[iBody].iNumBodies[update[iBody].iCBPPhiDot][0] = 1;
  update[iBody].iaBody[update[iBody].iCBPPhiDot][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iCBPPhiDot][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iCBPPhiDot][0][0] = iBody;

  update[iBody].pdCBPPhiDotBinary =
        &update[iBody].daDerivProc[update[iBody].iCBPPhiDot][0];
}

void fnPropsAuxBinary(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                      int iBody) {

  if (body[iBody].iBodyType == 0) { // CBP
    // If not including eqns in the matrix, compute main variables on the fly!
    SYSTEM *system         = NULL;    // dummy variable
    int iaBody[1]          = {iBody}; //  Pick out CBP
    body[iBody].dCBPR      = fndCBPRBinary(body, system, iaBody);
    body[iBody].dCBPZ      = fndCBPZBinary(body, system, iaBody);
    body[iBody].dCBPPhi    = fndCBPPhiBinary(body, system, iaBody);
    body[iBody].dCBPRDot   = fndCBPRDotBinary(body, system, iaBody);
    body[iBody].dCBPPhiDot = fndCBPPhiDotBinary(body, system, iaBody);
    body[iBody].dCBPZDot   = fndCBPZDotBinary(body, system, iaBody);

    // Set CBP orbital elements, mean motion
    fnvAssignOrbitalElements(body, iBody);
    body[iBody].dMeanMotion = fdSemiToMeanMotion(
          body[iBody].dR0, (body[0].dMass + body[1].dMass + body[iBody].dMass));
  } else if (body[iBody].iBodyType == 1 && iBody == 1) { // Binary
    // Correctly set binary's mean motion
    body[iBody].dMeanMotion = fdSemiToMeanMotion(
          body[iBody].dSemi, (body[0].dMass + body[1].dMass));

    // Compute binary's eccentricity from Kecc and Hecc (since they are primary
    // variables)
    body[iBody].dEcc =
          sqrt(pow(body[iBody].dKecc, 2) + pow(body[iBody].dHecc, 2));
    body[iBody].dEccSq = body[iBody].dEcc * body[iBody].dEcc;
  } else {
    return;
  }
}

void fnForceBehaviorBinary(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                           SYSTEM *system, UPDATE *update,
                           fnUpdateVariable ***fnUpdate, int iBody,
                           int iModule) {
  // Anything here?
}

void AssignBinaryDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody) {

  if (body[iBody].iBodyType == 0) // Planets are added to matrix
  {
    fnUpdate[iBody][update[iBody].iCBPR][0]      = &fndCBPRBinary;
    fnUpdate[iBody][update[iBody].iCBPZ][0]      = &fndCBPZBinary;
    fnUpdate[iBody][update[iBody].iCBPPhi][0]    = &fndCBPPhiBinary;
    fnUpdate[iBody][update[iBody].iCBPRDot][0]   = &fndCBPRDotBinary;
    fnUpdate[iBody][update[iBody].iCBPZDot][0]   = &fndCBPZDotBinary;
    fnUpdate[iBody][update[iBody].iCBPPhiDot][0] = &fndCBPPhiDotBinary;
  }
}

void NullBinaryDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                           fnUpdateVariable ***fnUpdate, int iBody) {

  if (body[iBody].iBodyType == 0) // Planets are added to matrix
  {
    // Add equations to the matrix
    fnUpdate[iBody][update[iBody].iCBPR][0]      = &fndUpdateFunctionTiny;
    fnUpdate[iBody][update[iBody].iCBPZ][0]      = &fndUpdateFunctionTiny;
    fnUpdate[iBody][update[iBody].iCBPPhi][0]    = &fndUpdateFunctionTiny;
    fnUpdate[iBody][update[iBody].iCBPRDot][0]   = &fndUpdateFunctionTiny;
    fnUpdate[iBody][update[iBody].iCBPZDot][0]   = &fndUpdateFunctionTiny;
    fnUpdate[iBody][update[iBody].iCBPPhiDot][0] = &fndUpdateFunctionTiny;
  }
}

void VerifyBinary(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  OUTPUT *output, SYSTEM *system, UPDATE *update, int iBody,
                  int iModule) {

  // If binary is being used, ALL bodies must have correct type
  if (iBody < 2) { // Primary or secondary star
    if (body[iBody].iBodyType != 1) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: In binary, bodies 0, 1 iBodyType must be 1 "
                        "(star == 1).\n");
        fprintf(stderr, "Body 2 must be 0 (planet == 0).\n");
        fprintf(stderr, "iBody: %d iBodyType: %d\n", iBody,
                body[iBody].iBodyType);
      }
      exit(EXIT_INPUT);
    }
  } else { // planets
    if (body[iBody].iBodyType != 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: In binary, bodies 0, 1 iBodyType must be 1 "
                        "(star == 1).\n");
        fprintf(stderr, "Body number > 1 must be 0 (planet == 0).\n");
        fprintf(stderr, "iBody: %d iBodyType: %d\n", iBody,
                body[iBody].iBodyType);
      }
      exit(EXIT_INPUT);
    }
  }

  // If binary is being used, ALL bodies must have bBinary == 1
  int i;
  for (i = 0; i < control->Evolve.iNumBodies; i++) {
    if (body[i].bBinary == 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: In binary, all bodies must have bBinary == 1.\n");
        fprintf(stderr, "body[i].bBinary == 0: %d\n", i);
      }
      exit(EXIT_INPUT);
    }
  }

  // Binary *should* only allow 3 bodies: 2 stars, 1 CBP since in LL13, cbps
  // can't interact But if the user wants to, they can have more, but make sure
  // they're warned
  if (control->Evolve.iNumBodies > 3 && iBody > 2) {
    fprintf(stderr,
            "WARNING: In binary, Leung and Lee 2013 is a 3 body problem: 2 "
            "stars, 1 planet. Include additional planets at your own peril!\n");
  }

  // For CBP, cannot have dLL13PhiAB set since that is for the binary!
  if (body[iBody].iBodyType == 0) {
    if (body[iBody].dLL13PhiAB > dTINY) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: The circumbinary planet cannot have dLL13PhiAB "
                        "set as that is the BINARY's initial mean anomaly.\n");
      }
      exit(EXIT_INPUT);
    }
  }

  // For binary, only the secondary should have the orbital elements set!
  // Also, shouldn't have CBPM0 set
  if (body[iBody].iBodyType == 1) {
    if (iBody == 0) { // Primary!
      // These values default to -1
      if (fabs(body[iBody].dInc) + 1 < dTINY ||
          fabs(body[iBody].dEcc) + 1 < dTINY ||
          fabs(body[iBody].dSemi + 1) < dTINY ||
          fabs(body[iBody].dMeanMotion) + 1 < dTINY) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr,
                  "ERROR: In binary, binary orbital element information can "
                  "ONLY be in the secondary star (iBody == 1).\n");
        }
        exit(EXIT_INPUT);
      }
    } else { // Secondary
      // Was dCBPM0, dCBPZeta, dCBPPsi set for one of the stars?
      if (body[iBody].dCBPM0 > dTINY || body[iBody].dCBPZeta > dTINY ||
          body[iBody].dCBPPsi > dTINY) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: In binary, only the CBP can have dCBPM0, "
                          "dCBPZeta, or dCBPPsi set.\n");
        }
        exit(EXIT_INPUT);
      }
    }
  }

  // Initialize the circumbinary planets
  if (body[iBody].iBodyType == 0) { // Planets are added to matrix
    // Call verifies to properly set up eqns in matrix
    VerifyCBPR(body, options, update, body[iBody].dAge, iBody);
    VerifyCBPZ(body, options, update, body[iBody].dAge, iBody);
    VerifyCBPPhi(body, options, update, body[iBody].dAge, iBody);
    VerifyCBPRDot(body, options, update, body[iBody].dAge, iBody);
    VerifyCBPZDot(body, options, update, body[iBody].dAge, iBody);
    VerifyCBPPhiDot(body, options, update, body[iBody].dAge, iBody);

    // Init parameters needed for subsequent cbp motion

    // dR0, dMeanMotion MUST be set before any of the frequencies
    body[iBody].dR0 =
          body[iBody].dSemi; // CBPs Guiding Radius initial equal to dSemi, must
                             // be set before N0,K0,V0 !!!
    body[iBody].dMeanMotion = fdSemiToMeanMotion(
          body[iBody].dR0, (body[0].dMass + body[1].dMass + body[iBody].dMass));
    body[iBody].dLL13N0 = fndMeanMotionBinary(body, iBody);
    body[iBody].dLL13K0 = fndEpiFreqK(body, iBody);
    body[iBody].dLL13V0 = fndEpiFreqV(body, iBody);

    // Set Planet initial positions, velocities according to LL13 theory
    int iaBody[1]          = {iBody}; //  Pick out CBP
    body[iBody].dCBPR      = fndCBPRBinary(body, system, iaBody);
    body[iBody].dCBPZ      = fndCBPZBinary(body, system, iaBody);
    body[iBody].dCBPPhi    = fndCBPPhiBinary(body, system, iaBody);
    body[iBody].dCBPRDot   = fndCBPRDotBinary(body, system, iaBody);
    body[iBody].dCBPPhiDot = fndCBPPhiDotBinary(body, system, iaBody);
    body[iBody].dCBPZDot   = fndCBPZDotBinary(body, system, iaBody);

    // Init orbital elements
    fnvAssignOrbitalElements(body, iBody);
    body[iBody].dSemi =
          body[iBody].dR0; // Fix semi-major axis to be guiding center

    // Set up initial orbital elements that are primary variables
    body[iBody].dHecc = body[iBody].dEcc * sin(body[iBody].dLongP);
    body[iBody].dKecc = body[iBody].dEcc * cos(body[iBody].dLongP);
  }

  // Inits if the body is the secondary (sets required binary parameters)
  if (body[iBody].iBodyType == 1 && iBody == 1) {
    // Set Initial Poincare H, K using imputted dEcc
    body[iBody].dHecc  = body[iBody].dEcc * sin(body[iBody].dLongP);
    body[iBody].dKecc  = body[iBody].dEcc * cos(body[iBody].dLongP);
    body[iBody].dEccSq = body[iBody].dEcc * body[iBody].dEcc;
  }

  // Inits for stars: General
  if (body[iBody].iBodyType == 1) {
    body[iBody].dR0        = 0.0;
    body[iBody].dInc       = 0.0; // Binary in the plane
    body[iBody].dArgP      = 0.0;
    body[iBody].dLongA     = 0.0;
    body[iBody].dCBPR      = 0.0;
    body[iBody].dCBPRDot   = 0.0;
    body[iBody].dCBPZ      = 0.0;
    body[iBody].dCBPZDot   = 0.0;
    body[iBody].dCBPPhi    = 0.0;
    body[iBody].dCBPPhiDot = 0.0;
    body[iBody].dLL13N0    = 0.0;
    body[iBody].dLL13K0    = 0.0;
    body[iBody].dLL13V0    = 0.0;
  }

  // Other things that must be set
  control->fnForceBehavior[iBody][iModule]   = &fnForceBehaviorBinary;
  control->fnPropsAux[iBody][iModule]        = &fnPropsAuxBinary;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyBinary;
}

/**************** BINARY Update ****************/

void InitializeUpdateBinary(BODY *body, UPDATE *update, int iBody) {
  /* Only planets should be in matrix, if the user is so inclinded */
  if (body[iBody].iBodyType == 0) {
    if (update[iBody].iNumCBPR == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumCBPR++;

    if (update[iBody].iNumCBPZ == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumCBPZ++;

    if (update[iBody].iNumCBPPhi == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumCBPPhi++;

    if (update[iBody].iNumCBPRDot == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumCBPRDot++;

    if (update[iBody].iNumCBPZDot == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumCBPZDot++;

    if (update[iBody].iNumCBPPhiDot == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumCBPPhiDot++;
  }
}

void FinalizeUpdateCBPRBinary(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = BINARY;
  update[iBody].iNumCBPR              = (*iEqn)++;
}

void FinalizeUpdateCBPZBinary(BODY *body, UPDATE *update, int *iEqn, int iVar,
                              int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = BINARY;
  update[iBody].iNumCBPZ              = (*iEqn)++;
}

void FinalizeUpdateCBPPhiBinary(BODY *body, UPDATE *update, int *iEqn, int iVar,
                                int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = BINARY;
  update[iBody].iNumCBPPhi            = (*iEqn)++;
}

void FinalizeUpdateCBPRDotBinary(BODY *body, UPDATE *update, int *iEqn,
                                 int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = BINARY;
  update[iBody].iNumCBPRDot           = (*iEqn)++;
}

void FinalizeUpdateCBPZDotBinary(BODY *body, UPDATE *update, int *iEqn,
                                 int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = BINARY;
  update[iBody].iNumCBPZDot           = (*iEqn)++;
}

void FinalizeUpdateCBPPhiDotBinary(BODY *body, UPDATE *update, int *iEqn,
                                   int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = BINARY;
  update[iBody].iNumCBPPhiDot         = (*iEqn)++;
}

/***************** BINARY Halts *****************/

/** If the CBP's dSemi is less than the Holman stability limit, it's unstable
   and integration ends */
int fbHaltHolmanUnstable(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                         UPDATE *update, fnUpdateVariable ***fnUpdate,
                         int iBody) {

  double a_crit = fndHolmanStability(body);

  // If the body is less than critical stability limit
  // Check stability for planets
  if (body[iBody].iBodyType == 0) {
    if (body[iBody].dSemi <= a_crit) {
      if (io->iVerbose >= VERBPROG) {
        fprintf(stderr,
                "HALT: %s's dSemi: %lf AU, Holman-Wiegert critial a: %lf AU.\n",
                body[iBody].cName, body[iBody].dSemi / AUM, a_crit / AUM);
      }
      return 1;
    }
    return 0;
  } else { // Doesn't apply to stars
    return 0;
  }
}

/** If the secondary enters the roche lobe of the primary, HALT! */
int fbHaltRocheLobe(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                    UPDATE *update, fnUpdateVariable ***fnUpdate, int iBody) {

  double r_crit = fndRocheLobe(body);

  // Check for roche lobe crossing
  if (body[iBody].iBodyType == 1 && iBody == 1) {
    if (body[iBody].dSemi <= r_crit) {
      if (io->iVerbose >= VERBPROG) {
        fprintf(stderr,
                "HALT: %s's dSemi: %lf AU, Primary Roche Lobe: %lf AU.\n",
                body[iBody].cName, body[iBody].dSemi / AUM, r_crit / AUM);
      }
      return 1;
    }
    return 0;
  } else { // Doesn't apply to CBP, central star
    return 0;
  }
}

void CountHaltsBinary(HALT *halt, int *iHalt) {
  if (halt->bHaltHolmanUnstable) {
    (*iHalt)++;
  }
  if (halt->bHaltRocheLobe) {
    (*iHalt)++;
  }
}

void VerifyHaltBinary(BODY *body, CONTROL *control, OPTIONS *options, int iBody,
                      int *iHalt) {
  if (control->Halt[iBody].bHaltHolmanUnstable) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltHolmanUnstable;
  }
  if (control->Halt[iBody].bHaltRocheLobe) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltRocheLobe;
  }
}

/************* BINARY Outputs ******************/

void WriteFreeEccBinary(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  // Note: Only makes sense for planets (iBodyType == 0)
  if (body[iBody].iBodyType == 0) {
    *dTmp = body[iBody].dFreeEcc;
  } else {
    *dTmp = -1;
  }

  strcpy(cUnit, "");
}

void WriteFreeIncBinary(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  // Note: Only makes sense for planets (iBodyType == 0)
  if (body[iBody].iBodyType == 0) {
    *dTmp = body[iBody].dFreeInc;
  } else {
    *dTmp = -1;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

/** Write the binary primary star radial position */
void WriteBinPriRBinary(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {

  // Compute current binary true anomaly
  double meanAnomaly = body[1].dMeanMotion * body[0].dAge + body[1].dLL13PhiAB;
  double eccAnomaly  = fndMeanToEccentric(
         meanAnomaly, body[1].dEcc); // Solve Kepler's equation
  double trueAnomaly = fndEccToTrue(eccAnomaly, body[1].dEcc);

  // Compute binary separation
  double radius = body[1].dSemi * (1.0 - body[1].dEcc * body[1].dEcc);
  radius /= (1.0 + body[1].dEcc * cos(trueAnomaly - body[1].dLongP));

  // Radial position of each star (- accounts for 180 deg phase offset)
  double dInvMass = 1.0 / (body[0].dMass + body[1].dMass);

  *dTmp = body[1].dMass * radius * dInvMass;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

/** Write the binary secondary star radial position */
void WriteBinSecRBinary(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {

  // Compute current binary true anomaly
  double meanAnomaly = body[1].dMeanMotion * body[1].dAge + body[1].dLL13PhiAB;
  double eccAnomaly  = fndMeanToEccentric(
         meanAnomaly, body[1].dEcc); // Solve Kepler's equation
  double trueAnomaly = fndEccToTrue(eccAnomaly, body[1].dEcc);

  // Compute binary separation
  double radius = body[1].dSemi * (1.0 - body[1].dEcc * body[1].dEcc);
  radius /= (1.0 + body[1].dEcc * cos(trueAnomaly - body[1].dLongP));

  // Radial position of each star (- accounts for 180 deg phase offset)
  double dInvMass = 1.0 / (body[0].dMass + body[1].dMass);

  *dTmp = body[0].dMass * radius * dInvMass;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}


void WriteBinPriPhiBinary(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {

  // Compute current binary true anomaly
  double meanAnomaly = body[1].dMeanMotion * body[0].dAge + body[1].dLL13PhiAB;
  double eccAnomaly  = fndMeanToEccentric(
         meanAnomaly, body[1].dEcc); // Solve Kepler's equation

  *dTmp = fndEccToTrue(eccAnomaly, body[1].dEcc);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}


void WriteBinSecPhiBinary(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {

  // Compute current binary true anomaly
  double meanAnomaly = body[1].dMeanMotion * body[1].dAge + body[1].dLL13PhiAB;
  double eccAnomaly  = fndMeanToEccentric(
         meanAnomaly, body[1].dEcc); // Solve Kepler's equation

  // Secondary is PI radians away from primary
  *dTmp = fndEccToTrue(eccAnomaly, body[1].dEcc) + PI;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}


void WriteCBPPhiBinary(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  if (body[iBody].iBodyType == 0) {
    *dTmp = body[iBody].dCBPPhi;
  } else {
    *dTmp = -1;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle, cUnit);
  }
}

void WriteCBPPhiDotBinary(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dCBPPhiDot;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}

void WriteLL13N0Binary(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  // Note: Only applies to planets (iBodyType == 0)
  if (body[iBody].iBodyType == 0) {
    *dTmp = body[iBody].dLL13N0;
  } else {
    *dTmp = -1;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
  }
}


void WriteLL13K0Binary(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  // Note: Only applies to planets (iBodyType == 0)
  if (body[iBody].iBodyType == 0) {
    *dTmp = body[iBody].dLL13K0;
  } else {
    *dTmp = -1;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
  }
}

void WriteLL13V0Binary(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  // Note: Only applies to planets (iBodyType == 0)
  if (body[iBody].iBodyType == 0) {
    *dTmp = body[iBody].dLL13V0;
  } else {
    *dTmp = -1;
  }

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsTime(units->iTime, cUnit);
  }
}

/** Write the circumbinary planet orbital radius (CBPR) */
void WriteCBPRBinary(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dCBPR;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

/** Write the circumbinary planet guiding radius (CBPR0) */
void WriteCBPR0Binary(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dR0;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

void WriteCBPZBinary(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dCBPZ;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength, cUnit);
  }
}

void WriteCBPRDotBinary(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dCBPRDot;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime) / fdUnitsLength(units->iLength);
    fsUnitsVel(units, cUnit);
  }
}

void WriteCBPZDotBinary(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {

  *dTmp = body[iBody].dCBPZDot;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime) / fdUnitsLength(units->iLength);
    fsUnitsVel(units, cUnit);
  }
}

/** Write Earth-normalized insolation received by CBP averaged over 1 binary
   orbit assuming P_bin << P_cbp */
void WriteCBPInsol(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UNITS *units, UPDATE *update, int iBody, double *dTmp,
                   char cUnit[]) {

  // Make sure that this is a planet, if not, -1
  if (iBody < 2 || body[iBody].iBodyType != 0) {
    *dTmp = -1;
  } else {
    *dTmp = fndApproxInsol(body, iBody);
  }

  // Always in units of insolation received by Earth
  strcpy(cUnit, "F/F_Earth");
}

void InitializeOutputBinary(OUTPUT *output, fnWriteOutput fnWrite[]) {
  sprintf(output[OUT_FREEECC].cName, "FreeEcc");
  sprintf(output[OUT_FREEECC].cDescr, "CBP's Free Eccentricity in BINARY");
  output[OUT_FREEECC].bNeg       = 0;
  output[OUT_FREEECC].iNum       = 1;
  output[OUT_FREEECC].iModuleBit = BINARY;
  fnWrite[OUT_FREEECC]           = &WriteFreeEccBinary;

  sprintf(output[OUT_FREEINC].cName, "FreeInc");
  sprintf(output[OUT_FREEINC].cDescr, "CBP's Free Inclination in BINARY");
  sprintf(output[OUT_FREEINC].cNeg, "Deg");
  output[OUT_FREEINC].bNeg       = 1;
  output[OUT_FREEINC].dNeg       = 1. / DEGRAD;
  output[OUT_FREEINC].iNum       = 1;
  output[OUT_FREEINC].iModuleBit = BINARY;
  fnWrite[OUT_FREEINC]           = &WriteFreeIncBinary;

  sprintf(output[OUT_CBPPHI].cName, "CBPPhi");
  sprintf(output[OUT_CBPPHI].cDescr, "CBP Orbital Azimuthal Angle in BINARY");
  sprintf(output[OUT_CBPPHI].cNeg, "Deg");
  output[OUT_CBPPHI].bNeg       = 1;
  output[OUT_CBPPHI].dNeg       = 1.0 / DEGRAD;
  output[OUT_CBPPHI].iNum       = 1;
  output[OUT_CBPPHI].iModuleBit = BINARY;
  fnWrite[OUT_CBPPHI]           = &WriteCBPPhiBinary;

  sprintf(output[OUT_BINPRIPHI].cName, "BinPriPhi");
  sprintf(output[OUT_BINPRIPHI].cDescr,
          "Binary primary star azimuthal angle in BINARY");
  sprintf(output[OUT_BINPRIPHI].cNeg, "Deg");
  output[OUT_BINPRIPHI].bNeg       = 1;
  output[OUT_BINPRIPHI].dNeg       = 1.0 / DEGRAD;
  output[OUT_BINPRIPHI].iNum       = 1;
  output[OUT_BINPRIPHI].iModuleBit = BINARY;
  fnWrite[OUT_BINPRIPHI]           = &WriteBinPriPhiBinary;

  sprintf(output[OUT_BINSECPHI].cName, "BinSecPhi");
  sprintf(output[OUT_BINSECPHI].cDescr,
          "Binary secondary star azimuthal angle in BINARY");
  sprintf(output[OUT_BINSECPHI].cNeg, "Deg");
  output[OUT_BINSECPHI].bNeg       = 1;
  output[OUT_BINSECPHI].dNeg       = 1.0 / DEGRAD;
  output[OUT_BINSECPHI].iNum       = 1;
  output[OUT_BINSECPHI].iModuleBit = BINARY;
  fnWrite[OUT_BINSECPHI]           = &WriteBinSecPhiBinary;

  sprintf(output[OUT_LL13N0].cName, "LL13N0");
  sprintf(output[OUT_LL13N0].cDescr, "CBP Mean Motion in BINARY");
  sprintf(output[OUT_LL13N0].cNeg, "1/year");
  output[OUT_LL13N0].bNeg       = 1;
  output[OUT_LL13N0].dNeg       = 1. / YEARSEC;
  output[OUT_LL13N0].iNum       = 1;
  output[OUT_LL13N0].iModuleBit = BINARY;
  fnWrite[OUT_LL13N0]           = &WriteLL13N0Binary;

  sprintf(output[OUT_LL13K0].cName, "LL13K0");
  sprintf(output[OUT_LL13K0].cDescr,
          "Leung+Lee 2013 Radial epicyclic frequency");
  sprintf(output[OUT_LL13K0].cNeg, "1/year");
  output[OUT_LL13K0].bNeg       = 1;
  output[OUT_LL13K0].dNeg       = 1. / YEARSEC;
  output[OUT_LL13K0].iNum       = 1;
  output[OUT_LL13K0].iModuleBit = BINARY;
  fnWrite[OUT_LL13K0]           = &WriteLL13K0Binary;

  sprintf(output[OUT_LL13V0].cName, "LL13V0");
  sprintf(output[OUT_LL13V0].cDescr,
          "CBP vertical epicyclic frequency in BINARY");
  sprintf(output[OUT_LL13V0].cNeg, "1/year");
  output[OUT_LL13V0].bNeg       = 1;
  output[OUT_LL13V0].dNeg       = 1. / YEARSEC;
  output[OUT_LL13V0].iNum       = 1;
  output[OUT_LL13V0].iModuleBit = BINARY;
  fnWrite[OUT_LL13V0]           = &WriteLL13V0Binary;

  sprintf(output[OUT_CBPR].cName, "CBPR");
  sprintf(output[OUT_CBPR].cDescr, "CBP's Orbital Radius in BINARY");
  output[OUT_CBPR].bNeg = 1;
  sprintf(output[OUT_CBPR].cNeg, "AU");
  output[OUT_CBPR].dNeg       = 1.0 / AUM;
  output[OUT_CBPR].iNum       = 1;
  output[OUT_CBPR].iModuleBit = BINARY;
  fnWrite[OUT_CBPR]           = &WriteCBPRBinary;

  sprintf(output[OUT_BINPRIR].cName, "BinPriR");
  sprintf(output[OUT_BINPRIR].cDescr,
          "Radial position of binary primary star for module BINARY");
  output[OUT_BINPRIR].bNeg = 1;
  sprintf(output[OUT_BINPRIR].cNeg, "AU");
  output[OUT_BINPRIR].dNeg       = 1.0 / AUM;
  output[OUT_BINPRIR].iNum       = 1;
  output[OUT_BINPRIR].iModuleBit = BINARY;
  fnWrite[OUT_BINPRIR]           = &WriteBinPriRBinary;

  sprintf(output[OUT_BINSECR].cName, "BinSecR");
  sprintf(output[OUT_BINSECR].cDescr,
          "Radial position of binary secondary star for module BINARY");
  output[OUT_BINSECR].bNeg = 1;
  sprintf(output[OUT_BINSECR].cNeg, "AU");
  output[OUT_BINSECR].dNeg       = 1.0 / AUM;
  output[OUT_BINSECR].iNum       = 1;
  output[OUT_BINSECR].iModuleBit = BINARY;
  fnWrite[OUT_BINSECR]           = &WriteBinSecRBinary;

  sprintf(output[OUT_CBPR0].cName, "R0");
  sprintf(output[OUT_CBPR0].cDescr,
          "CBP's Orbital Guiding Center Radius in BINARY");
  output[OUT_CBPR0].bNeg = 1;
  sprintf(output[OUT_CBPR0].cNeg, "AU");
  output[OUT_CBPR0].dNeg       = 1.0 / AUM;
  output[OUT_CBPR0].iNum       = 1;
  output[OUT_CBPR0].iModuleBit = BINARY;
  fnWrite[OUT_CBPR0]           = &WriteCBPR0Binary;

  sprintf(output[OUT_CBPZ].cName, "CBPZ");
  sprintf(
        output[OUT_CBPZ].cDescr,
        "CBP's Orbital Cylindrical Height Out of the Orbital Plane in BINARY");
  output[OUT_CBPZ].bNeg = 1;
  sprintf(output[OUT_CBPZ].cNeg, "AU");
  output[OUT_CBPZ].dNeg       = 1.0 / AUM;
  output[OUT_CBPZ].iNum       = 1;
  output[OUT_CBPZ].iModuleBit = BINARY;
  fnWrite[OUT_CBPZ]           = &WriteCBPZBinary;

  sprintf(output[OUT_CBPRDOT].cName, "CBPRDot");
  sprintf(output[OUT_CBPRDOT].cDescr,
          "CBP's Radial Orbital Velocity in BINARY");
  sprintf(output[OUT_CBPRDOT].cNeg, "/day");
  output[OUT_CBPRDOT].bNeg       = 0;
  output[OUT_CBPRDOT].dNeg       = DAYSEC;
  output[OUT_CBPRDOT].iNum       = 1;
  output[OUT_CBPRDOT].iModuleBit = BINARY;
  fnWrite[OUT_CBPRDOT]           = &WriteCBPRDotBinary;

  sprintf(output[OUT_CBPZDOT].cName, "CBPZDot");
  sprintf(output[OUT_CBPZDOT].cDescr, "CBP's Z Orbital Velocity in BINARY");
  sprintf(output[OUT_CBPZDOT].cNeg, "/day");
  output[OUT_CBPZDOT].bNeg       = 0;
  output[OUT_CBPZDOT].dNeg       = DAYSEC;
  output[OUT_CBPZDOT].iNum       = 1;
  output[OUT_CBPZDOT].iModuleBit = BINARY;
  fnWrite[OUT_CBPZDOT]           = &WriteCBPZDotBinary;

  sprintf(output[OUT_CBPPHIDOT].cName, "CBPPhiDot");
  sprintf(output[OUT_CBPPHIDOT].cDescr,
          "CBP's Phi Angular Orbital Velocity in BINARY");
  sprintf(output[OUT_CBPPHIDOT].cNeg, "/day");
  output[OUT_CBPPHIDOT].bNeg       = 0;
  output[OUT_CBPPHIDOT].dNeg       = DAYSEC;
  output[OUT_CBPPHIDOT].iNum       = 1;
  output[OUT_CBPPHIDOT].iModuleBit = BINARY;
  fnWrite[OUT_CBPPHIDOT]           = &WriteCBPPhiDotBinary;

  sprintf(output[OUT_CBPINSOL].cName, "CBPInsol");
  sprintf(output[OUT_CBPINSOL].cDescr,
          "CBP's binary Orbit-Averaged Insolation in BINARY");
  output[OUT_CBPINSOL].bNeg       = 0;
  output[OUT_CBPINSOL].iNum       = 1;
  output[OUT_CBPINSOL].iModuleBit = BINARY;
  fnWrite[OUT_CBPINSOL]           = &WriteCBPInsol;
}

/************ BINARY Logging Functions **************/

void LogOptionsBinary(CONTROL *control, FILE *fp) {

  fprintf(fp, "-------- BINARY Options -----\n\n");
}

/** Anything here? Nope. */
void LogBinary(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {
}

void LogBodyBinary(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                   UPDATE *update, fnWriteOutput fnWrite[], FILE *fp,
                   int iBody) {
  int iOut;
  fprintf(fp, "----- BINARY PARAMETERS (%s)------\n", body[iBody].cName);

  for (iOut = OUTSTARTBINARY; iOut < OUTENDBINARY; iOut++) {
    if (output[iOut].iNum > 0) {
      // Useful for debugging
      // printf("%d\n",iOut);
      // fflush(stdout);
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
  }
}

void AddModuleBinary(CONTROL *control, MODULE *module, int iBody, int iModule) {

  module->iaModule[iBody][iModule] = BINARY;

  module->fnInitializeUpdateTmpBody[iBody][iModule] =
        &InitializeUpdateTmpBodyBinary;

  module->fnCountHalts[iBody][iModule]        = &CountHaltsBinary;
  module->fnReadOptions[iBody][iModule]       = &ReadOptionsBinary;
  module->fnLogBody[iBody][iModule]           = &LogBodyBinary;
  module->fnVerify[iBody][iModule]            = &VerifyBinary;
  module->fnAssignDerivatives[iBody][iModule] = &AssignBinaryDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullBinaryDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltBinary;

  module->fnInitializeBody[iBody][iModule]   = &InitializeBodyBinary;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateBinary;

  // Update functions
  module->fnFinalizeUpdateCBPR[iBody][iModule]   = &FinalizeUpdateCBPRBinary;
  module->fnFinalizeUpdateCBPZ[iBody][iModule]   = &FinalizeUpdateCBPZBinary;
  module->fnFinalizeUpdateCBPPhi[iBody][iModule] = &FinalizeUpdateCBPPhiBinary;
  module->fnFinalizeUpdateCBPPhiDot[iBody][iModule] =
        &FinalizeUpdateCBPPhiDotBinary;
  module->fnFinalizeUpdateCBPRDot[iBody][iModule] =
        &FinalizeUpdateCBPRDotBinary;
  module->fnFinalizeUpdateCBPZDot[iBody][iModule] =
        &FinalizeUpdateCBPZDotBinary;
}

/************* BINARY Functions ************/

/*
 * Useful math functions
 */

/** 3D dot product */
double fndDot(double *a, double *b) {
  return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

/** Kronecker Delta */
int fniDelta(int i, int j) {
  if (i == j) {
    return 1;
  } else {
    return 0;
  }
}

/** Convert from cylindrical position coords to cartesian (3 dimensional) */
void fnvCylToCartPos(double *daCylPos, double *dCartPos) {
  dCartPos[0] = daCylPos[0] * cos(daCylPos[1]);
  dCartPos[1] = daCylPos[0] * sin(daCylPos[1]);
  dCartPos[2] = daCylPos[2];
}

/** Convert from cylindrical velocity coords to cartesian (3 dimensional) */
void fnvCylToCartVel(double *daCylPos, double *daCylVel, double *dCartVel) {
  dCartVel[0] = daCylVel[0] * cos(daCylPos[1]) -
                daCylPos[0] * sin(daCylPos[1]) * daCylVel[1];
  dCartVel[1] = daCylVel[0] * sin(daCylPos[1]) +
                daCylPos[0] * cos(daCylPos[1]) * daCylVel[1];
  dCartVel[2] = daCylVel[2];
}

/*
 * Functions for calculating orbital parameters assuming that the barycenter
 * of the binary is at the origin
 */

/** Calculate specific angular momentum
     h = r x v in cartesian coords */
void fnvSpecificAngMom(double *r, double *v, double *h) {
  // No return since resultant vector, h, is supplied
  cross(r, v, h); // r x v -> h
}

/** Calculate specific orbital energy
    eps = v^2/2 - mu/|r| */
double fndSpecificOrbEng(BODY *body, int iBody) {
  // For binary, iBody 0, 1 == stars, 2 == planet
  double mu   = BIGG * (body[0].dMass + body[1].dMass +
                      body[iBody].dMass); // Gravitational parameter
  double r[3] = {body[iBody].dCBPR, body[iBody].dCBPPhi, body[iBody].dCBPZ};
  double v[3] = {body[iBody].dCBPRDot, body[iBody].dCBPPhiDot,
                 body[iBody].dCBPZDot};
  double rCart[3];
  double vCart[3];

  // Convert from cyl->cart coords
  fnvCylToCartPos(r, rCart);
  fnvCylToCartVel(r, v, vCart);

  double r_norm = sqrt(fndDot(rCart, rCart));

  return fndDot(vCart, vCart) / 2.0 - (mu / r_norm);
}

/** Compute and assign CBP's orbital elements */
void fnvAssignOrbitalElements(BODY *body, int iBody) {
  body[iBody].dSemi = fndComputeSemi(body, iBody);
  body[iBody].dEcc  = fndComputeEcc(body, iBody);
  body[iBody].dInc  = fndComputeInc(body, iBody);

  // LongA, ArgP -> LongP (needed for dHecc, dKecc)
  body[iBody].dLongA = fndComputeLongA(body, iBody);
  body[iBody].dArgP  = fndComputeArgPeri(body, iBody);
  double LongP       = body[iBody].dLongA + body[iBody].dArgP;
  while (LongP > 2.0 * PI) {
    LongP -= 2.0 * PI;
  }
  while (LongP < 0.0) {
    LongP += 2.0 * PI;
  }
  body[iBody].dLongP = LongP;

  body[iBody].dEccSq = body[iBody].dEcc * body[iBody].dEcc;

  // Set Poincare H, K
  body[iBody].dHecc = body[iBody].dEcc * sin(body[iBody].dLongP);
  body[iBody].dKecc = body[iBody].dEcc * cos(body[iBody].dLongP);
}

/** Compute CBP's semi-major axis
    a = -mu/(2*eps) */
double fndComputeSemi(BODY *body, int iBody) {
  // For binary, iBody 0, 1 == stars, 2 == planet
  return -BIGG * (body[0].dMass + body[1].dMass + body[iBody].dMass) /
         (2.0 * fndSpecificOrbEng(body, iBody));
}

/** Compute CBP's orbital eccentricity
    e = sqrt(1 + 2*eps*h*h/(mu*mu)) */
double fndComputeEcc(BODY *body, int iBody) {
  // For binary, iBody 0, 1 == stars, 2 == planet
  double mu = BIGG * (body[0].dMass + body[1].dMass +
                      body[iBody].dMass); // Gravitational parameter
  double h[3];
  double r[3] = {body[iBody].dCBPR, body[iBody].dCBPPhi, body[iBody].dCBPZ};
  double v[3] = {body[iBody].dCBPRDot, body[iBody].dCBPPhiDot,
                 body[iBody].dCBPZDot};
  double rCart[3];
  double vCart[3];

  // Convert from cyl->cart coords
  fnvCylToCartPos(r, rCart);
  fnvCylToCartVel(r, v, vCart);
  fnvSpecificAngMom(rCart, vCart, h);

  return sqrt(1. +
              (2. * fndSpecificOrbEng(body, iBody) * fndDot(h, h)) / (mu * mu));
}

/** Compute CBP's inclincation
    i = arccos(h_z/|h|) */
double fndComputeInc(BODY *body, int iBody) {
  double h[3];
  double r[3] = {body[iBody].dCBPR, body[iBody].dCBPPhi, body[iBody].dCBPZ};
  double v[3] = {body[iBody].dCBPRDot, body[iBody].dCBPPhiDot,
                 body[iBody].dCBPZDot};
  double rCart[3];
  double vCart[3];

  // Convert from cyl->cart coords
  fnvCylToCartPos(r, rCart);
  fnvCylToCartVel(r, v, vCart);

  fnvSpecificAngMom(rCart, vCart, h);
  return acos(h[2] / sqrt(fndDot(h, h)));
}

/** Compute a CBP's longitude of ascending node
    See: https://en.wikipedia.org/wiki/Longitude_of_the_ascending_node */
double fndComputeLongA(BODY *body, int iBody) {
  double Omega = 0.0;
  double h[3];
  double r[3] = {body[iBody].dCBPR, body[iBody].dCBPPhi, body[iBody].dCBPZ};
  double v[3] = {body[iBody].dCBPRDot, body[iBody].dCBPPhiDot,
                 body[iBody].dCBPZDot};
  double rCart[3];
  double vCart[3];

  // Convert from cyl->cart coords
  fnvCylToCartPos(r, rCart);
  fnvCylToCartVel(r, v, vCart);

  fnvSpecificAngMom(rCart, vCart, h);
  double n[3] = {-h[1], h[0], 0};

  // Case: |n| = 0
  double mag_n = sqrt(fndDot(n, n));
  if (fabs(mag_n) < dTINY) {
    return Omega;
  }

  if (n[1] >= 0) {
    Omega = acos(n[0] / mag_n);
  } else {
    Omega = 2.0 * PI - acos(n[0] / mag_n);
  }

  return Omega;
}

/** Compute a CBP's eccentricity vector */
void fnvComputeEccVector(BODY *body, double *evec, int iBody) {
  double mu = BIGG * (body[0].dMass + body[1].dMass +
                      body[iBody].dMass); // Gravitational parameter
  double h[3];

  double r[3] = {body[iBody].dCBPR, body[iBody].dCBPPhi, body[iBody].dCBPZ};
  double v[3] = {body[iBody].dCBPRDot, body[iBody].dCBPPhiDot,
                 body[iBody].dCBPZDot};
  double rCart[3];
  double vCart[3];
  int i;

  // Convert from cyl->cart coords
  fnvCylToCartPos(r, rCart);
  fnvCylToCartVel(r, v, vCart);

  fnvSpecificAngMom(rCart, vCart, h);
  cross(vCart, h, evec);

  double mag_r = sqrt(fndDot(rCart, rCart));

  for (i = 0; i < 3; i++) {
    evec[i] = evec[i] / mu - rCart[i] / mag_r;
  }
}

/** Compute a CBP's argument of pericenter */
double fndComputeArgPeri(BODY *body, int iBody) {
  double evec[3];
  double h[3];

  double r[3] = {body[iBody].dCBPR, body[iBody].dCBPPhi, body[iBody].dCBPZ};
  double v[3] = {body[iBody].dCBPRDot, body[iBody].dCBPPhiDot,
                 body[iBody].dCBPZDot};
  double rCart[3];
  double vCart[3];

  // Convert from cyl->cart coords
  fnvCylToCartPos(r, rCart);
  fnvCylToCartVel(r, v, vCart);

  fnvSpecificAngMom(rCart, vCart, h);
  fnvComputeEccVector(body, evec, iBody);
  double n[3]     = {-h[1], h[0], 0};
  double mag_n    = sqrt(fndDot(n, n));
  double mag_evec = sqrt(fndDot(evec, evec));

  // if LongA ~ 0, assume planar orbit
  if (fabs(fndComputeLongA(body, iBody)) < dTINY) {
    if (h[2] > 0) {
      return atan2(evec[1], evec[0]);
    } else {
      return 2.0 * PI - atan2(evec[1], evec[0]);
    }
  } else {
    if (evec[2] >= 0) {
      return acos(fndDot(n, evec) / (mag_n * mag_evec));
    } else {
      return 2.0 * PI - acos(fndDot(n, evec) / (mag_n * mag_evec));
    }
  }
}

/**
Solves kepler's equation via Newton's method

@param M double, Mean anomaly
@param e, eccentricity
*/
double fndMeanToEccentric(double M, double e) {
  // Make sure M in [0,2pi)
  M = fmod(M, 2.0 * PI);

  // If e is 0, or close enough, return
  if (e < dTINY) {
    return M;
  } else if (e >= 1 ||
             e < 0) { // Should never happen, but better safe than sorry
    fprintf(stderr,
            "ERROR: in fndMeanToEccentric (binary), eccentricity must be "
            "within [0,1). e: %e\n",
            e);
    exit(1);
  }

  double E0 = M / (1.0 - e) -
              e * pow(M, 3.) /
                    (6. * pow(1. - e, 4.)); // First guess via series expansion
  double E  = E0;
  double dE = KEQNTOL + 1.0;
  int count = 0;

  while (dE > KEQNTOL) {
    // Compute E_n+1 (E) from E_n (E0)
    E  = E0 - (E0 - e * sin(E0) - M) / (1. - e * cos(E0));
    dE = fabs(E - E0);
    E0 = E;

    count += 1;

    if (count >=
        MAX_KEPLER_ITERS) { // Stop if too many iterations (should never happen)
      fprintf(stderr, "ERROR: in fndMeanToEccentric, too many iterations to "
                      "solve Kepler Equation\n");
      fprintf(stderr, "Iteration number: %d.  Eccentric anomaly: %lf.\n", count,
              E);
      exit(1);
    }
  }

  return E;
}


/** Convert eccentric anomaly to true anomaly */
double fndEccToTrue(double E, double e) {
  return 2.0 * atan2(sqrt(1. - e) * cos(E / 2.), sqrt(1. + e) * sin(E / 2.));
}

/** Compute the dynamical stability limit, a_crit, first computed by
    Holman and Wiegert 1999 that depends on binary dSemi, dEcc
    If CBP.dSemi < a_crit, planet is unstable and system should halt */
double fndHolmanStability(BODY *body) {
  double a  = body[1].dSemi;
  double e  = body[1].dEcc;
  double mu = body[1].dMass / (body[0].dMass + body[1].dMass);

  return (1.6 + 5.1 * e - 2.22 * e * e + 4.12 * mu - 4.27 * e * mu -
          5.09 * mu * mu + 4.61 * e * e * mu * mu) *
         a;
}

/** Compute the roche lobe of the primary according to Egglton's formula given
    in https://en.wikipedia.org/wiki/Roche_lobe */
double fndRocheLobe(BODY *body) {
  double q     = body[0].dMass / body[1].dMass;
  double num   = 0.49 * pow(q, 2. / 3.);
  double denom = 0.6 * pow(q, 2. / 3.) + log(1.0 + pow(q, 1. / 3.));

  double rochelobe = (num / denom) * body[1].dSemi;

  // Enforce hard radius limit
  if (rochelobe < body[0].dRadius) {
    return body[0].dRadius;
  } else {
    return rochelobe;
  }
}

/** Compute the binary mean anomaly M = n*t + phi where phi is an arbitrary
   offset Note: When used with the binary, dPhi == dLL13PhiAB */
double fndBinaryMeanAnomaly(double dMeanMotion, double dTime, double dPhi) {
  return dMeanMotion * dTime + dPhi;
}

// Below are functions Russell Dietrick already defined in distorb that i'll
// reuse double fdLaplaceCoeff(double dAxRatio, int iIndexJ, double dIndexS)
// double fndDerivLaplaceCoeff(int iNthDeriv, double dAxRatio, int iIndexJ,
// double dIndexS)
// Note: for Laplace Coeff functions, they go as b^J_s(alpha) where J is an int,
// S is a half int double !!

/*
 * Analytic equations from Leung+Lee 2013 that govern circumbinary planet (cbp)
 * evolution
 */

/** LL13 N0 */
double fndMeanMotionBinary(BODY *body, int iBody) {
  // Define intermediate quantities
  double M      = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi * body[1].dMass / M) / body[iBody].dR0;
  double alphab = (body[1].dSemi * body[0].dMass / M) / body[iBody].dR0;

  double N0 = body[iBody].dMeanMotion * body[iBody].dMeanMotion / 2.;

  double tmp1 = body[0].dMass * fndLaplaceCoeff(alphaa, 0, 0.5) / M;
  tmp1 += body[1].dMass * fndLaplaceCoeff(alphab, 0, 0.5) / M;

  double tmp2 = body[0].dMass * body[1].dMass * body[1].dSemi /
                (M * M * body[iBody].dR0);
  tmp2 *= (fndDerivLaplaceCoeff(1, alphaa, 0, 0.5) +
           fndDerivLaplaceCoeff(1, alphab, 0, 0.5));

  return sqrt(N0 * (tmp1 + tmp2));
}

/** LL13 K0 */
double fndEpiFreqK(BODY *body, int iBody) {
  // Define intermediate quantities
  double M      = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi * body[1].dMass / M) / body[iBody].dR0;
  double alphab = (body[1].dSemi * body[0].dMass / M) / body[iBody].dR0;

  double K0 = body[iBody].dMeanMotion * body[iBody].dMeanMotion / 2.;

  double tmp1 = body[0].dMass * fndLaplaceCoeff(alphaa, 0, 0.5) / M;
  tmp1 += body[1].dMass * fndLaplaceCoeff(alphab, 0, 0.5) / M;

  double tmp2 = body[0].dMass * body[1].dMass * body[1].dSemi /
                (M * M * body[iBody].dR0);
  tmp2 *= (fndDerivLaplaceCoeff(1, alphaa, 0, 0.5) +
           fndDerivLaplaceCoeff(1, alphab, 0, 0.5));

  double tmp3 = body[0].dMass * body[1].dMass * body[1].dSemi * body[1].dSemi /
                (M * M * body[iBody].dR0 * body[iBody].dR0);

  double tmp4 = body[1].dMass * fndDerivLaplaceCoeff(2, alphaa, 0, 0.5) / M;
  tmp4 += body[0].dMass * fndDerivLaplaceCoeff(2, alphab, 0, 0.5) / M;
  tmp3 *= tmp4;

  K0 *= (tmp1 - tmp2 - tmp3);
  return sqrt(K0);
}

/** LL13 V0 */
double fndEpiFreqV(BODY *body, int iBody) {
  // Define intermediate quantities
  double M      = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi * body[1].dMass / M) / body[iBody].dR0;
  double alphab = (body[1].dSemi * body[0].dMass / M) / body[iBody].dR0;

  double V0 = body[iBody].dMeanMotion * body[iBody].dMeanMotion / 2.;

  double tmp1 = body[0].dMass * fndLaplaceCoeff(alphaa, 0, 1.5) / M;
  tmp1 += body[1].dMass * fndLaplaceCoeff(alphab, 0, 1.5) / M;
  return sqrt(V0 * tmp1);
}

/** Circular (azimuthal) motion of the guiding center for a cbp: phi0
    dVarPhi here is equal to dCBPM0, the initial CBP mean anomaly
    LL13 Eqn 20 */
double fndPhi0(double dTime, double dMeanMotion, double dVarPhi) {
  return dMeanMotion * dTime + dVarPhi;
}

/*
 * Functions to compute binary potentials and their derivatives w.r.t. radius
 */

/** LL13 Eqn 15: Binary potential component 0 */
double fndPot0(int j, int k, double R, BODY *body) {

  // Define intermediate quantities
  double M      = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi * body[1].dMass / M) / R;
  double alphab = (body[1].dSemi * body[0].dMass / M) / R;

  // 2nd order binary eccentricity correction
  alphaa *= (1.0 + body[1].dEcc * body[1].dEcc / 2.);
  alphab *= (1.0 + body[1].dEcc * body[1].dEcc / 2.);

  double coeff = -(2. - fniDelta(k, 0)) / 2.;
  coeff *= BIGG * (body[0].dMass + body[1].dMass) / R;

  double tmp1 = pow(-1., k) * body[0].dMass *
                fndLaplaceCoeff(alphaa, k, (j + 1.) / 2.) / M;
  tmp1 += body[1].dMass * fndLaplaceCoeff(alphab, k, (j + 1.) / 2.) / M;

  return coeff * tmp1;
}

/** LL13 Eqn 15: d/dR of binary potential component 0 */
double fndPot0dR(int j, int k, double R, BODY *body) {

  // Define intermediate quantities
  double M      = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi * body[1].dMass / M) / R;
  double alphab = (body[1].dSemi * body[0].dMass / M) / R;

  // 2nd order binary eccentricity correction
  alphaa *= (1.0 + body[1].dEcc * body[1].dEcc / 2.);
  alphab *= (1.0 + body[1].dEcc * body[1].dEcc / 2.);

  double coeff = -(2. - fniDelta(k, 0)) / 2.;

  double f = pow(-1., k) * body[0].dMass *
             fndLaplaceCoeff(alphaa, k, (j + 1.) / 2.) / M;
  f += body[1].dMass * fndLaplaceCoeff(alphab, k, (j + 1.) / 2.) / M;
  double g = BIGG * (body[0].dMass + body[1].dMass) / R;

  double g_prime = -g / R;
  double f_prime = -pow(-1., k) * body[0].dMass * alphaa * R *
                   fndDerivLaplaceCoeff(1, alphaa, k, (j + 1.) / 2.) /
                   (R * R * M);
  f_prime += -body[1].dMass * alphab * R *
             fndDerivLaplaceCoeff(1, alphab, k, (j + 1.) / 2.) / (R * R * M);

  return coeff * (f_prime * g + g_prime * f);
}

/** LL13 eqn 16: Binary potential component 1 */
double fndPot1(int j, int k, double R, BODY *body) {

  // Define intermediate quantities
  double M      = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi * body[1].dMass / M) / R;
  double alphab = (body[1].dSemi * body[0].dMass / M) / R;

  // 2nd order binary eccentricity correction
  alphaa *= (1.0 + body[1].dEcc * body[1].dEcc / 2.);
  alphab *= (1.0 + body[1].dEcc * body[1].dEcc / 2.);

  double coeff = -(2. - fniDelta(k, 0)) / 2.;
  coeff *= BIGG * (body[0].dMass + body[1].dMass) / R;

  double tmp1 = pow(-1., k) * body[0].dMass * alphaa *
                fndDerivLaplaceCoeff(1, alphaa, k, (j + 1.) / 2.) / M;
  tmp1 += body[1].dMass * alphab *
          fndDerivLaplaceCoeff(1, alphab, k, (j + 1.) / 2.) / M;

  return coeff * tmp1;
}

/** LL13 eqn 16: d/dR of binary potential component 1 */
double fndPot1dR(int j, int k, double R, BODY *body) {

  // Define intermediate quantities
  double M      = body[0].dMass + body[1].dMass;
  double alphaa = (body[1].dSemi * body[1].dMass / M) / R;
  double alphab = (body[1].dSemi * body[0].dMass / M) / R;

  // 2nd order binary eccentricity correction
  alphaa *= (1.0 + body[1].dEcc * body[1].dEcc / 2.);
  alphab *= (1.0 + body[1].dEcc * body[1].dEcc / 2.);

  double coeff = -(2. - fniDelta(k, 0)) / 2.;

  // Break up big terms into multiple terms: x
  double f = pow(-1., k) * body[0].dMass * alphaa / M;
  double g = fndDerivLaplaceCoeff(1, alphaa, k, (j + 1.) / 2.);
  double h = body[1].dMass * alphab / M;
  double i = fndDerivLaplaceCoeff(1, alphab, k, (j + 1.) / 2.);
  double l = BIGG * (body[0].dMass + body[1].dMass) / R;

  // Derivatives of the above terms w.r.t. R (and hence, alphas): xp
  double fp = -pow(-1., k) * body[0].dMass * alphaa / (R * M);
  double gp = -alphaa * fndDerivLaplaceCoeff(2, alphaa, k, (j + 1.) / 2.) / R;
  double hp = -body[1].dMass * alphab / (R * M);
  double ip = -alphab * fndDerivLaplaceCoeff(2, alphab, k, (j + 1.) / 2.) / R;
  double lp = -BIGG * (body[0].dMass + body[1].dMass) / (R * R);

  return coeff *
         ((fp * g + gp * f + hp * i + ip * h) * l + lp * (f * g + h * i));
}

/** LL13 function defined in paragraph in between eqn 24 and 25 which is like
 *  the mean motion at R */
double fndn(double R, BODY *body) {
  return sqrt(fndPot0dR(0, 0, R, body) / R);
}

/*
 * Definitions for C, D terms from LL13
 * They are essentially combinations of different potential components
 * and their derivatives w.r.t R evaluated at the location of the cbp
 */

/** LL13 C0 as defined in eqn 28 */
double fndC0(BODY *body, int iBody) {

  double c = -body[1].dEcc * fndPot1dR(0, 0, body[iBody].dR0, body) /
             body[iBody].dR0;
  c /= (body[iBody].dLL13K0 * body[iBody].dLL13K0 -
        body[1].dMeanMotion * body[1].dMeanMotion);
  return c;
}

/** LL13 C^0_k as defined by eqn 29 */
double fndC0k(int k, BODY *body, int iBody) {

  double n = fndn(body[iBody].dR0, body);
  double c = fndPot0dR(0, k, body[iBody].dR0, body) +
             (2. * n * fndPot0(0, k, body[iBody].dR0, body)) /
                   (body[iBody].dR0 * (n - body[1].dMeanMotion));
  c /= body[iBody].dR0;
  c /= (body[iBody].dLL13K0 * body[iBody].dLL13K0 -
        k * k * pow(body[iBody].dLL13N0 - body[1].dMeanMotion, 2));
  return c;
}

/** LL13 C^+_k as defined by eqn 30a (the + term) */
double fndCPk(int k, BODY *body, int iBody) {

  double n    = fndn(body[iBody].dR0, body);
  double tmp1 = k * fndPot0dR(0, k, body[iBody].dR0, body) -
                0.5 * fndPot1dR(0, k, body[iBody].dR0, body);
  double tmp2 = k * n *
                (2. * k * fndPot0(0, k, body[iBody].dR0, body) -
                 fndPot1(0, k, body[iBody].dR0, body));
  tmp2 /= body[iBody].dR0 * (k * n - (k + 1.) * body[1].dMeanMotion);
  tmp1 += tmp2;

  double c = body[1].dEcc * tmp1 / body[iBody].dR0;
  c /= body[iBody].dLL13K0 * body[iBody].dLL13K0 -
       (k * body[iBody].dLL13N0 - (k + 1.) * body[1].dMeanMotion) *
             (k * body[iBody].dLL13N0 - (k + 1.) * body[1].dMeanMotion);
  return c;
}

/** LL13 C^-_k as defined by eqn 30b (the - term) */
double fndCMk(int k, BODY *body, int iBody) {

  double n    = fndn(body[iBody].dR0, body);
  double tmp1 = -k * fndPot0dR(0, k, body[iBody].dR0, body) -
                0.5 * fndPot1dR(0, k, body[iBody].dR0, body);
  double tmp2 = k * n *
                (-2. * k * fndPot0(0, k, body[iBody].dR0, body) -
                 fndPot1(0, k, body[iBody].dR0, body));
  tmp2 /= body[iBody].dR0 * (k * n - (k - 1.) * body[1].dMeanMotion);
  tmp1 += tmp2;

  double c = body[1].dEcc * tmp1 / body[iBody].dR0;
  c /= body[iBody].dLL13K0 * body[iBody].dLL13K0 -
       (k * body[iBody].dLL13N0 - (k - 1.) * body[1].dMeanMotion) *
             (k * body[iBody].dLL13N0 - (k - 1.) * body[1].dMeanMotion);
  return c;
}

/** LL13 D0 as defined by eqn 32 */
double fndD0(BODY *body, int iBody) {
  return 2.0 * fndC0(body, iBody);
}

/** LL13 Dk0 as defined by eqn 33 */
double fndDk0(int k, BODY *body, int iBody) {
  double n    = fndn(body[iBody].dR0, body);
  double tmp1 = 2.0 * fndC0k(k, body, iBody);
  double Dk   = -fndPot0(0, k, body[iBody].dR0, body) /
              (body[iBody].dR0 * body[iBody].dR0);
  Dk /= (n * (n - body[1].dMeanMotion));

  return (tmp1 - Dk);
}

/** LL13 D+_k as defined by eqn 34a (the + term) */
double fndDPk(int k, BODY *body, int iBody) {
  double n  = fndn(body[iBody].dR0, body);
  double Dk = 2.0 * fndCPk(k, body, iBody);

  double tmp1 =
        body[1].dEcc * (k * (2. * k * fndPot0(0, k, body[iBody].dR0, body) -
                             fndPot1(0, k, body[iBody].dR0, body)));
  tmp1 /= 2.0 * body[iBody].dR0 * body[iBody].dR0 * n *
          (k * n - (k + 1.) * body[1].dMeanMotion);

  return Dk - tmp1;
}

/** LL13 D-_k as defined by eqn 34b (the - term) */
double fndDMk(int k, BODY *body, int iBody) {
  double n  = fndn(body[iBody].dR0, body);
  double Dk = 2.0 * fndCMk(k, body, iBody);

  double tmp1 =
        body[1].dEcc * (k * (-2. * k * fndPot0(0, k, body[iBody].dR0, body) -
                             fndPot1(0, k, body[iBody].dR0, body)));
  tmp1 /= 2.0 * body[iBody].dR0 * body[iBody].dR0 * n *
          (k * n - (k - 1.) * body[1].dMeanMotion);

  return Dk - tmp1;
}

/*
 * LL13 Functions to compute cylindrical positions, velocities
 * R, phi, z and RDot, phiDot, zDot.  Note: iaBody ALWAYS has one element:
 * the CBP whose equations are being integrated/evaluated since in binary, each
 * CBP is assured to have bodies 0, 1 be the stars.
 */

/** Computes the CBP orbital radius */
double fndCBPRBinary(BODY *body, SYSTEM *system, int *iaBody) {

  int iBody = iaBody[0];
  int k;

  double dPsi  = body[iBody].dCBPPsi;
  double dTime = body[iBody].dAge; // Time == Age of the body

  // Useful intermediate quantities
  double M =
        fndBinaryMeanAnomaly(body[1].dMeanMotion, dTime, body[1].dLL13PhiAB);
  double phi0  = fndPhi0(dTime, body[iBody].dLL13N0, body[iBody].dCBPM0);
  double varpi = body[1].dLongP;

  double tmp1 = 1. -
                body[iBody].dFreeEcc * cos(body[iBody].dLL13K0 * dTime + dPsi) -
                fndC0(body, iBody) * cos(M);
  double tmp2 = 0.0;
  double tmp3 = 0.0;

  for (k = 1; k < K_MAX; k++) {
    tmp3 = fndC0k(k, body, iBody) * cos(k * (phi0 - M - varpi));
    tmp3 += fndCPk(k, body, iBody) * cos(k * (phi0 - varpi) - (k + 1.) * M);
    tmp3 += fndCMk(k, body, iBody) * cos(k * (phi0 - varpi) - (k - 1.) * M);
    tmp2 += tmp3;
  }

  return body[iBody].dR0 * (tmp1 - tmp2);
}

double fndCBPPhiBinary(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0], k;

  double dPsi  = body[iBody].dCBPPsi;
  double dTime = body[iBody].dAge; // Time == Age of the body

  // Useful intermediate quantities
  double M =
        fndBinaryMeanAnomaly(body[1].dMeanMotion, dTime, body[1].dLL13PhiAB);
  double phi0  = fndPhi0(dTime, body[iBody].dLL13N0, body[iBody].dCBPM0);
  double varpi = body[1].dLongP;

  double phi = phi0 + 2.0 * body[iBody].dLL13N0 * body[iBody].dFreeEcc *
                            sin(body[iBody].dLL13K0 * dTime + dPsi) /
                            body[iBody].dLL13K0;
  phi +=
        body[iBody].dLL13N0 * fndD0(body, iBody) * sin(M) / body[1].dMeanMotion;

  double tot  = 0.0;
  double tmp1 = 0.0;

  for (k = 1; k < K_MAX; k++) {
    tmp1 = body[iBody].dLL13N0 * fndDk0(k, body, iBody) *
           sin(k * (phi0 - M - varpi)) /
           (k * (body[iBody].dLL13N0 - body[1].dMeanMotion));
    tmp1 += body[iBody].dLL13N0 * fndDPk(k, body, iBody) *
            sin(k * (phi0 - varpi) - (k + 1.) * M) /
            (k * body[iBody].dLL13N0 - (k + 1.) * body[1].dMeanMotion);
    tmp1 += body[iBody].dLL13N0 * fndDMk(k, body, iBody) *
            sin(k * (phi0 - varpi) - (k - 1.) * M) /
            (k * body[iBody].dLL13N0 - (k - 1.) * body[1].dMeanMotion);
    tot += tmp1;
  }

  phi = phi + tot;

  return fmod(phi, 2.0 * PI);
}

double fndCBPZBinary(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0];

  double dZeta = body[iBody].dCBPZeta;
  double dTime = body[iBody].dAge; // Time == Age of the body

  return body[iBody].dR0 * body[iBody].dFreeInc *
         cos(body[iBody].dLL13V0 * dTime + dZeta);
}

double fndCBPRDotBinary(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0], k;

  double dPsi  = body[iBody].dCBPPsi;
  double dTime = body[iBody].dAge; // Time == Age of the body

  // Useful intermediate quantities
  double k0       = body[iBody].dLL13K0;
  double phi0     = fndPhi0(dTime, body[iBody].dLL13N0, body[iBody].dCBPM0);
  double phi0_dot = body[iBody].dLL13N0;
  double M_dot    = body[1].dMeanMotion;
  double M =
        fndBinaryMeanAnomaly(body[1].dMeanMotion, dTime, body[1].dLL13PhiAB);
  double varpi = body[1].dLongP;

  double tmp1 = k0 * body[iBody].dFreeEcc * sin(k0 * dTime + dPsi) +
                fndC0(body, iBody) * sin(M) * M_dot;

  double tmp2 = 0.0; // Total sum
  double tmp3 = 0.0; // Intermediate sum for each k

  for (k = 1; k < K_MAX; k++) {
    tmp3 = -fndC0k(k, body, iBody) * sin(k * (phi0 - M - varpi)) * k *
           (phi0_dot - M_dot);
    tmp3 -= fndCPk(k, body, iBody) * sin(k * (phi0 - varpi) - (k + 1.) * M) *
            (k * phi0_dot - (k + 1.) * M_dot);
    tmp3 -= fndCMk(k, body, iBody) * sin(k * (phi0 - varpi) - (k - 1.) * M) *
            (k * phi0_dot - (k - 1.) * M_dot);
    tmp2 += tmp3;
  }

  return body[iBody].dR0 * (tmp1 - tmp2);
}

double fndCBPPhiDotBinary(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0], k;

  // Set arbitrary phase constants to 0
  double dPsi  = body[iBody].dCBPPsi;
  double dTime = body[iBody].dAge; // Time == Age of the body

  // Useful intermediate quantities
  double k0       = body[iBody].dLL13K0;
  double phi0     = fndPhi0(dTime, body[iBody].dLL13N0, body[iBody].dCBPM0);
  double phi0_dot = body[iBody].dLL13N0;
  double n0       = body[iBody].dLL13N0;
  double n        = body[1].dMeanMotion;
  double M_dot    = n;
  double M =
        fndBinaryMeanAnomaly(body[1].dMeanMotion, dTime, body[1].dLL13PhiAB);
  double varpi = body[1].dLongP;

  double tmp1 = n0 + 2.0 * n0 * body[iBody].dFreeEcc * cos(k0 * dTime + dPsi) +
                (n0 / n) * fndD0(body, iBody) * cos(M) * M_dot;

  double tmp2 = 0.0; // Total loop sum
  double tmp3 = 0.0; // Intermediate loop sum

  for (k = 1; k < K_MAX; k++) {
    tmp3 = (n0 / (k * (n0 - n))) * fndDk0(k, body, iBody) *
           cos(k * (phi0 - M - varpi)) * k * (phi0_dot - M_dot);
    tmp3 += (n0 * fndDPk(k, body, iBody) / (k * n0 - (k + 1.) * n)) *
            cos(k * (phi0 - varpi) - (k + 1.) * M) *
            (k * phi0_dot - (k + 1.) * M_dot);
    tmp3 += (n0 * fndDMk(k, body, iBody) / (k * n0 - (k - 1.) * n)) *
            cos(k * (phi0 - varpi) - (k - 1.) * M) *
            (k * phi0_dot - (k - 1.) * M_dot);
    tmp2 += tmp3;
  }

  return (tmp1 + tmp2);
}

double fndCBPZDotBinary(BODY *body, SYSTEM *system, int *iaBody) {
  int iBody = iaBody[0];

  // Set arbitrary phase constants to 0 for simplicity
  double dZeta = body[iBody].dCBPZeta;
  double dTime = body[iBody].dAge; // Time == Age of the body

  return -body[iBody].dR0 * body[iBody].dFreeInc *
         sin(body[iBody].dLL13V0 * dTime + dZeta) * body[iBody].dLL13V0;
}

/* Misc Functions */

/** Compute the approximate flux in the limit P_bin << P_cbp
    received by the CBP from the 2 stars averaged over 1 binary orbit
    Assumes binary orb elements don't vary much per orbit (safe assumption) */
double fndFluxApproxBinary(BODY *body, int iBody) {

  // Compute CBP position in cylindrical coordinates
  double r   = sqrt(body[iBody].dCBPR * body[iBody].dCBPR +
                    body[iBody].dCBPZ * body[iBody].dCBPZ);
  double psi = body[iBody].dCBPPhi;

  // Intermediate quantities
  double mu1, mu2, tmp;
  mu1 = body[0].dMass / (body[0].dMass + body[1].dMass);
  mu2 = body[1].dMass / (body[0].dMass + body[1].dMass);

  double flux;

  // Monopole term
  flux = (body[0].dLuminosity + body[1].dLuminosity) / (4.0 * PI * r * r);

  // Dipole term
  tmp = (mu1 * body[1].dLuminosity - mu2 * body[0].dLuminosity) /
        (8.0 * PI * r * r * r);
  tmp *= 3.0 * body[1].dSemi * body[1].dEcc * cos(psi);

  return (flux + tmp);
}

/** Compute the exact flux (as close to exact as you want)
    received by the CBP from the 2 stars averaged over 1 CBP orbit
    Assumes binary orb elements don't vary much over 1 CBP orbit
    Also assumes that 1 CBP orbit is approximately Keplerian */
double fndFluxExactBinary(BODY *body, int iBody, double L0, double L1) {

  // Define/init all variables
  int i;
  int iaBody[]   = {iBody};
  SYSTEM *system = NULL;                               // dummy
  double period  = 2.0 * PI / body[iBody].dMeanMotion; // Period of CBP orbit
  double flux    = 0.0;
  double step    = period / FLUX_INT_MAX;
  double trueAnomaly = 0.0;
  double meanAnomaly = 0.0;
  double eccAnomaly  = 0.0;
  double radius      = 0.0;
  double r1 = 0.0, r2 = 0.0;
  double x1 = 0.0, x2 = 0.0, y1 = 0.0,
         y2 = 0.0;                    // Cartesian position of binary in plane
  double x = 0.0, y = 0.0;            // Cartesian positon of cbp
  double r = 0.0, phi = 0.0, z = 0.0; // Cyl position of cbp
  double dAge = body[iBody].dAge; // Save body[iaBody[0]].dAge so this function
                                  // doesn't actually change it

  // Loop over steps in CBP orbit, add flux due to each star at each step
  for (i = 0; i < FLUX_INT_MAX; i++) {
    // Get binary position by solving kepler's eqn
    // mean -> ecc -> true anomaly
    meanAnomaly = body[1].dMeanMotion * body[iBody].dAge + body[1].dLL13PhiAB;
    eccAnomaly  = fndMeanToEccentric(meanAnomaly,
                                     body[1].dEcc); // Solve Kepler's equation
    trueAnomaly = fndEccToTrue(eccAnomaly, body[1].dEcc);

    radius = body[1].dSemi * (1.0 - body[1].dEcc * body[1].dEcc);
    radius /= (1.0 + body[1].dEcc * cos(trueAnomaly - body[1].dLongP));

    // Radial position of each star (- accounts for 180 deg phase offset)
    double dInvMass = 1.0 / (body[0].dMass + body[1].dMass);
    r1              = body[1].dMass * radius * dInvMass;
    r2              = -body[0].dMass * radius * dInvMass;

    // Cartesian position of stars (in plane, no z)
    x1 = r1 * cos(trueAnomaly);
    x2 = r2 * cos(trueAnomaly);
    y1 = r1 * sin(trueAnomaly);
    y2 = r2 * sin(trueAnomaly);

    // Compute CBP positon in cylindrical coords
    phi = (360.0 * (i + 1.0) / FLUX_INT_MAX) / PI; // True anomaly
    r   = body[iBody].dSemi * (1.0 - body[iBody].dEcc * body[iBody].dEcc);
    r /= (1.0 + body[iBody].dEcc * cos(phi - body[iBody].dLongP));
    // r = fndCBPRBinary(body,system,iaBody);
    // phi = fndCBPPhiBinary(body,system,iaBody);
    z = fndCBPZBinary(body, system, iaBody);

    // Convert to cartesian
    x = r * cos(phi);
    y = r * sin(phi);
    // z = z

    // Compute squared distances from each star to CBP
    r1 = (x - x1) * (x - x1) + (y - y1) * (y - y1) + z * z;
    r2 = (x - x2) * (x - x2) + (y - y2) * (y - y2) + z * z;

    // Compute fluxes due to each star
    flux += (L0 / (4.0 * PI * r1) + L1 / (4.0 * PI * r2));

    // Increment body's age aka time
    // Need to do it this way for CBP cyl pos calculations
    body[iBody].dAge += step;
  }

  // Reset the body's dAge to what it was at the start of this function!
  body[iBody].dAge = dAge;

  return flux / FLUX_INT_MAX;
}

/** Compute the approximate equlibrirum temperature for a circumbinary
    planet averaged over the binary orbit */
double fndApproxEqTemp(BODY *body, int iBody, double dAlbedo) {
  // Compute the approximate flux
  double flux = fndFluxApproxBinary(body, iBody);

  return pow(flux * (1.0 - dAlbedo) / (4.0 * SIGMA), 1. / 4.);
}


/** Compute the approximate insolation for a circumbinary
    planet averaged over the binary orbit relative to Earth's
    where F_Earth = 1361 W/m^2 */
double fndApproxInsol(BODY *body, int iBody) {
  // Compute the approximate flux if the stars have luminosities
  if (body[0].dLuminosity > 0 && body[1].dLuminosity > 0) {
    double flux = fndFluxApproxBinary(body, iBody);

    return flux / FLUX_EARTH;
  } else {
    return -1;
  }
}

/** Dumps out a bunch of values to see if they agree with LL13 */
void fnvBinaryDebug(BODY *body) {

  fprintf(stderr, "binary debug information:\n");
  fprintf(stderr, "r0: %lf.\n", body[2].dR0 / AUM);
  fprintf(stderr, "nk: %lf.\n", body[2].dMeanMotion * YEARSEC);
  fprintf(stderr, "n0: %lf.\n", body[2].dLL13N0 * YEARSEC);
  fprintf(stderr, "nAB: %lf.\n", body[1].dMeanMotion * YEARSEC);
  fprintf(stderr, "k0: %lf.\n", body[2].dLL13K0 * YEARSEC);
  fprintf(stderr, "v0: %lf.\n", body[2].dLL13V0 * YEARSEC);

  fprintf(stderr, "n0/nk: %lf.\n", body[2].dLL13N0 / body[2].dMeanMotion);
  fprintf(stderr, "k0/nk: %lf.\n", body[2].dLL13K0 / body[2].dMeanMotion);
  fprintf(stderr, "v0/nk: %lf.\n", body[2].dLL13V0 / body[2].dMeanMotion);

  fprintf(stderr, "C0: %lf.\n", fndC0(body, 2));
  fprintf(stderr, "C01: %lf.\n", fndC0k(1, body, 2));
  fprintf(stderr, "C02: %lf.\n", fndC0k(2, body, 2));
  fprintf(stderr, "C03: %lf.\n", fndC0k(3, body, 2));

  fprintf(stderr, "C1+: %lf.\n", fndCPk(1, body, 2));
  fprintf(stderr, "C2+: %lf.\n", fndCPk(2, body, 2));
  fprintf(stderr, "C3+: %lf.\n", fndCPk(3, body, 2));

  fprintf(stderr, "C1-: %lf.\n", fndCMk(1, body, 2));
  fprintf(stderr, "C2-: %lf.\n", fndCMk(2, body, 2));
  fprintf(stderr, "C3-: %lf.\n", fndCMk(3, body, 2));

  fprintf(stderr, "dCBPM0: %lf.\n", body[2].dCBPM0);
  fprintf(stderr, "dLL13PhiAB: %lf.\n", body[1].dLL13PhiAB);
}
