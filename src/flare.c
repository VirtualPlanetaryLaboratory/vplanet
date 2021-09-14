/**
  @file flare.c

  @brief Subroutines that control the output of XUV flares.

  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

  @date Mar 25 2016

*/

#include "vplanet.h"
#include <stdio.h>
#include <math.h>

void BodyCopyFlare(BODY *dest,BODY *src,int foo,int iNumBodies,int iBody) {
  dest[iBody].dFlareMinEnergy = src[iBody].dFlareMinEnergy;
  dest[iBody].dFlareMaxEnergy = src[iBody].dFlareMaxEnergy;
  dest[iBody].dDeltat = src[iBody].dDeltat;
  dest[iBody].dLXUVFlare = src[iBody].dLXUVFlare;
  dest[iBody].dLXUVFlareUpper = src[iBody].dLXUVFlareUpper;
  dest[iBody].dLXUVFlareLower = src[iBody].dLXUVFlareLower;    
  dest[iBody].iFlareFFD = src[iBody].iFlareFFD;
  dest[iBody].iFlareActivity = src[iBody].iFlareActivity;
  dest[iBody].iFlareCalcLuminosity = src[iBody].iFlareCalcLuminosity;     
  dest[iBody].iFlareBandPass = src[iBody].iFlareBandPass;  
  dest[iBody].iFlareSlopeUnits = src[iBody].iFlareSlopeUnits;
  dest[iBody].dEnergyBin = src[iBody].dEnergyBin;
  dest[iBody].dFlareSlope = src[iBody].dFlareSlope;
  dest[iBody].dFlareYInt = src[iBody].dFlareYInt;
}  

/**************** FLARE options ********************/

void ReadFlareActivity(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"ac",2)) {
      body[iFile-1].iFlareActivity = FLARE_ACTIVE;
    } else if (!memcmp(sLower(cTmp),"in",2)) {
      body[iFile-1].iFlareActivity  = FLARE_INACTIVE;
    } else if (!memcmp(sLower(cTmp),"co",2)) {
      body[iFile-1].iFlareActivity  = FLARE_CONSTANT;
    }else if (!memcmp(sLower(cTmp),"al",2)) {
      body[iFile-1].iFlareActivity  = FLARE_ALLMDWARFS;
    }else if (!memcmp(sLower(cTmp),"ti",2)) {
      body[iFile-1].iFlareActivity  = FLARE_TILLEY;
    }
     else {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are ACTIVE, INACTIVE, ALLMDWARFS, TILLEY and CONSTANT.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].iFlareActivity  = FLARE_CONSTANT;
}


void ReadFlareEnergyBin(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // This parameter cannot exist in primary file
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dEnergyBin = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dEnergyBin = options->dDefault;
}

void ReadFlareDeltat(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // This parameter cannot exist in primary file
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dDeltat = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dDeltat = options->dDefault;
}


void ReadFlareFFD(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"da",2)) {
      body[iFile-1].iFlareFFD = FLARE_FFD_DAVENPORT;
    } else if (!memcmp(sLower(cTmp),"la",2)) {
      body[iFile-1].iFlareFFD  = FLARE_FFD_LACY;
    } else if (!memcmp(sLower(cTmp),"ob",2)) {
      body[iFile-1].iFlareFFD  = FLARE_FFD_OBSERV;
    } else {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are DAVENPORT, LACY, or OBSERV.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].iFlareFFD  = FLARE_FFD_DAVENPORT;
}

void ReadFlareSlopeUnits(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"se",2)) {
      body[iFile-1].iFlareSlopeUnits = FLARE_SLOPE_SEC;
    } else if (!memcmp(sLower(cTmp),"mi",2)) {
      body[iFile-1].iFlareSlopeUnits  = FLARE_SLOPE_MINUTE;
    } else if (!memcmp(sLower(cTmp),"ho",2)) {
      body[iFile-1].iFlareSlopeUnits  = FLARE_SLOPE_HOUR;
    } else if (!memcmp(sLower(cTmp),"da",2)) {
      body[iFile-1].iFlareSlopeUnits  = FLARE_SLOPE_DAY;
    } else {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are SEC, MIN, HOUR or DAY.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].iFlareSlopeUnits  = FLARE_SLOPE_DAY;
}

void ReadFlareBandPass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (!memcmp(sLower(cTmp),"ke",2)) {
      body[iFile-1].iFlareBandPass = FLARE_KEPLER;
    } else if (!memcmp(sLower(cTmp),"uv",2)) {
      body[iFile-1].iFlareBandPass  = FLARE_UV;
    } else if (!memcmp(sLower(cTmp),"go",2)) {
      body[iFile-1].iFlareBandPass  = FLARE_GOES;
    } else if (!memcmp(sLower(cTmp),"sr",2)) {
      body[iFile-1].iFlareBandPass  = FLARE_SXR;
    } else if (!memcmp(sLower(cTmp),"te",2)) {
      body[iFile-1].iFlareBandPass  = FLARE_TESS_UV;
    } else if (!memcmp(sLower(cTmp),"bo",2)) {
      body[iFile-1].iFlareBandPass  = FLARE_BOLOMETRIC;
    } else {
      if (control->Io.iVerbose >= VERBERR)
	      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are KEPLER, UV, GOES, TESSUV, BOLOMETRIC and SXR.\n",options->cName,cTmp);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].iFlareBandPass  = FLARE_KEPLER;
}

void ReadFlareMinEnergy(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // This parameter cannot exist in primary file
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dFlareMinEnergy = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dFlareMinEnergy = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFlareMinEnergy = options->dDefault;
}

void ReadFlareMaxEnergy(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
    // This parameter cannot exist in primary file
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dFlareMaxEnergy = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dFlareMaxEnergy = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFlareMaxEnergy = options->dDefault;
}

void ReadFlareYInt(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // This parameter cannot exist in primary file
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dFlareYInt = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFlareYInt = options->dDefault;
}

void ReadFlareYIntErrorUpper(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // This parameter cannot exist in primary file
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dFlareYIntErrorUpper = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFlareYIntErrorUpper = options->dDefault;
}

void ReadFlareYIntErrorLower(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // This parameter cannot exist in primary file
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dFlareYIntErrorLower = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFlareYIntErrorLower = options->dDefault;
}

void ReadFlareSlope(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // This parameter cannot exist in primary file
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dFlareSlope = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFlareSlope = options->dDefault;
}

void ReadFlareSlopeErrorUpper(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // This parameter cannot exist in primary file
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dFlareSlopeErrorUpper = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFlareSlopeErrorUpper = options->dDefault;
}

void ReadFlareSlopeErrorLower(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // This parameter cannot exist in primary file
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    body[iFile-1].dFlareSlopeErrorLower = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFlareSlopeErrorLower = options->dDefault;
}

/* Initiatlize Input Options */

