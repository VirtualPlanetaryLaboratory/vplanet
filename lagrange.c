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

void InitializeControlLagrange(CONTROL *control) {
  /* Not sure if I need anything here yet */
}

void InitializeModuleLagrange(CONTROL *control,MODULE *module) {
  /* Anything here? */
}

void BodyCopyLagrange(BODY *dest,BODY *src,int iBody) {
  int iIndex,iPert;

  dest[iBody].dhecc = src[iBody].dhecc;
  dest[iBody].dkecc = src[iBody].dkecc;
  dest[iBody].dpinc = src[iBody].dpinc;
  dest[iBody].dqinc = src[iBody].dqinc;

  for (iPert=0;iPert<src[iBody].iGravPerts;iPert++)
    dest[iBody].iaGravPerts[iPert] = src[iBody].iaGravPerts[iPert];
}

void InitializeBodyLagrange(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
  body[iBody].iaGravPerts = malloc(body[iBody].iGravPerts*sizeof(int));
}

void InitializeUpdateTmpBodyLagrange(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
  int iBodyPert;
  
  control->Evolve.tmpBody[iBody].iaGravPerts = malloc(body[iBody].iGravPerts);

}

/**************** LAGRANGE options ********************/

/* Inclination */

void ReadInc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,180].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    //body[iFile-1].dInc = dTmp; 
    body[iFile-1].dsInc = sin(0.5*dTmp);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dInc = options->dDefault;
}  

/* Longitude of ascending node */

void ReadLongA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile-1].dLongA = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dLongA = options->dDefault;
}  


/* Longitude of pericenter */

void ReadLongP(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    
    body[iFile-1].dLongP = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dLongP = options->dDefault;
}  


/* Argument of pericenter */

void ReadArgP(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    
    body[iFile-1].dArgP = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dArgP = options->dDefault;
}  

/* Mean anomaly */

void ReadMeanA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iFile-1].dMeanA = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dMeanA = options->dDefault;
} 


/* Mean longitude */

void ReadMeanL(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iFile-1].dMeanL = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dMeanL = options->dDefault;
}  

/* True longitude */

void ReadTrueL(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }  
    
    body[iFile-1].dTrueL = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dTrueL = options->dDefault;
}  

/* True anomaly */

void ReadTrueA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iFile-1].dTrueA = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dTrueA = options->dDefault;
}  

/* Eccentric anomaly */

void ReadEccA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iFile-1].dEccA = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dEccA = options->dDefault;
}  

void ReadPrecAngle(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iFile-1].dPrecA = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dPrecA = options->dDefault;
}  


void ReadDynEllip(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* Cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,options->iLine[iFile],control->iVerbose);
    if (dTmp < 0 || dTmp >= 1) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be in the range [0,1).\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);	
    }
    body[iFile-1].dDynEllip = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultDouble(&body[iFile-1].dDynEllip,*options,files->iNumInputs);
    
}

void ReadObliqEvol(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,options->iLine[iFile],control->iVerbose);
    body[iFile-1].bObliqEvol = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    /* Set to default */
    AssignDefaultInt(&body[iFile-1].bObliqEvol,*options,files->iNumInputs);
}

void InitializeOptionsLagrange(OPTIONS *options,fnReadOption fnRead[]) {
  
  sprintf(options[OPT_INC].cName,"dInc");
  sprintf(options[OPT_INC].cDescr,"Inclination of planet's orbital plane");
  sprintf(options[OPT_INC].cDefault,"0");
  options[OPT_INC].dDefault = 0.0;
  options[OPT_INC].iType = 2;  
  options[OPT_INC].iMultiFile = 1;  
  fnRead[OPT_INC] = &ReadInc;
  
  sprintf(options[OPT_LONGA].cName,"dLongA");
  sprintf(options[OPT_LONGA].cDescr,"Longitude of ascending node of planet's orbital plane");
  sprintf(options[OPT_LONGA].cDefault,"0");
  options[OPT_LONGA].dDefault = 0.0;
  options[OPT_LONGA].iType = 2;  
  options[OPT_LONGA].iMultiFile = 1; 
  fnRead[OPT_LONGA] = &ReadLongA;
  
  sprintf(options[OPT_LONGP].cName,"dLongP");
  sprintf(options[OPT_LONGP].cDescr,"Longitude of pericenter of planet's orbit");
  sprintf(options[OPT_LONGP].cDefault,"0");
  options[OPT_LONGP].dDefault = 0.0;
  options[OPT_LONGP].iType = 2;  
  options[OPT_LONGP].iMultiFile = 1;   
  fnRead[OPT_LONGP] = &ReadLongP;
  
  sprintf(options[OPT_ARGP].cName,"dArgP");
  sprintf(options[OPT_ARGP].cDescr,"Argument of pericenter of planet's orbit");
  sprintf(options[OPT_ARGP].cDefault,"0");
  options[OPT_ARGP].dDefault = 0.0;
  options[OPT_ARGP].iType = 2;  
  options[OPT_ARGP].iMultiFile = 1;   
  fnRead[OPT_ARGP] = &ReadArgP;
  
  sprintf(options[OPT_MEANA].cName,"dMeanA");
  sprintf(options[OPT_MEANA].cDescr,"Mean anomaly of planet");
  sprintf(options[OPT_MEANA].cDefault,"0");
  options[OPT_MEANA].dDefault = 0.0;
  options[OPT_MEANA].iType = 2;  
  options[OPT_MEANA].iMultiFile = 1;   
  fnRead[OPT_MEANA] = &ReadMeanA;
  
  sprintf(options[OPT_MEANL].cName,"dMeanL");
  sprintf(options[OPT_MEANL].cDescr,"Mean longitude of planet");
  sprintf(options[OPT_MEANL].cDefault,"0");
  options[OPT_MEANL].dDefault = 0.0;
  options[OPT_MEANL].iType = 2;  
  options[OPT_MEANL].iMultiFile = 1;   
  fnRead[OPT_MEANL] = &ReadMeanL;
 
  sprintf(options[OPT_TRUEL].cName,"dTrueL");
  sprintf(options[OPT_TRUEL].cDescr,"True longitude of planet");
  sprintf(options[OPT_TRUEL].cDefault,"0");
  options[OPT_TRUEL].dDefault = 0.0;
  options[OPT_TRUEL].iType = 2;  
  options[OPT_TRUEL].iMultiFile = 1;   
  fnRead[OPT_TRUEL] = &ReadTrueL;
  
  sprintf(options[OPT_TRUEA].cName,"dTrueA");
  sprintf(options[OPT_TRUEA].cDescr,"True anomaly of planet");
  sprintf(options[OPT_TRUEA].cDefault,"0");
  options[OPT_TRUEA].dDefault = 0.0;
  options[OPT_TRUEA].iType = 2;  
  options[OPT_TRUEA].iMultiFile = 1;  
  fnRead[OPT_TRUEA] = &ReadTrueA;
  
  sprintf(options[OPT_ECCA].cName,"dEccA");
  sprintf(options[OPT_ECCA].cDescr,"Eccentric anomaly of planet");
  sprintf(options[OPT_ECCA].cDefault,"0");
  options[OPT_ECCA].dDefault = 0.0;
  options[OPT_ECCA].iType = 2;  
  options[OPT_ECCA].iMultiFile = 1;  
  fnRead[OPT_ECCA] = &ReadEccA;*/
  
  sprintf(options[OPT_OBLEVOL].cName,"bObliqEvol");
  sprintf(options[OPT_OBLEVOL].cDescr,"Calculate obliquity evolution");
  sprintf(options[OPT_OBLEVOL].cDefault,"0");
  options[OPT_OBLEVOL].iType = 0; 
  options[OPT_OBLEVOL].iMultiFile = 1;
  fnRead[OPT_OBLEVOL] = &ReadObliqEvol; 
}

