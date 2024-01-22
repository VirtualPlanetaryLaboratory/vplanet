/**
 @file radheat.c

 @brief Subroutines that control the integration of the
        radiogenic heating model. Also includes subroutines
        that switch between the two models.

 @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

 @date Jun 28 2014

*/

#include "vplanet.h"

void fvInitializeControlRadheat(CONTROL *control, int iBody) {
  if (iBody == 0) {
    // Empty?
  }
}

/**
   Copy body elements to dest

   @param dest The destination body
   @param src The body source
   @param iNumBodies Number of bodies
   @param iBody Index of body to copy
*/
void fvBodyCopyRadheat(BODY *dest, BODY *src, int foo, int iNumBodies,
                       int iBody) {
  dest[iBody].d26AlNumMan    = src[iBody].d26AlNumMan;
  dest[iBody].d26AlConstMan  = src[iBody].d26AlConstMan;
  dest[iBody].d26AlNumCore   = src[iBody].d26AlNumCore;
  dest[iBody].d26AlConstCore = src[iBody].d26AlConstCore;

  dest[iBody].d40KNumMan     = src[iBody].d40KNumMan;
  dest[iBody].d40KConstMan   = src[iBody].d40KConstMan;
  dest[iBody].d40KNumCore    = src[iBody].d40KNumCore;
  dest[iBody].d40KConstCore  = src[iBody].d40KConstCore;
  dest[iBody].d40KNumCrust   = src[iBody].d40KNumCrust;
  dest[iBody].d40KConstCrust = src[iBody].d40KConstCrust;

  dest[iBody].d232ThNumMan     = src[iBody].d232ThNumMan;
  dest[iBody].d232ThConstMan   = src[iBody].d232ThConstMan;
  dest[iBody].d232ThNumCore    = src[iBody].d232ThNumCore;
  dest[iBody].d232ThConstCore  = src[iBody].d232ThConstCore;
  dest[iBody].d232ThNumCrust   = src[iBody].d232ThNumCrust;
  dest[iBody].d232ThConstCrust = src[iBody].d232ThConstCrust;

  dest[iBody].d238UNumMan     = src[iBody].d238UNumMan;
  dest[iBody].d238UConstMan   = src[iBody].d238UConstMan;
  dest[iBody].d238UNumCore    = src[iBody].d238UNumCore;
  dest[iBody].d238UConstCore  = src[iBody].d238UConstCore;
  dest[iBody].d238UNumCrust   = src[iBody].d238UNumCrust;
  dest[iBody].d238UConstCrust = src[iBody].d238UConstCrust;

  dest[iBody].d235UNumMan     = src[iBody].d235UNumMan;
  dest[iBody].d235UConstMan   = src[iBody].d235UConstMan;
  dest[iBody].d235UNumCore    = src[iBody].d235UNumCore;
  dest[iBody].d235UConstCore  = src[iBody].d235UConstCore;
  dest[iBody].d235UNumCrust   = src[iBody].d235UNumCrust;
  dest[iBody].d235UConstCrust = src[iBody].d235UConstCrust;

  dest[iBody].dRadPowerTotal = src[iBody].dRadPowerTotal;
  dest[iBody].dRadPowerMan   = src[iBody].dRadPowerMan;
  dest[iBody].dRadPowerCore  = src[iBody].dRadPowerCore;
  dest[iBody].dRadPowerCrust = src[iBody].dRadPowerCrust;
}

void fvInitializeBodyRadheat(BODY *body, CONTROL *control, UPDATE *update,
                             int iBody, int iModule) {
  // empty?
}

void fvInitializeUpdateTmpBodyRadheat(BODY *body, CONTROL *control,
                                      UPDATE *update, int iBody) {
  // Empty?
}

/**************** RADHEAT options ********************/

/* Aluminum-26 */
/**
   Read initial 26Al power in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead26AlPowerMan(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 26AlMass and 26AlNum */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].d26AlPowerMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      //      body[iFile-1].d26AlPowerMan =
      //      dTmp*fdUnitsMass(control->Units[iFile].iMass);
      // CHANGED units Mass to Power.
      body[iFile - 1].d26AlPowerMan =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].d26AlPowerMan = options->dDefault;
  }
}
/**
   Read initial 26Al power in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead26AlPowerCore(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].d26AlPowerCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d26AlPowerCore =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].d26AlPowerCore = options->dDefault;
  }
}
/**
   Read initial 26Al mass in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead26AlMassMan(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 26AlPower and 26AlNum */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d26AlMassMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d26AlMassMan =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d26AlMassMan = options->dDefault;
  }
}
/**
   Read initial 26Al mass in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead26AlMassCore(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d26AlMassCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d26AlMassCore =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d26AlMassCore = options->dDefault;
  }
}
/**
   Read initial 26Al number in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead26AlNumMan(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 26AlPower and 26AlNum */
  int lTmp = -1;
  double dTmp;

  //  fflush(stdout);
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d26AlNumMan =
            dTmp *
            dNegativeDouble(*options, files->Infile[iFile].cIn,
                            control->Io.iVerbose); // dTmp=input value,
                                                   // dNegativeDouble=-dNeg
                                                   // (default Value).
    } else {
      body[iFile - 1].d26AlNumMan = dTmp; // units of num are num!
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d26AlNumMan = options->dDefault;
  }
}
/**
   Read initial 26Al number in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead26AlNumCore(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;

  //  fflush(stdout);
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d26AlNumCore =
            dTmp *
            dNegativeDouble(*options, files->Infile[iFile].cIn,
                            control->Io.iVerbose); // dTmp=input value,
                                                   // dNegativeDouble=-dNeg
                                                   // (default Value).
    } else {
      body[iFile - 1].d26AlNumCore = dTmp; // units of num are num!
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d26AlNumCore = options->dDefault;
  }
}

/* Potassium */
/**
   Read initial 40K power in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead40KPowerMan(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 40KMass and 40KNum */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].d40KPowerMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      //      body[iFile-1].d40KPowerMan =
      //      dTmp*fdUnitsMass(control->Units[iFile].iMass);
      // CHANGED units Mass to Power.
      body[iFile - 1].d40KPowerMan =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].d40KPowerMan = options->dDefault;
  }
}
/**
   Read initial 40K power in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead40KPowerCore(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].d40KPowerCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d40KPowerCore =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].d40KPowerCore = options->dDefault;
  }
}
/**
   Read initial 40K mass in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead40KMassMan(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 40KPower and 40KNum */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d40KMassMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d40KMassMan =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d40KMassMan = options->dDefault;
  }
}
/**
   Read initial 40K mass in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead40KMassCore(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d40KMassCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d40KMassCore =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d40KMassCore = options->dDefault;
  }
}
/**
   Read initial 40K number in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead40KNumMan(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 40KPower and 40KNum */
  int lTmp = -1;
  double dTmp;

  //  fflush(stdout);
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d40KNumMan =
            dTmp *
            dNegativeDouble(*options, files->Infile[iFile].cIn,
                            control->Io.iVerbose); // dTmp=input value,
                                                   // dNegativeDouble=-dNeg
                                                   // (default Value).
    } else {
      body[iFile - 1].d40KNumMan = dTmp; // units of num are num!
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d40KNumMan = options->dDefault;
  }
}
/**
   Read initial 40K number in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead40KNumCore(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d40KNumCore =
            dTmp *
            dNegativeDouble(*options, files->Infile[iFile].cIn,
                            control->Io.iVerbose); // dTmp=input value,
                                                   // dNegativeDouble=-dNeg
                                                   // (default Value).
    } else {
      body[iFile - 1].d40KNumCore = dTmp; // units of num are num!
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d40KNumCore = options->dDefault;
  }
}
/**
   Read initial 40K power in crust

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead40KPowerCrust(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) { // if line num of option ge 0
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) { // if input value lt 0
      body[iFile - 1].d40KPowerCrust =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d40KPowerCrust =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile >
             0) { // if line num not ge 0, then if iFile gt 0, then set default.
    body[iFile - 1].d40KPowerCrust = options->dDefault;
  }
}
/**
   Read initial 40K mass in crust

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead40KMassCrust(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d40KMassCrust =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d40KMassCrust =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d40KMassCrust = options->dDefault;
  }
}
/**
   Read initial 40K number in crust

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead40KNumCrust(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d40KNumCrust =
            dTmp *
            dNegativeDouble(*options, files->Infile[iFile].cIn,
                            control->Io.iVerbose); // dTmp=input value,
                                                   // dNegativeDouble=-dNeg
                                                   // (default Value).
    } else {
      body[iFile - 1].d40KNumCrust = dTmp; // units of num are num!
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d40KNumCrust = options->dDefault;
  }
}

/* Thorium */
/**
   Read initial 232Th power in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead232ThPowerMan(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThMass and 232ThNum */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d232ThPowerMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      //      body[iFile-1].d232ThPower =
      //      dTmp*fdUnitsMass(control->Units[iFile].iMass);
      body[iFile - 1].d232ThPowerMan =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d232ThPowerMan = options->dDefault;
  }
}
/**
   Read initial 232Th power in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead232ThPowerCore(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThMass and 232ThNum */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d232ThPowerCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      //      body[iFile-1].d232ThPower =
      //      dTmp*fdUnitsMass(control->Units[iFile].iMass);
      body[iFile - 1].d232ThPowerCore =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d232ThPowerCore = options->dDefault;
  }
}
/**
   Read initial 232Th mass in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead232ThMassMan(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d232ThMassMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d232ThMassMan =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d232ThMassMan = options->dDefault;
  }
}
/**
   Read initial 232Th mass in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead232ThMassCore(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d232ThMassCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d232ThMassCore =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d232ThMassCore = options->dDefault;
  }
}
/**
   Read initial 232Th number in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead232ThNumMan(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d232ThNumMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d232ThNumMan = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d232ThNumMan = options->dDefault;
  }
}
/**
   Read initial 232Th number in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead232ThNumCore(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d232ThNumCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d232ThNumCore = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d232ThNumCore = options->dDefault;
  }
}
/**
   Read initial 232Th power in crust

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead232ThPowerCrust(BODY *body, CONTROL *control, FILES *files,
                           OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThMass and 232ThNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d232ThPowerCrust =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      //      body[iFile-1].d232ThPower =
      //      dTmp*fdUnitsMass(control->Units[iFile].iMass);
      body[iFile - 1].d232ThPowerCrust =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d232ThPowerCrust = options->dDefault;
  }
}
/**
   Read initial 232Th mass in crust

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead232ThMassCrust(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d232ThMassCrust =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d232ThMassCrust =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d232ThMassCrust = options->dDefault;
  }
}
/**
   Read initial 232Th number in crust

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead232ThNumCrust(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d232ThNumCrust =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d232ThNumCrust = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d232ThNumCrust = options->dDefault;
  }
}

/* Uranium 238 */
/**
   Read initial 238U power in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead238UPowerMan(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UMass and 232UNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d238UPowerMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      //      body[iFile-1].d238UPower =
      //      dTmp*fdUnitsMass(control->Units[iFile].iMass);
      body[iFile - 1].d238UPowerMan =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d238UPowerMan = options->dDefault;
  }
}
/**
   Read initial 238U mass in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead238UMassMan(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d238UMassMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d238UMassMan =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d238UMassMan = options->dDefault;
  }
}
/**
   Read initial 238U number in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead238UNumMan(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d238UNumMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d238UNumMan = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d238UNumMan = options->dDefault;
  }
}

/* Core */
/**
   Read initial 238U power in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead238UPowerCore(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UMass and 232UNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d238UPowerCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      //      body[iFile-1].d238UPower =
      //      dTmp*fdUnitsMass(control->Units[iFile].iMass);
      body[iFile - 1].d238UPowerCore =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d238UPowerCore = options->dDefault;
  }
}
/**
   Read initial 238U mass in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead238UMassCore(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d238UMassCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d238UMassCore =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d238UMassCore = options->dDefault;
  }
}
/**
   Read initial 238U number in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead238UNumCore(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d238UNumCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d238UNumCore = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d238UNumCore = options->dDefault;
  }
}

/* Crust */
/**
   Read initial 238U power in crust

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead238UPowerCrust(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UMass and 232UNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d238UPowerCrust =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d238UPowerCrust =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d238UPowerCrust = options->dDefault;
  }
}
/**
   Read initial 238U mass in crust

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead238UMassCrust(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d238UMassCrust =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d238UMassCrust =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d238UMassCrust = options->dDefault;
  }
}
/**
   Read initial 238U number in crust

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead238UNumCrust(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d238UNumCrust =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d238UNumCrust = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d238UNumCrust = options->dDefault;
  }
}

/* Uranium 235 */
/**
   Read initial 235U power in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead235UPowerMan(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UMass and 232UNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d235UPowerMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      //      body[iFile-1].d235UPower =
      //      dTmp*fdUnitsMass(control->Units[iFile].iMass);
      body[iFile - 1].d235UPowerMan =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d235UPowerMan = options->dDefault;
  }
}
/**
   Read initial 235U mass in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead235UMassMan(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d235UMassMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d235UMassMan =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d235UMassMan = options->dDefault;
  }
}
/**
   Read initial 235U number in mantle

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead235UNumMan(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d235UNumMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d235UNumMan = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d235UNumMan = options->dDefault;
  }
}
/**
   Read initial 235U power in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead235UPowerCore(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UMass and 232UNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d235UPowerCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      //      body[iFile-1].d235UPower =
      //      dTmp*fdUnitsMass(control->Units[iFile].iMass);
      body[iFile - 1].d235UPowerCore =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d235UPowerCore = options->dDefault;
  }
}
/**
   Read initial 235U mass in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead235UMassCore(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d235UMassCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d235UMassCore =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d235UMassCore = options->dDefault;
  }
}
/**
   Read initial 235U number in core

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead235UNumCore(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d235UNumCore =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d235UNumCore = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d235UNumCore = options->dDefault;
  }
}
/**
   Read initial 235U power in crust

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead235UPowerCrust(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UMass and 232UNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d235UPowerCrust =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      //      body[iFile-1].d235UPower =
      //      dTmp*fdUnitsMass(control->Units[iFile].iMass);
      body[iFile - 1].d235UPowerCrust =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d235UPowerCrust = options->dDefault;
  }
}
/**
   Read initial 235U mass in crust

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead235UMassCrust(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UNum */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d235UMassCrust =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d235UMassCrust =
            dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d235UMassCrust = options->dDefault;
  }
}
/**
   Read initial 235U number in crust

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvRead235UNumCrust(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].d235UNumCrust =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].d235UNumCrust = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].d235UNumCrust = options->dDefault;
  }
}

/* Halts */
/**
   Read halt 40K power

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvReadHalt40KPower(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      control->Halt[iFile - 1].dMin40KPower =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      control->Halt[iFile - 1].dMin40KPower =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }

    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    control->Halt[iFile - 1].dMin40KPower = options->dDefault;
  }
}
/**
   Read halt 235U power

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvReadHalt235UPower(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      control->Halt[iFile - 1].dMin235UPower =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      control->Halt[iFile - 1].dMin235UPower =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }

    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    control->Halt[iFile - 1].dMin235UPower = options->dDefault;
  }
}
/**
   Read halt 238U power

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvReadHalt238UPower(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      control->Halt[iFile - 1].dMin238UPower =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      control->Halt[iFile - 1].dMin238UPower =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }

    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    control->Halt[iFile - 1].dMin238UPower = options->dDefault;
  }
}
/**
   Read halt 232Th power

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvReadHalt232ThPower(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      control->Halt[iFile - 1].dMin232ThPower =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      control->Halt[iFile - 1].dMin232ThPower =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }

    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    control->Halt[iFile - 1].dMin232ThPower = options->dDefault;
  }
}
/**
   Read halt total radiogenic power

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param iFile Index of file
*/
void fvReadHaltRadPower(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp = -1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      control->Halt[iFile - 1].dMinRadPower =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      control->Halt[iFile - 1].dMinRadPower =
            dTmp * fdUnitsPower(control->Units[iFile].iTime,
                                control->Units[iFile].iMass,
                                control->Units[iFile].iLength);
    }

    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    control->Halt[iFile - 1].dMinRadPower = options->dDefault;
  }
}

