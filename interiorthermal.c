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

voﬁid  InitializeControlInteriorthermal(CONTROL *control) {
  /* Nothing for now, but this subroutine is necessary for module loops. */
}

void BodyCopyInteriorthermal(BODY *dest,BODY *src,int foo,int iBody) {
  dest[iBody].dTMan = src[iBody].dTMan;
}

void InitializeBodyInteriorthermal(BODY *body,CONTROL *control,UPDATE *update,int iBody,int iModule) {
}

void InitializeUpdateTmpBodyInteriorthermal(BODY *body,CONTROL *control,UPDATE *update,int iBody) {
}

/**************** RADHEAT options ********************/

/* Initial Mantle Temperature */

void ReadTMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {   //if line num of option ge 0
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)   //if input value lt 0
      body[iFile-1].d40KPowerMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
   else
       //      body[iFile-1].d40KPowerMan = dTmp*fdUnitsMass(control->Units[iFile].iMass);
       //CHANGED units Mass to Power.
       body[iFile-1].d40KPowerMan = dTmp*fdUnitsPower(control->Units[iFile].iTime,control->Units[iFile].iMass,control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
      if (iFile > 0)  //if line num not ge 0, then if iFile gt 0, then set default.
      body[iFile-1].d40KPowerMan = options->dDefault;
}


/* Initiatlize Input Options */

void InitializeOptionsInteriorthermal(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  /* TMan */
  sprintf(options[OPT_40KMASSMAN].cName,"dTMan");
  sprintf(options[OPT_40KMASSMAN].cDescr,"Initial Mantle Temperature");
  sprintf(options[OPT_40KMASSMAN].cDefault,"Default=3000");
  options[OPT_40KMASSMAN].iType = 2;
  options[OPT_40KMASSMAN].iMultiFile = 1;
  options[OPT_40KMASSMAN].dNeg = EMASSMAN40K;
  options[OPT_40KMASSMAN].dDefault = 3000d; 
  sprintf(options[OPT_40KMASSMAN].cNeg,"Earth Masses");
  fnRead[OPT_40KMASSMAN] = &Read40KMassMan;

}

void ReadOptionsInteriorthermal(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[],int iBody) {
  int iOpt;

  for (iOpt=OPTSTARTRADHEAT;iOpt<OPTENDRADHEAT;iOpt++) {
    if (options[iOpt].iType != -1) 
      fnRead[iOpt](body,control,files,&options[iOpt],system,iBody+1);
  }
}
    
/******************* Verify RADHEAT ******************/

void VerifyRotationInteriorthermal(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody) {
  /* Nothing */
}

void AssignTMan(BODY *body,OPTIONS *options,double dAge,int iBody) {
    /* Mantle */
    if (options[OPT_TMAN].iLine[iBody+1] >= 0) {
	//  I think here you need to define body.40KNum bc only the default value of 40Kmass has been chosen by user and set.
	//      printf("40KMass=%e, MASS40K=%e, 40KNum=%e\n",body[iBody].d40KMass,MASS40K,body[iBody].d40KNum);
	body[iBody].d40KNumMan=body[iBody].d40KMassMan/(MASS40K);
	printf("40KMassMan set, body[iBody].d40KNumMan=%e, ENUMMAN40K=%e\n",body[iBody].d40KNumMan,ENUMMAN40K);
    }
    body[iBody].dTMan = fd40KConstant(body[iBody].d40KNumMan,dAge);  //Get the constant given num and age.

}


void VerifyTMan(BODY *body,OPTIONS *options,UPDATE *update,double dAge,fnUpdateVariable ***fnUpdate,int iBody) {
  AssignTMan(body,options,dAge,iBody);
  /* Mantle */
  update[iBody].iaType[update[iBody].i40KMan][0] = 1;
  update[iBody].iNumBodies[update[iBody].i40KMan][0]=1;
  update[iBody].iaBody[update[iBody].i40KMan][0] = malloc(update[iBody].iNumBodies[update[iBody].i40KMan][0]*sizeof(int)); //iaBody is the number of bodies that are affected by this variable.
  update[iBody].iaBody[update[iBody].i40KMan][0][0]=iBody;
  update[iBody].pdD40KNumManDt = &update[iBody].daDerivProc[update[iBody].i40KMan][0];
  fnUpdate[iBody][update[iBody].i40KMan][0] = &fdD40KNumManDt;
}


