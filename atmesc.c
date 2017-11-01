/********************** ATMESC.C **********************/
/*
 * Rodrigo Luger, Tue May 12 11:11:38 PDT 2015
 *
 * Subroutines that control the integration of the
 * atmospheric escape model.
 *
*/
// NOTE: Still need to edit module.c, update.c

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void BodyCopyAtmEsc(BODY *dest,BODY *src,int foo,int iNumBodies,int iBody) {
  dest[iBody].dSurfaceWaterMass = src[iBody].dSurfaceWaterMass;
  dest[iBody].dOxygenMass = src[iBody].dOxygenMass;
  dest[iBody].dOxygenMantleMass = src[iBody].dOxygenMantleMass;
  dest[iBody].dEnvelopeMass = src[iBody].dEnvelopeMass;
  dest[iBody].dXFrac = src[iBody].dXFrac;
  dest[iBody].dAtmXAbsEffH = src[iBody].dAtmXAbsEffH;
  dest[iBody].dAtmXAbsEffH2O = src[iBody].dAtmXAbsEffH2O;
  dest[iBody].dMinSurfaceWaterMass = src[iBody].dMinSurfaceWaterMass;
  dest[iBody].dMinEnvelopeMass = src[iBody].dMinEnvelopeMass;
  dest[iBody].iWaterLossModel = src[iBody].iWaterLossModel;
  dest[iBody].dKTide = src[iBody].dKTide;
  dest[iBody].dMDotWater = src[iBody].dMDotWater;
  dest[iBody].dFHRef = src[iBody].dFHRef;
  dest[iBody].dOxygenEta = src[iBody].dOxygenEta;
  dest[iBody].dCrossoverMass = src[iBody].dCrossoverMass;
  dest[iBody].bRunaway = src[iBody].bRunaway;
  dest[iBody].iWaterEscapeRegime = src[iBody].iWaterEscapeRegime;
  dest[iBody].dFHDiffLim = src[iBody].dFHDiffLim;
  dest[iBody].iPlanetRadiusModel = src[iBody].iPlanetRadiusModel;
  dest[iBody].bInstantO2Sink = src[iBody].bInstantO2Sink;
  dest[iBody].dRGDuration = src[iBody].dRGDuration;
  dest[iBody].dRadXUV = src[iBody].dRadXUV;
  dest[iBody].dRadSolid = src[iBody].dRadSolid;
  dest[iBody].dPresXUV = src[iBody].dPresXUV;
  dest[iBody].dScaleHeight = src[iBody].dScaleHeight;
  dest[iBody].dThermTemp = src[iBody].dThermTemp;
  dest[iBody].dAtmGasConst = src[iBody].dAtmGasConst;
  dest[iBody].dFXUV = src[iBody].dFXUV;
  dest[iBody].bCalcFXUV = src[iBody].bCalcFXUV;

}

/**************** ATMESC options ********************/

void ReadFXUV(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile){
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dFXUV = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dFXUV = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFXUV = options->dDefault;
}

// Lehmer: Thermosphere temperature
void ReadThermTemp(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile){
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dThermTemp = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dThermTemp = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dThermTemp = options->dDefault;
}

// Lehmer: Atmospheric Gas constant
void ReadAtmGasConst(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile){
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dAtmGasConst = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dAtmGasConst = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dAtmGasConst = options->dDefault;
}

// Lehmer: Pressure where XUV absorption is unity
void ReadPresXUV(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile){
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dPresXUV = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dPresXUV = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dPresXUV = options->dDefault;
}

void ReadWaterLossModel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"lb15",4)) {
      body[iFile-1].iWaterLossModel = ATMESC_LB15;
    } else if (!memcmp(sLower(cTmp),"lbex",4)) {
      body[iFile-1].iWaterLossModel = ATMESC_LBEXACT;
    } else if (!memcmp(sLower(cTmp),"tian",4)) {
      body[iFile-1].iWaterLossModel = ATMESC_TIAN;
    } else {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are LB15, LBEXACT, or TIAN.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].iWaterLossModel = ATMESC_LBEXACT;
}

void ReadPlanetRadiusModel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"lo",2)) {
      body[iFile-1].iPlanetRadiusModel = ATMESC_LOP12;
    } else if (!memcmp(sLower(cTmp),"le",2)) {
      body[iFile-1].iPlanetRadiusModel = ATMESC_LEHMER17;
    } else if (!memcmp(sLower(cTmp),"pr",2)) {
      body[iFile-1].iPlanetRadiusModel = ATMESC_PROXCENB;
    } else if (!memcmp(sLower(cTmp),"no",2)) {
      body[iFile-1].iPlanetRadiusModel = ATMESC_NONE;
    }
     else {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are LOPEZ12, PROXCENB, LEHMER17 or NONE.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].iPlanetRadiusModel = ATMESC_NONE;
}

void ReadInstantO2Sink(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].bInstantO2Sink = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      AssignDefaultInt(options,&body[iFile-1].bInstantO2Sink,files->iNumInputs);
}

void ReadXFrac(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: %s must be >= 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    body[iFile-1].dXFrac = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dXFrac = options->dDefault;
}

void ReadAtmXAbsEffH(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: %s must be >= 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    body[iFile-1].dAtmXAbsEffH = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dAtmXAbsEffH = options->dDefault;
}

void ReadAtmXAbsEffH2O(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: %s must be >= 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    body[iFile-1].dAtmXAbsEffH2O = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dAtmXAbsEffH2O = options->dDefault;
}

void ReadEnvelopeMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dEnvelopeMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dEnvelopeMass = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dEnvelopeMass = options->dDefault;
}

void ReadOxygenMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: %s must be >= 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    body[iFile-1].dOxygenMass = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dOxygenMass = options->dDefault;
}

void ReadOxygenMantleMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: %s must be >= 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    body[iFile-1].dOxygenMantleMass = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dOxygenMantleMass = options->dDefault;
}

void ReadSurfaceWaterMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dSurfaceWaterMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dSurfaceWaterMass = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dSurfaceWaterMass = options->dDefault;
}

/* Halts */

void ReadHaltMinSurfaceWaterMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Halt[iFile-1].bSurfaceDesiccated = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultInt(options,&control->Halt[iFile-1].bSurfaceDesiccated,files->iNumInputs);
  }
}

void ReadMinSurfaceWaterMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dMinSurfaceWaterMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dMinSurfaceWaterMass = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dMinSurfaceWaterMass = options->dDefault;
}

void ReadHaltMinEnvelopeMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Halt[iFile-1].bEnvelopeGone = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultInt(options,&control->Halt[iFile-1].bEnvelopeGone,files->iNumInputs);
  }
}

void ReadMinEnvelopeMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dMinEnvelopeMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dMinEnvelopeMass = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dMinEnvelopeMass = options->dDefault;
}

