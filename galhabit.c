/********************** GALHABIT.C **********************/
/*
 * Russell Deitrick, July 7, 2015
 *
 * Subroutines that simulate the galactic habitat of a stellar system
*/

/* note August 15: system->dScalingF added to NearbyStarDist(), VelocityDisp(), VerifyGalHabit(), ForceBehavior(), CalcEncounterRate */

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"

void BodyCopyGalHabit(BODY *dest,BODY *src,int iTideModel,int iNumBodies,int iBody) {
    dest[iBody].dEcc = src[iBody].dEcc;
    dest[iBody].dPeriQ = src[iBody].dPeriQ;
    dest[iBody].dArgP = src[iBody].dArgP;
    dest[iBody].dInc = src[iBody].dInc;
}

/**************** GALHABIT options ********************/

void ReadGalacDensity(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
/* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than or equal to 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    system->dGalacDensity = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    AssignDefaultDouble(options,&system->dGalacDensity,files->iNumInputs);
}

void ReadRForm(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
/* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than or equal to 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    system->dRForm = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    AssignDefaultDouble(options,&system->dRForm,files->iNumInputs);
}

void ReadStarScaleL(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
/* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than or equal to 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    system->dStarScaleL = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    AssignDefaultDouble(options,&system->dStarScaleL,files->iNumInputs);
}

void ReadTMigration(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
/* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than or equal to 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    system->dTMigration = dTmp*fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    AssignDefaultDouble(options,&system->dTMigration,files->iNumInputs);
}

void ReadRadialMigr(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    system->bRadialMigr = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&system->bRadialMigr,files->iNumInputs);
}

void ReadGasDensity(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
/* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than or equal to 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    system->dGasDensity = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    AssignDefaultDouble(options,&system->dGasDensity,files->iNumInputs);
}

void ReadDMDensity(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
/* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than or equal to 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    system->dDMDensity = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    AssignDefaultDouble(options,&system->dDMDensity,files->iNumInputs);
}

void ReadRandSeed(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int iTmp;

  AddOptionInt(files->Infile[iFile].cIn,options->cName,&iTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    system->iSeed = iTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    AssignDefaultInt(options,&system->iSeed,files->iNumInputs);
}


void ReadEncounterRad(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;
    
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
//     NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) 
      system->dEncounterRad = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else 
      system->dEncounterRad = dTmp*fdUnitsLength(control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      AssignDefaultDouble(options,&system->dEncounterRad,files->iNumInputs);
}

void ReadHostBinary(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].bHostBinary = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bHostBinary,files->iNumInputs);
}

void ReadHostBinSemi(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dHostBinSemi = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dHostBinSemi = dTmp*fdUnitsLength(control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dHostBinSemi = options->dDefault;
}

void ReadHostBinEcc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* Cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0 || dTmp >= 1) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be in the range [0,1).\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);  
    }
    body[iFile-1].dHostBinEcc = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);

  } else
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dHostBinEcc,files->iNumInputs);
}

/* argument of pericenter */
void ReadHostBinArgP(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);        
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);        
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    
    body[iFile-1].dHostBinArgP = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dHostBinArgP = options->dDefault;
}  

void ReadHostBinLongA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);        
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);        
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    
    body[iFile-1].dHostBinLongA = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dHostBinLongA = options->dDefault;
}  


void ReadHostBinInc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);        
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,180].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);        
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile-1].dHostBinInc = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0) {
      body[iFile-1].dHostBinInc = options->dDefault;
    }
}  

