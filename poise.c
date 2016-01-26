/********************** POISE.C **********************/
/*
 * Russell Deitrick, September 10, 2015
 *
 * Subroutines that control the energy balance model for climate 
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"

void InitializeControlPoise(CONTROL *control) {
  /* Not sure if I need anything here yet */
}

void InitializeModulePoise(CONTROL *control,MODULE *module) {
  /* Anything here? */
}

void BodyCopyPoise(BODY *dest,BODY *src,int iTideModel,int iBody) {
  int iLat;
  
  if (src[iBody].bIceSheets) {
    dest[iBody].iNumLats = src[iBody].iNumLats;
    dest[iBody].dIceMassTot = src[iBody].dIceMassTot;
    dest[iBody].dIceDepRate = src[iBody].dIceDepRate;
    dest[iBody].dAlbedoGlobal = src[iBody].dAlbedoGlobal;
    dest[iBody].dIceAlbedo = src[iBody].dIceAlbedo;
    dest[iBody].bClimateModel = src[iBody].bClimateModel;
    dest[iBody].bIceSheets = src[iBody].bIceSheets;
    for (iLat=0;iLat<src[iBody].iNumLats;iLat++) {
      dest[iBody].daIceMass[iLat] = src[iBody].daIceMass[iLat];
      dest[iBody].daTemp[iLat] = src[iBody].daTemp[iLat];
      dest[iBody].daLats[iLat] = src[iBody].daLats[iLat];
      dest[iBody].daIceBalanceAnnual[iLat] = src[iBody].daIceBalanceAnnual[iLat];
      dest[iBody].daXBoundary[iLat] = src[iBody].daXBoundary[iLat];
    }
    dest[iBody].daXBoundary[iLat] = src[iBody].daXBoundary[iLat];
  }
}

void InitializeBodyPoise(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
}

void InitializeUpdateTmpBodyPoise(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
//   control->Evolve.tmpBody[iBody].daIceMass = malloc(body[iBody].iNumLats*sizeof(double));
  control->Evolve.tmpBody[iBody].daTemp = malloc(body[iBody].iNumLats*sizeof(double));
  control->Evolve.tmpBody[iBody].daIceBalanceAnnual = malloc(body[iBody].iNumLats*sizeof(double));
  
  control->Evolve.tmpBody[iBody].daLats = malloc(body[iBody].iNumLats*sizeof(double));

  control->Evolve.tmpBody[iBody].daIceHeight = malloc(body[iBody].iNumLats*sizeof(double));
  control->Evolve.tmpBody[iBody].daDIceHeightDy = malloc(body[iBody].iNumLats*sizeof(double));
  control->Evolve.tmpBody[iBody].daIceFlow = malloc(body[iBody].iNumLats*sizeof(double));
  control->Evolve.tmpBody[iBody].daIceFlowMid = malloc((body[iBody].iNumLats+1)*sizeof(double));
  control->Evolve.tmpBody[iBody].daXBoundary = malloc((body[iBody].iNumLats+1)*sizeof(double));
}

/**************** POISE options ********************/

void ReadLatCellNum(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,iTmp;
  AddOptionInt(files->Infile[iFile].cIn,options->cName,&iTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].iNumLats = iTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].iNumLats,files->iNumInputs);
}

void ReadPlanckA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dPlanckA = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dPlanckA = options->dDefault;
}

void ReadPlanckB(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dPlanckB = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dPlanckB = options->dDefault;
}

void ReadSurfAlbedo(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dSurfAlbedo = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dSurfAlbedo = options->dDefault;
}

void ReadIceAlbedo(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dIceAlbedo = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dIceAlbedo = options->dDefault;
}

void ReadTGlobalInit(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dTGlobalInit = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dTGlobalInit = options->dDefault;
}

void ReadPCO2(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dpCO2 = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dpCO2 = options->dDefault;
}

void ReadDiffusion(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dDiffCoeff = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dDiffCoeff = options->dDefault;
}

void ReadFixIceLat(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dFixIceLat = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFixIceLat = options->dDefault;
}

void ReadHeatCapAnn(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dHeatCapAnn = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dHeatCapAnn = options->dDefault;
}


void ReadIceDepRate(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dIceDepRate = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dIceDepRate = options->dDefault;
}

void ReadHadley(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].bHadley = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bHadley,files->iNumInputs);
}

void ReadCalcAB(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].bCalcAB = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bCalcAB,files->iNumInputs);
}

void ReadColdStart(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].bColdStart = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bColdStart,files->iNumInputs);
}

void ReadAlbedoZA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].bAlbedoZA = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bAlbedoZA,files->iNumInputs);
}

void ReadJormungand(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].bJormungand = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bJormungand,files->iNumInputs);
}

void ReadSeaIceModel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].bSeaIceModel = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bSeaIceModel,files->iNumInputs);
}

void ReadMEPDiff(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].bMEPDiff = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bMEPDiff,files->iNumInputs);
}

void ReadIceSheets(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1, bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].bIceSheets = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bIceSheets,files->iNumInputs);
}

void ReadClimateModel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  char cTmp[OPTLEN];

  /* Climate Model, use #defined variables */

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* This parameter can not appear in the primary input,
       as it is module specific (it's easier to code this
       way. It should also only appear in one body file
       so as different tidal models cannot be called. */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    //CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"ann",3)) {
      body[iFile-1].bClimateModel = ANN;
    } else if (!memcmp(sLower(cTmp),"sea",3)) {
      body[iFile-1].bClimateModel = SEA;
    } else {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are ann or sea.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);  
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bClimateModel,files->iNumInputs);
}

void ReadInitIceLat(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dInitIceLat = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dInitIceLat = options->dDefault;
}

void ReadInitIceHeight(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dInitIceHeight = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dInitIceHeight = options->dDefault;
}

void ReadHeatCapLand(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dHeatCapLand = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dHeatCapLand = options->dDefault;
}

void ReadSeaIceConduct(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dSeaIceConduct = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dSeaIceConduct = options->dDefault;
}

void ReadHeatCapWater(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dHeatCapWater = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dHeatCapWater = options->dDefault;
}

void ReadAlbedoLand(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dAlbedoLand = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dAlbedoLand = options->dDefault;
}


void ReadAlbedoWater(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dAlbedoWater = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dAlbedoWater = options->dDefault;
}

void ReadMixingDepth(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dMixingDepth = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dMixingDepth = options->dDefault;
}

void ReadFrzTSeaIce(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dFrzTSeaIce = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFrzTSeaIce = options->dDefault;
}

void ReadNuLandWater(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dNuLandWater = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dNuLandWater = options->dDefault;
}

void ReadNStepInYear(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,iTmp;
  AddOptionInt(files->Infile[iFile].cIn,options->cName,&iTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].iNStepInYear = iTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].iNStepInYear,files->iNumInputs);
}

void ReadNumYears(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,iTmp;
  AddOptionInt(files->Infile[iFile].cIn,options->cName,&iTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].iNumYears = iTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].iNumYears,files->iNumInputs);
}

