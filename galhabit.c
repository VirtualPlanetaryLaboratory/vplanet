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
    dest[iBody].dPeriQ = src[iBody].dPeriQ;
    dest[iBody].dArgP = src[iBody].dArgP;
    dest[iBody].dLongP = src[iBody].dLongP;
    dest[iBody].dInc = src[iBody].dInc;
    dest[iBody].dLongA = src[iBody].dLongA;
    dest[iBody].dEccX = src[iBody].dEccX;
    dest[iBody].dEccY = src[iBody].dEccY;
    dest[iBody].dEccZ = src[iBody].dEccZ;
    dest[iBody].dAngM = src[iBody].dAngM;
    dest[iBody].dAngMX = src[iBody].dAngMX;
    dest[iBody].dAngMY = src[iBody].dAngMY;
    dest[iBody].dAngMZ = src[iBody].dAngMZ;
    dest[iBody].dEccXTmp = src[iBody].dEccXTmp;
    dest[iBody].dEccYTmp = src[iBody].dEccYTmp;
    dest[iBody].dEccZTmp = src[iBody].dEccZTmp;
    dest[iBody].dAngMXTmp = src[iBody].dAngMXTmp;
    dest[iBody].dAngMYTmp = src[iBody].dAngMYTmp;
    dest[iBody].dAngMZTmp = src[iBody].dAngMZTmp;    
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

void ReadStellarEnc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    system->bStellarEnc = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&system->bStellarEnc,files->iNumInputs);
}

void ReadOutputEnc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    system->bOutputEnc = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&system->bOutputEnc,files->iNumInputs);
}

void ReadTimeEvolVelDisp(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    system->bTimeEvolVelDisp = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&system->bTimeEvolVelDisp,files->iNumInputs);
}

void ReadHostBinary(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    body[iFile-1].bHostBinary = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bHostBinary,files->iNumInputs);
}

void ReadGalacTides(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    body[iFile-1].bGalacTides = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&body[iFile-1].bGalacTides,files->iNumInputs);
}

// void ReadMinAllowed(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
//   /* This parameter cannot exist in primary file */
//   int lTmp=-1;
//   double dTmp;
// 
//   AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
//   if (lTmp >= 0) {
//     NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
//     if (dTmp < 0)
//       body[iFile-1].dMinAllowed = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
//     else
//       body[iFile-1].dMinAllowed = dTmp*fdUnitsLength(control->Units[iFile].iLength);
//     UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
//   } else
//     if (iFile > 0)
//       body[iFile-1].dMinAllowed = options->dDefault;
// }

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

void ReadMinAllowed(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dMinAllowed = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dMinAllowed = dTmp*fdUnitsLength(control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dMinAllowed = options->dDefault;
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

void ReadHostBinMass1(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with Radius, Density and MassRad */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dHostBinMass1 = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dHostBinMass1 = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dHostBinMass1 = options->dDefault;
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
  
  
  sprintf(options[OPT_STELLARENC].cName,"bStellarEnc");
  sprintf(options[OPT_STELLARENC].cDescr,"Model stellar encounters?");
  sprintf(options[OPT_STELLARENC].cDefault,"1");
  options[OPT_STELLARENC].dDefault = 1;
  options[OPT_STELLARENC].iType = 0;  
  options[OPT_STELLARENC].iMultiFile = 0; 
  fnRead[OPT_STELLARENC] = &ReadStellarEnc;
  
  sprintf(options[OPT_OUTPUTENC].cName,"bOutputEnc");
  sprintf(options[OPT_OUTPUTENC].cDescr,"Output stellar encounter information?");
  sprintf(options[OPT_OUTPUTENC].cDefault,"0");
  options[OPT_OUTPUTENC].dDefault = 0;
  options[OPT_OUTPUTENC].iType = 0;  
  options[OPT_OUTPUTENC].iMultiFile = 0; 
  fnRead[OPT_OUTPUTENC] = &ReadOutputEnc;
  
  sprintf(options[OPT_TIMEEVOLVELDISP].cName,"bTimeEvolVelDisp");
  sprintf(options[OPT_TIMEEVOLVELDISP].cDescr,"Scale velocity dispersion of stars with sqrt(t)?");
  sprintf(options[OPT_TIMEEVOLVELDISP].cDefault,"1");
  options[OPT_TIMEEVOLVELDISP].dDefault = 1;
  options[OPT_TIMEEVOLVELDISP].iType = 0;  
  options[OPT_TIMEEVOLVELDISP].iMultiFile = 0; 
  fnRead[OPT_TIMEEVOLVELDISP] = &ReadTimeEvolVelDisp;
  
  sprintf(options[OPT_HOSTBINARY].cName,"bHostBinary");
  sprintf(options[OPT_HOSTBINARY].cDescr,"Include 3 body interactions with binary star");
  sprintf(options[OPT_HOSTBINARY].cDefault,"0");
  options[OPT_HOSTBINARY].dDefault = 0;
  options[OPT_HOSTBINARY].iType = 0;  
  options[OPT_HOSTBINARY].iMultiFile = 0; 
  fnRead[OPT_HOSTBINARY] = &ReadHostBinary;
  
  sprintf(options[OPT_GALACTIDES].cName,"bGalacTides");
  sprintf(options[OPT_GALACTIDES].cDescr,"Include galactic tides");
  sprintf(options[OPT_GALACTIDES].cDefault,"1");
  options[OPT_GALACTIDES].dDefault = 1;
  options[OPT_GALACTIDES].iType = 0;  
  options[OPT_GALACTIDES].iMultiFile = 0; 
  fnRead[OPT_GALACTIDES] = &ReadGalacTides;
  
  sprintf(options[OPT_MINALLOWED].cName,"dMinAllowed");
  sprintf(options[OPT_MINALLOWED].cDescr,"Minimum close approach distance to primary");
  sprintf(options[OPT_MINALLOWED].cDefault,"1 AU"); 
  options[OPT_MINALLOWED].dDefault = AUCM;
  options[OPT_MINALLOWED].iType = 2;  
  options[OPT_MINALLOWED].iMultiFile = 0;
  options[OPT_MINALLOWED].dNeg = AUCM;
  sprintf(options[OPT_MINALLOWED].cNeg,"AU");   
  fnRead[OPT_MINALLOWED] = &ReadMinAllowed;
  
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
  
  sprintf(options[OPT_HOSTBINMASS1].cName,"dHostBinMass1");
  sprintf(options[OPT_HOSTBINMASS1].cDescr,"mass of larger host binary star");
  sprintf(options[OPT_HOSTBINMASS1].cDefault,"1.1 Msun"); 
  options[OPT_HOSTBINMASS1].dDefault = 1.1*MSUN;
  options[OPT_HOSTBINMASS1].iType = 2;  
  options[OPT_HOSTBINMASS1].iMultiFile = 0;   
  fnRead[OPT_HOSTBINMASS1] = &ReadHostBinMass1;
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

void InitializeEccXGalHabit(BODY *body,UPDATE *update,int iBody,int iEqn) {
  update[iBody].iaType[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]] = 2;
  update[iBody].padDEccXDtGalHabit[iEqn] = &update[iBody].daDerivProc[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]];
  update[iBody].iNumBodies[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]]=2;
  update[iBody].iaBody[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]] = malloc(update[iBody].iNumBodies[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]][0] = iBody;
  if (iBody == 1) {
    update[iBody].iaBody[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]][1] = 2;
  } else if (iBody == 2) {
    update[iBody].iaBody[update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]][1] = 1;
  }
}

void InitializeEccYGalHabit(BODY *body,UPDATE *update,int iBody,int iEqn) {
  update[iBody].iaType[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]] = 2;
  update[iBody].padDEccYDtGalHabit[iEqn] = &update[iBody].daDerivProc[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]];
  update[iBody].iNumBodies[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]]=2;
  update[iBody].iaBody[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]] = malloc(update[iBody].iNumBodies[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]][0] = iBody;
  if (iBody == 1) {
    update[iBody].iaBody[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]][1] = 2;
  } else if (iBody == 2) {
    update[iBody].iaBody[update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]][1] = 1;
  }
}


void InitializeEccZGalHabit(BODY *body,UPDATE *update,int iBody,int iEqn) {
  update[iBody].iaType[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]] = 2;
  update[iBody].padDEccZDtGalHabit[iEqn] = &update[iBody].daDerivProc[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]];
  update[iBody].iNumBodies[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]]=2;
  update[iBody].iaBody[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]] = malloc(update[iBody].iNumBodies[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]][0] = iBody;
  if (iBody == 1) {
    update[iBody].iaBody[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]][1] = 2;
  } else if (iBody == 2) {
    update[iBody].iaBody[update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]][1] = 1;
  }
}

void InitializeAngMXGalHabit(BODY *body,UPDATE *update,int iBody,int iEqn) {
  update[iBody].iaType[update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]] = 2;
  update[iBody].padDAngMXDtGalHabit[iEqn] = &update[iBody].daDerivProc[update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]];
  update[iBody].iNumBodies[update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]]=2;
  update[iBody].iaBody[update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]] = malloc(update[iBody].iNumBodies[update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]][0] = iBody;
  if (iBody == 1) {
    update[iBody].iaBody[update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]][1] = 2;
  } else if (iBody == 2) {
    update[iBody].iaBody[update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]][1] = 1;
  }
}

void InitializeAngMYGalHabit(BODY *body,UPDATE *update,int iBody,int iEqn) {
  update[iBody].iaType[update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]] = 2;
  update[iBody].padDAngMYDtGalHabit[iEqn] = &update[iBody].daDerivProc[update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]];
  update[iBody].iNumBodies[update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]]=2;
  update[iBody].iaBody[update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]] = malloc(update[iBody].iNumBodies[update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]][0] = iBody;
  if (iBody == 1) {
    update[iBody].iaBody[update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]][1] = 2;
  } else if (iBody == 2) {
    update[iBody].iaBody[update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]][1] = 1;
  }
}

void InitializeAngMZGalHabit(BODY *body,UPDATE *update,int iBody) {
  update[iBody].iaType[update[iBody].iAngMZ][0] = 2;
  update[iBody].padDAngMZDtGalHabit[0] = &update[iBody].daDerivProc[update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[0]];
  update[iBody].iNumBodies[update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[0]]=2;
  update[iBody].iaBody[update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[0]] = malloc(update[iBody].iNumBodies[update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[0]]*sizeof(int));
  update[iBody].iaBody[update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[0]][0] = iBody;
  if (iBody == 1) {
    update[iBody].iaBody[update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[0]][1] = 2;
  } else if (iBody == 2) {
    update[iBody].iaBody[update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[0]][1] = 1;
  }
}

void VerifyTidesBinary(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody,int iVerbose) {
  int i;
  if (body[iBody].bGalacTides == 0 && body[iBody].bHostBinary == 0) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: If using GalHabit, must set %s = 1 and/or %s = 1 in File: %s.\n",options[OPT_GALACTIDES].cName,options[OPT_HOSTBINARY].cName, cFile);
    exit(EXIT_INPUT);
  }
  if (body[iBody].bHostBinary) {
    if (control->Evolve.iNumBodies != 3) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: %s can only be used with exactly 3 bodies in GalHabit\n",options[OPT_HOSTBINARY].cName);
      exit(EXIT_INPUT);
    }
    if (body[1].bHostBinary == 1 && body[2].bHostBinary == 0) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: %s must be called for both body 1 and body 2 in GalHabit\n",options[OPT_HOSTBINARY].cName);
      exit(EXIT_INPUT);
    } else if (body[1].bHostBinary == 0 && body[2].bHostBinary == 1) {
      if (iVerbose >= VERBERR) 
        fprintf(stderr,"ERROR: %s must be called for both body 1 and body 2 in GalHabit\n",options[OPT_HOSTBINARY].cName);
      exit(EXIT_INPUT);
    }
    
    if (body[iBody].bGalacTides) {
      body[iBody].dMassInterior = 0;
      for (i=0;i<iBody;i++) {
        body[iBody].dMassInterior += body[i].dMass;
      }
    }
  }  
}

