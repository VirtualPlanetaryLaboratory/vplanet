#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "vplanet.h"

void InitializeUpdateBodyPerts(CONTROL *control,UPDATE *update,int iBody) {
  int iVar,iEqn;

  for (iVar=0;iVar<update[iBody].iNumVars;iVar++) {
    for (iEqn=0;iEqn<update[iBody].iNumEqns[iVar];iEqn++) {
      control->Evolve.tmpUpdate[iBody].iaBody[iVar][iEqn] = malloc(update[iBody].iNumBodies[iVar][iEqn]);
    }
  }
}

void InitializeUpdateTmpBody(BODY *body,CONTROL *control,MODULE *module,UPDATE *update,int iBody) {
  int iModule;

  for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
    module->fnInitializeUpdateTmpBody[iBody][iModule](body,control,update,iBody);
}

void UpdateCopy(UPDATE *dest,UPDATE *src,int iNumBodies) {
  int iBody,iVar,iEqn,iPert;

  for (iBody=0;iBody<iNumBodies;iBody++) {
    dest[iBody].iNumEcc = src[iBody].iNumEcc;
    dest[iBody].iEcc = src[iBody].iEcc;

    dest[iBody].iNum40K =src[iBody].iNum40K;
    dest[iBody].iNum40K =src[iBody].iNum40K;

    dest[iBody].iNum232Th =src[iBody].iNum232Th;
    dest[iBody].iNum232Th =src[iBody].iNum232Th;

    dest[iBody].iNum238U =src[iBody].iNum238U;
    dest[iBody].iNum238U =src[iBody].iNum238U;

    dest[iBody].iNum235U =src[iBody].iNum235U; //PED
    dest[iBody].iNum235U =src[iBody].iNum235U;

    dest[iBody].iNumObl = src[iBody].iNumObl;
    dest[iBody].iObl = src[iBody].iObl;

    dest[iBody].iNumRot = src[iBody].iNumRot;
    dest[iBody].iRot = src[iBody].iRot;

    dest[iBody].iNumSemi = src[iBody].iNumSemi;
    dest[iBody].iSemi = src[iBody].iSemi;

    dest[iBody].iNumVars = src[iBody].iNumVars;

    for (iVar=0;iVar<src[iBody].iNumVars;iVar++) {
      dest[iBody].iNumEqns[iVar] = src[iBody].iNumEqns[iVar];
      dest[iBody].iaVar[iVar] = src[iBody].iaVar[iVar];

      for (iEqn=0;iEqn<src[iBody].iNumEqns[iVar];iEqn++) {
	dest[iBody].iNumBodies[iVar][iEqn] = src[iBody].iNumBodies[iVar][iEqn];
	dest[iBody].iaType[iVar][iEqn] = src[iBody].iaType[iVar][iEqn];
	dest[iBody].iaModule[iVar][iEqn] = src[iBody].iaModule[iVar][iEqn];
	for (iPert=0;iPert<src[iBody].iNumBodies[iVar][iEqn];iPert++)
	  dest[iBody].iaBody[iVar][iEqn][iPert] = src[iBody].iaBody[iVar][iEqn][iPert];
      }
    }
  }
}