void InitializeOptionsPoise(OPTIONS *options,fnReadOption fnRead[]) {
  sprintf(options[OPT_LATCELLNUM].cName,"iLatCellNum");
  sprintf(options[OPT_LATCELLNUM].cDescr,"Number of latitude cells used in climate model");
  sprintf(options[OPT_LATCELLNUM].cDefault,"50");
  options[OPT_LATCELLNUM].dDefault = 50;
  options[OPT_LATCELLNUM].iType = 1;  
  options[OPT_LATCELLNUM].iMultiFile = 1;   
  fnRead[OPT_LATCELLNUM] = &ReadLatCellNum;
  
  sprintf(options[OPT_PLANCKA].cName,"dPlanckA");
  sprintf(options[OPT_PLANCKA].cDescr,"Constant 'A' used in OLR calculation");
  sprintf(options[OPT_PLANCKA].cDefault,"203.3");
  options[OPT_PLANCKA].dDefault = 203.3;
  options[OPT_PLANCKA].iType = 2;  
  options[OPT_PLANCKA].iMultiFile = 1;   
  fnRead[OPT_PLANCKA] = &ReadPlanckA;
  
  sprintf(options[OPT_PLANCKB].cName,"dPlanckB");
  sprintf(options[OPT_PLANCKB].cDescr,"Sensitivity 'B' used in OLR calculation");
  sprintf(options[OPT_PLANCKB].cDefault,"2.09");
  options[OPT_PLANCKB].dDefault = 2.09;
  options[OPT_PLANCKB].iType = 2;  
  options[OPT_PLANCKB].iMultiFile = 1;   
  fnRead[OPT_PLANCKB] = &ReadPlanckB;
  
  sprintf(options[OPT_ICEALBEDO].cName,"dIceAlbedo");
  sprintf(options[OPT_ICEALBEDO].cDescr,"Albedo of ice");
  sprintf(options[OPT_ICEALBEDO].cDefault,"0.6");
  options[OPT_ICEALBEDO].dDefault = 0.6;
  options[OPT_ICEALBEDO].iType = 2;  
  options[OPT_ICEALBEDO].iMultiFile = 1;   
  fnRead[OPT_ICEALBEDO] = &ReadIceAlbedo;
  
  sprintf(options[OPT_SURFALBEDO].cName,"dSurfAlbedo");
  sprintf(options[OPT_SURFALBEDO].cDescr,"Albedo of (ice-free) surface");
  sprintf(options[OPT_SURFALBEDO].cDefault,"0.3");
  options[OPT_SURFALBEDO].dDefault = 0.3;
  options[OPT_SURFALBEDO].iType = 2;  
  options[OPT_SURFALBEDO].iMultiFile = 1;   
  fnRead[OPT_SURFALBEDO] = &ReadSurfAlbedo;
  
  sprintf(options[OPT_TGLOBALEST].cName,"dTGlobalInit");
  sprintf(options[OPT_TGLOBALEST].cDescr,"Estimate of initial global temperature");
  sprintf(options[OPT_TGLOBALEST].cDefault,"14.85");
  options[OPT_TGLOBALEST].dDefault = 14.85;
  options[OPT_TGLOBALEST].iType = 2;  
  options[OPT_TGLOBALEST].iMultiFile = 1;   
  fnRead[OPT_TGLOBALEST] = &ReadTGlobalInit;
  
  sprintf(options[OPT_PCO2].cName,"dpCO2");
  sprintf(options[OPT_PCO2].cDescr,"Partial pressure of CO2 in atmosphere");
  sprintf(options[OPT_PCO2].cDefault,"3.3e-4");
  options[OPT_PCO2].dDefault = 3.3e-4;
  options[OPT_PCO2].iType = 2;  
  options[OPT_PCO2].iMultiFile = 1;   
  fnRead[OPT_PCO2] = &ReadPCO2;
  
  sprintf(options[OPT_CALCAB].cName,"bCalcAB");
  sprintf(options[OPT_CALCAB].cDescr,"Calculate A and B in OLR function, from (T & pCO2)");
  sprintf(options[OPT_CALCAB].cDefault,"0");
  options[OPT_CALCAB].dDefault = 0;
  options[OPT_CALCAB].iType = 0;  
  options[OPT_CALCAB].iMultiFile = 1;   
  fnRead[OPT_CALCAB] = &ReadCalcAB;
  
  sprintf(options[OPT_DIFFUSION].cName,"dDiffusion");
  sprintf(options[OPT_DIFFUSION].cDescr,"Heat diffusion coefficient");
  sprintf(options[OPT_DIFFUSION].cDefault,"0.44");
  options[OPT_DIFFUSION].dDefault = 0.44;
  options[OPT_DIFFUSION].iType = 2;  
  options[OPT_DIFFUSION].iMultiFile = 1;   
  fnRead[OPT_DIFFUSION] = &ReadDiffusion;
  
  sprintf(options[OPT_HADLEY].cName,"bHadley");
  sprintf(options[OPT_HADLEY].cDescr,"Enable Hadley circulation");
  sprintf(options[OPT_HADLEY].cDefault,"0");
  options[OPT_HADLEY].dDefault = 0;
  options[OPT_HADLEY].iType = 0;  
  options[OPT_HADLEY].iMultiFile = 1;   
  fnRead[OPT_HADLEY] = &ReadHadley;
  
  sprintf(options[OPT_COLDSTART].cName,"bColdStart");
  sprintf(options[OPT_COLDSTART].cDescr,"Start from snowball Earth conditions");
  sprintf(options[OPT_COLDSTART].cDefault,"0");
  options[OPT_COLDSTART].dDefault = 0;
  options[OPT_COLDSTART].iType = 0;  
  options[OPT_COLDSTART].iMultiFile = 1;   
  fnRead[OPT_COLDSTART] = &ReadColdStart;
  
  sprintf(options[OPT_FIXICELAT].cName,"dFixIceLat");
  sprintf(options[OPT_FIXICELAT].cDescr,"Fixes ice line latitude to a set value");
  sprintf(options[OPT_FIXICELAT].cDefault,"None");
  options[OPT_FIXICELAT].dDefault = 0;
  options[OPT_FIXICELAT].iType = 2;  
  options[OPT_FIXICELAT].iMultiFile = 1;   
  fnRead[OPT_FIXICELAT] = &ReadFixIceLat;
  
  sprintf(options[OPT_ALBEDOZA].cName,"bAlbedoZA");
  sprintf(options[OPT_ALBEDOZA].cDescr,"Use albedo based on zenith angle");
  sprintf(options[OPT_ALBEDOZA].cDefault,"0");
  options[OPT_ALBEDOZA].dDefault = 0;
  options[OPT_ALBEDOZA].iType = 0;  
  options[OPT_ALBEDOZA].iMultiFile = 1;   
  fnRead[OPT_ALBEDOZA] = &ReadAlbedoZA;
  
  sprintf(options[OPT_JORMUNGAND].cName,"bJormungand");
  sprintf(options[OPT_JORMUNGAND].cDescr,"With dFixIceLat, fixes ice latitude with cold equator");
  sprintf(options[OPT_JORMUNGAND].cDefault,"0");
  options[OPT_JORMUNGAND].dDefault = 0;
  options[OPT_JORMUNGAND].iType = 0;  
  options[OPT_JORMUNGAND].iMultiFile = 1;   
  fnRead[OPT_JORMUNGAND] = &ReadJormungand;
  
  sprintf(options[OPT_MEPDIFF].cName,"bMEPDiff");
  sprintf(options[OPT_MEPDIFF].cDescr,"Calculate diffusion from max entropy production (D=B/4)");
  sprintf(options[OPT_MEPDIFF].cDefault,"0");
  options[OPT_MEPDIFF].dDefault = 0;
  options[OPT_MEPDIFF].iType = 0;  
  options[OPT_MEPDIFF].iMultiFile = 1;   
  fnRead[OPT_MEPDIFF] = &ReadMEPDiff;
  
  sprintf(options[OPT_HEATCAPANN].cName,"dHeatCapAnn");
  sprintf(options[OPT_HEATCAPANN].cDescr,"Surface heat capacity in annual model");
  sprintf(options[OPT_HEATCAPANN].cDefault,"0.2");
  options[OPT_HEATCAPANN].dDefault = 0.2;
  options[OPT_HEATCAPANN].iType = 2;  
  options[OPT_HEATCAPANN].iMultiFile = 1;   
  fnRead[OPT_HEATCAPANN] = &ReadHeatCapAnn;
  
  sprintf(options[OPT_ICEDEPRATE].cName,"dIceDepRate");
  sprintf(options[OPT_ICEDEPRATE].cDescr,"Deposition rate of ice/snow to form ice sheets");
  sprintf(options[OPT_ICEDEPRATE].cDefault,"2.9e-5");
  options[OPT_ICEDEPRATE].dDefault = 2.9e-5;
  options[OPT_ICEDEPRATE].iType = 2;  
  options[OPT_ICEDEPRATE].iMultiFile = 1;   
  fnRead[OPT_ICEDEPRATE] = &ReadIceDepRate;
  
  sprintf(options[OPT_ICESHEETS].cName,"bIceSheets");
  sprintf(options[OPT_ICESHEETS].cDescr,"Include ice sheets");
  sprintf(options[OPT_ICESHEETS].cDefault,"0");
  options[OPT_ICESHEETS].dDefault = 0;
  options[OPT_ICESHEETS].iType = 0;  
  options[OPT_ICESHEETS].iMultiFile = 1;   
  fnRead[OPT_ICESHEETS] = &ReadIceSheets;

  sprintf(options[OPT_INITICELAT].cName,"dInitIceLat");
  sprintf(options[OPT_INITICELAT].cDescr,"Sets initial ice sheet latitude");
  sprintf(options[OPT_INITICELAT].cDefault,"90");
  options[OPT_INITICELAT].dDefault = 90.0;
  options[OPT_INITICELAT].iType = 2;  
  options[OPT_INITICELAT].iMultiFile = 1;   
  fnRead[OPT_INITICELAT] = &ReadInitIceLat;  
  
  sprintf(options[OPT_INITICEHEIGHT].cName,"dInitIceHeight");
  sprintf(options[OPT_INITICEHEIGHT].cDescr,"Sets initial ice sheet height");
  sprintf(options[OPT_INITICEHEIGHT].cDefault,"50");
  options[OPT_INITICEHEIGHT].dDefault = 50.0;
  options[OPT_INITICEHEIGHT].iType = 2;  
  options[OPT_INITICEHEIGHT].iMultiFile = 1;   
  fnRead[OPT_INITICEHEIGHT] = &ReadInitIceHeight;
  
  sprintf(options[OPT_CLIMATEMODEL].cName,"bClimateModel");
  sprintf(options[OPT_CLIMATEMODEL].cDescr,"Use annual or seasonal model");
  sprintf(options[OPT_CLIMATEMODEL].cDefault,"ann");
  options[OPT_CLIMATEMODEL].dDefault = ANN;
  options[OPT_CLIMATEMODEL].iType = 1;  
  options[OPT_CLIMATEMODEL].iMultiFile = 1;   
  fnRead[OPT_CLIMATEMODEL] = &ReadClimateModel;
  
  sprintf(options[OPT_HEATCAPLAND].cName,"dHeatCapLand");
  sprintf(options[OPT_HEATCAPLAND].cDescr,"Land heat capacity in seasonal model");
  sprintf(options[OPT_HEATCAPLAND].cDefault,"0.45");
  options[OPT_HEATCAPLAND].dDefault = 0.45;
  options[OPT_HEATCAPLAND].iType = 2;  
  options[OPT_HEATCAPLAND].iMultiFile = 1;   
  fnRead[OPT_HEATCAPLAND] = &ReadHeatCapLand;
  
  sprintf(options[OPT_HEATCAPWATER].cName,"dHeatCapWater");
  sprintf(options[OPT_HEATCAPWATER].cDescr,"Water heat capacity in seasonal model");
  sprintf(options[OPT_HEATCAPWATER].cDefault,"9.8");
  options[OPT_HEATCAPWATER].dDefault = 9.8;
  options[OPT_HEATCAPWATER].iType = 2;  
  options[OPT_HEATCAPWATER].iMultiFile = 1;   
  fnRead[OPT_HEATCAPWATER] = &ReadHeatCapWater;
  
  sprintf(options[OPT_MIXINGDEPTH].cName,"dMixingDepth");
  sprintf(options[OPT_MIXINGDEPTH].cDescr,"Mixing depth of ocean in seasonal model");
  sprintf(options[OPT_MIXINGDEPTH].cDefault,"50");
  options[OPT_MIXINGDEPTH].dDefault = 50.;
  options[OPT_MIXINGDEPTH].iType = 2;  
  options[OPT_MIXINGDEPTH].iMultiFile = 1;   
  fnRead[OPT_MIXINGDEPTH] = &ReadMixingDepth;
  
  sprintf(options[OPT_FRZTSEAICE].cName,"dFrzTSeaIce");
  sprintf(options[OPT_FRZTSEAICE].cDescr,"Temp of sea ice formation in seasonal model");
  sprintf(options[OPT_FRZTSEAICE].cDefault,"-2");
  options[OPT_FRZTSEAICE].dDefault = -2.;
  options[OPT_FRZTSEAICE].iType = 2;  
  options[OPT_FRZTSEAICE].iMultiFile = 1;   
  fnRead[OPT_FRZTSEAICE] = &ReadFrzTSeaIce;
  
  sprintf(options[OPT_NULANDWATER].cName,"dNuLandWater");
  sprintf(options[OPT_NULANDWATER].cDescr,"Coefficient of land-ocean heat flux");
  sprintf(options[OPT_NULANDWATER].cDefault,"3");
  options[OPT_NULANDWATER].dDefault = 3.;
  options[OPT_NULANDWATER].iType = 2;  
  options[OPT_NULANDWATER].iMultiFile = 1;   
  fnRead[OPT_NULANDWATER] = &ReadNuLandWater;
  
  sprintf(options[OPT_NSTEPINYEAR].cName,"iNStepInYear");
  sprintf(options[OPT_NSTEPINYEAR].cDescr,"Number of time-steps/year in seasonal model");
  sprintf(options[OPT_NSTEPINYEAR].cDefault,"60");
  options[OPT_NSTEPINYEAR].dDefault = 60;
  options[OPT_NSTEPINYEAR].iType = 1;  
  options[OPT_NSTEPINYEAR].iMultiFile = 1;   
  fnRead[OPT_NSTEPINYEAR] = &ReadNStepInYear;
  
  sprintf(options[OPT_NUMYEARS].cName,"iNumYears");
  sprintf(options[OPT_NUMYEARS].cDescr,"Number of years to run seasonal model");
  sprintf(options[OPT_NUMYEARS].cDefault,"10");
  options[OPT_NUMYEARS].dDefault = 10;
  options[OPT_NUMYEARS].iType = 1;  
  options[OPT_NUMYEARS].iMultiFile = 1;   
  fnRead[OPT_NUMYEARS] = &ReadNumYears;
  
  sprintf(options[OPT_SEAICEMODEL].cName,"bSeaIceModel");
  sprintf(options[OPT_SEAICEMODEL].cDescr,"model sea ice dynamics and heat flow?");
  sprintf(options[OPT_SEAICEMODEL].cDefault,"1");
  options[OPT_SEAICEMODEL].dDefault = 1;
  options[OPT_SEAICEMODEL].iType = 0;  
  options[OPT_SEAICEMODEL].iMultiFile = 1;   
  fnRead[OPT_SEAICEMODEL] = &ReadSeaIceModel;
  
  sprintf(options[OPT_ICECONDUCT].cName,"dSeaIceConduct");
  sprintf(options[OPT_ICECONDUCT].cDescr,"Heat conductivity of sea ice");
  sprintf(options[OPT_ICECONDUCT].cDefault,"2");
  options[OPT_ICECONDUCT].dDefault = 2.;
  options[OPT_ICECONDUCT].iType = 2;  
  options[OPT_ICECONDUCT].iMultiFile = 1;   
  fnRead[OPT_ICECONDUCT] = &ReadSeaIceConduct;
  
  sprintf(options[OPT_ALBEDOLAND].cName,"dAlbedoLand");
  sprintf(options[OPT_ALBEDOLAND].cDescr,"albedo of dry land");
  sprintf(options[OPT_ALBEDOLAND].cDefault,"0.363");
  options[OPT_ALBEDOLAND].dDefault = 0.363;
  options[OPT_ALBEDOLAND].iType = 2;  
  options[OPT_ALBEDOLAND].iMultiFile = 1;   
  fnRead[OPT_ALBEDOLAND] = &ReadAlbedoLand;
  
  sprintf(options[OPT_ALBEDOWATER].cName,"dAlbedoWater");
  sprintf(options[OPT_ALBEDOWATER].cDescr,"albedo of open water");
  sprintf(options[OPT_ALBEDOWATER].cDefault,"0.263");
  options[OPT_ALBEDOWATER].dDefault = 0.263;
  options[OPT_ALBEDOWATER].iType = 2;  
  options[OPT_ALBEDOWATER].iMultiFile = 1;   
  fnRead[OPT_ALBEDOWATER] = &ReadAlbedoWater;
}

void ReadOptionsPoise(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTPOISE;iOpt<OPTENDPOISE;iOpt++) { 
      if (options[iOpt].iType != -1) {
        fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
      }
  }
//   ReadGridOutput(files,options,output,iBody+1,control->Io.iVerbose);
}
    

/******************* Verify POISE ******************/
void VerifyAlbedo(BODY *body, OPTIONS *options, char cFile[], int iBody, int iVerbose) {
  if (body[iBody].bClimateModel == ANN) {
    /* If all of bColdstart, dFixIceLat, and bAlbedoZA are set, exit */
    if (options[OPT_COLDSTART].iLine[iBody+1] > -1 && options[OPT_FIXICELAT].iLine[iBody+1] > -1 && options[OPT_ALBEDOZA].iLine[iBody+1] > -1) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: Only one of %s, %s, and %s can be set in File: %s\n", options[OPT_COLDSTART].cName, options[OPT_FIXICELAT].cName, options[OPT_ALBEDOZA].cName, cFile);
      exit(EXIT_INPUT);
    }
    
    /* Any two of bColdstart, dFixIceLat, and bAlbedoZA are set, exit */ 
    if (options[OPT_COLDSTART].iLine[iBody+1] > -1 && options[OPT_FIXICELAT].iLine[iBody+1] > -1 && options[OPT_ALBEDOZA].iLine[iBody+1] == -1) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: Only one of %s and %s can be set in File: %s\n", options[OPT_COLDSTART].cName, options[OPT_FIXICELAT].cName, cFile);
      exit(EXIT_INPUT);
    }

    if (options[OPT_COLDSTART].iLine[iBody+1] > -1 && options[OPT_FIXICELAT].iLine[iBody+1] == -1 && options[OPT_ALBEDOZA].iLine[iBody+1] > -1) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: Only one of %s and %s can be set in File: %s\n", options[OPT_COLDSTART].cName, options[OPT_ALBEDOZA].cName, cFile);
      exit(EXIT_INPUT);
    } 
  
    if (options[OPT_COLDSTART].iLine[iBody+1] == -1 && options[OPT_FIXICELAT].iLine[iBody+1] > -1 && options[OPT_ALBEDOZA].iLine[iBody+1] > -1) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: Only one of %s and %s can be set in File: %s\n", options[OPT_FIXICELAT].cName, options[OPT_ALBEDOZA].cName, cFile);
      exit(EXIT_INPUT);
    } 
  
    /* If bJormungand is set, is dFixIceLat also set? If not, exit */
    if (options[OPT_JORMUNGAND].iLine[iBody+1] > -1 && options[OPT_FIXICELAT].iLine[iBody+1] == -1) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: If %s is set, %s must also be set in File: %s\n", options[OPT_JORMUNGAND].cName, options[OPT_FIXICELAT].cName, cFile);
      exit(EXIT_INPUT);
    }
    
    if (options[OPT_ALBEDOLAND].iLine[iBody+1] > -1 || options[OPT_ALBEDOWATER].iLine[iBody+1] == -1) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: Cannot set %s or %s for annual model in file %s\nPlease use option %s\n", options[OPT_ALBEDOLAND].cName, options[OPT_ALBEDOWATER].cName, cFile, options[OPT_SURFALBEDO].cName);
      exit(EXIT_INPUT);
    }
    
  } else if (body[iBody].bClimateModel == SEA) {
    if (options[OPT_SURFALBEDO].iLine[iBody+1] > -1) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: Cannot set %s for seasonal model in file %s\nPlease use options %s and %s\n",  options[OPT_SURFALBEDO].cName, cFile, options[OPT_ALBEDOLAND].cName, options[OPT_ALBEDOWATER].cName);
      exit(EXIT_INPUT);
    }
  }
}

void VerifyIceSheets(BODY *body, OPTIONS *options, char cFile[], int iBody, int iVerbose) {
  if (body[iBody].bClimateModel == ANN) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Cannot set %s in annual model in File:%s\n", options[OPT_ICESHEETS].cName, cFile);
    exit(EXIT_INPUT);
  }
}

void VerifyOLR(BODY *body, OPTIONS *options, char cFile[], int iBody, int iVerbose) {
  if (body[iBody].bCalcAB) {
    if (options[OPT_PLANCKA].iLine[iBody+1] > -1 || options[OPT_PLANCKB].iLine[iBody+1] > -1) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: Cannot set %s or %s when setting bCalcAB = 1 in File:%s\n", options[OPT_PLANCKA].cName, options[OPT_PLANCKB].cName, cFile);
      exit(EXIT_INPUT);
    }
      
  } else {
    if (options[OPT_PCO2].iLine[iBody+1] > -1 || options[OPT_TGLOBALEST].iLine[iBody+1] > -1) {
      if (iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: Cannot set %s or %s unless setting bCalcAB = 1 in File:%s\n", options[OPT_PCO2].cName, options[OPT_TGLOBALEST].cName, cFile);
      exit(EXIT_INPUT);
    }
  }
}

void VerifyNStepSeasonal(BODY *body, int iBody) {
  if (body[iBody].iNStepInYear > body[iBody].iNDays) {
    body[iBody].iNStepInYear = body[iBody].iNDays;
  }
}

void InitializeLatGrid(BODY *body, int iBody) {
  double delta_x, SinLat;
  int i;
  delta_x = 2.0/body[iBody].iNumLats;
  
  body[iBody].daLats = malloc(body[iBody].iNumLats*sizeof(double));
  
  for (i=0;i<body[iBody].iNumLats;i++) {
    SinLat = (-1.0 + delta_x/2.) + i*delta_x; 
    body[iBody].daLats[i] = asin(SinLat);  
  }
}
 
void InitializeLandWater(BODY *body, int iBody) {
  int iLat;
  
  body[iBody].daLandFrac = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daWaterFrac = malloc(body[iBody].iNumLats*sizeof(double));
  
  for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
    body[iBody].daLandFrac[iLat] = 0.34;
    body[iBody].daWaterFrac[iLat] = 1.0-body[iBody].daLandFrac[iLat];
  }  
}

