/********************** STELLAR.C **********************/
/*
 * Rodrigo Luger, Tue May 12 11:11:38 PDT 2015
 *
 * Subroutines that control the integration of the 
 * atmospheric escape model.
*/
// NOTE: Still need to edit module.c, update.c

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void BodyCopyStellar(BODY *dest,BODY *src,int foo,int iNumBodies,int iBody) {
  dest[iBody].dLuminosity = src[iBody].dLuminosity;
  dest[iBody].dTemperature = src[iBody].dTemperature;
  dest[iBody].dSatXUVFrac = src[iBody].dSatXUVFrac;
  dest[iBody].dSatXUVTime = src[iBody].dSatXUVTime;
  dest[iBody].dXUVBeta = src[iBody].dXUVBeta;
  dest[iBody].iStellarModel = src[iBody].iStellarModel;
  dest[iBody].iWindModel = src[iBody].iWindModel;
  dest[iBody].iXUVModel = src[iBody].iXUVModel;
  dest[iBody].dLXUV = src[iBody].dLXUV;
}

/**************** STELLAR options ********************/

void ReadSatXUVFrac(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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
    body[iFile-1].dSatXUVFrac = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dSatXUVFrac = options->dDefault;
}

void ReadSatXUVTime(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dSatXUVTime = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dSatXUVTime = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dSatXUVTime = options->dDefault;
}

void ReadXUVBeta(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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
    body[iFile-1].dXUVBeta = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dXUVBeta = options->dDefault;
}

void ReadStellarModel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"ba",2)) {
      body[iFile-1].iStellarModel = STELLAR_MODEL_BARAFFE;
    } else if (!memcmp(sLower(cTmp),"no",2)) {
      body[iFile-1].iStellarModel = STELLAR_MODEL_NONE;
    } else if (!memcmp(sLower(cTmp),"pr",2)) {
      body[iFile-1].iStellarModel = STELLAR_MODEL_PROXIMACEN;
    } else {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are BARAFFE, PROXIMACEN, or NONE.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);	
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].iStellarModel = STELLAR_MODEL_BARAFFE;
}

void ReadWindModel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"re",2)) {
      body[iFile-1].iWindModel = STELLAR_MODEL_REINERS;
    } else if (!memcmp(sLower(cTmp),"no",2)) {
      body[iFile-1].iWindModel = STELLAR_MODEL_NONE;
    } else {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are REINERS or NONE.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);	
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].iWindModel = STELLAR_MODEL_REINERS;
}

void ReadXUVModel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"ri",2)) {
      body[iFile-1].iXUVModel = STELLAR_MODEL_RIBAS;
    } else if (!memcmp(sLower(cTmp),"no",2)) {
      body[iFile-1].iXUVModel = STELLAR_MODEL_NONE;
    } else if (!memcmp(sLower(cTmp),"re",2)) {
      if (control->Io.iVerbose >= VERBINPUT)
	      fprintf(stderr,"WARNING: The REINERS XUV model has serious issues. The recommended model is RIBAS.\n");
      body[iFile-1].iXUVModel = STELLAR_MODEL_REINERS;
    } else {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are RIBAS, REINERS or NONE.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);	
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].iXUVModel = STELLAR_MODEL_RIBAS;
}


void ReadHZModel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"k",1)) {
      body[iFile-1].iXUVModel = HZ_MODEL_KOPPARAPU;
      // We should add more!
    } else {
      if (control->Io.iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are KOPPARAPU13.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);	
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].iHZModel = HZ_MODEL_KOPPARAPU;
}


void ReadLuminosity(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dLuminosity = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dLuminosity = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dLuminosity = options->dDefault;
}

void ReadTemperature(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dTemperature = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dTemperature = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dTemperature = options->dDefault;
}

/* Halts */

void ReadHaltEndBaraffeGrid(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Halt[iFile-1].bEndBaraffeGrid = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultInt(options,&control->Halt[iFile-1].bEndBaraffeGrid,files->iNumInputs); 
  }
}

