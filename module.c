/******************** MODULE.C **************************/
/*
 * Rory Barnes, Mon Jan 19 13:00:54 PST 2015
 *
 * Functions to manipulate the MODULE struct.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vplanet.h"

void FinalizeUpdateNULL(BODY *body,UPDATE *update,int *iEqn,int iVar,int iBody) {
  /* Nothing */
}

void VerifyRotationNULL(BODY *body,CONTROL *control,OPTIONS *options,char cFile[],int iBody) {
  /* Nothing */
}

double fdReturnOutputZero(BODY *body,SYSTEM *system,UPDATE *update,int iBody,int iBody1) {
  return 0;
}

void InitializeModule(MODULE *module,int iNumBodies) {
  int iBody;

  module->iNumModules = malloc(iNumBodies*sizeof(int));
  module->iaModule = malloc(iNumBodies*sizeof(int*));  

  // Function pointer vectors
  module->fnInitializeUpdate = malloc(iNumBodies*sizeof(fnInitializeUpdateModule));
  module->fnInitializeOutput = malloc(iNumBodies*sizeof(fnInitializeOutputModule*));
  module->fnFinalizeOutputFunction = malloc(iNumBodies*sizeof(fnFinalizeOutputFunctionModule*));

  module->fnFinalizeUpdateEcc = malloc(iNumBodies*sizeof(fnFinalizeUpdateEccModule));
  module->fnFinalizeUpdateObl = malloc(iNumBodies*sizeof(fnFinalizeUpdateOblModule));
  module->fnFinalizeUpdateRot = malloc(iNumBodies*sizeof(fnFinalizeUpdateRotModule));
  module->fnFinalizeUpdateSemi = malloc(iNumBodies*sizeof(fnFinalizeUpdateSemiModule ));

  module->fnFinalizeUpdate40KNumMan = malloc(iNumBodies*sizeof(fnFinalizeUpdate40KNumManModule));
  module->fnFinalizeUpdate232ThNumMan = malloc(iNumBodies*sizeof(fnFinalizeUpdate232ThNumManModule));
  module->fnFinalizeUpdate238UNumMan = malloc(iNumBodies*sizeof(fnFinalizeUpdate238UNumManModule));
  module->fnFinalizeUpdate235UNumMan = malloc(iNumBodies*sizeof(fnFinalizeUpdate235UNumManModule)); 

  module->fnFinalizeUpdate40KNumCore = malloc(iNumBodies*sizeof(fnFinalizeUpdate40KNumCoreModule));
  module->fnFinalizeUpdate232ThNumCore = malloc(iNumBodies*sizeof(fnFinalizeUpdate232ThNumCoreModule));
  module->fnFinalizeUpdate238UNumCore = malloc(iNumBodies*sizeof(fnFinalizeUpdate238UNumCoreModule));
  module->fnFinalizeUpdate235UNumCore = malloc(iNumBodies*sizeof(fnFinalizeUpdate235UNumCoreModule));

  module->fnFinalizeUpdateTMan = malloc(iNumBodies*sizeof(fnFinalizeUpdateTManModule));
  module->fnFinalizeUpdateTCore = malloc(iNumBodies*sizeof(fnFinalizeUpdateTCoreModule));
  
  // Function Pointer Matrices
  module->fnLogBody = malloc(iNumBodies*sizeof(fnLogBodyModule*));
  module->fnInitializeBody = malloc(iNumBodies*sizeof(fnInitializeBodyModule*));
  module->fnInitializeControl = malloc(iNumBodies*sizeof(fnInitializeControlModule*));
  module->fnInitializeUpdateTmpBody = malloc(iNumBodies*sizeof(fnInitializeUpdateTmpBodyModule*));
  module->fnCountHalts = malloc(iNumBodies*sizeof(fnCountHaltsModule*));
  module->fnReadOptions = malloc(iNumBodies*sizeof(fnReadOptionsModule*));
  module->fnVerify = malloc(iNumBodies*sizeof(fnVerifyModule*));
  module->fnVerifyHalt = malloc(iNumBodies*sizeof(fnVerifyHaltModule*));
  module->fnVerifyRotation = malloc(iNumBodies*sizeof(fnVerifyRotationModule*));

  /* Assume no modules per body to start */
  for (iBody=0;iBody<iNumBodies;iBody++) 
    module->iNumModules[iBody]=0;
}