void ReadOptionsLagrange(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTLAGRANGE;iOpt<OPTENDLAGRANGE;iOpt++) 
      if (options[iOpt].iType != -1) {
	fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
      }
}
    

/******************* Verify LAGRANGE ******************/
void InitializeLagrange(BODY *body,CONTROL *control,UPDATE *update) {
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

void VerifyLagrange(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  
    
  VerifyPericenter(control,&body[iBody],options,iBody+1,files->Infile[iBody+1].cIn,control->iVerbose);
  
  /* Setup Semi-major axis functions (LaplaceF) for secular terms*/
  system->fnLaplaceF = malloc(LAPLNUM*sizeof(fnLaplaceFunction*));
  for (j=0;j<LAPLNUM;j++) {
    system->fnLaplaceF[j] = malloc(1*sizeof(fnLaplaceFunction));
  }
  system->fnLaplaceF[0][0] = &fdSemiMajAxF1;
  system->fnLaplaceF[1][0] = &fdSemiMajAxF2;
  system->fnLaplaceF[2][0] = &fdSemiMajAxF3;
  system->fnLaplaceF[3][0] = &fdSemiMajAxF4;
  system->fnLaplaceF[4][0] = &fdSemiMajAxF5;
  system->fnLaplaceF[5][0] = &fdSemiMajAxF6;
  system->fnLaplaceF[6][0] = &fdSemiMajAxF7;
  system->fnLaplaceF[7][0] = &fdSemiMajAxF8;
  system->fnLaplaceF[8][0] = &fdSemiMajAxF9;
  system->fnLaplaceF[9][0] = &fdSemiMajAxF10;
  system->fnLaplaceF[10][0] = &fdSemiMajAxF11;
  system->fnLaplaceF[11][0] = &fdSemiMajAxF12;
  system->fnLaplaceF[12][0] = &fdSemiMajAxF13;
  system->fnLaplaceF[13][0] = &fdSemiMajAxF14;
  system->fnLaplaceF[14][0] = &fdSemiMajAxF15;
  system->fnLaplaceF[15][0] = &fdSemiMajAxF16;
  system->fnLaplaceF[16][0] = &fdSemiMajAxF17;
  system->fnLaplaceF[17][0] = &fdSemiMajAxF18;
  system->fnLaplaceF[18][0] = &fdSemiMajAxF19;
  system->fnLaplaceF[19][0] = &fdSemiMajAxF20;
  system->fnLaplaceF[20][0] = &fdSemiMajAxF21;
  system->fnLaplaceF[21][0] = &fdSemiMajAxF22;
  system->fnLaplaceF[22][0] = &fdSemiMajAxF23;
  system->fnLaplaceF[23][0] = &fdSemiMajAxF24;
  system->fnLaplaceF[24][0] = &fdSemiMajAxF25;
  system->fnLaplaceF[25][0] = &fdSemiMajAxF26;
  
  system->dmLaplaceC = malloc(Nchoosek(body[0].iNumBodies-1,2)*sizeof(double*));
  for (i=0;i<Nchoosek(body[0].iNumBodies-1,2);i++) {
    system->dmLaplaceC[i] = malloc(LAPLNUM*sizeof(double));  
  }
  
  system->imLaplaceN = malloc((body[0].iNumBodies)*sizeof(int*));
  for (i=1;i<body[0].iNumBodies;i++) {
    system->imLaplaceN[i] = malloc((body[0].iNumBodies)*sizeof(int));
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
	    system->imLaplaceN[iBody][jBody] = CombCount(iBody,jBody,body[0].iNumBodies-1);
	    system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceF[j][0](body[iBody].dSemi/body[jBody].dSemi, 0);
	  } else if (body[iBody].dSemi > body[jBody].dSemi) {
	    system->imLaplaceN[iBody][jBody] = CombCount(iBody,jBody,body[0].iNumBodies-1);
	    system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][j] = system->fnLaplaceF[j][0](body[jBody].dSemi/body[iBody].dSemi, 0);
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
	    system->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][j] = system->fnLaplaceF[j][0](body[iBody].dSemi/body[jBody].dSemi, 0);
	  } else if (body[iBody].dSemi > body[jBody].dSemi) {
	    system->dmLaplaceC[CombCount(iBody,jBody,body[0].iNumBodies-1)][j] = system->fnLaplaceF[j][0](body[jBody].dSemi/body[iBody].dSemi, 0);
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


void ForceBehaviorLagrange(BODY *body,CONTROL *control,SYSTEM *system,double dDt) {
  int iBody;
  double minsInc = 1.e-7, theta = 15.0;
 
  for (iBody=1; iBody<control->iNumBodies; iBody++) {
    if (body[iBody].dsInc < minsInc) {
      fvPropertiesLagrange(control,body,system,dDt); 
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

void WriteBodyDeccDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDeccDt(body, system, iBody)*365.25*86400.;
  }
}

void WriteBodyDsIncDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDsIncDt(body, system, iBody)*365.25*86400.;
  }
}  
  
void WriteBodyDvarDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDvarDt(body, system, iBody)*365.25*86400.;
  }
}  

void WriteBodyDOmgDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDOmgDt(body, system, iBody)*365.25*86400.;
  }
} 

void WriteBodyDOblDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDOblDt(body, system, iBody)*365.25*86400.;
  }
} 

void WriteBodysInc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dsInc;
}  

void WriteBodyInc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  body[iBody].dInc = 2.*asin(body[iBody].dsInc);  
  *dTmp = body[iBody].dInc / DEGRAD;
}  

void WriteBodyLongA(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dLongA / DEGRAD;
}  

void WriteBodyLongP(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dLongP / DEGRAD;
}  

void WriteBodyArgP(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  body[iBody].dArgP = body[iBody].dLongP - body[iBody].dLongA;
  
  while (body[iBody].dArgP < 0.0) {
    body[iBody].dArgP += 2*PI;
  }
  
  *dTmp = body[iBody].dArgP / DEGRAD;
}    