void InitializeOptionsStellar(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  sprintf(options[OPT_SATXUVFRAC].cName,"dSatXUVFrac");
  sprintf(options[OPT_SATXUVFRAC].cDescr,"Saturated XUV Luminosity Fraction");
  sprintf(options[OPT_SATXUVFRAC].cDefault,"1e-3");
  options[OPT_SATXUVFRAC].dDefault = 1.e-3;
  options[OPT_SATXUVFRAC].iType = 0;
  options[OPT_SATXUVFRAC].iMultiFile = 1;
  fnRead[OPT_SATXUVFRAC] = &ReadSatXUVFrac;

  sprintf(options[OPT_SATXUVTIME].cName,"dSatXUVTime");
  sprintf(options[OPT_SATXUVTIME].cDescr,"XUV saturation time");
  sprintf(options[OPT_SATXUVTIME].cDefault,"0.1 Gyr");
  options[OPT_SATXUVTIME].dDefault = 1.e8 * YEARSEC;
  options[OPT_SATXUVTIME].iType = 0;
  options[OPT_SATXUVTIME].iMultiFile = 1;
  options[OPT_SATXUVTIME].dNeg = 1e9*YEARSEC;
  sprintf(options[OPT_SATXUVTIME].cNeg,"Gyr");
  fnRead[OPT_SATXUVTIME] = &ReadSatXUVTime;

  sprintf(options[OPT_XUVBETA].cName,"dXUVBeta");
  sprintf(options[OPT_XUVBETA].cDescr,"XUV decay power law exponent");
  sprintf(options[OPT_XUVBETA].cDefault,"1.23");
  options[OPT_XUVBETA].dDefault = 1.23;
  options[OPT_XUVBETA].iType = 0;
  options[OPT_XUVBETA].iMultiFile = 1;
  fnRead[OPT_XUVBETA] = &ReadXUVBeta;  

  sprintf(options[OPT_STELLARMODEL].cName,"sStellarModel");
  sprintf(options[OPT_STELLARMODEL].cDescr,"Luminosity Evolution Model");
  sprintf(options[OPT_STELLARMODEL].cDefault,"BARAFFE");
  options[OPT_STELLARMODEL].iType = 3;
  options[OPT_STELLARMODEL].iMultiFile = 1;
  fnRead[OPT_STELLARMODEL] = &ReadStellarModel;

  sprintf(options[OPT_WINDMODEL].cName,"sWindModel");
  sprintf(options[OPT_WINDMODEL].cDescr,"Wind Angular Momentum Loss Model");
  sprintf(options[OPT_WINDMODEL].cDefault,"REINERS");
  options[OPT_WINDMODEL].iType = 3;
  options[OPT_WINDMODEL].iMultiFile = 1;
  fnRead[OPT_WINDMODEL] = &ReadWindModel;

  sprintf(options[OPT_XUVMODEL].cName,"sXUVModel");
  sprintf(options[OPT_XUVMODEL].cDescr,"XUV Evolution Model");
  sprintf(options[OPT_XUVMODEL].cDefault,"RIBAS");
  options[OPT_XUVMODEL].iType = 3;
  options[OPT_XUVMODEL].iMultiFile = 1;
  fnRead[OPT_XUVMODEL] = &ReadXUVModel;

  sprintf(options[OPT_HZMODEL].cName,"sHZModel");
  sprintf(options[OPT_HZMODEL].cDescr,"Habitable Zone Model: Kopparapu13");
  sprintf(options[OPT_HZMODEL].cDefault,"Kopparapu13");
  options[OPT_HZMODEL].iType = 3;
  options[OPT_HZMODEL].iMultiFile = 1;
  fnRead[OPT_HZMODEL] = &ReadHZModel;

  sprintf(options[OPT_LUMINOSITY].cName,"dLuminosity");
  sprintf(options[OPT_LUMINOSITY].cDescr,"Initial Luminosity");
  sprintf(options[OPT_LUMINOSITY].cDefault,"LSUN");
  options[OPT_LUMINOSITY].dDefault = LSUN;
  options[OPT_LUMINOSITY].iType = 0;
  options[OPT_LUMINOSITY].iMultiFile = 1;
  options[OPT_LUMINOSITY].dNeg = LSUN;
  sprintf(options[OPT_LUMINOSITY].cNeg,"Solar Luminosity (LSUN)");
  fnRead[OPT_LUMINOSITY] = &ReadLuminosity;

  sprintf(options[OPT_TEMPERATURE].cName,"dTemperature");
  sprintf(options[OPT_TEMPERATURE].cDescr,"Initial Effective Temperature");
  sprintf(options[OPT_TEMPERATURE].cDefault,"TSUN");
  options[OPT_TEMPERATURE].dDefault = TSUN;
  options[OPT_TEMPERATURE].iType = 0;
  options[OPT_TEMPERATURE].iMultiFile = 1;
  fnRead[OPT_TEMPERATURE] = &ReadTemperature;

  sprintf(options[OPT_HALTENDBARAFFEFGRID].cName,"bHaltEndBaraffeGrid");
  sprintf(options[OPT_HALTENDBARAFFEFGRID].cDescr,"Halt when we reach the end of the Baraffe+15 grid?");
  sprintf(options[OPT_HALTENDBARAFFEFGRID].cDefault,"1");
  options[OPT_HALTENDBARAFFEFGRID].iType = 0;
  fnRead[OPT_HALTENDBARAFFEFGRID] = &ReadHaltEndBaraffeGrid;

}

