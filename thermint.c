/********************** THERMINT.C **********************/
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

void  InitializeControlThermint(CONTROL *control) {
  /* Nothing for now, but this subroutine is necessary for module loops. */
}

void BodyCopyThermint(BODY *dest,BODY *src,int foo,int iBody) {
  dest[iBody].dTMan = src[iBody].dTMan;
  dest[iBody].dTCore = src[iBody].dTCore;
  /* Constants */
  dest[iBody].dViscRatioMan = src[iBody].dViscRatioMan;
  dest[iBody].dViscRef = src[iBody].dViscRef;
  dest[iBody].dTrefLind = src[iBody].dTrefLind;
  dest[iBody].dEruptEff = src[iBody].dEruptEff;
  /* Aux Props Variables */
  dest[iBody].dTUMan=src[iBody].dTUMan;
  dest[iBody].dTLMan=src[iBody].dTLMan;
  dest[iBody].dTCMB=src[iBody].dTCMB;
  dest[iBody].dTJumpUMan=src[iBody].dTJumpUMan;
  dest[iBody].dTJumpLMan=src[iBody].dTJumpLMan;
  dest[iBody].dSignTJumpUMan=src[iBody].dSignTJumpUMan;
  dest[iBody].dSignTJumpLMan=src[iBody].dSignTJumpLMan;
  dest[iBody].dViscUManArr=src[iBody].dViscUManArr;
  dest[iBody].dViscUMan=src[iBody].dViscUMan;
  dest[iBody].dViscLMan=src[iBody].dViscLMan;
  dest[iBody].dBLUMan=src[iBody].dBLUMan;
  dest[iBody].dBLLMan=src[iBody].dBLLMan;
  dest[iBody].dShmodUMan=src[iBody].dShmodUMan;
  dest[iBody].dTsolUMan=src[iBody].dTsolUMan;
  dest[iBody].dTsolLMan=src[iBody].dTsolLMan;
  dest[iBody].dTliqUMan=src[iBody].dTliqUMan;
  dest[iBody].dTliqLMan=src[iBody].dTliqLMan;
  dest[iBody].dFMeltUMan=src[iBody].dFMeltUMan;
  dest[iBody].dFMeltLMan=src[iBody].dFMeltLMan;
  dest[iBody].dMeltfactorUMan=src[iBody].dMeltfactorUMan;
  dest[iBody].dMeltfactorLMan=src[iBody].dMeltfactorLMan;
  dest[iBody].dDepthMeltMan=src[iBody].dDepthMeltMan;
  dest[iBody].dTDepthMeltMan=src[iBody].dTDepthMeltMan;
  dest[iBody].dTJumpMeltMan=src[iBody].dTJumpMeltMan;
  /* Tides */
  dest[iBody].dK2Man=src[iBody].dK2Man;
  dest[iBody].dImk2Man=src[iBody].dImk2Man;
  /* Heat Flows */
  /* Mantle */
  dest[iBody].dHfluxUMan=src[iBody].dHfluxUMan;
  dest[iBody].dHfluxLMan=src[iBody].dHfluxLMan;
  dest[iBody].dHfluxCMB=src[iBody].dHfluxCMB;
  dest[iBody].dHflowUMan=src[iBody].dHflowUMan;
  dest[iBody].dHflowLMan=src[iBody].dHflowLMan;
  dest[iBody].dHflowCMB=src[iBody].dHflowCMB;
  dest[iBody].dTidalPowMan=src[iBody].dTidalPowMan;
  dest[iBody].dHflowLatentMan=src[iBody].dHflowLatentMan;
  dest[iBody].dHflowMeltMan=src[iBody].dHflowMeltMan;
  /* Core */
  dest[iBody].dRIC=src[iBody].dRIC;
  dest[iBody].dDRICDTCMB=src[iBody].dDRICDTCMB;
  dest[iBody].dMassICDot=src[iBody].dMassICDot;
  dest[iBody].dHflowLatentIC=src[iBody].dHflowLatentIC;
  dest[iBody].dPowerGravIC=src[iBody].dPowerGravIC;
}

void InitializeBodyThermint(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
}

void InitializeUpdateTmpBodyThermint(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
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
      body[iFile-1].dTMan = fdUnitsTemp(dTmp,control->Units[iFile].iTemp,0);
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
       body[iFile-1].dTCore = fdUnitsTemp(dTmp,control->Units[iFile].iTemp,0);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dTCore = options->dDefault;
}
void ReadViscRatioMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dViscRatioMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dViscRatioMan = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dViscRatioMan = options->dDefault;
}
void ReadViscRef(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dViscRef = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dViscRef = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dViscRef = options->dDefault;
}

void ReadTrefLind(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dTrefLind = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dTrefLind = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dTrefLind = options->dDefault;
}

void ReadEruptEff(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;
  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].dEruptEff = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
     body[iFile-1].dEruptEff = dTmp;  //no units.
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].dEruptEff = options->dDefault;
}
/* Initiatlize Input Options */

void InitializeOptionsThermint(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  /* TMan */
  sprintf(options[OPT_TMAN].cName,"dTMan");
  sprintf(options[OPT_TMAN].cDescr,"Initial Mantle Temperature");
  sprintf(options[OPT_TMAN].cDefault,"Default=3000");
  options[OPT_TMAN].iType = 2;
  options[OPT_TMAN].iMultiFile = 1;
  options[OPT_TMAN].dNeg = 3000.0;  //Not sure about this??
  options[OPT_TMAN].dDefault = 3000.0; 
  sprintf(options[OPT_TMAN].cNeg,"Default=3000");
  fnRead[OPT_TMAN] = &ReadTMan;
   /* TCore */
  sprintf(options[OPT_TCORE].cName,"dTCore");
  sprintf(options[OPT_TCORE].cDescr,"Initial Core Temperature");
  sprintf(options[OPT_TCORE].cDefault,"Default=6000");
  options[OPT_TCORE].iType = 2;
  options[OPT_TCORE].iMultiFile = 1;
  options[OPT_TCORE].dNeg = 6000.0;  //Not sure about this??
  options[OPT_TCORE].dDefault = 6000.0; 
  sprintf(options[OPT_TCORE].cNeg,"Default=6000");
  fnRead[OPT_TCORE] = &ReadTCore;
   /* ViscRatioMan */
  sprintf(options[OPT_VISCRATIOMAN].cName,"dViscRatioMan");
  sprintf(options[OPT_VISCRATIOMAN].cDescr,"ViscRatioMan");
  sprintf(options[OPT_VISCRATIOMAN].cDefault,"Default is VISCJUMPULM");
  options[OPT_VISCRATIOMAN].iType = 2;
  options[OPT_VISCRATIOMAN].iMultiFile = 1;
  options[OPT_VISCRATIOMAN].dNeg = VISCJUMPULM;
  options[OPT_VISCRATIOMAN].dDefault = VISCJUMPULM; 
  sprintf(options[OPT_VISCRATIOMAN].cNeg,"Default");
  fnRead[OPT_VISCRATIOMAN] = &ReadViscRatioMan;
   /* ViscRef */
  sprintf(options[OPT_VISCREF].cName,"dViscRef");
  sprintf(options[OPT_VISCREF].cDescr,"ViscRef");
  sprintf(options[OPT_VISCREF].cDefault,"Default=1");
  options[OPT_VISCREF].iType = 2;
  options[OPT_VISCREF].iMultiFile = 1;
  options[OPT_VISCREF].dNeg = VISCREF;
  options[OPT_VISCREF].dDefault = VISCREF; 
  sprintf(options[OPT_VISCREF].cNeg,"Default value in thermint.h");
  fnRead[OPT_VISCREF] = &ReadViscRef;
   /* TrefLind */
  sprintf(options[OPT_TREFLIND].cName,"dTrefLind");
  sprintf(options[OPT_TREFLIND].cDescr,"TrefLind");
  sprintf(options[OPT_TREFLIND].cDefault,"Default in thermint.h");
  options[OPT_TREFLIND].iType = 2;
  options[OPT_TREFLIND].iMultiFile = 1;
  options[OPT_TREFLIND].dNeg = 1;
  options[OPT_TREFLIND].dDefault = TREFLIND; 
  sprintf(options[OPT_TREFLIND].cNeg,"Default in thermint.h");
  fnRead[OPT_TREFLIND] = &ReadTrefLind;
  /* EruptEff */
  sprintf(options[OPT_ERUPTEFF].cName,"dEruptEff");
  sprintf(options[OPT_ERUPTEFF].cDescr,"Melt Eruption Efficiency");
  sprintf(options[OPT_ERUPTEFF].cDefault,"Default is ERUPTEFF");
  options[OPT_ERUPTEFF].iType = 2;
  options[OPT_ERUPTEFF].iMultiFile = 1;
  options[OPT_ERUPTEFF].dNeg = ERUPTEFF;
  options[OPT_ERUPTEFF].dDefault = ERUPTEFF; 
  sprintf(options[OPT_ERUPTEFF].cNeg,"Default is ERUPTEFF");
  fnRead[OPT_ERUPTEFF] = &ReadEruptEff;

}

