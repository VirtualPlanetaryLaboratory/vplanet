/********************** LAGRANGE.C **********************/
/*
 * Russell Deitrick, April 28, 2015
 *
 * Subroutines that control the integration of the orbital model. 
*/



#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"

void  InitializeControlLagrange(CONTROL *control) {

  control->bForceEqSpin=malloc(control->iNumBodies*sizeof(int));
  control->dMaxLockDiff=malloc(control->iNumBodies*sizeof(double));
  control->dSyncEcc=malloc(control->iNumBodies*sizeof(double));
}

void InitializeModuleLagrange(CONTROL *control,MODULE *module) {
  /* Anything here? */
}

void BodyCopyLagrange(BODY *dest,BODY *src,int iBody) {
  int iIndex,iPert;

  dest[iBody].dsInc = src[iBody].dsInc;
  dest[iBody].dLongP = src[iBody].dLongP;
  dest[iBody].dLongA = src[iBody].dLongA;
  dest[iBody].dMeanL = src[iBody].dMeanL;
  dest[iBody].iNumBodies = src[iBody].iNumBodies;
  dest[iBody].dhecc = src[iBody].dhecc;
  dest[iBody].dkecc = src[iBody].dkecc;
  dest[iBody].dpinc = src[iBody].dpinc;
  dest[iBody].dqinc = src[iBody].dqinc;
  dest[iBody].dbeta = src[iBody].dbeta;
  dest[iBody].deta = src[iBody].deta;
  dest[iBody].dgamma = src[iBody].dgamma;
  dest[iBody].dPrecA = src[iBody].dPrecA;
  dest[iBody].bObliqEvol = src[iBody].bObliqEvol;
  for (iPert=0;iPert<src[iBody].iTidePerts;iPert++)
    dest[iBody].iaTidePerts[iPert] = src[iBody].iaTidePerts[iPert];
}


/**************** LAGRANGE options ********************/

void HelpOptionsLagrange(OPTIONS *options) {
  int i;

  printf("\n ------ LAGRANGE options ------\n");
  for (i=OPTSTARTLAGRANGE;i<OPTENDLAGRANGE;i++) 
    WriteHelpOption(options[i]);
}


/* Inclination */

void ReadInc(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iNumFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,PI].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,180].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    //body[iNumFile-1].dInc = dTmp; 
    body[iNumFile-1].dsInc = sin(0.5*dTmp);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dInc = options->dDefault;
}  

/* Longitude of ascending node */

void ReadLongA(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iNumFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iNumFile-1].dLongA = dTmp; 
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dLongA = options->dDefault;
}  


/* Longitude of pericenter */

void ReadLongP(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iNumFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    
    body[iNumFile-1].dLongP = dTmp; 
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dLongP = options->dDefault;
}  


/* Argument of pericenter */

void ReadArgP(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iNumFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    
    body[iNumFile-1].dArgP = dTmp; 
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dArgP = options->dDefault;
}  

/* Mean anomaly */

void ReadMeanA(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iNumFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iNumFile-1].dMeanA = dTmp; 
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dMeanA = options->dDefault;
} 


/* Mean longitude */

void ReadMeanL(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iNumFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iNumFile-1].dMeanL = dTmp; 
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dMeanL = options->dDefault;
}  

/* True longitude */

void ReadTrueL(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iNumFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }  
    
    body[iNumFile-1].dTrueL = dTmp; 
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dTrueL = options->dDefault;
}  

/* True anomaly */

void ReadTrueA(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iNumFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iNumFile-1].dTrueA = dTmp; 
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dTrueA = options->dDefault;
}  

/* Eccentric anomaly */

void ReadEccA(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iNumFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iNumFile-1].dEccA = dTmp; 
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dEccA = options->dDefault;
}  

void ReadPrecAngle(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iNumFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iNumFile-1].dPrecA = dTmp; 
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dPrecA = options->dDefault;
}  


void ReadDynEllip(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* Cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,options->iLine[iNumFile],control->iVerbose);
    if (dTmp < 0 || dTmp >= 1) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be in the range [0,1).\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    body[iNumFile-1].dDynEllip = dTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultDouble(&body[iNumFile-1].dDynEllip,*options,files->iNumInputs);
    
}

void ReadObliqEvol(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,options->iLine[iNumFile],control->iVerbose);
    body[iNumFile-1].bObliqEvol = bTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    /* Set to default */
    AssignDefaultInt(&body[iNumFile-1].bObliqEvol,*options,files->iNumInputs);
}

void ReadDissipate(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    control->bDissipate = bTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    /* Set to default */
    AssignDefaultInt(&control->bDissipate,*options,files->iNumInputs);
}


void ReadUseShadow(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    control->bUseShadow = bTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    /* Set to default */
    AssignDefaultInt(&control->bDissipate,*options,files->iNumInputs);
}

/* integration elements */

void ReadIntElements(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  char cTmp[OPTLEN];

  /* Tide Model, use #defined variables */

  AddOptionString(files->Infile[iNumFile].cIn,options->cName,cTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (!memcmp(sLower(cTmp),"hk",2)) {
      control->iIntElements = HKPQ;
    } else if (!memcmp(sLower(cTmp),"es",2)) {
      control->iIntElements = ESVO;
    } else {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are hk or es.\n",options->cName,cTmp);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  }
}



void InitializeOptionsLagrange(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  for (iOpt=OPTSTARTLAGRANGE;iOpt<OPTENDLAGRANGE;iOpt++) {
    sprintf(options[iOpt].cName,"null");
    options[iOpt].iLine = malloc(MAXFILES*sizeof(int));
    options[iOpt].iMultiFile=0;
    options[iOpt].iMultiIn=0;
    options[iOpt].iType = -1;
    
    for (iFile=0;iFile<MAXFILES;iFile++) {
      options[iOpt].iLine[iFile] = -1;
      sprintf(options[iOpt].cFile[iFile],"null");
    }
  }
  
  sprintf(options[OPT_INC].cName,"dInc");
  sprintf(options[OPT_INC].cDescr,"Inclination of planet's orbital plane");
  sprintf(options[OPT_INC].cDefault,"0");
  options[OPT_INC].dDefault = 0.0;
  options[OPT_INC].iType = 2;  //no idea what this means!!! XXX
  options[OPT_INC].iMultiFile = 1;  //nor this! XXX
  fnRead[OPT_INC] = &ReadInc;
  
  sprintf(options[OPT_LONGA].cName,"dLongA");
  sprintf(options[OPT_LONGA].cDescr,"Longitude of ascending node of planet's orbital plane");
  sprintf(options[OPT_LONGA].cDefault,"0");
  options[OPT_LONGA].dDefault = 0.0;
  options[OPT_LONGA].iType = 2;  //no idea what this means!!! XXX
  options[OPT_LONGA].iMultiFile = 1;  //nor this! XXX
  fnRead[OPT_LONGA] = &ReadLongA;
  
  sprintf(options[OPT_LONGP].cName,"dLongP");
  sprintf(options[OPT_LONGP].cDescr,"Longitude of pericenter of planet's orbit");
  sprintf(options[OPT_LONGP].cDefault,"0");
  options[OPT_LONGP].dDefault = 0.0;
  options[OPT_LONGP].iType = 2;  //no idea what this means!!! XXX
  options[OPT_LONGP].iMultiFile = 1;   //nor this! XXX
  fnRead[OPT_LONGP] = &ReadLongP;
  
  sprintf(options[OPT_ARGP].cName,"dArgP");
  sprintf(options[OPT_ARGP].cDescr,"Argument of pericenter of planet's orbit");
  sprintf(options[OPT_ARGP].cDefault,"0");
  options[OPT_ARGP].dDefault = 0.0;
  options[OPT_ARGP].iType = 2;  //no idea what this means!!! XXX
  options[OPT_ARGP].iMultiFile = 1;   //nor this! XXX
  fnRead[OPT_ARGP] = &ReadArgP;
  
  /*sprintf(options[OPT_MEANA].cName,"dMeanA");
  sprintf(options[OPT_MEANA].cDescr,"Mean anomaly of planet");
  sprintf(options[OPT_MEANA].cDefault,"0");
  options[OPT_MEANA].dDefault = 0.0;
  options[OPT_MEANA].iType = 2;  //no idea what this means!!! XXX
  options[OPT_MEANA].iMultiFile = 1;   //nor this! XXX
  fnRead[OPT_MEANA] = &ReadMeanA;*/
  
  sprintf(options[OPT_MEANL].cName,"dMeanL");
  sprintf(options[OPT_MEANL].cDescr,"Mean longitude of planet");
  sprintf(options[OPT_MEANL].cDefault,"0");
  options[OPT_MEANL].dDefault = 0.0;
  options[OPT_MEANL].iType = 2;  //no idea what this means!!! XXX
  options[OPT_MEANL].iMultiFile = 1;   //nor this! XXX
  fnRead[OPT_MEANL] = &ReadMeanL;
  
  sprintf(options[OPT_PRECA].cName,"dPrecA");
  sprintf(options[OPT_PRECA].cDescr,"Precession angle (angle b/w LongP and equinox)");
  sprintf(options[OPT_PRECA].cDefault,"0");
  options[OPT_PRECA].dDefault = 0.0;
  options[OPT_PRECA].iType = 2;  //no idea what this means!!! XXX
  options[OPT_PRECA].iMultiFile = 1;   //nor this! XXX
  fnRead[OPT_PRECA] = &ReadPrecAngle;
  
  sprintf(options[OPT_DYNELLIP].cName,"dDynEllip");
  sprintf(options[OPT_DYNELLIP].cDescr,"Dynamical ellipticity");
  sprintf(options[OPT_DYNELLIP].cDefault,"0");
  options[OPT_DYNELLIP].dDefault = 0.0;
  options[OPT_DYNELLIP].iType = 2;  //no idea what this means!!! XXX
  options[OPT_DYNELLIP].iMultiFile = 1;   //nor this! XXX
  fnRead[OPT_DYNELLIP] = &ReadDynEllip;
  
  sprintf(options[OPT_DISSIPATE].cName,"bDissipate");
  sprintf(options[OPT_DISSIPATE].cDescr,"Is a dissipative model included?");
  sprintf(options[OPT_DISSIPATE].cDefault,"No");
  options[OPT_DISSIPATE].dDefault = 0;
  options[OPT_DISSIPATE].iType = 0; 
  options[OPT_DISSIPATE].iMultiFile = 0;
  fnRead[OPT_DISSIPATE] = &ReadDissipate;
  /*sprintf(options[OPT_TRUEL].cName,"dTrueL");
  sprintf(options[OPT_TRUEL].cDescr,"True longitude of planet");
  sprintf(options[OPT_TRUEL].cDefault,"0");
  options[OPT_TRUEL].dDefault = 0.0;
  options[OPT_TRUEL].iType = 2;  //no idea what this means!!! XXX
  options[OPT_TRUEL].iMultiFile = 1;   //nor this! XXX
  fnRead[OPT_TRUEL] = &ReadTrueL;
  
  sprintf(options[OPT_TRUEA].cName,"dTrueA");
  sprintf(options[OPT_TRUEA].cDescr,"True anomaly of planet");
  sprintf(options[OPT_TRUEA].cDefault,"0");
  options[OPT_TRUEA].dDefault = 0.0;
  options[OPT_TRUEA].iType = 2;  //no idea what this means!!! XXX
  options[OPT_TRUEA].iMultiFile = 1;   //nor this! XXX
  fnRead[OPT_TRUEA] = &ReadTrueA;
  
  sprintf(options[OPT_ECCA].cName,"dEccA");
  sprintf(options[OPT_ECCA].cDescr,"Eccentric anomaly of planet");
  sprintf(options[OPT_ECCA].cDefault,"0");
  options[OPT_ECCA].dDefault = 0.0;
  options[OPT_ECCA].iType = 2;  //no idea what this means!!! XXX
  options[OPT_ECCA].iMultiFile = 1;   //nor this! XXX
  fnRead[OPT_ECCA] = &ReadEccA;*/
  
  sprintf(options[OPT_INTELEMENTS].cName,"sIntElements");
  sprintf(options[OPT_INTELEMENTS].cDescr,"Integration elements: hk (h,k,p,q) or es (e,s,varpi,Omega)");
  sprintf(options[OPT_INTELEMENTS].cDefault,"hk");
  options[OPT_INTELEMENTS].iType = 3;
  options[OPT_INTELEMENTS].iMultiFile = 0;
  fnRead[OPT_INTELEMENTS] = &ReadIntElements;
  
  sprintf(options[OPT_SHADOW].cName,"bUseShadow");
  sprintf(options[OPT_SHADOW].cDescr,"Use shadow particles for solar torque?");
  sprintf(options[OPT_SHADOW].cDefault,"No");
  options[OPT_SHADOW].dDefault = 0;
  options[OPT_SHADOW].iType = 0; 
  options[OPT_SHADOW].iMultiFile = 0;
  fnRead[OPT_SHADOW] = &ReadUseShadow;
  
  sprintf(options[OPT_OBLEVOL].cName,"bObliqEvol");
  sprintf(options[OPT_OBLEVOL].cDescr,"Calculate obliquity evolution");
  sprintf(options[OPT_OBLEVOL].cDefault,"No");
  options[OPT_OBLEVOL].dDefault = 0;
  options[OPT_OBLEVOL].iType = 0; 
  options[OPT_OBLEVOL].iMultiFile = 1;
  fnRead[OPT_OBLEVOL] = &ReadObliqEvol;
  
//   sprintf(options[OPT_DISTTERMS].cName,"bDisturbTerms");
//   sprintf(options[OPT_DISTTERMS].cDescr,"Use only given terms of disturbing function");
//   sprintf(options[OPT_DISTTERMS].cDefault,"No");
//   options[OPT_DISTTERMS].dDefault = 0;
//   options[OPT_DISTTERMS].iType = 0; 
//   options[OPT_DISTTERMS].iMultiFile = 0;
//   fnRead[OPT_DISTTERMS] = &ReadDistTerms;
  
  
}

void ReadOptionsLagrange(CONTROL *control,OPTIONS *options,BODY *body,SYSTEM *system,FILES *files,fnReadOption fnRead[]) {
  int iFile,iOpt;

  /* For each option, search the file for its occurence */
  /* is iLine necessary? */

  for (iFile=0;iFile<files->iNumInputs;iFile++) {
    /* First get Lagrange options */
    for (iOpt=OPTSTARTLAGRANGE;iOpt<OPTENDLAGRANGE;iOpt++) 
      if (options[iOpt].iType != -1) {
	//printf("%d\n", iOpt);
	fnRead[iOpt](control,body,system,&options[iOpt],files,iFile);
      }
  }
}
    

/******************* Verify LAGRANGE ******************/
void InitializeHKPQ(UPDATE *update,BODY *body, CONTROL *control) {
  int iBody,jBody,i,j;
   
  
  /* Body #0 updates */
  update[0].iNum = 0;   //Can this be zero? Central body does not evolve in Lagrange.
  InitializeUpdate(&update[0]);
  
  for (iBody=1; iBody<body[0].iNumBodies; iBody++) {
    /* Body #1 updatesla */
    if (body[iBody].bObliqEvol == 1) {
      if (control->bUseShadow == 1) {
	update[iBody].iNum = 9;
      } else {
	update[iBody].iNum = 7;
      }
    } else {
      update[iBody].iNum = 4;
    }
    InitializeUpdate(&update[iBody]);
    
    /* 0 -> h = e*sin(varpi) */
    update[iBody].iType[0] = 1;
    update[iBody].pdVar[0] = &body[iBody].dhecc;
    update[iBody].iNumEqns[0] = 1;
    update[iBody].dDeriv[0]=malloc(update[iBody].iNumEqns[0]*sizeof(double));
    update[iBody].bAng[0] = 0;   //do not set *both* bAng and bPolar = 1
    update[iBody].bPolar[0] = 1;
    
    /* 1 -> k = e*cos(varpi) */
    update[iBody].iType[1] = 1;
    update[iBody].pdVar[1] = &body[iBody].dkecc;
    update[iBody].iNumEqns[1] = 1;
    update[iBody].dDeriv[1]=malloc(update[iBody].iNumEqns[1]*sizeof(double));
    update[iBody].bAng[1] = 0;    //do not set *both* bAng and bPolar = 1
    update[iBody].bPolar[1] = 1;
    
    /* 2 -> p = s*sin(Omega) */
    update[iBody].iType[2] = 1;
    update[iBody].pdVar[2] = &body[iBody].dpinc;
    update[iBody].iNumEqns[2] = 1;
    update[iBody].dDeriv[2]=malloc(update[iBody].iNumEqns[2]*sizeof(double));
    update[iBody].bAng[2] = 0;   //do not set *both* bAng and bPolar = 1
    update[iBody].bPolar[2] = 1;
    
    /* 3 -> q = s*cos(Omega) */
    update[iBody].iType[3] = 1;
    update[iBody].pdVar[3] = &body[iBody].dqinc;
    update[iBody].iNumEqns[3] = 1;
    update[iBody].dDeriv[3]=malloc(update[iBody].iNumEqns[3]*sizeof(double));
    update[iBody].bAng[3] = 0;   //do not set *both* bAng and bPolar = 1
    update[iBody].bPolar[3] = 1;
    
    if (body[iBody].bObliqEvol == 1) {
      /* 4 -> beta = sin(obliq)*sin(precA) */
      update[iBody].iType[4] = 1;
      update[iBody].pdVar[4] = &body[iBody].dbeta;
      update[iBody].iNumEqns[4] = 1;
      update[iBody].dDeriv[4]=malloc(update[iBody].iNumEqns[4]*sizeof(double));
      update[iBody].bAng[4] = 0;   //do not set *both* bAng and bPolar = 1
      update[iBody].bPolar[4] = 1;
    
      /* 5 -> gamma = sin(obliq)*cos(precA) */
      update[iBody].iType[5] = 1;
      update[iBody].pdVar[5] = &body[iBody].dgamma;
      update[iBody].iNumEqns[5] = 1;
      update[iBody].dDeriv[5]=malloc(update[iBody].iNumEqns[5]*sizeof(double));
      update[iBody].bAng[5] = 0;   //do not set *both* bAng and bPolar = 1
      update[iBody].bPolar[5] = 1;
      
      /* 6 -> eta = cos(obliq) */
      update[iBody].iType[6] = 1;
      update[iBody].pdVar[6] = &body[iBody].deta;
      update[iBody].iNumEqns[6] = 1;
      update[iBody].dDeriv[6]=malloc(update[iBody].iNumEqns[6]*sizeof(double));
      update[iBody].bAng[6] = 0;   //do not set *both* bAng and bPolar = 1
      update[iBody].bPolar[6] = 1;
      
      if (control->bUseShadow == 1) {
	  /* 7 -> shadow beta */
	update[iBody].iType[7] = 1;
	update[iBody].pdVar[7] = &body[iBody].dShadowbeta;
	update[iBody].iNumEqns[7] = 1;
	update[iBody].dDeriv[7]=malloc(update[iBody].iNumEqns[7]*sizeof(double));
	update[iBody].bAng[7] = 0;   //do not set *both* bAng and bPolar = 1
	update[iBody].bPolar[7] = 1;
      
	/* 8 -> shadow gamma */
	update[iBody].iType[8] = 1;
	update[iBody].pdVar[8] = &body[iBody].dShadowgamma;
	update[iBody].iNumEqns[8] = 1;
	update[iBody].dDeriv[8]=malloc(update[iBody].iNumEqns[8]*sizeof(double));
	update[iBody].bAng[8] = 0;   //do not set *both* bAng and bPolar = 1
	update[iBody].bPolar[8] = 1;
      }
    }
    
    body[iBody].dPosition = malloc(3*sizeof(double));
    body[iBody].dVelocity = malloc(3*sizeof(double));
    //body[iBody].dLaplaceC = malloc((body[0].iNumBodies)*sizeof(double));
    
    
  }

}

void InitializeESVO(UPDATE *update,BODY *body) {
  int iBody,jBody,i,j;
   
  
  /* Body #0 updates */
  update[0].iNum = 0;   //Can this be zero? Central body does not evolve in Lagrange.
  InitializeUpdate(&update[0]);
  
  for (iBody=1; iBody<body[0].iNumBodies; iBody++) {
    /* Body #1 updatesla */
    update[iBody].iNum = 4;
    InitializeUpdate(&update[iBody]);
    
    /* 0 -> Eccentricity */
    update[iBody].iType[0] = 1;
    update[iBody].pdVar[0] = &body[iBody].dEcc;
    update[iBody].iNumEqns[0] = 1;
    update[iBody].dDeriv[0]=malloc(update[iBody].iNumEqns[0]*sizeof(double));
    update[iBody].bAng[0] = 0;    //do not set *both* bAng and bPolar = 1
    update[iBody].bPolar[0] = 0;
    
    /* 1 -> sin(1/2 * Inclination) */
    update[iBody].iType[1] = 1;
    update[iBody].pdVar[1] = &body[iBody].dsInc;
    update[iBody].iNumEqns[1] = 1;
    update[iBody].dDeriv[1]=malloc(update[iBody].iNumEqns[1]*sizeof(double));
    update[iBody].bAng[1] = 0;    //do not set *both* bAng and bPolar = 1
    update[iBody].bPolar[1] = 0;
    
    /* 2 -> Long. pericenter */
    update[iBody].iType[2] = 1;
    update[iBody].pdVar[2] = &body[iBody].dLongP;
    update[iBody].iNumEqns[2] = 1;
    update[iBody].dDeriv[2]=malloc(update[iBody].iNumEqns[2]*sizeof(double));
    update[iBody].bAng[2] = 1;    //do not set *both* bAng and bPolar = 1
    update[iBody].bPolar[2] = 0;
    
    /* 3 -> Long. Asc. Node */
    update[iBody].iType[3] = 1;
    update[iBody].pdVar[3] = &body[iBody].dLongA;
    update[iBody].iNumEqns[3] = 1;
    update[iBody].dDeriv[3]=malloc(update[iBody].iNumEqns[3]*sizeof(double));
    update[iBody].bAng[3] = 1;   //do not set *both* bAng and bPolar = 1
    update[iBody].bPolar[3] = 0;
    
    body[iBody].dPosition = malloc(3*sizeof(double));
    body[iBody].dVelocity = malloc(3*sizeof(double));
    //body[iBody].dLaplaceC = malloc((body[0].iNumBodies)*sizeof(double));
    
    
  }

}

/*
 *
 * Pericenter/Ascending node 
 *
 */

void VerifyPericenter(CONTROL *control,BODY *body,OPTIONS *options,int iBody,char cFile[],int iVerbose) {
  /* First see if longitude of ascending node and longitude of pericenter and nothing else set, i.e. the user input the default parameters */
  if (options[OPT_LONGA].iLine[iBody] > -1 && options[OPT_LONGP].iLine[iBody] > -1 && options[OPT_ARGP].iLine[iBody] == -1) 
    return;

  /* If none are set, raise error */
  if (options[OPT_LONGA].iLine[iBody] == -1 && options[OPT_LONGP].iLine[iBody] == -1 && options[OPT_ARGP].iLine[iBody] == -1) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Must set at least two of %s, %s, and %s in File: %s.\n",options[OPT_LONGA].cName,options[OPT_LONGP].cName,options[OPT_ARGP].cName, cFile);
    exit(EXIT_INPUT);
  }
  
  /* At least 2 must be set */
  if ((options[OPT_LONGA].iLine[iBody] == -1 && options[OPT_LONGP].iLine[iBody] == -1) || (options[OPT_LONGA].iLine[iBody] == -1 && options[OPT_ARGP].iLine[iBody] == -1) || (options[OPT_LONGP].iLine[iBody] == -1 && options[OPT_ARGP].iLine[iBody] == -1)) {
    if (iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Must set at least two of %s, %s, and %s in File: %s.\n",options[OPT_LONGA].cName,options[OPT_LONGP].cName,options[OPT_ARGP].cName, cFile);
    exit(EXIT_INPUT);
  }

  /* Were all set? */
  if (options[OPT_LONGA].iLine[iBody] > -1 && options[OPT_LONGP].iLine[iBody] > -1 && options[OPT_ARGP].iLine[iBody] > -1) {
    VerifyTripleExit(options[OPT_LONGA].cName,options[OPT_LONGP].cName,options[OPT_ARGP].cName,options[OPT_LONGA].iLine[iBody],options[OPT_LONGP].iLine[iBody],options[OPT_ARGP].iLine[iBody],cFile,iVerbose);
    exit(EXIT_INPUT);
  }

  /* Was LONGA set? */
  if (options[OPT_LONGA].iLine[iBody] > -1) {
    
    if (options[OPT_LONGP].iLine[iBody] > -1)
      /* LONGA and LONGP were the only two set - Nothing to do */
	 return;
    if (options[OPT_ARGP].iLine[iBody] > -1) 
      /* Must get radius from density */
      body->dLongP = fdCalcLongP(body->dLongA,body->dArgP);    
    return;
  }

  /* Was LongP set? */
  if (options[OPT_LONGP].iLine[iBody] > -1) {
    
    if (options[OPT_LONGA].iLine[iBody] > -1)
      /* LONGA and LONGP were the only two set - Nothing to do */
	 return;
    if (options[OPT_ARGP].iLine[iBody] > -1) 
      /* Must get radius from density */
      body->dLongA = fdCalcLongA(body->dLongP,body->dArgP);    
    return;
  }
}

void CalcHKPQ(BODY *body, int iBody) {
  body[iBody].dhecc = body[iBody].dEcc*sin(body[iBody].dLongP);
  body[iBody].dkecc = body[iBody].dEcc*cos(body[iBody].dLongP);
  body[iBody].dpinc = body[iBody].dsInc*sin(body[iBody].dLongA);
  body[iBody].dqinc = body[iBody].dsInc*cos(body[iBody].dLongA);
}

void CalcBeGa(BODY *body, int iBody) {
  body[iBody].dbeta = sin(body[iBody].dObliquity)*sin(body[iBody].dPrecA);
  body[iBody].dgamma = sin(body[iBody].dObliquity)*cos(body[iBody].dPrecA);
  body[iBody].deta = cos(body[iBody].dObliquity);
}

void CalcShadow(BODY *body, int iBody) {
  body[iBody].dShadowObl = body[iBody].dObliquity;
  body[iBody].dShadowPrecA = body[iBody].dPrecA;
  body[iBody].dShadowbeta = body[iBody].dbeta;
  body[iBody].dShadowgamma = body[iBody].dgamma;
}

void VerifyLagrange(CONTROL *control,OUTPUT *output,FILES *files,BODY *body,OPTIONS *options,GRIDS *grids,fnUpdateVariable ***fnUpdate,UPDATE *update) {
  int iBody,iTideLine,iEqn,iTideFile,iCol,iFile,j,i,jBody;

  for (iBody=1;iBody<control->iNumBodies;iBody++) {
      VerifyPericenter(control,&body[iBody],options,iBody+1,files->Infile[iBody+1].cIn,control->iVerbose);
  }
  
  /* Setup Semi-major axis functions (LaplaceF) for secular terms*/
  grids->fnLaplaceF = malloc(LAPLNUM*sizeof(fnLaplaceFunction*));
  for (j=0;j<LAPLNUM;j++) {
    grids->fnLaplaceF[j] = malloc(1*sizeof(fnLaplaceFunction));
  }
  grids->fnLaplaceF[0][0] = &fdSemiMajAxF1;
  grids->fnLaplaceF[1][0] = &fdSemiMajAxF2;
  grids->fnLaplaceF[2][0] = &fdSemiMajAxF3;
  grids->fnLaplaceF[3][0] = &fdSemiMajAxF4;
  grids->fnLaplaceF[4][0] = &fdSemiMajAxF5;
  grids->fnLaplaceF[5][0] = &fdSemiMajAxF6;
  grids->fnLaplaceF[6][0] = &fdSemiMajAxF7;
  grids->fnLaplaceF[7][0] = &fdSemiMajAxF8;
  grids->fnLaplaceF[8][0] = &fdSemiMajAxF9;
  grids->fnLaplaceF[9][0] = &fdSemiMajAxF10;
  grids->fnLaplaceF[10][0] = &fdSemiMajAxF11;
  grids->fnLaplaceF[11][0] = &fdSemiMajAxF12;
  grids->fnLaplaceF[12][0] = &fdSemiMajAxF13;
  grids->fnLaplaceF[13][0] = &fdSemiMajAxF14;
  grids->fnLaplaceF[14][0] = &fdSemiMajAxF15;
  grids->fnLaplaceF[15][0] = &fdSemiMajAxF16;
  grids->fnLaplaceF[16][0] = &fdSemiMajAxF17;
  grids->fnLaplaceF[17][0] = &fdSemiMajAxF18;
  grids->fnLaplaceF[18][0] = &fdSemiMajAxF19;
  grids->fnLaplaceF[19][0] = &fdSemiMajAxF20;
  grids->fnLaplaceF[20][0] = &fdSemiMajAxF21;
  grids->fnLaplaceF[21][0] = &fdSemiMajAxF22;
  grids->fnLaplaceF[22][0] = &fdSemiMajAxF23;
  grids->fnLaplaceF[23][0] = &fdSemiMajAxF24;
  grids->fnLaplaceF[24][0] = &fdSemiMajAxF25;
  grids->fnLaplaceF[25][0] = &fdSemiMajAxF26;
  
  grids->dmLaplaceC = malloc(Nchoosek(body[0].iNumBodies-1,2)*sizeof(double*));
  for (i=0;i<Nchoosek(body[0].iNumBodies-1,2);i++) {
    grids->dmLaplaceC[i] = malloc(LAPLNUM*sizeof(double));  
  }
  
  grids->imLaplaceN = malloc((body[0].iNumBodies)*sizeof(int*));
  for (i=1;i<body[0].iNumBodies;i++) {
    grids->imLaplaceN[i] = malloc((body[0].iNumBodies)*sizeof(int));
  }
  
  if (control->iIntElements == HKPQ) {
    InitializeHKPQ(update, body, control);
    control->dSysRot = 0.0; //need to get rid of this!!! Does it get called automatically?
    
    
    /* Body updates */
    for (iBody=1; iBody<control->iNumBodies; iBody++) {
      CalcHKPQ(body, iBody);
      
      /* 0 -> h = e*sin(varpi) */
      update[iBody].pdDhDt = &update[iBody].dDeriv[0][0];
      fnUpdate[iBody][0][0] = &fdLagrangeDhDt;
	
      /* 1 -> k = e*cos(varpi) */
      update[iBody].pdDkDt = &update[iBody].dDeriv[1][0];
      fnUpdate[iBody][1][0] = &fdLagrangeDkDt;
      
      /* 2 -> p = s*sin(Omega) */
      update[iBody].pdDpDt = &update[iBody].dDeriv[2][0];
      fnUpdate[iBody][2][0] = &fdLagrangeDpDt;
      
      /* 3 -> q = s*sin(Omega) */
      update[iBody].pdDqDt = &update[iBody].dDeriv[3][0];
      fnUpdate[iBody][3][0] = &fdLagrangeDqDt;
      
      if (body[iBody].bObliqEvol == 1) {
	CalcBeGa(body, iBody);
	/* 4 -> beta = sin(obl) sin(pA) */
	update[iBody].pdDbetaDt = &update[iBody].dDeriv[4][0];
	fnUpdate[iBody][4][0] = &fdLagrangeDbetaDt;
	
	/* 5 -> gamma = sin(obl) cos(pA) */
	update[iBody].pdDgammaDt = &update[iBody].dDeriv[5][0];
	fnUpdate[iBody][5][0] = &fdLagrangeDgammaDt;
	
	/* 6 -> eta = cos(obl) */
	update[iBody].pdDetaDt = &update[iBody].dDeriv[6][0];
	fnUpdate[iBody][6][0] = &fdLagrangeDetaDt;
      
	if (control->bUseShadow == 1) {
	  CalcShadow(body, iBody);  // set up variables for shadow particle (only feels solar torque)
	  update[iBody].pdDShbetaDt = &update[iBody].dDeriv[7][0];
	  fnUpdate[iBody][7][0] = &fdLagrangeDShadowbetaDt;
	  
	  update[iBody].pdDShgammaDt = &update[iBody].dDeriv[8][0];
	  fnUpdate[iBody][8][0] = &fdLagrangeDShadowgammaDt;
	}
      }
	
      for (jBody=iBody+1;jBody<body[0].iNumBodies;jBody++) {
	for (j=0;j<LAPLNUM;j++) {
	  if (body[iBody].dSemi < body[jBody].dSemi) {
	    grids->imLaplaceN[iBody][jBody] = CombCount(iBody,jBody,body[0].iNumBodies-1);
	    grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][j] = grids->fnLaplaceF[j][0](body[iBody].dSemi/body[jBody].dSemi, 0);
	  } else if (body[iBody].dSemi > body[jBody].dSemi) {
	    grids->imLaplaceN[iBody][jBody] = CombCount(iBody,jBody,body[0].iNumBodies-1);
	    grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][j] = grids->fnLaplaceF[j][0](body[jBody].dSemi/body[iBody].dSemi, 0);
	  }
	}
      }
    }
  }
    
  if (control->iIntElements == ESVO) {
    InitializeESVO(update, body);
    control->dSysRot = 0.0;
    
    for (iBody=1; iBody<control->iNumBodies; iBody++) {
      /* 0 -> Eccentricity */
      update[iBody].pdDeccDt = &update[iBody].dDeriv[0][0];
      fnUpdate[iBody][0][0] = &fdLagrangeDeccDt;
	
      /* 1 -> sin(1/2 * Inclination) */
      update[iBody].pdDsIncDt = &update[iBody].dDeriv[1][0];
      fnUpdate[iBody][1][0] = &fdLagrangeDsIncDt;
      
      /* 2 -> Long. pericenter */
      update[iBody].pdDvarDt = &update[iBody].dDeriv[2][0];
      fnUpdate[iBody][2][0] = &fdLagrangeDvarDt;
      
      /* 3 -> Long. Asc. Node */
      update[iBody].pdDOmgDt = &update[iBody].dDeriv[3][0];
      fnUpdate[iBody][3][0] = &fdLagrangeDOmgDt;

      for (jBody=iBody+1;jBody<body[0].iNumBodies;jBody++) {
	for (j=0;j<LAPLNUM;j++) {
	  if (body[iBody].dSemi < body[jBody].dSemi) {
	    grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][j] = grids->fnLaplaceF[j][0](body[iBody].dSemi/body[jBody].dSemi, 0);
	  } else if (body[iBody].dSemi > body[jBody].dSemi) {
	    grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][j] = grids->fnLaplaceF[j][0](body[jBody].dSemi/body[iBody].dSemi, 0);
	  }
	}
      }
    }
  }
}