void WriteBodyMeanA(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  body[iBody].dMeanA = body[iBody].dMeanL - body[iBody].dLongP;
 
  while (body[iBody].dMeanA < 0.0) {
    body[iBody].dMeanA += 2*PI;
  }
  
  *dTmp = body[iBody].dMeanA / DEGRAD;
}    

void WriteBodyMeanL(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dMeanL / DEGRAD;
}    

void WriteBodyTrueA(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dTrueA / DEGRAD;
}   

void WriteBodyTrueL(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dTrueL / DEGRAD;
}   

void WriteBodyEccA(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dEccA / DEGRAD;
}   

void WriteBodyPrecA(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
 
  while (body[iBody].dPrecA < 0.0) {
    body[iBody].dPrecA += 2*PI;
  }
  
  while (body[iBody].dPrecA > 2*PI) {
    body[iBody].dPrecA -= 2*PI;
  }
  
  *dTmp = body[iBody].dPrecA / DEGRAD;
}    

void WriteShadowObl(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM *system,int iBody,double *dTmp,char cUnit[]) {

  *dTmp = body[iBody].dShadowObl;
  if (output.bDoNeg[iBody]) {
    *dTmp *= output.dNeg;
    strcpy(cUnit,output.cNeg);
  } else {
    *dTmp /= fdAngleUnit(units->iAngle);
    fvAngleUnit(units->iAngle,cUnit);
  }
}

void WriteShadowPrecA(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
 
  while (body[iBody].dShadowPrecA < 0.0) {
    body[iBody].dShadowPrecA += 2*PI;
  }
  
  while (body[iBody].dShadowPrecA > 2*PI) {
    body[iBody].dShadowPrecA -= 2*PI;
  }
  
  *dTmp = body[iBody].dShadowPrecA / DEGRAD;
}    

void WriteBodyhecc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dhecc;
}  

void WriteBodykecc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dkecc;
} 

void WriteBodypinc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dpinc;
} 

void WriteBodyqinc(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  
  *dTmp = body[iBody].dqinc;
} 

void WriteBodyDheccDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDhDt(body, system, iBody)*365.25*86400.;
  }
}

void WriteBodyDkeccDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDkDt(body, system, iBody)*365.25*86400.;
  }
}

void WriteBodyDpincDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDpDt(body, system, iBody)*365.25*86400.;
  }
}

void WriteBodyDqincDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDqDt(body, system, iBody)*365.25*86400.;
  }
}

void WriteBodyDbetaDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDbetaDt(body, system, iBody)*365.25*86400.;
  }
}

void WriteBodyDgammaDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDgammaDt(body, system, iBody)*365.25*86400.;
  }
}

void WriteBodyDetaDtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDetaDt(body, system, iBody)*365.25*86400.;
  }
}

void WriteBodyDPrecADtLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdLagrangeDPrecADt(body, system, iBody)*365.25*86400.;
  }
}

void WriteBodyCenTorqLagrange(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = fdCentralTorqueR(body, iBody)*365.25;
  }
}

void WriteDpADt_SolTorq(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = dpAdt_SolTorq(body, system, iBody)*365.25*86400.;
  }
}

void WriteDpADt_R01(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = dpAdt_R01(body, system, iBody)*365.25*86400.;
  }
}

void WriteDpADt_R02(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = dpAdt_R02(body, system, iBody)*365.25*86400.;
  }
}

void WriteDOblDt_R01(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = dObldt_R01(body, system, iBody)*365.25*86400.;
  }
}

void WriteDOblDt_R02(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = dObldt_R02(body, system, iBody)*365.25*86400.;
  }
}

void WriteC1_2term(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  //Temporary hack to give this constant for planet and its next exterior companion (2 term disturbing fxn analysis) 
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = body[iBody].dEcc*body[iBody+1].dEcc*system->dmLaplaceC[system->imLaplaceN[iBody][iBody+1]][9] + pow(body[iBody].dEcc,3)*body[iBody+1].dEcc*system->dmLaplaceC[system->imLaplaceN[iBody][iBody+1]][10] + pow(body[iBody+1].dEcc,3)*body[iBody].dEcc*system->dmLaplaceC[system->imLaplaceN[iBody][iBody+1]][11] + body[iBody].dEcc*body[iBody+1].dEcc*(pow(body[iBody].dsInc,2) + pow(body[iBody+1].dsInc,2)) * system->dmLaplaceC[system->imLaplaceN[iBody][iBody+1]][12];
  }
}

void WriteC2_2term(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  //Temporary hack to give this constant for planet and its next exterior companion (2 term, 2 planet disturbing fxn analysis) 
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = body[iBody].dsInc*system->dmLaplaceC[system->imLaplaceN[iBody][iBody+1]][2] + (pow(body[iBody].dEcc,2)*body[iBody].dsInc+pow(body[iBody+1].dEcc,2)*body[iBody].dsInc) * system->dmLaplaceC[system->imLaplaceN[iBody][iBody+1]][6] + 2.*pow(body[iBody].dsInc,3) * system->dmLaplaceC[system->imLaplaceN[iBody][iBody+1]][7] + body[iBody].dsInc*pow(body[iBody+1].dsInc,2) * system->dmLaplaceC[system->imLaplaceN[iBody][iBody+1]][8];
  }
}
  
void WriteC3_2term(CONTROL *control,UNITS *units,OUTPUT output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM* system,int iBody,double *dTmp,char cUnit[]) {
  //Temporary hack to give this constant for planet and its next exterior companion (2 term, 2 planet disturbing fxn analysis) 
  if (iBody == 0) {
    *dTmp = 0.0;
  } else {
    *dTmp = body[iBody].dEcc*body[iBody+1].dEcc*body[iBody].dsInc * system->dmLaplaceC[system->imLaplaceN[iBody][iBody+1]][12];
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

void LogLagrange(CONTROL *control,OUTPUT *output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM *system,fnWriteOutput fnWrite[],FILE *fp) {
  int iOut;

  fprintf(fp,"\n----- LAGRANGE PARAMETERS ------\n");
  for (iOut=OUTSTARTLAGRANGE;iOut<OUTBODYSTARTLAGRANGE;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,system,0);
  }
}

void LogBodyLagrange(CONTROL *control,OUTPUT *output,BODY *body,SYSTEM *system,UPDATE *update,SYSTEM *system,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- LAGRANGE PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTBODYSTARTLAGRANGE;iOut<OUTENDLAGRANGE;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,system,iBody);
  }
}

void fvPropertiesLagrange(CONTROL *control,BODY *body,SYSTEM *system,double dDt) {
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


//----------------Disturbing function h k p q----------------------------------------------
//--------dR/dh-----------(inner body)------------------------------------------------------
double fdDdistDhDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dhecc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][1] + 2*(pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][3] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][4] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][6] );
}

double fdDdistDhDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dhecc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][9] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][11] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][12] ) + (2*body[iBody].dhecc*body[iBody].dkecc*body[jBody].dkecc + 3*pow(body[iBody].dhecc,2)*body[jBody].dhecc+body[jBody].dhecc*pow(body[iBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][10];
} 
 
double fdDdistDhDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dhecc*(body[iBody].dpinc*body[jBody].dpinc+body[iBody].dqinc*body[jBody].dqinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][14];
}

double fdDdistDhDir04(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2*body[iBody].dhecc*(pow(body[jBody].dhecc,2)-pow(body[jBody].dkecc,2))+4*body[jBody].dhecc*body[iBody].dkecc*body[jBody].dkecc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][16];
}

double fdDdistDhDir05(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2*body[iBody].dhecc*(pow(body[iBody].dpinc,2)-pow(body[iBody].dqinc,2))+4*body[iBody].dpinc*body[iBody].dqinc*body[iBody].dkecc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDhDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (body[jBody].dhecc*(pow(body[iBody].dpinc,2)-pow(body[iBody].dqinc,2))+2*body[jBody].dkecc*body[iBody].dpinc*body[iBody].dqinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][18];
}

double fdDdistDhDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*(body[iBody].dhecc*(body[iBody].dpinc*body[jBody].dpinc-body[iBody].dqinc*body[jBody].dqinc) + body[iBody].dkecc*(body[jBody].dpinc*body[iBody].dqinc+body[jBody].dqinc*body[iBody].dpinc))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][20];
}

double fdDdistDhDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dhecc*(body[iBody].dqinc*body[jBody].dqinc+body[iBody].dpinc*body[jBody].dpinc) + body[jBody].dkecc*(body[iBody].dpinc*body[jBody].dqinc-body[iBody].dqinc*body[jBody].dpinc) )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][21];
}

double fdDdistDhDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dhecc*(body[iBody].dqinc*body[jBody].dqinc+body[iBody].dpinc*body[jBody].dpinc) + body[jBody].dkecc*(body[iBody].dqinc*body[jBody].dpinc-body[iBody].dpinc*body[jBody].dqinc) )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][22];
}

double fdDdistDhDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dhecc*(body[iBody].dpinc*body[jBody].dpinc-body[iBody].dqinc*body[jBody].dqinc) + body[jBody].dkecc*(body[iBody].dqinc*body[jBody].dpinc+body[iBody].dpinc*body[jBody].dqinc) )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][23];
}
  
double fdDdistDhDir013(BODY *body, SYSTEM *system, int iBody, int jBody) {  
  return ( 2*body[iBody].dhecc*(pow(body[jBody].dpinc,2)-pow(body[jBody].dqinc,2)) + 4*body[jBody].dpinc*body[jBody].dqinc*body[iBody].dkecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][17];
}
  
double fdDdistDhDir014(BODY *body, SYSTEM *system, int iBody, int jBody) { 
  return ( body[jBody].dhecc*(pow(body[jBody].dpinc,2)-pow(body[jBody].dqinc,2)) + 2*body[jBody].dkecc*body[jBody].dpinc*body[jBody].dqinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][18];
}
  
double fdDdisturbDhecc(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDhDir01(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir04(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir05(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir06(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir08(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir013(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhDir014(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//---------dR/dh'------------------------------------------------------------------

double fdDdistDhPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dhecc*(system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][1] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][4] + 2*(pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][5] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2) + pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][6]);
}

double fdDdistDhPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dhecc*(system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][9] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][10] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)+pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][12]) + (2*body[iBody].dhecc*body[jBody].dkecc*body[iBody].dkecc + 3*pow(body[iBody].dhecc,2)*body[jBody].dhecc + body[jBody].dhecc*pow(body[iBody].dkecc,2)) * system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][11];
}

double fdDdistDhPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dhecc*(body[iBody].dpinc*body[jBody].dpinc+body[iBody].dqinc*body[jBody].dqinc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][14];
}

double fdDdistDhPrmDir04(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return (2*body[iBody].dhecc*(pow(body[jBody].dhecc,2)-pow(body[jBody].dkecc,2)) + 4*body[jBody].dhecc*body[jBody].dkecc*body[iBody].dkecc) * system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][16];
}

double fdDdistDhPrmDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dhecc*(pow(body[jBody].dpinc,2)-pow(body[jBody].dqinc,2)) + 2*body[jBody].dkecc*body[jBody].dpinc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDhPrmDir07(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( 2*body[iBody].dhecc*(pow(body[jBody].dpinc,2)-pow(body[jBody].dqinc,2)) + 4*body[jBody].dpinc*body[jBody].dqinc*body[iBody].dkecc ) * system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][19];
}

double fdDdistDhPrmDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dhecc*(body[iBody].dqinc*body[jBody].dqinc+body[iBody].dpinc*body[jBody].dpinc) + body[jBody].dkecc*(body[iBody].dpinc*body[jBody].dqinc-body[iBody].dqinc*body[jBody].dpinc) ) * system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][21];
}

double fdDdistDhPrmDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dhecc*(body[iBody].dqinc*body[jBody].dqinc+body[iBody].dpinc*body[jBody].dpinc) + body[jBody].dkecc*(body[iBody].dqinc*body[jBody].dpinc-body[iBody].dpinc*body[jBody].dqinc) ) * system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][22];
}

double fdDdistDhPrmDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dhecc*(-body[iBody].dqinc*body[jBody].dqinc+body[iBody].dpinc*body[jBody].dpinc) + body[jBody].dkecc*(body[iBody].dqinc*body[jBody].dpinc+body[iBody].dpinc*body[jBody].dqinc) )* system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][23];
}

double fdDdistDhPrmDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dhecc*(body[iBody].dpinc*body[jBody].dpinc-body[iBody].dqinc*body[jBody].dqinc) + body[iBody].dkecc*(body[iBody].dpinc*body[jBody].dqinc+body[iBody].dqinc*body[jBody].dpinc) )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][24];
}

double fdDdistDhPrmDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dhecc*(pow(body[iBody].dpinc,2)-pow(body[iBody].dqinc,2)) + 2*body[jBody].dkecc*body[iBody].dpinc*body[iBody].dqinc ) *system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDhPrmDir015(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dhecc*(pow(body[iBody].dpinc,2)-pow(body[iBody].dqinc,2)) + 2*body[iBody].dpinc*body[iBody].dqinc*body[iBody].dkecc ) * system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][19];
}