void InitializeClimateParams(BODY *body, int iBody) {
  int i, j;
  double Toffset, xboundary, TGlobalTmp;
  
  body[iBody].dIceMassTot = 0.0;
  body[iBody].daDiffusion = malloc((body[iBody].iNumLats+1)*sizeof(double));
  body[iBody].daInsol = malloc(body[iBody].iNumLats*sizeof(double*));
  body[iBody].daAnnualInsol = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daLambda = malloc((body[iBody].iNumLats+1)*sizeof(double));
  body[iBody].daAlbedo = malloc(body[iBody].iNumLats*sizeof(double)); 
  body[iBody].iNDays = (int)floor(body[iBody].dRotRate/body[iBody].dMeanMotion); //number of days in year
  //body[iBody].iNDays = 360;
  body[iBody].dMDiff = malloc(body[iBody].iNumLats*sizeof(double*)); // matrix of heat diffusion terms only
  body[iBody].daTemp = malloc(body[iBody].iNumLats*sizeof(double)); 
  body[iBody].daFlux = malloc(body[iBody].iNumLats*sizeof(double));  
  body[iBody].daFluxIn = malloc(body[iBody].iNumLats*sizeof(double)); 
  body[iBody].daFluxOut = malloc(body[iBody].iNumLats*sizeof(double)); 
  body[iBody].daDivFlux = malloc(body[iBody].iNumLats*sizeof(double));   
  body[iBody].daIceHeight = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daIceFlow = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daIceFlowMid = malloc((body[iBody].iNumLats+1)*sizeof(double));
  body[iBody].daDIceHeightDy = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daDeclination = malloc(body[iBody].iNDays*sizeof(double));
  body[iBody].daTGrad = malloc(body[iBody].iNumLats*sizeof(double)); 
  body[iBody].daDMidPt = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daPlanckA = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daPlanckB = malloc(body[iBody].iNumLats*sizeof(double));

  if (body[iBody].bColdStart) {
    Toffset = -40.0;
  } else {
    Toffset = 0.0;
  }
  
  body[iBody].dTGlobal = 0.0;
  body[iBody].dAlbedoGlobal = 0.0;
  
  if (body[iBody].bClimateModel == ANN) {      
 
    body[iBody].dMClim = malloc(body[iBody].iNumLats*sizeof(double*));
    
    body[iBody].dMEuler = malloc(body[iBody].iNumLats*sizeof(double*));
    body[iBody].dMEulerCopy = malloc(body[iBody].iNumLats*sizeof(double*));
    body[iBody].dInvM = malloc(body[iBody].iNumLats*sizeof(double*));
    body[iBody].daSourceF = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daTempTerms = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daTmpTemp = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daTmpTempTerms = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].rowswap =  malloc(body[iBody].iNumLats*sizeof(int));
    body[iBody].scale = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].dUnitV = malloc(body[iBody].iNumLats*sizeof(double));
  
    
    body[iBody].daDiffusion[0] = body[iBody].dDiffCoeff;
    for (i=0;i<=body[iBody].iNumLats;i++) {
      if (i!=body[iBody].iNumLats) {
        body[iBody].daTemp[i] = 20.*(1.0-1.5*pow(sin(body[iBody].daLats[i]),2))+Toffset;
        body[iBody].dTGlobal += body[iBody].daTemp[i]/body[iBody].iNumLats;
        body[iBody].daInsol[i] = malloc(body[iBody].iNDays*sizeof(double));
        body[iBody].dMClim[i] = malloc(body[iBody].iNumLats*sizeof(double));
        body[iBody].dMDiff[i] = malloc(body[iBody].iNumLats*sizeof(double));
        body[iBody].dMEuler[i] = malloc(body[iBody].iNumLats*sizeof(double));
        body[iBody].dMEulerCopy[i] = malloc(body[iBody].iNumLats*sizeof(double));
        body[iBody].dInvM[i] = malloc(body[iBody].iNumLats*sizeof(double));
      
        if (body[iBody].bIceSheets) {
          if (fabs(body[iBody].daLats[i])>=(body[iBody].dInitIceLat*DEGRAD)) {
            body[iBody].daIceMass[i] = body[iBody].dInitIceHeight*RHOICE;
            body[iBody].dIceMassTot += body[iBody].daIceMass[i]*(2*PI*pow(body[iBody].dRadius,2)*(sin(body[iBody].daLats[1])-sin(body[iBody].daLats[0]))); //XXX only works if all lat cells are equal area!!
  //           body[iBody].daIceHeight[i] = body[iBody].dInitIceHeight;
          } else {
            body[iBody].daIceMass[i] = 0.0;
  //           body[iBody].daIceHeight[i] = 0.0;
          }
        }
      }  
    
      if (body[iBody].bMEPDiff) {   
        body[iBody].daDiffusion[i] = body[iBody].daPlanckB[i]/4.0;   
      } else {
        body[iBody].daDiffusion[i] = body[iBody].dDiffCoeff;   
      }
    
      if (body[iBody].bHadley) {
        // XXX not self-consistent with rotation rate!
        xboundary = -1.0 + i*2.0/body[iBody].iNumLats;
        body[iBody].daDiffusion[i] += body[iBody].dDiffCoeff*9.*exp(-pow((xboundary/sin(25.*DEGRAD)),6));
      }
    }
  
    Albedo(body,iBody);
    for (i=0;i<body[iBody].iNumLats;i++) {
      body[iBody].dAlbedoGlobal += body[iBody].daAlbedo[i];
    }
    body[iBody].dAlbedoGlobal /= body[iBody].iNumLats;    
    
  } else if (body[iBody].bClimateModel == SEA) {
    /* oh yeah, seasonal model, oh yeah! 
    'tis the season to model ice sheets, fa la la la la, la la la la */
    VerifyNStepSeasonal(body,iBody);
    body[iBody].dSeasDeltax = 2.0/body[iBody].iNumLats;
    body[iBody].dSeasDeltat = 1./body[iBody].iNStepInYear;
    body[iBody].daXBoundary = malloc((body[iBody].iNumLats+1)*sizeof(double));
    
    body[iBody].daTempLand = malloc(body[iBody].iNumLats*sizeof(double)); 
    body[iBody].daTempWater = malloc(body[iBody].iNumLats*sizeof(double)); 
    body[iBody].daTempDaily = malloc(body[iBody].iNumLats*sizeof(double*));
    body[iBody].daFluxOutLand = malloc(body[iBody].iNumLats*sizeof(double)); 
    body[iBody].daFluxOutWater = malloc(body[iBody].iNumLats*sizeof(double)); 
    body[iBody].daFluxInLand = malloc(body[iBody].iNumLats*sizeof(double)); 
    body[iBody].daFluxInWater = malloc(body[iBody].iNumLats*sizeof(double)); 
    body[iBody].daSeaIceHeight = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].dMEuler = malloc(2*body[iBody].iNumLats*sizeof(double*));
    body[iBody].dMEulerCopy = malloc(2*body[iBody].iNumLats*sizeof(double*));
    body[iBody].dMInit = malloc(2*body[iBody].iNumLats*sizeof(double*));
    body[iBody].dInvM = malloc(2*body[iBody].iNumLats*sizeof(double*));
    body[iBody].daSourceL = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daSourceW = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daSourceLW = malloc(2*body[iBody].iNumLats*sizeof(double));
    body[iBody].dMLand = malloc(body[iBody].iNumLats*sizeof(double*));
    body[iBody].dMWater = malloc(body[iBody].iNumLats*sizeof(double*));
    body[iBody].rowswap =  malloc(2*body[iBody].iNumLats*sizeof(int));
    body[iBody].scale = malloc(2*body[iBody].iNumLats*sizeof(double));
    body[iBody].dUnitV = malloc(2*body[iBody].iNumLats*sizeof(double));
    body[iBody].daAlbedoLand = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daAlbedoWater = malloc(body[iBody].iNumLats*sizeof(double));
    //body[iBody].daAlbedoGlobalTmp = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daTmpTemp = malloc(2*body[iBody].iNumLats*sizeof(double));
    //body[iBody].daTGlobalTmp = malloc(body[iBody].iNDays*sizeof(double));
    body[iBody].daSeaIceHeight = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daSeaIceK = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daFluxSeaIce = malloc(body[iBody].iNumLats*sizeof(double));
    
    body[iBody].daTempAnnual = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daAlbedoAnnual = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daFluxAnnual = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daFluxInAnnual = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daDivFluxAnnual = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daFluxOutAnnual = malloc(body[iBody].iNumLats*sizeof(double));
    
    body[iBody].daIceBalance = malloc(body[iBody].iNumLats*sizeof(double*));
    body[iBody].daIceBalanceAnnual = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daIceMassTmp = malloc(body[iBody].iNumLats*sizeof(double));
    
    InitializeLandWater(body,iBody);
    body[iBody].dLatFHeatCp = 83.5;  //CC sez this is about right
    body[iBody].dLatentHeatIce = body[iBody].dHeatCapWater*body[iBody].dLatFHeatCp/body[iBody].dMixingDepth;
    
    for (i=0;i<=body[iBody].iNumLats;i++) {
      if (i!=body[iBody].iNumLats) {
        body[iBody].daTempLand[i] = (body[iBody].dTGlobalInit-7.35) + 20.*(1.0-2*pow(sin(body[iBody].daLats[i]),2)) + Toffset;
        body[iBody].daTempWater[i] = (body[iBody].dTGlobalInit-7.35) + 20.*(1.0-2*pow(sin(body[iBody].daLats[i]),2)) + Toffset;
        body[iBody].daTemp[i] = body[iBody].daLandFrac[i]*body[iBody].daTempLand[i] + \
                                  body[iBody].daWaterFrac[i]*body[iBody].daTempWater[i];
        if ((body[iBody].daTempWater[i] < body[iBody].dFrzTSeaIce) && (body[iBody].bSeaIceModel)) {
          body[iBody].daSeaIceHeight[i] = 2.0;
        } else {
          body[iBody].daSeaIceHeight[i] = 0.0;
        }
        
        body[iBody].dTGlobal += (body[iBody].daLandFrac[i]*body[iBody].daTempLand[i]+ \
                 body[iBody].daWaterFrac[i]*body[iBody].daTempWater[i])/body[iBody].iNumLats;
        body[iBody].daInsol[i] = malloc(body[iBody].iNDays*sizeof(double));
        body[iBody].daTempDaily[i] = malloc(body[iBody].iNumYears*body[iBody].iNStepInYear*sizeof(double));
        body[iBody].daIceBalance[i] = malloc(body[iBody].iNStepInYear*sizeof(double));
        body[iBody].dMLand[i] = malloc(body[iBody].iNumLats*sizeof(double));
        body[iBody].dMWater[i] = malloc(body[iBody].iNumLats*sizeof(double));
        body[iBody].dMDiff[i] = malloc(body[iBody].iNumLats*sizeof(double));

        /* Seasonal matrix is 2n x 2n to couple land and ocean */
        body[iBody].dMEuler[2*i] = malloc(2*body[iBody].iNumLats*sizeof(double));
        body[iBody].dMInit[2*i] = malloc(2*body[iBody].iNumLats*sizeof(double));
        body[iBody].dMEulerCopy[2*i] = malloc(2*body[iBody].iNumLats*sizeof(double));
        body[iBody].dInvM[2*i] = malloc(2*body[iBody].iNumLats*sizeof(double));
        body[iBody].dMEuler[2*i+1] = malloc(2*body[iBody].iNumLats*sizeof(double));
        body[iBody].dMInit[2*i+1] = malloc(2*body[iBody].iNumLats*sizeof(double));
        body[iBody].dMEulerCopy[2*i+1] = malloc(2*body[iBody].iNumLats*sizeof(double));
        body[iBody].dInvM[2*i+1] = malloc(2*body[iBody].iNumLats*sizeof(double));
        
        if (body[iBody].bIceSheets) {
          if (fabs(body[iBody].daLats[i])>=(body[iBody].dInitIceLat*DEGRAD)) {
            body[iBody].daIceMass[i] = body[iBody].dInitIceHeight*RHOICE;
            body[iBody].dIceMassTot += body[iBody].daIceMass[i]*(2*PI*pow(body[iBody].dRadius,2)*(sin(body[iBody].daLats[1])-sin(body[iBody].daLats[0]))); //XXX only works if all lat cells are equal area!!
  //           body[iBody].daIceHeight[i] = body[iBody].dInitIceHeight;
          } else {
            body[iBody].daIceMass[i] = 0.0;
  //           body[iBody].daIceHeight[i] = 0.0;
          }
        }
        if (body[iBody].bCalcAB) {
          /* Calculate A and B from Haqq-Misra et al 2016 result */
          body[iBody].daPlanckB[i] = dOLRdThm16(body,iBody,i);
          body[iBody].daPlanckA[i] = OLRhm16(body,iBody,i) \
            - body[iBody].daPlanckB[i]*(body[iBody].daTemp[i]); 
        } else {
          body[iBody].daPlanckB[i] = body[iBody].dPlanckB;
          body[iBody].daPlanckA[i] = body[iBody].dPlanckA;
        }
      }
      
      if (body[iBody].bMEPDiff) {   
        body[iBody].daDiffusion[i] = body[iBody].daPlanckB[i]/4.0;   
      } else {
        body[iBody].daDiffusion[i] = body[iBody].dDiffCoeff;   
      }
    
      body[iBody].daXBoundary[i]= -1.0 + i*2.0/body[iBody].iNumLats;
      if (body[iBody].bHadley) {
        // XXX not self-consistent with rotation rate!
        body[iBody].daDiffusion[i] += body[iBody].dDiffCoeff*9.*exp(-pow((body[iBody].daXBoundary[i]/sin(25.*DEGRAD)),6));
      }
    }  

    AlbedoSeasonal(body,iBody,0);
    body[iBody].dAlbedoGlobal = body[iBody].dAlbedoGlobal*body[iBody].iNStepInYear;
    AnnualInsolation(body, iBody); 
    MatrixSeasonal(body, iBody);
    SourceFSeas(body,iBody,0);
    if (body[iBody].bSeaIceModel) {
      SeaIce(body,iBody);  
    }
    
    /* "burn in" to a quasi equilibrium */
    TGlobalTmp = 0;
    while (fabs(TGlobalTmp - body[iBody].dTGlobal) > 0.01) {
      TGlobalTmp = body[iBody].dTGlobal; 
      PoiseSeasonal(body,iBody); 
    }
  }
} 

void VerifyAstro(BODY *body, int iBody) {
  if (body[iBody].bEqtide == 0) {
    if (body[iBody].bDistOrb == 0) {
      CalcHK(body, iBody);
    }
    if (body[iBody].bDistRot == 0) {
      CalcXYZobl(body,iBody);
    }
  }
}

void VerifyDiffusion(BODY *body, OPTIONS *options, char cFile[], int iBody, int iVerbose) {
  if (body[iBody].bMEPDiff) {
    if (options[OPT_DIFFUSION].iLine[iBody+1] > -1) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: Cannot set %s when setting bMEPDiff = 1 in File:%s\n", options[OPT_DIFFUSION].cName, cFile);
      exit(EXIT_INPUT);
    }
    if (body[iBody].bHadley) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: Cannot set both bHadley = 1 and bMEPDiff = 1 in File:%s\n", cFile);
      exit(EXIT_INPUT);
    }
  }
}
    