void InitializeVplanetLagrange(CONTROL *control,VPLANET *vplanet) {
  if (control->iIntElements == HKPQ) 
    InitializeHKPQ(vplanet->tmpUpdate,vplanet->tmpBody, control);
  if (control->iIntElements == ESVO)
    InitializeESVO(vplanet->tmpUpdate,vplanet->tmpBody);
}

/***************** LAGRANGE Halts *****************/


int HaltNegInc(CONTROL *control, BODY *body, UPDATE *update, int iBody) {
  if (body[iBody].dsInc < 0.0) {
    if (control->iVerbose >= VERBPROG) {
      printf("HALT: Negative inclination of planet %s at ", body[iBody].cName);
      fprintd(stdout,control->Integr.dTime/YEARSEC,control->iSciNot,control->iDigits);
      printf(" years.\n");
    }
    return 1;
  }
  
  return 0; 
}

void InitializeHaltLagrange(HALT *halt,VPLANET *vplanet,int iNumInputs,int *iHaltSys,int *iHaltBody) {

  vplanet->fnHaltBody[(*iHaltBody)++] = &HaltNegInc;
}


void ForceBehaviorLagrange(BODY *body,CONTROL *control,GRIDS *grids,double dDt) {
  int iBody;
  double minsInc = 1.e-7, theta = 15.0;
 
  for (iBody=1; iBody<control->iNumBodies; iBody++) {
    if (body[iBody].dsInc < minsInc) {
      fvPropertiesLagrange(control,body,grids,dDt); 
      printf("Note: s < %e for %s. Performing %f degree rotation.\n", minsInc, body[iBody].cName, theta);
      fvRotateX(body,theta*PI/180.0);
      control->dSysRot += theta;
    }
    
    while (body[iBody].dLongA >= 2*PI) {
      body[iBody].dLongA -= 2*PI;
    }
    while (body[iBody].dLongA < 0.0) {
      body[iBody].dLongA += 2*PI;
    }
    
    while (body[iBody].dLongP >= 2*PI) {
      body[iBody].dLongP -= 2*PI;
    }
    while (body[iBody].dLongP < 0.0) {
      body[iBody].dLongP += 2*PI;
    }
    
    while (body[iBody].dMeanL >= 2*PI) {
      body[iBody].dMeanL -= 2*PI;
    }
    while (body[iBody].dMeanL < 0.0) {
      body[iBody].dMeanL += 2*PI;
    }
    
    
  }
}

/************* LAGRANGE Outputs ******************/

void HelpOutputLagrange(OUTPUT *output) {
  int i;

  printf("\n ------ LAGRANGE output ------\n");
  for (i=OUTSTARTLAGRANGE;i<OUTENDLAGRANGE;i++) 
    WriteHelpOutput(output[i]);
}

void WriteBodyDeccDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDeccDt(body, grids, iBody)*365.25*86400.;
  }
}

void WriteBodyDsIncDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDsIncDt(body, grids, iBody)*365.25*86400.;
  }
}  
  
void WriteBodyDvarDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDvarDt(body, grids, iBody)*365.25*86400.;
  }
}  

void WriteBodyDOmgDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDOmgDt(body, grids, iBody)*365.25*86400.;
  }
} 

void WriteBodyDOblDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDOblDt(body, grids, iBody)*365.25*86400.;
  }
} 

void WriteBodysInc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dsInc;
}  

void WriteBodyInc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  body[iBody].dInc = 2.*asin(body[iBody].dsInc);  
  *dTmp = body[iBody].dInc / DEGRAD;
}  

void WriteBodyLongA(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dLongA / DEGRAD;
}  

void WriteBodyLongP(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dLongP / DEGRAD;
}  

void WriteBodyArgP(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  body[iBody].dArgP = body[iBody].dLongP - body[iBody].dLongA;
  
  while (body[iBody].dArgP < 0.0) {
    body[iBody].dArgP += 2*PI;
  }
  
  *dTmp = body[iBody].dArgP / DEGRAD;
}    

void WriteBodyMeanA(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  body[iBody].dMeanA = body[iBody].dMeanL - body[iBody].dLongP;
 
  while (body[iBody].dMeanA < 0.0) {
    body[iBody].dMeanA += 2*PI;
  }
  
  *dTmp = body[iBody].dMeanA / DEGRAD;
}    

void WriteBodyMeanL(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dMeanL / DEGRAD;
}    

void WriteBodyTrueA(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dTrueA / DEGRAD;
}   

void WriteBodyTrueL(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dTrueL / DEGRAD;
}   

void WriteBodyEccA(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dEccA / DEGRAD;
}   

void WriteBodyPrecA(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
 
  while (body[iBody].dPrecA < 0.0) {
    body[iBody].dPrecA += 2*PI;
  }
  
  while (body[iBody].dPrecA > 2*PI) {
    body[iBody].dPrecA -= 2*PI;
  }
  
  *dTmp = body[iBody].dPrecA / DEGRAD;
}    

void WriteShadowObl(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS *grids,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dShadowObl;
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdAngleUnit(units->iAngle);
    fvAngleUnit(units->iAngle,cUnit);
  }
}

void WriteShadowPrecA(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
 
  while (body[iBody].dShadowPrecA < 0.0) {
    body[iBody].dShadowPrecA += 2*PI;
  }
  
  while (body[iBody].dShadowPrecA > 2*PI) {
    body[iBody].dShadowPrecA -= 2*PI;
  }
  
  *dTmp = body[iBody].dShadowPrecA / DEGRAD;
}    

void WriteBodyhecc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dhecc;
}  

void WriteBodykecc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dkecc;
} 

void WriteBodypinc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dpinc;
} 

void WriteBodyqinc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dqinc;
} 

void WriteBodyDheccDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDhDt(body, grids, iBody)*365.25*86400.;
  }
}

void WriteBodyDkeccDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDkDt(body, grids, iBody)*365.25*86400.;
  }
}

void WriteBodyDpincDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDpDt(body, grids, iBody)*365.25*86400.;
  }
}

void WriteBodyDqincDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDqDt(body, grids, iBody)*365.25*86400.;
  }
}

void WriteBodyDbetaDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDbetaDt(body, grids, iBody)*365.25*86400.;
  }
}

void WriteBodyDgammaDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDgammaDt(body, grids, iBody)*365.25*86400.;
  }
}

void WriteBodyDetaDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDetaDt(body, grids, iBody)*365.25*86400.;
  }
}

void WriteBodyDPrecADtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDPrecADt(body, grids, iBody)*365.25*86400.;
  }
}

void WriteBodyCenTorqLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdCentralTorqueR(body, iBody)*365.25;
  }
}

void WriteDpADt_SolTorq(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = dpAdt_SolTorq(body, grids, iBody)*365.25*86400.;
  }
}

void WriteDpADt_R01(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = dpAdt_R01(body, grids, iBody)*365.25*86400.;
  }
}

void WriteDpADt_R02(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = dpAdt_R02(body, grids, iBody)*365.25*86400.;
  }
}

void WriteDOblDt_R01(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = dObldt_R01(body, grids, iBody)*365.25*86400.;
  }
}

void WriteDOblDt_R02(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = dObldt_R02(body, grids, iBody)*365.25*86400.;
  }
}

void WriteC1_2term(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  //Temporary hack to give this constant for planet and its next exterior companion (2 term disturbing fxn analysis) 
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = body[iBody].dEcc*body[iBody+1].dEcc*grids->dmLaplaceC[grids->imLaplaceN[iBody][iBody+1]][9] + pow(body[iBody].dEcc,3)*body[iBody+1].dEcc*grids->dmLaplaceC[grids->imLaplaceN[iBody][iBody+1]][10] + pow(body[iBody+1].dEcc,3)*body[iBody].dEcc*grids->dmLaplaceC[grids->imLaplaceN[iBody][iBody+1]][11] + body[iBody].dEcc*body[iBody+1].dEcc*(pow(body[iBody].dsInc,2) + pow(body[iBody+1].dsInc,2)) * grids->dmLaplaceC[grids->imLaplaceN[iBody][iBody+1]][12];
  }
}

void WriteC2_2term(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  //Temporary hack to give this constant for planet and its next exterior companion (2 term, 2 planet disturbing fxn analysis) 
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = body[iBody].dsInc*grids->dmLaplaceC[grids->imLaplaceN[iBody][iBody+1]][2] + (pow(body[iBody].dEcc,2)*body[iBody].dsInc+pow(body[iBody+1].dEcc,2)*body[iBody].dsInc) * grids->dmLaplaceC[grids->imLaplaceN[iBody][iBody+1]][6] + 2.*pow(body[iBody].dsInc,3) * grids->dmLaplaceC[grids->imLaplaceN[iBody][iBody+1]][7] + body[iBody].dsInc*pow(body[iBody+1].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[iBody][iBody+1]][8];
  }
}
  
void WriteC3_2term(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS* grids,int iBody,double *dTmp,char cUnit[]) {
  //Temporary hack to give this constant for planet and its next exterior companion (2 term, 2 planet disturbing fxn analysis) 
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = body[iBody].dEcc*body[iBody+1].dEcc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][iBody+1]][12];
  }
}


void InitializeOutputLagrange(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_BODYDECCDTLAGRANGE].cName,"BodyDeccDtLagrange");
  sprintf(output[OUT_BODYDECCDTLAGRANGE].cDescr,"Body's decc/dt in Lagrange");
  sprintf(output[OUT_BODYDECCDTLAGRANGE].cNeg,"1/s");
  output[OUT_BODYDECCDTLAGRANGE].bNeg = 1;
  output[OUT_BODYDECCDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_BODYDECCDTLAGRANGE].iNum = 1;
  fnWrite[OUT_BODYDECCDTLAGRANGE] = &WriteBodyDeccDtLagrange;
  
  sprintf(output[OUT_BODYDSINCDTLAGRANGE].cName,"BodyDsIncDtLagrange");
  sprintf(output[OUT_BODYDSINCDTLAGRANGE].cDescr,"Body's dsin(.5*Inc)/dt in Lagrange");
  sprintf(output[OUT_BODYDSINCDTLAGRANGE].cNeg,"1/s");
  output[OUT_BODYDSINCDTLAGRANGE].bNeg = 1;
  output[OUT_BODYDSINCDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_BODYDSINCDTLAGRANGE].iNum = 1;
  fnWrite[OUT_BODYDSINCDTLAGRANGE] = &WriteBodyDsIncDtLagrange;
  
  sprintf(output[OUT_BODYDVARDTLAGRANGE].cName,"BodyDvarDtLagrange");
  sprintf(output[OUT_BODYDVARDTLAGRANGE].cDescr,"Body's dvarpi/dt in Lagrange");
  sprintf(output[OUT_BODYDVARDTLAGRANGE].cNeg,"1/s");
  output[OUT_BODYDVARDTLAGRANGE].bNeg = 1;
  output[OUT_BODYDVARDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_BODYDVARDTLAGRANGE].iNum = 1;
  fnWrite[OUT_BODYDVARDTLAGRANGE] = &WriteBodyDvarDtLagrange;
  
  sprintf(output[OUT_BODYDOMGDTLAGRANGE].cName,"BodyDOmgDtLagrange");
  sprintf(output[OUT_BODYDOMGDTLAGRANGE].cDescr,"Body's dOmega/dt in Lagrange");
  sprintf(output[OUT_BODYDOMGDTLAGRANGE].cNeg,"1/s");
  output[OUT_BODYDOMGDTLAGRANGE].bNeg = 1;
  output[OUT_BODYDOMGDTLAGRANGE].dNeg = DEGRAD;
  output[OUT_BODYDOMGDTLAGRANGE].iNum = 1;
  fnWrite[OUT_BODYDOMGDTLAGRANGE] = &WriteBodyDOmgDtLagrange;
  
  sprintf(output[OUT_BODYDOBLDTLAGRANGE].cName,"BodyDOblDtLagrange");
  sprintf(output[OUT_BODYDOBLDTLAGRANGE].cDescr,"Body's dObliq/dt in Lagrange");
  sprintf(output[OUT_BODYDOBLDTLAGRANGE].cNeg,"1/s");
  output[OUT_BODYDOBLDTLAGRANGE].bNeg = 1;
  output[OUT_BODYDOBLDTLAGRANGE].dNeg = DEGRAD;
  output[OUT_BODYDOBLDTLAGRANGE].iNum = 1;
  fnWrite[OUT_BODYDOBLDTLAGRANGE] = &WriteBodyDOblDtLagrange;
  
  sprintf(output[OUT_BODYINC].cName,"Inc");
  sprintf(output[OUT_BODYINC].cDescr,"Body's Inclination in Lagrange");
  sprintf(output[OUT_BODYINC].cNeg,"Deg");
  output[OUT_BODYINC].bNeg = 1;
  output[OUT_BODYINC].dNeg = DEGRAD;
  output[OUT_BODYINC].iNum = 1;
  fnWrite[OUT_BODYINC] = &WriteBodyInc;
  
  sprintf(output[OUT_BODYSINC].cName,"sInc");
  sprintf(output[OUT_BODYSINC].cDescr,"Body's sin(1/2*Inclination) in Lagrange");
  sprintf(output[OUT_BODYSINC].cNeg,"Deg");
  output[OUT_BODYSINC].bNeg = 1;
  output[OUT_BODYSINC].dNeg = DEGRAD;
  output[OUT_BODYSINC].iNum = 1;
  fnWrite[OUT_BODYSINC] = &WriteBodysInc;
  
  sprintf(output[OUT_BODYLONGA].cName,"LongA");
  sprintf(output[OUT_BODYLONGA].cDescr,"Body's Longitude of ascending node in Lagrange");
  sprintf(output[OUT_BODYLONGA].cNeg,"Deg");
  output[OUT_BODYLONGA].bNeg = 1;
  output[OUT_BODYLONGA].dNeg = DEGRAD;
  output[OUT_BODYLONGA].iNum = 1;
  fnWrite[OUT_BODYLONGA] = &WriteBodyLongA;
  
//   sprintf(output[OUT_BODYLONGP].cName,"LongP");
//   sprintf(output[OUT_BODYLONGP].cDescr,"Body's Longitude of pericenter in Lagrange");
//   sprintf(output[OUT_BODYLONGP].cNeg,"Deg");
//   output[OUT_BODYLONGP].bNeg = 1;
//   output[OUT_BODYLONGP].dNeg = DEGRAD;
//   output[OUT_BODYLONGP].iNum = 1;
//   fnWrite[OUT_BODYLONGP] = &WriteBodyLongP; 
  
  sprintf(output[OUT_BODYARGP].cName,"ArgP");
  sprintf(output[OUT_BODYARGP].cDescr,"Body's argument of pericenter in Lagrange");
  sprintf(output[OUT_BODYARGP].cNeg,"Deg");
  output[OUT_BODYARGP].bNeg = 1;
  output[OUT_BODYARGP].dNeg = DEGRAD;
  output[OUT_BODYARGP].iNum = 1;
  fnWrite[OUT_BODYARGP] = &WriteBodyArgP; 
  
  sprintf(output[OUT_BODYMEANA].cName,"MeanA");
  sprintf(output[OUT_BODYMEANA].cDescr,"Body's mean anomaly in Lagrange");
  sprintf(output[OUT_BODYMEANA].cNeg,"Deg");
  output[OUT_BODYMEANA].bNeg = 1;
  output[OUT_BODYMEANA].dNeg = DEGRAD;
  output[OUT_BODYMEANA].iNum = 1;
  fnWrite[OUT_BODYMEANA] = &WriteBodyMeanA;
  
  sprintf(output[OUT_BODYMEANL].cName,"MeanL");
  sprintf(output[OUT_BODYMEANL].cDescr,"Body's mean longitude in Lagrange");
  sprintf(output[OUT_BODYMEANL].cNeg,"Deg");
  output[OUT_BODYMEANL].bNeg = 1;
  output[OUT_BODYMEANL].dNeg = DEGRAD;
  output[OUT_BODYMEANL].iNum = 1;
  fnWrite[OUT_BODYMEANL] = &WriteBodyMeanL; 
  
  sprintf(output[OUT_BODYTRUEL].cName,"TrueL");
  sprintf(output[OUT_BODYTRUEL].cDescr,"Body's true longitude in Lagrange");
  sprintf(output[OUT_BODYTRUEL].cNeg,"Deg");
  output[OUT_BODYTRUEL].bNeg = 1;
  output[OUT_BODYTRUEL].dNeg = DEGRAD;
  output[OUT_BODYTRUEL].iNum = 1;
  fnWrite[OUT_BODYTRUEL] = &WriteBodyTrueL; 
  
  sprintf(output[OUT_BODYTRUEA].cName,"TrueA");
  sprintf(output[OUT_BODYTRUEA].cDescr,"Body's true anomaly in Lagrange");
  sprintf(output[OUT_BODYTRUEA].cNeg,"Deg");
  output[OUT_BODYTRUEA].bNeg = 1;
  output[OUT_BODYTRUEA].dNeg = DEGRAD;
  output[OUT_BODYTRUEA].iNum = 1;
  fnWrite[OUT_BODYTRUEA] = &WriteBodyTrueA; 
  