void InitializeOptionsGalHabit(OPTIONS *options,fnReadOption fnRead[]) {
  sprintf(options[OPT_GALACDENSITY].cName,"dGalacDensity");
  sprintf(options[OPT_GALACDENSITY].cDescr,"Density of galactic environment");
  sprintf(options[OPT_GALACDENSITY].cDefault,"0.102"); //need to find updated value
  options[OPT_GALACDENSITY].dDefault = 0.102;
  options[OPT_GALACDENSITY].iType = 2;  
  options[OPT_GALACDENSITY].iMultiFile = 0;   
  fnRead[OPT_GALACDENSITY] = &ReadGalacDensity;
  
  sprintf(options[OPT_RANDSEED].cName,"iRandSeed");
  sprintf(options[OPT_RANDSEED].cDescr,"Seed for random number generator (stellar encounters)");
  sprintf(options[OPT_RANDSEED].cDefault,"42");
  options[OPT_RANDSEED].dDefault = 42;
  options[OPT_RANDSEED].iType = 1;  
  options[OPT_RANDSEED].iMultiFile = 0;   
  fnRead[OPT_RANDSEED] = &ReadRandSeed;
  
  sprintf(options[OPT_ENCOUNTERRAD].cName,"dEncounterRad");
  sprintf(options[OPT_ENCOUNTERRAD].cDescr,"Radius at which stellar encounters occur");
  sprintf(options[OPT_ENCOUNTERRAD].cDefault,"206265 AU"); 
  options[OPT_ENCOUNTERRAD].dDefault = 206265.0*AUCM;
  options[OPT_ENCOUNTERRAD].iType = 2;  
  options[OPT_ENCOUNTERRAD].iMultiFile = 0;   
  fnRead[OPT_ENCOUNTERRAD] = &ReadEncounterRad;
  
  sprintf(options[OPT_RFORM].cName,"dRForm");
  sprintf(options[OPT_RFORM].cDescr,"Galactic formation radius");
  sprintf(options[OPT_RFORM].cDefault,"4.5 kpc"); 
  options[OPT_RFORM].dDefault = 4.5;
  options[OPT_RFORM].iType = 2;  
  options[OPT_RFORM].iMultiFile = 0;   
  fnRead[OPT_RFORM] = &ReadRForm;
  
  sprintf(options[OPT_TMIGRATION].cName,"dTMigration");
  sprintf(options[OPT_TMIGRATION].cDescr,"Time of radial migration");
  sprintf(options[OPT_TMIGRATION].cDefault,"3 Gy");
  options[OPT_TMIGRATION].dDefault = 3e9*YEARSEC;
  options[OPT_TMIGRATION].iType = 2;  
  options[OPT_TMIGRATION].iMultiFile = 0;   
  options[OPT_TMIGRATION].dNeg = YEARSEC;
  sprintf(options[OPT_TMIGRATION].cNeg,"Years");
  fnRead[OPT_TMIGRATION] = &ReadTMigration;
  
  sprintf(options[OPT_RADIALMIGR].cName,"bRadialMigr");
  sprintf(options[OPT_RADIALMIGR].cDescr,"Use radial migration?");
  sprintf(options[OPT_RADIALMIGR].cDefault,"0");
  options[OPT_RADIALMIGR].dDefault = 0;
  options[OPT_RADIALMIGR].iType = 0;  
  options[OPT_RADIALMIGR].iMultiFile = 0; 
  fnRead[OPT_RADIALMIGR] = &ReadRadialMigr;
  
  sprintf(options[OPT_GASDENSITY].cName,"dGasDensity");
  sprintf(options[OPT_GASDENSITY].cDescr,"Local ISM density");
  sprintf(options[OPT_GASDENSITY].cDefault,"0.05 Msun pc^3"); 
  options[OPT_GASDENSITY].dDefault = 0.05;
  options[OPT_GASDENSITY].iType = 2;  
  options[OPT_GASDENSITY].iMultiFile = 0;   
  fnRead[OPT_GASDENSITY] = &ReadGasDensity;
  
  sprintf(options[OPT_DMDENSITY].cName,"dDMDensity");
  sprintf(options[OPT_DMDENSITY].cDescr,"Local dark matter density");
  sprintf(options[OPT_DMDENSITY].cDefault,"0.01 Msun pc^3"); 
  options[OPT_DMDENSITY].dDefault = 0.01;
  options[OPT_DMDENSITY].iType = 2;  
  options[OPT_DMDENSITY].iMultiFile = 0;   
  fnRead[OPT_DMDENSITY] = &ReadDMDensity;
  
  sprintf(options[OPT_STARSCALEL].cName,"dStarScaleL");
  sprintf(options[OPT_STARSCALEL].cDescr,"Stellar radial scale length in MW");
  sprintf(options[OPT_STARSCALEL].cDefault,"2.4 kpc"); 
  options[OPT_STARSCALEL].dDefault = 2.4;
  options[OPT_STARSCALEL].iType = 2;  
  options[OPT_STARSCALEL].iMultiFile = 0;   
  fnRead[OPT_STARSCALEL] = &ReadStarScaleL;
  
  sprintf(options[OPT_HOSTBINARY].cName,"bHostBinary");
  sprintf(options[OPT_HOSTBINARY].cDescr,"Model primary as binary with quadrupole moment?");
  sprintf(options[OPT_HOSTBINARY].cDefault,"0");
  options[OPT_HOSTBINARY].dDefault = 0;
  options[OPT_HOSTBINARY].iType = 0;  
  options[OPT_HOSTBINARY].iMultiFile = 0; 
  fnRead[OPT_HOSTBINARY] = &ReadHostBinary;
  
  sprintf(options[OPT_HOSTBINECC].cName,"dHostBinEcc");
  sprintf(options[OPT_HOSTBINECC].cDescr,"eccentricity of host binary");
  sprintf(options[OPT_HOSTBINECC].cDefault,"0.51"); 
  options[OPT_HOSTBINECC].dDefault = 0.51;
  options[OPT_HOSTBINECC].iType = 2;  
  options[OPT_HOSTBINECC].iMultiFile = 0;   
  fnRead[OPT_HOSTBINECC] = &ReadHostBinEcc;
  
  sprintf(options[OPT_HOSTBINSEMI].cName,"dHostBinSemi");
  sprintf(options[OPT_HOSTBINSEMI].cDescr,"Semi-major of host binary");
  sprintf(options[OPT_HOSTBINSEMI].cDefault,"17.57 AU"); 
  options[OPT_HOSTBINSEMI].dDefault = 17.57*AUCM;
  options[OPT_HOSTBINSEMI].iType = 2;  
  options[OPT_HOSTBINSEMI].iMultiFile = 0;
  options[OPT_HOSTBINSEMI].dNeg = AUCM;
  sprintf(options[OPT_HOSTBINSEMI].cNeg,"AU");   
  fnRead[OPT_HOSTBINSEMI] = &ReadHostBinSemi;
  
  sprintf(options[OPT_HOSTBININC].cName,"dHostBinInc");
  sprintf(options[OPT_HOSTBININC].cDescr,"inclination of host binary");
  sprintf(options[OPT_HOSTBININC].cDefault,"60.0"); 
  options[OPT_HOSTBININC].dDefault = 60.0*DEGRAD;
  options[OPT_HOSTBININC].iType = 2;  
  options[OPT_HOSTBININC].iMultiFile = 0;   
  fnRead[OPT_HOSTBININC] = &ReadHostBinInc;
  
  sprintf(options[OPT_HOSTBINARGP].cName,"dHostBinArgP");
  sprintf(options[OPT_HOSTBINARGP].cDescr,"Arg periapse of host binary");
  sprintf(options[OPT_HOSTBINARGP].cDefault,"0.0"); 
  options[OPT_HOSTBINARGP].dDefault = 0.0;
  options[OPT_HOSTBINARGP].iType = 2;  
  options[OPT_HOSTBINARGP].iMultiFile = 0;   
  fnRead[OPT_HOSTBINARGP] = &ReadHostBinArgP;
  
  sprintf(options[OPT_HOSTBINLONGA].cName,"dHostBinLongA");
  sprintf(options[OPT_HOSTBINLONGA].cDescr,"Long of ascending node of host binary");
  sprintf(options[OPT_HOSTBINLONGA].cDefault,"0.0"); 
  options[OPT_HOSTBINLONGA].dDefault = 0.0;
  options[OPT_HOSTBINLONGA].iType = 2;  
  options[OPT_HOSTBINLONGA].iMultiFile = 0;   
  fnRead[OPT_HOSTBINLONGA] = &ReadHostBinLongA;
  
  
}




void ReadOptionsGalHabit(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTGALHABIT;iOpt<OPTENDGALHABIT;iOpt++) { 
      if (options[iOpt].iType != -1) {
        fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
      }
  }
}
    

/******************* Verify GALHABIT ******************/