void ReadOptionsStellar(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTSTELLAR;iOpt<OPTENDSTELLAR;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify STELLAR ******************/

void VerifyRotRate(BODY *body, CONTROL *control, OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
  update[iBody].iaType[update[iBody].iRot][update[iBody].iRotStellar] = 1;
  update[iBody].iNumBodies[update[iBody].iRot][update[iBody].iRotStellar] = 1;
  update[iBody].iaBody[update[iBody].iRot][update[iBody].iRotStellar] = malloc(update[iBody].iNumBodies[update[iBody].iRot][update[iBody].iRotStellar]*sizeof(int));
  update[iBody].iaBody[update[iBody].iRot][update[iBody].iRotStellar][0] = iBody;

  update[iBody].pdRotRateStellar = &update[iBody].daDerivProc[update[iBody].iRot][update[iBody].iRotStellar];
  fnUpdate[iBody][update[iBody].iRot][update[iBody].iRotStellar] = &fdDRotRateDt; 
}

void VerifyLuminosity(BODY *body, CONTROL *control, OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  // Assign luminosity
  if (body[iBody].iStellarModel == STELLAR_MODEL_BARAFFE) {
    body[iBody].dLuminosity = fdLuminosityFunctionBaraffe(body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_LUMINOSITY].iLine[iBody+1] >= 0) {
      // User specified luminosity, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) 
        printf("WARNING: Luminosity set for body %d, but this value will be computed from the grid.\n", iBody);
    }
  } else if (body[iBody].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    body[iBody].dLuminosity = fdLuminosityFunctionProximaCen(body[iBody].dAge,body[iBody].dMass);
    if (options[OPT_LUMINOSITY].iLine[iBody+1] >= 0) {
      // User specified luminosity, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) 
        printf("WARNING: Luminosity set for body %d, but this value will be computed from the grid.\n", iBody);
    }
  }

  update[iBody].iaType[update[iBody].iLuminosity][0] = 0;
  update[iBody].iNumBodies[update[iBody].iLuminosity][0] = 1;
  update[iBody].iaBody[update[iBody].iLuminosity][0] = malloc(update[iBody].iNumBodies[update[iBody].iLuminosity][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iLuminosity][0][0] = iBody;

  update[iBody].pdLuminosityStellar = &update[iBody].daDerivProc[update[iBody].iLuminosity][0];  // NOTE: This points to the VALUE of the luminosity
  fnUpdate[iBody][update[iBody].iLuminosity][0] = &fdLuminosity;                                 // NOTE: Same here!
}

void VerifyRadius(BODY *body, CONTROL *control, OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  // Assign radius
  if (body[iBody].iStellarModel == STELLAR_MODEL_BARAFFE) {
    body[iBody].dRadius = fdRadiusFunctionBaraffe(body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_RADIUS].iLine[iBody+1] >= 0) {
      // User specified radius, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) 
        printf("WARNING: Radius set for body %d, but this value will be computed from the grid.\n", iBody);
    }
  } else if (body[iBody].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    body[iBody].dRadius = fdRadiusFunctionProximaCen(body[iBody].dAge,body[iBody].dMass);
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

  update[iBody].pdRadiusStellar = &update[iBody].daDerivProc[update[iBody].iRadius][0];  // NOTE: This points to the VALUE of the radius
  fnUpdate[iBody][update[iBody].iRadius][0] = &fdRadius;                                 // NOTE: Same here!
}

void VerifyTemperature(BODY *body, CONTROL *control, OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {


  // Assign temperature
  if (body[iBody].iStellarModel == STELLAR_MODEL_BARAFFE) {
    body[iBody].dTemperature = fdTemperatureFunctionBaraffe(body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_TEMPERATURE].iLine[iBody+1] >= 0) {
      // User specified temperature, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) 
        printf("WARNING: Temperature set for body %d, but this value will be computed from the grid.\n", iBody);
    }
  } else if (body[iBody].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    body[iBody].dTemperature = fdTemperatureFunctionProximaCen(body[iBody].dAge,body[iBody].dMass);
    if (options[OPT_TEMPERATURE].iLine[iBody+1] >= 0) {
      // User specified temperature, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT) 
        printf("WARNING: Temperature set for body %d, but this value will be computed from the grid.\n", iBody);
    }
  }

  update[iBody].iaType[update[iBody].iTemperature][0] = 0;
  update[iBody].iNumBodies[update[iBody].iTemperature][0] = 1;
  update[iBody].iaBody[update[iBody].iTemperature][0] = malloc(update[iBody].iNumBodies[update[iBody].iTemperature][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iTemperature][0][0] = iBody;

  update[iBody].pdTemperatureStellar = &update[iBody].daDerivProc[update[iBody].iTemperature][0];  // NOTE: This points to the VALUE of the temperature
  fnUpdate[iBody][update[iBody].iTemperature][0] = &fdTemperature;                                 // NOTE: Same here!
}

void fnPropertiesStellar(BODY *body, EVOLVE *evolve, UPDATE *update, int iBody) {

  // Update LXUV
  if (body[iBody].iXUVModel == STELLAR_MODEL_REINERS) {
  
    // REINERS wind model
    double dPer, dLXRay, dLXRaySat, dLEUV;
    dPer = 2 * PI / body[iBody].dRotRate;
  
    // Unsaturated regime (Reiners, Schussler & Passegger 2014, eqn. (11))
    dLXRay = 1.e-7 * pow(10., 30.71 - 2.01 * log10(dPer / DAYSEC));

    // Saturated regime (Reiners, Schussler & Passegger 2014)
    dLXRaySat = body[iBody].dLuminosity * pow(10., -3.12 - 0.11 * log10(dPer / DAYSEC));
    
    // Take the lesser value
    if (dLXRay > dLXRaySat) dLXRay = dLXRaySat;
      
    // Sanz-Forcada et al. (2011), eqn (3)
    dLEUV = 1.e7 * pow(10., 4.80 + 0.860 * log10(dLXRay * 1.e-7));
  
    // NOTE: We should add XRay and EUV to get XUV, but the Sanz-Forcada
    // model above yields unrealistically high EUV luminosities for M dwarfs.
    body[iBody].dLXUV = dLXRay;
    
  } else if (body[iBody].iXUVModel == STELLAR_MODEL_RIBAS) {
    
    // RIBAS power-law decay model
    double dAge = body[iBody].dAge / (1.e9 * YEARSEC);
    double dTMin = body[iBody].dSatXUVTime / (1.e9 * YEARSEC);
    if (dAge >= dTMin){
      body[iBody].dLXUV = body[iBody].dSatXUVFrac * body[iBody].dLuminosity * pow(dAge / dTMin, -body[iBody].dXUVBeta);
    } 
    else {
      /* No evolution at times earlier than dSatXUVTime */
      body[iBody].dLXUV = body[iBody].dSatXUVFrac * body[iBody].dLuminosity;
    }
    
  } else {
    
    // Constant XUV fraction
    body[iBody].dLXUV = body[iBody].dSatXUVFrac * body[iBody].dLuminosity;
    
  }
}

void fnForceBehaviorStellar(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  // Nothing
}

void VerifyStellar(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int bStellar=0;

  /* Stellar is active for this body if this subroutine is called. */
  
  if (update[iBody].iNumLuminosity > 1) {
    if (control->Io.iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Looks like there's more than one equation trying to set dLuminosity for body %d!", iBody);
    exit(EXIT_INPUT);
  }
  VerifyLuminosity(body,control,options,update,body[iBody].dAge,fnUpdate,iBody);
  bStellar = 1;
  
  if (update[iBody].iNumRadius > 1) {
    if (control->Io.iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Looks like there's more than one equation trying to set dRadius for body %d!", iBody);
    exit(EXIT_INPUT);
  }
  VerifyRadius(body,control,options,update,body[iBody].dAge,fnUpdate,iBody);
  
  VerifyRotRate(body,control,options,update,body[iBody].dAge,fnUpdate,iBody);
  
  if (update[iBody].iNumTemperature > 1) {
    if (control->Io.iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Looks like there's more than one equation trying to set dTemperature for body %d!", iBody);
    exit(EXIT_INPUT);
  }
  VerifyTemperature(body,control,options,update,body[iBody].dAge,fnUpdate,iBody);

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorStellar;
  control->fnPropsAux[iBody][iModule] = &fnPropertiesStellar;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyStellar;
  
}

void InitializeModuleStellar(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** STELLAR update ****************/

void InitializeUpdateStellar(BODY *body,UPDATE *update,int iBody) {  
  if (body[iBody].dLuminosity > 0) {
    if (update[iBody].iNumLuminosity == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumLuminosity++;
  }
  
  if (body[iBody].dRadius > 0) {
    if (update[iBody].iNumRadius == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumRadius++;
  }

  // NOTE: Rory and I decided to ALWAYS track the rotation evolution of the star,
  // so I'm not going to check whether dRotRate is zero here. If it is, it gets set
  // to its default value, and we track angular momentum conservation from there.
  if (update[iBody].iNumRot == 0)
    update[iBody].iNumVars++;
  update[iBody].iNumRot++;
  
  if (body[iBody].dTemperature > 0) {
    if (update[iBody].iNumTemperature == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumTemperature++;
  }
}

void FinalizeUpdateEccStellar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  /* Nothing */
}

void FinalizeUpdateLuminosityStellar(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iNumLuminosity = (*iEqn)++;
}
     
void FinalizeUpdateRadiusStellar(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iNumRadius = (*iEqn)++;
}

void FinalizeUpdateRotRateStellar(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iRotStellar = (*iEqn)++;
}

void FinalizeUpdateTemperatureStellar(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = STELLAR;
  update[iBody].iNumTemperature = (*iEqn)++;
}
 
void FinalizeUpdateOblStellar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  /* Nothing */
}

void FinalizeUpdateSemiStellar(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
  /* Nothing */
}


/***************** STELLAR Halts *****************/

int fbHaltEndBaraffeGrid(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  if (body[iBody].iStellarModel == STELLAR_MODEL_CONST) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s reached the edge of the luminosity grid at ", body[iBody].cName);
      fprintd(stdout, body[iBody].dAge/YEARSEC,io->iSciNot,io->iDigits);
      printf(" years.\n");
    }
    return 1;
  }
  return 0;
}

void CountHaltsStellar(HALT *halt,int *iHalt) {
  if (halt->bEndBaraffeGrid)
    (*iHalt)++;
}

void VerifyHaltStellar(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
  if (control->Halt[iBody].bEndBaraffeGrid)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltEndBaraffeGrid;
}

/************* STELLAR Outputs ******************/

void HelpOutputStellar(OUTPUT *output) {
  int iOut;

  printf("\n ------ STELLAR output ------\n");
  for (iOut=OUTSTARTSTELLAR;iOut<OUTENDSTELLAR;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

void WriteHZLimitRecentVenus(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double *dHZLimits;
  
  // Get limits
  dHZLimits = malloc(6*sizeof(double));
  if (body[iBody].iHZModel == HZ_MODEL_KOPPARAPU)
    fdHabitableZoneKopparapu2013(body[iBody].dLuminosity,body[iBody].dTemperature,dHZLimits);

  // Recent Venus limit is index 0
  *dTmp = dHZLimits[0];
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }

  free(dHZLimits);
}
      
void WriteHZLimitRunawayGreenhouse(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double *dHZLimits;
  
  // Get limits
  dHZLimits = malloc(6*sizeof(double));
  if (body[iBody].iHZModel == HZ_MODEL_KOPPARAPU)
    fdHabitableZoneKopparapu2013(body[iBody].dLuminosity,body[iBody].dTemperature,dHZLimits);

  // Runaway greenhouse limit is index 1
  *dTmp = dHZLimits[1];
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }

  free(dHZLimits);
}
      
void WriteHZLimitMoistGreenhouse(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double *dHZLimits;
  
  // Get limits
  dHZLimits = malloc(6*sizeof(double));
  if (body[iBody].iHZModel == HZ_MODEL_KOPPARAPU)
    fdHabitableZoneKopparapu2013(body[iBody].dLuminosity,body[iBody].dTemperature,dHZLimits);

  // Moist greenhouse limit is index 2
  *dTmp = dHZLimits[2];
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }

  free(dHZLimits);
}
      
void WriteHZLimitMaxGreenhouse(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double *dHZLimits;
  
  // Get limits
  dHZLimits = malloc(6*sizeof(double));
  if (body[iBody].iHZModel == HZ_MODEL_KOPPARAPU)
    fdHabitableZoneKopparapu2013(body[iBody].dLuminosity,body[iBody].dTemperature,dHZLimits);

  // Maximum greenhouse limit is index 3
  *dTmp = dHZLimits[3];
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }

  free(dHZLimits);
}

void WriteHZLimitEarlyMars(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  double *dHZLimits;
  
  // Get limits
  dHZLimits = malloc(6*sizeof(double));
  if (body[iBody].iHZModel == HZ_MODEL_KOPPARAPU)
    fdHabitableZoneKopparapu2013(body[iBody].dLuminosity,body[iBody].dTemperature,dHZLimits);

  // Early Mars limit is index 4
  *dTmp = dHZLimits[4];
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }

  free(dHZLimits);
}      
      
void WriteLuminosity(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dLuminosity;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void WriteTemperature(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dTemperature;
  // Kelvin only
  fsUnitsTemp(0,cUnit);
}

void WriteLXUV(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dLXUV;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units,cUnit);
  }

}

void WriteLXUVFrac(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dLXUV / body[iBody].dLuminosity;
  strcpy(cUnit,"");
}

void InitializeOutputStellar(OUTPUT *output,fnWriteOutput fnWrite[]) {
  
  sprintf(output[OUT_HZLIMRECVENUS].cName,"HZLimRecVenus");
  sprintf(output[OUT_HZLIMRECVENUS].cDescr,"Recent Venus Habitable Zone Limit");
  sprintf(output[OUT_HZLIMRECVENUS].cNeg,"RSUN");
  output[OUT_HZLIMRECVENUS].bNeg = 1;
  output[OUT_HZLIMRECVENUS].dNeg = 1./RSUN;
  output[OUT_HZLIMRECVENUS].iNum = 1;
  output[OUT_HZLIMRECVENUS].iModuleBit = STELLAR;
  fnWrite[OUT_HZLIMRECVENUS] = &WriteHZLimitRecentVenus;

  sprintf(output[OUT_HZLIMRUNAWAY].cName,"HZLimRunaway");
  sprintf(output[OUT_HZLIMRUNAWAY].cDescr,"Runaway Greenhouse Habitable Zone Limit");
  sprintf(output[OUT_HZLIMRUNAWAY].cNeg,"RSUN");
  output[OUT_HZLIMRUNAWAY].bNeg = 1;
  output[OUT_HZLIMRUNAWAY].dNeg = 1./RSUN;
  output[OUT_HZLIMRUNAWAY].iNum = 1;
  output[OUT_HZLIMRUNAWAY].iModuleBit = STELLAR;
  fnWrite[OUT_HZLIMRUNAWAY] = &WriteHZLimitRunawayGreenhouse;

  sprintf(output[OUT_HZLIMMOIST].cName,"HZLimMoistGreenhouse");
  sprintf(output[OUT_HZLIMMOIST].cDescr,"Moist Greenhouse Habitable Zone Limit");
  sprintf(output[OUT_HZLIMMOIST].cNeg,"RSUN");
  output[OUT_HZLIMMOIST].bNeg = 1;
  output[OUT_HZLIMMOIST].dNeg = 1./RSUN;
  output[OUT_HZLIMMOIST].iNum = 1;
  output[OUT_HZLIMMOIST].iModuleBit = STELLAR;
  fnWrite[OUT_HZLIMMOIST] = &WriteHZLimitMoistGreenhouse;

  sprintf(output[OUT_HZLIMMAX].cName,"HZLimMaxGreenhouse");
  sprintf(output[OUT_HZLIMMAX].cDescr,"Maximum Greenhouse Habitable Zone Limit");
  sprintf(output[OUT_HZLIMMAX].cNeg,"RSUN");
  output[OUT_HZLIMMAX].bNeg = 1;
  output[OUT_HZLIMMAX].dNeg = 1./RSUN;
  output[OUT_HZLIMMAX].iNum = 1;
  output[OUT_HZLIMMAX].iModuleBit = STELLAR;
  fnWrite[OUT_HZLIMMAX] = &WriteHZLimitMaxGreenhouse;

  sprintf(output[OUT_HZLIMEARLYMARS].cName,"HZLimEarlyMars");
  sprintf(output[OUT_HZLIMEARLYMARS].cDescr,"Early Mars Habitable Zone Limit");
  sprintf(output[OUT_HZLIMEARLYMARS].cNeg,"RSUN");
  output[OUT_HZLIMEARLYMARS].bNeg = 1;
  output[OUT_HZLIMEARLYMARS].dNeg = 1./RSUN;
  output[OUT_HZLIMEARLYMARS].iNum = 1;
  output[OUT_HZLIMEARLYMARS].iModuleBit = STELLAR;
  fnWrite[OUT_HZLIMEARLYMARS] = &WriteHZLimitEarlyMars;
  
  sprintf(output[OUT_LUMINOSITY].cName,"Luminosity");
  sprintf(output[OUT_LUMINOSITY].cDescr,"Luminosity");
  sprintf(output[OUT_LUMINOSITY].cNeg,"LSUN");
  output[OUT_LUMINOSITY].bNeg = 1;
  output[OUT_LUMINOSITY].dNeg = 1./LSUN;
  output[OUT_LUMINOSITY].iNum = 1;
  output[OUT_LUMINOSITY].iModuleBit = STELLAR;
  fnWrite[OUT_LUMINOSITY] = &WriteLuminosity;

  // Maybe change to TEFF? XXX
  sprintf(output[OUT_TEMPERATURE].cName,"Temperature");
  sprintf(output[OUT_TEMPERATURE].cDescr,"Effective Temperature");
  output[OUT_TEMPERATURE].bNeg = 0;
  output[OUT_TEMPERATURE].iNum = 1;
  output[OUT_TEMPERATURE].iModuleBit = STELLAR;
  fnWrite[OUT_TEMPERATURE] = &WriteTemperature;
  
  sprintf(output[OUT_LXUV].cName,"LXUVStellar");
  sprintf(output[OUT_LXUV].cDescr,"Base X-ray/XUV Luminosity");
  sprintf(output[OUT_LXUV].cNeg,"LSUN");
  output[OUT_LXUV].bNeg = 1;
  output[OUT_LXUV].dNeg = 1./LSUN;
  output[OUT_LXUV].iNum = 1;
  output[OUT_LXUV].iModuleBit = STELLAR;
  fnWrite[OUT_LXUV] = &WriteLXUV;

  sprintf(output[OUT_LXUVFRAC].cName,"LXUVFrac");
  sprintf(output[OUT_LXUVFRAC].cDescr,"X-ray/XUV Luminosity Fraction");
  output[OUT_LXUVFRAC].bNeg = 0;
  output[OUT_LXUVFRAC].iNum = 1;
  output[OUT_LXUVFRAC].iModuleBit = STELLAR;
  fnWrite[OUT_LXUVFRAC] = &WriteLXUVFrac;
}

/************ STELLAR Logging Functions **************/

void LogOptionsStellar(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- STELLAR Options -----\n\n");
  */
}

void LogStellar(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {

  /* Anything here? 
  int iOut;

  fprintf(fp,"\n----- STELLAR PARAMETERS ------\n");
  for (iOut=OUTSTARTSTELLAR;iOut<OUTBODYSTARTSTELLAR;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyStellar(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;  
  fprintf(fp,"----- STELLAR PARAMETERS (%s)------\n",body[iBody].cName);
  
  for (iOut=OUTSTARTSTELLAR;iOut<OUTENDSTELLAR;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

void AddModuleStellar(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = STELLAR;

  module->fnCountHalts[iBody][iModule] = &CountHaltsStellar;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsStellar;
  module->fnLogBody[iBody][iModule] = &LogBodyStellar;
  module->fnVerify[iBody][iModule] = &VerifyStellar;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltStellar;

  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateStellar;
  module->fnFinalizeUpdateLuminosity[iBody][iModule] = &FinalizeUpdateLuminosityStellar;
  module->fnFinalizeUpdateRadius[iBody][iModule] = &FinalizeUpdateRadiusStellar;
  module->fnFinalizeUpdateRot[iBody][iModule] = &FinalizeUpdateRotRateStellar;
  module->fnFinalizeUpdateTemperature[iBody][iModule] = &FinalizeUpdateTemperatureStellar;
}

/************* STELLAR Functions ************/

double fdLuminosity(BODY *body,SYSTEM *system,int *iaBody) {
  double foo;
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    foo = fdLuminosityFunctionBaraffe(body[iaBody[0]].dAge, body[iaBody[0]].dMass);
    if (!isnan(foo)) return foo;
    else body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
  } else if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    foo = fdLuminosityFunctionProximaCen(body[iaBody[0]].dAge,body[iaBody[0]].dMass);
    if (!isnan(foo)) return foo;
    else body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
  }
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_NONE || body[iaBody[0]].iStellarModel == STELLAR_MODEL_CONST)
    return body[iaBody[0]].dLuminosity;
  else
    return 0;
}