void InitializeOptionsFlare(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  sprintf(options[OPT_FLAREYINT].cName,"dFlareYInt");
  sprintf(options[OPT_FLAREYINT].cDescr,"Y-Intercept for Flare Frequency"); // Where the curve intercepts the y axis (y=ax+b, in this case, the parameter its "b")
  sprintf(options[OPT_FLAREYINT].cDefault,"20.9 (Proxima)");
  options[OPT_FLAREYINT].dDefault = 20.9;
  options[OPT_FLAREYINT].iType = 2;
  options[OPT_FLAREYINT].bMultiFile = 1;
  fnRead[OPT_FLAREYINT] = &ReadFlareYInt;

  sprintf(options[OPT_FLAREYINTERRORUPPER].cName,"dFlareYIntErrorUpper");
  sprintf(options[OPT_FLAREYINTERRORUPPER].cDescr,"Y-Intercept upper error"); 
  sprintf(options[OPT_FLAREYINTERRORUPPER].cDefault,"0.0");
  options[OPT_FLAREYINTERRORUPPER].dDefault = 0.0;
  options[OPT_FLAREYINTERRORUPPER].iType = 2;
  options[OPT_FLAREYINTERRORUPPER].bMultiFile = 1;
  fnRead[OPT_FLAREYINTERRORUPPER] = &ReadFlareYIntErrorUpper;

  sprintf(options[OPT_FLAREYINTERRORLOWER].cName,"dFlareYIntErrorLower");
  sprintf(options[OPT_FLAREYINTERRORLOWER].cDescr,"Y-Intercept lower error"); 
  sprintf(options[OPT_FLAREYINTERRORLOWER].cDefault,"0.0");
  options[OPT_FLAREYINTERRORLOWER].dDefault = 0.0;
  options[OPT_FLAREYINTERRORLOWER].iType = 2;
  options[OPT_FLAREYINTERRORLOWER].bMultiFile = 1;
  fnRead[OPT_FLAREYINTERRORLOWER] = &ReadFlareYIntErrorLower;

  sprintf(options[OPT_FLARESLOPE].cName,"dFlareSlope");
  sprintf(options[OPT_FLARESLOPE].cDescr,"Slope for Flare Frequency");
  sprintf(options[OPT_FLARESLOPE].cDefault,"-0.68 (Proxima)");
  options[OPT_FLARESLOPE].dDefault = -0.68;
  options[OPT_FLARESLOPE].iType = 2;
  options[OPT_FLARESLOPE].bMultiFile = 1;
  fnRead[OPT_FLARESLOPE] = &ReadFlareSlope;

  sprintf(options[OPT_FLARESLOPEERRORUPPER].cName,"dFlareSlopeErrorUpper");
  sprintf(options[OPT_FLARESLOPEERRORUPPER].cDescr,"Slope upper error"); 
  sprintf(options[OPT_FLARESLOPEERRORUPPER].cDefault,"0.0");
  options[OPT_FLARESLOPEERRORUPPER].dDefault = 0.0;
  options[OPT_FLARESLOPEERRORUPPER].iType = 2;
  options[OPT_FLARESLOPEERRORUPPER].bMultiFile = 1;
  fnRead[OPT_FLARESLOPEERRORUPPER] = &ReadFlareSlopeErrorUpper;

  sprintf(options[OPT_FLARESLOPEERRORLOWER].cName,"dFlareSlopeErrorLower");
  sprintf(options[OPT_FLARESLOPEERRORLOWER].cDescr,"Slope lower error"); 
  sprintf(options[OPT_FLARESLOPEERRORLOWER].cDefault,"0.0");
  options[OPT_FLARESLOPEERRORLOWER].dDefault = 0.0;
  options[OPT_FLARESLOPEERRORLOWER].iType = 2;
  options[OPT_FLARESLOPEERRORLOWER].bMultiFile = 1;
  fnRead[OPT_FLARESLOPEERRORLOWER] = &ReadFlareSlopeErrorLower;

  sprintf(options[OPT_FLAREMINENERGY].cName,"dFlareMinEnergy");
  sprintf(options[OPT_FLAREMINENERGY].cDescr,"Minimum Flare Energy to Consider");
  sprintf(options[OPT_FLAREMINENERGY].cDefault,"10^26 J ou 10^33 ergs");
  options[OPT_FLAREMINENERGY].dDefault = 1e26;
  options[OPT_FLAREMINENERGY].iType = 2;
  options[OPT_FLAREMINENERGY].bMultiFile = 1;
  options[OPT_FLAREMINENERGY].dNeg = 1e7;
  sprintf(options[OPT_FLAREMINENERGY].cNeg,"ergs");
  fnRead[OPT_FLAREMINENERGY] = &ReadFlareMinEnergy;

  sprintf(options[OPT_FLAREMAXENERGY].cName,"dFlareMaxEnergy");
  sprintf(options[OPT_FLAREMAXENERGY].cDescr,"Maximum Flare Energy to Consider");
  sprintf(options[OPT_FLAREMAXENERGY].cDefault,"10^29 J or 10^36 ergs");
  options[OPT_FLAREMAXENERGY].dDefault = 1e29;
  options[OPT_FLAREMAXENERGY].iType = 2;
  options[OPT_FLAREMAXENERGY].bMultiFile = 1;
  options[OPT_FLAREMAXENERGY].dNeg = 1e7;
  sprintf(options[OPT_FLAREMAXENERGY].cNeg,"ergs");
  fnRead[OPT_FLAREMAXENERGY] = &ReadFlareMaxEnergy;

  sprintf(options[OPT_FLAREENERGYBIN].cName,"dEnergyBin");
  sprintf(options[OPT_FLAREENERGYBIN].cDescr,"Number of energies consider between the minimum and maximum energies to calculate the luminosity by flares");
  sprintf(options[OPT_FLAREENERGYBIN].cDefault,"100 energies between dFlareMinEnergy and dFlareMaxEnergy");
  options[OPT_FLAREENERGYBIN].dDefault = 100.0;
  options[OPT_FLAREENERGYBIN].iType = 2;
  options[OPT_FLAREENERGYBIN].bMultiFile = 1;
  options[OPT_FLAREENERGYBIN].dNeg = 1;
  sprintf(options[OPT_FLAREENERGYBIN].cNeg,"None");
  fnRead[OPT_FLAREENERGYBIN] = &ReadFlareEnergyBin;

  sprintf(options[OPT_FLAREDELTAT].cName,"dDeltat");
  sprintf(options[OPT_FLAREDELTAT].cDescr,"Time duration of Flare");
  sprintf(options[OPT_FLAREDELTAT].cDefault,"1500 seconds");
  options[OPT_FLAREDELTAT].dDefault = 1500.0;
  options[OPT_FLAREDELTAT].iType = 2;
  options[OPT_FLAREDELTAT].bMultiFile = 1;
  options[OPT_FLAREDELTAT].dNeg = 1;
  sprintf(options[OPT_FLAREDELTAT].cNeg,"seconds");
  fnRead[OPT_FLAREDELTAT] = &ReadFlareDeltat;

  sprintf(options[OPT_FLAREACTIVITY].cName,"sFlareActivity");
  sprintf(options[OPT_FLAREACTIVITY].cDescr,"Ways of calculate the time duration of flares");
  sprintf(options[OPT_FLAREACTIVITY].cDefault,"CONSTANT");
  sprintf(options[OPT_FLAREACTIVITY].cValues,"ACTIVE INACTIVE CONSTANT ALLMDWARFS TILLEY");
  options[OPT_FLAREACTIVITY].iType = 3;
  options[OPT_FLAREACTIVITY].bMultiFile = 1;
  fnRead[OPT_FLAREACTIVITY] = &ReadFlareActivity;
  sprintf(options[OPT_FLAREACTIVITY].cLongDescr,
    " If ACTIVE is selected, the code will employ the model 'Active M stars' from the \n"
    "figure 3 of Loyd et al. 2018 (2018ApJ...867...71L) to calculates the time duration \n"
    "of the flares. If INACTIVE is selected, the code will employ the model 'Inactive M stars'\n"
    "from the figure 3 of Loyd et al. 2018 (2018ApJ...867...71L). If ALLMDWARFS is selected,\n"
    "the code will employ the model 'All M stars' from the figure 3 of \n"
    "Loyd et al. 2018 (2018ApJ...867...71L). If TILLEY is selected, the code will employ the\n"
    "equation 3 from Tilley et al. 2019 (https://doi.org/10.1089/ast.2017.1794).\n"
    "If the CONSTANT is selected, the user have \n"
    "to give the time duration of the flares dDeltat, that are in seconds.\n"
  );

  sprintf(options[OPT_FLAREFFD].cName,"sFlareFFD");
  sprintf(options[OPT_FLAREFFD].cDescr,"Modes of calculate the FFD");
  sprintf(options[OPT_FLAREFFD].cDefault,"DAVENPORT");
  sprintf(options[OPT_FLAREFFD].cValues,"DAVENPORT LACY OBSERV");
  options[OPT_FLAREFFD].iType = 3;
  options[OPT_FLAREFFD].bMultiFile = 1;
  fnRead[OPT_FLAREFFD] = &ReadFlareFFD;
  sprintf(options[OPT_FLAREFFD].cLongDescr,
    " If DAVENPORT is selected, the code will employ the model from Davenport et al.\n"
    "(2019ApJ...871..241D) the user have to give the mass and Stellar age, dMass \n"
    "and dAge, as well the maximum and minimum energy, dFlareMinEnergy and \n"
    "dFlareMaxEnergy, to calculate the linear and angular coeficients, dA and dB of\n"
    "the flare frequency distribution (FFD), and then the FFD in this energy range.\n"
    "If Lacy is selected, the code will employ the model of Lacy et al. (1976ApJS...30...85L),\n"
    "and the user have to give the same input of the DAVENPORT's mode, plus the \n"
    "linear and angular coeficients, dFlareSlope and dFlareYInt of the FFD. If OBSERV is selected,\n"
    "the code will calculates the XUV luminosity by flares using the two arrays of\n"
    "energy (daOBSEnergy) and flare rate (daOBSFlareRate).\n"
  );
/*
  sprintf(options[OPT_FLAREOBSFREQ].cName,"daOBSFlareRate");
  sprintf(options[OPT_FLAREOBSFREQ].cDescr,"Array of observed values of flare rate");
  sprintf(options[OPT_FLAREOBSFREQ].cDefault,"None");
  options[OPT_FLAREOBSFREQ].iType = 14;
  options[OPT_FLAREOBSFREQ].iModuleBit = 0;
  options[OPT_FLAREOBSFREQ].bNeg = 0;
  options[OPT_FLAREOBSFREQ].iFileType = 1;
  options[OPT_FLAREOBSFREQ].bMultiFile = 1;


  sprintf(options[OPT_FLAREOBSENERGY].cName,"daOBSEnergy");
  sprintf(options[OPT_FLAREOBSENERGY].cDescr,"Array of observed energies");
  sprintf(options[OPT_FLAREOBSENERGY].cDefault,"None");
  options[OPT_FLAREOBSENERGY].iType = 14;
  options[OPT_FLAREOBSENERGY].iModuleBit = FLARE;
  options[OPT_FLAREOBSENERGY].bNeg = 0;
  options[OPT_FLAREOBSENERGY].iFileType = 1;
  options[OPT_FLAREOBSENERGY].bMultiFile = 1;


  sprintf(options[OPT_GRIDOUTPUT].cName,"saGridOutput");
  sprintf(options[OPT_GRIDOUTPUT].cDescr,"Gridded Output Parameter(s)");
  sprintf(options[OPT_GRIDOUTPUT].cDefault,"None");
  options[OPT_GRIDOUTPUT].iType = 14;
  options[OPT_GRIDOUTPUT].iModuleBit = POISE;
  options[OPT_GRIDOUTPUT].bNeg = 0;
  options[OPT_GRIDOUTPUT].bMultiFile = 1;
  options[OPT_GRIDOUTPUT].iFileType = 1;*/

  sprintf(options[OPT_FLAREBANDPASS].cName,"sFlareBandPass");
  sprintf(options[OPT_FLAREBANDPASS].cDescr,"Options of band pass of the input energy of flares");
  sprintf(options[OPT_FLAREBANDPASS].cDefault,"KEPLER");
  sprintf(options[OPT_FLAREBANDPASS].cValues,"KEPLER UV GOES SXR BOLOMETRIC TESSUV");
  options[OPT_FLAREBANDPASS].iType = 3;
  options[OPT_FLAREBANDPASS].bMultiFile = 1;
  fnRead[OPT_FLAREBANDPASS] = &ReadFlareBandPass;
  sprintf(options[OPT_FLAREBANDPASS].cLongDescr,
    " If UV or GOES is selected, the code will convert the input energy of flares\n"
    "from the UV band (3000–4300 Å) or GOES band (1-8 Å) to kepler band (4000–9000 Å) to calculate.\n"
    " the FFD and to the SXR band (1.24 - 1239.85 Å) to calculate the luminosity.\n"
    " If SXR is selected, the code will convert the input energy of flares from the SXR band to kepler band \n"
    "to calculate the FFD and will use the same value of energy to calculate the luminosity .\n"
    " If KEPLER is selected, the code will use the same input energy to calculate the FFD\n"
    "and will convert the input energy to SXR band to calculate the luminosity.\n"
    "The convertion values are taken from Osten and Wolk (2015) (doi:10.1088/0004-637X/809/1/79).\n"
    "If the TESSUV its selected, the code will convert the input energy of flares\n"
    "using the conversion value to the band U (2000-2800 Å) to the TESS data, from \n"
    " Gunther et al 2020 (https://doi.org/10.3847/1538-3881/ab5d3a). \n"
    "If the BOLOMETRIC its selected, the code will convert the input energy of flares\n"
    "using the convertion values are taken from Osten and Wolk (2015) (doi:10.1088/0004-637X/809/1/79).\n"
    
    
  );

  sprintf(options[OPT_FLARESLOPEUNITS].cName,"sFlareSlopeUnits");
  sprintf(options[OPT_FLARESLOPEUNITS].cDescr,"Options to the units of the FFD in which the FFD slopes are define");
  sprintf(options[OPT_FLARESLOPEUNITS].cDefault,"DAY");
  sprintf(options[OPT_FLARESLOPEUNITS].cValues,"SEC MIN HOUR DAY");
  options[OPT_FLARESLOPEUNITS].iType = 3;
  options[OPT_FLARESLOPEUNITS].bMultiFile = 1;
  fnRead[OPT_FLARESLOPEUNITS] = &ReadFlareSlopeUnits;
  sprintf(options[OPT_FLARESLOPEUNITS].cLongDescr,
    "TENHO QUE PREENCHER ISSO \n"    
    
    
  );
  /*sprintf(options[OPT_NUMYEARS].cName,"iNumFlareBins");
  sprintf(options[OPT_NUMYEARS].cDescr,"Number of flares observed input in the module");
  sprintf(options[OPT_NUMYEARS].cDefault,"2");
  options[OPT_NUMYEARS].dDefault = 2;
  options[OPT_NUMYEARS].iType = 1;
  options[OPT_NUMYEARS].bMultiFile = 1;
  fnRead[OPT_NUMYEARS] = &ReadNumFlareBins;
  
  
  sprintf(options[OPT_FLAREOBSFREQ].cName,"daOBSFlareRate");
  sprintf(options[OPT_FLAREOBSFREQ].cDescr,"Array of the observed Flare rate");
  sprintf(options[OPT_FLAREOBSFREQ].cDefault,"Flares per sec");
  options[OPT_FLAREOBSFREQ].dDefault = {1.0e26,1.0e27,1.0e28,1.0e29,1.0e30,1.0e31,1.0e32,1.0e33,1.0e34,1.0e35,1.0e36};
  options[OPT_FLAREOBSFREQ].iType = 11;
  options[OPT_FLAREOBSFREQ].bMultiFile = 1;
  options[OPT_FLAREOBSFREQ].dNeg = {1.0e7,1.0e7,1.0e7,1.0e7,1.0e7,1.0e7,1.0e7,1.0e7,1.0e7,1.0e7,1.0e7};
  sprintf(options[OPT_FLAREOBSFREQ].cNeg,"ergs");
  fnRead[OPT_FLAREOBSFREQ] = &ReadOBSFlareRate;

  sprintf(options[OPT_FLAREOBSENERGY].cName,"daOBSFlareEnergy");
  sprintf(options[OPT_FLAREOBSENERGY].cDescr,"Array of the observed Flare Energy");
  sprintf(options[OPT_FLAREOBSENERGY].cDefault,"10^26 J or 10^33 ergs and 10^29 J or 10^36 ergs");
  options[OPT_FLAREOBSENERGY].dDefault = {1.0e26,1.0e27,1.0e28,1.0e29,1.0e30,1.0e31,1.0e32,1.0e33,1.0e34,1.0e35,1.0e36};
  options[OPT_FLAREOBSENERGY].iType = 11;
  options[OPT_FLAREOBSENERGY].bMultiFile = 1;
  options[OPT_FLAREOBSENERGY].dNeg = {1.0e7,1.0e7,1.0e7,1.0e7,1.0e7,1.0e7,1.0e7,1.0e7,1.0e7,1.0e7,1.0e7};
  sprintf(options[OPT_FLAREOBSENERGY].cNeg,"ergs");
  fnRead[OPT_FLAREOBSENERGY] = &ReadOBSFlareEnergy;
  
*/
}

