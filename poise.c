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
    for (iLat=0;iLat<src[iBody].iNumLats;iLat++) {
      dest[iBody].daIceMass[iLat] = src[iBody].daIceMass[iLat];
      dest[iBody].daTemp[iLat] = src[iBody].daTemp[iLat];
    }
  }
}

void InitializeBodyPoise(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
}

void InitializeUpdateTmpBodyPoise(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
//   control->Evolve.tmpBody[iBody].daIceMass = malloc(body[iBody].iNumLats*sizeof(double));
  control->Evolve.tmpBody[iBody].daTemp = malloc(body[iBody].iNumLats*sizeof(double));
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

void ReadTGlobalEst(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dTGlobal = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dTGlobal = options->dDefault;
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
    AssignDefaultInt(options,&body[iFile-1].bAlbedoZA,files->iNumInputs);
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
  
  sprintf(options[OPT_TGLOBALEST].cName,"dTGlobalEst");
  sprintf(options[OPT_TGLOBALEST].cDescr,"Estimate of initial global temperature");
  sprintf(options[OPT_TGLOBALEST].cDefault,"288");
  options[OPT_TGLOBALEST].dDefault = 288.0;
  options[OPT_TGLOBALEST].iType = 2;  
  options[OPT_TGLOBALEST].iMultiFile = 1;   
  fnRead[OPT_TGLOBALEST] = &ReadTGlobalEst;
  
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
}

void VerifyOLR(BODY *body, OPTIONS *options, char cFile[], int iBody, int iVerbose) {
  if (body[iBody].bCalcAB) {
    if (options[OPT_PLANCKA].iLine[iBody+1] > -1 || options[OPT_PLANCKB].iLine[iBody+1] > -1) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: Cannot set %s or %s when setting bCalcAB = 1 in File:%s\n", options[OPT_PLANCKA].cName, options[OPT_PLANCKB].cName, cFile);
      exit(EXIT_INPUT);
    }
    /* Calculate A and B from Williams & Kasting 1997 result */
    body[iBody].dPlanckB = dOLRdTwk97(body,iBody);
    body[iBody].dPlanckA = OLRwk97(body,iBody) \
      - body[iBody].dPlanckB*(body[iBody].dTGlobal - 273.15);
      
  } else {
    if (options[OPT_PCO2].iLine[iBody+1] > -1 || options[OPT_TGLOBALEST].iLine[iBody+1] > -1) {
      if (iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: Cannot set %s or %s unless setting bCalcAB = 1 in File:%s\n", options[OPT_PCO2].cName, options[OPT_TGLOBALEST].cName, cFile);
      exit(EXIT_INPUT);
    }
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
 
void InitializeClimateParams(BODY *body, int iBody) {
  int i;
  double Toffset, xboundary;
  
  body[iBody].daTemp = malloc(body[iBody].iNumLats*sizeof(double));   
  body[iBody].daDiffusion = malloc((body[iBody].iNumLats+1)*sizeof(double));
  body[iBody].daAlbedo = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daInsol = malloc(body[iBody].iNumLats*sizeof(double*));
  body[iBody].daAnnualInsol = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daTGrad = malloc(body[iBody].iNumLats*sizeof(double)); 
  body[iBody].daFlux = malloc(body[iBody].iNumLats*sizeof(double));  
  body[iBody].daFluxIn = malloc(body[iBody].iNumLats*sizeof(double)); 
  body[iBody].daFluxOut = malloc(body[iBody].iNumLats*sizeof(double)); 
  body[iBody].daDivFlux = malloc(body[iBody].iNumLats*sizeof(double));  
  
  body[iBody].daLambda = malloc((body[iBody].iNumLats+1)*sizeof(double));
  body[iBody].dMClim = malloc(body[iBody].iNumLats*sizeof(double*));
  body[iBody].dMDiff = malloc(body[iBody].iNumLats*sizeof(double*)); // matrix of heat diffusion terms only
  body[iBody].dMEuler = malloc(body[iBody].iNumLats*sizeof(double*));
  body[iBody].dMEulerCopy = malloc(body[iBody].iNumLats*sizeof(double*));
  body[iBody].dInvM = malloc(body[iBody].iNumLats*sizeof(double*));
  body[iBody].daSourceF = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daTempTerms = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daTmpTemp = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daTmpTempTerms = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].daDMidPt = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].rowswap =  malloc(body[iBody].iNumLats*sizeof(int));
  body[iBody].scale = malloc(body[iBody].iNumLats*sizeof(double));
  body[iBody].dUnitV = malloc(3*sizeof(double));
    
  body[iBody].iNDays = (int)floor(body[iBody].dRotRate/body[iBody].dMeanMotion); //number of days in year
  
  if (body[iBody].bColdStart) {
    Toffset = -40.0;
  } else {
    Toffset = 0.0;
  }
 //  
//   if (body[iBody].bIceSheets) {
//     body[iBody].daIceHeight = malloc(body[iBody].iNumLats*sizeof(double));
//   }
  
  body[iBody].dTGlobal = 0.0;
  body[iBody].dAlbedoGlobal = 0.0;
  body[iBody].daDiffusion[0] = body[iBody].dDiffCoeff;
  for (i=0;i<=body[iBody].iNumLats;i++) {
    if (i!=body[iBody].iNumLats) {
      body[iBody].daTemp[i] = 20.*(1.0-2*pow(sin(body[iBody].daLats[i]),2))+Toffset;
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
//           body[iBody].daIceHeight[i] = body[iBody].dInitIceHeight;
        } else {
          body[iBody].daIceMass[i] = 0.0;
//           body[iBody].daIceHeight[i] = 0.0;
        }
      }
    }  
    
    if (body[iBody].bMEPDiff) {   
      body[iBody].daDiffusion[i] = body[iBody].dPlanckB/4.0;   
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
  
  AnnualInsolation(body, iBody);

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
    
void InitializeIceMassPoise(BODY *body,UPDATE *update,int iBody,int iLat) {
  update[iBody].iaType[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassPoise[iLat]] = 1;
  update[iBody].padDIceMassDtPoise[iLat] = &update[iBody].daDerivProc[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassPoise[iLat]];
  update[iBody].iNumBodies[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassPoise[iLat]]=2;
  update[iBody].iaBody[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassPoise[iLat]] = malloc(2*sizeof(int));
  update[iBody].iaBody[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassPoise[iLat]][0] = iBody;
  update[iBody].iaBody[update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassPoise[iLat]][1] = iLat;
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
    for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
      InitializeIceMassPoise(body,update,iBody,iLat);
      fnUpdate[iBody][update[iBody].iaIceMass[iLat]][update[iBody].iaIceMassPoise[iLat]] = &fdPoiseDIceMassDt;   
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
    update[iBody].iNumIceMass = 1;
//     body[iBody].daIceMass = malloc(body[iBody].iNumLats*sizeof(double));
    update[iBody].iaIceMass = malloc(body[iBody].iNumLats*sizeof(int));
    update[iBody].padDIceMassDtPoise = malloc(body[iBody].iNumLats*sizeof(double*));
    update[iBody].iaIceMassPoise = malloc(body[iBody].iNumLats*sizeof(int));
  }
} 

void FinalizeUpdateIceMassPoise(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iLat) {
  update[iBody].iaModule[iVar][0] = POISE;
  update[iBody].iaIceMassPoise[iLat] = *iEqn;
  (*iEqn)++;
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
  *dTmp = body[iBody].daTemp[body[iBody].iWriteLat];
  
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
  *dTmp = body[iBody].daAlbedo[body[iBody].iWriteLat];
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
  *dTmp = body[iBody].daFluxIn[body[iBody].iWriteLat];
  
  if (output->bDoNeg[iBody]) {
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime,units->iMass,units->iLength);
    fsUnitsEnergyFlux(units,cUnit);
  }
}

