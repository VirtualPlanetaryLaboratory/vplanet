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
  dest[iBody].dEnvelopeMass = src[iBody].dEnvelopeMass;
  dest[iBody].dXFrac = src[iBody].dXFrac;
  dest[iBody].dAtmXAbsEff = src[iBody].dAtmXAbsEff;
  dest[iBody].dMinSurfaceWaterMass = src[iBody].dMinSurfaceWaterMass;
  dest[iBody].dMinEnvelopeMass = src[iBody].dMinEnvelopeMass;
}

/**************** ATMESC options ********************/

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


void ReadAtmXAbsEff(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
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
    body[iFile-1].dAtmXAbsEff = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dAtmXAbsEff = options->dDefault;
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
  
  sprintf(options[OPT_ATMXABSEFF].cName,"dAtmXAbsEff");
  sprintf(options[OPT_ATMXABSEFF].cDescr,"X-ray/XUV absorption efficiency (epsilon)");
  sprintf(options[OPT_ATMXABSEFF].cDefault,"0.15");
  options[OPT_ATMXABSEFF].dDefault = 0.15;
  options[OPT_ATMXABSEFF].iType = 2;
  options[OPT_ATMXABSEFF].iMultiFile = 1;
  fnRead[OPT_ATMXABSEFF] = &ReadAtmXAbsEff;

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

void fnPropertiesAtmEsc(BODY *body, UPDATE *update, int iBody) {
  /* Nothing */
}

void fnForceBehaviorAtmEsc(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  
  if (body[iBody].dSurfaceWaterMass <= body[iBody].dMinSurfaceWaterMass)
    // Let's desiccate this planet.
    body[iBody].dSurfaceWaterMass = 0;
  
  if (body[iBody].dEnvelopeMass <= body[iBody].dMinEnvelopeMass)
    // Let's remove its envelope.
    body[iBody].dEnvelopeMass = 0;
  
}

void VerifyAtmEsc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int bAtmEsc=0;

  /* AtmEsc is active for this body if this subroutine is called. */
  
  if (body[iBody].dSurfaceWaterMass > 0) {
    VerifySurfaceWaterMass(body,options,update,body[iBody].dAge,fnUpdate,iBody);
    VerifyOxygenMass(body,options,update,body[iBody].dAge,fnUpdate,iBody);
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
      fprintf(stderr,"ERROR: %s cannot be grater than %s in file %s.\n",options[OPT_ENVELOPEMASS].cName,options[OPT_MASS].cName,files->Infile[iBody+1].cIn);
    exit(EXIT_INPUT);
  }
  
  if (!bAtmEsc && control->Io.iVerbose >= VERBINPUT) 
    fprintf(stderr,"WARNING: ATMESC called for body %s, but no atmosphere/water present!\n",body[iBody].cName);

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorAtmEsc;
  control->Evolve.fnPropsAux[iBody][iModule] = &fnPropertiesAtmEsc;
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
  }
  
  if (body[iBody].dEnvelopeMass > 0) {
    if (update[iBody].iNumEnvelopeMass == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumEnvelopeMass++;
    
    if (update[iBody].iNumMass == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumMass++;
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

void InitializeOutputAtmEsc(OUTPUT *output,fnWriteOutput fnWrite[]) {
  
  sprintf(output[OUT_SURFACEWATERMASS].cName,"SurfWaterMass");
  sprintf(output[OUT_SURFACEWATERMASS].cDescr,"Surface Water Mass");
  sprintf(output[OUT_SURFACEWATERMASS].cNeg,"TO");
  output[OUT_SURFACEWATERMASS].bNeg = 1;
  output[OUT_SURFACEWATERMASS].dNeg = 1./TOMASS;
  output[OUT_SURFACEWATERMASS].iNum = 1;
  output[OUT_SURFACEWATERMASS].iModuleBit = ATMESC;
  fnWrite[OUT_SURFACEWATERMASS] = &WriteSurfaceWaterMass;

  sprintf(output[OUT_OXYGENMASS].cName,"OxygenMass");
  sprintf(output[OUT_OXYGENMASS].cDescr,"Oxygen Mass");
  sprintf(output[OUT_OXYGENMASS].cNeg,"bars");
  output[OUT_OXYGENMASS].bNeg = 1;
  output[OUT_OXYGENMASS].dNeg = 1;
  output[OUT_OXYGENMASS].iNum = 1;
  output[OUT_OXYGENMASS].iModuleBit = ATMESC;
  fnWrite[OUT_OXYGENMASS] = &WriteOxygenMass;

  sprintf(output[OUT_RGLIMIT].cName,"RGLimit");
  sprintf(output[OUT_RGLIMIT].cDescr,"Runaway Greenhouse Semi-Major Axis");
  sprintf(output[OUT_RGLIMIT].cNeg,"AU");
  output[OUT_RGLIMIT].bNeg = 1;
  output[OUT_RGLIMIT].dNeg = 1. / AUCM;
  output[OUT_RGLIMIT].iNum = 1;
  output[OUT_RGLIMIT].iModuleBit = ATMESC;
  fnWrite[OUT_RGLIMIT] = &WriteRGLimit;
  
  sprintf(output[OUT_ENVELOPEMASS].cName,"EnvelopeMass");
  sprintf(output[OUT_ENVELOPEMASS].cDescr,"Envelope Mass");
  sprintf(output[OUT_ENVELOPEMASS].cNeg,"Earth");
  output[OUT_ENVELOPEMASS].bNeg = 1;
  output[OUT_ENVELOPEMASS].dNeg = 1./MEARTH;
  output[OUT_ENVELOPEMASS].iNum = 1;
  output[OUT_ENVELOPEMASS].iModuleBit = ATMESC;
  fnWrite[OUT_ENVELOPEMASS] = &WriteEnvelopeMass;

}

void FinalizeOutputFunctionAtmEsc(OUTPUT *output,int iBody,int iModule) {
  output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxAtmEsc;
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
  module->fnFinalizeUpdateEnvelopeMass[iBody][iModule] = &FinalizeUpdateEnvelopeMassAtmEsc;

  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionAtmEsc;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionAtmEsc;

}

/************* ATMESC Functions ************/

double fdDSurfaceWaterMassDt(BODY *body,SYSTEM *system,int *iaBody) {
  // TODO: Add diffusion-limited escape option

  // TODO: The checks below need to be moved. Ideally we'd 
  // just remove this equation from the matrix if the
  // escape conditions are not met.
  
  // 1. Check if there's hydrogen to be lost; this happens first 
  if (body[iaBody[0]].dEnvelopeMass > 0)
    return 0;
  
  // 2. Check if planet is beyond RG limit; otherwise, assume the
  // cold trap prevents water loss.
  // NOTE: The RG flux limit below is calculated based on body zero's
  // spectrum! The Kopparapu+14 limit is for a single star only. This
  // approximation for a binary is only valid if the two stars have 
  // similar spectral types, or if body zero dominates the flux.
  double flux;
  if(body[iaBody[0]].bBinary == 1 && body[iaBody[0]].iBodyType == 0) { 
    flux = fdFluxExactBinary(body,system,iaBody,body[0].dLuminosity,body[1].dLuminosity);
  }
  else { 
    flux = body[0].dLuminosity / (4 * PI * pow(body[iaBody[0]].dSemi, 2) * 
           pow((1 - body[iaBody[0]].dEcc * body[iaBody[0]].dEcc), 0.5));
  }

  if (flux < fdHZRG14(body[0].dLuminosity, body[0].dTemperature, body[iaBody[0]].dEcc, body[iaBody[0]].dMass))
    return 0;
      
  // Compute Erkaev's tidal enhancement factor
  double elim, fxuv, xi, ktide;
  xi = (pow(body[iaBody[0]].dMass / (3. * body[0].dMass), (1. / 3)) * 
       body[iaBody[0]].dSemi) / (body[iaBody[0]].dRadius * body[iaBody[0]].dXFrac);
  if (xi > 1)	ktide = (1 - 3 / (2 * xi) + 1 / (2 * pow(xi, 3)));
  else ktide = 0;

  // If planet is a CBP, compute flux from 2 stars
  if(body[iaBody[0]].bBinary == 1 && body[iaBody[0]].iBodyType == 0) { 
    // CBP
    fxuv = fdFluxExactBinary(body,system,iaBody,body[0].dLXUV,body[1].dLXUV);
  }
  else { 
    // Normal single-star planet
    fxuv = body[0].dLXUV / (4 * PI * pow(body[iaBody[0]].dSemi, 2) * 
           pow((1 - body[iaBody[0]].dEcc * body[iaBody[0]].dEcc), 0.5));
  }

  // Rodrigo and Barnes (2015)
  double T = 400.;                  // Average thermospheric temperature (K, Venus)
  double b = 4.8e19 * pow(T, 0.75); // binary diffusion coefficient (m^-1 s^-1)
  double g = (BIGG * body[iaBody[0]].dMass) / (body[iaBody[0]].dRadius * body[iaBody[0]].dRadius);
  double FHREF = (body[iaBody[0]].dAtmXAbsEff * fxuv * body[iaBody[0]].dRadius) /
                 (4 * BIGG * body[iaBody[0]].dMass * ktide * ATOMMASS);
  double x = (KBOLTZ * T * FHREF) / (10 * b * g * ATOMMASS);
  double eta;
  if (x < 1)
    eta = 0;
  else
    eta = (x - 1) / (x + 8);
  double MDOTEL = FHREF * (4 * ATOMMASS * PI * body[iaBody[0]].dRadius * body[iaBody[0]].dRadius);
  
  return -(9. / (1 + 8 * eta)) * MDOTEL;
}

double fdDOxygenMassDt(BODY *body,SYSTEM *system,int *iaBody) {

  // TODO: The checks below need to be moved. Ideally we'd 
  // just remove this equation from the matrix if the
  // escape conditions are not met.
  
  // 1. Check if there's hydrogen to be lost; this happens first 
  if (body[iaBody[0]].dEnvelopeMass > 0)
    return 0;
    
  // 2. Check if planet is beyond RG limit; otherwise, assume the
  // cold trap prevents water loss.
  // NOTE: The RG flux limit below is calculated based on body zero's
  // spectrum! The Kopparapu+14 limit is for a single star only. This
  // approximation for a binary is only valid if the two stars have 
  // similar spectral types, or if body zero dominates the flux.
  double flux;
  if(body[iaBody[0]].bBinary == 1 && body[iaBody[0]].iBodyType == 0) { 
    flux = fdFluxExactBinary(body,system,iaBody,body[0].dLuminosity,body[1].dLuminosity);
  }
  else { 
    flux = body[0].dLuminosity / (4 * PI * pow(body[iaBody[0]].dSemi, 2) * 
           pow((1 - body[iaBody[0]].dEcc * body[iaBody[0]].dEcc), 0.5));
  }
  if (flux < fdHZRG14(body[0].dLuminosity, body[0].dTemperature, body[iaBody[0]].dEcc, body[iaBody[0]].dMass))
    return 0;
  
  // 3. Check if there's still water left
  if (body[iaBody[0]].dSurfaceWaterMass <= 0)
    return 0;
  
  // Rodrigo and Barnes (2015)
  double T = 400.;                  // Average thermospheric temperature (K, Venus)
  double b = 4.8e19 * pow(T, 0.75); // binary diffusion coefficient (m^-1 s^-1)
  return (320. * PI * BIGG * ATOMMASS * ATOMMASS * b * body[iaBody[0]].dMass) / (KBOLTZ * T);
}

double fdDEnvelopeMassDt(BODY *body,SYSTEM *system,int *iaBody) {
  // TODO: Currently this is just Erkaev's model. Add other escape regimes
  
  // TODO: This needs to be moved. Ideally we'd just remove this equation from the matrix.
  if (body[iaBody[0]].dEnvelopeMass <= 0){
    return 0;
  }
  double elim, fxuv, xi, ktide;
  
  xi = (pow(body[iaBody[0]].dMass / (3. * body[0].dMass), (1. / 3)) * 
       body[iaBody[0]].dSemi) / (body[iaBody[0]].dRadius * body[iaBody[0]].dXFrac);
  if (xi > 1)	ktide = (1 - 3 / (2 * xi) + 1 / (2 * pow(xi, 3)));
  else ktide = 0;

  // If planet is a CBP, compute flux from 2 stars
  if(body[iaBody[0]].bBinary == 1 && body[iaBody[0]].iBodyType == 0) { // CBP
    fxuv = fdFluxExactBinary(body,system,iaBody,body[0].dLXUV,body[1].dLXUV); 
  }
  else { // normal single-star planet
    fxuv = body[0].dLXUV / (4 * PI * pow(body[iaBody[0]].dSemi, 2) * 
           pow((1 - body[iaBody[0]].dEcc * body[iaBody[0]].dEcc), 0.5));
  }

  elim = PI * pow(body[iaBody[0]].dRadius, 3) * pow(body[iaBody[0]].dXFrac, 2) * 
         body[iaBody[0]].dAtmXAbsEff * fxuv / (BIGG * body[iaBody[0]].dMass * ktide);

  return -elim;
}

double fdSurfEnFluxAtmEsc(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  // This is silly, but necessary!
  return 0;
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