void ReadOptionsFlare(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTFLARE;iOpt<OPTENDFLARE;iOpt++) {
    if (options[iOpt].iType != -1)
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}

/******************* Verify FLARE ******************/

void PropsAuxFlare(BODY *body,EVOLVE *evolve,IO *io,UPDATE *update,int iBody) {
  /* Nothing */
}


/*
void VerifyFlareFFD(BODY *body, CONTROL *control, OPTIONS *options,UPDATE *update,double dAge,int iBody) {

  // Assign luminosity
  if (body[iBody].iFlareFFD == FLARE_FFD_DAVENPORT) {
    body[iBody].dLuminosity = fdLuminosityFunctionBaraffe(body[iBody].dAge, body[iBody].dMass);
    if (options[OPT_FLAREFFD].iLine[iBody+1] >= 0) {
      // User specified luminosity, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT)
        printf("INFO: The FFD choose will follow Davenport et. al, 2019.\n");
    }
  } else if (body[iBody].iFlareFFD == FLARE_FFD_LACY) {
    body[iBody].dLuminosity = fdLuminosityFunctionProximaCen(body[iBody].dAge,body[iBody].dMass);
    if (options[OPT_FLAREFFD].iLine[iBody+1] >= 0) {
      // User specified luminosity, but we're reading it from the grid!
      if (control->Io.iVerbose >= VERBINPUT)
        printf("INFO: The FFD will constant during all the simulation.\n");
    }
  }
}


















*/