void InitializePeriQGalHabit(BODY *body,UPDATE *update,int iBody) {
  update[iBody].iaType[update[iBody].iPeriQ][0] = 1;
  update[iBody].padDPeriQDtGalHabit[0] = &update[iBody].daDerivProc[update[iBody].iPeriQ][update[iBody].iaPeriQGalHabit[0]];
  update[iBody].iNumBodies[update[iBody].iPeriQ][update[iBody].iaPeriQGalHabit[0]]=2;
  update[iBody].iaBody[update[iBody].iPeriQ][update[iBody].iaPeriQGalHabit[0]] = malloc(update[iBody].iNumBodies[update[iBody].iPeriQ][update[iBody].iaPeriQGalHabit[0]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iPeriQ][update[iBody].iaPeriQGalHabit[0]][0] = iBody;
  update[iBody].iaBody[update[iBody].iPeriQ][update[iBody].iaPeriQGalHabit[0]][1] = 0;
}

void InitializeArgPGalHabit(BODY *body,UPDATE *update,int iBody) {
  update[iBody].iaType[update[iBody].iArgP][0] = 2;
  update[iBody].padDArgPDtGalHabit[0] = &update[iBody].daDerivProc[update[iBody].iArgP][update[iBody].iaArgPGalHabit[0]];
  update[iBody].iNumBodies[update[iBody].iArgP][update[iBody].iaArgPGalHabit[0]]=2;
  update[iBody].iaBody[update[iBody].iArgP][update[iBody].iaArgPGalHabit[0]] = malloc(update[iBody].iNumBodies[update[iBody].iArgP][update[iBody].iaArgPGalHabit[0]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iArgP][update[iBody].iaArgPGalHabit[0]][0] = iBody;
  update[iBody].iaBody[update[iBody].iArgP][update[iBody].iaArgPGalHabit[0]][1] = 0;
}


void VerifyGalHabit(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int i;
  int n;
  double dSigma, dDMR, dStarR, dGasR;
  char cOut[NAMELEN];
  FILE *fOut;
  
  srand(system->iSeed);
  
  if (iBody == 1) {
    system->dPassingStarR = malloc(3*sizeof(double));
    system->dPassingStarV = malloc(3*sizeof(double));
    system->dPassingStarImpact = malloc(3*sizeof(double));
    system->dHostApexVel = malloc(3*sizeof(double));
    if (system->bRadialMigr) {
      dDMR = DarkMatterDensity(system, system->dRForm);
      dStarR = (system->dGalacDensity-system->dGasDensity-system->dDMDensity)*\
              exp(-(system->dRForm-8)/system->dStarScaleL);
      dGasR = ISMDensity(system, system->dRForm);
      system->dScalingFTot = (dDMR+dStarR+dGasR)/system->dGalacDensity; //scale factor for total density (star+gas+dm)
      system->dScalingFStars = dStarR/(system->dGalacDensity-system->dGasDensity-system->dDMDensity);
      // system->dScalingFVelDisp = 1; //temporary, just to get solar neighborhood velocity dispersion
//       CalcMeanVelDispSolar(system);
      system->dScalingFVelDisp = exp(-(system->dRForm-8)/(2*system->dStarScaleL)); //based on Minchev+ 2012
    } else {
      system->dScalingFTot = 1.0;
      system->dScalingFStars = 1.0;
      system->dScalingFVelDisp = 1.0;
    }
    
    system->dGSNumberDens = malloc(13*sizeof(double));
    system->dGSNumberDens[0] = 0.43e-3;
    system->dGSNumberDens[1] = 3e-3;
    system->dGSNumberDens[2] = 0.06e-3;
    system->dGSNumberDens[3] = 0.27e-3;
    system->dGSNumberDens[4] = 0.44e-3;
    system->dGSNumberDens[5] = 1.42e-3;
    system->dGSNumberDens[6] = 0.64e-3;
    system->dGSNumberDens[7] = 1.52e-3;
    system->dGSNumberDens[8] = 2.34e-3;
    system->dGSNumberDens[9] = 2.68e-3;
    system->dGSNumberDens[10] = 5.26e-3;
    system->dGSNumberDens[11] = 8.72e-3;
    system->dGSNumberDens[12] = 41.55e-3;
    
    system->dGSBinMag = malloc(13*sizeof(double));
    system->dGSBinMag[0] = -7.0;
    system->dGSBinMag[1] = -6.0;
    system->dGSBinMag[2] = -5.0;
    system->dGSBinMag[3] = 0.0;
    system->dGSBinMag[4] = 2.0;
    system->dGSBinMag[5] = 3.0;
    system->dGSBinMag[6] = 3.8;
    system->dGSBinMag[7] = 4.2;
    system->dGSBinMag[8] = 5.0;
    system->dGSBinMag[9] = 6.0;
    system->dGSBinMag[10] = 7.0;
    system->dGSBinMag[11] = 9.0;
    system->dGSBinMag[12] = 13.0;
    
    CalcEncounterRate(system);  //need to update this, most likely XXX
    system->dDeltaTEnc = 0.0;
    system->dMinAllowed = 10.0*AUCM; //set to 10 au for now.
    system->dLastEncTime = 0.0;
    system->dCloseEncTime = 0.0;
    system->iNEncounters = 0;
    NextEncounterTime(system,0); 
    
  }
  
  
//   
//   GetStarMass(system);
//   GetStarVelocity(system);
//   GetStarPosition(system);
   testrand(system);
  
  // for (i=0;i<=10;i++) {
//     n = (int)((double)rand()*20/RAND_MAX)-3;
//     printf("%d\n",n);
//   }
  
  if (iBody >= 1) {
    sprintf(cOut,"%s.%s.Encounters",system->cName,body[iBody].cName);
    fOut = fopen(cOut,"w");
    fprintf(fOut,"#time MV mass sigma impx impy impz u_s v_s w_s u_r v_r w_r u_sun v_sun w_sun Rx Ry Rz\n");
    fclose(fOut);
    
    body[iBody].dPeriQ = body[iBody].dSemi*(1.0-body[iBody].dEcc);
    
    control->fnPropsAux[iBody][iModule] = &PropertiesGalHabit;
    InitializePeriQGalHabit(body,update,iBody);
    fnUpdate[iBody][update[iBody].iPeriQ][update[iBody].iaPeriQGalHabit[0]] = &fdGalHabitDPeriQDt;
    
    InitializeArgPGalHabit(body,update,iBody);
    fnUpdate[iBody][update[iBody].iArgP][update[iBody].iaArgPGalHabit[0]] = &fdGalHabitDArgPDt;

    control->fnForceBehavior[iBody][iModule]=&ForceBehaviorGalHabit;
    control->Evolve.fnBodyCopy[iBody][iModule]=&BodyCopyGalHabit;
  }
}


/***************** GALHABIT Update *****************/
void InitializeUpdateGalHabit(BODY *body,UPDATE *update,int iBody) {
  if (iBody > 0) {
    if (update[iBody].iNumPeriQ == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumPeriQ += 1;

    if (update[iBody].iNumArgP == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumArgP += 1;
  }
}

void FinalizeUpdatePeriQGalHabit(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
    update[iBody].padDPeriQDtGalHabit = malloc(1*sizeof(double*));
    update[iBody].iaPeriQGalHabit = malloc(1*sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaPeriQGalHabit[0] = (*iEqn)++;
}

void FinalizeUpdateArgPGalHabit(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
    update[iBody].padDArgPDtGalHabit = malloc(1*sizeof(double*));
    update[iBody].iaArgPGalHabit = malloc(1*sizeof(int));
    update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
    update[iBody].iaArgPGalHabit[0] = (*iEqn)++;
}


/***************** GALHABIT Halts *****************/

/* disruption? */
int HaltDisrupt(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {  

  if (body[iBody].iDisrupt == 1) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: body %d disrupted",iBody);
      printf(" at %.2e years\n",evolve->dTime/YEARSEC);
    }
    return 1;
  }

  return 0;
}

void CountHaltsGalHabit(HALT *halt,int *iNumHalts) { 
  (*iNumHalts)++;
}

void VerifyHaltGalHabit(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
  control->fnHalt[iBody][(*iHalt)++] = &HaltDisrupt;
}


/************* GALHABIT Outputs ******************/

void WriteBodyPeriQ(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
 
  *dTmp = body[iBody].dPeriQ;
  
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsLength(units->iLength);
    fsUnitsLength(units->iLength,cUnit);
  }
}

void WriteNEncounters(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
 
  *dTmp = (double)system->iNEncounters;
}
  
void InitializeOutputGalHabit(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_PERIQ].cName,"PeriQ");
  sprintf(output[OUT_PERIQ].cDescr,"Pericenter distance");
  sprintf(output[OUT_PERIQ].cNeg,"AU");
  output[OUT_PERIQ].bNeg = 1;
  output[OUT_PERIQ].dNeg = 1./AUCM;
  output[OUT_PERIQ].iNum = 1;
  output[OUT_PERIQ].iModuleBit = GALHABIT;
  fnWrite[OUT_PERIQ] = &WriteBodyPeriQ;
  
  sprintf(output[OUT_NENCOUNTERS].cName,"NEncounters");
  sprintf(output[OUT_NENCOUNTERS].cDescr,"cumulative number of stellar encounters");
  sprintf(output[OUT_NENCOUNTERS].cNeg," ");
  output[OUT_NENCOUNTERS].bNeg = 0;
  output[OUT_NENCOUNTERS].iNum = 1;
  output[OUT_NENCOUNTERS].iModuleBit = GALHABIT;
  fnWrite[OUT_NENCOUNTERS] = &WriteNEncounters;
}

void FinalizeOutputFunctionGalHabit(OUTPUT *output,int iBody,int iModule) {
  
}


/************ GALHABIT Logging Functions **************/

void LogOptionsGalHabit(CONTROL *control, FILE *fp) {

  fprintf(fp,"-------- GALHABIT Options -----\n\n");
  
}

void LogGalHabit(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  fprintf(fp,"\n----- GALHABIT PARAMETERS ------\n");
  for (iOut=OUTSTARTGALHABIT;iOut<OUTBODYSTARTGALHABIT;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,0);
  }
}

