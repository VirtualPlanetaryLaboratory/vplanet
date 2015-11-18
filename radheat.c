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

void  InitializeControlRadheat(CONTROL *control) {
  /* Nothing for now, but this subroutine is necessary for module loops. */
}

void BodyCopyRadheat(BODY *dest,BODY *src,int foo,int iBody) {
  dest[iBody].d40KNumMan = src[iBody].d40KNumMan;
  dest[iBody].d40KConstMan = src[iBody].d40KConstMan;

  dest[iBody].d40KNumCore = src[iBody].d40KNumCore;
  dest[iBody].d40KConstCore = src[iBody].d40KConstCore;

  dest[iBody].d232ThNumMan = src[iBody].d232ThNumMan;
  dest[iBody].d232ThConstMan = src[iBody].d232ThConstMan;

  dest[iBody].d232ThNumCore = src[iBody].d232ThNumCore;
  dest[iBody].d232ThConstCore = src[iBody].d232ThConstCore;

  dest[iBody].d238UNumMan = src[iBody].d238UNumMan;
  dest[iBody].d238UConstMan = src[iBody].d238UConstMan;

  dest[iBody].d238UNumCore = src[iBody].d238UNumCore;
  dest[iBody].d238UConstCore = src[iBody].d238UConstCore;

  dest[iBody].d235UNumMan = src[iBody].d235UNumMan;
  dest[iBody].d235UConstMan = src[iBody].d235UConstMan;

  dest[iBody].d235UNumCore = src[iBody].d235UNumCore;
  dest[iBody].d235UConstCore = src[iBody].d235UConstCore;
}

void InitializeBodyRadheat(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
}

void InitializeUpdateTmpBodyRadheat(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
}

/**************** RADHEAT options ********************/

/* Potassium */

void Read40KPowerMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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
void Read40KPowerCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read40KMassMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read40KMassCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read40KNumMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read40KNumCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;

  //  fflush(stdout);
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

/* Thorium */

void Read232ThPowerMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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
void Read232ThPowerCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read232ThMassMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read232ThMassCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read232ThNumMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read232ThNumCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

/* Uranium 238 */

void Read238UPowerMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read238UMassMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read238UNumMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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
void Read238UPowerCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read238UMassCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read238UNumCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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


/* Uranium 235 PED */
void Read235UPowerMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read235UMassMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read235UNumMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read235UPowerCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read235UMassCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

void Read235UNumCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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

/* Initiatlize Input Options */

void InitializeOptionsRadheat(OPTIONS *options,fnReadOption fnRead[]) {

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
  options[OPT_235UPOWERMAN].dNeg = EPOWERMAN235U;   //1e12*1e-7; // cgs
  options[OPT_235UPOWERMAN].dDefault = 0;
  sprintf(options[OPT_235UPOWERMAN].cNeg,"TW");
  fnRead[OPT_235UPOWERMAN] = &Read235UPowerMan;
  
  sprintf(options[OPT_235UMASSCORE].cName,"d235UMassCore");  //PED
  sprintf(options[OPT_235UMASSCORE].cDescr,"Initial Core Mass of 235U");
  sprintf(options[OPT_235UMASSCORE].cDefault,"Primordial Earth: xxx");
  options[OPT_235UMASSCORE].iType = 2;
  options[OPT_235UMASSCORE].iMultiFile = 1;
  options[OPT_235UMASSCORE].dNeg = EMASSCORE235U;
  options[OPT_235UMASSCORE].dDefault = 0;
  sprintf(options[OPT_235UMASSCORE].cNeg,"Earth Masses");
  fnRead[OPT_235UMASSCORE] = &Read235UMassCore;
  
  sprintf(options[OPT_235UNUMCORE].cName,"d235UNumCore");  //PED
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
  options[OPT_235UPOWERCORE].dNeg = EPOWERCORE235U;   //1e12*1e-7; // cgs
  options[OPT_235UPOWERCORE].dDefault = 0;
  sprintf(options[OPT_235UPOWERCORE].cNeg,"TW");
  fnRead[OPT_235UPOWERCORE] = &Read235UPowerCore;
}

