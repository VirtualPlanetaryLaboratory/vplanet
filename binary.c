/********************** BINARY.C **********************/
/*
 * David Fleming (dflemin3), Tue Jan 12 10:20am PDT 2016
 *
 * Subroutines that control the integration of the 
 * circumbinary planet dynamics module.
 *
*/
// NOTE: Still need to edit module.c, update.c

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void  InitializeControlBinary(CONTROL *control) {
  /* Nothing for now, but this subroutine is necessary for module loops. */
}

void BodyCopyBinary(BODY *dest,BODY *src,int foo,int iBody) {
  // Copy body properties from src to dest
  
  for(int i = 0; i < 3; i++) {
    dest[iBody].daCylPos[i] = src[iBody].daCylPos[i];
    dest[iBody].daCylVel[i] = src[iBody].daCylVel[i];
    dest[iBody].dCartPos[i] = src[iBody].dCartPos[i];
    dest[iBody].dCartVel[i] = src[iBody].dCartVel[i];
  }
  
  dest[iBody].dFreeEcc = src[iBody].dFreeEcc;
  dest[iBody].dFreeInc = src[iBody].dFreeInc;
  dest[iBody].dLL13N0 = src[iBody].dLL13N0;
  dest[iBody].dLL13K0 = src[iBody].dLL13K0;
  dest[iBody].dLL13V0 = src[iBody].dLL13V0;

}

void InitializeBodyBinary(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
// TODO:
// Have something here where if the body is a planet, set it's dLL13N0,K0, and V0 paremters
// 
  // Malloc space for cylindrical position, velocity arrays
  body[iBody].daCylPos = malloc(3*sizeof(double));
  body[iBody].daCylVel = malloc(3*sizeof(double));
  body[iBody].dCartPos = malloc(3*sizeof(double));
  body[iBody].dCartVel = malloc(3*sizeof(double));

  // Init values to 0s?
}

void InitializeUpdateTmpBodyBinary(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
  // Malloc space for cylindrical position, velocity arrays for tmp body
  control->Evolve.tmpBody[iBody].daCylPos = malloc(3*sizeof(double));
  control->Evolve.tmpBody[iBody].daCylVel = malloc(3*sizeof(double));
  control->Evolve.tmpBody[iBody].dCartPos = malloc(3*sizeof(double));
  control->Evolve.tmpBody[iBody].dCartVel = malloc(3*sizeof(double));

}

/**************** BINARY options ********************/

void ReadFreeEcc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // Free eccentricity, can't be in primary file
  // Note: Do error checking for negative values
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0.0 || dTmp >= 1.0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be in range [0,1).\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    } else
      body[iFile-1].dFreeEcc = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dFreeEcc = options->dDefault;
}

void ReadLL13N0(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // Lee + Leung 2013 Mean Motion N0
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);  
    }
    body[iFile-1].dLL13N0 = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dLL13N0,files->iNumInputs);
  }
}

void ReadLL13K0(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // Lee + Leung 2013 radial epicyclic frequency
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);  
    }
    body[iFile-1].dLL13K0 = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dLL13K0,files->iNumInputs);
  }
}


void ReadLL13V0(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  // Lee + Leung 2013 vertical epicyclic frequency
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);  
    }
    body[iFile-1].dLL13V0 = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dLL13V0,files->iNumInputs);
  }
}

void ReadFreeInc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  /* Free inclination goes from 0 to 180 degrees */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) { // Input as radians
      if (dTmp < 0 || dTmp > PI) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);        
      }
    } else { // Input as Degrees
      if (dTmp < 0 || dTmp > 180) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,180].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);        
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile-1].dFreeInc = dTmp; 
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else 
    if (iFile > 0)
      body[iFile-1].dFreeInc = options->dDefault;
}  

/* Init Options BINARY */