void InitializeOptionsAtmEsc(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  sprintf(options[OPT_XFRAC].cName,"dXFrac");
  sprintf(options[OPT_XFRAC].cDescr,"Fraction of planet radius in X-ray/XUV");
  sprintf(options[OPT_XFRAC].cDefault,"1");
  options[OPT_XFRAC].dDefault = 1;
  options[OPT_XFRAC].iType = 2;
  options[OPT_XFRAC].iMultiFile = 1;
  fnRead[OPT_XFRAC] = &ReadXFrac;

  sprintf(options[OPT_ATMXABSEFFH].cName,"dAtmXAbsEffH");
  sprintf(options[OPT_ATMXABSEFFH].cDescr,"Hydrogen X-ray/XUV absorption efficiency (epsilon)");
  sprintf(options[OPT_ATMXABSEFFH].cDefault,"0.15");
  options[OPT_ATMXABSEFFH].dDefault = 0.15;
  options[OPT_ATMXABSEFFH].iType = 2;
  options[OPT_ATMXABSEFFH].iMultiFile = 1;
  fnRead[OPT_ATMXABSEFFH] = &ReadAtmXAbsEffH;

  sprintf(options[OPT_ATMXABSEFFH2O].cName,"dAtmXAbsEffH2O");
  sprintf(options[OPT_ATMXABSEFFH2O].cDescr,"Water X-ray/XUV absorption efficiency (epsilon)");
  sprintf(options[OPT_ATMXABSEFFH2O].cDefault,"0.30");
  options[OPT_ATMXABSEFFH2O].dDefault = 0.15;
  options[OPT_ATMXABSEFFH2O].iType = 2;
  options[OPT_ATMXABSEFFH2O].iMultiFile = 1;
  fnRead[OPT_ATMXABSEFFH2O] = &ReadAtmXAbsEffH2O;

  sprintf(options[OPT_SURFACEWATERMASS].cName,"dSurfWaterMass");
  sprintf(options[OPT_SURFACEWATERMASS].cDescr,"Initial Surface Water Mass");
  sprintf(options[OPT_SURFACEWATERMASS].cDefault,"0");
  options[OPT_SURFACEWATERMASS].dDefault = 0;
  options[OPT_SURFACEWATERMASS].iType = 2;
  options[OPT_SURFACEWATERMASS].iMultiFile = 1;
  options[OPT_SURFACEWATERMASS].dNeg = TOMASS;
  sprintf(options[OPT_SURFACEWATERMASS].cNeg,"Terrestrial Oceans (TO)");
  fnRead[OPT_SURFACEWATERMASS] = &ReadSurfaceWaterMass;

  sprintf(options[OPT_OXYGENMASS].cName,"dOxygenMass");
  sprintf(options[OPT_OXYGENMASS].cDescr,"Initial Oxygen Mass");
  sprintf(options[OPT_OXYGENMASS].cDefault,"0");
  options[OPT_OXYGENMASS].dDefault = 0;
  options[OPT_OXYGENMASS].iType = 2;
  options[OPT_OXYGENMASS].iMultiFile = 1;
  fnRead[OPT_OXYGENMASS] = &ReadOxygenMass;

  sprintf(options[OPT_OXYGENMANTLEMASS].cName,"dOxygenMantleMass");
  sprintf(options[OPT_OXYGENMANTLEMASS].cDescr,"Initial Oxygen Mass in the Mantle");
  sprintf(options[OPT_OXYGENMANTLEMASS].cDefault,"0");
  options[OPT_OXYGENMANTLEMASS].dDefault = 0;
  options[OPT_OXYGENMANTLEMASS].iType = 2;
  options[OPT_OXYGENMANTLEMASS].iMultiFile = 1;
  fnRead[OPT_OXYGENMANTLEMASS] = &ReadOxygenMantleMass;

  sprintf(options[OPT_WATERLOSSMODEL].cName,"sWaterLossModel");
  sprintf(options[OPT_WATERLOSSMODEL].cDescr,"Water Loss and Oxygen Buildup Model");
  sprintf(options[OPT_WATERLOSSMODEL].cDefault,"LBEXACT");
  options[OPT_WATERLOSSMODEL].iType = 3;
  options[OPT_WATERLOSSMODEL].iMultiFile = 1;
  fnRead[OPT_WATERLOSSMODEL] = &ReadWaterLossModel;

  sprintf(options[OPT_PLANETRADIUSMODEL].cName,"sPlanetRadiusModel");
  sprintf(options[OPT_PLANETRADIUSMODEL].cDescr,"Gaseous Planet Radius Model");
  sprintf(options[OPT_PLANETRADIUSMODEL].cDefault,"NONE");
  options[OPT_PLANETRADIUSMODEL].iType = 3;
  options[OPT_PLANETRADIUSMODEL].iMultiFile = 1;
  fnRead[OPT_PLANETRADIUSMODEL] = &ReadPlanetRadiusModel;

  sprintf(options[OPT_INSTANTO2SINK].cName,"bInstantO2Sink");
  sprintf(options[OPT_INSTANTO2SINK].cDescr,"Is oxygen absorbed instantaneously at the surface?");
  sprintf(options[OPT_INSTANTO2SINK].cDefault,"0");
  options[OPT_INSTANTO2SINK].iType = 0;
  options[OPT_INSTANTO2SINK].iMultiFile = 1;
  fnRead[OPT_INSTANTO2SINK] = &ReadInstantO2Sink;

  sprintf(options[OPT_ENVELOPEMASS].cName,"dEnvelopeMass");
  sprintf(options[OPT_ENVELOPEMASS].cDescr,"Initial Envelope Mass");
  sprintf(options[OPT_ENVELOPEMASS].cDefault,"0");
  options[OPT_ENVELOPEMASS].dDefault = 0;
  options[OPT_ENVELOPEMASS].iType = 2;
  options[OPT_ENVELOPEMASS].iMultiFile = 1;
  options[OPT_ENVELOPEMASS].dNeg = MEARTH;
  sprintf(options[OPT_ENVELOPEMASS].cNeg,"Earth");
  fnRead[OPT_ENVELOPEMASS] = &ReadEnvelopeMass;

  sprintf(options[OPT_HALTDESICCATED].cName,"bHaltSurfaceDesiccated");
  sprintf(options[OPT_HALTDESICCATED].cDescr,"Halt at Desiccation?");
  sprintf(options[OPT_HALTDESICCATED].cDefault,"0");
  options[OPT_HALTDESICCATED].iType = 0;
  fnRead[OPT_HALTDESICCATED] = &ReadHaltMinSurfaceWaterMass;

  sprintf(options[OPT_HALTENVELOPEGONE].cName,"bHaltEnvelopeGone");
  sprintf(options[OPT_HALTENVELOPEGONE].cDescr,"Halt When Envelope Evaporates?");
  sprintf(options[OPT_HALTENVELOPEGONE].cDefault,"0");
  options[OPT_HALTENVELOPEGONE].iType = 0;
  fnRead[OPT_HALTENVELOPEGONE] = &ReadHaltMinEnvelopeMass;

  sprintf(options[OPT_MINSURFACEWATERMASS].cName,"dMinSurfWaterMass");
  sprintf(options[OPT_MINSURFACEWATERMASS].cDescr,"Minimum Surface Water Mass");
  sprintf(options[OPT_MINSURFACEWATERMASS].cDefault,"1.e-5 TO");
  options[OPT_MINSURFACEWATERMASS].dDefault = 1.e-5*TOMASS;
  options[OPT_MINSURFACEWATERMASS].iType = 2;
  options[OPT_MINSURFACEWATERMASS].dNeg = TOMASS;
  sprintf(options[OPT_MINSURFACEWATERMASS].cNeg,"Terrestrial Oceans (TO)");
  fnRead[OPT_MINSURFACEWATERMASS] = &ReadMinSurfaceWaterMass;

  sprintf(options[OPT_MINENVELOPEMASS].cName,"dMinEnvelopeMass");
  sprintf(options[OPT_MINENVELOPEMASS].cDescr,"Minimum Envelope Mass");
  sprintf(options[OPT_MINENVELOPEMASS].cDefault,"1.e-8 Earth");
  options[OPT_MINENVELOPEMASS].dDefault = 1.e-8*MEARTH;
  options[OPT_MINENVELOPEMASS].iType = 2;
  options[OPT_MINENVELOPEMASS].dNeg = MEARTH;
  sprintf(options[OPT_MINENVELOPEMASS].cNeg,"Earth");
  fnRead[OPT_MINENVELOPEMASS] = &ReadMinEnvelopeMass;

  sprintf(options[OPT_THERMTEMP].cName,"dThermTemp");
  sprintf(options[OPT_THERMTEMP].cDescr,"Thermosphere temperature");
  sprintf(options[OPT_THERMTEMP].cDefault,"880");
  options[OPT_THERMTEMP].dDefault = 880;
  options[OPT_THERMTEMP].iType = 2;
  options[OPT_THERMTEMP].iMultiFile = 1;
  fnRead[OPT_THERMTEMP] = &ReadThermTemp;

  sprintf(options[OPT_PRESXUV].cName,"dPresXUV");
  sprintf(options[OPT_PRESXUV].cDescr,"Pressure at base of Thermosphere");
  sprintf(options[OPT_PRESXUV].cDefault,"5 Pa");
  options[OPT_PRESXUV].dDefault = 5.0;
  options[OPT_PRESXUV].iType = 2;
  options[OPT_PRESXUV].iMultiFile = 1;
  fnRead[OPT_PRESXUV] = &ReadPresXUV;

  sprintf(options[OPT_ATMGASCONST].cName,"dAtmGasConst");
  sprintf(options[OPT_ATMGASCONST].cDescr,"Atmospheric Gas Constant");
  sprintf(options[OPT_ATMGASCONST].cDefault,"4124");
  options[OPT_ATMGASCONST].dDefault = 4124.0;
  options[OPT_ATMGASCONST].iType = 2;
  options[OPT_ATMGASCONST].iMultiFile = 1;
  fnRead[OPT_ATMGASCONST] = &ReadAtmGasConst;

  sprintf(options[OPT_FXUV].cName,"dFXUV");
  sprintf(options[OPT_FXUV].cDescr,"XUV Flux");
  options[OPT_FXUV].iType = 2;
  options[OPT_FXUV].iMultiFile = 1;
  fnRead[OPT_FXUV] = &ReadFXUV;
}