void LogBodyGalHabit(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"-----GALHABIT PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTGALHABIT;iOut<OUTENDGALHABIT;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

/************* MODULE Functions ***********/

void AddModuleGalHabit(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = GALHABIT;

  module->fnCountHalts[iBody][iModule] = &CountHaltsGalHabit;
  module->fnLogBody[iBody][iModule] = &LogBodyGalHabit;

  module->fnReadOptions[iBody][iModule] = &ReadOptionsGalHabit;
  module->fnVerify[iBody][iModule] = &VerifyGalHabit;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltGalHabit;

  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateGalHabit;
  module->fnInitializeOutput[iBody][iModule] = &InitializeOutputGalHabit;
  module->fnFinalizeUpdatePeriQ[iBody][iModule] = &FinalizeUpdatePeriQGalHabit;
  module->fnFinalizeUpdateArgP[iBody][iModule] = &FinalizeUpdateArgPGalHabit;

  //module->fnInitializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionEqtide;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionGalHabit;

}

/************* GALHABIT Functions ***********/
void PropertiesGalHabit(BODY *body,EVOLVE *evolve,UPDATE *update,int iBody) {  
  while (body[iBody].dArgP > 2*PI) {
    body[iBody].dArgP -= 2*PI;
  }
  while (body[iBody].dArgP < 0) {
    body[iBody].dArgP += 2*PI;
  }
  
  body[iBody].dEcc = 1.0 - body[iBody].dPeriQ/body[iBody].dSemi;
}

void ForceBehaviorGalHabit(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  double dp;
  char cOut[NAMELEN];
  FILE *fOut;
  
  if (system->bRadialMigr) {
    if (evolve->dTime >= system->dTMigration) {
      // time of migration passed? move to solar neighborhood
      system->dScalingFTot = 1.0; 
      system->dScalingFStars = 1.0;
      system->dScalingFVelDisp = 1.0;
      CalcEncounterRate(system);
      system->bRadialMigr = 0;  //don't recalculate this stuff again
    }
  }
  
  while (body[iBody].dArgP > 2*PI) {
    body[iBody].dArgP -= 2*PI;
  }
  while (body[iBody].dArgP < 0) {
    body[iBody].dArgP += 2*PI;
  }
  
  if (evolve->dTime + evolve->dCurrentDt >= system->dNextEncT) {
    system->dCloseEncTime = evolve->dTime + evolve->dCurrentDt;
    GetStarPosition(system);
    GetStarMass(system);
    system->dRelativeVelRad = 1.0;
    while (system->dRelativeVelRad >= 0) {
      GetStarVelocity(system); 
      GetRelativeVelocity(system);
    }
    /* next calculate impact parameter */
    CalcImpactParam(system); 
    
    /* then move the orbiter, get all distances/velocities, check for disruption */
    AdvanceMA(body,system,iBody);
    osc2cart(body,evolve->iNumBodies); //maybe need to convert to barycentric? XXX
    body[iBody].iDisrupt = check_disrupt(body,system,iBody);
  
    /* apply the impulse */
    ApplyDeltaV(body,system,iBody);
    cart2osc(body,evolve->iNumBodies);
    body[iBody].dInc = 2*asin(body[iBody].dSinc);
    body[iBody].dPeriQ = body[iBody].dSemi*(1.0-body[iBody].dEcc);
    body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[0].dMass+body[iBody].dMass);
    
    system->dLastEncTime = system->dCloseEncTime;
    system->iNEncounters += 1;
    NextEncounterTime(system,system->dCloseEncTime);
  
    /* write out encounter info */
    sprintf(cOut,"%s.%s.Encounters",system->cName,body[iBody].cName);
    fOut = fopen(cOut,"a");
    //fprintf(fOut,"#time MV mass sigma impx impy impz u_s v_s w_s u_r v_r w_r u_sun v_sun w_sun Rx Ry Rz\n");
    
    fprintd(fOut,evolve->dTime/YEARSEC,4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dPassingStarMagV,4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dPassingStarMass,4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dPassingStarSigma,4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dPassingStarImpact[0],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dPassingStarImpact[1],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dPassingStarImpact[2],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dPassingStarV[0],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dPassingStarV[1],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dPassingStarV[2],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dRelativeVel[0],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dRelativeVel[1],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dRelativeVel[2],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dHostApexVel[0],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dHostApexVel[1],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dHostApexVel[2],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dPassingStarR[0],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dPassingStarR[1],4,6);
    fprintf(fOut," ");
    fprintd(fOut,system->dPassingStarR[2],4,6);
    fprintf(fOut,"\n");
    
    fclose(fOut);
    
  }
}

int check_disrupt(BODY* body, SYSTEM *system, int iBody) {
  /* rather crude for the time being. checks that pericenter and apocenter are ok and 
     e < 1 */
  double apo;
  
  apo = body[iBody].dSemi*(1.0+body[iBody].dEcc);
  
  if (body[iBody].dPeriQ < system->dMinAllowed) {
    return 1;
  } else if (apo > system->dEncounterRad) {
    return 1;
  } else if (body[iBody].dEcc > 1.0) {
    return 1;
  } else {
    return 0;
  }   
}

double random_double() {
  double n;
  
  n = (double)rand()/RAND_MAX;
  return n;
}

int random_int(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Chop off all of the values that would cause skew...
    long end = RAND_MAX / n; // truncate skew
    assert (end > 0L);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}

void GetStarVelocity(SYSTEM *system) {
  /* get passing star velocities from dispersion = dSigma, using Box-Muller method*/
  double u1, u2, z0, z1, dSigma;
  int i;
  VelocityDisp(system);
  dSigma = system->dPassingStarSigma;
  
  u1 = random_double();
  u2 = random_double();
  
  z0 = sqrt(-2.0*log(u1))*cos(2.0*PI*u2);
  z1 = sqrt(-2.0*log(u1))*sin(2.0*PI*u2);
  
  system->dPassingStarV[0] = z0*dSigma*1000.0;  //scale with sigma and convert to m/s
  system->dPassingStarV[1] = z1*dSigma*1000.0;
  
  u1 = random_double();
  u2 = random_double();
  
  z0 = sqrt(-2.0*log(u1))*cos(2.0*PI*u2);
  
  system->dPassingStarV[2] = z0*dSigma*1000.0;
  system->dPassingStarVRad = 0;
  for (i=0;i<=2;i++) {
    system->dPassingStarVRad += system->dPassingStarV[i]*system->dPassingStarR[i];
  }
  system->dPassingStarVRad /= system->dPassingStarRMag;
}
    
double nsMinus6to15(double dMagV) {
  /* distribution of stars with mag: -6 <= MV <= 15 from Heisler, Tremaine & Alcock 1987
     !!! may need to be updated !!! */
  double dNs;
  
  dNs = 0.00358*pow(10.,(0.04*dMagV))/pow(1.0+1.8345*pow(10.,(-0.21*dMagV)),3.4) * \
        (1.0 - 0.15*pow(10.,(-0.25*fabs(dMagV-15.0))));
  
  return dNs;
}