void WriteFluxOut(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].daFluxOut[body[iBody].iWriteLat];
  
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
}

void ForceBehaviorPoise(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,int iBody,int iModule) {
  int iLat;
  for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
    if (body[iBody].daIceMass[iLat] < 0) {
      body[iBody].daIceMass[iLat] = 0.0;
    }
  }

  PoiseClimate(body,iBody);
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
  
  for (i=0;i<=body[iBody].iNDays;i++) {
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

double OLRwk97(BODY *body, int iBody){
  double phi, Int, T;
  
  phi = log(body[iBody].dpCO2/3.3e-4);
  T = body[iBody].dTGlobal;
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
  T = body[iBody].dTGlobal;
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
            body[iBody].daAlbedo[i] = 0.6;
          } else {
            body[iBody].daAlbedo[i] = 0.3;
          }
       } else {
          /* Cold poles */
          if (fabs(body[iBody].daLats[i]) > body[iBody].dFixIceLat) {
            body[iBody].daAlbedo[i] = 0.6;
          } else {
            body[iBody].daAlbedo[i] = 0.3;
          }
       }
    } else if (body[iBody].bAlbedoZA) {
       /* Use albedo based on zenith angle */
       body[iBody].daAlbedo[i] = 0.31+0.04*(3*pow(sin(body[iBody].daLats[i]),2)-1);
    } else if (body[iBody].bIceSheets) {
       if (body[iBody].daIceMass[i] > 0) {
          body[iBody].daAlbedo[i] = 0.6;
       } else {
          body[iBody].daAlbedo[i] = 0.3;
       }
    } else {
       if (body[iBody].daTemp[i] <= -10.0) {
         body[iBody].daAlbedo[i] = 0.6;
       } else {
         body[iBody].daAlbedo[i] = 0.3;
       }
    }
  }
}
    