double fdDdisturbDheccPrime(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDhPrmDir01(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir04(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir06(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir07(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir012(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir014(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDhPrmDir015(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//------------dR/dk--------------------------------------------------------------

double fdDdistDkDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dkecc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][1] + 2*(pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][3] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][4] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2) + pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][6] );
}

double fdDdistDkDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dkecc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][9] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][11] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][12] ) + (3*pow(body[iBody].dkecc,2)*body[jBody].dkecc+body[jBody].dkecc*pow(body[iBody].dhecc,2)+2*body[iBody].dkecc*body[iBody].dhecc*body[jBody].dhecc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][10];
}

double fdDdistDkDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dkecc*(body[iBody].dpinc*body[jBody].dpinc+body[iBody].dqinc*body[jBody].dqinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][14];
}

double fdDdistDkDir04(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( 2*body[iBody].dkecc*(pow(body[jBody].dkecc,2)-pow(body[jBody].dhecc,2)) + 4*body[iBody].dhecc*body[jBody].dhecc*body[jBody].dkecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][16];
}

double fdDdistDkDir05(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( 2*body[iBody].dkecc*(pow(body[iBody].dqinc,2)-pow(body[iBody].dpinc,2)) + 4*body[iBody].dpinc*body[iBody].dqinc*body[iBody].dhecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDkDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*(pow(body[iBody].dqinc,2)-pow(body[iBody].dpinc,2)) + 2*body[jBody].dhecc*body[iBody].dpinc*body[iBody].dqinc )* system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][18];
}

double fdDdistDkDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dkecc*(body[jBody].dqinc*body[iBody].dqinc - body[jBody].dpinc*body[iBody].dpinc) + body[iBody].dhecc*(body[jBody].dpinc*body[iBody].dqinc + body[jBody].dqinc*body[iBody].dpinc) ) * system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][20];
}

double fdDdistDkDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*(body[jBody].dqinc*body[iBody].dqinc + body[jBody].dpinc*body[iBody].dpinc) + body[jBody].dhecc*(body[jBody].dpinc*body[iBody].dqinc - body[jBody].dqinc*body[iBody].dpinc) ) * system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][21];
}

double fdDdistDkDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*(body[jBody].dqinc*body[iBody].dqinc + body[jBody].dpinc*body[iBody].dpinc) + body[jBody].dhecc*(body[jBody].dqinc*body[iBody].dpinc - body[jBody].dpinc*body[iBody].dqinc) ) * system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][22];
}

double fdDdistDkDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*(body[jBody].dqinc*body[iBody].dqinc-body[jBody].dpinc*body[iBody].dpinc) + body[jBody].dhecc*(body[jBody].dpinc*body[iBody].dqinc+body[jBody].dqinc*body[iBody].dpinc) ) * system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][23];
}

double fdDdistDkDir013(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( 2*body[iBody].dkecc*(pow(body[jBody].dqinc,2)-pow(body[jBody].dpinc,2)) + 4*body[jBody].dpinc*body[jBody].dqinc*body[iBody].dhecc ) * system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDkDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*(pow(body[jBody].dqinc,2)-pow(body[jBody].dpinc,2)) + 2*body[jBody].dhecc*body[jBody].dpinc*body[jBody].dqinc ) * system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][18];
}

double fdDdisturbDkecc(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDkDir01(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir04(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir05(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir06(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir08(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir013(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkDir014(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//----------dR/dk'------------------------------------------------------------
double fdDdistDkPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dkecc*(system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][1] + (pow(body[jBody].dhecc,2) + pow(body[jBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][4] + 2*(pow(body[iBody].dhecc,2) + pow(body[iBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][5] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2) + pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][6]);
}

double fdDdistDkPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dkecc*(system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][9] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][10] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2) + pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][12]) + (pow(body[iBody].dhecc,2)*body[jBody].dkecc+3*body[jBody].dkecc*pow(body[iBody].dkecc,2)+2*body[iBody].dkecc*body[jBody].dhecc*body[iBody].dhecc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][11];
}

double fdDdistDkPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dkecc*(body[jBody].dpinc*body[iBody].dpinc+body[iBody].dqinc*body[jBody].dqinc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][14];
}

double fdDdistDkPrmDir04(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dkecc*(pow(body[jBody].dkecc,2)-pow(body[jBody].dhecc,2)) + 2*body[jBody].dhecc*body[iBody].dhecc*body[jBody].dkecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][16];
}

double fdDdistDkPrmDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*(pow(body[jBody].dqinc,2) - pow(body[jBody].dpinc,2)) + 2*body[jBody].dhecc*body[jBody].dpinc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDkPrmDir07(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dkecc*(pow(body[jBody].dqinc,2)-pow(body[jBody].dpinc,2)) + 2*body[jBody].dpinc*body[jBody].dqinc*body[iBody].dhecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][19];
}

double fdDdistDkPrmDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*(body[iBody].dqinc*body[jBody].dqinc+body[iBody].dpinc*body[jBody].dpinc) - body[jBody].dhecc*(body[iBody].dpinc*body[jBody].dqinc-body[iBody].dqinc*body[jBody].dpinc) )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][21];
}

double fdDdistDkPrmDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*(body[iBody].dqinc*body[jBody].dqinc + body[iBody].dpinc*body[jBody].dpinc) - body[jBody].dhecc*(body[iBody].dqinc*body[jBody].dpinc-body[iBody].dpinc*body[jBody].dqinc) )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][22];
}

double fdDdistDkPrmDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*(body[jBody].dqinc*body[iBody].dqinc-body[jBody].dpinc*body[iBody].dpinc) + body[jBody].dhecc*(body[iBody].dpinc*body[jBody].dqinc+body[iBody].dqinc*body[jBody].dpinc) )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][23];
}

double fdDdistDkPrmDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dkecc*(body[iBody].dqinc*body[jBody].dqinc-body[iBody].dpinc*body[jBody].dpinc) + body[iBody].dhecc*(body[iBody].dpinc*body[jBody].dqinc+body[iBody].dqinc*body[jBody].dpinc) )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][24];
}

double fdDdistDkPrmDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*(pow(body[iBody].dqinc,2)-pow(body[iBody].dpinc,2)) + 2*body[jBody].dhecc*body[iBody].dpinc*body[iBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDkPrmDir015(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dkecc*(pow(body[iBody].dqinc,2)-pow(body[iBody].dpinc,2)) + 2*body[iBody].dpinc*body[iBody].dqinc*body[iBody].dhecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][19];
}

double fdDdisturbDkeccPrime(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDkPrmDir01(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir04(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir06(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir07(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir012(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir014(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDkPrmDir015(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//------------dR/dp---------------------------------------------------------//
double fdDdistDpDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dpinc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][2] + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)+pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][6] + 2*(pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][7] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][8] );
}

double fdDdistDpDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dpinc*(body[iBody].dhecc*body[jBody].dhecc + body[iBody].dkecc*body[jBody].dkecc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][12];
}

double fdDdistDpDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dpinc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][13] + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)+pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][14] + (pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2)+pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][15] ) + 2*body[iBody].dpinc*(body[iBody].dpinc*body[jBody].dpinc+body[iBody].dqinc*body[jBody].dqinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][15];
}