double mag2mass(double dMagV) {
  double dlogMass;
  
  // if (dMagV > 0) {
//     dlogMass = -0.0928*dMagV + 0.448;
//   } else if (dMagV == -4) {
//     dlogMass = -0.18708664335714442;
//   } else { 
//     dlogMass = -0.271*dMagV + 0.448;
//   }
  if (dMagV > 10 && dMagV < 15) {
    dlogMass = 1e-3*(0.3 + 1.87*dMagV + 7.614*pow(dMagV,2) - 1.698*pow(dMagV,3) +\
                      0.06096*pow(dMagV,4));
  } else if (dMagV >= 15) {
    dlogMass = 1e-3*(0.3 + 1.87*15 + 7.614*pow(15,2) - 1.698*pow(15,3) +\
                      0.06096*pow(15,4));
  } else if (dMagV <= 10 && dMagV >= -5.7) {
    dlogMass = 0.477 - 0.135*dMagV + 1.228e-2*pow(dMagV,2) - 6.734e-4*pow(dMagV,3);
  } else if (dMagV < -5.7 && dMagV >= -6.7) {
    dlogMass = log10(0.9); //white dwarfs
  } else if (dMagV < -6.7) {
    dlogMass = log10(4.0); //giants
  }
  
  return pow(10.0,dlogMass);
}

void CalcEncounterRate(SYSTEM* system) {
  double dEncR = 0, dn, dVRel;
  int i;
  
  for (i=0;i<=12;i++) {
    system->dPassingStarMagV = system->dGSBinMag[i];
    VelocityDisp(system);
    VelocityApex(system);
    dn = system->dScalingFStars*system->dGSNumberDens[i];
    dVRel = sqrt(pow(system->dHostApexVelMag,2)+pow(system->dPassingStarSigma,2));
    
    dEncR += dVRel*1000*dn*pow(AUCM*206265,-3.0);
  }
  dEncR *= PI*pow(system->dEncounterRad,2);
  
  system->dEncounterRate = dEncR;
}

void CalcMeanVelDispSolar(SYSTEM* system) {
  double dMeanS = 0, dn, dnTot=0;
  int i;
  
  for (i=-4;i<=15;i++) {
    system->dPassingStarMagV = (double)i;
    VelocityDisp(system);
    dn = NearbyStarDist(system->dPassingStarMagV);
    
    dMeanS += system->dPassingStarSigma*dn;
    dnTot += dn;
  }
  dMeanS /= dnTot;
  
  system->dVelDispSolar = dMeanS;
}

void VelocityDisp(SYSTEM* system) {
  double dSigma, dMagV;
  
  dMagV = system->dPassingStarMagV;
  
  // if (dMagV == -4) {
//     dSigma = 36.6;
//   } else if ((dMagV <= -2) && (dMagV != -4)) {
//     dSigma = 8.5;
//   } else if ((dMagV > -2) && (dMagV <= 0)) {
//     dSigma = 11.4;
//   } else if ((dMagV > 0) && (dMagV <= 2)) {
//     dSigma = 13.7;
//   } else if ((dMagV > 2) && (dMagV <= 3)) {
//     dSigma = 16.8;
//   } else if ((dMagV > 3) && (dMagV <= 4)) {
//     dSigma = 20.9;
//   } else if ((dMagV > 4) && (dMagV <= 5)) {
//     dSigma = 22.6;
//   } else if ((dMagV > 5) && (dMagV <= 6)) {
//     dSigma = 24.0;
//   } else if ((dMagV > 6) && (dMagV <= 7)) {
//     dSigma = 25.0;
//   } else if ((dMagV > 7) && (dMagV <= 9)) {
//     dSigma = 24.7;  
//   } else if ((dMagV > 9) && (dMagV <= 15)) {
//     dSigma = 24.1;  
//   }

  if (dMagV >= -5.7 && dMagV <= -0.2) {
    dSigma = 14.7;
  } else if (dMagV > -0.2 && dMagV <= 1.3) {
    dSigma = 19.7;
  } else if (dMagV > 1.3 && dMagV <= 2.4) {
    dSigma = 23.7;
  } else if (dMagV > 2.4 && dMagV <= 3.6) {
    dSigma = 29.1;
  } else if (dMagV > 3.6 && dMagV <= 4.0) {
    dSigma = 36.2;
  } else if (dMagV > 4.0 && dMagV <= 4.7) {
    dSigma = 37.4;
  } else if (dMagV > 4.7 && dMagV <= 5.5) {
    dSigma = 39.2;
  } else if (dMagV > 5.5 && dMagV <= 6.4) {
    dSigma = 34.1;
  } else if (dMagV > 6.4 && dMagV <= 8.1) {
    dSigma = 43.4;
  } else if (dMagV > 8.1 && dMagV <= 9.9) {
    dSigma = 42.7;
  } else if (dMagV > 9.9) {
    dSigma = 41.8;
  } else if (dMagV < -5.7 && dMagV >= -6.7) {
    dSigma = 63.4;  //white dwarfs
  } else if (dMagV < -6.7) {
    dSigma = 41.0;  //giants
  }
  
  system->dPassingStarSigma = system->dScalingFVelDisp*dSigma; //XXX not sure if this scaling will be the same as density
}

void VelocityApex(SYSTEM* system) {
  double dVel, dMagV, phi, theta;
  
  dMagV = system->dPassingStarMagV;

  if (dMagV >= -5.7 && dMagV <= -0.2) {
    dVel = 18.6;
  } else if (dMagV > -0.2 && dMagV <= 1.3) {
    dVel = 17.1;
  } else if (dMagV > 1.3 && dMagV <= 2.4) {
    dVel = 13.7;
  } else if (dMagV > 2.4 && dMagV <= 3.6) {
    dVel = 17.1;
  } else if (dMagV > 3.6 && dMagV <= 4.0) {
    dVel = 17.1;
  } else if (dMagV > 4.0 && dMagV <= 4.7) {
    dVel = 26.4;
  } else if (dMagV > 4.7 && dMagV <= 5.5) {
    dVel = 23.9;
  } else if (dMagV > 5.5 && dMagV <= 6.4) {
    dVel = 19.8;
  } else if (dMagV > 6.4 && dMagV <= 8.1) {
    dVel = 25.0;
  } else if (dMagV > 8.1 && dMagV <= 9.9) {
    dVel = 17.3;
  } else if (dMagV > 9.9) {
    dVel = 23.3;
  } else if (dMagV < -5.7 && dMagV >= -6.7) {
    dVel = 38.3; //white dwarfs
  } else if (dMagV < -6.7) {
    dVel = 21.0;  //giants
  }
  
  system->dHostApexVelMag = dVel; 
  
  phi = random_double()*PI;
  theta = random_double()*2*PI;
  system->dHostApexVel[0] = dVel*sin(phi)*cos(theta);
  system->dHostApexVel[1] = dVel*sin(phi)*sin(theta);
  system->dHostApexVel[2] = dVel*cos(phi);
}

void GetRelativeVelocity(SYSTEM* system) {
  int i;
  double dVsq;
  VelocityApex(system);
  
  system->dRelativeVel[0] = system->dPassingStarV[0] - system->dHostApexVel[0];
  system->dRelativeVel[1] = system->dPassingStarV[1] - system->dHostApexVel[1];
  system->dRelativeVel[2] = system->dPassingStarV[2] - system->dHostApexVel[2];
  dVsq = 0;
  system->dRelativeVelRad = 0;
  for (i=0;i<=2;i++) {
    dVsq += pow(system->dRelativeVel[i],2);
    system->dRelativeVelRad += system->dRelativeVel[i]*system->dPassingStarR[i];
  }
  system->dRelativeVelMag = sqrt(dVsq);
  system->dRelativeVelRad /= system->dPassingStarRMag;
}