double fdRadius(BODY *body,SYSTEM *system,int *iaBody) {
  double foo;
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    foo = fdRadiusFunctionBaraffe(body[iaBody[0]].dAge, body[iaBody[0]].dMass);
    if (!isnan(foo)) return foo;
    else body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
  } else if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    foo = fdRadiusFunctionProximaCen(body[iaBody[0]].dAge,body[iaBody[0]].dMass);
    if (!isnan(foo)) return foo;
    else body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
  }
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_NONE || body[iaBody[0]].iStellarModel == STELLAR_MODEL_CONST)
    return body[iaBody[0]].dRadius;
  else
    return 0;
}

double fdDRotRateDt(BODY *body,SYSTEM *system,int *iaBody) {
  
  double dDRadiusDt = 0;
  double dDJDt = 0;
  double dOmegaCrit;
  
  // First, let's calculate dR/dt due to contraction/expansion
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    // Compute a very simple derivative. NOTE: This won't work if variables like the
    // stellar mass are changing, too! Perhaps it's better to keep track of the previous
    // values of the radius and compute the derivative from those? TODO: Check this.
   
    // Delta t = 10 years. TODO: Check this.
    double eps = 10 * YEARDAY * DAYSEC;
    double dRadMinus, dRadPlus;
    
    dRadMinus = fdRadiusFunctionBaraffe(body[iaBody[0]].dAge - eps, body[iaBody[0]].dMass);
    dRadPlus = fdRadiusFunctionBaraffe(body[iaBody[0]].dAge + eps, body[iaBody[0]].dMass);
    dDRadiusDt = (dRadPlus - dRadMinus) /  (2. * eps);
  
  }
  
  // Now, let's calculate dJ/dt due to magnetic braking
  // This is from Reiners & Mohanty (2012); see eqn. (2.14) in Miles Timpe's Master's Thesis
  if (body[iaBody[0]].dMass > 0.35 * MSUN) dOmegaCrit = RM12OMEGACRIT;
  else dOmegaCrit = RM12OMEGACRITFULLYCONVEC;
  if (body[iaBody[0]].iWindModel == STELLAR_MODEL_REINERS) {    
    if (body[iaBody[0]].dRotRate >= dOmegaCrit) {
      dDJDt = -RM12CONST * body[iaBody[0]].dRotRate * pow(body[iaBody[0]].dRadius, 16. / 3.) 
                        * pow(body[iaBody[0]].dMass, -2. / 3);
    } else {
      dDJDt = -RM12CONST * pow(body[iaBody[0]].dRotRate / dOmegaCrit, 4.) * body[iaBody[0]].dRotRate
                        * pow(body[iaBody[0]].dRadius, 16. / 3.) * pow(body[iaBody[0]].dMass, -2. / 3);
    }
  }
  
  // Note that we force dRotRate/dt = 0 in the first 1e6 years, since the stellar rotation
  // is likely locked to the disk rotation (Kevin Covey's suggestion).
  //
  // The equation below comes from conservation of angular momentum:
  // 
  // dw/dt = d(I/J)/dt = (1/I) * dJ/dt - (J/I^2) * dI/dt
  //
  // where dJ/dt is due to winds and dI/dt is due to contraction.
  if (body[iaBody[0]].dAge >= 1.e6 * YEARSEC) {
    return dDJDt / (body[iaBody[0]].dRadGyra * body[iaBody[0]].dMass * body[iaBody[0]].dRadius * body[iaBody[0]].dRadius) 
               - 2 * body[iaBody[0]].dRotRate / body[iaBody[0]].dRadius * dDRadiusDt;
  } else {
    return 0.;
  }
}