void ReadOptionsAtmEsc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTATMESC;iOpt<OPTENDATMESC;iOpt++) {
    if (options[iOpt].iType != -1)
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}

/******************* Verify ATMESC ******************/

void VerifySurfaceWaterMass(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iSurfaceWaterMass][0] = 1;
  update[iBody].iNumBodies[update[iBody].iSurfaceWaterMass][0] = 1;
  update[iBody].iaBody[update[iBody].iSurfaceWaterMass][0] = malloc(update[iBody].iNumBodies[update[iBody].iSurfaceWaterMass][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iSurfaceWaterMass][0][0] = iBody;

  update[iBody].pdDSurfaceWaterMassDtAtmesc = &update[iBody].daDerivProc[update[iBody].iSurfaceWaterMass][0];
  fnUpdate[iBody][update[iBody].iSurfaceWaterMass][0] = &fdDSurfaceWaterMassDt;
}

void VerifyOxygenMass(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iOxygenMass][0] = 1;
  update[iBody].iNumBodies[update[iBody].iOxygenMass][0] = 1;
  update[iBody].iaBody[update[iBody].iOxygenMass][0] = malloc(update[iBody].iNumBodies[update[iBody].iOxygenMass][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iOxygenMass][0][0] = iBody;

  update[iBody].pdDOxygenMassDtAtmesc = &update[iBody].daDerivProc[update[iBody].iOxygenMass][0];
  fnUpdate[iBody][update[iBody].iOxygenMass][0] = &fdDOxygenMassDt;
}

void VerifyOxygenMantleMass(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iOxygenMantleMass][0] = 1;
  update[iBody].iNumBodies[update[iBody].iOxygenMantleMass][0] = 1;
  update[iBody].iaBody[update[iBody].iOxygenMantleMass][0] = malloc(update[iBody].iNumBodies[update[iBody].iOxygenMantleMass][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iOxygenMantleMass][0][0] = iBody;

  update[iBody].pdDOxygenMantleMassDtAtmesc = &update[iBody].daDerivProc[update[iBody].iOxygenMantleMass][0];
  fnUpdate[iBody][update[iBody].iOxygenMantleMass][0] = &fdDOxygenMantleMassDt;
}

void VerifyEnvelopeMass(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iEnvelopeMass][0] = 1;
  update[iBody].iNumBodies[update[iBody].iEnvelopeMass][0] = 1;
  update[iBody].iaBody[update[iBody].iEnvelopeMass][0] = malloc(update[iBody].iNumBodies[update[iBody].iEnvelopeMass][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iEnvelopeMass][0][0] = iBody;

  update[iBody].pdDEnvelopeMassDtAtmesc = &update[iBody].daDerivProc[update[iBody].iEnvelopeMass][0];
  fnUpdate[iBody][update[iBody].iEnvelopeMass][0] = &fdDEnvelopeMassDt;
}

void VerifyMassAtmEsc(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iMass][0] = 1;
  update[iBody].iNumBodies[update[iBody].iMass][0] = 1;
  update[iBody].iaBody[update[iBody].iMass][0] = malloc(update[iBody].iNumBodies[update[iBody].iMass][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iMass][0][0] = iBody;

  update[iBody].pdDMassDtAtmesc = &update[iBody].daDerivProc[update[iBody].iMass][0];
  fnUpdate[iBody][update[iBody].iMass][0] = &fdDEnvelopeMassDt;
}

void VerifyRadiusAtmEsc(BODY *body, CONTROL *control, OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  // Assign radius
  if (body[iBody].iPlanetRadiusModel == ATMESC_LOP12) {
    body[iBody].dRadius = fdLopezRadius(body[iBody].dMass, body[iBody].dEnvelopeMass / body[iBody].dMass, 10., body[iBody].dAge, 1);

    if (options[OPT_RADIUS].iLine[iBody+1] >= 0) {
      // User specified radius, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT)
        printf("WARNING: Radius set for body %d, but this value will be computed from the grid.\n", iBody);
    }
  } else if (body[iBody].iPlanetRadiusModel == ATMESC_PROXCENB) {
    body[iBody].dRadius = fdProximaCenBRadius(body[iBody].dEnvelopeMass / body[iBody].dMass, body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_RADIUS].iLine[iBody+1] >= 0) {
      // User specified radius, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT)
        printf("WARNING: Radius set for body %d, but this value will be computed from the grid.\n", iBody);
    }
  }

  update[iBody].iaType[update[iBody].iRadius][0] = 0;
  update[iBody].iNumBodies[update[iBody].iRadius][0] = 1;
  update[iBody].iaBody[update[iBody].iRadius][0] = malloc(update[iBody].iNumBodies[update[iBody].iRadius][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iRadius][0][0] = iBody;

  update[iBody].pdRadiusAtmesc = &update[iBody].daDerivProc[update[iBody].iRadius][0];   // NOTE: This points to the VALUE of the radius
  fnUpdate[iBody][update[iBody].iRadius][0] = &fdPlanetRadius;                            // NOTE: Same here!

}