void VerifyGalHabit(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int i, iEqn;
  int n;
  double dSigma, dDMR, dStarR, dGasR, dCurrentAge;
  char cOut[NAMELEN];
  FILE *fOut;
  
  srand(system->iSeed);
  
  VerifyTidesBinary(body,control,options,files->Infile[iBody+1].cIn,iBody,control->Io.iVerbose);

  if (iBody == 1) {
    body[iBody].dMeanA = 0.0;
    system->dGalaxyAge = 1e10*YEARSEC;
    dCurrentAge = system->dGalaxyAge-control->Evolve.dStopTime;
    system->dPassingStarR = malloc(3*sizeof(double));
    system->dPassingStarV = malloc(3*sizeof(double));
    system->dPassingStarImpact = malloc(3*sizeof(double));
    system->dHostApexVel = malloc(3*sizeof(double));
    system->dRelativeVel = malloc(3*sizeof(double));
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
    if (system->bTimeEvolVelDisp) {
      system->dScalingFVelDisp *= sqrt(dCurrentAge/system->dGalaxyAge);
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
    
    system->dEncounterRateMV = malloc(13*sizeof(double));
    CalcEncounterRate(system);  //need to update this, most likely XXX
    system->dDeltaTEnc = 0.0;
    //system->dMinAllowed = 40.0*AUCM; //set to 40 au for now...
    system->dLastEncTime = 0.0;
    system->dCloseEncTime = 0.0;
    system->iNEncounters = 0;
    NextEncounterTime(system,&control->Evolve,0); 
    
  }
  
  
//   
//   GetStarMass(system);
//   GetStarVelocity(system);
//   GetStarPosition(system);
//   testrand(system);
  
  // for (i=0;i<=10;i++) {
//     n = (int)((double)rand()*20/RAND_MAX)-3;
//     printf("%d\n",n);
//   }
  
  if (iBody >= 1) {
    if (system->bOutputEnc) {
      sprintf(cOut,"%s.%s.Encounters",system->cName,body[iBody].cName);
      fOut = fopen(cOut,"w");
      fprintf(fOut,"#time MV mass sigma impx impy impz u_star v_star w_star u_rel v_rel w_rel u_host v_host w_host Rx Ry Rz bbodyx bbodyy bbodyx vbodyx vbodyy vbodyz rbodyx rbodyy rbodyz\n");
      fclose(fOut);
    }
    
    CalcEccVec(body,iBody);
    CalcAngMVec(body,iBody);
    body[iBody].dCosArgP = cos(body[iBody].dArgP);
    body[iBody].dPeriQ = body[iBody].dSemi*(1.0-body[iBody].dEcc);
    body[iBody].dRelativeImpact = malloc(3*sizeof(double));
    body[iBody].dRelativeVel = malloc(3*sizeof(double));
    
    control->fnPropsAux[iBody][iModule] = &PropertiesGalHabit;
    
    iEqn = 0;
    if (body[iBody].bGalacTides) {
      InitializeEccXGalHabit(body,update,iBody,iEqn);
      fnUpdate[iBody][update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]] = &fdGalHabitDEccXDtTidal;
    
      InitializeEccYGalHabit(body,update,iBody,iEqn);
      fnUpdate[iBody][update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]] = &fdGalHabitDEccYDtTidal;
    
      InitializeEccZGalHabit(body,update,iBody,iEqn);
      fnUpdate[iBody][update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]] = &fdGalHabitDEccZDtTidal;
    
      InitializeAngMXGalHabit(body,update,iBody,iEqn);
      fnUpdate[iBody][update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]] = &fdGalHabitDAngMXDtTidal;

      InitializeAngMYGalHabit(body,update,iBody,iEqn);
      fnUpdate[iBody][update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]] = &fdGalHabitDAngMYDtTidal;
      
      iEqn++;
    }
    
    if (body[iBody].bHostBinary) {
      Rot2Bin(body,iBody);
      InitializeEccXGalHabit(body,update,iBody,iEqn);
      fnUpdate[iBody][update[iBody].iEccX][update[iBody].iaEccXGalHabit[iEqn]] = &fdGalHabitDEccXDtBV;
      
      InitializeEccYGalHabit(body,update,iBody,iEqn);
      fnUpdate[iBody][update[iBody].iEccY][update[iBody].iaEccYGalHabit[iEqn]] = &fdGalHabitDEccYDtBV;
      
      InitializeEccZGalHabit(body,update,iBody,iEqn);
      fnUpdate[iBody][update[iBody].iEccZ][update[iBody].iaEccZGalHabit[iEqn]] = &fdGalHabitDEccZDtBV;
      
      InitializeAngMXGalHabit(body,update,iBody,iEqn);
      fnUpdate[iBody][update[iBody].iAngMX][update[iBody].iaAngMXGalHabit[iEqn]] = &fdGalHabitDAngMXDtBV;
      
      InitializeAngMYGalHabit(body,update,iBody,iEqn);
      fnUpdate[iBody][update[iBody].iAngMY][update[iBody].iaAngMYGalHabit[iEqn]] = &fdGalHabitDAngMYDtBV;
      
      InitializeAngMZGalHabit(body,update,iBody);
      fnUpdate[iBody][update[iBody].iAngMZ][update[iBody].iaAngMZGalHabit[iEqn]] = &fdGalHabitDAngMZDtBV;
    }
    
    control->fnForceBehavior[iBody][iModule]=&ForceBehaviorGalHabit;
    control->Evolve.fnBodyCopy[iBody][iModule]=&BodyCopyGalHabit;
  }
}


/***************** GALHABIT Update *****************/
void InitializeUpdateGalHabit(BODY *body,UPDATE *update,int iBody) {
  if (iBody > 0) {
    if (body[iBody].bGalacTides || body[iBody].bHostBinary) {
      if (update[iBody].iNumEccX == 0)
        update[iBody].iNumVars++;

      if (update[iBody].iNumEccY == 0)
        update[iBody].iNumVars++;
    
      if (update[iBody].iNumEccZ == 0)
        update[iBody].iNumVars++;

      if (update[iBody].iNumAngMX == 0)
        update[iBody].iNumVars++;
    
      if (update[iBody].iNumAngMY == 0)
        update[iBody].iNumVars++;
    }
    
    if (body[iBody].bGalacTides) {
      update[iBody].iNumEccX += 1;
      update[iBody].iNumEccY += 1;
      update[iBody].iNumEccZ += 1;
      update[iBody].iNumAngMX += 1;
      update[iBody].iNumAngMY += 1;
    }
    
    if (body[iBody].bHostBinary) {
      update[iBody].iNumEccX += 1;
      update[iBody].iNumEccY += 1;
      update[iBody].iNumEccZ += 1;
      update[iBody].iNumAngMX += 1;
      update[iBody].iNumAngMY += 1;
      if (update[iBody].iNumAngMZ == 0)
        update[iBody].iNumVars++;
      update[iBody].iNumAngMZ += 1;
    }
  }
}

void FinalizeUpdateEccXGalHabit(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
    if (body[iBody].bHostBinary && body[iBody].bGalacTides) {
      update[iBody].padDEccXDtGalHabit = malloc(2*sizeof(double*));
      update[iBody].iaEccXGalHabit = malloc(2*sizeof(int));
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaEccXGalHabit[0] = (*iEqn)++;
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaEccXGalHabit[1] = (*iEqn)++;
    } else {
      update[iBody].padDEccXDtGalHabit = malloc(1*sizeof(double*));
      update[iBody].iaEccXGalHabit = malloc(1*sizeof(int));
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaEccXGalHabit[0] = (*iEqn)++;
    }
}

void FinalizeUpdateEccYGalHabit(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
    if (body[iBody].bHostBinary && body[iBody].bGalacTides) {
      update[iBody].padDEccYDtGalHabit = malloc(2*sizeof(double*));
      update[iBody].iaEccYGalHabit = malloc(2*sizeof(int));
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaEccYGalHabit[0] = (*iEqn)++;
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaEccYGalHabit[1] = (*iEqn)++;
    } else {
      update[iBody].padDEccYDtGalHabit = malloc(1*sizeof(double*));
      update[iBody].iaEccYGalHabit = malloc(1*sizeof(int));
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaEccYGalHabit[0] = (*iEqn)++;
    }
}

void FinalizeUpdateEccZGalHabit(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
    if (body[iBody].bHostBinary && body[iBody].bGalacTides) {
      update[iBody].padDEccZDtGalHabit = malloc(2*sizeof(double*));
      update[iBody].iaEccZGalHabit = malloc(2*sizeof(int));
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaEccZGalHabit[0] = (*iEqn)++;
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaEccZGalHabit[1] = (*iEqn)++;
    } else {
      update[iBody].padDEccZDtGalHabit = malloc(1*sizeof(double*));
      update[iBody].iaEccZGalHabit = malloc(1*sizeof(int));
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaEccZGalHabit[0] = (*iEqn)++;
    }
}

void FinalizeUpdateAngMXGalHabit(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
    if (body[iBody].bHostBinary && body[iBody].bGalacTides) {
      update[iBody].padDAngMXDtGalHabit = malloc(2*sizeof(double*));
      update[iBody].iaAngMXGalHabit = malloc(2*sizeof(int));
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaAngMXGalHabit[0] = (*iEqn)++;
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaAngMXGalHabit[1] = (*iEqn)++;
    } else {
      update[iBody].padDAngMXDtGalHabit = malloc(1*sizeof(double*));
      update[iBody].iaAngMXGalHabit = malloc(1*sizeof(int));
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaAngMXGalHabit[0] = (*iEqn)++;
    }
}

void FinalizeUpdateAngMYGalHabit(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
    if (body[iBody].bHostBinary && body[iBody].bGalacTides) {
      update[iBody].padDAngMYDtGalHabit = malloc(2*sizeof(double*));
      update[iBody].iaAngMYGalHabit = malloc(2*sizeof(int));
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaAngMYGalHabit[0] = (*iEqn)++;
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaAngMYGalHabit[1] = (*iEqn)++;
    } else {
      update[iBody].padDAngMYDtGalHabit = malloc(1*sizeof(double*));
      update[iBody].iaAngMYGalHabit = malloc(1*sizeof(int));
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaAngMYGalHabit[0] = (*iEqn)++;
    }
}