double fdTemperature(BODY *body,SYSTEM *system,int *iaBody) {
  double foo;
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_BARAFFE) {
    foo = fdTemperatureFunctionBaraffe(body[iaBody[0]].dAge, body[iaBody[0]].dMass);
    if (!isnan(foo)) return foo;
    else body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
  } else if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_PROXIMACEN) {
    foo = fdTemperatureFunctionProximaCen(body[iaBody[0]].dAge,body[iaBody[0]].dMass);
    if (!isnan(foo)) return foo;
    else body[iaBody[0]].iStellarModel = STELLAR_MODEL_CONST;
  }
  if (body[iaBody[0]].iStellarModel == STELLAR_MODEL_NONE || body[iaBody[0]].iStellarModel == STELLAR_MODEL_CONST)
    return body[iaBody[0]].dTemperature;
  else
    return 0;
}

double fdLuminosityFunctionBaraffe(double dAge, double dMass) {
  int iError;
  double L = fdBaraffe(STELLAR_L, dAge, dMass, 3, &iError);
  if ((iError == STELLAR_ERR_NONE) || (iError == STELLAR_ERR_LINEAR))
    return L;
  else if (iError == STELLAR_ERR_OUTOFBOUNDS_HI || iError == STELLAR_ERR_ISNAN)
    return NAN;
  else {
    if (iError == STELLAR_ERR_OUTOFBOUNDS_LO)
      fprintf(stderr,"ERROR: Out of bounds (low) in fdBaraffe().\n");
    else if (iError == STELLAR_ERR_FILE)
      fprintf(stderr,"ERROR: File access error in routine fdBaraffe().\n");
    else if (iError == STELLAR_ERR_BADORDER)
      fprintf(stderr,"ERROR: Bad interpolation order in routine fdBaraffe().\n");
    else
      fprintf(stderr,"ERROR: Undefined error in fdBaraffe().\n");
    exit(EXIT_INT);
  }
}