void fnForceBehaviorAtmEsc(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {

  if ((body[iBody].dSurfaceWaterMass <= body[iBody].dMinSurfaceWaterMass) && (body[iBody].dSurfaceWaterMass > 0.)){
    // Let's desiccate this planet.
    body[iBody].dSurfaceWaterMass = 0.;
  }
  if ((body[iBody].dEnvelopeMass <= body[iBody].dMinEnvelopeMass) && (body[iBody].dEnvelopeMass > 0.)){
    // Let's remove its envelope.
    body[iBody].dEnvelopeMass = 0.;
  }
}

void fnPropertiesAtmEsc(BODY *body, EVOLVE *evolve, UPDATE *update, int iBody) {

  if (body[iBody].iPlanetRadiusModel == ATMESC_LEHMER17) {
    body[iBody].dRadSolid = 1.3 * pow(body[iBody].dMass - body[iBody].dEnvelopeMass, 0.27);
    body[iBody].dGravAccel = BIGG * (body[iBody].dMass - body[iBody].dEnvelopeMass) / (body[iBody].dRadSolid * body[iBody].dRadSolid);
    body[iBody].dScaleHeight = body[iBody].dAtmGasConst * body[iBody].dThermTemp / body[iBody].dGravAccel;
    body[iBody].dRadXUV = fdLehmerRadius(body[iBody].dEnvelopeMass, body[iBody].dGravAccel, body[iBody].dRadSolid, body[iBody].dPresXUV, body[iBody].dScaleHeight,0);
  }

  // Ktide (due to body zero only). WARNING: not suited for binary...
  double xi = (pow(body[iBody].dMass / (3. * body[0].dMass), (1. / 3)) *
               body[iBody].dSemi) / (body[iBody].dRadius * body[iBody].dXFrac);

  // For circumbinary planets, assume no Ktide enhancement (ehhhhh sketchy)
  if(body[iBody].bBinary && body[iBody].iBodyType == 0)
  {
      body[iBody].dKTide = 1.0;
  }
  else
  {
      if (xi > 1)
        body[iBody].dKTide = (1 - 3 / (2 * xi) + 1 / (2 * pow(xi, 3)));
      else
        body[iBody].dKTide = 0;
  }

  // The XUV flux
  if (body[iBody].bCalcFXUV){
    body[iBody].dFXUV = fdInsolation(body, iBody, 1);
  }

  // Reference hydrogen flux for the water loss
  body[iBody].dFHRef = (body[iBody].dAtmXAbsEffH2O * body[iBody].dFXUV * body[iBody].dRadius) /
                       (4 * BIGG * body[iBody].dMass * body[iBody].dKTide * ATOMMASS);

  // Surface gravity
  double g = (BIGG * body[iBody].dMass) / (body[iBody].dRadius * body[iBody].dRadius);

  // Oxygen mixing ratio
  double XO = fdAtomicOxygenMixingRatio(body[iBody].dSurfaceWaterMass, body[iBody].dOxygenMass);

  // Diffusion-limited H escape rate
  body[iBody].dFHDiffLim = BDIFF * g * ATOMMASS * (QOH - 1.) / (KBOLTZ * THERMT * (1. + XO / (1. - XO)));

  // Is water escaping?
  if (!fbDoesWaterEscape(body, iBody)) {

    body[iBody].dOxygenEta = 0;
    body[iBody].dCrossoverMass = 0;
    body[iBody].bRunaway = 0;
    body[iBody].iWaterEscapeRegime = ATMESC_NONE;
    body[iBody].dMDotWater = 0;

  } else {

    body[iBody].bRunaway = 1;

    // Select an escape/oxygen buildup model
    if (body[iBody].iWaterLossModel == ATMESC_LB15) {

      // Luger and Barnes (2015)
      double x = (KBOLTZ * THERMT * body[iBody].dFHRef) / (10 * BDIFF * g * ATOMMASS);
      if (x < 1) {
        body[iBody].dOxygenEta = 0;
        body[iBody].dCrossoverMass = ATOMMASS + 1.5 * KBOLTZ * THERMT * body[iBody].dFHRef / (BDIFF * g);
      } else {
        body[iBody].dOxygenEta = (x - 1) / (x + 8);
        body[iBody].dCrossoverMass = 43. / 3. * ATOMMASS + KBOLTZ * THERMT * body[iBody].dFHRef / (6 * BDIFF * g);
      }

    } else if ((body[iBody].iWaterLossModel == ATMESC_LBEXACT) | (body[iBody].iWaterLossModel == ATMESC_TIAN)) {

      double x = (QOH - 1.) * (1. - XO) * (BDIFF * g * ATOMMASS) / (KBOLTZ * THERMT);
      double FH;
      double rat;

      // Get the crossover mass
      if (body[iBody].dFHRef < x) {

        // mcross < mo
        body[iBody].dCrossoverMass = ATOMMASS + (1. / (1. - XO)) * (KBOLTZ * THERMT * body[iBody].dFHRef) / (BDIFF * g);
        FH = body[iBody].dFHRef;
        rat = (body[iBody].dCrossoverMass / ATOMMASS - QOH) / (body[iBody].dCrossoverMass / ATOMMASS - 1.);
        body[iBody].dOxygenEta = 0;

      } else {

        // mcross >= mo
        double num = 1. + (XO / (1. - XO)) * QOH * QOH;
        double den = 1. + (XO / (1. - XO)) * QOH;
        body[iBody].dCrossoverMass = ATOMMASS * num / den + (KBOLTZ * THERMT * body[iBody].dFHRef) / (1 + XO * (QOH - 1) * BDIFF * g);
        rat = (body[iBody].dCrossoverMass / ATOMMASS - QOH) / (body[iBody].dCrossoverMass / ATOMMASS - 1.);
        FH = body[iBody].dFHRef * pow(1. + (XO / (1. + XO)) * QOH * rat, -1);
        body[iBody].dOxygenEta = 2 * XO / (1. - XO) * rat;

      }

    }

    if ((XO > 0.6) && (body[iBody].iWaterLossModel == ATMESC_LBEXACT)) {
      // Schaefer et al. (2016) prescription, section 2.2
      // NOTE: Perhaps a better criterion is (body[iBody].dOxygenEta > 1),
      // which ensures oxygen never escapes faster than it is being produced?
      body[iBody].iWaterEscapeRegime = ATMESC_DIFFLIM;
      body[iBody].dOxygenEta = 0;
      body[iBody].dMDotWater = body[iBody].dFHDiffLim * (4 * ATOMMASS * PI * body[iBody].dRadius * body[iBody].dRadius);
    } else {
      // In the Tian model, oxygen escapes when it's the dominant species. I think this is wrong...
      body[iBody].iWaterEscapeRegime = ATMESC_ELIM;
      body[iBody].dMDotWater = body[iBody].dFHRef * (4 * ATOMMASS * PI * body[iBody].dRadius * body[iBody].dRadius);
    }
  }

}

void VerifyAtmEsc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int bAtmEsc=0;

  /* AtmEsc is active for this body if this subroutine is called. */

  // Is FXUV specified in input file?
  if (options[OPT_FXUV].iLine[iBody+1] > -1){
    body[iBody].bCalcFXUV = 0;
  }
  else{
    body[iBody].bCalcFXUV = 1;
  }

  if (body[iBody].iPlanetRadiusModel == ATMESC_LEHMER17) {
    body[iBody].dRadSolid = 1.3 * pow(body[iBody].dMass - body[iBody].dEnvelopeMass, 0.27);
    body[iBody].dGravAccel = BIGG * (body[iBody].dMass - body[iBody].dEnvelopeMass) / (body[iBody].dRadSolid * body[iBody].dRadSolid);
    body[iBody].dScaleHeight = body[iBody].dAtmGasConst * body[iBody].dThermTemp / body[iBody].dGravAccel;
    body[iBody].dRadXUV = fdLehmerRadius(body[iBody].dEnvelopeMass, body[iBody].dGravAccel, body[iBody].dRadSolid, body[iBody].dPresXUV, body[iBody].dScaleHeight,0);
}

  if (body[iBody].dSurfaceWaterMass > 0) {
    VerifySurfaceWaterMass(body,options,update,body[iBody].dAge,fnUpdate,iBody);
    VerifyOxygenMass(body,options,update,body[iBody].dAge,fnUpdate,iBody);
    VerifyOxygenMantleMass(body,options,update,body[iBody].dAge,fnUpdate,iBody);
    bAtmEsc = 1;
  }

  if (body[iBody].dEnvelopeMass > 0) {
    VerifyEnvelopeMass(body,options,update,body[iBody].dAge,fnUpdate,iBody);
    VerifyMassAtmEsc(body,options,update,body[iBody].dAge,fnUpdate,iBody);
    bAtmEsc = 1;
  }

  // Ensure envelope mass is physical
  if (body[iBody].dEnvelopeMass > body[iBody].dMass) {
    if (control->Io.iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: %s cannot be greater than %s in file %s.\n",options[OPT_ENVELOPEMASS].cName,options[OPT_MASS].cName,files->Infile[iBody+1].cIn);
    exit(EXIT_INPUT);
  }

  // Initialize rg duration
  body[iBody].dRGDuration = 0.;

  if (!bAtmEsc && control->Io.iVerbose >= VERBINPUT)
    fprintf(stderr,"WARNING: ATMESC called for body %s, but no atmosphere/water present!\n",body[iBody].cName);

  // Radius evolution
  if (update[iBody].iNumRadius > 1) {
    if (control->Io.iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Looks like there's more than one equation trying to set dRadius for body %d!", iBody);
    exit(EXIT_INPUT);
  }
  VerifyRadiusAtmEsc(body,control,options,update,body[iBody].dAge,fnUpdate,iBody);

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorAtmEsc;
  control->fnPropsAux[iBody][iModule] = &fnPropertiesAtmEsc;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyAtmEsc;

}

/**************** ATMESC update ****************/

void InitializeUpdateAtmEsc(BODY *body,UPDATE *update,int iBody) {
  if (body[iBody].dSurfaceWaterMass > 0) {
    if (update[iBody].iNumSurfaceWaterMass == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumSurfaceWaterMass++;

    if (update[iBody].iNumOxygenMass == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumOxygenMass++;

    if (update[iBody].iNumOxygenMantleMass == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumOxygenMantleMass++;
  }

  if (body[iBody].dEnvelopeMass > 0) {
    if (update[iBody].iNumEnvelopeMass == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumEnvelopeMass++;

    if (update[iBody].iNumMass == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumMass++;
  }

  if (body[iBody].dRadius > 0) {
    if (update[iBody].iNumRadius == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumRadius++;
  }

}

void FinalizeUpdateEccAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  /* Nothing */
}

void FinalizeUpdateSurfaceWaterMassAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumSurfaceWaterMass = (*iEqn)++;
}

void FinalizeUpdateOxygenMassAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumOxygenMass = (*iEqn)++;
}

void FinalizeUpdateOxygenMantleMassAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumOxygenMantleMass = (*iEqn)++;
}

void FinalizeUpdateEnvelopeMassAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumEnvelopeMass = (*iEqn)++;
}

void FinalizeUpdateMassAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumMass = (*iEqn)++;
}

void FinalizeUpdateOblAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  /* Nothing */
}

void FinalizeUpdateRotAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  /* Nothing */
}