void fnPropertiesInteriorthermal(BODY *body,int iBody) {
  /* Nothing */
}

void fnForceBehaviorInteriorthermal(BODY *body,EVOLVE *evolve,IO *io,int iBody,int iModule) {
  if (body[iBody].dTMan < 0.5)
    body[iBody].dTMan = 0;
}

void VerifyInteriorthermal(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE *update,fnUpdateVariable ***fnUpdate,int iBody,int iModule) {
  int bInteriorthermal=0;

  if (body[iBody].d40KNumMan > 0 || body[iBody].d40KMassMan > 0 || body[iBody].d40KPowerMan > 0 ||
      body[iBody].d40KNumCore > 0 || body[iBody].d40KMassCore > 0 || body[iBody].d40KPowerCore > 0) {
    VerifyTMan(body,options,update,system->dAge,fnUpdate,iBody);  //Verify Man and Core.
    bInteriorthermal = 1;
  }

  if (!bInteriorthermal && control->Io.iVerbose >= VERBINPUT) 
    fprintf(stderr,"WARNING: INTERIORTHERMAL called for body %s, but no radiogenic species present.\n",body[iBody].cName);

  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorInteriorthermal;
  control->Evolve.fnAuxProps[iBody][iModule] = &fnPropertiesInteriorthermal;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyInteriorthermal;
  output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxInteriorthermal;
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
    update[iBody].iNumTMan++;
  }
}

