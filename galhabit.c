/********************** GALHABIT.C **********************/
/*
 * Russell Deitrick, July 7, 2015
 *
 * Subroutines that simulate the galactic habitat of a stellar system
*/

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

void InitializeOptionsGalHabit(OPTIONS *options,fnReadOption fnRead[]) {
  sprintf(options[OPT_GALACDENSITY].cName,"dGalacDensity");
  sprintf(options[OPT_GALACDENSITY].cDescr,"Density of galactic environment");
  sprintf(options[OPT_GALACDENSITY].cDefault,"0.185"); //need to find updated value
  options[OPT_GALACDENSITY].dDefault = 0.185;
  options[OPT_GALACDENSITY].iType = 2;  
  options[OPT_GALACDENSITY].iMultiFile = 0;   
  fnRead[OPT_GALACDENSITY] = &ReadGalacDensity;
  
  sprintf(options[OPT_RANDSEED].cName,"iRandSeed");
  sprintf(options[OPT_RANDSEED].cDescr,"Seed for random number generator (stellar encounters)");
  sprintf(options[OPT_RANDSEED].cDefault,"42"); //need to find updated value
  options[OPT_RANDSEED].dDefault = 42;
  options[OPT_RANDSEED].iType = 1;  
  options[OPT_RANDSEED].iMultiFile = 0;   
  fnRead[OPT_RANDSEED] = &ReadRandSeed;
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
  
  srand(system->iSeed);
  
  for (i=0;i<=20;i++) {
    
  
  }

  if (iBody >= 1) {
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

void CountHaltsGalHabit(HALT *halt,int *iNumHalts) { 
}

void VerifyHaltGalHabit(BODY *body,CONTROL *control,OPTIONS *options,int iBody,int *iHalt) {
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
  
void InitializeOutputGalHabit(OUTPUT *output,fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_PERIQ].cName,"PeriQ");
  sprintf(output[OUT_PERIQ].cDescr,"Pericenter distance");
  sprintf(output[OUT_PERIQ].cNeg,"AU");
  output[OUT_PERIQ].bNeg = 1;
  output[OUT_PERIQ].dNeg = 1./AUCM;
  output[OUT_PERIQ].iNum = 1;
  output[OUT_PERIQ].iModuleBit = GALHABIT;
  fnWrite[OUT_PERIQ] = &WriteBodyPeriQ;
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
  while (body[iBody].dArgP > 2*PI) {
    body[iBody].dArgP -= 2*PI;
  }
  while (body[iBody].dArgP < 0) {
    body[iBody].dArgP += 2*PI;
  }
}

double random_double() {
  double n;
  
  n = (double)rand()/RAND_MAX;
  return n;
}

double StarVelocities(SYSTEM *system, double dSigma) {
  /* get passing star velocities from dispersion = dSigma, using Box-Muller method*/
  double u1, u2, z0, z1;
  
  u1 = random_double();
  u2 = random_double();
  
  z0 = sqrt(-2.0*log(u1))*cos(2.0*PI*u2);
  z1 = sqrt(-2.0*log(u1))*sin(2.0*PI*u2);
  
  system->dPassingStarV[0] = z0*dSigma;
  system->dPassingStarV[1] = z1*dSigma;
  
  u1 = random_double();
  u2 = random_double();
  
  z0 = sqrt(-2.0*log(u1))*cos(2.0*PI*u2);
  
  system->dPassingStarV[2] = z0*dSigma;
}

double nsMinus6to15(double dMagV) {
  /* distribution of stars with mag: -6 <= MV <= 15 from Heisler, Tremaine & Alcock 1987
     !!! may need to be updated !!! */
  double dNs;
  
  dNs = 0.00358*pow(10.,(0.04*dMagV))/pow(1.0+1.8345*pow(10.,(-0.21*dMagV)),3.4) * \
        (1.0 - 0.15*pow(10.,(-0.25*fabs(dMagV-15.0))));
  
  return dNs;
}

double NearbyStarDist(double dMagV) {
  double dNs;
  
  if (dMagV <= -6.0) {
    dNs = 0.0;
  } else if (dMagV > 19.0) {
    dNs = 0.0;
  } else if (dMagV > 15.0) {
    dNs = nsMinus6to15(15.0);
  } else {
    dNs = nsMinus6to15(dMagV);
  }
  
  return dNs;
}

//--------------Galactic stuff!--------------------------------------------------------------

double fdGalHabitDPeriQDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dRho = system->dGalacDensity/pow(AUPC,3), dMu, dEcc;
  dMu = KGAUSS*KGAUSS*body[0].dMass/MSUN; //calculate mass coefficient for primary/primary+secondary
  dEcc = 1.0 - body[iaBody[0]].dPeriQ/body[iaBody[0]].dSemi; //calculate orbiter's eccentricity
  
  return -pow(body[iaBody[0]].dSemi/AUCM,5./2)*sqrt((1.0-pow(dEcc,2))/dMu)*5*PI*KGAUSS*KGAUSS*\
      dRho*dEcc*pow(sin(body[iaBody[0]].dInc),2)*sin(2*body[iaBody[0]].dArgP)/DAYSEC*AUCM;
}

double fdGalHabitDArgPDt(BODY *body, SYSTEM *system, int *iaBody) {
  double dRho = system->dGalacDensity/pow(AUPC,3), dMu, dEcc;
  dMu = KGAUSS*KGAUSS*body[0].dMass/MSUN; //calculate mass coefficient for primary/primary+secondary
  dEcc = 1.0 - body[iaBody[0]].dPeriQ/body[iaBody[0]].dSemi; //calculate orbiter's eccentricity

  return 2*PI*KGAUSS*KGAUSS*dRho*sqrt(pow(body[iaBody[0]].dSemi/AUCM,3)/(dMu*(1.0-pow(dEcc,2))))*\
      (1.-pow(dEcc,2)-5.*(1.-pow(dEcc,2)-pow(cos(body[iaBody[0]].dInc),2))*\
      pow(sin(body[iaBody[0]].dArgP),2.0))/DAYSEC;
}