double NearbyStarDist(double dMagV) {
  double dNs, w;
  
//   if (dMagV <= -5.0) {
//     dNs = 0.0;
//   } else if (dMagV == -4.0) {
//     //white dwarf hack
//     dNs = 0.008;
//   } else if (dMagV > 19.0) {
//     dNs = 0.0;
//   } else if (dMagV > 15.0) {
//     dNs = nsMinus6to15(15.0);
//   } else {
//     dNs = nsMinus6to15(dMagV);
//   }
//   above, Heisler distribution, below Garcia-Sanchez distribution
  
  if (dMagV >= -5.7 && dMagV <= -0.2) {
    w = (5.7-0.2);
    dNs = 0.06/w;
  } else if (dMagV > -0.2 && dMagV <= 1.3) {
    w = (1.3+0.2);
    dNs = 0.27/w;
  } else if (dMagV > 1.3 && dMagV <= 2.4) {
    w = 2.4-1.3;
    dNs = 0.44/w;
  } else if (dMagV > 2.4 && dMagV <= 3.6) {
    w = 3.6-2.4;
    dNs = 1.41/w;
  } else if (dMagV > 3.6 && dMagV <= 4.0) {
    w = 4.-3.6;
    dNs = 0.64/w;
  } else if (dMagV > 4.0 && dMagV <= 4.7) {
    w = 4.7-4.;
    dNs = 1.52/w;
  } else if (dMagV > 4.7 && dMagV <= 5.5) {
    w = 5.5-4.7;
    dNs = 2.34/w;
  } else if (dMagV > 5.5 && dMagV <= 6.4) {
    w = 6.4-5.5;
    dNs = 2.68/w;
  } else if (dMagV > 6.4 && dMagV <= 8.1) {
    w = 8.1-6.4;
    dNs = 5.26/w;
  } else if (dMagV > 8.1 && dMagV <= 9.9) {
    w = 9.9-8.1;
    dNs = 8.72/w;
  } else if (dMagV > 9.9) {
    w = 18-9.9;
    dNs = 41.55/w;
  } else if (dMagV < -5.7 && dMagV >= -6.7) {
    dNs = 3.0;  //white dwarfs
  } else if (dMagV < -6.7) {
    dNs = 0.43;  //giants
  }
  return dNs/1000; //divide by 1000 to get number/pc^3
}

void GetStarMass(SYSTEM *system) {
  double ns = 0, dTmp = 100, dMagV, dMaxN;
  dMaxN = system->dScalingFStars*NearbyStarDist(15);
  
  while (dTmp > ns) {
    //dMagV = (double)(random_int(20)-4); //draw stellar magnitude (-3<dMagV<15)
    dMagV = (random_double()*25.7-7.7);
    dTmp = random_double()*dMaxN;       //if dTmp exceeds the number density, reject dMagV
    ns = system->dScalingFStars*NearbyStarDist(dMagV);         //get number density at dMagV
  }
  
  system->dPassingStarMagV = dMagV;
  //now get the mass of the star
  system->dPassingStarMass = mag2mass(dMagV)*MSUN;
}

void GetStarPosition(SYSTEM *system) {
  double r = system->dEncounterRad, costheta, phi, sintheta;
  
  costheta = random_double()*2 - 1;
  sintheta = sqrt(fabs(1.0-pow(costheta,2)));
  phi = random_double()*2*PI;
  
  system->dPassingStarR[0] = r*sintheta*cos(phi);
  system->dPassingStarR[1] = r*sintheta*sin(phi);
  system->dPassingStarR[2] = r*costheta;
  system->dPassingStarRMag = r;
}

void CalcImpactParam(SYSTEM *system) {
  double vsq = 0.0, dtime = 0.0;
  double x, y, z, r;
  int i;
  
  for (i=0;i<=2;i++) {
    vsq += pow(system->dRelativeVel[i],2);
    dtime += -system->dPassingStarR[i]*system->dRelativeVel[i];
  }
  dtime /= vsq;
  
  system->dPassingStarImpact[0] = system->dRelativeVel[0]*dtime + system->dPassingStarR[0];
  system->dPassingStarImpact[1] = system->dRelativeVel[1]*dtime + system->dPassingStarR[1];
  system->dPassingStarImpact[2] = system->dRelativeVel[2]*dtime + system->dPassingStarR[2];
//   r = sqrt(pow(x,2)+pow(y,2)+pow(z,2));
  
//   system->dCloseEncTime += dtime;
}

void ApplyDeltaV(BODY *body, SYSTEM *system, int iBody) {
  double dRelativeImpactx, dRelativeImpacty, dRelativeImpactz, dRelativeImpactrsq;
  double dRelativeVx, dRelativeVy, dRelativeVz, dRelativeV;
  double dPassingStarImpactrsq, dPassingStarV;
  double dDeltaVx, dDeltaVy, dDeltaVz;

  dRelativeImpactx = system->dPassingStarImpact[0]-body[iBody].dCartPos[0]*AUCM;
  dRelativeImpacty = system->dPassingStarImpact[1]-body[iBody].dCartPos[1]*AUCM;
  dRelativeImpactz = system->dPassingStarImpact[2]-body[iBody].dCartPos[2]*AUCM;
  dRelativeImpactrsq = pow(dRelativeImpactx,2) + pow(dRelativeImpacty,2) + \
                          pow(dRelativeImpactz,2);           
                          
  dRelativeVx = system->dRelativeVel[0]-body[iBody].dCartVel[0]*AUCM/DAYSEC;
  dRelativeVy = system->dRelativeVel[1]-body[iBody].dCartVel[1]*AUCM/DAYSEC;
  dRelativeVz = system->dRelativeVel[2]-body[iBody].dCartVel[2]*AUCM/DAYSEC;

  dRelativeV = sqrt(pow(dRelativeVx,2)+pow(dRelativeVy,2)+pow(dRelativeVz,2));
  
  dPassingStarImpactrsq = pow(system->dPassingStarImpact[0],2) + pow(system->dPassingStarImpact[1],2) + \
                          pow(system->dPassingStarImpact[2],2);
  dPassingStarV = system->dRelativeVelMag;

  dDeltaVx = 2*BIGG*system->dPassingStarMass * (1.0/(dRelativeV*dRelativeImpactrsq)*dRelativeImpactx\
            - 1.0/(dPassingStarV*dPassingStarImpactrsq)*system->dPassingStarImpact[0]);
  dDeltaVy = 2*BIGG*system->dPassingStarMass * (1.0/(dRelativeV*dRelativeImpactrsq)*dRelativeImpacty\
            - 1.0/(dPassingStarV*dPassingStarImpactrsq)*system->dPassingStarImpact[1]);
  dDeltaVz = 2*BIGG*system->dPassingStarMass * (1.0/(dRelativeV*dRelativeImpactrsq)*dRelativeImpactz\
            - 1.0/(dPassingStarV*dPassingStarImpactrsq)*system->dPassingStarImpact[2]);
            
  body[iBody].dCartVel[0] += dDeltaVx/AUCM*DAYSEC;
  body[iBody].dCartVel[1] += dDeltaVy/AUCM*DAYSEC;
  body[iBody].dCartVel[2] += dDeltaVz/AUCM*DAYSEC;
}

void AdvanceMA(BODY *body, SYSTEM *system, int iBody) {
  double dTmp, dTime;
    
  dTmp = body[iBody].dMeanA;
  dTime = system->dCloseEncTime - system->dLastEncTime;
 
  body[iBody].dMeanA = (dTmp + body[iBody].dMeanMotion*dTime);
  while (body[iBody].dMeanA >= 2*PI) {
    body[iBody].dMeanA -= 2*PI;
  }
}