//   sprintf(output[OUT_BODYECCA].cName,"EccA");
//   sprintf(output[OUT_BODYECCA].cDescr,"Body's eccentric anomaly in Lagrange");
//   sprintf(output[OUT_BODYECCA].cNeg,"Deg");
//   output[OUT_BODYECCA].bNeg = 1;
//   output[OUT_BODYECCA].dNeg = DEGRAD;
//   output[OUT_BODYECCA].iNum = 1;
//   fnWrite[OUT_BODYECCA] = &WriteBodyEccA; 

  sprintf(output[OUT_BODYHECC].cName,"hecc");
  sprintf(output[OUT_BODYHECC].cDescr,"Body's h = e*sin(varpi) in Lagrange");
  sprintf(output[OUT_BODYHECC].cNeg,"0");
  output[OUT_BODYHECC].bNeg = 0;
  output[OUT_BODYHECC].iNum = 1;
  fnWrite[OUT_BODYHECC] = &WriteBodyhecc;
  
  sprintf(output[OUT_BODYKECC].cName,"kecc");
  sprintf(output[OUT_BODYKECC].cDescr,"Body's k = e*cos(varpi) in Lagrange");
  sprintf(output[OUT_BODYKECC].cNeg,"0");
  output[OUT_BODYKECC].bNeg = 0;
  output[OUT_BODYKECC].iNum = 1;
  fnWrite[OUT_BODYKECC] = &WriteBodykecc;
  
  sprintf(output[OUT_BODYPINC].cName,"pinc");
  sprintf(output[OUT_BODYPINC].cDescr,"Body's p = s*sin(Omega) in Lagrange");
  sprintf(output[OUT_BODYPINC].cNeg,"0");
  output[OUT_BODYPINC].bNeg = 0;
  output[OUT_BODYPINC].iNum = 1;
  fnWrite[OUT_BODYPINC] = &WriteBodypinc;
  
  sprintf(output[OUT_BODYQINC].cName,"qinc");
  sprintf(output[OUT_BODYQINC].cDescr,"Body's q = s* cos(Omega) in Lagrange");
  sprintf(output[OUT_BODYQINC].cNeg,"0");
  output[OUT_BODYQINC].bNeg = 0;
  output[OUT_BODYQINC].iNum = 1;
  fnWrite[OUT_BODYQINC] = &WriteBodyqinc;
  
  sprintf(output[OUT_BODYPRECA].cName,"PrecA");
  sprintf(output[OUT_BODYPRECA].cDescr,"Body's precession angle");
  sprintf(output[OUT_BODYPRECA].cNeg,"0");
  output[OUT_BODYPRECA].bNeg = 0;
  output[OUT_BODYPRECA].iNum = 1;
  fnWrite[OUT_BODYPRECA] = &WriteBodyPrecA;  
  
  sprintf(output[OUT_SHADOWPRECA].cName,"ShPrecA");
  sprintf(output[OUT_SHADOWPRECA].cDescr,"shadow's precession angle");
  sprintf(output[OUT_SHADOWPRECA].cNeg,"0");
  output[OUT_SHADOWPRECA].bNeg = 0;
  output[OUT_SHADOWPRECA].iNum = 1;
  fnWrite[OUT_SHADOWPRECA] = &WriteShadowPrecA;  
  
  sprintf(output[OUT_SHADOWOBL].cName,"ShObl");
  sprintf(output[OUT_SHADOWOBL].cDescr,"shadow's obliquity");
  sprintf(output[OUT_SHADOWOBL].cNeg,"0");
  output[OUT_SHADOWOBL].bNeg = 0;
  output[OUT_SHADOWOBL].iNum = 1;
  fnWrite[OUT_SHADOWOBL] = &WriteShadowObl;  
  
  sprintf(output[OUT_BODYDHDTLAGRANGE].cName,"CheezeWhiz");
  sprintf(output[OUT_BODYDHDTLAGRANGE].cDescr,"Body's dh/dt in Lagrange");
  sprintf(output[OUT_BODYDHDTLAGRANGE].cNeg,"1/s");
  output[OUT_BODYDHDTLAGRANGE].bNeg = 1;
  output[OUT_BODYDHDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_BODYDHDTLAGRANGE].iNum = 1;
  fnWrite[OUT_BODYDHDTLAGRANGE] = &WriteBodyDheccDtLagrange;
  
  sprintf(output[OUT_BODYDKDTLAGRANGE].cName,"CornChips");
  sprintf(output[OUT_BODYDKDTLAGRANGE].cDescr,"Body's dk/dt in Lagrange");
  sprintf(output[OUT_BODYDKDTLAGRANGE].cNeg,"1/s");
  output[OUT_BODYDKDTLAGRANGE].bNeg = 1;
  output[OUT_BODYDKDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_BODYDKDTLAGRANGE].iNum = 1;
  fnWrite[OUT_BODYDKDTLAGRANGE] = &WriteBodyDkeccDtLagrange;
  
  sprintf(output[OUT_BODYDPDTLAGRANGE].cName,"Liar");
  sprintf(output[OUT_BODYDPDTLAGRANGE].cDescr,"Body's dp/dt in Lagrange");
  sprintf(output[OUT_BODYDPDTLAGRANGE].cNeg,"1/s");
  output[OUT_BODYDPDTLAGRANGE].bNeg = 1;
  output[OUT_BODYDPDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_BODYDPDTLAGRANGE].iNum = 1;
  fnWrite[OUT_BODYDPDTLAGRANGE] = &WriteBodyDpincDtLagrange;
  
  sprintf(output[OUT_DBETADT].cName,"DBetaDt");
  sprintf(output[OUT_DBETADT].cDescr,"Body's dbeta/dt in Lagrange");
  sprintf(output[OUT_DBETADT].cNeg,"1/s");
  output[OUT_DBETADT].bNeg = 1;
  output[OUT_DBETADT].dNeg = YEARSEC;
  output[OUT_DBETADT].iNum = 1;
  fnWrite[OUT_DBETADT] = &WriteBodyDbetaDtLagrange;
  
  sprintf(output[OUT_DGAMMADT].cName,"DGammaDt");
  sprintf(output[OUT_DGAMMADT].cDescr,"Body's dgamma/dt in Lagrange");
  sprintf(output[OUT_DGAMMADT].cNeg,"1/s");
  output[OUT_DGAMMADT].bNeg = 1;
  output[OUT_DGAMMADT].dNeg = YEARSEC;
  output[OUT_DGAMMADT].iNum = 1;
  fnWrite[OUT_DGAMMADT] = &WriteBodyDgammaDtLagrange;
  
  sprintf(output[OUT_DETADT].cName,"DEtaDt");
  sprintf(output[OUT_DETADT].cDescr,"Body's deta/dt in Lagrange");
  sprintf(output[OUT_DETADT].cNeg,"1/s");
  output[OUT_DETADT].bNeg = 1;
  output[OUT_DETADT].dNeg = YEARSEC;
  output[OUT_DETADT].iNum = 1;
  fnWrite[OUT_DETADT] = &WriteBodyDetaDtLagrange;
  
  sprintf(output[OUT_BODYDQDTLAGRANGE].cName,"LikeMeOnFacebook");
  sprintf(output[OUT_BODYDQDTLAGRANGE].cDescr,"Body's dq/dt in Lagrange");
  sprintf(output[OUT_BODYDQDTLAGRANGE].cNeg,"1/s");
  output[OUT_BODYDQDTLAGRANGE].bNeg = 1;
  output[OUT_BODYDQDTLAGRANGE].dNeg = YEARSEC;
  output[OUT_BODYDQDTLAGRANGE].iNum = 1;
  fnWrite[OUT_BODYDQDTLAGRANGE] = &WriteBodyDqincDtLagrange;
  
  sprintf(output[OUT_C1R2].cName,"Irukandji");
  sprintf(output[OUT_C1R2].cDescr,"2 term C1 constant");
  sprintf(output[OUT_C1R2].cNeg,"1/s");
  output[OUT_C1R2].bNeg = 1;
  output[OUT_C1R2].dNeg = YEARSEC;
  output[OUT_C1R2].iNum = 1;
  fnWrite[OUT_C1R2] = &WriteC1_2term;
  
  sprintf(output[OUT_C2R2].cName,"Thylacine");
  sprintf(output[OUT_C2R2].cDescr,"2 term C2 constant");
  sprintf(output[OUT_C2R2].cNeg,"1/s");
  output[OUT_C2R2].bNeg = 1;
  output[OUT_C2R2].dNeg = YEARSEC;
  output[OUT_C2R2].iNum = 1;
  fnWrite[OUT_C2R2] = &WriteC2_2term;
  
  sprintf(output[OUT_C3R2].cName,"Coelacanth");
  sprintf(output[OUT_C3R2].cDescr,"2 term C3 constant");
  sprintf(output[OUT_C3R2].cNeg,"1/s");
  output[OUT_C3R2].bNeg = 1;
  output[OUT_C3R2].dNeg = YEARSEC;
  output[OUT_C3R2].iNum = 1;
  fnWrite[OUT_C3R2] = &WriteC3_2term;
  
  sprintf(output[OUT_BODYDPRECADTLAGRANGE].cName,"BodyDPrecADtLagrange");
  sprintf(output[OUT_BODYDPRECADTLAGRANGE].cDescr,"Body's dpA/dt in Lagrange");
  sprintf(output[OUT_BODYDPRECADTLAGRANGE].cNeg,"1/s");
  output[OUT_BODYDPRECADTLAGRANGE].bNeg = 1;
  output[OUT_BODYDPRECADTLAGRANGE].dNeg = YEARSEC;
  output[OUT_BODYDPRECADTLAGRANGE].iNum = 1;
  fnWrite[OUT_BODYDPRECADTLAGRANGE] = &WriteBodyDPrecADtLagrange;
  
  sprintf(output[OUT_BODYCENTQLAGRANGE].cName,"BodyCenTorqLagrange");
  sprintf(output[OUT_BODYCENTQLAGRANGE].cDescr,"Body's central torque in Lagrange");
  sprintf(output[OUT_BODYCENTQLAGRANGE].cNeg,"1/s");
  output[OUT_BODYCENTQLAGRANGE].bNeg = 1;
  output[OUT_BODYCENTQLAGRANGE].dNeg = YEARSEC;
  output[OUT_BODYCENTQLAGRANGE].iNum = 1;
  fnWrite[OUT_BODYCENTQLAGRANGE] = &WriteBodyCenTorqLagrange;
  
  sprintf(output[OUT_DPADTSOLTQ].cName,"SolTorq");
  sprintf(output[OUT_DPADTSOLTQ].cDescr,"Body's dpA/dt with central torque only");
  sprintf(output[OUT_DPADTSOLTQ].cNeg,"1/s");
  output[OUT_DPADTSOLTQ].bNeg = 1;
  output[OUT_DPADTSOLTQ].dNeg = YEARSEC;
  output[OUT_DPADTSOLTQ].iNum = 1;
  fnWrite[OUT_DPADTSOLTQ] = &WriteDpADt_SolTorq;
  
  sprintf(output[OUT_DPADTR01].cName,"pR01");
  sprintf(output[OUT_DPADTR01].cDescr,"Body's dpA/dt from R01");
  sprintf(output[OUT_DPADTR01].cNeg,"1/s");
  output[OUT_DPADTR01].bNeg = 1;
  output[OUT_DPADTR01].dNeg = YEARSEC;
  output[OUT_DPADTR01].iNum = 1;
  fnWrite[OUT_DPADTR01] = &WriteDpADt_R01;
  
  sprintf(output[OUT_DPADTR02].cName,"pr02");
  sprintf(output[OUT_DPADTR02].cDescr,"Body's dpA/dt from R02");
  sprintf(output[OUT_DPADTR02].cNeg,"1/s");
  output[OUT_DPADTR02].bNeg = 1;
  output[OUT_DPADTR02].dNeg = YEARSEC;
  output[OUT_DPADTR02].iNum = 1;
  fnWrite[OUT_DPADTR02] = &WriteDpADt_R02;
  
  sprintf(output[OUT_DOBLDTR01].cName,"OR01");
  sprintf(output[OUT_DOBLDTR01].cDescr,"Body's dObl/dt from R01");
  sprintf(output[OUT_DOBLDTR01].cNeg,"1/s");
  output[OUT_DOBLDTR01].bNeg = 1;
  output[OUT_DOBLDTR01].dNeg = YEARSEC;
  output[OUT_DOBLDTR01].iNum = 1;
  fnWrite[OUT_DOBLDTR01] = &WriteDOblDt_R01;
  
  sprintf(output[OUT_DOBLDTR02].cName,"Or02");
  sprintf(output[OUT_DOBLDTR02].cDescr,"Body's dObl/dt from R02");
  sprintf(output[OUT_DOBLDTR02].cNeg,"1/s");
  output[OUT_DOBLDTR02].bNeg = 1;
  output[OUT_DOBLDTR02].dNeg = YEARSEC;
  output[OUT_DOBLDTR02].iNum = 1;
  fnWrite[OUT_DOBLDTR02] = &WriteDOblDt_R02;
  
}


/************ LAGRANGE Logging Functions **************/

void LogOptionsLagrange(CONTROL *control, FILE *fp) {

  fprintf(fp,"-------- LAGRANGE Options -----\n\n");
  /* Tidal Model */
  fprintf(fp,"Tidal Model: ");
  if (control->iTideModel == CPL) {
    fprintf(fp,"Constant-Phase-Lag, 2nd order\n");
    fprintf(fp,"Use Discrete Rotation Rate Model: %d\n",control->Integr.bDiscreteRot);
  } 

  if (control->iTideModel == CTL) {
    fprintf(fp,"Constant-Time-Lag, 8th order\n");
  }
}

void LogLagrange(CONTROL *control,OUTPUT *output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS *grids,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  fprintf(fp,"\n----- LAGRANGE PARAMETERS ------\n");
  for (iOut=OUTSTARTLAGRANGE;iOut<OUTBODYSTARTLAGRANGE;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,grids,0);
  }
}

void LogBodyLagrange(CONTROL *control,OUTPUT *output,BODY *body,SYSTEM *system,UPDATE *update,GRIDS *grids,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- LAGRANGE PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTLAGRANGE;iOut<OUTENDLAGRANGE;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,grids,iBody);
  }
}

void fvPropertiesLagrange(CONTROL *control,BODY *body,GRIDS *grids,double dDt) {
  int i = 1;
  for (i = 1; i < control->iNumBodies; i++) {
    body[i].dMeanMotion = fdSemiToMeanMotion(body[i].dSemi,(body[0].dMass+body[i].dMass));
    
    if (control->iIntElements == HKPQ) {
      body[i].dEcc = sqrt(pow(body[i].dhecc,2)+pow(body[i].dkecc,2));
      body[i].dLongP = atan2(body[i].dhecc, body[i].dkecc);
      body[i].dsInc = sqrt(pow(body[i].dpinc,2)+pow(body[i].dqinc,2));
      body[i].dLongA = atan2(body[i].dpinc, body[i].dqinc);
      if (body[i].bObliqEvol == 1) {
	body[i].dObliquity = atan2(sqrt(pow(body[i].dbeta,2)+pow(body[i].dgamma,2)),body[i].deta);
	body[i].dPrecA = atan2(body[i].dbeta,body[i].dgamma);
	if (control->bUseShadow == 1) {
	  body[i].dShadowObl = asin(sqrt(pow(body[i].dShadowbeta,2)+pow(body[i].dShadowgamma,2)));
	  body[i].dShadowPrecA = atan2(body[i].dShadowbeta,body[i].dShadowgamma);
	}
      }
    }
    body[i].dInc = 2.*asin(body[i].dsInc);
    /* The next step is dangerous. Must pass dDt = 0 in WriteLog to prevent advancement of MeanL */
    /* Are there other places this must also be true? */
    body[i].dMeanL = fdCorrectDomain(body[i].dMeanL + body[i].dMeanMotion * dDt);   
    body[i].dMeanA = fdCorrectDomain(body[i].dMeanL - body[i].dLongP);
    body[i].dArgP = fdCorrectDomain(body[i].dLongP - body[i].dLongA);
    body[i].dPrecA = fdCorrectDomain(body[i].dPrecA);
    
  }
}



/************* Lagrange Functions ************/
/*
 * Element/coordinate conversions
 */

double fdVectorNormal(double *vector) {
  double vsq = 0.0;
  int i;
  
  for (i = 0; i < 3; i++) {
    vsq += vector[i]*vector[i];
  }
  return pow(vsq, 0.5);
}

double fvVectorCross(double *a, double *b, double *c) {
  c[0] = a[1]*b[2] - b[1]*a[2];
  c[1] = a[2]*b[0] - b[2]*a[0];
  c[2] = a[0]*b[1] - b[0]*a[1];
}

double fdXAngle1(double dLongA, double dArgP, double dInc) {
  return cos(dLongA)*cos(dArgP) - sin(dLongA)*sin(dArgP)*cos(dInc);
}

double fdXAngle2(double dLongA, double dArgP, double dInc) {
  return -cos(dLongA)*sin(dArgP) - sin(dLongA)*cos(dArgP)*cos(dInc);
}

double fdYAngle1(double dLongA, double dArgP, double dInc) {
  return sin(dLongA)*cos(dArgP) + cos(dLongA)*sin(dArgP)*cos(dInc);
}

double fdYAngle2(double dLongA, double dArgP, double dInc) {
  return -sin(dLongA)*sin(dArgP) + cos(dLongA)*cos(dArgP)*cos(dInc);
}

double fdZAngle1(double dArgP, double dInc) {
  return sin(dArgP)*sin(dInc);
}

double fdZAngle2(double dArgP, double dInc) {
  return cos(dArgP)*sin(dInc);
}

double fdxOrbPlane(double dSemi, double dEcc, double dEccA) {
  return dSemi * (cos(dEccA) - dEcc);
}

double fdyOrbPlane(double dSemi, double dEcc, double dEccA) {
  return dSemi * pow(1.-dEcc*dEcc,0.5) * sin(dEccA);
}

double fdvxOrbPlane(double dMassCentral, double dMass, double dSemi, double dxOrbPlane, double dyOrbPlane, double dEccA) {
  double mu, n;
  mu = KGAUSS*KGAUSS * (dMassCentral + dMass);
  n = pow(mu / pow(dSemi,3.), 0.5);
  return -dSemi * dSemi * n * sin(dEccA) / pow(dxOrbPlane*dxOrbPlane+dyOrbPlane*dyOrbPlane,0.5);
}

double fdvyOrbPlane(double dMassCentral, double dMass, double dSemi, double dxOrbPlane, double dyOrbPlane, double dEccA, double dEcc) {
  double mu, n;
  mu = KGAUSS*KGAUSS * (dMassCentral + dMass);
  n = pow(mu / pow(dSemi,3.), 0.5);
  return dSemi * dSemi * n * pow( (1.0-dEcc*dEcc) / (dxOrbPlane*dxOrbPlane+dyOrbPlane*dyOrbPlane), 0.5) * cos(dEccA);
}
 
double signof(double x) {
  if (x > 0) return 1.0;
  if (x < 0) return -1.0;
  return 0.0;
}
 
double fdEccAKeplersEq(double dMeanA, double dEcc) {
  double dEccA;
  double di_1, di_2, di_3 = 1.0, fi, fi_1, fi_2, fi_3;
  
  dEccA = dMeanA + signof(sin(dMeanA))*0.85*dEcc;
  while (di_3 > 1e-15) {
    fi = dEccA - dEcc*sin(dEccA) - dMeanA;
    fi_1 = 1.0 - dEcc*cos(dEccA);
    fi_2 = dEcc*sin(dEccA);
    fi_3 = dEcc*cos(dEccA);
    di_1 = -fi / fi_1;
    di_2 = -fi / (fi_1 + 0.5*di_1*fi_2);
    di_3 = -fi / (fi_1 + 0.5*di_2*fi_2 + 1./6.*di_2*di_2*fi_3);
    dEccA += di_3;
  }
  return dEccA;
}

double fdCalcAscNode(double *dAngMom) {
  double dLongA;  
  dLongA = fdCorrectDomain(atan2(dAngMom[0], -dAngMom[1]));
  return dLongA;
}
  
double fdTrue2EccA(double dTrueA, double dEcc) {
  double cosE, dEccA;
  
  while (dTrueA < 0.0) {
    dTrueA += 2*PI;
  }
  
  while (dTrueA >= 2*PI) {
    dTrueA -= 2*PI;
  }
  
  cosE = (cos(dTrueA) + dEcc) / (1.0 + dEcc*cos(dTrueA));
  if (dTrueA <= PI) {
    dEccA = acos(cosE);
  } else if (dTrueA > PI) {
    dEccA = 2*PI - acos(cosE);
  }
  return dEccA;
}

double fdMeanL2MeanA(double dMeanL, double dLongP) {
  double dMeanA;
  dMeanA = dMeanL - dLongP;
 
  while (dMeanA < 0.0) {
    dMeanA += 2*PI;
  }
  
  while (dMeanA >= 2*PI) {
    dMeanA -= 2*PI;
  }
  
  return dMeanA;
}
  
void fvOsc2Cart(BODY *body) {
  int iBody, iDim = 0;
  double xi, yi, vxi, vyi, **AngMtx;
  
  AngMtx = (double **) malloc(3*sizeof(double*));
  AngMtx[0] = malloc(3*sizeof(double));
  AngMtx[1] = malloc(3*sizeof(double));
  AngMtx[2] = malloc(3*sizeof(double));
  
  for (iBody = 1; iBody < body[0].iNumBodies; iBody++) {
    body[iBody].dEccA = fdEccAKeplersEq(body[iBody].dMeanA, body[iBody].dEcc);
    xi = fdxOrbPlane(body[iBody].dSemi/AUCM, body[iBody].dEcc, body[iBody].dEccA);
    yi = fdyOrbPlane(body[iBody].dSemi/AUCM, body[iBody].dEcc, body[iBody].dEccA);
    vxi = fdvxOrbPlane(body[0].dMass/MSUN, body[iBody].dMass/MSUN, body[iBody].dSemi/AUCM, xi, yi, body[iBody].dEccA);
    vyi = fdvyOrbPlane(body[0].dMass/MSUN, body[iBody].dMass/MSUN, body[iBody].dSemi/AUCM, xi, yi, body[iBody].dEccA, body[iBody].dEcc);
    
    AngMtx[0][0] = fdXAngle1(body[iBody].dLongA, body[iBody].dArgP, body[iBody].dInc);
    AngMtx[0][1] = fdXAngle2(body[iBody].dLongA, body[iBody].dArgP, body[iBody].dInc);
    AngMtx[1][0] = fdYAngle1(body[iBody].dLongA, body[iBody].dArgP, body[iBody].dInc);
    AngMtx[1][1] = fdYAngle2(body[iBody].dLongA, body[iBody].dArgP, body[iBody].dInc);
    AngMtx[2][0] = fdZAngle1(body[iBody].dArgP, body[iBody].dInc);
    AngMtx[2][1] = fdZAngle2(body[iBody].dArgP, body[iBody].dInc);
    
    for (iDim = 0; iDim < 3; iDim++) {
      body[iBody].dPosition[iDim] = xi * AngMtx[iDim][0] + yi * AngMtx[iDim][1];
      body[iBody].dVelocity[iDim] = vxi * AngMtx[iDim][0] + vyi * AngMtx[iDim][1];
    }
  }
}
  
void fvCart2Osc(BODY *body) {
  int iBody, j;
  double R, vsq, RxRdot, Rdot, mu, *h, hsq, a, sinwf, coswf, sinf, cosf, sinw, cosw, f;
  
  h = malloc(3*sizeof(double));
  for (iBody=1; iBody<body[0].iNumBodies; iBody++) {
    /* preliminary quantities */
    R = fdVectorNormal(body[iBody].dPosition);
    vsq = pow(fdVectorNormal(body[iBody].dVelocity),2);
    RxRdot = 0.0;
    for (j=0;j<3;j++) {
      RxRdot += body[iBody].dPosition[j]*body[iBody].dVelocity[j];
    }
    mu = KGAUSS*KGAUSS * (body[0].dMass + body[iBody].dMass)/MSUN;
    fvVectorCross(body[iBody].dPosition, body[iBody].dVelocity, h);
    hsq = fdVectorNormal(h)*fdVectorNormal(h);
    Rdot = signof(RxRdot) * pow(vsq - hsq/pow(R,2),0.5);
    
    /* elements */
    a = pow((2.0/R - vsq/mu),-1.0);
    body[iBody].dEcc = pow((1.0-hsq/(mu*a)),0.5);
    body[iBody].dInc = acos(h[2]/pow(hsq,0.5));
    body[iBody].dLongA = fdCalcAscNode(h);
    /* calc argument of pericenter */
    sinwf = body[iBody].dPosition[2] / (R*sin(body[iBody].dInc));
    coswf = (body[iBody].dPosition[0]/R + sin(body[iBody].dLongA)*sinwf*cos(body[iBody].dInc)) / \
      cos(body[iBody].dLongA);
    sinf = a * (1.0-pow(body[iBody].dEcc,2))/(pow(hsq,0.5)*body[iBody].dEcc) * Rdot;
    cosf = (a*(1.0-pow(body[iBody].dEcc,2))/R -1.0)/body[iBody].dEcc;
    sinw = sinwf * cosf - coswf * sinf;
    cosw = sinwf * sinf + coswf * cosf;
    body[iBody].dArgP = fdCorrectDomain(atan2(sinw, cosw));
    /* mean anomaly */
    f = fdCorrectDomain(atan2(sinf, cosf));
    body[iBody].dEccA = fdTrue2EccA(f, body[iBody].dEcc);
    body[iBody].dMeanA = fdCorrectDomain(body[iBody].dEccA - body[iBody].dEcc*sin(body[iBody].dEccA));
    /* finally, update dSemi, dsInc, dLongP, dMeanL */
    body[iBody].dSemi = a*AUCM;
    body[iBody].dsInc = sin(0.5*body[iBody].dInc);
    body[iBody].dLongP = fdCorrectDomain(body[iBody].dLongA + body[iBody].dArgP);
    body[iBody].dMeanL = fdCorrectDomain(body[iBody].dLongP + body[iBody].dMeanA);
  }
}