double fdDdistDpDir05(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dpinc*pow(body[iBody].dhecc,2)-body[iBody].dpinc*pow(body[iBody].dkecc,2)+2*body[iBody].dqinc*body[iBody].dhecc*body[iBody].dkecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDpDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dhecc*body[jBody].dhecc*body[iBody].dpinc - body[iBody].dkecc*body[jBody].dkecc*body[iBody].dpinc + body[jBody].dhecc*body[iBody].dkecc*body[iBody].dqinc + body[jBody].dkecc*body[iBody].dhecc*body[iBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][18];
}

double fdDdistDpDir07(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*(body[iBody].dpinc*pow(body[jBody].dhecc,2) - body[iBody].dpinc*pow(body[jBody].dkecc,2) + 2*body[iBody].dqinc*body[jBody].dhecc*body[jBody].dkecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][19];
}

double fdDdistDpDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dpinc*pow(body[iBody].dhecc,2)-body[jBody].dpinc*pow(body[iBody].dkecc,2)+2*body[iBody].dhecc*body[iBody].dkecc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][20];
}

double fdDdistDpDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*body[iBody].dkecc*body[jBody].dpinc + body[jBody].dhecc*body[iBody].dhecc*body[jBody].dpinc - body[jBody].dhecc*body[iBody].dkecc*body[jBody].dqinc + body[jBody].dkecc*body[iBody].dhecc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][21];
}

double fdDdistDpDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*body[iBody].dkecc*body[jBody].dpinc + body[jBody].dhecc*body[iBody].dhecc*body[jBody].dpinc + body[jBody].dhecc*body[iBody].dkecc*body[jBody].dqinc - body[jBody].dkecc*body[iBody].dhecc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][22];
}

double fdDdistDpDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( -body[jBody].dkecc*body[iBody].dkecc*body[jBody].dpinc + body[jBody].dhecc*body[iBody].dhecc*body[jBody].dpinc + body[jBody].dhecc*body[iBody].dkecc*body[jBody].dqinc + body[jBody].dkecc*body[iBody].dhecc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][23];
}

double fdDdistDpDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( -pow(body[jBody].dkecc,2)*body[jBody].dpinc + pow(body[jBody].dhecc,2)*body[jBody].dpinc + 2*body[jBody].dhecc*body[jBody].dkecc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][24];
}

double fdDdistDpDir016(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*(body[iBody].dpinc*pow(body[jBody].dpinc,2)-body[iBody].dpinc*pow(body[jBody].dqinc,2)+2*body[jBody].dpinc*body[jBody].dqinc*body[iBody].dqinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][25];
}

double fdDdisturbDpinc(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDpDir01(body, system, iaBody[0], iaBody[1]) \
     + fdDdistDpDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir05(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir06(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir07(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir08(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir012(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpDir016(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//-------dR/dp'------------------------------------------------------------//
double fdDdistDpPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dpinc*( system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][2] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2)+pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][6] + 2*(pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][7] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][8] );
}

double fdDdistDpPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dpinc*(body[jBody].dhecc*body[iBody].dhecc+body[jBody].dkecc*body[iBody].dkecc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][12];
}

double fdDdistDpPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dpinc*( system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][13] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2)+pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][14] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)+pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][15] ) + 2*body[iBody].dpinc*(body[jBody].dpinc*body[iBody].dpinc+body[jBody].dqinc*body[iBody].dqinc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][15];
}

double fdDdistDpPrmDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( -pow(body[jBody].dkecc,2)*body[jBody].dpinc+pow(body[jBody].dhecc,2)*body[jBody].dpinc+2*body[jBody].dhecc*body[jBody].dkecc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][20];
}

double fdDdistDpPrmDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[iBody].dkecc*body[jBody].dkecc*body[jBody].dpinc + body[iBody].dhecc*body[jBody].dhecc*body[jBody].dpinc + body[iBody].dhecc*body[jBody].dkecc*body[jBody].dqinc - body[iBody].dkecc*body[jBody].dhecc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][21];
}

double fdDdistDpPrmDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[iBody].dkecc*body[jBody].dkecc*body[jBody].dpinc + body[iBody].dhecc*body[jBody].dhecc*body[jBody].dpinc - body[iBody].dhecc*body[jBody].dkecc*body[jBody].dqinc + body[iBody].dkecc*body[jBody].dhecc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][22];
}

double fdDdistDpPrmDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( -body[iBody].dkecc*body[jBody].dkecc*body[jBody].dpinc + body[iBody].dhecc*body[jBody].dhecc*body[jBody].dpinc + body[iBody].dhecc*body[jBody].dkecc*body[jBody].dqinc + body[iBody].dkecc*body[jBody].dhecc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][23];
}

double fdDdistDpPrmDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( -pow(body[iBody].dkecc,2)*body[jBody].dpinc + pow(body[iBody].dhecc,2)*body[jBody].dpinc + 2*body[iBody].dhecc*body[iBody].dkecc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][24];
}

double fdDdistDpPrmDir013(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( -body[iBody].dpinc*pow(body[jBody].dkecc,2) + body[iBody].dpinc*pow(body[jBody].dhecc,2) + 2*body[iBody].dqinc*body[jBody].dhecc*body[jBody].dkecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][17];
}

double fdDdistDpPrmDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( -body[iBody].dkecc*body[jBody].dkecc*body[iBody].dpinc + body[iBody].dhecc*body[jBody].dhecc*body[iBody].dpinc + body[iBody].dhecc*body[jBody].dkecc*body[iBody].dqinc + body[iBody].dkecc*body[jBody].dhecc*body[iBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDpPrmDir015(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( -body[iBody].dpinc*pow(body[iBody].dkecc,2) + body[iBody].dpinc*pow(body[iBody].dhecc,2) + 2*body[iBody].dqinc*body[iBody].dhecc*body[iBody].dkecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][19];
}

double fdDdistDpPrmDir016(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( -body[iBody].dpinc*pow(body[jBody].dqinc,2) + pow(body[jBody].dpinc,2)*body[iBody].dpinc + 2*body[iBody].dqinc*body[jBody].dpinc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][25];
}

double fdDdisturbDpincPrime(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDpPrmDir01(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir08(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir012(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir013(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir014(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir015(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDpPrmDir016(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//--------------dR/dq-------------------------------------------------------//
double fdDdistDqDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dqinc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][2] + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)+pow(body[jBody].dkecc,2)+pow(body[jBody].dhecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][6] + 2*(pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][7] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][8] );
}

double fdDdistDqDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dqinc*(body[iBody].dhecc*body[jBody].dhecc+body[iBody].dkecc*body[jBody].dkecc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][12];
}

double fdDdistDqDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dqinc*( system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][13] + (pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2)+pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][14] + (pow(body[iBody].dpinc,2) + pow(body[iBody].dqinc,2) + pow(body[jBody].dpinc,2) + pow(body[jBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][15] ) + 2*body[iBody].dqinc*(body[iBody].dpinc*body[jBody].dpinc+body[iBody].dqinc*body[jBody].dqinc)*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][15];
}

double fdDdistDqDir05(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dqinc*pow(body[iBody].dkecc,2) - body[iBody].dqinc*pow(body[iBody].dhecc,2) + 2*body[iBody].dpinc*body[iBody].dhecc*body[iBody].dkecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][17];
}

double fdDdistDqDir06(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[jBody].dkecc*body[iBody].dkecc*body[iBody].dqinc - body[jBody].dhecc*body[iBody].dhecc*body[iBody].dqinc + body[jBody].dhecc*body[iBody].dkecc*body[iBody].dpinc + body[jBody].dkecc*body[iBody].dhecc*body[iBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][18];
}

double fdDdistDqDir07(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dqinc*pow(body[jBody].dkecc,2) - body[iBody].dqinc*pow(body[jBody].dhecc,2) + 2*body[iBody].dpinc*body[jBody].dhecc*body[jBody].dkecc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][19];
}

double fdDdistDqDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( pow(body[iBody].dkecc,2)*body[jBody].dqinc - pow(body[iBody].dhecc,2)*body[jBody].dqinc + 2*body[iBody].dhecc*body[iBody].dkecc*body[jBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][20];
}

double fdDdistDqDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*body[iBody].dkecc*body[jBody].dqinc + body[jBody].dhecc*body[iBody].dhecc*body[jBody].dqinc + body[jBody].dhecc*body[iBody].dkecc*body[jBody].dpinc - body[jBody].dkecc*body[iBody].dhecc*body[jBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][21];
}

double fdDdistDqDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*body[iBody].dkecc*body[jBody].dqinc + body[iBody].dhecc*body[jBody].dhecc*body[jBody].dqinc - body[jBody].dhecc*body[iBody].dkecc*body[jBody].dpinc + body[jBody].dkecc*body[iBody].dhecc*body[jBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][22];
}

double fdDdistDqDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[jBody].dkecc*body[iBody].dkecc*body[jBody].dqinc - body[jBody].dhecc*body[iBody].dhecc*body[jBody].dqinc + body[jBody].dhecc*body[iBody].dkecc*body[jBody].dpinc + body[jBody].dkecc*body[iBody].dhecc*body[jBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][23];
}

double fdDdistDqDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( pow(body[jBody].dkecc,2)*body[jBody].dqinc - pow(body[jBody].dhecc,2)*body[jBody].dqinc + 2*body[jBody].dhecc*body[jBody].dkecc*body[jBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][24];
}

double fdDdistDqDir016(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dqinc*pow(body[jBody].dqinc,2) - body[iBody].dqinc*pow(body[jBody].dpinc,2) + 2*body[jBody].dpinc*body[jBody].dqinc*body[iBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[iBody][jBody]][25];
}

  
double fdDdisturbDqinc(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[1]].dSemi/AUCM;
  y = ( fdDdistDqDir01(body, system, iaBody[0], iaBody[1]) \
     + fdDdistDqDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir05(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir06(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir07(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir08(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir012(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqDir016(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}

//------dR/dq'--------------------------------------------------------------//
double fdDdistDqPrmDir01(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dqinc*( system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][2] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2)+pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][6] + 2*(pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][7] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][8] );
}

double fdDdistDqPrmDir02(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*body[iBody].dqinc*(body[jBody].dhecc*body[iBody].dhecc+body[jBody].dkecc*body[iBody].dkecc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][12];
}

double fdDdistDqPrmDir03(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return body[jBody].dqinc*( system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][13] + (pow(body[jBody].dhecc,2)+pow(body[jBody].dkecc,2)+pow(body[iBody].dhecc,2)+pow(body[iBody].dkecc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][14] + (pow(body[jBody].dpinc,2)+pow(body[jBody].dqinc,2)+pow(body[iBody].dpinc,2)+pow(body[iBody].dqinc,2))*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][15] ) + 2*body[iBody].dqinc*(body[jBody].dpinc*body[iBody].dpinc+body[jBody].dqinc*body[iBody].dqinc)*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][15];
}