void VerifyLXUVFlare(BODY *body,OPTIONS *options,UPDATE *update,int iBody) {

  /* This may become useful once flare evolution is included
  update[iBody].iaType[update[iBody].iLXUV][0] = 1;
  update[iBody].iNumBodies[update[iBody].iLXUV][0]=1;
  update[iBody].iaBody[update[iBody].iLXUV][0] = malloc(update[iBody].iNumBodies[update[iBody].iLXUV][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iLXUV][0][0]=iBody;
  update[iBody].pdDLXUVFlareDt = &update[iBody].daDerivProc[update[iBody].iLXUV][0];*/

}


/*
void AssignStellarDerivatives(BODY *body,EVOLVE *evolve,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody) {
  fnUpdate[iBody][update[iBody].iRot][update[iBody].iRotStellar]               = &fdDRotRateDt;
  fnUpdate[iBody][update[iBody].iLostAngMom][update[iBody].iLostAngMomStellar] = &fdDJDtMagBrakingStellar;
  fnUpdate[iBody][update[iBody].iLostEng][update[iBody].iLostEngStellar]       = &fdDEDtStellar;
  fnUpdate[iBody][update[iBody].iLuminosity][0]                                = &fdLuminosity;  // NOTE: This points to the value of the Luminosity!
  fnUpdate[iBody][update[iBody].iRadius][0]                                    = &fdRadius;      // NOTE: This points to the value of the Radius!
  fnUpdate[iBody][update[iBody].iTemperature][0]                               = &fdTemperature; // NOTE: This points to the value of the Temperature!
  if(body[iBody].bEvolveRG) {
    fnUpdate[iBody][update[iBody].iRadGyra][0]                                 = &fdRadGyra; // NOTE: This points to the value of the Radius of Gyration!
  }
}

void NullStellarDerivatives(BODY *body,EVOLVE *evolve,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody) {
  fnUpdate[iBody][update[iBody].iRot][update[iBody].iRotStellar]               = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iLostAngMom][update[iBody].iLostAngMomStellar] = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iLostEng][update[iBody].iLostEngStellar]       = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iLuminosity][0]                                = &fndUpdateFunctionTiny; // NOTE: This points to the value of the Luminosity!
  fnUpdate[iBody][update[iBody].iRadius][0]                                    = &fndUpdateFunctionTiny; // NOTE: This points to the value of the Radius!
  fnUpdate[iBody][update[iBody].iLXUVFlare][0]                               = &fndUpdateFunctionTiny; // NOTE: This points to the value of the Temperature!
}
*/

void AssignFlareDerivatives(BODY *body,EVOLVE *evolve,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody) {
  /* No derivatives yet for flare.
  //This may become useful once flare evolution is included*/
  //fnUpdate[iBody][update[iBody].iLXUV][0] = &fdLXUVFlare;
  

}

void NullFlareDerivatives(BODY *body,EVOLVE *evolve,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody) {
  /* No derivatives yet for flare.
  This may become useful once flare evolution is included*/
  fnUpdate[iBody][update[iBody].iLXUV][0] = &fndUpdateFunctionTiny;
  
  //fnUpdate[iBody][update[iBody].iRot][update[iBody].iRotStellar]               = &fndUpdateFunctionTiny;
  //fnUpdate[iBody][update[iBody].iLostAngMom][update[iBody].iLostAngMomStellar] = &fndUpdateFunctionTiny;
  //fnUpdate[iBody][update[iBody].iLostEng][update[iBody].iLostEngStellar]       = &fndUpdateFunctionTiny;
  //fnUpdate[iBody][update[iBody].iLuminosity][0]                                = &fndUpdateFunctionTiny; // NOTE: This points to the value of the Luminosity!
  //fnUpdate[iBody][update[iBody].iRadius][0]                                    = &fndUpdateFunctionTiny; // NOTE: This points to the value of the Radius!
  //fnUpdate[iBody][update[iBody].iTemperature][0]                               = &fndUpdateFunctionTiny; // NOTE: This points to the value of the Temperature!
  //if(body[iBody].bEvolveRG) {
  //  fnUpdate[iBody][update[iBody].iRadGyra][0]                                 = &fndUpdateFunctionTiny; // NOTE: This points to the value of the Radius of Gyration!
  //}
}

void VerifyFlare(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,int iBody,int iModule) {
  int iFile=iBody+1;
  //double dLogEnergyMin, dLogEnergyMax, dEnergyMin, dEnergyMax;
  //int i;
  /* Mass must be in proper range */

  if (body[iBody].dMass < MINMASSFLARE*MSUN || body[iBody].dMass > MAXMASSFLARE*MSUN) {
    fprintf(stderr,"ERROR: Mass of %s must be between %.3lf and %.3lf\n",body[iBody].cName,MINMASSFLARE,MAXMASSFLARE);
    LineExit(files->Infile[iBody+1].cIn,options[OPT_MASS].iLine[iBody+1]);
  }

  VerifyLXUVFlare(body,options,update,iBody);
  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorFlare;
  control->fnPropsAux[iBody][iModule] = &PropsAuxFlare;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyFlare;

  
  /* For now, user may only input FlareConst and FlareExp. Eventually, user
     should also be able to input dLXUVFlare, which will require a call to
     NotTwoOfThree. So we must get initial LXUVFlare now. */

 
  /*Initializing the pointer dFFD*/
  //body[iBody].dFFD = malloc(8*sizeof(double));
}