void fvRotateX(BODY *body, double theta) {
  double ytmp, ztmp, vytmp, vztmp;
  int iBody;
  
  fvOsc2Cart(body);
  
  for (iBody = 1; iBody < body[0].iNumBodies; iBody++) {
    ytmp = body[iBody].dPosition[1];
    ztmp = body[iBody].dPosition[2];
    vytmp = body[iBody].dVelocity[1];
    vztmp = body[iBody].dVelocity[2];
    body[iBody].dPosition[1] = ytmp*cos(theta) - ztmp*sin(theta);
    body[iBody].dPosition[2] = ytmp*sin(theta) + ztmp*cos(theta);
    body[iBody].dVelocity[1] = vytmp*cos(theta) - vztmp*sin(theta);
    body[iBody].dVelocity[2] = vytmp*sin(theta) + vztmp*cos(theta);
  }
  
  fvCart2Osc(body);
}

// 
/* 
 * Semi-major axis functions
 */

#define A(iIndexJ) dAxRatio,iIndexJ,0.5
#define B(iIndexJ) dAxRatio,iIndexJ,1.5
#define C(iIndexJ) dAxRatio,iIndexJ,2.5

// A -> s = 1/2
// B -> s = 3/2
// C -> s = 5/2

double fdLaplaceCoeff(double dAxRatio, int iIndexJ, double dIndexS) {
/* Calculates Laplace coefficients via series form (M&D eqn 6.68) taking dAxRatio = ratio of semi-major axes and j and s as arguments */
  double fac = 1.0, sum = 1.0, term = 1.0;
  int k, n = 1;
  if (iIndexJ == 1)
    fac = dIndexS * dAxRatio;
  else {
    for (k = 1; k <= iIndexJ; k++)
      fac *= (dIndexS + k - 1.0) / (k) * dAxRatio;
  }
 
  while (term >= 1.0e-15*sum) {
    term = 1.0;
    for (k = 1; k <= n; k++) {
      term *= (dIndexS + k - 1.0) * (dIndexS + iIndexJ + k - 1.0) / (k * (iIndexJ + k)) * dAxRatio*dAxRatio;
    }
    sum = sum + term;
    
    n++;
  }
  return 2.0*fac*sum;
}

double fdDerivLaplaceCoeff(int iNthDeriv, double dAxRatio, int iIndexJ, double dIndexS) {
/* Calculates nth order derivative of Laplace coefficient using a recursive scheme */
  double result;

  if (iNthDeriv == 1)
    result = dIndexS * (fdLaplaceCoeff(dAxRatio, abs(iIndexJ-1), dIndexS + 1.0) - 2 * dAxRatio * \
      fdLaplaceCoeff(dAxRatio, iIndexJ, dIndexS + 1.0) + fdLaplaceCoeff(dAxRatio, iIndexJ + 1, dIndexS + 1.0));
  else if (iNthDeriv == 2)
    result = dIndexS * (fdDerivLaplaceCoeff(1,dAxRatio,abs(iIndexJ-1),dIndexS+1.) - 2 * dAxRatio * \
      fdDerivLaplaceCoeff(1,dAxRatio,iIndexJ,dIndexS+1.)+fdDerivLaplaceCoeff(1,dAxRatio,iIndexJ+1,dIndexS+1.) -\
      2 * fdLaplaceCoeff(dAxRatio,iIndexJ,dIndexS+1.));
  else
    result = dIndexS * (fdDerivLaplaceCoeff(iNthDeriv-1,dAxRatio,abs(iIndexJ-1),dIndexS+1.) - 2 * dAxRatio * \
      fdDerivLaplaceCoeff(iNthDeriv-1,dAxRatio,iIndexJ,dIndexS+1.)+fdDerivLaplaceCoeff(iNthDeriv-1,dAxRatio,iIndexJ+1,dIndexS+1.) - 2 * (iNthDeriv-1) * fdDerivLaplaceCoeff(iNthDeriv-2,dAxRatio,iIndexJ,dIndexS+1.));
  return result;
}

double fdSemiMajAxF1(double dAxRatio, int iIndexJ) {
  return 1./2 * fdLaplaceCoeff(A(iIndexJ));
}

double fdSemiMajAxF2(double dAxRatio, int iIndexJ) {
  return 1./8* (-4.*iIndexJ*iIndexJ * fdLaplaceCoeff(A(iIndexJ)) + 2.*dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ))\
    + dAxRatio*dAxRatio * fdDerivLaplaceCoeff(2,A(iIndexJ)) );
}

double fdSemiMajAxF3(double dAxRatio, int iIndexJ) {
  return -1./4*dAxRatio * ( fdLaplaceCoeff(B(abs(iIndexJ-1))) + fdLaplaceCoeff(B(iIndexJ+1)) );
}

double fdSemiMajAxF4(double dAxRatio, int iIndexJ) {
  return 1./128 * ( (-9.*iIndexJ*iIndexJ + 16.*pow(iIndexJ,4)) * fdLaplaceCoeff(A(iIndexJ))  \
    -8.*iIndexJ*iIndexJ*dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ)) \
    - 8.*iIndexJ*iIndexJ*pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,A(iIndexJ)) \
    + 4.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ)) );
}

double fdSemiMajAxF5(double dAxRatio, int iIndexJ) {
  return 1./32 * ( 16.*pow(iIndexJ,4) * fdLaplaceCoeff(A(iIndexJ)) \
    + (4. - 16.*iIndexJ*iIndexJ) * dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ)) \
    + (14. - 8.*iIndexJ*iIndexJ) * pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,A(iIndexJ)) \
    + 8.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ)) );
}

double fdSemiMajAxF6(double dAxRatio, int iIndexJ) {
  return 1./128 * ( (-17.*iIndexJ*iIndexJ + 16.*pow(iIndexJ,4)) * fdLaplaceCoeff(A(iIndexJ)) \
    + (1. - iIndexJ*iIndexJ) * 24. * dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ)) \
    + (36. - 8.*iIndexJ*iIndexJ) * pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,A(iIndexJ)) \
    + 12.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ)) );
}

double fdSemiMajAxF7(double dAxRatio, int iIndexJ) {
  return 1./16*( (-2.+4.*iIndexJ*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(abs(iIndexJ-1)))+fdLaplaceCoeff(B(iIndexJ+1)))
    - 4.*pow(dAxRatio,2) * (fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) + fdDerivLaplaceCoeff(1,B(iIndexJ+1))) \
    - pow(dAxRatio,3) * (fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) + fdDerivLaplaceCoeff(2,B(iIndexJ+1))) );
}

double fdSemiMajAxF8(double dAxRatio, int iIndexJ) {
  return 3./16 * dAxRatio*dAxRatio * ( fdLaplaceCoeff(C(abs(iIndexJ-2))) \
    + 4. * fdLaplaceCoeff(C(iIndexJ)) + fdLaplaceCoeff(C(iIndexJ+2)) );
}

double fdSemiMajAxF9(double dAxRatio, int iIndexJ) {
  return 1./4 * dAxRatio * (fdLaplaceCoeff(B(abs(iIndexJ-1))) + fdLaplaceCoeff(B(iIndexJ+1))) \
    + 3./8 * pow(dAxRatio,2) * ( fdLaplaceCoeff(C(abs(iIndexJ-2))) + 10. * fdLaplaceCoeff(C(iIndexJ)) \
    + fdLaplaceCoeff(C(iIndexJ+2)) );
}

double fdSemiMajAxF10(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (2. + 6.*iIndexJ + 4.*iIndexJ*iIndexJ) * fdLaplaceCoeff(A(iIndexJ+1)) \
    - 2. * dAxRatio * fdDerivLaplaceCoeff(1,A(iIndexJ+1)) \
    - pow(dAxRatio,2) * fdDerivLaplaceCoeff(2,A(iIndexJ+1)) );
}

double fdSemiMajAxF11(double dAxRatio, int iIndexJ) {
  return 1./32*((-6.*iIndexJ-26.*iIndexJ*iIndexJ-36.*pow(iIndexJ,3) \
    -16*pow(iIndexJ,4))*fdLaplaceCoeff(A(iIndexJ+1))\
    + dAxRatio * (6*iIndexJ + 12*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(1,A(iIndexJ+1)) \
    + pow(dAxRatio,2) * (-4. + 7*iIndexJ + 8*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(2,A(iIndexJ+1)) \
    - 6.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ+1)) \
    - pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ+1)) );
}

double fdSemiMajAxF12(double dAxRatio, int iIndexJ) {
  return 1./32 * ( (4. + 2.*iIndexJ - 22.*iIndexJ*iIndexJ - 36.*pow(iIndexJ,3) - 16.*pow(iIndexJ,4)) * \
    fdLaplaceCoeff(A(iIndexJ+1)) \
    + dAxRatio * (-4. + 22.*iIndexJ + 20.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(1,A(iIndexJ+1)) \
    + pow(dAxRatio,2) * (-22. + 7.*iIndexJ + 8.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(2,A(iIndexJ+1)) \
    - 10.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ+1)) \
    - pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ+1)) );
}

double fdSemiMajAxF13(double dAxRatio, int iIndexJ) {
  return 1./8*((-6.*iIndexJ-4.*iIndexJ*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(iIndexJ))+fdLaplaceCoeff(B(iIndexJ+2)))\
    + 4.*pow(dAxRatio,2) * (fdDerivLaplaceCoeff(1,B(iIndexJ)) + fdDerivLaplaceCoeff(1,B(iIndexJ+2)))\
    + pow(dAxRatio,3) * (fdDerivLaplaceCoeff(2,B(iIndexJ)) + fdDerivLaplaceCoeff(2,B(iIndexJ+2))) );
}

double fdSemiMajAxF14(double dAxRatio, int iIndexJ) {
  return dAxRatio * fdLaplaceCoeff(B(iIndexJ+1));
}

double fdSemiMajAxF15(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (2. - 4.*iIndexJ*iIndexJ) * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) \
   + 4.*pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ+1)) \
   + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ+1)) );
}

double fdSemiMajAxF16(double dAxRatio, int iIndexJ) {
  return -1./2 * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) \
    -3.* pow(dAxRatio,2) * fdLaplaceCoeff(C(iIndexJ)) - 3./2 * pow(dAxRatio,2) * fdLaplaceCoeff(C(iIndexJ+2));
}

double fdSemiMajAxF17(double dAxRatio, int iIndexJ) {
  return 1./64 * ( (12. + 64.*iIndexJ + 109.*iIndexJ*iIndexJ + 72.*pow(iIndexJ,3) + 16.*pow(iIndexJ,4)) \
    * fdLaplaceCoeff(A(iIndexJ+2)) \
    - dAxRatio * (12. + 28.*iIndexJ + 16.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(1,A(iIndexJ+2)) \
    + pow(dAxRatio,2) * (6. - 14.*iIndexJ - 8.*iIndexJ*iIndexJ) * fdDerivLaplaceCoeff(2,A(iIndexJ+2)) \
    + 8.*pow(dAxRatio,3) * fdDerivLaplaceCoeff(3,A(iIndexJ+2)) \
    + pow(dAxRatio,4) * fdDerivLaplaceCoeff(4,A(iIndexJ+2)) );
}

double fdSemiMajAxF18(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (12. - 15.*iIndexJ + 4.*iIndexJ*iIndexJ) * dAxRatio * fdLaplaceCoeff(B(abs(iIndexJ-1))) \
    + pow(dAxRatio,2) * (8. - 4.*iIndexJ) * fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) \
    + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) );
}

double fdSemiMajAxF19(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (6. - 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ)) \
   + pow(dAxRatio,2) * (-4. + 4.*iIndexJ) * fdDerivLaplaceCoeff(1,B(iIndexJ)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ)) );
}

double fdSemiMajAxF20(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (3. + 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) \
   - 4.*iIndexJ * pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ+1)) \
   + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ+1)) );
}

double fdSemiMajAxF21(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (-12. + 15.*iIndexJ - 4.*iIndexJ*iIndexJ) * dAxRatio * fdLaplaceCoeff(B(abs(iIndexJ-1))) \
   + pow(dAxRatio,2) * (-8. + 4.*iIndexJ) * fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1))) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1))) );
}

double fdSemiMajAxF22(double dAxRatio, int iIndexJ) {
  return 1./4 * ( dAxRatio * iIndexJ * (6. + 4.*iIndexJ) * fdLaplaceCoeff(B(iIndexJ)) \
   - 4. * pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ)) );
}

double fdSemiMajAxF23(double dAxRatio, int iIndexJ) {
  return 1./4 * ( dAxRatio * iIndexJ * (6. + 4.*iIndexJ) * fdLaplaceCoeff(B(iIndexJ+2)) \
   - 4. * pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ+2)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ+2)) );
}

double fdSemiMajAxF24(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (-6. + 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ))	\
   + 4.*pow(dAxRatio,2) * (1. - iIndexJ) * fdDerivLaplaceCoeff(1,B(iIndexJ))    \
   + pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ)) );
}

double fdSemiMajAxF25(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (-3. - 4.*iIndexJ) * iIndexJ * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) \
   + 4.*iIndexJ*pow(dAxRatio,2) * fdDerivLaplaceCoeff(1,B(iIndexJ+1)) \
   - pow(dAxRatio,3) * fdDerivLaplaceCoeff(2,B(iIndexJ+1)) );
}

double fdSemiMajAxF26(double dAxRatio, int iIndexJ) {
  return 1./2 * dAxRatio * fdLaplaceCoeff(B(iIndexJ+1)) + 3./4 * pow(dAxRatio,2) * fdLaplaceCoeff(C(iIndexJ)) \
    + 3./2 * pow(dAxRatio,2) * fdLaplaceCoeff(C(iIndexJ+2));
}

double fdSemiMajAxF27(double dAxRatio, int iIndexJ) {
  return 1./2 * ( -2.*iIndexJ*fdLaplaceCoeff(A(iIndexJ)) - dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ)) );
}

double fdSemiMajAxF28(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (2.*iIndexJ-10.*pow(iIndexJ,2)+8.*pow(iIndexJ,3))*fdLaplaceCoeff(A(iIndexJ)) + (3.-7.*iIndexJ+4.*pow(iIndexJ,2))*dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ)) + (-2.-2.*iIndexJ)*pow(dAxRatio,2)*fdDerivLaplaceCoeff(2,A(iIndexJ)) - pow(dAxRatio,3)*fdDerivLaplaceCoeff(3,A(iIndexJ)) );
}

double fdSemiMajAxF29(double dAxRatio, int iIndexJ) {
  return 1./8 * ( 8.*pow(iIndexJ,3)*fdLaplaceCoeff(A(iIndexJ)) + (-2.-4.*iIndexJ+4.*pow(iIndexJ,2))*dAxRatio*fdDerivLaplaceCoeff(1,A(iIndexJ)) + (-4.-2.*iIndexJ)*pow(dAxRatio,2)*fdDerivLaplaceCoeff(2,A(iIndexJ)) - pow(dAxRatio,3)*fdDerivLaplaceCoeff(3,A(iIndexJ)) );
}

double fdSemiMajAxF30(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (1.+2.*iIndexJ)*dAxRatio*(fdLaplaceCoeff(B(abs(iIndexJ-1)))+fdLaplaceCoeff(B(iIndexJ+1))) + pow(dAxRatio,2)*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))+fdDerivLaplaceCoeff(1,B(iIndexJ+1))) );
}

double fdSemiMajAxF31(double dAxRatio, int iIndexJ) {
  return 1./2 * ( (-1.+2.*iIndexJ)*fdLaplaceCoeff(A(abs(iIndexJ-1))) + dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1))) );
}

double fdSemiMajAxF32(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (4.-16.*iIndexJ+20.*pow(iIndexJ,2)-8.*pow(iIndexJ,3))*fdLaplaceCoeff(A(abs(iIndexJ-1))) +     (-4.+12.*iIndexJ-4.*pow(iIndexJ,2))*dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1))) + (3.0+2.*iIndexJ)*pow(dAxRatio,2)*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1))) + pow(dAxRatio,3)*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-1))) );
}

double fdSemiMajAxF33(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (-2.-1.*iIndexJ+10.*pow(iIndexJ,2)-8.*pow(iIndexJ,3))*fdLaplaceCoeff(A(abs(iIndexJ-1))) +     (2.+9.*iIndexJ-4.*pow(iIndexJ,2))*dAxRatio*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1))) +                         (5.+2.*iIndexJ)*pow(dAxRatio,2)*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1))) + pow(dAxRatio,3)*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-1))) );
}

double fdSemiMajAxF34(double dAxRatio, int iIndexJ) {
  return 1./4 * ( -2.*iIndexJ*dAxRatio*(fdLaplaceCoeff(B(abs(iIndexJ-2)))+fdLaplaceCoeff(B(iIndexJ))) - pow(dAxRatio,2)*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-2)))+fdDerivLaplaceCoeff(1,B(iIndexJ))) );
}

/*------------- Derivs of Laplace Coeffs for dR/da -----------------------------------*/
double fdDerivSemiF27(double dAxRatio, int iIndexJ) {
  return 1./2 * ( (-1.-2.*iIndexJ)*fdDerivLaplaceCoeff(1,A(iIndexJ)) - dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ)) );
}

double fdDerivSemiF28(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (3.-5.*iIndexJ-6.*pow(iIndexJ,2)+8.*pow(iIndexJ,3))*fdDerivLaplaceCoeff(1,A(iIndexJ)) + (-1.-11.*iIndexJ+4.*pow(iIndexJ,2))*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ)) + (-5.-2.*iIndexJ)*pow(dAxRatio,2)*fdDerivLaplaceCoeff(3,A(iIndexJ)) - pow(dAxRatio,3)*fdDerivLaplaceCoeff(4,A(iIndexJ)) );
}

double fdDerivSemiF29(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (-2.-4.*iIndexJ+4.*pow(iIndexJ,2)+8.*pow(iIndexJ,3))*fdDerivLaplaceCoeff(1,A(iIndexJ)) + (-10.-8.*iIndexJ+4.*pow(iIndexJ,2))*dAxRatio*fdDerivLaplaceCoeff(2,A(iIndexJ)) + (-7.-2.*iIndexJ)*pow(dAxRatio,2)*fdDerivLaplaceCoeff(3,A(iIndexJ)) - pow(dAxRatio,3)*fdDerivLaplaceCoeff(4,A(iIndexJ)) );
}

double fdDerivSemiF30(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (1.+2.*iIndexJ)*(fdLaplaceCoeff(B(abs(iIndexJ-1)))+fdLaplaceCoeff(B(iIndexJ+1))) +            (3.+2.*iIndexJ)*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-1)))+fdDerivLaplaceCoeff(1,B(iIndexJ+1))) + pow(dAxRatio,2)*(fdDerivLaplaceCoeff(2,B(abs(iIndexJ-1)))+fdDerivLaplaceCoeff(2,B(iIndexJ+1))) );
}

double fdDerivSemiF31(double dAxRatio, int iIndexJ) {
  return 1./2 * ( 2.*iIndexJ*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1))) + dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1))) );
}

double fdDerivSemiF32(double dAxRatio, int iIndexJ) {
  return 1./8 * ( (-4.*iIndexJ + 16.*pow(iIndexJ,2)-8.*pow(iIndexJ,3))*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1))) + (2.+16.*iIndexJ-4.*pow(iIndexJ,2))*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1))) + (6.+2.*iIndexJ)*pow(dAxRatio,2)*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-1))) + pow(dAxRatio,3)*fdDerivLaplaceCoeff(4,A(abs(iIndexJ-1))) );
}

double fdDerivSemiF33(double dAxRatio, int iIndexJ) {
  return 1./16 * ( (8.*iIndexJ+6.*pow(iIndexJ,2)-8.*pow(iIndexJ,3))*fdDerivLaplaceCoeff(1,A(abs(iIndexJ-1))) +  (12.+13.*iIndexJ-4.*pow(iIndexJ,2))*dAxRatio*fdDerivLaplaceCoeff(2,A(abs(iIndexJ-1))) + (8.+2.*iIndexJ)*pow(dAxRatio,2)*fdDerivLaplaceCoeff(3,A(abs(iIndexJ-1))) + pow(dAxRatio,3)*fdDerivLaplaceCoeff(4,A(abs(iIndexJ-1))) );
}  

double fdDerivSemiF34(double dAxRatio, int iIndexJ) {
  return 1./4 * ( (-2.*iIndexJ)*(fdLaplaceCoeff(B(abs(iIndexJ-2)))+fdLaplaceCoeff(B(iIndexJ))) +               (-2.-2.*iIndexJ)*dAxRatio*(fdDerivLaplaceCoeff(1,B(abs(iIndexJ-2)))+fdDerivLaplaceCoeff(1,B(iIndexJ))) - pow(dAxRatio,2)*(fdDerivLaplaceCoeff(2,B(abs(iIndexJ-2)))+fdDerivLaplaceCoeff(2,B(iIndexJ))) );
}
  
//------dR / dw -----------------------------------------------------------------//
// For unprimed disturbing fxn, iBody = unprimed, jBody = primed 
double fdDdistDvarDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = ( body[jBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][9] + pow(body[iBody].dEcc,2)*body[jBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][10] + pow(body[jBody].dEcc,3) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][11] + body[jBody].dEcc*(body[iBody].dsInc*body[iBody].dsInc + body[jBody].dsInc*body[jBody].dsInc) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][12] )* sin( (body[jBody].dLongP - body[iBody].dLongP) );
  return y;
}

double fdDdistDvarDir04(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = body[iBody].dEcc*pow(body[jBody].dEcc,2) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][16] * 2 * sin( ( 2.*(body[jBody].dLongP - body[iBody].dLongP)) );
  return y;
}

double fdDdistDvarDir05(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = body[iBody].dEcc*pow(body[iBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][17] * (-2) * sin( (2.*(body[iBody].dLongP - body[iBody].dLongA)));
  return y;
}

double fdDdistDvarDir06(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = -body[jBody].dEcc*body[iBody].dsInc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][18] * sin( (body[jBody].dLongP + body[iBody].dLongP - 2.*body[iBody].dLongA) );
  return y;
}

double fdDdistDvarDir08(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = body[iBody].dEcc*body[iBody].dsInc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][20]  * (-2) * sin( ( 2.*body[iBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
  return y;
}

double fdDdistDvarDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y; 
  y = body[jBody].dEcc*body[iBody].dsInc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][21]  * sin( ( body[jBody].dLongP - body[iBody].dLongP - body[jBody].dLongA + body[iBody].dLongA) );
  return y;
}

double fdDdistDvarDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = body[jBody].dEcc*body[iBody].dsInc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][22] * sin( ( body[jBody].dLongP - body[iBody].dLongP + body[jBody].dLongA - body[iBody].dLongA) );
  return y;
}

double fdDdistDvarDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = -body[jBody].dEcc*body[iBody].dsInc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][23] * sin( ( body[jBody].dLongP + body[iBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
  return y;
}

double fdDdistDvarDir013(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;  
  y = body[iBody].dEcc*pow(body[jBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][17] * (-2) * sin( ( 2.*(body[iBody].dLongP - body[jBody].dLongA)) );
  return y;
}

double fdDdistDvarDir014(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = -body[jBody].dEcc*body[jBody].dsInc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][18] * sin( ( body[jBody].dLongP + body[iBody].dLongP - 2.*body[jBody].dLongA));
  return y;
}

double fdDdisturbDvarpi(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  y = (fdDdistDvarDir02(body, grids, iBody, jBody)\
    + fdDdistDvarDir04(body, grids, iBody, jBody) \
    + fdDdistDvarDir05(body, grids, iBody, jBody) \
    + fdDdistDvarDir06(body, grids, iBody, jBody) \
    + fdDdistDvarDir08(body, grids, iBody, jBody) \
    + fdDdistDvarDir09(body, grids, iBody, jBody) \
    + fdDdistDvarDir010(body, grids, iBody, jBody) \
    + fdDdistDvarDir011(body, grids, iBody, jBody) \
    + fdDdistDvarDir013(body, grids, iBody, jBody) \
    + fdDdistDvarDir014(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//-----dR'/dw'----------------------------------------------------------------////
// For primed disturbing fxn, iBody = primed, jBody = unprimed 
double fdDdistDvarPrmDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return -( body[jBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][9] + pow(body[jBody].dEcc,3) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][10] + body[jBody].dEcc*pow(body[iBody].dEcc,2) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][11] + body[jBody].dEcc*(body[jBody].dsInc*body[jBody].dsInc + body[iBody].dsInc*body[iBody].dsInc) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][12] ) * sin( ( body[iBody].dLongP - body[jBody].dLongP) );
}

double fdDdistDvarPrmDir04(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return pow(body[jBody].dEcc,2)*body[iBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][16] * (-2) * sin( ( 2.*(body[iBody].dLongP - body[jBody].dLongP)) );
}

double fdDdistDvarPrmDir06(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return -body[jBody].dEcc*body[jBody].dsInc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][18] * sin( ( body[iBody].dLongP + body[jBody].dLongP - 2.*body[jBody].dLongA) );
}