void FinalizeUpdateSemiAtmEsc(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  /* Nothing */
}

void FinalizeUpdateRadiusAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumRadius = (*iEqn)++;
}

/***************** ATMESC Halts *****************/

int fbHaltSurfaceDesiccated(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (body[iBody].dSurfaceWaterMass <= body[iBody].dMinSurfaceWaterMass) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's surface water mass =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].dSurfaceWaterMass/TOMASS,io->iSciNot,io->iDigits);
      printf("TO.\n");
    }
    return 1;
  }
  return 0;
}

int fbHaltEnvelopeGone(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (body[iBody].dEnvelopeMass <= body[iBody].dMinEnvelopeMass) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's envelope mass =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].dEnvelopeMass/MEARTH,io->iSciNot,io->iDigits);
      printf("Earth Masses.\n");
    }
    return 1;
  }
  return 0;
}

void CountHaltsAtmEsc(HALT *halt,int *iHalt) {
  if (halt->bSurfaceDesiccated)
    (*iHalt)++;
  if (halt->bEnvelopeGone)
    (*iHalt)++;
}

void VerifyHaltAtmEsc(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {

  if (control->Halt[iBody].bSurfaceDesiccated)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltSurfaceDesiccated;

  if (control->Halt[iBody].bEnvelopeGone)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltEnvelopeGone;
}

/************* ATMESC Outputs ******************/

void HelpOutputAtmEsc(OUTPUT *output) {
  int iOut;

  printf("\n ------ ATMESC output ------\n");
  for (iOut=OUTSTARTATMESC;iOut<OUTENDATMESC;iOut++)
    WriteHelpOutput(&output[iOut]);
}

void WriteSurfaceWaterMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dSurfaceWaterMass;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }

}

void WriteOxygenMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dOxygenMass;

  if (output->bDoNeg[iBody]) {
    *dTmp *= 1.e-5 * ((BIGG * body[iBody].dMass) / (4. * PI * pow(body[iBody].dRadius, 4)));
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }

}

void WriteOxygenMantleMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dOxygenMantleMass;

  if (output->bDoNeg[iBody]) {
    *dTmp *= 1.e-5 * ((BIGG * body[iBody].dMass) / (4. * PI * pow(body[iBody].dRadius, 4)));
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }

}

void WritePlanetRadius(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dRadius;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }

}

void WriteEnvelopeMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dEnvelopeMass;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}

void WriteRGLimit(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {

  // Get the RG flux
  double flux = fdHZRG14(body[0].dLuminosity, body[0].dTemperature, body[iBody].dEcc, body[iBody].dMass);

  // Convert to semi-major axis *at current eccentricity!*
  *dTmp = pow(4 * PI * flux /  (body[0].dLuminosity * pow((1 - body[iBody].dEcc * body[iBody].dEcc), 0.5)), -0.5);

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}

void WriteOxygenMixingRatio(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdAtomicOxygenMixingRatio(body[iBody].dSurfaceWaterMass, body[iBody].dOxygenMass);
  strcpy(cUnit,"");
}

void WriteOxygenEta(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dOxygenEta;
  strcpy(cUnit,"");
}

void WritePlanetRadXUV(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dRadXUV;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}

void WriteDEnvMassDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]){
  double dDeriv;

  dDeriv = *(update[iBody].pdDEnvelopeMassDtAtmesc);
  *dTmp = dDeriv;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime)/fdUnitsMass(units->iMass);

  }
}

void WriteThermTemp(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
 *dTmp = body[iBody].dThermTemp;

 if (output->bDoNeg[iBody]) {
   *dTmp *= output->dNeg;
   strcpy(cUnit,output->cNeg);
 } else { }
}