void InitializeOptionsBinary(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  sprintf(options[OPT_FREEECC].cName,"dFreeEcc");
  sprintf(options[OPT_FREEECC].cDescr,"Circumbinary planet free eccentricity");
  sprintf(options[OPT_FREEECC].cDefault,"0.0");
  options[OPT_FREEECC].dDefault = 0.0;
  options[OPT_FREEECC].iType = 2;
  options[OPT_FREEECC].iMultiFile = 1;
  fnRead[OPT_FREEECC] = &ReadFreeEcc;

  sprintf(options[OPT_FREEINC].cName,"dFreeInc");
  sprintf(options[OPT_FREEINC].cDescr,"Circumbinary planet free inclination");
  sprintf(options[OPT_FREEINC].cDefault,"0.0 degrees");
  options[OPT_FREEINC].dDefault = 0.0;
  options[OPT_FREEINC].iType = 2;
  options[OPT_FREEINC].iMultiFile = 1;
  fnRead[OPT_FREEINC] = &ReadFreeInc;

  sprintf(options[OPT_LL13N0].cName,"dLL13N0");
  sprintf(options[OPT_LL13N0].cDescr,"Lee+Leung 2013 Mean Motion");
  sprintf(options[OPT_LL13N0].cDefault,"1 /yr");
  options[OPT_LL13N0].dDefault = 1./YEARSEC;
  options[OPT_LL13N0].iType = 2;
  options[OPT_LL13N0].iMultiFile = 1;
  options[OPT_LL13N0].dNeg = 1./YEARSEC;
  sprintf(options[OPT_LL13N0].cNeg,"/Year");
  fnRead[OPT_LL13N0] = &ReadLL13N0;

  sprintf(options[OPT_LL13K0].cName,"dLL13K0");
  sprintf(options[OPT_LL13K0].cDescr,"Lee+Leung 2013 Radial Epicyclic Frequency");
  sprintf(options[OPT_LL13K0].cDefault,"1 /yr");
  options[OPT_LL13K0].dDefault = 1./YEARSEC;
  options[OPT_LL13K0].iType = 2;
  options[OPT_LL13K0].iMultiFile = 1;
  options[OPT_LL13K0].dNeg = 1./YEARSEC;
  sprintf(options[OPT_LL13K0].cNeg,"/Year");
  fnRead[OPT_LL13K0] = &ReadLL13K0;

  sprintf(options[OPT_LL13V0].cName,"dLL13V0");
  sprintf(options[OPT_LL13V0].cDescr,"Lee+Leung 2013 Radial Epicyclic Frequency");
  sprintf(options[OPT_LL13V0].cDefault,"1 /yr");
  options[OPT_LL13V0].dDefault = 1./YEARSEC;
  options[OPT_LL13V0].iType = 2;
  options[OPT_LL13V0].iMultiFile = 1;
  options[OPT_LL13V0].dNeg = 1./YEARSEC;
  sprintf(options[OPT_LL13V0].cNeg,"/Year");
  fnRead[OPT_LL13V0] = &ReadLL13V0;

}