void InitializeIceMassDepMelt(BODY *body,UPDATE *update,int iBody,int iLat) {
  update[iBody].iaType[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassDepMelt[iLat]] = 9;
  update[iBody].padDIceMassDtPoise[iLat][0] = &update[iBody].daDerivProc[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassDepMelt[iLat]];
  update[iBody].iNumBodies[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassDepMelt[iLat]]=2;
  update[iBody].iaBody[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassDepMelt[iLat]] = malloc(2*sizeof(int));
  update[iBody].iaBody[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassDepMelt[iLat]][0] = iBody;
  update[iBody].iaBody[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassDepMelt[iLat]][1] = iLat;
}

void InitializeIceMassFlow(BODY *body,UPDATE *update,int iBody,int iLat) {
  update[iBody].iaType[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassFlow[iLat]] = 4;
  update[iBody].padDIceMassDtPoise[iLat][1] = &update[iBody].daDerivProc[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassFlow[iLat]];
  update[iBody].iNumBodies[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassFlow[iLat]]=2;
  update[iBody].iaBody[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassFlow[iLat]] = malloc(2*sizeof(int));
  update[iBody].iaBody[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassFlow[iLat]][0] = iBody;
  update[iBody].iaBody[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassFlow[iLat]][1] = iLat;
}    
      
void VerifyPoise(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int i, j=0, iLat=0;
  
  VerifyAlbedo(body,options,files->Infile[iBody+1].cIn,iBody,control->Io.iVerbose);
  VerifyAstro(body,iBody);
  VerifyOLR(body,options,files->Infile[iBody+1].cIn,iBody,control->Io.iVerbose);
  VerifyDiffusion(body,options,files->Infile[iBody+1].cIn,iBody,control->Io.iVerbose);
  
  /* Initialize climate arrays */
  InitializeLatGrid(body, iBody);
  InitializeClimateParams(body, iBody);
  
  /* POISE annual has one primary variable! 
     The climate simulation is done almost entirely in ForceBehavior. */
  if (body[iBody].bIceSheets) {
    VerifyIceSheets(body,options,files->Infile[iBody+1].cIn,iBody,control->Io.iVerbose);
    for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
      InitializeIceMassDepMelt(body,update,iBody,iLat);
      fnUpdate[iBody][update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassDepMelt[iLat]] = &fdPoiseDIceMassDtDepMelt;
      InitializeIceMassFlow(body,update,iBody,iLat);
      fnUpdate[iBody][update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassFlow[iLat]] = &fdPoiseDIceMassDtFlow;    
    }
  }
  
  control->Evolve.fnPropsAux[iBody][iModule] = &PropertiesPoise;

  control->fnForceBehavior[iBody][iModule]=&ForceBehaviorPoise;
  control->Evolve.fnBodyCopy[iBody][iModule]=&BodyCopyPoise;
}


/***************** POISE Update *****************/
void InitializeUpdatePoise(BODY *body,UPDATE *update,int iBody) {
  if (body[iBody].bIceSheets) {
    update[iBody].iNumVars += body[iBody].iNumLats;
    update[iBody].iNumIceMass = 2;
    update[iBody].iaIceMass = malloc(body[iBody].iNumLats*sizeof(int));
    update[iBody].padDIceMassDtPoise = malloc(body[iBody].iNumLats*sizeof(double**));
    update[iBody].iaIceMassDepMelt = malloc(body[iBody].iNumLats*sizeof(int));
    update[iBody].iaIceMassFlow = malloc(body[iBody].iNumLats*sizeof(int));
  }
} 

void FinalizeUpdateIceMassPoise(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iLat) {
  update[iBody].iaModule[iVar][0] = POISE;
  update[iBody].iaIceMassDepMelt[iLat] = *iEqn;
  (*iEqn)++;
  update[iBody].iaModule[iVar][1] = POISE;
  update[iBody].iaIceMassFlow[iLat] = *iEqn;
  (*iEqn)++;
  
  update[iBody].padDIceMassDtPoise[iLat] = malloc(update[iBody].iNumEqns[iVar]*sizeof(double*));
}

/***************** POISE Halts *****************/

void CountHaltsPoise(HALT *halt,int *iNumHalts) { 
}

void VerifyHaltPoise(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
}


/************* POISE Outputs ******************/

void WriteTGlobal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TGlobal */
  *dTmp = body[iBody].dTGlobal;
  if (output->bDoNeg[iBody]) {
    /* Units already in Celsius (POISE uses Celsius) */
    strcpy(cUnit,output->cNeg);
  } else { 
    *dTmp = fdUnitsTemp(*dTmp, 1, 0);
    fsUnitsTime(0,cUnit);
  }
}

void WriteAlbedoGlobal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get AlbedoGlobal */
  *dTmp = body[iBody].dAlbedoGlobal;
}

void WriteTempLat(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  if (body[iBody].bClimateModel == ANN) {
    *dTmp = body[iBody].daTemp[body[iBody].iWriteLat];
  } else if (body[iBody].bClimateModel == SEA) {
    *dTmp = body[iBody].daTempAnnual[body[iBody].iWriteLat];
  }
  
  if (output->bDoNeg[iBody]) {
    /* Units already in Celsius (POISE uses Celsius) */
    strcpy(cUnit,output->cNeg);
  } else { 
    *dTmp = fdUnitsTemp(*dTmp, 1, 0);
    fsUnitsTime(0,cUnit);
  }
}

void WriteLatitude(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daLats[body[iBody].iWriteLat];
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}

void WriteAlbedoLat(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  if (body[iBody].bClimateModel == ANN) {
    *dTmp = body[iBody].daAlbedo[body[iBody].iWriteLat];
  } else if (body[iBody].bClimateModel == SEA) {
    *dTmp = body[iBody].daAlbedoAnnual[body[iBody].iWriteLat];
  }
}
  
void WriteFluxInGlobal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFluxInGlobal;

  if (output->bDoNeg[iBody]) {
    // Negative option is SI
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}  
  
void WriteFluxOutGlobal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFluxOutGlobal;

  if (output->bDoNeg[iBody]) {
    // Negative option is SI
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
} 

void WriteTotIceMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dIceMassTot;

  if (output->bDoNeg[iBody]) {
    // Negative option is SI
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsMass(units->iMass);
    fsUnitsMass(units->iMass,cUnit);
  }
}    
  
void WriteAnnualInsol(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daAnnualInsol[body[iBody].iWriteLat];

  if (output->bDoNeg[iBody]) {
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}
  
void WriteDailyInsol(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  char cOut[NAMELEN];
  FILE *fp;
  int iLat,iDay;
   
  sprintf(cOut,"%s.%s.DailyInsolInit",system->cName,body[iBody].cName);

  fp = fopen(cOut,"w");
  for (iDay=0;iDay<body[iBody].iNDays;iDay++) {
    for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
      fprintd(fp,body[iBody].daInsol[iLat][iDay],control->Io.iSciNot,control->Io.iDigits);
      fprintf(fp," ");
    }
    fprintf(fp,"\n");
  }
  fclose(fp);
}

void WriteSeasonalTemp(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  char cOut[NAMELEN];
  FILE *fp;
  int iLat,iDay;
   
  sprintf(cOut,"%s.%s.SeasonalTempInit",system->cName,body[iBody].cName);

  fp = fopen(cOut,"w");
  for (iDay=0;iDay<body[iBody].iNumYears*body[iBody].iNStepInYear;iDay++) {
    for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
      fprintd(fp,body[iBody].daTempDaily[iLat][iDay],control->Io.iSciNot,control->Io.iDigits);
      fprintf(fp," ");
    }
    fprintf(fp,"\n");
  }
  fclose(fp);
}
  
void WriteSeasonalIceBalance(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  char cOut[NAMELEN];
  FILE *fp;
  int iLat,iDay;
   
  sprintf(cOut,"%s.%s.SeasonalIceBalance",system->cName,body[iBody].cName);

  fp = fopen(cOut,"w");
  for (iDay=0;iDay<body[iBody].iNStepInYear;iDay++) {
    for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
      fprintd(fp,body[iBody].daIceBalance[iLat][iDay],control->Io.iSciNot,control->Io.iDigits);
      fprintf(fp," ");
    }
    fprintf(fp,"\n");
  }
  fclose(fp);
}  
  