double fdRadiusFunctionBaraffe(double dAge, double dMass) {
  int iError;
  double R = fdBaraffe(STELLAR_R, dAge, dMass, 3, &iError);
  if ((iError == STELLAR_ERR_NONE) || (iError == STELLAR_ERR_LINEAR))
    return R;
  else if (iError == STELLAR_ERR_OUTOFBOUNDS_HI || iError == STELLAR_ERR_ISNAN)
    return NAN;
  else {
    if (iError == STELLAR_ERR_OUTOFBOUNDS_LO)
      fprintf(stderr,"ERROR: Out of bounds (low) in fdBaraffe().\n");
    else if (iError == STELLAR_ERR_FILE)
      fprintf(stderr,"ERROR: File access error in routine fdBaraffe().\n");
    else if (iError == STELLAR_ERR_BADORDER)
      fprintf(stderr,"ERROR: Bad interpolation order in routine fdBaraffe().\n");
    else
      fprintf(stderr,"ERROR: Undefined error in fdBaraffe().\n");
    exit(EXIT_INT);
  }
}

double fdTemperatureFunctionBaraffe(double dAge, double dMass) {
  int iError;
  double T = fdBaraffe(STELLAR_T, dAge, dMass, 3, &iError);
  if ((iError == STELLAR_ERR_NONE) || (iError == STELLAR_ERR_LINEAR))
    return T;
  else if (iError == STELLAR_ERR_OUTOFBOUNDS_HI || iError == STELLAR_ERR_ISNAN)
    return NAN;
  else {
    if (iError == STELLAR_ERR_OUTOFBOUNDS_LO)
      fprintf(stderr,"ERROR: Out of bounds (low) in fdBaraffe().\n");
    else if (iError == STELLAR_ERR_FILE)
      fprintf(stderr,"ERROR: File access error in routine fdBaraffe().\n");
    else if (iError == STELLAR_ERR_BADORDER)
      fprintf(stderr,"ERROR: Bad interpolation order in routine fdBaraffe().\n");
    else
      fprintf(stderr,"ERROR: Undefined error in fdBaraffe().\n");
    exit(EXIT_INT);
  }
}