void fnForceBehaviorFlare(BODY *body,MODULE *module,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {

  if (body[iBody].dLXUVFlare < 0)
    body[iBody].dLXUVFlare = 0;
  else
    body[iBody].dLXUVFlare = fdLXUVFlare(body,evolve->dTimeStep,iBody);
}


void InitializeBodyFlare(BODY *body,CONTROL *control,UPDATE *update,int iBody, int iModule) {
  //body[iBody].daOBSEnergy = malloc(control->Evolve.iNumBodies*sizeof(double));  // Allocating the size of the energy array to the OBSERV mode 
  //body[iBody].daOBSFlareRate = malloc(control->Evolve.iNumBodies*sizeof(double)); // Allocating the size of the flare rate array to the OBSERV mode
  body[iBody].dLXUVFlare = fdLXUVFlare(body,control->Evolve.dTimeStep,iBody);
//  body[iBody].daFlareFreq = fdFlareFreq(double dFlareCoefA, double dFlareCoefB, double dLogEnergyMin,double dLogEnergyMax, double dStarAge,double dStarMass) 
}

void InitializeModuleFlare(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** FLARE update ****************/

void InitializeUpdateFlare(BODY *body,UPDATE *update,int iBody) {
  /* STELLAR calculates LXUV from the star's properties, but FLARE calculates LXUV as a primary variable. It is the only possible update. */

  /* No primary variables for FLARE yet
  update[iBody].iNumVars++;
  update[iBody].iNumLXUV++;
  */
}

void FinalizeUpdateLXUVFlare(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  /* No primary variables for FLARE yet*/
  update[iBody].iaModule[iVar][*iEqn] = FLARE;
  update[iBody].iNumLXUV = (*iEqn)++;
  
}

/***************** FLARE Halts *****************/

/* Halt for massive flare? No Flares? */

void CountHaltsFlare(HALT *halt,int *iNumHalts) {
}

void VerifyHaltFlare(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
}

/************* FLARE Outputs ******************/

/* NOTE: If you write a new Write subroutine here you need to add the associate
   block of initialization in InitializeOutputFlare below */

void WriteLXUVFlare(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = fdLXUVFlare(body,control->Evolve.dTimeStep,iBody);
  // *dTmp = body[iBody].dLXUVFlare;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}


void WriteLXUVFlareUpper(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dLXUVFlareUpper;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}


void WriteLXUVFlareLower(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dLXUVFlareLower;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}

void WriteFlareFreq1(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareFreq1;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}
void WriteFlareFreq2(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareFreq2;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}
void WriteFlareFreq3(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareFreq3;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}
void WriteFlareFreq4(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareFreq4;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}
void WriteFlareFreq5(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareFreq5;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}
void WriteFlareFreq6(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareFreq6;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}
void WriteFlareEnergy1(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergy1;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}

void WriteFlareEnergy2(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergy2;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}

void WriteFlareEnergy3(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergy3;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}

void WriteFlareEnergy4(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergy4;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}
void WriteFlareEnergy5(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergy5;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}
void WriteFlareEnergy6(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareEnergy6;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}
void WriteFlareDuration1(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareDuration1;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}
void WriteFlareDuration2(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareDuration2;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}
void WriteFlareDuration3(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareDuration3;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}
void WriteFlareDuration4(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dFlareDuration4;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
   // *dTmp /= fdUnitsFrequency(units->iTime);
   // fsUnitsFrequency(units,cUnit);
  }
}


void InitializeOutputFlare(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_FLAREFREQ1].cName,"FlareFreq1");
  sprintf(output[OUT_FLAREFREQ1].cDescr,"Flare frequency in flares/day");
  sprintf(output[OUT_FLAREFREQ1].cNeg,"1/second");
  output[OUT_FLAREFREQ1].bNeg = 1;
  output[OUT_FLAREFREQ1].dNeg = 1./DAYSEC;
  output[OUT_FLAREFREQ1].iNum = 1;
  output[OUT_FLAREFREQ1].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ1] = &WriteFlareFreq1;

  sprintf(output[OUT_FLAREFREQ2].cName,"FlareFreq2");
  sprintf(output[OUT_FLAREFREQ2].cDescr,"Flare frequency in flares/day");
  sprintf(output[OUT_FLAREFREQ2].cNeg,"1/second");
  output[OUT_FLAREFREQ2].bNeg = 1;
  output[OUT_FLAREFREQ2].dNeg = 1./DAYSEC;
  output[OUT_FLAREFREQ2].iNum = 1;
  output[OUT_FLAREFREQ2].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ2] = &WriteFlareFreq2;

  sprintf(output[OUT_FLAREFREQ3].cName,"FlareFreq3");
  sprintf(output[OUT_FLAREFREQ3].cDescr,"Flare frequency in flares/day");
  sprintf(output[OUT_FLAREFREQ3].cNeg,"1/second");
  output[OUT_FLAREFREQ3].bNeg = 1;
  output[OUT_FLAREFREQ3].dNeg = 1./DAYSEC;
  output[OUT_FLAREFREQ3].iNum = 1;
  output[OUT_FLAREFREQ3].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ3] = &WriteFlareFreq3;

  sprintf(output[OUT_FLAREFREQ4].cName,"FlareFreq4");
  sprintf(output[OUT_FLAREFREQ4].cDescr,"Flare frequency in flares/day");
  sprintf(output[OUT_FLAREFREQ4].cNeg,"1/second");
  output[OUT_FLAREFREQ4].bNeg = 1;
  output[OUT_FLAREFREQ4].dNeg = 1./DAYSEC;
  output[OUT_FLAREFREQ4].iNum = 1;
  output[OUT_FLAREFREQ4].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ4] = &WriteFlareFreq4;

  sprintf(output[OUT_FLAREFREQ5].cName,"FlareFreq5");
  sprintf(output[OUT_FLAREFREQ5].cDescr,"Flare frequency in flares/day");
  sprintf(output[OUT_FLAREFREQ5].cNeg,"1/second");
  output[OUT_FLAREFREQ5].bNeg = 1;
  output[OUT_FLAREFREQ5].dNeg = 1./DAYSEC;
  output[OUT_FLAREFREQ5].iNum = 1;
  output[OUT_FLAREFREQ5].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ5] = &WriteFlareFreq5;

  sprintf(output[OUT_FLAREFREQ6].cName,"FlareFreq6");
  sprintf(output[OUT_FLAREFREQ6].cDescr,"Flare frequency in flares/day");
  sprintf(output[OUT_FLAREFREQ6].cNeg,"1/second");
  output[OUT_FLAREFREQ6].bNeg = 1;
  output[OUT_FLAREFREQ6].dNeg = 1./DAYSEC;
  output[OUT_FLAREFREQ6].iNum = 1;
  output[OUT_FLAREFREQ6].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ6] = &WriteFlareFreq6;


  sprintf(output[OUT_FLAREENERGY1].cName,"FlareEnergy1");
  sprintf(output[OUT_FLAREENERGY1].cDescr,"Flare energy in Joules");
  sprintf(output[OUT_FLAREENERGY1].cNeg,"1e7");
  output[OUT_FLAREENERGY1].bNeg = 1;
  output[OUT_FLAREENERGY1].dNeg = 1.0e7;
  output[OUT_FLAREENERGY1].iNum = 1;
  output[OUT_FLAREENERGY1].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY1] = &WriteFlareEnergy1;

  sprintf(output[OUT_FLAREENERGY2].cName,"FlareEnergy2");
  sprintf(output[OUT_FLAREENERGY2].cDescr,"Flare energy in Joules");
  sprintf(output[OUT_FLAREENERGY2].cNeg,"1e7");
  output[OUT_FLAREENERGY2].bNeg = 1;
  output[OUT_FLAREENERGY2].dNeg = 1.0e7;
  output[OUT_FLAREENERGY2].iNum = 1;
  output[OUT_FLAREENERGY2].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY2] = &WriteFlareEnergy2;

  sprintf(output[OUT_FLAREENERGY3].cName,"FlareEnergy3");
  sprintf(output[OUT_FLAREENERGY3].cDescr,"Flare energy in Joules");
  sprintf(output[OUT_FLAREENERGY3].cNeg,"1e7");
  output[OUT_FLAREENERGY3].bNeg = 1;
  output[OUT_FLAREENERGY3].dNeg = 1.0e7;
  output[OUT_FLAREENERGY3].iNum = 1;
  output[OUT_FLAREENERGY3].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY3] = &WriteFlareEnergy3;

  sprintf(output[OUT_FLAREENERGY4].cName,"FlareEnergy4");
  sprintf(output[OUT_FLAREENERGY4].cDescr,"Flare energy in Joules");
  sprintf(output[OUT_FLAREENERGY4].cNeg,"1e7");
  output[OUT_FLAREENERGY4].bNeg = 1;
  output[OUT_FLAREENERGY4].dNeg = 1.0e7;
  output[OUT_FLAREENERGY4].iNum = 1;
  output[OUT_FLAREENERGY4].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY4] = &WriteFlareEnergy4;

  sprintf(output[OUT_FLAREENERGY5].cName,"FlareEnergy5");
  sprintf(output[OUT_FLAREENERGY5].cDescr,"Flare energy in Joules");
  sprintf(output[OUT_FLAREENERGY5].cNeg,"1e7");
  output[OUT_FLAREENERGY5].bNeg = 1;
  output[OUT_FLAREENERGY5].dNeg = 1.0e7;
  output[OUT_FLAREENERGY5].iNum = 1;
  output[OUT_FLAREENERGY5].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY5] = &WriteFlareEnergy5;

  sprintf(output[OUT_FLAREENERGY6].cName,"FlareEnergy6");
  sprintf(output[OUT_FLAREENERGY6].cDescr,"Flare energy in Joules");
  sprintf(output[OUT_FLAREENERGY6].cNeg,"1e7");
  output[OUT_FLAREENERGY6].bNeg = 1;
  output[OUT_FLAREENERGY6].dNeg = 1.0e7;
  output[OUT_FLAREENERGY6].iNum = 1;
  output[OUT_FLAREENERGY6].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY6] = &WriteFlareEnergy6;

  sprintf(output[OUT_FLAREDURATION1].cName,"FlareDuration1");
  sprintf(output[OUT_FLAREDURATION1].cDescr,"Flare Duration in Seconds");
  sprintf(output[OUT_FLAREDURATION1].cNeg,"Minutes");
  output[OUT_FLAREDURATION1].bNeg = 1;
  output[OUT_FLAREDURATION1].dNeg = 1/60;
  output[OUT_FLAREDURATION1].iNum = 1;
  output[OUT_FLAREDURATION1].iModuleBit = FLARE;
  fnWrite[OUT_FLAREDURATION1] = &WriteFlareDuration1;

  sprintf(output[OUT_FLAREDURATION2].cName,"FlareDuration2");
  sprintf(output[OUT_FLAREDURATION2].cDescr,"Flare Duration in Seconds");
  sprintf(output[OUT_FLAREDURATION2].cNeg,"Minutes");
  output[OUT_FLAREDURATION2].bNeg = 1;
  output[OUT_FLAREDURATION2].dNeg = 1/60;
  output[OUT_FLAREDURATION2].iNum = 1;
  output[OUT_FLAREDURATION2].iModuleBit = FLARE;
  fnWrite[OUT_FLAREDURATION2] = &WriteFlareDuration2;

  sprintf(output[OUT_FLAREDURATION3].cName,"FlareDuration3");
  sprintf(output[OUT_FLAREDURATION3].cDescr,"Flare Duration in Seconds");
  sprintf(output[OUT_FLAREDURATION3].cNeg,"Minutes");
  output[OUT_FLAREDURATION3].bNeg = 1;
  output[OUT_FLAREDURATION3].dNeg = 1/60;
  output[OUT_FLAREDURATION3].iNum = 1;
  output[OUT_FLAREDURATION3].iModuleBit = FLARE;
  fnWrite[OUT_FLAREDURATION3] = &WriteFlareDuration3;

  sprintf(output[OUT_FLAREDURATION4].cName,"FlareDuration4");
  sprintf(output[OUT_FLAREDURATION4].cDescr,"Flare Duration in Seconds");
  sprintf(output[OUT_FLAREDURATION4].cNeg,"Minutes");
  output[OUT_FLAREDURATION4].bNeg = 1;
  output[OUT_FLAREDURATION4].dNeg = 1/60;
  output[OUT_FLAREDURATION4].iNum = 1;
  output[OUT_FLAREDURATION4].iModuleBit = FLARE;
  fnWrite[OUT_FLAREDURATION4] = &WriteFlareDuration4;

  sprintf(output[OUT_LXUVFLARE].cName,"LXUVFlare");
  sprintf(output[OUT_LXUVFLARE].cDescr,"XUV Luminosity from flares");
  sprintf(output[OUT_LXUVFLARE].cNeg,"LSUN");
  output[OUT_LXUVFLARE].bNeg = 1;
  output[OUT_LXUVFLARE].dNeg = 1./LSUN;
  output[OUT_LXUVFLARE].iNum = 1;
  output[OUT_LXUVFLARE].iModuleBit = FLARE;
  fnWrite[OUT_LXUVFLARE] = &WriteLXUVFlare;

  sprintf(output[OUT_LXUVFLAREUPPER].cName,"LXUVFlareUpper");
  sprintf(output[OUT_LXUVFLAREUPPER].cDescr,"Upper limit value to XUV Luminosity from flares when considerer flare rate slope and Y-intercept errors");
  sprintf(output[OUT_LXUVFLAREUPPER].cNeg,"LSUN");
  output[OUT_LXUVFLAREUPPER].bNeg = 1;
  output[OUT_LXUVFLAREUPPER].dNeg = 1./LSUN;
  output[OUT_LXUVFLAREUPPER].iNum = 1;
  output[OUT_LXUVFLAREUPPER].iModuleBit = FLARE;
  fnWrite[OUT_LXUVFLAREUPPER] = &WriteLXUVFlareUpper;

  sprintf(output[OUT_LXUVFLARELOWER].cName,"LXUVFlareLower");
  sprintf(output[OUT_LXUVFLARELOWER].cDescr,"Lower limit value to XUV Luminosity from flares when considerer flare rate slope and Y-intercept errors");
  sprintf(output[OUT_LXUVFLARELOWER].cNeg,"LSUN");
  output[OUT_LXUVFLARELOWER].bNeg = 1;
  output[OUT_LXUVFLARELOWER].dNeg = 1./LSUN;
  output[OUT_LXUVFLARELOWER].iNum = 1;
  output[OUT_LXUVFLARELOWER].iModuleBit = FLARE;
  fnWrite[OUT_LXUVFLARELOWER] = &WriteLXUVFlareLower;

}