void WriteFluxMerid(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daFlux[body[iBody].iWriteLat];
  
  if (output->bDoNeg[iBody]) {
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}
  
void WriteFluxIn(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  if (body[iBody].bClimateModel == ANN) {
    *dTmp = body[iBody].daFluxIn[body[iBody].iWriteLat];
  } else if (body[iBody].bClimateModel == SEA) {
    *dTmp = body[iBody].daFluxInAnnual[body[iBody].iWriteLat];
  }
  
  if (output->bDoNeg[iBody]) {
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteFluxOut(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  if (body[iBody].bClimateModel == ANN) {
    *dTmp = body[iBody].daFluxOut[body[iBody].iWriteLat];
  } else if (body[iBody].bClimateModel == SEA) {
    *dTmp = body[iBody].daFluxOutAnnual[body[iBody].iWriteLat];
  }
  
  if (output->bDoNeg[iBody]) {
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}
  
void WriteDivFlux(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daDivFlux[body[iBody].iWriteLat];
  
  if (output->bDoNeg[iBody]) {
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}       

void WriteIceMass(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daIceMass[body[iBody].iWriteLat];
  
  if (output->bDoNeg[iBody]) {
    strcpy(cUnit,output->cNeg);
  } else {
    //*dTmp /= fdUnitsMass(units->iMass)/pow(fdUnitsLength(units->iLength),2);
    //fsUnitsEnergyFlux(units,cUnit);
  }
}    

void WriteIceHeight(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daIceMass[body[iBody].iWriteLat]/RHOICE;
  
  if (output->bDoNeg[iBody]) {
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}  

void WriteDIceMassDt(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  //*dTmp = body[iBody].daIceBalanceAnnual[body[iBody].iWriteLat];
  *dTmp = *(update[iBody].padDIceMassDtPoise[body[iBody].iWriteLat][0]);
  
  if (output->bDoNeg[iBody]) {
    strcpy(cUnit,output->cNeg);
  } else {
    //*dTmp /= fdUnitsMass(units->iMass)/pow(fdUnitsLength(units->iLength),2);
    //fsUnitsEnergyFlux(units,cUnit);
  }
}
  
void WriteDIceMassDtFlow(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  //*dTmp = body[iBody].daIceBalanceAnnual[body[iBody].iWriteLat];
  *dTmp = *(update[iBody].padDIceMassDtPoise[body[iBody].iWriteLat][1]);
  
  if (output->bDoNeg[iBody]) {
    strcpy(cUnit,output->cNeg);
  } else {
    //*dTmp /= fdUnitsMass(units->iMass)/pow(fdUnitsLength(units->iLength),2);
    //fsUnitsEnergyFlux(units,cUnit);
  }
}  
  
void InitializeOutputPoise(OUTPUT *output,fnWriteOutput fnWrite[]) {
  sprintf(output[OUT_TGLOBAL].cName,"TGlobal");
  sprintf(output[OUT_TGLOBAL].cDescr,"Global mean temperature from POISE");
  sprintf(output[OUT_TGLOBAL].cNeg,"C");
  output[OUT_TGLOBAL].bNeg = 1;
  output[OUT_TGLOBAL].dNeg = 1; //conversion is hardcoded in write function
  output[OUT_TGLOBAL].iNum = 1;
  fnWrite[OUT_TGLOBAL] = &WriteTGlobal;
  
  sprintf(output[OUT_ALBEDOGLOBAL].cName,"AlbedoGlobal");
  sprintf(output[OUT_ALBEDOGLOBAL].cDescr,"Global mean bond albedo from POISE");
  output[OUT_ALBEDOGLOBAL].bNeg = 0;
  output[OUT_ALBEDOGLOBAL].iNum = 1;
  fnWrite[OUT_ALBEDOGLOBAL] = &WriteAlbedoGlobal;
  
  sprintf(output[OUT_TOTICEMASS].cName,"TotIceMass");
  sprintf(output[OUT_TOTICEMASS].cDescr,"Global total ice mass in ice sheets");
  sprintf(output[OUT_TOTICEMASS].cNeg,"kg");
  output[OUT_TOTICEMASS].bNeg = 1;
  output[OUT_TOTICEMASS].iNum = 1;
  fnWrite[OUT_TOTICEMASS] = &WriteTotIceMass;
  
  sprintf(output[OUT_FLUXINGLOBAL].cName,"FluxInGlobal");
  sprintf(output[OUT_FLUXINGLOBAL].cDescr,"Global mean flux in (insol*(1-albedo)) from POISE");
  /* Sadly, Russell, we must set the negative option to W/m^2.
  sprintf(output[OUT_FLUXINGLOBAL].cNeg,"pirate-ninjas/m^2");
  output[OUT_FLUXINGLOBAL].dNeg = 1/40.55185;
  */
  output[OUT_FLUXINGLOBAL].bNeg = 1;
  output[OUT_FLUXINGLOBAL].dNeg = 1; // Just in case
  sprintf(output[OUT_FLUXINGLOBAL].cNeg,"W/m^2");
  output[OUT_FLUXINGLOBAL].iNum = 1;
  fnWrite[OUT_FLUXINGLOBAL] = &WriteFluxInGlobal;
  
  sprintf(output[OUT_FLUXOUTGLOBAL].cName,"FluxOutGlobal");
  sprintf(output[OUT_FLUXOUTGLOBAL].cDescr,"Global mean flux out from POISE");
  /* Here, too
  sprintf(output[OUT_FLUXOUTGLOBAL].cNeg,"pirate-ninjas/m^2");
  output[OUT_FLUXOUTGLOBAL].dNeg = 1/40.55185;
  */
  output[OUT_FLUXOUTGLOBAL].bNeg = 1;
  output[OUT_FLUXOUTGLOBAL].dNeg = 1;
  sprintf(output[OUT_FLUXINGLOBAL].cNeg,"W/m^2");
  output[OUT_FLUXOUTGLOBAL].iNum = 1;
  fnWrite[OUT_FLUXOUTGLOBAL] = &WriteFluxOutGlobal;
  
  sprintf(output[OUT_TEMPLAT].cName,"TempLat");
  sprintf(output[OUT_TEMPLAT].cDescr,"Surface temperature by latitude.");
  sprintf(output[OUT_TEMPLAT].cNeg,"C");
  output[OUT_TEMPLAT].bNeg = 1;
  output[OUT_TEMPLAT].dNeg = 1; //conversion is hardcoded in write function
  output[OUT_TEMPLAT].iNum = 1;
  output[OUT_TEMPLAT].bGrid = 1;
  fnWrite[OUT_TEMPLAT] = &WriteTempLat; 
  
  sprintf(output[OUT_LATITUDE].cName,"Latitude");
  sprintf(output[OUT_LATITUDE].cDescr,"Latitude.");
  sprintf(output[OUT_LATITUDE].cNeg,"Degrees");
  output[OUT_LATITUDE].bNeg = 1;
  output[OUT_LATITUDE].dNeg = 1/DEGRAD; 
  output[OUT_LATITUDE].iNum = 1;
  output[OUT_LATITUDE].bGrid = 1;
  fnWrite[OUT_LATITUDE] = &WriteLatitude; 

  sprintf(output[OUT_ALBEDOLAT].cName,"AlbedoLat");
  sprintf(output[OUT_ALBEDOLAT].cDescr,"Surface albedo by latitude.");
  output[OUT_ALBEDOLAT].bNeg = 0;
  output[OUT_ALBEDOLAT].iNum = 1;
  output[OUT_ALBEDOLAT].bGrid = 1;
  fnWrite[OUT_ALBEDOLAT] = &WriteAlbedoLat; 
  
  sprintf(output[OUT_ANNUALINSOL].cName,"AnnInsol");
  sprintf(output[OUT_ANNUALINSOL].cDescr,"Annual insolation by latitude.");
  sprintf(output[OUT_ANNUALINSOL].cNeg,"W/m^2");
  output[OUT_ANNUALINSOL].bNeg = 1;
  output[OUT_ANNUALINSOL].dNeg = 1/40.55185;
  output[OUT_ANNUALINSOL].iNum = 1;
  output[OUT_ANNUALINSOL].bGrid = 1;
  fnWrite[OUT_ANNUALINSOL] = &WriteAnnualInsol; 
  
  sprintf(output[OUT_FLUXMERID].cName,"FluxMerid");
  sprintf(output[OUT_FLUXMERID].cDescr,"Meridional flux by latitude");
  sprintf(output[OUT_FLUXMERID].cNeg,"PW");
  output[OUT_FLUXMERID].bNeg = 1;
  output[OUT_FLUXMERID].dNeg = 1e-15;
  output[OUT_FLUXMERID].iNum = 1;
  output[OUT_FLUXMERID].bGrid = 1;
  fnWrite[OUT_FLUXMERID] = &WriteFluxMerid;  
  
  sprintf(output[OUT_FLUXIN].cName,"FluxIn");
  sprintf(output[OUT_FLUXIN].cDescr,"Incoming flux by latitude");
  sprintf(output[OUT_FLUXIN].cNeg,"W/m^2");
  output[OUT_FLUXIN].bNeg = 1;
  output[OUT_FLUXIN].dNeg = 1;
  output[OUT_FLUXIN].iNum = 1;
  output[OUT_FLUXIN].bGrid = 1;
  fnWrite[OUT_FLUXIN] = &WriteFluxIn; 
  
  sprintf(output[OUT_FLUXOUT].cName,"FluxOut");
  sprintf(output[OUT_FLUXOUT].cDescr,"Outgoing flux by latitude");
  sprintf(output[OUT_FLUXOUT].cNeg,"W/m^2");
  output[OUT_FLUXOUT].bNeg = 1;
  output[OUT_FLUXOUT].dNeg = 1;
  output[OUT_FLUXOUT].iNum = 1;
  output[OUT_FLUXOUT].bGrid = 1;
  fnWrite[OUT_FLUXOUT] = &WriteFluxOut; 
  
  sprintf(output[OUT_DIVFLUX].cName,"DivFlux");
  sprintf(output[OUT_DIVFLUX].cDescr,"Divergence of flux by latitude");
  sprintf(output[OUT_DIVFLUX].cNeg,"W/m^2");
  output[OUT_DIVFLUX].bNeg = 1;
  output[OUT_DIVFLUX].dNeg = 1;
  output[OUT_DIVFLUX].iNum = 1;
  output[OUT_DIVFLUX].bGrid = 1;
  fnWrite[OUT_DIVFLUX] = &WriteDivFlux; 
  
  sprintf(output[OUT_ICEMASS].cName,"IceMass");
  sprintf(output[OUT_ICEMASS].cDescr,"Mass of ice sheets/area by latitude");
  sprintf(output[OUT_ICEMASS].cNeg,"kg/m^2");
  output[OUT_ICEMASS].bNeg = 1;
  output[OUT_ICEMASS].dNeg = 1;
  output[OUT_ICEMASS].iNum = 1;
  output[OUT_ICEMASS].bGrid = 1;
  fnWrite[OUT_ICEMASS] = &WriteIceMass; 
  
  sprintf(output[OUT_ICEHEIGHT].cName,"IceHeight");
  sprintf(output[OUT_ICEHEIGHT].cDescr,"Height of ice sheets");
  sprintf(output[OUT_ICEHEIGHT].cNeg,"m");
  output[OUT_ICEHEIGHT].bNeg = 1;
  output[OUT_ICEHEIGHT].dNeg = 1;
  output[OUT_ICEHEIGHT].iNum = 1;
  output[OUT_ICEHEIGHT].bGrid = 1;
  fnWrite[OUT_ICEHEIGHT] = &WriteIceHeight; 
  
  sprintf(output[OUT_DICEMASSDT].cName,"DIceMassDt");
  sprintf(output[OUT_DICEMASSDT].cDescr,"derivative of mass of ice sheets/area by latitude");
  sprintf(output[OUT_DICEMASSDT].cNeg,"kg/m^2/s");
  output[OUT_DICEMASSDT].bNeg = 1;
  output[OUT_DICEMASSDT].dNeg = 1;
  output[OUT_DICEMASSDT].iNum = 1;
  output[OUT_DICEMASSDT].bGrid = 1;
  fnWrite[OUT_DICEMASSDT] = &WriteDIceMassDt; 
  
  sprintf(output[OUT_ICEFLOW].cName,"IceFlow");
  sprintf(output[OUT_ICEFLOW].cDescr,"flow of ice sheets/area by latitude");
  sprintf(output[OUT_ICEFLOW].cNeg,"kg/m^2/s");
  output[OUT_ICEFLOW].bNeg = 1;
  output[OUT_ICEFLOW].dNeg = 1;
  output[OUT_ICEFLOW].iNum = 1;
  output[OUT_ICEFLOW].bGrid = 1;
  fnWrite[OUT_ICEFLOW] = &WriteDIceMassDtFlow; 
  
  // sprintf(output[OUT_TIMELAT].cName,"Time");
//   sprintf(output[OUT_TIMELAT].cDescr,"time, printed for each latitude");
//   sprintf(output[OUT_TIMELAT].cNeg,"Gyr");
//   output[OUT_TIMELAT].bNeg = 1;
//   output[OUT_TIMELAT].dNeg = 1./(YEARSEC*1e9);
//   output[OUT_TIMELAT].iNum = 1;
//   output[OUT_TIMELAT].bGrid = 1;
//   fnWrite[OUT_TIMELAT] = &WriteTime; 
  
}

void FinalizeOutputFunctionPoise(OUTPUT *output,int iBody,int iModule) {
  
}


/************ POISE Logging Functions **************/

void LogOptionsPoise(CONTROL *control, FILE *fp) {
  fprintf(fp,"-------- POISE Options -----\n\n");
}

void LogPoise(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  fprintf(fp,"\n----- POISE PARAMETERS ------\n");
  for (iOut=OUTSTARTPOISE;iOut<OUTBODYSTARTPOISE;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }
}

void LogBodyPoise(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"-----POISE PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTPOISE;iOut<OUTENDPOISE;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

/************* MODULE Functions ***********/

void AddModulePoise(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = POISE;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlPoise;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyPoise;
  module->fnCountHalts[iBody][iModule] = &CountHaltsPoise;
  module->fnLogBody[iBody][iModule] = &LogBodyPoise;

  module->fnReadOptions[iBody][iModule] = &ReadOptionsPoise;
  module->fnVerify[iBody][iModule] = &VerifyPoise;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltPoise;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyPoise;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdatePoise;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputPoise;
  module->fnFinalizeUpdateIceMass[iBody][iModule] = &FinalizeUpdateIceMassPoise;
  
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionPoise;

}

/************* POISE Functions ***********/

void PropertiesPoise(BODY *body,UPDATE *update,int iBody) {  
  double deltax, Tice = 270, Aice, grav;
  int iLat;
  
  if (body[iBody].bEqtide) {
    body[iBody].dMeanMotion = \
          fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[iBody].dMass);
    body[iBody].iNDays = (int)floor(body[iBody].dRotRate/body[iBody].dMeanMotion);
    if (body[iBody].bClimateModel == SEA) {
      VerifyNStepSeasonal(body,iBody);
    }
  }
  
  /* deformability of ice, dependent on temperature */
  if (Tice < 263) {
    Aice = a1ICE * exp(-Q1ICE/(RGAS*Tice));
  } else {
    Aice = a2ICE * exp(-Q2ICE/(RGAS*Tice));
  }
  
  grav = BIGG*body[iBody].dMass/pow(body[iBody].dRadius,2);
  
  if (body[iBody].bIceSheets) {
    deltax = 2.0/body[iBody].iNumLats;
    for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
      body[iBody].daIceHeight[iLat] = body[iBody].daIceMass[iLat]/RHOICE;
    }
    
    for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
      /* calculate derivative to 2nd order accuracy */
      if (iLat == 0) {
        body[iBody].daDIceHeightDy[iLat] = sqrt(1.0-pow(body[iBody].daXBoundary[iLat+1],2)) * \
            (body[iBody].daIceHeight[iLat+1]-body[iBody].daIceHeight[iLat]) / \
            (body[iBody].dRadius*deltax);  
      } else if (iLat == (body[iBody].iNumLats-1)) {
        body[iBody].daDIceHeightDy[iLat] = sqrt(1.0-pow(body[iBody].daXBoundary[iLat],2)) * \
            (body[iBody].daIceHeight[iLat]-body[iBody].daIceHeight[iLat-1]) / \
            (body[iBody].dRadius*deltax);
      } else {
        body[iBody].daDIceHeightDy[iLat] = (sqrt(1.0-pow(body[iBody].daXBoundary[iLat+1],2)) *\
            (body[iBody].daIceHeight[iLat+1]-body[iBody].daIceHeight[iLat]) / \
            (body[iBody].dRadius*deltax) + sqrt(1.0-pow(body[iBody].daXBoundary[iLat],2)) *\
            (body[iBody].daIceHeight[iLat]-body[iBody].daIceHeight[iLat-1]) / \
            (body[iBody].dRadius*deltax) )/2.0;
      }
      body[iBody].daIceFlow[iLat] = 2*Aice*pow(RHOICE*grav,nGLEN)/(nGLEN+2.0) * \
          pow(fabs(body[iBody].daDIceHeightDy[iLat]),nGLEN-1) * \
          pow(body[iBody].daIceHeight[iLat],nGLEN+2);
    }
    
    for (iLat=0;iLat<body[iBody].iNumLats;iLat++) { 
      if (iLat == 0) {
        body[iBody].daIceFlowMid[iLat] = 0;
      } else if (iLat == body[iBody].iNumLats-1) {
        body[iBody].daIceFlowMid[iLat] = (body[iBody].daIceFlow[iLat] + \
           body[iBody].daIceFlow[iLat-1])/2.0;
        body[iBody].daIceFlowMid[iLat+1] = 0;
      } else {
        body[iBody].daIceFlowMid[iLat] = (body[iBody].daIceFlow[iLat] + \
           body[iBody].daIceFlow[iLat-1])/2.0;
      }    
    }
  }
}

void ForceBehaviorPoise(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,int iBody,int iModule) {
  int iLat;

  body[iBody].dIceMassTot = 0.0;
  if (body[iBody].bIceSheets) {
    for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
      if (body[iBody].daIceMass[iLat] < 0) {
        body[iBody].daIceMass[iLat] = 0.0;
      }//  else if (body[iBody].daIceMass[iLat] < 10 && update[iBody].daDeriv[update[iBody].iaIceMass[iLat]] < 0) {
//         body[iBody].daIceMass[iLat] = 0.0;
//       }
      if (body[iBody].bClimateModel == SEA) {
        body[iBody].dIceMassTot += body[iBody].daIceMass[iLat]*(2*PI*pow(body[iBody].dRadius,2)*(sin(body[iBody].daLats[1])-sin(body[iBody].daLats[0])))*body[iBody].daLandFrac[iLat]; 
      }
    }
  }
  if (body[iBody].bClimateModel == ANN) {
    PoiseAnnual(body,iBody);
  } else if (body[iBody].bClimateModel == SEA) {
    PoiseSeasonal(body,iBody);
    
  }
}

double true2eccA(double TrueA, double Ecc) {
  double cosE;
  cosE = (cos(TrueA) + Ecc) / (1.0 + Ecc*cos(TrueA));
  
  if (TrueA < PI) {
    return acos(cosE);
  } else {
    return (2*PI - acos(cosE));
  }
} 

void DailyInsolation(BODY *body, int iBody, int iDay) {
  int j;
  double Sconst, sin_delta, cos_delta, tan_delta, delta, HA;
  
  Sconst = body[0].dLuminosity / (4.*PI*pow(body[iBody].dSemi,2));
    
  sin_delta = sin(body[iBody].dObliquity)*sin(body[iBody].dTrueL);
  cos_delta = sqrt(1.0-pow(sin_delta,2));
  tan_delta = sin_delta/cos_delta;
  delta = asin(sin_delta);
  body[iBody].daDeclination[iDay] = delta;
  
  for (j=0;j<body[iBody].iNumLats;j++) {
    if (delta > 0.0) {
      /* Northern summer */
      if (body[iBody].daLats[j] >= (PI/2.-delta)) { 
        /* white night/no sunset */
        body[iBody].daInsol[j][iDay] = Sconst*sin(body[iBody].daLats[j])*sin_delta/pow(body[iBody].dAstroDist,2);
      } else if (-body[iBody].daLats[j] >= (PI/2.-delta) && body[iBody].daLats[j] < 0.0) {
        /* polar darkness/no sunrise */
        body[iBody].daInsol[j][iDay] = 0.0;
      } else {
        /* regular day/night cycle */
        HA = acos(-tan(body[iBody].daLats[j])*tan_delta);
        body[iBody].daInsol[j][iDay] = Sconst * (HA*sin(body[iBody].daLats[j])*sin_delta + \
            cos(body[iBody].daLats[j])*cos_delta*sin(HA)) / (PI*pow(body[iBody].dAstroDist,2));
      }
    } else {
      /* Southern summer */
      if (-body[iBody].daLats[j] >= (PI/2.+delta) && body[iBody].daLats[j] < 0.0) { 
        /* white night/no sunset */
        body[iBody].daInsol[j][iDay] = Sconst*sin(body[iBody].daLats[j])*sin_delta/pow(body[iBody].dAstroDist,2);
      } else if (body[iBody].daLats[j] >= (PI/2.+delta)) {
        /* polar darkness/no sunrise */
        body[iBody].daInsol[j][iDay] = 0.0;
      } else {
        /* regular day/night cycle */
        HA = acos(-tan(body[iBody].daLats[j])*tan_delta);
        body[iBody].daInsol[j][iDay] = Sconst * (HA*sin(body[iBody].daLats[j])*sin_delta + \
            cos(body[iBody].daLats[j])*cos_delta*sin(HA)) / (PI*pow(body[iBody].dAstroDist,2));
      }
    }
  }
}
          
void AnnualInsolation(BODY *body, int iBody) {
  int i, j;
  double LongP, TrueA, EccA, MeanL;
  
  LongP = body[iBody].dLongP; //Pericenter, relative to direction of primary at spring equinox

  body[iBody].dTrueL = -PI/2;        //starts the year at the (northern) winter solstice
  TrueA = body[iBody].dTrueL - LongP;
  while (TrueA < 0.0) TrueA += 2*PI;
  body[iBody].dEcc = sqrt(pow(body[iBody].dHecc,2)+pow(body[iBody].dKecc,2));
  body[iBody].dObliquity = atan2(sqrt(pow(body[iBody].dXobl,2)+pow(body[iBody].dYobl,2)),body[iBody].dZobl);
  EccA = true2eccA(TrueA, body[iBody].dEcc);
  MeanL = EccA - body[iBody].dEcc*sin(EccA) + LongP;
  
  for (j=0;j<body[iBody].iNumLats;j++)
    body[iBody].daAnnualInsol[j] = 0.0;
  
  for (i=0;i<body[iBody].iNDays;i++) {
    if (i!=0) {
      MeanL = MeanL + 2*PI/body[iBody].iNDays;
      /* This will only work for the secular orbital model. 
         Will need to be changed when/if resonances are added. */
      body[iBody].dMeanA = MeanL - LongP;
      kepler_eqn(body, iBody);
      EccA = body[iBody].dEccA;
      while (EccA >= 2*PI) EccA -= 2*PI;
      while (EccA < 0.0) EccA += 2*PI;
      if (EccA > PI) {
        TrueA = 2*PI - acos((cos(EccA) - body[iBody].dEcc)/(1.0 - body[iBody].dEcc*cos(EccA)));
      } else {
        TrueA = acos((cos(EccA) - body[iBody].dEcc)/(1.0 - body[iBody].dEcc*cos(EccA)));
      }      
      body[iBody].dTrueL = TrueA + LongP;     
    }
    
    while (body[iBody].dTrueL > 2*PI) body[iBody].dTrueL -= 2*PI;
    while (body[iBody].dTrueL < 0.0) body[iBody].dTrueL += 2*PI;
          
    // planet-star distance (units of semi-major axis):  
    body[iBody].dAstroDist = (1.0 - pow(body[iBody].dEcc,2))/(1.0+body[iBody].dEcc*cos(TrueA)); 
    
    DailyInsolation(body, iBody, i);
    
    for (j=0;j<body[iBody].iNumLats;j++) {
      body[iBody].daAnnualInsol[j] += body[iBody].daInsol[j][i]/((double)body[iBody].iNDays);
      
    }
  }
}

double OLRhm16(BODY *body, int iBody, int iLat) {
  double phi, Int, t, f;
  
  phi = log10(body[iBody].dpCO2);
  t = log10(body[iBody].daTemp[iLat]+273.15);
  f = -2.48628357550066070303*pow(t,4)+7.25686337527984814955*pow(t,3)*phi+\
    2.66003990584155758370e+01*pow(t,3)+1.28886975027996131260*pow(t,2)*pow(phi,2)-\
    4.52752012625326543116e+01*pow(t,2)*phi-1.03681774883879782578e+02*pow(t,2)-\
    1.87770911951353952629e-01*t*pow(phi,3)-7.09857849167248389222*t*pow(phi,2)+\
    9.06298526889689526342e+01*t*phi+1.77672602732389293578e+02*t+\
    1.04704125795963207124e-02*pow(phi,4)+4.97915275161904036239e-01*pow(phi,3)+\
    9.62210886703750745141*pow(phi,2)-5.72489253176301815529e+01*phi-\
    1.09454844237166085463e+02;
  Int = pow(10.0,f)/1000.;
  return Int;
}

double dOLRdThm16(BODY *body, int iBody, int iLat) {
  double phi, dI, t, f;
  
  phi = log10(body[iBody].dpCO2);
  t = log10(body[iBody].daTemp[iLat]+273.15);
  f = 4*(-2.48628357550066070303*pow(t,3))+3*7.25686337527984814955*pow(t,2)*phi+\
    3*2.66003990584155758370e+01*pow(t,2)+2*1.28886975027996131260*t*pow(phi,2)-\
    2*4.52752012625326543116e+01*t*phi-2*1.03681774883879782578e+02*t-\
    1.87770911951353952629e-01*pow(phi,3)-7.09857849167248389222*pow(phi,2)+\
    9.06298526889689526342e+01*phi+1.77672602732389293578e+02;
  dI = OLRhm16(body,iBody,iLat) * f / (body[iBody].daTemp[iLat]+273.15);
  return dI;
}

double OLRwk97(BODY *body, int iBody){
  double phi, Int, T;
  
  phi = log(body[iBody].dpCO2/3.3e-4);
  T = body[iBody].dTGlobal+273.15;
  Int = 9.468980 - 7.714727e-5*phi - 2.794778*T - 3.244753e-3*phi*T-3.547406e-4*pow(phi,2.) \
      + 2.212108e-2*pow(T,2) + 2.229142e-3*pow(phi,2)*T + 3.088497e-5*phi*pow(T,2) \
      - 2.789815e-5*pow(phi*T,2) - 3.442973e-3*pow(phi,3) - 3.361939e-5*pow(T,3) \
      + 9.173169e-3*pow(phi,3)*T - 7.775195e-5*pow(phi,3)*pow(T,2) \
      - 1.679112e-7*phi*pow(T,3) + 6.590999e-8*pow(phi,2)*pow(T,3) \
      + 1.528125e-7*pow(phi,3)*pow(T,3) - 3.367567e-2*pow(phi,4) - 1.631909e-4*pow(phi,4)*T \
      + 3.663871e-6*pow(phi,4)*pow(T,2) - 9.255646e-9*pow(phi,4)*pow(T,3);
  return Int;
}
  
double dOLRdTwk97(BODY *body, int iBody){
  double phi, dI, T;
  
  phi = log(body[iBody].dpCO2/3.3e-4);
  T = body[iBody].dTGlobal+273.15;
  dI = - 2.794778 + 2*2.212108e-2*T - 3*3.361939e-5*pow(T,2) - 3.244753e-3*phi \
       + 2*3.088497e-5*phi*T - 3*1.679112e-7*phi*pow(T,2) + 2.229142e-3*pow(phi,2) \
       - 2*2.789815e-5*pow(phi,2)*T + 3*6.590999e-8*pow(phi,2)*pow(T,2) \
       + 9.173169e-3*pow(phi,3) - 2*7.775195e-5*pow(phi,3)*T \
       + 3*1.528125e-7*pow(phi,3)*pow(T,2) - 1.631909e-4*pow(phi,4) \
       + 2*3.663871e-6*pow(phi,4)*T - 3*9.255646e-9*pow(phi,4)*pow(T,2);
  return dI;
}

void Albedo(BODY *body, int iBody) {
 int i;
 
 for (i=0;i<body[iBody].iNumLats;i++) { 
    if (body[iBody].dFixIceLat) {
       /* If user sets ice line latitude to be fixed, is it a cold pole or cold equator planet? */
       if (body[iBody].bJormungand) {
          /* Cold equator */
          if (fabs(body[iBody].daLats[i]) < body[iBody].dFixIceLat) {
            body[iBody].daAlbedo[i] = body[iBody].dIceAlbedo;
          } else {
            body[iBody].daAlbedo[i] = body[iBody].dSurfAlbedo;
          }
       } else {
          /* Cold poles */
          if (fabs(body[iBody].daLats[i]) > body[iBody].dFixIceLat) {
            body[iBody].daAlbedo[i] = body[iBody].dIceAlbedo;
          } else {
            body[iBody].daAlbedo[i] = body[iBody].dSurfAlbedo;
          }
       }
    } else if (body[iBody].bAlbedoZA) {
       /* Use albedo based on zenith angle */
       body[iBody].daAlbedo[i] = 0.31+0.04*(3*pow(sin(body[iBody].daLats[i]),2)-1);
    } else if (body[iBody].bIceSheets) {
       if (body[iBody].daIceMass[i] > 0) {
          body[iBody].daAlbedo[i] = body[iBody].dIceAlbedo;
       } else {
          body[iBody].daAlbedo[i] = body[iBody].dSurfAlbedo;
       }
    } else {
       if (body[iBody].daTemp[i] <= -10.0) {
         body[iBody].daAlbedo[i] = body[iBody].dIceAlbedo;
       } else {
         body[iBody].daAlbedo[i] = body[iBody].dSurfAlbedo;
       }
    }
  }
}

void MatrixInvert(BODY *body, int iBody) {
  double n = body[iBody].iNumLats;
  int i, j;
  float parity;
  
  // unitv = malloc(n*sizeof(double));
//   rowswap = malloc(n*sizeof(int));
// 
  if (body[iBody].bClimateModel == SEA) n *= 2;
  
 //ludcmp(body[iBody].dMEuler,n,body[iBody].rowswap,&parity);  
  LUDecomp(body[iBody].dMEuler,body[iBody].dMEulerCopy,body[iBody].scale,body[iBody].rowswap,n);
  for (i=0;i<n;i++) {
    for (j=0;j<n;j++) {
      if (j==i) {
        body[iBody].dUnitV[j] = 1.0;
      } else {
        body[iBody].dUnitV[j] = 0.0;
      }
    }

   // lubksb(body[iBody].dMEuler,n,body[iBody].rowswap,body[iBody].dUnitV);
    LUSolve(body[iBody].dMEulerCopy,body[iBody].dUnitV,body[iBody].rowswap, n);
    for (j=0;j<n;j++) {
      body[iBody].dInvM[j][i] = body[iBody].dUnitV[j];
    }
  }
//   free(unitv);
//   free(rowswap);
}

void TempGradient(BODY *body, double delta_x, int iBody) {
  int i;
  
  body[iBody].daTGrad[0] = (body[iBody].daTemp[1]-body[iBody].daTemp[0])/(delta_x);
  for (i=1;i<body[iBody].iNumLats-1;i++) {
    body[iBody].daTGrad[i] = (body[iBody].daTemp[i+1]-body[iBody].daTemp[i-1])/(2*delta_x); 
  }
  body[iBody].daTGrad[body[iBody].iNumLats-1] = (body[iBody].daTemp[body[iBody].iNumLats-1]-\
                          body[iBody].daTemp[body[iBody].iNumLats-2])/(delta_x);
}  
    
void PoiseAnnual(BODY *body, int iBody) {
  double delta_t, delta_x, xboundary, Tchange, tmpTglobal;
  double *lambda, **M, **Mcopy, **Mdiff, **invM,  *SourceF, *TempTerms, *tmpTemp, *tmpTempTerms, *Dmidpt; 
  int Nmax, i, j, n, k;
  
  /* Get cuurent climate parameters */
  Albedo(body, iBody);
  AnnualInsolation(body, iBody);
  if (body[iBody].bCalcAB) {
    body[iBody].dPlanckB = dOLRdTwk97(body,iBody);
    body[iBody].dPlanckA = OLRwk97(body,iBody) \
      - body[iBody].dPlanckB*(body[iBody].dTGlobal);
  }
  
  delta_t = 1.5/body[iBody].iNumLats;
  delta_x = 2.0/body[iBody].iNumLats;
  Nmax = 2000;
  
  /* Setup matrices, source function, temperature terms, global mean */
  for (i=0;i<body[iBody].iNumLats+1;i++) {
    xboundary = -1.0 + i*2.0/body[iBody].iNumLats;
    body[iBody].daLambda[i] = body[iBody].daDiffusion[i]*(1.0-pow(xboundary,2))/(pow(delta_x,2));
  }
  
  body[iBody].dTGlobal = 0.0;
  for (i=0;i<body[iBody].iNumLats;i++) {
    body[iBody].daTempTerms[i] = 0.0;
    
    for (j=0;j<body[iBody].iNumLats;j++) {
      if (j==i) {
        body[iBody].dMClim[i][j] = (-body[iBody].dPlanckB-body[iBody].daLambda[i+1]-body[iBody].daLambda[i])/body[iBody].dHeatCapAnn;
        body[iBody].dMDiff[i][j] = (-body[iBody].daLambda[i+1]-body[iBody].daLambda[i]);
        body[iBody].dMEuler[i][j] = -1.0/delta_t;
      } else if (j==(i+1)) {
        body[iBody].dMClim[i][j] = body[iBody].daLambda[j]/body[iBody].dHeatCapAnn;
        body[iBody].dMDiff[i][j] = body[iBody].daLambda[j];
        body[iBody].dMEuler[i][j] = 0.0;
      } else if (j==(i-1)) {
        body[iBody].dMClim[i][j] = body[iBody].daLambda[i]/body[iBody].dHeatCapAnn;
        body[iBody].dMDiff[i][j] = body[iBody].daLambda[i];
        body[iBody].dMEuler[i][j] = 0.0;
      } else {
        body[iBody].dMClim[i][j] = 0.0;
        body[iBody].dMDiff[i][j] = 0.0;
        body[iBody].dMEuler[i][j] = 0.0;
      }
      body[iBody].dMEuler[i][j] += 0.5*body[iBody].dMClim[i][j];
      body[iBody].daTempTerms[i] += body[iBody].dMClim[i][j]*body[iBody].daTemp[j];
    }
    body[iBody].daSourceF[i] = ((1.0-body[iBody].daAlbedo[i])*body[iBody].daAnnualInsol[i] - \
                         body[iBody].dPlanckA)/body[iBody].dHeatCapAnn;
    body[iBody].daTempTerms[i] += body[iBody].daSourceF[i];
    body[iBody].dTGlobal += body[iBody].daTemp[i]/body[iBody].iNumLats;
  }
  
  MatrixInvert(body,iBody);
    
  /* Relaxation to equilibrium */
  n = 1;
  Tchange = 1.0;
  while (fabs(Tchange) > 1e-12) {
    tmpTglobal = 0.0;
    for (i=0;i<body[iBody].iNumLats;i++) {
      body[iBody].daTmpTemp[i] = 0.0;
      body[iBody].daTmpTempTerms[i] = body[iBody].daSourceF[i];
      
      for (j=0;j<body[iBody].iNumLats;j++) {
        body[iBody].daTmpTemp[i] += -body[iBody].dInvM[i][j]*(0.5*(body[iBody].daTempTerms[j]+body[iBody].daSourceF[j])+body[iBody].daTemp[j]/delta_t);
        body[iBody].daTmpTempTerms[i] += body[iBody].dMClim[i][j]*body[iBody].daTemp[j];
      }
      tmpTglobal += body[iBody].daTmpTemp[i]/body[iBody].iNumLats;
    }
    Tchange = tmpTglobal - body[iBody].dTGlobal;
    
    /* Update albedo, source function, temperature, temperature terms, and global mean */
    Albedo(body,iBody);
    for (i=0;i<body[iBody].iNumLats;i++) {
      body[iBody].daSourceF[i] = ((1.0-body[iBody].daAlbedo[i])*body[iBody].daAnnualInsol[i] - \
                         body[iBody].dPlanckA)/body[iBody].dHeatCapAnn;
      body[iBody].daTemp[i] = body[iBody].daTmpTemp[i];
      body[iBody].daTempTerms[i] = body[iBody].daTmpTempTerms[i];
      body[iBody].dTGlobal = tmpTglobal;
    }
    if (n >= Nmax) {
      fprintf(stderr,"POISE solution not converged before max iterations reached.");
      exit(EXIT_INPUT);
    }
    n++;
  }
  
  /* Calculate some interesting quantities */
  body[iBody].dFluxInGlobal = 0.0;
  body[iBody].dFluxOutGlobal = 0.0;
  body[iBody].dAlbedoGlobal = 0.0;
  body[iBody].dIceMassTot = 0.0;
  TempGradient(body, delta_x, iBody);
  for (i=0;i<body[iBody].iNumLats;i++) {
    body[iBody].daDMidPt[i] = 0.5*(body[iBody].daDiffusion[i+1]+body[iBody].daDiffusion[i]);
    body[iBody].daFlux[i] = -2.*PI*pow(body[iBody].dRadius,2)*sqrt(1.0-pow(sin(body[iBody].daLats[i]),2)) * \
                            body[iBody].daDMidPt[i]*body[iBody].daTGrad[i];
    body[iBody].daFluxIn[i] = (1.0 - body[iBody].daAlbedo[i])*body[iBody].daAnnualInsol[i];
    body[iBody].daFluxOut[i] = body[iBody].dPlanckA + body[iBody].dPlanckB*body[iBody].daTemp[i];                       
    
    body[iBody].dFluxInGlobal += body[iBody].daFluxIn[i]/body[iBody].iNumLats;
    body[iBody].dFluxOutGlobal += body[iBody].daFluxOut[i]/body[iBody].iNumLats;
    
    body[iBody].daDivFlux[i] = 0.0;
    for (j=0;j<body[iBody].iNumLats;j++) {
      body[iBody].daDivFlux[i] += -body[iBody].dMDiff[i][j]*body[iBody].daTemp[j];
    }
    body[iBody].dAlbedoGlobal += body[iBody].daAlbedo[i]/body[iBody].iNumLats;
    body[iBody].dIceMassTot += body[iBody].daIceMass[i]*(2*PI*pow(body[iBody].dRadius,2)*(sin(body[iBody].daLats[1])-sin(body[iBody].daLats[0]))); //XXX only works if all lat cells are equal area!!

  } 
}

void AlbedoSeasonal(BODY *body, int iBody, int iDay) {
  int iLat;
  double zenith;
  
  body[iBody].dAlbedoGlobalTmp = 0;
  for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
    //zenith angle of sun at noon at each latitude
    zenith = fabs(body[iBody].daLats[iLat] - body[iBody].daDeclination[iDay]);
    
    body[iBody].daAlbedoLand[iLat] = body[iBody].dAlbedoLand+0.08*(3.*pow(sin(zenith),2)-1.)/2.;
    body[iBody].daAlbedoWater[iLat] = body[iBody].dAlbedoWater+0.08*(3.*pow(sin(zenith),2)-1.)/2.;
    if (body[iBody].daIceMassTmp[iLat] > 0 || body[iBody].daTempLand[iLat] <= -10) {
      body[iBody].daAlbedoLand[iLat] = body[iBody].dIceAlbedo;
    }
    if (body[iBody].daTempWater[iLat] <= body[iBody].dFrzTSeaIce) {
      body[iBody].daAlbedoWater[iLat] = body[iBody].dIceAlbedo;
    }
    body[iBody].daAlbedo[iLat] = body[iBody].daLandFrac[iLat]*body[iBody].daAlbedoLand[iLat]+ \
                              body[iBody].daWaterFrac[iLat]*body[iBody].daAlbedoWater[iLat];
    body[iBody].dAlbedoGlobalTmp += body[iBody].daAlbedo[iLat]/body[iBody].iNumLats;
  }  
  body[iBody].dAlbedoGlobal += \
            body[iBody].dAlbedoGlobalTmp/(body[iBody].iNStepInYear);
}

void SeaIce(BODY *body, int iBody) {
  int i, j;
  double nhicearea=0, shicearea=0, nhtotarea=0, shtotarea=0, nhfw, shfw, nhdW, shdW, Cw_dt;
  Cw_dt = body[iBody].dHeatCapWater/body[iBody].dSeasDeltat;
  
  for (i=0;i<2*body[iBody].iNumLats;i++) {
    for (j=0;j<2*body[iBody].iNumLats;j++) {
      body[iBody].dMEuler[i][j] = body[iBody].dMInit[i][j];
    }
  }
  
  for (i=0;i<body[iBody].iNumLats;i++) {
    if (body[iBody].daTempWater[i] <= body[iBody].dFrzTSeaIce) {
      if (body[iBody].daSeaIceHeight[i] > 0) {
        body[iBody].daSeaIceK[i] = body[iBody].dSeaIceConduct/body[iBody].daSeaIceHeight[i];
        body[iBody].daSourceLW[2*i+1] = body[iBody].daSeaIceK[i]*body[iBody].dFrzTSeaIce \
            - body[iBody].daSourceW[i];
        body[iBody].dMEuler[2*i+1][2*i+1] += -Cw_dt+body[iBody].daSeaIceK[i];
      } else {
        body[iBody].daSeaIceK[i] = 0.0;  
      }
    }
  }
  MatrixInvert(body,iBody);
  
  for (i=0;i<2*body[iBody].iNumLats;i++) {
    body[iBody].daTmpTemp[i] = 0.0;
    for (j=0;j<2*body[iBody].iNumLats;j++) {
      body[iBody].daTmpTemp[i] += body[iBody].dInvM[i][j]*body[iBody].daSourceLW[j];
    }
  }
  
  for (i=0;i<body[iBody].iNumLats;i++) {
    body[iBody].daTempLand[i] = body[iBody].daTmpTemp[2*i];
    body[iBody].daTempWater[i] = body[iBody].daTmpTemp[2*i+1];
    
    /* if sea ice present but T > -2, set to -2 */
    if (body[iBody].daSeaIceHeight[i] > 0) {
      if (body[iBody].daTempWater[i] > -2) {
        body[iBody].daTempWater[i] = -2.0;
      }
      
      body[iBody].daFluxSeaIce[i] = -body[iBody].daSourceW[i]-body[iBody].daPlanckB[i] \
              *body[iBody].daTempWater[i] - body[iBody].dNuLandWater/body[iBody].daWaterFrac[i]\
              *(body[iBody].daTmpTemp[2*i+1]-body[iBody].daTempLand[i]);
      for (j=0;j<body[iBody].iNumLats;j++) {
        body[iBody].daFluxSeaIce[i] += body[iBody].dMDiff[i][j]*body[iBody].daTmpTemp[2*j+1];
      }
      if (body[iBody].daLats[i] >= 0) {
        nhicearea += body[iBody].daWaterFrac[i];
        nhtotarea += body[iBody].daWaterFrac[i];
      } else {
        shicearea += body[iBody].daWaterFrac[i];
        shtotarea += body[iBody].daWaterFrac[i];
      }
    } else {
      body[iBody].daFluxSeaIce[i] = 0.0;
      /* no sea ice, still add to total area */
      if (body[iBody].daLats[i] >= 0) {
        nhtotarea += body[iBody].daWaterFrac[i];
      } else {
        shtotarea += body[iBody].daWaterFrac[i];
      }
    }
  }
  nhfw = 2.0*(2.0-2.0*(nhicearea - body[iBody].dSeasDeltax)/nhtotarea);
  shfw = 2.0*(2.0-2.0*(shicearea - body[iBody].dSeasDeltax)/shtotarea);
  if (nhfw > 4) nhfw = 4.0;
  if (shfw > 4) shfw = 4.0;
  nhdW = nhfw*nhicearea/(nhtotarea-nhicearea)/Cw_dt;
  shdW = shfw*shicearea/(shtotarea-shicearea)/Cw_dt;
  
  for (i=0;i<body[iBody].iNumLats;i++) {
    if (body[iBody].daSeaIceHeight[i] > 0) {
      if (body[iBody].daLats[i] >= 0) {
        body[iBody].daFluxSeaIce[i] += nhfw;
      } else {
        body[iBody].daFluxSeaIce[i] += shfw;
      }
    } else {
      if (body[iBody].daLats[i] >= 0) {
        body[iBody].daTempWater[i] -= nhdW;
      } else {
        body[iBody].daTempWater[i] -= shdW;
      }
    }
      
    body[iBody].daTemp[i] = body[iBody].daLandFrac[i]*body[iBody].daTempLand[i] + \
                            body[iBody].daWaterFrac[i]*body[iBody].daTempWater[i];
    body[iBody].dTGlobalTmp += body[iBody].daTemp[i]/body[iBody].iNumLats;
  }
  body[iBody].dTGlobal += \
          body[iBody].dTGlobalTmp/(body[iBody].iNStepInYear);
}  

void MatrixSeasonal(BODY *body, int iBody) {
  int i, j;
  double xboundary, nu_fl, nu_fw, Cl_dt, Cw_dt, dt_Lf;
  Cl_dt = body[iBody].dHeatCapLand/body[iBody].dSeasDeltat;
  Cw_dt = body[iBody].dHeatCapWater/body[iBody].dSeasDeltat;
  dt_Lf = body[iBody].dSeasDeltat/body[iBody].dLatentHeatIce;

  for (i=0;i<body[iBody].iNumLats+1;i++) {
//     xboundary = -1.0 + i*2.0/body[iBody].iNumLats;
    body[iBody].daLambda[i] = body[iBody].daDiffusion[i]*(1.0-pow(body[iBody].daXBoundary[i],2))/(pow(body[iBody].dSeasDeltax,2));
  }
  
  for (i=0;i<body[iBody].iNumLats;i++) {
    nu_fl = body[iBody].dNuLandWater/body[iBody].daLandFrac[i];
    nu_fw = body[iBody].dNuLandWater/body[iBody].daWaterFrac[i];
    for (j=0;j<body[iBody].iNumLats;j++) {
      if (j==i) {
        body[iBody].dMDiff[i][j] = (-body[iBody].daLambda[i+1]-body[iBody].daLambda[i]);
        body[iBody].dMLand[i][j] = Cl_dt+body[iBody].daPlanckB[i]+nu_fl-body[iBody].dMDiff[i][j];
        body[iBody].dMWater[i][j] = Cw_dt+body[iBody].daPlanckB[i]+nu_fw-body[iBody].dMDiff[i][j];
      } else if (j==(i+1)) {
        body[iBody].dMDiff[i][j] = body[iBody].daLambda[j];
        body[iBody].dMLand[i][j] = -body[iBody].daLambda[j];
        body[iBody].dMWater[i][j] = -body[iBody].daLambda[j];
      } else if (j==(i-1)) {
        body[iBody].dMDiff[i][j] = body[iBody].daLambda[i];
        body[iBody].dMLand[i][j] = -body[iBody].daLambda[i];
        body[iBody].dMWater[i][j] = -body[iBody].daLambda[i];
      } else {
        body[iBody].dMDiff[i][j] = 0.0;
        body[iBody].dMLand[i][j] = 0.0;
        body[iBody].dMWater[i][j] = 0.0;
      }  
    }
    body[iBody].dMInit[2*i][2*i+1] = -1*nu_fl;
    body[iBody].dMInit[2*i+1][2*i] = -1*nu_fw;

    for (j=0;j<body[iBody].iNumLats;j++) {
      body[iBody].dMInit[2*i][2*j] = body[iBody].dMLand[i][j];
      body[iBody].dMInit[2*i+1][2*j+1] = body[iBody].dMWater[i][j];
    }
  }
  
  for (i=0;i<2*body[iBody].iNumLats;i++) {
    for (j=0;j<2*body[iBody].iNumLats;j++) {
      body[iBody].dMEuler[i][j] = body[iBody].dMInit[i][j];
    }
  }
  
  MatrixInvert(body,iBody);
}

void SourceFSeas(BODY *body, int iBody, int day) {
  int i;
  
  for (i=0;i<body[iBody].iNumLats;i++) {
    body[iBody].daSourceL[i] = body[iBody].daPlanckA[i] \
            - (1.0-body[iBody].daAlbedoLand[i])*body[iBody].daInsol[i][day];
    body[iBody].daSourceW[i] = body[iBody].daPlanckA[i] \
            - (1.0-body[iBody].daAlbedoWater[i])*body[iBody].daInsol[i][day];
    body[iBody].daSourceLW[2*i] = body[iBody].daTempLand[i]\
                  * body[iBody].dHeatCapLand/body[iBody].dSeasDeltat-body[iBody].daSourceL[i];
    body[iBody].daSourceLW[2*i+1] = body[iBody].daTempWater[i]\
                  * body[iBody].dHeatCapWater/body[iBody].dSeasDeltat-body[iBody].daSourceW[i];
  }
}

void PoiseSeasonal(BODY *body, int iBody) {
  int i, j, nstep, nyear, day;
  double h;
  
  AnnualInsolation(body,iBody);
  
  h = 2*PI/body[iBody].dMeanMotion/body[iBody].iNStepInYear;

  /* main loop */
  for (nyear=0;nyear<body[iBody].iNumYears;nyear++) {
    body[iBody].dTGlobal = 0.0;
    
    body[iBody].dFluxOutGlobal = 0.0;
    
    for (i=0;i<body[iBody].iNumLats;i++) {
        //start of year, reset annual averages to zero 
        body[iBody].daTempAnnual[i] = 0.0;
        body[iBody].daAlbedoAnnual[i] = 0.0;
        body[iBody].daFluxAnnual[i] = 0.0;
        body[iBody].daFluxInAnnual[i] = 0.0;
        body[iBody].daDivFluxAnnual[i] = 0.0;
        body[iBody].daFluxOutAnnual[i] = 0.0;
        body[iBody].daIceBalanceAnnual[i] = 0.0;
        if (nyear == 0) {
          //reset ice sheet stuff only on first year       
          if (body[iBody].bIceSheets) {
            body[iBody].daIceMassTmp[i] = body[iBody].daIceMass[i];
          }
        }
    }
    AlbedoSeasonal(body,iBody,0);
    body[iBody].dAlbedoGlobal = 0.0;
    
    for (nstep=0;nstep<body[iBody].iNStepInYear;nstep++) {
      body[iBody].dTGlobalTmp = 0.0;
      body[iBody].dFluxOutGlobalTmp = 0.0;
      day = floor(body[iBody].dSeasDeltat*nstep*body[iBody].iNDays);
      
      SourceFSeas(body, iBody, day);
            
      if (body[iBody].bSeaIceModel == 1) {
        SeaIce(body,iBody);
        for (i=0;i<body[iBody].iNumLats;i++) {
          if (body[iBody].daSeaIceHeight[i] > 0) {
            // adjust height of present sea ice
            body[iBody].daSeaIceHeight[i] -=body[iBody].dSeasDeltat/body[iBody].dLatentHeatIce\
                                       * body[iBody].daFluxSeaIce[i];
            if (body[iBody].daSeaIceHeight[i] < 0) body[iBody].daSeaIceHeight[i] = 0;                           
          } else {
            if (body[iBody].daTempWater[i] < body[iBody].dFrzTSeaIce) {
              // create new sea ice 
              body[iBody].daSeaIceHeight[i] = \
                    -body[iBody].dHeatCapWater/body[iBody].dLatentHeatIce \
                    * (body[iBody].daTempWater[i] - body[iBody].dFrzTSeaIce);
            }
          }
          //calculate fluxes by latitude and global average 
          body[iBody].daFluxOutLand[i] = body[iBody].daPlanckA[i] \
                                  + body[iBody].daPlanckB[i]*body[iBody].daTempLand[i];
          body[iBody].daFluxOutWater[i] = body[iBody].daPlanckA[i] \
                                  + body[iBody].daPlanckB[i]*body[iBody].daTempWater[i];
          body[iBody].daFluxOut[i] = body[iBody].daLandFrac[i]*body[iBody].daFluxOutLand[i] + \
                                  body[iBody].daWaterFrac[i]*body[iBody].daFluxOutWater[i];
          body[iBody].dFluxOutGlobalTmp += body[iBody].daFluxOut[i]/body[iBody].iNumLats;
          
          body[iBody].daFluxInLand[i] = (1.0-body[iBody].daAlbedoLand[i])\
                                          *body[iBody].daInsol[i][day];
          body[iBody].daFluxInWater[i] = (1.0-body[iBody].daAlbedoWater[i])\
                                          *body[iBody].daInsol[i][day];
          body[iBody].daFluxIn[i] = body[iBody].daLandFrac[i]*body[iBody].daFluxInLand[i] + \
                                  body[iBody].daWaterFrac[i]*body[iBody].daFluxInWater[i];
          body[iBody].dFluxInGlobalTmp += body[iBody].daFluxIn[i]/body[iBody].iNumLats;
          
          // annual averages by latitude
          body[iBody].daTempAnnual[i] += body[iBody].daTemp[i]/body[iBody].iNStepInYear;
          body[iBody].daAlbedoAnnual[i] += body[iBody].daAlbedo[i]/body[iBody].iNStepInYear;
          body[iBody].daFluxInAnnual[i] += body[iBody].daFluxIn[i]/body[iBody].iNStepInYear;
          body[iBody].daFluxOutAnnual[i] += body[iBody].daFluxOut[i]/body[iBody].iNStepInYear;
          body[iBody].daTempDaily[i][nyear*body[iBody].iNStepInYear+nstep] = body[iBody].daTempLand[i];
          
          // ice growth/ablation
          if (body[iBody].bIceSheets) {  
              //calculate derivative of ice mass density and take an euler step
              body[iBody].daIceBalance[i][nstep] = IceMassBalance(body,iBody,i);
              body[iBody].daIceMassTmp[i] += h*body[iBody].daIceBalance[i][nstep];
              if (body[iBody].daIceMassTmp[i] < 0.0) {
                 body[iBody].daIceMassTmp[i] = 0.0;
              } //don't let ice mass become negative
          }
        }
        
        TempGradient(body, body[iBody].dSeasDeltax, iBody);
        for (i=0;i<body[iBody].iNumLats;i++) {
          body[iBody].daDMidPt[i] = 0.5*(body[iBody].daDiffusion[i+1]+body[iBody].daDiffusion[i]);
          body[iBody].daFlux[i] = -2.*PI*pow(body[iBody].dRadius,2)*sqrt(1.0-pow(sin(body[iBody].daLats[i]),2)) * \
                            body[iBody].daDMidPt[i]*body[iBody].daTGrad[i];
          body[iBody].daDivFlux[i] = 0.0;
          for (j=0;j<body[iBody].iNumLats;j++) {
            body[iBody].daDivFlux[i] += body[iBody].dMDiff[i][j]*body[iBody].daTemp[j];
            /* CC puts a minus sign here, so that a positive number is flux out of a cell.
               I prefer the opposite. ;) */
          }
        }
        
        body[iBody].dFluxOutGlobal += \
              body[iBody].dFluxOutGlobalTmp/(body[iBody].iNStepInYear);
        body[iBody].dFluxInGlobal += \
              body[iBody].dFluxInGlobalTmp/(body[iBody].iNStepInYear);
              
      } else {  
        for (i=0;i<2*body[iBody].iNumLats;i++) {
          body[iBody].daTmpTemp[i] = 0.0;
          for (j=0;j<2*body[iBody].iNumLats;j++) {
            body[iBody].daTmpTemp[i] += body[iBody].dInvM[i][j]*body[iBody].daSourceLW[j];
          }
        }
        for (i=0;i<body[iBody].iNumLats;i++) {
          //calculate temperature and fluxes by latitude and global average 
          body[iBody].daTempLand[i] = body[iBody].daTmpTemp[2*i];
          body[iBody].daTempWater[i] = body[iBody].daTmpTemp[2*i+1];
          body[iBody].daTemp[i] = body[iBody].daLandFrac[i]*body[iBody].daTempLand[i] + \
                                  body[iBody].daWaterFrac[i]*body[iBody].daTempWater[i];
          body[iBody].dTGlobalTmp += body[iBody].daTemp[i]/body[iBody].iNumLats;
          
          if (body[iBody].bCalcAB) {
            /* Calculate A and B from Haqq-Misra et al 2016 result */
            body[iBody].daPlanckB[i] = dOLRdThm16(body,iBody,i);
            body[iBody].daPlanckA[i] = OLRhm16(body,iBody,i) \
               - body[iBody].daPlanckB[i]*(body[iBody].daTemp[i]); 
          } 
            
          body[iBody].daFluxOutLand[i] = body[iBody].daPlanckA[i] \
                                  + body[iBody].daPlanckB[i]*body[iBody].daTempLand[i];
          body[iBody].daFluxOutWater[i] = body[iBody].daPlanckA[i] \
                                  + body[iBody].daPlanckB[i]*body[iBody].daTempWater[i];
          body[iBody].daFluxOut[i] = body[iBody].daLandFrac[i]*body[iBody].daFluxOutLand[i] + \
                                  body[iBody].daWaterFrac[i]*body[iBody].daFluxOutWater[i];
          body[iBody].dFluxOutGlobalTmp += body[iBody].daFluxOut[i]/body[iBody].iNumLats;
          
          body[iBody].daFluxInLand[i] = (1.0-body[iBody].daAlbedoLand[i])\
                                          *body[iBody].daInsol[i][day];
          body[iBody].daFluxInWater[i] = (1.0-body[iBody].daAlbedoWater[i])\
                                          *body[iBody].daInsol[i][day];
          body[iBody].daFluxIn[i] = body[iBody].daLandFrac[i]*body[iBody].daFluxInLand[i] + \
                                  body[iBody].daWaterFrac[i]*body[iBody].daFluxInWater[i];
          body[iBody].dFluxInGlobalTmp += body[iBody].daFluxIn[i]/body[iBody].iNumLats;
          
          // annual averages by latitude
          body[iBody].daTempAnnual[i] += body[iBody].daTemp[i]/body[iBody].iNStepInYear;
          body[iBody].daAlbedoAnnual[i] += body[iBody].daAlbedo[i]/body[iBody].iNStepInYear;
          body[iBody].daFluxInAnnual[i] += body[iBody].daFluxIn[i]/body[iBody].iNStepInYear;
          body[iBody].daFluxOutAnnual[i] += body[iBody].daFluxOut[i]/body[iBody].iNStepInYear;
          
          // ice growth/ablation
          if (body[iBody].bIceSheets) {  
              body[iBody].daIceBalance[i][nstep] = IceMassBalance(body,iBody,i);
              body[iBody].daIceMassTmp[i] += h*body[iBody].daIceBalance[i][nstep];
              if (body[iBody].daIceMassTmp[i] < 0.0) {
                body[iBody].daIceMassTmp[i] = 0.0;
              } //don't let ice mass become negative 
          }
          
          body[iBody].daTempDaily[i][nyear*body[iBody].iNStepInYear+nstep] = body[iBody].daTempLand[i];
        }
        
        TempGradient(body, body[iBody].dSeasDeltax, iBody);
        for (i=0;i<body[iBody].iNumLats;i++) {
          body[iBody].daDMidPt[i] = 0.5*(body[iBody].daDiffusion[i+1]+body[iBody].daDiffusion[i]);
          body[iBody].daFlux[i] = -2.*PI*pow(body[iBody].dRadius,2)*sqrt(1.0-pow(sin(body[iBody].daLats[i]),2)) * \
                            body[iBody].daDMidPt[i]*body[iBody].daTGrad[i];
          body[iBody].daDivFlux[i] = 0.0;
          for (j=0;j<body[iBody].iNumLats;j++) {
            body[iBody].daDivFlux[i] += body[iBody].dMDiff[i][j]*body[iBody].daTemp[j];
            /* CC puts a minus sign here, so that a positive number is flux out of a cell.
               I prefer the opposite. ;) */
          }
        }

        body[iBody].dTGlobal += \
              body[iBody].dTGlobalTmp/(body[iBody].iNStepInYear);
        body[iBody].dFluxOutGlobal += \
              body[iBody].dFluxOutGlobalTmp/(body[iBody].iNStepInYear);
        body[iBody].dFluxInGlobal += \
              body[iBody].dFluxInGlobalTmp/(body[iBody].iNStepInYear);
      }
      
      AlbedoSeasonal(body,iBody,day);
    }
    if (body[iBody].bIceSheets) {
      for (i=0;i<body[iBody].iNumLats;i++) {
        if (nyear != 0) {
          body[iBody].daIceBalanceAnnual[i] += h/2.*(body[iBody].daIceBalance[i][0])/ \
                         (body[iBody].iNumYears*2*PI/body[iBody].dMeanMotion);
        }
        for (nstep=1;nstep<body[iBody].iNStepInYear;nstep++) {
          //trapezoid rule
          //h = 2*PI/body[iBody].dMeanMotion/body[iBody].iNStepInYear;
          body[iBody].daIceBalanceAnnual[i] += h/2.*(body[iBody].daIceBalance[i][nstep]\
                         + body[iBody].daIceBalance[i][nstep-1])/ \
                         (body[iBody].iNumYears*2*PI/body[iBody].dMeanMotion);
          //above gets yearly average over NumYears
        }
        if (nyear != (body[iBody].iNumYears-1)) {
          body[iBody].daIceBalanceAnnual[i] += h/2.*(body[iBody].daIceBalance[i][nstep-1])/ \
                         (body[iBody].iNumYears*2*PI/body[iBody].dMeanMotion);
        }
      }
    }   
  }
}

double IceMassBalance(BODY *body, int iBody, int iLat) {
  double Tice = 273.15, dTmp;
  
  /* first, calculate melting/accumulation */
  if (body[iBody].daTempLand[iLat]>0.0) {
    /* Ice melting */
    /* (2.3 is used to match Huybers&Tziperman's ablation rate)*/
    dTmp = 2.3*SIGMA*(pow(Tice,4.0) - pow((body[iBody].daTempLand[iLat]+273.15),4.0))/LFICE;
  } else {
    if (body[iBody].dAlbedoGlobal >= 0.6) {
      /* no precip once planet is frozen */
      dTmp = 0.0;
    } else {
      if (body[iBody].dIceMassTot >= MOCEAN) {
        /* ice growth limited by mass of water available (really, really stupid) */
        dTmp = 0.0;
      } else {
        /* Ice deposits at fixed rate */
        dTmp = body[iBody].dIceDepRate;
      }
    }
  }
  return dTmp;
}

double fdPoiseDIceMassDtDepMelt(BODY *body, SYSTEM *system, int *iaBody) {
  double Tice = 273.15, dTmp;
  
  if (body[iaBody[0]].bClimateModel == ANN) {
    /* first, calculate melting/accumulation */
    if (body[iaBody[0]].daTemp[iaBody[1]]>0.0) {
      if (body[iaBody[0]].daIceMass[iaBody[1]] > 0.0) {
        /* Ice melting (2.3 is used to match Huybers&Tziperman's ablation rate)*/
        dTmp = 2.3*SIGMA*(pow(Tice,4.0) - pow((body[iaBody[0]].daTemp[iaBody[1]]+273.15),4.0))/LFICE;
      } else {
        dTmp = 0.0;
      }
    } else {
      if (body[iaBody[0]].dAlbedoGlobal >= body[iaBody[0]].dIceAlbedo) {
        /* no precip once planet is frozen */
        dTmp = 0.0;
      } else {
        if (body[iaBody[0]].dIceMassTot >= MOCEAN) {
          /* ice growth limited by mass of water available (really, really stupid) */
          dTmp = 0.0;
        } else {
          /* Ice deposits at fixed rate */
          dTmp = body[iaBody[0]].dIceDepRate;
        }
      }
    }
  } else {
    if (body[iaBody[0]].daIceMass[iaBody[1]] <= 0 && \
             body[iaBody[0]].daIceBalanceAnnual[iaBody[1]] < 0.0) {
      //if no ice present and derivative is negative, return 0
      dTmp = 0.0;
    } else {
      //ice derivative is calculated in PoiseSeasonal
      dTmp = body[iaBody[0]].daIceBalanceAnnual[iaBody[1]];
    }
    
    if ((body[iaBody[0]].dIceMassTot >= MOCEAN) && (dTmp > 0)) {
      dTmp = 0.0;
    }      
    
    if (body[iaBody[0]].dAlbedoGlobal == 0.6) {
      dTmp = 0.0;
    }
  }
  return dTmp;
}

double fdPoiseDIceMassDtFlow(BODY *body, SYSTEM *system, int *iaBody) {  
  double dTmp, deltax, deltayL, deltayR, dfdy;
  
  /* get deltay in meters at this latitude's edges */
  deltax = 2.0/body[iaBody[0]].iNumLats;
  deltayL = body[iaBody[0]].dRadius*deltax/sqrt(1.0-pow(body[iaBody[0]].daXBoundary[iaBody[1]],2));
  deltayR = body[iaBody[0]].dRadius*deltax/sqrt(1.0-pow(body[iaBody[0]].daXBoundary[iaBody[1]+1],2));

  if (iaBody[1] == 0) {
    dfdy =(body[iaBody[0]].daIceFlowMid[iaBody[1]+1]*(body[iaBody[0]].daIceHeight[iaBody[1]+1]\
      -body[iaBody[0]].daIceHeight[iaBody[1]]))/pow(deltayR,2);
  } else if (iaBody[1] == (body[iaBody[0]].iNumLats-1)) {
    dfdy = -body[iaBody[0]].daIceFlowMid[iaBody[1]]*(body[iaBody[0]].daIceHeight[iaBody[1]] -\
      body[iaBody[0]].daIceHeight[iaBody[1]-1])/pow(deltayL,2);
  } else {
    dfdy =(body[iaBody[0]].daIceFlowMid[iaBody[1]+1]*(body[iaBody[0]].daIceHeight[iaBody[1]+1]\
      -body[iaBody[0]].daIceHeight[iaBody[1]]))/pow(deltayR,2) - \
       (body[iaBody[0]].daIceFlowMid[iaBody[1]] * (body[iaBody[0]].daIceHeight[iaBody[1]] - \
      body[iaBody[0]].daIceHeight[iaBody[1]-1]))/pow(deltayL,2);
  }
  
  /* convert to mass */
  dTmp = dfdy*RHOICE;
  
  return dTmp;
}
  