void ReadOptionsRadheat(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTRADHEAT;iOpt<OPTENDRADHEAT;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify RADHEAT ******************/

void NotMassAndNum(OPTIONS *options,int iMass,int iNum,int iBody) {
  if (options[iMass].iLine[iBody] >= 0 && options[iNum].iLine[iBody] >= 0) {
    fprintf(stderr,"ERROR: Cannot set both %s and %s.\n",options[iMass].cName,options[iNum].cName);
    DoubleLineExit(options[iMass].cFile[iBody],options[iNum].cFile[iBody],options[iMass].iLine[iBody],options[iNum].iLine[iBody]);
  }
}

void Assign40KNum(BODY *body,OPTIONS *options,double dAge,int iBody) {
  // Add an int iFile=iBody+1?XXX

    /* Mantle */
  // XXX Looks like issues here -- maybe from Peter?
    if (options[OPT_40KMASSMAN].iLine[iBody+1] >= 0) {
      //  I think here you need to define body.40KNum bc only the default value of 40Kmass has been chosen by user and set.
      //      printf("40KMass=%e, MASS40K=%e, 40KNum=%e\n",body[iBody].d40KMass,MASS40K,body[iBody].d40KNum);
      body[iBody].d40KNumMan=body[iBody].d40KMassMan/(MASS40K);
      //printf("40KMassMan set, body[iBody].d40KNumMan=%e, ENUMMAN40K=%e\n",body[iBody].d40KNumMan,ENUMMAN40K);
    }
    if (options[OPT_40KNUMMAN].iLine[iBody+1] >= 0) {
      // Do nothing bc default Num set.
      //printf("40KNumMan set, body[iBody].d40KNumMan=%e, ENUMMAN40K=%e\n",body[iBody].d40KNumMan,ENUMMAN40K);
    }
    if (options[OPT_40KPOWERMAN].iLine[iBody+1] >= 0) {
      body[iBody].d40KNumMan=body[iBody].d40KPowerMan/(ENERGY40K)*(HALFLIFE40K);
      //printf("40KPowerMan set, .d40KPowerMan=%e, .d40KNumMan=%e, ENUMMAN40K=%e\n",body[iBody].d40KPowerMan,body[iBody].d40KNumMan,ENUMMAN40K);
      //      exit(1);
    }  
    body[iBody].d40KConstMan = fd40KConstant(body[iBody].d40KNumMan,dAge);  //Get the constant given num and age.

    /* Core */
    if (options[OPT_40KMASSCORE].iLine[iBody+1] >= 0) {
      body[iBody].d40KNumCore=body[iBody].d40KMassCore/(MASS40K);
      //printf("40KMassCore set, body[iBody].d40KNumCore=%e, ENUMCORE40K=%e\n",body[iBody].d40KNumCore,ENUMCORE40K);
    }
    if (options[OPT_40KNUMCORE].iLine[iBody+1] >= 0) {
      // Do nothing bc default Num set.
      //printf("40KNumCore set, body[iBody].d40KNumCore=%e, ENUMCORE40K=%e\n",body[iBody].d40KNumCore,ENUMCORE40K);
    }
    if (options[OPT_40KPOWERCORE].iLine[iBody+1] >= 0) {
      body[iBody].d40KNumCore=body[iBody].d40KPowerCore/(ENERGY40K)*(HALFLIFE40K);
      //printf("40KPowerCore set, .d40KPowerCore=%e, .d40KNumCore=%e, ENUMCORE40K=%e\n",body[iBody].d40KPowerCore,body[iBody].d40KNumCore,ENUMCORE40K);
      //      exit(1);
    }  
    body[iBody].d40KConstCore = fd40KConstant(body[iBody].d40KNumCore,dAge);  //moved from above.
   
}

void Assign232ThNum(BODY *body,OPTIONS *options,double dAge,int iBody) {
    /* Mantle */
    if (options[OPT_232THMASSMAN].iLine[iBody+1] >= 0) {
      body[iBody].d232ThNumMan=body[iBody].d232ThMassMan/(MASS232TH);
      //printf("232ThMassMan set, body[iBody].d232ThNumMan=%e, ENUMMAN232TH=%e\n",body[iBody].d232ThNumMan,ENUMMAN232TH);
    }
    if (options[OPT_232THNUMMAN].iLine[iBody+1] >= 0) {
      //Do nothing, use default.
      //printf("232ThNumMan set, body[iBody].d232ThKNumMan=%e, ENUMMAN232TH=%e\n",body[iBody].d232ThNumMan,ENUMMAN232TH);
    }
    if (options[OPT_232THPOWERMAN].iLine[iBody+1] >= 0) {
      body[iBody].d232ThNumMan=body[iBody].d232ThPowerMan/(ENERGY232TH)*(HALFLIFE232TH);
      //printf("232ThPowerMan set, .d232ThPowerMan=%e, .d232ThNumMan=%e, ENUMMAN232TH=%e\n",body[iBody].d232ThPowerMan,body[iBody].d232ThNumMan,ENUMMAN232TH);
    }
    body[iBody].d232ThConstMan = fd232ThConstant(body[iBody].d232ThNumMan,dAge);

    /* Core */
    //    /*
    if (options[OPT_232THMASSCORE].iLine[iBody+1] >= 0) {
      body[iBody].d232ThNumCore=body[iBody].d232ThMassCore/(MASS232TH);
      //printf("232ThMassCore set, body[iBody].d232ThNumCore=%e, ENUMCORE232TH=%e\n",body[iBody].d232ThNumCore,ENUMCORE232TH);
    }
    if (options[OPT_232THNUMCORE].iLine[iBody+1] >= 0) {
      //Do nothing, use default.
      //printf("232ThNumCore set, body[iBody].d232ThKNumCore=%e, ENUMCORE232TH=%e\n",body[iBody].d232ThNumCore,ENUMCORE232TH);
    }
    if (options[OPT_232THPOWERCORE].iLine[iBody+1] >= 0) {
      body[iBody].d232ThNumCore=body[iBody].d232ThPowerCore/(ENERGY232TH)*(HALFLIFE232TH);
      //printf("232ThPowerCore set, .d232ThPowerCore=%e, .d232ThNumCore=%e, ENUMCORE232TH=%e\n",body[iBody].d232ThPowerCore,body[iBody].d232ThNumCore,ENUMCORE232TH);
    }
    body[iBody].d232ThConstCore = fd232ThConstant(body[iBody].d232ThNumCore,dAge);
    //    */
}

void Assign238UNum(BODY *body,OPTIONS *options,double dAge,int iBody) {
    /* Mantle */
    if (options[OPT_238UMASSMAN].iLine[iBody+1] >= 0) {
      /*    printf("238UMass not implemented.\n");
          exit(1);*/
      body[iBody].d238UNumMan=body[iBody].d238UMassMan/(MASS238U);
      //printf("238UMassMan set, body[iBody].d238UNumMan=%e, ENUMMAN238U=%e\n",body[iBody].d238UNumMan,ENUMMAN238U);
    }
    if (options[OPT_238UNUMMAN].iLine[iBody+1] >= 0) {
      //printf("238UNumMan set, body[iBody].d238UNumMan=%e, ENUMMAN238U=%e\n",body[iBody].d238UNumMan,ENUMMAN238U);
    }
    if (options[OPT_238UPOWERMAN].iLine[iBody+1] >= 0) {
      body[iBody].d238UNumMan=body[iBody].d238UPowerMan/(ENERGY238U)*(HALFLIFE238U);
      //printf("238UPowerMan set, .d238UPowerMan=%e, .d238UNumMan=%e, ENUMMAN238U=%e\n",body[iBody].d238UPowerMan,body[iBody].d238UNumMan,ENUMMAN238U);
    }
    body[iBody].d238UConstMan = fd238UConstant(body[iBody].d238UNumMan,dAge);
    /* Core */
    if (options[OPT_238UMASSCORE].iLine[iBody+1] >= 0) {
      /*    printf("238UMass not implemented.\n");
          exit(1);*/
      body[iBody].d238UNumCore=body[iBody].d238UMassCore/(MASS238U);
      //printf("238UMassCore set, body[iBody].d238UNumCore=%e, ENUMCORE238U=%e\n",body[iBody].d238UNumCore,ENUMCORE238U);
    }
    if (options[OPT_238UNUMCORE].iLine[iBody+1] >= 0) {
      //printf("238UNumCore set, body[iBody].d238UNumCore=%e, ENUMCORE238U=%e\n",body[iBody].d238UNumCore,ENUMCORE238U);
    }
    if (options[OPT_238UPOWERCORE].iLine[iBody+1] >= 0) {
      body[iBody].d238UNumCore=body[iBody].d238UPowerCore/(ENERGY238U)*(HALFLIFE238U);
      //printf("238UPowerCore set, .d238UPowerCore=%e, .d238UNumCore=%e, ENUMCORE238U=%e\n",body[iBody].d238UPowerCore,body[iBody].d238UNumCore,ENUMCORE238U);
    }
    body[iBody].d238UConstCore = fd238UConstant(body[iBody].d238UNumCore,dAge);
    
}

void Assign235UNum(BODY *body,OPTIONS *options,double dAge,int iBody) {  //PED
    /* Mantle */
  if (options[OPT_235UMASSMAN].iLine[iBody+1] >= 0) {
      /*    printf("235UMass not implemented.\n");
          exit(1);*/
      body[iBody].d235UNumMan=body[iBody].d235UMassMan/(MASS235U);
      //printf("235UMassMan set, body[iBody].d235UNumMan=%e, ENUMMAN235U=%e\n",body[iBody].d235UNumMan,ENUMMAN235U);
  }
  if (options[OPT_235UNUMMAN].iLine[iBody+1] >= 0) {
    //printf("235UNumMan set, body[iBody].d235UNumMan=%e, ENUMMAN235U=%e\n",body[iBody].d235UNumMan,ENUMMAN235U);
  }
  if (options[OPT_235UPOWERMAN].iLine[iBody+1] >= 0) {
      body[iBody].d235UNumMan=body[iBody].d235UPowerMan/(ENERGY235U)*(HALFLIFE235U);
      //printf("235UPowerMan set, .d235UPowerMan=%e, .d235UNumMan=%e, ENUMMAN235U=%e\n",body[iBody].d235UPowerMan,body[iBody].d235UNumMan,ENUMMAN235U);
  }
  body[iBody].d235UConstMan = fd235UConstant(body[iBody].d235UNumMan,dAge);
  /* Core */
  if (options[OPT_235UMASSCORE].iLine[iBody+1] >= 0) {
      /*    printf("235UMass not implemented.\n");
          exit(1);*/
      body[iBody].d235UNumCore=body[iBody].d235UMassCore/(MASS235U);
      //printf("235UMassCore set, body[iBody].d235UNumCore=%e, ENUMCORE235U=%e\n",body[iBody].d235UNumCore,ENUMCORE235U);
  }
  if (options[OPT_235UNUMCORE].iLine[iBody+1] >= 0) {
    //printf("235UNumCore set, body[iBody].d235UNumCore=%e, ENUMCORE235U=%e\n",body[iBody].d235UNumCore,ENUMCORE235U);
  }
  if (options[OPT_235UPOWERCORE].iLine[iBody+1] >= 0) {
      body[iBody].d235UNumCore=body[iBody].d235UPowerCore/(ENERGY235U)*(HALFLIFE235U);
      //printf("235UPowerCore set, .d235UPowerCore=%e, .d235UNumCore=%e, ENUMCORE235U=%e\n",body[iBody].d235UPowerCore,body[iBody].d235UNumCore,ENUMCORE235U);
  }
  body[iBody].d235UConstCore = fd235UConstant(body[iBody].d235UNumCore,dAge);
}

void Verify40K(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
  Assign40KNum(body,options,dAge,iBody);
  /* Mantle */
  if (update[iBody].i40KMan >= 0) {
    update[iBody].iaType[update[iBody].i40KMan][0] = 1;
    update[iBody].iNumBodies[update[iBody].i40KMan][0]=1;
    update[iBody].iaBody[update[iBody].i40KMan][0] = malloc(update[iBody].iNumBodies[update[iBody].i40KMan][0]*sizeof(int)); //iaBody is the number of bodies that are affected by this variable.
    update[iBody].iaBody[update[iBody].i40KMan][0][0]=iBody;
    update[iBody].pdD40KNumManDt = &update[iBody].daDerivProc[update[iBody].i40KMan][0];
    fnUpdate[iBody][update[iBody].i40KMan][0] = &fdD40KNumManDt;
  }

  /* Core */
  if (update[iBody].i40KCore >= 0) {
    update[iBody].iaType[update[iBody].i40KCore][0] = 1;
    update[iBody].iNumBodies[update[iBody].i40KCore][0]=1;
    update[iBody].iaBody[update[iBody].i40KCore][0] = malloc(update[iBody].iNumBodies[update[iBody].i40KCore][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i40KCore][0][0]=iBody;
    update[iBody].pdD40KNumCoreDt = &update[iBody].daDerivProc[update[iBody].i40KCore][0];
    fnUpdate[iBody][update[iBody].i40KCore][0] = &fdD40KNumCoreDt;
  }
}

void Verify232Th(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
  Assign232ThNum(body,options,dAge,iBody);

  /* Mantle */
  if (update[iBody].i232ThMan >= 0) {
    update[iBody].iaType[update[iBody].i232ThMan][0] = 1;
    update[iBody].iNumBodies[update[iBody].i232ThMan][0]=1;
    update[iBody].iaBody[update[iBody].i232ThMan][0] = malloc(update[iBody].iNumBodies[update[iBody].i232ThMan][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i232ThMan][0][0]=iBody;
    update[iBody].pdD232ThNumManDt = &update[iBody].daDerivProc[update[iBody].i232ThMan][0];
    fnUpdate[iBody][update[iBody].i232ThMan][0] = &fdD232ThNumManDt;
  }

  /* Core */
  if (update[iBody].i232ThMan >= 0) {
    update[iBody].iaType[update[iBody].i232ThCore][0] = 1;
    update[iBody].iNumBodies[update[iBody].i232ThCore][0]=1;
    update[iBody].iaBody[update[iBody].i232ThCore][0] = malloc(update[iBody].iNumBodies[update[iBody].i232ThCore][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i232ThCore][0][0]=iBody;
    update[iBody].pdD232ThNumCoreDt = &update[iBody].daDerivProc[update[iBody].i232ThCore][0];
    fnUpdate[iBody][update[iBody].i232ThCore][0] = &fdD232ThNumCoreDt;
  }
}

void Verify238U(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
  Assign238UNum(body,options,dAge,iBody);
  /* Mantle */

  if (update[iBody].i238UMan >= 0) {
    update[iBody].iaType[update[iBody].i238UMan][0] = 1;
    update[iBody].iNumBodies[update[iBody].i238UMan][0]=1;
    update[iBody].iaBody[update[iBody].i238UMan][0] = malloc(update[iBody].iNumBodies[update[iBody].i238UMan][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i238UMan][0][0]=iBody;
    update[iBody].pdD238UNumManDt = &update[iBody].daDerivProc[update[iBody].i238UMan][0];
    fnUpdate[iBody][update[iBody].i238UMan][0] = &fdD238UNumManDt;
  }

  /* Core */
  if (update[iBody].i238UCore >= 0) {
    update[iBody].iaType[update[iBody].i238UCore][0] = 1;
    update[iBody].iNumBodies[update[iBody].i238UCore][0]=1;
    update[iBody].iaBody[update[iBody].i238UCore][0] = malloc(update[iBody].iNumBodies[update[iBody].i238UCore][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i238UCore][0][0]=iBody;
    update[iBody].pdD238UNumCoreDt = &update[iBody].daDerivProc[update[iBody].i238UCore][0];
    fnUpdate[iBody][update[iBody].i238UCore][0] = &fdD238UNumCoreDt;
  }
}

void Verify235U(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) { //PED
  Assign235UNum(body,options,dAge,iBody);

  /* Mantle */
  if (update[iBody].i235UMan >= 0) {
    update[iBody].iaType[update[iBody].i235UMan][0] = 1;
    update[iBody].iNumBodies[update[iBody].i235UMan][0]=1;
    update[iBody].iaBody[update[iBody].i235UMan][0] = malloc(update[iBody].iNumBodies[update[iBody].i235UMan][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i235UMan][0][0]=iBody;
    update[iBody].pdD235UNumManDt = &update[iBody].daDerivProc[update[iBody].i235UMan][0];
    fnUpdate[iBody][update[iBody].i235UMan][0] = &fdD235UNumManDt;
  }

  /* Core */
  if (update[iBody].i235UCore >= 0) {
    update[iBody].iaType[update[iBody].i235UCore][0] = 1;
    update[iBody].iNumBodies[update[iBody].i235UCore][0]=1;
    update[iBody].iaBody[update[iBody].i235UCore][0] = malloc(update[iBody].iNumBodies[update[iBody].i235UCore][0]*sizeof(int));
    update[iBody].iaBody[update[iBody].i235UCore][0][0]=iBody;
    update[iBody].pdD235UNumCoreDt = &update[iBody].daDerivProc[update[iBody].i235UCore][0];
    fnUpdate[iBody][update[iBody].i235UCore][0] = &fdD235UNumCoreDt;
  }
}

/*
double fdGetModuleIntRadheat(UPDATE *update,int iBody) {
  int iModule;
  for (iModule=0;iModule<update[iBody].iNumModules;iModule++) {
    if (update[iBody]->iaModule[iModule] == RADHEAT)
      return iModule;
  }
  fprintf(stderr,"ERROR: Radheat not found for body #%d.\n",iBody);
  exit(1);
}
*/
void PropsAuxRadheat(BODY *body,UPDATE *update,int iBody) {
  /* Nothing */
}

void fnForceBehaviorRadheat(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,int iBody,int iModule) {
  if (body[iBody].d40KNumMan < 0.5)
    body[iBody].d40KNumMan = 0;
  if (body[iBody].d40KNumCore < 0.5)
    body[iBody].d40KNumCore = 0;
  
  if (body[iBody].d232ThNumMan < 0.5)
    body[iBody].d232ThNumMan = 0;
  if (body[iBody].d232ThNumCore < 0.5)
    body[iBody].d232ThNumCore = 0;

  if (body[iBody].d238UNumMan < 0.5)
    body[iBody].d238UNumMan = 0;
  if (body[iBody].d238UNumCore < 0.5)
    body[iBody].d238UNumCore = 0;

  if (body[iBody].d235UNumMan < 0.5)
    body[iBody].d235UNumMan = 0;
  if (body[iBody].d235UNumCore < 0.5)
    body[iBody].d235UNumCore = 0;
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

  if (options[OPT_40KNUMMAN].iLine[iFile] == -1 && options[OPT_40KMASSMAN].iLine[iFile] == -1 && options[OPT_40KPOWERMAN].iLine[iFile] == -1)
    RadheatExit(files,"40KMan",iFile);

  if (options[OPT_40KNUMCORE].iLine[iFile] == -1 && options[OPT_40KMASSCORE].iLine[iFile] == -1 && options[OPT_40KPOWERCORE].iLine[iFile] == -1)
    RadheatExit(files,"40KCore",iFile);

  // XXX This looks like it's insufficient to capture all the permutations
  NotMassAndNum(options,OPT_40KMASSMAN,OPT_40KNUMMAN,iBody);

  // 40K set properly
  Verify40K(body,options,update,body[iBody].dAge,fnUpdate,iBody);  //Verify Man and Core.

  // 232Th
  if (options[OPT_232THNUMMAN].iLine[iFile] == -1 && options[OPT_232THMASSMAN].iLine[iFile] == -1 && options[OPT_232THPOWERMAN].iLine[iFile] == -1)
    RadheatExit(files,"232ThMan",iFile);

  if (options[OPT_232THNUMCORE].iLine[iFile] == -1 && options[OPT_232THMASSCORE].iLine[iFile] == -1 && options[OPT_232THPOWERCORE].iLine[iFile] == -1)
    RadheatExit(files,"232ThCore",iFile);

  // XXX Also insufficient?
  NotMassAndNum(options,OPT_232THMASSMAN,OPT_232THNUMMAN,iFile);

  // 232Th set corectly
  Verify232Th(body,options,update,body[iBody].dAge,fnUpdate,iBody);

  // 238U
  if (options[OPT_238UNUMMAN].iLine[iFile] == -1 && options[OPT_238UMASSMAN].iLine[iFile] == -1 && options[OPT_238UPOWERMAN].iLine[iFile] == -1)
    RadheatExit(files,"238UMan",iFile);

  if (options[OPT_238UNUMCORE].iLine[iFile] == -1 && options[OPT_238UMASSCORE].iLine[iFile] == -1 && options[OPT_238UPOWERCORE].iLine[iFile] == -1)
    RadheatExit(files,"238UCore",iFile);

  // XXX Still insufficient?
  NotMassAndNum(options,OPT_238UMASSMAN,OPT_238UNUMMAN,iFile);

  // 238U set correctly
  Verify238U(body,options,update,body[iBody].dAge,fnUpdate,iBody);

  // 235U
  if (options[OPT_235UNUMMAN].iLine[iFile] == -1 && options[OPT_235UMASSMAN].iLine[iFile] == -1 && options[OPT_235UPOWERMAN].iLine[iFile] == -1)
    RadheatExit(files,"235UMan",iFile);

  if (options[OPT_235UNUMCORE].iLine[iFile] == -1 && options[OPT_235UMASSCORE].iLine[iFile] == -1 && options[OPT_235UPOWERCORE].iLine[iFile] == -1)
    RadheatExit(files,"235UCore",iFile);

  // Insufficient?
  NotMassAndNum(options,OPT_235UMASSMAN,OPT_235UNUMMAN,iFile);

  // 235U set correctly
  Verify235U(body,options,update,body[iBody].dAge,fnUpdate,iBody);

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorRadheat;
  control->Evolve.fnPropsAux[iBody][iModule] = &PropsAuxRadheat;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyRadheat;
  output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxRadheat;
}


void InitializeModuleRadheat(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** RADHEAT update ****************/

void InitializeUpdateRadheat(BODY *body,UPDATE *update,int iBody) {
  /* Initially allow all radiogenic heat sources to be present. If any are 0, 
     or < dMinRadPower, they will me removed from update[iBody] in 
     ForceBehavior.
  */
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
  if (body[iBody].d235UNumMan > 0 || body[iBody].d235UMassMan > 0 || body[iBody].d235UPowerMan > 0) {  //PED
    update[iBody].iNumVars++;
    update[iBody].iNum235UMan++;
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
  if (body[iBody].d235UNumCore > 0 || body[iBody].d235UMassCore > 0 || body[iBody].d235UPowerCore > 0) {  //PED
    update[iBody].iNumVars++;
    update[iBody].iNum235UCore++;
  }
}

//PED: Combine these into ..HeatMan?  and ..HeatCore?
void FinalizeUpdate40KNumManRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD40KMAN;
  update[iBody].iNum40KMan = (*iEqn)++;
}
void FinalizeUpdate232ThNumManRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD232THMAN;
  update[iBody].iNum232ThMan = (*iEqn)++;
}
void FinalizeUpdate238UNumManRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD238UMAN;
  update[iBody].iNum238UMan = (*iEqn)++;
}
void FinalizeUpdate235UNumManRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {  //PED
  update[iBody].iaModule[iVar][*iEqn] = RAD235UMAN;
  update[iBody].iNum235UMan = (*iEqn)++;
}

void FinalizeUpdate40KNumCoreRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
    update[iBody].iaModule[iVar][*iEqn] = RAD40KCORE;
    update[iBody].iNum40KCore = (*iEqn)++;
}
void FinalizeUpdate232ThNumCoreRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD232THCORE;
  update[iBody].iNum232ThCore = (*iEqn)++;
}
void FinalizeUpdate238UNumCoreRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD238UCORE;
  update[iBody].iNum238UCore = (*iEqn)++;
}
void FinalizeUpdate235UNumCoreRadheat(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {  //PED
  update[iBody].iaModule[iVar][*iEqn] = RAD235UCORE;
  update[iBody].iNum235UCore = (*iEqn)++;
}

/***************** RADHEAT Halts *****************/

/* Minimum 40K Powering? */
//PED: these subroutines aren't finished.  The default halt values aren't set.  body.d40KPowerMan is not a primary variable, but NumMan is.
int fbHaltMin40KPower(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (body[iBody].d40KPowerMan < halt->dMin40KPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 40K Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d40KPowerMan,io->iSciNot,io->iDigits);
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

  if (body[iBody].d232ThPowerMan < halt->dMin232ThPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 232Th Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d232ThPowerMan,io->iSciNot,io->iDigits);
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

  if (body[iBody].d238UPowerMan < halt->dMin238UPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 238U Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d238UPowerMan,io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMin238UPower,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        

/* Minimum 235U Powering? PED */

int fbHaltMin235UPower(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (body[iBody].d235UPowerMan < halt->dMin235UPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 235U Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d235UPowerMan,io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMin235UPower,io->iSciNot,io->iDigits);
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
}

/************* RADHEAT Outputs ******************/

/* NOTE: If you write a new Write subroutine here you need to add the associate 
   block of initialization in InitializeOutputRadheat below */

/* Potassium */

void Write40KPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 40K */
  *dTmp = -(*(update[iBody].pdD40KNumManDt))*ENERGY40K;
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
  *dTmp = body[iBody].d40KPowerMan/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
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
  *dTmp = -(*(update[iBody].pdD40KNumCoreDt))*ENERGY40K;
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

/* Thorium */

void Write232ThPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total heat from 232Th */
  *dTmp = -(*(update[iBody].pdD232ThNumManDt))*ENERGY232TH;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void Write232ThEnFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get surface heat flux from 232Th */
  *dTmp = body[iBody].d232ThPowerMan/(4*PI*body[iBody].dRadius*body[iBody].dRadius);

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
  *dTmp = -(*(update[iBody].pdD232ThNumCoreDt))*ENERGY232TH;
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

/* Uranium 238 */

void Write238UPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 238U */
  *dTmp = -(*(update[iBody].pdD238UNumManDt))*ENERGY238U;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void Write238UEnFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get surface heat flux from 238U */
  *dTmp = body[iBody].d238UPowerMan/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
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
  *dTmp = -(*(update[iBody].pdD238UNumCoreDt))*ENERGY238U;
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

/* Uranium 235 PED */

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

void Write235UEnFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get surface heat flux from 235U */
  *dTmp = body[iBody].d235UPowerMan/(4*PI*body[iBody].dRadius*body[iBody].dRadius);

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
  *dTmp = -(*(update[iBody].pdD235UNumCoreDt))*ENERGY235U;
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

/* Totals */
void WriteRadPowerMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Total Radiogenic Power Production */
      *dTmp = fdRadPowerMan(body,update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}
void WriteRadPowerCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Total Radiogenic Power Production */
      *dTmp = fdRadPowerCore(body,update,iBody);
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
      *dTmp = fdRadPowerTotal(body,update,iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void WriteSurfEnFluxRadheat(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Radiogenic Surface Energy Flux */
  *dTmp = fdSurfEnFluxRadheat(body,system,update,iBody,iBody);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void InitializeOutputRadheat(OUTPUT *output,fnWriteOutput fnWrite[]) {

  /* Potassium */
    //  PED:  Do these default numbers matter??  If so they need to be changed.
  sprintf(output[OUT_40KPOWERMAN].cName,"40KPowerMan");
  sprintf(output[OUT_40KPOWERMAN].cDescr,"Total Power Generated by 40K");
  sprintf(output[OUT_40KPOWERMAN].cNeg,"TW");
  output[OUT_40KPOWERMAN].bNeg = 1;
  output[OUT_40KPOWERMAN].dNeg = 1e-12; 
  output[OUT_40KPOWERMAN].iNum = 1;
  fnWrite[OUT_40KPOWERMAN] = &Write40KPowerMan;
  
  sprintf(output[OUT_40KENFLUX].cName,"40KEnFlux");
  sprintf(output[OUT_40KENFLUX].cDescr,"Surface Energy Flux from 40K");
  sprintf(output[OUT_40KENFLUX].cNeg,"W/m^2");
  output[OUT_40KENFLUX].bNeg = 1;
  output[OUT_40KENFLUX].dNeg = 1;
  output[OUT_40KENFLUX].iNum = 1;
  fnWrite[OUT_40KENFLUX] = &Write40KEnFlux;

  sprintf(output[OUT_40KDPOWERDT].cName,"D40KPowerDt");
  sprintf(output[OUT_40KDPOWERDT].cDescr,"Time Rate of Change of 40K Power Generation");
  sprintf(output[OUT_40KDPOWERDT].cNeg,"TW/Gyr");
  output[OUT_40KDPOWERDT].bNeg = 1;
  output[OUT_40KDPOWERDT].dNeg = 1e12/1e9; /* XXX */
  output[OUT_40KDPOWERDT].iNum = 1;
  fnWrite[OUT_40KDPOWERDT] = &WriteD40KPowerDt;  //PED: that subr is not written yet?
  
  sprintf(output[OUT_40KDNUMDT].cName,"D40KNumManDt");
  sprintf(output[OUT_40KDNUMDT].cDescr,"Time Rate of Change of 40K Heat Generation");
  sprintf(output[OUT_40KDNUMDT].cNeg,"/Gyr");
  output[OUT_40KDNUMDT].bNeg = 1;
  output[OUT_40KDNUMDT].dNeg = YEARSEC*1e9; 
  output[OUT_40KDNUMDT].iNum = 1;
  fnWrite[OUT_40KDNUMDT] = &WriteD40KNumDt;
  
  sprintf(output[OUT_40KTIME].cName,"40KTimescale");
  sprintf(output[OUT_40KTIME].cDescr,"Timescale for 40K Power Generation");
  sprintf(output[OUT_40KTIME].cNeg,"Gyr");
  output[OUT_40KTIME].bNeg = 1;
  output[OUT_40KTIME].dNeg = YEARSEC*1e9;
  output[OUT_40KTIME].iNum = 1;
  fnWrite[OUT_40KTIME] = &Write40KTimescale;
  
  sprintf(output[OUT_40KMASSMAN].cName,"40KMassMan");
  sprintf(output[OUT_40KMASSMAN].cDescr,"Total Mass of 40K");
  sprintf(output[OUT_40KMASSMAN].cNeg,"Earth Masses");
  output[OUT_40KMASSMAN].bNeg = 1;
  output[OUT_40KMASSMAN].dNeg = MEARTH;
  output[OUT_40KMASSMAN].iNum = 1;
  fnWrite[OUT_40KMASSMAN] = &Write40KMassMan;
  
  sprintf(output[OUT_40KNUMMAN].cName,"40KNumMan");
  sprintf(output[OUT_40KNUMMAN].cDescr,"Total Number of 40K Atoms");
  sprintf(output[OUT_40KNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_40KNUMMAN].bNeg = 1;
  output[OUT_40KNUMMAN].dNeg = EMASSMAN40K/MASS40K;
  output[OUT_40KNUMMAN].iNum = 1;
  fnWrite[OUT_40KNUMMAN] = &Write40KNumMan;

  /* Core */
  sprintf(output[OUT_40KPOWERCORE].cName,"40KPowerCore");
  sprintf(output[OUT_40KPOWERCORE].cDescr,"Core Power Generated by 40K");
  sprintf(output[OUT_40KPOWERCORE].cNeg,"TW");
  output[OUT_40KPOWERCORE].bNeg = 1;
  output[OUT_40KPOWERCORE].dNeg = 1e-12; // ergs/s -> TW 
  output[OUT_40KPOWERCORE].iNum = 1;
  fnWrite[OUT_40KPOWERCORE] = &Write40KPowerCore;
  
  sprintf(output[OUT_40KMASSCORE].cName,"40KMassCore");
  sprintf(output[OUT_40KMASSCORE].cDescr,"Total Core Mass of 40K");
  sprintf(output[OUT_40KMASSCORE].cNeg,"Earth Masses");
  output[OUT_40KMASSCORE].bNeg = 1;
  output[OUT_40KMASSCORE].dNeg = MEARTH;
  output[OUT_40KMASSCORE].iNum = 1;
  fnWrite[OUT_40KMASSCORE] = &Write40KMassCore;
  
  sprintf(output[OUT_40KNUMCORE].cName,"40KNumCore");
  sprintf(output[OUT_40KNUMCORE].cDescr,"Total Core Number of 40K Atoms");
  sprintf(output[OUT_40KNUMCORE].cNeg,"Initial Primordial Earth Number");
  output[OUT_40KNUMCORE].bNeg = 1;
  output[OUT_40KNUMCORE].dNeg = EMASSCORE40K/MASS40K;
  output[OUT_40KNUMCORE].iNum = 1;
  fnWrite[OUT_40KNUMCORE] = &Write40KNumCore;
  
  /* Thorium */
  sprintf(output[OUT_232THPOWERMAN].cName,"232ThPowerMan");
  sprintf(output[OUT_232THPOWERMAN].cDescr,"Total Power Generated by 232Th");
  sprintf(output[OUT_232THPOWERMAN].cNeg,"TW");
  output[OUT_232THPOWERMAN].bNeg = 1;
  output[OUT_232THPOWERMAN].dNeg = 1e-12; 
  output[OUT_232THPOWERMAN].iNum = 1;
  fnWrite[OUT_232THPOWERMAN] = &Write232ThPowerMan;
  
  sprintf(output[OUT_232THENFLUX].cName,"232ThEnFlux");
  sprintf(output[OUT_232THENFLUX].cDescr,"Surface Energy Flux from 232Th");
  sprintf(output[OUT_232THENFLUX].cNeg,"W/m^2");
  output[OUT_232THENFLUX].bNeg = 1;
  output[OUT_232THENFLUX].dNeg = 1;
  output[OUT_232THENFLUX].iNum = 1;
  fnWrite[OUT_232THENFLUX] = &Write232ThEnFlux;

  sprintf(output[OUT_232THDNUMDT].cName,"D232ThNumManDt");
  sprintf(output[OUT_232THDNUMDT].cDescr,"Time Rate of Change of the number of 232Th nuclei");
  sprintf(output[OUT_232THDNUMDT].cNeg,"/Gyr");
  output[OUT_232THDNUMDT].bNeg = 1;
  output[OUT_232THDNUMDT].dNeg = YEARSEC*1e9;  
  output[OUT_232THDNUMDT].iNum = 1;
  fnWrite[OUT_232THDNUMDT] = &WriteD232ThNumDt;

  sprintf(output[OUT_232THTIME].cName,"232ThTimescale");
  sprintf(output[OUT_232THTIME].cDescr,"Timescale for 232Th Power Generation");
  sprintf(output[OUT_232THTIME].cNeg,"Gyr");
  output[OUT_232THTIME].bNeg = 1;
  output[OUT_232THTIME].dNeg = YEARSEC*1e9;
  output[OUT_232THTIME].iNum = 1;
  fnWrite[OUT_232THTIME] = &Write232ThTimescale;
  
  sprintf(output[OUT_232THMASSMAN].cName,"232ThMassMan");
  sprintf(output[OUT_232THMASSMAN].cDescr,"Total Mass of 232Th");
  sprintf(output[OUT_232THMASSMAN].cNeg,"Earth Masses");
  output[OUT_232THMASSMAN].bNeg = 1;
  output[OUT_232THMASSMAN].dNeg = MEARTH;
  output[OUT_232THMASSMAN].iNum = 1;
  fnWrite[OUT_232THMASSMAN] = &Write232ThMassMan;
  
  sprintf(output[OUT_232THNUMMAN].cName,"232ThNumMan");
  sprintf(output[OUT_232THNUMMAN].cDescr,"Total Number of 232Th Atoms");
  sprintf(output[OUT_232THNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_232THNUMMAN].bNeg = 1;
  output[OUT_232THNUMMAN].dNeg = EMASSMAN232TH/MASS232TH;
  output[OUT_232THNUMMAN].iNum = 1;
  fnWrite[OUT_232THNUMMAN] = &Write232ThNumMan;

  /* Core */
  sprintf(output[OUT_232THPOWERCORE].cName,"232ThPowerCore");
  sprintf(output[OUT_232THPOWERCORE].cDescr,"Total Power Generated by 232Th");
  sprintf(output[OUT_232THPOWERCORE].cNeg,"TW");
  output[OUT_232THPOWERCORE].bNeg = 1;
  output[OUT_232THPOWERCORE].dNeg = 1e-12; 
  output[OUT_232THPOWERCORE].iNum = 1;
  fnWrite[OUT_232THPOWERCORE] = &Write232ThPowerCore;

  sprintf(output[OUT_232THMASSCORE].cName,"232ThMassCore");
  sprintf(output[OUT_232THMASSCORE].cDescr,"Total Mass of 232Th");
  sprintf(output[OUT_232THMASSCORE].cNeg,"Earth Masses");
  output[OUT_232THMASSCORE].bNeg = 1;
  output[OUT_232THMASSCORE].dNeg = MEARTH;
  output[OUT_232THMASSCORE].iNum = 1;
  fnWrite[OUT_232THMASSCORE] = &Write232ThMassCore;
  
  sprintf(output[OUT_232THNUMCORE].cName,"232ThNumCore");
  sprintf(output[OUT_232THNUMCORE].cDescr,"Total Number of 232Th Atoms");
  sprintf(output[OUT_232THNUMCORE].cNeg,"Initial Primordial Earth Number");
  output[OUT_232THNUMCORE].bNeg = 1;
  output[OUT_232THNUMCORE].dNeg = EMASSCORE232TH/MASS232TH;
  output[OUT_232THNUMCORE].iNum = 1;
  fnWrite[OUT_232THNUMCORE] = &Write232ThNumCore;
    
  /* Uranium 238 */

  sprintf(output[OUT_238UPOWERMAN].cName,"238UPowerMan");
  sprintf(output[OUT_238UPOWERMAN].cDescr,"Total Power Generated by 238U");
  sprintf(output[OUT_238UPOWERMAN].cNeg,"TW");
  output[OUT_238UPOWERMAN].bNeg = 1;
  output[OUT_238UPOWERMAN].dNeg = 1e-12; 
  output[OUT_238UPOWERMAN].iNum = 1;
  fnWrite[OUT_238UPOWERMAN] = &Write238UPowerMan;
  
  sprintf(output[OUT_238UENFLUX].cName,"238UEnFlux");
  sprintf(output[OUT_238UENFLUX].cDescr,"Surface Energy Flux due to 238U");
  sprintf(output[OUT_238UENFLUX].cNeg,"W/m^2");
  output[OUT_238UENFLUX].bNeg = 1;
  output[OUT_238UENFLUX].dNeg = 1;
  output[OUT_238UENFLUX].iNum = 1;
  fnWrite[OUT_238UENFLUX] = &Write238UEnFlux;

  sprintf(output[OUT_238UDNUMDT].cName,"D238UNumManDt");
  sprintf(output[OUT_238UDNUMDT].cDescr,"Time Rate of Change of the Number of 238U Nuclei");
  sprintf(output[OUT_238UDNUMDT].cNeg,"Gyr");
  output[OUT_238UDNUMDT].bNeg = 1;
  output[OUT_238UDNUMDT].dNeg = YEARSEC*1e9;
  output[OUT_238UDNUMDT].iNum = 1;
  fnWrite[OUT_238UDNUMDT] = &WriteD238UNumDt;

  sprintf(output[OUT_238UTIME].cName,"238UTimescale");
  sprintf(output[OUT_238UTIME].cDescr,"Timescale for 238U Power Generation");
  sprintf(output[OUT_238UTIME].cNeg,"Gyr");
  output[OUT_238UTIME].bNeg = 1;
  output[OUT_238UTIME].dNeg = YEARSEC*1e9;
  output[OUT_238UTIME].iNum = 1;
  fnWrite[OUT_238UTIME] = &Write238UTimescale;
  
  sprintf(output[OUT_238UMASSMAN].cName,"238UMassMan");
  sprintf(output[OUT_238UMASSMAN].cDescr,"Total Mass of 238U");
  sprintf(output[OUT_238UMASSMAN].cNeg,"Earth Masses");
  output[OUT_238UMASSMAN].bNeg = 1;
  output[OUT_238UMASSMAN].dNeg = MEARTH;
  output[OUT_238UMASSMAN].iNum = 1;
  fnWrite[OUT_238UMASSMAN] = &Write238UMassMan;
  
  sprintf(output[OUT_238UNUMMAN].cName,"238UNumMan");
  sprintf(output[OUT_238UNUMMAN].cDescr,"Total Number of 238U Atoms");
  sprintf(output[OUT_238UNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_238UNUMMAN].bNeg = 1;
  output[OUT_238UNUMMAN].dNeg = 1;
  output[OUT_238UNUMMAN].iNum = 1;
  fnWrite[OUT_238UNUMMAN] = &Write238UNumMan;

  /* Core */
  sprintf(output[OUT_238UPOWERCORE].cName,"238UPowerCore");
  sprintf(output[OUT_238UPOWERCORE].cDescr,"Total Core Power Generated by 238U");
  sprintf(output[OUT_238UPOWERCORE].cNeg,"TW");
  output[OUT_238UPOWERCORE].bNeg = 1;
  output[OUT_238UPOWERCORE].dNeg = 1e-12; 
  output[OUT_238UPOWERCORE].iNum = 1;
  fnWrite[OUT_238UPOWERCORE] = &Write238UPowerCore;
  
  sprintf(output[OUT_238UMASSCORE].cName,"238UMassCore");
  sprintf(output[OUT_238UMASSCORE].cDescr,"Total Core Mass of 238U");
  sprintf(output[OUT_238UMASSCORE].cNeg,"Earth Masses");
  output[OUT_238UMASSCORE].bNeg = 1;
  output[OUT_238UMASSCORE].dNeg = MEARTH;
  output[OUT_238UMASSCORE].iNum = 1;
  fnWrite[OUT_238UMASSCORE] = &Write238UMassCore;
  
  sprintf(output[OUT_238UNUMCORE].cName,"238UNumCore");
  sprintf(output[OUT_238UNUMCORE].cDescr,"Total Core Number of 238U Atoms");
  sprintf(output[OUT_238UNUMCORE].cNeg,"Initial Primordial Earth Number");
  output[OUT_238UNUMCORE].bNeg = 1;
  output[OUT_238UNUMCORE].dNeg = 1;
  output[OUT_238UNUMCORE].iNum = 1;
  fnWrite[OUT_238UNUMCORE] = &Write238UNumCore;
  
  /* Uranium 235 */
  
  sprintf(output[OUT_235UPOWERMAN].cName,"235UPowerMan");
  sprintf(output[OUT_235UPOWERMAN].cDescr,"Total Power Generated by 235U");
  sprintf(output[OUT_235UPOWERMAN].cNeg,"TW");
  output[OUT_235UPOWERMAN].bNeg = 1;
  output[OUT_235UPOWERMAN].dNeg = 1e-12; 
  output[OUT_235UPOWERMAN].iNum = 1;
  fnWrite[OUT_235UPOWERMAN] = &Write235UPowerMan;
  
  sprintf(output[OUT_235UENFLUX].cName,"235UEnFlux");
  sprintf(output[OUT_235UENFLUX].cDescr,"Surface Energy Flux due to 235U");
  sprintf(output[OUT_235UENFLUX].cNeg,"W/m^2");
  output[OUT_235UENFLUX].bNeg = 1;
  output[OUT_235UENFLUX].dNeg = 1e-3;
  output[OUT_235UENFLUX].iNum = 1;
  fnWrite[OUT_235UENFLUX] = &Write235UEnFlux;

  sprintf(output[OUT_235UDNUMDT].cName,"D235UNumManDt");
  sprintf(output[OUT_235UDNUMDT].cDescr,"Time Rate of Change of the Number of 235U Nuclei");
  sprintf(output[OUT_235UDNUMDT].cNeg,"Gyr");
  output[OUT_235UDNUMDT].bNeg = 1;
  output[OUT_235UDNUMDT].dNeg = YEARSEC*1e9;
  output[OUT_235UDNUMDT].iNum = 1;
  fnWrite[OUT_235UDNUMDT] = &WriteD235UNumDt;

  sprintf(output[OUT_235UTIME].cName,"235UTimescale");
  sprintf(output[OUT_235UTIME].cDescr,"Timescale for 235U Power Generation");
  sprintf(output[OUT_235UTIME].cNeg,"Gyr");
  output[OUT_235UTIME].bNeg = 1;
  output[OUT_235UTIME].dNeg = YEARSEC*1e9;
  output[OUT_235UTIME].iNum = 1;
  fnWrite[OUT_235UTIME] = &Write235UTimescale;
  
  sprintf(output[OUT_235UMASSMAN].cName,"235UMassMan");
  sprintf(output[OUT_235UMASSMAN].cDescr,"Total Mass of 235U");
  sprintf(output[OUT_235UMASSMAN].cNeg,"Earth Masses");
  output[OUT_235UMASSMAN].bNeg = 1;
  output[OUT_235UMASSMAN].dNeg = MEARTH;
  output[OUT_235UMASSMAN].iNum = 1;
  fnWrite[OUT_235UMASSMAN] = &Write235UMassMan;
  
  sprintf(output[OUT_235UNUMMAN].cName,"235UNumMan");
  sprintf(output[OUT_235UNUMMAN].cDescr,"Total Number of 235U Atoms");
  sprintf(output[OUT_235UNUMMAN].cNeg,"Initial Primordial Earth Number");
  output[OUT_235UNUMMAN].bNeg = 1;
  output[OUT_235UNUMMAN].dNeg = 1;
  output[OUT_235UNUMMAN].iNum = 1;
  fnWrite[OUT_235UNUMMAN] = &Write235UNumMan;

  /* Core */
  sprintf(output[OUT_235UPOWERCORE].cName,"235UPowerCore");
  sprintf(output[OUT_235UPOWERCORE].cDescr,"Total Core Power Generated by 235U");
  sprintf(output[OUT_235UPOWERCORE].cNeg,"TW");
  output[OUT_235UPOWERCORE].bNeg = 1;
  output[OUT_235UPOWERCORE].dNeg = 1e-12; 
  output[OUT_235UPOWERCORE].iNum = 1;
  fnWrite[OUT_235UPOWERCORE] = &Write235UPowerCore;

  sprintf(output[OUT_235UMASSCORE].cName,"235UMassCore");
  sprintf(output[OUT_235UMASSCORE].cDescr,"Total Core Mass of 235U");
  sprintf(output[OUT_235UMASSCORE].cNeg,"Earth Masses");
  output[OUT_235UMASSCORE].bNeg = 1;
  output[OUT_235UMASSCORE].dNeg = MEARTH;
  output[OUT_235UMASSCORE].iNum = 1;
  fnWrite[OUT_235UMASSCORE] = &Write235UMassCore;
  
  sprintf(output[OUT_235UNUMCORE].cName,"235UNumCore");
  sprintf(output[OUT_235UNUMCORE].cDescr,"Total Core Number of 235U Atoms");
  sprintf(output[OUT_235UNUMCORE].cNeg,"Initial Primordial Earth Number");
  output[OUT_235UNUMCORE].bNeg = 1;
  output[OUT_235UNUMCORE].dNeg = 1;
  output[OUT_235UNUMCORE].iNum = 1;
  fnWrite[OUT_235UNUMCORE] = &Write235UNumCore;
      
  /* Totals */
  
  sprintf(output[OUT_RADPOWERMAN].cName,"RadPowerMan");
  sprintf(output[OUT_RADPOWERMAN].cDescr,"Total Power Generated by Radiogenic Nuclides in Mantle");
  sprintf(output[OUT_RADPOWERMAN].cNeg,"TW");
  output[OUT_RADPOWERMAN].bNeg = 1;
  output[OUT_RADPOWERMAN].dNeg = 1e-12; 
  output[OUT_RADPOWERMAN].iNum = 1;
  fnWrite[OUT_RADPOWERMAN] = &WriteRadPowerMan;

  sprintf(output[OUT_RADPOWERCORE].cName,"RadPowerCore");
  sprintf(output[OUT_RADPOWERCORE].cDescr,"Total Power Generated by Radiogenic Nuclides in Core");
  sprintf(output[OUT_RADPOWERCORE].cNeg,"TW");
  output[OUT_RADPOWERCORE].bNeg = 1;
  output[OUT_RADPOWERCORE].dNeg = 1e-12; 
  output[OUT_RADPOWERCORE].iNum = 1;
  fnWrite[OUT_RADPOWERCORE] = &WriteRadPowerCore;

  sprintf(output[OUT_RADPOWERTOTAL].cName,"RadPowerTotal");
  sprintf(output[OUT_RADPOWERTOTAL].cDescr,"Total Power Generated by Radiogenic Nuclides in Total (M+C)");
  sprintf(output[OUT_RADPOWERTOTAL].cNeg,"TW");
  output[OUT_RADPOWERTOTAL].bNeg = 1;
  output[OUT_RADPOWERTOTAL].dNeg = 1e-12; 
  output[OUT_RADPOWERTOTAL].iNum = 1;
  fnWrite[OUT_RADPOWERTOTAL] = &WriteRadPowerTotal;

  sprintf(output[OUT_SURFENFLUXRADHEAT].cName,"SurfEnFluxRadheat");
  sprintf(output[OUT_SURFENFLUXRADHEAT].cDescr,"Total Power Generated by Radiogenic Nuclides");
  sprintf(output[OUT_SURFENFLUXRADHEAT].cNeg,"W/m^2");
  output[OUT_SURFENFLUXRADHEAT].bNeg = 1;
  output[OUT_SURFENFLUXRADHEAT].dNeg = 1; 
  output[OUT_SURFENFLUXRADHEAT].iNum = 1;
  fnWrite[OUT_SURFENFLUXRADHEAT] = &WriteSurfEnFluxRadheat;

}

void FinalizeOutputFunctionRadheat(OUTPUT *output,int iBody,int iModule) {
  output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxRadheat;
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

  module->fnInitializeControl[iBody][iModule] = &InitializeControlRadheat;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyRadheat;

  module->fnCountHalts[iBody][iModule] = &CountHaltsRadHeat;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsRadheat;
  module->fnLogBody[iBody][iModule] = &LogBodyRadheat;
  module->fnVerify[iBody][iModule] = &VerifyRadheat;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltRadheat;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyRadheat;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateRadheat;

  module->fnFinalizeUpdate40KNumMan[iBody][iModule] = &FinalizeUpdate40KNumManRadheat;
  module->fnFinalizeUpdate232ThNumMan[iBody][iModule] = &FinalizeUpdate232ThNumManRadheat;
  module->fnFinalizeUpdate238UNumMan[iBody][iModule] = &FinalizeUpdate238UNumManRadheat;
  module->fnFinalizeUpdate235UNumMan[iBody][iModule] = &FinalizeUpdate235UNumManRadheat;

  //  NEED TO ADD CORE HERE?
  module->fnFinalizeUpdate40KNumCore[iBody][iModule] = &FinalizeUpdate40KNumCoreRadheat;
  module->fnFinalizeUpdate232ThNumCore[iBody][iModule] = &FinalizeUpdate232ThNumCoreRadheat;
  module->fnFinalizeUpdate238UNumCore[iBody][iModule] = &FinalizeUpdate238UNumCoreRadheat;
  module->fnFinalizeUpdate235UNumCore[iBody][iModule] = &FinalizeUpdate235UNumCoreRadheat;
  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionRadheat;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionRadheat;

}

/************* RADHEAT Functions ************/
// FIRST batch of subroutines are general for any species.
// N = N_0 * exp(-t/lambda)
// dN/dt = -(N_0/lambda) * exp(-t/lambda)

double fdRadPowerMan(BODY *body,UPDATE *update,int iBody) {
  return -(*(update[iBody].pdD238UNumManDt))*ENERGY238U - (*(update[iBody].pdD235UNumManDt))*ENERGY235U - (*(update[iBody].pdD232ThNumManDt))*ENERGY232TH - (*(update[iBody].pdD40KNumManDt))*ENERGY40K;
}

double fdRadPowerCore(BODY *body,UPDATE *update,int iBody) {
  return -(*(update[iBody].pdD238UNumCoreDt))*ENERGY238U - (*(update[iBody].pdD235UNumCoreDt))*ENERGY235U - (*(update[iBody].pdD232ThNumCoreDt))*ENERGY232TH - (*(update[iBody].pdD40KNumCoreDt))*ENERGY40K;
}

double fdRadPowerTotal(BODY *body,UPDATE *update,int iBody) {
    double dPowerMan;
    double dPowerCore;
    dPowerMan = fdRadPowerMan(body,update,iBody);
    dPowerCore = fdRadPowerCore(body,update,iBody);
    return dPowerMan+dPowerCore;
}

/* This is part of output[OUT_SURFENFLUX].fnOutput */
double fdSurfEnFluxRadheat(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  double dPower;

  dPower = fdRadPowerMan(body,update,iBody);
  return dPower/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
}

double fdRadheatConst(double dNum,double dAge,double dHalfLife) {
  return dNum/(exp(-dAge/dHalfLife));
}

double fdDNumRadDt(double dConst,double dHalfLife,double dAge) {  //dN/dt, can be used by any radioactive system?
  return -dConst/dHalfLife*exp(-dAge/dHalfLife);
}

double fdRadPower(double dConst,double dHalfLife,double dAge) {
    return dConst*exp(-dHalfLife/dAge);                      //Here const=N_0*energy_i*lambda, where energy_i=erg/num.
}

double fdRadEnFlux(double dConst,double dHalfLife,double dAge,double dRadius) {
  return fdRadPower(dConst,dHalfLife,dAge)/(4*PI*dRadius*dRadius);
}

//SECOND batch of subroutines are for individual variables of species.
/* Constant coefficients */
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

double fd40KPowerMan(BODY *body,SYSTEM *system,int *iaBody,int iBody) {
  return fdRadPower(body[iBody].d40KConstMan,HALFLIFE40K,body[iBody].dAge);   //redirects to fdRadPower
}

double fd232ThPowerMan(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d232ThConstMan,HALFLIFE232TH,body[iBody].dAge);    //redirects to fdRadPower
}

double fd238UPowerMan(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d238UConstMan,HALFLIFE238U,body[iBody].dAge);    //redirects to fdRadPower
}

double fd235UPowerMan(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d235UConstMan,HALFLIFE235U,body[iBody].dAge);    //redirects to fdRadPower
}

/* Energy Flux */
double fd40KEnFlux(BODY *body,SYSTEM *system,int *iaBody,int iBody) {
  return fdRadEnFlux(body[iBody].d40KConstMan,HALFLIFE40K,body[iBody].dAge,body[iBody].dRadius);
}

double fd232ThEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d232ThConstMan,HALFLIFE232TH,body[iBody].dAge,body[iBody].dRadius);
}

double fd238UEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d238UConstMan,HALFLIFE238U,body[iBody].dAge,body[iBody].dRadius);
}

double fd235UEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d235UConstMan,HALFLIFE235U,body[iBody].dAge,body[iBody].dRadius);
}

/* DN/Dt */

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
