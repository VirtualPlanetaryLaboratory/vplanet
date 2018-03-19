/********************** RADHEAT.C **********************/
/*
 * Rory Barnes, Sat Jun 28 07:07:38 PDT 2014
 *
 * Subroutines that control the integration of the 
 * radiogenic heating model. Also includes subroutines 
 * that switch between the two models.
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"


/**
   Copy body elements to dest

   @param dest The destination body
   @param src The body source
   @param iNumBodies Number of bodies
   @param iBody Index of body to copy
*/
void fvBodyCopyRadheat(BODY *dest,BODY *src,int foo,int iNumBodies,int iBody) {
  dest[iBody].d26AlNumMan = src[iBody].d26AlNumMan;
  dest[iBody].d26AlConstMan = src[iBody].d26AlConstMan;
  dest[iBody].d26AlNumCore = src[iBody].d26AlNumCore;
  dest[iBody].d26AlConstCore = src[iBody].d26AlConstCore;

  dest[iBody].d40KNumMan = src[iBody].d40KNumMan;
  dest[iBody].d40KConstMan = src[iBody].d40KConstMan;
  dest[iBody].d40KNumCore = src[iBody].d40KNumCore;
  dest[iBody].d40KConstCore = src[iBody].d40KConstCore;
  dest[iBody].d40KNumCrust = src[iBody].d40KNumCrust;
  dest[iBody].d40KConstCrust = src[iBody].d40KConstCrust;

  dest[iBody].d232ThNumMan = src[iBody].d232ThNumMan;
  dest[iBody].d232ThConstMan = src[iBody].d232ThConstMan;
  dest[iBody].d232ThNumCore = src[iBody].d232ThNumCore;
  dest[iBody].d232ThConstCore = src[iBody].d232ThConstCore;
  dest[iBody].d232ThNumCrust = src[iBody].d232ThNumCrust;
  dest[iBody].d232ThConstCrust = src[iBody].d232ThConstCrust;

  dest[iBody].d238UNumMan = src[iBody].d238UNumMan;
  dest[iBody].d238UConstMan = src[iBody].d238UConstMan;
  dest[iBody].d238UNumCore = src[iBody].d238UNumCore;
  dest[iBody].d238UConstCore = src[iBody].d238UConstCore;
  dest[iBody].d238UNumCrust = src[iBody].d238UNumCrust;
  dest[iBody].d238UConstCrust = src[iBody].d238UConstCrust;

  dest[iBody].d235UNumMan = src[iBody].d235UNumMan;
  dest[iBody].d235UConstMan = src[iBody].d235UConstMan;
  dest[iBody].d235UNumCore = src[iBody].d235UNumCore;
  dest[iBody].d235UConstCore = src[iBody].d235UConstCore;
  dest[iBody].d235UNumCrust = src[iBody].d235UNumCrust;
  dest[iBody].d235UConstCrust = src[iBody].d235UConstCrust;

  dest[iBody].dRadPowerTotal = src[iBody].dRadPowerTotal;
  dest[iBody].dRadPowerMan = src[iBody].dRadPowerMan;
  dest[iBody].dRadPowerCore = src[iBody].dRadPowerCore;
  dest[iBody].dRadPowerCrust = src[iBody].dRadPowerCrust;
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
void fvRead26AlPowerMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 26AlMass and 26AlNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].d26AlPowerMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d26AlPowerMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       //CHANGED units Mass to Power.
       body[iFile-1].d26AlPowerMan = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].d26AlPowerMan = options->dDefault;
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
void fvRead26AlPowerCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].d26AlPowerCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       body[iFile-1].d26AlPowerCore = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].d26AlPowerCore = options->dDefault;
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
void fvRead26AlMassMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 26AlPower and 26AlNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d26AlMassMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d26AlMassMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d26AlMassMan = options->dDefault;
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
void fvRead26AlMassCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d26AlMassCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d26AlMassCore = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d26AlMassCore = options->dDefault;
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
void fvRead26AlNumMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 26AlPower and 26AlNum */
  int lTmp=-1;
  double dTmp;

  //  fflush(stdout);
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d26AlNumMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);  //dTmp=input value, dNegativeDouble=-dNeg (default Value).
    else
      body[iFile-1].d26AlNumMan = dTmp;   //units of num are num!
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d26AlNumMan = options->dDefault;
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
void fvRead26AlNumCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;

  //  fflush(stdout);
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d26AlNumCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);  //dTmp=input value, dNegativeDouble=-dNeg (default Value).
    else
      body[iFile-1].d26AlNumCore = dTmp;   //units of num are num!
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d26AlNumCore = options->dDefault;
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
void fvRead40KPowerMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 40KMass and 40KNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].d40KPowerMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d40KPowerMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       //CHANGED units Mass to Power.
       body[iFile-1].d40KPowerMan = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].d40KPowerMan = options->dDefault;
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
void fvRead40KPowerCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].d40KPowerCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       body[iFile-1].d40KPowerCore = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].d40KPowerCore = options->dDefault;
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
void fvRead40KMassMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 40KPower and 40KNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d40KMassMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d40KMassMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d40KMassMan = options->dDefault;
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
void fvRead40KMassCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d40KMassCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d40KMassCore = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d40KMassCore = options->dDefault;
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
void fvRead40KNumMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 40KPower and 40KNum */
  int lTmp=-1;
  double dTmp;

  //  fflush(stdout);
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d40KNumMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);  //dTmp=input value, dNegativeDouble=-dNeg (default Value).
    else
      body[iFile-1].d40KNumMan = dTmp;   //units of num are num!
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d40KNumMan = options->dDefault;
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
void fvRead40KNumCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d40KNumCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);  //dTmp=input value, dNegativeDouble=-dNeg (default Value).
    else
      body[iFile-1].d40KNumCore = dTmp;   //units of num are num!
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d40KNumCore = options->dDefault;
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
void fvRead40KPowerCrust(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].d40KPowerCrust = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       body[iFile-1].d40KPowerCrust = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].d40KPowerCrust = options->dDefault;
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
void fvRead40KMassCrust(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d40KMassCrust = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d40KMassCrust = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d40KMassCrust = options->dDefault;
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
void fvRead40KNumCrust(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d40KNumCrust = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);  //dTmp=input value, dNegativeDouble=-dNeg (default Value).
    else
      body[iFile-1].d40KNumCrust = dTmp;   //units of num are num!
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d40KNumCrust = options->dDefault;
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
void fvRead232ThPowerMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThMass and 232ThNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d232ThPowerMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d232ThPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d232ThPowerMan = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThPowerMan = options->dDefault;
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
void fvRead232ThPowerCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThMass and 232ThNum */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d232ThPowerCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d232ThPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d232ThPowerCore = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThPowerCore = options->dDefault;
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
void fvRead232ThMassMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d232ThMassMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d232ThMassMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThMassMan = options->dDefault;
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
void fvRead232ThMassCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d232ThMassCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d232ThMassCore = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThMassCore = options->dDefault;
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
void fvRead232ThNumMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d232ThNumMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d232ThNumMan = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThNumMan = options->dDefault;
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
void fvRead232ThNumCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d232ThNumCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d232ThNumCore = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThNumCore = options->dDefault;
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
void fvRead232ThPowerCrust(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThMass and 232ThNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d232ThPowerCrust = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d232ThPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d232ThPowerCrust = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThPowerCrust = options->dDefault;
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
void fvRead232ThMassCrust(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d232ThMassCrust = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d232ThMassCrust = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThMassCrust = options->dDefault;
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
void fvRead232ThNumCrust(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 232ThPower and 232ThMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d232ThNumCrust = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d232ThNumCrust = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d232ThNumCrust = options->dDefault;
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
void fvRead238UPowerMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UMass and 232UNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d238UPowerMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d238UPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d238UPowerMan = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UPowerMan = options->dDefault;
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
void fvRead238UMassMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d238UMassMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d238UMassMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UMassMan = options->dDefault;
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
void fvRead238UNumMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d238UNumMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d238UNumMan = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UNumMan = options->dDefault;
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
void fvRead238UPowerCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UMass and 232UNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d238UPowerCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d238UPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d238UPowerCore = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UPowerCore = options->dDefault;
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
void fvRead238UMassCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d238UMassCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d238UMassCore = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UMassCore = options->dDefault;
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
void fvRead238UNumCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d238UNumCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d238UNumCore = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UNumCore = options->dDefault;
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
void fvRead238UPowerCrust(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UMass and 232UNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d238UPowerCrust = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       body[iFile-1].d238UPowerCrust = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UPowerCrust = options->dDefault;
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
void fvRead238UMassCrust(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d238UMassCrust = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d238UMassCrust = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UMassCrust = options->dDefault;
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
void fvRead238UNumCrust(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 238UPower and 238UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d238UNumCrust = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d238UNumCrust = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d238UNumCrust = options->dDefault;
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
void fvRead235UPowerMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UMass and 232UNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d235UPowerMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d235UPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d235UPowerMan = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d235UPowerMan = options->dDefault;
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
void fvRead235UMassMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d235UMassMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d235UMassMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d235UMassMan = options->dDefault;
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
void fvRead235UNumMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
      NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
      if (dTmp < 0)
        body[iFile-1].d235UNumMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
      else
        body[iFile-1].d235UNumMan = dTmp;
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d235UNumMan = options->dDefault;
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
void fvRead235UPowerCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UMass and 232UNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d235UPowerCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d235UPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d235UPowerCore = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d235UPowerCore = options->dDefault;
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
void fvRead235UMassCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d235UMassCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d235UMassCore = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d235UMassCore = options->dDefault;
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
void fvRead235UNumCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
      NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
      if (dTmp < 0)
        body[iFile-1].d235UNumCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
      else
        body[iFile-1].d235UNumCore = dTmp;
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)
        body[iFile-1].d235UNumCore = options->dDefault;
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
void fvRead235UPowerCrust(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UMass and 232UNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d235UPowerCrust = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d235UPower = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       body[iFile-1].d235UPowerCrust = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d235UPowerCrust = options->dDefault;
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
void fvRead235UMassCrust(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UNum */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].d235UMassCrust = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
      body[iFile-1].d235UMassCrust = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].d235UMassCrust = options->dDefault;
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
void fvRead235UNumCrust(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
      NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
      if (dTmp < 0)
        body[iFile-1].d235UNumCrust = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
      else
        body[iFile-1].d235UNumCrust = dTmp;
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)
        body[iFile-1].d235UNumCrust = options->dDefault;
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
void fvReadHalt40KPower(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
      NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
      if (dTmp < 0)
	control->Halt[iFile-1].dMin40KPower = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
      else
	control->Halt[iFile-1].dMin40KPower = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);

      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)
       control->Halt[iFile-1].dMin40KPower = options->dDefault;
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
void fvReadHalt235UPower(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
      NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
      if (dTmp < 0)
	control->Halt[iFile-1].dMin235UPower = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
      else
	control->Halt[iFile-1].dMin235UPower = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);

      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)
       control->Halt[iFile-1].dMin235UPower = options->dDefault;
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
void fvReadHalt238UPower(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
      NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
      if (dTmp < 0)
	control->Halt[iFile-1].dMin238UPower = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
      else
	control->Halt[iFile-1].dMin238UPower = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);

      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)
       control->Halt[iFile-1].dMin238UPower = options->dDefault;
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
void fvReadHalt232ThPower(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
      NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
      if (dTmp < 0)
	control->Halt[iFile-1].dMin232ThPower = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
      else
	control->Halt[iFile-1].dMin232ThPower = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);

      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)
       control->Halt[iFile-1].dMin232ThPower = options->dDefault;
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
void fvReadHaltRadPower(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with 235UPower and 235UMass */
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
      NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
      if (dTmp < 0)
	control->Halt[iFile-1].dMinRadPower = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
      else
	control->Halt[iFile-1].dMinRadPower = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);

      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)
       control->Halt[iFile-1].dMinRadPower = options->dDefault;
}