void ReadOptionsBinary(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTBINARY;iOpt<OPTENDBINARY;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify BINARY ******************/

/*
void VerifySurfaceWaterMass(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {

  update[iBody].iaType[update[iBody].iSurfaceWaterMass][0] = 1;
  update[iBody].iNumBodies[update[iBody].iSurfaceWaterMass][0] = 1;
  update[iBody].iaBody[update[iBody].iSurfaceWaterMass][0] = malloc(update[iBody].iNumBodies[update[iBody].iSurfaceWaterMass][0]*sizeof(int));
  update[iBody].iaBody[update[iBody].iSurfaceWaterMass][0][0] = iBody;

  update[iBody].pdDSurfaceWaterMassDtAtmesc = &update[iBody].daDerivProc[update[iBody].iSurfaceWaterMass][0];
  fnUpdate[iBody][update[iBody].iSurfaceWaterMass][0] = &fdDSurfaceWaterMassDt;
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

void fnForceBehaviorAtmEsc(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,int iBody,int iModule) {
  
  if (body[iBody].dSurfaceWaterMass <= body[iBody].dMinSurfaceWaterMass)
    // Let's desiccate this planet.
    body[iBody].dSurfaceWaterMass = 0;
  
  if (body[iBody].dEnvelopeMass <= body[iBody].dMinEnvelopeMass)
    // Let's remove its envelope.
    body[iBody].dEnvelopeMass = 0;
  
}
*/

void fnPropertiesBinary(BODY *body, UPDATE *update, int iBody){
}

/* Force behavior */
void fnForceBehaviorBinary(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,int iBody,int iModule){
}

void VerifyBinary(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  /*
  int bAtmEsc=0;

  
  if (body[iBody].dSurfaceWaterMass > 0) {
    VerifySurfaceWaterMass(body,options,update,body[iBody].dAge,fnUpdate,iBody);
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
  */

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorBinary;
  control->Evolve.fnPropsAux[iBody][iModule] = &fnPropertiesBinary;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyBinary;
}

void InitializeModuleBinary(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** BINARY Update ****************/

void InitializeUpdateBinary(BODY *body, UPDATE *update, int iBody) {
  //TODO
}

/*
void InitializeUpdateAtmEsc(BODY *body,UPDATE *update,int iBody) {  
  if (body[iBody].dSurfaceWaterMass > 0) {
    if (update[iBody].iNumSurfaceWaterMass == 0)
      update[iBody].iNumVars++;
    update[iBody].iNumSurfaceWaterMass++;
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

void FinalizeUpdateEccBinary(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody,int iFoo) {
}

void FinalizeUpdateSurfaceWaterMassAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumSurfaceWaterMass = (*iEqn)++;
}

void FinalizeUpdateEnvelopeMassAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumEnvelopeMass = (*iEqn)++;
}

void FinalizeUpdateMassAtmEsc(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = ATMESC;
  update[iBody].iNumMass = (*iEqn)++;
}

*/      

/***************** BINARY Halts *****************/

/*
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

*/

void CountHaltsBinary(HALT *halt,int *iHalt) {
// TODO
}

void VerifyHaltBinary(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {

/*
  if (control->Halt[iBody].bSurfaceDesiccated)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltSurfaceDesiccated;
  
  if (control->Halt[iBody].bEnvelopeGone)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltEnvelopeGone;
*/
}

/************* BINARY Outputs ******************/

void HelpOutputBinary(OUTPUT *output) {
  int iOut;

  printf("\n ------ BINARY output ------\n");
  for (iOut=OUTSTARTBINARY;iOut<OUTENDBINARY;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

void WriteFreeEcc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  // Note: Only makes sense for planets (iBodyType == 0)
  if(body[iBody].iBodyType == 0)
    *dTmp = body[iBody].dFreeEcc;
  else
    *dTmp = -1;

  strcpy(cUnit,"");
}

void WriteFreeInc(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  // Note: Only makes sense for planets (iBodyType == 0)
  if(body[iBody].iBodyType == 0)
    *dTmp = body[iBody].dFreeInc;
  else
    *dTmp = -1;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
    *dTmp /= fdUnitsAngle(units->iAngle);
    fsUnitsAngle(units->iAngle,cUnit);
  }
}

void WriteLL13N0(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
 // Note: Only applies to planets (iBodyType == 0)
 if(body[iBody].iBodyType == 0)
   *dTmp = body[iBody].dLL13N0;
 else
   *dTmp = -1;

 if(output->bDoNeg[iBody]) {
   *dTmp *= output->dNeg;
   strcpy(cUnit,output->cNeg);
 } else {
   *dTmp *= fdUnitsTime(units->iTime);
   fsUnitsTime(units->iTime,cUnit);
 }
}


void WriteLL13K0(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
 // Note: Only applies to planets (iBodyType == 0)
 if(body[iBody].iBodyType == 0)
   *dTmp = body[iBody].dLL13K0;
 else
   *dTmp = -1;

 if(output->bDoNeg[iBody]) {
   *dTmp *= output->dNeg;
   strcpy(cUnit,output->cNeg);
 } else {
   *dTmp *= fdUnitsTime(units->iTime);
   fsUnitsTime(units->iTime,cUnit);
 }
}

void WriteLL13V0(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
 // Note: Only applies to planets (iBodyType == 0)
 if(body[iBody].iBodyType == 0)
   *dTmp = body[iBody].dLL13V0;
 else
   *dTmp = -1;

 if(output->bDoNeg[iBody]) {
   *dTmp *= output->dNeg;
   strcpy(cUnit,output->cNeg);
 } else {
   *dTmp *= fdUnitsTime(units->iTime);
   fsUnitsTime(units->iTime,cUnit);
 }
}

void InitializeOutputBinary(OUTPUT *output,fnWriteOutput fnWrite[])
{
  sprintf(output[OUT_FREEECC].cName,"FreeEcc");
  sprintf(output[OUT_FREEECC].cDescr,"Free Eccentricity");
  output[OUT_FREEECC].bNeg = 0;
  output[OUT_FREEECC].iNum = 1;
  fnWrite[OUT_FREEECC] = &WriteFreeEcc;

  sprintf(output[OUT_FREEINC].cName,"FreeInc");
  sprintf(output[OUT_FREEINC].cDescr,"Free Inclination");
  sprintf(output[OUT_FREEINC].cNeg,"Deg");
  output[OUT_FREEINC].bNeg = 1;
  output[OUT_FREEINC].dNeg = 1./DEGRAD;
  output[OUT_FREEINC].iNum = 1;
  fnWrite[OUT_FREEINC] = &WriteFreeInc;

  sprintf(output[OUT_LL13N0].cName,"LL13N0");
  sprintf(output[OUT_LL13N0].cDescr,"Leung+Lee 2013 Mean Motion");
  sprintf(output[OUT_LL13N0].cNeg,"1/year");
  output[OUT_LL13N0].bNeg = 1;
  output[OUT_LL13N0].dNeg = 1./YEARSEC;
  output[OUT_LL13N0].iNum = 1;
  fnWrite[OUT_LL13N0] = &WriteLL13N0;

  sprintf(output[OUT_LL13K0].cName,"LL13K0");
  sprintf(output[OUT_LL13K0].cDescr,"Leung+Lee 2013 Radial epicyclic frequency");
  sprintf(output[OUT_LL13K0].cNeg,"1/year");
  output[OUT_LL13K0].bNeg = 1;
  output[OUT_LL13K0].dNeg = 1./YEARSEC;
  output[OUT_LL13K0].iNum = 1;
  fnWrite[OUT_LL13K0] = &WriteLL13K0;

  sprintf(output[OUT_LL13V0].cName,"LL13V0");
  sprintf(output[OUT_LL13V0].cDescr,"Leung+Lee 2013 vertical epicyclic frequency");
  sprintf(output[OUT_LL13V0].cNeg,"1/year");
  output[OUT_LL13V0].bNeg = 1;
  output[OUT_LL13V0].dNeg = 1./YEARSEC;
  output[OUT_LL13V0].iNum = 1;
  fnWrite[OUT_LL13V0] = &WriteLL13V0;

}

/*
void InitializeOutputAtmEsc(OUTPUT *output,fnWriteOutput fnWrite[]) {
  
  sprintf(output[OUT_SURFACEWATERMASS].cName,"SurfWaterMass");
  sprintf(output[OUT_SURFACEWATERMASS].cDescr,"Surface Water Mass");
  sprintf(output[OUT_SURFACEWATERMASS].cNeg,"TO");
  output[OUT_SURFACEWATERMASS].bNeg = 1;
  output[OUT_SURFACEWATERMASS].dNeg = 1./TOMASS;
  output[OUT_SURFACEWATERMASS].iNum = 1;
  fnWrite[OUT_SURFACEWATERMASS] = &WriteSurfaceWaterMass;
  
  sprintf(output[OUT_ENVELOPEMASS].cName,"EnvelopeMass");
  sprintf(output[OUT_ENVELOPEMASS].cDescr,"Envelope Mass");
  sprintf(output[OUT_ENVELOPEMASS].cNeg,"Earth");
  output[OUT_ENVELOPEMASS].bNeg = 1;
  output[OUT_ENVELOPEMASS].dNeg = 1./MEARTH;
  output[OUT_ENVELOPEMASS].iNum = 1;
  fnWrite[OUT_ENVELOPEMASS] = &WriteEnvelopeMass;

}
*/

void FinalizeOutputFunctionBinary(OUTPUT *output,int iBody,int iModule) {
  //output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxAtmEsc;
}

/************ BINARY Logging Functions **************/

void LogOptionsBinary(CONTROL *control, FILE *fp) {

fprintf(fp,"-------- BINARY Options -----\n\n");

}

void LogBinary(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  /* Anything here?
  int iOut;

  fprintf(fp,"\n----- BINARY PARAMETERS ------\n");
  for (iOut=OUTSTARTBINARY;iOut<OUTBODYSTARTBINARY;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyBinary(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;  
  fprintf(fp,"----- BINARY PARAMETERS (%s)------\n",body[iBody].cName);
  
  for (iOut=OUTSTARTBINARY;iOut<OUTENDBINARY;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
}

void AddModuleBinary(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = BINARY;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlBinary;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyBinary;

  module->fnCountHalts[iBody][iModule] = &CountHaltsBinary;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsBinary;
  module->fnLogBody[iBody][iModule] = &LogBodyBinary;
  module->fnVerify[iBody][iModule] = &VerifyBinary;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltBinary;

  module->fnInitializeBody[iBody][iModule] = &InitializeBodyBinary;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateBinary;

  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionBinary;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionBinary;

}

/************* BINARY Functions ************/

/*
double fdDSurfaceWaterMassDt(BODY *body,SYSTEM *system,int *iaBody) {
  // TODO: Currently this is just Erkaev's model. Add other escape regimes
  
  // TODO: This needs to be moved. Ideally we'd just remove this equation from the matrix.
  if (body[iaBody[0]].dEnvelopeMass > 0)
    return 0;
  
  double elim, fxuv, xi, ktide;
  
  xi = (pow(body[iaBody[0]].dMass / (3. * body[0].dMass), (1. / 3)) * 
       body[iaBody[0]].dSemi) / (body[iaBody[0]].dRadius * body[iaBody[0]].dXFrac);
  if (xi > 1)	ktide = (1 - 3 / (2 * xi) + 1 / (2 * pow(xi, 3)));
	else ktide = 0;

  fxuv = body[0].dLXUV / (4 * PI * pow(body[iaBody[0]].dSemi, 2) * 
         pow((1 - body[iaBody[0]].dEcc * body[iaBody[0]].dEcc), 0.5));
  
  elim = PI * pow(body[iaBody[0]].dRadius, 3) * pow(body[iaBody[0]].dXFrac, 2) * 
         body[iaBody[0]].dAtmXAbsEff * fxuv / (BIGG * body[iaBody[0]].dMass * ktide);

  return -elim;
}

double fdDEnvelopeMassDt(BODY *body,SYSTEM *system,int *iaBody) {
  // TODO: Currently this is just Erkaev's model. Add other escape regimes
  
  // TODO: This needs to be moved. Ideally we'd just remove this equation from the matrix.
  if (body[iaBody[0]].dEnvelopeMass <= 0)
    return 0;
  
  double elim, fxuv, xi, ktide;
  
  xi = (pow(body[iaBody[0]].dMass / (3. * body[0].dMass), (1. / 3)) * 
       body[iaBody[0]].dSemi) / (body[iaBody[0]].dRadius * body[iaBody[0]].dXFrac);
  if (xi > 1)	ktide = (1 - 3 / (2 * xi) + 1 / (2 * pow(xi, 3)));
	else ktide = 0;

  fxuv = body[0].dLXUV / (4 * PI * pow(body[iaBody[0]].dSemi, 2) * 
         pow((1 - body[iaBody[0]].dEcc * body[iaBody[0]].dEcc), 0.5));
  
  elim = PI * pow(body[iaBody[0]].dRadius, 3) * pow(body[iaBody[0]].dXFrac, 2) * 
         body[iaBody[0]].dAtmXAbsEff * fxuv / (BIGG * body[iaBody[0]].dMass * ktide);

  return -elim;
}
*/

/* 
 * Useful math functions 
 */

/* 3D dot product */
double fdDot(double *a, double *b)
{
  return (a[0]*b[0] + a[1]*b[1] + a[2]*b[2]);
}

/* Kronecker Delta */
int fiDelta(int i, int j)
{
  if(i == j)
    return 1;
  else
    return 0;
}

/* Convert from cylindrical position coords to cartesian (3 dimensional) */
void fvCylToCartPos(double *daCylPos, double *dCartPos)
{
  dCartPos[0] = daCylPos[0]*cos(daCylPos[1]);
  dCartPos[1] = daCylPos[1]*sin(daCylPos[1]);
  dCartPos[2] = daCylPos[2];
}

/* Convert from cylindrical velocity coords to cartesian (3 dimensional) */
void fvCylToCartVel(double *daCylPos, double *daCylVel, double *dCartVel)
{
  dCartVel[0] = daCylVel[0]*cos(daCylPos[1]) - daCylPos[0]*sin(daCylPos[1])*daCylVel[1];
  dCartVel[1] = daCylVel[0]*sin(daCylPos[1]) + daCylPos[0]*cos(daCylPos[1])*daCylVel[1];
  dCartVel[2] = daCylVel[2];
}

/*
 * Functions for calculating orbital parameters assuming that the barycenter
 * of the binary is at the origin
 */

/* Calculate specific angular momentum 
 * h = r x v in cartesian coords
 */
void fvSpecificAngMom(double *r, double *v, double *h)
{
  // No return since resultant vector, h, is supplied
  cross(r,v,h); // r x v -> h
}

/* Calculate specific orbital energy
 * eps = v^2/2 - mu/|r|
 */
double fdSpecificOrbEng(BODY *body)
{
  // For binary, iBody 0, 1 == stars, 2 == planet
  double mu = BIGG*(body[0].dMass + body[1].dMass); // Gravitational parameter
  double r_norm = sqrt(fdDot(body[2].dCartPos,body[2].dCartPos));
  
  return fdDot(body[2].dCartVel,body[2].dCartVel)/2.0 - (mu/r_norm);
}

/* Compute a body's semimajor axis
 * a = -mu/(2*eps)
 */
double fdComputeSemi(BODY *body)
{
  // For binary, iBody 0, 1 == stars, 2 == planet
  return -BIGG*(body[0].dMass + body[1].dMass)/(2.0*fdSpecificOrbEng(body));
}

/* Compute a body's orbital eccentricity
 * e = sqrt(1 + 2*eps*h*h/(mu*mu))
 */
double fdComputeEcc(BODY *body)
{
  // For binary, iBody 0, 1 == stars, 2 == planet
  double mu = BIGG*(body[0].dMass + body[1].dMass); // Gravitational parameter
  double h[3];
  fvSpecificAngMom(body[2].dCartPos,body[2].dCartVel,h);
  
  return sqrt(1. + (2.*fdSpecificOrbEng(body)*fdDot(h,h))/(mu*mu));
}

/* Compute the mean anomaly M = n*t + phi where phi is an arbitrary offset */
double fdMeanAnomaly(double dMeanMotion, double dTime, double dPhi)
{
  return dMeanMotion * dTime + dPhi;
}

//Below are functions russell already defined in distorb that i'll reuse
//double fdLaplaceCoeff(double dAxRatio, int iIndexJ, double dIndexS)
//double fdDerivLaplaceCoeff(int iNthDeriv, double dAxRatio, int iIndexJ, double dIndexS)
// Note: for Laplace Coeff functions, they go as b^J_s(alpha) where J is an int, S is a half int double !!

/* 
 * Analytic equations from Leung+Lee 2013 that govern circumbinary planet (cbp) evolution
 * for 1 (!) cbp
 */

/* LL13 N0 */
double fdMeanMotion(BODY *body)
{
  // Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[0].dSemi*body[1].dMass/M)/body[2].dSemi;
  double alphab = (body[0].dSemi*body[0].dMass/M)/body[2].dSemi;

  double N0 = body[2].dMeanMotion*body[2].dMeanMotion/2.;

  double tmp1 = body[0].dMass*fdLaplaceCoeff(alphaa,0,0.5)/M;
  tmp1 += body[1].dMass*fdLaplaceCoeff(alphab,0,0.5)/M;

  double tmp2 = body[0].dMass*body[1].dMass*body[0].dSemi/(M*M*body[2].dSemi);
  tmp2 *= (fdDerivLaplaceCoeff(1,alphaa,0,0.5) + fdDerivLaplaceCoeff(1,alphab,0,0.5));

  return sqrt(N0*(tmp1 + tmp2));
}

/* LL13 K0 */
double fdEpiFreqK(BODY *body)
{
  //Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[0].dSemi*body[1].dMass/M)/body[2].dSemi;
  double alphab = (body[0].dSemi*body[0].dMass/M)/body[2].dSemi;
 
  double K0 = body[2].dMeanMotion*body[2].dMeanMotion/2.;

  double tmp1 = body[0].dMass*fdLaplaceCoeff(alphaa,0,0.5)/M;
  tmp1 += body[1].dMass*fdLaplaceCoeff(alphab,0,0.5)/M;

  double tmp2 = body[0].dMass*body[1].dMass*body[0].dSemi/(M*M*body[2].dSemi);
  tmp2 *= (fdDerivLaplaceCoeff(1,alphaa,0,0.5) + fdDerivLaplaceCoeff(1,alphab,0,0.5));

  double tmp3 = body[0].dMass*body[1].dMass*body[0].dSemi*body[0].dSemi/(M*M*body[2].dSemi*body[2].dSemi);

  double tmp4 = body[1].dMass*fdDerivLaplaceCoeff(2,alphaa,0,0.5)/M;
  tmp4 += body[0].dMass*fdDerivLaplaceCoeff(2,alphab,0,0.5)/M;
  tmp3 *= tmp4;

  K0 *= (tmp1 - tmp2 - tmp3);
  return sqrt(K0);
}

/* LL13 V0 */
double fdEpiFreqV(BODY *body)
{
  //Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[0].dSemi*body[1].dMass/M)/body[2].dSemi;
  double alphab = (body[0].dSemi*body[0].dMass/M)/body[2].dSemi;
     
  double V0 = body[2].dMeanMotion*body[2].dMeanMotion/2.;

  double tmp1 = body[0].dMass*fdLaplaceCoeff(alphaa,0,1.5)/M;
  tmp1 += body[1].dMass*fdLaplaceCoeff(alphab,0,1.5)/M;
  return sqrt(V0 * tmp1);
}

/* Circular (azimuthal) motion of the guiding center for a cbp: phi0 */
double fdPhi0(double dTime, double dMeanMotion, double dPsi)
{
  return dMeanMotion * dTime + dPsi;
}

/*
 * Functions to compute binary potentials and their derivatives w.r.t. radius
 */

/* LL13 Eqn 15: Binary potential component 0 */
double fdPot0(int j, int k, double R, BODY *body)
{
  //Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[0].dSemi*body[1].dMass/M)/R;
  double alphab = (body[0].dSemi*body[0].dMass/M)/R;

  double coeff = -(2. - fiDelta(k,0))/2.;
  coeff *= BIGG*(body[0].dMass + body[1].dMass)/R;

  double tmp1 = pow(-1.,k)*body[0].dMass*fdLaplaceCoeff(alphaa,k,(j+1.)/2.)/M;
  tmp1 += body[1].dMass*fdLaplaceCoeff(alphab,k,(j+1.)/2.)/M;

  return coeff*tmp1;

}

/* LL13 Eqn 15: d/dR of binary potential component 0 */
double fdPot0dR(int j, int k, double R, BODY *body)
{
  //Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[0].dSemi*body[1].dMass/M)/R;
  double alphab = (body[0].dSemi*body[0].dMass/M)/R;

  double coeff = -(2. - fiDelta(k,0))/2.;

  double f = pow(-1.,k)*body[0].dMass*fdLaplaceCoeff(alphaa,k,(j+1.)/2.)/M;
  f += body[1].dMass*fdLaplaceCoeff(alphab,k,(j+1.)/2.)/M;
  double g = BIGG*(body[0].dMass + body[1].dMass)/R;

  double g_prime = -g/R;
  double f_prime = -pow(-1.,k)*body[0].dMass*alphaa*R*fdDerivLaplaceCoeff(1,alphaa,k,(j+1.)/2.)/(R*R*M);
  f_prime += -body[1].dMass*alphab*R*fdDerivLaplaceCoeff(1,alphab,k,(j+1.)/2.)/(R*R*M);

  return coeff*(f_prime*g + g_prime*f);
}

/* LL13 eqn 16: Binary potential component 1 */
double fdPot1(int j, int k, double R, BODY *body)
{
  //Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[0].dSemi*body[1].dMass/M)/R;
  double alphab = (body[0].dSemi*body[0].dMass/M)/R;

  double coeff = -(2. - fiDelta(k,0))/2.;
  coeff *= BIGG*(body[0].dMass + body[1].dMass)/R;

  double tmp1 = pow(-1.,k)*body[0].dMass*alphaa*fdDerivLaplaceCoeff(1,alphaa,k,(j+1.)/2.)/M;
  tmp1 += body[1].dMass*alphab*fdDerivLaplaceCoeff(1,alphab,k,(j+1.)/2.)/M;

  return coeff*tmp1;
}

/* LL13 eqn 16: d/dR of binary potential component 1 */
double fdPot1dR(int j, int k, double R, BODY * body)
{
  //Define intermediate quantities
  double M = body[0].dMass + body[1].dMass;
  double alphaa = (body[0].dSemi*body[1].dMass/M)/R;
  double alphab = (body[0].dSemi*body[0].dMass/M)/R;

  double coeff = -(2. - fiDelta(k,0))/2.;

  // Break up big terms into multiple terms: x
  double f = pow(-1.,k)*body[0].dMass*alphaa/M;
  double g = fdDerivLaplaceCoeff(1,alphaa,k,(j+1.)/2.);
  double h = body[1].dMass*alphab/M;
  double i = fdDerivLaplaceCoeff(1,alphab,k,(j+1.)/2.);
  double l = BIGG*(body[0].dMass + body[1].dMass)/R;

  // Derivatives of the above terms w.r.t. R (and hence, alphas): xp
  double fp = -pow(-1.,k)*body[0].dMass*alphaa/(R*M);
  double gp = -alphaa*fdDerivLaplaceCoeff(2,alphaa,k,(j+1.)/2.)/R;
  double hp = -body[1].dMass*alphab/(R*M);
  double ip = -alphab*fdDerivLaplaceCoeff(2,alphab,k,(j+1.)/2.)/R;
  double lp = -BIGG*(body[0].dMass + body[1].dMass)/(R*R);

  return coeff*((fp*g + gp*f + hp*i + ip*h)*l + lp*(f*g + h*i));
}

/* LL13 function defined in paragraph in between eqn 24 and 25 which is like
 * the mean motion at R
 */
double fdn(double R, BODY *body)
{
  return sqrt(fdPot0dR(0,0,R,body)/R);
}
