double fdDdistDvarPrmDir07(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*pow(body[jBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][19]  * (-2) * sin( ( 2.*(body[iBody].dLongP - body[jBody].dLongA)) );
}

double fdDdistDvarPrmDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return -body[jBody].dEcc*body[jBody].dsInc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][21] * sin( ( body[iBody].dLongP - body[jBody].dLongP - body[iBody].dLongA + body[jBody].dLongA) );
}

double fdDdistDvarPrmDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return -body[jBody].dEcc*body[jBody].dsInc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][22] * sin( ( body[iBody].dLongP - body[jBody].dLongP + body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDvarPrmDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return -body[jBody].dEcc*body[jBody].dsInc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][23] * sin( ( body[iBody].dLongP + body[jBody].dLongP - body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDvarPrmDir012(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*body[jBody].dsInc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][24] * (-2) * sin( ( 2.*body[iBody].dLongP - body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDvarPrmDir014(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return -body[jBody].dEcc*body[iBody].dsInc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][18] * sin( ( body[iBody].dLongP + body[jBody].dLongP - 2.*body[iBody].dLongA) );
}

double fdDdistDvarPrmDir015(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*pow(body[iBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][19] * (-2) * sin( ( 2.*(body[iBody].dLongP - body[iBody].dLongA)) );
}

double fdDdisturbDvarpiPrime(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  y = ( fdDdistDvarPrmDir02(body, grids, iBody, jBody)\
    + fdDdistDvarPrmDir04(body, grids, iBody, jBody)\
    + fdDdistDvarPrmDir06(body, grids, iBody, jBody) \
    + fdDdistDvarPrmDir07(body, grids, iBody, jBody) \
    + fdDdistDvarPrmDir09(body, grids, iBody, jBody) \
    + fdDdistDvarPrmDir010(body, grids, iBody, jBody) \
    + fdDdistDvarPrmDir011(body, grids, iBody, jBody) \
    + fdDdistDvarPrmDir012(body, grids, iBody, jBody) \
    + fdDdistDvarPrmDir014(body, grids, iBody, jBody) \
    + fdDdistDvarPrmDir015(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//-------------dR/dOmega--------------------------------------------------------//
// For unprimed disturbing fxn, iBody = unprimed, jBody = primed 
double fdDdistDOmgDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dsInc * (grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][13] + (body[iBody].dEcc*body[iBody].dEcc + body[jBody].dEcc*body[jBody].dEcc) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][14] + (body[iBody].dsInc*body[iBody].dsInc + body[jBody].dsInc*body[jBody].dsInc) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][15]) ) * sin( ( body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDOmgDir05(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return pow(body[iBody].dEcc,2)*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][17] * 2 * sin( ( 2.*(body[iBody].dLongP - body[iBody].dLongA)) );
}

double fdDdistDOmgDir06(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*body[jBody].dEcc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][18] * 2 * sin( ( body[jBody].dLongP + body[iBody].dLongP - 2.*body[iBody].dLongA) );
}

double fdDdistDOmgDir07(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return pow(body[jBody].dEcc,2)*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][19] * 2 * sin( ( 2.*(body[jBody].dLongP - body[iBody].dLongA)) );
}

double fdDdistDOmgDir08(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*body[iBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][20] * sin( ( 2.*body[iBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDOmgDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return -body[iBody].dEcc*body[jBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][21] * sin( ( body[jBody].dLongP - body[iBody].dLongP - body[jBody].dLongA + body[iBody].dLongA));
}

double fdDdistDOmgDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*body[jBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][22] * sin( ( body[jBody].dLongP - body[iBody].dLongP + body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDOmgDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*body[jBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][23] * sin( ( body[jBody].dLongP + body[iBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDOmgDir012(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[jBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][24] * sin( ( 2.*body[jBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDOmgDir016(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dsInc*pow(body[jBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][25] * 2 * sin( ( 2.*(body[jBody].dLongA - body[iBody].dLongA)) );
}

double fdDdisturbDOmega(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  return ( fdDdistDOmgDir03(body, grids, iBody, jBody) \
    + fdDdistDOmgDir05(body, grids, iBody, jBody) \
    + fdDdistDOmgDir06(body, grids, iBody, jBody) \
    + fdDdistDOmgDir07(body, grids, iBody, jBody) \
    + fdDdistDOmgDir08(body, grids, iBody, jBody) \
    + fdDdistDOmgDir09(body, grids, iBody, jBody) \
    + fdDdistDOmgDir010(body, grids, iBody, jBody) \
    + fdDdistDOmgDir011(body, grids, iBody, jBody) \
    + fdDdistDOmgDir012(body, grids, iBody, jBody) \
    + fdDdistDOmgDir016(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
}

//------dR'/dOm'-----------------------------------------------------------//
// For primed disturbing fxn, iBody = primed, jBody = unprimed 

double fdDdistDOmgPrmDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return -( body[jBody].dsInc * (grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][13] + (body[jBody].dEcc*body[jBody].dEcc + body[iBody].dEcc*body[iBody].dEcc) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][14] + (body[jBody].dsInc*body[jBody].dsInc + body[iBody].dsInc*body[iBody].dsInc) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][15]) ) * sin( ( body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDOmgPrmDir08(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[jBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][20] * sin( ( 2.*body[jBody].dLongP - body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDOmgPrmDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[iBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][21] * sin( ( body[iBody].dLongP - body[jBody].dLongP - body[iBody].dLongA + body[jBody].dLongA) );
}

double fdDdistDOmgPrmDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return -body[jBody].dEcc*body[iBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][22] * sin( ( body[iBody].dLongP - body[jBody].dLongP + body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDOmgPrmDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[iBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][23] * sin( ( body[iBody].dLongP + body[jBody].dLongP - body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDOmgPrmDir012(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*body[iBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][24] * sin( ( 2.*body[iBody].dLongP - body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDOmgPrmDir013(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return pow(body[jBody].dEcc,2)*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][17] * 2 * sin( ( 2.*(body[jBody].dLongP - body[iBody].dLongA)) );
}

double fdDdistDOmgPrmDir014(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[iBody].dEcc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][18] * 2 * sin( ( body[iBody].dLongP + body[jBody].dLongP - 2.*body[iBody].dLongA) );
}

double fdDdistDOmgPrmDir015(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return pow(body[iBody].dEcc,2)*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][19] * 2 * sin( ( 2.*(body[iBody].dLongP - body[iBody].dLongA)) );
}

double fdDdistDOmgPrmDir016(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return pow(body[jBody].dsInc,2)*body[iBody].dsInc* grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][25]* (-2) * sin( ( 2.*(body[iBody].dLongA - body[jBody].dLongA)));
}

double fdDdisturbDOmegaPrime(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  y = ( fdDdistDOmgPrmDir03(body, grids, iBody, jBody) \
    + fdDdistDOmgPrmDir08(body, grids, iBody, jBody) \
    + fdDdistDOmgPrmDir09(body, grids, iBody, jBody) \
    + fdDdistDOmgPrmDir010(body, grids, iBody, jBody) \
    + fdDdistDOmgPrmDir011(body, grids, iBody, jBody) \
    + fdDdistDOmgPrmDir012(body, grids, iBody, jBody) \
    + fdDdistDOmgPrmDir013(body, grids, iBody, jBody) \
    + fdDdistDOmgPrmDir014(body, grids, iBody, jBody) \
    + fdDdistDOmgPrmDir015(body, grids, iBody, jBody) \
    + fdDdistDOmgPrmDir016(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//------dR/de---------------------------------------------------------///

double fdDdistDeccDir01(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = ( 2*body[iBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][1] + 4*pow(body[iBody].dEcc,3) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][3] + 2*body[iBody].dEcc*body[jBody].dEcc*body[jBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][4] + (2*body[iBody].dEcc*pow(body[iBody].dsInc,2) + 2*body[iBody].dEcc*pow(body[jBody].dsInc,2)) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][6]);
  return y;
}

double fdDdistDeccDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = ( body[jBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][9] + 3*pow(body[iBody].dEcc,2)*body[jBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][10] + pow(body[jBody].dEcc,3) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][11] + body[jBody].dEcc*(body[iBody].dsInc*body[iBody].dsInc + body[jBody].dsInc*body[jBody].dsInc) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][12] ) * cos( ( body[jBody].dLongP - body[iBody].dLongP) );
  return y;
}

double fdDdistDeccDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = ( 2*body[iBody].dsInc*body[jBody].dsInc*body[iBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][14] ) * cos( ( body[jBody].dLongA - body[iBody].dLongA) );
  return y;
}

double fdDdistDeccDir04(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = 2*body[iBody].dEcc*pow(body[jBody].dEcc,2) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][16] * cos( ( 2.*(body[jBody].dLongP - body[iBody].dLongP)) );
  return y;
}

double fdDdistDeccDir05(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = 2*body[iBody].dEcc*pow(body[iBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][17] * cos( ( 2.*(body[iBody].dLongP - body[iBody].dLongA)) );
  return y;
}

double fdDdistDeccDir06(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = body[jBody].dEcc*body[iBody].dsInc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][18] * cos( ( body[jBody].dLongP + body[iBody].dLongP - 2.*body[iBody].dLongA) );
  return y;
}

double fdDdistDeccDir08(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = 2*body[iBody].dEcc*body[iBody].dsInc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][20] * cos( ( 2.*body[iBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
  return y;
}

double fdDdistDeccDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = body[jBody].dEcc*body[iBody].dsInc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][21] * cos( ( body[jBody].dLongP - body[iBody].dLongP - body[jBody].dLongA + body[iBody].dLongA));
  return y;
}

double fdDdistDeccDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = body[jBody].dEcc*body[iBody].dsInc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][22] * cos( ( body[jBody].dLongP - body[iBody].dLongP + body[jBody].dLongA - body[iBody].dLongA) );
  return y;
}

double fdDdistDeccDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = body[jBody].dEcc*body[iBody].dsInc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][23] * cos( ( body[jBody].dLongP + body[iBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
  return y;
}

double fdDdistDeccDir013(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = 2*body[iBody].dEcc*pow(body[jBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][17] * cos( ( 2.*(body[iBody].dLongP - body[jBody].dLongA)) );
  return y;
}

double fdDdistDeccDir014(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y;
  y = body[jBody].dEcc*body[jBody].dsInc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][18] * cos( ( body[jBody].dLongP + body[iBody].dLongP - 2.*body[jBody].dLongA) );
  return y;
}

double fdDdisturbDecc(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  y = ( fdDdistDeccDir01(body, grids, iBody, jBody) \
    + fdDdistDeccDir02(body, grids, iBody, jBody) \
    + fdDdistDeccDir03(body, grids, iBody, jBody) \
    + fdDdistDeccDir04(body, grids, iBody, jBody) \
    + fdDdistDeccDir05(body, grids, iBody, jBody) \
    + fdDdistDeccDir06(body, grids, iBody, jBody) \
    + fdDdistDeccDir08(body, grids, iBody, jBody) \
    + fdDdistDeccDir09(body, grids, iBody, jBody) \
    + fdDdistDeccDir010(body, grids, iBody, jBody) \
    + fdDdistDeccDir011(body, grids, iBody, jBody) \
    + fdDdistDeccDir013(body, grids, iBody, jBody) \
    + fdDdistDeccDir014(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//----dR'/de'-------------------------------------------------------------------//

double fdDdistDeccPrmDir01(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( 2*body[iBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][1] + 2*body[jBody].dEcc*body[jBody].dEcc*body[iBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][4] + 4*pow(body[iBody].dEcc,3) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][5] + (2*body[iBody].dEcc*pow(body[jBody].dsInc,2) + 2*body[iBody].dEcc*pow(body[iBody].dsInc,2)) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][6] );
}


double fdDdistDeccPrmDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][9] + pow(body[jBody].dEcc,3)* grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][10] + 3*body[jBody].dEcc*pow(body[iBody].dEcc,2) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][11] + body[jBody].dEcc*(body[jBody].dsInc*body[jBody].dsInc + body[iBody].dsInc*body[iBody].dsInc) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][12] ) * cos( ( body[iBody].dLongP - body[jBody].dLongP)  );
}

double fdDdistDeccPrmDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( 2*body[jBody].dsInc*body[iBody].dsInc*body[iBody].dEcc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][14] ) * cos( ( body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDeccPrmDir04(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dEcc*pow(body[jBody].dEcc,2) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][16] * cos( ( 2.*(body[iBody].dLongP - body[jBody].dLongP)) );
}

double fdDdistDeccPrmDir06(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[jBody].dsInc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][18] * cos( ( body[iBody].dLongP + body[jBody].dLongP - 2.*body[jBody].dLongA) );
}

double fdDdistDeccPrmDir07(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dEcc*pow(body[jBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][19] * cos( ( 2.*(body[iBody].dLongP - body[jBody].dLongA)) );
}

double fdDdistDeccPrmDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[jBody].dsInc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][21] * cos( ( body[iBody].dLongP - body[jBody].dLongP - body[iBody].dLongA + body[jBody].dLongA) );
}

double fdDdistDeccPrmDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[jBody].dsInc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][22] * cos( ( body[iBody].dLongP - body[jBody].dLongP + body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDeccPrmDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[jBody].dsInc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][23] * cos( ( body[iBody].dLongP + body[jBody].dLongP - body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDeccPrmDir012(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dEcc*body[jBody].dsInc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][24] * cos( ( 2.*body[iBody].dLongP - body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDeccPrmDir014(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[iBody].dsInc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][18] * cos( ( body[iBody].dLongP + body[jBody].dLongP - 2.*body[iBody].dLongA) );
}

double fdDdistDeccPrmDir015(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dEcc*pow(body[iBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][19] * cos( ( 2.*(body[iBody].dLongP - body[iBody].dLongA)) );
}

double fdDdisturbDeccPrime(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  y = ( fdDdistDeccPrmDir01(body, grids, iBody, jBody)  \
    + fdDdistDeccPrmDir02(body, grids, iBody, jBody) \
    + fdDdistDeccPrmDir03(body, grids, iBody, jBody) \
    + fdDdistDeccPrmDir04(body, grids, iBody, jBody) \
    + fdDdistDeccPrmDir06(body, grids, iBody, jBody) \
    + fdDdistDeccPrmDir07(body, grids, iBody, jBody) \
    + fdDdistDeccPrmDir09(body, grids, iBody, jBody) \
    + fdDdistDeccPrmDir010(body, grids, iBody, jBody) \
    + fdDdistDeccPrmDir011(body, grids, iBody, jBody) \
    + fdDdistDeccPrmDir012(body, grids, iBody, jBody) \
    + fdDdistDeccPrmDir014(body, grids, iBody, jBody) \
    + fdDdistDeccPrmDir015(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//-----dR/ds-------------------------------------------------------------------//

double fdDdistDsIncDir01(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( 2*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][2] + (2*body[iBody].dsInc*pow(body[iBody].dEcc,2) + 2*body[iBody].dsInc*pow(body[jBody].dEcc,2)) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][6] + 4*(pow(body[iBody].dsInc,3)) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][7] + 2*body[iBody].dsInc*pow(body[jBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][8] );
}

double fdDdistDsIncDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( 2*body[iBody].dEcc*body[jBody].dEcc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][12] ) * cos( ( body[jBody].dLongP - body[iBody].dLongP) );
}

double fdDdistDsIncDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][13] + body[jBody].dsInc*(body[iBody].dEcc*body[iBody].dEcc + body[jBody].dEcc*body[jBody].dEcc) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][14] + (3*body[iBody].dsInc*body[iBody].dsInc*body[jBody].dsInc + pow(body[jBody].dsInc,3)) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][15] ) * cos( ( body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDsIncDir05(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dsInc*pow(body[iBody].dEcc,2) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][17] * cos( ( 2.*(body[iBody].dLongP - body[iBody].dLongA)) );
}

double fdDdistDsIncDir06(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dEcc*body[jBody].dEcc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][18] * cos( ( body[jBody].dLongP + body[iBody].dLongP - 2.*body[iBody].dLongA) );
}

double fdDdistDsIncDir07(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dsInc*pow(body[jBody].dEcc,2) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][19] * cos( ( 2.*(body[jBody].dLongP - body[iBody].dLongA)) );
}

double fdDdistDsIncDir08(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*body[iBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][20] * cos( ( 2.*body[iBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDsIncDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*body[jBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][21] * cos( ( body[jBody].dLongP - body[iBody].dLongP - body[jBody].dLongA + body[iBody].dLongA));
}

double fdDdistDsIncDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*body[jBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][22] * cos( ( body[jBody].dLongP - body[iBody].dLongP + body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDsIncDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*body[jBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][23] * cos( ( body[jBody].dLongP + body[iBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDsIncDir012(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[jBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][24] * cos( ( 2.*body[jBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDsIncDir016(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dsInc*pow(body[jBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][25] * cos( ( 2.*(body[jBody].dLongA - body[iBody].dLongA)) );
}

double fdDdisturbDsInc(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  y = ( fdDdistDsIncDir01(body, grids, iBody, jBody)  \
    + fdDdistDsIncDir02(body, grids, iBody, jBody) \
    + fdDdistDsIncDir03(body, grids, iBody, jBody) \
    + fdDdistDsIncDir05(body, grids, iBody, jBody) \
    + fdDdistDsIncDir06(body, grids, iBody, jBody) \
    + fdDdistDsIncDir07(body, grids, iBody, jBody) \
    + fdDdistDsIncDir08(body, grids, iBody, jBody) \
    + fdDdistDsIncDir09(body, grids, iBody, jBody) \
    + fdDdistDsIncDir010(body, grids, iBody, jBody) \
    + fdDdistDsIncDir011(body, grids, iBody, jBody) \
    + fdDdistDsIncDir012(body, grids, iBody, jBody) \
    + fdDdistDsIncDir016(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//------dR'/ds'-----------------------------------------------------------------------//

double fdDdistDsIncPrmDir01(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( 2*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][2] + (2*body[iBody].dsInc*pow(body[jBody].dEcc,2) + 2*body[iBody].dsInc*pow(body[iBody].dEcc,2)) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][6] + 4*(pow(body[iBody].dsInc,3)) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][7] + 2*body[iBody].dsInc*pow(body[jBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][8] );
}

double fdDdistDsIncPrmDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( 2*body[jBody].dEcc*body[iBody].dEcc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][12] ) * cos( ( body[iBody].dLongP - body[jBody].dLongP) );
}

double fdDdistDsIncPrmDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][13] + body[jBody].dsInc*(body[jBody].dEcc*body[jBody].dEcc + body[iBody].dEcc*body[iBody].dEcc) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][14] + (3*body[jBody].dsInc*body[iBody].dsInc*body[iBody].dsInc + pow(body[jBody].dsInc,3)) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][15] ) * cos( ( body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDsIncPrmDir013(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*pow(body[jBody].dEcc,2)*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][17] * cos( ( 2.*(body[jBody].dLongP - body[iBody].dLongA)) );
}

double fdDdistDsIncPrmDir014(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[jBody].dEcc*body[iBody].dEcc*body[iBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][18] * cos( ( body[iBody].dLongP + body[jBody].dLongP - 2.*body[iBody].dLongA) );
}

double fdDdistDsIncPrmDir015(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dsInc*pow(body[iBody].dEcc,2) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][19] * cos( ( 2.*(body[iBody].dLongP - body[iBody].dLongA)) );
}

double fdDdistDsIncPrmDir08(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[jBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][20] * cos( ( 2.*body[jBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDsIncPrmDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[iBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][21] * cos( ( body[iBody].dLongP - body[jBody].dLongP - body[iBody].dLongA + body[jBody].dLongA));
}

double fdDdistDsIncPrmDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[iBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][22] * cos( ( body[iBody].dLongP - body[jBody].dLongP + body[iBody].dLongA - body[jBody].dLongA) );
}

double fdDdistDsIncPrmDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dEcc*body[iBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][23] * cos( ( body[iBody].dLongP + body[jBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDsIncPrmDir012(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dEcc*body[iBody].dEcc*body[jBody].dsInc * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][24] * cos( ( 2.*body[iBody].dLongP - body[jBody].dLongA - body[iBody].dLongA) );
}

double fdDdistDsIncPrmDir016(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dsInc*pow(body[jBody].dsInc,2) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][25] * cos( ( 2.*(body[iBody].dLongA - body[jBody].dLongA)) );
}

double fdDdisturbDsIncPrime(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  y = ( fdDdistDsIncPrmDir01(body, grids, iBody, jBody)   \
    + fdDdistDsIncPrmDir02(body, grids, iBody, jBody) \
    + fdDdistDsIncPrmDir03(body, grids, iBody, jBody) \
    + fdDdistDsIncPrmDir013(body, grids, iBody, jBody) \
    + fdDdistDsIncPrmDir014(body, grids, iBody, jBody) \
    + fdDdistDsIncPrmDir015(body, grids, iBody, jBody) \
    + fdDdistDsIncPrmDir08(body, grids, iBody, jBody) \
    + fdDdistDsIncPrmDir09(body, grids, iBody, jBody) \
    + fdDdistDsIncPrmDir010(body, grids, iBody, jBody) \
    + fdDdistDsIncPrmDir011(body, grids, iBody, jBody) \
    + fdDdistDsIncPrmDir012(body, grids, iBody, jBody) \
    + fdDdistDsIncPrmDir016(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//----------------Disturbing function h k p q----------------------------------------------
//--------dR/dh-----------(inner body)----------------------------------------------------------------
double fdDdistDhDir01(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dhecc*( grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][1] + 2*(pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][3] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][4] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][6] );
}

double fdDdistDhDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dhecc*( grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][9] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][11] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][12] ) + (2*body[iBody].dhecc*body[iBody].dkecc*body[jBody].dkecc + 3*pow(body[iBody].dhecc,2)*body[jBody].dhecc+body[jBody].dhecc*pow(body[iBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][10];
} 
 
double fdDdistDhDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dhecc*(body[iBody].dpinc*body[jBody].dpinc+body[iBody].dqinc*body[jBody].dqinc)*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][14];
}

double fdDdistDhDir04(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return (2*body[iBody].dhecc*(pow(body[jBody].dhecc,2)-pow(body[jBody].dkecc,2))+4*body[jBody].dhecc*body[iBody].dkecc*body[jBody].dkecc)*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][16];
}

double fdDdistDhDir05(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return (2*body[iBody].dhecc*(pow(body[iBody].dpinc,2)-pow(body[iBody].dqinc,2))+4*body[iBody].dpinc*body[iBody].dqinc*body[iBody].dkecc)*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDhDir06(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return (body[jBody].dhecc*(pow(body[iBody].dpinc,2)-pow(body[iBody].dqinc,2))+2*body[jBody].dkecc*body[iBody].dpinc*body[iBody].dqinc)*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][18];
}

double fdDdistDhDir08(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*(body[iBody].dhecc*(body[iBody].dpinc*body[jBody].dpinc-body[iBody].dqinc*body[jBody].dqinc) + body[iBody].dkecc*(body[jBody].dpinc*body[iBody].dqinc+body[jBody].dqinc*body[iBody].dpinc))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][20];
}

double fdDdistDhDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dhecc*(body[iBody].dqinc*body[jBody].dqinc+body[iBody].dpinc*body[jBody].dpinc) + body[jBody].dkecc*(body[iBody].dpinc*body[jBody].dqinc-body[iBody].dqinc*body[jBody].dpinc) )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][21];
}

double fdDdistDhDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dhecc*(body[iBody].dqinc*body[jBody].dqinc+body[iBody].dpinc*body[jBody].dpinc) + body[jBody].dkecc*(body[iBody].dqinc*body[jBody].dpinc-body[iBody].dpinc*body[jBody].dqinc) )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][22];
}

double fdDdistDhDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dhecc*(body[iBody].dpinc*body[jBody].dpinc-body[iBody].dqinc*body[jBody].dqinc) + body[jBody].dkecc*(body[iBody].dqinc*body[jBody].dpinc+body[iBody].dpinc*body[jBody].dqinc) )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][23];
}
  
double fdDdistDhDir013(BODY *body, GRIDS *grids, int iBody, int jBody) {  
  return ( 2*body[iBody].dhecc*(pow(body[jBody].dpinc,2)-pow(body[jBody].dqinc,2)) + 4*body[jBody].dpinc*body[jBody].dqinc*body[iBody].dkecc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][17];
}
  
double fdDdistDhDir014(BODY *body, GRIDS *grids, int iBody, int jBody) { 
  return ( body[jBody].dhecc*(pow(body[jBody].dpinc,2)-pow(body[jBody].dqinc,2)) + 2*body[jBody].dkecc*body[jBody].dpinc*body[jBody].dqinc)*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][18];
}
  
double fdDdisturbDhecc(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  y = ( fdDdistDhDir01(body, grids, iBody, jBody) \
    + fdDdistDhDir02(body, grids, iBody, jBody) \
    + fdDdistDhDir03(body, grids, iBody, jBody) \
    + fdDdistDhDir04(body, grids, iBody, jBody) \
    + fdDdistDhDir05(body, grids, iBody, jBody) \
    + fdDdistDhDir06(body, grids, iBody, jBody) \
    + fdDdistDhDir08(body, grids, iBody, jBody) \
    + fdDdistDhDir09(body, grids, iBody, jBody) \
    + fdDdistDhDir010(body, grids, iBody, jBody) \
    + fdDdistDhDir011(body, grids, iBody, jBody) \
    + fdDdistDhDir013(body, grids, iBody, jBody) \
    + fdDdistDhDir014(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dh'------------------------------------------------------------------

double fdDdistDhPrmDir01(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dhecc*(grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][1] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][4] + 2*(pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][5] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2) + pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][6]);
}

double fdDdistDhPrmDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dhecc*(grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][9] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][10] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)+pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][12]) + (2*body[iBody].dhecc*body[jBody].dkecc*body[iBody].dkecc + 3*pow(body[iBody].dhecc,2)*body[jBody].dhecc + body[jBody].dhecc*pow(body[iBody].dkecc,2)) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][11];
}

double fdDdistDhPrmDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dhecc*(body[iBody].dpinc*body[jBody].dpinc+body[iBody].dqinc*body[jBody].dqinc)*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][14];
}

double fdDdistDhPrmDir04(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return (2*body[iBody].dhecc*(pow(body[jBody].dhecc,2)-pow(body[jBody].dkecc,2)) + 4*body[jBody].dhecc*body[jBody].dkecc*body[iBody].dkecc) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][16];
}

double fdDdistDhPrmDir06(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dhecc*(pow(body[jBody].dpinc,2)-pow(body[jBody].dqinc,2)) + 2*body[jBody].dkecc*body[jBody].dpinc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDhPrmDir07(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( 2*body[iBody].dhecc*(pow(body[jBody].dpinc,2)-pow(body[jBody].dqinc,2)) + 4*body[jBody].dpinc*body[jBody].dqinc*body[iBody].dkecc ) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][19];
}

double fdDdistDhPrmDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dhecc*(body[iBody].dqinc*body[jBody].dqinc+body[iBody].dpinc*body[jBody].dpinc) + body[jBody].dkecc*(body[iBody].dpinc*body[jBody].dqinc-body[iBody].dqinc*body[jBody].dpinc) ) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][21];
}

double fdDdistDhPrmDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dhecc*(body[iBody].dqinc*body[jBody].dqinc+body[iBody].dpinc*body[jBody].dpinc) + body[jBody].dkecc*(body[iBody].dqinc*body[jBody].dpinc-body[iBody].dpinc*body[jBody].dqinc) ) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][22];
}

double fdDdistDhPrmDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dhecc*(-body[iBody].dqinc*body[jBody].dqinc+body[iBody].dpinc*body[jBody].dpinc) + body[jBody].dkecc*(body[iBody].dqinc*body[jBody].dpinc+body[iBody].dpinc*body[jBody].dqinc) )* grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][23];
}

double fdDdistDhPrmDir012(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dhecc*(body[iBody].dpinc*body[jBody].dpinc-body[iBody].dqinc*body[jBody].dqinc) + body[iBody].dkecc*(body[iBody].dpinc*body[jBody].dqinc+body[iBody].dqinc*body[jBody].dpinc) )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][24];
}

double fdDdistDhPrmDir014(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dhecc*(pow(body[iBody].dpinc,2)-pow(body[iBody].dqinc,2)) + 2*body[jBody].dkecc*body[iBody].dpinc*body[iBody].dqinc ) *grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDhPrmDir015(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dhecc*(pow(body[iBody].dpinc,2)-pow(body[iBody].dqinc,2)) + 2*body[iBody].dpinc*body[iBody].dqinc*body[iBody].dkecc ) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][19];
}

double fdDdisturbDheccPrime(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  y = ( fdDdistDhPrmDir01(body, grids, iBody, jBody) \
    + fdDdistDhPrmDir02(body, grids, iBody, jBody) \
    + fdDdistDhPrmDir03(body, grids, iBody, jBody) \
    + fdDdistDhPrmDir04(body, grids, iBody, jBody) \
    + fdDdistDhPrmDir06(body, grids, iBody, jBody) \
    + fdDdistDhPrmDir07(body, grids, iBody, jBody) \
    + fdDdistDhPrmDir09(body, grids, iBody, jBody) \
    + fdDdistDhPrmDir010(body, grids, iBody, jBody) \
    + fdDdistDhPrmDir011(body, grids, iBody, jBody) \
    + fdDdistDhPrmDir012(body, grids, iBody, jBody) \
    + fdDdistDhPrmDir014(body, grids, iBody, jBody) \
    + fdDdistDhPrmDir015(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//------------dR/dk--------------------------------------------------------------

double fdDdistDkDir01(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dkecc*( grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][1] + 2*(pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][3] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][4] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2) + pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][6] );
}

double fdDdistDkDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dkecc*( grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][9] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][11] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][12] ) + (3*pow(body[iBody].dkecc,2)*body[jBody].dkecc+body[jBody].dkecc*pow(body[iBody].dhecc,2)+2*body[iBody].dkecc*body[iBody].dhecc*body[jBody].dhecc)*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][10];
}

double fdDdistDkDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dkecc*(body[iBody].dpinc*body[jBody].dpinc+body[iBody].dqinc*body[jBody].dqinc)*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][14];
}

double fdDdistDkDir04(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( 2*body[iBody].dkecc*(pow(body[jBody].dkecc,2)-pow(body[jBody].dhecc,2)) + 4*body[iBody].dhecc*body[jBody].dhecc*body[jBody].dkecc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][16];
}

double fdDdistDkDir05(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( 2*body[iBody].dkecc*(pow(body[iBody].dqinc,2)-pow(body[iBody].dpinc,2)) + 4*body[iBody].dpinc*body[iBody].dqinc*body[iBody].dhecc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDkDir06(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*(pow(body[iBody].dqinc,2)-pow(body[iBody].dpinc,2)) + 2*body[jBody].dhecc*body[iBody].dpinc*body[iBody].dqinc )* grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][18];
}

double fdDdistDkDir08(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dkecc*(body[jBody].dqinc*body[iBody].dqinc - body[jBody].dpinc*body[iBody].dpinc) + body[iBody].dhecc*(body[jBody].dpinc*body[iBody].dqinc + body[jBody].dqinc*body[iBody].dpinc) ) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][20];
}

double fdDdistDkDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*(body[jBody].dqinc*body[iBody].dqinc + body[jBody].dpinc*body[iBody].dpinc) + body[jBody].dhecc*(body[jBody].dpinc*body[iBody].dqinc - body[jBody].dqinc*body[iBody].dpinc) ) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][21];
}

double fdDdistDkDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*(body[jBody].dqinc*body[iBody].dqinc + body[jBody].dpinc*body[iBody].dpinc) + body[jBody].dhecc*(body[jBody].dqinc*body[iBody].dpinc - body[jBody].dpinc*body[iBody].dqinc) ) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][22];
}

double fdDdistDkDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*(body[jBody].dqinc*body[iBody].dqinc-body[jBody].dpinc*body[iBody].dpinc) + body[jBody].dhecc*(body[jBody].dpinc*body[iBody].dqinc+body[jBody].dqinc*body[iBody].dpinc) ) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][23];
}

double fdDdistDkDir013(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( 2*body[iBody].dkecc*(pow(body[jBody].dqinc,2)-pow(body[jBody].dpinc,2)) + 4*body[jBody].dpinc*body[jBody].dqinc*body[iBody].dhecc ) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDkDir014(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*(pow(body[jBody].dqinc,2)-pow(body[jBody].dpinc,2)) + 2*body[jBody].dhecc*body[jBody].dpinc*body[jBody].dqinc ) * grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][18];
}

double fdDdisturbDkecc(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  y = ( fdDdistDkDir01(body, grids, iBody, jBody) \
    + fdDdistDkDir02(body, grids, iBody, jBody) \
    + fdDdistDkDir03(body, grids, iBody, jBody) \
    + fdDdistDkDir04(body, grids, iBody, jBody) \
    + fdDdistDkDir05(body, grids, iBody, jBody) \
    + fdDdistDkDir06(body, grids, iBody, jBody) \
    + fdDdistDkDir08(body, grids, iBody, jBody) \
    + fdDdistDkDir09(body, grids, iBody, jBody) \
    + fdDdistDkDir010(body, grids, iBody, jBody) \
    + fdDdistDkDir011(body, grids, iBody, jBody) \
    + fdDdistDkDir013(body, grids, iBody, jBody) \
    + fdDdistDkDir014(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//----------dR/dk'------------------------------------------------------------
double fdDdistDkPrmDir01(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dkecc*(grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][1] + (pow(body[jBody].dhecc,2) + pow(body[jBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][4] + 2*(pow(body[iBody].dhecc,2) + pow(body[iBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][5] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2) + pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][6]);
}

double fdDdistDkPrmDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dkecc*(grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][9] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][10] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2) + pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][12]) + (pow(body[iBody].dhecc,2)*body[jBody].dkecc+3*body[jBody].dkecc*pow(body[iBody].dkecc,2)+2*body[iBody].dkecc*body[jBody].dhecc*body[iBody].dhecc)*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][11];
}

double fdDdistDkPrmDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dkecc*(body[jBody].dpinc*body[iBody].dpinc+body[iBody].dqinc*body[jBody].dqinc)*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][14];
}

double fdDdistDkPrmDir04(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dkecc*(pow(body[jBody].dkecc,2)-pow(body[jBody].dhecc,2)) + 2*body[jBody].dhecc*body[iBody].dhecc*body[jBody].dkecc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][16];
}

double fdDdistDkPrmDir06(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*(pow(body[jBody].dqinc,2) - pow(body[jBody].dpinc,2)) + 2*body[jBody].dhecc*body[jBody].dpinc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDkPrmDir07(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dkecc*(pow(body[jBody].dqinc,2)-pow(body[jBody].dpinc,2)) + 2*body[jBody].dpinc*body[jBody].dqinc*body[iBody].dhecc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][19];
}

double fdDdistDkPrmDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*(body[iBody].dqinc*body[jBody].dqinc+body[iBody].dpinc*body[jBody].dpinc) - body[jBody].dhecc*(body[iBody].dpinc*body[jBody].dqinc-body[iBody].dqinc*body[jBody].dpinc) )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][21];
}

double fdDdistDkPrmDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*(body[iBody].dqinc*body[jBody].dqinc + body[iBody].dpinc*body[jBody].dpinc) - body[jBody].dhecc*(body[iBody].dqinc*body[jBody].dpinc-body[iBody].dpinc*body[jBody].dqinc) )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][22];
}

double fdDdistDkPrmDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*(body[jBody].dqinc*body[iBody].dqinc-body[jBody].dpinc*body[iBody].dpinc) + body[jBody].dhecc*(body[iBody].dpinc*body[jBody].dqinc+body[iBody].dqinc*body[jBody].dpinc) )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][23];
}

double fdDdistDkPrmDir012(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dkecc*(body[iBody].dqinc*body[jBody].dqinc-body[iBody].dpinc*body[jBody].dpinc) + body[iBody].dhecc*(body[iBody].dpinc*body[jBody].dqinc+body[iBody].dqinc*body[jBody].dpinc) )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][24];
}

double fdDdistDkPrmDir014(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*(pow(body[iBody].dqinc,2)-pow(body[iBody].dpinc,2)) + 2*body[jBody].dhecc*body[iBody].dpinc*body[iBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDkPrmDir015(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dkecc*(pow(body[iBody].dqinc,2)-pow(body[iBody].dpinc,2)) + 2*body[iBody].dpinc*body[iBody].dqinc*body[iBody].dhecc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][19];
}

double fdDdisturbDkeccPrime(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  y = ( fdDdistDkPrmDir01(body, grids, iBody, jBody) \
    + fdDdistDkPrmDir02(body, grids, iBody, jBody) \
    + fdDdistDkPrmDir03(body, grids, iBody, jBody) \
    + fdDdistDkPrmDir04(body, grids, iBody, jBody) \
    + fdDdistDkPrmDir06(body, grids, iBody, jBody) \
    + fdDdistDkPrmDir07(body, grids, iBody, jBody) \
    + fdDdistDkPrmDir09(body, grids, iBody, jBody) \
    + fdDdistDkPrmDir010(body, grids, iBody, jBody) \
    + fdDdistDkPrmDir011(body, grids, iBody, jBody) \
    + fdDdistDkPrmDir012(body, grids, iBody, jBody) \
    + fdDdistDkPrmDir014(body, grids, iBody, jBody) \
    + fdDdistDkPrmDir015(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//------------dR/dp---------------------------------------------------------//
double fdDdistDpDir01(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dpinc*( grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][2] + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)+pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][6] + 2*(pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][7] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][8] );
}

double fdDdistDpDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dpinc*(body[iBody].dhecc*body[jBody].dhecc + body[iBody].dkecc*body[jBody].dkecc)*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][12];
}

double fdDdistDpDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dpinc*( grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][13] + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)+pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][14] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][15] ) + 2*body[iBody].dpinc*(body[iBody].dpinc*body[jBody].dpinc+body[iBody].dqinc*body[jBody].dqinc)*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][15];
}

double fdDdistDpDir05(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dpinc*pow(body[iBody].dhecc,2)-body[iBody].dpinc*pow(body[iBody].dkecc,2)+2*body[iBody].dqinc*body[iBody].dhecc*body[iBody].dkecc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDpDir06(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dhecc*body[jBody].dhecc*body[iBody].dpinc - body[iBody].dkecc*body[jBody].dkecc*body[iBody].dpinc + body[jBody].dhecc*body[iBody].dkecc*body[iBody].dqinc + body[jBody].dkecc*body[iBody].dhecc*body[iBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][18];
}

double fdDdistDpDir07(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*(body[iBody].dpinc*pow(body[jBody].dhecc,2) - body[iBody].dpinc*pow(body[jBody].dkecc,2) + 2*body[iBody].dqinc*body[jBody].dhecc*body[jBody].dkecc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][19];
}

double fdDdistDpDir08(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dpinc*pow(body[iBody].dhecc,2)-body[jBody].dpinc*pow(body[iBody].dkecc,2)+2*body[iBody].dhecc*body[iBody].dkecc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][20];
}

double fdDdistDpDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*body[iBody].dkecc*body[jBody].dpinc + body[jBody].dhecc*body[iBody].dhecc*body[jBody].dpinc - body[jBody].dhecc*body[iBody].dkecc*body[jBody].dqinc + body[jBody].dkecc*body[iBody].dhecc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][21];
}

double fdDdistDpDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*body[iBody].dkecc*body[jBody].dpinc + body[jBody].dhecc*body[iBody].dhecc*body[jBody].dpinc + body[jBody].dhecc*body[iBody].dkecc*body[jBody].dqinc - body[jBody].dkecc*body[iBody].dhecc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][22];
}

double fdDdistDpDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( -body[jBody].dkecc*body[iBody].dkecc*body[jBody].dpinc + body[jBody].dhecc*body[iBody].dhecc*body[jBody].dpinc + body[jBody].dhecc*body[iBody].dkecc*body[jBody].dqinc + body[jBody].dkecc*body[iBody].dhecc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][23];
}

double fdDdistDpDir012(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( -pow(body[jBody].dkecc,2)*body[jBody].dpinc + pow(body[jBody].dhecc,2)*body[jBody].dpinc + 2*body[jBody].dhecc*body[jBody].dkecc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][24];
}

double fdDdistDpDir016(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*(body[iBody].dpinc*pow(body[jBody].dpinc,2)-body[iBody].dpinc*pow(body[jBody].dqinc,2)+2*body[jBody].dpinc*body[jBody].dqinc*body[iBody].dqinc)*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][25];
}

double fdDdisturbDpinc(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  y = ( fdDdistDpDir01(body, grids, iBody, jBody) \
     + fdDdistDpDir02(body, grids, iBody, jBody) \
    + fdDdistDpDir03(body, grids, iBody, jBody) \
    + fdDdistDpDir05(body, grids, iBody, jBody) \
    + fdDdistDpDir06(body, grids, iBody, jBody) \
    + fdDdistDpDir07(body, grids, iBody, jBody) \
    + fdDdistDpDir08(body, grids, iBody, jBody) \
    + fdDdistDpDir09(body, grids, iBody, jBody) \
    + fdDdistDpDir010(body, grids, iBody, jBody) \
    + fdDdistDpDir011(body, grids, iBody, jBody) \
    + fdDdistDpDir012(body, grids, iBody, jBody) \
    + fdDdistDpDir016(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//-------dR/dp'------------------------------------------------------------//
double fdDdistDpPrmDir01(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dpinc*( grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][2] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2)+pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][6] + 2*(pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][7] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][8] );
}

double fdDdistDpPrmDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dpinc*(body[jBody].dhecc*body[iBody].dhecc+body[jBody].dkecc*body[iBody].dkecc)*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][12];
}

double fdDdistDpPrmDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dpinc*( grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][13] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2)+pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][14] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)+pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][15] ) + 2*body[iBody].dpinc*(body[jBody].dpinc*body[iBody].dpinc+body[jBody].dqinc*body[iBody].dqinc)*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][15];
}

double fdDdistDpPrmDir08(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( -pow(body[jBody].dkecc,2)*body[jBody].dpinc+pow(body[jBody].dhecc,2)*body[jBody].dpinc+2*body[jBody].dhecc*body[jBody].dkecc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][20];
}

double fdDdistDpPrmDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[iBody].dkecc*body[jBody].dkecc*body[jBody].dpinc + body[iBody].dhecc*body[jBody].dhecc*body[jBody].dpinc + body[iBody].dhecc*body[jBody].dkecc*body[jBody].dqinc - body[iBody].dkecc*body[jBody].dhecc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][21];
}

double fdDdistDpPrmDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[iBody].dkecc*body[jBody].dkecc*body[jBody].dpinc + body[iBody].dhecc*body[jBody].dhecc*body[jBody].dpinc - body[iBody].dhecc*body[jBody].dkecc*body[jBody].dqinc + body[iBody].dkecc*body[jBody].dhecc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][22];
}

double fdDdistDpPrmDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( -body[iBody].dkecc*body[jBody].dkecc*body[jBody].dpinc + body[iBody].dhecc*body[jBody].dhecc*body[jBody].dpinc + body[iBody].dhecc*body[jBody].dkecc*body[jBody].dqinc + body[iBody].dkecc*body[jBody].dhecc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][23];
}

double fdDdistDpPrmDir012(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( -pow(body[iBody].dkecc,2)*body[jBody].dpinc + pow(body[iBody].dhecc,2)*body[jBody].dpinc + 2*body[iBody].dhecc*body[iBody].dkecc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][24];
}

double fdDdistDpPrmDir013(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( -body[iBody].dpinc*pow(body[jBody].dkecc,2) + body[iBody].dpinc*pow(body[jBody].dhecc,2) + 2*body[iBody].dqinc*body[jBody].dhecc*body[jBody].dkecc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][17];
}

double fdDdistDpPrmDir014(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( -body[iBody].dkecc*body[jBody].dkecc*body[iBody].dpinc + body[iBody].dhecc*body[jBody].dhecc*body[iBody].dpinc + body[iBody].dhecc*body[jBody].dkecc*body[iBody].dqinc + body[iBody].dkecc*body[jBody].dhecc*body[iBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDpPrmDir015(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( -body[iBody].dpinc*pow(body[iBody].dkecc,2) + body[iBody].dpinc*pow(body[iBody].dhecc,2) + 2*body[iBody].dqinc*body[iBody].dhecc*body[iBody].dkecc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][19];
}

double fdDdistDpPrmDir016(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( -body[iBody].dpinc*pow(body[jBody].dqinc,2) + pow(body[jBody].dpinc,2)*body[iBody].dpinc + 2*body[iBody].dqinc*body[jBody].dpinc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][25];
}

double fdDdisturbDpincPrime(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  y = ( fdDdistDpPrmDir01(body, grids, iBody, jBody) \
    + fdDdistDpPrmDir02(body, grids, iBody, jBody) \
    + fdDdistDpPrmDir03(body, grids, iBody, jBody) \
    + fdDdistDpPrmDir08(body, grids, iBody, jBody) \
    + fdDdistDpPrmDir09(body, grids, iBody, jBody) \
    + fdDdistDpPrmDir010(body, grids, iBody, jBody) \
    + fdDdistDpPrmDir011(body, grids, iBody, jBody) \
    + fdDdistDpPrmDir012(body, grids, iBody, jBody) \
    + fdDdistDpPrmDir013(body, grids, iBody, jBody) \
    + fdDdistDpPrmDir014(body, grids, iBody, jBody) \
    + fdDdistDpPrmDir015(body, grids, iBody, jBody) \
    + fdDdistDpPrmDir016(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//--------------dR/dq-------------------------------------------------------//
double fdDdistDqDir01(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dqinc*( grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][2] + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)+pow(body[jBody].dkecc,2)+pow(body[jBody].dhecc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][6] + 2*(pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][7] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][8] );
}

double fdDdistDqDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dqinc*(body[iBody].dhecc*body[jBody].dhecc+body[iBody].dkecc*body[jBody].dkecc)*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][12];
}

double fdDdistDqDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dqinc*( grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][13] + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)+pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][14] + (pow(body[iBody].dpinc,2) + pow(body[iBody].dqinc,2) + pow(body[jBody].dpinc,2) + pow(body[jBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][15] ) + 2*body[iBody].dqinc*(body[iBody].dpinc*body[jBody].dpinc+body[iBody].dqinc*body[jBody].dqinc)*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][15];
}

double fdDdistDqDir05(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dqinc*pow(body[iBody].dkecc,2) - body[iBody].dqinc*pow(body[iBody].dhecc,2) + 2*body[iBody].dpinc*body[iBody].dhecc*body[iBody].dkecc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDqDir06(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[jBody].dkecc*body[iBody].dkecc*body[iBody].dqinc - body[jBody].dhecc*body[iBody].dhecc*body[iBody].dqinc + body[jBody].dhecc*body[iBody].dkecc*body[iBody].dpinc + body[jBody].dkecc*body[iBody].dhecc*body[iBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][18];
}

double fdDdistDqDir07(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dqinc*pow(body[jBody].dkecc,2) - body[iBody].dqinc*pow(body[jBody].dhecc,2) + 2*body[iBody].dpinc*body[jBody].dhecc*body[jBody].dkecc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][19];
}

double fdDdistDqDir08(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( pow(body[iBody].dkecc,2)*body[jBody].dqinc - pow(body[iBody].dhecc,2)*body[jBody].dqinc + 2*body[iBody].dhecc*body[iBody].dkecc*body[jBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][20];
}

double fdDdistDqDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*body[iBody].dkecc*body[jBody].dqinc + body[jBody].dhecc*body[iBody].dhecc*body[jBody].dqinc + body[jBody].dhecc*body[iBody].dkecc*body[jBody].dpinc - body[jBody].dkecc*body[iBody].dhecc*body[jBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][21];
}

double fdDdistDqDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*body[iBody].dkecc*body[jBody].dqinc + body[iBody].dhecc*body[jBody].dhecc*body[jBody].dqinc - body[jBody].dhecc*body[iBody].dkecc*body[jBody].dpinc + body[jBody].dkecc*body[iBody].dhecc*body[jBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][22];
}

double fdDdistDqDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[jBody].dkecc*body[iBody].dkecc*body[jBody].dqinc - body[jBody].dhecc*body[iBody].dhecc*body[jBody].dqinc + body[jBody].dhecc*body[iBody].dkecc*body[jBody].dpinc + body[jBody].dkecc*body[iBody].dhecc*body[jBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][23];
}

double fdDdistDqDir012(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( pow(body[jBody].dkecc,2)*body[jBody].dqinc - pow(body[jBody].dhecc,2)*body[jBody].dqinc + 2*body[jBody].dhecc*body[jBody].dkecc*body[jBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][24];
}

double fdDdistDqDir016(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dqinc*pow(body[jBody].dqinc,2) - body[iBody].dqinc*pow(body[jBody].dpinc,2) + 2*body[jBody].dpinc*body[jBody].dqinc*body[iBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[iBody][jBody]][25];
}

  
double fdDdisturbDqinc(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  y = ( fdDdistDqDir01(body, grids, iBody, jBody) \
     + fdDdistDqDir02(body, grids, iBody, jBody) \
    + fdDdistDqDir03(body, grids, iBody, jBody) \
    + fdDdistDqDir05(body, grids, iBody, jBody) \
    + fdDdistDqDir06(body, grids, iBody, jBody) \
    + fdDdistDqDir07(body, grids, iBody, jBody) \
    + fdDdistDqDir08(body, grids, iBody, jBody) \
    + fdDdistDqDir09(body, grids, iBody, jBody) \
    + fdDdistDqDir010(body, grids, iBody, jBody) \
    + fdDdistDqDir011(body, grids, iBody, jBody) \
    + fdDdistDqDir012(body, grids, iBody, jBody) \
    + fdDdistDqDir016(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

//------dR/dq'--------------------------------------------------------------//
double fdDdistDqPrmDir01(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dqinc*( grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][2] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2)+pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][6] + 2*(pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][7] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][8] );
}

double fdDdistDqPrmDir02(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*body[iBody].dqinc*(body[jBody].dhecc*body[iBody].dhecc+body[jBody].dkecc*body[iBody].dkecc)*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][12];
}

double fdDdistDqPrmDir03(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[jBody].dqinc*( grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][13] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2)+pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][14] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)+pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][15] ) + 2*body[iBody].dqinc*(body[jBody].dpinc*body[iBody].dpinc+body[jBody].dqinc*body[iBody].dqinc)*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][15];
}