/* Initiatlize Input Options */
/**
   Initialize input options to default values

   @param options Options struct
   @param Function to read in options
*/
void fvInitializeOptionsRadheat(OPTIONS *options, fnReadOption fnRead[]) {

  /* 26Al */
  sprintf(options[OPT_26ALMASSMAN].cName, "d26AlMassMan");
  sprintf(options[OPT_26ALMASSMAN].cDescr, "Initial Mantle Mass of 26Al");
  sprintf(options[OPT_26ALMASSMAN].cDefault, "0");
  sprintf(options[OPT_26ALMASSMAN].cDimension, "mass");
  options[OPT_26ALMASSMAN].iType      = 2;
  options[OPT_26ALMASSMAN].bMultiFile = 1;
  options[OPT_26ALMASSMAN].dNeg       = MEARTH;
  options[OPT_26ALMASSMAN].dDefault   = 0;
  sprintf(options[OPT_26ALMASSMAN].cNeg, "Mearth");
  fnRead[OPT_26ALMASSMAN] = &fvRead26AlMassMan;

  sprintf(options[OPT_26ALMASSCORE].cName, "d26AlMassCore");
  sprintf(options[OPT_26ALMASSCORE].cDescr, "Initial Core Mass of 26Al");
  sprintf(options[OPT_26ALMASSCORE].cDefault, "0");
  sprintf(options[OPT_26ALMASSCORE].cDimension, "mass");
  options[OPT_26ALMASSCORE].iType      = 2;
  options[OPT_26ALMASSCORE].bMultiFile = 1;
  options[OPT_26ALMASSCORE].dNeg       = MEARTH;
  options[OPT_26ALMASSCORE].dDefault   = 0;
  sprintf(options[OPT_26ALMASSCORE].cNeg, "Mearth");
  fnRead[OPT_26ALMASSCORE] = &fvRead26AlMassCore;

  sprintf(options[OPT_26ALNUMMAN].cName, "d26AlNumMan");
  sprintf(options[OPT_26ALNUMMAN].cDescr,
          "Initial Mantle Number of 26Al Atoms");
  sprintf(options[OPT_26ALNUMMAN].cDefault, "0");
  sprintf(options[OPT_26ALNUMMAN].cDimension, "mass");
  options[OPT_26ALNUMMAN].iType      = 2;
  options[OPT_26ALNUMMAN].bMultiFile = 1;
  options[OPT_26ALNUMMAN].dNeg       = ENUMMAN26AL;
  options[OPT_26ALNUMMAN].dDefault   = 0;
  sprintf(options[OPT_26ALNUMMAN].cNeg, "Primordial Earth 26Al Number");
  fnRead[OPT_26ALNUMMAN] = &fvRead26AlNumMan;

  sprintf(options[OPT_26ALNUMCORE].cName, "d26AlNumCore");
  sprintf(options[OPT_26ALNUMCORE].cDescr, "Initial Core Number of 26Al Atoms");
  sprintf(options[OPT_26ALNUMCORE].cDefault, "0");
  sprintf(options[OPT_26ALNUMCORE].cDimension, "nd");
  options[OPT_26ALNUMCORE].iType      = 2;
  options[OPT_26ALNUMCORE].bMultiFile = 1;
  options[OPT_26ALNUMCORE].dNeg       = ENUMCORE26AL;
  options[OPT_26ALNUMCORE].dDefault   = 0;
  sprintf(options[OPT_26ALNUMCORE].cNeg, "Primordial Earth 26Al Number");
  fnRead[OPT_26ALNUMCORE] = &fvRead26AlNumCore;

  sprintf(options[OPT_26ALPOWERMAN].cName, "d26AlPowerMan");
  sprintf(options[OPT_26ALPOWERMAN].cDescr,
          "Initial Mantle Power Production from 26Al Atoms");
  sprintf(options[OPT_26ALPOWERMAN].cDefault, "0");
  sprintf(options[OPT_26ALPOWERMAN].cDimension, "energy/time");
  options[OPT_26ALPOWERMAN].iType      = 2;
  options[OPT_26ALPOWERMAN].bMultiFile = 1;
  options[OPT_26ALPOWERMAN].dNeg       = EPOWERMAN26AL; // Earth's POWER of 26Al
  options[OPT_26ALPOWERMAN].dDefault   = 0;
  sprintf(options[OPT_26ALPOWERMAN].cNeg, "TW");
  fnRead[OPT_26ALPOWERMAN] = &fvRead26AlPowerMan;

  sprintf(options[OPT_26ALPOWERCORE].cName, "d26AlPowerCore");
  sprintf(options[OPT_26ALPOWERCORE].cDescr,
          "Initial Core Power Production from 26Al Atoms");
  sprintf(options[OPT_26ALPOWERCORE].cDefault, "0");
  sprintf(options[OPT_26ALPOWERCORE].cDimension, "energy/time");
  options[OPT_26ALPOWERCORE].iType      = 2;
  options[OPT_26ALPOWERCORE].bMultiFile = 1;
  options[OPT_26ALPOWERCORE].dNeg     = EPOWERCORE26AL; // Earth's POWER of 26Al
  options[OPT_26ALPOWERCORE].dDefault = 0;
  sprintf(options[OPT_26ALPOWERCORE].cNeg, "TW");
  fnRead[OPT_26ALPOWERCORE] = &fvRead26AlPowerCore;

  /* 40K */
  sprintf(options[OPT_40KMASSMAN].cName, "d40KMassMan");
  sprintf(options[OPT_40KMASSMAN].cDescr, "Initial Mantle Mass of 40K");
  sprintf(options[OPT_40KMASSMAN].cDefault, "Primordial Earth");
  sprintf(options[OPT_40KMASSMAN].cDimension, "mass");
  options[OPT_40KMASSMAN].iType      = 2;
  options[OPT_40KMASSMAN].bMultiFile = 1;
  options[OPT_40KMASSMAN].dNeg       = EMASSMAN40K;
  options[OPT_40KMASSMAN].dDefault   = EMASSMAN40K;
  sprintf(options[OPT_40KMASSMAN].cNeg, "Primordial Earth Units");
  fnRead[OPT_40KMASSMAN] = &fvRead40KMassMan;

  sprintf(options[OPT_40KMASSCORE].cName, "d40KMassCore");
  sprintf(options[OPT_40KMASSCORE].cDescr, "Initial Core Mass of 40K");
  sprintf(options[OPT_40KMASSCORE].cDefault, "Primordial Earth Units");
  sprintf(options[OPT_40KMASSCORE].cDimension, "mass");
  options[OPT_40KMASSCORE].iType      = 2;
  options[OPT_40KMASSCORE].bMultiFile = 1;
  options[OPT_40KMASSCORE].dNeg       = EMASSCORE40K;
  options[OPT_40KMASSCORE].dDefault   = EMASSCORE40K;
  sprintf(options[OPT_40KMASSCORE].cNeg, "Primordial Earth");
  fnRead[OPT_40KMASSCORE] = &fvRead40KMassCore;

  sprintf(options[OPT_40KMASSCRUST].cName, "d40KMassCrust");
  sprintf(options[OPT_40KMASSCRUST].cDescr, "Initial Crust Mass of 40K");
  sprintf(options[OPT_40KMASSCRUST].cDefault, "Primordial Earth");
  sprintf(options[OPT_40KMASSCRUST].cDimension, "mass");
  options[OPT_40KMASSCRUST].iType      = 2;
  options[OPT_40KMASSCRUST].bMultiFile = 1;
  options[OPT_40KMASSCRUST].dNeg       = EMASSCRUST40K;
  options[OPT_40KMASSCRUST].dDefault   = EMASSCRUST40K;
  sprintf(options[OPT_40KMASSCRUST].cNeg, "Primordial Earth Units");
  fnRead[OPT_40KMASSCRUST] = &fvRead40KMassCrust;

  sprintf(options[OPT_40KNUMMAN].cName, "d40KNumMan");
  sprintf(options[OPT_40KNUMMAN].cDescr, "Initial Mantle Number of 40K Atoms");
  sprintf(options[OPT_40KNUMMAN].cDefault, "Primordial Earth");
  sprintf(options[OPT_40KNUMMAN].cDimension, "nd");
  options[OPT_40KNUMMAN].iType      = 2;
  options[OPT_40KNUMMAN].bMultiFile = 1;
  options[OPT_40KNUMMAN].dNeg       = ENUMMAN40K;
  options[OPT_40KNUMMAN].dDefault   = ENUMMAN40K;
  sprintf(options[OPT_40KNUMMAN].cNeg, "Primordial Earth Units");
  fnRead[OPT_40KNUMMAN] = &fvRead40KNumMan;

  sprintf(options[OPT_40KNUMCORE].cName, "d40KNumCore");
  sprintf(options[OPT_40KNUMCORE].cDescr, "Initial Core Number of 40K Atoms");
  sprintf(options[OPT_40KNUMCORE].cDefault, "Primordial Earth");
  sprintf(options[OPT_40KNUMCORE].cDimension, "nd");
  options[OPT_40KNUMCORE].iType      = 2;
  options[OPT_40KNUMCORE].bMultiFile = 1;
  options[OPT_40KNUMCORE].dNeg       = ENUMCORE40K;
  options[OPT_40KNUMCORE].dDefault   = ENUMCORE40K;
  sprintf(options[OPT_40KNUMCORE].cNeg, "Primordial Earth Units");
  fnRead[OPT_40KNUMCORE] = &fvRead40KNumCore;

  sprintf(options[OPT_40KNUMCRUST].cName, "d40KNumCrust");
  sprintf(options[OPT_40KNUMCRUST].cDescr, "Initial Crust Number of 40K Atoms");
  sprintf(options[OPT_40KNUMCRUST].cDefault, "Primordial Earth");
  sprintf(options[OPT_40KNUMCRUST].cDimension, "nd");
  options[OPT_40KNUMCRUST].iType      = 2;
  options[OPT_40KNUMCRUST].bMultiFile = 1;
  options[OPT_40KNUMCRUST].dNeg       = ENUMCRUST40K;
  options[OPT_40KNUMCRUST].dDefault   = ENUMCRUST40K;
  sprintf(options[OPT_40KNUMCRUST].cNeg, "Primordial Earth Units");
  fnRead[OPT_40KNUMCRUST] = &fvRead40KNumCrust;

  sprintf(options[OPT_40KPOWERMAN].cName, "d40KPowerMan");
  sprintf(options[OPT_40KPOWERMAN].cDescr,
          "Initial Mantle Power Production from 40K Atoms");
  sprintf(options[OPT_40KPOWERMAN].cDefault, "Primordial Earth");
  sprintf(options[OPT_40KPOWERMAN].cDimension, "energy/time");
  options[OPT_40KPOWERMAN].iType      = 2;
  options[OPT_40KPOWERMAN].bMultiFile = 1;
  options[OPT_40KPOWERMAN].dNeg       = EPOWERMAN40K; // Earth's POWER of 40K
  options[OPT_40KPOWERMAN].dDefault   = EPOWERMAN40K;
  sprintf(options[OPT_40KPOWERMAN].cNeg, "Primordial Earth Units");
  fnRead[OPT_40KPOWERMAN] = &fvRead40KPowerMan;

  sprintf(options[OPT_40KPOWERCORE].cName, "d40KPowerCore");
  sprintf(options[OPT_40KPOWERCORE].cDescr,
          "Initial Core Power Production from 40K Atoms");
  sprintf(options[OPT_40KPOWERCORE].cDefault, "Primordial Earth");
  sprintf(options[OPT_40KPOWERCORE].cDimension, "energy/time");
  options[OPT_40KPOWERCORE].iType      = 2;
  options[OPT_40KPOWERCORE].bMultiFile = 1;
  options[OPT_40KPOWERCORE].dNeg       = EPOWERCORE40K; // Earth's POWER of 40K
  options[OPT_40KPOWERCORE].dDefault   = EPOWERCORE40K;
  sprintf(options[OPT_40KPOWERCORE].cNeg, "Primordial Earth Units");
  fnRead[OPT_40KPOWERCORE] = &fvRead40KPowerCore;

  sprintf(options[OPT_40KPOWERCRUST].cName, "d40KPowerCrust");
  sprintf(options[OPT_40KPOWERCRUST].cDescr,
          "Initial Crust Power Production from 40K Atoms");
  sprintf(options[OPT_40KPOWERCRUST].cDefault, "Primordial Earth");
  sprintf(options[OPT_40KPOWERCRUST].cDimension, "energy/time");
  options[OPT_40KPOWERCRUST].iType      = 2;
  options[OPT_40KPOWERCRUST].bMultiFile = 1;
  options[OPT_40KPOWERCRUST].dNeg     = EPOWERCRUST40K; // Earth's POWER of 40K
  options[OPT_40KPOWERCRUST].dDefault = EPOWERCRUST40K;
  sprintf(options[OPT_40KPOWERCRUST].cNeg, "Primordial Earth Units");
  fnRead[OPT_40KPOWERCRUST] = &fvRead40KPowerCrust;

  /* 232Th */
  sprintf(options[OPT_232THMASSMAN].cName, "d232ThMassMan");
  sprintf(options[OPT_232THMASSMAN].cDescr, "Initial Mass of 232Th");
  sprintf(options[OPT_232THMASSMAN].cDefault, "Primordial Earth");
  sprintf(options[OPT_232THMASSMAN].cDimension, "mass");
  options[OPT_232THMASSMAN].iType      = 2;
  options[OPT_232THMASSMAN].bMultiFile = 1;
  options[OPT_232THMASSMAN].dNeg       = EMASSMAN232TH;
  options[OPT_232THMASSMAN].dDefault   = EMASSMAN232TH;
  sprintf(options[OPT_232THMASSMAN].cNeg, "Primordial Earth Units");
  fnRead[OPT_232THMASSMAN] = &fvRead232ThMassMan;

  sprintf(options[OPT_232THNUMMAN].cName, "d232ThNumMan");
  sprintf(options[OPT_232THNUMMAN].cDescr, "Initial Number of 232Th Atoms");
  sprintf(options[OPT_232THNUMMAN].cDefault, "Primordial Earth");
  sprintf(options[OPT_232THNUMMAN].cDimension, "nd");
  options[OPT_232THNUMMAN].iType      = 2;
  options[OPT_232THNUMMAN].bMultiFile = 1;
  options[OPT_232THNUMMAN].dNeg       = ENUMMAN232TH;
  options[OPT_232THNUMMAN].dDefault   = ENUMMAN232TH;
  sprintf(options[OPT_232THNUMMAN].cNeg, "Primordial Earth Units");
  fnRead[OPT_232THNUMMAN] = &fvRead232ThNumMan;

  sprintf(options[OPT_232THPOWERMAN].cName, "d232ThPowerMan");
  sprintf(options[OPT_232THPOWERMAN].cDescr,
          "Initial Power Production from 232Th Atoms");
  sprintf(options[OPT_232THPOWERMAN].cDefault, "Primordial Earth");
  sprintf(options[OPT_232THPOWERMAN].cDimension, "energy/time");
  options[OPT_232THPOWERMAN].iType      = 2;
  options[OPT_232THPOWERMAN].bMultiFile = 1;
  options[OPT_232THPOWERMAN].dNeg       = EPOWERMAN232TH; // 1e12*1e-7; // cgs
  options[OPT_232THPOWERMAN].dDefault   = EPOWERMAN232TH;
  sprintf(options[OPT_232THPOWERMAN].cNeg, "Primordial Earth Units");
  fnRead[OPT_232THPOWERMAN] = &fvRead232ThPowerMan;

  sprintf(options[OPT_232THMASSCORE].cName, "d232ThMassCore");
  sprintf(options[OPT_232THMASSCORE].cDescr, "Initial Core Mass of 232Th");
  sprintf(options[OPT_232THMASSCORE].cDefault, "Primordial Earth");
  sprintf(options[OPT_232THMASSCORE].cDimension, "mass");
  options[OPT_232THMASSCORE].iType      = 2;
  options[OPT_232THMASSCORE].bMultiFile = 1;
  options[OPT_232THMASSCORE].dNeg       = EMASSCORE232TH;
  options[OPT_232THMASSCORE].dDefault   = EMASSCORE232TH;
  sprintf(options[OPT_232THMASSCORE].cNeg, "Primordial Earth Units");
  fnRead[OPT_232THMASSCORE] = &fvRead232ThMassCore;

  sprintf(options[OPT_232THNUMCORE].cName, "d232ThNumCore");
  sprintf(options[OPT_232THNUMCORE].cDescr,
          "Initial Core Number of 232Th Atoms");
  sprintf(options[OPT_232THNUMCORE].cDefault, "Primordial Earth");
  sprintf(options[OPT_232THNUMCORE].cDimension, "nd");
  options[OPT_232THNUMCORE].iType      = 2;
  options[OPT_232THNUMCORE].bMultiFile = 1;
  options[OPT_232THNUMCORE].dNeg       = ENUMCORE232TH;
  options[OPT_232THNUMCORE].dDefault   = ENUMCORE232TH;
  sprintf(options[OPT_232THNUMCORE].cNeg, "Primordial Earth Units");
  fnRead[OPT_232THNUMCORE] = &fvRead232ThNumCore;

  sprintf(options[OPT_232THPOWERCORE].cName, "d232ThPowerCore");
  sprintf(options[OPT_232THPOWERCORE].cDescr,
          "Initial Core Power Production from 232Th Atoms");
  sprintf(options[OPT_232THPOWERCORE].cDefault, "Primordial Earth");
  sprintf(options[OPT_232THPOWERCORE].cDimension, "energy/time");
  options[OPT_232THPOWERCORE].iType      = 2;
  options[OPT_232THPOWERCORE].bMultiFile = 1;
  options[OPT_232THPOWERCORE].dNeg       = EPOWERCORE232TH; // 1e12*1e-7; // cgs
  options[OPT_232THPOWERCORE].dDefault   = EPOWERCORE232TH;
  sprintf(options[OPT_232THPOWERCORE].cNeg, "Primordial Earth Units");
  fnRead[OPT_232THPOWERCORE] = &fvRead232ThPowerCore;

  sprintf(options[OPT_232THMASSCRUST].cName, "d232ThMassCrust");
  sprintf(options[OPT_232THMASSCRUST].cDescr, "Initial Crust Mass of 232Th");
  sprintf(options[OPT_232THMASSCRUST].cDefault, "Primordial Earth");
  sprintf(options[OPT_232THMASSCRUST].cDimension, "mass");
  options[OPT_232THMASSCRUST].iType      = 2;
  options[OPT_232THMASSCRUST].bMultiFile = 1;
  options[OPT_232THMASSCRUST].dNeg       = EMASSCRUST232TH;
  options[OPT_232THMASSCRUST].dDefault   = EMASSCRUST232TH;
  sprintf(options[OPT_232THMASSCRUST].cNeg, "Primordial Earth Units");
  fnRead[OPT_232THMASSCRUST] = &fvRead232ThMassCrust;

  sprintf(options[OPT_232THNUMCRUST].cName, "d232ThNumCrust");
  sprintf(options[OPT_232THNUMCRUST].cDescr,
          "Initial Crust Number of 232Th Atoms");
  sprintf(options[OPT_232THNUMCRUST].cDefault, "Primordial Earth");
  sprintf(options[OPT_232THNUMCRUST].cDimension, "nd");
  options[OPT_232THNUMCRUST].iType      = 2;
  options[OPT_232THNUMCRUST].bMultiFile = 1;
  options[OPT_232THNUMCRUST].dNeg       = ENUMCRUST232TH;
  options[OPT_232THNUMCRUST].dDefault   = ENUMCRUST232TH;
  sprintf(options[OPT_232THNUMCRUST].cNeg, "Primordial Earth Units");
  fnRead[OPT_232THNUMCRUST] = &fvRead232ThNumCrust;

  sprintf(options[OPT_232THPOWERCRUST].cName, "d232ThPowerCrust");
  sprintf(options[OPT_232THPOWERCRUST].cDescr,
          "Initial Crust Power Production from 232Th Atoms");
  sprintf(options[OPT_232THPOWERCRUST].cDefault, "Primordial Earth");
  sprintf(options[OPT_232THPOWERCRUST].cDimension, "energy/time");
  options[OPT_232THPOWERCRUST].iType      = 2;
  options[OPT_232THPOWERCRUST].bMultiFile = 1;
  options[OPT_232THPOWERCRUST].dNeg     = EPOWERCRUST232TH; // 1e12*1e-7; // cgs
  options[OPT_232THPOWERCRUST].dDefault = EPOWERCRUST232TH;
  sprintf(options[OPT_232THPOWERCRUST].cNeg, "Primordial Earth Units");
  fnRead[OPT_232THPOWERCRUST] = &fvRead232ThPowerCrust;

  /* 238U*/
  sprintf(options[OPT_238UMASSMAN].cName, "d238UMassMan");
  sprintf(options[OPT_238UMASSMAN].cDescr, "Initial Mass of 238U");
  sprintf(options[OPT_238UMASSMAN].cDefault, "Primordial Earth");
  sprintf(options[OPT_238UMASSMAN].cDimension, "mass");
  options[OPT_238UMASSMAN].iType      = 2;
  options[OPT_238UMASSMAN].bMultiFile = 1;
  options[OPT_238UMASSMAN].dNeg       = EMASSMAN238U;
  options[OPT_238UMASSMAN].dDefault   = EMASSMAN238U;
  sprintf(options[OPT_238UMASSMAN].cNeg, "Primordial Earth Units");
  fnRead[OPT_238UMASSMAN] = &fvRead238UMassMan;

  sprintf(options[OPT_238UNUMMAN].cName, "d238UNumMan");
  sprintf(options[OPT_238UNUMMAN].cDescr, "Initial Number of 238U Atoms");
  sprintf(options[OPT_238UNUMMAN].cDefault, "Primordial Earth");
  sprintf(options[OPT_238UNUMMAN].cDimension, "nd");
  options[OPT_238UNUMMAN].dDefault   = 0;
  options[OPT_238UNUMMAN].iType      = 2;
  options[OPT_238UNUMMAN].bMultiFile = 1;
  options[OPT_238UNUMMAN].dNeg       = ENUMMAN238U;
  options[OPT_238UNUMMAN].dDefault   = ENUMMAN238U;
  sprintf(options[OPT_238UNUMMAN].cNeg, "Primordial Earth Units");
  fnRead[OPT_238UNUMMAN] = &fvRead238UNumMan;

  sprintf(options[OPT_238UPOWERMAN].cName, "d238UPowerMan"); // section added
                                                             // PED
  sprintf(options[OPT_238UPOWERMAN].cDescr,
          "Initial Power Production from 238U Atoms");
  sprintf(options[OPT_238UPOWERMAN].cDefault, "Primordial Earth");
  sprintf(options[OPT_238UPOWERMAN].cDimension, "energy/time");
  options[OPT_238UPOWERMAN].iType      = 2;
  options[OPT_238UPOWERMAN].bMultiFile = 1;
  options[OPT_238UPOWERMAN].dNeg       = EPOWERMAN238U; // 1e12*1e-7; // cgs
  options[OPT_238UPOWERMAN].dDefault   = EPOWERMAN238U;
  sprintf(options[OPT_238UPOWERMAN].cNeg, "Primordial Earth Units");
  fnRead[OPT_238UPOWERMAN] = &fvRead238UPowerMan;

  sprintf(options[OPT_238UMASSCORE].cName, "d238UMassCore");
  sprintf(options[OPT_238UMASSCORE].cDescr, "Initial Core Mass of 238U");
  sprintf(options[OPT_238UMASSCORE].cDefault, "Primordial Earth");
  sprintf(options[OPT_238UMASSCORE].cDimension, "mass");
  options[OPT_238UMASSCORE].iType      = 2;
  options[OPT_238UMASSCORE].bMultiFile = 1;
  options[OPT_238UMASSCORE].dNeg       = EMASSCORE238U;
  options[OPT_238UMASSCORE].dDefault   = EMASSCORE238U;
  sprintf(options[OPT_238UMASSCORE].cNeg, "Primordial Earth Units");
  fnRead[OPT_238UMASSCORE] = &fvRead238UMassCore;

  sprintf(options[OPT_238UNUMCORE].cName, "d238UNumCore");
  sprintf(options[OPT_238UNUMCORE].cDescr, "Initial Core Number of 238U Atoms");
  sprintf(options[OPT_238UNUMCORE].cDefault, "Primordial Earth");
  sprintf(options[OPT_238UNUMCORE].cDimension, "nd");
  options[OPT_238UNUMCORE].dDefault   = 0;
  options[OPT_238UNUMCORE].iType      = 2;
  options[OPT_238UNUMCORE].bMultiFile = 1;
  options[OPT_238UNUMCORE].dNeg       = ENUMCORE238U;
  options[OPT_238UNUMCORE].dDefault   = ENUMCORE238U;
  sprintf(options[OPT_238UNUMCORE].cNeg, "Primordial Earth Units");
  fnRead[OPT_238UNUMCORE] = &fvRead238UNumCore;

  sprintf(options[OPT_238UPOWERCORE].cName,
          "d238UPowerCore"); // section added PED
  sprintf(options[OPT_238UPOWERCORE].cDescr,
          "Initial Core Power Production from 238U Atoms");
  sprintf(options[OPT_238UPOWERCORE].cDefault, "Primordial Earth");
  sprintf(options[OPT_238UPOWERCORE].cDimension, "energy/time");
  options[OPT_238UPOWERCORE].iType      = 2;
  options[OPT_238UPOWERCORE].bMultiFile = 1;
  options[OPT_238UPOWERCORE].dNeg       = EPOWERCORE238U; // 1e12*1e-7; // cgs
  options[OPT_238UPOWERCORE].dDefault   = EPOWERCORE238U;
  sprintf(options[OPT_238UPOWERCORE].cNeg, "Primordial Earth Units");
  fnRead[OPT_238UPOWERCORE] = &fvRead238UPowerCore;

  sprintf(options[OPT_238UMASSCRUST].cName, "d238UMassCrust");
  sprintf(options[OPT_238UMASSCRUST].cDescr, "Initial Crust Mass of 238U");
  sprintf(options[OPT_238UMASSCRUST].cDefault, "Primordial Earth");
  sprintf(options[OPT_238UMASSCRUST].cDimension, "mass");
  options[OPT_238UMASSCRUST].iType      = 2;
  options[OPT_238UMASSCRUST].bMultiFile = 1;
  options[OPT_238UMASSCRUST].dNeg       = EMASSCRUST238U;
  options[OPT_238UMASSCRUST].dDefault   = EMASSCRUST238U;
  sprintf(options[OPT_238UMASSCRUST].cNeg, "Primordial Earth Units");
  fnRead[OPT_238UMASSCRUST] = &fvRead238UMassCrust;

  sprintf(options[OPT_238UNUMCRUST].cName, "d238UNumCrust");
  sprintf(options[OPT_238UNUMCRUST].cDescr,
          "Initial Crust Number of 238U Atoms");
  sprintf(options[OPT_238UNUMCRUST].cDefault, "Primordial Earth");
  sprintf(options[OPT_238UNUMCRUST].cDimension, "nd");
  options[OPT_238UNUMCRUST].dDefault   = 0;
  options[OPT_238UNUMCRUST].iType      = 2;
  options[OPT_238UNUMCRUST].bMultiFile = 1;
  options[OPT_238UNUMCRUST].dNeg       = ENUMCRUST238U;
  options[OPT_238UNUMCRUST].dDefault   = ENUMCRUST238U;
  sprintf(options[OPT_238UNUMCRUST].cNeg, "Primordial Earth Units");
  fnRead[OPT_238UNUMCRUST] = &fvRead238UNumCrust;

  sprintf(options[OPT_238UPOWERCRUST].cName,
          "d238UPowerCrust"); // section added PED
  sprintf(options[OPT_238UPOWERCRUST].cDescr,
          "Initial Crust Power Production from 238U Atoms");
  sprintf(options[OPT_238UPOWERCRUST].cDefault, "Primordial Earth");
  sprintf(options[OPT_238UPOWERCRUST].cDimension, "energy/time");
  options[OPT_238UPOWERCRUST].iType      = 2;
  options[OPT_238UPOWERCRUST].bMultiFile = 1;
  options[OPT_238UPOWERCRUST].dNeg       = EPOWERCRUST238U; // 1e12*1e-7; // cgs
  options[OPT_238UPOWERCRUST].dDefault   = EPOWERCRUST238U;
  sprintf(options[OPT_238UPOWERCRUST].cNeg, "Primordial Earth Units");
  fnRead[OPT_238UPOWERCRUST] = &fvRead238UPowerCrust;

  /* 235U */
  sprintf(options[OPT_235UMASSMAN].cName, "d235UMassMan"); // PED
  sprintf(options[OPT_235UMASSMAN].cDescr, "Initial Mass of 235U");
  sprintf(options[OPT_235UMASSMAN].cDefault, "Primordial Earth");
  sprintf(options[OPT_235UMASSMAN].cDimension, "nd");
  options[OPT_235UMASSMAN].iType      = 2;
  options[OPT_235UMASSMAN].bMultiFile = 1;
  options[OPT_235UMASSMAN].dNeg       = EMASSMAN235U;
  options[OPT_235UMASSMAN].dDefault   = EMASSMAN235U;
  sprintf(options[OPT_235UMASSMAN].cNeg, "Primordial Earth Units");
  fnRead[OPT_235UMASSMAN] = &fvRead235UMassMan;

  sprintf(options[OPT_235UNUMMAN].cName, "d235UNumMan"); // PED
  sprintf(options[OPT_235UNUMMAN].cDescr, "Initial Number of 235U Atoms");
  sprintf(options[OPT_235UNUMMAN].cDefault, "Primordial Earth");
  sprintf(options[OPT_235UNUMMAN].cDimension, "nd");
  options[OPT_235UNUMMAN].iType      = 2;
  options[OPT_235UNUMMAN].bMultiFile = 1;
  options[OPT_235UNUMMAN].dNeg       = ENUMMAN235U;
  options[OPT_235UNUMMAN].dDefault   = ENUMMAN235U;
  sprintf(options[OPT_235UNUMMAN].cNeg, "Primordial Earth Units");
  fnRead[OPT_235UNUMMAN] = &fvRead235UNumMan;

  sprintf(options[OPT_235UPOWERMAN].cName, "d235UPowerMan"); // section added
                                                             // PED
  sprintf(options[OPT_235UPOWERMAN].cDescr,
          "Initial Power Production from 235U Atoms");
  sprintf(options[OPT_235UPOWERMAN].cDefault, "Primordial Earth");
  sprintf(options[OPT_235UPOWERMAN].cDimension, "energy/time");
  options[OPT_235UPOWERMAN].iType      = 2;
  options[OPT_235UPOWERMAN].bMultiFile = 1;
  options[OPT_235UPOWERMAN].dNeg       = EPOWERMAN235U;
  options[OPT_235UPOWERMAN].dDefault   = EPOWERMAN235U;
  sprintf(options[OPT_235UPOWERMAN].cNeg, "Primoridal Earth Units");
  fnRead[OPT_235UPOWERMAN] = &fvRead235UPowerMan;

  sprintf(options[OPT_235UMASSCORE].cName, "d235UMassCore");
  sprintf(options[OPT_235UMASSCORE].cDescr, "Initial Core Mass of 235U");
  sprintf(options[OPT_235UMASSCORE].cDefault, "Primordial Earth");
  sprintf(options[OPT_235UMASSCORE].cDimension, "mass");
  options[OPT_235UMASSCORE].iType      = 2;
  options[OPT_235UMASSCORE].bMultiFile = 1;
  options[OPT_235UMASSCORE].dNeg       = EMASSCORE235U;
  options[OPT_235UMASSCORE].dDefault   = EMASSCORE235U;
  sprintf(options[OPT_235UMASSCORE].cNeg, "Primordial Earth Units");
  fnRead[OPT_235UMASSCORE] = &fvRead235UMassCore;

  sprintf(options[OPT_235UNUMCORE].cName, "d235UNumCore");
  sprintf(options[OPT_235UNUMCORE].cDescr, "Initial Core Number of 235U Atoms");
  sprintf(options[OPT_235UNUMCORE].cDefault, "Primordial Earth");
  sprintf(options[OPT_235UNUMCORE].cDimension, "nd");
  options[OPT_235UNUMCORE].iType      = 2;
  options[OPT_235UNUMCORE].bMultiFile = 1;
  options[OPT_235UNUMCORE].dNeg       = ENUMCORE235U;
  options[OPT_235UNUMCORE].dDefault   = ENUMCORE235U;
  sprintf(options[OPT_235UNUMCORE].cNeg, "Primordial Earth Units");
  fnRead[OPT_235UNUMCORE] = &fvRead235UNumCore;

  sprintf(options[OPT_235UPOWERCORE].cName,
          "d235UPowerCore"); // section added PED
  sprintf(options[OPT_235UPOWERCORE].cDescr,
          "Initial Core Power Production from 235U Atoms");
  sprintf(options[OPT_235UPOWERCORE].cDefault, "Primordial Earth");
  sprintf(options[OPT_235UPOWERCORE].cDimension, "energy/time");
  options[OPT_235UPOWERCORE].iType      = 2;
  options[OPT_235UPOWERCORE].bMultiFile = 1;
  options[OPT_235UPOWERCORE].dNeg       = EPOWERCORE235U;
  options[OPT_235UPOWERCORE].dDefault   = EPOWERCORE235U;
  sprintf(options[OPT_235UPOWERCORE].cNeg, "Primordial Earth Units");
  fnRead[OPT_235UPOWERCORE] = &fvRead235UPowerCore;

  sprintf(options[OPT_235UMASSCRUST].cName, "d235UMassCrust");
  sprintf(options[OPT_235UMASSCRUST].cDescr, "Initial Crust Mass of 235U");
  sprintf(options[OPT_235UMASSCRUST].cDefault, "Primordial Earth");
  sprintf(options[OPT_235UMASSCRUST].cDimension, "mass");
  options[OPT_235UMASSCRUST].iType      = 2;
  options[OPT_235UMASSCRUST].bMultiFile = 1;
  options[OPT_235UMASSCRUST].dNeg       = EMASSCRUST235U;
  options[OPT_235UMASSCRUST].dDefault   = EMASSCRUST235U;
  sprintf(options[OPT_235UMASSCRUST].cNeg, "Primordial Earth Units");
  fnRead[OPT_235UMASSCRUST] = &fvRead235UMassCrust;

  sprintf(options[OPT_235UNUMCRUST].cName, "d235UNumCrust");
  sprintf(options[OPT_235UNUMCRUST].cDescr,
          "Initial Crust Number of 235U Atoms");
  sprintf(options[OPT_235UNUMCRUST].cDefault, "Primordial Earth");
  sprintf(options[OPT_235UNUMCRUST].cDimension, "nd");
  options[OPT_235UNUMCRUST].iType      = 2;
  options[OPT_235UNUMCRUST].bMultiFile = 1;
  options[OPT_235UNUMCRUST].dNeg       = ENUMCRUST235U;
  options[OPT_235UNUMCRUST].dDefault   = ENUMCRUST235U;
  sprintf(options[OPT_235UNUMCRUST].cNeg, "Primordial Earth Units");
  fnRead[OPT_235UNUMCRUST] = &fvRead235UNumCrust;

  sprintf(options[OPT_235UPOWERCRUST].cName,
          "d235UPowerCrust"); // section added PED
  sprintf(options[OPT_235UPOWERCRUST].cDescr,
          "Initial Crust Power Production from 235U Atoms");
  sprintf(options[OPT_235UPOWERCRUST].cDefault, "Primordial Earth");
  sprintf(options[OPT_235UPOWERCRUST].cDimension, "energy/time");
  options[OPT_235UPOWERCRUST].iType      = 2;
  options[OPT_235UPOWERCRUST].bMultiFile = 1;
  options[OPT_235UPOWERCRUST].dNeg       = EPOWERCRUST235U;
  options[OPT_235UPOWERCRUST].dDefault   = EPOWERCRUST235U;
  sprintf(options[OPT_235UPOWERCRUST].cNeg, "Primordial Earth Units");
  fnRead[OPT_235UPOWERCRUST] = &fvRead235UPowerCrust;

  sprintf(options[OPT_HALT40KPOWER].cName, "dHalt40KPower");
  sprintf(options[OPT_HALT40KPOWER].cDescr, "Minimum 40K Power");
  sprintf(options[OPT_HALT40KPOWER].cDefault, "0");
  sprintf(options[OPT_HALT40KPOWER].cDimension, "energy/time");
  options[OPT_HALT40KPOWER].iType      = 2;
  options[OPT_HALT40KPOWER].bMultiFile = 1;
  options[OPT_HALT40KPOWER].dNeg       = 1e12; // TW
  options[OPT_HALT40KPOWER].dDefault   = 0;
  sprintf(options[OPT_HALT40KPOWER].cNeg, "TW");
  fnRead[OPT_HALT40KPOWER] = &fvReadHalt40KPower;

  sprintf(options[OPT_HALT232THPOWER].cName, "dHalt232ThPower");
  sprintf(options[OPT_HALT232THPOWER].cDescr, "Minimum 232Th Power");
  sprintf(options[OPT_HALT232THPOWER].cDefault, "0");
  sprintf(options[OPT_HALT232THPOWER].cDimension, "energy/time");
  options[OPT_HALT232THPOWER].iType      = 2;
  options[OPT_HALT232THPOWER].bMultiFile = 1;
  options[OPT_HALT232THPOWER].dNeg       = 1e12; // TW
  options[OPT_HALT232THPOWER].dDefault   = 0;
  sprintf(options[OPT_HALT232THPOWER].cNeg, "TW");
  fnRead[OPT_HALT232THPOWER] = &fvReadHalt232ThPower;

  sprintf(options[OPT_HALT235UPOWER].cName, "dHalt235UPower");
  sprintf(options[OPT_HALT235UPOWER].cDescr, "Minimum 235U Power");
  sprintf(options[OPT_HALT235UPOWER].cDefault, "0");
  sprintf(options[OPT_HALT235UPOWER].cDimension, "energy/time");
  options[OPT_HALT235UPOWER].iType      = 2;
  options[OPT_HALT235UPOWER].bMultiFile = 1;
  options[OPT_HALT235UPOWER].dNeg       = 1e12; // TW
  options[OPT_HALT235UPOWER].dDefault   = 0;
  sprintf(options[OPT_HALT235UPOWER].cNeg, "TW");
  fnRead[OPT_HALT235UPOWER] = &fvReadHalt235UPower;

  sprintf(options[OPT_HALT238UPOWER].cName, "dHalt238UPower");
  sprintf(options[OPT_HALT238UPOWER].cDescr, "Minimum 238U Power");
  sprintf(options[OPT_HALT238UPOWER].cDefault, "0");
  sprintf(options[OPT_HALT238UPOWER].cDimension, "energy/time");
  options[OPT_HALT238UPOWER].iType      = 2;
  options[OPT_HALT238UPOWER].bMultiFile = 1;
  options[OPT_HALT238UPOWER].dNeg       = 1e12; // TW
  options[OPT_HALT238UPOWER].dDefault   = 0;
  sprintf(options[OPT_HALT238UPOWER].cNeg, "TW");
  fnRead[OPT_HALT238UPOWER] = &fvReadHalt238UPower;

  sprintf(options[OPT_HALTRADPOWER].cName, "dHaltRadPower");
  sprintf(options[OPT_HALTRADPOWER].cDescr, "Minimum Total Radgiogenic Power");
  sprintf(options[OPT_HALTRADPOWER].cDefault, "0");
  sprintf(options[OPT_HALTRADPOWER].cDimension, "energy/time");
  options[OPT_HALTRADPOWER].iType      = 2;
  options[OPT_HALTRADPOWER].bMultiFile = 1;
  options[OPT_HALTRADPOWER].dNeg       = 1e12; // TW
  options[OPT_HALTRADPOWER].dDefault   = 0;
  sprintf(options[OPT_HALTRADPOWER].cNeg, "TW");
  fnRead[OPT_HALTRADPOWER] = &fvReadHaltRadPower;
}
/**
   Read input option

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param system System struct
   @param Function to read option
   @param iBody Index of body
*/
void fvReadOptionsRadheat(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system,
                          fnReadOption fnRead[], int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTRADHEAT; iOpt < OPTENDRADHEAT; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}