void ReadOptionsThermint(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTTHERMINT;iOpt<OPTENDTHERMINT;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify THERMINT ******************/

void VerifyRotationThermint(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody) {
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



/******************************  AUX PROPS  ***********************************************/
/* Auxiliary Properties */
void PropsAuxThermint(BODY *body,UPDATE *update,int iBody) {
  /* Scalar Properties */
  body[iBody].dTUMan=fdTUMan(body,iBody);
  body[iBody].dTLMan=fdTLMan(body,iBody);
  body[iBody].dTCMB=fdTCMB(body,iBody);
  body[iBody].dTJumpUMan=fdTJumpUMan(body,iBody);
  body[iBody].dTJumpLMan=fdTJumpLMan(body,iBody);
  body[iBody].dSignTJumpUMan=fdSignTJumpUMan(body,iBody);
  body[iBody].dSignTJumpLMan=fdSignTJumpLMan(body,iBody);
  //  body[iBody].dViscRatioMan=fdViscRatioMan(body,iBody);
  if (body[iBody].dMeltfactorUMan==0)
      body[iBody].dMeltfactorUMan=1.;  //initialize to avoid visc=visc/meltfactor crash.
  if (body[iBody].dMeltfactorUMan==0)
      body[iBody].dMeltfactorUMan=1.;  //initialize.
  /* Loop through melt calculation once to get dependence of visc on melt. */
  int i=0, nloop=1;
  for (i=0;i<nloop;i++) {
      body[iBody].dViscUManArr=fdViscUManArr(body,iBody);
      body[iBody].dViscUMan=fdViscUMan(body,iBody);
      body[iBody].dViscLMan=fdViscLMan(body,iBody);
      body[iBody].dBLUMan=fdBLUMan(body,iBody);
      body[iBody].dBLLMan=fdBLLMan(body,iBody);
      body[iBody].dShmodUMan=fdShmodUMan(body,iBody);
      body[iBody].dTsolUMan=fdTsolUMan(body,iBody);
      body[iBody].dTliqUMan=fdTliqUMan(body,iBody);
      body[iBody].dFMeltUMan=fdFMeltUMan(body,iBody);
      body[iBody].dMeltfactorUMan=fdMeltfactorUMan(body,iBody);
      body[iBody].dTsolLMan=fdTsolLMan(body,iBody);
      body[iBody].dTliqLMan=fdTliqLMan(body,iBody);
      body[iBody].dFMeltLMan=fdFMeltLMan(body,iBody);
      body[iBody].dMeltfactorLMan=fdMeltfactorLMan(body,iBody);
  }
  body[iBody].dDepthMeltMan=fdDepthMeltMan(body,iBody);
  body[iBody].dTDepthMeltMan=fdTDepthMeltMan(body,iBody);
  body[iBody].dTJumpMeltMan=fdTJumpMeltMan(body,iBody);

  /* Heat Flows */
  /* Mantle */
  body[iBody].dHfluxUMan=fdHfluxUMan(body,iBody);
  body[iBody].dHfluxLMan=fdHfluxLMan(body,iBody);
  body[iBody].dHfluxCMB=fdHfluxCMB(body,iBody);
  body[iBody].dHflowUMan=fdHflowUMan(body,iBody);
  body[iBody].dHflowLMan=fdHflowLMan(body,iBody);
  body[iBody].dHflowCMB=fdHflowCMB(body,iBody);
  body[iBody].dHflowLatentMan=fdHflowLatentMan(body,update,iBody);
  body[iBody].dHflowMeltMan=fdHflowMeltMan(body,iBody);
  /* Core */
  body[iBody].dRIC=fdRIC(body,iBody);
  body[iBody].dDRICDTCMB=fdDRICDTCMB(body,iBody);
  body[iBody].dMassICDot=fdMassICDot(body,update,iBody);
  body[iBody].dHflowLatentIC=fdHflowLatentIC(body,update,iBody);
  body[iBody].dPowerGravIC=fdPowerGravIC(body,update,iBody);
}

void fnForceBehaviorThermint(BODY *body,EVOLVE *evolve,IO *io,SYSTEM *system,UPDATE *update,int iBody,int iModule) {
  if (body[iBody].dTMan < 0.5)
    body[iBody].dTMan = 0;
  if (body[iBody].dTCore < 0.5)
    body[iBody].dTCore = 0;
}

void VerifyThermint(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  VerifyTMan(body,options,update,body[iBody].dAge,fnUpdate,iBody);  //Verify Man.
  VerifyTCore(body,options,update,body[iBody].dAge,fnUpdate,iBody);  //Verify Core.

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorThermint;
  control->Evolve.fnPropsAux[iBody][iModule] = &PropsAuxThermint;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyThermint;
  //  output[OUT_TDOTMAN].fnOutput[iBody][iModule] = &fdTDotMan;
}


void InitializeModuleThermint(CONTROL *control,MODULE *module) {
  /* Anything Here? */
}

/**************** RADHEAT update ****************/

void InitializeUpdateThermint(BODY *body,UPDATE *update,int iBody) {
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

void FinalizeUpdateTManThermint(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
  update[iBody].iaModule[iVar][*iEqn] = TMAN;
  update[iBody].iNumTMan = (*iEqn)++;
}

void FinalizeUpdateTCoreThermint(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody,int iFoo) {
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

void CountHaltsThermint(HALT *halt,int *iHalt) {
  if (halt->dMinTMan >= 0)
    (iHalt)++;
  if (halt->dMinTCore >= 0)
    (iHalt)++;
}

void VerifyHaltThermint(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
  if (control->Halt[iBody].dMinTMan > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMinTMan;
  if (control->Halt[iBody].dMinTCore > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMinTCore;
}

/************* THERMINT Outputs ******************/

void HelpOutputThermint(OUTPUT *output) {
  int iOut;

  printf("\n ------ THERMINT output ------\n");
  for (iOut=OUTSTARTTHERMINT;iOut<OUTENDTHERMINT;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

/* NOTE: If you write a new Write subroutine here you need to add the associate 
   block of initialization in InitializeOutputThermint below */

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
void WriteTUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TUMan */
    *dTmp = body[iBody].dTUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dTLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTJumpUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dTJumpUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTJumpLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dTJumpLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteSignTJumpUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dSignTJumpUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteSignTJumpLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dSignTJumpLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteTCMB(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TCore */
  *dTmp = body[iBody].dTCMB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTCore(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TCore */
  *dTmp = body[iBody].dTCore;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

void WriteViscUManArr(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get ViscUManArr */
    *dTmp = body[iBody].dViscUManArr;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteViscUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get ViscUMan */
    *dTmp = body[iBody].dViscUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteViscLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dViscLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteViscRatioMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get TLMan */
    *dTmp = body[iBody].dViscRatioMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteBLUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dBLUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteBLLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dBLLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteShmodUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dShmodUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteFMeltUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dFMeltUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteFMeltLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dFMeltLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteMeltfactorUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dMeltfactorUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteMeltfactorLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dMeltfactorLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteDepthMeltMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dDepthMeltMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTDepthMeltMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dTDepthMeltMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTJumpMeltMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dTJumpMeltMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteEruptEff(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dEruptEff;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteK2Man(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dK2Man;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteImk2Man(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  *dTmp = body[iBody].dImk2Man;
  strcpy(cUnit,"");
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteRIC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dRIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteDRICDTCMB(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dDRICDTCMB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}

/* Heat Flows/Fluxes */
void WriteHfluxUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHfluxUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHfluxLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHfluxLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHfluxCMB(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHfluxCMB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHflowUMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowUMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHflowLMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowLMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHflowCMB(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowCMB;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHflowLatentMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowLatentMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHflowMeltMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowMeltMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteTidalPowMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dTidalPowMan;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WriteHflowLatentIC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dHflowLatentIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}
void WritePowerGravIC(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dPowerGravIC;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
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
/** Write Constants. */
void WriteTrefLind(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
    *dTmp = body[iBody].dTrefLind;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { }
}



void InitializeOutputThermint(OUTPUT *output,fnWriteOutput fnWrite[]) {
  /* TMan */  
  sprintf(output[OUT_TMAN].cName,"TMan");
  sprintf(output[OUT_TMAN].cDescr,"Mantle Temperature");
  sprintf(output[OUT_TMAN].cNeg,"K");
  output[OUT_TMAN].bNeg = 1;
  output[OUT_TMAN].dNeg = 1; // default units are K. 
  output[OUT_TMAN].iNum = 1;
  fnWrite[OUT_TMAN] = &WriteTMan;
  /* TUMan */  
  sprintf(output[OUT_TUMAN].cName,"TUMan");
  sprintf(output[OUT_TUMAN].cDescr,"Upper Mantle Temperature");
  sprintf(output[OUT_TUMAN].cNeg,"K");
  output[OUT_TUMAN].bNeg = 1;
  output[OUT_TUMAN].dNeg = 1; // default units are K. 
  output[OUT_TUMAN].iNum = 1;
  fnWrite[OUT_TUMAN] = &WriteTUMan;
  /* TLMan */  
  sprintf(output[OUT_TLMAN].cName,"TLMan");
  sprintf(output[OUT_TLMAN].cDescr,"Lower Mantle Temperature");
  sprintf(output[OUT_TLMAN].cNeg,"K");
  output[OUT_TLMAN].bNeg = 1;
  output[OUT_TLMAN].dNeg = 1; // default units are K. 
  output[OUT_TLMAN].iNum = 1;
  fnWrite[OUT_TLMAN] = &WriteTLMan;
  /* TJumpUMan */
  sprintf(output[OUT_TJUMPUMAN].cName,"TJumpUMan");
  sprintf(output[OUT_TJUMPUMAN].cDescr,"Upper Mantle Temperature Jump");
  sprintf(output[OUT_TJUMPUMAN].cNeg,"K");
  output[OUT_TJUMPUMAN].bNeg = 1;
  output[OUT_TJUMPUMAN].dNeg = 1; // default units are K. 
  output[OUT_TJUMPUMAN].iNum = 1;
  fnWrite[OUT_TJUMPUMAN] = &WriteTJumpUMan;
  /* TJumpLMan */
  sprintf(output[OUT_TJUMPLMAN].cName,"TJumpLMan");
  sprintf(output[OUT_TJUMPLMAN].cDescr,"Lower Mantle Temperature Jump");
  sprintf(output[OUT_TJUMPLMAN].cNeg,"K");
  output[OUT_TJUMPLMAN].bNeg = 1;
  output[OUT_TJUMPLMAN].dNeg = 1; // default units are K. 
  output[OUT_TJUMPLMAN].iNum = 1;
  fnWrite[OUT_TJUMPLMAN] = &WriteTJumpLMan;
  /* SignTJumpUMan */
  sprintf(output[OUT_SIGNTJUMPUMAN].cName,"SignTJumpUMan");
  sprintf(output[OUT_SIGNTJUMPUMAN].cDescr,"Sign of Upper Mantle Temperature Jump");
  sprintf(output[OUT_SIGNTJUMPUMAN].cNeg,"K");
  output[OUT_SIGNTJUMPUMAN].bNeg = 1;
  output[OUT_SIGNTJUMPUMAN].dNeg = 1; // default units are K. 
  output[OUT_SIGNTJUMPUMAN].iNum = 1;
  fnWrite[OUT_SIGNTJUMPUMAN] = &WriteSignTJumpUMan;
  /* SignTJumpLMan */
  sprintf(output[OUT_SIGNTJUMPLMAN].cName,"SignTJumpLMan");
  sprintf(output[OUT_SIGNTJUMPLMAN].cDescr,"Sign of Lower Mantle Temperature Jump");
  sprintf(output[OUT_SIGNTJUMPLMAN].cNeg,"K");
  output[OUT_SIGNTJUMPLMAN].bNeg = 1;
  output[OUT_SIGNTJUMPLMAN].dNeg = 1; // default units are K. 
  output[OUT_SIGNTJUMPLMAN].iNum = 1;
  fnWrite[OUT_SIGNTJUMPLMAN] = &WriteSignTJumpLMan;
  /* TCMB */    
  sprintf(output[OUT_TCMB].cName,"TCMB");
  sprintf(output[OUT_TCMB].cDescr,"CMB Temperature");
  sprintf(output[OUT_TCMB].cNeg,"K");
  output[OUT_TCMB].bNeg = 1;
  output[OUT_TCMB].dNeg = 1; // default units are K. 
  output[OUT_TCMB].iNum = 1;
  fnWrite[OUT_TCMB] = &WriteTCMB;
  /* TCore */  
  sprintf(output[OUT_TCORE].cName,"TCore");
  sprintf(output[OUT_TCORE].cDescr,"Core Temperature");
  sprintf(output[OUT_TCORE].cNeg,"K");
  output[OUT_TCORE].bNeg = 1;
  output[OUT_TCORE].dNeg = 1; // default units are K. 
  output[OUT_TCORE].iNum = 1;
  fnWrite[OUT_TCORE] = &WriteTCore;
  /* ViscUman */
  sprintf(output[OUT_VISCUMAN].cName,"ViscUMan");
  sprintf(output[OUT_VISCUMAN].cDescr,"Upper Mantle Viscosity");
  sprintf(output[OUT_VISCUMAN].cNeg,"m^2/s");
  output[OUT_VISCUMAN].bNeg = 1;
  output[OUT_VISCUMAN].dNeg = 1; 
  output[OUT_VISCUMAN].iNum = 1;
  fnWrite[OUT_VISCUMAN] = &WriteViscUMan;
  /* ViscUManArr Arrhenius Only */
  sprintf(output[OUT_VISCUMANARR].cName,"ViscUManArr");
  sprintf(output[OUT_VISCUMANARR].cDescr,"Upper Mantle Arrhenius Viscosity");
  sprintf(output[OUT_VISCUMANARR].cNeg,"m^2/s");
  output[OUT_VISCUMANARR].bNeg = 1;
  output[OUT_VISCUMANARR].dNeg = 1; 
  output[OUT_VISCUMANARR].iNum = 1;
  fnWrite[OUT_VISCUMANARR] = &WriteViscUManArr;
  /* ViscLMan */
  sprintf(output[OUT_VISCLMAN].cName,"ViscLMan");
  sprintf(output[OUT_VISCLMAN].cDescr,"Lower Mantle Viscosity");
  sprintf(output[OUT_VISCLMAN].cNeg,"m^2/s");
  output[OUT_VISCLMAN].bNeg = 1;
  output[OUT_VISCLMAN].dNeg = 1; 
  output[OUT_VISCLMAN].iNum = 1;
  fnWrite[OUT_VISCLMAN] = &WriteViscLMan;
  /* ViscRatioMan */
  sprintf(output[OUT_VISCRATIOMAN].cName,"ViscRatioMan");
  sprintf(output[OUT_VISCRATIOMAN].cDescr,"Mantle Viscosity Ratio");
  sprintf(output[OUT_VISCRATIOMAN].cNeg,"nd");
  output[OUT_VISCRATIOMAN].bNeg = 1;
  output[OUT_VISCRATIOMAN].dNeg = 1; 
  output[OUT_VISCRATIOMAN].iNum = 1;
  fnWrite[OUT_VISCRATIOMAN] = &WriteViscRatioMan;
  /* BLUMan */
  sprintf(output[OUT_BLUMAN].cName,"BLUMan");
  sprintf(output[OUT_BLUMAN].cDescr,"Boundary Layer Thickness Upper Mantle");
  sprintf(output[OUT_BLUMAN].cNeg,"km");
  output[OUT_BLUMAN].bNeg = 1;
  output[OUT_BLUMAN].dNeg = 1;  //KM; 
  output[OUT_BLUMAN].iNum = 1;
  fnWrite[OUT_BLUMAN] = &WriteBLUMan;
  /* BLLMan */
  sprintf(output[OUT_BLLMAN].cName,"BLLMan");
  sprintf(output[OUT_BLLMAN].cDescr,"Boundary Layer Thickness Lower Mantle");
  sprintf(output[OUT_BLLMAN].cNeg,"km");
  output[OUT_BLLMAN].bNeg = 1;
  output[OUT_BLLMAN].dNeg = 1; 
  output[OUT_BLLMAN].iNum = 1;
  fnWrite[OUT_BLLMAN] = &WriteBLLMan;
  /* ShmodUMan */
  sprintf(output[OUT_SHMODUMAN].cName,"ShmodUMan");
  sprintf(output[OUT_SHMODUMAN].cDescr,"Shear Modulus Upper Mantle");
  sprintf(output[OUT_SHMODUMAN].cNeg,"Pa");
  output[OUT_SHMODUMAN].bNeg = 1;
  output[OUT_SHMODUMAN].dNeg = 1; 
  output[OUT_SHMODUMAN].iNum = 1;
  fnWrite[OUT_SHMODUMAN] = &WriteShmodUMan;
  /* FMeltUMan */
  sprintf(output[OUT_FMELTUMAN].cName,"FMeltUMan");
  sprintf(output[OUT_FMELTUMAN].cDescr,"Melt Fraction Upper Mantle");
  sprintf(output[OUT_FMELTUMAN].cNeg,"nd");
  output[OUT_FMELTUMAN].bNeg = 1;
  output[OUT_FMELTUMAN].dNeg = 1; 
  output[OUT_FMELTUMAN].iNum = 1;
  fnWrite[OUT_FMELTUMAN] = &WriteFMeltUMan;
  /* FMeltLMan */
  sprintf(output[OUT_FMELTLMAN].cName,"FMeltLMan");
  sprintf(output[OUT_FMELTLMAN].cDescr,"Melt Fraction Lower Mantle");
  sprintf(output[OUT_FMELTLMAN].cNeg,"nd");
  output[OUT_FMELTLMAN].bNeg = 1;
  output[OUT_FMELTLMAN].dNeg = 1; 
  output[OUT_FMELTLMAN].iNum = 1;
  fnWrite[OUT_FMELTLMAN] = &WriteFMeltLMan;
  /* MeltfactorUMan */
  sprintf(output[OUT_MELTFACTORUMAN].cName,"MeltfactorUMan");
  sprintf(output[OUT_MELTFACTORUMAN].cDescr,"Melt Factor Upper Mantle");
  sprintf(output[OUT_MELTFACTORUMAN].cNeg,"nd");
  output[OUT_MELTFACTORUMAN].bNeg = 1;
  output[OUT_MELTFACTORUMAN].dNeg = 1; 
  output[OUT_MELTFACTORUMAN].iNum = 1;
  fnWrite[OUT_MELTFACTORUMAN] = &WriteMeltfactorUMan;
  /* MeltfactorLMan */
  sprintf(output[OUT_MELTFACTORLMAN].cName,"MeltfactorLMan");
  sprintf(output[OUT_MELTFACTORLMAN].cDescr,"Melt Factor Lower Mantle");
  sprintf(output[OUT_MELTFACTORLMAN].cNeg,"nd");
  output[OUT_MELTFACTORLMAN].bNeg = 1;
  output[OUT_MELTFACTORLMAN].dNeg = 1; 
  output[OUT_MELTFACTORLMAN].iNum = 1;
  fnWrite[OUT_MELTFACTORLMAN] = &WriteMeltfactorLMan;
  /* DepthMeltMan */
  sprintf(output[OUT_DEPTHMELTMAN].cName,"DepthMeltMan");
  sprintf(output[OUT_DEPTHMELTMAN].cDescr,"Depth to base of UM Melt Region");
  sprintf(output[OUT_DEPTHMELTMAN].cNeg,"m");
  output[OUT_DEPTHMELTMAN].bNeg = 1;
  output[OUT_DEPTHMELTMAN].dNeg = 1; 
  output[OUT_DEPTHMELTMAN].iNum = 1;
  fnWrite[OUT_DEPTHMELTMAN] = &WriteDepthMeltMan;
  /* TDepthMeltMan */
  sprintf(output[OUT_TDEPTHMELTMAN].cName,"TDepthMeltMan");
  sprintf(output[OUT_TDEPTHMELTMAN].cDescr,"Temp at base of UM Melt Region");
  sprintf(output[OUT_TDEPTHMELTMAN].cNeg,"K");
  output[OUT_TDEPTHMELTMAN].bNeg = 1;
  output[OUT_TDEPTHMELTMAN].dNeg = 1; 
  output[OUT_TDEPTHMELTMAN].iNum = 1;
  fnWrite[OUT_TDEPTHMELTMAN] = &WriteTDepthMeltMan;
  /* TJumpMeltMan */
  sprintf(output[OUT_TJUMPMELTMAN].cName,"TJumpMeltMan");
  sprintf(output[OUT_TJUMPMELTMAN].cDescr,"Temp Jump across UM Melt Region");
  sprintf(output[OUT_TJUMPMELTMAN].cNeg,"K");
  output[OUT_TJUMPMELTMAN].bNeg = 1;
  output[OUT_TJUMPMELTMAN].dNeg = 1; 
  output[OUT_TJUMPMELTMAN].iNum = 1;
  fnWrite[OUT_TJUMPMELTMAN] = &WriteTJumpMeltMan;
  /* EruptEff */
  sprintf(output[OUT_ERUPTEFF].cName,"EruptEff");
  sprintf(output[OUT_ERUPTEFF].cDescr,"Mantle Melt Eruption Efficiency");
  sprintf(output[OUT_ERUPTEFF].cNeg,"nd");
  output[OUT_ERUPTEFF].bNeg = 1;
  output[OUT_ERUPTEFF].dNeg = 1; 
  output[OUT_ERUPTEFF].iNum = 1;
  fnWrite[OUT_ERUPTEFF] = &WriteEruptEff;
  /* K2Man */
  sprintf(output[OUT_K2MAN].cName,"K2Man");
  sprintf(output[OUT_K2MAN].cDescr,"Real Love Number k2 Mantle");
  sprintf(output[OUT_K2MAN].cNeg,"nd");
  output[OUT_K2MAN].bNeg = 1;
  output[OUT_K2MAN].dNeg = 1; 
  output[OUT_K2MAN].iNum = 1;
  fnWrite[OUT_K2MAN] = &WriteK2Man;
  /* Imk2Man */
  sprintf(output[OUT_IMK2MAN].cName,"Imk2Man");
  sprintf(output[OUT_IMK2MAN].cDescr,"Imaginary Love Number k2 Mantle");
  sprintf(output[OUT_IMK2MAN].cNeg,"nd");
  output[OUT_IMK2MAN].bNeg = 1;
  output[OUT_IMK2MAN].dNeg = 1; 
  output[OUT_IMK2MAN].iNum = 1;
  fnWrite[OUT_IMK2MAN] = &WriteImk2Man;
  /* RIC */
  sprintf(output[OUT_RIC].cName,"RIC");
  sprintf(output[OUT_RIC].cDescr,"Inner Core Radius");
  sprintf(output[OUT_RIC].cNeg,"m");
  output[OUT_RIC].bNeg = 1;
  output[OUT_RIC].dNeg = 1; 
  output[OUT_RIC].iNum = 1;
  fnWrite[OUT_RIC] = &WriteRIC;
  /* DRICDTCMB */
  sprintf(output[OUT_DRICDTCMB].cName,"DRICDTCMB");
  sprintf(output[OUT_DRICDTCMB].cDescr,"d(R_ic)/d(T_cmb)");
  sprintf(output[OUT_DRICDTCMB].cNeg,"m/K");
  output[OUT_DRICDTCMB].bNeg = 1;
  output[OUT_DRICDTCMB].dNeg = 1; 
  output[OUT_DRICDTCMB].iNum = 1;
  fnWrite[OUT_DRICDTCMB] = &WriteDRICDTCMB;

  /* Heat Fluxes/Flows */
  /* HFluxUMan */
  sprintf(output[OUT_HFLUXUMAN].cName,"HfluxUMan");
  sprintf(output[OUT_HFLUXUMAN].cDescr,"Heat Flux Upper Mantle");
  sprintf(output[OUT_HFLUXUMAN].cNeg,"W/m^2");
  output[OUT_HFLUXUMAN].bNeg = 1;
  output[OUT_HFLUXUMAN].dNeg = 1;  //KM; 
  output[OUT_HFLUXUMAN].iNum = 1;
  fnWrite[OUT_HFLUXUMAN] = &WriteHfluxUMan;
  /* HFluxLMan */
  sprintf(output[OUT_HFLUXLMAN].cName,"HfluxLMan");
  sprintf(output[OUT_HFLUXLMAN].cDescr,"Heat Flux Lower Mantle");
  sprintf(output[OUT_HFLUXLMAN].cNeg,"W/m^2");
  output[OUT_HFLUXLMAN].bNeg = 1;
  output[OUT_HFLUXLMAN].dNeg = 1;
  output[OUT_HFLUXLMAN].iNum = 1;
  fnWrite[OUT_HFLUXLMAN] = &WriteHfluxLMan;
  /* HFluxCMB */
  sprintf(output[OUT_HFLUXCMB].cName,"HfluxCMB");
  sprintf(output[OUT_HFLUXCMB].cDescr,"Heat Flux Core-Mantle Boundary");
  sprintf(output[OUT_HFLUXCMB].cNeg,"W/m^2");
  output[OUT_HFLUXCMB].bNeg = 1;
  output[OUT_HFLUXCMB].dNeg = 1;
  output[OUT_HFLUXCMB].iNum = 1;
  fnWrite[OUT_HFLUXCMB] = &WriteHfluxCMB;
  /* HFlowUMan */
  sprintf(output[OUT_HFLOWUMAN].cName,"HflowUMan");
  sprintf(output[OUT_HFLOWUMAN].cDescr,"Heat Flow Upper Mantle");
  sprintf(output[OUT_HFLOWUMAN].cNeg,"W");
  output[OUT_HFLOWUMAN].bNeg = 1;
  output[OUT_HFLOWUMAN].dNeg = 1;
  output[OUT_HFLOWUMAN].iNum = 1;
  fnWrite[OUT_HFLOWUMAN] = &WriteHflowUMan;
  /* HFlowLMan */
  sprintf(output[OUT_HFLOWLMAN].cName,"HflowLMan");
  sprintf(output[OUT_HFLOWLMAN].cDescr,"Heat Flow Lower Mantle");
  sprintf(output[OUT_HFLOWLMAN].cNeg,"W");
  output[OUT_HFLOWLMAN].bNeg = 1;
  output[OUT_HFLOWLMAN].dNeg = 1;
  output[OUT_HFLOWLMAN].iNum = 1;
  fnWrite[OUT_HFLOWLMAN] = &WriteHflowLMan;
  /* HFlowCMB */
  sprintf(output[OUT_HFLOWCMB].cName,"HflowCMB");
  sprintf(output[OUT_HFLOWCMB].cDescr,"Heat Flow Core-Mantle Boundary");
  sprintf(output[OUT_HFLOWCMB].cNeg,"W");
  output[OUT_HFLOWCMB].bNeg = 1;
  output[OUT_HFLOWCMB].dNeg = 1;
  output[OUT_HFLOWCMB].iNum = 1;
  fnWrite[OUT_HFLOWCMB] = &WriteHflowCMB;
  /* HFlowLatentMan */
  sprintf(output[OUT_HFLOWLATENTMAN].cName,"HflowLatentMan");
  sprintf(output[OUT_HFLOWLATENTMAN].cDescr,"Latent Heat Flow Mantle");
  sprintf(output[OUT_HFLOWLATENTMAN].cNeg,"W");
  output[OUT_HFLOWLATENTMAN].bNeg = 1;
  output[OUT_HFLOWLATENTMAN].dNeg = 1;
  output[OUT_HFLOWLATENTMAN].iNum = 1;
  fnWrite[OUT_HFLOWLATENTMAN] = &WriteHflowLatentMan;
  /* HFlowMeltMan */
  sprintf(output[OUT_HFLOWMELTMAN].cName,"HflowMeltMan");
  sprintf(output[OUT_HFLOWMELTMAN].cDescr,"Melt Heat Flow Mantle");
  sprintf(output[OUT_HFLOWMELTMAN].cNeg,"W");
  output[OUT_HFLOWMELTMAN].bNeg = 1;
  output[OUT_HFLOWMELTMAN].dNeg = 1;
  output[OUT_HFLOWMELTMAN].iNum = 1;
  fnWrite[OUT_HFLOWMELTMAN] = &WriteHflowMeltMan;
  /* TidalPowMan */
  sprintf(output[OUT_TIDALPOWMAN].cName,"TidalPowMan");
  sprintf(output[OUT_TIDALPOWMAN].cDescr,"Tidal Power Mantle");
  sprintf(output[OUT_TIDALPOWMAN].cNeg,"W");
  output[OUT_TIDALPOWMAN].bNeg = 1;
  output[OUT_TIDALPOWMAN].dNeg = 1;
  output[OUT_TIDALPOWMAN].iNum = 1;
  fnWrite[OUT_TIDALPOWMAN] = &WriteTidalPowMan;
  /* HFlowLatentIC */
  sprintf(output[OUT_HFLOWLATENTIC].cName,"HflowLatentIC");
  sprintf(output[OUT_HFLOWLATENTIC].cDescr,"Latent Heat Release at ICB");
  sprintf(output[OUT_HFLOWLATENTIC].cNeg,"W");
  output[OUT_HFLOWLATENTIC].bNeg = 1;
  output[OUT_HFLOWLATENTIC].dNeg = 1;
  output[OUT_HFLOWLATENTIC].iNum = 1;
  fnWrite[OUT_HFLOWLATENTIC] = &WriteHflowLatentIC;
  /* PowerGravIC */
  sprintf(output[OUT_POWERGRAVIC].cName,"PowerGravIC");
  sprintf(output[OUT_POWERGRAVIC].cDescr,"Gravitational Power Release at ICB");
  sprintf(output[OUT_POWERGRAVIC].cNeg,"W");
  output[OUT_POWERGRAVIC].bNeg = 1;
  output[OUT_POWERGRAVIC].dNeg = 1;
  output[OUT_POWERGRAVIC].iNum = 1;
  fnWrite[OUT_POWERGRAVIC] = &WritePowerGravIC;

  /* TDotMan */
  sprintf(output[OUT_TDOTMAN].cName,"TDotMan");
  sprintf(output[OUT_TDOTMAN].cDescr,"Change in Mantle Temperature");
  sprintf(output[OUT_TDOTMAN].cNeg,"K/s");
  output[OUT_TDOTMAN].bNeg = 1;
  output[OUT_TDOTMAN].dNeg = 1; // default units are K. 
  output[OUT_TDOTMAN].iNum = 1;
  fnWrite[OUT_TDOTMAN] = &WriteTDotMan;
  /* TDotCore */
  sprintf(output[OUT_TDOTCORE].cName,"TDotCore");
  sprintf(output[OUT_TDOTCORE].cDescr,"Change in Core Temperature");
  sprintf(output[OUT_TDOTCORE].cNeg,"K/s");
  output[OUT_TDOTCORE].bNeg = 1;
  output[OUT_TDOTCORE].dNeg = 1; // default units are K. 
  output[OUT_TDOTCORE].iNum = 1;
  fnWrite[OUT_TDOTCORE] = &WriteTDotCore;
  /* Constants */
  /* TrefLind */
  sprintf(output[OUT_TREFLIND].cName,"TrefLind");
  sprintf(output[OUT_TREFLIND].cDescr,"Core Liquidus Lindeman Reference");
  sprintf(output[OUT_TREFLIND].cNeg,"K");
  output[OUT_TREFLIND].bNeg = 1;
  output[OUT_TREFLIND].dNeg = 1; 
  output[OUT_TREFLIND].iNum = 1;
  fnWrite[OUT_TREFLIND] = &WriteTrefLind;

}

void FinalizeOutputFunctionThermint(OUTPUT *output,int iBody,int iModule) {
  //  output[OUT_TDOTMAN].fnOutput[iBody][iModule] = &fdTDotMan;
    output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxRadheat; //This is need to print the global var to log.  Needs to be fixed.
}

/************ THERMINT Logging Functions **************/

void LogOptionsThermint(CONTROL *control, FILE *fp) {
  /* Anything here?
  fprintf(fp,"-------- RADHEAT Options -----\n\n");
  */
}

//PED: this would be for global rad heat parameters, but this is blank bc rad is only relevant to each individual body.
void LogThermint(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp) {
  /* Anything here? 
  int iOut;
  fprintf(fp,"\n----- RADHEAT PARAMETERS ------\n");
  for (iOut=OUTSTARTRADHEAT;iOut<OUTBODYSTARTRADHEAT;iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
  }
  */
}

void LogBodyThermint(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UPDATE *update,fnWriteOutput fnWrite[],FILE *fp,int iBody) {
  int iOut;

  fprintf(fp,"----- THERMINT PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTSTARTTHERMINT;iOut<OUTENDTHERMINT;iOut++) {
    if (output[iOut].iNum > 0) 
      WriteLogEntry(body,control,&output[iOut],system,update,fnWrite[iOut],fp,iBody);
  }
  /* Write out some global constants. */
  fprintf(fp,"EMASS=%e EMASSMAN=%e ERMAN=%e ERCORE=%e EDMAN=%e EVOL=%e EVOLCORE=%e EVOLMAN=%e\n",EMASS,EMASSMAN,ERMAN,ERCORE,EDMAN,EVOL,EVOLCORE,EVOLMAN);
  fprintf(fp,"EDENS=%e EDENSMAN=%e EDENSCORE=%e EDENSOC=%e EDENSIC=%e STIFFNESS=%e\n",EDENS,EDENSMAN,EDENSCORE,EDENSOC,EDENSIC,STIFFNESS);
  fprintf(fp,"THERMEXPANMAN=%e THERMCONDUMAN=%e THERMCONDLMAN=%e THERMDIFFUMAN=%e cube(EDMAN)=%e\n",THERMEXPANMAN,THERMCONDUMAN,THERMCONDLMAN,THERMDIFFUMAN,cube(EDMAN));
  fprintf(fp,"TrefLind=%e ViscRef=%e \n",body[iBody].dTrefLind,body[iBody].dViscRef);
  //  fprintf(fp,"TrefLind=%e \n",(TREFLIND));
}

void AddModuleThermint(MODULE *module,int iBody,int iModule) {

  module->iaModule[iBody][iModule] = THERMINT;

  module->fnInitializeControl[iBody][iModule] = &InitializeControlThermint;
  module->fnInitializeUpdateTmpBody[iBody][iModule] = &InitializeUpdateTmpBodyThermint;

  module->fnCountHalts[iBody][iModule] = &CountHaltsRadHeat;
  module->fnReadOptions[iBody][iModule] = &ReadOptionsThermint;
  module->fnLogBody[iBody][iModule] = &LogBodyThermint;
  module->fnVerify[iBody][iModule] = &VerifyThermint;
  module->fnVerifyHalt[iBody][iModule] = &VerifyHaltThermint;
  module->fnVerifyRotation[iBody][iModule] = &VerifyRotationThermint;
  
  module->fnInitializeBody[iBody][iModule] = &InitializeBodyThermint;
  module->fnInitializeUpdate[iBody][iModule] = &InitializeUpdateThermint;

    // NEED TO ADD THERMINT VARIABLES HERE??
  module->fnFinalizeUpdateTMan[iBody][iModule] = &FinalizeUpdateTManThermint;
  module->fnFinalizeUpdateTCore[iBody][iModule] = &FinalizeUpdateTCoreThermint;

  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionThermint;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionThermint;

}

/************* THERMINT Functions ************/
/* Scalar Properties */
/* Get TUMan */
double fdTUMan(BODY *body,int iBody) {
  return ADJUMPM2UM*body[iBody].dTMan;
}
/* Get TLMan */
double fdTLMan(BODY *body,int iBody) {
    return ADJUMPM2LM*body[iBody].dTMan;
}
/* Get TCMB */
double fdTCMB(BODY *body,int iBody) {
    return ADJUMPC2CMB*body[iBody].dTCore;
}
/* Get TJumpUMan */ 
double fdTJumpUMan(BODY *body,int iBody) {
    return fabs(body[iBody].dTUMan-TSURF);
}
/* Get TJumpLMan */
double fdTJumpLMan(BODY *body,int iBody) {
    return fabs(body[iBody].dTCMB-body[iBody].dTLMan);
}
/* Get SignTJumpUMan */ 
double fdSignTJumpUMan(BODY *body,int iBody) {
    return (body[iBody].dTUMan-TSURF)/fabs(body[iBody].dTUMan-TSURF);
}
/* Get SignTJumpLMan */
double fdSignTJumpLMan(BODY *body,int iBody) {
    return (body[iBody].dTCMB-body[iBody].dTLMan)/fabs(body[iBody].dTCMB-body[iBody].dTLMan);
}
/* Get ViscUManArr Arrhenius law only */
double fdViscUManArr(BODY *body,int iBody) {  
  return body[iBody].dViscRef*exp(ACTVISCMAN/(GASCONSTANT*body[iBody].dTUMan));
}
/* Get ViscUMan */
double fdViscUMan(BODY *body,int iBody) {
  //  return body[iBody].dViscRef*exp(ACTVISCMAN/(GASCONSTANT*body[iBody].dTUMan))/body[iBody].dMeltfactorUMan;
  return body[iBody].dViscUManArr/body[iBody].dMeltfactorUMan;
}
double fdViscLMan(BODY *body,int iBody) {
  //    return body[iBody].dViscUMan*body[iBody].dViscRatioMan;  //w/o meltfactorLMan.
  return body[iBody].dViscUManArr*body[iBody].dViscRatioMan/body[iBody].dMeltfactorLMan;   //w/ meltfactorLMan
}
/* Get ViscRatioMan */
double fdViscRatioMan(BODY *body,int iBody) {
    return body[iBody].dViscRatioMan;
}
/* Get Boundary Layer Thicknesses */
double fdBLUMan(BODY *body,int iBody) {
  return (EDMAN)*pow((RACRIT)*body[iBody].dViscUMan*(THERMDIFFUMAN)/((THERMEXPANMAN)*(GRAVLMAN)*body[iBody].dTJumpLMan*cube(EDMAN)),(CONVEXPON));
}
double fdBLLMan(BODY *body,int iBody) {
  return (EDMAN)*pow((RACRIT)*body[iBody].dViscLMan*(THERMDIFFLMAN)/((THERMEXPANMAN)*(GRAVLMAN)*body[iBody].dTJumpLMan*cube(EDMAN)),(CONVEXPON));
}
double fdShmodUMan(BODY *body,int iBody) {
    return (SHMODREF)*exp((ACTSHMODMAN)/(GASCONSTANT*body[iBody].dTUMan))/body[iBody].dMeltfactorUMan;
}
double fdTsolUMan(BODY *body,int iBody) {
    return fdSolidusMan(body[iBody].dBLUMan);
}
double fdTsolLMan(BODY *body,int iBody) {
  return fdSolidusMan(ERADIUS-ERCORE-body[iBody].dBLLMan);  //Solidus fn of depth: z_LM=R-R_c-delta_LM
}
double fdTliqUMan(BODY *body,int iBody) {
    return body[iBody].dTsolUMan+DTLIQMAN;  //approx constant offset btwn sol and liq.
}
double fdTliqLMan(BODY *body,int iBody) {
    return body[iBody].dTsolLMan+DTLIQMAN;  //approx constant offset btwn sol and liq.
}
double fdFMeltUMan(BODY *body,int iBody) {
    double fmelt=(body[iBody].dTUMan-body[iBody].dTsolUMan)/(body[iBody].dTliqUMan-body[iBody].dTsolUMan);
    double fmelt2=min(fmelt,1.0);  //require fmelt<1
    fmelt2=max(fmelt2,0.0); //require fmelt>0
    double min1=min(1.0,2.0);  //test min
    double max1=max(1.0,2.0);  //test max
    return fmelt2;
}
double fdFMeltLMan(BODY *body,int iBody) {
    double fmelt=(body[iBody].dTLMan-body[iBody].dTsolLMan)/(body[iBody].dTliqLMan-body[iBody].dTsolLMan);
    double fmelt2=min(fmelt,1.0);  //require fmelt<1
    fmelt2=max(fmelt2,0.0); //require fmelt>0
    double min1=min(1.0,2.0);  //test min
    double max1=max(1.0,2.0);  //test max
    return fmelt2;
}
double fdMeltfactorUMan(BODY *body,int iBody) {
    /* Melt factor used in viscosity and shmod.  For fmelt=1, this should give 1.20428. */
    double bigphi=body[iBody].dFMeltUMan/(MELTPHISTAR);
    double bigf=(1.0-(MELTXI))*erf(sqrt(PI)/(2.0*(1.0-(MELTXI)))*bigphi*(1.0+pow(bigphi,(MELTGAMMA))));
    double meltexp=(MELTB)*(MELTPHISTAR);
    return (1.0+pow(bigphi,(MELTDELTA)))/pow(1.0-bigf,meltexp);
}
double fdMeltfactorLMan(BODY *body,int iBody) {
    /* Melt factor used in viscosity and shmod.  For fmelt=1, this should give 1.20428. */
    double bigphi=body[iBody].dFMeltLMan/(MELTPHISTAR);
    double bigf=(1.0-(MELTXI))*erf(sqrt(PI)/(2.0*(1.0-(MELTXI)))*bigphi*(1.0+pow(bigphi,(MELTGAMMA))));
    double meltexp=(MELTB)*(MELTPHISTAR);
    return (1.0+pow(bigphi,(MELTDELTA)))/pow(1.0-bigf,meltexp);
}

/* Get DepthMeltMan */ 
double fdDepthMeltMan(BODY *body,int iBody) {
    //    double guess1=body[iBody].dBLUMan;  //lower bound of depth to bottom of UM melt layer.
    //    double guess2=1.445e6;  //(EDMAN)/2.0;  //mid-mantle= upper bound to depth of melt layer.
    //    double tol=10.0;  //root resolution is +/- 10 m?
    //    int nmax=100;  //nmax iterations of root.
    int type=0;  //types: 0=UMan, 1=LMan, 2=ICN
    //    return root(type,body,iBody,guess1,guess2,tol,nmax);
    double depthmeltman=cubicroot(type,body,iBody);
    if (depthmeltman < body[iBody].dBLUMan) {        //if solidus intersects adiabat within UMTBL then recompute it.
      depthmeltman=cubicroot(1,body,iBody);        //type=1 to find intersection within UMTBL.
    }
    return depthmeltman;
}
/* Get TDepthMeltMan */
double fdTDepthMeltMan(BODY *body,int iBody) {
    if (body[iBody].dDepthMeltMan==0) {    //if no melt layer found.
          return 0;
      } else {                         //if yes melt layer found.
          return fdSolidusMan(body[iBody].dDepthMeltMan);
      }
}
/* Get TJumpMeltMan */ 
double fdTJumpMeltMan(BODY *body,int iBody) {
    return body[iBody].dTDepthMeltMan-TSURF-(ADGRADMAN)*body[iBody].dDepthMeltMan;  //Temp jump across entire UM melt region.
}
double fdK2Man(BODY *body,int iBody) {
    return 3./2/(1.+19./2*body[iBody].dShmodUMan/(STIFFNESS));
}

double fdImk2Man(BODY *body,int iBody) {
  double viscdyn=body[iBody].dViscUMan*(EDENSMAN); //dynamic viscosity.

  /* Peter's version. I think dRotRate should be dMeanMotion
  double denom2=pow((1.+(19./2)*(body[iBody].dShmodUMan/(STIFFNESS)))*(viscdyn*body[iBody].dRotRate/body[iBody].dShmodUMan),2.);
  double imk2=(57./4)*viscdyn*body[iBody].dRotRate/( (STIFFNESS)*(1.0+ denom2) );
  */
  double denom2=pow((1.+(19./2)*(body[iBody].dShmodUMan/(STIFFNESS)))*(viscdyn*body[iBody].dMeanMotion/body[iBody].dShmodUMan),2.);
  double imk2=(57./4)*viscdyn*body[iBody].dMeanMotion/( (STIFFNESS)*(1.0+ denom2) );
  return imk2;
}
/* Inner Core Size */
double fdRIC(BODY *body,int iBody) {
  //    double numerator=pow((DADCORE)/(ERCORE),2.0)*log((TREFLIND)/body[iBody].dTCMB)-1.0;
    double numerator=pow((DADCORE)/(ERCORE),2.0)*log(body[iBody].dTrefLind/body[iBody].dTCMB)-1.0;
    if (numerator>0) {    //IC Found.
        return (ERCORE)*sqrt( numerator/(2.0*(1.0-1.0/3.0/(GRUNEISEN))*pow((DADCORE)/(DLIND),2.0)-1.0) );
    } else {
        return 0;        //no IC.
    }
}

double fdTidalPowMan(BODY *body,int iBody) {
  /* Peter's version. I think dRotRate should be dMeanMotion.
  return (21./2)*body[iBody].dImk2Man*(BIGG)*pow(body[0].dMass/pow(body[iBody].dSemi,3.),2.)*pow(body[iBody].dRadius,5.)*body[iBody].dRotRate*pow(body[iBody].dEcc,2.);
  */
  return (21./2)*body[iBody].dImk2Man*(BIGG)*pow(body[0].dMass/pow(body[iBody].dSemi,3.),2.)*pow(body[iBody].dRadius,5.)*body[iBody].dMeanMotion*pow(body[iBody].dEcc,2.);
}

/* Heat Fluxes/flows */
double fdHfluxUMan(BODY *body,int iBody) {
  return (THERMCONDUMAN)*body[iBody].dSignTJumpUMan*body[iBody].dTJumpUMan/body[iBody].dBLUMan;
}
double fdHfluxLMan(BODY *body,int iBody) {
  return (THERMCONDLMAN)*body[iBody].dSignTJumpLMan*body[iBody].dTJumpLMan/body[iBody].dBLLMan;
}
double fdHfluxCMB(BODY *body,int iBody) {
  return fdHfluxLMan(body,iBody);
}
double fdHflowUMan(BODY *body,int iBody) {
  return (AREASURF)*fdHfluxUMan(body,iBody);
}
double fdHflowLMan(BODY *body,int iBody) {
  return (AREACMB)*fdHfluxLMan(body,iBody);
}
double fdHflowCMB(BODY *body,int iBody) {
  return fdHflowLMan(body,iBody);
}
double fdHflowLatentMan(BODY *body,UPDATE *update,int iBody) {
    double HflowLatentMan=(-DVLIQDTEMP)*(*(update[iBody].pdTDotMan))*(EDENSMAN)*(SPECLATENTMAN);  //which structure has dTDotMan??
    HflowLatentMan=max(HflowLatentMan,0);   //ensure positive.
    return HflowLatentMan;
}
double fdHflowMeltMan(BODY *body,int iBody) {
    double MeltMassDot=1.16*(THERMDIFFUMAN)*(AREASURF)/body[iBody].dBLUMan*(EDENSMAN)*body[iBody].dFMeltUMan; //DB15 (31)   
    return body[iBody].dEruptEff*MeltMassDot*((SPECLATENTMAN)+(SPECHEATMAN)*body[iBody].dTJumpMeltMan);
}
double fdDRICDTCMB(BODY *body,int iBody) {            //=d(R_ic)/d(T_cmb)
    if (body[iBody].dRIC>0) {   //If IC exists.
      double dn_rc2=pow((DADCORE)/(ERCORE),2.0); 
      //      return -(body[iBody].dRIC/(2.0*body[iBody].dTCMB))*dn_rc2/( dn_rc2*log((TREFLIND)/body[iBody].dTCMB)-1.0 ); //DB14 (32)   
      return -(body[iBody].dRIC/(2.0*body[iBody].dTCMB))*dn_rc2/( dn_rc2*log(body[iBody].dTrefLind/body[iBody].dTCMB)-1.0 ); //DB14 (32)   
    } else {                    //If no IC.
      return 0;
    }
}
double fdMassICDot(BODY *body,UPDATE *update,int iBody) {
    if (body[iBody].dRIC>0) {   //If IC exists.
      double areaic=4.0*PI*pow(body[iBody].dRIC,2.0);
      return areaic*(EDENSIC)*(*(update[iBody].pdTDotCore))/(ADJUMPC2CMB)*body[iBody].dDRICDTCMB;  //DB14 (31)
    } else {                    //If no IC.
      return 0;
    }
}

double fdHflowLatentIC(BODY *body,UPDATE *update,int iBody) {
    if (body[iBody].dRIC>0) {   //If IC exists.
      return body[iBody].dMassICDot*(SPECLATENTICB);  //DB14 (26)
    } else {                    //If no IC.
      return 0;
    }
}
double fdPowerGravIC(BODY *body,UPDATE *update,int iBody) {
    if (body[iBody].dRIC>0) {   //If IC exists.
      return body[iBody].dMassICDot*(SPECPOWGRAVIC);  //DB14 (26)
    } else {                    //If no IC.
      return 0;
    }
}

/*** These derivatives are called from the udpate matrix, format is fixed. ***/
/* Get TDotMan */
double fdTDotMan(BODY *body,SYSTEM *system,int *iaBody) {
  int iBody=iaBody[0];
  return (body[iBody].dHflowCMB+body[iBody].dPowRadiogMan+body[iBody].dHflowLatentMan+body[iBody].dTidalPowMan-body[iBody].dHflowUMan-body[iBody].dHflowMeltMan)/((EMASSMAN)*(SPECHEATMAN)); 
}

/* Get TDotCore */
double fdTDotCore(BODY *body,SYSTEM *system,int *iaBody) {
  int iBody=iaBody[0];   //Is this correct?
  //  return (-body[iBody].dHflowCMB+body[iBody].dPowRadiogCore)/((EMASSCORE)*(SPECHEATCORE));   //No IC heat.
  double areaic=4.0*PI*pow(body[iBody].dRIC,2.0);
  return (-body[iBody].dHflowCMB+body[iBody].dPowRadiogCore)/((EMASSCORE)*(SPECHEATCORE) -areaic*(EDENSIC)*(ADJUMPC2CMB)*body[iBody].dDRICDTCMB*(SPECLATENTICB+SPECPOWGRAVIC));
}



/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/*********************************   MATH   *********************************/
/****************************************************************************/
/* MATH FUNCTIONS */
double cube(double x) {
  return pow(x,3);
}
double root(int type,BODY *body,int iBody,double guess1,double guess2,double tol,int nmax) {
    double mid=0.0;  //current midpoint or root.
    double fmid=0.0;  //current value of f(mid).
    double fguess1=0.0;  //current value of f(guess1).
    int count=0.0;
    while (count <= nmax) {
      mid=(guess1+guess2)/2.0;                            //bisection method from wikipedia.
      if (type == 0) {                                    //use fdSolTempDiffMan
          fmid=fdSolTempDiffMan(mid,body,iBody);          //function at mid.
          fguess1=fdSolTempDiffMan(guess1,body,iBody);    //function at guess1.
      }
      if (fmid==0 || (guess2-guess1)/2.0<=tol) {          //solution found.
          return mid;
      }
      count++;             //increment count by 1.
      if (fmid/fabs(fmid)==fguess1/fabs(fguess1)) {guess2=mid;}
      else {guess1=mid;}
    }
    printf("method failed (nmax exceeded)\n");
    return 0;
}

double cubicroot(int type,BODY *body,int iBody) {
    double a=0,b=0,c=0,d=0;  //coefficients of cubic polynomial.    
    if (type==0){     //type=0 is melt intersection in adiabatic part of mantle, away from TBL's.
      a=ASOLIDUS;
      b=BSOLIDUS;
      c=CSOLIDUS+ADGRADMAN;
      d=DSOLIDUS-body[iBody].dTUMan-(ADGRADMAN)*((ERMAN)-body[iBody].dBLUMan);
    }
    if (type==1){    //type=1 is melt intersection within UM TBL.
      a=ASOLIDUS;
      b=BSOLIDUS;
      c=CSOLIDUS+body[iBody].dTJumpUMan/body[iBody].dBLUMan;
      d=DSOLIDUS-TSURF-body[iBody].dTJumpUMan/body[iBody].dBLUMan*(ERMAN);
    }   
    double delta0=pow(b,2.0)-3.0*a*c;                                             //cubic root component (wikip)
    double delta1=2.0*cube(b)-9.0*a*b*c+27.0*pow(a,2.0)*d;                        //cubic root component (wikip)
    double croot=pow( (delta1+sqrt(pow(delta1,2.0)-4.0*cube(delta0))) /2.0,1./3); //cubic root component (wikip)
    if (pow(delta1,2.0)-4.0*cube(delta0) < 0) {
      //        printf("imaginary cubic root!\n");
      //        exit(1);
      return 0;       //imaginary root implies no intersection, no melt layer?
    }
    double root=-1.0/(3.0*a)*(b+croot+delta0/croot);                              //real cubic root, radius of layer.
    return ERMAN-root;                                                            //Return depth.
}
double fdSolidusMan(double depth) {
    double r=(ERMAN)-depth;  //radius to bottom of region.
    double aterm=(ASOLIDUS)*pow(r,3.);
    double bterm=(BSOLIDUS)*pow(r,2.);
    double cterm=(CSOLIDUS)*r;
    return (ASOLIDUS)*pow(r,3.)+(BSOLIDUS)*pow(r,2.)+(CSOLIDUS)*r+(DSOLIDUS);
}    
double fdSolTempDiffMan(double depth,BODY *body,int iBody) { //Given a depth and BODY, return the difference between the solidus and geotherm at that depth.
    double solidus=fdSolidusMan(depth);
    double geotherm=TSURF+body[iBody].dSignTJumpUMan*body[iBody].dTJumpUMan*erf(2.0*depth/body[iBody].dBLUMan); //DB14 (16)
    return solidus-geotherm;
}