void FinalizeUpdateAngMZGalHabit(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
    if (body[iBody].bHostBinary) {
      update[iBody].padDAngMZDtGalHabit = malloc(1*sizeof(double*));
      update[iBody].iaAngMZGalHabit = malloc(1*sizeof(int));
      update[iBody].iaModule[iVar][*iEqn] = GALHABIT;
      update[iBody].iaAngMZGalHabit[0] = (*iEqn)++;
    }
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
 
  *dTmp = body[iBody].dSemi*(1.0-body[iBody].dEcc);
  
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

void WriteFVelDisp(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
 
  *dTmp = system->dScalingFVelDisp;
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
  
  sprintf(output[OUT_FVELDISP].cName,"FVelDisp");
  sprintf(output[OUT_FVELDISP].cDescr,"Scaling factor for velocity dispersion");
  sprintf(output[OUT_FVELDISP].cNeg," ");
  output[OUT_FVELDISP].bNeg = 0;
  output[OUT_FVELDISP].iNum = 1;
  output[OUT_FVELDISP].iModuleBit = GALHABIT;
  fnWrite[OUT_FVELDISP] = &WriteFVelDisp;
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
  module->fnFinalizeUpdateEccX[iBody][iModule] = &FinalizeUpdateEccXGalHabit;
  module->fnFinalizeUpdateEccY[iBody][iModule] = &FinalizeUpdateEccYGalHabit;
  module->fnFinalizeUpdateEccZ[iBody][iModule] = &FinalizeUpdateEccZGalHabit;
  module->fnFinalizeUpdateAngMX[iBody][iModule] = &FinalizeUpdateAngMXGalHabit;
  module->fnFinalizeUpdateAngMY[iBody][iModule] = &FinalizeUpdateAngMYGalHabit;
  module->fnFinalizeUpdateAngMZ[iBody][iModule] = &FinalizeUpdateAngMZGalHabit;
}

/************* GALHABIT Functions ***********/
void PropertiesGalHabit(BODY *body,EVOLVE *evolve,UPDATE *update,int iBody) { 
  double sinw, cosw, cosw_alt, sign;
  
  /* calculate osculating elements */
  body[iBody].dEcc = sqrt(pow(body[iBody].dEccX,2)+pow(body[iBody].dEccY,2)+pow(body[iBody].dEccZ,2));
  body[iBody].dAngM = sqrt(pow(body[iBody].dAngMX,2)+pow(body[iBody].dAngMY,2)+\
                                                          pow(body[iBody].dAngMZ,2));
  
  body[iBody].dInc = acos(body[iBody].dAngMZ/body[iBody].dAngM);
  body[iBody].dLongA = atan2(body[iBody].dAngMX,-body[iBody].dAngMY);
  
  sinw = -body[iBody].dEccX*body[iBody].dAngMX*body[iBody].dAngMZ \
         -body[iBody].dEccY*body[iBody].dAngMY*body[iBody].dAngMZ \
         +body[iBody].dEccZ*(pow(body[iBody].dAngMX,2)+pow(body[iBody].dAngMY,2));
  cosw = body[iBody].dAngM*(-body[iBody].dEccX*body[iBody].dAngMY \
                            +body[iBody].dEccY*body[iBody].dAngMX);
  body[iBody].dCosArgP = cosw*body[iBody].dAngM/(body[iBody].dEcc*\
                          sqrt(pow(body[iBody].dAngMX,2)+pow(body[iBody].dAngMY,2)));
  
  body[iBody].dArgP = atan2(sinw,cosw);
  
  // if (body[iBody].bHostBinary) {
//     Rot2Bin(body,iBody);
//   }
  
  body[iBody].dLongP = body[iBody].dLongA + body[iBody].dArgP; 
  while (body[iBody].dArgP > 2*PI) {
    body[iBody].dArgP -= 2*PI;
  }
  while (body[iBody].dArgP < 0) {
    body[iBody].dArgP += 2*PI;
  }
  while (body[iBody].dLongP > 2*PI) {
    body[iBody].dLongP -= 2*PI;
  }
  while (body[iBody].dLongP < 0) {
    body[iBody].dLongP += 2*PI;
  }
  while (body[iBody].dLongA > 2*PI) {
    body[iBody].dLongA -= 2*PI;
  }
  while (body[iBody].dLongA < 0) {
    body[iBody].dLongA += 2*PI;
  }
  
//   body[iBody].dEcc = 1.0 - body[iBody].dPeriQ/body[iBody].dSemi;
  
}

void ForceBehaviorGalHabit(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  double dp, dkzi, dVMax, dCurrentAge;
  double sinw, cosw, cosw_alt, sign;
  char cOut[NAMELEN];
  FILE *fOut;
  
  dCurrentAge = system->dGalaxyAge-evolve->dStopTime+evolve->dTime;
  if (system->bTimeEvolVelDisp) {
    system->dScalingFVelDisp *= sqrt((dCurrentAge+evolve->dCurrentDt)/dCurrentAge);
  }

  if (system->bRadialMigr) {
    if (evolve->dTime >= system->dTMigration) {
      // time of migration passed? move to solar neighborhood
      system->dScalingFTot = 1.0; 
      system->dScalingFStars = 1.0;  
      if (system->bTimeEvolVelDisp) {  
        system->dScalingFVelDisp = sqrt(dCurrentAge/system->dGalaxyAge);
      } else {
        system->dScalingFVelDisp = 1.0;
      }
      CalcEncounterRate(system);
      system->bRadialMigr = 0;  //don't recalculate this stuff again
    }
  }
  
  /* calculate osculating elements */
  body[iBody].dEcc = sqrt(pow(body[iBody].dEccX,2)+pow(body[iBody].dEccY,2)+pow(body[iBody].dEccZ,2));
  body[iBody].dAngM = sqrt(pow(body[iBody].dAngMX,2)+pow(body[iBody].dAngMY,2)+\
                                                          pow(body[iBody].dAngMZ,2));
  
  body[iBody].dInc = acos(body[iBody].dAngMZ/body[iBody].dAngM);
  body[iBody].dLongA = atan2(body[iBody].dAngMX,-body[iBody].dAngMY);
  
  while (body[iBody].dLongA > 2*PI) {
    body[iBody].dLongA -= 2*PI;
  }
  while (body[iBody].dLongA < 0) {
    body[iBody].dLongA += 2*PI;
  }
  
  sinw = -body[iBody].dEccX*body[iBody].dAngMX*body[iBody].dAngMZ \
         -body[iBody].dEccY*body[iBody].dAngMY*body[iBody].dAngMZ \
         +body[iBody].dEccZ*(pow(body[iBody].dAngMX,2)+pow(body[iBody].dAngMY,2));
  cosw = body[iBody].dAngM*(-body[iBody].dEccX*body[iBody].dAngMY \
                            +body[iBody].dEccY*body[iBody].dAngMX);
  body[iBody].dCosArgP = cosw*body[iBody].dAngM/(body[iBody].dEcc*\
                          sqrt(pow(body[iBody].dAngMX,2)+pow(body[iBody].dAngMY,2)));
                          
  body[iBody].dArgP = atan2(sinw,cosw);
  
 //  if (body[iBody].dArgP > PI/2.) {
//     printf("stop here\n");
//   }
  
  while (body[iBody].dArgP > 2*PI) {
    body[iBody].dArgP -= 2*PI;
  }
  while (body[iBody].dArgP < 0) {
    body[iBody].dArgP += 2*PI;
  }
  
  // if (body[iBody].bHostBinary) {
//     Rot2Bin(body,iBody);
//   }

  body[iBody].iDisrupt = check_disrupt(body,system,iBody);

  if (evolve->dTime + evolve->dCurrentDt >= system->dNextEncT) {
    system->dCloseEncTime = evolve->dTime + evolve->dCurrentDt;
    GetStarPosition(system);
    GetStarMass(system);
    system->dRelativeVelRad = 1.0;
    system->dRelativeVelMag = 1.0;
    dkzi = 10.0;
    dVMax = 1.0;
    while (dkzi > system->dRelativeVelMag/dVMax || system->dRelativeVelRad >= 0) {
      GetStarVelocity(system); 
      GetRelativeVelocity(system);
      dkzi = random_double();
      dVMax = system->dHostApexVelMag + 3.0*system->dPassingStarSigma*1000.0;
    }
    
    /* next calculate impact parameter */
    CalcImpactParam(system); 
    
    /* then move the orbiter, get all distances/velocities, check for disruption */
    AdvanceMA(body,system,iBody);
    body[iBody].dSinc = sin(0.5*body[iBody].dInc);
    osc2cart(body,evolve->iNumBodies); //maybe need to convert to barycentric? XXX
  
    /* apply the impulse */
    ApplyDeltaV(body,system,iBody);
    cart2osc(body,evolve->iNumBodies);
    body[iBody].dInc = 2*asin(body[iBody].dSinc);
    body[iBody].dPeriQ = body[iBody].dSemi*(1.0-body[iBody].dEcc);
    body[iBody].dMeanMotion = fdSemiToMeanMotion(body[iBody].dSemi,body[iBody].dMassInterior+body[iBody].dMass);
    CalcEccVec(body,iBody);
    CalcAngMVec(body,iBody);
    
    system->dLastEncTime = system->dCloseEncTime;
    system->iNEncounters += 1;
    NextEncounterTime(system,evolve,system->dCloseEncTime);
  
    /* write out encounter info */
    if (system->bOutputEnc) {
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
      fprintf(fOut," ");
      fprintd(fOut,body[iBody].dRelativeImpact[0],4,6);
      fprintf(fOut," ");
      fprintd(fOut,body[iBody].dRelativeImpact[1],4,6);
      fprintf(fOut," ");
      fprintd(fOut,body[iBody].dRelativeImpact[2],4,6);
      fprintf(fOut," ");
      fprintd(fOut,body[iBody].dRelativeVel[0],4,6);
      fprintf(fOut," ");
      fprintd(fOut,body[iBody].dRelativeVel[1],4,6);
      fprintf(fOut," ");
      fprintd(fOut,body[iBody].dRelativeVel[2],4,6);
      fprintf(fOut," ");

      fprintd(fOut,body[iBody].dCartPos[0]*AUCM,4,6);
      fprintf(fOut," ");
      fprintd(fOut,body[iBody].dCartPos[1]*AUCM,4,6);
      fprintf(fOut," ");
      fprintd(fOut,body[iBody].dCartPos[2]*AUCM,4,6);
    
      fprintf(fOut,"\n");
    
      fclose(fOut);
    }
  }
}

void Rot2Bin(BODY *body, int iBody) {
  double sinw, cosw; 
  
  body[iBody].dEccXTmp = body[iBody].dEccX*cos(body[iBody].dHostBinLongA) + \
                            body[iBody].dEccY*sin(body[iBody].dHostBinLongA);
  body[iBody].dEccYTmp = -body[iBody].dEccX*cos(body[iBody].dHostBinInc)\
                            *sin(body[iBody].dHostBinLongA)\
                            +body[iBody].dEccY*cos(body[iBody].dHostBinInc)\
                            *cos(body[iBody].dHostBinLongA)\
                            +body[iBody].dEccZ*sin(body[iBody].dHostBinInc); 
  body[iBody].dEccZTmp = body[iBody].dEccX*sin(body[iBody].dHostBinInc)*sin(body[iBody].dHostBinLongA)\
                            -body[iBody].dEccY*sin(body[iBody].dHostBinInc)\
                            *cos(body[iBody].dHostBinLongA)\
                            +body[iBody].dEccZ*cos(body[iBody].dHostBinInc);   
  
  body[iBody].dAngMXTmp = body[iBody].dAngMX*cos(body[iBody].dHostBinLongA) + \
                            body[iBody].dAngMY*sin(body[iBody].dHostBinLongA);
  body[iBody].dAngMYTmp = -body[iBody].dAngMX*cos(body[iBody].dHostBinInc)\
                            *sin(body[iBody].dHostBinLongA)\
                            +body[iBody].dAngMY*cos(body[iBody].dHostBinInc)*\
                            cos(body[iBody].dHostBinLongA)\
                            +body[iBody].dAngMZ*sin(body[iBody].dHostBinInc); 
  body[iBody].dAngMZTmp = body[iBody].dAngMX*sin(body[iBody].dHostBinInc)\
                            *sin(body[iBody].dHostBinLongA)\
                            -body[iBody].dAngMY*sin(body[iBody].dHostBinInc)\
                            *cos(body[iBody].dHostBinLongA)\
                            +body[iBody].dAngMZ*cos(body[iBody].dHostBinInc);  
                            
  /* calculate osculating elements */
//   body[iBody].dEcc = sqrt(pow(body[iBody].dEccX,2)+pow(body[iBody].dEccY,2)+pow(body[iBody].dEccZ,2));
//   body[iBody].dAngM = sqrt(pow(body[iBody].dAngMX,2)+pow(body[iBody].dAngMY,2)+\
                                                          pow(body[iBody].dAngMZ,2));
  
  body[iBody].dIncTmp = acos(body[iBody].dAngMZTmp/body[iBody].dAngM);
  body[iBody].dLongATmp = atan2(body[iBody].dAngMXTmp,-body[iBody].dAngMYTmp);
  
  sinw = -body[iBody].dEccXTmp*body[iBody].dAngMXTmp*body[iBody].dAngMZTmp \
         -body[iBody].dEccYTmp*body[iBody].dAngMYTmp*body[iBody].dAngMZTmp \
         +body[iBody].dEccZTmp*(pow(body[iBody].dAngMXTmp,2)+pow(body[iBody].dAngMYTmp,2));
  cosw = body[iBody].dAngM*(-body[iBody].dEccXTmp*body[iBody].dAngMYTmp \
                            +body[iBody].dEccYTmp*body[iBody].dAngMXTmp);
//   body[iBody].dCosArgP = cosw*body[iBody].dAngM/(body[iBody].dEcc*\
                          sqrt(pow(body[iBody].dAngMX,2)+pow(body[iBody].dAngMY,2)));
  
  body[iBody].dArgPTmp = atan2(sinw,cosw);    
  
    
  while (body[iBody].dLongATmp > 2*PI) {
    body[iBody].dLongATmp -= 2*PI;
  }
  while (body[iBody].dLongATmp < 0) {
    body[iBody].dLongATmp += 2*PI;
  }
           
  while (body[iBody].dArgPTmp > 2*PI) {
    body[iBody].dArgPTmp -= 2*PI;
  }
  while (body[iBody].dArgPTmp < 0) {
    body[iBody].dArgPTmp += 2*PI;
  }                       
}  

// int Rot2Bin(BODY *body, SYSTEM *system, int iBody) {                                                 
void CalcEccVec(BODY *body, int iBody) {
  body[iBody].dEccX = body[iBody].dEcc*(cos(body[iBody].dLongA)*cos(body[iBody].dArgP) - \
                    sin(body[iBody].dLongA)*sin(body[iBody].dArgP)*cos(body[iBody].dInc));
  body[iBody].dEccY = body[iBody].dEcc*(sin(body[iBody].dLongA)*cos(body[iBody].dArgP) + \
                    cos(body[iBody].dLongA)*sin(body[iBody].dArgP)*cos(body[iBody].dInc));
  body[iBody].dEccZ = body[iBody].dEcc*sin(body[iBody].dArgP)*sin(body[iBody].dInc);
} 