/************ FLARE Logging Functions **************/

void LogOptionsFlare(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- FLARE Options -----\n\n");
  */
}

void LogFlare(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {

 /* Anything here?
  int iOut;

  fprintf(fp,"\n----- FLARE PARAMETERS ------\n");
  for (iOut=OUTSTARTRADHEAT;iOut<OUTBODYSTARTRADHEAT;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyFlare(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- FLARE PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTSTARTFLARE;iOut<OUTENDFLARE;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

void AddModuleFlare(CONTROL *control,MODULE *module,int iBody,int iModule) {

  module->fnCountHalts[iBody][iModule]         = &CountHaltsFlare;
  module->fnReadOptions[iBody][iModule]        = &ReadOptionsFlare;
  module->fnLogBody[iBody][iModule]            = &LogBodyFlare;
  module->fnVerify[iBody][iModule]             = &VerifyFlare;
  module->fnAssignDerivatives[iBody][iModule]  = &AssignFlareDerivatives;
  module->fnNullDerivatives[iBody][iModule]    = &NullFlareDerivatives;
  module->fnVerifyHalt[iBody][iModule]         = &VerifyHaltFlare;

  module->fnInitializeUpdate[iBody][iModule]   = &InitializeUpdateFlare;
  module->fnInitializeBody[iBody][iModule]     = &InitializeBodyFlare;
  module->fnFinalizeUpdateLXUV[iBody][iModule] = &FinalizeUpdateLXUVFlare;

  /* XXX I think this approach to multi-module outputs is defunct
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionFlare;
  */
}

/************* FLARE Functions ************/

// Function to convert the input band pass to the right one (SXR)

double fdBandPassXUV(BODY *body,int iBody,double dInputEnergy){
  double dEnergyXUV, dLogEnergyXUV;

    if (body[iBody].iFlareBandPass == FLARE_KEPLER) {// Band pass 4000 – 9000 Å
     // 1.875 comes from the Osten & Wolk 2015 (https://ui.adsabs.harvard.edu/abs/2015ApJ...809...79O/abstract), it's in Table 2 - the SXR
      //makes up 30% of the bolometric energy of the flare, and the Kepler bandpass makes up 16%. So if you have 
      //the Kepler energy or flux, multiply it by 30/16=1.875 to get the SXR energy. update 30th July 2020: I figured out that what I have to use its 16/30=0.533333
     dEnergyXUV = dInputEnergy*1.875;
     dLogEnergyXUV = log10(dInputEnergy*1.875);
    }
    else if (body[iBody].iFlareBandPass == FLARE_UV) { // Band pass 3000 – 4300 Å
     dEnergyXUV = dInputEnergy*2.727;
     dLogEnergyXUV = log10(dInputEnergy*2.727);
    }
    else if  (body[iBody].iFlareBandPass == FLARE_GOES) {   // Band pass 1 - 8 Å
     dEnergyXUV = dInputEnergy*5;
     dLogEnergyXUV = log10(dInputEnergy*5);   
    }

    else if  (body[iBody].iFlareBandPass == FLARE_SXR) { // Band pass 1.24 - 1239.85 Å
     dEnergyXUV = dInputEnergy;
     dLogEnergyXUV = log10(dInputEnergy);
    } 
  /*  else if (body[iBody].iFlareBandPass == FLARE_FUV) { // Band pass 1.24 - 1239.85 Å
     dEnergyXUV = dInputEnergy;
     dLogEnergyXUV = log10(dInputEnergy);
    } 
    else if (body[iBody].iFlareBandPass == FLARE_NUV) { // Band pass 1.24 - 1239.85 Å
     dEnergyXUV = dInputEnergy;
     dLogEnergyXUV = log10(dInputEnergy);
    }    
    else if (body[iBody].iFlareBandPass == FLARE_LYALPHA) { // Band pass 1.24 - 1239.85 Å
     dEnergyXUV = dInputEnergy;
     dLogEnergyXUV = log10(dInputEnergy);
    } */
    else if  (body[iBody].iFlareBandPass == FLARE_TESS_UV) { // Band pass 1.24 - 1239.85 Å
     dEnergyXUV = dInputEnergy*(0.3/0.076);
     dLogEnergyXUV = log10(dInputEnergy)*(0.3/0.076);
    }

    else if  (body[iBody].iFlareBandPass == FLARE_BOLOMETRIC) { // Band pass 1.24 - 1239.85 Å
     dEnergyXUV = dInputEnergy*0.3;
     dLogEnergyXUV = log10(dInputEnergy)*0.3;
    }
    return dLogEnergyXUV;
}



double fdBandPassKepler(BODY *body,int iBody,double dInputEnergy){
  double dEnergy, dLogEnergy;

    if (body[iBody].iFlareBandPass == FLARE_KEPLER) {// Band pass 4000 – 9000 Å
     dEnergy = dInputEnergy;
     dLogEnergy = log10(dInputEnergy);
    }
    else if (body[iBody].iFlareBandPass == FLARE_UV) { // Band pass 3000 – 4300 Å

     dEnergy = dInputEnergy*1.455;
     dLogEnergy = log10(dInputEnergy*1.455);
      
    }
    else if  (body[iBody].iFlareBandPass == FLARE_GOES) {   // Band pass 1 - 8 Å

     dEnergy = dInputEnergy*2.667;
     dLogEnergy = log10(dInputEnergy*2.667);
    
    }

    else if  (body[iBody].iFlareBandPass == FLARE_SXR) { // Band pass 1.24 - 1239.85 Å
     dEnergy = dInputEnergy*0.5334;
     dLogEnergy = log10(dInputEnergy*0.5334);
    }

    else if  (body[iBody].iFlareBandPass == FLARE_TESS_UV) { // Band pass 1.24 - 1239.85 Å
     dEnergy = dInputEnergy*(0.16/0.076);
     dLogEnergy = log10(dInputEnergy)*(0.16/0.076);
    }

    else if  (body[iBody].iFlareBandPass == FLARE_BOLOMETRIC) { // Band pass 1.24 - 1239.85 Å
     dEnergy = dInputEnergy*0.16;
     dLogEnergy = log10(dInputEnergy)*0.16;
    }
    return dLogEnergy;
}


// Davenport parameters of a star with mass dStarMass and age dStarAge
double fdDavenport(double dA1,double dA2,double dA3,double dStarAge,double dStarMass) {
  //Davenport function recives log(ergs) and return log(flare/day)
  // Coefficients calculate with dA(1,3) and dB(1,3)
  // Coefficients of Davenport et. al 2019
  //double dA1 = -0.07;
  //double dA2 = 0.79;
  //double dA3 = -1.06;
  // Calculating the dA and dB parameters with the Davenport et. al 2019 equation
  double dA, dStarAge_, dStarMass_;
  dStarAge_ = dStarAge/(YEARSEC*1.0e6);  // Convert dStarAge from seconds to million years because Davenport's function just acept Myr.
  dStarMass_ = dStarMass/MSUN;           // Convert dStarMass from kg to solar mass because Davenport's function just acept Msun.
// The StarMass its divide by a factor 1.99e30 (solar mass) because when you defines oon vpl.in you gives just in solar masses, but the
// code converts to kg
  dA = (dA1*log10(dStarAge_))+dA2*(dStarMass_)+dA3;

  return dA;
}

double fdFFD(BODY *body,int iBody,double dLogEnergy, double dFlareSlope, double dFlareYInt){
  double dFlareFreq, dFFDAY, dFFD ;
    dFlareFreq = (dFlareSlope*dLogEnergy)+(dFlareYInt);  //Here the Flare frequency are in log(flares/day). 

        if (body[iBody].iFlareFFD == FLARE_FFD_DAVENPORT){
          dFFDAY = pow(10,dFlareFreq);                                //Here the Flare frequency are in flares/day.
          dFFD = pow(10,dFlareFreq);                                //Here the Flare frequency are in flares/day.
          dFFD =  dFFD/DAYSEC;                                      //Here the Flare frequency are in flares/second.
        }
        if (body[iBody].iFlareFFD == FLARE_FFD_LACY){

          if  (body[iBody].iFlareSlopeUnits == FLARE_SLOPE_DAY){
            dFFD = pow(10,dFlareFreq);                              //Here the Flare frequency are in flares/day.
          }
          else if  (body[iBody].iFlareSlopeUnits == FLARE_SLOPE_SEC){
            dFFD = pow(10,dFlareFreq);                              //Here the Flare frequency are in flares/days.
            dFFD = dFFD/DAYSEC;                                   //Here the Flare frequency are in flares/seconds.
          }
          else if  (body[iBody].iFlareSlopeUnits == FLARE_SLOPE_HOUR){
            dFFD = pow(10,dFlareFreq);                              //Here the Flare frequency are in flares/hour.
            dFFD = dFFD/3600;                                   //Here the Flare frequency are in flares/seconds.
          }    
          else if  (body[iBody].iFlareSlopeUnits == FLARE_SLOPE_MINUTE){
            dFFD = pow(10,dFlareFreq);                              //Here the Flare frequency are in flares/minute.
            dFFD = dFFD/60;                                   //Here the Flare frequency are in flares/seconds.
          }    
          else {
            dFFD = pow(10,dFlareFreq);                                //Here the Flare frequency are in flares/day.
            dFFD =  dFFD/DAYSEC;                                      //Here the Flare frequency are in flares/second.
          }
        }  
  return dFFD;
}

double fdEnergyJoulesXUV(double dLogEnergyXUV){
  double dEnergyJOUXUV;

    dEnergyJOUXUV = pow(10,(dLogEnergyXUV));

 return dEnergyJOUXUV;
}



double fdLXUVFlare(BODY *body,double dDeltaTime,int iBody) {
  double dFlareSlope,dFlareYInt,dDuration;
  double dStarAge,dStarMass,dLXUVFlare;
  double dLogEnergyMinERG, dLogEnergyMaxERG, dEnergyMin, dEnergyMax, dLogEnergyMin, dLogEnergyMax;
  int i, iEnergyBin, iLogEnergyMinERG, iLogEnergyMaxERG, iLogEnergyMin, iLogEnergyMax;
  double dLogEnergyMinERGXUV, dLogEnergyMaxERGXUV, dEnergyMinXUV, dEnergyMaxXUV, dLogEnergyMinXUV, dLogEnergyMaxXUV;
  int iLogEnergyMinERGXUV, iLogEnergyMaxERGXUV, iLogEnergyMinXUV, iLogEnergyMaxXUV;
  double dEnergyStep, dEnergyStepXUV, dEnergyBin;


    //######################### 1. Choosing how to calculate FFD: slopes(age) or slopes(constant)?##################################

    if (body[iBody].iFlareFFD == FLARE_FFD_DAVENPORT) {
      dFlareSlope = fdDavenport(-0.07054598,0.81225239,-1.07054511,body[iBody].dAge,body[iBody].dMass);
      dFlareYInt = fdDavenport(2.06012734,-25.79885288,34.44115635,body[iBody].dAge,body[iBody].dMass);
    }

    if (body[iBody].iFlareFFD == FLARE_FFD_LACY) {
      dFlareSlope = body[iBody].dFlareSlope;
      dFlareYInt = body[iBody].dFlareYInt;
      /*dFlareSlopeErrorUpper = body[iBody].dFlareSlopeErrorUpper;
      dFlareSlopeErrorLower = body[iBody].dFlareSlopeErrorLower;
      dFlareYIntErrorUpper = body[iBody].dFlareYIntErrorUpper;
      dFlareYIntErrorLower = body[iBody].dFlareYIntErrorLower;*/
    }
    
     //################# 2. Calculating the XUV energy (SXR 1.24 - 1239.85 Å)#######################################################

     dLogEnergyMinXUV = fdBandPassXUV(body, iBody, body[iBody].dFlareMinEnergy);
     dLogEnergyMaxXUV = fdBandPassXUV(body, iBody, body[iBody].dFlareMaxEnergy);
     
     //1.0 J = 1.0e7 ergs, but its in log, so you have to multiply this by 7.0, not by 1.0e7
     //Convert the units of the energy from Joules to ergs because Davenport's function just acept ergs.
     dLogEnergyMinERGXUV = dLogEnergyMinXUV +7.0; 
     dLogEnergyMaxERGXUV = dLogEnergyMaxXUV +7.0;  
     
     // defining the array size (dEbin) of energies
     dEnergyBin = body[iBody].dEnergyBin;
     iEnergyBin = (int)dEnergyBin;

     dEnergyStepXUV = (dLogEnergyMaxERGXUV - dLogEnergyMinERGXUV)/iEnergyBin;
      
     //Declaring the XUV Energy arrays of size dEnergyBin
      double daEnergyERGXUV[iEnergyBin+1], daEnergyJOUXUV[iEnergyBin+1], daLogEnerXUV[iEnergyBin+1];

     //################# 3. Calculating the energy in the Kepler band pass (4000 – 9000 Å) ##############################################
    
     dLogEnergyMin = fdBandPassKepler(body, iBody, body[iBody].dFlareMinEnergy);
     dLogEnergyMax = fdBandPassKepler(body, iBody, body[iBody].dFlareMaxEnergy);

     dLogEnergyMinERG = dLogEnergyMin +7.0; 
     dLogEnergyMaxERG = dLogEnergyMax +7.0;  

     dEnergyStep = (dLogEnergyMaxERG - dLogEnergyMinERG)/iEnergyBin;

     //Declaring the Kepler Energy arrays of size iEnergyBin
      double daEnergyERG[iEnergyBin+1], daEnergyJOU[iEnergyBin+1], daLogEner[iEnergyBin+1];

     //############################ 4. Fulling the energy arrays ########################################################################
//  if (body[iBody].iFlareCalcLuminosity == FLARE_LUM_FFD){

       for(i = 0; i < iEnergyBin + 1; i++) {
            // XUV energy (energy_joules)
           daEnergyJOUXUV[i] = fdEnergyJoulesXUV(dLogEnergyMinXUV+i*dEnergyStepXUV);
            // Kepler energy (log(energy_ergs))
           daLogEner[i] = dLogEnergyMinERG+i*dEnergyStep;    

       }

      body[iBody].dFlareEnergy1 = daLogEner[0];
      body[iBody].dFlareEnergy2 = daLogEner[1];
      body[iBody].dFlareEnergy3 = daLogEner[2];
      body[iBody].dFlareEnergy4 = daLogEner[3];
      body[iBody].dFlareEnergy5 = daLogEner[4];
      body[iBody].dFlareEnergy6 = daLogEner[5];
     //############################ 5. Fulling the FFD arrays ########################################################################

      //Declaring the Flare Frequency distribution (FFD) arrays of size dEnergyBin
      double daFlareFreq[iEnergyBin+1],daFFD[iEnergyBin+1],daFFDAY[iEnergyBin+1];

     // When DAVENPORT or LACY are selected, we have to calculate the FFD. 
     // So we calculate the Flare rate with the principal values of slope and YInt.
     
      for(i = 0; i < iEnergyBin + 1; i++) {
         daFFD[i]= fdFFD(body, iBody,daLogEner[i], dFlareSlope, dFlareYInt);
         daFFDAY[i] = DAYSEC*daFFD[i];
      }

      body[iBody].dFlareFreq1 = daFFDAY[0];
      body[iBody].dFlareFreq2 = daFFDAY[1];
      body[iBody].dFlareFreq3 = daFFDAY[2];
      body[iBody].dFlareFreq4 = daFFDAY[3];
      body[iBody].dFlareFreq5 = daFFDAY[4];
      body[iBody].dFlareFreq6 = daFFDAY[5];
    
     //############################ 6. Calculating the XUV luminosity by flares ########################################################################
      double daLXUVFlare[iEnergyBin];
  
       // Calculating the luminosity by flares for DAVENPORT or LACY mode
       dLXUVFlare = 0;

      // If the user select to calculate the luminosity using FFD
    //  if (body[iBody].iFlareCalcLuminosity == FLARE_LUM_FFD) {
         for(i = 0; i < iEnergyBin; i++) {
         // If the user select to calculate the luminosity using GERSHBERG
          daLXUVFlare[i] = (daEnergyJOUXUV[i+1]-daEnergyJOUXUV[i])*((daFFD[i+1] + daFFD[i])/2);
          dLXUVFlare += daLXUVFlare[i];
         }
      //}      
 // }
 return dLXUVFlare;

}