void WritePresSurf(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dPresSurf;

  if (output->bDoNeg[iBody]){
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WritePresXUV(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dPresXUV;

  if (output->bDoNeg[iBody]){
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteScaleHeight(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dScaleHeight;

  if (output->bDoNeg[iBody]){
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteAtmGasConst(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dAtmGasConst;

  if (output->bDoNeg[iBody]){
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteRadSolid(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dRadSolid;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}

void WriteFXUV(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFXUV;

  if (output->bDoNeg[iBody]){
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}



void InitializeOutputAtmEsc(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_SURFACEWATERMASS].cName,"SurfWaterMass");
  sprintf(output[OUT_SURFACEWATERMASS].cDescr,"Surface Water Mass");
  sprintf(output[OUT_SURFACEWATERMASS].cNeg,"TO");
  output[OUT_SURFACEWATERMASS].bNeg = 1;
  output[OUT_SURFACEWATERMASS].dNeg = 1./TOMASS;
  output[OUT_SURFACEWATERMASS].iNum = 1;
  output[OUT_SURFACEWATERMASS].iModuleBit = ATMESC;
  fnWrite[OUT_SURFACEWATERMASS] = &WriteSurfaceWaterMass;

  sprintf(output[OUT_PLANETRADIUS].cName,"PlanetRadius");
  sprintf(output[OUT_PLANETRADIUS].cDescr,"Planet Radius");
  sprintf(output[OUT_PLANETRADIUS].cNeg,"Earth Radii");
  output[OUT_PLANETRADIUS].bNeg = 1;
  output[OUT_PLANETRADIUS].dNeg = 1./REARTH;
  output[OUT_PLANETRADIUS].iNum = 1;
  output[OUT_PLANETRADIUS].iModuleBit = ATMESC;
  fnWrite[OUT_PLANETRADIUS] = &WritePlanetRadius;

  sprintf(output[OUT_OXYGENMASS].cName,"OxygenMass");
  sprintf(output[OUT_OXYGENMASS].cDescr,"Oxygen Mass");
  sprintf(output[OUT_OXYGENMASS].cNeg,"bars");
  output[OUT_OXYGENMASS].bNeg = 1;
  output[OUT_OXYGENMASS].dNeg = 1;
  output[OUT_OXYGENMASS].iNum = 1;
  output[OUT_OXYGENMASS].iModuleBit = ATMESC;
  fnWrite[OUT_OXYGENMASS] = &WriteOxygenMass;

  sprintf(output[OUT_OXYGENMANTLEMASS].cName,"OxygenMantleMass");
  sprintf(output[OUT_OXYGENMANTLEMASS].cDescr,"Mass of Oxygen in Mantle");
  sprintf(output[OUT_OXYGENMANTLEMASS].cNeg,"bars");
  output[OUT_OXYGENMANTLEMASS].bNeg = 1;
  output[OUT_OXYGENMANTLEMASS].dNeg = 1;
  output[OUT_OXYGENMANTLEMASS].iNum = 1;
  output[OUT_OXYGENMANTLEMASS].iModuleBit = ATMESC;
  fnWrite[OUT_OXYGENMANTLEMASS] = &WriteOxygenMantleMass;

  sprintf(output[OUT_RGLIMIT].cName,"RGLimit");
  sprintf(output[OUT_RGLIMIT].cDescr,"Runaway Greenhouse Semi-Major Axis");
  sprintf(output[OUT_RGLIMIT].cNeg,"AU");
  output[OUT_RGLIMIT].bNeg = 1;
  output[OUT_RGLIMIT].dNeg = 1. / AUCM;
  output[OUT_RGLIMIT].iNum = 1;
  output[OUT_RGLIMIT].iModuleBit = ATMESC;
  fnWrite[OUT_RGLIMIT] = &WriteRGLimit;

  sprintf(output[OUT_XO].cName,"XO");
  sprintf(output[OUT_XO].cDescr,"Atomic Oxygen Mixing Ratio in Upper Atmosphere");
  output[OUT_XO].bNeg = 0;
  output[OUT_XO].iNum = 1;
  output[OUT_XO].iModuleBit = ATMESC;
  fnWrite[OUT_XO] = &WriteOxygenMixingRatio;

  sprintf(output[OUT_ETAO].cName,"EtaO");
  sprintf(output[OUT_ETAO].cDescr,"Oxygen Eta Parameter (Luger and Barnes 2015)");
  output[OUT_ETAO].bNeg = 0;
  output[OUT_ETAO].iNum = 1;
  output[OUT_ETAO].iModuleBit = ATMESC;
  fnWrite[OUT_ETAO] = &WriteOxygenEta;

  sprintf(output[OUT_ENVELOPEMASS].cName,"EnvelopeMass");
  sprintf(output[OUT_ENVELOPEMASS].cDescr,"Envelope Mass");
  sprintf(output[OUT_ENVELOPEMASS].cNeg,"Earth");
  output[OUT_ENVELOPEMASS].bNeg = 1;
  output[OUT_ENVELOPEMASS].dNeg = 1./MEARTH;
  output[OUT_ENVELOPEMASS].iNum = 1;
  output[OUT_ENVELOPEMASS].iModuleBit = ATMESC;
  fnWrite[OUT_ENVELOPEMASS] = &WriteEnvelopeMass;


  sprintf(output[OUT_PLANETRADXUV].cName,"RadXUV");
  sprintf(output[OUT_PLANETRADXUV].cDescr,"XUV Radius separating hydro. dyn. escpape and equilibrium");
  sprintf(output[OUT_PLANETRADXUV].cNeg,"Earth Radii");
  output[OUT_PLANETRADXUV].bNeg = 1;
  output[OUT_PLANETRADXUV].dNeg = 1./REARTH;
  output[OUT_PLANETRADXUV].iNum = 1;
  output[OUT_PLANETRADXUV].iModuleBit = ATMESC;
  fnWrite[OUT_PLANETRADXUV] = &WritePlanetRadXUV;

  sprintf(output[OUT_DENVMASSDT].cName,"DEnvMassDt");
  sprintf(output[OUT_DENVMASSDT].cDescr,"Envelope Mass Loss Rate");
  sprintf(output[OUT_DENVMASSDT].cNeg,"kg/s");
  output[OUT_DENVMASSDT].bNeg = 1;
  output[OUT_DENVMASSDT].iNum = 1;
  output[OUT_DENVMASSDT].iModuleBit = ATMESC;
  fnWrite[OUT_DENVMASSDT] = &WriteDEnvMassDt;

  sprintf(output[OUT_THERMTEMP].cName,"ThermTemp");
  sprintf(output[OUT_THERMTEMP].cDescr,"Isothermal Atmospheric Temperature");
  sprintf(output[OUT_THERMTEMP].cNeg,"K");
  output[OUT_THERMTEMP].bNeg = 1;
  output[OUT_THERMTEMP].dNeg = 1; // default units are K.
  output[OUT_THERMTEMP].iNum = 1;
  output[OUT_THERMTEMP].iModuleBit = ATMESC;
  fnWrite[OUT_THERMTEMP] = &WriteThermTemp;

  sprintf(output[OUT_PRESSURF].cName,"PresSurf");
  sprintf(output[OUT_PRESSURF].cDescr,"Surface Pressure due to Atmosphere");
  sprintf(output[OUT_PRESSURF].cNeg,"Pa");
  output[OUT_PRESSURF].bNeg = 1;
  output[OUT_PRESSURF].dNeg = 1;
  output[OUT_PRESSURF].iNum = 1;
  output[OUT_PRESSURF].iModuleBit = ATMESC;
  fnWrite[OUT_PRESSURF] = &WritePresSurf;

  sprintf(output[OUT_PRESXUV].cName,"PresXUV");
  sprintf(output[OUT_PRESXUV].cDescr,"Pressure at base of Thermosphere");
  sprintf(output[OUT_PRESXUV].cNeg,"Pa");
  output[OUT_PRESXUV].bNeg = 1;
  output[OUT_PRESXUV].dNeg = 1;
  output[OUT_PRESXUV].iNum = 1;
  output[OUT_PRESXUV].iModuleBit = ATMESC;
  fnWrite[OUT_PRESXUV] = &WritePresXUV;

  sprintf(output[OUT_SCALEHEIGHT].cName,"ScaleHeight");
  sprintf(output[OUT_SCALEHEIGHT].cDescr,"Scaling factor in fdLehmerRadius");
  sprintf(output[OUT_SCALEHEIGHT].cNeg,"J s^2 / kg m");
  output[OUT_SCALEHEIGHT].bNeg = 1;
  output[OUT_SCALEHEIGHT].dNeg = 1;
  output[OUT_SCALEHEIGHT].iNum = 1;
  output[OUT_SCALEHEIGHT].iModuleBit = ATMESC;
  fnWrite[OUT_SCALEHEIGHT] = &WriteScaleHeight;

  sprintf(output[OUT_ATMGASCONST].cName,"AtmGasConst");
  sprintf(output[OUT_ATMGASCONST].cDescr,"Atmospheric Gas Constant");
  sprintf(output[OUT_ATMGASCONST].cNeg,"J / K kg");
  output[OUT_ATMGASCONST].bNeg = 1;
  output[OUT_ATMGASCONST].dNeg = 1;
  output[OUT_ATMGASCONST].iNum = 1;
  output[OUT_ATMGASCONST].iModuleBit = ATMESC;
  fnWrite[OUT_ATMGASCONST] = &WriteAtmGasConst;

  sprintf(output[OUT_RADSOLID].cName,"RadSolid");
  sprintf(output[OUT_RADSOLID].cDescr,"Radius to the solid surface");
  sprintf(output[OUT_RADSOLID].cNeg,"Earth Radii");
  output[OUT_RADSOLID].bNeg = 1;
  output[OUT_RADSOLID].dNeg = 1./REARTH;
  output[OUT_RADSOLID].iNum = 1;
  output[OUT_RADSOLID].iModuleBit = ATMESC;
  fnWrite[OUT_RADSOLID] = &WriteRadSolid;

  sprintf(output[OUT_FXUV].cName,"FXUV");
  sprintf(output[OUT_FXUV].cDescr,"XUV Flux");
  sprintf(output[OUT_FXUV].cNeg,"W/m^2");
  output[OUT_FXUV].bNeg = 1;
  output[OUT_FXUV].dNeg = 1;
  output[OUT_FXUV].iNum = 1;
  output[OUT_FXUV].iModuleBit = ATMESC;
  fnWrite[OUT_FXUV] = &WriteFXUV;

}

/************ ATMESC Logging Functions **************/

void LogOptionsAtmEsc(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- ATMESC Options -----\n\n");
  */
}

void LogAtmEsc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {

  /* Anything here?
  int iOut;

  fprintf(fp,"\n----- ATMESC PARAMETERS ------\n");
  for (iOut=OUTSTARTATMESC;iOut<OUTBODYSTARTATMESC;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyAtmEsc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;
  fprintf(fp,"----- ATMESC PARAMETERS (%s)------\n",body[iBody].cName);

  for (iOut=OUTSTARTATMESC;iOut<OUTENDATMESC;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }

  // TODO: Log this the standard way
  fprintf(fp,"(RGDuration) Runaway Greenhouse Duration [years]: %.5e\n", body[iBody].dRGDuration / YEARSEC);

}

void AddModuleAtmEsc(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = ATMESC;

  module->fnCountHalts[iBody][iModule] = &CountHaltsAtmEsc;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsAtmEsc;
  module->fnLogBody[iBody][iModule] = &LogBodyAtmEsc;
  module->fnVerify[iBody][iModule] = &VerifyAtmEsc;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltAtmEsc;

  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateAtmEsc;
  module->fnFinalizeUpdateSurfaceWaterMass[iBody][iModule] = &FinalizeUpdateSurfaceWaterMassAtmEsc;
  module->fnFinalizeUpdateOxygenMass[iBody][iModule] = &FinalizeUpdateOxygenMassAtmEsc;
  module->fnFinalizeUpdateOxygenMantleMass[iBody][iModule] = &FinalizeUpdateOxygenMantleMassAtmEsc;
  module->fnFinalizeUpdateEnvelopeMass[iBody][iModule] = &FinalizeUpdateEnvelopeMassAtmEsc;
  module->fnFinalizeUpdateMass[iBody][iModule] = &FinalizeUpdateEnvelopeMassAtmEsc;
  module->fnFinalizeUpdateRadius[iBody][iModule] = &FinalizeUpdateRadiusAtmEsc;
}

/************* ATMESC Functions ************/

double fdDSurfaceWaterMassDt(BODY *body,SYSTEM *system,int *iaBody) {

  if ((body[iaBody[0]].bRunaway) && (body[iaBody[0]].dSurfaceWaterMass > 0)) {

    // This takes care of both energy-limited and diffusion limited escape!
    return -(9. / (1 + 8 * body[iaBody[0]].dOxygenEta)) * body[iaBody[0]].dMDotWater;

  } else {

    return 0.;

  }
}

double fdDOxygenMassDt(BODY *body,SYSTEM *system,int *iaBody) {

  if ((body[iaBody[0]].bRunaway) && (!body[iaBody[0]].bInstantO2Sink) && (body[iaBody[0]].dSurfaceWaterMass > 0)) {

    //printf("Oxygen building up at %.3e years\n", body[iaBody[0]].dAge / YEARSEC); // DEBUG DEBUG DEBUG

    if (body[iaBody[0]].iWaterLossModel == ATMESC_LB15) {

      // Rodrigo and Barnes (2015)
      if (body[iaBody[0]].dCrossoverMass >= 16 * ATOMMASS)
        return (320. * PI * BIGG * ATOMMASS * ATOMMASS * BDIFF * body[iaBody[0]].dMass) / (KBOLTZ * THERMT);
      else
        return (8 - 8 * body[iaBody[0]].dOxygenEta) / (1 + 8 * body[iaBody[0]].dOxygenEta) * body[iaBody[0]].dMDotWater;

    } else {

      // Exact
      return (8 - 8 * body[iaBody[0]].dOxygenEta) / (1 + 8 * body[iaBody[0]].dOxygenEta) * body[iaBody[0]].dMDotWater;

    }
  } else {

    return 0.;

  }

}

double fdDOxygenMantleMassDt(BODY *body,SYSTEM *system,int *iaBody) {

  if ((body[iaBody[0]].bRunaway) && (body[iaBody[0]].bInstantO2Sink) && (body[iaBody[0]].dSurfaceWaterMass > 0)) {

    if (body[iaBody[0]].iWaterLossModel == ATMESC_LB15) {

      // Rodrigo and Barnes (2015)
      if (body[iaBody[0]].dCrossoverMass >= 16 * ATOMMASS)
        return (320. * PI * BIGG * ATOMMASS * ATOMMASS * BDIFF * body[iaBody[0]].dMass) / (KBOLTZ * THERMT);
      else
        return (8 - 8 * body[iaBody[0]].dOxygenEta) / (1 + 8 * body[iaBody[0]].dOxygenEta) * body[iaBody[0]].dMDotWater;

    } else {

      // Exact
      return (8 - 8 * body[iaBody[0]].dOxygenEta) / (1 + 8 * body[iaBody[0]].dOxygenEta) * body[iaBody[0]].dMDotWater;

    }
  } else {

    return 0.;

  }

}

double fdDEnvelopeMassDt(BODY *body,SYSTEM *system,int *iaBody) {

  // TODO: This needs to be moved. Ideally we'd just remove this equation from the matrix.
  if (body[iaBody[0]].dEnvelopeMass <= 0){
    return 0;
  }

  if (body[iaBody[0]].iPlanetRadiusModel == ATMESC_LEHMER17){
    
  	return -body[iaBody[0]].dAtmXAbsEffH * PI * body[iaBody[0]].dFXUV * pow(body[iaBody[0]].dRadXUV, 3.0) / ( BIGG * (body[iaBody[0]].dMass - body[iaBody[0]].dEnvelopeMass));

  }
  else{
  	return -body[iaBody[0]].dFHRef * (body[iaBody[0]].dAtmXAbsEffH / body[iaBody[0]].dAtmXAbsEffH2O) * (4 * ATOMMASS * PI * body[iaBody[0]].dRadius * body[iaBody[0]].dRadius);
  }

}

double fdSurfEnFluxAtmEsc(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  // This is silly, but necessary!
  return 0;
}

double fdPlanetRadius(BODY *body,SYSTEM *system,int *iaBody) {

  if (body[iaBody[0]].iPlanetRadiusModel == ATMESC_LEHMER17) {
    body[iaBody[0]].dRadXUV = fdLehmerRadius(body[iaBody[0]].dEnvelopeMass, body[iaBody[0]].dGravAccel, body[iaBody[0]].dRadSolid, body[iaBody[0]].dPresXUV, body[iaBody[0]].dScaleHeight,0);
    body[iaBody[0]].dPresSurf = fdLehmerRadius(body[iaBody[0]].dEnvelopeMass, body[iaBody[0]].dGravAccel, body[iaBody[0]].dRadSolid, body[iaBody[0]].dPresXUV, body[iaBody[0]].dScaleHeight,1);
  }

  double foo;
  if (body[iaBody[0]].iPlanetRadiusModel == ATMESC_LOP12) {
    foo = fdLopezRadius(body[iaBody[0]].dMass, body[iaBody[0]].dEnvelopeMass / body[iaBody[0]].dMass, 10., body[iaBody[0]].dAge, 1);
    if (!isnan(foo))
      return foo;
    else
      return body[iaBody[0]].dRadius;
  } else if (body[iaBody[0]].iPlanetRadiusModel == ATMESC_PROXCENB) {
    return fdProximaCenBRadius(body[iaBody[0]].dEnvelopeMass / body[iaBody[0]].dMass, body[iaBody[0]].dAge, body[iaBody[0]].dMass);
  } else
    return body[iaBody[0]].dRadius;


}

/************* ATMESC Helper Functions ************/

double fdInsolation(BODY *body, int iBody, int iXUV) {

  double flux;

  if (body[iBody].bBinary && body[iBody].iBodyType == 0) {

    // Body orbits two stars
    if (iXUV)
      flux = fdFluxExactBinary(body,iBody,body[0].dLXUV,body[1].dLXUV);
    else
      flux = fdFluxExactBinary(body,iBody,body[0].dLuminosity,body[1].dLuminosity);

  } else {

    // Body orbits one star
    if (iXUV)
      flux = body[0].dLXUV / (4 * PI * pow(body[iBody].dSemi, 2) *
             pow((1 - body[iBody].dEcc * body[iBody].dEcc), 0.5));
    else
      flux = body[0].dLuminosity / (4 * PI * pow(body[iBody].dSemi, 2) *
             pow((1 - body[iBody].dEcc * body[iBody].dEcc), 0.5));
  }

  return flux;

}

int fbDoesWaterEscape(BODY *body, int iBody) {
  // TODO: The checks below need to be moved. Ideally we'd
  // just remove this equation from the matrix if the
  // escape conditions are not met.

  // 1. Check if there's hydrogen to be lost; this happens first
  if (body[iBody].dEnvelopeMass > 0) {
    // (But let's still check whether the RG phase has ended)
    if ((body[iBody].dRGDuration == 0.) && (fdInsolation(body, iBody, 0) < fdHZRG14(body[0].dLuminosity, body[0].dTemperature, body[iBody].dEcc, body[iBody].dMass)))
      body[iBody].dRGDuration = body[iBody].dAge;
    return 0;
  }

  // 2. Check if planet is beyond RG limit; otherwise, assume the
  // cold trap prevents water loss.
  // NOTE: The RG flux limit below is calculated based on body zero's
  // spectrum! The Kopparapu+14 limit is for a single star only. This
  // approximation for a binary is only valid if the two stars have
  // similar spectral types, or if body zero dominates the flux.
  else if (fdInsolation(body, iBody, 0) < fdHZRG14(body[0].dLuminosity, body[0].dTemperature, body[iBody].dEcc, body[iBody].dMass)){
    if (body[iBody].dRGDuration == 0.)
      body[iBody].dRGDuration = body[iBody].dAge;
    return 0;
  }

  // 3. Is there still water to be lost?
  else if (body[iBody].dSurfaceWaterMass <= 0)
    return 0;

  else
    return 1;

}

double fdAtomicOxygenMixingRatio(double dSurfaceWaterMass, double dOxygenMass) {
  // Mixing ratio X_O of atomic oxygen in the upper atmosphere
  // assuming atmosphere is well-mixed up to the photolysis layer
  double NO2 = dOxygenMass / (32 * ATOMMASS);
  double NH2O = dSurfaceWaterMass / (18 * ATOMMASS);
  if (NH2O > 0)
    return 1. / (1 + 1. / (0.5 + NO2 / NH2O));
  else {
    if (NO2 > 0)
      return 1.;
    else
      return 0.;
  }
}

double fdHZRG14(double dLuminosity, double dTeff, double dEcc, double dPlanetMass) {
  // Do a simple log-linear fit to the Kopparapu+14 mass-dependent RG limit
  int i;
  double seff[3];
  double daCoeffs[2];

  double tstar = dTeff - 5780;
  double daLogMP[3] = {-1.0, 0., 0.69897};
  double seffsun[3] = {0.99, 1.107, 1.188};
  double a[3] = {1.209e-4, 1.332e-4, 1.433e-4};
  double b[3] = {1.404e-8, 1.58e-8, 1.707e-8};
  double c[3] = {-7.418e-12, -8.308e-12, -8.968e-12};
  double d[3] = {-1.713e-15, -1.931e-15, -2.084e-15};

  for (i=0;i<3;i++){
  	seff[i] = seffsun[i] + a[i]*tstar + b[i]*tstar*tstar + c[i]*pow(tstar,3) + d[i]*pow(tstar,4);
  }

  // BUG BUG BUG BUG BUG BUG BUG (August 20, 2016)
  // Something is wrong with this linear fit in the first ~5 Myr, as it diverges.
  // This ***does not*** affect ANY calculations for Proxima Cen b, since the planet
  // remains in the RG for the correct amount of time regardless of this issue.
  // But I do need to address this soon.

  fvLinearFit(daLogMP,seff,3,daCoeffs);

  return (daCoeffs[0]*log10(dPlanetMass/MEARTH) + daCoeffs[1]) * LSUN / (4 * PI * AUCM * AUCM);
}

void fvLinearFit(double *x, double *y, int iLen, double *daCoeffs){
	// Simple least squares linear regression, y(x) = mx + b
	// from http://en.wikipedia.org/wiki/Simple_linear_regression
	double num = 0, den = 0;
	double xavg = 0,yavg = 0;
	double m,b;
	int i;
	for (i=0;i<iLen;i++){
		xavg += x[i];
		yavg += y[i];
	}
	xavg /= iLen;
	yavg /= iLen;
	for (i=0;i<iLen;i++){
		num += (x[i]-xavg)*(y[i]-yavg);
		den += (x[i]-xavg)*(x[i]-xavg);
	}
	daCoeffs[0] = num/den;									// Slope
	daCoeffs[1] = yavg-daCoeffs[0]*xavg;		// Intercept
}