void NextEncounterTime(SYSTEM *system, double dTime) {
  double dp;
  
  dp = random_double();
  system->dNextEncT = dTime - log(dp)/system->dEncounterRate;
}

void testrand(SYSTEM *system) { 
  char cOut[NAMELEN];
  FILE *fOut;
  double n = 0, m, y = 100;
  int i;
  
  sprintf(cOut,"randoms");
  fOut = fopen(cOut,"w");
  
  for (i=0;i<=100000;i++) {
    while (y > n) {
        m = (random_double()*23.7-5.7);
        y = random_double()*20;
        n = NearbyStarDist(m);
    }
    
    fprintd(fOut,m,4,6);
    fprintf(fOut,"\n");
    y = 100;
    n = 0;
  }
  fclose(fOut);
  
  //return 0;
}

double DarkMatterDensity(SYSTEM *system, double dRad) {
  /* halo model from Kordopatis+ 2015 */
  double dR0 = 8.0, gamma = -2.0, beta = 2.21, q = 0.8, z = 0.025, r0 = 1.09, rc = 1000.0;
  double dmR, dm8, drho_R;
  dm8 = sqrt(pow(8,2)+pow(z/q,2));
  dmR = sqrt(pow(dRad,2)+pow(z/q,2));

  drho_R = pow(dm8/dmR,gamma)*pow((1+dm8/r0)/(1+dmR/r0),beta-gamma)*exp(-1*(pow(dmR,2)-pow(dm8,2))/pow(rc,2));
  
  return drho_R*system->dDMDensity; //convert to Msun pc^-3
}

double ISMDensity(SYSTEM *system, double dRad) {
  double dRd = 2*system->dStarScaleL, dRh = 4.0, drho_R;
  
  drho_R = exp(-(dRad-8)/dRd-dRh/dRad+dRh/8)*system->dGasDensity;
  return drho_R;
}  

double dX2TimeAvg(BODY *body) {
  /* calculate host binary's time-averaged x^2 */
  double dTmp, a, i, e, w, Om;
  a = body[0].dHostBinSemi;
  e = body[0].dHostBinEcc;
  i = body[0].dHostBinInc;
  w = body[0].dHostBinArgP;
  Om = body[0].dHostBinLongA;
  
  dTmp = 1./64.*pow(a,2)*( 24.+36.*pow(e,2)+4.*(2.+3.*pow(e,2))*cos(2*i)-\
              10.*pow(e,2)*cos(2*(i-w))+20.*pow(e,2)*cos(2*w)-10.*pow(e,2)*cos(2*(i+w))-\
              20.*pow(e,2)*cos(i+2*w-2*Om)-4*cos(2*(i-Om))-\
              6.*pow(e,2)*cos(2*(i-Om))+5.*pow(e,2)*cos(2*(i-w-Om))+\
              30.*pow(e,2)*cos(2*(w-Om))+5.*pow(e,2)*cos(2*(i+w-Om))+8.*cos(2*Om)+\
              12*pow(e,2)*cos(2*Om)-4*cos(2*(i+Om))-6*pow(e,2)*cos(2*(i+Om))+\
              5.*pow(e,2)*cos(2*(i-w+Om))+30.*pow(e,2)*cos(2*(w+Om))+\
              5.*pow(e,2)*cos(2*(i+w+Om))-20.*pow(e,2)*cos(i-2*w+2*Om)+\
              20.*pow(e,2)*cos(i-2*(w+Om))+20.*pow(e,2)*cos(i+2*(w+Om)) );
  
  return dTmp;
} 

double dY2TimeAvg(BODY *body) {
  /* calculate host binary's time-averaged y^2 */
  double dTmp, a, i, e, w, Om;
  a = body[0].dHostBinSemi;
  e = body[0].dHostBinEcc;
  i = body[0].dHostBinInc;
  w = body[0].dHostBinArgP;
  Om = body[0].dHostBinLongA;
  
  dTmp = -1./64.*pow(a,2)*( -24.-36.*pow(e,2)-4.*(2.+3.*pow(e,2))*cos(2*i)+\
              10.*pow(e,2)*cos(2*(i-w))-20.*pow(e,2)*cos(2*w)+10.*pow(e,2)*cos(2*(i+w))-\
              20.*pow(e,2)*cos(i+2*w-2*Om)-4*cos(2*(i-Om))-\
              6.*pow(e,2)*cos(2*(i-Om))+5.*pow(e,2)*cos(2*(i-w-Om))+\
              30.*pow(e,2)*cos(2*(w-Om))+5.*pow(e,2)*cos(2*(i+w-Om))+8.*cos(2*Om)+\
              12*pow(e,2)*cos(2*Om)-4*cos(2*(i+Om))-6*pow(e,2)*cos(2*(i+Om))+\
              5.*pow(e,2)*cos(2*(i-w+Om))+30.*pow(e,2)*cos(2*(w+Om))+\
              5.*pow(e,2)*cos(2*(i+w+Om))-20.*pow(e,2)*cos(i-2*w+2*Om)+\
              20.*pow(e,2)*cos(i-2*(w+Om))+20.*pow(e,2)*cos(i+2*(w+Om)) );
  
  return dTmp;
} 

double dZ2TimeAvg(BODY *body) {
  /* calculate host binary's time-averaged z^2 */
  double dTmp, a, i, e, w, Om;
  a = body[0].dHostBinSemi;
  e = body[0].dHostBinEcc;
  i = body[0].dHostBinInc;
  w = body[0].dHostBinArgP;
  Om = body[0].dHostBinLongA;
  
  dTmp = 1./4*pow(a,2)*(2.0+3.*pow(e,2)-5.*pow(e,2)*cos(2*w))*pow(sin(i),2);
  
  return dTmp;
} 
    
double dXYTimeAvg(BODY *body) {
  /* calculate host binary's time-averaged z^2 */
  double dTmp, a, i, e, w, Om;
  a = body[0].dHostBinSemi;
  e = body[0].dHostBinEcc;
  i = body[0].dHostBinInc;
  w = body[0].dHostBinArgP;
  Om = body[0].dHostBinLongA;
  
  dTmp = 1./16.*pow(a,2)*(20.*pow(e,2)*cos(i)*cos(2*Om)*sin(2*Om) + \
            (5.*pow(e,2)*(3+cos(2*i))*cos(2*w)+2*(2.+3.*pow(e,2))*pow(sin(i),2))*sin(2*Om));
  
  return dTmp;
} 

double dXZTimeAvg(BODY *body) {
  /* calculate host binary's time-averaged z^2 */
  double dTmp, a, i, e, w, Om;
  a = body[0].dHostBinSemi;
  e = body[0].dHostBinEcc;
  i = body[0].dHostBinInc;
  w = body[0].dHostBinArgP;
  Om = body[0].dHostBinLongA;
  
  dTmp = 1./4.*pow(a,2)*sin(i)*(5.*pow(e,2)*cos(Om)*sin(2.*w)+\
            cos(i)*(-2.-3*pow(e,2)+5*pow(e,2)*cos(2.*w))*sin(Om));
  
  return dTmp;
}   