void CalcAngMVec(BODY *body, int iBody) {
  body[iBody].dAngM = sqrt((1.0-pow(body[iBody].dEcc,2)));
  body[iBody].dAngMX = body[iBody].dAngM*(sin(body[iBody].dLongA)*sin(body[iBody].dInc));
  body[iBody].dAngMY = -body[iBody].dAngM*(cos(body[iBody].dLongA)*sin(body[iBody].dInc));
  body[iBody].dAngMZ = body[iBody].dAngM*cos(body[iBody].dInc);
}                                           

int check_disrupt(BODY* body, SYSTEM *system, int iBody) {
  /* rather crude for the time being. checks that pericenter and apocenter are ok and 
     e < 1 */
  double apo, peri;
  
  apo = body[iBody].dSemi*(1.0+body[iBody].dEcc);
  peri = body[iBody].dSemi*(1.0-body[iBody].dEcc);
  
  if (peri < body[iBody].dMinAllowed) {
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
  dSigma = system->dPassingStarSigma/sqrt(3.);  //sqrt(3) to account for 3 dimensions
  
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
    dVRel = sqrt(pow(system->dHostApexVelMag/1000,2)+pow(system->dPassingStarSigma,2));
    
    system->dEncounterRateMV[i] = PI*pow(system->dEncounterRad,2)*dVRel*1000*dn*pow(AUCM*206265,-3.0)*YEARSEC*1e6;
    dEncR += dVRel*1000*dn*pow(AUCM*206265,-3.0);
  }
  
  dEncR *= PI*pow(system->dEncounterRad,2);
  
  system->dEncounterRate = dEncR;
}

void GetStarMass(SYSTEM *system) {
  double fs = 0, dTmp = 100, dMagV, dMaxN;
  dMaxN = system->dEncounterRateMV[12];
  
  while (dTmp > fs) {
    //dMagV = (double)(random_int(20)-4); //draw stellar magnitude (-3<dMagV<15)
    dMagV = (random_double()*25.7-7.7);
    dTmp = random_double()*dMaxN;       //if dTmp exceeds the frequency, reject dMagV
    fs = NearbyStarFrEnc(system,dMagV);         //get frequency at dMagV
  }
  
  system->dPassingStarMagV = dMagV;
  //now get the mass of the star
  system->dPassingStarMass = mag2mass(dMagV)*MSUN;
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
  
  system->dPassingStarSigma = system->dScalingFVelDisp*dSigma;
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
  
  dVel *= 1000.0;
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
    dNs = 1.42/w;
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

double NearbyStarFrEnc(SYSTEM* system, double dMagV) {
  double dFs, w;
  
  if (dMagV >= -5.7 && dMagV <= -0.2) {
    dFs = system->dEncounterRateMV[2]/(5.7-0.2);
  } else if (dMagV > -0.2 && dMagV <= 1.3) {
    dFs = system->dEncounterRateMV[3]/(1.3+0.2);
  } else if (dMagV > 1.3 && dMagV <= 2.4) {
    dFs = system->dEncounterRateMV[4]/(2.4-1.3);
  } else if (dMagV > 2.4 && dMagV <= 3.6) {
    dFs = system->dEncounterRateMV[5]/(3.6-2.4);
  } else if (dMagV > 3.6 && dMagV <= 4.0) {
    dFs = system->dEncounterRateMV[6]/(4.0-3.6);
  } else if (dMagV > 4.0 && dMagV <= 4.7) {
    dFs = system->dEncounterRateMV[7]/(4.7-4.);
  } else if (dMagV > 4.7 && dMagV <= 5.5) {
    dFs = system->dEncounterRateMV[8]/(5.5-4.7);
  } else if (dMagV > 5.5 && dMagV <= 6.4) {
    dFs = system->dEncounterRateMV[9]/(6.4-5.5);
  } else if (dMagV > 6.4 && dMagV <= 8.1) {
    dFs = system->dEncounterRateMV[10]/(8.1-6.4);
  } else if (dMagV > 8.1 && dMagV <= 9.9) {
    dFs = system->dEncounterRateMV[11]/(9.9-8.1);
  } else if (dMagV > 9.9) {
    dFs = system->dEncounterRateMV[12]/(18.0-9.9);
  } else if (dMagV < -5.7 && dMagV >= -6.7) {
    dFs = system->dEncounterRateMV[1];  //white dwarfs
  } else if (dMagV < -6.7) {
    dFs = system->dEncounterRateMV[0];  //giants
  }
  return dFs;
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
                       
  body[iBody].dRelativeImpact[0] = dRelativeImpactx;
  body[iBody].dRelativeImpact[1] = dRelativeImpacty;
  body[iBody].dRelativeImpact[2] = dRelativeImpactz;

  dRelativeVx = system->dRelativeVel[0]-body[iBody].dCartVel[0]*AUCM/DAYSEC;
  dRelativeVy = system->dRelativeVel[1]-body[iBody].dCartVel[1]*AUCM/DAYSEC;
  dRelativeVz = system->dRelativeVel[2]-body[iBody].dCartVel[2]*AUCM/DAYSEC;

  body[iBody].dRelativeVel[0] = dRelativeVx;
  body[iBody].dRelativeVel[1] = dRelativeVy;
  body[iBody].dRelativeVel[2] = dRelativeVz;

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

void NextEncounterTime(SYSTEM *system, EVOLVE *evolve, double dTime) {
  double dp;
  
  if (system->bStellarEnc) {
    dp = random_double();
    system->dNextEncT = dTime - log(dp)/system->dEncounterRate;
  } else {
    system->dNextEncT = evolve->dStopTime*1.10;
  }
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

//--------------Chain rule derivatives for variable transformations-----------------------------
double dexdJ(BODY *body, int iBody) {  
  return (-sqrt((1.0-pow(body[iBody].dEcc,2.))) \
             *body[iBody].dEccX/pow(body[iBody].dEcc,2)*pow(sin(body[iBody].dInc),2) \
          + body[iBody].dEccZ/(sqrt((1.0-pow(body[iBody].dEcc,2.))))* \
             sin(body[iBody].dLongA)*cos(body[iBody].dInc)*sin(body[iBody].dInc));
}

double dexdla(double dEccY) {
  return -dEccY;
}

double dexdap(double dEccY, double dEccZ, double dInc, double dLongA) {
  return -dEccZ*cos(dLongA)*sin(dInc) - dEccY*cos(dInc);
}

double deydJ(BODY *body, int iBody) {  
  return (-sqrt((1.0-pow(body[iBody].dEcc,2.))) \
             *body[iBody].dEccY/pow(body[iBody].dEcc,2)*pow(sin(body[iBody].dInc),2) \
          - body[iBody].dEccZ/(sqrt((1.0-pow(body[iBody].dEcc,2.))))* \
             cos(body[iBody].dLongA)*cos(body[iBody].dInc)*sin(body[iBody].dInc));
}

double deydla(double dEccX) {
  return dEccX;
}

double deydap(double dEccX, double dEccZ, double dInc, double dLongA) {
  return dEccX*cos(dInc) - dEccZ*sin(dLongA)*sin(dInc);
}

double dezdJ(BODY *body, int iBody) {
  return (pow(body[iBody].dEcc,2)-pow(sin(body[iBody].dInc),2))*body[iBody].dEccZ /\
          (sqrt((1.0-pow(body[iBody].dEcc,2))) * \
              pow(body[iBody].dEcc,2));
}

double dezdap(double dArgP, double dEcc, double dInc) {  
  return dEcc*cos(dArgP)*sin(dInc); 
}  

//--------------Galactic tides!--------------------------------------------------------------

double fdGalHabitDJDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dRho = system->dScalingFTot*system->dGalacDensity/pow(AUPC,3), dMu, dL;
  dMu = KGAUSS*KGAUSS*(body[iaBody[0]].dMassInterior+body[iaBody[0]].dMass)/MSUN; //calculate mass coefficient for primary/primary+secondary
  dL = sqrt(dMu*body[iaBody[0]].dSemi/AUCM);

  return -5.0*PI*KGAUSS*KGAUSS*dRho*\
          pow(body[iaBody[0]].dSemi/AUCM*body[iaBody[0]].dEcc,2.) * \
          sin(2*body[iaBody[0]].dArgP)/DAYSEC/dL;
  return 0.;
}

double fdGalHabitDPeriQDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dRho = system->dScalingFTot*system->dGalacDensity/pow(AUPC,3), dMu, dEcc;
  dMu = KGAUSS*KGAUSS*(body[iaBody[0]].dMassInterior+body[iaBody[0]].dMass)/MSUN; //calculate mass coefficient for primary/primary+secondary
  dEcc = 1.0 - body[iaBody[0]].dPeriQ/body[iaBody[0]].dSemi; //calculate orbiter's eccentricity
  
  return -pow(body[iaBody[0]].dSemi/AUCM,5./2)*sqrt((1.0-pow(dEcc,2))/dMu)*5*PI*KGAUSS*KGAUSS*\
      dRho*dEcc*pow(sin(body[iaBody[0]].dInc),2)*sin(2*body[iaBody[0]].dArgP)/DAYSEC*AUCM;
}

double fdGalHabitDArgPDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dRho = system->dScalingFTot*system->dGalacDensity/pow(AUPC,3), dMu, dEcc;
  dMu = KGAUSS*KGAUSS*(body[iaBody[0]].dMassInterior+body[iaBody[0]].dMass)/MSUN; //calculate mass coefficient for primary/primary+secondary
  dEcc = 1.0 - body[iaBody[0]].dPeriQ/body[iaBody[0]].dSemi; //calculate orbiter's eccentricity

  return 2*PI*KGAUSS*KGAUSS*dRho*sqrt(pow(body[iaBody[0]].dSemi/AUCM,3)/(dMu*(1.0-pow(dEcc,2))))*\
      (1.-pow(dEcc,2)-5.*(1.-pow(dEcc,2)-pow(cos(body[iaBody[0]].dInc),2))*\
      pow(sin(body[iaBody[0]].dArgP),2.0))/DAYSEC;
}

double fdGalHabitDIncDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dRho = system->dScalingFTot*system->dGalacDensity/pow(AUPC,3), dMu, dEcc, dL, dJ, dJz;
  dMu = KGAUSS*KGAUSS*(body[iaBody[0]].dMassInterior+body[iaBody[0]].dMass)/MSUN; //calculate mass coefficient for primary/primary+secondary
  dEcc = 1.0 - body[iaBody[0]].dPeriQ/body[iaBody[0]].dSemi; //calculate orbiter's eccentricity
  dL = sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
  dJ = dL*sqrt(1.0-pow(dEcc,2));
  dJz = dJ*cos(body[iaBody[0]].dInc);
  
  return (-5.*PI*KGAUSS*KGAUSS*dRho/pow(dMu,2)*pow(dL,2)/pow(dJ,3)*dJz*sqrt(pow(dJ,2)-pow(dJz,2))*\
      (pow(dL,2)-pow(dJ,2))*sin(2*body[iaBody[0]].dArgP))/DAYSEC;
}

double fdGalHabitDLongADt(BODY *body, SYSTEM *system, int *iaBody) {
  double dRho = system->dScalingFTot*system->dGalacDensity/pow(AUPC,3), dMu, dEcc, dL, dJ, dJz;
  dMu = KGAUSS*KGAUSS*(body[iaBody[0]].dMassInterior+body[iaBody[0]].dMass)/MSUN; //calculate mass coefficient for primary/primary+secondary
  dEcc = 1.0 - body[iaBody[0]].dPeriQ/body[iaBody[0]].dSemi; //calculate orbiter's eccentricity
  dL = sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
  dJ = dL*sqrt(1.0-pow(dEcc,2));
  dJz = dJ*cos(body[iaBody[0]].dInc);
  
  return -2.*PI*KGAUSS*KGAUSS*dRho/pow(dMu,2)*pow(dL/dJ,2)*dJz*\
      (pow(dJ,2)+5.*(pow(dL,2)-pow(dJ,2))*pow(sin(body[iaBody[0]].dArgP),2))/DAYSEC;
}