double fdDdistDqPrmDir08(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( pow(body[jBody].dkecc,2)*body[jBody].dqinc - pow(body[jBody].dhecc,2)*body[jBody].dqinc + 2*body[jBody].dhecc*body[jBody].dkecc*body[jBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][20];
}

double fdDdistDqPrmDir09(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[iBody].dkecc*body[jBody].dkecc*body[jBody].dqinc + body[iBody].dhecc*body[jBody].dhecc*body[jBody].dqinc - body[iBody].dhecc*body[jBody].dkecc*body[jBody].dpinc + body[iBody].dkecc*body[jBody].dhecc*body[jBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][21];
}

double fdDdistDqPrmDir010(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[iBody].dkecc*body[jBody].dkecc*body[jBody].dqinc + body[iBody].dhecc*body[jBody].dhecc*body[jBody].dqinc + body[iBody].dhecc*body[jBody].dkecc*body[jBody].dpinc - body[iBody].dkecc*body[jBody].dhecc*body[jBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][22];
}

double fdDdistDqPrmDir011(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( body[iBody].dkecc*body[jBody].dkecc*body[jBody].dqinc - body[iBody].dhecc*body[jBody].dhecc*body[jBody].dqinc + body[iBody].dhecc*body[jBody].dkecc*body[jBody].dpinc + body[iBody].dkecc*body[jBody].dhecc*body[jBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][23];
}

double fdDdistDqPrmDir012(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return ( pow(body[iBody].dkecc,2)*body[jBody].dqinc - pow(body[iBody].dhecc,2)*body[jBody].dqinc + 2*body[iBody].dhecc*body[iBody].dkecc*body[jBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][24];
}
  
double fdDdistDqPrmDir013(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dqinc*pow(body[jBody].dkecc,2) - body[iBody].dqinc*pow(body[jBody].dhecc,2) + 2*body[iBody].dpinc*body[jBody].dhecc*body[jBody].dkecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][17];
}

double fdDdistDqPrmDir014(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dqinc*body[jBody].dkecc*body[iBody].dkecc - body[iBody].dqinc*body[jBody].dhecc*body[iBody].dhecc + body[iBody].dhecc*body[jBody].dkecc*body[iBody].dpinc + body[iBody].dkecc*body[jBody].dhecc*body[iBody].dpinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][18];
}

double fdDdistDqPrmDir015(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( body[iBody].dqinc*pow(body[iBody].dkecc,2) - body[iBody].dqinc*pow(body[iBody].dhecc,2) + 2*body[iBody].dpinc*body[iBody].dhecc*body[iBody].dkecc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][19];
}

double fdDdistDqPrmDir016(BODY *body, SYSTEM *system, int iBody, int jBody) {
  return 2*( pow(body[jBody].dqinc,2)*body[iBody].dqinc - pow(body[jBody].dpinc,2)*body[iBody].dqinc + 2*body[iBody].dpinc*body[jBody].dpinc*body[jBody].dqinc )*system->dmLaplaceC[system->imLaplaceN[jBody][iBody]][25];
} 
    
double fdDdisturbDqincPrime(BODY *body, SYSTEM *system, int *iaBody) {
  double y, dMfac, dSemiPrm;
  dMfac = KGAUSS*KGAUSS*body[iaBody[1]].dMass/MSUN;
  dSemiPrm = body[iaBody[0]].dSemi/AUCM;
  y = ( fdDdistDqPrmDir01(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir02(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir03(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir08(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir09(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir010(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir011(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir012(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir013(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir014(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir015(body, system, iaBody[0], iaBody[1]) \
    + fdDdistDqPrmDir016(body, system, iaBody[0], iaBody[1]) ) * dMfac/dSemiPrm;
  return y;
}



//------------------- h k p q -------------------------------------
double fdLagrangeDhDt(BODY *body, SYSTEM *system, int *iaBody) {
  int i;
  double sum = 0.0, dMu;
  //Here, iaBody[0] = body in question, iaBody[1] = perturber

  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += ( sqrt(1-pow(body[iaBody[0]].dhecc,2)-pow(body[iaBody[0]].dkecc,2))*fdDdisturbDkecc(body, system, iaBody) + body[iaBody[0]].dkecc*(body[iaBody[0]].dpinc*fdDdisturbDpinc(body, system, iaBody) +body[iaBody[0]].dqinc*fdDdisturbDqinc(body, system, iaBody))/(2*sqrt(1-pow(body[iaBody[0]].dhecc,2)-pow(body[iaBody[0]].dkecc,2))) ) / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      
  } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
    sum += ( sqrt(1-pow(body[iaBody[0]].dhecc,2)-pow(body[iaBody[0]].dkecc,2))*fdDdisturbDkeccPrime(body, system, iaBody) + body[iaBody[0]].dkecc*(body[iaBody[0]].dpinc*fdDdisturbDpincPrime(body, system, iaBody) +body[iaBody[0]].dqinc*fdDdisturbDqincPrime(body, system, iaBody))/(2*sqrt(1-pow(body[iaBody[0]].dhecc,2)-pow(body[iaBody[0]].dkecc,2))) ) / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
  }
  
  return sum/DAYSEC;
}

double fdLagrangeDkDt(BODY *body, SYSTEM *system, int *iaBody) {
  int i;
  double sum = 0.0, dMu;
  
  dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iBody].dMass)/MSUN;
  if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
    sum += -( sqrt(1-pow(body[iaBody[0]].dhecc,2)-pow(body[iaBody[0]].dkecc,2))*fdDdisturbDhecc(body, system, iaBody) + body[iaBody[0]].dhecc*(body[iaBody[0]].dpinc*fdDdisturbDpinc(body, system, iaBody) +body[iaBody[0]].dqinc*fdDdisturbDqinc(body, system, iaBody))/(2*sqrt(1-pow(body[iaBody[0]].dhecc,2)-pow(body[iaBody[0]].dkecc,2))) ) / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
      
  } else if (body[iaBody].dSemi > body[iaBody[1]].dSemi) {
    sum += -( sqrt(1-pow(body[iaBody[0]].dhecc,2)-pow(body[iaBody[0]].dkecc,2))*fdDdisturbDheccPrime(body, system, iaBody) + body[iaBody[0]].dhecc*(body[iaBody[0]].dpinc*fdDdisturbDpincPrime(body, system, iaBody) +body[iaBody[0]].dqinc*fdDdisturbDqincPrime(body, system, iaBody))/(2*sqrt(1-pow(body[iaBody[0]].dhecc,2)-pow(body[iaBody[0]].dkecc,2))) ) / sqrt(dMu*body[iaBody[0]].dSemi/AUCM);
  }      

  return sum/DAYSEC;
}

double fdLagrangeDpDt(BODY *body, SYSTEM *system, int *iaBody) {
    int i;
    double sum = 0.0, dMu;
    
    dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
    if (body[iaBody[0]].dSemi < body[iaBody[1].dSemi) {
      sum += ( body[iaBody[0]].dpinc*(-body[iaBody[0]].dkecc*fdDdisturbDhecc(body, system, iaBody[0], iaBody[i])+body[iaBody[0]].dhecc*fdDdisturbDkecc(body, system, iaBody)) + 1.0/2.0*fdDdisturbDqinc(body, system, iaBody) )/(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(1-pow(body[iaBody[0]].dhecc,2)-pow(body[iaBody[0]].dkecc,2))));
    } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dpinc*(-body[iaBody[0]].dkecc*fdDdisturbDheccPrime(body, system, iaBody)+body[iaBody[0]].dhecc*fdDdisturbDkeccPrime(body, system, iaBody)) + 1.0/2.0*fdDdisturbDqincPrime(body, system, iaBody) )/(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(1-pow(body[iaBody[0]].dhecc,2)-pow(body[iaBody[0]].dkecc,2))));
    }
    
    return sum/DAYSEC;
}


double fdLagrangeDqDt(BODY *body, SYSTEM *system, int *iaBody) {
    int i;
    double sum = 0.0, dMu;
    
    dMu = KGAUSS*KGAUSS*(body[0].dMass+body[iaBody[0]].dMass)/MSUN;
    if (body[iaBody[0]].dSemi < body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dqinc*(-body[iaBody[0]].dkecc*fdDdisturbDhecc(body, system, iaBody)+body[iaBody[0]].dhecc*fdDdisturbDkecc(body, system, iaBody)) - 1.0/2.0*fdDdisturbDpinc(body, system, iaBody) )/(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(1-pow(body[iaBody[0]].dhecc,2)-pow(body[iaBody[0]].dkecc,2))));
    } else if (body[iaBody[0]].dSemi > body[iaBody[1]].dSemi) {
      sum += ( body[iaBody[0]].dqinc*(-body[iaBody[0]].dkecc*fdDdisturbDheccPrime(body, system, iaBody)+body[iaBody[0]].dhecc*fdDdisturbDkeccPrime(body, system, iaBody)) - 1.0/2.0*fdDdisturbDpincPrime(body, system, iaBody) )/(2*sqrt(dMu*body[iaBody[0]].dSemi/AUCM*(1-pow(body[iaBody[0]].dhecc,2)-pow(body[iaBody[0]].dkecc,2))));
    }
   
    return sum/DAYSEC;
}