/* Initiatlize Input Options */
/**
   Initialize input options to default values

   @param options Options struct
   @param Function to read in options
*/
void fvInitializeOptionsRadheat(OPTIONS *options,fnReadOption fnRead[]) {

  /* 26Al */
  sprintf(options[OPT_26ALMASSMAN].cName,"d26AlMassMan");
  sprintf(options[OPT_26ALMASSMAN].cDescr,"Initial Mantle Mass of 26Al");
  sprintf(options[OPT_26ALMASSMAN].cDefault,"Primordial Earth: 0");
  options[OPT_26ALMASSMAN].iType = 2;
  options[OPT_26ALMASSMAN].iMultiFile = 1;
  options[OPT_26ALMASSMAN].dNeg = MEARTH;
  options[OPT_26ALMASSMAN].dDefault = 0; 
  sprintf(options[OPT_26ALMASSMAN].cNeg,"Earth Masses");
  fnRead[OPT_26ALMASSMAN] = &Read26AlMassMan;

  sprintf(options[OPT_26ALMASSCORE].cName,"d26AlMassCore");
  sprintf(options[OPT_26ALMASSCORE].cDescr,"Initial Core Mass of 26Al");
  sprintf(options[OPT_26ALMASSCORE].cDefault,"Primordial Earth: 0");
  options[OPT_26ALMASSCORE].iType = 2;
  options[OPT_26ALMASSCORE].iMultiFile = 1;
  options[OPT_26ALMASSCORE].dNeg = MEARTH;
  options[OPT_26ALMASSCORE].dDefault = 0; 
  sprintf(options[OPT_26ALMASSCORE].cNeg,"Earth Masses");
  fnRead[OPT_26ALMASSCORE] = &Read26AlMassCore;
  
  sprintf(options[OPT_26ALNUMMAN].cName,"d26AlNumMan");
  sprintf(options[OPT_26ALNUMMAN].cDescr,"Initial Mantle Number of 26Al Atoms");
  sprintf(options[OPT_26ALNUMMAN].cDefault,"Primordial Earth: 0");
  options[OPT_26ALNUMMAN].iType = 2;
  options[OPT_26ALNUMMAN].iMultiFile = 1;
  options[OPT_26ALNUMMAN].dNeg = ENUMMAN26AL;
  options[OPT_26ALNUMMAN].dDefault = 0;
  sprintf(options[OPT_26ALNUMMAN].cNeg,"Primordial Earth 26Al Number");
  fnRead[OPT_26ALNUMMAN] = &Read26AlNumMan;

  sprintf(options[OPT_26ALNUMCORE].cName,"d26AlNumCore");
  sprintf(options[OPT_26ALNUMCORE].cDescr,"Initial Core Number of 26Al Atoms");
  sprintf(options[OPT_26ALNUMCORE].cDefault,"Primordial Earth: 0");
  options[OPT_26ALNUMCORE].iType = 2;
  options[OPT_26ALNUMCORE].iMultiFile = 1;
  options[OPT_26ALNUMCORE].dNeg = ENUMCORE26AL;
  options[OPT_26ALNUMCORE].dDefault = 0;
  sprintf(options[OPT_26ALNUMCORE].cNeg,"Primordial Earth 26Al Number");
  fnRead[OPT_26ALNUMCORE] = &Read26AlNumCore;

  sprintf(options[OPT_26ALPOWERMAN].cName,"d26AlPowerMan");
  sprintf(options[OPT_26ALPOWERMAN].cDescr,"Initial Mantle Power Production from 26Al Atoms");
  sprintf(options[OPT_26ALPOWERMAN].cDefault,"Primordial Earth: 0 TW");
  options[OPT_26ALPOWERMAN].iType = 2;
  options[OPT_26ALPOWERMAN].iMultiFile = 1;
  options[OPT_26ALPOWERMAN].dNeg = EPOWERMAN26AL;  //Earth's POWER of 26Al
  options[OPT_26ALPOWERMAN].dDefault = 0;
  sprintf(options[OPT_26ALPOWERMAN].cNeg,"TW");
  fnRead[OPT_26ALPOWERMAN] = &Read26AlPowerMan;

  sprintf(options[OPT_26ALPOWERCORE].cName,"d26AlPowerCore");
  sprintf(options[OPT_26ALPOWERCORE].cDescr,"Initial Core Power Production from 26Al Atoms");
  sprintf(options[OPT_26ALPOWERCORE].cDefault,"Primordial Earth: 0 TW");
  options[OPT_26ALPOWERCORE].iType = 2;
  options[OPT_26ALPOWERCORE].iMultiFile = 1;
  options[OPT_26ALPOWERCORE].dNeg = EPOWERCORE26AL;  //Earth's POWER of 26Al
  options[OPT_26ALPOWERCORE].dDefault = 0;
  sprintf(options[OPT_26ALPOWERCORE].cNeg,"TW");
  fnRead[OPT_26ALPOWERCORE] = &Read26AlPowerCore;

  /* 40K */
  sprintf(options[OPT_40KMASSMAN].cName,"d40KMassMan");
  sprintf(options[OPT_40KMASSMAN].cDescr,"Initial Mantle Mass of 40K");
  sprintf(options[OPT_40KMASSMAN].cDefault,"Primordial Earth: xxx");
  options[OPT_40KMASSMAN].iType = 2;
  options[OPT_40KMASSMAN].iMultiFile = 1;
  options[OPT_40KMASSMAN].dNeg = EMASSMAN40K;
  options[OPT_40KMASSMAN].dDefault = 0; 
  sprintf(options[OPT_40KMASSMAN].cNeg,"Earth Masses");
  fnRead[OPT_40KMASSMAN] = &Read40KMassMan;

  sprintf(options[OPT_40KMASSCORE].cName,"d40KMassCore");
  sprintf(options[OPT_40KMASSCORE].cDescr,"Initial Core Mass of 40K");
  sprintf(options[OPT_40KMASSCORE].cDefault,"Primordial Earth: xxx");
  options[OPT_40KMASSCORE].iType = 2;
  options[OPT_40KMASSCORE].iMultiFile = 1;
  options[OPT_40KMASSCORE].dNeg = EMASSCORE40K;
  options[OPT_40KMASSCORE].dDefault = 0; 
  sprintf(options[OPT_40KMASSCORE].cNeg,"Earth Masses");
  fnRead[OPT_40KMASSCORE] = &Read40KMassCore;

  sprintf(options[OPT_40KMASSCRUST].cName,"d40KMassCrust");
  sprintf(options[OPT_40KMASSCRUST].cDescr,"Initial Crust Mass of 40K");
  sprintf(options[OPT_40KMASSCRUST].cDefault,"Primordial Earth: xxx");
  options[OPT_40KMASSCRUST].iType = 2;
  options[OPT_40KMASSCRUST].iMultiFile = 1;
  options[OPT_40KMASSCRUST].dNeg = EMASSCRUST40K;
  options[OPT_40KMASSCRUST].dDefault = 0; 
  sprintf(options[OPT_40KMASSCRUST].cNeg,"Earth Masses");
  fnRead[OPT_40KMASSCRUST] = &Read40KMassCrust;
  
  sprintf(options[OPT_40KNUMMAN].cName,"d40KNumMan");
  sprintf(options[OPT_40KNUMMAN].cDescr,"Initial Mantle Number of 40K Atoms");
  sprintf(options[OPT_40KNUMMAN].cDefault,"Primordial Earth: xxx");
  options[OPT_40KNUMMAN].iType = 2;
  options[OPT_40KNUMMAN].iMultiFile = 1;
  options[OPT_40KNUMMAN].dNeg = ENUMMAN40K;
  options[OPT_40KNUMMAN].dDefault = 0;
  sprintf(options[OPT_40KNUMMAN].cNeg,"Primordial Earth 40K Number");
  fnRead[OPT_40KNUMMAN] = &Read40KNumMan;

  sprintf(options[OPT_40KNUMCORE].cName,"d40KNumCore");
  sprintf(options[OPT_40KNUMCORE].cDescr,"Initial Core Number of 40K Atoms");
  sprintf(options[OPT_40KNUMCORE].cDefault,"Primordial Earth: xxx");
  options[OPT_40KNUMCORE].iType = 2;
  options[OPT_40KNUMCORE].iMultiFile = 1;
  options[OPT_40KNUMCORE].dNeg = ENUMCORE40K;
  options[OPT_40KNUMCORE].dDefault = 0;
  sprintf(options[OPT_40KNUMCORE].cNeg,"Primordial Earth 40K Number");
  fnRead[OPT_40KNUMCORE] = &Read40KNumCore;

  sprintf(options[OPT_40KNUMCRUST].cName,"d40KNumCrust");
  sprintf(options[OPT_40KNUMCRUST].cDescr,"Initial Crust Number of 40K Atoms");
  sprintf(options[OPT_40KNUMCRUST].cDefault,"Primordial Earth: xxx");
  options[OPT_40KNUMCRUST].iType = 2;
  options[OPT_40KNUMCRUST].iMultiFile = 1;
  options[OPT_40KNUMCRUST].dNeg = ENUMCRUST40K;
  options[OPT_40KNUMCRUST].dDefault = 0;
  sprintf(options[OPT_40KNUMCRUST].cNeg,"Primordial Earth 40K Number");
  fnRead[OPT_40KNUMCRUST] = &Read40KNumCrust;
  
  sprintf(options[OPT_40KPOWERMAN].cName,"d40KPowerMan");
  sprintf(options[OPT_40KPOWERMAN].cDescr,"Initial Mantle Power Production from 40K Atoms");
  sprintf(options[OPT_40KPOWERMAN].cDefault,"Primordial Earth: xx TW");
  options[OPT_40KPOWERMAN].iType = 2;
  options[OPT_40KPOWERMAN].iMultiFile = 1;
  options[OPT_40KPOWERMAN].dNeg = EPOWERMAN40K;  //Earth's POWER of 40K
  options[OPT_40KPOWERMAN].dDefault = 0;
  sprintf(options[OPT_40KPOWERMAN].cNeg,"TW");
  fnRead[OPT_40KPOWERMAN] = &Read40KPowerMan;

  sprintf(options[OPT_40KPOWERCORE].cName,"d40KPowerCore");
  sprintf(options[OPT_40KPOWERCORE].cDescr,"Initial Core Power Production from 40K Atoms");
  sprintf(options[OPT_40KPOWERCORE].cDefault,"Primordial Earth: xx TW");
  options[OPT_40KPOWERCORE].iType = 2;
  options[OPT_40KPOWERCORE].iMultiFile = 1;
  options[OPT_40KPOWERCORE].dNeg = EPOWERCORE40K;  //Earth's POWER of 40K
  options[OPT_40KPOWERCORE].dDefault = 0;
  sprintf(options[OPT_40KPOWERCORE].cNeg,"TW");
  fnRead[OPT_40KPOWERCORE] = &Read40KPowerCore;

  sprintf(options[OPT_40KPOWERCRUST].cName,"d40KPowerCrust");
  sprintf(options[OPT_40KPOWERCRUST].cDescr,"Initial Crust Power Production from 40K Atoms");
  sprintf(options[OPT_40KPOWERCRUST].cDefault,"Primordial Earth: xx TW");
  options[OPT_40KPOWERCRUST].iType = 2;
  options[OPT_40KPOWERCRUST].iMultiFile = 1;
  options[OPT_40KPOWERCRUST].dNeg = EPOWERCRUST40K;  //Earth's POWER of 40K
  options[OPT_40KPOWERCRUST].dDefault = 0;
  sprintf(options[OPT_40KPOWERCRUST].cNeg,"TW");
  fnRead[OPT_40KPOWERCRUST] = &Read40KPowerCrust;
  
  /* 232Th */
  sprintf(options[OPT_232THMASSMAN].cName,"d232ThMassMan");
  sprintf(options[OPT_232THMASSMAN].cDescr,"Initial Mass of 232Th");
  sprintf(options[OPT_232THMASSMAN].cDefault,"Primordial Earth: xxx");
  options[OPT_232THMASSMAN].iType = 2;
  options[OPT_232THMASSMAN].iMultiFile = 1;
  options[OPT_232THMASSMAN].dNeg = EMASSMAN232TH;
  options[OPT_232THMASSMAN].dDefault = 0;
  sprintf(options[OPT_232THMASSMAN].cNeg,"Earth Masses");
  fnRead[OPT_232THMASSMAN] = &Read232ThMassMan; 
  
  sprintf(options[OPT_232THNUMMAN].cName,"d232ThNumMan");
  sprintf(options[OPT_232THNUMMAN].cDescr,"Initial Number of 232Th Atoms");
  sprintf(options[OPT_232THNUMMAN].cDefault,"Primordial Earth: xxx");
  options[OPT_232THNUMMAN].iType = 2;
  options[OPT_232THNUMMAN].iMultiFile = 1;
  options[OPT_232THNUMMAN].dNeg = ENUMMAN232TH;
  options[OPT_232THNUMMAN].dDefault = 0;
  sprintf(options[OPT_232THNUMMAN].cNeg,"Primordial Earth 232Th Number");
  fnRead[OPT_232THNUMMAN] = &Read232ThNumMan;
  
  sprintf(options[OPT_232THPOWERMAN].cName,"d232ThPowerMan");
  sprintf(options[OPT_232THPOWERMAN].cDescr,"Initial Power Production from 232Th Atoms");
  sprintf(options[OPT_232THPOWERMAN].cDefault,"Primordial Earth: xx TW");
  options[OPT_232THPOWERMAN].iType = 2;
  options[OPT_232THPOWERMAN].iMultiFile = 1;
  options[OPT_232THPOWERMAN].dNeg = EPOWERMAN232TH;  //1e12*1e-7; // cgs
  options[OPT_232THPOWERMAN].dDefault = 0;
  sprintf(options[OPT_232THPOWERMAN].cNeg,"TW");
  fnRead[OPT_232THPOWERMAN] = &Read232ThPowerMan;

  sprintf(options[OPT_232THMASSCORE].cName,"d232ThMassCore");
  sprintf(options[OPT_232THMASSCORE].cDescr,"Initial Core Mass of 232Th");
  sprintf(options[OPT_232THMASSCORE].cDefault,"Primordial Earth: xxx");
  options[OPT_232THMASSCORE].iType = 2;
  options[OPT_232THMASSCORE].iMultiFile = 1;
  options[OPT_232THMASSCORE].dNeg = EMASSCORE232TH;
  options[OPT_232THMASSCORE].dDefault = 0;
  sprintf(options[OPT_232THMASSCORE].cNeg,"Earth Masses");
  fnRead[OPT_232THMASSCORE] = &Read232ThMassCore; 
  
  sprintf(options[OPT_232THNUMCORE].cName,"d232ThNumCore");
  sprintf(options[OPT_232THNUMCORE].cDescr,"Initial Core Number of 232Th Atoms");
  sprintf(options[OPT_232THNUMCORE].cDefault,"Primordial Earth: xxx");
  options[OPT_232THNUMCORE].iType = 2;
  options[OPT_232THNUMCORE].iMultiFile = 1;
  options[OPT_232THNUMCORE].dNeg = ENUMCORE232TH;
  options[OPT_232THNUMCORE].dDefault = 0;
  sprintf(options[OPT_232THNUMCORE].cNeg,"Primordial Earth 232Th Number");
  fnRead[OPT_232THNUMCORE] = &Read232ThNumCore;
  
  sprintf(options[OPT_232THPOWERCORE].cName,"d232ThPowerCore");
  sprintf(options[OPT_232THPOWERCORE].cDescr,"Initial Core Power Production from 232Th Atoms");
  sprintf(options[OPT_232THPOWERCORE].cDefault,"Primordial Earth: xx TW");
  options[OPT_232THPOWERCORE].iType = 2;
  options[OPT_232THPOWERCORE].iMultiFile = 1;
  options[OPT_232THPOWERCORE].dNeg = EPOWERCORE232TH;  //1e12*1e-7; // cgs
  options[OPT_232THPOWERCORE].dDefault = 0;
  sprintf(options[OPT_232THPOWERCORE].cNeg,"TW");
  fnRead[OPT_232THPOWERCORE] = &Read232ThPowerCore;

  sprintf(options[OPT_232THMASSCRUST].cName,"d232ThMassCrust");
  sprintf(options[OPT_232THMASSCRUST].cDescr,"Initial Crust Mass of 232Th");
  sprintf(options[OPT_232THMASSCRUST].cDefault,"Primordial Earth: xxx");
  options[OPT_232THMASSCRUST].iType = 2;
  options[OPT_232THMASSCRUST].iMultiFile = 1;
  options[OPT_232THMASSCRUST].dNeg = EMASSCRUST232TH;
  options[OPT_232THMASSCRUST].dDefault = 0;
  sprintf(options[OPT_232THMASSCRUST].cNeg,"Earth Masses");
  fnRead[OPT_232THMASSCRUST] = &Read232ThMassCrust; 
  
  sprintf(options[OPT_232THNUMCRUST].cName,"d232ThNumCrust");
  sprintf(options[OPT_232THNUMCRUST].cDescr,"Initial Crust Number of 232Th Atoms");
  sprintf(options[OPT_232THNUMCRUST].cDefault,"Primordial Earth: xxx");
  options[OPT_232THNUMCRUST].iType = 2;
  options[OPT_232THNUMCRUST].iMultiFile = 1;
  options[OPT_232THNUMCRUST].dNeg = ENUMCRUST232TH;
  options[OPT_232THNUMCRUST].dDefault = 0;
  sprintf(options[OPT_232THNUMCRUST].cNeg,"Primordial Earth 232Th Number");
  fnRead[OPT_232THNUMCRUST] = &Read232ThNumCrust;
  
  sprintf(options[OPT_232THPOWERCRUST].cName,"d232ThPowerCrust");
  sprintf(options[OPT_232THPOWERCRUST].cDescr,"Initial Crust Power Production from 232Th Atoms");
  sprintf(options[OPT_232THPOWERCRUST].cDefault,"Primordial Earth: xx TW");
  options[OPT_232THPOWERCRUST].iType = 2;
  options[OPT_232THPOWERCRUST].iMultiFile = 1;
  options[OPT_232THPOWERCRUST].dNeg = EPOWERCRUST232TH;  //1e12*1e-7; // cgs
  options[OPT_232THPOWERCRUST].dDefault = 0;
  sprintf(options[OPT_232THPOWERCRUST].cNeg,"TW");
  fnRead[OPT_232THPOWERCRUST] = &Read232ThPowerCrust;

  /* 238U*/
  sprintf(options[OPT_238UMASSMAN].cName,"d238UMassMan");
  sprintf(options[OPT_238UMASSMAN].cDescr,"Initial Mass of 238U");
  sprintf(options[OPT_238UMASSMAN].cDefault,"Primordial Earth: xxx");
  options[OPT_238UMASSMAN].iType = 2;
  options[OPT_238UMASSMAN].iMultiFile = 1;
  options[OPT_238UMASSMAN].dNeg = EMASSMAN238U;
  options[OPT_238UMASSMAN].dDefault = 0;
  sprintf(options[OPT_238UMASSMAN].cNeg,"Earth Masses");
  fnRead[OPT_238UMASSMAN] = &Read238UMassMan;
  
  sprintf(options[OPT_238UNUMMAN].cName,"d238UNumMan");
  sprintf(options[OPT_238UNUMMAN].cDescr,"Initial Number of 238U Atoms");
  sprintf(options[OPT_238UNUMMAN].cDefault,"1");
  options[OPT_238UNUMMAN].dDefault = 0;
  options[OPT_238UNUMMAN].iType = 2;
  options[OPT_238UNUMMAN].iMultiFile = 1;
  options[OPT_238UNUMMAN].dNeg = ENUMMAN238U;
  options[OPT_238UNUMMAN].dDefault = 0;
  sprintf(options[OPT_238UNUMMAN].cNeg,"Primordial Earth 238U Number");
  fnRead[OPT_238UNUMMAN] = &Read238UNumMan;

  sprintf(options[OPT_238UPOWERMAN].cName,"d238UPowerMan");  //section added PED
  sprintf(options[OPT_238UPOWERMAN].cDescr,"Initial Power Production from 238U Atoms");
  sprintf(options[OPT_238UPOWERMAN].cDefault,"Primordial Earth: xx TW");
  options[OPT_238UPOWERMAN].iType = 2;
  options[OPT_238UPOWERMAN].iMultiFile = 1;
  options[OPT_238UPOWERMAN].dNeg = EPOWERMAN238U;   //1e12*1e-7; // cgs
  options[OPT_238UPOWERMAN].dDefault = 0;
  sprintf(options[OPT_238UPOWERMAN].cNeg,"TW");
  fnRead[OPT_238UPOWERMAN] = &Read238UPowerMan;

  sprintf(options[OPT_238UMASSCORE].cName,"d238UMassCore");
  sprintf(options[OPT_238UMASSCORE].cDescr,"Initial Core Mass of 238U");
  sprintf(options[OPT_238UMASSCORE].cDefault,"Primordial Earth: xxx");
  options[OPT_238UMASSCORE].iType = 2;
  options[OPT_238UMASSCORE].iMultiFile = 1;
  options[OPT_238UMASSCORE].dNeg = EMASSCORE238U;
  options[OPT_238UMASSCORE].dDefault = 0;
  sprintf(options[OPT_238UMASSCORE].cNeg,"Earth Masses");
  fnRead[OPT_238UMASSCORE] = &Read238UMassCore;
  
  sprintf(options[OPT_238UNUMCORE].cName,"d238UNumCore");
  sprintf(options[OPT_238UNUMCORE].cDescr,"Initial Core Number of 238U Atoms");
  sprintf(options[OPT_238UNUMCORE].cDefault,"1");
  options[OPT_238UNUMCORE].dDefault = 0;
  options[OPT_238UNUMCORE].iType = 2;
  options[OPT_238UNUMCORE].iMultiFile = 1;
  options[OPT_238UNUMCORE].dNeg = ENUMCORE238U;
  options[OPT_238UNUMCORE].dDefault = 0;
  sprintf(options[OPT_238UNUMCORE].cNeg,"Primordial Earth 238U Number");
  fnRead[OPT_238UNUMCORE] = &Read238UNumCore;

  sprintf(options[OPT_238UPOWERCORE].cName,"d238UPowerCore");  //section added PED
  sprintf(options[OPT_238UPOWERCORE].cDescr,"Initial Core Power Production from 238U Atoms");
  sprintf(options[OPT_238UPOWERCORE].cDefault,"Primordial Earth: xx TW");
  options[OPT_238UPOWERCORE].iType = 2;
  options[OPT_238UPOWERCORE].iMultiFile = 1;
  options[OPT_238UPOWERCORE].dNeg = EPOWERCORE238U;   //1e12*1e-7; // cgs
  options[OPT_238UPOWERCORE].dDefault = 0;
  sprintf(options[OPT_238UPOWERCORE].cNeg,"TW");
  fnRead[OPT_238UPOWERCORE] = &Read238UPowerCore;

  sprintf(options[OPT_238UMASSCRUST].cName,"d238UMassCrust");
  sprintf(options[OPT_238UMASSCRUST].cDescr,"Initial Crust Mass of 238U");
  sprintf(options[OPT_238UMASSCRUST].cDefault,"Primordial Earth: xxx");
  options[OPT_238UMASSCRUST].iType = 2;
  options[OPT_238UMASSCRUST].iMultiFile = 1;
  options[OPT_238UMASSCRUST].dNeg = EMASSCRUST238U;
  options[OPT_238UMASSCRUST].dDefault = 0;
  sprintf(options[OPT_238UMASSCRUST].cNeg,"Earth Masses");
  fnRead[OPT_238UMASSCRUST] = &Read238UMassCrust;
  
  sprintf(options[OPT_238UNUMCRUST].cName,"d238UNumCrust");
  sprintf(options[OPT_238UNUMCRUST].cDescr,"Initial Crust Number of 238U Atoms");
  sprintf(options[OPT_238UNUMCRUST].cDefault,"1");
  options[OPT_238UNUMCRUST].dDefault = 0;
  options[OPT_238UNUMCRUST].iType = 2;
  options[OPT_238UNUMCRUST].iMultiFile = 1;
  options[OPT_238UNUMCRUST].dNeg = ENUMCRUST238U;
  options[OPT_238UNUMCRUST].dDefault = 0;
  sprintf(options[OPT_238UNUMCRUST].cNeg,"Primordial Earth 238U Number");
  fnRead[OPT_238UNUMCRUST] = &Read238UNumCrust;

  sprintf(options[OPT_238UPOWERCRUST].cName,"d238UPowerCrust");  //section added PED
  sprintf(options[OPT_238UPOWERCRUST].cDescr,"Initial Crust Power Production from 238U Atoms");
  sprintf(options[OPT_238UPOWERCRUST].cDefault,"Primordial Earth: xx TW");
  options[OPT_238UPOWERCRUST].iType = 2;
  options[OPT_238UPOWERCRUST].iMultiFile = 1;
  options[OPT_238UPOWERCRUST].dNeg = EPOWERCRUST238U;   //1e12*1e-7; // cgs
  options[OPT_238UPOWERCRUST].dDefault = 0;
  sprintf(options[OPT_238UPOWERCRUST].cNeg,"TW");
  fnRead[OPT_238UPOWERCRUST] = &Read238UPowerCrust;
  
  /* 235U */
  sprintf(options[OPT_235UMASSMAN].cName,"d235UMassMan");  //PED
  sprintf(options[OPT_235UMASSMAN].cDescr,"Initial Mass of 235U");
  sprintf(options[OPT_235UMASSMAN].cDefault,"Primordial Earth: xxx");
  options[OPT_235UMASSMAN].iType = 2;
  options[OPT_235UMASSMAN].iMultiFile = 1;
  options[OPT_235UMASSMAN].dNeg = EMASSMAN235U;
  options[OPT_235UMASSMAN].dDefault = 0;
  sprintf(options[OPT_235UMASSMAN].cNeg,"Earth Masses");
  fnRead[OPT_235UMASSMAN] = &Read235UMassMan;
  
  sprintf(options[OPT_235UNUMMAN].cName,"d235UNumMan");  //PED
  sprintf(options[OPT_235UNUMMAN].cDescr,"Initial Number of 235U Atoms");
  sprintf(options[OPT_235UNUMMAN].cDefault,"1");
  options[OPT_235UNUMMAN].iType = 2;
  options[OPT_235UNUMMAN].iMultiFile = 1;
  options[OPT_235UNUMMAN].dNeg = ENUMMAN235U;
  options[OPT_235UNUMMAN].dDefault = 0;
  sprintf(options[OPT_235UNUMMAN].cNeg,"Primordial Earth 235U Numer");
  fnRead[OPT_235UNUMMAN] = &Read235UNumMan;

  sprintf(options[OPT_235UPOWERMAN].cName,"d235UPowerMan");  //section added PED
  sprintf(options[OPT_235UPOWERMAN].cDescr,"Initial Power Production from 235U Atoms");
  sprintf(options[OPT_235UPOWERMAN].cDefault,"Primordial Earth: xx TW");
  options[OPT_235UPOWERMAN].iType = 2;
  options[OPT_235UPOWERMAN].iMultiFile = 1;
  options[OPT_235UPOWERMAN].dNeg = EPOWERMAN235U;  
  options[OPT_235UPOWERMAN].dDefault = 0;
  sprintf(options[OPT_235UPOWERMAN].cNeg,"TW");
  fnRead[OPT_235UPOWERMAN] = &Read235UPowerMan;
  
  sprintf(options[OPT_235UMASSCORE].cName,"d235UMassCore"); 
  sprintf(options[OPT_235UMASSCORE].cDescr,"Initial Core Mass of 235U");
  sprintf(options[OPT_235UMASSCORE].cDefault,"Primordial Earth: xxx");
  options[OPT_235UMASSCORE].iType = 2;
  options[OPT_235UMASSCORE].iMultiFile = 1;
  options[OPT_235UMASSCORE].dNeg = EMASSCORE235U;
  options[OPT_235UMASSCORE].dDefault = 0;
  sprintf(options[OPT_235UMASSCORE].cNeg,"Earth Masses");
  fnRead[OPT_235UMASSCORE] = &Read235UMassCore;
  
  sprintf(options[OPT_235UNUMCORE].cName,"d235UNumCore"); 
  sprintf(options[OPT_235UNUMCORE].cDescr,"Initial Core Number of 235U Atoms");
  sprintf(options[OPT_235UNUMCORE].cDefault,"1");
  options[OPT_235UNUMCORE].iType = 2;
  options[OPT_235UNUMCORE].iMultiFile = 1;
  options[OPT_235UNUMCORE].dNeg = ENUMCORE235U;
  options[OPT_235UNUMCORE].dDefault = 0;
  sprintf(options[OPT_235UNUMCORE].cNeg,"Primordial Earth 235U Numer");
  fnRead[OPT_235UNUMCORE] = &Read235UNumCore;

  sprintf(options[OPT_235UPOWERCORE].cName,"d235UPowerCore");  //section added PED
  sprintf(options[OPT_235UPOWERCORE].cDescr,"Initial Core Power Production from 235U Atoms");
  sprintf(options[OPT_235UPOWERCORE].cDefault,"Primordial Earth: xx TW");
  options[OPT_235UPOWERCORE].iType = 2;
  options[OPT_235UPOWERCORE].iMultiFile = 1;
  options[OPT_235UPOWERCORE].dNeg = EPOWERCORE235U; 
  options[OPT_235UPOWERCORE].dDefault = 0;
  sprintf(options[OPT_235UPOWERCORE].cNeg,"TW");
  fnRead[OPT_235UPOWERCORE] = &Read235UPowerCore;

  sprintf(options[OPT_235UMASSCRUST].cName,"d235UMassCrust"); 
  sprintf(options[OPT_235UMASSCRUST].cDescr,"Initial Crust Mass of 235U");
  sprintf(options[OPT_235UMASSCRUST].cDefault,"Primordial Earth: xxx");
  options[OPT_235UMASSCRUST].iType = 2;
  options[OPT_235UMASSCRUST].iMultiFile = 1;
  options[OPT_235UMASSCRUST].dNeg = EMASSCRUST235U;
  options[OPT_235UMASSCRUST].dDefault = 0;
  sprintf(options[OPT_235UMASSCRUST].cNeg,"Earth Masses");
  fnRead[OPT_235UMASSCRUST] = &Read235UMassCrust;
  
  sprintf(options[OPT_235UNUMCRUST].cName,"d235UNumCrust"); 
  sprintf(options[OPT_235UNUMCRUST].cDescr,"Initial Crust Number of 235U Atoms");
  sprintf(options[OPT_235UNUMCRUST].cDefault,"1");
  options[OPT_235UNUMCRUST].iType = 2;
  options[OPT_235UNUMCRUST].iMultiFile = 1;
  options[OPT_235UNUMCRUST].dNeg = ENUMCRUST235U;
  options[OPT_235UNUMCRUST].dDefault = 0;
  sprintf(options[OPT_235UNUMCRUST].cNeg,"Primordial Earth 235U Numer");
  fnRead[OPT_235UNUMCRUST] = &Read235UNumCrust;

  sprintf(options[OPT_235UPOWERCRUST].cName,"d235UPowerCrust");  //section added PED
  sprintf(options[OPT_235UPOWERCRUST].cDescr,"Initial Crust Power Production from 235U Atoms");
  sprintf(options[OPT_235UPOWERCRUST].cDefault,"Primordial Earth: xx TW");
  options[OPT_235UPOWERCRUST].iType = 2;
  options[OPT_235UPOWERCRUST].iMultiFile = 1;
  options[OPT_235UPOWERCRUST].dNeg = EPOWERCRUST235U; 
  options[OPT_235UPOWERCRUST].dDefault = 0;
  sprintf(options[OPT_235UPOWERCRUST].cNeg,"TW");
  fnRead[OPT_235UPOWERCRUST] = &Read235UPowerCrust;

  sprintf(options[OPT_HALT40KPOWER].cName,"dHalt40KPower");
  sprintf(options[OPT_HALT40KPOWER].cDescr,"Minimum 40K Power");
  sprintf(options[OPT_HALT40KPOWER].cDefault,"0");
  options[OPT_HALT40KPOWER].iType = 2;
  options[OPT_HALT40KPOWER].iMultiFile = 1;
  options[OPT_HALT40KPOWER].dNeg = 1e12; // TW
  options[OPT_HALT40KPOWER].dDefault = 0;
  sprintf(options[OPT_HALT40KPOWER].cNeg,"TW");
  fnRead[OPT_HALT40KPOWER] = &ReadHalt40KPower;

  sprintf(options[OPT_HALT232THPOWER].cName,"dHalt232ThPower");
  sprintf(options[OPT_HALT232THPOWER].cDescr,"Minimum 232Th Power");
  sprintf(options[OPT_HALT232THPOWER].cDefault,"0");
  options[OPT_HALT232THPOWER].iType = 2;
  options[OPT_HALT232THPOWER].iMultiFile = 1;
  options[OPT_HALT232THPOWER].dNeg = 1e12; // TW
  options[OPT_HALT232THPOWER].dDefault = 0;
  sprintf(options[OPT_HALT232THPOWER].cNeg,"TW");
  fnRead[OPT_HALT232THPOWER] = &ReadHalt232ThPower;

  sprintf(options[OPT_HALT235UPOWER].cName,"dHalt235UPower");
  sprintf(options[OPT_HALT235UPOWER].cDescr,"Minimum 235U Power");
  sprintf(options[OPT_HALT235UPOWER].cDefault,"0");
  options[OPT_HALT235UPOWER].iType = 2;
  options[OPT_HALT235UPOWER].iMultiFile = 1;
  options[OPT_HALT235UPOWER].dNeg = 1e12; // TW
  options[OPT_HALT235UPOWER].dDefault = 0;
  sprintf(options[OPT_HALT235UPOWER].cNeg,"TW");
  fnRead[OPT_HALT235UPOWER] = &ReadHalt235UPower;

  sprintf(options[OPT_HALT238UPOWER].cName,"dHalt238UPower");
  sprintf(options[OPT_HALT238UPOWER].cDescr,"Minimum 238U Power");
  sprintf(options[OPT_HALT238UPOWER].cDefault,"0");
  options[OPT_HALT238UPOWER].iType = 2;
  options[OPT_HALT238UPOWER].iMultiFile = 1;
  options[OPT_HALT238UPOWER].dNeg = 1e12; // TW
  options[OPT_HALT238UPOWER].dDefault = 0;
  sprintf(options[OPT_HALT238UPOWER].cNeg,"TW");
  fnRead[OPT_HALT238UPOWER] = &ReadHalt238UPower;

  sprintf(options[OPT_HALTRADPOWER].cName,"dHaltRadPower");
  sprintf(options[OPT_HALTRADPOWER].cDescr,"Minimum Total Radgiogenic Power");
  sprintf(options[OPT_HALTRADPOWER].cDefault,"0");
  options[OPT_HALTRADPOWER].iType = 2;
  options[OPT_HALTRADPOWER].iMultiFile = 1;
  options[OPT_HALTRADPOWER].dNeg = 1e12; // TW
  options[OPT_HALTRADPOWER].dDefault = 0;
  sprintf(options[OPT_HALTRADPOWER].cNeg,"TW");
  fnRead[OPT_HALTRADPOWER] = &ReadHaltRadPower;

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
void fvReadOptionsRadheat(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTRADHEAT;iOpt<OPTENDRADHEAT;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
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
void fvNotMassAndNum(OPTIONS *options,int iMass,int iNum,int iBody) {
  if (options[iMass].iLine[iBody] >= 0 && options[iNum].iLine[iBody] >= 0) {
    fprintf(stderr,"ERROR: Cannot set both %s and %s.\n",options[iMass].cName,options[iNum].cName);
    DoubleLineExit(options[iMass].cFile[iBody],options[iNum].cFile[iBody],options[iMass].iLine[iBody],options[iNum].iLine[iBody]);
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
void fvAssign26AlNum(BODY *body,OPTIONS *options,double dAge,int iBody) {
  /* Mantle */
  if (options[OPT_26ALMASSMAN].iLine[iBody+1] >= 0) {
    body[iBody].d26AlNumMan=body[iBody].d26AlMassMan/(MASS26AL);
  }
  if (options[OPT_26ALNUMMAN].iLine[iBody+1] >= 0) {
    // Do nothing bc default Num set.
  }
  if (options[OPT_26ALPOWERMAN].iLine[iBody+1] >= 0) {
    body[iBody].d26AlNumMan=body[iBody].d26AlPowerMan/(ENERGY26AL)*(HALFLIFE26AL);
  }  
  body[iBody].d26AlConstMan = fd26AlConstant(body[iBody].d26AlNumMan,dAge);  //Get the constant given num and age.
  
  /* Core */
  if (options[OPT_26ALMASSCORE].iLine[iBody+1] >= 0) {
    body[iBody].d26AlNumCore=body[iBody].d26AlMassCore/(MASS26AL);
  }
  if (options[OPT_26ALNUMCORE].iLine[iBody+1] >= 0) {
  }
  if (options[OPT_26ALPOWERCORE].iLine[iBody+1] >= 0) {
    body[iBody].d26AlNumCore=body[iBody].d26AlPowerCore/(ENERGY26AL)*(HALFLIFE26AL);
  }  
  body[iBody].d26AlConstCore = fd26AlConstant(body[iBody].d26AlNumCore,dAge);
}
/**
   Assign 40K number to body

   @param body Body struct
   @param options Options struct
   @param Age
   @param iBody Index of body
*/
void fvAssign40KNum(BODY *body,OPTIONS *options,double dAge,int iBody) {
  /* Mantle */
  if (options[OPT_40KMASSMAN].iLine[iBody+1] >= 0) {
    body[iBody].d40KNumMan=body[iBody].d40KMassMan/(MASS40K);
  }
  if (options[OPT_40KNUMMAN].iLine[iBody+1] >= 0) {
    // Do nothing bc default Num set.
  }
  if (options[OPT_40KPOWERMAN].iLine[iBody+1] >= 0) {
    body[iBody].d40KNumMan=body[iBody].d40KPowerMan/(ENERGY40K)*(HALFLIFE40K);
  }  
  body[iBody].d40KConstMan = fd40KConstant(body[iBody].d40KNumMan,dAge);  //Get the constant given num and age.
  
  /* Core */
  if (options[OPT_40KMASSCORE].iLine[iBody+1] >= 0) {
    body[iBody].d40KNumCore=body[iBody].d40KMassCore/(MASS40K);
  }
  if (options[OPT_40KNUMCORE].iLine[iBody+1] >= 0) {
  }
  if (options[OPT_40KPOWERCORE].iLine[iBody+1] >= 0) {
    body[iBody].d40KNumCore=body[iBody].d40KPowerCore/(ENERGY40K)*(HALFLIFE40K);
  }  
  body[iBody].d40KConstCore = fd40KConstant(body[iBody].d40KNumCore,dAge);

  /* Crust */
  if (options[OPT_40KMASSCRUST].iLine[iBody+1] >= 0) {
    body[iBody].d40KNumCrust=body[iBody].d40KMassCrust/(MASS40K);
  }
  if (options[OPT_40KNUMCRUST].iLine[iBody+1] >= 0) {
  }
  if (options[OPT_40KPOWERCRUST].iLine[iBody+1] >= 0) {
    body[iBody].d40KNumCrust=body[iBody].d40KPowerCrust/(ENERGY40K)*(HALFLIFE40K);
  }  
  body[iBody].d40KConstCrust = fd40KConstant(body[iBody].d40KNumCrust,dAge);
  
}
/**
   Assign 232Th number to body

   @param body Body struct
   @param options Options struct
   @param Age
   @param iBody Index of body
*/
void fvAssign232ThNum(BODY *body,OPTIONS *options,double dAge,int iBody) {
    /* Mantle */
    if (options[OPT_232THMASSMAN].iLine[iBody+1] >= 0) {
      body[iBody].d232ThNumMan=body[iBody].d232ThMassMan/(MASS232TH);
    }
    if (options[OPT_232THNUMMAN].iLine[iBody+1] >= 0) {      //Do nothing, use default.
    }
    if (options[OPT_232THPOWERMAN].iLine[iBody+1] >= 0) {
      body[iBody].d232ThNumMan=body[iBody].d232ThPowerMan/(ENERGY232TH)*(HALFLIFE232TH);
    }
    body[iBody].d232ThConstMan = fd232ThConstant(body[iBody].d232ThNumMan,dAge);

    /* Core */
    if (options[OPT_232THMASSCORE].iLine[iBody+1] >= 0) {
      body[iBody].d232ThNumCore=body[iBody].d232ThMassCore/(MASS232TH);
    }
    if (options[OPT_232THNUMCORE].iLine[iBody+1] >= 0) {      //Do nothing, use default.
    }
    if (options[OPT_232THPOWERCORE].iLine[iBody+1] >= 0) {
      body[iBody].d232ThNumCore=body[iBody].d232ThPowerCore/(ENERGY232TH)*(HALFLIFE232TH);
    }
    body[iBody].d232ThConstCore = fd232ThConstant(body[iBody].d232ThNumCore,dAge);

    /* Crust */
    if (options[OPT_232THMASSCRUST].iLine[iBody+1] >= 0) {
      body[iBody].d232ThNumCrust=body[iBody].d232ThMassCrust/(MASS232TH);
    }
    if (options[OPT_232THNUMCRUST].iLine[iBody+1] >= 0) {      //Do nothing, use default.
    }
    if (options[OPT_232THPOWERCRUST].iLine[iBody+1] >= 0) {
      body[iBody].d232ThNumCrust=body[iBody].d232ThPowerCrust/(ENERGY232TH)*(HALFLIFE232TH);
    }
    body[iBody].d232ThConstCrust = fd232ThConstant(body[iBody].d232ThNumCrust,dAge);
}
/**
   Assign 238U number to body

   @param body Body struct
   @param options Options struct
   @param Age
   @param iBody Index of body
*/
void fvAssign238UNum(BODY *body,OPTIONS *options,double dAge,int iBody) {
    /* Mantle */
    if (options[OPT_238UMASSMAN].iLine[iBody+1] >= 0) {
      body[iBody].d238UNumMan=body[iBody].d238UMassMan/(MASS238U);
    }
    if (options[OPT_238UNUMMAN].iLine[iBody+1] >= 0) {
    }
    if (options[OPT_238UPOWERMAN].iLine[iBody+1] >= 0) {
      body[iBody].d238UNumMan=body[iBody].d238UPowerMan/(ENERGY238U)*(HALFLIFE238U);
    }
    body[iBody].d238UConstMan = fd238UConstant(body[iBody].d238UNumMan,dAge);
    /* Core */
    if (options[OPT_238UMASSCORE].iLine[iBody+1] >= 0) {
      body[iBody].d238UNumCore=body[iBody].d238UMassCore/(MASS238U);
    }
    if (options[OPT_238UNUMCORE].iLine[iBody+1] >= 0) {
    }
    if (options[OPT_238UPOWERCORE].iLine[iBody+1] >= 0) {
      body[iBody].d238UNumCore=body[iBody].d238UPowerCore/(ENERGY238U)*(HALFLIFE238U);
    }
    body[iBody].d238UConstCore = fd238UConstant(body[iBody].d238UNumCore,dAge);
    /* Crust */
    if (options[OPT_238UMASSCRUST].iLine[iBody+1] >= 0) {
      body[iBody].d238UNumCrust=body[iBody].d238UMassCrust/(MASS238U);
    }
    if (options[OPT_238UNUMCRUST].iLine[iBody+1] >= 0) {
    }
    if (options[OPT_238UPOWERCRUST].iLine[iBody+1] >= 0) {
      body[iBody].d238UNumCrust=body[iBody].d238UPowerCrust/(ENERGY238U)*(HALFLIFE238U);
    }
    body[iBody].d238UConstCrust = fd238UConstant(body[iBody].d238UNumCrust,dAge);    
}
/**
   Assign 235U number to body

   @param body Body struct
   @param options Options struct
   @param Age
   @param iBody Index of body
*/
void fvAssign235UNum(BODY *body,OPTIONS *options,double dAge,int iBody) {  //PED
    /* Mantle */
  if (options[OPT_235UMASSMAN].iLine[iBody+1] >= 0) {
      body[iBody].d235UNumMan=body[iBody].d235UMassMan/(MASS235U);
  }
  if (options[OPT_235UNUMMAN].iLine[iBody+1] >= 0) {
  }
  if (options[OPT_235UPOWERMAN].iLine[iBody+1] >= 0) {
      body[iBody].d235UNumMan=body[iBody].d235UPowerMan/(ENERGY235U)*(HALFLIFE235U);
  }
  body[iBody].d235UConstMan = fd235UConstant(body[iBody].d235UNumMan,dAge);
  /* Core */
  if (options[OPT_235UMASSCORE].iLine[iBody+1] >= 0) {
      body[iBody].d235UNumCore=body[iBody].d235UMassCore/(MASS235U);
  }
  if (options[OPT_235UNUMCORE].iLine[iBody+1] >= 0) {
  }
  if (options[OPT_235UPOWERCORE].iLine[iBody+1] >= 0) {
      body[iBody].d235UNumCore=body[iBody].d235UPowerCore/(ENERGY235U)*(HALFLIFE235U);
  }
  body[iBody].d235UConstCore = fd235UConstant(body[iBody].d235UNumCore,dAge);
  /* Crust */
  if (options[OPT_235UMASSCRUST].iLine[iBody+1] >= 0) {
      body[iBody].d235UNumCrust=body[iBody].d235UMassCrust/(MASS235U);
  }
  if (options[OPT_235UNUMCRUST].iLine[iBody+1] >= 0) {
  }
  if (options[OPT_235UPOWERCRUST].iLine[iBody+1] >= 0) {
      body[iBody].d235UNumCrust=body[iBody].d235UPowerCrust/(ENERGY235U)*(HALFLIFE235U);
  }
  body[iBody].d235UConstCrust = fd235UConstant(body[iBody].d235UNumCrust,dAge);
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
void fvVerify26Al(BODY *body,OPTIONS *options,SYSTEM *system,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
  Assign26AlNum(body,options,dAge,iBody);

  /* Mantle */
  if (update[iBody].i26AlMan >= 0) {
    update[iBody].iaType[update[iBody].i26AlMan][0] = 1;
    update[iBody].iNumBodies[update[iBody].i26AlMan][0]=1;
    update[iBody].iaBody[update[iBody].i26AlMan][0] = malloc(update[iBody].iNumBodies[update[iBody].i26AlMan][0]*sizeof(int)); //iaBody is the number of bodies that are affected by this variable.
    update[iBody].iaBody[update[iBody].i26AlMan][0][0]=iBody;
    // Initialize derivative so radiogenic power is known at the beginning
    update[iBody].daDerivProc[update[iBody].i26AlMan][0] = fdD26AlNumManDt(body,system,update[iBody].iaBody[update[iBody].i26AlMan][0]);
    update[iBody].pdD26AlNumManDt = &update[iBody].daDerivProc[update[iBody].i26AlMan][0];
    fnUpdate[iBody][update[iBody].i26AlMan][0] = &fdD26AlNumManDt;
  } else
    // Set to zero, so RadPower calculations can still be used
    update[iBody].pdD26AlNumManDt = &update[iBody].dZero;

  /* Core */
  if (update[iBody].i26AlCore >= 0) {
    update[iBody].iaType[update[iBody].i26AlCore][0] = 1;
    update[iBody].iNumBodies[update[iBody].i26AlCore][0]=1;
    update[iBody].iaBody[update[iBody].i26AlCore][0] = malloc(update[iBody].iNumBodies[update[iBody].i26AlCore][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i26AlCore][0][0]=iBody;
    // Initialize derivative so radiogenic power is known at the beginning
    update[iBody].daDerivProc[update[iBody].i26AlCore][0] = fdD26AlNumCoreDt(body,system,update[iBody].iaBody[update[iBody].i26AlCore][0]);
    update[iBody].pdD26AlNumCoreDt = &update[iBody].daDerivProc[update[iBody].i26AlCore][0];
    fnUpdate[iBody][update[iBody].i26AlCore][0] = &fdD26AlNumCoreDt;
  } else
    update[iBody].pdD26AlNumCoreDt = &update[iBody].dZero;

}

void Verify40K(BODY *body,OPTIONS *options,SYSTEM *system,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
  Assign40KNum(body,options,dAge,iBody);
  /* Mantle */
  if (update[iBody].i40KMan >= 0) {
    update[iBody].iaType[update[iBody].i40KMan][0] = 1;
    update[iBody].iNumBodies[update[iBody].i40KMan][0]=1;
    update[iBody].iaBody[update[iBody].i40KMan][0] = malloc(update[iBody].iNumBodies[update[iBody].i40KMan][0]*sizeof(int)); //iaBody is the number of bodies that are affected by this variable.
    update[iBody].iaBody[update[iBody].i40KMan][0][0]=iBody;
    // Initialize derivative so radiogenic power is known at the beginning
    update[iBody].daDerivProc[update[iBody].i40KMan][0] = fdD40KNumManDt(body,system,update[iBody].iaBody[update[iBody].i40KMan][0]);
    update[iBody].pdD40KNumManDt = &update[iBody].daDerivProc[update[iBody].i40KMan][0];
    fnUpdate[iBody][update[iBody].i40KMan][0] = &fdD40KNumManDt;
  } else
    // Set to zero, so RadPower calculations can still be used
    update[iBody].pdD40KNumManDt = &update[iBody].dZero;

  /* Core */
  if (update[iBody].i40KCore >= 0) {
    update[iBody].iaType[update[iBody].i40KCore][0] = 1;
    update[iBody].iNumBodies[update[iBody].i40KCore][0]=1;
    update[iBody].iaBody[update[iBody].i40KCore][0] = malloc(update[iBody].iNumBodies[update[iBody].i40KCore][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i40KCore][0][0]=iBody;
    update[iBody].daDerivProc[update[iBody].i40KCore][0] = fdD40KNumCoreDt(body,system,update[iBody].iaBody[update[iBody].i40KCore][0]);
    update[iBody].pdD40KNumCoreDt = &update[iBody].daDerivProc[update[iBody].i40KCore][0];
    fnUpdate[iBody][update[iBody].i40KCore][0] = &fdD40KNumCoreDt;
  } else
    update[iBody].pdD40KNumCoreDt = &update[iBody].dZero;
  
  /* Crust */
  if (update[iBody].i40KCrust >= 0) {
    update[iBody].iaType[update[iBody].i40KCrust][0] = 1;
    update[iBody].iNumBodies[update[iBody].i40KCrust][0]=1;
    update[iBody].iaBody[update[iBody].i40KCrust][0] = malloc(update[iBody].iNumBodies[update[iBody].i40KCrust][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i40KCrust][0][0]=iBody;
    update[iBody].daDerivProc[update[iBody].i40KCrust][0] = fdD40KNumCrustDt(body,system,update[iBody].iaBody[update[iBody].i40KCrust][0]);
    update[iBody].pdD40KNumCrustDt = &update[iBody].daDerivProc[update[iBody].i40KCrust][0];
    fnUpdate[iBody][update[iBody].i40KCrust][0] = &fdD40KNumCrustDt;
  } else
    update[iBody].pdD40KNumCrustDt = &update[iBody].dZero;

}

void Verify232Th(BODY *body,OPTIONS *options,SYSTEM *system,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
  Assign232ThNum(body,options,dAge,iBody);
  /* Mantle */
  if (update[iBody].i232ThMan >= 0) {
    update[iBody].iaType[update[iBody].i232ThMan][0] = 1;
    update[iBody].iNumBodies[update[iBody].i232ThMan][0]=1;
    update[iBody].iaBody[update[iBody].i232ThMan][0] = malloc(update[iBody].iNumBodies[update[iBody].i232ThMan][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i232ThMan][0][0]=iBody;
    // Initialize derivative so radiogenic power is known at the beginning
    update[iBody].daDerivProc[update[iBody].i232ThMan][0] = fdD232ThNumManDt(body,system,update[iBody].iaBody[update[iBody].i232ThMan][0]);
    update[iBody].pdD232ThNumManDt = &update[iBody].daDerivProc[update[iBody].i232ThMan][0];
    fnUpdate[iBody][update[iBody].i232ThMan][0] = &fdD232ThNumManDt;
  } else
    update[iBody].pdD232ThNumManDt = &update[iBody].dZero;

  /* Core */
  if (update[iBody].i232ThCore >= 0) {
    update[iBody].iaType[update[iBody].i232ThCore][0] = 1;
    update[iBody].iNumBodies[update[iBody].i232ThCore][0]=1;
    update[iBody].iaBody[update[iBody].i232ThCore][0] = malloc(update[iBody].iNumBodies[update[iBody].i232ThCore][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i232ThCore][0][0]=iBody;
    update[iBody].daDerivProc[update[iBody].i232ThCore][0] = fdD232ThNumCoreDt(body,system,update[iBody].iaBody[update[iBody].i232ThCore][0]);
    update[iBody].pdD232ThNumCoreDt = &update[iBody].daDerivProc[update[iBody].i232ThCore][0];
    fnUpdate[iBody][update[iBody].i232ThCore][0] = &fdD232ThNumCoreDt;
  } else
    update[iBody].pdD232ThNumCoreDt = &update[iBody].dZero;

  /* Crust */
  if (update[iBody].i232ThCrust >= 0) {
    update[iBody].iaType[update[iBody].i232ThCrust][0] = 1;
    update[iBody].iNumBodies[update[iBody].i232ThCrust][0]=1;
    update[iBody].iaBody[update[iBody].i232ThCrust][0] = malloc(update[iBody].iNumBodies[update[iBody].i232ThCrust][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i232ThCrust][0][0]=iBody;
    update[iBody].daDerivProc[update[iBody].i232ThCrust][0] = fdD232ThNumCrustDt(body,system,update[iBody].iaBody[update[iBody].i232ThCrust][0]);
    update[iBody].pdD232ThNumCrustDt = &update[iBody].daDerivProc[update[iBody].i232ThCrust][0];
    fnUpdate[iBody][update[iBody].i232ThCrust][0] = &fdD232ThNumCrustDt;
  } else
    update[iBody].pdD232ThNumCrustDt = &update[iBody].dZero;
}

void Verify238U(BODY *body,OPTIONS *options,SYSTEM *system,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
  Assign238UNum(body,options,dAge,iBody);
  /* Mantle */
  if (update[iBody].i238UMan >= 0) {
    update[iBody].iaType[update[iBody].i238UMan][0] = 1;
    update[iBody].iNumBodies[update[iBody].i238UMan][0]=1;
    update[iBody].iaBody[update[iBody].i238UMan][0] = malloc(update[iBody].iNumBodies[update[iBody].i238UMan][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i238UMan][0][0]=iBody;
    // Initialize derivative so radiogenic power is known at the beginning
    update[iBody].daDerivProc[update[iBody].i238UMan][0] = fdD238UNumManDt(body,system,update[iBody].iaBody[update[iBody].i238UMan][0]);
    update[iBody].pdD238UNumManDt = &update[iBody].daDerivProc[update[iBody].i238UMan][0];
    fnUpdate[iBody][update[iBody].i238UMan][0] = &fdD238UNumManDt;
  } else
    update[iBody].pdD238UNumManDt = &update[iBody].dZero;

  /* Core */
  if (update[iBody].i238UCore >= 0) {
    update[iBody].iaType[update[iBody].i238UCore][0] = 1;
    update[iBody].iNumBodies[update[iBody].i238UCore][0]=1;
    update[iBody].iaBody[update[iBody].i238UCore][0] = malloc(update[iBody].iNumBodies[update[iBody].i238UCore][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i238UCore][0][0]=iBody;
    update[iBody].daDerivProc[update[iBody].i238UCore][0] = fdD238UNumCoreDt(body,system,update[iBody].iaBody[update[iBody].i238UCore][0]);
    update[iBody].pdD238UNumCoreDt = &update[iBody].daDerivProc[update[iBody].i238UCore][0];
    fnUpdate[iBody][update[iBody].i238UCore][0] = &fdD238UNumCoreDt;
  } else
    update[iBody].pdD238UNumCoreDt = &update[iBody].dZero;

  /* Crust */
  if (update[iBody].i238UCrust >= 0) {
    update[iBody].iaType[update[iBody].i238UCrust][0] = 1;
    update[iBody].iNumBodies[update[iBody].i238UCrust][0]=1;
    update[iBody].iaBody[update[iBody].i238UCrust][0] = malloc(update[iBody].iNumBodies[update[iBody].i238UCrust][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i238UCrust][0][0]=iBody;
    update[iBody].daDerivProc[update[iBody].i238UCrust][0] = fdD238UNumCrustDt(body,system,update[iBody].iaBody[update[iBody].i238UCrust][0]);
    update[iBody].pdD238UNumCrustDt = &update[iBody].daDerivProc[update[iBody].i238UCrust][0];
    fnUpdate[iBody][update[iBody].i238UCrust][0] = &fdD238UNumCrustDt;
  } else
    update[iBody].pdD238UNumCrustDt = &update[iBody].dZero;
}

void Verify235U(BODY *body,OPTIONS *options,SYSTEM *system,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
  Assign235UNum(body,options,dAge,iBody);
  /* Mantle */
  if (update[iBody].i235UMan >= 0) {
    update[iBody].iaType[update[iBody].i235UMan][0] = 1;
    update[iBody].iNumBodies[update[iBody].i235UMan][0]=1;
    update[iBody].iaBody[update[iBody].i235UMan][0] = malloc(update[iBody].iNumBodies[update[iBody].i235UMan][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i235UMan][0][0]=iBody;
    // Initialize derivative so radiogenic power is known at the beginning
    update[iBody].daDerivProc[update[iBody].i235UMan][0] = fdD235UNumManDt(body,system,update[iBody].iaBody[update[iBody].i235UMan][0]);
    update[iBody].pdD235UNumManDt = &update[iBody].daDerivProc[update[iBody].i235UMan][0];
    fnUpdate[iBody][update[iBody].i235UMan][0] = &fdD235UNumManDt;
  } else
    update[iBody].pdD235UNumManDt = &update[iBody].dZero;

  /* Core */
  if (update[iBody].i235UCore >= 0) {
    update[iBody].iaType[update[iBody].i235UCore][0] = 1;
    update[iBody].iNumBodies[update[iBody].i235UCore][0]=1;
    update[iBody].iaBody[update[iBody].i235UCore][0] = malloc(update[iBody].iNumBodies[update[iBody].i235UCore][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i235UCore][0][0]=iBody;
    update[iBody].pdD235UNumCoreDt = &update[iBody].daDerivProc[update[iBody].i235UCore][0];
    update[iBody].daDerivProc[update[iBody].i235UCore][0] = fdD235UNumCoreDt(body,system,update[iBody].iaBody[update[iBody].i235UCore][0]);
    fnUpdate[iBody][update[iBody].i235UCore][0] = &fdD235UNumCoreDt;
  } else
    update[iBody].pdD235UNumCoreDt = &update[iBody].dZero;

  /* Crust */
  if (update[iBody].i235UCrust >= 0) {
    update[iBody].iaType[update[iBody].i235UCrust][0] = 1;
    update[iBody].iNumBodies[update[iBody].i235UCrust][0]=1;
    update[iBody].iaBody[update[iBody].i235UCrust][0] = malloc(update[iBody].iNumBodies[update[iBody].i235UCrust][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i235UCrust][0][0]=iBody;
    update[iBody].pdD235UNumCrustDt = &update[iBody].daDerivProc[update[iBody].i235UCrust][0];
    update[iBody].daDerivProc[update[iBody].i235UCrust][0] = fdD235UNumCrustDt(body,system,update[iBody].iaBody[update[iBody].i235UCrust][0]);
    fnUpdate[iBody][update[iBody].i235UCrust][0] = &fdD235UNumCrustDt;
  } else
    update[iBody].pdD235UNumCrustDt = &update[iBody].dZero;
}

/* Auxs Props */
void PropsAuxRadheat(BODY *body,EVOLVE *evolve,UPDATE *update,int iBody) {
  body[iBody].dRadPowerMan=fdRadPowerMan(update,iBody);
  body[iBody].dRadPowerCore=fdRadPowerCore(update,iBody);
  body[iBody].dRadPowerCrust=fdRadPowerCrust(update,iBody);
  body[iBody].dRadPowerTotal=fdRadPowerTotal(body,iBody);
}

void fnForceBehaviorRadheat(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  if (body[iBody].d26AlNumMan < 0.5)
    body[iBody].d26AlNumMan = 0;
  if (body[iBody].d26AlNumCore < 0.5)
    body[iBody].d26AlNumCore = 0;
  
  if (body[iBody].d40KNumMan < 0.5)
    body[iBody].d40KNumMan = 0;
  if (body[iBody].d40KNumCore < 0.5)
    body[iBody].d40KNumCore = 0;
  if (body[iBody].d40KNumCrust < 0.5)
    body[iBody].d40KNumCrust = 0;
  
  if (body[iBody].d232ThNumMan < 0.5)
    body[iBody].d232ThNumMan = 0;
  if (body[iBody].d232ThNumCore < 0.5)
    body[iBody].d232ThNumCore = 0;
  if (body[iBody].d232ThNumCrust < 0.5)
    body[iBody].d232ThNumCrust = 0;

  if (body[iBody].d238UNumMan < 0.5)
    body[iBody].d238UNumMan = 0;
  if (body[iBody].d238UNumCore < 0.5)
    body[iBody].d238UNumCore = 0;
  if (body[iBody].d238UNumCrust < 0.5)
    body[iBody].d238UNumCrust = 0;

  if (body[iBody].d235UNumMan < 0.5)
    body[iBody].d235UNumMan = 0;
  if (body[iBody].d235UNumCore < 0.5)
    body[iBody].d235UNumCore = 0;
  if (body[iBody].d235UNumCrust < 0.5)
    body[iBody].d235UNumCrust = 0;
}

void RadheatExit(FILES *files,char cSpecies[16],int iFile) {
  fprintf(stderr,"ERROR: Radheat called, but no %s option provided.\n",cSpecies);
  fprintf(stderr,"\tFile: %s\n",files->Infile[iFile].cIn);
  exit(EXIT_INPUT);
}

void VerifyRadheat(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int iFile=iBody+1;

  /* Cannot set 2 or more of Power, Mass and Number for any isotope */
  /* XXX Need a VerifyOneOfThree subroutine */
  /* Radheat is active for this body if this subroutine is called. */

  // XXX This looks like it's insufficient to capture all the permutations
  NotMassAndNum(options,OPT_26ALMASSMAN,OPT_26ALNUMMAN,iBody);

  // 26Al set properly
  Verify26Al(body,options,system,update,body[iBody].dAge,fnUpdate,iBody);  //Verify Man and Core.

  // XXX This looks like it's insufficient to capture all the permutations
  NotMassAndNum(options,OPT_40KMASSMAN,OPT_40KNUMMAN,iBody);

  // 40K set properly
  Verify40K(body,options,system,update,body[iBody].dAge,fnUpdate,iBody);  //Verify Man and Core.

  // 232Th
  // XXX Also insufficient?
  NotMassAndNum(options,OPT_232THMASSMAN,OPT_232THNUMMAN,iFile);

  // 232Th set corectly
  Verify232Th(body,options,system,update,body[iBody].dAge,fnUpdate,iBody);

  // 238U
  NotMassAndNum(options,OPT_238UMASSMAN,OPT_238UNUMMAN,iFile);

  // 238U set correctly
  Verify238U(body,options,system,update,body[iBody].dAge,fnUpdate,iBody);

  // 235U
  NotMassAndNum(options,OPT_235UMASSMAN,OPT_235UNUMMAN,iFile);

  // 235U set correctly
  Verify235U(body,options,system,update,body[iBody].dAge,fnUpdate,iBody);

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorRadheat;
  control->fnPropsAux[iBody][iModule] = &PropsAuxRadheat;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyRadheat;
  //  output[OUT_SURFENFLUXRADTOTAL].fnOutput[iBody][iModule] = &fdSurfEnFluxRadTotal;   //PD: Is this right?
}


/**************** RADHEAT update ****************/

void InitializeUpdateRadheat(BODY *body,UPDATE *update,int iBody) {
  /* Initially allow all radiogenic heat sources to be present. If any are 0, 
     or < dMinRadPower, they will me removed from update[iBody] in 
     ForceBehavior.
  */

  // Mantle
  if (body[iBody].d26AlNumMan > 0 || body[iBody].d26AlMassMan > 0 || body[iBody].d26AlPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum26AlMan++;
  }

  if (body[iBody].d40KNumMan > 0 || body[iBody].d40KMassMan > 0 || body[iBody].d40KPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum40KMan++;
  }

  if (body[iBody].d232ThNumMan > 0 || body[iBody].d232ThMassMan > 0 || body[iBody].d232ThPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum232ThMan++;
  }

  if (body[iBody].d238UNumMan > 0 || body[iBody].d238UMassMan > 0 || body[iBody].d238UPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum238UMan++;
  }

  if (body[iBody].d235UNumMan > 0 || body[iBody].d235UMassMan > 0 || body[iBody].d235UPowerMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum235UMan++;
  }

  // Core
  if (body[iBody].d26AlNumCore > 0 || body[iBody].d26AlMassCore > 0 || body[iBody].d26AlPowerCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum26AlCore++;
  }

  if (body[iBody].d40KNumCore > 0 || body[iBody].d40KMassCore > 0 || body[iBody].d40KPowerCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum40KCore++;
  }

  if (body[iBody].d232ThNumCore > 0 || body[iBody].d232ThMassCore > 0 || body[iBody].d232ThPowerCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum232ThCore++;
  }

  if (body[iBody].d238UNumCore > 0 || body[iBody].d238UMassCore > 0 || body[iBody].d238UPowerCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum238UCore++;
  }

  if (body[iBody].d235UNumCore > 0 || body[iBody].d235UMassCore > 0 || body[iBody].d235UPowerCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum235UCore++;
  }

  // Crust
  if (body[iBody].d40KNumCrust > 0 || body[iBody].d40KMassCrust > 0 || body[iBody].d40KPowerCrust > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum40KCrust++;
  }

  if (body[iBody].d232ThNumCrust > 0 || body[iBody].d232ThMassCrust > 0 || body[iBody].d232ThPowerCrust > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum232ThCrust++;
  }

  if (body[iBody].d238UNumCrust > 0 || body[iBody].d238UMassCrust > 0 || body[iBody].d238UPowerCrust > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum238UCrust++;
  }

  if (body[iBody].d235UNumCrust > 0 || body[iBody].d235UMassCrust > 0 || body[iBody].d235UPowerCrust > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNum235UCrust++;
  }
}

//PED: Combine these into ..HeatMan?  and ..HeatCore?
void FinalizeUpdate26AlNumManRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum26AlMan = (*iEqn)++;
}

void FinalizeUpdate40KNumManRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum40KMan = (*iEqn)++;
}

void FinalizeUpdate232ThNumManRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum232ThMan = (*iEqn)++;
}

void FinalizeUpdate238UNumManRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum238UMan = (*iEqn)++;
}

void FinalizeUpdate235UNumManRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {  //PED
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum235UMan = (*iEqn)++;
}

void FinalizeUpdate26AlNumCoreRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum26AlCore = (*iEqn)++;
}

void FinalizeUpdate40KNumCoreRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
    update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
    update[iBody].iNum40KCore = (*iEqn)++;
}

void FinalizeUpdate232ThNumCoreRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum232ThCore = (*iEqn)++;
}

void FinalizeUpdate238UNumCoreRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum238UCore = (*iEqn)++;
}

void FinalizeUpdate235UNumCoreRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {  //PED
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum235UCore = (*iEqn)++;
}

void FinalizeUpdate40KNumCrustRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
    update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
    update[iBody].iNum40KCrust = (*iEqn)++;
}
void FinalizeUpdate232ThNumCrustRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum232ThCrust = (*iEqn)++;
}
void FinalizeUpdate238UNumCrustRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum238UCrust = (*iEqn)++;
}
void FinalizeUpdate235UNumCrustRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = RADHEAT;
  update[iBody].iNum235UCrust = (*iEqn)++;
}

/***************** RADHEAT Halts *****************/

/* Minimum 40K Powering? */
int fbHaltMin40KPower(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (fd40KPower(update,iBody) < halt->dMin40KPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 40K Power =  ",body[iBody].cName);
      fprintd(stdout,fd40KPower(update,iBody),io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMin40KPower,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        

/* Minimum 232Th Powering? */

int fbHaltMin232ThPower(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (fd232ThPower(update,iBody) < halt->dMin232ThPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 232Th Power =  ",body[iBody].cName);
      fprintd(stdout,fd232ThPower(update,iBody),io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMin232ThPower,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        

/* Minimum 238U Powering? */

int fbHaltMin238UPower(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (fd238UPower(update,iBody) < halt->dMin238UPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 238U Power =  ",body[iBody].cName);
      fprintd(stdout,fd238UPower(update,iBody),io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMin238UPower,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        

int fbHaltMin235UPower(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (fd235UPower(update,iBody) < halt->dMin235UPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 235U Power =  ",body[iBody].cName);
      fprintd(stdout,fd235UPower(update,iBody),io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMin235UPower,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        

/* Minimum Radiogenic Power */

int fbHaltMinRadPower(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  int iFoo;
  iFoo = fdRadPowerTotal(body,iBody);

  if (fdRadPowerTotal(body,iBody) < halt->dMinRadPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's Total Radiogenic Power =  ",body[iBody].cName);
      fprintd(stdout,fdRadPowerTotal(body,iBody),io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMinRadPower,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        


void CountHaltsRadHeat(HALT *halt,int *iNumHalts) {
  if (halt->dMin40KPower > 0)
    (*iNumHalts)++;
  if (halt->dMin232ThPower > 0)
    (*iNumHalts)++;
  if (halt->dMin238UPower > 0)
    (*iNumHalts)++;
  if (halt->dMin235UPower > 0)  //PED
    (*iNumHalts)++;
  if (halt->dMinRadPower > 0)
    (*iNumHalts)++;
}

void VerifyHaltRadheat(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {

  if (control->Halt[iBody].dMin40KPower > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin40KPower;
  if (control->Halt[iBody].dMin232ThPower > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin232ThPower;
  if (control->Halt[iBody].dMin238UPower > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin238UPower;
  if (control->Halt[iBody].dMin235UPower > 0)  //PED
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin235UPower;
  if (control->Halt[iBody].dMinRadPower > 0) 
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMinRadPower;
}

/************* RADHEAT Outputs ******************/

/* NOTE: If you write a new Write subroutine here you need to add the associate 
   block of initialization in InitializeOutputRadheat below */

/* Aluminum */

void Write26AlPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 26Al
  *dTmp = -(*(update[iBody].pdD26AlNumManDt))*ENERGY26Al;  */
  *dTmp = fd26AlPowerMan(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { 
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void Write26AlEnFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get surface heat flux from 26Al */
  *dTmp = fd26AlEnFlux(body,update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteD26AlNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = *(update[iBody].pdD26AlNumManDt);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void Write26AlTimescale(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}

void Write26AlMassMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d26AlNumMan*MASS26AL;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

//PED: this subr is not finished?
void WriteD26AlPowerDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = -1;
}

void Write26AlNumMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d26AlNumMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Core */
void Write26AlPowerCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 26Al */
  *dTmp = fd26AlPowerCore(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { 
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void Write26AlMassCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d26AlNumCore*MASS26AL;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}
void Write26AlNumCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d26AlNumCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Potassium */

void Write40KPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 40K
  *dTmp = -(*(update[iBody].pdD40KNumManDt))*ENERGY40K;  */
  *dTmp = fd40KPowerMan(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { 
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void Write40KEnFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get surface heat flux from 40K */
  *dTmp = fd40KEnFlux(body,update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteD40KNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = *(update[iBody].pdD40KNumManDt);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}

void Write40KTimescale(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}

void Write40KMassMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d40KNumMan*MASS40K;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

//PED: this subr is not finished?
void WriteD40KPowerDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = -1;
}

void Write40KNumMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d40KNumMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Core */
void Write40KPowerCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 40K */
  *dTmp = fd40KPowerCore(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { 
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void Write40KMassCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d40KNumCore*MASS40K;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}
void Write40KNumCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d40KNumCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Crust */
void Write40KPowerCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 40K */
  *dTmp = fd40KPowerCrust(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { 
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void Write40KMassCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d40KNumCrust*MASS40K;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}
void Write40KNumCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d40KNumCrust;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Thorium */
void Write232ThEnFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get surface heat flux from 232Th */
  *dTmp = fd232ThEnFlux(body,update,iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}
void WriteD232ThNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = *(update[iBody].pdD232ThNumManDt);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}
void WritedD232ThPowerDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}
void Write232ThTimescale(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}

void Write232ThPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total heat from 232Th */
  *dTmp = fd232ThPowerMan(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void Write232ThMassMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d232ThNumMan*MASS232TH;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}
void Write232ThNumMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d232ThNumMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Core */
void Write232ThPowerCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total heat from 232Th */
  *dTmp = fd232ThPowerCore(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void Write232ThMassCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d232ThNumCore*MASS232TH;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}
void Write232ThNumCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d232ThNumCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Crust */
void Write232ThPowerCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total heat from 232Th */
  *dTmp = fd232ThPowerCrust(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void Write232ThMassCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d232ThNumCrust*MASS232TH;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}
void Write232ThNumCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d232ThNumCrust;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Uranium 238 */
void Write238UEnFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get surface heat flux from 238U */
  *dTmp = fd238UEnFlux(body,update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}
void WriteD238UNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = *(update[iBody].pdD238UNumManDt);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}
void WritedD238UPowerDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}
void Write238UTimescale(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}

/* Mantle */
void Write238UPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 238U */
  *dTmp = fd238UPowerMan(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void Write238UMassMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d238UNumMan*MASS238U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}
void Write238UNumMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d238UNumMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Core */
void Write238UPowerCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 238U */
  *dTmp = fd238UPowerCore(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void Write238UMassCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d238UNumCore*MASS238U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}
void Write238UNumCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d238UNumCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Crust */
void Write238UPowerCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 238U */
  *dTmp = fd238UPowerCrust(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void Write238UMassCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d238UNumCrust*MASS238U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}
void Write238UNumCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d238UNumCrust;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Uranium 235 PED */
void Write235UEnFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get surface heat flux from 235U */
  *dTmp = fd235UEnFlux(body,update,iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}
void WriteD235UNumDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = *(update[iBody].pdD235UNumManDt);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime,cUnit);
  }
}
void WritedD235UPowerDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}
void Write235UTimescale(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp=-1;
}

/* Mantle */
void Write235UPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 235U */
  *dTmp = -(*(update[iBody].pdD235UNumManDt))*ENERGY235U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void Write235UMassMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d235UNumMan*MASS235U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}
void Write235UNumMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d235UNumMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Core */
void Write235UPowerCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 235U */
  *dTmp = fd235UPowerCore(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void Write235UMassCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d235UNumCore*MASS235U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}
void Write235UNumCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d235UNumCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Crust */
void Write235UPowerCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 235U */
  *dTmp = fd235UPowerCrust(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void Write235UMassCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d235UNumCrust*MASS235U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}
void Write235UNumCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].d235UNumCrust;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  }
  /* else nothing, as it is a number */
}

/* Totals */
void WriteRadPowerCrust(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Radiogenic Power Production in crust*/
  *dTmp = fdRadPowerCrust(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void WriteRadPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Radiogenic Power Production in mantle*/
  *dTmp = fdRadPowerMan(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void WriteRadPowerCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Total Radiogenic Power Production in core*/
  *dTmp = fdRadPowerCore(update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void WriteRadPowerTotal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Total Radiogenic Power Production */
  *dTmp = body[iBody].dRadPowerTotal;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void WriteSurfEnFluxRadTotal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Radiogenic Surface Energy Flux */
  *dTmp = fdSurfEnFluxRadTotal(body,system,update,iBody,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void InitializeOutputRadheat(OUTPUT *output,fnWriteOutput fnWrite[]) {

  /* Aluminum */
    //  PED:  Do these default numbers matter??  If so they need to be changed.
  sprintf(output[OUT_26ALPOWERMAN].cName,"26AlPowerMan");
  sprintf(output[OUT_26ALPOWERMAN].cDescr,"Total Power Generated by 26Al");
  sprintf(output[OUT_26ALPOWERMAN].cNeg,"TW");
  output[OUT_26ALPOWERMAN].bNeg = 1;
  output[OUT_26ALPOWERMAN].dNeg = 1e-12; 
  output[OUT_26ALPOWERMAN].iNum = 1;
  output[OUT_26ALPOWERMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALPOWERMAN] = &Write26AlPowerMan;
  
  sprintf(output[OUT_26ALENFLUX].cName,"26AlEnFlux");
  sprintf(output[OUT_26ALENFLUX].cDescr,"Surface Energy Flux from 26Al");
  sprintf(output[OUT_26ALENFLUX].cNeg,"W/m^2");
  output[OUT_26ALENFLUX].bNeg = 1;
  output[OUT_26ALENFLUX].dNeg = 1;
  output[OUT_26ALENFLUX].iNum = 1;
  output[OUT_26ALENFLUX].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALENFLUX] = &Write26AlEnFlux;

  sprintf(output[OUT_26ALDPOWERDT].cName,"D26AlPowerDt");
  sprintf(output[OUT_26ALDPOWERDT].cDescr,"Time Rate of Change of 26Al Power Generation");
  sprintf(output[OUT_26ALDPOWERDT].cNeg,"TW/Gyr");
  output[OUT_26ALDPOWERDT].bNeg = 1;
  output[OUT_26ALDPOWERDT].dNeg = 1e12/1e9; /* XXX */
  output[OUT_26ALDPOWERDT].iNum = 1;
  output[OUT_26ALDPOWERDT].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALDPOWERDT] = &WriteD26AlPowerDt;  //PED: that subr is not written yet?
  
  sprintf(output[OUT_26ALDNUMDT].cName,"D26AlNumManDt");
  sprintf(output[OUT_26ALDNUMDT].cDescr,"Time Rate of Change of 26Al Heat Generation");
  sprintf(output[OUT_26ALDNUMDT].cNeg,"/Gyr");
  output[OUT_26ALDNUMDT].bNeg = 1;
  output[OUT_26ALDNUMDT].dNeg = YEARSEC*1e9; 
  output[OUT_26ALDNUMDT].iNum = 1;
  output[OUT_26ALDNUMDT].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALDNUMDT] = &WriteD26AlNumDt;
  
  sprintf(output[OUT_26ALTIME].cName,"26AlTimescale");
  sprintf(output[OUT_26ALTIME].cDescr,"Timescale for 26Al Power Generation");
  sprintf(output[OUT_26ALTIME].cNeg,"Gyr");
  output[OUT_26ALTIME].bNeg = 1;
  output[OUT_26ALTIME].dNeg = YEARSEC*1e9;
  output[OUT_26ALTIME].iNum = 1;
  output[OUT_26ALTIME].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALTIME] = &Write26AlTimescale;
  
  sprintf(output[OUT_26ALMASSMAN].cName,"26AlMassMan");
  sprintf(output[OUT_26ALMASSMAN].cDescr,"Mass of Mantle in 26Al");
  sprintf(output[OUT_26ALMASSMAN].cNeg,"Earth Masses");
  output[OUT_26ALMASSMAN].bNeg = 1;
  output[OUT_26ALMASSMAN].dNeg = 1./MEARTH;
  output[OUT_26ALMASSMAN].iNum = 1;
  output[OUT_26ALMASSMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALMASSMAN] = &Write26AlMassMan;
  
  sprintf(output[OUT_26ALNUMMAN].cName,"26AlNumMan");
  sprintf(output[OUT_26ALNUMMAN].cDescr,"Total Number of 26Al Atoms");
  sprintf(output[OUT_26ALNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_26ALNUMMAN].bNeg = 1;
  output[OUT_26ALNUMMAN].dNeg = 1;
  output[OUT_26ALNUMMAN].iNum = 1;
  output[OUT_26ALNUMMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALNUMMAN] = &Write26AlNumMan;

  /* Core */
  sprintf(output[OUT_26ALPOWERCORE].cName,"26AlPowerCore");
  sprintf(output[OUT_26ALPOWERCORE].cDescr,"Core Power Generated by 26Al");
  sprintf(output[OUT_26ALPOWERCORE].cNeg,"TW");
  output[OUT_26ALPOWERCORE].bNeg = 1;
  output[OUT_26ALPOWERCORE].dNeg = 1e-12; // ergs/s -> TW 
  output[OUT_26ALPOWERCORE].iNum = 1;
  output[OUT_26ALPOWERCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALPOWERCORE] = &Write26AlPowerCore;
  
  sprintf(output[OUT_26ALMASSCORE].cName,"26AlMassCore");
  sprintf(output[OUT_26ALMASSCORE].cDescr,"Mass of core in 26Al");
  sprintf(output[OUT_26ALMASSCORE].cNeg,"Earth Masses");
  output[OUT_26ALMASSCORE].bNeg = 1;
  output[OUT_26ALMASSCORE].dNeg = 1./MEARTH;
  output[OUT_26ALMASSCORE].iNum = 1;
  output[OUT_26ALMASSCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALMASSCORE] = &Write26AlMassCore;
  
  sprintf(output[OUT_26ALNUMCORE].cName,"26AlNumCore");
  sprintf(output[OUT_26ALNUMCORE].cDescr,"Total Core Number of 26Al Atoms");
  sprintf(output[OUT_26ALNUMCORE].cNeg,"Initial Primordial Earth Number");
  output[OUT_26ALNUMCORE].bNeg = 1;
  output[OUT_26ALNUMCORE].dNeg = EMASSCORE26AL/MASS26AL;
  output[OUT_26ALNUMCORE].iNum = 1;
  output[OUT_26ALNUMCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_26ALNUMCORE] = &Write26AlNumCore;
  
  /* Potassium */
    //  PED:  Do these default numbers matter??  If so they need to be changed.
  sprintf(output[OUT_40KENFLUX].cName,"40KEnFlux");
  sprintf(output[OUT_40KENFLUX].cDescr,"Surface Energy Flux from 40K");
  sprintf(output[OUT_40KENFLUX].cNeg,"W/m^2");
  output[OUT_40KENFLUX].bNeg = 1;
  output[OUT_40KENFLUX].dNeg = 1;
  output[OUT_40KENFLUX].iNum = 1;
  output[OUT_40KENFLUX].iModuleBit = RADHEAT;
  fnWrite[OUT_40KENFLUX] = &Write40KEnFlux;

  sprintf(output[OUT_40KDPOWERDT].cName,"D40KPowerDt");
  sprintf(output[OUT_40KDPOWERDT].cDescr,"Time Rate of Change of 40K Power Generation");
  sprintf(output[OUT_40KDPOWERDT].cNeg,"TW/Gyr");
  output[OUT_40KDPOWERDT].bNeg = 1;
  output[OUT_40KDPOWERDT].dNeg = 1e12/1e9; /* XXX */
  output[OUT_40KDPOWERDT].iNum = 1;
  output[OUT_40KDPOWERDT].iModuleBit = RADHEAT;
  fnWrite[OUT_40KDPOWERDT] = &WriteD40KPowerDt;  //PED: that subr is not written yet?
  
  sprintf(output[OUT_40KDNUMDT].cName,"D40KNumManDt");
  sprintf(output[OUT_40KDNUMDT].cDescr,"Time Rate of Change of 40K Heat Generation");
  sprintf(output[OUT_40KDNUMDT].cNeg,"/Gyr");
  output[OUT_40KDNUMDT].bNeg = 1;
  output[OUT_40KDNUMDT].dNeg = YEARSEC*1e9; 
  output[OUT_40KDNUMDT].iNum = 1;
  output[OUT_40KDNUMDT].iModuleBit = RADHEAT;
  fnWrite[OUT_40KDNUMDT] = &WriteD40KNumDt;
  
  sprintf(output[OUT_40KTIME].cName,"40KTimescale");
  sprintf(output[OUT_40KTIME].cDescr,"Timescale for 40K Power Generation");
  sprintf(output[OUT_40KTIME].cNeg,"Gyr");
  output[OUT_40KTIME].bNeg = 1;
  output[OUT_40KTIME].dNeg = YEARSEC*1e9;
  output[OUT_40KTIME].iNum = 1;
  output[OUT_40KTIME].iModuleBit = RADHEAT;
  fnWrite[OUT_40KTIME] = &Write40KTimescale;

  sprintf(output[OUT_40KPOWERMAN].cName,"40KPowerMan");
  sprintf(output[OUT_40KPOWERMAN].cDescr,"Total Power Generated by 40K");
  sprintf(output[OUT_40KPOWERMAN].cNeg,"TW");
  output[OUT_40KPOWERMAN].bNeg = 1;
  output[OUT_40KPOWERMAN].dNeg = 1e-12; 
  output[OUT_40KPOWERMAN].iNum = 1;
  output[OUT_40KPOWERMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_40KPOWERMAN] = &Write40KPowerMan;
  
  sprintf(output[OUT_40KMASSMAN].cName,"40KMassMan");
  sprintf(output[OUT_40KMASSMAN].cDescr,"Total Mass of 40K");
  sprintf(output[OUT_40KMASSMAN].cNeg,"Earth Masses");
  output[OUT_40KMASSMAN].bNeg = 1;
  output[OUT_40KMASSMAN].dNeg = MEARTH;
  output[OUT_40KMASSMAN].iNum = 1;
  output[OUT_40KMASSMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_40KMASSMAN] = &Write40KMassMan;
  
  sprintf(output[OUT_40KNUMMAN].cName,"40KNumMan");
  sprintf(output[OUT_40KNUMMAN].cDescr,"Total Number of 40K Atoms");
  sprintf(output[OUT_40KNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_40KNUMMAN].bNeg = 1;
  output[OUT_40KNUMMAN].dNeg = EMASSMAN40K/MASS40K;
  output[OUT_40KNUMMAN].iNum = 1;
  output[OUT_40KNUMMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_40KNUMMAN] = &Write40KNumMan;

  /* Core */
  sprintf(output[OUT_40KPOWERCORE].cName,"40KPowerCore");
  sprintf(output[OUT_40KPOWERCORE].cDescr,"Core Power Generated by 40K");
  sprintf(output[OUT_40KPOWERCORE].cNeg,"TW");
  output[OUT_40KPOWERCORE].bNeg = 1;
  output[OUT_40KPOWERCORE].dNeg = 1e-12; // ergs/s -> TW 
  output[OUT_40KPOWERCORE].iNum = 1;
  output[OUT_40KPOWERCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_40KPOWERCORE] = &Write40KPowerCore;
  
  sprintf(output[OUT_40KMASSCORE].cName,"40KMassCore");
  sprintf(output[OUT_40KMASSCORE].cDescr,"Total Core Mass of 40K");
  sprintf(output[OUT_40KMASSCORE].cNeg,"Earth Masses");
  output[OUT_40KMASSCORE].bNeg = 1;
  output[OUT_40KMASSCORE].dNeg = MEARTH;
  output[OUT_40KMASSCORE].iNum = 1;
  output[OUT_40KMASSCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_40KMASSCORE] = &Write40KMassCore;
  
  sprintf(output[OUT_40KNUMCORE].cName,"40KNumCore");
  sprintf(output[OUT_40KNUMCORE].cDescr,"Total Core Number of 40K Atoms");
  sprintf(output[OUT_40KNUMCORE].cNeg,"Initial Primordial Earth Number");
  output[OUT_40KNUMCORE].bNeg = 1;
  output[OUT_40KNUMCORE].dNeg = EMASSCORE40K/MASS40K;
  output[OUT_40KNUMCORE].iNum = 1;
  output[OUT_40KNUMCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_40KNUMCORE] = &Write40KNumCore;

  /* Crust */
  sprintf(output[OUT_40KPOWERCRUST].cName,"40KPowerCrust");
  sprintf(output[OUT_40KPOWERCRUST].cDescr,"Crust Power Generated by 40K");
  sprintf(output[OUT_40KPOWERCRUST].cNeg,"TW");
  output[OUT_40KPOWERCRUST].bNeg = 1;
  output[OUT_40KPOWERCRUST].dNeg = 1e-12; // ergs/s -> TW 
  output[OUT_40KPOWERCRUST].iNum = 1;
  output[OUT_40KPOWERCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_40KPOWERCRUST] = &Write40KPowerCrust;
  
  sprintf(output[OUT_40KMASSCRUST].cName,"40KMassCrust");
  sprintf(output[OUT_40KMASSCRUST].cDescr,"Total Crust Mass of 40K");
  sprintf(output[OUT_40KMASSCRUST].cNeg,"Earth Masses");
  output[OUT_40KMASSCRUST].bNeg = 1;
  output[OUT_40KMASSCRUST].dNeg = MEARTH;
  output[OUT_40KMASSCRUST].iNum = 1;
  output[OUT_40KMASSCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_40KMASSCRUST] = &Write40KMassCrust;
  
  sprintf(output[OUT_40KNUMCRUST].cName,"40KNumCrust");
  sprintf(output[OUT_40KNUMCRUST].cDescr,"Total Crust Number of 40K Atoms");
  sprintf(output[OUT_40KNUMCRUST].cNeg,"Initial Primordial Earth Number");
  output[OUT_40KNUMCRUST].bNeg = 1;
  output[OUT_40KNUMCRUST].dNeg = EMASSCRUST40K/MASS40K;
  output[OUT_40KNUMCRUST].iNum = 1;
  output[OUT_40KNUMCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_40KNUMCRUST] = &Write40KNumCrust;
  
  /* Thorium */
  sprintf(output[OUT_232THPOWERMAN].cName,"232ThPowerMan");
  sprintf(output[OUT_232THPOWERMAN].cDescr,"Total Power Generated by 232Th");
  sprintf(output[OUT_232THPOWERMAN].cNeg,"TW");
  output[OUT_232THPOWERMAN].bNeg = 1;
  output[OUT_232THPOWERMAN].dNeg = 1e-12; 
  output[OUT_232THPOWERMAN].iNum = 1;
  output[OUT_232THPOWERMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_232THPOWERMAN] = &Write232ThPowerMan;
  
  sprintf(output[OUT_232THENFLUX].cName,"232ThEnFlux");
  sprintf(output[OUT_232THENFLUX].cDescr,"Surface Energy Flux from 232Th");
  sprintf(output[OUT_232THENFLUX].cNeg,"W/m^2");
  output[OUT_232THENFLUX].bNeg = 1;
  output[OUT_232THENFLUX].dNeg = 1;
  output[OUT_232THENFLUX].iNum = 1;
  output[OUT_232THENFLUX].iModuleBit = RADHEAT;
  fnWrite[OUT_232THENFLUX] = &Write232ThEnFlux;

  sprintf(output[OUT_232THDNUMDT].cName,"D232ThNumManDt");
  sprintf(output[OUT_232THDNUMDT].cDescr,"Time Rate of Change of the number of 232Th nuclei");
  sprintf(output[OUT_232THDNUMDT].cNeg,"/Gyr");
  output[OUT_232THDNUMDT].bNeg = 1;
  output[OUT_232THDNUMDT].dNeg = YEARSEC*1e9;  
  output[OUT_232THDNUMDT].iNum = 1;
  output[OUT_232THDNUMDT].iModuleBit = RADHEAT;
  fnWrite[OUT_232THDNUMDT] = &WriteD232ThNumDt;

  sprintf(output[OUT_232THTIME].cName,"232ThTimescale");
  sprintf(output[OUT_232THTIME].cDescr,"Timescale for 232Th Power Generation");
  sprintf(output[OUT_232THTIME].cNeg,"Gyr");
  output[OUT_232THTIME].bNeg = 1;
  output[OUT_232THTIME].dNeg = YEARSEC*1e9;
  output[OUT_232THTIME].iNum = 1;
  output[OUT_232THTIME].iModuleBit = RADHEAT;
  fnWrite[OUT_232THTIME] = &Write232ThTimescale;
  
  sprintf(output[OUT_232THMASSMAN].cName,"232ThMassMan");
  sprintf(output[OUT_232THMASSMAN].cDescr,"Total Mass of 232Th");
  sprintf(output[OUT_232THMASSMAN].cNeg,"Earth Masses");
  output[OUT_232THMASSMAN].bNeg = 1;
  output[OUT_232THMASSMAN].dNeg = MEARTH;
  output[OUT_232THMASSMAN].iNum = 1;
  output[OUT_232THMASSMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_232THMASSMAN] = &Write232ThMassMan;
  
  sprintf(output[OUT_232THNUMMAN].cName,"232ThNumMan");
  sprintf(output[OUT_232THNUMMAN].cDescr,"Total Number of 232Th Atoms");
  sprintf(output[OUT_232THNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_232THNUMMAN].bNeg = 1;
  output[OUT_232THNUMMAN].dNeg = EMASSMAN232TH/MASS232TH;
  output[OUT_232THNUMMAN].iNum = 1;
  output[OUT_232THNUMMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_232THNUMMAN] = &Write232ThNumMan;

  /* Core */
  sprintf(output[OUT_232THPOWERCORE].cName,"232ThPowerCore");
  sprintf(output[OUT_232THPOWERCORE].cDescr,"Total Power Generated by 232Th");
  sprintf(output[OUT_232THPOWERCORE].cNeg,"TW");
  output[OUT_232THPOWERCORE].bNeg = 1;
  output[OUT_232THPOWERCORE].dNeg = 1e-12; 
  output[OUT_232THPOWERCORE].iNum = 1;
  output[OUT_232THPOWERCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_232THPOWERCORE] = &Write232ThPowerCore;

  sprintf(output[OUT_232THMASSCORE].cName,"232ThMassCore");
  sprintf(output[OUT_232THMASSCORE].cDescr,"Total Mass of 232Th");
  sprintf(output[OUT_232THMASSCORE].cNeg,"Earth Masses");
  output[OUT_232THMASSCORE].bNeg = 1;
  output[OUT_232THMASSCORE].dNeg = MEARTH;
  output[OUT_232THMASSCORE].iNum = 1;
  output[OUT_232THMASSCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_232THMASSCORE] = &Write232ThMassCore;
  
  sprintf(output[OUT_232THNUMCORE].cName,"232ThNumCore");
  sprintf(output[OUT_232THNUMCORE].cDescr,"Total Number of 232Th Atoms");
  sprintf(output[OUT_232THNUMCORE].cNeg,"Initial Primordial Earth Number");
  output[OUT_232THNUMCORE].bNeg = 1;
  output[OUT_232THNUMCORE].dNeg = EMASSCORE232TH/MASS232TH;
  output[OUT_232THNUMCORE].iNum = 1;
  output[OUT_232THNUMCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_232THNUMCORE] = &Write232ThNumCore;
    
  /* Crust */
  sprintf(output[OUT_232THPOWERCRUST].cName,"232ThPowerCrust");
  sprintf(output[OUT_232THPOWERCRUST].cDescr,"Total Power Generated by 232Th");
  sprintf(output[OUT_232THPOWERCRUST].cNeg,"TW");
  output[OUT_232THPOWERCRUST].bNeg = 1;
  output[OUT_232THPOWERCRUST].dNeg = 1e-12; 
  output[OUT_232THPOWERCRUST].iNum = 1;
  output[OUT_232THPOWERCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_232THPOWERCRUST] = &Write232ThPowerCrust;

  sprintf(output[OUT_232THMASSCRUST].cName,"232ThMassCrust");
  sprintf(output[OUT_232THMASSCRUST].cDescr,"Total Mass of 232Th");
  sprintf(output[OUT_232THMASSCRUST].cNeg,"Earth Masses");
  output[OUT_232THMASSCRUST].bNeg = 1;
  output[OUT_232THMASSCRUST].dNeg = MEARTH;
  output[OUT_232THMASSCRUST].iNum = 1;
  output[OUT_232THMASSCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_232THMASSCRUST] = &Write232ThMassCrust;
  
  sprintf(output[OUT_232THNUMCRUST].cName,"232ThNumCrust");
  sprintf(output[OUT_232THNUMCRUST].cDescr,"Total Number of 232Th Atoms");
  sprintf(output[OUT_232THNUMCRUST].cNeg,"Initial Primordial Earth Number");
  output[OUT_232THNUMCRUST].bNeg = 1;
  output[OUT_232THNUMCRUST].dNeg = EMASSCRUST232TH/MASS232TH;
  output[OUT_232THNUMCRUST].iNum = 1;
  output[OUT_232THNUMCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_232THNUMCRUST] = &Write232ThNumCrust;
  
  /* Uranium 238 */

  sprintf(output[OUT_238UPOWERMAN].cName,"238UPowerMan");
  sprintf(output[OUT_238UPOWERMAN].cDescr,"Total Power Generated by 238U");
  sprintf(output[OUT_238UPOWERMAN].cNeg,"TW");
  output[OUT_238UPOWERMAN].bNeg = 1;
  output[OUT_238UPOWERMAN].dNeg = 1e-12; 
  output[OUT_238UPOWERMAN].iNum = 1;
  output[OUT_238UPOWERMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_238UPOWERMAN] = &Write238UPowerMan;
  
  sprintf(output[OUT_238UENFLUX].cName,"238UEnFlux");
  sprintf(output[OUT_238UENFLUX].cDescr,"Surface Energy Flux due to 238U");
  sprintf(output[OUT_238UENFLUX].cNeg,"W/m^2");
  output[OUT_238UENFLUX].bNeg = 1;
  output[OUT_238UENFLUX].dNeg = 1;
  output[OUT_238UENFLUX].iNum = 1;
  output[OUT_238UENFLUX].iModuleBit = RADHEAT;
  fnWrite[OUT_238UENFLUX] = &Write238UEnFlux;

  sprintf(output[OUT_238UDNUMDT].cName,"D238UNumManDt");
  sprintf(output[OUT_238UDNUMDT].cDescr,"Time Rate of Change of the Number of 238U Nuclei");
  sprintf(output[OUT_238UDNUMDT].cNeg,"Gyr");
  output[OUT_238UDNUMDT].bNeg = 1;
  output[OUT_238UDNUMDT].dNeg = YEARSEC*1e9;
  output[OUT_238UDNUMDT].iNum = 1;
  output[OUT_238UDNUMDT].iModuleBit = RADHEAT;
  fnWrite[OUT_238UDNUMDT] = &WriteD238UNumDt;

  sprintf(output[OUT_238UTIME].cName,"238UTimescale");
  sprintf(output[OUT_238UTIME].cDescr,"Timescale for 238U Power Generation");
  sprintf(output[OUT_238UTIME].cNeg,"Gyr");
  output[OUT_238UTIME].bNeg = 1;
  output[OUT_238UTIME].dNeg = YEARSEC*1e9;
  output[OUT_238UTIME].iNum = 1;
  output[OUT_238UTIME].iModuleBit = RADHEAT;
  fnWrite[OUT_238UTIME] = &Write238UTimescale;
  
  sprintf(output[OUT_238UMASSMAN].cName,"238UMassMan");
  sprintf(output[OUT_238UMASSMAN].cDescr,"Total Mass of 238U");
  sprintf(output[OUT_238UMASSMAN].cNeg,"Earth Masses");
  output[OUT_238UMASSMAN].bNeg = 1;
  output[OUT_238UMASSMAN].dNeg = MEARTH;
  output[OUT_238UMASSMAN].iNum = 1;
  output[OUT_238UMASSMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_238UMASSMAN] = &Write238UMassMan;
  
  sprintf(output[OUT_238UNUMMAN].cName,"238UNumMan");
  sprintf(output[OUT_238UNUMMAN].cDescr,"Total Number of 238U Atoms");
  sprintf(output[OUT_238UNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_238UNUMMAN].bNeg = 1;
  output[OUT_238UNUMMAN].dNeg = 1;
  output[OUT_238UNUMMAN].iNum = 1;
  output[OUT_238UNUMMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_238UNUMMAN] = &Write238UNumMan;

  /* Core */
  sprintf(output[OUT_238UPOWERCORE].cName,"238UPowerCore");
  sprintf(output[OUT_238UPOWERCORE].cDescr,"Total Core Power Generated by 238U");
  sprintf(output[OUT_238UPOWERCORE].cNeg,"TW");
  output[OUT_238UPOWERCORE].bNeg = 1;
  output[OUT_238UPOWERCORE].dNeg = 1e-12; 
  output[OUT_238UPOWERCORE].iNum = 1;
  output[OUT_238UPOWERCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_238UPOWERCORE] = &Write238UPowerCore;
  
  sprintf(output[OUT_238UMASSCORE].cName,"238UMassCore");
  sprintf(output[OUT_238UMASSCORE].cDescr,"Total Core Mass of 238U");
  sprintf(output[OUT_238UMASSCORE].cNeg,"Earth Masses");
  output[OUT_238UMASSCORE].bNeg = 1;
  output[OUT_238UMASSCORE].dNeg = MEARTH;
  output[OUT_238UMASSCORE].iNum = 1;
  output[OUT_238UMASSCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_238UMASSCORE] = &Write238UMassCore;
  
  sprintf(output[OUT_238UNUMCORE].cName,"238UNumCore");
  sprintf(output[OUT_238UNUMCORE].cDescr,"Total Core Number of 238U Atoms");
  sprintf(output[OUT_238UNUMCORE].cNeg,"Initial Primordial Earth Number");
  output[OUT_238UNUMCORE].bNeg = 1;
  output[OUT_238UNUMCORE].dNeg = 1;
  output[OUT_238UNUMCORE].iNum = 1;
  output[OUT_238UNUMCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_238UNUMCORE] = &Write238UNumCore;
  
  /* Crust */
  sprintf(output[OUT_238UPOWERCRUST].cName,"238UPowerCrust");
  sprintf(output[OUT_238UPOWERCRUST].cDescr,"Total Crust Power Generated by 238U");
  sprintf(output[OUT_238UPOWERCRUST].cNeg,"TW");
  output[OUT_238UPOWERCRUST].bNeg = 1;
  output[OUT_238UPOWERCRUST].dNeg = 1e-12; 
  output[OUT_238UPOWERCRUST].iNum = 1;
  output[OUT_238UPOWERCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_238UPOWERCRUST] = &Write238UPowerCrust;
  
  sprintf(output[OUT_238UMASSCRUST].cName,"238UMassCrust");
  sprintf(output[OUT_238UMASSCRUST].cDescr,"Total Crust Mass of 238U");
  sprintf(output[OUT_238UMASSCRUST].cNeg,"Earth Masses");
  output[OUT_238UMASSCRUST].bNeg = 1;
  output[OUT_238UMASSCRUST].dNeg = MEARTH;
  output[OUT_238UMASSCRUST].iNum = 1;
  output[OUT_238UMASSCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_238UMASSCRUST] = &Write238UMassCrust;
  
  sprintf(output[OUT_238UNUMCRUST].cName,"238UNumCrust");
  sprintf(output[OUT_238UNUMCRUST].cDescr,"Total Crust Number of 238U Atoms");
  sprintf(output[OUT_238UNUMCRUST].cNeg,"Initial Primordial Earth Number");
  output[OUT_238UNUMCRUST].bNeg = 1;
  output[OUT_238UNUMCRUST].dNeg = 1;
  output[OUT_238UNUMCRUST].iNum = 1;
  output[OUT_238UNUMCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_238UNUMCRUST] = &Write238UNumCrust;
  
  /* Uranium 235 */
  
  sprintf(output[OUT_235UPOWERMAN].cName,"235UPowerMan");
  sprintf(output[OUT_235UPOWERMAN].cDescr,"Total Power Generated by 235U");
  sprintf(output[OUT_235UPOWERMAN].cNeg,"TW");
  output[OUT_235UPOWERMAN].bNeg = 1;
  output[OUT_235UPOWERMAN].dNeg = 1e-12; 
  output[OUT_235UPOWERMAN].iNum = 1;
  output[OUT_235UPOWERMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_235UPOWERMAN] = &Write235UPowerMan;
  
  sprintf(output[OUT_235UENFLUX].cName,"235UEnFlux");
  sprintf(output[OUT_235UENFLUX].cDescr,"Surface Energy Flux due to 235U");
  sprintf(output[OUT_235UENFLUX].cNeg,"W/m^2");
  output[OUT_235UENFLUX].bNeg = 1;
  output[OUT_235UENFLUX].dNeg = 1e-3;
  output[OUT_235UENFLUX].iNum = 1;
  output[OUT_235UENFLUX].iModuleBit = RADHEAT;
  fnWrite[OUT_235UENFLUX] = &Write235UEnFlux;

  sprintf(output[OUT_235UDNUMDT].cName,"D235UNumManDt");
  sprintf(output[OUT_235UDNUMDT].cDescr,"Time Rate of Change of the Number of 235U Nuclei");
  sprintf(output[OUT_235UDNUMDT].cNeg,"Gyr");
  output[OUT_235UDNUMDT].bNeg = 1;
  output[OUT_235UDNUMDT].dNeg = YEARSEC*1e9;
  output[OUT_235UDNUMDT].iNum = 1;
  output[OUT_235UDNUMDT].iModuleBit = RADHEAT;
  fnWrite[OUT_235UDNUMDT] = &WriteD235UNumDt;

  sprintf(output[OUT_235UTIME].cName,"235UTimescale");
  sprintf(output[OUT_235UTIME].cDescr,"Timescale for 235U Power Generation");
  sprintf(output[OUT_235UTIME].cNeg,"Gyr");
  output[OUT_235UTIME].bNeg = 1;
  output[OUT_235UTIME].dNeg = YEARSEC*1e9;
  output[OUT_235UTIME].iNum = 1;
  output[OUT_235UTIME].iModuleBit = RADHEAT;
  fnWrite[OUT_235UTIME] = &Write235UTimescale;
  
  sprintf(output[OUT_235UMASSMAN].cName,"235UMassMan");
  sprintf(output[OUT_235UMASSMAN].cDescr,"Total Mass of 235U");
  sprintf(output[OUT_235UMASSMAN].cNeg,"Earth Masses");
  output[OUT_235UMASSMAN].bNeg = 1;
  output[OUT_235UMASSMAN].dNeg = MEARTH;
  output[OUT_235UMASSMAN].iNum = 1;
  output[OUT_235UMASSMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_235UMASSMAN] = &Write235UMassMan;
  
  sprintf(output[OUT_235UNUMMAN].cName,"235UNumMan");
  sprintf(output[OUT_235UNUMMAN].cDescr,"Total Number of 235U Atoms");
  sprintf(output[OUT_235UNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_235UNUMMAN].bNeg = 1;
  output[OUT_235UNUMMAN].dNeg = 1;
  output[OUT_235UNUMMAN].iNum = 1;
  output[OUT_235UNUMMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_235UNUMMAN] = &Write235UNumMan;

  /* Core */
  sprintf(output[OUT_235UPOWERCORE].cName,"235UPowerCore");
  sprintf(output[OUT_235UPOWERCORE].cDescr,"Total Core Power Generated by 235U");
  sprintf(output[OUT_235UPOWERCORE].cNeg,"TW");
  output[OUT_235UPOWERCORE].bNeg = 1;
  output[OUT_235UPOWERCORE].dNeg = 1e-12; 
  output[OUT_235UPOWERCORE].iNum = 1;
  output[OUT_235UPOWERCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_235UPOWERCORE] = &Write235UPowerCore;

  sprintf(output[OUT_235UMASSCORE].cName,"235UMassCore");
  sprintf(output[OUT_235UMASSCORE].cDescr,"Total Core Mass of 235U");
  sprintf(output[OUT_235UMASSCORE].cNeg,"Earth Masses");
  output[OUT_235UMASSCORE].bNeg = 1;
  output[OUT_235UMASSCORE].dNeg = MEARTH;
  output[OUT_235UMASSCORE].iNum = 1;
  output[OUT_235UMASSCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_235UMASSCORE] = &Write235UMassCore;
  
  sprintf(output[OUT_235UNUMCORE].cName,"235UNumCore");
  sprintf(output[OUT_235UNUMCORE].cDescr,"Total Core Number of 235U Atoms");
  sprintf(output[OUT_235UNUMCORE].cNeg,"Initial Primordial Earth Number");
  output[OUT_235UNUMCORE].bNeg = 1;
  output[OUT_235UNUMCORE].dNeg = 1;
  output[OUT_235UNUMCORE].iNum = 1;
  output[OUT_235UNUMCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_235UNUMCORE] = &Write235UNumCore;

  /* Crust */
  sprintf(output[OUT_235UPOWERCRUST].cName,"235UPowerCrust");
  sprintf(output[OUT_235UPOWERCRUST].cDescr,"Total Crust Power Generated by 235U");
  sprintf(output[OUT_235UPOWERCRUST].cNeg,"TW");
  output[OUT_235UPOWERCRUST].bNeg = 1;
  output[OUT_235UPOWERCRUST].dNeg = 1e-12; 
  output[OUT_235UPOWERCRUST].iNum = 1;
  output[OUT_235UPOWERCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_235UPOWERCRUST] = &Write235UPowerCrust;

  sprintf(output[OUT_235UMASSCRUST].cName,"235UMassCrust");
  sprintf(output[OUT_235UMASSCRUST].cDescr,"Total Crust Mass of 235U");
  sprintf(output[OUT_235UMASSCRUST].cNeg,"Earth Masses");
  output[OUT_235UMASSCRUST].bNeg = 1;
  output[OUT_235UMASSCRUST].dNeg = MEARTH;
  output[OUT_235UMASSCRUST].iNum = 1;
  output[OUT_235UMASSCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_235UMASSCRUST] = &Write235UMassCrust;
  
  sprintf(output[OUT_235UNUMCRUST].cName,"235UNumCrust");
  sprintf(output[OUT_235UNUMCRUST].cDescr,"Total Crust Number of 235U Atoms");
  sprintf(output[OUT_235UNUMCRUST].cNeg,"Initial Primordial Earth Number");
  output[OUT_235UNUMCRUST].bNeg = 1;
  output[OUT_235UNUMCRUST].dNeg = 1;
  output[OUT_235UNUMCRUST].iNum = 1;
  output[OUT_235UNUMCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_235UNUMCRUST] = &Write235UNumCrust;
      
  /* Totals */
  
  sprintf(output[OUT_RADPOWERCRUST].cName,"RadPowerCrust");
  sprintf(output[OUT_RADPOWERCRUST].cDescr,"Total Power Generated by Radiogenic Nuclides in the crust");
  sprintf(output[OUT_RADPOWERCRUST].cNeg,"TW");
  output[OUT_RADPOWERCRUST].bNeg = 1;
  output[OUT_RADPOWERCRUST].dNeg = 1e-12; 
  output[OUT_RADPOWERCRUST].iNum = 1;
  output[OUT_RADPOWERCRUST].iModuleBit = RADHEAT;
  fnWrite[OUT_RADPOWERCRUST] = &WriteRadPowerCrust;

  sprintf(output[OUT_RADPOWERMAN].cName,"RadPowerMan");
  sprintf(output[OUT_RADPOWERMAN].cDescr,"Total Power Generated by Radiogenic Nuclides in the mantle");
  sprintf(output[OUT_RADPOWERMAN].cNeg,"TW");
  output[OUT_RADPOWERMAN].bNeg = 1;
  output[OUT_RADPOWERMAN].dNeg = 1e-12; 
  output[OUT_RADPOWERMAN].iNum = 1;
  output[OUT_RADPOWERMAN].iModuleBit = RADHEAT;
  fnWrite[OUT_RADPOWERMAN] = &WriteRadPowerMan;

  sprintf(output[OUT_RADPOWERCORE].cName,"RadPowerCore");
  sprintf(output[OUT_RADPOWERCORE].cDescr,"Total Power Generated by Radiogenic Nuclides in Core");
  sprintf(output[OUT_RADPOWERCORE].cNeg,"TW");
  output[OUT_RADPOWERCORE].bNeg = 1;
  output[OUT_RADPOWERCORE].dNeg = 1e-12; 
  output[OUT_RADPOWERCORE].iNum = 1;
  output[OUT_RADPOWERCORE].iModuleBit = RADHEAT;
  fnWrite[OUT_RADPOWERCORE] = &WriteRadPowerCore;

  sprintf(output[OUT_RADPOWERTOTAL].cName,"RadPowerTotal");
  sprintf(output[OUT_RADPOWERTOTAL].cDescr,"Total Power Generated by Radiogenic Nuclides in Total (M+C)");
  sprintf(output[OUT_RADPOWERTOTAL].cNeg,"TW");
  output[OUT_RADPOWERTOTAL].bNeg = 1;
  output[OUT_RADPOWERTOTAL].dNeg = 1e-12; 
  output[OUT_RADPOWERTOTAL].iNum = 1;
  output[OUT_RADPOWERTOTAL].iModuleBit = RADHEAT;
  fnWrite[OUT_RADPOWERTOTAL] = &WriteRadPowerTotal;

  sprintf(output[OUT_SURFENFLUXRADTOTAL].cName,"SurfEnFluxRadTotal");
  sprintf(output[OUT_SURFENFLUXRADTOTAL].cDescr,"Total Surface Heat Flux Generated by Radiogenic Nuclides");
  sprintf(output[OUT_SURFENFLUXRADTOTAL].cNeg,"W/m^2");
  output[OUT_SURFENFLUXRADTOTAL].bNeg = 1;
  output[OUT_SURFENFLUXRADTOTAL].dNeg = 1; 
  output[OUT_SURFENFLUXRADTOTAL].iNum = 1;
  output[OUT_SURFENFLUXRADTOTAL].iModuleBit = RADHEAT;
  fnWrite[OUT_SURFENFLUXRADTOTAL] = &WriteSurfEnFluxRadTotal;

}

/************ RADHEAT Logging Functions **************/

void LogOptionsRadheat(CONTROL *control, FILE *fp) {
  /* Anything here?
  fprintf(fp,"-------- RADHEAT Options -----\n\n");
  */
}

//PED: this would be for global rad heat parameters, but this is blank bc rad is only relevant to each individual body.
void LogRadheat(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  /* Anything here? 
  int iOut;
  fprintf(fp,"\n----- RADHEAT PARAMETERS ------\n");
  for (iOut=OUTSTARTRADHEAT;iOut<OUTBODYSTARTRADHEAT;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyRadheat(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- RADHEAT PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTSTARTRADHEAT;iOut<OUTENDRADHEAT;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
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

void AddModuleRadheat(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = RADHEAT;

  module->fnCountHalts[iBody][iModule] = &CountHaltsRadHeat;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsRadheat;
  module->fnLogBody[iBody][iModule] = &LogBodyRadheat;
  module->fnVerify[iBody][iModule] = &VerifyRadheat;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltRadheat;

  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateRadheat;

  module->fnFinalizeUpdate26AlNumMan[iBody][iModule] = &FinalizeUpdate26AlNumManRadheat;
  module->fnFinalizeUpdate40KNumMan[iBody][iModule] = &FinalizeUpdate40KNumManRadheat;
  module->fnFinalizeUpdate232ThNumMan[iBody][iModule] = &FinalizeUpdate232ThNumManRadheat;
  module->fnFinalizeUpdate238UNumMan[iBody][iModule] = &FinalizeUpdate238UNumManRadheat;
  module->fnFinalizeUpdate235UNumMan[iBody][iModule] = &FinalizeUpdate235UNumManRadheat;

  module->fnFinalizeUpdate26AlNumCore[iBody][iModule] = &FinalizeUpdate26AlNumCoreRadheat;
  module->fnFinalizeUpdate40KNumCore[iBody][iModule] = &FinalizeUpdate40KNumCoreRadheat;
  module->fnFinalizeUpdate232ThNumCore[iBody][iModule] = &FinalizeUpdate232ThNumCoreRadheat;
  module->fnFinalizeUpdate238UNumCore[iBody][iModule] = &FinalizeUpdate238UNumCoreRadheat;
  module->fnFinalizeUpdate235UNumCore[iBody][iModule] = &FinalizeUpdate235UNumCoreRadheat;

  module->fnFinalizeUpdate40KNumCrust[iBody][iModule] = &FinalizeUpdate40KNumCrustRadheat;
  module->fnFinalizeUpdate232ThNumCrust[iBody][iModule] = &FinalizeUpdate232ThNumCrustRadheat;
  module->fnFinalizeUpdate238UNumCrust[iBody][iModule] = &FinalizeUpdate238UNumCrustRadheat;
  module->fnFinalizeUpdate235UNumCrust[iBody][iModule] = &FinalizeUpdate235UNumCrustRadheat;
}

/************* RADHEAT Functions ************/
// FIRST batch of subroutines are general for any species.
// N = N_0 * exp(-t/lambda)
// dN/dt = -(N_0/lambda) * exp(-t/lambda)

double fdRadPowerMan(UPDATE *update,int iBody) {
  /*  return -(*(update[iBody].pdD238UNumManDt))*ENERGY238U - (*(update[iBody].pdD235UNumManDt))*ENERGY235U - (*(update[iBody].pdD232ThNumManDt))*ENERGY232TH - (*(update[iBody].pdD40KNumManDt))*ENERGY40K;
   */
  return fd26AlPowerMan(update,iBody) + fd40KPowerMan(update,iBody) + fd232ThPowerMan(update,iBody) + fd238UPowerMan(update,iBody) + fd235UPowerMan(update,iBody);
}

double fdRadPowerCore(UPDATE *update,int iBody) {
  /*
  return -(*(update[iBody].pdD238UNumCoreDt))*ENERGY238U - (*(update[iBody].pdD235UNumCoreDt))*ENERGY235U - (*(update[iBody].pdD232ThNumCoreDt))*ENERGY232TH - (*(update[iBody].pdD40KNumCoreDt))*ENERGY40K;
  */
  return fd26AlPowerCore(update,iBody) + fd40KPowerCore(update,iBody) + fd232ThPowerCore(update,iBody) + fd238UPowerCore(update,iBody) + fd235UPowerCore(update,iBody);
}

double fdRadPowerCrust(UPDATE *update,int iBody) {
  /*
  return -(*(update[iBody].pdD238UNumCrustDt))*ENERGY238U - (*(update[iBody].pdD235UNumCrustDt))*ENERGY235U - (*(update[iBody].pdD232ThNumCrustDt))*ENERGY232TH - (*(update[iBody].pdD40KNumCrustDt))*ENERGY40K;
  */
  return fd40KPowerCrust(update,iBody) + fd232ThPowerCrust(update,iBody) + fd238UPowerCrust(update,iBody) + fd235UPowerCrust(update,iBody);
}

//PD: Note total doesn't need 'update', only 'body', which has the reservoir powers.
double fdRadPowerTotal(BODY *body,int iBody) {
  return body[iBody].dRadPowerMan + body[iBody].dRadPowerCore + body[iBody].dRadPowerCrust;
}

/* This is part of output[OUT_SURFENFLUX].fnOutput */
double fdSurfEnFluxRadTotal(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  //  return (fdRadPowerCrust(update,iBody) + fdRadPowerTot(update,iBody))/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
  // PD: why does above say crust+radpowertotal when radpowertotal already includes crust?
  //    return (fdRadPowerTot(update,iBody))/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
  return (body[iBody].dRadPowerTotal)/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
}

double fdRadheatConst(double dNum,double dAge,double dHalfLife) {
  return dNum/(exp(-dAge/dHalfLife));
}

double fdDNumRadDt(double dConst,double dHalfLife,double dAge) {  //dN/dt, can be used by any radioactive system?
  return -dConst/dHalfLife*exp(-dAge/dHalfLife);
}

//SECOND batch of subroutines are for individual variables of species.
/* Constant coefficients */
double fd26AlConstant(double dNum,double dAge) {  
    return fdRadheatConst(dNum,dAge,HALFLIFE26AL);   //redirects to fdRadheatConst
}

double fd40KConstant(double dNum,double dAge) {  
    return fdRadheatConst(dNum,dAge,HALFLIFE40K);   //redirects to fdRadheatConst
}
double fd232ThConstant(double dNum,double dAge) {  //PED: changed dPower to dNum.
  return fdRadheatConst(dNum,dAge,HALFLIFE232TH);  //redirects to fdRadheatConst
}
double fd238UConstant(double dNum,double dAge) {  //PED: changed dPower to dNum.
  return fdRadheatConst(dNum,dAge,HALFLIFE238U);  //redirects to fdRadheatConst
}
double fd235UConstant(double dNum,double dAge) {  //PED: changed dPower to dNum.
  return fdRadheatConst(dNum,dAge,HALFLIFE235U);  //redirects to fdRadheatConst
}

double fd26AlPowerMan(UPDATE *update,int iBody) {
  return -(*(update[iBody].pdD26AlNumManDt))*ENERGY26AL;
}

double fd40KPowerMan(UPDATE *update,int iBody) {
  return -(*(update[iBody].pdD40KNumManDt))*ENERGY40K;
}
double fd232ThPowerMan(UPDATE *update,int iBody) {
  return -(*(update[iBody].pdD232ThNumManDt))*ENERGY232TH;
}
double fd238UPowerMan(UPDATE *update,int iBody) {
  return  -(*(update[iBody].pdD238UNumManDt))*ENERGY238U;
}
double fd235UPowerMan(UPDATE *update,int iBody) {
  return  -(*(update[iBody].pdD235UNumManDt))*ENERGY235U;
}

double fd26AlPowerCore(UPDATE *update,int iBody) {
  return -(*(update[iBody].pdD26AlNumCoreDt))*ENERGY26AL;
}

double fd40KPowerCore(UPDATE *update,int iBody) {
  return -(*(update[iBody].pdD40KNumCoreDt))*ENERGY40K;
}
double fd232ThPowerCore(UPDATE *update,int iBody) {
  return -(*(update[iBody].pdD232ThNumCoreDt))*ENERGY232TH;
}
double fd238UPowerCore(UPDATE *update,int iBody) {
  return  -(*(update[iBody].pdD238UNumCoreDt))*ENERGY238U;
}
double fd235UPowerCore(UPDATE *update,int iBody) {
  return  -(*(update[iBody].pdD235UNumCoreDt))*ENERGY235U;
}

double fd40KPowerCrust(UPDATE *update,int iBody) {
  return -(*(update[iBody].pdD40KNumCrustDt))*ENERGY40K;
}
double fd232ThPowerCrust(UPDATE *update,int iBody) {
  return -(*(update[iBody].pdD232ThNumCrustDt))*ENERGY232TH;
}
double fd238UPowerCrust(UPDATE *update,int iBody) {
  return  -(*(update[iBody].pdD238UNumCrustDt))*ENERGY238U;
}
double fd235UPowerCrust(UPDATE *update,int iBody) {
  return  -(*(update[iBody].pdD235UNumCrustDt))*ENERGY235U;
}

double fd26AlPower(UPDATE *update,int iBody) {
  return fd26AlPowerMan(update,iBody) + fd40KPowerCore(update,iBody);
}

double fd40KPower(UPDATE *update,int iBody) {
  return fd40KPowerMan(update,iBody) + fd40KPowerCore(update,iBody) + fd40KPowerCrust(update,iBody);
}

double fd232ThPower(UPDATE *update,int iBody) {
  return fd232ThPowerMan(update,iBody) + fd232ThPowerCore(update,iBody) + fd232ThPowerCrust(update,iBody);
}

double fd238UPower(UPDATE *update,int iBody) {
  return fd238UPowerMan(update,iBody) + fd238UPowerCore(update,iBody) + fd238UPowerCrust(update,iBody);
}

double fd235UPower(UPDATE *update,int iBody) {
  return fd235UPowerMan(update,iBody) + fd235UPowerCore(update,iBody) + fd235UPowerCrust(update,iBody);
}

/* Energy Flux */
double fd26AlEnFlux(BODY *body,UPDATE *update,int iBody) {
  return fd40KPower(update,iBody)/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
}

double fd40KEnFlux(BODY *body,UPDATE *update,int iBody) {
  return fd40KPower(update,iBody)/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
}
double fd232ThEnFlux(BODY *body,UPDATE *update,int iBody) {
  return fd232ThPower(update,iBody)/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
}
double fd238UEnFlux(BODY *body,UPDATE *update,int iBody) {
  return fd238UPower(update,iBody)/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
}
double fd235UEnFlux(BODY *body,UPDATE *update,int iBody) {
  return fd235UPower(update,iBody)/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
}

/* DN/Dt */
double fdD26AlNumManDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d26AlConstMan,HALFLIFE26AL,body[iaBody[0]].dAge);
}

double fdD40KNumManDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d40KConstMan,HALFLIFE40K,body[iaBody[0]].dAge);
}
double fdD232ThNumManDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d232ThConstMan,HALFLIFE232TH,body[iaBody[0]].dAge);
}
double fdD238UNumManDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d238UConstMan,HALFLIFE238U,body[iaBody[0]].dAge);
}
double fdD235UNumManDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d235UConstMan,HALFLIFE235U,body[iaBody[0]].dAge);
}

double fdD26AlNumCoreDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d26AlConstCore,HALFLIFE26AL,body[iaBody[0]].dAge);
}

double fdD40KNumCoreDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d40KConstCore,HALFLIFE40K,body[iaBody[0]].dAge);
}
double fdD232ThNumCoreDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d232ThConstCore,HALFLIFE232TH,body[iaBody[0]].dAge);
}
double fdD238UNumCoreDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d238UConstCore,HALFLIFE238U,body[iaBody[0]].dAge);
}
double fdD235UNumCoreDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d235UConstCore,HALFLIFE235U,body[iaBody[0]].dAge);
}

double fdD40KNumCrustDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d40KConstCrust,HALFLIFE40K,body[iaBody[0]].dAge);
}
double fdD232ThNumCrustDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d232ThConstCrust,HALFLIFE232TH,body[iaBody[0]].dAge);
}
double fdD238UNumCrustDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d238UConstCrust,HALFLIFE238U,body[iaBody[0]].dAge);
}
double fdD235UNumCrustDt(BODY *body,SYSTEM *system,int *iaBody) {
  return fdDNumRadDt(body[iaBody[0]].d235UConstCrust,HALFLIFE235U,body[iaBody[0]].dAge);
}


//double fdRadPowerCrust(UPDATE *update,int iBody) {
  /* This is a fudge to get SurfEnFluxTot to behave. The current Earth 
     produces 7 TW of of radiogenic power in the crust. I will wave my hands
     and say that since Earth's crust has been growing with time, while at
     the same time radioactive decay is decreasing, the two magically
     cancel out and throughout Earth's history, the crust has always produced
     7 TW of power. */
/*
  return 7e12;
}
*/