void FinalizeUpdateEccInteriorthermal(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateTManInteriorthermal(BODY *body,UPDATE*update,int *iEqn,int iVar,int iBody) {
  update[iBody].iaModule[iVar][*iEqn] = RAD40KMAN;
  update[iBody].iNum40KMan = (*iEqn)++;
}

void FinalizeUpdateOblInteriorthermal(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateRotInteriorthermal(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void FinalizeUpdateSemiInteriorthermal(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}


/***************** RADHEAT Halts *****************/

/* Minimum 40K Powering? */
//PED: these subroutines aren't finished.  The default halt values aren't set.  body.d40KPowerMan is not a primary variable, but NumMan is.
int fbHaltMinTMan(BODY *body,EVOLVE *evolve,HALT *halt,IO *io,UPDATE *update,int iBody) {

  if (body[iBody].d40KPowerMan < halt->dMin40KPower) {
    if (io->iVerbose >= VERBPROG) {
      printf("HALT: %s's 40K Power =  ",body[iBody].cName);
      fprintd(stdout,body[iBody].d40KPowerMan,io->iSciNot,io->iDigits);
      printf(" < ");
      fprintd(stdout,halt->dMin40KPower,io->iSciNot,io->iDigits);
      printf(".\n");
    }
    return 1;
  }
  return 0;
}        



void CountHaltsRadHeat(HALT *halt,int *iHalt) {
  if (halt->dMin40KPower >= 0)
    (iHalt)++;
  if (halt->dMin232ThPower >=0)
    (iHalt)++;
  if (halt->dMin238UPower >= 0)
    (iHalt)++;
  if (halt->dMin235UPower >= 0)  //PED
    (iHalt)++;
}

void VerifyHaltInteriorthermal(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {

  if (control->Halt[iBody].dMin40KPower > 0)
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltMin40KPower;
}

/************* RADHEAT Outputs ******************/

void HelpOutputInteriorthermal(OUTPUT *output) {
  int iOut;

  printf("\n ------ RADHEAT output ------\n");
  for (iOut=OUTSTARTRADHEAT;iOut<OUTENDRADHEAT;iOut++) 
    WriteHelpOutput(&output[iOut]);
}

/* NOTE: If you write a new Write subroutine here you need to add the associate 
   block of initialization in InitializeOutputInteriorthermal below */


void WriteTMan(BODY *body,CONTROL *control,OUTPUT *output,SYSTEM *system,UNITS *units,UPDATE *update,int iBody,double *dTmp,char cUnit[]) {
  /* Get total power from 40K */
  *dTmp = -(*(update[iBody].pdD40KNumManDt))*ENERGY40K;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit,output->cNeg);
  } else { 
    *dTmp /= fdUnitsPower(units->iTime,units->iMass,units->iLength);
    fsUnitsPower(units,cUnit);
  }
}



void InitializeOutputInteriorthermal(OUTPUT *output,fnWriteOutput fnWrite[]) {

  /* Potassium */
    //  PED:  Do these default numbers matter??  If so they need to be changed.
  sprintf(output[OUT_40KPOWERMAN].cName,"40KPowerMan");
  sprintf(output[OUT_40KPOWERMAN].cDescr,"Total Power Generated by 40K");
  sprintf(output[OUT_40KPOWERMAN].cNeg,"TW");
  output[OUT_40KPOWERMAN].bNeg = 1;
  output[OUT_40KPOWERMAN].dNeg = 1e-19; // ergs/s -> TW 
  output[OUT_40KPOWERMAN].iNum = 1;
  fnWrite[OUT_40KPOWERMAN] = &Write40KPowerMan;
 
}

void FinalizeOutputFunctionInteriorthermal(OUTPUT *output,int iBody,int iModule) {
  output[OUT_SURFENFLUX].fnOutput[iBody][iModule] = &fdSurfEnFluxInteriorthermal;
}

/************ RADHEAT Logging Functions **************/

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

  fprintf(fp,"----- RADHEAT PARAMETERS (%s)------\n",body[iBody].cName);
  for (iOut=OUTSTARTRADHEAT;iOut<OUTENDRADHEAT;iOut++) {
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

  module->iaModule[iBody][iModule] = RADHEAT;

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

  module->fnFinalizeUpdate40KNumMan[iBody][iModule] = &FinalizeUpdate40KNumManInteriorthermal;
  
  // Now include other primary variables not used by RADHEAT 
  module->fnFinalizeUpdateEcc[iBody][iModule] = &FinalizeUpdateEccInteriorthermal;
  module->fnFinalizeUpdateObl[iBody][iModule] = &FinalizeUpdateOblInteriorthermal;
  module->fnFinalizeUpdateRot[iBody][iModule] = &FinalizeUpdateRotInteriorthermal;
  module->fnFinalizeUpdateSemi[iBody][iModule] = &FinalizeUpdateSemiInteriorthermal;

  // NEED TO ADD RADHEAT VARIABLES HERE??

  //module->fnIntializeOutputFunction[iBody][iModule] = &InitializeOutputFunctionInteriorthermal;
  module->fnFinalizeOutputFunction[iBody][iModule] = &FinalizeOutputFunctionInteriorthermal;

}

/************* RADHEAT Functions ************/
// FIRST batch of subroutines are general for any species.
// N = N_0 * exp(-t/lambda)
// dN/dt = -(N_0/lambda) * exp(-t/lambda)

double fdRadPowerMan(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  return -(*(update[iBody].pdD238UNumManDt))*ENERGY238U - (*(update[iBody].pdD235UNumManDt))*ENERGY235U - (*(update[iBody].pdD232ThNumManDt))*ENERGY232TH - (*(update[iBody].pdD40KNumManDt))*ENERGY40K;
}
double fdRadPowerCore(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  return -(*(update[iBody].pdD238UNumCoreDt))*ENERGY238U - (*(update[iBody].pdD235UNumCoreDt))*ENERGY235U - (*(update[iBody].pdD232ThNumCoreDt))*ENERGY232TH - (*(update[iBody].pdD40KNumCoreDt))*ENERGY40K;
}
double fdRadPowerTotal(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
    double dPowerMan;
    double dPowerCore;
    dPowerMan = fdRadPowerMan(body,system,update,iBody,iFoo);
    dPowerCore = fdRadPowerCore(body,system,update,iBody,iFoo);
    return dPowerMan+dPowerCore;
}

/* This is part of output[OUT_SURFENFLUX].fnOutput */
double fdSurfEnFluxInteriorthermal(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iFoo) {
  double dPower;
    dPower = fdRadPowerMan(body,system,update,iBody,iFoo);
  return dPower/(4*PI*body[iBody].dRadius*body[iBody].dRadius);
}

double fdInteriorthermalConst(double dNum,double dAge,double dHalfLife) {
  return dNum/(exp(-dAge/dHalfLife));
}

double fdDNumRadDt(double dConst,double dHalfLife,double dAge) {  //dN/dt, can be used by any radioactive system?
  return -dConst/dHalfLife*exp(-dAge/dHalfLife);
}

double fdRadPower(double dConst,double dHalfLife,double dAge) {
    return dConst*exp(-dHalfLife/dAge);                      //Here const=N_0*energy_i*lambda, where energy_i=erg/num.
}

double fdRadEnFlux(double dConst,double dHalfLife,double dAge,double dRadius) {
  return fdRadPower(dConst,dHalfLife,dAge)/(4*PI*dRadius*dRadius);
}

//SECOND batch of subroutines are for individual variables of species.
/* Constant coefficients */
double fd40KConstant(double dNum,double dAge) {  
    return fdInteriorthermalConst(dNum,dAge,HALFLIFE40K);   //redirects to fdInteriorthermalConst
}

double fd232ThConstant(double dNum,double dAge) {  //PED: changed dPower to dNum.
  return fdInteriorthermalConst(dNum,dAge,HALFLIFE232TH);  //redirects to fdInteriorthermalConst
}

double fd238UConstant(double dNum,double dAge) {  //PED: changed dPower to dNum.
  return fdInteriorthermalConst(dNum,dAge,HALFLIFE238U);  //redirects to fdInteriorthermalConst
}

double fd235UConstant(double dNum,double dAge) {  //PED: changed dPower to dNum.
  return fdInteriorthermalConst(dNum,dAge,HALFLIFE235U);  //redirects to fdInteriorthermalConst
}

double fd40KPowerMan(BODY *body,SYSTEM *system,int *iaBody,int iBody) {
  return fdRadPower(body[iBody].d40KConstMan,HALFLIFE40K,system->dAge);   //redirects to fdRadPower
}

double fd232ThPowerMan(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d232ThConstMan,HALFLIFE232TH,system->dAge);    //redirects to fdRadPower
}

double fd238UPowerMan(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d238UConstMan,HALFLIFE238U,system->dAge);    //redirects to fdRadPower
}

