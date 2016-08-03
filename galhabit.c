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
  
  sprintf(options[OPT_ENCOUNTERRAD].cName,"dEncounterRad");
  sprintf(options[OPT_ENCOUNTERRAD].cDescr,"Radius at which stellar encounters occur");
  sprintf(options[OPT_ENCOUNTERRAD].cDefault,"206265 AU"); 
  options[OPT_ENCOUNTERRAD].dDefault = 206265.0*AUCM;
  options[OPT_ENCOUNTERRAD].iType = 2;  
  options[OPT_ENCOUNTERRAD].iMultiFile = 0;   
  fnRead[OPT_ENCOUNTERRAD] = &ReadEncounterRad;
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
  double dSigma;
  
  srand(system->iSeed);
  
  if (iBody == 1) {
    system->dPassingStarR = malloc(3*sizeof(double));
    system->dPassingStarV = malloc(3*sizeof(double));
    system->dEncounterTime = 13.1/1e6/YEARSEC;  //need to update this, most likely XXX
    system->dDeltaTEnc = 0.0;
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
  double dpEnc, dp;

  while (body[iBody].dArgP > 2*PI) {
    body[iBody].dArgP -= 2*PI;
  }
  while (body[iBody].dArgP < 0) {
    body[iBody].dArgP += 2*PI;
  }
  
  system->dDeltaTEnc += evolve->dCurrentDt;
  dpEnc = 1.0-exp(-system->dEncounterTime*system->dDeltaTEnc); //probability encounter will occur
  dp = random_double();
  if (dp < dpEnc) {
    GetStarPosition(system);
    GetStarMass(system);
    GetStarVelocity(system); 
    /* next calculate impact parameter */
    
    /* then move the orbiter, get all distances/velocities, check for disruption */
    
    /* apply the impulse */
    
    /* reset the DeltaT */
    system->dDeltaTEnc = 0.0;
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
  VelocityDisp(system);
  dSigma = system->dPassingStarSigma;
  
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

double mag2mass(double dMagV) {
  double dlogMass;
  
  if (dMagV > 0) {
    dlogMass = -0.0928*dMagV + 0.448;
  } else { 
    dlogMass = -0.271*dMagV + 0.448;
  }
  
  return pow(10.0,dlogMass);
}

void VelocityDisp(SYSTEM* system) {
  double dSigma, dMagV;
  
  dMagV = system->dPassingStarMagV;
  
  if (dMagV <= -2) {
    dSigma = 8.5;
  } else if ((dMagV > -2) && (dMagV <= 0)) {
    dSigma = 11.4;
  } else if ((dMagV > 0) && (dMagV <= 2)) {
    dSigma = 13.7;
  } else if ((dMagV > 2) && (dMagV <= 3)) {
    dSigma = 16.8;
  } else if ((dMagV > 3) && (dMagV <= 4)) {
    dSigma = 20.9;
  } else if ((dMagV > 4) && (dMagV <= 5)) {
    dSigma = 22.6;
  } else if ((dMagV > 5) && (dMagV <= 6)) {
    dSigma = 24.0;
  } else if ((dMagV > 6) && (dMagV <= 7)) {
    dSigma = 25.0;
  } else if ((dMagV > 7) && (dMagV <= 9)) {
    dSigma = 24.7;  
  } else if ((dMagV > 9) && (dMagV <= 15)) {
    dSigma = 24.1;  
  }
  
  system->dPassingStarSigma =  dSigma;
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

void GetStarMass(SYSTEM *system) {
  double ns = 0, dTmp = 100, dMagV;
  
  while (dTmp > ns) {
    dMagV = (double)(random_int(19)-3); //draw stellar magnitude (-3<dMagV<15)
    dTmp = random_double()*0.014;       //if dTmp exceeds the number density, reject dMagV
    ns = NearbyStarDist(dMagV);         //get number density at dMagV
  }
  
  system->dPassingStarMagV = dMagV;
  //now get the mass of the star
  system->dPassingStarMass = mag2mass(dMagV);
}

void GetStarPosition(SYSTEM *system) {
  double r = system->dEncounterRad, costheta, phi, sintheta;
  
  costheta = random_double()*2 - 1;
  sintheta = sqrt(fabs(1.0-pow(costheta,2)));
  phi = random_double()*2*PI;
  
  system->dPassingStarR[0] = r*sintheta*cos(phi);
  system->dPassingStarR[1] = r*sintheta*sin(phi);
  system->dPassingStarR[2] = r*costheta;
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
        m = (double)(random_int(19)-3);
        y = random_double()*0.014;
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

