/********************** INTERIORTHERMAL.C **********************/
/*
 * Peter Driscoll 6/1/15
 *
 * Thermal evolution of the interior (mantle+core)
 *
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void  InitializeControlInteriorthermal(CONTROL *control) {
  /* Nothing for now, but this subroutine is necessary for module loops. */
}

void BodyCopyInteriorthermal(BODY *dest,BODY *src,int foo,int iBody) {
  dest[iBody].dTMan = src[iBody].dTMan;
  dest[iBody].dTCore = src[iBody].dTCore;
}

void InitializeBodyInteriorthermal(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
}

void InitializeUpdateTmpBodyInteriorthermal(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
}

/**************** RADHEAT options ********************/

/* Initial Mantle & Core Temperature */

void ReadTMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dTMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       body[iFile-1].dTMan = dTmp*fdUnitsTemp(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dTMan = options->dDefault;
}

void ReadTCore(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dTCore = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       body[iFile-1].dTCore = dTmp*fdUnitsTemp(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dTCore = options->dDefault;
}


/* Initiatlize Input Options */

void InitializeOptionsInteriorthermal(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  /* TMan */
  sprintf(options[OPT_TMAN].cName,"dTMan");
  sprintf(options[OPT_TMAN].cDescr,"Initial Mantle Temperature");
  sprintf(options[OPT_TMAN].cDefault,"Default=3000");
  options[OPT_TMAN].iType = 2;
  options[OPT_TMAN].iMultiFile = 1;
  options[OPT_TMAN].dNeg = 3000.0d;  //Not sure about this??
  options[OPT_TMAN].dDefault = 3000.0d; 
  sprintf(options[OPT_TMAN].cNeg,"Default=3000");
  fnRead[OPT_TMAN] = &ReadTMan;
   /* TCore */
  sprintf(options[OPT_TCORE].cName,"dTCore");
  sprintf(options[OPT_TCORE].cDescr,"Initial Core Temperature");
  sprintf(options[OPT_TCORE].cDefault,"Default=6000");
  options[OPT_TCORE].iType = 2;
  options[OPT_TCORE].iMultiFile = 1;
  options[OPT_TCORE].dNeg = 6000.0d;  //Not sure about this??
  options[OPT_TCORE].dDefault = 6000.0d; 
  sprintf(options[OPT_TCORE].cNeg,"Default=6000");
  fnRead[OPT_TCORE] = &ReadTCore;

}

void ReadOptionsInteriorthermal(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTINTERIORTHERMAL;iOpt<OPTENDINTERIORTHERMAL;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify INTERIORTHERMAL ******************/

void VerifyRotationInteriorthermal(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody) {
  /* Nothing */
}

/* Is this necessary? */
void AssignTMan(BODY *body,OPTIONS *options,double dAge,int iBody) {
    /* Mantle */
    /*    if (options[OPT_TMAN].iLine[iBody+1] >= 0) {
	body[iBody].d40KNumMan=body[iBody].d40KMassMan/(MASS40K);
	printf("40KMassMan set, body[iBody].d40KNumMan=%e, ENUMMAN40K=%e\n",body[iBody].d40KNumMan,ENUMMAN40K);
    }
    body[iBody].dTMan = fd40KConstant(body[iBody].d40KNumMan,dAge);  //Get the constant given num and age.
    */
}


void VerifyTMan(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
    //  AssignTMan(body,options,dAge,iBody);
    /* Mantle */
    update[iBody].iaType[update[iBody].iTMan][0] = 1; //iaType=0 for prescribed evolution, =1 for differential evolution (normal)
    update[iBody].iNumBodies[update[iBody].iTMan][0]=1;
    update[iBody].iaBody[update[iBody].iTMan][0] = malloc(update[iBody].iNumBodies[update[iBody].iTMan][0]*sizeof(int)); //iaBody is the number of bodies that are affected by this variable.
    update[iBody].iaBody[update[iBody].iTMan][0][0]=iBody;
    update[iBody].pdTDotMan = &update[iBody].daDerivProc[update[iBody].iTMan][0];
    fnUpdate[iBody][update[iBody].iTMan][0] = &fdTDotMan;
}

void VerifyTCore(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
    //  AssignTCore(body,options,dAge,iBody);
    /* Core */
    update[iBody].iaType[update[iBody].iTCore][0] = 1; //iaType=0 for prescribed evolution, =1 for differential evolution (normal)
    update[iBody].iNumBodies[update[iBody].iTCore][0]=1;
    update[iBody].iaBody[update[iBody].iTCore][0] = malloc(update[iBody].iNumBodies[update[iBody].iTCore][0]*sizeof(int)); //iaBody is the number of bodies that are affected by this variable.
    update[iBody].iaBody[update[iBody].iTCore][0][0]=iBody;
    update[iBody].pdTDotCore = &update[iBody].daDerivProc[update[iBody].iTCore][0];
    fnUpdate[iBody][update[iBody].iTCore][0] = &fdTDotCore;
}



void fnPropertiesInteriorthermal(BODY *body,int iBody) {
  /* Nothing */
}

void fnForceBehaviorInteriorthermal(BODY *body,EVOLVE *evolve,IO *io,int iBody,int iModule) {
  if (body[iBody].dTMan < 0.5)
    body[iBody].dTMan = 0;
  if (body[iBody].dTCore < 0.5)
    body[iBody].dTCore = 0;
}

void VerifyInteriorthermal(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  VerifyTMan(body,options,update,system->dAge,fnUpdate,iBody);  //Verify Man.
  VerifyTCore(body,options,update,system->dAge,fnUpdate,iBody);  //Verify Core.

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorInteriorthermal;
  control->Evolve.fnAuxProps[iBody][iModule] = &fnPropertiesInteriorthermal;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyInteriorthermal;
  //  output[OUT_TDOTMAN].fnOutput[iBody][iModule] = &fdTDotMan;
}


void InitializeModuleInteriorthermal(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** RADHEAT update ****************/

void InitializeUpdateInteriorthermal(BODY *body,UPDATE *update,int iBody) {
  /* Initially allow all radiogenic heat sources to be present. If any are 0, 
     or < dMinRadPower, they will me removed from update[iBody] in 
     ForceBehavior.
  */
  if (body[iBody].dTMan > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNumTMan++;  //Why is iNumTMan incremented here and below?
  }
  if (body[iBody].dTCore > 0) {
    update[iBody].iNumVars++;
    update[iBody].iNumTCore++;  
  }
}

void FinalizeUpdateTManInteriorthermal(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = TMAN;
  update[iBody].iNumTMan = (*iEqn)++;
}

void FinalizeUpdateTCoreInteriorthermal(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = TCORE;
  update[iBody].iNumTCore = (*iEqn)++;
}


/***************** RADHEAT Halts *****************/

/* Minimum TMan */
int fbHaltMinTMan(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  if (body[iBody].dTMan < halt->dMinTMan) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's TMan =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].dTMan,io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMinTMan,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}

int fbHaltMinTCore(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {
  if (body[iBody].dTCore < halt->dMinTCore) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's TCore =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].dTCore,io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMinTCore,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        

void CountHaltsInteriorthermal(HALT *halt,int *iHalt) {
  if (halt->dMinTMan >= 0)
    (iHalt)++;
  if (halt->dMinTCore >= 0)
    (iHalt)++;
}

void VerifyHaltInteriorthermal(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
  if (control->Halt[iBody].dMinTMan > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMinTMan;
  if (control->Halt[iBody].dMinTCore > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMinTCore;
}

/************* RADHEAT Outputs ******************/

void HelpOutputInteriorthermal(OUTPUT *output) {
  int iOut;

  printf("\n ------ INTERIORTHERMAL output ------\n");
  for (iOut=OUTSTARTINTERIORTHERMAL;iOut<OUTENDINTERIORTHERMAL;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

/* NOTE: If you write a new Write subroutine here you need to add the associate 
   block of initialization in InitializeOutputInteriorthermal below */

void WriteTMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TMan */
  *dTmp = body[iBody].dTMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { 
      /*      *dTmp /= fdUnitsTemp(body[iBody].dTman,0,units->iTemp);  //set "iOldType" to 0, second input var, arbitarily.
    fsUnitsTemp(units->iTemp,cUnit);
      */
  }
}

void WriteTCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TCore */
  *dTmp = body[iBody].dTCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { 
      /*      *dTmp /= fdUnitsTemp(body[iBody].dTman,0,units->iTemp);  //set "iOldType" to 0, second input var, arbitarily.
    fsUnitsTemp(units->iTemp,cUnit);
      */
  }
}

void WriteTDotMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TDotMan */
  *dTmp = (*(update[iBody].pdTDotMan));
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
      /*
      *dTmp /= fdUnitsTemp(body[iBody].dTman,0,units->iTemp)/fdUnitsTime(units->iTime);
      fsUnitsTempRate(units->iTemp,cUnit);  // only handles Temp/s, need to add yr and Gyr options.
      */
  }
}

void WriteTDotCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TDotCore */
  *dTmp = (*(update[iBody].pdTDotCore));
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else {
      /*
      *dTmp /= fdUnitsTemp(body[iBody].dTman,0,units->iTemp)/fdUnitsTime(units->iTime);
      fsUnitsTempRate(units->iTemp,cUnit);  // only handles Temp/s, need to add yr and Gyr options.
      */
  }
}

void InitializeOutputInteriorthermal(OUTPUT *output,fnWriteOutput fnWrite[]) {
  sprintf(output[OUT_TMAN].cName,"TMan");
  sprintf(output[OUT_TMAN].cDescr,"Mantle Temperature");
  sprintf(output[OUT_TMAN].cNeg,"K");
  output[OUT_TMAN].bNeg = 1;
  output[OUT_TMAN].dNeg = 1; // default units are K. 
  output[OUT_TMAN].iNum = 1;
  fnWrite[OUT_TMAN] = &WriteTMan;

  sprintf(output[OUT_TCORE].cName,"TCore");
  sprintf(output[OUT_TCORE].cDescr,"Core Temperature");
  sprintf(output[OUT_TCORE].cNeg,"K");
  output[OUT_TCORE].bNeg = 1;
  output[OUT_TCORE].dNeg = 1; // default units are K. 
  output[OUT_TCORE].iNum = 1;
  fnWrite[OUT_TCORE] = &WriteTCore;

  sprintf(output[OUT_TDOTMAN].cName,"TDotMan");
  sprintf(output[OUT_TDOTMAN].cDescr,"Change in Mantle Temperature");
  sprintf(output[OUT_TDOTMAN].cNeg,"K/s");
  output[OUT_TDOTMAN].bNeg = 1;
  output[OUT_TDOTMAN].dNeg = 1; // default units are K. 
  output[OUT_TDOTMAN].iNum = 1;
  fnWrite[OUT_TDOTMAN] = &WriteTDotMan;
  
  sprintf(output[OUT_TDOTCORE].cName,"TDotCore");
  sprintf(output[OUT_TDOTCORE].cDescr,"Change in Core Temperature");
  sprintf(output[OUT_TDOTCORE].cNeg,"K/s");
  output[OUT_TDOTCORE].bNeg = 1;
  output[OUT_TDOTCORE].dNeg = 1; // default units are K. 
  output[OUT_TDOTCORE].iNum = 1;
  fnWrite[OUT_TDOTCORE] = &WriteTDotCore;
}

void FinalizeOutputFunctionInteriorthermal(OUTPUT *output,int iBody,int iModule) {
  //  output[OUT_TDOTMAN].fnOutput[iBody][iModule] = &fdTDotMan;
    output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxRadheat; //This is need to print the global var to log.  Needs to be fixed.
}

/************ INTERIORTHERMAL Logging Functions **************/

void LogOptionsInteriorthermal(CONTROL *control, FILE *fp) {
  /* Anything here?
  fprintf(fp,"-------- RADHEAT Options -----\n\n");
  */
}

//PED: this would be for global rad heat parameters, but this is blank bc rad is only relevant to each individual body.
void LogInteriorthermal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  /* Anything here? 
  int iOut;
  fprintf(fp,"\n----- RADHEAT PARAMETERS ------\n");
  for (iOut=OUTSTARTRADHEAT;iOut<OUTBODYSTARTRADHEAT;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyInteriorthermal(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- INTERIORTHERMAL PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTSTARTINTERIORTHERMAL;iOut<OUTENDINTERIORTHERMAL;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
    /*
    fprintf(fp,"40K Constant: ");
    fprintd(fp,body[iBody].d40KConst,control->Io.iSciNot,control->Io.iDigits);
    fprintf(fp,"\n");
    */
  }
}

void AddModuleInteriorthermal(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = INTERIORTHERMAL;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlInteriorthermal;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyInteriorthermal;

  module->fnCountHalts[iBody][iModule] = &CountHaltsRadHeat;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsInteriorthermal;
  module->fnLogBody[iBody][iModule] = &LogBodyInteriorthermal;
  module->fnVerify[iBody][iModule] = &VerifyInteriorthermal;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltInteriorthermal;
  module->fnVerifyRotation[iBody][iModule] = &VerifyRotationInteriorthermal;
  
  module->fnInitializeBody[iBody][iModule] = &InitializeBodyInteriorthermal;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateInteriorthermal;

    // NEED TO ADD INTERIORTHERMAL VARIABLES HERE??
  module->fnFinalizeUpdateTMan[iBody][iModule] = &FinalizeUpdateTManInteriorthermal;
  module->fnFinalizeUpdateTCore[iBody][iModule] = &FinalizeUpdateTCoreInteriorthermal;

  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionInteriorthermal;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionInteriorthermal;

}

/************* INTERIORTHERMAL Functions ************/

/* Get TUMan */
double fdTUMan(BODY *body,SYSTEM *system,int *iaBody,int TMan) {
    return ADJUMP2UM*TMan;
}

/* Get TLMan */
double fdTUMan(BODY *body,SYSTEM *system,int *iaBody,int TMan) {
    return ADJUMP2LM*TMan;
}

/* Get TUMan */
double fdTUMan(BODY *body,SYSTEM *system,int *iaBody,int TMan) {
    return ADJUMP2UM*TMan;
}

/* Get TDotMan */
double fdTDotMan(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
    return -(100.0/1e9/YEARSEC);   //arbitrary for now.
}

/* Get TDotCore */
double fdTDotCore(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
    return -(100.0/1e9/YEARSEC);   //arbitrary for now.
}