double fd235UPowerMan(BODY *body,SYSTEM *system,int iBody) {
  return fdRadPower(body[iBody].d235UConstMan,HALFLIFE235U,system->dAge);    //redirects to fdRadPower
}

/* Energy Flux */
double fd40KEnFlux(BODY *body,SYSTEM *system,int *iaBody,int iBody) {
  return fdRadEnFlux(body[iBody].d40KConstMan,HALFLIFE40K,system->dAge,body[iBody].dRadius);
}

double fd232ThEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d232ThConstMan,HALFLIFE232TH,system->dAge,body[iBody].dRadius);
}

double fd238UEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d238UConstMan,HALFLIFE238U,system->dAge,body[iBody].dRadius);
}

double fd235UEnFlux(BODY *body,SYSTEM *system,int iBody) {
  return fdRadEnFlux(body[iBody].d235UConstMan,HALFLIFE235U,system->dAge,body[iBody].dRadius);
}

/* DN/Dt */
double fdD40KNumManDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d40KConstMan,HALFLIFE40K,system->dAge);
}

double fdD232ThNumManDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d232ThConstMan,HALFLIFE232TH,system->dAge);
}

double fdD238UNumManDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d238UConstMan,HALFLIFE238U,system->dAge);
}

double fdD235UNumManDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d235UConstMan,HALFLIFE235U,system->dAge);
}

double fdD40KNumCoreDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d40KConstCore,HALFLIFE40K,system->dAge);
}

double fdD232ThNumCoreDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d232ThConstCore,HALFLIFE232TH,system->dAge);
}

double fdD238UNumCoreDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d238UConstCore,HALFLIFE238U,system->dAge);
}

double fdD235UNumCoreDt(BODY *body,SYSTEM *system,int *iaBody,int iNumBodies) {
  return fdDNumRadDt(body[iaBody[0]].d235UConstCore,HALFLIFE235U,system->dAge);
}