void FinalizeModule(BODY *body,MODULE *module,int iBody) {
  int iModule=0,iNumModules = 0;

  /************************
   * ADD NEW MODULES HERE *
   ************************/

  if (body[iBody].bEqtide)
    iNumModules++;
  if (body[iBody].bRadheat)
    iNumModules++;
  if (body[iBody].bThermint)
    iNumModules++;

  module->iNumModules[iBody] = iNumModules;
  module->iaModule[iBody] = malloc(iNumModules*sizeof(int));

  module->fnLogBody[iBody] = malloc(iNumModules*sizeof(fnLogBodyModule));
  module->fnInitializeControl[iBody] = malloc(iNumModules*sizeof(fnInitializeControlModule));
  module->fnInitializeOutput[iBody] = malloc(iNumModules*sizeof(fnInitializeOutputModule));
  module->fnInitializeUpdateTmpBody[iBody] = malloc(iNumModules*sizeof(fnInitializeUpdateTmpBodyModule));

  module->fnFinalizeOutputFunction[iBody] = malloc(iNumModules*sizeof(fnInitializeOutputFunctionModule));

  module->fnCountHalts[iBody] = malloc(iNumModules*sizeof(fnCountHaltsModule));
  module->fnReadOptions[iBody] = malloc(iNumModules*sizeof(fnReadOptionsModule));
  module->fnVerify[iBody] = malloc(iNumModules*sizeof(fnVerifyModule));

  module->fnVerify[iBody] = malloc(iNumModules*sizeof(fnVerifyModule));
  module->fnVerifyHalt[iBody] = malloc(iNumModules*sizeof(fnVerifyHaltModule));
  module->fnVerifyRotation[iBody] = malloc(iNumModules*sizeof(fnVerifyRotationModule));

  module->fnInitializeBody[iBody] = malloc(iNumModules*sizeof(fnInitializeBodyModule));
  module->fnInitializeUpdate[iBody] = malloc(iNumModules*sizeof(fnInitializeUpdateModule));
  module->fnFinalizeUpdateEcc[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateEccModule));
  module->fnFinalizeUpdateObl[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateOblModule));
  module->fnFinalizeUpdateRot[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateRotModule));
  module->fnFinalizeUpdateSemi[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateSemiModule));

  module->fnFinalizeUpdate40KNumMan[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate40KNumManModule));
  module->fnFinalizeUpdate232ThNumMan[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate232ThNumManModule));
  module->fnFinalizeUpdate238UNumMan[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate238UNumManModule));
  module->fnFinalizeUpdate235UNumMan[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate235UNumManModule));  

  module->fnFinalizeUpdate40KNumCore[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate40KNumCoreModule));
  module->fnFinalizeUpdate232ThNumCore[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate232ThNumCoreModule));
  module->fnFinalizeUpdate238UNumCore[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate238UNumCoreModule));
  module->fnFinalizeUpdate235UNumCore[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdate235UNumCoreModule));

  module->fnFinalizeUpdateTMan[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateTManModule));
  module->fnFinalizeUpdateTCore[iBody] = malloc(iNumModules*sizeof(fnFinalizeUpdateTCoreModule));
  
  for(iModule = 0; iModule < iNumModules; iModule++) {
    module->fnFinalizeUpdateEcc[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateObl[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateRot[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateSemi[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate40KNumMan[iBody][iModule] = &FinalizeUpdateNULL;  //PED added man's.
    module->fnFinalizeUpdate232ThNumMan[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate238UNumMan[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate235UNumMan[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate40KNumCore[iBody][iModule] = &FinalizeUpdateNULL;  //PED added core's.
    module->fnFinalizeUpdate232ThNumCore[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate238UNumCore[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdate235UNumCore[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateTMan[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnFinalizeUpdateTCore[iBody][iModule] = &FinalizeUpdateNULL;
    module->fnVerifyRotation[iBody][iModule] = &VerifyRotationNULL;
    }

  /************************
   * ADD NEW MODULES HERE *
   ************************/

  iModule = 0;
  if (body[iBody].bEqtide) {
    AddModuleEqtide(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = EQTIDE;
  }
  if (body[iBody].bRadheat) {
    AddModuleRadheat(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = RADHEAT;
  }
  if (body[iBody].bThermint) {
      AddModuleThermint(module,iBody,iModule);
    module->iaModule[iBody][iModule++] = THERMINT;
  }
}

void ReadModules(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,int iFile){
  int iNumIndices=0,iNumLines=0,iModule;
  int *lTmp;
  char saTmp[MAXARRAY][OPTLEN];

  lTmp=malloc(MAXLINES*sizeof(int));

  AddOptionStringArray(files->Infile[iFile].cIn,options->cName,saTmp,&iNumIndices,&iNumLines,lTmp,control->Io.iVerbose);
  if (lTmp[0] >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp[0],control->Io.iVerbose);
    if (iNumIndices == 0) {
      if (control->Io.iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: No modules input to option %s.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp[0]);
    }

    for (iModule=0;iModule<iNumIndices;iModule++) {

      /************************
       * ADD NEW MODULES HERE *
       ************************/

      if (memcmp(sLower(saTmp[iModule]),"eqtide",6) == 0) {
	body[iFile-1].bEqtide = 1;
      } else if (memcmp(sLower(saTmp[iModule]),"radheat",7) == 0) {
	body[iFile-1].bRadheat = 1;
      } else if (memcmp(sLower(saTmp[iModule]),"thermint",8) == 0) {
	body[iFile-1].bThermint = 1;
      } else {
	if (control->Io.iVerbose >= VERBERR)
	  fprintf(stderr,"ERROR: Unknown Module %s provided to %s.\n",saTmp[iModule],options->cName);
	LineExit(files->Infile[iFile].cIn,lTmp[0]);
      }
    }
    UpdateFoundOptionMulti(&files->Infile[iFile],options,lTmp,iNumLines,0);
  } else {
    if (control->Io.iVerbose >= VERBERR && iFile > 0) 
      fprintf(stderr,"WARNING: %s not present in file %s. No evolution will occur for this body.\n",options->cName,files->Infile[iFile].cIn);
  }

  free(lTmp);
}


/*
 * Verify multi-module dependencies
 */

void VerifyModuleMultiRadheatThermint(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,int iBody,int *iModule) {

  /* This will need modification if material can move between layers */

  if (body[iBody].bThermint) {
    if (!body[iBody].bRadheat) {
      if (control->Io.iVerbose > VERBINPUT)
	fprintf(stderr,"WARNING: Module THERMINT selected for %s, but RADHEAT not selected.\n",body[iBody].cName);
      body[iBody].dPowRadiogCore = 0;
      body[iBody].dPowRadiogMan = 0;
    } else
      control->Evolve.fnAuxPropsMulti[iBody][(*iModule)++] = &PropertiesRadheatThermint;
  }
}

void VerifyModuleMulti(BODY *body,CONTROL *control,FILES *files,MODULE *module,OPTIONS *options,int iBody) {
  int iModule=0;

  if (module->iNumModules[iBody] > 1) {
    /* XXX Note that the number of elements here is really a permutation, 
       but this should work for a while. */
    control->Evolve.fnAuxPropsMulti[iBody] = malloc(2*module->iNumModules[iBody]*sizeof(fnAuxPropsModule*));

    // Now verify 
    VerifyModuleMultiRadheatThermint(body,control,files,options,iBody,&iModule);
  }

  control->Evolve.iNumMulti[iBody] = iModule;
  if (control->Io.iVerbose >= VERBALL)
    fprintf(stdout,"All of %s's modules verified.\n",body[iBody].cName);
}

/*
 * Auxiliary Properties for multi-module calculations
 */

void PropertiesRadheatThermint(BODY *body,UPDATE *update,int iBody) {
  body[iBody].dPowRadiogCore = fdRadPowerCore(body,update,iBody);
  body[iBody].dPowRadiogMan = fdRadPowerMan(body,update,iBody);
}