/******************* Verify RADHEAT ******************/
/**
   Verify that mass and number are not both input

   @param options Options struct
   @param iMass Index of mass
   @param iNum Index of number
   @param iBody Index of body
*/
void fvNotMassAndNum(OPTIONS *options, int iMass, int iNum, int iBody) {
  if (options[iMass].iLine[iBody] >= 0 && options[iNum].iLine[iBody] >= 0) {
    fprintf(stderr, "ERROR: Cannot set both %s and %s.\n", options[iMass].cName,
            options[iNum].cName);
    DoubleLineExit(options[iMass].cFile[iBody], options[iNum].cFile[iBody],
                   options[iMass].iLine[iBody], options[iNum].iLine[iBody]);
  }
}
/**
   Assign 26Al number to body

   @param body Body struct
   @param options Options struct
   @param Age
   @param iBody Index of body
*/
/* Assign Nums */
void fvAssign26AlNum(BODY *body, OPTIONS *options, double dAge, int iBody) {
  /* Mantle */
  if (options[OPT_26ALMASSMAN].iLine[iBody + 1] >= 0) {
    body[iBody].d26AlNumMan = body[iBody].d26AlMassMan / (MASS26AL);
  }
  if (options[OPT_26ALNUMMAN].iLine[iBody + 1] >= 0) {
    // Do nothing bc default Num set.
  }
  if (options[OPT_26ALPOWERMAN].iLine[iBody + 1] >= 0) {
    body[iBody].d26AlNumMan =
          body[iBody].d26AlPowerMan / (ENERGY26AL) * (HALFLIFE26AL);
  }
  body[iBody].d26AlConstMan = fd26AlConstant(
        body[iBody].d26AlNumMan, dAge); // Get the constant given num and age.

  /* Core */
  if (options[OPT_26ALMASSCORE].iLine[iBody + 1] >= 0) {
    body[iBody].d26AlNumCore = body[iBody].d26AlMassCore / (MASS26AL);
  }
  if (options[OPT_26ALNUMCORE].iLine[iBody + 1] >= 0) {
  }
  if (options[OPT_26ALPOWERCORE].iLine[iBody + 1] >= 0) {
    body[iBody].d26AlNumCore =
          body[iBody].d26AlPowerCore / (ENERGY26AL) * (HALFLIFE26AL);
  }
  body[iBody].d26AlConstCore = fd26AlConstant(body[iBody].d26AlNumCore, dAge);
}
/**
   Assign 40K number to body

   @param body Body struct
   @param options Options struct
   @param Age
   @param iBody Index of body
*/
void fvAssign40KNum(BODY *body, OPTIONS *options, double dAge, int iBody) {
  /* Mantle */
  if (options[OPT_40KMASSMAN].iLine[iBody + 1] >= 0) {
    body[iBody].d40KNumMan = body[iBody].d40KMassMan / (MASS40K);
  }
  if (options[OPT_40KNUMMAN].iLine[iBody + 1] >= 0) {
    // Do nothing bc default Num set.
  }
  if (options[OPT_40KPOWERMAN].iLine[iBody + 1] >= 0) {
    body[iBody].d40KNumMan =
          body[iBody].d40KPowerMan / (ENERGY40K) * (HALFLIFE40K);
  }
  body[iBody].d40KConstMan = fd40KConstant(
        body[iBody].d40KNumMan, dAge); // Get the constant given num and age.

  /* Core */
  if (options[OPT_40KMASSCORE].iLine[iBody + 1] >= 0) {
    body[iBody].d40KNumCore = body[iBody].d40KMassCore / (MASS40K);
  }
  if (options[OPT_40KNUMCORE].iLine[iBody + 1] >= 0) {
  }
  if (options[OPT_40KPOWERCORE].iLine[iBody + 1] >= 0) {
    body[iBody].d40KNumCore =
          body[iBody].d40KPowerCore / (ENERGY40K) * (HALFLIFE40K);
  }
  body[iBody].d40KConstCore = fd40KConstant(body[iBody].d40KNumCore, dAge);

  /* Crust */
  if (options[OPT_40KMASSCRUST].iLine[iBody + 1] >= 0) {
    body[iBody].d40KNumCrust = body[iBody].d40KMassCrust / (MASS40K);
  }
  if (options[OPT_40KNUMCRUST].iLine[iBody + 1] >= 0) {
  }
  if (options[OPT_40KPOWERCRUST].iLine[iBody + 1] >= 0) {
    body[iBody].d40KNumCrust =
          body[iBody].d40KPowerCrust / (ENERGY40K) * (HALFLIFE40K);
  }
  body[iBody].d40KConstCrust = fd40KConstant(body[iBody].d40KNumCrust, dAge);
}
/**
   Assign 232Th number to body

   @param body Body struct
   @param options Options struct
   @param Age
   @param iBody Index of body
*/
void fvAssign232ThNum(BODY *body, OPTIONS *options, double dAge, int iBody) {
  /* Mantle */
  if (options[OPT_232THMASSMAN].iLine[iBody + 1] >= 0) {
    body[iBody].d232ThNumMan = body[iBody].d232ThMassMan / (MASS232TH);
  }
  if (options[OPT_232THNUMMAN].iLine[iBody + 1] >=
      0) { // Do nothing, use default.
  }
  if (options[OPT_232THPOWERMAN].iLine[iBody + 1] >= 0) {
    body[iBody].d232ThNumMan =
          body[iBody].d232ThPowerMan / (ENERGY232TH) * (HALFLIFE232TH);
  }
  body[iBody].d232ThConstMan = fd232ThConstant(body[iBody].d232ThNumMan, dAge);

  /* Core */
  if (options[OPT_232THMASSCORE].iLine[iBody + 1] >= 0) {
    body[iBody].d232ThNumCore = body[iBody].d232ThMassCore / (MASS232TH);
  }
  if (options[OPT_232THNUMCORE].iLine[iBody + 1] >=
      0) { // Do nothing, use default.
  }
  if (options[OPT_232THPOWERCORE].iLine[iBody + 1] >= 0) {
    body[iBody].d232ThNumCore =
          body[iBody].d232ThPowerCore / (ENERGY232TH) * (HALFLIFE232TH);
  }
  body[iBody].d232ThConstCore =
        fd232ThConstant(body[iBody].d232ThNumCore, dAge);

  /* Crust */
  if (options[OPT_232THMASSCRUST].iLine[iBody + 1] >= 0) {
    body[iBody].d232ThNumCrust = body[iBody].d232ThMassCrust / (MASS232TH);
  }
  if (options[OPT_232THNUMCRUST].iLine[iBody + 1] >=
      0) { // Do nothing, use default.
  }
  if (options[OPT_232THPOWERCRUST].iLine[iBody + 1] >= 0) {
    body[iBody].d232ThNumCrust =
          body[iBody].d232ThPowerCrust / (ENERGY232TH) * (HALFLIFE232TH);
  }
  body[iBody].d232ThConstCrust =
        fd232ThConstant(body[iBody].d232ThNumCrust, dAge);
}
/**
   Assign 238U number to body

   @param body Body struct
   @param options Options struct
   @param Age
   @param iBody Index of body
*/
void fvAssign238UNum(BODY *body, OPTIONS *options, double dAge, int iBody) {
  /* Mantle */
  if (options[OPT_238UMASSMAN].iLine[iBody + 1] >= 0) {
    body[iBody].d238UNumMan = body[iBody].d238UMassMan / (MASS238U);
  }
  if (options[OPT_238UNUMMAN].iLine[iBody + 1] >= 0) {
  }
  if (options[OPT_238UPOWERMAN].iLine[iBody + 1] >= 0) {
    body[iBody].d238UNumMan =
          body[iBody].d238UPowerMan / (ENERGY238U) * (HALFLIFE238U);
  }
  body[iBody].d238UConstMan = fd238UConstant(body[iBody].d238UNumMan, dAge);
  /* Core */
  if (options[OPT_238UMASSCORE].iLine[iBody + 1] >= 0) {
    body[iBody].d238UNumCore = body[iBody].d238UMassCore / (MASS238U);
  }
  if (options[OPT_238UNUMCORE].iLine[iBody + 1] >= 0) {
  }
  if (options[OPT_238UPOWERCORE].iLine[iBody + 1] >= 0) {
    body[iBody].d238UNumCore =
          body[iBody].d238UPowerCore / (ENERGY238U) * (HALFLIFE238U);
  }
  body[iBody].d238UConstCore = fd238UConstant(body[iBody].d238UNumCore, dAge);
  /* Crust */
  if (options[OPT_238UMASSCRUST].iLine[iBody + 1] >= 0) {
    body[iBody].d238UNumCrust = body[iBody].d238UMassCrust / (MASS238U);
  }
  if (options[OPT_238UNUMCRUST].iLine[iBody + 1] >= 0) {
  }
  if (options[OPT_238UPOWERCRUST].iLine[iBody + 1] >= 0) {
    body[iBody].d238UNumCrust =
          body[iBody].d238UPowerCrust / (ENERGY238U) * (HALFLIFE238U);
  }
  body[iBody].d238UConstCrust = fd238UConstant(body[iBody].d238UNumCrust, dAge);
}
/**
   Assign 235U number to body

   @param body Body struct
   @param options Options struct
   @param Age
   @param iBody Index of body
*/
void fvAssign235UNum(BODY *body, OPTIONS *options, double dAge,
                     int iBody) { // PED
  /* Mantle */
  if (options[OPT_235UMASSMAN].iLine[iBody + 1] >= 0) {
    body[iBody].d235UNumMan = body[iBody].d235UMassMan / (MASS235U);
  }
  if (options[OPT_235UNUMMAN].iLine[iBody + 1] >= 0) {
  }
  if (options[OPT_235UPOWERMAN].iLine[iBody + 1] >= 0) {
    body[iBody].d235UNumMan =
          body[iBody].d235UPowerMan / (ENERGY235U) * (HALFLIFE235U);
  }
  body[iBody].d235UConstMan = fd235UConstant(body[iBody].d235UNumMan, dAge);
  /* Core */
  if (options[OPT_235UMASSCORE].iLine[iBody + 1] >= 0) {
    body[iBody].d235UNumCore = body[iBody].d235UMassCore / (MASS235U);
  }
  if (options[OPT_235UNUMCORE].iLine[iBody + 1] >= 0) {
  }
  if (options[OPT_235UPOWERCORE].iLine[iBody + 1] >= 0) {
    body[iBody].d235UNumCore =
          body[iBody].d235UPowerCore / (ENERGY235U) * (HALFLIFE235U);
  }
  body[iBody].d235UConstCore = fd235UConstant(body[iBody].d235UNumCore, dAge);
  /* Crust */
  if (options[OPT_235UMASSCRUST].iLine[iBody + 1] >= 0) {
    body[iBody].d235UNumCrust = body[iBody].d235UMassCrust / (MASS235U);
  }
  if (options[OPT_235UNUMCRUST].iLine[iBody + 1] >= 0) {
  }
  if (options[OPT_235UPOWERCRUST].iLine[iBody + 1] >= 0) {
    body[iBody].d235UNumCrust =
          body[iBody].d235UPowerCrust / (ENERGY235U) * (HALFLIFE235U);
  }
  body[iBody].d235UConstCrust = fd235UConstant(body[iBody].d235UNumCrust, dAge);
}
/**
   Verify 26Al is initialized

   @param body Body struct
   @param options Options struct
   @param system System struct
   @param update Update struct
   @param Age
   @param Function to update variable
   @param iBody Index of body
*/
/* Verify */
void fvVerify26Al(BODY *body, OPTIONS *options, SYSTEM *system, UPDATE *update,
                  double dAge, int iBody) {
  fvAssign26AlNum(body, options, dAge, iBody);

  /* Mantle */
  if (update[iBody].i26AlMan >= 0) {
    update[iBody].iaType[update[iBody].i26AlMan][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i26AlMan][0] = 1;
    update[iBody].iaBody[update[iBody].i26AlMan][0] =
          malloc(update[iBody].iNumBodies[update[iBody].i26AlMan][0] *
                 sizeof(int)); // iaBody is the number of bodies that are
                               // affected by this variable.
    update[iBody].iaBody[update[iBody].i26AlMan][0][0] = iBody;
    // Initialize derivative so radiogenic power is known at the beginning
    update[iBody].daDerivProc[update[iBody].i26AlMan][0] = fdD26AlNumManDt(
          body, system, update[iBody].iaBody[update[iBody].i26AlMan][0]);
    update[iBody].pdD26AlNumManDt =
          &update[iBody].daDerivProc[update[iBody].i26AlMan][0];
  } else {
    // Set to zero, so RadPower calculations can still be used
    update[iBody].pdD26AlNumManDt = &update[iBody].dZero;
  }

  /* Core */
  if (update[iBody].i26AlCore >= 0) {
    update[iBody].iaType[update[iBody].i26AlCore][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i26AlCore][0] = 1;
    update[iBody].iaBody[update[iBody].i26AlCore][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].i26AlCore][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].i26AlCore][0][0] = iBody;
    // Initialize derivative so radiogenic power is known at the beginning
    update[iBody].daDerivProc[update[iBody].i26AlCore][0] = fdD26AlNumCoreDt(
          body, system, update[iBody].iaBody[update[iBody].i26AlCore][0]);
    update[iBody].pdD26AlNumCoreDt =
          &update[iBody].daDerivProc[update[iBody].i26AlCore][0];
  } else {
    update[iBody].pdD26AlNumCoreDt = &update[iBody].dZero;
  }
}
/**
   Verify 40K is initialized

   @param body Body struct
   @param options Options struct
   @param system System struct
   @param update Update struct
   @param Age
   @param Function to update variable
   @param iBody Index of body
*/
void fvVerify40K(BODY *body, OPTIONS *options, SYSTEM *system, UPDATE *update,
                 double dAge, int iBody) {
  fvAssign40KNum(body, options, dAge, iBody);
  /* Mantle */
  if (update[iBody].i40KMan >= 0) {
    update[iBody].iaType[update[iBody].i40KMan][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i40KMan][0] = 1;
    update[iBody].iaBody[update[iBody].i40KMan][0] =
          malloc(update[iBody].iNumBodies[update[iBody].i40KMan][0] *
                 sizeof(int)); // iaBody is the number of bodies that are
                               // affected by this variable.
    update[iBody].iaBody[update[iBody].i40KMan][0][0] = iBody;
    // Initialize derivative so radiogenic power is known at the beginning
    update[iBody].daDerivProc[update[iBody].i40KMan][0] = fdD40KNumManDt(
          body, system, update[iBody].iaBody[update[iBody].i40KMan][0]);
    update[iBody].pdD40KNumManDt =
          &update[iBody].daDerivProc[update[iBody].i40KMan][0];
  } else {
    // Set to zero, so RadPower calculations can still be used
    update[iBody].pdD40KNumManDt = &update[iBody].dZero;
  }

  /* Core */
  if (update[iBody].i40KCore >= 0) {
    update[iBody].iaType[update[iBody].i40KCore][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i40KCore][0] = 1;
    update[iBody].iaBody[update[iBody].i40KCore][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].i40KCore][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].i40KCore][0][0]   = iBody;
    update[iBody].daDerivProc[update[iBody].i40KCore][0] = fdD40KNumCoreDt(
          body, system, update[iBody].iaBody[update[iBody].i40KCore][0]);
    update[iBody].pdD40KNumCoreDt =
          &update[iBody].daDerivProc[update[iBody].i40KCore][0];
  } else {
    update[iBody].pdD40KNumCoreDt = &update[iBody].dZero;
  }

  /* Crust */
  if (update[iBody].i40KCrust >= 0) {
    update[iBody].iaType[update[iBody].i40KCrust][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i40KCrust][0] = 1;
    update[iBody].iaBody[update[iBody].i40KCrust][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].i40KCrust][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].i40KCrust][0][0]   = iBody;
    update[iBody].daDerivProc[update[iBody].i40KCrust][0] = fdD40KNumCrustDt(
          body, system, update[iBody].iaBody[update[iBody].i40KCrust][0]);
    update[iBody].pdD40KNumCrustDt =
          &update[iBody].daDerivProc[update[iBody].i40KCrust][0];
  } else {
    update[iBody].pdD40KNumCrustDt = &update[iBody].dZero;
  }
}
/**
   Verify 232Th is initialized

   @param body Body struct
   @param options Options struct
   @param system System struct
   @param update Update struct
   @param Age
   @param Function to update variable
   @param iBody Index of body
*/
void fvVerify232Th(BODY *body, OPTIONS *options, SYSTEM *system, UPDATE *update,
                   double dAge, int iBody) {
  fvAssign232ThNum(body, options, dAge, iBody);
  /* Mantle */
  if (update[iBody].i232ThMan >= 0) {
    update[iBody].iaType[update[iBody].i232ThMan][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i232ThMan][0] = 1;
    update[iBody].iaBody[update[iBody].i232ThMan][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].i232ThMan][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].i232ThMan][0][0] = iBody;
    // Initialize derivative so radiogenic power is known at the beginning
    update[iBody].daDerivProc[update[iBody].i232ThMan][0] = fdD232ThNumManDt(
          body, system, update[iBody].iaBody[update[iBody].i232ThMan][0]);
    update[iBody].pdD232ThNumManDt =
          &update[iBody].daDerivProc[update[iBody].i232ThMan][0];
  } else {
    update[iBody].pdD232ThNumManDt = &update[iBody].dZero;
  }

  /* Core */
  if (update[iBody].i232ThCore >= 0) {
    update[iBody].iaType[update[iBody].i232ThCore][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i232ThCore][0] = 1;
    update[iBody].iaBody[update[iBody].i232ThCore][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].i232ThCore][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].i232ThCore][0][0]   = iBody;
    update[iBody].daDerivProc[update[iBody].i232ThCore][0] = fdD232ThNumCoreDt(
          body, system, update[iBody].iaBody[update[iBody].i232ThCore][0]);
    update[iBody].pdD232ThNumCoreDt =
          &update[iBody].daDerivProc[update[iBody].i232ThCore][0];
  } else {
    update[iBody].pdD232ThNumCoreDt = &update[iBody].dZero;
  }

  /* Crust */
  if (update[iBody].i232ThCrust >= 0) {
    update[iBody].iaType[update[iBody].i232ThCrust][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i232ThCrust][0] = 1;
    update[iBody].iaBody[update[iBody].i232ThCrust][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].i232ThCrust][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].i232ThCrust][0][0] = iBody;
    update[iBody].daDerivProc[update[iBody].i232ThCrust][0] =
          fdD232ThNumCrustDt(
                body, system,
                update[iBody].iaBody[update[iBody].i232ThCrust][0]);
    update[iBody].pdD232ThNumCrustDt =
          &update[iBody].daDerivProc[update[iBody].i232ThCrust][0];
  } else {
    update[iBody].pdD232ThNumCrustDt = &update[iBody].dZero;
  }
}
/**
   Verify 238U is initialized

   @param body Body struct
   @param options Options struct
   @param system System struct
   @param update Update struct
   @param Age
   @param Function to update variable
   @param iBody Index of body
*/
void fvVerify238U(BODY *body, OPTIONS *options, SYSTEM *system, UPDATE *update,
                  double dAge, int iBody) {
  fvAssign238UNum(body, options, dAge, iBody);
  /* Mantle */
  if (update[iBody].i238UMan >= 0) {
    update[iBody].iaType[update[iBody].i238UMan][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i238UMan][0] = 1;
    update[iBody].iaBody[update[iBody].i238UMan][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].i238UMan][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].i238UMan][0][0] = iBody;
    // Initialize derivative so radiogenic power is known at the beginning
    update[iBody].daDerivProc[update[iBody].i238UMan][0] = fdD238UNumManDt(
          body, system, update[iBody].iaBody[update[iBody].i238UMan][0]);
    update[iBody].pdD238UNumManDt =
          &update[iBody].daDerivProc[update[iBody].i238UMan][0];
  } else {
    update[iBody].pdD238UNumManDt = &update[iBody].dZero;
  }

  /* Core */
  if (update[iBody].i238UCore >= 0) {
    update[iBody].iaType[update[iBody].i238UCore][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i238UCore][0] = 1;
    update[iBody].iaBody[update[iBody].i238UCore][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].i238UCore][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].i238UCore][0][0]   = iBody;
    update[iBody].daDerivProc[update[iBody].i238UCore][0] = fdD238UNumCoreDt(
          body, system, update[iBody].iaBody[update[iBody].i238UCore][0]);
    update[iBody].pdD238UNumCoreDt =
          &update[iBody].daDerivProc[update[iBody].i238UCore][0];
  } else {
    update[iBody].pdD238UNumCoreDt = &update[iBody].dZero;
  }

  /* Crust */
  if (update[iBody].i238UCrust >= 0) {
    update[iBody].iaType[update[iBody].i238UCrust][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i238UCrust][0] = 1;
    update[iBody].iaBody[update[iBody].i238UCrust][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].i238UCrust][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].i238UCrust][0][0]   = iBody;
    update[iBody].daDerivProc[update[iBody].i238UCrust][0] = fdD238UNumCrustDt(
          body, system, update[iBody].iaBody[update[iBody].i238UCrust][0]);
    update[iBody].pdD238UNumCrustDt =
          &update[iBody].daDerivProc[update[iBody].i238UCrust][0];
  } else {
    update[iBody].pdD238UNumCrustDt = &update[iBody].dZero;
  }
}
/**
   Verify 235U is initialized

   @param body Body struct
   @param options Options struct
   @param system System struct
   @param update Update struct
   @param Age
   @param Function to update variable
   @param iBody Index of body
*/
void fvVerify235U(BODY *body, OPTIONS *options, SYSTEM *system, UPDATE *update,
                  double dAge, int iBody) {
  fvAssign235UNum(body, options, dAge, iBody);
  /* Mantle */
  if (update[iBody].i235UMan >= 0) {
    update[iBody].iaType[update[iBody].i235UMan][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i235UMan][0] = 1;
    update[iBody].iaBody[update[iBody].i235UMan][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].i235UMan][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].i235UMan][0][0] = iBody;
    // Initialize derivative so radiogenic power is known at the beginning
    update[iBody].daDerivProc[update[iBody].i235UMan][0] = fdD235UNumManDt(
          body, system, update[iBody].iaBody[update[iBody].i235UMan][0]);
    update[iBody].pdD235UNumManDt =
          &update[iBody].daDerivProc[update[iBody].i235UMan][0];
  } else {
    update[iBody].pdD235UNumManDt = &update[iBody].dZero;
  }

  /* Core */
  if (update[iBody].i235UCore >= 0) {
    update[iBody].iaType[update[iBody].i235UCore][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i235UCore][0] = 1;
    update[iBody].iaBody[update[iBody].i235UCore][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].i235UCore][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].i235UCore][0][0] = iBody;
    update[iBody].pdD235UNumCoreDt =
          &update[iBody].daDerivProc[update[iBody].i235UCore][0];
    update[iBody].daDerivProc[update[iBody].i235UCore][0] = fdD235UNumCoreDt(
          body, system, update[iBody].iaBody[update[iBody].i235UCore][0]);
  } else {
    update[iBody].pdD235UNumCoreDt = &update[iBody].dZero;
  }

  /* Crust */
  if (update[iBody].i235UCrust >= 0) {
    update[iBody].iaType[update[iBody].i235UCrust][0]     = 1;
    update[iBody].iNumBodies[update[iBody].i235UCrust][0] = 1;
    update[iBody].iaBody[update[iBody].i235UCrust][0]     = malloc(
              update[iBody].iNumBodies[update[iBody].i235UCrust][0] * sizeof(int));
    update[iBody].iaBody[update[iBody].i235UCrust][0][0] = iBody;
    update[iBody].pdD235UNumCrustDt =
          &update[iBody].daDerivProc[update[iBody].i235UCrust][0];
    update[iBody].daDerivProc[update[iBody].i235UCrust][0] = fdD235UNumCrustDt(
          body, system, update[iBody].iaBody[update[iBody].i235UCrust][0]);
  } else {
    update[iBody].pdD235UNumCrustDt = &update[iBody].dZero;
  }
}
/**
   Define auxiliary radheat body properties.

   @param body Body struct
   @param evolve Evolve struct
   @param update Update struct
   @param iBody Index of body
*/
/* Auxs Props */
void fvPropsAuxRadheat(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                       int iBody) {
  body[iBody].dRadPowerMan   = fdRadPowerMan(update, iBody);
  body[iBody].dRadPowerCore  = fdRadPowerCore(update, iBody);
  body[iBody].dRadPowerCrust = fdRadPowerCrust(update, iBody);
  body[iBody].dRadPowerTotal = fdRadPowerTotal(body, iBody);
}
/**
   Force behavior in radheat: Set number of radiogenic elements to zero if <
   0.5.

   @param body Body struct
   @param evolve Evolve struct
   @param io IO struct
   @param system System struct
   @param update Update struct
   @param iBody Index of body
*/
void fvForceBehaviorRadheat(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                            SYSTEM *system, UPDATE *update,
                            fnUpdateVariable ***fnUpdate, int iBody,
                            int iModule) {
  if (body[iBody].d26AlNumMan < 0.5) {
    body[iBody].d26AlNumMan = 0;
  }
  if (body[iBody].d26AlNumCore < 0.5) {
    body[iBody].d26AlNumCore = 0;
  }

  if (body[iBody].d40KNumMan < 0.5) {
    body[iBody].d40KNumMan = 0;
  }
  if (body[iBody].d40KNumCore < 0.5) {
    body[iBody].d40KNumCore = 0;
  }
  if (body[iBody].d40KNumCrust < 0.5) {
    body[iBody].d40KNumCrust = 0;
  }

  if (body[iBody].d232ThNumMan < 0.5) {
    body[iBody].d232ThNumMan = 0;
  }
  if (body[iBody].d232ThNumCore < 0.5) {
    body[iBody].d232ThNumCore = 0;
  }
  if (body[iBody].d232ThNumCrust < 0.5) {
    body[iBody].d232ThNumCrust = 0;
  }

  if (body[iBody].d238UNumMan < 0.5) {
    body[iBody].d238UNumMan = 0;
  }
  if (body[iBody].d238UNumCore < 0.5) {
    body[iBody].d238UNumCore = 0;
  }
  if (body[iBody].d238UNumCrust < 0.5) {
    body[iBody].d238UNumCrust = 0;
  }

  if (body[iBody].d235UNumMan < 0.5) {
    body[iBody].d235UNumMan = 0;
  }
  if (body[iBody].d235UNumCore < 0.5) {
    body[iBody].d235UNumCore = 0;
  }
  if (body[iBody].d235UNumCrust < 0.5) {
    body[iBody].d235UNumCrust = 0;
  }
}

void fvAssignRadheatDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                                fnUpdateVariable ***fnUpdate, int iBody) {
  // 26Al
  // Mantle
  if (update[iBody].i26AlMan >= 0) {
    fnUpdate[iBody][update[iBody].i26AlMan][0] = &fdD26AlNumManDt;
  }
  // Core
  if (update[iBody].i26AlCore >= 0) {
    fnUpdate[iBody][update[iBody].i26AlCore][0] = &fdD26AlNumCoreDt;
  }

  // 40K
  // Mantle
  if (update[iBody].i40KMan >= 0) {
    fnUpdate[iBody][update[iBody].i40KMan][0] = &fdD40KNumManDt;
  }
  // Core
  if (update[iBody].i40KCore >= 0) {
    fnUpdate[iBody][update[iBody].i40KCore][0] = &fdD40KNumCoreDt;
  }
  // Crust
  if (update[iBody].i40KCrust >= 0) {
    fnUpdate[iBody][update[iBody].i40KCrust][0] = &fdD40KNumCrustDt;
  }

  // 232Th
  // Mantle
  if (update[iBody].i232ThMan >= 0) {
    fnUpdate[iBody][update[iBody].i232ThMan][0] = &fdD232ThNumManDt;
  }
  // Core
  if (update[iBody].i232ThCore >= 0) {
    fnUpdate[iBody][update[iBody].i232ThCore][0] = &fdD232ThNumCoreDt;
  }
  // Crust
  if (update[iBody].i232ThCrust >= 0) {
    fnUpdate[iBody][update[iBody].i232ThCrust][0] = &fdD232ThNumCrustDt;
  }

  // 238U
  // Mantle
  if (update[iBody].i238UMan >= 0) {
    fnUpdate[iBody][update[iBody].i238UMan][0] = &fdD238UNumManDt;
  }
  // Core
  if (update[iBody].i238UCore >= 0) {
    fnUpdate[iBody][update[iBody].i238UCore][0] = &fdD238UNumCoreDt;
  }
  // Crust
  if (update[iBody].i238UCrust >= 0) {
    fnUpdate[iBody][update[iBody].i238UCrust][0] = &fdD238UNumCrustDt;
  }

  // 235U
  // Mantle
  if (update[iBody].i235UMan >= 0) {
    fnUpdate[iBody][update[iBody].i235UMan][0] = &fdD235UNumManDt;
  }
  // Core
  if (update[iBody].i235UCore >= 0) {
    fnUpdate[iBody][update[iBody].i235UCore][0] = &fdD235UNumCoreDt;
  }
  // Crust
  if (update[iBody].i235UCrust >= 0) {
    fnUpdate[iBody][update[iBody].i235UCrust][0] = &fdD235UNumCrustDt;
  }
}

void fvNullRadheatDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                              fnUpdateVariable ***fnUpdate, int iBody) {
  // 26Al
  // Mantle
  if (update[iBody].i26AlMan >= 0) {
    fnUpdate[iBody][update[iBody].i26AlMan][0] = &fndUpdateFunctionTiny;
  }
  // Core
  if (update[iBody].i26AlCore >= 0) {
    fnUpdate[iBody][update[iBody].i26AlCore][0] = &fndUpdateFunctionTiny;
  }

  // 40K
  // Mantle
  if (update[iBody].i40KMan >= 0) {
    fnUpdate[iBody][update[iBody].i40KMan][0] = &fndUpdateFunctionTiny;
  }
  // Core
  if (update[iBody].i40KCore >= 0) {
    fnUpdate[iBody][update[iBody].i40KCore][0] = &fndUpdateFunctionTiny;
  }
  // Crust
  if (update[iBody].i40KCrust >= 0) {
    fnUpdate[iBody][update[iBody].i40KCrust][0] = &fndUpdateFunctionTiny;
  }

  // 232Th
  // Mantle
  if (update[iBody].i232ThMan >= 0) {
    fnUpdate[iBody][update[iBody].i232ThMan][0] = &fndUpdateFunctionTiny;
  }
  // Core
  if (update[iBody].i232ThCore >= 0) {
    fnUpdate[iBody][update[iBody].i232ThCore][0] = &fndUpdateFunctionTiny;
  }
  // Crust
  if (update[iBody].i232ThCrust >= 0) {
    fnUpdate[iBody][update[iBody].i232ThCrust][0] = &fndUpdateFunctionTiny;
  }

  // 238U
  // Mantle
  if (update[iBody].i238UMan >= 0) {
    fnUpdate[iBody][update[iBody].i238UMan][0] = &fndUpdateFunctionTiny;
  }
  // Core
  if (update[iBody].i238UCore >= 0) {
    fnUpdate[iBody][update[iBody].i238UCore][0] = &fndUpdateFunctionTiny;
  }
  // Crust
  if (update[iBody].i238UCrust >= 0) {
    fnUpdate[iBody][update[iBody].i238UCrust][0] = &fndUpdateFunctionTiny;
  }

  // 235U
  // Mantle
  if (update[iBody].i235UMan >= 0) {
    fnUpdate[iBody][update[iBody].i235UMan][0] = &fndUpdateFunctionTiny;
  }
  // Core
  if (update[iBody].i235UCore >= 0) {
    fnUpdate[iBody][update[iBody].i235UCore][0] = &fndUpdateFunctionTiny;
  }
  // Crust
  if (update[iBody].i235UCrust >= 0) {
    fnUpdate[iBody][update[iBody].i235UCrust][0] = &fndUpdateFunctionTiny;
  }
}


/**
   Verify radheat is initialized: that both man and core initialize and not both
   mass and number specified.

   @param body Body struct
   @param control Control struct
   @param files Files struct
   @param options Options struct
   @param output Output struct
   @param system System struct
   @param update Update struct
   @param iBody Index of body
   @param iModule Index of module
*/
void fvVerifyRadheat(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, OUTPUT *output, SYSTEM *system,
                     UPDATE *update, int iBody, int iModule) {
  int iFile = iBody + 1;

  /* Cannot set 2 or more of Power, Mass and Number for any isotope */
  /* XXX Need a VerifyOneOfThree subroutine */
  /* Radheat is active for this body if this subroutine is called. */

  // XXX This looks like it's insufficient to capture all the permutations
  fvNotMassAndNum(options, OPT_26ALMASSMAN, OPT_26ALNUMMAN, iBody);
  // 26Al set properly
  fvVerify26Al(body, options, system, update, body[iBody].dAge,
               iBody); // Verify Man and Core.
  // XXX This looks like it's insufficient to capture all the permutations
  fvNotMassAndNum(options, OPT_40KMASSMAN, OPT_40KNUMMAN, iBody);
  // 40K set properly
  fvVerify40K(body, options, system, update, body[iBody].dAge,
              iBody); // Verify Man and Core.
  // 232Th
  fvNotMassAndNum(options, OPT_232THMASSMAN, OPT_232THNUMMAN, iFile);
  // 232Th set corectly
  fvVerify232Th(body, options, system, update, body[iBody].dAge, iBody);
  // 238U
  fvNotMassAndNum(options, OPT_238UMASSMAN, OPT_238UNUMMAN, iFile);
  // 238U set correctly
  fvVerify238U(body, options, system, update, body[iBody].dAge, iBody);
  // 235U
  fvNotMassAndNum(options, OPT_235UMASSMAN, OPT_235UNUMMAN, iFile);
  // 235U set correctly
  fvVerify235U(body, options, system, update, body[iBody].dAge, iBody);

  control->fnForceBehavior[iBody][iModule]   = &fvForceBehaviorRadheat;
  control->fnPropsAux[iBody][iModule]        = &fvPropsAuxRadheat;
  control->Evolve.fnBodyCopy[iBody][iModule] = &fvBodyCopyRadheat;
}


