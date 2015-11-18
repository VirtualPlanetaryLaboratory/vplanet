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
    // The iNumX and iX members don't need to be copied

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
  int iBody,iBodyPert,iVar,iEqn,iModule,iLat,iFoo;
  int iNum,iSubStep;

  // Allocate the firt dimension of the Update matrix to be the number of bodies
  *fnUpdate=malloc(control->Evolve.iNumBodies*sizeof(fnUpdateVariable**));

  // For each body, determine how many independent variables need updating.
  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    update[iBody].iaModule = malloc(module->iNumModules[iBody]*sizeof(int));

    // Assume no modules to start
    //update[iBody].iNumEcc=0;
    update[iBody].iNum40KMan=0;
    update[iBody].iNum40KCore=0;
    update[iBody].iNum232ThMan=0;
    update[iBody].iNum232ThCore=0;
    update[iBody].iNum235UCore=0;
    update[iBody].iNum235UMan=0;
    update[iBody].iNum238UCore=0;
    update[iBody].iNum238UMan=0;
    update[iBody].iNumHecc=0;
    update[iBody].iNumKecc=0;
    update[iBody].iNumPinc=0;
    update[iBody].iNumQinc=0;
    update[iBody].iNumRot=0;
    update[iBody].iNumSemi=0;
    update[iBody].iNumTMan=0;
    update[iBody].iNumTCore=0;
    update[iBody].iNumXobl=0;
    update[iBody].iNumYobl=0;
    update[iBody].iNumZobl=0;
    update[iBody].iNumSurfaceWaterMass=0;
    update[iBody].iNumEnvelopeMass=0;
    update[iBody].iNumLuminosity=0;
    update[iBody].iNumTemperature=0;
    update[iBody].iNumRadius=0;
    update[iBody].iNumMass=0;
    update[iBody].iNumIceMass=0;

    update[iBody].iNumVars=0;
    
    /* XXX Hack to get distorb working. Need iGravPerts for following arrays */
    if (body[iBody].bDistOrb) {
      if (control->Evolve.iDistOrbModel == RD4) {
        body[iBody].iGravPerts = control->Evolve.iNumBodies - 2;
        body[iBody].iDistOrbModel = RD4;
      } else if (control->Evolve.iDistOrbModel == LL2) {
        /* "Perturbers" in LL2 correspond to eigenfrequencies, not planet pairs. 
           Number of eigenfrequencies = number of planets. */
        body[iBody].iGravPerts = control->Evolve.iNumBodies - 1;
        body[iBody].iDistOrbModel = LL2;
      }
    }
    
    /* First we must identify how many variables and models must be 
       assigned so we can malloc the update struct. */
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
      module->fnInitializeUpdate[iBody][iModule](body,update,iBody);

    // The second dimension of the Update matrix is the variables
    (*fnUpdate)[iBody]=malloc(update[iBody].iNumVars*sizeof(fnUpdateVariable*));
    update[iBody].iaVar = malloc(update[iBody].iNumVars*sizeof(int));
    update[iBody].iNumEqns = malloc(update[iBody].iNumVars*sizeof(int));
    update[iBody].iaType = malloc(update[iBody].iNumVars*sizeof(int*));
    update[iBody].iaModule = malloc(update[iBody].iNumVars*sizeof(int*));
    update[iBody].pdVar = malloc(update[iBody].iNumVars*sizeof(double*));
    update[iBody].daDeriv = malloc(update[iBody].iNumVars*sizeof(double));
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

    /* Now we malloc some pointers, and perform some initializations for the
       UPDATE struct based on the primary variables required for each's 
       planet's assigned modules. */

    iVar=0;

    /* 40K Core */
    update[iBody].i40KCore = -1;
    if (update[iBody].iNum40KCore) {
      update[iBody].i40KCore = iVar;
      update[iBody].iaVar[iVar] = VNUM40KCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum40KCore;
      update[iBody].pdVar[iVar] = &body[iBody].d40KNumCore;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d40KNumCore;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum40KCore*sizeof(double));
          control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int));
          control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum40KCore*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdate40KNumCore[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    /* 40K Mantle */
    update[iBody].i40KMan = -1;
    if (update[iBody].iNum40KMan) {
      update[iBody].i40KMan = iVar;
      update[iBody].iaVar[iVar] = VNUM40KMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum40KMan;
      update[iBody].pdVar[iVar] = &body[iBody].d40KNumMan;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d40KNumMan;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum40KMan*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum40KMan*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdate40KNumMan[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
     
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    /* 232Th Core */    
    update[iBody].i232ThCore = -1;
    if (update[iBody].iNum232ThCore) {
      update[iBody].i232ThCore = iVar;
      update[iBody].iaVar[iVar] = VNUM232THCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum232ThCore;
      update[iBody].pdVar[iVar] = &body[iBody].d232ThNumCore;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d232ThNumCore;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(double));
          control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int));
          control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum232ThCore*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdate232ThNumCore[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    /* 232Th Mantle */    
    update[iBody].i232ThMan = -1;
    if (update[iBody].iNum232ThMan) {
      update[iBody].i232ThMan = iVar;
      update[iBody].iaVar[iVar] = VNUM232THMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum232ThMan;
      update[iBody].pdVar[iVar] = &body[iBody].d232ThNumMan;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d232ThNumMan;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(double));
          control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int));
          control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum232ThMan*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdate232ThNumMan[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    /* 235U Core */            
    update[iBody].i235UCore = -1;  
    if (update[iBody].iNum235UCore) {
      update[iBody].i235UCore = iVar;
      update[iBody].iaVar[iVar] = VNUM235UCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum235UCore;
      update[iBody].pdVar[iVar] = &body[iBody].d235UNumCore;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d235UNumCore;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum235UCore*sizeof(double));
          control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int));
          control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum235UCore*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdate235UNumCore[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    /* 235U Mantle */            
    update[iBody].i235UMan = -1;  
    if (update[iBody].iNum235UMan) {
      update[iBody].i235UMan = iVar;
      update[iBody].iaVar[iVar] = VNUM235UMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum235UMan;
      update[iBody].pdVar[iVar] = &body[iBody].d235UNumMan;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d235UNumMan;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum235UMan*sizeof(double));
          control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int));
          control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum235UMan*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdate235UNumMan[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    /* 238U Core */    
    update[iBody].i238UCore = -1;
    if (update[iBody].iNum238UCore) {
      update[iBody].i238UCore = iVar;
      update[iBody].iaVar[iVar] = VNUM238UCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum238UCore;
      update[iBody].pdVar[iVar] = &body[iBody].d238UNumCore;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d238UNumCore;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum238UCore*sizeof(double));
          control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int));
          control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum238UCore*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdate238UNumCore[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    /* 238U Mantle */    
    update[iBody].i238UMan = -1;
    if (update[iBody].iNum238UMan) {
      update[iBody].i238UMan = iVar;
      update[iBody].iaVar[iVar] = VNUM238UMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNum238UMan;
      update[iBody].pdVar[iVar] = &body[iBody].d238UNumMan;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].d238UNumMan;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNum238UMan*sizeof(double));
          control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int));
          control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNum238UMan*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdate238UNumMan[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // Atmospheric Envelope Mass
    update[iBody].iEnvelopeMass = -1;
    if (update[iBody].iNumEnvelopeMass) {
      update[iBody].iEnvelopeMass = iVar;
      update[iBody].iaVar[iVar] = VENVELOPEMASS;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumEnvelopeMass;
      update[iBody].pdVar[iVar] = &body[iBody].dEnvelopeMass;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumEnvelopeMass*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumEnvelopeMass*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumEnvelopeMass*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumEnvelopeMass*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dEnvelopeMass;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumEnvelopeMass*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumEnvelopeMass*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumEnvelopeMass*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumEnvelopeMass*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumEnvelopeMass*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateEnvelopeMass[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // Poincare's h (Hecc)
    update[iBody].iHecc = -1;
    if (update[iBody].iNumHecc) {
      update[iBody].iHecc = iVar;
      update[iBody].iaVar[iVar] = VHECC;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumHecc;
      update[iBody].pdVar[iVar] = &body[iBody].dHecc;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumHecc*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumHecc*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumHecc*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumHecc*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dHecc;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumHecc*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumHecc*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumHecc*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumHecc*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumHecc*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateHecc[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);

      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // Poincare's k (Kecc)
    update[iBody].iKecc = -1;
    if (update[iBody].iNumKecc) {
      update[iBody].iKecc = iVar;
      update[iBody].iaVar[iVar] = VKECC;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumKecc;
      update[iBody].pdVar[iVar] = &body[iBody].dKecc;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumKecc*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumKecc*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumKecc*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumKecc*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dKecc;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumKecc*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumKecc*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumKecc*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumKecc*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumKecc*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateKecc[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);

      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
      
    // Luminosity:
    update[iBody].iLuminosity = -1;
    if (update[iBody].iNumLuminosity) {
      update[iBody].iLuminosity = iVar;
      update[iBody].iaVar[iVar] = VLUMINOSITY;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumLuminosity;
      update[iBody].pdVar[iVar] = &body[iBody].dLuminosity;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumLuminosity*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumLuminosity*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumLuminosity*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumLuminosity*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dLuminosity;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumLuminosity*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumLuminosity*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumLuminosity*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumLuminosity*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumLuminosity*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateLuminosity[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }  
    
    /* Obsolete!
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
    */
    
    // Poincare's p (Pinc)
    update[iBody].iPinc = -1;
    if (update[iBody].iNumPinc) {
      update[iBody].iPinc = iVar;
      update[iBody].iaVar[iVar] = VPINC;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumPinc;
      update[iBody].pdVar[iVar] = &body[iBody].dPinc;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumPinc*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumPinc*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumPinc*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumPinc*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dPinc;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumPinc*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumPinc*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumPinc*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumPinc*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumPinc*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdatePinc[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);

      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // Poincare's q (Qinc)
    update[iBody].iQinc = -1;
    if (update[iBody].iNumQinc) {
      update[iBody].iQinc = iVar;
      update[iBody].iaVar[iVar] = VQINC;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumQinc;
      update[iBody].pdVar[iVar] = &body[iBody].dQinc;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumQinc*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumQinc*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumQinc*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumQinc*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dQinc;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumQinc*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumQinc*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumQinc*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumQinc*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumQinc*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateQinc[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);

      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    // Radius:
    update[iBody].iRadius = -1;
    if (update[iBody].iNumRadius) {
      update[iBody].iRadius = iVar;
      update[iBody].iaVar[iVar] = VRADIUS;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumRadius;
      update[iBody].pdVar[iVar] = &body[iBody].dRadius;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumRadius*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumRadius*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumRadius*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumRadius*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dRadius;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumRadius*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumRadius*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumRadius*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumRadius*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumRadius*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateRadius[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // Mass:
    update[iBody].iMass = -1;
    if (update[iBody].iNumMass) {
      update[iBody].iMass = iVar;
      update[iBody].iaVar[iVar] = VMASS;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumMass;
      update[iBody].pdVar[iVar] = &body[iBody].dMass;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumMass*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumMass*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumMass*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumMass*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dMass;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumMass*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumMass*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumMass*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumMass*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumMass*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateMass[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
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
        module->fnFinalizeUpdateRot[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
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
        module->fnFinalizeUpdateSemi[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);

      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }  
    
    // Surface Water Mass
    update[iBody].iSurfaceWaterMass = -1;
    if (update[iBody].iNumSurfaceWaterMass) {
      update[iBody].iSurfaceWaterMass = iVar;
      update[iBody].iaVar[iVar] = VSURFACEWATERMASS;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumSurfaceWaterMass;
      update[iBody].pdVar[iVar] = &body[iBody].dSurfaceWaterMass;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumSurfaceWaterMass*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumSurfaceWaterMass*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumSurfaceWaterMass*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumSurfaceWaterMass*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dSurfaceWaterMass;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumSurfaceWaterMass*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumSurfaceWaterMass*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumSurfaceWaterMass*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumSurfaceWaterMass*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumSurfaceWaterMass*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateSurfaceWaterMass[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    // Stellar Temperature:
    update[iBody].iTemperature = -1;
    if (update[iBody].iNumTemperature) {
      update[iBody].iTemperature = iVar;
      update[iBody].iaVar[iVar] = VTEMPERATURE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumTemperature;
      update[iBody].pdVar[iVar] = &body[iBody].dTemperature;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumTemperature*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumTemperature*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumTemperature*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumTemperature*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dTemperature;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumTemperature*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumTemperature*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumTemperature*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumTemperature*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumTemperature*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateTemperature[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }  

    /* Core Temperature (TCore) */
    update[iBody].iTCore = -1;
    if (update[iBody].iNumTCore) {
      update[iBody].iTCore = iVar;
      update[iBody].iaVar[iVar] = VTCORE;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumTCore;
      update[iBody].pdVar[iVar] = &body[iBody].dTCore;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumTCore*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumTCore*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumTCore*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumTCore*sizeof(int));
      
      if (control->Evolve.iOneStep == RUNGEKUTTA) {             
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dTCore;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumTCore*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumTCore*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumTCore*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumTCore*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumTCore*sizeof(int*));
      }
      
      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateTCore[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }

    /* Mantle Temperature (TMan) */
    update[iBody].iTMan = -1;
    if (update[iBody].iNumTMan) {
      update[iBody].iTMan = iVar;
      update[iBody].iaVar[iVar] = VTMAN;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumTMan;
      update[iBody].pdVar[iVar] = &body[iBody].dTMan;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumTMan*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumTMan*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumTMan*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumTMan*sizeof(int));
      
      if (control->Evolve.iOneStep == RUNGEKUTTA) {             
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dTMan;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumTMan*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumTMan*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumTMan*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumTMan*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumTMan*sizeof(int*));
      }
      
      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateTMan[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);
      
      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // DistRot's X (Xobl)
    update[iBody].iXobl = -1;
    if (update[iBody].iNumXobl) {
      update[iBody].iXobl = iVar;
      update[iBody].iaVar[iVar] = VXOBL;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumXobl;
      update[iBody].pdVar[iVar] = &body[iBody].dXobl;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumXobl*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumXobl*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumXobl*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumXobl*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dXobl;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumXobl*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumXobl*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumXobl*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumXobl*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumXobl*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateXobl[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);

      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // DistRot's Y (Yobl)
    update[iBody].iYobl = -1;
    if (update[iBody].iNumYobl) {
      update[iBody].iYobl = iVar;
      update[iBody].iaVar[iVar] = VYOBL;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumYobl;
      update[iBody].pdVar[iVar] = &body[iBody].dYobl;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumYobl*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumYobl*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumYobl*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumYobl*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dYobl;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumYobl*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumYobl*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumYobl*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumYobl*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumYobl*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateYobl[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);

      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    // DistRot's Z (Zobl)
    update[iBody].iZobl = -1;
    if (update[iBody].iNumZobl) {
      update[iBody].iZobl = iVar;
      update[iBody].iaVar[iVar] = VZOBL;
      update[iBody].iNumEqns[iVar] = update[iBody].iNumZobl;
      update[iBody].pdVar[iVar] = &body[iBody].dZobl;
      update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumZobl*sizeof(int));
      update[iBody].iaBody[iVar] = malloc(update[iBody].iNumZobl*sizeof(int*));
      update[iBody].iaType[iVar] = malloc(update[iBody].iNumZobl*sizeof(int));
      update[iBody].iaModule[iVar] = malloc(update[iBody].iNumZobl*sizeof(int));

      if (control->Evolve.iOneStep == RUNGEKUTTA) {
        control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].dZobl;
        control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumZobl*sizeof(int));
        control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumZobl*sizeof(double));
        control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumZobl*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumZobl*sizeof(int));
        control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumZobl*sizeof(int*));
      }

      iEqn=0;
      for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
        module->fnFinalizeUpdateZobl[iBody][iModule](body,update,&iEqn,iVar,iBody,iFoo);

      (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
      update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
      iVar++;
    }
    
    
    // POISE's ice mass
    update[iBody].iIceMass = -1;
    /* XXX hack to get ice sheets working, since since these don't get malloced until verify */
    control->Evolve.tmpBody[iBody].daIceMass = malloc(body[iBody].iNumLats*sizeof(double));
    body[iBody].daIceMass = malloc(body[iBody].iNumLats*sizeof(double)); 
    if (update[iBody].iNumIceMass) { 
      update[iBody].iIceMass = iVar;
      for (iLat=0;iLat<body[iBody].iNumLats;iLat++) {
        update[iBody].iaIceMass[iLat] = iVar;
        update[iBody].iaVar[iVar] = VICEMASS;
        update[iBody].iNumEqns[iVar] = 1;
        update[iBody].pdVar[iVar] = &body[iBody].daIceMass[iLat];
        update[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumIceMass*sizeof(int));
        update[iBody].iaBody[iVar] = malloc(update[iBody].iNumIceMass*sizeof(int*));
        update[iBody].iaType[iVar] = malloc(update[iBody].iNumIceMass*sizeof(int));
        update[iBody].iaModule[iVar] = malloc(update[iBody].iNumIceMass*sizeof(int));

        if (control->Evolve.iOneStep == RUNGEKUTTA) {
          control->Evolve.tmpUpdate[iBody].pdVar[iVar] = &control->Evolve.tmpBody[iBody].daIceMass[iLat];
          control->Evolve.tmpUpdate[iBody].iNumBodies[iVar] = malloc(update[iBody].iNumIceMass*sizeof(int));
          control->Evolve.tmpUpdate[iBody].daDerivProc[iVar] = malloc(update[iBody].iNumIceMass*sizeof(double));
          control->Evolve.tmpUpdate[iBody].iaType[iVar] = malloc(update[iBody].iNumIceMass*sizeof(int));
          control->Evolve.tmpUpdate[iBody].iaModule[iVar] = malloc(update[iBody].iNumIceMass*sizeof(int));
          control->Evolve.tmpUpdate[iBody].iaBody[iVar] = malloc(update[iBody].iNumIceMass*sizeof(int*));
        }
    
        iEqn=0;
        for (iModule=0;iModule<module->iNumModules[iBody];iModule++) 
          module->fnFinalizeUpdateIceMass[iBody][iModule](body,update,&iEqn,iVar,iBody,iLat);
      
        (*fnUpdate)[iBody][iVar]=malloc(iEqn*sizeof(fnUpdateVariable));
        update[iBody].daDerivProc[iVar]=malloc(iEqn*sizeof(double));
        iVar++;
      }
      
    }
  }
}