double fdDdistDqPrmDir08(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( pow(body[jBody].dkecc,2)*body[jBody].dqinc - pow(body[jBody].dhecc,2)*body[jBody].dqinc + 2*body[jBody].dhecc*body[jBody].dkecc*body[jBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][20];
}

double fdDdistDqPrmDir09(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[iBody].dkecc*body[jBody].dkecc*body[jBody].dqinc + body[iBody].dhecc*body[jBody].dhecc*body[jBody].dqinc - body[iBody].dhecc*body[jBody].dkecc*body[jBody].dpinc + body[iBody].dkecc*body[jBody].dhecc*body[jBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][21];
}

double fdDdistDqPrmDir010(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[iBody].dkecc*body[jBody].dkecc*body[jBody].dqinc + body[iBody].dhecc*body[jBody].dhecc*body[jBody].dqinc + body[iBody].dhecc*body[jBody].dkecc*body[jBody].dpinc - body[iBody].dkecc*body[jBody].dhecc*body[jBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][22];
}

double fdDdistDqPrmDir011(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( body[iBody].dkecc*body[jBody].dkecc*body[jBody].dqinc - body[iBody].dhecc*body[jBody].dhecc*body[jBody].dqinc + body[iBody].dhecc*body[jBody].dkecc*body[jBody].dpinc + body[iBody].dkecc*body[jBody].dhecc*body[jBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][23];
}

double fdDdistDqPrmDir012(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( pow(body[iBody].dkecc,2)*body[jBody].dqinc - pow(body[iBody].dhecc,2)*body[jBody].dqinc + 2*body[iBody].dhecc*body[iBody].dkecc*body[jBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][24];
}
  
double fdDdistDqPrmDir013(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dqinc*pow(body[jBody].dkecc,2) - body[iBody].dqinc*pow(body[jBody].dhecc,2) + 2*body[iBody].dpinc*body[jBody].dhecc*body[jBody].dkecc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][17];
}

double fdDdistDqPrmDir014(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dqinc*body[jBody].dkecc*body[iBody].dkecc - body[iBody].dqinc*body[jBody].dhecc*body[iBody].dhecc + body[iBody].dhecc*body[jBody].dkecc*body[iBody].dpinc + body[iBody].dkecc*body[jBody].dhecc*body[iBody].dpinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDqPrmDir015(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( body[iBody].dqinc*pow(body[iBody].dkecc,2) - body[iBody].dqinc*pow(body[iBody].dhecc,2) + 2*body[iBody].dpinc*body[iBody].dhecc*body[iBody].dkecc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][19];
}

double fdDdistDqPrmDir016(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2*( pow(body[jBody].dqinc,2)*body[iBody].dqinc - pow(body[jBody].dpinc,2)*body[iBody].dqinc + 2*body[iBody].dpinc*body[jBody].dpinc*body[jBody].dqinc )*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][25];
} 
    
double fdDdisturbDqincPrime(BODY *body, GRIDS *grids, int iBody, int jBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  y = ( fdDdistDqPrmDir01(body, grids, iBody, jBody) \
    + fdDdistDqPrmDir02(body, grids, iBody, jBody) \
    + fdDdistDqPrmDir03(body, grids, iBody, jBody) \
    + fdDdistDqPrmDir08(body, grids, iBody, jBody) \
    + fdDdistDqPrmDir09(body, grids, iBody, jBody) \
    + fdDdistDqPrmDir010(body, grids, iBody, jBody) \
    + fdDdistDqPrmDir011(body, grids, iBody, jBody) \
    + fdDdistDqPrmDir012(body, grids, iBody, jBody) \
    + fdDdistDqPrmDir013(body, grids, iBody, jBody) \
    + fdDdistDqPrmDir014(body, grids, iBody, jBody) \
    + fdDdistDqPrmDir015(body, grids, iBody, jBody) \
    + fdDdistDqPrmDir016(body, grids, iBody, jBody) ) * dMfac/dSemiPrm;
  return y;
}

/*-------- Resonant terms of disturbing function------------------------------- */
//-----R-------first order direct----------------------------------------------
double fdDisturbDir11(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  double y;
  if (body[iBody].dSemi < body[jBody].dSemi) {
    y = ( fdSemiMajAxF27(body[iBody].dSemi/body[jBody].dSemi,iIndexJ) + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*fdSemiMajAxF28(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)+ (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*fdSemiMajAxF29(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)+ (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))* fdSemiMajAxF30(body[iBody].dSemi/body[jBody].dSemi,iIndexJ) ) * (body[iBody].dkecc*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL)+body[iBody].dhecc*sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL));
    return y;
  } else if (body[iBody].dSemi > body[jBody].dSemi) {
    return ( fdSemiMajAxF27(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*fdSemiMajAxF28(body[jBody].dSemi/body[iBody].dSemi,iIndexJ)+ (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*fdSemiMajAxF29(body[jBody].dSemi/body[iBody].dSemi,iIndexJ)+ (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))* fdSemiMajAxF30(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) ) * (body[jBody].dkecc*cos(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL)+body[jBody].dhecc*sin(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL));
  }
}

double fdDisturbDir12(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  if (body[iBody].dSemi < body[jBody].dSemi) {
    return ( fdSemiMajAxF31(body[iBody].dSemi/body[jBody].dSemi,iIndexJ) + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*fdSemiMajAxF32(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)+ (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*fdSemiMajAxF33(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)+ (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))* fdSemiMajAxF34(body[iBody].dSemi/body[jBody].dSemi,iIndexJ) ) * (body[jBody].dkecc*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL)+body[jBody].dhecc*sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL));
  } else if (body[iBody].dSemi > body[jBody].dSemi) {
    return ( fdSemiMajAxF31(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*fdSemiMajAxF32(body[jBody].dSemi/body[iBody].dSemi,iIndexJ)+ (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*fdSemiMajAxF33(body[jBody].dSemi/body[iBody].dSemi,iIndexJ)+ (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))* fdSemiMajAxF34(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) ) * (body[iBody].dkecc*cos(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL)+body[iBody].dhecc*sin(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL));
  }
}

//-----dR/dalpha---first order direct------------------------------------------------
double fdDdistDalphaDir11(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  if (body[iBody].dSemi < body[jBody].dSemi) {
    return ( fdDerivSemiF27(body[iBody].dSemi/body[jBody].dSemi,iIndexJ) + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*fdDerivSemiF28(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)+ (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*fdDerivSemiF29(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)+ (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))* fdDerivSemiF30(body[iBody].dSemi/body[jBody].dSemi,iIndexJ) ) * (body[iBody].dkecc*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL)+body[iBody].dhecc*sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL));
  } else if (body[iBody].dSemi > body[jBody].dSemi) {
    return ( fdDerivSemiF27(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*fdDerivSemiF28(body[jBody].dSemi/body[iBody].dSemi,iIndexJ)+ (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*fdDerivSemiF29(body[jBody].dSemi/body[iBody].dSemi,iIndexJ)+ (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))* fdDerivSemiF30(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) ) * (body[jBody].dkecc*cos(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL)+body[jBody].dhecc*sin(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL));
  }
}

double fdDdistDalphaDir12(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  if (body[iBody].dSemi < body[jBody].dSemi) {
    return ( fdDerivSemiF31(body[iBody].dSemi/body[jBody].dSemi,iIndexJ) + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*fdDerivSemiF32(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)+ (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*fdDerivSemiF33(body[iBody].dSemi/body[jBody].dSemi,iIndexJ)+ (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))* fdDerivSemiF34(body[iBody].dSemi/body[jBody].dSemi,iIndexJ) ) * (body[jBody].dkecc*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL)+body[jBody].dhecc*sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL));
  } else if (body[iBody].dSemi > body[jBody].dSemi) {
    return ( fdDerivSemiF31(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*fdDerivSemiF32(body[jBody].dSemi/body[iBody].dSemi,iIndexJ)+ (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*fdDerivSemiF33(body[jBody].dSemi/body[iBody].dSemi,iIndexJ)+ (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))* fdDerivSemiF34(body[jBody].dSemi/body[iBody].dSemi,iIndexJ) ) * (body[iBody].dkecc*cos(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL)+body[iBody].dhecc*sin(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL));
  }
}

//-----R------first order external perturber---------------------------------
double fdDisturbExt13(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( -2.+pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)+3./2*(pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))+ 2.*(pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)) ) * (body[jBody].dkecc*cos(2.*body[jBody].dMeanL-body[iBody].dMeanL) + body[jBody].dhecc*sin(2.*body[jBody].dMeanL-body[iBody].dMeanL));
}

//-----R------first order internal perturber---------------------------------
double fdDisturbInt13(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 1./4 * fdDisturbExt13(body, grids, jBody, iBody);
}

//-----dR/da----------------------------------------------------------------------------
double fdDdisturbDsemi(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  //This will need to account for various resonance options (2:1, 3:2, 3:1, etc...)
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  y = (fdDdistDalphaDir11(body, grids, iBody, jBody, iIndexJ) + fdDdistDalphaDir12(body, grids, iBody, jBody, iIndexJ) + fdDisturbExt13(body,grids,iBody,jBody)) * dMfac/pow(dSemiPrm,2); 
  return y;
}

//-----dR'/da'----------------------------------------------------------------------------
double fdDdisturbDsemiPrm(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  //This will need to account for various resonance options (2:1, 3:2, 3:1, etc...)
  double y, dMfac, dSemi, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemi = body[jBody].dSemi/AUCM;
  dSemiPrm = body[iBody].dSemi/AUCM;
  y = dMfac * ( -(fdDisturbDir11(body,grids,iBody,jBody,iIndexJ)+fdDisturbDir12(body,grids,iBody,jBody,iIndexJ))/pow(dSemiPrm,2)-(fdDdistDalphaDir11(body, grids, iBody, jBody, iIndexJ)+fdDdistDalphaDir12(body, grids, iBody, jBody, iIndexJ))*dSemi/pow(dSemiPrm,3) + fdDisturbInt13(body,grids,iBody,jBody)/pow(dSemi,2) ); 
  return y;
}

//-------dR/dh--first order terms--------------------------------------------------------------
double fdDdistDhDir11(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return (grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][26] + (3.*pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)) * grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][27] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))* grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][28] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)) * grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][29]) * sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL) + 2.*body[iBody].dhecc*body[iBody].dkecc*grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][27]*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL);
}

double fdDdistDhDir12(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return 2.*body[iBody].dhecc*grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][31]* (body[jBody].dkecc*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL)+body[jBody].dhecc*sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL));
}

//-------dR/dh----first order indirect terms--------------------------------------------------
double fdDdistDhExt13(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2.*body[iBody].dhecc*(body[jBody].dkecc*cos(2.*body[jBody].dMeanL-body[iBody].dMeanL) + body[jBody].dhecc*sin(2.*body[jBody].dMeanL-body[iBody].dMeanL));
}

//-------dR/dh----first order------------------------------------------------------------
double fdDdisturbDhecc1(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  double dMfac, dSemiPrm, dAxRatio;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  dAxRatio = body[iBody].dSemi/body[jBody].dSemi;
  return (fdDdistDhDir11(body,grids,iBody,jBody,iIndexJ) + \
	  fdDdistDhDir12(body,grids,iBody,jBody,iIndexJ) + \
	  dAxRatio*fdDdistDhExt13(body,grids,iBody,jBody)) * dMfac/dSemiPrm;
}

//-------dR'/dh'--first order terms--------------------------------------------------------------
double fdDdistDhPrmDir11(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return 2.*body[iBody].dhecc*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][28]* (body[jBody].dkecc*cos(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL)+body[jBody].dhecc*sin(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL));
}

double fdDdistDhPrmDir12(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return (grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][30] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2)) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][31] + (3.*pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))* grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][32] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][33]) * sin(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL) + 2.*body[iBody].dhecc*body[iBody].dkecc*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][32]*cos(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL);
}

//-------dR'/dh'----first order indirect terms--------------------------------------------------
double fdDdistDhInt13(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 1./4* (-2.+pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2)+9./2*pow(body[iBody].dhecc,2) + 3./2*pow(body[iBody].dkecc,2)+2.*(pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2) + pow(body[jBody].dqinc,2))*sin(2.*body[iBody].dMeanL-body[jBody].dMeanL) + 3.*body[iBody].dhecc*body[iBody].dkecc*cos(2*body[iBody].dMeanL-body[jBody].dMeanL));
}

//-------dR'/dh'----first order------------------------------------------------------------
double fdDdisturbDheccPrime1(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  double dMfac, dSemiPrm, dAxRatio;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  dAxRatio = body[jBody].dSemi/body[iBody].dSemi;
  return (fdDdistDhPrmDir11(body,grids,iBody,jBody,iIndexJ) + \
	  fdDdistDhPrmDir12(body,grids,iBody,jBody,iIndexJ) + \
	  1./pow(dAxRatio,2)*fdDdistDhInt13(body,grids,iBody,jBody)) * dMfac/dSemiPrm;
}

//--------dR/dk-----first order terms--------------------------------------------------------
double fdDdistDkDir11(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return (grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][26] + (pow(body[iBody].dhecc,2)+3.*pow(body[iBody].dkecc,2)) * grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][27] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))* grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][28] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)) * grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][29]) * cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL) + 2.*body[iBody].dhecc*body[iBody].dkecc*grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][27]*sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL);
}

double fdDdistDkDir12(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return 2.*body[iBody].dkecc*grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][31]* (body[jBody].dkecc*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL)+body[jBody].dhecc*sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL));
}
  
//-------dR/dk----first order indirect terms--------------------------------------------------
double fdDdistDkExt13(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 2.*body[iBody].dkecc*(body[jBody].dkecc*cos(2.*body[jBody].dMeanL-body[iBody].dMeanL) + body[jBody].dhecc*sin(2.*body[jBody].dMeanL-body[iBody].dMeanL));
}  
  
//-------dR/dk----first order------------------------------------------------------------
double fdDdisturbDkecc1(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  double dMfac, dSemiPrm, dAxRatio;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  dAxRatio = body[iBody].dSemi/body[jBody].dSemi;
  return (fdDdistDkDir11(body,grids,iBody,jBody,iIndexJ) + \
	  fdDdistDkDir12(body,grids,iBody,jBody,iIndexJ) + \
	  dAxRatio*fdDdistDkExt13(body,grids,iBody,jBody)) * dMfac/dSemiPrm;
}  
  
 //-------dR'/dk'--first order terms--------------------------------------------------------------
double fdDdistDkPrmDir11(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return 2.*body[iBody].dkecc*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][28]* (body[jBody].dkecc*cos(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL)+body[jBody].dhecc*sin(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL));
}

double fdDdistDkPrmDir12(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return (grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][30] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2)) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][31] + (pow(body[iBody].dhecc,2)+3.*pow(body[iBody].dkecc,2))* grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][32] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)) * grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][33]) * cos(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL) + 2.*body[iBody].dhecc*body[iBody].dkecc*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][32]*sin(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL);
}

//-------dR'/dk'----first order indirect terms--------------------------------------------------
double fdDdistDkInt13(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 1./4* (-2.+pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2)+9./2*pow(body[iBody].dhecc,2) + 3./2*pow(body[iBody].dkecc,2)+2.*(pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2) + pow(body[jBody].dqinc,2))*sin(2.*body[iBody].dMeanL-body[jBody].dMeanL) + 3.*body[iBody].dhecc*body[iBody].dkecc*cos(2*body[iBody].dMeanL-body[jBody].dMeanL));
}

//-------dR'/dk'----first order------------------------------------------------------------
double fdDdisturbDkeccPrime1(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  double dMfac, dSemiPrm, dAxRatio;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  dAxRatio = body[jBody].dSemi/body[iBody].dSemi;
  return (fdDdistDkPrmDir11(body,grids,iBody,jBody,iIndexJ) + \
	  fdDdistDkPrmDir12(body,grids,iBody,jBody,iIndexJ) + \
	  1./pow(dAxRatio,2)*fdDdistDkInt13(body,grids,iBody,jBody)) * dMfac/dSemiPrm;
} 
  
//-------dR/dp----first order terms------------------------------------------------------
double fdDdistDpDir11(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return 2.*body[iBody].dpinc*grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][29] * (body[iBody].dkecc*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL) + body[iBody].dhecc*sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL));
}

double fdDdistDpDir12(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return 2.*body[iBody].dpinc*grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][33] * (body[jBody].dkecc*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL) + body[jBody].dhecc*sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL));
}  

//-------dR/dp----first order indirect terms--------------------------------------------------
double fdDdistDpExt13(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 4.*body[iBody].dpinc*(body[jBody].dkecc*cos(2.*body[jBody].dMeanL-body[iBody].dMeanL) + body[jBody].dhecc*sin(2.*body[jBody].dMeanL-body[iBody].dMeanL));
} 

//-------dR/dp----first order------------------------------------------------------------
double fdDdisturbDpinc1(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  double dMfac, dSemiPrm, dAxRatio;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  dAxRatio = body[iBody].dSemi/body[jBody].dSemi;
  return (fdDdistDpDir11(body,grids,iBody,jBody,iIndexJ) + \
	  fdDdistDpDir12(body,grids,iBody,jBody,iIndexJ) + \
	  dAxRatio,2*fdDdistDpExt13(body,grids,iBody,jBody)) * dMfac/dSemiPrm;
}  

//-------dR'/dp'----first order terms------------------------------------------------------
double fdDdistDpPrmDir11(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return 2.*body[iBody].dpinc*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][29] * (body[jBody].dkecc*cos(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL) + body[jBody].dhecc*sin(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL));
}

double fdDdistDpPrmDir12(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return 2.*body[iBody].dpinc*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][33] * (body[iBody].dkecc*cos(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL) + body[iBody].dhecc*sin(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL));
}  

//-------dR'/dp'----first order indirect terms--------------------------------------------------
double fdDdistDpInt13(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dpinc*(body[iBody].dkecc*cos(2.*body[iBody].dMeanL-body[jBody].dMeanL) + body[iBody].dhecc*sin(2.*body[iBody].dMeanL-body[jBody].dMeanL));
} 

//-------dR'/dp'----first order------------------------------------------------------------
double fdDdisturbDpincPrime1(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  double dMfac, dSemiPrm, dAxRatio;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  dAxRatio = body[jBody].dSemi/body[iBody].dSemi;
  return (fdDdistDpPrmDir11(body,grids,iBody,jBody,iIndexJ) + \
	  fdDdistDpPrmDir12(body,grids,iBody,jBody,iIndexJ) + \
	  1./pow(dAxRatio,2)*fdDdistDpInt13(body,grids,iBody,jBody)) * dMfac/dSemiPrm;
}  

//-------dR/dq----first order terms------------------------------------------------------
double fdDdistDqDir11(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return 2.*body[iBody].dqinc*grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][29] * (body[iBody].dkecc*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL) + body[iBody].dhecc*sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL));
}

double fdDdistDqDir12(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return 2.*body[iBody].dqinc*grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][33] * (body[jBody].dkecc*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL) + body[jBody].dhecc*sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL));
}  

//-------dR/dq----first order indirect terms--------------------------------------------------
double fdDdistDqExt13(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return 4.*body[iBody].dqinc*(body[jBody].dkecc*cos(2.*body[jBody].dMeanL-body[iBody].dMeanL) + body[jBody].dhecc*sin(2.*body[jBody].dMeanL-body[iBody].dMeanL));
} 

//-------dR/dq----first order------------------------------------------------------------
double fdDdisturbDqinc1(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  double dMfac, dSemiPrm, dAxRatio;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  dAxRatio = body[iBody].dSemi/body[jBody].dSemi;
  return (fdDdistDqDir11(body,grids,iBody,jBody,iIndexJ) + \
	  fdDdistDqDir12(body,grids,iBody,jBody,iIndexJ) + \
	  dAxRatio*fdDdistDqExt13(body,grids,iBody,jBody)) * dMfac/dSemiPrm;
}  

//-------dR'/dq'----first order terms------------------------------------------------------
double fdDdistDqPrmDir11(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return 2.*body[iBody].dqinc*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][29] * (body[jBody].dkecc*cos(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL) + body[jBody].dhecc*sin(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL));
}

double fdDdistDqPrmDir12(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return 2.*body[iBody].dqinc*grids->dmLaplaceC[grids->imLaplaceN[jBody][iBody]][33] * (body[iBody].dkecc*cos(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL) + body[iBody].dhecc*sin(iIndexJ*body[iBody].dMeanL+(1-iIndexJ)*body[jBody].dMeanL));
}  

//-------dR'/dq'----first order indirect terms--------------------------------------------------
double fdDdistDqInt13(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return body[iBody].dqinc*(body[iBody].dkecc*cos(2.*body[iBody].dMeanL-body[jBody].dMeanL) + body[iBody].dhecc*sin(2.*body[iBody].dMeanL-body[jBody].dMeanL));
} 

//-------dR'/dq'----first order------------------------------------------------------------
double fdDdisturbDqincPrime1(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  double dMfac, dSemiPrm, dAxRatio;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  dAxRatio = body[jBody].dSemi/body[iBody].dSemi;
  return (fdDdistDqPrmDir11(body,grids,iBody,jBody,iIndexJ) + \
	  fdDdistDqPrmDir12(body,grids,iBody,jBody,iIndexJ) + \
	  1./pow(dAxRatio,2)*fdDdistDqInt13(body,grids,iBody,jBody)) * dMfac/dSemiPrm;
}  

//-------dR/dlambda-----first order terms------------------------------------------------
double fdDdistDmeanlDir11(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return (1-iIndexJ)*(grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][26] + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)) * grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][27] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))* grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][28] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)) * grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][29]) * (-body[iBody].dkecc *sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL) + body[iBody].dhecc*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL));
}

double fdDdistDmeanlDir12(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return (1-iIndexJ)*(grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][30] + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)) * grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][31] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))* grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][32] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)) * grids->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][33]) * (-body[iBody].dkecc *sin(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL) + body[iBody].dhecc*cos(iIndexJ*body[jBody].dMeanL+(1-iIndexJ)*body[iBody].dMeanL));
}

//-------dR/dlambda-----first order indirect terms------------------------------------------------
double fdDdistDmeanlExt13(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return ( -2.+pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)+3./2*(pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))+ 2.*(pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)) ) * (body[jBody].dkecc*sin(2.*body[jBody].dMeanL-body[iBody].dMeanL) - body[jBody].dhecc*cos(2.*body[jBody].dMeanL-body[iBody].dMeanL));
}

//-------dR/dlambda----first order------------------------------------------------------------
double fdDdisturbDmeanl1(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  double dMfac, dSemiPrm, dAxRatio;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[jBody].dSemi/AUCM;
  dAxRatio = body[iBody].dSemi/body[jBody].dSemi;
  return (fdDdistDmeanlDir11(body,grids,iBody,jBody,iIndexJ) + \
	  fdDdistDmeanlDir12(body,grids,iBody,jBody,iIndexJ) + \
	  dAxRatio*fdDdistDmeanlExt13(body,grids,iBody,jBody)) * dMfac/dSemiPrm;
}

//-------dR'/dlambda'-----first order terms------------------------------------------------
double fdDdistDmeanlPrmDir11(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return iIndexJ/(1-iIndexJ) * fdDdistDmeanlDir11(body,grids,jBody,iBody,iIndexJ);
}

double fdDdistDmeanlPrmDir12(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  return iIndexJ/(1-iIndexJ) * fdDdistDmeanlDir12(body,grids,jBody,iBody,iIndexJ);
}

//-------dR'/dlambda'-----first order indirect terms------------------------------------------------
double fdDdistDmeanlInt13(BODY *body, GRIDS *grids, int iBody, int jBody) {
  return -1./2 * fdDdistDmeanlExt13(body,grids,jBody,iBody);
}

//-------dR'/dlambda'----first order------------------------------------------------------------
double fdDdisturbDmeanlPrime1(BODY *body, GRIDS *grids, int iBody, int jBody, int iIndexJ) {
  double dMfac, dSemiPrm, dAxRatio;
  dMfac = KGAUSS*KGAUSS*body[jBody].dMass/MSUN;
  dSemiPrm = body[iBody].dSemi/AUCM;
  dAxRatio = body[jBody].dSemi/body[iBody].dSemi;
  return (fdDdistDmeanlPrmDir11(body,grids,iBody,jBody,iIndexJ) + \
	  fdDdistDmeanlPrmDir12(body,grids,iBody,jBody,iIndexJ) + \
	  1./pow(dAxRatio,2)*fdDdistDmeanlInt13(body,grids,iBody,jBody)) * dMfac/dSemiPrm;
}
  
//----Lagrange's equations!!! ---------------------------------------------------------------
double fdLagrangeDeccDt(BODY *body, GRIDS *grids, int iBody) {
  int i;
  double sum = 0.0, dMu;
  //XXX need to watch units of mean motion, mu, and angles!
  for (i = 1; i < body[iBody].iNumBodies; i++) {
    if (i != iBody) {
      dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
      if (body[iBody].dSemi < body[i].dSemi) {  //calculate derivs if perturber is exterior (iBody = interior)
	
	sum += -(sqrt(1.0-pow(body[iBody].dEcc,2)))/(pow(dMu*body[iBody].dSemi/AUCM, 0.5)) * fdDdisturbDvarpi(body, grids, iBody, i);
	
      } else if (body[iBody].dSemi > body[i].dSemi) {  //calc derivs for interior perturber 
	
	sum += -(sqrt(1.0 - pow(body[iBody].dEcc,2)))/(pow(dMu*body[iBody].dSemi/AUCM, 0.5)) * fdDdisturbDvarpiPrime(body, grids, iBody, i); 
	
      }
    }
  }
  return sum/DAYSEC;
}

double fdLagrangeDvarDt(BODY *body, GRIDS *grids, int iBody) {
  int i;
  double sum = 0.0, dMu;
  
  if (body[iBody].dEcc != 0.0) {
    for (i = 1; i < body[iBody].iNumBodies; i++) {
      if (i != iBody) { 
	dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
	if (body[iBody].dSemi < body[i].dSemi) {
	  
	  sum += ( body[iBody].dsInc / (2.0 * pow(dMu*body[iBody].dSemi/AUCM, 0.5) * sqrt(1.0 - pow(body[iBody].dEcc,2))) ) * fdDdisturbDsInc(body, grids, iBody, i) \
	  + sqrt(1.0 - pow(body[iBody].dEcc,2)) / (pow(dMu*body[iBody].dSemi/AUCM, 0.5) * body[iBody].dEcc) * fdDdisturbDecc(body, grids, iBody, i);
	  
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  
	  sum += ( body[iBody].dsInc / (2.0 * pow(dMu*body[iBody].dSemi/AUCM, 0.5) * sqrt(1.0 - pow(body[iBody].dEcc,2))) ) * fdDdisturbDsIncPrime(body, grids, iBody, i) \
	  + sqrt(1.0 - pow(body[iBody].dEcc,2)) / (pow(dMu*body[iBody].dSemi/AUCM, 0.5) * body[iBody].dEcc) * fdDdisturbDeccPrime(body, grids, iBody, i);
	  
	}
      }
    }
  }
  return sum/DAYSEC;
}