/**************** RADHEAT update ****************/
/**
   Initialize or update iNumVars and iNumSpecies in update struct.

   @param body Body struct
   @param update Update struct
   @param iBody Index of body
*/
void fvInitializeUpdateRadheat(BODY *body, UPDATE *update, int iBody) {
  /* Initially allow all radiogenic heat sources to be present. If any are 0,
     or < dMinRadPower, they will me removed from update[iBody] in
     ForceBehavior.
  */

  // Mantle
  if (body[iBody].d26AlNumMan > 0 || body[iBody].d26AlMassMan > 0 ||
      body[iBody].d26AlPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum26AlMan++;
  }

  if (body[iBody].d40KNumMan > 0 || body[iBody].d40KMassMan > 0 ||
      body[iBody].d40KPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum40KMan++;
  }

  if (body[iBody].d232ThNumMan > 0 || body[iBody].d232ThMassMan > 0 ||
      body[iBody].d232ThPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum232ThMan++;
  }

  if (body[iBody].d238UNumMan > 0 || body[iBody].d238UMassMan > 0 ||
      body[iBody].d238UPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum238UMan++;
  }

  if (body[iBody].d235UNumMan > 0 || body[iBody].d235UMassMan > 0 ||
      body[iBody].d235UPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum235UMan++;
  }

  // Core
  if (body[iBody].d26AlNumCore > 0 || body[iBody].d26AlMassCore > 0 ||
      body[iBody].d26AlPowerCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum26AlCore++;
  }

  if (body[iBody].d40KNumCore > 0 || body[iBody].d40KMassCore > 0 ||
      body[iBody].d40KPowerCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum40KCore++;
  }

  if (body[iBody].d232ThNumCore > 0 || body[iBody].d232ThMassCore > 0 ||
      body[iBody].d232ThPowerCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum232ThCore++;
  }

  if (body[iBody].d238UNumCore > 0 || body[iBody].d238UMassCore > 0 ||
      body[iBody].d238UPowerCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum238UCore++;
  }

  if (body[iBody].d235UNumCore > 0 || body[iBody].d235UMassCore > 0 ||
      body[iBody].d235UPowerCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum235UCore++;
  }

  // Crust
  if (body[iBody].d40KNumCrust > 0 || body[iBody].d40KMassCrust > 0 ||
      body[iBody].d40KPowerCrust > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum40KCrust++;
  }

  if (body[iBody].d232ThNumCrust > 0 || body[iBody].d232ThMassCrust > 0 ||
      body[iBody].d232ThPowerCrust > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum232ThCrust++;
  }

  if (body[iBody].d238UNumCrust > 0 || body[iBody].d238UMassCrust > 0 ||
      body[iBody].d238UPowerCrust > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum238UCrust++;
  }

  if (body[iBody].d235UNumCrust > 0 || body[iBody].d235UMassCrust > 0 ||
      body[iBody].d235UPowerCrust > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum235UCrust++;
  }
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate26AlNumManRadheat(BODY *body, UPDATE *update, int *iEqn,
                                       int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum26AlMan           = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate40KNumManRadheat(BODY *body, UPDATE *update, int *iEqn,
                                      int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum40KMan            = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate232ThNumManRadheat(BODY *body, UPDATE *update, int *iEqn,
                                        int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum232ThMan          = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate238UNumManRadheat(BODY *body, UPDATE *update, int *iEqn,
                                       int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum238UMan           = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate235UNumManRadheat(BODY *body, UPDATE *update, int *iEqn,
                                       int iVar, int iBody, int iFoo) { // PED
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum235UMan           = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate26AlNumCoreRadheat(BODY *body, UPDATE *update, int *iEqn,
                                        int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum26AlCore          = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate40KNumCoreRadheat(BODY *body, UPDATE *update, int *iEqn,
                                       int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum40KCore           = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate232ThNumCoreRadheat(BODY *body, UPDATE *update, int *iEqn,
                                         int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum232ThCore         = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate238UNumCoreRadheat(BODY *body, UPDATE *update, int *iEqn,
                                        int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum238UCore          = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate235UNumCoreRadheat(BODY *body, UPDATE *update, int *iEqn,
                                        int iVar, int iBody, int iFoo) { // PED
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum235UCore          = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate40KNumCrustRadheat(BODY *body, UPDATE *update, int *iEqn,
                                        int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum40KCrust          = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate232ThNumCrustRadheat(BODY *body, UPDATE *update, int *iEqn,
                                          int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum232ThCrust        = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate238UNumCrustRadheat(BODY *body, UPDATE *update, int *iEqn,
                                         int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum238UCrust         = (*iEqn)++;
}
/**
   Finalize update: update struct for this variable and equation gets module
   index and equation number.

   @param body Body struct
   @param update Update struct
   @param iEqn Index of equation
   @param iVar Index of variable
   @param iBody Index of body
*/
void fvFinalizeUpdate235UNumCrustRadheat(BODY *body, UPDATE *update, int *iEqn,
                                         int iVar, int iBody, int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum235UCrust         = (*iEqn)++;
}

/***************** RADHEAT Halts *****************/

/**
   Halt if minimum power reached.

   @param body Body struct
   @param evolve Evolve struct
   @param halt Halt struct
   @param io IO struct
   @param update Update struct
   @param iBody Index of body
*/
int fbHaltMin40KPower(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                      UPDATE *update, fnUpdateVariable ***fnUpdate, int iBody) {

  if (fd40KPower(update, iBody) < halt->dMin40KPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 40K Power =  ", body[iBody].cName);
      fprintd(stdout, fd40KPower(update, iBody), io->iSciNot, io->iDigits);
      printf(" < ");
      fprintd(stdout, halt->dMin40KPower, io->iSciNot, io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}
/**
   Halt if minimum power reached.

   @param body Body struct
   @param evolve Evolve struct
   @param halt Halt struct
   @param io IO struct
   @param update Update struct
   @param iBody Index of body
*/
int fbHaltMin232ThPower(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                        UPDATE *update, fnUpdateVariable ***fnUpdate,
                        int iBody) {

  if (fd232ThPower(update, iBody) < halt->dMin232ThPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 232Th Power =  ", body[iBody].cName);
      fprintd(stdout, fd232ThPower(update, iBody), io->iSciNot, io->iDigits);
      printf(" < ");
      fprintd(stdout, halt->dMin232ThPower, io->iSciNot, io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}
/**
   Halt if minimum power reached.

   @param body Body struct
   @param evolve Evolve struct
   @param halt Halt struct
   @param io IO struct
   @param update Update struct
   @param iBody Index of body
*/
int fbHaltMin238UPower(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                       UPDATE *update, fnUpdateVariable ***fnUpdate,
                       int iBody) {

  if (fd238UPower(update, iBody) < halt->dMin238UPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 238U Power =  ", body[iBody].cName);
      fprintd(stdout, fd238UPower(update, iBody), io->iSciNot, io->iDigits);
      printf(" < ");
      fprintd(stdout, halt->dMin238UPower, io->iSciNot, io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}
/**
   Halt if minimum power reached.

   @param body Body struct
   @param evolve Evolve struct
   @param halt Halt struct
   @param io IO struct
   @param update Update struct
   @param iBody Index of body
*/
int fbHaltMin235UPower(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                       UPDATE *update, fnUpdateVariable ***fnUpdate,
                       int iBody) {

  if (fd235UPower(update, iBody) < halt->dMin235UPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 235U Power =  ", body[iBody].cName);
      fprintd(stdout, fd235UPower(update, iBody), io->iSciNot, io->iDigits);
      printf(" < ");
      fprintd(stdout, halt->dMin235UPower, io->iSciNot, io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}
/**
   Halt if minimum power reached.

   @param body Body struct
   @param evolve Evolve struct
   @param halt Halt struct
   @param io IO struct
   @param update Update struct
   @param iBody Index of body
*/
int fbHaltMinRadPower(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                      UPDATE *update, fnUpdateVariable ***fnUpdate, int iBody) {

  if (fdRadPowerTotal(body, iBody) < halt->dMinRadPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's Total Radiogenic Power =  ", body[iBody].cName);
      fprintd(stdout, fdRadPowerTotal(body, iBody), io->iSciNot, io->iDigits);
      printf(" < ");
      fprintd(stdout, halt->dMinRadPower, io->iSciNot, io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}
/**
   Count halts if minimum power reached.

   @param halt Halt struct
   @param iNumHalts Index of number of halts
*/
void fvCountHaltsRadheat(HALT *halt, int *iNumHalts) {
  if (halt->dMin40KPower > 0) {
    (*iNumHalts)++;
  }
  if (halt->dMin232ThPower > 0) {
    (*iNumHalts)++;
  }
  if (halt->dMin238UPower > 0) {
    (*iNumHalts)++;
  }
  if (halt->dMin235UPower > 0) {
    (*iNumHalts)++;
  }
  if (halt->dMinRadPower > 0) {
    (*iNumHalts)++;
  }
}
/**
   Add link to halt function in control struct.

   @param body Body struct
   @param control Control struct
   @param options Options struct
   @param iHalt Index of halt
   @param iBody Index of body
*/
void fvVerifyHaltRadheat(BODY *body, CONTROL *control, OPTIONS *options,
                         int iBody, int *iHalt) {

  if (control->Halt[iBody].dMin40KPower > 0) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin40KPower;
  }
  if (control->Halt[iBody].dMin232ThPower > 0) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin232ThPower;
  }
  if (control->Halt[iBody].dMin238UPower > 0) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin238UPower;
  }
  if (control->Halt[iBody].dMin235UPower > 0) { // PED
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin235UPower;
  }
  if (control->Halt[iBody].dMinRadPower > 0) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMinRadPower;
  }
}

/************* RADHEAT Outputs ******************/

/* NOTE: If you write a new Write subroutine here you need to add the associate
   block of initialization in InitializeOutputRadheat below */

/* Aluminum */
/**
   Write radiogenic 26Al mantle power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite26AlPowerMan(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  /* Get total power from 26Al
   *dTmp = -(*(update[iBody].pdD26AlNumManDt))*ENERGY26Al;  */
  *dTmp = fd26AlPowerMan(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 26Al surface heat flux.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite26AlEnFlux(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  /* Get surface heat flux from 26Al */
  *dTmp = fd26AlEnFlux(body, update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}
/**
   Write radiogenic 26Al mantle DNum/Dt.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWriteD26AlNumDt(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  *dTmp = *(update[iBody].pdD26AlNumManDt);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
/**
   Write radiogenic 26Al decay time?

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite26AlTimescale(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = -1;
}
/**
   Write radiogenic 26Al mantle mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite26AlMassMan(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d26AlNumMan * MASS26AL;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 26Al DPower/Dt.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWriteD26AlPowerDt(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = -1;
}
/**
   Write radiogenic 26Al mantle number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite26AlNumMan(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d26AlNumMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Core */
/**
   Write radiogenic 26Al core power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite26AlPowerCore(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  /* Get total power from 26Al */
  *dTmp = fd26AlPowerCore(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 26Al core mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite26AlMassCore(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d26AlNumCore * MASS26AL;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 26Al core number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite26AlNumCore(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d26AlNumCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/**
   Write total power from 26Al decay.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite26AlPowerTot(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = fd26AlPower(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}


/* Potassium */
/**
   Write radiogenic 40K mantle power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite40KPowerMan(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  /* Get total power from 40K
   *dTmp = -(*(update[iBody].pdD40KNumManDt))*ENERGY40K;  */
  *dTmp = fd40KPowerMan(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 40K surface heat flux.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite40KEnFlux(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  /* Get surface heat flux from 40K */
  *dTmp = fd40KEnFlux(body, update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}
/**
   Write radiogenic 40K mantle DNum/Dt.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWriteD40KNumDt(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = *(update[iBody].pdD40KNumManDt);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
/**
   Write radiogenic 40K decay time?

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite40KTimescale(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = -1;
}
/**
   Write radiogenic 40K mantle mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite40KMassMan(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d40KNumMan * MASS40K;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}

/**
   Write radiogenic 40K DPower/Dt.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWriteD40KPowerDt(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = -1;
}
/**
   Write radiogenic 40K mantle number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite40KNumMan(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d40KNumMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Core */
/**
   Write radiogenic 40K core power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite40KPowerCore(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  /* Get total power from 40K */
  *dTmp = fd40KPowerCore(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 40K core mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite40KMassCore(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d40KNumCore * MASS40K;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 40K core number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite40KNumCore(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d40KNumCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Crust */
/**
   Write radiogenic 40K crust power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite40KPowerCrust(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  /* Get total power from 40K */
  *dTmp = fd40KPowerCrust(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 40K crust mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite40KMassCrust(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d40KNumCrust * MASS40K;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 40K crust number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite40KNumCrust(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d40KNumCrust;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/**
   Write total power from 40K decay.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite40KPowerTot(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = fd40KPower(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

/* Thorium */
/**
   Write radiogenic 232Th surface heat flux.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite232ThEnFlux(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  /* Get surface heat flux from 232Th */
  *dTmp = fd232ThEnFlux(body, update, iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}
/**
   Write radiogenic 232Th mantle DNum/Dt.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWriteD232ThNumDt(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = *(update[iBody].pdD232ThNumManDt);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
/**
   Write radiogenic 232Th DPower/Dt.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWritedD232ThPowerDt(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  *dTmp = -1;
}
/**
   Write radiogenic 232Th decay time?

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite232ThTimescale(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  *dTmp = -1;
}
/**
   Write radiogenic 232Th mantle power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite232ThPowerMan(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  /* Get total heat from 232Th */
  *dTmp = fd232ThPowerMan(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 232Th mantle mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite232ThMassMan(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d232ThNumMan * MASS232TH;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 232Th mantle number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite232ThNumMan(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d232ThNumMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Core */
/**
   Write radiogenic 232Th core power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite232ThPowerCore(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  /* Get total heat from 232Th */
  *dTmp = fd232ThPowerCore(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 232Th core mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite232ThMassCore(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d232ThNumCore * MASS232TH;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 232Th core number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite232ThNumCore(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d232ThNumCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Crust */
/**
   Write radiogenic 232Th crust power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite232ThPowerCrust(BODY *body, CONTROL *control, OUTPUT *output,
                            SYSTEM *system, UNITS *units, UPDATE *update,
                            int iBody, double *dTmp, char cUnit[]) {
  /* Get total heat from 232Th */
  *dTmp = fd232ThPowerCrust(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 232Th crust mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite232ThMassCrust(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d232ThNumCrust * MASS232TH;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 232Th crust number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite232ThNumCrust(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d232ThNumCrust;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/**
   Write total power from 232Th decay.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite232ThPowerTot(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = fd232ThPower(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

/* Uranium 238 */
/**
   Write radiogenic 238U surface heat flux.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite238UEnFlux(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  /* Get surface heat flux from 238U */
  *dTmp = fd238UEnFlux(body, update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}
/**
   Write radiogenic 238U mantle DNum/Dt.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWriteD238UNumDt(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  *dTmp = *(update[iBody].pdD238UNumManDt);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
/**
   Write radiogenic 238U DPower/Dt.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWritedD238UPowerDt(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = -1;
}
/**
   Write radiogenic 238U decay time?

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite238UTimescale(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = -1;
}

/* Mantle */
/**
   Write radiogenic 238U mantle power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite238UPowerMan(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  /* Get total power from 238U */
  *dTmp = fd238UPowerMan(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 238U mantle mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite238UMassMan(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d238UNumMan * MASS238U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 238U mantle number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite238UNumMan(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d238UNumMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Core */
/**
   Write radiogenic 238U core power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite238UPowerCore(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  /* Get total power from 238U */
  *dTmp = fd238UPowerCore(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 238U core mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite238UMassCore(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d238UNumCore * MASS238U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 238U core number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite238UNumCore(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d238UNumCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Crust */
/**
   Write radiogenic 238U crust power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite238UPowerCrust(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  /* Get total power from 238U */
  *dTmp = fd238UPowerCrust(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 238U crust mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite238UMassCrust(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d238UNumCrust * MASS238U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 238U crust number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite238UNumCrust(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d238UNumCrust;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/**
   Write total power from 238U decay.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite238UPowerTot(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = fd238UPower(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}


/* Uranium 235 */
/**
   Write radiogenic 235U surface heat flux.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite235UEnFlux(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  /* Get surface heat flux from 235U */
  *dTmp = fd235UEnFlux(body, update, iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}
/**
   Write radiogenic 235U mantle DNum/Dt.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWriteD235UNumDt(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  *dTmp = *(update[iBody].pdD235UNumManDt);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
/**
   Write radiogenic 235U DPower/Dt.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWritedD235UPowerDt(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = -1;
}
/**
   Write radiogenic 235U decay time?

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite235UTimescale(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = -1;
}

/* Mantle */
/**
   Write radiogenic 235U mantle power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite235UPowerMan(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  /* Get total power from 235U */
  *dTmp = -(*(update[iBody].pdD235UNumManDt)) * ENERGY235U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 235U mantle mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite235UMassMan(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d235UNumMan * MASS235U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 235U mantle number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite235UNumMan(BODY *body, CONTROL *control, OUTPUT *output,
                       SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                       double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d235UNumMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Core */
/**
   Write radiogenic 235U core power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite235UPowerCore(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  /* Get total power from 235U */
  *dTmp = fd235UPowerCore(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 235U core mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite235UMassCore(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d235UNumCore * MASS235U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 235U core number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite235UNumCore(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d235UNumCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Crust */
/**
   Write radiogenic 235U crust power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite235UPowerCrust(BODY *body, CONTROL *control, OUTPUT *output,
                           SYSTEM *system, UNITS *units, UPDATE *update,
                           int iBody, double *dTmp, char cUnit[]) {
  /* Get total power from 235U */
  *dTmp = fd235UPowerCrust(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write radiogenic 235U crust mass.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite235UMassCrust(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d235UNumCrust * MASS235U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass, cUnit);
  }
}
/**
   Write radiogenic 235U crust number.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite235UNumCrust(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].d235UNumCrust;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  }
  /* else nothing, as it is a number */
}

/**
   Write total power from 235U decay.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWrite235UPowerTot(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  *dTmp = fd235UPower(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}


/* Totals */
/**
   Write total radiogenic power in crust.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWriteRadPowerCrust(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  /* Radiogenic Power Production in crust*/
  *dTmp = fdRadPowerCrust(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write total radiogenic power in mantle.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWriteRadPowerMan(BODY *body, CONTROL *control, OUTPUT *output,
                        SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                        double *dTmp, char cUnit[]) {
  /* Radiogenic Power Production in mantle*/
  *dTmp = fdRadPowerMan(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write total radiogenic power in core.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWriteRadPowerCore(BODY *body, CONTROL *control, OUTPUT *output,
                         SYSTEM *system, UNITS *units, UPDATE *update,
                         int iBody, double *dTmp, char cUnit[]) {
  /* Total Radiogenic Power Production in core*/
  *dTmp = fdRadPowerCore(update, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write total radiogenic power.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWriteRadPowerTotal(BODY *body, CONTROL *control, OUTPUT *output,
                          SYSTEM *system, UNITS *units, UPDATE *update,
                          int iBody, double *dTmp, char cUnit[]) {
  /* Total Radiogenic Power Production */
  *dTmp = body[iBody].dRadPowerTotal;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}
/**
   Write total radiogenic surface heat flux.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param units Units struct
   @param update Update struct
   @param iBody Index of body
   @param dTmp Temporary variable
   @param cUnit Units
*/
void fvWriteSurfEnFluxRadTotal(BODY *body, CONTROL *control, OUTPUT *output,
                               SYSTEM *system, UNITS *units, UPDATE *update,
                               int iBody, double *dTmp, char cUnit[]) {
  /* Radiogenic Surface Energy Flux */
  *dTmp = fdSurfEnFluxRadTotal(body, system, update, iBody, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

/**
   Initialize output for each variable: name, description, default units, and
   write function.

   @param output Output struct
   @param fnWrite fnWriteOutput
*/
void fvInitializeOutputRadheat(OUTPUT *output, fnWriteOutput fnWrite[]) {
  /* Aluminum */
  sprintf(output[OUT_26ALPOWERMAN].cName, "26AlPowerMan");
  sprintf(output[OUT_26ALPOWERMAN].cDescr, "Total Power Generated by 26Al");
  sprintf(output[OUT_26ALPOWERMAN].cNeg, "TW");
  output[OUT_26ALPOWERMAN].bNeg       = 1;
  output[OUT_26ALPOWERMAN].dNeg       = 1e-12;
  output[OUT_26ALPOWERMAN].iNum       = 1;
  output[OUT_26ALPOWERMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALPOWERMAN]           = &fvWrite26AlPowerMan;

  sprintf(output[OUT_26ALENFLUX].cName, "26AlEnFlux");
  sprintf(output[OUT_26ALENFLUX].cDescr, "Surface Energy Flux from 26Al");
  sprintf(output[OUT_26ALENFLUX].cNeg, "W/m^2");
  output[OUT_26ALENFLUX].bNeg       = 1;
  output[OUT_26ALENFLUX].dNeg       = 1;
  output[OUT_26ALENFLUX].iNum       = 1;
  output[OUT_26ALENFLUX].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALENFLUX]           = &fvWrite26AlEnFlux;

  sprintf(output[OUT_26ALDPOWERDT].cName, "D26AlPowerDt");
  sprintf(output[OUT_26ALDPOWERDT].cDescr,
          "Time Rate of Change of 26Al Power Generation");
  sprintf(output[OUT_26ALDPOWERDT].cNeg, "TW/Gyr");
  output[OUT_26ALDPOWERDT].bNeg = 1;
  output[OUT_26ALDPOWERDT].dNeg =
        1e9 / 1e12; //[yr/Gyr*TW/W] //1e12/1e9; /* XXX */
  output[OUT_26ALDPOWERDT].iNum       = 1;
  output[OUT_26ALDPOWERDT].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALDPOWERDT]           = &fvWriteD26AlPowerDt;

  sprintf(output[OUT_26ALDNUMDT].cName, "D26AlNumManDt");
  sprintf(output[OUT_26ALDNUMDT].cDescr,
          "Time Rate of Change of 26Al Heat Generation");
  sprintf(output[OUT_26ALDNUMDT].cNeg, "/Gyr");
  output[OUT_26ALDNUMDT].bNeg       = 1;
  output[OUT_26ALDNUMDT].dNeg       = YEARSEC * 1e9; //[s/Gyr]
  output[OUT_26ALDNUMDT].iNum       = 1;
  output[OUT_26ALDNUMDT].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALDNUMDT]           = &fvWriteD26AlNumDt;

  sprintf(output[OUT_26ALTIME].cName, "26AlTimescale");
  sprintf(output[OUT_26ALTIME].cDescr, "Timescale for 26Al Power Generation");
  sprintf(output[OUT_26ALTIME].cNeg, "Gyr");
  output[OUT_26ALTIME].bNeg = 1;
  output[OUT_26ALTIME].dNeg = 1 / (YEARSEC * 1e9); //[Gyr/s] //YEARSEC*1e9;
  output[OUT_26ALTIME].iNum = 1;
  output[OUT_26ALTIME].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALTIME]           = &fvWrite26AlTimescale;

  sprintf(output[OUT_26ALMASSMAN].cName, "26AlMassMan");
  sprintf(output[OUT_26ALMASSMAN].cDescr, "Mass of Mantle in 26Al");
  sprintf(output[OUT_26ALMASSMAN].cNeg, "Mearth");
  output[OUT_26ALMASSMAN].bNeg       = 1;
  output[OUT_26ALMASSMAN].dNeg       = 1. / MEARTH; //[EMass/kg]
  output[OUT_26ALMASSMAN].iNum       = 1;
  output[OUT_26ALMASSMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALMASSMAN]           = &fvWrite26AlMassMan;

  sprintf(output[OUT_26ALNUMMAN].cName, "26AlNumMan");
  sprintf(output[OUT_26ALNUMMAN].cDescr, "Total Number of 26Al Atoms");
  sprintf(output[OUT_26ALNUMMAN].cNeg, "Initial Primordial Earth Number");
  output[OUT_26ALNUMMAN].bNeg       = 1;
  output[OUT_26ALNUMMAN].dNeg       = EMASSMAN26AL / MASS26AL;
  output[OUT_26ALNUMMAN].iNum       = 1;
  output[OUT_26ALNUMMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALNUMMAN]           = &fvWrite26AlNumMan;

  /* Core */
  sprintf(output[OUT_26ALPOWERCORE].cName, "26AlPowerCore");
  sprintf(output[OUT_26ALPOWERCORE].cDescr, "Core Power Generated by 26Al");
  sprintf(output[OUT_26ALPOWERCORE].cNeg, "TW");
  output[OUT_26ALPOWERCORE].bNeg       = 1;
  output[OUT_26ALPOWERCORE].dNeg       = 1 / 1e12; //[TW/W]
  output[OUT_26ALPOWERCORE].iNum       = 1;
  output[OUT_26ALPOWERCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALPOWERCORE]           = &fvWrite26AlPowerCore;

  sprintf(output[OUT_26ALMASSCORE].cName, "26AlMassCore");
  sprintf(output[OUT_26ALMASSCORE].cDescr, "Mass of core in 26Al");
  sprintf(output[OUT_26ALMASSCORE].cNeg, "Mearth");
  output[OUT_26ALMASSCORE].bNeg       = 1;
  output[OUT_26ALMASSCORE].dNeg       = 1. / MEARTH;
  output[OUT_26ALMASSCORE].iNum       = 1;
  output[OUT_26ALMASSCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALMASSCORE]           = &fvWrite26AlMassCore;

  sprintf(output[OUT_26ALNUMCORE].cName, "26AlNumCore");
  sprintf(output[OUT_26ALNUMCORE].cDescr, "Total Core Number of 26Al Atoms");
  sprintf(output[OUT_26ALNUMCORE].cNeg, "Initial Primordial Earth Number");
  output[OUT_26ALNUMCORE].bNeg       = 1;
  output[OUT_26ALNUMCORE].dNeg       = EMASSCORE26AL / MASS26AL;
  output[OUT_26ALNUMCORE].iNum       = 1;
  output[OUT_26ALNUMCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALNUMCORE]           = &fvWrite26AlNumCore;

  sprintf(output[OUT_26ALPOWERTOT].cName, "26AlPowerTotal");
  sprintf(output[OUT_26ALPOWERTOT].cDescr,
          "Total Power from Decay of 26Al Atoms");
  sprintf(output[OUT_26ALPOWERTOT].cNeg, "TW");
  output[OUT_26ALPOWERTOT].bNeg       = 1;
  output[OUT_26ALPOWERTOT].dNeg       = 1e-12;
  output[OUT_26ALPOWERTOT].iNum       = 1;
  output[OUT_26ALPOWERTOT].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALPOWERTOT]           = &fvWrite26AlPowerTot;

  /* Potassium */
  sprintf(output[OUT_40KENFLUX].cName, "40KEnFlux");
  sprintf(output[OUT_40KENFLUX].cDescr, "Surface Energy Flux from 40K");
  sprintf(output[OUT_40KENFLUX].cNeg, "W/m^2");
  output[OUT_40KENFLUX].bNeg       = 1;
  output[OUT_40KENFLUX].dNeg       = 1;
  output[OUT_40KENFLUX].iNum       = 1;
  output[OUT_40KENFLUX].iModuleBit = RADHEAT;
  fnWrite[OUT_40KENFLUX]           = &fvWrite40KEnFlux;

  sprintf(output[OUT_40KDPOWERDT].cName, "D40KPowerDt");
  sprintf(output[OUT_40KDPOWERDT].cDescr,
          "Time Rate of Change of 40K Power Generation");
  sprintf(output[OUT_40KDPOWERDT].cNeg, "TW/Gyr");
  output[OUT_40KDPOWERDT].bNeg       = 1;
  output[OUT_40KDPOWERDT].dNeg       = 1e9 / 1e12; /* :) */
  output[OUT_40KDPOWERDT].iNum       = 1;
  output[OUT_40KDPOWERDT].iModuleBit = RADHEAT;
  fnWrite[OUT_40KDPOWERDT]           = &fvWriteD40KPowerDt;

  sprintf(output[OUT_40KDNUMDT].cName, "D40KNumManDt");
  sprintf(output[OUT_40KDNUMDT].cDescr,
          "Time Rate of Change of 40K Heat Generation");
  sprintf(output[OUT_40KDNUMDT].cNeg, "/Gyr");
  output[OUT_40KDNUMDT].bNeg       = 1;
  output[OUT_40KDNUMDT].dNeg       = YEARSEC * 1e9;
  output[OUT_40KDNUMDT].iNum       = 1;
  output[OUT_40KDNUMDT].iModuleBit = RADHEAT;
  fnWrite[OUT_40KDNUMDT]           = &fvWriteD40KNumDt;

  sprintf(output[OUT_40KTIME].cName, "40KTimescale");
  sprintf(output[OUT_40KTIME].cDescr, "Timescale for 40K Power Generation");
  sprintf(output[OUT_40KTIME].cNeg, "Gyr");
  output[OUT_40KTIME].bNeg       = 1;
  output[OUT_40KTIME].dNeg       = 1 / (YEARSEC * 1e9);
  output[OUT_40KTIME].iNum       = 1;
  output[OUT_40KTIME].iModuleBit = RADHEAT;
  fnWrite[OUT_40KTIME]           = &fvWrite40KTimescale;

  sprintf(output[OUT_40KPOWERMAN].cName, "40KPowerMan");
  sprintf(output[OUT_40KPOWERMAN].cDescr, "Total Power Generated by 40K");
  sprintf(output[OUT_40KPOWERMAN].cNeg, "TW");
  output[OUT_40KPOWERMAN].bNeg       = 1;
  output[OUT_40KPOWERMAN].dNeg       = 1e-12;
  output[OUT_40KPOWERMAN].iNum       = 1;
  output[OUT_40KPOWERMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_40KPOWERMAN]           = &fvWrite40KPowerMan;

  sprintf(output[OUT_40KMASSMAN].cName, "40KMassMan");
  sprintf(output[OUT_40KMASSMAN].cDescr, "Total Mass of 40K");
  sprintf(output[OUT_40KMASSMAN].cNeg, "Mearth");
  output[OUT_40KMASSMAN].bNeg       = 1;
  output[OUT_40KMASSMAN].dNeg       = 1. / MEARTH;
  output[OUT_40KMASSMAN].iNum       = 1;
  output[OUT_40KMASSMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_40KMASSMAN]           = &fvWrite40KMassMan;

  sprintf(output[OUT_40KNUMMAN].cName, "40KNumMan");
  sprintf(output[OUT_40KNUMMAN].cDescr, "Total Number of 40K Atoms");
  sprintf(output[OUT_40KNUMMAN].cNeg, "Initial Primordial Earth Number");
  output[OUT_40KNUMMAN].bNeg       = 1;
  output[OUT_40KNUMMAN].dNeg       = EMASSMAN40K / MASS40K;
  output[OUT_40KNUMMAN].iNum       = 1;
  output[OUT_40KNUMMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_40KNUMMAN]           = &fvWrite40KNumMan;

  /* Core */
  sprintf(output[OUT_40KPOWERCORE].cName, "40KPowerCore");
  sprintf(output[OUT_40KPOWERCORE].cDescr, "Core Power Generated by 40K");
  sprintf(output[OUT_40KPOWERCORE].cNeg, "TW");
  output[OUT_40KPOWERCORE].bNeg       = 1;
  output[OUT_40KPOWERCORE].dNeg       = 1e-12; // ergs/s -> TW
  output[OUT_40KPOWERCORE].iNum       = 1;
  output[OUT_40KPOWERCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_40KPOWERCORE]           = &fvWrite40KPowerCore;

  sprintf(output[OUT_40KMASSCORE].cName, "40KMassCore");
  sprintf(output[OUT_40KMASSCORE].cDescr, "Total Core Mass of 40K");
  sprintf(output[OUT_40KMASSCORE].cNeg, "Mearth");
  output[OUT_40KMASSCORE].bNeg       = 1;
  output[OUT_40KMASSCORE].dNeg       = 1. / MEARTH;
  output[OUT_40KMASSCORE].iNum       = 1;
  output[OUT_40KMASSCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_40KMASSCORE]           = &fvWrite40KMassCore;

  sprintf(output[OUT_40KNUMCORE].cName, "40KNumCore");
  sprintf(output[OUT_40KNUMCORE].cDescr, "Total Core Number of 40K Atoms");
  sprintf(output[OUT_40KNUMCORE].cNeg, "Initial Primordial Earth Number");
  output[OUT_40KNUMCORE].bNeg       = 1;
  output[OUT_40KNUMCORE].dNeg       = EMASSCORE40K / MASS40K;
  output[OUT_40KNUMCORE].iNum       = 1;
  output[OUT_40KNUMCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_40KNUMCORE]           = &fvWrite40KNumCore;

  /* Crust */
  sprintf(output[OUT_40KPOWERCRUST].cName, "40KPowerCrust");
  sprintf(output[OUT_40KPOWERCRUST].cDescr, "Crust Power Generated by 40K");
  sprintf(output[OUT_40KPOWERCRUST].cNeg, "TW");
  output[OUT_40KPOWERCRUST].bNeg       = 1;
  output[OUT_40KPOWERCRUST].dNeg       = 1e-12; // ergs/s -> TW
  output[OUT_40KPOWERCRUST].iNum       = 1;
  output[OUT_40KPOWERCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_40KPOWERCRUST]           = &fvWrite40KPowerCrust;

  sprintf(output[OUT_40KMASSCRUST].cName, "40KMassCrust");
  sprintf(output[OUT_40KMASSCRUST].cDescr, "Total Crust Mass of 40K");
  sprintf(output[OUT_40KMASSCRUST].cNeg, "Mearth");
  output[OUT_40KMASSCRUST].bNeg       = 1;
  output[OUT_40KMASSCRUST].dNeg       = 1. / MEARTH;
  output[OUT_40KMASSCRUST].iNum       = 1;
  output[OUT_40KMASSCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_40KMASSCRUST]           = &fvWrite40KMassCrust;

  sprintf(output[OUT_40KNUMCRUST].cName, "40KNumCrust");
  sprintf(output[OUT_40KNUMCRUST].cDescr, "Total Crust Number of 40K Atoms");
  sprintf(output[OUT_40KNUMCRUST].cNeg, "Initial Primordial Earth Number");
  output[OUT_40KNUMCRUST].bNeg       = 1;
  output[OUT_40KNUMCRUST].dNeg       = EMASSCRUST40K / MASS40K;
  output[OUT_40KNUMCRUST].iNum       = 1;
  output[OUT_40KNUMCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_40KNUMCRUST]           = &fvWrite40KNumCrust;

  sprintf(output[OUT_40KPOWERTOT].cName, "40KPowerTotal");
  sprintf(output[OUT_40KPOWERTOT].cDescr,
          "Total Power from Decay of 40K Atoms");
  sprintf(output[OUT_40KPOWERTOT].cNeg, "TW");
  output[OUT_40KPOWERTOT].bNeg       = 1;
  output[OUT_40KPOWERTOT].dNeg       = 1e-12;
  output[OUT_40KPOWERTOT].iNum       = 1;
  output[OUT_40KPOWERTOT].iModuleBit = RADHEAT;
  fnWrite[OUT_40KPOWERTOT]           = &fvWrite40KPowerTot;

  /* Thorium */
  sprintf(output[OUT_232THPOWERMAN].cName, "232ThPowerMan");
  sprintf(output[OUT_232THPOWERMAN].cDescr, "Total Power Generated by 232Th");
  sprintf(output[OUT_232THPOWERMAN].cNeg, "TW");
  output[OUT_232THPOWERMAN].bNeg       = 1;
  output[OUT_232THPOWERMAN].dNeg       = 1e-12;
  output[OUT_232THPOWERMAN].iNum       = 1;
  output[OUT_232THPOWERMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_232THPOWERMAN]           = &fvWrite232ThPowerMan;

  sprintf(output[OUT_232THENFLUX].cName, "232ThEnFlux");
  sprintf(output[OUT_232THENFLUX].cDescr, "Surface Energy Flux from 232Th");
  sprintf(output[OUT_232THENFLUX].cNeg, "W/m^2");
  output[OUT_232THENFLUX].bNeg       = 1;
  output[OUT_232THENFLUX].dNeg       = 1;
  output[OUT_232THENFLUX].iNum       = 1;
  output[OUT_232THENFLUX].iModuleBit = RADHEAT;
  fnWrite[OUT_232THENFLUX]           = &fvWrite232ThEnFlux;

  sprintf(output[OUT_232THDNUMDT].cName, "D232ThNumManDt");
  sprintf(output[OUT_232THDNUMDT].cDescr,
          "Time Rate of Change of the number of 232Th nuclei");
  sprintf(output[OUT_232THDNUMDT].cNeg, "/Gyr");
  output[OUT_232THDNUMDT].bNeg       = 1;
  output[OUT_232THDNUMDT].dNeg       = YEARSEC * 1e9;
  output[OUT_232THDNUMDT].iNum       = 1;
  output[OUT_232THDNUMDT].iModuleBit = RADHEAT;
  fnWrite[OUT_232THDNUMDT]           = &fvWriteD232ThNumDt;

  sprintf(output[OUT_232THTIME].cName, "232ThTimescale");
  sprintf(output[OUT_232THTIME].cDescr, "Timescale for 232Th Power Generation");
  sprintf(output[OUT_232THTIME].cNeg, "Gyr");
  output[OUT_232THTIME].bNeg       = 1;
  output[OUT_232THTIME].dNeg       = 1. / (YEARSEC * 1e9);
  output[OUT_232THTIME].iNum       = 1;
  output[OUT_232THTIME].iModuleBit = RADHEAT;
  fnWrite[OUT_232THTIME]           = &fvWrite232ThTimescale;

  sprintf(output[OUT_232THMASSMAN].cName, "232ThMassMan");
  sprintf(output[OUT_232THMASSMAN].cDescr, "Total Mass of 232Th");
  sprintf(output[OUT_232THMASSMAN].cNeg, "Mearth");
  output[OUT_232THMASSMAN].bNeg       = 1;
  output[OUT_232THMASSMAN].dNeg       = 1. / MEARTH;
  output[OUT_232THMASSMAN].iNum       = 1;
  output[OUT_232THMASSMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_232THMASSMAN]           = &fvWrite232ThMassMan;

  sprintf(output[OUT_232THNUMMAN].cName, "232ThNumMan");
  sprintf(output[OUT_232THNUMMAN].cDescr, "Total Number of 232Th Atoms");
  sprintf(output[OUT_232THNUMMAN].cNeg, "Initial Primordial Earth Number");
  output[OUT_232THNUMMAN].bNeg       = 1;
  output[OUT_232THNUMMAN].dNeg       = EMASSMAN232TH / MASS232TH;
  output[OUT_232THNUMMAN].iNum       = 1;
  output[OUT_232THNUMMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_232THNUMMAN]           = &fvWrite232ThNumMan;

  /* Core */
  sprintf(output[OUT_232THPOWERCORE].cName, "232ThPowerCore");
  sprintf(output[OUT_232THPOWERCORE].cDescr, "Total Power Generated by 232Th");
  sprintf(output[OUT_232THPOWERCORE].cNeg, "TW");
  output[OUT_232THPOWERCORE].bNeg       = 1;
  output[OUT_232THPOWERCORE].dNeg       = 1e-12;
  output[OUT_232THPOWERCORE].iNum       = 1;
  output[OUT_232THPOWERCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_232THPOWERCORE]           = &fvWrite232ThPowerCore;

  sprintf(output[OUT_232THMASSCORE].cName, "232ThMassCore");
  sprintf(output[OUT_232THMASSCORE].cDescr, "Total Mass of 232Th");
  sprintf(output[OUT_232THMASSCORE].cNeg, "Mearth");
  output[OUT_232THMASSCORE].bNeg       = 1;
  output[OUT_232THMASSCORE].dNeg       = 1. / MEARTH;
  output[OUT_232THMASSCORE].iNum       = 1;
  output[OUT_232THMASSCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_232THMASSCORE]           = &fvWrite232ThMassCore;

  sprintf(output[OUT_232THNUMCORE].cName, "232ThNumCore");
  sprintf(output[OUT_232THNUMCORE].cDescr, "Total Number of 232Th Atoms");
  sprintf(output[OUT_232THNUMCORE].cNeg, "Initial Primordial Earth Number");
  output[OUT_232THNUMCORE].bNeg       = 1;
  output[OUT_232THNUMCORE].dNeg       = EMASSCORE232TH / MASS232TH;
  output[OUT_232THNUMCORE].iNum       = 1;
  output[OUT_232THNUMCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_232THNUMCORE]           = &fvWrite232ThNumCore;

  /* Crust */
  sprintf(output[OUT_232THPOWERCRUST].cName, "232ThPowerCrust");
  sprintf(output[OUT_232THPOWERCRUST].cDescr, "Total Power Generated by 232Th");
  sprintf(output[OUT_232THPOWERCRUST].cNeg, "TW");
  output[OUT_232THPOWERCRUST].bNeg       = 1;
  output[OUT_232THPOWERCRUST].dNeg       = 1e-12;
  output[OUT_232THPOWERCRUST].iNum       = 1;
  output[OUT_232THPOWERCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_232THPOWERCRUST]           = &fvWrite232ThPowerCrust;

  sprintf(output[OUT_232THMASSCRUST].cName, "232ThMassCrust");
  sprintf(output[OUT_232THMASSCRUST].cDescr, "Total Mass of 232Th");
  sprintf(output[OUT_232THMASSCRUST].cNeg, "Mearth");
  output[OUT_232THMASSCRUST].bNeg       = 1;
  output[OUT_232THMASSCRUST].dNeg       = 1. / MEARTH;
  output[OUT_232THMASSCRUST].iNum       = 1;
  output[OUT_232THMASSCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_232THMASSCRUST]           = &fvWrite232ThMassCrust;

  sprintf(output[OUT_232THNUMCRUST].cName, "232ThNumCrust");
  sprintf(output[OUT_232THNUMCRUST].cDescr, "Total Number of 232Th Atoms");
  sprintf(output[OUT_232THNUMCRUST].cNeg, "Initial Primordial Earth Number");
  output[OUT_232THNUMCRUST].bNeg       = 1;
  output[OUT_232THNUMCRUST].dNeg       = EMASSCRUST232TH / MASS232TH;
  output[OUT_232THNUMCRUST].iNum       = 1;
  output[OUT_232THNUMCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_232THNUMCRUST]           = &fvWrite232ThNumCrust;

  sprintf(output[OUT_232THPOWERTOT].cName, "232ThPowerTotal");
  sprintf(output[OUT_232THPOWERTOT].cDescr,
          "Total Power from Decay of 232Th Atoms");
  sprintf(output[OUT_232THPOWERTOT].cNeg, "TW");
  output[OUT_232THPOWERTOT].bNeg       = 1;
  output[OUT_232THPOWERTOT].dNeg       = 1e-12;
  output[OUT_232THPOWERTOT].iNum       = 1;
  output[OUT_232THPOWERTOT].iModuleBit = RADHEAT;
  fnWrite[OUT_232THPOWERTOT]           = &fvWrite232ThPowerTot;


  /* Uranium 238 */

  sprintf(output[OUT_238UPOWERMAN].cName, "238UPowerMan");
  sprintf(output[OUT_238UPOWERMAN].cDescr, "Total Power Generated by 238U");
  sprintf(output[OUT_238UPOWERMAN].cNeg, "TW");
  output[OUT_238UPOWERMAN].bNeg       = 1;
  output[OUT_238UPOWERMAN].dNeg       = 1e-12;
  output[OUT_238UPOWERMAN].iNum       = 1;
  output[OUT_238UPOWERMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_238UPOWERMAN]           = &fvWrite238UPowerMan;

  sprintf(output[OUT_238UENFLUX].cName, "238UEnFlux");
  sprintf(output[OUT_238UENFLUX].cDescr, "Surface Energy Flux due to 238U");
  sprintf(output[OUT_238UENFLUX].cNeg, "W/m^2");
  output[OUT_238UENFLUX].bNeg       = 1;
  output[OUT_238UENFLUX].dNeg       = 1;
  output[OUT_238UENFLUX].iNum       = 1;
  output[OUT_238UENFLUX].iModuleBit = RADHEAT;
  fnWrite[OUT_238UENFLUX]           = &fvWrite238UEnFlux;

  sprintf(output[OUT_238UDNUMDT].cName, "D238UNumManDt");
  sprintf(output[OUT_238UDNUMDT].cDescr,
          "Time Rate of Change of the Number of 238U Nuclei");
  sprintf(output[OUT_238UDNUMDT].cNeg, "/Gyr");
  output[OUT_238UDNUMDT].bNeg       = 1;
  output[OUT_238UDNUMDT].dNeg       = YEARSEC * 1e9;
  output[OUT_238UDNUMDT].iNum       = 1;
  output[OUT_238UDNUMDT].iModuleBit = RADHEAT;
  fnWrite[OUT_238UDNUMDT]           = &fvWriteD238UNumDt;

  sprintf(output[OUT_238UTIME].cName, "238UTimescale");
  sprintf(output[OUT_238UTIME].cDescr, "Timescale for 238U Power Generation");
  sprintf(output[OUT_238UTIME].cNeg, "Gyr");
  output[OUT_238UTIME].bNeg       = 1;
  output[OUT_238UTIME].dNeg       = 1. / (YEARSEC * 1e9);
  output[OUT_238UTIME].iNum       = 1;
  output[OUT_238UTIME].iModuleBit = RADHEAT;
  fnWrite[OUT_238UTIME]           = &fvWrite238UTimescale;

  sprintf(output[OUT_238UMASSMAN].cName, "238UMassMan");
  sprintf(output[OUT_238UMASSMAN].cDescr, "Total Mass of 238U");
  sprintf(output[OUT_238UMASSMAN].cNeg, "Mearth");
  output[OUT_238UMASSMAN].bNeg       = 1;
  output[OUT_238UMASSMAN].dNeg       = 1. / MEARTH;
  output[OUT_238UMASSMAN].iNum       = 1;
  output[OUT_238UMASSMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_238UMASSMAN]           = &fvWrite238UMassMan;

  sprintf(output[OUT_238UNUMMAN].cName, "238UNumMan");
  sprintf(output[OUT_238UNUMMAN].cDescr, "Total Number of 238U Atoms");
  sprintf(output[OUT_238UNUMMAN].cNeg, "Initial Primordial Earth Number");
  output[OUT_238UNUMMAN].bNeg       = 1;
  output[OUT_238UNUMMAN].dNeg       = EMASSMAN238U / MASS238U;
  output[OUT_238UNUMMAN].iNum       = 1;
  output[OUT_238UNUMMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_238UNUMMAN]           = &fvWrite238UNumMan;

  /* Core */
  sprintf(output[OUT_238UPOWERCORE].cName, "238UPowerCore");
  sprintf(output[OUT_238UPOWERCORE].cDescr,
          "Total Core Power Generated by 238U");
  sprintf(output[OUT_238UPOWERCORE].cNeg, "TW");
  output[OUT_238UPOWERCORE].bNeg       = 1;
  output[OUT_238UPOWERCORE].dNeg       = 1e-12;
  output[OUT_238UPOWERCORE].iNum       = 1;
  output[OUT_238UPOWERCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_238UPOWERCORE]           = &fvWrite238UPowerCore;

  sprintf(output[OUT_238UMASSCORE].cName, "238UMassCore");
  sprintf(output[OUT_238UMASSCORE].cDescr, "Total Core Mass of 238U");
  sprintf(output[OUT_238UMASSCORE].cNeg, "Mearth");
  output[OUT_238UMASSCORE].bNeg       = 1;
  output[OUT_238UMASSCORE].dNeg       = 1. / MEARTH;
  output[OUT_238UMASSCORE].iNum       = 1;
  output[OUT_238UMASSCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_238UMASSCORE]           = &fvWrite238UMassCore;

  sprintf(output[OUT_238UNUMCORE].cName, "238UNumCore");
  sprintf(output[OUT_238UNUMCORE].cDescr, "Total Core Number of 238U Atoms");
  sprintf(output[OUT_238UNUMCORE].cNeg, "Initial Primordial Earth Number");
  output[OUT_238UNUMCORE].bNeg       = 1;
  output[OUT_238UNUMCORE].dNeg       = EMASSCORE238U / MASS238U;
  output[OUT_238UNUMCORE].iNum       = 1;
  output[OUT_238UNUMCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_238UNUMCORE]           = &fvWrite238UNumCore;

  /* Crust */
  sprintf(output[OUT_238UPOWERCRUST].cName, "238UPowerCrust");
  sprintf(output[OUT_238UPOWERCRUST].cDescr,
          "Total Crust Power Generated by 238U");
  sprintf(output[OUT_238UPOWERCRUST].cNeg, "TW");
  output[OUT_238UPOWERCRUST].bNeg       = 1;
  output[OUT_238UPOWERCRUST].dNeg       = 1e-12;
  output[OUT_238UPOWERCRUST].iNum       = 1;
  output[OUT_238UPOWERCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_238UPOWERCRUST]           = &fvWrite238UPowerCrust;

  sprintf(output[OUT_238UMASSCRUST].cName, "238UMassCrust");
  sprintf(output[OUT_238UMASSCRUST].cDescr, "Total Crust Mass of 238U");
  sprintf(output[OUT_238UMASSCRUST].cNeg, "Mearth");
  output[OUT_238UMASSCRUST].bNeg       = 1;
  output[OUT_238UMASSCRUST].dNeg       = 1. / MEARTH;
  output[OUT_238UMASSCRUST].iNum       = 1;
  output[OUT_238UMASSCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_238UMASSCRUST]           = &fvWrite238UMassCrust;

  sprintf(output[OUT_238UNUMCRUST].cName, "238UNumCrust");
  sprintf(output[OUT_238UNUMCRUST].cDescr, "Total Crust Number of 238U Atoms");
  sprintf(output[OUT_238UNUMCRUST].cNeg, "Initial Primordial Earth Number");
  output[OUT_238UNUMCRUST].bNeg       = 1;
  output[OUT_238UNUMCRUST].dNeg       = EMASSCRUST238U / MASS238U;
  output[OUT_238UNUMCRUST].iNum       = 1;
  output[OUT_238UNUMCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_238UNUMCRUST]           = &fvWrite238UNumCrust;

  sprintf(output[OUT_238UPOWERTOT].cName, "238UPowerTotal");
  sprintf(output[OUT_238UPOWERTOT].cDescr,
          "Total Power from Decay of 238U Atoms");
  sprintf(output[OUT_238UPOWERTOT].cNeg, "TW");
  output[OUT_238UPOWERTOT].bNeg       = 1;
  output[OUT_238UPOWERTOT].dNeg       = 1e-12;
  output[OUT_238UPOWERTOT].iNum       = 1;
  output[OUT_238UPOWERTOT].iModuleBit = RADHEAT;
  fnWrite[OUT_238UPOWERTOT]           = &fvWrite238UPowerTot;


  /* Uranium 235 */

  sprintf(output[OUT_235UPOWERMAN].cName, "235UPowerMan");
  sprintf(output[OUT_235UPOWERMAN].cDescr, "Total Power Generated by 235U");
  sprintf(output[OUT_235UPOWERMAN].cNeg, "TW");
  output[OUT_235UPOWERMAN].bNeg       = 1;
  output[OUT_235UPOWERMAN].dNeg       = 1e-12;
  output[OUT_235UPOWERMAN].iNum       = 1;
  output[OUT_235UPOWERMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_235UPOWERMAN]           = &fvWrite235UPowerMan;

  sprintf(output[OUT_235UENFLUX].cName, "235UEnFlux");
  sprintf(output[OUT_235UENFLUX].cDescr, "Surface Energy Flux due to 235U");
  sprintf(output[OUT_235UENFLUX].cNeg, "W/m^2");
  output[OUT_235UENFLUX].bNeg       = 1;
  output[OUT_235UENFLUX].dNeg       = 1;
  output[OUT_235UENFLUX].iNum       = 1;
  output[OUT_235UENFLUX].iModuleBit = RADHEAT;
  fnWrite[OUT_235UENFLUX]           = &fvWrite235UEnFlux;

  sprintf(output[OUT_235UDNUMDT].cName, "D235UNumManDt");
  sprintf(output[OUT_235UDNUMDT].cDescr,
          "Time Rate of Change of the Number of 235U Nuclei");
  sprintf(output[OUT_235UDNUMDT].cNeg, "/Gyr");
  output[OUT_235UDNUMDT].bNeg       = 1;
  output[OUT_235UDNUMDT].dNeg       = YEARSEC * 1e9;
  output[OUT_235UDNUMDT].iNum       = 1;
  output[OUT_235UDNUMDT].iModuleBit = RADHEAT;
  fnWrite[OUT_235UDNUMDT]           = &fvWriteD235UNumDt;

  sprintf(output[OUT_235UTIME].cName, "235UTimescale");
  sprintf(output[OUT_235UTIME].cDescr, "Timescale for 235U Power Generation");
  sprintf(output[OUT_235UTIME].cNeg, "Gyr");
  output[OUT_235UTIME].bNeg       = 1;
  output[OUT_235UTIME].dNeg       = 1. / (YEARSEC * 1e9);
  output[OUT_235UTIME].iNum       = 1;
  output[OUT_235UTIME].iModuleBit = RADHEAT;
  fnWrite[OUT_235UTIME]           = &fvWrite235UTimescale;

  sprintf(output[OUT_235UMASSMAN].cName, "235UMassMan");
  sprintf(output[OUT_235UMASSMAN].cDescr, "Total Mass of 235U");
  sprintf(output[OUT_235UMASSMAN].cNeg, "Mearth");
  output[OUT_235UMASSMAN].bNeg       = 1;
  output[OUT_235UMASSMAN].dNeg       = 1. / MEARTH;
  output[OUT_235UMASSMAN].iNum       = 1;
  output[OUT_235UMASSMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_235UMASSMAN]           = &fvWrite235UMassMan;

  sprintf(output[OUT_235UNUMMAN].cName, "235UNumMan");
  sprintf(output[OUT_235UNUMMAN].cDescr, "Total Number of 235U Atoms");
  sprintf(output[OUT_235UNUMMAN].cNeg, "Initial Primordial Earth Number");
  output[OUT_235UNUMMAN].bNeg       = 1;
  output[OUT_235UNUMMAN].dNeg       = EMASSMAN235U / MASS235U;
  output[OUT_235UNUMMAN].iNum       = 1;
  output[OUT_235UNUMMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_235UNUMMAN]           = &fvWrite235UNumMan;

  /* Core */
  sprintf(output[OUT_235UPOWERCORE].cName, "235UPowerCore");
  sprintf(output[OUT_235UPOWERCORE].cDescr,
          "Total Core Power Generated by 235U");
  sprintf(output[OUT_235UPOWERCORE].cNeg, "TW");
  output[OUT_235UPOWERCORE].bNeg       = 1;
  output[OUT_235UPOWERCORE].dNeg       = 1e-12;
  output[OUT_235UPOWERCORE].iNum       = 1;
  output[OUT_235UPOWERCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_235UPOWERCORE]           = &fvWrite235UPowerCore;

  sprintf(output[OUT_235UMASSCORE].cName, "235UMassCore");
  sprintf(output[OUT_235UMASSCORE].cDescr, "Total Core Mass of 235U");
  sprintf(output[OUT_235UMASSCORE].cNeg, "Mearth");
  output[OUT_235UMASSCORE].bNeg       = 1;
  output[OUT_235UMASSCORE].dNeg       = 1. / MEARTH;
  output[OUT_235UMASSCORE].iNum       = 1;
  output[OUT_235UMASSCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_235UMASSCORE]           = &fvWrite235UMassCore;

  sprintf(output[OUT_235UNUMCORE].cName, "235UNumCore");
  sprintf(output[OUT_235UNUMCORE].cDescr, "Total Core Number of 235U Atoms");
  sprintf(output[OUT_235UNUMCORE].cNeg, "Initial Primordial Earth Number");
  output[OUT_235UNUMCORE].bNeg       = 1;
  output[OUT_235UNUMCORE].dNeg       = EMASSCORE235U / MASS235U;
  output[OUT_235UNUMCORE].iNum       = 1;
  output[OUT_235UNUMCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_235UNUMCORE]           = &fvWrite235UNumCore;

  /* Crust */
  sprintf(output[OUT_235UPOWERCRUST].cName, "235UPowerCrust");
  sprintf(output[OUT_235UPOWERCRUST].cDescr,
          "Total Crust Power Generated by 235U");
  sprintf(output[OUT_235UPOWERCRUST].cNeg, "TW");
  output[OUT_235UPOWERCRUST].bNeg       = 1;
  output[OUT_235UPOWERCRUST].dNeg       = 1e-12;
  output[OUT_235UPOWERCRUST].iNum       = 1;
  output[OUT_235UPOWERCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_235UPOWERCRUST]           = &fvWrite235UPowerCrust;

  sprintf(output[OUT_235UMASSCRUST].cName, "235UMassCrust");
  sprintf(output[OUT_235UMASSCRUST].cDescr, "Total Crust Mass of 235U");
  sprintf(output[OUT_235UMASSCRUST].cNeg, "Mearth");
  output[OUT_235UMASSCRUST].bNeg       = 1;
  output[OUT_235UMASSCRUST].dNeg       = 1. / MEARTH;
  output[OUT_235UMASSCRUST].iNum       = 1;
  output[OUT_235UMASSCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_235UMASSCRUST]           = &fvWrite235UMassCrust;

  sprintf(output[OUT_235UNUMCRUST].cName, "235UNumCrust");
  sprintf(output[OUT_235UNUMCRUST].cDescr, "Total Crust Number of 235U Atoms");
  sprintf(output[OUT_235UNUMCRUST].cNeg, "Initial Primordial Earth Number");
  output[OUT_235UNUMCRUST].bNeg       = 1;
  output[OUT_235UNUMCRUST].dNeg       = EMASSCRUST235U / MASS235U;
  output[OUT_235UNUMCRUST].iNum       = 1;
  output[OUT_235UNUMCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_235UNUMCRUST]           = &fvWrite235UNumCrust;

  sprintf(output[OUT_235UPOWERTOT].cName, "235UPowerTotal");
  sprintf(output[OUT_235UPOWERTOT].cDescr,
          "Total Power from Decay of 235U Atoms");
  sprintf(output[OUT_235UPOWERTOT].cNeg, "TW");
  output[OUT_235UPOWERTOT].bNeg       = 1;
  output[OUT_235UPOWERTOT].dNeg       = 1e-12;
  output[OUT_235UPOWERTOT].iNum       = 1;
  output[OUT_235UPOWERTOT].iModuleBit = RADHEAT;
  fnWrite[OUT_235UPOWERTOT]           = &fvWrite235UPowerTot;


  /* Totals */
  sprintf(output[OUT_RADPOWERCRUST].cName, "RadPowerCrust");
  sprintf(output[OUT_RADPOWERCRUST].cDescr,
          "Total Power Generated by Radiogenic Nuclides in the crust");
  sprintf(output[OUT_RADPOWERCRUST].cNeg, "TW");
  output[OUT_RADPOWERCRUST].bNeg       = 1;
  output[OUT_RADPOWERCRUST].dNeg       = 1e-12;
  output[OUT_RADPOWERCRUST].iNum       = 1;
  output[OUT_RADPOWERCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_RADPOWERCRUST]           = &fvWriteRadPowerCrust;

  sprintf(output[OUT_RADPOWERMAN].cName, "RadPowerMan");
  sprintf(output[OUT_RADPOWERMAN].cDescr,
          "Total Power Generated by Radiogenic Nuclides in the mantle");
  sprintf(output[OUT_RADPOWERMAN].cNeg, "TW");
  output[OUT_RADPOWERMAN].bNeg       = 1;
  output[OUT_RADPOWERMAN].dNeg       = 1e-12;
  output[OUT_RADPOWERMAN].iNum       = 1;
  output[OUT_RADPOWERMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_RADPOWERMAN]           = &fvWriteRadPowerMan;

  sprintf(output[OUT_RADPOWERCORE].cName, "RadPowerCore");
  sprintf(output[OUT_RADPOWERCORE].cDescr,
          "Total Power Generated by Radiogenic Nuclides in Core");
  sprintf(output[OUT_RADPOWERCORE].cNeg, "TW");
  output[OUT_RADPOWERCORE].bNeg       = 1;
  output[OUT_RADPOWERCORE].dNeg       = 1e-12;
  output[OUT_RADPOWERCORE].iNum       = 1;
  output[OUT_RADPOWERCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_RADPOWERCORE]           = &fvWriteRadPowerCore;

  sprintf(output[OUT_RADPOWERTOTAL].cName, "RadPowerTotal");
  sprintf(output[OUT_RADPOWERTOTAL].cDescr,
          "Total Power Generated by Radiogenic Nuclides in Total (M+C)");
  sprintf(output[OUT_RADPOWERTOTAL].cNeg, "TW");
  output[OUT_RADPOWERTOTAL].bNeg       = 1;
  output[OUT_RADPOWERTOTAL].dNeg       = 1e-12;
  output[OUT_RADPOWERTOTAL].iNum       = 1;
  output[OUT_RADPOWERTOTAL].iModuleBit = RADHEAT;
  fnWrite[OUT_RADPOWERTOTAL]           = &fvWriteRadPowerTotal;

  sprintf(output[OUT_SURFENFLUXRADTOTAL].cName, "SurfEnFluxRadTotal");
  sprintf(output[OUT_SURFENFLUXRADTOTAL].cDescr,
          "Total Surface Heat Flux Generated by Radiogenic Nuclides");
  sprintf(output[OUT_SURFENFLUXRADTOTAL].cNeg, "W/m^2");
  output[OUT_SURFENFLUXRADTOTAL].bNeg       = 1;
  output[OUT_SURFENFLUXRADTOTAL].dNeg       = 1;
  output[OUT_SURFENFLUXRADTOTAL].iNum       = 1;
  output[OUT_SURFENFLUXRADTOTAL].iModuleBit = RADHEAT;
  fnWrite[OUT_SURFENFLUXRADTOTAL]           = &fvWriteSurfEnFluxRadTotal;
}

/************ RADHEAT Logging Functions **************/
/**
   Print radheat options to log.

   @param control Control struct
   @param fp File file pointer
*/
void fvLogOptionsRadheat(CONTROL *control, FILE *fp) {
  /* Anything here?
  fprintf(fp,"-------- RADHEAT Options -----\n\n");
  */
}

/**
   Print radheat parameters to log.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param update Update struct
   @param fnWrite fnWriteOutput function to write output
   @param fp File file pointer
*/
void fvLogRadheat(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
                  UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {
  // PED: this would be for global rad heat parameters, but this is blank bc rad
  // is only relevant to each individual body.
  /* Anything here?
  int iOut;
  fprintf(fp,"\n----- RADHEAT PARAMETERS ------\n");
  for (iOut=OUTSTARTRADHEAT;iOut<OUTBODYSTARTRADHEAT;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}
/**
   Print radheat parameters of body to log.

   @param body Body struct
   @param control Control struct
   @param output Output struct
   @param system System struct
   @param update Update struct
   @param fnWrite fnWriteOutput function to write output
   @param fp File file pointer
   @param iBody Index of body
*/
void fvLogBodyRadheat(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UPDATE *update, fnWriteOutput fnWrite[],
                      FILE *fp, int iBody) {
  int iOut;

  fprintf(fp, "----- RADHEAT PARAMETERS (%s)------\n", body[iBody].cName);
  for (iOut = OUTSTARTRADHEAT; iOut < OUTENDRADHEAT; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
    /*
    fprintf(fp,"40K Constant: ");
    fprintd(fp,body[iBody].d40KConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");

    fprintf(fp,"232Th Constant: ");
    fprintd(fp,body[iBody].d232ThConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");

    fprintf(fp,"238U Constant: ");
    fprintd(fp,body[iBody].d238UConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");
    */
  }
}
/**
   Add radheat module to module struct.

   @param module Module struct
   @param iModule Index of module
   @param iBody Index of body
*/
void fvAddModuleRadheat(CONTROL *control, MODULE *module, int iBody,
                        int iModule) {

  module->iaModule[iBody][iModule] = RADHEAT;

  // control->fnVerifyImK2[iBody][iModule]                 = &VerifyImK2Radheat;

  module->fnInitializeControl[iBody][iModule] = &fvInitializeControlRadheat;
  module->fnInitializeUpdateTmpBody[iBody][iModule] =
        &fvInitializeUpdateTmpBodyRadheat;
  module->fnCountHalts[iBody][iModule] = &fvCountHaltsRadheat;
  module->fnLogBody[iBody][iModule]    = &fvLogBodyRadheat;

  module->fnReadOptions[iBody][iModule]       = &fvReadOptionsRadheat;
  module->fnVerify[iBody][iModule]            = &fvVerifyRadheat;
  module->fnAssignDerivatives[iBody][iModule] = &fvAssignRadheatDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &fvNullRadheatDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &fvVerifyHaltRadheat;

  module->fnInitializeBody[iBody][iModule]   = &fvInitializeBodyRadheat;
  module->fnInitializeUpdate[iBody][iModule] = &fvInitializeUpdateRadheat;
  module->fnInitializeOutput[iBody][iModule] = &fvInitializeOutputRadheat;

  module->fnFinalizeUpdate26AlNumMan[iBody][iModule] =
        &fvFinalizeUpdate26AlNumManRadheat;
  module->fnFinalizeUpdate40KNumMan[iBody][iModule] =
        &fvFinalizeUpdate40KNumManRadheat;
  module->fnFinalizeUpdate232ThNumMan[iBody][iModule] =
        &fvFinalizeUpdate232ThNumManRadheat;
  module->fnFinalizeUpdate238UNumMan[iBody][iModule] =
        &fvFinalizeUpdate238UNumManRadheat;
  module->fnFinalizeUpdate235UNumMan[iBody][iModule] =
        &fvFinalizeUpdate235UNumManRadheat;

  module->fnFinalizeUpdate26AlNumCore[iBody][iModule] =
        &fvFinalizeUpdate26AlNumCoreRadheat;
  module->fnFinalizeUpdate40KNumCore[iBody][iModule] =
        &fvFinalizeUpdate40KNumCoreRadheat;
  module->fnFinalizeUpdate232ThNumCore[iBody][iModule] =
        &fvFinalizeUpdate232ThNumCoreRadheat;
  module->fnFinalizeUpdate238UNumCore[iBody][iModule] =
        &fvFinalizeUpdate238UNumCoreRadheat;
  module->fnFinalizeUpdate235UNumCore[iBody][iModule] =
        &fvFinalizeUpdate235UNumCoreRadheat;

  module->fnFinalizeUpdate40KNumCrust[iBody][iModule] =
        &fvFinalizeUpdate40KNumCrustRadheat;
  module->fnFinalizeUpdate232ThNumCrust[iBody][iModule] =
        &fvFinalizeUpdate232ThNumCrustRadheat;
  module->fnFinalizeUpdate238UNumCrust[iBody][iModule] =
        &fvFinalizeUpdate238UNumCrustRadheat;
  module->fnFinalizeUpdate235UNumCrust[iBody][iModule] =
        &fvFinalizeUpdate235UNumCrustRadheat;
}

/************* RADHEAT Functions ************/
// FIRST batch of subroutines are general for any species.
// N = N_0 * exp(-t/lambda)
// dN/dt = -(N_0/lambda) * exp(-t/lambda)

/**
   Total mantle radiogenic power.

   @param update Update struct
   @param iBody Index of body

   @return Total mantle radiogenic power
*/
double fdRadPowerMan(UPDATE *update, int iBody) {
  return fd26AlPowerMan(update, iBody) + fd40KPowerMan(update, iBody) +
         fd232ThPowerMan(update, iBody) + fd238UPowerMan(update, iBody) +
         fd235UPowerMan(update, iBody);
}
/**
   Total core radiogenic power.

   @param update Update struct
   @param iBody Index of body

   @return Total core radiogenic power
*/
double fdRadPowerCore(UPDATE *update, int iBody) {
  return fd26AlPowerCore(update, iBody) + fd40KPowerCore(update, iBody) +
         fd232ThPowerCore(update, iBody) + fd238UPowerCore(update, iBody) +
         fd235UPowerCore(update, iBody);
}
/**
   Total crust radiogenic power.

   @param update Update struct
   @param iBody Index of body

   @return Total crust radiogenic power
*/
double fdRadPowerCrust(UPDATE *update, int iBody) {
  return fd40KPowerCrust(update, iBody) + fd232ThPowerCrust(update, iBody) +
         fd238UPowerCrust(update, iBody) + fd235UPowerCrust(update, iBody);
}
/**
   Total body radiogenic power.

   @param body Body struct
   @param iBody Index of body

   @return Total body radiogenic power
*/
double fdRadPowerTotal(BODY *body, int iBody) {
  return body[iBody].dRadPowerMan + body[iBody].dRadPowerCore +
         body[iBody].dRadPowerCrust;
}

/**
   Total surface radiogenic energy (heat) flux.

   @param body Body struct
   @param system System struct
   @param update Update struct
   @param iBody Index of body

   @return Total surface radiogenic energy (heat) flux
*/
double fdSurfEnFluxRadTotal(BODY *body, SYSTEM *system, UPDATE *update,
                            int iBody, int iFoo) {
  return (body[iBody].dRadPowerTotal) /
         (4 * PI * body[iBody].dRadius * body[iBody].dRadius);
}
/**
   Radiogenic heat production coefficient.

   @param dNum Number of species
   @param dAge Age
   @param dHalflife Decay halflife

   @return Radiogenic heat production coefficient
*/
double fdRadheatConst(double dNum, double dAge, double dHalfLife) {
  return dNum / (exp(-dAge / dHalfLife));
}
/**
   Time derivative of number of radiogenic species.

   @param dConst Constant coefficient
   @param dAge Age
   @param dHalflife Decay halflife

   @return Time derivative of number of radiogenic species
*/
double
fdDNumRadDt(double dConst, double dHalfLife,
            double dAge) { // dN/dt, can be used by any radioactive system?
  return -dConst / dHalfLife * exp(-dAge / dHalfLife);
}

/**
   Radiogenic heat production coefficient for 26Al.

   @param dNum Number of species
   @param dAge Age

   @return Radiogenic heat production coefficient for 26Al
*/
double fd26AlConstant(double dNum, double dAge) {
  return fdRadheatConst(dNum, dAge, HALFLIFE26AL); // redirects to
                                                   // fdRadheatConst
}
/**
   Radiogenic heat production coefficient for 40K.

   @param dNum Number of species
   @param dAge Age

   @return Radiogenic heat production coefficient for 40K
*/
double fd40KConstant(double dNum, double dAge) {
  return fdRadheatConst(dNum, dAge, HALFLIFE40K); // redirects to fdRadheatConst
}
/**
   Radiogenic heat production coefficient for 232Th.

   @param dNum Number of species
   @param dAge Age

   @return Radiogenic heat production coefficient for 232Th
*/
double fd232ThConstant(double dNum,
                       double dAge) { // PED: changed dPower to dNum.
  return fdRadheatConst(dNum, dAge,
                        HALFLIFE232TH); // redirects to fdRadheatConst
}
/**
   Radiogenic heat production coefficient for 238U.

   @param dNum Number of species
   @param dAge Age

   @return Radiogenic heat production coefficient for 238U
*/
double fd238UConstant(double dNum, double dAge) {  // PED: changed dPower to
                                                   // dNum.
  return fdRadheatConst(dNum, dAge, HALFLIFE238U); // redirects to
                                                   // fdRadheatConst
}
/**
   Radiogenic heat production coefficient for 235U.

   @param dNum Number of species
   @param dAge Age

   @return Radiogenic heat production coefficient for 235U
*/
double fd235UConstant(double dNum, double dAge) {  // PED: changed dPower to
                                                   // dNum.
  return fdRadheatConst(dNum, dAge, HALFLIFE235U); // redirects to
                                                   // fdRadheatConst
}
/**
   Mantle radiogenic power for 26Al.

   @param update Update struct
   @param iBody Index of body

   @return Mantle radiogenic power for 26Al
*/
double fd26AlPowerMan(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD26AlNumManDt)) * ENERGY26AL;
}
/**
   Mantle radiogenic power for 40K.

   @param update Update struct
   @param iBody Index of body

   @return Mantle radiogenic power for 40K
*/
double fd40KPowerMan(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD40KNumManDt)) * ENERGY40K;
}
/**
   Mantle radiogenic power for 232Th.

   @param update Update struct
   @param iBody Index of body

   @return Mantle radiogenic power for 232Th
*/
double fd232ThPowerMan(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD232ThNumManDt)) * ENERGY232TH;
}
/**
   Mantle radiogenic power for 238U.

   @param update Update struct
   @param iBody Index of body

   @return Mantle radiogenic power for 238U
*/
double fd238UPowerMan(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD238UNumManDt)) * ENERGY238U;
}
/**
   Mantle radiogenic power for 235U.

   @param update Update struct
   @param iBody Index of body

   @return Mantle radiogenic power for 235U
*/
double fd235UPowerMan(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD235UNumManDt)) * ENERGY235U;
}
/**
   Core radiogenic power for 26Al.

   @param update Update struct
   @param iBody Index of body

   @return Core radiogenic power for 26Al
*/
double fd26AlPowerCore(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD26AlNumCoreDt)) * ENERGY26AL;
}
/**
   Core radiogenic power for 40K.

   @param update Update struct
   @param iBody Index of body

   @return Core radiogenic power for 40K
*/
double fd40KPowerCore(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD40KNumCoreDt)) * ENERGY40K;
}
/**
   Core radiogenic power for 232Th.

   @param update Update struct
   @param iBody Index of body

   @return Core radiogenic power for 232Th
*/
double fd232ThPowerCore(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD232ThNumCoreDt)) * ENERGY232TH;
}
/**
   Core radiogenic power for 238U.

   @param update Update struct
   @param iBody Index of body

   @return Core radiogenic power for 238U
*/
double fd238UPowerCore(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD238UNumCoreDt)) * ENERGY238U;
}
/**
   Core radiogenic power for 235U.

   @param update Update struct
   @param iBody Index of body

   @return Core radiogenic power for 235U
*/
double fd235UPowerCore(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD235UNumCoreDt)) * ENERGY235U;
}
/**
   Crust radiogenic power for 40K.

   @param update Update struct
   @param iBody Index of body

   @return Crust radiogenic power for 40K
*/
double fd40KPowerCrust(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD40KNumCrustDt)) * ENERGY40K;
}
/**
   Crust radiogenic power for 232Th.

   @param update Update struct
   @param iBody Index of body

   @return Crust radiogenic power for 232Th
*/
double fd232ThPowerCrust(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD232ThNumCrustDt)) * ENERGY232TH;
}
/**
   Crust radiogenic power for 238U.

   @param update Update struct
   @param iBody Index of body

   @return Crust radiogenic power for 238U
*/
double fd238UPowerCrust(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD238UNumCrustDt)) * ENERGY238U;
}
/**
   Crust radiogenic power for 235U.

   @param update Update struct
   @param iBody Index of body

   @return Crust radiogenic power for 235U
*/
double fd235UPowerCrust(UPDATE *update, int iBody) {
  return -(*(update[iBody].pdD235UNumCrustDt)) * ENERGY235U;
}
/**
   Total body (mantle+core) radiogenic power for 26Al.

   @param update Update struct
   @param iBody Index of body

   @return Total body radiogenic power for 26Al
*/
double fd26AlPower(UPDATE *update, int iBody) {
  return fd26AlPowerMan(update, iBody) +
         fd26AlPowerCore(update, iBody); // + fd26AlPowerCrust(update,iBody);
}
/**
   Total body (mantle+core+crust) radiogenic power for 40K.

   @param update Update struct
   @param iBody Index of body

   @return Total body radiogenic power for 40K
*/
double fd40KPower(UPDATE *update, int iBody) {
  return fd40KPowerMan(update, iBody) + fd40KPowerCore(update, iBody) +
         fd40KPowerCrust(update, iBody);
}
/**
   Total body (mantle+core+crust) radiogenic power for 232Th.

   @param update Update struct
   @param iBody Index of body

   @return Total body radiogenic power for 232Th
*/
double fd232ThPower(UPDATE *update, int iBody) {
  return fd232ThPowerMan(update, iBody) + fd232ThPowerCore(update, iBody) +
         fd232ThPowerCrust(update, iBody);
}
/**
   Total body (mantle+core+crust) radiogenic power for 238U.

   @param update Update struct
   @param iBody Index of body

   @return Total body radiogenic power for 238U
*/
double fd238UPower(UPDATE *update, int iBody) {
  return fd238UPowerMan(update, iBody) + fd238UPowerCore(update, iBody) +
         fd238UPowerCrust(update, iBody);
}
/**
   Total body (mantle+core+crust) radiogenic power for 235U.

   @param update Update struct
   @param iBody Index of body

   @return Total body radiogenic power for 235U
*/
double fd235UPower(UPDATE *update, int iBody) {
  return fd235UPowerMan(update, iBody) + fd235UPowerCore(update, iBody) +
         fd235UPowerCrust(update, iBody);
}

/* Energy Flux */
/**
   Surface radiogenic energy (heat) flux for 26Al.

   @param body Body struct
   @param update Update struct
   @param iBody Index of body

   @return Surface radiogenic energy (heat) flux for 26Al
*/
double fd26AlEnFlux(BODY *body, UPDATE *update, int iBody) {
  return fd26AlPower(update, iBody) /
         (4 * PI * body[iBody].dRadius * body[iBody].dRadius);
}
/**
   Surface radiogenic energy (heat) flux for 40K.

   @param body Body struct
   @param update Update struct
   @param iBody Index of body

   @return Surface radiogenic energy (heat) flux for 40K
*/
double fd40KEnFlux(BODY *body, UPDATE *update, int iBody) {
  return fd40KPower(update, iBody) /
         (4 * PI * body[iBody].dRadius * body[iBody].dRadius);
}
/**
   Surface radiogenic energy (heat) flux for 232Th.

   @param body Body struct
   @param update Update struct
   @param iBody Index of body

   @return Surface radiogenic energy (heat) flux for 232Th
*/
double fd232ThEnFlux(BODY *body, UPDATE *update, int iBody) {
  return fd232ThPower(update, iBody) /
         (4 * PI * body[iBody].dRadius * body[iBody].dRadius);
}
/**
   Surface radiogenic energy (heat) flux for 238U.

   @param body Body struct
   @param update Update struct
   @param iBody Index of body

   @return Surface radiogenic energy (heat) flux for 238U
*/
double fd238UEnFlux(BODY *body, UPDATE *update, int iBody) {
  return fd238UPower(update, iBody) /
         (4 * PI * body[iBody].dRadius * body[iBody].dRadius);
}
/**
   Surface radiogenic energy (heat) flux for 235U.

   @param body Body struct
   @param update Update struct
   @param iBody Index of body

   @return Surface radiogenic energy (heat) flux for 235U
*/
double fd235UEnFlux(BODY *body, UPDATE *update, int iBody) {
  return fd235UPower(update, iBody) /
         (4 * PI * body[iBody].dRadius * body[iBody].dRadius);
}

/* DN/Dt */
/**
   Time derivative of number of 26Al in mantle.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 26Al in mantle
*/
double fdD26AlNumManDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d26AlConstMan, HALFLIFE26AL,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 40K in mantle.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 40K in mantle
*/
double fdD40KNumManDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d40KConstMan, HALFLIFE40K,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 232Th in mantle.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 232Th in mantle
*/
double fdD232ThNumManDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d232ThConstMan, HALFLIFE232TH,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 238U in mantle.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 238U in mantle
*/
double fdD238UNumManDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d238UConstMan, HALFLIFE238U,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 235U in mantle.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 235U in mantle
*/
double fdD235UNumManDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d235UConstMan, HALFLIFE235U,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 26Al in mantle.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 26Al in mantle
*/
double fdD26AlNumCoreDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d26AlConstCore, HALFLIFE26AL,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 40K in mantle.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 40K in mantle
*/
double fdD40KNumCoreDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d40KConstCore, HALFLIFE40K,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 232Th in core.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 232Th in core
*/
double fdD232ThNumCoreDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d232ThConstCore, HALFLIFE232TH,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 238U in core.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 238U in core
*/
double fdD238UNumCoreDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d238UConstCore, HALFLIFE238U,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 235U in core.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 235U in core
*/
double fdD235UNumCoreDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d235UConstCore, HALFLIFE235U,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 40K in crust.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 40K in crust
*/
double fdD40KNumCrustDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d40KConstCrust, HALFLIFE40K,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 232Th in crust.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 232Th in crust
*/
double fdD232ThNumCrustDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d232ThConstCrust, HALFLIFE232TH,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 238U in crust.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 238U in crust
*/
double fdD238UNumCrustDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d238UConstCrust, HALFLIFE238U,
                     body[iaBody[0]].dAge);
}
/**
   Time derivative of number of 235U in crust.

   @param body Body struct
   @param system System struct
   @param iaBody Index of abody

   @return Time derivative of number of 235U in crust
*/
double fdD235UNumCrustDt(BODY *body, SYSTEM *system, int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d235UConstCrust, HALFLIFE235U,
                     body[iaBody[0]].dAge);
}