// void DiagMatrix(double *v, double **M, int n, int offset) {
//   /* Construct a square diagonal nxn matrix from vector v
//      Diagonal can be offset  */
//   int i;
//    
//   for (i=0;i<(n-abs(offset));i++) {
//     if (offset >= 0) {
//       M[i][i+offset] = v[i];
//     } else {
//       M[i+offset][i] = v[i];
//     }
//   }
// }

void MatrixInvert(BODY *body, int iBody) {
  double n = body[iBody].iNumLats;
  int i, j;
  
  // unitv = malloc(n*sizeof(double));
//   rowswap = malloc(n*sizeof(int));
//   
//   ludcmp(Mcopy,n,rowswap,&parity);
  LUDecomp(body[iBody].dMEuler,body[iBody].dMEulerCopy,body[iBody].scale,body[iBody].rowswap,n);
  for (i=0;i<n;i++) {
    for (j=0;j<n;j++) {
      if (j==i) {
        body[iBody].dUnitV[j] = 1.0;
      } else {
        body[iBody].dUnitV[j] = 0.0;
      }
    }

//     lubksb(Mcopy,n,rowswap,unitv);
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
    
void PoiseClimate(BODY *body, int iBody) {
  double delta_t, delta_x, xboundary, Tchange, tmpTglobal;
  double *lambda, **M, **Mcopy, **Mdiff, **invM,  *SourceF, *TempTerms, *tmpTemp, *tmpTempTerms, *Dmidpt; 
  int Nmax, i, j, n, k;
  
  /* Get cuurent climate parameters */
  Albedo(body, iBody);
  AnnualInsolation(body, iBody);
  
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
  } 
}

double fdPoiseDIceMassDt(BODY *body, SYSTEM *system, int *iaBody) {
  double Tice = 273.0;
  if (body[iaBody[0]].daTemp[iaBody[1]]>0.0) {
    if (body[iaBody[0]].daIceMass[iaBody[1]] > 0.0) {
      /* Ice melting */
      return SIGMA*(pow(Tice,4.0) - pow((body[iaBody[0]].daTemp[iaBody[1]]+273.15),4.0))/LFICE;
    } else {
      return 0.0;
    }
  } else {
    /* Ice deposits at fixed rate */
    return 5.0e-5;
  } 
}
  
  