double fdLuminosityFunctionProximaCen(double dAge, double dMass) {
  int iError;
  double L = fdProximaCenStellar(PROXIMACEN_L, dAge, dMass, &iError);
  if (iError == PROXIMACEN_ERROR)
    return NAN;
  else {
    return L;
  }
}

double fdTemperatureFunctionProximaCen(double dAge, double dMass) {
  int iError;
  double L = fdProximaCenStellar(PROXIMACEN_T, dAge, dMass, &iError);
  if (iError == PROXIMACEN_ERROR)
    return NAN;
  else {
    return L;
  }
}

double fdRadiusFunctionProximaCen(double dAge, double dMass) {
  int iError;
  double L = fdProximaCenStellar(PROXIMACEN_R, dAge, dMass, &iError);
  if (iError == PROXIMACEN_ERROR)
    return NAN;
  else {
    return L;
  }
}

double fdSurfEnFluxStellar(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  // This is silly, but necessary!
// RORY: I don't think so! -- This function should be set to ReturnOutputZero
  return 0;
}

// Habitable zone limits from Kopparapu et al. (2013)
void fdHabitableZoneKopparapu2013(double dLuminosity,double dTeff,double *daHZLimit) {
  int i;
  double tstar,seff[6];
  double seffsun[6],a[6],b[6],c[6],d[6];

  // Luminosity must be expressed in solar units
  dLuminosity /= LSUN;
  
  seffsun[0] = 1.7763;
  seffsun[1] = 1.0385;
  seffsun[2] = 1.0146;
  seffsun[3] = 0.3507;
  seffsun[4] = 0.2946;
  seffsun[5] = 0.2484;

  a[0] = 1.4335e-4;
  a[1] = 1.2456e-4;
  a[2] = 8.1884e-5;
  a[3] = 5.9578e-5;
  a[4] = 4.9952e-5;
  a[5] = 4.2588e-5;

  b[0] = 3.3954e-9; 
  b[1] = 1.4612e-8;
  b[2] = 1.9394e-9;
  b[3] = 1.6707e-9;
  b[4] = 1.3893e-9;
  b[5] = 1.1963e-9;

  c[0] = -7.6364e-12;
  c[1] = -7.6345e-12;
  c[2] = -4.3618e-12;
  c[3] = -3.0058e-12;
  c[4] = -2.5331e-12;
  c[5] = -2.1709e-12;

  d[0] = -1.1950e-15;
  d[1] = -1.7511E-15;
  d[2] = -6.8260e-16;
  d[3] = -5.1925e-16;
  d[4] = -4.3896e-16;
  d[5] = -3.8282e-16;
 
  tstar = dTeff - 5700;

  for (i=0;i<6;i++) {
    seff[i] = seffsun[i] + a[i]*tstar + b[i]*tstar*tstar + 
c[i]*pow(tstar,3) + d[i]*pow(tstar,4);
    // Limits are in AU, convert to meters
    daHZLimit[i] = pow(dLuminosity/seff[i],0.5)*AUCM;
  }
}