double fdGalHabitDEccXDtTidal(BODY *body, SYSTEM *system, int *iaBody) {
  return dexdJ(body,iaBody[0])*fdGalHabitDJDt(body,system,iaBody) + \
         dexdla(body[iaBody[0]].dEccY)*fdGalHabitDLongADt(body,system,iaBody) + \
         dexdap(body[iaBody[0]].dEccY,body[iaBody[0]].dEccZ,body[iaBody[0]].dInc,body[iaBody[0]].dLongA)*fdGalHabitDArgPDt(body,system,iaBody);
}

double fdGalHabitDEccYDtTidal(BODY *body, SYSTEM *system, int *iaBody) {
  return deydJ(body,iaBody[0])*fdGalHabitDJDt(body,system,iaBody) + \
         deydla(body[iaBody[0]].dEccX)*fdGalHabitDLongADt(body,system,iaBody) + \
         deydap(body[iaBody[0]].dEccX,body[iaBody[0]].dEccZ,body[iaBody[0]].dInc, body[iaBody[0]].dLongA)*fdGalHabitDArgPDt(body,system,iaBody);
}

double fdGalHabitDEccZDtTidal(BODY *body, SYSTEM *system, int *iaBody) {
  return dezdJ(body,iaBody[0])*fdGalHabitDJDt(body,system,iaBody) + \
         dezdap(body[iaBody[0]].dArgP,body[iaBody[0]].dEcc, body[iaBody[0]].dInc)*fdGalHabitDArgPDt(body,system,iaBody);
}

double fdGalHabitDAngMXDtTidal(BODY *body, SYSTEM *system, int *iaBody) {
  double dMu, dJ;
  dMu = KGAUSS*KGAUSS*(body[iaBody[0]].dMassInterior+body[iaBody[0]].dMass)/MSUN; 
  dJ = sqrt((1.0-pow(body[iaBody[0]].dEcc,2)));
    
  return sin(body[iaBody[0]].dLongA)*sin(body[iaBody[0]].dInc)*fdGalHabitDJDt(body,system,iaBody) + \
       dJ*sin(body[iaBody[0]].dInc)*cos(body[iaBody[0]].dLongA)*fdGalHabitDLongADt(body,system,iaBody);
}

double fdGalHabitDAngMYDtTidal(BODY *body, SYSTEM *system, int *iaBody) {
  double dMu, dJ;
  dMu = KGAUSS*KGAUSS*(body[iaBody[0]].dMassInterior+body[iaBody[0]].dMass)/MSUN; 
  dJ = sqrt((1.0-pow(body[iaBody[0]].dEcc,2)));
    
  return -cos(body[iaBody[0]].dLongA)*sin(body[iaBody[0]].dInc)*fdGalHabitDJDt(body,system,iaBody) + \
      dJ*sin(body[iaBody[0]].dInc)*sin(body[iaBody[0]].dLongA)*fdGalHabitDLongADt(body,system,iaBody);
}

double fdGalHabitDAngMZDtTidal(BODY *body, SYSTEM *system, int *iaBody) {
  return 0; //need to get rid of this eventually!!!!
}

//------------Binary quadrupole moment --------------------------------------------------
//------------Supplementary equations-------------------------------------
double HostBinA0(BODY *body,int iBody) {
  return 0.5*pow(body[iBody].dHostBinSemi/AUCM,2)*(2.0+3.0*pow(body[iBody].dHostBinEcc,2));
}

double HostBinA1(BODY *body,int iBody) {
  return 0.5*pow(body[iBody].dHostBinSemi/AUCM,2)*(1.0+4.0*pow(body[iBody].dHostBinEcc,2));
}

double HostBinA2(BODY *body,int iBody) {
  return 0.5*pow(body[iBody].dHostBinSemi/AUCM,2)*(1.0-pow(body[iBody].dHostBinEcc,2));
}

double BodyDC0De(BODY *body,int iBody) {
  return 3.0/(pow(body[iBody].dSemi/AUCM,3)*pow(1.0-pow(body[iBody].dEcc,2),2.5));
}

double BodyDC1De(BODY *body,int iBody) {
  return 3.0/(8*pow(body[iBody].dSemi/AUCM,3)*pow(1.0-pow(body[iBody].dEcc,2),2.5))\
    *(3.0+cos(2*body[iBody].dIncTmp)+2*cos(2*body[iBody].dLongATmp)*pow(sin(body[iBody].dIncTmp),2));
}

double BodyDC2De(BODY *body,int iBody) {
  return 3.0/(8*pow(body[iBody].dSemi/AUCM,3)*pow(1.0-pow(body[iBody].dEcc,2),2.5))\
    *(3.0+cos(2*body[iBody].dIncTmp)-2*cos(2*body[iBody].dLongATmp)*pow(sin(body[iBody].dIncTmp),2));
}

double BodyDC1Di(BODY *body,int iBody) {
  return (cos(body[iBody].dIncTmp)*(cos(2*body[iBody].dLongATmp)-1))\
          /(2.*pow(body[iBody].dSemi/AUCM,3)*pow(1.0-pow(body[iBody].dEcc,2),1.5));
}

double BodyDC2Di(BODY *body,int iBody) {
  return (-cos(body[iBody].dIncTmp)*(cos(2*body[iBody].dLongATmp)+1))\
          /(2.*pow(body[iBody].dSemi/AUCM,3)*pow(1.0-pow(body[iBody].dEcc,2),1.5));
}

double BodyDC1Dla(BODY *body,int iBody) {
  return (-sin(2*body[iBody].dLongATmp)*sin(body[iBody].dIncTmp))\
          /(2.*pow(body[iBody].dSemi/AUCM,3)*pow(1.0-pow(body[iBody].dEcc,2),1.5));
}

double BodyDC2Dla(BODY *body,int iBody) {
  return (sin(2*body[iBody].dLongATmp)*sin(body[iBody].dIncTmp))\
          /(2.*pow(body[iBody].dSemi/AUCM,3)*pow(1.0-pow(body[iBody].dEcc,2),1.5));
}

//------------Derivatives------------------------------------------------
double fdGalHabitDLongADtQuad0(BODY *body, SYSTEM *system, int *iaBody) {
  double dMu;
  dMu = KGAUSS*KGAUSS*(body[iaBody[0]].dHostBinMass1*(body[0].dMass-\
        body[iaBody[0]].dHostBinMass1))/(body[0].dMass)/MSUN;
              
  return 3*dMu*(HostBinA1(body,iaBody[0])*BodyDC1Di(body,iaBody[0])+\
          HostBinA2(body,iaBody[0])*BodyDC2Di(body,iaBody[0]))/\
          (body[iaBody[0]].dMeanMotion*DAYSEC*pow(body[iaBody[0]].dSemi/AUCM,2)*\
          sqrt(1.0-pow(body[iaBody[0]].dEcc,2)))/DAYSEC;
}

double fdGalHabitDLongPDtQuad0(BODY *body, SYSTEM *system, int *iaBody) {
  double dMu;
  dMu = KGAUSS*KGAUSS*(body[iaBody[0]].dHostBinMass1*(body[0].dMass-\
        body[iaBody[0]].dHostBinMass1))/(body[0].dMass)/MSUN; 
  
  return dMu*(3*(HostBinA1(body,iaBody[0])*BodyDC1De(body,iaBody[0])+\
          HostBinA2(body,iaBody[0])*BodyDC2De(body,iaBody[0]))-\
          HostBinA0(body,iaBody[0])*BodyDC0De(body,iaBody[0]))*sqrt(1.-pow(body[iaBody[0]].dEcc,2))
          /(body[iaBody[0]].dMeanMotion*DAYSEC*pow(body[iaBody[0]].dSemi/AUCM,2))/DAYSEC\
          +3*dMu*(HostBinA1(body,iaBody[0])*BodyDC1Di(body,iaBody[0])+\
          HostBinA2(body,iaBody[0])*BodyDC2Di(body,iaBody[0]))*sin(body[iaBody[0]].dIncTmp)*\
          tan(body[iaBody[0]].dIncTmp/2)/(body[iaBody[0]].dMeanMotion*DAYSEC*\
          pow(body[iaBody[0]].dSemi/AUCM,2)*sqrt(1.0-pow(body[iaBody[0]].dEcc,2)))/DAYSEC;
}

double fdGalHabitDArgPDtQuad0(BODY *body, SYSTEM *system, int *iaBody) {
  double dMu;
  dMu = KGAUSS*KGAUSS*(body[iaBody[0]].dHostBinMass1*(body[0].dMass-\
        body[iaBody[0]].dHostBinMass1))/(body[0].dMass)/MSUN; 
  
  return fdGalHabitDLongPDtQuad0(body,system,iaBody)-\
          fdGalHabitDLongADtQuad0(body,system,iaBody);
}

double fdGalHabitDIncDtQuad0(BODY *body, SYSTEM *system, int *iaBody) {
  double dMu;
  dMu = KGAUSS*KGAUSS*(body[iaBody[0]].dHostBinMass1*(body[0].dMass-\
        body[iaBody[0]].dHostBinMass1))/(body[0].dMass)/MSUN; 
  
  return -3*dMu*(HostBinA1(body,iaBody[0])*BodyDC1Dla(body,iaBody[0])+\
          HostBinA2(body,iaBody[0])*BodyDC2Dla(body,iaBody[0]))\
          /(body[iaBody[0]].dMeanMotion*DAYSEC*pow(body[iaBody[0]].dSemi/AUCM,2)\
          *sqrt(1.0-pow(body[iaBody[0]].dEcc,2)))/DAYSEC;
}

double fdGalHabitDEccXDtQuad0(BODY *body, SYSTEM *system, int *iaBody) {
  return dexdla(body[iaBody[0]].dEccYTmp)*fdGalHabitDLongADtQuad0(body,system,iaBody) +\
        dexdap(body[iaBody[0]].dEccYTmp,body[iaBody[0]].dEccZTmp,body[iaBody[0]].dIncTmp, body[iaBody[0]].dLongATmp)*fdGalHabitDArgPDtQuad0(body,system,iaBody) + \
        body[iaBody[0]].dEcc*sin(body[iaBody[0]].dLongATmp)*sin(body[iaBody[0]].dArgPTmp)*\
        sin(body[iaBody[0]].dIncTmp)*fdGalHabitDIncDtQuad0(body,system,iaBody);
}

double fdGalHabitDEccYDtQuad0(BODY *body, SYSTEM *system, int *iaBody) {
  return deydla(body[iaBody[0]].dEccXTmp)*fdGalHabitDLongADtQuad0(body,system,iaBody) +\
          deydap(body[iaBody[0]].dEccXTmp,body[iaBody[0]].dEccZTmp,body[iaBody[0]].dIncTmp, body[iaBody[0]].dLongATmp)*fdGalHabitDArgPDtQuad0(body,system,iaBody) - \
        body[iaBody[0]].dEcc*cos(body[iaBody[0]].dLongATmp)*sin(body[iaBody[0]].dArgPTmp)*\
        sin(body[iaBody[0]].dIncTmp)*fdGalHabitDIncDtQuad0(body,system,iaBody);
}

double fdGalHabitDEccZDtQuad0(BODY *body, SYSTEM *system, int *iaBody) {
  return dezdap(body[iaBody[0]].dArgPTmp,body[iaBody[0]].dEcc, body[iaBody[0]].dIncTmp)*\
        fdGalHabitDArgPDtQuad0(body,system,iaBody) + \
        body[iaBody[0]].dEcc*sin(body[iaBody[0]].dArgPTmp)*\
        cos(body[iaBody[0]].dIncTmp)*fdGalHabitDIncDtQuad0(body,system,iaBody);
}

double fdGalHabitDAngMXDtQuad0(BODY *body, SYSTEM *system, int *iaBody) {
  double dJ;
  dJ = sqrt(1.0-pow(body[iaBody[0]].dEcc,2));
  
  return dJ*cos(body[iaBody[0]].dLongATmp)*sin(body[iaBody[0]].dIncTmp)*\
        fdGalHabitDLongADtQuad0(body,system,iaBody)+ \
        dJ*sin(body[iaBody[0]].dLongATmp)*cos(body[iaBody[0]].dIncTmp) *\
        fdGalHabitDIncDtQuad0(body,system,iaBody);
}