void InitializeUpdate(BODY*body,CONTROL *control,MODULE *module,UPDATE *update,fnUpdateVariable ****fnUpdate) {
  int iBody,iBodyPert,iVar,iEqn,iModule;
  int iNum,iSubStep;

  // Allocate the firt dimension of the Update matrix to be the number of bodies
  *fnUpdate=malloc(control->Evolve.iNumBodies*sizeof(fnUpdateVariable**));

  // For each body, determine how many independent variables need updating.
  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    update[iBody].iaModule = malloc(module->iNumModules[iBody]*sizeof(int));

    // Assume no modules to start
    update[iBody].iNumEcc=0;
    update[iBody].iNum40K=0;
    update[iBody].iNum232Th=0;
    update[iBody].iNum238U=0;
    update[iBody].iNum235U=0; //PED
    update[iBody].iNumObl=0;
    update[iBody].iNumRot=0;
    update[iBody].iNumSemi=0;
    update[iBody].iNumVars=0;
    
    /* First we must identify how many variables and models must be 
       assigned so we can malloc the update struct. */
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
      module->fnInitializeUpdate[iBody][iModule](body,update,iBody);

    // The second dimension of the Update matrix is the variables
    (*fnUpdate)[iBody]=malloc(update[iBody].iNumVars*sizeof(fnUpdateVariable*));
    update[iBody].iaVar = malloc(update[iBody].iNumVars*sizeof(int));
    update[iBody].iNumEqns = malloc(update[iBody].iNumVars*sizeof(double));
    update[iBody].iaType = malloc(update[iBody].iNumVars*sizeof(int*));
    update[iBody].iaModule = malloc(update[iBody].iNumVars*sizeof(int*));
    update[iBody].pdVar = malloc(update[iBody].iNumVars*sizeof(double*));
    update[iBody].daDeriv = malloc(update[iBody].iNumVars*sizeof(double*));
    update[iBody].daDerivProc = malloc(update[iBody].iNumVars*sizeof(double*));
    update[iBody].iNumBodies = malloc(update[iBody].iNumVars*sizeof(int*));
    update[iBody].iaBody = malloc(update[iBody].iNumVars*sizeof(int**));

    // May also have to allocate space for the temp UPDATE
    if (control->Evolve.iOneStep == RUNGEKUTTA) {
      control->Evolve.tmpUpdate[iBody].iaVar = malloc(update[iBody].iNumVars*sizeof(int));
      control->Evolve.tmpUpdate[iBody].iNumEqns = malloc(update[iBody].iNumVars*sizeof(double));
      control->Evolve.tmpUpdate[iBody].iaType = malloc(update[iBody].iNumVars*sizeof(int*));
      control->Evolve.tmpUpdate[iBody].iaModule = malloc(update[iBody].iNumVars*sizeof(int*));
      control->Evolve.tmpUpdate[iBody].pdVar = malloc(update[iBody].iNumVars*sizeof(double*));
      control->Evolve.tmpUpdate[iBody].daDeriv = malloc(update[iBody].iNumVars*sizeof(double*));
      control->Evolve.tmpUpdate[iBody].daDerivProc = malloc(update[iBody].iNumVars*sizeof(double*));
      control->Evolve.tmpUpdate[iBody].iNumBodies = malloc(update[iBody].iNumVars*sizeof(int*));
      control->Evolve.tmpUpdate[iBody].iaBody = malloc(update[iBody].iNumVars*sizeof(int**));
    }
    for (iSubStep=0;iSubStep<4;iSubStep++) 
      control->Evolve.daDeriv[iSubStep][iBody]=malloc(update[iBody].iNumVars*sizeof(double));

    // Eccentricity

    iVar=0;
    update[iBody].iEcc = -1; // Assume not tracked
    if (update[iBody].iNumEcc) {
      // See vplanet.h for description of Update members
      update[iBody].iEcc = iVar;
      update[iBody].iaVar[iVar] = VECC;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumEcc;
      // XXX assign pointer out of if-then?
      update[iBody].pdVar[iVar] = &body[iBody].dEcc;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumEcc*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumEcc*sizeof(int*));

      update[iBody].iaType[iVar] = malloc(update[iBody].iNumEcc*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumEcc*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dEcc;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumEcc*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumEcc*sizeof(int*));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumEcc*sizeof(double));
	control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumEcc*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumEcc*sizeof(int));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdateEcc[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    // Radiogenic Isotopes
    
    // Num40KNum -- Sort out!!

    update[iBody].i40K = -1;
    if (update[iBody].iNum40K) {
      update[iBody].i40K = iVar;
      update[iBody].iaVar[iVar] = VNUM40K;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum40K;
      update[iBody].pdVar[iVar] = &body[iBody].d40KNum;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum40K*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum40K*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum40K*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum40K*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d40KNum;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum40K*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum40K*sizeof(double));
	  control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum40K*sizeof(int));
	  control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum40K*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum40K*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdate40KNum[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    update[iBody].i232Th = -1;
    if (update[iBody].iNum232Th) {
      update[iBody].i232Th = iVar;
      update[iBody].iaVar[iVar] = VNUM232TH;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum232Th;
      update[iBody].pdVar[iVar] = &body[iBody].d232ThNum;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum232Th*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum232Th*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum232Th*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum232Th*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d232ThNum;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum232Th*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum232Th*sizeof(double));
	  control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum232Th*sizeof(int));
	  control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum232Th*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum232Th*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdate232ThNum[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    update[iBody].i238U = -1;
    if (update[iBody].iNum238U) {
      update[iBody].i238U = iVar;
      update[iBody].iaVar[iVar] = VNUM238U;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum238U;
      update[iBody].pdVar[iVar] = &body[iBody].d238UNum;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum238U*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum238U*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum238U*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum238U*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d238UNum;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum238U*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum238U*sizeof(double));
	  control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum238U*sizeof(int));
	  control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum238U*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum238U*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdate238UNum[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

        
    update[iBody].i235U = -1;   //PED
    if (update[iBody].iNum235U) {
      update[iBody].i235U = iVar;
      update[iBody].iaVar[iVar] = VNUM235U;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum235U;
      update[iBody].pdVar[iVar] = &body[iBody].d235UNum;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum235U*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum235U*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum235U*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum235U*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d235UNum;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum235U*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum235U*sizeof(double));
	  control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum235U*sizeof(int));
	  control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum235U*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum235U*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdate235UNum[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // Obliquity
    
    update[iBody].iObl = -1;
    if (update[iBody].iNumObl) {
      update[iBody].iObl = iVar;
      update[iBody].iaVar[iVar] = VOBL;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumObl;
      update[iBody].pdVar[iVar] = &body[iBody].dObliquity;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumObl*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumObl*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumObl*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumObl*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dObliquity;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumObl*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumObl*sizeof(double));
	control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumObl*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumObl*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumObl*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdateObl[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // Rotation Rate

    update[iBody].iRot = -1;
    if (update[iBody].iNumRot) {
      update[iBody].iRot = iVar;
      update[iBody].iaVar[iVar] = VROT;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumRot;
      update[iBody].pdVar[iVar] = &body[iBody].dRotRate;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumRot*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumRot*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumRot*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumRot*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dRotRate;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumRot*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumRot*sizeof(double));
	control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumRot*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumRot*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumRot*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdateRot[iBody][iModule](body,update,&iEqn,iVar,iBody);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // Semi-major Axis

    update[iBody].iSemi = -1;
    if (update[iBody].iNumSemi) {
      update[iBody].iSemi = iVar;
      update[iBody].iaVar[iVar] = VSEMI;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumSemi;
      update[iBody].pdVar[iVar] = &body[iBody].dSemi;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumSemi*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
	control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dSemi;
	control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
	control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumSemi*sizeof(double));
	control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumSemi*sizeof(int));
	control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumSemi*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
	module->fnFinalizeUpdateSemi[iBody][iModule](body,update,&iEqn,iVar,iBody);

      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
  }
}