double dYZTimeAvg(BODY *body) {
  /* calculate host binary's time-averaged z^2 */
  double dTmp, a, i, e, w, Om;
  a = body[0].dHostBinSemi;
  e = body[0].dHostBinEcc;
  i = body[0].dHostBinInc;
  w = body[0].dHostBinArgP;
  Om = body[0].dHostBinLongA;
  
  dTmp = 1./4.*pow(a,2)*sin(i)*(-cos(i)*(-2.-3*pow(e,2)+5.*pow(e,2)*cos(2*w))*cos(Om) +\
                  5.*pow(e,2)*sin(2*w)*sin(Om) );
  
  return dTmp;
}

// double dX2r5TimeAvg(BODY *body, int iBody) {
//   double dTmp, a, e, i, Om;
//   
//   a = body[iBody].dSemi;
//   e = body[iBody].dEcc;
//   i = body[iBody].dInc;
//   Om = body[iBody].dLongA;
//   
//   dTmp = ( 3.0+cos(2*i)+2.*cos(2*Om)*pow(sin(i),2.) )/(8.*pow(a,3)*pow((1-pow(e,2)),1.5);
//   
//   return dTmp;
// }


double dDADeDist(BODY *body, int iBody) {
  double dTmp, a, e, i, Om;
  
  a = body[iBody].dSemi;
  e = body[iBody].dEcc;
  i = body[iBody].dInc;
  Om = body[iBody].dLongA;
  
  dTmp = 3.*e*( 3.+cos(2*i)+2.*cos(2*Om)*pow(sin(i),2) )/(8.*pow(a,3)*pow((1.-pow(e,2)),2.5));
  
  return dTmp;
}

double dDADiDist(BODY *body, int iBody) {
  double dTmp, a, e, i, Om;
  
  a = body[iBody].dSemi;
  e = body[iBody].dEcc;
  i = body[iBody].dInc;
  Om = body[iBody].dLongA;
  
  dTmp = ( -2.*sin(2*i)+4.*cos(2*Om)*cos(i)*sin(i) )/(8.*pow(a,3)*pow((1.-pow(e,2)),1.5));
  
  return dTmp;
}

double dDADOmDist(BODY *body, int iBody) {
  double dTmp, a, e, i, Om;
  
  a = body[iBody].dSemi;
  e = body[iBody].dEcc;
  i = body[iBody].dInc;
  Om = body[iBody].dLongA;
  
  dTmp = -( sin(2*Om)*pow(sin(i),2) )/(2.*pow(a,3)*pow((1.-pow(e,2)),1.5));
  
  return dTmp;
}

double dDBDeDist(BODY *body, int iBody) {
  double dTmp, a, e, i, Om;
  
  a = body[iBody].dSemi;
  e = body[iBody].dEcc;
  i = body[iBody].dInc;
  Om = body[iBody].dLongA;
  
  dTmp = 3.*e*( 3.+cos(2*i)-2.*cos(2*Om)*pow(sin(i),2) )/(8.*pow(a,3)*pow((1.-pow(e,2)),2.5));
  
  return dTmp;
}

double dDBDiDist(BODY *body, int iBody) {
  double dTmp, a, e, i, Om;
  
  a = body[iBody].dSemi;
  e = body[iBody].dEcc;
  i = body[iBody].dInc;
  Om = body[iBody].dLongA;
  
  dTmp = ( -2.*sin(2*i)-4.*cos(2*Om)*cos(i)*sin(i) )/(8.*pow(a,3)*pow((1.-pow(e,2)),1.5));
  
  return dTmp;
}

double dDBDOmDist(BODY *body, int iBody) {
  double dTmp, a, e, i, Om;
  
  a = body[iBody].dSemi;
  e = body[iBody].dEcc;
  i = body[iBody].dInc;
  Om = body[iBody].dLongA;
  
  dTmp = ( sin(2*Om)*pow(sin(i),2) )/(2.*pow(a,3)*pow((1.-pow(e,2)),1.5));
  
  return dTmp;
}

double dDCDeDist(BODY *body, int iBody) {
  double dTmp, a, e, i;
  
  a = body[iBody].dSemi;
  e = body[iBody].dEcc;
  i = body[iBody].dInc;
  
  dTmp = -( 3.*e*pow(sin(i),2) )/(2.*pow(a,3)*pow((1.-pow(e,2)),2.5));
  
  return dTmp;
}

double dDCDiDist(BODY *body, int iBody) {
  double dTmp, a, e, i;
  
  a = body[iBody].dSemi;
  e = body[iBody].dEcc;
  i = body[iBody].dInc;
  
  dTmp = -( cos(i)*sin(i) )/(pow(a,3)*pow((1.-pow(e,2)),1.5));
  
  return dTmp;
}

double dDDDeDist(BODY *body, int iBody) {
  double dTmp, a, e, i, Om;
  
  a = body[iBody].dSemi;
  e = body[iBody].dEcc;
  i = body[iBody].dInc;
  Om = body[iBody].dLongA;
  
  dTmp = -3.*e*(sin(2*Om)*pow(sin(i),2))/(4.*pow(a,3)*pow((1.-pow(e,2)),2.5));
  
  return dTmp;
}

double dDDDiDist(BODY *body, int iBody) {
  double dTmp, a, e, i, Om;
  
  a = body[iBody].dSemi;
  e = body[iBody].dEcc;
  i = body[iBody].dInc;
  Om = body[iBody].dLongA;
  
  dTmp = (sin(2*Om)*cos(i)*sin(i))/(2.*pow(a,3)*pow((1.-pow(e,2)),1.5));
  
  return dTmp;
}

double dDDDOmDist(BODY *body, int iBody) {
  double dTmp, a, e, i, Om;
  
  a = body[iBody].dSemi;
  e = body[iBody].dEcc;
  i = body[iBody].dInc;
  Om = body[iBody].dLongA;
  
  dTmp = (cos(2*Om)*pow(sin(i),2))/(2.*pow(a,3)*pow((1.-pow(e,2)),1.5));
  
  return dTmp;
}

//--------------Galactic stuff!--------------------------------------------------------------

double fdGalHabitDPeriQDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dRho = system->dScalingFTot*system->dGalacDensity/pow(AUPC,3), dMu, dEcc;
  dMu = KGAUSS*KGAUSS*body[0].dMass/MSUN; //calculate mass coefficient for primary/primary+secondary
  dEcc = 1.0 - body[iaBody[0]].dPeriQ/body[iaBody[0]].dSemi; //calculate orbiter's eccentricity
  
  return -pow(body[iaBody[0]].dSemi/AUCM,5./2)*sqrt((1.0-pow(dEcc,2))/dMu)*5*PI*KGAUSS*KGAUSS*\
      dRho*dEcc*pow(sin(body[iaBody[0]].dInc),2)*sin(2*body[iaBody[0]].dArgP)/DAYSEC*AUCM;
}

double fdGalHabitDArgPDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dRho = system->dScalingFTot*system->dGalacDensity/pow(AUPC,3), dMu, dEcc;
  dMu = KGAUSS*KGAUSS*body[0].dMass/MSUN; //calculate mass coefficient for primary/primary+secondary
  dEcc = 1.0 - body[iaBody[0]].dPeriQ/body[iaBody[0]].dSemi; //calculate orbiter's eccentricity

  return 2*PI*KGAUSS*KGAUSS*dRho*sqrt(pow(body[iaBody[0]].dSemi/AUCM,3)/(dMu*(1.0-pow(dEcc,2))))*\
      (1.-pow(dEcc,2)-5.*(1.-pow(dEcc,2)-pow(cos(body[iaBody[0]].dInc),2))*\
      pow(sin(body[iaBody[0]].dArgP),2.0))/DAYSEC;
}