double fdGalHabitDAngMYDtQuad0(BODY *body, SYSTEM *system, int *iaBody) {
  double dJ;
  dJ = sqrt(1.0-pow(body[iaBody[0]].dEcc,2));
  
  return dJ*sin(body[iaBody[0]].dLongATmp)*sin(body[iaBody[0]].dIncTmp)*\
        fdGalHabitDLongADtQuad0(body,system,iaBody)- \
        dJ*cos(body[iaBody[0]].dLongATmp)*cos(body[iaBody[0]].dIncTmp) *\
        fdGalHabitDIncDtQuad0(body,system,iaBody);
}

double fdGalHabitDAngMZDtQuad0(BODY *body, SYSTEM *system, int *iaBody) {
  double dJ;
  dJ = sqrt(1.0-pow(body[iaBody[0]].dEcc,2));
  
  return -dJ*sin(body[iaBody[0]].dIncTmp)*fdGalHabitDIncDtQuad0(body,system,iaBody); 
}

double fdGalHabitDEccXDtQuad(BODY *body, SYSTEM *system, int *iaBody) {
  return fdGalHabitDEccXDtQuad0(body,system,iaBody)*cos(body[iaBody[0]].dHostBinLongA) - \
         fdGalHabitDEccYDtQuad0(body,system,iaBody)*cos(body[iaBody[0]].dHostBinInc)*\
              sin(body[iaBody[0]].dHostBinLongA) + \
         fdGalHabitDEccZDtQuad0(body,system,iaBody)*sin(body[iaBody[0]].dHostBinInc)*\
              sin(body[iaBody[0]].dHostBinLongA);
}

double fdGalHabitDEccYDtQuad(BODY *body, SYSTEM *system, int *iaBody) {
  return fdGalHabitDEccXDtQuad0(body,system,iaBody)*sin(body[iaBody[0]].dHostBinLongA) + \
         fdGalHabitDEccYDtQuad0(body,system,iaBody)*cos(body[iaBody[0]].dHostBinInc)*\
              cos(body[iaBody[0]].dHostBinLongA) - \
         fdGalHabitDEccZDtQuad0(body,system,iaBody)*sin(body[iaBody[0]].dHostBinInc)*\
              cos(body[iaBody[0]].dHostBinLongA);
}

double fdGalHabitDEccZDtQuad(BODY *body, SYSTEM *system, int *iaBody) {
  return fdGalHabitDEccYDtQuad0(body,system,iaBody)*sin(body[iaBody[0]].dHostBinInc)+ \
         fdGalHabitDEccZDtQuad0(body,system,iaBody)*cos(body[iaBody[0]].dHostBinInc);
}

double fdGalHabitDAngMXDtQuad(BODY *body, SYSTEM *system, int *iaBody) {
  return fdGalHabitDAngMXDtQuad0(body,system,iaBody)*cos(body[iaBody[0]].dHostBinLongA) - \
         fdGalHabitDAngMYDtQuad0(body,system,iaBody)*cos(body[iaBody[0]].dHostBinInc)*\
              sin(body[iaBody[0]].dHostBinLongA)+ \
         fdGalHabitDAngMZDtQuad0(body,system,iaBody)*sin(body[iaBody[0]].dHostBinInc)*\
              sin(body[iaBody[0]].dHostBinLongA);
}

double fdGalHabitDAngMYDtQuad(BODY *body, SYSTEM *system, int *iaBody) {
  return fdGalHabitDAngMXDtQuad0(body,system,iaBody)*sin(body[iaBody[0]].dHostBinLongA) + \
         fdGalHabitDAngMYDtQuad0(body,system,iaBody)*cos(body[iaBody[0]].dHostBinInc)*\
              cos(body[iaBody[0]].dHostBinLongA)- \
         fdGalHabitDAngMZDtQuad0(body,system,iaBody)*sin(body[iaBody[0]].dHostBinInc)*\
              cos(body[iaBody[0]].dHostBinLongA);
}

double fdGalHabitDAngMZDtQuad(BODY *body, SYSTEM *system, int *iaBody) {
  return fdGalHabitDAngMYDtQuad0(body,system,iaBody)*sin(body[iaBody[0]].dHostBinInc)+ \
         fdGalHabitDAngMZDtQuad0(body,system,iaBody)*cos(body[iaBody[0]].dHostBinInc);
}

//----Fuck it, let's try these equations (Breiter & Vokrouhlicky 2015)-----------------------------
double QuadC2(BODY *body, int *iaBody) {
  double M1, X0, X1, a1, a2, m2;
  
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    //iaBody[0] = inner body
    M1 = body[0].dMass+body[iaBody[0]].dMass;
    X0 = body[0].dMass/M1;
    X1 = body[iaBody[0]].dMass/M1;
    a1 = body[iaBody[0]].dSemi;
    a2 = body[iaBody[1]].dSemi;
    m2 = body[iaBody[1]].dMass;
  } else {
    //iaBody[0] = outer body
    M1 = body[0].dMass+body[iaBody[1]].dMass; 
    X0 = body[0].dMass/M1;
    X1 = body[iaBody[1]].dMass/M1;
    a1 = body[iaBody[1]].dSemi;
    a2 = body[iaBody[0]].dSemi;
    m2 = body[iaBody[0]].dMass;
  }
  
  return 3./8*KGAUSS*KGAUSS*m2*M1/pow(MSUN,2)/(a2/AUCM)*X0*X1*pow(a1/a2,2.0);
}

double DQuadDEccXInner(BODY *body, int *iaBody) {
  //iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ13;
  
  dEta2 = body[iaBody[1]].dAngM;
  dQ13 = body[iaBody[0]].dEccX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dAngMZ;
         
  return -2*QuadC2(body,iaBody)/pow(dEta2,5.0)*(2*pow(dEta2,2.0)*body[iaBody[0]].dEccX -\
              5*dQ13*body[iaBody[1]].dAngMX);
}

double DQuadDEccYInner(BODY *body, int *iaBody) {
  //iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ13;
  
  dEta2 = body[iaBody[1]].dAngM;
  dQ13 = body[iaBody[0]].dEccX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dAngMZ;
         
  return -2*QuadC2(body,iaBody)/pow(dEta2,5.0)*(2*pow(dEta2,2.0)*body[iaBody[0]].dEccY -\
              5*dQ13*body[iaBody[1]].dAngMY);
}

double DQuadDEccZInner(BODY *body, int *iaBody) {
  //iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ13;
  
  dEta2 = body[iaBody[1]].dAngM;
  dQ13 = body[iaBody[0]].dEccX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dAngMZ;
         
  return -2*QuadC2(body,iaBody)/pow(dEta2,5.0)*(2*pow(dEta2,2.0)*body[iaBody[0]].dEccZ -\
              5*dQ13*body[iaBody[1]].dAngMZ);
}

double DQuadDAngMXInner(BODY *body, int *iaBody) {
  //iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ33;
  
  dEta2 = body[iaBody[1]].dAngM;
  dQ33 = body[iaBody[0]].dAngMX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dAngMZ;
  
  return -2*QuadC2(body,iaBody)/pow(dEta2,5.0)*dQ33*body[iaBody[1]].dAngMX;
}  

double DQuadDAngMYInner(BODY *body, int *iaBody) {
  //iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ33;
  
  dEta2 = body[iaBody[1]].dAngM;
  dQ33 = body[iaBody[0]].dAngMX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dAngMZ;
  
  return -2*QuadC2(body,iaBody)/pow(dEta2,5.0)*dQ33*body[iaBody[1]].dAngMY;
}  

double DQuadDAngMZInner(BODY *body, int *iaBody) {
  //iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ33;
  
  dEta2 = body[iaBody[1]].dAngM;
  dQ33 = body[iaBody[0]].dAngMX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dAngMZ;
  
  return -2*QuadC2(body,iaBody)/pow(dEta2,5.0)*dQ33*body[iaBody[1]].dAngMZ;
}  

double DQuadDAngMXOuter(BODY *body, int *iaBody) {
  //iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ13, dQ33;
  
  dEta2 = body[iaBody[0]].dAngM;
  dQ13 = body[iaBody[1]].dEccX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dEccY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dEccZ*body[iaBody[0]].dAngMZ;
  dQ33 = body[iaBody[1]].dAngMX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dAngMY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dAngMZ*body[iaBody[0]].dAngMZ;
   
  return QuadC2(body,iaBody)/pow(dEta2,5.0)*( 10.0*dQ13*body[iaBody[1]].dEccX - \
          2.*dQ33*body[iaBody[1]].dAngMX - (1.0-6.*pow(body[iaBody[1]].dEcc,2) + \
          5./pow(dEta2,2)*(5*pow(dQ13,2)-pow(dQ33,2)))*body[iaBody[0]].dAngMX );
}

double DQuadDAngMYOuter(BODY *body, int *iaBody) {
  //iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ13, dQ33;
  
  dEta2 = body[iaBody[0]].dAngM;
  dQ13 = body[iaBody[1]].dEccX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dEccY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dEccZ*body[iaBody[0]].dAngMZ;
  dQ33 = body[iaBody[1]].dAngMX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dAngMY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dAngMZ*body[iaBody[0]].dAngMZ;
   
  return QuadC2(body,iaBody)/pow(dEta2,5.0)*( 10.0*dQ13*body[iaBody[1]].dEccY - \
          2.*dQ33*body[iaBody[1]].dAngMY - (1.0-6.*pow(body[iaBody[1]].dEcc,2) + \
          5./pow(dEta2,2)*(5*pow(dQ13,2)-pow(dQ33,2)))*body[iaBody[0]].dAngMY );
}

double DQuadDAngMZOuter(BODY *body, int *iaBody) {
  //iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ13, dQ33;
  
  dEta2 = body[iaBody[0]].dAngM;
  dQ13 = body[iaBody[1]].dEccX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dEccY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dEccZ*body[iaBody[0]].dAngMZ;
  dQ33 = body[iaBody[1]].dAngMX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dAngMY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dAngMZ*body[iaBody[0]].dAngMZ;
   
  return QuadC2(body,iaBody)/pow(dEta2,5.0)*( 10.0*dQ13*body[iaBody[1]].dEccZ - \
          2.*dQ33*body[iaBody[1]].dAngMZ - (1.0-6.*pow(body[iaBody[1]].dEcc,2) + \
          5./pow(dEta2,2)*(5*pow(dQ13,2)-pow(dQ33,2)))*body[iaBody[0]].dAngMZ );
}

double fdGalHabitDAngMXDtBV(BODY *body, SYSTEM *system, int *iaBody) {
  double dL, dHdeY, dHdeZ, dHdKY, dHdKZ, dFirstTerm, dSecondTerm;
  
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    //iaBody[0] is the inner body
    dL = (body[0].dMass*body[iaBody[0]].dMass)/pow(MSUN,2)*\
         sqrt(KGAUSS*KGAUSS*body[iaBody[0]].dSemi/AUCM/((body[0].dMass+body[iaBody[0]].dMass)/MSUN));
    dHdeY = DQuadDEccYInner(body, iaBody) + DOctDEccYInner(body, iaBody);
    dHdeZ = DQuadDEccZInner(body, iaBody) + DOctDEccZInner(body, iaBody);
    dHdKY = DQuadDAngMYInner(body, iaBody) + DOctDAngMYInner(body, iaBody);
    dHdKZ = DQuadDAngMZInner(body, iaBody) + DOctDAngMZInner(body, iaBody);
  } else {
    //iaBody[0] is the outer body
    dL = ((body[0].dMass+body[iaBody[1]].dMass)*body[iaBody[0]].dMass)/pow(MSUN,2)*\
         sqrt(KGAUSS*KGAUSS*body[iaBody[0]].dSemi/AUCM/\
         ((body[0].dMass+body[iaBody[0]].dMass+body[iaBody[1]].dMass)/MSUN));
    dHdeY = DOctDEccYOuter(body, iaBody);
    dHdeZ = DOctDEccZOuter(body, iaBody);
    dHdKY = DQuadDAngMYOuter(body, iaBody) + DOctDAngMYOuter(body, iaBody);
    dHdKZ = DQuadDAngMZOuter(body, iaBody) + DOctDAngMZOuter(body, iaBody);
  }
  dFirstTerm = body[iaBody[0]].dEccY*dHdeZ - body[iaBody[0]].dEccZ*dHdeY;
  dSecondTerm = body[iaBody[0]].dAngMY*dHdKZ - body[iaBody[0]].dAngMZ*dHdKY;
  
  return -1.0/dL*(dFirstTerm + dSecondTerm)/DAYSEC;
}