double fdLagrangeDsIncDt(BODY *body, GRIDS *grids, int iBody) {
  int i;
  double sum = 0.0, dMu;
  
  for (i = 1; i < body[iBody].iNumBodies; i++) {
    if (i != iBody) {
      dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
      if (body[iBody].dSemi < body[i].dSemi) {
	
	sum += -( body[iBody].dEcc * body[iBody].dsInc * fdDdisturbDvarpi(body, grids, iBody, i) \
	+ 1./2. * fdDdisturbDOmega(body, grids, iBody, i) ) / (2.0 * pow(dMu*body[iBody].dSemi/AUCM, 0.5) * sqrt(1.0 - pow(body[iBody].dEcc,2)));
	
      } else if (body[iBody].dSemi > body[i].dSemi) {
	
	sum += -( body[iBody].dEcc * body[iBody].dsInc * fdDdisturbDvarpiPrime(body, grids, iBody, i) \
	+ 1./2. * fdDdisturbDOmegaPrime(body, grids, iBody, i) ) / (2.0 * pow(dMu*body[iBody].dSemi/AUCM, 0.5) * sqrt(1.0 - pow(body[iBody].dEcc,2)));
	
      }
    }
  }
  return sum/DAYSEC;
}

double fdLagrangeDOmgDt(BODY *body, GRIDS *grids, int iBody) {
  int i;
  double sum = 0.0, dMu;
  
  if (body[iBody].dsInc != 0.0) {
    for (i = 1; i < body[iBody].iNumBodies; i++) {
      if (i != iBody) { 
	dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
	if (body[iBody].dSemi < body[i].dSemi) {
	  
     	  sum += ( 1.0 / (4.0 * body[iBody].dsInc * pow(dMu*body[iBody].dSemi/AUCM, 0.5) * sqrt(1.0 - pow(body[iBody].dEcc,2))) ) * fdDdisturbDsInc(body, grids, iBody, i);
	  
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  
	  sum += ( 1.0 / (4.0 * body[iBody].dsInc * pow(dMu*body[iBody].dSemi/AUCM, 0.5) * sqrt(1.0 - pow(body[iBody].dEcc,2))) ) * fdDdisturbDsIncPrime(body, grids, iBody, i);
	  
	}
      }
    }
  }
  return sum/DAYSEC;
}

//-------------------h k p q -------------------------------------
double fdLagrangeDhDt(BODY *body, GRIDS *grids, int iBody) {
  int i;
  double sum = 0.0, dMu;
  

  for (i = 1; i < body[iBody].iNumBodies; i++) {
      if (i != iBody) { 
	dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += ( sqrt(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))*fdDdisturbDkecc(body, grids, iBody, i) + body[iBody].dkecc*(body[iBody].dpinc*fdDdisturbDpinc(body, grids, iBody, i) +body[iBody].dqinc*fdDdisturbDqinc(body, grids, iBody, i))/(2*sqrt(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))) ) / sqrt(dMu*body[iBody].dSemi/AUCM);
	    
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += ( sqrt(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))*fdDdisturbDkeccPrime(body, grids, iBody, i) + body[iBody].dkecc*(body[iBody].dpinc*fdDdisturbDpincPrime(body, grids, iBody, i) +body[iBody].dqinc*fdDdisturbDqincPrime(body, grids, iBody, i))/(2*sqrt(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))) ) / sqrt(dMu*body[iBody].dSemi/AUCM);
	}
      }
  }
  return sum/DAYSEC;
}

double fdLagrangeDkDt(BODY *body, GRIDS *grids, int iBody) {
  int i;
  double sum = 0.0, dMu;
  

  for (i = 1; i < body[iBody].iNumBodies; i++) {
      if (i != iBody) { 
	dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += -( sqrt(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))*fdDdisturbDhecc(body, grids, iBody, i) + body[iBody].dhecc*(body[iBody].dpinc*fdDdisturbDpinc(body, grids, iBody, i) +body[iBody].dqinc*fdDdisturbDqinc(body, grids, iBody, i))/(2*sqrt(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))) ) / sqrt(dMu*body[iBody].dSemi/AUCM);
	    
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += -( sqrt(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))*fdDdisturbDheccPrime(body, grids, iBody, i) + body[iBody].dhecc*(body[iBody].dpinc*fdDdisturbDpincPrime(body, grids, iBody, i) +body[iBody].dqinc*fdDdisturbDqincPrime(body, grids, iBody, i))/(2*sqrt(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))) ) / sqrt(dMu*body[iBody].dSemi/AUCM);
	}
      }
  }
  return sum/DAYSEC;
}

double fdLagrangeDpDt(BODY *body, GRIDS *grids, int iBody) {
    int i;
    double sum = 0.0, dMu;
    
    for (i = 1; i < body[iBody].iNumBodies; i++) {
      if (i != iBody) { 
	dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += ( body[iBody].dpinc*(-body[iBody].dkecc*fdDdisturbDhecc(body, grids, iBody, i)+body[iBody].dhecc*fdDdisturbDkecc(body, grids, iBody, i)) + 1.0/2.0*fdDdisturbDqinc(body, grids, iBody, i) )/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))));
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += ( body[iBody].dpinc*(-body[iBody].dkecc*fdDdisturbDheccPrime(body, grids, iBody, i)+body[iBody].dhecc*fdDdisturbDkeccPrime(body, grids, iBody, i)) + 1.0/2.0*fdDdisturbDqincPrime(body, grids, iBody, i) )/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))));
	}
      }
    }
    return sum/DAYSEC;
}


double fdLagrangeDqDt(BODY *body, GRIDS *grids, int iBody) {
    int i;
    double sum = 0.0, dMu;
    
    for (i = 1; i < body[iBody].iNumBodies; i++) {
      if (i != iBody) { 
	dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += ( body[iBody].dqinc*(-body[iBody].dkecc*fdDdisturbDhecc(body, grids, iBody, i)+body[iBody].dhecc*fdDdisturbDkecc(body, grids, iBody, i)) - 1.0/2.0*fdDdisturbDpinc(body, grids, iBody, i) )/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))));
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += ( body[iBody].dqinc*(-body[iBody].dkecc*fdDdisturbDheccPrime(body, grids, iBody, i)+body[iBody].dhecc*fdDdisturbDkeccPrime(body, grids, iBody, i)) - 1.0/2.0*fdDdisturbDpincPrime(body, grids, iBody, i) )/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))));
	}
      }
    }
    return sum/DAYSEC;
}

//-------Resonant/fast Lagrange equations-----------------
/*
double fdLagrangeDhDtRes(BODY *body, GRIDS *grids, int iBody) {
  int i;
  double sum = 0.0, dMu;
  sum = fdLagrangeDhDt(body, grids, iBody);
  
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
  for (i = 1; i < body[iBody].iNumBodies; i++) {
    if (i != iBody) { 
      if (body[iBody].bMMR[i] == 1) {
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += - (body[iBody].dhecc*sqrt(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))) / (sqrt(dMu*body[iBody].dSemi/AUCM)*(1.0+sqrt(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)))) * fdDdisturbDmeanl(body, grids, iBody, i);
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += - (body[iBody].dhecc*sqrt(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))) / (sqrt(dMu*body[iBody].dSemi/AUCM)*(1.0+sqrt(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)))) * fdDdisturbDmeanlPrime(body, grids, iBody, i);
	}
      }
    } 
  }
  return sum/DAYSEC;
}
  
double fdLagrangeDkDtRes(BODY *body, GRIDS *grids, int iBody) {
  int i;
  double sum = 0.0, dMu;
  sum = fdLagrangeDkDt(body, grids, iBody);
  
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
  for (i = 1; i < body[iBody].iNumBodies; i++) {
    if (i != iBody) { 
      if (body[iBody].bMMR[i] == 1) {
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += - (body[iBody].dkecc*sqrt(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))) / (sqrt(dMu*body[iBody].dSemi/AUCM)*(1.0+sqrt(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)))) * fdDdisturbDmeanl(body, grids, iBody, i);
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += - (body[iBody].dkecc*sqrt(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))) / (sqrt(dMu*body[iBody].dSemi/AUCM)*(1.0+sqrt(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)))) * fdDdisturbDmeanlPrime(body, grids, iBody, i);
	}
      }
    } 
  }
  return sum/DAYSEC;
}
  
double fdLagrangeDpDtRes(BODY *body, GRIDS *grids, int iBody) {
  int i;
  double sum = 0.0, dMu;
  sum = fdLagrangeDpDt(body, grids, iBody);
  
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
  for (i = 1; i < body[iBody].iNumBodies; i++) {
    if (i != iBody) { 
      if (body[iBody].bMMR[i] == 1) {
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += - (body[iBody].dpinc) / (2*sqrt(dMu*body[iBody].dSemi/AUCM*(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)))) * fdDdisturbDmeanl(body, grids, iBody, i);
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += - (body[iBody].dpinc / (2*sqrt(dMu*body[iBody].dSemi/AUCM*(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)))) * fdDdisturbDmeanlPrime(body, grids, iBody, i);
	}
      }
    } 
  }
  return sum/DAYSEC;
}

double fdLagrangeDqDtRes(BODY *body, GRIDS *grids, int iBody) {
  int i;
  double sum = 0.0, dMu;
  sum = fdLagrangeDqDt(body, grids, iBody);
  
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
  for (i = 1; i < body[iBody].iNumBodies; i++) {
    if (i != iBody) { 
      if (body[iBody].bMMR[i] == 1) {
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += - (body[iBody].dqinc) / (2*sqrt(dMu*body[iBody].dSemi/AUCM*(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)))) * fdDdisturbDmeanl(body, grids, iBody, i);
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += - (body[iBody].dqinc / (2*sqrt(dMu*body[iBody].dSemi/AUCM*(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)))) * fdDdisturbDmeanlPrime(body, grids, iBody, i);
	}
      }
    } 
  }
  return sum/DAYSEC;
}

double fdLagrangeDsemiDt(BODY *body, GRIDS *grids, int iBody) {
  int i;
  double sum = 0.0, dMu;
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
  
  for (i = 1; i < body[iBody].iNumBodies; i++) {
    if (i != iBody) { 
      if (body[iBody].bMMR[i] == 1) {
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += 2.0 / sqrt(dMu/(body[iBody].dSemi/AUCM)) * fdDdisturbDmeanl(body, grids, iBody, i);
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += 2.0 / sqrt(dMu/(body[iBody].dSemi/AUCM)) * fdDdisturbDmeanlPrime(body, grids, iBody, i);
	}
      }
    }
  }
  return sum/DAYSEC;
}
  
double fdLagrangeDmeanlDt(BODY *body, GRIDS *grids, int iBody) {
  int i;
  double sum = 0.0, dMu;
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
  
  for (i = 1; i < body[iBody].iNumBodies; i++) {
    if (i != iBody) { 
      if (body[iBody].bMMR[i] == 1) {
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += sqrt(dMu/pow(body[iBody].dSemi/AUCM,3)) - 2.0/sqrt(dMu/(body[iBody].dSemi/AUCM)) * fdDdisturbDsemi(body, grids, iBody, i) + sqrt(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)) / (sqrt(dMu*body[iBody].dSemi/AUCM)*(1.0+sqrt(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)))) * (body[iBody].dhecc*fdDdisturbDhecc(body, grids, iBody, i) + body[iBody].dkecc*fdDdisturbDkecc(body, grids, iBody, i)) + 1.0/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)))) * (body[iBody].dpinc*fdDdisturbDpinc(body, grids, iBody, i) + body[iBody].dqinc*fdDdisturbDqinc(body, grids, iBody, i)); 
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += sqrt(dMu/pow(body[iBody].dSemi/AUCM,3)) - 2.0/sqrt(dMu/(body[iBody].dSemi/AUCM)) * fdDdisturbDsemiPrime(body, grids, iBody, i) + sqrt(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)) / (sqrt(dMu*body[iBody].dSemi/AUCM)*(1.0+sqrt(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)))) * (body[iBody].dhecc*fdDdisturbDheccPrime(body, grids, iBody, i) + body[iBody].dkecc*fdDdisturbDkeccPrime(body, grids, iBody, i)) + 1.0/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1.0-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2)))) * (body[iBody].dpinc*fdDdisturbDpincPrime(body, grids, iBody, i) + body[iBody].dqinc*fdDdisturbDqincPrime(body, grids, iBody, i));
	}
      }
    }
  }
  return sum/DAYSEC;
}
*/
/* Equations used to calculate obliquity/spin evolution */
double fdCentralTorqueSfac(BODY *body, int iBody) {
  return 0.5*pow(1-pow(body[iBody].dEcc,2),-1.5) - dS0;
}
  
double fdCentralTorqueR(BODY *body, int iBody) {
  return 3*pow(KGAUSS,2)*body[0].dMass/MSUN/(pow(body[iBody].dSemi/AUCM,3)*body[iBody].dRotRate*DAYSEC)*body[iBody].dDynEllip*fdCentralTorqueSfac(body, iBody)*cos(body[iBody].dObliquity);
}

double fdCentralTorqueRShadow(BODY *body, int iBody) {
  return 3*pow(KGAUSS,2)*body[0].dMass/MSUN/(pow(body[iBody].dSemi/AUCM,3)*body[iBody].dRotRate*DAYSEC)*body[iBody].dDynEllip*fdCentralTorqueSfac(body, iBody)*cos(body[iBody].dShadowObl);
}

double fdObliquityC(BODY *body, GRIDS *grids, int iBody) {
  return body[iBody].dqinc*fdLagrangeDpDt(body,grids,iBody) - body[iBody].dpinc*fdLagrangeDqDt(body,grids,iBody);
}

double fdObliquityA(BODY *body, GRIDS *grids, int iBody) {
  return 2.0/sqrt(1-pow(body[iBody].dpinc,2)-pow(body[iBody].dpinc,2)) * ( fdLagrangeDqDt(body,grids,iBody) + body[iBody].dpinc*fdObliquityC(body,grids,iBody) );
}

double fdObliquityB(BODY *body, GRIDS *grids, int iBody) {
  return 2.0/sqrt(1-pow(body[iBody].dpinc,2)-pow(body[iBody].dpinc,2)) * ( fdLagrangeDpDt(body,grids,iBody) - body[iBody].dqinc*fdObliquityC(body,grids,iBody) );
}


//--------------Obliquity/spin evolution---------------------------------------------------------------
double fdLagrangeDPrecADt(BODY *body, GRIDS *grids, int iBody) {
  return fdCentralTorqueR(body,iBody)/DAYSEC - 1./tan(body[iBody].dObliquity) * (fdObliquityA(body,grids,iBody)*sin(body[iBody].dPrecA)+fdObliquityB(body,grids,iBody)*cos(body[iBody].dPrecA)) - 2.*fdObliquityC(body,grids,iBody);
}
	
double fdLagrangeDOblDt(BODY *body, GRIDS *grids, int iBody) {
  return -fdObliquityB(body,grids,iBody)*sin(body[iBody].dPrecA) + fdObliquityA(body,grids,iBody)*cos(body[iBody].dPrecA);
}

double fdLagrangeDbetaDt(BODY *body, GRIDS *grids, int iBody) {
  double y;
  y = 1.0 - pow(body[iBody].dbeta,2) - pow(body[iBody].dgamma,2);
  if (y < 0.) {
    y *= -1.;
  }
  return -fdObliquityB(body,grids,iBody)*sqrt(y) + body[iBody].dgamma*(fdCentralTorqueR(body,iBody)/DAYSEC - 2.*fdObliquityC(body,grids,iBody));
}

double fdLagrangeDgammaDt(BODY *body, GRIDS *grids, int iBody) {
  double y;
  y = 1.0 - pow(body[iBody].dbeta,2) - pow(body[iBody].dgamma,2);
  if (y < 0.) {
    y *= -1.;
  }
  return fdObliquityA(body,grids,iBody)*sqrt(y) - body[iBody].dbeta*(fdCentralTorqueR(body,iBody)/DAYSEC - 2.*fdObliquityC(body,grids,iBody));
}

double fdLagrangeDetaDt(BODY *body, GRIDS *grids, int iBody) {
  return body[iBody].dbeta*fdObliquityB(body,grids,iBody) - body[iBody].dgamma*fdObliquityA(body,grids,iBody);
}

double fdLagrangeDShadowbetaDt(BODY *body, GRIDS *grids, int iBody) {
  return /*-fdObliquityB(body,grids,iBody)*sqrt(1.0-pow(body[iBody].dbeta,2)-pow(body[iBody].dgamma,2)) +*/ body[iBody].dShadowgamma*(fdCentralTorqueRShadow(body,iBody)/DAYSEC /*- 2.*fdObliquityC(body,grids,iBody)*/);
}

double fdLagrangeDShadowgammaDt(BODY *body, GRIDS *grids, int iBody) {
  return /*fdObliquityA(body,grids,iBody)*sqrt(1.0-pow(body[iBody].dbeta,2)-pow(body[iBody].dgamma,2))*/ - body[iBody].dShadowbeta*(fdCentralTorqueRShadow(body,iBody)/DAYSEC /*- 2.*fdObliquityC(body,grids,iBody)*/);
}	
	
	
/* Isolated derivatives for analysis */	

double fdLagrangeDpDt01(BODY *body, GRIDS *grids, int iBody) {
    int i;
    double sum = 0.0, dMu;
    
    for (i = 1; i < body[iBody].iNumBodies; i++) {
      if (i != iBody) { 
	dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += KGAUSS*KGAUSS*body[i].dMass/MSUN/(body[i].dSemi/AUCM)*( body[iBody].dpinc*(-body[iBody].dkecc*fdDdistDhDir01(body, grids, iBody, i)+body[iBody].dhecc*fdDdistDkDir01(body, grids, iBody, i)) + 1.0/2.0*fdDdistDqDir01(body, grids, iBody, i) )/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))));
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += KGAUSS*KGAUSS*body[i].dMass/MSUN/(body[iBody].dSemi/AUCM)*( body[iBody].dpinc*(-body[iBody].dkecc*fdDdistDhPrmDir01(body, grids, iBody, i)+body[iBody].dhecc*fdDdistDkPrmDir01(body, grids, iBody, i)) + 1.0/2.0*fdDdistDqPrmDir01(body, grids, iBody, i) )/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))));
	}
      }
    }
    return sum/DAYSEC;
}


double fdLagrangeDqDt01(BODY *body, GRIDS *grids, int iBody) {
    int i;
    double sum = 0.0, dMu;
    
    for (i = 1; i < body[iBody].iNumBodies; i++) {
      if (i != iBody) { 
	dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += KGAUSS*KGAUSS*body[i].dMass/MSUN/(body[i].dSemi/AUCM)*( body[iBody].dqinc*(-body[iBody].dkecc*fdDdistDhDir01(body, grids, iBody, i)+body[iBody].dhecc*fdDdistDkDir01(body, grids, iBody, i)) - 1.0/2.0*fdDdistDpDir01(body, grids, iBody, i) )/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))));
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += KGAUSS*KGAUSS*body[i].dMass/MSUN/(body[iBody].dSemi/AUCM)*( body[iBody].dqinc*(-body[iBody].dkecc*fdDdistDhPrmDir01(body, grids, iBody, i)+body[iBody].dhecc*fdDdistDkPrmDir01(body, grids, iBody, i)) - 1.0/2.0*fdDdistDpPrmDir01(body, grids, iBody, i) )/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))));
	}
      }
    }
    return sum/DAYSEC;
}

double fdLagrangeDpDt02(BODY *body, GRIDS *grids, int iBody) {
    int i;
    double sum = 0.0, dMu;
    
    for (i = 1; i < body[iBody].iNumBodies; i++) {
      if (i != iBody) { 
	dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += KGAUSS*KGAUSS*body[i].dMass/MSUN/(body[i].dSemi/AUCM)*( body[iBody].dpinc*(-body[iBody].dkecc*fdDdistDhDir02(body, grids, iBody, i)+body[iBody].dhecc*fdDdistDkDir02(body, grids, iBody, i)) + 1.0/2.0*fdDdistDqDir02(body, grids, iBody, i) )/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))));
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += KGAUSS*KGAUSS*body[i].dMass/MSUN/(body[iBody].dSemi/AUCM)*( body[iBody].dpinc*(-body[iBody].dkecc*fdDdistDhPrmDir02(body, grids, iBody, i)+body[iBody].dhecc*fdDdistDkPrmDir02(body, grids, iBody, i)) + 1.0/2.0*fdDdistDqPrmDir02(body, grids, iBody, i) )/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))));
	}
      }
    }
    return sum/DAYSEC;
}


double fdLagrangeDqDt02(BODY *body, GRIDS *grids, int iBody) {
    int i;
    double sum = 0.0, dMu;
    
    for (i = 1; i < body[iBody].iNumBodies; i++) {
      if (i != iBody) { 
	dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
	if (body[iBody].dSemi < body[i].dSemi) {
	  sum += KGAUSS*KGAUSS*body[i].dMass/MSUN/(body[i].dSemi/AUCM)*( body[iBody].dqinc*(-body[iBody].dkecc*fdDdistDhDir02(body, grids, iBody, i)+body[iBody].dhecc*fdDdistDkDir02(body, grids, iBody, i)) - 1.0/2.0*fdDdistDpDir02(body, grids, iBody, i) )/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))));
	} else if (body[iBody].dSemi > body[i].dSemi) {
	  sum += KGAUSS*KGAUSS*body[i].dMass/MSUN/(body[iBody].dSemi/AUCM)*( body[iBody].dqinc*(-body[iBody].dkecc*fdDdistDhPrmDir02(body, grids, iBody, i)+body[iBody].dhecc*fdDdistDkPrmDir02(body, grids, iBody, i)) - 1.0/2.0*fdDdistDpPrmDir02(body, grids, iBody, i) )/(2*sqrt(dMu*body[iBody].dSemi/AUCM*(1-pow(body[iBody].dhecc,2)-pow(body[iBody].dkecc,2))));
	}
      }
    }
    return sum/DAYSEC;
}

double fdObliquityC01(BODY *body, GRIDS *grids, int iBody) {
  return body[iBody].dqinc*fdLagrangeDpDt01(body,grids,iBody) - body[iBody].dpinc*fdLagrangeDqDt01(body,grids,iBody);
}

double fdObliquityA01(BODY *body, GRIDS *grids, int iBody) {
  return 2.0/sqrt(1-pow(body[iBody].dpinc,2)-pow(body[iBody].dpinc,2)) * ( fdLagrangeDqDt01(body,grids,iBody) - body[iBody].dpinc*fdObliquityC01(body,grids,iBody) );
}

double fdObliquityB01(BODY *body, GRIDS *grids, int iBody) {
  return 2.0/sqrt(1-pow(body[iBody].dpinc,2)-pow(body[iBody].dpinc,2)) * ( fdLagrangeDpDt01(body,grids,iBody) - body[iBody].dqinc*fdObliquityC01(body,grids,iBody) );
}

double fdObliquityC02(BODY *body, GRIDS *grids, int iBody) {
  return body[iBody].dqinc*fdLagrangeDpDt02(body,grids,iBody) - body[iBody].dpinc*fdLagrangeDqDt02(body,grids,iBody);
}

double fdObliquityA02(BODY *body, GRIDS *grids, int iBody) {
  return 2.0/sqrt(1-pow(body[iBody].dpinc,2)-pow(body[iBody].dpinc,2)) * ( fdLagrangeDqDt02(body,grids,iBody) - body[iBody].dpinc*fdObliquityC02(body,grids,iBody) );
}

double fdObliquityB02(BODY *body, GRIDS *grids, int iBody) {
  return 2.0/sqrt(1-pow(body[iBody].dpinc,2)-pow(body[iBody].dpinc,2)) * ( fdLagrangeDpDt02(body,grids,iBody) - body[iBody].dqinc*fdObliquityC02(body,grids,iBody) );
}


double dpAdt_SolTorq(BODY *body, GRIDS *grids, int iBody) {
  return fdCentralTorqueR(body,iBody)/DAYSEC;
}

double dpAdt_R01(BODY *body, GRIDS *grids, int iBody) {
  return - 1./tan(body[iBody].dObliquity) * (fdObliquityA01(body,grids,iBody)*sin(body[iBody].dPrecA)+fdObliquityB01(body,grids,iBody)*cos(body[iBody].dPrecA)) - 2.*fdObliquityC01(body,grids,iBody);
}

double dpAdt_R02(BODY *body, GRIDS *grids, int iBody) {
  return - 1./tan(body[iBody].dObliquity) * (fdObliquityA02(body,grids,iBody)*sin(body[iBody].dPrecA)+fdObliquityB02(body,grids,iBody)*cos(body[iBody].dPrecA)) - 2.*fdObliquityC02(body,grids,iBody);
}
	
double dObldt_R01(BODY *body, GRIDS *grids, int iBody) {
  return -fdObliquityB01(body,grids,iBody)*sin(body[iBody].dPrecA) + fdObliquityA01(body,grids,iBody)*cos(body[iBody].dPrecA);
}	
	
double dObldt_R02(BODY *body, GRIDS *grids, int iBody) {
  return -fdObliquityB02(body,grids,iBody)*sin(body[iBody].dPrecA) + fdObliquityA02(body,grids,iBody)*cos(body[iBody].dPrecA);
}		
	

	
//-------------------------------------------------------------------------------------------------------------