double fdGalHabitDAngMYDtBV(BODY *body, SYSTEM *system, int *iaBody) {
  double dL, dHdeX, dHdeZ, dHdKX, dHdKZ, dFirstTerm, dSecondTerm;
  
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    dL = (body[0].dMass*body[iaBody[0]].dMass)/pow(MSUN,2)*\
         sqrt(KGAUSS*KGAUSS*body[iaBody[0]].dSemi/AUCM/((body[0].dMass+body[iaBody[0]].dMass)/MSUN));
    //iaBody[0] is the inner body
    dHdeX = DQuadDEccXInner(body, iaBody) + DOctDEccXInner(body, iaBody);
    dHdeZ = DQuadDEccZInner(body, iaBody) + DOctDEccZInner(body, iaBody);
    dHdKX = DQuadDAngMXInner(body, iaBody) + DOctDAngMXInner(body, iaBody);
    dHdKZ = DQuadDAngMZInner(body, iaBody) + DOctDAngMZInner(body, iaBody);
  } else {
    //iaBody[0] is the outer body
    dL = ((body[0].dMass+body[iaBody[1]].dMass)*body[iaBody[0]].dMass)/pow(MSUN,2)*\
         sqrt(KGAUSS*KGAUSS*body[iaBody[0]].dSemi/AUCM/\
         ((body[0].dMass+body[iaBody[0]].dMass+body[iaBody[1]].dMass)/MSUN));
    dHdeX = DOctDEccXOuter(body, iaBody);
    dHdeZ = DOctDEccZOuter(body, iaBody);
    dHdKX = DQuadDAngMXOuter(body, iaBody) + DOctDAngMXOuter(body, iaBody);
    dHdKZ = DQuadDAngMZOuter(body, iaBody) + DOctDAngMZOuter(body, iaBody);
  }
  dFirstTerm = body[iaBody[0]].dEccZ*dHdeX - body[iaBody[0]].dEccX*dHdeZ;
  dSecondTerm = body[iaBody[0]].dAngMZ*dHdKX - body[iaBody[0]].dAngMX*dHdKZ;
  
  return -1.0/dL*(dFirstTerm + dSecondTerm)/DAYSEC;
}

double fdGalHabitDAngMZDtBV(BODY *body, SYSTEM *system, int *iaBody) {
  double dL, dHdeX, dHdeY, dHdKX, dHdKY, dFirstTerm, dSecondTerm;
  
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    dL = (body[0].dMass*body[iaBody[0]].dMass)/pow(MSUN,2)*\
         sqrt(KGAUSS*KGAUSS*body[iaBody[0]].dSemi/AUCM/((body[0].dMass+body[iaBody[0]].dMass)/MSUN));
    //iaBody[0] is the inner body
    dHdeX = DQuadDEccXInner(body, iaBody) + DOctDEccXInner(body, iaBody);
    dHdeY = DQuadDEccYInner(body, iaBody) + DOctDEccYInner(body, iaBody);
    dHdKX = DQuadDAngMXInner(body, iaBody) + DOctDAngMXInner(body, iaBody);
    dHdKY = DQuadDAngMYInner(body, iaBody) + DOctDAngMYInner(body, iaBody);
  } else {
    //iaBody[0] is the outer body
    dL = ((body[0].dMass+body[iaBody[1]].dMass)*body[iaBody[0]].dMass)/pow(MSUN,2)*\
         sqrt(KGAUSS*KGAUSS*body[iaBody[0]].dSemi/AUCM/\
         ((body[0].dMass+body[iaBody[0]].dMass+body[iaBody[1]].dMass)/MSUN));
    dHdeX = DOctDEccXOuter(body, iaBody);
    dHdeY = DOctDEccYOuter(body, iaBody);
    dHdKX = DQuadDAngMXOuter(body, iaBody) + DOctDAngMXOuter(body, iaBody);
    dHdKY = DQuadDAngMYOuter(body, iaBody) + DOctDAngMYOuter(body, iaBody);
  }
  dFirstTerm = body[iaBody[0]].dEccX*dHdeY - body[iaBody[0]].dEccY*dHdeX;
  dSecondTerm = body[iaBody[0]].dAngMX*dHdKY - body[iaBody[0]].dAngMY*dHdKX;
  
  return -1.0/dL*(dFirstTerm + dSecondTerm)/DAYSEC;
}

double fdGalHabitDEccXDtBV(BODY *body, SYSTEM *system, int *iaBody) {
  double dL, dHdeY, dHdeZ, dHdKY, dHdKZ, dFirstTerm, dSecondTerm;
  
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    //iaBody[0] is the inner body
    dL = (body[0].dMass*body[iaBody[0]].dMass)/pow(MSUN,2)*\
         sqrt(KGAUSS*KGAUSS*body[iaBody[0]].dSemi/AUCM/((body[0].dMass+body[iaBody[0]].dMass)/MSUN));
    dHdeY = DQuadDEccYInner(body, iaBody) + DOctDEccYInner(body, iaBody);
    dHdeZ = DQuadDEccZInner(body, iaBody) + DOctDEccZInner(body, iaBody);
    dHdKY = DQuadDAngMYInner(body, iaBody) + DOctDAngMYInner(body, iaBody);
    dHdKZ = DQuadDAngMZInner(body, iaBody) + DOctDAngMZInner(body, iaBody);
  } else {
    //iaBody[0] is the outer body
    dL = ((body[0].dMass+body[iaBody[1]].dMass)*body[iaBody[0]].dMass)/pow(MSUN,2)*\
         sqrt(KGAUSS*KGAUSS*body[iaBody[0]].dSemi/AUCM/\
         ((body[0].dMass+body[iaBody[0]].dMass+body[iaBody[1]].dMass)/MSUN));
    dHdeY = DOctDEccYOuter(body, iaBody);
    dHdeZ = DOctDEccZOuter(body, iaBody);
    dHdKY = DQuadDAngMYOuter(body, iaBody) + DOctDAngMYOuter(body, iaBody);
    dHdKZ = DQuadDAngMZOuter(body, iaBody) + DOctDAngMZOuter(body, iaBody);
  }
  dFirstTerm = body[iaBody[0]].dAngMY*dHdeZ - body[iaBody[0]].dAngMZ*dHdeY;
  dSecondTerm = body[iaBody[0]].dEccY*dHdKZ - body[iaBody[0]].dEccZ*dHdKY;
  
  return -1.0/dL*(dFirstTerm + dSecondTerm)/DAYSEC;
}

double fdGalHabitDEccYDtBV(BODY *body, SYSTEM *system, int *iaBody) {
  double dL, dHdeX, dHdeZ, dHdKX, dHdKZ, dFirstTerm, dSecondTerm;

  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    //iaBody[0] is the inner body
    dL = (body[0].dMass*body[iaBody[0]].dMass)/pow(MSUN,2)*\
         sqrt(KGAUSS*KGAUSS*body[iaBody[0]].dSemi/AUCM/((body[0].dMass+body[iaBody[0]].dMass)/MSUN));
    dHdeX = DQuadDEccXInner(body, iaBody) + DOctDEccXInner(body, iaBody);
    dHdeZ = DQuadDEccZInner(body, iaBody) + DOctDEccZInner(body, iaBody);
    dHdKX = DQuadDAngMXInner(body, iaBody) + DOctDAngMXInner(body, iaBody);
    dHdKZ = DQuadDAngMZInner(body, iaBody) + DOctDAngMZInner(body, iaBody);
  } else {
    //iaBody[0] is the outer body
    dL = ((body[0].dMass+body[iaBody[1]].dMass)*body[iaBody[0]].dMass)/pow(MSUN,2)*\
         sqrt(KGAUSS*KGAUSS*body[iaBody[0]].dSemi/AUCM/\
         ((body[0].dMass+body[iaBody[0]].dMass+body[iaBody[1]].dMass)/MSUN));
    dHdeX = DOctDEccXOuter(body, iaBody);
    dHdeZ = DOctDEccZOuter(body, iaBody);
    dHdKX = DQuadDAngMXOuter(body, iaBody) + DOctDAngMXOuter(body, iaBody);
    dHdKZ = DQuadDAngMZOuter(body, iaBody) + DOctDAngMZOuter(body, iaBody);
  }
  dFirstTerm = body[iaBody[0]].dAngMZ*dHdeX - body[iaBody[0]].dAngMX*dHdeZ;
  dSecondTerm = body[iaBody[0]].dEccZ*dHdKX - body[iaBody[0]].dEccX*dHdKZ;
  
  return -1.0/dL*(dFirstTerm + dSecondTerm)/DAYSEC;
}

double fdGalHabitDEccZDtBV(BODY *body, SYSTEM *system, int *iaBody) {
  double dL, dHdeX, dHdeY, dHdKX, dHdKY, dFirstTerm, dSecondTerm;
  
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    //iaBody[0] is the inner body
    dL = (body[0].dMass*body[iaBody[0]].dMass)/pow(MSUN,2)*\
         sqrt(KGAUSS*KGAUSS*body[iaBody[0]].dSemi/AUCM/((body[0].dMass+body[iaBody[0]].dMass)/MSUN));
    dHdeX = DQuadDEccXInner(body, iaBody) + DOctDEccXInner(body, iaBody);
    dHdeY = DQuadDEccYInner(body, iaBody) + DOctDEccYInner(body, iaBody);
    dHdKX = DQuadDAngMXInner(body, iaBody) + DOctDAngMXInner(body, iaBody);
    dHdKY = DQuadDAngMYInner(body, iaBody) + DOctDAngMYInner(body, iaBody);
  } else {
    //iaBody[0] is the outer body
    dL = ((body[0].dMass+body[iaBody[1]].dMass)*body[iaBody[0]].dMass)/pow(MSUN,2)*\
         sqrt(KGAUSS*KGAUSS*body[iaBody[0]].dSemi/AUCM/\
         ((body[0].dMass+body[iaBody[0]].dMass+body[iaBody[1]].dMass)/MSUN));
    dHdeX = DOctDEccXOuter(body, iaBody);
    dHdeY = DOctDEccYOuter(body, iaBody);
    dHdKX = DQuadDAngMXOuter(body, iaBody) + DOctDAngMXOuter(body, iaBody);
    dHdKY = DQuadDAngMYOuter(body, iaBody) + DOctDAngMYOuter(body, iaBody);
  }
  dFirstTerm = body[iaBody[0]].dAngMX*dHdeY - body[iaBody[0]].dAngMY*dHdeX;
  dSecondTerm = body[iaBody[0]].dEccX*dHdKY - body[iaBody[0]].dEccY*dHdKX;
  
  return -1.0/dL*(dFirstTerm + dSecondTerm)/DAYSEC;
}

double OctC3(BODY *body, int *iaBody) {
  double M1, X0, X1, a1, a2;
  
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    //iaBody[0] = inner body
    M1 = body[0].dMass+body[iaBody[0]].dMass;
    X0 = body[0].dMass/M1;
    X1 = body[iaBody[0]].dMass/M1;
    a1 = body[iaBody[0]].dSemi;
    a2 = body[iaBody[1]].dSemi;
  } else {
    //iaBody[0] = outer body
    M1 = body[0].dMass+body[iaBody[1]].dMass;
    X0 = body[0].dMass/M1;
    X1 = body[iaBody[1]].dMass/M1;
    a1 = body[iaBody[1]].dSemi;
    a2 = body[iaBody[0]].dSemi;
  }
  
  return 5./8*QuadC2(body,iaBody)*(X0-X1)*a1/a2;
}

double DOctDEccXInner(BODY *body, int *iaBody) {
  //iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ11, dQ13, dQ31, dQ33;
  
  dEta2 = body[iaBody[1]].dAngM;
  dQ11 = body[iaBody[0]].dEccX*body[iaBody[1]].dEccX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dEccY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dEccZ;
  
  dQ13 = body[iaBody[0]].dEccX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dAngMZ;
         
  dQ31 = body[iaBody[0]].dAngMX*body[iaBody[1]].dEccX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dEccY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dEccZ;
  
  dQ33 = body[iaBody[0]].dAngMX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dAngMZ;       
         
  return OctC3(body,iaBody)/pow(dEta2,7)* ( 16*pow(dEta2,2)*dQ11*body[iaBody[0]].dEccX \
         + 10*(dQ31*dQ33 - 7*dQ11*dQ13)*body[iaBody[1]].dAngMX \
         - ((1.-8*pow(body[iaBody[0]].dEcc,2))*pow(dEta2,2)+35*pow(dQ13,2) \
         - 5*pow(dQ33,2))*body[iaBody[1]].dEccX );
}

double DOctDEccYInner(BODY *body, int *iaBody) {
  //iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ11, dQ13, dQ31, dQ33;
  
  dEta2 = body[iaBody[1]].dAngM;
  dQ11 = body[iaBody[0]].dEccX*body[iaBody[1]].dEccX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dEccY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dEccZ;
  
  dQ13 = body[iaBody[0]].dEccX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dAngMZ;
         
  dQ31 = body[iaBody[0]].dAngMX*body[iaBody[1]].dEccX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dEccY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dEccZ;
  
  dQ33 = body[iaBody[0]].dAngMX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dAngMZ;       
         
  return OctC3(body,iaBody)/pow(dEta2,7)* ( 16*pow(dEta2,2)*dQ11*body[iaBody[0]].dEccY \
         + 10*(dQ31*dQ33 - 7*dQ11*dQ13)*body[iaBody[1]].dAngMY \
         - ((1.-8*pow(body[iaBody[0]].dEcc,2))*pow(dEta2,2)+35*pow(dQ13,2) \
         - 5*pow(dQ33,2))*body[iaBody[1]].dEccY );
}

double DOctDEccZInner(BODY *body, int *iaBody) {
  //iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ11, dQ13, dQ31, dQ33;
  
  dEta2 = body[iaBody[1]].dAngM;
  dQ11 = body[iaBody[0]].dEccX*body[iaBody[1]].dEccX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dEccY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dEccZ;
  
  dQ13 = body[iaBody[0]].dEccX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dAngMZ;
         
  dQ31 = body[iaBody[0]].dAngMX*body[iaBody[1]].dEccX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dEccY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dEccZ;
  
  dQ33 = body[iaBody[0]].dAngMX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dAngMZ;       
         
  return OctC3(body,iaBody)/pow(dEta2,7)* ( 16*pow(dEta2,2)*dQ11*body[iaBody[0]].dEccZ \
         + 10*(dQ31*dQ33 - 7*dQ11*dQ13)*body[iaBody[1]].dAngMZ \
         - ((1.-8*pow(body[iaBody[0]].dEcc,2))*pow(dEta2,2)+35*pow(dQ13,2) \
         - 5*pow(dQ33,2))*body[iaBody[1]].dEccZ );
}

double DOctDAngMXInner(BODY *body, int *iaBody) {
  //iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ11, dQ13, dQ31, dQ33;
  
  dEta2 = body[iaBody[1]].dAngM;
  dQ11 = body[iaBody[0]].dEccX*body[iaBody[1]].dEccX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dEccY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dEccZ;
  
  dQ13 = body[iaBody[0]].dEccX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dAngMZ;
         
  dQ31 = body[iaBody[0]].dAngMX*body[iaBody[1]].dEccX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dEccY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dEccZ;
  
  dQ33 = body[iaBody[0]].dAngMX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dAngMZ;       
         
  return 10*OctC3(body,iaBody)/pow(dEta2,7)* ( (dQ13*dQ31+dQ11*dQ33)*body[iaBody[1]].dAngMX \
          + dQ13*dQ33*body[iaBody[1]].dEccX );
}

double DOctDAngMYInner(BODY *body, int *iaBody) {
  //iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ11, dQ13, dQ31, dQ33;
  
  dEta2 = body[iaBody[1]].dAngM;
  dQ11 = body[iaBody[0]].dEccX*body[iaBody[1]].dEccX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dEccY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dEccZ;
  
  dQ13 = body[iaBody[0]].dEccX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dAngMZ;
         
  dQ31 = body[iaBody[0]].dAngMX*body[iaBody[1]].dEccX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dEccY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dEccZ;
  
  dQ33 = body[iaBody[0]].dAngMX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dAngMZ;       
         
  return 10*OctC3(body,iaBody)/pow(dEta2,7)* ( (dQ13*dQ31+dQ11*dQ33)*body[iaBody[1]].dAngMY \
          + dQ13*dQ33*body[iaBody[1]].dEccY );
}

double DOctDAngMZInner(BODY *body, int *iaBody) {
  //iaBody[0] is the inner body now, iaBody[1] is the outer
  double dEta2, dQ11, dQ13, dQ31, dQ33;
  
  dEta2 = body[iaBody[1]].dAngM;
  dQ11 = body[iaBody[0]].dEccX*body[iaBody[1]].dEccX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dEccY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dEccZ;
  
  dQ13 = body[iaBody[0]].dEccX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dEccY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dEccZ*body[iaBody[1]].dAngMZ;
         
  dQ31 = body[iaBody[0]].dAngMX*body[iaBody[1]].dEccX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dEccY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dEccZ;
  
  dQ33 = body[iaBody[0]].dAngMX*body[iaBody[1]].dAngMX + \
         body[iaBody[0]].dAngMY*body[iaBody[1]].dAngMY + \
         body[iaBody[0]].dAngMZ*body[iaBody[1]].dAngMZ;       
         
  return 10*OctC3(body,iaBody)/pow(dEta2,7)* ( (dQ13*dQ31+dQ11*dQ33)*body[iaBody[1]].dAngMZ \
          + dQ13*dQ33*body[iaBody[1]].dEccZ );
}

double DOctDEccXOuter(BODY *body, int *iaBody) {
  //iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ13, dQ33;
  
  dEta2 = body[iaBody[0]].dAngM;
  
  dQ13 = body[iaBody[1]].dEccX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dEccY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dEccZ*body[iaBody[0]].dAngMZ;
  
  dQ33 = body[iaBody[1]].dAngMX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dAngMY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dAngMZ*body[iaBody[0]].dAngMZ;       
         
  return OctC3(body,iaBody)/pow(dEta2,7)* ( 10*dQ13*dQ33*body[iaBody[1]].dAngMX \
          - ((1.-8*pow(body[iaBody[1]].dEcc,2))*pow(dEta2,2) + 35*pow(dQ13,2) \
          - 5*pow(dQ33,2))*body[iaBody[1]].dEccX );
}

double DOctDEccYOuter(BODY *body, int *iaBody) {
  //iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ13, dQ33;
  
  dEta2 = body[iaBody[0]].dAngM;
  
  dQ13 = body[iaBody[1]].dEccX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dEccY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dEccZ*body[iaBody[0]].dAngMZ;
  
  dQ33 = body[iaBody[1]].dAngMX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dAngMY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dAngMZ*body[iaBody[0]].dAngMZ;       
         
  return OctC3(body,iaBody)/pow(dEta2,7)* ( 10*dQ13*dQ33*body[iaBody[1]].dAngMY \
          - ((1.-8*pow(body[iaBody[1]].dEcc,2))*pow(dEta2,2) + 35*pow(dQ13,2) \
          - 5*pow(dQ33,2))*body[iaBody[1]].dEccY );
}

double DOctDEccZOuter(BODY *body, int *iaBody) {
  //iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ13, dQ33;
  
  dEta2 = body[iaBody[0]].dAngM;
  
  dQ13 = body[iaBody[1]].dEccX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dEccY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dEccZ*body[iaBody[0]].dAngMZ;
  
  dQ33 = body[iaBody[1]].dAngMX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dAngMY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dAngMZ*body[iaBody[0]].dAngMZ;       
         
  return OctC3(body,iaBody)/pow(dEta2,7)* ( 10*dQ13*dQ33*body[iaBody[1]].dAngMZ \
          - ((1.-8*pow(body[iaBody[1]].dEcc,2))*pow(dEta2,2) + 35*pow(dQ13,2) \
          - 5*pow(dQ33,2))*body[iaBody[1]].dEccZ );
}

double DOctDAngMXOuter(BODY *body, int *iaBody) {
  //iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ11, dQ13, dQ31, dQ33;
  
  dEta2 = body[iaBody[0]].dAngM;
  dQ11 = body[iaBody[1]].dEccX*body[iaBody[0]].dEccX + \
         body[iaBody[1]].dEccY*body[iaBody[0]].dEccY + \
         body[iaBody[1]].dEccZ*body[iaBody[0]].dEccZ;
  
  dQ13 = body[iaBody[1]].dEccX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dEccY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dEccZ*body[iaBody[0]].dAngMZ;
         
  dQ31 = body[iaBody[1]].dAngMX*body[iaBody[0]].dEccX + \
         body[iaBody[1]].dAngMY*body[iaBody[0]].dEccY + \
         body[iaBody[1]].dAngMZ*body[iaBody[0]].dEccZ;
  
  dQ33 = body[iaBody[1]].dAngMX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dAngMY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dAngMZ*body[iaBody[0]].dAngMZ;       
         
  return 5*OctC3(body,iaBody)/pow(dEta2,7)* ( 2*(dQ13*dQ31+dQ11*dQ33)*body[iaBody[1]].dAngMX \
          + 2*(dQ31*dQ33-7*dQ11*dQ13)*body[iaBody[1]].dEccX \
          + ((1.-8*pow(body[iaBody[1]].dEcc,2))*dQ11 + 7/pow(dEta2,2)*(dQ11*
          (7*pow(dQ13,2)-pow(dQ33,2)) - 2*dQ13*dQ31*dQ33))*body[iaBody[0]].dAngMX );
}

double DOctDAngMYOuter(BODY *body, int *iaBody) {
  //iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ11, dQ13, dQ31, dQ33;
  
  dEta2 = body[iaBody[0]].dAngM;
  dQ11 = body[iaBody[1]].dEccX*body[iaBody[0]].dEccX + \
         body[iaBody[1]].dEccY*body[iaBody[0]].dEccY + \
         body[iaBody[1]].dEccZ*body[iaBody[0]].dEccZ;
  
  dQ13 = body[iaBody[1]].dEccX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dEccY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dEccZ*body[iaBody[0]].dAngMZ;
         
  dQ31 = body[iaBody[1]].dAngMX*body[iaBody[0]].dEccX + \
         body[iaBody[1]].dAngMY*body[iaBody[0]].dEccY + \
         body[iaBody[1]].dAngMZ*body[iaBody[0]].dEccZ;
  
  dQ33 = body[iaBody[1]].dAngMX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dAngMY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dAngMZ*body[iaBody[0]].dAngMZ;       
         
  return 5*OctC3(body,iaBody)/pow(dEta2,7)* ( 2*(dQ13*dQ31+dQ11*dQ33)*body[iaBody[1]].dAngMY \
          + 2*(dQ31*dQ33-7*dQ11*dQ13)*body[iaBody[1]].dEccY \
          + ((1.-8*pow(body[iaBody[1]].dEcc,2))*dQ11 + 7/pow(dEta2,2)*(dQ11*
          (7*pow(dQ13,2)-pow(dQ33,2)) - 2*dQ13*dQ31*dQ33))*body[iaBody[0]].dAngMY );
}

double DOctDAngMZOuter(BODY *body, int *iaBody) {
  //iaBody[0] is the outer body now, iaBody[1] is the inner
  double dEta2, dQ11, dQ13, dQ31, dQ33;
  
  dEta2 = body[iaBody[0]].dAngM;
  dQ11 = body[iaBody[1]].dEccX*body[iaBody[0]].dEccX + \
         body[iaBody[1]].dEccY*body[iaBody[0]].dEccY + \
         body[iaBody[1]].dEccZ*body[iaBody[0]].dEccZ;
  
  dQ13 = body[iaBody[1]].dEccX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dEccY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dEccZ*body[iaBody[0]].dAngMZ;
         
  dQ31 = body[iaBody[1]].dAngMX*body[iaBody[0]].dEccX + \
         body[iaBody[1]].dAngMY*body[iaBody[0]].dEccY + \
         body[iaBody[1]].dAngMZ*body[iaBody[0]].dEccZ;
  
  dQ33 = body[iaBody[1]].dAngMX*body[iaBody[0]].dAngMX + \
         body[iaBody[1]].dAngMY*body[iaBody[0]].dAngMY + \
         body[iaBody[1]].dAngMZ*body[iaBody[0]].dAngMZ;       
         
  return 5*OctC3(body,iaBody)/pow(dEta2,7)* ( 2*(dQ13*dQ31+dQ11*dQ33)*body[iaBody[1]].dAngMZ \
          + 2*(dQ31*dQ33-7*dQ11*dQ13)*body[iaBody[1]].dEccZ \
          + ((1.-8*pow(body[iaBody[1]].dEcc,2))*dQ11 + 7/pow(dEta2,2)*(dQ11*
          (7*pow(dQ13,2)-pow(dQ33,2)) - 2*dQ13*dQ31*dQ33))*body[iaBody[0]].dAngMZ );
}
